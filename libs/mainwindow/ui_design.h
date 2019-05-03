/********************************************************************************
** Form generated from reading UI file 'design.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DESIGN_H
#define UI_DESIGN_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QLabel *display_image;
    QLabel *display_image_2;
    QPushButton *startVideo;
    QPushButton *joystick_status;
    QPushButton *atMega_status;
    QLabel *gradient;
    QLabel *gradient_2;
    QPushButton *error_video;
    QLabel *gradient_3;
    QPushButton *auto_drive;
    QPushButton *shapes_recognize;
    QLabel *gradient_4;
    QLabel *gradient_5;
    QLabel *gradient_6;
    QPushButton *home;
    QSlider *trackbar_circle;
    QLCDNumber *lcdNumberTrackbar;
    QLabel *logoLabel;
    QPushButton *cannon_measure;
    QLabel *gradient_7;
    QLabel *gradient_8;
    QPushButton *term_sensor;
    QTextEdit *console;
    QPushButton *home_3;
    QLabel *gradient_9;
    QPushButton *measure_button;
    QLabel *gradient_10;
    QLabel *gridLabel;
    QSlider *trackbar_circle_2;
    QLCDNumber *lcdNumberTrackbar_2;
    QLCDNumber *lcdNumberTrackbar_3;
    QSlider *trackbar_circle_3;
    QCheckBox *debugCheck;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QStringLiteral("MainWindow"));
        MainWindow->resize(1300, 701);
        MainWindow->setStyleSheet(QLatin1String("QPushButton{\n"
"background-color:white;\n"
"border-style: solid;\n"
"\n"
"border-width: 1px;\n"
"border-radius: 7px;\n"
"}\n"
"QMainWindow{\n"
"	background-color: #37474f;\n"
"}"));
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        centralWidget->setStyleSheet(QStringLiteral(""));
        display_image = new QLabel(centralWidget);
        display_image->setObjectName(QStringLiteral("display_image"));
        display_image->setGeometry(QRect(20, 70, 640, 480));
        display_image->setStyleSheet(QLatin1String("border-style: solid;\n"
"border-color: #00838f ;\n"
"border-width: 3px;\n"
""));
        display_image_2 = new QLabel(centralWidget);
        display_image_2->setObjectName(QStringLiteral("display_image_2"));
        display_image_2->setGeometry(QRect(670, 70, 351, 480));
        display_image_2->setStyleSheet(QLatin1String("border-style: solid;\n"
"border-color: #00838f ;\n"
"border-width: 3px;"));
        startVideo = new QPushButton(centralWidget);
        startVideo->setObjectName(QStringLiteral("startVideo"));
        startVideo->setGeometry(QRect(20, 640, 131, 54));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        startVideo->setFont(font);
        startVideo->setStyleSheet(QLatin1String("background-color:#eeeeee;\n"
"color: black;\n"
"border-color: white;"));
        joystick_status = new QPushButton(centralWidget);
        joystick_status->setObjectName(QStringLiteral("joystick_status"));
        joystick_status->setGeometry(QRect(20, 560, 61, 61));
        joystick_status->setStyleSheet(QLatin1String("QPushButton\n"
"{\n"
"	background-color: #64dd17; \n"
"\n"
"}"));
        atMega_status = new QPushButton(centralWidget);
        atMega_status->setObjectName(QStringLiteral("atMega_status"));
        atMega_status->setGeometry(QRect(90, 560, 61, 61));
        atMega_status->setStyleSheet(QLatin1String("QPushButton\n"
"{\n"
"	background-color: #c62828;\n"
"}"));
        gradient = new QLabel(centralWidget);
        gradient->setObjectName(QStringLiteral("gradient"));
        gradient->setGeometry(QRect(20, 570, 61, 57));
        gradient->setStyleSheet(QLatin1String("background: qlineargradient(spread:pad, x1:0, y1:0.784, x2:0, y2:1, stop:0 rgba(0, 0, 0, 255), stop:1 rgba(55, 71, 76, 255));\n"
"\n"
"border-width: 1px;\n"
"border-radius: 10px;"));
        gradient_2 = new QLabel(centralWidget);
        gradient_2->setObjectName(QStringLiteral("gradient_2"));
        gradient_2->setGeometry(QRect(90, 570, 61, 57));
        gradient_2->setStyleSheet(QLatin1String("background: qlineargradient(spread:pad, x1:0, y1:0.784, x2:0, y2:1, stop:0 rgba(0, 0, 0, 255), stop:1 rgba(55, 71, 79, 255));\n"
"\n"
"border-width: 1px;\n"
"border-radius: 10px;"));
        error_video = new QPushButton(centralWidget);
        error_video->setObjectName(QStringLiteral("error_video"));
        error_video->setGeometry(QRect(130, 110, 400, 400));
        error_video->setStyleSheet(QStringLiteral("background-color: rgba(0,0,0,0);"));
        gradient_3 = new QLabel(centralWidget);
        gradient_3->setObjectName(QStringLiteral("gradient_3"));
        gradient_3->setGeometry(QRect(20, 640, 131, 61));
        gradient_3->setStyleSheet(QLatin1String("background: qlineargradient(spread:pad, x1:0, y1:0.784, x2:0, y2:1, stop:0 rgba(0, 0, 0, 255), stop:1 rgba(55, 71, 76, 255));\n"
"\n"
"border-width: 1px;\n"
"border-radius: 10px;"));
        auto_drive = new QPushButton(centralWidget);
        auto_drive->setObjectName(QStringLiteral("auto_drive"));
        auto_drive->setGeometry(QRect(320, 560, 141, 131));
        auto_drive->setStyleSheet(QLatin1String("QPushButton\n"
"{\n"
"	background-color: #00838f ; \n"
"\n"
"}"));
        shapes_recognize = new QPushButton(centralWidget);
        shapes_recognize->setObjectName(QStringLiteral("shapes_recognize"));
        shapes_recognize->setGeometry(QRect(470, 560, 131, 131));
        shapes_recognize->setStyleSheet(QLatin1String("QPushButton\n"
"{\n"
"	background-color: #00838f ; \n"
"\n"
"}"));
        gradient_4 = new QLabel(centralWidget);
        gradient_4->setObjectName(QStringLiteral("gradient_4"));
        gradient_4->setGeometry(QRect(320, 560, 141, 141));
        gradient_4->setStyleSheet(QLatin1String("background: qlineargradient(spread:pad, x1:0, y1:0.784, x2:0, y2:1, stop:0 rgba(0, 0, 0, 255), stop:1 rgba(55, 71, 76, 255));\n"
"\n"
"border-width: 1px;\n"
"border-radius: 10px;"));
        gradient_5 = new QLabel(centralWidget);
        gradient_5->setObjectName(QStringLiteral("gradient_5"));
        gradient_5->setGeometry(QRect(470, 560, 131, 141));
        gradient_5->setStyleSheet(QLatin1String("background: qlineargradient(spread:pad, x1:0, y1:0.784, x2:0, y2:1, stop:0 rgba(0, 0, 0, 255), stop:1 rgba(55, 71, 76, 255));\n"
"\n"
"border-width: 1px;\n"
"border-radius: 10px;"));
        gradient_6 = new QLabel(centralWidget);
        gradient_6->setObjectName(QStringLiteral("gradient_6"));
        gradient_6->setGeometry(QRect(170, 560, 141, 141));
        gradient_6->setStyleSheet(QLatin1String("background: qlineargradient(spread:pad, x1:0, y1:0.784, x2:0, y2:1, stop:0 rgba(0, 0, 0, 255), stop:1 rgba(55, 71, 76, 255));\n"
"\n"
"border-width: 1px;\n"
"border-radius: 10px;"));
        home = new QPushButton(centralWidget);
        home->setObjectName(QStringLiteral("home"));
        home->setGeometry(QRect(170, 560, 141, 131));
        home->setStyleSheet(QLatin1String("QPushButton\n"
"{\n"
"	background-color: #00838f ; \n"
"\n"
"}"));
        trackbar_circle = new QSlider(centralWidget);
        trackbar_circle->setObjectName(QStringLiteral("trackbar_circle"));
        trackbar_circle->setGeometry(QRect(1100, 120, 121, 51));
        trackbar_circle->setOrientation(Qt::Horizontal);
        lcdNumberTrackbar = new QLCDNumber(centralWidget);
        lcdNumberTrackbar->setObjectName(QStringLiteral("lcdNumberTrackbar"));
        lcdNumberTrackbar->setGeometry(QRect(1233, 130, 51, 31));
        logoLabel = new QLabel(centralWidget);
        logoLabel->setObjectName(QStringLiteral("logoLabel"));
        logoLabel->setGeometry(QRect(570, -90, 691, 321));
        cannon_measure = new QPushButton(centralWidget);
        cannon_measure->setObjectName(QStringLiteral("cannon_measure"));
        cannon_measure->setGeometry(QRect(610, 560, 141, 131));
        cannon_measure->setStyleSheet(QLatin1String("QPushButton\n"
"{\n"
"	background-color: #00838f ; \n"
"\n"
"}"));
        gradient_7 = new QLabel(centralWidget);
        gradient_7->setObjectName(QStringLiteral("gradient_7"));
        gradient_7->setGeometry(QRect(610, 560, 141, 141));
        gradient_7->setStyleSheet(QLatin1String("background: qlineargradient(spread:pad, x1:0, y1:0.784, x2:0, y2:1, stop:0 rgba(0, 0, 0, 255), stop:1 rgba(55, 71, 76, 255));\n"
"\n"
"border-width: 1px;\n"
"border-radius: 10px;"));
        gradient_8 = new QLabel(centralWidget);
        gradient_8->setObjectName(QStringLiteral("gradient_8"));
        gradient_8->setGeometry(QRect(1220, 390, 50, 45));
        gradient_8->setStyleSheet(QLatin1String("background: qlineargradient(spread:pad, x1:0, y1:0.784, x2:0, y2:1, stop:0 rgba(0, 0, 0, 255), stop:1 rgba(55, 71, 76, 255));\n"
"\n"
"border-width: 1px;\n"
"border-radius: 10px;"));
        term_sensor = new QPushButton(centralWidget);
        term_sensor->setObjectName(QStringLiteral("term_sensor"));
        term_sensor->setGeometry(QRect(1220, 380, 50, 50));
        term_sensor->setStyleSheet(QLatin1String("QPushButton\n"
"{\n"
"	background-color: #00838f ; \n"
"\n"
"}"));
        console = new QTextEdit(centralWidget);
        console->setObjectName(QStringLiteral("console"));
        console->setGeometry(QRect(770, 560, 521, 131));
        console->setStyleSheet(QLatin1String("border-style: solid;\n"
"border-color: #00838f ;\n"
"border-width: 1px;"));
        home_3 = new QPushButton(centralWidget);
        home_3->setObjectName(QStringLiteral("home_3"));
        home_3->setGeometry(QRect(1220, 310, 50, 50));
        home_3->setStyleSheet(QLatin1String("QPushButton\n"
"{\n"
"	background-color: #00838f ; \n"
"\n"
"}"));
        gradient_9 = new QLabel(centralWidget);
        gradient_9->setObjectName(QStringLiteral("gradient_9"));
        gradient_9->setGeometry(QRect(1220, 320, 50, 45));
        gradient_9->setStyleSheet(QLatin1String("background: qlineargradient(spread:pad, x1:0, y1:0.784, x2:0, y2:1, stop:0 rgba(0, 0, 0, 255), stop:1 rgba(55, 71, 76, 255));\n"
"\n"
"border-width: 1px;\n"
"border-radius: 10px;"));
        measure_button = new QPushButton(centralWidget);
        measure_button->setObjectName(QStringLiteral("measure_button"));
        measure_button->setGeometry(QRect(1180, 460, 91, 81));
        measure_button->setStyleSheet(QLatin1String("QPushButton\n"
"{\n"
"	background-color: #00838f ; \n"
"\n"
"}"));
        gradient_10 = new QLabel(centralWidget);
        gradient_10->setObjectName(QStringLiteral("gradient_10"));
        gradient_10->setGeometry(QRect(1180, 460, 91, 91));
        gradient_10->setStyleSheet(QLatin1String("background: qlineargradient(spread:pad, x1:0, y1:0.784, x2:0, y2:1, stop:0 rgba(0, 0, 0, 255), stop:1 rgba(55, 71, 76, 255));\n"
"\n"
"border-width: 1px;\n"
"border-radius: 10px;"));
        gridLabel = new QLabel(centralWidget);
        gridLabel->setObjectName(QStringLiteral("gridLabel"));
        gridLabel->setGeometry(QRect(680, 240, 400, 311));
        trackbar_circle_2 = new QSlider(centralWidget);
        trackbar_circle_2->setObjectName(QStringLiteral("trackbar_circle_2"));
        trackbar_circle_2->setGeometry(QRect(1100, 190, 121, 51));
        trackbar_circle_2->setOrientation(Qt::Horizontal);
        lcdNumberTrackbar_2 = new QLCDNumber(centralWidget);
        lcdNumberTrackbar_2->setObjectName(QStringLiteral("lcdNumberTrackbar_2"));
        lcdNumberTrackbar_2->setGeometry(QRect(1233, 200, 51, 31));
        lcdNumberTrackbar_3 = new QLCDNumber(centralWidget);
        lcdNumberTrackbar_3->setObjectName(QStringLiteral("lcdNumberTrackbar_3"));
        lcdNumberTrackbar_3->setGeometry(QRect(1233, 270, 51, 31));
        trackbar_circle_3 = new QSlider(centralWidget);
        trackbar_circle_3->setObjectName(QStringLiteral("trackbar_circle_3"));
        trackbar_circle_3->setGeometry(QRect(1100, 260, 121, 51));
        trackbar_circle_3->setOrientation(Qt::Horizontal);
        debugCheck = new QCheckBox(centralWidget);
        debugCheck->setObjectName(QStringLiteral("debugCheck"));
        debugCheck->setGeometry(QRect(20, 40, 92, 23));
        MainWindow->setCentralWidget(centralWidget);
        gridLabel->raise();
        display_image->raise();
        display_image_2->raise();
        gradient->raise();
        joystick_status->raise();
        gradient_2->raise();
        atMega_status->raise();
        error_video->raise();
        gradient_3->raise();
        startVideo->raise();
        gradient_4->raise();
        gradient_5->raise();
        shapes_recognize->raise();
        auto_drive->raise();
        gradient_6->raise();
        home->raise();
        lcdNumberTrackbar->raise();
        gradient_7->raise();
        cannon_measure->raise();
        gradient_8->raise();
        term_sensor->raise();
        trackbar_circle->raise();
        gradient_9->raise();
        home_3->raise();
        console->raise();
        gradient_10->raise();
        measure_button->raise();
        trackbar_circle_2->raise();
        lcdNumberTrackbar_2->raise();
        lcdNumberTrackbar_3->raise();
        trackbar_circle_3->raise();
        debugCheck->raise();
        logoLabel->raise();

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", Q_NULLPTR));
        display_image->setText(QString());
        display_image_2->setText(QString());
        startVideo->setText(QApplication::translate("MainWindow", "Start Video", Q_NULLPTR));
        joystick_status->setText(QString());
        atMega_status->setText(QString());
        gradient->setText(QString());
        gradient_2->setText(QString());
        error_video->setText(QString());
        gradient_3->setText(QString());
        auto_drive->setText(QString());
        shapes_recognize->setText(QString());
        gradient_4->setText(QString());
        gradient_5->setText(QString());
        gradient_6->setText(QString());
        home->setText(QString());
        logoLabel->setText(QString());
        cannon_measure->setText(QString());
        gradient_7->setText(QString());
        gradient_8->setText(QString());
        term_sensor->setText(QString());
        console->setHtml(QApplication::translate("MainWindow", "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-weight:600; color:#204a87;\">-- PoliTOcean Carassius v2.0:</span></p>\n"
"<p style=\"-qt-paragraph-type:empty; margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><br /></p></body></html>", Q_NULLPTR));
        home_3->setText(QString());
        gradient_9->setText(QString());
        measure_button->setText(QString());
        gradient_10->setText(QString());
        gridLabel->setText(QString());
        debugCheck->setText(QApplication::translate("MainWindow", "Debug", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DESIGN_H
