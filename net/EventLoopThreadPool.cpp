#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include "Ycast.h"
#include <stdio.h>
#include <string.h>
#include <sstream>

EventLoopThreadPool::EventLoopThreadPool()
{
    m_nNext = 0;
    m_bStarted = false;
    m_nThreadNumbers = 0;
    m_baseLoop = nullptr;
}

EventLoopThreadPool::~EventLoopThreadPool()
{

}


void EventLoopThreadPool::Init(EventLoop* baseLoop, int numThreads)
{
	m_baseLoop = baseLoop;
	m_nThreadNumbers = numThreads;
}

void EventLoopThreadPool::Start()
{
	if (m_baseLoop == NULL)
	{
		return;
	}

	if (m_bStarted)
	{
		return;
	}
	//TODO：判断开启的线程ID
	/*m_baseLoop->assertInLoopThread();*/

	m_bStarted = true;

	for (int i = 0; i < m_nThreadNumbers; i++)
	{
		std::unique_ptr<EventLoopThread> t(new EventLoopThread());
		m_vecLoops.push_back(t->StartLoop());
		m_vecThreads.push_back(std::move(t));
	}
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
    if(!m_bStarted)
    {
        return NULL;
    }
    EventLoop* TempLoop = m_baseLoop;
    
    if(!m_vecLoops.empty())
    {
        TempLoop = m_vecLoops[m_nNext];
        ++m_nNext;
        if (implicit_cast<size_t>(m_nNext) >= m_vecLoops.size())
        {
            m_nNext = 0;
        }
    }
    return TempLoop;
}
