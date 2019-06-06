#include "ipcamera.h"

using namespace FlyCapture2;

namespace Politocean {

IpCamera::IpCamera()
{
    //Connect the camera
    FlyCapture2::Error error = camera.Connect( 0 );
    if(error != PGRERROR_OK){
        std::cout << "FlyCapture::ErrorType::" << error.GetType() << " " << error.GetDescription() << std::endl;
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
    reconnecting = true;
    std::cout << "Reconnecting\n";

    reconnectionThd = new std::thread(
        [&](){
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
            camera.GetCameraInfo( &camInfo );
            std::cout << camInfo.vendorName << " "
                    << camInfo.modelName << " "
                    << camInfo.serialNumber << std::endl;

            camera.StartCapture();
        });
}

cv::Mat IpCamera::getFrame()
{
    Image raw;
    cv::Mat img;

    if(ipcamera_active){
        //raw.ReleaseBuffer();
        FlyCapture2::Error error = camera.RetrieveBuffer(&raw);
        
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
