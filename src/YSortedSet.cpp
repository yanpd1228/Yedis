#include "YedisCommon.h"
#include "YSortedSet.h"
#include "YedisFormate.h"
#include "YedisStore.h"


namespace Yedis
{

YSortedSet::YSortedSet()
{
    m_Score2Members.clear();
    m_Member2Scores.clear();
}

YSortedSet::~YSortedSet()
{

}

YObject* YSortedSet::getSortedset(const std::string& strName, ReplyBuffer* reply)
{
    YObject* value;  
    YError err = YSTORE.getValueByType(strName, value, YType_sortedSet);
    if (err != YError_ok)  
    {
        LOGD("error type");
        reply->pushData("-ERR error type\r\n", sizeof("-ERR error type") -1);
        return nullptr;
    }

    return value;
}

YObject* YSortedSet::getOrCreateSortedset(const std::string& strName, ReplyBuffer* reply)
{
    YObject* value;
    YError err = YSTORE.getValueByType(strName, value, YType_sortedSet);
    if (err != YError_ok && err != YError_notExist) 
    {
        LOGD("error type");
        reply->pushData("-ERR error type\r\n", sizeof("-ERR error type") -1);
        return nullptr;
    }
    if (err == YError_notExist)
    {
        value = YSTORE.setValue(strName, YObject::createSSet());
    }
    return value;
}

YSortedSet::Member2Score::iterator YSortedSet::findMember(const std::string& member)
{
    return  m_Member2Scores.find(member);
}

void YSortedSet::addMember(const std::string& member, double score)
{
    assert (findMember(member) == m_Member2Scores.end());
        
    m_Member2Scores.insert(Member2Score::value_type(member, score));
    m_Score2Members[score].insert(member);
}

YError YSortedSet::zadd(const std::vector<std::string>& params, ReplyBuffer* reply)
{
    if (params.size() % 2 != 0)
    {
        LOGD("errot YError_syntax");
        reply->pushData("-ERR syntax error\r\n", sizeof("-ERR syntax error\r\n") -1);
        return YError_syntax;
    }
    
    YObject* value = getOrCreateSortedset(params[1], reply);
    std::size_t newMembers = 0;
    YSortedSet* sset = (YSortedSet*)value->castSortedSet();
    for (std::size_t i = 2; i < params.size(); i += 2)
    {
        double score = atof(params[i].c_str());

        auto it = sset->findMember(params[i+1]);
        if (it == sset->end())
        {
            sset->addMember(params[i+1], score);
            ++newMembers;
        }
    }

    YedisFormate::formatInt(newMembers, reply);
    return YError_ok;
}

}
