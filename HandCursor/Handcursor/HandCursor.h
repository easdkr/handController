#pragma once
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <iostream>
#include <Windows.h>
#include <WindowsX.h>
#include <vector>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include <string>
#include <map>

#include "RealSense/CaptureManager.h"
#include "RealSense/Metadata.h"
#include "REalSense/Service/SessionService.h"
#include "DeviceHandler.h"
#include "RealSense/Capture.h"
#include "RealSense/VideoModule.h"
#include "RealSense/Session.h"
#include "RealSense/HandCursor/CursorConfiguration.h"
#include "RealSense/HandCursor/CursorData.h"
#include "RealSense/HandCursor/HandCursorModule.h"
#include "RealSense/SenseManager.h"

#include "resource1.h"
#include "timer.h"
#include "HandCursor.h"
#include "socketheader.h"
using namespace std;
using namespace cv;
using namespace Intel::RealSense;
using namespace Intel::RealSense::HandCursor;

#define CURSOR_FACTOR_X 60
#define CURSOR_FACTOR_Y_UP 120
#define CURSOR_FACTOR_Y_DOWN 40
#define MAX_NUM_OF_HANDS 2

int Handcursor();

float GetXPosition(float);
float GetYPosition(float);
__declspec(selectany) int GestureNum = 100;
__declspec(selectany) Point3DF32 imagePoint;

enum GESTURE_ENUM {
    NORMAL = 100,
    SELECT = 200,
    ON = 300,
    OFF = 400
};