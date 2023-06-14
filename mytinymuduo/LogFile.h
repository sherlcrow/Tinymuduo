#pragma once
#ifndef LOG_FILE_H
#define LOG_FILE_H
// 提供对日志文件的操作
// 包括滚动日志文件
// 将 log 数据写到当前 log 文件
// flush log数据到当前 log 文件
#include "FileUtil.h"

#include <mutex>
#include <memory>
class LogFile
{
public:
    LogFile(const std::string& basename,
        off_t rollSize,
        int flushInterval = 3,
        int checkEveryN = 1024);
    ~LogFile() = default;

    void append(const char* data, int len);
    void flush();
    bool rollFile(); // 滚动日志
private:
    static std::string getLogFileName(const std::string& basename, time_t* now);
    void appendInLock(const char* data, int len);

    const std::string basename_; //文件名
    const off_t rollSize_; 
    const int flushInterval_;
    const int checkEveryN_;

    int count_;

    std::unique_ptr<std::mutex> mutex_;
    time_t startOfPeriod_;
    time_t lastRoll_;
    time_t lastFlush_;
    std::unique_ptr<FileUtil> file_;

    const static int kRollPerSeconds_ = 60 * 60 * 24;

};
#endif
