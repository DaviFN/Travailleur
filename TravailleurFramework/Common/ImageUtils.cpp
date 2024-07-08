#include "ImageUtils.h"

#include "Common/FilesystemUtils.h"

bool ImageUtils::matsAreEqual(const cv::Mat& mat1, const cv::Mat& mat2)
{
    if (mat1.dims == mat2.dims &&
        mat1.size == mat2.size &&
        mat1.elemSize() == mat2.elemSize())
    {
        if (mat1.isContinuous() && mat2.isContinuous())
        {
            return 0 == memcmp(mat1.ptr(), mat2.ptr(), mat1.total() * mat1.elemSize());
        }
        else
        {
            const cv::Mat* arrays[] = { &mat1, &mat2, 0 };
            uchar* ptrs[2];
            cv::NAryMatIterator it(arrays, ptrs, 2);
            for (unsigned int p = 0; p < it.nplanes; p++, ++it)
                if (0 != memcmp(it.ptrs[0], it.ptrs[1], it.size * mat1.elemSize()))
                    return false;

            return true;
        }
    }

    return false;
}

// inputs: r, g, b in range 0-255, outputs: h, s, v in ranges h 0-179, s, v 0-255
//bool ImageUtils::RGBtoHSV(const int r, const int g, const int b, int& h, int& s, int& v)
//{
//    if (r < 0 || r > 255) return false;
//    if (g < 0 || g > 255) return false;
//    if (b < 0 || b > 255) return false;
//
//    float fR = (float)(r) / 255.0;
//    float fG = (float)(g) / 255.0;
//    float fB = (float)(b) / 255.0;
//
//    float fH;
//    float fS;
//    float fV;
//
//    RGBtoHSV_innerfunction(fR, fG, fB, fH, fS, fV);
//
//    fH /= 2.0;
//    fS *= 255.0;
//    fV *= 255.0;
//    
//    return true;
//}
//
///*Convert RGB to HSV color space
//
//  Converts a given set of RGB values `r', `g', `b' into HSV
//  coordinates. The input RGB values are in the range [0, 1], and the
//  output HSV values are in the ranges h = [0, 360], and s, v = [0,
//  1], respectively.
//
//  \param fR Red component, used as input, range: [0, 1]
//  \param fG Green component, used as input, range: [0, 1]
//  \param fB Blue component, used as input, range: [0, 1]
//  \param fH Hue component, used as output, range: [0, 360]
//  \param fS Hue component, used as output, range: [0, 1]
//  \param fV Hue component, used as output, range: [0, 1]
//
//*/
//
//void ImageUtils::RGBtoHSV_innerfunction(float& fR, float& fG, float fB, float& fH, float& fS, float& fV) 
//{
//    float fCMax = std::max(std::max(fR, fG), fB);
//    float fCMin = std::min(std::min(fR, fG), fB);
//    float fDelta = fCMax - fCMin;
//
//    if (fDelta > 0) {
//        if (fCMax == fR) {
//            fH = 60 * (fmod(((fG - fB) / fDelta), 6));
//        }
//        else if (fCMax == fG) {
//            fH = 60 * (((fB - fR) / fDelta) + 2);
//        }
//        else if (fCMax == fB) {
//            fH = 60 * (((fR - fG) / fDelta) + 4);
//        }
//
//        if (fCMax > 0) {
//            fS = fDelta / fCMax;
//        }
//        else {
//            fS = 0;
//        }
//
//        fV = fCMax;
//    }
//    else {
//        fH = 0;
//        fS = 0;
//        fV = fCMax;
//    }
//
//    if (fH < 0) {
//        fH = 360 + fH;
//    }
//}
//
//// outputs: h, s, v in ranges h 0-179, s, v 0-255, inputs: r, g, b in range 0-255, 
//void ImageUtils::HSVtoRGB(const int h, const int s, const int v, int& r, int& g, int& b)
//{
//    float fH = 
//}
//
///*Convert HSV to RGB color space
//
//  Converts a given set of HSV values `h', `s', `v' into RGB
//  coordinates. The output RGB values are in the range [0, 1], and
//  the input HSV values are in the ranges h = [0, 360], and s, v =
//  [0, 1], respectively.
//
//  \param fR Red component, used as output, range: [0, 1]
//  \param fG Green component, used as output, range: [0, 1]
//  \param fB Blue component, used as output, range: [0, 1]
//  \param fH Hue component, used as input, range: [0, 360]
//  \param fS Hue component, used as input, range: [0, 1]
//  \param fV Hue component, used as input, range: [0, 1]
//
//*/
//void ImageUtils::HSVtoRGB_innerfunction(float& fR, float& fG, float& fB, float& fH, float& fS, float& fV) 
//{
//    float fC = fV * fS; // Chroma
//    float fHPrime = fmod(fH / 60.0, 6);
//    float fX = fC * (1 - fabs(fmod(fHPrime, 2) - 1));
//    float fM = fV - fC;
//
//    if (0 <= fHPrime && fHPrime < 1) {
//        fR = fC;
//        fG = fX;
//        fB = 0;
//    }
//    else if (1 <= fHPrime && fHPrime < 2) {
//        fR = fX;
//        fG = fC;
//        fB = 0;
//    }
//    else if (2 <= fHPrime && fHPrime < 3) {
//        fR = 0;
//        fG = fC;
//        fB = fX;
//    }
//    else if (3 <= fHPrime && fHPrime < 4) {
//        fR = 0;
//        fG = fX;
//        fB = fC;
//    }
//    else if (4 <= fHPrime && fHPrime < 5) {
//        fR = fX;
//        fG = 0;
//        fB = fC;
//    }
//    else if (5 <= fHPrime && fHPrime < 6) {
//        fR = fC;
//        fG = 0;
//        fB = fX;
//    }
//    else {
//        fR = 0;
//        fG = 0;
//        fB = 0;
//    }
//
//    fR += fM;
//    fG += fM;
//    fB += fM;
//}


//cv::Mat makeUpMatFromRawImage(RawImage* image)
//{
//    cv::Mat newMat(image->getHeight(), image->getWidth(), CV_8UC4);
//
//    for (int i = 0; i < image->getWidth(); ++i)
//    {
//        for (int j = 0; j < image->getHeight(); ++j)
//        {
//            Pixel& pixel = image->at(i, j);
//            cv::Vec4b& bgraVector = newMat.at<cv::Vec4b>(j, i);
//            bgraVector[3] = pixel.a;
//            bgraVector[2] = pixel.r;
//            bgraVector[1] = pixel.g;
//            bgraVector[0] = pixel.b;
//        }
//    }
//    return newMat;
//}
//
//RawImage* makeUpRawImageFromMat(const cv::Mat& mat)
//{
//    const int imageWidth = mat.cols;
//    const int imageHeight = mat.rows;
//
//    RawImage* newImage = new RawImage(imageWidth, imageHeight);
//
//    for (int i = 0; i < imageWidth; ++i)
//    {
//        for (int j = 0; j < imageHeight; ++j)
//        {
//            const cv::Vec4b& bgraVector = mat.at<cv::Vec4b>(j, i);
//            const uint8_t& r = bgraVector[2];
//            const uint8_t& g = bgraVector[1];
//            const uint8_t& b = bgraVector[0];
//            const uint8_t& a = bgraVector[3];
//            Pixel& pixel = newImage->at(i, j);
//            pixel.r = r;
//            pixel.g = g;
//            pixel.b = b;
//            pixel.a = a;
//        }
//    }
//
//    return newImage;
//}
//
//bool saveRawImagePNGformat(RawImage* rawImage, std::string filePath)
//{
//    cv::Mat cvImg = makeUpMatFromRawImage(rawImage);
//
//    std::vector<unsigned char> buff;
//    std::vector<int> param(2);
//    param[0] = cv::IMWRITE_PNG_COMPRESSION;
//    param[1] = 9;
//    cv::imencode(".png", cvImg, buff, param);
//
//    FILE* f = fopen(filePath.c_str(), "wb");
//    if (f != nullptr)
//    {
//        fwrite(buff.data(), sizeof(unsigned char), buff.size(), f);
//        fclose(f);
//        return true;
//    }
//    return false;
//}

//RawImage* loadRawImagePNGformat(const std::string& filepath)
//{
//    if (!FilesystemUtils::fileExists(filepath)) return nullptr;
//
//    const cv::Mat cvImg = cv::imread(filepath, cv::IMREAD_UNCHANGED);
//    return makeUpRawImageFromMat(cvImg);
//}
//
//RawImage* ImageUtils::scaleRawImage(RawImage* sourceImage, const double scaleFactorX, const double scaleFactorY)
//{
//    cv::Mat matImage = makeUpMatFromRawImage(sourceImage);
//
//    if (scaleFactorX > 0 && scaleFactorY > 0)
//    {
//        cv::resize(matImage, matImage, cv::Size(matImage.cols * scaleFactorX, matImage.rows * scaleFactorY), cv::INTER_LINEAR);
//    }
//
//    RawImage* resultingImage = makeUpRawImageFromMat(matImage);
//
//    return resultingImage;
//}
//
//RawImage* ImageUtils::resizeRawImage(RawImage* sourceImage, const double newWidth, const double newHeight)
//{
//    const int width = sourceImage->getWidth();
//    const int height = sourceImage->getHeight();
//    if (width == 0 || height == 0) return nullptr;
//    if (width == newWidth && height == newHeight) return new RawImage(*sourceImage);
//    const double scaleFactorX = (double)newWidth / (double)width;
//    const double scaleFactorY = (double)newHeight / (double)height;
//    return scaleRawImage(sourceImage, scaleFactorX, scaleFactorY);
//}