#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include <iostream>
#include "monitor.h"

#define PIDFILE "monitor.pid"

int main (int argc, char **argv) 
{
  
    pidMonitor monitor;

    monitor.add(getpid(),{5.f,"kill -9 %PID","ls"});
    monitor.add(9999,{5.f,"ls","ls"});
    
    monitor.run(100).detach();
    

    int i = 7777;
    while(true)
    {

        monitor.add(i++,{5.f,"ls","ls"});
        usleep(50000);
    }

    
       


    // pid_t pid = fork();
    // if(pid < 0) exit(EXIT_FAILURE);
    // if(!pid)
    // {
    //     umask(0);

    //     pid_t sid = setsid();
    //     if(sid < 0) exit(EXIT_FAILURE);
    //     close(STDIN_FILENO);
    //     close(STDOUT_FILENO);
    //     close(STDERR_FILENO);
        
    //     if(open("/dev/null",O_RDONLY) == -1) 
    //         exit(-1);;
        
    //     pid_file("pid.nfo"); 
    
     
    //     if ((chdir("/")) < 0) exit(EXIT_FAILURE);
        
    //     while(true)
    //     {
    //         process();    
    //         usleep(20000);    
            
    //     }
       
       
    // }
    // else
        return EXIT_SUCCESS;
}
