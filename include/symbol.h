/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Symbol defs and table header file.
 * @Date: 2019-10-21 14:50:01
 * @Last Modified: 2020-01-08 21:06:54
 */
#ifndef T_SYMBOL_H_
#define T_SYMBOL_H_

#include "util.h"
#include "table.h"

typedef struct S_symbol_ *S_symbol;

//* Make a unique symbol from a given string
S_symbol S_Symbol(string name);

//* Extract the underlying string from a symbol
string S_name(S_symbol sym);

//* S_table is a mapping from S_symbol to any
//* Where "any" is represented here by void*
typedef struct TAB_table_ *S_table;

//* Make a new table
S_table S_empty(void);

//* Enter a binding sym->value into "t"
//* shadowing but not deleting the previous one
void S_enter(S_table t, S_symbol sym, void *value);

//* Look up the most recent binding of "sym" in "t"
void *S_look(S_table t, S_symbol sym);

//* Start a new "scope" in "t", scope are nested
void S_beginScope(S_table t);

//* Remove any bindings entered since the current scope
//* began and end the current scope
void S_endScope(S_table t);

//* Call "show" on every "key"->"value" pair in symbol table
void S_dump(S_table t, void (*show)(S_symbol sym, void *binding));

#endif