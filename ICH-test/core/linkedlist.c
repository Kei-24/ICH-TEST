/*
 * linkedlist.c - 链表纯数据操作实现
 */

#include "linkedlist.h"

/* ================================================================
 *                        档案链表
 * ================================================================ */

Archive* archive_node_create(void) {
    Archive *node = (Archive *)malloc(sizeof(Archive));
    if (node) {
        memset(node, 0, sizeof(Archive));
        node->next = NULL;
    }
    return node;
}

void archive_node_append(Archive **head, Archive *node) {
    if (!head || !node) return;
    if (*head == NULL) {
        *head = node;
        return;
    }
    Archive *cur = *head;
    while (cur->next) {
        cur = cur->next;
    }
    cur->next = node;
    node->next = NULL;
}

Archive* archive_node_find_by_id(Archive *head, int id) {
    Archive *cur = head;
    while (cur) {
        if (cur->id == id) return cur;
        cur = cur->next;
    }
    return NULL;
}

int archive_node_count(Archive *head) {
    int count = 0;
    Archive *cur = head;
    while (cur) {
        count++;
        cur = cur->next;
    }
    return count;
}

int archive_node_count_active(Archive *head) {
    int count = 0;
    Archive *cur = head;
    while (cur) {
        if (cur->status == ASTAT_NORMAL) count++;
        cur = cur->next;
    }
    return count;
}

int archive_node_max_id(Archive *head) {
    int maxId = 10000; /* 起始ID为10001，所以基准值为10000 */
    Archive *cur = head;
    while (cur) {
        if (cur->id > maxId) maxId = cur->id;
        cur = cur->next;
    }
    return maxId;
}

void archive_list_free(Archive *head) {
    Archive *cur = head;
    while (cur) {
        Archive *tmp = cur;
        cur = cur->next;
        free(tmp);
    }
}

/* ================================================================
 *                         用户链表
 * ================================================================ */

User* user_node_create(void) {
    User *node = (User *)malloc(sizeof(User));
    if (node) {
        memset(node, 0, sizeof(User));
        node->next = NULL;
    }
    return node;
}

void user_node_append(User **head, User *node) {
    if (!head || !node) return;
    if (*head == NULL) {
        *head = node;
        return;
    }
    User *cur = *head;
    while (cur->next) {
        cur = cur->next;
    }
    cur->next = node;
    node->next = NULL;
}

User* user_node_find_by_account(User *head, const char *account) {
    if (!account) return NULL;
    User *cur = head;
    while (cur) {
        if (strcmp(cur->account, account) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void user_list_free(User *head) {
    User *cur = head;
    while (cur) {
        User *tmp = cur;
        cur = cur->next;
        free(tmp);
    }
}

/* ================================================================
 *                        收藏链表
 * ================================================================ */

Favorite* fav_node_create(void) {
    Favorite *node = (Favorite *)malloc(sizeof(Favorite));
    if (node) {
        memset(node, 0, sizeof(Favorite));
        node->next = NULL;
    }
    return node;
}

void fav_node_append(Favorite **head, Favorite *node) {
    if (!head || !node) return;
    if (*head == NULL) {
        *head = node;
        return;
    }
    Favorite *cur = *head;
    while (cur->next) {
        cur = cur->next;
    }
    cur->next = node;
    node->next = NULL;
}

Favorite* fav_node_find(Favorite *head, const char *account, int archiveId) {
    if (!account) return NULL;
    Favorite *cur = head;
    while (cur) {
        if (strcmp(cur->account, account) == 0 && cur->archiveId == archiveId)
            return cur;
        cur = cur->next;
    }
    return NULL;
}

int fav_node_remove(Favorite **head, const char *account, int archiveId) {
    if (!head || !account) return 0;

    Favorite *prev = NULL;
    Favorite *cur = *head;

    while (cur) {
        if (strcmp(cur->account, account) == 0 && cur->archiveId == archiveId) {
            if (prev) {
                prev->next = cur->next;
            } else {
                *head = cur->next;
            }
            free(cur);
            return 1; /* 成功删除 */
        }
        prev = cur;
        cur = cur->next;
    }
    return 0; /* 未找到 */
}

int fav_count_by_archive(Favorite *head, int archiveId) {
    int count = 0;
    Favorite *cur = head;
    while (cur) {
        if (cur->archiveId == archiveId) count++;
        cur = cur->next;
    }
    return count;
}

int fav_count_by_account(Favorite *head, const char *account) {
    if (!account) return 0;
    int count = 0;
    Favorite *cur = head;
    while (cur) {
        if (strcmp(cur->account, account) == 0) count++;
        cur = cur->next;
    }
    return count;
}

void fav_list_free(Favorite *head) {
    Favorite *cur = head;
    while (cur) {
        Favorite *tmp = cur;
        cur = cur->next;
        free(tmp);
    }
}
