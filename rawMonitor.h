#ifndef _RAW_MONITOR_H_
#define _RAW_MONITOR_H_

#include <unistd.h>
#include "logger.h"

#define LOG L->print

class rawMonitor
{
public:
	 rawMonitor(logger * l) : L(l){LOG("[",getpid(),"]\t create RAW monitor");}
   	~rawMonitor()  {}

private:
	logger *L;

};

#endif