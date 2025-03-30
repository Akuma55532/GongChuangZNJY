#ifndef OPENCV_HPP
#define OPENCV_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <fstream>
#include <vector>

#define CAMERA_PORT "/dev/video0"

class OpencvClass
{
private:
    /* data */
    cv::VideoCapture cap; // 摄像头设备
    cv::Mat frame; // 最新画面
public:
    OpencvClass(/* args */);
    ~OpencvClass();

    cv::Mat & GetCurFrame();
    void FrameProcess(void);
};

#endif