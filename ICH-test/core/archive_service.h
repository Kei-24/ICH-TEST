#ifndef ARCHIVE_SERVICE_H
#define ARCHIVE_SERVICE_H

/*
 * archive_service.h - 档案业务逻辑层
 * 负责增删改查 / 搜索 / 排序 / 统计
 * 每次变更后自动保存文件 + 写日志
 */

#include "models.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ===== 增删改 ===== */

/* 添加单个档案，自动分配ID，成功后保存+日志，返回新ID，失败返回-1 */
int       archive_add(Archive *data);

/* 修改档案，按id定位，成功后保存+日志，返回1成功/0失败 */
int       archive_update(int id, Archive *newData);

/* 软删除档案（设status=ASTAT_REMOVED），成功后保存+日志，返回1/0 */
int       archive_delete(int id);

/* ===== 查询 ===== */

/* 按ID精确查询（仅返回status==NORMAL的），未找到返回NULL */
Archive*  archive_query_by_id(int id);

/* 模糊搜索（strstr），返回指针数组，*count返回匹配数。仅搜索NORMAL的 */
/* 调用者需free返回的数组，但不要free数组中的元素 */
Archive** archive_search_by_name(const char *keyword, int *count);

/* ===== 排序 ===== */

/* 按国际热度指数降序，返回指针数组（仅NORMAL），*count返回数量 */
Archive** archive_sort_by_heat(int *count);

/* 按综合传播热度(heatIndex*favoriteCount)降序，返回指针数组，*count返回数量 */
Archive** archive_sort_by_composite_heat(int *count);

/* ===== 统计 ===== */

/* 各类别档案数量统计（仅NORMAL），结果存入result[CAT_COUNT] */
void      archive_stat_category(int result[]);

/* 有效档案总数（status==NORMAL） */
int       archive_stat_total_active(void);

/* ===== 输入校验 ===== */

/* 校验文本字段是否包含管道符|，返回1=合法/0=含非法字符 */
int       archive_validate_text(const char *text);

#ifdef __cplusplus
}
#endif

#endif /* ARCHIVE_SERVICE_H */
