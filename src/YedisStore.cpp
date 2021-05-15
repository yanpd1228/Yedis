#include "YedisStore.h"
#include "YSortedSet.h"
#include "YString.h"
#include <list>

namespace Yedis{



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
