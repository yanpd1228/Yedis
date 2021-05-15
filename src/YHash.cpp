#include "YHash.h"
#include "YedisFormate.h"
#include "YedisStore.h"
#include "../base/AsyncLog.h"
#include <unordered_map>
#include <vector>

namespace Yedis
{

YHash::YHash()
{

}

YHash::~YHash()
{

}


YError YHash::hset(const std::vector<std::string>& params, ReplyBuffer* reply)
{
    YObject* value = getOrCreateHash(params[1], reply);
    if(value == nullptr)
    {
        return YError_type;
    }
    auto hash = value->castHash();
    std::string key = params[2];
    std::string val = params[3];
    auto it(hash->find(key));
    if (it != hash->end())
    {
        it->second = val;
    }
    else
    {
        it = hash->insert(std::unordered_map<std::string, std::string>::value_type(key, val)).first;
    }
    Yedis::YedisFormate::formatInt(1, reply);
    return YError_ok;

}

YError YHash::hget(const std::vector<std::string>& params, ReplyBuffer* reply)
{
    YObject* value = getHash(params[1], reply);
    if(value == nullptr)
    {
        return YError_type;
    }
    auto hash= value->castHash();
    auto it = hash->find(params[2]);

    if (it != hash->end())
    {
        YedisFormate::formatBulk(it->second, reply);
    }
    else
    {
        YedisFormate::formatNull(reply);
    }

    return YError_ok;
}

YObject* YHash::getHash(const std::string& strHashName, ReplyBuffer* reply)
{
    YObject* value;
    YError err = YSTORE.getValueByType(strHashName, value, YType_hash);
    if (err != YError_ok)
    {
        LOGD("error type");
        reply->pushData("-ERR error type\r\n", sizeof("-ERR error type") -1);
        return nullptr; 
    }
    return value;
}

YObject* YHash::getOrCreateHash(const std::string& strHashName, ReplyBuffer* reply)
{
    YObject* value;
    YError err = YSTORE.getValueByType(strHashName, value, YType_hash);
    if (err != YError_ok && err != YError_notExist)
    {
        LOGD("error type");
        reply->pushData("-ERR error type\r\n", sizeof("-ERR error type") -1);
        return nullptr; 
    }
    if(err == YError_notExist)
    {
        value = YSTORE.setValue(strHashName, YObject::createHash());
    }
    return value;
}

}
