#ifndef _MY_HTTP_
#define _MY_HTTP_

#include "sys/socket.h"
#include "unistd.h"
#include <cerrno>

#include "env.h"
#include "cstring"
#include "cstddef"
#include "logHelper.h"
#include "env.h"
#include "httpReq.h"
#include <iostream>

namespace {
    const size_t BUFFSIZE = 2048;
    const int TIMEOUT = 1;
}


class readBuf{
public:
    char buff[BUFFSIZE];   
    size_t rest;
    size_t cur;

public:
    int sockfd;

    // for time out, select 
    timeval tv;
    fd_set rset;

    // default constructor
    readBuf (int sock) : rest(0), cur(0), sockfd(sock) {
        memset(buff, 0, 2048);
    }
    int readn(char *des, size_t n) {
        if (n <= rest) {
            memcpy(des, buff + cur, n);
            rest -= n;
            cur += n;
            return n;
        } else {
            size_t already = 0;
            memcpy(des, buff + cur, rest);
            already += rest;
            while(true) {
                FD_ZERO(&rest);
                FD_SET(sockfd, &rset);
                tv.tv_sec = TIMEOUT;
                tv.tv_usec = 0;
                int reads = select(sockfd + 1, &rset, nullptr, nullptr, &tv);
                if (reads <= 0) {
                    if (errno == EINTR) {
                        continue;
                    } else {
                        return already;
                    }
                }
                reads = read(sockfd, buff, BUFFSIZE);
                if (reads < 0) {
                    if (errno == EINTR) {
                        continue;
                    } else {
                        return already;
                    }
                }
                if (reads + rest < n) {
                    memcpy(des +already, buff, reads);
                    already += reads;
                    rest = 0;
                    cur = 0;
                    return already;
                } else {
                    memcpy(des + already, buff, n - already);
                    rest = reads - (n - already);
                    cur = n - already;
                    return n;
                }

                //loop just do once
                break;
            }
        }
    }

    ////////////////  size_t may overflow //////////
    int writen(const char *src, size_t n) {
        int err = 0; 
        int rest = n;
        while(rest > 0) {
            err = write(sockfd, src, rest);
            if (err < 0) {
                if (errno == EINTR) {
                    continue;
                } else {
                    return -1;
                }
            }
            rest -= err;
        }
        return n;
    }
};

class myhttp {
public:
    myhttp(int sockfd);
    myhttp(const myhttp&) = delete;
    void doService();
private:
    int sockfd;
    readBuf readbuf;
    //generate httpReq
    httpReq parseHttpReq();
    //parse from sockfd
    int parseReqline(httpReq&);
    
    static std::string head_invalid;
    static std::string head_ok;
    static std::string head_not_found;
    static std::string content_type_text_html;
    static std::string content_length;
};

#endif
