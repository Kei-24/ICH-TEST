#ifndef APP_INIT_H
#define APP_INIT_H

/*
 * app_init.h - 应用初始化与关闭
 * 负责加载数据、刷新计算字段、程序退出时释放内存
 */

#include "models.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 全局应用上下文 */
extern AppContext g_ctx;

/*
 * 初始化应用：创建data目录、从文件加载三个链表、刷新收藏计数
 * 返回值：1=成功, 0=失败
 */
int app_init(void);

/*
 * 关闭应用：释放三个链表内存
 */
void app_shutdown(void);

/*
 * 刷新所有档案的 favoriteCount（遍历收藏链表统计）
 */
void favorite_refresh_all_counts(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_INIT_H */
