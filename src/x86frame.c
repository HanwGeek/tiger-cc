/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: X86 machine stack frame implement.
 * @Date: 2019-11-01 20:51:20
 * @Last Modified: 2020-02-29 15:29:26
 */
#include "frame.h"
#include "util.h"

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
//* Return access of temp in frame with `offset` to frame pointer
static F_access InFrame(int offset);
static F_access InReg(Temp_temp reg);
static F_accessList F_AccessList(F_access head, F_accessList tail);
//* Convert formal escapability boollist to F_accessList
static F_accessList makeFormalAccessList(U_boolList formals);

static Temp_tempList F_special_regs(void);
//* Pass actual params regs
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

//* Mapping temps to registers
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

Temp_label F_name(F_frame f) {
  return f->name;
}

F_accessList F_formals(F_frame f) {
  return f->formals;
}

int F_inFrameOffset(F_access access) {
  assert(access && access->kind == inFrame);
  return access->u.offset;
}

static F_accessList makeFormalAccessList(U_boolList formals) {
  F_accessList al = NULL;
  int formal_cnt = 0;
  for (U_boolList fml = formals; fml; fml = fml->tail, formal_cnt++) {
    //* Assume all are escape var in frame
    F_access a = InFrame((2 + formal_cnt) * F_WORD_SIZE);
    al = F_AccessList(a, al);
  }
  return al;
}

F_access F_allocLocal(F_frame f, bool escape) {
  f->local_count++;
  if (escape) return InFrame(F_WORD_SIZE * (-f->local_count));
  return InReg(Temp_newtemp());
}

bool F_isEscape(F_access access) {
  return access != NULL && access->kind == inFrame;
}

static Temp_temp fp = NULL;
Temp_temp F_FP(void) {
  if (!fp) {
    fp = Temp_newtemp();
    F_add_to_map("ebp", fp);
  }
  return fp;
}

static Temp_temp sp = NULL;
Temp_temp F_SP(void) {
  if (!sp) {
    sp = Temp_newtemp();
    F_add_to_map("esp", sp);
  }
  return sp;
}

static Temp_temp ra = NULL;
Temp_temp F_RA(void) {
  if (!ra) {
    ra = Temp_newtemp();
    F_add_to_map("rdkd", ra);
  }
  return ra;
}

static Temp_temp zero = NULL;
Temp_temp F_ZERO(void) {
  if (!zero) {
    zero = Temp_newtemp();
    F_add_to_map("zero", zero); 
  }
  return zero;
}

static Temp_temp rv = NULL;
Temp_temp F_RV(void) {
  if (!rv) {
    rv = Temp_newtemp();
    F_add_to_map("eax", rv);
  }
  return rv;
}

static Temp_tempList callerSaves = NULL;
Temp_tempList F_caller_saves(void) {
  if (!callerSaves) {
    //* F_RV() eax
    //* arg reg: edi, esi, edx, ecx, *r8d, *r9d
    callerSaves = Temp_TempList(F_RV(), F_arg_regs());
  }
  return callerSaves;
}

static Temp_tempList calleeSaves = NULL;
Temp_tempList F_callee_saves(void) {
  if (!calleeSaves) {
    Temp_temp ebx = Temp_newtemp();
    F_add_to_map("ebp", ebx);
    calleeSaves = Temp_TempList(ebx,
                    Temp_TempList(F_FP(), 
                      Temp_TempList(F_SP(), NULL)));
  }
  return calleeSaves;
}

Temp_tempList argRegs = NULL;
Temp_tempList F_arg_regs(void) {
  if (!argRegs) {
    Temp_temp edi = Temp_newtemp(), esi = Temp_newtemp(),
              edx = Temp_newtemp(), ecx = Temp_newtemp();
    F_add_to_map("edi", edi); F_add_to_map("esi", esi);
    F_add_to_map("edx", edx); F_add_to_map("ecx", ecx);
    argRegs = Temp_TempList(edi,
                Temp_TempList(esi,
                  Temp_TempList(edx, 
                    Temp_TempList(ecx, NULL))));    
  }
  return argRegs;
}

Temp_tempList specicalRegs = NULL;
static Temp_tempList F_special_regs(void) {
  if (!specicalRegs)
    specicalRegs = Temp_TempList(F_SP(), 
                    Temp_TempList(F_FP(),
                      Temp_TempList(F_RV(), 
                        Temp_TempList(F_RA(),
                          Temp_TempList(F_ZERO(), NULL)))));
  return specicalRegs;
}

static Temp_tempList regs = NULL;
Temp_tempList F_registers(void) {
  if (!regs) {
    Temp_tempList tail = NULL;
    for (Temp_tempList r = F_callee_saves(); r; r = r->tail) {
      if (!regs) {
        regs = Temp_TempList(r->head, NULL);
        tail = regs;
      }
      tail->tail = Temp_TempList(r->head, NULL);
      tail = tail->tail;
    }
    for (Temp_tempList r = F_caller_saves(); r; r = r->tail) {
      tail->tail = Temp_TempList(r->head, NULL);
      tail = tail->tail;
    }
    tail->tail = Temp_TempList(F_RA(), 
                  Temp_TempList(F_ZERO(), NULL));
  }
  return regs;
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

T_stm F_procEntryExit1(F_frame frame, T_stm stm) {
  F_accessList args = frame->formals;
  Temp_tempList argRegs = F_arg_regs(); 
}

static Temp_tempList returnSink = NULL;
AS_instrList F_procEntryExit2(AS_instrList body) {
  if (!returnSink)
    returnSink = Temp_TempList(F_RA(),
                  Temp_TempList(F_SP(), F_callee_saves()));
  return AS_splice(body, AS_InstrList(AS_Oper("", NULL, returnSink, NULL), NULL));
}

AS_proc F_procEntryExit3(F_frame frame, AS_instrList body) {
  return AS_Proc("prolog", body, "epilog");
}
