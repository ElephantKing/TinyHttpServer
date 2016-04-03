#include "config.h"
#include "myhttp.h"
#include "env.h"
#include "logHelper.h"

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <errno.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

namespace {
    const int MAXLISTENQUEUE= 1000;
    void *storefd(int);
    int getfd(int64_t); 


    void *workFunc(void *ptr) {
        int socket = getfd(reinterpret_cast<int64_t>(ptr));
        myhttp mh(socket);
        mh.doService();
        return (void*)0;
    }

    void *storefd(int fd) {
        int64_t ret = 0; 
        for (int i = 0; i < 32; ++i) {
            if((fd >> i) & 1) {
                ret |= (1l << i);
            }
        }
        return reinterpret_cast<void*>(ret);
    }

    int getfd(int64_t fd) {
        int ret = 0;
        for (int i = 0; i < 32; ++i) {
            if((fd >> i) & 1l) {
                ret |= (1 << i);
            }
        }
        return ret;
    }
}
int main() {
    seteuid(0);
    //load config
    config::load_conf(); 
    
    //init server address 
    sockaddr_in   serverAddr;
    memset(&serverAddr, 0, sizeof(sockaddr_in));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(env::getPort());
    if(inet_pton(AF_INET, env::getIP().c_str(), &serverAddr.sin_addr.s_addr) != 1) {
        getlogHelper().print("init server address error");
        return -1;
    }

    //init listenfd
    int listenfd = 0;
    if ((listenfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1 ) {
        getlogHelper().print("create listenfd error");
        return listenfd;
    }
    int on = 1;
    if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) != 0) {
        getlogHelper().print("set SO_REUSEADDR error");
        return -1;
    }

    //bind listenfd to serverAddr
    if (bind(listenfd, (sockaddr*)&serverAddr, sizeof(sockaddr_in)) != 0) {
        getlogHelper().print("bind error\n");
        return -1;
    }


    //init pthread_attr_t,  set detached 
    pthread_attr_t attr;
    if (pthread_attr_init(&attr) !=0) {
        getlogHelper().print("init pthread attr error");
        return -1;
    }
    if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) != 0) {
        getlogHelper().print("set detached attr error");
        return -1;
    } 
    socklen_t len;
    if (getsockname(listenfd, (sockaddr*)&serverAddr, &len) != 0) {
        getlogHelper().print("get sockname error");
    }
    getlogHelper().print("start listen\n");
    
    char buf[20];
    inet_ntop(AF_INET, &serverAddr, buf, 100);
    printf("%s\n", inet_ntop(AF_INET, &serverAddr.sin_addr.s_addr, buf, 20));
    printf("\n %d\n",ntohs(serverAddr.sin_port));

    while(true) {
        if (listen(listenfd, MAXLISTENQUEUE) == -1) {
            getlogHelper().print("listen error");
        } 
        int clientfd = -1;
        if ((clientfd = accept(listenfd, nullptr, nullptr)) == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                getlogHelper().print("accept error");
                return -1;
            }
        }
        pthread_t tid;
        int err = 0;
        if ((err = pthread_create(&tid, &attr, workFunc, storefd(clientfd))) != 0) {
            getlogHelper().print("create thread error");
        }
    }
    return 0;
}
