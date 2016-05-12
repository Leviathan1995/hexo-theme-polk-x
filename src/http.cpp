//
// Created by leviathan on 3/29/16.
//
#include "http.h"
#include "lo.h"

namespace lo
{
    //http class static variable
    http_request http::request;
    http_response http::response;
    
    //parse request
    void  http::parse_request(const char * buff,int num)
    {
        int index = 0;
        
        //method
        if (buff[index] == 'G' && buff[index + 1] == 'E' && buff[index + 2] == 'T')    //GET
        {
            request.method = HTTP_GET;
            index += 3;
        }
        else if (buff[index] == 'P' && buff[index + 1] == 'O' && buff[index + 2] == 'S' && buff[index + 3] == 'T')   //POST
        {
            request.method = HTTP_POST;
            index += 4;
        }
        else if(buff[index]=='H'&& buff[index+1]=='E'&&buff[index+2]=='A'&&buff[index+3]=='D')  //HEAD
        {
            request.method=HTTP_HEAD;
            index+=4;
        }
        //space
        if (buff[index] == ' ')
            index++;
        
        //url
        char *url = new char[256];
        int url_index = 0;
        while (buff[index] != ' ') {
            url[url_index] = buff[index];
            url_index++;
            index++;
        }
        request.url = url;
        
        //space
        if (buff[index] == ' ')
            index++;
        
        //version
        while (buff[index] != ' ') {
            if (buff[index] == '/') {
                index++;
                if (buff[index] == '1' && buff[index + 1] == '.' && buff[index + 2] == '1') {
                    request.version = HTTP_VERSION_1_1;
                    index += 3;
                };
                break;
            }
            else
                index++;
        }
        
        // \r\n
        if (buff[index] == '\r' && buff[index + 1] == '\n')
            index += 2;
        
        while(buff[index]=='\r'&&buff[index+1]=='\n'&&buff[index+2]=='\r'&&buff[index+3]=='\n') // \r\n\r\n
        {
            if(buff[index]=='H'&&buff[index+1]=='o'&&buff[index+2]=='s'&&buff[index+3]=='t')    //host
                index+=4;
            else if(buff[index]=='C'&&buff[index+1]=='o'&&buff[index+2]=='n'&&buff[index+3]=='n'&&buff[index+4]=='e'&&buff[index+5]=='c'&&buff[index+6]=='t'&&buff[index+6]=='i'&&buff[index+7]=='o'&&buff[index+8]=='t')   //connection
                index+=9;
            
        }
    }
    
    //get response
    char * http::get_response()
    {
        response.version=HTTP_VERSION_1_1;
        response.charset="utf-8";
        char *buff=new char[MAX_BUFFSIZE];
        char * content;
        int content_length=0;
        if(request.method==HTTP_GET)
        {
            std::string str_file(request.url);
            
            //check file type
            int file_pos;
            std::string str_url(request.url);
            file_pos=str_url.rfind(".",str_url.length());
            std::string str_url_suffix=str_url.substr(++file_pos);
            response.type=str_url_suffix;
            
            //read file
            std::fstream file;
            file.open(root+str_file,std::ios::binary|std::ios::in);
            if(!file) //404
            {
                response.status=HTTP_NOT_FIND;
                std::cout << "open file failed!" << std::endl;
                file.open(root+"/NOT_FOUND.html",std::ios::binary|std::ios::in);
                file.seekg(0, std::ios::end);
                content_length = file.tellg();
                content = new char[content_length];
                file.seekg(0, std::ios::beg);
                file.read(content, content_length);
            }
            else  //200
            {
                response.status = HTTP_OK;
                file.seekg(0, std::ios::end);
                content_length = file.tellg();
                content = new char[content_length];
                file.seekg(0, std::ios::beg);
                file.read(content, content_length);
            }
            
            //
            //make response
            //
            
            //status line
            
            //version
            if(response.version==0x0011)
                strcat(buff,"HTTP/1.1");
            strcat(buff," ");
            //status
            if(response.status==200)  //200
                strcat(buff,"200 OK");
            else if(response.status==404) //404
                strcat(buff,"404 NOT FOUND");
            
            strcat(buff,"\r\n");
            
            //
            //response header
            //
            
            //MIME TYPE "text"
            if(response.type=="html")
                strcat(buff,"Content-Type:text/html;");
            else if(response.type=="css")
                strcat(buff,"Content-Type:text/html;");
            else if(response.type=="plain")
                strcat(buff,"Content-Type:text/plain");
            else if(response.type=="js")
                strcat(buff,"Content-Type:test/javascript");
            //MIME TYPE "image"
            else if(response.type=="bmp")
                strcat(buff,"Content-Type:image/bmp");
            else if(response.type=="gif")
                strcat(buff,"Content-Type:image/gif");
            else if(response.type=="png")
                strcat(buff,"Content-Type:image/png");
            else if(response.type=="jpeg")
                strcat(buff,"Content-Type:image/jpeg");
            
            
            
            if(response.charset=="utf-8")
                strcat(buff,"charset=utf-8");
            strcat(buff,"\r\n");
            
            //content length
            std::string str_length=std::to_string(content_length);
            char * length=new char[str_length.length()];
            int length_index=0;
            for(auto s:str_length)
                length[length_index++]=s;
            
            strcat(buff,"Content-Length:");
            strcat(buff,length);
            strcat(buff,"\r\n");
            strcat(buff,"\r\n");
            
            //content
            strcat(buff,content);
            
            //detele
            delete [] length;
            delete [] content;
        }
        return buff;
    }
    
}//namespace
