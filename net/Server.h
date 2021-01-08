#ifndef _YPD_SERVER_H_
#define _YPD_SERVER_H_
#include <functional>
#include "SocketAddr.h"
#include "TcpConnection.h"
#include <atomic>
#include <memory>
#include <map>
class AcceptNew;
class EventLoop;
class EventLoopThreadPool;

class Server
{
public:
	enum Option
	{
		kNoReusePort,
		kReusePort,
	};

	Server(EventLoop* loop,
		const SocketAddr& listenAddr,
		const std::string& nameArg,
		Option option = kReusePort);
	~Server();

public:
    void setConnectionCallback(const ConnectionCallback& cb){m_NewConnectionCallback = cb; }
	const std::string& hostport() const { return m_strHostPort; }
	const std::string& name() const { return m_strName; }
	EventLoop* getLoop() const { return m_ptrLoop; }
    typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;
    void start(int nWorkThreadCount);
    void newConnection(int nSockfd, SocketAddr& peerAdder);
    void removeConnection(const TcpConnectionPtr& conn);
    void removeConnectionInLoop(const TcpConnectionPtr& conn);
private:
	EventLoop*                                      m_ptrLoop;  // the acceptor loop
	std::string                                     m_strHostPort;
	std::string                                     m_strName;
	std::unique_ptr<AcceptNew>                      m_ptrAccepNewConn;
	std::unique_ptr<EventLoopThreadPool>            m_ptrEventLoopThreadPool;
	ConnectionCallback                              m_NewConnectionCallback;
	MessageCallback                                 m_MessageCallback;
	WriteCompleteCallback                           m_WriteCompleteCallback;
	ThreadInitCallback                              m_ThreadInitCallback;
	std::atomic<int>                                m_nStarted;
	int                                             m_nNextConnId;
	ConnectionMap                                   m_mapConnections;
};

#endif //!_YPD_SERVER_H_
