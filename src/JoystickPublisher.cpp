
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

#include "MqttClient.h"
#include "Joystick.h"

#include "PolitoceanExceptions.hpp"
#include "PolitoceanConstants.h"
#include "PolitoceanUtils.hpp"

#include "logger.h"
#include "mqttLogger.h"

#include "json.hpp"

using namespace Politocean;
using namespace Politocean::Constants;

/**************************************************************
 * Listener class for Joystick device
 *************************************************************/

class Listener {
	std::vector<int> axes_;
	unsigned char button_;

public:
	void listen(const std::vector<int>& axes, unsigned char button);

	std::vector<int> axes();
	unsigned char button();
};

void Listener::listen(const std::vector<int>& axes, unsigned char button)
{
	axes_ 	= axes;
	button_	= button;
}

std::vector<int> Listener::axes()
{
	return axes_;
}

unsigned char Listener::button()
{
	return button_;
}

/**************************************************************
 * Talker class for Joystick publisher
 *************************************************************/

class Talker {
	/**
	 * @axesTalker		: talker thread for axes values
	 * @buttonTalker	: talker thread for button value
	 */
	std::thread *axesTalker_, *buttonTalker_;

	/**
	 * @isTalking_ : it is true if the talker is talking
	 */
	bool isTalking_ = false;

public:
	void startTalking(MqttClient& publisher, Listener& listener);
	void stopTalking();

	bool isTalking();
};

void Talker::startTalking(MqttClient& publisher, Listener& listener)
{
	if (isTalking_)
		return ;

	isTalking_ = true;

	axesTalker_ = new std::thread([&]() {
		while (isTalking_)
		{
			nlohmann::json j_map = listener.axes();

			publisher.publish(Topics::JOYSTICK_AXES, j_map.dump());

			std::this_thread::sleep_for(std::chrono::milliseconds(Timing::Milliseconds::JOYSTICK_AXIS));
		}

		isTalking_ = false;
	});

	buttonTalker_ = new std::thread([&]() {
		unsigned char lastButton = -1;
		unsigned char button;

		while (isTalking_)
		{
			if ((button = listener.button()) == lastButton)
				continue;

			publisher.publish(Topics::JOYSTICK_BUTTONS, std::to_string(button));
			lastButton = button;
		}

		isTalking_ = false;
	});
}

void Talker::stopTalking()
{
	if (!isTalking_)
		return ;


	isTalking_ = false;
	axesTalker_->join(); buttonTalker_->join();
}

bool Talker::isTalking()
{
	return isTalking_;
}

/**************************************************************
 * Main section
 *************************************************************/

#define DFLT_CLIENT_ID "JoystickPublisher"
#define MAX_JOYSTICK_CONNECTION_RETRY 5

int main(int argc, const char *argv[])
{
	logger::enableLevel(logger::INFO);

	// Create a publisher object and a talker.
	MqttClient& joystickPublisher = MqttClient::getInstance(Hmi::JOYSTICK_ID, Hmi::IP_ADDRESS);
	Talker talker;

	mqttLogger logger = mqttLogger::getInstance(joystickPublisher);
    logger.setPublishLevel(logger::CONFIG);
	// Try to connect the publisher
	try
	{
		joystickPublisher.connect();
	}
	catch(const mqttException& e)
	{
		std::cerr << e.what() << '\n';
	}

	
	// Create a joystick object and a listener.
	Joystick joystick;
	Listener listener;

	// Try to connect to the joystick device.
	// If error has caught, terminate with EXIT_FAILURE
	while (!joystick.isConnected())
	{
		try
		{
			joystick.connect();
		}
		catch (const JoystickException& e)
		{
			std::cerr << e.what() << std::endl;
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	Politocean::publishComponents(Hmi::JOYSTICK_ID, Components::JOYSTICK, Components::Status::ENABLED);
	std::cout << "Joystick device is connected.\n" << std::endl;

	// Start reading data from the joystick device.
	joystick.startReading(&Listener::listen, &listener);
	// Start talker talking
	talker.startTalking(joystickPublisher, listener);

	int nretry = 0;
	while (joystickPublisher.is_connected())
	{
		if (joystick.isConnected())
			continue ;

		Politocean::publishComponents(Hmi::JOYSTICK_ID, Components::JOYSTICK, Components::Status::ERROR);
		std::cerr << "Joystick device disconnected" << std::endl;
		
		talker.stopTalking();

		while (!joystick.isConnected())
		{
			std::cout << "\tRetry to reconnect... " << nretry++ << std::endl;

			try
			{
				joystick.connect();
			}
			catch(const JoystickException& e)
			{
				std::cerr << e.what() << '\n';
			}
			
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}

		Politocean::publishComponents(Hmi::JOYSTICK_ID, Components::JOYSTICK, Components::Status::ENABLED);

		std::cout << "Joystick device is connected." << std::endl;
		
		talker.startTalking(joystickPublisher, listener);
	}

	return 0;
}