#ifndef _YEDIS_YLIST_H_
#define _YEDIS_YLIST_H_
#include <string>
#include <vector>
#include <list>
#include "YedisCommon.h"
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
    YError llen(const std::vector<std::string>& params, ReplyBuffer* reply);
    YError lrange(const std::vector<std::string>& params, ReplyBuffer* reply);

private:
    YError push(const std::vector<std::string>& params, ReplyBuffer* reply, ListPosition pos, bool createIfNotExist = true);
    YError pop(const std::string& key, ListPosition pos, std::string& result);

};


inline void adjustIndex(long& start, long& end, size_t  size)
{
    if (size == 0)
    {
        end = 0, start = 1;
        return;
    }
    
    if (start < 0)  start += size;
    if (start < 0)  start = 0;
    if (end < 0)    end += size;
    
    if (end >= static_cast<long>(size))  end = size - 1;
}

}



#endif //!_YEDIS_YLIST_H_
