#include <iostream>
#include <string>
#include <map>
#include <thread>
#include <chrono>

#include "MqttClient.h"

#include "PolitoceanExceptions.hpp"
#include "PolitoceanConstants.h"

#include "logger.h"
#include "mqttLogger.h"

#include "json.hpp"

// Let Catch provide main():
#define CATCH_CONFIG_MAIN

#include <catch2/catch.hpp>

using namespace Politocean;
using namespace Politocean::Constants;
using namespace Politocean::Constants::Commands;
/**************************************************************
 * Listener class for Joystick device
 *************************************************************/

class Listener {
    int id_, value_;
public:
    void callBackFunc(const std::string& payload); // my func
};


/**************************************************************
 * Talker class for Joystick publisher
 *************************************************************/

class Talker {

public:
    void startTalking(MqttClient& publisher, Listener& listener);
    void stopTalking();

};

void Talker::startTalking(MqttClient& publisher, Listener& listener)
{
    
    if(publisher.is_connected()){
        std::string action = "";
        std::string topic = "";
        publisher.publish(topic,action);
    }
    
}

void Talker::stopTalking()
{
    /*if (!isTalking_)
        return ;

    isTalking_ = false;
    buttonTalker_->join();*/
}

/*bool Talker::isTalking()
{
    return isTalking_;
}*/

TEST_CASE( "First mqtt", "[mqtt]" ) {
    /*REQUIRE( Factorial(0) == 1 );
    REQUIRE( Factorial(1) == 1 );
    REQUIRE( Factorial(2) == 2 );
    REQUIRE( Factorial(3) == 6 );
    REQUIRE( Factorial(10) == 3628800 );*/
    REQUIRE( 0 == 0 );
}

/*int main(int argc, const char* argv[])
{
    logger::enableLevel(logger::DEBUG, true);
    const std::string id = "custom";
    const std::string address = "127.0.0.1"; // localhost

    MqttClient myClient(id,address);
    Listener l;

    //MqttClient rovClient(Constants::Hmi::CMD_ID, Constants::Rov::IP_ADDRESS);
    //Talker talker;

    //MqttClient hmiClient(Constants::Hmi::CMD_ID, Constants::Hmi::IP_ADDRESS);
    //Listener listener;

    //mqttLogger ptoLogger(&hmiClient);

    try
    {   
        myClient.connect();
        //rovClient.connect();
        //hmiClient.connect();
    }
    catch (const mqttException& e)
    {
        std::cerr << e.what() << '\n';
    }

    //hmiClient.subscribeTo(Topics::JOYSTICK_BUTTONS, &Listener::listenForButtons, &listener);
    //hmiClient.subscribeTo(Topics::JOYSTICK_AXES, &Listener::listenForAxes, &listener);

    const std::string topic_name = "test";
    myClient.subscribeTo(topic_name,&Listener::myFunc, &l);

    //talker.startTalking(rovClient, listener);
    
    //rovClient.wait();
    myClient.wait();

    //talker.stopTalking();

    return 0;
}*/