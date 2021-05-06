#include "YString.h"
#include "YedisStore.h"

namespace Yedis{

#define CRLF "\r\n"
bool Ystring::stringToLong(const char* ptr, std::size_t nBytes, long* outVal)
{
    if (nBytes == 0 || nBytes > 20)
    {
        return false;
    }
    errno = 0;
    char* pEnd = 0;
    *outVal = strtol(ptr, &pEnd, 0);

    if (errno == ERANGE ||   errno == EINVAL)
    {
        return false;
    }
    return pEnd == ptr + nBytes;
}


bool Ystring::setValue(const std::string& key, const std::string& value, bool exclusive)
{
    if (exclusive)
    {
        YObject* val;
        if (YSTORE.getValue(key, val) == YError_ok)
        {
            return false;
        }
    }

    YSTORE.setValue(key, YObject::createString(value));

    return true;
}

std::string Ystring::getDecodeString(const YObject* value) const
{
    if (value->encoding == YEncode_raw)
    {
        return *(value->castString());
    }
    else if (value->encoding == YEncode_int)
    {
        intptr_t val = (intptr_t)value->value;
        
        char buf[32];
        int nLength = snprintf(buf, sizeof buf - 1, "%ld",  val);
        return std::string(buf);
    }
}

YError Ystring::setCmd(const std::vector<std::string>& params, ReplyBuffer* reply) const
{
    setValue(params[1], params[2]);
    size_t oldSize = reply->readableSize();
    reply->pushData("+OK" CRLF, 5);
    reply->readableSize() - oldSize;
    return YError_ok;
}

YError Ystring::getCmd(const std::vector<std::string>& params, ReplyBuffer* reply) const
{
    YObject* value;
    YError err = YSTORE.getValueByType(params[1], value, YType_string);
    if (err != YError_ok)
    {
        if (err == YError_notExist)
        {
            size_t oldSize = reply->readableSize();
            reply->pushData("$-1" CRLF, 5);
            reply->readableSize() - oldSize;
        }
        else
        {
            std::string strError = "-ERR no such key\r\n";
            size_t oldSize = reply->readableSize();
            reply->pushData(strError.c_str(), strError.size());
            reply->readableSize() - oldSize;
        }
        return err;  
    }
    std::string strValue = getDecodeString(value);
    size_t oldSize = reply->readableSize();
    reply->pushData("$", 1);
    char val[32];
    int tmp = snprintf(val, sizeof val - 1, "%lu" CRLF, strValue.size());
    reply->pushData(val, tmp);
    reply->pushData(strValue.c_str(), strValue.size());
    reply->pushData(CRLF, 2);
    reply->readableSize() - oldSize;
    return err;

}

YError Ystring::setnxCmd(const std::vector<std::string>& params, ReplyBuffer* reply) const
{
    if(setValue(params[1], params[2], true))
    {
        const char* val = ":1\r\n";
        size_t oldSize = reply->readableSize();
        reply->pushData(val, 4);
        reply->readableSize() - oldSize;
    }
    else
    {
        const char* val = ":0\r\n";
        size_t oldSize = reply->readableSize();
        reply->pushData(val, 4);
        reply->readableSize() - oldSize;
    }
    return YError_ok;
}

}
