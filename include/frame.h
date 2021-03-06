/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Stack frame prototype header file.
 * @Date: 2019-10-31 19:22:40
 * @Last Modified: 2020-03-07 18:36:25
 */
#ifndef T_FRAME_H_
#define T_FRAME_H_

#include "util.h"
#include "temp.h"
#include "tree.h"
#include "assem.h"

//* Memeory stack unit, including
//* all formal access, instruction to 'view shift'
//* size of frame allocated, function start label
//* {Temp_label name; F_accessList formals; int local_count;}
typedef struct F_frame_ *F_frame;

//* Access of fmal param or local var in frame or register
//* {enum {inFrame, inReg} kind; union {int offset; Temp_temp reg;} u;}
typedef struct F_access_ *F_access;

typedef struct F_accessList_ *F_accessList;
struct F_accessList_ {F_access head; F_accessList tail;};


//* Fragment of string or function machine code sequence
//* {F_stringFrag, F_procFrag} kind;
typedef struct F_frag_ *F_frag;
struct F_frag_ {
  enum {F_stringFrag, F_procFrag} kind;
  union {
    struct {Temp_label label; string str;} stringg;
    struct {T_stm body; F_frame frame;} proc;
  } u;
};

F_frag F_StringFrag(Temp_label label, string str);
F_frag F_ProcFrag(T_stm body, F_frame frame);

typedef struct F_fragList_ *F_fragList;
struct F_fragList_ {F_frag head; F_fragList tail;};
F_fragList F_FragList(F_frag head, F_fragList tail);

extern Temp_map F_tempMap;
Temp_tempList F_registers(void);
string F_getlabel(F_frame frame);
//* Return the addr of F_access{acc}
T_exp F_Exp(F_access acc, T_exp framePtr);
//* Allocate local var access in frame{f}
F_access F_allocLocal(F_frame f, bool escape);
//* Return formals in frame{f}
F_accessList F_formals(F_frame f);
int F_inFrameOffset(F_access access);
//* Return name of frame
Temp_label F_name(F_frame f);
//* Const val of word size
extern const int F_WORD_SIZE;
//* Current frame pointer reg
Temp_temp F_FP(void);
//* Current stack pointer reg
Temp_temp F_SP(void);

Temp_temp F_ZERO(void);
//* Return value of function temp reg
Temp_temp F_RV(void);
//* Return address of function temp reg
Temp_temp F_RA(void);

//* Create a new frame with function {name}
//* and a bool list represents the escapability of its formals
F_frame F_newFrame(Temp_label name, U_boolList formals);
//* Call external function
T_exp F_externalCall(string s, T_expList args);
//* Whether a F_access is escape or not
bool F_isEscape(F_access access);
//* Return callee saved regs 
Temp_tempList F_callee_saves(void);
//* Return caller saved regs
Temp_tempList F_caller_saves(void);

string F_string(F_frag str) {
  
}

//* Generate prolog & epilog of proc
T_stm F_procEntryExit1(F_frame frame, T_exp body);

AS_instrList F_procEntryExit2(AS_instrList body);

AS_proc F_procEntryExit3(F_frame frame, AS_instrList body);

#endif