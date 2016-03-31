//
// Created by leviathan on 3/26/16.
//
#include "pthread_pool.h"
#include "lo.h"

namespace lo
{

    std::queue<int > pthread_pool::task_list;
    int pthread_pool::epoll_fd=0;
    bool pthread_pool::shutdown= false;
    pthread_mutex_t pthread_pool::pthread_mutex=PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t pthread_pool::pthread_cond=PTHREAD_COND_INITIALIZER;

    pthread_pool::pthread_pool(int num)
    {
        pthread_num=num;
        pthread_id=new pthread_t[pthread_num];
        create_pool();
    }

    void pthread_pool::set_pthread_num(int num)
    {
        pthread_num=num;
    }

    void pthread_pool::set_epoll_fd(int fd)
    {
        epoll_fd=fd;
    }
    //create pthread pool
    void pthread_pool::create_pool()
    {
        for (int i = 0; i <pthread_num ; ++i)
        {
            pthread_create(&pthread_id[i], NULL, pthread_call, NULL);
        }
    }

    //add fd to task_list
    void pthread_pool::add_task(int fd)
    {
        pthread_mutex_lock(&pthread_mutex);
        task_list.push(fd);
        pthread_mutex_unlock(&pthread_mutex);
        pthread_cond_signal(&pthread_cond);
    }

    void * pthread_pool::pthread_call(void *data)
    {
        pthread_t self_pid=pthread_self();
        while(1)
        {
            pthread_mutex_lock(&pthread_mutex);
            while(task_list.size()==0)
              pthread_cond_wait(&pthread_cond,&pthread_mutex);

            int  fd=task_list.front();
            task_list.pop();
            pthread_mutex_unlock(&pthread_mutex);

            socket_handle(fd);
        }
    }



    void pthread_pool::socket_handle(int fd)
    {
        int n=0,nread=0;
        char * buff=new char[MAX_BUFFSIZE];
        while((nread=read(fd,buff+n,MAX_BUFFSIZE-1))!=0)
        {
            n += nread;
            std::cout<<strlen(buff);
            if (nread == -1) {
                if (errno == EAGAIN) {
                    //reset epooll event
                    epoll_event event;
                    event.data.fd = fd;
                    event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
                    epoll_ctl(epoll_fd, EPOLL_CTL_MOD, fd, &event);
                }
            }
            else {

                char *http_respone = "HTTP/1.1 200 OK\r\n"
                        "Content-Type:text/html;charset=utf-8\r\n"
                        "Content-Length:18\r\n"
                        "\r\n"
                        "Welcome to tinyweb";
                write(fd, http_respone, MAX_BUFFSIZE);
            }
        }
        if (nread == 0)
            close(fd);
        delete []buff;
        return ;
    }

} //namespace lo

