#ifndef KMEANSDIALOG_H
#define KMEANSDIALOG_H

#include <QDialog>

namespace Ui {
class KMeansDialog;
}

class KMeansDialog : public QDialog
{
    Q_OBJECT

public:
    explicit KMeansDialog(QWidget *parent = 0);
    ~KMeansDialog();

    int K = 8;
    int kSize = 15;
    bool hasBlur = false;

private slots:
    void on_buttonBox_accepted();

    void on_hasBlurCheckBox_clicked();

private:
    Ui::KMeansDialog *ui;
};

#endif // KMEANSDIALOG_H
