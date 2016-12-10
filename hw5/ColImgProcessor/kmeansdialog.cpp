#include "kmeansdialog.h"
#include "ui_kmeansdialog.h"

KMeansDialog::KMeansDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KMeansDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Set group number K");      // Set dialog title
}

KMeansDialog::~KMeansDialog()
{
    delete ui;
}

//
// Update the dialog attributes
//
void KMeansDialog::on_buttonBox_accepted()
{
    K = ui->KSpinBox->value();
    kSize = ui->sizeSpinBox->value();
    hasBlur = ui->hasBlurCheckBox->isChecked();
}

//
// Enable image bluring option
//
void KMeansDialog::on_hasBlurCheckBox_clicked()
{
    ui->kSizeLabel->setEnabled(ui->hasBlurCheckBox->isChecked());
    ui->sizeSpinBox->setEnabled(ui->hasBlurCheckBox->isChecked());
}
