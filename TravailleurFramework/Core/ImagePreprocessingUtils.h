#ifndef ImagePreprocessingUtils_H
#define ImagePreprocessingUtils_H

class Image;
class ImagePreprocessing;
class ImagePreprocessingStep;

class ImagePreprocessingUtils {
public:
	static void applyImagePreprocessing(Image* image, ImagePreprocessing* imagePreprocessing);

private:
	static void applyImagePreprocessingStep(Image* image, ImagePreprocessingStep* step);

	static void applyThreshold(Image* image, ImagePreprocessingStep* step);

	static void applyAdaptiveThreshold(Image* image, ImagePreprocessingStep* step);

	static void applyInRangeHsv(Image* image, ImagePreprocessingStep* step);

	static void applyReplaceColorInRangeHsv(Image* image, ImagePreprocessingStep* step);

	static void applyErode(Image* image, ImagePreprocessingStep* step);

	static void applyDilate(Image* image, ImagePreprocessingStep* step);

	static void applyNormalize(Image* image, ImagePreprocessingStep* step);

	static void applyCannyEdgeDetection(Image* image, ImagePreprocessingStep* step);
};

#endif