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
    this->setWindowTitle("ColImgProcessor");
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
    QString tempFilename;
    /******************************DEBUGMODE******************************/
    if (!DEBUG)
    {
        tempFilename = QFileDialog::getOpenFileName(this,
        tr("Open Image"), ".",
        tr("Image Files (BMP (*.bmp);;JPEG (*.jpg *.jpeg)"));
    /******************************DEBUGMODE******************************/
    } else
    {
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
    ui->actionFalse_Color->setEnabled(true);
    ui->actionK_means->setEnabled(true);

    ui->rdoBtnGroupBox->setEnabled(true);                       // For color space convertion panel
    ui->RGBRadioButton->setChecked(true);

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
        } else
        {
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
            if (bufDst.channels() > 1)
            {
                cvtColor(bufDst, bufSave, COLOR_RGB2BGR);
                bufSave.convertTo(bufSave, CV_8U);
            }
            else
            {
                bufDst.convertTo(bufSave, CV_8U);
            }
            imwrite((f + "_dst" + e).toStdString(), bufSave);
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
    int colorNum = fcSettingDialog->model->rowCount();
    Mat cmap = Mat::zeros(colorNum, 1, CV_32FC3);
    for (int i = 0; i < colorNum; ++i)
    {
        QModelIndex index = fcSettingDialog->model->index(i, 0, QModelIndex());
        QColor color = index.data(Qt::BackgroundRole).value<QBrush>().color();
        cmap.at<Vec3f>(i, 0) = Vec3f(color.red(),color.green(), color.blue());
    }
    delete fcSettingDialog;

    // Create colorbar
    Mat colorbarSrc = Mat::zeros(10, 256, CV_32F);
    for (int i = 0; i < colorbarSrc.rows; ++i)
        for (int j = 0; j < colorbarSrc.cols; ++j)
            colorbarSrc.at<float>(i, j) = j;

    Mat colorbarDst = Mat::zeros(10, 256, CV_32FC3);
    processor.getFCImg(colorbarSrc, colorbarDst, cmap);

    // Convert image source to grayscale image
    if (bufSrc.channels() > 1)
    {
        cvtColor(bufSrc, bufSrc, COLOR_RGB2GRAY);
    }

    // Create false color image
    bufDst = Mat::zeros(bufSrc.rows, bufSrc.cols, CV_32FC3);
    processor.getFCImg(bufSrc, bufDst, cmap);

    // Update figures
    updateFigures(colorbarDst, ui->colorbarLabel);
    updateFigures(bufSrc, ui->srcImgLabel);
    updateFigures(bufDst, ui->dstImgLabel);
}

//
// K-means clustering
// Reference: https://goo.gl/t9EXUd
//
void MainWindow::on_actionK_means_triggered()
{
    // Get the group number
    KMeansDialog *kMeansDialog = new KMeansDialog(this);
    if (kMeansDialog->exec() == QDialog::Rejected) return;
    int K = kMeansDialog->K;
    int kSize = kMeansDialog->kSize;
    bool hasBlur = kMeansDialog->hasBlur;
    delete kMeansDialog;

    // Check the mask size
    if (hasBlur && kSize % 2 == 0 )
    {
        msgBox = QMessageBox::warning(this,
                     tr("Mask size error"),
                     tr("The kernel size must be odd.\n"));
        return;
    }

    // Perform image clustering
    processor.imgClustering(bufSrc, bufDst, K, hasBlur, kSize);

    ui->colorbarLabel->clear();
    updateFigures(bufDst, ui->dstImgLabel);
}

//
// Copy the result image to source image buffer
//
void MainWindow::on_toLeftPushButtion_clicked()
{
    bufDst.copyTo(bufSrc);

    ui->colorbarLabel->clear();
    ui->dstImgLabel->clear();
    updateFigures(bufSrc, ui->srcImgLabel);
}

//
// Reset all the attribures
//
void MainWindow::init()
{
    // For radiobuttons
    ui->RGBRadioButton->setChecked(true);

    // Reset all attrubures
    bufSrc.release();
    bufDst.release();

    ui->colorbarLabel->clear();         // Clean image labels
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

    // Enable or Disable the toLeft buttion
    ui->toLeftPushButtion->setEnabled(ui->dstImgLabel->pixmap());
}
