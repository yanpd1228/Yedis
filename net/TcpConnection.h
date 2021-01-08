#ifndef _YPD_TCP_CONNECTION_H_
#define _YPD_TCP_CONNECTION_H_
#include <bits/c++config.h>
#include <thread>
#include <string>
#include "SocketAddr.h"
#include "Sockets.h"
#include "Channel.h"
#include "../base/Callbacks.h"
#include "../base/Buffer.h"
using namespace net;
class EventLoop;
class TcpConnection;
typedef std::function<void(EventLoop*)> ThreadInitCallback;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void()> TimerCallback;
typedef std::function<void(const TcpConnectionPtr&, Buffer*)> MessageCallback;
typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallback;
typedef std::function<void(const TcpConnectionPtr&)> CloseCallback;
typedef std::function<void(const TcpConnectionPtr&)> WriteCompleteCallback;
class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{

public:
	TcpConnection(EventLoop* loop, int sockfd);
	~TcpConnection();
    void setConnectionCallBack(const ConnectionCallback& cb)
    {
        m_NewConnectionCallback = cb;
    }

    void setMessageCallBack(const MessageCallback& cb)
    {
        m_MessageCallback = cb;
    }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb)
    {
        m_WriteCompleteCallback = cb;
    }
    void setCloseCallback(const CloseCallback& cb)
    {
        m_CloseCallback = cb;
    }

private:
	enum State { kDisconnected, kConnecting, kConnected, kDisconnecting };

public:
	const SocketAddr& localAddress() const { return m_LocalAddr; }
	const SocketAddr& peerAddress() const { return m_PeerAddr; }
	bool connected() const { return m_State == kConnected; }
	std::string getTcpInfoString() const;
    std::string name() {return m_name;}
    EventLoop* getLoop(){return m_ptrLoop;}
    void setState(State s){m_State = s;}
    void connectEstablished();
    void connectDestroyed();
    bool Conncted(){return m_State == kConnected;}
    void send(const void* data,int nLength);
private:
    void sendInLoop(const std::string& strMessage);
    void sendInLoop(const void* data, std::size_t nLength);
    void handleClose();    
    void handleRead();
    void handleWrite();
private:
	EventLoop*                  m_ptrLoop;
    std::string                 m_name;
	State                       m_State;
	std::unique_ptr<Sockets>    m_ptrSocket;
	std::shared_ptr<Channel>    m_ptrChannel;
	const SocketAddr            m_LocalAddr;
	const SocketAddr            m_PeerAddr;
	ConnectionCallback          m_NewConnectionCallback;
	MessageCallback             m_MessageCallback;
	WriteCompleteCallback       m_WriteCompleteCallback;
	//HighWaterMarkCallback       m_HighWaterMarkCallback;
	CloseCallback               m_CloseCallback;
	size_t                      m_nHighWaterMark_;
	Buffer                      m_InputBuffer;
	Buffer                      m_OutputBuffer;
};
#endif
