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

pid_t pid1, pid2;

void signal_handler( int signal_num ) {
    logger::getInstance().log(logger::CONFIG, "Exit: signal received: "+to_string(signal_num)+"\n");
    kill(pid2, signal_num);
    kill(pid1, signal_num);
    exit(signal_num);
}

int main(int argc, char *argv[])
{
    logger::enableLevel(logger::DEBUG);

    char *args[]={"",NULL};
    pid1 = fork();
    if (pid1 == 0)
        execvp(args[0]="PolitoceanJoystick",args);
    
    pid2 = fork();
    if (pid2 == 0)
        execvp(args[0]="PolitoceanCommands",args);

    signal(SIGABRT, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    MqttClient& subscriber = MqttClient::getInstance(Hmi::GUI_ID, Hmi::IP_ADDRESS);

    QApplication a(argc, argv);

    MainWindow gui;
    gui.show();

    subscriber.subscribeTo(Topics::LOGS+"#", &MainWindow::messageArrived, &gui);
    subscriber.subscribeTo(Topics::COMPONENTS, &MainWindow::messageArrived, &gui);

    int result = a.exec();

    signal_handler(SIGTERM);

    return result;
}
