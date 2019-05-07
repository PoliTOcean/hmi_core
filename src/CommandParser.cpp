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

    bool isUpdated_ = false;

public:
	void listen(const std::string& payload);

	int id();
    int value();

	bool isButtonUpdated();
};

void Listener::listen(const std::string& payload)
{
    int button = static_cast<int>(std::stoi(payload));
    value_  = (button >> 7) & 0x01;
    id_     = button & 0x7F;

    isUpdated_ = true;
}

int Listener::id()
{
    isUpdated_ = false;

    return id_;
}

int Listener::value()
{
    isUpdated_ = false;

    return value_;
}

bool Listener::isButtonUpdated()
{
    return isUpdated_;
}

/**************************************************************
 * Talker class for Joystick publisher
 *************************************************************/

class Talker {
	/**
	 * @buttonTalker	: talker thread for button value
	 */
	std::thread *buttonTalker_;

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

    buttonTalker_ = new std::thread([&]() {
        while (publisher.is_connected())
        {
            if (!listener.isButtonUpdated())
                continue;

            int id      = listener.id();
            int value   = listener.value();

            unsigned char action = Constants::Commands::Actions::NONE;

            // Parsing button by identifier
            switch (id)
            {
                // Parsing 12V motors
                case Constants::Commands::Buttons::MOTORS:
                    if (value)
                        action = Constants::Commands::Actions::MOTORS_SWAP;
                break;

                // Parsing reset button
                case Constants::Commands::Buttons::RESET:
                    if (value)
                        action = Constants::Commands::Actions::RESET;
                break;

                // Parsing vertical up button
                case Constants::Commands::Buttons::VUP:
                    value ? action = Constants::Commands::Actions::VUP_ON : action = Constants::Commands::Actions::VUP_OFF;
                break;

                // Parsing vertical down button
                case Constants::Commands::Buttons::VDOWN:
                    value ? action = Constants::Commands::Actions::VDOWN_ON : action = Constants::Commands::Actions::VDOWN_OFF;
                break;

                // Parsing wrist button
                case Constants::Commands::Buttons::WRIST:
                    if (value)
                        action = Constants::Commands::Actions::WRIST_SWAP;
                break; 

                default:
                    break;
            }

            if (action != Constants::Commands::Actions::NONE)
                publisher.publish(Constants::Topics::BUTTONS, std::to_string(action));
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
    Publisher publisher(Constants::Hmi::IP_ADDRESS, Constants::Hmi::CMD_PRS_ID_PUB);
    Talker talker;

    Subscriber subscriber(Constants::Hmi::IP_ADDRESS, Constants::Hmi::CMD_PRS_ID_SUB);
    Listener listener;

    mqttLogger ptoLogger(&publisher);
	logger::enableLevel(logger::DEBUG, true);

    subscriber.subscribeTo(Constants::Topics::JOYSTICK_BUTTONS, &Listener::listen, &listener);

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