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
    IpCamera(std::function<void(cv::Mat)> extCb, int freqDivider);
    ~IpCamera();

    Camera *camera;
    CameraInfo camInfo;
    std::thread* reconnect();
    void stop();
    void start();

private:
    static void callback(FlyCapture2::Image *raw, const void *pCallbackData);

    static bool updated, updated_monitor;
    static int counterFrame, freqDivider;
    static std::function<void(cv::Mat)> extCallback;
    bool reconnecting, active, started;
    std::thread monitor;
};

}

#endif // IPCAMERA_H
