//
// Created by leviathan on 16/3/24.
//
#include "pthread_pool.h"
#include "lo.h"

namespace lo
{
    
    std::queue<int > ThreadPool::task_list_;
    int ThreadPool::io_event_fd_=0;
    bool ThreadPool::shutdown_= false;
    pthread_mutex_t ThreadPool::thread_mutex_=PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t ThreadPool::thread_cond_=PTHREAD_COND_INITIALIZER;
    
    ThreadPool::ThreadPool(int num)
    {
        thread_num_=num;
        thread_id_=new pthread_t[thread_num_];
        CreatePool();
    }
    
    void ThreadPool::SetThreadNum(int num)
    {
        thread_num_=num;
    }
    
    void ThreadPool::SetIoEventFd(int event_fd)
    {
        io_event_fd_=event_fd;
    }

    //create pthread pool
    void ThreadPool::CreatePool()
    {
        for (int i = 0; i <thread_num_; ++i)
        {
            pthread_create(&thread_id_[i], NULL,ThreadCall, NULL);
        }
    }
    
    //add fd to task_list
    void ThreadPool::AddTask(int fd)
    {
        pthread_mutex_lock(&thread_mutex_);
        task_list_.push(fd);
        pthread_mutex_unlock(&thread_mutex_);
        pthread_cond_signal(&thread_cond_);
    }
    
    void * ThreadPool::ThreadCall(void *data)
    {
        while(1)
        {
            pthread_mutex_lock(&thread_mutex_);
            while(task_list_.size()==0)
                pthread_cond_wait(&thread_cond_,&thread_mutex_);
            
            int fd=task_list_.front();
            task_list_.pop();
            pthread_mutex_unlock(&thread_mutex_);
            
            HandleSocket(fd);//handle funciton
        }
    }
    
    
    
    void ThreadPool::HandleSocket(int data_fd)
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

