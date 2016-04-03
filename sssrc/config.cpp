#include "config.h"
#include "env.h"

using namespace std;
int config::load_conf() {
    return load_conf("./conf/configure");
}

int config::load_conf(string path) {
    path = "";
    env::port = 8080;
    env::ip = "10.103.240.188";    
    env::mainPath = "/Users/soultuanz/project/tinyHttpServer";
    env::defaultPage = "/index.html";
    return 0;
}
