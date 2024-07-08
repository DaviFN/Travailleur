#include "ImagePreprocessing.h"

#include <algorithm>

#include "imgui.h"

#include "Common/TravailleurUtils.h"
#include "Common/SimplePersistenceUtils.h"
#include "ImagePreprocessingStep.h"
#include "ImagePreprocessingUtils.h"
#include "ImGuiWrappers/ImGuiTextOutput.h"
#include "TravailleurContext.h"

ImagePreprocessing::ImagePreprocessing(const std::string& name, TravailleurContext* travailleurContext) :
    name(name),
	travailleurContext(travailleurContext)
{
	assureSaveDirectoryExists();

	stepsSelector = std::make_unique<ImGuiSingleSelectionList>("steps-selector");
	updateStepsSelector();

	load();
}

ImagePreprocessing::~ImagePreprocessing()
{
	save();

	for (ImagePreprocessingStep* step : steps) {
		delete step;
	}
}

std::string ImagePreprocessing::getSaveDirectory() const
{
	return TravailleurUtils::getImagePreprocessingsPath(travailleurContext->getName()) + "\\" + name;
}

void ImagePreprocessing::assureSaveDirectoryExists()
{
	FilesystemUtils::assureDirectoryExists(getSaveDirectory());
}

void ImagePreprocessing::save() const
{
	saveSteps();

	SimplePersistenceUtils::saveInt32("step-index-selected", (int32_t)stepsSelector->getSelectedIndex(), getSaveDirectory());
}

void ImagePreprocessing::load()
{
	loadSteps();

	int32_t loadedValue = 0;
	SimplePersistenceUtils::loadInt32("step-index-selected", loadedValue, getSaveDirectory());
	stepsSelector->setSelectedIndex(loadedValue);
}

void ImagePreprocessing::saveSteps() const
{
	FILE* f = fopen((getSaveDirectory() + "\\steps").c_str(), "wb");
	if (f != nullptr) {
		const size_t nSteps = steps.size();
		fwrite(&nSteps, sizeof(size_t), 1, f);
		for (ImagePreprocessingStep* step : steps) {
			step->save(f);
		}
		fclose(f);
	}
}

void ImagePreprocessing::loadSteps()
{
	FILE* f = fopen((getSaveDirectory() + "\\steps").c_str(), "rb");
	if (f != nullptr) {
		size_t nSteps = 0;
		fread(&nSteps, sizeof(size_t), 1, f);
		for (size_t i = 0; i < nSteps; ++i) {
			addStep();
		}
		for (ImagePreprocessingStep* step : steps) {
			step->load(f);
		}
		fclose(f);
	}
}

const std::string& ImagePreprocessing::getName() const
{
	return name;
}

void ImagePreprocessing::setName(const std::string& name)
{
	this->name = name;
}

void ImagePreprocessing::showGui()
{
    ImGuiTextOutput(name);

	const std::string nStepsString(std::to_string(steps.size()) + " steps");
	ImGuiTextOutput(nStepsString);

	if(ImGui::Button("Add step")) {
		addStep();
	}

	if (stepsSelector->nOptions() > 0) {
		stepsSelector->showGui();
	}
	else {
		ImGuiTextOutput("No steps");
	}

	if (getSelectedStep() != nullptr) {
		getSelectedStep()->showGui();

		if (ImGui::Button("Remove selected step")) {
			removeStep(stepsSelector->getSelectedIndex());
		}

		if (ImGui::Button("Send step above")) {
			const int selectedIndex = stepsSelector->getSelectedIndex();
			const int newIndex = selectedIndex - 1;
			swapSteps(selectedIndex, newIndex);
			if (newIndex >= 0 && newIndex < steps.size()) {
				stepsSelector->setSelectedIndex(newIndex);
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Send step below")) {
			const int selectedIndex = stepsSelector->getSelectedIndex();
			const int newIndex = selectedIndex + 1;
			swapSteps(selectedIndex, newIndex);
			if (newIndex >= 0 && newIndex < steps.size()) {
				stepsSelector->setSelectedIndex(newIndex);
			}
		}
	}
	else {
		ImGuiTextOutput("No selected step");
	}

}

void ImagePreprocessing::apply(Image* image)
{
	if (image == nullptr) return;
	ImagePreprocessingUtils::applyImagePreprocessing(image, this);
}

const std::vector<ImagePreprocessingStep*>& ImagePreprocessing::getImagePreprocessingSteps() const
{
	return steps;
}

void ImagePreprocessing::updateStepsSelector()
{
	std::vector<std::string> options;
	for (size_t i = 1; i <= steps.size(); ++i) {
		options.push_back("Step #" + std::to_string(i));
	}
	stepsSelector->setOptions(options);
}

void ImagePreprocessing::addStep()
{
	steps.push_back(new ImagePreprocessingStep);
	updateStepsSelector();
	stepsSelector->setSelectedIndex(steps.size() - 1);
}

void ImagePreprocessing::removeStep(const int stepIndex)
{
	steps.erase(steps.begin() + stepIndex);
	updateStepsSelector();
	if (steps.size() != 0) {
		stepsSelector->setSelectedIndex(stepIndex - 1);
	}
}

void ImagePreprocessing::swapSteps(const int stepIndex1, const int stepIndex2)
{
	if (stepIndex1 < 0 || stepIndex1 >= steps.size()) return;
	if (stepIndex2 < 0 || stepIndex2 >= steps.size()) return;

	std::swap(steps[stepIndex1], steps[stepIndex2]);

	updateStepsSelector();
}

ImagePreprocessingStep* ImagePreprocessing::getSelectedStep() const
{
	const int selectedStepIndex = stepsSelector->getSelectedIndex();
	return selectedStepIndex != -1 ? steps[selectedStepIndex] : nullptr;
}