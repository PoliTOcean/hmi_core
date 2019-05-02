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

Publisher publisher("127.0.0.1", Hmi::CLIENT_ID);

Subscriber subscriber("127.0.0.1",Hmi::CLIENT_ID,Hmi::Topics::JOYSTICK_BUTTON,&button_callback);
//ToDo: aggiungere metodo per aggiungere topic

int main(int argc, char* argv[])
{
    bool connected = false;

    try {
        int n_tries=0;
        while(!connected && n_tries < MAX_TRIES)
        {
            ++n_tries;
            try {
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
            publisher.publish(Hmi::Topics::JOYSTICK_AXES,payload);
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

    static int on[N_BUTTONS]={0,0,0,0,0,0,0,0};

    switch(button)
    {
        case Buttons::MOTOR:
            if(!on[Buttons::MOTOR] && msb) //MOTOR_ON
            {
                on[Buttons::MOTOR]=1;
                return "MOTOR_ON";
            }
            else if(on[Buttons::MOTOR] && !msb) //MOTOR_OFF
            {
                on[Buttons::MOTOR]=0;
                return "MOTOR_OFF";
            }
            else
                return "NULL";

        case Buttons::RESET:
            if(msb) //RESET
                return "RESET";
            else
                return "NULL";

        case Buttons::AUTONOMOUS:
            if(!on[Buttons::AUTONOMOUS] && msb) //AUTONOMOUS_ON
            {
                on[Buttons::AUTONOMOUS]=1;
                return "AUTONOMOUS_ON";
            }
            else if(on[Buttons::AUTONOMOUS] && !msb) //AUTONOMOUS_OFF
            {
                on[Buttons::AUTONOMOUS]=0;
                return "AUTONOMOUS_OFF";
            }
            else
                return "NULL";

        case Buttons::WRIST:
            if(!on[Buttons::WRIST] && msb) //WRIST_ON
            {
                on[Buttons::WRIST]=1;
                return "WRIST_ON";
            }
            else if(on[Buttons::WRIST] && !msb) //WRIST_OFF
            {
                on[Buttons::WRIST]=0;
                return "WRIST_OFF";
            }
            else
                return "NULL";

        case Buttons::V_UP:
            if(msb)
                return "V_UP";
            else
                return "NULL";

        case Buttons::V_DOWN:
            if(msb)
                return "V_DOWN";
            else
                return "NULL";

        case Buttons::SLOW:
            if(msb)
                return "SLOW";
            else
                return "NULL";

        case Buttons::MEDIUM_FAST:
            if(msb)
                return "MEDIUM_FAST";
            else
                return "NULL";

        default:
            return "NULL";

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

        if (out.compare("NULL"))
        {
            try
            {
                publisher.publish(Hmi::Topics::JOYSTICK_BUTTONS,out);
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