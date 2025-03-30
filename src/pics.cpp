#include "./opencv/opencv.h"
#include <iostream>
#include <sstream> // 用于 stringstream
#include <thread>
#include <mutex>   // 用于线程同步
#include <atomic>  // 用于原子操作

std::unique_ptr<OpencvClass> Camera;
cv::Mat frame; // 当前帧
std::atomic<bool> is_running(true); // 用于控制线程的运行状态


int main()
{
    int index = 0;
    Camera = std::make_unique<OpencvClass>();
    while(true)
    {
        frame = Camera->GetCurFrame();
        if (frame.empty())
        {
            std::cout << "Video stream ended." << std::endl;
            break;
        }
        cv::imshow("frame", frame);

        int key = cv::waitKey(30);
        if(key == 'q')
        {
            std::stringstream filename;
            filename << "/home/orangepi/Desktop/Akuma/imgs/image_" << index++ << ".jpg";

            cv::imwrite(filename.str(), frame);
            std::cout << "Image saved: " << filename.str() << std::endl;
        }
    }

    return 0;
}