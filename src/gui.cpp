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

void retriveFrame(MainWindow* gui){
    while(true){
        gui->setFrame(gui->camera.getFrame());
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

int main(int argc, char *argv[])
{
    Subscriber subscriber("127.0.0.1", Hmi::GUI_ID);

    QApplication a(argc, argv);

    MainWindow gui;
    std::thread thread(retriveFrame,&gui);
    gui.show();

    subscriber.subscribeTo(Topics::ERRORS, &MainWindow::messageArrived, &gui);
    subscriber.subscribeTo(Topics::COMPONENTS, &MainWindow::messageArrived, &gui);
    subscriber.subscribeTo(Topics::INFO, &MainWindow::messageArrived, &gui);
    subscriber.connect();

    subscriber.connect();


    //thread.join();
    return a.exec();
}

