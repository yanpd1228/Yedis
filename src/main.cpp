#include "Server.h"
#include "SocketAddr.h"
#include "EventLoop.h"
#include "YedisServer.h"

EventLoop g_mainLoop;


int main(int argc, char* argv[])
{
    std::string strHost("0.0.0.0");
    
    YedisServer yeServer;    
    yeServer.init(strHost, "9900", &g_mainLoop);

    g_mainLoop.loop();

	return 0;
}
