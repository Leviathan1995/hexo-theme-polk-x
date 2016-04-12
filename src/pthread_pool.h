//
// Created by leviathan on 3/26/16.
//

#ifndef LO_THREAD_POOL_H
#define LO_THREAD_POOL_H

#include <vector>
#include <pthread.h>
#include <queue>
#include <functional>
#include <unistd.h>
#include <sys/epoll.h>

#include "http.h"

namespace  lo
{
    //pthread pool module
    class pthread_pool
    {
    public:
        pthread_pool(){};
        pthread_pool(int num);
        ~pthread_pool(){};

        void create_pool();
        void add_task(int fd);
        void set_pthread_num(int num);
        static void set_epoll_fd(int fd);
        static void *pthread_call(void *data);

        static void socket_handle(int fd);
        int stop();

    private:
        static int epoll_fd;
        static bool shutdown;
        static pthread_mutex_t pthread_mutex;
        static pthread_cond_t pthread_cond;
        int pthread_num;
        pthread_t * pthread_id;
        static std::queue<int > task_list;  //socket fd list

    };

}
#endif //LO_THREAD_POOL_H
