#ifndef _YPD_YEDIS_SERVER_H_
#define _YPD_YEDIS_SERVER_H_
#include "TcpConnection.h"
#include <list>
#include <memory>
#include <string>
#include "Server.h"
class YedisServer final
{
public:
    YedisServer();
    ~YedisServer();

public:
    bool init(const char* ip, short port, EventLoop* lopp);

    void uninit();

    void onConnection(std::shared_ptr<TcpConnection> conn);

    void closeConnection(std::shared_ptr<TcpConnection> conn);

private:

    std::shared_ptr<Server>                m_ptrServer;
    std::list<std::shared_ptr<TcpSession>> m_listSessions;         
    std::string                            m_strHostPort;
    std::string                            m_strHostPort;

};
#endif //!_YPD_YEDIS_SERVER_H_
