#include "mhedgedialog.h"
#include "ui_mhedgedialog.h"

MHEdgeDialog::MHEdgeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MHEdgeDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Marr-Hildreth edge detector settings");

    // Initialize the tableView object
    model = new QStandardItemModel(3, 3, this);
    QModelIndex index;

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
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
                model->setData(index, -4);
            } else {
                index = model->index(i, j, QModelIndex());
                model->setData(index, 1);
            }
        }
    }

    ui->tableView->setModel(model);
}

MHEdgeDialog::~MHEdgeDialog()
{
    delete ui;
}

//
// Set the sigma of gaussian filter automatically
// Reference: https://goo.gl/qw6nxZ
//
void MHEdgeDialog::on_maskSizeSpinBox_valueChanged(int arg1)
{
    ui->sigmaDoubleSpinBox->setValue(0.3*((arg1-1)*0.5 - 1) + 0.8);
}

//
// Change the type of Laplacian filter
//
void MHEdgeDialog::on_maskTypeComboBox_currentIndexChanged(int idx)
{
    QModelIndex index;
    switch (idx)
    {
        case 0:         //  First type of Laplacian filter mask
            for (int i = 0; i < 3; ++i)
            {
                for (int j = 0; j < 3; ++j)
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
                        model->setData(index, -4);
                    } else {
                        index = model->index(i, j, QModelIndex());
                        model->setData(index, 1);
                    }
                }
            }
            break;
        case 1:         //  Second type of Laplacian filter mask
            for (int i = 0; i < 3; ++i)
            {
                for (int j = 0; j < 3; ++j)
                {
                    if (i == 1 && j == 1)
                    {
                        index = model->index(i, j, QModelIndex());
                        model->setData(index, -8);
                    } else {
                        index = model->index(i, j, QModelIndex());
                        model->setData(index, 1);
                    }
                }
            }
            break;
    }

    ui->tableView->setModel(model);
}

//
// Apply the settings as dialog object attributes
//
void MHEdgeDialog::on_buttonBox_accepted()
{
    size = ui->maskSizeSpinBox->value();
    sigma = ui->sigmaDoubleSpinBox->value();
    thres = ui->thresholdDoubleSpinBox->value() / 100;
    timing = ui->timingCheckBox->checkState() == Qt::Checked;
}
