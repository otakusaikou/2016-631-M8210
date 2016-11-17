#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace std;
using namespace cv;

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
    QString tempFileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), ".",
        tr("Image Files (BMP (*.bmp);;JPEG (*.jpg *.jpeg)"));

    if(!tempFileName.isEmpty() && !tempFileName.isNull())
    {
        fileName = tempFileName;

        // Open image file
        imgSrc.release();       // Free image memory before reading new image
        imgSrc = imread(fileName.toStdString().data());
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
    this->setWindowTitle("FFTProcessor - " + fileName);         // Reset mainwindow title

    init();

    // Convert to grayscale
    if (imgSrc.channels() > 1)
        cvtColor(imgSrc, imgSrc, COLOR_BGR2GRAY);

    // Copy the source image to image buffer
    imgSrc.convertTo(bufSrc, CV_64F);
    updateFigures(bufSrc, ui->srcImgLabel);         // Update the image label

    // Initialize the mask property panel
    int maximum = (bufSrc.rows > bufSrc.cols ? bufSrc.rows : bufSrc.cols);
    ui->D0ValSlider->setMaximum(maximum/2);

    /* Create FFT image */
    // Transform the image from spatial domain to frequency domain
    bufFFTSrc = processor.getOptimalImg(bufSrc);    // Create optimized two band image
    dft(bufFFTSrc, bufFFTSrc);                      // The FFT need two band image for computation

    // Initialize the output FFT image buffer
    bufFFTDst = Mat::zeros(bufFFTSrc.size(), CV_64FC2);
}

//
// Close the program
//
void MainWindow::on_actionExit_triggered()
{
    close();
}

//
// Switch the source image label from frequency domain to spatial domain
//
void MainWindow::on_srcFreqRadioButton_clicked()
{
    Mat mag;
    toSpm(bufFFTSrc, mag);
    updateFigures(mag, ui->srcImgLabel);
}

//
// Switch the source image label from spatial domain to frequency domain
//
void MainWindow::on_srcSpaRadioButton_clicked()
{
    updateFigures(bufSrc, ui->srcImgLabel);
}

//
// Switch the output image label from frequency domain to spatial domain
//
void MainWindow::on_dstSpaRadioButton_clicked()
{
    updateFigures(bufDst, ui->dstImgLabel);
}

//
// Switch the output image label from spatial domain to frequency domain
//
void MainWindow::on_dstFreqRadioButton_clicked()
{
    Mat mag;
    toSpm(bufFFTDst, mag);
    updateFigures(mag, ui->dstImgLabel);
}

//
// Start Ideal filtering mode
//
void MainWindow::on_actionIdeal_filter_triggered()
{
    // Enable other mask filtering functions
    ui->actionButterworth_Filter->setEnabled(true);
    ui->actionGaussian_Filter->setEnabled(true);
    ui->actionHomomorphic_Filter->setEnabled(true);

    // Disable the function itself
    ui->actionIdeal_filter->setEnabled(false);

    // Enable the radiobutton in output image group box
    ui->dstSpaRadioButton->setEnabled(true);
    ui->dstFreqRadioButton->setEnabled(true);

    // Enable the slider for determining the cuts off radius
    ui->D0ValLabel->setEnabled(true);
    ui->D0ValSlider->setEnabled(true);
    ui->D0Val->setEnabled(true);

    // Enable the mask type switching radiobuttons
    ui->LPFRadioButton->setEnabled(true);
    ui->HPFRadiobutton->setEnabled(true);

    // Disable parameters for other type of mask
    ui->gammaHLabel->setEnabled(false);             // Parameter 'gammaH' in Homomorphic filter
    ui->gammaHSlider->setEnabled(false);
    ui->gammaHSlider->setValue(100);
    ui->gammaHVal->setEnabled(false);

    ui->gammaLLabel->setEnabled(false);             // Parameter 'gammaL' in Homomorphic filter
    ui->gammaLSlider->setEnabled(false);
    ui->gammaLSlider->setValue(0);
    ui->gammaLVal->setEnabled(false);

    ui->BMaskOrderLabel->setEnabled(false);         // Parameter 'n' in Butterworth filter
    ui->BMaskOrderSpinBox->setValue(1);
    ui->BMaskOrderSpinBox->setEnabled(false);

    ui->CLabel->setEnabled(false);                  // Parameter 'c' in Homomorphic filter
    ui->CSpinBox->setValue(1);
    ui->CSpinBox->setEnabled(false);

    mode = 0;           // Change the mode number
    applyMask();        // Update the mask

    // Create a normalized mask for the display
    Mat normMat;
    bufMask.copyTo(normMat);
    normMat *= 255;
    processor.shiftMat(normMat);
    updateFigures(normMat, ui->maskLabel);

    // Update the output image
    if (ui->dstSpaRadioButton->isChecked())         // For spatial domain
    {
        updateFigures(bufDst, ui->dstImgLabel);
    } else {                                        // For frequency domain
        Mat mag;
        toSpm(bufFFTDst, mag);
        updateFigures(mag, ui->dstImgLabel);
    }
}

//
// Start Butterworth filtering mode
//
void MainWindow::on_actionButterworth_Filter_triggered()
{
    // Enable other mask filtering functions
    ui->actionIdeal_filter->setEnabled(true);
    ui->actionGaussian_Filter->setEnabled(true);
    ui->actionHomomorphic_Filter->setEnabled(true);

    // Disable the function itself
    ui->actionButterworth_Filter->setEnabled(false);

    // Enable the radiobutton in output image group box
    ui->dstSpaRadioButton->setEnabled(true);
    ui->dstFreqRadioButton->setEnabled(true);

    // Enable the slider for determining the cuts off radius
    ui->D0ValLabel->setEnabled(true);
    ui->D0ValSlider->setEnabled(true);
    ui->D0Val->setEnabled(true);

    ui->BMaskOrderLabel->setEnabled(true);         // Parameter 'n' in Butterworth filter
    ui->BMaskOrderSpinBox->setValue(1);
    ui->BMaskOrderSpinBox->setEnabled(true);

    // Enable the mask type switching radiobuttons
    ui->LPFRadioButton->setEnabled(true);
    ui->HPFRadiobutton->setEnabled(true);

    // Disable parameters for other type of mask
    ui->gammaHLabel->setEnabled(false);             // Parameter 'gammaH' in Homomorphic filter
    ui->gammaHSlider->setEnabled(false);
    ui->gammaHSlider->setValue(100);
    ui->gammaHVal->setEnabled(false);

    ui->gammaLLabel->setEnabled(false);             // Parameter 'gammaL' in Homomorphic filter
    ui->gammaLSlider->setEnabled(false);
    ui->gammaLSlider->setValue(0);
    ui->gammaLVal->setEnabled(false);

    ui->CLabel->setEnabled(false);                  // Parameter 'c' in Homomorphic filter
    ui->CSpinBox->setValue(1);
    ui->CSpinBox->setEnabled(false);

    mode = 1;           // Change the mode number
    applyMask();        // Update the mask

    // Create a normalized mask for the display
    Mat normMat;
    bufMask.copyTo(normMat);
    normMat *= 255;
    processor.shiftMat(normMat);
    updateFigures(normMat, ui->maskLabel);

    // Update the output image
    if (ui->dstSpaRadioButton->isChecked())         // For spatial domain
    {
        updateFigures(bufDst, ui->dstImgLabel);
    } else {                                        // For frequency domain
        Mat mag;
        toSpm(bufFFTDst, mag);
        updateFigures(mag, ui->dstImgLabel);
    }
}

//
// Start Butterworth filtering mode
//
void MainWindow::on_actionGaussian_Filter_triggered()
{
    // Enable other mask filtering functions
    ui->actionIdeal_filter->setEnabled(true);
    ui->actionButterworth_Filter->setEnabled(true);
    ui->actionHomomorphic_Filter->setEnabled(true);

    // Disable the function itself
    ui->actionGaussian_Filter->setEnabled(false);

    // Enable the radiobutton in output image group box
    ui->dstSpaRadioButton->setEnabled(true);
    ui->dstFreqRadioButton->setEnabled(true);

    // Enable the slider for determining the cuts off radius
    ui->D0ValLabel->setEnabled(true);
    ui->D0ValSlider->setEnabled(true);
    ui->D0Val->setEnabled(true);

    // Enable the mask type switching radiobuttons
    ui->LPFRadioButton->setEnabled(true);
    ui->HPFRadiobutton->setEnabled(true);

    // Disable parameters for other type of mask
    ui->gammaHLabel->setEnabled(false);             // Parameter 'gammaH' in Homomorphic filter
    ui->gammaHSlider->setEnabled(false);
    ui->gammaHSlider->setValue(100);
    ui->gammaHVal->setEnabled(false);

    ui->gammaLLabel->setEnabled(false);             // Parameter 'gammaL' in Homomorphic filter
    ui->gammaLSlider->setEnabled(false);
    ui->gammaLSlider->setValue(0);
    ui->gammaLVal->setEnabled(false);

    ui->BMaskOrderLabel->setEnabled(false);         // Enable the spinbox for constant 'n'
    ui->BMaskOrderSpinBox->setValue(1);
    ui->BMaskOrderSpinBox->setEnabled(false);

    ui->CLabel->setEnabled(false);                  // Parameter 'c' in Homomorphic filter
    ui->CSpinBox->setValue(1);
    ui->CSpinBox->setEnabled(false);

    mode = 2;           // Change the mode number
    applyMask();        // Update the mask

    // Create a normalized mask for the display
    Mat normMat;
    bufMask.copyTo(normMat);
    normMat *= 255;
    processor.shiftMat(normMat);
    updateFigures(normMat, ui->maskLabel);

    // Update the output image
    if (ui->dstSpaRadioButton->isChecked())         // For spatial domain
    {
        updateFigures(bufDst, ui->dstImgLabel);
    } else {                                        // For frequency domain
        Mat mag;
        toSpm(bufFFTDst, mag);
        updateFigures(mag, ui->dstImgLabel);
    }
}

//
// Start Homomorphic filtering mode
//
void MainWindow::on_actionHomomorphic_Filter_triggered()
{
    // Enable other mask filtering functions
    ui->actionIdeal_filter->setEnabled(true);
    ui->actionButterworth_Filter->setEnabled(true);
    ui->actionGaussian_Filter->setEnabled(true);

    // Disable the function itself
    ui->actionHomomorphic_Filter->setEnabled(false);

    // Enable the radiobutton in output image group box
    ui->dstSpaRadioButton->setEnabled(true);
    ui->dstFreqRadioButton->setEnabled(true);

    // Enable the slider for determining the cuts off radius
    ui->D0ValLabel->setEnabled(true);
    ui->D0ValSlider->setEnabled(true);
    ui->D0Val->setEnabled(true);

    // Enable sliders for determining the high and low value of Homomorphic filter
    ui->gammaHLabel->setEnabled(true);             // Parameter 'gammaH' in Homomorphic filter
    ui->gammaHSlider->setEnabled(true);
    ui->gammaHSlider->setValue(100);
    ui->gammaHVal->setEnabled(true);

    ui->gammaLLabel->setEnabled(true);             // Parameter 'gammaL' in Homomorphic filter
    ui->gammaLSlider->setEnabled(true);
    ui->gammaLSlider->setValue(0);
    ui->gammaLVal->setEnabled(true);

    ui->CLabel->setEnabled(true);                  // Parameter 'c' in Homomorphic filter
    ui->CSpinBox->setValue(1);
    ui->CSpinBox->setEnabled(true);

    // Disable the mask type switching radiobuttons
    ui->LPFRadioButton->setChecked(true);
    ui->LPFRadioButton->setEnabled(false);
    ui->HPFRadiobutton->setEnabled(false);

    // Disable parameters for other type of mask
    ui->BMaskOrderLabel->setEnabled(false);         // Parameter 'n' in Butterworth filter
    ui->BMaskOrderSpinBox->setValue(1);
    ui->BMaskOrderSpinBox->setEnabled(false);

    mode = 3;           // Change the mode number
    applyMask();        // Update the mask

    // Create a normalized mask for the display
    Mat normMat;
    bufMask.copyTo(normMat);
    normMat = (255 * (normMat - gammaL) / (gammaH - gammaL));

    processor.shiftMat(normMat);
    updateFigures(normMat, ui->maskLabel);

    // Update the output image
    if (ui->dstSpaRadioButton->isChecked())         // For spatial domain
    {
        updateFigures(bufDst, ui->dstImgLabel);
    } else {                                        // For frequency domain
        Mat mag;
        toSpm(bufFFTDst, mag);
        updateFigures(mag, ui->dstImgLabel);
    }
}

//
// Apply the mask after the slide bar of D0 changed
//
void MainWindow::on_D0ValSlider_valueChanged(int value)
{
    // Update the value label beside the slider
    QString valStr = QString::number(value);
    ui->D0Val->setText(valStr);

    D0 = ui->D0ValSlider->value();
    applyMask();        // Update the mask and output image buffer

    // Create a normalized mask for the display
    Mat normMat;
    bufMask.copyTo(normMat);
    normMat *= 255;
    processor.shiftMat(normMat);
    updateFigures(normMat, ui->maskLabel);

    // Update the output image
    if (ui->dstSpaRadioButton->isChecked())         // For spatial domain
    {
        updateFigures(bufDst, ui->dstImgLabel);
    } else {                                        // For frequency domain
        Mat mag;
        toSpm(bufFFTDst, mag);
        updateFigures(mag, ui->dstImgLabel);
    }
}

//
// Apply the mask after the slide bar of gammaH changed
//
void MainWindow::on_gammaHSlider_valueChanged(int value)
{
    // Limit the domain to be (1 <= gammaH)  AND (10 >= gammaH)
    double maximum = 10;
    double step = (maximum-1) / 100;
    double val = maximum - (100-value)*step;
    QString valStr = QString::number(val, 'f', 2);
    ui->gammaHVal->setText(valStr);

    gammaH = val;

    applyMask();        // Update the mask

    // Create a normalized mask for the display
    Mat normMat;
    bufMask.copyTo(normMat);
    normMat *= 255;
    processor.shiftMat(normMat);
    updateFigures(normMat, ui->maskLabel);

    // Update the output image
    if (ui->dstSpaRadioButton->isChecked())         // For spatial domain
    {
        updateFigures(bufDst, ui->dstImgLabel);
    } else {                                        // For frequency domain
        Mat mag;
        toSpm(bufFFTDst, mag);
        updateFigures(mag, ui->dstImgLabel);
    }
}

//
// Apply the mask after the slide bar of gammaL changed
//
void MainWindow::on_gammaLSlider_valueChanged(int value)
{
    // Limit the domain to be (1 <= gammaL)  AND (10 >= gammaL)
    double maximum = 1;
    double step = maximum / 100;
    double val = maximum - (100-value)*step;
    QString valStr = QString::number(val, 'f', 2);
    ui->gammaLVal->setText(valStr);

    gammaL = val;

    applyMask();        // Update the mask

    // Create a normalized mask for the display
    Mat normMat;
    bufMask.copyTo(normMat);
    normMat *= 255;
    processor.shiftMat(normMat);
    updateFigures(normMat, ui->maskLabel);

    // Update the output image
    if (ui->dstSpaRadioButton->isChecked())         // For spatial domain
    {
        updateFigures(bufDst, ui->dstImgLabel);
    } else {                                        // For frequency domain
        Mat mag;
        toSpm(bufFFTDst, mag);
        updateFigures(mag, ui->dstImgLabel);
    }
}

//
// Apply the mask after the spinbox of constant 'n' changed
//
void MainWindow::on_BMaskOrderSpinBox_valueChanged(int arg1)
{
    n = arg1;

    applyMask();        // Update the mask and output image buffer

    // Create a normalized mask for the display
    Mat normMat;
    bufMask.copyTo(normMat);
    normMat *= 255;
    processor.shiftMat(normMat);
    updateFigures(normMat, ui->maskLabel);

    // Update the output image
    if (ui->dstSpaRadioButton->isChecked())         // For spatial domain
    {
        updateFigures(bufDst, ui->dstImgLabel);
    } else {                                        // For frequency domain
        Mat mag;
        toSpm(bufFFTDst, mag);
        updateFigures(mag, ui->dstImgLabel);
    }
}

//
// Apply the mask after the spinbox of constant 'c' changed
//
void MainWindow::on_CSpinBox_valueChanged(int arg1)
{
    c = arg1;

    applyMask();        // Update the mask and output image buffer

    // Create a normalized mask for the display
    Mat normMat;
    bufMask.copyTo(normMat);
    normMat *= 255;
    processor.shiftMat(normMat);
    updateFigures(normMat, ui->maskLabel);

    // Update the output image
    if (ui->dstSpaRadioButton->isChecked())         // For spatial domain
    {
        updateFigures(bufDst, ui->dstImgLabel);
    } else {                                        // For frequency domain
        Mat mag;
        toSpm(bufFFTDst, mag);
        updateFigures(mag, ui->dstImgLabel);
    }
}

//
// Switch to lowpass filter
//
void MainWindow::on_LPFRadioButton_clicked()
{
    applyMask();        // Update and apply the mask

    Mat normMat;        // Create a normalized mask for the display
    bufMask.copyTo(normMat);
    normMat *= 255;
    processor.shiftMat(normMat);
    updateFigures(normMat, ui->maskLabel);

    // Update the output image
    if (ui->dstSpaRadioButton->isChecked())         // For spatial domain
    {
        updateFigures(bufDst, ui->dstImgLabel);
    } else {                                        // For frequency domain
        Mat mag;
        toSpm(bufFFTDst, mag);
        updateFigures(mag, ui->dstImgLabel);
    }
}

//
// Switch to highpass filter
//
void MainWindow::on_HPFRadiobutton_clicked()
{
    applyMask();        // Update and apply the mask

    Mat normMat;        // Create a normalized mask for the display
    bufMask.copyTo(normMat);
    normMat *= 255;
    processor.shiftMat(normMat);
    updateFigures(normMat, ui->maskLabel);

    // Update the output image
    if (ui->dstSpaRadioButton->isChecked())         // For spatial domain
    {
        updateFigures(bufDst, ui->dstImgLabel);
    } else {                                        // For frequency domain
        Mat mag;
        toSpm(bufFFTDst, mag);
        updateFigures(mag, ui->dstImgLabel);
    }
}

//
// Reset all the attribures
//
void MainWindow::init()
{
    ui->actionIdeal_filter->setEnabled(true);       // Enable mask filtering functions
    ui->actionButterworth_Filter->setEnabled(true);
    ui->actionGaussian_Filter->setEnabled(true);
    ui->actionHomomorphic_Filter->setEnabled(true);

    // Enable the radiobutton in source image group box
    ui->srcSpaRadioButton->setChecked(true);
    ui->srcSpaRadioButton->setEnabled(true);
    ui->srcFreqRadioButton->setEnabled(true);

    // Disable the radiobutton in output image group box
    ui->dstSpaRadioButton->setChecked(true);
    ui->dstSpaRadioButton->setEnabled(false);
    ui->dstFreqRadioButton->setEnabled(false);

    // Disable all widget in mask property panel
    ui->D0ValLabel->setEnabled(false);              // The cuts off radius
    ui->D0ValSlider->setEnabled(false);
    ui->D0ValSlider->setValue(0);
    ui->D0Val->setEnabled(false);

    ui->gammaHLabel->setEnabled(false);             // Parameter 'gammaH' in Homomorphic filter
    ui->gammaHSlider->setEnabled(false);
    ui->gammaHSlider->setValue(100);
    ui->gammaHVal->setEnabled(false);

    ui->gammaLLabel->setEnabled(false);             // Parameter 'gammaL' in Homomorphic filter
    ui->gammaLSlider->setEnabled(false);
    ui->gammaLSlider->setValue(0);
    ui->gammaLVal->setEnabled(false);

    ui->BMaskOrderLabel->setEnabled(false);         // Parameter 'n' in Butterworth filter
    ui->BMaskOrderSpinBox->setValue(1);
    ui->BMaskOrderSpinBox->setEnabled(false);

    ui->CLabel->setEnabled(false);                  // Parameter 'c' in Homomorphic filter
    ui->CSpinBox->setValue(1);
    ui->CSpinBox->setEnabled(false);

    // Enable the mask type switching radiobuttons
    ui->LPFRadioButton->setChecked(true);
    ui->LPFRadioButton->setEnabled(false);
    ui->HPFRadiobutton->setEnabled(false);

    // Reset all attrubures
    bufSrc.release();
    bufFFTSrc.release();
    bufMask.release();
    bufDst.release();
    bufFFTDst.release();

    D0 = 1;
    fftMax = 0;
    fftMin = 0;
    gammaH = 10.0;
    gammaL = 0.0;
    n = 1;
    c = 1;

    mode = -1;

    ui->maskLabel->clear();                         // Clean image labels
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

    // Conver opencv image matrix to QImage object
    if (src.channels() == 1)
    {                   // For grayscale image
        QImg = QImage((const uchar*) (bufShow.data),
                            bufShow.cols, bufShow.rows, bufShow.step1(), QImage::Format_Grayscale8);
    } else
    {                   // For color image
        QImg = QImage((const uchar*) (bufShow.data),
                            bufShow.cols, bufShow.rows, bufShow.step1(), QImage::Format_RGB888);
    }

    // Resize the QImage to fit the label display
    QImage imgResize = QImg.scaled(label->width(), label->height(), Qt::KeepAspectRatio);

    // Update the pixelmap
    label->setPixmap(QPixmap::fromImage(imgResize));
}

//
// Calculate the magnitude and enhance it for the display
//
void MainWindow::toSpm(const Mat &src, Mat &dst)
{
    // Calculate the magnitude
    dst = Mat::zeros(src.size(), CV_64F);
    processor.getMagnitude(src, dst);

    // Crop the spectrum, if it has an odd number of rows or columns
    // Using: Mat operator () ( const Rect&roi ) const
    dst = dst(Rect(0, 0, dst.cols & -2, dst.rows & -2));    // (*1)
    dst += Scalar::all(1);          // Apply log transformation (1 + log|F(u, v)|)
    log(dst, dst);

    if (fftMax == fftMin)           // Update the max and min magnitude of source FFT image
        minMaxLoc(dst, &fftMin, &fftMax);

    // Normalize the spectrum
    dst = (255 * (dst - fftMin) / (fftMax - fftMin));

    processor.shiftMat(dst);        // Shift the result for the display
}

//
// Apply the mask to image base on given mode number
//
void MainWindow::applyMask()
{
    // Update the mask
    if (mode == 0)              // Ideal filter
    {
        bool isLowpass = ui->LPFRadioButton->isChecked();
        bufMask = processor.getIMask(bufFFTSrc.size(), D0, isLowpass);
    } else if (mode == 1)       // Butterworth filter
    {
        bool isLowpass = ui->LPFRadioButton->isChecked();
        bufMask = processor.getBMask(bufFFTSrc.size(), D0, n, isLowpass);
    } else if (mode == 2)       // Gaussian filter
    {
        bool isLowpass = ui->LPFRadioButton->isChecked();
        bufMask = processor.getGMask(bufFFTSrc.size(), D0, isLowpass);
    } else if (mode == 3)       // Homomorphic filter
    {
        bufMask = processor.getHMask(bufFFTSrc.size(), gammaH, gammaL, c, D0);
        processor.HFiltering(bufSrc, bufMask, bufDst, bufFFTDst);
        return;
    }

    // Multiply the FFT image by mask and update the output image label
    processor.shiftMat(bufMask);
    processor.multiplyMat(bufFFTSrc, bufMask, bufFFTDst);

    // Transform the image from frequency domain to spatial domain
    dft(bufFFTDst, bufDst, DFT_INVERSE | DFT_SCALE | DFT_REAL_OUTPUT);
}
