//
//  Created by leviathan on 16/6/17.
//

#include "log.h"

namespace lo
{

    std::string strlevel[]=
    {
        "ERROR",
        "WARN",
        "INFO",
        "DEBUG",
        "TRACE"
    };
    
    void Logger::PrintLog(int level, const char* file, int line, const char* func, const char * pram, ...)
    {
        if (level>level_)
        {
            return;
        }
        
        struct timeval now_tv;
        gettimeofday(&now_tv,NULL); //得到当前时间
        const time_t seconds=now_tv.tv_sec;
        struct tm t;
        localtime_r(&seconds, &t);  //返回当地时间
        
        char *buffer=new char[4*1024];
        char* p = buffer;

        p+= snprintf(p,4*1024,
                      "%04d/%02d/%02d-%02d:%02d:%02d.%06d %lu %s %s:%d %s%s",
                      t.tm_year + 1900,
                      t.tm_mon + 1,
                      t.tm_mday,
                      t.tm_hour,
                      t.tm_min,
                      t.tm_sec,
                      static_cast<int>(now_tv.tv_usec),
                      pthread_self(),
                      strlevel[level].c_str(),
                      file,
                      line,
                      func,
                      "()"
                      );

        va_list pvar;
        va_start(pvar,pram);
        p += vsnprintf(p,4*1024,pram,pvar);
        va_end(pvar);
        
        std::cout<<buffer<<std::endl<<std::endl;
        delete []  buffer;
    }
}