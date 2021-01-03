#include "TcpConnection.h"
#include "Poller.h"
#include <bits/c++config.h>

TcpConnection::TcpConnection(EventLoop* loop, int sockfd)
{
    m_name = "";
    m_ptrLoop = loop;
    m_State = kConnecting;
    m_ptrChannel.reset(new Channel(loop, sockfd));
    
}

TcpConnection::~TcpConnection()
{

}

void TcpConnection::connectEstablished()
{
    if(m_State != kConnecting)
    {
        return;
    }
    setState(kConnected);
    m_ptrChannel->tie(shared_from_this());
    if(!m_ptrChannel->enableReading())
    {
        handleClose();
        return;
    }

    m_NewConnectionCallback(shared_from_this());
}

void TcpConnection::connectDestroyed()
{
    if(m_State == kConnected)
    {
        setState(kDisconnected);
        m_ptrChannel->disableAll();
        m_NewConnectionCallback(shared_from_this());
    }
    m_ptrChannel->remove();
}

void TcpConnection::handleClose()
{
    if(m_State == kDisconnected)
    {
        return;
    }

    setState(kDisconnected);
    m_ptrChannel->disableAll();

    TcpConnectionPtr callBack(shared_from_this());
    m_NewConnectionCallback(callBack);
    m_CloseCallback(callBack);
}

void TcpConnection::send(const void* data, int nLength)
{
    if(m_State == kConnected)
    {
        if(m_ptrLoop->isInLoopThread())
        {
            sendInLoop(data,nLength);
        }
        else
        {

        }
    }
}

void TcpConnection::sendInLoop(const std::string& strMessage)
{
    sendInLoop(strMessage.c_str(),strMessage.length());
}

void TcpConnection::sendInLoop(const void* data, std::size_t nLength)
{
    m_ptrLoop->assertInLoopThread();
    int32_t nWrote = 0;
    std::size_t remaining = nLength;
    bool bFaultError = false;
    if (m_State == kDisconnected)
    {
        return;
    }
    nWrote = m_ptrSocket->Write(m_ptrChannel->fd(), data, nLength); 
}
