#ifndef _YEDIS_YLIST_H_
#define _YEDIS_YLIST_H_
#include <string>
#include <vector>
#include "../base/ReplyBuffer.h"
#include "YedisStore.h"

namespace Yedis
{

enum ListPosition
{
    head,
    tail,
};

class YList
{
public:
    YList();
    ~YList();

public:
    YError lpush(const std::vector<std::string>& params, ReplyBuffer* reply);
    YError rpush(const std::vector<std::string>& params, ReplyBuffer* reply);
    YError lpushx(const std::vector<std::string>& params, ReplyBuffer* reply);
    YError rpushx(const std::vector<std::string>& params, ReplyBuffer* reply);
    YError lpop(const std::vector<std::string>& params, ReplyBuffer* reply);
    YError rpop(const std::vector<std::string>& params, ReplyBuffer* reply);

private:
    YError push(const std::vector<std::string>& params, ReplyBuffer* reply, ListPosition pos, bool createIfNotExist = true);
    YError pop(const std::string& key, ListPosition pos, std::string& result);

};
}



#endif //!_YEDIS_YLIST_H_
