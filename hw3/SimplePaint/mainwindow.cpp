#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace QtCharts;
using namespace std;
using namespace cv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("SimplePaint");
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
        imgSrc = imread(fileName.toStdString().data());
        if (!imgSrc.data)
        {
            msgBox = QMessageBox::warning(this,
                         tr("File I/O Error"),
                         tr("Can't open the image file.\n"));
            return;
        }
    } else {                    // The case with empty file name
        return;
    }

    // Reset mainwindow title
    this->setWindowTitle("SimplePaint - " + fileName);

    // Copy the source image data to color image buffer
    cvtColor(imgSrc, imgSrc, COLOR_BGR2RGB);
    imgSrc.copyTo(bufRGB);

    // Set current image pointer to color image buffer
    curImg = &bufRGB;

    // Update the image label and histogram chart
    updateFigures();

    // Enable the save action
    ui->actionSave->setDisabled(false);

    // Enable the reset action
    ui->actionReset->setDisabled(false);

    // Enable the grayscale conversion function
    ui->actionConvert_to_GRAY_Type_A->setDisabled(false);
    ui->actionConvert_to_GRAY_Type_B->setDisabled(false);

    // Enable contrast and brightness slider
    ui->contrastSlider->setDisabled(false);
    ui->brightnessSlider->setDisabled(false);

    // Initialize the slider and alpha, beta values
    ui->contrastSlider->setValue(50);
    ui->brightnessSlider->setValue(50);
    alpha = 1.0;
    beta = 0.0;

    // Enable the resize function
    ui->resizeButton->setDisabled(false);
    ui->resizeSpinBox->setDisabled(false);

    // Enable the grayscale modification function
    ui->grayscaleComboBox->setDisabled(false);
    ui->changeButton->setDisabled(false);

    // Enable histogram equalization
    ui->actionHistogram_Equalization->setDisabled(false);

    // Enable spatial filtering operations
    ui->actionAveraging_Filter->setDisabled(false);
    ui->actionGaussian_Filter->setDisabled(false);
    ui->actionMedian_Filter->setDisabled(false);
    ui->actionMaxminum_Filter->setDisabled(false);
    ui->actionMinimum_Filter->setDisabled(false);
    ui->actionLaplacian_filter->setDisabled(false);
    ui->actionUnsharp_Masking->setDisabled(false);

    // Enable original size image rendering function
    ui->actionRender_original_size_image->setDisabled(false);
}

//
// Close the program
//
void MainWindow::on_actionExit_triggered()
{
    close();
}

//
// Convert the image into grayscale image using formula A in the homework assignment
//
void MainWindow::on_actionConvert_to_GRAY_Type_A_triggered()
{
    // Check whether the image label is empty
    if (!ui->imgLabel->pixmap())
    {
        msgBox = QMessageBox::warning(this,
                     tr("File I/O Error"),
                     tr("There are no images available.\n"));
        return;
    }

    // Don't do anything if current image is grayscale image
    if (curImg->channels() == 1) return;

    uchar newVal;
    // Create a new gray image buffer
    bufGray.release();
    bufGray = Mat::zeros(bufRGB.rows, bufRGB.cols, CV_8U);

    for (int i = 0; i < bufRGB.rows; ++i)
    {
        for (int j = 0; j < bufRGB.cols; ++j)
        {
            // Compute the new value
            newVal = (bufRGB.data[bufRGB.channels() * (bufRGB.cols*i + j)]
                      + bufRGB.data[bufRGB.channels() * (bufRGB.cols*i + j) + 1]
                      + bufRGB.data[bufRGB.channels() * (bufRGB.cols*i + j) + 2]) / 3.0;

            // Assign the new value to the image buffer on the display
            bufGray.data[bufRGB.cols*i + j] = newVal;
        }
    }

    // Set current image pointer to gray image buffer
    curImg = &bufGray;

    // Update the image label and histogram chart
    updateFigures();

    // Disable the grayscale conversion function after the conversion
    ui->actionConvert_to_GRAY_Type_A->setDisabled(true);
    ui->actionConvert_to_GRAY_Type_B->setDisabled(true);

    // Enable the thresholding function after the conversion
    ui->actionThreshold->setDisabled(false);
}

//
// Convert the image into grayscale image using formula B in the homework assignment
//
void MainWindow::on_actionConvert_to_GRAY_Type_B_triggered()
{
    // Check whether the image label is empty
    if (!ui->imgLabel->pixmap())
    {
        msgBox = QMessageBox::warning(this,
                     tr("File I/O Error"),
                     tr("There are no images available.\n"));
        return;
    }

    // Don't do anything if current image is grayscale image
    if (curImg->channels() == 1) return;

    uchar newVal;
    // Create a new gray image buffer
    bufGray.release();
    bufGray = Mat::zeros(bufRGB.rows, bufRGB.cols, CV_8U);

    for (int i = 0; i < bufRGB.rows; ++i)
    {
        for (int j = 0; j < bufRGB.cols; ++j)
        {
            // Compute the new value
            newVal = (0.299*bufRGB.data[bufRGB.channels() * (bufRGB.cols*i + j)]
                      + 0.587*bufRGB.data[bufRGB.channels() * (bufRGB.cols*i + j) + 1]
                      + 0.114*bufRGB.data[bufRGB.channels() * (bufRGB.cols*i + j) + 2]);

            // Assign the new value to the image buffer on the display
            bufGray.data[bufRGB.cols*i + j] = newVal;
        }
    }

    // Set current image pointer to gray image buffer
    curImg = &bufGray;

    // Update the image label and histogram chart
    updateFigures();

    // Disable the grayscale conversion function after the conversion
    ui->actionConvert_to_GRAY_Type_A->setDisabled(true);
    ui->actionConvert_to_GRAY_Type_B->setDisabled(true);

    // Enable the thresholding function after the conversion
    ui->actionThreshold->setDisabled(false);
}

//
// Save the result (*.bmp, *.jpg, *.jpeg)
//
void MainWindow::on_actionSave_triggered()
{
    // Check whether the image label is empty
    if (!ui->imgLabel->pixmap())
    {
        msgBox = QMessageBox::warning(this,
                     tr("File I/O Error"),
                     tr("There are no images available.\n"));
        return;
    }

    // Get the output image file name
    // Set default extension for saving image
    QString tempFileName = QFileDialog::getSaveFileName(this,
        tr("Save Image"), fileName.split(".", QString::SkipEmptyParts).at(0) + ".bmp",
        tr("Image Files (BMP (*.bmp);;JPEG (*.jpg *.jpeg)"));

    if(!tempFileName.isEmpty() && !tempFileName.isNull())
    {
        // Check file extension
        if (tempFileName.split(".", QString::SkipEmptyParts).length() == 1)
        {
            tempFileName += ".bmp";
        }

        // Copy the result to the image save buffer
        Mat bufSave;
        curImg->copyTo(bufSave);

        // If current image is color image, convert it from rgb to bgr
        if (bufSave.channels() > 1)
        {
            cvtColor(bufSave, bufSave, COLOR_RGB2BGR);
        }

        imwrite(tempFileName.toStdString(), bufSave);
    } else {                    // The case with empty file name
        return;
    }
}

//
// Reset the image label and histogram
//
void MainWindow::on_actionReset_triggered()
{
    // Check whether the image label is empty
    if (!ui->imgLabel->pixmap())
    {
        msgBox = QMessageBox::warning(this,
                     tr("File I/O Error"),
                     tr("There are no images available.\n"));
        return;
    }

    // Release memory
    bufRGB.release();
    bufGray.release();
    bufTmp.release();

    // Copy the source image data to color image buffer
    imgSrc.copyTo(bufRGB);

    // Set current image pointer to color image buffer
    curImg = &bufRGB;

    // Update the image label and histogram chart
    updateFigures();

    // Disable the grayscale conversion function first
    ui->actionConvert_to_GRAY_Type_A->setDisabled(false);
    ui->actionConvert_to_GRAY_Type_B->setDisabled(false);

    // Disable thresholding function
    ui->actionThreshold->setDisabled(true);

    // Reset the slider, alpha, beta values
    ui->contrastSlider->setValue(50);
    ui->brightnessSlider->setValue(50);
    alpha = 1.0;
    beta = 0.0;

    // Reset the spinbox and combobox values
    ui->resizeSpinBox->setValue(1.0);
    ui->grayscaleComboBox->setCurrentIndex(0);
}

//
// Apply thresholding function to get binary image
//
void MainWindow::on_actionThreshold_triggered()
{
    // Check whether the image label is empty
    if (!ui->imgLabel->pixmap())
    {
        msgBox = QMessageBox::warning(this,
                     tr("File I/O Error"),
                     tr("There are no images available.\n"));
        return;
    }

    // Don't do anything if there is no any grayscale image exists
    if (bufGray.empty()) return;

    for (int i = 0; i < bufGray.rows; ++i)
    {
        for (int j = 0; j < bufGray.cols; ++j)
        {
            // Assign the new value after thresholding
            if  (bufGray.data[bufGray.cols*i + j] > 127)
            {
                bufGray.data[bufGray.cols*i + j] = 255;
            } else {
                bufGray.data[bufGray.cols*i + j] = 0;
            }
        }
    }

    // Set current image pointer to gray image buffer
    curImg = &bufGray;

    // Update the image label and histogram chart
    updateFigures();

    // Disable the grayscale conversion function after the conversion
    ui->actionConvert_to_GRAY_Type_A->setDisabled(true);
    ui->actionConvert_to_GRAY_Type_B->setDisabled(true);
}

//
// Record current alpha value when user press the slider
//
void MainWindow::on_contrastSlider_sliderPressed()
{
    alpha = (2.0/100) * ui->contrastSlider->value();
}

//
// Change the contrast after user released the mouse button
//
void MainWindow::on_contrastSlider_sliderReleased()
{
    // The new_image = alpha * old_image
    for(int i = 0; i < curImg->rows; ++i)
    {
        for(int j = 0; j < curImg->cols; ++j)
        {
            for(int k = 0; k < curImg->channels(); ++k)
            {
                // Assign the new value to the image buffer on the display
                curImg->data[curImg->channels() * (curImg->cols*i + j) + k] =
                        saturate_cast<uchar>(
                            ((2.0/100) * ui->contrastSlider->value()/alpha) *
                            (curImg->data[curImg->channels() * (curImg->cols*i + j) + k]));
            }
        }
    }

    // Update the image label and histogram chart
    updateFigures();
}

//
// Record current beta value when user press the slider
//
void MainWindow::on_brightnessSlider_sliderPressed()
{
    beta = (255.0/100) * ui->brightnessSlider->value() - 127.5;
}

//
// Change the brightness after user released the mouse button
//
void MainWindow::on_brightnessSlider_sliderReleased()
{
    // The new_image = old_image + beta
    for(int i = 0; i < curImg->rows; ++i)
    {
        for(int j = 0; j < curImg->cols; ++j)
        {
            for(int k = 0; k < curImg->channels(); ++k)
            {
                // Assign the new value to the image buffer on the display
                curImg->data[curImg->channels() * (curImg->cols*i + j) + k] =
                        saturate_cast<uchar>(
                            curImg->data[curImg->channels() * (curImg->cols*i + j) + k] -
                            beta + (255.0/100) * ui->brightnessSlider->value() - 127.5);
            }
        }
    }

    // Update the image label and histogram chart
    updateFigures();
}

//
// Resize image
//
void MainWindow::on_resizeButton_clicked()
{
    double scale = ui->resizeSpinBox->value();
    bufTmp.release();
    if (curImg->channels() == 1)
    {                       // For grayscale image
        bufTmp = Mat::zeros(static_cast<int>(curImg->rows*scale), static_cast<int>(curImg->cols*scale), CV_8UC1);
    } else{                 // For color image
        bufTmp = Mat::zeros(static_cast<int>(curImg->rows*scale), static_cast<int>(curImg->cols*scale), CV_8UC3);
    }
    // Resample the image
    resize((*curImg), bufTmp);

    // Update the image label and histogram chart
    curImg->release();
    bufTmp.copyTo((*curImg));
    updateFigures();
}

//
// Change the grayscale level
//
void MainWindow::on_changeButton_clicked()
{
    int scale = pow(2, ui->grayscaleComboBox->count() - (ui->grayscaleComboBox->currentIndex()));

    // The new_image = floor(old_image * scale / 256) * (255 / (scale - 1))
    for(int i = 0; i < curImg->rows; ++i)
    {
        for(int j = 0; j < curImg->cols; ++j)
        {
            for(int k = 0; k < curImg->channels(); ++k)
            {
                // Assign the new value to the image buffer on the display
                curImg->data[curImg->channels() * (curImg->cols*i + j) + k] =
                         saturate_cast<uchar>(
                            floor((1.0*scale/256) * (curImg->data[curImg->channels() * (curImg->cols*i + j) + k])) *
                            (255/(scale-1)));
            }
        }
    }

    // Update the image label and histogram chart
    updateFigures();
}

//
// Perform histogram equalization
//
void MainWindow::on_actionHistogram_Equalization_triggered()
{
    vector<int> hist(256*curImg->channels(), 0);        // Histogram
    vector<int> cumHist(256*curImg->channels(), 0);     // Cumulative histogram
    vector<int> lut(256*curImg->channels(), 0);         // Look up table for histogram equalization
    double scale = 255.0/(curImg->rows*curImg->cols);

    // Calculate the histogram
    for (int i = 0; i < curImg->rows; ++i)
    {
        for (int j = 0; j < curImg->cols; ++j)
        {
            for (int k = 0; k < curImg->channels(); ++k)
            {
                ++hist.at(256*k + curImg->data[curImg->channels() * (curImg->cols*i + j) + k]);
            }
        }
    }

    // Calculate the cumulative histogram
    for (int k = 0; k < curImg->channels(); ++k)
    {
        for (int i = 0; i < 256; ++i)
        {
            if (i == 0)
            {
                cumHist.at(256*k + i) = hist.at(256*k + i);
            } else {
                cumHist.at(256*k + i) = hist.at(256*k + i) + cumHist.at(256*k + i - 1);
            }
        }
    }

    // Generate the look up table
    for (int k = 0; k < curImg->channels(); ++k)
    {
        for (int i = 0; i < 256; ++i)
        {
            lut.at(256*k + i) = cvRound(cumHist.at(256*k + i)*scale);
        }
    }

    // Equalize the image
    for (int i = 0; i < curImg->rows; ++i)
    {
        for (int j = 0; j < curImg->cols; ++j)
        {
            for (int k = 0; k < curImg->channels(); ++k)
            {
                curImg->data[curImg->channels() * (curImg->cols*i + j) + k] =
                    lut.at(256*k + curImg->data[curImg->channels() * (curImg->cols*i + j) + k]);
            }
        }
    }

    // Update the image label and histogram chart
    updateFigures();
}

//
// Smooth the image with averaging filter
//
void MainWindow::on_actionAveraging_Filter_triggered()
{
    // Get the mask size
    MaskSizeDialog *maskSizeDialog = new MaskSizeDialog(this);
    if (maskSizeDialog->exec() == QDialog::Rejected) return;
    int maskRows = maskSizeDialog->rows;
    int maskCols = maskSizeDialog->cols;

    // Check the mask size
    if (maskRows % 2 == 0 || maskCols % 2 == 0)
    {
        msgBox = QMessageBox::warning(this,
                     tr("Mask size error"),
                     tr("Mask row and col numbers must be odd.\n"));
        return;
    }

    bool timing = maskSizeDialog->timing;

    // Create the mask, mode0: averaging filter Mask
    MaskDialog *maskDialog = new MaskDialog(maskRows, maskCols, 0, this);
    if (maskDialog->exec() == QDialog::Rejected) return;

    // Create mask array
    QStandardItemModel *model = maskDialog->model;
    double *mask = new double[maskRows*maskCols];
    double m = maskDialog->m;
    for (int i = 0; i < maskRows; ++i)
    {
        for (int j = 0; j < maskCols; ++j)
        {
            mask[maskCols*i + j] = m*(model->index(i, j).data().toInt());
        }
    }

    // Generate new image
    bufTmp.release();
    if (curImg->channels() == 1)
    {               // For grayscale image
        bufTmp = Mat::zeros(curImg->rows, curImg->cols, CV_8UC1);
    } else{         // For color image
        bufTmp = Mat::zeros(curImg->rows, curImg->cols, CV_8UC3);
    }

    // Measure the computation time
    if (timing)
    {
        const clock_t begin_time = clock();
        convolve((*curImg), bufTmp, mask, maskRows, maskCols);
        QString msg = "It took " +
                QString::number((float(clock () - begin_time) / CLOCKS_PER_SEC), 'f', 3) +
                " seconds for the smoothing operation";
        msgBox = QMessageBox::information(this, tr("Computation time"), msg);
    } else {
        convolve((*curImg), bufTmp, mask, maskRows, maskCols);
    }

    // Update the image label and histogram chart
    curImg->release();
    bufTmp.copyTo((*curImg));
    updateFigures();
}

//
// Perform image blurring with gaussian filter
//
void MainWindow::on_actionGaussian_Filter_triggered()
{
    // Get the gaussian filter
    GMaskDialog *gMaskDialog = new GMaskDialog(this);
    if (gMaskDialog->exec() == QDialog::Rejected) return;
    int size = gMaskDialog->size;
    double sigma = gMaskDialog->sigma;
    double *gFilter = new double[size*size];
    bool timing = gMaskDialog->timing;

    // Check the filter size
    if (size % 2 == 0)
    {
        msgBox = QMessageBox::warning(this,
                     tr("Filter size error"),
                     tr("Filter size must be odd.\n"));
        return;
    }

    genGaussianFilter(size, sigma, gFilter);

    // Generate new image
    bufTmp.release();
    if (curImg->channels() == 1)
    {               // For grayscale image
        bufTmp = Mat::zeros(curImg->rows, curImg->cols, CV_8UC1);
    } else{         // For color image
        bufTmp = Mat::zeros(curImg->rows, curImg->cols, CV_8UC3);
    }

    if (timing)
    {
        const clock_t begin_time = clock();
        convolve((*curImg), bufTmp, gFilter, size, size);
        QString msg = "It took " +
                QString::number((float(clock () - begin_time) / CLOCKS_PER_SEC), 'f', 3) +
                " seconds for the smoothing operation";
        msgBox = QMessageBox::information(this, tr("Computation time"), msg);
    } else {
        convolve((*curImg), bufTmp, gFilter, size, size);
    }

    // Update the image label and histogram chart
    curImg->release();
    bufTmp.copyTo((*curImg));
    updateFigures();
}

//
// Smooth the image with median filter
//
void MainWindow::on_actionMedian_Filter_triggered()
{
    // Get the mask size
    MaskSizeDialog *maskSizeDialog = new MaskSizeDialog(this);
    if (maskSizeDialog->exec() == QDialog::Rejected) return;
    int maskRows = maskSizeDialog->rows;
    int maskCols = maskSizeDialog->cols;
    bool timing = maskSizeDialog->timing;

    // Check the mask size
    if (maskRows % 2 == 0 || maskCols % 2 == 0)
    {
        msgBox = QMessageBox::warning(this,
                     tr("Mask size error"),
                     tr("Mask row and col numbers must be odd.\n"));
        return;
    }

    // Generate new image
    bufTmp.release();
    if (curImg->channels() == 1)
    {               // For grayscale image
        bufTmp = Mat::zeros(curImg->rows, curImg->cols, CV_8UC1);
    } else{         // For color image
        bufTmp = Mat::zeros(curImg->rows, curImg->cols, CV_8UC3);
    }

    // Measure the computation time
    if (timing)
    {
        const clock_t begin_time = clock();
        median((*curImg), bufTmp, maskRows, maskCols);
        QString msg = "It took " +
                QString::number((float(clock () - begin_time) / CLOCKS_PER_SEC), 'f', 3) +
                " seconds for the smoothing operation";
        msgBox = QMessageBox::information(this, tr("Computation time"), msg);
    } else {
        median((*curImg), bufTmp, maskRows, maskCols);
    }

    // Update the image label and histogram chart
    curImg->release();
    bufTmp.copyTo((*curImg));
    updateFigures();
}

//
// Generate new image with maximum filter
//
void MainWindow::on_actionMaxminum_Filter_triggered()
{
    // Get the mask size
    MaskSizeDialog *maskSizeDialog = new MaskSizeDialog(this);
    if (maskSizeDialog->exec() == QDialog::Rejected) return;
    int maskRows = maskSizeDialog->rows;
    int maskCols = maskSizeDialog->cols;
    bool timing = maskSizeDialog->timing;

    // Check the mask size
    if (maskRows % 2 == 0 || maskCols % 2 == 0)
    {
        msgBox = QMessageBox::warning(this,
                     tr("Mask size error"),
                     tr("Mask row and col numbers must be odd.\n"));
        return;
    }

    // Generate new image
    bufTmp.release();
    if (curImg->channels() == 1)
    {               // For grayscale image
        bufTmp = Mat::zeros(curImg->rows, curImg->cols, CV_8UC1);
    } else{         // For color image
        bufTmp = Mat::zeros(curImg->rows, curImg->cols, CV_8UC3);
    }

    // Measure the computation time
    if (timing)
    {
        const clock_t begin_time = clock();
        max((*curImg), bufTmp, maskRows, maskCols);
        QString msg = "It took " +
                QString::number((float(clock () - begin_time) / CLOCKS_PER_SEC), 'f', 3) +
                " seconds for the smoothing operation";
        msgBox = QMessageBox::information(this, tr("Computation time"), msg);
    } else {
        max((*curImg), bufTmp, maskRows, maskCols);
    }

    // Update the image label and histogram chart
    curImg->release();
    bufTmp.copyTo((*curImg));
    updateFigures();
}

//
// Generate new image with minimum filter
//
void MainWindow::on_actionMinimum_Filter_triggered()
{
    // Get the mask size
    MaskSizeDialog *maskSizeDialog = new MaskSizeDialog(this);
    if (maskSizeDialog->exec() == QDialog::Rejected) return;
    int maskRows = maskSizeDialog->rows;
    int maskCols = maskSizeDialog->cols;
    bool timing = maskSizeDialog->timing;

    // Check the mask size
    if (maskRows % 2 == 0 || maskCols % 2 == 0)
    {
        msgBox = QMessageBox::warning(this,
                     tr("Mask size error"),
                     tr("Mask row and col numbers must be odd.\n"));
        return;
    }

    // Generate new image
    bufTmp.release();
    if (curImg->channels() == 1)
    {               // For grayscale image
        bufTmp = Mat::zeros(curImg->rows, curImg->cols, CV_8UC1);
    } else{         // For color image
        bufTmp = Mat::zeros(curImg->rows, curImg->cols, CV_8UC3);
    }

    // Measure the computation time
    if (timing)
    {
        const clock_t begin_time = clock();
        min((*curImg), bufTmp, maskRows, maskCols);
        QString msg = "It took " +
                QString::number((float(clock () - begin_time) / CLOCKS_PER_SEC), 'f', 3) +
                " seconds for the smoothing operation";
        msgBox = QMessageBox::information(this, tr("Computation time"), msg);
    } else {
        min((*curImg), bufTmp, maskRows, maskCols);
    }

    // Update the image label and histogram chart
    curImg->release();
    bufTmp.copyTo((*curImg));
    updateFigures();
}

//
// Sharpen the image with Laplacian filter
//
void MainWindow::on_actionLaplacian_filter_triggered()
{
    // Create the mask, mode1: Laplacian filter Mask
    MaskDialog *maskDialog = new MaskDialog(3, 3, 1, this);
    if (maskDialog->exec() == QDialog::Rejected) return;

    // Create mask array
    QStandardItemModel *model = maskDialog->model;
    double *mask = new double[9];
    double m = maskDialog->m;
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            mask[3*i + j] = m*(model->index(i, j).data().toInt());
        }
    }

    // Generate new image
    bufTmp.release();
    if (curImg->channels() == 1)
    {               // For grayscale image
        bufTmp = Mat::zeros(curImg->rows, curImg->cols, CV_8UC1);
    } else{         // For color image
        bufTmp = Mat::zeros(curImg->rows, curImg->cols, CV_8UC3);
    }
    convolve((*curImg), bufTmp, mask, 3, 3);

    // Update the image label and histogram chart
    curImg->release();
    bufTmp.copyTo((*curImg));
    updateFigures();
}

//
// Sharpen the image with Unsharp masking method
//
void MainWindow::on_actionUnsharp_Masking_triggered()
{
    // Get the gaussian filter
    UMaskDialog *uMaskDialog = new UMaskDialog(this);
    if (uMaskDialog->exec() == QDialog::Rejected) return;
    int size = uMaskDialog->size;
    double sigma = uMaskDialog->sigma;
    double *gFilter = new double[size*size];
    bool timing = uMaskDialog->timing;
    double K = uMaskDialog->k;

    // Check the filter size
    if (size % 2 == 0)
    {
        msgBox = QMessageBox::warning(this,
                     tr("Filter size error"),
                     tr("Filter size must be odd.\n"));
        return;
    }

    genGaussianFilter(size, sigma, gFilter);

    // Blur the original image
    bufTmp.release();
    if (curImg->channels() == 1)
    {               // For grayscale image
        bufTmp = Mat::zeros(curImg->rows, curImg->cols, CV_8UC1);
    } else{         // For color image
        bufTmp = Mat::zeros(curImg->rows, curImg->cols, CV_8UC3);
    }

    if (timing)
    {
        const clock_t begin_time = clock();
        convolve((*curImg), bufTmp, gFilter, size, size);
        QString msg = "It took " +
                QString::number((float(clock () - begin_time) / CLOCKS_PER_SEC), 'f', 3) +
                " seconds for the smoothing operation";
        msgBox = QMessageBox::information(this, tr("Computation time"), msg);
    } else {
        convolve((*curImg), bufTmp, gFilter, size, size);
    }

    // New image = original image + K * (original image - blurred image)
    for(int i = 0; i < curImg->rows; ++i)
    {
        for(int j = 0; j < curImg->cols; ++j)
        {
            for(int k = 0; k < curImg->channels(); ++k)
            {
                // Assign the new value to the image buffer on the display
                bufTmp.data[bufTmp.channels() * (bufTmp.cols*i + j) + k] =
                        saturate_cast<uchar>(
                            (1 + K) * (curImg->data[curImg->channels() * (curImg->cols*i + j) + k]) -
                            K * bufTmp.data[bufTmp.channels() * (bufTmp.cols*i + j) + k]);
            }
        }
    }

    // Update the image label and histogram chart
    curImg->release();
    bufTmp.copyTo((*curImg));
    updateFigures();
}

//
// Display the image with original size
//
void MainWindow::on_actionRender_original_size_image_triggered()
{
    // Open dialog without mainwindow as it's parent
    ImshowDialog *imshowDialog = new ImshowDialog(curImg, fileName);
    imshowDialog->setModal(false);
    imshowDialog->show();
}

//
// Update the image label and histogram chart
//
void MainWindow::updateFigures()
{
    QImage QImg;
    vector<int> hist(256*curImg->channels(), 0);        // Histogram

    // Conver opencv image matrix to QImage object
    if (curImg->channels() == 1)
    {                       // For grayscale image
        QImg = QImage((const uchar*) (curImg->data),
                            curImg->cols, curImg->rows, curImg->step1(), QImage::Format_Grayscale8);
    } else {                // For color image
        QImg = QImage((const uchar*) (curImg->data),
                            curImg->cols, curImg->rows, curImg->step1(), QImage::Format_RGB888);
    }

    // Calculate histogram
    for (int i = 0; i < curImg->rows; ++i)
    {
        for (int j = 0; j < curImg->cols; ++j)
        {
            for (int k = 0; k < curImg->channels(); ++k)
            {
                ++hist.at(256*k + curImg->data[curImg->channels() * (curImg->cols*i + j) + k]);
            }
        }
    }

    // Resize the QImage to fit the label display
    QImage imgResize = QImg.scaled(ui->imgLabel->width(), ui->imgLabel->height(), Qt::KeepAspectRatio);

    // Update the pixelmap
    ui->imgLabel->setPixmap(QPixmap::fromImage(imgResize));

    // Create QValueAxis object for showing the histogram value
    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0, 255);
    axisX->setTickCount(16);
    axisX->setLabelFormat("%d");

    // Create QBarSet and QBarSeries object for bar plot
    QBarSet *set;
    QBarSeries *series = new QBarSeries();
    QColor colorArray[3] = {Qt::red, Qt::green, Qt::blue};
    for (int k = 0; k < curImg->channels(); ++k)
    {
        set = new QBarSet("Histogram");
        for(int i = 0; i < 256; ++i)
        {
            *set << hist.at(256*k + i);
        }
        set->setBorderColor(colorArray[k]);
        set->setColor(colorArray[k]);
        series->append(set);
    }

    // Create QChart object for histogram
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Histogram");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(false);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    while(!ui->histogram->isEmpty())
    {
        // Clear the horizontal layout content if there is any
        ui->histogram->removeItem(ui->histogram->itemAt(0));
    }

    // Update the histogram
    ui->histogram->addWidget(chartView);
}

//
// Resample the image with bilinear interpolation
//
void MainWindow::resize(const Mat &imgSrc, Mat &imgDst)
{
    int x0;
    int x1;
    int y0;
    int y1;
    uchar Ia;
    uchar Ib;
    uchar Ic;
    uchar Id;
    double wa;
    double wb;
    double wc;
    double wd;
    double x;
    double y;
    double scaleX = 1.0*imgSrc.cols/imgDst.cols;
    double scaleY = 1.0*imgSrc.rows/imgDst.rows ;

    for (int i = 0; i < imgDst.rows; ++i)
    {
        for (int j = 0; j < imgDst.cols; ++j)
        {
            for (int k = 0; k < imgDst.channels(); ++k)
            {
                x = j*scaleX;
                y = i*scaleY;
                // Get coordinates of nearest four points
                x0 = cvFloor(x);
                if (x0 > (imgSrc.cols - 1)) x0 = (imgSrc.cols - 1);

                x1 = x0 + 1;
                if (x1 >= (imgSrc.cols - 1)) x1 = (imgSrc.cols - 1);

                y0 = cvFloor(y);
                if (y0 >= (imgSrc.rows - 1)) y0 = (imgSrc.rows - 1);

                y1 = y0 + 1;
                if (y1 >= (imgSrc.rows - 1)) y1 = (imgSrc.rows - 1);

                // Get intensity of nearest four points
                Ia = imgSrc.data[imgSrc.channels() * (imgSrc.cols*y0 + x0) + k];      // Upper left corner
                Ib = imgSrc.data[imgSrc.channels() * (imgSrc.cols*y1 + x0) + k];      // Lower left corner
                Ic = imgSrc.data[imgSrc.channels() * (imgSrc.cols*y0 + x1) + k];      // Upper right corner
                Id = imgSrc.data[imgSrc.channels() * (imgSrc.cols*y1 + x1) + k];      // Lower right corner

                // Compute the weight of four points
                wa = (x1 - x)*(y1 - y);
                wb = (x1 - x)*(y - y0);
                wc = (x - x0)*(y1 - y);
                wd = (x - x0)*(y - y0);

                // Assign values to target image
                if (wa + wb + wc + wd == 0)
                {
                    imgDst.data[imgDst.channels() * (imgDst.cols*i + j) + k] = Ia;
                } else {
                    imgDst.data[imgDst.channels() * (imgDst.cols*i + j) + k] =
                            (wa*Ia + wb*Ib + wc*Ic + wd*Id);
                }
            }
        }
    }
}

//
// Reflect the image at the borders (e.g. col[-1] = col[1], col[-2] = col[2])
// Reference: https://goo.gl/6I3OKv
//
int MainWindow::reflect(const int &M, const int &x)
{
    if(x < 0)
    {
        return -x - 1;
    }
    if(x >= M)
    {
        return 2*M - x - 1;
    }
    return x;
}

//
// Perform 2-dimensional convolution
//
void MainWindow::convolve(const Mat &imgSrc, Mat &imgDst, const double *mask, const int &maskRows, const int &maskCols)
{
    double newVal;      // The new value
    int r1;             // The pixel index emcompassed by the mask
    int c1;
    // Traverse all the pixels in source image
    for (int i = 0; i < imgSrc.rows; ++i)
    {
        for (int j = 0; j < imgSrc.cols; ++j)
        {
            for (int k = 0; k < imgSrc.channels(); ++k)
            {
                // Traverse all the coefficients in the mask
                newVal = 0.0;
                for (int ii = -(maskRows/2); ii <= (maskRows/2); ++ii)
                {
                    for (int jj = -(maskCols/2); jj <= (maskCols/2); ++jj)
                    {
                        r1 = reflect(imgSrc.rows, i - ii);
                        c1 = reflect(imgSrc.cols, j - jj);
                        newVal += mask[(maskCols*(ii + (maskRows/2)) + (jj + (maskCols/2)))] *
                                imgSrc.data[imgSrc.channels() * (imgSrc.cols*r1 + c1) + k];
                    }
                }
                imgDst.data[imgDst.channels() * (imgDst.cols*i + j) + k] =
                        saturate_cast<uchar>(newVal);
            }
        }
    }
}

//
// Generate gaussian filter
//
void MainWindow::genGaussianFilter(const int &size, const double &sigma, double *gFilter)
{
    double r;
    double s = 2.0 * sigma * sigma;

    // Sum of all values for normalization
    double sum = 0.0;

    // Generate filter with given size
    for (int x = -(size/2); x <= (size/2); ++x)
    {
        for(int y = -(size/2); y <= (size/2); ++y)
        {
            r = sqrt(x*x + y*y);
            gFilter[size*(x+(size/2)) + (y+(size/2))] = (exp(-(r*r)/s)) / (M_PI * s);
            sum += gFilter[size*(x+(size/2)) + (y+(size/2))];
        }
    }

    // Normalize the filter
    for(int i = 0; i < size; ++i)
        for(int j = 0; j < size; ++j)
            gFilter[size*i + j] /= sum;
}

//
// Assign new value with median within the mask
//
void MainWindow::median(const Mat &imgSrc, Mat &imgDst, const int &maskRows, const int &maskCols)
{
    int r1;             // The pixel index emcompassed by the mask
    int c1;
    // Traverse all the pixels in source image
    for (int i = 0; i < imgSrc.rows; ++i)
    {
        for (int j = 0; j < imgSrc.cols; ++j)
        {
            for (int k = 0; k < imgSrc.channels(); ++k)
            {
                // Traverse all the values in the mask
                vector<uchar> valVec;      // Vector for finding the median
                for (int ii = -(maskRows/2); ii <= (maskRows/2); ++ii)
                {
                    for (int jj = -(maskCols/2); jj <= (maskCols/2); ++jj)
                    {
                        r1 = reflect(imgSrc.rows, i - ii);
                        c1 = reflect(imgSrc.cols, j - jj);
                        valVec.push_back(imgSrc.data[imgSrc.channels() * (imgSrc.cols*r1 + c1) + k]);
                    }

                }

                // Find the median
                sort(valVec.begin(), valVec.end());
                imgDst.data[imgDst.channels() * (imgDst.cols*i + j) + k] =
                        saturate_cast<uchar>(valVec[valVec.size() / 2]);
            }
        }
    }
}

//
// Assign new value with maximum gray vlaue within the mask
//
void MainWindow::max(const Mat &imgSrc, Mat &imgDst, const int &maskRows, const int &maskCols)
{
    uchar newVal;      // The new value
    int r1;             // The pixel index emcompassed by the mask
    int c1;
    // Traverse all the pixels in source image
    for (int i = 0; i < imgSrc.rows; ++i)
    {
        for (int j = 0; j < imgSrc.cols; ++j)
        {
            for (int k = 0; k < imgSrc.channels(); ++k)
            {
                // Traverse all the coefficients in the mask
                newVal = 0;
                for (int ii = -(maskRows/2); ii <= (maskRows/2); ++ii)
                {
                    for (int jj = -(maskCols/2); jj <= (maskCols/2); ++jj)
                    {
                        r1 = reflect(imgSrc.rows, i - ii);
                        c1 = reflect(imgSrc.cols, j - jj);
                        if (imgSrc.data[imgSrc.channels() * (imgSrc.cols*r1 + c1) + k] > newVal)
                        {
                            newVal = imgSrc.data[imgSrc.channels() * (imgSrc.cols*r1 + c1) + k];
                        }
                    }
                }
                imgDst.data[imgDst.channels() * (imgDst.cols*i + j) + k] =
                        saturate_cast<uchar>(newVal);
            }
        }
    }
}

//
// Assign new value with minimum gray vlaue within the mask
//
void MainWindow::min(const Mat &imgSrc, Mat &imgDst, const int &maskRows, const int &maskCols)
{
    uchar newVal;      // The new value
    int r1;             // The pixel index emcompassed by the mask
    int c1;
    // Traverse all the pixels in source image
    for (int i = 0; i < imgSrc.rows; ++i)
    {
        for (int j = 0; j < imgSrc.cols; ++j)
        {
            for (int k = 0; k < imgSrc.channels(); ++k)
            {
                // Traverse all the coefficients in the mask
                newVal = 255;
                for (int ii = -(maskRows/2); ii <= (maskRows/2); ++ii)
                {
                    for (int jj = -(maskCols/2); jj <= (maskCols/2); ++jj)
                    {
                        r1 = reflect(imgSrc.rows, i - ii);
                        c1 = reflect(imgSrc.cols, j - jj);
                        if (imgSrc.data[imgSrc.channels() * (imgSrc.cols*r1 + c1) + k] < newVal)
                        {
                            newVal = imgSrc.data[imgSrc.channels() * (imgSrc.cols*r1 + c1) + k];
                        }
                    }
                }
                imgDst.data[imgDst.channels() * (imgDst.cols*i + j) + k] =
                        saturate_cast<uchar>(newVal);
            }
        }
    }
}
