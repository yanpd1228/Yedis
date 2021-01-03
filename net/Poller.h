#ifndef _POLLER_H_
#define _POLLER_H_
#include <vector>
class Channel;
class Poller
{
public:
	Poller();
	~Poller();
public:
	typedef std::vector<Channel*> ChannelVec;

	virtual void poll(int timeoutMs, ChannelVec* activeChannels) = 0;

	virtual bool updateChannel(Channel* channel) = 0;

	virtual void removeChannel(Channel* channel) = 0;

	virtual bool hasChannel(Channel* channel) const = 0;

};
#endif