#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")


int main() {
    // 1. 初始化 start
    WSADATA wsaData;
    int iResult;
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }
    else {
        printf("WSAStartup is OK\r\n");
    }
    // 1. 初始化 end

    // 2. 创建套接字 start
#define DEFAULT_PORT "9501" // 服务器监听的端口
    struct addrinfo*
        result = NULL,
        * ptr = NULL,
        hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET; //AF _INET 用于指定 IPv4 地址族
    hints.ai_socktype = SOCK_STREAM;// SOCK _STREAM 用于指定流套接字
    hints.ai_protocol = IPPROTO_TCP;// IPPROTO _TCP 用于指定 tcp 协议
    hints.ai_flags = AI_PASSIVE;

    // 从本机中获取ip地址等信息为了sockcet 使用
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }
    else{
        printf("getaddrinfo is OK\r\n");
    }

    // 创建socket对象
    SOCKET ListenSocket = INVALID_SOCKET;
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }
    else {
        printf("Socket is OK\r\n");
    }
    // 2. 创建套接字 end

    // 3. 绑定套接字 start
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);// 调用 bind 函数后，不再需要地址信息 释放
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    else {
        printf("bind is OK\r\n");
    }
    // 3. 绑定套接字 end

    // 4.在套接字上监听客户端 start
    if (listen(ListenSocket, SOMAXCONN) == SOCKET_ERROR) {
        // SOMAXCONN定义了此套接字允许最大连接
        printf("Listen failed with error: %ld\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    else {
        printf("Listening...\r\n");
    }
    // 4.在套接字上监听客户端 end

    // 5.接受来自客户端的连接 start
    SOCKET ClientSocket; //临时 套接字 对象，以接受来自客户端的连接
    // 这里没有考虑多线程并发接受多个客户端 只接受一个
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }
    // 5.接受来自客户端的连接 end

    // 6.在服务器上接收和发送数据 start
#define DEFAULT_BUFLEN 512 //  字符缓冲区长度
    char recvbuf[DEFAULT_BUFLEN]; // 字符缓冲区数组
    int  iSendResult;
    int recvbuflen = DEFAULT_BUFLEN;

    do {
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);

            // recvbuf参数表示: 这里为了简单将客户端发送过来的消息再发送给客户端 
            iSendResult = send(ClientSocket, recvbuf, iResult, 0);
            if (iSendResult == SOCKET_ERROR) {
                printf("send failed: %d\n", WSAGetLastError());
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            }
            printf("Bytes sent: %d\n", iSendResult);
        }
        else if (iResult == 0)
            printf("Connection closing...\n");
        else {
            printf("recv failed: %d\n", WSAGetLastError());
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        }
    } while (iResult > 0);
    // 6.在服务器上接收和发送数据 end

    // 7.断开连接 start
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }
    else {
        printf("shutdown\r\n");
    }
    // 7.断开连接 end
    system("pause");
    return 0;
}