#include <iostream>
#include <unistd.h>

#include "Subscriber.h"

#include "PolitoceanConstants.h"
#include "PolitoceanExceptions.hpp"
#include "mqttLogger.h"
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
        std::this_thread::sleep_for(std::chrono::milliseconds(41));
    }
}

int main(int argc, char *argv[])
{
    MqttClient subscriber(Hmi::GUI_ID, Hmi::IP_ADDRESS);
    subscriber.connect();

    QApplication a(argc, argv);

    MainWindow gui;
    std::thread thread(retriveFrame,&gui);
    gui.show();

    subscriber.subscribeTo(Topics::ERRORS, &MainWindow::messageArrived, &gui);
    subscriber.subscribeTo(Topics::COMPONENTS, &MainWindow::messageArrived, &gui);
    subscriber.subscribeTo(Topics::INFO, &MainWindow::messageArrived, &gui);

    int code = a.exec();
    readFromCamera = false;
    thread.join();
    return code;
}

