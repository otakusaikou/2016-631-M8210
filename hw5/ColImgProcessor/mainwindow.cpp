#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;
using namespace cv;
bool DEBUG = false;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//
// Read the color BMP or JPEG image file and show it on the main window
//
void MainWindow::on_actionOpen_triggered()
{
    /**************************************************DEBUGMODE**************************************************/
    QString tempFilename;
    if (!DEBUG) {
        tempFilename = QFileDialog::getOpenFileName(this,
        tr("Open Image"), ".",
        tr("Image Files (BMP (*.bmp);;JPEG (*.jpg *.jpeg)"));
    } else {
        tempFilename = "/home/otakusaikou/2016-631-M8210/hw5/img/HW05-Part 3-01.bmp";
    }

    if(!tempFilename.isEmpty() && !tempFilename.isNull())
    {
        filename = tempFilename;

        // Open image file
        imgSrc.release();       // Free image memory before reading new image
        imgSrc = imread(filename.toStdString().data());
        if (!imgSrc.data)
        {
            msgBox = QMessageBox::warning(this,
                         tr("File I/O Error"),
                         tr("Can't open the image file.\n"));
            return;
        }
    } else
    {                    // The case with empty file name
        return;
    }

    /* For GUI */
    this->setWindowTitle("ColImgProcessor - " + filename);      // Reset mainwindow title
    ui->actionSave->setEnabled(true);                           // For toolbox
    ui->actionReset->setEnabled(true);
    ui-> rdoBtnGroupBox->setEnabled(true);                      // For color space convertion panel

    // Copy the source image to image buffer
    cvtColor(imgSrc, imgSrc, COLOR_BGR2RGB);
    imgSrc.convertTo(bufSrc, CV_32F);
    updateFigures(bufSrc, ui->srcImgLabel);         // Update the image label
}

//
// Save the result (*.bmp, *.jpg, *.jpeg)
//
void MainWindow::on_actionSave_triggered()
{
    // Get the output image file name
    // Set default extension for saving image
    QString tempFilename = QFileDialog::getSaveFileName(this,
        tr("Save Image"), filename.split(".", QString::SkipEmptyParts).at(0) + ".bmp",
        tr("Image Files (BMP (*.bmp);;JPEG (*.jpg *.jpeg)"));

    if(!tempFilename.isEmpty() && !tempFilename.isNull())
    {
        QString f = tempFilename.split(".", QString::SkipEmptyParts).at(0);
        QString e;
        // Check file extension
        if (tempFilename.split(".", QString::SkipEmptyParts).length() == 1)
        {
            e = ".bmp";
        } else {
            e = "." + tempFilename.split(".", QString::SkipEmptyParts).at(1);
        }

        Mat bufSave;

        // Save source image
        if (bufSrc.channels() > 1)
        {
            cvtColor(bufSrc, bufSave, COLOR_RGB2BGR);
            bufSave.convertTo(bufSave, CV_8U);
        }
        else
        {
            bufSrc.convertTo(bufSave, CV_8U);
        }

        imwrite((f + e).toStdString(), bufSave);

        // Save output image if the dst image label is not empty
        if (!bufDst.empty())
        {
            cvtColor(bufDst, bufSave, COLOR_RGB2BGR);
            bufSave.convertTo(bufSave, CV_8U);
            imwrite((f + "_false" + e).toStdString(), bufSave);
        }
    } else
    {                    // The case with empty file name
        return;
    }
}

//
// Reset the application state
//
void MainWindow::on_actionReset_triggered()
{
    init();

    // Copy the source image to image buffer
    imgSrc.convertTo(bufSrc, CV_32F);
    updateFigures(bufSrc, ui->srcImgLabel);
}

//
// Close the program
//
void MainWindow::on_actionExit_triggered()
{
    close();
}

//
// Change to RGB color space
//
void MainWindow::on_RGBRadioButton_clicked()
{
    imgSrc.convertTo(bufSrc, CV_32F);
    updateFigures(bufSrc, ui->srcImgLabel);
}

//
// Change to CMY color space
//
void MainWindow::on_CMYRadioButtion_clicked()
{
    imgSrc.convertTo(bufSrc, CV_32F);
    processor.convertMat(bufSrc, bufSrc, processor.RGB2CMY);
    updateFigures(bufSrc, ui->srcImgLabel);
}

//
// Change to HSI color space
//
void MainWindow::on_HSIRadioButtion_clicked()
{
    imgSrc.convertTo(bufSrc, CV_32F);
    processor.convertMat(bufSrc, bufSrc, processor.RGB2HSI);
    updateFigures(bufSrc, ui->srcImgLabel);
}

//
// Change to XYZ color space
//
void MainWindow::on_XYZRadioButtion_clicked()
{
    imgSrc.convertTo(bufSrc, CV_32F);
    processor.convertMat(bufSrc, bufSrc, processor.RGB2XYZ);
    updateFigures(bufSrc, ui->srcImgLabel);
}

//
// Change to L*a*b* color space
//
void MainWindow::on_LabRadioButtion_clicked()
{
    imgSrc.convertTo(bufSrc, CV_32F);
    processor.convertMat(bufSrc, bufSrc, processor.RGB2Lab);
    updateFigures(bufSrc, ui->srcImgLabel);
}

//
// Change to YUV color space
//
void MainWindow::on_YUVRadioButtion_clicked()
{
    imgSrc.convertTo(bufSrc, CV_32F);
    processor.convertMat(bufSrc, bufSrc, processor.RGB2YUV);
    updateFigures(bufSrc, ui->srcImgLabel);
}

//
// Generate false color image and colorbar
//
void MainWindow::on_actionFalse_Color_triggered()
{
    // Get the setting for false color
    FCSettingDialog *fcSettingDialog = new FCSettingDialog(this);
    if (fcSettingDialog->exec() == QDialog::Rejected) return;

    // Get colors
    QModelIndex index;
    QColor color;
    vector<float> RVec;
    vector<float> GVec;
    vector<float> BVec;
    int colorNum = fcSettingDialog->model->rowCount();
    for (int i = 0; i < colorNum; ++i)
    {
        index = fcSettingDialog->model->index(i, 0, QModelIndex());
        color = index.data(Qt::BackgroundRole).value<QBrush>().color();
        RVec.push_back(color.red());
        GVec.push_back(color.green());
        BVec.push_back(color.blue());
    }

    // Generate color bar
    float interval = 256 / (colorNum - 1);
    float x;
    int x0;
    int x1;
    Mat colorbar = Mat::zeros(10, 256, CV_8UC3);
    for (int i = 0; i < 256; ++i)
    {
        x = i / interval;
        x0 = static_cast<int>(x);
        x1 = x0 + 1;
        int valR;
        int valG;
        int valB;
        if (x == (colorNum - 1))
        {
            valR = RVec[colorNum - 1];
            valG = GVec[colorNum - 1];
            valB = BVec[colorNum - 1];
        } else if (x == 0)
        {
            valR = RVec[x0];
            valG = GVec[x0];
            valB = BVec[x0];

        } else
        {
            valR = (x1-x)*RVec[x0] + (x-x0)*RVec[x1];
            valG = (x1-x)*GVec[x0] + (x-x0)*GVec[x1];
            valB = (x1-x)*BVec[x0] + (x-x0)*BVec[x1];
        }
        for (int j = 0; j < colorbar.rows; ++j)
            colorbar.at<Vec3b>(j, i) = Vec3b(valR, valG, valB);
    }
    updateFigures(colorbar, ui->colorbarLabel);

    // Convert image source to grayscale image
    if (bufSrc.channels() > 1)
    {
        cvtColor(bufSrc, bufSrc, COLOR_RGB2GRAY);
        updateFigures(bufSrc, ui->srcImgLabel);
    }

    // Generate false color image
    bufDst = Mat::zeros(bufSrc.size(), CV_8UC3);
    for (int i = 0; i < bufDst.rows;  ++i)
    {
        for (int j = 0; j < bufDst.cols; ++j)
        {
            x = bufSrc.at<float>(i, j) / interval;
            x0 = static_cast<int>(x);
            x1 = x0 + 1;
            int valR;
            int valG;
            int valB;
            if (x == (colorNum - 1))
            {
                valR = RVec[colorNum - 1];
                valG = GVec[colorNum - 1];
                valB = BVec[colorNum - 1];
            } else if (x == 0)
            {
                valR = RVec[x0];
                valG = GVec[x0];
                valB = BVec[x0];

            } else
            {
                valR = (x1-x)*RVec[x0] + (x-x0)*RVec[x1];
                valG = (x1-x)*GVec[x0] + (x-x0)*GVec[x1];
                valB = (x1-x)*BVec[x0] + (x-x0)*BVec[x1];
            }
            bufDst.at<Vec3b>(i, j) = Vec3b(valR, valG, valB);
        }
    }
    updateFigures(bufDst, ui->dstImgLabel);
}

//
// Reset all the attribures
//
void MainWindow::init()
{
    // For radiobuttons
    ui->RGBRadioButton->setEnabled(true);

    // Reset all attrubures
    bufSrc.release();
    bufDst.release();

    ui->colorbarLabel->clear();                         // Clean image labels
    ui->dstImgLabel->clear();
}

//
// Update the image label
//
void MainWindow::updateFigures(const Mat &src, QLabel *label)
{
    Mat bufShow;        // Buffer for showing image
    src.convertTo(bufShow, CV_8U);

    QImage QImg;

    // Convert opencv image matrix to QImage object
    if (src.channels() == 1)
    {
        QImg = QImage((const uchar*) (bufShow.data),
                            bufShow.cols, bufShow.rows, bufShow.step1(), QImage::Format_Grayscale8);
    } else
    {
        QImg = QImage((const uchar*) (bufShow.data),
                            bufShow.cols, bufShow.rows, bufShow.step1(), QImage::Format_RGB888);
    }

    // Resize the QImage to fit the label display
    QImage imgResize = QImg.scaled(label->width(), label->height(), Qt::KeepAspectRatio);

    // Update the pixelmap
    label->setPixmap(QPixmap::fromImage(imgResize));
}

//
// K-means clustering
// Reference: https://goo.gl/t9EXUd
//
void MainWindow::on_actionK_means_triggered()
{
    imgSrc.copyTo(bufSrc);

    getKDialog *getkdialog = new getKDialog(this);
    if (getkdialog->exec() == QDialog::Rejected) return;
    int K = getkdialog->K;

    Mat p = Mat::zeros(bufSrc.cols*bufSrc.rows, 5, CV_32F);
    Mat bestLabels, centers, clustered;
    vector<Mat> bgr;
    split(bufSrc, bgr);
    // i think there is a better way to split pixel bgr color
    for(int i=0; i<bufSrc.cols*bufSrc.rows; i++) {
        p.at<float>(i,0) = (i/bufSrc.cols) / bufSrc.rows;
        p.at<float>(i,1) = (i%bufSrc.cols) / bufSrc.cols;
        p.at<float>(i,2) = bgr[0].data[i] / 255.0;
        p.at<float>(i,3) = bgr[1].data[i] / 255.0;
        p.at<float>(i,4) = bgr[2].data[i] / 255.0;
    }

    kmeans(p, K, bestLabels,
            TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0),
            3, KMEANS_PP_CENTERS, centers);

    int colors[K];
    for(int i=0; i<K; i++) {
        colors[i] = 255/(i+1);
    }
    // i think there is a better way to do this mayebe some Mat::reshape?
    clustered = Mat(bufSrc.rows, bufSrc.cols, CV_32F);
    for(int i=0; i<bufSrc.cols*bufSrc.rows; i++) {
        clustered.at<float>(i/bufSrc.cols, i%bufSrc.cols) = (float)(colors[bestLabels.at<int>(0,i)]);
//      cout << bestLabels.at<int>(0,i) << " " <<
//              colors[bestLabels.at<int>(0,i)] << " " <<
//              clustered.at<float>(i/src.cols, i%src.cols) << " " <<
//              endl;
    }

    clustered.convertTo(clustered, CV_8U);
    imshow("1", clustered);
    imwrite("out.bmp", clustered);

    updateFigures(clustered, ui->dstImgLabel);
}
