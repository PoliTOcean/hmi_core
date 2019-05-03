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
    Subscriber sub("127.0.0.1", "testClientSub");
    
    QApplication a(argc, argv);
    MainWindow gui;

    gui.show();
    sub.subscribeTo("topicTest", &MainWindow::messageArrivedTest, &gui);

    sub.connect();

    return a.exec();
}
