#include "YedisRegisterCmd.h"
#include "YList.h"
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
    //string 相关命令
    { 
        std::shared_ptr<Ystring>  ptrYstring(new Ystring);

        //注册set命令
        handleCallback setHandler = std::bind(&Yedis::Ystring::setCmd, ptrYstring.get(), std::placeholders::_1, std::placeholders::_2);
        static YedisCommand retSet;
        retSet.cmd = "set";
        retSet.handler = setHandler;
        YCommand::getInstance().addCommand("set", &retSet);

        //注册get命令
        handleCallback getHandler = std::bind(&Yedis::Ystring::getCmd, ptrYstring.get(), std::placeholders::_1, std::placeholders::_2);
        static YedisCommand retGet;
        retGet.cmd = "get";
        retGet.handler = getHandler;
        YCommand::getInstance().addCommand("get", &retGet);

        //注册setnx命令
        handleCallback setnxHandler = std::bind(&Yedis::Ystring::setnxCmd, ptrYstring.get(), std::placeholders::_1, std::placeholders::_2);
        static YedisCommand retSetnx;
        retSetnx.cmd = "setnx";
        retSetnx.handler = setnxHandler;
        YCommand::getInstance().addCommand("setnx", &retSetnx);

        m_ptrYStringlist.push_back(ptrYstring);
    }
    //list相关命令
    {
        std::shared_ptr<YList> ptrYList(new YList);

        //注册lpush命令
        handleCallback lpushHandler =  std::bind(&Yedis::YList::lpush, ptrYList.get(), std::placeholders::_1, std::placeholders::_2);
        static YedisCommand retLpush;
        retLpush.cmd = "LPUSH";
        retLpush.handler = lpushHandler;
        YCommand::getInstance().addCommand("LPUSH", &retLpush);


        //注册rpush命令
        handleCallback rpushHandler =  std::bind(&Yedis::YList::rpush, ptrYList.get(), std::placeholders::_1, std::placeholders::_2);
        static YedisCommand retRpush;
        retRpush.cmd = "RPUSH";
        retRpush.handler = rpushHandler;
        YCommand::getInstance().addCommand("RPUSH", &retRpush);


        //注册lpushx命令
        handleCallback lpushxHandler =  std::bind(&Yedis::YList::lpushx, ptrYList.get(), std::placeholders::_1, std::placeholders::_2);
        static YedisCommand retLpushx;
        retLpushx.cmd = "LPUSHX";
        retLpushx.handler = lpushxHandler;
        YCommand::getInstance().addCommand("LPUSHX", &retLpushx);

        //注册rpushx命令
        handleCallback rpushxHandler =  std::bind(&Yedis::YList::rpushx, ptrYList.get(), std::placeholders::_1, std::placeholders::_2);
        static YedisCommand retRpushx;
        retRpushx.cmd = "RPUSHX";
        retRpushx.handler = rpushxHandler;
        YCommand::getInstance().addCommand("RPUSHX", &retRpushx);

        //注册lpop命令
        handleCallback lpopHandler =  std::bind(&Yedis::YList::lpop, ptrYList.get(), std::placeholders::_1, std::placeholders::_2);
        static YedisCommand retLpop;
        retLpop.cmd = "LPOP";
        retLpop.handler = lpopHandler;
        YCommand::getInstance().addCommand("LPOP", &retLpop);

        //注册rpop命令
        handleCallback rpopHandler =  std::bind(&Yedis::YList::rpop, ptrYList.get(), std::placeholders::_1, std::placeholders::_2);
        static YedisCommand retRpop;
        retRpop.cmd = "RPOP";
        retRpop.handler = rpopHandler;
        YCommand::getInstance().addCommand("RPOP", &retRpop);

        m_ptrYListlist.push_back(ptrYList);
    }
}

}
