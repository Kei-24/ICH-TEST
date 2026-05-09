/*
 * favorite_service.c - 收藏业务逻辑实现
 */

#include "favorite_service.h"
#include "app_init.h"
#include "linkedlist.h"
#include "file_io.h"
#include "logger.h"
#include <time.h>

int favorite_add(int archiveId) {
    if (!g_ctx.currentUser) return 0;
    const char *account = g_ctx.currentUser->account;

    /* 检查档案是否存在且有效 */
    Archive *arch = archive_node_find_by_id(g_ctx.archiveHead, archiveId);
    if (!arch || arch->status != ASTAT_NORMAL) return 0;

    /* 检查是否已收藏 */
    if (fav_node_find(g_ctx.favoriteHead, account, archiveId)) return 0;

    Favorite *node = fav_node_create();
    if (!node) return 0;

    strncpy(node->account, account, MAX_ACCOUNT_LEN - 1);
    node->archiveId = archiveId;

    /* 记录当前时间 */
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(node->time, MAX_TIME_LEN, "%Y-%m-%d %H:%M:%S", t);

    fav_node_append(&g_ctx.favoriteHead, node);

    /* 刷新收藏计数 */
    favorite_refresh_all_counts();

    save_favorites(FAVORITE_FILE, g_ctx.favoriteHead);
    save_archives(ARCHIVE_FILE, g_ctx.archiveHead); /* favoriteCount 已更新 */

    char msg[128];
    snprintf(msg, sizeof(msg), "收藏档案: %d %s", archiveId, arch->name);
    log_action(LOG_FILE, account, msg);

    return 1;
}

int favorite_remove(int archiveId) {
    if (!g_ctx.currentUser) return 0;
    const char *account = g_ctx.currentUser->account;

    Archive *arch = archive_node_find_by_id(g_ctx.archiveHead, archiveId);

    int ok = fav_node_remove(&g_ctx.favoriteHead, account, archiveId);
    if (!ok) return 0;

    /* 刷新收藏计数 */
    favorite_refresh_all_counts();

    save_favorites(FAVORITE_FILE, g_ctx.favoriteHead);
    save_archives(ARCHIVE_FILE, g_ctx.archiveHead);

    char msg[128];
    snprintf(msg, sizeof(msg), "取消收藏: %d %s", archiveId, arch ? arch->name : "");
    log_action(LOG_FILE, account, msg);

    return 1;
}

Favorite** favorite_list_by_current_user(int *count) {
    *count = 0;
    if (!g_ctx.currentUser) return NULL;
    return favorite_list_by_account(g_ctx.currentUser->account, count);
}

Favorite** favorite_list_by_account(const char *account, int *count) {
    *count = 0;
    if (!account) return NULL;

    /* 先计数 */
    int total = fav_count_by_account(g_ctx.favoriteHead, account);
    if (total == 0) return NULL;

    Favorite **result = (Favorite **)malloc(total * sizeof(Favorite *));
    if (!result) return NULL;

    int idx = 0;
    Favorite *cur = g_ctx.favoriteHead;
    while (cur) {
        if (strcmp(cur->account, account) == 0)
            result[idx++] = cur;
        cur = cur->next;
    }

    *count = idx;
    return result;
}
