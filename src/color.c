/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: color module implement
 * @Date: 2020-02-17 20:05:43
 * @Last Modified: 2020-02-17 21:42:00
 */
#include "color.h"
#include "stdlib.h"
#include "string.h"

const int STACK_SIZE = 256;
typedef struct stack_ *stack;
struct stack_ {
  Temp_temp* u;
  int top, size;
};
static stack Stack();
static void stackPush(stack s, Temp_temp t);
static Temp_temp stackPop(stack s);
static Temp_tempList simplify();
static void DecrementDegree();

static stack Stack(void) {
  stack s = checked_malloc(sizeof(*s));
  s->u = (Temp_temp*)malloc(STACK_SIZE * sizeof(Temp_temp));
  s->top = 0; s->size = STACK_SIZE;
  return s;
}

static void stackPush(stack s, Temp_temp t) {
  if (s->top + 1 == s->size) {
    s->size += s->size >> 1;
    s->u = (Temp_temp*)realloc(s->u, s->size * sizeof(Temp_temp));
  }
  s->u[s->top++] = t;
}

static Temp_temp stackPop(stack s) {
  assert(s->top == 0);
  return s->u[s->top--];
}

struct COL_result COL_color(G_graph ig, Temp_map initial, Temp_tempList regs) {
  Temp_tempList simplifyWorklist = NULL;
}
