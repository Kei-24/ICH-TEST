/*
 * file_io.c - 文件读写实现
 * 文件格式：文本，字段以 | 分隔，每行一条记录
 */

#include "file_io.h"
#include "linkedlist.h"

/* ================================================================
 *                      档案文件读写
 * ================================================================ */

/*
 * 文件格式：id|name|category|origin|inheritor|protection|digitalStatus|heatIndex|imagePath|status
 * 示例：10001|京剧|2|北京|梅葆玖|1|0|85|10001.jpg|0
 */
Archive* load_archives(const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) return NULL;

    Archive *head = NULL;
    char line[512];

    while (fgets(line, sizeof(line), fp)) {
        /* 去掉换行符 */
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) == 0) continue;

        Archive *node = archive_node_create();
        if (!node) continue;

        int cat, prot, digi, stat;
        int parsed = sscanf(line, "%d|%63[^|]|%d|%63[^|]|%63[^|]|%d|%d|%d|%127[^|]|%d",
            &node->id, node->name, &cat, node->origin, node->inheritor,
            &prot, &digi, &node->heatIndex, node->imagePath, &stat);

        if (parsed == 10) {
            node->category = (CultureCategory)cat;
            node->protection = (ProtectionLevel)prot;
            node->digitalStatus = (DigitalStatus)digi;
            node->status = (ArchiveStatus)stat;
            node->favoriteCount = 0; /* 启动后由 favorite_refresh_all_counts 刷新 */
            archive_node_append(&head, node);
        } else {
            free(node);
        }
    }

    fclose(fp);
    return head;
}

int save_archives(const char *filepath, Archive *head) {
    FILE *fp = fopen(filepath, "w");
    if (!fp) return 0;

    Archive *cur = head;
    while (cur) {
        fprintf(fp, "%d|%s|%d|%s|%s|%d|%d|%d|%s|%d\n",
            cur->id, cur->name, (int)cur->category, cur->origin, cur->inheritor,
            (int)cur->protection, (int)cur->digitalStatus, cur->heatIndex,
            cur->imagePath, (int)cur->status);
        cur = cur->next;
    }

    fclose(fp);
    return 1;
}

/* ================================================================
 *                       用户文件读写
 * ================================================================ */

/*
 * 文件格式：displayName|account|password|role|status
 * 示例：管理员|admin|admin123|0|0
 */
User* load_users(const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) return NULL;

    User *head = NULL;
    char line[256];

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) == 0) continue;

        User *node = user_node_create();
        if (!node) continue;

        int role, stat;
        int parsed = sscanf(line, "%63[^|]|%31[^|]|%31[^|]|%d|%d",
            node->displayName, node->account, node->password, &role, &stat);

        if (parsed == 5) {
            node->role = (UserRole)role;
            node->status = (UserStatus)stat;
            user_node_append(&head, node);
        } else {
            free(node);
        }
    }

    fclose(fp);
    return head;
}

int save_users(const char *filepath, User *head) {
    FILE *fp = fopen(filepath, "w");
    if (!fp) return 0;

    User *cur = head;
    while (cur) {
        fprintf(fp, "%s|%s|%s|%d|%d\n",
            cur->displayName, cur->account, cur->password,
            (int)cur->role, (int)cur->status);
        cur = cur->next;
    }

    fclose(fp);
    return 1;
}

/* ================================================================
 *                      收藏文件读写
 * ================================================================ */

/*
 * 文件格式：account|archiveId|time
 * 示例：zhangsan|10001|2026-04-09 14:30:05
 */
Favorite* load_favorites(const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) return NULL;

    Favorite *head = NULL;
    char line[256];

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) == 0) continue;

        Favorite *node = fav_node_create();
        if (!node) continue;

        int parsed = sscanf(line, "%31[^|]|%d|%19[^\n]",
            node->account, &node->archiveId, node->time);

        if (parsed == 3) {
            fav_node_append(&head, node);
        } else {
            free(node);
        }
    }

    fclose(fp);
    return head;
}

int save_favorites(const char *filepath, Favorite *head) {
    FILE *fp = fopen(filepath, "w");
    if (!fp) return 0;

    Favorite *cur = head;
    while (cur) {
        fprintf(fp, "%s|%d|%s\n",
            cur->account, cur->archiveId, cur->time);
        cur = cur->next;
    }

    fclose(fp);
    return 1;
}
