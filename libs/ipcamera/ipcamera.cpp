#include "ipcamera.h"
#include <chrono>         // std::chrono::milliseconds
#include <mqttLogger.h>
#include <string>
#include <sstream>

#define DEFAULT_FREQ_DIVIDER 1


using namespace FlyCapture2;

namespace Politocean {

std::function<void(cv::Mat)> IpCamera::extCallback;
bool IpCamera::updated = false;
int IpCamera::counterFrame = 0;
int IpCamera::freqDivider = DEFAULT_FREQ_DIVIDER;

IpCamera::IpCamera(std::function<void(cv::Mat)> extCb) : IpCamera(extCb, DEFAULT_FREQ_DIVIDER) {}

IpCamera::IpCamera(std::function<void(cv::Mat)> extCb, int freqDivider)
    :   camera(nullptr), reconnecting(false), active(true), started(false),
        monitor([&]() {
            while (active) {
                reconnect()->join();
                if (started) camera->StartCapture( callback, this );

                while (!updated) std::this_thread::sleep_for(std::chrono::milliseconds(50));

                while (updated){
                    updated = false;
                    std::this_thread::sleep_for(std::chrono::milliseconds(500));
                }
            }
        })
{
    IpCamera::freqDivider = freqDivider;
    IpCamera::extCallback = extCb;
}

void IpCamera::callback(FlyCapture2::Image *raw, const void *pCallbackData) {
    IpCamera::updated = true;
    IpCamera::counterFrame++;
    if (IpCamera::counterFrame < IpCamera::freqDivider)
        return;
    IpCamera::counterFrame = 0;

    Image rgb;
    raw->Convert( FlyCapture2::PIXEL_FORMAT_BGR, &rgb );

    unsigned int row = (double)rgb.GetReceivedDataSize()/(double)rgb.GetRows();

    //cv::imshow("test", cv::Mat(rgb.GetRows(), rgb.GetCols(), CV_8UC3, rgb.GetData(), row));
    IpCamera::extCallback(cv::Mat(rgb.GetRows(), rgb.GetCols(), CV_8UC3, rgb.GetData(), row));
}

IpCamera::~IpCamera()
{
    active = false;
    monitor.join();
    camera->StopCapture();
    camera->Disconnect();
}

void IpCamera::stop() {
    started = false;
    if (camera != nullptr)
        camera->StopCapture();
}

void IpCamera::start() {
    started = true;
    if (camera != nullptr)
        camera->StartCapture( callback, this );
}

std::thread* IpCamera::reconnect()
{
    if (reconnecting) return new std::thread();

    updated = false;
    reconnecting = true;
    mqttLogger::getInstance(LIB_TAG).log(logger::CONFIG, "Scanning for the GigE Camera...");

    return new std::thread(
        [&]() {
            if (camera != nullptr) {
                camera->StopCapture();
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
                }
                else {
                    mqttLogger::getInstance(LIB_TAG).log(logger::WARNING, "Could not connect to the GigE Camera: ");
                    mqttLogger::getInstance(LIB_TAG).log(logger::INFO, std::to_string(error.GetType()) + " " + error.GetDescription());
                    mqttLogger::getInstance(LIB_TAG).log(logger::WARNING, "Retrying...");
                    delete camera;
                }
            }
            camera->GetCameraInfo( &camInfo );

            std::stringstream ss;
            ss  << camInfo.vendorName << " " << camInfo.modelName << " " << camInfo.serialNumber << std::endl
                << camInfo.sensorResolution << " " << camInfo.sensorInfo;
            mqttLogger::getInstance(LIB_TAG).log(logger::CONFIG, ss.str()); 
        }
    );
}

}
