#include "cyc_include.h"

 typedef int Cyc_ptrdiff_t; typedef unsigned int Cyc_size_t; typedef
unsigned short Cyc_wchar_t; typedef unsigned int Cyc_wint_t; typedef char Cyc_u_char;
typedef unsigned short Cyc_u_short; typedef unsigned int Cyc_u_int; typedef
unsigned int Cyc_u_long; typedef unsigned short Cyc_ushort; typedef unsigned int
Cyc_uint; typedef unsigned int Cyc_clock_t; typedef int Cyc_time_t; struct Cyc_timespec{
int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; typedef int Cyc_daddr_t; typedef
char* Cyc_caddr_t; typedef unsigned int Cyc_ino_t; typedef unsigned int Cyc_vm_offset_t;
typedef unsigned int Cyc_vm_size_t; typedef char Cyc_int8_t; typedef char Cyc_u_int8_t;
typedef short Cyc_int16_t; typedef unsigned short Cyc_u_int16_t; typedef int Cyc_int32_t;
typedef unsigned int Cyc_u_int32_t; typedef long long Cyc_int64_t; typedef
unsigned long long Cyc_u_int64_t; typedef int Cyc_register_t; typedef short Cyc_dev_t;
typedef int Cyc_off_t; typedef unsigned short Cyc_uid_t; typedef unsigned short
Cyc_gid_t; typedef int Cyc_pid_t; typedef int Cyc_key_t; typedef int Cyc_ssize_t;
typedef char* Cyc_addr_t; typedef int Cyc_mode_t; typedef unsigned short Cyc_nlink_t;
typedef int Cyc_fd_mask; struct Cyc__types_fd_set{ int fds_bits[ 8u]; } ;
typedef struct Cyc__types_fd_set Cyc__types_fd_set; typedef char* Cyc_Cstring;
typedef struct _tagged_string Cyc_string; typedef struct _tagged_string Cyc_string_t;
typedef struct _tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void*
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef
struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg[ 15u];
struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Not_found[ 14u];
extern char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{ char*
tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern int Cyc_List_length( struct Cyc_List_List*
x); extern char Cyc_List_List_empty[ 15u]; extern char Cyc_List_List_mismatch[
18u]; extern struct Cyc_List_List* Cyc_List_append( struct Cyc_List_List* x,
struct Cyc_List_List* y); extern char Cyc_List_Nth[ 8u]; extern int Cyc_List_mem(
int(* compare)( void*, void*), struct Cyc_List_List* l, void* x); extern struct
Cyc_List_List* Cyc_List_filter_c( int(* f)( void*, void*), void* env, struct Cyc_List_List*
l); struct Cyc_Stdio___sFILE; typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError[ 18u]; struct
Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Stdio_FileCloseError[ 19u]; extern int Cyc_String_strcmp( struct
_tagged_string s1, struct _tagged_string s2); extern int Cyc_String_strptrcmp(
struct _tagged_string* s1, struct _tagged_string* s2); extern int Cyc_String_zstrptrcmp(
struct _tagged_string*, struct _tagged_string*); struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col; }
; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit[
9u]; struct Cyc_Position_Segment; typedef struct Cyc_Position_Segment* Cyc_Position_seg_t;
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern char Cyc_Position_Nocontext[ 14u]; typedef struct
_tagged_string* Cyc_Absyn_field_name_t; typedef struct _tagged_string* Cyc_Absyn_var_t;
typedef struct _tagged_string* Cyc_Absyn_tvarname_t; typedef void* Cyc_Absyn_nmspace_t;
struct _tuple0{ void* f1; struct _tagged_string* f2; } ; typedef struct _tuple0*
Cyc_Absyn_qvar_t; typedef struct _tuple0* Cyc_Absyn_qvar_opt_t; typedef struct
_tuple0* Cyc_Absyn_typedef_name_t; typedef struct _tuple0* Cyc_Absyn_typedef_name_opt_t;
struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct
Cyc_Absyn_PtrInfo; struct Cyc_Absyn_FnInfo; struct Cyc_Absyn_TunionInfo; struct
Cyc_Absyn_TunionFieldInfo; struct Cyc_Absyn_Exp; struct Cyc_Absyn_Stmt; struct
Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause; struct Cyc_Absyn_Fndecl; struct
Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl; struct Cyc_Absyn_Tuniondecl;
struct Cyc_Absyn_Tunionfield; struct Cyc_Absyn_Enumfield; struct Cyc_Absyn_Enumdecl;
struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl; struct Cyc_Absyn_Decl;
struct Cyc_Absyn_Structfield; typedef void* Cyc_Absyn_scope_t; typedef struct
Cyc_Absyn_Tqual Cyc_Absyn_tqual_t; typedef void* Cyc_Absyn_size_of_t; typedef
void* Cyc_Absyn_kind_t; typedef struct Cyc_Absyn_Tvar* Cyc_Absyn_tvar_t; typedef
void* Cyc_Absyn_sign_t; typedef struct Cyc_Absyn_Conref* Cyc_Absyn_conref_t;
typedef void* Cyc_Absyn_constraint_t; typedef void* Cyc_Absyn_bounds_t; typedef
struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t; typedef struct Cyc_Absyn_FnInfo
Cyc_Absyn_fn_info_t; typedef struct Cyc_Absyn_TunionInfo Cyc_Absyn_tunion_info_t;
typedef struct Cyc_Absyn_TunionFieldInfo Cyc_Absyn_tunion_field_info_t; typedef
void* Cyc_Absyn_type_t; typedef void* Cyc_Absyn_rgntype_t; typedef void* Cyc_Absyn_funcparams_t;
typedef void* Cyc_Absyn_type_modifier_t; typedef void* Cyc_Absyn_cnst_t; typedef
void* Cyc_Absyn_primop_t; typedef void* Cyc_Absyn_incrementor_t; typedef void*
Cyc_Absyn_raw_exp_t; typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_t; typedef
struct Cyc_Absyn_Exp* Cyc_Absyn_exp_opt_t; typedef void* Cyc_Absyn_raw_stmt_t;
typedef struct Cyc_Absyn_Stmt* Cyc_Absyn_stmt_t; typedef struct Cyc_Absyn_Stmt*
Cyc_Absyn_stmt_opt_t; typedef void* Cyc_Absyn_raw_pat_t; typedef struct Cyc_Absyn_Pat*
Cyc_Absyn_pat_t; typedef void* Cyc_Absyn_binding_t; typedef struct Cyc_Absyn_Switch_clause*
Cyc_Absyn_switch_clause_t; typedef struct Cyc_Absyn_Fndecl* Cyc_Absyn_fndecl_t;
typedef struct Cyc_Absyn_Structdecl* Cyc_Absyn_structdecl_t; typedef struct Cyc_Absyn_Uniondecl*
Cyc_Absyn_uniondecl_t; typedef struct Cyc_Absyn_Tunionfield* Cyc_Absyn_tunionfield_t;
typedef struct Cyc_Absyn_Tuniondecl* Cyc_Absyn_tuniondecl_t; typedef struct Cyc_Absyn_Typedefdecl*
Cyc_Absyn_typedefdecl_t; typedef struct Cyc_Absyn_Enumfield* Cyc_Absyn_enumfield_t;
typedef struct Cyc_Absyn_Enumdecl* Cyc_Absyn_enumdecl_t; typedef struct Cyc_Absyn_Vardecl*
Cyc_Absyn_vardecl_t; typedef void* Cyc_Absyn_raw_decl_t; typedef struct Cyc_Absyn_Decl*
Cyc_Absyn_decl_t; typedef void* Cyc_Absyn_designator_t; typedef void* Cyc_Absyn_stmt_annot_t;
typedef void* Cyc_Absyn_attribute_t; typedef struct Cyc_List_List* Cyc_Absyn_attributes_t;
typedef struct Cyc_Absyn_Structfield* Cyc_Absyn_structfield_t; static const int
Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0; struct Cyc_Absyn_Rel_n_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Abs_n= 1;
struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List* f1; } ; static
const int Cyc_Absyn_Static= 0; static const int Cyc_Absyn_Abstract= 1; static
const int Cyc_Absyn_Public= 2; static const int Cyc_Absyn_Extern= 3; static
const int Cyc_Absyn_ExternC= 4; struct Cyc_Absyn_Tqual{ int q_const: 1; int
q_volatile: 1; int q_restrict: 1; } ; static const int Cyc_Absyn_B1= 0; static
const int Cyc_Absyn_B2= 1; static const int Cyc_Absyn_B4= 2; static const int
Cyc_Absyn_B8= 3; static const int Cyc_Absyn_AnyKind= 0; static const int Cyc_Absyn_MemKind=
1; static const int Cyc_Absyn_BoxKind= 2; static const int Cyc_Absyn_RgnKind= 3;
static const int Cyc_Absyn_EffKind= 4; static const int Cyc_Absyn_Signed= 0;
static const int Cyc_Absyn_Unsigned= 1; struct Cyc_Absyn_Conref{ void* v; } ;
static const int Cyc_Absyn_Eq_constr= 0; struct Cyc_Absyn_Eq_constr_struct{ int
tag; void* f1; } ; static const int Cyc_Absyn_Forward_constr= 1; struct Cyc_Absyn_Forward_constr_struct{
int tag; struct Cyc_Absyn_Conref* f1; } ; static const int Cyc_Absyn_No_constr=
0; struct Cyc_Absyn_Tvar{ struct _tagged_string* name; struct Cyc_Absyn_Conref*
kind; } ; static const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b=
0; struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
nullable; struct Cyc_Absyn_Tqual tq; struct Cyc_Absyn_Conref* bounds; } ; struct
Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars; struct Cyc_Core_Opt* effect; void*
ret_typ; struct Cyc_List_List* args; int varargs; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{ struct _tuple0* name; int
is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion= 0; struct Cyc_Absyn_UnknownTunion_struct{
int tag; struct Cyc_Absyn_UnknownTunionInfo f1; } ; static const int Cyc_Absyn_KnownTunion=
1; struct Cyc_Absyn_KnownTunion_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
} ; struct Cyc_Absyn_TunionInfo{ void* tunion_info; struct Cyc_List_List* targs;
void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{ struct _tuple0*
tunion_name; struct _tuple0* field_name; int is_xtunion; } ; static const int
Cyc_Absyn_UnknownTunionfield= 0; struct Cyc_Absyn_UnknownTunionfield_struct{ int
tag; struct Cyc_Absyn_UnknownTunionFieldInfo f1; } ; static const int Cyc_Absyn_KnownTunionfield=
1; struct Cyc_Absyn_KnownTunionfield_struct{ int tag; struct Cyc_Absyn_Tuniondecl*
f1; struct Cyc_Absyn_Tunionfield* f2; } ; struct Cyc_Absyn_TunionFieldInfo{ void*
field_info; struct Cyc_List_List* targs; } ; static const int Cyc_Absyn_VoidType=
0; static const int Cyc_Absyn_Evar= 0; struct Cyc_Absyn_Evar_struct{ int tag;
void* f1; struct Cyc_Core_Opt* f2; int f3; } ; static const int Cyc_Absyn_VarType=
1; struct Cyc_Absyn_VarType_struct{ int tag; struct Cyc_Absyn_Tvar* f1; } ;
static const int Cyc_Absyn_TunionType= 2; struct Cyc_Absyn_TunionType_struct{
int tag; struct Cyc_Absyn_TunionInfo f1; } ; static const int Cyc_Absyn_TunionFieldType=
3; struct Cyc_Absyn_TunionFieldType_struct{ int tag; struct Cyc_Absyn_TunionFieldInfo
f1; } ; static const int Cyc_Absyn_PointerType= 4; struct Cyc_Absyn_PointerType_struct{
int tag; struct Cyc_Absyn_PtrInfo f1; } ; static const int Cyc_Absyn_IntType= 5;
struct Cyc_Absyn_IntType_struct{ int tag; void* f1; void* f2; } ; static const
int Cyc_Absyn_FloatType= 1; static const int Cyc_Absyn_DoubleType= 2; static
const int Cyc_Absyn_ArrayType= 6; struct Cyc_Absyn_ArrayType_struct{ int tag;
void* f1; struct Cyc_Absyn_Tqual f2; struct Cyc_Absyn_Exp* f3; } ; static const
int Cyc_Absyn_FnType= 7; struct Cyc_Absyn_FnType_struct{ int tag; struct Cyc_Absyn_FnInfo
f1; } ; static const int Cyc_Absyn_TupleType= 8; struct Cyc_Absyn_TupleType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_StructType= 9;
struct Cyc_Absyn_StructType_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Structdecl** f3; } ; static const int Cyc_Absyn_UnionType=
10; struct Cyc_Absyn_UnionType_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Uniondecl** f3; } ; static const int Cyc_Absyn_EnumType= 11;
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; static const int Cyc_Absyn_RgnHandleType= 12; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; static const int Cyc_Absyn_TypedefType= 13; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
} ; static const int Cyc_Absyn_HeapRgn= 3; static const int Cyc_Absyn_AccessEff=
14; struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_JoinEff= 15; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_NoTypes= 0; struct Cyc_Absyn_NoTypes_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static
const int Cyc_Absyn_WithTypes= 1; struct Cyc_Absyn_WithTypes_struct{ int tag;
struct Cyc_List_List* f1; int f2; struct Cyc_Core_Opt* f3; } ; static const int
Cyc_Absyn_NonNullable_ps= 0; struct Cyc_Absyn_NonNullable_ps_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Nullable_ps= 1; struct
Cyc_Absyn_Nullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static
const int Cyc_Absyn_TaggedArray_ps= 0; static const int Cyc_Absyn_Regparm_att= 0;
struct Cyc_Absyn_Regparm_att_struct{ int tag; int f1; } ; static const int Cyc_Absyn_Stdcall_att=
0; static const int Cyc_Absyn_Cdecl_att= 1; static const int Cyc_Absyn_Noreturn_att=
2; static const int Cyc_Absyn_Const_att= 3; static const int Cyc_Absyn_Aligned_att=
1; struct Cyc_Absyn_Aligned_att_struct{ int tag; int f1; } ; static const int
Cyc_Absyn_Packed_att= 4; static const int Cyc_Absyn_Section_att= 2; struct Cyc_Absyn_Section_att_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Nocommon_att=
5; static const int Cyc_Absyn_Shared_att= 6; static const int Cyc_Absyn_Unused_att=
7; static const int Cyc_Absyn_Weak_att= 8; static const int Cyc_Absyn_Dllimport_att=
9; static const int Cyc_Absyn_Dllexport_att= 10; static const int Cyc_Absyn_No_instrument_function_att=
11; static const int Cyc_Absyn_Constructor_att= 12; static const int Cyc_Absyn_Destructor_att=
13; static const int Cyc_Absyn_No_check_memory_usage_att= 14; static const int
Cyc_Absyn_Carray_mod= 0; static const int Cyc_Absyn_ConstArray_mod= 0; struct
Cyc_Absyn_ConstArray_mod_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static
const int Cyc_Absyn_Pointer_mod= 1; struct Cyc_Absyn_Pointer_mod_struct{ int tag;
void* f1; void* f2; struct Cyc_Absyn_Tqual f3; } ; static const int Cyc_Absyn_Function_mod=
2; struct Cyc_Absyn_Function_mod_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_TypeParams_mod= 3; struct Cyc_Absyn_TypeParams_mod_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; int f3; } ; static
const int Cyc_Absyn_Attributes_mod= 4; struct Cyc_Absyn_Attributes_mod_struct{
int tag; struct Cyc_Position_Segment* f1; struct Cyc_List_List* f2; } ; static
const int Cyc_Absyn_Char_c= 0; struct Cyc_Absyn_Char_c_struct{ int tag; void* f1;
char f2; } ; static const int Cyc_Absyn_Short_c= 1; struct Cyc_Absyn_Short_c_struct{
int tag; void* f1; short f2; } ; static const int Cyc_Absyn_Int_c= 2; struct Cyc_Absyn_Int_c_struct{
int tag; void* f1; int f2; } ; static const int Cyc_Absyn_LongLong_c= 3; struct
Cyc_Absyn_LongLong_c_struct{ int tag; void* f1; long long f2; } ; static const
int Cyc_Absyn_Float_c= 4; struct Cyc_Absyn_Float_c_struct{ int tag; struct
_tagged_string f1; } ; static const int Cyc_Absyn_String_c= 5; struct Cyc_Absyn_String_c_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Null_c= 0;
static const int Cyc_Absyn_Plus= 0; static const int Cyc_Absyn_Times= 1; static
const int Cyc_Absyn_Minus= 2; static const int Cyc_Absyn_Div= 3; static const
int Cyc_Absyn_Mod= 4; static const int Cyc_Absyn_Eq= 5; static const int Cyc_Absyn_Neq=
6; static const int Cyc_Absyn_Gt= 7; static const int Cyc_Absyn_Lt= 8; static
const int Cyc_Absyn_Gte= 9; static const int Cyc_Absyn_Lte= 10; static const int
Cyc_Absyn_Not= 11; static const int Cyc_Absyn_Bitnot= 12; static const int Cyc_Absyn_Bitand=
13; static const int Cyc_Absyn_Bitor= 14; static const int Cyc_Absyn_Bitxor= 15;
static const int Cyc_Absyn_Bitlshift= 16; static const int Cyc_Absyn_Bitlrshift=
17; static const int Cyc_Absyn_Bitarshift= 18; static const int Cyc_Absyn_Size=
19; static const int Cyc_Absyn_Printf= 20; static const int Cyc_Absyn_Fprintf=
21; static const int Cyc_Absyn_Xprintf= 22; static const int Cyc_Absyn_Scanf= 23;
static const int Cyc_Absyn_Fscanf= 24; static const int Cyc_Absyn_Sscanf= 25;
static const int Cyc_Absyn_PreInc= 0; static const int Cyc_Absyn_PostInc= 1;
static const int Cyc_Absyn_PreDec= 2; static const int Cyc_Absyn_PostDec= 3;
static const int Cyc_Absyn_Const_e= 0; struct Cyc_Absyn_Const_e_struct{ int tag;
void* f1; } ; static const int Cyc_Absyn_Var_e= 1; struct Cyc_Absyn_Var_e_struct{
int tag; struct _tuple0* f1; void* f2; } ; static const int Cyc_Absyn_UnknownId_e=
2; struct Cyc_Absyn_UnknownId_e_struct{ int tag; struct _tuple0* f1; } ; static
const int Cyc_Absyn_Primop_e= 3; struct Cyc_Absyn_Primop_e_struct{ int tag; void*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_AssignOp_e= 4;
struct Cyc_Absyn_AssignOp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Core_Opt* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Increment_e=
5; struct Cyc_Absyn_Increment_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; void*
f2; } ; static const int Cyc_Absyn_Conditional_e= 6; struct Cyc_Absyn_Conditional_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_SeqExp_e= 7; struct Cyc_Absyn_SeqExp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; static const
int Cyc_Absyn_UnknownCall_e= 8; struct Cyc_Absyn_UnknownCall_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_FnCall_e=
9; struct Cyc_Absyn_FnCall_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Throw_e= 10; struct Cyc_Absyn_Throw_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_NoInstantiate_e=
11; struct Cyc_Absyn_NoInstantiate_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
} ; static const int Cyc_Absyn_Instantiate_e= 12; struct Cyc_Absyn_Instantiate_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Cast_e= 13; struct Cyc_Absyn_Cast_e_struct{ int tag; void* f1;
struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Address_e= 14; struct
Cyc_Absyn_Address_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static const
int Cyc_Absyn_New_e= 15; struct Cyc_Absyn_New_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Sizeoftyp_e= 16;
struct Cyc_Absyn_Sizeoftyp_e_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Sizeofexp_e=
17; struct Cyc_Absyn_Sizeofexp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Deref_e= 18; struct Cyc_Absyn_Deref_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_StructMember_e= 19;
struct Cyc_Absyn_StructMember_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct _tagged_string* f2; } ; static const int Cyc_Absyn_StructArrow_e= 20;
struct Cyc_Absyn_StructArrow_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tagged_string* f2; } ; static const int Cyc_Absyn_Subscript_e= 21; struct Cyc_Absyn_Subscript_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; static const
int Cyc_Absyn_Tuple_e= 22; struct Cyc_Absyn_Tuple_e_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_CompoundLit_e= 23; struct _tuple1{ struct Cyc_Core_Opt*
f1; struct Cyc_Absyn_Tqual f2; void* f3; } ; struct Cyc_Absyn_CompoundLit_e_struct{
int tag; struct _tuple1* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e=
24; struct Cyc_Absyn_Array_e_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_Comprehension_e= 25; struct Cyc_Absyn_Comprehension_e_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Struct_e= 26; struct Cyc_Absyn_Struct_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_Structdecl* f4; } ; static const int Cyc_Absyn_Tunion_e= 27;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 28; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_Malloc_e= 29; struct Cyc_Absyn_Malloc_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_UnresolvedMem_e=
30; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 31; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 32; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 33; struct Cyc_Absyn_Fill_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_Exp{ struct Cyc_Core_Opt*
topt; void* r; struct Cyc_Position_Segment* loc; } ; static const int Cyc_Absyn_Skip_s=
0; static const int Cyc_Absyn_Exp_s= 0; struct Cyc_Absyn_Exp_s_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_Seq_s= 1; struct Cyc_Absyn_Seq_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2; } ; static const
int Cyc_Absyn_Return_s= 2; struct Cyc_Absyn_Return_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; } ; static const int Cyc_Absyn_IfThenElse_s= 3; struct Cyc_Absyn_IfThenElse_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt*
f3; } ; static const int Cyc_Absyn_While_s= 4; struct _tuple2{ struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Stmt* f2; } ; struct Cyc_Absyn_While_s_struct{ int tag;
struct _tuple2 f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Break_s=
5; struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
static const int Cyc_Absyn_Continue_s= 6; struct Cyc_Absyn_Continue_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Goto_s= 7;
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s= 8; struct Cyc_Absyn_For_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct _tuple2 f2; struct _tuple2 f3; struct Cyc_Absyn_Stmt*
f4; } ; static const int Cyc_Absyn_Switch_s= 9; struct Cyc_Absyn_Switch_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Fallthru_s= 10; struct Cyc_Absyn_Fallthru_s_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Absyn_Switch_clause** f2; } ; static const
int Cyc_Absyn_Decl_s= 11; struct Cyc_Absyn_Decl_s_struct{ int tag; struct Cyc_Absyn_Decl*
f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Cut_s= 12; struct
Cyc_Absyn_Cut_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const int
Cyc_Absyn_Splice_s= 13; struct Cyc_Absyn_Splice_s_struct{ int tag; struct Cyc_Absyn_Stmt*
f1; } ; static const int Cyc_Absyn_Label_s= 14; struct Cyc_Absyn_Label_s_struct{
int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt* f2; } ; static const
int Cyc_Absyn_Do_s= 15; struct Cyc_Absyn_Do_s_struct{ int tag; struct Cyc_Absyn_Stmt*
f1; struct _tuple2 f2; } ; static const int Cyc_Absyn_TryCatch_s= 16; struct Cyc_Absyn_TryCatch_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Region_s= 17; struct Cyc_Absyn_Region_s_struct{ int tag; struct
Cyc_Absyn_Tvar* f1; struct Cyc_Absyn_Vardecl* f2; struct Cyc_Absyn_Stmt* f3; } ;
struct Cyc_Absyn_Stmt{ void* r; struct Cyc_Position_Segment* loc; struct Cyc_List_List*
non_local_preds; int try_depth; void* annot; } ; static const int Cyc_Absyn_Wild_p=
0; static const int Cyc_Absyn_Var_p= 0; struct Cyc_Absyn_Var_p_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Null_p= 1; static
const int Cyc_Absyn_Int_p= 1; struct Cyc_Absyn_Int_p_struct{ int tag; void* f1;
int f2; } ; static const int Cyc_Absyn_Char_p= 2; struct Cyc_Absyn_Char_p_struct{
int tag; char f1; } ; static const int Cyc_Absyn_Float_p= 3; struct Cyc_Absyn_Float_p_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Absyn_Tuple_p= 4;
struct Cyc_Absyn_Tuple_p_struct{ int tag; struct Cyc_List_List* f1; } ; static
const int Cyc_Absyn_Pointer_p= 5; struct Cyc_Absyn_Pointer_p_struct{ int tag;
struct Cyc_Absyn_Pat* f1; } ; static const int Cyc_Absyn_Reference_p= 6; struct
Cyc_Absyn_Reference_p_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; static
const int Cyc_Absyn_Struct_p= 7; struct Cyc_Absyn_Struct_p_struct{ int tag;
struct Cyc_Absyn_Structdecl* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_List_List* f4; } ; static const int Cyc_Absyn_Tunion_p= 8; struct
Cyc_Absyn_Tunion_p_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1; struct Cyc_Absyn_Tunionfield*
f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4; } ; static const int Cyc_Absyn_Enum_p=
9; struct Cyc_Absyn_Enum_p_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1;
struct Cyc_Absyn_Enumfield* f2; } ; static const int Cyc_Absyn_UnknownId_p= 10;
struct Cyc_Absyn_UnknownId_p_struct{ int tag; struct _tuple0* f1; } ; static
const int Cyc_Absyn_UnknownCall_p= 11; struct Cyc_Absyn_UnknownCall_p_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; static const int Cyc_Absyn_UnknownFields_p= 12; struct Cyc_Absyn_UnknownFields_p_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
} ; struct Cyc_Absyn_Pat{ void* r; struct Cyc_Core_Opt* topt; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Switch_clause{ struct Cyc_Absyn_Pat* pattern; struct
Cyc_Core_Opt* pat_vars; struct Cyc_Absyn_Exp* where_clause; struct Cyc_Absyn_Stmt*
body; struct Cyc_Position_Segment* loc; } ; static const int Cyc_Absyn_Unresolved_b=
0; static const int Cyc_Absyn_Global_b= 0; struct Cyc_Absyn_Global_b_struct{ int
tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Funname_b= 1;
struct Cyc_Absyn_Funname_b_struct{ int tag; struct Cyc_Absyn_Fndecl* f1; } ;
static const int Cyc_Absyn_Param_b= 2; struct Cyc_Absyn_Param_b_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Local_b= 3; struct
Cyc_Absyn_Local_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; static
const int Cyc_Absyn_Pat_b= 4; struct Cyc_Absyn_Pat_b_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; struct Cyc_Absyn_Vardecl{ void* sc; struct _tuple0* name; struct Cyc_Absyn_Tqual
tq; void* type; struct Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int
is_inline; struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
effect; void* ret_type; struct Cyc_List_List* args; int varargs; struct Cyc_Absyn_Stmt*
body; struct Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls;
struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Structfield{ struct
_tagged_string* name; struct Cyc_Absyn_Tqual tq; void* type; struct Cyc_Core_Opt*
width; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Structdecl{ void*
sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Uniondecl{ void*
sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_Tunionfield{
struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_List_List* typs;
struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Tuniondecl{ void* sc;
struct _tuple0* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields;
int is_xtunion; } ; struct Cyc_Absyn_Enumfield{ struct _tuple0* name; struct Cyc_Absyn_Exp*
tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc;
struct _tuple0* name; struct Cyc_List_List* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple0* name; struct Cyc_List_List* tvs; void* defn; } ; static const
int Cyc_Absyn_Var_d= 0; struct Cyc_Absyn_Var_d_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; static const int Cyc_Absyn_Fn_d= 1; struct Cyc_Absyn_Fn_d_struct{ int
tag; struct Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Let_d= 2;
struct Cyc_Absyn_Let_d_struct{ int tag; struct Cyc_Absyn_Pat* f1; struct Cyc_Core_Opt*
f2; struct Cyc_Core_Opt* f3; struct Cyc_Absyn_Exp* f4; int f5; } ; static const
int Cyc_Absyn_Struct_d= 3; struct Cyc_Absyn_Struct_d_struct{ int tag; struct Cyc_Absyn_Structdecl*
f1; } ; static const int Cyc_Absyn_Union_d= 4; struct Cyc_Absyn_Union_d_struct{
int tag; struct Cyc_Absyn_Uniondecl* f1; } ; static const int Cyc_Absyn_Tunion_d=
5; struct Cyc_Absyn_Tunion_d_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1; }
; static const int Cyc_Absyn_Enum_d= 6; struct Cyc_Absyn_Enum_d_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1; } ; static const int Cyc_Absyn_Typedef_d= 7;
struct Cyc_Absyn_Typedef_d_struct{ int tag; struct Cyc_Absyn_Typedefdecl* f1; }
; static const int Cyc_Absyn_Namespace_d= 8; struct Cyc_Absyn_Namespace_d_struct{
int tag; struct _tagged_string* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Using_d= 9; struct Cyc_Absyn_Using_d_struct{ int tag; struct
_tuple0* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d=
10; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static
const int Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1;
struct Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ;
extern char Cyc_Absyn_EmptyAnnot[ 15u]; extern int Cyc_Absyn_qvar_cmp( struct
_tuple0*, struct _tuple0*); extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref*
x); extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct _tagged_string Cyc_Absyn_attribute2string( void*); struct Cyc_PP_Ppstate;
typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t; struct Cyc_PP_Out; typedef
struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc; typedef struct Cyc_PP_Doc*
Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string( void*);
extern struct _tagged_string Cyc_Absynpp_ckind2string( struct Cyc_Absyn_Conref*);
extern struct _tagged_string Cyc_Absynpp_qvar2string( struct _tuple0*); struct
Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef struct Cyc_Set_Set*
Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t; extern struct Cyc_Set_Set*
Cyc_Set_empty( int(* comp)( void*, void*)); extern struct Cyc_Set_Set* Cyc_Set_insert(
struct Cyc_Set_Set* s, void* elt); extern int Cyc_Set_member( struct Cyc_Set_Set*
s, void* elt); extern char Cyc_Set_Absent[ 11u]; struct Cyc_Dict_Dict; typedef
struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef struct Cyc_Dict_Dict* Cyc_Dict_dict_t;
extern char Cyc_Dict_Present[ 12u]; extern char Cyc_Dict_Absent[ 11u]; extern
int Cyc_Dict_member( struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Dict_Dict*
Cyc_Dict_insert( struct Cyc_Dict_Dict* d, void* key, void* data); extern void*
Cyc_Dict_lookup( struct Cyc_Dict_Dict* d, void* key); extern struct Cyc_Core_Opt*
Cyc_Dict_lookup_opt( struct Cyc_Dict_Dict* d, void* key); extern void Cyc_Dict_iter(
void(* f)( void*, void*), struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict*
Cyc_Dict_map( void*(* f)( void*), struct Cyc_Dict_Dict* d); extern struct Cyc_Dict_Dict*
Cyc_Dict_filter_c( int(* f)( void*, void*, void*), void* env, struct Cyc_Dict_Dict*
d); static const int Cyc_Tcenv_VarRes= 0; struct Cyc_Tcenv_VarRes_struct{ int
tag; void* f1; } ; static const int Cyc_Tcenv_StructRes= 1; struct Cyc_Tcenv_StructRes_struct{
int tag; struct Cyc_Absyn_Structdecl* f1; } ; static const int Cyc_Tcenv_TunionRes=
2; struct Cyc_Tcenv_TunionRes_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1;
struct Cyc_Absyn_Tunionfield* f2; } ; static const int Cyc_Tcenv_EnumRes= 3;
struct Cyc_Tcenv_EnumRes_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; struct
Cyc_Absyn_Enumfield* f2; } ; typedef void* Cyc_Tcenv_resolved_t; struct Cyc_Tcenv_Genv{
struct Cyc_Set_Set* namespaces; struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict*
uniondecls; struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict* enumdecls;
struct Cyc_Dict_Dict* typedefs; struct Cyc_Dict_Dict* ordinaries; struct Cyc_List_List*
availables; } ; typedef struct Cyc_Tcenv_Genv* Cyc_Tcenv_genv_t; struct Cyc_Tcenv_Fenv;
typedef struct Cyc_Tcenv_Fenv* Cyc_Tcenv_fenv_t; static const int Cyc_Tcenv_NotLoop_j=
0; static const int Cyc_Tcenv_CaseEnd_j= 1; static const int Cyc_Tcenv_FnEnd_j=
2; static const int Cyc_Tcenv_Stmt_j= 0; struct Cyc_Tcenv_Stmt_j_struct{ int tag;
struct Cyc_Absyn_Stmt* f1; } ; typedef void* Cyc_Tcenv_jumpee_t; static const
int Cyc_Tcenv_Outermost= 0; struct Cyc_Tcenv_Outermost_struct{ int tag; void* f1;
} ; static const int Cyc_Tcenv_Frame= 1; struct Cyc_Tcenv_Frame_struct{ int tag;
void* f1; void* f2; } ; static const int Cyc_Tcenv_Hidden= 2; struct Cyc_Tcenv_Hidden_struct{
int tag; void* f1; void* f2; } ; typedef void* Cyc_Tcenv_frames_t; struct Cyc_Tcenv_Tenv{
struct Cyc_List_List* ns; struct Cyc_Dict_Dict* ae; struct Cyc_Core_Opt* le; } ;
typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern struct Cyc_Tcenv_Genv*
Cyc_Tcenv_empty_genv(); extern struct Cyc_Tcenv_Fenv* Cyc_Tcenv_new_fenv( struct
Cyc_Position_Segment*, struct Cyc_Absyn_Fndecl*); extern struct Cyc_List_List*
Cyc_Tcenv_resolve_namespace( struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*,
struct _tagged_string*, struct Cyc_List_List*); extern struct Cyc_Core_Opt* Cyc_Tcenv_lookup_xtuniondecl(
struct Cyc_Tcenv_Tenv*, struct Cyc_Position_Segment*, struct _tuple0*); extern
int Cyc_Tcenv_all_labels_resolved( struct Cyc_Tcenv_Tenv*); extern char Cyc_Tcutil_TypeErr[
12u]; extern void* Cyc_Tcutil_impos( struct _tagged_string); extern void Cyc_Tcutil_terr(
struct Cyc_Position_Segment*, struct _tagged_string); extern void Cyc_Tcutil_warn(
struct Cyc_Position_Segment*, struct _tagged_string); extern void* Cyc_Tcutil_compress(
void* t); extern int Cyc_Tcutil_coerce_assign( struct Cyc_Tcenv_Tenv*, struct
Cyc_Absyn_Exp*, void*); extern int Cyc_Tcutil_is_function_type( void* t); extern
int Cyc_Tcutil_unify( void*, void*); extern void* Cyc_Tcutil_substitute( struct
Cyc_List_List*, void*); extern void* Cyc_Tcutil_fndecl2typ( struct Cyc_Absyn_Fndecl*);
extern struct Cyc_Absyn_Exp* Cyc_Tcutil_default_initializer( struct Cyc_Tcenv_Tenv*,
void*, struct Cyc_Position_Segment*); extern void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, void*); extern void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*, struct Cyc_Absyn_Fndecl*);
extern void Cyc_Tcutil_check_type( struct Cyc_Position_Segment*, struct Cyc_Tcenv_Tenv*,
struct Cyc_List_List* bound_tvars, void* k, void*); extern void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*, struct Cyc_List_List*); extern int Cyc_Tcutil_equal_tqual(
struct Cyc_Absyn_Tqual tq1, struct Cyc_Absyn_Tqual tq2); extern int Cyc_Tcutil_same_atts(
struct Cyc_List_List*, struct Cyc_List_List*); extern int Cyc_Tcutil_bits_only(
void* t); extern int Cyc_Tcutil_is_const_exp( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Absyn_Exp* e); extern void* Cyc_Tcexp_tcExpInitializer( struct Cyc_Tcenv_Tenv*,
void**, struct Cyc_Absyn_Exp*); extern void Cyc_Tcstmt_tcStmt( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Absyn_Stmt* s, int new_block); extern unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e); extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int
add_cyc_namespace, struct Cyc_List_List* ds); extern struct Cyc_List_List* Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*); static void Cyc_Tc_redecl_err(
struct Cyc_Position_Segment* loc, struct _tagged_string t, struct _tagged_string*
v, struct _tagged_string new_sc, struct _tagged_string old_sc){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp0= t; struct _tagged_string _temp1=* v; struct
_tagged_string _temp2= new_sc; struct _tagged_string _temp3= old_sc; xprintf("redeclaration of %.*s %.*s cannot be %.*s when earlier definition is %.*s",
_temp0.last_plus_one - _temp0.curr, _temp0.curr, _temp1.last_plus_one - _temp1.curr,
_temp1.curr, _temp2.last_plus_one - _temp2.curr, _temp2.curr, _temp3.last_plus_one
- _temp3.curr, _temp3.curr);}));} struct _tuple3{ void* f1; void* f2; } ; static
void* Cyc_Tc_scope_redecl_okay( void* s1, struct Cyc_Core_Opt* fields1, void* s2,
struct Cyc_Core_Opt* fields2, struct Cyc_Position_Segment* loc, struct
_tagged_string t, struct _tagged_string* v){{ struct _tuple3 _temp5=({ struct
_tuple3 _temp4; _temp4.f1= s1; _temp4.f2= s2; _temp4;}); void* _temp35; void*
_temp37; void* _temp39; void* _temp41; void* _temp43; void* _temp45; void*
_temp47; void* _temp49; void* _temp51; void* _temp53; void* _temp55; void*
_temp57; void* _temp59; void* _temp61; void* _temp63; void* _temp65; void*
_temp68; void* _temp70; void* _temp72; void* _temp74; void* _temp76; void*
_temp78; void* _temp80; void* _temp82; void* _temp85; void* _temp87; void*
_temp89; void* _temp91; _LL7: _LL38: _temp37= _temp5.f1; if( _temp37 ==( void*)
Cyc_Absyn_ExternC){ goto _LL36;} else{ goto _LL9;} _LL36: _temp35= _temp5.f2;
if( _temp35 ==( void*) Cyc_Absyn_ExternC){ goto _LL8;} else{ goto _LL9;} _LL9:
_LL42: _temp41= _temp5.f1; goto _LL40; _LL40: _temp39= _temp5.f2; if( _temp39 ==(
void*) Cyc_Absyn_ExternC){ goto _LL10;} else{ goto _LL11;} _LL11: _LL46: _temp45=
_temp5.f1; if( _temp45 ==( void*) Cyc_Absyn_Static){ goto _LL44;} else{ goto
_LL13;} _LL44: _temp43= _temp5.f2; if( _temp43 ==( void*) Cyc_Absyn_Static){
goto _LL12;} else{ goto _LL13;} _LL13: _LL50: _temp49= _temp5.f1; goto _LL48;
_LL48: _temp47= _temp5.f2; if( _temp47 ==( void*) Cyc_Absyn_Static){ goto _LL14;}
else{ goto _LL15;} _LL15: _LL54: _temp53= _temp5.f1; if( _temp53 ==( void*) Cyc_Absyn_Static){
goto _LL52;} else{ goto _LL17;} _LL52: _temp51= _temp5.f2; if( _temp51 ==( void*)
Cyc_Absyn_Public){ goto _LL16;} else{ goto _LL17;} _LL17: _LL58: _temp57= _temp5.f1;
if( _temp57 ==( void*) Cyc_Absyn_Abstract){ goto _LL56;} else{ goto _LL19;}
_LL56: _temp55= _temp5.f2; if( _temp55 ==( void*) Cyc_Absyn_Public){ goto _LL18;}
else{ goto _LL19;} _LL19: _LL62: _temp61= _temp5.f1; goto _LL60; _LL60: _temp59=
_temp5.f2; if( _temp59 ==( void*) Cyc_Absyn_Public){ goto _LL20;} else{ goto
_LL21;} _LL21: _LL66: _temp65= _temp5.f1; if( _temp65 ==( void*) Cyc_Absyn_Static){
goto _LL64;} else{ goto _LL23;} _LL64: _temp63= _temp5.f2; if( _temp63 ==( void*)
Cyc_Absyn_Extern){ goto _LL22;} else{ goto _LL23;} _LL23: _LL71: _temp70= _temp5.f1;
if( _temp70 ==( void*) Cyc_Absyn_Abstract){ goto _LL69;} else{ goto _LL25;}
_LL69: _temp68= _temp5.f2; if( _temp68 ==( void*) Cyc_Absyn_Extern){ goto _LL67;}
else{ goto _LL25;} _LL67: if( fields2 != 0){ goto _LL24;} else{ goto _LL25;}
_LL25: _LL75: _temp74= _temp5.f1; goto _LL73; _LL73: _temp72= _temp5.f2; if(
_temp72 ==( void*) Cyc_Absyn_Extern){ goto _LL26;} else{ goto _LL27;} _LL27:
_LL79: _temp78= _temp5.f1; if( _temp78 ==( void*) Cyc_Absyn_Static){ goto _LL77;}
else{ goto _LL29;} _LL77: _temp76= _temp5.f2; if( _temp76 ==( void*) Cyc_Absyn_Abstract){
goto _LL28;} else{ goto _LL29;} _LL29: _LL83: _temp82= _temp5.f1; if( _temp82 ==(
void*) Cyc_Absyn_Public){ goto _LL81;} else{ goto _LL31;} _LL81: _temp80= _temp5.f2;
if( _temp80 ==( void*) Cyc_Absyn_Abstract){ goto _LL30;} else{ goto _LL31;}
_LL31: _LL88: _temp87= _temp5.f1; if( _temp87 ==( void*) Cyc_Absyn_Extern){ goto
_LL86;} else{ goto _LL33;} _LL86: _temp85= _temp5.f2; if( _temp85 ==( void*) Cyc_Absyn_Abstract){
goto _LL84;} else{ goto _LL33;} _LL84: if( fields1 != 0){ goto _LL32;} else{
goto _LL33;} _LL33: _LL92: _temp91= _temp5.f1; goto _LL90; _LL90: _temp89=
_temp5.f2; if( _temp89 ==( void*) Cyc_Absyn_Abstract){ goto _LL34;} else{ goto
_LL6;} _LL8: goto _LL6; _LL10: Cyc_Tc_redecl_err( loc, t, v,( struct
_tagged_string)({ char* _temp93=( char*)"non-extern \"C\""; struct
_tagged_string _temp94; _temp94.curr= _temp93; _temp94.base= _temp93; _temp94.last_plus_one=
_temp93 + 15; _temp94;}),( struct _tagged_string)({ char* _temp95=( char*)"extern \"C\"";
struct _tagged_string _temp96; _temp96.curr= _temp95; _temp96.base= _temp95;
_temp96.last_plus_one= _temp95 + 11; _temp96;})); goto _LL6; _LL12: goto _LL6;
_LL14: Cyc_Tc_redecl_err( loc, t, v,( struct _tagged_string)({ char* _temp97=(
char*)"non-static"; struct _tagged_string _temp98; _temp98.curr= _temp97;
_temp98.base= _temp97; _temp98.last_plus_one= _temp97 + 11; _temp98;}),( struct
_tagged_string)({ char* _temp99=( char*)"static"; struct _tagged_string _temp100;
_temp100.curr= _temp99; _temp100.base= _temp99; _temp100.last_plus_one= _temp99
+ 7; _temp100;})); goto _LL6; _LL16: Cyc_Tc_redecl_err( loc, t, v,( struct
_tagged_string)({ char* _temp101=( char*)"static"; struct _tagged_string
_temp102; _temp102.curr= _temp101; _temp102.base= _temp101; _temp102.last_plus_one=
_temp101 + 7; _temp102;}),( struct _tagged_string)({ char* _temp103=( char*)"public";
struct _tagged_string _temp104; _temp104.curr= _temp103; _temp104.base= _temp103;
_temp104.last_plus_one= _temp103 + 7; _temp104;})); goto _LL6; _LL18: Cyc_Tc_redecl_err(
loc, t, v,( struct _tagged_string)({ char* _temp105=( char*)"abstract"; struct
_tagged_string _temp106; _temp106.curr= _temp105; _temp106.base= _temp105;
_temp106.last_plus_one= _temp105 + 9; _temp106;}),( struct _tagged_string)({
char* _temp107=( char*)"public"; struct _tagged_string _temp108; _temp108.curr=
_temp107; _temp108.base= _temp107; _temp108.last_plus_one= _temp107 + 7;
_temp108;})); goto _LL6; _LL20: s1=( void*) Cyc_Absyn_Public; goto _LL6; _LL22:
Cyc_Tc_redecl_err( loc, t, v,( struct _tagged_string)({ char* _temp109=( char*)"static";
struct _tagged_string _temp110; _temp110.curr= _temp109; _temp110.base= _temp109;
_temp110.last_plus_one= _temp109 + 7; _temp110;}),( struct _tagged_string)({
char* _temp111=( char*)"extern"; struct _tagged_string _temp112; _temp112.curr=
_temp111; _temp112.base= _temp111; _temp112.last_plus_one= _temp111 + 7;
_temp112;})); goto _LL6; _LL24: Cyc_Tc_redecl_err( loc, t, v,( struct
_tagged_string)({ char* _temp113=( char*)"abstract"; struct _tagged_string
_temp114; _temp114.curr= _temp113; _temp114.base= _temp113; _temp114.last_plus_one=
_temp113 + 9; _temp114;}),( struct _tagged_string)({ char* _temp115=( char*)"transparent";
struct _tagged_string _temp116; _temp116.curr= _temp115; _temp116.base= _temp115;
_temp116.last_plus_one= _temp115 + 12; _temp116;})); goto _LL6; _LL26: goto _LL6;
_LL28: Cyc_Tc_redecl_err( loc, t, v,( struct _tagged_string)({ char* _temp117=(
char*)"static"; struct _tagged_string _temp118; _temp118.curr= _temp117;
_temp118.base= _temp117; _temp118.last_plus_one= _temp117 + 7; _temp118;}),(
struct _tagged_string)({ char* _temp119=( char*)"abstract"; struct
_tagged_string _temp120; _temp120.curr= _temp119; _temp120.base= _temp119;
_temp120.last_plus_one= _temp119 + 9; _temp120;})); goto _LL6; _LL30: Cyc_Tc_redecl_err(
loc, t, v,( struct _tagged_string)({ char* _temp121=( char*)"public"; struct
_tagged_string _temp122; _temp122.curr= _temp121; _temp122.base= _temp121;
_temp122.last_plus_one= _temp121 + 7; _temp122;}),( struct _tagged_string)({
char* _temp123=( char*)"abstract"; struct _tagged_string _temp124; _temp124.curr=
_temp123; _temp124.base= _temp123; _temp124.last_plus_one= _temp123 + 9;
_temp124;})); goto _LL6; _LL32: Cyc_Tc_redecl_err( loc, t, v,( struct
_tagged_string)({ char* _temp125=( char*)"[extern] transparent"; struct
_tagged_string _temp126; _temp126.curr= _temp125; _temp126.base= _temp125;
_temp126.last_plus_one= _temp125 + 21; _temp126;}),( struct _tagged_string)({
char* _temp127=( char*)"abstract"; struct _tagged_string _temp128; _temp128.curr=
_temp127; _temp128.base= _temp127; _temp128.last_plus_one= _temp127 + 9;
_temp128;})); goto _LL6; _LL34: s1=( void*) Cyc_Absyn_Abstract; goto _LL6; _LL6:;}
return s1;} struct _tuple4{ struct Cyc_Absyn_Tvar* f1; void* f2; } ; struct
_tuple5{ struct Cyc_Absyn_Tqual f1; void* f2; } ; static void Cyc_Tc_field_redecl_okay(
struct Cyc_Absyn_Tunionfield* f1, struct Cyc_Absyn_Tunionfield* f2, struct Cyc_List_List*
inst, struct Cyc_Tcenv_Tenv* te, struct _tagged_string* v){ struct Cyc_Position_Segment*
loc= f1->loc; if( Cyc_String_zstrptrcmp((* f1->name).f2,(* f2->name).f2) != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp131=( struct _tagged_string)({
char* _temp129=( char*)"[x]tunion"; struct _tagged_string _temp130; _temp130.curr=
_temp129; _temp130.base= _temp129; _temp130.last_plus_one= _temp129 + 10;
_temp130;}); struct _tagged_string _temp132=* v; struct _tagged_string _temp133=*(*
f1->name).f2; struct _tagged_string _temp134=*(* f2->name).f2; xprintf("redeclaration of %.*s %.*s: field name mismatch %.*s != %.*s",
_temp131.last_plus_one - _temp131.curr, _temp131.curr, _temp132.last_plus_one -
_temp132.curr, _temp132.curr, _temp133.last_plus_one - _temp133.curr, _temp133.curr,
_temp134.last_plus_one - _temp134.curr, _temp134.curr);}));}{ struct Cyc_List_List*
tvs1= f1->tvs; struct Cyc_List_List* tvs2= f2->tvs; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( tvs1) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
tvs2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp137=( struct
_tagged_string)({ char* _temp135=( char*)"[x]tunion"; struct _tagged_string
_temp136; _temp136.curr= _temp135; _temp136.base= _temp135; _temp136.last_plus_one=
_temp135 + 10; _temp136;}); struct _tagged_string _temp138=* v; struct
_tagged_string _temp139=*(* f1->name).f2; xprintf("redeclaration of %.*s %.*s, field %.*s: type parameter number mismatch",
_temp137.last_plus_one - _temp137.curr, _temp137.curr, _temp138.last_plus_one -
_temp138.curr, _temp138.curr, _temp139.last_plus_one - _temp139.curr, _temp139.curr);}));}
for( 0; tvs1 != 0; tvs1=({ struct Cyc_List_List* _temp140= tvs1; if( _temp140 ==
0){ _throw( Null_Exception);} _temp140->tl;}), tvs2=({ struct Cyc_List_List*
_temp141= tvs2; if( _temp141 == 0){ _throw( Null_Exception);} _temp141->tl;})){
inst=({ struct Cyc_List_List* _temp142=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp142->hd=( void*)({ struct _tuple4* _temp143=(
struct _tuple4*) GC_malloc( sizeof( struct _tuple4)); _temp143->f1=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp147= tvs2; if( _temp147 == 0){ _throw( Null_Exception);}
_temp147->hd;}); _temp143->f2=( void*)({ struct Cyc_Absyn_VarType_struct*
_temp144=( struct Cyc_Absyn_VarType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct));
_temp144[ 0]=({ struct Cyc_Absyn_VarType_struct _temp145; _temp145.tag= Cyc_Absyn_VarType;
_temp145.f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp146= tvs1;
if( _temp146 == 0){ _throw( Null_Exception);} _temp146->hd;}); _temp145;});
_temp144;}); _temp143;}); _temp142->tl= inst; _temp142;});}{ struct Cyc_List_List*
typs1= f1->typs; struct Cyc_List_List* typs2= f2->typs; if((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( typs1) !=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)(
typs2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp150=( struct
_tagged_string)({ char* _temp148=( char*)"[x]tunion"; struct _tagged_string
_temp149; _temp149.curr= _temp148; _temp149.base= _temp148; _temp149.last_plus_one=
_temp148 + 10; _temp149;}); struct _tagged_string _temp151=* v; struct
_tagged_string _temp152=*(* f1->name).f2; xprintf("redeclaration of %.*s %.*s, field %.*s: parameter number mismatch",
_temp150.last_plus_one - _temp150.curr, _temp150.curr, _temp151.last_plus_one -
_temp151.curr, _temp151.curr, _temp152.last_plus_one - _temp152.curr, _temp152.curr);}));}
for( 0; typs1 != 0; typs1=({ struct Cyc_List_List* _temp153= typs1; if( _temp153
== 0){ _throw( Null_Exception);} _temp153->tl;}), typs2=({ struct Cyc_List_List*
_temp154= typs2; if( _temp154 == 0){ _throw( Null_Exception);} _temp154->tl;})){
if( ! Cyc_Tcutil_equal_tqual((*(( struct _tuple5*)({ struct Cyc_List_List*
_temp155= typs1; if( _temp155 == 0){ _throw( Null_Exception);} _temp155->hd;}))).f1,(*((
struct _tuple5*)({ struct Cyc_List_List* _temp156= typs2; if( _temp156 == 0){
_throw( Null_Exception);} _temp156->hd;}))).f1)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp159=( struct _tagged_string)({ char* _temp157=( char*)"[x]tunion";
struct _tagged_string _temp158; _temp158.curr= _temp157; _temp158.base= _temp157;
_temp158.last_plus_one= _temp157 + 10; _temp158;}); struct _tagged_string
_temp160=* v; struct _tagged_string _temp161=*(* f1->name).f2; xprintf("redeclaration of %.*s %.*s, field %.*s: parameter qualifier",
_temp159.last_plus_one - _temp159.curr, _temp159.curr, _temp160.last_plus_one -
_temp160.curr, _temp160.curr, _temp161.last_plus_one - _temp161.curr, _temp161.curr);}));}{
void* subst_t2= Cyc_Tcutil_substitute( inst,(*(( struct _tuple5*)({ struct Cyc_List_List*
_temp171= typs2; if( _temp171 == 0){ _throw( Null_Exception);} _temp171->hd;}))).f2);
if( ! Cyc_Tcutil_unify((*(( struct _tuple5*)({ struct Cyc_List_List* _temp162=
typs1; if( _temp162 == 0){ _throw( Null_Exception);} _temp162->hd;}))).f2,
subst_t2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp166=( struct
_tagged_string)({ char* _temp163=( char*)"[x]tunion"; struct _tagged_string
_temp164; _temp164.curr= _temp163; _temp164.base= _temp163; _temp164.last_plus_one=
_temp163 + 10; _temp164;}); struct _tagged_string _temp167=* v; struct
_tagged_string _temp168=*(* f1->name).f2; struct _tagged_string _temp169= Cyc_Absynpp_typ2string((*((
struct _tuple5*)({ struct Cyc_List_List* _temp165= typs1; if( _temp165 == 0){
_throw( Null_Exception);} _temp165->hd;}))).f2); struct _tagged_string _temp170=
Cyc_Absynpp_typ2string( subst_t2); xprintf("redeclaration of %.*s %.*s, field %.*s: parameter type mismatch %.*s != %.*s",
_temp166.last_plus_one - _temp166.curr, _temp166.curr, _temp167.last_plus_one -
_temp167.curr, _temp167.curr, _temp168.last_plus_one - _temp168.curr, _temp168.curr,
_temp169.last_plus_one - _temp169.curr, _temp169.curr, _temp170.last_plus_one -
_temp170.curr, _temp170.curr);}));}}}}}} struct _tuple6{ void* f1; int f2; } ;
static int Cyc_Tc_var_redecl_okay( struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct _tagged_string* name, void* t, void* sc, struct Cyc_List_List* atts,
int check_no_fun){ struct Cyc_Core_Opt* ans=(( struct Cyc_Core_Opt*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->ordinaries,
name); if( ans == 0){ return 0;} else{ void* sc2; void* t2; struct Cyc_List_List*
atts2;{ void* _temp173=(*(( struct _tuple6*)({ struct Cyc_Core_Opt* _temp172=
ans; if( _temp172 == 0){ _throw( Null_Exception);} _temp172->v;}))).f1; void*
_temp181; struct Cyc_Absyn_Fndecl* _temp183; void* _temp185; struct Cyc_Absyn_Vardecl*
_temp187; _LL175: if(*(( int*) _temp173) == Cyc_Tcenv_VarRes){ _LL182: _temp181=(
void*)(( struct Cyc_Tcenv_VarRes_struct*) _temp173)->f1; if(( unsigned int)
_temp181 > 1u?*(( int*) _temp181) == Cyc_Absyn_Funname_b: 0){ _LL184: _temp183=(
struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*) _temp181)->f1;
goto _LL176;} else{ goto _LL177;}} else{ goto _LL177;} _LL177: if(*(( int*)
_temp173) == Cyc_Tcenv_VarRes){ _LL186: _temp185=( void*)(( struct Cyc_Tcenv_VarRes_struct*)
_temp173)->f1; if(( unsigned int) _temp185 > 1u?*(( int*) _temp185) == Cyc_Absyn_Global_b:
0){ _LL188: _temp187=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Global_b_struct*)
_temp185)->f1; goto _LL178;} else{ goto _LL179;}} else{ goto _LL179;} _LL179:
goto _LL180; _LL176: if( check_no_fun){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp189=* name; xprintf("redefinition of function %.*s",
_temp189.last_plus_one - _temp189.curr, _temp189.curr);}));} sc2=( void*)
_temp183->sc; t2=( void*)({ struct Cyc_Core_Opt* _temp190= _temp183->cached_typ;
if( _temp190 == 0){ _throw( Null_Exception);} _temp190->v;}); atts2= _temp183->attributes;
goto _LL174; _LL178: sc2=( void*) _temp187->sc; t2=( void*) _temp187->type;
atts2= _temp187->attributes; goto _LL174; _LL180: return 1; _LL174:;} if( sc ==(
void*) Cyc_Absyn_Static? sc2 !=( void*) Cyc_Absyn_Static: 0){ Cyc_Tcutil_warn(
loc,({ struct _tagged_string _temp191=* name; xprintf("static declaration of %.*s follows non-static declaration",
_temp191.last_plus_one - _temp191.curr, _temp191.curr);}));} if( sc2 ==( void*)
Cyc_Absyn_Static? sc !=( void*) Cyc_Absyn_Static: 0){ Cyc_Tcutil_warn( loc,({
struct _tagged_string _temp192=* name; xprintf("non-static declaration of %.*s follows static declaration",
_temp192.last_plus_one - _temp192.curr, _temp192.curr);}));} if(( sc2 ==( void*)
Cyc_Absyn_ExternC? sc !=( void*) Cyc_Absyn_ExternC: 0)? 1:( sc2 !=( void*) Cyc_Absyn_ExternC?
sc ==( void*) Cyc_Absyn_ExternC: 0)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp193=* name; xprintf("incompatible redeclaration of %.*s",
_temp193.last_plus_one - _temp193.curr, _temp193.curr);}));} else{ if((( sc !=(
void*) Cyc_Absyn_Extern? sc2 !=( void*) Cyc_Absyn_Extern: 0)? !( sc ==( void*)
Cyc_Absyn_Static? sc2 ==( void*) Cyc_Absyn_Static: 0): 0)? !( sc ==( void*) Cyc_Absyn_Public?
sc2 ==( void*) Cyc_Absyn_Public: 0): 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp194=* name; xprintf("incompatible redeclaration of %.*s",
_temp194.last_plus_one - _temp194.curr, _temp194.curr);}));}} if( ! Cyc_Tcutil_unify(
t, t2)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp195=* name; struct
_tagged_string _temp196= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp197= Cyc_Absynpp_typ2string( t2); xprintf("redeclaration of %.*s at a different type: %.*s != %.*s",
_temp195.last_plus_one - _temp195.curr, _temp195.curr, _temp196.last_plus_one -
_temp196.curr, _temp196.curr, _temp197.last_plus_one - _temp197.curr, _temp197.curr);}));}
if( ! Cyc_Tcutil_same_atts( atts, atts2)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp198=* name; xprintf("redeclaration of %.*s with different attributes",
_temp198.last_plus_one - _temp198.curr, _temp198.curr);}));} return 1;}} static
struct Cyc_List_List* Cyc_Tc_transfer_fn_type_atts( void* t, struct Cyc_List_List*
atts){ void* _temp199= Cyc_Tcutil_compress( t); struct Cyc_Absyn_FnInfo _temp205;
struct Cyc_List_List* _temp207; struct Cyc_List_List** _temp209; int _temp210;
struct Cyc_List_List* _temp212; void* _temp214; struct Cyc_Core_Opt* _temp216;
struct Cyc_List_List* _temp218; _LL201: if(( unsigned int) _temp199 > 4u?*(( int*)
_temp199) == Cyc_Absyn_FnType: 0){ _LL206: _temp205=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp199)->f1; _LL219: _temp218=( struct Cyc_List_List*)
_temp205.tvars; goto _LL217; _LL217: _temp216=( struct Cyc_Core_Opt*) _temp205.effect;
goto _LL215; _LL215: _temp214=( void*) _temp205.ret_typ; goto _LL213; _LL213:
_temp212=( struct Cyc_List_List*) _temp205.args; goto _LL211; _LL211: _temp210=(
int) _temp205.varargs; goto _LL208; _LL208: _temp207=( struct Cyc_List_List*)
_temp205.attributes; _temp209=&((( struct Cyc_Absyn_FnType_struct*) _temp199)->f1).attributes;
goto _LL202;} else{ goto _LL203;} _LL203: goto _LL204; _LL202: { struct Cyc_List_List*
res_atts= 0; for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp220= atts;
if( _temp220 == 0){ _throw( Null_Exception);} _temp220->tl;})){ void* _temp222=(
void*)({ struct Cyc_List_List* _temp221= atts; if( _temp221 == 0){ _throw(
Null_Exception);} _temp221->hd;}); int _temp236; _LL224: if(( unsigned int)
_temp222 > 15u?*(( int*) _temp222) == Cyc_Absyn_Regparm_att: 0){ _LL237:
_temp236=( int)(( struct Cyc_Absyn_Regparm_att_struct*) _temp222)->f1; goto
_LL225;} else{ goto _LL226;} _LL226: if( _temp222 ==( void*) Cyc_Absyn_Stdcall_att){
goto _LL227;} else{ goto _LL228;} _LL228: if( _temp222 ==( void*) Cyc_Absyn_Cdecl_att){
goto _LL229;} else{ goto _LL230;} _LL230: if( _temp222 ==( void*) Cyc_Absyn_Noreturn_att){
goto _LL231;} else{ goto _LL232;} _LL232: if( _temp222 ==( void*) Cyc_Absyn_Const_att){
goto _LL233;} else{ goto _LL234;} _LL234: goto _LL235; _LL225: goto _LL227;
_LL227: goto _LL229; _LL229: goto _LL231; _LL231: goto _LL233; _LL233:* _temp209=({
struct Cyc_List_List* _temp238=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp238->hd=( void*)(( void*)({ struct Cyc_List_List*
_temp239= atts; if( _temp239 == 0){ _throw( Null_Exception);} _temp239->hd;}));
_temp238->tl=* _temp209; _temp238;}); continue; _LL235: res_atts=({ struct Cyc_List_List*
_temp240=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp240->hd=( void*)(( void*)({ struct Cyc_List_List* _temp241= atts; if(
_temp241 == 0){ _throw( Null_Exception);} _temp241->hd;})); _temp240->tl=
res_atts; _temp240;}); continue; _LL223:;} return res_atts;} _LL204: return((
struct Cyc_List_List*(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp242=( char*)"transfer_fn_type_atts"; struct
_tagged_string _temp243; _temp243.curr= _temp242; _temp243.base= _temp242;
_temp243.last_plus_one= _temp242 + 22; _temp243;})); _LL200:;} static void Cyc_Tc_tcVardecl(
struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment*
loc, struct Cyc_Absyn_Vardecl* vd){ struct _tagged_string* v=(* vd->name).f2;
void* t=( void*) vd->type; void* sc=( void*) vd->sc; struct Cyc_List_List* atts=
vd->attributes;{ void* _temp244=(* vd->name).f1; struct Cyc_List_List* _temp252;
struct Cyc_List_List* _temp254; _LL246: if(( unsigned int) _temp244 > 1u?*(( int*)
_temp244) == Cyc_Absyn_Rel_n: 0){ _LL253: _temp252=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp244)->f1; if( _temp252 == 0){ goto _LL247;}
else{ goto _LL248;}} else{ goto _LL248;} _LL248: if(( unsigned int) _temp244 > 1u?*((
int*) _temp244) == Cyc_Absyn_Abs_n: 0){ _LL255: _temp254=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp244)->f1; if( _temp254 == 0){ goto _LL249;}
else{ goto _LL250;}} else{ goto _LL250;} _LL250: goto _LL251; _LL247: goto
_LL245; _LL249: goto _LL245; _LL251: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp256=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp257; _temp257.curr= _temp256; _temp257.base= _temp256;
_temp257.last_plus_one= _temp256 + 43; _temp257;})); return; _LL245:;}(* vd->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp258=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp258[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp259; _temp259.tag= Cyc_Absyn_Abs_n; _temp259.f1= te->ns; _temp259;});
_temp258;});{ void* _temp260= Cyc_Tcutil_compress( t); struct Cyc_Absyn_Exp*
_temp267; struct Cyc_Absyn_Tqual _temp269; void* _temp271; _LL262: if((
unsigned int) _temp260 > 4u?*(( int*) _temp260) == Cyc_Absyn_ArrayType: 0){
_LL272: _temp271=( void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp260)->f1;
goto _LL270; _LL270: _temp269=( struct Cyc_Absyn_Tqual)(( struct Cyc_Absyn_ArrayType_struct*)
_temp260)->f2; goto _LL268; _LL268: _temp267=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp260)->f3; if( _temp267 == 0){ goto _LL266;}
else{ goto _LL264;}} else{ goto _LL264;} _LL266: if( vd->initializer != 0){ goto
_LL263;} else{ goto _LL264;} _LL264: goto _LL265; _LL263:{ void* _temp274=( void*)({
struct Cyc_Absyn_Exp* _temp273= vd->initializer; if( _temp273 == 0){ _throw(
Null_Exception);} _temp273->r;}); void* _temp284; struct _tagged_string _temp286;
struct Cyc_List_List* _temp288; struct Cyc_Core_Opt* _temp290; struct Cyc_List_List*
_temp292; _LL276: if(*(( int*) _temp274) == Cyc_Absyn_Const_e){ _LL285: _temp284=(
void*)(( struct Cyc_Absyn_Const_e_struct*) _temp274)->f1; if(( unsigned int)
_temp284 > 1u?*(( int*) _temp284) == Cyc_Absyn_String_c: 0){ _LL287: _temp286=(
struct _tagged_string)(( struct Cyc_Absyn_String_c_struct*) _temp284)->f1; goto
_LL277;} else{ goto _LL278;}} else{ goto _LL278;} _LL278: if(*(( int*) _temp274)
== Cyc_Absyn_UnresolvedMem_e){ _LL291: _temp290=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_UnresolvedMem_e_struct*) _temp274)->f1; goto _LL289; _LL289: _temp288=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnresolvedMem_e_struct*) _temp274)->f2;
goto _LL279;} else{ goto _LL280;} _LL280: if(*(( int*) _temp274) == Cyc_Absyn_Array_e){
_LL293: _temp292=( struct Cyc_List_List*)(( struct Cyc_Absyn_Array_e_struct*)
_temp274)->f1; goto _LL281;} else{ goto _LL282;} _LL282: goto _LL283; _LL277: t=(
void*)( vd->type=( void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp294=(
struct Cyc_Absyn_ArrayType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct));
_temp294[ 0]=({ struct Cyc_Absyn_ArrayType_struct _temp295; _temp295.tag= Cyc_Absyn_ArrayType;
_temp295.f1=( void*) _temp271; _temp295.f2= _temp269; _temp295.f3=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(({ struct _tagged_string _temp296= _temp286;( unsigned int)(
_temp296.last_plus_one - _temp296.curr);}), 0); _temp295;}); _temp294;}))); goto
_LL275; _LL279: _temp292= _temp288; goto _LL281; _LL281: t=( void*)( vd->type=(
void*)(( void*)({ struct Cyc_Absyn_ArrayType_struct* _temp297=( struct Cyc_Absyn_ArrayType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_ArrayType_struct)); _temp297[ 0]=({ struct
Cyc_Absyn_ArrayType_struct _temp298; _temp298.tag= Cyc_Absyn_ArrayType; _temp298.f1=(
void*) _temp271; _temp298.f2= _temp269; _temp298.f3=( struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp((
unsigned int)(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( _temp292), 0);
_temp298;}); _temp297;}))); goto _LL275; _LL283: goto _LL275; _LL275:;} goto
_LL261; _LL265: goto _LL261; _LL261:;} Cyc_Tcutil_check_valid_toplevel_type( loc,
te, t); if( Cyc_Tcutil_is_function_type( t)){ atts= Cyc_Tc_transfer_fn_type_atts(
t, atts);}{ int is_redecl= Cyc_Tc_var_redecl_okay( ge, loc, v, t, sc, atts, 0);
if( sc ==( void*) Cyc_Absyn_Extern? 1: sc ==( void*) Cyc_Absyn_ExternC){ if( vd->initializer
!= 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp299=( char*)"extern declaration should not have initializer";
struct _tagged_string _temp300; _temp300.curr= _temp299; _temp300.base= _temp299;
_temp300.last_plus_one= _temp299 + 47; _temp300;}));}} else{ if( ! Cyc_Tcutil_is_function_type(
t)){ for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp301= atts; if(
_temp301 == 0){ _throw( Null_Exception);} _temp301->tl;})){ void* _temp303=(
void*)({ struct Cyc_List_List* _temp302= atts; if( _temp302 == 0){ _throw(
Null_Exception);} _temp302->hd;}); int _temp323; struct _tagged_string _temp325;
_LL305: if(( unsigned int) _temp303 > 15u?*(( int*) _temp303) == Cyc_Absyn_Aligned_att:
0){ _LL324: _temp323=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp303)->f1;
goto _LL306;} else{ goto _LL307;} _LL307: if(( unsigned int) _temp303 > 15u?*((
int*) _temp303) == Cyc_Absyn_Section_att: 0){ _LL326: _temp325=( struct
_tagged_string)(( struct Cyc_Absyn_Section_att_struct*) _temp303)->f1; goto
_LL308;} else{ goto _LL309;} _LL309: if( _temp303 ==( void*) Cyc_Absyn_Nocommon_att){
goto _LL310;} else{ goto _LL311;} _LL311: if( _temp303 ==( void*) Cyc_Absyn_Shared_att){
goto _LL312;} else{ goto _LL313;} _LL313: if( _temp303 ==( void*) Cyc_Absyn_Unused_att){
goto _LL314;} else{ goto _LL315;} _LL315: if( _temp303 ==( void*) Cyc_Absyn_Weak_att){
goto _LL316;} else{ goto _LL317;} _LL317: if( _temp303 ==( void*) Cyc_Absyn_Dllimport_att){
goto _LL318;} else{ goto _LL319;} _LL319: if( _temp303 ==( void*) Cyc_Absyn_Dllexport_att){
goto _LL320;} else{ goto _LL321;} _LL321: goto _LL322; _LL306: continue; _LL308:
continue; _LL310: continue; _LL312: continue; _LL314: continue; _LL316:
continue; _LL318: continue; _LL320: continue; _LL322: Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp328= Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List*
_temp327= atts; if( _temp327 == 0){ _throw( Null_Exception);} _temp327->hd;}));
struct _tagged_string _temp329= Cyc_Absynpp_qvar2string( vd->name); xprintf("bad attribute %.*s for variable %.*s",
_temp328.last_plus_one - _temp328.curr, _temp328.curr, _temp329.last_plus_one -
_temp329.curr, _temp329.curr);})); goto _LL304; _LL304:;}{ struct Cyc_Absyn_Exp*
e; if( vd->initializer == 0){ e= Cyc_Tcutil_default_initializer( te, t, loc); vd->initializer=(
struct Cyc_Absyn_Exp*) e;} else{ e=( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp330= vd->initializer; if( _temp330 == 0){ _throw( Null_Exception);}
_temp330;});}{ void* t2= Cyc_Tcexp_tcExpInitializer( te,( void**)& t, e); if( !
Cyc_Tcutil_coerce_assign( te, e, t)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp331= Cyc_Absynpp_qvar2string( vd->name); struct
_tagged_string _temp332= Cyc_Absynpp_typ2string( t); struct _tagged_string
_temp333= Cyc_Absynpp_typ2string( t2); xprintf("%.*s is declared with type %.*s but initialized with type %.*s",
_temp331.last_plus_one - _temp331.curr, _temp331.curr, _temp332.last_plus_one -
_temp332.curr, _temp332.curr, _temp333.last_plus_one - _temp333.curr, _temp333.curr);}));}
if( ! Cyc_Tcutil_is_const_exp( te, e)){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp334=( char*)"initializer is not a constant expression";
struct _tagged_string _temp335; _temp335.curr= _temp334; _temp335.base= _temp334;
_temp335.last_plus_one= _temp334 + 41; _temp335;}));}}}} else{ for( 0; atts != 0;
atts=({ struct Cyc_List_List* _temp336= atts; if( _temp336 == 0){ _throw(
Null_Exception);} _temp336->tl;})){ void* _temp338=( void*)({ struct Cyc_List_List*
_temp337= atts; if( _temp337 == 0){ _throw( Null_Exception);} _temp337->hd;});
int _temp356; int _temp358; _LL340: if(( unsigned int) _temp338 > 15u?*(( int*)
_temp338) == Cyc_Absyn_Regparm_att: 0){ _LL357: _temp356=( int)(( struct Cyc_Absyn_Regparm_att_struct*)
_temp338)->f1; goto _LL341;} else{ goto _LL342;} _LL342: if( _temp338 ==( void*)
Cyc_Absyn_Stdcall_att){ goto _LL343;} else{ goto _LL344;} _LL344: if( _temp338
==( void*) Cyc_Absyn_Cdecl_att){ goto _LL345;} else{ goto _LL346;} _LL346: if(
_temp338 ==( void*) Cyc_Absyn_Noreturn_att){ goto _LL347;} else{ goto _LL348;}
_LL348: if( _temp338 ==( void*) Cyc_Absyn_Const_att){ goto _LL349;} else{ goto
_LL350;} _LL350: if(( unsigned int) _temp338 > 15u?*(( int*) _temp338) == Cyc_Absyn_Aligned_att:
0){ _LL359: _temp358=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp338)->f1;
goto _LL351;} else{ goto _LL352;} _LL352: if( _temp338 ==( void*) Cyc_Absyn_Packed_att){
goto _LL353;} else{ goto _LL354;} _LL354: goto _LL355; _LL341: goto _LL343;
_LL343: goto _LL345; _LL345: goto _LL347; _LL347: goto _LL349; _LL349:(( void(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp360=( char*)"tcVardecl: fn type attributes in function var decl"; struct
_tagged_string _temp361; _temp361.curr= _temp360; _temp361.base= _temp360;
_temp361.last_plus_one= _temp360 + 51; _temp361;})); goto _LL339; _LL351: goto
_LL353; _LL353: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp363= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp362= atts; if( _temp362 == 0){ _throw(
Null_Exception);} _temp362->hd;})); xprintf("bad attribute %.*s in function declaration",
_temp363.last_plus_one - _temp363.curr, _temp363.curr);})); goto _LL339; _LL355:
continue; _LL339:;}}} if( !( is_redecl? sc ==( void*) Cyc_Absyn_Extern? 1: Cyc_Tcutil_is_function_type(
t): 0)){ ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d,
struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple6* _temp364=( struct _tuple6*) GC_malloc( sizeof( struct
_tuple6)); _temp364->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp365=(
struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp365[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp366; _temp366.tag= Cyc_Tcenv_VarRes;
_temp366.f1=( void*)(( void*)({ struct Cyc_Absyn_Global_b_struct* _temp367=(
struct Cyc_Absyn_Global_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Global_b_struct));
_temp367[ 0]=({ struct Cyc_Absyn_Global_b_struct _temp368; _temp368.tag= Cyc_Absyn_Global_b;
_temp368.f1= vd; _temp368;}); _temp367;})); _temp366;}); _temp365;}); _temp364->f2=
is_redecl; _temp364;}));}}} static void Cyc_Tc_tcFndecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Fndecl*
fd){ struct _tagged_string* v=(* fd->name).f2; void* sc=( void*) fd->sc;{ void*
_temp369=(* fd->name).f1; struct Cyc_List_List* _temp377; struct Cyc_List_List*
_temp379; _LL371: if(( unsigned int) _temp369 > 1u?*(( int*) _temp369) == Cyc_Absyn_Rel_n:
0){ _LL378: _temp377=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp369)->f1; if( _temp377 == 0){ goto _LL372;} else{ goto _LL373;}} else{ goto
_LL373;} _LL373: if(( unsigned int) _temp369 > 1u?*(( int*) _temp369) == Cyc_Absyn_Abs_n:
0){ _LL380: _temp379=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp369)->f1; goto _LL374;} else{ goto _LL375;} _LL375: goto _LL376; _LL372:
goto _LL370; _LL374:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp381=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp381[ 0]=({ struct Cyc_Core_Impossible_struct _temp382; _temp382.tag= Cyc_Core_Impossible;
_temp382.f1=( struct _tagged_string)({ char* _temp383=( char*)"tc: Abs_n in tcFndecl";
struct _tagged_string _temp384; _temp384.curr= _temp383; _temp384.base= _temp383;
_temp384.last_plus_one= _temp383 + 22; _temp384;}); _temp382;}); _temp381;}));
_LL376: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp385=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp386; _temp386.curr= _temp385; _temp386.base= _temp385;
_temp386.last_plus_one= _temp385 + 43; _temp386;})); return; _LL370:;}(* fd->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp387=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp387[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp388; _temp388.tag= Cyc_Absyn_Abs_n; _temp388.f1= te->ns; _temp388;});
_temp387;}); Cyc_Tcutil_check_fndecl_valid_type( loc, te, fd);{ void* t= Cyc_Tcutil_fndecl2typ(
fd); int is_redecl= Cyc_Tc_var_redecl_okay( ge, loc, v, t, sc, fd->attributes, 1);{
struct Cyc_List_List* atts= fd->attributes; for( 0; atts != 0; atts=({ struct
Cyc_List_List* _temp389= atts; if( _temp389 == 0){ _throw( Null_Exception);}
_temp389->tl;})){ void* _temp391=( void*)({ struct Cyc_List_List* _temp390= atts;
if( _temp390 == 0){ _throw( Null_Exception);} _temp390->hd;}); int _temp399;
_LL393: if( _temp391 ==( void*) Cyc_Absyn_Packed_att){ goto _LL394;} else{ goto
_LL395;} _LL395: if(( unsigned int) _temp391 > 15u?*(( int*) _temp391) == Cyc_Absyn_Aligned_att:
0){ _LL400: _temp399=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp391)->f1;
goto _LL396;} else{ goto _LL397;} _LL397: goto _LL398; _LL394: goto _LL396;
_LL396: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp402= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp401= atts; if( _temp401 == 0){ _throw(
Null_Exception);} _temp401->hd;})); xprintf("bad attribute %.*s for function",
_temp402.last_plus_one - _temp402.curr, _temp402.curr);})); goto _LL392; _LL398:
goto _LL392; _LL392:;}} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple6* _temp403=( struct _tuple6*) GC_malloc( sizeof( struct
_tuple6)); _temp403->f1=( void*)({ struct Cyc_Tcenv_VarRes_struct* _temp404=(
struct Cyc_Tcenv_VarRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_VarRes_struct));
_temp404[ 0]=({ struct Cyc_Tcenv_VarRes_struct _temp405; _temp405.tag= Cyc_Tcenv_VarRes;
_temp405.f1=( void*)(( void*)({ struct Cyc_Absyn_Funname_b_struct* _temp406=(
struct Cyc_Absyn_Funname_b_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Funname_b_struct));
_temp406[ 0]=({ struct Cyc_Absyn_Funname_b_struct _temp407; _temp407.tag= Cyc_Absyn_Funname_b;
_temp407.f1= fd; _temp407;}); _temp406;})); _temp405;}); _temp404;}); _temp403->f2=
is_redecl; _temp403;})); te->le=({ struct Cyc_Core_Opt* _temp408=( struct Cyc_Core_Opt*)
GC_malloc( sizeof( struct Cyc_Core_Opt)); _temp408->v=( void*)(( void*)({ struct
Cyc_Tcenv_Outermost_struct* _temp409=( struct Cyc_Tcenv_Outermost_struct*)
GC_malloc( sizeof( struct Cyc_Tcenv_Outermost_struct)); _temp409[ 0]=({ struct
Cyc_Tcenv_Outermost_struct _temp410; _temp410.tag= Cyc_Tcenv_Outermost; _temp410.f1=(
void*) Cyc_Tcenv_new_fenv( loc, fd); _temp410;}); _temp409;})); _temp408;}); Cyc_Tcstmt_tcStmt(
te, fd->body, 0); if( ! Cyc_Tcenv_all_labels_resolved( te)){ Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp411=( char*)"function has goto statements to undefined labels";
struct _tagged_string _temp412; _temp412.curr= _temp411; _temp412.base= _temp411;
_temp412.last_plus_one= _temp411 + 49; _temp412;}));} te->le= 0;}} static void
Cyc_Tc_tcTypedefdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Typedefdecl* td){ struct
_tagged_string* v=(* td->name).f2;{ void* _temp413=(* td->name).f1; struct Cyc_List_List*
_temp421; struct Cyc_List_List* _temp423; _LL415: if(( unsigned int) _temp413 >
1u?*(( int*) _temp413) == Cyc_Absyn_Rel_n: 0){ _LL422: _temp421=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp413)->f1; if( _temp421 == 0){ goto _LL416;}
else{ goto _LL417;}} else{ goto _LL417;} _LL417: if(( unsigned int) _temp413 > 1u?*((
int*) _temp413) == Cyc_Absyn_Abs_n: 0){ _LL424: _temp423=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp413)->f1; if( _temp423 == 0){ goto _LL418;}
else{ goto _LL419;}} else{ goto _LL419;} _LL419: goto _LL420; _LL416: goto
_LL414; _LL418: goto _LL414; _LL420: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp425=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp426; _temp426.curr= _temp425; _temp426.base= _temp425;
_temp426.last_plus_one= _temp425 + 43; _temp426;})); return; _LL414:;} if((( int(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_member)( ge->typedefs,
v)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp427=* v; xprintf("redeclaration of typedef %.*s",
_temp427.last_plus_one - _temp427.curr, _temp427.curr);})); return;}(* td->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp428=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp428[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp429; _temp429.tag= Cyc_Absyn_Abs_n; _temp429.f1= te->ns; _temp429;});
_temp428;}); Cyc_Tcutil_check_unique_tvars( loc, td->tvs); Cyc_Tcutil_check_type(
loc, te, td->tvs,( void*) Cyc_Absyn_AnyKind,( void*) td->defn); ge->typedefs=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Typedefdecl* data)) Cyc_Dict_insert)( ge->typedefs, v, td);}
struct _tuple7{ struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; } ; static
void Cyc_Tc_tcStructdecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv* ge,
struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Structdecl* sd){ if( sd->name
== 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp430=( char*)"anonymous structs are not allowed at top level";
struct _tagged_string _temp431; _temp431.curr= _temp430; _temp431.base= _temp430;
_temp431.last_plus_one= _temp430 + 47; _temp431;})); return;}{ struct
_tagged_string* v=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp671= sd->name;
if( _temp671 == 0){ _throw( Null_Exception);} _temp671->v;}))).f2;{ struct Cyc_List_List*
atts= sd->attributes; for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp432=
atts; if( _temp432 == 0){ _throw( Null_Exception);} _temp432->tl;})){ void*
_temp434=( void*)({ struct Cyc_List_List* _temp433= atts; if( _temp433 == 0){
_throw( Null_Exception);} _temp433->hd;}); int _temp442; _LL436: if( _temp434 ==(
void*) Cyc_Absyn_Packed_att){ goto _LL437;} else{ goto _LL438;} _LL438: if((
unsigned int) _temp434 > 15u?*(( int*) _temp434) == Cyc_Absyn_Aligned_att: 0){
_LL443: _temp442=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp434)->f1;
goto _LL439;} else{ goto _LL440;} _LL440: goto _LL441; _LL437: continue; _LL439:
continue; _LL441: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp445= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp444= atts; if( _temp444 == 0){ _throw(
Null_Exception);} _temp444->hd;})); struct _tagged_string _temp446=* v; xprintf("bad attribute %.*s in struct %.*s definition",
_temp445.last_plus_one - _temp445.curr, _temp445.curr, _temp446.last_plus_one -
_temp446.curr, _temp446.curr);})); goto _LL435; _LL435:;}}{ struct Cyc_List_List*
tvs2= sd->tvs; for( 0; tvs2 != 0; tvs2=({ struct Cyc_List_List* _temp447= tvs2;
if( _temp447 == 0){ _throw( Null_Exception);} _temp447->tl;})){ struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp463= tvs2; if( _temp463 ==
0){ _throw( Null_Exception);} _temp463->hd;}))->kind); void* _temp448=( void*) c->v;
void* _temp456; _LL450: if( _temp448 ==( void*) Cyc_Absyn_No_constr){ goto
_LL451;} else{ goto _LL452;} _LL452: if(( unsigned int) _temp448 > 1u?*(( int*)
_temp448) == Cyc_Absyn_Eq_constr: 0){ _LL457: _temp456=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp448)->f1; if( _temp456 ==( void*) Cyc_Absyn_MemKind){ goto _LL453;} else{
goto _LL454;}} else{ goto _LL454;} _LL454: goto _LL455; _LL451:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp458=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp458[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp459; _temp459.tag= Cyc_Absyn_Eq_constr; _temp459.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp459;}); _temp458;}))); continue; _LL453:
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp461=* v; struct
_tagged_string _temp462=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp460= tvs2; if( _temp460 == 0){ _throw( Null_Exception);} _temp460->hd;}))->name;
xprintf("struct %.*s attempts to abstract type variable %.*s of kind M",
_temp461.last_plus_one - _temp461.curr, _temp461.curr, _temp462.last_plus_one -
_temp462.curr, _temp462.curr);})); continue; _LL455: continue; _LL449:;}}{
struct Cyc_List_List* tvs= sd->tvs;{ void* _temp465=(*(( struct _tuple0*)({
struct Cyc_Core_Opt* _temp464= sd->name; if( _temp464 == 0){ _throw(
Null_Exception);} _temp464->v;}))).f1; struct Cyc_List_List* _temp473; struct
Cyc_List_List* _temp475; _LL467: if(( unsigned int) _temp465 > 1u?*(( int*)
_temp465) == Cyc_Absyn_Rel_n: 0){ _LL474: _temp473=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp465)->f1; if( _temp473 == 0){ goto _LL468;}
else{ goto _LL469;}} else{ goto _LL469;} _LL469: if(( unsigned int) _temp465 > 1u?*((
int*) _temp465) == Cyc_Absyn_Abs_n: 0){ _LL476: _temp475=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp465)->f1; if( _temp475 == 0){ goto _LL470;}
else{ goto _LL471;}} else{ goto _LL471;} _LL471: goto _LL472; _LL468: goto
_LL466; _LL470: goto _LL466; _LL472: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp477=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp478; _temp478.curr= _temp477; _temp478.base= _temp477;
_temp478.last_plus_one= _temp477 + 43; _temp478;})); return; _LL466:;}(*((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp479= sd->name; if( _temp479 == 0){
_throw( Null_Exception);} _temp479->v;}))).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp480=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp480[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp481; _temp481.tag= Cyc_Absyn_Abs_n;
_temp481.f1= te->ns; _temp481;}); _temp480;}); Cyc_Tcutil_check_unique_tvars(
loc, tvs);{ struct Cyc_Core_Opt* _temp482=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->structdecls, v);
struct Cyc_Core_Opt _temp488; struct Cyc_Absyn_Structdecl** _temp489; _LL484:
if( _temp482 == 0){ goto _LL485;} else{ goto _LL486;} _LL486: if( _temp482 == 0){
goto _LL483;} else{ _temp488=* _temp482; _LL490: _temp489=( struct Cyc_Absyn_Structdecl**)
_temp488.v; goto _LL487;} _LL485: ge->structdecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Structdecl**
data)) Cyc_Dict_insert)( ge->structdecls, v,({ struct Cyc_Absyn_Structdecl**
_temp491=( struct Cyc_Absyn_Structdecl**) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*));
_temp491[ 0]= sd; _temp491;})); if( sd->fields == 0){ return;} goto _LL483;
_LL487: { struct Cyc_Absyn_Structdecl* sd2=* _temp489; struct Cyc_List_List*
tvs2= sd2->tvs; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs)
!=(( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp492=* v; xprintf("redeclaration of struct %.*s has a different number of type parameters",
_temp492.last_plus_one - _temp492.curr, _temp492.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1=({ struct Cyc_List_List*
_temp493= x1; if( _temp493 == 0){ _throw( Null_Exception);} _temp493->tl;}), x2=({
struct Cyc_List_List* _temp494= x2; if( _temp494 == 0){ _throw( Null_Exception);}
_temp494->tl;})){ struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp503= x1; if( _temp503 == 0){ _throw( Null_Exception);}
_temp503->hd;}))->kind); struct Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp502= x2; if( _temp502 == 0){ _throw( Null_Exception);}
_temp502->hd;}))->kind); if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr? c1 !=
c2: 0){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp495=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp495[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp496; _temp496.tag= Cyc_Absyn_Forward_constr; _temp496.f1= c2; _temp496;});
_temp495;})));} if((( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( c2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp498=* v; struct _tagged_string _temp499= Cyc_Absynpp_ckind2string(
c1); struct _tagged_string _temp500=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp497= x1; if( _temp497 == 0){ _throw( Null_Exception);} _temp497->hd;}))->name;
struct _tagged_string _temp501= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of struct %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp498.last_plus_one - _temp498.curr, _temp498.curr, _temp499.last_plus_one -
_temp499.curr, _temp499.curr, _temp500.last_plus_one - _temp500.curr, _temp500.curr,
_temp501.last_plus_one - _temp501.curr, _temp501.curr);}));}}( void*)( sd->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) sd->sc, sd->fields,( void*)
sd2->sc, sd2->fields, loc,( struct _tagged_string)({ char* _temp504=( char*)"struct";
struct _tagged_string _temp505; _temp505.curr= _temp504; _temp505.base= _temp504;
_temp505.last_plus_one= _temp504 + 7; _temp505;}), v));{ struct _tuple7 _temp507=({
struct _tuple7 _temp506; _temp506.f1= sd->fields; _temp506.f2= sd2->fields;
_temp506;}); struct Cyc_Core_Opt* _temp517; struct Cyc_Core_Opt* _temp519;
struct Cyc_Core_Opt* _temp521; struct Cyc_Core_Opt* _temp523; struct Cyc_Core_Opt*
_temp525; struct Cyc_Core_Opt* _temp527; struct Cyc_Core_Opt* _temp529; struct
Cyc_Core_Opt* _temp531; _LL509: _LL520: _temp519= _temp507.f1; if( _temp519 == 0){
goto _LL518;} else{ goto _LL511;} _LL518: _temp517= _temp507.f2; if( _temp517 ==
0){ goto _LL510;} else{ goto _LL511;} _LL511: _LL524: _temp523= _temp507.f1;
goto _LL522; _LL522: _temp521= _temp507.f2; if( _temp521 == 0){ goto _LL512;}
else{ goto _LL513;} _LL513: _LL528: _temp527= _temp507.f1; if( _temp527 == 0){
goto _LL526;} else{ goto _LL515;} _LL526: _temp525= _temp507.f2; goto _LL514;
_LL515: _LL532: _temp531= _temp507.f1; goto _LL530; _LL530: _temp529= _temp507.f2;
goto _LL516; _LL510: return; _LL512:* _temp489= sd; goto _LL508; _LL514: sd->fields=
sd2->fields; sd->tvs= tvs; return; _LL516: { struct Cyc_List_List* inst= 0;{
struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0=({ struct Cyc_List_List*
_temp533= tvs0; if( _temp533 == 0){ _throw( Null_Exception);} _temp533->tl;}),
tvs2=({ struct Cyc_List_List* _temp534= tvs2; if( _temp534 == 0){ _throw(
Null_Exception);} _temp534->tl;})){ inst=({ struct Cyc_List_List* _temp535=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp535->hd=(
void*)({ struct _tuple4* _temp536=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp536->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp540= tvs2; if( _temp540 == 0){ _throw( Null_Exception);} _temp540->hd;});
_temp536->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp537=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp537[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp538; _temp538.tag= Cyc_Absyn_VarType; _temp538.f1=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp539= tvs0; if( _temp539 == 0){ _throw( Null_Exception);}
_temp539->hd;}); _temp538;}); _temp537;}); _temp536;}); _temp535->tl= inst;
_temp535;});}} if( ! Cyc_Tcutil_same_atts( sd->attributes, sd2->attributes)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp541=* v; xprintf("redeclaration of struct %.*s has different attributes",
_temp541.last_plus_one - _temp541.curr, _temp541.curr);}));}{ struct Cyc_List_List*
f1s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp597= sd->fields; if(
_temp597 == 0){ _throw( Null_Exception);} _temp597->v;}); struct Cyc_List_List*
f2s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp596= sd2->fields; if(
_temp596 == 0){ _throw( Null_Exception);} _temp596->v;}); for( 0; f1s != 0? f2s
!= 0: 0; f1s=({ struct Cyc_List_List* _temp542= f1s; if( _temp542 == 0){ _throw(
Null_Exception);} _temp542->tl;}), f2s=({ struct Cyc_List_List* _temp543= f2s;
if( _temp543 == 0){ _throw( Null_Exception);} _temp543->tl;})){ struct Cyc_Absyn_Structfield
_temp547; struct Cyc_List_List* _temp548; struct Cyc_Core_Opt* _temp550; void*
_temp552; struct Cyc_Absyn_Tqual _temp554; struct _tagged_string* _temp556;
struct Cyc_Absyn_Structfield* _temp545=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp544= f1s; if( _temp544 == 0){ _throw( Null_Exception);}
_temp544->hd;}); _temp547=* _temp545; _LL557: _temp556=( struct _tagged_string*)
_temp547.name; goto _LL555; _LL555: _temp554=( struct Cyc_Absyn_Tqual) _temp547.tq;
goto _LL553; _LL553: _temp552=( void*) _temp547.type; goto _LL551; _LL551:
_temp550=( struct Cyc_Core_Opt*) _temp547.width; goto _LL549; _LL549: _temp548=(
struct Cyc_List_List*) _temp547.attributes; goto _LL546; _LL546: { struct Cyc_Absyn_Structfield
_temp561; struct Cyc_List_List* _temp562; struct Cyc_Core_Opt* _temp564; void*
_temp566; struct Cyc_Absyn_Tqual _temp568; struct _tagged_string* _temp570;
struct Cyc_Absyn_Structfield* _temp559=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp558= f2s; if( _temp558 == 0){ _throw( Null_Exception);}
_temp558->hd;}); _temp561=* _temp559; _LL571: _temp570=( struct _tagged_string*)
_temp561.name; goto _LL569; _LL569: _temp568=( struct Cyc_Absyn_Tqual) _temp561.tq;
goto _LL567; _LL567: _temp566=( void*) _temp561.type; goto _LL565; _LL565:
_temp564=( struct Cyc_Core_Opt*) _temp561.width; goto _LL563; _LL563: _temp562=(
struct Cyc_List_List*) _temp561.attributes; goto _LL560; _LL560: if( Cyc_String_zstrptrcmp(
_temp556, _temp570) != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp572=* v; struct _tagged_string _temp573=* _temp556; struct _tagged_string
_temp574=* _temp570; xprintf("redeclaration of struct %.*s: field name mismatch %.*s != %.*s",
_temp572.last_plus_one - _temp572.curr, _temp572.curr, _temp573.last_plus_one -
_temp573.curr, _temp573.curr, _temp574.last_plus_one - _temp574.curr, _temp574.curr);}));}
if( ! Cyc_Tcutil_same_atts( _temp548, _temp562)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp575=* v; struct _tagged_string _temp576=* _temp556; xprintf("redeclaration of struct %.*s: attribute mismatch on field %.*s",
_temp575.last_plus_one - _temp575.curr, _temp575.curr, _temp576.last_plus_one -
_temp576.curr, _temp576.curr);}));} if( ! Cyc_Tcutil_equal_tqual( _temp554,
_temp568)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp577=* v; struct
_tagged_string _temp578=* _temp556; xprintf("redeclaration of struct %.*s: qualifier mismatch on field %.*s",
_temp577.last_plus_one - _temp577.curr, _temp577.curr, _temp578.last_plus_one -
_temp578.curr, _temp578.curr);}));} if( _temp550 != 0? ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp579= _temp550; if(
_temp579 == 0){ _throw( Null_Exception);} _temp579->v;})): 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp580=* v; struct _tagged_string _temp581=*
_temp556; xprintf("redeclaration of struct %.*s: bad bitfield %.*s", _temp580.last_plus_one
- _temp580.curr, _temp580.curr, _temp581.last_plus_one - _temp581.curr, _temp581.curr);}));}
else{ if((( _temp550 != 0? _temp564 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp582= _temp550; if( _temp582
== 0){ _throw( Null_Exception);} _temp582->v;})) != Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp583= _temp564; if( _temp583
== 0){ _throw( Null_Exception);} _temp583->v;})): 0)? 1: _temp550 != _temp564){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp584=* v; struct
_tagged_string _temp585=* _temp556; xprintf("redeclaration of struct %.*s: bitfield mismatch on field %.*s",
_temp584.last_plus_one - _temp584.curr, _temp584.curr, _temp585.last_plus_one -
_temp585.curr, _temp585.curr);}));}} Cyc_Tcutil_check_type( loc, te, tvs,( void*)
Cyc_Absyn_MemKind, _temp552);{ void* subst_t2= Cyc_Tcutil_substitute( inst,
_temp566); if( ! Cyc_Tcutil_unify( _temp552, subst_t2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp586=* v; struct _tagged_string _temp587=* _temp556;
struct _tagged_string _temp588= Cyc_Absynpp_typ2string( _temp552); struct
_tagged_string _temp589= Cyc_Absynpp_typ2string( subst_t2); xprintf("redeclaration of struct %.*s: type mismatch on field %.*s: %.*s != %.*s",
_temp586.last_plus_one - _temp586.curr, _temp586.curr, _temp587.last_plus_one -
_temp587.curr, _temp587.curr, _temp588.last_plus_one - _temp588.curr, _temp588.curr,
_temp589.last_plus_one - _temp589.curr, _temp589.curr);}));}}}} if( f2s != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp591=* v; struct
_tagged_string _temp592=*(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp590= f2s; if( _temp590 == 0){ _throw( Null_Exception);} _temp590->hd;}))->name;
xprintf("redeclaration of struct %.*s is missing field %.*s", _temp591.last_plus_one
- _temp591.curr, _temp591.curr, _temp592.last_plus_one - _temp592.curr, _temp592.curr);}));}
if( f1s != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp594=* v;
struct _tagged_string _temp595=*(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp593= f1s; if( _temp593 == 0){ _throw( Null_Exception);} _temp593->hd;}))->name;
xprintf("redeclaration of struct %.*s has extra field %.*s", _temp594.last_plus_one
- _temp594.curr, _temp594.curr, _temp595.last_plus_one - _temp595.curr, _temp595.curr);}));}
return;}} _LL508:;} goto _LL483;}} _LL483:;}{ struct Cyc_Absyn_Structdecl** sdp=((
struct Cyc_Absyn_Structdecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->structdecls, v);* sdp=({ struct Cyc_Absyn_Structdecl*
_temp598=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp598->sc=( void*)(( void*) sd->sc); _temp598->name= sd->name; _temp598->tvs=
tvs; _temp598->fields= 0; _temp598->attributes= 0; _temp598;});{ struct Cyc_List_List*
prev_fields= 0;{ struct Cyc_List_List* fs=( struct Cyc_List_List*)({ struct Cyc_Core_Opt*
_temp667= sd->fields; if( _temp667 == 0){ _throw( Null_Exception);} _temp667->v;});
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp599= fs; if( _temp599 == 0){
_throw( Null_Exception);} _temp599->tl;})){ struct Cyc_Absyn_Structfield
_temp603; struct Cyc_List_List* _temp604; struct Cyc_Core_Opt* _temp606; void*
_temp608; struct Cyc_Absyn_Tqual _temp610; struct _tagged_string* _temp612;
struct Cyc_Absyn_Structfield* _temp601=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp600= fs; if( _temp600 == 0){ _throw( Null_Exception);}
_temp600->hd;}); _temp603=* _temp601; _LL613: _temp612=( struct _tagged_string*)
_temp603.name; goto _LL611; _LL611: _temp610=( struct Cyc_Absyn_Tqual) _temp603.tq;
goto _LL609; _LL609: _temp608=( void*) _temp603.type; goto _LL607; _LL607:
_temp606=( struct Cyc_Core_Opt*) _temp603.width; goto _LL605; _LL605: _temp604=(
struct Cyc_List_List*) _temp603.attributes; goto _LL602; _LL602: if((( int(*)(
int(* compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,
_temp612)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp614=* _temp612;
xprintf("duplicate field %.*s in struct", _temp614.last_plus_one - _temp614.curr,
_temp614.curr);}));} if( Cyc_String_strcmp(* _temp612,( struct _tagged_string)({
char* _temp615=( char*)""; struct _tagged_string _temp616; _temp616.curr=
_temp615; _temp616.base= _temp615; _temp616.last_plus_one= _temp615 + 1;
_temp616;})) != 0){ prev_fields=({ struct Cyc_List_List* _temp617=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp617->hd=( void*) _temp612;
_temp617->tl= prev_fields; _temp617;});} Cyc_Tcutil_check_type( loc, te, tvs,(
void*) Cyc_Absyn_MemKind, _temp608); if( _temp606 != 0){ unsigned int w= 0; if(
! Cyc_Tcutil_is_const_exp( te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt*
_temp618= _temp606; if( _temp618 == 0){ _throw( Null_Exception);} _temp618->v;}))){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp619=* _temp612; xprintf("bitfield %.*s does not have constant width",
_temp619.last_plus_one - _temp619.curr, _temp619.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp620= _temp606; if( _temp620
== 0){ _throw( Null_Exception);} _temp620->v;}));}{ void* _temp621= Cyc_Tcutil_compress(
_temp608); void* _temp627; void* _temp629; _LL623: if(( unsigned int) _temp621 >
4u?*(( int*) _temp621) == Cyc_Absyn_IntType: 0){ _LL630: _temp629=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp621)->f1; goto _LL628; _LL628: _temp627=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp621)->f2; goto _LL624;} else{
goto _LL625;} _LL625: goto _LL626; _LL624:{ void* _temp631= _temp627; _LL633:
if( _temp631 ==( void*) Cyc_Absyn_B1){ goto _LL634;} else{ goto _LL635;} _LL635:
if( _temp631 ==( void*) Cyc_Absyn_B2){ goto _LL636;} else{ goto _LL637;} _LL637:
if( _temp631 ==( void*) Cyc_Absyn_B4){ goto _LL638;} else{ goto _LL639;} _LL639:
if( _temp631 ==( void*) Cyc_Absyn_B8){ goto _LL640;} else{ goto _LL632;} _LL634:
if( w > 8){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp641=(
char*)"bitfield larger than type"; struct _tagged_string _temp642; _temp642.curr=
_temp641; _temp642.base= _temp641; _temp642.last_plus_one= _temp641 + 26;
_temp642;}));} goto _LL632; _LL636: if( w > 16){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp643=( char*)"bitfield larger than type"; struct
_tagged_string _temp644; _temp644.curr= _temp643; _temp644.base= _temp643;
_temp644.last_plus_one= _temp643 + 26; _temp644;}));} goto _LL632; _LL638: if( w
> 32){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp645=( char*)"bitfield larger than type";
struct _tagged_string _temp646; _temp646.curr= _temp645; _temp646.base= _temp645;
_temp646.last_plus_one= _temp645 + 26; _temp646;}));} goto _LL632; _LL640: if( w
> 64){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp647=( char*)"bitfield larger than type";
struct _tagged_string _temp648; _temp648.curr= _temp647; _temp648.base= _temp647;
_temp648.last_plus_one= _temp647 + 26; _temp648;}));} goto _LL632; _LL632:;}
goto _LL622; _LL626: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp649=*
_temp612; struct _tagged_string _temp650= Cyc_Absynpp_typ2string( _temp608);
xprintf("bitfield %.*s must have integral type but has type %.*s", _temp649.last_plus_one
- _temp649.curr, _temp649.curr, _temp650.last_plus_one - _temp650.curr, _temp650.curr);}));
goto _LL622; _LL622:;}} for( 0; _temp604 != 0; _temp604=({ struct Cyc_List_List*
_temp651= _temp604; if( _temp651 == 0){ _throw( Null_Exception);} _temp651->tl;})){
void* _temp653=( void*)({ struct Cyc_List_List* _temp652= _temp604; if( _temp652
== 0){ _throw( Null_Exception);} _temp652->hd;}); int _temp661; _LL655: if(
_temp653 ==( void*) Cyc_Absyn_Packed_att){ goto _LL656;} else{ goto _LL657;}
_LL657: if(( unsigned int) _temp653 > 15u?*(( int*) _temp653) == Cyc_Absyn_Aligned_att:
0){ _LL662: _temp661=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp653)->f1;
goto _LL658;} else{ goto _LL659;} _LL659: goto _LL660; _LL656: continue; _LL658:
continue; _LL660: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp664= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp663= _temp604; if( _temp663 == 0){ _throw(
Null_Exception);} _temp663->hd;})); struct _tagged_string _temp665=* v; struct
_tagged_string _temp666=* _temp612; xprintf("bad attribute %.*s in struct %.*s, member %.*s",
_temp664.last_plus_one - _temp664.curr, _temp664.curr, _temp665.last_plus_one -
_temp665.curr, _temp665.curr, _temp666.last_plus_one - _temp666.curr, _temp666.curr);}));
goto _LL654; _LL654:;}}} ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,
v,({ struct _tuple6* _temp668=( struct _tuple6*) GC_malloc( sizeof( struct
_tuple6)); _temp668->f1=( void*)({ struct Cyc_Tcenv_StructRes_struct* _temp669=(
struct Cyc_Tcenv_StructRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_StructRes_struct));
_temp669[ 0]=({ struct Cyc_Tcenv_StructRes_struct _temp670; _temp670.tag= Cyc_Tcenv_StructRes;
_temp670.f1= sd; _temp670;}); _temp669;}); _temp668->f2= 1; _temp668;}));* sdp=
sd;}}}}} static void Cyc_Tc_tcUniondecl( struct Cyc_Tcenv_Tenv* te, struct Cyc_Tcenv_Genv*
ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Uniondecl* ud){ if( ud->name
== 0){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp672=( char*)"anonymous unions are not allowed at top level";
struct _tagged_string _temp673; _temp673.curr= _temp672; _temp673.base= _temp672;
_temp673.last_plus_one= _temp672 + 46; _temp673;})); return;}{ struct
_tagged_string* v=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp910= ud->name;
if( _temp910 == 0){ _throw( Null_Exception);} _temp910->v;}))).f2;{ struct Cyc_List_List*
atts= ud->attributes; for( 0; atts != 0; atts=({ struct Cyc_List_List* _temp674=
atts; if( _temp674 == 0){ _throw( Null_Exception);} _temp674->tl;})){ void*
_temp676=( void*)({ struct Cyc_List_List* _temp675= atts; if( _temp675 == 0){
_throw( Null_Exception);} _temp675->hd;}); int _temp684; _LL678: if( _temp676 ==(
void*) Cyc_Absyn_Packed_att){ goto _LL679;} else{ goto _LL680;} _LL680: if((
unsigned int) _temp676 > 15u?*(( int*) _temp676) == Cyc_Absyn_Aligned_att: 0){
_LL685: _temp684=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp676)->f1;
goto _LL681;} else{ goto _LL682;} _LL682: goto _LL683; _LL679: continue; _LL681:
continue; _LL683: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp687= Cyc_Absyn_attribute2string((
void*)({ struct Cyc_List_List* _temp686= atts; if( _temp686 == 0){ _throw(
Null_Exception);} _temp686->hd;})); struct _tagged_string _temp688=* v; xprintf("bad attribute %.*s in union %.*s definition",
_temp687.last_plus_one - _temp687.curr, _temp687.curr, _temp688.last_plus_one -
_temp688.curr, _temp688.curr);})); goto _LL677; _LL677:;}}{ struct Cyc_List_List*
tvs2= ud->tvs; for( 0; tvs2 != 0; tvs2=({ struct Cyc_List_List* _temp689= tvs2;
if( _temp689 == 0){ _throw( Null_Exception);} _temp689->tl;})){ struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(((
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp705= tvs2; if( _temp705 ==
0){ _throw( Null_Exception);} _temp705->hd;}))->kind); void* _temp690=( void*) c->v;
void* _temp698; _LL692: if( _temp690 ==( void*) Cyc_Absyn_No_constr){ goto
_LL693;} else{ goto _LL694;} _LL694: if(( unsigned int) _temp690 > 1u?*(( int*)
_temp690) == Cyc_Absyn_Eq_constr: 0){ _LL699: _temp698=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp690)->f1; if( _temp698 ==( void*) Cyc_Absyn_MemKind){ goto _LL695;} else{
goto _LL696;}} else{ goto _LL696;} _LL696: goto _LL697; _LL693:( void*)( c->v=(
void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp700=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp700[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp701; _temp701.tag= Cyc_Absyn_Eq_constr; _temp701.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp701;}); _temp700;}))); continue; _LL695:
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp703=* v; struct
_tagged_string _temp704=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp702= tvs2; if( _temp702 == 0){ _throw( Null_Exception);} _temp702->hd;}))->name;
xprintf("union %.*s attempts to abstract type variable %.*s of kind M", _temp703.last_plus_one
- _temp703.curr, _temp703.curr, _temp704.last_plus_one - _temp704.curr, _temp704.curr);}));
continue; _LL697: continue; _LL691:;}}{ struct Cyc_List_List* tvs= ud->tvs;{
void* _temp707=(*(( struct _tuple0*)({ struct Cyc_Core_Opt* _temp706= ud->name;
if( _temp706 == 0){ _throw( Null_Exception);} _temp706->v;}))).f1; struct Cyc_List_List*
_temp715; struct Cyc_List_List* _temp717; _LL709: if(( unsigned int) _temp707 >
1u?*(( int*) _temp707) == Cyc_Absyn_Rel_n: 0){ _LL716: _temp715=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp707)->f1; if( _temp715 == 0){ goto _LL710;}
else{ goto _LL711;}} else{ goto _LL711;} _LL711: if(( unsigned int) _temp707 > 1u?*((
int*) _temp707) == Cyc_Absyn_Abs_n: 0){ _LL718: _temp717=( struct Cyc_List_List*)((
struct Cyc_Absyn_Abs_n_struct*) _temp707)->f1; if( _temp717 == 0){ goto _LL712;}
else{ goto _LL713;}} else{ goto _LL713;} _LL713: goto _LL714; _LL710: goto
_LL708; _LL712: goto _LL708; _LL714: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp719=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp720; _temp720.curr= _temp719; _temp720.base= _temp719;
_temp720.last_plus_one= _temp719 + 43; _temp720;})); return; _LL708:;}(*((
struct _tuple0*)({ struct Cyc_Core_Opt* _temp721= ud->name; if( _temp721 == 0){
_throw( Null_Exception);} _temp721->v;}))).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp722=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp722[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp723; _temp723.tag= Cyc_Absyn_Abs_n;
_temp723.f1= te->ns; _temp723;}); _temp722;}); Cyc_Tcutil_check_unique_tvars(
loc, tvs);{ struct Cyc_Core_Opt* _temp724=(( struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->uniondecls, v); struct
Cyc_Core_Opt _temp730; struct Cyc_Absyn_Uniondecl** _temp731; _LL726: if(
_temp724 == 0){ goto _LL727;} else{ goto _LL728;} _LL728: if( _temp724 == 0){
goto _LL725;} else{ _temp730=* _temp724; _LL732: _temp731=( struct Cyc_Absyn_Uniondecl**)
_temp730.v; goto _LL729;} _LL727: ge->uniondecls=(( struct Cyc_Dict_Dict*(*)(
struct Cyc_Dict_Dict* d, struct _tagged_string* key, struct Cyc_Absyn_Uniondecl**
data)) Cyc_Dict_insert)( ge->uniondecls, v,({ struct Cyc_Absyn_Uniondecl**
_temp733=( struct Cyc_Absyn_Uniondecl**) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl*));
_temp733[ 0]= ud; _temp733;})); if( ud->fields == 0){ return;} goto _LL725;
_LL729: { struct Cyc_Absyn_Uniondecl* ud2=* _temp731; struct Cyc_List_List* tvs2=
ud2->tvs; if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs) !=((
int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp734=* v; xprintf("redeclaration of union %.*s has a different number of type parameters",
_temp734.last_plus_one - _temp734.curr, _temp734.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1=({ struct Cyc_List_List*
_temp735= x1; if( _temp735 == 0){ _throw( Null_Exception);} _temp735->tl;}), x2=({
struct Cyc_List_List* _temp736= x2; if( _temp736 == 0){ _throw( Null_Exception);}
_temp736->tl;})){ struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp745= x1; if( _temp745 == 0){ _throw( Null_Exception);}
_temp745->hd;}))->kind); struct Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp744= x2; if( _temp744 == 0){ _throw( Null_Exception);}
_temp744->hd;}))->kind); if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr? c1 !=
c2: 0){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp737=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp737[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp738; _temp738.tag= Cyc_Absyn_Forward_constr; _temp738.f1= c2; _temp738;});
_temp737;})));} if((( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( c2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp740=* v; struct _tagged_string _temp741= Cyc_Absynpp_ckind2string(
c1); struct _tagged_string _temp742=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp739= x1; if( _temp739 == 0){ _throw( Null_Exception);} _temp739->hd;}))->name;
struct _tagged_string _temp743= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of union %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp740.last_plus_one - _temp740.curr, _temp740.curr, _temp741.last_plus_one -
_temp741.curr, _temp741.curr, _temp742.last_plus_one - _temp742.curr, _temp742.curr,
_temp743.last_plus_one - _temp743.curr, _temp743.curr);}));}}( void*)( ud->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) ud->sc, ud->fields,( void*)
ud2->sc, ud2->fields, loc,( struct _tagged_string)({ char* _temp746=( char*)"union";
struct _tagged_string _temp747; _temp747.curr= _temp746; _temp747.base= _temp746;
_temp747.last_plus_one= _temp746 + 6; _temp747;}), v));{ struct _tuple7 _temp749=({
struct _tuple7 _temp748; _temp748.f1= ud->fields; _temp748.f2= ud2->fields;
_temp748;}); struct Cyc_Core_Opt* _temp759; struct Cyc_Core_Opt* _temp761;
struct Cyc_Core_Opt* _temp763; struct Cyc_Core_Opt* _temp765; struct Cyc_Core_Opt*
_temp767; struct Cyc_Core_Opt* _temp769; struct Cyc_Core_Opt* _temp771; struct
Cyc_Core_Opt* _temp773; _LL751: _LL762: _temp761= _temp749.f1; if( _temp761 == 0){
goto _LL760;} else{ goto _LL753;} _LL760: _temp759= _temp749.f2; if( _temp759 ==
0){ goto _LL752;} else{ goto _LL753;} _LL753: _LL766: _temp765= _temp749.f1;
goto _LL764; _LL764: _temp763= _temp749.f2; if( _temp763 == 0){ goto _LL754;}
else{ goto _LL755;} _LL755: _LL770: _temp769= _temp749.f1; if( _temp769 == 0){
goto _LL768;} else{ goto _LL757;} _LL768: _temp767= _temp749.f2; goto _LL756;
_LL757: _LL774: _temp773= _temp749.f1; goto _LL772; _LL772: _temp771= _temp749.f2;
goto _LL758; _LL752: return; _LL754:* _temp731= ud; goto _LL750; _LL756: ud->fields=
ud2->fields; ud->tvs= tvs; return; _LL758: { struct Cyc_List_List* inst= 0;{
struct Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0=({ struct Cyc_List_List*
_temp775= tvs0; if( _temp775 == 0){ _throw( Null_Exception);} _temp775->tl;}),
tvs2=({ struct Cyc_List_List* _temp776= tvs2; if( _temp776 == 0){ _throw(
Null_Exception);} _temp776->tl;})){ inst=({ struct Cyc_List_List* _temp777=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp777->hd=(
void*)({ struct _tuple4* _temp778=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp778->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp782= tvs2; if( _temp782 == 0){ _throw( Null_Exception);} _temp782->hd;});
_temp778->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp779=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp779[ 0]=({ struct Cyc_Absyn_VarType_struct
_temp780; _temp780.tag= Cyc_Absyn_VarType; _temp780.f1=( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp781= tvs0; if( _temp781 == 0){ _throw( Null_Exception);}
_temp781->hd;}); _temp780;}); _temp779;}); _temp778;}); _temp777->tl= inst;
_temp777;});}} if( ! Cyc_Tcutil_same_atts( ud->attributes, ud2->attributes)){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp783=* v; xprintf("redeclaration of union %.*s has different attributes",
_temp783.last_plus_one - _temp783.curr, _temp783.curr);}));}{ struct Cyc_List_List*
f1s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp839= ud->fields; if(
_temp839 == 0){ _throw( Null_Exception);} _temp839->v;}); struct Cyc_List_List*
f2s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp838= ud2->fields; if(
_temp838 == 0){ _throw( Null_Exception);} _temp838->v;}); for( 0; f1s != 0? f2s
!= 0: 0; f1s=({ struct Cyc_List_List* _temp784= f1s; if( _temp784 == 0){ _throw(
Null_Exception);} _temp784->tl;}), f2s=({ struct Cyc_List_List* _temp785= f2s;
if( _temp785 == 0){ _throw( Null_Exception);} _temp785->tl;})){ struct Cyc_Absyn_Structfield
_temp789; struct Cyc_List_List* _temp790; struct Cyc_Core_Opt* _temp792; void*
_temp794; struct Cyc_Absyn_Tqual _temp796; struct _tagged_string* _temp798;
struct Cyc_Absyn_Structfield* _temp787=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp786= f1s; if( _temp786 == 0){ _throw( Null_Exception);}
_temp786->hd;}); _temp789=* _temp787; _LL799: _temp798=( struct _tagged_string*)
_temp789.name; goto _LL797; _LL797: _temp796=( struct Cyc_Absyn_Tqual) _temp789.tq;
goto _LL795; _LL795: _temp794=( void*) _temp789.type; goto _LL793; _LL793:
_temp792=( struct Cyc_Core_Opt*) _temp789.width; goto _LL791; _LL791: _temp790=(
struct Cyc_List_List*) _temp789.attributes; goto _LL788; _LL788: { struct Cyc_Absyn_Structfield
_temp803; struct Cyc_List_List* _temp804; struct Cyc_Core_Opt* _temp806; void*
_temp808; struct Cyc_Absyn_Tqual _temp810; struct _tagged_string* _temp812;
struct Cyc_Absyn_Structfield* _temp801=( struct Cyc_Absyn_Structfield*)({ struct
Cyc_List_List* _temp800= f2s; if( _temp800 == 0){ _throw( Null_Exception);}
_temp800->hd;}); _temp803=* _temp801; _LL813: _temp812=( struct _tagged_string*)
_temp803.name; goto _LL811; _LL811: _temp810=( struct Cyc_Absyn_Tqual) _temp803.tq;
goto _LL809; _LL809: _temp808=( void*) _temp803.type; goto _LL807; _LL807:
_temp806=( struct Cyc_Core_Opt*) _temp803.width; goto _LL805; _LL805: _temp804=(
struct Cyc_List_List*) _temp803.attributes; goto _LL802; _LL802: if( Cyc_String_zstrptrcmp(
_temp798, _temp812) != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string
_temp814=* v; struct _tagged_string _temp815=* _temp798; struct _tagged_string
_temp816=* _temp812; xprintf("redeclaration of union %.*s: field name mismatch %.*s != %.*s",
_temp814.last_plus_one - _temp814.curr, _temp814.curr, _temp815.last_plus_one -
_temp815.curr, _temp815.curr, _temp816.last_plus_one - _temp816.curr, _temp816.curr);}));}
if( ! Cyc_Tcutil_same_atts( _temp790, _temp804)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp817=* v; struct _tagged_string _temp818=* _temp798; xprintf("redeclaration of union %.*s: attribute mismatch on field %.*s",
_temp817.last_plus_one - _temp817.curr, _temp817.curr, _temp818.last_plus_one -
_temp818.curr, _temp818.curr);}));} if( ! Cyc_Tcutil_equal_tqual( _temp796,
_temp810)){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp819=* v; struct
_tagged_string _temp820=* _temp798; xprintf("redeclaration of union %.*s: qualifier mismatch on field %.*s",
_temp819.last_plus_one - _temp819.curr, _temp819.curr, _temp820.last_plus_one -
_temp820.curr, _temp820.curr);}));} if( _temp792 != 0? ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp821= _temp792; if(
_temp821 == 0){ _throw( Null_Exception);} _temp821->v;})): 0){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp822=* v; struct _tagged_string _temp823=*
_temp798; xprintf("redeclaration of struct %.*s: bad bitfield %.*s", _temp822.last_plus_one
- _temp822.curr, _temp822.curr, _temp823.last_plus_one - _temp823.curr, _temp823.curr);}));}
else{ if((( _temp792 != 0? _temp806 != 0: 0)? Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp824= _temp792; if( _temp824
== 0){ _throw( Null_Exception);} _temp824->v;})) != Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp825= _temp806; if( _temp825
== 0){ _throw( Null_Exception);} _temp825->v;})): 0)? 1: _temp792 != _temp806){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp826=* v; struct
_tagged_string _temp827=* _temp798; xprintf("redeclaration of struct %.*s: bitfield mismatch on field %.*s",
_temp826.last_plus_one - _temp826.curr, _temp826.curr, _temp827.last_plus_one -
_temp827.curr, _temp827.curr);}));}} Cyc_Tcutil_check_type( loc, te, tvs,( void*)
Cyc_Absyn_MemKind, _temp794);{ void* subst_t2= Cyc_Tcutil_substitute( inst,
_temp808); if( ! Cyc_Tcutil_unify( _temp794, subst_t2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp828=* v; struct _tagged_string _temp829=* _temp798;
struct _tagged_string _temp830= Cyc_Absynpp_typ2string( _temp794); struct
_tagged_string _temp831= Cyc_Absynpp_typ2string( subst_t2); xprintf("redeclaration of union %.*s: type mismatch on field %.*s: %.*s != %.*s",
_temp828.last_plus_one - _temp828.curr, _temp828.curr, _temp829.last_plus_one -
_temp829.curr, _temp829.curr, _temp830.last_plus_one - _temp830.curr, _temp830.curr,
_temp831.last_plus_one - _temp831.curr, _temp831.curr);}));}}}} if( f2s != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp833=* v; struct
_tagged_string _temp834=*(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp832= f2s; if( _temp832 == 0){ _throw( Null_Exception);} _temp832->hd;}))->name;
xprintf("redeclaration of union %.*s is missing field %.*s", _temp833.last_plus_one
- _temp833.curr, _temp833.curr, _temp834.last_plus_one - _temp834.curr, _temp834.curr);}));}
if( f1s != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp836=* v;
struct _tagged_string _temp837=*(( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List*
_temp835= f1s; if( _temp835 == 0){ _throw( Null_Exception);} _temp835->hd;}))->name;
xprintf("redeclaration of union %.*s has extra field %.*s", _temp836.last_plus_one
- _temp836.curr, _temp836.curr, _temp837.last_plus_one - _temp837.curr, _temp837.curr);}));}
return;}} _LL750:;} goto _LL725;}} _LL725:;}{ struct Cyc_Absyn_Uniondecl** udp=((
struct Cyc_Absyn_Uniondecl**(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->uniondecls, v);* udp=({ struct Cyc_Absyn_Uniondecl*
_temp840=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp840->sc=( void*)(( void*) ud->sc); _temp840->name= ud->name; _temp840->tvs=
tvs; _temp840->fields= 0; _temp840->attributes= 0; _temp840;});{ struct Cyc_List_List*
prev_fields= 0; struct Cyc_List_List* prev_types= 0;{ struct Cyc_List_List* fs=(
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp909= ud->fields; if( _temp909
== 0){ _throw( Null_Exception);} _temp909->v;}); for( 0; fs != 0; fs=({ struct
Cyc_List_List* _temp841= fs; if( _temp841 == 0){ _throw( Null_Exception);}
_temp841->tl;})){ struct Cyc_Absyn_Structfield _temp845; struct Cyc_List_List*
_temp846; struct Cyc_Core_Opt* _temp848; void* _temp850; struct Cyc_Absyn_Tqual
_temp852; struct _tagged_string* _temp854; struct Cyc_Absyn_Structfield*
_temp843=( struct Cyc_Absyn_Structfield*)({ struct Cyc_List_List* _temp842= fs;
if( _temp842 == 0){ _throw( Null_Exception);} _temp842->hd;}); _temp845=*
_temp843; _LL855: _temp854=( struct _tagged_string*) _temp845.name; goto _LL853;
_LL853: _temp852=( struct Cyc_Absyn_Tqual) _temp845.tq; goto _LL851; _LL851:
_temp850=( void*) _temp845.type; goto _LL849; _LL849: _temp848=( struct Cyc_Core_Opt*)
_temp845.width; goto _LL847; _LL847: _temp846=( struct Cyc_List_List*) _temp845.attributes;
goto _LL844; _LL844: if((( int(*)( int(* compare)( struct _tagged_string*,
struct _tagged_string*), struct Cyc_List_List* l, struct _tagged_string* x)) Cyc_List_mem)(
Cyc_String_zstrptrcmp, prev_fields, _temp854)){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp856=* _temp854; xprintf("duplicate field %.*s in union",
_temp856.last_plus_one - _temp856.curr, _temp856.curr);}));} prev_fields=({
struct Cyc_List_List* _temp857=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp857->hd=( void*) _temp854; _temp857->tl=
prev_fields; _temp857;}); Cyc_Tcutil_check_type( loc, te, tvs,( void*) Cyc_Absyn_MemKind,
_temp850); if( _temp848 != 0){ unsigned int w= 0; if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp858= _temp848; if(
_temp858 == 0){ _throw( Null_Exception);} _temp858->v;}))){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp859=* _temp854; xprintf("bitfield %.*s does not have constant width",
_temp859.last_plus_one - _temp859.curr, _temp859.curr);}));} else{ w= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Core_Opt* _temp860= _temp848; if( _temp860
== 0){ _throw( Null_Exception);} _temp860->v;}));}{ void* _temp861= Cyc_Tcutil_compress(
_temp850); void* _temp867; void* _temp869; _LL863: if(( unsigned int) _temp861 >
4u?*(( int*) _temp861) == Cyc_Absyn_IntType: 0){ _LL870: _temp869=( void*)((
struct Cyc_Absyn_IntType_struct*) _temp861)->f1; goto _LL868; _LL868: _temp867=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp861)->f2; goto _LL864;} else{
goto _LL865;} _LL865: goto _LL866; _LL864:{ void* _temp871= _temp867; _LL873:
if( _temp871 ==( void*) Cyc_Absyn_B1){ goto _LL874;} else{ goto _LL875;} _LL875:
if( _temp871 ==( void*) Cyc_Absyn_B2){ goto _LL876;} else{ goto _LL877;} _LL877:
if( _temp871 ==( void*) Cyc_Absyn_B4){ goto _LL878;} else{ goto _LL879;} _LL879:
if( _temp871 ==( void*) Cyc_Absyn_B8){ goto _LL880;} else{ goto _LL872;} _LL874:
if( w > 8){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp881=(
char*)"bitfield larger than type"; struct _tagged_string _temp882; _temp882.curr=
_temp881; _temp882.base= _temp881; _temp882.last_plus_one= _temp881 + 26;
_temp882;}));} goto _LL872; _LL876: if( w > 16){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp883=( char*)"bitfield larger than type"; struct
_tagged_string _temp884; _temp884.curr= _temp883; _temp884.base= _temp883;
_temp884.last_plus_one= _temp883 + 26; _temp884;}));} goto _LL872; _LL878: if( w
> 32){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp885=( char*)"bitfield larger than type";
struct _tagged_string _temp886; _temp886.curr= _temp885; _temp886.base= _temp885;
_temp886.last_plus_one= _temp885 + 26; _temp886;}));} goto _LL872; _LL880: if( w
> 64){ Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp887=( char*)"bitfield larger than type";
struct _tagged_string _temp888; _temp888.curr= _temp887; _temp888.base= _temp887;
_temp888.last_plus_one= _temp887 + 26; _temp888;}));} goto _LL872; _LL872:;}
goto _LL862; _LL866: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp889=*
_temp854; struct _tagged_string _temp890= Cyc_Absynpp_typ2string( _temp850);
xprintf("bitfield %.*s must have integral type but has type %.*s", _temp889.last_plus_one
- _temp889.curr, _temp889.curr, _temp890.last_plus_one - _temp890.curr, _temp890.curr);}));
goto _LL862; _LL862:;}} if( ! Cyc_Tcutil_bits_only( _temp850)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp891=* _temp854; struct _tagged_string _temp892=*
v; xprintf("%.*s has a type that is possibly incompatible with other members of union %.*s",
_temp891.last_plus_one - _temp891.curr, _temp891.curr, _temp892.last_plus_one -
_temp892.curr, _temp892.curr);}));} for( 0; _temp846 != 0; _temp846=({ struct
Cyc_List_List* _temp893= _temp846; if( _temp893 == 0){ _throw( Null_Exception);}
_temp893->tl;})){ void* _temp895=( void*)({ struct Cyc_List_List* _temp894=
_temp846; if( _temp894 == 0){ _throw( Null_Exception);} _temp894->hd;}); int
_temp903; _LL897: if( _temp895 ==( void*) Cyc_Absyn_Packed_att){ goto _LL898;}
else{ goto _LL899;} _LL899: if(( unsigned int) _temp895 > 15u?*(( int*) _temp895)
== Cyc_Absyn_Aligned_att: 0){ _LL904: _temp903=( int)(( struct Cyc_Absyn_Aligned_att_struct*)
_temp895)->f1; goto _LL900;} else{ goto _LL901;} _LL901: goto _LL902; _LL898:
continue; _LL900: continue; _LL902: Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp906= Cyc_Absyn_attribute2string(( void*)({ struct Cyc_List_List*
_temp905= _temp846; if( _temp905 == 0){ _throw( Null_Exception);} _temp905->hd;}));
struct _tagged_string _temp907=* v; struct _tagged_string _temp908=* _temp854;
xprintf("bad attribute %.*s in union %.*s, member %.*s", _temp906.last_plus_one
- _temp906.curr, _temp906.curr, _temp907.last_plus_one - _temp907.curr, _temp907.curr,
_temp908.last_plus_one - _temp908.curr, _temp908.curr);})); goto _LL896; _LL896:;}}}*
udp= ud;}}}}} static void Cyc_Tc_tcTuniondecl( struct Cyc_Tcenv_Tenv* te, struct
Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Tuniondecl*
tud){ struct _tagged_string* v=(* tud->name).f2; struct Cyc_List_List* tvs= tud->tvs;{
struct Cyc_List_List* tvs2= tvs; for( 0; tvs2 != 0; tvs2=({ struct Cyc_List_List*
_temp911= tvs2; if( _temp911 == 0){ _throw( Null_Exception);} _temp911->tl;})){
struct Cyc_Absyn_Conref* c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp927= tvs2; if( _temp927 == 0){ _throw( Null_Exception);} _temp927->hd;}))->kind);
void* _temp912=( void*) c->v; void* _temp920; _LL914: if( _temp912 ==( void*)
Cyc_Absyn_No_constr){ goto _LL915;} else{ goto _LL916;} _LL916: if((
unsigned int) _temp912 > 1u?*(( int*) _temp912) == Cyc_Absyn_Eq_constr: 0){
_LL921: _temp920=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp912)->f1;
if( _temp920 ==( void*) Cyc_Absyn_MemKind){ goto _LL917;} else{ goto _LL918;}}
else{ goto _LL918;} _LL918: goto _LL919; _LL915:( void*)( c->v=( void*)(( void*)({
struct Cyc_Absyn_Eq_constr_struct* _temp922=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp922[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp923; _temp923.tag= Cyc_Absyn_Eq_constr; _temp923.f1=(
void*)(( void*) Cyc_Absyn_BoxKind); _temp923;}); _temp922;}))); goto _LL913;
_LL917: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp925=* v; struct
_tagged_string _temp926=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp924= tvs2; if( _temp924 == 0){ _throw( Null_Exception);} _temp924->hd;}))->name;
xprintf("[x]tunion %.*s attempts to abstract type variable %.*s of kind M",
_temp925.last_plus_one - _temp925.curr, _temp925.curr, _temp926.last_plus_one -
_temp926.curr, _temp926.curr);})); goto _LL913; _LL919: goto _LL913; _LL913:;}}{
struct Cyc_Core_Opt* tud_opt; if( tud->is_xtunion){{ struct _handler_cons
_temp928; _push_handler(& _temp928);{ void* _temp929=( void*) setjmp( _temp928.handler);
if( ! _temp929){ tud_opt= Cyc_Tcenv_lookup_xtuniondecl( te, loc, tud->name);;
_pop_handler();} else{ void* _temp931= _temp929; _LL933: if( _temp931 == Cyc_Dict_Absent){
goto _LL934;} else{ goto _LL935;} _LL935: goto _LL936; _LL934: Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp937= Cyc_Absynpp_qvar2string( tud->name);
xprintf("qualified xtunion declaration %.*s is not an existing xtunion",
_temp937.last_plus_one - _temp937.curr, _temp937.curr);})); return; _LL936:(
void) _throw( _temp931); _LL932:;}}} if( tud_opt != 0){ tud->name=(*(( struct
Cyc_Absyn_Tuniondecl**)({ struct Cyc_Core_Opt* _temp938= tud_opt; if( _temp938
== 0){ _throw( Null_Exception);} _temp938->v;})))->name;}} else{{ void* _temp939=(*
tud->name).f1; struct Cyc_List_List* _temp947; struct Cyc_List_List* _temp949;
_LL941: if(( unsigned int) _temp939 > 1u?*(( int*) _temp939) == Cyc_Absyn_Rel_n:
0){ _LL948: _temp947=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp939)->f1; if( _temp947 == 0){ goto _LL942;} else{ goto _LL943;}} else{ goto
_LL943;} _LL943: if(( unsigned int) _temp939 > 1u?*(( int*) _temp939) == Cyc_Absyn_Abs_n:
0){ _LL950: _temp949=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp939)->f1; goto _LL944;} else{ goto _LL945;} _LL945: goto _LL946; _LL942:(*
tud->name).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp951=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp951[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp952; _temp952.tag= Cyc_Absyn_Abs_n; _temp952.f1= te->ns; _temp952;});
_temp951;}); goto _LL940; _LL944: goto _LL946; _LL946: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp953=( char*)"qualified declarations are not implemented";
struct _tagged_string _temp954; _temp954.curr= _temp953; _temp954.base= _temp953;
_temp954.last_plus_one= _temp953 + 43; _temp954;})); return; _LL940:;} tud_opt=((
struct Cyc_Core_Opt*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key))
Cyc_Dict_lookup_opt)( ge->tuniondecls, v);} Cyc_Tcutil_check_unique_tvars( loc,
tvs);{ struct Cyc_List_List* prev_fields;{ struct Cyc_Core_Opt* _temp955=
tud_opt; struct Cyc_Core_Opt _temp961; struct Cyc_Absyn_Tuniondecl** _temp962;
_LL957: if( _temp955 == 0){ goto _LL958;} else{ goto _LL959;} _LL959: if(
_temp955 == 0){ goto _LL956;} else{ _temp961=* _temp955; _LL963: _temp962=(
struct Cyc_Absyn_Tuniondecl**) _temp961.v; goto _LL960;} _LL958: ge->tuniondecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Tuniondecl** data)) Cyc_Dict_insert)( ge->tuniondecls, v,({
struct Cyc_Absyn_Tuniondecl** _temp964=( struct Cyc_Absyn_Tuniondecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl*)); _temp964[ 0]= tud; _temp964;}));
if( tud->fields == 0){ return;} prev_fields= 0; goto _LL956; _LL960: { struct
Cyc_Absyn_Tuniondecl* tud2=* _temp962; struct Cyc_List_List* tvs2= tud2->tvs;
if((( int(*)( struct Cyc_List_List* x)) Cyc_List_length)( tvs) !=(( int(*)(
struct Cyc_List_List* x)) Cyc_List_length)( tvs2)){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp965=* v; xprintf("redeclaration of [x]tunion %.*s has a different number of type parameters",
_temp965.last_plus_one - _temp965.curr, _temp965.curr);}));}{ struct Cyc_List_List*
x1= tvs; struct Cyc_List_List* x2= tvs2; for( 0; x1 != 0; x1=({ struct Cyc_List_List*
_temp966= x1; if( _temp966 == 0){ _throw( Null_Exception);} _temp966->tl;}), x2=({
struct Cyc_List_List* _temp967= x2; if( _temp967 == 0){ _throw( Null_Exception);}
_temp967->tl;})){ struct Cyc_Absyn_Conref* c1=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp976= x1; if( _temp976 == 0){ _throw( Null_Exception);}
_temp976->hd;}))->kind); struct Cyc_Absyn_Conref* c2=(( struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)((( struct Cyc_Absyn_Tvar*)({
struct Cyc_List_List* _temp975= x2; if( _temp975 == 0){ _throw( Null_Exception);}
_temp975->hd;}))->kind); if(( void*) c1->v ==( void*) Cyc_Absyn_No_constr? c1 !=
c2: 0){( void*)( c1->v=( void*)(( void*)({ struct Cyc_Absyn_Forward_constr_struct*
_temp968=( struct Cyc_Absyn_Forward_constr_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_Forward_constr_struct)); _temp968[ 0]=({ struct Cyc_Absyn_Forward_constr_struct
_temp969; _temp969.tag= Cyc_Absyn_Forward_constr; _temp969.f1= c2; _temp969;});
_temp968;})));} if((( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
c1) !=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)( c2)){ Cyc_Tcutil_terr(
loc,({ struct _tagged_string _temp971=* v; struct _tagged_string _temp972= Cyc_Absynpp_ckind2string(
c1); struct _tagged_string _temp973=*(( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp970= x1; if( _temp970 == 0){ _throw( Null_Exception);} _temp970->hd;}))->name;
struct _tagged_string _temp974= Cyc_Absynpp_ckind2string( c2); xprintf("redeclaration of [x]tunion %.*s has a different kind (%.*s) for type parameter %.*s (%.*s)",
_temp971.last_plus_one - _temp971.curr, _temp971.curr, _temp972.last_plus_one -
_temp972.curr, _temp972.curr, _temp973.last_plus_one - _temp973.curr, _temp973.curr,
_temp974.last_plus_one - _temp974.curr, _temp974.curr);}));}}( void*)( tud->sc=(
void*)(( void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) tud->sc, tud->fields,(
void*) tud2->sc, tud2->fields, loc,( struct _tagged_string)({ char* _temp977=(
char*)"[x]tunion"; struct _tagged_string _temp978; _temp978.curr= _temp977;
_temp978.base= _temp977; _temp978.last_plus_one= _temp977 + 10; _temp978;}), v));
if( tud->is_xtunion != tud2->is_xtunion){ Cyc_Tcutil_terr( loc, xprintf("redeclaration of tunion as xtunion or vice-versa"));}{
struct _tuple7 _temp980=({ struct _tuple7 _temp979; _temp979.f1= tud->fields;
_temp979.f2= tud2->fields; _temp979;}); struct Cyc_Core_Opt* _temp990; struct
Cyc_Core_Opt* _temp992; struct Cyc_Core_Opt* _temp994; struct Cyc_Core_Opt*
_temp996; struct Cyc_Core_Opt* _temp998; struct Cyc_Core_Opt* _temp1000; struct
Cyc_Core_Opt* _temp1002; struct Cyc_Core_Opt* _temp1004; _LL982: _LL993:
_temp992= _temp980.f1; if( _temp992 == 0){ goto _LL991;} else{ goto _LL984;}
_LL991: _temp990= _temp980.f2; if( _temp990 == 0){ goto _LL983;} else{ goto
_LL984;} _LL984: _LL997: _temp996= _temp980.f1; if( _temp996 == 0){ goto _LL995;}
else{ goto _LL986;} _LL995: _temp994= _temp980.f2; goto _LL985; _LL986: _LL1001:
_temp1000= _temp980.f1; goto _LL999; _LL999: _temp998= _temp980.f2; if( _temp998
== 0){ goto _LL987;} else{ goto _LL988;} _LL988: _LL1005: _temp1004= _temp980.f1;
goto _LL1003; _LL1003: _temp1002= _temp980.f2; goto _LL989; _LL983: return;
_LL985: tud->fields= tud2->fields; tud->tvs= tvs2; return; _LL987: prev_fields=
0;* _temp962= tud; goto _LL981; _LL989: { struct Cyc_List_List* inst= 0;{ struct
Cyc_List_List* tvs0= tvs; for( 0; tvs0 != 0; tvs0=({ struct Cyc_List_List*
_temp1006= tvs0; if( _temp1006 == 0){ _throw( Null_Exception);} _temp1006->tl;}),
tvs2=({ struct Cyc_List_List* _temp1007= tvs2; if( _temp1007 == 0){ _throw(
Null_Exception);} _temp1007->tl;})){ inst=({ struct Cyc_List_List* _temp1008=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1008->hd=(
void*)({ struct _tuple4* _temp1009=( struct _tuple4*) GC_malloc( sizeof( struct
_tuple4)); _temp1009->f1=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List*
_temp1013= tvs2; if( _temp1013 == 0){ _throw( Null_Exception);} _temp1013->hd;});
_temp1009->f2=( void*)({ struct Cyc_Absyn_VarType_struct* _temp1010=( struct Cyc_Absyn_VarType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_VarType_struct)); _temp1010[ 0]=({ struct
Cyc_Absyn_VarType_struct _temp1011; _temp1011.tag= Cyc_Absyn_VarType; _temp1011.f1=(
struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp1012= tvs0; if( _temp1012
== 0){ _throw( Null_Exception);} _temp1012->hd;}); _temp1011;}); _temp1010;});
_temp1009;}); _temp1008->tl= inst; _temp1008;});}}{ struct Cyc_List_List* f1s=(
struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1053= tud->fields; if(
_temp1053 == 0){ _throw( Null_Exception);} _temp1053->v;}); struct Cyc_List_List*
f2s=( struct Cyc_List_List*)({ struct Cyc_Core_Opt* _temp1052= tud2->fields; if(
_temp1052 == 0){ _throw( Null_Exception);} _temp1052->v;});{ struct Cyc_List_List*
fs= f1s; for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp1014= fs; if(
_temp1014 == 0){ _throw( Null_Exception);} _temp1014->tl;})){ struct Cyc_Absyn_Tunionfield*
f=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1041= fs; if(
_temp1041 == 0){ _throw( Null_Exception);} _temp1041->hd;}); struct Cyc_List_List*
alltvs=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( tvs, f->tvs); Cyc_Tcutil_check_unique_tvars( f->loc,
alltvs);{ struct Cyc_List_List* typs= f->typs; for( 0; typs != 0; typs=({ struct
Cyc_List_List* _temp1015= typs; if( _temp1015 == 0){ _throw( Null_Exception);}
_temp1015->tl;})){ Cyc_Tcutil_check_type( f->loc, te, alltvs,( void*) Cyc_Absyn_MemKind,(*((
struct _tuple5*)({ struct Cyc_List_List* _temp1016= typs; if( _temp1016 == 0){
_throw( Null_Exception);} _temp1016->hd;}))).f2);}}{ void* _temp1017=(* f->name).f1;
struct Cyc_List_List* _temp1027; struct Cyc_List_List* _temp1029; struct Cyc_List_List*
_temp1031; _LL1019: if(( unsigned int) _temp1017 > 1u?*(( int*) _temp1017) ==
Cyc_Absyn_Rel_n: 0){ _LL1028: _temp1027=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp1017)->f1; if( _temp1027 == 0){ goto _LL1020;} else{ goto _LL1021;}} else{
goto _LL1021;} _LL1021: if(( unsigned int) _temp1017 > 1u?*(( int*) _temp1017)
== Cyc_Absyn_Rel_n: 0){ _LL1030: _temp1029=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp1017)->f1; goto _LL1022;} else{ goto _LL1023;} _LL1023: if(( unsigned int)
_temp1017 > 1u?*(( int*) _temp1017) == Cyc_Absyn_Abs_n: 0){ _LL1032: _temp1031=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp1017)->f1; goto
_LL1024;} else{ goto _LL1025;} _LL1025: if( _temp1017 ==( void*) Cyc_Absyn_Loc_n){
goto _LL1026;} else{ goto _LL1018;} _LL1020: if( tud->is_xtunion){(* f->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp1033=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1033[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1034; _temp1034.tag= Cyc_Absyn_Abs_n; _temp1034.f1= te->ns; _temp1034;});
_temp1033;});} else{(* f->name).f1=(* tud->name).f1;} goto _LL1018; _LL1022: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp1035=( char*)"qualified declarations are not allowed";
struct _tagged_string _temp1036; _temp1036.curr= _temp1035; _temp1036.base=
_temp1035; _temp1036.last_plus_one= _temp1035 + 39; _temp1036;})); goto _LL1018;
_LL1024: goto _LL1018; _LL1026:( void) _throw(( void*)({ struct Cyc_Core_Impossible_struct*
_temp1037=( struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp1037[ 0]=({ struct Cyc_Core_Impossible_struct _temp1038; _temp1038.tag= Cyc_Core_Impossible;
_temp1038.f1=( struct _tagged_string)({ char* _temp1039=( char*)"tcTuniondecl: Loc_n";
struct _tagged_string _temp1040; _temp1040.curr= _temp1039; _temp1040.base=
_temp1039; _temp1040.last_plus_one= _temp1039 + 20; _temp1040;}); _temp1038;});
_temp1037;})); goto _LL1018; _LL1018:;}}} if( ! tud->is_xtunion){ for( 0; f1s !=
0? f2s != 0: 0; f1s=({ struct Cyc_List_List* _temp1042= f1s; if( _temp1042 == 0){
_throw( Null_Exception);} _temp1042->tl;}), f2s=({ struct Cyc_List_List*
_temp1043= f2s; if( _temp1043 == 0){ _throw( Null_Exception);} _temp1043->tl;})){
Cyc_Tc_field_redecl_okay(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp1044= f1s; if( _temp1044 == 0){ _throw( Null_Exception);} _temp1044->hd;}),(
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1045= f2s; if(
_temp1045 == 0){ _throw( Null_Exception);} _temp1045->hd;}), inst, te, v);} if(
f1s != 0){ Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp1047=* v; struct
_tagged_string _temp1048=*(*(( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp1046= f1s; if( _temp1046 == 0){ _throw( Null_Exception);} _temp1046->hd;}))->name).f2;
xprintf("redeclaration of tunion %.*s has extra field %.*s", _temp1047.last_plus_one
- _temp1047.curr, _temp1047.curr, _temp1048.last_plus_one - _temp1048.curr,
_temp1048.curr);}));} if( f2s != 0){ Cyc_Tcutil_terr( loc,({ struct
_tagged_string _temp1050=* v; struct _tagged_string _temp1051=*(*(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1049= f2s; if( _temp1049 == 0){ _throw(
Null_Exception);} _temp1049->hd;}))->name).f2; xprintf("redeclaration of tunion %.*s is missing field %.*s",
_temp1050.last_plus_one - _temp1050.curr, _temp1050.curr, _temp1051.last_plus_one
- _temp1051.curr, _temp1051.curr);}));} return;} prev_fields= f2s; goto _LL981;}}
_LL981:;} goto _LL956;}} _LL956:;}{ struct Cyc_List_List* fs=( struct Cyc_List_List*)({
struct Cyc_Core_Opt* _temp1089= tud->fields; if( _temp1089 == 0){ _throw(
Null_Exception);} _temp1089->v;}); for( 0; fs != 0; fs=({ struct Cyc_List_List*
_temp1054= fs; if( _temp1054 == 0){ _throw( Null_Exception);} _temp1054->tl;})){
struct Cyc_Absyn_Tunionfield* f=( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp1088= fs; if( _temp1088 == 0){ _throw( Null_Exception);} _temp1088->hd;});
if( tud->is_xtunion){ struct Cyc_List_List* prevs= prev_fields; for( 0; prevs !=
0; prevs=({ struct Cyc_List_List* _temp1055= prevs; if( _temp1055 == 0){ _throw(
Null_Exception);} _temp1055->tl;})){ if( Cyc_Absyn_qvar_cmp((( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1056= prevs; if( _temp1056 == 0){ _throw(
Null_Exception);} _temp1056->hd;}))->name,(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1057= fs; if( _temp1057 == 0){ _throw( Null_Exception);}
_temp1057->hd;}))->name) == 0){ Cyc_Tc_field_redecl_okay(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1058= prevs; if( _temp1058 == 0){ _throw(
Null_Exception);} _temp1058->hd;}),( struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List*
_temp1059= fs; if( _temp1059 == 0){ _throw( Null_Exception);} _temp1059->hd;}),
0, te, v); break;}} if( prevs != 0){ continue;}} else{ struct Cyc_List_List*
prevs= prev_fields; for( 0; prevs != 0; prevs=({ struct Cyc_List_List* _temp1060=
prevs; if( _temp1060 == 0){ _throw( Null_Exception);} _temp1060->tl;})){ if( Cyc_Absyn_qvar_cmp(((
struct Cyc_Absyn_Tunionfield*)({ struct Cyc_List_List* _temp1061= prevs; if(
_temp1061 == 0){ _throw( Null_Exception);} _temp1061->hd;}))->name,(( struct Cyc_Absyn_Tunionfield*)({
struct Cyc_List_List* _temp1062= fs; if( _temp1062 == 0){ _throw( Null_Exception);}
_temp1062->hd;}))->name) == 0){ Cyc_Tcutil_terr( f->loc,({ struct _tagged_string
_temp1063=*(* f->name).f2; xprintf("duplicate field name %.*s", _temp1063.last_plus_one
- _temp1063.curr, _temp1063.curr);}));}}} prev_fields=({ struct Cyc_List_List*
_temp1064=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1064->hd=( void*) f; _temp1064->tl= prev_fields; _temp1064;});{ struct Cyc_List_List*
tvs= f->tvs; for( 0; tvs != 0; tvs=({ struct Cyc_List_List* _temp1065= tvs; if(
_temp1065 == 0){ _throw( Null_Exception);} _temp1065->tl;})){ struct Cyc_Absyn_Tvar*
tv=( struct Cyc_Absyn_Tvar*)({ struct Cyc_List_List* _temp1079= tvs; if(
_temp1079 == 0){ _throw( Null_Exception);} _temp1079->hd;}); struct Cyc_Absyn_Conref*
c=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
tv->kind); void* _temp1066=( void*) c->v; void* _temp1074; _LL1068: if(
_temp1066 ==( void*) Cyc_Absyn_No_constr){ goto _LL1069;} else{ goto _LL1070;}
_LL1070: if(( unsigned int) _temp1066 > 1u?*(( int*) _temp1066) == Cyc_Absyn_Eq_constr:
0){ _LL1075: _temp1074=( void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp1066)->f1;
if( _temp1074 ==( void*) Cyc_Absyn_MemKind){ goto _LL1071;} else{ goto _LL1072;}}
else{ goto _LL1072;} _LL1072: goto _LL1073; _LL1069:( void*)( c->v=( void*)((
void*)({ struct Cyc_Absyn_Eq_constr_struct* _temp1076=( struct Cyc_Absyn_Eq_constr_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct)); _temp1076[ 0]=({ struct
Cyc_Absyn_Eq_constr_struct _temp1077; _temp1077.tag= Cyc_Absyn_Eq_constr;
_temp1077.f1=( void*)(( void*) Cyc_Absyn_BoxKind); _temp1077;}); _temp1076;})));
goto _LL1067; _LL1071: Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp1078=*(*
f->name).f2; xprintf("field %.*s abstracts type variable of kind M", _temp1078.last_plus_one
- _temp1078.curr, _temp1078.curr);})); goto _LL1067; _LL1073: goto _LL1067;
_LL1067:;}}{ struct Cyc_List_List* alltvs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( tvs, f->tvs); Cyc_Tcutil_check_unique_tvars(
loc, alltvs);{ struct Cyc_List_List* typs= f->typs; for( 0; typs != 0; typs=({
struct Cyc_List_List* _temp1080= typs; if( _temp1080 == 0){ _throw(
Null_Exception);} _temp1080->tl;})){ void* t=(*(( struct _tuple5*)({ struct Cyc_List_List*
_temp1081= typs; if( _temp1081 == 0){ _throw( Null_Exception);} _temp1081->hd;}))).f2;
Cyc_Tcutil_check_type( f->loc, te, alltvs,( void*) Cyc_Absyn_MemKind, t);}}(* f->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp1082=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1082[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1083; _temp1083.tag= Cyc_Absyn_Abs_n; _temp1083.f1= te->ns; _temp1083;});
_temp1082;});{ struct Cyc_Absyn_Tuniondecl* tudres= tud; if( tud_opt != 0){
tudres=*(( struct Cyc_Absyn_Tuniondecl**)({ struct Cyc_Core_Opt* _temp1084=
tud_opt; if( _temp1084 == 0){ _throw( Null_Exception);} _temp1084->v;}));} ge->ordinaries=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,(* f->name).f2,({ struct
_tuple6* _temp1085=( struct _tuple6*) GC_malloc( sizeof( struct _tuple6));
_temp1085->f1=( void*)({ struct Cyc_Tcenv_TunionRes_struct* _temp1086=( struct
Cyc_Tcenv_TunionRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_TunionRes_struct));
_temp1086[ 0]=({ struct Cyc_Tcenv_TunionRes_struct _temp1087; _temp1087.tag= Cyc_Tcenv_TunionRes;
_temp1087.f1= tudres; _temp1087.f2= f; _temp1087;}); _temp1086;}); _temp1085->f2=
1; _temp1085;}));}}}}}}} static void Cyc_Tc_tcEnumdecl( struct Cyc_Tcenv_Tenv*
te, struct Cyc_Tcenv_Genv* ge, struct Cyc_Position_Segment* loc, struct Cyc_Absyn_Enumdecl*
ed){ struct _tagged_string* v=(* ed->name).f2;{ void* _temp1090=(* ed->name).f1;
struct Cyc_List_List* _temp1098; struct Cyc_List_List* _temp1100; _LL1092: if((
unsigned int) _temp1090 > 1u?*(( int*) _temp1090) == Cyc_Absyn_Rel_n: 0){
_LL1099: _temp1098=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp1090)->f1; if( _temp1098 == 0){ goto _LL1093;} else{ goto _LL1094;}} else{
goto _LL1094;} _LL1094: if(( unsigned int) _temp1090 > 1u?*(( int*) _temp1090)
== Cyc_Absyn_Abs_n: 0){ _LL1101: _temp1100=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp1090)->f1; if( _temp1100 == 0){ goto _LL1095;} else{ goto _LL1096;}} else{
goto _LL1096;} _LL1096: goto _LL1097; _LL1093: goto _LL1091; _LL1095: goto
_LL1091; _LL1097: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp1102=( char*)"qualified declarations are not implemented"; struct
_tagged_string _temp1103; _temp1103.curr= _temp1102; _temp1103.base= _temp1102;
_temp1103.last_plus_one= _temp1102 + 43; _temp1103;})); return; _LL1091:;}(* ed->name).f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp1104=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1104[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1105; _temp1105.tag= Cyc_Absyn_Abs_n; _temp1105.f1= te->ns; _temp1105;});
_temp1104;});{ struct Cyc_Core_Opt* _temp1106=(( struct Cyc_Core_Opt*(*)( struct
Cyc_Dict_Dict* d, struct _tagged_string* key)) Cyc_Dict_lookup_opt)( ge->enumdecls,
v); struct Cyc_Core_Opt _temp1112; struct Cyc_Absyn_Enumdecl** _temp1113;
_LL1108: if( _temp1106 == 0){ goto _LL1109;} else{ goto _LL1110;} _LL1110: if(
_temp1106 == 0){ goto _LL1107;} else{ _temp1112=* _temp1106; _LL1114: _temp1113=(
struct Cyc_Absyn_Enumdecl**) _temp1112.v; goto _LL1111;} _LL1109: ge->enumdecls=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string* key,
struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)( ge->enumdecls, v,({ struct
Cyc_Absyn_Enumdecl** _temp1115=( struct Cyc_Absyn_Enumdecl**) GC_malloc( sizeof(
struct Cyc_Absyn_Enumdecl*)); _temp1115[ 0]= ed; _temp1115;})); goto _LL1107;
_LL1111: { struct Cyc_Absyn_Enumdecl* ed2=* _temp1113;( void*)( ed->sc=( void*)((
void*(*)( void* s1, struct Cyc_Core_Opt* fields1, void* s2, struct Cyc_Core_Opt*
fields2, struct Cyc_Position_Segment* loc, struct _tagged_string t, struct
_tagged_string* v)) Cyc_Tc_scope_redecl_okay)(( void*) ed->sc, 0,( void*) ed2->sc,
0, loc,( struct _tagged_string)({ char* _temp1116=( char*)"enum"; struct
_tagged_string _temp1117; _temp1117.curr= _temp1116; _temp1117.base= _temp1116;
_temp1117.last_plus_one= _temp1116 + 5; _temp1117;}), v));{ struct Cyc_List_List*
f1s= ed->fields; struct Cyc_List_List* f2s= ed2->fields; unsigned int tag_count=
0; for( 0; f1s != 0? f2s != 0: 0; f1s=({ struct Cyc_List_List* _temp1118= f1s;
if( _temp1118 == 0){ _throw( Null_Exception);} _temp1118->tl;}), f2s=({ struct
Cyc_List_List* _temp1119= f2s; if( _temp1119 == 0){ _throw( Null_Exception);}
_temp1119->tl;})){ struct Cyc_Absyn_Enumfield _temp1123; struct Cyc_Position_Segment*
_temp1124; struct Cyc_Absyn_Exp* _temp1126; struct Cyc_Absyn_Exp** _temp1128;
struct _tuple0* _temp1129; struct Cyc_Absyn_Enumfield* _temp1121=( struct Cyc_Absyn_Enumfield*)({
struct Cyc_List_List* _temp1120= f1s; if( _temp1120 == 0){ _throw(
Null_Exception);} _temp1120->hd;}); _temp1123=* _temp1121; _LL1130: _temp1129=(
struct _tuple0*) _temp1123.name; goto _LL1127; _LL1127: _temp1126=( struct Cyc_Absyn_Exp*)
_temp1123.tag; _temp1128=&(* _temp1121).tag; goto _LL1125; _LL1125: _temp1124=(
struct Cyc_Position_Segment*) _temp1123.loc; goto _LL1122; _LL1122: { struct Cyc_Absyn_Enumfield
_temp1134; struct Cyc_Position_Segment* _temp1135; struct Cyc_Absyn_Exp*
_temp1137; struct _tuple0* _temp1139; struct Cyc_Absyn_Enumfield* _temp1132=(
struct Cyc_Absyn_Enumfield*)({ struct Cyc_List_List* _temp1131= f2s; if(
_temp1131 == 0){ _throw( Null_Exception);} _temp1131->hd;}); _temp1134=*
_temp1132; _LL1140: _temp1139=( struct _tuple0*) _temp1134.name; goto _LL1138;
_LL1138: _temp1137=( struct Cyc_Absyn_Exp*) _temp1134.tag; goto _LL1136; _LL1136:
_temp1135=( struct Cyc_Position_Segment*) _temp1134.loc; goto _LL1133; _LL1133:(*
_temp1129).f1=( void*)({ struct Cyc_Absyn_Abs_n_struct* _temp1141=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1141[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1142; _temp1142.tag= Cyc_Absyn_Abs_n; _temp1142.f1= te->ns; _temp1142;});
_temp1141;}); if( Cyc_String_zstrptrcmp((* _temp1129).f2,(* _temp1139).f2) != 0){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp1143=* v; struct
_tagged_string _temp1144=*(* _temp1129).f2; struct _tagged_string _temp1145=*(*
_temp1139).f2; xprintf("redeclaration of enum %.*s: field name mismatch %.*s != %.*s",
_temp1143.last_plus_one - _temp1143.curr, _temp1143.curr, _temp1144.last_plus_one
- _temp1144.curr, _temp1144.curr, _temp1145.last_plus_one - _temp1145.curr,
_temp1145.curr);}));} if(* _temp1128 == 0){* _temp1128=( struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp( tag_count, _temp1124);} else{ if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1146=* _temp1128; if(
_temp1146 == 0){ _throw( Null_Exception);} _temp1146;}))){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp1147=* v; struct _tagged_string _temp1148=*(*
_temp1129).f2; xprintf("redeclaration of enum %.*s, field %.*s: expression is not constant",
_temp1147.last_plus_one - _temp1147.curr, _temp1147.curr, _temp1148.last_plus_one
- _temp1148.curr, _temp1148.curr);}));}}{ unsigned int t1= Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1152=* _temp1128; if(
_temp1152 == 0){ _throw( Null_Exception);} _temp1152;})); tag_count= t1 + 1; if(
t1 != Cyc_Evexp_eval_const_uint_exp(( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp*
_temp1149= _temp1137; if( _temp1149 == 0){ _throw( Null_Exception);} _temp1149;}))){
Cyc_Tcutil_terr( loc,({ struct _tagged_string _temp1150=* v; struct
_tagged_string _temp1151=*(* _temp1129).f2; xprintf("redeclaration of enum %.*s, field %.*s, value mismatch",
_temp1150.last_plus_one - _temp1150.curr, _temp1150.curr, _temp1151.last_plus_one
- _temp1151.curr, _temp1151.curr);}));}}}} return;}} _LL1107:;}{ struct Cyc_List_List*
prev_fields= 0; unsigned int tag_count= 0;{ struct Cyc_List_List* fs= ed->fields;
for( 0; fs != 0; fs=({ struct Cyc_List_List* _temp1153= fs; if( _temp1153 == 0){
_throw( Null_Exception);} _temp1153->tl;})){ struct Cyc_Absyn_Enumfield* f=(
struct Cyc_Absyn_Enumfield*)({ struct Cyc_List_List* _temp1165= fs; if(
_temp1165 == 0){ _throw( Null_Exception);} _temp1165->hd;}); if((( int(*)( int(*
compare)( struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List*
l, struct _tagged_string* x)) Cyc_List_mem)( Cyc_String_zstrptrcmp, prev_fields,(*
f->name).f2)){ Cyc_Tcutil_terr( f->loc,({ struct _tagged_string _temp1154=*(* f->name).f2;
xprintf("duplicate field name %.*s", _temp1154.last_plus_one - _temp1154.curr,
_temp1154.curr);}));} prev_fields=({ struct Cyc_List_List* _temp1155=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1155->hd=( void*)(*
f->name).f2; _temp1155->tl= prev_fields; _temp1155;}); if( f->tag == 0){ f->tag=(
struct Cyc_Absyn_Exp*) Cyc_Absyn_uint_exp( tag_count, f->loc);} else{ if( ! Cyc_Tcutil_is_const_exp(
te,( struct Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1156= f->tag; if(
_temp1156 == 0){ _throw( Null_Exception);} _temp1156;}))){ Cyc_Tcutil_terr( loc,({
struct _tagged_string _temp1157=* v; struct _tagged_string _temp1158=*(* f->name).f2;
xprintf("enum %.*s, field %.*s: expression is not constant", _temp1157.last_plus_one
- _temp1157.curr, _temp1157.curr, _temp1158.last_plus_one - _temp1158.curr,
_temp1158.curr);}));}}{ unsigned int t1= Cyc_Evexp_eval_const_uint_exp(( struct
Cyc_Absyn_Exp*)({ struct Cyc_Absyn_Exp* _temp1164= f->tag; if( _temp1164 == 0){
_throw( Null_Exception);} _temp1164;})); tag_count= t1 + 1;(* f->name).f1=( void*)({
struct Cyc_Absyn_Abs_n_struct* _temp1159=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp1159[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp1160; _temp1160.tag= Cyc_Absyn_Abs_n; _temp1160.f1= te->ns; _temp1160;});
_temp1159;}); ge->ordinaries=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct _tuple6* data)) Cyc_Dict_insert)( ge->ordinaries,(*
f->name).f2,({ struct _tuple6* _temp1161=( struct _tuple6*) GC_malloc( sizeof(
struct _tuple6)); _temp1161->f1=( void*)({ struct Cyc_Tcenv_EnumRes_struct*
_temp1162=( struct Cyc_Tcenv_EnumRes_struct*) GC_malloc( sizeof( struct Cyc_Tcenv_EnumRes_struct));
_temp1162[ 0]=({ struct Cyc_Tcenv_EnumRes_struct _temp1163; _temp1163.tag= Cyc_Tcenv_EnumRes;
_temp1163.f1= ed; _temp1163.f2= f; _temp1163;}); _temp1162;}); _temp1161->f2= 1;
_temp1161;}));}}} ge->enumdecls=(( struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict*
d, struct _tagged_string* key, struct Cyc_Absyn_Enumdecl** data)) Cyc_Dict_insert)(
ge->enumdecls, v,({ struct Cyc_Absyn_Enumdecl** _temp1166=( struct Cyc_Absyn_Enumdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Enumdecl*)); _temp1166[ 0]= ed; _temp1166;}));}}
static int Cyc_Tc_okay_externC( struct Cyc_Position_Segment* loc, void* sc){
void* _temp1167= sc; _LL1169: if( _temp1167 ==( void*) Cyc_Absyn_Static){ goto
_LL1170;} else{ goto _LL1171;} _LL1171: if( _temp1167 ==( void*) Cyc_Absyn_Abstract){
goto _LL1172;} else{ goto _LL1173;} _LL1173: if( _temp1167 ==( void*) Cyc_Absyn_Public){
goto _LL1174;} else{ goto _LL1175;} _LL1175: if( _temp1167 ==( void*) Cyc_Absyn_Extern){
goto _LL1176;} else{ goto _LL1177;} _LL1177: if( _temp1167 ==( void*) Cyc_Absyn_ExternC){
goto _LL1178;} else{ goto _LL1168;} _LL1170: Cyc_Tcutil_warn( loc,( struct
_tagged_string)({ char* _temp1179=( char*)"static declaration nested within extern \"C\"";
struct _tagged_string _temp1180; _temp1180.curr= _temp1179; _temp1180.base=
_temp1179; _temp1180.last_plus_one= _temp1179 + 44; _temp1180;})); return 0;
_LL1172: Cyc_Tcutil_warn( loc,( struct _tagged_string)({ char* _temp1181=( char*)"abstract declaration nested within extern \"C\"";
struct _tagged_string _temp1182; _temp1182.curr= _temp1181; _temp1182.base=
_temp1181; _temp1182.last_plus_one= _temp1181 + 46; _temp1182;})); return 0;
_LL1174: return 1; _LL1176: return 1; _LL1178: Cyc_Tcutil_warn( loc,( struct
_tagged_string)({ char* _temp1183=( char*)"nested extern \"C\" declaration";
struct _tagged_string _temp1184; _temp1184.curr= _temp1183; _temp1184.base=
_temp1183; _temp1184.last_plus_one= _temp1183 + 30; _temp1184;})); return 1;
_LL1168:;} static void Cyc_Tc_tc_decls( struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List*
ds0, int in_externC){ struct Cyc_Tcenv_Genv* ge=(( struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict* d, struct Cyc_List_List* key)) Cyc_Dict_lookup)( te->ae,
te->ns); struct Cyc_List_List* ds= ds0; for( 0; ds != 0; ds=({ struct Cyc_List_List*
_temp1185= ds; if( _temp1185 == 0){ _throw( Null_Exception);} _temp1185->tl;})){
struct Cyc_Absyn_Decl* d=( struct Cyc_Absyn_Decl*)({ struct Cyc_List_List*
_temp1285= ds; if( _temp1285 == 0){ _throw( Null_Exception);} _temp1285->hd;});
struct Cyc_Position_Segment* loc= d->loc; void* _temp1186=( void*) d->r; int
_temp1210; struct Cyc_Absyn_Exp* _temp1212; struct Cyc_Core_Opt* _temp1214;
struct Cyc_Core_Opt* _temp1216; struct Cyc_Absyn_Pat* _temp1218; struct Cyc_Absyn_Vardecl*
_temp1220; struct Cyc_Absyn_Fndecl* _temp1222; struct Cyc_Absyn_Typedefdecl*
_temp1224; struct Cyc_Absyn_Structdecl* _temp1226; struct Cyc_Absyn_Uniondecl*
_temp1228; struct Cyc_Absyn_Tuniondecl* _temp1230; struct Cyc_Absyn_Enumdecl*
_temp1232; struct Cyc_List_List* _temp1234; struct _tagged_string* _temp1236;
struct Cyc_List_List* _temp1238; struct _tuple0* _temp1240; struct _tuple0
_temp1242; struct _tagged_string* _temp1243; void* _temp1245; struct Cyc_List_List*
_temp1247; _LL1188: if(*(( int*) _temp1186) == Cyc_Absyn_Let_d){ _LL1219:
_temp1218=( struct Cyc_Absyn_Pat*)(( struct Cyc_Absyn_Let_d_struct*) _temp1186)->f1;
goto _LL1217; _LL1217: _temp1216=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Let_d_struct*)
_temp1186)->f2; goto _LL1215; _LL1215: _temp1214=( struct Cyc_Core_Opt*)((
struct Cyc_Absyn_Let_d_struct*) _temp1186)->f3; goto _LL1213; _LL1213: _temp1212=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Let_d_struct*) _temp1186)->f4; goto
_LL1211; _LL1211: _temp1210=( int)(( struct Cyc_Absyn_Let_d_struct*) _temp1186)->f5;
goto _LL1189;} else{ goto _LL1190;} _LL1190: if(*(( int*) _temp1186) == Cyc_Absyn_Var_d){
_LL1221: _temp1220=( struct Cyc_Absyn_Vardecl*)(( struct Cyc_Absyn_Var_d_struct*)
_temp1186)->f1; goto _LL1191;} else{ goto _LL1192;} _LL1192: if(*(( int*)
_temp1186) == Cyc_Absyn_Fn_d){ _LL1223: _temp1222=( struct Cyc_Absyn_Fndecl*)((
struct Cyc_Absyn_Fn_d_struct*) _temp1186)->f1; goto _LL1193;} else{ goto _LL1194;}
_LL1194: if(*(( int*) _temp1186) == Cyc_Absyn_Typedef_d){ _LL1225: _temp1224=(
struct Cyc_Absyn_Typedefdecl*)(( struct Cyc_Absyn_Typedef_d_struct*) _temp1186)->f1;
goto _LL1195;} else{ goto _LL1196;} _LL1196: if(*(( int*) _temp1186) == Cyc_Absyn_Struct_d){
_LL1227: _temp1226=( struct Cyc_Absyn_Structdecl*)(( struct Cyc_Absyn_Struct_d_struct*)
_temp1186)->f1; goto _LL1197;} else{ goto _LL1198;} _LL1198: if(*(( int*)
_temp1186) == Cyc_Absyn_Union_d){ _LL1229: _temp1228=( struct Cyc_Absyn_Uniondecl*)((
struct Cyc_Absyn_Union_d_struct*) _temp1186)->f1; goto _LL1199;} else{ goto
_LL1200;} _LL1200: if(*(( int*) _temp1186) == Cyc_Absyn_Tunion_d){ _LL1231:
_temp1230=( struct Cyc_Absyn_Tuniondecl*)(( struct Cyc_Absyn_Tunion_d_struct*)
_temp1186)->f1; goto _LL1201;} else{ goto _LL1202;} _LL1202: if(*(( int*)
_temp1186) == Cyc_Absyn_Enum_d){ _LL1233: _temp1232=( struct Cyc_Absyn_Enumdecl*)((
struct Cyc_Absyn_Enum_d_struct*) _temp1186)->f1; goto _LL1203;} else{ goto
_LL1204;} _LL1204: if(*(( int*) _temp1186) == Cyc_Absyn_Namespace_d){ _LL1237:
_temp1236=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1186)->f1; goto _LL1235; _LL1235: _temp1234=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1186)->f2; goto _LL1205;} else{ goto
_LL1206;} _LL1206: if(*(( int*) _temp1186) == Cyc_Absyn_Using_d){ _LL1241:
_temp1240=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp1186)->f1;
_temp1242=* _temp1240; _LL1246: _temp1245= _temp1242.f1; goto _LL1244; _LL1244:
_temp1243= _temp1242.f2; goto _LL1239; _LL1239: _temp1238=( struct Cyc_List_List*)((
struct Cyc_Absyn_Using_d_struct*) _temp1186)->f2; goto _LL1207;} else{ goto
_LL1208;} _LL1208: if(*(( int*) _temp1186) == Cyc_Absyn_ExternC_d){ _LL1248:
_temp1247=( struct Cyc_List_List*)(( struct Cyc_Absyn_ExternC_d_struct*)
_temp1186)->f1; goto _LL1209;} else{ goto _LL1187;} _LL1189: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp1249=( char*)"top level let-declarations are not implemented";
struct _tagged_string _temp1250; _temp1250.curr= _temp1249; _temp1250.base=
_temp1249; _temp1250.last_plus_one= _temp1249 + 47; _temp1250;})); goto _LL1187;
_LL1191: if( in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp1220->sc): 0){(
void*)( _temp1220->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcVardecl(
te, ge, loc, _temp1220); goto _LL1187; _LL1193: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp1222->sc): 0){( void*)( _temp1222->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcFndecl( te, ge, loc, _temp1222); goto _LL1187; _LL1195: Cyc_Tc_tcTypedefdecl(
te, ge, loc, _temp1224); goto _LL1187; _LL1197: if( in_externC? Cyc_Tc_okay_externC(
d->loc,( void*) _temp1226->sc): 0){( void*)( _temp1226->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcStructdecl( te, ge, loc, _temp1226); goto _LL1187; _LL1199: if(
in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp1228->sc): 0){( void*)(
_temp1228->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcUniondecl( te, ge,
loc, _temp1228); goto _LL1187; _LL1201: if( in_externC? Cyc_Tc_okay_externC( d->loc,(
void*) _temp1230->sc): 0){( void*)( _temp1230->sc=( void*)(( void*) Cyc_Absyn_ExternC));}
Cyc_Tc_tcTuniondecl( te, ge, loc, _temp1230); goto _LL1187; _LL1203: if(
in_externC? Cyc_Tc_okay_externC( d->loc,( void*) _temp1232->sc): 0){( void*)(
_temp1232->sc=( void*)(( void*) Cyc_Absyn_ExternC));} Cyc_Tc_tcEnumdecl( te, ge,
loc, _temp1232); goto _LL1187; _LL1205: { struct Cyc_List_List* ns= te->ns;
struct Cyc_List_List* ns2=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x,
struct Cyc_List_List* y)) Cyc_List_append)( ns,({ struct Cyc_List_List*
_temp1251=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp1251->hd=( void*) _temp1236; _temp1251->tl= 0; _temp1251;})); if( !(( int(*)(
struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_member)( ge->namespaces,
_temp1236)){ ge->namespaces=(( struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s,
struct _tagged_string* elt)) Cyc_Set_insert)( ge->namespaces, _temp1236); te->ae=((
struct Cyc_Dict_Dict*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List* key,
struct Cyc_Tcenv_Genv* data)) Cyc_Dict_insert)( te->ae, ns2, Cyc_Tcenv_empty_genv());}
te->ns= ns2; Cyc_Tc_tc_decls( te, _temp1234, in_externC); te->ns= ns; goto
_LL1187;} _LL1207: { struct _tagged_string* first; struct Cyc_List_List* rest;{
void* _temp1252= _temp1245; struct Cyc_List_List* _temp1264; struct Cyc_List_List*
_temp1266; struct Cyc_List_List* _temp1268; struct Cyc_List_List _temp1270;
struct Cyc_List_List* _temp1271; struct _tagged_string* _temp1273; struct Cyc_List_List*
_temp1275; struct Cyc_List_List _temp1277; struct Cyc_List_List* _temp1278;
struct _tagged_string* _temp1280; _LL1254: if( _temp1252 ==( void*) Cyc_Absyn_Loc_n){
goto _LL1255;} else{ goto _LL1256;} _LL1256: if(( unsigned int) _temp1252 > 1u?*((
int*) _temp1252) == Cyc_Absyn_Rel_n: 0){ _LL1265: _temp1264=( struct Cyc_List_List*)((
struct Cyc_Absyn_Rel_n_struct*) _temp1252)->f1; if( _temp1264 == 0){ goto
_LL1257;} else{ goto _LL1258;}} else{ goto _LL1258;} _LL1258: if(( unsigned int)
_temp1252 > 1u?*(( int*) _temp1252) == Cyc_Absyn_Abs_n: 0){ _LL1267: _temp1266=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp1252)->f1; if(
_temp1266 == 0){ goto _LL1259;} else{ goto _LL1260;}} else{ goto _LL1260;}
_LL1260: if(( unsigned int) _temp1252 > 1u?*(( int*) _temp1252) == Cyc_Absyn_Rel_n:
0){ _LL1269: _temp1268=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp1252)->f1; if( _temp1268 == 0){ goto _LL1262;} else{ _temp1270=* _temp1268;
_LL1274: _temp1273=( struct _tagged_string*) _temp1270.hd; goto _LL1272; _LL1272:
_temp1271=( struct Cyc_List_List*) _temp1270.tl; goto _LL1261;}} else{ goto
_LL1262;} _LL1262: if(( unsigned int) _temp1252 > 1u?*(( int*) _temp1252) == Cyc_Absyn_Abs_n:
0){ _LL1276: _temp1275=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp1252)->f1; if( _temp1275 == 0){ goto _LL1253;} else{ _temp1277=* _temp1275;
_LL1281: _temp1280=( struct _tagged_string*) _temp1277.hd; goto _LL1279; _LL1279:
_temp1278=( struct Cyc_List_List*) _temp1277.tl; goto _LL1263;}} else{ goto
_LL1253;} _LL1255: goto _LL1257; _LL1257: goto _LL1259; _LL1259: first=
_temp1243; rest= 0; goto _LL1253; _LL1261: _temp1280= _temp1273; _temp1278=
_temp1271; goto _LL1263; _LL1263: first= _temp1280; rest=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x, struct Cyc_List_List* y)) Cyc_List_append)( _temp1278,({
struct Cyc_List_List* _temp1282=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1282->hd=( void*) _temp1243; _temp1282->tl= 0;
_temp1282;})); goto _LL1253; _LL1253:;}{ struct Cyc_List_List* ns2= Cyc_Tcenv_resolve_namespace(
te, loc, first, rest); ge->availables=({ struct Cyc_List_List* _temp1283=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp1283->hd=(
void*) ns2; _temp1283->tl= ge->availables; _temp1283;}); Cyc_Tc_tc_decls( te,
_temp1238, in_externC); ge->availables=({ struct Cyc_List_List* _temp1284= ge->availables;
if( _temp1284 == 0){ _throw( Null_Exception);} _temp1284->tl;}); goto _LL1187;}}
_LL1209: Cyc_Tc_tc_decls( te, _temp1247, 1); goto _LL1187; _LL1187:;}} static
char _temp1288[ 4u]="Cyc"; static struct _tagged_string Cyc_Tc_cyc_string=(
struct _tagged_string){ _temp1288, _temp1288, _temp1288 + 4u}; static struct
_tagged_string* Cyc_Tc_cyc_ns=& Cyc_Tc_cyc_string; void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv*
te, int add_cyc_namespace, struct Cyc_List_List* ds){ if( add_cyc_namespace){ ds=({
struct Cyc_List_List* _temp1289=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp1289->hd=( void*)({ struct Cyc_Absyn_Decl*
_temp1290=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp1290->r=( void*)(( void*)({ struct Cyc_Absyn_Namespace_d_struct* _temp1291=(
struct Cyc_Absyn_Namespace_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Namespace_d_struct));
_temp1291[ 0]=({ struct Cyc_Absyn_Namespace_d_struct _temp1292; _temp1292.tag=
Cyc_Absyn_Namespace_d; _temp1292.f1= Cyc_Tc_cyc_ns; _temp1292.f2= ds; _temp1292;});
_temp1291;})); _temp1290->loc= 0; _temp1290;}); _temp1289->tl= 0; _temp1289;});}
Cyc_Tc_tc_decls( te, ds, 0);} typedef struct Cyc_Dict_Dict* Cyc_Tc_treeshake_env_t;
static int Cyc_Tc_vardecl_needed( struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl*
d); static struct Cyc_List_List* Cyc_Tc_treeshake_f( struct Cyc_Dict_Dict* env,
struct Cyc_List_List* ds){ return(( struct Cyc_List_List*(*)( int(* f)( struct
Cyc_Dict_Dict*, struct Cyc_Absyn_Decl*), struct Cyc_Dict_Dict* env, struct Cyc_List_List*
l)) Cyc_List_filter_c)( Cyc_Tc_vardecl_needed, env, ds);} struct _tuple8{ struct
Cyc_Tcenv_Genv* f1; struct Cyc_Set_Set* f2; } ; static int Cyc_Tc_vardecl_needed(
struct Cyc_Dict_Dict* env, struct Cyc_Absyn_Decl* d){ void* _temp1293=( void*) d->r;
struct Cyc_Absyn_Vardecl* _temp1303; struct Cyc_List_List* _temp1305; struct Cyc_List_List**
_temp1307; struct _tuple0* _temp1308; struct Cyc_List_List* _temp1310; struct
Cyc_List_List** _temp1312; struct _tagged_string* _temp1313; _LL1295: if(*(( int*)
_temp1293) == Cyc_Absyn_Var_d){ _LL1304: _temp1303=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Var_d_struct*) _temp1293)->f1; goto _LL1296;} else{ goto
_LL1297;} _LL1297: if(*(( int*) _temp1293) == Cyc_Absyn_Using_d){ _LL1309:
_temp1308=( struct _tuple0*)(( struct Cyc_Absyn_Using_d_struct*) _temp1293)->f1;
goto _LL1306; _LL1306: _temp1305=( struct Cyc_List_List*)(( struct Cyc_Absyn_Using_d_struct*)
_temp1293)->f2; _temp1307=&(( struct Cyc_Absyn_Using_d_struct*) _temp1293)->f2;
goto _LL1298;} else{ goto _LL1299;} _LL1299: if(*(( int*) _temp1293) == Cyc_Absyn_Namespace_d){
_LL1314: _temp1313=( struct _tagged_string*)(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1293)->f1; goto _LL1311; _LL1311: _temp1310=( struct Cyc_List_List*)((
struct Cyc_Absyn_Namespace_d_struct*) _temp1293)->f2; _temp1312=&(( struct Cyc_Absyn_Namespace_d_struct*)
_temp1293)->f2; goto _LL1300;} else{ goto _LL1301;} _LL1301: goto _LL1302;
_LL1296: if(( void*) _temp1303->sc !=( void*) Cyc_Absyn_Extern){ return 1;}{
struct _tuple0 _temp1317; struct _tagged_string* _temp1318; void* _temp1320;
struct _tuple0* _temp1315= _temp1303->name; _temp1317=* _temp1315; _LL1321:
_temp1320= _temp1317.f1; goto _LL1319; _LL1319: _temp1318= _temp1317.f2; goto
_LL1316; _LL1316: { struct Cyc_List_List* ns;{ void* _temp1322= _temp1320;
struct Cyc_List_List* _temp1330; struct Cyc_List_List* _temp1332; _LL1324: if(
_temp1322 ==( void*) Cyc_Absyn_Loc_n){ goto _LL1325;} else{ goto _LL1326;}
_LL1326: if(( unsigned int) _temp1322 > 1u?*(( int*) _temp1322) == Cyc_Absyn_Rel_n:
0){ _LL1331: _temp1330=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp1322)->f1; goto _LL1327;} else{ goto _LL1328;} _LL1328: if(( unsigned int)
_temp1322 > 1u?*(( int*) _temp1322) == Cyc_Absyn_Abs_n: 0){ _LL1333: _temp1332=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*) _temp1322)->f1; goto
_LL1329;} else{ goto _LL1323;} _LL1325: ns= 0; goto _LL1323; _LL1327: ns=
_temp1330; goto _LL1323; _LL1329: ns= _temp1332; goto _LL1323; _LL1323:;}{
struct _tuple8* nsenv=(( struct _tuple8*(*)( struct Cyc_Dict_Dict* d, struct Cyc_List_List*
key)) Cyc_Dict_lookup)( env, ns); struct Cyc_Tcenv_Genv* ge=(* nsenv).f1; int
needed=(*(( struct _tuple6*(*)( struct Cyc_Dict_Dict* d, struct _tagged_string*
key)) Cyc_Dict_lookup)( ge->ordinaries, _temp1318)).f2; if( ! needed){(* nsenv).f2=((
struct Cyc_Set_Set*(*)( struct Cyc_Set_Set* s, struct _tagged_string* elt)) Cyc_Set_insert)((*
nsenv).f2, _temp1318);} return needed;}}} _LL1298: _temp1312= _temp1307; goto
_LL1300; _LL1300:* _temp1312= Cyc_Tc_treeshake_f( env,* _temp1312); return 1;
_LL1302: return 1; _LL1294:;} static int Cyc_Tc_treeshake_remove_f( struct Cyc_Set_Set*
set, struct _tagged_string* x, struct _tuple6* y){ return !(( int(*)( struct Cyc_Set_Set*
s, struct _tagged_string* elt)) Cyc_Set_member)( set, x);} static void Cyc_Tc_treeshake_remove(
struct Cyc_List_List* ns, struct _tuple8* nsenv){ struct Cyc_Tcenv_Genv* ge=(*
nsenv).f1; struct Cyc_Set_Set* set=(* nsenv).f2; ge->ordinaries=(( struct Cyc_Dict_Dict*(*)(
int(* f)( struct Cyc_Set_Set*, struct _tagged_string*, struct _tuple6*), struct
Cyc_Set_Set* env, struct Cyc_Dict_Dict* d)) Cyc_Dict_filter_c)( Cyc_Tc_treeshake_remove_f,
set, ge->ordinaries);} static struct _tuple8* Cyc_Tc_treeshake_make_env_f(
struct Cyc_Tcenv_Genv* ge){ return({ struct _tuple8* _temp1334=( struct _tuple8*)
GC_malloc( sizeof( struct _tuple8)); _temp1334->f1= ge; _temp1334->f2=(( struct
Cyc_Set_Set*(*)( int(* comp)( struct _tagged_string*, struct _tagged_string*)))
Cyc_Set_empty)( Cyc_String_strptrcmp); _temp1334;});} struct Cyc_List_List* Cyc_Tc_treeshake(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* ds){ struct Cyc_Dict_Dict* env=((
struct Cyc_Dict_Dict*(*)( struct _tuple8*(* f)( struct Cyc_Tcenv_Genv*), struct
Cyc_Dict_Dict* d)) Cyc_Dict_map)( Cyc_Tc_treeshake_make_env_f, te->ae); struct
Cyc_List_List* ds1= Cyc_Tc_treeshake_f( env, ds);(( void(*)( void(* f)( struct
Cyc_List_List*, struct _tuple8*), struct Cyc_Dict_Dict* d)) Cyc_Dict_iter)( Cyc_Tc_treeshake_remove,
env); return ds1;}