#include "Server.h"
#include "SocketAddr.h"
#include "EventLoop.h"

EventLoop g_mainLoop;


int main(int argc, char* argv[])
{
    SocketAddr sockaddr("0.0.0.0:9900");
    
    std::shared_ptr<Server> ptrServer = std::make_shared<Server>(&g_mainLoop, sockaddr, "test"); 
    ptrServer->Start(6);

    g_mainLoop.loop();

	return 0;
}
