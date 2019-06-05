#include <iostream>
#include <unistd.h>

#include "MqttClient.h"

#include "PolitoceanConstants.h"
#include "PolitoceanExceptions.hpp"
#include "mainwindow.h"
#include <QApplication>
#include <thread>

using namespace std;
using namespace Politocean;
using namespace Politocean::Constants;

bool readFromCamera = true;

void retriveFrame(MainWindow* gui){
    while(readFromCamera){
        gui->setFrame(gui->camera.getFrame());
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
    }
}

int main(int argc, char *argv[])
{
    logger::enableLevel(logger::DEBUG);

    MqttClient& subscriber = MqttClient::getInstance(Hmi::GUI_ID, Hmi::IP_ADDRESS);

    QApplication a(argc, argv);

    MainWindow gui;
    std::thread thread(retriveFrame,&gui);
    gui.show();

    subscriber.subscribeTo(Topics::LOGS+"#", &MainWindow::messageArrived, &gui);
    subscriber.subscribeTo(Topics::COMPONENTS, &MainWindow::messageArrived, &gui);
    int code = a.exec();
    readFromCamera = false;
    thread.join();
    return 0;
}
