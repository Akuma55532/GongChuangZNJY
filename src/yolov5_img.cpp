
#include <opencv2/opencv.hpp>

#include "task/yolov5.h"
#include "utils/logging.h"
#include "draw/cv_draw.h"
#include "./opencv/opencv.h"

int main(int argc, char **argv)
{
    OpencvClass Camera;
    cv::Mat frame;
    frame = Camera.GetCurFrame();
    cv::imwrite("test.jpg",frame);
    // model file path
    const char *model_file = "/home/orangepi/Desktop/Akuma/weights/qiu_best.rknn";
    // input img path
    const char *img_file = "/home/orangepi/Desktop/Akuma/build/test.jpg";
    // 读取图片
    cv::Mat img = cv::imread(img_file);
    // print img size
    NN_LOG_INFO("img size: %d x %d", img.cols, img.rows);

    // 初始化
    Yolov5 yolo;
    // 加载模型
    yolo.LoadModel(model_file);

    // 运行模型
    std::vector<Detection> objects;
    yolo.Run(img, objects);

    // 显示结果
    DrawDetections(img, objects);

    // 保存结果
    cv::imwrite("result.jpg", img);

    return 0;
}