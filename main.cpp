#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <iostream>
#include "pidMonitor.h"
#include "rawMonitor.h"

#define PIDFILE "monitor.pid"
#define LOGFILE "monitor.log"

static logger L(LOGFILE);

int main() 
{
  
    pidMonitor pid_monitor(&L);
    rawMonitor raw_monitor(&L);

    pid_t pid;

    switch(pid = fork()) 
    {
        case -1:
            L.fatal("[error!]\t can't create fork");    
        case  0:
        {
            L.print("[",getpid(),"]\t create child process");
            umask(0);
            pid_t sid = setsid();
            if(sid < 0)  L.fatal("[error!]\t can't create sid");   
            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);

            if(open("/dev/null",O_RDONLY) == -1) L.fatal("[error!]\t can't touch /dev/null"); 

            pid_monitor.add(getpid(),{5.f,"kill -9 %PID",""});
            pid_monitor.add(9876,{5.f,"","echo delete %PID"});
            pid_monitor.run(100).join();
        }
        default:
        {
             L.print("[",getpid(),"]\t done");
        }


    }
    return EXIT_SUCCESS;
}
