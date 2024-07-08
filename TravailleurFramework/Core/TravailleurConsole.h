#ifndef TravailleurConsole_H
#define TravailleurConsole_H

#include <memory>

#include "ImGuiWrappers/ImGuiConsole.h"

class TravailleurContext;

class TravailleurConsole
{
public:
	TravailleurConsole(TravailleurContext* travailleurContext);
	~TravailleurConsole();

	void addNormalMessage(const std::string& message);
	void addErrorMessage(const std::string& message);
	void addWarningMessage(const std::string& message);
	void addInfoMessage(const std::string& message);
	void addSuccessMessage(const std::string& message);
	void addDebugMessage(const std::string& message);

	void showGui();

private:
	void applyInterfaceConfig();
	void processTravailleurConsoleBuiltInCommands();

	TravailleurContext* travailleurContext = nullptr;

	std::unique_ptr<ImGuiConsole> console;
};

#endif