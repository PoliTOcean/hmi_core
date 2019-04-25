//
// Created by pettinz.
//

#include "JoystickPublisher.h"

#include "json.hpp"

using namespace std;

const string JoystickPublisher::DFLT_ADDRESS    { "tcp://localhost:1883" };
const string JoystickPublisher::DFLT_CLIENT_ID  { "JoystickPublisher" };
const string JoystickPublisher::DFLT_TOPIC      { "JoystickTopic" };

void JoystickPublisher::callback(std::map<int,int> axes, std::map<int,int>buttons) {
    map<string, map<int,int>> c_map { {"axes", axes}, {"buttons", buttons} };
    nlohmann::json j_map(c_map);

    this->publish(DFLT_TOPIC, j_map.dump());
}
