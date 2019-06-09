
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <queue>
#include <mutex>

#include "MqttClient.h"
#include "Joystick.h"

#include "PolitoceanExceptions.hpp"
#include "PolitoceanConstants.h"

#include "logger.h"
#include "mqttLogger.h"

#include "json.hpp"

#include "Component.hpp"
#include "ComponentsManager.hpp"

#include <Reflectables/Vector.hpp>

using namespace Politocean;
using namespace Politocean::Constants;

/**************************************************************
 * Listener class for Joystick device
 *************************************************************/

class Listener {
	std::vector<int> axes_;
	std::queue<unsigned char> buttons_;
	unsigned char lastButton_ = 0;

public:
	void listen(const std::vector<int>& axes, unsigned char button);

	std::vector<int> axes();
	unsigned char button();
	bool isButtonUpdated();
};

void Listener::listen(const std::vector<int>& axes, unsigned char button)
{
	axes_ 	= axes;
	
	if(buttons_.empty() && button != lastButton_ || !buttons_.empty() && button != buttons_.back())
	{
		buttons_.push(button);
	}
}

std::vector<int> Listener::axes()
{
	return axes_;
}

unsigned char Listener::button()
{
	if (buttons_.empty()) return lastButton_;

	unsigned char button = buttons_.front();
	buttons_.pop();
	if (buttons_.empty()) lastButton_ = button;

	return button;
}

bool Listener::isButtonUpdated()
{
	return !buttons_.empty();
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
			std::this_thread::sleep_for(std::chrono::milliseconds(Timing::Milliseconds::JOYSTICK));

			Types::Vector<int> axes = listener.axes();

			publisher.publish(Topics::JOYSTICK_AXES, axes);
		}

		isTalking_ = false;
	});

	buttonTalker_ = new std::thread([&]() {
		unsigned char button;

		while (isTalking_)
		{			
			if (!listener.isButtonUpdated())
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(Timing::Milliseconds::JOYSTICK));
				continue;
			}
			button = listener.button();
			publisher.publish(Topics::JOYSTICK_BUTTONS, std::to_string(button));
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

	mqttLogger& logger = mqttLogger::getInstance(joystickPublisher);
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

	ComponentsManager::Init(Hmi::COMPONENTS_ID);

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
			ComponentsManager::SetComponentState(component_t::JOYSTICK, Component::Status::ERROR);
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	ComponentsManager::SetComponentState(component_t::JOYSTICK, Component::Status::ENABLED);

	// Start reading data from the joystick device.
	joystick.startReading(&Listener::listen, &listener);
	// Start talker talking
	talker.startTalking(joystickPublisher, listener);

	int nretry = 0;
	while (joystickPublisher.is_connected())
	{
		if (joystick.isConnected())
			continue ;

		ComponentsManager::SetComponentState(component_t::JOYSTICK, Component::Status::ERROR);

		talker.stopTalking();

		while (!joystick.isConnected())
		{
			std::cout << "\tRetry to reconnect... " << nretry++ << std::endl;
			/*
			/*
			if (nretry >= MAX_JOYSTICK_CONNECTION_RETRY)
				std::cerr << "Cannot reconnect to joystick device" << std::endl;
			*/

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

		ComponentsManager::SetComponentState(component_t::JOYSTICK, Component::Status::ENABLED);
		
		talker.startTalking(joystickPublisher, listener);
	}

	return 0;
}