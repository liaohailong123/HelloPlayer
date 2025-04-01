//
// Created by 廖海龙 on 2024/11/15.
//

#ifndef HLSDEMO_FILEUTIL_HPP
#define HLSDEMO_FILEUTIL_HPP

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <cstring>


/**
 * Author: liaohailong
 * Date: 2024/11/15
 * Time: 17:57
 * Description:
 **/
namespace FileUtil {
    bool isExist(const char *path);

    int remove_directory(const char *path);

    FILE *clearAndNewFile(const char *filepath);
}


#endif //HLSDEMO_FILEUTIL_HPP
