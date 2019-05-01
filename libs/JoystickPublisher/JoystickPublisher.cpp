//
// Created by pettinz.
//

#include "JoystickPublisher.h"



namespace Politocean {

using namespace std;

const string JoystickPublisher::DFLT_ADDRESS    { "tcp://localhost:1883" };
const string JoystickPublisher::DFLT_CLIENT_ID  { "JoystickPublisher" };
const string JoystickPublisher::DFLT_TOPIC      { "JoystickTopic" };

void JoystickPublisher::callback(vector<int>& axes, unsigned char button) {
    nlohmann::json j_map({ {"axes", axes}, {"button", button} });

    this->publish(DFLT_TOPIC, j_map.dump());
}

}