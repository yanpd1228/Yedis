#include "YedisFormate.h"


namespace Yedis
{

std::size_t YedisFormate::formatInt(long value, ReplyBuffer* reply)
{
    if (!reply)
    {
        return 0;
    }
    
    char val[32];
    int len = snprintf(val, sizeof val, "%ld\r\n", value);
    
    std::size_t  oldSize = reply->readableSize();
    reply->pushData(":", 1);
    reply->pushData(val, len);
    
    return reply->readableSize() - oldSize;
}

std::size_t YedisFormate::formatSingle(const char* str, std::size_t len, ReplyBuffer* reply)
{
    if (!reply)
    {
        return 0;
    }
    std::size_t oldSize = reply->readableSize();
    reply->pushData("+", 1);
    reply->pushData(str, len);
    reply->pushData("\r\n", 2);

    return reply->readableSize() - oldSize;
}


std::size_t YedisFormate::formatSingle(const std::string& str, ReplyBuffer* reply)
{
    return formatSingle(str.c_str(), str.size(), reply);
}
std::size_t YedisFormate::formatBulk(const char* str, std::size_t len, ReplyBuffer* reply)
{
    if (!reply)
    {
        return 0;
    }

    std::size_t oldSize = reply->readableSize();
    reply->pushData("$", 1);

    char val[32];
    int tmp = snprintf(val, sizeof val - 1, "%lu\r\n", len);
    reply->pushData(val, tmp);

    if (str && len > 0)
    {
        reply->pushData(str, len);
    }
    
    reply->pushData("\r\n", 2);
    
    return reply->readableSize() - oldSize;
}

std::size_t YedisFormate::formatBulk(const std::string& str, ReplyBuffer* reply)
{
    return formatBulk(str.c_str(), str.size(), reply);
}


std::size_t YedisFormate::formatEmptyBulk(ReplyBuffer* reply)
{
    return reply->pushData("$0\r\n\r\n", 6);
}


std::size_t YedisFormate::formatNull(ReplyBuffer* reply)
{
    if (!reply)
    {
        return 0;
    }
    std::size_t oldSize = reply->readableSize();
    reply->pushData("$-1\r\n", 5);
    
    return reply->readableSize() - oldSize;
}


std::size_t YedisFormate::formatNullArray(ReplyBuffer* reply)
{
    if (!reply)
    {
        return 0;
    }
    std::size_t oldSize = reply->readableSize();
    reply->pushData("*-1\r\n", 5);
    
    return reply->readableSize() - oldSize;
}


std::size_t YedisFormate::formatOK(ReplyBuffer* reply)
{
    if (!reply)
    {
        return 0;
    }
    std::size_t oldSize = reply->readableSize();
    reply->pushData("+OK\r\n", 5);
    
    return reply->readableSize() - oldSize;
}


std::size_t YedisFormate::format1(ReplyBuffer* reply)
{
    if (!reply)
    {
        return 0;
    }
    const char* val = ":1\r\n";
    
    std::size_t oldSize = reply->readableSize();
    reply->pushData(val, 4);
    
    return reply->readableSize() - oldSize;
}


std::size_t YedisFormate::format0(ReplyBuffer* reply)
{
    if (!reply)
    {
        return 0;
    }
    const char* val = ":0\r\n";
    
    std::size_t oldSize = reply->readableSize();
    reply->pushData(val, 4);
    
    return reply->readableSize() - oldSize;
}

}
