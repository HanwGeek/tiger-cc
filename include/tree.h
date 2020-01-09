/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Definitions for intermediate representation (IR) trees.
 * @Date: 2019-11-04 21:54:03
 * @Last Modified: 2019-11-09 13:58:37
 */
#ifndef T_TREE_H_
#define T_TREE_H_
#include "temp.h"

//* IR without return value
typedef struct T_stm_ *T_stm;
//* IR with return value
typedef struct T_exp_ *T_exp;
typedef struct T_stmList_ *T_stmList;
typedef struct T_expList_ *T_expList;

struct T_stmList_ {T_stm head; T_stmList tail;};
struct T_expList_ {T_exp head; T_expList tail;};

typedef enum {
  T_plus, T_minus, T_mul, T_div,
  T_and, T_or, T_lshift, T_rshift, T_arshift, T_xor
} T_binOp;

typedef enum {
  T_eq, T_ne, T_lt, T_gt, T_le, T_ge,
  T_ult, T_ule, T_ugt, T_uge
} T_relOp;

struct T_stm_ {
  enum {
    T_SEQ,   //* stm sequence
    T_LABEL, //* machine code addr label
    T_JUMP,  //* control shift jump
    T_CJUMP, //* conditional jump
    T_MOVE,  //* move contents to mem or reg addr
    T_EXP    //* eval exp without result
  } kind;
  union {
    struct {T_stm left, right;} SEQ;
    Temp_label LABEL;
    struct {T_exp exp; Temp_labelList jumps;} JUMP;
    struct {T_relOp op; T_exp left, right; Temp_label truee, falsee;} CJUMP;
    struct {T_exp dst, src;} MOVE;
    T_exp EXP;
  } u;
};

//* the eval of value
struct T_exp_ {
  enum {T_BINOP, //* BINOP(o, e1, e2): bin oper on e1 & e2
        T_MEM,   //* contents begins at address{e} of wordSize
        T_TEMP,  //* temp var
        T_ESEQ,  //* ESEQ(s, e)
        T_NAME,  //* label const
        T_CONST, //* int const
        T_CALL   //* process call
  } kind;
  union {
    struct {T_binOp op; T_exp left, right;} BINOP;
    T_exp MEM;
    Temp_temp TEMP;
    struct {T_stm stm; T_exp exp;} ESEQ;
    Temp_label NAME;
    int CONST;
    struct {T_exp fun; T_expList args;} CALL;
  } u;
};

T_expList T_ExpList(T_exp head, T_expList tail);
T_stmList T_StmList(T_stm head, T_stmList tail);

T_stm T_Seq(T_stm left, T_stm right);
T_stm T_Label(Temp_label label);
T_stm T_Jump(T_exp exp, Temp_labelList labels);
T_stm T_Cjump(T_relOp op, T_exp left, T_exp right, 
	      Temp_label truee, Temp_label falsee);
T_stm T_Move(T_exp dst, T_exp src );
T_stm T_Exp(T_exp exp);

T_exp T_Binop(T_binOp, T_exp, T_exp);
T_exp T_Mem(T_exp);
T_exp T_Temp(Temp_temp);
T_exp T_Eseq(T_stm, T_exp);
T_exp T_Name(Temp_label);
T_exp T_Const(int);
T_exp T_Call(T_exp, T_expList);

T_relOp T_notRel(T_relOp);
T_relOp T_commute(T_relOp);

#endif