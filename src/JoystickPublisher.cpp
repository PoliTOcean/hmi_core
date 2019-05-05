
#include <iostream>
#include <string>
#include <vector>

#include "Subscriber.h"
#include "Publisher.h"
#include "Joystick.h"

#include "PolitoceanExceptions.h"
#include "PolitoceanConstants.h"

#include "json.hpp"

using namespace Politocean;

/**************************************************************
 * Listener class for Joystick device
 *************************************************************/

class Listener {
	std::vector<int> axes_;
	unsigned char button_;

	Joystick joystick_;
public:
	Listener(const Joystick& joystick) : joystick_(joystick) {}

	void listen(const std::vector<int>& axes, unsigned char button);

	std::vector<int> axes();
	unsigned char button();

	void setJoystick(const Joystick& joystick);
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
	bool isTalking_;

public:
	void startTalking(const Publisher& publisher, const Listener& listener);
	void stopTalking();

	bool isTalking();
}

void Talker::startTalking(const Publisher& publisher, const Listener& listener)
{
	if (isTalking_)
		return ;

	isTalking_ = true;

	axesTalker = new std::thread([publisher, listener]() {
		while (publisher.isConnected() && listener.joystick.isConnected())
		{
			nlohmann::json j_map = { listener.axes() };

			publisher.publish(Constants::Topics::JOYSTICK_AXES, j_map.dump());
		}

		isTalking_ = false;
	});

	buttonTalker = new std::thread([publisher, listener]() {
		while (publisher.isConnected() && listener.joystick.isConnected)

			publisher.publish(Constants::Topics::JOYSTICK_BUTTONS, std::to_string(listener.button()));
		isTalking_ = false;
	});
}

void Talker::stopTalking()
{
	if (!isTalking_)
		return ;

	isTalking_ = false;
	axesTalker->join(); buttonTalker->join();
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

Publisher pub(Hmi::IP_ADDRESS, Hmi::CLIENT_ID);
mqttLogger ptoLogger(&pub);

void testcb(const std::string& payload){
    cout << payload << endl;
}
Subscriber sub(Hmi::IP_ADDRESS, "testhmi");

int main(int argc, const char *argv[])
{
    // Enable logging
    sub.connect();
    logger::enableLevel(logger::DEBUG, true);

	// Create a publisher object and a talker.
	Publisher joystickPublisher(Hmi::IP_ADDRESS, DFLT_CLIENT_ID);
	Talker talker;
	
	// Create a joystick object and a listener.
	Joystick joystick;
	Listener listener(joystick);

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
	talker.startTalking(&publisher, &listener);

	int nretry = 0;
	while (joystickPublisher.isConnected())
	{
		while (joystick.isConnected() && joystickPublisher.isConnected());
		
		if (!joystickPublisher.isConnected())
			break;
		
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

	}

    sub.wait();

	return 0;
}