//
// Created by pettinz.
//

#include "JoystickPublisher.h"



namespace Politocean {

using namespace std;

const string JoystickPublisher::DFLT_ADDRESS    { "tcp://localhost:1883" };
const string JoystickPublisher::DFLT_CLIENT_ID  { "JoystickPublisher" };
const string JoystickPublisher::DFLT_TOPIC      { "JoystickTopic" };

const string JoystickPublisher::DFLT_TOPIC_AXIS     { JoystickPublisher::DFLT_TOPIC+"Axis" };
const string JoystickPublisher::DFLT_TOPIC_BUTTON   { JoystickPublisher::DFLT_TOPIC+"Button" };

void JoystickPublisher::callback(const vector<int>& axes, unsigned char button)
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
        while (isPublishing_)
        {
            nlohmann::json j_map({ {"axes", axes_} });

            publish(DFLT_TOPIC_AXIS, j_map.dump());

            this_thread::sleep_for(chrono::milliseconds(50));
        }
    });
}

thread *JoystickPublisher::publishButtons()
{
    if (isPublishingButtons_)
        return nullptr;
    
    isPublishingButtons_ = true;

    return new thread([this]() {
        unsigned char lastButton = -1;

        while (isPublishing_)
        {
            if (button_ == lastButton)
                continue;

            lastButton = button_;
            publish(DFLT_TOPIC_BUTTON, to_string(button_));
        }
    });
}

void JoystickPublisher::startPublishing()
{
    if (isPublishing_)
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

}