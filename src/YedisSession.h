#ifndef _YPD_YEDIS_SESSION_H_
#define _YPD_YEDIS_SESSION_H_
#include "Buffer.h"
#include <memory>
#include "Buffer.h"
#include "TcpConnection.h"
class YedisSession final
{
public:
    YedisSession(std::shared_ptr<TcpConnection> conn);
    ~YedisSession();
public:
    void OnRead(const std::shared_ptr<TcpConnection>& conn, Buffer* buffer);
private:
    bool decodeMessage(const std::string& strRecv);
    void Split(const std::string& str, std::vector<std::string>& v, const char* delimiter);
};

#endif //!_YPD_YEDIS_SESSION_H_
