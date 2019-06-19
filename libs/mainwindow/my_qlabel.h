#ifndef MY_QLABEL_H
#define MY_QLABEL_H


#include <QMouseEvent>
#include <QLabel>
#include <QEvent>
#include <QDebug>

class my_qlabel : public QLabel
{
    Q_OBJECT
public:
    explicit my_qlabel(QWidget *parent = 0);

    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);

    int x,y;

signals:
    void Mouse_Pressed();
    void Mouse_Pos();
    void Mouse_Left();

public slots:

};

#endif // MY_QLABEL_H
