/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: X86 machine stack frame implement.
 * @Date: 2019-11-01 20:51:20
 * @Last Modified: 2020-01-25 12:00:34
 */
#include "frame.h"

//* Formal param or local var in frame or register
struct F_access_ {
  enum {
    inFrame, //* In frame
    inReg    //* In reg
  } kind;
  union {
    int offset; 
    Temp_temp reg;
  } u;
};

//* Memeory stack unit
struct F_frame_ {
  Temp_label name;
  F_accessList formals;
  int local_count;
};

const int F_R = 6; //* Numbers of params in reg;
const int F_WORD_SIZE = 4; //* Define word size
static F_access InFrame(int offset);
static F_access InReg(Temp_temp reg);
static F_accessList F_AccessList(F_access head, F_accessList tail);
static F_accessList makeFormalAccessList(U_boolList formals);

static Temp_tempList F_make_arg_regs(void);
static Temp_tempList F_make_calle_saves(void);
static Temp_tempList F_callee_saves(void);
static Temp_tempList F_make_caller_saves(void);
static Temp_tempList F_special_regs(void);
static Temp_tempList F_arg_regs(void);

static void F_add_to_map(string str, Temp_temp temp);

static F_access InFrame(int offset) {
  F_access fa = checked_malloc(sizeof(*fa));
  fa->kind = inFrame;
  fa->u.offset = offset;
  return fa;
}

static F_access InReg(Temp_temp reg) {
  F_access fa = checked_malloc(sizeof(*fa));
  fa->kind = inReg;
  fa->u.reg = reg;
  return fa;
}

static F_accessList F_AccessList(F_access head, F_accessList tail) {
  F_accessList al = checked_malloc(sizeof(*al));
  al->head = head; al->tail = tail;
  return al;
}

static Temp_tempList F_make_arg_regs(void) {
  Temp_temp eax = Temp_newtemp(), ebx = Temp_newtemp(),
            ecx = Temp_newtemp(), edx = Temp_newtemp(),
            edi = Temp_newtemp(), esi = Temp_newtemp();
   F_add_to_map("eax", eax); F_add_to_map("ebx", ebx);
   F_add_to_map("ecx", ecx); F_add_to_map("edx", edx);
   F_add_to_map("edi", edi); F_add_to_map("esi", esi);
   return Temp_TempList(eax, Temp_TempList(ebx,
            Temp_TempList(ecx, Temp_TempList(edx,
              Temp_TempList(edi, Temp_TempList(esi, NULL))))));
}

static Temp_tempList F_make_calle_saves(void);
static Temp_tempList F_callee_saves(void);
static Temp_tempList F_make_caller_saves(void);
static Temp_tempList F_special_regs(void);
static Temp_tempList F_arg_regs(void);

Temp_map F_tempMap = NULL;
static void F_add_to_map(string str, Temp_temp temp) {
  if (!F_tempMap)
    F_tempMap = Temp_name();
  Temp_enter(F_tempMap, temp, str);
}

F_frag F_StringFrag(Temp_label label, string str) {
  F_frag p = checked_malloc(sizeof(*p));
  p->kind = F_stringFrag;
  p->u.stringg.label = label;
  p->u.stringg.str = str;
  return p;
}

F_frag F_ProcFrag(T_stm body, F_frame frame) {
  F_frag p = checked_malloc(sizeof(*p));
  p->kind = F_procFrag;
  p->u.proc.body = body;
  p->u.proc.frame = frame;
  return p;
}

F_fragList F_FragList(F_frag head, F_fragList tail) {
  F_fragList f = checked_malloc(sizeof(*f));
  f->head = head; f->tail = tail;
  return f;
}

F_frame F_newFrame(Temp_label name, U_boolList formals) {
  F_frame f = checked_malloc(sizeof(*f));
  f->name = name;
  f->formals = makeFormalAccessList(formals);
  return f;
}

//* Return F_formals in F_frame {f}
F_accessList F_formals(F_frame f) {
  return f->formals;
}

//* Convert formal escapability boollist to F_accessList
F_accessList makeFormalAccessList(U_boolList formals) {
  F_accessList al = NULL;
  int formal_cnt = 0;
  for (U_boolList fml = formals; fml; fml = formals->tail, formal_cnt++) {
    //* Assume all are escape var in frame
    F_access a = InFrame((2 + formal_cnt) * F_WORD_SIZE);
    al = F_AccessList(a, al);
  }
  return al;
}

F_access F_allocLocal(F_frame f, bool escape) {
  f->local_count++;
  if (escape) return InFrame(F_WORD_SIZE * (- f->local_count));
  return InReg(Temp_newtemp());
}

bool F_isEscape(F_access access) {
  return access != NULL && access->kind == InFrame;
}

static Temp_temp fp = NULL;
Temp_temp F_FP(void) {
  if (!fp) 
    fp = Temp_newtemp();
  return fp;
}

T_exp F_Exp(F_access acc, T_exp framePtr) {
  if (acc->kind == inReg) 
    return T_Temp(acc->u.reg);
  if (acc->kind == inFrame)
    return T_Mem(T_Binop(T_plus, framePtr, T_Const(acc->u.offset)));
  assert(0);
}

T_exp F_externalCall(string s, T_expList args) {
  return T_Call(T_Name(Temp_namedlabel(s)), args);
}
