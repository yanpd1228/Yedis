#ifndef _EVENT_LOOP_H_
#define _EVENT_LOOP_H_
#ifdef _WIN32
#pragma once
#endif
#include <functional>
#include "Channel.h"
#include "Poller.h"
#include "Sockets.h"
#include <thread>
#include <vector>
#include <condition_variable>
class Channel;
class EventLoop
{
public:
	EventLoop();
	~EventLoop();
public:
	typedef std::function<void()> Functor;
	/* 主要循环函数，可能在许多线程里面都需要 */
	void loop();
	/* 退出循环函数 */
	void quit();


	bool updateChannel(Channel* channel);
	void removeChannel(Channel* channel);
	bool hasChannel(Channel* channel);

	void assertInLoopThread()
	{
		if (!isInLoopThread())
		{
			abortNotInLoopThread();
		}
	}


    void runInLoop(const Functor& cb);
    
    void queueInLoop(const Functor& cb);

	bool isInLoopThread() const { return m_strThreadId == std::this_thread::get_id(); }

	bool eventHandling() const { return eventHandling_; }

	const std::thread::id getThreadID() const
	{
		return m_strThreadId;
	}
private:
	bool createFd();
	void abortNotInLoopThread();
	bool handleRead();
    bool wakeUp();
    void doPendingFunctors();

private:
#ifdef _WIN32
	SOCKET  m_wakeupFdSend;
	SOCKET  m_wakeupFdListen;
	SOCKET  m_wakeupFdRecv;

#else
	SOCKET  m_wakeupFd;
#endif

private:
	typedef std::vector<Channel*> ChannelVec;
	std::unique_ptr<Channel>      m_wakeupChannel;
	std::thread::id               m_strThreadId;
	bool                          eventHandling_; /* 原子的 */
	std::unique_ptr<Poller>       m_ptrPoller;
	bool                          m_bIsInLooping;
	bool                          m_bQuit;
	ChannelVec                    m_vecActiveChannels;
	Channel*                      m_currentActiveChannel;
	Sockets                       m_CSockets; 
    std::mutex                    m_mutex;
    std::vector<Functor>          m_vecPendingFunctor;
    bool                          m_bCallingPendingFunctor;	
};
#endif //!_EVENT_LOOP_H_
