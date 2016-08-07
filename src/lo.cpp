//
// Created by leviathan on 16/3/24.
//

#include "lo.h"

namespace lo {
    //
    //global variable
    //


    const int MAX_EVENTS = 1024;
    int MAX_BACKLOG = 100;
    int MAX_CONNECTS = 1;
    int MAX_BUFFSIZE = 1024 * 1024;

    struct kevent triggered_event[MAX_EVENTS]; //event that was triggered
    struct kevent monitor_event[MAX_EVENTS];


    std::map<std::string,std::string> LoServer::config_;
    //
    //web server module
    //
    void LoServer::Start() {
        this->Init();
        this->Work();
    }

    void LoServer::Init() {

        //parsing config
        std::string line;
        std::ifstream file("/Users/hashdata/Code/Django1995/Lo/lo.config");
        std::regex re("(.*)=(.*)");
        while(std::getline(file,line)){
            std::smatch sm;
            std::regex_match(line,sm,re);
            config_[sm[1]]=sm[2];
        }

        listen_fd_ = Socket(AF_INET, SOCK_STREAM, 0);
        SetNonblocking(listen_fd_);  //set a socket as Nonblocking
        SetReuseaddr(listen_fd_);    //set a socket as SO_REUSEADDR

        //init server address
        bzero(&server_addr_, sizeof(server_addr_));
        server_addr_.sin_family = AF_INET;
        server_addr_.sin_port = htons(8080);
        server_addr_.sin_addr.s_addr = inet_addr("127.0.0.1");

        //bind socket with server address
        Bind(listen_fd_, (struct sockaddr *) &server_addr_, sizeof(server_addr_));
        Listen(listen_fd_, MAX_BACKLOG);

        ioevent_fd_ = KqueueCreate();                                               //create listen kqueue fd

        struct kevent ev;
        EV_SET(&ev, listen_fd_, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);          //initialize kevent structure
        int n = kevent(ioevent_fd_,&ev, 1, NULL, 0, NULL);
        FATALIF(n,"LoServer","Init()","Kqueue modify failed.");

        pool_.CreatePool();     //create thread pool
        pool_.SetIoEventFd(ioevent_fd_);

    }


    void LoServer::Work() {
        //event loop
        for (;;) {
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
            nev_= kevent(ioevent_fd_, NULL, 0, triggered_event, MAX_EVENTS, NULL);

            if (nev_ < 0) {
                    ERROR("No Event Triggering");
            }
            else if (nev_ > 0) {
                for (int i = 0; i < nev_; i++) {
                    if (triggered_event[i].flags & EV_ERROR)    //error
                    {
                        ERROR("kevent() Error.");
                        break;
                    }
                    else if (triggered_event[i].ident == listen_fd_)    //connect comming
                    {
                        connect_fd_ = Accept(listen_fd_, (struct sockaddr *) &client_addr_, &addrlen_);
                        INFO("Establish  Connection Success.")
                        SetNonblocking(connect_fd_);

                        struct kevent ev;
                        EV_SET(&ev, connect_fd_, EVFILT_READ, EV_ADD | EV_ENABLE | EV_ONESHOT, 0, 0, 0);
                        int n = kevent(ioevent_fd_,&ev, 1, NULL, 0, NULL);
                        FATALIF(n,"Kqueue modify failed.");

                    }
                    else if (triggered_event[i].flags & EVFILT_READ)    //data comming
                    {
                        INFO("Data Comming.");
                        pool_.AddTask(int(triggered_event[i].ident));
                    }
                }
            }
        }
    }
    //
    //system  function
    //

    int LoServer::Socket(int family, int type, int protocol) {
        int fd = socket(family, type, protocol);
        if (fd == -1) {
            ERROR("LoServer", "Socket()", "Create Socket File Descriptor Failed.");
            exit(-1);
        }
        else
            return fd;
    }

    void LoServer::SetNonblocking(int socket_fd) {
        int flags;
        if ((flags = fcntl(socket_fd, F_GETFL, 0)) < 0) {
            ERROR("LoServer", "SetNonblocking()", "F_GETFL Error");
            exit(-1);
        }
        flags |= O_NONBLOCK;
        if (fcntl(socket_fd, F_SETFL, flags) < 0) {
            ERROR("LoServer", "SetNonblocking()", "F_SETFL Error");
            exit(-1);
        }
    }

    void LoServer::SetReuseaddr(int socket_fd) {
        int on = 1, ret;
        if ((ret = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) == -1) {
            ERROR("LoServer", "SerReuseaddr()", "SO_REUSEADDR Error");
            exit(-1);
        }
    }

    void LoServer::Listen(int listen_fd, int backlog) {
        if (listen(listen_fd, backlog) == -1) {
            ERROR("LoServer", "Listen()", "Socket Listen Failed.");
            exit(-1);
        }
    }

    void LoServer::Bind(int socket_fd, const struct sockaddr *addr, socklen_t addrlen) {
        if (bind(socket_fd, addr, addrlen) == -1) {
            ERROR("LoServer", "Bind()", "Socket Bind Failed.");
            exit(-1);
        }
    }

    int LoServer::Accept(int socket_fd, struct sockaddr *addr, socklen_t *addrlen) {
        int conn_fd = 0;
        for (;;) {
            conn_fd = accept(socket_fd, addr, addrlen);
            if (conn_fd < 0) {
                if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                    break;
                }
                else {
                    ERROR("LoServer", "Accept()", "Accept Socket Failed.");
                    break;
                }
            }
            return conn_fd;
        }
        return -1;
    }

    //kqueue function
    int LoServer::KqueueCreate() {
        int kqueue_fd = kqueue();
        if (kqueue_fd == -1) {
            ERROR("LoServer", "KqueueCreate()", "Create Kqueue Fd Failed.");
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


}


