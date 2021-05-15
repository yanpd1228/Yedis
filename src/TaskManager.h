#ifndef _YEDIS_TASKMANAGER_H_
#define _YEDIS_TASKMANAGER_H_
#include <vector>
#include <deque>
#include <unordered_map>
#include <memory>

#include "../net/TcpConnection.h"

namespace Yedis{
struct connAndCmd
{
    std::shared_ptr<TcpConnection> conn;
    std::vector<std::string>       vecCmd;
};
class TaskManager
{
public:
    TaskManager();
    ~TaskManager();

public:
    void init(int nThreadNum);

public:
    //生产产品
    static void producer(const connAndCmd& item);
    //消费产品
    static void consumer();
    //生产任务
    static void producerTask(const connAndCmd& item);
    //消费任务
    static void consumerTask();

private:
    //任务队列
    static std::deque<connAndCmd> m_deque;
    // 互斥量,保护产品缓冲区
    static std::mutex m_mtx;         
    static std::mutex m_itemCounterMtx;
    // 条件变量, 指产品仓库缓冲区不为满
    static std::condition_variable m_repositoryNotFull;    
    // 条件变量, 指产品仓库缓冲区不为空
    static std::condition_variable m_repositoryNotEmpty;
    //队列最大长度
    static int m_nMAxSize;
    //消费者线程
    static std::unique_ptr<std::thread>     m_spConsumerThread;
    
};
}
#endif //!_YEDIS_TASKMANAGER_H_
