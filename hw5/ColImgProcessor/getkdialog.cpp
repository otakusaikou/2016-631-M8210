#include "getkdialog.h"
#include "ui_getkdialog.h"

getKDialog::getKDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::getKDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("False Color Settings");      // Set dialog title
}

getKDialog::~getKDialog()
{
    delete ui;
}

//
// Update the dialog attributes
//
void getKDialog::on_buttonBox_accepted()
{
    K = ui->kSpinBox->value();
}
