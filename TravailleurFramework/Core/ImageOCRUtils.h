#ifndef ImageOCRUtils_H
#define ImageOCRUtils_H

#include <memory>
#include <mutex>
#include <string>
#include <tesseract/baseapi.h>
#include <thread>
#include <vector>

#include "Common/Rectangle2D.h"
#include "ImageOCR.h"

class Image;
class ImagePreprocessing;

class ImageOCRUtils {
public:
	static std::string performOCR(Image* image, const ImageOCR::PSMType psmType);

	static void addInitialTesseractEngines();

	static void addTesseractEngines(const size_t numberOfEnginesToAdd);

	static void removeTesseractEngines();

private:
	struct TesseractEngineData {
		std::unique_ptr<tesseract::TessBaseAPI> tesseractEngine;
		bool isInUse = false;
		std::thread::id idOfThreadThatIsUsing;
	};

	static void acquireTesseractEngineForCurrentThread();
	static TesseractEngineData* getTesseractEngineDataForCurrentThread();
	static tesseract::TessBaseAPI* getTesseractEngineForCurrentThread();
	static void releaseTesseractEngineForCurrentThread();

	static tesseract::PageSegMode correspondentPageSegMode(const ImageOCR::PSMType psmType);

	static void setImageOfTesseractEngine(tesseract::TessBaseAPI* tesseractEngine, Image* image);

	static std::vector<TesseractEngineData*> tesseractEngineDatas;

	static std::mutex mutex;
};

#endif