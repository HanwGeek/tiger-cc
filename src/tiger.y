%{
#include <stdio.h>
#include "util.h"
#include "errormsg.h"
#include "symbol.h"
#include "absyn.h"

int yydebug = 0;
int yylex(void);

A_exp absyn_root;

void yyerror(string s) {
  EM_error(EM_tokPos, "%s", s);
}
%}

%union {
  int pos;
  int ival;
  string sval;
  S_symbol sym;
  A_var var;
  A_exp exp;
  A_dec dec;
  A_ty ty;
  A_decList decList;
  A_expList expList;
  A_field field;
  A_fieldList fieldList;
  A_fundec fundec;
  A_fundecList fundecList;
  A_namety namety;
  A_nametyList nametyList;
  A_efield efield;
  A_efieldList efieldList;
}

%type <sym> id
%type <var> lvalue
%type <exp> program exp condexp letexp funcall
%type <dec> dec vardec fundecs tydecs
%type <ty>  ty
%type <decList> decs
%type <expList> expseq arglist
%type <field> tyfield
%type <fieldList> tyfields
%type <fundec> fundec
%type <namety> tydec 
%type <efieldList> reclist

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

program:	  exp {absyn_root = $1;}
            ; 

exp:        INT  {$$ = A_IntExp(EM_tokPos, $1);}                         
            |STRING {$$ = A_StringExp(EM_tokPos, $1);}
            |NIL {$$ = A_NilExp(EM_tokPos);}
            |lvalue {$$ = A_VarExp(EM_tokPos, $1);}
            |LPAREN expseq RPAREN {$$ = A_SeqExp(EM_tokPos, $2);}
            |funcall {$$ = $1;}
            |condexp {$$ = $1;}
            |letexp  {$$ = $1;}
            |exp PLUS exp {$$ = A_OpExp(EM_tokPos, A_plusOp, $1, $3);}
            |exp MINUS exp {$$ = A_OpExp(EM_tokPos, A_minusOp, $1, $3);}
            |exp TIMES exp {$$ = A_OpExp(EM_tokPos, A_timesOp, $1, $3);}
            |exp DIVIDE exp {$$ = A_OpExp(EM_tokPos, A_divOp, $1, $3);}
            |exp LT exp {$$ = A_OpExp(EM_tokPos, A_ltOp, $1, $3);}
            |exp GT exp {$$ = A_OpExp(EM_tokPos, A_gtOp, $1, $3);}
            |exp LE exp {$$ = A_OpExp(EM_tokPos, A_leOp, $1, $3);}
            |exp GE exp {$$ = A_OpExp(EM_tokPos, A_geOp, $1, $3);}
            |exp EQ exp {$$ = A_OpExp(EM_tokPos, A_eqOp, $1, $3);}
            |exp NEQ exp {$$ = A_OpExp(EM_tokPos, A_neqOp, $1, $3);}
            |exp OR exp {$$ = A_IfExp(EM_tokPos, $1, A_IntExp(EM_tokPos, 1), $3);}
            |exp AND exp {$$ = A_IfExp(EM_tokPos, $1, $3, A_IntExp(EM_tokPos, 0));}
            |id LBRACK exp RBRACK OF exp {$$ = A_ArrayExp(EM_tokPos, $1, $3, $6);}
            |id LBRACE reclist RBRACE {$$ = A_RecordExp(EM_tokPos, $1, $3);}
            |lvalue ASSIGN exp {$$ = A_AssignExp(EM_tokPos, $1, $3);}
            |MINUS exp %prec UMINUS {$$ = A_OpExp(EM_tokPos, A_minusOp, A_IntExp(EM_tokPos, 0), $2);}
            |BREAK {$$ = A_BreakExp(EM_tokPos);}
            ;

reclist:    id EQ exp {$$ = A_EfieldList(A_Efield($1, $3), NULL);}
            |id EQ exp COMMA reclist {$$ = A_EfieldList(A_Efield($1, $3), $5);}
            | {$$ = NULL;}
            ;

expseq:     exp {$$ = A_ExpList($1, NULL);}
            |exp SEMICOLON expseq {$$ = A_ExpList($1, $3);}
            | {$$ = NULL;}
            ;

funcall:    id LPAREN arglist RPAREN {A_CallExp(EM_tokPos, $1, $3);}
            ;

arglist:    exp {$$ = A_ExpList($1, NULL);}
            |exp COMMA arglist {$$ = A_ExpList($1, $3);}
            | {$$ = NULL;}
            ;

condexp:    IF exp THEN exp ELSE exp {$$ = A_IfExp(EM_tokPos, $2, $4, $6);}
            |IF exp THEN exp {$$ = A_IfExp(EM_tokPos, $2, $4, NULL);}
            |WHILE exp DO exp {$$ = A_WhileExp(EM_tokPos, $2, $4);}
            |FOR id ASSIGN exp TO exp DO exp {$$ = A_ForExp(EM_tokPos, $2, $4, $6, $8);}
            ;

letexp:     LET decs IN expseq END {$$ = A_LetExp(EM_tokPos, $2, A_SeqExp(EM_tokPos, $4));}
            ;

decs:       dec decs {$$ = A_DecList($1, $2);}
            | {$$ = NULL;}
            ;

dec:        tydecs {$$ = $1;}
            |vardec {$$ = $1;}
            |fundecs {$$ = $1;}
            ;

tydecs:     tydec %prec LOW {$$ = A_TypeDec(EM_tokPos, A_NametyList($1, NULL));}
            |tydec tydecs {$$ = A_TypeDec(EM_tokPos, A_NametyList($1, $2->u.type));}

tydec:      TYPE id EQ ty {$$ = A_Namety($2, $4);}
            ;

ty:         id {$$ = A_NameTy(EM_tokPos, $1);}
            |LBRACE tyfields RBRACE {$$ = A_RecordTy(EM_tokPos, $2);}
            |ARRAY OF id {$$ = A_ArrayTy(EM_tokPos, $3);}
            ;

tyfields:   tyfield %prec LOW {$$ = A_FieldList($1, NULL);}
            |tyfield COMMA tyfields {$$ = A_FieldList($1, $3);}
            | {$$ = NULL;}
            ;

tyfield:    id COLON id {$$ = A_Field(EM_tokPos, $1, $3);}
            ;

vardec:     VAR id ASSIGN exp {$$ = A_VarDec(EM_tokPos, $2, NULL, $4);}
            |VAR id COLON id ASSIGN exp {$$ = A_VarDec(EM_tokPos, $2, $4, $6);}
            ;

fundecs:    fundec %prec LOW {$$ = A_FunctionDec(EM_tokPos, A_FundecList($1, NULL));}
            |fundec fundecs {$$ = A_FunctionDec(EM_tokPos, A_FundecList($1, $2->u.function));}
            ;

fundec:     FUNCTION id LPAREN tyfields RPAREN EQ exp {$$ = A_Fundec(EM_tokPos, $2, $4, NULL, $7);}
            |FUNCTION id LPAREN tyfields RPAREN COLON id EQ exp {$$ = A_Fundec(EM_tokPos, $2, $4, $7, $9);}
            ;

lvalue:     id %prec LOW {$$ = A_SimpleVar(EM_tokPos, $1);}
            |id LBRACK exp RBRACK {A_SubscriptVar(EM_tokPos, A_SimpleVar(EM_tokPos, $1), $3);}
            |lvalue DOT id {A_FieldVar(EM_tokPos, $1, $3);}
            |lvalue LBRACK exp RBRACK {A_SubscriptVar(EM_tokPos, $1, $3);}
            ;

id:         ID {$$ = S_Symbol($1);}
            ;