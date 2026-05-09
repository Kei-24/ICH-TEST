#ifndef LOGGER_H
#define LOGGER_H

/*
 * logger.h - 操作日志模块
 * 记录用户的增删改查操作到日志文件
 */

#include "models.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * 写一条日志
 * @param filepath  日志文件路径
 * @param account   操作者账号
 * @param action    操作描述，如 "新增档案 10001 京剧"
 */
void log_action(const char *filepath, const char *account, const char *action);

/*
 * 读取全部日志并输出到一个缓冲区（调用者负责 free）
 * @param filepath  日志文件路径
 * @return          日志全文字符串，失败返回 NULL
 */
char* log_read_all(const char *filepath);

#ifdef __cplusplus
}
#endif

#endif /* LOGGER_H */
