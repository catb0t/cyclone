#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple3{ struct
Cyc_Absyn_Tqual* f1; void* f2; } ; struct _tuple4{ void* f1; void* f2; } ;
struct _tuple5{ struct Cyc_List_List* f1; struct Cyc_Absyn_Exp* f2; } ; typedef
unsigned int Cyc_uint; typedef char* Cyc_Cstring; typedef struct _tagged_string
Cyc_string; typedef struct _tagged_string Cyc_string_t; typedef struct
_tagged_string* Cyc_stringptr; typedef int Cyc_bool; extern void* exit( int);
extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; typedef struct Cyc_Core_Opt*
Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[ 11u]; struct Cyc_Core_InvalidArg_struct{
char* tag; struct _tagged_string f1; } ; extern char Cyc_Core_Failure_tag[ 8u];
struct Cyc_Core_Failure_struct{ char* tag; struct _tagged_string f1; } ; extern
char Cyc_Core_Impossible_tag[ 11u]; struct Cyc_Core_Impossible_struct{ char* tag;
struct _tagged_string f1; } ; extern char Cyc_Core_Not_found_tag[ 10u]; struct
Cyc_Core_Not_found_struct{ char* tag; } ; extern char Cyc_Core_Unreachable_tag[
12u]; struct Cyc_Core_Unreachable_struct{ char* tag; struct _tagged_string f1; }
; extern char* string_to_Cstring( struct _tagged_string); extern char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_List_List{ void*
hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern char Cyc_List_List_empty_tag[ 11u]; struct Cyc_List_List_empty_struct{
char* tag; } ; extern struct Cyc_List_List* Cyc_List_map( void*(* f)( void*),
struct Cyc_List_List* x); extern char Cyc_List_List_mismatch_tag[ 14u]; struct
Cyc_List_List_mismatch_struct{ char* tag; } ; extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern char Cyc_List_Nth_tag[ 4u]; struct Cyc_List_Nth_struct{
char* tag; } ; extern int Cyc_List_list_cmp( int(* cmp)( void*, void*), struct
Cyc_List_List* l1, struct Cyc_List_List* l2); struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col; }
; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[
5u]; struct Cyc_Position_Exit_struct{ char* tag; } ; struct Cyc_Position_Segment;
typedef struct Cyc_Position_Segment* Cyc_Position_seg_t; typedef void* Cyc_Position_Error_kind;
extern const unsigned int Cyc_Position_Lex; extern const unsigned int Cyc_Position_Parse;
extern const unsigned int Cyc_Position_Elab; typedef void* Cyc_Position_error_kind_t;
struct Cyc_Position_Error{ struct _tagged_string source; struct Cyc_Position_Segment*
seg; void* kind; struct _tagged_string desc; } ; typedef struct Cyc_Position_Error*
Cyc_Position_error_t; extern char Cyc_Position_Nocontext_tag[ 10u]; struct Cyc_Position_Nocontext_struct{
char* tag; } ; typedef struct _tagged_string* Cyc_Absyn_field_name_t; typedef
struct _tagged_string* Cyc_Absyn_var_t; typedef struct _tagged_string* Cyc_Absyn_tvarname_t;
typedef void* Cyc_Absyn_Nmspace; typedef void* Cyc_Absyn_nmspace_t; typedef
struct _tuple0* Cyc_Absyn_qvar_t; typedef struct _tuple0* Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0* Cyc_Absyn_typedef_name_t; typedef struct _tuple0* Cyc_Absyn_typedef_name_opt_t;
struct Cyc_Absyn_Tvar; typedef void* Cyc_Absyn_Scope; struct Cyc_Absyn_Tqual;
typedef void* Cyc_Absyn_Size_of; typedef void* Cyc_Absyn_Kind; typedef void* Cyc_Absyn_Array_kind;
typedef void* Cyc_Absyn_Sign; struct Cyc_Absyn_Conref; typedef void* Cyc_Absyn_Constraint;
typedef void* Cyc_Absyn_Bounds; struct Cyc_Absyn_PtrInfo; struct Cyc_Absyn_FnInfo;
struct Cyc_Absyn_TunionInfo; struct Cyc_Absyn_XTunionInfo; struct Cyc_Absyn_TunionFieldInfo;
struct Cyc_Absyn_XTunionFieldInfo; typedef void* Cyc_Absyn_Type; typedef void*
Cyc_Absyn_Funcparams; typedef void* Cyc_Absyn_Type_modifier; typedef void* Cyc_Absyn_Cnst;
typedef void* Cyc_Absyn_Primop; typedef void* Cyc_Absyn_Incrementor; typedef
void* Cyc_Absyn_Raw_exp; struct Cyc_Absyn_Exp; typedef void* Cyc_Absyn_Raw_stmt;
struct Cyc_Absyn_Stmt; typedef void* Cyc_Absyn_Raw_pat; struct Cyc_Absyn_Pat;
typedef void* Cyc_Absyn_Binding; struct Cyc_Absyn_Switch_clause; struct Cyc_Absyn_Fndecl;
struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl; struct Cyc_Absyn_Tuniondecl;
struct Cyc_Absyn_XTuniondecl; struct Cyc_Absyn_Tunionfield; struct Cyc_Absyn_Enumfield;
struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl;
typedef void* Cyc_Absyn_Raw_decl; struct Cyc_Absyn_Decl; typedef void* Cyc_Absyn_Designator;
typedef struct _xtunion_struct* Cyc_Absyn_StmtAnnot; typedef void* Cyc_Absyn_Attribute;
struct Cyc_Absyn_Structfield; typedef void* Cyc_Absyn_Mallocarg; typedef void*
Cyc_Absyn_scope_t; typedef struct Cyc_Absyn_Tqual* Cyc_Absyn_tqual_t; typedef
void* Cyc_Absyn_size_of_t; typedef void* Cyc_Absyn_kind_t; typedef struct Cyc_Absyn_Tvar*
Cyc_Absyn_tvar_t; typedef void* Cyc_Absyn_sign_t; typedef struct Cyc_Absyn_Conref*
Cyc_Absyn_conref_t; typedef void* Cyc_Absyn_constraint_t; typedef void* Cyc_Absyn_bounds_t;
typedef struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t; typedef struct Cyc_Absyn_FnInfo
Cyc_Absyn_fn_info_t; typedef struct Cyc_Absyn_TunionInfo Cyc_Absyn_tunion_info_t;
typedef struct Cyc_Absyn_XTunionInfo Cyc_Absyn_xtunion_info_t; typedef struct
Cyc_Absyn_TunionFieldInfo Cyc_Absyn_tunion_field_info_t; typedef struct Cyc_Absyn_XTunionFieldInfo
Cyc_Absyn_xtunion_field_info_t; typedef void* Cyc_Absyn_type_t; typedef void*
Cyc_Absyn_funcparams_t; typedef void* Cyc_Absyn_type_modifier_t; typedef void*
Cyc_Absyn_cnst_t; typedef void* Cyc_Absyn_primop_t; typedef void* Cyc_Absyn_incrementor_t;
typedef void* Cyc_Absyn_raw_exp_t; typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_t;
typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_opt_t; typedef void* Cyc_Absyn_raw_stmt_t;
typedef struct Cyc_Absyn_Stmt* Cyc_Absyn_stmt_t; typedef struct Cyc_Absyn_Stmt*
Cyc_Absyn_stmt_opt_t; typedef void* Cyc_Absyn_raw_pat_t; typedef struct Cyc_Absyn_Pat*
Cyc_Absyn_pat_t; typedef void* Cyc_Absyn_binding_t; typedef struct Cyc_Absyn_Switch_clause*
Cyc_Absyn_switch_clause_t; typedef struct Cyc_Absyn_Fndecl* Cyc_Absyn_fndecl_t;
typedef struct Cyc_Absyn_Structdecl* Cyc_Absyn_structdecl_t; typedef struct Cyc_Absyn_Uniondecl*
Cyc_Absyn_uniondecl_t; typedef struct Cyc_Absyn_Tunionfield* Cyc_Absyn_tunionfield_t;
typedef struct Cyc_Absyn_Tuniondecl* Cyc_Absyn_tuniondecl_t; typedef struct Cyc_Absyn_XTuniondecl*
Cyc_Absyn_xtuniondecl_t; typedef struct Cyc_Absyn_Typedefdecl* Cyc_Absyn_typedefdecl_t;
typedef struct Cyc_Absyn_Enumfield* Cyc_Absyn_enumfield_t; typedef struct Cyc_Absyn_Enumdecl*
Cyc_Absyn_enumdecl_t; typedef struct Cyc_Absyn_Vardecl* Cyc_Absyn_vardecl_t;
typedef void* Cyc_Absyn_raw_decl_t; typedef struct Cyc_Absyn_Decl* Cyc_Absyn_decl_t;
typedef void* Cyc_Absyn_designator_t; typedef struct _xtunion_struct* Cyc_Absyn_stmt_annot_t;
typedef void* Cyc_Absyn_attribute_t; typedef struct Cyc_List_List* Cyc_Absyn_attributes_t;
typedef struct Cyc_Absyn_Structfield* Cyc_Absyn_structfield_t; typedef void* Cyc_Absyn_mallocarg_t;
const unsigned int Cyc_Absyn_Loc_n= 0; const int Cyc_Absyn_Rel_n_tag= 0; struct
Cyc_Absyn_Rel_n_struct{ int tag; struct Cyc_List_List* f1; } ; const int Cyc_Absyn_Abs_n_tag=
1; struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List* f1; } ; const
unsigned int Cyc_Absyn_Static= 0; const unsigned int Cyc_Absyn_Abstract= 1;
const unsigned int Cyc_Absyn_Public= 2; const unsigned int Cyc_Absyn_Extern= 3;
const unsigned int Cyc_Absyn_ExternC= 4; struct Cyc_Absyn_Tqual{ int q_const: 1;
int q_volatile: 1; int q_restrict: 1; } ; const unsigned int Cyc_Absyn_B1= 0;
const unsigned int Cyc_Absyn_B2= 1; const unsigned int Cyc_Absyn_B4= 2; const
unsigned int Cyc_Absyn_B8= 3; const unsigned int Cyc_Absyn_AnyKind= 0; const
unsigned int Cyc_Absyn_MemKind= 1; const unsigned int Cyc_Absyn_BoxKind= 2;
const unsigned int Cyc_Absyn_RgnKind= 3; const unsigned int Cyc_Absyn_EffKind= 4;
const unsigned int Cyc_Absyn_Signed= 0; const unsigned int Cyc_Absyn_Unsigned= 1;
struct Cyc_Absyn_Conref{ void* v; } ; const int Cyc_Absyn_Eq_constr_tag= 0;
struct Cyc_Absyn_Eq_constr_struct{ int tag; void* f1; } ; const int Cyc_Absyn_Forward_constr_tag=
1; struct Cyc_Absyn_Forward_constr_struct{ int tag; struct Cyc_Absyn_Conref* f1;
} ; const unsigned int Cyc_Absyn_No_constr= 0; struct Cyc_Absyn_Tvar{ struct
_tagged_string* name; struct Cyc_Absyn_Conref* kind; } ; const unsigned int Cyc_Absyn_Unknown_b=
0; const int Cyc_Absyn_Upper_b_tag= 0; struct Cyc_Absyn_Upper_b_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_PtrInfo{ void* elt_typ; void*
rgn_typ; struct Cyc_Absyn_Conref* nullable; struct Cyc_Absyn_Tqual* tq; struct
Cyc_Absyn_Conref* bounds; } ; struct Cyc_Absyn_FnInfo{ struct Cyc_List_List*
tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct Cyc_List_List* args;
int varargs; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_TunionInfo{
struct _tuple0* name; struct Cyc_List_List* targs; void* rgn; struct Cyc_Absyn_Tuniondecl*
tud; } ; struct Cyc_Absyn_XTunionInfo{ struct _tuple0* name; void* rgn; struct
Cyc_Absyn_XTuniondecl* xtud; } ; struct Cyc_Absyn_TunionFieldInfo{ struct
_tuple0* name; struct Cyc_List_List* targs; struct _tuple0* fname; struct Cyc_Absyn_Tuniondecl*
tud; struct Cyc_Absyn_Tunionfield* tufd; } ; struct Cyc_Absyn_XTunionFieldInfo{
struct _tuple0* name; struct _tuple0* fname; struct Cyc_Absyn_XTuniondecl* xtud;
struct Cyc_Absyn_Tunionfield* xtufd; } ; const unsigned int Cyc_Absyn_VoidType=
0; const int Cyc_Absyn_Evar_tag= 0; struct Cyc_Absyn_Evar_struct{ int tag; void*
f1; struct Cyc_Core_Opt* f2; int f3; } ; const int Cyc_Absyn_VarType_tag= 1;
struct Cyc_Absyn_VarType_struct{ int tag; struct Cyc_Absyn_Tvar* f1; } ; const
int Cyc_Absyn_TunionType_tag= 2; struct Cyc_Absyn_TunionType_struct{ int tag;
struct Cyc_Absyn_TunionInfo f1; } ; const int Cyc_Absyn_XTunionType_tag= 3;
struct Cyc_Absyn_XTunionType_struct{ int tag; struct Cyc_Absyn_XTunionInfo f1; }
; const int Cyc_Absyn_TunionFieldType_tag= 4; struct Cyc_Absyn_TunionFieldType_struct{
int tag; struct Cyc_Absyn_TunionFieldInfo f1; } ; const int Cyc_Absyn_XTunionFieldType_tag=
5; struct Cyc_Absyn_XTunionFieldType_struct{ int tag; struct Cyc_Absyn_XTunionFieldInfo
f1; } ; const int Cyc_Absyn_PointerType_tag= 6; struct Cyc_Absyn_PointerType_struct{
int tag; struct Cyc_Absyn_PtrInfo f1; } ; const int Cyc_Absyn_IntType_tag= 7;
struct Cyc_Absyn_IntType_struct{ int tag; void* f1; void* f2; } ; const
unsigned int Cyc_Absyn_FloatType= 1; const unsigned int Cyc_Absyn_DoubleType= 2;
const int Cyc_Absyn_ArrayType_tag= 8; struct Cyc_Absyn_ArrayType_struct{ int tag;
void* f1; struct Cyc_Absyn_Tqual* f2; struct Cyc_Absyn_Exp* f3; } ; const int
Cyc_Absyn_FnType_tag= 9; struct Cyc_Absyn_FnType_struct{ int tag; struct Cyc_Absyn_FnInfo
f1; } ; const int Cyc_Absyn_TupleType_tag= 10; struct Cyc_Absyn_TupleType_struct{
int tag; struct Cyc_List_List* f1; } ; const int Cyc_Absyn_StructType_tag= 11;
struct Cyc_Absyn_StructType_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Structdecl** f3; } ; const int Cyc_Absyn_UnionType_tag= 12;
struct Cyc_Absyn_UnionType_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Uniondecl** f3; } ; const int Cyc_Absyn_EnumType_tag= 13;
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl*
f2; } ; const int Cyc_Absyn_RgnHandleType_tag= 14; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; const int Cyc_Absyn_TypedefType_tag= 15; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
} ; const unsigned int Cyc_Absyn_HeapRgn= 3; const int Cyc_Absyn_AccessEff_tag=
16; struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; const int Cyc_Absyn_JoinEff_tag=
17; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List* f1; } ;
const int Cyc_Absyn_NoTypes_tag= 0; struct Cyc_Absyn_NoTypes_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; const int Cyc_Absyn_WithTypes_tag=
1; struct Cyc_Absyn_WithTypes_struct{ int tag; struct Cyc_List_List* f1; int f2;
struct Cyc_Core_Opt* f3; } ; typedef void* Cyc_Absyn_Pointer_Sort; const int Cyc_Absyn_NonNullable_ps_tag=
0; struct Cyc_Absyn_NonNullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; }
; const int Cyc_Absyn_Nullable_ps_tag= 1; struct Cyc_Absyn_Nullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; const unsigned int Cyc_Absyn_TaggedArray_ps=
0; const int Cyc_Absyn_Regparm_att_tag= 0; struct Cyc_Absyn_Regparm_att_struct{
int tag; int f1; } ; const unsigned int Cyc_Absyn_Stdcall_att= 0; const
unsigned int Cyc_Absyn_Cdecl_att= 1; const unsigned int Cyc_Absyn_Noreturn_att=
2; const unsigned int Cyc_Absyn_Const_att= 3; const int Cyc_Absyn_Aligned_att_tag=
1; struct Cyc_Absyn_Aligned_att_struct{ int tag; int f1; } ; const unsigned int
Cyc_Absyn_Packed_att= 4; const int Cyc_Absyn_Section_att_tag= 2; struct Cyc_Absyn_Section_att_struct{
int tag; struct _tagged_string f1; } ; const unsigned int Cyc_Absyn_Nocommon_att=
5; const unsigned int Cyc_Absyn_Shared_att= 6; const unsigned int Cyc_Absyn_Unused_att=
7; const unsigned int Cyc_Absyn_Weak_att= 8; const unsigned int Cyc_Absyn_Dllimport_att=
9; const unsigned int Cyc_Absyn_Dllexport_att= 10; const unsigned int Cyc_Absyn_No_instrument_function_att=
11; const unsigned int Cyc_Absyn_Constructor_att= 12; const unsigned int Cyc_Absyn_Destructor_att=
13; const unsigned int Cyc_Absyn_No_check_memory_usage_att= 14; const
unsigned int Cyc_Absyn_Carray_mod= 0; const int Cyc_Absyn_ConstArray_mod_tag= 0;
struct Cyc_Absyn_ConstArray_mod_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
const int Cyc_Absyn_Pointer_mod_tag= 1; struct Cyc_Absyn_Pointer_mod_struct{ int
tag; void* f1; void* f2; struct Cyc_Absyn_Tqual* f3; } ; const int Cyc_Absyn_Function_mod_tag=
2; struct Cyc_Absyn_Function_mod_struct{ int tag; void* f1; } ; const int Cyc_Absyn_TypeParams_mod_tag=
3; struct Cyc_Absyn_TypeParams_mod_struct{ int tag; struct Cyc_List_List* f1;
struct Cyc_Position_Segment* f2; int f3; } ; const int Cyc_Absyn_Attributes_mod_tag=
4; struct Cyc_Absyn_Attributes_mod_struct{ int tag; struct Cyc_Position_Segment*
f1; struct Cyc_List_List* f2; } ; const int Cyc_Absyn_Char_c_tag= 0; struct Cyc_Absyn_Char_c_struct{
int tag; void* f1; char f2; } ; const int Cyc_Absyn_Short_c_tag= 1; struct Cyc_Absyn_Short_c_struct{
int tag; void* f1; short f2; } ; const int Cyc_Absyn_Int_c_tag= 2; struct Cyc_Absyn_Int_c_struct{
int tag; void* f1; int f2; } ; const int Cyc_Absyn_LongLong_c_tag= 3; struct Cyc_Absyn_LongLong_c_struct{
int tag; void* f1; long long f2; } ; const int Cyc_Absyn_Float_c_tag= 4; struct
Cyc_Absyn_Float_c_struct{ int tag; struct _tagged_string f1; } ; const int Cyc_Absyn_String_c_tag=
5; struct Cyc_Absyn_String_c_struct{ int tag; struct _tagged_string f1; } ;
const unsigned int Cyc_Absyn_Null_c= 0; const unsigned int Cyc_Absyn_Plus= 0;
const unsigned int Cyc_Absyn_Times= 1; const unsigned int Cyc_Absyn_Minus= 2;
const unsigned int Cyc_Absyn_Div= 3; const unsigned int Cyc_Absyn_Mod= 4; const
unsigned int Cyc_Absyn_Eq= 5; const unsigned int Cyc_Absyn_Neq= 6; const
unsigned int Cyc_Absyn_Gt= 7; const unsigned int Cyc_Absyn_Lt= 8; const
unsigned int Cyc_Absyn_Gte= 9; const unsigned int Cyc_Absyn_Lte= 10; const
unsigned int Cyc_Absyn_Not= 11; const unsigned int Cyc_Absyn_Bitnot= 12; const
unsigned int Cyc_Absyn_Bitand= 13; const unsigned int Cyc_Absyn_Bitor= 14; const
unsigned int Cyc_Absyn_Bitxor= 15; const unsigned int Cyc_Absyn_Bitlshift= 16;
const unsigned int Cyc_Absyn_Bitlrshift= 17; const unsigned int Cyc_Absyn_Bitarshift=
18; const unsigned int Cyc_Absyn_Size= 19; const unsigned int Cyc_Absyn_Printf=
20; const unsigned int Cyc_Absyn_Fprintf= 21; const unsigned int Cyc_Absyn_Xprintf=
22; const unsigned int Cyc_Absyn_Scanf= 23; const unsigned int Cyc_Absyn_Fscanf=
24; const unsigned int Cyc_Absyn_Sscanf= 25; const unsigned int Cyc_Absyn_PreInc=
0; const unsigned int Cyc_Absyn_PostInc= 1; const unsigned int Cyc_Absyn_PreDec=
2; const unsigned int Cyc_Absyn_PostDec= 3; const int Cyc_Absyn_Typ_m_tag= 0;
struct Cyc_Absyn_Typ_m_struct{ int tag; void* f1; } ; const int Cyc_Absyn_Unresolved_m_tag=
1; struct Cyc_Absyn_Unresolved_m_struct{ int tag; struct _tuple0* f1; } ; const
int Cyc_Absyn_Tunion_m_tag= 2; struct Cyc_Absyn_Tunion_m_struct{ int tag; struct
Cyc_Absyn_Tuniondecl* f1; struct Cyc_Absyn_Tunionfield* f2; } ; const int Cyc_Absyn_XTunion_m_tag=
3; struct Cyc_Absyn_XTunion_m_struct{ int tag; struct Cyc_Absyn_XTuniondecl* f1;
struct Cyc_Absyn_Tunionfield* f2; } ; const int Cyc_Absyn_Const_e_tag= 0; struct
Cyc_Absyn_Const_e_struct{ int tag; void* f1; } ; const int Cyc_Absyn_Var_e_tag=
1; struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple0* f1; void* f2; } ;
const int Cyc_Absyn_UnknownId_e_tag= 2; struct Cyc_Absyn_UnknownId_e_struct{ int
tag; struct _tuple0* f1; } ; const int Cyc_Absyn_Primop_e_tag= 3; struct Cyc_Absyn_Primop_e_struct{
int tag; void* f1; struct Cyc_List_List* f2; } ; const int Cyc_Absyn_AssignOp_e_tag=
4; struct Cyc_Absyn_AssignOp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Core_Opt* f2; struct Cyc_Absyn_Exp* f3; } ; const int Cyc_Absyn_Increment_e_tag=
5; struct Cyc_Absyn_Increment_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; void*
f2; } ; const int Cyc_Absyn_Conditional_e_tag= 6; struct Cyc_Absyn_Conditional_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; const int Cyc_Absyn_SeqExp_e_tag= 7; struct Cyc_Absyn_SeqExp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; const int Cyc_Absyn_UnknownCall_e_tag=
8; struct Cyc_Absyn_UnknownCall_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_List_List* f2; } ; const int Cyc_Absyn_FnCall_e_tag= 9; struct Cyc_Absyn_FnCall_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; const int Cyc_Absyn_Throw_e_tag=
10; struct Cyc_Absyn_Throw_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
const int Cyc_Absyn_NoInstantiate_e_tag= 11; struct Cyc_Absyn_NoInstantiate_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; const int Cyc_Absyn_Instantiate_e_tag= 12;
struct Cyc_Absyn_Instantiate_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_List_List* f2; } ; const int Cyc_Absyn_Cast_e_tag= 13; struct Cyc_Absyn_Cast_e_struct{
int tag; void* f1; struct Cyc_Absyn_Exp* f2; } ; const int Cyc_Absyn_Address_e_tag=
14; struct Cyc_Absyn_Address_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
const int Cyc_Absyn_New_e_tag= 15; struct Cyc_Absyn_New_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; const int Cyc_Absyn_Sizeoftyp_e_tag= 16; struct
Cyc_Absyn_Sizeoftyp_e_struct{ int tag; void* f1; } ; const int Cyc_Absyn_Sizeofexp_e_tag=
17; struct Cyc_Absyn_Sizeofexp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
const int Cyc_Absyn_Deref_e_tag= 18; struct Cyc_Absyn_Deref_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; const int Cyc_Absyn_StructMember_e_tag= 19; struct
Cyc_Absyn_StructMember_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tagged_string* f2; } ; const int Cyc_Absyn_StructArrow_e_tag= 20; struct Cyc_Absyn_StructArrow_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tagged_string* f2; } ; const int Cyc_Absyn_Subscript_e_tag=
21; struct Cyc_Absyn_Subscript_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Exp* f2; } ; const int Cyc_Absyn_Tuple_e_tag= 22; struct Cyc_Absyn_Tuple_e_struct{
int tag; struct Cyc_List_List* f1; } ; const int Cyc_Absyn_CompoundLit_e_tag= 23;
struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; } ; const int Cyc_Absyn_Array_e_tag= 24; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; const int Cyc_Absyn_Comprehension_e_tag=
25; struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; const int Cyc_Absyn_Struct_e_tag=
26; struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple0* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; const int Cyc_Absyn_Tunion_e_tag=
27; struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct
Cyc_Core_Opt* f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4;
struct Cyc_Absyn_Tunionfield* f5; } ; const int Cyc_Absyn_XTunion_e_tag= 28;
struct Cyc_Absyn_XTunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_XTuniondecl* f3; struct Cyc_Absyn_Tunionfield* f4; } ;
const int Cyc_Absyn_Enum_e_tag= 29; struct Cyc_Absyn_Enum_e_struct{ int tag;
struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; const int Cyc_Absyn_Malloc_e_tag= 30; struct Cyc_Absyn_Malloc_e_struct{
int tag; void* f1; } ; const int Cyc_Absyn_UnresolvedMem_e_tag= 31; struct Cyc_Absyn_UnresolvedMem_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List* f2; } ; const int Cyc_Absyn_StmtExp_e_tag=
32; struct Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
const int Cyc_Absyn_Codegen_e_tag= 33; struct Cyc_Absyn_Codegen_e_struct{ int
tag; struct Cyc_Absyn_Fndecl* f1; } ; const int Cyc_Absyn_Fill_e_tag= 34; struct
Cyc_Absyn_Fill_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct Cyc_Absyn_Exp{
struct Cyc_Core_Opt* topt; void* r; struct Cyc_Position_Segment* loc; } ; const
unsigned int Cyc_Absyn_Skip_s= 0; const int Cyc_Absyn_Exp_s_tag= 0; struct Cyc_Absyn_Exp_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; const int Cyc_Absyn_Seq_s_tag= 1; struct
Cyc_Absyn_Seq_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt*
f2; } ; const int Cyc_Absyn_Return_s_tag= 2; struct Cyc_Absyn_Return_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; const int Cyc_Absyn_IfThenElse_s_tag= 3;
struct Cyc_Absyn_IfThenElse_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt* f3; } ; const int Cyc_Absyn_While_s_tag=
4; struct Cyc_Absyn_While_s_struct{ int tag; struct _tuple2 f1; struct Cyc_Absyn_Stmt*
f2; } ; const int Cyc_Absyn_Break_s_tag= 5; struct Cyc_Absyn_Break_s_struct{ int
tag; struct Cyc_Absyn_Stmt* f1; } ; const int Cyc_Absyn_Continue_s_tag= 6;
struct Cyc_Absyn_Continue_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ;
const int Cyc_Absyn_Goto_s_tag= 7; struct Cyc_Absyn_Goto_s_struct{ int tag;
struct _tagged_string* f1; struct Cyc_Absyn_Stmt* f2; } ; const int Cyc_Absyn_For_s_tag=
8; struct Cyc_Absyn_For_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
_tuple2 f2; struct _tuple2 f3; struct Cyc_Absyn_Stmt* f4; } ; const int Cyc_Absyn_Switch_s_tag=
9; struct Cyc_Absyn_Switch_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_List_List* f2; } ; const int Cyc_Absyn_Fallthru_s_tag= 10; struct Cyc_Absyn_Fallthru_s_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Absyn_Switch_clause** f2; } ;
const int Cyc_Absyn_Decl_s_tag= 11; struct Cyc_Absyn_Decl_s_struct{ int tag;
struct Cyc_Absyn_Decl* f1; struct Cyc_Absyn_Stmt* f2; } ; const int Cyc_Absyn_Cut_s_tag=
12; struct Cyc_Absyn_Cut_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; const
int Cyc_Absyn_Splice_s_tag= 13; struct Cyc_Absyn_Splice_s_struct{ int tag;
struct Cyc_Absyn_Stmt* f1; } ; const int Cyc_Absyn_Label_s_tag= 14; struct Cyc_Absyn_Label_s_struct{
int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt* f2; } ; const int Cyc_Absyn_Do_s_tag=
15; struct Cyc_Absyn_Do_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; struct
_tuple2 f2; } ; const int Cyc_Absyn_TryCatch_s_tag= 16; struct Cyc_Absyn_TryCatch_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_List_List* f2; } ; struct Cyc_Absyn_Stmt{
void* r; struct Cyc_Position_Segment* loc; struct Cyc_List_List* non_local_preds;
int try_depth; struct _xtunion_struct* annot; } ; const unsigned int Cyc_Absyn_Wild_p=
0; const int Cyc_Absyn_Var_p_tag= 0; struct Cyc_Absyn_Var_p_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; const unsigned int Cyc_Absyn_Null_p= 1; const
int Cyc_Absyn_Int_p_tag= 1; struct Cyc_Absyn_Int_p_struct{ int tag; void* f1;
int f2; } ; const int Cyc_Absyn_Char_p_tag= 2; struct Cyc_Absyn_Char_p_struct{
int tag; char f1; } ; const int Cyc_Absyn_Float_p_tag= 3; struct Cyc_Absyn_Float_p_struct{
int tag; struct _tagged_string f1; } ; const int Cyc_Absyn_Tuple_p_tag= 4;
struct Cyc_Absyn_Tuple_p_struct{ int tag; struct Cyc_List_List* f1; } ; const
int Cyc_Absyn_Pointer_p_tag= 5; struct Cyc_Absyn_Pointer_p_struct{ int tag;
struct Cyc_Absyn_Pat* f1; } ; const int Cyc_Absyn_Reference_p_tag= 6; struct Cyc_Absyn_Reference_p_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; const int Cyc_Absyn_Struct_p_tag= 7;
struct Cyc_Absyn_Struct_p_struct{ int tag; struct Cyc_Absyn_Structdecl* f1;
struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3; struct Cyc_List_List* f4; } ;
const int Cyc_Absyn_Tunion_p_tag= 8; struct Cyc_Absyn_Tunion_p_struct{ int tag;
struct _tuple0* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3; struct
Cyc_List_List* f4; struct Cyc_Absyn_Tuniondecl* f5; struct Cyc_Absyn_Tunionfield*
f6; } ; const int Cyc_Absyn_XTunion_p_tag= 9; struct Cyc_Absyn_XTunion_p_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3;
struct Cyc_Absyn_XTuniondecl* f4; struct Cyc_Absyn_Tunionfield* f5; } ; const
int Cyc_Absyn_Enum_p_tag= 10; struct Cyc_Absyn_Enum_p_struct{ int tag; struct
_tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield* f3; } ;
const int Cyc_Absyn_UnknownId_p_tag= 11; struct Cyc_Absyn_UnknownId_p_struct{
int tag; struct _tuple0* f1; } ; const int Cyc_Absyn_UnknownCall_p_tag= 12;
struct Cyc_Absyn_UnknownCall_p_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; const int Cyc_Absyn_UnknownFields_p_tag= 13;
struct Cyc_Absyn_UnknownFields_p_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; struct Cyc_Absyn_Pat{ void* r; struct Cyc_Core_Opt*
topt; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat* pattern; struct Cyc_Core_Opt* pat_vars; struct Cyc_Absyn_Exp*
where_clause; struct Cyc_Absyn_Stmt* body; struct Cyc_Position_Segment* loc; } ;
const unsigned int Cyc_Absyn_Unresolved_b= 0; const int Cyc_Absyn_Global_b_tag=
0; struct Cyc_Absyn_Global_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
const int Cyc_Absyn_Funname_b_tag= 1; struct Cyc_Absyn_Funname_b_struct{ int tag;
struct Cyc_Absyn_Fndecl* f1; } ; const int Cyc_Absyn_Param_b_tag= 2; struct Cyc_Absyn_Param_b_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; const int Cyc_Absyn_Local_b_tag= 3;
struct Cyc_Absyn_Local_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
const int Cyc_Absyn_Pat_b_tag= 4; struct Cyc_Absyn_Pat_b_struct{ int tag; struct
Cyc_Absyn_Vardecl* f1; } ; struct Cyc_Absyn_Vardecl{ void* sc; struct _tuple0*
name; struct Cyc_Absyn_Tqual* tq; void* type; struct Cyc_Absyn_Exp* initializer;
int shadow; struct Cyc_Core_Opt* region; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_Fndecl{ void* sc; int is_inline; struct _tuple0* name; struct
Cyc_List_List* tvs; struct Cyc_Core_Opt* effect; void* ret_type; struct Cyc_List_List*
args; int varargs; struct Cyc_Absyn_Stmt* body; struct Cyc_Core_Opt* cached_typ;
struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_Structfield{ struct _tagged_string* name; struct Cyc_Absyn_Tqual*
tq; void* type; struct Cyc_Core_Opt* width; struct Cyc_List_List* attributes; }
; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Tunionfield{ struct _tuple0* name; struct Cyc_List_List* tvs; struct
Cyc_List_List* typs; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Tuniondecl{
void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt*
fields; } ; struct Cyc_Absyn_XTuniondecl{ void* sc; struct _tuple0* name; struct
Cyc_List_List* fields; } ; struct Cyc_Absyn_Enumfield{ struct _tuple0* name;
struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Enumdecl{
void* sc; struct _tuple0* name; struct Cyc_List_List* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple0* name; struct Cyc_List_List* tvs; void* defn; } ; const int Cyc_Absyn_Var_d_tag=
0; struct Cyc_Absyn_Var_d_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
const int Cyc_Absyn_Fn_d_tag= 1; struct Cyc_Absyn_Fn_d_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; const int Cyc_Absyn_Let_d_tag= 2; struct Cyc_Absyn_Let_d_struct{
int tag; struct Cyc_Absyn_Pat* f1; struct Cyc_Core_Opt* f2; struct Cyc_Core_Opt*
f3; struct Cyc_Absyn_Exp* f4; int f5; } ; const int Cyc_Absyn_Struct_d_tag= 3;
struct Cyc_Absyn_Struct_d_struct{ int tag; struct Cyc_Absyn_Structdecl* f1; } ;
const int Cyc_Absyn_Union_d_tag= 4; struct Cyc_Absyn_Union_d_struct{ int tag;
struct Cyc_Absyn_Uniondecl* f1; } ; const int Cyc_Absyn_Tunion_d_tag= 5; struct
Cyc_Absyn_Tunion_d_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1; } ; const
int Cyc_Absyn_XTunion_d_tag= 6; struct Cyc_Absyn_XTunion_d_struct{ int tag;
struct Cyc_Absyn_XTuniondecl* f1; } ; const int Cyc_Absyn_Enum_d_tag= 7; struct
Cyc_Absyn_Enum_d_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; } ; const int
Cyc_Absyn_Typedef_d_tag= 8; struct Cyc_Absyn_Typedef_d_struct{ int tag; struct
Cyc_Absyn_Typedefdecl* f1; } ; const int Cyc_Absyn_Namespace_d_tag= 9; struct
Cyc_Absyn_Namespace_d_struct{ int tag; struct _tagged_string* f1; struct Cyc_List_List*
f2; } ; const int Cyc_Absyn_Using_d_tag= 10; struct Cyc_Absyn_Using_d_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; } ; const int Cyc_Absyn_ExternC_d_tag=
11; struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ;
struct Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; const int
Cyc_Absyn_ArrayElement_tag= 0; struct Cyc_Absyn_ArrayElement_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; const int Cyc_Absyn_FieldName_tag= 1; struct Cyc_Absyn_FieldName_struct{
int tag; struct _tagged_string* f1; } ; char Cyc_Absyn_EmptyAnnot_tag[ 11u]="EmptyAnnot";
struct Cyc_Absyn_EmptyAnnot_struct{ char* tag; } ; extern int Cyc_Absyn_qvar_cmp(
struct _tuple0*, struct _tuple0*); extern int Cyc_Absyn_varlist_cmp( struct Cyc_List_List*,
struct Cyc_List_List*); extern int Cyc_Absyn_tvar_cmp( struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*); extern struct Cyc_Absyn_Tqual* Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual* x, struct Cyc_Absyn_Tqual* y); extern struct Cyc_Absyn_Tqual*
Cyc_Absyn_empty_tqual(); extern struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref(
void* x); extern struct Cyc_Absyn_Conref* Cyc_Absyn_empty_conref(); extern
struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x);
extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); extern void* Cyc_Absyn_new_evar(
void*); extern void* Cyc_Absyn_wildtyp(); extern void* Cyc_Absyn_uchar_t; extern
void* Cyc_Absyn_ushort_t; extern void* Cyc_Absyn_uint_t; extern void* Cyc_Absyn_ulong_t;
extern void* Cyc_Absyn_schar_t; extern void* Cyc_Absyn_sshort_t; extern void*
Cyc_Absyn_sint_t; extern void* Cyc_Absyn_slong_t; extern void* Cyc_Absyn_float_t;
extern void* Cyc_Absyn_double_t; extern struct _tuple0* Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_XTuniondecl* Cyc_Absyn_exn_xed; extern void* Cyc_Absyn_exn_typ;
extern void* Cyc_Absyn_string_typ( void* rgn); extern void* Cyc_Absyn_file_typ();
extern void* Cyc_Absyn_starb_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual* tq,
void* b); extern void* Cyc_Absyn_atb_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual*
tq, void* b); extern void* Cyc_Absyn_star_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual*
tq); extern void* Cyc_Absyn_at_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual*
tq); extern void* Cyc_Absyn_cstar_typ( void* t, struct Cyc_Absyn_Tqual* tq);
extern void* Cyc_Absyn_tagged_typ( void* t, void* rgn, struct Cyc_Absyn_Tqual*
tq); extern void* Cyc_Absyn_void_star_typ(); extern void* Cyc_Absyn_strct(
struct _tagged_string* name); extern void* Cyc_Absyn_strctq( struct _tuple0*
name); extern void* Cyc_Absyn_unionq_typ( struct _tuple0* name); extern void*
Cyc_Absyn_union_typ( struct _tagged_string* name); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_new_exp( void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_New_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_const_exp( void*, struct Cyc_Position_Segment*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_null_exp( struct Cyc_Position_Segment*); extern
struct Cyc_Absyn_Exp* Cyc_Absyn_bool_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_false_exp( struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void*, int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_uint_exp( unsigned int, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_char_exp( char c, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp( struct _tagged_string f,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp(
struct _tagged_string s, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_var_exp( struct _tuple0*, struct Cyc_Position_Segment*); extern struct
Cyc_Absyn_Exp* Cyc_Absyn_varb_exp( struct _tuple0*, void*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_unknownid_exp( struct _tuple0*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_primop_exp( void*,
struct Cyc_List_List* es, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_prim1_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_prim2_exp( void*, struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_add_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_subtract_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_times_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_divide_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_eq_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_neq_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_lt_exp( struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_gte_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_lte_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Core_Opt*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_increment_exp(
struct Cyc_Absyn_Exp*, void*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_post_inc_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_post_dec_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_pre_dec_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_conditional_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_and_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_or_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_seq_exp( struct Cyc_Absyn_Exp*, struct
Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_unknowncall_exp(
struct Cyc_Absyn_Exp*, struct Cyc_List_List*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_fncall_exp( struct Cyc_Absyn_Exp*, struct
Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_throw_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_noinstantiate_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_instantiate_exp( struct Cyc_Absyn_Exp*,
struct Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp( void*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_address_exp( struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeoftyp_exp(
void* t, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_sizeofexp_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_deref_exp( struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp*,
struct _tagged_string*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_structarrow_exp( struct Cyc_Absyn_Exp*, struct _tagged_string*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*, struct Cyc_Absyn_Exp*, struct Cyc_Position_Segment*);
extern struct Cyc_Absyn_Exp* Cyc_Absyn_tuple_exp( struct Cyc_List_List*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_stmt_exp( struct
Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_null_pointer_exn_exp(
struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp* Cyc_Absyn_array_exp(
struct Cyc_List_List*, struct Cyc_Position_Segment*); extern struct Cyc_Absyn_Exp*
Cyc_Absyn_unresolvedmem_exp( struct Cyc_Core_Opt*, struct Cyc_List_List*, struct
Cyc_Position_Segment*); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_new_stmt( void*
s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt(
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmts(
struct Cyc_List_List*, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_return_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_ifthenelse_stmt( struct Cyc_Absyn_Exp* e,
struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_while_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_break_stmt( struct Cyc_Position_Segment* loc); extern
struct Cyc_Absyn_Stmt* Cyc_Absyn_continue_stmt( struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3, struct Cyc_Absyn_Stmt* s,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_switch_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_List_List*, struct Cyc_Position_Segment* loc);
extern struct Cyc_Absyn_Stmt* Cyc_Absyn_fallthru_stmt( struct Cyc_List_List* el,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_declare_stmt( struct _tuple0*,
void*, struct Cyc_Absyn_Exp* init, struct Cyc_Absyn_Stmt*, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_cut_stmt( struct Cyc_Absyn_Stmt* s,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_splice_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt( struct _tagged_string* v, struct Cyc_Absyn_Stmt* s, struct
Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_trycatch_stmt( struct Cyc_Absyn_Stmt*
s, struct Cyc_List_List* scs, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Stmt* Cyc_Absyn_goto_stmt( struct _tagged_string* lab, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Stmt* Cyc_Absyn_assign_stmt( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc); extern struct
Cyc_Absyn_Pat* Cyc_Absyn_new_pat( void* p, struct Cyc_Position_Segment* s);
extern struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl( void* r, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_let_decl( struct Cyc_Absyn_Pat* p,
struct Cyc_Core_Opt* t_opt, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple0* x,
void* t, struct Cyc_Absyn_Exp* init); extern struct Cyc_Absyn_Vardecl* Cyc_Absyn_static_vardecl(
struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init); extern struct Cyc_Absyn_Decl*
Cyc_Absyn_struct_decl( void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts,
struct Cyc_Core_Opt* fs, struct Cyc_List_List* atts, struct Cyc_Position_Segment*
loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_union_decl( void* s, struct Cyc_Core_Opt*
n, struct Cyc_List_List* ts, struct Cyc_Core_Opt* fs, struct Cyc_List_List* atts,
struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_tunion_decl(
void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts, struct Cyc_Core_Opt*
fs, struct Cyc_Position_Segment* loc); extern struct Cyc_Absyn_Decl* Cyc_Absyn_xtunion_decl(
void* s, struct _tuple0* n, struct Cyc_List_List* fs, struct Cyc_Position_Segment*
loc); extern int Cyc_Absyn_is_format_prim( void* p); extern void* Cyc_Absyn_function_typ(
struct Cyc_List_List* tvs, struct Cyc_Core_Opt* eff_typ, void* ret_typ, struct
Cyc_List_List* args, int varargs, struct Cyc_List_List*); extern void* Cyc_Absyn_pointer_expand(
void*); extern int Cyc_Absyn_is_lvalue( struct Cyc_Absyn_Exp*); extern struct
Cyc_Absyn_Structfield* Cyc_Absyn_lookup_field( struct Cyc_Core_Opt*, struct
_tagged_string*); extern struct Cyc_Absyn_Structfield* Cyc_Absyn_lookup_struct_field(
struct Cyc_Absyn_Structdecl*, struct _tagged_string*); extern struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_union_field( struct Cyc_Absyn_Uniondecl*, struct _tagged_string*);
extern struct _tuple3* Cyc_Absyn_lookup_tuple_field( struct Cyc_List_List*, int);
extern struct _tagged_string Cyc_Absyn_attribute2string( void*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef unsigned int Cyc_Stdio_size_t;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; extern int Cyc_String_zstrptrcmp( struct _tagged_string*, struct
_tagged_string*); struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t;
typedef struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t;
extern char Cyc_Set_Absent_tag[ 7u]; struct Cyc_Set_Absent_struct{ char* tag; }
; struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef
struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[ 8u];
struct Cyc_Dict_Present_struct{ char* tag; } ; extern char Cyc_Dict_Absent_tag[
7u]; typedef void* Cyc_Tcenv_Resolved; extern const int Cyc_Tcenv_VarRes_tag;
struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; extern const int Cyc_Tcenv_StructRes_tag;
struct Cyc_Tcenv_StructRes_struct{ int tag; struct Cyc_Absyn_Structdecl* f1; } ;
extern const int Cyc_Tcenv_TunionRes_tag; struct Cyc_Tcenv_TunionRes_struct{ int
tag; struct Cyc_Absyn_Tuniondecl* f1; struct Cyc_Absyn_Tunionfield* f2; } ;
extern const int Cyc_Tcenv_XTunionRes_tag; struct Cyc_Tcenv_XTunionRes_struct{
int tag; struct Cyc_Absyn_XTuniondecl* f1; struct Cyc_Absyn_Tunionfield* f2; } ;
extern const int Cyc_Tcenv_EnumRes_tag; struct Cyc_Tcenv_EnumRes_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ; typedef void*
Cyc_Tcenv_resolved_t; struct Cyc_Tcenv_Genv{ struct Cyc_Set_Set* namespaces;
struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict* uniondecls; struct Cyc_Dict_Dict*
tuniondecls; struct Cyc_Dict_Dict* xtuniondecls; struct Cyc_Dict_Dict* enumdecls;
struct Cyc_Dict_Dict* typedefs; struct Cyc_Dict_Dict* ordinaries; struct Cyc_List_List*
availables; } ; typedef struct Cyc_Tcenv_Genv* Cyc_Tcenv_genv_t; struct Cyc_Tcenv_Fenv;
typedef struct Cyc_Tcenv_Fenv* Cyc_Tcenv_fenv_t; typedef void* Cyc_Tcenv_Jumpee;
extern const unsigned int Cyc_Tcenv_NotLoop_j; extern const unsigned int Cyc_Tcenv_CaseEnd_j;
extern const unsigned int Cyc_Tcenv_FnEnd_j; extern const int Cyc_Tcenv_Stmt_j_tag;
struct Cyc_Tcenv_Stmt_j_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; typedef
void* Cyc_Tcenv_jumpee_t; typedef void* Cyc_Tcenv_Frames; extern const int Cyc_Tcenv_Outermost_tag;
struct Cyc_Tcenv_Outermost_struct{ int tag; void* f1; } ; extern const int Cyc_Tcenv_Frame_tag;
struct Cyc_Tcenv_Frame_struct{ int tag; void* f1; void* f2; } ; extern const int
Cyc_Tcenv_Hidden_tag; struct Cyc_Tcenv_Hidden_struct{ int tag; void* f1; void*
f2; } ; typedef void* Cyc_Tcenv_frames_t; struct Cyc_Tcenv_Tenv{ struct Cyc_List_List*
ns; struct Cyc_Dict_Dict* ae; struct Cyc_Core_Opt* le; } ; typedef struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tenv_t; extern char Cyc_Tcutil_TypeErr_tag[ 8u]; struct Cyc_Tcutil_TypeErr_struct{
char* tag; } ; extern void* Cyc_Tcutil_impos( struct _tagged_string); extern
void* Cyc_Tcutil_compress( void* t); static int Cyc_Absyn_zstrlist_cmp( struct
Cyc_List_List* ss1, struct Cyc_List_List* ss2){ return(( int(*)( int(* cmp)(
struct _tagged_string*, struct _tagged_string*), struct Cyc_List_List* l1,
struct Cyc_List_List* l2)) Cyc_List_list_cmp)( Cyc_String_zstrptrcmp, ss1, ss2);}
int Cyc_Absyn_varlist_cmp( struct Cyc_List_List* vs1, struct Cyc_List_List* vs2){
return Cyc_Absyn_zstrlist_cmp( vs1, vs2);} int Cyc_Absyn_qvar_cmp( struct
_tuple0* q1, struct _tuple0* q2){ void* n1=(* q1).f1; void* n2=(* q2).f1;{
struct _tuple4 _temp1=({ struct _tuple4 _temp0; _temp0.f1= n1; _temp0.f2= n2;
_temp0;}); void* _temp17; void* _temp19; void* _temp21; struct Cyc_List_List*
_temp23; void* _temp25; struct Cyc_List_List* _temp27; void* _temp29; struct Cyc_List_List*
_temp31; void* _temp33; struct Cyc_List_List* _temp35; void* _temp37; void*
_temp39; void* _temp41; void* _temp43; void* _temp45; void* _temp47; struct Cyc_List_List*
_temp49; void* _temp51; struct Cyc_List_List* _temp53; void* _temp55; _LL3:
_LL20: _temp19= _temp1.f1; if(( int) _temp19 == Cyc_Absyn_Loc_n){ goto _LL18;}
else{ goto _LL5;} _LL18: _temp17= _temp1.f2; if(( int) _temp17 == Cyc_Absyn_Loc_n){
goto _LL4;} else{ goto _LL5;} _LL5: _LL26: _temp25= _temp1.f1; if(( unsigned int)
_temp25 > 1u?(( struct _tunion_struct*) _temp25)->tag == Cyc_Absyn_Rel_n_tag: 0){
_LL28: _temp27=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp25)->f1; goto _LL22;} else{ goto _LL7;} _LL22: _temp21= _temp1.f2; if((
unsigned int) _temp21 > 1u?(( struct _tunion_struct*) _temp21)->tag == Cyc_Absyn_Rel_n_tag:
0){ _LL24: _temp23=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp21)->f1; goto _LL6;} else{ goto _LL7;} _LL7: _LL34: _temp33= _temp1.f1; if((
unsigned int) _temp33 > 1u?(( struct _tunion_struct*) _temp33)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL36: _temp35=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp33)->f1; goto _LL30;} else{ goto _LL9;} _LL30: _temp29= _temp1.f2; if((
unsigned int) _temp29 > 1u?(( struct _tunion_struct*) _temp29)->tag == Cyc_Absyn_Abs_n_tag:
0){ _LL32: _temp31=( struct Cyc_List_List*)(( struct Cyc_Absyn_Abs_n_struct*)
_temp29)->f1; goto _LL8;} else{ goto _LL9;} _LL9: _LL40: _temp39= _temp1.f1; if((
int) _temp39 == Cyc_Absyn_Loc_n){ goto _LL38;} else{ goto _LL11;} _LL38: _temp37=
_temp1.f2; goto _LL10; _LL11: _LL44: _temp43= _temp1.f1; goto _LL42; _LL42:
_temp41= _temp1.f2; if(( int) _temp41 == Cyc_Absyn_Loc_n){ goto _LL12;} else{
goto _LL13;} _LL13: _LL48: _temp47= _temp1.f1; if(( unsigned int) _temp47 > 1u?((
struct _tunion_struct*) _temp47)->tag == Cyc_Absyn_Rel_n_tag: 0){ _LL50: _temp49=(
struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*) _temp47)->f1; goto
_LL46;} else{ goto _LL15;} _LL46: _temp45= _temp1.f2; goto _LL14; _LL15: _LL56:
_temp55= _temp1.f1; goto _LL52; _LL52: _temp51= _temp1.f2; if(( unsigned int)
_temp51 > 1u?(( struct _tunion_struct*) _temp51)->tag == Cyc_Absyn_Rel_n_tag: 0){
_LL54: _temp53=( struct Cyc_List_List*)(( struct Cyc_Absyn_Rel_n_struct*)
_temp51)->f1; goto _LL16;} else{ goto _LL2;} _LL4: goto _LL2; _LL6: _temp35=
_temp27; _temp31= _temp23; goto _LL8; _LL8: { int i= Cyc_Absyn_zstrlist_cmp(
_temp35, _temp31); if( i != 0){ return i;} goto _LL2;} _LL10: return - 1; _LL12:
return 1; _LL14: return - 1; _LL16: return 1; _LL2:;} return Cyc_String_zstrptrcmp((*
q1).f2,(* q2).f2);} int Cyc_Absyn_tvar_cmp( struct Cyc_Absyn_Tvar* tv1, struct
Cyc_Absyn_Tvar* tv2){ return Cyc_String_zstrptrcmp( tv1->name, tv2->name);}
static int Cyc_Absyn_new_type_counter= 0; void* Cyc_Absyn_new_evar( void* k){
return( void*)({ struct Cyc_Absyn_Evar_struct* _temp57=( struct Cyc_Absyn_Evar_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Evar_struct)); _temp57[ 0]=({ struct Cyc_Absyn_Evar_struct
_temp58; _temp58.tag= Cyc_Absyn_Evar_tag; _temp58.f1=( void*) k; _temp58.f2= 0;
_temp58.f3=( Cyc_Absyn_new_type_counter ++); _temp58;}); _temp57;});} void* Cyc_Absyn_wildtyp(){
return Cyc_Absyn_new_evar(( void*) Cyc_Absyn_MemKind);} struct Cyc_Absyn_Tqual*
Cyc_Absyn_combine_tqual( struct Cyc_Absyn_Tqual* x, struct Cyc_Absyn_Tqual* y){
return({ struct Cyc_Absyn_Tqual* _temp59=( struct Cyc_Absyn_Tqual*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp59->q_const= x->q_const?
1: y->q_const; _temp59->q_volatile= x->q_volatile? 1: y->q_volatile; _temp59->q_restrict=
x->q_restrict? 1: y->q_restrict; _temp59;});} struct Cyc_Absyn_Tqual* Cyc_Absyn_empty_tqual(){
return({ struct Cyc_Absyn_Tqual* _temp60=( struct Cyc_Absyn_Tqual*)
GC_malloc_atomic( sizeof( struct Cyc_Absyn_Tqual)); _temp60->q_const= 0; _temp60->q_volatile=
0; _temp60->q_restrict= 0; _temp60;});} int Cyc_Absyn_is_format_prim( void* p){
void* _temp61= p; _LL63: if(( int) _temp61 == Cyc_Absyn_Printf){ goto _LL64;}
else{ goto _LL65;} _LL65: if(( int) _temp61 == Cyc_Absyn_Fprintf){ goto _LL66;}
else{ goto _LL67;} _LL67: if(( int) _temp61 == Cyc_Absyn_Xprintf){ goto _LL68;}
else{ goto _LL69;} _LL69: if(( int) _temp61 == Cyc_Absyn_Scanf){ goto _LL70;}
else{ goto _LL71;} _LL71: if(( int) _temp61 == Cyc_Absyn_Fscanf){ goto _LL72;}
else{ goto _LL73;} _LL73: if(( int) _temp61 == Cyc_Absyn_Sscanf){ goto _LL74;}
else{ goto _LL75;} _LL75: goto _LL76; _LL64: return 1; _LL66: return 1; _LL68:
return 1; _LL70: return 1; _LL72: return 1; _LL74: return 1; _LL76: return 0;
_LL62:;} struct Cyc_Absyn_Conref* Cyc_Absyn_new_conref( void* x){ return({
struct Cyc_Absyn_Conref* _temp77=( struct Cyc_Absyn_Conref*) GC_malloc( sizeof(
struct Cyc_Absyn_Conref)); _temp77->v=( void*)(( void*)({ struct Cyc_Absyn_Eq_constr_struct*
_temp78=( struct Cyc_Absyn_Eq_constr_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Eq_constr_struct));
_temp78[ 0]=({ struct Cyc_Absyn_Eq_constr_struct _temp79; _temp79.tag= Cyc_Absyn_Eq_constr_tag;
_temp79.f1=( void*) x; _temp79;}); _temp78;})); _temp77;});} struct Cyc_Absyn_Conref*
Cyc_Absyn_empty_conref(){ return({ struct Cyc_Absyn_Conref* _temp80=( struct Cyc_Absyn_Conref*)
GC_malloc( sizeof( struct Cyc_Absyn_Conref)); _temp80->v=( void*)(( void*) Cyc_Absyn_No_constr);
_temp80;});} static struct Cyc_Absyn_Eq_constr_struct Cyc_Absyn_true_constraint={
0u,( void*) 1}; static struct Cyc_Absyn_Eq_constr_struct Cyc_Absyn_false_constraint={
0u,( void*) 0}; struct Cyc_Absyn_Conref Cyc_Absyn_true_conref_v=( struct Cyc_Absyn_Conref){.v=(
void*)(( void*)& Cyc_Absyn_true_constraint)}; struct Cyc_Absyn_Conref Cyc_Absyn_false_conref_v=(
struct Cyc_Absyn_Conref){.v=( void*)(( void*)& Cyc_Absyn_false_constraint)};
struct Cyc_Absyn_Conref* Cyc_Absyn_true_conref=& Cyc_Absyn_true_conref_v; struct
Cyc_Absyn_Conref* Cyc_Absyn_false_conref=& Cyc_Absyn_false_conref_v; struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref* x){ void* _temp83=( void*) x->v;
void* _temp91; struct Cyc_Absyn_Conref* _temp93; struct Cyc_Absyn_Conref**
_temp95; _LL85: if(( int) _temp83 == Cyc_Absyn_No_constr){ goto _LL86;} else{
goto _LL87;} _LL87: if(( unsigned int) _temp83 > 1u?(( struct _tunion_struct*)
_temp83)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL92: _temp91=( void*)(( struct
Cyc_Absyn_Eq_constr_struct*) _temp83)->f1; goto _LL88;} else{ goto _LL89;} _LL89:
if(( unsigned int) _temp83 > 1u?(( struct _tunion_struct*) _temp83)->tag == Cyc_Absyn_Forward_constr_tag:
0){ _LL94: _temp93=( struct Cyc_Absyn_Conref*)(( struct Cyc_Absyn_Forward_constr_struct*)
_temp83)->f1; _temp95=&(( struct Cyc_Absyn_Forward_constr_struct*) _temp83)->f1;
goto _LL90;} else{ goto _LL84;} _LL86: return x; _LL88: return x; _LL90: {
struct Cyc_Absyn_Conref* z=(( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)(* _temp95);* _temp95= z; return z;} _LL84:;} void*
Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x){ void* _temp96=( void*)(((
struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_compress_conref)(
x))->v; void* _temp102; _LL98: if(( unsigned int) _temp96 > 1u?(( struct
_tunion_struct*) _temp96)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL103: _temp102=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp96)->f1; goto _LL99;} else{
goto _LL100;} _LL100: goto _LL101; _LL99: return _temp102; _LL101: return(( void*(*)(
struct _tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char*
_temp104=( char*)"conref_val"; struct _tagged_string _temp105; _temp105.curr=
_temp104; _temp105.base= _temp104; _temp105.last_plus_one= _temp104 + 11;
_temp105;})); _LL97:;} static struct Cyc_Absyn_IntType_struct Cyc_Absyn_uchar_tt={
7u,( void*)(( void*) 1u),( void*)(( void*) 0u)}; static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_ushort_tt={ 7u,( void*)(( void*) 1u),( void*)(( void*) 1u)}; static
struct Cyc_Absyn_IntType_struct Cyc_Absyn_uint_tt={ 7u,( void*)(( void*) 1u),(
void*)(( void*) 2u)}; static struct Cyc_Absyn_IntType_struct Cyc_Absyn_ulong_tt={
7u,( void*)(( void*) 1u),( void*)(( void*) 3u)}; void* Cyc_Absyn_uchar_t=( void*)&
Cyc_Absyn_uchar_tt; void* Cyc_Absyn_ushort_t=( void*)& Cyc_Absyn_ushort_tt; void*
Cyc_Absyn_uint_t=( void*)& Cyc_Absyn_uint_tt; void* Cyc_Absyn_ulong_t=( void*)&
Cyc_Absyn_ulong_tt; static struct Cyc_Absyn_IntType_struct Cyc_Absyn_schar_tt={
7u,( void*)(( void*) 0u),( void*)(( void*) 0u)}; static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_sshort_tt={ 7u,( void*)(( void*) 0u),( void*)(( void*) 1u)}; static
struct Cyc_Absyn_IntType_struct Cyc_Absyn_sint_tt={ 7u,( void*)(( void*) 0u),(
void*)(( void*) 2u)}; static struct Cyc_Absyn_IntType_struct Cyc_Absyn_slong_tt={
7u,( void*)(( void*) 0u),( void*)(( void*) 3u)}; void* Cyc_Absyn_schar_t=( void*)&
Cyc_Absyn_schar_tt; void* Cyc_Absyn_sshort_t=( void*)& Cyc_Absyn_sshort_tt; void*
Cyc_Absyn_sint_t=( void*)& Cyc_Absyn_sint_tt; void* Cyc_Absyn_slong_t=( void*)&
Cyc_Absyn_slong_tt; void* Cyc_Absyn_float_t=( void*) 1u; void* Cyc_Absyn_double_t=(
void*) 2u; static char _temp116[ 4u]="exn"; static struct _tagged_string Cyc_Absyn_exn_string=(
struct _tagged_string){ _temp116, _temp116, _temp116 + 4u}; static struct Cyc_Absyn_Abs_n_struct
Cyc_Absyn_abs_null={ 1u, 0}; static struct _tuple0 Cyc_Absyn_exn_name_v=( struct
_tuple0){.f1=( void*)& Cyc_Absyn_abs_null,.f2=& Cyc_Absyn_exn_string}; struct
_tuple0* Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v; static struct Cyc_Absyn_XTuniondecl
Cyc_Absyn_exn_xed_v=( struct Cyc_Absyn_XTuniondecl){.sc=( void*)(( void*) 3u),.name=&
Cyc_Absyn_exn_name_v,.fields= 0}; struct Cyc_Absyn_XTuniondecl* Cyc_Absyn_exn_xed=&
Cyc_Absyn_exn_xed_v; static struct Cyc_Absyn_XTunionType_struct Cyc_Absyn_exn_typ_tt={
3u,( struct Cyc_Absyn_XTunionInfo){.name=& Cyc_Absyn_exn_name_v,.rgn=( void*)((
void*) 3u),.xtud=( struct Cyc_Absyn_XTuniondecl*)& Cyc_Absyn_exn_xed_v}}; void*
Cyc_Absyn_exn_typ=( void*)& Cyc_Absyn_exn_typ_tt; static struct Cyc_Core_Opt*
Cyc_Absyn_string_t_opt= 0; void* Cyc_Absyn_string_typ( void* rgn){ if( Cyc_Absyn_string_t_opt
== 0){ void* t= Cyc_Absyn_starb_typ( Cyc_Absyn_uchar_t, rgn, Cyc_Absyn_empty_tqual(),(
void*) Cyc_Absyn_Unknown_b); Cyc_Absyn_string_t_opt=({ struct Cyc_Core_Opt*
_temp119=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp119->v=( void*) t; _temp119;});} return( void*) Cyc_Absyn_string_t_opt->v;}
void* Cyc_Absyn_starb_typ( void* t, void* r, struct Cyc_Absyn_Tqual* tq, void* b){
return( void*)({ struct Cyc_Absyn_PointerType_struct* _temp120=( struct Cyc_Absyn_PointerType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct)); _temp120[ 0]=({ struct
Cyc_Absyn_PointerType_struct _temp121; _temp121.tag= Cyc_Absyn_PointerType_tag;
_temp121.f1=({ struct Cyc_Absyn_PtrInfo _temp122; _temp122.elt_typ=( void*) t;
_temp122.rgn_typ=( void*) r; _temp122.nullable= Cyc_Absyn_true_conref; _temp122.tq=
tq; _temp122.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
b); _temp122;}); _temp121;}); _temp120;});} void* Cyc_Absyn_atb_typ( void* t,
void* r, struct Cyc_Absyn_Tqual* tq, void* b){ return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp123=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp123[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp124; _temp124.tag= Cyc_Absyn_PointerType_tag;
_temp124.f1=({ struct Cyc_Absyn_PtrInfo _temp125; _temp125.elt_typ=( void*) t;
_temp125.rgn_typ=( void*) r; _temp125.nullable= Cyc_Absyn_false_conref; _temp125.tq=
tq; _temp125.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)(
b); _temp125;}); _temp124;}); _temp123;});} void* Cyc_Absyn_star_typ( void* t,
void* r, struct Cyc_Absyn_Tqual* tq){ return Cyc_Absyn_starb_typ( t, r, tq,(
void*)({ struct Cyc_Absyn_Upper_b_struct* _temp126=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp126[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp127; _temp127.tag= Cyc_Absyn_Upper_b_tag; _temp127.f1= Cyc_Absyn_signed_int_exp(
1, 0); _temp127;}); _temp126;}));} void* Cyc_Absyn_cstar_typ( void* t, struct
Cyc_Absyn_Tqual* tq){ return Cyc_Absyn_starb_typ( t,( void*) Cyc_Absyn_HeapRgn,
tq,( void*)({ struct Cyc_Absyn_Upper_b_struct* _temp128=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp128[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp129; _temp129.tag= Cyc_Absyn_Upper_b_tag; _temp129.f1= Cyc_Absyn_signed_int_exp(
1, 0); _temp129;}); _temp128;}));} void* Cyc_Absyn_at_typ( void* t, void* r,
struct Cyc_Absyn_Tqual* tq){ return Cyc_Absyn_atb_typ( t, r, tq,( void*)({
struct Cyc_Absyn_Upper_b_struct* _temp130=( struct Cyc_Absyn_Upper_b_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Upper_b_struct)); _temp130[ 0]=({ struct Cyc_Absyn_Upper_b_struct
_temp131; _temp131.tag= Cyc_Absyn_Upper_b_tag; _temp131.f1= Cyc_Absyn_signed_int_exp(
1, 0); _temp131;}); _temp130;}));} void* Cyc_Absyn_tagged_typ( void* t, void* r,
struct Cyc_Absyn_Tqual* tq){ return( void*)({ struct Cyc_Absyn_PointerType_struct*
_temp132=( struct Cyc_Absyn_PointerType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_PointerType_struct));
_temp132[ 0]=({ struct Cyc_Absyn_PointerType_struct _temp133; _temp133.tag= Cyc_Absyn_PointerType_tag;
_temp133.f1=({ struct Cyc_Absyn_PtrInfo _temp134; _temp134.elt_typ=( void*) t;
_temp134.rgn_typ=( void*) r; _temp134.nullable= Cyc_Absyn_true_conref; _temp134.tq=
tq; _temp134.bounds=(( struct Cyc_Absyn_Conref*(*)( void* x)) Cyc_Absyn_new_conref)((
void*) Cyc_Absyn_Unknown_b); _temp134;}); _temp133;}); _temp132;});} static
struct Cyc_Core_Opt* Cyc_Absyn_file_t_opt= 0; static char _temp137[ 8u]="__sFILE";
static struct _tagged_string Cyc_Absyn_sf_str=( struct _tagged_string){ _temp137,
_temp137, _temp137 + 8u}; static struct _tagged_string* Cyc_Absyn_sf=& Cyc_Absyn_sf_str;
static char _temp140[ 4u]="Cyc"; static struct _tagged_string Cyc_Absyn_cyc_str=(
struct _tagged_string){ _temp140, _temp140, _temp140 + 4u}; static struct
_tagged_string* Cyc_Absyn_cyc=& Cyc_Absyn_cyc_str; static char _temp143[ 6u]="Stdio";
static struct _tagged_string Cyc_Absyn_st_str=( struct _tagged_string){ _temp143,
_temp143, _temp143 + 6u}; static struct _tagged_string* Cyc_Absyn_st=& Cyc_Absyn_st_str;
void* Cyc_Absyn_file_typ(){ if( Cyc_Absyn_file_t_opt == 0){ struct _tuple0*
file_t_name=({ struct _tuple0* _temp144=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0)); _temp144->f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp145=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp145[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp146; _temp146.tag= Cyc_Absyn_Abs_n_tag;
_temp146.f1=( struct Cyc_List_List*)({ struct Cyc_List_List* _temp147=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp147->hd=( void*)
Cyc_Absyn_cyc; _temp147->tl=({ struct Cyc_List_List* _temp148=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp148->hd=( void*) Cyc_Absyn_st;
_temp148->tl= 0; _temp148;}); _temp147;}); _temp146;}); _temp145;}); _temp144->f2=
Cyc_Absyn_sf; _temp144;}); struct Cyc_Absyn_Structdecl* sd=({ struct Cyc_Absyn_Structdecl*
_temp149=( struct Cyc_Absyn_Structdecl*) GC_malloc( sizeof( struct Cyc_Absyn_Structdecl));
_temp149->sc=( void*)(( void*) Cyc_Absyn_Abstract); _temp149->name=({ struct Cyc_Core_Opt*
_temp150=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp150->v=( void*) file_t_name; _temp150;}); _temp149->tvs= 0; _temp149->fields=
0; _temp149->attributes= 0; _temp149;}); void* file_struct_typ=( void*)({ struct
Cyc_Absyn_StructType_struct* _temp151=( struct Cyc_Absyn_StructType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct)); _temp151[ 0]=({ struct
Cyc_Absyn_StructType_struct _temp152; _temp152.tag= Cyc_Absyn_StructType_tag;
_temp152.f1=( struct _tuple0*) file_t_name; _temp152.f2= 0; _temp152.f3=({
struct Cyc_Absyn_Structdecl** _temp153=( struct Cyc_Absyn_Structdecl**)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl*) * 1); _temp153[ 0]= sd;
_temp153;}); _temp152;}); _temp151;}); Cyc_Absyn_file_t_opt=({ struct Cyc_Core_Opt*
_temp154=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp154->v=( void*) Cyc_Absyn_at_typ( file_struct_typ,( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); _temp154;});} return( void*) Cyc_Absyn_file_t_opt->v;}
static struct Cyc_Core_Opt* Cyc_Absyn_void_star_t_opt= 0; void* Cyc_Absyn_void_star_typ(){
if( Cyc_Absyn_void_star_t_opt == 0){ Cyc_Absyn_void_star_t_opt=({ struct Cyc_Core_Opt*
_temp155=( struct Cyc_Core_Opt*) GC_malloc( sizeof( struct Cyc_Core_Opt));
_temp155->v=( void*) Cyc_Absyn_star_typ(( void*) Cyc_Absyn_VoidType,( void*) Cyc_Absyn_HeapRgn,
Cyc_Absyn_empty_tqual()); _temp155;});} return( void*) Cyc_Absyn_void_star_t_opt->v;}
void* Cyc_Absyn_strct( struct _tagged_string* name){ return( void*)({ struct Cyc_Absyn_StructType_struct*
_temp156=( struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp156[ 0]=({ struct Cyc_Absyn_StructType_struct _temp157; _temp157.tag= Cyc_Absyn_StructType_tag;
_temp157.f1=({ struct _tuple0* _temp158=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0) * 1); _temp158[ 0]=({ struct _tuple0 _temp159; _temp159.f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp160=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp160[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp161; _temp161.tag= Cyc_Absyn_Abs_n_tag; _temp161.f1= 0; _temp161;});
_temp160;}); _temp159.f2= name; _temp159;}); _temp158;}); _temp157.f2= 0;
_temp157.f3= 0; _temp157;}); _temp156;});} void* Cyc_Absyn_union_typ( struct
_tagged_string* name){ return( void*)({ struct Cyc_Absyn_UnionType_struct*
_temp162=( struct Cyc_Absyn_UnionType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct));
_temp162[ 0]=({ struct Cyc_Absyn_UnionType_struct _temp163; _temp163.tag= Cyc_Absyn_UnionType_tag;
_temp163.f1=({ struct _tuple0* _temp164=( struct _tuple0*) GC_malloc( sizeof(
struct _tuple0) * 1); _temp164[ 0]=({ struct _tuple0 _temp165; _temp165.f1=(
void*)({ struct Cyc_Absyn_Abs_n_struct* _temp166=( struct Cyc_Absyn_Abs_n_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct)); _temp166[ 0]=({ struct Cyc_Absyn_Abs_n_struct
_temp167; _temp167.tag= Cyc_Absyn_Abs_n_tag; _temp167.f1= 0; _temp167;});
_temp166;}); _temp165.f2= name; _temp165;}); _temp164;}); _temp163.f2= 0;
_temp163.f3= 0; _temp163;}); _temp162;});} void* Cyc_Absyn_strctq( struct
_tuple0* name){ return( void*)({ struct Cyc_Absyn_StructType_struct* _temp168=(
struct Cyc_Absyn_StructType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_StructType_struct));
_temp168[ 0]=({ struct Cyc_Absyn_StructType_struct _temp169; _temp169.tag= Cyc_Absyn_StructType_tag;
_temp169.f1=( struct _tuple0*) name; _temp169.f2= 0; _temp169.f3= 0; _temp169;});
_temp168;});} void* Cyc_Absyn_unionq_typ( struct _tuple0* name){ return( void*)({
struct Cyc_Absyn_UnionType_struct* _temp170=( struct Cyc_Absyn_UnionType_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnionType_struct)); _temp170[ 0]=({ struct
Cyc_Absyn_UnionType_struct _temp171; _temp171.tag= Cyc_Absyn_UnionType_tag;
_temp171.f1=( struct _tuple0*) name; _temp171.f2= 0; _temp171.f3= 0; _temp171;});
_temp170;});} struct Cyc_Absyn_Exp* Cyc_Absyn_new_exp( void* r, struct Cyc_Position_Segment*
loc){ return({ struct Cyc_Absyn_Exp* _temp172=( struct Cyc_Absyn_Exp*) GC_malloc(
sizeof( struct Cyc_Absyn_Exp)); _temp172->topt= 0; _temp172->r=( void*) r;
_temp172->loc= loc; _temp172;});} struct Cyc_Absyn_Exp* Cyc_Absyn_New_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_New_e_struct* _temp173=( struct Cyc_Absyn_New_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_New_e_struct)); _temp173[ 0]=({ struct Cyc_Absyn_New_e_struct
_temp174; _temp174.tag= Cyc_Absyn_New_e_tag; _temp174.f1= e; _temp174;});
_temp173;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_copy_exp( struct Cyc_Absyn_Exp*
e){ return({ struct Cyc_Absyn_Exp* _temp175=( struct Cyc_Absyn_Exp*) GC_malloc(
sizeof( struct Cyc_Absyn_Exp)); _temp175->topt= e->topt; _temp175->r=( void*)((
void*) e->r); _temp175->loc= e->loc; _temp175;});} struct Cyc_Absyn_Exp* Cyc_Absyn_const_exp(
void* c, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Const_e_struct* _temp176=( struct Cyc_Absyn_Const_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct)); _temp176[ 0]=({ struct Cyc_Absyn_Const_e_struct
_temp177; _temp177.tag= Cyc_Absyn_Const_e_tag; _temp177.f1=( void*) c; _temp177;});
_temp176;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_null_exp( struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Const_e_struct*
_temp178=( struct Cyc_Absyn_Const_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Const_e_struct));
_temp178[ 0]=({ struct Cyc_Absyn_Const_e_struct _temp179; _temp179.tag= Cyc_Absyn_Const_e_tag;
_temp179.f1=( void*)(( void*) Cyc_Absyn_Null_c); _temp179;}); _temp178;}), loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_int_exp( void* s, int i, struct Cyc_Position_Segment*
seg){ return Cyc_Absyn_const_exp(( void*)({ struct Cyc_Absyn_Int_c_struct*
_temp180=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp180[ 0]=({ struct Cyc_Absyn_Int_c_struct _temp181; _temp181.tag= Cyc_Absyn_Int_c_tag;
_temp181.f1=( void*) s; _temp181.f2= i; _temp181;}); _temp180;}), seg);} struct
Cyc_Absyn_Exp* Cyc_Absyn_signed_int_exp( int i, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Signed, i, loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_uint_exp( unsigned int i, struct Cyc_Position_Segment* loc){ return
Cyc_Absyn_int_exp(( void*) Cyc_Absyn_Unsigned,( int) i, loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_bool_exp( int b, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_signed_int_exp(
b? 1: 0, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_true_exp( struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_bool_exp( 1, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_false_exp(
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_bool_exp( 0, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_char_exp( char c, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_const_exp(( void*)({ struct Cyc_Absyn_Char_c_struct* _temp182=(
struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp182[ 0]=({ struct Cyc_Absyn_Char_c_struct _temp183; _temp183.tag= Cyc_Absyn_Char_c_tag;
_temp183.f1=( void*)(( void*) Cyc_Absyn_Unsigned); _temp183.f2= c; _temp183;});
_temp182;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_float_exp( struct
_tagged_string f, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_const_exp((
void*)({ struct Cyc_Absyn_Float_c_struct* _temp184=( struct Cyc_Absyn_Float_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Float_c_struct)); _temp184[ 0]=({ struct Cyc_Absyn_Float_c_struct
_temp185; _temp185.tag= Cyc_Absyn_Float_c_tag; _temp185.f1= f; _temp185;});
_temp184;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_string_exp( struct
_tagged_string s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_const_exp((
void*)({ struct Cyc_Absyn_String_c_struct* _temp186=( struct Cyc_Absyn_String_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_String_c_struct)); _temp186[ 0]=({ struct
Cyc_Absyn_String_c_struct _temp187; _temp187.tag= Cyc_Absyn_String_c_tag;
_temp187.f1= s; _temp187;}); _temp186;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_var_exp(
struct _tuple0* q, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Var_e_struct* _temp188=( struct Cyc_Absyn_Var_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct)); _temp188[ 0]=({ struct Cyc_Absyn_Var_e_struct
_temp189; _temp189.tag= Cyc_Absyn_Var_e_tag; _temp189.f1= q; _temp189.f2=( void*)((
void*) Cyc_Absyn_Unresolved_b); _temp189;}); _temp188;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_varb_exp( struct _tuple0* q, void* b, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Var_e_struct* _temp190=(
struct Cyc_Absyn_Var_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_e_struct));
_temp190[ 0]=({ struct Cyc_Absyn_Var_e_struct _temp191; _temp191.tag= Cyc_Absyn_Var_e_tag;
_temp191.f1= q; _temp191.f2=( void*) b; _temp191;}); _temp190;}), loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_unknownid_exp( struct _tuple0* q, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnknownId_e_struct*
_temp192=( struct Cyc_Absyn_UnknownId_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_UnknownId_e_struct));
_temp192[ 0]=({ struct Cyc_Absyn_UnknownId_e_struct _temp193; _temp193.tag= Cyc_Absyn_UnknownId_e_tag;
_temp193.f1= q; _temp193;}); _temp192;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_primop_exp(
void* p, struct Cyc_List_List* es, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Primop_e_struct* _temp194=( struct Cyc_Absyn_Primop_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Primop_e_struct)); _temp194[ 0]=({ struct
Cyc_Absyn_Primop_e_struct _temp195; _temp195.tag= Cyc_Absyn_Primop_e_tag;
_temp195.f1=( void*) p; _temp195.f2= es; _temp195;}); _temp194;}), loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_prim1_exp( void* p, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_primop_exp( p,({ struct Cyc_List_List* _temp196=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp196->hd=( void*)
e; _temp196->tl= 0; _temp196;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_prim2_exp(
void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_primop_exp( p,({ struct Cyc_List_List* _temp197=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp197->hd=( void*)
e1; _temp197->tl=({ struct Cyc_List_List* _temp198=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp198->hd=( void*) e2; _temp198->tl=
0; _temp198;}); _temp197;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_add_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Plus, e1, e2, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_subtract_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Minus,
e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_times_exp( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Times, e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Div, e1, e2, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_eq_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Eq,
e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_neq_exp( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Neq, e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Gt, e1, e2, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_lt_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Lt,
e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_gte_exp( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_prim2_exp((
void*) Cyc_Absyn_Gte, e1, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_prim2_exp(( void*) Cyc_Absyn_Lte, e1, e2, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_assignop_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Core_Opt*
popt, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_AssignOp_e_struct* _temp199=( struct Cyc_Absyn_AssignOp_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_AssignOp_e_struct)); _temp199[ 0]=({ struct
Cyc_Absyn_AssignOp_e_struct _temp200; _temp200.tag= Cyc_Absyn_AssignOp_e_tag;
_temp200.f1= e1; _temp200.f2= popt; _temp200.f3= e2; _temp200;}); _temp199;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_assign_exp( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_assignop_exp(
e1, 0, e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_increment_exp( struct Cyc_Absyn_Exp*
e, void* i, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Increment_e_struct* _temp201=( struct Cyc_Absyn_Increment_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Increment_e_struct)); _temp201[ 0]=({ struct
Cyc_Absyn_Increment_e_struct _temp202; _temp202.tag= Cyc_Absyn_Increment_e_tag;
_temp202.f1= e; _temp202.f2=( void*) i; _temp202;}); _temp201;}), loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_post_inc_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_increment_exp( e,( void*) Cyc_Absyn_PostInc, loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_pre_inc_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_increment_exp( e,( void*) Cyc_Absyn_PreInc, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_pre_dec_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_increment_exp( e,( void*) Cyc_Absyn_PreDec, loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_post_dec_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_increment_exp( e,( void*) Cyc_Absyn_PostDec, loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_conditional_exp( struct Cyc_Absyn_Exp* e1,
struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Conditional_e_struct*
_temp203=( struct Cyc_Absyn_Conditional_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Conditional_e_struct));
_temp203[ 0]=({ struct Cyc_Absyn_Conditional_e_struct _temp204; _temp204.tag=
Cyc_Absyn_Conditional_e_tag; _temp204.f1= e1; _temp204.f2= e2; _temp204.f3= e3;
_temp204;}); _temp203;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_and_exp( struct
Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_conditional_exp( e1, e2, Cyc_Absyn_false_exp( loc), loc);}
struct Cyc_Absyn_Exp* Cyc_Absyn_or_exp( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp*
e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_conditional_exp( e1, Cyc_Absyn_true_exp(
loc), e2, loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_seq_exp( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_SeqExp_e_struct* _temp205=( struct Cyc_Absyn_SeqExp_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_SeqExp_e_struct)); _temp205[ 0]=({ struct
Cyc_Absyn_SeqExp_e_struct _temp206; _temp206.tag= Cyc_Absyn_SeqExp_e_tag;
_temp206.f1= e1; _temp206.f2= e2; _temp206;}); _temp205;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp( struct Cyc_Absyn_Exp* e, struct Cyc_List_List* es,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(( void*)({ struct
Cyc_Absyn_UnknownCall_e_struct* _temp207=( struct Cyc_Absyn_UnknownCall_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_UnknownCall_e_struct)); _temp207[ 0]=({
struct Cyc_Absyn_UnknownCall_e_struct _temp208; _temp208.tag= Cyc_Absyn_UnknownCall_e_tag;
_temp208.f1= e; _temp208.f2= es; _temp208;}); _temp207;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_fncall_exp( struct Cyc_Absyn_Exp* e, struct Cyc_List_List* es, struct
Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_FnCall_e_struct*
_temp209=( struct Cyc_Absyn_FnCall_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnCall_e_struct));
_temp209[ 0]=({ struct Cyc_Absyn_FnCall_e_struct _temp210; _temp210.tag= Cyc_Absyn_FnCall_e_tag;
_temp210.f1= e; _temp210.f2= es; _temp210;}); _temp209;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_noinstantiate_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_NoInstantiate_e_struct*
_temp211=( struct Cyc_Absyn_NoInstantiate_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_NoInstantiate_e_struct)); _temp211[ 0]=({ struct Cyc_Absyn_NoInstantiate_e_struct
_temp212; _temp212.tag= Cyc_Absyn_NoInstantiate_e_tag; _temp212.f1= e; _temp212;});
_temp211;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_instantiate_exp( struct Cyc_Absyn_Exp*
e, struct Cyc_List_List* ts, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Instantiate_e_struct* _temp213=( struct Cyc_Absyn_Instantiate_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Instantiate_e_struct)); _temp213[ 0]=({
struct Cyc_Absyn_Instantiate_e_struct _temp214; _temp214.tag= Cyc_Absyn_Instantiate_e_tag;
_temp214.f1= e; _temp214.f2= ts; _temp214;}); _temp213;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp( void* t, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Cast_e_struct*
_temp215=( struct Cyc_Absyn_Cast_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cast_e_struct));
_temp215[ 0]=({ struct Cyc_Absyn_Cast_e_struct _temp216; _temp216.tag= Cyc_Absyn_Cast_e_tag;
_temp216.f1=( void*) t; _temp216.f2= e; _temp216;}); _temp215;}), loc);} struct
Cyc_Absyn_Exp* Cyc_Absyn_throw_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Throw_e_struct*
_temp217=( struct Cyc_Absyn_Throw_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Throw_e_struct));
_temp217[ 0]=({ struct Cyc_Absyn_Throw_e_struct _temp218; _temp218.tag= Cyc_Absyn_Throw_e_tag;
_temp218.f1= e; _temp218;}); _temp217;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Address_e_struct* _temp219=( struct Cyc_Absyn_Address_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Address_e_struct)); _temp219[ 0]=({ struct
Cyc_Absyn_Address_e_struct _temp220; _temp220.tag= Cyc_Absyn_Address_e_tag;
_temp220.f1= e; _temp220;}); _temp219;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_sizeoftyp_exp(
void* t, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(( void*)({
struct Cyc_Absyn_Sizeoftyp_e_struct* _temp221=( struct Cyc_Absyn_Sizeoftyp_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Sizeoftyp_e_struct)); _temp221[ 0]=({ struct
Cyc_Absyn_Sizeoftyp_e_struct _temp222; _temp222.tag= Cyc_Absyn_Sizeoftyp_e_tag;
_temp222.f1=( void*) t; _temp222;}); _temp221;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_sizeofexp_exp( struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Sizeofexp_e_struct*
_temp223=( struct Cyc_Absyn_Sizeofexp_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Sizeofexp_e_struct));
_temp223[ 0]=({ struct Cyc_Absyn_Sizeofexp_e_struct _temp224; _temp224.tag= Cyc_Absyn_Sizeofexp_e_tag;
_temp224.f1= e; _temp224;}); _temp223;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Deref_e_struct* _temp225=( struct Cyc_Absyn_Deref_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Deref_e_struct)); _temp225[ 0]=({ struct Cyc_Absyn_Deref_e_struct
_temp226; _temp226.tag= Cyc_Absyn_Deref_e_tag; _temp226.f1= e; _temp226;});
_temp225;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_structmember_exp( struct Cyc_Absyn_Exp*
e, struct _tagged_string* n, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_StructMember_e_struct* _temp227=( struct Cyc_Absyn_StructMember_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructMember_e_struct)); _temp227[ 0]=({
struct Cyc_Absyn_StructMember_e_struct _temp228; _temp228.tag= Cyc_Absyn_StructMember_e_tag;
_temp228.f1= e; _temp228.f2= n; _temp228;}); _temp227;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_structarrow_exp( struct Cyc_Absyn_Exp* e, struct _tagged_string* n,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp(( void*)({ struct
Cyc_Absyn_StructArrow_e_struct* _temp229=( struct Cyc_Absyn_StructArrow_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StructArrow_e_struct)); _temp229[ 0]=({
struct Cyc_Absyn_StructArrow_e_struct _temp230; _temp230.tag= Cyc_Absyn_StructArrow_e_tag;
_temp230.f1= e; _temp230.f2= n; _temp230;}); _temp229;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_arrow_exp( struct Cyc_Absyn_Exp* e, struct _tagged_string* n, struct
Cyc_Position_Segment* loc){ return Cyc_Absyn_deref_exp( Cyc_Absyn_structmember_exp(
e, n, loc), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_subscript_exp( struct Cyc_Absyn_Exp*
e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_Subscript_e_struct* _temp231=( struct Cyc_Absyn_Subscript_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Subscript_e_struct)); _temp231[ 0]=({ struct
Cyc_Absyn_Subscript_e_struct _temp232; _temp232.tag= Cyc_Absyn_Subscript_e_tag;
_temp232.f1= e1; _temp232.f2= e2; _temp232;}); _temp231;}), loc);} struct Cyc_Absyn_Exp*
Cyc_Absyn_tuple_exp( struct Cyc_List_List* es, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Tuple_e_struct* _temp233=(
struct Cyc_Absyn_Tuple_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tuple_e_struct));
_temp233[ 0]=({ struct Cyc_Absyn_Tuple_e_struct _temp234; _temp234.tag= Cyc_Absyn_Tuple_e_tag;
_temp234.f1= es; _temp234;}); _temp233;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_exp((
void*)({ struct Cyc_Absyn_StmtExp_e_struct* _temp235=( struct Cyc_Absyn_StmtExp_e_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_StmtExp_e_struct)); _temp235[ 0]=({ struct
Cyc_Absyn_StmtExp_e_struct _temp236; _temp236.tag= Cyc_Absyn_StmtExp_e_tag;
_temp236.f1= s; _temp236;}); _temp235;}), loc);} static char _temp239[ 15u]="Null_Exception";
static struct _tagged_string Cyc_Absyn_ne_str=( struct _tagged_string){ _temp239,
_temp239, _temp239 + 15u}; static struct _tagged_string* Cyc_Absyn_ne=& Cyc_Absyn_ne_str;
struct Cyc_Absyn_Exp* Cyc_Absyn_null_pointer_exn_exp( struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_var_exp(({ struct _tuple0* _temp240=( struct _tuple0*)
GC_malloc( sizeof( struct _tuple0)); _temp240->f1=( void*)({ struct Cyc_Absyn_Abs_n_struct*
_temp241=( struct Cyc_Absyn_Abs_n_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Abs_n_struct));
_temp241[ 0]=({ struct Cyc_Absyn_Abs_n_struct _temp242; _temp242.tag= Cyc_Absyn_Abs_n_tag;
_temp242.f1= 0; _temp242;}); _temp241;}); _temp240->f2= Cyc_Absyn_ne; _temp240;}),
loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_array_exp( struct Cyc_List_List* es,
struct Cyc_Position_Segment* loc){ struct Cyc_List_List* dles= 0; for( 0; es !=
0; es= es->tl){ dles=({ struct Cyc_List_List* _temp243=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp243->hd=( void*)({ struct
_tuple5* _temp244=( struct _tuple5*) GC_malloc( sizeof( struct _tuple5));
_temp244->f1= 0; _temp244->f2=( struct Cyc_Absyn_Exp*) es->hd; _temp244;});
_temp243->tl= dles; _temp243;});} dles=(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x)) Cyc_List_imp_rev)( dles); return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_Array_e_struct*
_temp245=( struct Cyc_Absyn_Array_e_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Array_e_struct));
_temp245[ 0]=({ struct Cyc_Absyn_Array_e_struct _temp246; _temp246.tag= Cyc_Absyn_Array_e_tag;
_temp246.f1= dles; _temp246;}); _temp245;}), loc);} struct Cyc_Absyn_Exp* Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt* n, struct Cyc_List_List* dles, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_exp(( void*)({ struct Cyc_Absyn_UnresolvedMem_e_struct*
_temp247=( struct Cyc_Absyn_UnresolvedMem_e_struct*) GC_malloc( sizeof( struct
Cyc_Absyn_UnresolvedMem_e_struct)); _temp247[ 0]=({ struct Cyc_Absyn_UnresolvedMem_e_struct
_temp248; _temp248.tag= Cyc_Absyn_UnresolvedMem_e_tag; _temp248.f1= n; _temp248.f2=
dles; _temp248;}); _temp247;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_new_stmt(
void* s, struct Cyc_Position_Segment* loc){ return({ struct Cyc_Absyn_Stmt*
_temp249=( struct Cyc_Absyn_Stmt*) GC_malloc( sizeof( struct Cyc_Absyn_Stmt));
_temp249->r=( void*) s; _temp249->loc= loc; _temp249->non_local_preds= 0;
_temp249->try_depth= 0; _temp249->annot=( struct _xtunion_struct*)({ struct Cyc_Absyn_EmptyAnnot_struct*
_temp250=( struct Cyc_Absyn_EmptyAnnot_struct*) GC_malloc( sizeof( struct Cyc_Absyn_EmptyAnnot_struct));
_temp250[ 0]=({ struct Cyc_Absyn_EmptyAnnot_struct _temp251; _temp251.tag= Cyc_Absyn_EmptyAnnot_tag;
_temp251;}); _temp250;}); _temp249;});} struct Cyc_Absyn_Stmt* Cyc_Absyn_skip_stmt(
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*) Cyc_Absyn_Skip_s,
loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_exp_stmt( struct Cyc_Absyn_Exp* e,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({ struct
Cyc_Absyn_Exp_s_struct* _temp252=( struct Cyc_Absyn_Exp_s_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Exp_s_struct)); _temp252[ 0]=({ struct Cyc_Absyn_Exp_s_struct
_temp253; _temp253.tag= Cyc_Absyn_Exp_s_tag; _temp253.f1= e; _temp253;});
_temp252;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_seq_stmts( struct Cyc_List_List*
ss, struct Cyc_Position_Segment* loc){ if( ss == 0){ return Cyc_Absyn_skip_stmt(
loc);} else{ if( ss->tl == 0){ return( struct Cyc_Absyn_Stmt*) ss->hd;} else{
return Cyc_Absyn_seq_stmt(( struct Cyc_Absyn_Stmt*) ss->hd, Cyc_Absyn_seq_stmts(
ss->tl, loc), loc);}}} struct Cyc_Absyn_Stmt* Cyc_Absyn_return_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_Return_s_struct* _temp254=( struct Cyc_Absyn_Return_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Return_s_struct)); _temp254[ 0]=({ struct
Cyc_Absyn_Return_s_struct _temp255; _temp255.tag= Cyc_Absyn_Return_s_tag;
_temp255.f1= e; _temp255;}); _temp254;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({ struct
Cyc_Absyn_IfThenElse_s_struct* _temp256=( struct Cyc_Absyn_IfThenElse_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_IfThenElse_s_struct)); _temp256[ 0]=({
struct Cyc_Absyn_IfThenElse_s_struct _temp257; _temp257.tag= Cyc_Absyn_IfThenElse_s_tag;
_temp257.f1= e; _temp257.f2= s1; _temp257.f3= s2; _temp257;}); _temp256;}), loc);}
struct Cyc_Absyn_Stmt* Cyc_Absyn_while_stmt( struct Cyc_Absyn_Exp* e, struct Cyc_Absyn_Stmt*
s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({
struct Cyc_Absyn_While_s_struct* _temp258=( struct Cyc_Absyn_While_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_While_s_struct)); _temp258[ 0]=({ struct Cyc_Absyn_While_s_struct
_temp259; _temp259.tag= Cyc_Absyn_While_s_tag; _temp259.f1=({ struct _tuple2
_temp260; _temp260.f1= e; _temp260.f2= Cyc_Absyn_skip_stmt( e->loc); _temp260;});
_temp259.f2= s; _temp259;}); _temp258;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({ struct
Cyc_Absyn_Break_s_struct* _temp261=( struct Cyc_Absyn_Break_s_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Break_s_struct)); _temp261[ 0]=({ struct Cyc_Absyn_Break_s_struct
_temp262; _temp262.tag= Cyc_Absyn_Break_s_tag; _temp262.f1= 0; _temp262;});
_temp261;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_continue_stmt( struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Continue_s_struct*
_temp263=( struct Cyc_Absyn_Continue_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Continue_s_struct));
_temp263[ 0]=({ struct Cyc_Absyn_Continue_s_struct _temp264; _temp264.tag= Cyc_Absyn_Continue_s_tag;
_temp264.f1= 0; _temp264;}); _temp263;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_for_stmt(
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2, struct Cyc_Absyn_Exp* e3,
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_For_s_struct* _temp265=( struct Cyc_Absyn_For_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_For_s_struct)); _temp265[ 0]=({ struct Cyc_Absyn_For_s_struct
_temp266; _temp266.tag= Cyc_Absyn_For_s_tag; _temp266.f1= e1; _temp266.f2=({
struct _tuple2 _temp267; _temp267.f1= e2; _temp267.f2= Cyc_Absyn_skip_stmt( e3->loc);
_temp267;}); _temp266.f3=({ struct _tuple2 _temp268; _temp268.f1= e3; _temp268.f2=
Cyc_Absyn_skip_stmt( e3->loc); _temp268;}); _temp266.f4= s; _temp266;});
_temp265;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_switch_stmt( struct Cyc_Absyn_Exp*
e, struct Cyc_List_List* scs, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Switch_s_struct* _temp269=( struct Cyc_Absyn_Switch_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Switch_s_struct)); _temp269[ 0]=({ struct
Cyc_Absyn_Switch_s_struct _temp270; _temp270.tag= Cyc_Absyn_Switch_s_tag;
_temp270.f1= e; _temp270.f2= scs; _temp270;}); _temp269;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_seq_stmt( struct Cyc_Absyn_Stmt* s1, struct Cyc_Absyn_Stmt* s2, struct
Cyc_Position_Segment* loc){ void* _temp271=( void*) s1->r; _LL273: if(( int)
_temp271 == Cyc_Absyn_Skip_s){ goto _LL274;} else{ goto _LL275;} _LL275: goto
_LL276; _LL274: return s2; _LL276: return Cyc_Absyn_new_stmt(( void*)({ struct
Cyc_Absyn_Seq_s_struct* _temp277=( struct Cyc_Absyn_Seq_s_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Seq_s_struct)); _temp277[ 0]=({ struct Cyc_Absyn_Seq_s_struct
_temp278; _temp278.tag= Cyc_Absyn_Seq_s_tag; _temp278.f1= s1; _temp278.f2= s2;
_temp278;}); _temp277;}), loc); _LL272:;} struct Cyc_Absyn_Stmt* Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List* el, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Fallthru_s_struct* _temp279=( struct Cyc_Absyn_Fallthru_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Fallthru_s_struct)); _temp279[ 0]=({ struct
Cyc_Absyn_Fallthru_s_struct _temp280; _temp280.tag= Cyc_Absyn_Fallthru_s_tag;
_temp280.f1= el; _temp280.f2= 0; _temp280;}); _temp279;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_decl_stmt( struct Cyc_Absyn_Decl* d, struct Cyc_Absyn_Stmt* s, struct
Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct*
_temp281=( struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp281[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp282; _temp282.tag= Cyc_Absyn_Decl_s_tag;
_temp282.f1= d; _temp282.f2= s; _temp282;}); _temp281;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_declare_stmt( struct _tuple0* x, void* t, struct Cyc_Absyn_Exp* init,
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){ struct Cyc_Absyn_Decl*
d= Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Var_d_struct* _temp283=(
struct Cyc_Absyn_Var_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Var_d_struct));
_temp283[ 0]=({ struct Cyc_Absyn_Var_d_struct _temp284; _temp284.tag= Cyc_Absyn_Var_d_tag;
_temp284.f1= Cyc_Absyn_new_vardecl( x, t, init); _temp284;}); _temp283;}), loc);
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Decl_s_struct* _temp285=(
struct Cyc_Absyn_Decl_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Decl_s_struct));
_temp285[ 0]=({ struct Cyc_Absyn_Decl_s_struct _temp286; _temp286.tag= Cyc_Absyn_Decl_s_tag;
_temp286.f1= d; _temp286.f2= s; _temp286;}); _temp285;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_cut_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){
return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Cut_s_struct* _temp287=(
struct Cyc_Absyn_Cut_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Cut_s_struct));
_temp287[ 0]=({ struct Cyc_Absyn_Cut_s_struct _temp288; _temp288.tag= Cyc_Absyn_Cut_s_tag;
_temp288.f1= s; _temp288;}); _temp287;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_splice_stmt(
struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_Splice_s_struct* _temp289=( struct Cyc_Absyn_Splice_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Splice_s_struct)); _temp289[ 0]=({ struct
Cyc_Absyn_Splice_s_struct _temp290; _temp290.tag= Cyc_Absyn_Splice_s_tag;
_temp290.f1= s; _temp290;}); _temp289;}), loc);} struct Cyc_Absyn_Stmt* Cyc_Absyn_label_stmt(
struct _tagged_string* v, struct Cyc_Absyn_Stmt* s, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Label_s_struct*
_temp291=( struct Cyc_Absyn_Label_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Label_s_struct));
_temp291[ 0]=({ struct Cyc_Absyn_Label_s_struct _temp292; _temp292.tag= Cyc_Absyn_Label_s_tag;
_temp292.f1= v; _temp292.f2= s; _temp292;}); _temp291;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_do_stmt( struct Cyc_Absyn_Stmt* s, struct Cyc_Absyn_Exp* e, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Do_s_struct*
_temp293=( struct Cyc_Absyn_Do_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Do_s_struct));
_temp293[ 0]=({ struct Cyc_Absyn_Do_s_struct _temp294; _temp294.tag= Cyc_Absyn_Do_s_tag;
_temp294.f1= s; _temp294.f2=({ struct _tuple2 _temp295; _temp295.f1= e; _temp295.f2=
Cyc_Absyn_skip_stmt( e->loc); _temp295;}); _temp294;}); _temp293;}), loc);}
struct Cyc_Absyn_Stmt* Cyc_Absyn_trycatch_stmt( struct Cyc_Absyn_Stmt* s, struct
Cyc_List_List* scs, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_stmt((
void*)({ struct Cyc_Absyn_TryCatch_s_struct* _temp296=( struct Cyc_Absyn_TryCatch_s_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_TryCatch_s_struct)); _temp296[ 0]=({ struct
Cyc_Absyn_TryCatch_s_struct _temp297; _temp297.tag= Cyc_Absyn_TryCatch_s_tag;
_temp297.f1= s; _temp297.f2= scs; _temp297;}); _temp296;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_goto_stmt( struct _tagged_string* lab, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_stmt(( void*)({ struct Cyc_Absyn_Goto_s_struct*
_temp298=( struct Cyc_Absyn_Goto_s_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Goto_s_struct));
_temp298[ 0]=({ struct Cyc_Absyn_Goto_s_struct _temp299; _temp299.tag= Cyc_Absyn_Goto_s_tag;
_temp299.f1= lab; _temp299.f2= 0; _temp299;}); _temp298;}), loc);} struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt( struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_exp_stmt( Cyc_Absyn_assign_exp(
e1, e2, loc), loc);} struct Cyc_Absyn_Pat* Cyc_Absyn_new_pat( void* p, struct
Cyc_Position_Segment* s){ return({ struct Cyc_Absyn_Pat* _temp300=( struct Cyc_Absyn_Pat*)
GC_malloc( sizeof( struct Cyc_Absyn_Pat)); _temp300->r=( void*) p; _temp300->topt=
0; _temp300->loc= s; _temp300;});} struct Cyc_Absyn_Decl* Cyc_Absyn_new_decl(
void* r, struct Cyc_Position_Segment* loc){ return({ struct Cyc_Absyn_Decl*
_temp301=( struct Cyc_Absyn_Decl*) GC_malloc( sizeof( struct Cyc_Absyn_Decl));
_temp301->r=( void*) r; _temp301->loc= loc; _temp301;});} struct Cyc_Absyn_Decl*
Cyc_Absyn_let_decl( struct Cyc_Absyn_Pat* p, struct Cyc_Core_Opt* t_opt, struct
Cyc_Absyn_Exp* e, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Let_d_struct* _temp302=( struct Cyc_Absyn_Let_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Let_d_struct)); _temp302[ 0]=({ struct Cyc_Absyn_Let_d_struct
_temp303; _temp303.tag= Cyc_Absyn_Let_d_tag; _temp303.f1= p; _temp303.f2= 0;
_temp303.f3= t_opt; _temp303.f4= e; _temp303.f5= 0; _temp303;}); _temp302;}),
loc);} struct Cyc_Absyn_Vardecl* Cyc_Absyn_new_vardecl( struct _tuple0* x, void*
t, struct Cyc_Absyn_Exp* init){ return({ struct Cyc_Absyn_Vardecl* _temp304=(
struct Cyc_Absyn_Vardecl*) GC_malloc( sizeof( struct Cyc_Absyn_Vardecl));
_temp304->sc=( void*)(( void*) Cyc_Absyn_Public); _temp304->name= x; _temp304->tq=
Cyc_Absyn_empty_tqual(); _temp304->type=( void*) t; _temp304->initializer= init;
_temp304->shadow= 0; _temp304->region= 0; _temp304->attributes= 0; _temp304;});}
struct Cyc_Absyn_Vardecl* Cyc_Absyn_static_vardecl( struct _tuple0* x, void* t,
struct Cyc_Absyn_Exp* init){ return({ struct Cyc_Absyn_Vardecl* _temp305=(
struct Cyc_Absyn_Vardecl*) GC_malloc( sizeof( struct Cyc_Absyn_Vardecl));
_temp305->sc=( void*)(( void*) Cyc_Absyn_Static); _temp305->name= x; _temp305->tq=
Cyc_Absyn_empty_tqual(); _temp305->type=( void*) t; _temp305->initializer= init;
_temp305->shadow= 0; _temp305->region= 0; _temp305->attributes= 0; _temp305;});}
struct Cyc_Absyn_Decl* Cyc_Absyn_struct_decl( void* s, struct Cyc_Core_Opt* n,
struct Cyc_List_List* ts, struct Cyc_Core_Opt* fs, struct Cyc_List_List* atts,
struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_decl(( void*)({ struct
Cyc_Absyn_Struct_d_struct* _temp306=( struct Cyc_Absyn_Struct_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Struct_d_struct)); _temp306[ 0]=({ struct
Cyc_Absyn_Struct_d_struct _temp307; _temp307.tag= Cyc_Absyn_Struct_d_tag;
_temp307.f1=({ struct Cyc_Absyn_Structdecl* _temp308=( struct Cyc_Absyn_Structdecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Structdecl)); _temp308->sc=( void*) s;
_temp308->name= n; _temp308->tvs= ts; _temp308->fields= fs; _temp308->attributes=
atts; _temp308;}); _temp307;}); _temp306;}), loc);} struct Cyc_Absyn_Decl* Cyc_Absyn_union_decl(
void* s, struct Cyc_Core_Opt* n, struct Cyc_List_List* ts, struct Cyc_Core_Opt*
fs, struct Cyc_List_List* atts, struct Cyc_Position_Segment* loc){ return Cyc_Absyn_new_decl((
void*)({ struct Cyc_Absyn_Union_d_struct* _temp309=( struct Cyc_Absyn_Union_d_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Union_d_struct)); _temp309[ 0]=({ struct Cyc_Absyn_Union_d_struct
_temp310; _temp310.tag= Cyc_Absyn_Union_d_tag; _temp310.f1=({ struct Cyc_Absyn_Uniondecl*
_temp311=( struct Cyc_Absyn_Uniondecl*) GC_malloc( sizeof( struct Cyc_Absyn_Uniondecl));
_temp311->sc=( void*) s; _temp311->name= n; _temp311->tvs= ts; _temp311->fields=
fs; _temp311->attributes= atts; _temp311;}); _temp310;}); _temp309;}), loc);}
struct Cyc_Absyn_Decl* Cyc_Absyn_tunion_decl( void* s, struct Cyc_Core_Opt* n,
struct Cyc_List_List* ts, struct Cyc_Core_Opt* fs, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_Tunion_d_struct*
_temp312=( struct Cyc_Absyn_Tunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Tunion_d_struct));
_temp312[ 0]=({ struct Cyc_Absyn_Tunion_d_struct _temp313; _temp313.tag= Cyc_Absyn_Tunion_d_tag;
_temp313.f1=({ struct Cyc_Absyn_Tuniondecl* _temp314=( struct Cyc_Absyn_Tuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_Tuniondecl)); _temp314->sc=( void*) s;
_temp314->name= n; _temp314->tvs= ts; _temp314->fields= fs; _temp314;});
_temp313;}); _temp312;}), loc);} struct Cyc_Absyn_Decl* Cyc_Absyn_xtunion_decl(
void* s, struct _tuple0* n, struct Cyc_List_List* fs, struct Cyc_Position_Segment*
loc){ return Cyc_Absyn_new_decl(( void*)({ struct Cyc_Absyn_XTunion_d_struct*
_temp315=( struct Cyc_Absyn_XTunion_d_struct*) GC_malloc( sizeof( struct Cyc_Absyn_XTunion_d_struct));
_temp315[ 0]=({ struct Cyc_Absyn_XTunion_d_struct _temp316; _temp316.tag= Cyc_Absyn_XTunion_d_tag;
_temp316.f1=({ struct Cyc_Absyn_XTuniondecl* _temp317=( struct Cyc_Absyn_XTuniondecl*)
GC_malloc( sizeof( struct Cyc_Absyn_XTuniondecl)); _temp317->sc=( void*) s;
_temp317->name= n; _temp317->fields= fs; _temp317;}); _temp316;}); _temp315;}),
loc);} static struct _tuple1* Cyc_Absyn_expand_arg( struct _tuple1* a){ return({
struct _tuple1* _temp318=( struct _tuple1*) GC_malloc( sizeof( struct _tuple1));
_temp318->f1=(* a).f1; _temp318->f2=(* a).f2; _temp318->f3= Cyc_Absyn_pointer_expand((*
a).f3); _temp318;});} void* Cyc_Absyn_function_typ( struct Cyc_List_List* tvs,
struct Cyc_Core_Opt* eff_typ, void* ret_typ, struct Cyc_List_List* args, int
varargs, struct Cyc_List_List* atts){ return( void*)({ struct Cyc_Absyn_FnType_struct*
_temp319=( struct Cyc_Absyn_FnType_struct*) GC_malloc( sizeof( struct Cyc_Absyn_FnType_struct));
_temp319[ 0]=({ struct Cyc_Absyn_FnType_struct _temp320; _temp320.tag= Cyc_Absyn_FnType_tag;
_temp320.f1=({ struct Cyc_Absyn_FnInfo _temp321; _temp321.tvars= tvs; _temp321.ret_typ=(
void*) Cyc_Absyn_pointer_expand( ret_typ); _temp321.effect= eff_typ; _temp321.args=((
struct Cyc_List_List*(*)( struct _tuple1*(* f)( struct _tuple1*), struct Cyc_List_List*
x)) Cyc_List_map)( Cyc_Absyn_expand_arg, args); _temp321.varargs= varargs;
_temp321.attributes= atts; _temp321;}); _temp320;}); _temp319;});} void* Cyc_Absyn_pointer_expand(
void* t){ void* _temp322= t; struct Cyc_Absyn_FnInfo _temp328; _LL324: if((
unsigned int) _temp322 > 4u?(( struct _tunion_struct*) _temp322)->tag == Cyc_Absyn_FnType_tag:
0){ _LL329: _temp328=( struct Cyc_Absyn_FnInfo)(( struct Cyc_Absyn_FnType_struct*)
_temp322)->f1; goto _LL325;} else{ goto _LL326;} _LL326: goto _LL327; _LL325:
return Cyc_Absyn_at_typ( t,( void*) Cyc_Absyn_HeapRgn, Cyc_Absyn_empty_tqual());
_LL327: return t; _LL323:;} int Cyc_Absyn_is_lvalue( struct Cyc_Absyn_Exp* e){
void* _temp330=( void*) e->r; void* _temp350; struct Cyc_Absyn_Fndecl* _temp352;
struct _tuple0* _temp354; void* _temp356; struct Cyc_Absyn_Vardecl* _temp358;
struct _tuple0* _temp360; void* _temp362; struct Cyc_Absyn_Vardecl* _temp364;
struct _tuple0* _temp366; void* _temp368; struct _tuple0* _temp370; struct Cyc_Absyn_Exp*
_temp372; struct Cyc_Absyn_Exp* _temp374; struct _tagged_string* _temp376;
struct Cyc_Absyn_Exp* _temp378; struct _tagged_string* _temp380; struct Cyc_Absyn_Exp*
_temp382; struct Cyc_Absyn_Exp* _temp384; _LL332: if((( struct _tunion_struct*)
_temp330)->tag == Cyc_Absyn_Var_e_tag){ _LL355: _temp354=( struct _tuple0*)((
struct Cyc_Absyn_Var_e_struct*) _temp330)->f1; goto _LL351; _LL351: _temp350=(
void*)(( struct Cyc_Absyn_Var_e_struct*) _temp330)->f2; if(( unsigned int)
_temp350 > 1u?(( struct _tunion_struct*) _temp350)->tag == Cyc_Absyn_Funname_b_tag:
0){ _LL353: _temp352=( struct Cyc_Absyn_Fndecl*)(( struct Cyc_Absyn_Funname_b_struct*)
_temp350)->f1; goto _LL333;} else{ goto _LL334;}} else{ goto _LL334;} _LL334:
if((( struct _tunion_struct*) _temp330)->tag == Cyc_Absyn_Var_e_tag){ _LL361:
_temp360=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp330)->f1;
goto _LL357; _LL357: _temp356=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp330)->f2; if(( unsigned int) _temp356 > 1u?(( struct _tunion_struct*)
_temp356)->tag == Cyc_Absyn_Global_b_tag: 0){ _LL359: _temp358=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Global_b_struct*) _temp356)->f1; goto _LL335;} else{ goto
_LL336;}} else{ goto _LL336;} _LL336: if((( struct _tunion_struct*) _temp330)->tag
== Cyc_Absyn_Var_e_tag){ _LL367: _temp366=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*)
_temp330)->f1; goto _LL363; _LL363: _temp362=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp330)->f2; if(( unsigned int) _temp362 > 1u?(( struct _tunion_struct*)
_temp362)->tag == Cyc_Absyn_Local_b_tag: 0){ _LL365: _temp364=( struct Cyc_Absyn_Vardecl*)((
struct Cyc_Absyn_Local_b_struct*) _temp362)->f1; goto _LL337;} else{ goto _LL338;}}
else{ goto _LL338;} _LL338: if((( struct _tunion_struct*) _temp330)->tag == Cyc_Absyn_Var_e_tag){
_LL371: _temp370=( struct _tuple0*)(( struct Cyc_Absyn_Var_e_struct*) _temp330)->f1;
goto _LL369; _LL369: _temp368=( void*)(( struct Cyc_Absyn_Var_e_struct*)
_temp330)->f2; goto _LL339;} else{ goto _LL340;} _LL340: if((( struct
_tunion_struct*) _temp330)->tag == Cyc_Absyn_Subscript_e_tag){ _LL375: _temp374=(
struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*) _temp330)->f1;
goto _LL373; _LL373: _temp372=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Subscript_e_struct*)
_temp330)->f2; goto _LL341;} else{ goto _LL342;} _LL342: if((( struct
_tunion_struct*) _temp330)->tag == Cyc_Absyn_StructMember_e_tag){ _LL379:
_temp378=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructMember_e_struct*)
_temp330)->f1; goto _LL377; _LL377: _temp376=( struct _tagged_string*)(( struct
Cyc_Absyn_StructMember_e_struct*) _temp330)->f2; goto _LL343;} else{ goto _LL344;}
_LL344: if((( struct _tunion_struct*) _temp330)->tag == Cyc_Absyn_StructArrow_e_tag){
_LL383: _temp382=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_StructArrow_e_struct*)
_temp330)->f1; goto _LL381; _LL381: _temp380=( struct _tagged_string*)(( struct
Cyc_Absyn_StructArrow_e_struct*) _temp330)->f2; goto _LL345;} else{ goto _LL346;}
_LL346: if((( struct _tunion_struct*) _temp330)->tag == Cyc_Absyn_Deref_e_tag){
_LL385: _temp384=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Deref_e_struct*)
_temp330)->f1; goto _LL347;} else{ goto _LL348;} _LL348: goto _LL349; _LL333:
return 0; _LL335: _temp364= _temp358; goto _LL337; _LL337: { void* _temp386= Cyc_Tcutil_compress((
void*) _temp364->type); struct Cyc_Absyn_Exp* _temp392; struct Cyc_Absyn_Tqual*
_temp394; void* _temp396; _LL388: if(( unsigned int) _temp386 > 4u?(( struct
_tunion_struct*) _temp386)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL397: _temp396=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp386)->f1; goto _LL395; _LL395:
_temp394=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp386)->f2; goto _LL393; _LL393: _temp392=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp386)->f3; goto _LL389;} else{ goto _LL390;}
_LL390: goto _LL391; _LL389: return 0; _LL391: return 1; _LL387:;} _LL339:
return 1; _LL341: return 1; _LL343: return Cyc_Absyn_is_lvalue( _temp378);
_LL345: return 1; _LL347: return 1; _LL349: return 0; _LL331:;} struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_field( struct Cyc_Core_Opt* fields, struct _tagged_string* v){
if( fields == 0){ return 0;}{ struct Cyc_List_List* fs=( struct Cyc_List_List*)
fields->v; for( 0; fs != 0; fs= fs->tl){ struct Cyc_Absyn_Structfield* f=(
struct Cyc_Absyn_Structfield*) fs->hd; if( Cyc_String_zstrptrcmp( f->name, v) ==
0){ return( struct Cyc_Absyn_Structfield*) f;}}} return 0;} struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_struct_field( struct Cyc_Absyn_Structdecl* sd, struct
_tagged_string* v){ return Cyc_Absyn_lookup_field( sd->fields, v);} struct Cyc_Absyn_Structfield*
Cyc_Absyn_lookup_union_field( struct Cyc_Absyn_Uniondecl* ud, struct
_tagged_string* v){ return Cyc_Absyn_lookup_field( ud->fields, v);} struct
_tuple3* Cyc_Absyn_lookup_tuple_field( struct Cyc_List_List* ts, int i){ for( 0;
i != 0; -- i){ if( ts == 0){ return 0;} ts= ts->tl;} if( ts == 0){ return 0;}
return( struct _tuple3*)(( struct _tuple3*) ts->hd);} struct _tagged_string Cyc_Absyn_attribute2string(
void* a){ void* _temp398= a; int _temp436; int _temp438; struct _tagged_string
_temp440; _LL400: if(( unsigned int) _temp398 > 15u?(( struct _tunion_struct*)
_temp398)->tag == Cyc_Absyn_Regparm_att_tag: 0){ _LL437: _temp436=( int)((
struct Cyc_Absyn_Regparm_att_struct*) _temp398)->f1; goto _LL401;} else{ goto
_LL402;} _LL402: if(( int) _temp398 == Cyc_Absyn_Stdcall_att){ goto _LL403;}
else{ goto _LL404;} _LL404: if(( int) _temp398 == Cyc_Absyn_Cdecl_att){ goto
_LL405;} else{ goto _LL406;} _LL406: if(( int) _temp398 == Cyc_Absyn_Noreturn_att){
goto _LL407;} else{ goto _LL408;} _LL408: if(( int) _temp398 == Cyc_Absyn_Const_att){
goto _LL409;} else{ goto _LL410;} _LL410: if(( unsigned int) _temp398 > 15u?((
struct _tunion_struct*) _temp398)->tag == Cyc_Absyn_Aligned_att_tag: 0){ _LL439:
_temp438=( int)(( struct Cyc_Absyn_Aligned_att_struct*) _temp398)->f1; goto
_LL411;} else{ goto _LL412;} _LL412: if(( int) _temp398 == Cyc_Absyn_Packed_att){
goto _LL413;} else{ goto _LL414;} _LL414: if(( unsigned int) _temp398 > 15u?((
struct _tunion_struct*) _temp398)->tag == Cyc_Absyn_Section_att_tag: 0){ _LL441:
_temp440=( struct _tagged_string)(( struct Cyc_Absyn_Section_att_struct*)
_temp398)->f1; goto _LL415;} else{ goto _LL416;} _LL416: if(( int) _temp398 ==
Cyc_Absyn_Nocommon_att){ goto _LL417;} else{ goto _LL418;} _LL418: if(( int)
_temp398 == Cyc_Absyn_Shared_att){ goto _LL419;} else{ goto _LL420;} _LL420: if((
int) _temp398 == Cyc_Absyn_Unused_att){ goto _LL421;} else{ goto _LL422;} _LL422:
if(( int) _temp398 == Cyc_Absyn_Weak_att){ goto _LL423;} else{ goto _LL424;}
_LL424: if(( int) _temp398 == Cyc_Absyn_Dllimport_att){ goto _LL425;} else{ goto
_LL426;} _LL426: if(( int) _temp398 == Cyc_Absyn_Dllexport_att){ goto _LL427;}
else{ goto _LL428;} _LL428: if(( int) _temp398 == Cyc_Absyn_No_instrument_function_att){
goto _LL429;} else{ goto _LL430;} _LL430: if(( int) _temp398 == Cyc_Absyn_Constructor_att){
goto _LL431;} else{ goto _LL432;} _LL432: if(( int) _temp398 == Cyc_Absyn_Destructor_att){
goto _LL433;} else{ goto _LL434;} _LL434: if(( int) _temp398 == Cyc_Absyn_No_check_memory_usage_att){
goto _LL435;} else{ goto _LL399;} _LL401: return xprintf("regparm(%d)", _temp436);
_LL403: return( struct _tagged_string)({ char* _temp442=( char*)"stdcall";
struct _tagged_string _temp443; _temp443.curr= _temp442; _temp443.base= _temp442;
_temp443.last_plus_one= _temp442 + 8; _temp443;}); _LL405: return( struct
_tagged_string)({ char* _temp444=( char*)"cdecl"; struct _tagged_string _temp445;
_temp445.curr= _temp444; _temp445.base= _temp444; _temp445.last_plus_one=
_temp444 + 6; _temp445;}); _LL407: return( struct _tagged_string)({ char*
_temp446=( char*)"noreturn"; struct _tagged_string _temp447; _temp447.curr=
_temp446; _temp447.base= _temp446; _temp447.last_plus_one= _temp446 + 9;
_temp447;}); _LL409: return( struct _tagged_string)({ char* _temp448=( char*)"const";
struct _tagged_string _temp449; _temp449.curr= _temp448; _temp449.base= _temp448;
_temp449.last_plus_one= _temp448 + 6; _temp449;}); _LL411: if( _temp438 == - 1){
return( struct _tagged_string)({ char* _temp450=( char*)"aligned"; struct
_tagged_string _temp451; _temp451.curr= _temp450; _temp451.base= _temp450;
_temp451.last_plus_one= _temp450 + 8; _temp451;});} else{ return xprintf("aligned(%d)",
_temp438);} _LL413: return( struct _tagged_string)({ char* _temp452=( char*)"packed";
struct _tagged_string _temp453; _temp453.curr= _temp452; _temp453.base= _temp452;
_temp453.last_plus_one= _temp452 + 7; _temp453;}); _LL415: return({ struct
_tagged_string _temp454= _temp440; xprintf("section(\"%.*s\")", _temp454.last_plus_one
- _temp454.curr, _temp454.curr);}); _LL417: return( struct _tagged_string)({
char* _temp455=( char*)"nocommon"; struct _tagged_string _temp456; _temp456.curr=
_temp455; _temp456.base= _temp455; _temp456.last_plus_one= _temp455 + 9;
_temp456;}); _LL419: return( struct _tagged_string)({ char* _temp457=( char*)"shared";
struct _tagged_string _temp458; _temp458.curr= _temp457; _temp458.base= _temp457;
_temp458.last_plus_one= _temp457 + 7; _temp458;}); _LL421: return( struct
_tagged_string)({ char* _temp459=( char*)"unused"; struct _tagged_string
_temp460; _temp460.curr= _temp459; _temp460.base= _temp459; _temp460.last_plus_one=
_temp459 + 7; _temp460;}); _LL423: return( struct _tagged_string)({ char*
_temp461=( char*)"weak"; struct _tagged_string _temp462; _temp462.curr= _temp461;
_temp462.base= _temp461; _temp462.last_plus_one= _temp461 + 5; _temp462;});
_LL425: return( struct _tagged_string)({ char* _temp463=( char*)"dllimport";
struct _tagged_string _temp464; _temp464.curr= _temp463; _temp464.base= _temp463;
_temp464.last_plus_one= _temp463 + 10; _temp464;}); _LL427: return( struct
_tagged_string)({ char* _temp465=( char*)"dllexport"; struct _tagged_string
_temp466; _temp466.curr= _temp465; _temp466.base= _temp465; _temp466.last_plus_one=
_temp465 + 10; _temp466;}); _LL429: return( struct _tagged_string)({ char*
_temp467=( char*)"no_instrument_function"; struct _tagged_string _temp468;
_temp468.curr= _temp467; _temp468.base= _temp467; _temp468.last_plus_one=
_temp467 + 23; _temp468;}); _LL431: return( struct _tagged_string)({ char*
_temp469=( char*)"constructor"; struct _tagged_string _temp470; _temp470.curr=
_temp469; _temp470.base= _temp469; _temp470.last_plus_one= _temp469 + 12;
_temp470;}); _LL433: return( struct _tagged_string)({ char* _temp471=( char*)"destructor";
struct _tagged_string _temp472; _temp472.curr= _temp471; _temp472.base= _temp471;
_temp472.last_plus_one= _temp471 + 11; _temp472;}); _LL435: return( struct
_tagged_string)({ char* _temp473=( char*)"no_check_memory_usage"; struct
_tagged_string _temp474; _temp474.curr= _temp473; _temp474.base= _temp473;
_temp474.last_plus_one= _temp473 + 22; _temp474;}); _LL399:;}