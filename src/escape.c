/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Date: 2019-11-01 21:40:30
 * @Last Modified: 2020-02-24 18:47:15
 */
#include "escape.h"
#include "semant.h"

static void traverseExp(S_table env, int depth, A_exp e);
static void traverseDec(S_table env, int depth, A_dec d);
static void traverseVar(S_table env, int depth, A_var v);

/* ---------- Static Methods ---------- */
static void traverseExp(S_table env, int depth, A_exp e) {
  switch (e->kind) {
    case A_letExp: {
      for (A_decList decs = e->u.let.decs; decs; decs = decs->tail)
        traverseDec(env, depth, decs->head);
      break;
    }
    case A_seqExp: {
      for (A_expList exps = e->u.seq; exps; exps = exps->tail)
        traverseExp(env, depth, exps->head);
      break;
    }
    default: break;
  }
}

static void traverseDec(S_table env, int depth, A_dec d) {
  switch (d->kind) {
    case A_functionDec: {
      for (A_fundecList fundecs = d->u.function; fundecs; fundecs = fundecs->tail)
        traverseExp(env, depth + 1, fundecs->head->body);
      break;
    }
    case A_varDec: {
      S_enter(eenv, d->u.var.var, E_EscEntry(depth, &(d->u.var.escape)));
      d->u.var.escape = FALSE;
      break;
    }
    case A_typeDec: break;
    default: assert(0); break;
  }
}

static void traverseVar(S_table env, int depth, A_var v) {

}

/* ---------- Static Methods End ---------- */
void ESC_findEscape(A_exp e) {
  if (!eenv) eenv = E_base_eenv();
  traverseExp(eenv, 0, e);
} 