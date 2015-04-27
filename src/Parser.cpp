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
     Http_Message *http_message=new Http_Message;
    
}
//析构函数
Http_Server::~Http_Server()
{
    delete http_message;
}
//打印报文首部
void Http_Server::Http_PrintHttpHeadText(Http_HeadText & head)
{
    if(!head.empty())
    {
        for(auto i=head.begin();i!=head.end();i++)
            cout<<i->first<<":"<<i->second<<endl;
    }
}
//打印报文
void Http_Server::Http_PrintHttpHead(Http_Message &head)
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
bool  Http_Server::Http_ParserRequest(string & request,Http_Message * http_header)
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

    //解析Http请求行
    if(Next=http_request.find(Crlf,Index)==string::nops)
    {
        string First_line(http_request.substr(Index,Next-Index));
        Index=Next;
        string [] FirstLine= First_line.split(" ");
        FirstLine[0]=http_header->Method;
        FirstLine[1]=http_header->Url;
        FirstLine[2]=http_header->Version;
    }
    else{
        cout<<"Parser_Request:http_request is not have \\r\\n";
        return false;
    }
    //查找\r\n\r\n
    Crlf_Pos=http_request.find(Crlf2,Index);//Crlf的位置
    if(Crlf_Pos<=string::npos)
    {
        cout<<"Parser_Request:http_request have not a \"\r\n\r\n\"";
        return false;
    }
    //解析请求头部
    string  Buff,Key,value;
    while(1)
    {
        Next=http_request.find(Crlf,Index+2);
        if(Next<=Crlf_Pos)
        {
            //Buff为请求头部的一行
            Buff=http_request.substr(Index+2,Next-Index-2);
            int Position=Buff.find(":");//找到“：”的位置
            string Key=Buff,substr(0,Position); //头部字段名
            string Value=BUff.substr(Position,Buff.length();)//头部值
            http_header->Header.insert(pair<string,string>(Key,Value)); //获取一个首部
            Index=Next;
        }
        else{
            break;
        }
    }
    //获取http请求的主体
    http_header->body=http_request.substr(Crlf_Pos+4,http_request.size()-Crlf_Pos-4);
    return true;

}
//返回指定的用户请求
bool Http_Server::Http_ReturnRequest(string & http_request,Http_HeadText &http_header)
{
    if(http_header.empty())
        return "";
    http_header::const_iterator Pos=http_header.find(Key);
    if(Pos==http_header.end())
        return "";
    return (*Pos).second;

}
