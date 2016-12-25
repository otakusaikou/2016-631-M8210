#include "mainwindow.h"
#include "ui_mainwindow.h"
bool DEBUG = false;

using namespace std;
using namespace cv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("ImgWarper");
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
        tempFilename = "/home/otakusaikou/2016-631-M8210/hw6/img/IP_dog.bmp";
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

    // Initialize parameters
    init();

    /* For GUI */
    this->setWindowTitle("ImgWarper - " + filename);    // Reset mainwindow title
    ui->actionSave->setEnabled(true);                   // For toolbox
    ui->actionReset->setEnabled(true);

    // For control panel at right side
    ui->trapeGroupBox->setEnabled(true);
    ui->wavyGroupBox->setEnabled(true);
    ui->circleGroupBox->setEnabled(true);

    // Copy the source image to image buffers
    cvtColor(imgSrc, imgSrc, COLOR_BGR2RGB);
    imgSrc.convertTo(bufSrc, CV_32F);
    bufSrc.copyTo(bufDst);
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
            cvtColor(bufDst, bufSave, COLOR_RGB2BGR);
            bufSave.convertTo(bufSave, CV_8U);
        }
        else
        {
            bufDst.convertTo(bufSave, CV_8U);
        }

        imwrite((f + e).toStdString(), bufSave);
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

    // Copy the source image to image buffers
    imgSrc.convertTo(bufSrc, CV_32F);
    bufSrc.copyTo(bufDst);
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
// Switch the target to top edge
//
void MainWindow::on_ToRadioButton_clicked()
{
    // Change the flag
    edgePos = processor.TOP;

    // Reset the maximum value of spinbox and slider
    float height = bufSrc.rows;
    float width = bufSrc.cols;

    ui->lengthHorizontalSlider->setMaximum(width - 50);
    ui->lengthSpinBox->setMaximum(width - 50);
    ui->shiftHorizontalSlider->setMaximum(height - 50);
    ui->shiftSpinBox->setMaximum(height - 50);

    // Get 8 parameters
    Mat param;
    processor.getParam(width, height, length, shift, param, edgePos);

    // Create new image
    bufDst = Mat::zeros(bufSrc.size(), CV_32FC3);
    processor.resampleTrap(bufSrc, bufDst, param);

    updateFigures(bufDst, ui->srcImgLabel);
}

//
// Switch the target to bottom edge
//
void MainWindow::on_BoRadioButton_clicked()
{
    // Change the flag
    edgePos = processor.BOTTOM;

    // Reset the maximum value of spinbox and slider
    float height = bufSrc.rows;
    float width = bufSrc.cols;

    ui->lengthHorizontalSlider->setMaximum(width - 50);
    ui->lengthSpinBox->setMaximum(width - 50);
    ui->shiftHorizontalSlider->setMaximum(height - 50);
    ui->shiftSpinBox->setMaximum(height - 50);

    // Get 8 parameters
    Mat param;
    processor.getParam(width, height, length, shift, param, edgePos);

    // Create new image
    bufDst = Mat::zeros(bufSrc.size(), CV_32FC3);
    processor.resampleTrap(bufSrc, bufDst, param);

    updateFigures(bufDst, ui->srcImgLabel);
}

//
// Switch the target to left edge
//
void MainWindow::on_LeRadioButton_clicked()
{
    // Change the flag
    edgePos = processor.LEFT;

    // Reset the maximum value of spinbox and slider
    float height = bufSrc.rows;
    float width = bufSrc.cols;

    ui->lengthHorizontalSlider->setMaximum(height - 50);
    ui->lengthSpinBox->setMaximum(height - 50);
    ui->shiftHorizontalSlider->setMaximum(width - 50);
    ui->shiftSpinBox->setMaximum(width - 50);

    // Get 8 parameters
    Mat param;
    processor.getParam(width, height, length, shift, param, edgePos);

    // Create new image
    bufDst = Mat::zeros(bufSrc.size(), CV_32FC3);
    processor.resampleTrap(bufSrc, bufDst, param);

    updateFigures(bufDst, ui->srcImgLabel);
}

//
// Switch the target to right edge
//
void MainWindow::on_RiRadioButton_clicked()
{
    // Change the flag
    edgePos = processor.RIGHT;

    // Reset the maximum value of spinbox and slider
    float height = bufSrc.rows;
    float width = bufSrc.cols;

    ui->lengthHorizontalSlider->setMaximum(height - 50);
    ui->lengthSpinBox->setMaximum(height - 50);
    ui->shiftHorizontalSlider->setMaximum(width - 50);
    ui->shiftSpinBox->setMaximum(width - 50);

    // Get 8 parameters
    Mat param;
    processor.getParam(width, height, length, shift, param, edgePos);

    // Create new image
    bufDst = Mat::zeros(bufSrc.size(), CV_32FC3);
    processor.resampleTrap(bufSrc, bufDst, param);

    updateFigures(bufDst, ui->srcImgLabel);
}

//
// If length spinbox value changed, then update the length slider value
// and resample the image
//
void MainWindow::on_lengthSpinBox_valueChanged(int arg1)
{
    ui->lengthHorizontalSlider->setValue(arg1);
    length = static_cast<float>(arg1);

    float height = bufSrc.rows;
    float width = bufSrc.cols;

    // Get 8 parameters
    Mat param;
    processor.getParam(width, height, length, shift, param, edgePos);

    // Create new image
    bufDst = Mat::zeros(bufSrc.size(), CV_32FC3);
    processor.resampleTrap(bufSrc, bufDst, param);

    updateFigures(bufDst, ui->srcImgLabel);
}

//
// Update the length spinbox value when length slider moved
//
void MainWindow::on_lengthHorizontalSlider_sliderMoved(int position)
{
    ui->lengthSpinBox->setValue(position);
    length = static_cast<float>(position);
}

//
// Update shift slider value and resample the image
//
void MainWindow::on_shiftSpinBox_valueChanged(int arg1)
{
    ui->shiftHorizontalSlider->setValue(arg1);
    shift = static_cast<float>(arg1);

    float height = bufSrc.rows;
    float width = bufSrc.cols;

    // Get 8 parameters
    Mat param;
    processor.getParam(width, height, length, shift, param, edgePos);

    // Create new image
    bufDst = Mat::zeros(bufSrc.size(), CV_32FC3);
    processor.resampleTrap(bufSrc, bufDst, param);

    updateFigures(bufDst, ui->srcImgLabel);
}

//
// Update the shift spinbox value when shift slider moved
//
void MainWindow::on_shiftHorizontalSlider_sliderMoved(int position)
{
    ui->shiftSpinBox->setValue(position);
    shift = static_cast<float>(position);
}

//
// Update amplitude slider value and resample the image
//
void MainWindow::on_amplSpinBox_valueChanged(int arg1)
{
    ui->amplHorizontalSlider->setValue(arg1);
    ampl = static_cast<float>(arg1);

    // Create new image
    bufDst = Mat::zeros(bufSrc.size(), CV_32FC3);
    processor.resampleWavy(bufSrc, bufDst, ampl, freq);

    updateFigures(bufDst, ui->srcImgLabel);
}

//
// Update the amplitude spinbox value when amplitude slider moved
//
void MainWindow::on_amplHorizontalSlider_sliderMoved(int position)
{
    ui->amplSpinBox->setValue(position);
    ampl = static_cast<float>(position);
}

//
// Update frequency slider value and resample the image
//
void MainWindow::on_freqSpinBox_valueChanged(int arg1)
{
    ui->freqHorizontalSlider->setValue(arg1);
    freq = 0.1 * static_cast<double>(arg1) / 100;

    // Create new image
    bufDst = Mat::zeros(bufSrc.size(), CV_32FC3);
    processor.resampleWavy(bufSrc, bufDst, ampl, freq);

    updateFigures(bufDst, ui->srcImgLabel);
}

//
// Update frequency slider value and resample the image
//
void MainWindow::on_freqHorizontalSlider_sliderMoved(int position)
{
    ui->freqSpinBox->setValue(position);
    freq = 0.1 * static_cast<double>(position) / 100;
}

//
// Update zoom slider value and resample the image
//
void MainWindow::on_zoomSpinBox_valueChanged(int arg1)
{
    ui->zoomHorizontalSlider->setValue(arg1);
    zoom = 0.5 + (static_cast<double>(arg1)/100);

    // Create new image
    bufDst = Mat::zeros(bufSrc.size(), CV_32FC3);
    processor.resampleCircular(bufSrc, bufDst, zoom, strength);

    updateFigures(bufDst, ui->srcImgLabel);
}

//
// Update zoom slider value and resample the image
//
void MainWindow::on_zoomHorizontalSlider_sliderMoved(int position)
{
    ui->zoomSpinBox->setValue(position);
    zoom = 0.5 + (static_cast<double>(position)/100);
}

//
// Update strength slider value and resample the image
//
void MainWindow::on_streSpinBox_valueChanged(int arg1)
{
    ui->streHorizontalSlider->setValue(arg1);
    strength = 2 * (static_cast<double>(arg1)/100);

    // Create new image
    bufDst = Mat::zeros(bufSrc.size(), CV_32FC3);
    processor.resampleCircular(bufSrc, bufDst, zoom, strength);

    updateFigures(bufDst, ui->srcImgLabel);
}

//
// Update strength slider value and resample the image
//
void MainWindow::on_streHorizontalSlider_sliderMoved(int position)
{
    ui->streSpinBox->setValue(position);
    strength = 2 * (static_cast<double>(position)/100);
}

//
// Reset all the attribures
//
void MainWindow::init()
{
    // For radiobuttons
    ui->LeRadioButton->setChecked(true);

    // Reset all attribures
    edgePos = processor.LEFT;
    length = imgSrc.cols - 50;
    shift = 0;
    ampl = 0;
    freq = 0.001;
    zoom = 0.5;
    strength = 0;

    // For panel at right side
    ui->lengthHorizontalSlider->setMaximum(imgSrc.rows - 50);
    ui->lengthSpinBox->setMaximum(imgSrc.rows - 50);
    ui->lengthSpinBox->setValue(imgSrc.rows - 50);

    ui->shiftHorizontalSlider->setMaximum(length);
    ui->shiftSpinBox->setMaximum(length);
    ui->shiftSpinBox->setValue(0);

    ui->amplSpinBox->setValue(0);
    ui->freqSpinBox->setValue(0);
    ui->zoomSpinBox->setValue(0);
    ui->streSpinBox->setValue(0);

    // Release memory of image buffer
    bufSrc.release();
    bufDst.release();

    ui->srcImgLabel->clear();
}

//
// Update the image label
//
void MainWindow::updateFigures(const Mat &src, QLabel *label)
{
    if (src.empty())
        return;

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
