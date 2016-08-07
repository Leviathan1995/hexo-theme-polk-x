//
// Created by leviathan on 16/3/24.
//

#ifndef LO_H_
#define LO_H_

#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/event.h>
#include <netdb.h>
#include <string.h>
#include <map>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <fstream>
#include <search.h>
#include <regex>


#include "http.h"
#include "thread_pool.h"
#include "log.h"

namespace lo
{
    //
    //global variable
    //

    extern const int MAX_EVENTS;          // model listen events
    extern int MAX_BACKLOG;
    extern int MAX_CONNECTS;
    extern int MAX_BUFFSIZE;
    
    extern struct kevent triggered_event[]; //event that was triggered
    extern struct kevent monitor_event[];
    //
    // http server
    //
    struct Config
    {
        std::string path;
    };
    class LoServer
    {
    public:
        LoServer(): listen_fd_(-1),connect_fd_(-1),nev_(-1),errorno_(-1),ioevent_fd_(-1),pool_(MAX_CONNECTS){};
        ~LoServer(){};
        
        //start web server
        void Start();
        //init web server
        void Init();
        void Work();
        
        //
        //system  function
        //
        int Socket(int family,int type,int protocol);
        void Listen(int socket_fd,int backlog);
        void Bind(int socket_fd,const struct sockaddr * addr,socklen_t addrlen);
        void SetNonblocking(int socket_fd);
        void SetReuseaddr(int socket_fd);
        int Accept(int socket_fd,struct sockaddr * addr, socklen_t * addrlen);

        //kqueue
        int KqueueCreate();
        
        /*
        void lo_epoll_ctl(int epoll_fd,int op,int listen_fd,epoll_event * events);
        int lo_epoll_wait(int epoll_fd,epoll_event *events,int maxevents,int timeout);
        */

        static std::string GetPath(){return config_.find("PATH")->second;}
        
    private:
        int listen_fd_;
        int connect_fd_;
        int errorno_;
        int ioevent_fd_;
        int nev_;
        sockaddr_in server_addr_;
        sockaddr_in client_addr_;
        socklen_t addrlen_;
        ThreadPool pool_;  //pthread pool

        static std::map<std::string,std::string> config_;
        
        // No copying allowed
        LoServer(const LoServer &);
        LoServer & operator=(const LoServer &);
        
    };

    
} //namespace lo

#endif
