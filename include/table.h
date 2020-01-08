/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Data structure table module header file.
 * @Date: 2019-10-21 13:40:42
 * @Last Modified: 2020-01-08 21:05:13
 */
#ifndef T_TABLE_H_
#define T_TABLE_H_

typedef struct TAB_table_ *TAB_table;

//* Make a new table mapping "keys" to "values"
TAB_table TAB_empty(void);

//* Enter the mapping "key" -> "value" into table "t"
void TAB_enter(TAB_table t, void *key, void *value);

//* Look up the most recent binding for "key" in table "t"
void *TAB_look(TAB_table t, void *key);

//* Pop the most recent binding and return its key
void *TAB_pop(TAB_table t);

//* Call "show" on every "key"->"value" pair in table
void TAB_dump(TAB_table t, void (*show)(void *key, void *value));

#endif