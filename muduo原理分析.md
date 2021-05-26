# muduo原理分析

[muduo](https://link.zhihu.com/?target=https%3A//github.com/chenshuo/muduo)是[陈硕](https://link.zhihu.com/?target=http%3A//chenshuo.com/)大神个人开发的C++的TCP网络编程库。muduo基于Reactor模式实现。Reactor模式也是目前大多数Linux端高性能网络编程框架和网络应用所选择的主要架构，例如内存数据库Redis和Java的Netty库等。



一.muduo是基于Reactor模式的，其中有一个主loop，这个loop主要是用来接收新连接的，隶属于Accepptor这个类，在Acceptor这个类初始化时做了如下两个操作创建一个非阻塞socket，并且将IP地址和端口绑定到这个socket上面(这个socket就是**侦听socket**)，而Acceptor有一个成员变量Channel拥有这个loop并且设置了socket上有事件时指定的回调函数handleRead，handleRead做了接收新连接产生一个**connectsocket**，同时调用在创建Acceptor类是就设置好的产生新连接的回调函数TcServer::newConnection,这个函数会创建一个TcpConnection对象，这个对象在建立的时候也会设置自己的一系列回调函数，并在设置完成回调函数之后立即调用connectEstablished这个函数生成一个session对象，这个时候就可以使用这个session对象来做业务逻辑处理，如协议解析等,大致流程如下：

![](.\AcceptNew说明.png)