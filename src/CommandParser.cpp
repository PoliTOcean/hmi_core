#include <iostream>
#include <string>
#include <map>
#include <thread>
#include <chrono>

#include "Publisher.h"
#include "Subscriber.h"

#include <json.hpp>

#include "PolitoceanConstants.h"
#include "PolitoceanExceptions.hpp"
#include "mqttLogger.h"

using namespace Politocean;
using namespace Politocean::Constants;
using namespace Politocean::Constants::Commands;
/**************************************************************
 * Listener class for Joystick device
 *************************************************************/

class Listener {
    int id_, value_;

    bool isButtonUpdated_ = false;
    bool isAxesUpdated_ = false;

    std::vector<int> axes_;


public:
    void listenForButtons(const std::string& payload);
    void listenForAxes(const std::string& payload);

    int id();
    int value();

    std::vector<int> axes();

    bool isButtonUpdated();
    bool isAxesUpdated();
};

void Listener::listenForButtons(const std::string& payload)
{
    int button  = static_cast<int>(std::stoi(payload));
    value_      = (button >> 7) & 0x01;
    id_         = button & 0x7F;

    isButtonUpdated_ = true;
}

int Listener::id()
{
    isButtonUpdated_ = false;

    return id_;
}

int Listener::value()
{
    isButtonUpdated_ = false;

    return value_;
}

bool Listener::isButtonUpdated()
{
    return isButtonUpdated_;
}

bool Listener::isAxesUpdated()
{
    return isAxesUpdated_;
}

void Listener::listenForAxes(const std::string& payload)
{
    auto c_map = nlohmann::json::parse(payload);
    axes_ = c_map.get<std::vector<int>>();

    isAxesUpdated_ = true;
}

std::vector<int> Listener::axes(){
    isAxesUpdated_ = false;
    return axes_;
}

/**************************************************************
 * Talker class for Joystick publisher
 *************************************************************/

class Talker {
    /**
     * @buttonTalker	: talker thread for button value
     */
    std::thread *buttonTalker_, *axesTalker_;

    /**
     * @isTalking_ : it is true if the talker is isTalking
     */
    bool isTalking_ = false;

public:
    void startTalking(Publisher& publisher, Listener& listener);
    void stopTalking();

    bool isTalking();
};

void Talker::startTalking(Publisher& publisher, Listener& listener)
{
    if (isTalking_)
        return ;

    isTalking_ = true;

    axesTalker_ = new std::thread([&](){
        while(publisher.is_connected())
        {
            if(!listener.isAxesUpdated())
                continue;
            
            std::vector<int> axes = listener.axes();

            std::vector<int> atmega_axes = {
                axes[Axes::X],
                axes[Axes::Y],
                axes[Axes::RZ]
            };
            nlohmann::json atmega = atmega_axes;
            publisher.publish(Topics::JOYSTICK_AXES, atmega.dump());

            int shoulder_axes = axes[Axes::SHOULDER];
            nlohmann::json shoulder = shoulder_axes;
            publisher.publish(Topics::SHOULDER_VELOCITY, shoulder.dump());

            int shoulder_wrist = axes[Axes::WRIST];
            nlohmann::json wrist = shoulder_wrist;
            publisher.publish(Topics::WRIST_VELOCITY, wrist.dump());

            int shoulder_hand = axes[Axes::HAND];
            nlohmann::json hand = shoulder_hand;
            publisher.publish(Topics::HAND_VELOCITY, hand.dump());

        }
    });

    buttonTalker_ = new std::thread([&]() {

        map<int, bool> state = {{Buttons::MOTORS,false}};
        while (publisher.is_connected())
        {
            if (!listener.isButtonUpdated())
                continue;
            
            int id      = listener.id();
            int value   = listener.value();
            

            string action = Actions::NONE;

            string topic = "";

            // Parsing button by identifier
            switch (id)
            {
                case Buttons::START_AND_STOP:
                    topic = Topics::COMMANDS;
                    if (value)
                        action = Actions::AtMega::START_AND_STOP;
                    break;
                
                case Buttons::MOTORS:
                    topic = Topics::COMMANDS;
                    if (value && !state[id])
                    {
                        action = Actions::ON;
                        state[id] = true;
                    }
                    else if (value && state[id])
                    {
                        action = Actions::OFF;
                        state[id] = false;
                    }
                    break;

                case Buttons::RESET:
                    topic = Topics::COMMANDS;
                    if (value)
                        action = Actions::RESET;
                    break;

                case Buttons::VUP:
                    topic = Topics::COMMANDS;
                    value ? action = Actions::AtMega::VUP_ON : action = Actions::AtMega::VUP_OFF;
                    break;

                case Buttons::VDOWN:
                    topic = Topics::COMMANDS;
                    value ? action = Actions::AtMega::VDOWN_ON : action = Actions::AtMega::VDOWN_OFF;
                    break;

                case Buttons::SLOW:
                    topic = Topics::COMMANDS;
                    if (value)
                        action = Actions::AtMega::SLOW;
                    break;
                
                case Buttons::MEDIUM_FAST:
                    topic = Topics::COMMANDS;
                    if (value)
                        action = Actions::AtMega::MEDIUM;
                    else
                        action = Actions::AtMega::FAST;
                    break;

                case Buttons::SHOULDER_ENABLE:
                    topic = Topics::SHOULDER;
                    if (value)
                        action = Actions::ON;
                break;
                case Buttons::SHOULDER_DISABLE:
                    topic = Topics::SHOULDER;
                    if (value)
                        action = Actions::OFF;

                case Buttons::WRIST_ENABLE:
                    topic = Topics::WRIST;
                    if (value)
                        action = Actions::ON;
                    break;

                case Buttons::WRIST_DISABLE:
                    topic = Topics::WRIST;
                    if (value)
                        action = Actions::OFF;
                    break;

                case Buttons::WRIST:
                    topic = Topics::WRIST;
                    if (value)
                        action = Actions::START;
                    else
                        action = Actions::STOP;
                    break;

                case Buttons::SHOULDER_UP:
                    topic = Topics::SHOULDER;
                    if (value)
                        action = Actions::Arm::SHOULDER_UP;
                    else
                        action = Actions::Arm::SHOULDER_DOWN;
                    break;

                case Buttons::SHOULDER_DOWN:
                    topic = Topics::SHOULDER;
                    if(value)
                        action = Actions::Arm::SHOULDER_DOWN;
                    else
                        action = Actions::Arm::SHOULDER_UP;
                    break;

                case Buttons::HAND:
                    topic = Topics::HAND;
                    if(value)
                        action = Actions::START;
                    else
                        action = Actions::STOP;
                    break;

                default: 
                    break;
            }

           // std::cout << topic << " " << action << std::endl;

            if(action != Actions::NONE)
                publisher.publish(topic, action);
        }

        isTalking_ = false;
    });
}

void Talker::stopTalking()
{
    if (!isTalking_)
        return ;

    isTalking_ = false;
    buttonTalker_->join();
}

bool Talker::isTalking()
{
    return isTalking_;
}

int main(int argc, const char* argv[])
{

    Publisher publisher(Constants::Rov::IP_ADDRESS, Constants::Hmi::CMD_ID);
    Talker talker;

    Subscriber subscriber(Constants::Hmi::IP_ADDRESS, Constants::Hmi::CMD_ID);
    Listener listener;

    mqttLogger ptoLogger(&publisher);
    // logger::enableLevel(logger::DEBUG, true);

    subscriber.subscribeTo(Topics::JOYSTICK_BUTTONS, &Listener::listenForButtons, &listener);
    subscriber.subscribeTo(Topics::JOYSTICK_AXES, &Listener::listenForAxes, &listener);

    try
    {
        publisher.connect();
        subscriber.connect();
    }
    catch (const mqttException& e)
    {
        std::cerr << e.what() << '\n';
    }

    talker.startTalking(publisher, listener);
    
    subscriber.wait();

    talker.stopTalking();

    return 0;
}