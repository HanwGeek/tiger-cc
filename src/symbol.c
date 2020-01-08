/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Var and func symbol defs and table module.
 * @Date: 2019-10-21 16:05:17
 * @Last Modified: 2020-01-08 21:09:19
 */
#include <stdio.h>
#include <string.h>
#include "util.h"
#include "table.h"
#include "symbol.h"

#ifndef SIZE
#define SIZE 109 // should be prime
#endif

typedef struct S_symbol_ *S_symbol;
struct S_symbol_ {string name; S_symbol next;};

static S_symbol mksymbol(string name, S_symbol next) {
  S_symbol s = checked_malloc(sizeof(*s));
  //! String copy bug
  s->name = strdup(name); s->next = next; 
  return s;
}

static S_symbol hashtable[SIZE];

// * hash function mapping string to int index
static unsigned int hash(char *s0) {
  unsigned int h = 0;
  for (char *s = s0; *s; s++)
    h = h * 65599 + *s;
  return h;
}

static int streq(string a, string b) {
  return !strcmp(a, b);
}

//* Make a unique symbol from a given string
S_symbol S_Symbol(string name) {
  int index = hash(name) % SIZE;
  S_symbol syms = hashtable[index], sym;
  for (sym = syms; sym; sym = sym->next)
    if (streq(sym->name, name)) return sym;
  sym = mksymbol(name, syms);
  hashtable[index] = sym;
  return sym;
}

//* Extract the underlying string from a symbol
string S_name(S_symbol sym) {
  return sym->name;
}

//* Make a new table
S_table S_empty(void) {
  return TAB_empty();
}

//* Enter a binding sym->value into "t"
//* shadowing but not deleting the previous one
void S_enter(S_table t, S_symbol sym, void *value) {
  TAB_enter(t, sym, value);
}

//* Look up the most recent binding of "sym" in "t"
void *S_look(S_table t, S_symbol sym) {
  return TAB_look(t, sym);
}

static struct S_symbol_ marksym = {"<mark>", 0};

//* Start a new "scope" in "t", scope are nested
void S_beginScope(S_table t) {
  S_enter(t, &marksym, NULL);
}

//* Remove any bindings entered since the current scope
//* began and end the current scope
void S_endScope(S_table t) {
  S_symbol s;
  do s = TAB_pop(t);
  while (s != &marksym);
}

//* Call "show" on every "key"->"value" pair in symbol table
void S_dump(S_table t, void (*show)(S_symbol sym, void *binding)) {
  TAB_dump(t, (void (*)(void *, void *)) show);
}