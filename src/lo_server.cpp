#include <iostream>
#include <fstream>
#include <uv.h>
#include <assert.h>
#include <cstring>
#include "lo_server.h"
using namespace std;

string str_type[]=
{
    "text/html",
    "text/javascript",
    "text/css",
    "image/jpeg",
    "image/png",
    "image/gif",
    "text/plain"
};

string str_status[]=
{
    "200 OK",
    "404 Not Found"
};

req_content req_pass;

//lo class static member
const char * lo::http_request="";
const char * lo::http_response="";
string lo::file_priv="../test";


uv_tcp_t _server;
uv_tcp_t _client;
uv_loop_t * _loop;

void lo::lo_start(uv_loop_t* loop, const char* ip, int port) {
        sockaddr_in addr;
        uv_ip4_addr(ip, port, &addr);
        _loop = loop;
        uv_tcp_init(_loop, &_server);
        uv_tcp_bind(&_server, (const struct sockaddr*) &addr, 0);
        uv_listen((uv_stream_t*)&_server, 8, lo_on_connection);
}

void lo::tinyweb_close_client(uv_stream_t * client)
{
	uv_close((uv_handle_t *)client,after_uv_close);
}


content_type lo::enum_convert(string type)
{
    	if(type=="html")
	    return HTML;
}
void lo::after_uv_close(uv_handle_t* handle) {
        delete handle; // uv_tcp_t* client, see tinyweb_on_connection()
}

void lo::on_uv_alloc(uv_handle_t * handle,size_t suggested_size,uv_buf_t *buf)
{
        char *base=new char[suggested_size];
        *buf=uv_buf_init(base,suggested_size);
}

void lo::after_uv_write(uv_write_t* w, int status) {
        uv_close((uv_handle_t*)w->handle, after_uv_close); // close client
}

void lo::on_uv_read(uv_stream_t *client,ssize_t nread,const uv_buf_t *buf)
{
	http_request=buf->base;
        if(nread>0)
        {
		string str=http_request;
		cout<<str<<endl;
		http_response=get_response(http_request);
	    	write_uv_data(client,http_response,-1,0);
        }
	else if(nread==-1)
	{
		tinyweb_close_client(client);
	}
}

const char * lo::response_404(req_content & req_pass)
{
	string line="",opline="",path;
	if(req_pass.type==HTML) //text
	{
	    ifstream in_file;
	    path=file_priv+"/page404.html";
	    in_file.open(path);
	    while(getline(in_file,opline))
		line+=opline;
	    in_file.close();
	    req_pass.length=line.length();
	}
	else //image
	{
		
	}
	return make_response(req_pass,line);
}


const char * lo::response_content(req_content req_pass)
{
	string line="",opline="",path;
	if(req_pass.type==HTML) //text
	{
	    ifstream in_file;
	    path=file_priv+req_pass.req_uri;
 	    in_file.open(path);
	    while(getline(in_file,opline))
		line+=opline;
	    in_file.close();
	    req_pass.length=line.length();
	}
	else //other file eg:image
	{
	    line=get_filebin(req_pass);	
	}
	return make_response(req_pass,line);
}

const string  lo::get_filebin(req_content req_pass)
{
	int length;
	string path,line;
	fstream file;
	path=file_priv+req_pass.req_uri;
	file.open(path,ios::binary|ios::in);
	if(!file)
	    return response_404(req_pass);
	else
	{
	    length=file.tellg();
	    cout<<length<<endl;
	    file.read((char *)(&line),length);
	    return line;
	}
	
}
const char * lo::get_response(const char *request)
{
	string path;
	int file_pos;
	analyze_request(req_pass,request);
	if(req_pass.method=="GET")
	{
	    file_pos=req_pass.req_uri.rfind(".",req_pass.req_uri.length());
	    string file_suffix=req_pass.req_uri.substr(++file_pos);
	    req_pass.type=enum_convert(file_suffix);
	    fstream file;
	    path=(file_priv+req_pass.req_uri).c_str();
	    file.open(path);
	    if(!file)
	    {
	        //404 not find
	        req_pass.status=_404;
		return response_404(req_pass);
	    }
	    else
   	    {
	        //read request page
	        req_pass.status=_200;
		return response_content(req_pass);
	    }
	    file.close(); 
	}
	else
	{
		
	}
}

const char * lo::make_response(req_content req_pass,string content)
{
	string response="";
	response=response+"HTTP/1.1 "+str_status[req_pass.status]+"\r\n"+"Content-Type:"+str_type[req_pass.type]+";"+"charset=utf-8\r\n"+"Content-Length:"+to_string(content.length())+"\r\n"+"\r\n"+content;
	return response.c_str();
}

void lo::analyze_request(req_content & req_pass,const char * request)
{
	int index=0;
	//method
	if(request[index]=='G'&&request[index+1]=='E'&&request[index+2]=='T') //GET method
	{
	    index+=3;
	    req_pass.method="GET";
	}
	else if(request[index]=='P'&&request[index]=='O'&&request[index]=='S'&&request[index]=='T') //post method
	{   
	    req_pass.method="POST";   
	    index+=4;
	}
	index++;
	//uri
	req_pass.req_uri="";
	while(request[index]!=' ')
	{
	    req_pass.req_uri+=request[index];
	    index++;
	}
	index++; //space		
}


void lo::write_uv_data(uv_stream_t* stream, const char* data, unsigned int len, int need_copy_data) {
        uv_buf_t buf;
        uv_write_t* w=new uv_write_t;
        char* newdata =strdup(data);
        if(data == NULL || len == 0) return;
        if(len ==(unsigned int)-1)
            len = strlen(data);
        buf = uv_buf_init(newdata, len);
        w->data = newdata;
        uv_write(w, stream, &buf, 1, after_uv_write); // free w and w->data in after_uv_write()

}

void lo::lo_on_connection(uv_stream_t* server, int status) {
    assert(server == (uv_stream_t*)&_server);
    if(status == 0) {
        uv_tcp_t* client = new uv_tcp_t;
        uv_tcp_init(_loop, client);
        uv_accept((uv_stream_t*)&_server, (uv_stream_t*)client);
        uv_read_start((uv_stream_t *)client,on_uv_alloc,on_uv_read);
       //write_uv_data((uv_stream_t*)client, http_respone, -1, 0);
       //close client after uv_write, and free it in after_uv_close()
        }

}
