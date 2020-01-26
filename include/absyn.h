/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Abstract tree defs header file.
 * @Date: 2019-10-21 13:50:02
 * @Last Modified: 2020-01-26 14:51:05
 */
#ifndef T_ABSYN_H_
#define T_ABSYN_H_
#include "util.h"
#include "symbol.h"

//* Type Definitions
typedef int A_pos;

//* Abstract syntax variable pointer
typedef struct A_var_ *A_var;
//* Abstract syntax expression pointer
typedef struct A_exp_ *A_exp;
//* Abstract syntax declartion pointer
typedef struct A_dec_ *A_dec;
//* Abstract syntax type pointer,
//* including {A_nameTy, A_recordTy, A_arrayTy}
typedef struct A_ty_ *A_ty;

typedef struct A_decList_ *A_decList;
typedef struct A_expList_ *A_expList;
//* Abstract syntax type field{sym, type} pointer
typedef struct A_field_ *A_field;
//* Abstract syntax field list pointer
typedef struct A_fieldList_ *A_fieldList;
typedef struct A_fundec_ *A_fundec;
typedef struct A_fundecList_ *A_fundecList;
//* Abstract syntax of symbol with its type
typedef struct A_namety_ *A_namety;
//* Abstract syntax of symbol & type list
typedef struct A_nametyList_ *A_nametyList;
typedef struct A_efield_ *A_efield;
typedef struct A_efieldList_ *A_efieldList;

// Operator
typedef enum {
  A_plusOp, A_minusOp, A_timesOp, A_divOp,
  A_eqOp, A_neqOp, A_ltOp, A_leOp, A_gtOp, A_geOp,
} A_oper;

// Variable
struct A_var_ {
  enum {
    A_simpleVar,   //* Simple variable: lvalue
    A_fieldVar,    //* Field variable: lvalue.id
    A_subscriptVar //* Subscript variable: lvalue[exp]
  } kind;
  A_pos pos;
  union {
    S_symbol simple; //* simple: the var symbol
    struct {A_var var; S_symbol sym;} field; //* var: the record variable, sym: the field symbol
    struct {A_var var; A_exp exp;} subscript; //* var: the array, exp: the index value
  } u;
};

// expression
struct A_exp_ {
  enum {A_varExp, A_nilExp, A_intExp, A_stringExp, 
  A_callExp, A_opExp, A_recordExp, A_seqExp, A_assignExp,
  A_ifExp, A_whileExp, A_forExp, A_breakExp, A_letExp,
  A_arrayExp
  } kind;
  A_pos pos;
  union {
    A_var var;
    // nil
    int intt;
    string stringg;
    struct {S_symbol func; A_expList args;} call;
    struct {A_oper oper; A_exp left; A_exp right;} op;
    struct {S_symbol typ; A_efieldList fields;} record;
    A_expList seq;
    struct {A_var var; A_exp exp;} assign;
    struct {A_exp cond, then, elsee;} iff;
    struct {A_exp cond, body;} whilee;
    struct {S_symbol var; A_exp lo, hi, body; bool escape;} forr;
    // break
    struct {A_decList decs; A_exp body;} let;
    struct {S_symbol typ; A_exp size, init;} array;
  } u;
};

// Declartion
struct A_dec_ {
  enum {A_functionDec, A_varDec, A_typeDec} kind;
  A_pos pos;
  union {
    A_fundecList function;
    struct {S_symbol var; S_symbol typ; 
            A_exp init; bool escape;} var;
    A_nametyList type;
  } u;
};

// Type
struct A_ty_ {
  enum {A_nameTy, A_recordTy, A_arrayTy} kind;
  A_pos pos;
  union {
    S_symbol name;
    A_fieldList record;
    S_symbol array;
  } u;
};

// Linked lists and nodes of lists
struct A_field_ {S_symbol name, typ; A_pos pos; bool escape;};
struct A_fieldList_ {A_field head; A_fieldList tail;};
struct A_expList_ {A_exp head; A_expList tail;};
struct A_fundec_ {A_pos pos; S_symbol name; 
                  A_fieldList params; S_symbol result; A_exp body;};
struct A_fundecList_ {A_fundec head; A_fundecList tail;};
struct A_decList_ {A_dec head; A_decList tail;};
struct A_namety_ {S_symbol name; A_ty ty;};
struct A_nametyList_ {A_namety head; A_nametyList tail;};
struct A_efield_ {S_symbol name; A_exp exp;};
struct A_efieldList_ {A_efield head; A_efieldList tail;};

// Function Prototypes
A_var A_SimpleVar(A_pos pos, S_symbol sym);
A_var A_FieldVar(A_pos pos, A_var var, S_symbol sym);
A_var A_SubscriptVar(A_pos pos, A_var var, A_exp exp);
A_exp A_VarExp(A_pos pos, A_var var);
A_exp A_NilExp(A_pos pos);
A_exp A_IntExp(A_pos pos, int i);
A_exp A_StringExp(A_pos pos, string s);
A_exp A_CallExp(A_pos pos, S_symbol func, A_expList args);
A_exp A_OpExp(A_pos pos, A_oper oper, A_exp left, A_exp right);
A_exp A_RecordExp(A_pos pos, S_symbol typ, A_efieldList fields);
A_exp A_SeqExp(A_pos pos, A_expList seq);
A_exp A_AssignExp(A_pos pos, A_var var, A_exp exp);
A_exp A_IfExp(A_pos pos, A_exp cond, A_exp then, A_exp elsee);
A_exp A_WhileExp(A_pos pos, A_exp cond, A_exp body);
A_exp A_ForExp(A_pos pos, S_symbol var, A_exp lo, A_exp hi, A_exp body);
A_exp A_BreakExp(A_pos pos);
A_exp A_LetExp(A_pos pos, A_decList decs, A_exp body);
A_exp A_ArrayExp(A_pos pos, S_symbol typ, A_exp size, A_exp init);
A_dec A_FunctionDec(A_pos pos, A_fundecList function);
A_dec A_VarDec(A_pos pos, S_symbol var, S_symbol typ, A_exp init);
A_dec A_TypeDec(A_pos pos, A_nametyList type);
A_ty A_NameTy(A_pos pos, S_symbol name);
A_ty A_RecordTy(A_pos pos, A_fieldList record);
A_ty A_ArrayTy(A_pos pos, S_symbol array);
A_field A_Field(A_pos pos, S_symbol name, S_symbol typ);
A_fieldList A_FieldList(A_field head, A_fieldList tail);
A_expList A_ExpList(A_exp head, A_expList tail);
A_fundec A_Fundec(A_pos pos, S_symbol name, A_fieldList params, S_symbol result, A_exp body);
A_fundecList A_FundecList(A_fundec head, A_fundecList tail);
A_decList A_DecList(A_dec head, A_decList tail);
A_namety A_Namety(S_symbol name, A_ty ty);
A_nametyList A_NametyList(A_namety head, A_nametyList tail);
A_efield A_Efield(S_symbol name, A_exp exp);
A_efieldList A_EfieldList(A_efield head, A_efieldList tail);

#endif