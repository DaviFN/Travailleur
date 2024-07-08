#include "ImagePreprocessingStep.h"

#include "imgui.h"

#include "ImGuiWrappers/ImGuiSimpleColorSelector.h"
#include "ImGuiWrappers/ImGuiHSVRangeInput.h"
#include "ImGuiWrappers/ImGuiSimpleDoubleSlider.h"
#include "ImGuiWrappers/ImGuiSimpleIntSlider.h"
#include "ImGuiWrappers/ImGuiTextOutput.h"

ImagePreprocessingStep::ImagePreprocessingStep()
{
	stepTypeCombo = std::make_unique<ImGuiCombo>("Step type");
	initializeStepTypeCombo();

	active = std::make_unique<ImGuiCheckbox>("Active", true);

	thresholdTypeCombo = std::make_unique<ImGuiCombo>("Threshold type");
	initializeThresholdTypeCombo();
	thresholdAdditionalFlagsCombo = std::make_unique<ImGuiCombo>("Threshold additional flags");
	initializeThresholdAdditionalFlagsCombo();

	adaptiveThresholdTypeCombo = std::make_unique<ImGuiCombo>("Adaptive threshold type");
	initializeAdaptiveThresholdTypeCombo();
	adaptiveThresholdThresholdTypeCombo = std::make_unique<ImGuiCombo>("Adaptive threshold's threshold type");
	initializeAdaptiveThresholdThresholdTypeCombo();

	inRangeHSV_invertResult = std::make_unique<ImGuiCheckbox>("Invert result of InRangeHSV", false);

	replaceColorInRangeHSV_invertResult = std::make_unique<ImGuiCheckbox>("Invert result of ReplaceColorInRangeHSV", false);
	
	erosionMorphShapeCombo = std::make_unique<ImGuiCombo>("Erosion morph shape");
	initializeErosionMorphShapeCombo();

	dilationMorphShapeCombo = std::make_unique<ImGuiCombo>("Dilation morph shape");
	initializeDilationMorphShapeCombo();

	normalizationNormTypeCombo = std::make_unique<ImGuiCombo>("Normalization norm type");
	initializenormalizationNormTypeCombo();
}

ImagePreprocessingStep::~ImagePreprocessingStep()
{
}

void ImagePreprocessingStep::save(FILE* f)
{
	const int stepTypeAsInteger = (int)getType();
	fwrite(&stepTypeAsInteger, sizeof(int), 1, f);

	const bool activeValue = active->getValue();
	fwrite(&activeValue, sizeof(bool), 1, f);

	// GaussianBlur
	fwrite(&gaussianKernelParamX, sizeof(int), 1, f);
	fwrite(&gaussianKernelParamY, sizeof(int), 1, f);

	// Scale
	fwrite(&scaleFactorX, sizeof(double), 1, f);
	fwrite(&scaleFactorY, sizeof(double), 1, f);

	// Resize
	fwrite(&newSizeX, sizeof(int), 1, f);
	fwrite(&newSizeY, sizeof(int), 1, f);

	// Threshold
	const int thresholdTypeAsInteger = (int)getThresholdType();
	fwrite(&thresholdTypeAsInteger, sizeof(int), 1, f);
	fwrite(&threshold_thresh, sizeof(int), 1, f);
	fwrite(&threshold_maxval, sizeof(int), 1, f);
	const int thresholdAdditionalFlagsAsInteger = (int)getThresholdAdditionalFlags();
	fwrite(&thresholdAdditionalFlagsAsInteger, sizeof(int), 1, f);

	// Adaptive threshold
	const int adaptiveThresholdTypeAsInteger = (int)getAdaptiveThresholdType();
	fwrite(&adaptiveThresholdTypeAsInteger, sizeof(int), 1, f);
	fwrite(&adaptive_threshold_maxval, sizeof(int), 1, f);
	const int adaptiveThresholdThresholdTypeAsInteger = (int)getAdaptiveThresholdThresholdType();
	fwrite(&adaptiveThresholdThresholdTypeAsInteger, sizeof(int), 1, f);
	fwrite(&adaptive_threshold_blocksize_param, sizeof(int), 1, f);
	fwrite(&adaptive_threshold_c, sizeof(double), 1, f);

	// Denoise
	fwrite(&denoise_h, sizeof(double), 1, f);
	fwrite(&denoise_templateWindowParam, sizeof(int), 1, f);
	fwrite(&denoise_searchWindowParam, sizeof(int), 1, f);

	// DenoiseColored
	fwrite(&denoise_colored_h, sizeof(double), 1, f);
	fwrite(&denoise_colored_hColor, sizeof(double), 1, f);
	fwrite(&denoise_colored_templateWindowParam, sizeof(int), 1, f);
	fwrite(&denoise_colored_searchWindowParam, sizeof(int), 1, f);

	// InRangeHSV
	fwrite(&inRangeHSV_h_lower, sizeof(double), 1, f);
	fwrite(&inRangeHSV_s_lower, sizeof(double), 1, f);
	fwrite(&inRangeHSV_v_lower, sizeof(double), 1, f);
	fwrite(&inRangeHSV_h_upper, sizeof(double), 1, f);
	fwrite(&inRangeHSV_s_upper, sizeof(double), 1, f);
	fwrite(&inRangeHSV_v_upper, sizeof(double), 1, f);
	const bool inRangeHSV_invertResultValue = inRangeHSV_invertResult->getValue();
	fwrite(&inRangeHSV_invertResultValue, sizeof(bool), 1, f);

	// ReplaceColorInRangeHSV
	fwrite(&replaceColorInRangeHSV_h_lower, sizeof(double), 1, f);
	fwrite(&replaceColorInRangeHSV_s_lower, sizeof(double), 1, f);
	fwrite(&replaceColorInRangeHSV_v_lower, sizeof(double), 1, f);
	fwrite(&replaceColorInRangeHSV_h_upper, sizeof(double), 1, f);
	fwrite(&replaceColorInRangeHSV_s_upper, sizeof(double), 1, f);
	fwrite(&replaceColorInRangeHSV_v_upper, sizeof(double), 1, f);
	const bool replaceColorInRangeHSV_invertResultValue = replaceColorInRangeHSV_invertResult->getValue();
	fwrite(&replaceColorInRangeHSV_invertResultValue, sizeof(bool), 1, f);
	replaceColorInRangeHSV_color.saveToBinaryFile(f);

	// Erode
	const int erosionMorphShapeTypeAsInteger = (int)getErosionMorphShapeType();
	fwrite(&erosionMorphShapeTypeAsInteger, sizeof(int), 1, f);
	fwrite(&erosionSizeX, sizeof(int), 1, f);
	fwrite(&erosionSizeY, sizeof(int), 1, f);

	// Dilate
	const int dilationMorphShapeTypeAsInteger = (int)getDilationMorphShapeType();
	fwrite(&dilationMorphShapeTypeAsInteger, sizeof(int), 1, f);
	fwrite(&dilationSizeX, sizeof(int), 1, f);
	fwrite(&dilationSizeY, sizeof(int), 1, f);

	// Normalize
	const int normalizationNormTypeComboAsInteger = (int)getNormalizationNormType();
	fwrite(&normalizationNormTypeComboAsInteger, sizeof(int), 1, f);
	fwrite(&normalizationAlpha, sizeof(double), 1, f);
	fwrite(&normalizationBeta, sizeof(double), 1, f);

	// CannyEdgeDetection
	fwrite(&canny_threshold1, sizeof(double), 1, f);
	fwrite(&canny_threshold2, sizeof(double), 1, f);
	fwrite(&canny_apertureSize, sizeof(int), 1, f);
}

void ImagePreprocessingStep::load(FILE* f)
{
	int stepTypeComboAsInteger = 0;
	fread(&stepTypeComboAsInteger, sizeof(int), 1, f);
	stepTypeCombo->setSelectedIndex(stepTypeComboAsInteger);

	bool activeValue = false;
	fread(&activeValue, sizeof(bool), 1, f);
	active->setValue(activeValue);

	// GaussianBlur
	fread(&gaussianKernelParamX, sizeof(int), 1, f);
	fread(&gaussianKernelParamY, sizeof(int), 1, f);

	// Scale
	fread(&scaleFactorX, sizeof(double), 1, f);
	fread(&scaleFactorY, sizeof(double), 1, f);

	// Resize
	fread(&newSizeX, sizeof(int), 1, f);
	fread(&newSizeY, sizeof(int), 1, f);

	// Threshold
	int thresholdTypeComboAsInteger = 0;
	fread(&thresholdTypeComboAsInteger, sizeof(int), 1, f);
	thresholdTypeCombo->setSelectedIndex(thresholdTypeComboAsInteger);
	fread(&threshold_thresh, sizeof(int), 1, f);
	fread(&threshold_maxval, sizeof(int), 1, f);
	int thresholdAdditionalFlagsComboAsInteger = 0;
	fread(&thresholdAdditionalFlagsComboAsInteger, sizeof(int), 1, f);
	thresholdAdditionalFlagsCombo->setSelectedIndex(thresholdAdditionalFlagsComboAsInteger);

	// Adaptive threshold
	int adaptiveThresholdTypeComboAsInteger = 0;
	fread(&adaptiveThresholdTypeComboAsInteger, sizeof(int), 1, f);
	adaptiveThresholdTypeCombo->setSelectedIndex(adaptiveThresholdTypeComboAsInteger);
	fread(&adaptive_threshold_maxval, sizeof(int), 1, f);
	int adaptiveThresholdThresholdTypeComboAsInteger = 0;
	fread(&adaptiveThresholdThresholdTypeComboAsInteger, sizeof(int), 1, f);
	adaptiveThresholdThresholdTypeCombo->setSelectedIndex(adaptiveThresholdThresholdTypeComboAsInteger);
	fread(&adaptive_threshold_blocksize_param, sizeof(int), 1, f);
	fread(&adaptive_threshold_c, sizeof(double), 1, f);

	// Denoise
	fread(&denoise_h, sizeof(double), 1, f);
	fread(&denoise_templateWindowParam, sizeof(int), 1, f);
	fread(&denoise_searchWindowParam, sizeof(int), 1, f);

	// DenoiseColored
	fread(&denoise_colored_h, sizeof(double), 1, f);
	fread(&denoise_colored_hColor, sizeof(double), 1, f);
	fread(&denoise_colored_templateWindowParam, sizeof(int), 1, f);
	fread(&denoise_colored_searchWindowParam, sizeof(int), 1, f);

	// InRangeHSV
	fread(&inRangeHSV_h_lower, sizeof(double), 1, f);
	fread(&inRangeHSV_s_lower, sizeof(double), 1, f);
	fread(&inRangeHSV_v_lower, sizeof(double), 1, f);
	fread(&inRangeHSV_h_upper, sizeof(double), 1, f);
	fread(&inRangeHSV_s_upper, sizeof(double), 1, f);
	fread(&inRangeHSV_v_upper, sizeof(double), 1, f);
	bool inRangeHSV_invertResultValue = false;
	fread(&inRangeHSV_invertResultValue, sizeof(bool), 1, f);
	inRangeHSV_invertResult->setValue(inRangeHSV_invertResultValue);

	// ReplaceColorInRangeHSV
	fread(&replaceColorInRangeHSV_h_lower, sizeof(double), 1, f);
	fread(&replaceColorInRangeHSV_s_lower, sizeof(double), 1, f);
	fread(&replaceColorInRangeHSV_v_lower, sizeof(double), 1, f);
	fread(&replaceColorInRangeHSV_h_upper, sizeof(double), 1, f);
	fread(&replaceColorInRangeHSV_s_upper, sizeof(double), 1, f);
	fread(&replaceColorInRangeHSV_v_upper, sizeof(double), 1, f);
	bool replaceColorInRangeHSV_invertResultValue = false;
	fread(&replaceColorInRangeHSV_invertResultValue, sizeof(bool), 1, f);
	replaceColorInRangeHSV_invertResult->setValue(replaceColorInRangeHSV_invertResultValue);
	replaceColorInRangeHSV_color.loadFromBinaryFile(f);

	// Erode
	int erosionMorphShapeTypeComboAsInteger = 0;
	fread(&erosionMorphShapeTypeComboAsInteger, sizeof(int), 1, f);
	erosionMorphShapeCombo->setSelectedIndex(erosionMorphShapeTypeComboAsInteger);
	fread(&erosionSizeX, sizeof(int), 1, f);
	fread(&erosionSizeY, sizeof(int), 1, f);

	// Dilate
	int dilationMorphShapeTypeComboAsInteger = 0;
	fread(&dilationMorphShapeTypeComboAsInteger, sizeof(int), 1, f);
	dilationMorphShapeCombo->setSelectedIndex(dilationMorphShapeTypeComboAsInteger);
	fread(&dilationSizeX, sizeof(int), 1, f);
	fread(&dilationSizeY, sizeof(int), 1, f);

	// Normalize
	int normalizationNormTypeComboAsInteger = 0;
	fread(&dilationMorphShapeTypeComboAsInteger, sizeof(int), 1, f);
	normalizationNormTypeCombo->setSelectedIndex(dilationMorphShapeTypeComboAsInteger);
	fread(&normalizationAlpha, sizeof(double), 1, f);
	fread(&normalizationBeta, sizeof(double), 1, f);

	// CannyEdgeDetection
	fread(&canny_threshold1, sizeof(double), 1, f);
	fread(&canny_threshold2, sizeof(double), 1, f);
	fread(&canny_apertureSize, sizeof(int), 1, f);
}

void ImagePreprocessingStep::initializeStepTypeCombo()
{
	std::vector<std::string> options;
	options.push_back("DoNothing");
	options.push_back("Invert");
	options.push_back("GaussianBlur");
	options.push_back("Scale");
	options.push_back("Resize");
	options.push_back("Threshold");
	options.push_back("AdaptiveThreshold");
	options.push_back("Denoise");
	options.push_back("DenoiseColored");
	options.push_back("CvtToGray");
	options.push_back("InRangeHSV");
	options.push_back("ReplaceColorInRangeHSV");
	options.push_back("Erode");
	options.push_back("Dilate");
	options.push_back("Normalize");
	options.push_back("CannyEdgeDetection");
	stepTypeCombo->setOptions(options);
}

void ImagePreprocessingStep::initializeThresholdTypeCombo()
{
	std::vector<std::string> options;
	options.push_back("ThreshBinary");
	options.push_back("ThreshBinaryInv");
	options.push_back("ThreshTrunc");
	options.push_back("ThreshToZero");
	options.push_back("ThreshToZeroInv");
	thresholdTypeCombo->setOptions(options);
}

void ImagePreprocessingStep::initializeThresholdAdditionalFlagsCombo()
{
	std::vector<std::string> options;
	options.push_back("None");
	options.push_back("ThreshOtsu");
	options.push_back("ThreshTriangle");
	thresholdAdditionalFlagsCombo->setOptions(options);
}

void ImagePreprocessingStep::initializeAdaptiveThresholdTypeCombo()
{
	std::vector<std::string> options;
	options.push_back("AdaptiveThreshMeanC");
	options.push_back("AdaptiveThreshGaussianC");
	adaptiveThresholdTypeCombo->setOptions(options);
}

void ImagePreprocessingStep::initializeAdaptiveThresholdThresholdTypeCombo()
{
	std::vector<std::string> options;
	options.push_back("ThreshBinary");
	options.push_back("ThreshBinaryInv");
	adaptiveThresholdThresholdTypeCombo->setOptions(options);
}

void ImagePreprocessingStep::initializeStructuringElementTypeCombo(ImGuiCombo& combo)
{
	std::vector<std::string> options;
	options.push_back("MorphCross");
	options.push_back("MorphRect");
	options.push_back("MorphEllipse");
	combo.setOptions(options);
}

void ImagePreprocessingStep::initializeErosionMorphShapeCombo()
{
	initializeStructuringElementTypeCombo(*erosionMorphShapeCombo);
}

void ImagePreprocessingStep::initializeDilationMorphShapeCombo()
{
	initializeStructuringElementTypeCombo(*dilationMorphShapeCombo);
}

void ImagePreprocessingStep::initializenormalizationNormTypeCombo()
{
	std::vector<std::string> options;
	options.push_back("NormMinMax");
	options.push_back("NormInf");
	options.push_back("NormL1");
	options.push_back("NormL2");
	normalizationNormTypeCombo->setOptions(options);
}

ImagePreprocessingStep::StepType ImagePreprocessingStep::getType() const
{
	return (ImagePreprocessingStep::StepType)stepTypeCombo->getSelectedIndex();
}

bool ImagePreprocessingStep::isActive() const
{
	return active->getValue();
}

ImagePreprocessingStep::ThresholdType ImagePreprocessingStep::getThresholdType() const
{
	return (ImagePreprocessingStep::ThresholdType)thresholdTypeCombo->getSelectedIndex();
}

ImagePreprocessingStep::ThresholdAdditionalFlags ImagePreprocessingStep::getThresholdAdditionalFlags() const
{
	return (ImagePreprocessingStep::ThresholdAdditionalFlags)thresholdAdditionalFlagsCombo->getSelectedIndex();
}

ImagePreprocessingStep::AdaptiveThresholdType ImagePreprocessingStep::getAdaptiveThresholdType() const
{
	return (ImagePreprocessingStep::AdaptiveThresholdType)adaptiveThresholdTypeCombo->getSelectedIndex();
}

ImagePreprocessingStep::AdaptiveThresholdThresholdType ImagePreprocessingStep::getAdaptiveThresholdThresholdType() const
{
	return (ImagePreprocessingStep::AdaptiveThresholdThresholdType)adaptiveThresholdThresholdTypeCombo->getSelectedIndex();
}

ImagePreprocessingStep::MorphShapeType ImagePreprocessingStep::getErosionMorphShapeType() const
{
	return (ImagePreprocessingStep::MorphShapeType)erosionMorphShapeCombo->getSelectedIndex();
}

ImagePreprocessingStep::MorphShapeType ImagePreprocessingStep::getDilationMorphShapeType() const
{
	return (ImagePreprocessingStep::MorphShapeType)dilationMorphShapeCombo->getSelectedIndex();
}

ImagePreprocessingStep::NormalizationNormType ImagePreprocessingStep::getNormalizationNormType() const
{
	return (ImagePreprocessingStep::NormalizationNormType)normalizationNormTypeCombo->getSelectedIndex();
}

void ImagePreprocessingStep::showGui()
{
	stepTypeCombo->showGui();

	if (getType() != StepType::DoNothing) {
		active->showGui();
	}

	// GaussianBlur
	if (getType() == StepType::GaussianBlur) {
		ImGuiSimpleIntSlider("gaussianKernelParamX", &gaussianKernelParamX, 1.0, 0, 1000);
		ImGuiSimpleIntSlider("gaussianKernelParamY", &gaussianKernelParamY, 1.0, 0, 1000);
	}
	else if (getType() == StepType::Scale) {
		ImGuiSimpleDoubleSlider("scaleFactorX", &scaleFactorX, 0.01, 0, 20);
		ImGuiSimpleDoubleSlider("scaleFactorY", &scaleFactorY, 0.01, 0, 20);
	}
	else if (getType() == StepType::Resize) {
		ImGuiSimpleIntSlider("newSizeX", &newSizeX, 1.0, 1, 4000);
		ImGuiSimpleIntSlider("newSizeY", &newSizeY, 1.0, 1, 4000);
	}
	else if (getType() == StepType::Threshold) {
		thresholdTypeCombo->showGui();
		ImGuiSimpleIntSlider("threshold_thresh", &threshold_thresh, 1.0, 0, 255);
		ImGuiSimpleIntSlider("threshold_maxval", &threshold_maxval, 1.0, 0, 255);
		thresholdAdditionalFlagsCombo->showGui();
	}
	else if (getType() == StepType::AdaptiveThreshold) {
		adaptiveThresholdTypeCombo->showGui();
		ImGuiSimpleIntSlider("adaptive_threshold_maxval", &adaptive_threshold_maxval, 1.0, 0, 255);
		adaptiveThresholdThresholdTypeCombo->showGui();
		ImGuiSimpleIntSlider("adaptive_threshold_blocksize_param", &adaptive_threshold_blocksize_param, 1.0, 0, 4000);
		ImGuiSimpleDoubleSlider("adaptive_threshold_c", &adaptive_threshold_c, 0.1, -4000.0, 4000.0);
	}
	else if (getType() == StepType::Denoise) {
		ImGuiSimpleDoubleSlider("denoise_h", &denoise_h, 0.1, 0, 4000.0);
		ImGuiSimpleIntSlider("denoise_templateWindowSize", &denoise_templateWindowParam, 1.0, 0, 4000);
		ImGuiSimpleIntSlider("denoise_searchWindowSize", &denoise_searchWindowParam, 1.0, 0, 4000);
	}
	else if (getType() == StepType::DenoiseColored) {
		ImGuiSimpleDoubleSlider("denoise_colored_h", &denoise_colored_h, 0.1, 0, 4000.0);
		ImGuiSimpleDoubleSlider("denoise_colored_hColor", &denoise_colored_hColor, 0.1, 0, 4000.0);
		ImGuiSimpleIntSlider("denoise_colored_templateWindowSize", &denoise_colored_templateWindowParam, 1.0, 0, 4000);
		ImGuiSimpleIntSlider("denoise_colored_searchWindowSize", &denoise_colored_searchWindowParam, 1.0, 0, 4000);
	}
	else if (getType() == StepType::InRangeHSV) {
        ImGuiHSVRangeInput("InRangeHSV", &inRangeHSV_h_lower, &inRangeHSV_s_lower, &inRangeHSV_v_lower, &inRangeHSV_h_upper, &inRangeHSV_s_upper, &inRangeHSV_v_upper, 1.0);
		inRangeHSV_invertResult->showGui();
	}
	else if (getType() == StepType::ReplaceColorInRangeHSV) {
		ImGuiHSVRangeInput("ReplaceColorInRangeHSV", &replaceColorInRangeHSV_h_lower, &replaceColorInRangeHSV_s_lower, &replaceColorInRangeHSV_v_lower, &replaceColorInRangeHSV_h_upper, &replaceColorInRangeHSV_s_upper, &replaceColorInRangeHSV_v_upper, 1.0);
		replaceColorInRangeHSV_invertResult->showGui();
		ImGuiSimpleColorSelector("ReplaceColorInRangeHSV's color", replaceColorInRangeHSV_color);
	}
	else if (getType() == StepType::Erode) {
		erosionMorphShapeCombo->showGui();
		ImGuiSimpleIntSlider("erosionSizeX", &erosionSizeX, 1.0, 1, 4000);
		ImGuiSimpleIntSlider("erosionSizeY", &erosionSizeY, 1.0, 1, 4000);
	}
	else if (getType() == StepType::Dilate) {
		dilationMorphShapeCombo->showGui();
		ImGuiSimpleIntSlider("dilationSizeX", &dilationSizeX, 1.0, 1, 4000);
		ImGuiSimpleIntSlider("dilationSizeY", &dilationSizeY, 1.0, 1, 4000);
	}
	else if (getType() == StepType::Normalize) {
		normalizationNormTypeCombo->showGui();
		ImGuiSimpleDoubleSlider("normalizationAlpha", &normalizationAlpha, 0.1, -4000.0, 4000.0);
		ImGuiSimpleDoubleSlider("normalizationBeta", &normalizationBeta, 0.1, -4000.0, 4000.0);
	}
	else if (getType() == StepType::CannyEdgeDetection) {
		ImGuiSimpleDoubleSlider("threshold1", &canny_threshold1, 0.1, -4000.0, 4000.0);
		ImGuiSimpleDoubleSlider("threshold2", &canny_threshold2, 0.1, -4000.0, 4000.0);
		ImGuiSimpleIntSlider("apertureSize", &canny_apertureSize, 1.0, 1, 3);
	}
}