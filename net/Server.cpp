#include "Server.h"
#include "AcceptNew.h"
#include "Channel.h"
#include "EventLoop.h"
#include "TcpConnection.h"
#include "EventLoopThreadPool.h"
#include <cstdlib>
#include <functional>
#include <memory>
#include <string>



void defautlConnectionCallback(const TcpConnectionPtr& conn)
{
    std::string strWelcomeMessage = "Welcome to Yedis.";
    conn->send(strWelcomeMessage.c_str(), strWelcomeMessage.length());
}

void defaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buff)
{
    buff->retrieveAll();
}

Server::Server(EventLoop* loop,
		const SocketAddr& listenAddr,
		const std::string& nameArg,
		Option option)
{
    m_nStarted = 0;
    m_ptrAccepNewConn.reset(new AcceptNew(loop,listenAddr,option));
    m_ptrAccepNewConn->setNewConnectionCallback((std::bind(&Server::NewConnection, this, std::placeholders::_1, std::placeholders::_2)));
    m_ptrLoop = loop;
    unsigned short shPort = listenAddr.GetPort();
    m_strHostPort = std::to_string(shPort);
    m_strName = nameArg;
    m_NewConnectionCallback = defautlConnectionCallback;
    m_MessageCallback = defaultMessageCallback;

}

Server::~Server()
{

}
    

void Server::Start(int nWorkThreadCount)
{
    if(m_nStarted == 0)
    {
        m_ptrEventLoopThreadPool.reset(new EventLoopThreadPool());
        m_ptrEventLoopThreadPool->Init(m_ptrLoop,nWorkThreadCount);
        m_ptrEventLoopThreadPool->Start();

        m_ptrLoop->runInLoop(std::bind(&AcceptNew::listen,m_ptrAccepNewConn.get()));
        m_nStarted = 1;
    }
}

void Server::NewConnection(int nSockfd, SocketAddr &peerAdder)
{
    EventLoop* nextLoop = m_ptrEventLoopThreadPool->getNextLoop();
    TcpConnectionPtr newConnection(new TcpConnection(nextLoop, nSockfd));
    newConnection->setConnectionCallBack(m_NewConnectionCallback);
    newConnection->setMessageCallBack(m_MessageCallback);
    newConnection->setCloseCallback(std::bind(&Server::removeConnection, this, std::placeholders::_1));
    newConnection->setWriteCompleteCallback(m_WriteCompleteCallback);
    nextLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, newConnection));
}


void Server::removeConnection(const TcpConnectionPtr& conn)
{
    m_ptrLoop->runInLoop(std::bind(&Server::removeConnectionInLoop,this, conn));    
}

void Server::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
    size_t n = m_mapConnections.erase(conn->name());
    if(n != 1)
    {
        return;
    }

    EventLoop* loop = conn->getLoop();
    loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}




