#include "ImagePreprocessingUtils.h"

#include "Common/Image.h"
#include "ImagePreprocessing.h"
#include "ImagePreprocessingStep.h"

void ImagePreprocessingUtils::applyImagePreprocessing(Image* image, ImagePreprocessing* imagePreprocessing)
{
	for (ImagePreprocessingStep* step : imagePreprocessing->getImagePreprocessingSteps()) {
		applyImagePreprocessingStep(image, step);
	}
}

void ImagePreprocessingUtils::applyImagePreprocessingStep(Image* image, ImagePreprocessingStep* step)
{
	if (!step->isActive()) return;

	const ImagePreprocessingStep::StepType type = step->getType();
	cv::Mat* mat = image->getMat();

	if (type == ImagePreprocessingStep::StepType::Invert) {
		image->invert();
	}
	else if (type == ImagePreprocessingStep::StepType::GaussianBlur) {
		cv::GaussianBlur(*mat, *mat, cv::Size(2 * step->gaussianKernelParamX + 1, 2 * step->gaussianKernelParamY + 1), 0, 0);
	}
	else if (type == ImagePreprocessingStep::StepType::Scale) {
		image->scale(step->scaleFactorX, step->scaleFactorY);
	}
	else if (type == ImagePreprocessingStep::StepType::Resize) {
		image->resize(step->newSizeX, step->newSizeY);
	}
	else if (type == ImagePreprocessingStep::StepType::Threshold) {
		applyThreshold(image, step);
	}
	else if (type == ImagePreprocessingStep::StepType::AdaptiveThreshold) {
		applyAdaptiveThreshold(image, step);
	}
	else if (type == ImagePreprocessingStep::StepType::Denoise) {
		const double templateWindowSize = 1 + 2 * step->denoise_templateWindowParam;
		const double searchWindowSize = 1 + 2 * step->denoise_searchWindowParam;
		const int h = step->denoise_h;
		cv::cvtColor(*mat, *mat, cv::COLOR_BGRA2GRAY);
		cv::fastNlMeansDenoising(*mat, *mat, templateWindowSize, searchWindowSize, h);
		cv::cvtColor(*mat, *mat, cv::COLOR_GRAY2BGRA);
	}
	else if (type == ImagePreprocessingStep::StepType::DenoiseColored) {
		const int h = step->denoise_colored_h;
		const int hColor = step->denoise_colored_hColor;
		const double templateWindowSize = 1 + 2 * step->denoise_colored_templateWindowParam;
		const double searchWindowSize = 1 + 2 * step->denoise_colored_searchWindowParam;
		cv::fastNlMeansDenoisingColored(*mat, *mat, h, hColor, templateWindowSize, searchWindowSize);
	}
	else if (type == ImagePreprocessingStep::StepType::CvtToGray) {
		image->toGrayScale();
	}
	else if (type == ImagePreprocessingStep::StepType::InRangeHSV) {
		applyInRangeHsv(image, step);
	}
	else if (type == ImagePreprocessingStep::StepType::ReplaceColorInRangeHSV) {
		applyReplaceColorInRangeHsv(image, step);
	}
	else if (type == ImagePreprocessingStep::StepType::Erode) {
		applyErode(image, step);
	}
	else if (type == ImagePreprocessingStep::StepType::Dilate) {
		applyDilate(image, step);
	}
	else if (type == ImagePreprocessingStep::StepType::Normalize) {
		applyNormalize(image, step);
	}
	else if (type == ImagePreprocessingStep::StepType::CannyEdgeDetection) {
		applyCannyEdgeDetection(image, step);
	}
}

void ImagePreprocessingUtils::applyThreshold(Image* image, ImagePreprocessingStep* step)
{
	cv::Mat* mat = image->getMat();

	cv::cvtColor(*mat, *mat, cv::COLOR_BGRA2GRAY);

	cv::ThresholdTypes thresholdType = cv::THRESH_BINARY;
	switch (step->getThresholdType()) {
		case ImagePreprocessingStep::ThresholdType::ThreshBinary:
			thresholdType = cv::THRESH_BINARY;
			break;
		case ImagePreprocessingStep::ThresholdType::ThreshBinaryInv:
			thresholdType = cv::THRESH_BINARY_INV;
			break;
		case ImagePreprocessingStep::ThresholdType::ThreshToZero:
			thresholdType = cv::THRESH_TOZERO;
			break;
		case ImagePreprocessingStep::ThresholdType::ThreshToZeroInv:
			thresholdType = cv::THRESH_TOZERO_INV;
			break;
		case ImagePreprocessingStep::ThresholdType::ThreshTrunc:
			thresholdType = cv::THRESH_TRUNC;
			break;
	}

	int thresholdAdditionalFlags = 0;
	switch (step->getThresholdAdditionalFlags()) {
	case ImagePreprocessingStep::ThresholdAdditionalFlags::ThreshOtsu:
		thresholdAdditionalFlags |= (int)cv::THRESH_OTSU;
		break;
	case ImagePreprocessingStep::ThresholdAdditionalFlags::ThreshTriangle:
		thresholdAdditionalFlags |= (int)cv::THRESH_TRIANGLE;
		break;
	}

	cv::threshold(*mat, *mat, step->threshold_thresh, step->threshold_maxval, thresholdType | thresholdAdditionalFlags);

	cv::cvtColor(*mat, *mat, cv::COLOR_GRAY2BGRA);
}

void ImagePreprocessingUtils::applyAdaptiveThreshold(Image* image, ImagePreprocessingStep* step)
{
	cv::Mat* mat = image->getMat();

	cv::cvtColor(*mat, *mat, cv::COLOR_BGRA2GRAY);

	cv::AdaptiveThresholdTypes adaptiveThresholdType = cv::ADAPTIVE_THRESH_MEAN_C;
	switch (step->getAdaptiveThresholdType()) {
	case ImagePreprocessingStep::AdaptiveThresholdType::AdaptiveThresholdMeanC:
		adaptiveThresholdType = cv::ADAPTIVE_THRESH_MEAN_C;
		break;
	case ImagePreprocessingStep::AdaptiveThresholdType::AdaptiveThresholdGaussianC:
		adaptiveThresholdType = cv::ADAPTIVE_THRESH_GAUSSIAN_C;
		break;
	}

	cv::ThresholdTypes adaptiveThresholdThresholdType = cv::THRESH_BINARY;
	switch (step->getAdaptiveThresholdThresholdType()) {
	case ImagePreprocessingStep::AdaptiveThresholdThresholdType::ThreshBinary:
		adaptiveThresholdThresholdType = cv::THRESH_BINARY;
		break;
	case ImagePreprocessingStep::AdaptiveThresholdThresholdType::ThreshBinaryInv:
		adaptiveThresholdThresholdType = cv::THRESH_BINARY_INV;
		break;
	}

	const int blockSize = 3 + 2 * step->adaptive_threshold_blocksize_param;

	cv::adaptiveThreshold(*mat, *mat, step->adaptive_threshold_maxval, adaptiveThresholdType, adaptiveThresholdThresholdType, blockSize, step->adaptive_threshold_c);

	cv::cvtColor(*mat, *mat, cv::COLOR_GRAY2BGRA);
}

void ImagePreprocessingUtils::applyInRangeHsv(Image* image, ImagePreprocessingStep* step)
{
	cv::Mat* mat = image->getMat();

	cv::Mat auxMat;
	cv::cvtColor(*mat, auxMat, cv::COLOR_BGRA2BGR);
	cv::cvtColor(auxMat, auxMat, cv::COLOR_BGR2HSV);

	const double h_lower = step->inRangeHSV_h_lower;
	const double s_lower = step->inRangeHSV_s_lower;
	const double v_lower = step->inRangeHSV_v_lower;
	const double h_upper = step->inRangeHSV_h_upper;
	const double s_upper = step->inRangeHSV_s_upper;
	const double v_upper = step->inRangeHSV_v_upper;

	cv::inRange(auxMat, cv::Scalar(h_lower, s_lower, v_lower), cv::Scalar(h_upper, s_upper, v_upper), auxMat);
	const bool invertResult = step->inRangeHSV_invertResult->getValue();
	if (invertResult) {
		cv::bitwise_not(auxMat, auxMat);
	}
	cv::cvtColor(auxMat, *mat, cv::COLOR_GRAY2BGRA);
}

void ImagePreprocessingUtils::applyReplaceColorInRangeHsv(Image* image, ImagePreprocessingStep* step)
{
	cv::Mat* mat = image->getMat();

	const double h_lower = step->replaceColorInRangeHSV_h_lower;
	const double s_lower = step->replaceColorInRangeHSV_s_lower;
	const double v_lower = step->replaceColorInRangeHSV_v_lower;
	const double h_upper = step->replaceColorInRangeHSV_h_upper;
	const double s_upper = step->replaceColorInRangeHSV_s_upper;
	const double v_upper = step->replaceColorInRangeHSV_v_upper;
	const Pixel& color = step->replaceColorInRangeHSV_color;
	const bool invertResult = step->replaceColorInRangeHSV_invertResult->getValue();

	cv::Mat auxMat;
	cv::cvtColor(*mat, auxMat, cv::COLOR_BGRA2BGR);
	cv::cvtColor(auxMat, auxMat, cv::COLOR_BGR2HSV);
	cv::inRange(auxMat, cv::Scalar(h_lower, s_lower, v_lower), cv::Scalar(h_upper, s_upper, v_upper), auxMat);
	if (invertResult) {
		cv::bitwise_not(auxMat, auxMat);
	}

	mat->setTo(cv::Scalar(color.b, color.g, color.r, color.a), auxMat);
}

void ImagePreprocessingUtils::applyErode(Image* image, ImagePreprocessingStep* step)
{
	cv::Mat* mat = image->getMat();

	const ImagePreprocessingStep::MorphShapeType morphShapeType = step->getErosionMorphShapeType();
	cv::MorphShapes cvMorphShapeType = cv::MorphShapes::MORPH_CROSS;
	if (morphShapeType == ImagePreprocessingStep::MorphShapeType::MorphEllipse) {
		cvMorphShapeType = cv::MorphShapes::MORPH_ELLIPSE;
	}
	else if (morphShapeType == ImagePreprocessingStep::MorphShapeType::MorphRect) {
		cvMorphShapeType = cv::MorphShapes::MORPH_RECT;
	}
	const int sizeX = step->erosionSizeX;
	const int sizeY = step->erosionSizeY;

	const cv::Mat structuringElement = cv::getStructuringElement(cvMorphShapeType, cv::Size(sizeX, sizeY));

	cv::erode(*mat, *mat, structuringElement);
}

void ImagePreprocessingUtils::applyDilate(Image* image, ImagePreprocessingStep* step)
{
	cv::Mat* mat = image->getMat();

	const ImagePreprocessingStep::MorphShapeType morphShapeType = step->getDilationMorphShapeType();
	cv::MorphShapes cvMorphShapeType = cv::MorphShapes::MORPH_CROSS;
	if (morphShapeType == ImagePreprocessingStep::MorphShapeType::MorphEllipse) {
		cvMorphShapeType = cv::MorphShapes::MORPH_ELLIPSE;
	}
	else if (morphShapeType == ImagePreprocessingStep::MorphShapeType::MorphRect) {
		cvMorphShapeType = cv::MorphShapes::MORPH_RECT;
	}
	const int sizeX = step->dilationSizeX;
	const int sizeY = step->dilationSizeY;

	const cv::Mat structuringElement = cv::getStructuringElement(cvMorphShapeType, cv::Size(sizeX, sizeY));

	cv::dilate(*mat, *mat, structuringElement);
}

void ImagePreprocessingUtils::applyNormalize(Image* image, ImagePreprocessingStep* step)
{
	cv::Mat* mat = image->getMat();

	const ImagePreprocessingStep::NormalizationNormType normalizationNormType = step->getNormalizationNormType();
	int cvNormType = cv::NormTypes::NORM_INF;
	if (normalizationNormType == ImagePreprocessingStep::NormalizationNormType::NormL1) {
		cvNormType = cv::NormTypes::NORM_L1;
	}
	else if (normalizationNormType == ImagePreprocessingStep::NormalizationNormType::NormL2) {
		cvNormType = cv::NormTypes::NORM_L2;
	}
	else if (normalizationNormType == ImagePreprocessingStep::NormalizationNormType::NormMinMax) {
		cvNormType = cv::NormTypes::NORM_MINMAX;
	}
	const double alpha = step->normalizationAlpha;
	const double beta = step->normalizationBeta;

	cv::normalize(*mat, *mat, alpha, beta, cvNormType);
}

void ImagePreprocessingUtils::applyCannyEdgeDetection(Image* image, ImagePreprocessingStep* step)
{
	cv::Mat* mat = image->getMat();

	double threshold1 = step->canny_threshold1;
	double threshold2 = step->canny_threshold2;
	int apertureSize = step->canny_apertureSize * 2 + 1;

	cv::cvtColor(*mat, *mat, cv::COLOR_BGRA2GRAY);
	cv::Canny(*mat, *mat, threshold1, threshold2, apertureSize);
	cv::cvtColor(*mat, *mat, cv::COLOR_GRAY2BGRA);
}