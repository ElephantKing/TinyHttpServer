#ifndef _CONFIG_
#define _CONFIG_

#include <string>

class config {
public:
    static int load_conf();
    static int load_conf(std::string path);
};
#endif
