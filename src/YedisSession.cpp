#include "YedisSession.h"
#include <memory>
#include "Buffer.h"

YedisSession::YedisSession(std::shared_ptr<TcpConnection> conn)
{


}

YedisSession::~YedisSession()
{

}


void YedisSession::OnRead(const std::shared_ptr<TcpConnection>& conn, Buffer* buffer)
{
    std::string strInputBuffer;
    strInputBuffer.append(buffer->peek(),buffer->readableBytes());
    decodeMessage(strInputBuffer);
}

bool YedisSession::decodeMessage(const std::string& strRecv)
{
    std::string strEnd = strRecv.substr(strRecv.length() - 2);
    if (strEnd != "\r\n")
    {
        return false;
    }

    std::vector<std::string> veclines;
    Split(strRecv, veclines, "\r\n");
    if (veclines.size() < 1 || veclines[0].empty())
    {
        return false;
    }
    if (veclines[0] != "*")
    {
        return false;
    }
    std::size_t nPos = 0;

    /* 最终，set name bert经过协议编码后：
    *3\r\n$3\r\nset\r\n$4\r\nname\r\n$4\r\nbert\r\n
    协议解析流程为：首先读*符号，发现有三个参数；然后读取第一个$符号，知道第一个参数有三个字符，继续往后读得到了set；
    接着读取第二个$符号，知道该参数有四个字符，得到了name；
    最后读取第三个$符号，该参数也有四个字符，得到了bert；*/
    for(nPos; nPos < veclines.size(); nPos++)
    {
        if(veclines[nPos] != "*")
        {
            return false;
        }
        
    }
    
}

void YedisSession::Split(const std::string& str, std::vector<std::string>& v, const char* delimiter)
{
    if (delimiter == NULL || str.empty())
        return;

    std::string buf = str;
    size_t pos = std::string::npos;
    std::string substr;
    int delimiterlength = strlen(delimiter);
    while (true)
    {
        pos = buf.find(delimiter);
        if (pos != std::string::npos)
        {
            substr = buf.substr(0, pos);
            if (!substr.empty())
                v.push_back(substr);

            buf = buf.substr(pos + delimiterlength);
        }
        else
        {
            if (!buf.empty())
                v.push_back(buf);
            break;
        }           
    }
}
