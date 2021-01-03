#ifndef _SOCKETS_H_
#define _SOCKETS_H_

#include <bits/stdint-intn.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#pragma comment(lib,"ws2_32.lib")
#endif // !WIN32
#include <iostream>
#include <string.h>
#include <sys/types.h>
#ifndef _WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <arpa/inet.h>  //for htonl() and htons()
#include <unistd.h>
#endif
#include "Platform.h"
#include <fcntl.h>
#include <list>
#include <errno.h>
#include <time.h>
#include <sstream>
//#include <iomanip> //for std::setw()/setfill()

class Sockets
{
public:
	Sockets() { m_ServerSockfd = -1; };
	Sockets(int sockfd) : m_ServerSockfd(sockfd)
	{ };
	~Sockets();
	
public:
	/* 创建阻塞的socket */
	SOCKET CreateSocket();

	/* 创建非阻塞的socket */
	SOCKET CreateNonblockingSocket();

	/* 接受新连接产生一个新的客户端socket */
	SOCKET AcceptSocket(SOCKET sockfd, struct sockaddr_in* addr);

	/* 获取当前产生的服务端socket */
	SOCKET GetSocket() 
	{ 
		return m_ServerSockfd;
	};

	/* 将socket设置为非阻塞的； */
	void   SetNonBlockSocket(SOCKET sockfd);

	/* 绑定 socket */
	bool BindSocket(SOCKET sockfd, const struct sockaddr_in& addr);

	/* 监听 socket */
	bool ListenSocket(SOCKET sockfd);

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	///SO_REUSEADDR可以用在以下四种情况下。 (摘自《Unix网络编程》卷一，即UNPv1)
	///
	///	1、当有一个有相同本地地址和端口的socket1处于TIME_WAIT状态时，而你启动的程序的socket2要占用该地址和端口，
	///    你的程序就要用到该选项。
	/// 2、SO_REUSEADDR允许同一port上启动同一服务器的多个实例(多个进程)。
	///   但每个实例绑定的IP地址是不能相同的。在有多块网卡或用IP Alias技术的机器可以测试这种情况。
	///	3、SO_REUSEADDR允许单个进程绑定相同的端口到多个socket上，但每个socket绑定的ip地址不同。这和2很相似，区别请看UNPv1。
	///	4、SO_REUSEADDR允许完全相同的地址和端口的重复绑定。但这只用于UDP的多播，不用于TCP。
	///
	void SetReuseAddr(SOCKET sockfd, bool on);

	void SetReusePort(SOCKET sockfd, bool on);

	int32_t Read(SOCKET sockfd, void* buf, int32_t count);

    int32_t Write(SOCKET sockfd, const void* buf, int32_t count);


	void close(SOCKET sockfd);

public:
	const struct sockaddr* sockaddr_cast(const struct sockaddr_in* addr);

	struct sockaddr* sockaddr_cast(struct sockaddr_in* addr);

	const struct sockaddr_in* sockaddr_in_cast(const struct sockaddr* addr);

	struct sockaddr_in* sockaddr_in_cast(struct sockaddr* addr);

private:
	int m_ServerSockfd;
};


#endif //!_SOCKETS_H_

