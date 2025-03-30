#include <opencv2/opencv.hpp>
#include <chrono>
#include "./serial/serial.h"
#include "./opencv/opencv.h"



int main() {
    SerialClass serial;
    while(true)
    {
        unsigned char * buf = serial.Serial_Read();
        serial.Serial_Process(buf);
    }
}


// int main()
// {
//     OpencvClass Camera;
//     cv::Mat frame;
//     frame = Camera.GetCurFrame();
//     cv::imwrite("test.jpg",frame);
//     // while(true)
//     // {
//     //     frame = Camera.GetCurFrame(); 
//     //     if (frame.empty())
//     //     {
//     //         std::cout << "Frame is empty" << std::endl;
//     //     }
//     //     cv::imshow("frame", frame);
//     //     cv::waitKey(20);
//     // }
// }
// #include <opencv2/opencv.hpp>
// #include <iostream>

// int main() {
//     // 读取图像
//     OpencvClass Camera;
//     cv::Mat frame;
//     frame = Camera.GetCurFrame();
//     if (frame.empty()) {
//         std::cerr << "无法加载图像！" << std::endl;
//         return -1;
//     }

//     // 定义固定区域
//     cv::Rect roi(230, 410, 250, 69);

//     // 提取固定区域
//     cv::Mat region = frame(roi);

//     // 转换到HSV颜色空间
//     cv::Mat hsv_region;
//     cv::cvtColor(region, hsv_region, cv::COLOR_BGR2HSV);

//     // 定义蓝色范围
//     cv::Scalar lower_blue(78, 43, 46); // 这里的值是示例，可能需要根据实际情况调整
//     cv::Scalar upper_blue(110, 255, 255); // 同样，这里的值也需要根据具体需求调整

//     // 创建蓝色掩码
//     cv::Mat blue_mask;
//     cv::inRange(hsv_region, lower_blue, upper_blue, blue_mask);

//     // 计算蓝色填充度
//     int total_pixels = blue_mask.rows * blue_mask.cols;
//     int blue_pixels = cv::countNonZero(blue_mask);

//     double blue_fill_ratio = static_cast<double>(blue_pixels) / total_pixels;
//     std::cout << "蓝色填充度: " << blue_fill_ratio * 100 << "%" << std::endl;

//     // 显示结果（可选）
//     cv::imshow("Original", frame);
//     cv::imshow("Blue Mask", blue_mask);
//     cv::waitKey(0);

//     return 0;
// }

// int main() {
//     // 读取图像
//     OpencvClass Camera;
//     cv::Mat frame;
//     frame = Camera.GetCurFrame();
//     if (frame.empty()) {
//         std::cerr << "无法加载图像！" << std::endl;
//         return -1;
//     }

//     // 定义固定区域
//     cv::Rect roi(230, 410, 250, 69);

//     // 提取固定区域
//     cv::Mat region = frame(roi);

//     // 转换到HSV颜色空间
//     cv::Mat hsv_region;
//     cv::cvtColor(region, hsv_region, cv::COLOR_BGR2HSV);

//     // 定义红色范围（分为低范围和高范围）
//     cv::Scalar lower_red1(0, 100, 100);   // 低范围
//     cv::Scalar upper_red1(10, 255, 255);  // 低范围
//     cv::Scalar lower_red2(170, 100, 100); // 高范围
//     cv::Scalar upper_red2(180, 255, 255); // 高范围

//     // 创建红色掩码（低范围和高范围合并）
//     cv::Mat red_mask1, red_mask2, red_mask;
//     cv::inRange(hsv_region, lower_red1, upper_red1, red_mask1);
//     cv::inRange(hsv_region, lower_red2, upper_red2, red_mask2);
//     cv::bitwise_or(red_mask1, red_mask2, red_mask);

//     // 计算红色填充度
//     int total_pixels = red_mask.rows * red_mask.cols;
//     int red_pixels = cv::countNonZero(red_mask);

//     double red_fill_ratio = static_cast<double>(red_pixels) / total_pixels;
//     std::cout << "红色填充度: " << red_fill_ratio * 100 << "%" << std::endl;

//     // 显示结果（可选）
//     cv::imshow("Original", frame);
//     cv::imshow("Red Mask", red_mask);
//     cv::waitKey(0);

//     return 0;
// }