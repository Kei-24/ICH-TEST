#ifndef USER_SERVICE_H
#define USER_SERVICE_H

/*
 * user_service.h - 用户业务逻辑层
 */

#include "models.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 登录验证，成功设置currentUser并返回User*，失败返回NULL */
User* user_login(const char *account, const char *password);

/* 退出登录 */
void  user_logout(void);

/* 添加普通用户，成功后保存文件，返回1/0 */
int   user_add(const char *displayName, const char *account, const char *password);

/* 软删除用户（设status=USTAT_CANCELLED），返回1/0 */
int   user_delete(const char *account);

#ifdef __cplusplus
}
#endif

#endif /* USER_SERVICE_H */
