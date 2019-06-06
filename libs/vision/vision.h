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

    static bool checkTop(cv::Mat src);
    static bool checkLeft(cv::Mat src);
    static bool checkRight(cv::Mat src);
    static bool checkBottom(cv::Mat src);

    /* FILTERS */
    static Mat filterRed(cv::Mat src);
    static Mat filterBlue(cv::Mat src);
    static Mat filterBlack(cv::Mat src);
    static Mat addROI(cv::Mat src,Rect roi);
    static bool isCentered(cv::Mat srcBlackFiltered);
    static int getLenghtFromBlack(cv::Mat black,cv::Mat blue);
    static void getLenghtFromCenter(cv::Mat src);

    /* SHAPE RECOGNITION */
    static void Circle( cv::Mat img, int position[4]);
    static void Triangle( cv::Mat img ,int position[4]);
    static void Line( cv::Mat img,int position[4]);
    static void Rectangle( cv::Mat img, int position[4] );
    static cv::Mat getshape(cv::Mat image1,bool debug,int mean);
    static cv::Mat getImageBlackShape(cv::Mat src,int thresh);
    static int mean_mode( int numeri[100]);

    static cv::Mat addCircle(cv::Mat src, int value);

private:
    int height,width;
};

}

#endif // VISION_H
