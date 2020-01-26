/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Language type defs header file.
 * @Date: 2019-10-24 21:32:39
 * @Last Modified: 2020-01-26 21:24:37
 */
#ifndef T_TYPE_H_
#define T_TYPE_H_

#include "symbol.h"

//* Type of exp return value
typedef struct Ty_ty_ *Ty_ty;
typedef struct Ty_tyList_ *Ty_tyList;
//* Type of record field,
//* including {symbol, ty}
typedef struct Ty_field_ *Ty_field;
//* Type of record field list
typedef struct Ty_fieldList_ *Ty_fieldList;

struct Ty_ty_ {
  enum {
    Ty_record, //* Record type
    Ty_nil,    //* Nil type
    Ty_int,    //* Int type
    Ty_string, //* String type
    Ty_array,  //* Array type
    Ty_name,   //* User declared type
    Ty_void    //* Void type
  } kind;
  union {
    Ty_fieldList record;
    Ty_ty array;
    struct {S_symbol sym; Ty_ty ty;} name;
  } u;
};

struct Ty_tyList_ {Ty_ty head; Ty_tyList tail;};
struct Ty_field_ {S_symbol name; Ty_ty ty;};
struct Ty_fieldList_ {Ty_field head; Ty_fieldList tail;};
  
Ty_ty Ty_Nil(void);
Ty_ty Ty_Int(void);
Ty_ty Ty_String(void);
Ty_ty Ty_Void(void);

Ty_ty Ty_Record(Ty_fieldList fields);
Ty_ty Ty_Array(Ty_ty ty);
Ty_ty Ty_Name(S_symbol sym, Ty_ty ty);

Ty_tyList Ty_TyList(Ty_ty head, Ty_tyList tail);
Ty_field Ty_Field(S_symbol name, Ty_ty ty);
Ty_fieldList Ty_FieldList(Ty_field head, Ty_fieldList tail);

void Ty_print(Ty_ty t);
void TyList_print(Ty_tyList list);
string Ty_ToString(Ty_ty ty);

#endif