/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Date: 2019-11-01 21:40:30
 * @Last Modified: 2020-02-25 20:11:48
 */
#include "escape.h"
#include "semant.h"

static void traverseExp(S_table env, int depth, A_exp e);
static void traverseDec(S_table env, int depth, A_dec d);
static void traverseVar(S_table env, int depth, A_var v);

/* ---------- Static Methods ---------- */
static void traverseExp(S_table env, int depth, A_exp e) {
  switch (e->kind) {
    case A_varExp: {
      traverseVar(env, depth, e->u.var);
      break;
    }
    case A_nilExp:
    case A_intExp:
    case A_stringExp:
      break;
    case A_letExp: {
      S_beginScope(env);
      for (A_decList decs = e->u.let.decs; decs; decs = decs->tail)
        traverseDec(env, depth, decs->head);
      traverseExp(env, depth, e->u.let.body);
      S_endScope(env);
      break;
    }
    case A_seqExp: {
      for (A_expList exps = e->u.seq; exps; exps = exps->tail)
        traverseExp(env, depth, exps->head);
      break;
    }
    case A_callExp: {
      for (A_expList args = e->u.call.args; args; args = args->tail)
        traverseExp(env, depth, args->head);
      break;
    }
    case A_opExp: {
      traverseExp(env, depth, e->u.op.left);
      traverseExp(env, depth, e->u.op.right);
      break;
    }
    case A_recordExp: {
      for (A_efieldList fields = e->u.record.fields; fields; fields = fields->tail)
        traverseExp(env, depth, fields->head->exp);
      break;
    }
    case A_assignExp: {
      traverseVar(env, depth, e->u.assign.var);
      traverseExp(env, depth, e->u.assign.exp);
      break;
    }
    case A_ifExp: {
      traverseExp(env, depth, e->u.iff.cond);
      traverseExp(env, depth, e->u.iff.then);
      if (e->u.iff.elsee) 
        traverseExp(env, depth, e->u.iff.elsee);
      break;
    }
    case A_forExp: {
      S_beginScope(env);
      e->u.forr.escape = FALSE;
      S_enter(env, e->u.forr.var, E_EscEntry(depth, &(e->u.forr.escape)));
      traverseExp(env, depth, e->u.forr.lo);
      traverseExp(env, depth, e->u.forr.hi);
      traverseExp(env, depth, e->u.forr.body);
      S_endScope(env);
      break;
    }
    case A_arrayExp: {
      traverseExp(env, depth, e->u.array.size);
      traverseExp(env, depth, e->u.array.init);
    }
    default: break;
  }
}

static void traverseDec(S_table env, int depth, A_dec d) {
  switch (d->kind) {
    case A_functionDec: {
      for (A_fundecList fundecs = d->u.function; fundecs; fundecs = fundecs->tail) {
        S_beginScope(env);
        for (A_fieldList fields = fundecs->head->params; fields; fields = fields->tail) {
          fields->head->escape = FALSE;
          S_enter(env, fields->head->name, E_EscEntry(depth + 1, &(fields->head->escape)));
        }
        traverseExp(env, depth + 1, fundecs->head->body);
        S_endScope(env);
      }
      break;
    }
    case A_varDec: {
      traverseExp(env, depth, d->u.var.init);
      d->u.var.escape = FALSE;
      S_enter(env, d->u.var.var, E_EscEntry(depth, &(d->u.var.escape)));
      break;
    }
    case A_typeDec: break;
    default: assert(0); break;
  }
}

static void traverseVar(S_table env, int depth, A_var v) {
  switch (v->kind) {
    case A_simpleVar: {
      E_enventry x = S_look(env, v->u.simple);
      if (x && depth > x->u.esc.d)
        *(x->u.esc.e) = TRUE;
      break;
    }
    case A_fieldVar: {
      traverseVar(env, depth, v->u.field.var);
      break;
    }
    case A_subscriptVar: {
      traverseVar(env, depth, v->u.subscript.var);
      traverseExp(env, depth, v->u.subscript.exp);
      break;
    }
    default: assert(0); break;
  }
}

/* ---------- Static Methods End ---------- */
void ESC_findEscape(A_exp e) {
  S_table eenv = E_base_eenv();
  S_beginScope(eenv);
  traverseExp(eenv, 0, e);
  S_endScope(eenv);
} 