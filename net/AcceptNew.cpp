#include "AcceptNew.h"


AcceptNew::AcceptNew(EventLoop* loop, const SocketAddr& listenAddr, bool reuseport)
{
	m_acceptSocket = CreateNonblockingSocket();
    m_CSockets.BindSocket(m_acceptSocket, listenAddr.GetAddr());
    m_CSockets.SetReuseAddr(m_acceptSocket, true);
	m_ptrAcceptChannel.reset(new  Channel(loop, m_acceptSocket));
    m_ptrLoop.reset(loop);
    m_ptrAcceptChannel->setReadCallback(std::bind(&AcceptNew::handleRead,this));
	m_bListenning = false;
}

AcceptNew::~AcceptNew()
{
}

void AcceptNew::listen()
{
	m_ptrLoop->assertInLoopThread();
	m_bListenning = true;
	if (ListenSocket(m_acceptSocket))
	{
		m_ptrAcceptChannel->enableReading();
	}
	
}

void AcceptNew::handleRead()
{
	m_ptrLoop->assertInLoopThread();
	struct sockaddr_in addr;
	memset(&addr, 0, sizeof addr);
	//FIXME loop until no more
	int nConnfd = AcceptSocket(m_acceptSocket, &addr);
    SocketAddr sockAddr(addr);
	if (nConnfd >= 0)
	{
		if (m_newConnectionCallback)
		{
			m_newConnectionCallback(nConnfd, sockAddr);
		}
		else
		{
			close(nConnfd);
		}
	}
	else
	{

#ifndef _WIN32
			/*
			The special problem of accept()ing when you can't

			Many implementations of the POSIX accept function (for example, found in post-2004 Linux)
			have the peculiar behaviour of not removing a connection from the pending queue in all error cases.

			For example, larger servers often run out of file descriptors (because of resource limits),
			causing accept to fail with ENFILE but not rejecting the connection, leading to libev signalling
			readiness on the next iteration again (the connection still exists after all), and typically
			causing the program to loop at 100% CPU usage.

			Unfortunately, the set of errors that cause this issue differs between operating systems,
			there is usually little the app can do to remedy the situation, and no known thread-safe
			method of removing the connection to cope with overload is known (to me).

			One of the easiest ways to handle this situation is to just ignore it - when the program encounters
			an overload, it will just loop until the situation is over. While this is a form of busy waiting,
			no OS offers an event-based way to handle this situation, so it's the best one can do.

			A better way to handle the situation is to log any errors other than EAGAIN and EWOULDBLOCK,
			making sure not to flood the log with such messages, and continue as usual, which at least gives
			the user an idea of what could be wrong ("raise the ulimit!"). For extra points one could
			stop the ev_io watcher on the listening fd "for a while", which reduces CPU usage.

			If your program is single-threaded, then you could also keep a dummy file descriptor for overload
			situations (e.g. by opening /dev/null), and when you run into ENFILE or EMFILE, close it,
			run accept, close that fd, and create a new dummy fd. This will gracefully refuse clients under
			typical overload conditions.

			The last way to handle it is to simply log the error and exit, as is often done with malloc
			failures, but this results in an easy opportunity for a DoS attack.
			*/
			if (errno == EMFILE)
			{
				::close(idleFd_);
				idleFd_ = ::accept(m_acceptSocket, NULL, NULL);
				::close(idleFd_);
				idleFd_ = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
			}
#endif
		}
}
