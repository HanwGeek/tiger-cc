/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Utils module
 * @Date: 2019-10-08 15:45:32
 * @Last Modified: 2020-01-13 16:33:28
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "util.h"

void *checked_malloc(int len) {
  void *p = malloc(len);
  if (!p) {
    fprintf(stderr, "\nRan out of memory!\n");
    exit(1);
  }
  return p;
}

string String(char *s) {
  string p = checked_malloc(strlen(s) + 1);
  strcpy(p, s);
  return p;
}

string String_format(const char *s, ...) {
  int i = 0, len = 0;
  const char *p = s, *str = NULL;
  char *buf = NULL;
  buf = (char*)malloc(sizeof(*buf) * BUFSIZE);
  va_list ap;
  va_start(ap, s);
  for(; *p; p++) {
    if (*p == '%') {
      switch (*++p) {
        case 'd': {
          char b[32];
          sprintf(b, "%d", va_arg(ap, int));
          str = b;
          break;
        }
        case 's': {
          str = va_arg(ap, const char *);
          break;
        }
        default: assert(0);
      }
      len = strlen(str);
      strncpy(buf + i, str, len);
      i += len;
    } else {
      buf[i++] = *p;
    }
  }
  va_end(ap);
  return buf;
}

U_boolList U_BoolList(bool head, U_boolList tail) {
  U_boolList list = checked_malloc(sizeof(*list));
  list->head = head;
  list->tail = tail;
  return list;
}