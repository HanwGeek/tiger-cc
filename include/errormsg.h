/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Error message module header file.
 * @Date: 2019-10-08 15:45:53
 * @Last Modified: 2020-01-08 21:07:08
 */
#ifndef T_ERRMSG_H_
#define T_ERRMSG_H_
#include "util.h"

extern bool EM_anyErrors;

void EM_newline(void);

extern int EM_tokPos;

void EM_error(int, string,...);
void EM_impossible(string,...);
void EM_reset(string filename);

#endif