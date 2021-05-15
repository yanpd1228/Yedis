#include "YSortedSet.h"
#include "YedisCommon.h"

#include "YString.h"

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
    if (Yedis::Ystring::stringToLong(value.c_str(), value.size(), &val))
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

YObject YObject::createList()
{
    YObject obj(YType_list);
    obj.reset(new std::list<std::string>);

    return obj;
}

YObject YObject::createSet()
{
    YObject obj(YType_set);
    obj.reset(new std::unordered_set<std::string>);
    return obj;
}

YObject YObject::createHash()
{
    YObject obj(YType_hash);
    obj.reset(new std::unordered_map<std::string, std::string>);
    return obj;
}

YObject YObject::createSSet()
{
    YObject obj(YType_sortedSet);
    obj.reset(new Yedis::YSortedSet());
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
                    
        case YEncode_sset:
             delete reinterpret_cast<Yedis::YSortedSet*>(value);
             break;
                     
        case YEncode_hash:
            delete castHash();
            break;
                    
        default:
            break;
    }
}
