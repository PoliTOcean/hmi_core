#ifndef AUTODRIVE_H
#define AUTODRIVE_H

#include <vision.h>
#include <opencv2/opencv.hpp>
#include <PolitoceanConstants.h>
#include <MqttClient.h>
#include <logger.h>
#include <mqttLogger.h>

#define MAX_PATH 12

namespace Politocean {

enum class Direction{
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class AutoDrive
{
    static const std::string LIB_TAG;

public:
    AutoDrive();
    AutoDrive(Direction startDirection);
    Direction updateDirection(Mat frame);
    Mat getGrid();
    void reset();


private:
    Point currentPos;
    Direction direction;
    Direction path[MAX_PATH];
    Mat grid;

};

const std::string AutoDrive::LIB_TAG = "Autodrive";

}

#endif // AUTODRIVE_H
