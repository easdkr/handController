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
#include "rapidjson\document.h"
#include "rapidjson\writer.h"
#include "rapidjson\stringbuffer.h"
#include <vector>
using namespace rapidjson;

#define BUFFERSIZE 255
#define THREAD std::thread
#define EXTERN __declspec(selectany)

struct Coords {
    int X = 0;
    int Y = 0;
};

struct _Range {
    int Width = 0;
    int Height = 0;
};

struct Thing {
    SOCKET socket_id;
    std::string thing_id;
    _Range thing_range;
    Coords thing_coords;
    std::string status;
    std::string type;
};

__declspec(selectany) SOCKADDR_IN server_addr, client_addr; //socket 주소, 포트 등을 담는 구조체
__declspec(selectany) SOCKET server_socket, client_socket;
__declspec(selectany) SOCKET hand_tracking_socket; // 핸드 트레킹쪽 소켓 아이디 여기 저장할거임 
//__declspec(selectany) SOCKET device_socket;//device socket
__declspec(selectany) char _coord[BUFFERSIZE];
__declspec(selectany) char coord[BUFFERSIZE];
__declspec(selectany) char message[BUFFERSIZE];
//__declspec(selectany) char device_message[BUFFERSIZE];
__declspec(selectany) int port = 80;
__declspec(selectany) char* c_coord;

//디바이스랑 통신하는데 필요한 변수 선언 
__declspec(selectany) int clntCnt = 0;
__declspec(selectany) Thing things[BUFFERSIZE];
__declspec(selectany) char dev_msg[BUFFERSIZE];
__declspec(selectany) bool is_selected = false;
EXTERN std::string selected_thing_id;
EXTERN int x = 0;
EXTERN int y = 0;
EXTERN int gesture = 100;
EXTERN bool coord_in_device = false;
void ErrorHandling(char * message);

std::vector<std::string> split(std::string str, std::string sep);

void parse_coord(char * coord);

void create_server();

void close_socket();

bool selectFunc(std::string thingID);

bool statusChangeFuc(std::string thingID, std::string status);

__declspec(selectany) char HAND_STRING[BUFFERSIZE] = "HAND";


enum TYPE {
    HAND, DEVICE
};

enum GESTURE_ENUM {
    NORMAL = 100,
    SELECT = 200,
    ON = 300,
    OFF = 400
};
