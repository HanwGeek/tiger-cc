/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: color module implement
 * @Date: 2020-02-17 20:05:43
 * @Last Modified: 2020-02-19 17:42:41
 */
#include <stdlib.h>
#include "color.h"
#include "graph.h"
#include "temp.h"

const int STACK_SIZE = 256;
const int K = 6;
typedef struct stack_ *stack;
struct stack_ {
  G_node* u;
  int top, size;
};

static stack selectStack = NULL;
static G_nodeList simplifyWorklist = NULL;
static stack Stack();
static void stackPush(stack s, G_node t);
static G_node stackPop(stack s);
static G_node Simplify();
static void DecrementDegree();

static stack Stack(void) {
  stack s = checked_malloc(sizeof(*s));
  s->u = (G_node*)malloc(STACK_SIZE * sizeof(G_node));
  s->top = 0; s->size = STACK_SIZE;
  return s;
}

static void stackPush(stack s, G_node n) {
  if (s->top + 1 == s->size) {
    s->size += s->size >> 1;
    s->u = (G_node*)realloc(s->u, s->size * sizeof(G_node));
  }
  s->u[s->top++] = n;
}

static G_node stackPop(stack s) {
  assert(s->top == 0);
  return s->u[s->top--];
}

static G_node Simplify() {
  for (G_nodeList nodes = simplifyWorklist; nodes; nodes = nodes->tail) {
    G_node n = G_removeList(simplifyWorklist, nodes->head);
    stackPush(selectStack, n);
    for (G_nodeList succs = G_succ(n); succs; succs = succs->tail)
      G_rmEdge(n, succs->head);
    for (G_nodeList preds = G_pred(n); preds; preds = preds->tail)
      G_rmEdge(preds->head, n);
  }
}

struct COL_result COL_color(G_graph ig, Temp_map initial, Temp_tempList regs) {
  
}
