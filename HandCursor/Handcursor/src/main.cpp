#include "hand_client.h"
#include "HandCursor.h"

void Hand_Cursor_thread() {
    printf("Hand Tracking Start\n");
    Handcursor();
}
std::string coord_to_string() {
    std::string x = std::to_string(int(imagePoint.x)); //+100
    std::string y = std::to_string(int(imagePoint.y)); //+40
    if (GestureNum != NORMAL) 
        printf("GestureNum : %d\n", GestureNum);
    std::string gesture = std::to_string((int)GestureNum);
    std::string coord = x + "," + y +","+ gesture;
    return coord;
}

void send_coordinate() {
    connect_server();
    printf("Connect server :: %d\n", port);
    char recv_message[BUFFERSIZE];
    char send_message[BUFFERSIZE] = "0";
    send(_socket, send_message, strlen(send_message), 0);//send (1)
    printf("Send to server : %s\n", send_message);
    if (0 < (recv(_socket, recv_message, sizeof(recv_message), 0))) {
        printf("Message from server : %s\n", recv_message);
        while (TRUE) {
            printf("%d\n", GestureNum);
            std::string coord = coord_to_string(); //좌표 값을 string 형태로 변환 
            c_coord = new char[coord.length() + 1]; // 좌표 값을 담을 char 배열 초기화
            strcpy(c_coord, coord.c_str()); //char 배열에 좌표 값 복사
            send(_socket, c_coord, strlen(c_coord), 0);
            if (0 > (recv(_socket, recv_message, sizeof(recv_message), 0)))
                ErrorHandling("Coordinate send error!"); 
            else {
                if((GestureNum==ON)|| (GestureNum == OFF)|| (GestureNum == SELECT)) 
                    GestureNum = NORMAL; 
            }
            //printf("Message from server : %s\n", recv_message);
            /*GestureNum = 100;*/
        }//end while 
    }
    else {
        ErrorHandling("recv() error!");
    }
    close_socket();
}

int main() 
{
    std::thread hand_cursor(&Hand_Cursor_thread);
    std::thread send_coordinate(&send_coordinate);

    hand_cursor.join();
    send_coordinate.join();
    return 0;
}
