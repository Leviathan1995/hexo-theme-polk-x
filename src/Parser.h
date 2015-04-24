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
#include <map>

using namespace std;
map<string,string> Http_HeadText;

//Http_Server 头部
struct Http_Header
{
    string Method; //方法
    string Url;    //链接
    stirng Version;//版本
    Http_HeadText Header;//首部
    string body;
}
//Http_Server 类
class Http_Server
{
    public:
    void Http_PrintHttpHeadText(); //打印Http_HeadText
    void Http_PrintHttpHead();     //打印Http_HeadText

}
#endif
