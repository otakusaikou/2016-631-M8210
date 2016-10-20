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

    // Disable some features first
    ui->actionSave->setDisabled(true);                      // Save
    ui->actionReset->setDisabled(true);                     // Reset
    ui->actionConvert_to_GRAY_Type_A->setDisabled(true);    // Grayscale conversion function
    ui->actionConvert_to_GRAY_Type_B->setDisabled(true);
    ui->actionThreshold->setDisabled(true);                 // Thresholding function
    ui->contrastSlider->setDisabled(true);                  // Contrast and Brightness slider
    ui->brightnessSlider->setDisabled(true);
    ui->resizeButton->setDisabled(true);                    // Resize
    ui->resizeSpinBox->setDisabled(true);
    ui->actionHistogram_Equalization->setDisabled(true);    // Histogram equalization
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

    if(!tempFileName.isEmpty() && !tempFileName.isNull()) {
        fileName = tempFileName;

        // Open image file
        imgSrc = imread(fileName.toStdString().data());
        if (!imgSrc.data) {
            errMsg = QMessageBox::warning(this,
                         tr("File I/O Error"),
                         tr("Can't open the image file.\n"));
            return;
        }
    } else {                    // The case with empty file name
        return;
    }

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

    // Enable histogram equalization
    ui->actionHistogram_Equalization->setDisabled(false);
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
    if (!ui->imgLabel->pixmap()) {
        errMsg = QMessageBox::warning(this,
                     tr("File I/O Error"),
                     tr("There are no images available.\n"));
        return;
    }

    // Don't do anything if current image is grayscale image
    if ((*curImg).channels() == 1) return;

    unsigned char newVal;

    // Create a new gray image buffer
    bufGray.release();
    bufGray = Mat::zeros(bufRGB.rows, bufRGB.cols, CV_8U);

    for (int i = 0; i < bufRGB.rows; ++i) {
        for (int j = 0; j < bufRGB.cols; ++j) {
            // Compute the new value
            newVal = (bufRGB.data[bufRGB.channels() * (bufRGB.cols * i + j)]
                      + bufRGB.data[bufRGB.channels() * (bufRGB.cols * i + j) + 1]
                      + bufRGB.data[bufRGB.channels() * (bufRGB.cols * i + j) + 2]) / 3.0;

            // Assign the new value to the image buffer on the display
            bufGray.data[bufRGB.cols * i + j] = newVal;
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
    if (!ui->imgLabel->pixmap()) {
        errMsg = QMessageBox::warning(this,
                     tr("File I/O Error"),
                     tr("There are no images available.\n"));
        return;
    }

    // Don't do anything if current image is grayscale image
    if ((*curImg).channels() == 1) return;

    unsigned char newVal;
    // Create a new gray image buffer
    bufGray.release();
    bufGray = Mat::zeros(bufRGB.rows, bufRGB.cols, CV_8U);

    for (int i = 0; i < bufRGB.rows; ++i) {
        for (int j = 0; j < bufRGB.cols; ++j) {
            // Compute the new value
            newVal = (0.299 * bufRGB.data[bufRGB.channels() * (bufRGB.cols * i + j)]
                      + 0.587 * bufRGB.data[bufRGB.channels() * (bufRGB.cols * i + j) + 1]
                      + 0.114 * bufRGB.data[bufRGB.channels() * (bufRGB.cols * i + j) + 2]);

            // Assign the new value to the image buffer on the display
            bufGray.data[bufRGB.cols * i + j] = newVal;
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
    if (!ui->imgLabel->pixmap()) {
        errMsg = QMessageBox::warning(this,
                     tr("File I/O Error"),
                     tr("There are no images available.\n"));
        return;
    }

    // Get the output image file name
    // Set default extension for saving image
    QString tempFileName = QFileDialog::getSaveFileName(this,
        tr("Save Image"), fileName.split(".", QString::SkipEmptyParts).at(0) + ".bmp",
        tr("Image Files (BMP (*.bmp);;JPEG (*.jpg *.jpeg)"));

    if(!tempFileName.isEmpty() && !tempFileName.isNull()) {
        // Check file extension
        if (tempFileName.split(".", QString::SkipEmptyParts).length() == 1) {
            tempFileName += ".bmp";
        }

        // Copy the result to the image save buffer
        Mat bufSave;
        (*curImg).copyTo(bufSave);

        // If current image is color image, convert it from rgb to bgr
        if (bufSave.channels() > 1) {
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
    if (!ui->imgLabel->pixmap()) {
        errMsg = QMessageBox::warning(this,
                     tr("File I/O Error"),
                     tr("There are no images available.\n"));
        return;
    }

    // Release memory
    bufRGB.release();
    bufGray.release();
    bufResize.release();

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

    // Reset the slider, alpha, beta values and spinbox value
    ui->contrastSlider->setValue(50);
    ui->brightnessSlider->setValue(50);
    ui->resizeSpinBox->setValue(1.0);
    alpha = 1.0;
    beta = 0.0;
}

//
// Apply thresholding function to get binary image
//
void MainWindow::on_actionThreshold_triggered()
{
    // Check whether the image label is empty
    if (!ui->imgLabel->pixmap()) {
        errMsg = QMessageBox::warning(this,
                     tr("File I/O Error"),
                     tr("There are no images available.\n"));
        return;
    }

    // Don't do anything if there is no any grayscale image exists
    if (bufGray.empty()) return;

    for (int i = 0; i < bufGray.rows; ++i) {
        for (int j = 0; j < bufGray.cols; ++j) {
            // Assign the new value after thresholding
            if  (bufGray.data[bufGray.cols * i + j] > 127) {
                bufGray.data[bufGray.cols * i + j] = 255;
            } else {
                bufGray.data[bufGray.cols * i + j] = 0;
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
    alpha = (2.0 / 100) * ui->contrastSlider->value();
}

//
// Change the contrast after user released the mouse button
//
void MainWindow::on_contrastSlider_sliderReleased()
{
    // The new_image = alpha * old_image
    for(int i = 0; i < (*curImg).rows; ++i) {
        for(int j = 0; j < (*curImg).cols; ++j) {
            for(int k = 0; k < (*curImg).channels(); ++k) {
                // Assign the new value to the image buffer on the display
                (*curImg).data[(*curImg).channels() * ((*curImg).cols * i + j) + k] =
                        saturate_cast<uchar>(
                            ((2.0 / 100) * ui->contrastSlider->value() / alpha) *
                            ((*curImg).data[(*curImg).channels() * ((*curImg).cols * i + j) + k]));
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
    beta = (255.0 / 100) * ui->brightnessSlider->value() - 127.5;
}

//
// Change the brightness after user released the mouse button
//
void MainWindow::on_brightnessSlider_sliderReleased()
{
    // The new_image = old_image + beta
    for(int i = 0; i < (*curImg).rows; ++i) {
        for(int j = 0; j < (*curImg).cols; ++j) {
            for(int k = 0; k < (*curImg).channels(); ++k) {
                // Assign the new value to the image buffer on the display
                (*curImg).data[(*curImg).channels() * ((*curImg).cols * i + j) + k] =
                        saturate_cast<uchar>(
                            (*curImg).data[(*curImg).channels() * ((*curImg).cols * i + j) + k] -
                            beta + (255.0 / 100) * ui->brightnessSlider->value() - 127.5);
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
    bufResize.release();
    if ((*curImg).channels() == 1) {        // For grayscale image
        bufResize = Mat::zeros(static_cast<int>((*curImg).rows*scale), static_cast<int>((*curImg).cols*scale), CV_8UC1);
    } else{                                 // For color image
        bufResize = Mat::zeros(static_cast<int>((*curImg).rows*scale), static_cast<int>((*curImg).cols*scale), CV_8UC3);
    }
    // Resample the image
    resize((*curImg), bufResize);

    // Update the image label and histogram chart
    (*curImg).release();
    bufResize.copyTo((*curImg));
    updateFigures();
}

//
// Perform histogram equalization
//
void MainWindow::on_actionHistogram_Equalization_triggered()
{
    vector<int> hist(256 * (*curImg).channels(), 0);        // Histogram
    vector<int> cumHist(256 * (*curImg).channels(), 0);     // Cumulative histogram
    vector<double> cumPdf(256 * (*curImg).channels(), 0);   // Cumulative distribution function
    vector<int> lut(256 * (*curImg).channels(), 0);         // Look up table for histogram equalization
    double scale = 255.0 / ((*curImg).rows * (*curImg).cols);

    // Calculate the histogram
    for (int i = 0; i < (*curImg).rows; ++i) {
        for (int j = 0; j < (*curImg).cols; ++j) {
            for (int k = 0; k < (*curImg).channels(); ++k) {
                ++hist.at(256 * k + (*curImg).data[(*curImg).channels() * ((*curImg).cols * i + j) + k]);
            }
        }
    }

    // Calculate the cumulative histogram
    for (int k = 0; k < (*curImg).channels(); ++k) {
        for (int i = 0; i < 256; ++i) {
            if (i == 0) {
                cumHist.at(256 * k + i) = hist.at(256 * k + i);
            } else {
                cumHist.at(256 * k + i) = hist.at(256 * k + i) + cumHist.at(256 * k + i - 1);
            }
        }
    }

    // Generate the look up table
    for (int k = 0; k < (*curImg).channels(); ++k) {
        for (int i = 0; i < 256; ++i) {
            lut.at(256 * k + i) = cvRound(cumHist.at(256 * k + i) * scale);
        }
    }


    // Equalize the image
    for (int i = 0; i < (*curImg).rows; ++i) {
        for (int j = 0; j < (*curImg).cols; ++j) {
            for (int k = 0; k < (*curImg).channels(); ++k) {
                (*curImg).data[(*curImg).channels() * ((*curImg).cols * i + j) + k] =
                    lut.at(256 * k + (*curImg).data[(*curImg).channels() * ((*curImg).cols * i + j) + k]);
            }
        }
    }

    // Update the image label and histogram chart
    updateFigures();
}

//
// Update the image label and histogram chart
//
void MainWindow::updateFigures() {
    QImage QImg;
    vector<int> hist(256 * (*curImg).channels(), 0);        // Histogram

    // Conver opencv image matrix to QImage object
    if ((*curImg).channels() == 1) {        // For grayscale image
        QImg = QImage((const unsigned char*) ((*curImg).data),
                            (*curImg).cols, (*curImg).rows, (*curImg).step1(), QImage::Format_Grayscale8);
    } else {                                // For color image
        QImg = QImage((const unsigned char*) ((*curImg).data),
                            (*curImg).cols, (*curImg).rows, (*curImg).step1(), QImage::Format_RGB888);
    }

    // Calculate histogram
    for (int i = 0; i < (*curImg).rows; ++i) {
        for (int j = 0; j < (*curImg).cols; ++j) {
            for (int k = 0; k < (*curImg).channels(); ++k) {
                ++hist.at(256 * k + (*curImg).data[(*curImg).channels() * ((*curImg).cols * i + j) + k]);
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
    for (int k = 0; k < (*curImg).channels(); ++k) {
        set = new QBarSet("Histogram");
        for(int i = 0; i < 256; ++i) {
            *set << hist.at(256 * k + i);
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
void MainWindow::resize(const Mat &imgSrc, Mat &imgDst) {
    int x0;
    int x1;
    int y0;
    int y1;
    unsigned char Ia;
    unsigned char Ib;
    unsigned char Ic;
    unsigned char Id;
    double wa;
    double wb;
    double wc;
    double wd;
    double x;
    double y;
    double scaleX = 1.0 * imgSrc.cols / imgDst.cols;
    double scaleY = 1.0 * imgSrc.rows / imgDst.rows ;

    for (int i = 0; i < imgDst.rows; ++i) {
        for (int j = 0; j < imgDst.cols; ++j) {
            for (int k = 0; k < imgDst.channels(); ++k) {
                x = j * scaleX;
                y = i * scaleY;
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
                Ia = imgSrc.data[imgSrc.channels() * (imgSrc.cols * y0 + x0) + k];      // Upper left corner
                Ib = imgSrc.data[imgSrc.channels() * (imgSrc.cols * y1 + x0) + k];      // Lower left corner
                Ic = imgSrc.data[imgSrc.channels() * (imgSrc.cols * y0 + x1) + k];      // Upper right corner
                Id = imgSrc.data[imgSrc.channels() * (imgSrc.cols * y1 + x1) + k];      // Lower right corner

                // Compute the weight of four points
                wa = (x1 - x) * (y1 - y);
                wb = (x1 - x) * (y - y0);
                wc = (x - x0) * (y1 - y);
                wd = (x - x0) * (y - y0);

                // Assign values to target image
                if (wa + wb + wc + wd == 0) {
                    imgDst.data[imgDst.channels() * (imgDst.cols * i + j) + k] = Ia;
                } else {
                    imgDst.data[imgDst.channels() * (imgDst.cols * i + j) + k] =
                            (wa*Ia + wb*Ib + wc*Ic + wd*Id);
                }
            }
        }
    }
}
