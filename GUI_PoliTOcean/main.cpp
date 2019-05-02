#include "mainwindow.h"
#include <QApplication>

#include <rovcontrol.h>
int main(int argc, char *argv[])
{
    RovControl* rov;
    QApplication a(argc, argv);


    rov = new RovControl();


    return a.exec();
}

