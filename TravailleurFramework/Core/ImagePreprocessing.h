#ifndef ImagePreprocessing_H
#define ImagePreprocessing_H

#include <memory>
#include <string>
#include <vector>

#include "ImGuiWrappers/ImGuiSingleSelectionList.h"

class Image;
class ImagePreprocessingStep;
class TravailleurContext;

class ImagePreprocessing
{
public:
	ImagePreprocessing(const std::string& name, TravailleurContext* travailleurContext);
	~ImagePreprocessing();

	const std::string& getName() const;
	void setName(const std::string& name);

	void showGui();

	void apply(Image* image);

	const std::vector<ImagePreprocessingStep*>& getImagePreprocessingSteps() const;

private:
	void save() const;
	void load();

	void saveSteps() const;
	void loadSteps();

	std::string getSaveDirectory() const;
	void assureSaveDirectoryExists();

	void updateStepsSelector();
	void addStep();
	void removeStep(const int stepIndex);
	void swapSteps(const int stepIndex1, const int stepIndex2);

	ImagePreprocessingStep* getSelectedStep() const;

	std::string name;
	std::vector<ImagePreprocessingStep*> steps;

	std::unique_ptr<ImGuiSingleSelectionList> stepsSelector;
	
	TravailleurContext* travailleurContext = nullptr;
};

#endif