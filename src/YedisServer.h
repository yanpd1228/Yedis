#ifndef _YPD_YEDIS_SERVER_H_
#define _YPD_YEDIS_SERVER_H_
#include "TcpConnection.h"
#include <list>
#include <memory>
#include <string>
#include "Server.h"
#include "YedisSession.h"
class YedisServer final
{
public:
    YedisServer();
    ~YedisServer();

public:
    bool init(const std::string ip, const std::string port, EventLoop* loop);

    void uninit();

    void onConnection(std::shared_ptr<TcpConnection> conn);

    void closeConnection(std::shared_ptr<TcpConnection> conn);

private:

    std::shared_ptr<Server>                  m_ptrServer;
    std::list<std::shared_ptr<YedisSession>> m_listSessions;         
    std::string                              m_strHostPort;
    std::mutex                               m_mutexForSession; 

};
#endif //!_YPD_YEDIS_SERVER_H_
