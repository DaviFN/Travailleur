#include "TravailleurConsole.h"

#include "imgui.h"

#include "Common/TravailleurUtils.h"
#include "TravailleurContext.h"

TravailleurConsole::TravailleurConsole(TravailleurContext* travailleurContext):
	travailleurContext(travailleurContext)
{
	console = std::make_unique<ImGuiConsole>(TravailleurUtils::getConsolePath(travailleurContext->getName()) + "\\TravailleurConsole");
}

TravailleurConsole::~TravailleurConsole()
{
}

void TravailleurConsole::addNormalMessage(const std::string& message)
{
	console->addNormalMessage(message);
}

void TravailleurConsole::addErrorMessage(const std::string& message)
{
	console->addErrorMessage(message);
}

void TravailleurConsole::addWarningMessage(const std::string& message)
{
	console->addWarningMessage(message);
}

void TravailleurConsole::addInfoMessage(const std::string& message)
{
	console->addInfoMessage(message);
}

void TravailleurConsole::addSuccessMessage(const std::string& message)
{
	console->addSuccessMessage(message);

}

void TravailleurConsole::addDebugMessage(const std::string& message)
{
	console->addDebugMessage(message);
}

void TravailleurConsole::processTravailleurConsoleBuiltInCommands()
{
	const std::vector<std::string> args = console->getLowercaseArgumentsOfCommandToProcess();
	if (args.empty()) return;
}

void TravailleurConsole::applyInterfaceConfig()
{
	const InterfaceConfig* interfaceConfig = travailleurContext->getInterfaceConfig();
	console->setBackgroundColor(interfaceConfig->getTravailleurConsoleBackgroundColor());
	console->setCommandInputMessageColor(interfaceConfig->getTravailleurConsoleCommandInputMessageColor());
	console->setUnrecognizedCommandMessageColor(interfaceConfig->getTravailleurConsoleUnrecognizedCommandMessageColor());
	console->setNormalMessageColor(interfaceConfig->getTravailleurConsoleNormalMessageColor());
	console->setErrorMessageColor(interfaceConfig->getTravailleurConsoleErrorMessageColor());
	console->setWarningMessageColor(interfaceConfig->getTravailleurConsoleWarningMessageColor());
	console->setInfoMessageColor(interfaceConfig->getTravailleurConsoleInfoMessageColor());
	console->setSuccessMessageColor(interfaceConfig->getTravailleurConsoleSuccessMessageColor());
	console->setDebugMessageColor(interfaceConfig->getTravailleurConsoleDebugMessageColor());
}

void TravailleurConsole::showGui()
{
	ImGui::Begin("TravailleurConsole");

	applyInterfaceConfig();

	console->showGui();
	processTravailleurConsoleBuiltInCommands();

	ImGui::End();
}