#ifndef ImGuiConsole_H
#define ImGuiConsole_H

#include "Common/Image.h"
#include "ImGuiWrappers/ImGuiCheckbox.h"

#include <list>
#include <map>
#include <string>
#include <vector>

class ImGuiInputTextCallbackData;

class ImGuiConsole {
	public:
		ImGuiConsole(const std::string& persistencePath);
		~ImGuiConsole();
		void showGui();

		static Pixel getDefaultBackgroundColor();
		static Pixel getDefaultCommandInputMessageColor();
		static Pixel getDefaultUnrecognizedCommandMessageColor();
		static Pixel getDefaultNormalMessageColor();
		static Pixel getDefaultErrorMessageColor();
		static Pixel getDefaultWarningMessageColor();
		static Pixel getDefaultInfoMessageColor();
		static Pixel getDefaultSuccessMessageColor();
		static Pixel getDefaultDebugMessageColor();

		const Pixel& getBackgroundColor() const;
		void setBackgroundColor(const Pixel& color);

		size_t getMaxNOfMessagesDisplayed() const;
		void setMaxNOfMessagesDisplayed(const size_t n);

		void addNormalMessage(const std::string& message);
		void addErrorMessage(const std::string& message);
		void addWarningMessage(const std::string& message);
		void addInfoMessage(const std::string& message);
		void addSuccessMessage(const std::string& message);
		void addDebugMessage(const std::string& message);
		
		void setCommandInputMessageColor(const Pixel& color);
		void setUnrecognizedCommandMessageColor(const Pixel& color);

		void setNormalMessageColor(const Pixel& color);
		void setErrorMessageColor(const Pixel& color);
		void setWarningMessageColor(const Pixel& color);
		void setInfoMessageColor(const Pixel& color);
		void setSuccessMessageColor(const Pixel& color);
		void setDebugMessageColor(const Pixel& color);

		void clear();

		bool hasCommandToProcess() const;
		const std::string& getCommandToProcess() const;
		std::string getLowercaseCommandToProcess() const;
		std::vector<std::string> getArgumentsOfCommandToProcess() const;
		std::vector<std::string> getLowercaseArgumentsOfCommandToProcess() const;
		void informCommandHasBeenProcessed();

	private:
		struct MessageType {
			std::string typeName;
			Pixel color;
			bool showTypeNameAlongsideMessage = false;
		};

		struct MessageInfo {
			std::string text;
			std::string typeName;
		};

		MessageType getDefaultMessageType() const;
		MessageType getMessageType(const std::string& typeName) const;
		void setMessageTypeColor(const std::string& typeName, const Pixel& color);
		void addMessageType(const std::string& typeName, const Pixel& color, const bool showTypeNameAlongsideMessage);
		void addMessageInfo(const std::string& text, const std::string& typeName);
		void assureThereAreNoMoreMessagesThanAllowed();
		void addCommandHistoryEntry(const std::string& commandHistoryEntry);
		void assureThereAreNoMoreHistoryEntriesThanAllowed();
		void commandHistoryGoUpCallback();
		void commandHistoryGoDownCallback();
		std::string currentCommandHistoryEntry();
		void showMessageArea();
		void showInputArea();
		void processBuiltInCommands();
		void sendDummyMessages();
		bool isCommandRelatedMessageType(const MessageType& messageType) const;
		bool shouldShowMessageType(const MessageType& messageType) const;

		std::string persistencePath;
		size_t maxNOfMessagesDisplayed = 100;
		std::list<MessageInfo> messageInfos;
		std::string commandToProcess;
		Pixel backgroundColor;
		std::map<std::string, MessageType> mapFromMessageTypeNameToMessageType;

		static const size_t commandHistoryMaxNEntries;
		std::vector<std::string> commandHistory;
		int currentCommandHistoryEntryIndex = -1;

		bool shouldScrollToBottom = false;

		char* inputBuffer = nullptr;
		static const size_t inputBufferSize;

		std::unique_ptr<ImGuiCheckbox> showOnlyCommandRelatedMessages = nullptr;
};

#endif