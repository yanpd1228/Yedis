#include "Server.h"
#include "SocketAddr.h"
#include "EventLoop.h"
#include "TaskManager.h"
#include "YedisServer.h"
#include "YedisRegisterCmd.h"

EventLoop g_mainLoop;


int main(int argc, char* argv[])
{
    std::string strHost("0.0.0.0");
    Yedis::YedisRegisterCmd registerCmd;
    registerCmd.init();

    Yedis::TaskManager taskManager;
    taskManager.init(1);

    YedisServer yeServer;    
    yeServer.init(strHost, "9900", &g_mainLoop);

    g_mainLoop.loop();

	return 0;
}
