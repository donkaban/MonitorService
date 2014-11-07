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
#define LOGFILE "monitor.log"


void pid_file(const std::string &filename)
{
    FILE* f;
    f = fopen(filename.c_str(), "w+");
    if(f)
    {
        fprintf(f, "%u", getpid());
        fclose(f);
    }
}



int main (int argc, char **argv) 
{

    L("try start demon"); 
   
    pidMonitor monitor;

    monitor.add(getpid(),{5.f,"kill -9 %PID","ls"});

    while(true)
        monitor.update(.01);


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
