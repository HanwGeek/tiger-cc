/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Date: 2019-10-25 13:56:49
 * @Last Modified: 2020-02-25 20:06:47
 */
#include "env.h"

E_enventry E_VarEntry(Tr_access access, Ty_ty ty) {
  E_enventry p = checked_malloc(sizeof(*p));
  p->kind = E_varEntry;
  p->u.var.access = access;
  p->u.var.ty = ty;
  return p;
}

E_enventry E_FunEntry(Tr_level level, Temp_label label, Ty_tyList formals, Ty_ty result) {
  E_enventry p = checked_malloc(sizeof(*p));
  p->kind = E_funEntry;
  p->u.fun.level = level;
  p->u.fun.label = label;
  p->u.fun.formals = formals;
  p->u.fun.result = result;
  return p;
}

E_enventry E_EscEntry(int depth, bool* escape) {
  E_enventry p = checked_malloc(sizeof(*p));
  p->kind = E_escEntry;
  p->u.esc.d = depth;
  p->u.esc.e = escape;
  return p;
}

S_table E_base_tenv(void) {
  S_table tenv = S_empty();
  S_enter(tenv, S_Symbol("int"), Ty_Int());
  S_enter(tenv, S_Symbol("string"), Ty_String());
  return tenv;
}

S_table E_base_venv(void) {
  S_table venv = S_empty();
  return venv;
}

S_table E_base_eenv(void) {
  S_table eenv = S_empty();
  return eenv;
}