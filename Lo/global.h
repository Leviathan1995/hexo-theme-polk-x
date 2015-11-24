//
//  global.h
//  Lo
//
//  Created by Leviathan on 15/11/22.
//  Copyright © 2015年 Leviathan. All rights reserved.
//

#ifndef global_h
#define global_h

#include <sys/epoll.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

#define MAX_BACKLOG 100 //监听队列最大数
#define MAX_EVENTS 1024 //epoll最大监听事件数

struct epollfd_connfd
{
    int epoll_fd;
    int conn_fd;
}
#endif /* global_h */
