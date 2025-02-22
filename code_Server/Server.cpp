#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#pragma comment(lib, "ws2_32.lib")

int main()
{
    // 设置控制台代码页
    system("chcp 65001");

    // 初始化 WinSock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStartup 失败\n");
        return 1;
    }

    // 创建监听套接字
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket == INVALID_SOCKET)
    {
        printf("创建套接字失败\n");
        WSACleanup();
        return 1;
    }

    // 设置服务器地址
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8888);
    serverAddr.sin_addr.s_addr = INADDR_ANY; // 修改回INADDR_ANY以监听所有接口

    printf("正在监听所有网络接口的8888端口...\n");

    // 绑定地址
    if (bind(listenSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        printf("绑定失败，错误码: %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // 打印所有可用的网络接口
    char hostname[256];
    gethostname(hostname, sizeof(hostname));
    struct hostent *host = gethostbyname(hostname);
    if (host)
    {
        printf("\n可用的网络接口:\n");
        for (int i = 0; host->h_addr_list[i] != 0; ++i)
        {
            struct in_addr addr;
            memcpy(&addr, host->h_addr_list[i], sizeof(struct in_addr));
            printf("%d: %s\n", i, inet_ntoa(addr));
        }
    }

    printf("绑定成功\n");
    printf("开始监听...\n");

    // 开始监听
    if (listen(listenSocket, 1) == SOCKET_ERROR)
    {
        printf("监听失败\n");
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    printf("服务器启动，等待连接...\n");
    printf("本地端口: 8888\n");

    // 接受连接
    sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    SOCKET clientSocket = accept(listenSocket, (sockaddr *)&clientAddr, &clientAddrLen);
    if (clientSocket == INVALID_SOCKET)
    {
        printf("接受连接失败\n");
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    // 打印客户端信息
    char clientIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, clientIP, INET_ADDRSTRLEN);
    printf("客户端连接成功: %s:%d\n", clientIP, ntohs(clientAddr.sin_port));

    // 接收客户端数据
    char buffer[1024];
    int bytes = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytes > 0)
    {
        buffer[bytes] = '\0';
        printf("收到客户端数据: %s\n", buffer);

        // 发送响应
        const char *response = "Hello from server!";
        printf("发送响应: %s\n", response);
        send(clientSocket, response, strlen(response), 0);
    }

    // 等待一段时间
    printf("连接将保持30秒...\n");
    std::this_thread::sleep_for(std::chrono::seconds(30));

    // 关闭连接
    printf("\n开始关闭连接...\n");

    // 先关闭发送方向
    printf("发送FIN包...\n");
    shutdown(clientSocket, SD_SEND);

    // 接收客户端可能发送的最后数据
    while (recv(clientSocket, buffer, sizeof(buffer), 0) > 0)
    {
        // 继续接收直到收到FIN
    }

    // 完全关闭连接
    closesocket(clientSocket);
    closesocket(listenSocket);
    WSACleanup();

    printf("服务器关闭\n");
    printf("按回车键退出...\n");
    getchar();
    return 0;
}
