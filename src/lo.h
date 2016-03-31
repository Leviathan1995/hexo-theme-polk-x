//
// Created by leviathan on 16/3/24.
//

#ifndef LO_H_
#define LO_H_

#include <iostream>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "http.h"
#include "pthread_pool.h"

namespace lo
{
    const int MAX_EVENTS=1024  ;  // epoll listen events
    const int MAX_BACKLOG=100  ;
    const int MAX_CONNECTS=3 ;
    const int MAX_BUFFSIZE=1024*1024  ;

    extern epoll_event epoll_events[];
    extern epoll_event ev;


    // web server
    class lo_server
    {
    public:
        lo_server():
                listen_fd(-1),connect_fd(-1),errorno(-1),epoll_fd(-1),pservent(new servent),pool(MAX_CONNECTS){};
        ~lo_server(){if(pservent!=NULL) delete pservent;};

        //start web server
        void start();
        //init web server
        void init();

        //system package function
        int lo_socket(int family,int type,int protocol);
        void lo_listen(int socket_fd,int backlog);
        void lo_bind(int socket_fd,const struct sockaddr * addr,socklen_t addrlen);
        void lo_set_nonblocking(int socket_fd);
        void lo_set_reuseaddr(int socket_fd);
        int lo_accept(int socket_fd,struct sockaddr * addr, socklen_t * addrlen);
        int lo_epoll_create(int events);
        void lo_epoll_ctl(int epoll_fd,int op,int listen_fd,epoll_event * events);
        int lo_epoll_wait(int epoll_fd,epoll_event *events,int maxevents,int timeout);

    private:
        int listen_fd;
        int connect_fd;
        int errorno;
        int epoll_fd;
        struct servent * pservent;
        sockaddr_in server_addr;
        sockaddr_in client_addr;
        socklen_t addrlen;

        pthread_pool pool;  //pthread pool

        // No copying allowed
        lo_server(const lo_server &);
        lo_server & operator=(const lo_server &);

    };

    //error msg
    class error
    {
    public:
        error(){}
        error(std::string module_,std::string function_,std::string msg_):
                module(module_),function(function_),msg(msg_){}
        friend std::ostream & operator << (std::ostream & output,const error & e);
    private:
        std::string module;
        std::string function;
        std::string msg;

    };

} //namespace lo

#endif
