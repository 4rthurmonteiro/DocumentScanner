//FOUND AT http://stackoverflow.com/questions/22519545/automatic-perspective-correction-opencv
//see this too http://stackoverflow.com/questions/6555629/algorithm-to-detect-corners-of-paper-sheet-in-photo
//FOR THRESHOLD:
//http://docs.opencv.org/3.2.0/d7/d1b/group__imgproc__misc.html#ga72b913f352e4a1b1b397736707afcde3
//http://docs.opencv.org/3.2.0/db/d8e/tutorial_threshold.html
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

int edgeThresh = 1;
int lowThreshold = 50;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;
char* window_name = "Edge Map";

Mat src, src_gray;
Mat dst, detected_edges;

/**
 * @function CannyThreshold
 * @brief Trackbar callback - Canny thresholds input with a ratio 1:3
 */
void CannyThreshold(int, void*)
{
  /// Reduce noise with a kernel 3x3
  blur( src_gray, detected_edges, Size(3,3) );

  /// Canny detector
  Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );

  /// Using Canny's output as a mask, we display our result
  dst = Scalar::all(0);

  src.copyTo( dst, detected_edges);
 }


/* @function main */
int main( int argc, char** argv )
{

 src=imread(argv[1]);

 ///******* DETECÇÃO DE BORDA********///

 /// Create a matrix of the same type and size as src (for dst)
  dst.create( src.size(), src.type() );

  /// Convert the image to grayscale
  cvtColor( src, src_gray, CV_BGR2GRAY );

  /// Show the image
   CannyThreshold(0, 0);

   namedWindow("detected_edges", CV_WINDOW_KEEPRATIO);
   imshow("detected_edges",detected_edges);
   waitKey();
 ///********HOMOGRAFIA*********///

 vector< vector <Point> > contours; // Vector for storing contour
 vector< Vec4i > hierarchy;
 int largest_contour_index=0;
 int largest_area=0;

 Mat dst_h(src.rows,src.cols,CV_8UC1,Scalar::all(0)); //create destination image with homography
 cvtColor(dst,dst,CV_BGR2GRAY);

 findContours(dst.clone(), contours, hierarchy,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE ); // Find the contours in the image
 for( int i = 0; i< contours.size(); i++ ){
    double a=contourArea( contours[i],false);  //  Find the area of contour
    if(a>largest_area){
        largest_area=a;
        largest_contour_index=i;                //Store the index of largest contour
    }
 }


 drawContours( dst_h,contours, largest_contour_index, Scalar(255,255,255),CV_FILLED, 8, hierarchy );
 vector<vector<Point> > contours_poly(1);
 approxPolyDP( Mat(contours[largest_contour_index]), contours_poly[0],5, true );
 Rect boundRect=boundingRect(contours[largest_contour_index]);

 cout<<"the points of contours_poly[0] are \n"<<contours_poly[0]<<endl;

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


    ///**********LIMIARIZAÇÃO************///

    Mat transformed_thresholded;

    transformed = transformed(boundRect); //cut image for the interesting region

    cvtColor(transformed,transformed_thresholded,CV_BGR2GRAY);

    adaptiveThreshold(transformed_thresholded,transformed_thresholded,255,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY,11,12);
//    threshold( transformed_thresholded, transformed_thresholded, 150, 255,CV_THRESH_BINARY );

    namedWindow("quadrilateral", CV_WINDOW_KEEPRATIO);
    imshow("quadrilateral", transformed);

    namedWindow("thresholded", CV_WINDOW_KEEPRATIO);
    imshow("thresholded", transformed_thresholded);

    namedWindow("dst", CV_WINDOW_KEEPRATIO);
    imshow("dst",dst);

    namedWindow("src", CV_WINDOW_KEEPRATIO);
    imshow("src",src);

    imwrite("dst.jpg",dst);
    imwrite("src.jpg",src);
    imwrite("transformed.jpg",transformed);
    waitKey();
   }
   else
    cout<<"Make sure that your are getting 4 corner using approxPolyDP..."<<endl;
  return 0;
  }

