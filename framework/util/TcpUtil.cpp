//
// Created by liaohailong on 2024/4/20.
//

#include "TcpUtil.h"


int TCPUtil::getFreePort(int from, int to) {
    int sock_fd;
    struct sockaddr_in address{};

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (sock_fd < 0) {
//        AndroidLog::error("tcp_util", "cannot open socket");
        return -1;
    }

    address.sin_family = AF_INET; // IPV4协议
    address.sin_addr.s_addr = htonl(INADDR_ANY); // 监听本地多有IP地址，INADDR_ANY表示所有网卡

    int port = from;
    int result = -1;
    do {
        address.sin_port = htons(port++);
        result = bind(sock_fd, (const struct sockaddr *) &address, sizeof(address));
    } while (result < 0 && port <= to && errno == EADDRINUSE);

    if (result < 0) {
//        AndroidLog::error("TCPUtil", "cannot find a free port [%d]%s",
//                          errno, strerror(errno));
        close(sock_fd); // 关闭套接字，防止系统句柄不够用
        return -1;
    }

    close(sock_fd); // 关闭套接字，防止系统句柄不够用
    return port - 1;
}

bool TCPUtil::setNonBlock(int fd) {
    int nb = 1; // 0:清除 1:设置
    if (ioctl(fd, FIONBIO, &nb) == -1) {
        return false;
    }

    return true;
}


bool TCPUtil::getSockAddrInfo(const sockaddr_storage &clientaddr,
                              char *clientIp, size_t clientIpLength,
                              char *clientPort, size_t clientPortLength) {

    socklen_t clientlen = sizeof(struct sockaddr_storage);
    int errcode = getnameinfo((struct sockaddr *) &clientaddr, clientlen,
                              clientIp, clientIpLength,
                              clientPort, clientPortLength, 0);
    if (errcode != 0) {
//        const char *error_str = gai_strerror(errcode);
//        AndroidLog::error("TCPUtil", "gai_strerror[%d]: %s", errcode, error_str);
        return false;
    }

    // 如果是 IPv6 映射的 IPv4 地址，转换为 IPv4 地址格式
    if (clientaddr.ss_family == AF_INET6) {
        auto *addr_in6 = (struct sockaddr_in6 *) &clientaddr;
        if (IN6_IS_ADDR_V4MAPPED(&addr_in6->sin6_addr)) {
            struct in_addr in4_addr{};
            memcpy(&in4_addr, &addr_in6->sin6_addr.s6_addr[12], sizeof(in4_addr));
            inet_ntop(AF_INET, &in4_addr, clientIp, clientIpLength);  // 将 IPv4 映射地址转换为 IPv4 地址
        }
    }
    return true;
}