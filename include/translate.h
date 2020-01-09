/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Translate to IR module header file.
 * @Date: 2019-10-25 14:17:30
 * @Last Modified: 2019-12-09 20:19:08
 */
#ifndef T_TRANSLATE_H_
#define T_TRANSLATE_H_

#include "util.h"
#include "temp.h"
#include "absyn.h"
#include "frame.h"
#include <stdio.h>

//* Function call nested level 
//* {Tr_level parent; Temp_label name; F_frame frame; Tr_accessList formals;}
typedef struct Tr_level_ *Tr_level;
//* Translated access struct 
//* {Tr_level level; F_access access;}
typedef struct Tr_access_ *Tr_access;
//* Translated access list struct
typedef struct Tr_accessList_ *Tr_accessList;
//* Translated exp, including ex, nx and cx;
typedef struct Tr_exp_ *Tr_exp;
typedef struct Tr_expList_ *Tr_expList;

struct Tr_accessList_ {
  Tr_access head;
  Tr_accessList tail;
};

Tr_accessList Tr_AccessList(Tr_access head, Tr_accessList tail);

void Tr_procEntryExit(Tr_level level, Tr_exp body, Tr_accessList formals);
//* Get fragments
F_fragList Tr_getResult(void);

Tr_expList Tr_ExpList(void);
//* Prepend to explist head
Tr_expList Tr_ExpList_prepend(Tr_exp exp, Tr_expList list);
//* Append to explist tail
Tr_expList Tr_ExpList_append(Tr_exp exp, Tr_expList list);

//* Get the outermost level
Tr_level Tr_outermost(void);
Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals);

Tr_accessList Tr_formals(Tr_level level);
Tr_access Tr_allocLocal(Tr_level level, bool escape);

//* Init an empty exp with const 0
Tr_exp Tr_emptyExp(void);

//* Translate simple var
//* access->level: defintion level; level: var use level
Tr_exp Tr_simpleVar(Tr_access access, Tr_level level);
//* Translate field var
Tr_exp Tr_fieldVar(Tr_exp baseAddr, int fieldoffset);
//* Translate subscript var
Tr_exp Tr_subscriptVar(Tr_exp baseAddr, Tr_exp index);

//* Translate int
Tr_exp Tr_intExp(int n);
//* Translate string
Tr_exp Tr_stringExp(string s);
//* Translate record exp
Tr_exp Tr_recordExp(Tr_expList list, int n);
//* Translate array exp
Tr_exp Tr_arrayExp(Tr_exp size, Tr_exp init);
//* Translate nil exp
Tr_exp Tr_nilExp(void);
//* Translate arithmatic op exp, including plus, minus, time and div
Tr_exp Tr_arithOpExp(A_oper op, Tr_exp left, Tr_exp right);
//* Translate relation op exp, including lt, le, gt and ge
Tr_exp Tr_relOpExp(A_oper op, Tr_exp left, Tr_exp right);
//* Translate equal or non-equal expression
Tr_exp Tr_eqExp(A_oper op, Tr_exp left, Tr_exp right);
Tr_exp Tr_eqStringExp(A_oper op, Tr_exp left, Tr_exp right);
Tr_exp Tr_eqRef(A_oper op, Tr_exp left, Tr_exp right);

//* Translate expression sequence
Tr_exp Tr_seqExp(Tr_expList list);
//* Translate assign expression
Tr_exp Tr_assignExp(Tr_exp var, Tr_exp exp);
//* Translate call expression
Tr_exp Tr_callExp(Tr_level level, Tr_level funLevel, Temp_label name, Tr_expList argList);
//* Translate if-condition expression
Tr_exp Tr_ifCondExp(Tr_exp cond, Tr_exp then, Tr_exp elsee);
//* Translate while expression
Tr_exp Tr_whileExp(Tr_exp cond, Tr_exp done, Tr_exp body);
//* Translate done expression
Tr_exp Tr_doneExp(void);
//* Translate for expression
Tr_exp Tr_forExp();
//* Translate break expression
Tr_exp Tr_breakExp(Tr_exp breakk);

#endif