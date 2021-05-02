#include <cassert>
#include <vector>
#include "YedisParser.h"

namespace Yedis
{
void YProtoParse::reset()
{
    m_nMulti = -1;
    m_nParamLen = -1;
    m_nNumOfParam = 0;

    while (m_vecParams.size() > 3)
    {
        m_vecParams.pop_back();
    }
}

YParseResult YProtoParse::getIntUntilCRLF(const char *&ptr, std::size_t nBytes, int &val)
{
    if (nBytes < 3)
    {
        return YParseResult::wait;
    }
    
    std::size_t i = 0;
    bool negtive = false;
    if (ptr[0] == '-')
    {
        negtive = true;
        ++ i;
    }
    else if (ptr[0] == '+')
    {
        ++ i;
    }
    
    int value = 0;
    for (; i < nBytes; ++ i)
    {
        if (isdigit(ptr[i]))
        {
            value *= 10;
            value += ptr[i] - '0';
        }
        else
        {
            if (ptr[i] != '\r' || (i+1 < nBytes && ptr[i+1] != '\n'))
            {
                return YParseResult::error;
            }
            if (i + 1 == nBytes)
            {
                return YParseResult::wait;
            }
            break;
        }
    }
    
    if (negtive)
    {
        value *= -1;
    }
    ptr += i;
    ptr += 2;
    val = value;
    return YParseResult::ok;
}

YParseResult YProtoParse::parseRequest(const char*& ptr, const char* end)
{
    if (m_nMulti == -1)
    {
        auto parseRet = parseMulti(ptr, end, m_nMulti);
        if (parseRet == YParseResult::error || m_nMulti < -1)
        {
            return YParseResult::error;
        }
        if (parseRet != YParseResult::ok)
        {
            return YParseResult::wait;
        }
    }

    return parseStrlist(ptr, end, m_vecParams);
}

YParseResult YProtoParse::parseMulti(const char*& ptr, const char* end, int& result)
{
    if (end - ptr < 3)
    {
        return YParseResult::wait;
    }
    if (*ptr != '*')
    {
        return YParseResult::error;
    }
    ++ptr;

    return getIntUntilCRLF(ptr,  end - ptr, result);
}

YParseResult YProtoParse::parseStrlist(const char*& ptr, const char* end, std::vector<std::string>& results)
{
    while (static_cast<int>(m_nNumOfParam) < m_nMulti)
    {
        if (results.size() < m_nNumOfParam + 1)
        {
            results.resize(m_nNumOfParam + 1);
        }
        auto parseRet = parseStr(ptr, end, results[m_nNumOfParam]);

        if (parseRet == YParseResult::ok)
        {
            ++ m_nNumOfParam;
        }
        else
        {
            return parseRet;
        }
    }

    results.resize(m_nNumOfParam);
    return YParseResult::ok;
}

YParseResult YProtoParse::parseStr(const char*& ptr, const char* end, std::string& result)
{
    if (m_nParamLen == -1)
    {
        auto parseRet = parseStrlen(ptr, end, m_nParamLen);
        if (parseRet == YParseResult::error || m_nParamLen < -1)
        {
            return YParseResult::error;
        }

        if (parseRet != YParseResult::ok)
        {
            return YParseResult::wait;
        }
    }

    if (m_nParamLen == -1)
    {
        result.clear();
        return YParseResult::ok;
    }
    else
    {
        return parseStrval(ptr, end, result);
    }
}

YParseResult YProtoParse::parseStrval(const char*& ptr, const char* end, std::string& result)
{
    assert (m_nParamLen >= 0);

    if (static_cast<int>(end - ptr) < m_nParamLen + 2)
    {
        return YParseResult::wait;
    }
    auto tail = ptr + m_nParamLen;
    if (tail[0] != '\r' || tail[1] != '\n')
    {
        return YParseResult::error;
    }
    result.assign(ptr, tail - ptr);
    ptr = tail + 2;
    m_nParamLen = -1;

    return YParseResult::ok;
}

YParseResult YProtoParse::parseStrlen(const char*& ptr, const char* end, int& result)
{
    if (end - ptr < 3)
    {
        return YParseResult::wait;
    }
    if (*ptr != '$')
    {
        return YParseResult::error;
    }
    ++ptr;

    const auto ret = getIntUntilCRLF(ptr,  end - ptr, result);
    if (ret != YParseResult::ok)
    {
        --ptr;
    }
    return ret;
}
}
