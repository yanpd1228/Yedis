#ifndef _EVENT_LOOP_THREAD_POOL_H_
#define _EVENT_LOOP_THREAD_POOL_H_

#ifdef _WIN32
#pragma once
#endif // WIN32
#include "EventLoop.h"
#include "EventLoopThread.h"
#include <iostream>
#include <vector>
#include <memory>
#include <thread>
class EventLoopThreadPool
{
public:
	EventLoopThreadPool();
    ~EventLoopThreadPool();
	EventLoopThreadPool(const EventLoopThreadPool& rhs) = delete;
	EventLoopThreadPool& operator=(const EventLoopThreadPool& rhs) = delete;
public:
	//初始化线程池所需参数
	void Init(EventLoop* baseLoop, int numThreads);
	//开启线程池
	void Start();
	////判断当前线程ID
	//bool isInLoopThread() const { return m_strCurrentThreadId == std::this_thread::get_id(); }
    //获取下一个Eventloop 循环
    EventLoop* getNextLoop();


private:
	EventLoop*                                      m_baseLoop;
	bool                                            m_bStarted;
	int                                             m_nThreadNumbers;
	int                                             m_nNext;
	std::vector<std::shared_ptr<EventLoopThread>>   m_vecThreads;
	std::vector<EventLoop*>                         m_vecLoops;
	//std::thread::id                                 m_strCurrentThreadId;
};

#endif // !_EVENT_LOOP_THREAD_POOL_H_

