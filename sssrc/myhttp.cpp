#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "myhttp.h"
#include "logHelper.h"

#include <iostream>
#include <algorithm>

using namespace std;

namespace {
    const int MAXLINE = 2048;
    const int FILEBUFF_SIZE = 4096;
    int getline(readBuf& buf, char* des) {
        char *p = des;
        int n = 0;
        //while(p < des + MAXLINE && read(sockfd, p, 1) == 1) {
        while(p < des + MAXLINE && buf.readn(p, 1) == 1) {
            ++n;
            if (*p == '\n') {
                return n;
            } else {
                ++p;
            } 
        }
        if (n == MAXLINE) {
            return MAXLINE;
        } else {
            return -1;
        }
    }
    string get_file_size(int fd)  {  
        unsigned long filesize = 0;      
        struct stat statbuff;  
        if(fstat(fd, &statbuff) < 0){  
            return string();  
        }else{  
            filesize = statbuff.st_size;  
        }  
        if (filesize == 0) {
            return "0";
        }
        string ret;
        while (filesize != 0) {
            ret.push_back(filesize % 10 + '0'); 
            filesize /= 10;
        }
        reverse(ret.begin(), ret.end());
        return ret;
    }  
}
myhttp::myhttp(int sock) : sockfd(sock), readbuf(sock) {  }

void myhttp::doService() {
    httpReq hr = parseHttpReq(); 
    int err = 0;
    switch(hr.type) {
        case httpReq::GET:
            if (hr.exType == httpReq::NONE) {

                string filePath;
                if (hr.url == "/") {
                    filePath = env::getMainPath() + env::defaultPage;
                } else {
                    filePath = env::getMainPath() + hr.url;
                }

                //404 not found
                if (access(filePath.c_str(), F_OK) != 0) {
                    err = readbuf.writen(head_not_found.c_str(), head_not_found.size());
                    if (err < 0) return;

                    err = readbuf.writen(env::serverInfo.c_str(), env::serverInfo.size());
                    if (err < 0) return;

                }

                err = readbuf.writen(head_ok.c_str(), head_ok.size());
                if (err < 0) return;
                cout << head_ok;


                err = readbuf.writen(env::serverInfo.c_str(), env::serverInfo.size());
                if (err < 0) return;
                cout << env::serverInfo;

                int fd = open(filePath.c_str(), O_RDONLY);
                if (fd < 0){
                    return;
                }

                err = readbuf.writen(content_type_text_html.c_str(), content_type_text_html.size());
                if (err < 0) return;
                cout << content_type_text_html;

                string len = content_length + get_file_size(fd) + "\n\n";
                cout << len;
                err = readbuf.writen(len.c_str(), len.size());
                if (err < 0) return;

                char fileBuff[FILEBUFF_SIZE] = {0};
                while ((err = read(fd, fileBuff, FILEBUFF_SIZE)) > 0) {
                    cout << fileBuff;
                    readbuf.writen(fileBuff, err);
                }
                cout << endl << "ok";
                close(fd);
            }
            break;
        case httpReq::INVALID:
            
            err = readbuf.writen(head_invalid.c_str(), head_invalid.size());
            if (err != 0) return;

            err = readbuf.writen(env::serverInfo.c_str(), env::serverInfo.size());
            if (err != 0) return;
            
            break;
        default:
            return;

    }
    sleep(10000);
    close(sockfd);
}

httpReq myhttp::parseHttpReq() {
    httpReq hr;
    char line[MAXLINE] = {0};
    int lc = 0;

    if ((lc = getline(readbuf, line) >= 12)) { // at least : "GET / HTTP/1"
    /********** parse request line ***********/

        if (strncmp("GET", line, 3) == 0) {
            /****************** GET method *********************/
            hr.type= httpReq::GET;
            size_t p = 3;
            while (p < MAXLINE && line[p] == ' ') {
                ++p;
            }
            if (p == MAXLINE) {
                hr.type = httpReq::INVALID;
                return hr;
            }
            //认为请求行长度不会超过 MAXLINE
            while(p < MAXLINE && line[p] != ' ') {
                hr.url.push_back(line[p++]);
            }
            hr.parseURL(); 
            return hr;
        } else if (strncmp("POST", line, 4) == 0) {
             
        }
        
    }

    hr.type = httpReq::INVALID;
    return hr;
}

string myhttp::head_invalid =       "HTTP/1.1 400 Bad Request\r\n";
string myhttp::head_ok =            "HTTP/1.1 200 OK\r\n";
string myhttp::head_not_found =     "HTTP/1.1 404 Not Found\r\n";

string myhttp::content_type_text_html = "Content-Type: text/html\n";
string myhttp::content_length =         "Content-Length: ";
