//
// Created by pettinz.
//

#include "JoystickPublisher.h"



namespace Politocean {

using namespace std;

const string JoystickPublisher::DFLT_ADDRESS    { "tcp://localhost:1883" };
const string JoystickPublisher::DFLT_CLIENT_ID  { "JoystickPublisher" };
const string JoystickPublisher::DFLT_TOPIC      { "JoystickTopic" };

void JoystickPublisher::callback(vector<int>& axes, vector<int>& buttons) {
    map<string, vector<int>> c_map { {"axes", axes}, {"buttons", buttons} };

    nlohmann::json j_map(c_map);

    this->publish(DFLT_TOPIC, j_map.dump());
}

}