/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Memory temp var module.
 * @Date: 2019-10-31 19:37:48
 * @Last Modified: 2020-02-13 21:48:14
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

string Temp_look(Temp_map m, Temp_temp t) {
  assert(m && m->tab);
  string s;
  s = TAB_look(m->tab, t);
  if (s) return s;
  else if (m->under) return Temp_look(m->under, t);
  else return NULL;
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
