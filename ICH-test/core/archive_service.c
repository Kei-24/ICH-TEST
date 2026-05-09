/*
 * archive_service.c - 档案业务逻辑实现
 */

#include "archive_service.h"
#include "app_init.h"
#include "linkedlist.h"
#include "file_io.h"
#include "logger.h"

/* ================================================================
 *                         增删改
 * ================================================================ */

int archive_add(Archive *data) {
    if (!data) return -1;

    Archive *node = archive_node_create();
    if (!node) return -1;

    /* 分配新ID */
    int newId = archive_node_max_id(g_ctx.archiveHead) + 1;
    *node = *data;
    node->id = newId;
    node->status = ASTAT_NORMAL;
    node->favoriteCount = 0;
    node->next = NULL;

    archive_node_append(&g_ctx.archiveHead, node);
    save_archives(ARCHIVE_FILE, g_ctx.archiveHead);

    char msg[128];
    snprintf(msg, sizeof(msg), "添加档案: %d %s", newId, node->name);
    const char *who = g_ctx.currentUser ? g_ctx.currentUser->account : "system";
    log_action(LOG_FILE, who, msg);

    return newId;
}

int archive_update(int id, Archive *newData) {
    if (!newData) return 0;

    Archive *target = archive_node_find_by_id(g_ctx.archiveHead, id);
    if (!target || target->status == ASTAT_REMOVED) return 0;

    /* 保留不可修改的字段 */
    int savedId = target->id;
    int savedFavCount = target->favoriteCount;
    ArchiveStatus savedStatus = target->status;
    struct Archive *savedNext = target->next;

    *target = *newData;
    target->id = savedId;
    target->favoriteCount = savedFavCount;
    target->status = savedStatus;
    target->next = savedNext;

    save_archives(ARCHIVE_FILE, g_ctx.archiveHead);

    char msg[128];
    snprintf(msg, sizeof(msg), "修改档案: %d %s", id, target->name);
    const char *who = g_ctx.currentUser ? g_ctx.currentUser->account : "system";
    log_action(LOG_FILE, who, msg);

    return 1;
}

int archive_delete(int id) {
    Archive *target = archive_node_find_by_id(g_ctx.archiveHead, id);
    if (!target || target->status == ASTAT_REMOVED) return 0;

    char msg[128];
    snprintf(msg, sizeof(msg), "删除档案: %d %s (软删除)", id, target->name);

    target->status = ASTAT_REMOVED;
    save_archives(ARCHIVE_FILE, g_ctx.archiveHead);

    const char *who = g_ctx.currentUser ? g_ctx.currentUser->account : "system";
    log_action(LOG_FILE, who, msg);

    return 1;
}

/* ================================================================
 *                           查询
 * ================================================================ */

Archive* archive_query_by_id(int id) {
    Archive *node = archive_node_find_by_id(g_ctx.archiveHead, id);
    if (node && node->status == ASTAT_NORMAL) return node;
    return NULL;
}

Archive** archive_search_by_name(const char *keyword, int *count) {
    *count = 0;
    if (!keyword) return NULL;

    /* 先计数 */
    int total = 0;
    Archive *cur = g_ctx.archiveHead;
    while (cur) {
        if (cur->status == ASTAT_NORMAL && strstr(cur->name, keyword))
            total++;
        cur = cur->next;
    }

    if (total == 0) return NULL;

    Archive **result = (Archive **)malloc(total * sizeof(Archive *));
    if (!result) return NULL;

    int idx = 0;
    cur = g_ctx.archiveHead;
    while (cur) {
        if (cur->status == ASTAT_NORMAL && strstr(cur->name, keyword))
            result[idx++] = cur;
        cur = cur->next;
    }

    *count = total;
    return result;
}

/* ================================================================
 *                           排序
 * ================================================================ */

/* 收集所有NORMAL档案到指针数组 */
static Archive** collect_active(int *count) {
    int total = 0;
    Archive *cur = g_ctx.archiveHead;
    while (cur) {
        if (cur->status == ASTAT_NORMAL) total++;
        cur = cur->next;
    }

    if (total == 0) { *count = 0; return NULL; }

    Archive **arr = (Archive **)malloc(total * sizeof(Archive *));
    if (!arr) { *count = 0; return NULL; }

    int idx = 0;
    cur = g_ctx.archiveHead;
    while (cur) {
        if (cur->status == ASTAT_NORMAL)
            arr[idx++] = cur;
        cur = cur->next;
    }

    *count = total;
    return arr;
}

static int cmp_heat_desc(const void *a, const void *b) {
    Archive *aa = *(Archive **)a;
    Archive *bb = *(Archive **)b;
    return bb->heatIndex - aa->heatIndex;
}

static int cmp_composite_desc(const void *a, const void *b) {
    Archive *aa = *(Archive **)a;
    Archive *bb = *(Archive **)b;
    long long ca = (long long)aa->heatIndex * aa->favoriteCount;
    long long cb = (long long)bb->heatIndex * bb->favoriteCount;
    if (cb > ca) return 1;
    if (cb < ca) return -1;
    return 0;
}

Archive** archive_sort_by_heat(int *count) {
    Archive **arr = collect_active(count);
    if (arr && *count > 1)
        qsort(arr, *count, sizeof(Archive *), cmp_heat_desc);
    return arr;
}

Archive** archive_sort_by_composite_heat(int *count) {
    Archive **arr = collect_active(count);
    if (arr && *count > 1)
        qsort(arr, *count, sizeof(Archive *), cmp_composite_desc);
    return arr;
}

/* ================================================================
 *                           统计
 * ================================================================ */

void archive_stat_category(int result[]) {
    int i;
    for (i = 0; i < CAT_COUNT; i++) result[i] = 0;

    Archive *cur = g_ctx.archiveHead;
    while (cur) {
        if (cur->status == ASTAT_NORMAL && cur->category >= 0 && cur->category < CAT_COUNT)
            result[cur->category]++;
        cur = cur->next;
    }
}

int archive_stat_total_active(void) {
    return archive_node_count_active(g_ctx.archiveHead);
}

/* ================================================================
 *                         输入校验
 * ================================================================ */

int archive_validate_text(const char *text) {
    if (!text) return 0;
    return (strchr(text, '|') == NULL) ? 1 : 0;
}
