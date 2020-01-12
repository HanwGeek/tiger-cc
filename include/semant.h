/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Semantic translate & check module header file.
 * @Date: 2019-10-25 13:44:32
 * @Last Modified: 2020-01-12 10:42:25
 */
#ifndef T_SEMANT_H_
#define T_SEMANT_H_

#include "util.h"
#include "absyn.h"
#include "types.h"
#include "translate.h"

//* expty: Translated exp of IR & its type
struct expty {Tr_exp exp; Ty_ty ty;};

F_fragList SEM_transProg(A_exp exp);

#endif