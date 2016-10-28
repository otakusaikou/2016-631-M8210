#ifndef GMASKDIALOG_H
#define GMASKDIALOG_H

#include <QDialog>

namespace Ui {
class GMaskDialog;
}

class GMaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GMaskDialog(QWidget *parent = 0);
    ~GMaskDialog();

    int size;               // Mask size
    double sigma;           // Sigma for gaussian filter
    bool timing = false;    // Timing flag

private slots:
    void on_maskSizeSpinBox_valueChanged(int arg1);

    void on_buttonBox_accepted();

private:
    Ui::GMaskDialog *ui;
};

#endif // GMASKDIALOG_H
