#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>

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

    //flip(image,image,1); // processa imagem
       int largest_area=0;
       int largest_contour_index=0;
       Rect bounding_rect;

       Mat thr;

       cvtColor(image,image, CV_BGR2RGB); // muda a ordem das cores de BGR para RGB(Qimage operation)

       cvtColor( image, thr, COLOR_BGR2GRAY ); //Convert to gray
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

       drawContours( image, contours,largest_contour_index, Scalar( 0, 255, 0 ), 2 ); // Draw the largest contour using previously stored index.
   //    namedWindow("result", CV_WINDOW_NORMAL);
   //    imshow( "result", src );

       Mat cropImage = image(bounding_rect);

//       namedWindow("cropImage", CV_WINDOW_NORMAL);
//       imshow("cropImage", cropImage);
    Mat imagemFinal;

    cvtColor( cropImage, imagemFinal, COLOR_BGR2GRAY ); //Convert to gray

//           namedWindow("cropImage", CV_WINDOW_NORMAL);
//           imshow("cropImage", imagemFinal);
    QImage img = QImage((const unsigned char*)(imagemFinal.data), imagemFinal.cols, imagemFinal.rows, QImage::Format_RGB888);

    ui->label->setPixmap(QPixmap::fromImage(img));
    ui->label->resize(ui->label->pixmap()->size());
    namedWindow("Output Image");
    imshow("Output Image", imagemFinal);
}
