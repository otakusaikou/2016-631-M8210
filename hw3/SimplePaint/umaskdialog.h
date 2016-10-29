#ifndef UMASKDIALOG_H
#define UMASKDIALOG_H

#include <QDialog>

namespace Ui {
class UMaskDialog;
}

class UMaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UMaskDialog(QWidget *parent = 0);
    ~UMaskDialog();

    int size;               // Mask size
    double sigma;           // Sigma for gaussian filter
    double k;               // Weight for unsharp masking
    bool timing = false;    // Timing flag

private slots:
    void on_buttonBox_accepted();

    void on_maskSizeSpinBox_valueChanged(int arg1);

private:
    Ui::UMaskDialog *ui;
};

#endif // UMASKDIALOG_H
