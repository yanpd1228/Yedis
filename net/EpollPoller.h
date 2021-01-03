#ifndef _YPD_EPOLL_POLLER_H_
#define _YPD_EPOLL_POLLER_H_
#include "Channel.h"
#include "EventLoop.h"
#include "Poller.h"
#include <map>
#ifndef _WIN32

class EPollPoller : public Poller
{
    public:
        EPollPoller(EventLoop* loop);
        virtual ~EPollPoller();
        virtual void poll(int timeoutMs, ChannelVec* activeChannels);
        virtual bool updateChannel(Channel* channel);
        virtual void removeChannel(Channel* channel);
        virtual bool hasChannel(Channel* channel) const;
        void assertInLoopThread() const;

    private:
        static const int kInitEventListSize = 16;

        void fillActiveChannels(int numEvents, ChannelVec* activeChannels) const;
        bool update(int operation, Channel* channel);

    private:
        typedef std::vector<struct epoll_event> EventList;

        int             m_nEpollfd;
        EventList       m_vecEvents;

        typedef std::map<int, Channel*> ChannelMap;

        ChannelMap      mapChannels;
        EventLoop*      m_ownerLoop;

};

#endif
#endif //!_YPD_EPOLL_POLLER_H_
