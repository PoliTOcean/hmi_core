#include <iostream>
#include <unistd.h>
#include <json.hpp>
#include <string>
#include <map>

#include "Publisher.h"
#include "Subscriber.h"
#include "mqttLogger.h"

#include "PolitoceanConstants.h"
#include "PolitoceanExceptions.hpp"
#include "mqttLogger.h"

#define MAX_TRIES 10

using namespace std;
using namespace Politocean;
using namespace Politocean::Constants;
using namespace Politocean::Constants::Commands;

void button_callback(const string& payload);

string map_button(int button);

Publisher publisher(Hmi::IP_ADDRESS, Hmi::CMD_PRS_ID_PUB);
Subscriber subscriber(Hmi::IP_ADDRESS, Hmi::CMD_PRS_ID_SUB);

mqttLogger ptoLogger(&publisher);

int main(int argc, char* argv[])
{
    logger::enableLevel(logger::DEBUG, true);

    bool connected = false;

    subscriber.subscribeTo(Topics::JOYSTICK_BUTTONS, &button_callback);

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
                ptoLogger.logError(e);
                connected = false;
            }

        }
    } catch (std::exception& e) {
        ptoLogger.logError(e);
        exit(EXIT_FAILURE);
    }

    subscriber.wait();

    return 0;
}

map<int, bool> on;

bool isButtonOn(int b){
    if(on.find(b)!=on.end())
        return on[b];
    else
        return on[b] = false;    
}

string map_button(int msb, int button)
{
    switch(button)
    {
        case Buttons::MOTORS:
            if(msb) //MOTOR_ON
                return Actions::MOTORS_SWAP;
            else
                return Actions::NONE;

        case Buttons::RESET:
            if(msb) //RESET
                return Actions::RESET;
            else
                return Actions::NONE;

        case Buttons::AUTONOMOUS:
            if(!isButtonOn(Buttons::AUTONOMOUS) && msb) //AUTONOMOUS_ON
            {
                on[Buttons::AUTONOMOUS]=1;
                return Actions::AUTONOMOUS_ON;
            }
            else if(isButtonOn(Buttons::AUTONOMOUS) && !msb) //AUTONOMOUS_OFF
            {
                on[Buttons::AUTONOMOUS]=0;
                return Actions::AUTONOMOUS_OFF;
            }
            else
                return Actions::NONE;

        case Buttons::WRIST:
            if( msb)
                return Actions::WRIST_SWAP;
            else
                return Actions::NONE;

        case Buttons::VUP:
            if(msb)
                return Actions::VUP;
            else
                return Actions::VUP_STOP;

        case Buttons::VDOWN:
            if(msb)
                return Actions::VDOWN;
            else
                return Actions::VDOWN_STOP;

        case Buttons::SLOW:
            if(msb)
                return Actions::SLOW;
            else
                return Actions::SLOW_STOP;

        case Buttons::MEDIUM_FAST:
            if(msb)
                return Actions::MEDIUM_FAST;
            else
                return Actions::MEDIUM_FAST_STOP;

        default:
            return Actions::NONE;

    }

}

void button_callback(const string& payload)
{
    unsigned char data = static_cast<unsigned char>(std::stoi(payload));

    bool value 				= (data >> 7) & 0x01;
    unsigned short int id 	= data & 0x7F;
;
    try
    {
        string out = map_button(value, id);

        if (!out.empty())
            publisher.publish(Topics::JOYSTICK_BUTTONS,out);

    }
    catch (Politocean::mqttException& e)
    {
        ptoLogger.logError(e);
    }
    catch (std::exception e)
    {
        ptoLogger.logError(e);
    }

    return;
}