#include "ImageOCRUtils.h"

#include <assert.h>

#include "Common/Image.h"
#include "ImagePreprocessing.h"
#include "RectangularRegion.h"

std::vector<ImageOCRUtils::TesseractEngineData*> ImageOCRUtils::tesseractEngineDatas;
std::mutex ImageOCRUtils::mutex;

void ImageOCRUtils::addInitialTesseractEngines()
{
	constexpr size_t initialNumberOfTesseractEngines = 16;
	ImageOCRUtils::addTesseractEngines(initialNumberOfTesseractEngines);
}

void ImageOCRUtils::addTesseractEngines(const size_t numberOfEnginesToAdd)
{
	for(size_t i = 0 ; i < numberOfEnginesToAdd ; ++i) {
		TesseractEngineData* tesseractEngineData = new TesseractEngineData();
		tesseractEngineDatas.push_back(tesseractEngineData);

		tesseractEngineData->tesseractEngine = std::make_unique<tesseract::TessBaseAPI>();
		const std::string tesseractDataPath = "C:\\devtools\\tessdata";
		const int engineInitializationResult = tesseractEngineData->tesseractEngine->Init(tesseractDataPath.c_str(), "eng");
		assert(engineInitializationResult == 0);
	}
}

void ImageOCRUtils::removeTesseractEngines()
{
	for (TesseractEngineData* tesseractEngineData : tesseractEngineDatas) {
		delete tesseractEngineData;
	}
	tesseractEngineDatas.clear();
}

tesseract::PageSegMode ImageOCRUtils::correspondentPageSegMode(const ImageOCR::PSMType psmType)
{
	switch (psmType) {
	case ImageOCR::PSMType::PsmOsdOnly:
		return tesseract::PageSegMode::PSM_OSD_ONLY;
		break;
	case ImageOCR::PSMType::PsmAutoOnly:
		return tesseract::PageSegMode::PSM_AUTO_ONLY;
		break;
	case ImageOCR::PSMType::PsmAuto:
		return tesseract::PageSegMode::PSM_AUTO;
		break;
	case ImageOCR::PSMType::PsmSingleColumn:
		return tesseract::PageSegMode::PSM_SINGLE_COLUMN;
		break;
	case ImageOCR::PSMType::PsmSingleBlockVertText:
		return tesseract::PageSegMode::PSM_SINGLE_BLOCK_VERT_TEXT;
		break;
	case ImageOCR::PSMType::PsmSingleBlock:
		return tesseract::PageSegMode::PSM_SINGLE_BLOCK;
		break;
	case ImageOCR::PSMType::PsmSingleLine:
		return tesseract::PageSegMode::PSM_SINGLE_LINE;
		break;
	case ImageOCR::PSMType::PsmSingleWord:
		return tesseract::PageSegMode::PSM_SINGLE_WORD;
		break;
	case ImageOCR::PSMType::PsmCircleWord:
		return tesseract::PageSegMode::PSM_CIRCLE_WORD;
		break;
	case ImageOCR::PSMType::PsmSingleChar:
		return tesseract::PageSegMode::PSM_SINGLE_CHAR;
		break;
	case ImageOCR::PSMType::PsmSparseText:
		return tesseract::PageSegMode::PSM_SPARSE_TEXT;
		break;
	case ImageOCR::PSMType::PsmSparseTextOcd:
		return tesseract::PageSegMode::PSM_SPARSE_TEXT_OSD;
		break;
	case ImageOCR::PSMType::PsmRawLine:
		return tesseract::PageSegMode::PSM_RAW_LINE;
		break;
	}
	return tesseract::PageSegMode::PSM_RAW_LINE;
}

void ImageOCRUtils::setImageOfTesseractEngine(tesseract::TessBaseAPI* tesseractEngine, Image* image)
{
	cv::Mat matInBgr;
	cv::cvtColor(*image->getMat(), matInBgr, cv::COLOR_BGRA2RGBA);
	tesseractEngine->SetImage(matInBgr.data, matInBgr.cols, matInBgr.rows, 4, matInBgr.step);
}

std::string ImageOCRUtils::performOCR(Image* image, const ImageOCR::PSMType psmType)
{
	acquireTesseractEngineForCurrentThread();

	tesseract::TessBaseAPI* tesseractEngine = getTesseractEngineForCurrentThread();

	tesseractEngine->SetPageSegMode(correspondentPageSegMode(psmType));

	setImageOfTesseractEngine(tesseractEngine, image);

	std::string ocrResult(tesseractEngine->GetUTF8Text());

	releaseTesseractEngineForCurrentThread();

	return ocrResult;
}

void ImageOCRUtils::acquireTesseractEngineForCurrentThread()
{
	mutex.lock();

	int index = -1;

	for (size_t i = 0; i < tesseractEngineDatas.size(); ++i) {
		TesseractEngineData* tesseractEngineData = tesseractEngineDatas[i];
		if (!tesseractEngineData->isInUse) {
			index = i;
			break;
		}
	}

	if (index < 0) {
		addTesseractEngines(1);
		index = tesseractEngineDatas.size() - 1;
	}

	tesseractEngineDatas[index]->isInUse = true;
	tesseractEngineDatas[index]->idOfThreadThatIsUsing = std::this_thread::get_id();

	mutex.unlock();
}

ImageOCRUtils::TesseractEngineData* ImageOCRUtils::getTesseractEngineDataForCurrentThread()
{
	mutex.lock();

	TesseractEngineData* tesseractEngineDataBeingUsed = nullptr;

	for (size_t i = 0; i < tesseractEngineDatas.size(); ++i) {
		TesseractEngineData* tesseractEngineData = tesseractEngineDatas[i];
		if (tesseractEngineData->isInUse && tesseractEngineData->idOfThreadThatIsUsing == std::this_thread::get_id()) {
			tesseractEngineDataBeingUsed = tesseractEngineData;
		}
	}

	assert(tesseractEngineDataBeingUsed != nullptr);

	mutex.unlock();

	return tesseractEngineDataBeingUsed;
}

tesseract::TessBaseAPI* ImageOCRUtils::getTesseractEngineForCurrentThread()
{
	TesseractEngineData* tesseractEngineData = getTesseractEngineDataForCurrentThread();
	assert(tesseractEngineData != nullptr);
	return tesseractEngineData->tesseractEngine.get();
}

void ImageOCRUtils::releaseTesseractEngineForCurrentThread()
{
	TesseractEngineData* tesseractEngineDataBeingUsed = getTesseractEngineDataForCurrentThread();

	assert(tesseractEngineDataBeingUsed != nullptr);

	mutex.lock();

	tesseractEngineDataBeingUsed->isInUse = false;

	mutex.unlock();
}