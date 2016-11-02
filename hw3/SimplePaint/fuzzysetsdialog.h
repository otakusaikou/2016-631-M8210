#ifndef FUZZYSETSDIALOG_H
#define FUZZYSETSDIALOG_H

#include <QDialog>

namespace Ui {
class FuzzySetsDialog;
}

class FuzzySetsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FuzzySetsDialog(QWidget *parent = 0);
    ~FuzzySetsDialog();

    double sigma;                   // Sigma for normal distribution function
    int lowerBound;                 // Lowerbound for the white function
    int upperBound;                 // UpperBound for the black function

private slots:
    void on_buttonBox_accepted();

private:
    Ui::FuzzySetsDialog *ui;
};

#endif // FUZZYSETSDIALOG_H
