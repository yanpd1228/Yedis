#include "YedisServer.h"
#include "SocketAddr.h"
#include "YedisSession.h"
#include <memory>

YedisServer::YedisServer()
{

}

YedisServer::~YedisServer()
{

}

bool YedisServer::init(const std::string strHost, const std::string strPort, EventLoop *lopp)
{
    std::string strHostPort = strHost + ":" + strPort;
    SocketAddr sockaddr(strHostPort);
    
    m_ptrServer.reset(new Server(loop, sockaddr, "YedisServer"))
    m_ptrServer->setConnectionCallBack(std::bind(&YedisServer::onConnection,this,std::placeholders::_1));
    m_ptrServer->Start(6);
    unsigned int threadCount = std::thread::hardware_concurrency() + 2;
    m_ptrServer->Start(threadCount);
    return true;
}


void YedisServer::uninit()
{

}

void YedisServer::onConnection(std::shared_ptr<TcpConnection> conn)
{
    if(conn->onConneted())
    {
        std::shared_ptr<YedisSession> ptrYedisSession = std::make_shared<YedisSession>(conn);
        conn->setMessageCallback(std::bind(&YedisSession::OnRead,ptrYedisSession.get(),std::placeholders::_1,
                    std::placeholders::_2));
    }

}

void YedisServer::closeConnection(std::shared_ptr<TcpConnection> conn)
{

}
