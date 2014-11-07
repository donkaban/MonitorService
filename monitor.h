#ifndef _MONITOR_H_
#define _MONITOR_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string>
#include <unordered_map>
#include <chrono>
#include <signal.h>
#include <thread>
#include <mutex>

#include "logger.h"

struct atom
{
    float       timeout   = 0;
    float       timestamp = 0;
    std::string timeout_cmd   = "";
    std::string noexist_cmd   = "";
    atom() {}
    atom(float to, const std::string & to_cmd, const std::string & ne_cmd) : 
        timeout(to),
        timestamp(to),
        timeout_cmd(to_cmd),
        noexist_cmd(ne_cmd)
    {}    
};

#define LOGFILE "monitor.log"


static logger L(LOGFILE);

class pidMonitor
{
public:
    pidMonitor()  {}
   ~pidMonitor()  {}

    void add(pid_t p,const atom &a) 
    {                                                   
        std::lock_guard<std::mutex> lock(locker);
        L("[", p, "]\t add timeout: ", a.timeout, "; to_cmd : '", a.timeout_cmd, "'; ne_cmd : '", a.noexist_cmd, "'");
        atoms[p] = a;
    }  
    void del(pid_t p)               
    {
        std::lock_guard<std::mutex> lock(locker);
        L("[", p,"]\t delete");
        if(check(p)) atoms.erase(p);
    }
    bool check(pid_t p)  {return atoms.find(p) != atoms.end();}
    bool exist(pid_t p)  {return (kill(p, 0) == 0); } 

    void command(std::string cmd, pid_t p)
    {
        replace<std::string>(cmd,"%PID",std::to_string(p));
        L("[",p,"]\t call command : '", cmd,"'");
        auto result = system(cmd.c_str());
        L("[",p,"]\t result : '", result,"'");
    }
    void touch(pid_t p)
    {
        std::lock_guard<std::mutex> lock(locker);
        if(check(p)) 
        {    
            L("[",p,"]\t touch" );
            atoms[p].timestamp = atoms[p].timeout;
        }
        else
              L("[",p,"]\t bad touch, not exist");
    }
    std::thread & run(int sleeptime = 500)
    {
        if(working) 
        {
            L("[!] try run running monitoring");
            return thrUpdate;
        }
        working = true;
        thrUpdate = std::thread([&]
        {
            std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
            std::chrono::time_point<std::chrono::system_clock> current;
            std::chrono::milliseconds dura(sleeptime);
            while(working)
            {
                locker.lock();
                    current = std::chrono::system_clock::now();
                    std::chrono::duration<float> dt(current - start);
                    update(dt.count());
                    start = current;
                locker.unlock();
                std::this_thread::sleep_for(dura);
            }    
        });
        return thrUpdate;
    } 

    void stop()
    {
        working = false;
    }

private:
    std::mutex  locker;
    bool working = false;
    std::unordered_map<pid_t, atom> atoms;
    std::thread thrUpdate;
    void update(float dt)
    {
        auto i = atoms.begin();
        while(i != atoms.end())
        {
            if(exist(i->first))
            {
                i->second.timestamp-=dt;
                std::cout << "timestamp : " << i->second.timestamp << std::endl;
                if(i->second.timestamp < 0)
                {
                    L("[", i->first, "]\t timeout, kill from table");
                    command(i->second.timeout_cmd, i->first);
                    i = atoms.erase(i);

                }
                else
                    ++i;
            }
            else
            {
                L("[", i->first, "]\t not exist, kill from table");
                command(i->second.noexist_cmd, i->first);
                i = atoms.erase(i);
            }   
        }   

    }

};




#endif