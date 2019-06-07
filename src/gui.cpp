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

int main(int argc, char *argv[])
{
    logger::enableLevel(logger::DEBUG);

    MqttClient& subscriber = MqttClient::getInstance(Hmi::GUI_ID, Hmi::IP_ADDRESS);

    QApplication a(argc, argv);

    MainWindow gui;
    gui.show();

    subscriber.subscribeTo(Topics::LOGS+"#", &MainWindow::messageArrived, &gui);
    subscriber.subscribeTo(Topics::COMPONENTS, &MainWindow::messageArrived, &gui);

    return a.exec();
}
