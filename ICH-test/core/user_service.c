/*
 * user_service.c - 用户业务逻辑实现
 */

#include "user_service.h"
#include "app_init.h"
#include "linkedlist.h"
#include "file_io.h"
#include "logger.h"

User* user_login(const char *account, const char *password) {
    if (!account || !password) return NULL;

    User *user = user_node_find_by_account(g_ctx.userHead, account);
    if (!user) return NULL;

    /* 检查状态 */
    if (user->status != USTAT_NORMAL) return NULL;

    /* 校验密码 */
    if (strcmp(user->password, password) != 0) return NULL;

    g_ctx.currentUser = user;

    char msg[128];
    snprintf(msg, sizeof(msg), "用户登录: %s (%s)",
             user->displayName, user_role_to_str(user->role));
    log_action(LOG_FILE, account, msg);

    return user;
}

void user_logout(void) {
    if (g_ctx.currentUser) {
        char msg[64];
        snprintf(msg, sizeof(msg), "用户登出");
        log_action(LOG_FILE, g_ctx.currentUser->account, msg);
        g_ctx.currentUser = NULL;
    }
}

int user_add(const char *displayName, const char *account, const char *password) {
    if (!displayName || !account || !password) return 0;
    if (strlen(account) == 0 || strlen(password) == 0) return 0;

    /* 检查账号是否已存在 */
    User *existing = user_node_find_by_account(g_ctx.userHead, account);
    if (existing) return 0;

    /* 校验管道符 */
    if (strchr(displayName, '|') || strchr(account, '|') || strchr(password, '|'))
        return 0;

    User *node = user_node_create();
    if (!node) return 0;

    strncpy(node->displayName, displayName, MAX_NAME_LEN - 1);
    strncpy(node->account, account, MAX_ACCOUNT_LEN - 1);
    strncpy(node->password, password, MAX_PASSWORD_LEN - 1);
    node->role = ROLE_USER;
    node->status = USTAT_NORMAL;

    user_node_append(&g_ctx.userHead, node);
    save_users(USER_FILE, g_ctx.userHead);

    char msg[128];
    snprintf(msg, sizeof(msg), "添加用户: %s (账号: %s)", displayName, account);
    const char *who = g_ctx.currentUser ? g_ctx.currentUser->account : "system";
    log_action(LOG_FILE, who, msg);

    return 1;
}

int user_delete(const char *account) {
    if (!account) return 0;

    User *user = user_node_find_by_account(g_ctx.userHead, account);
    if (!user) return 0;

    /* 不允许删除管理员 */
    if (user->role == ROLE_ADMIN) return 0;
    /* 已注销的不重复操作 */
    if (user->status == USTAT_CANCELLED) return 0;

    char msg[128];
    snprintf(msg, sizeof(msg), "删除用户: %s (账号: %s, 软删除)", user->displayName, account);

    user->status = USTAT_CANCELLED;
    save_users(USER_FILE, g_ctx.userHead);

    const char *who = g_ctx.currentUser ? g_ctx.currentUser->account : "system";
    log_action(LOG_FILE, who, msg);

    return 1;
}
