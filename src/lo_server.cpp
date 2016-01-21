#include <iostream>
#include <uv.h>
#include <assert.h>
#include <string.h>
#include "lo_server.h"
using namespace std;

uv_tcp_t _server;
uv_tcp_t _client;
uv_loop_t * _loop;
const char* http_respone = "HTTP/1.1 200 OK\r\n"
    "Content-Type:text/html;charset=utf-8\r\n"
        "Content-Length:18\r\n"
            "\r\n"
                "Welcome to tinyweb";

void lo::tinyweb_start(uv_loop_t* loop, const char* ip, int port) {
        sockaddr_in addr;
        uv_ip4_addr(ip, port, &addr);
        _loop = loop;
        uv_tcp_init(_loop, &_server);
        uv_tcp_bind(&_server, (const struct sockaddr*) &addr, 0);
        uv_listen((uv_stream_t*)&_server, 8, tinyweb_on_connection);
}

void lo::after_uv_close(uv_handle_t* handle) {
        delete handle; // uv_tcp_t* client, see tinyweb_on_connection()
}

void lo::after_uv_write(uv_write_t* w, int status) {
        if(w->data)
            delete w->data;
        uv_close((uv_handle_t*)w->handle, after_uv_close); // close client
        delete w;

}


void lo::write_uv_data(uv_stream_t* stream, const char* data, unsigned int len, int need_copy_data) {
        uv_buf_t buf;
        uv_write_t* w=new uv_write_t;
        char* newdata  =strdup(data);
        if(data == NULL || len == 0) return;
        if(len ==(unsigned int)-1)
            len = strlen(data);
        buf = uv_buf_init(newdata, len);
        w->data = newdata;
        uv_write(w, stream, &buf, 1, after_uv_write); // free w and w->data in after_uv_write()

}

void lo::tinyweb_on_connection(uv_stream_t* server, int status) {
    assert(server == (uv_stream_t*)&_server);
    if(status == 0) {
        uv_tcp_t* client = new uv_tcp_t;
        uv_tcp_init(_loop, client);
        uv_accept((uv_stream_t*)&_server, (uv_stream_t*)client);
        write_uv_data((uv_stream_t*)client, http_respone, -1, 0);
       //close client after uv_write, and free it in after_uv_close()
        }

}
