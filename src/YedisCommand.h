#ifndef _YEDIS_COMMAND_H_
#define _YEDIS_COMMAND_H_
#include <functional>
#include <string>
#include <vector>
#include <list>
#include "YedisStore.h"
#include "../base/ReplyBuffer.h"


namespace Yedis
{
typedef std::function<Yedis::YError(const std::vector<std::string>& vecParam,ReplyBuffer* reply)> handleCallback;

struct YedisCommand
{
    std::string             cmd;
    handleCallback          handler;
};

class YCommand
{
public:
    YCommand(const YCommand&)=delete;
    YCommand& operator=(const YCommand&)=delete;
    static YCommand& getInstance()
    {
        static YCommand instance;
        return instance;
    }
    YCommand();
public:
    bool addCommand(const std::string& cmd, const YedisCommand* command);
    bool delCommand(const std::string& cmd);
    bool execCommand(std::vector<std::string>& params, ReplyBuffer& replyBuffer);
    bool existCommand(const std::string& cmd);
    const YedisCommand* getCommand(const std::string& cmd);
private:
    std::unordered_map<std::string, const YedisCommand*> m_cmdInfo;
    std::list<YedisCommand>                              m_listStoreCmd;
};

}
#endif //!_YEDIS_COMMAND_H_
