<!--
 * @Author: HanwGeek
 * @Github: https://github.com/HanwGeek
 * @Description: README of tiger-cc.
 * @Date: 2019-10-07 15:10:57
 * @Last Modified: 2020-04-30 14:26:44
 -->
# Tiger-cc
 * The program works of *Modern Compiler Implementations in C*
    《现代编译原理:C语言描述》课后程序设计作业
 * Current progress: Register Allocation
   当前进度: 第11章(弃坑) 

## 模块结构
### table 表:
* binder: {*key* => *value*}绑定链表单元
* TAB_table: *binder[TABSIZE]* 链式表 + 栈

### symbol 标识符: 
* S_symbol: 由*string*和*next*指针类型组成的链表单元
* hashtable: *S_symbol指针*类型链表数组, 即string hash表
* S_table: {*S_symbol* => *value*}的链表

### absyn 抽象语法:
* A_var: 变量(左值)
  * A_simpleVar(*S_symbol*): *lvalue* 变量, 由S_symbol表示
  * A_fieldVar(*A_var*, *S_symbol*): *lvalue.id* 记录域, A_var表示该变量所在的记录
  * A_subscriptVar(*A_var*, *A_exp*): *lvalue[exp]* 数组下标, A_var表示数组, Aexp表示数组后方括号内表达式值作为索引

* A_ty: 数据类型
  * A_nameTy(*S_symbol*): 已有(内建|先前声明)类型, S_symbol为类型标识符
  * A_recordTy(*A_fieldList*): 记录类型, A_fieldList为记录域列表
  * A_arrayTy(*S_symbol*): 数组类型, S_symbol为数组元素类型标识符

* A_dec: 声明
  * A_varDec(*S_symbol*, *S_symbol*, *A_exp*): 变量声明, S_symbol分别为变量和类型标识符, A_exp为变量初值表达式
  * A_typeDec(*A_nametyList*): 类型声明, A_nametyList为数据类型列表
  * A_functionDec(*A_fundecList*): 函数声明, A_fundecList为函数声明列表

* A_field(*S_symbol*, *S_symbol*): 记录域, S_symbol分别为域名和域类型
* A_fieldList(*A_field*, *A_fieldList*): 记录域列表
* A_fundec(*S_symbol*, *A_fieldList*, *S_symbol*): 函数声明, S_symbol分别为函数标识符和返回值标识符, A_fieldList为形参列表
* A_fundecList(*A_fundec*, *A_fundecList*):函数声明列表
* A_namety(*S_symbol*, *A_ty*): 类型, S_symbol为数据类型标识符, A_ty为数据类型
* A_nametyList(*A_namety*, *A_nametyList*): 数据类型列表
* A_efield(*S_symbol*, *A_exp*): 记录域值表达式, S_symbol为域标识符, A_exp为记录域值 
* A_efieldList(*A_efield*, *A_efieldList*): 记录域值列表

* A_exp: 表达式
  * A_varExp(*A_var*): 变量表达式(左值)
  * A_nilExp(): nil表达式
  * A_intExp(*int*): 整型字面量表达式
  * A_stringExp(*string*): 字符串型字面量表达式
  * A_callExp(*S_symbol*, *A_expList*): 函数调用表达式, S_symbol为函数标识符, A_expList为形参列表
  * A_opExp(*A_oper*, *A_exp*, *A_exp*): 算术操作表达式, A_oper为操作符, A_exp为左右表达式
  * A_recordExp(*S_symbol*, *A_efieldList*): 记录创建表达式,  S_symbol为记录标识符, A_efieldList为记录项列表
  * A_seqExp(*A_expList*): 表达式序列
  * A_assignExp(*A_var*, *A_exp*): 赋值表达式, A_var为变量, A_exp为值表达式
  * A_ifExp(*A_exp*, *A_exp*, *A_exp*): if-条件表达式, A_exp分别为条件表达式, then表达式, else表达式
  * A_whileExp(*A_exp*, *A_exp*): while-循环表达式, A_exp分别为条件表达式, then表达式
  * A_forExp(*S_symbol*, *A_exp*, *A_exp*, *A_exp*): for-循环表达式, S_symbol为循环变量标识符, A_exp分别为起始值, 终值和循环节表达式
  * A_breakExp: break表达式
  * A_letExp(*A_decList*, *A_exp*): let表达式, A_decList为let定义表达式, A_exp为主体表达式
  * A_arrayExp(*S_symbol*, *A_exp*, *A_exp*): 数组创建表达式, S_symbol为标识符, A_exp分别为数组大小和初始值表达式

### type 类型:
* Ty_ty: 类型标识符
  * Ty_record: 记录类型标识符
  * Ty_nil: nil类型标识符
  * Ty_int: int类型标识符
  * Ty_string: 字符串类型标识符
  * Ty_array: 数组类型标识符
  * Ty_name: 命名类型标识符, 即记录域类型
  * Ty_void: 无返回值类型标识符
* Ty_tyList: 类型标识符列表
* Ty_field: 记录域标识符
* Ty_fieldList: 记录域标识符列表:

### env 环境:
* tenv: 类型环境, 标识符 => Ty_ty类型的映射
* venv: 值环境, 标识符 => E_enventry的映射
* E_enventry: 环境登记项
  * E_varEntry(*Ty_ty*): 变量标识符登记项,  Ty_ty为其类型
  * E_funEntry(*Ty_tyList*, *Ty_ty*): 函数标识符登记项,  Ty_tyList为形参类型列表, Ty_ty为返回值类型

### semant 语义:
* expty(*Tr_exp*, *Ty_ty*): 转换后的中间代码表达式和其类型

### Temp 临时变量:
* Temp_temp: 局部变量名, 位于寄存器
* Temp_label: 静态存储地址名, 位于主存

### Frame 栈帧:
* F_access: 位于栈帧或寄存器的形参或局部变量的抽象
  * inFrame(*int*): 位于栈帧
  * inReg(*Temp_temp*): 位于寄存器
* F_accessList: F_access表
* F_frame(*Temp_label*, *F_accessList*): 栈帧单元

### Tree 中间表达树:
* T_stm: 无返回值的表达式
  * T_Seq(*s1*, *s2*): T_stm序列, s1后紧跟s2
  * T_Label（*n*): 定义名字n的常数值为当前机器码的地址，类似于标号
  * T_JUMP(*e*, *labs*): 跳转指令, 将控制转移到地址e， labs指出表达式e可能计算出的所有目标地址
  * T_CJUMP: 条件跳转指令(*o*, *e1*, *e2*, *t*, *f*): 计算e1和e2生成a, b; 根据关系操作符o比较a, b并跳转到t或f
  * T_MOVE: 存取指令
    * T_MOVE(*Temp t*, *e*): 计算e并将结果送入临时单元t
    * T_MOVE(*MEM(e1)*, *e2*): 计算e1，由它生成地址a; 然后计算e2， 将结果存储在从a开始的wordSize个字节中国
  * T_EXP(*e*): 执行T_exp但不返回结果
* T_exp: 有返回值的表达式
  * T_BINOP(*o*, *e1*, *e2*): 二元操作符, 对e1, e2施加二元操作o
  * T_MEM(*e*): 字节内容, 开始与地址e的wordSize个字节内容
  * T_TEMP（*t*): 临时变量, 类似与寄存器
  * T_ESEQ(*s*, *e*): 计算s形成副作用, 计算e作为表达式结果 
  * T_NAME(*n*): 汇编语言标号
  * T_CONST(*i*): 整型常量
  * T_CALL(*f*, *l*): 过程调用, 以参数表l调用f
  
### Translate 转译:
* Cx: 条件跳转结构
* Tr_exp: 转译后的中间表达树