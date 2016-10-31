#ifndef SMASKDIALOG_H
#define SMASKDIALOG_H

#include <QDialog>

namespace Ui {
class SMaskDialog;
}

class SMaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SMaskDialog(QWidget *parent = 0);
    ~SMaskDialog();

    double thres;                   // Threshold for Sobel edge detector

private slots:
    void on_buttonBox_accepted();

private:
    Ui::SMaskDialog *ui;
};

#endif // SMASKDIALOG_H
