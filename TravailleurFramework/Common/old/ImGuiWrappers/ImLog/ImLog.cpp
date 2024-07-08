#include "ImLog.h"

std::mutex ImLog::mutex;

ImLog::ImLog(std::string name, int maxLogsQuantity, int linesShown)
{
    this->name = name;

    //printf("ImLog %s created\n", this->name.c_str());

    this->maxLogsQuantity = maxLogsQuantity;
    this->linesShown = linesShown;

    if(this->maxLogsQuantity < 0)
    {
        printf("ImLog error: maxLogsQuantity < 0, setting to default 10\n");
        this->maxLogsQuantity = 10;
    }

    if(this->linesShown < 0)
    {
        printf("ImLog error: linesShown < 0, setting to default 10\n");
        this->linesShown = 10;
    }

    c_str_log_array = NULL;
    c_str_log_array_size = 0;


    lineBreakSize = sizeof("\n");

    acceptingLogs = true;
}

ImLog::~ImLog()
{
    if(c_str_log_array != NULL)
    {
        delete[] c_str_log_array;
    }
}

std::string ImLog::getName()
{
    return this->name;
}

void ImLog::addLog(std::string logString)
{
    if(!acceptingLogs)
    {
        return;
    }
    mutex.lock();
    if(logStrings.size() >= maxLogsQuantity)
    {
        logStrings.erase(logStrings.begin());
    }
    logStrings.push_back(logString);

    // scrolls view down
    //ImGui::BeginChild(this->name.c_str());
    //ImGui::SetScrollHereY(1.0f);
    //ImGui::EndChild();
    scrollToBottom = true;

    mutex.unlock();
}

void ImLog::clearLogs()
{
    //mutex.lock();
    logStrings.clear();
    //delete[] c_str_log_array;
    //c_str_log_array = NULL;
    //c_str_log_array_size = 0;
    //mutex.unlock();
    //printf("logs cleared\n");
}

void ImLog::assureCStrArrayIsBigEnough()
{
    int minimumSize = 0;
    int logStringsSize = logStrings.size();
    for(int i = 0 ; i < logStringsSize ; i++)
    {
        minimumSize += logStrings[i].size();
    }
    
    if(logStringsSize != 0)
    {
        minimumSize += (logStringsSize - 1) * lineBreakSize;
    }

    minimumSize++;
    //printf("debug 1\n");
    if(c_str_log_array_size < minimumSize)
    {
        if(c_str_log_array != NULL)
        {
            delete[] c_str_log_array;
        }
        c_str_log_array = new char[minimumSize];
        printf("c_str_log_array allocated: %d\n", c_str_log_array);
        c_str_log_array_size = minimumSize;
    }
    //printf("debug 2 (it's in %d)\n", c_str_log_array);
}

void ImLog::sendLogsToCStrArray()
{
    
    assureCStrArrayIsBigEnough();
    c_str_log_array[0] = 0;
    int logStringsSize = logStrings.size();
    for(int i = 0 ; i < logStringsSize ; i++)
    {
        strcat(c_str_log_array, logStrings[i].c_str());
        if(i != logStringsSize - 1)
        {
            strcat(c_str_log_array, "\n");
        }
    }
}

void ImLog::showGui()
{
    //printf("c_str_log_array: %d\n", c_str_log_array);
    mutex.lock();
    //ImGui::Button("dumbButton");
    /*
    static char text[1024 * 16] =
                "\n"
                " The Pentium F00F bug, shorthand for F0 0F C7 C8,\n"
                " the hexadecimal encoding of one offending instruction,\n"
                " more formally, the invalid operand with locked CMPXCHG8B\n"
                " instruction bug, is a design flaw in the majority of\n"
                " Intel Pentium, Pentium MMX, and Pentium OverDrive\n"
                " processors (all in the P5 microarchitecture).\n"
                "\n\n"
                "label:\n"
                "\tlock cmpxchg8b eax\n";
        */

            //static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;
            //ImGui::CheckboxFlags("ImGuiInputTextFlags_ReadOnly", (unsigned int*)&flags, ImGuiInputTextFlags_ReadOnly);
            //ImGui::CheckboxFlags("ImGuiInputTextFlags_AllowTabInput", (unsigned int*)&flags, ImGuiInputTextFlags_AllowTabInput);
            //ImGui::CheckboxFlags("ImGuiInputTextFlags_CtrlEnterForNewLine", (unsigned int*)&flags, ImGuiInputTextFlags_CtrlEnterForNewLine);
    //ImGui::InputTextMultiline(this->name.c_str(), text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * this->linesShown), ImGuiInputTextFlags_ReadOnly);
    //sendLogsToCStrArray();
    //printf("c_str_log_array: %d\n", c_str_log_array);
    char nameText[2048];
    sprintf(nameText, "ImLog.name: %s", this->name.c_str());
    ImGui::TextWrapped(nameText);
    
    //ImGui::InputTextMultiline(this->name.c_str(), c_str_log_array, c_str_log_array_size, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * this->linesShown), ImGuiInputTextFlags_ReadOnly);
    
    ImGui::Separator();
    
    ImGui::BeginChild(this->name.c_str(), ImVec2(0, ImGui::GetTextLineHeight() * linesShown), false, ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4,1)); // Tighten spacing
    int logStringsSize = logStrings.size();
    for (int i = 0 ; i < logStringsSize; i++)
    {
        // Normally you would store more information in your item than just a string.
        // (e.g. make Items[] an array of structure, store color/type etc.)
        //ImVec4 color;
        //color = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        //ImGui::PushStyleColor(ImGuiCol_Text, color);
        
        char log_c_str[2048];
        strcpy(log_c_str, logStrings[i].c_str());
        ImGui::TextUnformatted(log_c_str);
        
        //ImGui::PopStyleColor();
    }
    
    if(scrollToBottom)
    {
        ImGui::SetScrollHereY(1.0f);
        scrollToBottom = false;
    }
    
    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::Separator();

    // makes sure it's always scrolled down
    /*
    ImGui::BeginChild(this->name.c_str());
    ImGui::SetScrollHereY(1.0);
    ImGui::EndChild();
    */
    
    char clearLogsText[2048];
    sprintf(clearLogsText, "clear logs (%s)", this->name.c_str());
    if(ImGui::Button(clearLogsText))
    {
        this->clearLogs();
    }

    char acceptingLogsText[2048];
    sprintf(acceptingLogsText, "accepting logs (%s)", this->name.c_str());
    ImGui::Checkbox(acceptingLogsText, &acceptingLogs);

    mutex.unlock();
}