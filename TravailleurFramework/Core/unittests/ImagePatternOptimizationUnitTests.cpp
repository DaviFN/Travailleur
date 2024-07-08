#include "gtest/gtest.h"

#include <cstdio>

#include "Core/ImagePattern.h"

void initBufferAccordingToBufferDimension_2(void** buffer)
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

uint8_t getRandomNumberBetween0And15()
{
    return rand() % 16;
}

void fillOptimizationBuffer(void* buffer, const std::vector<uint8_t>& vector)
{
    const size_t nPixels = vector.size();
    for (size_t i = 0; 2 * i < nPixels; ++i) {
        const size_t approximatedColorIndex = 2 * i;
        uint8_t representation = 0;
        if (2 * i + 1 < nPixels) {
            representation = vector[approximatedColorIndex + 1];
        }



        representation = representation << 4;



        const uint8_t last4Bits = vector[approximatedColorIndex];
        representation |= last4Bits;

        ((uint8_t*)buffer)[i] = representation;
    }

    //for (int i = 0; i < vector.size(); ++i) {
    //    printf("%d ", vector[i]);
    //}
    // 9 3 14 4 1 12 6 14 2 0 9 1 1 11 9 11 3 6 11 12 7 12 14 9 4 14 13 12 6 7 7 14 3 2 13 8 3 11 11 6 15 3 10 13 9 8 5 15 13 4 11 12 6 5 5 11 3 13 9 10 12 11 14 2
}

void fillWithRandomPixelRepresentations(std::vector<uint8_t>& vector, size_t howMany)
{
    vector.clear();
    for (size_t i = 0; i < howMany; ++i) {
        vector.push_back(getRandomNumberBetween0And15());
    }
}

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 

TEST(ImagePatternOptimizationUnitTests, patternPixelsAccessUnitTest)
{

    //srand(time(NULL));

    // tests pass
    //const size_t imageWidth = 20;
    //const size_t imageHeight = 20;
    //
    //const size_t patternWidth = 8;
    //const size_t patternHeight = 8;

    const size_t imageWidth = 21;
    const size_t imageHeight = 21;

    //const size_t patternWidth = 21;
    //const size_t patternHeight = 21;
    
    const size_t patternWidth = 17;
    const size_t patternHeight = 17;

    std::vector<uint8_t> imagePixelRepresentations;
    std::vector<uint8_t> patternPixelRepresentations;
    fillWithRandomPixelRepresentations(patternPixelRepresentations, patternWidth * patternHeight);
    fillWithRandomPixelRepresentations(imagePixelRepresentations, imageWidth * imageHeight);
    

    void* imageOptimizationBuffer = nullptr;
    void* patternOptimizationBuffer = nullptr;
    initBufferAccordingToBufferDimension_2(&patternOptimizationBuffer);
    initBufferAccordingToBufferDimension_2(&imageOptimizationBuffer);

    fillOptimizationBuffer(patternOptimizationBuffer, patternPixelRepresentations);
    fillOptimizationBuffer(imageOptimizationBuffer, imagePixelRepresentations);






    uint64_t* patternQwords = (uint64_t*)patternOptimizationBuffer;
    uint64_t currentPatternQword = 0;

    int x = 0;
    int y = 0;

    printf("imagePixelRepresentations:");
    for (uint32_t i = 0; i < imageWidth * imageHeight; ++i) {
        printf("#%d ", i);
        printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(imagePixelRepresentations[i]));
        printf(" ");
    }
    printf("\n\n");
    
    printf("imageOptimizationBuffer:");
    for (uint32_t i = 0; i < (imageWidth * imageHeight) / 2; ++i) {
        printf("#%d ", i);
        uint8_t byte = ((uint8_t*)imageOptimizationBuffer)[i];
        printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(byte));
        //printf("%x", ((uint8_t*)imageOptimizationBuffer)[i]);
        printf(" ");
    }
    printf("\n\n");

	for (uint32_t j = 0; j < patternHeight; ++j) {

		uint32_t T = (x)+(y + j) * imageWidth;

		// two cases: T is even or T is odd
		// if T is even, the row in the image (which is, in fact, the sub image that represents the search region)
		// is of type R
		// else, is of type S
		// in type R, the first 4 bits of the first qword represent the first color in the subimage
		// in type S, the first 4 bits are to be ignored, and the next 4 bits represent the first color

		// T / 2 specifies the first byte of the first qword to be read
		//uint64_t* currentSearchRegionQwordAddress = (uint64_t*)((uint8_t*)imageOptimizationBuffer + T / 2);

        uint64_t* currentSearchRegionQwordAddress = (uint64_t*)((uint8_t*)imageOptimizationBuffer + T / 2);

		if (T % 2 == 0) {
        //if(true) {
			// type R

			// we just read just like we read the pattern colors

			for (uint32_t i = 0; i < patternWidth; ++i) {

				// we possibly update the current search region qword:
				//if (i != 0 && i % 16 == 0) {
				//	++currentSearchRegionQwordAddress;
				//}

                if (i != 0 && i % 16 == 0) {
                    ++currentSearchRegionQwordAddress;
                }

				uint32_t pos = i + patternWidth * j;

				uint8_t patternColorPositionInPatternQword = (pos % 16) * 4;

				if (patternColorPositionInPatternQword == 0) {
					currentPatternQword = patternQwords[pos / 16];
				}

				// [bbbb bbbb] [bbbb bbbb] [bbbb bbbb] [bbbb bbbb] [bbbb bbbb] [bbbb bbbb] [bbbb bbbb] [bbbb bbbb]

				//uint8_t patternColorBitRepresentation = (currentPatternQword >> (60 - patternColorPositionInPatternQword)) & 0b00001111;
                uint8_t patternColorBitRepresentation = (currentPatternQword >> (patternColorPositionInPatternQword)) & 0b00001111;
                //uint8_t actualPatternColorBitRepresentation = patternPixelRepresentations[pos];
                //EXPECT_EQ(patternColorBitRepresentation, actualPatternColorBitRepresentation);

                //uint32_t howMuchToShift = T % 2 ? (i % imageWidth) * 4 : (i % imageWidth) * 4 + 4;

                uint32_t howMuchToShift = (i % imageWidth) * 4;

                uint8_t imageColorBitRepresentation = (*currentSearchRegionQwordAddress >> (howMuchToShift)) & 0b00001111;

                uint32_t actualImagePixelsIndexBeingAccessed = (x + i) + (j + y) * imageWidth;
                //printf("actualImagePixelsIndexBeingAccessed: %d\n", actualImagePixelsIndexBeingAccessed);

                uint8_t actualImageColorBitRepresentation = imagePixelRepresentations[(x + i) + (j + y) * imageWidth];

                static int ll = 0;
                ++ll;

                //printf("howMuchToShift: %d\n", howMuchToShift);
                //printf("%d - (i,j) = (%d, %d) - %x, %x - %llx\n", ll, i, j, imageColorBitRepresentation, actualImageColorBitRepresentation, *currentSearchRegionQwordAddress);
                //printf("got: ");
                //printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(imageColorBitRepresentation));
                //printf(" actual : ");
                //printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(actualImageColorBitRepresentation));
                //printf("\n");

                EXPECT_EQ(imageColorBitRepresentation, actualImageColorBitRepresentation);


                //uint8_t imageColorBitRepresentation = (currentImageQword >> (patternColorPositionInImageQword)) & 0b00001111;
                //uint8_t actualImageColorBitRepresentation = imagePixelRepresentations[pos];
                //EXPECT_EQ(patternColorBitRepresentation, actualPatternColorBitRepresentation);



				//if (patternColorBitRepresentation != 0) // 0 is the transparent pixel, which indicates that the approximation did not yield any of the pattern colors
				if(true)
                {

                    ////uint8_t actualImageColorBitRepresentation = patternPixelRepresentations[pos];
					//uint8_t imageColorBitRepresentation = (*currentSearchRegionQwordAddress >> (60 - patternColorPositionInPatternQword)) & 0b00001111;

                    //EXPECT_EQ(imageColorBitRepresentation, actualImageColorBitRepresentation);
                    
                    //if (patternColorBitRepresentation != imageColorBitRepresentation) {
					//	++wrongPixelsCountForPatternColor[patternColorBitRepresentation - 1];
					//	if (wrongPixelsCountForPatternColor[patternColorBitRepresentation - 1] > maxWrongPixelsAllowedOptimizationBuffer[patternColorBitRepresentation - 1]) {
					//		return false;
					//	}
					//}

				}

			}

		}
		else {
			// type S

            for (uint32_t i = 0; i < patternWidth; ++i) {

                // we possibly update the current search region qword:
                //if (i != 0 && i % 16 == 0) {
                //	++currentSearchRegionQwordAddress;
                //}

                if (i != 0 && (i + 1) % 16 == 0) {
                    ++currentSearchRegionQwordAddress;
                }

                uint32_t pos = i + patternWidth * j;

                uint8_t patternColorPositionInPatternQword = (pos % 16) * 4;

                if (patternColorPositionInPatternQword == 0) {
                    currentPatternQword = patternQwords[pos / 16];
                }

                // [bbbb bbbb] [bbbb bbbb] [bbbb bbbb] [bbbb bbbb] [bbbb bbbb] [bbbb bbbb] [bbbb bbbb] [bbbb bbbb]

                //uint8_t patternColorBitRepresentation = (currentPatternQword >> (60 - patternColorPositionInPatternQword)) & 0b00001111;
                uint8_t patternColorBitRepresentation = (currentPatternQword >> (patternColorPositionInPatternQword)) & 0b00001111;
                //uint8_t actualPatternColorBitRepresentation = patternPixelRepresentations[pos];
                //EXPECT_EQ(patternColorBitRepresentation, actualPatternColorBitRepresentation);

                //uint32_t howMuchToShift = T % 2 ? (i % imageWidth) * 4 : (i % imageWidth) * 4 + 4;

                uint32_t howMuchToShift = (i % imageWidth) * 4 + 4;

                uint8_t imageColorBitRepresentation = (*currentSearchRegionQwordAddress >> (howMuchToShift)) & 0b00001111;

                uint32_t actualImagePixelsIndexBeingAccessed = (x + i) + (j + y) * imageWidth;
                //printf("actualImagePixelsIndexBeingAccessed: %d\n", actualImagePixelsIndexBeingAccessed);

                uint8_t actualImageColorBitRepresentation = imagePixelRepresentations[(x + i) + (j + y) * imageWidth];

                static int ll = 0;
                ++ll;

                printf("howMuchToShift: %d\n", howMuchToShift);
                printf("%d - (i,j) = (%d, %d) - %x, %x - %llx\n", ll, i, j, imageColorBitRepresentation, actualImageColorBitRepresentation, *currentSearchRegionQwordAddress);
                printf("got: ");
                printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(imageColorBitRepresentation));
                printf(" actual : ");
                printf(BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(actualImageColorBitRepresentation));
                printf("\n");

                EXPECT_EQ(imageColorBitRepresentation, actualImageColorBitRepresentation);


                //uint8_t imageColorBitRepresentation = (currentImageQword >> (patternColorPositionInImageQword)) & 0b00001111;
                //uint8_t actualImageColorBitRepresentation = imagePixelRepresentations[pos];
                //EXPECT_EQ(patternColorBitRepresentation, actualPatternColorBitRepresentation);



                //if (patternColorBitRepresentation != 0) // 0 is the transparent pixel, which indicates that the approximation did not yield any of the pattern colors
                if (true)
                {

                    ////uint8_t actualImageColorBitRepresentation = patternPixelRepresentations[pos];
                    //uint8_t imageColorBitRepresentation = (*currentSearchRegionQwordAddress >> (60 - patternColorPositionInPatternQword)) & 0b00001111;

                    //EXPECT_EQ(imageColorBitRepresentation, actualImageColorBitRepresentation);

                    //if (patternColorBitRepresentation != imageColorBitRepresentation) {
                    //	++wrongPixelsCountForPatternColor[patternColorBitRepresentation - 1];
                    //	if (wrongPixelsCountForPatternColor[patternColorBitRepresentation - 1] > maxWrongPixelsAllowedOptimizationBuffer[patternColorBitRepresentation - 1]) {
                    //		return false;
                    //	}
                    //}

                }

            }


		}
	}






    free(imageOptimizationBuffer);
    free(patternOptimizationBuffer);

    EXPECT_TRUE(true);
}