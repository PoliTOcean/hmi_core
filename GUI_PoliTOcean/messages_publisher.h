#ifndef MESSAGES_PUBLISHER_H
#define MESSAGES_PUBLISHER_H

#include <string>
#include "mqtt/async_client.h"
#include "rovmqtt.h"


using namespace std;
class MessagesPublisher
{
public:
    MessagesPublisher(const std::string);
    ~MessagesPublisher();
    void publishMessage(const string topic,const string message);

private:
     mqtt::async_client_ptr client;
};

#endif // MESSAGES_PUBLISHER_H
