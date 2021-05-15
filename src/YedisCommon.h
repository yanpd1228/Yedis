#ifndef _YEDIS_COMMON_H_
#define _YEDIS_COMMON_H_

#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <set>

class YSortedSet;

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

static const int kLRUBits = 24;

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
    YSortedSet* castSortedSet() { return reinterpret_cast<YSortedSet*>(value); }
    std::unordered_map<std::string,std::string>*    castHash()         const { return reinterpret_cast<std::unordered_map<std::string,std::string>*>(value);   }
   
private:
    void moveFrom(YObject&& obj);
    void freeValue();
};
#endif //!_YEDIS_COMMON_H_
