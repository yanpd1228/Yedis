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

bool YedisServer::init(const std::string strHost, const std::string strPort, EventLoop *loop)
{
    std::string strHostPort = strHost + ":" + strPort;
    SocketAddr sockaddr(strHostPort);
    
    m_ptrServer.reset(new Server(loop, sockaddr, "YedisServer"));
    m_ptrServer->setConnectionCallback(std::bind(&YedisServer::onConnection,this,std::placeholders::_1));
    unsigned int threadCount = std::thread::hardware_concurrency() + 2;
    m_ptrServer->start(threadCount);
    return true;
}


void YedisServer::uninit()
{

}

void YedisServer::onConnection(std::shared_ptr<TcpConnection> conn)
{
    if(conn->connected())
    {
        std::shared_ptr<YedisSession> ptrYedisSession = std::make_shared<YedisSession>(conn);
        conn->setMessageCallBack(std::bind(&YedisSession::onRead,ptrYedisSession.get(),std::placeholders::_1,std::placeholders::_2));
        {
            std::lock_guard<std::mutex> guard(m_mutexForSession);
            m_listSessions.push_back(ptrYedisSession);
        }
    }

}

void YedisServer::closeConnection(std::shared_ptr<TcpConnection> conn)
{

}
