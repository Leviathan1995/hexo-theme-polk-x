//
//  Main.cpp
//  Lo
//
//  Created by Leviathan on 15/11/22.
//  Copyright © 2015年 Leviathan. All rights reserved.
//


#include <iostream>
#include "global.h"
using namespace std;

int main()
{
    struct sockaddr_in  client_addr;         //客户端地址结构
    struct sockaddr_in  server_addr;         //服务器地址结构
    struct epoll_event  event;               //需要监听的事件类型
    struct epoll_event  events[MAX_EVENTS];  //事件集合
    socklen_t           addrlen;
    pthread_attr_t      pthread_attr;
    pthread_t           thread_id;
    
    int listen_fd,epoll_fd,conn_fd,nfds,on=1;
    
    //监听套接字
    listen_fd=socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on,sizeof(on));
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(80);
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    //绑定地址结构到套接字描述符
    bind(listen_fd,(struct sockaddr *)&server_addr,sizeof(server_addr));
    //设定侦听队列长度
    if(listen(listen_fd,MAX_BACKLOG)==-1)
        cout<<"listen error"<<endl;
    
    
    //创建epoll文件描述符
    epoll_fd=epoll_create();
    event.events=EPOLLIN;       //对应的文件描述符可读
    event.data.fd=listen_fd;
    epoll_ctl(epoll_fd,EPOLL_CTL_ADD,listen_fd,&event); //需要监听的事件注册
    
    pthread_attr_init(&pthread_attr);
    pthread_attr_setdetachstate(&pthread_attr, PTHREAD_CREATE_DETACHED);
    
    
    for(;;)
    {
        nfds=epoll_wait(epoll_fd,events,MAX_EVENTS,-1);
        if (nfds==-1) {
            continue;
        }
        for (int i=0; i!=nfds; ++i) {
            //如果有新的连接
            if (events[i].data.fd==listen_fd)
            {
                conn_fd=accept(listen_fd, (struct sockaddr*)&client_addr,&addrlen);
                //设置新的套接字为非阻塞模式
                
                //设置读事件和ET模式
                event.events=EPOLLIN|EPOLLET;
                event.data.fd=conn_fd;
                epoll_ctl(epoll_fd,EPOLL_CTL_ADD,conn_fd,&event);
            }
            else
            {
                
            }
        }
    }
    
    close(listen_fd); //关闭监听套接字
    return 0;
}
