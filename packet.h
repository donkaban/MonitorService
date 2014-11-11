#ifndef _IPC_PACKET_
#define _IPC_PACKET_
#include <iostream>
template<int...>          struct seq {};
template<int N, int ...M> struct gen : gen<N-1, N-1, M...> { };
template<int ...N>        struct gen<0, N...> {typedef seq<N...> type;};

using strref = const std::string &;

template <typename ... A>
class packet
{
public: 
   
    using raw_t = const char *;
    using arg_t = std::tuple<A...>; 
    
    packet();
    packet(A && ... a) {pack(a...);}
    packet(strref m) : str(m)
    {
        unpack();
    }
    virtual ~packet();
  
    inline raw_t   get_msg()      const {return str.c_str();}
    inline size_t  get_msg_size() const {return str.size();} 
   
    template<typename T> T &  get(strref key)               {}    
    template<typename T> void set(strref key, const T &val) {}; 

private:
    arg_t        args;    
    std::string  str;
    void _p(int v)    {str.append(":int:" + std::to_string(v));}
    void _p(float v)  {str.append(":rel:" + std::to_string(v));}
    void _p(strref v) {str.append(":str:" + v);}

    void pack() {str.append(":::");}
    template <typename T> void pack(const T &t) { _p(t);}
    template <typename T, typename... Ta> void pack(const T &head, const Ta&... tail) { _p(head); pack(tail...);}

    void unpack();


};




#endif