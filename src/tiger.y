%{
#include <stdio.h>
#include "util.h"
#include "errormsg.h"

int yydebug = 1;
int yylex(void);

void yyerror(string s) {
  EM_error(EM_tokPos, "%s", s);
}
%}

%union {
  int pos;
  int ival;
  string sval;
}
%token <sval> ID STRING 
%token <ival> INT

%token 
COMMA COLON SEMICOLON LPAREN RPAREN LBRACK RBRACK 
LBRACE RBRACE DOT 
PLUS MINUS TIMES DIVIDE EQ NEQ LT LE GT GE
AND OR ASSIGN
ARRAY IF THEN ELSE WHILE FOR TO DO LET IN END OF 
BREAK NIL
FUNCTION VAR TYPE 

%nonassoc LOW
%right FUNCTION FOR DO LET OF IF THEN ELSE
%left RBRACK RBRACE
%right LBRACK LBRACE
%nonassoc ASSIGN
%left OR AND 
%nonassoc EQ NEQ LE GE LT GT
%left PLUS MINUS
%left TIMES DIVIDE
%right UMINUS

%start program

%%

program:	  exp
            ; 

exp:        INT  
            |STRING
            |NIL
            |lvalue
            |LPAREN expseq RPAREN
            |funcall
            |condexp
            |letexp
            |exp PLUS exp
            |exp MINUS exp
            |exp TIMES exp 
            |exp DIVIDE exp
            |exp LT exp
            |exp GT exp
            |exp LE exp
            |exp GE exp 
            |exp EQ exp
            |exp NEQ exp 
            |exp OR exp 
            |exp AND exp
            |id LBRACK exp RBRACK OF exp
            |id LBRACE reclist RBRACE
            |lvalue ASSIGN exp 
            |MINUS exp %prec UMINUS
            |BREAK
            ;

reclist:    id EQ exp
            |id EQ exp COMMA reclist
            |
            ;

expseq:     exp 
            |exp SEMICOLON expseq
            |
            ;

funcall:    id LPAREN arglist RPAREN
            ;

arglist:    exp
            |exp COMMA arglist
            |
            ;

condexp:    IF exp THEN exp ELSE exp
            |IF exp THEN exp 
            |WHILE exp DO exp
            |FOR id ASSIGN exp TO exp DO exp
            ;

letexp:     LET decs IN expseq END
            ;

decs:       dec decs
            |
            ;

dec:        tydec
            |vardec
            |fundec
            ;

tydec:      TYPE id EQ ty
            ;

ty:         id
            |LBRACE tyfields RBRACE
            |ARRAY OF id
            ;

tyfields:   tyfield COMMA tyfields
            |tyfield
            |
            ;

tyfield:    id COLON id
            ;

vardec:     VAR id ASSIGN exp
            |VAR id COLON id ASSIGN exp
            ;

fundec:     FUNCTION id LPAREN tyfields RPAREN EQ exp
            |FUNCTION id LPAREN tyfields RPAREN COLON id EQ exp
            ;

lvalue:     id %prec LOW
            |id LBRACK exp RBRACK
            |lvalue DOT id
            |lvalue LBRACK exp RBRACK
            ;

id:         ID
            ;