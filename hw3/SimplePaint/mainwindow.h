#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QDebug>
#include <iostream>
#include <QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>
#include <ctime>
#include "thresdialog.h"
#include "masksizedialog.h"
#include "maskdialog.h"
#include "gmaskdialog.h"
#include "umaskdialog.h"
#include "imshowdialog.h"
#include "mhedgedialog.h"
#include "smaskdialog.h"
#include "fuzzysetsdialog.h"

typedef unsigned char uchar;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    QString fileName;
    cv::Mat imgSrc;         // Source image
    cv::Mat bufImg;         // Image buffer
    double alpha;           // Contrast parameter
    double beta;            // Brightness parameter
    int msgBox;

private slots:
    void on_actionOpen_triggered();

    void on_actionExit_triggered();

    void on_actionConvert_to_GRAY_Type_A_triggered();

    void on_actionConvert_to_GRAY_Type_B_triggered();

    void on_actionSave_triggered();

    void on_actionReset_triggered();

    void on_actionThreshold_triggered();

    void on_contrastSlider_sliderPressed();

    void on_contrastSlider_sliderReleased();

    void on_brightnessSlider_sliderPressed();

    void on_brightnessSlider_sliderReleased();

    void on_resizeButton_clicked();

    void on_changeButton_clicked();

    void on_actionHistogram_Equalization_triggered();

    void on_actionAveraging_Filter_triggered();

    void on_actionGaussian_Filter_triggered();

    void on_actionMedian_Filter_triggered();

    void on_actionMaxminum_Filter_triggered();

    void on_actionLaplacian_filter_triggered();

    void on_actionMinimum_Filter_triggered();

    void on_actionUnsharp_Masking_triggered();

    void on_actionMarr_Hildreth_Edge_Detector_triggered();

    void on_actionSobel_Edge_Detector_triggered();

    void on_actionFuzzy_Sets_Spatial_Filtering_triggered();

    void on_actionRender_original_size_image_triggered();

private:
    Ui::MainWindow *ui;

    void updateFigures();

    void resize(const cv::Mat &imgSrc, cv::Mat &imgDst);

    int reflect(const int &M, const int &x);

    void convolve(const cv::Mat &imgSrc, cv::Mat &imgDst, const double *mask, const int &maskRows, const int &maskCols, const int &depth);

    void genGaussianFilter(const int &size, const double &sigma, double *gFilter);

    void median(const cv::Mat &imgSrc, cv::Mat &imgDst, const int &maskRows, const int &maskCols);

    void max(const cv::Mat &imgSrc, cv::Mat &imgDst, const int &maskRows, const int &maskCols);

    void min(const cv::Mat &imgSrc, cv::Mat &imgDst, const int &maskRows, const int &maskCols);

    void zeroCross(const cv::Mat &imgSrc, cv::Mat &imgDst, const double &thres);

    void sobel(const cv::Mat &imgSrc, cv::Mat &imgDst, const double &thres);

    void fuzzy(const cv::Mat &imgSrc, cv::Mat &imgDst, const double &sigma, const int &lowerBound, const int &upperBound);
};

#endif // MAINWINDOW_H
