// 简单的HTTP客户端程序
// 程序请求连接到本地8888端口，发送一条消息，然后等待30秒后关闭连接
// 地址采用的本地回环地址

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

    // 创建套接字
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET)
    {
        printf("创建套接字失败\n");
        WSACleanup();
        return 1;
    }

    // 设置服务器地址
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8888);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // 本地回环地址

    // 打印连接信息
    printf("尝试连接到 127.0.0.1:8888\n");

    // 获取本地地址信息
    sockaddr_in localAddr;
    int localAddrLen = sizeof(localAddr);
    if (getsockname(sock, (sockaddr *)&localAddr, &localAddrLen) == 0)
    {
        printf("本地地址: %s:%d\n",
               inet_ntoa(localAddr.sin_addr),
               ntohs(localAddr.sin_port));
    }

    // 连接服务器
    printf("正在连接服务器...\n");
    if (connect(sock, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        printf("连接失败，错误码: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        return 1;
    }
    printf("连接成功\n");

    // 发送测试数据
    const char *testData = "Hello from client!";
    printf("发送数据: %s\n", testData);
    if (send(sock, testData, strlen(testData), 0) == SOCKET_ERROR)
    {
        printf("发送数据失败\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // 接收服务器响应
    char recvBuffer[1024];
    int bytes = recv(sock, recvBuffer, sizeof(recvBuffer), 0);
    if (bytes > 0)
    {
        recvBuffer[bytes] = '\0';
        printf("收到服务器响应: %s\n", recvBuffer);
    }

    // 等待一段时间
    printf("连接将保持30秒...\n");
    std::this_thread::sleep_for(std::chrono::seconds(30));

    // 关闭连接
    printf("\n开始关闭连接...\n");

    // 发送FIN包
    printf("发送FIN包...\n");
    shutdown(sock, SD_SEND);

    // 等待服务器的响应
    char buffer[1024];
    while (recv(sock, buffer, sizeof(buffer), 0) > 0)
    {
        // 继续接收直到收到FIN
    }

    // 完全关闭连接
    closesocket(sock);
    WSACleanup();

    printf("连接已关闭\n");
    printf("按回车键退出...\n");
    getchar();

    return 0;
}