#include "imshowdialog.h"
#include "ui_imshowdialog.h"

ImshowDialog::ImshowDialog(const cv::Mat &imgSrc, const QString &fileName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImshowDialog)
{
    ui->setupUi(this);

    this->setWindowTitle("Original size image - " + fileName);

    // Display the image on the image label
    QImage QImg;

    // Conver opencv image matrix to QImage object
    if (imgSrc.channels() == 1)
    {                       // For grayscale image
        QImg = QImage((const uchar*) (imgSrc.data),
                            imgSrc.cols, imgSrc.rows, imgSrc.step1(), QImage::Format_Grayscale8);
    } else {                // For color image
        QImg = QImage((const uchar*) (imgSrc.data),
                            imgSrc.cols, imgSrc.rows, imgSrc.step1(), QImage::Format_RGB888);
    }

    // Update the pixelmap
    ui->imgLabel->setPixmap(QPixmap::fromImage(QImg));
}

ImshowDialog::~ImshowDialog()
{
    delete ui;
}
