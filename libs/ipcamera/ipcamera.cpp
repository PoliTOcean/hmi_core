#include "ipcamera.h"
#include <future>         // std::async, std::future
#include <chrono>         // std::chrono::milliseconds
#include <functional>

using namespace FlyCapture2;

namespace Politocean {

cv::Mat IpCamera::frame;
bool IpCamera::updated = false;
int IpCamera::counterFrame = 0;
std::mutex IpCamera::mtx;

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

void IpCamera::callback(FlyCapture2::Image *raw, const void *pCallbackData) {
    updated = true;
    counterFrame++;
    if (counterFrame < 2)
        return;
    counterFrame = 0;

    Image rgb;
    raw->Convert( FlyCapture2::PIXEL_FORMAT_BGR, &rgb );

    unsigned int row = (double)rgb.GetReceivedDataSize()/(double)rgb.GetRows();

    std::lock_guard<std::mutex> lck (IpCamera::mtx);
    IpCamera::frame = cv::Mat(rgb.GetRows(), rgb.GetCols(), CV_8UC3, rgb.GetData(),row);
}

IpCamera::~IpCamera()
{
    camera->StopCapture();
    camera->Disconnect();
}

void IpCamera::reconnect()
{
    if (camera!=nullptr && camera->IsConnected()) return;

    updated = false;
    ipcamera_active = false;
    reconnecting = true;
    std::cout << "Searching for the GigE Camera...\n";

    reconnectionThd = new std::thread(
        [&]() {
            if (camera != nullptr) {
                delete camera;
            }

            while (reconnecting)
            {
                camera = new Camera();
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                FlyCapture2::Error error = camera->Connect( 0 );
                if (error == PGRERROR_OK)
                {
                    reconnecting = false;
                    ipcamera_active = true;
                    camera->StartCapture( callback, this );
                }
                else {
                    std::cout << error.GetType() << " " << error.GetDescription() << std::endl;
                    delete camera;
                }
            }
            camera->GetCameraInfo( &camInfo );
            std::cout << camInfo.vendorName << " "
                    << camInfo.modelName << " "
                    << camInfo.serialNumber << std::endl
                    << camInfo.sensorResolution << " " << camInfo.sensorInfo << std::endl; 
        }
    );
}

cv::Mat IpCamera::getFrame()
{
    if (!updated && !reconnecting)
    {
        camera->StopCapture();
        camera->Disconnect();
        reconnect();
    }
    updated = false;
    
    std::lock_guard<std::mutex> lck (IpCamera::mtx);
    return frame;
}
/*
    Image raw;
    cv::Mat img;
    if (!ipcamera_active) return img;

    camera->StartCapture();
    camera->SetCallback()
    

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
        return img;
    }
    

    //cv::imshow("test",img);

    return img;
}
*/
}
