#include "Sockets.h"

#ifndef _YPD_SOCKET_ADDR_H_
#define _YPD_SOCKET_ADDR_H_
struct SocketAddr
{
	SocketAddr()
	{
		Clear();
	}

	SocketAddr(const SocketAddr& other)
	{
		memcpy(&_addr, &other._addr, sizeof _addr);
	}

	SocketAddr& operator= (const SocketAddr& other)
	{
		if (this != &other)
		{
			memcpy(&_addr, &other._addr, sizeof _addr);
		}

		return *this;
	}

	SocketAddr(const sockaddr_in& addr)
	{
		Init(addr);
	}

	/* ¿‡–Õ 127.0.0.1:8008*/
	SocketAddr(const std::string& ipport)
	{
		std::string::size_type p = ipport.find_first_of(':');
		std::string ip = ipport.substr(0, p);
		std::string port = ipport.substr(p + 1);

		Init(ip.c_str(), static_cast<uint16_t>(std::stoi(port)));
	}

	~SocketAddr() {};

	const sockaddr_in& GetAddr() const
	{
		return _addr;
	}

	const char* GetIP() const
	{
		return ::inet_ntoa(_addr.sin_addr);
	}

	const char* GetIP(char* buf, socklen_t size) const
	{
		return ::inet_ntop(AF_INET, (const char*)& _addr.sin_addr, buf, size);
	}

	unsigned short GetPort() const
	{
		return ntohs(_addr.sin_port);
	}

	std::string ToString() const
	{
		char tmp[32];
		const char* res = inet_ntop(AF_INET,
			&_addr.sin_addr,
			tmp,
			(socklen_t)(sizeof tmp));

		return std::string(res) + ":" + std::to_string(ntohs(_addr.sin_port));
	}

	void Init(const sockaddr_in& addr)
	{
		memcpy(&_addr, &addr, sizeof(addr));
	}

	void Init(uint32_t nNetIp, uint16_t nNetPort)
	{
		_addr.sin_family = AF_INET;
		_addr.sin_family = nNetIp;
		_addr.sin_port = nNetPort;
	}

	void Init(const char* ip, uint16_t hostport)
	{
		_addr.sin_family = AF_INET;
		_addr.sin_addr.s_addr = ::inet_addr(ip);
		_addr.sin_port = htons(hostport);
	}

	bool  Empty() const 
	{ 
		return  0 == _addr.sin_family;
	}

	void Clear()
	{ 
		memset(&_addr, 0, sizeof _addr);
	}

	inline friend bool operator== (const SocketAddr& a, const SocketAddr& b)
	{
		return a._addr.sin_family == b._addr.sin_family &&
			a._addr.sin_addr.s_addr == b._addr.sin_addr.s_addr &&
			a._addr.sin_port == b._addr.sin_port;
	}

	inline friend bool operator!= (const SocketAddr& a, const SocketAddr& b)
	{
		return !(a == b);
	}

	sockaddr_in _addr;
};

#endif //  _YPD_SOCKET_ADDR_H_

