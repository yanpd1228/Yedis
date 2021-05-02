#include "EventLoopThread.h"

EventLoopThread::EventLoopThread()
{
	m_bExiting = false;
	m_loop = NULL;
}

EventLoopThread::~EventLoopThread()
{
}



EventLoop* EventLoopThread::StartLoop()
{
	m_ptrThread.reset(new std::thread(std::bind(&EventLoopThread::ThreadFunc, this)));

	{
		std::unique_lock<std::mutex> lock(m_Mutex_);
		while (m_loop == NULL)
		{
			m_Cond.wait(lock);
		}
	}

	return m_loop;
}

void EventLoopThread::StopLoop()
{
	/*if (m_loop != NULL)
		m_loop->quit();*/

	m_ptrThread->join();
}

void EventLoopThread::ThreadFunc()
{
	EventLoop loop;
	{
		//һ��һ�����̴߳���
		std::unique_lock<std::mutex> lock(m_Mutex_);
		m_loop = &loop;
		m_Cond.notify_all();
	}
	loop.loop();
}
