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

#include "http.h"
#include "pthread_pool.h"
#include "log.h"

namespace lo
{
    //
    //global variable
    //
    extern pthread_mutex_t flage_mutex;

    extern int MAX_EVENTS;  // epoll listen events
    extern int MAX_BACKLOG;
    extern int MAX_CONNECTS;
    extern int MAX_BUFFSIZE;
    
    extern struct kevent monitor_event[];   //event we want to monitor
    extern struct kevent triggered_event[]; //event that was triggered
    extern int flags[];
    extern std::map<int,int> id_fd;
    extern std::string root;
    
    //
    //global funciton
    //
    int find_free();
    int find_max();
    
    //
    // http server
    //
    class LoServer
    {
    public:
        LoServer():
        listen_fd(-1),connect_fd(-1),errorno(-1),kq_fd(-1),pool(MAX_CONNECTS){};
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
        
    private:
        int listen_fd;
        int connect_fd;
        int errorno;
        int kq_fd;
        int nev;
        sockaddr_in server_addr;
        sockaddr_in client_addr;
        socklen_t addrlen;
        ThreadPool pool;  //pthread pool
        
        // No copying allowed
        LoServer(const LoServer &);
        LoServer & operator=(const LoServer &);
        
    };

    
} //namespace lo

#endif
