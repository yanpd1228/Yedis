#ifndef _EVENT_LOOP_THREAD_H_
#define _EVENT_LOOP_THREAD_H_
#include <mutex>
#include "EventLoop.h"
#include <functional>
class EventLoopThread
{
public:
	EventLoopThread();
	~EventLoopThread();

public:
	EventLoop* StartLoop();
	void StopLoop();

private:
	void                        ThreadFunc();

	EventLoop*                   m_loop;
	bool                         m_bExiting;
	std::unique_ptr<std::thread> m_ptrThread;
	std::mutex                   m_Mutex_;
	std::condition_variable      m_Cond;

};
#endif //!_EVENT_LOOP_THREAD_H_
