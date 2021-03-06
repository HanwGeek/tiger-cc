/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Semantic tranlate & check module.
 * @Date: 2019-10-25 13:45:45
 * @Last Modified: 2020-02-25 20:26:57
 */
#include <stdlib.h>
#include "semant.h"
#include "symbol.h"
#include "errormsg.h"
#include "types.h"
#include "env.h"
#include "escape.h"
#include "frame.h"
#include "translate.h"

static struct expty expTy(Tr_exp exp, Ty_ty ty);
static struct expty transVar(Tr_level level, S_table venv, S_table tenv, Tr_exp breakk, A_var v);
static struct expty transExp(Tr_level level, S_table venv, S_table tenv, Tr_exp breakk, A_exp e);
static Tr_exp transDec(Tr_level level, S_table venv, S_table tenv, Tr_exp breakk, A_dec d);
static Ty_ty transTy(S_table tenv, A_ty a);
//* Return the actual type of Ty_name.ty
static Ty_ty actual_ty(Ty_ty ty);
static Ty_ty S_look_ty(S_table tenv, S_symbol sym);
static U_boolList makeFormals(A_fieldList params);
static Ty_tyList makeFormalTyList(S_table tenv, A_fieldList params);
static Ty_fieldList makeFieldTys(S_table tenv, A_fieldList fields);
static int ty_equal(Ty_ty a, Ty_ty b);

//* Translate variable of {simpleVar, fieldVar, subscriptVar  }
static struct expty transVar(Tr_level level, S_table venv, S_table tenv, Tr_exp breakk, A_var v) {
  switch (v->kind)
  {
    case A_simpleVar: {
      E_enventry x = S_look(venv, v->u.simple);
      Tr_exp tr_ret = Tr_emptyExp();
      if (x && x->kind == E_varEntry) {
        tr_ret = Tr_simpleVar(x->u.var.access, level);
        return expTy(tr_ret, actual_ty(x->u.var.ty));
      }
      else {
        EM_error(v->pos, "error: undefined variable %s", S_name(v->u.simple));
        return expTy(tr_ret, Ty_Int());
      }
    }
    case A_fieldVar: {
      struct expty e = transVar(level, venv, tenv, breakk, v->u.field.var);
      Tr_exp tr_ret = Tr_emptyExp();
      if (e.ty->kind != Ty_record) {
        EM_error(v->u.field.var->pos, "error: %s not a record type", S_name(v->u.field.sym));
      } else {
        // Record var store in a continuous mem space
        int offset = 0;
        for (Ty_fieldList f = e.ty->u.record; f; f = f->tail, offset++) {
          if (f->head->name == v->u.field.sym) {
            return expTy(Tr_fieldVar(e.exp, offset), actual_ty(f->head->ty));
          }
        }
        EM_error(v->pos, "error: no such field %s for record type", S_name(v->u.field.sym));
      }
      return expTy(tr_ret, Ty_Int());
    }
    case A_subscriptVar: {
      struct expty e = transVar(level, venv, tenv, breakk, v->u.subscript.var);
      Tr_exp tr_ret = Tr_emptyExp();
      if (e.ty->kind != Ty_array) {
        EM_error(v->u.subscript.var->pos, "error: not an array type");
        return expTy(tr_ret, Ty_Int());
      } else {
        struct expty index = transExp(level, venv, tenv, breakk, v->u.subscript.exp);
        if (index.ty != Ty_Int()) {
          EM_error(v->u.subscript.exp->pos, "error: index type of int required");
        } else {
          tr_ret = Tr_subscriptVar(e.exp, index.exp);
        }
        return expTy(tr_ret, actual_ty(e.ty->u.array));
      }
    }
    default:
      assert(0);
  }
} 

//* Translate expression
static struct expty transExp(Tr_level level, S_table venv, S_table tenv, Tr_exp breakk, A_exp a) {
  switch (a->kind)
  {
    case A_varExp: {
      return transVar(level, venv, tenv, breakk, a->u.var);
    }
    case A_nilExp: {
      return expTy(Tr_nilExp(), Ty_Nil());
    }
    case A_intExp: {
      return expTy(Tr_intExp(a->u.intt), Ty_Int());
    }
    case A_stringExp: {
      return expTy(Tr_stringExp(a->u.stringg), Ty_String());
    }
    case A_callExp: {
      E_enventry x = S_look(venv, a->u.call.func);
      A_expList args = NULL;
      Tr_exp ret = Tr_emptyExp();
      Tr_expList argList = Tr_ExpList();
      if (x && x->kind == E_funEntry) {
        Ty_tyList formals = x->u.fun.formals;
        for (args = a->u.call.args; args && formals; args = args->tail, formals = formals->tail) {
          struct expty arg = transExp(level, venv, tenv, breakk, args->head);
          if (!ty_equal(arg.ty, formals->head))
            EM_error(args->head->pos, "error: expect formal type %s instead of %s", Ty_ToString(formals->head), Ty_ToString(arg.ty));
          Tr_ExpList_append(arg.exp, argList);
        }
        if (args == NULL && formals != NULL)
          EM_error(a->pos, "error: not enough arguments");
        if (args != NULL && formals == NULL)
          EM_error(a->pos, "error: too many arguments");
        return expTy(Tr_callExp(level, x->u.fun.level, x->u.fun.label, argList), actual_ty(x->u.fun.result));
      } else {
        EM_error(a->pos, "error: undefined function %s", S_name(a->u.call.func));
        return expTy(ret, Ty_Int());
      }
    }
    case A_opExp: {
      A_oper oper = a->u.op.oper;
      struct expty left = transExp(level, venv, tenv, breakk, a->u.op.left);
      struct expty right = transExp(level, venv, tenv, breakk, a->u.op.right);
      switch (oper) {
        case A_plusOp:
        case A_minusOp:
        case A_timesOp:
        case A_divOp: {
          if (left.ty->kind != Ty_int)
            EM_error(a->u.op.left->pos, "error: integer required!");
          if (right.ty->kind != Ty_int)
            EM_error(a->u.op.right->pos, "error: integer required!");
          return expTy(Tr_arithOpExp(oper, left.exp, right.exp), Ty_Int());
        }
        case A_eqOp:
        case A_neqOp: {
          if (!ty_equal(left.ty, right.ty)) {
            EM_error(a->u.op.left->pos, "error: diffrent type cannot be compared");
            return expTy(Tr_emptyExp(), Ty_Int());
          }
          Tr_exp ret = Tr_emptyExp();
          if (left.ty->kind == Ty_int)
            ret = Tr_eqExp(oper, left.exp, right.exp);
          if (left.ty->kind == Ty_string)
            ret = Tr_eqStringExp(oper, left.exp, right.exp);
          if (left.ty->kind == Ty_array)
            ret = Tr_eqRef(oper, left.exp, right.exp);
          return expTy(ret, Ty_Int());
        }
        case A_ltOp:
        case A_leOp:
        case A_gtOp:
        case A_geOp: {
          if (left.ty != right.ty)
            EM_error(a->u.op.left->pos, "error: cannot compare between %s and %s!", Ty_ToString(left.ty), Ty_ToString(right.ty));
          return expTy(Tr_relOpExp(a->u.op.oper, left.exp, right.exp), Ty_Int());
        }
      }
    }
    case A_recordExp: {
      Ty_ty typ = S_look_ty(tenv, a->u.record.typ);
      if (!typ) {
        EM_error(a->pos, "error: undefined type %s", S_name(a->u.record.typ));
        return expTy(Tr_emptyExp(), Ty_Record(NULL));
      }
      if (typ->kind != Ty_record) 
        EM_error(a->pos, "error: %s is not a record type", S_name(a->u.record.typ));
      
      int n = 0;
      Ty_fieldList fieldTys = typ->u.record;
      Tr_expList list = Tr_ExpList();
      for (A_efieldList recList = a->u.record.fields; recList; recList = recList->tail, fieldTys = fieldTys->tail, n++) {
        struct expty e = transExp(level, venv, tenv, breakk, recList->head->exp);
        if (recList->head->name != fieldTys->head->name)
          EM_error(a->pos, "error: %s not a valid field name", recList->head->name);
        if (!ty_equal(fieldTys->head->ty, e.ty))
          EM_error(recList->head->exp->pos, "error: given %s but expect %s", Ty_ToString(e.ty), Ty_ToString(fieldTys->head->ty));
        Tr_ExpList_prepend(e.exp, list);
      }
      return expTy(Tr_recordExp(list, n), typ);
    }
    case A_seqExp: {
      struct expty exp = expTy(Tr_emptyExp(), Ty_Void());
      Tr_expList list = Tr_ExpList();
      for (A_expList p = a->u.seq; p; p = p->tail) {
        exp = transExp(level, venv, tenv, breakk, p->head);
        Tr_ExpList_prepend(exp.exp, list);
      }
      return expTy(Tr_seqExp(list), exp.ty);
    }
    case A_assignExp: {
      struct expty var = transVar(level, venv, tenv, breakk, a->u.assign.var);
      struct expty exp = transExp(level, tenv, venv, breakk, a->u.assign.exp);
      if (!ty_equal(var.ty, exp.ty))
        EM_error(a->u.assign.exp->pos, "error: expression not of expected type %s", Ty_ToString(var.ty));
      return expTy(Tr_assignExp(var.exp, exp.exp), Ty_Void());
    }
    case A_ifExp: {
      struct expty cond = transExp(level, venv, tenv, breakk, a->u.iff.cond);
      if (cond.ty->kind != Ty_int)
        EM_error(a->u.iff.cond->pos, "error: integer type of condition required");
      struct expty then = transExp(level, venv, tenv, breakk, a->u.iff.then);
      if (a->u.iff.elsee) {
        struct expty elsee = transExp(level, venv, tenv, breakk, a->u.iff.elsee);
        if (!ty_equal(cond.ty, elsee.ty))
          EM_error(a->u.iff.elsee->pos, "error: then and else must return same type");
        return expTy(Tr_ifCondExp(cond.exp, then.exp, elsee.exp), then.ty);
      } else {
        if (then.ty->kind != Ty_void)
          EM_error(a->u.iff.then->pos, "error: then-expression must return no value");
        return expTy(Tr_ifCondExp(cond.exp, then.exp, NULL), Ty_Void());
      }
    }
    case A_whileExp: {
      struct expty cond = transExp(level, venv, tenv, breakk, a->u.whilee.cond);
      if (cond.ty != Ty_Int())
        EM_error(a->u.whilee.cond->pos, "error: integer type of condition required");
      Tr_exp newBreakk = Tr_doneExp();
      struct expty body = transExp(level, venv, tenv, breakk, a->u.whilee.body);
      if (body.ty != Ty_Void())
        EM_error(a->u.whilee.body->pos, "error: body-expression must return no value");
      return expTy(Tr_whileExp(cond.exp, newBreakk, body.exp), Ty_Void());
    }
    case A_forExp: {
      A_dec i = A_VarDec(a->pos, a->u.forr.var, NULL, a->u.forr.lo);
      A_dec limit = A_VarDec(a->pos, S_Symbol("limit"), NULL, a->u.forr.hi);
      A_dec cond = A_VarDec(a->pos, S_Symbol("cond"), NULL, A_IntExp(a->pos, 1));
      A_exp iExp = A_VarExp(a->pos, A_SimpleVar(a->pos, a->u.forr.var));
      A_exp limitExp = A_VarExp(a->pos, A_SimpleVar(a->pos, S_Symbol("limit")));
      A_exp condExp = A_VarExp(a->pos, A_SimpleVar(a->pos, S_Symbol("cond")));
      A_exp increment = A_AssignExp(a->pos, A_SimpleVar(a->pos, a->u.forr.var), A_OpExp(a->pos, A_plusOp, iExp, A_IntExp(a->pos, 1)));
      A_exp setFalse = A_AssignExp(a->pos, A_SimpleVar(a->pos, S_Symbol("cond")), A_IntExp(a->pos, 0));
      A_exp letExp = A_LetExp(a->pos,
            A_DecList(i, A_DecList(limit, A_DecList(cond, NULL))),
            A_SeqExp(a->pos, 
              A_ExpList(
                A_IfExp(a->pos, A_OpExp(a->pos, A_leOp, a->u.forr.lo, a->u.forr.hi),
                  A_WhileExp(a->pos, condExp, 
                    A_SeqExp(a->pos, A_ExpList(a->u.forr.body,
                      A_ExpList(A_IfExp(a->pos, A_OpExp(a->pos, A_ltOp, iExp, limitExp), increment, setFalse), NULL)))), NULL), NULL)));
      struct expty e = transExp(level, venv, tenv, breakk, letExp);
      return e;
    }
    case A_breakExp: {
      Tr_exp ret = Tr_emptyExp();
      if (!breakk)
        EM_error(a->pos, "error: illegal break token out of loop");
      else
        ret = Tr_breakExp(breakk);
      return expTy(ret, Ty_Void());
    }
    case A_letExp:{
      struct expty exp;
      Tr_expList list = Tr_ExpList();
      S_beginScope(venv);
      S_beginScope(tenv);
      for (A_decList d = a->u.let.decs; d; d = d->tail)
        transDec(level, venv, tenv, breakk, d->head);
      exp = transExp(level, venv, tenv, breakk, a->u.let.body);
      Tr_ExpList_prepend(exp.exp, list);
      S_endScope(venv);
      S_endScope(tenv);
      return expTy(Tr_seqExp(list), exp.ty);
    }
    case A_arrayExp: {
      Ty_ty typ = S_look_ty(tenv, a->u.array.typ);
      if (!typ) {
        EM_error(a->pos, "error: undefined type");
        return expTy(Tr_emptyExp(), Ty_Int());
      }
      struct expty init = transExp(level, venv, tenv, breakk, a->u.array.init);
      struct expty size = transExp(level, tenv, venv, breakk, a->u.array.size);
      if (size.ty->kind != Ty_int)
        EM_error(a->u.array.size->pos, "error: type int required for array size"); 
      if (!ty_equal(typ->u.array, init.ty))
        EM_error(a->u.array.init->pos, "error: init type of %s but %s required", Ty_ToString(init.ty), Ty_ToString(typ));
      return expTy(Tr_arrayExp(size.exp, init.exp), typ);
    }
    default: assert(0); break;
  }
}

//* Translate declaration
Tr_exp transDec(Tr_level level, S_table venv, S_table tenv, Tr_exp breakk, A_dec d) {
  switch (d->kind) {
    case A_varDec: {
      struct expty e = transExp(level, venv, tenv, breakk, d->u.var.init);
      Tr_access access = Tr_allocLocal(level, TRUE);
      if (d->u.var.typ) {
        Ty_ty typ = S_look_ty(tenv, d->u.var.typ);
        if (!typ) {
          EM_error(d->pos, "error: undefined type %s", S_name(d->u.var.typ));
          typ = Ty_Void();
        }
        if (!ty_equal(typ, e.ty))
          EM_error(d->pos, "error: %s type of declare expression expected instead of %s", Ty_ToString(typ), Ty_ToString(e.ty));
        S_enter(venv, d->u.var.var, E_VarEntry(access, e.ty));
      } else {
        if (e.ty->kind == Ty_nil)
          EM_error(d->u.var.init->pos, "error: illeal use nil expression");
        S_enter(venv, d->u.var.var, E_VarEntry(access, e.ty));
      }
      return Tr_assignExp(Tr_simpleVar(access, level), e.exp);
    }
    case A_functionDec: {
      //* Process function declaration first in case of recursive function
      for (A_fundecList funList = d->u.function; funList; funList = funList->tail) {
        Ty_ty resultTy = NULL;
        if (funList->head->result) {
          resultTy = S_look(tenv, funList->head->result);
          if (!resultTy)
            EM_error(funList->head->pos, "error: undefined return type");
        }
        if (!resultTy) resultTy = Ty_Void();

        Ty_tyList formalTys = makeFormalTyList(tenv, funList->head->params);
        U_boolList formals = makeFormals(funList->head->params);
        Temp_label funLabel = Temp_newlabel();
        Tr_level funLevel = Tr_newLevel(level, funLabel, formals);
        S_enter(venv, funList->head->name,
                E_FunEntry(funLevel, funLabel, formalTys, resultTy));
      }

      //* Process function definition
      for (A_fundecList funList = d->u.function; funList; funList = funList->tail) {
        E_enventry funEntry = S_look(venv, funList->head->name);
        S_beginScope(venv);
        Ty_tyList formalTys = funEntry->u.fun.formals;
        Tr_accessList accessList = Tr_formals(funEntry->u.fun.level);
        for (A_fieldList paramFields = funList->head->params; paramFields; 
              paramFields = paramFields->tail, formalTys = formalTys->tail, accessList = accessList->tail) {
          S_enter(venv, paramFields->head->name,
                    E_VarEntry(accessList->head, formalTys->head));
        }
        
        struct expty e = transExp(funEntry->u.fun.level, venv, tenv,
                                    breakk, funList->head->body);
        if (!ty_equal(funEntry->u.fun.result, e.ty))
          EM_error(funList->head->body->pos, "error: incompatible return type %s; expected %s",
                    Ty_ToString(e.ty), Ty_ToString(funEntry->u.fun.result));
        Tr_procEntryExit(funEntry->u.fun.level, e.exp, accessList);
        S_endScope(venv);                                    
      }
      return Tr_emptyExp();
    }
    case A_typeDec: {
      bool isCycle = TRUE;
      //* Make type declaration entry with nil type first
      for (A_nametyList nameList = d->u.type; nameList; nameList = nameList->tail)
        S_enter(tenv, nameList->head->name, Ty_Name(nameList->head->name, NULL));
      
      //* Process type definition in case of recursive ones
      for (A_nametyList nameList = d->u.type; nameList; nameList = nameList->tail) {
        Ty_ty t = transTy(tenv, nameList->head->ty);
        if (isCycle && t->kind != Ty_name) isCycle = FALSE;
        Ty_ty nameTy = S_look(tenv, nameList->head->name);
        nameTy->u.name.ty = t; 
      }
      if (isCycle)
        EM_error(d->pos, "error: illegal cycle type");
      return Tr_emptyExp();
    }
  }
  assert(0);
}

static Ty_ty transTy(S_table tenv, A_ty t) {
  Ty_ty ty;
  switch (t->kind) {
    case A_nameTy: {
      ty = S_look(tenv, t->u.name);
      if (!ty)
        EM_error(t->pos, "error: undefined type %s", S_name(t->u.name));
      return ty;
    }
    case A_recordTy: {
      Ty_fieldList fieldTys = makeFieldTys(tenv, t->u.record);
      return Ty_Record(fieldTys);
    }
    case A_arrayTy: {
      ty = S_look(tenv, t->u.array);
      if (!ty)
        EM_error(t->pos, "error: undefined type %s", S_name(t->u.array));
      return Ty_Array(ty);
    }
  }
  assert(0);
}

static U_boolList makeFormals(A_fieldList params) {
  //* Assume all are escape var
  U_boolList formals = NULL;
  for (A_fieldList fl = params; fl; fl = fl->tail) {
    formals = U_BoolList(TRUE, formals);
  }
  return formals;
}

static struct expty expTy(Tr_exp exp, Ty_ty ty) {
  struct expty e;
  e.exp = exp; e.ty = ty;
  return e;
}

static Ty_ty actual_ty(Ty_ty ty) {
  if (ty->kind == Ty_name)
    return actual_ty(ty->u.name.ty);
  else 
    return ty;
}

static Ty_ty S_look_ty(S_table tenv, S_symbol sym) {
  Ty_ty t = S_look(tenv, sym);
  if (t)
    return actual_ty(t);
  else 
    return NULL;
}

static Ty_tyList makeFormalTyList(S_table tenv, A_fieldList params) {
  Ty_tyList p = NULL, tailList = p;
  for (A_fieldList paramList = params; paramList; paramList = paramList->tail) {
    Ty_ty t = S_look_ty(tenv, paramList->head->typ);
    if (!t) {
      EM_error(paramList->head->pos, "error: undefined type %s", S_name(paramList->head->typ));
    } else {
      if (p) {
        tailList->tail = Ty_TyList(t, NULL);
        tailList = tailList->tail;
      } else {
        p = Ty_TyList(t, NULL);
        tailList = p;
      }
    }
  }
  return p;
}

static int ty_equal(Ty_ty a, Ty_ty b) {
  a = actual_ty(a); b = actual_ty(b);
  return ((a->kind == Ty_record || a->kind == Ty_array) && a == b) ||
         (a->kind == Ty_record && b->kind == Ty_nil) ||
         ((a->kind != Ty_record && a->kind != Ty_array) && (a->kind == b->kind)); 
}

static Ty_fieldList makeFieldTys(S_table tenv, A_fieldList fields) {
  Ty_fieldList fieldTys = NULL;
  for (A_fieldList fieldList = fields; fieldList; fieldList = fieldList->tail) {
    Ty_ty ty = S_look(tenv, fieldList->head->typ);
    if (!ty) {
      EM_error(fieldList->head->pos, "undefined type %s", S_name(fieldList->head->typ));
    } else {
      Ty_field f = Ty_Field(fieldList->head->name, ty);
      if (fieldTys) {
        fieldTys->tail = Ty_FieldList(f, NULL);
      } else {
        fieldTys = Ty_FieldList(f, NULL);
      }
    }
  }
  return fieldTys;
}

F_fragList SEM_transProg(A_exp exp) {
  ESC_findEscape(exp);
  S_table tenv = E_base_tenv();
  S_table venv = E_base_venv();
  Tr_level baseLevel = Tr_outermost();
  struct expty e = transExp(baseLevel, venv, tenv, NULL, exp);
  Tr_procEntryExit(baseLevel, e.exp, NULL);
  return Tr_getResult();
}