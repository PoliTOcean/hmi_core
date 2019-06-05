#ifndef IPCAMERA_H
#define IPCAMERA_H

#include "flycapture/FlyCapture2.h"
#include <opencv2/opencv.hpp>
#include <thread>

namespace Politocean {

using namespace FlyCapture2;

class IpCamera
{
public:
    IpCamera();
    ~IpCamera();
    cv::Mat getFrame();
    Camera camera;
    cv::VideoCapture webcam;
    CameraInfo camInfo;
    void reconnect();

private:
    bool ipcamera_active = false, reconnecting = false;
    std::thread *reconnectionThd;

};

}

#endif // IPCAMERA_H
