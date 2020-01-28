/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Utils module header file.
 * @Date: 2019-10-08 15:44:46
 * @Last Modified: 2020-01-27 21:14:03
 */
#ifndef T_UTIL_H_
#define T_UTIL_H_

#include <assert.h>
typedef char *string;
typedef char bool;

#define TRUE 1
#define FALSE 0
#define BUFSIZE 512

void *checked_malloc(int len);

string String(char *);
string String_format(const char* s, ...);

typedef struct U_boolList_ *U_boolList;
struct U_boolList_ {bool head; U_boolList tail;};
U_boolList U_BoolList(bool head, U_boolList tail);

#endif