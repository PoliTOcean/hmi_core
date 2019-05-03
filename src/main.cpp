#include <iostream>
#include <unistd.h>
#include <json.hpp>
#include <string>

#include "Joystick.h"
#include "Publisher.h"
#include "Subscriber.h"
#include "JoystickPublisher.h"

#include "PolitoceanConstants.h"
#include "PolitoceanExceptions.hpp"
#include "mqttLogger.h"

#define MAX_TRIES 10

using namespace std;
using namespace Politocean;
using namespace Politocean::Constants;
using json = nlohmann::json;


void axis_callback(const string& payload);

void button_callback(const string& payload);

string map_button(int button);

Publisher publisher("127.0.0.1", Hmi::CMD_ID_PUB);

Subscriber subscriber("127.0.0.1",Hmi::CMD_ID_SUB);

int main(int argc, char* argv[])
{
    bool connected = false;

    try {
        int n_tries=0;
        while(!connected && n_tries < MAX_TRIES)
        {
            ++n_tries;
            try {
                subscriber.subscribeTo(Topics::JOYSTICK_AXES,&axis_callback);
                subscriber.subscribeTo(Topics::JOYSTICK_BUTTONS,&button_callback);
                publisher.connect();
                subscriber.connect();
                connected=true;
            }
            catch(Politocean::mqttException& e){
                cout << "MQTT error: " << e.what() << endl;
                connected = false;
            }

        }
    } catch (std::exception& e) {
        cout << e.what() << endl;
        exit(EXIT_FAILURE);
    }

    subscriber.wait();

    return 0;
}

void axis_callback(const std::string& payload)
{
    vector<int> axes;

    //check if payload is well serialized
    try {
        json j = json::parse(payload);
        axes = j["axes"].get<vector<int>>();

        //Publish
        try
        {
            publisher.publish(Topics::AXES,payload);
        }
        catch (Politocean::mqttException& e)
        {
            cout << "MQTT error: " << e.what() << endl;
        }
    }
    catch (json::exception e)
    {
        cout << e.what() << endl;
    }


    return;
}

string map_button(int b)
{
    int msb = b >> 7;
    int button = b < 1;

    static map<string,int> state = {{"MOTORS",0}};

    switch(button)
    {
        //statefull buttons
        case Commands::Buttons::MOTORS:
            if(msb && !state["MOTORS"])
            {
                state["MOTORS"]=1;
                return Commands::Actions::MOTORS_ON;
            }
            else if (msb && state["MOTORS"])
            {
                state["MOTORS"]=0;
                return Commands::Actions::MOTORS_OFF;
            }
            else
                return Commands::Actions::NONE;

        //stateless buttons
        case Commands::Buttons::VDOWN:
            if(msb) 
                return Commands::Actions::VDOWN;
            else
                return Commands::Actions::VDOWN_STOP;

        case Commands::Buttons::VUP:
            if(msb) 
                return Commands::Actions::VDOWN;
            else
                return Commands::Actions::VDOWN_STOP;

        case Commands::Buttons::WRIST:
            if(msb) 
                return Commands::Actions::WRIST;
            else
                return Commands::Actions::WRIST_STOP;

        case Commands::Buttons::MEDIUM_FAST:
            if(msb)
                return Commands::Actions::MEDIUM_FAST;
            else
                return Commands::Actions::MEDIUM_FAST_STOP;

        case Commands::Buttons::SLOW:
            if(msb)
                return Commands::Actions::SLOW;
            else
                return Commands::Actions::SLOW_STOP;

        default:
            return Commands::Actions::NONE;

    }

}

void button_callback(const string& payload)
{
    string out;
    try
    {
        int button = stoi(payload, nullptr, 2);
        cout << button << endl;
        out = map_button(button);

        if (out.compare(Commands::Actions::NONE))
        {
            try
            {
                publisher.publish(Topics::BUTTONS,out);
            }
            catch (Politocean::mqttException& e)
            {
                cout << "MQTT error: " << e.what() << endl;
            }
        }

    }
    catch (std::exception e)
    {
        cout << e.what() << endl;
    }


    return;
}