#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    struct hostent* host = gethostbyname("example.com");
    
    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = *(unsigned long*)host->h_addr;
    
    connect(sock, (sockaddr*)&addr, sizeof(addr));
    
    const char* request = "GET / HTTP/1.0\r\nHost: example.com\r\n\r\n";
    send(sock, request, strlen(request), 0);
    
    closesocket(sock);
    WSACleanup();
    return 0;
} 