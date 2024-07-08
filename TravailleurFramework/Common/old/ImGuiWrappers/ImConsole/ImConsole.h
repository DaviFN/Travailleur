#ifndef IMCONSOLE_H
#define IMCONSOLE_H

#include <vector>
#include <string>
#include <mutex>

#include "TravailleurFramework\3rdparty\GUI\ImGui\dear-imgui\imgui.h"

class ImConsole
{
    std::string name;
    std::vector<std::string> logStrings;
    int maxLogsQuantity;
    int linesShown;
    static std::mutex mutex;

    char* c_str_log_array;
    int c_str_log_array_size;

    int lineBreakSize;

    void assureCStrArrayIsBigEnough();
    void sendLogsToCStrArray();

    char* command;

    std::string lastCommand;
    bool userHasProcessedLastCommand;

    void sendCommand();

    bool scrollToBottom = false;

    public:
    ImConsole(std::string name, int maxLogsQuantity, int linesShown);
    ~ImConsole();
    std::string getName();
    void addLog(std::string logString);
    void clearLogs();
    void showGui();
    void endIteration();
    bool onCommand(std::string command);
};

#endif