#ifndef IPCAMERA_H
#define IPCAMERA_H

#include "flycapture/FlyCapture2.h"
#include <opencv2/opencv.hpp>
#include <thread>
#include <functional>


namespace Politocean {

using namespace FlyCapture2;

class IpCamera
{
public:
    IpCamera(std::function<void(cv::Mat)> extCb);
    ~IpCamera();
    Camera *camera;
  //  cv::VideoCapture webcam;
    CameraInfo camInfo;
    std::thread* reconnect();

private:
    static void callback(FlyCapture2::Image *raw, const void *pCallbackData);

    static bool updated, updated_monitor;
    static int counterFrame;
    static std::function<void(cv::Mat)> extCallback;
    bool reconnecting, active;
    std::thread monitor;
};

}

#endif // IPCAMERA_H
