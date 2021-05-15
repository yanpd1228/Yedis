#include "YedisRegisterCmd.h"
#include "YHash.h"
#include "YSet.h"
#include "YList.h"
#include "YSortedSet.h"
#include "YString.h"
#include "YedisCommand.h"
#include "YedisCommon.h"
#include "YedisStore.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>

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

YError YedisRegisterCmd::command(const std::vector<std::string> &params, ReplyBuffer *reply)
{
    if(params[0] == "COMMAND")
    {
        std::string strReply = "+OK\r\n";
        reply->pushData(strReply.c_str(), strReply.size());
    }
    return YError_ok;
}

void YedisRegisterCmd::init()
{
    {
        //注册comand命令
        handleCallback commandHandler = std::bind(&Yedis::YedisRegisterCmd::command, this, std::placeholders::_1, std::placeholders::_2);
        static YedisCommand retCommand;
        retCommand.cmd = "COMMAND";
        retCommand.handler = commandHandler;
        YCommand::getInstance().addCommand("COMMAND", &retCommand);
    }
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

        //注册llen命令
        handleCallback llenHandler =  std::bind(&Yedis::YList::llen, ptrYList.get(), std::placeholders::_1, std::placeholders::_2);
        static YedisCommand retLlen;
        retLlen.cmd = "LLEN";
        retLlen.handler = llenHandler;
        YCommand::getInstance().addCommand("LLEN", &retLlen);

        //注册lrange命令
        handleCallback lrangeHandler =  std::bind(&Yedis::YList::lrange, ptrYList.get(), std::placeholders::_1, std::placeholders::_2);
        static YedisCommand retLrange;
        retLrange.cmd = "LRANGE";
        retLrange.handler = lrangeHandler;
        YCommand::getInstance().addCommand("LRANGE", &retLrange);

        m_ptrYListlist.push_back(ptrYList);
    }

    //set 相关命令
    {
        std::shared_ptr<YSet> ptrYSet(new YSet);
        //注册sadd命令
        handleCallback saddHandler =  std::bind(&Yedis::YSet::sadd, ptrYSet.get(), std::placeholders::_1, std::placeholders::_2);
        static YedisCommand retSetAdd;
        retSetAdd.cmd = "SADD";
        retSetAdd.handler = saddHandler;
        YCommand::getInstance().addCommand("SADD", &retSetAdd);
        
        //注册scard命令
        handleCallback scardHandler =  std::bind(&Yedis::YSet::scard, ptrYSet.get(), std::placeholders::_1, std::placeholders::_2);
        static YedisCommand retSetScard;
        retSetScard.cmd = "SCARD";
        retSetScard.handler = scardHandler;
        YCommand::getInstance().addCommand("SCARD", &retSetScard);

        //注册smember命令
        handleCallback smemberHandler =  std::bind(&Yedis::YSet::smember, ptrYSet.get(), std::placeholders::_1, std::placeholders::_2);
        static YedisCommand retSetSmember;
        retSetSmember.cmd = "SMEMBERS";
        retSetSmember.handler = smemberHandler;
        YCommand::getInstance().addCommand("SMEMBERS", &retSetSmember);

        //注册spop命令
        handleCallback spopHandler =  std::bind(&Yedis::YSet::spop, ptrYSet.get(), std::placeholders::_1, std::placeholders::_2);
        static YedisCommand retSetSpop;
        retSetSpop.cmd = "SPOP";
        retSetSpop.handler = spopHandler;
        YCommand::getInstance().addCommand("SPOP", &retSetSpop);

        m_ptrYSetlist.push_back(ptrYSet);
    }
    //HASH相关命令
    {
        std::shared_ptr<YHash> ptrHash(new YHash);
        //注册hget命令
        handleCallback hgetHandler =  std::bind(&Yedis::YHash::hget, ptrHash.get(), std::placeholders::_1, std::placeholders::_2);
        static YedisCommand HashHget;
        HashHget.cmd = "HGET";
        HashHget.handler = hgetHandler;
        YCommand::getInstance().addCommand("HGET", &HashHget);

        //注册hset命令
        handleCallback hsetHandler =  std::bind(&Yedis::YHash::hset, ptrHash.get(), std::placeholders::_1, std::placeholders::_2);
        static YedisCommand HashHset;
        HashHset.cmd = "HSET";
        HashHset.handler = hsetHandler;
        YCommand::getInstance().addCommand("HSET", &HashHset);
        m_ptrYHashlist.push_back(ptrHash);
    }
    //sortedset相关命令
    {
        std::shared_ptr<YSortedSet> ptrYSortedSet;
        //注册zadd命令
        handleCallback zaddHandler =  std::bind(&Yedis::YSortedSet::zadd, ptrYSortedSet.get(), std::placeholders::_1, std::placeholders::_2);
        static YedisCommand SortedZadd;
        SortedZadd.cmd = "ZADD";
        SortedZadd.handler = zaddHandler;
        YCommand::getInstance().addCommand("ZADD", &SortedZadd);

        m_ptrYSortedSetlist.push_back(ptrYSortedSet); 
    }
}

}
