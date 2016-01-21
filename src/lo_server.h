#ifndef _LO_SERVER_H
#define _LO_SERVER_H

#include <iostream>
#include <uv.h>

const char* http_respone = "HTTP/1.1 200 OK\r\n"
    "Content-Type:text/html;charset=utf-8\r\n"
        "Content-Length:18\r\n"
            "\r\n"
                "Welcome to tinyweb";

class lo
{
public:
    void tinyweb_on_connection(uv_stream_t* server, int status);
    void tinyweb_start(uv_loop_t* loop, const char* ip, int port);
    void after_uv_close(uv_handle_t* handle);
    void after_uv_write(uv_write_t* w, int status);
    void write_uv_data(uv_stream_t* stream, const void* data, unsigned int len, int need_copy_data);
    void tinyweb_on_connection(uv_stream_t* server, int status);
    uv_tcp_t   _server;
    uv_tcp_t   _client;
    uv_loop_t* _loop;
};
#endif

