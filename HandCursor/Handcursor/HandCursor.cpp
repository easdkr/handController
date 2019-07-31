#include "HandCursor.h"

Session *session;
float maxRangeValue = 1000;
int m_cursorClick[2] = {};
std::vector<Point3DF32> m_cursorPoints[2] = {};
CursorData::BodySideType m_cursorBodyType[2] = {};
int gestureIndex = 0;
bool liveCamera = false;
HandCursorModule *handCursorAnalyzer = NULL;

int Handcursor()
{
    Mat image(Size(640, 420), Mat::AUTO_STEP);

    VideoCapture cap(1);
    int camOpen = cap.open(CV_CAP_ANY);

    namedWindow("window", WINDOW_NORMAL);

    resizeWindow("window", 640, 420);




    session = Session::CreateInstance();   //세션만든다음
    SenseManager *sm = session->CreateSenseManager();   //senseManager 만들어야함
    if (!sm)
    {
        printf("Failed to create Sensemanager");
        return 0;
    }
    bool sts2 = true;

    Status status = sm->EnableHandCursor(0);
    if (status != Status::STATUS_NO_ERROR)
    {
        printf("Failed to pair the hand cursor module with I/O");
        return 0;
    }

    int timer; //timer
    printf("Init Started\n");


    handCursorAnalyzer = sm->QueryHandCursor();
    if (handCursorAnalyzer == NULL)
    {
        printf("Failed to pair the hand cursor module with I/O2");
        return 0;
    }
    DeviceInfo dinfo;
    //Init
    if (sm->Init() >= STATUS_NO_ERROR)  //카메라 켜짐 확인
    {
        Capture::Device *device = sm->QueryCaptureManager()->QueryDevice();
        maxRangeValue = device->QueryDepthSensorRange().max;

        CursorData* outputCursorData = NULL;
        CursorConfiguration* configCursor = NULL;

        if (handCursorAnalyzer)
        {
            outputCursorData = handCursorAnalyzer->CreateOutput();
            configCursor = handCursorAnalyzer->CreateActiveConfiguration();
            printf("handCursorAnalyzer\n");
        }
        else
            return 0;

        if (configCursor)
        {
            configCursor->EnableAllAlerts();
            configCursor->ApplyChanges();
            printf("configCursor\n");
        }
        else
            return 0;

        int frameCounter = 0;
        int frameNumber = 0;

        device->QueryDeviceInfo(&dinfo);
        if (dinfo.model == Capture::DEVICE_MODEL_SR300)
        {
            while (true) {
                if (configCursor != NULL)
                {
                    configCursor->DisableAllGestures();
                    configCursor->EnableGesture(CursorData::GestureType::CURSOR_CLICK);
                    configCursor->EnableGesture(CursorData::GestureType::CURSOR_CLOCKWISE_CIRCLE);
                    configCursor->EnableGesture(CursorData::GestureType::CURSOR_COUNTER_CLOCKWISE_CIRCLE);
                    configCursor->ApplyChanges();
                }

                Status sts = sm->AcquireFrame(true);
                if (sm->IsConnected())
                {
                    //printf("%d\n", (int)(sm->IsConnected()));
                    if (sts < STATUS_NO_ERROR)
                        break;


                    cap >> image;

                    imshow("window", image);
                    frameCounter++;
                    //printf("%d\n",frameCounter);

                    const Capture::Sample *sample;
                    sample = sm->QueryHandCursorSample();
                    //printf("1\n");
                    if (sample)
                    {
                        //printf("2\n");
                        frameNumber = liveCamera ? frameCounter : sm->QueryCaptureManager()->QueryFrameIndex();
                        //printf("%d\n", frameNumber);
                        if (outputCursorData != NULL)
                        {
                            outputCursorData->Update();
                            //DisplayCursorPoint
                            m_cursorClick[0] = max(0, m_cursorClick[0] - 1);
                            m_cursorClick[1] = max(0, m_cursorClick[1] - 1);

                            //Iterate hands

                            int32_t numOfHands = outputCursorData->QueryNumberOfCursors();
                            //printf("%d\n", numOfHands);

                            if (numOfHands == 0)
                            {
                                m_cursorPoints[0].clear();
                                m_cursorPoints[1].clear();
                            }

                            if (numOfHands == 1)
                            {
                                m_cursorPoints[1].clear();
                            }

                            for (int32_t i = 0; i < numOfHands; i++)
                            {
                                CursorData::ICursor* cursorData;
                                if (outputCursorData->QueryCursorData(CursorData::AccessOrderType::ACCESS_ORDER_BY_TIME, i, cursorData) == STATUS_NO_ERROR)
                                {
                                    //cursor point
                                    imagePoint = cursorData->QueryCursorImagePoint();
                                    imagePoint.x = GetXPosition(imagePoint.x);
                                    imagePoint.y = GetYPosition(imagePoint.y);
                                    //printf("%f\t%f\n", imagePoint.x, imagePoint.y); //ok

                                    if (m_cursorPoints[i].size() > 50)
                                        m_cursorPoints[i].erase(m_cursorPoints[i].begin());
                                    m_cursorPoints[i].push_back(imagePoint);

                                    CursorData::GestureData gestureData;
                                    if (outputCursorData->IsGestureFiredByHand(CursorData::CURSOR_CLICK, cursorData->QueryUniqueId(), gestureData))
                                    {
                                        m_cursorClick[i] = 7;
                                    }

                                    m_cursorBodyType[i] = cursorData->QueryBodySide();
                                }

                            }
                            //draw cursor
                            for (int i = 0; i < MAX_NUM_OF_HANDS; ++i)
                            {
                                int sz = 4;

                                for (int j = 0; j < m_cursorPoints[i].size(); ++j)
                                {
                                    int pointImageX = (int)m_cursorPoints[i][j].x;
                                    int pointImageY = (int)m_cursorPoints[i][j].y;
                                    //요놈이 좌표 찍어주는 놈
                                    //printf("%d\t%d\n", pointImageX, pointImageY);
                                    circle(image, Point(pointImageX, pointImageY), 5, CV_RGB(255, 255, 0), 10);
                                    Mat circleImage = image.clone();
                                    imshow("window", circleImage);

                                }

                                //draw cursor click gesture
                                if (0 < m_cursorClick[i])
                                {
                                    Point3DF32 clickPoint = { 0.0, 0.0, 0.0 };
                                    if (m_cursorPoints[i].size() > 0)
                                        clickPoint = m_cursorPoints[i][m_cursorPoints[i].size() - 1];

                                    int pointImageX = (int)clickPoint.x;
                                    int pointImageY = (int)clickPoint.y;
                                    circle(image, Point(pointImageX, pointImageY), 20, CV_RGB(0, 255, 0), 10);
                                    Mat clickImage = image.clone();
                                    imshow("window", clickImage);

                                }
                            }

                            //Display Cursor Gesture
                            int numOfGesture = outputCursorData->QueryFiredGesturesNumber();

                            if (numOfGesture > 0)
                            {
                                for (int i = 0; i < numOfGesture; i++)
                                {
                                    CursorData::GestureData gestureData;
                                    if (outputCursorData->QueryFiredGestureData(i, gestureData) == STATUS_NO_ERROR)
                                    {
                                        CursorData::ICursor* cursorData;
                                        if (outputCursorData->QueryCursorDataById(gestureData.handId, cursorData) == STATUS_NO_ERROR)
                                        {
                                            if (cursorData->QueryBodySide() == CursorData::BodySideType::BODY_SIDE_LEFT)
                                            {
                                                printf("Left Hand Gesture\n");
                                            }
                                            else if (cursorData->QueryBodySide() == CursorData::BodySideType::BODY_SIDE_RIGHT)
                                            {
                                                printf("Right Hand Gesture\n");
                                            }

                                            switch (gestureData.label)
                                            {
                                            case CursorData::CURSOR_CLICK:
                                                GestureNum = 200;
                                                printf("cursor_click\n");
                                                break;
                                            case CursorData::CURSOR_CLOCKWISE_CIRCLE:
                                                GestureNum = 300;
                                                printf("CURSOR_CLOCKWISE_CIRCLE\n");
                                                break;
                                            case CursorData::CURSOR_COUNTER_CLOCKWISE_CIRCLE:
                                                GestureNum = 400;
                                                printf("CURSOR_COUNTER_CLOCKWISE_CIRCLE\n");
                                                break;
                                            default:
                                                GestureNum = 100;
                                                break;
                                            }//end of switch 
                                        }//end if (outputCursorData->QueryCursorDataById(gestureData.handId, cursorData) == STATUS_NO_ERROR
                                    }//if (outputCursorData->QueryFiredGestureData(i, gestureData) == STATUS_NO_ERROR)
                                }//for ~ 
                            }//end if (numOfGesture > 0)
                            else {
                                GestureNum = 100;
                                //printf("gestureNum : %d\n", GestureNum);
                            }
                        }
                    }


                    waitKey(15);
                }


                //// delay 33ms


                sm->ReleaseFrame();
            }//end while
        }

        if (configCursor) configCursor->Release();
        if (outputCursorData) outputCursorData->Release();
    }
    else
    {
        printf("Init Failed\n");
        return 0;
    }

    if (sm)
    {
        sm->Close();
        sm->Release();
    }
    return 0;
}
float GetXPosition(float cursorXPos)
{
    if (cursorXPos < CURSOR_FACTOR_X)
    {
        return 0.0;
    }
    if (cursorXPos >= 1280 - CURSOR_FACTOR_X)
    {
        return (float)1280 - 1.0f;
    }
    return ((cursorXPos - CURSOR_FACTOR_X) / (1280 - 2 * CURSOR_FACTOR_X)) * 1280;
}
float GetYPosition(float cursorYPos)
{
    if (cursorYPos < CURSOR_FACTOR_Y_UP)
    {
        return 0.0;
    }
    if (cursorYPos >= 720 - CURSOR_FACTOR_Y_DOWN)
    {
        return (float)720 - 1.0f;
    }
    return ((cursorYPos - CURSOR_FACTOR_Y_UP) / (720 - CURSOR_FACTOR_Y_UP - CURSOR_FACTOR_Y_DOWN)) * 720;
}
//std::string coord_to_string() {
//    std::string x = std::to_string(int(imagePoint.x)); //+100
//    std::string y = std::to_string(int(imagePoint.y)); //+40
//    if (GestureNum != 100) printf("GestureNum : %d\n", GestureNum);
//    std::string gesture = std::to_string((int)GestureNum);
//    std::string coord = x + "," + y + "," + gesture;
//    return coord;
//}
//
//void select_send() {
//    char recv_message[BUFFERSIZE];
//    std::string coord = coord_to_string(); //좌표 값을 string 형태로 변환 
//    c_coord = new char[coord.length() + 1]; // 좌표 값을 담을 char 배열 초기화
//    strcpy(c_coord, coord.c_str()); //char 배열에 좌표 값 복사
//                                    //printf("%s\n", c_coord);
//                                    //if (GestureNum != 100)printf("ㅅ\n");
//    if (0 < send(_socket, c_coord, strlen(c_coord), 0)) {
//
//    }; //보내기 
//    if (0 > (recv(_socket, recv_message, sizeof(recv_message), 0)))
//        ErrorHandling("Coordinate send error!");
//    printf("Message from server : %s\n", recv_message);
//}