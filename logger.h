#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <sstream>
#include <fstream>
#include <iostream>

template <typename T>
void replace(T& src, const T& find, const T& replace)
{
    size_t find_lenght = find.size();
    size_t rep_lenght  = replace.size();
    for(size_t pos = 0; (pos=src.find(find, pos))!=T::npos; pos+=rep_lenght) 
        src.replace(pos, find_lenght, replace);
}


class logger
{
public:  
    logger(const std::string & logname)
    {
   	 	_file.open(logname, std::ios::app);
    }
   ~logger() 
   {
   		_file.close();
   } 
    template<typename ... T> void operator()(T && ... args)
    {
        _prn(args...);
        _file << timestamp() << " " << _str.str();
        _file.flush();
        _str.str("");
    }
   
 	template<typename ... T> void fatal(T && ... args)
    {
    	this(args...);
    	exit(-1);
    }

private:
    std::stringstream _str;
    std::ofstream 	  _file;
    char * timestamp()
    {
     	std::time_t t = std::time(NULL);
    	static char mbstr[100];
    	std::strftime(mbstr, sizeof(mbstr), "%b:%0d:%H:%M:%S", std::localtime(&t));
    	return mbstr;
        
    }
    void _prn() 
    {
        _str << std::endl;
    }
    template <typename T> void _prn(const T &t) 
    {
        _str << t << std::endl;
    }
    template <typename T, typename... A> void _prn(const T &head, const A&... tail) 
    {
        _str << head;
        _prn(tail...); 
    }
};





#endif