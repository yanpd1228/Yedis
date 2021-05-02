#include "YedisSession.h"
#include <memory>
#include <vector>
#include "Buffer.h"
#include "YedisParser.h"
#include "YString.h"
#include "YedisRegisterCmd.h"
#include "YedisStore.h"
#include "YedisCommand.h"
#include "../base/ReplyBuffer.h"

YedisSession::YedisSession(std::weak_ptr<TcpConnection> conn) : m_weakPtrTempConn(conn)
{

}

YedisSession::~YedisSession()
{

}


void YedisSession::onRead(const std::shared_ptr<TcpConnection>& conn, Buffer* buffer)
{
    const char* ptr = buffer->peek();
    const char* end = ptr + buffer->readableBytes();
    std::string strInputBuffer;
    strInputBuffer.append(buffer->peek(),buffer->readableBytes());
    buffer->retrieve(buffer->readableBytes());
    Yedis::YProtoParse parse;
    parse.parseRequest(ptr, end);
    std::vector<std::string> vecRes = parse.getParams();
    if(vecRes.size() == 3 || vecRes.size() == 2)
    {
        ReplyBuffer replyBuffer;
        //Yedis::YedisRegisterCmd cmd = Yedis::YedisRegisterCmd::getInstance();
        Yedis::YCommand::getInstance().execCommand(vecRes, replyBuffer);
        sendPackage(replyBuffer.readAddr(), replyBuffer.readableSize());
    }
    else
    {
        send("+OK\r\n");
    }
}

void YedisSession::send(const std::string strMessage)
{
    sendPackage(strMessage.c_str(), strMessage.length());
}

void YedisSession::sendPackage(const char* p, int32_t length)
{   
    std::string srcbuf(p, length);

    if (m_weakPtrTempConn.expired())
    {
        return;
    }

    std::shared_ptr<TcpConnection> conn = m_weakPtrTempConn.lock();
    if (conn)
    {
        conn->send(srcbuf);
    }
}

