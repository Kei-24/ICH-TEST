#ifndef FAVORITE_SERVICE_H
#define FAVORITE_SERVICE_H

/*
 * favorite_service.h - 收藏业务逻辑层
 */

#include "models.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 添加收藏（当前登录用户收藏指定档案），成功后刷新计数+保存，返回1/0 */
int        favorite_add(int archiveId);

/* 取消收藏，成功后刷新计数+保存，返回1/0 */
int        favorite_remove(int archiveId);

/* 查询当前用户的收藏列表，返回指针数组，*count返回数量 */
/* 调用者需free数组，不要free元素 */
Favorite** favorite_list_by_current_user(int *count);

/* 查询指定用户的收藏列表（管理员用） */
Favorite** favorite_list_by_account(const char *account, int *count);

#ifdef __cplusplus
}
#endif

#endif /* FAVORITE_SERVICE_H */
