#include <iostream>
#include <unistd.h>

#include "Publisher.h"
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
    Publisher pub("127.0.0.1", "testClientPub");
    mqttLogger ptoLogger(&pub);
    Subscriber sub_errors("127.0.0.1", "prova");
    Subscriber sub_components("127.0.0.1", "TOPIC_COMPONENTS");
    Subscriber sub_messages("127.0.0.1", "TOPIC_MESSAGES");
    
    QApplication a(argc, argv);
    MainWindow gui;

    gui.show();
    sub_errors.subscribeTo(Topics::ERRORS, &MainWindow::messageArrived, &gui);
    sub_components.subscribeTo("topicComponents", &MainWindow::messageArrived, &gui);
    sub_messages.subscribeTo("topicMessages", &MainWindow::messageArrived, &gui);

    sub_errors.connect();
    sub_components.connect();
    sub_messages.connect();

    return a.exec();
}
