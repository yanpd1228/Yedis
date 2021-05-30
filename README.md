# Yedis

C++ 实现的redis
本项目是自己想学习C++11的练手作品，本来只打算仿照陈硕先生的muduo做一个网络库，但是在写的过程中了解了redis的一些数据，所以基于C++的STL容器实现了Redis的数据结构,并实现了部分Redis命令，
项目完全兼容Redis协议，暂未实现Redis的设置key过期的功能。
其中Redis的string是用std::string实现的，list是std::list,set对应std::unorded_set,hash对应std::unorded_map,zset主要就是两个映射：**从分数可以查成员名字，**
**从名字也可以反查分数**。其中前者要求有序，所以用map，Redis用了跳表也是有序。后者无序，所以std::unoderd_map即可，类似Redis的dict。

具体对应如下：

```C++
string <-> std::string;
list <-> std::list;
set <-> std::unordered_set;
hash <-> std::unordered_map;
zet 是自己实现的一个类，主要有两个成员变量map和std::unoderd_map
```

其中项目本质上是使用了一个大的hash表来存储数据，主要结构是std::unordered_map<std::string, YObject>，其中Yobjct类似Redis的redisObject大致结构如下

```C++
struct YObject { // 存储数据的大哈希表的值类型（键类型是字符串）
    YType type; // list或string或hash或set或sortedset

    // 由于union不支持存放c++对象，这里干脆放弃union，采用了void*指针
    void* value; // 指向具体的值对象

    QObject() {
        type = QType_invalid;
        value = nullptr;
    }
}
```

项目已实现的命令如下

```
string：set，get，setnx
list：LPUSH，RPUSH，LPUSHX，RPUSHX，LPOP，RPOP，LLEN，LRANGE
set：SADD，SCARD，SMEMBER，SPOP
hash：HGET，HSET
zset：ZADD
```

协议解析流程如下在YedisParser.cpp中：

```C++
协议解析，以set name ypd 为例 我们接受到的其实是下面的文本协议
*$3\r\nset\r\n$4\r\nname\r\n$4\r\nypd\r\n
协议解析流程为：首先读*符号，发现有三个参数；然后读取第一个$符号，知道第一个参数有三个字符，继 
续往后读得到了set；
接着读取第二个$符号，知道该参数有四个字符，得到了name;
最后读取第三个$符号，该参数也有四个字符，得到了ypd；
所以，协议解析器还原得到了命令set name ypd
```

项目中还实现了简单定时器，基于C++的priority_queue 实现小根堆来存储定时事件，基本思路如下：

```C++
先实现一个类Timer表示每一个被添加的定时事件，构造时需要一个millisecond为单位的超时时间，一个回调函数，一个回调函数的参数（暂未使用）。TimerManager是用户操作的接口，提供增加，删除定时器的功能。STL中提供能优先队列，直接可以拿来用。
```

TODO：

```
实现类似Redis的主从机制（进行中），实现类似Redis的哨兵机制（初步考虑使用Zookeeper作为哨兵）
```
