#ifndef _PID_MONITOR_H_
#define _PID_MONITOR_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
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

#define LOG L->print

class pidMonitor
{
public:
    pidMonitor(logger * l) : L(l) {LOG("[",getpid(),"]\t create PID monitor");}
   ~pidMonitor()  {}

    void add(pid_t p,const atom &a) 
    {                                                   
        std::lock_guard<std::mutex> lock(locker);
        LOG("[", p, "]\t add; timeout: ", a.timeout, "; to_cmd : '", a.timeout_cmd, "'; ne_cmd : '", a.noexist_cmd, "'");
        atoms[p] = a;
    }  
    void del(pid_t p)               
    {
        std::lock_guard<std::mutex> lock(locker);
        LOG("[", p,"]\t delete");
        if(check(p)) atoms.erase(p);
    }
    bool check(pid_t p)  {return atoms.find(p) != atoms.end();}
    bool exist(pid_t p)  {return (kill(p, 0) == 0); } 

    void command(const std::string &tag,std::string cmd, pid_t p)
    {
        replace<std::string>(cmd,"%PID",std::to_string(p));
        LOG("[",p,"]\t ",tag,"; cmd: '", cmd,"'");
        if(system(cmd.c_str()) != 0)   
            LOG("[",p,"]\t ",tag,"; bad result from call");
    }
    void touch(pid_t p)
    {
        std::lock_guard<std::mutex> lock(locker);
        if(check(p)) 
        {    
           LOG("[",p,"]\t touch" );
            atoms[p].timestamp = atoms[p].timeout;
        }
        else
             LOG("[",p,"]\t bad touch, not exist");
    }
    std::thread & run(size_t sleeptime = 500)
    {
        LOG("[",getpid(),"]\t run PID monitor");
      
        if(working) 
        {
           LOG("[!] try run running PID monitor");
            return thrUpdate;
        }
        working = true;
        thrUpdate = std::thread([&]
        {
            std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();
            std::chrono::time_point<std::chrono::system_clock> current;
            std::chrono::milliseconds pause(sleeptime);
            while(working)
            {
                    current = std::chrono::system_clock::now();
                    std::chrono::duration<float> dt(current - start);
                    start = current;
                    update(dt.count());
                    std::this_thread::sleep_for(pause);
            }    
        });
        return thrUpdate;
    } 

    void stop()
    {
        working = false;
    }

private:
    logger * L;
    std::mutex  locker;
    bool working = false;
    std::unordered_map<pid_t, atom> atoms;
    std::thread thrUpdate;
   
    void update(float dt)
    {
        std::lock_guard<std::mutex> lock(locker);
        auto i = atoms.begin();
        while(i != atoms.end())
        {
            if(exist(i->first))
            {
                i->second.timestamp-=dt;
                if(i->second.timestamp < 0)
                {
                    command("what: timeout",i->second.timeout_cmd, i->first);
                    i = atoms.erase(i);

                }
                else
                    ++i;
            }
            else
            {
                command("what: not exist",i->second.noexist_cmd, i->first);
                i = atoms.erase(i);
            }   
        }   

    }

};




#endif