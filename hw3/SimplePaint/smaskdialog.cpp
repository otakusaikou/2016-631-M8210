#include "smaskdialog.h"
#include "ui_smaskdialog.h"

SMaskDialog::SMaskDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SMaskDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Sobel edge dector settings");
}

SMaskDialog::~SMaskDialog()
{
    delete ui;
}

//
// Apply the settings as dialog object attributes
//
void SMaskDialog::on_buttonBox_accepted()
{
    thres = ui->thresholdDoubleSpinBox->value();
}
