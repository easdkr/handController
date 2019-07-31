#include "hand_client.h"
void ErrorHandling(char *message) {
    fprintf(stderr, message);
    fprintf(stderr, "\n");
    exit(-1);
}
void connect_server() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) ErrorHandling("WSAStartup() error!");
    _socket = socket(PF_INET, SOCK_STREAM, 0);
    if (_socket == INVALID_SOCKET) ErrorHandling("socket() error!!");
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(addr);
    server_addr.sin_port = htons(port);
    while (TRUE) {
        if (connect(_socket, (SOCKADDR*)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
            fprintf(stderr,"connect error!");
        else
            break;
    }
}



void close_socket() {
    closesocket(_socket);
    WSACleanup();
}