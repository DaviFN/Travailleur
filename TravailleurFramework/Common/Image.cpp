#include "Image.h"

#include <cstdio>

#include "Common/ImageUtils.h"
#include "Common/FilesystemUtils.h"
#include "Common/SimplePersistenceUtils.h"
#include "md5.h"
#include "HexUtils.h"

void Pixel::saveToBinaryFile(const std::string& persistenceFilepath) const
{
	std::vector<uint8_t> components;
	components.reserve(4);
	components.push_back(r);
	components.push_back(g);
	components.push_back(b);
	components.push_back(a);
	SimplePersistenceUtils::saveVectorOfUint8(FilesystemUtils::filenameFromFilepath(persistenceFilepath), components, FilesystemUtils::parentDirectoryOfFilepath(persistenceFilepath));
}

void Pixel::loadFromBinaryFile(const std::string& persistenceFilepath)
{
	std::vector<uint8_t> components(4);
	const LoadOperationResult result = SimplePersistenceUtils::loadVectorOfUint8(FilesystemUtils::filenameFromFilepath(persistenceFilepath), components, FilesystemUtils::parentDirectoryOfFilepath(persistenceFilepath));
	if (result == LoadOperationResult::LoadedSuccessfully) {
		r = components[0];
		g = components[1];
		b = components[2];
		a = components[3];
	}
}

void Pixel::saveToBinaryFile(FILE* f) const
{
	fwrite(&r, sizeof(uint8_t), 1, f);
	fwrite(&g, sizeof(uint8_t), 1, f);
	fwrite(&b, sizeof(uint8_t), 1, f);
	fwrite(&a, sizeof(uint8_t), 1, f);
}

void Pixel::loadFromBinaryFile(FILE* f)
{
	fread(&r, sizeof(uint8_t), 1, f);
	fread(&g, sizeof(uint8_t), 1, f);
	fread(&b, sizeof(uint8_t), 1, f);
	fread(&a, sizeof(uint8_t), 1, f);
}

Image::Image()
{
}

Image::Image(const int width, const int height)
{
	if (width > 0 && height > 0) {
		mat = new cv::Mat(height, width, CV_8UC4, cv::Scalar(0, 0, 0, 255));
	}
}

Image::Image(const std::string& filepath)
{
	loadFromFile(filepath);
}

Image::Image(const Image& other)
{
	if (other.mat != nullptr) {
		mat = new cv::Mat(other.mat->clone());
	}
}

Image::Image(Image&& other)
{
	if (other.mat != nullptr) {
		mat = new cv::Mat(std::move(*other.mat));
	}
}

Image::~Image()
{
	delete mat;
}

void Image::saveToBinaryFile(const std::string& persistenceFilepath) const
{
	FILE* f = fopen(persistenceFilepath.c_str(), "wb");
	if (f != nullptr) {
		const int width = getWidth();
		const int height = getHeight();
		fwrite(&width, sizeof(int), 1, f);
		fwrite(&height, sizeof(int), 1, f);
		if (width > 0 && height > 0) {
			uint8_t* imageData = new uint8_t[width * height * 4];
			for (int i = 0; i < width; ++i) {
				for (int j = 0; j < height; j++) {
					const Pixel pixel = pixelAt(i, j);
					const int pixelsIndex = i * 4 * height + j * 4;
					imageData[pixelsIndex] = pixel.r;
					imageData[pixelsIndex + 1] = pixel.g;
					imageData[pixelsIndex + 2] = pixel.b;
					imageData[pixelsIndex + 3] = pixel.a;
				}
			}
			fwrite(imageData, sizeof(uint8_t), width * height * 4, f);
			delete[] imageData;
		}
		fclose(f);
	}
}

void Image::loadFromBinaryFile(const std::string& persistenceFilepath)
{
	FILE* f = fopen(persistenceFilepath.c_str(), "rb");
	if (f != nullptr) {
		int width = 0;
		int height = 0;
		fread((void*)&(width), sizeof(int), 1, f);
		fread((void*)&(height), sizeof(int), 1, f);
		if (width > 0 && height > 0) {
			delete mat;
			mat = new cv::Mat(height, width, CV_8UC4, cv::Scalar(0, 0, 0, 255));
			uint8_t* imageData = new uint8_t[width * height * 4];
			fread(imageData, sizeof(uint8_t), width * height * 4, f);
			for (int i = 0; i < width; ++i) {
				for (int j = 0; j < height; j++) {
					const int pixelsIndex = i * 4 * height + j * 4;
					const Pixel pixel(imageData[pixelsIndex], imageData[pixelsIndex + 1], imageData[pixelsIndex + 2], imageData[pixelsIndex + 3]);
					setPixel(i, j, pixel);
				}
			}
			delete[] imageData;
		}
		fclose(f);
	}
}

bool Image::operator==(const Image& other) const
{
	return ImageUtils::matsAreEqual(*mat, *other.mat);
}

bool Image::operator!=(const Image& other) const
{
	return !(other == *this);
}

Image& Image::operator=(const Image& other)
{
	if (other.mat != nullptr) {
		if (mat != nullptr) {
			*mat = other.mat->clone();
		}
		else {
			mat = new cv::Mat(*other.mat);
		}
	}
	else {
		delete mat;
		mat = nullptr;
	}
	return *this;
}

Image& Image::operator=(Image&& other)
{
	if (mat != nullptr) {
		*mat = std::move(*other.mat);
	}
	else {
		mat = other.mat;
	}
	return *this;
}

void Image::changeAllPixelsOfSpecificColor(const Pixel& color, const Pixel& colorToChangeTo)
{
	if (mat != nullptr) {
		for (size_t i = 0; i < getWidth(); ++i) {
			for (size_t j = 0; j < getHeight(); ++j) {
				if (pixelAt(i, j) == color) {
					setPixel(i, j, colorToChangeTo);
				}

			}
		}
	}
}

std::string Image::generateStringRepresentation() const
{
	if (mat == nullptr) {
		return "empty image";
	}
	std::string dimensionsRepresentation = std::to_string(getWidth()) + "x" + std::to_string(getHeight()) + "-";

	// to avoid performance loss due to string concatenations
	char* pixelsStringRepresentation = new char[getWidth() * getHeight() * 8 + 1];
	pixelsStringRepresentation[getWidth() * getHeight() * 8] = 0; // null terminator
	for (size_t j = 0; j < getHeight(); ++j) {
		for (size_t i = 0; i < getWidth(); ++i) {
			const Pixel pixel = pixelAt(i, j);
			char* whereToPutPixelHexRepresentation = pixelsStringRepresentation + 8 * (i + j * getWidth());
			HexUtils::putHexByteRepresentationInCStrWithoutNullTerminator(whereToPutPixelHexRepresentation, pixel.r);
			HexUtils::putHexByteRepresentationInCStrWithoutNullTerminator(whereToPutPixelHexRepresentation + 2, pixel.g);
			HexUtils::putHexByteRepresentationInCStrWithoutNullTerminator(whereToPutPixelHexRepresentation + 4, pixel.b);
			HexUtils::putHexByteRepresentationInCStrWithoutNullTerminator(whereToPutPixelHexRepresentation + 6, pixel.a);
		}
	}

	const std::string stringRepresentation = dimensionsRepresentation + std::string(pixelsStringRepresentation);

	delete[] pixelsStringRepresentation;

	return stringRepresentation;
}

std::string Image::generateMd5Hash() const
{
	return md5(generateStringRepresentation());
}

void Image::setToEmptyMaskForImage(const Image& image)
{
	if (!hasSameDimensionsOf(image)) {
		if (mat != nullptr) {
			*mat = image.mat->clone();
		}
		else {
			mat = new cv::Mat(*image.mat);
		}
	}
	setToTransparent();
}

void Image::fillWithSamePixel(const Pixel& pixel)
{
	if (mat == nullptr) return;
	mat->setTo(cv::Scalar(pixel.b, pixel.g, pixel.r, pixel.a));
}

bool Image::isValid() const
{
	return mat != nullptr && hasValidDimensions();
}

bool Image::hasValidDimensions() const
{
	return getWidth() > 0 && getHeight() > 0;
}

bool Image::hasSameDimensionsOf(const Image& other) const
{
	return getWidth() == other.getWidth() && getHeight() == other.getHeight();
}

bool Image::crop(const Point2D<int>& topLeft, const Point2D<int>& size)
{
	if (mat == nullptr || !hasValidDimensions()) return false;
	cv::Rect safeRect(topLeft.x, topLeft.y, size.x, size.y);
	(*mat)(safeRect).copyTo(*mat);
	return true;
}

bool Image::crop(const Rectangle2D<int>& region)
{
	return crop(region.topLeft, region.size);
}

void Image::invert()
{
	if (mat == nullptr) return;
	for (size_t i = 0; i < getWidth(); ++i) {
		for (size_t j = 0; j < getHeight(); ++j) {
			Pixel pixel = pixelAt(i, j);
			pixel.invert();
			setPixel(i, j, pixel);
		}
	}
}

bool Image::resize(const int newWidth, const int newHeight)
{
	if (mat == nullptr) return false;
	if (newWidth <= 0 || newHeight <= 0) return false;
	cv::resize(*mat, *mat, cv::Size(newWidth, newHeight), cv::INTER_LINEAR);
	return true;
}

bool Image::scale(const double scaleFactorX, const double scaleFactorY)
{
	if (mat == nullptr) return false;
	const int newSizeX = mat->cols * scaleFactorX;
	const int newSizeY = mat->rows * scaleFactorY;
	if (newSizeX <= 0 || newSizeY <= 0) {
		return false;
	}
	cv::resize(*mat, *mat, cv::Size(mat->cols * scaleFactorX, mat->rows * scaleFactorY), cv::INTER_LINEAR);
	return true;
}

void Image::toGrayScale()
{
	if (mat == nullptr) return;
	cv::cvtColor(*mat, *mat, cv::COLOR_BGRA2GRAY);
	cv::cvtColor(*mat, *mat, cv::COLOR_GRAY2BGRA);
}

void Image::setToTransparent()
{
	if (mat == nullptr) return;
	mat->setTo(cv::Scalar(0, 0, 0, 0));
}

void Image::drawMask(const Image& mask)
{
	if (mat == nullptr) return;
	//Image* maskToDraw = hasSameDimensionsOf(mask) ? new Image(mask) : new Image(mask);
	//if (!hasSameDimensionsOf(*maskToDraw)) {
	//	maskToDraw->resize(getWidth(), getHeight());
	//}
	
	if (!hasSameDimensionsOf(mask)) {
		return;
	}
	//for (int i = 0; i < maskToDraw->getWidth(); ++i) {
	//	for (int j = 0; j < maskToDraw->getHeight(); ++j) {
	//		const Pixel& pixel = maskToDraw->pixelAt(i, j);
	//		if (!pixel.isTransparent()) {
	//			setPixel(i, j, Pixel(pixel.r, pixel.g, pixel.b, pixel.a));
	//		}
	//	}
	//}
	for (int i = 0; i < mask.getWidth(); ++i) {
		for (int j = 0; j < mask.getHeight(); ++j) {
			const Pixel& pixel = mask.pixelAt(i, j);
			if (!pixel.isTransparent()) {
				setPixel(i, j, Pixel(pixel.r, pixel.g, pixel.b, pixel.a));
			}
		}
	}

	//delete maskToDraw;
}

void Image::drawRawImage(const Point2D<int>& topLeft, const Image& image)
{
	if (mat == nullptr) return;
	for (int i = 0; i < image.getWidth(); ++i) {
		for (int j = 0; j < image.getHeight(); ++j) {
			const int myI = i + topLeft.x;
			const int myJ = j + topLeft.y;
			setPixel(myI, myJ, image.pixelAt(i, j));
		}
	}
}

void Image::drawRectangle(const Point2D<int>& topLeft, const Point2D<int>& size, const Pixel color)
{
	if (mat == nullptr) return;
	for (int i = topLeft.x; i < topLeft.x + size.x; ++i) {
		for (int j = topLeft.y; j < topLeft.y + size.y; ++j) {
			setPixel(i, j, color);
		}
	}
}

void Image::highlightRectangularRegion(const Point2D<int>& topLeft, const Point2D<int>& size, const int highlightSize, const Pixel color, const bool excludeCornerBorders)
{
	if (mat == nullptr) return;
	// top border
	for (int i = topLeft.x; i < topLeft.x + size.x; ++i) {
		for (int j = topLeft.y - highlightSize; j < topLeft.y; ++j) {
			setPixel(i, j, color);
		}
	}
	// bottom border
	for (int i = topLeft.x; i < topLeft.x + size.x; ++i) {
		for (int j = topLeft.y + size.y; j < topLeft.y + size.y + highlightSize; ++j) {
			setPixel(i, j, color);
		}
	}
	// left border
	for (int i = topLeft.x - highlightSize; i < topLeft.x; ++i) {
		for (int j = topLeft.y; j < topLeft.y + size.y; ++j) {
			setPixel(i, j, color);
		}
	}
	// right border
	for (int i = topLeft.x + size.x; i < topLeft.x + size.x + highlightSize; ++i) {
		for (int j = topLeft.y; j < topLeft.y + size.y; ++j) {
			setPixel(i, j, color);
		}
	}
	if (!excludeCornerBorders) {
		// top left corner
		for (int i = topLeft.x - highlightSize; i < topLeft.x; ++i) {
			for (int j = topLeft.y - highlightSize; j < topLeft.y; ++j) {
				setPixel(i, j, color);
			}
		}
		// bottom left corner
		for (int i = topLeft.x - highlightSize; i < topLeft.x; ++i) {
			for (int j = topLeft.y + size.y; j < topLeft.y + size.y + highlightSize; ++j) {
				setPixel(i, j, color);
			}
		}
		// bottom right corner
		for (int i = topLeft.x + size.x; i < topLeft.x + size.x + highlightSize; ++i) {
			for (int j = topLeft.y + size.y; j < topLeft.y + size.y + highlightSize; ++j) {
				setPixel(i, j, color);
			}
		}
		// top right corner
		for (int i = topLeft.x + size.x; i < topLeft.x + size.x + highlightSize; ++i) {
			for (int j = topLeft.y - highlightSize; j < topLeft.y; ++j) {
				setPixel(i, j, color);
			}
		}
	}
}

void Image::drawCross(const Point2D<int>& center, const int size, const int thickness, const Pixel color, const bool excludeMiddlePart)
{
	if (mat == nullptr) return;
	// left part
	for (int i = center.x - thickness - size; i < center.x - thickness; ++i) {
		for (int j = center.y - thickness; j < center.y + thickness; ++j) {
			setPixel(i, j, color);
		}
	}
	// right part
	for (int i = center.x + thickness; i < center.x + thickness + size; ++i) {
		for (int j = center.y - thickness; j < center.y + thickness; ++j) {
			setPixel(i, j, color);
		}
	}
	// top part
	for (int i = center.x - thickness; i < center.x + thickness; ++i) {
		for (int j = center.y - thickness - size; j < center.y - thickness; ++j) {
			setPixel(i, j, color);
		}
	}
	// bottom part
	for (int i = center.x - thickness; i < center.x + thickness; ++i) {
		for (int j = center.y + thickness; j < center.y + thickness + size; ++j) {
			setPixel(i, j, color);
		}
	}
	// middle part
	if (!excludeMiddlePart) {
		for (int i = center.x - thickness; i < center.x + thickness; ++i) {
			for (int j = center.y - thickness; j < center.y + thickness; ++j) {
				setPixel(i, j, color);
			}
		}
	}
}

void Image::drawText(const Point2D<int>& topLeft, const std::string& text, const Pixel color, const double scaleParameter)
{
	if (mat == nullptr) return;
	cv::Size textDimensions = cv::getTextSize(text, cv::FONT_HERSHEY_DUPLEX, scaleParameter, 1, 0);
	cv::putText(*mat, text, cv::Point(topLeft.x, topLeft.y + textDimensions.height), cv::FONT_HERSHEY_DUPLEX, scaleParameter, cv::Scalar(color.r, color.g, color.b, 255));
}

void Image::drawTextWithBackground(const Point2D<int>& topLeft, const std::string& text, const size_t borderSize, const Pixel color, const Pixel backgroundColor, const double scaleParameter)
{
	if (mat == nullptr) return;
	cv::Size textDimensions = cv::getTextSize(text, cv::FONT_HERSHEY_DUPLEX, scaleParameter, 1, 0);
	drawRectangle(Point2D<int>(topLeft.x - borderSize, topLeft.y - borderSize), Point2D<int>(textDimensions.width + 2 * borderSize, textDimensions.height + 2 * borderSize), backgroundColor);
	cv::putText(*mat, text, cv::Point(topLeft.x, topLeft.y + textDimensions.height), cv::FONT_HERSHEY_DUPLEX, 1.0, cv::Scalar(color.r, color.g, color.b, 255));
}

bool Image::saveAsPng(const std::string& filepath) const
{
	if (mat == nullptr) return false;
	std::vector<unsigned char> buffer;
	std::vector<int> param(2);
	param[0] = cv::IMWRITE_PNG_COMPRESSION;
	param[1] = 9;
	cv::imencode(".png", *mat, buffer, param);
	
	if (FilesystemUtils::fileExists(filepath)) return false;

	FILE* f = fopen(filepath.c_str(), "wb");
	if (f != nullptr)
	{
	    fwrite(buffer.data(), sizeof(unsigned char), buffer.size(), f);
	    fclose(f);
	    return true;
	}
	return false;
}

bool Image::loadFromFile(const std::string& filepath)
{
	delete mat;
	mat = nullptr;
	if (!FilesystemUtils::fileExists(filepath)) {
		return false;
	};
	mat = new cv::Mat(cv::imread(filepath, cv::IMREAD_UNCHANGED));
	return true;
}