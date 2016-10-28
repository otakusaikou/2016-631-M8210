#include "gmaskdialog.h"
#include "ui_gmaskdialog.h"

GMaskDialog::GMaskDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GMaskDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Gaussian smoothing filter settings");
}

GMaskDialog::~GMaskDialog()
{
    delete ui;
}

//
// Set the sigma of gaussian filter automatically
// Reference: https://goo.gl/qw6nxZ
//
void GMaskDialog::on_maskSizeSpinBox_valueChanged(int arg1)
{
    ui->sigmaDoubleSpinBox->setValue(0.3*((arg1-1)*0.5 - 1) + 0.8);
}

//
// Apply the settings as dialog object attributes
//
void GMaskDialog::on_buttonBox_accepted()
{
    size = ui->maskSizeSpinBox->value();
    sigma = ui->sigmaDoubleSpinBox->value();
    timing = ui->timingCheckBox->checkState() == Qt::Checked;
}
