#ifndef _TINY_CALLBACKS_H_
#define _TINY_CALLBACKS_H_

#include <memory>
#include <functional>

#include "../base/Timestamp.h"
namespace tiny {

using std::shared_ptr;
using std::function;

class Buffer;
class TcpConnection;
typedef shared_ptr<TcpConnection> TcpConnectionPtr;
typedef function<void()> TimerCallback;
typedef function<void (const TcpConnectionPtr&)> ConnectionCallback;
typedef function<void (const TcpConnectionPtr&)> CloseCallback;
typedef function<void (const TcpConnectionPtr&)> WriteCompleteCallback;
typedef function<void (const TcpConnectionPtr&, size_t)> HighWaterMarkCallback;

typedef function<void (const TcpConnectionPtr&,
					   Buffer*,
					   Timestamp)> MessageCallback;

void defaultConnectionCallback(const TcpConnectionPtr& conn);
void defaultMessageCallback(const TcpConnectionPtr& conn,
							Buffer *buff,
							Timestamp receiveTime);
}
#endif

