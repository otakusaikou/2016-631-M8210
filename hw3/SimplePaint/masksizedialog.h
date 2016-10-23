#ifndef MASKSIZEDIALOG_H
#define MASKSIZEDIALOG_H

#include <QDialog>

namespace Ui {
class MaskSizeDialog;
}

class MaskSizeDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MaskSizeDialog(QWidget *parent = 0);
    ~MaskSizeDialog();

    // Mask size
    int rows = 3;
    int cols = 3;

    // Timing flag
    bool timing = false;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::MaskSizeDialog *ui;
};

#endif // MASKSIZEDIALOG_H
