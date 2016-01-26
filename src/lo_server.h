#ifndef _LO_SERVER_H
#define _LO_SERVER_H

#include <iostream>
#include <uv.h>

class lo
{
public:
    void tinyweb_start(uv_loop_t* loop, const char* ip, int port);
    static void on_uv_alloc(uv_handle_t *handle,size_t suggested_size,uv_buf_t * buf);
    static void on_uv_read(uv_stream_t * client,ssize_t nread,const uv_buf_t *buf);
    static void after_uv_close(uv_handle_t* handle);
    static void tinyweb_close_client(uv_stream_t * client);
    static void after_uv_write(uv_write_t* w, int status);
    static void write_uv_data(uv_stream_t* stream, const char* data, unsigned int len, int need_copy_data);
    const char *get_response(const char *request);
    static void tinyweb_on_connection(uv_stream_t* server, int status);
    const char * request;
    const char * response;
};
#endif
