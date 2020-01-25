/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Function prototypes to translate to Assem-instructions using Maximal Munch.
 * @Date: 2020-01-06 15:19:59
 * @Last Modified: 2020-01-24 23:31:58
 */

#ifndef T_ASSEM_H_
#define T_ASSEM_H_
#include "temp.h"

typedef struct {Temp_labelList labels;} *AS_targets;
AS_targets AS_Targets(Temp_labelList labels);

//* Assem instruction without specific register
typedef struct AS_instr_ *AS_instr;
struct AS_instr_ {
  enum {I_OPER, I_LABEL, I_MOVE} kind;
  union {
    struct {string assem; Temp_tempList dst, src; AS_targets jumps;} OPER; /* Operator with src and dst */
    struct {string assem; Temp_label label;} LABEL; /* Jump to label */
    struct {string assem; Temp_tempList dst, src;} MOVE; /* Move data */
  } u;
};

//* Instruction for oper between src & dst
AS_instr AS_Oper(string a, Temp_tempList d, Temp_tempList s, AS_targets j);
//* Instruction for assem label
AS_instr AS_Label(string a, Temp_label label);
//* Instruction for moving data
AS_instr AS_Move(string a, Temp_tempList d, Temp_tempList s);

void AS_print(FILE *out, AS_instr i, Temp_map m);

typedef struct AS_instrList_ *AS_instrList;
struct AS_instrList_ { AS_instr head; AS_instrList tail;};
AS_instrList AS_InstrList(AS_instr head, AS_instrList tail);

AS_instrList AS_splice(AS_instrList a, AS_instrList b);
void AS_printInstrList (FILE *out, AS_instrList iList, Temp_map m);

typedef struct AS_proc_ *AS_proc;
struct AS_proc_ {
  string prolog;
  AS_instrList body;
  string epilog;
};

AS_proc AS_Proc(string p, AS_instrList b, string e);

#endif