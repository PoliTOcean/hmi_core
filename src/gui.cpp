#include <iostream>
#include <unistd.h>

#include "Subscriber.h"

#include "PolitoceanConstants.h"
#include "PolitoceanExceptions.hpp"
#include "mqttLogger.h"
#include "mainwindow.h"
#include <QApplication>

using namespace std;
using namespace Politocean;
using namespace Politocean::Constants;

int main(int argc, char *argv[])
{
    Subscriber subscriber(Hmi::IP_ADDRESS, Hmi::GUI_ID_SUB);
    
    QApplication a(argc, argv);
    MainWindow gui;

    gui.show();
    subscriber.subscribeTo(Topics::ERRORS, &MainWindow::messageArrived, &gui);
    subscriber.subscribeTo(Topics::COMPONENTS, &MainWindow::messageArrived, &gui);
    subscriber.subscribeTo(Topics::INFO, &MainWindow::messageArrived, &gui);
    subscriber.connect();

    return a.exec();
}
