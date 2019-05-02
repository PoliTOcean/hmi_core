#include "messages_publisher.h"


MessagesPublisher::MessagesPublisher(const std::string CLIENT_ID)
{
    /* CREATE CLIENT */
    client = make_shared<mqtt::async_client>(HOST_ROV,CLIENT_ID);

    try {
        std::cout << "Mi sto connettendo.." << endl;
        client->connect();
    }
    catch (const mqtt::exception& exc) {
        std::cerr << "Error: " << exc.what() << " ["
            << exc.get_reason_code() << "]" << std::endl;
    }
}


MessagesPublisher::~MessagesPublisher()
{
    /* DISCONNECT CLIENT */
    try{
        client->disconnect();
    }
    catch (const mqtt::exception& exc) {
        std::cerr << "Error: " << exc.what() << " ["
            << exc.get_reason_code() << "]" << std::endl;
    }

}

void MessagesPublisher::publishMessage(const string topic,const string message)
{
    /* PUBLISH MESSAGE */
    try {

        client->publish(topic,message.c_str(), strlen(message.c_str()),0,false);
    }
    catch (const mqtt::exception& exc) {
        std::cerr << "Error: " << exc.what() << " ["
            << exc.get_reason_code() << "]" << std::endl;
    }

}
