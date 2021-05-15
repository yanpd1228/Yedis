#include "YSet.h"
#include "YedisStore.h"
#include "YedisFormate.h"
#include <unordered_set>
#include <vector>


namespace Yedis{

YSet::YSet()
{

}

YSet::~YSet()
{

}


YObject* YSet::getSet(const std::string& strSetName, ReplyBuffer* reply)
{
    YObject* value;
    YError err = YSTORE.getValueByType(strSetName, value, YType_set);
    if(err != YError_ok)
    {
        if(err == YError_notExist)
        {
            Yedis::YedisFormate::formatNull(reply);
        }
        else
        {
            LOGD("error type");
            reply->pushData("-ERR error type\r\n", sizeof("-ERR error type") -1);
        }
        return nullptr;
    }
    return value;
}

YObject* YSet::creatrOrGetSet(const std::string& strSetName, ReplyBuffer* reply)
{
    YObject* value;
    YError err = YSTORE.getValueByType(strSetName, value, YType_set);
    if (err != YError_ok && err != YError_notExist)
    {
        LOGD("error type");
        reply->pushData("-ERR error type\r\n", sizeof("-ERR error type") -1);
        return nullptr;
    }
    if (err == YError_notExist)
    {
        value = YSTORE.setValue(strSetName, YObject::createSet());
    }

    return value;
}

YError YSet::sadd(const std::vector<std::string>& params, ReplyBuffer* reply)
{
    YObject* value = creatrOrGetSet(params[1], reply);
    if(value == nullptr)
    {
        reply->pushData("-ERR error type\r\n", sizeof("-ERR error type") -1);
        return YError_type;
    }
    int res = 0;
    auto set = value->castSet();
    for (std::size_t i = 2; i < params.size(); ++ i)
    {
        if (set->insert(params[i]).second)
        {
            ++res;
        }
    }
    
    Yedis::YedisFormate::formatInt(res, reply);
    return YError_ok;
}

YError YSet::scard(const std::vector<std::string>& params, ReplyBuffer* reply)
{
    YObject* value = getSet(params[1], reply);
    if(value == nullptr)
    {
        reply->pushData("-ERR error type\r\n", sizeof("-ERR error type") -1);
        return YError_type;
    }
    auto set = value->castSet();
    long size = static_cast<long>(set->size());
    
    Yedis::YedisFormate::formatInt(size, reply);
    return YError_ok;
}

YError YSet::smember(const std::vector<std::string>& params, ReplyBuffer* reply)
{
    YObject* value = getSet(params[1], reply);
    if(value == nullptr)
    {
        reply->pushData("-ERR error type\r\n", sizeof("-ERR error type") -1);
        return YError_type;
    }

    auto set = value->castSet();
    Yedis::YedisFormate::preFormatMultiBulk(set->size(), reply);
    for (const auto& member : *set)
    {
        Yedis::YedisFormate::formatBulk(member, reply);
    }
    return YError_ok;
}


YError YSet::spop(const std::vector<std::string>& params, ReplyBuffer* reply)
{

    YObject* value = getSet(params[1], reply);
    if(value == nullptr)
    {
        reply->pushData("-ERR error type\r\n", sizeof("-ERR error type") -1);
        return YError_type;
    }

    auto set = value->castSet();
    std::string res;
    std::unordered_set<std::string>::const_local_iterator it = Yedis::randomHashMember(*set);
    if (it != std::unordered_set<std::string>::const_local_iterator())
    {
        res = *it; 
        Yedis::YedisFormate::formatBulk(res, reply);
        set->erase(res);
        if (set->empty())
        {
            YSTORE.deleteKey(params[1]);
        }
        
    }
    else
    {
        Yedis::YedisFormate::formatNull(reply);
        return YError_notExist;
    }
    
    return YError_ok;
}

}
