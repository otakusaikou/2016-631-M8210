#include "maskdialog.h"
#include "ui_maskdialog.h"

MaskDialog::MaskDialog(const int &rows, const int &cols, const int &mode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MaskDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Mask value settings");

    // Initialize the tableView object
    model = new QStandardItemModel(rows, cols, this);
    QModelIndex index;
    switch (mode)
    {
        case 0:         // Averaging filter mask
            for (int i = 0; i < rows; ++i)
            {
                for (int j = 0; j < cols; ++j)
                {
                    index = model->index(i, j, QModelIndex());
                    model->setData(index, 1);
                }

            }
            // Set the multiplier parameters
            ui->numSpinBox->setValue(1);
            ui->denSpinBox->setValue(rows*cols);
            break;
        case 1:         //  Laplace filter mask
            for (int i = 0; i < rows; ++i)
            {
                for (int j = 0; j < cols; ++j)
                {
                    if ((i == 0 && j == 0) ||
                        (i == 0 && j == 2) ||
                        (i == 2 && j == 0) ||
                        (i == 2 && j == 2))
                    {
                        index = model->index(i, j, QModelIndex());
                        model->setData(index, 0);
                    } else if (i == 1 && j == 1) {
                        index = model->index(i, j, QModelIndex());
                        model->setData(index, 5);
                    } else {
                        index = model->index(i, j, QModelIndex());
                        model->setData(index, -1);
                    }
                }
            }
            // Set the multiplier parameters
            ui->numSpinBox->setValue(1);
            ui->denSpinBox->setValue(1);
            break;
    }

    ui->tableView->setModel(model);
}

MaskDialog::~MaskDialog()
{
    delete ui;
}

//
// Update mask multiplier
//
void MaskDialog::on_buttonBox_accepted()
{
    m = 1.0 * ui->numSpinBox->value() / ui->denSpinBox->value();
}
