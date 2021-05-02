#ifndef _YPD_YEDIS_SESSION_H_
#define _YPD_YEDIS_SESSION_H_
#include "Buffer.h"
#include <memory>
#include "Buffer.h"
#include "TcpConnection.h"
class YedisSession final
{
public:
    YedisSession(std::weak_ptr<TcpConnection> conn);
    ~YedisSession();
public:
    void onRead(const std::shared_ptr<TcpConnection>& conn, Buffer* buffer);
    void send(const std::string strMessage);
private:
    void sendPackage(const char* p, int32_t length);
private:
    std::weak_ptr<TcpConnection>    m_weakPtrTempConn;
};

#endif //!_YPD_YEDIS_SESSION_H_
