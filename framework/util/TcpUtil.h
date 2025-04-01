//
// Created by liaohailong on 2024/4/20.
//

#ifndef TCPSERVER_TCPUTIL_H
#define TCPSERVER_TCPUTIL_H

#include <cerrno>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <cstring>
#include <cstring>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>


namespace TCPUtil {

    /**
     * @return 获取一个空闲的端口号
     */
    int getFreePort(int from, int to);

    /**
     * @param fd 将文件句柄设置为非阻塞式IO
     * @return true表示设置成功
     */
    bool setNonBlock(int fd);

    /**
     * 获取客户端ip和port
     * @param addr
     * @param clientIp 用于存储主机名
     * @param clientPort 用于存储服务名（端口号）
     * @return true表示获取成功
     */
    bool getSockAddrInfo(const sockaddr_storage &clientaddr,
                         char *clientIp, size_t clientIpLength,
                         char *clientPort, size_t clientPortLength);

}


#endif //TCPSERVER_TCPUTIL_H
