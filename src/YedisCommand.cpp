#include "YedisCommand.h"
#include <utility>
#include <vector>

namespace Yedis
{

YCommand::YCommand()
{

}


bool YCommand::addCommand(const std::string& cmd, const YedisCommand* command)
{
    if(cmd.empty())
    {
        return true;
    }

    return m_cmdInfo.insert(std::make_pair(cmd,command)).second;
}

bool YCommand::delCommand(const std::string& cmd)
{
    auto it = m_cmdInfo.find(cmd);
    if(it != m_cmdInfo.end())
    {
        m_cmdInfo.erase(it);
        return true;
    }
    return false;
}

bool YCommand::execCommand(std::vector<std::string>& params, ReplyBuffer& replyBuffer)
{
    const YedisCommand* temp = getCommand(params[0]);
    if (!temp)
    {
        size_t oldSize = replyBuffer.readableSize();
        std::string strError = "-ERR Unknown command\r\n";
        replyBuffer.pushData(strError.c_str(), strError.size());
        replyBuffer.readableSize() - oldSize;
        return false;
    }
    temp->handler(params, &replyBuffer);
    return true;
}

bool YCommand::existCommand(const std::string& cmd)
{
    auto it = m_cmdInfo.find(cmd);
    if(it != m_cmdInfo.end())
    {
        return true;
    }
    return false;
}


const YedisCommand* YCommand::getCommand(const std::string& cmd)
{
    auto it = m_cmdInfo.find(cmd);
    if(it != m_cmdInfo.end())
    {
        return it->second;
    }
    return nullptr;
}
}
