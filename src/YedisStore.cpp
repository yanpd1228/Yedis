#include "YedisStore.h"
#include "YString.h"

namespace Yedis{

uint32_t YObject::lruclock = static_cast<uint32_t>(::time(nullptr));
    

YObject::YObject(YType t) : type(t)
{
    switch (type)
    {
        case YType_list:
            encoding = YEncode_list;
            break;
                    
        case YType_set:
            encoding = YEncode_set;
            break;
                    
        case YType_sortedSet:
            encoding = YEncode_sset;
            break;
                    
        case YType_hash:
            encoding = YEncode_hash;
            break;
                    
        default:
            encoding = YEncode_invalid;
            break;
    }

    lru = 0;
    value = nullptr;
}
        
YObject::~YObject()
{
    freeValue();
}
    
void YObject::clear()
{
    freeValue();
            
    type = YType_invalid;
    encoding = YEncode_invalid;
    lru = 0;
    value = nullptr;
}

YObject YObject::createString(const std::string& value)
{
    YObject obj(YType_string);

    long val;
    if (Ystring::stringToLong(value.c_str(), value.size(), &val))
    {
        obj.encoding = YEncode_int;
        obj.value = (void*)val;
    }
    else
    {
        obj.encoding = YEncode_raw;
        obj.value = new std::string(value);
    }

    return obj;
}

YObject YObject::createString(long val)
{
    YObject obj(YType_string);
    
    obj.encoding = YEncode_int;
    obj.value = (void*)val;
    
    return obj;
}
        
void YObject::reset(void* newvalue)
{
    freeValue();
    value = newvalue;
}
        
YObject::YObject(YObject&& obj) :
    type(YType_invalid),
    encoding(YEncode_invalid),
    lru(0),
    value(nullptr)
{
    moveFrom(std::move(obj));
}
        
YObject& YObject::operator= (YObject&& obj)
{
    moveFrom(std::move(obj));
    return *this;
}
    
void YObject::moveFrom(YObject&& obj)
{
    this->reset();
            
    this->encoding = obj.encoding;
    this->type = obj.type;
    this->value = obj.value;
    this->lru = obj.lru;
            
    obj.encoding = YEncode_invalid;
    obj.type = YType_invalid;
    obj.value = nullptr;
    obj.lru = 0;
}
        
void YObject::freeValue()
{
    switch (encoding)
    {
        case YEncode_raw:
            delete castString();
            break;
                    
        case YEncode_list:
            delete castList();
            break;
                    
        case YEncode_set:
            delete castSet();
            break;
                    
       // case YEncode_sset:
       //     delete castSortedSet();
       //     break;
       //             
        case YEncode_hash:
            delete castHash();
            break;
                    
        default:
            break;
    }
}

YStore& YStore::Instance()
{
    static YStore store;
    return store;
}

void  YStore::init(int dbNum)
{
    if (dbNum < 1)
    {
        dbNum = 1;
    }
    else if (dbNum > kMaxDbNum)
    {
        dbNum = kMaxDbNum;
    }
    m_vecStore.resize(dbNum);
}


int YStore::selectDB(int dbno)
{
    if (dbno == m_nDbno)
    {
        return  m_nDbno;
    }
    if (dbno >= 0 && dbno < static_cast<int>(m_vecStore.size()))
    {
        int oldDb = m_nDbno;

        m_nDbno = dbno;
        return oldDb;
    }
        
    return -1;
}

int YStore::getDB() const
{
    return m_nDbno;
}

const YObject* YStore::getObject(const std::string& key) const
{
    auto db = &m_vecStore[m_nDbno];
    YDB::const_iterator it(db->find(key));
    if (it != db->end())
    {
        return &it->second;
    }
    return nullptr;
}

bool YStore::deleteKey(const std::string& key)
{
    auto db = &m_vecStore[m_nDbno];

    return db->erase(key) != 0;
}

bool YStore::existsKey(const std::string& key) const
{
    const YObject* obj = getObject(key);
    return obj != nullptr;
}

YType YStore::getKeyType(const std::string& key) const
{
    const YObject* obj = getObject(key);
    if (!obj)
    {
        return YType_invalid;
    }
    return YType(obj->type);
}

YError YStore::getValue(const std::string& key, YObject*& value, bool touch)
{
    if (touch)
    {
        return getValueByType(key, value);
    }
    else
    {
        return getValueByTypeNoTouch(key, value);
    }
}

YError  YStore::getValueByType(const std::string& key, YObject*& value, YType type)
{
    return getValueByType(key, value, type, true);
}

YError  YStore::getValueByTypeNoTouch(const std::string& key, YObject*& value, YType type)
{
    return getValueByType(key, value, type, false);
}

YError YStore::getValueByType(const std::string& key, YObject*& value, YType type, bool touch)
{
    
    auto cobj = getObject(key);
    if (cobj)
    {
        if (type != YType_invalid && type != YType(cobj->type))
        {
            return YError_type;
        }
        else
        {
            value = const_cast<YObject*>(cobj);
            return YError_ok;
        }
    }
    else
    {
        return YError_notExist;
    }

    return  YError_ok;
}


YObject* YStore::setValue(const std::string& key, YObject&& value)
{
    auto db = &m_vecStore[m_nDbno];
    YObject& obj = ((*db)[key] = std::move(value));
    obj.lru = YObject::lruclock;
    return &obj;
}

}
