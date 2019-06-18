#include "mainwindow.h"
#include "ui_design.h"
#include <QTimer>
#include "vision.h"
#include "ipcamera.h"
#include <iostream>
#include <sstream>
#include "PolitoceanConstants.h"
#include "Serial.hpp"
#include "ComponentsManager.hpp"
#include <mutex>

#define sizeIconMenu 80
#define sizeIconComponent 30

using namespace Politocean;
using namespace Politocean::Constants;

std::mutex mtx;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    camera( std::bind(&MainWindow::setFrame, this, std::placeholders::_1), 2 ),
    ui(new Ui::MainWindow)
{
    /* SETUP UI*/
    ui->setupUi(this);
    
    //INIT PRIVATE VARIABLE
    video = false;

    /*TIMER DISPLAY CAMERAS*/
    connect(this, SIGNAL(frameArrived()), this, SLOT(DisplayImage()));
    connect(this, SIGNAL(sensorsUpdating()),this, SLOT(setSensorsLabel()));

    /*CONNECTION BUTTONS*/
    connect(ui->startVideo,SIGNAL(clicked()),SLOT(setVideoStart()));
    connect(ui->auto_drive,SIGNAL(clicked()),SLOT(modeAuto()));
    connect(ui->home,SIGNAL(clicked()),SLOT(modeHome()));
    connect(ui->shapes_recognize,SIGNAL(clicked()),SLOT(modeShapes()));
    connect(ui->measure_button,SIGNAL(clicked()),SLOT(startMeasure()));
    connect(ui->ph_button,SIGNAL(clicked()),SLOT(phRead()));

    /* WIDGET CONNECTIONS */
    connect(ui->trackbar_circle,SIGNAL(valueChanged(int)),this,SLOT(valueTrackbar(int)));
    connect(this,SIGNAL(messageArrived(QString,int)),this,SLOT(setMessageConsole(QString,int)));

    //LOAD ICONS BUTTONS
    icon.addFile(QString::fromUtf8("images/microchip-solid_w.png"), QSize(), QIcon::Normal, QIcon::Off);
    icon2.addFile(QString::fromUtf8("images/gamepad-solid_w.png"), QSize(), QIcon::Normal, QIcon::Off);
    video_icon.addFile(QString::fromUtf8("images/video_error.png"), QSize(), QIcon::Normal, QIcon::Off);
    auto_icon.addFile(QString::fromUtf8("images/robot-solid.png"), QSize(), QIcon::Normal, QIcon::Off);
    shapes_icon.addFile(QString::fromUtf8("images/shapes-solid.png"), QSize(), QIcon::Normal, QIcon::Off);
    home_icon.addFile(QString::fromUtf8("images/home.png"), QSize(), QIcon::Normal, QIcon::Off);
    auto_icon_w.addFile(QString::fromUtf8("images/robot-solid_w.png"), QSize(), QIcon::Normal, QIcon::Off);
    shapes_icon_w.addFile(QString::fromUtf8("images/shapes-solid_w.png"), QSize(), QIcon::Normal, QIcon::Off);
    home_icon_w.addFile(QString::fromUtf8("images/home_w.png"), QSize(), QIcon::Normal, QIcon::Off);
    cannon_icon.addFile(QString::fromUtf8("images/cannon.png"), QSize(), QIcon::Normal, QIcon::Off);
    term_icon.addFile(QString::fromUtf8("images/thermometer.png"), QSize(), QIcon::Normal, QIcon::Off);
    depth_icon.addFile(QString::fromUtf8("images/depth.png"), QSize(), QIcon::Normal, QIcon::Off);
    ph_icon.addFile(QString::fromUtf8("images/ph_icon.png"), QSize(), QIcon::Normal, QIcon::Off);
    shoulder_icon.addFile(QString::fromUtf8("images/arm_icon.png"), QSize(), QIcon::Normal, QIcon::Off);

    QPixmap pix;
    if(pix.load("images/LogoOcean.png")){
        /* scale pixmap to fit in label'size and keep ratio of pixmap */
        pix = pix.scaled(ui->logoLabel->size(),Qt::KeepAspectRatio);
        ui->logoLabel->setPixmap(pix);
    }

    //SET ICON
    ui->joystick_status->setIcon(icon2);
    ui->joystick_status->setIconSize(QSize(sizeIconComponent, sizeIconComponent));
    ui->power_status->setIcon(icon);
    ui->power_status->setIconSize(QSize(sizeIconComponent, sizeIconComponent));
    ui->shoulder_status->setIcon(shoulder_icon);
    ui->shoulder_status->setIconSize(QSize(sizeIconComponent, sizeIconComponent));
    ui->error_video->setIcon(video_icon);
    ui->error_video->setIconSize(QSize(sizeIconMenu,sizeIconMenu));
    ui->auto_drive->setIcon(auto_icon);
    ui->auto_drive->setIconSize(QSize(sizeIconMenu,sizeIconMenu));
    ui->shapes_recognize->setIcon(shapes_icon);
    ui->shapes_recognize->setIconSize(QSize(sizeIconMenu,sizeIconMenu));
    ui->home->setIcon(home_icon_w);
    ui->home->setIconSize(QSize(sizeIconMenu,sizeIconMenu));
    ui->cannon_measure->setIcon(cannon_icon);
    ui->cannon_measure->setIconSize(QSize(sizeIconMenu,sizeIconMenu));
    ui->term_sensor->setIcon(term_icon);
    ui->term_sensor->setIconSize(QSize(sizeIconComponent,sizeIconComponent));
    ui->depth_button->setIcon(depth_icon);
    ui->depth_button->setIconSize(QSize(sizeIconComponent,sizeIconComponent));
    ui->ph_button->setIcon(ph_icon);
    ui->ph_button->setIconSize(QSize(sizeIconComponent,sizeIconComponent));

    //-> setting TRACKBAR
    ui->trackbar_circle->setMaximum(255);

    value_track = 0;
    lenght_blue = 0;
    num_average_lenght = 0;
    snap_b = false;
    ph_read = false;

    setVideoStart();
}

MainWindow::~MainWindow()
{
    camera.stop();
    delete ui;
}

void MainWindow::phMeasure(MainWindow* gui)
{
    std::string ph;
    Serial serial("tty/ACM0");
    while(gui->ph_read){
        serial.readLine(ph);
        std::string delimiter = ";";
        std::string ph_string = ph.substr(1, ph.find(delimiter));
        std::string temp_string = ph.substr(2, ph.find(delimiter));
        gui->ui->ph_label->setText("PH: "+QString::fromStdString(ph_string));
        gui->ui->temp_label->setText("Temp "+QString::fromStdString(temp_string));
    }
}

void MainWindow::phRead()
{
    if(!ph_read){
        ph_thread = new std::thread(phMeasure,this);
    }
    else{
        ph_read = false;
    }

}
void MainWindow::setFrame(const cv::Mat frame)
{
    std::lock_guard<std::mutex> lock(mtx);
    img = frame.clone();
    this->frameArrived();
}

void MainWindow::DisplayImage(){
    Mat img_hls;
    if(!video || img.empty())
        return;

    Mat frame, frame_rsz;

    std::lock_guard<std::mutex> lock(mtx);
    cvtColor(img, frame_rsz, CV_BGR2RGB);
    cvtColor(img,img_hls,CV_BGR2HLS);
    mtx.unlock();

    cv::resize(frame_rsz, frame, cv::Size(1024,720));

    if(mode == MODE::MODE_AUTO){
        //img = Vision::addCircle(frame,value_track);
        std::lock_guard<std::mutex> lock(mtx);
        QImage cam1((uchar*)img.data, img.cols, img.rows, img.step, QImage::Format_RGB888);
        mtx.unlock();

        ui->display_image->setPixmap(QPixmap::fromImage(cam1));

        if(Vision::checkCenter(img_hls)){
            num_average_lenght++;
            lenght_blue += Vision::getLenghtFromCenter(img_hls);
            if(num_average_lenght > 10){
                modeHome();
                std::cout<< lenght_blue  << std::endl;
            }
        }
    }

    else if(mode  == MODE::MODE_HOME){
        //VISION TEST:
        if(ui->debugCheck->isChecked()){
            cv::Mat img_hls;

            std::lock_guard<std::mutex> lock(mtx);
            cvtColor(img, img_hls, CV_BGR2HLS);
            mtx.unlock();

            cv::Mat filtered = Vision::filterRed(img_hls);
            QImage cam1((uchar*)filtered.data, filtered.cols, filtered.rows, filtered.step, QImage::Format_Grayscale8);
            ui->display_image->setPixmap(QPixmap::fromImage(cam1));
        }
        else{
            QImage cam1((uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
            ui->display_image->setPixmap(QPixmap::fromImage(cam1));
        }
    }

    else if(mode == MODE::MODE_SHAPES){
        /*
        //img = Vision::getImageBlackShape(frame,value_track);
        QImage cam1((uchar*)img.data, img.cols, img.rows, img.step, QImage::Format_Grayscale8);
        ui->display_image->setPixmap(QPixmap::fromImage(cam1));

        if(snap_b){
            cv::Mat res;
            ui->display_image_2->setVisible(true);
            //res = Vision::getshape(img,value_track);
            QImage cam2((uchar*)res.data, res.cols, res.rows, res.step, QImage::Format_RGB888);
            ui->display_image_2->setPixmap(QPixmap::fromImage(cam2));
            snap_b = false;
        }
        */
    }
}

void MainWindow::setVideoStart()
{
    //TOOGLE START VIDEO
    video = !video;
    if(video){
        camera.start();
        ui->error_video->setIcon(QIcon());
    }
    else{
        camera.stop();
        ui->error_video->setIcon(video_icon);
        ui->error_video->setIconSize(QSize(sizeIconMenu,sizeIconMenu));
    }
}

void MainWindow::setMessageConsole(QString msg,int type)
{
    QString color_red,color_yellow,color_black;
    QString label;

    if(type == -1){
        label = "[ERROR]: ";
        color_red ="<span style=\"font-weight:600; color:#a40000;\">";
        color_red.append(label);
        color_red.append("</span>");
        color_red.append(msg);
        ui->console->append(color_red);
    }

    else if(type == 0){
        label = "[INFO]: ";
        color_black = "<span style=\"font-weight:600;\">";
        color_black.append(label);
        color_black.append("</span>");
        color_black.append(msg);
        ui->console->append(color_black);
    }

    else if(type == 1){
        label = "[WARNING]: ";
        color_yellow = "<span style=\"font-weight:600;color:#204a87;\">";
        color_yellow.append(label);
        color_yellow.append("</span>");
        color_yellow.append(msg);
        ui->console->append(color_yellow);
    }

    ui->console->scrollBarWidgets(Qt::AlignBottom);
}

void MainWindow::modeAuto()
{
    //autodrive.reset();
    mode = MODE::MODE_AUTO;
    ui->auto_drive->setIcon(auto_icon_w);
    ui->auto_drive->setIconSize(QSize(sizeIconMenu,sizeIconMenu));

    ui->home->setIcon(home_icon);
    ui->home->setIconSize(QSize(sizeIconMenu,sizeIconMenu));
    ui->shapes_recognize->setIcon(shapes_icon);
    ui->shapes_recognize->setIconSize(QSize(sizeIconMenu,sizeIconMenu));

    ui->gridLabel->setVisible(true);
    ui->display_image_2->setVisible(false);
}

void MainWindow::modeShapes()
{
    mode = MODE::MODE_SHAPES;

    /* DRAW WHITE ICON */
    ui->shapes_recognize->setIcon(shapes_icon_w);
    ui->shapes_recognize->setIconSize(QSize(sizeIconMenu,sizeIconMenu));
    ui->auto_drive->setIcon(auto_icon);
    ui->auto_drive->setIconSize(QSize(sizeIconMenu,sizeIconMenu));
    ui->home->setIcon(home_icon);
    ui->home->setIconSize(QSize(sizeIconMenu,sizeIconMenu));

    ui->gridLabel->setVisible(false);
    ui->display_image_2->setVisible(false);
}

void MainWindow::modeHome()
{
    mode = MODE::MODE_HOME;

    lenght_blue = 0;

    ui->home->setIcon(home_icon_w);
    ui->home->setIconSize(QSize(sizeIconMenu,sizeIconMenu));

    ui->shapes_recognize->setIcon(shapes_icon);
    ui->shapes_recognize->setIconSize(QSize(sizeIconMenu,sizeIconMenu));
    ui->auto_drive->setIcon(auto_icon);
    ui->auto_drive->setIconSize(QSize(sizeIconMenu,sizeIconMenu));

    ui->gridLabel->setVisible(false);
    ui->display_image_2->setVisible(false);

}

void MainWindow::setJoystick(bool connected)
{

    if(connected){
        ui->joystick_status->setStyleSheet("QPushButton{background-color: #64dd17; }");
    }
    else{
        ui->joystick_status->setStyleSheet("QPushButton{background-color: #c62828; }");
    }

}

void MainWindow::setAtMega(bool connected)
{

}

void MainWindow::valueTrackbar(int value)
{
    value_track = value;
    ui->lcdNumberTrackbar->display(value);
}

void MainWindow::startMeasure()
{
    snap_b = true;

    if(mode  == MODE::MODE_HOME){
    }
}

void MainWindow::messageArrived(const std::string& payload, const std::string& topic){

    /* ERROR MESSAGE */
    if(topic == Logger::LOGS_PATH + Logger::Levels::ERROR){
        this->messageArrived(QString::fromStdString(payload),0);
    }

    else if(topic == Logger::LOGS_PATH + Logger::Levels::INFO){
        this->messageArrived(QString::fromStdString(payload),-1);
    }

    else if(topic == Logger::LOGS_PATH + Logger::Levels::WARNING){
        this->messageArrived(QString::fromStdString(payload),1);
    }
}

void MainWindow::sensorArrived(Types::Vector<Sensor<float>> payload){
    this->sensors_ = payload;
    this->sensorsUpdating();
}


void MainWindow::componentArrived(const std::string& payload, const std::string& topic)
{
    this->componentChanged();
}

void MainWindow::setComponentStatus()
{
        if(ComponentsManager::GetComponentState(component_t::JOYSTICK) == Component::Status::ENABLED){
            ui->joystick_status->setStyleSheet("QPushButton{background-color: #64dd17; }");
        }
        else{
            ui->joystick_status->setStyleSheet("QPushButton{background-color: #c62828; }");
        }

        if(ComponentsManager::GetComponentState(component_t::SHOULDER) == Component::Status::ENABLED){
            ui->shoulder_status->setStyleSheet("QPushButton{background-color: #64dd17; }");
        }
        else{
            ui->shoulder_status->setStyleSheet("QPushButton{background-color: #c62828; }");
        }

        if(ComponentsManager::GetComponentState(component_t::POWER) == Component::Status::ENABLED){
            ui->power_status->setStyleSheet("QPushButton{background-color: #64dd17; }");
        }
        else{
            ui->power_status->setStyleSheet("QPushButton{background-color: #c62828; }");
        }
}


void MainWindow::setSensorsLabel()
{
    for(Sensor<float> s : sensors_){
        if(s.getType() == sensor_t::PRESSURE){
            ui->pressure_label->setText(QString::number(s.getValue()));
        }
        else if(s.getType() == sensor_t::TEMPERATURE_INT){
            ui->temperature_label->setText(QString::number(s.getValue()));
        }
    }
}
