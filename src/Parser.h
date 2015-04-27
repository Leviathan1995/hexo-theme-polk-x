/*************************************************************************
    > File Name: Parser.h
    > Author: Leviathan1995
    > Mail: Leviathan1995@outlook.com
    > Created Time: 2015年04月24日 星期五 02时25分25秒
 ************************************************************************/
#pragma once
#ifndef _PARSER_H
#define _PARSER_H
#include <string>
#include <sstream>
#include <map>

using namespace std;
map<string,string> Http_HeadText;//报文首部

//Http_Server 报文
struct Http_Message
{
    //请求行
    string Method; //方法
    string Url;    //链接
    stirng Version;//版本
    //报文首部
    Http_HeadText Header;
    //报文主体
    string Body;
}
//Http_Server 类
class Http_Server
{
    public:
    Http_Server();
    ~Http_Server();
    Http_Message *http_message;     //请求报文                    
    void Http_PrintHttpHeadText(Http_HeadText & head); //打印请求首部
    void Http_PrintHttpMessage(Http_Message  & head);            //打印报文
    bool  Http_ParserRequest(string & request,Http_Message * http_header);     //解析http_request,并返回报文
    string Http_ReturnRequest(string & key,Http_HeadText & header);//返回指定的用户的请求
}
#endif
