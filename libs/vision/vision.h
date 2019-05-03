#ifndef VISION_H
#define VISION_H

#include <opencv2/opencv.hpp>

#define LINE_TICKNESS 2.0
#define SQUARE_LENGTH 21.0
#define OFFSET 10
#define TOLERANCE 1000

namespace Politocean {

using namespace cv;

class Vision
{
public:
    Vision();

    static bool checkTop(Mat src);
    static bool checkLeft(Mat src);
    static bool checkRight(Mat src);
    static bool checkBottom(Mat src);

    /* FILTERS */
    static Mat filterRed(Mat src);
    static Mat filterBlue(Mat src);
    static Mat filterBlack(Mat src);
    static Mat addROI(Mat src,Rect roi);
    static bool isCentered(Mat srcBlackFiltered);
    static int getLenghtFromBlack(Mat black,Mat blue);
    static void getLenghtFromCenter(Mat src);

    /* SHAPE RECOGNITION */
    static void Circle( Mat img, int position[4]);
    static void Triangle( Mat img ,int position[4]);
    static void Line( Mat img,int position[4]);
    static void Rectangle( Mat img, int position[4] );
    static Mat getshape(Mat image1,int tresh);
    static Mat getImageBlackShape(Mat src,int thresh);

    static Mat addCircle(Mat src, int value);



private:
    int height,width;
};

}

#endif // VISION_H
