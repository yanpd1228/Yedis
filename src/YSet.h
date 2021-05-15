#ifndef _YEDIS_SET_H_
#define _YEDIS_SET_H_
#include <set>
#include "YedisStore.h"
#include "YedisCommon.h"
#include "../base/ReplyBuffer.h"
#include "../base/AsyncLog.h"
namespace Yedis {

class YSet
{
public:
    YSet();
    ~YSet();
public:
    YError sadd(const std::vector<std::string>& params, ReplyBuffer* reply);
    YError scard(const std::vector<std::string>& params, ReplyBuffer* reply);
    YError smember(const std::vector<std::string>& params, ReplyBuffer* reply);
    YError spop(const std::vector<std::string>& params, ReplyBuffer* reply);

private:
    YObject* creatrOrGetSet(const std::string& strSetName, ReplyBuffer* reply);
    YObject* getSet(const std::string& strSetName, ReplyBuffer* reply);

};
}
#endif //!_YEDIS_SET_H_
