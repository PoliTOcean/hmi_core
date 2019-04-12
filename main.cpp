//
// Created by pettinz.
//

#include <iostream>
#include <map>

#include "JoystickPublisher.h"
#include "Joystick.h"

#include <Publisher.h>
#include <Subscriber.h>
#include <json.hpp>

#define DFLT_ADDRESS "tcp://localhost:1883"
#define DFLT_CLIENT_ID "JoystickPublisher"
#define TOPIC "JoystickTopic"

#define JS_DEV "/dev/input/js0"

using namespace std;

JoystickPublisher joystickPublisher(DFLT_ADDRESS, DFLT_CLIENT_ID);

void callback(map<int,int> axes, map<int,int>buttons)
{
    map<string, map<int,int>> c_map { {"axes", axes}, {"buttons", buttons} };
    nlohmann::json j_map(c_map);

    joystickPublisher.sendMessage(TOPIC, j_map.dump());
}

int main(int argc, const char *argv[])
{
    Joystick *joystick;
    
    try {
        joystick = new Joystick(JS_DEV);

        joystickPublisher.connect();
        joystick->startListening(callback)->join();
        joystickPublisher.disconnect();
    } catch (Joystick::JoystickException& e) {
        cerr << e.what() << endl;
    } catch (mqtt::exception& e) {
        cerr << e.what() << endl;
    }

    return 0;
}
