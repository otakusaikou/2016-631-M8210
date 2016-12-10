#include "fcsettingdialog.h"
#include "ui_fcsettingdialog.h"

FCSettingDialog::FCSettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FCSettingDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Set color combination");      // Set dialog title

    // Initialize the tableView object
    int rows = 3;
    model = new QStandardItemModel(rows, 1, this);

    model->setData(
                model->index(0, 0, QModelIndex()),
                QBrush(Qt::blue),
                Qt::BackgroundColorRole);

    model->setData(
                model->index(1, 0, QModelIndex()),
                QBrush(Qt::green),
                Qt::BackgroundColorRole);

    model->setData(
                model->index(2, 0, QModelIndex()),
                QBrush(Qt::red),
                Qt::BackgroundColorRole);

    ui->tableView->setModel(model);
}

FCSettingDialog::~FCSettingDialog()
{
    delete ui;
}

//
// Insert new color to table view
//
void FCSettingDialog::on_addColorButton_clicked()
{
    // Open color dialog
    QColor color = QColorDialog::getColor(Qt::yellow, this);
    if (!color.isValid()) return;

    // Create new item
    QStandardItem *item = new QStandardItem();
    item->setBackground(QBrush(color));

    // Insert new item to the selected index
    QModelIndex index = ui->tableView->selectionModel()->currentIndex();
    if (index.row() == -1)
        model->insertRow(model->rowCount(), item);
    else
        model->insertRow(index.row() + 1, item);
}

//
// Remove color from table view
//
void FCSettingDialog::on_removeColorButton_clicked()
{
    QModelIndex index = ui->tableView->selectionModel()->currentIndex();
    if (model->rowCount() > 2)
    {
        if (index.row() == -1)
            model->removeRow(model->rowCount() - 1);
        else
            model->removeRow(index.row());
    }
}
