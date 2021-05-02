#include "ReplyBuffer.h"
#include <iostream>
#include <limits>
#include <cassert>

const std::size_t ReplyBuffer::MAX_BUFFER_SIZE = std::numeric_limits<std::size_t>::max() / 2;


std::size_t ReplyBuffer::write(const void* pData, std::size_t nSize)
{
    return pushData(pData, nSize);
}

std::size_t ReplyBuffer::pushData(const void* pData, std::size_t nSize)
{
    std::size_t nBytes  = pushDataAt(pData, nSize);
    adjustWritePtr(nBytes);

    return nBytes;
}

std::size_t ReplyBuffer::pushDataAt(const void* pData, std::size_t nSize, std::size_t offset)
{
    if (!pData || nSize == 0)
    {
        return 0;
    }
    if (readableSize() == ReplyBuffer::MAX_BUFFER_SIZE)
    {
        return 0;
    }
    assureSpace(nSize + offset);

    assert (nSize + offset <= writableSize());

   	::memcpy(&m_vecBuffer[m_nWritePos + offset], pData, nSize);
    return  nSize;
}

std::size_t ReplyBuffer::peekData(void* pBuf, std::size_t nSize)
{
    std::size_t nBytes  = peekDataAt(pBuf, nSize);
    adjustReadPtr(nBytes);

    return nBytes;
}

std::size_t ReplyBuffer::peekDataAt(void* pBuf, std::size_t nSize, std::size_t offset)
{
    const std::size_t dataSize = readableSize();
    if (!pBuf ||
         nSize == 0 ||
         dataSize <= offset)
        return 0;

    if (nSize + offset > dataSize)
        nSize = dataSize - offset;

	::memcpy(pBuf, &m_vecBuffer[m_nReadPos + offset], nSize);

    return nSize;
}


void ReplyBuffer::assureSpace(std::size_t nSize)
{
    if (nSize <= writableSize())
        return;

    std::size_t maxSize = m_vecBuffer.size();

    while (nSize > writableSize() + m_nReadPos)
    {
        if (maxSize < 64)
            maxSize = 64;
        else if (maxSize <= ReplyBuffer::MAX_BUFFER_SIZE)
            maxSize += (maxSize / 2);
        else 
            break;

        m_vecBuffer.resize(maxSize);
    }
        
    if (m_nReadPos > 0)
    {
        std::size_t dataSize = readableSize();
        std::cout << dataSize << " bytes moved from " << m_nReadPos << std::endl;
        ::memmove(&m_vecBuffer[0], &m_vecBuffer[m_nReadPos], dataSize);
        m_nReadPos  = 0;
        m_nWritePos = dataSize;
    }
}

void ReplyBuffer::shrink(bool tight)
{
    assert (m_vecBuffer.capacity() == m_vecBuffer.size());

    if (m_vecBuffer.empty())
    { 
        assert (m_nReadPos == 0);
        assert (m_nWritePos == 0);
        return;
    }

    std::size_t oldCap   = m_vecBuffer.size();
    std::size_t dataSize = readableSize();
    if (!tight && dataSize > oldCap / 2)
        return;

    std::vector<char>  tmp;
    tmp.resize(dataSize);
    memcpy(&tmp[0], &m_vecBuffer[m_nReadPos], dataSize);
    tmp.swap(m_vecBuffer);

    m_nReadPos  = 0;
    m_nWritePos = dataSize;

    std::cout << oldCap << " shrink to " << m_vecBuffer.size() << std::endl;
}

void ReplyBuffer::clear()
{
    m_nReadPos = m_nWritePos = 0; 
}


void ReplyBuffer::swap(ReplyBuffer& buf)
{
    m_vecBuffer.swap(buf.m_vecBuffer);
    std::swap(m_nReadPos, buf.m_nReadPos);
    std::swap(m_nWritePos, buf.m_nWritePos);
}
