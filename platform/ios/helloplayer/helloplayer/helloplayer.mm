//
//  HelloWorld.cpp
//  helloplayer
//
//  Created by 廖海龙 on 2025/3/18.
//

#include <stdarg.h>

#include "helloplayer.h"


void onLogPrint2(Logger::Level level, const char *tag, const char *fmt, va_list ap)
{
    // 获取当前时间并格式化为 yyyy-MM-dd hh:mm:ss.zzz
    NSDate *currentDate = [NSDate date];
    NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
    [dateFormatter setDateFormat:@"yyyy-MM-dd HH:mm:ss.SSS"];  // 注意HH表示24小时制，mm是分钟，ss是秒，SSS是毫秒
    NSString *formattedDate = [dateFormatter stringFromDate:currentDate];
    
    // 格式化日志内容
    char buffer[8196];
    vsnprintf(buffer, sizeof(buffer), fmt, ap);  // 使用vsnprintf带长度限制
    
    // 创建最终的日志消息
    NSString *logMsg = [NSString stringWithFormat:@"%@ %s: %s", formattedDate, tag, buffer];
    
    // 根据日志级别输出日志
    if (Logger::Level::Info == level)
    {
        NSLog(@"[INFO] %@", logMsg);
    }
    else if (Logger::Level::Error == level)
    {
        NSLog(@"[ERROR] %@", logMsg);
    }
}

void onLogPrint(const char *tag, const char *fmt, va_list ap) {
    onLogPrint2(Logger::Level::Info, tag, fmt, ap);
}

void HelloPlayerContext::init()
{
    // 注册log能力
    // NativeHanlder 配置log打印
    NHLog::instance()->logPrinter = onLogPrint;
    Logger::logPrinter = onLogPrint2;
    Logger::enable = true;
}

