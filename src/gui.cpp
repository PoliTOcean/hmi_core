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
    Publisher pub("127.0.0.1", Hmi::CLIENT_ID);
    mqttLogger ptoLogger(&pub);
    
    QApplication a(argc, argv);
    MainWindow gui;

    gui.show();

    return a.exec();
}
