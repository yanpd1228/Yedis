#include "Buffer.h"

#include "Callbacks.h"

using namespace net;

const char Buffer::kCRLF[] = "\r\n";

const size_t Buffer::kCheapPrepend;
const size_t Buffer::kInitialSize;

size_t  Buffer::readFd(int fd, int* savedErrno)
{
	// saved an ioctl()/FIONREAD call to tell how much to read
	char extrabuf[65536];
	const size_t writable = writableBytes();
#ifndef _WIN32
	struct iovec vec[2];

	vec[0].iov_base = begin() + writerIndex_;
	vec[0].iov_len = writable;
	vec[1].iov_base = extrabuf;
	vec[1].iov_len = sizeof extrabuf;
	// when there is enough space in this buffer, don't read into extrabuf.
	// when extrabuf is used, we read 128k-1 bytes at most.
	const int iovcnt = (writable < sizeof extrabuf) ? 2 : 1;
	const ssize_t n = m_Sockets.Readv(fd, vec, iovcnt);
#else
	const int32_t n = sockets::read(fd, extrabuf, sizeof(extrabuf));
#endif
	if (n <= 0)
	{
#ifdef _WIN32
		* savedErrno = ::WSAGetLastError();
#else
		* savedErrno = errno;
#endif
	}
	else if (implicit_cast<size_t>(n) <= writable)
	{
#ifdef _WIN32
		/* Windowsƽ̨��Ҫ�ֶ��ѽ��յ������ݼ���buffer�У�Linuxƽ̨�Ѿ��� struct iovec ��ָ���˻�����д��λ�� */
		append(extrabuf, n);
#else
		writerIndex_ += n;
#endif
	}
	else
	{
#ifdef _WIN32
		/* Windowsƽֱ̨�ӽ����е��ֽڷ��뻺����ȥ */
		append(extrabuf, n);
#else
		/* Linuxƽ̨��ʣ�µ��ֽڲ���ȥ */
		writerIndex_ = buffer_.size();
		append(extrabuf, n - writable);
#endif
	}
	return n;
}
