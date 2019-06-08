#include <iostream>
#include <unistd.h>

#include "MqttClient.h"

#include "PolitoceanConstants.h"
#include "PolitoceanExceptions.hpp"
#include "mainwindow.h"
#include <QApplication>
#include <thread>
#include <unistd.h>
#include <signal.h>

using namespace std;
using namespace Politocean;
using namespace Politocean::Constants;

int main(int argc, char *argv[])
{
    logger::enableLevel(logger::DEBUG);

    pid_t pid1, pid2;
    char *args[]={"",NULL};
    pid1 = fork();
    if (pid1 == 0)
        execvp(args[0]="PolitoceanJoystick",args);
    
    pid2 = fork();
    if (pid2 == 0)
        execvp(args[0]="PolitoceanCommands",args);

    MqttClient& subscriber = MqttClient::getInstance(Hmi::GUI_ID, Hmi::IP_ADDRESS);

    QApplication a(argc, argv);

    MainWindow gui;
    gui.show();

    subscriber.subscribeTo(Topics::LOGS+"#", &MainWindow::messageArrived, &gui);
    subscriber.subscribeTo(Topics::COMPONENTS, &MainWindow::messageArrived, &gui);

    int result = a.exec();

    kill(pid2, SIGTERM);
    kill(pid1, SIGTERM);

    return result;
}
