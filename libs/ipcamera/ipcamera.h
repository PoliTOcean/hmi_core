#ifndef IPCAMERA_H
#define IPCAMERA_H

#include "flycapture/FlyCapture2.h"
#include <opencv2/opencv.hpp>

namespace Politocean {

using namespace FlyCapture2;
using namespace cv;
class IpCamera
{
public:
    IpCamera();
    ~IpCamera();
    Mat getFrame();
    Camera camera;
    CameraInfo camInfo;
};

}

#endif // IPCAMERA_H
