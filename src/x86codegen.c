/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Codegen module for x86.
 * @Date: 2020-01-06 16:44:12
 * @Last Modified: 2020-01-25 14:40:01
 */
#include <stdlib.h>
#include "codegen.h"
#include "temp.h"
#include "util.h"

static AS_instrList iList = NULL, last = NULL;

//* Register instruction to list
static void emit(AS_instr inst);
static void munchStm(T_stm s);
static Temp_temp munchExp(T_exp e);
static Temp_tempList munchArgs();

static F_frame CODEGEN_frame = NULL;

static void emit(AS_instr inst) {
  if (last != NULL) last = last->tail = AS_InstrList(inst, NULL);
  else last = iList = AS_InstrList(inst, NULL);
}

static void munchStm(T_stm s) {
  switch (s->kind) {
    case T_MOVE: {
      T_exp dst = s->u.MOVE.dst, src = s->u.MOVE.src;
      if (dst->kind = T_MEM) {
        if (dst->u.MEM->kind == T_BINOP &&
            dst->u.MEM->u.BINOP.op == T_plus &&
            dst->u.MEM->u.BINOP.right->kind == T_CONST) {
              //* MOVE(MEM(BINOP(PLUS, e1, CONST(n))), e2)
              T_exp e1 = dst->u.BINOP.left, e2 = src;
              emit(AS_Move(String_format("mov [`s0 + %d], `s1\n", dst->u.MEM->u.CONST), NULL, 
                    Temp_TempList(munchExp(e1), 
                      Temp_TempList(munchExp(e2), NULL))));
            } else if (dst->u.MEM->kind == T_BINOP &&
                       dst->u.MEM->u.BINOP.op == T_plus &&
                       dst->u.MEM->u.BINOP.left == T_CONST) {
              //* MOVE(MEM(BINOP(PLUS, CONST(n), e1)), e2)
              T_exp e1 = dst->u.MEM->u.BINOP.right, e2 = src;
              emit(AS_Move(String_format("mov [`s0 + %d], `s1\n", dst->u.MEM->u.BINOP.left->u.CONST), NULL, 
                    Temp_TempList(munchExp(e1),
                      Temp_TempList(munchExp(e2), NULL))));
            } else if (src->kind == T_MEM) {
              //* MOVE(MEM(e1), MEM(e2))
              T_exp e1 = dst->u.MEM, e2 = src->u.MEM;
              emit(AS_Move("mov [`s0] `s1\n", NULL,
                    Temp_TempList(munchExp(e1),
                      Temp_TempList(munchExp(e2), NULL))));
            } else {
              //* MOVE(MEM(e1), e2)
              T_exp e1 = dst->u.MEM, e2 = src;
              emit(AS_Move("mov [`s0] `s1\n", NULL,
                    Temp_TempList(munchExp(e1), 
                      Temp_TempList(munchExp(e2), NULL))));
            }
      } else if (dst->kind = T_TEMP) {
        emit(AS_Move("mov  `d0, `s0\n", Temp_TempList(munchExp(dst), NULL),
              Temp_TempList(munchExp(src), NULL)));
      } else assert(0);
      break;
    }
    case T_SEQ: {
      munchStm(s->u.SEQ.left); munchStm(s->u.SEQ.right);
      break;
    }
    case T_LABEL: {
      emit(AS_Label(String_format("%s:\n", Temp_labelstring(s->u.LABEL)), s->u.LABEL));
      break;
    }
    case T_JUMP: {
      Temp_temp t = munchExp(s->u.JUMP.exp);
      emit(AS_Oper("jmp `d0\n", Temp_TempList(t, NULL), NULL,
            AS_Targets(s->u.JUMP.jumps)));
      break;                   
    }
    case T_CJUMP: {
      Temp_temp l = munchExp(s->u.CJUMP.left), r = munchExp(s->u.CJUMP.right);
      emit(AS_Oper("cmp `s0, `s1\n", NULL,
            Temp_TempList(l, 
              Temp_TempList(r, NULL)), NULL));
      char* instr = NULL;
      switch (s->u.CJUMP.op) {
        case T_eq: instr = "je";  break;
        case T_ne: instr = "jne"; break;
        case T_lt: instr = "jl";  break;
        case T_gt: instr = "jg";  break;
        case T_le: instr = "jle"; break;
        case T_ge: instr = "jge"; break;
        default: assert(0);
      }
      emit(AS_Oper(String_format("%s `j0\n", instr), NULL, NULL,
            AS_Targets(Temp_LabelList(s->u.CJUMP.truee, NULL))));
      break;
    }
    case T_EXP: {
      munchExp(s->u.EXP);
      break;
    }
    default: assert(0);
  }
}

static Temp_temp munchExp(T_exp e) {
  switch (e->kind) {
    case T_BINOP: {
      char *op = NULL, *sign = NULL;
      switch (e->u.BINOP.op) {
        case T_plus:  op = "add"; sign = "+"; break;
        case T_minus: op = "sub"; sign = "-"; break;
        case T_mul:   op = "mul"; sign = "*"; break;
        case T_div:   op = "div"; sign = "/"; break;
        default: assert(0);
      }
      if (e->u.BINOP.left->kind == T_CONST) {
        //* BINOP(op, CONST(i), e2)
        Temp_temp t = Temp_newtemp();
        T_exp e2 = e->u.BINOP.right;
        emit(AS_Oper(String_format("%s, `d0, `s0%s%d\n", op, sign, e->u.BINOP.left->u.CONST),
              Temp_TempList(t, NULL), 
                Temp_TempList(munchExp(e2), NULL), NULL));
        return t;
      } else if (e->u.BINOP.right == T_CONST) {
        //* BINOP(op, e2, CONST(i));
        Temp_temp t = Temp_newtemp();
        T_exp e2 = e->u.BINOP.left;
        emit(AS_Oper(String_format("%s, `d0, `s0%s%d\n", op, sign, e->u.BINOP.right->u.CONST),
              Temp_TempList(t, NULL),
                Temp_TempList(munchExp(e2), NULL), NULL));
        return t;
      } else {
        //* BINOP(op, e1, e2)
        Temp_temp t = Temp_newtemp();
        T_exp e1 = e->u.BINOP.left, e2 = e->u.BINOP.right;
        emit(AS_Oper(String_format("%s, `d0, `s0%s`s1\n", op, sign),
              Temp_LabelList(t, NULL),
                Temp_TempList(munchExp(e1), 
                  Temp_TempList(e2, NULL)), NULL));
        return t;
      }
      return NULL;
    }
    case T_MEM: {
      T_exp loc = e->u.MEM;
      if (loc->kind == T_BINOP && loc->u.BINOP.op == T_plus) {
        if (loc->u.BINOP.left->kind == T_CONST) {
          //* MEM(BINOP(PLUS, CONST(i), e2))
          Temp_temp t = Temp_newtemp();
          emit(AS_Move(String_format("mov `d0, [`s0 + %d]\n", loc->u.BINOP.left->u.CONST),
                Temp_TempList(t, NULL), 
                  Temp_TempList(munchExp(loc->u.BINOP.right), NULL)));
          return t;
        } else if (loc->u.BINOP.right->kind == T_CONST) {
          Temp_temp t = Temp_newtemp();
          emit(AS_Move(String_format("mov `s0, `[`d0 + %d]\n", loc->u.BINOP.right->u.CONST),
                        Temp_TempList(t, NULL),
                          Temp_TempList(munchExp(loc->u.BINOP.left), NULL)));
          return t;           
        } else assert(0);
      } else if (loc->kind == T_CONST) {
        //* MEM(CONST(i))
        Temp_temp t = Temp_newtemp;
        emit(AS_Move(String_format("mov `d0, [%d]\n", loc->u.CONST),
              Temp_TempList(t, NULL), NULL));
        return t;
      } else {
        //* MEM(e1)
        Temp_temp t = Temp_newtemp();
        T_exp e1 = loc->u.MEM;
        emit(AS_Move(String_format("mov `d0, [`s0]\n"), 
              Temp_TempList(t, NULL), 
                Temp_TempList(munchExp(e1), NULL)));
        return t;
      }
    } 
    case T_TEMP: {
      return e->u.TEMP;
    }
    case T_ESEQ: {
      munchStm(e->u.ESEQ.stm);
      return munchExp(e->u.ESEQ.exp);
    }
    case T_NAME: {
      //* NAME(n)
      Temp_temp t = Temp_newtemp();
      //TODO:
      return t;
    }
    case T_CONST: {
      //* CONST(i)
      Temp_temp t = Temp_newtemp();
      emit(AS_Move(String_format("mov `d0, %d\n", e->u.CONST),
                   Temp_TempList(t, NULL), NULL));
      return t;
    }
    case T_CALL: {
      Temp_temp t = munchExp(e->u.CALL.fun);
      Temp_tempList plist = munchArgs(0, e->u.CALL.args,
                                      F_formals(CODEGEN_frame));
      emit(AS_Oper("call `s0\n", F_caller_saves(),
            Temp_TempList(t, NULL), NULL));
    }
    default: assert(0);
  }
}

static char *reg_names[] = {"eax", "ebx", "ecx", "edx", "edi", "esi"};
static unsigned int reg_count = 0;
static Temp_tempList munchArgs(unsigned int n, T_expList eList, F_accessList formals) {
  if (!CODEGEN_frame) assert(0);
  if (!eList || !formals) return NULL;

  Temp_tempList tlist = munchArgs(n + 1, eList->tail, formals->tail);
  Temp_temp e = munchExp(eList->head);
  if (F_isEscape(formals->head)) {
    emit(AS_Oper("push `s0\n", NULL,
          Temp_TempList(e, NULL), NULL));
  } else {
    emit(AS_Move(String_format("mov %s, `s0", reg_names[reg_count++]), NULL,
          Temp_TempList(e, NULL)));
  }
  return Temp_TempList(e, tlist);
}

AS_instrList F_codegen(F_frame f, T_stmList stmList) {
  
}