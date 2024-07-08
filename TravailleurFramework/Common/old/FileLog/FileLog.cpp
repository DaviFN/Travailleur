#include "FileLog.h"

FileLog::FileLog(std::string fileLogName)
{
    this->name = fileLogName;
    f = fopen(fileLogName.c_str(), "w");
    if(f == NULL)
    {
        printf("error: couldn't open fileLog: %s\n", fileLogName);
    }
}

FileLog::~FileLog()
{
    if(f != NULL)
    {
        fclose(f);
    }
}

std::mutex fileMutex;
bool fileWrite(std::string filename, std::string filecontent)
{
    fileMutex.lock();

    if(filename == "")
    {
        fileMutex.unlock();
        return false;
    }

    char filename_modified[2048];
    sprintf(filename_modified, "%s\\%s.txt", travailleurDataPath.c_str(), filename.c_str());

    assure_subdirs_exist(filename_modified);

    FILE* f = fopen(filename_modified, "a+");
    if(f != NULL)
    {
        fprintf(f, "%s", filecontent.c_str());
        fclose(f);

        fileMutex.unlock();
        return true;
    }

    fileMutex.unlock();
    return false;
}

FILE* FileLog::getFilePointer()
{
    return this->f;
}

bool FileLog::write(std::string content)
{
    if(f == NULL)
    {
        return false;
    }
    
    mutex.lock();

    fprintf(f, "%s", content.c_str());

    mutex.unlock();

    return true;
}