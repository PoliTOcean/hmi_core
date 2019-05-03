#include "ipcamera.h"

using namespace FlyCapture2;

namespace Politocean {

IpCamera::IpCamera()
{
    //Connect the camera
    camera.Connect( 0 );

    // Get the camera info and print it out
    camera.GetCameraInfo( &camInfo );
    std::cout << camInfo.vendorName << " "
              << camInfo.modelName << " "
              << camInfo.serialNumber << std::endl;

    camera.StartCapture();
}

IpCamera::~IpCamera()
{
    camera.StopCapture();
    camera.Disconnect();

}

Mat IpCamera::getFrame()
{
    Image raw;
    camera.RetrieveBuffer(&raw);

    printf("Frames lost: \n");
    Image rgb;
    raw.Convert( FlyCapture2::PIXEL_FORMAT_RGB, &rgb );

    unsigned int row = (double)rgb.GetReceivedDataSize()/(double)rgb.GetRows();

    return cv::Mat(rgb.GetRows(), rgb.GetCols(), CV_8UC3, rgb.GetData(),row);
}

}
