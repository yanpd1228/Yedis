#include "SelectPoller.h"
#include <sstream>
#ifdef _WIN32
namespace
{
	const int kNew = -1;
	const int kAdded = 1;
	const int kDeleted = 2;
}
SelectPoller::SelectPoller():m_nEpollfd(-1), m_ownerLoop(NULL)
{
}

SelectPoller::~SelectPoller()
{
}

void SelectPoller::poll(int timeoutMs, ChannelVec* activeChannels)
{

	fd_set readfds, writefds;
	FD_ZERO(&readfds);
	FD_ZERO(&writefds);

	//获取最大fd 
	int maxfd = 0;

	int tmpfd;
	for (const auto& iter : mapChannels)
	{
		tmpfd = iter.first;
		if (tmpfd > maxfd)
			maxfd = tmpfd;

		if (iter.second->events() & XPOLLIN)
			FD_SET(tmpfd, &readfds);

		if (iter.second->events() & XPOLLOUT)
			FD_SET(tmpfd, &writefds);
	}

	struct timeval timeout;
	timeout.tv_sec = timeoutMs / 1000;
	timeout.tv_usec = (timeoutMs - timeoutMs / 1000 * 1000) * 1000;
	int nEvents = select(maxfd + 1, &readfds, &writefds, NULL, &timeout);
	if (nEvents > 0)
	{
		fillActiveChannels(nEvents, activeChannels, readfds, writefds);
		if (static_cast<size_t>(nEvents) == m_vecEvents.size())
		{
			m_vecEvents.resize(m_vecEvents.size() * 2);
		}
	}
}

bool SelectPoller::updateChannel(Channel* channel)
{
	const int index = channel->index();
	if (index == kNew || index == kDeleted)
	{
		// a new one, add with XEPOLL_CTL_ADD
		int fd = channel->fd();
		if (index == kNew)
		{
			//assert(channels_.find(fd) == channels_.end())
			if (mapChannels.find(fd) != mapChannels.end())
			{
				return false;
			}


			mapChannels[fd] = channel;

		}
		else // index == kDeleted
		{
			//assert(channels_.find(fd) != channels_.end());
			if (mapChannels.find(fd) == mapChannels.end())
			{
				return false;
			}

			//assert(channels_[fd] == channel);
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
		// update existing one with XEPOLL_CTL_MOD/DEL
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

void SelectPoller::removeChannel(Channel* channel)
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

bool SelectPoller::hasChannel(Channel* channel) const
{
	ChannelMap::const_iterator it = mapChannels.find(channel->fd());
	return it != mapChannels.end() && it->second == channel;
}

void SelectPoller::fillActiveChannels(int numEvents, ChannelVec* activeChannels, fd_set& readfds, fd_set& writefds) const
{
	Channel* channel = NULL;
	bool eventTriggered = false;
	//TODO：每次遍历mapChannels，效率太低，能否改进？
	int currentCount = 0;
	for (const auto& iter : mapChannels)
	{
		channel = iter.second;

		if (FD_ISSET(iter.first, &readfds))
		{
			channel->addRevents(XPOLLIN);
			eventTriggered = true;
		}

		if (FD_ISSET(iter.first, &writefds))
		{
			channel->addRevents(XPOLLOUT);
			eventTriggered = true;
		}

		if (eventTriggered)
		{
			activeChannels->push_back(channel);
			//重置标志
			eventTriggered = false;

			++currentCount;
			if (currentCount >= numEvents)
				break;
		}
	}// end for-loop
}

bool SelectPoller::update(int operation, Channel* channel)
{
	int fd = channel->fd();
	if (operation == XEPOLL_CTL_ADD)
	{
		struct epoll_event event;
		memset(&event, 0, sizeof event);
		event.events = channel->events();
		event.data.ptr = channel;

		m_vecEvents.push_back(std::move(event));
		return true;
	}

	if (operation == XEPOLL_CTL_DEL)
	{
		for (auto iter = m_vecEvents.begin(); iter != m_vecEvents.end(); ++iter)
		{
			if (iter->data.ptr == channel)
			{
				m_vecEvents.erase(iter);
				return true;
			}
		}
	}
	else if (operation == XEPOLL_CTL_MOD)
	{
		for (auto iter = m_vecEvents.begin(); iter != m_vecEvents.end(); ++iter)
		{
			if (iter->data.ptr == channel)
			{
				iter->events = channel->events();
				return true;
			}
		}
	}


	std::ostringstream os;
	os << "SelectPoller update fd failed, op = " << operation << ", fd = " << fd;
	os << ", events_ content: \n";
	for (const auto& iter : m_vecEvents)
	{
		os << "fd: " << iter.data.fd << ", Channel: 0x%x: " << iter.data.ptr << ", events: " << iter.events << "\n";
	}
	return false;
}
#endif
