#include "autodrive.h"
#define K 10000


namespace Politocean {

using namespace Politocean::Constants;

AutoDrive::AutoDrive() : AutoDrive(Politocean::Direction::UP)
{
}

AutoDrive::AutoDrive(Politocean::Direction startDirection) : direction(startDirection)
{
    grid = imread("images/grid.png",CV_LOAD_IMAGE_COLOR);
    currentPos.x = 50;
    currentPos.y = 50;
    if(!grid.data )                              // Check for invalid input
    {
        mqttLogger::getInstance(LIB_TAG).log(logger::WARNING, "Could not open or find the image");
    }
}

Mat AutoDrive::getGrid(){
    Mat final_grid;

    if(path.size() == 4){
        // return design 3
        final_grid = imread("images/grid_3.png");
        rectangle(final_grid,Point(blue_pos* (830/4),720/4),Point(830/6,720/6),
                  Scalar(0,0,255));
        return final_grid;
    }

    if(path.size() == 5){
        // return design 5
        final_grid = imread("images/grid_5.png");
        rectangle(final_grid,Point(blue_pos* (830/4),720/4),Point(830/6,720/6),
                  Scalar(0,0,255));
        return final_grid;
    }

    if(path.size() == 7){
        // return design 6
        final_grid = imread("images/grid_7.png");
        rectangle(final_grid,Point(blue_pos* (830/4),720/4),Point(830/6,720/6),
                Scalar(0,0,255));
        return final_grid;
    }

    if(path.size() == 6){
        path.pop_back();

        if(path.back() == Politocean::Direction::UP){

            // return design 2
            final_grid = imread("images/grid_2.png");
            rectangle(final_grid,Point(blue_pos* (830/4),720/4),Point(830/6,720/6),
                      Scalar(0,0,255));
            return final_grid;
        }
        else {
            // return design 1
            final_grid = imread("images/grid_1.png");
            rectangle(final_grid,Point(blue_pos* (830/4),720/4),Point(830/6,720/6),
                      Scalar(0,0,255));
            return final_grid;
        }
    }

    if(path.size() == 8){
        // return design 4
        final_grid = imread("images/grid_4.png");
        rectangle(final_grid,Point(blue_pos* (830/4),720/4),Point(830/6,720/6),
                  Scalar(0,0,255));
        return final_grid;
    }

    //AN ERROR OCCURRED
    final_grid = imread("images/grid_3.png");
    rectangle(final_grid,Point(blue_pos* (830/4),720/4),Point(830/6,720/6),
              Scalar(0,0,255));
    return final_grid;
}

Politocean::Direction AutoDrive::updateDirection(Mat frame)
{
    MqttClient& publisher = MqttClient::getInstance(Hmi::AUTODRIVE_ID, Hmi::IP_ADDRESS);

    if(direction == Politocean::Direction::DOWN || direction == Politocean::Direction::UP){

        if(Vision::checkLeft(frame)){
            direction = Politocean::Direction::LEFT;

            /* SENDING MQTT TOPIC */
            std::string out_string;
            std::stringstream ss;
            ss << "{\"X\":"<< -K << ", \"Y\":0, \"MOTORS_ON\":0, \"MOTOROS_OFF\":0}";
            out_string = ss.str();

            publisher.publish(Topics::AUTODRIVE, out_string);
            mqttLogger::getInstance(LIB_TAG).log(logger::DEBUG, " direction left");

            currentPos.x = currentPos.x - 100;
            path.push_back(Politocean::Direction::LEFT);
        }
        else if(Vision::checkRight(frame)){
            direction = Politocean::Direction::RIGHT;

            /* SENDING MQTT TOPIC */
            std::string out_string;
            std::stringstream ss;
            ss << "{\"X\":0, \"Y\":0, \"MOTORS_ON\":"<< 1 <<", \"MOTOROS_OFF\":0}";
            out_string = ss.str();

            publisher.publish(Topics::AUTODRIVE,out_string);
            // TO DO: mqttLogger: publisher.publish(MESSAGE_TOPIC,"AUTODRIVE: direction right");
            /** TO DO: implement JSON **/

            currentPos.x = currentPos.x + 100;
            path.push_back(Politocean::Direction::RIGHT);
        }
    }

    else if(direction == Politocean::Direction::LEFT || direction == Politocean::Direction::RIGHT){

        if(Vision::checkTop(frame)){
            direction = Politocean::Direction::UP;

            /* SENDING MQTT TOPIC */
            std::string out_string;
            std::stringstream ss;
            ss << "{\"X\":0, \"Y\":0, \"MOTORS_ON\":0, \"MOTOROS_OFF\":"<< 1 <<"}";
            out_string = ss.str();

            publisher.publish(Topics::AUTODRIVE,out_string);
            // TODO: mqttLogger: publisher.publish(MESSAGE_TOPIC,"AUTODRIVE: direction up");
            /** TO DO: implement JSON **/

            currentPos.y = currentPos.y - 100;
            path.push_back(Politocean::Direction::UP);
        }
        else if(Vision::checkBottom(frame)){
            direction = Politocean::Direction::DOWN;

            /* SENDING MQTT TOPIC */
            std::string out_string;
            std::stringstream ss;
            ss << "{\"X\":0, \"Y\":0, \"MOTORS_ON\":"<< 1 <<", \"MOTOROS_OFF\":0}";
            out_string = ss.str();

            publisher.publish(Topics::AUTODRIVE,out_string);
            // TODO: mqttLogger: publisher.publish(MESSAGE_TOPIC,"AUTODRIVE: direction down");
            /** TO DO: implement JSON **/

            currentPos.y = currentPos.y + 100;
            path.push_back(Politocean::Direction::DOWN);
        }
    }

    return direction;
}

void AutoDrive::setBluePosition()
{
    blue_pos = path.size();
}

void AutoDrive::reset()
{
    direction = Politocean::Direction::UP;
    currentPos.x = 50;
    currentPos.y = 50;
}

}
