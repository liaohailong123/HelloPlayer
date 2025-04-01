//
// Created by 廖海龙 on 2024/11/15.
//

#include "FileUtil.hpp"


bool FileUtil::isExist(const char *path)
{
    struct stat file_stat;
    if (stat(path, &file_stat) == 0)
    {
        return true;
    } else
    {
        return false;
    }
}

// 递归删除目录及其内容
int FileUtil::remove_directory(const char *path)
{
    DIR *dir;
    struct dirent *entry;
    char temp_path[4096];

    // 打开目录
    dir = opendir(path);
    if (dir == NULL)
    {
        // 如果目录不存在或无法访问,返回错误
        return -1;
    }

    // 遍历目录中的文件和子目录
    while ((entry = readdir(dir)) != NULL)
    {
        // 跳过 "." 和 ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        {
            continue;
        }

        // 构建完整的文件路径
        snprintf(temp_path, sizeof(temp_path), "%s/%s", path, entry->d_name);

        // 如果是目录,递归删除
        if (entry->d_type == DT_DIR)
        {
            if (remove_directory(temp_path) != 0)
            {
                // 如果删除子目录失败,关闭目录并返回错误
                closedir(dir);
                return -1;
            }
        } else
        {
            // 如果是文件,直接删除
            if (remove(temp_path) != 0)
            {
                // 如果删除文件失败,关闭目录并返回错误
                closedir(dir);
                return -1;
            }
        }
    }

    // 关闭目录
    closedir(dir);

    // 删除空目录
    if (rmdir(path) != 0)
    {
        // 如果删除目录失败,返回错误
        return -1;
    }

    return 0;
}

FILE *FileUtil::clearAndNewFile(const char *filepath) {
    if (FileUtil::isExist(filepath)) {
        if (remove(filepath) == 0) {

        } else {

        }
    }
    // ab 追加模式
    FILE *f = fopen(filepath, "ab");
    return f;
}