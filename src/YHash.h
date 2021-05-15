#ifndef _YEDIS_HASH_H_
#define _YEDIS_HASH_H_
#include <vector>
#include <string>
#include "YedisCommon.h"
#include "YedisStore.h"
#include "../base/ReplyBuffer.h"


namespace Yedis {

class YHash
{
public:
    YHash();
    ~YHash();

public:
    YError hset(const std::vector<std::string>& params, ReplyBuffer* reply);
    YError hget(const std::vector<std::string>& params, ReplyBuffer* reply);

private:
    YObject* getHash(const std::string& strHashName, ReplyBuffer* reply);
    YObject* getOrCreateHash(const std::string& strHashName, ReplyBuffer* reply);

};
}

#endif //!_YEDIS_HASH_H_

