#include "masksizedialog.h"
#include "ui_masksizedialog.h"
#include <QDebug>

MaskSizeDialog::MaskSizeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MaskSizeDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Mask size settings");
}

MaskSizeDialog::~MaskSizeDialog()
{
    delete ui;
}

//
// Apply the settings as dialog object attributes
//
void MaskSizeDialog::on_buttonBox_accepted()
{
    rows = ui->rowSpinBox->value();
    cols = ui->colSpinBox->value();
    timing = ui->timingCheckBox->checkState() == Qt::Checked;
}
