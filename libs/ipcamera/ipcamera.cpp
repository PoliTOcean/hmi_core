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

void IpCamera::reconnect()
{
    camera.StopCapture();
    camera.Disconnect();
    ipcamera_active = false;
    reconnecting = false;

    FlyCapture2::Error error = camera.Connect( 0 );
    if(error != PGRERROR_OK){
        std::cout << "Impossibile accedere all'IpCamera" << std::endl;
        return;
    }
    camera.GetCameraInfo( &camInfo );
    std::cout << camInfo.vendorName << " "
              << camInfo.modelName << " "
              << camInfo.serialNumber << std::endl;

    camera.StartCapture();

    ipcamera_active = true;
}

cv::Mat IpCamera::getFrame()
{
    Image raw;
    cv::Mat img;

    if(ipcamera_active){
        FlyCapture2::Error error = camera.RetrieveBuffer(&raw);
        if (error != PGRERROR_OK){
            if (error == PGRERROR_FAILED || error == PGRERROR_NOT_CONNECTED || error == PGRERROR_NOT_FOUND)
            {
                std::cout << "Camera error, try to reconnect!" <<std::endl;
                ipcamera_active = false;
                reconnecting = true;
                reconnectionThd = new std::thread([&](){
                    while (reconnecting)
                    {
                        FlyCapture2::Error error = camera.Connect( 0 );
                        if (error == PGRERROR_OK)
                        {
                            reconnecting = false;
                            ipcamera_active = true;
                        }
                        std::this_thread::sleep_for(std::chrono::milliseconds(500));
                    }
                });
            }
        }
        else{
            Image rgb;
            raw.SetDimensions(768, 1024, raw.GetStride(), FlyCapture2::PIXEL_FORMAT_BGR, raw.GetBayerTileFormat());
            raw.Convert( FlyCapture2::PIXEL_FORMAT_BGR, &rgb );
            raw.ReleaseBuffer();

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
