#include "Platform.h"

#ifdef _WIN32

NetworkInitializer::NetworkInitializer()
{
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsaData;
    int nRet = ::WSAStartup(wVersionRequested, &wsaData);
    if (nRet != 0)
    {
        return;
    }
}

NetworkInitializer::~NetworkInitializer()
{
    ::WSACleanup();
}

#endif