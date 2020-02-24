/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: color module implement
 * @Date: 2020-02-17 20:05:43
 * @Last Modified: 2020-02-24 11:56:52
 */
#include <stdlib.h>
#include "color.h"
#include "frame.h"
#include "graph.h"
#include "temp.h"
#include "table.h"

static const int STACK_SIZE = 256;
static const int K = 6;
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

static COL_stack selectStack = NULL;
static COL_nodeList simplifyWorklist = NULL;
static COL_nodeList spillWorklist = NULL;
static G_nodeList coloredNodes = NULL;
static COL_nodeList worklistMoves = NULL;
static COL_nodeList freezeWorklist = NULL;
static int* degree = NULL;
static int *color = NULL;
static int registers[6];
static const string regNames[] = {"eax", "ebx", "ecx", "edx", "esi", "edi"};
//* Adjacent list of graph nodes
static G_nodeList* adjList = NULL;

static COL_stack Stack(void);
static void stackPush(COL_stack s, COL_node t);
static COL_node stackPop(COL_stack s);
static bool stackEmpty(COL_stack s);
static void enterSet(adjSet set, adjPair edge);
static COL_node COL_SimplifyNode(G_node n);
static COL_node COL_SpillNode(G_node n);
static void addNode(COL_nodeList list, COL_node n);
static COL_nodeList rmNode(COL_nodeList list, COL_node n);
static void makeWorkList(Temp_map initial, G_graph g);
//* Remove nodes whose degree < K
static void Simplify(void);
static void assignColors(void);
static bool MoveRelated(G_node n);
static void DecrementDegree();
static bool colorEmpty(void);
static int selectColor(void);

static COL_stack Stack(void) {
  COL_stack s = checked_malloc(sizeof(*s));
  s->u = (COL_node*)malloc(STACK_SIZE * sizeof(COL_node));
  s->top = 0; s->size = STACK_SIZE;
  return s;
}

static void stackPush(COL_stack s, COL_node n) {
  if (s->top + 1 >= s->size) {
    s->size += s->size >> 1;
    s->u = (COL_node*)realloc(s->u, s->size * sizeof(COL_node));
  }
  s->u[s->top++] = n;
}

static COL_node stackPop(COL_stack s) {
  assert(s->top == 0);
  return s->u[--s->top];
}

static bool stackEmpty(COL_stack s) {
  return s->top == 0;
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
    p->prev = list; p->next = list->next;
    if (list->next) list->next->prev = p;
    list->next = p;
  }
}

static COL_nodeList rmNode(COL_nodeList list, COL_node n) {
  while (list->n != n) list = list->next;
  if (list->prev == list) return list->next;
  list->prev->next = list->next;
  return list;
}

static void makeWorkList(Temp_map initial, G_graph g) {
  Temp_temp n = NULL;
  while ((n = Temp_popMap(initial))) {
    //TODO: remove `Temp_tempnum`
    if (degree[Temp_tempnum(n)] >= K) 
      addNode(spillWorklist, COL_SpillNode(G_findInNodes(Temp_look(initial, n), g)));
    else if (MoveRelated(G_findInNodes(n, g))) {

    } else addNode(simplifyWorklist, COL_SimplifyNode(G_findInNodes(Temp_look(initial, n), g)));
  }
}

static void Simplify() {
  for (COL_nodeList nodes = simplifyWorklist; nodes; nodes = nodes->next) {
    simplifyWorklist = rmNode(simplifyWorklist, nodes->n);
    G_node n = nodes->n->n;
    stackPush(selectStack, COL_SimplifyNode(n));
    for (G_nodeList succs = G_succ(n); succs; succs = succs->tail)
      G_rmEdge(n, succs->head);
    for (G_nodeList preds = G_pred(n); preds; preds = preds->tail)  
      G_rmEdge(preds->head, n);
  } 
}

static bool MoveRelated(G_node n) {
  
}

static void DecrementDegree(COL_node n) {
  
}

static void assignColors(void) {
  while (stackEmpty(selectStack)) {
    COL_node n = stackPop(selectStack);
    for (G_nodeList nodes = adjList[Temp_tempnum(n->n)]; nodes; nodes = nodes->tail)
      if (G_inNodeList(nodes->head, coloredNodes))
        registers[color[Temp_tempnum(G_nodeInfo(nodes->head))]] = 0;
    if (colorEmpty())
      addNode(spillWorklist, n);
    else {
      addNode(coloredNodes, n);
      color[Temp_tempnum(n)] = selectColor();
    }
  }
}

static bool colorEmpty(void) {
  for (int i = 0; i < K; i++)
    if (registers[i] != 0)
      return FALSE;
  return TRUE;
}

static int selectColor(void) {
  for (int i = 0; i < K; i++)
    if (registers[i]) return i;
  assert(0);
  return -1;
}

struct COL_result COL_color(G_graph ig, Temp_map initial, Temp_tempList regs) {
  int nodeCnt = G_nodeCount(ig);
  if (!color) color = (int*)checked_malloc(nodeCnt * sizeof(int));
  for (int i = 0; i < K; i++) color[i] = 1;
  if (!degree) degree = (int*)checked_malloc(nodeCnt * sizeof(int));
  if (!adjList) adjList = (G_nodeList*)checked_malloc(nodeCnt * sizeof(G_nodeList));
  if (!selectStack) selectStack = Stack();
  for (G_nodeList nodes = G_nodes(ig); nodes; nodes = nodes->tail) {
    int tempNum = Temp_tempnum(G_nodeInfo(nodes->head));
    degree[tempNum] = G_degree(nodes->head);
    adjList[tempNum] = G_adj(nodes->head);
  }

  makeWorkList(initial, ig);
  while (simplifyWorklist) Simplify();

  Temp_map regMap = Temp_name();
  for (G_nodeList nodes = coloredNodes; nodes; nodes = nodes->tail) {
    Temp_temp temp = G_nodeInfo(nodes->head);
    Temp_enter(regMap, temp, regNames[color[Temp_tempnum(temp)]]);
  }
  struct COL_result result = {regMap, spillWorklist};
  return result;
}

