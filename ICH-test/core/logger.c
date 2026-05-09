/*
 * logger.c - 操作日志实现
 * 日志格式：[2026-04-09 14:30:05] admin: 新增档案 10001 京剧
 */

#include "logger.h"
#include <time.h>

void log_action(const char *filepath, const char *account, const char *action) {
    if (!filepath || !account || !action) return;

    FILE *fp = fopen(filepath, "a");
    if (!fp) return;

    /* 获取当前时间 */
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timeStr[MAX_TIME_LEN];
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", t);

    fprintf(fp, "[%s] %s: %s\n", timeStr, account, action);
    fclose(fp);
}

char* log_read_all(const char *filepath) {
    if (!filepath) return NULL;

    FILE *fp = fopen(filepath, "r");
    if (!fp) return NULL;

    /* 获取文件大小 */
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (size <= 0) {
        fclose(fp);
        return NULL;
    }

    char *buf = (char *)malloc(size + 1);
    if (!buf) {
        fclose(fp);
        return NULL;
    }

    size_t read = fread(buf, 1, size, fp);
    buf[read] = '\0';
    fclose(fp);

    return buf;
}
