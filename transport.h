#ifndef _IPC_TRANSPORT_
#define _IPC_TRANSPORT_

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "packet.h"

namespace proto
{
    enum command
    {
        CMD_PING = 0,
        CMD_PONG,
        CMD_EVENT,
        CMD_RETVAL,
        CMD_ERROR
    };
    enum encode
    {
        ENCODE_XML = 0,
        ENCODE_JSON,
        ENCODE_BINARY
    };
    const uint8_t  hdr_magic = 42; 
    
    struct header
    {
        uint8_t  magic         = hdr_magic; 
        uint8_t  encode        = ENCODE_JSON;
        uint8_t  command       = CMD_ERROR;
        uint16_t message_size  = 0;    // byteorder!!!!
    };
    const size_t hdr_size  = sizeof(header);
   
}



class theClient
{
    using rcvHdl =  std::function<void (const thePacket &)>;
    using nfoHdl =  std::function<void(strref)>;
  
    static const uint connectTimeout; 

public:
    theClient (const theClient &)             = delete;
    theClient & operator= (const theClient&)  = delete;

    theClient(strref ,strref , const rcvHdl &);
    virtual ~theClient(); 

    void push(const thePacket &);
    
    inline void receiveHandler(const rcvHdl &f)    {_receiveHandler = f;} 
    inline void connectHandler(const nfoHdl &f)    {_connectHandler = f;} 
    inline void disconnectHandler(const nfoHdl &f) {_disconnectHandler = f;} 

private:
    int         _socket     = 0; 
    std::string _srvAddr    = "127.0.0.1"; 
    std::string _srvPort    = "9876";
    bool        _connected  = false;
    bool        _terminate  = false;
    addrinfo    _server;
    addrinfo *  _host_list;
    
    rcvHdl      _receiveHandler;
    std::thread _receiveThread;
    std::thread _connectThread;
    std::mutex  _locker;

    nfoHdl _disconnectHandler;
    nfoHdl _connectHandler;
    
    void _connect();
    void _runRcvThread();

};

class theServer
{
    using cbf   =  std::function<void (int, thePacket)>;
    using hdl   =  std::function<void(int)>;
    using thr_t =  std::pair<std::thread, bool>; 

    static const uint connectionsLimit;
    static const uint listenerTimeout; 

    const static hdl  defCHdl; 
    const static hdl  defDHdl;

private:
    std::mutex  _locker;

    int         _socket; 
    std::string _srvPort;
    addrinfo    _host;
    addrinfo *  _host_list;
 
    std::unordered_map<int, thr_t> clients;
   
    void connectClient(int);
    void invalidateClients();

    cbf _receiveHandler;
    hdl _disconnectHandler;
    hdl _connectHandler;

public:

    theServer(strref,const hdl & ch = defCHdl, const hdl & dc = defDHdl);
    virtual ~theServer();

    void listener(const cbf &);
    void push(int,const thePacket &);
    void broadcast(const thePacket &);
    
    inline void connectHandler(const hdl &f)    {_connectHandler = f;} 
    inline void disconnectHandler(const hdl &f) {_disconnectHandler = f;} 
    inline int  connections() const {return clients.size();}
  
};



#endif
