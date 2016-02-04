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


//lo class static member
const char * lo::http_request="";
const char * lo::http_response="";
string lo::file_priv="../test";
string lo::merge_response="";
parse_content lo::parse_pass;

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
	if(type=="png")
	    return PNG;
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
		string str_request=http_request;
		cout<<str_request<<endl;  //print request
		http_response=get_response(http_request);
	    	write_uv_data(client,http_response);
        }
	else if(nread==-1)
	{
		tinyweb_close_client(client);
	}
}

const char * lo::response_404()
{
	string line="",opline="",path;
	if(parse_pass.type==HTML) //text
	{
	    ifstream in_file;
	    path=file_priv+"/page404.html";
	    in_file.open(path);
	    while(getline(in_file,opline))
		line+=opline;
	    in_file.close();
	    parse_pass.content_length=line.length();
	}
	else //image
	{
		
	}
	return make_response(line);
}


const char * lo::response_content()
{
	char *data;
	string line="",opline="",path;
	if(parse_pass.type==HTML) //text
	{
	    ifstream in_file;
	    path=file_priv+parse_pass.req_uri;
 	    in_file.open(path);
	    while(getline(in_file,opline))
		line+=opline;
	    in_file.close();
	    parse_pass.content_length=line.length();
	    return make_response(line);
	}
	else //other file eg:image
	{
	    return get_filebin();
	}
}

const char*  lo::get_filebin()
{
	string path;
	int length;
	char *data;
	ifstream file;
	path=file_priv+parse_pass.req_uri;
	file.open(path,ios::binary|ios::app);
	if(file)
	{
	    file.seekg(0,ios::end);
	    length=file.tellg();
	    parse_pass.content_length=length;
	    data=new char[length];
	    file.seekg(0,ios::beg);
	    file.read(data,length);
	    return make_response(string(data,length));
	}
	
}
const char * lo::get_response(const char *request)
{
	string path;
	int file_pos;
	analyze_request(request);
	if(parse_pass.method=="GET")
	{
	    file_pos=parse_pass.req_uri.rfind(".",parse_pass.req_uri.length());
	    string file_suffix=parse_pass.req_uri.substr(++file_pos);
	    parse_pass.type=enum_convert(file_suffix);
	    fstream file;
	    path=(file_priv+parse_pass.req_uri).c_str();
	    file.open(path);
	    if(!file)
	    {
	        //404 not find
	        parse_pass.status=_404;
		return response_404();
	    }
	    else
   	    {
	        //read request page
	        parse_pass.status=_200;
		return response_content();
	    }
	    file.close(); 
	}
	else
	{
		
	}
}

const char * lo::make_response(string content)
{
	string response="";
	if(parse_pass.type==HTML)
	{
	    response=response+"HTTP/1.1 "+str_status[parse_pass.status]+"\r\n"+"Content-Type:"+str_type[parse_pass.type]+";"+"charset=utf-8\r\n"+"Content-Length:"+to_string(content.length())+"\r\n"+"\r\n"+content;
	    parse_pass.msg_length=response.length();
	    cout<<response<<endl;
	}
	else  //image ..
	{
	    response=response+"HTTP/1.1 "+str_status[parse_pass.status]+"\r\n"+"Content-Type:"+str_type[parse_pass.type]+"\r\n"+"Accept-Ranges:bytes\r\n"+"Content-Length:"+to_string(content.length())+"\r\n"+"Connection:keep-alive\r\n"+"\r\n";
	    parse_pass.msg_length=response.length()+content.length();
	    response+=content;
	}
	merge_response=response;
	return merge_response.data();
}

void lo::analyze_request(const char * request)
{
	int index=0;
	//method
	if(request[index]=='G'&&request[index+1]=='E'&&request[index+2]=='T') //GET method
	{
	    index+=3;
	    parse_pass.method="GET";
	}
	else if(request[index]=='P'&&request[index]=='O'&&request[index]=='S'&&request[index]=='T') //post method
	{   
	    parse_pass.method="POST";   
	    index+=4;
	}
	index++;
	//uri
	parse_pass.req_uri="";
	while(request[index]!=' ')
	{
	    parse_pass.req_uri+=request[index];
	    index++;
	}
	index++; //space		
}


void lo::write_uv_data(uv_stream_t* stream, const void* data)
{
	int len=0;
	uv_buf_t buf;
        uv_write_t* w=new uv_write_t;
        void* newdata =(void *)data;
        if(data == NULL || parse_pass.msg_length==0) 
	    return;
	cout<<"Request method:"<<parse_pass.method<<endl;
	cout<<"Status code:"<<str_status[parse_pass.status]<<endl;
	cout<<"Content-type:"<<str_type[parse_pass.type]<<endl;
	cout<<"Response msg length:"<<parse_pass.msg_length<<endl<<endl;
	len=parse_pass.msg_length;
        buf = uv_buf_init((char *)newdata, len);
        w->data = newdata;
	string s=(char *)newdata;
	cout<<s<<endl;
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
