#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

int main(  )
{
    Mat src = imread( "ticket.jpg" );

    int largest_area=0;
    int largest_contour_index=0;
    Rect bounding_rect;

    Mat thr;
    cvtColor( src, thr, COLOR_BGR2GRAY ); //Convert to gray
    threshold( thr, thr, 125, 255, THRESH_BINARY ); //Threshold the gray

    vector<vector<Point> > contours; // Vector for storing contours

    findContours( thr, contours, RETR_CCOMP, CHAIN_APPROX_SIMPLE ); // Find the contours in the image

    for( size_t i = 0; i< contours.size(); i++ ) // iterate through each contour.
    {
        double area = contourArea( contours[i] );  //  Find the area of contour

        if( area > largest_area )
        {
            largest_area = area;
            largest_contour_index = i;               //Store the index of largest contour
            bounding_rect = boundingRect( contours[i] ); // Find the bounding rectangle for biggest contour
        }
    }

    drawContours( src, contours,largest_contour_index, Scalar( 0, 255, 0 ), 2 ); // Draw the largest contour using previously stored index.
//    namedWindow("result", CV_WINDOW_NORMAL);
//    imshow( "result", src );

    Mat cropImage = src(bounding_rect);
    namedWindow("cropImage", CV_WINDOW_NORMAL);
    imshow("cropImage", cropImage);

    waitKey();
    return 0;
}
