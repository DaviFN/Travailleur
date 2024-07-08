#ifndef FileLog_H
#define FileLog_H

#include <string>
#include <cstdio>

#include <mutex>

#include "TravailleurFramework\Common\common_functions\common_functions.h"

#include "TravailleurFramework\Common\travailleur_definitions\travailleur_definitions.h"
class FileLog
{
    std::string name;
    FILE* f;
    std::mutex mutex;
    public:
    FileLog(std::string fileLogName);
    ~FileLog();
    bool write(std::string content);
    FILE* getFilePointer();
};


bool fileWrite(std::string filename, std::string filecontent);

#endif