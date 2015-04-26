/*************************************************************************
	> File Name: Parser.cpp
	> Author: 
	> Mail: 
	> Created Time: 2015年04月24日 星期五 02时21分50秒
 ************************************************************************/

#include<iostream>
using namespace std;
#include "Parser.h"
#include "string"
//构造函数
Http_Server::Http_Server()
{

}
//析构函数
Http_Server::~Http_Server()
{
    
}
//打印Http_HeadText
void Http_Server:: Http_PrintHttpHeadText(Http_HeadText & head)
{
    if(!head.empty())
    {
        for(auto i=head.begin();i!=head.end();i++)
            cout<<i->first<<":"<<i->second<<endl;
    }
}
//打印Http_Header
void Http_Server::Http_PrintHttpHead(Http_Header &head)
{
    if(head==NULL)
    {
        cout<<"Http_Header == NULL"<<endl;
        return ;
    }
    cout<<head->Method<<" "<<head->Url<<" "<<head->Version<<endl;
    Http_PrintHttpHeadText(head->Header);
    cout<<endl;
    cout<<head->Body<<endl;
}
//解析http request
bool  Http_Server::Http_ParserRequest(string & request,Http_Header * http_header)
{
    if(request.empty())
    {
        cout<<" Http_ReturnRequest: http_request is null"<<endl;
        return false;
    }
    if(http_header==NULL)
    {
        cout<<" Http_ReturnRequest: http_header is null"<<end;
        return false;
    }
    string Crlf("\r\n"),Crlf2("\r\n\r\n");
    int Index=0,Next=0;

    //解析Http请求的起始行
    if(Next=http_request.find(Crlf,Index)==string::nops)
    {
        string FIrst_line(http_request.substr(Index,Next-Index));
        Index=Next;
        stringstream sstream(FIrst_line);
        sstream>>(head->Method);
        sstream>>(head->Url);
        sstream>>(head->Version);
    }
    else{
        cout<<"Parser_Request:http_request is not have \\r\\n";
        return false;
    }
    //查找\r\n\r\n
    Index=http_request.find(Crlf2,Index);
    if(Index==string::npos)
    {
        cout<<"Parser_Request:http_request have not a \"\r\n\r\n\"";
        return false;
    }

}
//返回指定的用户请求
bool Http_Server::Http_ReturnRequest(string & http_request,Http_Header *http_header)
{
    
}
