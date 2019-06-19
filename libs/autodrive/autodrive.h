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
    const std::string LIB_TAG = "AutoDrive";

public:
    AutoDrive();
    AutoDrive(Politocean::Direction startDirection);
    Politocean::Direction updateDirection(Mat frame);
    Mat getGrid();
    void reset();
    void setBluePosition();


private:
    int blue_pos;
    Point currentPos;
    Politocean::Direction direction;
    std::list<Politocean::Direction> path;
    Mat grid,grid_return;

};


}

#endif // AUTODRIVE_H
