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

    // Reset mainwindow title
    this->setWindowTitle("SimplePaint - " + fileName);

    // Copy the source image data to color image buffer
    cvtColor(imgSrc, imgSrc, COLOR_BGR2RGB);
    imgSrc.copyTo(bufImg);

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

    // Enable the resize and grayscale changing function
    ui->resizeSpinBox->setDisabled(false);
    ui->resizeButton->setDisabled(false);
    ui->grayscaleComboBox->setDisabled(false);
    ui->changeButton->setDisabled(false);

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

    // Enable edge detection methods
    ui->actionMarr_Hildreth_Edge_Detector->setDisabled(false);
    ui->actionSobel_Edge_Detector->setDisabled(false);

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
    if (bufImg.channels() == 1) return;

    // Create a template image buffer
    Mat tmpImg = Mat::zeros(bufImg.rows, bufImg.cols, CV_8U);
    for (int i = 0; i < bufImg.rows; ++i)
    {
        for (int j = 0; j < bufImg.cols; ++j)
        {
            // Compute the new value
            uchar newVal = (
                    bufImg.at<Vec3b>(i, j)[0] +
                    bufImg.at<Vec3b>(i, j)[1] +
                    bufImg.at<Vec3b>(i, j)[2]) / 3.0;

            // Assign the new value to the template image buffer
            tmpImg.at<uchar>(i, j) = newVal;
        }
    }

    // Copy the content of template image to current image buffer
    tmpImg.copyTo(bufImg);

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
    if (bufImg.channels() == 1) return;

    // Create a template image buffer
    Mat tmpImg = Mat::zeros(bufImg.rows, bufImg.cols, CV_8U);
    for (int i = 0; i < bufImg.rows; ++i)
    {
        for (int j = 0; j < bufImg.cols; ++j)
        {
            // Compute the new value
            uchar newVal = (
                    0.299*bufImg.at<Vec3b>(i, j)[0] +
                    0.587*bufImg.at<Vec3b>(i, j)[1] +
                    0.114*bufImg.at<Vec3b>(i, j)[2]);

            // Assign the new value to the template image buffer
            tmpImg.at<uchar>(i, j) = newVal;
        }
    }

    // Copy the content of template image to current image buffer
    tmpImg.copyTo(bufImg);

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
        bufImg.copyTo(bufSave);

        // If current image is color image, convert it from rgb to bgr
        if (bufSave.channels() > 1)
        {
            cvtColor(bufSave, bufSave, COLOR_RGB2BGR);
        }

        imwrite(tempFileName.toStdString(), bufSave);
    } else
    {                    // The case with empty file name
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

    // Copy the source image data to color image buffer
    imgSrc.copyTo(bufImg);

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

    for (int i = 0; i < bufImg.rows; ++i)
    {
        for (int j = 0; j < bufImg.cols; ++j)
        {
            // Assign the new value after thresholding
            if (bufImg.at<uchar>(i, j) > 127)
            {
                bufImg.at<uchar>(i, j) = 255;
            } else
            {
                bufImg.at<uchar>(i, j) = 0;
            }
        }
    }

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
    for(int i = 0; i < bufImg.rows; ++i)
    {
        for(int j = 0; j < bufImg.cols; ++j)
        {
            // Assign the new value to the image buffer on the display
            if (bufImg.channels() == 1)
            {                           // For grayscale image
                bufImg.at<uchar>(i, j) = saturate_cast<uchar>(
                        ((2.0/100) * ui->contrastSlider->value()/alpha) * (bufImg.at<uchar>(i, j)));
            } else
            {                           // For color image
                for(int k = 0; k < bufImg.channels(); ++k)
                {
                    bufImg.at<Vec3b>(i, j)[k] = saturate_cast<uchar>(
                            ((2.0/100) * ui->contrastSlider->value()/alpha) * (bufImg.at<Vec3b>(i, j)[k]));
                }
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
    for(int i = 0; i < bufImg.rows; ++i)
    {
        for(int j = 0; j < bufImg.cols; ++j)
        {
            // Assign the new value to the image buffer on the display
            if (bufImg.channels() == 1)
            {                           // For grayscale image
                bufImg.at<uchar>(i, j) = saturate_cast<uchar>(
                        bufImg.at<uchar>(i, j) - beta + (255.0/100) * ui->brightnessSlider->value() - 127.5);
            } else
            {                           // For color image
                for(int k = 0; k < bufImg.channels(); ++k)
                {
                    bufImg.at<Vec3b>(i, j)[k] = saturate_cast<uchar>(
                            bufImg.at<Vec3b>(i, j)[k] - beta + (255.0/100) * ui->brightnessSlider->value() - 127.5);
                }
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
    Mat tmpImg;
    if (bufImg.channels() == 1)
    {                       // For grayscale image
        tmpImg = Mat::zeros(static_cast<int>(bufImg.rows*scale), static_cast<int>(bufImg.cols*scale), CV_8UC1);
    } else{                 // For color image
        tmpImg = Mat::zeros(static_cast<int>(bufImg.rows*scale), static_cast<int>(bufImg.cols*scale), CV_8UC3);
    }
    // Resample the image
    resize(bufImg, tmpImg);

    // Copy the content of template image to current image buffer
    tmpImg.copyTo(bufImg);

    // Update the image label and histogram chart
    updateFigures();
}

//
// Change the grayscale level
//
void MainWindow::on_changeButton_clicked()
{
    int scale = pow(2, ui->grayscaleComboBox->count() - (ui->grayscaleComboBox->currentIndex()));

    // The new_image = floor(old_image * scale / 256) * (255 / (scale - 1))
    for(int i = 0; i < bufImg.rows; ++i)
    {
        for(int j = 0; j < bufImg.cols; ++j)
        {
            // Assign the new value to the image buffer on the display
            if (bufImg.channels() == 1)
            {                           // For grayscale image
                bufImg.at<uchar>(i, j) =
                         saturate_cast<uchar>(
                            floor((1.0*scale/256) * (bufImg.at<uchar>(i, j))) *
                            (255/(scale-1)));
            } else
            {                           // For color image
                for(int k = 0; k < bufImg.channels(); ++k)
                {
                    bufImg.at<Vec3b>(i, j)[k] =
                             saturate_cast<uchar>(
                                floor((1.0*scale/256) * (bufImg.at<Vec3b>(i, j)[k])) *
                                (255/(scale-1)));
                }
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
    vector<int> hist(256*bufImg.channels(), 0);        // Histogram
    vector<int> cumHist(256*bufImg.channels(), 0);     // Cumulative histogram
    vector<int> lut(256*bufImg.channels(), 0);         // Look up table for histogram equalization
    double scale = 255.0/(bufImg.rows*bufImg.cols);

    // Calculate the histogram
    for (int i = 0; i < bufImg.rows; ++i)
    {
        for (int j = 0; j < bufImg.cols; ++j)
        {
            if (bufImg.channels() == 1)
            {                           // For grayscale image
                ++hist.at(bufImg.at<uchar>(i, j));
            } else
            {                           // For color image
                for(int k = 0; k < bufImg.channels(); ++k)
                {
                    ++hist.at(256*k + bufImg.at<Vec3b>(i, j)[k]);
                }
            }
        }
    }

    // Calculate the cumulative histogram
    for (int k = 0; k < bufImg.channels(); ++k)
    {
        for (int i = 0; i < 256; ++i)
        {
            if (i == 0)
            {
                cumHist.at(256*k + i) = hist.at(256*k + i);
            } else
            {
                cumHist.at(256*k + i) = hist.at(256*k + i) + cumHist.at(256*k + i - 1);
            }
        }
    }

    // Generate the look up table
    for (int k = 0; k < bufImg.channels(); ++k)
    {
        for (int i = 0; i < 256; ++i)
        {
            lut.at(256*k + i) = cvRound(cumHist.at(256*k + i)*scale);
        }
    }

    // Equalize the image
    for (int i = 0; i < bufImg.rows; ++i)
    {
        for (int j = 0; j < bufImg.cols; ++j)
        {
            if (bufImg.channels() == 1)
            {                           // For grayscale image
                bufImg.at<uchar>(i, j) = lut.at(bufImg.at<uchar>(i, j));
            } else
            {                           // For color image
                for(int k = 0; k < bufImg.channels(); ++k)
                {
                    bufImg.at<Vec3b>(i, j)[k] = lut.at(256*k + bufImg.at<Vec3b>(i, j)[k]);
                }
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
    bool timing = maskSizeDialog->timing;

    // Check the mask size
    if (maskRows % 2 == 0 || maskCols % 2 == 0)
    {
        msgBox = QMessageBox::warning(this,
                     tr("Mask size error"),
                     tr("Mask row and col numbers must be odd.\n"));
        return;
    }

    // Create the mask, mode0: averaging filter Mask
    MaskDialog *maskDialog = new MaskDialog(maskRows, maskCols, 0, this);
    if (maskDialog->exec() == QDialog::Rejected) return;

    // Measure the computation time
    clock_t begin_time;
    if (timing) begin_time = clock();   // Timing start

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

    // Create a template image buffer
    Mat tmpImg = Mat::zeros(bufImg.rows, bufImg.cols, bufImg.type());

    convolve(bufImg, tmpImg, mask, maskRows, maskCols, CV_8U);

    if (timing)                         // Timing end
    {
        QString msg = "It took " +
                QString::number((float(clock () - begin_time) / CLOCKS_PER_SEC), 'f', 3) +
                " seconds for the averaging filter operation";
        msgBox = QMessageBox::information(this, tr("Computation time"), msg);
    }

    // Copy the content of template image to current image buffer
    tmpImg.copyTo(bufImg);

    // Update the image label and histogram chart
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

    // Measure the computation time
    clock_t begin_time = 0;
    if (timing) begin_time = clock();   // Timing start

    genGaussianFilter(size, sigma, gFilter);

    // Create a template image buffer
    Mat tmpImg = Mat::zeros(bufImg.rows, bufImg.cols, bufImg.type());

    convolve(bufImg, tmpImg, gFilter, size, size, CV_8U);

    if (timing)                         // Timing end
    {
        QString msg = "It took " +
                QString::number((float(clock () - begin_time) / CLOCKS_PER_SEC), 'f', 3) +
                " seconds for the Gaussian image blurring";
        msgBox = QMessageBox::information(this, tr("Computation time"), msg);
    }

    // Copy the content of template image to current image buffer
    tmpImg.copyTo(bufImg);

    // Update the image label and histogram chart
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

    // Measure the computation time
    clock_t begin_time = 0;
    if (timing) begin_time = clock();   // Timing start

    // Create a template image buffer
    Mat tmpImg = Mat::zeros(bufImg.rows, bufImg.cols, bufImg.type());

    median(bufImg, tmpImg, maskRows, maskCols);

    if (timing)                         // Timing end
    {
        QString msg = "It took " +
                QString::number((float(clock () - begin_time) / CLOCKS_PER_SEC), 'f', 3) +
                " seconds for the median filter operation";
        msgBox = QMessageBox::information(this, tr("Computation time"), msg);
    }

    // Copy the content of template image to current image buffer
    tmpImg.copyTo(bufImg);

    // Update the image label and histogram chart
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

    // Measure the computation time
    clock_t begin_time = 0;
    if (timing) begin_time = clock();   // Timing start

    // Create a template image buffer
    Mat tmpImg = Mat::zeros(bufImg.rows, bufImg.cols, bufImg.type());

    max(bufImg, tmpImg, maskRows, maskCols);

    if (timing)                         // Timing end
    {
        QString msg = "It took " +
                QString::number((float(clock () - begin_time) / CLOCKS_PER_SEC), 'f', 3) +
                " seconds for the maximum filter operation";
        msgBox = QMessageBox::information(this, tr("Computation time"), msg);
    }

    // Copy the content of template image to current image buffer
    tmpImg.copyTo(bufImg);

    // Update the image label and histogram chart
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

    // Measure the computation time
    clock_t begin_time = 0;
    if (timing) begin_time = clock();   // Timing start

    // Create a template image buffer
    Mat tmpImg = Mat::zeros(bufImg.rows, bufImg.cols, bufImg.type());

    min(bufImg, tmpImg, maskRows, maskCols);

    if (timing)                         // Timing end
    {
        QString msg = "It took " +
                QString::number((float(clock () - begin_time) / CLOCKS_PER_SEC), 'f', 3) +
                " seconds for the minimum filter operation";
        msgBox = QMessageBox::information(this, tr("Computation time"), msg);
    }

    // Copy the content of template image to current image buffer
    tmpImg.copyTo(bufImg);

    // Update the image label and histogram chart
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

    // Create a template image buffer
    Mat tmpImg = Mat::zeros(bufImg.rows, bufImg.cols, bufImg.type());

    convolve(bufImg, tmpImg, mask, 3, 3, CV_8U);

    // Copy the content of template image to current image buffer
    tmpImg.copyTo(bufImg);

    // Update the image label and histogram chart
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

    // Check the mask size
    if (size % 2 == 0)
    {
        msgBox = QMessageBox::warning(this,
                     tr("Filter size error"),
                     tr("Filter size must be odd.\n"));
        return;
    }

    // Measure the computation time
    clock_t begin_time;
    if (timing) begin_time = clock();   // Timing start

    // Blur the original image
    genGaussianFilter(size, sigma, gFilter);

    // Create a template image buffer
    Mat tmpImg = Mat::zeros(bufImg.rows, bufImg.cols, bufImg.type());

    convolve(bufImg, tmpImg, gFilter, size, size, CV_8U);

    // New image = original image + K * (original image - blurred image)
    for(int i = 0; i < bufImg.rows; ++i)
    {
        for(int j = 0; j < bufImg.cols; ++j)
        {
            if (bufImg.channels() == 1)
            {                           // For grayscale image
                tmpImg.at<uchar>(i, j) =
                        saturate_cast<uchar>(
                            (1 + K) * (bufImg.at<uchar>(i, j)) -
                            K * (tmpImg.at<uchar>(i, j)));
            } else
            {                           // For color image
                for(int k = 0; k < bufImg.channels(); ++k)
                {
                    tmpImg.at<Vec3b>(i, j)[k] =
                            saturate_cast<uchar>(
                                (1 + K) * (bufImg.at<Vec3b>(i, j)[k]) -
                                K * (tmpImg.at<Vec3b>(i, j)[k]));
                }
            }
        }
    }

    if (timing)                         // Timing end
    {
        QString msg = "It took " +
                QString::number((float(clock () - begin_time) / CLOCKS_PER_SEC), 'f', 3) +
                " seconds for the unsharp masking";
        msgBox = QMessageBox::information(this, tr("Computation time"), msg);
    }

    // Copy the content of template image to current image buffer
    tmpImg.copyTo(bufImg);

    // Update the image label and histogram chart
    updateFigures();
}

//
// Detect the edge with Marr-Hildreth method
//
void MainWindow::on_actionMarr_Hildreth_Edge_Detector_triggered()
{
    // Get parameters of Marr-hildreth edge detector
    MHEdgeDialog *mHEdgeDialog = new MHEdgeDialog(this);
    if (mHEdgeDialog->exec() == QDialog::Rejected) return;
    int size = mHEdgeDialog->size;
    double sigma = mHEdgeDialog->sigma;
    double *gFilter = new double[size*size];
    double thres = mHEdgeDialog->thres;
    bool timing = mHEdgeDialog->timing;

    // Check the mask size
    if (size % 2 == 0)
    {
        msgBox = QMessageBox::warning(this,
                     tr("Filter size error"),
                     tr("Filter size must be odd.\n"));
        return;
    }

    // Measure the computation time
    clock_t begin_time = 0;
    if (timing) begin_time = clock();   // Timing start

    // If current image is color image, convert it to grayscale image
    if (bufImg.channels() == 3) cvtColor(bufImg, bufImg, COLOR_RGB2GRAY);

    // Blur the original image
    genGaussianFilter(size, sigma, gFilter);

    // Create a template image buffer
    Mat tmpImg = Mat::zeros(bufImg.rows, bufImg.cols, bufImg.type());

    convolve(bufImg, tmpImg, gFilter, size, size, CV_8U);
    tmpImg.copyTo(bufImg);

    // Compute the Laplacian of the Gaussian blurred image (LoG)
    tmpImg = Mat::zeros(bufImg.rows, bufImg.cols, CV_16S);
    QStandardItemModel *model = mHEdgeDialog->model;
    double *mask = new double[9];
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            mask[3*i + j] = (model->index(i, j).data().toInt());
        }
    }

    convolve(bufImg, tmpImg, mask, 3, 3, CV_16S);
    tmpImg.copyTo(bufImg);

    // Find the zero crossing points in the image
    tmpImg = Mat::zeros(bufImg.rows, bufImg.cols, CV_8U);
    zeroCross(bufImg, tmpImg, thres);

    if (timing)                         // Timing end
    {
        QString msg = "It took " +
                QString::number((float(clock () - begin_time) / CLOCKS_PER_SEC), 'f', 3) +
                " seconds for the Marr-Hildreth edge detection";
        msgBox = QMessageBox::information(this, tr("Computation time"), msg);
    }

    // Copy the content of template image to current image buffer
    tmpImg.copyTo(bufImg);

    // Update the image label and histogram chart
    updateFigures();

    // Disable the grayscale conversion function
    ui->actionConvert_to_GRAY_Type_A->setDisabled(true);
    ui->actionConvert_to_GRAY_Type_B->setDisabled(true);
}

//
// Detect the edge with Sobel mask
//
void MainWindow::on_actionSobel_Edge_Detector_triggered()
{
    // Get threshold of Sobel edge detector
    SMaskDialog *sMaskDialog = new SMaskDialog(this);
    if (sMaskDialog->exec() == QDialog::Rejected) return;
    double thres = sMaskDialog->thres;

    // If current image is color image, convert it to grayscale image
    if (bufImg.channels() == 3) cvtColor(bufImg, bufImg, COLOR_RGB2GRAY);

    Mat tmpImg = Mat::zeros(bufImg.rows, bufImg.cols, bufImg.type());

    sobel(bufImg, tmpImg, thres);
    tmpImg.copyTo(bufImg);

    // Update the image label and histogram chart
    updateFigures();

    // Disable the grayscale conversion function
    ui->actionConvert_to_GRAY_Type_A->setDisabled(true);
    ui->actionConvert_to_GRAY_Type_B->setDisabled(true);
}

//
// Display the image with original size
//
void MainWindow::on_actionRender_original_size_image_triggered()
{
    // Open dialog without mainwindow as it's parent
    ImshowDialog *imshowDialog = new ImshowDialog(bufImg, fileName);
    imshowDialog->setModal(false);
    imshowDialog->show();
}

//
// Update the image label and histogram chart
//
void MainWindow::updateFigures()
{
    QImage QImg;
    vector<int> hist(256*bufImg.channels(), 0);        // Histogram

    // Conver opencv image matrix to QImage object
    if (bufImg.channels() == 1)
    {                   // For grayscale image
        QImg = QImage((const uchar*) (bufImg.data),
                            bufImg.cols, bufImg.rows, bufImg.step1(), QImage::Format_Grayscale8);
    } else
    {                   // For color image
        QImg = QImage((const uchar*) (bufImg.data),
                            bufImg.cols, bufImg.rows, bufImg.step1(), QImage::Format_RGB888);
    }

    // Calculate histogram
    for (int i = 0; i < bufImg.rows; ++i)
    {
        for (int j = 0; j < bufImg.cols; ++j)
        {
            if (bufImg.channels() == 1)
            {                   // For grayscale image
                ++hist.at(bufImg.at<uchar>(i, j));
            } else
            {                   // For color image
                for (int k = 0; k < bufImg.channels(); ++k)
                {
                    ++hist.at(256*k + bufImg.at<Vec3b>(i, j)[k]);
                }
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
    for (int k = 0; k < bufImg.channels(); ++k)
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
    double x;
    double y;
    double scaleX = 1.0*imgSrc.cols/imgDst.cols;
    double scaleY = 1.0*imgSrc.rows/imgDst.rows ;

    for (int i = 0; i < imgDst.rows; ++i)
    {
        for (int j = 0; j < imgDst.cols; ++j)
        {
            if (bufImg.channels() == 1)
            {                           // For grayscale image
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
                Ia = imgSrc.at<uchar>(y0, x0);      // Upper left corner
                Ib = imgSrc.at<uchar>(y1, x0);      // Lower left corner
                Ic = imgSrc.at<uchar>(y0, x1);      // Upper right corner
                Id = imgSrc.at<uchar>(y1, x1);      // Lower right corner

                // Assign values to target image
                if (x0 == x1 && y0 == y1)
                {
                    imgDst.at<uchar>(i, j) = Ia;
                } else if (x0 == x1 && y0 != y1)
                {
                    imgDst.at<uchar>(i, j) = ((y1 - y)*Ia + (y - y0)*Ib);
                } else if (x0 != x1 && y0 == y1)
                {
                    imgDst.at<uchar>(i, j) = ((x1 - x)*Ia + (x - x0)*Ic);
                } else if (x0 != x1 && y0 != y1)
                {
                    imgDst.at<uchar>(i, j) = (
                                (x1 - x)*(y1 - y)*Ia +
                                (x1 - x)*(y - y0)*Ib +
                                (x - x0)*(y1 - y)*Ic +
                                (x - x0)*(y - y0)*Id);
                }

            } else
            {                           // For color image
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
                    Ia = imgSrc.at<Vec3b>(y0, x0)[k];      // Upper left corner
                    Ib = imgSrc.at<Vec3b>(y1, x0)[k];      // Lower left corner
                    Ic = imgSrc.at<Vec3b>(y0, x1)[k];      // Upper right corner
                    Id = imgSrc.at<Vec3b>(y1, x1)[k];      // Lower right corner

                    // Assign values to target image
                    if (x0 == x1 && y0 == y1)
                    {
                        imgDst.at<Vec3b>(i, j)[k] = Ia;
                    } else if (x0 == x1 && y0 != y1)
                    {
                        imgDst.at<Vec3b>(i, j)[k] = ((y1 - y)*Ia + (y - y0)*Ib);
                    } else if (x0 != x1 && y0 == y1)
                    {
                        imgDst.at<Vec3b>(i, j)[k] = ((x1 - x)*Ia + (x - x0)*Ic);
                    } else if (x0 != x1 && y0 != y1)
                    {
                        imgDst.at<Vec3b>(i, j)[k] = (
                                    (x1 - x)*(y1 - y)*Ia +
                                    (x1 - x)*(y - y0)*Ib +
                                    (x - x0)*(y1 - y)*Ic +
                                    (x - x0)*(y - y0)*Id);
                    }
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
void MainWindow::convolve(const Mat &imgSrc, Mat &imgDst, const double *mask, const int &maskRows, const int &maskCols, const int &depth)
{
    double newVal;      // The new value
    int r1;             // The pixel index emcompassed by the mask
    int c1;
    // Traverse all the pixels in source image
    for (int i = 0; i < imgSrc.rows; ++i)
    {
        for (int j = 0; j < imgSrc.cols; ++j)
        {
            // Traverse all the coefficients in the mask
            if (bufImg.channels() == 1)
            {                           // For grayscale image
                newVal = 0.0;
                for (int ii = -(maskRows/2); ii <= (maskRows/2); ++ii)
                {
                    for (int jj = -(maskCols/2); jj <= (maskCols/2); ++jj)
                    {
                        r1 = reflect(imgSrc.rows, i - ii);
                        c1 = reflect(imgSrc.cols, j - jj);
                        newVal += mask[(maskCols*(ii + (maskRows/2)) + (jj + (maskCols/2)))] * imgSrc.at<uchar>(r1, c1);
                    }
                }

                if (depth == CV_8U)
                {
                    imgDst.at<uchar>(i, j) = saturate_cast<uchar>(newVal);
                } else if (depth == CV_16S)
                {
                    imgDst.at<short>(i, j) = saturate_cast<short>(newVal);
                }
            } else
            {                           // For color image
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
                            newVal += mask[(maskCols*(ii + (maskRows/2)) + (jj + (maskCols/2)))] * imgSrc.at<Vec3b>(r1, c1)[k];
                        }
                    }

                    if (depth == CV_8U)
                    {
                        imgDst.at<Vec3b>(i, j)[k] = saturate_cast<uchar>(newVal);
                    } else if (depth == CV_16S)
                    {
                        imgDst.at<Vec3b>(i, j)[k] = saturate_cast<short>(newVal);
                    }
                }
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
            // Traverse all the coefficients in the mask
            if (bufImg.channels() == 1)
            {                           // For grayscale image
                vector<uchar> valVec;       // Vector for finding the median
                for (int ii = -(maskRows/2); ii <= (maskRows/2); ++ii)
                {
                    for (int jj = -(maskCols/2); jj <= (maskCols/2); ++jj)
                    {
                        r1 = reflect(imgSrc.rows, i - ii);
                        c1 = reflect(imgSrc.cols, j - jj);
                        valVec.push_back(imgSrc.at<uchar>(r1, c1));
                    }

                }

                // Find the median
                sort(valVec.begin(), valVec.end());
                imgDst.at<uchar>(i, j) = saturate_cast<uchar>(valVec[valVec.size() / 2]);
            } else
            {                           // For color image
                for (int k = 0; k < imgSrc.channels(); ++k)
                {
                    vector<uchar> valVec;       // Vector for finding the median
                    for (int ii = -(maskRows/2); ii <= (maskRows/2); ++ii)
                    {
                        for (int jj = -(maskCols/2); jj <= (maskCols/2); ++jj)
                        {
                            r1 = reflect(imgSrc.rows, i - ii);
                            c1 = reflect(imgSrc.cols, j - jj);
                            valVec.push_back(imgSrc.at<Vec3b>(r1, c1)[k]);
                        }

                    }

                    // Find the median
                    sort(valVec.begin(), valVec.end());
                    imgDst.at<Vec3b>(i, j)[k] = saturate_cast<uchar>(valVec[valVec.size() / 2]);
                }
            }
        }
    }
}

//
// Assign new value with maximum gray vlaue within the mask
//
void MainWindow::max(const Mat &imgSrc, Mat &imgDst, const int &maskRows, const int &maskCols)
{
    uchar newVal;       // The new value
    int r1;             // The pixel index emcompassed by the mask
    int c1;
    // Traverse all the pixels in source image
    for (int i = 0; i < imgSrc.rows; ++i)
    {
        for (int j = 0; j < imgSrc.cols; ++j)
        {
            // Traverse all the elements in the mask
            if (bufImg.channels() == 1)
            {                           // For grayscale image
                newVal = 0;
                for (int ii = -(maskRows/2); ii <= (maskRows/2); ++ii)
                {
                    for (int jj = -(maskCols/2); jj <= (maskCols/2); ++jj)
                    {
                        r1 = reflect(imgSrc.rows, i - ii);
                        c1 = reflect(imgSrc.cols, j - jj);
                        if (imgSrc.at<uchar>(r1, c1) > newVal)
                        {
                            newVal = imgSrc.at<uchar>(r1, c1);
                        }
                    }
                }
                imgDst.at<uchar>(i, j) = saturate_cast<uchar>(newVal);
            } else
            {                           // For color image
                for (int k = 0; k < imgSrc.channels(); ++k)
                {
                    newVal = 0;
                    for (int ii = -(maskRows/2); ii <= (maskRows/2); ++ii)
                    {
                        for (int jj = -(maskCols/2); jj <= (maskCols/2); ++jj)
                        {
                            r1 = reflect(imgSrc.rows, i - ii);
                            c1 = reflect(imgSrc.cols, j - jj);
                            if (imgSrc.at<Vec3b>(r1, c1)[k] > newVal)
                            {
                                newVal = imgSrc.at<Vec3b>(r1, c1)[k];
                            }
                        }
                    }
                    imgDst.at<Vec3b>(i, j)[k] = saturate_cast<uchar>(newVal);
                }
            }
        }
    }
}

//
// Assign new value with minimum gray vlaue within the mask
//
void MainWindow::min(const Mat &imgSrc, Mat &imgDst, const int &maskRows, const int &maskCols)
{
    uchar newVal;       // The new value
    int r1;             // The pixel index emcompassed by the mask
    int c1;
    // Traverse all the pixels in source image
    for (int i = 0; i < imgSrc.rows; ++i)
    {
        for (int j = 0; j < imgSrc.cols; ++j)
        {
            // Traverse all the elements in the mask
            if (bufImg.channels() == 1)
            {                           // For grayscale image
                newVal = 255;
                for (int ii = -(maskRows/2); ii <= (maskRows/2); ++ii)
                {
                    for (int jj = -(maskCols/2); jj <= (maskCols/2); ++jj)
                    {
                        r1 = reflect(imgSrc.rows, i - ii);
                        c1 = reflect(imgSrc.cols, j - jj);
                        if (imgSrc.at<uchar>(r1, c1) < newVal)
                        {
                            newVal = imgSrc.at<uchar>(r1, c1);
                        }
                    }
                }
                imgDst.at<uchar>(i, j) = saturate_cast<uchar>(newVal);
            } else
            {                           // For color image
                for (int k = 0; k < imgSrc.channels(); ++k)
                {
                    newVal = 255;
                    for (int ii = -(maskRows/2); ii <= (maskRows/2); ++ii)
                    {
                        for (int jj = -(maskCols/2); jj <= (maskCols/2); ++jj)
                        {
                            r1 = reflect(imgSrc.rows, i - ii);
                            c1 = reflect(imgSrc.cols, j - jj);
                            if (imgSrc.at<Vec3b>(r1, c1)[k] < newVal)
                            {
                                newVal = imgSrc.at<Vec3b>(r1, c1)[k];
                            }
                        }
                    }
                    imgDst.at<Vec3b>(i, j)[k] = saturate_cast<uchar>(newVal);
                }
            }
        }
    }
}

//
// Find the zero crossing points in the image
//
void MainWindow::zeroCross(const Mat &imgSrc, Mat &imgDst, const double &thres)
{
    double minVal;
    double maxVal;
    minMaxIdx(imgSrc, &minVal, &maxVal);
    double thresVal = maxVal * thres;

    int r1;
    int r2;
    int c1;
    int c2;
    // Traverse all the pixels in source image
    for (int i = 0; i < imgSrc.rows; ++i)
    {
        for (int j = 0; j < imgSrc.cols; ++j)
        {
            // Test four cases: left/right, up/down, two diagonals
            int count = 0;

            // Left right
            r1 = reflect(imgSrc.rows, i);
            c1 = reflect(imgSrc.cols, j - 1);
            c2 = reflect(imgSrc.cols, j + 1);

            if (imgSrc.at<short>(r1, c1) * imgSrc.at<short>(r1, c2) < 0)
            {
                if (abs(imgSrc.at<short>(r1, c1) - imgSrc.at<short>(r1, c2)) >= thresVal) ++count;
            }

            // Up down
            r1 = reflect(imgSrc.rows, i - 1);
            r2 = reflect(imgSrc.rows, i + 1);
            c1 = reflect(imgSrc.cols, j);

            if (imgSrc.at<short>(r1, c1) * imgSrc.at<short>(r2, c1) < 0)
            {
                if (abs(imgSrc.at<short>(r1, c1) - imgSrc.at<short>(r2, c1)) >= thresVal) ++count;
            }

            // Upper left and lower right
            r1 = reflect(imgSrc.rows, i - 1);
            c1 = reflect(imgSrc.cols, j - 1);
            r2 = reflect(imgSrc.rows, i + 1);
            c2 = reflect(imgSrc.cols, j + 1);
            if (imgSrc.at<short>(r1, c1) * imgSrc.at<short>(r2, c2) < 0)
            {
                if (abs(imgSrc.at<short>(r1, c1) - imgSrc.at<short>(r2, c2)) >= thresVal) ++count;
            }

            // Lower left and upper right
            r1 = reflect(imgSrc.rows, i + 1);
            c1 = reflect(imgSrc.cols, j - 1);
            r2 = reflect(imgSrc.rows, i - 1);
            c2 = reflect(imgSrc.cols, j + 1);
            if (imgSrc.at<short>(r1, c1) * imgSrc.at<short>(r2, c2) < 0)
            {
                if (abs(imgSrc.at<short>(r1, c1) - imgSrc.at<short>(r2, c2)) >= thresVal) ++count;
            }

            if (count >= 2)
            {
                imgDst.at<uchar>(i, j) = 255;
            } else
            {
                imgDst.at<uchar>(i, j) = 0;
            }
        }
    }
}

//
// Apply the horizontal and vertical sobel mask
//
void MainWindow::sobel(const Mat &imgSrc, Mat &imgDst, const double &thres)
{
    double HMask[] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};    // For detection of horizontal edges
    double VMask[] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};    // For detection of vertical edges

    int r1;             // The pixel index emcompassed by the mask
    int c1;
    // Traverse all the pixels in source image
    for (int i = 0; i < imgSrc.rows; ++i)
    {
        for (int j = 0; j < imgSrc.cols; ++j)
        {
            // Traverse all the coefficients in the mask
            int hVal = 0;   // For horizontal edges
            for (int ii = -1; ii <= 1; ++ii)
            {
                for (int jj = -1; jj <= 1; ++jj)
                {
                    r1 = reflect(imgSrc.rows, i - ii);
                    c1 = reflect(imgSrc.cols, j - jj);
                    hVal += HMask[(3*(ii + 1) + (jj + 1))] * imgSrc.at<uchar>(r1, c1);
                }
            }

            int vVal = 0;   // For vertical edges
            for (int ii = -1; ii <= 1; ++ii)
            {
                for (int jj = -1; jj <= 1; ++jj)
                {
                    r1 = reflect(imgSrc.rows, i - ii);
                    c1 = reflect(imgSrc.cols, j - jj);
                    vVal += VMask[(3*(ii + 1) + (jj + 1))] * imgSrc.at<uchar>(r1, c1);
                }
            }

            if (sqrt(hVal*hVal + vVal*vVal) >= thres)
            {
                imgDst.at<uchar>(i, j) = 255;
            } else
            {
                imgDst.at<uchar>(i, j) = 0;
            }
        }
    }
}
