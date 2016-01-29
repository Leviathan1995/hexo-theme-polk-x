#ifndef _LO_SERVER_H
#define _LO_SERVER_H

#include <iostream>
#include <string>
#include <uv.h>

enum content_type
{
    HTML,
    JS, //javascript
    CSS,
    JPEG,
    PNG,
    GIF,
    TXT
};

enum status_code
{
    _200,
    _404
};

extern std::string str_type[];
extern std::string str_status[];

class req_content
{
public:
    std::string method;
    std::string req_uri;
    status_code status;
    content_type type;
    int length;
};

class lo
{
public:
    //function member
    void lo_start(uv_loop_t* loop, const char* ip, int port);
    static void on_uv_alloc(uv_handle_t *handle,size_t suggested_size,uv_buf_t * buf);
    static void on_uv_read(uv_stream_t * client,ssize_t nread,const uv_buf_t *buf);
    static void after_uv_close(uv_handle_t* handle);
    static void tinyweb_close_client(uv_stream_t * client);
    static void after_uv_write(uv_write_t* w, int status);
    static void write_uv_data(uv_stream_t* stream, const char* data, unsigned int len, int need_copy_data);
    static const char *get_response(const char *request);
    static const char *make_response(req_content  req_pass,std::string content);
    static void analyze_request(req_content & req_pass,const char * request);
    static content_type  enum_convert(std::string type);
    static void lo_on_connection(uv_stream_t* server, int status);
    static const char * response_404(req_content & req_pass);
    static const char * response_content(req_content req_pass);
    static const std::string  get_filebin(req_content req_pass);
    //data member
    static const char * http_request;
    static const char * http_response;
    static std::string file_priv;
};

#endif
