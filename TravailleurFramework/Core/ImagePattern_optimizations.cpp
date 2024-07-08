#include "ImagePattern.h"

// four bits represents a pattern color
// that can be used to represent up to 15 colors, alongside the transparent color
// 0 represents the transparent color, and 1 to 15 represent the pattern colors

void initBufferAccordingToBufferDimension(void** buffer)
{
	const size_t nPixelsInImageApproximationBuffer = ImagePattern::bufferWidth * ImagePattern::bufferHeight;

	size_t nBytesInBuffer = nPixelsInImageApproximationBuffer / 2;
	if (nPixelsInImageApproximationBuffer % 2 != 0) {
		nBytesInBuffer++;
	}

	*buffer = malloc(nBytesInBuffer * sizeof(uint8_t));
	for (size_t i = 0; i < nBytesInBuffer; ++i) {
		((uint8_t*)(*buffer))[i] = 0;
	}
}

void ImagePattern::initImageOptimizationBuffer()
{
	initBufferAccordingToBufferDimension(&imageOptimizationBuffer);
}

void ImagePattern::deinitImageOptimizationBuffer()
{
	free(imageOptimizationBuffer);
	imageOptimizationBuffer = nullptr;
}

void ImagePattern::initPatternOptimizationBuffer()
{
	initBufferAccordingToBufferDimension(&patternOptimizationBuffer);
}

void ImagePattern::deinitPatternOptimizationBuffer()
{
	free(patternOptimizationBuffer);
	patternOptimizationBuffer = nullptr;
}

void ImagePattern::initOptimizationBuffers()
{
	initImageOptimizationBuffer();
	initPatternOptimizationBuffer();

	maxWrongPixelsAllowedOptimizationBuffer = new uint32_t[15];
	for (uint8_t i = 0; i < 15; ++i) {
		maxWrongPixelsAllowedOptimizationBuffer[i] = 0;
	}
}

void ImagePattern::deinitOptimizationBuffers()
{
	deinitImageOptimizationBuffer();
	deinitPatternOptimizationBuffer();

	delete[] maxWrongPixelsAllowedOptimizationBuffer;
}

uint8_t get4bitsThatRepresentApproximatedColor(
	const Pixel& approximatedColor,
	const std::vector<ImagePattern::PatternColor>& patternColors
)
{
	for (uint8_t i = 0; i < patternColors.size(); ++i) {
		if (i >= 16) return 0b00000000;
		if (approximatedColor == patternColors[i].color) {
			return i + 1;
		}
	}
	return 0b00000000;
}

void ImagePattern::fillOptimizationBuffer(void* buffer, Image* image) const
{
	const size_t nPixels = image->nPixels();
	for (size_t i = 0; 2 * i < nPixels; ++i) {
		const size_t approximatedColorIndex = 2 * i;

		uint8_t representation = 0;

		if (2 * i + 1 < nPixels) {
			const Pixel approximationOfFirstColor = getPixelApproximation(image->pixelAt(approximatedColorIndex + 1));

			representation = get4bitsThatRepresentApproximatedColor(approximationOfFirstColor, patternColors);
		}

		representation = representation << 4;

		
		const Pixel approximationOfSecondColor = getPixelApproximation(image->pixelAt(approximatedColorIndex));
		const uint8_t last4Bits = get4bitsThatRepresentApproximatedColor(approximationOfSecondColor, patternColors);
		representation |= last4Bits;

		((uint8_t*)buffer)[i] = representation;
	}
}

void ImagePattern::fillOptimizationBuffers(Image* image, const Rectangle2D<int>& searchRegion) const
{
	Image subImageRepresentingSearchRegion(*image);
	subImageRepresentingSearchRegion.crop(searchRegion);
	fillOptimizationBuffer(imageOptimizationBuffer, &subImageRepresentingSearchRegion);
	fillOptimizationBuffer(patternOptimizationBuffer, pattern.get());

	const size_t nPixelsInPattern = pattern->nPixels();
	for (uint8_t i = 0; i < patternColors.size(); ++i) {
		if (i <= 15) {
			const PatternColor& patternColor = patternColors[i];
			const int maxWrongPixelsAllowed = floor((nPixelsInPattern * (100.0 - patternColor.minMatchesPercentageToStillConsiderMatch)) / 100.0);
			maxWrongPixelsAllowedOptimizationBuffer[i] = maxWrongPixelsAllowed;
		}
	}
}

bool ImagePattern::checkForMatch_optimized(const Rectangle2D<int>& searchRegion, const int x, const int y) const
{
	if (patternColors.size() == 0 || searchRegion.size.x == 0 || searchRegion.size.y == 0 || x > searchRegion.getWidth() - pattern->getWidth() || y > searchRegion.getHeight() - pattern->getHeight()) return false;

	const size_t nPixelsInPattern = pattern->nPixels();

	const size_t patternWidth = pattern->getWidth();
	const size_t patternHeight = pattern->getHeight();
	const size_t imageWidth = searchRegion.getWidth();

	// optimized implementation
	uint32_t wrongPixelsCountForPatternColor[15];
	for (uint8_t i = 0; i < 15; ++i) {
		wrongPixelsCountForPatternColor[i] = 0;
	}

	// in optimization buffers, each 4 bits represent a pattern color (or transparent)
	// originally there are totalNumberOfPixels approximated pixels (for the image pattern),
	// each approximated pixel took originally 4x8 = 32 bits to be represented
	// now it takes 4 bits, hence it takes an eigth of what it used to take
	// so the pixels are represented by totalNumberOfPixels/8 bits, in imageOptimizationBuffer
	// in other words, one byte is now able to represent two approximated pixels
	// we can read 8 bytes at a time, hence we can read the representation of 16 approximated pixels at a time
	//uint32_t nOfQwordsToRead = nPixelsInPattern / 16;
	//if (nPixelsInPattern % 16 != 0) {
	//	++nOfQwordsToRead;
	//}

	//uint64_t* imageQwords = (uint64_t*)imageOptimizationBuffer;
	uint64_t* patternQwords = (uint64_t*)patternOptimizationBuffer;

	uint64_t currentPatternQword = 0;

	for (uint32_t j = 0; j < patternHeight; ++j) {

		uint32_t T = (x) + (y + j) * imageWidth;

		// two cases: T is even or T is odd
		// if T is even, the row in the image (which is, in fact, the sub image that represents the search region)
		// is of type R
		// else, is of type S
		// in type R, the first 4 bits of the first qword represent the first color in the subimage
		// in type S, the first 4 bits are to be ignored, and the next 4 bits represent the first color

		// T / 2 specifies the first byte of the first qword to be read
		uint64_t* currentImageQwordAddress = (uint64_t*)((uint8_t*)imageOptimizationBuffer + T/2);
		uint64_t currentImageQword = *currentImageQwordAddress;


		if (T % 2 == 0) {
			// type R

			// we just read just like we read the pattern colors

			for (uint32_t i = 0; i < patternWidth; ++i) {

				// we possibly update the current search region qword:
				if (i != 0 && i % 16 == 0) {
					++currentImageQwordAddress;
					currentImageQword = *currentImageQwordAddress;
				}

				uint32_t pos = i + patternWidth * j;
			
				uint8_t patternColorPositionInPatternQword = (pos % 16) * 4;
			
				if (patternColorPositionInPatternQword == 0) {
					currentPatternQword = patternQwords[pos / 16];
				}
			
				// [bbbb bbbb] [bbbb bbbb] [bbbb bbbb] [bbbb bbbb] [bbbb bbbb] [bbbb bbbb] [bbbb bbbb] [bbbb bbbb]
			
				uint8_t patternColorBitRepresentation = (currentPatternQword >> (patternColorPositionInPatternQword)) & 0b00001111;

				if (patternColorBitRepresentation != 0) // 0 is the transparent pixel, which indicates that the approximation did not yield any of the pattern colors
				{
					 
					uint8_t imageColorBitRepresentation = (currentImageQword >> ((i % imageWidth) * 4)) & 0b00001111;

					if (patternColorBitRepresentation != imageColorBitRepresentation) {
						++wrongPixelsCountForPatternColor[patternColorBitRepresentation - 1];
						if (wrongPixelsCountForPatternColor[patternColorBitRepresentation - 1] > maxWrongPixelsAllowedOptimizationBuffer[patternColorBitRepresentation - 1]) {
							return false;
						}
					}

				}

			}

		}
		else {
			// type S

			// we just read just like we read the pattern colors

			for (uint32_t i = 0; i < patternWidth; ++i) {

				// we possibly update the current search region qword:
				if (i != 0 && (i + 1) % 16 == 0) {
					++currentImageQwordAddress;
                    currentImageQword = *currentImageQwordAddress;
				}

				uint32_t pos = i + patternWidth * j;

				uint8_t patternColorPositionInPatternQword = (pos % 16) * 4;

				if (patternColorPositionInPatternQword == 0) {
					currentPatternQword = patternQwords[pos / 16];
				}

				// [bbbb bbbb] [bbbb bbbb] [bbbb bbbb] [bbbb bbbb] [bbbb bbbb] [bbbb bbbb] [bbbb bbbb] [bbbb bbbb]

				uint8_t patternColorBitRepresentation = (currentPatternQword >> (patternColorPositionInPatternQword)) & 0b00001111;

				if (patternColorBitRepresentation != 0) // 0 is the transparent pixel, which indicates that the approximation did not yield any of the pattern colors
				{

					uint8_t imageColorBitRepresentation = (currentImageQword >> ((i % imageWidth) * 4 + 4)) & 0b00001111;

					if (patternColorBitRepresentation != imageColorBitRepresentation) {
						++wrongPixelsCountForPatternColor[patternColorBitRepresentation - 1];
						if (wrongPixelsCountForPatternColor[patternColorBitRepresentation - 1] > maxWrongPixelsAllowedOptimizationBuffer[patternColorBitRepresentation - 1]) {
							return false;
						}
					}

				}

			}
		}

		//uint64_t* currentSearchRegionQword = ;
		//
		//for (uint32_t i = 0; i < patternWidth; ++i) {
		//	uint32_t pos = i + patternWidth * j;
		//
		//	uint8_t patternColorPositionInPatternQword = (pos % 16) * 4;
		//
		//	if (patternColorPositionInPatternQword == 0) {
		//		currentPatternQword = patternQwords[pos];
		//	}
		//
		//	// [bbbb bbbb] [bbbb bbbb] [bbbb bbbb] [bbbb bbbb] [bbbb bbbb] [bbbb bbbb] [bbbb bbbb] [bbbb bbbb]
		//
		//	uint8_t patternColorBitRepresentation = (currentPatternQword >> (60 - patternColorPositionInPatternQword)) & 0b00001111;
		//	
		//	if (patternColorBitRepresentation != 0) // 0 is the transparent pixel, which indicates that the approximation did not yield any of the pattern colors
		//	{
		//		 
		//	}
		//
		//	// we have the pattern color, now we gotta compare it to the one from the image and act accordingly
		//	// how the fuck do we obtain the one from the image, though...
		//
		//	//wrongPixelsCountForPatternColor[13] += patternColorBitRepresentation;
		//
		//	//if (pos % 16 == 0) {
		//	//	uint64_t currentPatternQword = patternQwords[i + patternWidth * j];
		//	//	uint64_t qwordInImage = imageQwords[i + patternWidth * j];
		//	//	++(wrongPixelsCountForPatternColor[13]);
		//	//	++(wrongPixelsCountForPatternColor[11]);
		//	//	++(wrongPixelsCountForPatternColor[10]);
		//	//	++(wrongPixelsCountForPatternColor[9]);
		//	//
		//	//	qwordInPattern &= qwordInImage;
		//	//	wrongPixelsCountForPatternColor[9] &= qwordInPattern;
		//	//	//wrongPixelsCountForPatternColor[13] = wrongPixelsCountForPatternColor[13] + 1;
		//	//
		//	//	// after some thinking, the conclusion is:
		//	//	// it seems to be possible to speed up image pattern dettection by at least 8x
		//	//	// it demands, however, some crazy bit manipulations here, which are not trivial
		//	//	// so it is maybe not worth implementing at the moment (not as priority)
		//}
	}

	
	//
	//for (uint32_t z = 0; z < nOfQwordsToRead; ++z) {
	//
	//	uint64_t patternQword = patternQwords[z];
	//
	//	uint32_t i = z / patternWidth;
	//	uint32_t j = z % patternWidth;
	//
	//}


	// end of optimized implementation
	// 
	//return (wrongPixelsCountForPatternColor[13] + wrongPixelsCountForPatternColor[11] + wrongPixelsCountForPatternColor[10] + wrongPixelsCountForPatternColor[9]) % 3 == 0;
	// 
	return true;






	//const size_t totalNumberOfPixels = 25413;

	//uint32_t wrongPixelsCountForPatternColor[15];
	//uint32_t wrongPixelsCountForPatternColor[15];
	//uint32_t wrongPixelsCountForPatternColor1 = 0;
	//uint32_t wrongPixelsCountForPatternColor2 = 0;
	//uint32_t wrongPixelsCountForPatternColor3 = 0;
	//uint32_t wrongPixelsCountForPatternColor4 = 0;
	//uint32_t wrongPixelsCountForPatternColor5 = 0;
	//uint32_t wrongPixelsCountForPatternColor6 = 0;
	//uint32_t wrongPixelsCountForPatternColor7 = 0;
	//uint32_t wrongPixelsCountForPatternColor8 = 0;
	//uint32_t wrongPixelsCountForPatternColor9 = 0;
	//uint32_t wrongPixelsCountForPatternColor10 = 0;
	//uint32_t wrongPixelsCountForPatternColor11 = 0;
	//uint32_t wrongPixelsCountForPatternColor12 = 0;
	//uint32_t wrongPixelsCountForPatternColor13 = 0;
	//uint32_t wrongPixelsCountForPatternColor14 = 0;
	//uint32_t wrongPixelsCountForPatternColor15 = 0;

	////for (const PatternColor& patternColor : patternColors) {
	//	//const int maxWrongPixelsAllowed = floor((totalNumberOfPixels * (100.0 - patternColor.minMatchesPercentageToStillConsiderMatch)) / 100.0);
	//	const int maxWrongPixelsAllowed = 1000;
	//	int wrongPixels = 0;
	//	for (int i = 0; i <  / 16; ++i) {
	//		//for (int i = x; i < x + searchRegion.size.x; ++i) {
	//		//	for (int j = y; j < y + searchRegion.size.y; ++j) {
	//		//if (pattern->pixelAt(i, j) == patternColor->color) {
	//		uint64_t imageBits = ((uint64_t*)(imageOptimizationBuffer))[i];
	//		uint64_t patternBits = ((uint64_t*)(patternOptimizationBuffer))[i];
	//
	//		//uint64_t wrongPixelsCountForPatternColors1And2 = 0;
	//		//uint64_t wrongPixelsCountForPatternColors3And4 = 0;
	//		//uint64_t wrongPixelsCountForPatternColors5And6 = 0;
	//		//uint64_t wrongPixelsCountForPatternColors7And8 = 0;
	//
	//		uint32_t wrongPixelsCountForPatternColor1 = 0;
	//		uint32_t wrongPixelsCountForPatternColor2 = 0;
	//		uint32_t wrongPixelsCountForPatternColor3 = 0;
	//		uint32_t wrongPixelsCountForPatternColor4 = 0;
	//		uint32_t wrongPixelsCountForPatternColor5 = 0;
	//		uint32_t wrongPixelsCountForPatternColor6 = 0;
	//		uint32_t wrongPixelsCountForPatternColor7 = 0;
	//		uint32_t wrongPixelsCountForPatternColor8 = 0;
	//		uint32_t wrongPixelsCountForPatternColor9 = 0;
	//		uint32_t wrongPixelsCountForPatternColor10 = 0;
	//		uint32_t wrongPixelsCountForPatternColor11 = 0;
	//		uint32_t wrongPixelsCountForPatternColor12 = 0;
	//		uint32_t wrongPixelsCountForPatternColor13 = 0;
	//		uint32_t wrongPixelsCountForPatternColor14 = 0;
	//		uint32_t wrongPixelsCountForPatternColor15 = 0;
	//
	//		//uint64_t asd[100];
	//		//for (int i = 0; i < 100; ++i) {
	//		//	asd[i] = 0;
	//		//}
	//
	//		//uint64_t a = 0;
	//		//uint64_t b = 0;
	//		//uint64_t c = 0;
	//		//uint64_t d = 0;
	//		//uint64_t e = 0;
	//		//uint64_t f = 0;
	//		//uint64_t g = 0;
	//		//uint64_t h = 0;
	//		//uint64_t _i = 0;
	//		//uint64_t j = 0;
	//		//uint64_t k = 0;
	//
	//		//a += 1;
	//		//b += 1;
	//		//c += 1;
	//		//d += 1;
	//		//e += 1;
	//		//f += 1;
	//		//g += 1;
	//		//h += 1;
	//		//_i += 1;
	//		//j += 1;
	//		//k += 1;
	//
	//		//for (int i = 0; i < 10000; ++i) {
	//		//	k &= 0b10010101;
	//		//}
	//		//uint64_t wrongPixelsCountForPatternColors7And8 = 0;
	//
	//		//for (uint8_t i = 0; i < 15; ++i) {
	//		//	//wrongPixelsCountForPatternColor[i] &= imageBits;
	//		//	//wrongPixelsCountForPatternColor[i] ^= patternBits;
	//		//}
	//
	//		//wrongPixelsCountForPatternColor1 |= imageBits;
	//		//wrongPixelsCountForPatternColor1 ^= patternBits;
	//		//wrongPixelsCountForPatternColor2 |= imageBits;
	//		//wrongPixelsCountForPatternColor2 ^= patternBits;
	//		//wrongPixelsCountForPatternColor3 |= imageBits;
	//		//wrongPixelsCountForPatternColor3 ^= patternBits;
	//		//wrongPixelsCountForPatternColor4 |= imageBits;
	//		//wrongPixelsCountForPatternColor4 ^= patternBits;
	//		//wrongPixelsCountForPatternColor5 |= imageBits;
	//		//wrongPixelsCountForPatternColor5 ^= patternBits;
	//		//wrongPixelsCountForPatternColor6 |= imageBits;
	//		//wrongPixelsCountForPatternColor6 ^= patternBits;
	//		//wrongPixelsCountForPatternColor7 |= imageBits;
	//		//wrongPixelsCountForPatternColor7 ^= patternBits;
	//		//wrongPixelsCountForPatternColor8 |= imageBits;
	//		//wrongPixelsCountForPatternColor8 ^= patternBits;
	//		//wrongPixelsCountForPatternColor9 |= imageBits;
	//		//wrongPixelsCountForPatternColor9 ^= patternBits;
	//		//wrongPixelsCountForPatternColor10 |= imageBits;
	//		//wrongPixelsCountForPatternColor10 ^= patternBits;
	//		//wrongPixelsCountForPatternColor11 |= imageBits;
	//		//wrongPixelsCountForPatternColor11 ^= patternBits;
	//		//wrongPixelsCountForPatternColor12 |= imageBits;
	//		//wrongPixelsCountForPatternColor12 ^= patternBits;
	//		//wrongPixelsCountForPatternColor13 |= imageBits;
	//		//wrongPixelsCountForPatternColor13 ^= patternBits;
	//		//wrongPixelsCountForPatternColor14 |= imageBits;
	//		//wrongPixelsCountForPatternColor14 ^= patternBits;
	//		//wrongPixelsCountForPatternColor15 |= imageBits;
	//		//wrongPixelsCountForPatternColor15 ^= patternBits;
	//
	//		//patternOptimizationBuffer
	//		if (imageBits == patternBits) {
	//			++wrongPixels;
	//		}
	//		//if (pattern->pixelAt(i) == patternColor->color) {
	//			//const Pixel& approximatedPixel = imageApproximationBuffer->pixelAt(i);
	//			//if (approximatedPixel != patternColor->color) {
	//			//	++wrongPixels;
	//			//	
	//			//}
	//		}
	//		//}
	//	//}
	//
	//	//wrongPixelsCountForPatternColor1 &= wrongPixelsCountForPatternColor2;
	//	//wrongPixelsCountForPatternColor1 &= wrongPixelsCountForPatternColor3;
	//	//wrongPixelsCountForPatternColor1 &= wrongPixelsCountForPatternColor4;
	//	//wrongPixelsCountForPatternColor1 &= wrongPixelsCountForPatternColor5;
	//	//wrongPixelsCountForPatternColor1 &= wrongPixelsCountForPatternColor6;
	//	//wrongPixelsCountForPatternColor1 &= wrongPixelsCountForPatternColor7;
	//	//wrongPixelsCountForPatternColor1 &= wrongPixelsCountForPatternColor8;
	//	//wrongPixelsCountForPatternColor1 &= wrongPixelsCountForPatternColor9;
	//	//wrongPixelsCountForPatternColor1 &= wrongPixelsCountForPatternColor10;
	//	//wrongPixelsCountForPatternColor1 &= wrongPixelsCountForPatternColor11;
	//	//wrongPixelsCountForPatternColor1 &= wrongPixelsCountForPatternColor12;
	//	//wrongPixelsCountForPatternColor1 &= wrongPixelsCountForPatternColor13;
	//	//wrongPixelsCountForPatternColor1 &= wrongPixelsCountForPatternColor14;
	//	//wrongPixelsCountForPatternColor1 &= wrongPixelsCountForPatternColor15;
	//
	//	//if (wrongPixels > maxWrongPixelsAllowed == 2 && wrongPixelsCountForPatternColor1 == 1) {
	//	//	return false;
	//	//}
	//
	//	if (wrongPixels > maxWrongPixelsAllowed) {
	//		return false;
	//	}
	////}
	//return true;
}