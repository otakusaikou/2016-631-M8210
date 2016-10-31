#ifndef IMSHOWDIALOG_H
#define IMSHOWDIALOG_H

#include <QDialog>
#include <opencv2/opencv.hpp>

namespace Ui {
class ImshowDialog;
}

class ImshowDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ImshowDialog(const cv::Mat &imgSrc, const QString &fileName, QWidget *parent = 0);
    ~ImshowDialog();

private:
    Ui::ImshowDialog *ui;
};

#endif // IMSHOWDIALOG_H
