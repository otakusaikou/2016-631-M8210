#include "utils.h"
#include <QDebug>

#define PI 3.14159265

using namespace std;
using namespace cv;

utils::utils()
{
}

//
// Convert the image color space with given mode
// Reference: https://goo.gl/tKAUkt, https://goo.gl/yZsKcn, https://goo.gl/Jhphu8
//
void utils::convertMat(const Mat &src, Mat &dst, const CvtMode &mode)
{
    // RGB values from 0 to 255
    switch(mode)
    {
        case RGB2CMY:
            // C:[0, 255], M:[0, 255], Y:[0, 255]
            for (int i = 0; i < src.rows; ++i)
            {
                for (int j = 0; j < src.cols; ++j)
                {
                    float R = src.at<Vec3f>(i, j)[0];
                    float G = src.at<Vec3f>(i, j)[1];
                    float B = src.at<Vec3f>(i, j)[2];

                    // Normalize the RGB value to [0, 1]
                    R /= 255.0;
                    G /= 255.0;
                    B /= 255.0;

                    float C = 255.0*(1 - R);
                    float M = 255.0*(1 - G);
                    float Y = 255.0*(1 - B);
                    dst.at<Vec3f>(i, j) = Vec3f(C, M, Y);
                }
            }
            break;

        case RGB2HSI:
            // H:[0, 179], S:[0, 255], I:[0, 255]
            for (int i = 0; i < src.rows; ++i)
            {
                for (int j = 0; j < src.cols; ++j)
                {
                    float R = src.at<Vec3f>(i, j)[0];
                    float G = src.at<Vec3f>(i, j)[1];
                    float B = src.at<Vec3f>(i, j)[2];

                    // Normalize the RGB value to [0, 1]
                    R /= 255.0;
                    G /= 255.0;
                    B /= 255.0;

                    float H;
                    float S;
                    float I;

                    // For Hue
                    float num = ((R-G)+(R-B)) / 2;
                    float den = sqrt(pow((R-G), 2) + (R-B)*(G-B));
                    float theta = acos(num / den) * (180.0/PI);
                    if (den == 0)
                        H = 0;
                    else
                        H = B <= G ? theta : 360.0 - theta;

                    H /= 2.0;

                    // For Saturation
                    num = 3 * min(R, min(G, B));
                    den = (R + G + B);
                    S = den == 0 ? 0 : 255.0 * (1 - num / den);

                    // For intensity
                    I = 255.0 * (R + G + B) / 3;

                    dst.at<Vec3f>(i, j) = Vec3f(H, S, I);
                }
            }
            break;

        case RGB2XYZ:
            // X:[0, 255], Y:[0, 255], Z:[0, 255]
            for (int i = 0; i < src.rows; ++i)
            {
                for (int j = 0; j < src.cols; ++j)
                {
                    float R = src.at<Vec3f>(i, j)[0];
                    float G = src.at<Vec3f>(i, j)[1];
                    float B = src.at<Vec3f>(i, j)[2];

                    // Normalize the RGB value to [0, 1]
                    R /= 255.0;
                    G /= 255.0;
                    B /= 255.0;

                    float X = 0.412453*R + 0.357580*G + 0.180423*B;
                    float Y = 0.212671*R + 0.715160*G + 0.072169*B;
                    float Z = 0.019334*R + 0.119193*G + 0.950227*B;

                    // Yn = 1.0
                    float Xn = 0.950456;
                    float Zn = 1.088754;
                    X *= 255.0/Xn;
                    Y *= 255.0;
                    Z *= 255.0/Zn;

                    dst.at<Vec3f>(i, j) = Vec3f(X, Y, Z);
                }
            }
            break;

        case RGB2Lab:
            for (int i = 0; i < src.rows; ++i)
            {
                for (int j = 0; j < src.cols; ++j)
                {
                    float R = src.at<Vec3f>(i, j)[0];
                    float G = src.at<Vec3f>(i, j)[1];
                    float B = src.at<Vec3f>(i, j)[2];

                    // Normalize the RGB value to [0, 1]
                    R /= 255.0;
                    G /= 255.0;
                    B /= 255.0;

                    // Linearize the nonlinear sRGB values
                    R = R <= 0.04045 ? (R/12.92) : pow((R+0.055) / 1.055, 2.4);
                    G = G <= 0.04045 ? (G/12.92) : pow((G+0.055) / 1.055, 2.4);
                    B = B <= 0.04045 ? (B/12.92) : pow((B+0.055) / 1.055, 2.4);

                    float L;
                    float a;
                    float b;

                    float X = 0.412453*R + 0.357580*G + 0.180423*B;
                    float Y = 0.212671*R + 0.715160*G + 0.072169*B;
                    float Z = 0.019334*R + 0.119193*G + 0.950227*B;

                    // Yn = 1.0
                    float Xn = 0.950456;
                    float Zn = 1.088754;
                    X /= Xn;
                    Z /= Zn;

                    // For L*
                    float FX = X > 0.008856 ? pow(X, 1.0/3.0) : (7.787*X + 16.0/116.0);
                    float FY = Y > 0.008856 ? pow(Y, 1.0/3.0) : (7.787*Y + 16.0/116.0);
                    float FZ = Z > 0.008856 ? pow(Z, 1.0/3.0) : (7.787*Z + 16.0/116.0);

                    L = Y > 0.008856 ? 116.0*FY - 16 : 903.3*Y;
                    L *=  255.0/100.0;

                    // For a* and b*
                    a = 500.0*(FX - FY) + 128;
                    b = 200.0*(FY - FZ) + 128;

                    dst.at<Vec3f>(i, j) = Vec3f(L, a, b);
                }
            }
            break;

        case RGB2YUV:
            for (int i = 0; i < src.rows; ++i)
            {
                for (int j = 0; j < src.cols; ++j)
                {
                    float R = src.at<Vec3f>(i, j)[0];
                    float G = src.at<Vec3f>(i, j)[1];
                    float B = src.at<Vec3f>(i, j)[2];

                    // Normalize the RGB value to [0, 1]
                    R /= 255.0;
                    G /= 255.0;
                    B /= 255.0;

                    float Y = 0.299*R + 0.587*G + 0.114*B;
                    float U = (R-Y)*0.713 + 0.5;      // Cr
                    float V = (B-Y)*0.564 + 0.5;      // Cb

                    Y *= 255.0;
                    U *= 255.0;
                    V *= 255.0;
                    dst.at<Vec3f>(i, j) = Vec3f(Y, U, V);
                }
            }
            break;
    }
}
