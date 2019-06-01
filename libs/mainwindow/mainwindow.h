#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "ipcamera.h"
#include "PolitoceanConstants.h"
//#include "autodrive.h"
#include "mqttLogger.h"
#include "MqttClient.h"
#include "ipcamera.h"


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

    
    QImage imdisplay;  //This will create QImage which is shown in Qt label
    QTimer* Timer;   // A timer is needed in GUI application
    void setFrame(cv::Mat frame);
    //IpCamera cam;
    VideoCapture cap;
    IpCamera camera;

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

signals:
    void componentChanged();
    void messageArrived(QString msg,int type);
    void frameArrived();

private:
    bool video,snap_b,debug,snap_a;
    Ui::MainWindow *ui;
    Mat img,shape;
    QIcon icon,icon2,video_icon,auto_icon,shapes_icon,home_icon,cannon_icon;
    QIcon auto_icon_w,shapes_icon_w,home_icon_w,term_icon,cannon_icon_w;
    MODE mode = MODE::MODE_HOME;
    //AutoDrive autodrive;
    int value_track,cnt;
    Point left,right;
    Mat img_hls,res,frame,frame_rsz;

};

#endif // MAINWINDOW_H
