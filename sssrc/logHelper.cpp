#include "logHelper.h"
#include <string>
#include <cctype>
#include <cstdlib>
#include <cstring>

using namespace std;
void logHelper::print(const string &s) {
    pthread_mutex_lock(&mutex);
    write(STDERR_FILENO, s.c_str(), s.size());
    pthread_mutex_unlock(&mutex);
}

void logHelper::print(const int &intn) {
    int n = intn;
    string output;
    if (n == 0) {
        print("0");
    }
    if (n < 0) {
        output.push_back('-');
        n = -n;
    }
    string tmp;
    while(n != 0) {
        tmp.push_back(n % 10 + '0');
        n /= 10;
    }
    for (int i = tmp.size() - 1; i != 0; --i) {
        output.push_back(tmp[i]);
    }
    print(output);
}

void logHelper::print(const long long &longn) {
    long long n = longn;
    string output;
    if (n == 0) {
        print("0");
    }
    if (n < 0) {
        output.push_back('-');
        n = -n;
    }
    string tmp;
    while(n != 0) {
        tmp.push_back(n % 10 + '0');
        n /= 10;
    }
    for (size_t i = tmp.size() - 1; i != 0; --i) {
        output.push_back(tmp[i]);
    }
    print(output);
}

void logHelper::print(const unsigned int &uin) {
    unsigned int n = uin;
    if (n == 0) {
        print("0");
    }
    string output;
    while (n != 0) {
        output.push_back(n % 10 + '0');
        n /= 10;
    }
    int tmp;
    for (unsigned i = 0; i < output.size() / 2; ++i) {
        tmp = output[i];
        output[i] = output[output.size() - 1 - i];
        output[output.size() - i - 1] = tmp;
    }
    print(output);
} 

void logHelper::print(const short &sn) {
    short n = sn;
    print(static_cast<int>(n));
}

void logHelper::print(const unsigned short &sn) {
    unsigned short n = sn;
    print(static_cast<unsigned>(n));
}


void logHelper::print(const char *msg) {
    size_t maxline = 128; 
    size_t size = 0;
    const char *p = msg;
    while (size < maxline && *p != 0) {
        ++size;
        ++p;
    }
    pthread_mutex_lock(&mutex);
    write(STDERR_FILENO, msg, size);
    pthread_mutex_unlock(&mutex);
} 


logHelper& getlogHelper() {
    static logHelper lh;
    return lh;
}
