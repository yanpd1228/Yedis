#ifndef _CHANNEL_H_
#define _CHANNEL_H_
#ifdef _WIN32
#pragma once
#endif
#include <memory>
#include <functional>
#include "EventLoop.h"
#include "../base/Platform.h"
class EventLoop;
typedef std::function<void()> EventCallback;
class Channel
{
public:
	Channel(EventLoop* loop, int fd);
	~Channel();
	void HandleEvent();
	void setReadCallback(const EventCallback& cb)
	{
		readCallback_ = cb;
	}
	void setWriteCallback(const EventCallback& cb)
	{
		writeCallback_ = cb;
	}
	void setCloseCallback(const EventCallback& cb)
	{
		closeCallback_ = cb;
	}
	void setErrorCallback(const EventCallback& cb)
	{
		errorCallback_ = cb;
	}

	int fd() const { return m_nSockfd; }
	int events() const { return m_nEvents; }
	void setRevents(int revt) { m_nEvents = revt; }  // used by pollers
	void addRevents(int revt) { m_nEvents |= revt; } // used by pollers
	bool isNoneEvent() const { return m_nEvents == kNoneEvent; }

	bool enableReading();
	bool disableReading();
	bool enableWriting();
	bool disableWriting();
	bool disableAll();

	bool isWriting() const { return m_nEvents & kWriteEvent; }
	EventLoop* ownerLoop() { return m_Loop; }

	int index() { return m_nIndex; }
	void set_index(int idx) { m_nIndex = idx; }
    void tie(const std::shared_ptr<void>&);
    void remove();
private:
	bool update();
private:
	static const int            kNoneEvent;
	static const int            kReadEvent;
	static const int            kWriteEvent;

	EventCallback               readCallback_;
	EventCallback               writeCallback_;
	EventCallback               closeCallback_;
	EventCallback               errorCallback_;

	EventLoop*                  m_Loop;
	const int                   m_nSockfd;

	int                         m_nEvents;
	int                         m_nIndex; // used by Poller.

	std::weak_ptr<void>         tie_;
	bool                        m_bTied;


};



#endif //!_CHANNEL_H_

