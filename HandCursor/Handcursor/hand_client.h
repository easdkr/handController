#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <string>

#define BUFFERSIZE 255

__declspec(selectany) SOCKADDR_IN server_addr, client_addr; //socket 주소, 포트 등을 담는 구조체
__declspec(selectany) SOCKET _socket;
__declspec(selectany) const char* addr = new char[30]{ "127.0.0.1" };
__declspec(selectany) int port = 80;
__declspec(selectany) char* c_coord;
void ErrorHandling(char * message);
void connect_server();

void close_socket();