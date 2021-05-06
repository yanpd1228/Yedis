#ifndef _YEDIS_m_vecStoreH_
#define _YEDIS_m_vecStoreH_
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <set>
namespace Yedis{

static const int kLRUBits = 24;
enum YType
{
    YType_invalid,
    YType_string,
    YType_list,
    YType_set,
    YType_sortedSet,
    YType_hash,
};

enum YEncode
{
    YEncode_invalid,

    YEncode_raw, // string
    YEncode_int, // string as int

    YEncode_list,
    
    YEncode_set,
    YEncode_hash,
    
    YEncode_sset,
};


enum YError
{
    YError_nop       = -1,
    YError_ok        = 0,
    YError_type      = 1,
    YError_exist     = 2,
    YError_notExist  = 3,
    YError_param     = 4,
    YError_unknowCmd = 5,
    YError_nan       = 6,
    YError_syntax    = 7,
    YError_dirtyExec = 8,
    YError_watch     = 9,
    YError_noMulti   = 10,
    YError_invalidDB = 11,
    YError_readonlySlave = 12,
    YError_needAuth  = 13,
    YError_errAuth   = 14,
    YError_nomodule   = 15,
    YError_moduleinit = 16,
    YError_moduleuninit = 17,
    YError_modulerepeat = 18,
    YError_busykey      = 19,
    YError_max,
};

struct YObject
{
public:
    static uint32_t lruclock;

    unsigned int type : 4;
    unsigned int encoding : 4;
    unsigned int lru : kLRUBits;

    void* value;
    
    explicit
    YObject(YType = YType_invalid);
    ~YObject();

    YObject(const YObject& obj) = delete;
    YObject& operator= (const YObject& obj) = delete;
    
    YObject(YObject&& obj);
    YObject& operator= (YObject&& obj);
    
    void clear();
    void reset(void* newvalue = nullptr);
    
    static YObject createString(const std::string& value);
    static YObject createString(long value);
    static YObject createList();
    static YObject createSet();
    static YObject createSSet();
    static YObject createHash();
    
    std::string*  castString()       const { return reinterpret_cast<std::string*>(value); }
    std::list<std::string>*    castList()         const { return reinterpret_cast<std::list<std::string>*>(value);   }
    std::unordered_set<std::string>*     castSet()          const { return reinterpret_cast<std::unordered_set<std::string>*>(value);    }
    //PSSET    castSortedSet()    const { return reinterpret_cast<PSSET>(value); }
    std::unordered_map<std::string,std::string>*    castHash()         const { return reinterpret_cast<std::unordered_map<std::string,std::string>*>(value);   }
   
private:
    void moveFrom(YObject&& obj);
    void freeValue();
};

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
}
#endif //!YEDIS_m_vecStoreH_
