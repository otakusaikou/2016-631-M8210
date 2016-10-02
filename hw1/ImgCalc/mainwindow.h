#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <iostream>
#include <fstream>
#include <QtCharts>
#include <QtCharts/QChartView>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLegend>
#include <QtCharts/QBarCategoryAxis>
#include <QDebug>

using namespace QtCharts;
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    unsigned char srcImg[64][64];       // The 64*64 image source
    unsigned char tmpImg[64][64];       // The template image for the display
    unsigned char leftImg[64][64];      // The left image source
    unsigned char rightImg[64][64];     // The right image source
    unsigned char mergeImg[64][64];     // The new image after the image merge process
    int errMsg;

private slots:
    void on_actionOpen_triggered();

    void on_actionExit_triggered();

    void on_add_clicked();

    void on_substract_clicked();

    void on_multiply_clicked();

    void on_reset_clicked();

    void on_tabWidget_currentChanged(int index);

    void on_actionLImg_triggered();

    void on_actionRImg_triggered();

    void on_average_clicked();

    void on_getG_clicked();

    void on_actionSave_triggered();


private:
    Ui::MainWindow *ui;

    void updateFigures();

    void updatePixmap(unsigned char img[][64], QLabel *targetLabel);
};

#endif // MAINWINDOW_H
