/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Error message module to point out the error place.
 * @Date: 2019-10-08 15:46:20
 * @Last Modified: 2020-01-08 21:17:11
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "util.h"
#include "errormsg.h"

bool anyErrors = FALSE;
static string fileName = "";
static int lineNum = 1;
int EM_tokPos = 0;
extern FILE *yyin;

typedef struct intList {int i; struct intList *rest;} *intList_;

static intList_ intList(int i, intList_ rest) {
  intList_ l = checked_malloc(sizeof(*l));
  l->i = i; l->rest = rest;
  return l;
}

static intList_ linePos = NULL;

void EM_newline(void) {
  // And new line to list head;
  lineNum++;
  linePos = intList(EM_tokPos, linePos);
}

//* Put error pos and message to stderr
void EM_error(int pos, char *message, ...) {
  va_list ap;
  intList_ lines = linePos;
  int num = lineNum;

  anyErrors = TRUE;
  while (lines && lines->i >= pos)
  {
    lines = lines->rest;
    num--;
  }

  if (fileName) fprintf(stderr, "%s ", fileName);
  if (lines) fprintf(stderr, "%d.%d ", num, pos - lines->i);
  va_start(ap, message);
  vfprintf(stderr, message, ap);
  va_end(ap);
  fprintf(stderr,"\n");
}

void EM_reset(string fname) {
  anyErrors = FALSE;
  fileName = fname;
  lineNum = 1;
  linePos = intList(0, NULL);
  yyin = fopen(fname, "r");
  if (!yyin) {EM_error(0, "Cannot open file!"); exit(1);}
}