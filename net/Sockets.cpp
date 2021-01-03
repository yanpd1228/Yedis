#include "Sockets.h"
#include "Platform.h"
#include <bits/stdint-intn.h>
#include <unistd.h>
#ifdef _WIN32
#include <io.h>
#endif


Sockets::~Sockets()
{
}
//typedef int          socklen_t;
template<typename To, typename From>
inline To implicit_cast(From const& f)
{
	return f;
}

const struct sockaddr* Sockets::sockaddr_cast(const struct sockaddr_in* addr)
{
	return static_cast<const struct sockaddr*>(implicit_cast<const void*>(addr));
}

struct sockaddr* Sockets::sockaddr_cast(struct sockaddr_in* addr)
{
	return static_cast<struct sockaddr*>(implicit_cast<void*>(addr));
}

const struct sockaddr_in* Sockets::sockaddr_in_cast(const struct sockaddr* addr)
{
	return static_cast<const struct sockaddr_in*>(implicit_cast<const void*>(addr));
}

struct sockaddr_in* Sockets::sockaddr_in_cast(struct sockaddr* addr)
{
	return static_cast<struct sockaddr_in*>(implicit_cast<void*>(addr));
}


SOCKET Sockets::CreateSocket()
{
#ifdef _WIN32
	WSADATA wsa;
	/* 初始化套接字DLL */
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return -1;
	}
	SOCKET nSockfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (nSockfd < 0)
	{
		return -1;
	}
#else
	SOCKET nSockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
	if (nSockfd < 0)
	{
		return -1;
	}
#endif

	return nSockfd;
}

SOCKET Sockets::CreateNonblockingSocket()
{
#ifdef _WIN32
	WSADATA wsa;
	/* 初始化套接字DLL */
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return -1;
	}
	SOCKET nSockfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (nSockfd < 0)
	{
		return -1;
	}
#else
	SOCKET nSockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
	if (nSockfd < 0)
	{
		return -1;
	}
#endif
	SetNonBlockSocket(nSockfd);
	return nSockfd;
}

SOCKET Sockets::AcceptSocket(SOCKET sockfd, struct sockaddr_in* addr)
{
	socklen_t addrlen = static_cast<socklen_t>(sizeof * addr);
#ifdef _WIN32
	SOCKET nConnfd = ::accept(sockfd, sockaddr_cast(addr), &addrlen);
	SetNonBlockSocket(nConnfd);
#else  
	SOCKET nConnfd = ::accept4(sockfd, sockaddr_cast(addr), &addrlen, SOCK_NONBLOCK | SOCK_CLOEXEC);
#endif
	
	return nConnfd;
}

void Sockets::SetNonBlockSocket(SOCKET sockfd)
{
#ifdef _WIN32
	/* 将socket设置成非阻塞的 */
	unsigned long on = 1;
	::ioctlsocket(sockfd, FIONBIO, &on);
#else
	// non-block
	int flags = ::fcntl(sockfd, F_GETFL, 0);
	flags |= O_NONBLOCK;
	int ret = ::fcntl(sockfd, F_SETFL, flags);
	// FIXME check

	// close-on-exec
	flags = ::fcntl(sockfd, F_GETFD, 0);
	flags |= FD_CLOEXEC;
	ret = ::fcntl(sockfd, F_SETFD, flags);
	// FIXME check

	(void)ret;
#endif    
}

bool Sockets::BindSocket(SOCKET sockfd, const struct sockaddr_in& addr)
{
	int nRet = ::bind(sockfd, sockaddr_cast(&addr), static_cast<socklen_t>(sizeof addr));
	if (nRet < 0)
	{
		return false;
	}
	return true;
}

bool Sockets::ListenSocket(SOCKET sockfd)
{
	int nRet = ::listen(sockfd, SOMAXCONN);
	if (nRet < 0)
	{
		return false;
	}
	return true;
}


void Sockets::SetReuseAddr(SOCKET sockfd, bool on)
{
	int optval = on ? 1 : 0;
#ifdef _WIN32
	::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)& optval, sizeof(optval));
#else
	::setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, static_cast<socklen_t>(sizeof optval));
#endif
}

void Sockets::SetReusePort(SOCKET sockfd, bool on)
{
	/* Windows 系统没有 SO_REUSEPORT 选项 */
#ifndef _WIN32
	int optval = on ? 1 : 0;
	int ret = ::setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &optval, static_cast<socklen_t>(sizeof optval));
	if (ret < 0 && on)
	{
		//LOGSYSE("SO_REUSEPORT failed.");
	}
#endif
}

int32_t Sockets::Read(SOCKET sockfd, void* buf, int32_t count)
{
#ifdef _WIN32
	return ::recv(sockfd, (char*)buf, count, 0);
#else
	return ::read(sockfd, buf, count);
#endif
}

int32_t Sockets::Write(int sockfd, const void *buf, int32_t count)
{
#ifdef _WIN32
    return ::send(sockfd, (const char*)buf, count, 0);
#else
    return ::write(sockfd, buf, count);
#endif
}

void Sockets::close(SOCKET sockfd)
{
#ifdef _WIN32   
	if (::closesocket(sockfd) < 0)
#else
	if (::close(sockfd) < 0)
    {

    }
#endif

}
