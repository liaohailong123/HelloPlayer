//
// Created by 廖海龙 on 2025/2/8.
//

#include "Logger.hpp"

Logger::onLogPrint Logger::logPrinter = nullptr;
bool Logger::enable = false;

Logger::Logger(const char *_tag) : tag(_tag) {}

Logger::~Logger() = default;

void Logger::i(const char *format, ...)
{
    if (enable)
    {
        va_list args;
        va_start(args, format);
        if (logPrinter)
        {
            logPrinter(Info, tag, format, args);
        }
        va_end(args);
    }
}

void Logger::e(const char *format, ...)
{
    if (enable)
    {
        va_list args;
        va_start(args, format);
        if (logPrinter)
        {
            logPrinter(Error, tag, format, args);
        }
        va_end(args);
    }
}