
//
//  Created by leviathan on 16/6/17.
//

#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdio.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>


namespace lo
{
    extern std::string strlevel[];
    
    //log level
    enum loglevel
    {
        LDEBUG, //指明细致的事件信息，对调试应用最有用。
        LERROR, //指明错误事件，但应用可能还能继续运行。
        LFATAL, //指明非常严重的错误事件，可能会导致应用终止执行。
        LINFO,  //指明描述信息，从粗粒度上描述了应用运行过程。
        LTRACE, //比DEBUG级别的粒度更细。
        LWARN,  //指明潜在的有害状况。

    };
    
    
#define printlog(level,...) \
    Log::Instance().PrintLog(level, __FILE__, __LINE__, __func__, __VA_ARGS__);
    
    
#define TRACE(...) printlog(LTRACE, __VA_ARGS__)
#define DEBUG(...) pringlog(LDEBUG, __VA_ARGS__)
#define INFO(...) printlog(LINFO, __VA_ARGS__)
#define WARN(...) printlog(LWARN, __VA_ARGS__)
#define ERROR(...) printlog(LERROR, __VA_ARGS__)
#define FATAL(...) printlog(LFATAL, __VA_ARGS__)
#define FATALIF(b, ...) do { if((b)) { printlog(LFATAL, __VA_ARGS__); } } while (0)
#define CHECK(b, ...) do { if((b)) { printlog(LFATAL, __VA_ARGS__); } } while (0)
#define EXITIF(b, ...) do { if ((b)) { printlog(LERROR, __VA_ARGS__); _exit(1); }} while(0)
    

    
    //
    //Log
    //
    class Log
    {
    public:
        static Log & Instance()
        {
            static Log log;
            return log;
        }
        void PrintLog(int level,const char * file,int line,const char * func ,const char * parm,...);

    private:
        Log(){};
        ~Log(){};
        Log & operator=(Log const &);   // assign op. hidden
        Log(Log const &);               // copy ctor hidden

    };
}

#endif /* log_h */
