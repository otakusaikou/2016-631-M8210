#include "imshowdialog.h"
#include "ui_imshowdialog.h"

ImshowDialog::ImshowDialog(const cv::Mat *curImg, const QString &fileName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImshowDialog)
{
    ui->setupUi(this);

    this->setWindowTitle("Original size image - " + fileName);

    // Display the image on the image label
    QImage QImg;

    // Conver opencv image matrix to QImage object
    if (curImg->channels() == 1)
    {                       // For grayscale image
        QImg = QImage((const uchar*) (curImg->data),
                            curImg->cols, curImg->rows, curImg->step1(), QImage::Format_Grayscale8);
    } else {                // For color image
        QImg = QImage((const uchar*) (curImg->data),
                            curImg->cols, curImg->rows, curImg->step1(), QImage::Format_RGB888);
    }

    // Update the pixelmap
    ui->imgLabel->setPixmap(QPixmap::fromImage(QImg));
}

ImshowDialog::~ImshowDialog()
{
    delete ui;
}
