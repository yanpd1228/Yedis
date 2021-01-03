#ifndef _YPD_BUFFER_H_
#define _YPD_BUFFER_H_
#include <vector>
#include <cassert>
#include <search.h>
#include <string>
#include <cstring>
#include <algorithm>
#include "../base/Platform.h"
#include "../net/Sockets.h"
#include "../base/Platform.h"
#include "../net/Endian.h"
/* �������modul��buffer ��*/
using namespace net;
class Buffer 
{
public:
	static const size_t kCheapPrepend = 8; /* ǰ��Ԥ���ռ��С */

	static const size_t kInitialSize = 1024; /* ��ʼ���洢���ݿռ� */

	explicit Buffer(size_t initialSize = kInitialSize)
		: buffer_(kCheapPrepend + initialSize), /* ��ʼ��Buffer�ܴ�С */
		readerIndex_(kCheapPrepend),
		writerIndex_(kCheapPrepend)
	{
		assert(readableBytes() == 0);
		assert(writableBytes() == initialSize);
		assert(prependableBytes() == kCheapPrepend);
	}

	// implicit copy-ctor, move-ctor, dtor and assignment are fine
	// NOTE: implicit move-ctor is added in g++ 4.6

	void swap(Buffer& rhs)
	{
		buffer_.swap(rhs.buffer_);
		std::swap(readerIndex_, rhs.readerIndex_);
		std::swap(writerIndex_, rhs.writerIndex_);
	}

	size_t readableBytes() const /* �ɶ��ռ��ж�󣨼��洢�������� �� */
	{
		return writerIndex_ - readerIndex_;
	}

	size_t writableBytes() const/* ��д��ռ��ж�� */
	{
		return buffer_.size() - writerIndex_;
	}

	size_t prependableBytes() const/* ��д��λ�� */
	{
		return readerIndex_;
	}

	const char* peek() const/* ����ָ��ɶ��ռ����ʼλ��ָ�� */
	{
		return begin() + readerIndex_;
	}

	const char* findCRLF() const//��[peek(), beginWrite())����\r\n */
	{
		// FIXME: replace with memmem()?
		const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF + 2);
		return crlf == beginWrite() ? NULL : crlf;
	}

	const char* findCRLF(const char* start) const/* ��λ��start��ʼ���� */
	{
		assert(peek() <= start);
		assert(start <= beginWrite());
		// FIXME: replace with memmem()?
		const char* crlf = std::search(start, beginWrite(), kCRLF, kCRLF + 2);
		return crlf == beginWrite() ? NULL : crlf;
	}

	const char* findEOL() const/* ����'\n'��λ�� */
	{
		const void* eol = memchr(peek(), '\n', readableBytes());
		return static_cast<const char*>(eol);
	}

	const char* findEOL(const char* start) const
	{
		assert(peek() <= start);
		assert(start <= beginWrite());
		const void* eol = memchr(start, '\n', beginWrite() - start);
		return static_cast<const char*>(eol);
	}

	// retrieve returns void, to prevent
	// string str(retrieve(readableBytes()), readableBytes());
	// the evaluation of two functions are unspecified
	void retrieve(size_t len)/* ȡ��len�������ݣ���Ҫ��������readerIndex_��writerIndex_��ֵ */
	{
		assert(len <= readableBytes());
		if (len < readableBytes())
		{
			readerIndex_ += len;
		}
		else
		{
			retrieveAll();/* ȫ��ȡ�� */
		}
	}

	void retrieveUntil(const char* end)/* endǰ������ȫȡ�� */
	{
		assert(peek() <= end);
		assert(end <= beginWrite());
		retrieve(end - peek());
	}

	void retrieveInt64()
	{
		retrieve(sizeof(int64_t));
	}

	void retrieveInt32()
	{
		retrieve(sizeof(int32_t));
	}

	void retrieveInt16()
	{
		retrieve(sizeof(int16_t));
	}

	void retrieveInt8()
	{
		retrieve(sizeof(int8_t));
	}

	void retrieveAll()
	{
		readerIndex_ = kCheapPrepend;
		writerIndex_ = kCheapPrepend;
	}

	std::string retrieveAllAsString()/* Buffer�������������ַ�����ʽȡ�� */
	{
		return retrieveAsString(readableBytes());;
	}

	std::string retrieveAsString(size_t len)/* Buffer��len�����������ַ�����ʽȡ�� */
	{
		assert(len <= readableBytes());
		std::string result(peek(), len);
		retrieve(len);
		return result;
	}

	std::string toStringPiece() const
	{
		return std::string(peek(), static_cast<int>(readableBytes()));
	}

	void append(const std::string& str)
	{
		append(str.c_str(), str.size());
	}

	void append(const char* /*restrict*/ data, size_t len)
	{
		ensureWritableBytes(len);
		std::copy(data, data + len, beginWrite());
		hasWritten(len);
	}

	void append(const void* /*restrict*/ data, size_t len)
	{
		append(static_cast<const char*>(data), len);
	}

	void ensureWritableBytes(size_t len)/* ȷ��Buffer�п���д��len�������� */
	{
		if (writableBytes() < len)/* ��д��ռ䲻���� */
		{
			makeSpace(len);/* ȷ����д��ռ䲻С��len */
		}
		assert(writableBytes() >= len);
	}

	char* beginWrite()/* ����ָ���д��λ�õ�ָ�� */
	{
		return begin() + writerIndex_;
	}

	const char* beginWrite() const
	{
		return begin() + writerIndex_;
	}

	void hasWritten(size_t len)/* д��len�������ݺ󣬵��ñ����� */
	{
		assert(len <= writableBytes());
		writerIndex_ += len;
	}

	void unwrite(size_t len)/* �������д���len�������� */
	{
		assert(len <= readableBytes());
		writerIndex_ -= len;
	}

	///
	/// Append int64_t using network endian
	///
	void appendInt64(int64_t x)
	{
		int64_t be64 = sockets::hostToNetwork64(x);
		append(&be64, sizeof be64);
	}

	///
	/// Append int32_t using network endian
	///
	void appendInt32(int32_t x)
	{
		int32_t be32 = sockets::hostToNetwork32(x);
		append(&be32, sizeof be32);
	}

	void appendInt16(int16_t x)
	{
		int16_t be16 = sockets::hostToNetwork16(x);
		append(&be16, sizeof be16);
	}

	void appendInt8(int8_t x)
	{
		append(&x, sizeof x);
	}

	///
	/// Read int64_t from network endian
	///
	/// Require: buf->readableBytes() >= sizeof(int32_t)
	int64_t readInt64()
	{
		int64_t result = peekInt64();
		retrieveInt64();
		return result;
	}

	///
	/// Read int32_t from network endian
	///
	/// Require: buf->readableBytes() >= sizeof(int32_t)
	int32_t readInt32()
	{
		int32_t result = peekInt32();
		retrieveInt32();
		return result;
	}

	int16_t readInt16()
	{
		int16_t result = peekInt16();
		retrieveInt16();
		return result;
	}

	int8_t readInt8()
	{
		int8_t result = peekInt8();
		retrieveInt8();
		return result;
	}

	///
	/// Peek int64_t from network endian
	///
	/// Require: buf->readableBytes() >= sizeof(int64_t)
	int64_t peekInt64() const/* ��Buffer�ж�Int64�������ݲ����ش����� */
	{
		assert(readableBytes() >= sizeof(int64_t));
		int64_t be64 = 0;
		::memcpy(&be64, peek(), sizeof be64);
		return sockets::networkToHost64(be64);
	}

	///
	/// Peek int32_t from network endian
	///
	/// Require: buf->readableBytes() >= sizeof(int32_t)
	int32_t peekInt32() const
	{
		assert(readableBytes() >= sizeof(int32_t));
		int32_t be32 = 0;
		::memcpy(&be32, peek(), sizeof be32);
		return sockets::networkToHost32(be32);
	}

	int16_t peekInt16() const
	{
		assert(readableBytes() >= sizeof(int16_t));
		int16_t be16 = 0;
		::memcpy(&be16, peek(), sizeof be16);
		return sockets::networkToHost16(be16);
	}

	int8_t peekInt8() const
	{
		assert(readableBytes() >= sizeof(int8_t));
		int8_t x = *peek();
		return x;
	}

	///
	/// Prepend int64_t using network endian
	///
	void prependInt64(int64_t x)/* ��ǰ׺д���� */
	{
		int64_t be64 = sockets::hostToNetwork64(x);
		prepend(&be64, sizeof be64);
	}

	///
	/// Prepend int32_t using network endian
	///
	void prependInt32(int32_t x)
	{
		int32_t be32 = sockets::hostToNetwork32(x);
		prepend(&be32, sizeof be32);
	}

	void prependInt16(int16_t x)
	{
		int16_t be16 = sockets::hostToNetwork16(x);
		prepend(&be16, sizeof be16);
	}

	void prependInt8(int8_t x)
	{
		prepend(&x, sizeof x);
	}

	void prepend(const void* /*restrict*/ data, size_t len)/* �Ѵ�data��ʼ������Ϊlen�����ݼӵ�ǰ׺ */
	{
		assert(len <= prependableBytes());
		readerIndex_ -= len;
		const char* d = static_cast<const char*>(data);
		std::copy(d, d + len, begin() + readerIndex_);
	}

	void shrink(size_t reserve)/* ����Buffer�Ĵ�С��ʹ���д��ռ�Ϊreserve��С */
	{
		// FIXME: use vector::shrink_to_fit() in C++ 11 if possible.
		Buffer other;
		other.ensureWritableBytes(readableBytes() + reserve);
		other.append(toStringPiece());
		swap(other);
	}

	size_t internalCapacity() const
	{
		return buffer_.capacity();
	}

	/// Read data directly into buffer.
	///
	/// It may implement with readv(2)
	/// @return result of read(2), @c errno is saved
	size_t readFd(int fd, int* savedErrno);

private:

	char* begin()/* ����Buffer����ʼ��ַ */
	{
		return &*buffer_.begin();
	}

	const char* begin() const
	{
		return &*buffer_.begin();
	}

	void makeSpace(size_t len)/* resize���ƶ����ݣ�ʹBuffer������len������ */
	{
		if (writableBytes() + prependableBytes() < len + kCheapPrepend)/* Buffer�ܵĿռ䲻���� */
		{
			// FIXME: move readable data
			buffer_.resize(writerIndex_ + len);
		}
		else/* ͨ������ǰ�ƿ����ڳ��㹻��ռ� */
		{
			// move readable data to the front, make space inside buffer
			assert(kCheapPrepend < readerIndex_);
			size_t readable = readableBytes();
			std::copy(begin() + readerIndex_,//begin
				begin() + writerIndex_,//end
				begin() + kCheapPrepend);//destination
			readerIndex_ = kCheapPrepend;
			writerIndex_ = readerIndex_ + readable;
			assert(readable == readableBytes());
		}
	}

private:
    Sockets           m_Sockets; 
	std::vector<char> buffer_;/* �����ڴ� */
	size_t readerIndex_;/* �ɶ���ʼλ�� */
	size_t writerIndex_;/* ��д����ʼλ�� */

	static const char kCRLF[];/* �洢ƥ�䴮���� */
};

#endif
