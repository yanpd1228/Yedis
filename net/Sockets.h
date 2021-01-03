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
	/* ����������socket */
	SOCKET CreateSocket();

	/* ������������socket */
	SOCKET CreateNonblockingSocket();

	/* ���������Ӳ���һ���µĿͻ���socket */
	SOCKET AcceptSocket(SOCKET sockfd, struct sockaddr_in* addr);

	/* ��ȡ��ǰ�����ķ����socket */
	SOCKET GetSocket() 
	{ 
		return m_ServerSockfd;
	};

	/* ��socket����Ϊ�������ģ� */
	void   SetNonBlockSocket(SOCKET sockfd);

	/* �� socket */
	bool BindSocket(SOCKET sockfd, const struct sockaddr_in& addr);

	/* ���� socket */
	bool ListenSocket(SOCKET sockfd);

	/////////////////////////////////////////////////////////////////////////////////////////////////////
	///SO_REUSEADDR��������������������¡� (ժ�ԡ�Unix�����̡���һ����UNPv1)
	///
	///	1������һ������ͬ���ص�ַ�Ͷ˿ڵ�socket1����TIME_WAIT״̬ʱ�����������ĳ����socket2Ҫռ�øõ�ַ�Ͷ˿ڣ�
	///    ��ĳ����Ҫ�õ���ѡ�
	/// 2��SO_REUSEADDR����ͬһport������ͬһ�������Ķ��ʵ��(�������)��
	///   ��ÿ��ʵ���󶨵�IP��ַ�ǲ�����ͬ�ġ����ж����������IP Alias�����Ļ������Բ������������
	///	3��SO_REUSEADDR���������̰���ͬ�Ķ˿ڵ����socket�ϣ���ÿ��socket�󶨵�ip��ַ��ͬ�����2�����ƣ������뿴UNPv1��
	///	4��SO_REUSEADDR������ȫ��ͬ�ĵ�ַ�Ͷ˿ڵ��ظ��󶨡�����ֻ����UDP�Ķಥ��������TCP��
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

