#include "thresdialog.h"
#include "ui_thresdialog.h"

ThresDialog::ThresDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ThresDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Thresholding settings");
}

ThresDialog::~ThresDialog()
{
    delete ui;
}

//
// Apply the settings as dialog object attributes
//
void ThresDialog::on_buttonBox_accepted()
{
    thres = ui->thresSpinBox->value();
    maxVal = ui->maxValSpinBox->value();
}
