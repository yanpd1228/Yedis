#ifndef _YEDIS_BASE_REPLYm_vecBufferH_
#define _YEDIS_BASE_REPLYm_vecBufferH_
#include <vector>
#include <cstring>
class ReplyBuffer
{
public:
    ReplyBuffer() :m_nReadPos(0),m_nWritePos(0)
    {
    }

    std::size_t pushDataAt(const void* pData, std::size_t nSize, std::size_t offset = 0);
    std::size_t pushData(const void* pData, std::size_t nSize);
    std::size_t write(const void* pData, std::size_t nSize);
    void adjustWritePtr(std::size_t nBytes) {   m_nWritePos += nBytes; }

    std::size_t peekDataAt(void* pBuf, std::size_t nSize, std::size_t offset = 0);
    std::size_t peekData(void* pBuf, std::size_t nSize);
    void adjustReadPtr(std::size_t nBytes) {   m_nReadPos  += nBytes; }

    char* readAddr()  {  return &m_vecBuffer[m_nReadPos];  }
    char* writeAddr() {  return &m_vecBuffer[m_nWritePos]; }

    bool isEmpty() const { return readableSize() == 0; }
    std::size_t readableSize() const {  return m_nWritePos - m_nReadPos;  }
    std::size_t writableSize() const {  return m_vecBuffer.size() - m_nWritePos;  }

    void shrink(bool tight = false);
    void clear();
    void swap(ReplyBuffer& buf);

    static const std::size_t  MAX_BUFFER_SIZE;
private:
    void assureSpace(std::size_t size);
private:
    std::size_t        m_nReadPos;
    std::size_t        m_nWritePos;
    std::vector<char>  m_vecBuffer;
};


#endif
