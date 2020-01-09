/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Tranlate to IR module.
 * @Date: 2019-10-31 21:34:35
 * @Last Modified: 2019-12-10 17:30:14
 */
#include "translate.h"
#include "absyn.h"
#include "frame.h"
#include "tree.h"
#include "temp.h"
#include "util.h"

typedef struct patchList_ *patchList;
struct Tr_level_ {
  Tr_level parent;
  Temp_label name;
  F_frame frame;
  Tr_accessList formals;
};
struct Tr_access_ {Tr_level level; F_access access;};

//* Table of label need to be filled
struct patchList_ {Temp_label *head; patchList tail;};

//* Condition transfer expression
struct Cx {patchList trues, falses; T_stm stm;};

struct Tr_exp_ {
  enum {
    Tr_ex, //* expression
    Tr_nx, //* void return
    Tr_cx  //* condition transfer
  } kind;
  union {T_exp ex; T_stm nx; struct Cx cx;} u;
};

struct Tr_expList_ {Tr_exp head; Tr_expList tail;};

//* ---------- Static methods ----------
static patchList PatchList(Temp_label *head, patchList tail);
//* Fill patchList{tList} with {label}
static void doPatch(patchList tList, Temp_label label);
//* Join two patchList
static patchList joinPatch(patchList first, patchList second);

//* Transform Tr_Exp{e} to Ex
static T_exp unEx(Tr_exp e);
//* Transform Tr_Exp{e} to Nx
static T_stm unNx(Tr_exp e);
//* Transform Tr_Exp{e} to Cx
static struct Cx unCx(Tr_exp e);


static Tr_exp Tr_Ex(T_exp ex);
static Tr_exp Tr_Nx(T_exp nx);
static Tr_exp Tr_Cx(T_stm stm, patchList trues, patchList falses);

static Tr_level outer = NULL;
static Tr_exp Tr_StaticLink(Tr_level level, Tr_level funLevel);
static Tr_access Tr_Access(Tr_level, F_access);
static Tr_accessList makeFormalAccessList(Tr_level);

static patchList PatchList(Temp_label *head, patchList tail) {
  patchList p = checked_malloc(sizeof(*p));
  p->head = head; p->tail = tail;
  return p;
}

static void doPatch(patchList tList, Temp_label label) {
  for (; tList; tList = tList->tail)
    *(tList->head) = label;
}

static patchList joinPatch(patchList first, patchList second) {
  if (!first) return second;
  for (; first->tail; first = first->tail);
  first->tail = second;
  return first;
}

static T_exp unEx(Tr_exp e) {
  switch (e->kind)
  {
    case Tr_ex: {
      return e->u.ex;
    }
    case Tr_nx: {
      return T_Eseq(e->u.nx, T_Const(0));
    }
    case Tr_cx: {
      Temp_temp r = Temp_newtemp();
      Temp_label t = Temp_newlabel(), f = Temp_newlabel();
      doPatch(e->u.cx.trues, t);
      doPatch(e->u.cx.falses, f);
      return T_Eseq(T_Move(T_Temp(r), T_Const(1)),
              T_Eseq(e->u.cx.stm, 
                T_Eseq(T_Label(f),
                  T_Eseq(T_Move(T_Temp(r), T_Const(0)), 
                    T_Eseq(T_Label(t), T_Temp(r))))));
    }
    assert(0); return NULL;
  }
}


static T_stm unNx(Tr_exp e) {
  switch (e->kind)
  {
    case Tr_nx: {
      return e->u.nx;
    }
    case Tr_ex: {
      return T_Exp(e->u.ex);
    }
    case Tr_cx: {
      Temp_temp r = Temp_newtemp();
      Temp_label t = Temp_newlabel(), f = Temp_newlabel();
      doPatch(e->u.cx.trues, t);
      doPatch(e->u.cx.falses, f);
      return T_Exp(T_Eseq(T_Move(T_Temp(r), T_Const(1)),
              T_Eseq(e->u.cx.stm,
                T_Eseq(T_Label(f),
                  T_Eseq(T_Move(T_Temp(r), T_Const(0)),
                    T_Eseq(T_Label(t), T_Temp(r)))))));
    }
    assert(0); return NULL;
  }
}

static struct Cx unCx(Tr_exp e) {
  switch (e->kind)
  {
    case Tr_cx: {
      return e->u.cx;
    }
    case Tr_nx: {
      assert(0); // Never happen
    }
    case Tr_ex: {
      struct Cx cx;
      cx.stm = T_Cjump(T_eq, e->u.ex, T_Const(0), NULL, NULL);
      cx.trues = PatchList(&(cx.stm->u.CJUMP.falsee), NULL);
      cx.falses = PatchList(&(cx.stm->u.CJUMP.truee), NULL);
      return cx;
    }
    assert(0);
  }
}

static Tr_exp Tr_Ex(T_exp ex) {
  Tr_exp p = checked_malloc(sizeof(*p));
  p->kind = Tr_ex; p->u.ex = ex;
  return p;
}

static Tr_exp Tr_Nx(T_exp nx) {
  Tr_exp p = checked_malloc(sizeof(*p));
  p->kind = Tr_nx; p->u.nx = nx;
  return p;
}

static Tr_exp Tr_Cx(T_stm stm, patchList trues, patchList falses) {
  Tr_exp p = checked_malloc(sizeof(*p));
  p->kind = Tr_cx; p->u.cx.stm = stm;
  p->u.cx.trues = trues; p->u.cx.falses = falses;
  return p;
}

static Tr_exp Tr_StaticLink(Tr_level level, Tr_level funLevel) {
  T_exp addr = T_Temp(F_FP());
  while (level != funLevel->parent) {
    // Reuturn the static link 
    F_access staticLink = F_formals(level->frame)->head;
    addr = F_Exp(staticLink, addr);
    level = level->parent;
  }
  return Tr_Ex(addr);
}

static Tr_access Tr_Access(Tr_level level, F_access access) {
  Tr_access a = checked_malloc(sizeof(*a));
  a->level = level; a->access = access;
  return a;
}

static Tr_accessList makeFormalAccessList(Tr_level lev) {
  //* Asume all formals are escape var
  Tr_accessList al = NULL;
  F_accessList f_al = F_formals(lev->frame);
  for (; f_al; f_al = f_al->tail)
    al = Tr_AccessList(Tr_Access(lev, f_al->head), al);
  return al;
}

//* ---------- global methods ----------

//* Static frag list
static F_fragList fragList = NULL;
static F_fragList stringList = NULL;
void Tr_procEntryExit(Tr_level level, Tr_exp body, Tr_accessList formals) {
  F_frag pfrag = F_ProcFrag(unNx(body), level->frame);
  fragList = F_FragList(pfrag, fragList);
}

F_fragList Tr_getResult(void) {
  F_fragList cur = NULL, prev = NULL;
  for (cur = stringList; cur; cur = cur->tail)
    prev = cur;
  if (prev) prev->tail = fragList;
  return stringList ? stringList : fragList;
}

Tr_expList Tr_ExpList(void) {
  Tr_expList p = checked_malloc(sizeof(*p));
  p->head = NULL; p->tail = NULL;
  return p;
}

Tr_expList Tr_ExpList_prepend(Tr_exp exp, Tr_expList list) {
  Tr_expList p = checked_malloc(sizeof(*p));
  p->head = exp; p->tail = list;
  return p;
}

Tr_expList Tr_ExpList_append(Tr_exp exp, Tr_expList list) {
  Tr_expList p = checked_malloc(sizeof(*p)), t;
  p->head = exp; p->tail = NULL;
  for (t = list->tail; t; t = t->tail);
  t->tail = p;
  return list;
}

Tr_accessList Tr_AccessList(Tr_access head, Tr_accessList tail) {
  Tr_accessList al = checked_malloc(sizeof(*al));
  al->head = head; al->tail = tail;
  return al;
}

Tr_level Tr_outermost(void) {
  if (!outer) outer = Tr_newLevel(NULL, Temp_newlabel(), NULL);
  return outer;
}

Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals) {
  Tr_level level = checked_malloc(sizeof(*level));
  level->parent = parent;
  level->frame = F_newFrame(name, U_BoolList(TRUE, formals)); //* Additional `TRUE` for static link
  level->formals = makeFormalAccessList(level);
  return level;
}

Tr_accessList Tr_formals(Tr_level level) {
  return level->formals;
}

Tr_access Tr_allocLocal(Tr_level level, bool escape) {
  return Tr_Access(level, F_allocLocal(level->frame, escape));
}

Tr_exp Tr_emptyExp(void) {
  return Tr_Ex(T_Const(0));
}

Tr_exp Tr_simpleVar(Tr_access access, Tr_level level) {
  T_exp addr = T_Temp(F_FP());
  while (level != access->level) {
    // Static link is the first formal
    F_access staticLink = F_formals(level->frame)->head;
    addr = F_Exp(staticLink, addr);
    level = level->parent;
  }
  return Tr_Ex(F_Exp(access->access, addr));
}

Tr_exp Tr_fieldVar(Tr_exp baseAddr, int fieldoffset) {
  return Tr_Ex(T_Mem(T_Binop(T_plus, unEx(baseAddr), T_Const(fieldoffset * F_WORD_SIZE))));
}

Tr_exp Tr_subscriptVar(Tr_exp baseAddr, Tr_exp index) {
  return Tr_Ex(T_Mem(T_Binop(T_plus, unEx(baseAddr), 
              T_Binop(T_mul, unEx(index), T_Const(F_WORD_SIZE)))));
}

Tr_exp Tr_intExp(int n) {
  return Tr_Ex(T_Const(n));
}

Tr_exp Tr_stringExp(string s) {
  Temp_label lab = Temp_newlabel();
  F_frag fragment = F_StringFrag(lab, s);
  stringList = F_FragList(fragment, stringList);
  return Tr_Ex(T_Name(lab));
}

Tr_exp Tr_arrayExp(Tr_exp size, Tr_exp init) {
  return Tr_Ex(F_externalCall("initArray", T_ExpList(unEx(size), T_ExpList(unEx(init), NULL))));
}

static Temp_temp nilTemp = NULL;
Tr_exp Tr_nilExp(void) {
  if (!nilTemp) {
    nilTemp = Temp_newtemp();
    T_stm alloc = T_Move(T_Temp(nilTemp), T_Const(0));
    return Tr_Ex(T_Eseq(alloc, T_Temp(nilTemp)));
  }
  return Tr_Ex(T_Temp(nilTemp));
}

Tr_exp Tr_arithOpExp(A_oper op, Tr_exp left, Tr_exp right) {
  T_binOp oper;
  switch (op)
  {
    case A_plusOp: oper = T_plus; break;
    case A_minusOp: oper = T_minus; break;
    case A_timesOp: oper = T_mul; break;
    case A_divOp: oper = T_div; break;
    default: assert(0); break;
  }
  return Tr_Ex(T_Binop(oper, unEx(left), unEx(right)));
}

Tr_exp Tr_eqExp(A_oper op, Tr_exp left, Tr_exp right) {
  T_relOp oper;
  if (op == A_eqOp) oper = T_eq;
  if (op == A_neqOp) oper = T_ne;
  T_stm cond = T_Cjump(oper, unEx(left), unEx(right), NULL, NULL);
  patchList trues = PatchList(&(cond->u.CJUMP.truee), NULL);
  patchList falses = PatchList(&(cond->u.CJUMP.falsee), NULL);
  return Tr_Cx(cond, trues, falses);
}

Tr_exp Tr_eqStringExp(A_oper op, Tr_exp left, Tr_exp right) {
  T_exp ret = F_externalCall(String("stringEqual"),
                             T_ExpList(unEx(left),
                             T_ExpList(unEx(right), NULL)));
  if (op == A_eqOp) return Tr_Ex(ret);
  if (op == A_neqOp) {
    T_exp e = (ret->kind == T_CONST && ret->u.CONST == 1) ? T_Const(0) : T_Const(1);
    return Tr_Ex(e);
  }
}

Tr_exp Tr_eqRef(A_oper op, Tr_exp left, Tr_exp right) {
  return Tr_Ex(T_Const(0));
}

Tr_exp Tr_relOpExp(A_oper op, Tr_exp left, Tr_exp right) {
  T_binOp oper;
  switch (op)
  {
    case A_ltOp: oper = T_lt; break;
    case A_leOp: oper = T_le; break;
    case A_gtOp: oper = T_gt; break;
    case A_geOp: oper = T_ge; break;
    default: assert(0); break;
  }
  T_stm cond = T_Cjump(oper, unEx(left), unEx(right), NULL, NULL);
  patchList trues = PatchList(&(cond->u.CJUMP.truee), NULL);
  patchList falses = PatchList(&(cond->u.CJUMP.falsee), NULL);
  return Tr_Cx(cond, trues, falses);
}

Tr_exp Tr_ifCondExp(Tr_exp cond, Tr_exp then, Tr_exp elsee) {
  Temp_label t = Temp_newlabel(), f = Temp_newlabel();
  struct Cx cxCond = unCx(cond);
  Tr_exp ret = Tr_emptyExp();
  doPatch(cxCond.trues, t);
  doPatch(cxCond.falses, f);
  
  if (elsee) {
    // else
    // specify else-exp type
    // TODO:
    Temp_label join = Temp_newlabel();
    Temp_temp r = Temp_newtemp();
    T_stm joinJump = T_Jump(T_Name(join), Temp_LabelList(join, NULL));
  } else {
    // no else
    // specify then-exp type
    if (then->kind == Tr_nx) {
      ret = Tr_Nx(T_Seq(cxCond.stm, T_Seq(
                    T_Label(t), T_Seq(
                      then->u.nx, T_Label(f)))));
    } else if (then->kind == Tr_cx) {
      ret = Tr_Nx(T_Seq(cxCond.stm, T_Seq(
                    T_Label(t), T_Seq(
                      then->u.cx.stm, T_Label(f)))));
    } else {
      ret = Tr_Nx(T_Seq(cxCond.stm, T_Seq(
                    T_Label(t), T_Seq(
                      T_Exp(unEx(then)), T_Label(f)))));
    }
  }
}

Tr_exp Tr_seqExp(Tr_expList list) {
  Tr_exp ret = unEx(list->head);
  for (Tr_expList p = list->tail; p; p = p->tail)
    ret = T_Eseq(T_Exp(p->head), ret);
  return ret;
}

Tr_exp Tr_assignExp(Tr_exp var, Tr_exp exp) {
  return Tr_Nx(T_Move(unEx(var), unEx(exp)));
}

Tr_exp Tr_callExp(Tr_level level, Tr_level funLevel, Temp_label label, Tr_expList argList) {
  argList = Tr_ExpList_prepend(Tr_StaticLink(level, funLevel), argList);
  //TODO: Convert args
  return T_Call(T_Name(label), argList);
}

Tr_exp Tr_recordExp(Tr_expList list, int n) {
  Temp_temp r = Temp_newtemp();
  T_stm alloc = T_Move(T_Temp(r),
                F_externalCall(String("initRecord"), T_ExpList(T_Const(n * F_WORD_SIZE), NULL)));
  int i = i - 1;
  T_stm seq = T_Move(T_Mem(T_Binop(T_plus, T_Temp(r), T_Const(i-- * F_WORD_SIZE))),
                     unEx(list->head));
  for (Tr_expList p = list->tail; p; p = p->tail, i--)
    seq = T_Seq(T_Move(T_Mem(T_Binop(T_plus, T_Temp(r), T_Const(i * F_WORD_SIZE))), unEx(p->head)), seq);              
  return Tr_Ex(T_Eseq(T_Seq(alloc, seq), T_Temp(r)));
}


Tr_exp Tr_whileExp(Tr_exp cond, Tr_exp done, Tr_exp body) {
  Temp_label condLabel = Temp_newlabel(), bodyLabel = Temp_newlabel();
  return Tr_Ex(T_Eseq(T_Jump(T_Name(condLabel), Temp_LabelList(condLabel, NULL)),
                T_Eseq(T_Label(bodyLabel), T_Eseq(unEx(body),
                  T_Eseq(T_Label(condLabel),
                    T_Eseq(T_Cjump(T_eq, unEx(cond), T_Const(0), unEx(done)->u.NAME, bodyLabel),
                      T_Eseq(T_Label(unEx(done)->u.NAME), T_Const(0))))))));
}

Tr_exp Tr_doneExp(void) {
  return Tr_Ex(T_Name(Temp_newlabel()));
}

Tr_exp Tr_breakExp(Tr_exp breakk) {
  T_stm s = unNx(breakk);
  if (s->kind == T_LABEL)
    return Tr_Nx(T_Jump(T_Name(s->u.LABEL), Temp_LabelList(s->u.LABEL, NULL)));
  assert(0);
}