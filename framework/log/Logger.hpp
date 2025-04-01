//
// Created by 廖海龙 on 2025/2/8.
//

#ifndef HELLOPLAYER_LOGGER_HPP
#define HELLOPLAYER_LOGGER_HPP

#include <string>
#include <cstdarg>


/**
 * Author: liaohailong
 * Date: 2025/2/8
 * Time: 14:13
 * Description: Log日志
 **/
class Logger {
public:
    enum Level
    {
        Info,
        Error
    };
    typedef void (*onLogPrint)(Level level, const char *tag, const char *fmt, va_list ap);
public:
    explicit Logger(const char *tag);

    ~Logger();

    void i(const char *format, ...);

    void e(const char *format, ...);

public:
    /**
     * 让各自平台侧实现日志打印能力,添加进来
     */
    static onLogPrint logPrinter;
    static bool enable;
private:
    const char* tag;

};



#endif //HELLOPLAYER_LOGGER_HPP
