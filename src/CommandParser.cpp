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
    int button = static_cast<int>(std::stoi(payload));
    value_  = (button >> 7) & 0x01;
    id_     = button & 0x7F;

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
	 * @isTalking_ : it is true if the talker is talking
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
                axes[Constants::Commands::Axes::X],
                axes[Constants::Commands::Axes::Y],
                axes[Constants::Commands::Axes::RZ]
            };
            nlohmann::json atmega = atmega_axes;
            publisher.publish(Constants::Topics::JOYSTICK_AXES, atmega.dump());

            int shoulder_axes = axes[Constants::Commands::Axes::SHOULDER];
            nlohmann::json shoulder = shoulder_axes;
            publisher.publish(Constants::Topics::SHOULDER_VELOCITY, shoulder.dump());

            int shoulder_wrist = axes[Constants::Commands::Axes::WRIST];
            nlohmann::json wrist = shoulder_wrist;
            publisher.publish(Constants::Topics::WRIST_VELOCITY, wrist.dump());

            int shoulder_hand = axes[Constants::Commands::Axes::HAND];
            nlohmann::json hand = shoulder_hand;
            publisher.publish(Constants::Topics::HAND_VELOCITY, hand.dump());

        }
    });

    buttonTalker_ = new std::thread([&]() {
        while (publisher.is_connected())
        {
            if (!listener.isButtonUpdated())
                continue;

            int id      = listener.id();
            int value   = listener.value();

            unsigned char action = Constants::Commands::Actions::NONE;

            string topic = "";

            // Parsing button by identifier
            switch (id)
            {
                case Constants::Commands::Buttons::START_AND_STOP:
                    topic = Constants::Topics::BUTTONS;
                    if (value)
                        action = Constants::Commands::Actions::START_AND_STOP;
                break;
                // Parsing 12V motors
                case Constants::Commands::Buttons::MOTORS:
                    topic = Constants::Topics::BUTTONS;
                    if (value)
                        action = Constants::Commands::Actions::MOTORS_SWAP;
                break;

                // Parsing reset button
                case Constants::Commands::Buttons::RESET:
                    topic = Constants::Topics::BUTTONS;
                    if (value)
                        action = Constants::Commands::Actions::RESET;
                break;

                // Parsing vertical up button
                case Constants::Commands::Buttons::VUP:
                    topic = Constants::Topics::BUTTONS;
                    value ? action = Constants::Commands::Actions::VUP_ON : action = Constants::Commands::Actions::VUP_OFF;
                break;

                // Parsing vertical down button
                case Constants::Commands::Buttons::VDOWN:
                    topic = Constants::Topics::BUTTONS;
                    value ? action = Constants::Commands::Actions::VDOWN_ON : action = Constants::Commands::Actions::VDOWN_OFF;
                break;

                case Constants::Commands::Buttons::SLOW:
                    topic = Constants::Topics::BUTTONS;
                    if(value)
                        action = Constants::Commands::Actions::SLOW;
                break;
                
                case Constants::Commands::Buttons::MEDIUM_FAST:
                    topic = Constants::Topics::BUTTONS;
                    if(value)
                        action = Constants::Commands::Actions::MEDIUM;
                    else
                        action = Constants::Commands::Actions::FAST;
                break;

                case Constants::Commands::Buttons::SHOULDER_ENABLE:
                    topic = Constants::Topics::SHOULDER;
                    if(value)
                        action = Constants::Commands::Actions::SHOULDER_ON;
                break;
                case Constants::Commands::Buttons::SHOULDER_DISABLE:
                    topic = Constants::Topics::SHOULDER;
                    if(value)
                        action = Constants::Commands::Actions::SHOULDER_OFF;
                break;

                case Constants::Commands::Buttons::WRIST_ENABLE:
                    topic = Constants::Topics::WRIST;
                    if(value)
                        action = Constants::Commands::Actions::WRIST_ON;
                break;
                case Constants::Commands::Buttons::WRIST_DISABLE:
                    topic = Constants::Topics::WRIST;
                    if(value)
                        action = Constants::Commands::Actions::WRIST_OFF;
                break;

                case Constants::Commands::Buttons::WRIST:
                    topic = Constants::Topics::WRIST;
                    if(value)
                        action = Constants::Commands::Actions::WRIST_START;
                    else
                        action = Constants::Commands::Actions::WRIST_STOP;
                break;

                case Constants::Commands::Buttons::SHOULDER_UP:
                    topic = Constants::Topics::SHOULDER;
                    if(value)
                        action = Constants::Commands::Actions::SHOULDER_UP;
                    else
                        action = Constants::Commands::Actions::SHOULDER_STOP;
                break;

                case Constants::Commands::Buttons::SHOULDER_DOWN:
                    topic = Constants::Topics::SHOULDER;
                    if(value)
                        action = Constants::Commands::Actions::SHOULDER_DOWN;
                    else
                        action = Constants::Commands::Actions::SHOULDER_STOP;
                break;

                default:
                break;
            }

            if (action != Constants::Commands::Actions::NONE)
                publisher.publish(topic, std::to_string(action));
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

    Publisher publisher(Constants::Rov::IP_ADDRESS, Constants::Hmi::CMD_PRS_ID_PUB);
    Talker talker;

    Subscriber subscriber(Constants::Hmi::IP_ADDRESS, Constants::Hmi::CMD_PRS_ID_SUB);
    Listener listener;

    mqttLogger ptoLogger(&publisher);
//	logger::enableLevel(logger::DEBUG, true);

    subscriber.subscribeTo(Constants::Topics::JOYSTICK_BUTTONS, &Listener::listenForButtons, &listener);
    subscriber.subscribeTo(Constants::Topics::JOYSTICK_AXES, &Listener::listenForAxes, &listener);

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