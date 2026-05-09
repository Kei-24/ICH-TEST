#ifndef FILE_IO_H
#define FILE_IO_H

/*
 * file_io.h - 文件读写层
 * 负责将链表与dat文件之间互相转换
 * 文件格式：文本格式，字段以 | 分隔，每行一条记录
 */

#include "models.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 数据文件路径常量 */
#define DATA_DIR         "data"
#define ARCHIVE_FILE     "data/archives.dat"
#define USER_FILE        "data/users.dat"
#define FAVORITE_FILE    "data/favorites.dat"
#define LOG_FILE         "data/log.txt"

/* 档案文件读写 */
Archive*  load_archives(const char *filepath);
int       save_archives(const char *filepath, Archive *head);

/* 用户文件读写 */
User*     load_users(const char *filepath);
int       save_users(const char *filepath, User *head);

/* 收藏文件读写 */
Favorite* load_favorites(const char *filepath);
int       save_favorites(const char *filepath, Favorite *head);

#ifdef __cplusplus
}
#endif

#endif /* FILE_IO_H */
