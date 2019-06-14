#include "vision.h"
#include <iostream>
#include <vector>

namespace Politocean {



#define TOLLERANCE_BLUE 100
#define MINIMUM_BLUE 500
using namespace std;
Vision::Vision()
{


}

bool Vision::checkTop(Mat src)
{
    //DEFINE ROI (Region Of Interest)
    Rect roi;

    roi.x = src.size().width/2 - OFFSET;
    roi.y = 0;
    roi.width = OFFSET *2;
    roi.height = src.size().height/4;

    //COUNT NUMBER OF PIXEL != BLACK
    int n_pixels = countNonZero(src(roi));
    addROI(src,roi);
    if(n_pixels > TOLERANCE)
        return true;

    return false;
}

bool Vision::checkLeft(Mat src)
{
    //DEFINE ROI (Region Of Interest)
    Rect roi;

    roi.x = 0;
    roi.y = src.size().height/2 - OFFSET;
    roi.width = src.size().width/4;
    roi.height = OFFSET * 2;

    //COUNT NUMBER OF PIXEL != BLACK
    int n_pixels = countNonZero(src(roi));
    addROI(src,roi);
    if(n_pixels > TOLERANCE)
        return true;

    return false;
}

 bool Vision::checkCenter(cv::Mat src)
 {
     cv::Mat filtered_blue = Vision::filterBlue(src);
     Rect roi_left,roi_right,roi_center;
     
     roi_left.width = src.size().width/4;
     roi_left.height = src.size().height;
     roi_left.x = 0;
     roi_left.y = 0;

     roi_right.width = src.size().width/4;
     roi_right.height = src.size().height;
     roi_right.x = src.size().width*3/4;;
     roi_right.y = 0;

     roi_center.width = src.size().width*2/4;
     roi_center.height = src.size().height;
     roi_center.x = src.size().width/4;
     roi_center.y = 0;

     if(countNonZero(filtered_blue(roi_left)) > TOLLERANCE_BLUE) return false;
     if(countNonZero(filtered_blue(roi_right)) > TOLLERANCE_BLUE) return false;
     if(countNonZero(filtered_blue(roi_center)) > MINIMUM_BLUE) return true;
 }

bool Vision::checkRight(Mat src)
{
    //DEFINE ROI (Region Of Interest)
    Rect roi;

    roi.width = src.size().width/4;
    roi.height = OFFSET * 2;
    roi.x = src.size().width - roi.width;
    roi.y = src.size().height/2 - OFFSET;

    //COUNT NUMBER OF PIXEL != BLACK
    int n_pixels = countNonZero(src(roi));
    addROI(src,roi);
    if(n_pixels > TOLERANCE)
        return true;

    return false;
}

bool Vision::checkBottom(Mat src)
{
    //DEFINE ROI (Region Of Interest)
    Rect roi;

    roi.width = OFFSET *2;
    roi.height = src.size().height/4;
    roi.x = src.size().width/2 - OFFSET;
    roi.y = src.size().height - roi.height;


    //COUNT NUMBER OF PIXEL != BLACK
    int n_pixels = countNonZero(src(roi));
    addROI(src,roi);
    if(n_pixels > TOLERANCE)
        return true;

    return false;
}

Mat Vision::filterRed(Mat src)
{
    Mat res,redOnly,redOnly2;

    //COLOR RED IN HSL (N.B in openCV is HLS
    /* RED HUE 340 -> 360 to 0 -> 10  H = H/2 */
    inRange(src, Scalar(170, 50, 50), Scalar(180,160, 255), redOnly);
    inRange(src, Scalar(0, 50, 50), Scalar(10, 160, 255), redOnly2);

    src.copyTo(res,redOnly2);
    src.copyTo(res,redOnly);

    cvtColor(res,res,CV_HLS2RGB);
    cvtColor(res,res,CV_RGB2GRAY);
    threshold(res,res,0,255,CV_THRESH_BINARY);

    return res;
}


Mat Vision::filterBlue(Mat src)
{
    Mat res,blueOnly,canny_output;

    //COLOR BLUE IN HSL (N.B in openCV is HLS
    /* Be careful to S value -> low saturation is not good */
    inRange(src, Scalar(100, 25, 90), Scalar(120,200, 255), blueOnly);
    src.copyTo(res,blueOnly);

    cvtColor(res,res,CV_HLS2RGB);
    cvtColor(res,res,CV_RGB2GRAY);
    threshold(res,res,0,255,CV_THRESH_BINARY);

    return res;
}

Mat Vision::filterBlack(Mat src)
{
    Mat res,blackOnly,gray;

    blur(src,src,Size(2,2));
    inRange(src, Scalar(0, 0, 0), Scalar(255,40, 255), blackOnly);
    src.copyTo(res,blackOnly);

    cvtColor(res,res,CV_HLS2RGB);
    cvtColor(res,res,CV_RGB2GRAY);

    threshold(res,res,0,255,CV_THRESH_BINARY);

    return res;
}


double Vision::getLenghtFromCenter(Mat src)
{
    int areaRed,areaBlue;
    int lato = 80;

    double lineTicknessPixel,lengthPixel,length = 0,cmPerPixel;
    double max = 0;

    Mat red,blue,canny_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    Rect roi;

    roi.x = src.size().width/2 - lato/2;
    roi.y = src.size().height/2 - lato/2;
    roi.width = lato;
    roi.height = lato;

    red = filterRed(src);
    blue = filterBlue(src);

    areaRed = countNonZero(red(roi));
    areaBlue = countNonZero(blue);

    lineTicknessPixel = (areaRed / (lato) );
    cmPerPixel = LINE_TICKNESS / lineTicknessPixel;

    /* FIRST ATTEMPT */
    //lengthPixel = areaBlue / lineTicknessPixel;
    //length = lengthPixel * cmPerPixel;

    /* NEW TRY */
    /// Detect edges using canny
    //Canny(blue, canny_output, 0, 255, 3 );
    /// Find contours
    findContours( blue, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

    int perimeter;
    for( unsigned int i = 0; i< contours.size(); i++ )
         {
            perimeter = arcLength(contours[i],true);
            if(perimeter > max){
                max  = perimeter;
                length = (perimeter - lineTicknessPixel*2) / 2 * cmPerPixel;
            }
         }


    return length;
}

Mat Vision::addROI(Mat src, Rect roi){

    int lato = 80;

    Point p1(roi.x , roi.y);
    Point p2(roi.x + roi.width, roi.y);
    Point p3(roi.x, roi.y + roi.height);
    Point p4(roi.x + roi.width , roi.y + roi.height);

//    Point p1(src.size().width/2 - lato/2,src.size().height/2 - lato/2);
//    Point p2(src.size().width/2 + lato/2,src.size().height/2 - lato/2);
//    Point p3(src.size().width/2 - lato/2,src.size().height/2 + lato/2);
//    Point p4(src.size().width/2 + lato/2,src.size().height/2 + lato/2);

    vector<Point> contour;
    contour.push_back(p4);
    contour.push_back(p2);
    contour.push_back(p1);
    contour.push_back(p3);
    vector<vector<Point> > con = vector<vector<Point> >(1, contour);

    Scalar color = Scalar(0,255,0);
    drawContours(src, con, -1, color, 1, 1);

    return src;
}

bool Vision::isCentered(Mat srcBlackFiltered)
{

    Mat image;
    int cont = 0;

    image = filterBlack(srcBlackFiltered);

    for (int row = 50; row < 90; ++row)
    {
        const uchar *ptr = image.ptr(row);
        const uchar * uc_pixel = ptr;
        int a = uc_pixel[0];
        if(a){
            cont++;
            if(cont > 20) return true;
        }
    }

    return false;
}

int Vision::getLenghtFromBlack(Mat black,Mat blue)
{
    int cont = 0,dist = 0,areaBlue,lengthBluePixel;
    double pixelPerCm,length = 0;

    for(int col = black.size().width - 1; col > 0; col--){


        const uchar *ptr = black.ptr(50);
        const uchar * uc_pixel = ptr + col;

        if(uc_pixel[0]){
            cont = 0;
            for (int row = 50; row < 90; ++row)
            {
                const uchar *ptr = black.ptr(row) + col;
                const uchar * uc_pixel = ptr;

                if(uc_pixel[0]){
                    cont++;
                    if(cont > 20){
                        dist = col;
                        pixelPerCm = double(dist / SQUARE_LENGTH);
                        areaBlue = countNonZero(blue);
                        lengthBluePixel = (double) (areaBlue / (LINE_TICKNESS * pixelPerCm));
                        length = lengthBluePixel / pixelPerCm;

                         cout <<length<<endl;

                         return length;
                    }
                }

                //cout << cont << endl;
            }
        }
    }

    cout<< length <<endl;
    return 0;
}

void Vision::Circle( Mat img, int position[4] )
{
     stringstream circ;
     circ << position[0];
     string str = circ.str();

    //se c'è il cerchio allora mostralo
    if (position[0] >= 1)
    {
        circle( img,Point( 200, 75) ,40,Scalar( 255, 0, 0 ),FILLED,LINE_8 );
        putText( img, str, Point (30,100), FONT_HERSHEY_COMPLEX, 3,Scalar(255, 0, 0), 5, 6 );
    }
}

void Vision::Triangle( Mat image, int position[4])
{
    // Cast from integer to string for obtaining a feasible output for puttext
     stringstream tri;
     tri << position[1];
     string str = tri.str();

    Point number;
    //se c'è il triangolo
    if (position[1]>=1)
    {
        Point rook_points[1][3];
        //no cerchio, riga 1
        if (position[0]==0)
        {
              rook_points[0][0]  = Point( 150, 125);
              rook_points[0][1]  = Point( 200, 50);
              rook_points[0][2]  = Point(250, 125);
              number = Point (30,125);
        }
        //riga 2
        else
        {
              rook_points[0][0]  = Point( 150, 225);
              rook_points[0][1]  = Point( 200, 150);
              rook_points[0][2]  = Point(250, 225);
              number = Point (30,225);
        }

        int lineType = LINE_8;
        const Point* ppt[1] = { rook_points[0]};
        int npt[] = { 3 };

        fillPoly( image,ppt,npt,1,Scalar( 255, 0, 0 ),lineType );
        putText( image, str, number, FONT_HERSHEY_COMPLEX, 3, Scalar(255, 0, 0), 5, 6 );
    }
}

void Vision::Line( Mat image, int position[4])
{


     stringstream lin;
     lin << position[2];
     string str = lin.str();

    //se c'è la linea
    if (position[2] >= 1){
        Point start, end,number;
        // no cerchio, riga 2
        if (position[0] ==0)
        {
            start = Point( 150, 200 );
            end = Point( 250, 200);
            number = Point (30,225);
            //no cerchio, no triangolo, riga 1
            if (position[1] == 0)
            {
                start = Point( 150, 100 );
                end = Point( 250, 100);
                number = Point (30,125);
            }
        }
        if (position[1]==0)
        {
            start = Point( 150, 200 );
            end = Point( 250, 200);
            number = Point (30,225);
        }
        else{
            start = Point( 150, 300 );
            end = Point( 250, 300);
            number = Point (30,325);
        }
        int thickness = 6;
        int lineType = LINE_8;
        line( image,
            start,
            end,
            Scalar( 255, 0, 0 ),
            thickness,
            lineType );
        putText( image, str, number, FONT_HERSHEY_COMPLEX, 3, Scalar(255, 0, 0), 5, 6 );
    }
}

void Vision::Rectangle( Mat img, int position[4])
{
     stringstream rect;
     rect << position[3];
     string str = rect.str();

    if (position[3]>=1)
    {
        Point left, right,number;

        if (position[0] ==0)
        {
            //no cercio, riga 3
            left = Point( 162, 250 );
            right = Point( 237, 325);
            number = Point (30,325);

            //no cerchio e triangolo, riga 2
            if (position[1] == 0)
            {
                left = Point( 162, 150 );
                right = Point( 237, 225);
                number = Point (30,225);

                //no cerchio, triangolo e linea, riga 1
                if (position[2] == 0)
                {
                    left = Point( 162, 50 );
                    right = Point( 237, 125);
                    number = Point (30,125);
                }
            }
        }
        //no triangolo, riga 3
        else if (position[1]==0)
        {
            left = Point( 162, 250 );
            right = Point( 237, 325);
            number = Point (30,325);
            // no linea e triangolo, riga 2
            if (position[2] == 0)
            {
                left = Point( 162, 150 );
                right = Point( 237, 225);
                number = Point (30,225);
            }
        }
        //no linea, riga 3
        else if (position[2]==0)
        {
            left = Point( 162, 250 );
            right = Point( 237, 325);
            number = Point (30,325);
        }
        else{
            //riga 4
            left = Point( 162, 350);
            right = Point( 237,425);
            number = Point (30,425);
        }

        rectangle( img,left,right,Scalar( 255, 0, 0 ),FILLED,LINE_8 );
        putText( img, str, number, FONT_HERSHEY_COMPLEX, 3,
                Scalar(255, 0, 0), 5, 6 );
    }
}

Mat Vision::getImageBlackShape(Mat src,int thresh){
    IplImage copy = src;
    IplImage* img = &copy;

    cvNot(img,img);
    //converting the original image into grayscale
    IplImage* imgGrayScale = cvCreateImage(cvGetSize(img), 8, 1);
    cvCvtColor(img,imgGrayScale,CV_BGR2GRAY);

    //thresholding the grayscale image to get better results
    cvThreshold(imgGrayScale,imgGrayScale,thresh,255,CV_THRESH_BINARY);


    Mat ret = cvarrToMat(imgGrayScale);
    return ret;

}

Mat Vision::getshape(Mat image1,int thresh)
{
     //counter for each figure
     int cnt_tri = 0;
     int cnt_circ = 0;
     int cnt_rect = 0;
     int cnt_line = 0;

     Mat blank_img(500,400, CV_8UC3, Scalar(250, 250, 250));
     IplImage copy = image1;
     IplImage* img = &copy;

     IplImage copy2 = image1;
     IplImage* imgGrayScale = &copy2;


//     cvNot(img,img);
//     //converting the original image into grayscale
//     IplImage* imgGrayScale = cvCreateImage(cvGetSize(img), 8, 1);
//     cvCvtColor(img,imgGrayScale,CV_BGR2GRAY);

//     //thresholding the grayscale image to get better results
//     cvThreshold(imgGrayScale,imgGrayScale,thresh,255,CV_THRESH_BINARY);

     CvSeq* contours;  //hold the pointer to a contour in the memory block
     CvSeq* result;   //hold sequence of points of a contour
     CvMemStorage *storage = cvCreateMemStorage(0); //storage area for all contours

     //finding all contours in the image
     cvFindContours(imgGrayScale, storage, &contours, sizeof(CvContour), CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, cvPoint(0,0));


     //iterating through each contour
     while(contours)
     {
         //obtain a sequence of points of contour, pointed by the variable 'contour'
         result = cvApproxPoly(contours, sizeof(CvContour), storage, CV_POLY_APPROX_DP, cvContourPerimeter(contours)*0.02, 0);

         if(result->total==2 ){
                      CvPoint *pt[2];
                      for (int i=0;i<2;i++){
                          pt[i]= (CvPoint*)cvGetSeqElem(result,i);
                      }
                     cvLine(img,*pt[0],*pt[1],cvScalar(255,255,255),4);
                     cvLine(img,*pt[1],*pt[0],cvScalar(255,255,255),4);
                     if (cnt_line<=6){
                      cnt_line++;}
                  }

         //if there are 3 vertices in the contour(It should be a triangle)
         else if(result->total==3 ){
             CvPoint *pt[3];
             for (int i=0;i<3;i++){
                 pt[i]= (CvPoint*)cvGetSeqElem(result,i);
             }
            cvLine(img,*pt[0],*pt[1],cvScalar(255,0,0),4);
            cvLine(img,*pt[1],*pt[2],cvScalar(255,0,0),4);
            cvLine(img,*pt[2],*pt[0],cvScalar(255,0,0),4);
            if (cnt_tri<=6){
             cnt_tri++;}
         }

         //if there are 4 vertices in the contour(It should be a quadrilateral)
         else if(result->total==4 )
         {
             //iterating through each point
             CvPoint *pt[4];
             for(int i=0;i<4;i++){
                 pt[i] = (CvPoint*)cvGetSeqElem(result, i);
             }

              int * a = &pt[0]->y;
              int * b = &pt[1]->y;
              int * c = &pt[1]->x;
              int * d = &pt[2]->x;
              int base = *d-*c;
              int altezza = *b-*a;
              //se è una linea la base o l'altezza sono molto differenti
             if (base<altezza/6){
                 cvLine(img,*pt[0],*pt[1],cvScalar(255,255,0),4);
                 cvLine(img,*pt[1],*pt[2],cvScalar(255,255,0),4);
                 cvLine(img,*pt[2],*pt[3],cvScalar(255,255,0),4);
                 cvLine(img,*pt[3],*pt[0],cvScalar(255,255,0),4);

                 cnt_line++;
             }
             else if( altezza<base/6){
                 cvLine(img,*pt[0],*pt[1],cvScalar(255,255,0),4);
                 cvLine(img,*pt[1],*pt[2],cvScalar(255,255,0),4);
                 cvLine(img,*pt[2],*pt[3],cvScalar(255,255,0),4);
                 cvLine(img,*pt[3],*pt[0],cvScalar(255,255,0),4);
                 cnt_line++;
             }
             // altrimenti è un quadrato
             else{
                 cvLine(img,*pt[0],*pt[1],cvScalar(255,255,0),4);
                 cvLine(img,*pt[1],*pt[2],cvScalar(255,255,0),4);
                 cvLine(img,*pt[2],*pt[3],cvScalar(255,255,0),4);
                 cvLine(img,*pt[3],*pt[0],cvScalar(255,255,0),4);
                 if (cnt_rect<=6){
                  cnt_rect++;}
             }
         }
        //if there are 8 vertices in the contour(It should be a circle)
        else if(result->total==8 ){
             if (cnt_circ<=6){
              cnt_circ++;}
        }

         //obtain the next contour
         contours = contours->h_next;
     }

     int pos[4] = {cnt_circ,cnt_tri,cnt_line,cnt_rect};

     //drawing results
     Vision::Circle( blank_img, pos );
     Vision::Triangle(blank_img,pos);
     Vision::Line(blank_img, pos);
     Vision::Rectangle(blank_img, pos);

     /* DEBUG */

//     cvDestroyAllWindows();
//     cvNamedWindow("original");
//     cvShowImage("original",img);

     cvReleaseMemStorage(&storage);

     return blank_img;

}

Mat Vision::addCircle(Mat src, int value)
{
    Point center(src.size().width/2, src.size().height/2);
    circle(src,center,value,Scalar(255,0,0),2,LINE_8);

    return src;
}
}
