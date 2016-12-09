#ifndef FCSETTINGDIALOG_H
#define FCSETTINGDIALOG_H

#include <QDialog>
#include <QStandardItemModel>
#include <QColorDialog>
#include <QDebug>

namespace Ui {
class FCSettingDialog;
}

class FCSettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FCSettingDialog(QWidget *parent = 0);
    ~FCSettingDialog();

    QStandardItemModel *model;

private slots:
    void on_addColorButton_clicked();

    void on_removeColorButton_clicked();

private:
    Ui::FCSettingDialog *ui;
};

#endif // FCSETTINGDIALOG_H
