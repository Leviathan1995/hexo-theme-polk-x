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
    
    void pthread_pool::set_kq_fd(int kq_fd)
    {
        kq_fd=kq_fd;
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
        while(1)
        {
            pthread_mutex_lock(&pthread_mutex);
            while(task_list.size()==0)
                pthread_cond_wait(&pthread_cond,&pthread_mutex);
            
            int fd=task_list.front();
            task_list.pop();
            pthread_mutex_unlock(&pthread_mutex);
            
            socket_handle(fd);//handle funciton
        }
    }
    
    
    
    void pthread_pool::socket_handle(int data_fd)
    {
        int n=0,nread=0;
        char * request=new char[MAX_BUFFSIZE];
        while((nread=int(read(data_fd,request+n,MAX_BUFFSIZE-1)))!=0)
        {
            n += nread;
            if (nread == -1)
            {
                if (errno == EAGAIN)
                {
                    /*
                    pthread_mutex_lock(&flage_mutex);
                    EV_SET(&monitor_event[data_fd], data_fd, EVFILT_READ, EV_ADD|EV_ENABLE|EV_ONESHOT, 0, 0, 0);
                    std::cout<<data_fd<<" read eagain"<<std::endl;
                    id_fd[find_free()]=data_fd;
                    flags[find_free()]=1;
                    pthread_mutex_unlock(&flage_mutex);
                    */
                    break;
                }
            }
            else
            {
                
                /*
                 char *http_response = "HTTP/1.1 200 OK\r\n"
                 "Content-Type:text/html;charset=utf-8\r\n"
                 "Content-Length:18\r\n"
                 "\r\n"
                 "Welcome to tinyweb";
                 */
                http::parse_request(request,nread);
                char * response=http::get_response();
                write(data_fd,response, MAX_BUFFSIZE);
                delete [] response;
            }
        }
        if (nread == 0)
        {
            close(data_fd);
            /*
            pthread_mutex_lock(&flage_mutex);
            EV_SET(&monitor_event[data_fd], data_fd, EVFILT_READ, EV_DELETE, 0, 0, 0);
            std::cout<<data_fd<<" "<<" close fd"<<std::endl;
            flags[id_fd.find(data_fd)->first]=0;
            id_fd.erase(id_fd.find(data_fd)->first);
            pthread_mutex_unlock(&flage_mutex);
            */
            
        }
        delete [] request;
        return ;
    }
    
} //namespace lo

