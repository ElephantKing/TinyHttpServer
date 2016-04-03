#ifndef _LOG_HELPER_
#define _LOG_HELPER_

#include <unistd.h>
#include <pthread.h>
#include <string>
#include <cstring>

class logHelper { 
public:
    logHelper() {
        pthread_mutex_init(&mutex, nullptr);
    }
    ~logHelper() {
        pthread_mutex_destroy(&mutex);
    }
    template<typename ...Args>
        void println(const Args&...);
    template<typename T, typename ...Args>
        void print(const T&, const Args& ...);
    template<typename T>
        void print(const T&);
    void print(const char*);
    void print(const int&);
    void print(const long long&);
    void print(const short&);
    void print(const unsigned int&);
    void print(const unsigned short&);
    void print(const std::string&);
private:
    pthread_mutex_t mutex;
};


template<typename ...Args>
void logHelper::println(const Args&... args) {
    print(args...);
    print("\n");
}

template <typename T>
void logHelper::print(const T&) {
    char msg[] = "unknow type to print";
    pthread_mutex_lock(&mutex);
    write(STDERR_FILENO, msg, strlen(msg));
    pthread_mutex_unlock(&mutex);
}
template <typename T, typename ...Args>
void logHelper::print(const T& t, const Args&...args) {
    print(t);
    print(args...);
}
logHelper& getlogHelper();
#endif
