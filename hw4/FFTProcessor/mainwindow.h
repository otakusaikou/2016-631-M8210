#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QDebug>
#include <opencv2/opencv.hpp>
#include "utils.h"

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
    cv::Mat bufSrc;         // Image buffer for source image
    cv::Mat bufFFTSrc;      // Image buffer for source FFT image
    cv::Mat bufMask;        // Buffer for mask
    cv::Mat bufFFTLog;      // Buffer for FFT of logarithmic image
    cv::Mat bufDst;         // Image buffer for destination image
    cv::Mat bufFFTDst;      // Image buffer for dsetination FFT image
    utils processor;        // FFT processor
    double D0;              // The cuts off radius
    double fftMax;          // Max and min values for normalization of FFT image
    double fftMin;
    double gammaH;          // Upper and lower bound Homomorphic function
    double gammaL;
    int n;
    int c;
    int msgBox;
    int mode;               // Filtering mode (-1: NULL (0: Ideal (1: Butterworth
                            // (2: Gaussian (3: Homomorphic

private slots:
    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionReset_triggered();

    void on_actionExit_triggered();

    void on_srcFreqRadioButton_clicked();

    void on_srcSpaRadioButton_clicked();

    void on_dstSpaRadioButton_clicked();

    void on_dstFreqRadioButton_clicked();

    void on_actionIdeal_filter_triggered();

    void on_actionButterworth_Filter_triggered();

    void on_actionGaussian_Filter_triggered();

    void on_actionHomomorphic_Filter_triggered();

    void on_D0ValSlider_valueChanged(int value);

    void on_gammaHSlider_valueChanged(int value);

    void on_gammaLSlider_valueChanged(int value);

    void on_BMaskOrderSpinBox_valueChanged(int arg1);

    void on_CSpinBox_valueChanged(int arg1);

    void on_LPFRadioButton_clicked();

    void on_HPFRadiobutton_clicked();

private:
    Ui::MainWindow *ui;

    void init();

    void updateFigures(const cv::Mat &src, QLabel *label);

    void toSpm(const cv::Mat &src, cv::Mat &dst);

    void applyMask();
};

#endif // MAINWINDOW_H
