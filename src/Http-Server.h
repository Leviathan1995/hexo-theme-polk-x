/*************************************************************************
	> File Name: Http-Server.h
	> Author: 
	> Mail: 
	> Created Time: 2015年04月27日 星期一 09时58分00秒
 ************************************************************************/

#ifndef _HTTP-SERVER_H
#define _HTTP-SERVER_H
#include <sys/epoll.h>
using namespace std;

struct Epoll_Connfd
{
    int Epoll_fd;
    int Conn_fd;
};
/*
 Define 定义
*/

#define Max-_Events 1024  //Epoll 最大监听事件数
#define Max_Queue   100   //监听队列最大数
//Http 状态码

#define TYHP_CONTINUE       100//收到了请求的起始部分，客户端应该继续请求
#define TYHP_OK             200//服务器已经成功处理请求
#define TYHP_ACCEPTED       202//请求已接受，服务器尚未处理

#define TYHP_MOVED          301//请求的URL已移走，响应应该包含Location URL
#define TYHP_FOUND          302//请求的URL临时移走，响应应该包含Location URL
#define TYHP_SEEOTHER       303//告诉客户端应该用另一个URL获取资源，响应应该包含Location URL
#define TYHP_NOTMODIFIED    304//资源未发生变化

#define TYHP_BADREQUEST     400//客户端发送了一条异常请求
#define TYHP_FORBIDDEN      403//服务器拒绝请求
#define TYHP_NOTFOUND       404//URL未找到


#define TYHP_ERRR           500//服务器出错
#define TYHP_NOIMPLEMENTED  501 //服务器不支持当前请求所需要的某个功能
#define TYHP_BADGATEWAY     502//作为代理或网关使用的服务器遇到了来自响应链中上游的无效响应
#define TYHP_SRVUNAVAILABLE 503 //服务器目前无法提供请求服务，过一段时间后可以恢复
/*
    MIME定义
*/
struct Mime_struct
{
    string Type;
    stirng Value;
};

Mime_struct Http_Mime[]=
{
            {".html", "text/html"},
            {".xml", "text/xml"},
            {".xhtml", "application/xhtml+xml"},
            {".txt", "text/plain"},
            {".rtf", "application/rtf"},
            {".pdf", "application/pdf"},
            {".word", "application/msword"},
            {".png", "image/png"},
            {".gif", "image/gif"},
            {".jpg", "image/jpeg"},
            {".jpeg", "image/jpeg"},
            {".au", "audio/basic"},
            {".mpeg", "video/mpeg"},
            {".mpg", "video/mpeg"},
            {".avi", "video/x-msvideo"},
            {".gz", "application/x-gzip"},
            {".tar", "application/x-tar"},
            {NULL ,NULL}
};

#endif
