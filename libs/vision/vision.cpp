#include "vision.h"
#include <iostream>
#include <vector>

namespace Politocean {

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

    blur(src,src,Size(9,9));
    inRange(src, Scalar(0, 0, 0), Scalar(255,40, 255), blackOnly);
    src.copyTo(res,blackOnly);

    cvtColor(res,res,CV_HLS2RGB);
    cvtColor(res,res,CV_RGB2GRAY);
    threshold(res,res,0,255,CV_THRESH_BINARY);
    return res;
}

void Vision::getLenghtFromCenter(Mat src)
{
    int areaRed, areaBlue;
    int lato = 80;

    double lineTicknessPixel,length = 0,cmPerPixel;
    double max = 0;

    Mat red,blue;
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


    cout<< "Lunghezza Linea Blue: "<< length << " cm" << endl;
}

Mat Vision::addROI(Mat src, Rect roi){

    //int lato = 80;

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

void Vision::Circle(Mat img, int position[4] )
{
     stringstream circ;
     circ << position[0];
     string str = circ.str();

     circle( img,Point( 200, 75) ,40,Scalar( 255, 0, 0 ),FILLED,LINE_8 );
     putText( img, str, Point (30,100), FONT_HERSHEY_COMPLEX, 3,Scalar(255, 0, 0), 5, 6 );
}
void Vision::Triangle( Mat image, int position[4])
{
    // Cast from integer to string for obtaining a feasible output for puttext
     stringstream tri;
     tri << position[1];
     string str = tri.str();

    Point number;

    Point rook_points[1][3];
    rook_points[0][0]  = Point( 150, 225);
    rook_points[0][1]  = Point( 200, 150);
    rook_points[0][2]  = Point(250, 225);
    number = Point (30,225);
    int lineType = LINE_8;
    const Point* ppt[1] = { rook_points[0]};
    int npt[] = { 3 };

    fillPoly( image,ppt,npt,1,Scalar( 255, 0, 0 ),lineType );
    putText( image, str, number, FONT_HERSHEY_COMPLEX, 3, Scalar(255, 0, 0), 5, 6 );
}

void Vision::Line(Mat image, int position[4])
{


     stringstream lin;
     lin << position[2];
     string str = lin.str();
     Point start, end,number;
     start = Point( 150, 300 );
     end = Point( 250, 300);
     number = Point (30,325);

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

void Vision::Rectangle(Mat img, int position[4])
{
     stringstream rect;
     rect << position[3];
     string str = rect.str();
     Point left, right,number;
     left = Point( 162, 350);
     right = Point( 237,425);
     number = Point (30,425);
     rectangle( img,left,right,Scalar( 255, 0, 0 ),FILLED,LINE_8 );
     putText( img, str, number, FONT_HERSHEY_COMPLEX, 3,
             Scalar(255, 0, 0), 5, 6 );

}

Mat Vision::getImageBlackShape(Mat src,int thresh){

    Mat src_gray;

    /// Convert image to gray and blur it
    cvtColor( src, src_gray, CV_BGR2GRAY );
    blur( src_gray, src_gray, Size(1,1) );
    Mat canny_output;

    /// Detect edges using canny
    Canny( src_gray, canny_output, thresh, thresh*2, 3 );
    return canny_output;

}



int Vision::mean_mode( int numeri[100],int length){


   //Qui comincia l'algoritmo di ricerca della moda

      int cont=0, max=0, pos=0,j=0,final;

      for (int i=0; i<length; i++){
          cont=0;
          for (j=i+1; j<length; j++){
              //confronta il primo numero del vettore con numeri[i]+1;
              if (numeri[i]==numeri[j]){
                  cont++;
                  pos=i; //attuale candidato ad essere moda indicato non con il numero preciso ma con la sua posizione
              }
              if (cont>max){
                  max=cont;
                  final=pos;
              }
          }
      }


   // cout<< "La moda è: "<<numeri[final]<< " e compare "<<max<<" volte";
    //cout<<posizione;

    return numeri[final];
}

Mat Vision::getshape(Mat src,bool debug,int j,int final){

    int cnt_tri = 0;
    int cnt_circ = 0;
    int cnt_rect = 0;
    int cnt_line = 0;

    static int tri[100];
    static int circ[100];
    static int rect[100];
    static int line[100];

    static int circle_moda[100];
    static int triangle_moda[100] ;
    static int line_moda[100];
    static int square_moda[100];





     Mat blank_img(720,830, CV_8UC3, Scalar(250, 250, 250));

     // Mat canny_output;
      vector<vector<Point> > contours;
      vector<Vec4i> hierarchy;

      // Find contours
      findContours( src, contours, hierarchy,CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

      /// Draw contours
      Mat drawing = Mat::zeros( src.size(), CV_8UC3 );
      vector<Point> approx;;
      /// Approximate contours to polygons + get bounding rects and circles
      vector<Rect> boundRect( contours.size() );

      for( size_t i = 0; i< contours.size(); i++ )
         {
          //find poligons
          approxPolyDP(contours[i], approx, arcLength(contours[i], true)*0.04, true);
          boundRect[i] = boundingRect(approx );

          //number of points
          int vtc = approx.size();

          double area = contourArea(contours[i], true);

          //triangle
          if(vtc==3 && fabs(contourArea(approx)) > 20 &&
                  isContourConvex(approx) ){
              if(area>0 && cnt_tri<6){
                  Scalar color = Scalar( 255, 0,0 );
                  drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
                  cnt_tri++;

                  }

              }
          //square or line
          else if(vtc==4 && fabs(contourArea(approx)) > 20 &&
                  isContourConvex(approx) ){

              if(area>0 && area<10000){
                  //compute the bounding box of the contour and use the
                  //bounding box to compute the aspect ratio

                  float ar = float(boundRect[i].width)/float(boundRect[i].height);

                  //a square will have an aspect ratio that is approximately
                  //equal to one, otherwise, the shape is a rectangle
                    ///square
                  if (ar >= 0.5 && ar <= 2){
                          if (cnt_rect<6)
                          {
                              Scalar color = Scalar( 255, 255,0 );
                              drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
                              cnt_rect++;

                          }
                      }
                    // otherwise it is line
                    else if(ar < 0.5 || ar > 2){
                        if (cnt_line<6)
                        {
                            Scalar color = Scalar( 0, 255,0 );
                            drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
                            cnt_line++;

                        }
                    }
                 }
              }

          //circle
          else if(vtc>=6 && fabs(contourArea(approx)) > 20 &&
                  isContourConvex(approx) ){
              if(area>0 && cnt_circ<6){
                  Scalar color = Scalar( 0, 0,255 );
                  drawContours( drawing, contours, i, color, 2, 8, hierarchy, 0, Point() );
                  cnt_circ++;

                }
              }
          }

        circ[j] = cnt_circ;
        tri[j] = cnt_tri;
        line[j] = cnt_line;
        rect[j] = cnt_rect;

        /* DEBUG */
        if(debug){
            return drawing;
            }
        //acquisisco 20 campioni e ne faccio la moda
      else if(j==20){

            circle_moda[final] = Vision::mean_mode(circ,j);
            triangle_moda[final] = Vision::mean_mode(tri,j);
            line_moda[final] = Vision::mean_mode(line,j);
            square_moda[final]= Vision::mean_mode(rect,j);


            int pos[4] = {circle_moda[final],triangle_moda[final],line_moda[final],square_moda[final]};


            Vision::Circle( blank_img, pos );
            Vision::Triangle(blank_img, pos);
            Vision::Line(blank_img, pos);
            Vision::Rectangle(blank_img, pos);

            return blank_img;
        }
        else if(final == 10){

            int circle_final = Vision::mean_mode(circle_moda,final);
            int triangle_final = Vision::mean_mode(triangle_moda,final);
            int line_final = Vision::mean_mode(line_moda,final);
            int square_final = Vision::mean_mode(square_moda,final);


            int pos[4] = {circle_final,triangle_final,line_final,square_final};


            Vision::Circle( blank_img, pos );
            Vision::Triangle(blank_img, pos);
            Vision::Line(blank_img, pos);
            Vision::Rectangle(blank_img, pos);

            return blank_img;
        }

    return blank_img;
}

Mat Vision::addCircle(Mat src, int value)
{
    Point center(src.size().width/2, src.size().height/2);
    circle(src,center,value,Scalar(255,0,0),2,LINE_8);

    return src;
}
}
