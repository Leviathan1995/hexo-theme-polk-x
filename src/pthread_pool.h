//
// Created by leviathan on 16/3/24.
//

#ifndef LO_THREAD_POOL_H
#define LO_THREAD_POOL_H

#include <vector>
#include <pthread.h>
#include <queue>
#include <functional>
#include <unistd.h>
#include <sys/event.h>

#include "http.h"

namespace  lo
{
    //pthread pool module
    class ThreadPool
    {
    public:
        ThreadPool(){};
        ThreadPool(int num);
        ~ThreadPool(){};
        
        void CreatePool();
        void AddTask(int fd);
        void SetThreadNum(int num);
        static void SetIoEventFd(int fd);
        static void *ThreadCall(void *data);
        
        static void HandleSocket(int fd);

    private:
        static int io_event_fd_;
        static bool shutdown_;
        static pthread_mutex_t thread_mutex_;
        static pthread_cond_t thread_cond_;
        int thread_num_;
        pthread_t * thread_id_;
        static std::queue<int > task_list_;
    };
    
}
#endif //LO_THREAD_POOL_H
