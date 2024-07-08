#ifndef IMLOG_H
#define IMLOG_H

#include <vector>
#include <string>
#include <mutex>

#include "TravailleurFramework\3rdparty\GUI\ImGui\dear-imgui\imgui.h"

class ImLog
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

    bool acceptingLogs;

    bool scrollToBottom = false;

    public:
    ImLog(std::string name, int maxLogsQuantity, int linesShown);
    ~ImLog();
    std::string getName();
    void addLog(std::string logString);
    void clearLogs();
    void showGui();
};

#endif