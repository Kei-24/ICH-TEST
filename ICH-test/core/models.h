#ifndef MODELS_H
#define MODELS_H

/*
 * models.h - 非遗文化数字化平台 数据模型定义
 * 纯C头文件，不依赖任何Qt头文件
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========== 常量定义 ========== */
#define MAX_NAME_LEN     64
#define MAX_ORIGIN_LEN   64
#define MAX_PATH_LEN    128
#define MAX_ACCOUNT_LEN  32
#define MAX_PASSWORD_LEN 32
#define MAX_TIME_LEN     20    /* "2026-04-09 14:30:05" */
#define PAGE_SIZE         10   /* 每页显示条数 */

/* ========== 枚举类型 ========== */

/* 文化类别 */
typedef enum {
    CAT_TRADITIONAL_CRAFT = 0,  /* 传统技艺 */
    CAT_TRADITIONAL_MUSIC,      /* 传统音乐 */
    CAT_TRADITIONAL_DRAMA,      /* 传统戏剧 */
    CAT_TRADITIONAL_ART,        /* 传统美术 */
    CAT_FOLK_CUSTOM,            /* 民俗 */
    CAT_TRADITIONAL_MEDICINE,   /* 传统医药 */
    CAT_COUNT                   /* 类别总数（用于遍历） */
} CultureCategory;

/* 保护级别 */
typedef enum {
    PROT_WORLD = 0,             /* 世界级 */
    PROT_NATIONAL,              /* 国家级 */
    PROT_PROVINCIAL,            /* 省级 */
    PROT_CITY,                  /* 市级 */
    PROT_ENDANGERED,            /* 濒危 */
    PROT_COUNT
} ProtectionLevel;

/* 数字化状态 */
typedef enum {
    DIGI_DONE = 0,              /* 已数字化 */
    DIGI_IN_PROGRESS,           /* 数字化中 */
    DIGI_NOT_STARTED,           /* 未数字化 */
    DIGI_COUNT
} DigitalStatus;

/* 用户身份 */
typedef enum {
    ROLE_ADMIN = 0,             /* 管理员 */
    ROLE_USER                   /* 普通用户（游客/受众） */
} UserRole;

/* 用户状态 */
typedef enum {
    USTAT_NORMAL = 0,           /* 正常 */
    USTAT_BANNED,               /* 封禁 */
    USTAT_CANCELLED             /* 注销（软删除） */
} UserStatus;

/* 档案状态 */
typedef enum {
    ASTAT_NORMAL = 0,           /* 正常展示 */
    ASTAT_HIDDEN,               /* 隐藏（下架） */
    ASTAT_REMOVED               /* 移除（软删除） */
} ArchiveStatus;

/* ========== 结构体定义 ========== */

/* 文化档案 */
typedef struct Archive {
    int                id;                          /* 项目编号，从10001递增 */
    char               name[MAX_NAME_LEN];          /* 项目名称 */
    CultureCategory    category;                    /* 文化类别 */
    char               origin[MAX_ORIGIN_LEN];      /* 朝代/发源地 */
    char               inheritor[MAX_NAME_LEN];     /* 传承人 */
    ProtectionLevel    protection;                  /* 保护级别 */
    DigitalStatus      digitalStatus;               /* 数字化状态 */
    int                heatIndex;                   /* 国际热度指数 (0-100) */
    char               imagePath[MAX_PATH_LEN];     /* 封面图片文件名 */
    int                favoriteCount;               /* 收藏量（计算项） */
    ArchiveStatus      status;                      /* 档案状态 */
    struct Archive    *next;                        /* 链表指针 */
} Archive;

/* 用户 */
typedef struct User {
    char               displayName[MAX_NAME_LEN];   /* 用户名（显示用） */
    char               account[MAX_ACCOUNT_LEN];    /* 账号（登录用，唯一） */
    char               password[MAX_PASSWORD_LEN];  /* 密码 */
    UserRole           role;                        /* 身份 */
    UserStatus         status;                      /* 用户状态 */
    struct User       *next;
} User;

/* 收藏记录 */
typedef struct Favorite {
    char               account[MAX_ACCOUNT_LEN];    /* 收藏者账号 */
    int                archiveId;                   /* 收藏的项目编号 */
    char               time[MAX_TIME_LEN];          /* 收藏时间 */
    struct Favorite   *next;
} Favorite;

/* 全局应用上下文 */
typedef struct AppContext {
    Archive  *archiveHead;     /* 档案链表头指针 */
    User     *userHead;        /* 用户链表头指针 */
    Favorite *favoriteHead;    /* 收藏链表头指针 */
    User     *currentUser;     /* 当前登录用户（未登录时为NULL） */
} AppContext;

/* ========== 枚举转字符串辅助函数（内联） ========== */

static inline const char* category_to_str(CultureCategory cat) {
    static const char *names[] = {
        "传统技艺", "传统音乐", "传统戏剧",
        "传统美术", "民俗", "传统医药"
    };
    if (cat >= 0 && cat < CAT_COUNT) return names[cat];
    return "未知";
}

static inline const char* protection_to_str(ProtectionLevel prot) {
    static const char *names[] = {
        "世界级", "国家级", "省级", "市级", "濒危"
    };
    if (prot >= 0 && prot < PROT_COUNT) return names[prot];
    return "未知";
}

static inline const char* digital_status_to_str(DigitalStatus ds) {
    static const char *names[] = {
        "已数字化", "数字化中", "未数字化"
    };
    if (ds >= 0 && ds < DIGI_COUNT) return names[ds];
    return "未知";
}

static inline const char* user_role_to_str(UserRole role) {
    return role == ROLE_ADMIN ? "管理员" : "普通用户";
}

static inline const char* user_status_to_str(UserStatus s) {
    static const char *names[] = { "正常", "封禁", "注销" };
    if (s >= 0 && s <= 2) return names[s];
    return "未知";
}

static inline const char* archive_status_to_str(ArchiveStatus s) {
    static const char *names[] = { "正常", "隐藏", "移除" };
    if (s >= 0 && s <= 2) return names[s];
    return "未知";
}

#endif /* MODELS_H */
