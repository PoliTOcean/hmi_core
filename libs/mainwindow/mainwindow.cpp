#include "mainwindow.h"
#include "ui_design.h"
#include <QTimer>
#include "my_qlabel.h"
#include "vision.h"
#include "ipcamera.h"
#include <QMouseEvent>
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
    video = true;

    /*TIMER DISPLAY CAMERAS*/
    connect(this, SIGNAL(frameArrived()), this, SLOT(DisplayImage()));
    connect(this, SIGNAL(sensorsUpdating()),this, SLOT(setSensorsLabel()));
    connect(this, SIGNAL(phArrived()),this, SLOT(setPhValue()));

    /*CONNECTION BUTTONS*/
    connect(ui->startVideo,SIGNAL(clicked()),SLOT(setVideoStart()));
    connect(ui->auto_drive,SIGNAL(clicked()),SLOT(modeAuto()));
    connect(ui->home,SIGNAL(clicked()),SLOT(modeHome()));
    connect(ui->shapes_recognize,SIGNAL(clicked()),SLOT(modeShapes()));
    connect(ui->measure_button,SIGNAL(clicked()),SLOT(startMeasure()));
    connect(ui->depth_button,SIGNAL(clicked()),SLOT(setDepthOffset()));

    connect(ui->cannon_measure,SIGNAL(clicked()),SLOT(modeCannon()));
    connect(ui->home_4,SIGNAL(clicked()),SLOT(calculate()));
    connect(ui->home_5,SIGNAL(clicked()),SLOT(change_unit()));
    connect(ui->home_6,SIGNAL(clicked()),SLOT(zoom()));
    connect(ui->home_7,SIGNAL(clicked()),SLOT(scroll_left()));
    connect(ui->home_8,SIGNAL(clicked()),SLOT(scroll_right()));
    connect(ui->grid_button,SIGNAL(clicked()),SLOT(getGrid()));


    //Mouse Callback for cannon mode
    connect(ui->display_image_2,SIGNAL(Mouse_Pos()),this,SLOT(Mouse_current_pos()));
    connect(ui->display_image_2,SIGNAL(Mouse_Pressed()),this,SLOT(Mouse_Pressed()));
    connect(ui->display_image_2,SIGNAL(Mouse_Left()),this,SLOT(Mouse_left()));
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
    cannon_icon_w.addFile(QString::fromUtf8("images/cannon_w.png"), QSize(), QIcon::Normal, QIcon::Off);
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

    value_track = 150;
    snap_b = false;
    cnt = 0;
    left = Point(0,0);
    right = Point(0,0);
    a = 0;
    b = 0;
    l = 0;
    i = 0;
    unit = 0;
    turn = 0;
    snap = false;
    snap_a = false;

    previous = false;
    next = false;
    lenght_blue = 0;
    depth_off = 0;
    num_average_lenght = 0;
    ph_read = false;
    checkBlue = false;

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
    Serial serial("/dev/ttyACM0");
    serial.open();
    //gui->setMessageConsole(QString::fromUtf8("Ph Thread ON"),0);
    while(gui->ph_read){
        std::this_thread::sleep_for(std::chrono::seconds(1));
        try{
            serial.readLine(ph);

            std::string delimiter = ";";
            int pos = ph.find(delimiter);
            ph.erase(0,pos+1);

            pos = ph.find(delimiter);
            gui->ph_string = ph.substr(0,pos);
            ph.erase(0,pos+1);

            pos = ph.find(delimiter);
            gui->temp_string = ph.substr(0,pos);
            ph.erase(0,pos+1);

            gui->phArrived();
            //gui->ui->ph_label->setText("PH: "+QString::fromStdString(ph_string));
            //gui->ui->temp_label->setText("Temp "+QString::fromStdString(temp_string));
         }
        catch(SerialException e){
            std::cerr << e.what()<< std::endl;
        }
        catch(...){

        }
    }
    try{
        //serial.close();
    }
    catch(...){
        //std::cerr << e.what() <<std::endl;
    }
}

void MainWindow::setPhValue(){
    ui->ph_label->setText("PH: "+QString::fromStdString(ph_string));
    ui->temp_label->setText("Temp "+QString::fromStdString(temp_string));

}

void MainWindow::phRead()
{
    //setMessageConsole(QString::fromUtf8("Ph Thread ON"),0);
    if(!ph_read){
        //setMessageConsole(QString::fromUtf8("Ph Thread ON"),0);
        ph_read = true;
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

void MainWindow::setDepthOffset()
{
    depth_off = pressure;
}

void MainWindow::DisplayImage(){
    Mat img_hls;
    if(!video || img.empty())
        return;

    Mat frame, frame_rsz, res;

    std::lock_guard<std::mutex> lock(mtx);
    cvtColor(img, frame_rsz, CV_BGR2RGB);

    mtx.unlock();

    cv::resize(frame_rsz, frame, cv::Size(910,720));
    cvtColor(frame,img_hls,CV_RGB2HLS);

    if(mode == MODE::MODE_AUTO){
        autodrive.updateDirection(Vision::filterRed(img_hls));
        //img = Vision::addCircle(frame,value_track);
        if(ui->debugCheck->isChecked()){
            cv::Mat filtered = Vision::filterBlue(img_hls);
            QImage cam1((uchar*)filtered.data, filtered.cols, filtered.rows, filtered.step, QImage::Format_Grayscale8);
            ui->display_image->setPixmap(QPixmap::fromImage(cam1));
        }
        else{
            std::lock_guard<std::mutex> lock(mtx);
            QImage cam1((uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
            mtx.unlock();

            ui->display_image->setPixmap(QPixmap::fromImage(cam1));

            if(snap_b){
                lenght_blue = Vision::getLenghtFromCenter(img_hls);
                autodrive.setBluePosition();
                std::ostringstream strs;
                strs << lenght_blue;
                std::string text  = "CRACK LENGTH: " + strs.str() + " cm";

                setMessageConsole(QString::fromStdString(text),0);

                checkBlue = false;
                num_average_lenght = 0;
                snap_b = false;
            }

        }
    }

    else if(mode  == MODE::MODE_HOME){
        //VISION TEST:
        if(ui->debugCheck->isChecked()){
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

                shape = Vision::getImageBlackShape(frame,value_track);
                ui->display_image_2->setVisible(true);
                //DRAW THE REGION OF INTEREST
                if(previous){
                    previous=false;
                }
                rectangle( shape,Point(200,300),Point(600,600),Scalar( 255, 255, 255 ),1,LINE_4 );

                //SELECT ONLY THIS REGION OF THE IMAGE
                Rect roi;
                roi.x = 200;
                roi.y = 300;
                roi.width = (600-200);
                roi.height=(600-300);
               // shape = shape(roi);

                if(!ui->debugCheck->isChecked()){
                     QImage cam1((uchar*)shape.data, shape.cols, shape.rows, shape.step, QImage::Format_Grayscale8);
                     ui->display_image->setPixmap(QPixmap::fromImage(cam1));
                 }
                 else if(ui->debugCheck->isChecked()){

                     debug = true;
                     res = Vision::getshape(shape,debug,mean,moda);
                     QImage cam1((uchar*)res.data, res.cols, res.rows, res.step,QImage::Format_RGB888);
                     ui->display_image->setPixmap(QPixmap::fromImage(cam1));
                 }

               if(snap_b){
                   shape = shape(roi);
                   debug = false;

                   if(moda!=20){
                        mean++;
                        res = Vision::getshape(shape,debug,mean,moda);
                    }

                   if(mean == 20 ){
                       mean=0;
                       moda++;
                       QImage cam2((uchar*)res.data, res.cols, res.rows, res.step, QImage::Format_RGB888);
                       ui->display_image_2->setPixmap(QPixmap::fromImage(cam2));
                       //snap_b = false;
                   }
                   if(moda==10){
                       res = Vision::getshape(shape,debug,mean,moda);
                       QImage cam2((uchar*)res.data, res.cols, res.rows, res.step, QImage::Format_RGB888);
                       ui->display_image_2->setPixmap(QPixmap::fromImage(cam2));
                       moda = 0;
                       snap_b = false;
                   }



               }


            }

    else if(mode == MODE::MODE_CANNON){

        QImage cam1((uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
        ui->display_image->setPixmap(QPixmap::fromImage(cam1));

        if(snap_b){

            str =  "images/cannon_mode"+std::to_string(i)+".png";
            i++;
            imwrite(str,frame);
            max = i;
            cnt = 1;
            snap_b = false;

        }
        if(previous){
            if(i!=0){
                i--;
                str =  "images/cannon_mode"+std::to_string(i)+".png";
                cnt =1;
            }

            previous = false;
        }
        if(next){
            if (i!=max){
                i++;
                str =  "images/cannon_mode"+std::to_string(i)+".png";
                cnt =1;
            }

            next = false;
        }

        if(cnt == 1){
            src = imread(str);
            //cv::resize(src,src,cv::Size(900,720));
            cnt=2;
        }


        if(cnt == 2){
            copy_src = src.clone();
            rectangle( copy_src,left,right,Scalar( 255, 255, 0 ),1,LINE_4 );
            //ZOOM condition
            if(snap_a){

                Rect roi;
                roi.x = left.x;
                roi.y = left.y;
                roi.width = (right.x - left.x);
                roi.height= (right.x - left.x)*(830/720);

                Mat zoom = src(roi);
                cv::resize(zoom,zoom,cv::Size(830,720));
                str =  "images/cannon_mode"+std::to_string(i)+".png";

                imwrite( str, zoom );
                cnt = 3;
                snap_a = false;

            }
            else{
                QImage cam2((uchar*)copy_src.data, copy_src.cols, copy_src.rows, copy_src.step, QImage::Format_RGB888);
                ui->display_image_2->setPixmap(QPixmap::fromImage(cam2));

            }
        }
        if(cnt == 3){
            src = imread(str);
            cnt=4;
        }
        if(cnt == 4){
            copy_src = src.clone();

            line(copy_src,left,right,Scalar(255,180,180),3,LINE_4);

            QImage cam2((uchar*)copy_src.data, copy_src.cols, copy_src.rows, copy_src.step, QImage::Format_RGB888);
            ui->display_image_2->setPixmap(QPixmap::fromImage(cam2));

        }
    }

    mtx.unlock();
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
        ui->consoleSerialException e->append(color_yellow);
    }

    ui->console->scrollBarWidgets(Qt::AlignBottom);
}


void MainWindow::modeCannon()
{
    mode = MODE::MODE_CANNON;

    /* Draw white icon */
    ui->cannon_measure->setIcon(cannon_icon_w);
    ui->cannon_measure->setIconSize(QSize(sizeIconMenu,sizeIconMenu));
    ui->auto_drive->setIcon(auto_icon);
    ui->auto_drive->setIconSize(QSize(sizeIconMenu,sizeIconMenu));
    ui->home->setIcon(home_icon);
    ui->home->setIconSize(QSize(sizeIconMenu,sizeIconMenu));
    ui->shapes_recognize->setIcon(shapes_icon);
    ui->shapes_recognize->setIconSize(QSize(sizeIconMenu,sizeIconMenu));
}

void MainWindow::getGrid()
{
    Mat res = autodrive.getGrid();
    QImage cam1((uchar*)res.data, res.cols, res.rows, res.step,QImage::Format_RGB888);
    ui->display_image_2->setPixmap(QPixmap::fromImage(cam1));
}

void MainWindow::modeAuto()
{
    //autodrive.reset();
    mode = MODE::MODE_AUTO;
    autodrive.init(img);
    checkBlue = true;
    ui->auto_drive->setIcon(auto_icon_w);
    ui->auto_drive->setIconSize(QSize(sizeIconMenu,sizeIconMenu));

    ui->home->setIcon(home_icon);
    ui->home->setIconSize(QSize(sizeIconMenu,sizeIconMenu));
    ui->shapes_recognize->setIcon(shapes_icon);
    ui->shapes_recognize->setIconSize(QSize(sizeIconMenu,sizeIconMenu));

    ui->gridLabel->setVisible(true);
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
    ui->cannon_measure->setIcon(cannon_icon);
    ui->cannon_measure->setIconSize(QSize(sizeIconMenu,sizeIconMenu));

    ui->gridLabel->setVisible(false);

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
    ui->cannon_measure->setIcon(cannon_icon);
    ui->cannon_measure->setIconSize(QSize(sizeIconMenu,sizeIconMenu));

    ui->gridLabel->setVisible(false);


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
    sensorsUpdating();
}


void MainWindow::Mouse_current_pos()
{
    if(snap){
       left = Point(ui->display_image_2->x,ui->display_image_2->y);
    }
}

void MainWindow::Mouse_Pressed()
{
        snap = true;
        right = Point(ui->display_image_2->x,ui->display_image_2->y);

}

void MainWindow::Mouse_left()
{
        snap = false;
}

void MainWindow::change_unit(){

    if(change == 1){
        base = 19;
        change++;

    }
    else if(change==2){
        base = 9;
        change++;
    }
    else if(change==3){
        base = 5.5;
        change=1;
    }

    ui->unit_conv->setText(QString("Size = %1 cm").arg(base));
}


void MainWindow::zoom(){
      snap_a = !snap_a;
}

void MainWindow::scroll_left()
{
    previous = true;

}

void MainWindow::scroll_right()
{
    next = true;

}

void MainWindow::calculate()
{

    a = abs(left.x-right.x);
    b = abs(left.y-right.y);
    l = sqrt(pow(a,2) + pow(b,2));
    if(turn == 0){

        ui->lblMouse_unit->setText(QString("1cm = 0"));
        ui->lblMouse_R1->setText(QString("D1 = 0"));
        ui->lblMouse_R2->setText(QString("D2 = 0"));
        ui->lblMouse_L->setText(QString("L = 0"));
        turn++;
    }
    else if(turn == 1){
        //convesion pixel in cm
        unit = base/l;

        ui->lblMouse_unit->setText(QString("1cm = %1 px").arg(unit));
        turn++;
    }
    else if(turn==2){
        L = l*unit;
        ui->lblMouse_L->setText(QString("L = %1 cm").arg(L));
        turn++;
    }
    else if(turn==3){
        R1 = l*unit;
        ui->lblMouse_R1->setText(QString("D1 = %1 cm").arg(R1));
        turn++;

    }
    else{
        R2 = l*unit;
        ui->lblMouse_R2->setText(QString("D2 = %1 cm").arg(R2));
        turn = 0;

    }
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
            pressure = s.getValue();
            ui->pressure_label->setText(QString::number((pressure - depth_off)/100) + QString::fromUtf8("m"));
        }
        else if(s.getType() == sensor_t::TEMPERATURE_INT){
            ui->temperature_label->setText(QString::number(s.getValue()) + QString::fromUtf8("°"));
        }
        else if(s.getType() == sensor_t::TEMPERATURE_PWR){
            ui->temperature_label_2->setText(QString::number(s.getValue()) + QString::fromUtf8("°"));
        }
    }
}
