//
// Created by leviathan on 3/29/16.
//

#ifndef LO_HTTP_H
#define LO_HTTP_H

#include <iostream>
#include <fstream>

namespace lo
{


    //status code
    #define HTTP_OK 200;
    #define HTTP_NOT_FIND 404;

    //method
    #define HTTP_GET 0x0001
    #define HTTP_POST 0x0002
    #define HTTP_HEAD 0x0003

    //version
    #define HTTP_VERSION_1_1 0X0011;

    class http_request
    {
    public:
        http_request(){};
        ~http_request()
        {
            delete [] url;
            delete [] host;
        };
        unsigned int    method;
        unsigned int    version;
        char *  url;
        char *  host;
    };

    class http_response
    {
    public:
        int status;
        unsigned int version;
        std::string type;
        std::string charset;

    };
    class http
    {
    public:
        http(){};
        ~http(){};
        static http_request request;
        static http_response response;
        static void  parse_request(const char * buff,int num);
        static char * get_response();


    };
}//namespace lo
#endif //LO_HTTP_H
