#include "cyc_include.h"

 struct _tuple0{ void* f1; struct _tagged_string* f2; } ; struct _tuple1{ struct
Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual* f2; void* f3; } ; struct _tuple2{
struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; } ; struct _tuple3{ void*
f1; void* f2; } ; struct _tuple4{ struct Cyc_Absyn_Tqual* f1; void* f2; } ;
struct _tuple5{ void* f1; int f2; } ; typedef unsigned int Cyc_uint; typedef
char* Cyc_Cstring; typedef struct _tagged_string Cyc_string; typedef struct
_tagged_string Cyc_string_t; typedef struct _tagged_string* Cyc_stringptr;
typedef int Cyc_bool; extern void* exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; typedef struct Cyc_Core_Opt* Cyc_Core_opt_t; extern char Cyc_Core_InvalidArg_tag[
11u]; struct Cyc_Core_InvalidArg_struct{ char* tag; struct _tagged_string f1; }
; extern char Cyc_Core_Failure_tag[ 8u]; struct Cyc_Core_Failure_struct{ char*
tag; struct _tagged_string f1; } ; extern char Cyc_Core_Impossible_tag[ 11u];
struct Cyc_Core_Impossible_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Core_Not_found_tag[ 10u]; struct Cyc_Core_Not_found_struct{ char*
tag; } ; extern char Cyc_Core_Unreachable_tag[ 12u]; struct Cyc_Core_Unreachable_struct{
char* tag; struct _tagged_string f1; } ; extern char* string_to_Cstring( struct
_tagged_string); extern char* underlying_Cstring( struct _tagged_string); extern
struct _tagged_string Cstring_to_string( char*); extern int system( char*);
struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List*
Cyc_List_glist_t; typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct
Cyc_List_List* Cyc_List_List_t; extern char Cyc_List_List_empty_tag[ 11u];
struct Cyc_List_List_empty_struct{ char* tag; } ; extern char Cyc_List_List_mismatch_tag[
14u]; struct Cyc_List_List_mismatch_struct{ char* tag; } ; extern char Cyc_List_Nth_tag[
4u]; struct Cyc_List_Nth_struct{ char* tag; } ; struct Cyc_Lineno_Pos{ struct
_tagged_string logical_file; struct _tagged_string line; int line_no; int col; }
; typedef struct Cyc_Lineno_Pos* Cyc_Lineno_pos_t; extern char Cyc_Position_Exit_tag[
5u]; struct Cyc_Position_Exit_struct{ char* tag; } ; struct Cyc_Position_Segment;
typedef struct Cyc_Position_Segment* Cyc_Position_seg_t; typedef void* Cyc_Position_Error_kind;
extern void* Cyc_Position_Lex; extern void* Cyc_Position_Parse; extern void* Cyc_Position_Elab;
typedef void* Cyc_Position_error_kind_t; struct Cyc_Position_Error{ struct
_tagged_string source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_string desc; } ; typedef struct Cyc_Position_Error* Cyc_Position_error_t;
extern struct Cyc_Position_Error* Cyc_Position_mk_err_elab( struct Cyc_Position_Segment*,
struct _tagged_string); extern char Cyc_Position_Nocontext_tag[ 10u]; struct Cyc_Position_Nocontext_struct{
char* tag; } ; extern void Cyc_Position_post_error( struct Cyc_Position_Error*);
typedef struct _tagged_string* Cyc_Absyn_field_name; typedef struct
_tagged_string* Cyc_Absyn_var; typedef struct _tagged_string* Cyc_Absyn_tvarname_t;
typedef void* Cyc_Absyn_Nmspace; typedef struct _tuple0* Cyc_Absyn_qvar; typedef
struct _tuple0* Cyc_Absyn_qvar_opt_t; typedef struct _tuple0* Cyc_Absyn_typedef_name_t;
typedef struct _tuple0* Cyc_Absyn_typedef_name_opt_t; struct Cyc_Absyn_Tvar;
typedef void* Cyc_Absyn_Scope; struct Cyc_Absyn_Tqual; typedef void* Cyc_Absyn_Size_of;
typedef void* Cyc_Absyn_Kind; typedef void* Cyc_Absyn_Array_kind; typedef void*
Cyc_Absyn_Sign; struct Cyc_Absyn_Conref; typedef void* Cyc_Absyn_Constraint;
typedef void* Cyc_Absyn_Bounds; struct Cyc_Absyn_PtrInfo; struct Cyc_Absyn_FnInfo;
typedef void* Cyc_Absyn_Typ; typedef void* Cyc_Absyn_Funcparams; typedef void*
Cyc_Absyn_Type_modifier; typedef void* Cyc_Absyn_Cnst; typedef void* Cyc_Absyn_Primop;
typedef void* Cyc_Absyn_Incrementor; typedef void* Cyc_Absyn_Raw_exp; struct Cyc_Absyn_Exp;
typedef void* Cyc_Absyn_Raw_stmt; struct Cyc_Absyn_Stmt; typedef void* Cyc_Absyn_Raw_pat;
struct Cyc_Absyn_Pat; typedef void* Cyc_Absyn_Binding; struct Cyc_Absyn_Switch_clause;
struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl; struct Cyc_Absyn_Uniondecl;
struct Cyc_Absyn_Enumfield; struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Xenumdecl;
struct Cyc_Absyn_Typedefdecl; struct Cyc_Absyn_Vardecl; typedef void* Cyc_Absyn_Raw_decl;
struct Cyc_Absyn_Decl; typedef void* Cyc_Absyn_Designator; typedef struct
_xenum_struct* Cyc_Absyn_StmtAnnot; typedef void* Cyc_Absyn_Attribute; struct
Cyc_Absyn_Structfield; typedef void* Cyc_Absyn_Mallocarg; typedef void* Cyc_Absyn_scope;
typedef struct Cyc_Absyn_Tqual* Cyc_Absyn_tqual; typedef void* Cyc_Absyn_size_of_t;
typedef void* Cyc_Absyn_kind_t; typedef struct Cyc_Absyn_Tvar* Cyc_Absyn_tvar;
typedef void* Cyc_Absyn_sign_t; typedef struct Cyc_Absyn_Conref* Cyc_Absyn_conref;
typedef void* Cyc_Absyn_constraint_t; typedef void* Cyc_Absyn_bounds_t; typedef
struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t; typedef struct Cyc_Absyn_FnInfo
Cyc_Absyn_fn_info_t; typedef void* Cyc_Absyn_typ; typedef void* Cyc_Absyn_funcparams_t;
typedef void* Cyc_Absyn_type_modifier; typedef void* Cyc_Absyn_cnst_t; typedef
void* Cyc_Absyn_primop; typedef void* Cyc_Absyn_incrementor_t; typedef void* Cyc_Absyn_raw_exp_t;
typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp; typedef struct Cyc_Absyn_Exp* Cyc_Absyn_exp_opt;
typedef void* Cyc_Absyn_raw_stmt_t; typedef struct Cyc_Absyn_Stmt* Cyc_Absyn_stmt;
typedef struct Cyc_Absyn_Stmt* Cyc_Absyn_stmt_opt; typedef void* Cyc_Absyn_raw_pat_t;
typedef struct Cyc_Absyn_Pat* Cyc_Absyn_pat; typedef void* Cyc_Absyn_binding_t;
typedef struct Cyc_Absyn_Switch_clause* Cyc_Absyn_switch_clause; typedef struct
Cyc_Absyn_Fndecl* Cyc_Absyn_fndecl; typedef struct Cyc_Absyn_Structdecl* Cyc_Absyn_structdecl;
typedef struct Cyc_Absyn_Uniondecl* Cyc_Absyn_uniondecl_t; typedef struct Cyc_Absyn_Enumfield*
Cyc_Absyn_enumfield; typedef struct Cyc_Absyn_Enumdecl* Cyc_Absyn_enumdecl;
typedef struct Cyc_Absyn_Xenumdecl* Cyc_Absyn_xenumdecl; typedef struct Cyc_Absyn_Typedefdecl*
Cyc_Absyn_typedefdecl; typedef struct Cyc_Absyn_Vardecl* Cyc_Absyn_vardecl;
typedef void* Cyc_Absyn_raw_decl; typedef struct Cyc_Absyn_Decl* Cyc_Absyn_decl;
typedef void* Cyc_Absyn_designator; typedef struct _xenum_struct* Cyc_Absyn_stmt_annot_t;
typedef void* Cyc_Absyn_attribute_t; typedef struct Cyc_List_List* Cyc_Absyn_attributes_t;
typedef struct Cyc_Absyn_Structfield* Cyc_Absyn_structfield_t; typedef void* Cyc_Absyn_mallocarg_t;
extern void* Cyc_Absyn_Loc_n; extern const int Cyc_Absyn_Rel_n_tag; struct Cyc_Absyn_Rel_n_struct{
int tag; struct Cyc_List_List* f1; } ; extern const int Cyc_Absyn_Abs_n_tag;
struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List* f1; } ; extern
void* Cyc_Absyn_Static; extern void* Cyc_Absyn_Abstract; extern void* Cyc_Absyn_Public;
extern void* Cyc_Absyn_Extern; extern void* Cyc_Absyn_ExternC; struct Cyc_Absyn_Tqual{
int q_const; int q_volatile; int q_restrict; } ; extern void* Cyc_Absyn_B1;
extern void* Cyc_Absyn_B2; extern void* Cyc_Absyn_B4; extern void* Cyc_Absyn_B8;
extern void* Cyc_Absyn_AnyKind; extern void* Cyc_Absyn_MemKind; extern void* Cyc_Absyn_BoxKind;
extern void* Cyc_Absyn_RgnKind; extern void* Cyc_Absyn_EffKind; extern void* Cyc_Absyn_Signed;
extern void* Cyc_Absyn_Unsigned; struct Cyc_Absyn_Conref{ void* v; } ; extern
const int Cyc_Absyn_Eq_constr_tag; struct Cyc_Absyn_Eq_constr_struct{ int tag;
void* f1; } ; extern const int Cyc_Absyn_Forward_constr_tag; struct Cyc_Absyn_Forward_constr_struct{
int tag; struct Cyc_Absyn_Conref* f1; } ; extern void* Cyc_Absyn_No_constr;
struct Cyc_Absyn_Tvar{ struct _tagged_string* name; struct Cyc_Absyn_Conref*
kind; } ; extern void* Cyc_Absyn_Unknown_b; extern const int Cyc_Absyn_Upper_b_tag;
struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct
Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
nullable; struct Cyc_Absyn_Tqual* tq; struct Cyc_Absyn_Conref* bounds; } ;
struct Cyc_Absyn_FnInfo{ struct Cyc_List_List* tvars; struct Cyc_Core_Opt*
effect; void* ret_typ; struct Cyc_List_List* args; int varargs; struct Cyc_List_List*
attributes; } ; extern void* Cyc_Absyn_VoidType; extern const int Cyc_Absyn_Evar_tag;
struct Cyc_Absyn_Evar_struct{ int tag; void* f1; struct Cyc_Core_Opt* f2; int f3;
} ; extern const int Cyc_Absyn_VarType_tag; struct Cyc_Absyn_VarType_struct{ int
tag; struct Cyc_Absyn_Tvar* f1; } ; extern const int Cyc_Absyn_EnumType_tag;
struct Cyc_Absyn_EnumType_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Enumdecl** f3; } ; extern const int Cyc_Absyn_XenumType_tag;
struct Cyc_Absyn_XenumType_struct{ int tag; struct _tuple0* f1; struct Cyc_Absyn_Xenumdecl**
f2; } ; extern const int Cyc_Absyn_PointerType_tag; struct Cyc_Absyn_PointerType_struct{
int tag; struct Cyc_Absyn_PtrInfo f1; } ; extern const int Cyc_Absyn_IntType_tag;
struct Cyc_Absyn_IntType_struct{ int tag; void* f1; void* f2; } ; extern void*
Cyc_Absyn_FloatType; extern void* Cyc_Absyn_DoubleType; extern const int Cyc_Absyn_ArrayType_tag;
struct Cyc_Absyn_ArrayType_struct{ int tag; void* f1; struct Cyc_Absyn_Tqual* f2;
struct Cyc_Absyn_Exp* f3; } ; extern const int Cyc_Absyn_FnType_tag; struct Cyc_Absyn_FnType_struct{
int tag; struct Cyc_Absyn_FnInfo f1; } ; extern const int Cyc_Absyn_TupleType_tag;
struct Cyc_Absyn_TupleType_struct{ int tag; struct Cyc_List_List* f1; } ; extern
const int Cyc_Absyn_StructType_tag; struct Cyc_Absyn_StructType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; extern const int Cyc_Absyn_UnionType_tag; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; extern const int Cyc_Absyn_RgnHandleType_tag; struct Cyc_Absyn_RgnHandleType_struct{
int tag; void* f1; } ; extern const int Cyc_Absyn_TypedefType_tag; struct Cyc_Absyn_TypedefType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3;
} ; extern void* Cyc_Absyn_HeapRgn; extern const int Cyc_Absyn_AccessEff_tag;
struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; extern const int Cyc_Absyn_JoinEff_tag;
struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List* f1; } ; extern
const int Cyc_Absyn_NoTypes_tag; struct Cyc_Absyn_NoTypes_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; extern const int
Cyc_Absyn_WithTypes_tag; struct Cyc_Absyn_WithTypes_struct{ int tag; struct Cyc_List_List*
f1; int f2; struct Cyc_Core_Opt* f3; } ; typedef void* Cyc_Absyn_Pointer_Sort;
extern const int Cyc_Absyn_NonNullable_ps_tag; struct Cyc_Absyn_NonNullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; extern const int Cyc_Absyn_Nullable_ps_tag;
struct Cyc_Absyn_Nullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
extern void* Cyc_Absyn_TaggedArray_ps; extern const int Cyc_Absyn_Regparm_att_tag;
struct Cyc_Absyn_Regparm_att_struct{ int tag; int f1; } ; extern void* Cyc_Absyn_Stdcall_att;
extern void* Cyc_Absyn_Cdecl_att; extern void* Cyc_Absyn_Noreturn_att; extern
void* Cyc_Absyn_Const_att; extern const int Cyc_Absyn_Aligned_att_tag; struct
Cyc_Absyn_Aligned_att_struct{ int tag; int f1; } ; extern void* Cyc_Absyn_Packed_att;
extern const int Cyc_Absyn_Section_att_tag; struct Cyc_Absyn_Section_att_struct{
int tag; struct _tagged_string f1; } ; extern void* Cyc_Absyn_Nocommon_att;
extern void* Cyc_Absyn_Shared_att; extern void* Cyc_Absyn_Unused_att; extern
void* Cyc_Absyn_Weak_att; extern void* Cyc_Absyn_Dllimport_att; extern void* Cyc_Absyn_Dllexport_att;
extern void* Cyc_Absyn_No_instrument_function_att; extern void* Cyc_Absyn_Constructor_att;
extern void* Cyc_Absyn_Destructor_att; extern void* Cyc_Absyn_No_check_memory_usage_att;
extern void* Cyc_Absyn_Carray_mod; extern const int Cyc_Absyn_ConstArray_mod_tag;
struct Cyc_Absyn_ConstArray_mod_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
extern const int Cyc_Absyn_Pointer_mod_tag; struct Cyc_Absyn_Pointer_mod_struct{
int tag; void* f1; void* f2; struct Cyc_Absyn_Tqual* f3; } ; extern const int
Cyc_Absyn_Function_mod_tag; struct Cyc_Absyn_Function_mod_struct{ int tag; void*
f1; } ; extern const int Cyc_Absyn_TypeParams_mod_tag; struct Cyc_Absyn_TypeParams_mod_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; int f3; } ;
extern const int Cyc_Absyn_Attributes_mod_tag; struct Cyc_Absyn_Attributes_mod_struct{
int tag; struct Cyc_Position_Segment* f1; struct Cyc_List_List* f2; } ; extern
const int Cyc_Absyn_Char_c_tag; struct Cyc_Absyn_Char_c_struct{ int tag; void*
f1; char f2; } ; extern const int Cyc_Absyn_Short_c_tag; struct Cyc_Absyn_Short_c_struct{
int tag; void* f1; short f2; } ; extern const int Cyc_Absyn_Int_c_tag; struct
Cyc_Absyn_Int_c_struct{ int tag; void* f1; int f2; } ; extern const int Cyc_Absyn_LongLong_c_tag;
struct Cyc_Absyn_LongLong_c_struct{ int tag; void* f1; long long f2; } ; extern
const int Cyc_Absyn_Float_c_tag; struct Cyc_Absyn_Float_c_struct{ int tag;
struct _tagged_string f1; } ; extern const int Cyc_Absyn_String_c_tag; struct
Cyc_Absyn_String_c_struct{ int tag; int f1; struct _tagged_string f2; } ; extern
void* Cyc_Absyn_Null_c; extern void* Cyc_Absyn_Plus; extern void* Cyc_Absyn_Times;
extern void* Cyc_Absyn_Minus; extern void* Cyc_Absyn_Div; extern void* Cyc_Absyn_Mod;
extern void* Cyc_Absyn_Eq; extern void* Cyc_Absyn_Neq; extern void* Cyc_Absyn_Gt;
extern void* Cyc_Absyn_Lt; extern void* Cyc_Absyn_Gte; extern void* Cyc_Absyn_Lte;
extern void* Cyc_Absyn_Not; extern void* Cyc_Absyn_Bitnot; extern void* Cyc_Absyn_Bitand;
extern void* Cyc_Absyn_Bitor; extern void* Cyc_Absyn_Bitxor; extern void* Cyc_Absyn_Bitlshift;
extern void* Cyc_Absyn_Bitlrshift; extern void* Cyc_Absyn_Bitarshift; extern
void* Cyc_Absyn_Size; extern void* Cyc_Absyn_Printf; extern void* Cyc_Absyn_Fprintf;
extern void* Cyc_Absyn_Xprintf; extern void* Cyc_Absyn_Scanf; extern void* Cyc_Absyn_Fscanf;
extern void* Cyc_Absyn_Sscanf; extern void* Cyc_Absyn_PreInc; extern void* Cyc_Absyn_PostInc;
extern void* Cyc_Absyn_PreDec; extern void* Cyc_Absyn_PostDec; extern const int
Cyc_Absyn_Typ_m_tag; struct Cyc_Absyn_Typ_m_struct{ int tag; void* f1; } ;
extern const int Cyc_Absyn_Unresolved_m_tag; struct Cyc_Absyn_Unresolved_m_struct{
int tag; struct _tuple0* f1; } ; extern const int Cyc_Absyn_Enum_m_tag; struct
Cyc_Absyn_Enum_m_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; struct Cyc_Absyn_Enumfield*
f2; } ; extern const int Cyc_Absyn_Xenum_m_tag; struct Cyc_Absyn_Xenum_m_struct{
int tag; struct Cyc_Absyn_Xenumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ;
extern const int Cyc_Absyn_Const_e_tag; struct Cyc_Absyn_Const_e_struct{ int tag;
void* f1; } ; extern const int Cyc_Absyn_Var_e_tag; struct Cyc_Absyn_Var_e_struct{
int tag; struct _tuple0* f1; void* f2; } ; extern const int Cyc_Absyn_UnknownId_e_tag;
struct Cyc_Absyn_UnknownId_e_struct{ int tag; struct _tuple0* f1; } ; extern
const int Cyc_Absyn_Primop_e_tag; struct Cyc_Absyn_Primop_e_struct{ int tag;
void* f1; struct Cyc_List_List* f2; } ; extern const int Cyc_Absyn_AssignOp_e_tag;
struct Cyc_Absyn_AssignOp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Core_Opt* f2; struct Cyc_Absyn_Exp* f3; } ; extern const int Cyc_Absyn_Increment_e_tag;
struct Cyc_Absyn_Increment_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; void* f2;
} ; extern const int Cyc_Absyn_Conditional_e_tag; struct Cyc_Absyn_Conditional_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp*
f3; } ; extern const int Cyc_Absyn_SeqExp_e_tag; struct Cyc_Absyn_SeqExp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Exp* f2; } ; extern const
int Cyc_Absyn_UnknownCall_e_tag; struct Cyc_Absyn_UnknownCall_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; extern const int Cyc_Absyn_FnCall_e_tag;
struct Cyc_Absyn_FnCall_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List*
f2; } ; extern const int Cyc_Absyn_Throw_e_tag; struct Cyc_Absyn_Throw_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; extern const int Cyc_Absyn_NoInstantiate_e_tag;
struct Cyc_Absyn_NoInstantiate_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
extern const int Cyc_Absyn_Instantiate_e_tag; struct Cyc_Absyn_Instantiate_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; extern const
int Cyc_Absyn_Cast_e_tag; struct Cyc_Absyn_Cast_e_struct{ int tag; void* f1;
struct Cyc_Absyn_Exp* f2; } ; extern const int Cyc_Absyn_Address_e_tag; struct
Cyc_Absyn_Address_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; extern const
int Cyc_Absyn_Sizeoftyp_e_tag; struct Cyc_Absyn_Sizeoftyp_e_struct{ int tag;
void* f1; } ; extern const int Cyc_Absyn_Sizeofexp_e_tag; struct Cyc_Absyn_Sizeofexp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; extern const int Cyc_Absyn_Deref_e_tag;
struct Cyc_Absyn_Deref_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; extern
const int Cyc_Absyn_StructMember_e_tag; struct Cyc_Absyn_StructMember_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tagged_string* f2; } ; extern const
int Cyc_Absyn_StructArrow_e_tag; struct Cyc_Absyn_StructArrow_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct _tagged_string* f2; } ; extern const int Cyc_Absyn_Subscript_e_tag;
struct Cyc_Absyn_Subscript_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Absyn_Exp* f2; } ; extern const int Cyc_Absyn_Tuple_e_tag; struct Cyc_Absyn_Tuple_e_struct{
int tag; struct Cyc_List_List* f1; } ; extern const int Cyc_Absyn_CompoundLit_e_tag;
struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple1* f1; struct Cyc_List_List*
f2; } ; extern const int Cyc_Absyn_Array_e_tag; struct Cyc_Absyn_Array_e_struct{
int tag; int f1; struct Cyc_List_List* f2; } ; extern const int Cyc_Absyn_Comprehension_e_tag;
struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl* f1;
struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; extern const int Cyc_Absyn_Struct_e_tag;
struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple0* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; extern const
int Cyc_Absyn_Enum_e_tag; struct Cyc_Absyn_Enum_e_struct{ int tag; struct Cyc_Core_Opt*
f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Enumdecl*
f4; struct Cyc_Absyn_Enumfield* f5; } ; extern const int Cyc_Absyn_Xenum_e_tag;
struct Cyc_Absyn_Xenum_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List*
f2; struct Cyc_Absyn_Xenumdecl* f3; struct Cyc_Absyn_Enumfield* f4; } ; extern
const int Cyc_Absyn_Malloc_e_tag; struct Cyc_Absyn_Malloc_e_struct{ int tag;
void* f1; } ; extern const int Cyc_Absyn_UnresolvedMem_e_tag; struct Cyc_Absyn_UnresolvedMem_e_struct{
int tag; struct Cyc_Core_Opt* f1; struct Cyc_List_List* f2; } ; extern const int
Cyc_Absyn_StmtExp_e_tag; struct Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt*
f1; } ; extern const int Cyc_Absyn_Codegen_e_tag; struct Cyc_Absyn_Codegen_e_struct{
int tag; struct Cyc_Absyn_Fndecl* f1; } ; extern const int Cyc_Absyn_Fill_e_tag;
struct Cyc_Absyn_Fill_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; struct
Cyc_Absyn_Exp{ struct Cyc_Core_Opt* topt; void* r; struct Cyc_Position_Segment*
loc; } ; extern void* Cyc_Absyn_Skip_s; extern const int Cyc_Absyn_Exp_s_tag;
struct Cyc_Absyn_Exp_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; extern
const int Cyc_Absyn_Seq_s_tag; struct Cyc_Absyn_Seq_s_struct{ int tag; struct
Cyc_Absyn_Stmt* f1; struct Cyc_Absyn_Stmt* f2; } ; extern const int Cyc_Absyn_Return_s_tag;
struct Cyc_Absyn_Return_s_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; extern
const int Cyc_Absyn_IfThenElse_s_tag; struct Cyc_Absyn_IfThenElse_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Absyn_Stmt* f2; struct Cyc_Absyn_Stmt*
f3; } ; extern const int Cyc_Absyn_While_s_tag; struct Cyc_Absyn_While_s_struct{
int tag; struct _tuple2 f1; struct Cyc_Absyn_Stmt* f2; } ; extern const int Cyc_Absyn_Break_s_tag;
struct Cyc_Absyn_Break_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; extern
const int Cyc_Absyn_Continue_s_tag; struct Cyc_Absyn_Continue_s_struct{ int tag;
struct Cyc_Absyn_Stmt* f1; } ; extern const int Cyc_Absyn_Goto_s_tag; struct Cyc_Absyn_Goto_s_struct{
int tag; struct _tagged_string* f1; struct Cyc_Absyn_Stmt* f2; } ; extern const
int Cyc_Absyn_For_s_tag; struct Cyc_Absyn_For_s_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct _tuple2 f2; struct _tuple2 f3; struct Cyc_Absyn_Stmt* f4; } ; extern
const int Cyc_Absyn_Switch_s_tag; struct Cyc_Absyn_Switch_s_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; extern const int Cyc_Absyn_Fallthru_s_tag;
struct Cyc_Absyn_Fallthru_s_struct{ int tag; struct Cyc_List_List* f1; struct
Cyc_Absyn_Switch_clause** f2; } ; extern const int Cyc_Absyn_Decl_s_tag; struct
Cyc_Absyn_Decl_s_struct{ int tag; struct Cyc_Absyn_Decl* f1; struct Cyc_Absyn_Stmt*
f2; } ; extern const int Cyc_Absyn_Cut_s_tag; struct Cyc_Absyn_Cut_s_struct{ int
tag; struct Cyc_Absyn_Stmt* f1; } ; extern const int Cyc_Absyn_Splice_s_tag;
struct Cyc_Absyn_Splice_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; extern
const int Cyc_Absyn_Label_s_tag; struct Cyc_Absyn_Label_s_struct{ int tag;
struct _tagged_string* f1; struct Cyc_Absyn_Stmt* f2; } ; extern const int Cyc_Absyn_Do_s_tag;
struct Cyc_Absyn_Do_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; struct _tuple2
f2; } ; extern const int Cyc_Absyn_TryCatch_s_tag; struct Cyc_Absyn_TryCatch_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_List_List* f2; } ; struct Cyc_Absyn_Stmt{
void* r; struct Cyc_Position_Segment* loc; struct Cyc_List_List* non_local_preds;
int try_depth; struct _xenum_struct* annot; } ; extern void* Cyc_Absyn_Wild_p;
extern const int Cyc_Absyn_Var_p_tag; struct Cyc_Absyn_Var_p_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; extern void* Cyc_Absyn_Null_p; extern const
int Cyc_Absyn_Int_p_tag; struct Cyc_Absyn_Int_p_struct{ int tag; void* f1; int
f2; } ; extern const int Cyc_Absyn_Char_p_tag; struct Cyc_Absyn_Char_p_struct{
int tag; char f1; } ; extern const int Cyc_Absyn_Float_p_tag; struct Cyc_Absyn_Float_p_struct{
int tag; struct _tagged_string f1; } ; extern const int Cyc_Absyn_Tuple_p_tag;
struct Cyc_Absyn_Tuple_p_struct{ int tag; struct Cyc_List_List* f1; } ; extern
const int Cyc_Absyn_Pointer_p_tag; struct Cyc_Absyn_Pointer_p_struct{ int tag;
struct Cyc_Absyn_Pat* f1; } ; extern const int Cyc_Absyn_Reference_p_tag; struct
Cyc_Absyn_Reference_p_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; extern
const int Cyc_Absyn_Struct_p_tag; struct Cyc_Absyn_Struct_p_struct{ int tag;
struct Cyc_Absyn_Structdecl* f1; struct Cyc_Core_Opt* f2; struct Cyc_List_List*
f3; struct Cyc_List_List* f4; } ; extern const int Cyc_Absyn_Enum_p_tag; struct
Cyc_Absyn_Enum_p_struct{ int tag; struct _tuple0* f1; struct Cyc_Core_Opt* f2;
struct Cyc_List_List* f3; struct Cyc_List_List* f4; struct Cyc_Absyn_Enumdecl*
f5; struct Cyc_Absyn_Enumfield* f6; } ; extern const int Cyc_Absyn_Xenum_p_tag;
struct Cyc_Absyn_Xenum_p_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Xenumdecl* f4; struct Cyc_Absyn_Enumfield*
f5; } ; extern const int Cyc_Absyn_UnknownId_p_tag; struct Cyc_Absyn_UnknownId_p_struct{
int tag; struct _tuple0* f1; } ; extern const int Cyc_Absyn_UnknownCall_p_tag;
struct Cyc_Absyn_UnknownCall_p_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; extern const int Cyc_Absyn_UnknownFields_p_tag;
struct Cyc_Absyn_UnknownFields_p_struct{ int tag; struct _tuple0* f1; struct Cyc_List_List*
f2; struct Cyc_List_List* f3; } ; struct Cyc_Absyn_Pat{ void* r; struct Cyc_Core_Opt*
topt; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_Switch_clause{
struct Cyc_Absyn_Pat* pattern; struct Cyc_Core_Opt* pat_vars; struct Cyc_Absyn_Exp*
where_clause; struct Cyc_Absyn_Stmt* body; struct Cyc_Position_Segment* loc; } ;
extern void* Cyc_Absyn_Unresolved_b; extern const int Cyc_Absyn_Global_b_tag;
struct Cyc_Absyn_Global_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
extern const int Cyc_Absyn_Funname_b_tag; struct Cyc_Absyn_Funname_b_struct{ int
tag; struct Cyc_Absyn_Fndecl* f1; } ; extern const int Cyc_Absyn_Param_b_tag;
struct Cyc_Absyn_Param_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
extern const int Cyc_Absyn_Local_b_tag; struct Cyc_Absyn_Local_b_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; extern const int Cyc_Absyn_Pat_b_tag; struct
Cyc_Absyn_Pat_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; struct Cyc_Absyn_Vardecl{
void* sc; struct _tuple0* name; struct Cyc_Absyn_Tqual* tq; void* type; struct
Cyc_Absyn_Exp* initializer; int shadow; struct Cyc_Core_Opt* region; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int is_inline; struct _tuple0*
name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* effect; void* ret_type;
struct Cyc_List_List* args; int varargs; struct Cyc_Absyn_Stmt* body; struct Cyc_Core_Opt*
cached_typ; struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structfield{ struct _tagged_string* name;
struct Cyc_Absyn_Tqual* tq; void* type; struct Cyc_List_List* attributes; } ;
struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List* attributes; } ; struct
Cyc_Absyn_Enumfield{ struct _tuple0* name; struct Cyc_Absyn_Exp* tag; struct Cyc_List_List*
tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment* loc; } ; struct
Cyc_Absyn_Enumdecl{ void* sc; struct Cyc_Core_Opt* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; } ; struct Cyc_Absyn_Xenumdecl{ void* sc;
struct _tuple0* name; struct Cyc_List_List* fields; } ; struct Cyc_Absyn_Typedefdecl{
struct _tuple0* name; struct Cyc_List_List* tvs; void* defn; } ; extern const
int Cyc_Absyn_Var_d_tag; struct Cyc_Absyn_Var_d_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; } ; extern const int Cyc_Absyn_Fn_d_tag; struct Cyc_Absyn_Fn_d_struct{ int
tag; struct Cyc_Absyn_Fndecl* f1; } ; extern const int Cyc_Absyn_Let_d_tag;
struct Cyc_Absyn_Let_d_struct{ int tag; struct Cyc_Absyn_Pat* f1; struct Cyc_Core_Opt*
f2; struct Cyc_Core_Opt* f3; struct Cyc_Absyn_Exp* f4; int f5; } ; extern const
int Cyc_Absyn_Struct_d_tag; struct Cyc_Absyn_Struct_d_struct{ int tag; struct
Cyc_Absyn_Structdecl* f1; } ; extern const int Cyc_Absyn_Union_d_tag; struct Cyc_Absyn_Union_d_struct{
int tag; struct Cyc_Absyn_Uniondecl* f1; } ; extern const int Cyc_Absyn_Enum_d_tag;
struct Cyc_Absyn_Enum_d_struct{ int tag; struct Cyc_Absyn_Enumdecl* f1; } ;
extern const int Cyc_Absyn_Xenum_d_tag; struct Cyc_Absyn_Xenum_d_struct{ int tag;
struct Cyc_Absyn_Xenumdecl* f1; } ; extern const int Cyc_Absyn_Typedef_d_tag;
struct Cyc_Absyn_Typedef_d_struct{ int tag; struct Cyc_Absyn_Typedefdecl* f1; }
; extern const int Cyc_Absyn_Namespace_d_tag; struct Cyc_Absyn_Namespace_d_struct{
int tag; struct _tagged_string* f1; struct Cyc_List_List* f2; } ; extern const
int Cyc_Absyn_Using_d_tag; struct Cyc_Absyn_Using_d_struct{ int tag; struct
_tuple0* f1; struct Cyc_List_List* f2; } ; extern const int Cyc_Absyn_ExternC_d_tag;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; extern const int
Cyc_Absyn_ArrayElement_tag; struct Cyc_Absyn_ArrayElement_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; extern const int Cyc_Absyn_FieldName_tag; struct
Cyc_Absyn_FieldName_struct{ int tag; struct _tagged_string* f1; } ; extern char
Cyc_Absyn_EmptyAnnot_tag[ 11u]; struct Cyc_Absyn_EmptyAnnot_struct{ char* tag; }
; extern struct Cyc_Absyn_Conref* Cyc_Absyn_compress_conref( struct Cyc_Absyn_Conref*
x); extern void* Cyc_Absyn_conref_val( struct Cyc_Absyn_Conref* x); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; typedef unsigned int Cyc_Stdio_size_t;
typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError_tag[ 14u];
struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1; } ;
extern char Cyc_Stdio_FileCloseError_tag[ 15u]; struct Cyc_Stdio_FileCloseError_struct{
char* tag; } ; struct Cyc_PP_Ppstate; typedef struct Cyc_PP_Ppstate* Cyc_PP_ppstate_t;
struct Cyc_PP_Out; typedef struct Cyc_PP_Out* Cyc_PP_out_t; struct Cyc_PP_Doc;
typedef struct Cyc_PP_Doc* Cyc_PP_doc_t; extern struct _tagged_string Cyc_Absynpp_typ2string(
void*); struct Cyc_Set_Set; typedef struct Cyc_Set_Set* Cyc_Set_gset_t; typedef
struct Cyc_Set_Set* Cyc_Set_hset_t; typedef struct Cyc_Set_Set* Cyc_Set_set_t;
extern char Cyc_Set_Absent_tag[ 7u]; struct Cyc_Set_Absent_struct{ char* tag; }
; struct Cyc_Dict_Dict; typedef struct Cyc_Dict_Dict* Cyc_Dict_hdict_t; typedef
struct Cyc_Dict_Dict* Cyc_Dict_dict_t; extern char Cyc_Dict_Present_tag[ 8u];
struct Cyc_Dict_Present_struct{ char* tag; } ; extern char Cyc_Dict_Absent_tag[
7u]; typedef void* Cyc_Tcenv_Resolved; extern const int Cyc_Tcenv_VarRes_tag;
struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; extern const int Cyc_Tcenv_StructRes_tag;
struct Cyc_Tcenv_StructRes_struct{ int tag; struct Cyc_Absyn_Structdecl* f1; } ;
extern const int Cyc_Tcenv_EnumRes_tag; struct Cyc_Tcenv_EnumRes_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ; extern const
int Cyc_Tcenv_XenumRes_tag; struct Cyc_Tcenv_XenumRes_struct{ int tag; struct
Cyc_Absyn_Xenumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ; typedef void* Cyc_Tcenv_resolved_t;
struct Cyc_Tcenv_Genv{ struct Cyc_Set_Set* namespaces; struct Cyc_Dict_Dict*
structdecls; struct Cyc_Dict_Dict* uniondecls; struct Cyc_Dict_Dict* enumdecls;
struct Cyc_Dict_Dict* xenumdecls; struct Cyc_Dict_Dict* typedefs; struct Cyc_Dict_Dict*
ordinaries; struct Cyc_List_List* availables; } ; typedef struct Cyc_Tcenv_Genv*
Cyc_Tcenv_genv_t; struct Cyc_Tcenv_Fenv; typedef struct Cyc_Tcenv_Fenv* Cyc_Tcenv_fenv_t;
typedef void* Cyc_Tcenv_Jumpee; extern void* Cyc_Tcenv_NotLoop_j; extern void*
Cyc_Tcenv_CaseEnd_j; extern void* Cyc_Tcenv_FnEnd_j; extern const int Cyc_Tcenv_Stmt_j_tag;
struct Cyc_Tcenv_Stmt_j_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; typedef
void* Cyc_Tcenv_jumpee_t; typedef void* Cyc_Tcenv_Frames; extern const int Cyc_Tcenv_Outermost_tag;
struct Cyc_Tcenv_Outermost_struct{ int tag; void* f1; } ; extern const int Cyc_Tcenv_Frame_tag;
struct Cyc_Tcenv_Frame_struct{ int tag; void* f1; void* f2; } ; extern const int
Cyc_Tcenv_Hidden_tag; struct Cyc_Tcenv_Hidden_struct{ int tag; void* f1; void*
f2; } ; typedef void* Cyc_Tcenv_frames; struct Cyc_Tcenv_Tenv{ struct Cyc_List_List*
ns; struct Cyc_Dict_Dict* ae; struct Cyc_Core_Opt* le; } ; typedef struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tenv; typedef struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tenv_t; extern char Cyc_Tcutil_TypeErr_tag[
8u]; struct Cyc_Tcutil_TypeErr_struct{ char* tag; } ; extern void* Cyc_Tcutil_impos(
struct _tagged_string); extern void Cyc_Tcutil_terr( struct Cyc_Position_Segment*,
struct _tagged_string); extern void* Cyc_Tcutil_compress( void* t); extern
unsigned int Cyc_Evexp_eval_const_uint_exp( struct Cyc_Absyn_Exp* e); extern
unsigned int Cyc_Evexp_szof( void* t, struct Cyc_Position_Segment* loc); void
Cyc_Evexp_exp_err( struct Cyc_Position_Segment* loc, struct _tagged_string msg){
Cyc_Position_post_error( Cyc_Position_mk_err_elab( loc,({ struct _tagged_string
_temp0= msg; xprintf("Exp_err: %.*s", _temp0.last_plus_one - _temp0.curr, _temp0.curr);})));}
extern void* Cyc_Evexp_promote_const( void* cn); extern void* Cyc_Evexp_eval_const_exp(
struct Cyc_Absyn_Exp* e); extern void* Cyc_Evexp_eval_const_unprimop( void* p,
struct Cyc_Absyn_Exp* e); extern void* Cyc_Evexp_eval_const_binprimop( void* p,
struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2); extern void* Cyc_Evexp_eval_sizeof(
void* t, struct Cyc_Position_Segment* loc); unsigned int Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp* e){ void* _temp1= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e)); int _temp7; void* _temp9; _LL3: if(( unsigned int) _temp1 > 1u?(( struct
_enum_struct*) _temp1)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL10: _temp9=( void*)((
struct Cyc_Absyn_Int_c_struct*) _temp1)->f1; goto _LL8; _LL8: _temp7=( int)((
struct Cyc_Absyn_Int_c_struct*) _temp1)->f2; goto _LL4;} else{ goto _LL5;} _LL5:
goto _LL6; _LL4: return( unsigned int) _temp7; _LL6: Cyc_Evexp_exp_err( e->loc,(
struct _tagged_string)({ char* _temp11=( char*)"expecting unsigned int"; struct
_tagged_string _temp12; _temp12.curr= _temp11; _temp12.base= _temp11; _temp12.last_plus_one=
_temp11 + 23; _temp12;})); return 0u; _LL2:;} void* Cyc_Evexp_promote_const(
void* cn){ void* _temp13= cn; char _temp21; void* _temp23; short _temp25; void*
_temp27; _LL15: if(( unsigned int) _temp13 > 1u?(( struct _enum_struct*) _temp13)->tag
== Cyc_Absyn_Char_c_tag: 0){ _LL24: _temp23=( void*)(( struct Cyc_Absyn_Char_c_struct*)
_temp13)->f1; goto _LL22; _LL22: _temp21=( char)(( struct Cyc_Absyn_Char_c_struct*)
_temp13)->f2; goto _LL16;} else{ goto _LL17;} _LL17: if(( unsigned int) _temp13
> 1u?(( struct _enum_struct*) _temp13)->tag == Cyc_Absyn_Short_c_tag: 0){ _LL28:
_temp27=( void*)(( struct Cyc_Absyn_Short_c_struct*) _temp13)->f1; goto _LL26;
_LL26: _temp25=( short)(( struct Cyc_Absyn_Short_c_struct*) _temp13)->f2; goto
_LL18;} else{ goto _LL19;} _LL19: goto _LL20; _LL16: return({ struct Cyc_Absyn_Int_c_struct*
_temp29=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp29->tag= Cyc_Absyn_Int_c_tag; _temp29->f1=( void*) _temp23; _temp29->f2=(
int) _temp21;( void*) _temp29;}); _LL18: return({ struct Cyc_Absyn_Int_c_struct*
_temp30=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp30->tag= Cyc_Absyn_Int_c_tag; _temp30->f1=( void*) _temp27; _temp30->f2=(
int) _temp25;( void*) _temp30;}); _LL20: return cn; _LL14:;} int Cyc_Evexp_eval_const_bool_exp(
struct Cyc_Absyn_Exp* e){ void* _temp31= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e)); int _temp41; void* _temp43; long long _temp45; void* _temp47; _LL33: if((
unsigned int) _temp31 > 1u?(( struct _enum_struct*) _temp31)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL44: _temp43=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp31)->f1; goto
_LL42; _LL42: _temp41=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp31)->f2;
goto _LL34;} else{ goto _LL35;} _LL35: if(( unsigned int) _temp31 > 1u?(( struct
_enum_struct*) _temp31)->tag == Cyc_Absyn_LongLong_c_tag: 0){ _LL48: _temp47=(
void*)(( struct Cyc_Absyn_LongLong_c_struct*) _temp31)->f1; goto _LL46; _LL46:
_temp45=( long long)(( struct Cyc_Absyn_LongLong_c_struct*) _temp31)->f2; goto
_LL36;} else{ goto _LL37;} _LL37: if( _temp31 == Cyc_Absyn_Null_c){ goto _LL38;}
else{ goto _LL39;} _LL39: goto _LL40; _LL34: return _temp41 != 0; _LL36: return
_temp45 !=( long long) 0; _LL38: return 0; _LL40: Cyc_Evexp_exp_err( e->loc,(
struct _tagged_string)({ char* _temp49=( char*)"expecting bool"; struct
_tagged_string _temp50; _temp50.curr= _temp49; _temp50.base= _temp49; _temp50.last_plus_one=
_temp49 + 15; _temp50;})); return 0; _LL32:;} void* Cyc_Evexp_eval_const_exp(
struct Cyc_Absyn_Exp* e){ void* cn;{ void* _temp51=( void*) e->r; void* _temp67;
struct Cyc_Absyn_Exp* _temp69; struct Cyc_Absyn_Exp* _temp71; struct Cyc_Absyn_Exp*
_temp73; struct Cyc_List_List* _temp75; void* _temp77; void* _temp79; struct Cyc_Absyn_Exp*
_temp81; struct Cyc_Absyn_Exp* _temp83; void* _temp85; _LL53: if((( struct
_enum_struct*) _temp51)->tag == Cyc_Absyn_Const_e_tag){ _LL68: _temp67=( void*)((
struct Cyc_Absyn_Const_e_struct*) _temp51)->f1; goto _LL54;} else{ goto _LL55;}
_LL55: if((( struct _enum_struct*) _temp51)->tag == Cyc_Absyn_Conditional_e_tag){
_LL74: _temp73=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp51)->f1; goto _LL72; _LL72: _temp71=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp51)->f2; goto _LL70; _LL70: _temp69=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Conditional_e_struct*)
_temp51)->f3; goto _LL56;} else{ goto _LL57;} _LL57: if((( struct _enum_struct*)
_temp51)->tag == Cyc_Absyn_Primop_e_tag){ _LL78: _temp77=( void*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp51)->f1; goto _LL76; _LL76: _temp75=( struct Cyc_List_List*)(( struct Cyc_Absyn_Primop_e_struct*)
_temp51)->f2; goto _LL58;} else{ goto _LL59;} _LL59: if((( struct _enum_struct*)
_temp51)->tag == Cyc_Absyn_Sizeoftyp_e_tag){ _LL80: _temp79=( void*)(( struct
Cyc_Absyn_Sizeoftyp_e_struct*) _temp51)->f1; goto _LL60;} else{ goto _LL61;}
_LL61: if((( struct _enum_struct*) _temp51)->tag == Cyc_Absyn_Sizeofexp_e_tag){
_LL82: _temp81=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Sizeofexp_e_struct*)
_temp51)->f1; goto _LL62;} else{ goto _LL63;} _LL63: if((( struct _enum_struct*)
_temp51)->tag == Cyc_Absyn_Cast_e_tag){ _LL86: _temp85=( void*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp51)->f1; goto _LL84; _LL84: _temp83=( struct Cyc_Absyn_Exp*)(( struct Cyc_Absyn_Cast_e_struct*)
_temp51)->f2; goto _LL64;} else{ goto _LL65;} _LL65: goto _LL66; _LL54: return
_temp67; _LL56: cn= Cyc_Evexp_eval_const_bool_exp( _temp73)? Cyc_Evexp_eval_const_exp(
_temp71): Cyc_Evexp_eval_const_exp( _temp69); goto _LL52; _LL58: if( _temp75 ==
0){ Cyc_Evexp_exp_err( e->loc,( struct _tagged_string)({ char* _temp87=( char*)"bad static expression (no args to primop)";
struct _tagged_string _temp88; _temp88.curr= _temp87; _temp88.base= _temp87;
_temp88.last_plus_one= _temp87 + 42; _temp88;})); return({ struct Cyc_Absyn_Int_c_struct*
_temp89=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp89->tag= Cyc_Absyn_Int_c_tag; _temp89->f1=( void*) Cyc_Absyn_Signed;
_temp89->f2= 0;( void*) _temp89;});} if( _temp75->tl == 0){ cn= Cyc_Evexp_eval_const_unprimop(
_temp77,( struct Cyc_Absyn_Exp*) _temp75->hd);} else{ if(( _temp75->tl)->tl != 0){
Cyc_Evexp_exp_err( e->loc,( struct _tagged_string)({ char* _temp90=( char*)"bad static expression (too many args to primop)";
struct _tagged_string _temp91; _temp91.curr= _temp90; _temp91.base= _temp90;
_temp91.last_plus_one= _temp90 + 48; _temp91;})); return({ struct Cyc_Absyn_Int_c_struct*
_temp92=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp92->tag= Cyc_Absyn_Int_c_tag; _temp92->f1=( void*) Cyc_Absyn_Signed;
_temp92->f2= 0;( void*) _temp92;});} else{ cn= Cyc_Evexp_eval_const_binprimop(
_temp77,( struct Cyc_Absyn_Exp*) _temp75->hd,( struct Cyc_Absyn_Exp*)( _temp75->tl)->hd);}}
goto _LL52; _LL60: cn= Cyc_Evexp_eval_sizeof( _temp79, e->loc); goto _LL52;
_LL62: cn= Cyc_Evexp_eval_sizeof(( void*)( _temp81->topt)->v, _temp81->loc);
goto _LL52; _LL64: cn= Cyc_Evexp_eval_const_exp( _temp83);{ struct _tuple3
_temp94=({ struct _tuple3 _temp93; _temp93.f1= _temp85; _temp93.f2= cn; _temp93;});
void* _temp100; int _temp102; void* _temp104; void* _temp106; void* _temp108;
void* _temp110; _LL96: _LL107: _temp106= _temp94.f1; if(( unsigned int) _temp106
> 4u?(( struct _enum_struct*) _temp106)->tag == Cyc_Absyn_IntType_tag: 0){
_LL111: _temp110=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp106)->f1;
goto _LL109; _LL109: _temp108=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp106)->f2; goto _LL101;} else{ goto _LL98;} _LL101: _temp100= _temp94.f2;
if(( unsigned int) _temp100 > 1u?(( struct _enum_struct*) _temp100)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL105: _temp104=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp100)->f1;
goto _LL103; _LL103: _temp102=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp100)->f2;
goto _LL97;} else{ goto _LL98;} _LL98: goto _LL99; _LL97: if( _temp110 !=
_temp104){ cn=({ struct Cyc_Absyn_Int_c_struct* _temp112=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp112->tag= Cyc_Absyn_Int_c_tag;
_temp112->f1=( void*) _temp110; _temp112->f2= _temp102;( void*) _temp112;});}
goto _LL95; _LL99: Cyc_Evexp_exp_err( e->loc,({ struct _tagged_string _temp113=
Cyc_Absynpp_typ2string( _temp85); xprintf("eval_const: cannot cast to %.*s",
_temp113.last_plus_one - _temp113.curr, _temp113.curr);})); return({ struct Cyc_Absyn_Int_c_struct*
_temp114=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp114->tag= Cyc_Absyn_Int_c_tag; _temp114->f1=( void*) Cyc_Absyn_Signed;
_temp114->f2= 0;( void*) _temp114;}); _LL95:;} goto _LL52; _LL66: Cyc_Evexp_exp_err(
e->loc,( struct _tagged_string)({ char* _temp115=( char*)"bad static expression";
struct _tagged_string _temp116; _temp116.curr= _temp115; _temp116.base= _temp115;
_temp116.last_plus_one= _temp115 + 22; _temp116;})); return({ struct Cyc_Absyn_Int_c_struct*
_temp117=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp117->tag= Cyc_Absyn_Int_c_tag; _temp117->f1=( void*) Cyc_Absyn_Signed;
_temp117->f2= 0;( void*) _temp117;}); _LL52:;}( void*)( e->r=( void*)({ struct
Cyc_Absyn_Const_e_struct* _temp118=( struct Cyc_Absyn_Const_e_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Const_e_struct)); _temp118->tag= Cyc_Absyn_Const_e_tag;
_temp118->f1=( void*) cn;( void*) _temp118;})); return cn;} unsigned int Cyc_Evexp_szof(
void* t, struct Cyc_Position_Segment* loc){ void* _temp119= Cyc_Tcutil_compress(
t); struct Cyc_Absyn_Tvar* _temp167; int _temp169; struct Cyc_Core_Opt* _temp171;
void* _temp173; int _temp175; struct Cyc_Core_Opt* _temp177; void* _temp179;
struct Cyc_Absyn_Enumdecl** _temp181; struct Cyc_List_List* _temp183; struct
_tuple0* _temp185; struct Cyc_Absyn_Xenumdecl** _temp187; struct _tuple0*
_temp189; struct Cyc_Absyn_PtrInfo _temp191; struct Cyc_Absyn_Conref* _temp193;
struct Cyc_Absyn_Tqual* _temp195; struct Cyc_Absyn_Conref* _temp197; void*
_temp199; void* _temp201; void* _temp203; void* _temp205; void* _temp207; void*
_temp209; void* _temp211; void* _temp213; void* _temp215; void* _temp217; struct
Cyc_Absyn_Exp* _temp219; struct Cyc_Absyn_Tqual* _temp221; void* _temp223;
struct Cyc_Absyn_FnInfo _temp225; struct Cyc_List_List* _temp227; void* _temp229;
struct Cyc_Absyn_Structdecl** _temp231; struct Cyc_List_List* _temp233; struct
_tuple0* _temp235; struct Cyc_Absyn_Uniondecl** _temp237; struct Cyc_List_List*
_temp239; struct _tuple0* _temp241; struct Cyc_Core_Opt* _temp243; struct Cyc_List_List*
_temp245; struct _tuple0* _temp247; struct Cyc_List_List* _temp249; void*
_temp251; _LL121: if( _temp119 == Cyc_Absyn_VoidType){ goto _LL122;} else{ goto
_LL123;} _LL123: if(( unsigned int) _temp119 > 4u?(( struct _enum_struct*)
_temp119)->tag == Cyc_Absyn_VarType_tag: 0){ _LL168: _temp167=( struct Cyc_Absyn_Tvar*)((
struct Cyc_Absyn_VarType_struct*) _temp119)->f1; goto _LL124;} else{ goto _LL125;}
_LL125: if(( unsigned int) _temp119 > 4u?(( struct _enum_struct*) _temp119)->tag
== Cyc_Absyn_Evar_tag: 0){ _LL174: _temp173=( void*)(( struct Cyc_Absyn_Evar_struct*)
_temp119)->f1; if( _temp173 == Cyc_Absyn_BoxKind){ goto _LL172;} else{ goto
_LL127;} _LL172: _temp171=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp119)->f2; goto _LL170; _LL170: _temp169=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp119)->f3; goto _LL126;} else{ goto _LL127;} _LL127: if(( unsigned int)
_temp119 > 4u?(( struct _enum_struct*) _temp119)->tag == Cyc_Absyn_Evar_tag: 0){
_LL180: _temp179=( void*)(( struct Cyc_Absyn_Evar_struct*) _temp119)->f1; goto
_LL178; _LL178: _temp177=( struct Cyc_Core_Opt*)(( struct Cyc_Absyn_Evar_struct*)
_temp119)->f2; goto _LL176; _LL176: _temp175=( int)(( struct Cyc_Absyn_Evar_struct*)
_temp119)->f3; goto _LL128;} else{ goto _LL129;} _LL129: if(( unsigned int)
_temp119 > 4u?(( struct _enum_struct*) _temp119)->tag == Cyc_Absyn_EnumType_tag:
0){ _LL186: _temp185=( struct _tuple0*)(( struct Cyc_Absyn_EnumType_struct*)
_temp119)->f1; goto _LL184; _LL184: _temp183=( struct Cyc_List_List*)(( struct
Cyc_Absyn_EnumType_struct*) _temp119)->f2; goto _LL182; _LL182: _temp181=(
struct Cyc_Absyn_Enumdecl**)(( struct Cyc_Absyn_EnumType_struct*) _temp119)->f3;
goto _LL130;} else{ goto _LL131;} _LL131: if(( unsigned int) _temp119 > 4u?((
struct _enum_struct*) _temp119)->tag == Cyc_Absyn_XenumType_tag: 0){ _LL190:
_temp189=( struct _tuple0*)(( struct Cyc_Absyn_XenumType_struct*) _temp119)->f1;
goto _LL188; _LL188: _temp187=( struct Cyc_Absyn_Xenumdecl**)(( struct Cyc_Absyn_XenumType_struct*)
_temp119)->f2; goto _LL132;} else{ goto _LL133;} _LL133: if(( unsigned int)
_temp119 > 4u?(( struct _enum_struct*) _temp119)->tag == Cyc_Absyn_PointerType_tag:
0){ _LL192: _temp191=( struct Cyc_Absyn_PtrInfo)(( struct Cyc_Absyn_PointerType_struct*)
_temp119)->f1; _LL202: _temp201=( void*) _temp191.elt_typ; goto _LL200; _LL200:
_temp199=( void*) _temp191.rgn_typ; goto _LL198; _LL198: _temp197=( struct Cyc_Absyn_Conref*)
_temp191.nullable; goto _LL196; _LL196: _temp195=( struct Cyc_Absyn_Tqual*)
_temp191.tq; goto _LL194; _LL194: _temp193=( struct Cyc_Absyn_Conref*) _temp191.bounds;
goto _LL134;} else{ goto _LL135;} _LL135: if(( unsigned int) _temp119 > 4u?((
struct _enum_struct*) _temp119)->tag == Cyc_Absyn_IntType_tag: 0){ _LL206:
_temp205=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp119)->f1; goto _LL204;
_LL204: _temp203=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp119)->f2; if(
_temp203 == Cyc_Absyn_B1){ goto _LL136;} else{ goto _LL137;}} else{ goto _LL137;}
_LL137: if(( unsigned int) _temp119 > 4u?(( struct _enum_struct*) _temp119)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL210: _temp209=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f1; goto _LL208; _LL208: _temp207=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f2; if( _temp207 == Cyc_Absyn_B2){ goto _LL138;} else{ goto _LL139;}}
else{ goto _LL139;} _LL139: if(( unsigned int) _temp119 > 4u?(( struct
_enum_struct*) _temp119)->tag == Cyc_Absyn_IntType_tag: 0){ _LL214: _temp213=(
void*)(( struct Cyc_Absyn_IntType_struct*) _temp119)->f1; goto _LL212; _LL212:
_temp211=( void*)(( struct Cyc_Absyn_IntType_struct*) _temp119)->f2; if(
_temp211 == Cyc_Absyn_B4){ goto _LL140;} else{ goto _LL141;}} else{ goto _LL141;}
_LL141: if(( unsigned int) _temp119 > 4u?(( struct _enum_struct*) _temp119)->tag
== Cyc_Absyn_IntType_tag: 0){ _LL218: _temp217=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f1; goto _LL216; _LL216: _temp215=( void*)(( struct Cyc_Absyn_IntType_struct*)
_temp119)->f2; if( _temp215 == Cyc_Absyn_B8){ goto _LL142;} else{ goto _LL143;}}
else{ goto _LL143;} _LL143: if( _temp119 == Cyc_Absyn_FloatType){ goto _LL144;}
else{ goto _LL145;} _LL145: if( _temp119 == Cyc_Absyn_DoubleType){ goto _LL146;}
else{ goto _LL147;} _LL147: if(( unsigned int) _temp119 > 4u?(( struct
_enum_struct*) _temp119)->tag == Cyc_Absyn_ArrayType_tag: 0){ _LL224: _temp223=(
void*)(( struct Cyc_Absyn_ArrayType_struct*) _temp119)->f1; goto _LL222; _LL222:
_temp221=( struct Cyc_Absyn_Tqual*)(( struct Cyc_Absyn_ArrayType_struct*)
_temp119)->f2; goto _LL220; _LL220: _temp219=( struct Cyc_Absyn_Exp*)(( struct
Cyc_Absyn_ArrayType_struct*) _temp119)->f3; goto _LL148;} else{ goto _LL149;}
_LL149: if(( unsigned int) _temp119 > 4u?(( struct _enum_struct*) _temp119)->tag
== Cyc_Absyn_FnType_tag: 0){ _LL226: _temp225=( struct Cyc_Absyn_FnInfo)((
struct Cyc_Absyn_FnType_struct*) _temp119)->f1; goto _LL150;} else{ goto _LL151;}
_LL151: if(( unsigned int) _temp119 > 4u?(( struct _enum_struct*) _temp119)->tag
== Cyc_Absyn_TupleType_tag: 0){ _LL228: _temp227=( struct Cyc_List_List*)((
struct Cyc_Absyn_TupleType_struct*) _temp119)->f1; goto _LL152;} else{ goto
_LL153;} _LL153: if(( unsigned int) _temp119 > 4u?(( struct _enum_struct*)
_temp119)->tag == Cyc_Absyn_RgnHandleType_tag: 0){ _LL230: _temp229=( void*)((
struct Cyc_Absyn_RgnHandleType_struct*) _temp119)->f1; goto _LL154;} else{ goto
_LL155;} _LL155: if(( unsigned int) _temp119 > 4u?(( struct _enum_struct*)
_temp119)->tag == Cyc_Absyn_StructType_tag: 0){ _LL236: _temp235=( struct
_tuple0*)(( struct Cyc_Absyn_StructType_struct*) _temp119)->f1; goto _LL234;
_LL234: _temp233=( struct Cyc_List_List*)(( struct Cyc_Absyn_StructType_struct*)
_temp119)->f2; goto _LL232; _LL232: _temp231=( struct Cyc_Absyn_Structdecl**)((
struct Cyc_Absyn_StructType_struct*) _temp119)->f3; goto _LL156;} else{ goto
_LL157;} _LL157: if(( unsigned int) _temp119 > 4u?(( struct _enum_struct*)
_temp119)->tag == Cyc_Absyn_UnionType_tag: 0){ _LL242: _temp241=( struct _tuple0*)((
struct Cyc_Absyn_UnionType_struct*) _temp119)->f1; goto _LL240; _LL240: _temp239=(
struct Cyc_List_List*)(( struct Cyc_Absyn_UnionType_struct*) _temp119)->f2; goto
_LL238; _LL238: _temp237=( struct Cyc_Absyn_Uniondecl**)(( struct Cyc_Absyn_UnionType_struct*)
_temp119)->f3; goto _LL158;} else{ goto _LL159;} _LL159: if( _temp119 == Cyc_Absyn_HeapRgn){
goto _LL160;} else{ goto _LL161;} _LL161: if(( unsigned int) _temp119 > 4u?((
struct _enum_struct*) _temp119)->tag == Cyc_Absyn_TypedefType_tag: 0){ _LL248:
_temp247=( struct _tuple0*)(( struct Cyc_Absyn_TypedefType_struct*) _temp119)->f1;
goto _LL246; _LL246: _temp245=( struct Cyc_List_List*)(( struct Cyc_Absyn_TypedefType_struct*)
_temp119)->f2; goto _LL244; _LL244: _temp243=( struct Cyc_Core_Opt*)(( struct
Cyc_Absyn_TypedefType_struct*) _temp119)->f3; goto _LL162;} else{ goto _LL163;}
_LL163: if(( unsigned int) _temp119 > 4u?(( struct _enum_struct*) _temp119)->tag
== Cyc_Absyn_JoinEff_tag: 0){ _LL250: _temp249=( struct Cyc_List_List*)(( struct
Cyc_Absyn_JoinEff_struct*) _temp119)->f1; goto _LL164;} else{ goto _LL165;}
_LL165: if(( unsigned int) _temp119 > 4u?(( struct _enum_struct*) _temp119)->tag
== Cyc_Absyn_AccessEff_tag: 0){ _LL252: _temp251=( void*)(( struct Cyc_Absyn_AccessEff_struct*)
_temp119)->f1; goto _LL166;} else{ goto _LL120;} _LL122: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp253=( char*)"cannot apply sizeof to void type";
struct _tagged_string _temp254; _temp254.curr= _temp253; _temp254.base= _temp253;
_temp254.last_plus_one= _temp253 + 33; _temp254;})); return 0u; _LL124: { void*
_temp255=(( void*(*)( struct Cyc_Absyn_Conref* x)) Cyc_Absyn_conref_val)(
_temp167->kind); _LL257: if( _temp255 == Cyc_Absyn_BoxKind){ goto _LL258;} else{
goto _LL259;} _LL259: if( _temp255 == Cyc_Absyn_AnyKind){ goto _LL260;} else{
goto _LL261;} _LL261: if( _temp255 == Cyc_Absyn_MemKind){ goto _LL262;} else{
goto _LL263;} _LL263: if( _temp255 == Cyc_Absyn_RgnKind){ goto _LL264;} else{
goto _LL265;} _LL265: if( _temp255 == Cyc_Absyn_EffKind){ goto _LL266;} else{
goto _LL256;} _LL258: return 4u; _LL260: goto _LL262; _LL262: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp267=( char*)"cannot apply sizeof to a non-boxed type variable";
struct _tagged_string _temp268; _temp268.curr= _temp267; _temp268.base= _temp267;
_temp268.last_plus_one= _temp267 + 49; _temp268;})); return 0u; _LL264: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp269=( char*)"cannot apply sizeof to a region";
struct _tagged_string _temp270; _temp270.curr= _temp269; _temp270.base= _temp269;
_temp270.last_plus_one= _temp269 + 32; _temp270;})); return 0u; _LL266: Cyc_Tcutil_terr(
loc,( struct _tagged_string)({ char* _temp271=( char*)"cannot apply sizeof to an effect";
struct _tagged_string _temp272; _temp272.curr= _temp271; _temp272.base= _temp271;
_temp272.last_plus_one= _temp271 + 33; _temp272;})); return 0u; _LL256:;} _LL126:
return 4u; _LL128: Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char*
_temp273=( char*)"cannot apply sizeof to unknown type"; struct _tagged_string
_temp274; _temp274.curr= _temp273; _temp274.base= _temp273; _temp274.last_plus_one=
_temp273 + 36; _temp274;})); return 0u; _LL130: return 4u; _LL132: return 4u;
_LL134: { void* _temp275=( void*)((( struct Cyc_Absyn_Conref*(*)( struct Cyc_Absyn_Conref*
x)) Cyc_Absyn_compress_conref)( _temp193))->v; void* _temp283; void* _temp285;
_LL277: if(( unsigned int) _temp275 > 1u?(( struct _enum_struct*) _temp275)->tag
== Cyc_Absyn_Eq_constr_tag: 0){ _LL284: _temp283=( void*)(( struct Cyc_Absyn_Eq_constr_struct*)
_temp275)->f1; if( _temp283 == Cyc_Absyn_Unknown_b){ goto _LL278;} else{ goto
_LL279;}} else{ goto _LL279;} _LL279: if(( unsigned int) _temp275 > 1u?(( struct
_enum_struct*) _temp275)->tag == Cyc_Absyn_Eq_constr_tag: 0){ _LL286: _temp285=(
void*)(( struct Cyc_Absyn_Eq_constr_struct*) _temp275)->f1; goto _LL280;} else{
goto _LL281;} _LL281: goto _LL282; _LL278: return 12u; _LL280: return 4u; _LL282:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp287=( char*)"bounds information not known for pointer type";
struct _tagged_string _temp288; _temp288.curr= _temp287; _temp288.base= _temp287;
_temp288.last_plus_one= _temp287 + 46; _temp288;})); return 0u; _LL276:;} _LL136:
return 1u; _LL138: return 1u; _LL140: return 1u; _LL142: return 1u; _LL144:
return 4u; _LL146: return 8u; _LL148: return Cyc_Evexp_szof( _temp223, loc) *
Cyc_Evexp_eval_const_uint_exp( _temp219); _LL150: Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp289=( char*)"cannot apply sizeof to function type";
struct _tagged_string _temp290; _temp290.curr= _temp289; _temp290.base= _temp289;
_temp290.last_plus_one= _temp289 + 37; _temp290;})); return 0u; _LL152: {
unsigned int s= 0; for( 0; _temp227 != 0; _temp227= _temp227->tl){ s += Cyc_Evexp_szof((*((
struct _tuple4*) _temp227->hd)).f2, loc);} return s;} _LL154: return 4u; _LL156:
if( _temp231 == 0){ return(( unsigned int(*)( struct _tagged_string)) Cyc_Tcutil_impos)((
struct _tagged_string)({ char* _temp291=( char*)"szof on unchecked StructType";
struct _tagged_string _temp292; _temp292.curr= _temp291; _temp292.base= _temp291;
_temp292.last_plus_one= _temp291 + 29; _temp292;}));}{ struct Cyc_Absyn_Structdecl*
sd=* _temp231; if( sd->fields == 0){ Cyc_Tcutil_terr( loc,( struct
_tagged_string)({ char* _temp293=( char*)"cannot apply sizeof to abstract struct type";
struct _tagged_string _temp294; _temp294.curr= _temp293; _temp294.base= _temp293;
_temp294.last_plus_one= _temp293 + 44; _temp294;}));}{ unsigned int s= 0;{
struct Cyc_List_List* fs=( struct Cyc_List_List*)( sd->fields)->v; for( 0; fs !=
0; fs= fs->tl){ s += Cyc_Evexp_szof(( void*)(( struct Cyc_Absyn_Structfield*) fs->hd)->type,
loc);}} return s;}} _LL158: if( _temp237 == 0){ return(( unsigned int(*)( struct
_tagged_string)) Cyc_Tcutil_impos)(( struct _tagged_string)({ char* _temp295=(
char*)"szof on unchecked UnionType"; struct _tagged_string _temp296; _temp296.curr=
_temp295; _temp296.base= _temp295; _temp296.last_plus_one= _temp295 + 28;
_temp296;}));}{ struct Cyc_Absyn_Uniondecl* ud=* _temp237; if( ud->fields == 0){
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp297=( char*)"cannot apply sizeof to abstract union type";
struct _tagged_string _temp298; _temp298.curr= _temp297; _temp298.base= _temp297;
_temp298.last_plus_one= _temp297 + 43; _temp298;}));}{ unsigned int s= 0;{
struct Cyc_List_List* fs=( struct Cyc_List_List*)( ud->fields)->v; for( 0; fs !=
0; fs= fs->tl){ unsigned int i= Cyc_Evexp_szof(( void*)(( struct Cyc_Absyn_Structfield*)
fs->hd)->type, loc); s= s > i? s: i;}} return s;}} _LL160: Cyc_Tcutil_terr( loc,(
struct _tagged_string)({ char* _temp299=( char*)"cannot apply sizoef to a region";
struct _tagged_string _temp300; _temp300.curr= _temp299; _temp300.base= _temp299;
_temp300.last_plus_one= _temp299 + 32; _temp300;})); return 0u; _LL162: return((
unsigned int(*)( struct _tagged_string)) Cyc_Tcutil_impos)(( struct
_tagged_string)({ char* _temp301=( char*)"szof typedeftype"; struct
_tagged_string _temp302; _temp302.curr= _temp301; _temp302.base= _temp301;
_temp302.last_plus_one= _temp301 + 17; _temp302;})); _LL164: goto _LL166; _LL166:
Cyc_Tcutil_terr( loc,( struct _tagged_string)({ char* _temp303=( char*)"cannot apply sizoef to an effect";
struct _tagged_string _temp304; _temp304.curr= _temp303; _temp304.base= _temp303;
_temp304.last_plus_one= _temp303 + 33; _temp304;})); return 0u; _LL120:;} void*
Cyc_Evexp_eval_sizeof( void* t, struct Cyc_Position_Segment* loc){ return({
struct Cyc_Absyn_Int_c_struct* _temp305=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp305->tag= Cyc_Absyn_Int_c_tag;
_temp305->f1=( void*) Cyc_Absyn_Unsigned; _temp305->f2=( int) Cyc_Evexp_szof( t,
loc);( void*) _temp305;});} int Cyc_Evexp_is_arith_const( void* cn){ void*
_temp306= cn; char _temp316; void* _temp318; int _temp320; void* _temp322;
struct _tagged_string _temp324; _LL308: if(( unsigned int) _temp306 > 1u?((
struct _enum_struct*) _temp306)->tag == Cyc_Absyn_Char_c_tag: 0){ _LL319:
_temp318=( void*)(( struct Cyc_Absyn_Char_c_struct*) _temp306)->f1; goto _LL317;
_LL317: _temp316=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp306)->f2; goto
_LL309;} else{ goto _LL310;} _LL310: if(( unsigned int) _temp306 > 1u?(( struct
_enum_struct*) _temp306)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL323: _temp322=(
void*)(( struct Cyc_Absyn_Int_c_struct*) _temp306)->f1; goto _LL321; _LL321:
_temp320=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp306)->f2; goto _LL311;}
else{ goto _LL312;} _LL312: if(( unsigned int) _temp306 > 1u?(( struct
_enum_struct*) _temp306)->tag == Cyc_Absyn_Float_c_tag: 0){ _LL325: _temp324=(
struct _tagged_string)(( struct Cyc_Absyn_Float_c_struct*) _temp306)->f1; goto
_LL313;} else{ goto _LL314;} _LL314: goto _LL315; _LL309: return 1; _LL311:
return 1; _LL313: return 1; _LL315: return 0; _LL307:;} void* Cyc_Evexp_eval_const_unprimop(
void* p, struct Cyc_Absyn_Exp* e){ void* cn= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e)); struct _tuple3 _temp327=({ struct _tuple3 _temp326; _temp326.f1= p;
_temp326.f2= cn; _temp326;}); void* _temp345; void* _temp347; void* _temp349;
int _temp351; void* _temp353; void* _temp355; void* _temp357; short _temp359;
void* _temp361; void* _temp363; void* _temp365; char _temp367; void* _temp369;
void* _temp371; void* _temp373; int _temp375; void* _temp377; void* _temp379;
void* _temp381; void* _temp383; void* _temp385; int _temp387; void* _temp389;
void* _temp391; _LL329: _LL348: _temp347= _temp327.f1; if( _temp347 == Cyc_Absyn_Plus){
goto _LL346;} else{ goto _LL331;} _LL346: _temp345= _temp327.f2; goto _LL330;
_LL331: _LL356: _temp355= _temp327.f1; if( _temp355 == Cyc_Absyn_Minus){ goto
_LL350;} else{ goto _LL333;} _LL350: _temp349= _temp327.f2; if(( unsigned int)
_temp349 > 1u?(( struct _enum_struct*) _temp349)->tag == Cyc_Absyn_Int_c_tag: 0){
_LL354: _temp353=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp349)->f1; goto
_LL352; _LL352: _temp351=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp349)->f2;
goto _LL332;} else{ goto _LL333;} _LL333: _LL364: _temp363= _temp327.f1; if(
_temp363 == Cyc_Absyn_Minus){ goto _LL358;} else{ goto _LL335;} _LL358: _temp357=
_temp327.f2; if(( unsigned int) _temp357 > 1u?(( struct _enum_struct*) _temp357)->tag
== Cyc_Absyn_Short_c_tag: 0){ _LL362: _temp361=( void*)(( struct Cyc_Absyn_Short_c_struct*)
_temp357)->f1; goto _LL360; _LL360: _temp359=( short)(( struct Cyc_Absyn_Short_c_struct*)
_temp357)->f2; goto _LL334;} else{ goto _LL335;} _LL335: _LL372: _temp371=
_temp327.f1; if( _temp371 == Cyc_Absyn_Minus){ goto _LL366;} else{ goto _LL337;}
_LL366: _temp365= _temp327.f2; if(( unsigned int) _temp365 > 1u?(( struct
_enum_struct*) _temp365)->tag == Cyc_Absyn_Char_c_tag: 0){ _LL370: _temp369=(
void*)(( struct Cyc_Absyn_Char_c_struct*) _temp365)->f1; goto _LL368; _LL368:
_temp367=( char)(( struct Cyc_Absyn_Char_c_struct*) _temp365)->f2; goto _LL336;}
else{ goto _LL337;} _LL337: _LL380: _temp379= _temp327.f1; if( _temp379 == Cyc_Absyn_Not){
goto _LL374;} else{ goto _LL339;} _LL374: _temp373= _temp327.f2; if((
unsigned int) _temp373 > 1u?(( struct _enum_struct*) _temp373)->tag == Cyc_Absyn_Int_c_tag:
0){ _LL378: _temp377=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp373)->f1;
goto _LL376; _LL376: _temp375=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp373)->f2;
goto _LL338;} else{ goto _LL339;} _LL339: _LL384: _temp383= _temp327.f1; if(
_temp383 == Cyc_Absyn_Not){ goto _LL382;} else{ goto _LL341;} _LL382: _temp381=
_temp327.f2; if( _temp381 == Cyc_Absyn_Null_c){ goto _LL340;} else{ goto _LL341;}
_LL341: _LL392: _temp391= _temp327.f1; if( _temp391 == Cyc_Absyn_Bitnot){ goto
_LL386;} else{ goto _LL343;} _LL386: _temp385= _temp327.f2; if(( unsigned int)
_temp385 > 1u?(( struct _enum_struct*) _temp385)->tag == Cyc_Absyn_Int_c_tag: 0){
_LL390: _temp389=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp385)->f1; goto
_LL388; _LL388: _temp387=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp385)->f2;
goto _LL342;} else{ goto _LL343;} _LL343: goto _LL344; _LL330: if( ! Cyc_Evexp_is_arith_const(
cn)){ Cyc_Evexp_exp_err( e->loc,( struct _tagged_string)({ char* _temp393=( char*)"expecting arithmetic constant";
struct _tagged_string _temp394; _temp394.curr= _temp393; _temp394.base= _temp393;
_temp394.last_plus_one= _temp393 + 30; _temp394;}));} return cn; _LL332: return({
struct Cyc_Absyn_Int_c_struct* _temp395=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp395->tag= Cyc_Absyn_Int_c_tag;
_temp395->f1=( void*) Cyc_Absyn_Signed; _temp395->f2= - _temp351;( void*)
_temp395;}); _LL334: return({ struct Cyc_Absyn_Short_c_struct* _temp396=( struct
Cyc_Absyn_Short_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Short_c_struct));
_temp396->tag= Cyc_Absyn_Short_c_tag; _temp396->f1=( void*) Cyc_Absyn_Signed;
_temp396->f2= - _temp359;( void*) _temp396;}); _LL336: return({ struct Cyc_Absyn_Char_c_struct*
_temp397=( struct Cyc_Absyn_Char_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Char_c_struct));
_temp397->tag= Cyc_Absyn_Char_c_tag; _temp397->f1=( void*) Cyc_Absyn_Signed;
_temp397->f2= - _temp367;( void*) _temp397;}); _LL338: return({ struct Cyc_Absyn_Int_c_struct*
_temp398=( struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp398->tag= Cyc_Absyn_Int_c_tag; _temp398->f1=( void*) Cyc_Absyn_Signed;
_temp398->f2= _temp375 == 0? 1: 0;( void*) _temp398;}); _LL340: return({ struct
Cyc_Absyn_Int_c_struct* _temp399=( struct Cyc_Absyn_Int_c_struct*) GC_malloc(
sizeof( struct Cyc_Absyn_Int_c_struct)); _temp399->tag= Cyc_Absyn_Int_c_tag;
_temp399->f1=( void*) Cyc_Absyn_Signed; _temp399->f2= 1;( void*) _temp399;});
_LL342: return({ struct Cyc_Absyn_Int_c_struct* _temp400=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp400->tag= Cyc_Absyn_Int_c_tag;
_temp400->f1=( void*) Cyc_Absyn_Unsigned; _temp400->f2= ~ _temp387;( void*)
_temp400;}); _LL344: Cyc_Evexp_exp_err( e->loc,( struct _tagged_string)({ char*
_temp401=( char*)"bad constant expression"; struct _tagged_string _temp402;
_temp402.curr= _temp401; _temp402.base= _temp401; _temp402.last_plus_one=
_temp401 + 24; _temp402;})); return cn; _LL328:;} void* Cyc_Evexp_eval_const_binprimop(
void* p, struct Cyc_Absyn_Exp* e1, struct Cyc_Absyn_Exp* e2){ void* cn1= Cyc_Evexp_promote_const(
Cyc_Evexp_eval_const_exp( e1)); void* cn2= Cyc_Evexp_promote_const( Cyc_Evexp_eval_const_exp(
e2)); void* s1; void* s2; int i1; int i2;{ void* _temp403= cn1; int _temp409;
void* _temp411; _LL405: if(( unsigned int) _temp403 > 1u?(( struct _enum_struct*)
_temp403)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL412: _temp411=( void*)(( struct
Cyc_Absyn_Int_c_struct*) _temp403)->f1; goto _LL410; _LL410: _temp409=( int)((
struct Cyc_Absyn_Int_c_struct*) _temp403)->f2; goto _LL406;} else{ goto _LL407;}
_LL407: goto _LL408; _LL406: s1= _temp411; i1= _temp409; goto _LL404; _LL408:
Cyc_Evexp_exp_err( e1->loc,( struct _tagged_string)({ char* _temp413=( char*)"bad constant expression";
struct _tagged_string _temp414; _temp414.curr= _temp413; _temp414.base= _temp413;
_temp414.last_plus_one= _temp413 + 24; _temp414;})); return cn1; _LL404:;}{ void*
_temp415= cn2; int _temp421; void* _temp423; _LL417: if(( unsigned int) _temp415
> 1u?(( struct _enum_struct*) _temp415)->tag == Cyc_Absyn_Int_c_tag: 0){ _LL424:
_temp423=( void*)(( struct Cyc_Absyn_Int_c_struct*) _temp415)->f1; goto _LL422;
_LL422: _temp421=( int)(( struct Cyc_Absyn_Int_c_struct*) _temp415)->f2; goto
_LL418;} else{ goto _LL419;} _LL419: goto _LL420; _LL418: s2= _temp423; i2=
_temp421; goto _LL416; _LL420: Cyc_Evexp_exp_err( e2->loc,( struct
_tagged_string)({ char* _temp425=( char*)"bad constant expression"; struct
_tagged_string _temp426; _temp426.curr= _temp425; _temp426.base= _temp425;
_temp426.last_plus_one= _temp425 + 24; _temp426;})); return cn1; _LL416:;}{ void*
_temp427= p; _LL429: if( _temp427 == Cyc_Absyn_Div){ goto _LL430;} else{ goto
_LL431;} _LL431: if( _temp427 == Cyc_Absyn_Mod){ goto _LL432;} else{ goto _LL433;}
_LL433: goto _LL434; _LL430: if( i2 == 0){ Cyc_Evexp_exp_err( e2->loc,( struct
_tagged_string)({ char* _temp435=( char*)"division by zero in constant expression";
struct _tagged_string _temp436; _temp436.curr= _temp435; _temp436.base= _temp435;
_temp436.last_plus_one= _temp435 + 40; _temp436;})); return cn1;} goto _LL428;
_LL432: if( i2 == 0){ Cyc_Evexp_exp_err( e2->loc,( struct _tagged_string)({ char*
_temp437=( char*)"division by zero in constant expression"; struct
_tagged_string _temp438; _temp438.curr= _temp437; _temp438.base= _temp437;
_temp438.last_plus_one= _temp437 + 40; _temp438;})); return cn1;} goto _LL428;
_LL434: goto _LL428; _LL428:;}{ int has_u_arg= s1 == Cyc_Absyn_Unsigned? 1: s2
== Cyc_Absyn_Unsigned; unsigned int u1=( unsigned int) i1; unsigned int u2=(
unsigned int) i2; int i3= 0; unsigned int u3= 0; int b3= 1; int use_i3= 0; int
use_u3= 0; int use_b3= 0;{ struct _tuple5 _temp440=({ struct _tuple5 _temp439;
_temp439.f1= p; _temp439.f2= has_u_arg; _temp439;}); int _temp496; void*
_temp498; int _temp500; void* _temp502; int _temp504; void* _temp506; int
_temp508; void* _temp510; int _temp512; void* _temp514; int _temp516; void*
_temp518; int _temp520; void* _temp522; int _temp524; void* _temp526; int
_temp528; void* _temp530; int _temp532; void* _temp534; int _temp536; void*
_temp538; int _temp540; void* _temp542; int _temp544; void* _temp546; int
_temp548; void* _temp550; int _temp552; void* _temp554; int _temp556; void*
_temp558; int _temp560; void* _temp562; int _temp564; void* _temp566; int
_temp568; void* _temp570; int _temp572; void* _temp574; int _temp576; void*
_temp578; int _temp580; void* _temp582; int _temp584; void* _temp586; int
_temp588; void* _temp590; int _temp592; void* _temp594; int _temp596; void*
_temp598; _LL442: _LL499: _temp498= _temp440.f1; if( _temp498 == Cyc_Absyn_Plus){
goto _LL497;} else{ goto _LL444;} _LL497: _temp496= _temp440.f2; if( _temp496 ==
0){ goto _LL443;} else{ goto _LL444;} _LL444: _LL503: _temp502= _temp440.f1; if(
_temp502 == Cyc_Absyn_Times){ goto _LL501;} else{ goto _LL446;} _LL501: _temp500=
_temp440.f2; if( _temp500 == 0){ goto _LL445;} else{ goto _LL446;} _LL446:
_LL507: _temp506= _temp440.f1; if( _temp506 == Cyc_Absyn_Minus){ goto _LL505;}
else{ goto _LL448;} _LL505: _temp504= _temp440.f2; if( _temp504 == 0){ goto
_LL447;} else{ goto _LL448;} _LL448: _LL511: _temp510= _temp440.f1; if( _temp510
== Cyc_Absyn_Div){ goto _LL509;} else{ goto _LL450;} _LL509: _temp508= _temp440.f2;
if( _temp508 == 0){ goto _LL449;} else{ goto _LL450;} _LL450: _LL515: _temp514=
_temp440.f1; if( _temp514 == Cyc_Absyn_Mod){ goto _LL513;} else{ goto _LL452;}
_LL513: _temp512= _temp440.f2; if( _temp512 == 0){ goto _LL451;} else{ goto
_LL452;} _LL452: _LL519: _temp518= _temp440.f1; if( _temp518 == Cyc_Absyn_Plus){
goto _LL517;} else{ goto _LL454;} _LL517: _temp516= _temp440.f2; if( _temp516 ==
1){ goto _LL453;} else{ goto _LL454;} _LL454: _LL523: _temp522= _temp440.f1; if(
_temp522 == Cyc_Absyn_Times){ goto _LL521;} else{ goto _LL456;} _LL521: _temp520=
_temp440.f2; if( _temp520 == 1){ goto _LL455;} else{ goto _LL456;} _LL456:
_LL527: _temp526= _temp440.f1; if( _temp526 == Cyc_Absyn_Minus){ goto _LL525;}
else{ goto _LL458;} _LL525: _temp524= _temp440.f2; if( _temp524 == 1){ goto
_LL457;} else{ goto _LL458;} _LL458: _LL531: _temp530= _temp440.f1; if( _temp530
== Cyc_Absyn_Div){ goto _LL529;} else{ goto _LL460;} _LL529: _temp528= _temp440.f2;
if( _temp528 == 1){ goto _LL459;} else{ goto _LL460;} _LL460: _LL535: _temp534=
_temp440.f1; if( _temp534 == Cyc_Absyn_Mod){ goto _LL533;} else{ goto _LL462;}
_LL533: _temp532= _temp440.f2; if( _temp532 == 1){ goto _LL461;} else{ goto
_LL462;} _LL462: _LL539: _temp538= _temp440.f1; if( _temp538 == Cyc_Absyn_Eq){
goto _LL537;} else{ goto _LL464;} _LL537: _temp536= _temp440.f2; goto _LL463;
_LL464: _LL543: _temp542= _temp440.f1; if( _temp542 == Cyc_Absyn_Neq){ goto
_LL541;} else{ goto _LL466;} _LL541: _temp540= _temp440.f2; goto _LL465; _LL466:
_LL547: _temp546= _temp440.f1; if( _temp546 == Cyc_Absyn_Gt){ goto _LL545;}
else{ goto _LL468;} _LL545: _temp544= _temp440.f2; if( _temp544 == 0){ goto
_LL467;} else{ goto _LL468;} _LL468: _LL551: _temp550= _temp440.f1; if( _temp550
== Cyc_Absyn_Lt){ goto _LL549;} else{ goto _LL470;} _LL549: _temp548= _temp440.f2;
if( _temp548 == 0){ goto _LL469;} else{ goto _LL470;} _LL470: _LL555: _temp554=
_temp440.f1; if( _temp554 == Cyc_Absyn_Gte){ goto _LL553;} else{ goto _LL472;}
_LL553: _temp552= _temp440.f2; if( _temp552 == 0){ goto _LL471;} else{ goto
_LL472;} _LL472: _LL559: _temp558= _temp440.f1; if( _temp558 == Cyc_Absyn_Lte){
goto _LL557;} else{ goto _LL474;} _LL557: _temp556= _temp440.f2; if( _temp556 ==
0){ goto _LL473;} else{ goto _LL474;} _LL474: _LL563: _temp562= _temp440.f1; if(
_temp562 == Cyc_Absyn_Gt){ goto _LL561;} else{ goto _LL476;} _LL561: _temp560=
_temp440.f2; if( _temp560 == 1){ goto _LL475;} else{ goto _LL476;} _LL476:
_LL567: _temp566= _temp440.f1; if( _temp566 == Cyc_Absyn_Lt){ goto _LL565;}
else{ goto _LL478;} _LL565: _temp564= _temp440.f2; if( _temp564 == 1){ goto
_LL477;} else{ goto _LL478;} _LL478: _LL571: _temp570= _temp440.f1; if( _temp570
== Cyc_Absyn_Gte){ goto _LL569;} else{ goto _LL480;} _LL569: _temp568= _temp440.f2;
if( _temp568 == 1){ goto _LL479;} else{ goto _LL480;} _LL480: _LL575: _temp574=
_temp440.f1; if( _temp574 == Cyc_Absyn_Lte){ goto _LL573;} else{ goto _LL482;}
_LL573: _temp572= _temp440.f2; if( _temp572 == 1){ goto _LL481;} else{ goto
_LL482;} _LL482: _LL579: _temp578= _temp440.f1; if( _temp578 == Cyc_Absyn_Bitand){
goto _LL577;} else{ goto _LL484;} _LL577: _temp576= _temp440.f2; goto _LL483;
_LL484: _LL583: _temp582= _temp440.f1; if( _temp582 == Cyc_Absyn_Bitor){ goto
_LL581;} else{ goto _LL486;} _LL581: _temp580= _temp440.f2; goto _LL485; _LL486:
_LL587: _temp586= _temp440.f1; if( _temp586 == Cyc_Absyn_Bitxor){ goto _LL585;}
else{ goto _LL488;} _LL585: _temp584= _temp440.f2; goto _LL487; _LL488: _LL591:
_temp590= _temp440.f1; if( _temp590 == Cyc_Absyn_Bitlshift){ goto _LL589;} else{
goto _LL490;} _LL589: _temp588= _temp440.f2; goto _LL489; _LL490: _LL595:
_temp594= _temp440.f1; if( _temp594 == Cyc_Absyn_Bitlrshift){ goto _LL593;}
else{ goto _LL492;} _LL593: _temp592= _temp440.f2; goto _LL491; _LL492: _LL599:
_temp598= _temp440.f1; if( _temp598 == Cyc_Absyn_Bitarshift){ goto _LL597;}
else{ goto _LL494;} _LL597: _temp596= _temp440.f2; goto _LL493; _LL494: goto
_LL495; _LL443: i3= i1 + i2; use_i3= 1; goto _LL441; _LL445: i3= i1 * i2; use_i3=
1; goto _LL441; _LL447: i3= i1 - i2; use_i3= 1; goto _LL441; _LL449: i3= i1 / i2;
use_i3= 1; goto _LL441; _LL451: i3= i1 % i2; use_i3= 1; goto _LL441; _LL453: u3=
u1 + u2; use_u3= 1; goto _LL441; _LL455: u3= u1 * u2; use_u3= 1; goto _LL441;
_LL457: u3= u1 - u2; use_u3= 1; goto _LL441; _LL459: u3= u1 / u2; use_u3= 1;
goto _LL441; _LL461: u3= u1 % u2; use_u3= 1; goto _LL441; _LL463: b3= i1 == i2;
use_b3= 1; goto _LL441; _LL465: b3= i1 != i2; use_b3= 1; goto _LL441; _LL467: b3=
i1 > i2; use_b3= 1; goto _LL441; _LL469: b3= i1 < i2; use_b3= 1; goto _LL441;
_LL471: b3= i1 >= i2; use_b3= 1; goto _LL441; _LL473: b3= i1 <= i2; use_b3= 1;
goto _LL441; _LL475: b3= u1 > u2; use_b3= 1; goto _LL441; _LL477: b3= u1 < u2;
use_b3= 1; goto _LL441; _LL479: b3= u1 >= u2; use_b3= 1; goto _LL441; _LL481: b3=
u1 <= u2; use_b3= 1; goto _LL441; _LL483: u3= u1 & u2; use_u3= 1; goto _LL441;
_LL485: u3= u1 | u2; use_u3= 1; goto _LL441; _LL487: u3= u1 ^ u2; use_u3= 1;
goto _LL441; _LL489: u3= u1 << u2; use_u3= 1; goto _LL441; _LL491: u3= u1 >> u2;
use_u3= 1; goto _LL441; _LL493: Cyc_Evexp_exp_err( e1->loc,( struct
_tagged_string)({ char* _temp600=( char*)">>> NOT IMPLEMENTED"; struct
_tagged_string _temp601; _temp601.curr= _temp600; _temp601.base= _temp600;
_temp601.last_plus_one= _temp600 + 20; _temp601;})); return cn1; _LL495: Cyc_Evexp_exp_err(
e1->loc,( struct _tagged_string)({ char* _temp602=( char*)"bad constant expression";
struct _tagged_string _temp603; _temp603.curr= _temp602; _temp603.base= _temp602;
_temp603.last_plus_one= _temp602 + 24; _temp603;})); return cn1; _LL441:;} if(
use_i3){ return({ struct Cyc_Absyn_Int_c_struct* _temp604=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp604->tag= Cyc_Absyn_Int_c_tag;
_temp604->f1=( void*) Cyc_Absyn_Signed; _temp604->f2= i3;( void*) _temp604;});}
if( use_u3){ return({ struct Cyc_Absyn_Int_c_struct* _temp605=( struct Cyc_Absyn_Int_c_struct*)
GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct)); _temp605->tag= Cyc_Absyn_Int_c_tag;
_temp605->f1=( void*) Cyc_Absyn_Unsigned; _temp605->f2=( int) u3;( void*)
_temp605;});} if( use_b3){ return({ struct Cyc_Absyn_Int_c_struct* _temp606=(
struct Cyc_Absyn_Int_c_struct*) GC_malloc( sizeof( struct Cyc_Absyn_Int_c_struct));
_temp606->tag= Cyc_Absyn_Int_c_tag; _temp606->f1=( void*) Cyc_Absyn_Signed;
_temp606->f2= b3? 1: 0;( void*) _temp606;});}( void) _throw(({ struct Cyc_Core_Unreachable_struct*
_temp607=( struct Cyc_Core_Unreachable_struct*) GC_malloc( sizeof( struct Cyc_Core_Unreachable_struct));
_temp607->tag= Cyc_Core_Unreachable_tag; _temp607->f1=( struct _tagged_string)({
char* _temp608=( char*)"Evexp::eval_const_binop"; struct _tagged_string _temp609;
_temp609.curr= _temp608; _temp609.base= _temp608; _temp609.last_plus_one=
_temp608 + 24; _temp609;});( struct _xenum_struct*) _temp607;}));}}