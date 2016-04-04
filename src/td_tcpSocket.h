#include "td_socket.h"

namespace td {

class tcpSocket : public socket {
public:
	int abort();
	int connect(tcpSocket);
};

}
