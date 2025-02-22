# TCP服务端程序说明

## 程序功能
这是一个基于WinSocket的TCP服务端程序，主要用于：
1. 监听指定端口（8888）
2. 接受客户端连接
3. 与客户端进行简单的数据交换
4. 演示TCP连接的完整生命周期

## 程序流程

### 1. 初始化阶段
- 设置控制台编码为UTF-8
- 初始化WinSocket库
- 创建监听套接字
- 设置服务器地址和端口

### 2. 网络接口设置
- 获取本地主机名
- 列出所有可用的网络接口
- 绑定到所有网络接口（INADDR_ANY）
- 开始监听连接请求

### 3. 连接处理
- 等待客户端连接
- 接受连接请求
- 显示客户端信息（IP地址和端口）

### 4. 数据交换
- 接收客户端发送的数据
- 显示接收到的数据
- 发送响应数据给客户端
- 保持连接30秒用于观察

### 5. 连接关闭
- 发送FIN包开始关闭过程
- 等待接收客户端的最后数据
- 完全关闭连接
- 清理资源

## 关键代码说明

### 1. 套接字创建和绑定
```cpp
SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
sockaddr_in serverAddr;
serverAddr.sin_family = AF_INET;
serverAddr.sin_port = htons(8888);
serverAddr.sin_addr.s_addr = INADDR_ANY;
bind(listenSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
```

### 2. 监听连接
```cpp
listen(listenSocket, 1);
```

### 3. 接受连接
```cpp
sockaddr_in clientAddr;
int clientAddrLen = sizeof(clientAddr);
SOCKET clientSocket = accept(listenSocket, (sockaddr*)&clientAddr, &clientAddrLen);
```

### 4. 数据交换
```cpp
char buffer[1024];
int bytes = recv(clientSocket, buffer, sizeof(buffer), 0);
const char* response = "Hello from server!";
send(clientSocket, response, strlen(response), 0);
```

## 使用方法
1. 以管理员权限运行程序
2. 等待客户端连接
3. 观察数据交换过程
4. 等待连接自动关闭

## 注意事项
1. 需要管理员权限运行
2. 端口8888必须未被占用
3. 防火墙可能需要放行
4. 配合客户端程序使用
5. 可以使用Wireshark观察TCP连接过程

## 程序特点
1. 支持多网卡环境
2. 显示详细的连接信息
3. 实现完整的TCP连接生命周期
4. 便于观察TCP握手和挥手过程
5. 支持中文显示

## 应用场景
1. TCP协议学习和教学
2. 网络编程测试
3. TCP连接过程观察
4. 网络抓包实验
