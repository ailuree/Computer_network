#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <stdio.h>
#include <string>
#include <fcntl.h>
#include <io.h>

#pragma comment(lib, "ws2_32.lib")

// IP 头部结构
typedef struct ip_hdr
{
    unsigned char ip_header_len : 4; // 首部长度
    unsigned char ip_version : 4;    // 版本
    unsigned char ip_tos;            // 服务类型
    unsigned short ip_total_length;  // 总长度
    unsigned short ip_id;            // 标识
    unsigned short ip_frag_offset;   // 标志(3 bits) + 片偏移(13 bits)
    unsigned char ip_ttl;            // 生存时间
    unsigned char ip_protocol;       // 协议类型
    unsigned short ip_checksum;      // 首部校验和
    unsigned int ip_srcaddr;         // 源IP地址
    unsigned int ip_destaddr;        // 目的IP地址
} IP_HDR, *PIP_HDR;

// TCP 头部结构
typedef struct tcp_hdr
{
    unsigned short source_port;       // 源端口
    unsigned short dest_port;         // 目的端口
    unsigned int sequence;            // 序号
    unsigned int acknowledge;         // 确认号
    unsigned char ns : 1;             // ECN-nonce标志
    unsigned char reserved_part1 : 3; // 保留
    unsigned char data_offset : 4;    // 数据偏移
    unsigned char fin : 1;            // FIN标志
    unsigned char syn : 1;            // SYN标志
    unsigned char rst : 1;            // RST标志
    unsigned char psh : 1;            // PSH标志
    unsigned char ack : 1;            // ACK标志
    unsigned char urg : 1;            // URG标志
    unsigned char ecn : 1;            // ECN标志
    unsigned char cwr : 1;            // CWR标志
    unsigned short window;            // 窗口大小
    unsigned short checksum;          // 校验和
    unsigned short urgent_pointer;    // 紧急指针
} TCP_HDR, *PTCP_HDR;

// UDP 头部结构
typedef struct udp_hdr
{
    unsigned short source_port; // 源端口
    unsigned short dest_port;   // 目的端口
    unsigned short length;      // UDP长度
    unsigned short checksum;    // 校验和
} UDP_HDR, *PUDP_HDR;

// ICMP 头部结构
typedef struct icmp_hdr
{
    unsigned char type;      // 类型
    unsigned char code;      // 代码
    unsigned short checksum; // 校验和
    unsigned short id;       // 标识符
    unsigned short sequence; // 序号
} ICMP_HDR, *PICMP_HDR;

// 添加全局变量
int filter_port = 0;
bool enable_port_filter = false;
bool enable_loopback_filter = false;

// 处理IP数据包
void ProcessIPPacket(char *buffer, int size)
{
    IP_HDR *iphdr = (IP_HDR *)buffer;

    // 获取源IP和目的IP
    struct in_addr source, dest;
    source.s_addr = iphdr->ip_srcaddr;
    dest.s_addr = iphdr->ip_destaddr;

    // 如果启用了本地回环过滤，只处理TCP包
    if (enable_loopback_filter)
    {
        // 只处理TCP包且是本地回环地址
        if (iphdr->ip_protocol == IPPROTO_TCP &&
            (strncmp(inet_ntoa(source), "127.", 4) == 0 ||
             strncmp(inet_ntoa(dest), "127.", 4) == 0))
        {

            TCP_HDR *tcphdr = (TCP_HDR *)(buffer + iphdr->ip_header_len * 4);
            unsigned short src_port = ntohs(tcphdr->source_port);
            unsigned short dst_port = ntohs(tcphdr->dest_port);

            // 检查端口过滤
            if (!enable_port_filter ||
                (enable_port_filter && (src_port == filter_port || dst_port == filter_port)))
            {

                // 打印TCP包信息
                printf("\n=== TCP 数据包 ===\n");
                printf("源地址: %s:%d\n", inet_ntoa(source), src_port);
                printf("目的地址: %s:%d\n", inet_ntoa(dest), dst_port);

                // 打印TCP标志
                printf("TCP 标志: ");
                if (tcphdr->syn)
                    printf("SYN ");
                if (tcphdr->ack)
                    printf("ACK ");
                if (tcphdr->fin)
                    printf("FIN ");
                if (tcphdr->rst)
                    printf("RST ");
                if (tcphdr->psh)
                    printf("PSH ");
                if (tcphdr->urg)
                    printf("URG ");
                printf("\n");

                // 打印序列号和确认号
                printf("序列号: %u\n", ntohl(tcphdr->sequence));
                printf("确认号: %u\n", ntohl(tcphdr->acknowledge));
            }
        }
        return; // 如果启用本地回环过滤，不处理其他包
    }

    // 如果没有启用本地回环过滤，按原来的方式处理所有包
    switch (iphdr->ip_protocol)
    {
    case IPPROTO_TCP:
    {
        TCP_HDR *tcphdr = (TCP_HDR *)(buffer + iphdr->ip_header_len * 4);
        unsigned short src_port = ntohs(tcphdr->source_port);
        unsigned short dst_port = ntohs(tcphdr->dest_port);

        // 根据过滤条件显示数据包
        if (!enable_port_filter ||
            (enable_port_filter && (src_port == filter_port || dst_port == filter_port)))
        {

            // 打印TCP包信息
            printf("\n=== TCP Packet ===\n");
            printf("源地址: %s:%d\n", inet_ntoa(source), src_port);
            printf("目的地址: %s:%d\n", inet_ntoa(dest), dst_port);

            // 打印TCP标志
            printf("TCP 标志: ");
            if (tcphdr->syn)
                printf("SYN ");
            if (tcphdr->ack)
                printf("ACK ");
            if (tcphdr->fin)
                printf("FIN ");
            if (tcphdr->rst)
                printf("RST ");
            if (tcphdr->psh)
                printf("PSH ");
            if (tcphdr->urg)
                printf("URG ");
            printf("\n");

            // 打印序列号和确认号
            printf("序列号: %u\n", ntohl(tcphdr->sequence));
            printf("确认号: %u\n", ntohl(tcphdr->acknowledge));
        }
        break;
    }
    case IPPROTO_UDP:
    {
        printf("UDP\n");
        UDP_HDR *udphdr = (UDP_HDR *)(buffer + iphdr->ip_header_len * 4);
        printf("源端口: %d\n", ntohs(udphdr->source_port));
        printf("目的端口: %d\n", ntohs(udphdr->dest_port));
        printf("长度: %d\n", ntohs(udphdr->length));
        break;
    }
    case IPPROTO_ICMP:
    {
        printf("ICMP\n");
        ICMP_HDR *icmphdr = (ICMP_HDR *)(buffer + iphdr->ip_header_len * 4);
        printf("类型: %d\n", icmphdr->type);
        printf("代码: %d\n", icmphdr->code);
        if (icmphdr->type == 8)
        {
            printf("ICMP 请求\n");
        }
        else if (icmphdr->type == 0)
        {
            printf("ICMP 回显回复\n");
        }
        break;
    }
    default:
        printf("Other Protocol: %d\n", iphdr->ip_protocol);
        break;
    }
    printf("------------------------\n");
}

int main()
{
    // 设置控制台代码页
    system("chcp 65001");

    // 初始化 WinSock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        printf("WSAStartup 失败，错误码: %d\n", WSAGetLastError());
        system("pause");
        return 1;
    }

    // 获取本地主机名和地址信息
    char hostname[100];
    if (gethostname(hostname, sizeof(hostname)) != 0)
    {
        printf("获取主机名失败，错误码: %d\n", WSAGetLastError());
        WSACleanup();
        system("pause");
        return 1;
    }

    struct hostent *local = gethostbyname(hostname);
    if (local == nullptr)
    {
        printf("获取主机信息失败，错误码: %d\n", WSAGetLastError());
        WSACleanup();
        system("pause");
        return 1;
    }

    // 手动添加回环地址
    printf("\n可用的网络接口:\n");
    printf("0: 127.0.0.1 (本地回环)\n");
    int interface_count = 1;

    // 打印其他网络接口
    for (int i = 0; local->h_addr_list[i] != 0; ++i)
    {
        struct in_addr addr;
        memcpy(&addr, local->h_addr_list[i], sizeof(struct in_addr));
        printf("%d: %s\n", interface_count + i, inet_ntoa(addr));
    }
    interface_count += local->h_addr_list[0] != 0;

    // 让用户选择网络接口
    int selected_interface;
    printf("\n选择网络接口编号 (0-%d): ", interface_count - 1);
    printf("\n提示：如果要抓取本地回环通信，请选择0号接口(127.0.0.1)\n");

    if (scanf("%d", &selected_interface) != 1)
    {
        printf("输入无效\n");
        WSACleanup();
        system("pause");
        return 1;
    }

    if (selected_interface < 0 || selected_interface >= interface_count)
    {
        printf("无效的接口编号\n");
        WSACleanup();
        system("pause");
        return 1;
    }

    // 创建原始套接字
    SOCKET sock = socket(AF_INET, SOCK_RAW, IPPROTO_IP);
    if (sock == INVALID_SOCKET)
    {
        int error = WSAGetLastError();
        printf("创建套接字失败，错误码: %d\n", error);
        switch (error)
        {
        case WSAEACCES:
            printf("权限不足，请以管理员身份运行\n");
            break;
        case WSAEINVAL:
            printf("无效参数\n");
            break;
        case WSAEAFNOSUPPORT:
            printf("地址族不支持\n");
            break;
        default:
            printf("未知错误\n");
        }
        WSACleanup();
        system("pause");
        return 1;
    }

    // 绑定到选择的网络接口
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    if (selected_interface == 0)
    {
        addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    }
    else
    {
        addr.sin_addr.s_addr = ((struct in_addr *)local->h_addr_list[selected_interface - 1])->s_addr;
    }
    addr.sin_port = 0;

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == SOCKET_ERROR)
    {
        printf("绑定失败，错误码: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        system("pause");
        return 1;
    }

    printf("\n已选择接口: %s\n", inet_ntoa(addr.sin_addr));

    // 设置混杂模式
    DWORD flag = 1;
    if (ioctlsocket(sock, SIO_RCVALL, &flag) != 0)
    {
        printf("设置混杂模式失败，错误码: %d\n", WSAGetLastError());
        closesocket(sock);
        WSACleanup();
        system("pause");
        return 1;
    }

    // 添加过滤选项
    printf("\n请选择过滤选项:\n");
    printf("1. 是否启用TCP端口过滤？(1:是 0:否): ");
    int choice;
    if (scanf("%d", &choice) != 1)
    {
        printf("输入无效\n");
        closesocket(sock);
        WSACleanup();
        system("pause");
        return 1;
    }

    if (choice == 1)
    {
        printf("请输入要过滤的TCP端口号(例如:8888): ");
        if (scanf("%d", &filter_port) != 1)
        {
            printf("输入无效\n");
            closesocket(sock);
            WSACleanup();
            system("pause");
            return 1;
        }
        enable_port_filter = true;
        printf("已启用端口过滤，将只显示TCP端口 %d 的数据包\n", filter_port);
    }

    printf("2. 是否只显示本地回环地址(127.0.0.1)的数据包？(1:是 0:否): ");
    if (scanf("%d", &choice) != 1)
    {
        printf("输入无效\n");
        closesocket(sock);
        WSACleanup();
        system("pause");
        return 1;
    }

    if (choice == 1)
    {
        enable_loopback_filter = true;
        printf("已启用本地回环过滤\n");
    }

    printf("\n开始抓取数据包...\n");
    if (enable_port_filter)
    {
        printf("- 过滤端口: %d\n", filter_port);
    }
    if (enable_loopback_filter)
    {
        printf("- 只显示本地回环数据包\n");
    }
    printf("按Ctrl+C停止捕获\n\n");

    // 循环接收数据包
    char buffer[65536];
    while (true)
    {
        int bytes = recv(sock, buffer, sizeof(buffer), 0);
        if (bytes > 0)
        {
            ProcessIPPacket(buffer, bytes);
        }
        else if (bytes == SOCKET_ERROR)
        {
            printf("接收数据失败，错误码: %d\n", WSAGetLastError());
            break;
        }
    }

    closesocket(sock);
    WSACleanup();
    system("pause");
    return 0;
}
