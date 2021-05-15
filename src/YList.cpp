#include "YList.h"
#include "YedisFormate.h"
#include "../base/AsyncLog.h"
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

YError YList::llen(const std::vector<std::string>& params, ReplyBuffer* reply)
{
    YObject* value;
    YError err = YSTORE.getValueByType(params[1], value, YType_list);
    if (err != YError_ok)
    {
        if (err == YError_type)
        {
            std::string strError = "-ERR Unknown command\r\n";
            std::size_t oldSize = reply->readableSize();
            reply->pushData(strError.c_str(), strError.size());
            reply->readableSize() - oldSize;
        }
        else
        {
            Yedis::YedisFormate::format0(reply);
        }

        return  err;
    }
    
    auto list = value->castList();
    Yedis::YedisFormate::formatInt(static_cast<long>(list->size()), reply);
    return YError_ok;
}


static void index2Iterator(long start, long end,
                           std::list<std::string>&  list,
                           std::list<std::string>::iterator* beginIt, std::list<std::string>::iterator* endIt)
{
    assert (start >= 0 && end >= 0 && start <= end);
    assert (end < static_cast<long>(list.size()));
    
    long size = static_cast<long>(list.size());
    if (beginIt)
    {
        if (start * 2 < size)
        {
            *beginIt = list.begin();
            while (start -- > 0)   ++ *beginIt;
        }
        else
        {
            *beginIt = list.end();
            while (start ++ < size)  -- *beginIt;
        } 
    } 
    
    if (endIt)
    {
        if (end * 2 < size)
        {
            *endIt = list.begin();
            while (end -- > 0)   ++ *endIt;
        }
        else
        {
            *endIt = list.end();
            while (end ++ < size)  -- *endIt;
        }
    }
}

static size_t getRange(long start, long end,
                       std::list<std::string>&  list,
                       std::list<std::string>::iterator* beginIt = nullptr,
                       std::list<std::string>::iterator* endIt = nullptr)
{
    size_t rangeLen = 0;
    if (start > end)  // empty
    {
        if (beginIt)    *beginIt = list.end();
        if (endIt)      *endIt = list.end();
    }
    else if (start != 0 || end + 1 != static_cast<long>(list.size()))
    {
        rangeLen = end - start + 1;
        index2Iterator(start, end, list, beginIt, endIt);
    }
    else
    {
        rangeLen = list.size();
        if (beginIt) *beginIt = list.begin();
        if (endIt)   *endIt   = --list.end();  // entire list
    }
    
    return rangeLen;
}



bool YedisStrtol(const char* ptr, size_t nBytes, long& outVal)
{
    if (nBytes == 0 || nBytes > 20) // include the sign
    { 
        return false;
    }

    errno = 0;
    char* pEnd = 0;
    outVal = strtol(ptr, &pEnd, 0);

    if (errno == ERANGE || errno == EINVAL)
    {
        return false;
    }
    return pEnd == ptr + nBytes;
}

YError YList::lrange(const std::vector<std::string>& params, ReplyBuffer* reply)
{
    YObject* value;
    YError err = YSTORE.getValueByType(params[1], value, YType_list);
    if (err != YError_ok)
    {
        LOGD("error params");
        return err;
    }
    
    long start, end = 0;
    if (!YedisStrtol(params[2].c_str(), params[2].size(), start) ||
        !YedisStrtol(params[3].c_str(), params[3].size(), end))
    {
        LOGD("error params");
        return err;
    }
    
    auto list = value->castList();
    adjustIndex(start, end, list->size());
    
    std::list<std::string>::iterator beginIter;
    size_t rangeLen = getRange(start, end, *list, &beginIter);
    
    YedisFormate::preFormatMultiBulk(rangeLen, reply);
    if (beginIter != list->end())
    {
        while (rangeLen != 0)
        {
            YedisFormate::formatBulk(beginIter->c_str(), beginIter->size(), reply);
            ++beginIter;
            --rangeLen;
        }
    }
    
    return YError_ok;
}

}
