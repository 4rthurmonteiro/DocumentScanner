//FOUND AT http://stackoverflow.com/questions/22519545/automatic-perspective-correction-opencv

#include <stdio.h>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/xfeatures2d.hpp>
using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;

/* @function main */
int main( int argc, char** argv )
{
  Mat src=imread("qrcode_nota.JPG");
 Mat thr;
 cvtColor(src,thr,CV_BGR2GRAY);
 threshold( thr, thr, 70, 255,CV_THRESH_BINARY );

 vector< vector <Point> > contours; // Vector for storing contour
 vector< Vec4i > hierarchy;
 int largest_contour_index=0;
 int largest_area=0;

 Mat dst(src.rows,src.cols,CV_8UC1,Scalar::all(0)); //create destination image
 findContours( thr.clone(), contours, hierarchy,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE ); // Find the contours in the image
 for( int i = 0; i< contours.size(); i++ ){
    double a=contourArea( contours[i],false);  //  Find the area of contour
    if(a>largest_area){
        largest_area=a;
        largest_contour_index=i;                //Store the index of largest contour
    }
 }

 drawContours( dst,contours, largest_contour_index, Scalar(255,255,255),CV_FILLED, 8, hierarchy );
 vector<vector<Point> > contours_poly(1);
 approxPolyDP( Mat(contours[largest_contour_index]), contours_poly[0],5, true );
 Rect boundRect=boundingRect(contours[largest_contour_index]);
 if(contours_poly[0].size()==4){
    std::vector<Point2f> quad_pts;
    std::vector<Point2f> squre_pts;
    quad_pts.push_back(Point2f(contours_poly[0][0].x,contours_poly[0][0].y));
    quad_pts.push_back(Point2f(contours_poly[0][1].x,contours_poly[0][1].y));
    quad_pts.push_back(Point2f(contours_poly[0][3].x,contours_poly[0][3].y));
    quad_pts.push_back(Point2f(contours_poly[0][2].x,contours_poly[0][2].y));
    squre_pts.push_back(Point2f(boundRect.x,boundRect.y));
    squre_pts.push_back(Point2f(boundRect.x,boundRect.y+boundRect.height));
    squre_pts.push_back(Point2f(boundRect.x+boundRect.width,boundRect.y));
    squre_pts.push_back(Point2f(boundRect.x+boundRect.width,boundRect.y+boundRect.height));

    Mat transmtx = getPerspectiveTransform(quad_pts,squre_pts);
    Mat transformed = Mat::zeros(src.rows, src.cols, CV_8UC3);
    warpPerspective(src, transformed, transmtx, src.size());
    Point P1=contours_poly[0][0];
    Point P2=contours_poly[0][1];
    Point P3=contours_poly[0][2];
    Point P4=contours_poly[0][3];


    line(src,P1,P2, Scalar(0,0,255),1,CV_AA,0);
    line(src,P2,P3, Scalar(0,0,255),1,CV_AA,0);
    line(src,P3,P4, Scalar(0,0,255),1,CV_AA,0);
    line(src,P4,P1, Scalar(0,0,255),1,CV_AA,0);
    rectangle(src,boundRect,Scalar(0,255,0),1,8,0);
    rectangle(transformed,boundRect,Scalar(0,255,0),1,8,0);

    imshow("quadrilateral", transformed);
    imshow("thr",thr);
    imshow("dst",dst);
    imshow("src",src);
    imwrite("result1.jpg",dst);
    imwrite("result2.jpg",src);
    imwrite("result3.jpg",transformed);
    waitKey();
   }
   else
    cout<<"Make sure that your are getting 4 corner using approxPolyDP..."<<endl;
  return 0;
  }
