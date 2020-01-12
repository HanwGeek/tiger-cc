/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Stack frame prototype header file.
 * @Date: 2019-10-31 19:22:40
 * @Last Modified: 2020-01-12 15:16:43
 */
#ifndef T_FRAME_H_
#define T_FRAME_H_

#include "util.h"
#include "temp.h"
#include "tree.h"

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

typedef struct F_frag_ *F_frag;
//* Fragment of string or function machine code sequence
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

//* Create a new frame with function {name}
//* and a bool list represents the escapability of its formals
F_frame F_newFrame(Temp_label name, U_boolList formals);
//* Return formals in frame{f}
F_accessList F_formals(F_frame f);
//* Allocate local var access in frame{f}
F_access F_allocLocal(F_frame f, bool escape);

//* Current frame pointer reg
Temp_temp F_FP(void);
//* Const val of word size
extern const int F_WORD_SIZE;
//* Return the addr of F_access{acc}
T_exp F_Exp(F_access acc, T_exp framePtr);

//* Call external function
T_exp F_externalCall(string s, T_expList args);

#endif