//
//global envrinoments
//

#ifndef _ENV_
#define _ENV_

#include <string>
#include <cstdint>

class config;
class env {
    friend class config;
public:
    env() = delete;
    static int16_t getPort() { return port; }
    static std::string getMainPath() { return mainPath; }
    static std::string getIP() { return ip; }

    static std::string serverInfo;
    static std::string defaultPage;
private:
    static int16_t port;
    static std::string mainPath;
    static std::string ip;
};
#endif
