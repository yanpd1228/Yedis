#include "YedisRegisterCmd.h"
#include "YString.h"
#include "YedisCommand.h"
#include "YedisStore.h"
#include <functional>
#include <string>

namespace Yedis
{
YedisRegisterCmd::YedisRegisterCmd()
{
    Yedis::YSTORE.init(0);
    Yedis::YSTORE.selectDB(0);
}

YedisRegisterCmd::~YedisRegisterCmd()
{

}

void YedisRegisterCmd::init()
{
    
    std::shared_ptr<Ystring>  ptrYstring(new Ystring);
    handleCallback setHandler = std::bind(&Yedis::Ystring::setCmd, ptrYstring.get(), std::placeholders::_1, std::placeholders::_2);
    static YedisCommand retSet;
    retSet.cmd = "set";
    retSet.handler = setHandler;
    YCommand::getInstance().addCommand("set", &retSet);
    handleCallback getHandler = std::bind(&Yedis::Ystring::getCmd, ptrYstring.get(), std::placeholders::_1, std::placeholders::_2);
    static YedisCommand retGet;
    retGet.cmd = "get";
    retGet.handler = getHandler;
    YCommand::getInstance().addCommand("get", &retGet);
    m_list.push_back(ptrYstring);
}

}
