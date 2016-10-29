#include "umaskdialog.h"
#include "ui_umaskdialog.h"

UMaskDialog::UMaskDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UMaskDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Unsharp mask settings");
}

UMaskDialog::~UMaskDialog()
{
    delete ui;
}

//
// Set the sigma of gaussian filter automatically
// Reference: https://goo.gl/qw6nxZ
void UMaskDialog::on_maskSizeSpinBox_valueChanged(int arg1)
{
    ui->sigmaDoubleSpinBox->setValue(0.3*((arg1-1)*0.5 - 1) + 0.8);
}

//
// Apply the settings as dialog object attributes
//
void UMaskDialog::on_buttonBox_accepted()
{
    size = ui->maskSizeSpinBox->value();
    sigma = ui->sigmaDoubleSpinBox->value();
    k = ui->unsharpWeightDoubleSpinBox->value();
    timing = ui->timingCheckBox->checkState() == Qt::Checked;
}
