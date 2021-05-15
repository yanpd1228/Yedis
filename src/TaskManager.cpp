#include "TaskManager.h"
#include "YedisCommand.h"
#include "../base/ReplyBuffer.h"
#include <memory>
#include <thread>
#include <vector>

namespace Yedis
{

std::unique_ptr<std::thread> TaskManager::m_spConsumerThread;

std::deque<connAndCmd> TaskManager::m_deque;
std::mutex TaskManager::m_mtx;         
std::mutex TaskManager::m_itemCounterMtx;
std::condition_variable TaskManager::m_repositoryNotFull;    
std::condition_variable TaskManager::m_repositoryNotEmpty;
int TaskManager::m_nMAxSize = 30;


TaskManager::TaskManager()
{
}

TaskManager::~TaskManager()
{

}

void  TaskManager::init(int nThreadNum)
{
   m_spConsumerThread.reset(new std::thread(consumerTask)); 
}

void TaskManager::producer(const connAndCmd &item)
{
    std::unique_lock<std::mutex> lock(m_mtx);
    m_repositoryNotFull.wait(lock,[]{
        bool full = m_deque.size() >= m_nMAxSize;
        return !full;
           });
    m_deque.push_back(item);
    m_repositoryNotEmpty.notify_all();
    lock.unlock();
}

void TaskManager::consumer()
{
    std::unique_lock<std::mutex> lock(m_mtx);

    // 等待信号不为空的通知，wait 第二参数为true时 向下执行，否则一直等待
    m_repositoryNotEmpty.wait(lock, [] {
        bool empty = m_deque.empty();
        return !empty;
    });

    const connAndCmd temp = m_deque.front();
     
    ReplyBuffer replyBuffer;
    std::vector<std::string> vecTemp = temp.vecCmd;
    std::shared_ptr<TcpConnection> ptrTempConnection = temp.conn;
    Yedis::YCommand::getInstance().execCommand(vecTemp, replyBuffer);
    std::string srcbuf(replyBuffer.readAddr(), replyBuffer.readableSize());
    ptrTempConnection->send(srcbuf);

    m_deque.pop_front();
    m_repositoryNotFull.notify_all();
    lock.unlock();
    return;
}

void TaskManager::producerTask(const connAndCmd &item)
{
    std::unique_lock<std::mutex> lock(m_itemCounterMtx);

    producer(item);
    lock.unlock();
}

void TaskManager::consumerTask()
{
    while(1)
    {
        consumer();
    }
}


}
