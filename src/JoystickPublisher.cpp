
#include <iostream>
#include <string>
#include <vector>

#include "MqttClient.h"
#include "Joystick.h"

#include "PolitoceanExceptions.hpp"
#include "PolitoceanConstants.h"

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
		while (publisher.is_connected())
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

		while (publisher.is_connected())
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
	logger::enableLevel(logger::DEBUG);

	// Create a publisher object and a talker.
	MqttClient joystickPublisher = MqttClient::getInstance(Hmi::JOYSTICK_ID, Hmi::IP_ADDRESS);
	Talker talker;

	mqttLogger logger = mqttLogger::getInstance(joystickPublisher);
    logger.setPublishLevel(logger::CONFIG);

	// Create a joystick object and a listener.
	Joystick joystick;
	Listener listener;

	// Try to connect to the joystick device.
	// If error has caught, terminate with EXIT_FAILURE
	try
	{
		joystick.connect();
	}
	catch (const JoystickException& e)
	{
		std::cerr << e.what() << std::endl;
		std::exit(EXIT_FAILURE);
	}

	// Start reading data from the joystick device.
	joystick.startReading(&Listener::listen, &listener);
	// Start talker talking
	talker.startTalking(joystickPublisher, listener);

	int nretry = 0;
	while (joystickPublisher.is_connected())
	{
		while (joystick.isConnected() && joystickPublisher.is_connected());
		
		if (!joystickPublisher.is_connected())
			break;
		
		talker.stopTalking();

		std::cerr << "Joystick device disconnected" << std::endl;
		while (!joystick.isConnected())
		{
			std::cout << "\tRetry to reconnect... " << nretry++ << std::endl;

			if (nretry >= MAX_JOYSTICK_CONNECTION_RETRY)
			{
				std::cerr << "Cannot reconnect to joystick device" << std::endl;
				std::exit(EXIT_FAILURE);
			}
		}

		talker.startTalking(joystickPublisher, listener);
	}

	return 0;
}