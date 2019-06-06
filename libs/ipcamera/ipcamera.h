#ifndef IPCAMERA_H
#define IPCAMERA_H

#include "flycapture/FlyCapture2.h"
#include <opencv2/opencv.hpp>
#include <thread>
#include <chrono>
#include <mutex>

namespace Politocean {

using namespace FlyCapture2;

class IpCamera
{
public:
    IpCamera();
    ~IpCamera();
    cv::Mat getFrame();
    Camera *camera;
  //  cv::VideoCapture webcam;
    CameraInfo camInfo;
    void reconnect();

private:
    static void callback(FlyCapture2::Image *raw, const void *pCallbackData);
    static bool updated;
    static int counterFrame;
    static cv::Mat frame;

    bool ipcamera_active = false, reconnecting = false;
    std::thread *reconnectionThd;

};

}

#endif // IPCAMERA_H
