#ifndef _YEDIS_SORTED_SET_H_
#define _YEDIS_SORTED_SET_H_
#include "../base/AsyncLog.h"
#include "YedisStore.h"
#include "YedisCommon.h"
#include "../base/ReplyBuffer.h"
#include <map>
#include <set>
#include <vector>
#include <unordered_map>
#include <string>
#include <cassert>

namespace Yedis
{
class YSortedSet
{
public:
    YSortedSet();
    ~YSortedSet();
public:
    using Members = std::set<std::string>;
    using Score2Members = std::map<double, Members>;

    using Member2Score = std::unordered_map<std::string, double>;

    Member2Score::iterator findMember(const std::string& member);
    Member2Score::const_iterator begin() const {  return m_Member2Scores.begin(); };
    Member2Score::iterator begin() {  return m_Member2Scores.begin(); };
    Member2Score::const_iterator end() const {  return m_Member2Scores.end(); };
    Member2Score::iterator end() {  return m_Member2Scores.end(); };
    void    addMember   (const std::string& member, double score);
    double  updateMember(const Member2Score::iterator& itMem, double delta);

    int     yedisRank        (const std::string& member) const;// 0-based
    int     revRank          (const std::string& member) const;// 0-based
    bool    delMember        (const std::string& member);

    Member2Score::value_type getMemberByRank(std::size_t rank) const;
    
    std::vector<Member2Score::value_type > RangeByRank(long start, long end) const;

    std::vector<Member2Score::value_type> rangeByScore(double minScore, double maxScore);

    std::size_t size() const;

public:
    YError zadd(const std::vector<std::string>& params, ReplyBuffer* reply);

private:
    YObject* getSortedset(const std::string& strName, ReplyBuffer* reply);
    YObject* getOrCreateSortedset(const std::string& strName, ReplyBuffer* reply);

private:
    Score2Members   m_Score2Members;
    Member2Score    m_Member2Scores;

};

}
#endif //!_YEDIS_SORTED_SET_H_
