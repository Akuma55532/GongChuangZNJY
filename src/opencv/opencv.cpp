#include "./opencv.h"
#include <opencv2/opencv.hpp>
#include <iostream>

OpencvClass::OpencvClass(/* args */)
{
    // 打开摄像头
    if(!this->cap.open(CAMERA_PORT)){
        perror("Cap Open Failed!");
    }

    // 设置摄像头的视频帧大小
    int width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    int fps = cap.get(cv::CAP_PROP_FPS);
    this->cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    this->cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
    std::cout << "Video size" << width << "x" << height << " fps:" << fps << std::endl;
}


OpencvClass::~OpencvClass()
{
    this->cap.release();
    cv::destroyAllWindows();
}

cv::Mat & OpencvClass::GetCurFrame()
{
    if(!this->cap.read(this->frame)){
        perror("Frame Get Failed!");
    }
    //上下翻转
    cv::flip(this->frame,this->frame,-1);
    return this->frame;
}