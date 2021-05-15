#ifndef _YEDIS_STORE_H_
#define _YEDIS_STORE_H_
#include "YHash.h"
#include "YList.h"
#include "YSet.h"
#include "YSortedSet.h"
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <set>
#include "YString.h"
#include "YedisCommon.h"

namespace Yedis{



using YDB = std::unordered_map<std::string, YObject>;
const int kMaxDbNum = 65536;

class YStore
{
public:
    YStore() : m_nDbno(0)
    {
    }
    static YStore& Instance();
    
    YStore(const YStore& ) = delete;
    void operator= (const YStore& ) = delete;
    
    void init(int dbNum = 16);

    int selectDB(int dbno);
    int getDB() const;
    
    // Key operation
    bool deleteKey(const std::string& key);
    bool existsKey(const std::string& key) const;
    YType getKeyType(const std::string& key) const;
    //std::string RandomKey(YObject** val = nullptr) const;
    size_t DBSize() const { return m_vecStore[m_nDbno].size(); }
    //size_t ScanKey(size_t cursor, size_t count, std::vector<std::string>& res) const;

    // iterator
    YDB::const_iterator begin() const   { return m_vecStore[m_nDbno].begin(); }
    YDB::const_iterator end()   const   { return m_vecStore[m_nDbno].end(); }
    YDB::iterator       begin()         { return m_vecStore[m_nDbno].begin(); }
    YDB::iterator       end()           { return m_vecStore[m_nDbno].end(); }
    
    const YObject* getObject(const std::string& key) const;
    YError getValue(const std::string& key, YObject*& value, bool touch = true);
    YError getValueByType(const std::string& key, YObject*& value, YType type = YType_invalid);
    YError  getValueByTypeNoTouch(const std::string& key, YObject*& value, YType type = YType_invalid);
    YObject* setValue(const std::string& key, YObject&& value);
private:
    YError getValueByType(const std::string& key, YObject*& value, YType type, bool touch);
private:
    mutable std::vector<YDB> m_vecStore;
    int                      m_nDbno;
};

#define YSTORE YStore::Instance()

template <typename HASH>
inline typename HASH::const_local_iterator randomHashMember(const HASH& container)
{
    if (container.empty())
    {
        return typename HASH::const_local_iterator();
    }
    
    while (true)
    {
        std::size_t bucket = random() % container.bucket_count();
        if (container.bucket_size(bucket) == 0)
        {
            continue;
        }
        long lucky = random() % container.bucket_size(bucket);
        typename HASH::const_local_iterator it = container.begin(bucket);
        while (lucky > 0)
        {
            ++ it;
            -- lucky;
        }
        
        return it;
    }
        
    return typename HASH::const_local_iterator();
}

}
#endif //!YEDIS_STORE_H_
