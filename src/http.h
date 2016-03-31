//
// Created by leviathan on 3/29/16.
//

#ifndef LO_HTTP_H
#define LO_HTTP_H

#include <string>

namespace lo
{

    //status code
    #define HTTP_OK 200;
    #define HTTP_NOT_FIND 404;

    class http_request
    {
    public:
        unsigned int    method;
        unsigned int    version;
        std::string     uri;
        std::string     url;
    };

    class http
    {
    public:
        http_request request;
        void parse_request(const char *);

    };
}//namespace lo
#endif //LO_HTTP_H
