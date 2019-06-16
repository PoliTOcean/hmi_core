#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <iostream>
#include <QLabel>
#include <opencv2/opencv.hpp>
#include "ipcamera.h"
#include "PolitoceanConstants.h"
//#include "autodrive.h"
#include "mqttLogger.h"
#include "MqttClient.h"
#include "ipcamera.h"
#include "Sensor.h"
#include "Reflectables/Vector.hpp"


using namespace Politocean;
using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{

    enum class MODE{
        MODE_HOME,
        MODE_AUTO,
        MODE_SHAPES,
        MODE_CANNON
    };
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    void setJoystick(bool connected);
    void setAtMega(bool connected);
    void messageArrived(const std::string& payload, const std::string& topic);
    void sensorArrived(Types::Vector<Sensor<float>> payload);
    void setFrame(const cv::Mat frame);
    
    QImage imdisplay;  //This will create QImage which is shown in Qt label
    QTimer* Timer;   // A timer is needed in GUI application
    IpCamera camera;
    Types::Vector<Sensor<float>> sensors_; // vector of sensors
    ~MainWindow();

public
slots:    // A slot or function is defined which will be intiated by timer
    void DisplayImage();
    void setVideoStart();
    void modeAuto();
    void modeCannon();
    void modeShapes();
    void modeHome();
    void startMeasure();
    void valueTrackbar(int value);
    void setMessageConsole(QString message, int type);
    void calculate();
    void change_unit();
    void zoom();
    void scroll_left();
    void scroll_right();

private slots:
    void Mouse_current_pos();
    void Mouse_Pressed();
    void Mouse_left();

signals:
    void componentChanged();
    void messageArrived(QString msg,int type);
    void frameArrived();

private:
    bool video,snap_b,debug,snap_a,snap,previous,next;
    Ui::MainWindow *ui;
    Mat img,shape;
    QIcon icon,icon2,video_icon,auto_icon,shapes_icon,home_icon,cannon_icon;
    QIcon auto_icon_w,shapes_icon_w,home_icon_w,term_icon,cannon_icon_w;
    MODE mode = MODE::MODE_HOME;
    //AutoDrive autodrive;
    int value_track,cnt,i=0,mean=0,change=1,base,turn,moda=0,max;
    double R1,R2,L,a,b,l,unit;
    std::string str;
    Point left,right;
};

#endif // MAINWINDOW_H
