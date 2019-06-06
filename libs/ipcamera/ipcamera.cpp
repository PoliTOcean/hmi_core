#include "ipcamera.h"
#include <future>         // std::async, std::future
#include <chrono>         // std::chrono::milliseconds
#include <functional>

using namespace FlyCapture2;

namespace Politocean {

IpCamera::IpCamera()
{
    //Connect the camera
    /*FlyCapture2::Error error = camera->Connect( 0 );
    if(error != PGRERROR_OK){
        std::cout << "FlyCapture::ErrorType::" << error.GetType() << " " << error.GetDescription() << std::endl;
        std::cout << "Impossibile accedere all'IpCamera" << std::endl;
        ipcamera_active = false;
        webcam.open(0);
    }
    else{
        // Get the camera info and print it out
        camera->GetCameraInfo( &camInfo );
        std::cout << camInfo.vendorName << " "
                  << camInfo.modelName << " "
                  << camInfo.serialNumber << std::endl;

        camera->StartCapture();
        ipcamera_active = true;
    }*/
    camera = nullptr;
    reconnect();
}

IpCamera::~IpCamera()
{
    camera->StopCapture();
    camera->Disconnect();

}

void IpCamera::reconnect()
{
    if (camera!=nullptr && camera->IsConnected()) return;

    ipcamera_active = false;
    reconnecting = true;
    std::cout << "Reconnecting\n";

    if (camera != nullptr) delete camera;
    camera = new Camera();

    while (reconnecting)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        FlyCapture2::Error error = camera->Connect( 0 );
        if (error == PGRERROR_OK)
        {
            reconnecting = false;
            ipcamera_active = true;
            camera->StartCapture();
        }
        else
            std::cout << error.GetType() << " " << error.GetDescription() << std::endl;
    }
    camera->GetCameraInfo( &camInfo );
    std::cout << camInfo.vendorName << " "
            << camInfo.modelName << " "
            << camInfo.serialNumber << std::endl
            << camInfo.sensorResolution << " " << camInfo.sensorInfo << std::endl; 
}

cv::Mat IpCamera::getFrame()
{
    Image raw;
    cv::Mat img;

    if(ipcamera_active){
        raw.ReleaseBuffer();

        // call function asynchronously:
        std::future<FlyCapture2::Error> fut = std::async ( std::bind(&FlyCapture2::Camera::RetrieveBuffer, camera, std::placeholders::_1), &raw); 
        if (fut.wait_for(std::chrono::milliseconds(500))==std::future_status::timeout)
        {
            camera->StopCapture();
            camera->Disconnect();
            reconnect();
            return img;
        }

        FlyCapture2::Error error = fut.get();
        if (error != PGRERROR_OK){
            std::cout << "FlyCapture::ErrorType::" << error.GetType() << " " << error.GetDescription() << std::endl;
         /*   if (error != PGRERROR_BUFFER_TOO_SMALL)
            {
                reconnect();                
            }*/
        }
        else{
            Image rgb;
            raw.Convert( FlyCapture2::PIXEL_FORMAT_BGR, &rgb );

            unsigned int row = (double)rgb.GetReceivedDataSize()/(double)rgb.GetRows();


            img = cv::Mat(rgb.GetRows(), rgb.GetCols(), CV_8UC3, rgb.GetData(),row);
            //cv::imshow("test",img);
        }
    }
    else if (!reconnecting){
        webcam.read(img);
    }
    return img;
}

}
