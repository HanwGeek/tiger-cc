/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: color module implement
 * @Date: 2020-02-17 20:05:43
 * @Last Modified: 2020-02-20 14:45:10
 */
#include <stdlib.h>
#include "color.h"
#include "graph.h"
#include "temp.h"
#include "table.h"

const int STACK_SIZE = 256;
const int K = 6;
typedef struct COL_stack_ *COL_stack;
typedef struct COL_node_ *COL_node;
typedef struct COL_nodeList_ *COL_nodeList;
typedef struct adjPair_ *adjPair;
typedef struct adjSet_ *adjSet;
struct COL_stack_ {
  COL_node* u;
  int top, size;
};

struct COL_node_ {
  enum {
    COL_Simplify,
    COL_Freeze,
    COL_Spill,
    COL_spilled
  } kind;
  G_node n;
};

struct COL_nodeList_ {
  COL_node n;
  COL_nodeList prev, next, tail;
};

struct adjPair_ {
  G_node u, v;
};

struct adjSet_ {
  TAB_table s;
};

static int* degree = NULL;
static COL_stack selectStack = NULL;
static COL_nodeList simplifyWorklist = NULL;
static COL_nodeList worklistMoves = NULL;
static COL_nodeList freezeWorklist = NULL;
static COL_nodeList spillWorklist = NULL;

static COL_stack Stack();
static void stackPush(COL_stack s, COL_node t);
static COL_node stackPop(COL_stack s);
static void enterSet(adjSet set, adjPair edge);
static COL_node COL_SimplifyNode(G_node n);
static COL_node COL_SpillNode(G_node n);
static void addNode(COL_nodeList list, COL_node n);
static void rmNode(COL_nodeList list, COL_node n);
static void makeWorkList(Temp_map initial);
static COL_node Simplify(void);
static bool MoveRelated(COL_node n);
static void DecrementDegree();

static COL_stack Stack(void) {
  COL_stack s = checked_malloc(sizeof(*s));
  s->u = (G_node*)malloc(STACK_SIZE * sizeof(G_node));
  s->top = 0; s->size = STACK_SIZE;
  return s;
}

static void stackPush(COL_stack s, COL_node n) {
  if (s->top + 1 == s->size) {
    s->size += s->size >> 1;
    s->u = (COL_node*)realloc(s->u, s->size * sizeof(COL_node));
  }
  s->u[s->top++] = n;
}

static COL_node stackPop(COL_stack s) {
  assert(s->top == 0);
  return s->u[s->top--];
}

static void enterSet(adjSet set, adjPair edge) {
  TAB_enter(set->s, edge, NULL);
}

static COL_node COL_SimplifyNode(G_node n) {
  COL_node node = checked_malloc(sizeof(*node));
  node->n = n; node->kind = COL_Simplify;
  return node;
}

static COL_node COL_SpillNode(G_node n) {
  COL_node node = checked_malloc(sizeof(*node));
  node->n = n; node->kind = COL_Spill;
  return node;
}

static void addNode(COL_nodeList list, COL_node n) {
  if (list == NULL) {
    list = checked_malloc(sizeof(*list));
    list->n = n;
    list->next = NULL;
    list->prev = list;
  } else {
    COL_nodeList p = checked_malloc(sizeof(*p));
    list->next->prev = p;
    p->prev = list;
    p->next = list->next;
    list->next = p;
  }
}

static void rmNode(COL_nodeList list, COL_node n) {
  while (list->n != n) list = list->next;
  list->prev->next = list->next;
}

static void makeWorkList(Temp_map initial) {
  Temp_temp n = NULL;
  while (n = Temp_popMap(initial)) {
    if (degree[Temp_tempnum(n)] >= K) 
      addNode(spillWorklist, COL_SpillNode(G_findInNodes(n, initial)));
    else if (MoveRelated(n)) {

    } else addNode(simplifyWorklist, COL_SimplifyNode(G_findInNodes(n, initial)));
  }
}

static COL_node Simplify() {
  for (COL_nodeList nodes = simplifyWorklist; nodes; nodes = nodes->next) {
    rmNode(simplifyWorklist, nodes->n);
    G_node n = nodes->n->n;
    stackPush(selectStack, n);
    for (G_nodeList succs = G_succ(n); succs; succs = succs->tail)
      G_rmEdge(n, succs->head);
    for (G_nodeList preds = G_pred(n); preds; preds = preds->tail)  
      G_rmEdge(preds->head, n);
  } 
}

struct COL_result COL_color(G_graph ig, Temp_map initial, Temp_tempList regs) {
  degree = (int*)checked_malloc(G_nodeCount(ig) * sizeof(int));
  for (G_nodeList nodes = G_nodes(ig); nodes; nodes = nodes->tail)
    degree[Temp_tempnum(G_nodeInfo(nodes->head))] = G_degree(nodes->head);
}