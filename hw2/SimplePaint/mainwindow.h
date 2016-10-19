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
#include <opencv2/highgui.hpp>
#include <vector>
#include <cmath>

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
    cv::Mat bufRGB;         // Buffer for color image
    cv::Mat bufGray;        // Buffer for operation involved grayscale image
    cv::Mat bufSave;        // Buffer for saving image
    cv::Mat *curImg;        // Pointer to the current image on the display
    cv::Mat bufResize;
    double alpha;           // Contrast parameter
    double beta;            // Brightness parameter
    int errMsg;

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

private:
    Ui::MainWindow *ui;

    void updateFigures();

    void resize(const cv::Mat &imgSrc, cv::Mat &imgDst);
};

#endif // MAINWINDOW_H
