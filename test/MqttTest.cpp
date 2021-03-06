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
    std::string value_;
    bool updated_;
    bool tested_;
public:
    Listener() : updated_(false), tested_(false) {}
    void callBackFunc(const std::string& payload); // my func

    std::string value();
    bool isUpdated();
    bool isTested();
};

void Listener::callBackFunc(const std::string& payload)
{
    value_ = payload;
    if(value_ == "test"){
        tested_ = true;
    }
    updated_ = true;
}

std::string Listener::value()
{
    updated_ = false;
    return value_;
}

bool Listener::isUpdated()
{
    return updated_;
}

bool Listener::isTested(){
    return tested_;
}


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

/*TEST_CASE("First mqtt","[mqtt]")
{
    const std::string id = "ID_1";
    const std::string address = "127.0.0.1";
    const std::string topic_name = "test";
    MqttClient myClient(id,address);
    Listener listener;

    try
    {   
        myClient.connect();
    }
    catch (const mqttException& e)
    {
        std::cerr << e.what() << '\n';
    }

    myClient.subscribeTo(topic_name, &Listener::callBackFunc, &listener);

    while (!listener.isUpdated());

    REQUIRE(listener.value() == "ciao");
}

/**
 * Testin the control station @ 10.0.0.1
 */
TEST_CASE("Testing Control station", "control"){

    const std::string id_pub = "TEST_PUB_C";
    const std::string id_sub = "TEST_SUB_C";
    const std::string address = "10.0.0.1";
 

    SECTION("Axes") {
        MqttClient& publisher = MqttClient::getInstance(id_pub, address);
        MqttClient& subscriber = MqttClient::getInstance(id_sub,address);
        Listener listener;
        // check connection
        REQUIRE(publisher.is_connected() == true);
        REQUIRE(subscriber.is_connected() == true);
        subscriber.subscribeTo(Politocean::Constants::Topics::JOYSTICK_AXES, &Listener::callBackFunc, &listener);
        sleep(1);
        publisher.publish(Politocean::Constants::Topics::JOYSTICK_AXES,"test");
        sleep(5);
        REQUIRE(listener.isUpdated() == true );
        REQUIRE(listener.isTested() == true );
        subscriber.unsubscribeFrom(Politocean::Constants::Topics::JOYSTICK_AXES);
        subscriber.disconnect();
        publisher.disconnect();
    };

    SECTION("Buttons") {
        MqttClient& publisher = MqttClient::getInstance(id_pub, address);
        MqttClient& subscriber = MqttClient::getInstance(id_sub,address);
        Listener listener;
        // check connection
        REQUIRE(publisher.is_connected() == true);
        REQUIRE(subscriber.is_connected() == true);
        subscriber.subscribeTo(Politocean::Constants::Topics::JOYSTICK_BUTTONS, &Listener::callBackFunc, &listener);
        sleep(1);
        publisher.publish(Politocean::Constants::Topics::JOYSTICK_BUTTONS,"test");
        sleep(5);
        REQUIRE(listener.isUpdated() == true );
        REQUIRE(listener.isTested() == true);
        subscriber.unsubscribeFrom(Politocean::Constants::Topics::JOYSTICK_BUTTONS);
        subscriber.disconnect();
        publisher.disconnect();
    };

    SECTION("Mixed") {
        MqttClient& publisher = MqttClient::getInstance(id_pub, address);
        MqttClient& subscriber = MqttClient::getInstance(id_sub,address);
        Listener listener;
        // check connection
        REQUIRE(publisher.is_connected() == true);
        REQUIRE(subscriber.is_connected() == true);
        subscriber.subscribeTo(Politocean::Constants::Topics::JOYSTICK + "#", &Listener::callBackFunc, &listener);
        sleep(1);
        publisher.publish(Politocean::Constants::Topics::JOYSTICK_BUTTONS,"test");
        sleep(5);
        REQUIRE(listener.isUpdated() == true );
        REQUIRE(listener.isTested() == true);
        subscriber.unsubscribeFrom(Politocean::Constants::Topics::JOYSTICK + "#");
        subscriber.disconnect();
        publisher.disconnect();
    };
}


/**
 * Testing the ROV @ 10.0.0.2
 */
/*TEST_CASE("Testing ROV", "aida"){
    SECTION( "Section 1" ) {
        REQUIRE(0 == 0);
    }
    SECTION( "Section 2" ) {
        REQUIRE(0 == 0);
    }
    SECTION( "Section 3" ) {
        REQUIRE(0 == 0);
    }
}*/

/**
 * Testing the ROV @ 10.0.0.3
 */
/*TEST_CASE("Testing MicroROV", "rino"){
    SECTION( "Section 1" ) {
        REQUIRE(0 == 0);
    }
    SECTION( "Section 2" ) {
        REQUIRE(0 == 0);
    }
    SECTION( "Section 3" ) {
        REQUIRE(0 == 1);
    }
}*/

/*int main(int argc, const char* argv[])
{
    logger::enableLevel(logger::DEBUG, true);
    const std::string id = "custom";
    const std::string address = "127.0.0.1"; // localhost
    const std::string topic_name = "test";

    MqttClient myClient(id,address);
    Listener l;

    
    myClient.subscribeTo(topic_name,&Listener::myFunc, &l);

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