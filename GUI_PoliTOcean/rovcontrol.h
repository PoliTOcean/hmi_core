#ifndef ROVCONTROL_H
#define ROVCONTROL_H

#include "mainwindow.h"
#include "mqtt/string_collection.h"
#include "mqtt/async_client.h"
#include "rovmqtt.h"
#include <iostream>

class callback : public virtual mqtt::callback,public virtual mqtt::iaction_listener
{
    MainWindow* gui;
    // Counter for the number of connection retries
    int nretry_;
    // The MQTT client
    mqtt::async_client& cli_;
    // Options to use if we need to reconnect
    mqtt::connect_options& connOpts_;
    // An action listener to display the result of actions.
    action_listener subListener_;

    void reconnect() {
        std::this_thread::sleep_for(std::chrono::milliseconds(2500));
        try {
            cli_.connect(connOpts_, nullptr, *this);
        }
        catch (const mqtt::exception& exc) {
            std::cerr << "Error: " << exc.what() << std::endl;
            exit(1);
        }
    }

    // Re-connection failure
    void on_failure(const mqtt::token& tok) override {
        std::cout << "Connection attempt failed" << std::endl;
        reconnect();
    }

    // (Re)connection success
    // Either this or connected() can be used for callbacks.
    void on_success(const mqtt::token& tok) override {}

    // (Re)connection success
    void connected(const std::string& cause) override {
        std::cout << "\nConnection success" << std::endl;

        mqtt::string_collection_ptr collections = std::make_shared<mqtt::string_collection>();
        collections->push_back(ERROR_TOPIC);
        collections->push_back(COMPONENT_TOPIC);
        collections->push_back(MESSAGE_TOPIC);

        mqtt::async_client::qos_collection qos;
        qos.push_back(0);
        qos.push_back(0);
        qos.push_back(0);

        std::cout << "Subscribing.."<<std::endl;

        cli_.subscribe(collections, qos);
        std::cout << "Subscribed to " << ERROR_TOPIC << ", "<< MESSAGE_TOPIC <<", " << COMPONENT_TOPIC <<std::endl;
    }

    // Callback for when the connection is lost.
    // This will initiate the attempt to manually reconnect.
    void connection_lost(const std::string& cause) override {
        std::cout << "\nConnection lost" << std::endl;
        if (!cause.empty())
            std::cout << "\tcause: " << cause << std::endl;

        std::cout << "Reconnecting..." << std::endl;
        nretry_ = 0;
        reconnect();
    }

    // Callback for when a message arrives.
    void message_arrived(mqtt::const_message_ptr msg) override {
        //std::cout << "Message arrived" << std::endl;

        /* INFORMATION ABOUT COMPONENT */
        if(msg->get_topic() == COMPONENT_TOPIC){
            if(msg->get_payload() == JOYSTICK_ON){
                gui->setJoystick(true);
                gui->messageArrived("Joystick connected",1);

            }
            else if(msg->get_payload() == JOYSTICK_OFF){
                gui->setJoystick(false);
                gui->messageArrived("Joystick disconnected",1);
            }
            if(msg->get_payload() == ATMEGA_ON){
                gui->setAtMega(true);
                gui->messageArrived("ATMega connected",1);
            }
            else if(msg->get_payload() == ATMEGA_OFF){
                gui->setAtMega(false);
                gui->messageArrived("ATMega disconnected",1);
            }

        }

        /* ERROR MESSAGE */
        else if(msg->get_topic() == ERROR_TOPIC){
            gui->messageArrived(QString::fromStdString(msg->get_payload()),-1);
        }

        /* COMUNICATION MESSAGE */
        else if(msg->get_topic() == MESSAGE_TOPIC){
            gui->messageArrived(QString::fromStdString(msg->get_payload()),0);
        }

    }

    void delivery_complete(mqtt::delivery_token_ptr token) override {}

    public:
        callback(mqtt::async_client& cli, mqtt::connect_options& connOpts)
    : nretry_(0), cli_(cli), connOpts_(connOpts), subListener_("Subscription") {}

        void setGui(MainWindow* gui){this->gui = gui;}

};


class RovControl
{
public:
    RovControl();
private:
    MainWindow gui;
    mqtt::async_client_ptr client;
    mqtt::connect_options_ptr connOpts;
    callback* cb;
};

#endif // ROVCONTROL_H
