/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Escape variable search module header file.
 * @Date: 2019-11-01 21:40:10
 * @Last Modified: 2020-02-24 18:18:21
 */
#ifndef T_ESCAPE_H_
#define T_ESCAPE_H_

#include "absyn.h"
#include "env.h"

S_table eenv = NULL;
void ESC_findEscape(A_exp e);

#endif