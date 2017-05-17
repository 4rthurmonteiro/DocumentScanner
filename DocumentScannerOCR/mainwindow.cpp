#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QImage>
#include <QFile>
#include <QTextStream>
#include <QPixmap>
#include <QLabel>
#include "functions.cpp"
#include <QScrollBar>

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
    ui->label->setText("Imagem carregada..\nClique em DocumentScanner para continuar.\n");
    ui->label->adjustSize();
}

void MainWindow::on_processButton_clicked()
{

    // initilize tesseract OCR engine
     tesseract::TessBaseAPI *myOCR = new tesseract::TessBaseAPI();
     printf("Tesseract-ocr version: %s\n",myOCR->Version());

     if (myOCR->Init(NULL, "eng")) {
      fprintf(stderr, "Could not initialize tesseract.\n");
      exit(1);
     }

    // treat the image as a single text line
     tesseract::PageSegMode pagesegmode = static_cast<tesseract::PageSegMode>(11);
     myOCR->SetPageSegMode(pagesegmode);

     Mat thr;
     cvtColor(image,image, CV_BGR2RGB);

     cvtColor(image,thr,CV_BGR2GRAY);
     threshold( thr, thr, 120, 255,CV_THRESH_BINARY );

//     waitKey();

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

//     Mat cropped(image, boundRect);

//     namedWindow("cropped Image", CV_WINDOW_KEEPRATIO);
//     imshow("cropped Image",cropped);
//     waitKey();

     cout<<"the points of contours_poly[0] before filter are "<<contours_poly[0]<<endl;
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

        cout << quad_pts << endl;
        cout << squre_pts << endl;

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
        threshold(transformed_thresholded, transformed_thresholded, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

        // recognize text
        myOCR->TesseractRect( transformed_thresholded.data, 1, transformed_thresholded.step1(), 0, 0, transformed_thresholded.cols, transformed_thresholded.rows);
        const char *text1 = myOCR->GetUTF8Text();

        // remove "newline"
        string t1(text1);

        // print found text
        cout << t1.c_str() << endl;

        //write to file
        ofstream outfile ("out.txt");
        outfile << text1 << std::endl;

        ui->label->setText(QString::fromStdString(text1));
        ui->label->adjustSize();
        //ui->label->scroll(400,400);

        outfile.close();

       }
       else{
         ui->label->setText("Aviso:\nNão foi possível gerar um arquivo texto da imagem.\n\nAs possíveis soluções pode ser:\n ->Fundo da imagem sem um bom contraste.\n ->Objetos, dedos e outras coisas ao redor do documento.\n -> Foto tirada inclinada, o documento precisa estar alinhado o máximo possível.\n");
         ui->label->adjustSize();
        cout<<"Essa imagem não é possível de carregar!"<<endl;

}
     // destroy tesseract OCR engine
     myOCR->Clear();
     myOCR->End();



}
