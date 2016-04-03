#ifndef _HTTP_REQ_
#define _HTTP_REQ_

#include <string>

class httpReq {
public:
    enum reqType {GET, POST, 
        UMP,  //未支持
        INVALID, // 无效请求 
        DISCON  // 连接中断
    };
    enum extendType {
        NONE,   //normal request 
        CGI   //cgi-request
    };
    reqType type;
    extendType exType;
    std::string url;
    bool keep_alive;
    std::string content;
    void parseURL();
};
#endif
