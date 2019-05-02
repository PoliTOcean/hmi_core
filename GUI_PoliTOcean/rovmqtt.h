#ifndef ROVMQTT_H
#define ROVMQTT_H

#include <iostream>
#include "mqtt/string_collection.h"
#include "mqtt/async_client.h"

/* INIT MQTT MESSENGER */
const std::string HOST_ROV("127.0.0.1:1883");
const std::string MESSENGER_ID("messenger");
const std::string LISTENER_ID("listener");
const std::string GUI_ID("gui_sub");
const std::string UI_ID("gui_pub");
const std::string AUTODRIVE_ID("autodrive");

/* TOPIC NAMES */
const std::string TEMPERATURE_TOPIC("temperature");
const std::string ERROR_TOPIC("errors");
const std::string MESSAGE_TOPIC("messages");
const std::string COMPONENT_TOPIC("components");
const std::string AUTODRIVE_TOPIC("auto_drive");

/* STATUS FOR COMPONENTS */
const std::string ATMEGA_ON("atmega_on");
const std::string ATMEGA_OFF("atmega_off");

const std::string JOYSTICK_ON("joystick_on");
const std::string JOYSTICK_OFF("joystick_off");

class action_listener : public virtual mqtt::iaction_listener
{
    std::string name_;

    void on_failure(const mqtt::token& tok) override {
        std::cout << name_ << " failure";
        if (tok.get_message_id() != 0)
            std::cout << " for token: [" << tok.get_message_id() << "]" << std::endl;
        std::cout << std::endl;
    }

    void on_success(const mqtt::token& tok) override {
        std::cout << name_ << " success";
        if (tok.get_message_id() != 0)
            std::cout << " for token: [" << tok.get_message_id() << "]" << std::endl;
        auto top = tok.get_topics();
        if (top && !top->empty())
            std::cout << "\ttoken topic: '" << (*top)[0] << "', ..." << std::endl;
        std::cout << std::endl;
    }

public:
    action_listener(const std::string& name) : name_(name) {}
};


#endif // ROVMQTT_H
