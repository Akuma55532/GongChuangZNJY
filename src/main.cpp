#include <iostream>
#include <thread>
#include <algorithm>
#include <chrono>
#include <opencv2/opencv.hpp>
#include "./serial/serial.h"
#include "./opencv/opencv.h"
#include "./task/yolov5.h"
#include "./task/yolov5_thread_pool.h"
#include "./draw/cv_draw.h"

#define num_threads 3

typedef enum
{
    StatusBallDetection = 0,
    StatusBallDetected = 1,
    StatusPlaceDection = 2,
    StatusPlaceDetected = 3,
    StatusRedStartDection = 4,
    StatusRedStartDetected = 5,
    StatusBlueStartDection = 6,
    StatusBlueStartDetected = 7,
    StatusAfterPlaceDetection = 8,
    StatusAfterPlaceDetected = 9,
    StatusAfterBallDetected = 10
} status_enum;


typedef enum
{
    NOT_IN_ANY_START = 0,
    IN_BLUE_START = 1,
    IN_RED_START = 2,
    IN_BETWEEN_BLUE_AND_RED_START = 3
} where_enum;

status_enum status = StatusBallDetection;
status_enum status_next = StatusBallDetection;
where_enum where = IN_BLUE_START; // change change change change change change

std::unique_ptr<SerialClass> Serial;
std::unique_ptr<OpencvClass> Camera;
std::unique_ptr<Yolov5ThreadPool> yolothreadpool;

bool isVideoEnd = false;

bool isFirsrBall = true;

cv::Mat Curentframe;

std::vector<Detection> Targetobjects;
cv::Mat TargetImg;

std::vector<std::string> TargetBall = {"Blue_Ball","Black_Ball","Yellow_Ball"}; // change change change change change change
std::string TargetZone = "Blue_Placement_Zone"; // change change change change change change

std::string MyBall = "Blue_Ball"; // change change change change change change

std::string YourBall = "Red_Ball";

cv::Rect2d Blue_Zone_Rect;
cv::Rect2d Red_Zone_Rect;
cv::Rect2d Blue_Start_Rect;
cv::Rect2d Red_Start_Rect;

float MOVEX_Gain = 0.0015;
float MOVEZ_Gain = 0.06;

int stable_count = 0;
int stop_count = 0;

#define camera_center_x 338
#define camera_center_y 445

std::map<std::string, int> ball_side = {
    {"ball_leftside", 330},
    {"ball_rightside", 346},
    {"ball_upside", 380},
    {"ball_downside", 480}
};

std::map<std::string, int> zone_side = {
    {"zone_leftside", 260},
    {"zone_rightside", 420},
    {"zone_upside", 360},
    {"zone_downside", 420}
};

std::map<std::string, int> zone_back_side = {
    {"zone_back_upside", 160},
    {"zone_back_downside", 220}
};

std::map<std::string, int> start_side = {
    {"start_leftside", 200},
    {"start_rightside", 480},
    {"start_upside", 300},
    {"start_downside", 480}
};

bool IS_IN_DetectZone = false;

cv::Scalar lower_blue(78, 43, 46); // 这里的值是示例，可能需要根据实际情况调整
cv::Scalar upper_blue(110, 255, 255); // 同样，这里的值也需要根据具体需求调整

cv::Scalar lower_red(78, 43, 46); // 这里的值是示例，可能需要根据实际情况调整
cv::Scalar upper_red(110, 255, 255); // 同样，这里的值也需要根据具体需求调整

cv::Rect roi(230, 410, 250, 69);

bool isMyBall = false;

bool isStringInVector(const std::string& str, const std::vector<std::string>& vec) {
    return std::find(vec.begin(), vec.end(), str) != vec.end();
}

void xianfu_MOVE_X()
{
    if(MOVE_X < 0)
    {
        MOVE_X = -MOVE_X;
        X_REVERSAL_SINGLE = 1;
    }
    if(MOVE_X > 0.5)
    {
        MOVE_X = 0.5;
    }
}

void Submmit_Task()
{
    cv::Mat hsv;
    cv::Mat hsv_region;
    cv::Mat mask;
    while (!isVideoEnd)
    {
        Curentframe = Camera->GetCurFrame();
        if (Curentframe.empty())
        {
            NN_LOG_INFO("Video isvideoend.");
            isVideoEnd = true;
            break;
        }
        yolothreadpool->submitTask(Curentframe.clone(), 0);
        if(status == StatusPlaceDection)
        {
            cv::Mat region = Curentframe(roi);
            cv::cvtColor(Curentframe, hsv, cv::COLOR_BGR2HSV);
            
            cv::cvtColor(region, hsv_region, cv::COLOR_BGR2HSV);
            
            cv::inRange(hsv_region, lower_blue, upper_blue, mask);
            // cv::inRange(hsv_region, lower_red, upper_red, mask);
            int total_pixels = mask.rows * mask.cols;
            int color_pixels = cv::countNonZero(mask);

            double color_fill_ratio = static_cast<double>(color_pixels) / total_pixels;
            std::cout << color_fill_ratio << std::endl;
            if(isMyBall == true)
            {
                if(color_fill_ratio > 0.95 )
                {
                    IS_IN_DetectZone = true;
                }
            }
            else {
                if(color_fill_ratio > 0.67 )
                {
                    IS_IN_DetectZone = true;
                }
            }
        }
        cv::waitKey(10);
    }
}

void Control_Task()
{
    while (!isVideoEnd)
    {
        MOVE_X = 0;MOVE_Z = 0;X_REVERSAL_SINGLE = 0;
        int box_area_max = 0;
        int box_area_max_your = 0;
        cv::Point2d Center;
        cv::Point2d YorCenter;
        
        // 获取识别结果
        auto ret = yolothreadpool->getTargetResult(Targetobjects);

        cv::waitKey(10);

        //遍历所有objects,找到BLUE_ZONE和RED_ZONE的位置(如果视野中有的话)
        for (auto &obj : Targetobjects)
        {
            if(obj.className == "Blue_Placement_Zone")
            {
                Blue_Zone_Rect = obj.box;
            }
            else if(obj.className == "Red_Placement_Zone")
            {
                Red_Zone_Rect = obj.box;
            }
            // std::cout << obj.className << std::endl;
            if(isFirsrBall)
            {
                if(obj.className == "Blue_Ball" && status == StatusBallDetection)
                {
                    status = StatusBallDetected;
                    if(obj.box.area() > box_area_max)
                    {
                        box_area_max = obj.box.area();
                        Center = cv::Point2d(obj.box.x + obj.box.width / 2, obj.box.y + obj.box.height / 2);
                        if(obj.className == MyBall)
                        {
                            isMyBall = true;
                        }
                        else
                        {
                            isMyBall = false;
                        }
                    }
                }
            }
            else {
                if(isStringInVector(obj.className,TargetBall) && status == StatusBallDetection) // 如果识别到可抓取球，则进行判断其是否在蓝 blue placement zone内
                {
                    cv::Point2d center = cv::Point2d(obj.box.x + obj.box.width / 2, obj.box.y + obj.box.height / 2);
                    if((center.x > Blue_Zone_Rect.x && center.x < Blue_Zone_Rect.x + Blue_Zone_Rect.width && center.y < Blue_Zone_Rect.y + Blue_Zone_Rect.height)
    || (center.x > Red_Zone_Rect.x && center.x < Red_Zone_Rect.x + Red_Zone_Rect.width && center.y < Red_Zone_Rect.y + Red_Zone_Rect.height)
    || (center.y < 128))
                    {
                        continue;
                    }
                    else { // 如果没在就可以继续判断是不是面积最大(距离最近的球)
                        status = StatusBallDetected;
                        if(obj.box.area() > box_area_max)
                        {
                            box_area_max = obj.box.area();
                            Center = cv::Point2d(obj.box.x + obj.box.width / 2, obj.box.y + obj.box.height / 2);
                            if(obj.className == MyBall)
                            {
                                isMyBall = true;
                            }
                            else
                            {
                                isMyBall = false;
                            }
                        }
                    }
                }
            }
            if (obj.className == YourBall && SCRATCH_FLAG == 1)
            {
                if(obj.box.area() > box_area_max_your)
                {
                    box_area_max_your = obj.box.area();
                    YorCenter = cv::Point2d(obj.box.x + obj.box.width / 2, obj.box.y + obj.box.height / 2);
                }
            }
            if (obj.className == TargetZone && status == StatusPlaceDection) //如果识别到框框，并且正处于放置阶段，则进行判断其是否在框框内
            {
                status = StatusPlaceDetected;
                Center = cv::Point2d(obj.box.x + obj.box.width / 2, obj.box.y + obj.box.height / 2);
            }
            else if (obj.className == TargetZone && status == StatusAfterPlaceDetection) //如果放置完成，则进行后退
            {
                status = StatusAfterPlaceDetected;
                Center = cv::Point2d(obj.box.x + obj.box.width / 2, obj.box.y + obj.box.height / 2);
            }
            else if(obj.className == "Blue_Start_Zone" && where == NOT_IN_ANY_START && stable_count > 10) //如果前面都不属于，则进行寻找红蓝起始区
            {
                Blue_Start_Rect = obj.box;
                status = StatusBlueStartDetected;
                Center = cv::Point2d(obj.box.x + obj.box.width / 2, obj.box.y + obj.box.height / 2);
            }
            else if(obj.className == "Red_Start_Zone" && where == NOT_IN_ANY_START && stable_count > 10)
            {
                Red_Start_Rect = obj.box;
                status = StatusRedStartDetected;
                Center = cv::Point2d(obj.box.x + obj.box.width / 2, obj.box.y + obj.box.height / 2);
            }
        }
        // 如果在检测球的过程中检测不到任何bluezone之外的球就转圈(右转)
        if (Center.x == 0 && Center.y == 0)
        {
            stable_count++;
            std::cout << "Can not find anything then Move right" << std::endl;
            if(stable_count > 10)
            {
                TURN_MODE = 1;
                Serial->data_transition();
                Serial->Serial_Write();
            }
        }
        else {
            stable_count = 0;
            TURN_MODE = 0;
            Serial->data_transition();
            Serial->Serial_Write();
        }
        if (IsStoping == 1)
        {
            stop_count++;
            if (stop_count > 20)
            {
                MOVE_X = -0.4;
                if(MOVE_X < 0)
                {
                    MOVE_X = -MOVE_X;
                    X_REVERSAL_SINGLE = 1;
                }
                xianfu_MOVE_X();
                Serial->data_transition();
                Serial->Serial_Write();
                std::this_thread::sleep_for(std::chrono::seconds(2));
                MOVE_X = 0;
                Serial->data_transition();
                Serial->Serial_Write();
            }
        }
        else {
            stop_count = 0;
        }
        std::cout << "status:" << status << std::endl;
        std::cout << "where:" << where << std::endl;
        // 如果在检测球的过程中检测到blue zone之外的球就进行移动
        if (status == StatusBallDetected && (int)SCRATCH_FLAG == 0)
        {
            std::cout << "Blue Ball Detected" << std::endl;
            if(Center.x < ball_side["ball_leftside"])
            {
                std::cout << "Move left" << std::endl;
                MOVE_Z = (MOVEZ_Gain * (camera_center_x - Center.x)) + 1.8;
                status = StatusBallDetection;
            }
            else if(Center.x > ball_side["ball_rightside"])
            {
                std::cout << "Move right" << std::endl;
                MOVE_Z = (MOVEZ_Gain * (camera_center_x - Center.x)) - 1.8;
                status = StatusBallDetection;
            }
            if (Center.y < ball_side["ball_upside"])
            {
                std::cout << "Move forward" << std::endl;
                // MOVE_X = (MOVEX_Gain * (ball_side["ball_upside"] - Center.y)) + 0.18;
                MOVE_X = (MOVEX_Gain * (camera_center_y - Center.y)) + 0.3;
                status = StatusBallDetection;
            }
            else if (Center.y > ball_side["ball_downside"])
            {
                std::cout << "Move Back" << std::endl;
                // MOVE_X = (MOVEX_Gain * (ball_side["ball_downside"] - Center.y)) - 0.18;
                MOVE_X = (MOVEX_Gain * (camera_center_y - Center.y)) + 0.3;
                status = StatusBallDetection;
            }
            if (Center.x > ball_side["ball_leftside"] && Center.x < ball_side["ball_rightside"] && Center.y > ball_side["ball_upside"] && Center.y < ball_side["ball_downside"]) {
                // 如果符合条件就抓取,并且将状态设置为blueZone的检测过程中
                MOVE_X = 0;MOVE_Z = 0;
                SCRATCH_FLAG = 1;
                status = StatusAfterBallDetected;
                status_next = StatusPlaceDection;
                where = NOT_IN_ANY_START;
            }
            xianfu_MOVE_X();
            Serial->data_transition();
            Serial->Serial_Write();
            continue;
        }

        if(SCRATCH_FLAG == 1 && YorCenter.x > ball_side["ball_leftside"] && YorCenter.x < ball_side["ball_rightside"] && YorCenter.y > ball_side["ball_upside"] && YorCenter.y < ball_side["ball_downside"])
        {
            std::cout << "Red Ball !!!" << std::endl;
            SCRATCH_FLAG = 0;
            MOVE_X = -0.4;
            if(MOVE_X < 0)
            {
                MOVE_X = -MOVE_X;
                X_REVERSAL_SINGLE = 1;
            }
            xianfu_MOVE_X();
            Serial->data_transition();
            Serial->Serial_Write();
            std::this_thread::sleep_for(std::chrono::seconds(2));
            MOVE_X = 0;
            Serial->data_transition();
            Serial->Serial_Write();
            status = StatusBallDetection;
            status_next = StatusBallDetection;
            where = NOT_IN_ANY_START;
            continue;
        }

        if(status == StatusAfterBallDetected && (int)SCRATCH_FLAG == 1)
        {
            MOVE_X = -0.4;
            if(MOVE_X < 0)
            {
                MOVE_X = -MOVE_X;
                X_REVERSAL_SINGLE = 1;
            }
            xianfu_MOVE_X();
            Serial->data_transition();
            Serial->Serial_Write();
            std::this_thread::sleep_for(std::chrono::seconds(2));
            MOVE_X = 0;
            Serial->data_transition();
            Serial->Serial_Write();
            status = StatusPlaceDection;
            continue;
        }

        if(status == StatusPlaceDetected && (int)SCRATCH_FLAG == 1)
        {
            std::cout << "Blue_Placement_Zone" << std::endl;
            if(Center.x < zone_side["zone_leftside"])
            {
                std::cout << "Move left" << std::endl;
                MOVE_Z = (MOVEZ_Gain * (camera_center_x - Center.x)) + 1.3;
                status = StatusPlaceDection;
            }
            else if(Center.x > zone_side["zone_rightside"])
            {
                std::cout << "Move right" << std::endl;
                MOVE_Z = (MOVEZ_Gain * (camera_center_x - Center.x)) - 1.3;
                status = StatusPlaceDection;
            }
            if (Center.y < zone_side["zone_upside"])
            {
                std::cout << "Move forward" << std::endl;
                MOVE_X = (MOVEX_Gain * (zone_side["zone_upside"] - Center.y)) + 0.3;
                status = StatusPlaceDection;
            }
            else if (Center.y > zone_side["zone_downside"])
            {
                std::cout << "Move back" << std::endl;
                MOVE_X = (MOVEX_Gain * (zone_side["zone_downside"] - Center.y)) - 0.3;
                status = StatusPlaceDection;
            }
            if (IS_IN_DetectZone && Center.y > zone_side["zone_upside"]-100 && Center.y < zone_side["zone_downside"]+100) {
                // 如果符合条件就抓取,并且将状态设置为blueZone的检测过程中
                IS_IN_DetectZone = false;
                MOVE_X = 0;MOVE_Z = 0;
                SCRATCH_FLAG = 0;
                status = StatusAfterPlaceDetection;
                status_next = StatusAfterPlaceDetection;
                where = NOT_IN_ANY_START;
                isMyBall = false;
                isFirsrBall = false;
            }
            xianfu_MOVE_X();
            Serial->data_transition();
            Serial->Serial_Write();
            continue;
        }
        if (status == StatusAfterPlaceDetected && (int)SCRATCH_FLAG == 0)
        {
            std::cout << "After Blue_Placement_Zone" << std::endl;
            if (Center.y < zone_back_side["zone_back_upside"])
            {
                std::cout << "After Move forward" << std::endl;
                MOVE_X = (MOVEX_Gain * (zone_back_side["zone_back_upside"] - Center.y)) + 0.20;
                status = StatusAfterPlaceDetection;
            }
            else if (Center.y > zone_back_side["zone_back_downside"])
            {
                std::cout << "After Move back" << std::endl;
                MOVE_X = (MOVEX_Gain * (zone_back_side["zone_back_downside"] - Center.y)) - 0.20;
                status = StatusAfterPlaceDetection;
            }
            if (Center.y > zone_back_side["zone_back_upside"] && Center.y < zone_back_side["zone_back_downside"])
            {
                status = StatusBallDetection;
                status_next = StatusBallDetection;
                where = NOT_IN_ANY_START;
            }
            xianfu_MOVE_X();
            Serial->data_transition();
            Serial->Serial_Write();
            continue;
        }
        else if (status == StatusBlueStartDetected && where == NOT_IN_ANY_START && where != IN_RED_START)
        {
            std::cout << "Blue_Start" << std::endl;
            if(Center.x < start_side["start_leftside"])
            {
                std::cout << "Move left" << std::endl;
                MOVE_Z = (MOVEZ_Gain * (camera_center_x - Center.x)) + 1.8;
                status = StatusBlueStartDection;
            }
            else if(Center.x > start_side["start_rightside"])
            {
                std::cout << "Move right" << std::endl;
                MOVE_Z = (MOVEZ_Gain * (camera_center_x - Center.x)) - 1.8;
                status = StatusBlueStartDection;
            }
            if (Center.y < start_side["start_upside"])
            {
                std::cout << "Move forward" << std::endl;
                MOVE_X = (MOVEX_Gain * (start_side["start_upside"] - Center.y)) + 0.4;
                status = StatusBlueStartDection;
            }
            else if (Center.y > start_side["start_downside"])
            {
                std::cout << "Move back" << std::endl;
                MOVE_X = (MOVEX_Gain * (start_side["start_downside"] - Center.y)) - 0.4;
                status = StatusBlueStartDection;
            }
            if (Center.x > start_side["start_leftside"] && Center.x < start_side["start_rightside"] && Center.y > start_side["start_upside"] && Center.y < start_side["start_downside"]) {
                // 如果符合条件就抓取,并且将状态设置为bluestart的检测过程中
                MOVE_X = 0;MOVE_Z = 0;
                status = status_next;
                where = IN_BLUE_START;
            }
            xianfu_MOVE_X();
            Serial->data_transition();
            Serial->Serial_Write();
            continue;
        }
        else if (status == StatusRedStartDetected && where == NOT_IN_ANY_START && where != IN_BLUE_START)
        {
            std::cout << "RED_Start" << std::endl;
            if(Center.x < start_side["start_leftside"])
            {
                std::cout << "Move left" << std::endl;
                MOVE_Z = (MOVEZ_Gain * (camera_center_x - Center.x)) + 1.8;
                status = StatusRedStartDection;
            }
            else if(Center.x > start_side["start_rightside"])
            {
                std::cout << "Move right" << std::endl;
                MOVE_Z = (MOVEZ_Gain * (camera_center_x - Center.x)) - 1.8;
                status = StatusRedStartDection;
            }
            if (Center.y < start_side["start_upside"])
            {
                std::cout << "Move forward" << std::endl;
                MOVE_X = (MOVEX_Gain * (start_side["start_upside"] - Center.y)) + 0.4;
                status = StatusRedStartDection;
            }
            else if (Center.y > start_side["start_downside"])
            {
                std::cout << "Move back" << std::endl;
                MOVE_X = (MOVEX_Gain * (start_side["start_downside"] - Center.y)) - 0.4;
                status = StatusRedStartDection;
            }
            if (Center.x > start_side["start_leftside"] && Center.x < start_side["start_rightside"] && Center.y > start_side["start_upside"] && Center.y < start_side["start_downside"]) {
                // 如果符合条件就抓取,并且将状态设置为redstart的检测过程中
                MOVE_X = 0;MOVE_Z = 0;
                status = status_next;
                where = IN_RED_START;
            }
            xianfu_MOVE_X();
            Serial->data_transition();
            Serial->Serial_Write();
            continue;
        }
    }
}


void Serial_Function(void)
{
    while (!isVideoEnd)
    {
        unsigned char *buf = Serial->Serial_Read();
        Serial->Serial_Process(buf);
    }
}


int main()
{
    Serial = std::make_unique<SerialClass>();
    Camera = std::make_unique<OpencvClass>();
    yolothreadpool = std::make_unique<Yolov5ThreadPool>();

    std::string modelfile = "/home/orangepi/Desktop/Akuma/weights/qiu_best.rknn";
    yolothreadpool->setUp(modelfile, num_threads);

    while(Flag_Stop == 0)
    {
        unsigned char *buf = Serial->Serial_Read();
        Serial->Serial_Process(buf);
    }
    
    std::thread thread_serial(Serial_Function);
    std::thread thread_submmit(Submmit_Task);
    std::thread thread_control(Control_Task);
    
    thread_serial.join();
    thread_submmit.join();
    thread_control.join();
    

    std::cout << "Main thread exit." << std::endl;

    yolothreadpool.reset();
    Camera.reset();
    Serial.reset();
    
    return 0;
}