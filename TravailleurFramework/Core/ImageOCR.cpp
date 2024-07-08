#include "ImageOCR.h"

#include "imgui.h"

#include <vector>

#include "Common/FilesystemUtils.h"
#include "Common/TravailleurUtils.h"
#include "Common/SimplePersistenceUtils.h"
#include "ImageOCRUtils.h"
#include "ImGuiWrappers/ImGuiTextOutput.h"
#include "TravailleurContext.h"

ImageOCR::ImageOCR(const std::string& name, TravailleurContext* travailleurContext) :
    name(name),
	travailleurContext(travailleurContext)
{
    assureSaveDirectoryExists();

	rectangularRegionsNamesList = std::make_unique<ImGuiSingleSelectionList>("rectangular-regions-names-image-ocr");

	usesImagePreprocessingCheckbox = std::make_unique<ImGuiCheckbox>("Uses image preprocessing", false, getSaveDirectory() + "\\uses-image-preprocessing");

	imagePreprocessingsNamesList = std::make_unique<ImGuiSingleSelectionList>("image-preprocessing-names-image-ocr");

	psmTypeCombo = std::make_unique<ImGuiCombo>("Psm type");
	initializePsmTypeCombo();

	load();
}

ImageOCR::~ImageOCR()
{
	save();
}

void ImageOCR::load()
{
	int psmTypeAsInt = 0;
	SimplePersistenceUtils::loadInt32(
		"psm-type",
		psmTypeAsInt,
		getSaveDirectory());
	psmTypeCombo->setSelectedIndex(psmTypeAsInt);
}

void ImageOCR::save() const
{
	const std::string nameOfSelectedRectangularRegion = rectangularRegionsNamesList->getSelectedOption();
	SimplePersistenceUtils::saveString(
		"name-of-selected-rectangular-region",
		nameOfSelectedRectangularRegion,
		getSaveDirectory());

	const std::string nameOfSelectedImagePreprocessing = imagePreprocessingsNamesList->getSelectedOption();
	SimplePersistenceUtils::saveString(
		"name-of-selected-image-preprocessing",
		nameOfSelectedImagePreprocessing,
		getSaveDirectory());

	const int psmTypeAsInt = (int)psmTypeCombo->getSelectedIndex();
	SimplePersistenceUtils::saveInt32(
		"psm-type",
		psmTypeAsInt,
		getSaveDirectory());
}

std::string ImageOCR::getSaveDirectory() const
{
    return TravailleurUtils::getImageOCRsPath(travailleurContext->getName()) + "\\" + name;
}

void ImageOCR::assureSaveDirectoryExists()
{
    FilesystemUtils::assureDirectoryExists(getSaveDirectory());
}

const std::string& ImageOCR::getName() const
{
	return name;
}

void ImageOCR::setName(const std::string& name)
{
	this->name = name;
}

void ImageOCR::showGui()
{
	ImGuiTextOutput(name);

	rectangularRegionsNamesList->showGui();

	usesImagePreprocessingCheckbox->showGui();
	if (usesImagePreprocessingCheckbox->getValue()) {
		imagePreprocessingsNamesList->showGui();
	}

	psmTypeCombo->showGui();
}

RectangularRegion* ImageOCR::getRectangularRegion() const
{
	return travailleurContext->getRectangularRegion(rectangularRegionsNamesList->getSelectedOption());
}

bool ImageOCR::usesImagePreprocessing() const
{
	return usesImagePreprocessingCheckbox->getValue();
}

ImagePreprocessing* ImageOCR::getImagePreprocessing() const
{
	if (usesImagePreprocessing()) {
		return travailleurContext->getImagePreprocessing(imagePreprocessingsNamesList->getSelectedOption());
	}
	return nullptr;
}

void ImageOCR::informRectangularRegionsNames(const std::vector<std::string>& rectangularRegionsNames)
{
	rectangularRegionsNamesList->setOptions(rectangularRegionsNames);
	std::string nameOfSelectedRectangularRegion;
	SimplePersistenceUtils::loadString(
		"name-of-selected-rectangular-region",
		nameOfSelectedRectangularRegion,
		getSaveDirectory());
	rectangularRegionsNamesList->setSelectedOption(nameOfSelectedRectangularRegion);
}

void ImageOCR::informImagePreprocessingsNames(const std::vector<std::string>& imagePreprocessingsNames)
{
	imagePreprocessingsNamesList->setOptions(imagePreprocessingsNames);
	std::string nameOfSelectedImagePreprocessing;
	SimplePersistenceUtils::loadString(
		"name-of-selected-image-preprocessing",
		nameOfSelectedImagePreprocessing,
		getSaveDirectory());
	imagePreprocessingsNamesList->setSelectedOption(nameOfSelectedImagePreprocessing);
}

void ImageOCR::initializePsmTypeCombo()
{
	std::vector<std::string> options;
	options.push_back("PsmOsdOnly");
	options.push_back("PsmAutoOsd");
	options.push_back("PsmAutoOnly");
	options.push_back("PsmAuto");
	options.push_back("PsmSingleColumn");
	options.push_back("PsmSingleBlockVertText");
	options.push_back("PsmSingleBlock");
	options.push_back("PsmSingleLine");
	options.push_back("PsmSingleWord");
	options.push_back("PsmCircleWord");
	options.push_back("PsmSingleChar");
	options.push_back("PsmSparseText");
	options.push_back("PsmSparseTextOcd");
	options.push_back("PsmRawLine");
	psmTypeCombo->setOptions(options);
}

std::string ImageOCR::performOCROnRegion(Image* frame, const Rectangle2D<int>& region)
{
	if (!region.isValid() || !region.isInsideOther(frame->getRegion())) return "";

	Image inputImage(*frame);
	if (!inputImage.crop(region)) {
		return "";
	}

	ImagePreprocessing* imagePreprocessing = getImagePreprocessing();
	if (imagePreprocessing != nullptr) {
		imagePreprocessing->apply(&inputImage);
	}

	PSMType psmType = (PSMType)psmTypeCombo->getSelectedIndex();

	return ImageOCRUtils::performOCR(&inputImage, psmType);
}