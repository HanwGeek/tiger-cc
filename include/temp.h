/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Memory temp var module header file.
 * @Date: 2019-10-31 19:21:13
 * @Last Modified: 2020-02-13 21:47:56
 */
#ifndef T_TEMP_H_
#define T_TEMP_H_

#include <stdio.h>
#include "util.h"
#include "symbol.h"

//* Table type mapping labels to infomation
typedef struct TAB_table_ *TL_table;
//* temp: Local variable name in register
typedef struct Temp_temp_ *Temp_temp;
//* Return a new temp
Temp_temp Temp_newtemp(void);
//* Return the number of temp
int Temp_tempnum(Temp_temp t);
//* label: static memory address name
typedef S_symbol Temp_label;
//* Return a new label
Temp_label Temp_newlabel(void);
//* Given a asm name {string} return a label
Temp_label Temp_namedlabel(string name);
//* Return the temp label name
string Temp_labelstring(Temp_label s);

TL_table TL_empty(void);
void TL_enter(TL_table t, Temp_label label, void *v);
void* TL_look(TL_table t, Temp_label label);

typedef struct Temp_tempList_ *Temp_tempList;
struct Temp_tempList_ {Temp_temp head; Temp_tempList tail;};
Temp_tempList Temp_TempList(Temp_temp head, Temp_tempList tail);

typedef struct Temp_labelList_ *Temp_labelList;
struct Temp_labelList_ {Temp_label head; Temp_labelList tail;};
Temp_labelList Temp_LabelList(Temp_label head, Temp_labelList tail);

//* TAB_table list
typedef struct Temp_map_ *Temp_map;
//* Create an empty map
Temp_map Temp_empty(void);
Temp_map Temp_layerMap(Temp_map over, Temp_map under);
void Temp_enter(Temp_map m, Temp_temp t, string s);
string Temp_look(Temp_map m, Temp_temp t);
void Temp_dumpMap(FILE *out, Temp_map m);
//* Create an empty map
Temp_map Temp_name(void);

#endif