#include "EpollPoller.h"
#include "Channel.h"
#include <sys/epoll.h>
#ifndef _WIN32 

namespace
{
	const int kNew = -1;
	const int kAdded = 1;
	const int kDeleted = 2;
}
EPollPoller::EPollPoller(EventLoop* loop) : m_nEpollfd(::epoll_create1(EPOLL_CLOEXEC)), m_ownerLoop(loop)
{
    m_vecEvents.resize(16);
}

EPollPoller::~EPollPoller()
{
    
}


void EPollPoller::poll(int timeoutMs, ChannelVec* activeChannels)
{
    int numEvents = ::epoll_wait(m_nEpollfd,
    &*m_vecEvents.begin(),
    static_cast<int>(m_vecEvents.size()),
    timeoutMs);
    int savedErrno = errno;
    if (numEvents > 0)
    {
        fillActiveChannels(numEvents, activeChannels);
        if (static_cast<size_t>(numEvents) == m_vecEvents.size())
        {
            m_vecEvents.resize(m_vecEvents.size() * 2);
        }
    }
    else if (numEvents == 0)
    {

    }
    else
    {
        if (savedErrno != EINTR)
        {
            errno = savedErrno;
        }
    }
    
}

void EPollPoller::fillActiveChannels(int numEvents, ChannelVec* activeChannels) const
{
    for (int i = 0; i < numEvents; ++i)
    {
        Channel* channel = (Channel*)(m_vecEvents[i].data.ptr);
        int fd = channel->fd();
        ChannelMap::const_iterator it = mapChannels.find(fd);
        if (it == mapChannels.end() || it->second != channel)
        {
            return;
        }
        channel->setRevents(m_vecEvents[i].events);
        activeChannels->push_back(channel);
    }
}

bool EPollPoller::updateChannel(Channel* channel)
{
	const int index = channel->index();
	if (index == kNew || index == kDeleted)
	{
		int fd = channel->fd();
		if (index == kNew)
		{
			if (mapChannels.find(fd) != mapChannels.end())
			{
				return false;
			}


			mapChannels[fd] = channel;

		}
		else // index == kDeleted
		{
			if (mapChannels.find(fd) == mapChannels.end())
			{
				return false;
			}

			if (mapChannels[fd] != channel)
			{
				return false;
			}
		}
		channel->set_index(kAdded);

		return update(XEPOLL_CTL_ADD, channel);
	}
	else
	{
		int fd = channel->fd();
		if (mapChannels.find(fd) == mapChannels.end() || mapChannels[fd] != channel || index != kAdded)
		{
			return false;
		}

		if (channel->isNoneEvent())
		{
			if (update(XEPOLL_CTL_DEL, channel))
			{
				channel->set_index(kDeleted);
				return true;
			}
			return false;
		}
		else
		{
			return update(XEPOLL_CTL_MOD, channel);
		}
	}
	return false;
}

void EPollPoller::removeChannel(Channel* channel)
{
	int fd = channel->fd();
	if (mapChannels.find(fd) == mapChannels.end())
		return;

	if (mapChannels[fd] != channel)
	{
		return;
	}

	if (!channel->isNoneEvent())
		return;

	int index = channel->index();
	size_t n = mapChannels.erase(fd);
	if (n != 1)
	{
		return;
	}
	channel->set_index(kNew);
}

bool EPollPoller::hasChannel(Channel* channel) const
{
	ChannelMap::const_iterator it = mapChannels.find(channel->fd());
	return it != mapChannels.end() && it->second == channel;
}

bool EPollPoller::update(int operation, Channel* channel)
{
	int fd = channel->fd();
	struct epoll_event event;
	memset(&event, 0, sizeof event);
	event.events = channel->events();
	event.data.ptr = channel;
    if (::epoll_ctl(m_nEpollfd, operation, fd, &event) < 0)
    {
        return false;
    }

	return true;
}
#endif
