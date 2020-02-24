/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Memory temp var module.
 * @Date: 2019-10-31 19:37:48
 * @Last Modified: 2020-02-24 11:16:36
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"
#include "symbol.h"
#include "temp.h"
#include "table.h"

//* temp: Local variable name
//* label: static memory address name
struct Temp_temp_ {int num;};

static int labels = 0;
static int temps = 128;

Temp_temp Temp_newtemp(void) {
  Temp_temp p = (Temp_temp)checked_malloc(sizeof(*p));
  p->num = temps++;
  char r[16];
  sprintf(r, "%d", p->num);
  Temp_enter(Temp_name(), p, String(r));
  return p;
}

int Temp_tempnum(Temp_temp t) {
  return t->num;
}

Temp_label Temp_newlabel(void) {
  char buf[128];
  sprintf(buf, "L%d", labels++);
  return Temp_namedlabel(String(buf));
}

Temp_label Temp_namedlabel(string name) {
  return S_Symbol(name);
}

string Temp_labelstring(Temp_label s) {
  return S_name(s);
}

TL_table TL_empty(void) {
  return TAB_empty();
}

void TL_enter(TL_table t, Temp_label label, void *v) {
  return TAB_enter(t, label, v);
}

void* TL_look(TL_table t, Temp_label label) {
  return TAB_look(t, label);
}

Temp_tempList Temp_TempList(Temp_temp head, Temp_tempList tail) {
  Temp_tempList p = (Temp_tempList)checked_malloc(sizeof(*p));
  p->head = head; p->tail = tail;
  return p;
}

Temp_labelList Temp_LabelList(Temp_label head, Temp_labelList tail) {
  Temp_labelList p = (Temp_labelList)checked_malloc(sizeof(*p));
  p->head = head; p->tail = tail;
  return p;
}

struct Temp_map_ {TAB_table tab; Temp_map under;};
Temp_map newMap(TAB_table tab, Temp_map under) {
  Temp_map m = checked_malloc(sizeof(*m));
  m->tab = tab; m->under = under;
  return m;
}

Temp_map Temp_empty(void) {
  return newMap(TAB_empty(), NULL);
}

Temp_map Temp_layerMap(Temp_map over, Temp_map under) {
  if (over == NULL) return under;
  else return newMap(over->tab, Temp_layerMap(over->under, under));
}

void Temp_enter(Temp_map m, Temp_temp t, string s) {
  assert(m && m->tab);
  TAB_enter(m->tab, t, s);
}


Temp_temp Temp_pop(Temp_map m) {
  return TAB_pop(m->tab);
}

string Temp_look(Temp_map m, Temp_temp t) {
  assert(m && m->tab);
  string s;
  s = TAB_look(m->tab, t);
  if (s) return s;
  else if (m->under) return Temp_look(m->under, t);
  else return NULL;
}

Temp_temp Temp_popMap(Temp_map m) {
  if(!m) return NULL;
  Temp_temp t = TAB_pop(m->tab);
  if (t) return t;
  if (m->under) return Temp_popMap(m->under);
  return NULL;
}

static FILE *outfile;
void showit(Temp_temp t, string r) {
  fprintf(outfile, "t%d => %s\n", t->num, r);  
}

void Temp_dumpMap(FILE *out, Temp_map m) {
  outfile = out;
  TAB_dump(m->tab, (void (*)(void *, void *))showit);
  if (m->under) {
    fprintf(out, "----------\n");
    Temp_dumpMap(out, m->under);
  }
}

Temp_map Temp_name(void) {
  static Temp_map m = NULL;
  if (!m) m = Temp_empty();
  return m;
}

void Temp_assignList(Temp_tempList list1, Temp_tempList list2) {
  while (!list1 && !list2) {
    list1->head = list2->head;
    list1 = list1->tail;
    list2 = list2->tail;
  }
  if (!list1) list1 = NULL;
  if (!list2) list1 = list2;
}

void Temp_enterList(Temp_tempList list, Temp_temp t) {
  Temp_tempList cur = list, prev = NULL;
  while (cur && Temp_tempnum(cur->head) < Temp_tempnum(t)) {
    prev = cur;
    cur = cur->tail;
  }
  prev->tail = Temp_TempList(t, cur);
}

Temp_tempList Temp_removeList(Temp_tempList list, Temp_temp t) {
  Temp_tempList cur = list, prev = NULL;
  while (cur && cur->head != t) {
    prev = cur;
    cur = cur->tail;
  }
  prev->tail = cur->tail;
  //? Memory Leak
  return list;
}

Temp_tempList Temp_mergeList(Temp_tempList list1, Temp_tempList list2) { 
  Temp_tempList p = Temp_TempList(NULL, NULL), prev = p;
  while (list1 && list2) {
    int num1 = Temp_tempnum(list1->head), num2 = Temp_tempnum(list2->head);
    if (num1 < num2) {
      prev->tail = list1;
      list1 = list1->tail;
    } else if (num1 > num2) {
      prev->tail = list2;
      list2 = list2->tail;
    } else {
      prev->tail = list1;
      list1 = list1->tail;
      list2 = list2->tail;
    }
    prev = prev->tail;
  }
  prev->tail = list1 == NULL ? list2 : list1;
  return p->tail;
}

Temp_tempList Temp_substractList(Temp_tempList list1, Temp_tempList list2) {
  Temp_tempList retList = NULL, tailList = NULL;
  while (!list1 && !list2) {
    int num1 = Temp_tempnum(list1->head), num2 = Temp_tempnum(list2->head);
    if (num1 < num2) {
      if (!retList) {
        retList = Temp_TempList(list1->head, NULL);
        tailList = retList->tail;
      }
      else {
        tailList = Temp_TempList(list1->head, NULL);
        tailList = tailList->tail;
      }
    }
    if (num1 == num2) {
      list1 = list1->tail;
      list2 = list2->tail;
    }
    if (num1 > num2) {
      list2 = list2->tail;
    }
  }
  tailList = list1;
  return retList;
}

Temp_tempList Temp_joinList(Temp_tempList list1, Temp_tempList list2) {
  if (!list1 || list1 == list2) assert(0);
  Temp_tempList cur = list1;
  while (cur->tail) cur = cur->tail;
  cur->tail = list2;
  return list1;
}

bool Temp_isequalList(Temp_tempList list1, Temp_tempList list2) {
  while (!list1 && !list2) {
    if (list1->head != list2->head) return FALSE;
    list1 = list1->tail; list2 = list2->tail;
  }
  if (list1 || list2) return FALSE;
  return TRUE;
}