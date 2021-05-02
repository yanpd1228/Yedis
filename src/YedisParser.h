#ifndef _YEDIS_SRC_YEDISPARSER_H_
#define _YEDIS_SRC_YEDISPARSER_H_
#include <vector>
#include "../base/AsyncLog.h"

namespace Yedis{

/*
 * 协议解析，以set name ypd 为例 我们接受到的其实是下面的文本协议
 * *$3\r\nset\r\n$4\r\nname\r\n$4\r\nypd\r\n
 * 协议解析流程为：首先读*符号，发现有三个参数；然后读取第一个$符号，知道第一个参数有三个字符，继 * 续往后读得到了set；
 * 接着读取第二个$符号，知道该参数有四个字符，得到了name;
 * 最后读取第三个$符号，该参数也有四个字符，得到了ypd；
 * 所以，协议解析器还原得到了命令set name ypd
 */
enum class YParseResult : int8_t
{
    ok,
    wait,
    error,
};


class YProtoParse
{
public:
    YParseResult getIntUntilCRLF(const char*& ptr, std::size_t nBytes, int& val);
    void reset();
    YParseResult parseRequest(const char*& ptr, const char* end);

    const std::vector<std::string>& getParams() const { return m_vecParams; }
    void setParams(std::vector<std::string> p) { m_vecParams = std::move(p); }
    
    bool isInitialState() const { return m_nMulti == -1; }

private:
    YParseResult parseMulti(const char*& ptr, const char* end, int& result);
    YParseResult parseStrlist(const char*& ptr, const char* end, std::vector<std::string>& results);
    YParseResult parseStr(const char*& ptr, const char* end, std::string& result);
    YParseResult parseStrval(const char*& ptr, const char* end, std::string& result);
    YParseResult parseStrlen(const char*& ptr, const char* end, int& result);

private:
    int m_nMulti = -1;
    int m_nParamLen = -1;

    std::size_t m_nNumOfParam = 0; 
    std::vector<std::string> m_vecParams;
};


}
#endif //!YEDIS_SRC_YEDISPARSER_H_
