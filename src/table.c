/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Data structure table and k-v binder module.
 * @Date: 2019-10-21 15:47:28
 * @Last Modified: 2020-02-20 13:58:37
 */
#include <stdio.h>
#include "util.h"
#include "table.h"

#ifndef TABSIZE
#define TABSIZE 127
#endif

//* k-v binder list 
typedef struct binder_ *binder;
struct binder_ {void *key; void *value; binder next; void *prevtop;};
struct TAB_table_ {
  binder table[TABSIZE];
  void *top;
};

static binder Binder(void *key, void *value, binder next, void *prevtop) {
  binder b = checked_malloc(sizeof(*b));
  b->key = key; b->value = value;
  b->next = next; b->prevtop = prevtop;
  return b;
}

//* Make a new table mapping "keys" to "values"
TAB_table TAB_empty(void) {
  TAB_table t = checked_malloc(sizeof(*t));
  t->top = NULL;
  for (int i = 0; i < TABSIZE; i++)
    t->table[i] = NULL;
  return t;
}

//* Enter the mapping "key" -> "value" into table "t"
void TAB_enter(TAB_table t, void *key, void *value) {
  assert(t && key);
  int index = ((unsigned)key) % TABSIZE;
  t->table[index] = Binder(key, value, t->table[index], t->top);
  t->top = key;
}

//* Look up the most recent binding for "key" in table "t"
void *TAB_look(TAB_table t, void *key) {
  assert(t && key);
  int index = ((unsigned)key) % TABSIZE;
  for (binder b = t->table[index]; b; b = b->next)
    if (b->key == key) return b->value;
  return NULL;
}

//* Pop the most recent binding and return its key
void *TAB_pop(TAB_table t) {
  assert(t);
  void *k = t->top;
  if (k == NULL) return NULL;
  int index = ((unsigned)k) % TABSIZE;
  binder b = t->table[index];
  assert(b);
  t->table[index] = b->next;
  t->top = b->prevtop;
  return b->key;
}

//* Call "show" on every "key"->"value" pair in table
void TAB_dump(TAB_table t, void (*show)(void *key, void *value)) {
  void *k = t->top;
  int index = ((unsigned)k) % TABSIZE;
  binder b = t->table[index];
  if (b == NULL) return;
  t->table[index] = b->next;
  t->top = b->prevtop;
  show(b->key, b->value);
  TAB_dump(t, show);
  assert(t->top == b->prevtop && t->table[index] == b->next);
  t->top = k;
  t->table[index] = b;
}