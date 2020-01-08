/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Utils module header file.
 * @Date: 2019-10-08 15:44:46
 * @Last Modified: 2019-10-25 14:01:13
 */
#include <assert.h>

#ifndef T_UTIL_H_
#define T_UTIL_H_
typedef char *string;
typedef char bool;

#define TRUE 1
#define FALSE 0

void *checked_malloc(int);
string String(char *);

typedef struct U_boolList_ *U_boolList;
struct U_boolList_ {bool head; U_boolList tail;};
U_boolList U_BoolList(bool head, U_boolList tail);

#endif