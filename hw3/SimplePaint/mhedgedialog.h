#ifndef MHEDGEDIALOG_H
#define MHEDGEDIALOG_H

#include <QDialog>
#include <QStandardItemModel>

namespace Ui {
class MHEdgeDialog;
}

class MHEdgeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MHEdgeDialog(QWidget *parent = 0);
    ~MHEdgeDialog();

    int size;                       // Mask size
    double sigma;                   // Sigma for gaussian filter
    QStandardItemModel *model;      // Model for storing the mask value
    double thres;                   // Threshold for zero crossing
    bool timing = false;            // Timing flag

private slots:
    void on_buttonBox_accepted();

    void on_maskSizeSpinBox_valueChanged(int arg1);

    void on_maskTypeComboBox_currentIndexChanged(int index);

private:
    Ui::MHEdgeDialog *ui;
};

#endif // MHEDGEDIALOG_H
