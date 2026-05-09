#ifndef LINKEDLIST_H
#define LINKEDLIST_H

/*
 * linkedlist.h - 链表纯数据操作层
 * 只做节点的创建、插入、查找、删除、释放
 * 不含业务逻辑、不写日志、不保存文件
 */

#include "models.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ===== 档案链表 ===== */
Archive*  archive_node_create(void);
void      archive_node_append(Archive **head, Archive *node);
Archive*  archive_node_find_by_id(Archive *head, int id);
int       archive_node_count(Archive *head);         /* 统计所有节点数（含已移除） */
int       archive_node_count_active(Archive *head);  /* 仅统计status=NORMAL的 */
int       archive_node_max_id(Archive *head);
void      archive_list_free(Archive *head);

/* ===== 用户链表 ===== */
User*     user_node_create(void);
void      user_node_append(User **head, User *node);
User*     user_node_find_by_account(User *head, const char *account);
void      user_list_free(User *head);

/* ===== 收藏链表 ===== */
Favorite* fav_node_create(void);
void      fav_node_append(Favorite **head, Favorite *node);
Favorite* fav_node_find(Favorite *head, const char *account, int archiveId);
int       fav_node_remove(Favorite **head, const char *account, int archiveId);
int       fav_count_by_archive(Favorite *head, int archiveId);
int       fav_count_by_account(Favorite *head, const char *account);
void      fav_list_free(Favorite *head);

#ifdef __cplusplus
}
#endif

#endif /* LINKEDLIST_H */
