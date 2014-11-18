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
#include "packet.h"

#define PIDFILE "monitor.pid"
#define LOGFILE "monitor.log"

static logger L(LOGFILE);


void test(pidMonitor & monitor)
{
    monitor.add(getpid(),{5.f,"kill -9 %PID",""});
    monitor.add(9876,{5.f,"","echo delete %PID"});
    monitor.run(100).join();
}


using pack1 = packet<int,int,int,int,int>;
using pack2 = packet<int,std::string,float,bool>;


int main() 
{
  
    pidMonitor pid_monitor(&L);
    rawMonitor raw_monitor(&L);

    pack1  p1(1,2,3,4,5);
    pack2  p2(42,"ololo",42.42, true);

    std::cout << "packet 1 -> " << p1.get_msg() << "\n";
    std::cout << "packet 2 -> " << p2.get_msg() << "\n";

    std::cout << "0: " << p2.get<0>() << " 1: "<< p2.get<1>()  << " 2: "<< p2.get<2>() << "\n";
    p2.get<1>() = "12345!!!!";
    std::cout << "0: " << p2.get<0>() << " 1: "<< p2.get<1>()  << " 2: "<< p2.get<2>() << "\n";



    // pid_t pid;

    // switch(pid = fork()) 
    // {
    //     case -1:
    //         L.fatal("[error!]\t can't create fork");    
    //     case  0:
    //     {
    //         L.print("[",getpid(),"]\t create and detach service process");
    //         umask(0);
    //         pid_t sid = setsid();
    //         if(sid < 0)  L.fatal("[error!]\t can't create sid");   
    //         close(STDIN_FILENO);
    //         close(STDOUT_FILENO);
    //         close(STDERR_FILENO);

    //         if(open("/dev/null",O_RDONLY) == -1) L.fatal("[error!]\t can't touch /dev/null"); 
    //         test(pid_monitor);
           
    //     }
    //     default:
    //     {
    //          L.print("[",getpid(),"]\t done");
    //     }


    // }
    return EXIT_SUCCESS;
}
