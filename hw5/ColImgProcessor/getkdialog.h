#ifndef GETKDIALOG_H
#define GETKDIALOG_H

#include <QDialog>

namespace Ui {
class getKDialog;
}

class getKDialog : public QDialog
{
    Q_OBJECT

public:
    explicit getKDialog(QWidget *parent = 0);
    ~getKDialog();

    int K;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::getKDialog *ui;
};

#endif // GETKDIALOG_H
