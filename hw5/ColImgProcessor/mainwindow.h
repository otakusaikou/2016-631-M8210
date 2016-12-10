#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QString>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QDebug>
#include <vector>
#include <opencv2/opencv.hpp>
#include "fcsettingdialog.h"
#include "kmeansdialog.h"
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
    utils processor;        // Color image processor
    int msgBox;

private slots:
    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionReset_triggered();

    void on_actionExit_triggered();

    void on_RGBRadioButton_clicked();

    void on_CMYRadioButtion_clicked();

    void on_HSIRadioButtion_clicked();

    void on_XYZRadioButtion_clicked();

    void on_LabRadioButtion_clicked();

    void on_YUVRadioButtion_clicked();

    void on_actionFalse_Color_triggered();

    void on_actionK_means_triggered();

    void on_toLeftPushButtion_clicked();

private:
    Ui::MainWindow *ui;

    void init();

    void updateFigures(const cv::Mat &src, QLabel *label);
};

#endif // MAINWINDOW_H
