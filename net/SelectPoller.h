#ifndef _YPD_SELECT_POLLER_H_
#define _YPD_SELECT_POLLER_H_

#ifdef _WIN32
#pragma once
#include "Poller.h"
#include "../base/Platform.h"
#include <map>
#include "EventLoop.h"
class SelectPoller : public Poller
{
public:
	SelectPoller();
	~SelectPoller();
public:
    virtual void poll(int timeoutMs, ChannelVec* activeChannels);

    virtual bool updateChannel(Channel* channel);
    virtual void removeChannel(Channel* channel);

    virtual bool hasChannel(Channel* channel) const;

    //static EPollPoller* newDefaultPoller(EventLoop* loop);

    /*void assertInLoopThread() const;*/

private:
    static const int kInitEventListSize = 16;

    void fillActiveChannels(int numEvents, ChannelVec* activeChannels, fd_set& readfds, fd_set& writefds) const;
    bool update(int operation, Channel* channel);

private:
    typedef std::vector<struct epoll_event> EventList;

    int             m_nEpollfd;
    EventList       m_vecEvents;

    typedef std::map<int, Channel*> ChannelMap;

    ChannelMap      mapChannels;
    EventLoop*      m_ownerLoop;
};

#endif // _WIN32
#endif // !_YPD_SELECT_POLLER_H_
