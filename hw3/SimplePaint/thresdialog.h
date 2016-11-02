#ifndef THRESDIALOG_H
#define THRESDIALOG_H

#include <QDialog>

namespace Ui {
class ThresDialog;
}

class ThresDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ThresDialog(QWidget *parent = 0);
    ~ThresDialog();

    int thres;          // Threshold value
    int maxVal;         // The new value for pixel greater than threshold

private slots:
    void on_buttonBox_accepted();

private:
    Ui::ThresDialog *ui;

};

#endif // THRESDIALOG_H
