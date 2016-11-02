#include "fuzzysetsdialog.h"
#include "ui_fuzzysetsdialog.h"

FuzzySetsDialog::FuzzySetsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FuzzySetsDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Fuzzy sets spatial filtering settings");
}

FuzzySetsDialog::~FuzzySetsDialog()
{
    delete ui;
}

//
// Apply the settings as dialog object attributes
//
void FuzzySetsDialog::on_buttonBox_accepted()
{
    sigma = ui->sigmaDoubleSpinBox->value();
    lowerBound = ui->lowerBoundSpinBox->value();
    upperBound = ui->upperBoundSpinBox->value();
}
