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
   ~pidMonitor() {}

    void add(pid_t p,const atom &a) 
    {                                                   
        L("[", p, "] add\n\ttimeout: ", a.timeout, "\n\tto_cmd : ", a.timeout_cmd, "\n\tne_cmd : ", a.noexist_cmd);
        atoms[p] = a;
    }  
    void del(pid_t p)               
    {
        L("[", p,"] delete");
        if(check(p)) atoms.erase(p);
    }
    bool check(pid_t p)             {return atoms.find(p) != atoms.end();}
    bool exist(pid_t p)             
    {
       return (kill(p, 0) == 0); 
    }   
    void command(std::string cmd, pid_t p)
    {
        replace<std::string>(cmd,"%PID",std::to_string(p));
        L("[",p,"] call command : '", cmd,"'");
        if(!system(cmd.c_str())) L("bad result for call command : ", cmd);
    }
    void touch(pid_t p)
    {
        if(check(p)) atoms[p].timestamp = atoms[p].timeout;
        else
            L("err: touch not exist pid: ", p);
    }
    
    void update(float dt)
    {
        auto i = atoms.begin();
        while(i != atoms.end())
        {
            if(exist(i->first))
            {
                i->second.timestamp-=dt;
                if(i->second.timestamp < 0)
                {
                      L("[", i->first, "] timeout");
                      command(i->second.timeout_cmd, i->first);
                }
                i++;    
            }
            else
            {
                L("[", i->first, "] not exist, kill from table");
                command(i->second.noexist_cmd, i->first);
                i = atoms.erase(i);
            }   
        }   

    }


private:
    std::unordered_map<pid_t, atom> atoms;


};




#endif