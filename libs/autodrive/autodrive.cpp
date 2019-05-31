#include "autodrive.h"
#define K 10000

namespace Politocean {

using namespace Politocean::Constants;

AutoDrive::AutoDrive() : AutoDrive(Direction::UP)
{
}

AutoDrive::AutoDrive(Direction startDirection) : publisher(MqttClient::getInstance(Hmi::AUTODRIVE_ID, Hmi::IP_ADDRESS))
{
    direction = startDirection;
    grid = imread("images/grid.png",CV_LOAD_IMAGE_COLOR);
    currentPos.x = 50;
    currentPos.y = 50;
    if(!grid.data )                              // Check for invalid input
    {
        //TO DO: use logger: std::cout <<  "Could not open or find the image" << std::endl ;
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
    if(direction == Direction::DOWN || direction == Direction::UP){

        if(Vision::checkLeft(frame)){
            direction = Direction::LEFT;

            /* SENDING MQTT TOPIC */
            std::string out_string;
            std::stringstream ss;
            ss << "{\"X\":"<< -K << ", \"Y\":0, \"MOTORS_ON\":0, \"MOTOROS_OFF\":0}";
            out_string = ss.str();

            publisher.publish(Topics::AUTODRIVE,out_string);
            // TO DO: mqttLogger: publisher.publish(MESSAGE_TOPIC,"AUTODRIVE: direction left");
            /** TO DO: implement JSON **/

            currentPos.x = currentPos.x - 100;
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