//
// Created by pettinz.
//

#ifndef JOYSTICKPUBLISHER_JOYSTICKPUBLISHER_H
#define JOYSTICKPUBLISHER_JOYSTICKPUBLISHER_H

#include <string>

#include "mqtt/async_client.h"

class JoystickPublisher {
    std::string address, clientID;
    mqtt::async_client *client;
    mqtt::token_ptr tok;

public:
    const int QOS = 1;
    const std::chrono::seconds TIMEOUT;

    /**
     * Creates new client with @clientID listening on a server with address @address
     */
    JoystickPublisher(std::string address, std::string clientID);

    /*
     * Connects the client to the server.
     */
    void connect();
    /*
     * Disconnects the client from the server.
     */
    void disconnect();

    /**
     * Sends a message @payload to the topic @topic.
     */
    void sendMessage(std::string topic, std::string payload);
};


#endif //JOYSTICKPUBLISHER_JOYSTICKPUBLISHER_H
