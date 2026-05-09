/*
 * app_init.c - 应用初始化与关闭实现
 */

#include "app_init.h"
#include "file_io.h"
#include "linkedlist.h"
#include "logger.h"

#ifdef _WIN32
#include <direct.h>
#define MKDIR(dir) _mkdir(dir)
#else
#include <sys/stat.h>
#define MKDIR(dir) mkdir(dir, 0755)
#endif

/* 全局应用上下文 */
AppContext g_ctx = { NULL, NULL, NULL, NULL };

int app_init(void) {
    /* 确保 data 目录存在 */
    MKDIR(DATA_DIR);

    /* 加载三个链表 */
    g_ctx.archiveHead  = load_archives(ARCHIVE_FILE);
    g_ctx.userHead     = load_users(USER_FILE);
    g_ctx.favoriteHead = load_favorites(FAVORITE_FILE);
    g_ctx.currentUser  = NULL;

    /* 刷新收藏计数 */
    favorite_refresh_all_counts();

    log_action(LOG_FILE, "system", "应用启动，数据加载完成");
    return 1;
}

void app_shutdown(void) {
    log_action(LOG_FILE, "system", "应用关闭");

    archive_list_free(g_ctx.archiveHead);
    user_list_free(g_ctx.userHead);
    fav_list_free(g_ctx.favoriteHead);

    g_ctx.archiveHead  = NULL;
    g_ctx.userHead     = NULL;
    g_ctx.favoriteHead = NULL;
    g_ctx.currentUser  = NULL;
}

void favorite_refresh_all_counts(void) {
    /* 先将所有档案的收藏计数清零 */
    Archive *arch = g_ctx.archiveHead;
    while (arch) {
        arch->favoriteCount = 0;
        arch = arch->next;
    }

    /* 遍历收藏链表，逐条累加 */
    Favorite *fav = g_ctx.favoriteHead;
    while (fav) {
        Archive *target = archive_node_find_by_id(g_ctx.archiveHead, fav->archiveId);
        if (target) {
            target->favoriteCount++;
        }
        fav = fav->next;
    }
}
