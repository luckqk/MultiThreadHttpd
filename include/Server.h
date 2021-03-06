#ifndef SERVER_H
#define SERVER_H
#include <memory>
#include <vector>
#include <sys/epoll.h>
#include "Socket.h"
#include "ConfigLoad.h"
#include "TWork.h"
namespace MThttpd
{
/**
 * RAII封装创建启动和结束线程池
 * （1）服务器总开关；创建、开启、关闭线程池（工作线程）
 * （2）主线程epoll 于listenfd和signalfd，accept后选择并分配给工作线程或收到INT平稳结束程序
 * （3）拥有handler成员函数处理请求(TODO:handler放在哪个类中更合理)
 */
class Server
{
public:
  Server();
  ~Server();
  void start();
  size_t ChooseTW(); //选择工作线程
  static int Handler(std::shared_ptr<Socket> pSock);

  Server(const Server &) = delete;
  const Server &operator=(const Server &) = delete;

private:
  int m_sigFd; //signalfd，以file destriptor方式操作信号
  bool m_bIsRun;

  Socket m_listen; //监听socket
  std::shared_ptr<ConfigLoad> m_conf;
  std::vector<std::shared_ptr<TWork>> m_tPool; //线程池,TWork必须是heap object，其头文件中有解释
  size_t m_nWorkIndex;                            //用于ChooseTW()中选择工作线程

  static const int sm_nMaxEvents = 2; //目前只需要监听listen socket和signalfd两个FD
  std::unique_ptr<struct epoll_event[]> m_upEvents;
  int m_epFd; //epoll_create返回的FD
};
}

#endif //SERVER_H