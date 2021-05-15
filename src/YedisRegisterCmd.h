#ifndef _YEDIS_REGISTER_CMD_H_
#define _YEDIS_REGISTER_CMD_H_
#include "../base/AsyncLog.h"
#include "YHash.h"
#include "YList.h"
#include "YSet.h"
#include "YSortedSet.h"
#include "YedisCommand.h"
#include "YString.h"
#include "YedisCommon.h"
#include <list>
#include <memory>
namespace Yedis
{
class YedisRegisterCmd final
{
public:
    YedisRegisterCmd();
    ~YedisRegisterCmd();
    YedisRegisterCmd(const YedisRegisterCmd&)=delete;
    YedisRegisterCmd& operator=(const YedisRegisterCmd&)=delete;
    static YedisRegisterCmd& getInstance()
    {
        static YedisRegisterCmd instance;
        return instance;
    }
public:
    void init();
    YError command(const std::vector<std::string>& params, ReplyBuffer* reply);

private:
    std::list<std::shared_ptr<Ystring>> m_ptrYStringlist;
    std::list<std::shared_ptr<YList>>   m_ptrYListlist;
    std::list<std::shared_ptr<YSet>>    m_ptrYSetlist;
    std::list<std::shared_ptr<YHash>>   m_ptrYHashlist;
    std::list<std::shared_ptr<YSortedSet>>   m_ptrYSortedSetlist;

};
}
#endif //!YEDIS_REGISTER_CMD_H_
