#ifndef RawImage_H
#define RawImage_H

#include <opencv2/opencv.hpp>
#include <memory>
#include <stdint.h>
#include <string>

#include "Common/Point2D.h"
#include "Common/Rectangle2D.h"

struct Pixel
{
public:
	Pixel() {};
	Pixel(const uint8_t gray) : r(gray), g(gray), b(gray), a(255) {};
	Pixel(const uint8_t r, const uint8_t g, const uint8_t b) : r(r), g(g), b(b), a(255) {};
	Pixel(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a) : r(r), g(g), b(b), a(a) {};
	Pixel(const Pixel& other) : r(other.r), g(other.g), b(other.b), a(other.a) {};
	inline Pixel& operator=(const Pixel& other)
	{
		r = other.r;
		g = other.g;
		b = other.b;
		a = other.a;
		return *this;
	}
	inline bool operator!=(const Pixel& other) const
	{
		return !(other == *this);
	}
	inline bool operator==(const Pixel& other) const
	{
		return r == other.r && g == other.g && b == other.b && a == other.a;
	}
	inline bool isTransparent() const
	{
		return r == 0 && g == 0 && b == 0 && a == 0;
	}
	inline static Pixel transparentPixel() {
		return Pixel(0, 0, 0, 0);
	}
	inline void invert()
	{
		r = 255 - r;
		g = 255 - g;
		b = 255 - b;
	}
	int getPositionSquared() const
	{
		return r * r + g * g + b * b;
	}
	static int getMaximumPossiblePositionSquared()
	{
		return 255 * 255 + 255 * 255 + 255 * 255;
	}
	void saveToBinaryFile(const std::string& persistenceFilepath) const;
	void loadFromBinaryFile(const std::string& persistenceFilepath);
	void saveToBinaryFile(FILE* f) const;
	void loadFromBinaryFile(FILE* f);
	uint8_t r = 0;
	uint8_t g = 0;
	uint8_t b = 0;
	uint8_t a = 255;
};

// when referencing a pixel in the image,
// the first parameter refers to the horizontal position starting from the left,
// and the second parameter refers to the vertical position starting from the top
// the logic is: start from the top-left corner and go right, and it goes like we write
class Image
{
public:
	Image();
	Image(const int width, const int height);
	Image(const std::string& filepath);
	Image(const Image& other);
	Image(Image&& other);
	~Image();
	void saveToBinaryFile(const std::string& persistenceFilepath) const;
	void loadFromBinaryFile(const std::string& persistenceFilepath);
	bool operator==(const Image& other) const;
	bool operator!=(const Image& other) const;
	Image& operator=(const Image& other);
	Image& operator=(Image&& other);

	inline int getWidth() const
	{
		return mat != nullptr ? mat->cols : 0;
	};

	inline int getHeight() const
	{
		return mat != nullptr ? mat->rows : 0;
	};

	inline int nPixels() const
	{
		return mat != nullptr ? getWidth() * getHeight() : 0;
	};

	inline Rectangle2D<int> getRegion() const
	{
		return Rectangle2D<int>(Point2D<int>(0, 0), Point2D<int>(getWidth(), getHeight()));
	}

	void changeAllPixelsOfSpecificColor(const Pixel& color, const Pixel& colorToChangeTo);

	std::string generateStringRepresentation() const;

	std::string generateMd5Hash() const;

	void setToEmptyMaskForImage(const Image& image);

	void fillWithSamePixel(const Pixel& pixel);

	bool isValid() const;

	bool hasValidDimensions() const;

	bool hasSameDimensionsOf(const Image& other) const;

	bool crop(const Point2D<int>& topLeft, const Point2D<int>& size);

	bool crop(const Rectangle2D<int>& region);

	void invert();

	bool resize(const int newWidth, const int newHeight);

	bool scale(const double scaleFactorX, const double scaleFactorY);

	void toGrayScale();

	void setToTransparent();

	void drawMask(const Image& mask);

	void drawRawImage(const Point2D<int>& topLeft, const Image& image);
	
	void drawRectangle(const Point2D<int>& topLeft, const Point2D<int>& size, const Pixel color = Pixel(0, 0 ,0));
	
	void highlightRectangularRegion(const Point2D<int>& topLeft, const Point2D<int>& size, const int highlightSize, const Pixel color = Pixel(0, 0, 0), const bool excludeCornerBorders = false);
	
	void drawCross(const Point2D<int>& center, const int size, const int thickness, const Pixel color = Pixel(0, 0, 0), const bool excludeMiddlePart = false);

	inline bool isValidPixelLocation(const int x, const int y)
	{
		return x >= 0 && x < getWidth()  && y >= 0 &&y < getHeight();
	}

	inline bool isValidPixelLocation(const Point2D<int>& point)
	{
		return isValidPixelLocation(point.x, point.y);
	}

	inline Pixel pixelAt(const int unidimensionalPosition) const
	{
		if (mat == nullptr) {
			return Pixel(0, 0, 0);
		}
		const int x = unidimensionalPosition % getWidth();
		const int y = unidimensionalPosition / getWidth();
		return pixelAt(x, y);
	}

	inline Pixel pixelAt(const int x, const int y) const
	{
		if (mat == nullptr || x < 0 || y < 0 || x >= getWidth() || y >= getHeight()) {
			return Pixel(0, 0, 0);
		}
		const cv::Vec4b& bgraVector = mat->at<cv::Vec4b>(y, x);
		const uint8_t& b = bgraVector[0];
		const uint8_t& g = bgraVector[1];
		const uint8_t& r = bgraVector[2];
		const uint8_t& a = bgraVector[3];
		return Pixel(r, g, b, a);
	}

	inline Pixel pixelAt(const Point2D<int>& point) const
	{
		return pixelAt(point.x, point.y);
	};

	inline void setPixel(const int x, const int y, const Pixel& pixel)
	{
		if (mat == nullptr) return;
		if (isValidPixelLocation(x, y)) {
			cv::Vec4b& bgraVector = mat->at<cv::Vec4b>(y, x);
			bgraVector[0] = pixel.b;
			bgraVector[1] = pixel.g;
			bgraVector[2] = pixel.r;
			bgraVector[3] = pixel.a;
		}
	};

	inline void setPixel(const Point2D<int>& point, const Pixel& pixel)
	{
		setPixel(point.x, point.y, pixel);
	}

	inline cv::Mat* getMat()
	{
		return mat;
	}

	inline void setMat(const cv::Mat& mat)
	{
		delete this->mat;
		this->mat = new cv::Mat(mat);
	}

	void drawText(const Point2D<int>& topLeft, const std::string& text, const Pixel color = Pixel(0, 0, 0), const double scaleParameter = 1.0);

	void drawTextWithBackground(const Point2D<int>& topLeft, const std::string& text, const size_t borderSize = 0, const Pixel color = Pixel(0, 0, 0), const Pixel backgroundColor = Pixel(255, 255, 255), const double scaleParameter = 1.0);

	bool saveAsPng(const std::string& filepath) const;

	bool loadFromFile(const std::string& filepath);

private:
	cv::Mat* mat = nullptr;
};

#endif