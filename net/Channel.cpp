#include "Channel.h"


const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = XPOLLIN | XPOLLPRI;
const int Channel::kWriteEvent = XPOLLOUT;

Channel::Channel(EventLoop* loop, int fd) : m_Loop(loop),
m_nSockfd(fd),
m_nEvents(0),
m_nRevents(0),
m_nIndex(-1),
m_bTied(false)
{
	
}

Channel::~Channel()
{

}

void Channel::tie(const std::shared_ptr<void>& obj)
{
    tie_ = obj;
    m_bTied = true;
}


void Channel::HandleEvent()
{
	if (m_nEvents & (XPOLLERR | XPOLLNVAL))
	{
		if (errorCallback_)
			errorCallback_();
	}

	if (m_nEvents & (XPOLLIN | XPOLLPRI | XPOLLRDHUP))
	{
		/* ��������socketʱ��readCallback_ָ��Acceptor::handleRead */
		/* ���ǿͻ���socketʱ������TcpConnection::handleRead */
		if (readCallback_)
			readCallback_();
	}

	if (m_nEvents & XPOLLOUT)
	{
		/* ���������״̬����socket����writeCallback_ָ��Connector::handleWrite() */
		if (writeCallback_)
			writeCallback_();
	}
}

void Channel::remove()
{
    m_Loop->removeChannel(this);
}

bool Channel::enableReading()
{
	m_nEvents |= kReadEvent;
	return update();
}

bool Channel::disableReading()
{
	m_nEvents &= ~kReadEvent;
	return update();
}

bool Channel::enableWriting()
{
	m_nEvents |= kWriteEvent;
	return update();
}

bool Channel::disableWriting()
{
	m_nEvents &= ~kWriteEvent;
	return update();
}

bool Channel::disableAll()
{
	m_nEvents = kNoneEvent;
	return update();
}

bool Channel::update()
{
	return m_Loop->updateChannel(this);
}
