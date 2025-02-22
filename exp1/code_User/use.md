# TCP客户端程序说明

## 程序功能
这是一个基于WinSocket的TCP客户端程序，主要用于：
1. 连接到指定服务器（本地回环地址:8888）
2. 发送测试数据
3. 接收服务器响应
4. 演示TCP连接的完整生命周期

## 程序流程

### 1. 初始化阶段
- 设置控制台编码为UTF-8
- 初始化WinSocket库
- 创建TCP套接字
- 设置服务器地址信息

### 2. 连接建立
- 显示连接信息
- 获取本地地址信息
- 连接到服务器
- 等待用户确认

### 3. 数据交换
- 发送测试数据到服务器
- 接收服务器响应
- 显示接收到的数据
- 保持连接30秒用于观察

### 4. 连接关闭
- 发送FIN包开始关闭过程
- 等待服务器响应
- 完全关闭连接
- 清理资源

## 关键代码说明

### 1. 套接字创建
```cpp
SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
```

### 2. 连接服务器
```cpp
sockaddr_in serverAddr;
serverAddr.sin_family = AF_INET;
serverAddr.sin_port = htons(8888);
serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr));
```

### 3. 数据交换
```cpp
const char* testData = "Hello from client!";
send(sock, testData, strlen(testData), 0);
char recvBuffer[1024];
recv(sock, recvBuffer, sizeof(recvBuffer), 0);
```

## 使用方法
1. 先运行服务端程序
2. 运行客户端程序
3. 按回车开始连接
4. 观察数据交换过程
5. 等待连接自动关闭

## 注意事项
1. 需要管理员权限运行
2. 服务端必须先启动
3. 默认连接到本地服务器
4. 配合服务端程序使用
5. 可以使用Wireshark观察TCP连接过程

## 程序特点
1. 用户交互设计
2. 显示详细的连接信息
3. 实现完整的TCP连接生命周期
4. 便于观察TCP握手和挥手过程
5. 支持中文显示

## 应用场景
1. TCP协议学习和教学
2. 网络编程测试
3. TCP连接过程观察
4. 网络抓包实验
