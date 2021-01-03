#pragma once
#ifndef _YPD_AcceptNew_H_
#define _YPD_AcceptNew_H_
#include <functional>

#include "Channel.h"
#include "Sockets.h"
#include "SocketAddr.h"

class AcceptNew : public Sockets
{
public:
	typedef std::function<void(int sockfd, SocketAddr&)> newConnectionCallback;

	AcceptNew(EventLoop* loop, const SocketAddr& listenAddr, bool reuseport);
	~AcceptNew();

	/* 设置新连接到来的回调函数 */
	void setNewConnectionCallback(const newConnectionCallback& cb)
	{
		m_newConnectionCallback = cb;
	}

	bool listenning() const { return m_bListenning; }
	void listen();

private:
	void handleRead();

private:
	std::unique_ptr<EventLoop>             m_ptrLoop;
	SOCKET                                 m_acceptSocket;
	std::unique_ptr<Channel>               m_ptrAcceptChannel;
	newConnectionCallback                  m_newConnectionCallback;
	bool                                   m_bListenning;
    Sockets                                m_CSockets;

#ifndef _WIN32
	int                   idleFd_;
#endif

};

#endif //!_YPD_AcceptNew_H_
