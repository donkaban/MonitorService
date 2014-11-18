#ifndef _IPC_PACKET_
#define _IPC_PACKET_

#include <iostream>

using strref = const std::string &;

template <typename ... A>
class packet
{
public: 
   
    using raw_t = const char *;
    using val_t = std::tuple<A...>; 

    packet();
    packet(A && ... a) : val(std::make_tuple(a...)) {pack(a...);}
    packet(strref m) : str(m) {unpack();}
   ~packet() {};
  
    raw_t   get_msg()      const {return str.c_str();}
    size_t  get_msg_size() const {return str.size();} 
    
    template <size_t N> 
    auto get() -> typename std::tuple_element<N, std::tuple<A...> >::type&  {return std::get<N>(val);}
       
private:
    val_t        val;
    std::string  str;
    
    void _p(int v)    {str.append(":i:" + std::to_string(v));}
    void _p(float v)  {str.append(":f:" + std::to_string(v));}
    void _p(double v) {str.append(":d:" + std::to_string(v));}
    void _p(bool v)   {str.append(":b:" + std::to_string(v));}
    void _p(strref v) {str.append(":s:" + v);}


    template <typename T> void pack(const T &t) { _p(t);}
    template <typename T, typename... Ta> void pack(const T &head, const Ta&... tail) { _p(head); pack(tail...);}

    void unpack();

};




#endif