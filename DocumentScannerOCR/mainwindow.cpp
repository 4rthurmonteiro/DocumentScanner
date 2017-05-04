#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <QFile>
#include <QTextStream>
#include <QPixmap>
#include <QLabel>

using namespace cv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //ui->processButton->setEnabled(false);

//    if(image.data){
//        ui->processButton->setEnabled(true);
//    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_openButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
         tr("Abrir Documento"), ".",
       tr("Image Files (*.png *.jpg *.jpeg *.bmp)"));

    image = imread(fileName.toLatin1().data());
    //namedWindow("Original Image");
    //imshow("Original Image", image);
}

void MainWindow::on_processButton_clicked()
{

    // initilize tesseract OCR engine
     tesseract::TessBaseAPI *myOCR = new tesseract::TessBaseAPI();
     printf("Tesseract-ocr version: %s\n",myOCR->Version());

     // printf("Leptonica version: %s\n",
     //        getLeptonicaVersion());

     if (myOCR->Init(NULL, "eng")) {
      fprintf(stderr, "Could not initialize tesseract.\n");
      exit(1);
     }

    // PSM_SPARSE_TEXT = 11,    ///< Find as much text as possible in no particular order.
    // PSM_SPARSE_TEXT_OSD = 12,  ///< Sparse text with orientation and script det.
    // treat the image as a single text line
     tesseract::PageSegMode pagesegmode = static_cast<tesseract::PageSegMode>(11);
     myOCR->SetPageSegMode(pagesegmode);

     Mat thr;
     cvtColor(image,image, CV_BGR2RGB);

     cvtColor(image,thr,CV_BGR2GRAY);
     threshold( thr, thr, 120, 255,CV_THRESH_BINARY );


//     namedWindow("thr_init", CV_WINDOW_KEEPRATIO);
//     imshow("thr_init",thr);
     waitKey();

     vector< vector <Point> > contours; // Vector for storing contour
     vector< Vec4i > hierarchy;
     int largest_contour_index=0;
     int largest_area=0;

     Mat dst(image.rows,image.cols,CV_8UC1,Scalar::all(0)); //create destination image
     findContours( thr.clone(), contours, hierarchy,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE ); // Find the contours in the image
     for( unsigned int i = 0; i< contours.size(); i++ ){
        double a=contourArea( contours[i],false);  //  Find the area of contour
        if(a>largest_area){
            largest_area=a;
            largest_contour_index=i;                //Store the index of largest contour
        }
     }


     drawContours( dst,contours, largest_contour_index, Scalar(255,255,255),CV_FILLED, 8, hierarchy );
     vector<vector<Point> > contours_poly(1);
     approxPolyDP( Mat(contours[largest_contour_index]), contours_poly[0],40, true );
     Rect boundRect=boundingRect(contours[largest_contour_index]);

//     namedWindow("boundRect in the Image", CV_WINDOW_KEEPRATIO);
//     imshow("boundRect in the Image",Mat(image,boundRect));

     cout<<"the points of contours_poly[0] are "<<contours_poly[0]<<endl;

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
        Mat transformed = Mat::zeros(image.rows, image.cols, CV_8UC3);
        warpPerspective(image, transformed, transmtx, image.size());
        Point P1=contours_poly[0][0];
        Point P2=contours_poly[0][1];
        Point P3=contours_poly[0][2];
        Point P4=contours_poly[0][3];


        line(image,P1,P2, Scalar(0,0,255),1,CV_AA,0);
        line(image,P2,P3, Scalar(0,0,255),1,CV_AA,0);
        line(image,P3,P4, Scalar(0,0,255),1,CV_AA,0);
        line(image,P4,P1, Scalar(0,0,255),1,CV_AA,0);
        rectangle(image,boundRect,Scalar(0,255,0),1,8,0);
        rectangle(transformed,boundRect,Scalar(0,255,0),1,8,0);

        Mat transformed_thresholded;

        transformed = transformed(boundRect); //cut image for the interesting region

        cvtColor(transformed,transformed_thresholded,CV_BGR2GRAY);
    //    adaptiveThreshold(transformed_thresholded,transformed_thresholded,255,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY,5,6);
        threshold( transformed_thresholded, transformed_thresholded, 120, 255,CV_THRESH_BINARY );

        // recognize text
    //    myOCR->TesseractRect( transformed.data, 1, transformed.step1(), boundRect.x, boundRect.y, boundRect.width, boundRect.height);
        myOCR->TesseractRect( transformed_thresholded.data, 1, transformed_thresholded.step1(), 0, 0, transformed_thresholded.cols, transformed_thresholded.rows);
        const char *text1 = myOCR->GetUTF8Text();

        // remove "newline"
        string t1(text1);
        // t1.erase(std::remove(t1.begin(), t1.end(), '\n'), t1.end());

        // print found text
        cout << t1.c_str() << endl;

        //write to file
        ofstream outfile ("out.txt");
        outfile << text1 << std::endl;
        ui->label->setText(QString::fromStdString(text1));
        ui->label->adjustSize();
        outfile.close();

       }
       else{
        cout<<"Make sure that your are getting 4 corner using approxPolyDP..."<<endl;

}
     // destroy tesseract OCR engine
     myOCR->Clear();
     myOCR->End();



}
