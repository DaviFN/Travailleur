#ifndef ImagePreprocessingStep_H
#define ImagePreprocessingStep_H

#include <cstdio>
#include <memory>
#include <string>

#include "Common/Image.h"
#include "ImGuiWrappers/ImGuiCheckbox.h"
#include "ImGuiWrappers/ImGuiCombo.h"
#include "ImagePreprocessingUtils.h"

class ImagePreprocessingStep
{
	friend class ImagePreprocessingUtils;
public:
	enum class StepType {
		DoNothing,
		Invert,
		GaussianBlur,
		Scale,
		Resize,
		Threshold,
		AdaptiveThreshold,
		Denoise,
		DenoiseColored,
		CvtToGray,
		InRangeHSV,
		ReplaceColorInRangeHSV,
		Erode,
		Dilate,
		Normalize,
		CannyEdgeDetection
	};

	enum class ThresholdType {
		ThreshBinary,
		ThreshBinaryInv,
		ThreshTrunc,
		ThreshToZero,
		ThreshToZeroInv
	};

	enum class ThresholdAdditionalFlags {
		None,
		ThreshOtsu,
		ThreshTriangle
	};

	enum class AdaptiveThresholdType {
		AdaptiveThresholdMeanC,
		AdaptiveThresholdGaussianC
	};

	enum class AdaptiveThresholdThresholdType {
		ThreshBinary,
		ThreshBinaryInv
	};

	enum class MorphShapeType {
		MorphRect,
		MorphCross,
		MorphEllipse
	};

	enum class NormalizationNormType {
		NormMinMax,
		NormInf,
		NormL1,
		NormL2
	};

	ImagePreprocessingStep();
	~ImagePreprocessingStep();

	void showGui();

	void save(FILE* f);
	void load(FILE* f);

	StepType getType() const;
	bool isActive() const;

	ThresholdType getThresholdType() const;
	ThresholdAdditionalFlags getThresholdAdditionalFlags() const;

	AdaptiveThresholdType getAdaptiveThresholdType() const;
	AdaptiveThresholdThresholdType getAdaptiveThresholdThresholdType() const;
	MorphShapeType getErosionMorphShapeType() const;
	MorphShapeType getDilationMorphShapeType() const;
	NormalizationNormType getNormalizationNormType() const;

private:
	void initializeStepTypeCombo();

	void initializeStructuringElementTypeCombo(ImGuiCombo& combo);

	std::unique_ptr<ImGuiCombo> stepTypeCombo;

	std::unique_ptr<ImGuiCheckbox> active;

	// GaussianBlur
	int gaussianKernelParamX = 0;
	int gaussianKernelParamY = 0;

	// Scale
	double scaleFactorX = 1.0;
	double scaleFactorY = 1.0;

	// Resize
	int newSizeX = 100;
	int newSizeY = 100;

	// Threshold
	std::unique_ptr<ImGuiCombo> thresholdTypeCombo;
	void initializeThresholdTypeCombo();
	int threshold_thresh = 127;
	int threshold_maxval = 255;
	std::unique_ptr<ImGuiCombo> thresholdAdditionalFlagsCombo;
	void initializeThresholdAdditionalFlagsCombo();

	// Adaptive threshold
	std::unique_ptr<ImGuiCombo> adaptiveThresholdTypeCombo;
	void initializeAdaptiveThresholdTypeCombo();
	int adaptive_threshold_maxval = 255;
	std::unique_ptr<ImGuiCombo> adaptiveThresholdThresholdTypeCombo;
	void initializeAdaptiveThresholdThresholdTypeCombo();
	int adaptive_threshold_blocksize_param = 0;
	double adaptive_threshold_c = 0.0;

	// Denoise
	double denoise_h = 10.0;
	int denoise_templateWindowParam = 3;
	int denoise_searchWindowParam = 10;

	// DenoiseColored
	double denoise_colored_h = 10.0;
	double denoise_colored_hColor = 10.0;
	int denoise_colored_templateWindowParam = 3;
	int denoise_colored_searchWindowParam = 10;

	// InRangeHSV
	double inRangeHSV_h_lower = 0.0;
	double inRangeHSV_s_lower = 0.0;
	double inRangeHSV_v_lower = 0.0;
	double inRangeHSV_h_upper = 0.0;
	double inRangeHSV_s_upper = 0.0;
	double inRangeHSV_v_upper = 0.0;
	std::unique_ptr<ImGuiCheckbox> inRangeHSV_invertResult;

	// ReplaceColorInRangeHSV
	double replaceColorInRangeHSV_h_lower = 0.0;
	double replaceColorInRangeHSV_s_lower = 0.0;
	double replaceColorInRangeHSV_v_lower = 0.0;
	double replaceColorInRangeHSV_h_upper = 0.0;
	double replaceColorInRangeHSV_s_upper = 0.0;
	double replaceColorInRangeHSV_v_upper = 0.0;
	std::unique_ptr<ImGuiCheckbox> replaceColorInRangeHSV_invertResult;
	Pixel replaceColorInRangeHSV_color;

	// Erode
	std::unique_ptr<ImGuiCombo> erosionMorphShapeCombo;
	void initializeErosionMorphShapeCombo();
	int erosionSizeX = 2;
	int erosionSizeY = 2;

	// Dilate
	std::unique_ptr<ImGuiCombo> dilationMorphShapeCombo;
	void initializeDilationMorphShapeCombo();
	int dilationSizeX = 2;
	int dilationSizeY = 2;

	// Normalize
	std::unique_ptr<ImGuiCombo> normalizationNormTypeCombo;
	void initializenormalizationNormTypeCombo();
	double normalizationAlpha = 1.0;
	double normalizationBeta = 0.0;

	// CannyEdgeDetection
	double canny_threshold1 = 1.0;
	double canny_threshold2 = 1.0;
	int canny_apertureSize = 1;
};

#endif