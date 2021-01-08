#include "TcpConnection.h"
#include "Poller.h"
#include <functional>
#include <memory>

TcpConnection::TcpConnection(EventLoop* loop, int sockfd)
{
    m_name = "";
    m_ptrLoop = loop;
    m_State = kConnecting;
    m_ptrChannel = std::make_shared<Channel>(loop, sockfd);
    m_ptrChannel->setReadCallback(std::bind(&TcpConnection::handleRead, this));
    m_ptrChannel->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    m_ptrChannel->setCloseCallback(std::bind(&TcpConnection::handleClose, this));
    m_ptrChannel->setErrorCallback(std::bind(&TcpConnection::handleClose,this));
    
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
    //m_ptrChannel->tie(shared_from_this());
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

void TcpConnection::handleRead()
{
    m_ptrLoop->assertInLoopThread();
    int nReadError = 0;
    int32_t nRead = m_InputBuffer.readFd(m_ptrChannel->fd(), &nReadError);
    if(nRead > 0)
    {
        m_MessageCallback(shared_from_this(), &m_InputBuffer);
    }
}

void TcpConnection::handleWrite()
{
    m_ptrLoop->assertInLoopThread();
    if(m_ptrChannel->isWriting())
    {
        int32_t nWrite = m_ptrSocket->Write(m_ptrChannel->fd(),m_OutputBuffer->peek(),m_OutputBuffer->readableBytes());

    }
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
            std::string strMessage(static_cast<const char*>(data),nLength);
            m_ptrLoop->runInLoop(
                std::bind(static_cast<void (TcpConnection::*)(const string&)>(&TcpConnection::sendInLoop),
                    this,   
                    strMessage));
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
