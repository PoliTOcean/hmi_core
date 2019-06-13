#include "autodrive.h"
#define K 10000

const std::string LIB_TAG = "Autodrive: ";

namespace Politocean {

using namespace Politocean::Constants;

AutoDrive::AutoDrive() : AutoDrive(Direction::UP)
{
}

AutoDrive::AutoDrive(Direction startDirection) : direction(startDirection)
{
    grid = imread("images/grid.png",CV_LOAD_IMAGE_COLOR);
    currentPos.x = 50;
    currentPos.y = 50;
    if(!grid.data )                              // Check for invalid input
    {
        logger::getInstance().log(logger::WARNING, LIB_TAG + "Could not open or find the image");
    }
}

Mat AutoDrive::getGrid(){
    Mat final_grid;
    grid.copyTo(final_grid);
    circle(final_grid,currentPos,30,Scalar(255,0,0),4,LINE_8);
    return final_grid;
}

Direction AutoDrive::updateDirection(Mat frame)
{
    MqttClient& publisher = MqttClient::getInstance(Hmi::AUTODRIVE_ID, Hmi::IP_ADDRESS);

    if(direction == Direction::DOWN || direction == Direction::UP){

        if(Vision::checkLeft(frame)){
            direction = Direction::LEFT;

            /* SENDING MQTT TOPIC */
            std::string out_string;
            std::stringstream ss;
            ss << "{\"X\":"<< -K << ", \"Y\":0, \"MOTORS_ON\":0, \"MOTOROS_OFF\":0}";
            out_string = ss.str();

            publisher.publish(Topics::AUTODRIVE, out_string);
            mqttLogger::getInstance(publisher).log(logger::DEBUG, string(LIB_TAG)+" direction left");
            // TODO implement JSON / Reflectable

            currentPos.x = currentPos.x - 100;
            path.push_back(Direction::LEFT);
        }
        else if(Vision::checkRight(frame)){
            direction = Direction::RIGHT;

            /* SENDING MQTT TOPIC */
            std::string out_string;
            std::stringstream ss;
            ss << "{\"X\":0, \"Y\":0, \"MOTORS_ON\":"<< 1 <<", \"MOTOROS_OFF\":0}";
            out_string = ss.str();

            publisher.publish(Topics::AUTODRIVE,out_string);
            // TO DO: mqttLogger: publisher.publish(MESSAGE_TOPIC,"AUTODRIVE: direction right");
            /** TO DO: implement JSON **/

            currentPos.x = currentPos.x + 100;
            path.push_back(Direction::RIGHT);
        }
    }

    else if(direction == Direction::LEFT || direction == Direction::RIGHT){

        if(Vision::checkTop(frame)){
            direction = Direction::UP;

            /* SENDING MQTT TOPIC */
            std::string out_string;
            std::stringstream ss;
            ss << "{\"X\":0, \"Y\":0, \"MOTORS_ON\":0, \"MOTOROS_OFF\":"<< 1 <<"}";
            out_string = ss.str();

            publisher.publish(Topics::AUTODRIVE,out_string);
            // TODO: mqttLogger: publisher.publish(MESSAGE_TOPIC,"AUTODRIVE: direction up");
            /** TO DO: implement JSON **/

            currentPos.y = currentPos.y - 100;
            path.push_back(Direction::UP);
        }
        else if(Vision::checkBottom(frame)){
            direction = Direction::DOWN;

            /* SENDING MQTT TOPIC */
            std::string out_string;
            std::stringstream ss;
            ss << "{\"X\":0, \"Y\":0, \"MOTORS_ON\":"<< 1 <<", \"MOTOROS_OFF\":0}";
            out_string = ss.str();

            publisher.publish(Topics::AUTODRIVE,out_string);
            // TODO: mqttLogger: publisher.publish(MESSAGE_TOPIC,"AUTODRIVE: direction down");
            /** TO DO: implement JSON **/

            currentPos.y = currentPos.y + 100;

        }
    }

    return direction;
}

void AutoDrive::reset()
{
    direction = Direction::UP;
    currentPos.x = 50;
    currentPos.y = 50;
}

}
