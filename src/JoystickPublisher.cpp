//
// Created by pettinz.
//

#include "JoystickPublisher.h"

using namespace std;

JoystickPublisher::JoystickPublisher(string address, string clientID)
    : address(address), clientID(clientID), TIMEOUT(10)
{
    cout << "Initializing for server '" << address << "'...";
    client = new mqtt::async_client(address, clientID);
    cout << "OK" << endl;
}

void JoystickPublisher::connect()
{
    cout << "\nConnecting...";
    tok = client->connect();
    tok->wait();
    cout << "OK" << endl;
}

void JoystickPublisher::sendMessage(string topic, string payload)
{
    cout << "\nSending message...";
    mqtt::message_ptr pubmsg = mqtt::make_message(topic, payload);
    pubmsg->set_qos(QOS);
    client->publish(pubmsg)->wait_for(TIMEOUT);
    cout << "OK" << endl;
}

void JoystickPublisher::disconnect()
{
    auto toks = client->get_pending_delivery_tokens();
    if (!toks.empty())
        cout << "Error: There are pending delivery tokens." << endl;

    cout << "\nDisconnecting...";
    tok = client->disconnect();
    tok->wait();
    cout << "OK" << endl;
}