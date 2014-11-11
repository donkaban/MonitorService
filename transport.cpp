// #include "transport.h"
// #include <unistd.h>
// #include <sys/types.h>
// #include <chrono>
// #include <algorithm>

// const theServer::hdl theServer::defCHdl = [](int id) {std::cout << "[theServer] default disconnect " <<  id;};
// const theServer::hdl theServer::defDHdl = [](int id) {std::cout << "[theServer] default connect " << id ;};

// const uint theServer::connectionsLimit  = 25;
// const uint theServer::listenerTimeout   = 10; 
// const uint theClient::connectTimeout    = 500; 


// theClient::theClient(strref node, strref port,const rcvHdl & rcv) : 
//     _srvAddr(node),
//     _srvPort(port),
//     _receiveHandler(rcv),
//     _disconnectHandler([&](strref info) {logger::error("[theClient] can't connect to %s reconnect ...", info.c_str());}),
//     _connectHandler([&](strref info)    {logger::system("[theClient] connected to %s ", info.c_str());})
 
// {
//     _connect();
//     _runRcvThread();
// }

// theClient::~theClient()
// {
//     shutdown(_socket, 2);
//     close(_socket);
//     _terminate = true;
//     if(_receiveThread.joinable()) _receiveThread.join();
// }

// void theClient::_connect()
// {
//     std::lock_guard<std::mutex> lock(_locker);
//     _connected = false;
//     while(true)
//     {
//         memset(&_server, 0,sizeof(_server));
//         _server.ai_family   = AF_UNSPEC;   
//         _server.ai_socktype = SOCK_STREAM;
//         int status = getaddrinfo(_srvAddr.c_str(), _srvPort.c_str(), &_server, &_host_list); 
//         if(status != 0) 
//             FATAL_ERROR("[theClient] %s",gai_strerror(status));
//         _socket = socket(_host_list->ai_family, _host_list->ai_socktype,_host_list->ai_protocol);
//         if(_socket < 0)
//             FATAL_ERROR("[theClient] can't create socket %s:%s",_srvAddr.c_str(),_srvPort.c_str());
//         if(connect(_socket,_host_list->ai_addr, _host_list->ai_addrlen) >= 0) 
//         {
//             _connected = true;
//             _connectHandler(std::string(util::format("%s:%s id:%d",_srvAddr.c_str(),_srvPort.c_str(),_socket)));
//             return;           
//         }    
        
//         freeaddrinfo(_host_list);
//         close(_socket);
//         _disconnectHandler(std::string(util::format("%s:%s id:%d",_srvAddr.c_str(),_srvPort.c_str(),_socket)));
//         std::this_thread::sleep_for(std::chrono::milliseconds(connectTimeout));        
//     }    
// }

// void theClient::_runRcvThread()
// {
//     _receiveThread = std::thread([&]()
//     { 
//         logger::system("[theClient] start recieve handler for id: %d",_socket);
      

//         while(!_terminate)
//         {
//             if(!_connected) {_connect();}
            
//             proto::header hdr;
//             std::string   msg;
            
//             auto check = recv(_socket,(char *)(&hdr),proto::hdr_size,0);
//             if(check == 0) {_connected = false; continue;}
//             if(check <  0) continue;
//             if(proto::hdr_magic != hdr.magic) {logger::error("[theClient] can't parse header, drop ..."); continue;}
            
//             switch(hdr.command)
//             {
//                 case proto::CMD_RETVAL : 
//                 case proto::CMD_PING   : 
//                 case proto::CMD_PONG   : 
//                 case proto::CMD_EVENT  : 
//                 {    
//                     msg.resize(hdr.message_size);
//                     auto check = recv(_socket,&msg[0],hdr.message_size,0);
//                     if(check == 0) {_connected = false; continue;}
//                     if(check <  0) {logger::error("[theClient] can't recieve message, drop ..."); continue;}
//                     _receiveHandler(thePacket(msg));
//                     break;

//                 }
//                     break;
//                 default :
//                     logger::error("[theClient] unknown command : %d", hdr.command);
//                     break;

//             }
//         }
//         logger::system("[theClient] recieve handle id: %d was complete",_socket);
//     });
// }

// void theClient::push(const thePacket &packet)
// {
//     proto::header header;
//     header.command      = proto::CMD_EVENT;
//     header.message_size = packet.getMessageSize();
//     bool pushed = false;
//     while(!pushed)
//     {    
//         if(send(_socket,(char *) &header, proto::hdr_size, 0) < 0)              
//         {
//             _locker.lock();
//                 _connected = false;
//             _locker.unlock();  
//             continue;
//         }
//        if(send(_socket, packet.getMessage(), packet.getMessageSize(),0) < 0)   
//         {
//             _locker.lock();
//                 _connected = false;
//             _locker.unlock();  
//             continue;
//         }
//        pushed = true; 
//     }
// }









// //////////////////////////////////////////////////////////////////////////////////////////////////////

// void theServer::invalidateClients()
// {
//     for(auto it = clients.begin(); it != clients.end();)
//     {
//         if(it->second.second) 
//             it++;
//         else
//         {    
//             it->second.first.join();
//             it=clients.erase(it);
//         }
//     }    
// }

// void theServer::connectClient(int id)
// {
//     std::lock_guard<std::mutex> lock(_locker);
//     invalidateClients();    
//     clients[id].first  = std::thread([id,this]()
//     {
//         while(clients[id].second)
//         {
//             proto::header header;
//             std::string   message;

//             auto size = recv(id,(char *)(& header),proto::hdr_size,0);
//             if(size == 0) {clients[id].second = false; continue;}
//             if(size <  0) {logger::error("[theServer] can't recieve header, drop ..."); continue;}
             
//             switch(header.command)   
//             {
//                 case proto::CMD_PING   : 
//                 case proto::CMD_PONG   : 
//                 case proto::CMD_RETVAL : 
//                 case proto::CMD_EVENT  : 
//                     message.resize(header.message_size);
//                     size = recv(id,&message[0],header.message_size,0);
//                     if(size == 0) {clients[id].second = false; continue;}
//                     if(size <  0) {logger::error("[theServer] can't recieve message, drop ..."); continue;}
//                     _locker.lock();
//                         _receiveHandler(id,thePacket(message));
//                     _locker.unlock();
//                     break;
//                 default :
//                     logger::error("[theServer] unknown command : %d", header.command);
//                     break;
//             }
//          //   std::this_thread::sleep_for(std::chrono::milliseconds(listenerTimeout));        
//         }  
//         _disconnectHandler(id);    
//         clients[id].second = false;
//     });
//     _connectHandler(id);
//     clients[id].second = true;
// }  


// theServer::theServer(strref port,const hdl &c, const hdl &dc) : 
//     _socket(-1),
//     _srvPort(port),
//     _disconnectHandler(c),
//     _connectHandler(dc)
// {
//      memset(&_host, 0,sizeof(_host));
//     _host.ai_family   = AF_UNSPEC;   
//     _host.ai_socktype = SOCK_STREAM;
//     _host.ai_flags    = AI_PASSIVE;
//     int status = getaddrinfo(NULL, _srvPort.c_str(), &_host, &_host_list); 
//     if(status != 0) 
//         FATAL_ERROR("[theServer] %s",gai_strerror(status));
//     _socket = socket(_host_list->ai_family, _host_list->ai_socktype,_host_list->ai_protocol);
//     if(_socket < 0) 
//         FATAL_ERROR("[theServer] can't create socket in port %s",_srvPort.c_str());
//     int yes = 1;
//     status = setsockopt(_socket,SOL_SOCKET,SO_REUSEADDR, &yes, sizeof(int));
//     if(bind(_socket, _host_list->ai_addr, _host_list->ai_addrlen) < 0)
//         FATAL_ERROR("[theServer] 'port %s already used",_srvPort.c_str());
// }

// void theServer::listener(const cbf & task)    
// {
//     logger::system("[theServer] start listen port: %s",_srvPort.c_str());
//     _receiveHandler = task;    
//     while(true)
//     {    
//         if(listen(_socket,connectionsLimit) < 0)
//             FATAL_ERROR("[theServer] can't listen port: %s",_srvPort.c_str());
//         sockaddr_storage clientAddr;
//         socklen_t addrSize = sizeof(clientAddr);
//         auto id = accept(_socket,(sockaddr *) &clientAddr, &addrSize);
//         if(id < 0) 
//             FATAL_ERROR("[theServer] listen port error");
//         else 
//             connectClient(id);
//     }
// }

// theServer::~theServer()
// {
//     freeaddrinfo(_host_list);
//     close(_socket);
// }

// void theServer::push(int id, const thePacket & packet)
// {
//     proto::header header;
//     header.command = proto::CMD_EVENT;
//     header.message_size = packet.getMessageSize();
//     if  ((send(id, &header, proto::hdr_size, 0) < 0) ||                   
//         (send(id, packet.getMessage(), packet.getMessageSize(),0) < 0)) 
//     {
//         clients[id].second = false; 
//         invalidateClients();
//     } 
// }

// void theServer::broadcast(const thePacket & packet)
// {
//     for(const auto & cl : clients) 
//         if(cl.second.second) push(cl.first, packet);
// }
