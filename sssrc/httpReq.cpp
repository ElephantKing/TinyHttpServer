#include "httpReq.h"

using namespace std;

void httpReq::parseURL() {
    //////      simple parse    ////////
    if (url.size() == 0) {
        type = INVALID;
        return;
    }
    for (string::size_type i = 0; i < url.size(); ++i) {
        if (url[i] == '?') {
            exType = CGI;
            return;
        }
    }
    return;
}
