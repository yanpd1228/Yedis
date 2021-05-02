#ifndef _YEDIS_REGISTER_CMD_H_
#define _YEDIS_REGISTER_CMD_H_
#include "../base/AsyncLog.h"
#include "YedisCommand.h"
#include "YString.h"
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
    //std::unique_ptr<YCommand>& getYcommand() { return m_ptrYCommand;}

private:
    std::list<std::shared_ptr<Ystring>> m_list;

};
}
#endif //!YEDIS_REGISTER_CMD_H_
