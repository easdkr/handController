#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WINDOW_NAME	"4Tower HTRC 2.0.1"

#include "server.h"
#include <vector>
#include <Windows.h>
#include <WindowsX.h>
#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <opencv\cv.h>
#include <opencv\highgui.h>
#include <thread>

#include <iomanip>
#include <cstdlib>
#include <time.h>
#include <ctime>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#define CVUI_DISABLE_COMPILATION_NOTICES
#define CVUI_IMPLEMENTATION
#include "cvui.h"

//using namespace std;
using namespace rapidjson;
using namespace cv;

#include "yolo_v2_class.hpp"
#include "demo.h"

#include <opencv2\opencv.hpp>
#include "�ҽ�.h"
#pragma comment(lib,"opencv_core249.lib")
#pragma comment(lib,"opencv_imgproc249.lib")
#pragma comment(lib,"opencv_highgui249.lib")

#define BUF_SIZE 4096
#define ADDRESS "127.0.0.1"
std::vector<bbox_t> result_vec;
int objectId;
char cObjectId[BUFFERSIZE] = { '\0' };

void listen_client() {
    int client_addr_size = sizeof(client_addr);
    int type;
    char to_hand[BUFFERSIZE] = { "HAND TRACKING CLIENT CONNECTED!\n" };
    while (TRUE) {
        //Client connect
        client_socket = accept(server_socket, (SOCKADDR*)&client_addr, &client_addr_size);
        if (client_socket == INVALID_SOCKET) ErrorHandling("accept() error!");

        //Client Ȯ��
        if (0 < (recv(client_socket, message, sizeof(message), 0))) { // recv 1
            Document rtq;
            printf("Message from client : %s\n", message);
            std::string str(message);
            if (str == "0") type = HAND;
            else type = DEVICE;
            printf("Type : %d\n", type);
            switch (type) {
            case HAND:
                send(client_socket, to_hand, strlen(to_hand), 0);
                hand_tracking_socket = client_socket;
                break;
            case DEVICE:
                rtq.Parse(message);

                if (((std::string)rtq["Format"].GetString() == "RTQ") && rtq.IsObject()) {
                    std::cout << rtq["Data"]["thingID"].GetString() << std::endl;
                }
                things[clntCnt].socket_id = client_socket;
                things[clntCnt].thing_id = rtq["Data"]["thingID"].GetString();
                things[clntCnt].status = rtq["Data"]["status"].GetString();
                things[clntCnt++].type = rtq["Data"]["type"].GetString();
                send(client_socket, "received", strlen("received"), 0);//�۽�
                memset(message, 0, BUFFERSIZE);
                break;
            default:
                break;
            }//end switch
        }//end if 
    }//end while
}//end function

bool selectFunc(std::string thingID) {
    std::string SRQ = R"(
	{
		"Format": "SRQ",
		"Data":{
			"thingID" : ")" + thingID + R"("
		}
	})";
    char SDS[BUFFERSIZE] = "";
    int i;
    SOCKET clntsock;

    for (i = 0; i < clntCnt; i++) {//ã��
        if (things[i].thing_id == thingID) break;
    }

    //send event �߻�
    send(things[i].socket_id, SRQ.c_str(), strlen(SRQ.c_str()), 0);
    if (recv(things[i].socket_id, SDS, BUFFERSIZE, 0) >= 0) {
        std::cout << SDS << std::endl;
        Document sds;
        sds.Parse(SDS);

        if (((std::string)sds["Format"].GetString() == "SDS") && sds.IsObject()) {
            std::cout << sds["Data"]["thingID"].GetString() << "_status : " << sds["Data"]["status"].GetString() << std::endl;
            things[i].status = sds["Data"]["status"].GetString();
            return true;
        }
        else {
            printf("sds error()\n");
            return false;
        }
    }
    else {
        printf("recv() error !\n");
        return false;
    }
}

bool statusChangeFuc(std::string thingID, std::string status) {
    std::string SUQ = R"(
	{
		"Format": "SUQ",
		"Data":{
			"thingID" : ")" + thingID + R"(",
			"status" : ")" + status + R"("
		}
	})";
    int i;
    char SUS[BUFFERSIZE] = "";

    for (i = 0; i < clntCnt; i++) {//ã��
        if (things[i].thing_id == thingID)break;
    }

    //send event �߻�
    send(things[i].socket_id, SUQ.c_str(), strlen(SUQ.c_str()), 0);
    recv(things[i].socket_id, SUS, BUFFERSIZE, 0);

    Document sus;
    sus.Parse(SUS);
    std::cout << SUS << std::endl;
    if (((std::string)sus["Format"].GetString() == "SUS") && sus.IsObject() && ((std::string)sus["Data"]["Info"].GetString() == "Done")) {
        std::cout << sus["Data"]["thingID"].GetString() << std::endl;
        things[i].status = status;
        return true;
    }
    return false;
}

void parse_coord(char* coord) {
    std::vector<std::string> arr = split((std::string)coord, (std::string)",");
    int old_x = x;
    try
    {
        x = std::stoi(arr[0])*2;
    }
    catch (const std::exception&)
    {
        x = old_x;
    }
    int old_y = y;
    try
    {
        y = std::stoi(arr[1])*2.5;
    }
    catch (const std::exception&)
    {
        y = old_y;
    }
    int old_gesture = gesture;
    try
    {
        gesture = std::stoi(arr[2]);
        if (gesture >= 1000 && gesture < 10000)
            gesture /= 10;
        else if (gesture >= 10000)
            gesture /= 100;
    }
    catch (const std::exception&)
    {
        gesture = old_gesture;
    }
}
void draw_boxes(cv::Mat mat_img, std::vector<bbox_t> result_vec) {
    for (auto &i : result_vec) {
        cv::rectangle(mat_img, cv::Rect(i.x, i.y, i.w, i.h), cv::Scalar(50, 200, 50), 3);
        cv::imshow("window name", mat_img);
        cv::waitKey(0);
    }
}

std::string cfg = "C:\\Users\\dunghwa\\Downloads\\4-Tower_cfg2.cfg";
std::string weights = "C:\\Users\\dunghwa\\Downloads\\4-Tower_cfg2_30000.weights";
int top(cv::Mat& frame, int x, int y, int width, int height) {
	int h = (height - 15) / 2;
	cv::Point pos(x, y);
	time_t current_time;
	struct tm  local_time;

	time(&current_time);
	localtime_s(&local_time, &current_time);

	cvui::rect(frame, pos.x, pos.y, width, height / 3, 0x4a4a4a, 0x212121);
	cvui::printf(frame, 225, 10, "%d / %d / %d  %d : %d : %d", local_time.tm_year + 1900, local_time.tm_mon + 1, local_time.tm_mday, local_time.tm_hour, local_time.tm_min, local_time.tm_sec);
	pos.y += h;

	return h;
}

int bottom(cv::Mat& frame, int x, int y, int width, int height) {
	int padding = 5, w = (width - padding) / 4, h = (height - 15 - padding) / 2;
	cv::Point pos(x + padding, y + padding);

	cvui::window(frame, pos.x, pos.y, width - padding * 2, height - padding * 2, "Status information");
	pos.y += h;

	return h;
}

void detector() {
    int height = 100;
    cv::Rect rectangle(0, 0, 0, 0);
    cvui::init(WINDOW_NAME);

    Detector detector(cfg, weights);
    cv::VideoCapture capture(0);
    if (!capture.isOpened()) {
        std::cerr << "could not open camera" << std::endl;
        return;
    }

    while (true) {
        bool frame_valid = true;
        cv::Mat frame;
        try {
            capture >> frame;
            result_vec = detector.detect(frame);
            top(frame, 0, 0, frame.cols, height);
            bottom(frame, 0, 380, frame.cols, height);
            //cvui::text(frame, 10, 50, "back camera video area");
            std::string str = "IS SELECTED : ";
            str += is_selected ? "TRUE" : "FALSE";
            cvui::printf(frame, 20, 420, str.c_str());
            if (is_selected) {
                std::string selectedThing = "Selected : " + selected_thing_id;
                cvui::printf(frame, 20, 440, selectedThing.c_str());
                
                int i = 0;
                for (i = 0; i < clntCnt; i++) {//ã��
                    if (things[i].thing_id == selected_thing_id)break;
                }
                std::string selectedStatus = "Type : " + things[i].type + " , Status : " + things[i].status;
                cvui::printf(frame, 20, 460, selectedStatus.c_str());

            }
            else {
                if (objectId != -1)
                    cvui::printf(frame, 20, 440, cObjectId);
                else {
                    cvui::printf(frame, 20, 440, "No object");
                    //std::ostringstream buff;
                    //buff << result_vec[objectId].prob;
                    //cvui::printf(frame, 200, 440, buff.str().c_str());
                }
            }
            //cvui::printf(frame, 20, 420, "Mouse pointer is at (%d,%d)", cvui::mouse().x, cvui::mouse().y);
            cv::resize(frame, frame, cv::Size(640, 420));
        }
        catch (cv::Exception e) {
            std::cerr << "Exception occured Ignoring frame.." << e.err << std::endl;
            frame_valid = false;
        }
        if (frame_valid) {

            circle(frame, cv::Point(640-x, y), 10.0, Scalar(0, 0, 255), -1, 8);
            
            /*for (auto &i : result_vec) {
                cv::rectangle(frame, cv::Rect(i.x, i.y, i.w, i.h), cv::Scalar(50, 200, 50), 3);
            }*/
            
            gesture_event_handler();
        }

        cvui::update();
        

        cv::imshow(WINDOW_NAME, frame);
        
        if (cv::waitKey(30) >= 30) break;
    }

}
void device_Id_Int_to_parse(int iObjectId, char* cObjectId) {
    if (iObjectId == 0)
        strcpy(cObjectId, "AirConditioner");
    else if (iObjectId == 1)
        strcpy(cObjectId, "RiceCooker");
    else if (iObjectId == 2)
        strcpy(cObjectId, "Washer");
    //else if (iObjectId == 3)
    //    strcpy(cObjectId, "Voccum cleaner");
    else if (iObjectId == 4)
        strcpy(cObjectId, "Fan");
    else if (iObjectId == 5)
        strcpy(cObjectId, "Tv");
    else if (iObjectId == 6)
        strcpy(cObjectId, "Door");
}
void gesture_event_handler() {
    if(gesture != 100)
        printf("%d\n", gesture);
    objectId = Hand_in_Range(result_vec);
    device_Id_Int_to_parse(objectId, cObjectId);
}
int Hand_in_Range(std::vector<bbox_t> result_vec) {
    x = 640 - x;
    for (auto &i : result_vec) {
        int _x = i.x, _y = i.y;
        int w = i.w, h = i.h;
        int leftX = _x, rightX = _x + w;
        int upperY = _y, underY = _y + h;
        if ((x >= leftX) && (x <= rightX) 
            && (y >= upperY)  && (y <= underY)) {
            return i.obj_id;
        }//end if
    } //end for
    return -1;
}
void communicate_hand_tracker() {
    WSADATA wsaData;
    hand_tracking_socket = NULL;
    //device_socket = NULL;
    int socket_result = 0;

    //���� ���̺귯�� �ʱ�ȭ
    socket_result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (socket_result != 0) ErrorHandling("WSAStartup() Error!");

    //���� ����
    server_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) ErrorHandling("socket() error!");

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    //���Ͽ� IP�ּҿ� PORT ��ȣ �Ҵ�
    socket_result = bind(server_socket, (SOCKADDR*)&server_addr, sizeof(server_addr));
    if (socket_result == SOCKET_ERROR) ErrorHandling("bind() error!");
    socket_result = listen(server_socket, SOMAXCONN);
    if (socket_result == SOCKET_ERROR) ErrorHandling("listen() error!");
    std::cout << "listening clinet..." << std::endl;
    THREAD accept_thread(&listen_client);
    while (TRUE) {
        //while ���ο��� �������� �ڵ� �־������
        if (hand_tracking_socket) {
            int len = 0;
            if (0 < (len = (recv(hand_tracking_socket, coord, sizeof(coord), 0)))); {
                parse_coord(coord);
                if (gesture == SELECT) {
                    printf("SELECT\n");
                    if (objectId != -1) {
                        printf("Object ID : %d\n", objectId);
                        printf("Device : %s\n", cObjectId);
                        if (is_selected) {
                            is_selected = false;
                            selected_thing_id = "";
                            printf("DESELECT\n");
                        }
                        else {
                            if (selectFunc(cObjectId)) {
                                is_selected = true;
                                selected_thing_id = cObjectId;
                            }
                            else
                                printf("select���� Error �߻�\n");
                        }
                    }
                }
                if (gesture == ON&&is_selected){
                    printf("ON, %s\n", selected_thing_id);
                    if(!statusChangeFuc(selected_thing_id,"on"))printf("statusChange���� Error �߻�\n");
                }
                if (gesture == OFF&&is_selected){
                    printf("OFF, %s\n", selected_thing_id);
                    if (!statusChangeFuc(selected_thing_id, "off"))printf("statusChange���� Error �߻�\n");
                }
            }
            char connect_message[BUFFERSIZE] = { "Coord recv success!" };
            send(hand_tracking_socket, connect_message, strlen(connect_message), 0);
        }//end if
        else {
            fprintf(stderr, "socket error!\n");
        }
    }//end while 
    close_socket();
}

void close_socket() {
    closesocket(server_socket);
    closesocket(client_socket);
    WSACleanup();
}
int main()
{

    std::thread socket_server(&communicate_hand_tracker);
    std::thread detector(&detector);
    //_timer.join();
    socket_server.join();
    detector.join();
    return 0;
}
