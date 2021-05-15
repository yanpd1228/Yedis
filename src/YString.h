#ifndef _YEDIS_STRING_H_
#define _YEDIS_STRING_H_
#include <string>
#include "YedisStore.h"
#include "YedisCommon.h"
#include "../base/ReplyBuffer.h"

namespace Yedis{
class Ystring final
{
public:
    Ystring(){};
    Ystring(const Ystring& rhs) = delete;
    Ystring& operator =(const Ystring& rhs) = delete;
public:
    static bool stringToLong(const char* ptr, std::size_t nBytes, long* outVal);

    static bool setValue(const std::string& key, const std::string& value, bool exclusive = false); 

    std::string getDecodeString(const YObject* value) const;

public:
    YError setCmd(const std::vector<std::string>& params, ReplyBuffer* reply) const;
    YError getCmd(const std::vector<std::string>& params, ReplyBuffer* reply) const;
    YError setnxCmd(const std::vector<std::string>& params, ReplyBuffer* reply) const;
};

}


#endif //!_YEDIS_STRING_H_
