#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//
// Read the *.64 image file and show it on the main window
//
void MainWindow::on_actionOpen_triggered()
{
    char value;
    int i;
    int j;
    ifstream fin;

    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), ".",
        tr("Image Files (*.64)"));

    if(!fileName.isEmpty() && !fileName.isNull()) {
        // Open text file
        fin.close();                        // Close in case it was already open
        fin.clear();                        // Clear any existing errors
        fin.open(fileName.toStdString());   // Open the file with given file name
        if (!fin) {
            errMsg = QMessageBox::warning(this,
                         tr("File I/O Error"),
                         tr("Can't open the *.64 file.\n"));
            return;
        }
    } else {                                // The case with empty file name
        return;
    }

    // Write values to image, also update the histogram array
    for (i = 0; i < 64; i++) {
        for (j = 0; j < 64; j++) {
            fin >> value;
            if (value >= 'A') {
                value -= ('A' - '9' - 1);
            }
            srcImg[i][j] = value - '0';
            tmpImg[i][j] = srcImg[i][j];
        }
    }
    fin.close();

    // Update the image label and histogram chart
    updateFigures();
}

//
// Close the program
//
void MainWindow::on_actionExit_triggered()
{
    close();
}

//
// Add a constant value to each pixel in the image
//
void MainWindow::on_add_clicked()
{
    // Check whether the image label is empty
    if (!ui->ImgLabel->pixmap()) {
        errMsg = QMessageBox::warning(this,
                     tr("File I/O Error"),
                     tr("The program didn't load any image file.\n"));
        return;
    }

    int value = ui->shiftValue->value();
    int i;
    int j;

    // Update the image value
    for (i = 0; i < 64; i++) {
        for (j = 0; j < 64; j++) {
            // Make sure new value is not more than 255
            if (tmpImg[i][j] + value > 31) {
                tmpImg[i][j] = 31;
            } else {
                tmpImg[i][j] += value;
            }
        }
    }

    // Update the image label and histogram chart
    updateFigures();
}

//
// Substract a constant value to each pixel in the image
//
void MainWindow::on_substract_clicked()
{
    // Check whether the image label is empty
    if (!ui->ImgLabel->pixmap()) {
        errMsg = QMessageBox::warning(this,
                     tr("File I/O Error"),
                     tr("The program didn't load any image file.\n"));
        return;
    }

    int value = ui->shiftValue->value();
    int i;
    int j;

    // Update the image value
    for (i = 0; i < 64; i++) {
        for (j = 0; j < 64; j++) {
            // Make sure new value is not less than 0
            if (tmpImg[i][j] - value < 0) {
                tmpImg[i][j] = 0;
            } else {
                tmpImg[i][j] -= value;
            }
        }
    }

    // Update the image label and histogram chart
    updateFigures();
}

//
// Multiply a constant to each pixel in the image
//
void MainWindow::on_multiply_clicked()
{
    // Check whether the image label is empty
    if (!ui->ImgLabel->pixmap()) {
        errMsg = QMessageBox::warning(this,
                     tr("File I/O Error"),
                     tr("The program didn't load any image file.\n"));
        return;
    }

    double value = ui->ratioValue->value();
    int i;
    int j;

    // Update the image value
    for (i = 0; i < 64; i++) {
        for (j = 0; j < 64; j++) {
            // Make sure new value is not more than 255
            if (tmpImg[i][j] * value > 31) {
                tmpImg[i][j] = 31;
            } else {
                tmpImg[i][j] *= value;
            }
        }
    }

    // Update the image label and histogram chart
    updateFigures();
}

//
// Reset the image label and histogram
//
void MainWindow::on_reset_clicked()
{
    // Check whether the image label is empty
    if (!ui->ImgLabel->pixmap()) {
        errMsg = QMessageBox::warning(this,
                     tr("File I/O Error"),
                     tr("The program didn't load any image file.\n"));
        return;
    }

    int i;
    int j;

    // Reset the image value
    for (i = 0; i < 64; i++) {
        for (j = 0; j < 64; j++) {
            tmpImg[i][j] = srcImg[i][j];
        }
    }

    // Update the image label and histogram chart
    updateFigures();
}

//
// Turn on/off the 'Open (Left/Right) Image' action by checking the tab index
//
void MainWindow::on_tabWidget_currentChanged(int index)
{
    // Set the menu status
    if (index == 0) {
        ui->actionOpen->setEnabled(true);
        ui->actionLImg->setEnabled(false);
        ui->actionRImg->setEnabled(false);
    } else {
        ui->actionOpen->setEnabled(false);
        ui->actionLImg->setEnabled(true);
        ui->actionRImg->setEnabled(true);
    }
}

//
// Load and display the image on the left
//
void MainWindow::on_actionLImg_triggered()
{
    char value;
    int i;
    int j;
    ifstream fin;

    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Left Image"), ".",
        tr("Image Files (*.64)"));

    if(!fileName.isEmpty() && !fileName.isNull()) {
        // Open text file
        fin.close();                        // Close in case it was already open
        fin.clear();                        // Clear any existing errors
        fin.open(fileName.toStdString());   // Open the file with given file name
        if (!fin) {
            errMsg = QMessageBox::warning(this,
                         tr("File I/O Error"),
                         tr("Can't open the *.64 file.\n"));
            return;
        }
    } else {                                // The case with empty file name
        return;
    }

    // Write values to image array
    for (i = 0; i < 64; i++) {
        for (j = 0; j < 64; j++) {
            fin >> value;
            if (value >= 'A') {
                value -= ('A' - '9' - 1);
            }
            leftImg[i][j] = value - '0';
        }
    }
    fin.close();

    updatePixmap(leftImg, ui->LeftImgLabel);
}

//
// Load and display the image on the right
//
void MainWindow::on_actionRImg_triggered()
{
    char value;
    int i;
    int j;
    ifstream fin;

    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Right Image"), ".",
        tr("Image Files (*.64)"));

    if(!fileName.isEmpty() && !fileName.isNull()) {
        // Open text file
        fin.close();                        // Close in case it was already open
        fin.clear();                        // Clear any existing errors
        fin.open(fileName.toStdString());   // Open the file with given file name
        if (!fin) {
            errMsg = QMessageBox::warning(this,
                         tr("File I/O Error"),
                         tr("Can't open the *.64 file.\n"));
            return;
        }
    } else {                                // The case with empty file name
        return;
    }

    // Write values to image array
    for (i = 0; i < 64; i++) {
        for (j = 0; j < 64; j++) {
            fin >> value;
            if (value >= 'A') {
                value -= ('A' - '9' - 1);
            }
            rightImg[i][j] = value - '0';
        }
    }
    fin.close();

    updatePixmap(rightImg, ui->RightImgLabel);
}

//
// Compute the average values for the given two images
//
void MainWindow::on_average_clicked()
{
    // Check whether the image label is empty
    if (!ui->LeftImgLabel->pixmap() || !ui->RightImgLabel->pixmap()) {
        errMsg = QMessageBox::warning(this,
                     tr("File I/O Error"),
                     tr("Insufficient image file.\n"));
        return;
    }

    int i;
    int j;

    // Write values to image array
    for (i = 0; i < 64; i++) {
        for (j = 0; j < 64; j++) {
            mergeImg[i][j] = 0.5 * (leftImg[i][j] + rightImg[i][j]);
        }
    }

    updatePixmap(mergeImg, ui->ResultImgLabel);
}

//
// Create new image g(x,y) by using the given equation g(x,y) = f(x,y) - f(x-1,y)
//
void MainWindow::on_getG_clicked()
{
    // Check whether the image label is empty
    if (!ui->LeftImgLabel->pixmap()) {
        errMsg = QMessageBox::warning(this,
                     tr("File I/O Error"),
                     tr("Insufficient image file.\n"));
        return;
    }

    int i;
    int j;

    // Update the right image label
    for (i = 0; i < 64; i++) {
        for (j = 0; j < 64; j++) {
            if (j == 0) {
                rightImg[i][j] = leftImg[i][j];         // For the first column, repeat value of the next pixel
            } else {
                rightImg[i][j] = leftImg[i][j - 1];
            }
        }
    }

    updatePixmap(rightImg, ui->RightImgLabel);

    // Add the left and right image label
    for (i = 0; i < 64; i++) {
        for (j = 0; j < 64; j++) {
            // Make sure new value is not more than 255
            if (leftImg[i][j] + rightImg[i][j] > 31) {
                mergeImg[i][j] = 31;
            } else {
                mergeImg[i][j] = leftImg[i][j] + rightImg[i][j];
            }
        }
    }

    updatePixmap(mergeImg, ui->ResultImgLabel);
}

//
// Save the result (*.64)
//
void MainWindow::on_actionSave_triggered()
{
    char value;
    int i;
    int j;
    ofstream fout;

    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save Image"), ".",
        tr("Image Files (*.64)"));

    if (!fileName.endsWith(".64")) {
        fileName += ".64";
    }

    // Open text file
    fout.close();                        // Close in case it was already open
    fout.clear();                        // Clear any existing errors
    fout.open(fileName.toStdString());   // Open the file with given file name
    if (!fout) {
        errMsg = QMessageBox::warning(this,
                     tr("File I/O Error"),
                     tr("Can't open the *.64 file.\n"));
        return;
    }

    if (ui->tabWidget->currentIndex() == 0) {       // Save the result for Calc tab
        for (i = 0; i < 64; i++) {
            for (j = 0; j < 64; j++) {
                value = tmpImg[i][j] + '0';
                if (value > '9') {
                    value += ('A' - '9' - 1);
                }
                fout << value;
            }
            fout << endl;
        }
    } else {                                        // Save the result of Merge tab
        for (i = 0; i < 64; i++) {
            for (j = 0; j < 64; j++) {
                value = mergeImg[i][j] + '0';
                if (value > '9') {
                    value += ('A' - '9' - 1);
                }
                fout << value;
            }
            fout << endl;
        }
    }
    fout.close();
}

//
// Update the image label and histogram chart for the Calc tab
//
void MainWindow::updateFigures() {
    // For image
    int hist[32] = {0};                 // Histogram of the 64x64 image (32 gray scales)
    int i;
    int j;

    // Create QImage object for the image
    QImage QImg(64, 64, QImage::Format_RGB32);
    for (i = 0; i < 64; i++) {
        for (j = 0; j < 64; j++) {
            // Set the pixel value of the QImage
            // Transform the image value to 256 gray scale
            QImg.setPixel(j, i, qRgb(tmpImg[i][j] * 8, tmpImg[i][j] * 8, tmpImg[i][j] * 8));
            hist[tmpImg[i][j]]++;       // Update the histogram array
        }
    }

    // Resize the QImage to fit the label display
    QImage imgResize = QImg.scaled(ui->ImgLabel->width(),ui->ImgLabel->height(),Qt::KeepAspectRatio);

    // Update the pixelmap
    ui->ImgLabel->setPixmap(QPixmap::fromImage(imgResize));

    // For histogram
    QBarSet *set0 = new QBarSet("Histogram");

    for(i = 0; i < 32; i++) {
        *set0 << hist[i];
    }

    QBarSeries *series = new QBarSeries();
    series->append(set0);

    // Create QChart object for histogram
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Histogram");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(false);
    chart->legend()->setAlignment(Qt::AlignBottom);

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
// Update the target image label with given image array for the Merge tab
//
void MainWindow::updatePixmap(unsigned char img[][64], QLabel *targetLabel) {
    int i;
    int j;

    // Create QImage object for the image array
    QImage QImg(64, 64, QImage::Format_RGB32);
    for (i = 0; i < 64; i++) {
        for (j = 0; j < 64; j++) {
            // Set the pixel value of the QImage
            // Transform the image value to 256 gray scale
            QImg.setPixel(j, i, qRgb(img[i][j] * 8, img[i][j] * 8, img[i][j] * 8));
        }
    }

    // Resize the QImage to fit the label display
    QImage imgResize = QImg.scaled(targetLabel->width(), targetLabel->height(),Qt::KeepAspectRatio);

    // Update the pixelmap
    targetLabel->setPixmap(QPixmap::fromImage(imgResize));
}
