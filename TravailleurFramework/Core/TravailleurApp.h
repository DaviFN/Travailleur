#ifndef TravailleurApp_H
#define TravailleurApp_H

#include <map>
#include <string>

#include "Common/Image.h"
#include "Common/KeyboardAndMouseDefs.h"

class ImageResource;
class RectangularRegion;
class TravailleurContext;

class TravailleurApp {
	friend class TravailleurAppsHandler;

public:
	TravailleurApp();
	~TravailleurApp();

	TravailleurContext* getContext() const;

	virtual std::string getName() const = 0;

	virtual void setup() = 0;

	virtual void processFrame() = 0;

	virtual void beforeLaunch();

	virtual void afterExecutionStopped();

	void registerPointInput(const std::string& name);

	void registerRectangularRegion(const std::string& name);

	void registerImagePattern(const std::string& name);

	void registerImagePreprocessing(const std::string& name);

	void registerImageOCR(const std::string& name);

	void registerImageResource(const std::string& name);

	bool hasImagePattern(const std::string& imagePatternName, const std::string& rectangularRegionName);

	std::string performImageOcr(const std::string& imageOcrName, const std::string& rectangularRegionName);

	void pushAsyncHasImagePattern(bool* result, const std::string& imagePatternName, const std::string& rectangularRegionName);

	void pushAsyncPerformImageOcr(std::string* result, const std::string& imageOcrName, const std::string& rectangularRegionName);

	void runAsyncReadings();

	bool rectangularRegionIsValidOnFrame(const std::string& rectangularRegionName) const;

	void showImageResource(const std::string& imageResourceName);

	void showNormalMessage(const std::string& message);
	void showErrorMessage(const std::string& message);
	void showWarningMessage(const std::string& message);
	void showInfoMessage(const std::string& message);
	void showSuccessMessage(const std::string& message);
	void showDebugMessage(const std::string& message);

	virtual void showPreRunGui() = 0;
	
	virtual void showRunningGui() = 0;

	std::string getAppPath() const;

	std::string getFramesConfigPath() const;

	virtual void addAppFramesDisableOption(const std::string& frameName);

	bool frameIsEnabledByUser(const std::string& frameName) const;

	virtual bool canBeLaunched() const { return true; }

protected:
	virtual void save() const;
	virtual void load();

	void pressKey(const KeyboardAndMouseDefs::Key key, const int pressTimeInMs);
	void pressLeftMouseButton(const int pressTimeInMs);
	void pressRightMouseButton(const int pressTimeInMs);
	void moveMouse(const int x, const int y);
	void moveMouse(const std::string& pointInputName);

private:
	enum class VerificationType {
		Type_HasImagePattern,
		Type_PerformImageOcr
	};

	struct AsyncVerificationData {
		VerificationType type;

		bool* result_hasImagePattern;
		std::string* result_performImageOcr;

		std::string imagePatternName;
		std::string imageOcrName;
		std::string rectangularRegionName;
	};

	void beforeProcessFrame();

	void baseSave() const;
	void baseLoad();

	void assureDirectoriesExist() const;

	void afterSetup();
	void showAppFramesInMainMenu();

	Image* getFrameBeingProcessed() const;

	void insertFrameRectangularOverlay(RectangularRegion* rectangularRegion, const Pixel& color);
	void insertFrameImageOverlay(ImageResource* imageResource);

	void runAsyncReading(const AsyncVerificationData& data);

	TravailleurContext* travailleurContext = nullptr;

	std::map<std::string, bool> appFrameEnabledMap;

	std::vector<AsyncVerificationData> asyncVerificationDatas;
};

#endif