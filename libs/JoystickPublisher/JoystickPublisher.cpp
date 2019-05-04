//
// Created by pettinz.
//

#include "JoystickPublisher.h"
#include "logger.h"



namespace Politocean {

using namespace std;

const string JoystickPublisher::DFLT_ADDRESS    { "tcp://localhost:1883" };
const string JoystickPublisher::DFLT_CLIENT_ID  { "JoystickPublisher" };

void JoystickPublisher::updateValues(const vector<int>& axes, unsigned char button)
{
    axes_ = axes;
    button_ = button;
}

thread *JoystickPublisher::publishAxes()
{
    if (isPublishingAxes_)
        return nullptr;
    
    isPublishingAxes_ = true;

    return new thread([this] {
        logger::log(logger::DEBUG, "Starting publishing axes...");
        while (isPublishing())
        {
            nlohmann::json j_map({ {"axes", axes_} });

            publish(Constants::Topics::JOYSTICK_AXES, j_map.dump());

            this_thread::sleep_for(chrono::milliseconds(50));
        }
        isPublishingAxes_ = false;
    });
}

thread *JoystickPublisher::publishButtons()
{
    if (isPublishingButtons_)
        return nullptr;
    
    isPublishingButtons_ = true;

    return new thread([this]() {
        unsigned char lastButton = -1;

        logger::log(logger::DEBUG, "Starting publishing buttons...");
        while (isPublishing())
        {
            if (button_ == lastButton)
                continue;

            lastButton = button_;
            publish(Constants::Topics::JOYSTICK_BUTTONS, to_string(button_));
        }
        isPublishingButtons_ = false;
    });
}

void JoystickPublisher::startPublishing()
{
    if (isPublishing_ || !(this->is_connected()))
        return ;

    isPublishing_ = true;

    publishAxes_    = publishAxes();
    publishButtons_ = publishButtons();
}

void JoystickPublisher::stopPublishing()
{
    if (!isPublishing_)
        return;

    isPublishing_           = false;
    isPublishingAxes_       = false;
    isPublishingButtons_    = false;

    publishAxes_->join();
    publishButtons_->join();
}

bool JoystickPublisher::isPublishing(){
    return isPublishing_ && this->is_connected();
}

}