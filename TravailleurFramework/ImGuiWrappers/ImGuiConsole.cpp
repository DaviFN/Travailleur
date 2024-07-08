#include "ImGuiConsole.h"

#include "Common/FilesystemUtils.h"
#include "Common/StringUtils.h"
#include "ImGuiWrappers/ImGuiTextOutput.h"
#include "ImGuiWrappers/ImGuiUtils.h"

#include "imgui.h"

const size_t ImGuiConsole::inputBufferSize = 4096;
const size_t ImGuiConsole::commandHistoryMaxNEntries = 4096;

ImGuiConsole::ImGuiConsole(const std::string& persistencePath)
	: persistencePath(persistencePath),
    backgroundColor(getDefaultBackgroundColor())
{
    inputBuffer = new char[inputBufferSize];
    inputBuffer[0] = 0;

    FilesystemUtils::assureDirectoryExists(persistencePath);

    showOnlyCommandRelatedMessages = std::make_unique<ImGuiCheckbox>("showOnlyCommandRelatedMessages", false, persistencePath + "\\show-only-command-related-messages");

    addMessageType("command input", getDefaultCommandInputMessageColor(), true);
    addMessageType("unrecognized command", getDefaultUnrecognizedCommandMessageColor(), true);

    addMessageType("normal", getDefaultNormalMessageColor(), false);
    addMessageType("error", getDefaultErrorMessageColor(), true);
    addMessageType("warning", getDefaultWarningMessageColor(), true);
    addMessageType("info", getDefaultInfoMessageColor(), true);
    addMessageType("success", getDefaultSuccessMessageColor(), true);
    addMessageType("debug", getDefaultDebugMessageColor(), true);
}

ImGuiConsole::~ImGuiConsole()
{
    delete[] inputBuffer;
}

const Pixel& ImGuiConsole::getBackgroundColor() const
{
    return backgroundColor;
}

Pixel ImGuiConsole::getDefaultBackgroundColor()
{
    return Pixel(0, 0, 0);
}

Pixel ImGuiConsole::getDefaultCommandInputMessageColor()
{
    return Pixel(255, 255, 255);
}

Pixel ImGuiConsole::getDefaultUnrecognizedCommandMessageColor()
{
    return Pixel(255, 0, 0);
}

Pixel ImGuiConsole::getDefaultNormalMessageColor()
{
    return Pixel(255, 255, 255);
}

Pixel ImGuiConsole::getDefaultErrorMessageColor()
{
    return Pixel(255, 0, 0);
}

Pixel ImGuiConsole::getDefaultWarningMessageColor()
{
    return Pixel(245, 173, 66);
}

Pixel ImGuiConsole::getDefaultInfoMessageColor()
{
    return Pixel(35, 145, 240);
}

Pixel ImGuiConsole::getDefaultSuccessMessageColor()
{
    return Pixel(30, 220, 30);
}

Pixel ImGuiConsole::getDefaultDebugMessageColor()
{
    return Pixel(153, 51, 255);
}

void ImGuiConsole::setBackgroundColor(const Pixel& color)
{
    backgroundColor = color;
}

size_t ImGuiConsole::getMaxNOfMessagesDisplayed() const
{
    return maxNOfMessagesDisplayed;
}

void ImGuiConsole::setMaxNOfMessagesDisplayed(const size_t n)
{
    maxNOfMessagesDisplayed = n;
    assureThereAreNoMoreMessagesThanAllowed();
}

void ImGuiConsole::addNormalMessage(const std::string& message)
{
    addMessageInfo(message, "normal");
}

void ImGuiConsole::addErrorMessage(const std::string& message)
{
    addMessageInfo(message, "error");
}

void ImGuiConsole::addWarningMessage(const std::string& message)
{
    addMessageInfo(message, "warning");
}

void ImGuiConsole::addInfoMessage(const std::string& message)
{
    addMessageInfo(message, "info");
}

void ImGuiConsole::addSuccessMessage(const std::string& message)
{
    addMessageInfo(message, "success");
}

void ImGuiConsole::addDebugMessage(const std::string& message)
{
    addMessageInfo(message, "debug");
}

void ImGuiConsole::setCommandInputMessageColor(const Pixel& color)
{
    setMessageTypeColor("command input", color);
}

void ImGuiConsole::setUnrecognizedCommandMessageColor(const Pixel& color)
{
    setMessageTypeColor("unrecognized command", color);
}

void ImGuiConsole::setNormalMessageColor(const Pixel& color)
{
    setMessageTypeColor("normal", color);
}

void ImGuiConsole::setErrorMessageColor(const Pixel& color)
{
    setMessageTypeColor("error", color);
}

void ImGuiConsole::setWarningMessageColor(const Pixel& color)
{
    setMessageTypeColor("warning", color);
}

void ImGuiConsole::setInfoMessageColor(const Pixel& color)
{
    setMessageTypeColor("info", color);
}

void ImGuiConsole::setSuccessMessageColor(const Pixel& color)
{
    setMessageTypeColor("success", color);
}

void ImGuiConsole::setDebugMessageColor(const Pixel& color)
{
    setMessageTypeColor("debug", color);
}

void ImGuiConsole::assureThereAreNoMoreMessagesThanAllowed()
{
    while (messageInfos.size() > maxNOfMessagesDisplayed) {
        messageInfos.pop_front();
    }
}

void ImGuiConsole::addCommandHistoryEntry(const std::string& commandHistoryEntry)
{
    commandHistory.push_back(commandHistoryEntry);
    assureThereAreNoMoreHistoryEntriesThanAllowed();
}

void ImGuiConsole::assureThereAreNoMoreHistoryEntriesThanAllowed()
{
    while (commandHistory.size() > commandHistoryMaxNEntries) {
        commandHistory.erase(commandHistory.begin());
        if (currentCommandHistoryEntryIndex != -1) {
            --currentCommandHistoryEntryIndex;
        }
    }
}

void ImGuiConsole::commandHistoryGoUpCallback()
{
    if (commandHistory.empty() || currentCommandHistoryEntryIndex == 0) return;
    if (currentCommandHistoryEntryIndex == -1) {
        currentCommandHistoryEntryIndex = commandHistory.size() - 1;
    }
    else {
        --currentCommandHistoryEntryIndex;
    }
}

void ImGuiConsole::commandHistoryGoDownCallback()
{
    if (currentCommandHistoryEntryIndex < 0 || currentCommandHistoryEntryIndex + 1 >= commandHistory.size()) return;
    ++currentCommandHistoryEntryIndex;
}

std::string ImGuiConsole::currentCommandHistoryEntry()
{
    if (currentCommandHistoryEntryIndex < 0) return "";
    return commandHistory[currentCommandHistoryEntryIndex];
}

ImGuiConsole::MessageType ImGuiConsole::getDefaultMessageType() const
{
    MessageType defaultMessageType;
    defaultMessageType.showTypeNameAlongsideMessage = false;
    defaultMessageType.color = Pixel(255, 255, 255);
    defaultMessageType.typeName = "default";
    return defaultMessageType;
}

ImGuiConsole::MessageType ImGuiConsole::getMessageType(const std::string& typeName) const
{
    auto it = mapFromMessageTypeNameToMessageType.find(typeName);
    if (it != mapFromMessageTypeNameToMessageType.end()) {
        return it->second;
    }
    return getDefaultMessageType();
}

void ImGuiConsole::setMessageTypeColor(const std::string& typeName, const Pixel& color)
{
    auto it = mapFromMessageTypeNameToMessageType.find(typeName);
    if (it != mapFromMessageTypeNameToMessageType.end()) {
        MessageType& messageType = it->second;
        messageType.color = color;
    }
}

void ImGuiConsole::addMessageType(const std::string& typeName, const Pixel& color, const bool showTypeNameAlongsideMessage)
{
    MessageType messageType;
    messageType.typeName = typeName;
    messageType.color = color;
    messageType.showTypeNameAlongsideMessage = showTypeNameAlongsideMessage;
    mapFromMessageTypeNameToMessageType[typeName] = messageType;
}

void ImGuiConsole::addMessageInfo(const std::string& text, const std::string& typeName)
{
    MessageInfo messageInfo;
    messageInfo.text = text;
    messageInfo.typeName = typeName;
    messageInfos.push_back(messageInfo);
    assureThereAreNoMoreMessagesThanAllowed();

    shouldScrollToBottom = true;
}

void ImGuiConsole::clear()
{
    messageInfos.clear();
}

bool ImGuiConsole::hasCommandToProcess() const
{
    return !commandToProcess.empty();
}

const std::string& ImGuiConsole::getCommandToProcess() const
{
    return commandToProcess;
}

std::string ImGuiConsole::getLowercaseCommandToProcess() const
{
    return StringUtils::toLower(commandToProcess);
}

std::vector<std::string> ImGuiConsole::getArgumentsOfCommandToProcess() const
{
    return StringUtils::split(commandToProcess, ' ');
}

std::vector<std::string> ImGuiConsole::getLowercaseArgumentsOfCommandToProcess() const
{
    return StringUtils::split(getLowercaseCommandToProcess(), ' ');
}

void ImGuiConsole::informCommandHasBeenProcessed()
{
    commandToProcess.clear();
}

void ImGuiConsole::showGui()
{
    showMessageArea();

    ImGui::Separator();

    showInputArea();

    if (ImGui::Button("clear")) {
        clear();
    }

    showOnlyCommandRelatedMessages->showGui();
}

void ImGuiConsole::showMessageArea()
{
    const ImVec4 colorBlack = ImGuiUtils::pixelToImVec4(Pixel(0, 255, 0));

    const ImVec4 backgroundImGuiColor = ImGuiUtils::pixelToImVec4(backgroundColor);
    ImGui::PushStyleColor(ImGuiCol_ChildBg, backgroundImGuiColor);

    ImGui::BeginChild("ScrollingRegion", ImVec2(0, 300), false, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

    for (const MessageInfo& messageInfo : messageInfos) {
        const MessageType messageType =  getMessageType(messageInfo.typeName);
        const std::string messagePrefix = messageType.showTypeNameAlongsideMessage ? "[" + messageInfo.typeName + "] " : "";
        
        if (shouldShowMessageType(messageType)) {
            ImGuiTextOutput(messagePrefix + messageInfo.text, messageType.color);
        }
    }

    if (shouldScrollToBottom) {
        ImGui::SetScrollHereY(1.0f);
        shouldScrollToBottom = false;
    }

    ImGui::PopStyleVar();
    ImGui::EndChild();

    ImGui::PopStyleColor();
}

void ImGuiConsole::showInputArea()
{
    auto callback = [](ImGuiInputTextCallbackData* data) -> int {
        ImGuiConsole* self = (ImGuiConsole*)(data->UserData);
        bool shouldSetText = false;
        switch (data->EventFlag)
        {
            case ImGuiInputTextFlags_CallbackHistory:
                if (data->EventKey == ImGuiKey_UpArrow) {
                    self->commandHistoryGoUpCallback();
                    shouldSetText = true;
                }
                else if (data->EventKey == ImGuiKey_DownArrow) {
                    self->commandHistoryGoDownCallback();
                    shouldSetText = true;
                }
        }

        if (shouldSetText) {
            if (!self->currentCommandHistoryEntry().empty()) {
                data->DeleteChars(0, data->BufTextLen);
                data->InsertChars(0, self->currentCommandHistoryEntry().c_str());
            }
        }

        return 0;
    };

    if (hasCommandToProcess()) {
        addMessageInfo("not sure what you meant by '" + commandToProcess + "'", "unrecognized command");
        commandToProcess = "";
    }
    
    ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackHistory;
    if (ImGui::InputText("Input", inputBuffer, inputBufferSize, input_text_flags, callback, (void*)this)) {
        commandToProcess.assign(inputBuffer);

        inputBuffer[0] = 0;
        if (!commandToProcess.empty()) {
            addMessageInfo(commandToProcess, "command input");

            addCommandHistoryEntry(commandToProcess);
            currentCommandHistoryEntryIndex = -1;
        }

        processBuiltInCommands();

        ImGui::SetKeyboardFocusHere(-1);
    }
}

void ImGuiConsole::processBuiltInCommands()
{
    const std::vector<std::string> args = getLowercaseArgumentsOfCommandToProcess();
    if (args.empty()) return;

    if (args[0] == "cls" || args[0] == "clear") {
        clear();
        informCommandHasBeenProcessed();
    }

    if (args[0] == "senddummymessages") {
        sendDummyMessages();
    }
}

void ImGuiConsole::sendDummyMessages()
{
    for (const auto& item : mapFromMessageTypeNameToMessageType) {
        const std::string& typeName = item.first;
        addMessageInfo("dummy message", typeName);
    }
}

bool ImGuiConsole::isCommandRelatedMessageType(const MessageType& messageType) const
{
    return messageType.typeName == "command input" || messageType.typeName == "unrecognized command";
}

bool ImGuiConsole::shouldShowMessageType(const MessageType& messageType) const
{
    if (showOnlyCommandRelatedMessages->getValue()) {
        return isCommandRelatedMessageType(messageType);
    }
    return true;
}