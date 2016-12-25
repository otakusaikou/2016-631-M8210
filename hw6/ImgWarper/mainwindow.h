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

    QString filename;
    cv::Mat imgSrc;         // Source image
    cv::Mat bufSrc;         // Image buffer for source image
    cv::Mat bufDst;         // Image buffer for output image
    utils processor;        // Image transformation processor
    int msgBox;

    // Parameters for trapezoidal transformation
    utils::pos edgePos;
    float length;
    float shift;

    float ampl;             // Parameters for wavy transformation
    float freq;

    float zoom;             // Parameters for circular transformation
    float strength;

private slots:
    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionReset_triggered();

    void on_actionExit_triggered();

    void on_ToRadioButton_clicked();

    void on_BoRadioButton_clicked();

    void on_LeRadioButton_clicked();

    void on_RiRadioButton_clicked();

    void on_lengthSpinBox_valueChanged(int arg1);

    void on_lengthHorizontalSlider_sliderMoved(int position);

    void on_shiftSpinBox_valueChanged(int arg1);

    void on_shiftHorizontalSlider_sliderMoved(int position);

    void on_amplSpinBox_valueChanged(int arg1);

    void on_amplHorizontalSlider_sliderMoved(int position);

    void on_freqSpinBox_valueChanged(int arg1);

    void on_freqHorizontalSlider_sliderMoved(int position);

    void on_zoomSpinBox_valueChanged(int arg1);

    void on_zoomHorizontalSlider_sliderMoved(int position);

    void on_streSpinBox_valueChanged(int arg1);

    void on_streHorizontalSlider_sliderMoved(int position);

    void init();

    void updateFigures(const cv::Mat &src, QLabel *label);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
