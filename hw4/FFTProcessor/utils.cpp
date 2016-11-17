#include "utils.h"
#include <QDebug>

using namespace std;
using namespace cv;

utils::utils()
{    
}

//
// Multiply the spectrums
//
void utils::multiplyMat(const Mat &A, const Mat &B, Mat &dst)
{
    for (int i = 0; i < A.rows; ++i)
    {
        for (int j = 0; j < A.cols; ++j)
        {
            dst.at<Vec2d>(i, j) = Vec2d(
                A.at<Vec2d>(i, j)[0]*B.at<double>(i, j),
                A.at<Vec2d>(i, j)[1]*B.at<double>(i, j));
        }
    }
}

//
// Compute the magnitude
//
void utils::getMagnitude(const Mat &src, Mat &dst)
{
    for (int i = 0; i < dst.rows; ++i)
    {
        for (int j = 0; j < dst.cols; ++j)
        {
            double real = src.at<Vec2d>(i, j)[0];
            double imagin = src.at<Vec2d>(i, j)[1];
            dst.at<double>(i, j) = sqrt((real*real) + (imagin*imagin));
        }
    }
}

//
// Rearrange the quadrants of Fourier image
// so that the origin is at the image center
//
void utils::shiftMat(Mat &dst)
{
    int cx = dst.cols/2;
    int cy = dst.rows/2;

    /*
    | q0 | q1 |      | q0 | q2 |
    |    |    |  ->  |    |    |
    | q2 | q3 |      | q1 | q3 |
    */

    Mat tmp;
    Mat q0(dst, Rect(0, 0, cx, cy));
    Mat q1(dst, Rect(cx, 0, cx, cy));
    Mat q2(dst, Rect(0, cy, cx, cy));
    Mat q3(dst, Rect(cx, cy, cx, cy));

    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);

    q1.copyTo(tmp);
    q2.copyTo(q1);
    tmp.copyTo(q2);
}

//
// Make the image value between 0 ~ 255
//
void utils::normMat(Mat &dst)
{
    double min;
    double max;
    cv::minMaxLoc(dst, &min, &max);
    dst = (255 * (dst - min) / (max - min));
}

//
// Enhance given image with Homomorphic filtering
//
void utils::HFiltering(const Mat &src
    , Mat &mask
    , Mat &dst
    , Mat &dstFFT)
{
    src.convertTo(dst, CV_64F);
    dst += Scalar(1);
    log(dst, dst);

    // Create optimized two band image
    dstFFT = getOptimalImg(dst);
    dft(dstFFT, dstFFT);

    // Apply the filter
    shiftMat(mask);
    multiplyMat(dstFFT, mask, dstFFT);

    // Calculate the inverse of FFT
    dft(dstFFT, dst, DFT_INVERSE | DFT_SCALE | DFT_REAL_OUTPUT);
    exp(dst, dst);
    dst -= Scalar(1);
    normMat(dst);
}

//
// Generate ideal low/high pass filter
//
Mat utils::getIMask(const Size &size, const double &D0, const bool &isLowpass)
{
    Mat mask = Mat::zeros(size, CV_64F);
    int crow = mask.rows / 2;
    int ccol = mask.cols / 2;
    for (int i = 0; i < mask.rows; ++i)
    {
        for (int j = 0; j < mask.cols; ++j)
        {
            double D = sqrt((i-crow)*(i-crow) + (j-ccol)*(j-ccol));
            if (isLowpass)          // For lowpass filter
            {
                if (D <= D0)
                    mask.at<double>(i, j) = 1.0;
            } else                  // For highpass filter
            {
                if (D > D0)
                    mask.at<double>(i, j) = 1.0;
            }
        }
    }
    return mask;
}

//
// Create Butterworth low/high pass filter
//
Mat utils::getBMask(const Size &size, const double &D0, const int &n, const bool &isLowpass)
{
    Mat mask = Mat::zeros(size, CV_64F);
    int crow = mask.rows / 2;
    int ccol = mask.cols / 2;
    for (int i = 0; i < mask.rows; ++i)
    {
        for (int j = 0; j < mask.cols; ++j)
        {
            double D = sqrt((i-crow)*(i-crow) + (j-ccol)*(j-ccol));
            if (isLowpass)              // For lowpass filter
                mask.at<double>(i, j) = 1.0 / (1 + pow((D/D0), (2*n)));
            else                        // For highpass filter
                mask.at<double>(i, j) = 1.0 / (1 + pow((D0/D), (2*n)));
        }
    }
    return mask;
}

//
// Create Gaussian low/high pass filter
//
Mat utils::getGMask(const Size &size, const double &D0, const bool &isLowpass)
{
    Mat mask = Mat::zeros(size, CV_64F);
    int crow = mask.rows / 2;
    int ccol = mask.cols / 2;
    for (int i = 0; i < mask.rows; ++i)
    {
        for (int j = 0; j < mask.cols; ++j)
        {
            double D = sqrt((i-crow)*(i-crow) + (j-ccol)*(j-ccol));
            if (isLowpass)              // For lowpass filter
                mask.at<double>(i, j) = exp((-D*D) / (2*D0*D0));
            else                        // For highpass filter
                mask.at<double>(i, j) = 1 - exp((-D*D) / (2*D0*D0));
        }
    }
    return mask;
}

//
// Create Homomorphic filter
//
Mat utils::getHMask(const Size &size
    , const double &gammaH      // gammaH should greater than 1
    , const double &gammaL      // gammaL should less than 1
    , const double &c
    , const double &D0)
{
    Mat mask = Mat::zeros(size, CV_64F);
    int crow = mask.rows / 2;
    int ccol = mask.cols / 2;
    for (int i = 0; i < mask.rows; ++i)
    {
        for (int j = 0; j < mask.cols; ++j)
        {
            double D = sqrt((i-crow)*(i-crow) + (j-ccol)*(j-ccol));
            mask.at<double>(i, j) =
                (gammaH-gammaL) * (1-exp(-c*((D*D)/(D0*D0)))) + gammaL;
        }
    }
    return mask;
}

//
// Create a resized two band image for Fourier transformation
//
Mat utils::getOptimalImg(const Mat &img)
{
    // Expand input image to optimal size
    int m = getOptimalDFTSize(img.rows);
    int n = getOptimalDFTSize(img.cols);

    Mat padded = Mat::zeros(m, n, CV_64F);
    img.copyTo(padded(Rect(0, 0, img.cols, img.rows)));

    // Generate a new two band image with the second channel is an empty image
    Mat fftImg = Mat::zeros(padded.rows, padded.cols, CV_64FC2);
    for (int i = 0; i < fftImg.rows; ++i)
    {
        for (int j = 0; j < fftImg.cols; ++j)
        {
            fftImg.at<Vec2d>(i, j) = Vec2d(padded.at<double>(i, j), 0);
        }
    }
    return fftImg;
}
