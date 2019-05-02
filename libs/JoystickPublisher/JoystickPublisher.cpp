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

void JoystickPublisher::callback(const std::string& topic, const vector<int>& axes, unsigned char button) {
    nlohmann::json j_map({ {"axes", axes}, {"button", button} });

    this->publish(topic, j_map.dump());
}

}