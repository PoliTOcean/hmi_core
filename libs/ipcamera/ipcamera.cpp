#include "ipcamera.h"
#include <chrono>         // std::chrono::milliseconds

using namespace FlyCapture2;

namespace Politocean {

std::function<void(cv::Mat)> IpCamera::extCallback;
bool IpCamera::updated = false;
int IpCamera::counterFrame = 0;

IpCamera::IpCamera(std::function<void(cv::Mat)> extCb) 
    :   camera(nullptr), reconnecting(false), active(true),
        monitor([&]() {
            while (active) {
                reconnect()->join();

                while (!updated) std::this_thread::sleep_for(std::chrono::milliseconds(50));

                while (updated){
                    updated = false;
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }
            }
        })
{
    extCallback = extCb;
}

void IpCamera::callback(FlyCapture2::Image *raw, const void *pCallbackData) {
    updated = true;
  /*  counterFrame++;
    if (counterFrame < 2)
        return;
    counterFrame = 0;*/

    Image rgb;
    raw->Convert( FlyCapture2::PIXEL_FORMAT_BGR, &rgb );

    unsigned int row = (double)rgb.GetReceivedDataSize()/(double)rgb.GetRows();

    extCallback(cv::Mat(rgb.GetRows(), rgb.GetCols(), CV_8UC3, rgb.GetData(), row));
}

IpCamera::~IpCamera()
{
    active = false;
    monitor.join();
    camera->StopCapture();
    camera->Disconnect();
}

std::thread* IpCamera::reconnect()
{
    if (reconnecting) return new std::thread();

    updated = false;
    reconnecting = true;
    std::cout << "Scanning for the GigE Camera...\n";

    return new std::thread(
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

}
