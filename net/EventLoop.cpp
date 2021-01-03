#include "EventLoop.h"
#include "SelectPoller.h"
#include "Sockets.h"
#include "EpollPoller.h"
#include <bits/stdint-uintn.h>
#include <mutex>
#include <thread>
const int kPollTimeMs = 1;

EventLoop::EventLoop():eventHandling_(false),m_bIsInLooping(false),m_bQuit(false)
{
    m_strThreadId = std::this_thread::get_id();
	createFd();
#ifdef _WIN32
	m_wakeupChannel.reset(new Channel(this, m_wakeupFdSend));
	m_ptrPoller.reset(new SelectPoller());
#else
	m_wakeupChannel.reset(new Channel(this, m_wakeupFd));
	m_ptrPoller.reset(new EPollPoller(this));
#endif
	m_wakeupChannel->setReadCallback(std::bind(&EventLoop::handleRead, this));
	m_wakeupChannel->enableReading();
}

EventLoop::~EventLoop()
{
}

void EventLoop::loop()
{
	m_bIsInLooping = true;
	m_bQuit = false;
	while (!m_bQuit)
	{
		m_vecActiveChannels.clear();
		m_ptrPoller->poll(kPollTimeMs, &m_vecActiveChannels);
		eventHandling_ = true;
		for (const auto& it : m_vecActiveChannels)
		{
			m_currentActiveChannel = it;
			m_currentActiveChannel->HandleEvent();
		}
        doPendingFunctors();
	}
    

}

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> vecFunctors;
	m_bCallingPendingFunctor = true;

	{
		std::unique_lock<std::mutex> lock(m_mutex);
		vecFunctors.swap(m_vecPendingFunctor);
	}

	for (std::size_t i = 0; i < vecFunctors.size(); ++i)
	{
		vecFunctors[i]();
	}
	m_bCallingPendingFunctor = false;
}

void EventLoop::runInLoop(const Functor& cb)
{
    if (isInLoopThread()) 
    {
        cb();
    }
    else
    {
        queueInLoop(cb);
    }
}


void EventLoop::queueInLoop(const Functor& cb)
{
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_vecPendingFunctor.push_back(cb);
    }
    if(!isInLoopThread() || m_bCallingPendingFunctor)
    {
        wakeUp(); 
    }
}


bool EventLoop::handleRead()
{
	uint64_t one = 1;
#ifdef _WIN32
	int32_t n = m_CSockets.read(m_wakeupFdRecv, &one, sizeof(one));
#else
	int32_t n = m_CSockets.Read(m_wakeupFd, &one, sizeof(one));
#endif

	if (n != sizeof one)
	{
#ifdef _WIN32
		DWORD error = WSAGetLastError();
#else
		int error = errno;
#endif
		return false;
	}

	return true;
}

bool EventLoop::wakeUp()
{
    uint64_t one = 1;
#ifdef _WIN32
    int32_t n = m_CSockets.Write(m_wakeupFdSend, &one, sizeof(one));
#else
    int32_t n = m_CSockets.Write(m_wakeupFd, &one, sizeof(one));
#endif
    if (n != sizeof(one))
    {
#ifdef _WIN32
        DWORD error = WSAGetLastError();
#else
        int error = errno;
#endif
        return false;
    }
    return true;
}


bool EventLoop::createFd()
{
#ifdef _WIN32

	m_wakeupFdListen = m_CSockets.CreateSocket();
	m_wakeupFdSend = m_CSockets.CreateSocket();

	/* Windows上需要创建一对socket */
	struct sockaddr_in bindaddr;
	bindaddr.sin_family = AF_INET;
	bindaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
	/* 将port设为0，然后进行bind，再接着通过getsockname来获取port，这可以满足获取随机端口的情况。 */
	bindaddr.sin_port = 0;
	m_CSockets.SetReuseAddr(m_wakeupFdListen, true);
	m_CSockets.BindSocket(m_wakeupFdListen, bindaddr);
	m_CSockets.ListenSocket(m_wakeupFdListen);

	struct sockaddr_in serveraddr;
	int serveraddrlen = sizeof(serveraddr);
	if (getsockname(m_wakeupFdListen, (sockaddr*)& serveraddr, &serveraddrlen) < 0)
	{
		return false;
	}

	int useport = ntohs(serveraddr.sin_port);

	if (::connect(m_wakeupFdSend, (struct sockaddr*) & serveraddr, sizeof(serveraddr)) < 0)
	{
		return false;
	}

	struct sockaddr_in clientaddr;
	socklen_t clientaddrlen = sizeof(clientaddr);
	m_wakeupFdRecv = ::accept(m_wakeupFdListen, (struct sockaddr*) & clientaddr, &clientaddrlen);
	if (m_wakeupFdRecv < 0)
	{
		return false;
	}

	m_CSockets.SetNonBlockSocket(m_wakeupFdSend);
	m_CSockets.SetNonBlockSocket(m_wakeupFdRecv);

#else
	/* Linux上一个eventfd就够了，可以实现读写 */
	m_wakeupFd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (m_wakeupFd < 0)
	{
		return false;
	}

#endif

	return true;
}


void EventLoop::quit()
{
}

bool EventLoop::updateChannel(Channel* channel)
{
	if (channel->ownerLoop() != this)
	{
		return false;
	}
	return m_ptrPoller->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
    if(channel->ownerLoop() != this)
    {
        return;
    }

    m_ptrPoller->removeChannel(channel);

}

bool EventLoop::hasChannel(Channel* channel)
{
	return false;
}

void EventLoop::abortNotInLoopThread()
{
}
