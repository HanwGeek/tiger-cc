/*
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: Var & func entry module header file.
 * @Date: 2019-10-25 13:48:45
 * @Last Modified: 2020-02-24 18:46:55
 */
#ifndef T_ENV_H_
#define T_ENV_H_

#include "types.h"
#include "symbol.h"
#include "translate.h"

typedef struct E_enventry_ *E_enventry;
struct E_enventry_ {
  enum {
    E_varEntry, //* Variable entry
    E_funEntry, //* Function entry
    E_escEntry  //* Escape var entry
  } kind;
  union {
    struct {
      Tr_access access; //* Var access type
      Ty_ty ty; //* Var type
    } var;
    struct {
      Tr_level level;
      Temp_label label;
      Ty_tyList formals; 
      Ty_ty result;} fun;
    struct {
      int d;
      bool e;
    } esc;
  } u;
};

//* Make a env entry of variable
E_enventry E_VarEntry(Tr_access access, Ty_ty ty);
//* Make a env entry of function
E_enventry E_FunEntry(Tr_level level, Temp_label label, Ty_tyList formals, Ty_ty result);
//* Make a env entry of escape var
E_enventry E_EscEntry(int depth, bool* escape);

S_table E_base_tenv(void); /* Ty_ty environment */
S_table E_base_venv(void); /* E_enventry environment */
S_table E_base_eenv(void); /* Escape var environment */

#endif