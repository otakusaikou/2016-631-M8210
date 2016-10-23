#ifndef MASKDIALOG_H
#define MASKDIALOG_H

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class MaskDialog;
}

class MaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MaskDialog(const int &rows, const int &cols, const int &mode, QWidget *parent = 0);
    ~MaskDialog();

    QStandardItemModel *model;  // Model for storing the mask value
    double m;                   // Multiplier of kernel

private slots:
    void on_buttonBox_accepted();

private:
    Ui::MaskDialog *ui;
};

#endif // MASKDIALOG_H
