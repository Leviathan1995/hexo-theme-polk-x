//
// Created by leviathan on 3/24/16.
//

#include <search.h>
#include "lo.h"

namespace lo
{
    //
    //global variable
    //
    
    pthread_mutex_t flage_mutex=PTHREAD_MUTEX_INITIALIZER;

    int MAX_EVENTS=1024;  // epoll listen events
    int MAX_BACKLOG=100;
    int MAX_CONNECTS=1;
    int MAX_BUFFSIZE=1024*1024  ;
    
    struct kevent monitor_event[9999];   //event we want to monitor
    struct kevent triggered_event[9999]; //event that was triggered
    
    int flags[9999]={0};
    
    std::string root="/Users/leviathan/lo_root";
    
    std::map<int,int> id_fd;
    
    //
    //global funciton
    //

    int find_free()
    {
        for(int i=0;i<9999;i++)
        {
            if (flags[i]==0) {
                return i;
            }
        }
        return -1;
    }
    
    int find_max()
    {
        auto end=id_fd.rbegin();
        int max=end->first;
        return max+1;
    }

    //
    //web server module
    //
    void lo_server::start()
    {
        this->init();
    }
    
    void lo_server::init()
    {
        listen_fd=lo_socket(AF_INET,SOCK_STREAM,0);
        lo_set_nonblocking(listen_fd); //set a socket as nonblocking
        lo_set_reuseaddr(listen_fd); //set a socket as SO_REUSEADDR
        
        //init server addr
        bzero(&server_addr,sizeof(server_addr));
        server_addr.sin_family=AF_INET;
        server_addr.sin_port=htons(8080);
        server_addr.sin_addr.s_addr=inet_addr("127.0.0.1");
        
        //bind socket with server_addr
        lo_bind(listen_fd, (struct sockaddr*)&server_addr, sizeof(server_addr));
        lo_listen(listen_fd,MAX_BACKLOG);
        
        kq_fd=lo_kqueue_create(); //create listen kqueue fd

        EV_SET(&monitor_event[0], listen_fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0); //initalise kevent structure
        flags[0]=1;
        id_fd[0]=listen_fd;
        
        pool.create_pool();//create pthread pool
        pool.set_kq_fd(kq_fd);
        
        //loop event
        for(;;)
        {
            /*
            int nfds=lo_epoll_wait(epoll_fd,epoll_events,MAX_EVENTS,-1);
            if(nfds==-1)
                continue;
            for(int i=0;i!=nfds;++i)
            {
                if ((epoll_events[i].events&EPOLLERR)||
                    (epoll_events[i].events&EPOLLHUP)||
                    (!(epoll_events[i].events&EPOLLIN)))  //error
                {
                    std::cout << error("lo_server","epoll wait","epoll error") << std::endl;
                    exit(-1);
                }
                
                else if(epoll_events[i].data.fd==listen_fd) //connect socket
                {
                    connect_fd=lo_accept(listen_fd,(struct sockaddr*)&client_addr, &addrlen);
                    std::cout<<"connect fd: "<<connect_fd<<std::endl;
                    lo_set_nonblocking(connect_fd);
                    ev.data.fd=connect_fd;
                    ev.events=EPOLLIN|EPOLLET|EPOLLONESHOT;
                    epoll_ctl(epoll_fd,EPOLL_CTL_ADD,connect_fd,&ev);
                }
                
                else if(epoll_events[i].events&EPOLLIN) //data on the fd waiting to be read
                {
                    std::cout<<"data comming fd: "<<ev.data.fd<<std::endl;
                    pool.add_task(ev.data.fd);
                }
            }
            */
            int max=find_max();
            pthread_mutex_lock(&flage_mutex);
            nev=kevent(kq_fd, monitor_event, find_max(), triggered_event, 9999, NULL);
            pthread_mutex_unlock(&flage_mutex);
            
            if (nev<0)
            {
                
            }
            
            else if(nev>0)
            {
                for(int i=0;i<nev;i++)
                {
                    if (triggered_event[i].flags & EV_ERROR)    //error
                    {
                        
                        std::cout <<error("lo_server","kevent","kqueue error") << std::endl;
                        break;
                        //exit(-1);
                    }
                    else if (triggered_event[i].ident==listen_fd)    //connect comming
                    {
                        connect_fd=lo_accept(listen_fd,(struct sockaddr*)&client_addr, &addrlen);
                        std::cout<<"connect fd: "<<connect_fd<<std::endl;
                        lo_set_nonblocking(connect_fd);
                        //
                        pthread_mutex_lock(&flage_mutex);
                        EV_SET(&monitor_event[find_free()], connect_fd, EVFILT_READ, EV_ADD | EV_ENABLE | EV_ONESHOT, 0, 0, 0);
                        id_fd[find_free()]=connect_fd;
                        flags[find_free()]=1;
                        pthread_mutex_unlock(&flage_mutex);

                    }
                    else if(triggered_event[i].flags & EVFILT_READ)    //data comming
                    {
                        std::cout<<"data comming fd: "<<triggered_event[i].ident<<std::endl;
                        pool.add_task(int(triggered_event[i].ident));
                        
                        pthread_mutex_lock(&flage_mutex);
                        flags[id_fd.find(int(triggered_event[i].ident))->first]=0;
                        id_fd.erase(id_fd.find(int(triggered_event[i].ident))->first);
                        pthread_mutex_unlock(&flage_mutex);

                    }
                }
            }
            
        }
        
    }
    
    //
    //system  function
    //
    int lo_server::lo_socket(int family,int type,int protocol)
    {
        int fd=socket(family,type,protocol);
        if(fd==-1)
        {
            std::cout<<error("lo_server","lo_socket","create sockfd failed!")<<std::endl;
            exit(-1);
        }
        else
            return fd;
    }
    
    void lo_server::lo_set_nonblocking(int socket_fd)
    {
        int flags;
        if((flags=fcntl(socket_fd,F_GETFL,0))<0)
        {
            std::cout<<error("lo_server","lo_set_nonblocking","F_GETFL error")<<std::endl;
            exit(-1);
        }
        flags|=O_NONBLOCK;
        if (fcntl(socket_fd,F_SETFL,flags)<0)
        {
            std::cout<<error("lo_server","lo_set_nonblocking","F_SETFL error")<<std::endl;
            exit(-1);
        }
    }
    
    void lo_server::lo_set_reuseaddr(int socket_fd)
    {
        int on=1,ret;
        if((ret=setsockopt(socket_fd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)))==-1)
        {
            std::cout << error("lo_server","lo_set_reuseaddr","SO_REUSEADDR error") << std::endl;
            exit(-1);
        }
    }
    
    void lo_server::lo_listen(int listen_fd,int backlog)
    {
        if(listen(listen_fd,backlog)==-1)
        {
            std::cout << error("lo_server","lo_listen","socket listen failed!") << std::endl;
            exit(-1);
        }
    }
    
    void lo_server::lo_bind(int socket_fd,const struct sockaddr * addr,socklen_t addrlen)
    {
        if(bind(socket_fd,addr,addrlen)==-1)
        {
            std::cout << error("lo_server","lo_bind","socket bind failed!") << std::endl;
            exit(-1);
        }
    }
    
    int lo_server::lo_accept(int socket_fd, struct sockaddr *addr, socklen_t *addrlen)
    {
        int conn_fd=0;
        for(;;)
        {
            conn_fd=accept(socket_fd,addr,addrlen);
            if (conn_fd < 0)
            {
                if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
                {
                    break;
                }
                else
                {
                    std::cout<<error("lo_server","lo_accept","accept socket failed!");
                    break;
                }
            }
            return conn_fd;
        }
        return -1;
    }
    
    //kqueue function
    int lo_server::lo_kqueue_create()
    {
        int kqueue_fd=kqueue();
        if(kqueue_fd==-1)
        {
            std::cout<<error("lo_server","lo_kqueue_create","create kqueue fd failed!")<<std::endl;
            exit(-1);
        }
        return kqueue_fd;
    }
    
    /*
    void lo_server::lo_epoll_ctl(int epoll_fd,int op,int listen_fd,struct epoll_event * event)
    {
        
        epoll_event ev;
        ev.events=EPOLLIN;    //read
        ev.data.fd=listen_fd;
        if(epoll_ctl(epoll_fd, op, listen_fd, &ev)==-1)
        {
            std::cout << error("lo_server","lo_epoll_ctl","epoll ctl failed!") << std::endl;
            exit(-1);
        }
    }
    
    int lo_server::lo_epoll_wait(int epoll_fd,struct epoll_event *events, int maxevents, int timeout)
    {
        int nfds=epoll_wait(epoll_fd,events,maxevents,timeout);
        if (nfds==-1&&errno==EINVAL)
        {
            std::cout << errno<<" "<<error("lo_server","lo_epoll_wait","epoll wait failed!") << std::endl;
            
            exit(-1);
        }
        return nfds;
    }
    */
    
    
    //
    //error msg module
    //
    std::ostream & operator << (std::ostream & output,const error & e)
    {
        output<<e.module<<" "<<e.function<<" "<<e.msg<<std::endl;
        return output;
    }
}


