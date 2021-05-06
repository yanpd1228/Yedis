#include "YList.h"
#include "YedisFormate.h"
#include <cstdlib>
#include <string>
#include <cassert>

namespace Yedis
{
YList::YList()
{

}

YList::~YList()
{
}

YError YList::push(const std::vector<std::string>& params, ReplyBuffer* reply, ListPosition pos, bool createIfNotExist/* = true*/)
{
    YObject* value;
    
    YError err = YSTORE.getValueByType(params[1], value, YType_list);
    if (err != YError_ok)
    {
        if (err != YError_notExist)
        {
            std::string strError = "-ERR no such key\r\n";
            size_t oldSize = reply->readableSize();
            reply->pushData(strError.c_str(), strError.size());
            reply->readableSize() - oldSize;
            return err;
        }
        else if (createIfNotExist)
        {
            value = YSTORE.setValue(params[1], YObject::createList());
        }
        else
        {
            std::string strError = "-ERR no such key\r\n";
            size_t oldSize = reply->readableSize();
            reply->pushData(strError.c_str(), strError.size());
            reply->readableSize() - oldSize;
            return err;
        }
    }

    auto list = value->castList();
    for (size_t i = 2; i < params.size(); ++ i)
    {
        if (pos == ListPosition::head)
        {
            list->push_front(params[i]);
        }
        else
        {
            list->push_back(params[i]);
        }
    }
    long listValue = static_cast<long>(list->size());
    char val[32];
    int len = snprintf(val, sizeof val, "%ld\r\n", listValue);
    
    size_t  oldSize = reply->readableSize();
    reply->pushData(":", 1);
    reply->pushData(val, len);
    reply->readableSize() - oldSize;
}

YError YList::lpush(const std::vector<std::string>& params, ReplyBuffer* reply)
{
    return push(params, reply, ListPosition::head);
}

YError YList::rpush(const std::vector<std::string>& params, ReplyBuffer* reply)
{
    return push(params, reply, ListPosition::tail);
}

YError YList::lpushx(const std::vector<std::string>& params, ReplyBuffer* reply)
{
    return push(params, reply, ListPosition::head, false);
}

YError YList::rpushx(const std::vector<std::string>& params, ReplyBuffer* reply)
{
    return push(params, reply, ListPosition::tail, false);
}

YError YList::lpop(const std::vector<std::string>& params, ReplyBuffer* reply)
{
    std::string result;
    YError err = pop(params[1], ListPosition::head, result);
    switch (err)
    {
        case YError_ok:
            Yedis::YedisFormate::formatBulk(result, reply);
            break;
            
        default:
            break;
    }
    
    return err;
}


YError YList::rpop(const std::vector<std::string>& params, ReplyBuffer* reply)
{
    std::string result;
    YError err = pop(params[1], ListPosition::tail, result);
    switch (err)
    {
        case YError_ok:
            Yedis::YedisFormate::formatBulk(result, reply);
            break;
            
        default:
            break;
    }
    
    return  err;
}


YError YList::pop(const std::string& key, ListPosition pos, std::string& result)
{
    YObject* value;
    
    YError err = YSTORE.getValueByType(key, value, YType_list);
    if (err != YError_ok)
    {
        return  err;
    }
    
    auto list = value->castList();
    assert (!list->empty());

    if (pos == ListPosition::head)
    {
        result = std::move(list->front());
        list->pop_front();
    }
    else
    {
        result = std::move(list->back());
        list->pop_back();
    }
    
    if (list->empty())
    {
        YSTORE.deleteKey(key);
    }
    
    return YError_ok;
}
}
