#include "ipcamera.h"

using namespace FlyCapture2;

namespace Politocean {

IpCamera::IpCamera()
{
    //Connect the camera
    FlyCapture2::Error error = camera.Connect( 0 );
    if(error != PGRERROR_OK){
        std::cout << "Impossibile accedere all'IpCamera" << std::endl;
        ipcamera_active = false;
        webcam.open(0);
    }
    else{
        // Get the camera info and print it out
        camera.GetCameraInfo( &camInfo );
        std::cout << camInfo.vendorName << " "
                  << camInfo.modelName << " "
                  << camInfo.serialNumber << std::endl;

        camera.StartCapture();
        ipcamera_active = true;
    }
}

IpCamera::~IpCamera()
{
    camera.StopCapture();
    camera.Disconnect();

}

cv::Mat IpCamera::getFrame()
{
    Image raw;
    cv::Mat img;
    if(ipcamera_active){
        FlyCapture2::Error error = camera.RetrieveBuffer(&raw);
        if (error != PGRERROR_OK){
                //std::cout << "network loss frame" << std::endl;
        }
        else{
            Image rgb;
            raw.Convert( FlyCapture2::PIXEL_FORMAT_BGR, &rgb );

            unsigned int row = (double)rgb.GetReceivedDataSize()/(double)rgb.GetRows();


            img = cv::Mat(rgb.GetRows(), rgb.GetCols(), CV_8UC3, rgb.GetData(),row);
            //cv::imshow("test",img);
        }
    }
    else{
        webcam.read(img);
    }
    return img;
}

}
