 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, unsigned int);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; extern struct Cyc_List_List* Cyc_List_list(
struct _tagged_arr); extern unsigned char Cyc_List_List_empty[ 15u]; extern
struct Cyc_List_List* Cyc_List_map( void*(* f)( void*), struct Cyc_List_List* x);
extern unsigned char Cyc_List_List_mismatch[ 18u]; extern struct Cyc_List_List*
Cyc_List_rev( struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_imp_rev(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_List_append( struct
Cyc_List_List* x, struct Cyc_List_List* y); extern unsigned char Cyc_List_Nth[ 8u];
extern struct Cyc_List_List* Cyc_List_filter( int(* f)( void*), struct Cyc_List_List*
l); extern unsigned char Cyc_Arg_Bad[ 8u]; struct Cyc_Arg_Bad_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Arg_Error[
10u]; static const int Cyc_Arg_Unit_spec= 0; struct Cyc_Arg_Unit_spec_struct{
int tag; void(* f1)(); } ; static const int Cyc_Arg_Flag_spec= 1; struct Cyc_Arg_Flag_spec_struct{
int tag; void(* f1)( struct _tagged_arr); } ; static const int Cyc_Arg_FlagString_spec=
2; struct Cyc_Arg_FlagString_spec_struct{ int tag; void(* f1)( struct
_tagged_arr, struct _tagged_arr); } ; static const int Cyc_Arg_Set_spec= 3;
struct Cyc_Arg_Set_spec_struct{ int tag; int* f1; } ; static const int Cyc_Arg_Clear_spec=
4; struct Cyc_Arg_Clear_spec_struct{ int tag; int* f1; } ; static const int Cyc_Arg_String_spec=
5; struct Cyc_Arg_String_spec_struct{ int tag; void(* f1)( struct _tagged_arr);
} ; static const int Cyc_Arg_Int_spec= 6; struct Cyc_Arg_Int_spec_struct{ int
tag; void(* f1)( int); } ; static const int Cyc_Arg_Rest_spec= 7; struct Cyc_Arg_Rest_spec_struct{
int tag; void(* f1)( struct _tagged_arr); } ; extern void Cyc_Arg_parse( struct
Cyc_List_List* specs, void(* anonfun)( struct _tagged_arr), struct _tagged_arr
errmsg, struct _tagged_arr args); struct Cyc_std___sFILE; extern struct Cyc_std___sFILE*
Cyc_std_stdout; extern struct Cyc_std___sFILE* Cyc_std_stderr; extern int Cyc_std_remove(
struct _tagged_arr); extern int Cyc_std_fclose( struct Cyc_std___sFILE*); extern
int Cyc_std_fflush( struct Cyc_std___sFILE*); extern struct Cyc_std___sFILE* Cyc_std_fopen(
struct _tagged_arr __filename, struct _tagged_arr __modes); extern unsigned char
Cyc_std_FileCloseError[ 19u]; extern unsigned char Cyc_std_FileOpenError[ 18u];
struct Cyc_std_FileOpenError_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern struct Cyc_std___sFILE* Cyc_std_file_open( struct _tagged_arr fname,
struct _tagged_arr mode); extern void Cyc_std_file_close( struct Cyc_std___sFILE*);
static const int Cyc_std_String_pa= 0; struct Cyc_std_String_pa_struct{ int tag;
struct _tagged_arr f1; } ; static const int Cyc_std_Int_pa= 1; struct Cyc_std_Int_pa_struct{
int tag; unsigned int f1; } ; static const int Cyc_std_Double_pa= 2; struct Cyc_std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_std_ShortPtr_pa= 3; struct Cyc_std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_std_IntPtr_pa= 4; struct Cyc_std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_std_fprintf( struct Cyc_std___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); extern int Cyc_std_printf( struct
_tagged_arr fmt, struct _tagged_arr); extern struct _tagged_arr Cyc_std_aprintf(
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_std_ShortPtr_sa=
0; struct Cyc_std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_std_UShortPtr_sa= 1; struct Cyc_std_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_std_IntPtr_sa= 2; struct Cyc_std_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_std_UIntPtr_sa= 3; struct Cyc_std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_std_StringPtr_sa= 4; struct
Cyc_std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_std_DoublePtr_sa= 5; struct Cyc_std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_std_FloatPtr_sa= 6; struct Cyc_std_FloatPtr_sa_struct{
int tag; float* f1; } ; extern int system( unsigned char*); struct Cyc_std__Div{
int quot; int rem; } ; struct Cyc_std__Ldiv{ int quot; int rem; } ; extern int
abs( int __x); extern int atexit( void(* __func)()); extern struct Cyc_std__Div
div( int __numer, int __denom); extern struct Cyc_std__Ldiv ldiv( int __numer,
int __denom); extern int random(); extern void srandom( unsigned int __seed);
extern int rand(); extern void srand( unsigned int __seed); extern int rand_r(
unsigned int* __seed); extern int grantpt( int __fd); extern int unlockpt( int
__fd); extern struct _tagged_arr Cyc_std_getenv( struct _tagged_arr); extern
unsigned int Cyc_std_strlen( struct _tagged_arr s); extern int Cyc_std_strcmp(
struct _tagged_arr s1, struct _tagged_arr s2); extern struct _tagged_arr Cyc_std_strconcat(
struct _tagged_arr, struct _tagged_arr); extern struct _tagged_arr Cyc_std_strconcat_l(
struct Cyc_List_List*); extern struct _tagged_arr Cyc_std_str_sepstr( struct Cyc_List_List*,
struct _tagged_arr); extern struct _tagged_arr Cyc_std_substring( struct
_tagged_arr, int ofs, unsigned int n); extern struct _tagged_arr Cyc_std_strchr(
struct _tagged_arr s, unsigned char c); struct Cyc_Dict_Dict; extern
unsigned char Cyc_Dict_Present[ 12u]; extern unsigned char Cyc_Dict_Absent[ 11u];
extern struct _tagged_arr Cyc_Filename_concat( struct _tagged_arr, struct
_tagged_arr); extern struct _tagged_arr Cyc_Filename_chop_extension( struct
_tagged_arr); extern int Cyc_Filename_check_suffix( struct _tagged_arr, struct
_tagged_arr); struct Cyc_Lineno_Pos{ struct _tagged_arr logical_file; struct
_tagged_arr line; int line_no; int col; } ; extern unsigned char Cyc_Position_Exit[
9u]; extern void Cyc_Position_reset_position( struct _tagged_arr); struct Cyc_Position_Segment;
static const int Cyc_Position_Lex= 0; static const int Cyc_Position_Parse= 1;
static const int Cyc_Position_Elab= 2; struct Cyc_Position_Error{ struct
_tagged_arr source; struct Cyc_Position_Segment* seg; void* kind; struct
_tagged_arr desc; } ; extern unsigned char Cyc_Position_Nocontext[ 14u]; extern
int Cyc_Position_error_p(); struct _tuple0{ void* f1; struct _tagged_arr* f2; }
; struct Cyc_Absyn_Tvar; struct Cyc_Absyn_Tqual; struct Cyc_Absyn_Conref; struct
Cyc_Absyn_PtrInfo; struct Cyc_Absyn_VarargInfo; struct Cyc_Absyn_FnInfo; struct
Cyc_Absyn_TunionInfo; struct Cyc_Absyn_TunionFieldInfo; struct Cyc_Absyn_VarargCallInfo;
struct Cyc_Absyn_Exp; struct Cyc_Absyn_Stmt; struct Cyc_Absyn_Pat; struct Cyc_Absyn_Switch_clause;
struct Cyc_Absyn_SwitchC_clause; struct Cyc_Absyn_Fndecl; struct Cyc_Absyn_Structdecl;
struct Cyc_Absyn_Uniondecl; struct Cyc_Absyn_Tuniondecl; struct Cyc_Absyn_Tunionfield;
struct Cyc_Absyn_Enumfield; struct Cyc_Absyn_Enumdecl; struct Cyc_Absyn_Typedefdecl;
struct Cyc_Absyn_Vardecl; struct Cyc_Absyn_Decl; struct Cyc_Absyn_Structfield;
static const int Cyc_Absyn_Loc_n= 0; static const int Cyc_Absyn_Rel_n= 0; struct
Cyc_Absyn_Rel_n_struct{ int tag; struct Cyc_List_List* f1; } ; static const int
Cyc_Absyn_Abs_n= 1; struct Cyc_Absyn_Abs_n_struct{ int tag; struct Cyc_List_List*
f1; } ; static const int Cyc_Absyn_Static= 0; static const int Cyc_Absyn_Abstract=
1; static const int Cyc_Absyn_Public= 2; static const int Cyc_Absyn_Extern= 3;
static const int Cyc_Absyn_ExternC= 4; struct Cyc_Absyn_Tqual{ int q_const: 1;
int q_volatile: 1; int q_restrict: 1; } ; static const int Cyc_Absyn_B1= 0;
static const int Cyc_Absyn_B2= 1; static const int Cyc_Absyn_B4= 2; static const
int Cyc_Absyn_B8= 3; static const int Cyc_Absyn_AnyKind= 0; static const int Cyc_Absyn_MemKind=
1; static const int Cyc_Absyn_BoxKind= 2; static const int Cyc_Absyn_RgnKind= 3;
static const int Cyc_Absyn_EffKind= 4; static const int Cyc_Absyn_Signed= 0;
static const int Cyc_Absyn_Unsigned= 1; struct Cyc_Absyn_Conref{ void* v; } ;
static const int Cyc_Absyn_Eq_constr= 0; struct Cyc_Absyn_Eq_constr_struct{ int
tag; void* f1; } ; static const int Cyc_Absyn_Forward_constr= 1; struct Cyc_Absyn_Forward_constr_struct{
int tag; struct Cyc_Absyn_Conref* f1; } ; static const int Cyc_Absyn_No_constr=
0; struct Cyc_Absyn_Tvar{ struct _tagged_arr* name; int* identity; struct Cyc_Absyn_Conref*
kind; } ; static const int Cyc_Absyn_Unknown_b= 0; static const int Cyc_Absyn_Upper_b=
0; struct Cyc_Absyn_Upper_b_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
struct Cyc_Absyn_PtrInfo{ void* elt_typ; void* rgn_typ; struct Cyc_Absyn_Conref*
nullable; struct Cyc_Absyn_Tqual tq; struct Cyc_Absyn_Conref* bounds; } ; struct
Cyc_Absyn_VarargInfo{ struct Cyc_Core_Opt* name; struct Cyc_Absyn_Tqual tq; void*
type; void* rgn; int inject; } ; struct Cyc_Absyn_FnInfo{ struct Cyc_List_List*
tvars; struct Cyc_Core_Opt* effect; void* ret_typ; struct Cyc_List_List* args;
int c_varargs; struct Cyc_Absyn_VarargInfo* cyc_varargs; struct Cyc_List_List*
rgn_po; struct Cyc_List_List* attributes; } ; struct Cyc_Absyn_UnknownTunionInfo{
struct _tuple0* name; int is_xtunion; } ; static const int Cyc_Absyn_UnknownTunion=
0; struct Cyc_Absyn_UnknownTunion_struct{ int tag; struct Cyc_Absyn_UnknownTunionInfo
f1; } ; static const int Cyc_Absyn_KnownTunion= 1; struct Cyc_Absyn_KnownTunion_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; } ; struct Cyc_Absyn_TunionInfo{ void*
tunion_info; struct Cyc_List_List* targs; void* rgn; } ; struct Cyc_Absyn_UnknownTunionFieldInfo{
struct _tuple0* tunion_name; struct _tuple0* field_name; int is_xtunion; } ;
static const int Cyc_Absyn_UnknownTunionfield= 0; struct Cyc_Absyn_UnknownTunionfield_struct{
int tag; struct Cyc_Absyn_UnknownTunionFieldInfo f1; } ; static const int Cyc_Absyn_KnownTunionfield=
1; struct Cyc_Absyn_KnownTunionfield_struct{ int tag; struct Cyc_Absyn_Tuniondecl*
f1; struct Cyc_Absyn_Tunionfield* f2; } ; struct Cyc_Absyn_TunionFieldInfo{ void*
field_info; struct Cyc_List_List* targs; } ; static const int Cyc_Absyn_VoidType=
0; static const int Cyc_Absyn_Evar= 0; struct Cyc_Absyn_Evar_struct{ int tag;
struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt* f2; int f3; struct Cyc_Core_Opt*
f4; } ; static const int Cyc_Absyn_VarType= 1; struct Cyc_Absyn_VarType_struct{
int tag; struct Cyc_Absyn_Tvar* f1; } ; static const int Cyc_Absyn_TunionType= 2;
struct Cyc_Absyn_TunionType_struct{ int tag; struct Cyc_Absyn_TunionInfo f1; } ;
static const int Cyc_Absyn_TunionFieldType= 3; struct Cyc_Absyn_TunionFieldType_struct{
int tag; struct Cyc_Absyn_TunionFieldInfo f1; } ; static const int Cyc_Absyn_PointerType=
4; struct Cyc_Absyn_PointerType_struct{ int tag; struct Cyc_Absyn_PtrInfo f1; }
; static const int Cyc_Absyn_IntType= 5; struct Cyc_Absyn_IntType_struct{ int
tag; void* f1; void* f2; } ; static const int Cyc_Absyn_FloatType= 1; static
const int Cyc_Absyn_DoubleType= 2; static const int Cyc_Absyn_ArrayType= 6;
struct Cyc_Absyn_ArrayType_struct{ int tag; void* f1; struct Cyc_Absyn_Tqual f2;
struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_FnType= 7; struct Cyc_Absyn_FnType_struct{
int tag; struct Cyc_Absyn_FnInfo f1; } ; static const int Cyc_Absyn_TupleType= 8;
struct Cyc_Absyn_TupleType_struct{ int tag; struct Cyc_List_List* f1; } ; static
const int Cyc_Absyn_StructType= 9; struct Cyc_Absyn_StructType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Structdecl** f3;
} ; static const int Cyc_Absyn_UnionType= 10; struct Cyc_Absyn_UnionType_struct{
int tag; struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Absyn_Uniondecl**
f3; } ; static const int Cyc_Absyn_AnonStructType= 11; struct Cyc_Absyn_AnonStructType_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_AnonUnionType=
12; struct Cyc_Absyn_AnonUnionType_struct{ int tag; struct Cyc_List_List* f1; }
; static const int Cyc_Absyn_EnumType= 13; struct Cyc_Absyn_EnumType_struct{ int
tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; } ; static const int Cyc_Absyn_RgnHandleType=
14; struct Cyc_Absyn_RgnHandleType_struct{ int tag; void* f1; } ; static const
int Cyc_Absyn_TypedefType= 15; struct Cyc_Absyn_TypedefType_struct{ int tag;
struct _tuple0* f1; struct Cyc_List_List* f2; struct Cyc_Core_Opt* f3; } ;
static const int Cyc_Absyn_HeapRgn= 3; static const int Cyc_Absyn_AccessEff= 16;
struct Cyc_Absyn_AccessEff_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_JoinEff=
17; struct Cyc_Absyn_JoinEff_struct{ int tag; struct Cyc_List_List* f1; } ;
static const int Cyc_Absyn_RgnsEff= 18; struct Cyc_Absyn_RgnsEff_struct{ int tag;
void* f1; } ; static const int Cyc_Absyn_NoTypes= 0; struct Cyc_Absyn_NoTypes_struct{
int tag; struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; } ; static
const int Cyc_Absyn_WithTypes= 1; struct Cyc_Absyn_WithTypes_struct{ int tag;
struct Cyc_List_List* f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt*
f4; struct Cyc_List_List* f5; } ; static const int Cyc_Absyn_NonNullable_ps= 0;
struct Cyc_Absyn_NonNullable_ps_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Nullable_ps= 1; struct Cyc_Absyn_Nullable_ps_struct{
int tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_TaggedArray_ps=
0; static const int Cyc_Absyn_Printf_ft= 0; static const int Cyc_Absyn_Scanf_ft=
1; static const int Cyc_Absyn_Regparm_att= 0; struct Cyc_Absyn_Regparm_att_struct{
int tag; int f1; } ; static const int Cyc_Absyn_Stdcall_att= 0; static const int
Cyc_Absyn_Cdecl_att= 1; static const int Cyc_Absyn_Fastcall_att= 2; static const
int Cyc_Absyn_Noreturn_att= 3; static const int Cyc_Absyn_Const_att= 4; static
const int Cyc_Absyn_Aligned_att= 1; struct Cyc_Absyn_Aligned_att_struct{ int tag;
int f1; } ; static const int Cyc_Absyn_Packed_att= 5; static const int Cyc_Absyn_Section_att=
2; struct Cyc_Absyn_Section_att_struct{ int tag; struct _tagged_arr f1; } ;
static const int Cyc_Absyn_Nocommon_att= 6; static const int Cyc_Absyn_Shared_att=
7; static const int Cyc_Absyn_Unused_att= 8; static const int Cyc_Absyn_Weak_att=
9; static const int Cyc_Absyn_Dllimport_att= 10; static const int Cyc_Absyn_Dllexport_att=
11; static const int Cyc_Absyn_No_instrument_function_att= 12; static const int
Cyc_Absyn_Constructor_att= 13; static const int Cyc_Absyn_Destructor_att= 14;
static const int Cyc_Absyn_No_check_memory_usage_att= 15; static const int Cyc_Absyn_Format_att=
3; struct Cyc_Absyn_Format_att_struct{ int tag; void* f1; int f2; int f3; } ;
static const int Cyc_Absyn_Carray_mod= 0; static const int Cyc_Absyn_ConstArray_mod=
0; struct Cyc_Absyn_ConstArray_mod_struct{ int tag; struct Cyc_Absyn_Exp* f1; }
; static const int Cyc_Absyn_Pointer_mod= 1; struct Cyc_Absyn_Pointer_mod_struct{
int tag; void* f1; void* f2; struct Cyc_Absyn_Tqual f3; } ; static const int Cyc_Absyn_Function_mod=
2; struct Cyc_Absyn_Function_mod_struct{ int tag; void* f1; } ; static const int
Cyc_Absyn_TypeParams_mod= 3; struct Cyc_Absyn_TypeParams_mod_struct{ int tag;
struct Cyc_List_List* f1; struct Cyc_Position_Segment* f2; int f3; } ; static
const int Cyc_Absyn_Attributes_mod= 4; struct Cyc_Absyn_Attributes_mod_struct{
int tag; struct Cyc_Position_Segment* f1; struct Cyc_List_List* f2; } ; static
const int Cyc_Absyn_Char_c= 0; struct Cyc_Absyn_Char_c_struct{ int tag; void* f1;
unsigned char f2; } ; static const int Cyc_Absyn_Short_c= 1; struct Cyc_Absyn_Short_c_struct{
int tag; void* f1; short f2; } ; static const int Cyc_Absyn_Int_c= 2; struct Cyc_Absyn_Int_c_struct{
int tag; void* f1; int f2; } ; static const int Cyc_Absyn_LongLong_c= 3; struct
Cyc_Absyn_LongLong_c_struct{ int tag; void* f1; long long f2; } ; static const
int Cyc_Absyn_Float_c= 4; struct Cyc_Absyn_Float_c_struct{ int tag; struct
_tagged_arr f1; } ; static const int Cyc_Absyn_String_c= 5; struct Cyc_Absyn_String_c_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Absyn_Null_c= 0; static
const int Cyc_Absyn_Plus= 0; static const int Cyc_Absyn_Times= 1; static const
int Cyc_Absyn_Minus= 2; static const int Cyc_Absyn_Div= 3; static const int Cyc_Absyn_Mod=
4; static const int Cyc_Absyn_Eq= 5; static const int Cyc_Absyn_Neq= 6; static
const int Cyc_Absyn_Gt= 7; static const int Cyc_Absyn_Lt= 8; static const int
Cyc_Absyn_Gte= 9; static const int Cyc_Absyn_Lte= 10; static const int Cyc_Absyn_Not=
11; static const int Cyc_Absyn_Bitnot= 12; static const int Cyc_Absyn_Bitand= 13;
static const int Cyc_Absyn_Bitor= 14; static const int Cyc_Absyn_Bitxor= 15;
static const int Cyc_Absyn_Bitlshift= 16; static const int Cyc_Absyn_Bitlrshift=
17; static const int Cyc_Absyn_Bitarshift= 18; static const int Cyc_Absyn_Size=
19; static const int Cyc_Absyn_PreInc= 0; static const int Cyc_Absyn_PostInc= 1;
static const int Cyc_Absyn_PreDec= 2; static const int Cyc_Absyn_PostDec= 3;
struct Cyc_Absyn_VarargCallInfo{ int num_varargs; struct Cyc_List_List*
injectors; struct Cyc_Absyn_VarargInfo* vai; } ; static const int Cyc_Absyn_Const_e=
0; struct Cyc_Absyn_Const_e_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Var_e=
1; struct Cyc_Absyn_Var_e_struct{ int tag; struct _tuple0* f1; void* f2; } ;
static const int Cyc_Absyn_UnknownId_e= 2; struct Cyc_Absyn_UnknownId_e_struct{
int tag; struct _tuple0* f1; } ; static const int Cyc_Absyn_Primop_e= 3; struct
Cyc_Absyn_Primop_e_struct{ int tag; void* f1; struct Cyc_List_List* f2; } ;
static const int Cyc_Absyn_AssignOp_e= 4; struct Cyc_Absyn_AssignOp_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_Core_Opt* f2; struct Cyc_Absyn_Exp*
f3; } ; static const int Cyc_Absyn_Increment_e= 5; struct Cyc_Absyn_Increment_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_Conditional_e=
6; struct Cyc_Absyn_Conditional_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_SeqExp_e=
7; struct Cyc_Absyn_SeqExp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; struct
Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_UnknownCall_e= 8; struct Cyc_Absyn_UnknownCall_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_FnCall_e= 9; struct Cyc_Absyn_FnCall_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct Cyc_List_List* f2; struct Cyc_Absyn_VarargCallInfo* f3; } ; static
const int Cyc_Absyn_Throw_e= 10; struct Cyc_Absyn_Throw_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_NoInstantiate_e= 11;
struct Cyc_Absyn_NoInstantiate_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Instantiate_e= 12; struct Cyc_Absyn_Instantiate_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Cast_e= 13; struct Cyc_Absyn_Cast_e_struct{ int tag; void* f1;
struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Address_e= 14; struct
Cyc_Absyn_Address_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ; static const
int Cyc_Absyn_New_e= 15; struct Cyc_Absyn_New_e_struct{ int tag; struct Cyc_Absyn_Exp*
f1; struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Sizeoftyp_e= 16;
struct Cyc_Absyn_Sizeoftyp_e_struct{ int tag; void* f1; } ; static const int Cyc_Absyn_Sizeofexp_e=
17; struct Cyc_Absyn_Sizeofexp_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_Offsetof_e= 18; struct Cyc_Absyn_Offsetof_e_struct{
int tag; void* f1; struct _tagged_arr* f2; } ; static const int Cyc_Absyn_Deref_e=
19; struct Cyc_Absyn_Deref_e_struct{ int tag; struct Cyc_Absyn_Exp* f1; } ;
static const int Cyc_Absyn_StructMember_e= 20; struct Cyc_Absyn_StructMember_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct _tagged_arr* f2; } ; static const int
Cyc_Absyn_StructArrow_e= 21; struct Cyc_Absyn_StructArrow_e_struct{ int tag;
struct Cyc_Absyn_Exp* f1; struct _tagged_arr* f2; } ; static const int Cyc_Absyn_Subscript_e=
22; struct Cyc_Absyn_Subscript_e_struct{ int tag; struct Cyc_Absyn_Exp* f1;
struct Cyc_Absyn_Exp* f2; } ; static const int Cyc_Absyn_Tuple_e= 23; struct Cyc_Absyn_Tuple_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_CompoundLit_e=
24; struct _tuple1{ struct Cyc_Core_Opt* f1; struct Cyc_Absyn_Tqual f2; void* f3;
} ; struct Cyc_Absyn_CompoundLit_e_struct{ int tag; struct _tuple1* f1; struct
Cyc_List_List* f2; } ; static const int Cyc_Absyn_Array_e= 25; struct Cyc_Absyn_Array_e_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Comprehension_e=
26; struct Cyc_Absyn_Comprehension_e_struct{ int tag; struct Cyc_Absyn_Vardecl*
f1; struct Cyc_Absyn_Exp* f2; struct Cyc_Absyn_Exp* f3; } ; static const int Cyc_Absyn_Struct_e=
27; struct Cyc_Absyn_Struct_e_struct{ int tag; struct _tuple0* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Structdecl* f4; } ; static const
int Cyc_Absyn_AnonStruct_e= 28; struct Cyc_Absyn_AnonStruct_e_struct{ int tag;
void* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Tunion_e= 29;
struct Cyc_Absyn_Tunion_e_struct{ int tag; struct Cyc_Core_Opt* f1; struct Cyc_Core_Opt*
f2; struct Cyc_List_List* f3; struct Cyc_Absyn_Tuniondecl* f4; struct Cyc_Absyn_Tunionfield*
f5; } ; static const int Cyc_Absyn_Enum_e= 30; struct Cyc_Absyn_Enum_e_struct{
int tag; struct _tuple0* f1; struct Cyc_Absyn_Enumdecl* f2; struct Cyc_Absyn_Enumfield*
f3; } ; static const int Cyc_Absyn_Malloc_e= 31; struct Cyc_Absyn_Malloc_e_struct{
int tag; struct Cyc_Absyn_Exp* f1; void* f2; } ; static const int Cyc_Absyn_UnresolvedMem_e=
32; struct Cyc_Absyn_UnresolvedMem_e_struct{ int tag; struct Cyc_Core_Opt* f1;
struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_StmtExp_e= 33; struct
Cyc_Absyn_StmtExp_e_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static const
int Cyc_Absyn_Codegen_e= 34; struct Cyc_Absyn_Codegen_e_struct{ int tag; struct
Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Fill_e= 35; struct Cyc_Absyn_Fill_e_struct{
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
struct Cyc_Absyn_Goto_s_struct{ int tag; struct _tagged_arr* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_For_s= 8; struct Cyc_Absyn_For_s_struct{ int
tag; struct Cyc_Absyn_Exp* f1; struct _tuple2 f2; struct _tuple2 f3; struct Cyc_Absyn_Stmt*
f4; } ; static const int Cyc_Absyn_Switch_s= 9; struct Cyc_Absyn_Switch_s_struct{
int tag; struct Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_SwitchC_s= 10; struct Cyc_Absyn_SwitchC_s_struct{ int tag; struct
Cyc_Absyn_Exp* f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_Fallthru_s=
11; struct Cyc_Absyn_Fallthru_s_struct{ int tag; struct Cyc_List_List* f1;
struct Cyc_Absyn_Switch_clause** f2; } ; static const int Cyc_Absyn_Decl_s= 12;
struct Cyc_Absyn_Decl_s_struct{ int tag; struct Cyc_Absyn_Decl* f1; struct Cyc_Absyn_Stmt*
f2; } ; static const int Cyc_Absyn_Cut_s= 13; struct Cyc_Absyn_Cut_s_struct{ int
tag; struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Absyn_Splice_s= 14;
struct Cyc_Absyn_Splice_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; } ; static
const int Cyc_Absyn_Label_s= 15; struct Cyc_Absyn_Label_s_struct{ int tag;
struct _tagged_arr* f1; struct Cyc_Absyn_Stmt* f2; } ; static const int Cyc_Absyn_Do_s=
16; struct Cyc_Absyn_Do_s_struct{ int tag; struct Cyc_Absyn_Stmt* f1; struct
_tuple2 f2; } ; static const int Cyc_Absyn_TryCatch_s= 17; struct Cyc_Absyn_TryCatch_s_struct{
int tag; struct Cyc_Absyn_Stmt* f1; struct Cyc_List_List* f2; } ; static const
int Cyc_Absyn_Region_s= 18; struct Cyc_Absyn_Region_s_struct{ int tag; struct
Cyc_Absyn_Tvar* f1; struct Cyc_Absyn_Vardecl* f2; struct Cyc_Absyn_Stmt* f3; } ;
struct Cyc_Absyn_Stmt{ void* r; struct Cyc_Position_Segment* loc; struct Cyc_List_List*
non_local_preds; int try_depth; void* annot; } ; static const int Cyc_Absyn_Wild_p=
0; static const int Cyc_Absyn_Var_p= 0; struct Cyc_Absyn_Var_p_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Null_p= 1; static
const int Cyc_Absyn_Int_p= 1; struct Cyc_Absyn_Int_p_struct{ int tag; void* f1;
int f2; } ; static const int Cyc_Absyn_Char_p= 2; struct Cyc_Absyn_Char_p_struct{
int tag; unsigned char f1; } ; static const int Cyc_Absyn_Float_p= 3; struct Cyc_Absyn_Float_p_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Absyn_Tuple_p= 4;
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
body; struct Cyc_Position_Segment* loc; } ; struct Cyc_Absyn_SwitchC_clause{
struct Cyc_Absyn_Exp* cnst_exp; struct Cyc_Absyn_Stmt* body; struct Cyc_Position_Segment*
loc; } ; static const int Cyc_Absyn_Unresolved_b= 0; static const int Cyc_Absyn_Global_b=
0; struct Cyc_Absyn_Global_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_Absyn_Funname_b= 1; struct Cyc_Absyn_Funname_b_struct{ int
tag; struct Cyc_Absyn_Fndecl* f1; } ; static const int Cyc_Absyn_Param_b= 2;
struct Cyc_Absyn_Param_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ;
static const int Cyc_Absyn_Local_b= 3; struct Cyc_Absyn_Local_b_struct{ int tag;
struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Pat_b= 4; struct
Cyc_Absyn_Pat_b_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; struct Cyc_Absyn_Vardecl{
void* sc; struct _tuple0* name; struct Cyc_Absyn_Tqual tq; void* type; struct
Cyc_Absyn_Exp* initializer; struct Cyc_Core_Opt* rgn; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Fndecl{ void* sc; int is_inline; struct _tuple0*
name; struct Cyc_List_List* tvs; struct Cyc_Core_Opt* effect; void* ret_type;
struct Cyc_List_List* args; int c_varargs; struct Cyc_Absyn_VarargInfo*
cyc_varargs; struct Cyc_List_List* rgn_po; struct Cyc_Absyn_Stmt* body; struct
Cyc_Core_Opt* cached_typ; struct Cyc_Core_Opt* param_vardecls; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structfield{ struct _tagged_arr* name; struct
Cyc_Absyn_Tqual tq; void* type; struct Cyc_Absyn_Exp* width; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Structdecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Uniondecl{ void* sc; struct Cyc_Core_Opt* name;
struct Cyc_List_List* tvs; struct Cyc_Core_Opt* fields; struct Cyc_List_List*
attributes; } ; struct Cyc_Absyn_Tunionfield{ struct _tuple0* name; struct Cyc_List_List*
tvs; struct Cyc_List_List* typs; struct Cyc_Position_Segment* loc; void* sc; } ;
struct Cyc_Absyn_Tuniondecl{ void* sc; struct _tuple0* name; struct Cyc_List_List*
tvs; struct Cyc_Core_Opt* fields; int is_xtunion; } ; struct Cyc_Absyn_Enumfield{
struct _tuple0* name; struct Cyc_Absyn_Exp* tag; struct Cyc_Position_Segment*
loc; } ; struct Cyc_Absyn_Enumdecl{ void* sc; struct _tuple0* name; struct Cyc_Core_Opt*
fields; } ; struct Cyc_Absyn_Typedefdecl{ struct _tuple0* name; struct Cyc_List_List*
tvs; void* defn; } ; static const int Cyc_Absyn_Var_d= 0; struct Cyc_Absyn_Var_d_struct{
int tag; struct Cyc_Absyn_Vardecl* f1; } ; static const int Cyc_Absyn_Fn_d= 1;
struct Cyc_Absyn_Fn_d_struct{ int tag; struct Cyc_Absyn_Fndecl* f1; } ; static
const int Cyc_Absyn_Let_d= 2; struct Cyc_Absyn_Let_d_struct{ int tag; struct Cyc_Absyn_Pat*
f1; struct Cyc_Core_Opt* f2; struct Cyc_Core_Opt* f3; struct Cyc_Absyn_Exp* f4;
int f5; } ; static const int Cyc_Absyn_Letv_d= 3; struct Cyc_Absyn_Letv_d_struct{
int tag; struct Cyc_List_List* f1; } ; static const int Cyc_Absyn_Struct_d= 4;
struct Cyc_Absyn_Struct_d_struct{ int tag; struct Cyc_Absyn_Structdecl* f1; } ;
static const int Cyc_Absyn_Union_d= 5; struct Cyc_Absyn_Union_d_struct{ int tag;
struct Cyc_Absyn_Uniondecl* f1; } ; static const int Cyc_Absyn_Tunion_d= 6;
struct Cyc_Absyn_Tunion_d_struct{ int tag; struct Cyc_Absyn_Tuniondecl* f1; } ;
static const int Cyc_Absyn_Enum_d= 7; struct Cyc_Absyn_Enum_d_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1; } ; static const int Cyc_Absyn_Typedef_d= 8;
struct Cyc_Absyn_Typedef_d_struct{ int tag; struct Cyc_Absyn_Typedefdecl* f1; }
; static const int Cyc_Absyn_Namespace_d= 9; struct Cyc_Absyn_Namespace_d_struct{
int tag; struct _tagged_arr* f1; struct Cyc_List_List* f2; } ; static const int
Cyc_Absyn_Using_d= 10; struct Cyc_Absyn_Using_d_struct{ int tag; struct _tuple0*
f1; struct Cyc_List_List* f2; } ; static const int Cyc_Absyn_ExternC_d= 11;
struct Cyc_Absyn_ExternC_d_struct{ int tag; struct Cyc_List_List* f1; } ; struct
Cyc_Absyn_Decl{ void* r; struct Cyc_Position_Segment* loc; } ; static const int
Cyc_Absyn_ArrayElement= 0; struct Cyc_Absyn_ArrayElement_struct{ int tag; struct
Cyc_Absyn_Exp* f1; } ; static const int Cyc_Absyn_FieldName= 1; struct Cyc_Absyn_FieldName_struct{
int tag; struct _tagged_arr* f1; } ; extern unsigned char Cyc_Absyn_EmptyAnnot[
15u]; extern unsigned char Cyc_Lexing_Error[ 10u]; struct Cyc_Lexing_Error_struct{
unsigned char* tag; struct _tagged_arr f1; } ; struct Cyc_Lexing_lexbuf{ void(*
refill_buff)( struct Cyc_Lexing_lexbuf*); void* refill_state; struct _tagged_arr
lex_buffer; int lex_buffer_len; int lex_abs_pos; int lex_start_pos; int
lex_curr_pos; int lex_last_pos; int lex_last_action; int lex_eof_reached; } ;
struct Cyc_Lexing_function_lexbuf_state{ int(* read_fun)( struct _tagged_arr,
int, void*); void* read_fun_state; } ; struct Cyc_Lexing_lex_tables{ struct
_tagged_arr lex_base; struct _tagged_arr lex_backtrk; struct _tagged_arr
lex_default; struct _tagged_arr lex_trans; struct _tagged_arr lex_check; } ;
extern struct Cyc_List_List* Cyc_Parse_parse_file( struct Cyc_std___sFILE* f);
struct Cyc_Declaration_spec; struct Cyc_Declarator; struct Cyc_Abstractdeclarator;
extern unsigned char Cyc_AbstractDeclarator_tok[ 27u]; struct Cyc_AbstractDeclarator_tok_struct{
unsigned char* tag; struct Cyc_Abstractdeclarator* f1; } ; extern unsigned char
Cyc_AttributeList_tok[ 22u]; struct Cyc_AttributeList_tok_struct{ unsigned char*
tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Attribute_tok[ 18u];
struct Cyc_Attribute_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_Bool_tok[ 13u]; struct Cyc_Bool_tok_struct{ unsigned char* tag;
int f1; } ; extern unsigned char Cyc_Char_tok[ 13u]; struct Cyc_Char_tok_struct{
unsigned char* tag; unsigned char f1; } ; extern unsigned char Cyc_DeclList_tok[
17u]; struct Cyc_DeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_DeclSpec_tok[ 17u]; struct Cyc_DeclSpec_tok_struct{
unsigned char* tag; struct Cyc_Declaration_spec* f1; } ; extern unsigned char
Cyc_DeclaratorExpoptList_tok[ 29u]; struct Cyc_DeclaratorExpoptList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_DeclaratorExpopt_tok[
25u]; struct _tuple3{ struct Cyc_Declarator* f1; struct Cyc_Absyn_Exp* f2; } ;
struct Cyc_DeclaratorExpopt_tok_struct{ unsigned char* tag; struct _tuple3* f1;
} ; extern unsigned char Cyc_Declarator_tok[ 19u]; struct Cyc_Declarator_tok_struct{
unsigned char* tag; struct Cyc_Declarator* f1; } ; extern unsigned char Cyc_DesignatorList_tok[
23u]; struct Cyc_DesignatorList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Designator_tok[ 19u]; struct Cyc_Designator_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_EnumfieldList_tok[ 22u];
struct Cyc_EnumfieldList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Enumfield_tok[ 18u]; struct Cyc_Enumfield_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Enumfield* f1; } ; extern unsigned char Cyc_ExpList_tok[
16u]; struct Cyc_ExpList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Exp_tok[ 12u]; struct Cyc_Exp_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Exp* f1; } ; extern unsigned char Cyc_FieldPatternList_tok[
25u]; struct Cyc_FieldPatternList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_FieldPattern_tok[ 21u]; struct _tuple4{ struct
Cyc_List_List* f1; struct Cyc_Absyn_Pat* f2; } ; struct Cyc_FieldPattern_tok_struct{
unsigned char* tag; struct _tuple4* f1; } ; extern unsigned char Cyc_FnDecl_tok[
15u]; struct Cyc_FnDecl_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Fndecl*
f1; } ; extern unsigned char Cyc_IdList_tok[ 15u]; struct Cyc_IdList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_InitDeclList_tok[
21u]; struct Cyc_InitDeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_InitDecl_tok[ 17u]; struct Cyc_InitDecl_tok_struct{
unsigned char* tag; struct _tuple3* f1; } ; extern unsigned char Cyc_InitializerList_tok[
24u]; struct Cyc_InitializerList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Int_tok[ 12u]; struct _tuple5{ void* f1; int f2;
} ; struct Cyc_Int_tok_struct{ unsigned char* tag; struct _tuple5* f1; } ;
extern unsigned char Cyc_Kind_tok[ 13u]; struct Cyc_Kind_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Okay_tok[ 13u];
extern unsigned char Cyc_ParamDeclListBool_tok[ 26u]; struct _tuple6{ struct Cyc_List_List*
f1; int f2; struct Cyc_Absyn_VarargInfo* f3; struct Cyc_Core_Opt* f4; struct Cyc_List_List*
f5; } ; struct Cyc_ParamDeclListBool_tok_struct{ unsigned char* tag; struct
_tuple6* f1; } ; extern unsigned char Cyc_ParamDeclList_tok[ 22u]; struct Cyc_ParamDeclList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_ParamDecl_tok[
18u]; struct Cyc_ParamDecl_tok_struct{ unsigned char* tag; struct _tuple1* f1; }
; extern unsigned char Cyc_PatternList_tok[ 20u]; struct Cyc_PatternList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_Pattern_tok[
16u]; struct Cyc_Pattern_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Pat*
f1; } ; extern unsigned char Cyc_Pointer_Sort_tok[ 21u]; struct Cyc_Pointer_Sort_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Primop_tok[ 15u];
struct Cyc_Primop_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_Primopopt_tok[ 18u]; struct Cyc_Primopopt_tok_struct{
unsigned char* tag; struct Cyc_Core_Opt* f1; } ; extern unsigned char Cyc_QualId_tok[
15u]; struct Cyc_QualId_tok_struct{ unsigned char* tag; struct _tuple0* f1; } ;
extern unsigned char Cyc_QualSpecList_tok[ 21u]; struct _tuple7{ struct Cyc_Absyn_Tqual
f1; struct Cyc_List_List* f2; struct Cyc_List_List* f3; } ; struct Cyc_QualSpecList_tok_struct{
unsigned char* tag; struct _tuple7* f1; } ; extern unsigned char Cyc_Rgnorder_tok[
17u]; struct Cyc_Rgnorder_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_Scope_tok[ 14u]; struct Cyc_Scope_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Short_tok[ 14u];
struct Cyc_Short_tok_struct{ unsigned char* tag; short f1; } ; extern
unsigned char Cyc_Stmt_tok[ 13u]; struct Cyc_Stmt_tok_struct{ unsigned char* tag;
struct Cyc_Absyn_Stmt* f1; } ; extern unsigned char Cyc_StorageClass_tok[ 21u];
struct Cyc_StorageClass_tok_struct{ unsigned char* tag; void* f1; } ; extern
unsigned char Cyc_String_tok[ 15u]; struct Cyc_String_tok_struct{ unsigned char*
tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Stringopt_tok[ 18u];
struct Cyc_Stringopt_tok_struct{ unsigned char* tag; struct Cyc_Core_Opt* f1; }
; extern unsigned char Cyc_StructFieldDeclListList_tok[ 32u]; struct Cyc_StructFieldDeclListList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_StructFieldDeclList_tok[
28u]; struct Cyc_StructFieldDeclList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_StructOrUnion_tok[ 22u]; struct Cyc_StructOrUnion_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_SwitchCClauseList_tok[
26u]; struct Cyc_SwitchCClauseList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_SwitchClauseList_tok[ 25u]; struct Cyc_SwitchClauseList_tok_struct{
unsigned char* tag; struct Cyc_List_List* f1; } ; extern unsigned char Cyc_TunionFieldList_tok[
24u]; struct Cyc_TunionFieldList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_TunionField_tok[ 20u]; struct Cyc_TunionField_tok_struct{
unsigned char* tag; struct Cyc_Absyn_Tunionfield* f1; } ; extern unsigned char
Cyc_TypeList_tok[ 17u]; struct Cyc_TypeList_tok_struct{ unsigned char* tag;
struct Cyc_List_List* f1; } ; extern unsigned char Cyc_TypeModifierList_tok[ 25u];
struct Cyc_TypeModifierList_tok_struct{ unsigned char* tag; struct Cyc_List_List*
f1; } ; extern unsigned char Cyc_TypeOpt_tok[ 16u]; struct Cyc_TypeOpt_tok_struct{
unsigned char* tag; struct Cyc_Core_Opt* f1; } ; extern unsigned char Cyc_TypeQual_tok[
17u]; struct Cyc_TypeQual_tok_struct{ unsigned char* tag; struct Cyc_Absyn_Tqual
f1; } ; extern unsigned char Cyc_TypeSpecifier_tok[ 22u]; struct Cyc_TypeSpecifier_tok_struct{
unsigned char* tag; void* f1; } ; extern unsigned char Cyc_Type_tok[ 13u];
struct Cyc_Type_tok_struct{ unsigned char* tag; void* f1; } ; struct Cyc_Yyltype{
int timestamp; int first_line; int first_column; int last_line; int last_column;
} ; struct Cyc_PP_Ppstate; struct Cyc_PP_Out; struct Cyc_PP_Doc; struct Cyc_Absynpp_Params{
int expand_typedefs: 1; int qvar_to_Cids: 1; int add_cyc_prefix: 1; int to_VC: 1;
int decls_first: 1; int rewrite_temp_tvars: 1; int print_all_tvars: 1; int
print_all_kinds: 1; int print_using_stmts: 1; int print_externC_stmts: 1; int
print_full_evars: 1; int use_curr_namespace: 1; struct Cyc_List_List*
curr_namespace; } ; extern void Cyc_Absynpp_set_params( struct Cyc_Absynpp_Params*
fs); extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r; extern struct
Cyc_Absynpp_Params Cyc_Absynpp_c_params_r; extern void Cyc_Absynpp_decllist2file(
struct Cyc_List_List* tdl, struct Cyc_std___sFILE* f); extern void Cyc_Absyndump_set_params(
struct Cyc_Absynpp_Params* fs); extern void Cyc_Absyndump_dumpdecllist2file(
struct Cyc_List_List* tdl, struct Cyc_std___sFILE* f); struct Cyc_Set_Set;
extern unsigned char Cyc_Set_Absent[ 11u]; static const int Cyc_Tcenv_VarRes= 0;
struct Cyc_Tcenv_VarRes_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_StructRes=
1; struct Cyc_Tcenv_StructRes_struct{ int tag; struct Cyc_Absyn_Structdecl* f1;
} ; static const int Cyc_Tcenv_TunionRes= 2; struct Cyc_Tcenv_TunionRes_struct{
int tag; struct Cyc_Absyn_Tuniondecl* f1; struct Cyc_Absyn_Tunionfield* f2; } ;
static const int Cyc_Tcenv_EnumRes= 3; struct Cyc_Tcenv_EnumRes_struct{ int tag;
struct Cyc_Absyn_Enumdecl* f1; struct Cyc_Absyn_Enumfield* f2; } ; struct Cyc_Tcenv_Genv{
struct Cyc_Set_Set* namespaces; struct Cyc_Dict_Dict* structdecls; struct Cyc_Dict_Dict*
uniondecls; struct Cyc_Dict_Dict* tuniondecls; struct Cyc_Dict_Dict* enumdecls;
struct Cyc_Dict_Dict* typedefs; struct Cyc_Dict_Dict* ordinaries; struct Cyc_List_List*
availables; } ; struct Cyc_Tcenv_Fenv; static const int Cyc_Tcenv_NotLoop_j= 0;
static const int Cyc_Tcenv_CaseEnd_j= 1; static const int Cyc_Tcenv_FnEnd_j= 2;
static const int Cyc_Tcenv_Stmt_j= 0; struct Cyc_Tcenv_Stmt_j_struct{ int tag;
struct Cyc_Absyn_Stmt* f1; } ; static const int Cyc_Tcenv_Outermost= 0; struct
Cyc_Tcenv_Outermost_struct{ int tag; void* f1; } ; static const int Cyc_Tcenv_Frame=
1; struct Cyc_Tcenv_Frame_struct{ int tag; void* f1; void* f2; } ; static const
int Cyc_Tcenv_Hidden= 2; struct Cyc_Tcenv_Hidden_struct{ int tag; void* f1; void*
f2; } ; struct Cyc_Tcenv_Tenv{ struct Cyc_List_List* ns; struct Cyc_Dict_Dict*
ae; struct Cyc_Core_Opt* le; } ; extern struct Cyc_Tcenv_Tenv* Cyc_Tcenv_tc_init();
extern unsigned char Cyc_Tcutil_TypeErr[ 12u]; extern void Cyc_Tcutil_flush_warnings();
extern void Cyc_Tc_tc( struct Cyc_Tcenv_Tenv* te, int var_default_init, struct
Cyc_List_List* ds); extern struct Cyc_List_List* Cyc_Tc_treeshake( struct Cyc_Tcenv_Tenv*
te, struct Cyc_List_List*); extern struct Cyc_List_List* Cyc_Toc_toc( struct Cyc_List_List*
ds); extern struct Cyc_List_List* Cyc_Tovc_tovc( struct Cyc_List_List* decls);
struct Cyc_CfFlowInfo_Place; static const int Cyc_CfFlowInfo_VarRoot= 0; struct
Cyc_CfFlowInfo_VarRoot_struct{ int tag; struct Cyc_Absyn_Vardecl* f1; } ; static
const int Cyc_CfFlowInfo_MallocPt= 1; struct Cyc_CfFlowInfo_MallocPt_struct{ int
tag; struct Cyc_Absyn_Exp* f1; } ; static const int Cyc_CfFlowInfo_StructF= 0;
struct Cyc_CfFlowInfo_StructF_struct{ int tag; struct _tagged_arr* f1; } ;
static const int Cyc_CfFlowInfo_TupleF= 1; struct Cyc_CfFlowInfo_TupleF_struct{
int tag; int f1; } ; struct Cyc_CfFlowInfo_Place{ void* root; struct Cyc_List_List*
fields; } ; static const int Cyc_CfFlowInfo_Esc= 0; static const int Cyc_CfFlowInfo_Unesc=
1; static const int Cyc_CfFlowInfo_NoneIL= 0; static const int Cyc_CfFlowInfo_ThisIL=
1; static const int Cyc_CfFlowInfo_AllIL= 2; static const int Cyc_CfFlowInfo_UnknownIS=
0; struct Cyc_CfFlowInfo_UnknownIS_struct{ int tag; void* f1; void* f2; } ;
static const int Cyc_CfFlowInfo_MustPointTo= 1; struct Cyc_CfFlowInfo_MustPointTo_struct{
int tag; struct Cyc_CfFlowInfo_Place* f1; } ; static const int Cyc_CfFlowInfo_LeafPI=
0; struct Cyc_CfFlowInfo_LeafPI_struct{ int tag; void* f1; } ; static const int
Cyc_CfFlowInfo_TuplePI= 1; struct Cyc_CfFlowInfo_TuplePI_struct{ int tag; struct
Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_StructPI= 2; struct Cyc_CfFlowInfo_StructPI_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; static const int Cyc_CfFlowInfo_BottomFL=
0; static const int Cyc_CfFlowInfo_InitsFL= 0; struct Cyc_CfFlowInfo_InitsFL_struct{
int tag; struct Cyc_Dict_Dict* f1; } ; struct Cyc_NewControlFlow_AnalEnv{ struct
Cyc_Dict_Dict* roots; int in_try; void* tryflow; } ; extern void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List* ds); struct Cyc_Interface_I; extern struct Cyc_Interface_I*
Cyc_Interface_empty(); extern struct Cyc_Interface_I* Cyc_Interface_final();
extern struct Cyc_Interface_I* Cyc_Interface_extract( struct Cyc_Dict_Dict* ae);
struct _tuple8{ struct _tagged_arr f1; struct _tagged_arr f2; } ; extern int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I* i1, struct Cyc_Interface_I* i2, struct _tuple8* info);
extern struct Cyc_Interface_I* Cyc_Interface_get_and_merge_list( struct Cyc_Interface_I*(*
get)( void*), struct Cyc_List_List* la, struct Cyc_List_List* linfo); extern
struct Cyc_Interface_I* Cyc_Interface_parse( struct Cyc_std___sFILE*); extern
void Cyc_Interface_save( struct Cyc_Interface_I*, struct Cyc_std___sFILE*);
extern struct Cyc_Interface_I* Cyc_Interface_load( struct Cyc_std___sFILE*);
extern void Cyc_Lex_lex_init(); static int Cyc_pp_r= 0; static int Cyc_noexpand_r=
0; static int Cyc_noshake_r= 0; static int Cyc_stop_after_cpp_r= 0; static int
Cyc_parseonly_r= 0; static int Cyc_tc_r= 0; static int Cyc_ic_r= 0; static int
Cyc_toc_r= 0; static int Cyc_stop_after_objectfile_r= 0; static int Cyc_stop_after_asmfile_r=
0; static int Cyc_tovc_r= 0; static int Cyc_v_r= 0; static int Cyc_save_temps_r=
0; static int Cyc_save_c_r= 0; static int Cyc_nogc_r= 0; static int Cyc_pa_r= 0;
static int Cyc_add_cyc_namespace_r= 1; static int Cyc_print_full_evars_r= 0;
static int Cyc_print_all_tvars_r= 0; static int Cyc_print_all_kinds_r= 0; static
struct _tagged_arr* Cyc_output_file= 0; static void Cyc_set_output_file( struct
_tagged_arr s){ Cyc_output_file=({ struct _tagged_arr* _temp0=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp0[ 0]= s; _temp0;});}
extern unsigned char* Cdef_inc_path; extern unsigned char* Cdef_lib_path; extern
unsigned char* Carch; static unsigned char _temp1[ 33u]="gcc -x c -E -U__GNUC__ -nostdinc";
static struct _tagged_arr Cyc_cpp={ _temp1, _temp1, _temp1 +  33u}; static void
Cyc_set_cpp( struct _tagged_arr s){ Cyc_cpp= s;} static struct Cyc_List_List*
Cyc_cppargs= 0; static void Cyc_add_cpparg( struct _tagged_arr s){ Cyc_cppargs=({
struct Cyc_List_List* _temp2=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp2->hd=( void*)({ struct _tagged_arr* _temp3=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp3[ 0]= s; _temp3;});
_temp2->tl= Cyc_cppargs; _temp2;});} static int Cyc_is_cyclone_sourcefile(
struct _tagged_arr s){ unsigned int _temp4= Cyc_std_strlen( s); if( _temp4 <=  4){
return 0;} else{ return Cyc_std_strcmp( _tagged_arr_plus( s, sizeof(
unsigned char),( int)( _temp4 -  4)), _tag_arr(".cyc", sizeof( unsigned char), 5u))
==  0;}} static struct Cyc_List_List* Cyc_cyclone_lib_path= 0; static void Cyc_add_cyclone_lib_path(
struct _tagged_arr s){ unsigned int _temp5= Cyc_std_strlen( s); if( _temp5 <=  2){
return;}{ struct _tagged_arr _temp6=( struct _tagged_arr) Cyc_std_substring( s,
2, _temp5 -  2); Cyc_cyclone_lib_path=({ struct Cyc_List_List* _temp7=( struct
Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp7->hd=( void*)({
struct _tagged_arr* _temp8=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp8[ 0]= _temp6; _temp8;}); _temp7->tl= Cyc_cyclone_lib_path;
_temp7;});}} static struct Cyc_List_List* Cyc_ccargs= 0; static void Cyc_add_ccarg(
struct _tagged_arr s){ Cyc_ccargs=({ struct Cyc_List_List* _temp9=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp9->hd=( void*)({ struct
_tagged_arr* _temp10=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp10[ 0]= s; _temp10;}); _temp9->tl= Cyc_ccargs; _temp9;});}
static void Cyc_add_ccarg2( struct _tagged_arr flag, struct _tagged_arr arg){
Cyc_ccargs=({ struct Cyc_List_List* _temp11=( struct Cyc_List_List*) GC_malloc(
sizeof( struct Cyc_List_List)); _temp11->hd=( void*)({ struct _tagged_arr*
_temp14=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp14[
0]= arg; _temp14;}); _temp11->tl=({ struct Cyc_List_List* _temp12=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp12->hd=( void*)({ struct
_tagged_arr* _temp13=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp13[ 0]= flag; _temp13;}); _temp12->tl= Cyc_ccargs; _temp12;});
_temp11;});} static struct Cyc_List_List* Cyc_libargs= 0; static void Cyc_add_libarg(
struct _tagged_arr s){ Cyc_libargs=({ struct Cyc_List_List* _temp15=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp15->hd=( void*)({ struct
_tagged_arr* _temp16=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp16[ 0]= s; _temp16;}); _temp15->tl= Cyc_libargs; _temp15;});}
static void Cyc_set_save_temps(){ Cyc_save_temps_r= 1; Cyc_add_ccarg( _tag_arr("-save-temps",
sizeof( unsigned char), 12u));} static int Cyc_produce_dependencies= 0; static
void Cyc_set_produce_dependencies(){ Cyc_stop_after_cpp_r= 1; Cyc_produce_dependencies=
1; Cyc_add_cpparg( _tag_arr("-M", sizeof( unsigned char), 3u));} static struct
_tagged_arr* Cyc_dependencies_target= 0; static void Cyc_set_dependencies_target(
struct _tagged_arr s){ Cyc_dependencies_target=({ struct _tagged_arr* _temp17=(
struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp17[ 0]= s;
_temp17;});} static void Cyc_set_stop_after_objectfile(){ Cyc_stop_after_objectfile_r=
1; Cyc_add_ccarg( _tag_arr("-c", sizeof( unsigned char), 3u));} static void Cyc_set_nocyc(){
Cyc_add_cyc_namespace_r= 0; Cyc_add_ccarg( _tag_arr("-DNO_CYC_PREFIX", sizeof(
unsigned char), 16u));} static void Cyc_set_pa(){ Cyc_pa_r= 1; Cyc_add_ccarg(
_tag_arr("-DCYC_REGION_PROFILE", sizeof( unsigned char), 21u));} static void Cyc_set_stop_after_asmfile(){
Cyc_stop_after_asmfile_r= 1; Cyc_add_ccarg( _tag_arr("-S", sizeof( unsigned char),
3u));} static struct Cyc_List_List* Cyc_cyclone_files= 0; static void Cyc_add_other(
struct _tagged_arr s){ if( Cyc_is_cyclone_sourcefile( s)){ Cyc_cyclone_files=({
struct Cyc_List_List* _temp18=( struct Cyc_List_List*) GC_malloc( sizeof( struct
Cyc_List_List)); _temp18->hd=( void*)({ struct _tagged_arr* _temp19=( struct
_tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp19[ 0]= s; _temp19;});
_temp18->tl= Cyc_cyclone_files; _temp18;});{ struct _tagged_arr _temp20= Cyc_Filename_chop_extension(
s); struct _tagged_arr _temp21= Cyc_std_strconcat(( struct _tagged_arr) _temp20,
_tag_arr(".c", sizeof( unsigned char), 3u)); Cyc_add_ccarg(( struct _tagged_arr)
_temp21);}} else{ Cyc_add_ccarg( s);}} static void Cyc_remove_file( struct
_tagged_arr s){ if( Cyc_save_temps_r){ return;} else{ Cyc_std_remove( s);}} int
Cyc_compile_failure= 0; struct Cyc_std___sFILE* Cyc_try_file_open( struct
_tagged_arr filename, struct _tagged_arr mode, struct _tagged_arr msg_part){
struct _handler_cons _temp22; _push_handler(& _temp22);{ int _temp24= 0; if(
setjmp( _temp22.handler)){ _temp24= 1;} if( ! _temp24){{ struct Cyc_std___sFILE*
_temp25=( struct Cyc_std___sFILE*) Cyc_std_file_open( filename, mode);
_npop_handler( 0u); return _temp25;}; _pop_handler();} else{ void* _temp23=(
void*) _exn_thrown; void* _temp27= _temp23; _LL29: goto _LL30; _LL31: goto _LL32;
_LL30: Cyc_compile_failure= 1;({ struct Cyc_std_String_pa_struct _temp35;
_temp35.tag= Cyc_std_String_pa; _temp35.f1=( struct _tagged_arr) filename;{
struct Cyc_std_String_pa_struct _temp34; _temp34.tag= Cyc_std_String_pa; _temp34.f1=(
struct _tagged_arr) msg_part;{ void* _temp33[ 2u]={& _temp34,& _temp35}; Cyc_std_fprintf(
Cyc_std_stderr, _tag_arr("\nError: couldn't open %s %s\n", sizeof( unsigned char),
29u), _tag_arr( _temp33, sizeof( void*), 2u));}}}); Cyc_std_fflush(( struct Cyc_std___sFILE*)
Cyc_std_stderr); return 0; _LL32:( void) _throw( _temp27); _LL28:;}}} struct Cyc_List_List*
Cyc_do_stage( struct _tagged_arr stage_name, struct Cyc_List_List*(* f)( void*,
struct Cyc_List_List*), void* env, struct Cyc_List_List* tds){ struct
_tagged_arr exn_string= _tag_arr("", sizeof( unsigned char), 1u); struct
_tagged_arr explain_string= _tag_arr("", sizeof( unsigned char), 1u); int
other_exn= 0; void* ex=( void*)({ struct Cyc_Core_Impossible_struct* _temp64=(
struct Cyc_Core_Impossible_struct*) GC_malloc( sizeof( struct Cyc_Core_Impossible_struct));
_temp64[ 0]=({ struct Cyc_Core_Impossible_struct _temp65; _temp65.tag= Cyc_Core_Impossible;
_temp65.f1= _tag_arr("", sizeof( unsigned char), 1u); _temp65;}); _temp64;});
struct Cyc_List_List* _temp36= 0;{ struct _handler_cons _temp37; _push_handler(&
_temp37);{ int _temp39= 0; if( setjmp( _temp37.handler)){ _temp39= 1;} if( !
_temp39){ _temp36= f( env, tds);; _pop_handler();} else{ void* _temp38=( void*)
_exn_thrown; void* _temp41= _temp38; struct _tagged_arr _temp53; struct
_tagged_arr _temp55; _LL43: if(*(( void**) _temp41) ==  Cyc_Core_Impossible){
_LL54: _temp53=(( struct Cyc_Core_Impossible_struct*) _temp41)->f1; goto _LL44;}
else{ goto _LL45;} _LL45: if( _temp41 ==  Cyc_Dict_Absent){ goto _LL46;} else{
goto _LL47;} _LL47: if(*(( void**) _temp41) ==  Cyc_Core_InvalidArg){ _LL56:
_temp55=(( struct Cyc_Core_InvalidArg_struct*) _temp41)->f1; goto _LL48;} else{
goto _LL49;} _LL49: goto _LL50; _LL51: goto _LL52; _LL44: exn_string= _tag_arr("Exception Core::Impossible",
sizeof( unsigned char), 27u); explain_string= _temp53; goto _LL42; _LL46:
exn_string= _tag_arr("Exception Dict::Absent", sizeof( unsigned char), 23u);
goto _LL42; _LL48: exn_string= _tag_arr("Exception Core::InvalidArg", sizeof(
unsigned char), 27u); explain_string= _temp55; goto _LL42; _LL50: ex= _temp41;
other_exn= 1; exn_string= _tag_arr("Uncaught exception", sizeof( unsigned char),
19u); goto _LL42; _LL52:( void) _throw( _temp41); _LL42:;}}} if( Cyc_Position_error_p()){
Cyc_compile_failure= 1;} if( Cyc_std_strcmp( exn_string, _tag_arr("", sizeof(
unsigned char), 1u)) !=  0){ Cyc_compile_failure= 1;({ struct Cyc_std_String_pa_struct
_temp60; _temp60.tag= Cyc_std_String_pa; _temp60.f1=( struct _tagged_arr)
explain_string;{ struct Cyc_std_String_pa_struct _temp59; _temp59.tag= Cyc_std_String_pa;
_temp59.f1=( struct _tagged_arr) stage_name;{ struct Cyc_std_String_pa_struct
_temp58; _temp58.tag= Cyc_std_String_pa; _temp58.f1=( struct _tagged_arr)
exn_string;{ void* _temp57[ 3u]={& _temp58,& _temp59,& _temp60}; Cyc_std_fprintf(
Cyc_std_stderr, _tag_arr("\n%s thrown during %s: %s", sizeof( unsigned char), 25u),
_tag_arr( _temp57, sizeof( void*), 3u));}}}});} if( Cyc_compile_failure){({ void*
_temp61[ 0u]={}; Cyc_std_fprintf( Cyc_std_stderr, _tag_arr("\nCOMPILATION FAILED!\n",
sizeof( unsigned char), 22u), _tag_arr( _temp61, sizeof( void*), 0u));}); Cyc_std_fflush((
struct Cyc_std___sFILE*) Cyc_std_stderr); if( other_exn){( int) _throw( ex);}
return _temp36;} else{ if( Cyc_v_r){({ struct Cyc_std_String_pa_struct _temp63;
_temp63.tag= Cyc_std_String_pa; _temp63.f1=( struct _tagged_arr) stage_name;{
void* _temp62[ 1u]={& _temp63}; Cyc_std_fprintf( Cyc_std_stderr, _tag_arr("%s completed.\n",
sizeof( unsigned char), 15u), _tag_arr( _temp62, sizeof( void*), 1u));}}); Cyc_std_fflush((
struct Cyc_std___sFILE*) Cyc_std_stderr); return _temp36;}} return _temp36;}
struct Cyc_List_List* Cyc_do_parse( struct Cyc_std___sFILE* f, struct Cyc_List_List*
ignore){ Cyc_Lex_lex_init();{ struct Cyc_List_List* _temp66= Cyc_Parse_parse_file(
f); Cyc_Lex_lex_init(); return _temp66;}} struct Cyc_List_List* Cyc_do_typecheck(
struct Cyc_Tcenv_Tenv* te, struct Cyc_List_List* tds){ Cyc_Tc_tc( te, 1, tds);
if( ! Cyc_noshake_r){ tds= Cyc_Tc_treeshake( te, tds);} return tds;} struct Cyc_List_List*
Cyc_do_cfcheck( int ignore, struct Cyc_List_List* tds){ Cyc_NewControlFlow_cf_check(
tds); return tds;} struct _tuple9{ struct Cyc_Tcenv_Tenv* f1; struct Cyc_std___sFILE*
f2; struct Cyc_std___sFILE* f3; } ; struct Cyc_List_List* Cyc_do_interface(
struct _tuple9* params, struct Cyc_List_List* tds){ struct _tuple9 _temp69;
struct Cyc_std___sFILE* _temp70; struct Cyc_std___sFILE* _temp72; struct Cyc_Tcenv_Tenv*
_temp74; struct _tuple9* _temp67= params; _temp69=* _temp67; _LL75: _temp74=
_temp69.f1; goto _LL73; _LL73: _temp72= _temp69.f2; goto _LL71; _LL71: _temp70=
_temp69.f3; goto _LL68; _LL68: { struct Cyc_Interface_I* _temp76= Cyc_Interface_extract(
_temp74->ae); if( _temp72 ==  0){ Cyc_Interface_save( _temp76, _temp70);} else{
struct Cyc_Interface_I* _temp77= Cyc_Interface_parse(( struct Cyc_std___sFILE*)
_check_null( _temp72)); if( ! Cyc_Interface_is_subinterface( _temp77, _temp76,({
struct _tuple8* _temp78=( struct _tuple8*) GC_malloc( sizeof( struct _tuple8));
_temp78->f1= _tag_arr("written interface", sizeof( unsigned char), 18u); _temp78->f2=
_tag_arr("maximal interface", sizeof( unsigned char), 18u); _temp78;}))){ Cyc_compile_failure=
1;} else{ Cyc_Interface_save( _temp77, _temp70);}} return tds;}} struct Cyc_List_List*
Cyc_do_translate( int ignore, struct Cyc_List_List* tds){ return Cyc_Toc_toc(
tds);} struct Cyc_List_List* Cyc_do_tovc( int ignore, struct Cyc_List_List* tds){
return Cyc_Tovc_tovc( tds);} struct Cyc_List_List* Cyc_do_print( struct Cyc_std___sFILE*
out_file, struct Cyc_List_List* tds){ struct Cyc_Absynpp_Params params_r; if(
Cyc_tc_r){ params_r= Cyc_Absynpp_cyc_params_r;} else{ params_r= Cyc_Absynpp_c_params_r;}
params_r.expand_typedefs= ! Cyc_noexpand_r; params_r.to_VC= Cyc_tovc_r; params_r.add_cyc_prefix=
Cyc_add_cyc_namespace_r; params_r.print_full_evars= Cyc_print_full_evars_r;
params_r.print_all_tvars= Cyc_print_all_tvars_r; params_r.print_all_kinds= Cyc_print_all_kinds_r;
if( Cyc_pp_r){ Cyc_Absynpp_set_params(& params_r); Cyc_Absynpp_decllist2file(
tds, out_file);} else{ Cyc_Absyndump_set_params(& params_r); Cyc_Absyndump_dumpdecllist2file(
tds, out_file);} Cyc_std_fflush(( struct Cyc_std___sFILE*) out_file); return tds;}
static struct Cyc_List_List* Cyc_cfiles= 0; static void Cyc_remove_cfiles(){ if(
! Cyc_save_c_r){ for( 0; Cyc_cfiles !=  0; Cyc_cfiles=(( struct Cyc_List_List*)
_check_null( Cyc_cfiles))->tl){ Cyc_remove_file(*(( struct _tagged_arr*)((
struct Cyc_List_List*) _check_null( Cyc_cfiles))->hd));}}} static struct Cyc_List_List*
Cyc_split_by_char( struct _tagged_arr s, unsigned char c){ if( s.curr == ((
struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ return 0;}{ struct Cyc_List_List*
_temp79= 0; unsigned int _temp80= Cyc_std_strlen( s); while( _temp80 >  0) {
struct _tagged_arr _temp81= Cyc_std_strchr( s, c); if( _temp81.curr == (( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){ _temp79=({ struct Cyc_List_List*
_temp82=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp82->hd=( void*)({ struct _tagged_arr* _temp83=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp83[ 0]= s; _temp83;}); _temp82->tl=
_temp79; _temp82;}); break;} else{ _temp79=({ struct Cyc_List_List* _temp84=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp84->hd=(
void*)({ struct _tagged_arr* _temp85=( struct _tagged_arr*) GC_malloc( sizeof(
struct _tagged_arr)); _temp85[ 0]=( struct _tagged_arr) Cyc_std_substring( s, 0,(
unsigned int)(((( struct _tagged_arr) _temp81).curr -  s.curr) /  sizeof(
unsigned char))); _temp85;}); _temp84->tl= _temp79; _temp84;}); _temp80 -=(((
struct _tagged_arr) _temp81).curr -  s.curr) /  sizeof( unsigned char); s=
_tagged_arr_plus( _temp81, sizeof( unsigned char), 1);}} return(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_imp_rev)( _temp79);}} static int Cyc_file_exists(
struct _tagged_arr file){ struct Cyc_std___sFILE* f= 0;{ struct _handler_cons
_temp86; _push_handler(& _temp86);{ int _temp88= 0; if( setjmp( _temp86.handler)){
_temp88= 1;} if( ! _temp88){ f=( struct Cyc_std___sFILE*) Cyc_std_file_open(
file, _tag_arr("r", sizeof( unsigned char), 2u));; _pop_handler();} else{ void*
_temp87=( void*) _exn_thrown; void* _temp90= _temp87; _LL92: goto _LL93; _LL94:
goto _LL95; _LL93: goto _LL91; _LL95:( void) _throw( _temp90); _LL91:;}}} if( f
==  0){ return 0;} else{ Cyc_std_fclose(( struct Cyc_std___sFILE*) _check_null(
f)); return 1;}} static struct _tagged_arr* Cyc_find( struct Cyc_List_List* dirs,
struct _tagged_arr file){ if( file.curr == (( struct _tagged_arr) _tag_arr( 0u,
0u, 0u)).curr){ return 0;} for( 0; dirs !=  0; dirs=(( struct Cyc_List_List*)
_check_null( dirs))->tl){ struct _tagged_arr _temp96=*(( struct _tagged_arr*)((
struct Cyc_List_List*) _check_null( dirs))->hd); if( _temp96.curr == (( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr? 1: Cyc_std_strlen( _temp96) ==  0){
continue;}{ struct _tagged_arr s=( struct _tagged_arr) Cyc_Filename_concat(
_temp96, file); if( Cyc_file_exists( s)){ return({ struct _tagged_arr* _temp97=(
struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp97[ 0]= s;
_temp97;});}}} return 0;} static struct _tagged_arr Cyc_do_find( struct Cyc_List_List*
dirs, struct _tagged_arr file){ struct _tagged_arr* _temp98= Cyc_find( dirs,
file); if( _temp98 ==  0){({ struct Cyc_std_String_pa_struct _temp100; _temp100.tag=
Cyc_std_String_pa; _temp100.f1=( struct _tagged_arr) file;{ void* _temp99[ 1u]={&
_temp100}; Cyc_std_fprintf( Cyc_std_stderr, _tag_arr("Error: can't find internal compiler file %s",
sizeof( unsigned char), 44u), _tag_arr( _temp99, sizeof( void*), 1u));}}); Cyc_compile_failure=
1; Cyc_remove_cfiles(); exit( 1);} return*(( struct _tagged_arr*) _check_null(
_temp98));} static int Cyc_is_other_special( unsigned char c){ switch( c){ case
'\\': _LL101: goto _LL102; case '"': _LL102: goto _LL103; case ';': _LL103: goto
_LL104; case '&': _LL104: goto _LL105; case '(': _LL105: goto _LL106; case ')':
_LL106: goto _LL107; case '|': _LL107: goto _LL108; case '^': _LL108: goto
_LL109; case '<': _LL109: goto _LL110; case '>': _LL110: goto _LL111; case ' ':
_LL111: goto _LL112; case '\n': _LL112: goto _LL113; case '\t': _LL113: return 1;
default: _LL114: return 0;}} static struct _tagged_arr Cyc_sh_escape_string(
struct _tagged_arr s){ unsigned int _temp116= Cyc_std_strlen( s); int _temp117=
0; int _temp118= 0;{ int i= 0; for( 0; i <  _temp116; i ++){ unsigned char
_temp119=*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), i)); if( _temp119 == '\''){ _temp117 ++;} else{ if( Cyc_is_other_special(
_temp119)){ _temp118 ++;}}}} if( _temp117 ==  0? _temp118 ==  0: 0){ return s;}
if( _temp117 ==  0){ return( struct _tagged_arr) Cyc_std_strconcat_l(({ struct
_tagged_arr* _temp120[ 3u]; _temp120[ 2u]= _init_tag_arr(( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)),"'", sizeof( unsigned char), 2u);
_temp120[ 1u]=({ struct _tagged_arr* _temp121=( struct _tagged_arr*) GC_malloc(
sizeof( struct _tagged_arr)); _temp121[ 0]=( struct _tagged_arr) s; _temp121;});
_temp120[ 0u]= _init_tag_arr(( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)),"'", sizeof( unsigned char), 2u);(( struct Cyc_List_List*(*)(
struct _tagged_arr)) Cyc_List_list)( _tag_arr( _temp120, sizeof( struct
_tagged_arr*), 3u));}));}{ unsigned int _temp122=( _temp116 +  _temp117) + 
_temp118; struct _tagged_arr _temp123=({ unsigned int _temp127= _temp122 +  1;
unsigned char* _temp128=( unsigned char*) GC_malloc_atomic( sizeof(
unsigned char) *  _temp127); struct _tagged_arr _temp130= _tag_arr( _temp128,
sizeof( unsigned char), _temp122 +  1);{ unsigned int _temp129= _temp127;
unsigned int i; for( i= 0; i <  _temp129; i ++){ _temp128[ i]='\000';}};
_temp130;}); int _temp124= 0; int _temp125= 0; for( 0; _temp124 <  _temp116;
_temp124 ++){ unsigned char _temp126=*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), _temp124)); if( _temp126 == '\''?
1: Cyc_is_other_special( _temp126)){*(( unsigned char*) _check_unknown_subscript(
_temp123, sizeof( unsigned char), _temp125 ++))='\\';}*(( unsigned char*)
_check_unknown_subscript( _temp123, sizeof( unsigned char), _temp125 ++))=
_temp126;} return( struct _tagged_arr) _temp123;}} static struct _tagged_arr*
Cyc_sh_escape_stringptr( struct _tagged_arr* sp){ return({ struct _tagged_arr*
_temp131=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr));
_temp131[ 0]= Cyc_sh_escape_string(* sp); _temp131;});} static void Cyc_process_file(
struct _tagged_arr filename){ struct _tagged_arr _temp132= Cyc_Filename_chop_extension(
filename); struct _tagged_arr _temp133= Cyc_std_strconcat(( struct _tagged_arr)
_temp132, _tag_arr(".cyp", sizeof( unsigned char), 5u)); struct _tagged_arr
_temp134= Cyc_std_strconcat(( struct _tagged_arr) _temp132, _tag_arr(".cyci",
sizeof( unsigned char), 6u)); struct _tagged_arr _temp135= Cyc_std_strconcat((
struct _tagged_arr) _temp132, _tag_arr(".cycio", sizeof( unsigned char), 7u));
struct _tagged_arr _temp136= Cyc_std_strconcat(( struct _tagged_arr) _temp132,
_tag_arr(".c", sizeof( unsigned char), 3u)); if( Cyc_v_r){({ struct Cyc_std_String_pa_struct
_temp138; _temp138.tag= Cyc_std_String_pa; _temp138.f1=( struct _tagged_arr)
filename;{ void* _temp137[ 1u]={& _temp138}; Cyc_std_fprintf( Cyc_std_stderr,
_tag_arr("Compiling %s\n", sizeof( unsigned char), 14u), _tag_arr( _temp137,
sizeof( void*), 1u));}});}{ struct Cyc_std___sFILE* f0= Cyc_try_file_open(
filename, _tag_arr("r", sizeof( unsigned char), 2u), _tag_arr("input file",
sizeof( unsigned char), 11u)); if( Cyc_compile_failure){ return;} Cyc_std_fclose((
struct Cyc_std___sFILE*) _check_null( f0));{ struct _tagged_arr _temp139= Cyc_std_str_sepstr(({
struct Cyc_List_List* _temp193=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp193->hd=( void*) _init_tag_arr(( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)),"", sizeof( unsigned char), 1u);
_temp193->tl=(( struct Cyc_List_List*(*)( struct _tagged_arr*(* f)( struct
_tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)( Cyc_sh_escape_stringptr,((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_cppargs));
_temp193;}), _tag_arr(" ", sizeof( unsigned char), 2u)); struct _tagged_arr
stdinc_string; struct _tagged_arr def_inc_path=( struct _tagged_arr)
Cstring_to_string( Cdef_inc_path); if( Cyc_std_strlen( def_inc_path) >  0){
stdinc_string=( struct _tagged_arr)({ struct Cyc_std_String_pa_struct _temp141;
_temp141.tag= Cyc_std_String_pa; _temp141.f1=( struct _tagged_arr) def_inc_path;{
void* _temp140[ 1u]={& _temp141}; Cyc_std_aprintf( _tag_arr(" -I%s", sizeof(
unsigned char), 6u), _tag_arr( _temp140, sizeof( void*), 1u));}});} else{
stdinc_string= _tag_arr("", sizeof( unsigned char), 1u);}{ struct _tagged_arr
_temp142= Cyc_std_getenv( _tag_arr("CYCLONE_INCLUDE_PATH", sizeof( unsigned char),
21u)); if( _temp142.curr != ( _tag_arr( 0u, 0u, 0u)).curr){ struct Cyc_List_List*
_temp143= Cyc_split_by_char(( struct _tagged_arr) _temp142,':'); struct
_tagged_arr _temp144= Cyc_std_str_sepstr(({ struct Cyc_List_List* _temp148=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp148->hd=(
void*) _init_tag_arr(( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)),"", sizeof( unsigned char), 1u); _temp148->tl=(( struct Cyc_List_List*(*)(
struct _tagged_arr*(* f)( struct _tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_sh_escape_stringptr, _temp143); _temp148;}), _tag_arr(" -I", sizeof(
unsigned char), 4u)); stdinc_string=( struct _tagged_arr)({ struct Cyc_std_String_pa_struct
_temp147; _temp147.tag= Cyc_std_String_pa; _temp147.f1=( struct _tagged_arr)
stdinc_string;{ struct Cyc_std_String_pa_struct _temp146; _temp146.tag= Cyc_std_String_pa;
_temp146.f1=( struct _tagged_arr) _temp144;{ void* _temp145[ 2u]={& _temp146,&
_temp147}; Cyc_std_aprintf( _tag_arr("%s%s", sizeof( unsigned char), 5u),
_tag_arr( _temp145, sizeof( void*), 2u));}}});}{ struct _tagged_arr ofile_string;
if( Cyc_stop_after_cpp_r){ if( Cyc_output_file !=  0){ ofile_string=( struct
_tagged_arr)({ struct Cyc_std_String_pa_struct _temp150; _temp150.tag= Cyc_std_String_pa;
_temp150.f1=( struct _tagged_arr)*(( struct _tagged_arr*) _check_null( Cyc_output_file));{
void* _temp149[ 1u]={& _temp150}; Cyc_std_aprintf( _tag_arr(" > %s", sizeof(
unsigned char), 6u), _tag_arr( _temp149, sizeof( void*), 1u));}});} else{
ofile_string= _tag_arr("", sizeof( unsigned char), 1u);}} else{ ofile_string=(
struct _tagged_arr)({ struct Cyc_std_String_pa_struct _temp152; _temp152.tag=
Cyc_std_String_pa; _temp152.f1=( struct _tagged_arr) Cyc_sh_escape_string((
struct _tagged_arr) _temp133);{ void* _temp151[ 1u]={& _temp152}; Cyc_std_aprintf(
_tag_arr(" > %s", sizeof( unsigned char), 6u), _tag_arr( _temp151, sizeof( void*),
1u));}});}{ struct _tagged_arr fixup_string; if( Cyc_produce_dependencies){ if(
Cyc_dependencies_target ==  0){ fixup_string= _tag_arr(" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",
sizeof( unsigned char), 35u);} else{ fixup_string=( struct _tagged_arr)({ struct
Cyc_std_String_pa_struct _temp154; _temp154.tag= Cyc_std_String_pa; _temp154.f1=(
struct _tagged_arr)*(( struct _tagged_arr*) _check_null( Cyc_dependencies_target));{
void* _temp153[ 1u]={& _temp154}; Cyc_std_aprintf( _tag_arr(" | sed 's/^.*\\.cyc\\.o:/%s:/'",
sizeof( unsigned char), 29u), _tag_arr( _temp153, sizeof( void*), 1u));}});}}
else{ fixup_string= _tag_arr("", sizeof( unsigned char), 1u);}{ struct
_tagged_arr arch=( struct _tagged_arr) Cstring_to_string( Carch); struct
_tagged_arr _temp155=({ struct Cyc_std_String_pa_struct _temp192; _temp192.tag=
Cyc_std_String_pa; _temp192.f1=( struct _tagged_arr) ofile_string;{ struct Cyc_std_String_pa_struct
_temp191; _temp191.tag= Cyc_std_String_pa; _temp191.f1=( struct _tagged_arr)
fixup_string;{ struct Cyc_std_String_pa_struct _temp190; _temp190.tag= Cyc_std_String_pa;
_temp190.f1=( struct _tagged_arr) Cyc_sh_escape_string( filename);{ struct Cyc_std_String_pa_struct
_temp189; _temp189.tag= Cyc_std_String_pa; _temp189.f1=( struct _tagged_arr)
stdinc_string;{ struct Cyc_std_String_pa_struct _temp188; _temp188.tag= Cyc_std_String_pa;
_temp188.f1=( struct _tagged_arr) _temp139;{ struct Cyc_std_String_pa_struct
_temp187; _temp187.tag= Cyc_std_String_pa; _temp187.f1=( struct _tagged_arr)
arch;{ struct Cyc_std_String_pa_struct _temp186; _temp186.tag= Cyc_std_String_pa;
_temp186.f1=( struct _tagged_arr) Cyc_cpp;{ void* _temp185[ 7u]={& _temp186,&
_temp187,& _temp188,& _temp189,& _temp190,& _temp191,& _temp192}; Cyc_std_aprintf(
_tag_arr("%s '-D_PLATFORM_INCLUDES_=\"arch/%s.h\"'%s%s %s%s%s", sizeof(
unsigned char), 50u), _tag_arr( _temp185, sizeof( void*), 7u));}}}}}}}}); if(
Cyc_v_r){({ struct Cyc_std_String_pa_struct _temp157; _temp157.tag= Cyc_std_String_pa;
_temp157.f1=( struct _tagged_arr) _temp155;{ void* _temp156[ 1u]={& _temp157};
Cyc_std_fprintf( Cyc_std_stderr, _tag_arr("%s\n", sizeof( unsigned char), 4u),
_tag_arr( _temp156, sizeof( void*), 1u));}});} if( system( string_to_Cstring((
struct _tagged_arr) _temp155)) !=  0){ Cyc_compile_failure= 1;({ void* _temp158[
0u]={}; Cyc_std_fprintf( Cyc_std_stderr, _tag_arr("\nError: preprocessing\n",
sizeof( unsigned char), 23u), _tag_arr( _temp158, sizeof( void*), 0u));});
return;} if( Cyc_stop_after_cpp_r){ return;} Cyc_Position_reset_position((
struct _tagged_arr) _temp133);{ struct Cyc_std___sFILE* in_file= Cyc_try_file_open((
struct _tagged_arr) _temp133, _tag_arr("r", sizeof( unsigned char), 2u),
_tag_arr("file", sizeof( unsigned char), 5u)); if( Cyc_compile_failure){ return;}{
struct Cyc_List_List* tds= 0;{ struct _handler_cons _temp159; _push_handler(&
_temp159);{ int _temp161= 0; if( setjmp( _temp159.handler)){ _temp161= 1;} if( !
_temp161){ tds=(( struct Cyc_List_List*(*)( struct _tagged_arr stage_name,
struct Cyc_List_List*(* f)( struct Cyc_std___sFILE*, struct Cyc_List_List*),
struct Cyc_std___sFILE* env, struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("parsing",
sizeof( unsigned char), 8u), Cyc_do_parse,( struct Cyc_std___sFILE*) _check_null(
in_file), tds);; _pop_handler();} else{ void* _temp160=( void*) _exn_thrown;
void* _temp163= _temp160; _LL165: goto _LL166; _LL167: goto _LL168; _LL166: Cyc_std_file_close((
struct Cyc_std___sFILE*) _check_null( in_file)); Cyc_remove_file(( struct
_tagged_arr) _temp133);( int) _throw( _temp163); _LL168:( void) _throw( _temp163);
_LL164:;}}} Cyc_std_file_close(( struct Cyc_std___sFILE*) _check_null( in_file));
if( Cyc_compile_failure){ Cyc_remove_file(( struct _tagged_arr) _temp133);
return;}{ struct Cyc_Tcenv_Tenv* _temp169= Cyc_Tcenv_tc_init(); if( Cyc_parseonly_r){
goto PRINTC;} tds=(( struct Cyc_List_List*(*)( struct _tagged_arr stage_name,
struct Cyc_List_List*(* f)( struct Cyc_Tcenv_Tenv*, struct Cyc_List_List*),
struct Cyc_Tcenv_Tenv* env, struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("type checking",
sizeof( unsigned char), 14u), Cyc_do_typecheck, _temp169, tds); if( Cyc_compile_failure){
Cyc_remove_file(( struct _tagged_arr) _temp133); return;} tds=(( struct Cyc_List_List*(*)(
struct _tagged_arr stage_name, struct Cyc_List_List*(* f)( int, struct Cyc_List_List*),
int env, struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("control-flow checking",
sizeof( unsigned char), 22u), Cyc_do_cfcheck, 1, tds); if( ! Cyc_compile_failure){
Cyc_Tcutil_flush_warnings();} Cyc_remove_file(( struct _tagged_arr) _temp133);
if( Cyc_compile_failure){ return;} if( Cyc_ic_r){ struct Cyc_std___sFILE*
inter_file= Cyc_std_fopen(( struct _tagged_arr) _temp134, _tag_arr("r", sizeof(
unsigned char), 2u)); struct Cyc_std___sFILE* inter_objfile= Cyc_try_file_open((
struct _tagged_arr) _temp135, _tag_arr("w", sizeof( unsigned char), 2u),
_tag_arr("interface object file", sizeof( unsigned char), 22u)); if(
inter_objfile ==  0){ Cyc_compile_failure= 1; return;} Cyc_Position_reset_position((
struct _tagged_arr) _temp134); tds=(( struct Cyc_List_List*(*)( struct
_tagged_arr stage_name, struct Cyc_List_List*(* f)( struct _tuple9*, struct Cyc_List_List*),
struct _tuple9* env, struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("interface checking",
sizeof( unsigned char), 19u), Cyc_do_interface,({ struct _tuple9* _temp170=(
struct _tuple9*) GC_malloc( sizeof( struct _tuple9)); _temp170->f1= _temp169;
_temp170->f2= inter_file; _temp170->f3=( struct Cyc_std___sFILE*) _check_null(
inter_objfile); _temp170;}), tds); if( inter_file !=  0){ Cyc_std_file_close((
struct Cyc_std___sFILE*) _check_null( inter_file));} Cyc_std_file_close(( struct
Cyc_std___sFILE*) _check_null( inter_objfile));} if( Cyc_tc_r){ goto PRINTC;}
tds=(( struct Cyc_List_List*(*)( struct _tagged_arr stage_name, struct Cyc_List_List*(*
f)( int, struct Cyc_List_List*), int env, struct Cyc_List_List* tds)) Cyc_do_stage)(
_tag_arr("translation to C", sizeof( unsigned char), 17u), Cyc_do_translate, 1,
tds); if( Cyc_compile_failure){ return;} if( Cyc_tovc_r){ tds=(( struct Cyc_List_List*(*)(
struct _tagged_arr stage_name, struct Cyc_List_List*(* f)( int, struct Cyc_List_List*),
int env, struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("post-pass to VC",
sizeof( unsigned char), 16u), Cyc_do_tovc, 1, tds);} if( Cyc_compile_failure){
return;} PRINTC: if( tds !=  0){ struct Cyc_std___sFILE* out_file; if( Cyc_parseonly_r?
1: Cyc_tc_r){ if( Cyc_output_file !=  0){ out_file= Cyc_try_file_open(*(( struct
_tagged_arr*) _check_null( Cyc_output_file)), _tag_arr("w", sizeof(
unsigned char), 2u), _tag_arr("output file", sizeof( unsigned char), 12u));}
else{ out_file=( struct Cyc_std___sFILE*) Cyc_std_stdout;}} else{ if( Cyc_toc_r?
Cyc_output_file !=  0: 0){ out_file= Cyc_try_file_open(*(( struct _tagged_arr*)
_check_null( Cyc_output_file)), _tag_arr("w", sizeof( unsigned char), 2u),
_tag_arr("output file", sizeof( unsigned char), 12u));} else{ out_file= Cyc_try_file_open((
struct _tagged_arr) _temp136, _tag_arr("w", sizeof( unsigned char), 2u),
_tag_arr("output file", sizeof( unsigned char), 12u));}} if( Cyc_compile_failure){
return;}{ struct _handler_cons _temp171; _push_handler(& _temp171);{ int
_temp173= 0; if( setjmp( _temp171.handler)){ _temp173= 1;} if( ! _temp173){ tds=((
struct Cyc_List_List*(*)( struct _tagged_arr stage_name, struct Cyc_List_List*(*
f)( struct Cyc_std___sFILE*, struct Cyc_List_List*), struct Cyc_std___sFILE* env,
struct Cyc_List_List* tds)) Cyc_do_stage)( _tag_arr("printing", sizeof(
unsigned char), 9u), Cyc_do_print,( struct Cyc_std___sFILE*) _check_null(
out_file), tds);; _pop_handler();} else{ void* _temp172=( void*) _exn_thrown;
void* _temp175= _temp172; _LL177: goto _LL178; _LL179: goto _LL180; _LL178: Cyc_compile_failure=
1; Cyc_std_file_close(( struct Cyc_std___sFILE*) _check_null( out_file)); Cyc_cfiles=({
struct Cyc_List_List* _temp181=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp181->hd=( void*)({ struct _tagged_arr* _temp182=(
struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp182[ 0]=(
struct _tagged_arr) _temp136; _temp182;}); _temp181->tl= Cyc_cfiles; _temp181;});(
int) _throw( _temp175); _LL180:( void) _throw( _temp175); _LL176:;}}} Cyc_std_file_close((
struct Cyc_std___sFILE*) _check_null( out_file)); Cyc_cfiles=({ struct Cyc_List_List*
_temp183=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp183->hd=( void*)({ struct _tagged_arr* _temp184=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr)); _temp184[ 0]=( struct _tagged_arr)
_temp136; _temp184;}); _temp183->tl= Cyc_cfiles; _temp183;});}}}}}}}}}}} static
unsigned char _temp194[ 8u]="<final>"; static struct _tagged_arr Cyc_final_str={
_temp194, _temp194, _temp194 +  8u}; static struct _tagged_arr* Cyc_final_strptr=&
Cyc_final_str; static struct Cyc_Interface_I* Cyc_read_cycio( struct _tagged_arr*
n){ if( n == ( struct _tagged_arr*) Cyc_final_strptr){ return Cyc_Interface_final();}{
struct _tagged_arr basename;{ struct _handler_cons _temp195; _push_handler(&
_temp195);{ int _temp197= 0; if( setjmp( _temp195.handler)){ _temp197= 1;} if( !
_temp197){ basename=( struct _tagged_arr) Cyc_Filename_chop_extension(* n);;
_pop_handler();} else{ void* _temp196=( void*) _exn_thrown; void* _temp199=
_temp196; _LL201: if(*(( void**) _temp199) ==  Cyc_Core_InvalidArg){ goto _LL202;}
else{ goto _LL203;} _LL203: goto _LL204; _LL202: basename=* n; goto _LL200;
_LL204:( void) _throw( _temp199); _LL200:;}}}{ struct _tagged_arr _temp205= Cyc_std_strconcat(
basename, _tag_arr(".cycio", sizeof( unsigned char), 7u)); struct Cyc_std___sFILE*
_temp206= Cyc_try_file_open(( struct _tagged_arr) _temp205, _tag_arr("r",
sizeof( unsigned char), 2u), _tag_arr("interface object file", sizeof(
unsigned char), 22u)); if( _temp206 ==  0){ Cyc_compile_failure= 1; Cyc_remove_cfiles();
exit( 1);} Cyc_Position_reset_position(( struct _tagged_arr) _temp205);{ struct
Cyc_Interface_I* _temp207= Cyc_Interface_load(( struct Cyc_std___sFILE*)
_check_null( _temp206)); Cyc_std_file_close(( struct Cyc_std___sFILE*)
_check_null( _temp206)); return _temp207;}}}} static int Cyc_is_cfile( struct
_tagged_arr* n){ return*(( const unsigned char*) _check_unknown_subscript(* n,
sizeof( unsigned char), 0)) != '-';} struct _tuple10{ struct _tagged_arr f1; int
f2; struct _tagged_arr f3; void* f4; struct _tagged_arr f5; } ; int Cyc_main(
int argc, struct _tagged_arr argv){ struct Cyc_List_List* options=({ struct
_tuple10* _temp282[ 39u]; _temp282[ 38u]=({ struct _tuple10* _temp397=( struct
_tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp397->f1= _tag_arr("-printfullevars",
sizeof( unsigned char), 16u); _temp397->f2= 0; _temp397->f3= _tag_arr("",
sizeof( unsigned char), 1u); _temp397->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp398=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp398[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp399; _temp399.tag= Cyc_Arg_Set_spec;
_temp399.f1=& Cyc_print_full_evars_r; _temp399;}); _temp398;}); _temp397->f5=
_tag_arr("Print full information for evars (type debugging)", sizeof(
unsigned char), 50u); _temp397;}); _temp282[ 37u]=({ struct _tuple10* _temp394=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp394->f1= _tag_arr("-printallkinds",
sizeof( unsigned char), 15u); _temp394->f2= 0; _temp394->f3= _tag_arr("",
sizeof( unsigned char), 1u); _temp394->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp395=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp395[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp396; _temp396.tag= Cyc_Arg_Set_spec;
_temp396.f1=& Cyc_print_all_kinds_r; _temp396;}); _temp395;}); _temp394->f5=
_tag_arr("Always print kinds of type variables", sizeof( unsigned char), 37u);
_temp394;}); _temp282[ 36u]=({ struct _tuple10* _temp391=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp391->f1= _tag_arr("-printalltvars",
sizeof( unsigned char), 15u); _temp391->f2= 0; _temp391->f3= _tag_arr("",
sizeof( unsigned char), 1u); _temp391->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp392=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp392[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp393; _temp393.tag= Cyc_Arg_Set_spec;
_temp393.f1=& Cyc_print_all_tvars_r; _temp393;}); _temp392;}); _temp391->f5=
_tag_arr("Print all type variables (even implicit default effects)", sizeof(
unsigned char), 57u); _temp391;}); _temp282[ 35u]=({ struct _tuple10* _temp388=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp388->f1= _tag_arr("-noexpandtypedefs",
sizeof( unsigned char), 18u); _temp388->f2= 0; _temp388->f3= _tag_arr("",
sizeof( unsigned char), 1u); _temp388->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp389=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp389[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp390; _temp390.tag= Cyc_Arg_Set_spec;
_temp390.f1=& Cyc_noexpand_r; _temp390;}); _temp389;}); _temp388->f5= _tag_arr("Don't expand typedefs in pretty printing",
sizeof( unsigned char), 41u); _temp388;}); _temp282[ 34u]=({ struct _tuple10*
_temp385=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp385->f1=
_tag_arr("-noremoveunused", sizeof( unsigned char), 16u); _temp385->f2= 0;
_temp385->f3= _tag_arr("", sizeof( unsigned char), 1u); _temp385->f4=( void*)({
struct Cyc_Arg_Set_spec_struct* _temp386=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp386[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp387; _temp387.tag= Cyc_Arg_Set_spec; _temp387.f1=& Cyc_noshake_r; _temp387;});
_temp386;}); _temp385->f5= _tag_arr("Don't remove externed variables that aren't used",
sizeof( unsigned char), 49u); _temp385;}); _temp282[ 33u]=({ struct _tuple10*
_temp382=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp382->f1=
_tag_arr("-nogc", sizeof( unsigned char), 6u); _temp382->f2= 0; _temp382->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp382->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp383=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp383[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp384; _temp384.tag= Cyc_Arg_Set_spec;
_temp384.f1=& Cyc_nogc_r; _temp384;}); _temp383;}); _temp382->f5= _tag_arr("Don't link in the garbage collector",
sizeof( unsigned char), 36u); _temp382;}); _temp282[ 32u]=({ struct _tuple10*
_temp379=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp379->f1=
_tag_arr("-nocyc", sizeof( unsigned char), 7u); _temp379->f2= 0; _temp379->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp379->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp380=( struct Cyc_Arg_Unit_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp380[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp381; _temp381.tag= Cyc_Arg_Unit_spec;
_temp381.f1= Cyc_set_nocyc; _temp381;}); _temp380;}); _temp379->f5= _tag_arr("Don't add implicit namespace Cyc",
sizeof( unsigned char), 33u); _temp379;}); _temp282[ 31u]=({ struct _tuple10*
_temp376=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp376->f1=
_tag_arr("-tc", sizeof( unsigned char), 4u); _temp376->f2= 0; _temp376->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp376->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp377=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp377[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp378; _temp378.tag= Cyc_Arg_Set_spec;
_temp378.f1=& Cyc_tc_r; _temp378;}); _temp377;}); _temp376->f5= _tag_arr("Stop after type checking",
sizeof( unsigned char), 25u); _temp376;}); _temp282[ 30u]=({ struct _tuple10*
_temp373=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp373->f1=
_tag_arr("-save-c", sizeof( unsigned char), 8u); _temp373->f2= 0; _temp373->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp373->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp374=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp374[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp375; _temp375.tag= Cyc_Arg_Set_spec;
_temp375.f1=& Cyc_save_c_r; _temp375;}); _temp374;}); _temp373->f5= _tag_arr("Don't delete temporary C files",
sizeof( unsigned char), 31u); _temp373;}); _temp282[ 29u]=({ struct _tuple10*
_temp370=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp370->f1=
_tag_arr("-use-cpp", sizeof( unsigned char), 9u); _temp370->f2= 0; _temp370->f3=
_tag_arr("<path>", sizeof( unsigned char), 7u); _temp370->f4=( void*)({ struct
Cyc_Arg_String_spec_struct* _temp371=( struct Cyc_Arg_String_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_String_spec_struct)); _temp371[ 0]=({ struct
Cyc_Arg_String_spec_struct _temp372; _temp372.tag= Cyc_Arg_String_spec; _temp372.f1=
Cyc_set_cpp; _temp372;}); _temp371;}); _temp370->f5= _tag_arr("Indicate which preprocessor to use",
sizeof( unsigned char), 35u); _temp370;}); _temp282[ 28u]=({ struct _tuple10*
_temp367=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp367->f1=
_tag_arr("-save-temps", sizeof( unsigned char), 12u); _temp367->f2= 0; _temp367->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp367->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp368=( struct Cyc_Arg_Unit_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp368[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp369; _temp369.tag= Cyc_Arg_Unit_spec;
_temp369.f1= Cyc_set_save_temps; _temp369;}); _temp368;}); _temp367->f5=
_tag_arr("Don't delete temporary files", sizeof( unsigned char), 29u); _temp367;});
_temp282[ 27u]=({ struct _tuple10* _temp364=( struct _tuple10*) GC_malloc(
sizeof( struct _tuple10)); _temp364->f1= _tag_arr("-tovc", sizeof( unsigned char),
6u); _temp364->f2= 0; _temp364->f3= _tag_arr("", sizeof( unsigned char), 1u);
_temp364->f4=( void*)({ struct Cyc_Arg_Set_spec_struct* _temp365=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp365[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp366; _temp366.tag= Cyc_Arg_Set_spec; _temp366.f1=& Cyc_tovc_r; _temp366;});
_temp365;}); _temp364->f5= _tag_arr("Avoid gcc extensions in C output", sizeof(
unsigned char), 33u); _temp364;}); _temp282[ 26u]=({ struct _tuple10* _temp361=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp361->f1= _tag_arr("-up",
sizeof( unsigned char), 4u); _temp361->f2= 0; _temp361->f3= _tag_arr("", sizeof(
unsigned char), 1u); _temp361->f4=( void*)({ struct Cyc_Arg_Clear_spec_struct*
_temp362=( struct Cyc_Arg_Clear_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Clear_spec_struct));
_temp362[ 0]=({ struct Cyc_Arg_Clear_spec_struct _temp363; _temp363.tag= Cyc_Arg_Clear_spec;
_temp363.f1=& Cyc_pp_r; _temp363;}); _temp362;}); _temp361->f5= _tag_arr("Ugly print",
sizeof( unsigned char), 11u); _temp361;}); _temp282[ 25u]=({ struct _tuple10*
_temp358=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp358->f1=
_tag_arr("-pp", sizeof( unsigned char), 4u); _temp358->f2= 0; _temp358->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp358->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp359=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp359[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp360; _temp360.tag= Cyc_Arg_Set_spec;
_temp360.f1=& Cyc_pp_r; _temp360;}); _temp359;}); _temp358->f5= _tag_arr("Pretty print",
sizeof( unsigned char), 13u); _temp358;}); _temp282[ 24u]=({ struct _tuple10*
_temp355=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp355->f1=
_tag_arr("-ic", sizeof( unsigned char), 4u); _temp355->f2= 0; _temp355->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp355->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp356=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp356[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp357; _temp357.tag= Cyc_Arg_Set_spec;
_temp357.f1=& Cyc_ic_r; _temp357;}); _temp356;}); _temp355->f5= _tag_arr("Activate the link-checker",
sizeof( unsigned char), 26u); _temp355;}); _temp282[ 23u]=({ struct _tuple10*
_temp352=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp352->f1=
_tag_arr("-toc", sizeof( unsigned char), 5u); _temp352->f2= 0; _temp352->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp352->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp353=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp353[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp354; _temp354.tag= Cyc_Arg_Set_spec;
_temp354.f1=& Cyc_toc_r; _temp354;}); _temp353;}); _temp352->f5= _tag_arr("Stop after translation to C",
sizeof( unsigned char), 28u); _temp352;}); _temp282[ 22u]=({ struct _tuple10*
_temp349=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp349->f1=
_tag_arr("-tc", sizeof( unsigned char), 4u); _temp349->f2= 0; _temp349->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp349->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp350=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp350[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp351; _temp351.tag= Cyc_Arg_Set_spec;
_temp351.f1=& Cyc_tc_r; _temp351;}); _temp350;}); _temp349->f5= _tag_arr("Stop after type checking",
sizeof( unsigned char), 25u); _temp349;}); _temp282[ 21u]=({ struct _tuple10*
_temp346=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp346->f1=
_tag_arr("-parseonly", sizeof( unsigned char), 11u); _temp346->f2= 0; _temp346->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp346->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp347=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp347[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp348; _temp348.tag= Cyc_Arg_Set_spec;
_temp348.f1=& Cyc_parseonly_r; _temp348;}); _temp347;}); _temp346->f5= _tag_arr("Stop after parsing",
sizeof( unsigned char), 19u); _temp346;}); _temp282[ 20u]=({ struct _tuple10*
_temp343=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp343->f1=
_tag_arr("-E", sizeof( unsigned char), 3u); _temp343->f2= 0; _temp343->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp343->f4=( void*)({ struct Cyc_Arg_Set_spec_struct*
_temp344=( struct Cyc_Arg_Set_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct));
_temp344[ 0]=({ struct Cyc_Arg_Set_spec_struct _temp345; _temp345.tag= Cyc_Arg_Set_spec;
_temp345.f1=& Cyc_stop_after_cpp_r; _temp345;}); _temp344;}); _temp343->f5=
_tag_arr("Stop after preprocessing", sizeof( unsigned char), 25u); _temp343;});
_temp282[ 19u]=({ struct _tuple10* _temp340=( struct _tuple10*) GC_malloc(
sizeof( struct _tuple10)); _temp340->f1= _tag_arr("-MT", sizeof( unsigned char),
4u); _temp340->f2= 0; _temp340->f3= _tag_arr(" <target>", sizeof( unsigned char),
10u); _temp340->f4=( void*)({ struct Cyc_Arg_String_spec_struct* _temp341=(
struct Cyc_Arg_String_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_String_spec_struct));
_temp341[ 0]=({ struct Cyc_Arg_String_spec_struct _temp342; _temp342.tag= Cyc_Arg_String_spec;
_temp342.f1= Cyc_set_dependencies_target; _temp342;}); _temp341;}); _temp340->f5=
_tag_arr("Give target for dependencies", sizeof( unsigned char), 29u); _temp340;});
_temp282[ 18u]=({ struct _tuple10* _temp337=( struct _tuple10*) GC_malloc(
sizeof( struct _tuple10)); _temp337->f1= _tag_arr("-MG", sizeof( unsigned char),
4u); _temp337->f2= 0; _temp337->f3= _tag_arr("", sizeof( unsigned char), 1u);
_temp337->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct* _temp338=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp338[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp339; _temp339.tag= Cyc_Arg_Flag_spec; _temp339.f1= Cyc_add_cpparg; _temp339;});
_temp338;}); _temp337->f5= _tag_arr("When producing dependencies assume that missing files are generated",
sizeof( unsigned char), 68u); _temp337;}); _temp282[ 17u]=({ struct _tuple10*
_temp334=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp334->f1=
_tag_arr("-M", sizeof( unsigned char), 3u); _temp334->f2= 0; _temp334->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp334->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp335=( struct Cyc_Arg_Unit_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp335[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp336; _temp336.tag= Cyc_Arg_Unit_spec;
_temp336.f1= Cyc_set_produce_dependencies; _temp336;}); _temp335;}); _temp334->f5=
_tag_arr("Produce dependencies", sizeof( unsigned char), 21u); _temp334;});
_temp282[ 16u]=({ struct _tuple10* _temp331=( struct _tuple10*) GC_malloc(
sizeof( struct _tuple10)); _temp331->f1= _tag_arr("-S", sizeof( unsigned char),
3u); _temp331->f2= 0; _temp331->f3= _tag_arr("", sizeof( unsigned char), 1u);
_temp331->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct* _temp332=( struct Cyc_Arg_Unit_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct)); _temp332[ 0]=({ struct Cyc_Arg_Unit_spec_struct
_temp333; _temp333.tag= Cyc_Arg_Unit_spec; _temp333.f1= Cyc_set_stop_after_asmfile;
_temp333;}); _temp332;}); _temp331->f5= _tag_arr("Stop after producing assembly code",
sizeof( unsigned char), 35u); _temp331;}); _temp282[ 15u]=({ struct _tuple10*
_temp328=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp328->f1=
_tag_arr("-pa", sizeof( unsigned char), 4u); _temp328->f2= 0; _temp328->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp328->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp329=( struct Cyc_Arg_Unit_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp329[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp330; _temp330.tag= Cyc_Arg_Unit_spec;
_temp330.f1= Cyc_set_pa; _temp330;}); _temp329;}); _temp328->f5= _tag_arr("Compile for aprof",
sizeof( unsigned char), 18u); _temp328;}); _temp282[ 14u]=({ struct _tuple10*
_temp325=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp325->f1=
_tag_arr("-pg", sizeof( unsigned char), 4u); _temp325->f2= 0; _temp325->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp325->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp326=( struct Cyc_Arg_Flag_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp326[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp327; _temp327.tag= Cyc_Arg_Flag_spec;
_temp327.f1= Cyc_add_ccarg; _temp327;}); _temp326;}); _temp325->f5= _tag_arr("Compile for gprof",
sizeof( unsigned char), 18u); _temp325;}); _temp282[ 13u]=({ struct _tuple10*
_temp322=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp322->f1=
_tag_arr("-p", sizeof( unsigned char), 3u); _temp322->f2= 0; _temp322->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp322->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp323=( struct Cyc_Arg_Flag_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp323[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp324; _temp324.tag= Cyc_Arg_Flag_spec;
_temp324.f1= Cyc_add_ccarg; _temp324;}); _temp323;}); _temp322->f5= _tag_arr("Compile for prof",
sizeof( unsigned char), 17u); _temp322;}); _temp282[ 12u]=({ struct _tuple10*
_temp319=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp319->f1=
_tag_arr("-g", sizeof( unsigned char), 3u); _temp319->f2= 0; _temp319->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp319->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp320=( struct Cyc_Arg_Flag_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp320[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp321; _temp321.tag= Cyc_Arg_Flag_spec;
_temp321.f1= Cyc_add_ccarg; _temp321;}); _temp320;}); _temp319->f5= _tag_arr("Compile for debugging",
sizeof( unsigned char), 22u); _temp319;}); _temp282[ 11u]=({ struct _tuple10*
_temp316=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp316->f1=
_tag_arr("-O3", sizeof( unsigned char), 4u); _temp316->f2= 0; _temp316->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp316->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp317=( struct Cyc_Arg_Flag_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp317[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp318; _temp318.tag= Cyc_Arg_Flag_spec;
_temp318.f1= Cyc_add_ccarg; _temp318;}); _temp317;}); _temp316->f5= _tag_arr("Optimize",
sizeof( unsigned char), 9u); _temp316;}); _temp282[ 10u]=({ struct _tuple10*
_temp313=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp313->f1=
_tag_arr("-O2", sizeof( unsigned char), 4u); _temp313->f2= 0; _temp313->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp313->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp314=( struct Cyc_Arg_Flag_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp314[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp315; _temp315.tag= Cyc_Arg_Flag_spec;
_temp315.f1= Cyc_add_ccarg; _temp315;}); _temp314;}); _temp313->f5= _tag_arr("Optimize",
sizeof( unsigned char), 9u); _temp313;}); _temp282[ 9u]=({ struct _tuple10*
_temp310=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp310->f1=
_tag_arr("-O", sizeof( unsigned char), 3u); _temp310->f2= 0; _temp310->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp310->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp311=( struct Cyc_Arg_Flag_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp311[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp312; _temp312.tag= Cyc_Arg_Flag_spec;
_temp312.f1= Cyc_add_ccarg; _temp312;}); _temp311;}); _temp310->f5= _tag_arr("Optimize",
sizeof( unsigned char), 9u); _temp310;}); _temp282[ 8u]=({ struct _tuple10*
_temp307=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp307->f1=
_tag_arr("-s", sizeof( unsigned char), 3u); _temp307->f2= 0; _temp307->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp307->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp308=( struct Cyc_Arg_Flag_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp308[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp309; _temp309.tag= Cyc_Arg_Flag_spec;
_temp309.f1= Cyc_add_ccarg; _temp309;}); _temp308;}); _temp307->f5= _tag_arr("Remove all symbol table and relocation info from executable",
sizeof( unsigned char), 60u); _temp307;}); _temp282[ 7u]=({ struct _tuple10*
_temp304=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp304->f1=
_tag_arr("-c", sizeof( unsigned char), 3u); _temp304->f2= 0; _temp304->f3=
_tag_arr("", sizeof( unsigned char), 1u); _temp304->f4=( void*)({ struct Cyc_Arg_Unit_spec_struct*
_temp305=( struct Cyc_Arg_Unit_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Unit_spec_struct));
_temp305[ 0]=({ struct Cyc_Arg_Unit_spec_struct _temp306; _temp306.tag= Cyc_Arg_Unit_spec;
_temp306.f1= Cyc_set_stop_after_objectfile; _temp306;}); _temp305;}); _temp304->f5=
_tag_arr("Produce object file", sizeof( unsigned char), 20u); _temp304;});
_temp282[ 6u]=({ struct _tuple10* _temp301=( struct _tuple10*) GC_malloc(
sizeof( struct _tuple10)); _temp301->f1= _tag_arr("-l", sizeof( unsigned char),
3u); _temp301->f2= 1; _temp301->f3= _tag_arr("<file>", sizeof( unsigned char), 7u);
_temp301->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct* _temp302=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp302[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp303; _temp303.tag= Cyc_Arg_Flag_spec; _temp303.f1= Cyc_add_libarg; _temp303;});
_temp302;}); _temp301->f5= _tag_arr("Library file", sizeof( unsigned char), 13u);
_temp301;}); _temp282[ 5u]=({ struct _tuple10* _temp298=( struct _tuple10*)
GC_malloc( sizeof( struct _tuple10)); _temp298->f1= _tag_arr("-L", sizeof(
unsigned char), 3u); _temp298->f2= 1; _temp298->f3= _tag_arr("<dir>", sizeof(
unsigned char), 6u); _temp298->f4=( void*)({ struct Cyc_Arg_Flag_spec_struct*
_temp299=( struct Cyc_Arg_Flag_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct));
_temp299[ 0]=({ struct Cyc_Arg_Flag_spec_struct _temp300; _temp300.tag= Cyc_Arg_Flag_spec;
_temp300.f1= Cyc_add_cpparg; _temp300;}); _temp299;}); _temp298->f5= _tag_arr("Add to the list of directories for -l",
sizeof( unsigned char), 38u); _temp298;}); _temp282[ 4u]=({ struct _tuple10*
_temp295=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp295->f1=
_tag_arr("-I", sizeof( unsigned char), 3u); _temp295->f2= 1; _temp295->f3=
_tag_arr("<dir>", sizeof( unsigned char), 6u); _temp295->f4=( void*)({ struct
Cyc_Arg_Flag_spec_struct* _temp296=( struct Cyc_Arg_Flag_spec_struct*) GC_malloc(
sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp296[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp297; _temp297.tag= Cyc_Arg_Flag_spec; _temp297.f1= Cyc_add_cpparg; _temp297;});
_temp296;}); _temp295->f5= _tag_arr("Add to the list of directories to search for include files",
sizeof( unsigned char), 59u); _temp295;}); _temp282[ 3u]=({ struct _tuple10*
_temp292=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp292->f1=
_tag_arr("-B", sizeof( unsigned char), 3u); _temp292->f2= 1; _temp292->f3=
_tag_arr("<file>", sizeof( unsigned char), 7u); _temp292->f4=( void*)({ struct
Cyc_Arg_Flag_spec_struct* _temp293=( struct Cyc_Arg_Flag_spec_struct*) GC_malloc(
sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp293[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp294; _temp294.tag= Cyc_Arg_Flag_spec; _temp294.f1= Cyc_add_cyclone_lib_path;
_temp294;}); _temp293;}); _temp292->f5= _tag_arr("Add to the list of directories to search for compiler files",
sizeof( unsigned char), 60u); _temp292;}); _temp282[ 2u]=({ struct _tuple10*
_temp289=( struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp289->f1=
_tag_arr("-D", sizeof( unsigned char), 3u); _temp289->f2= 1; _temp289->f3=
_tag_arr("<name>[=<value>]", sizeof( unsigned char), 17u); _temp289->f4=( void*)({
struct Cyc_Arg_Flag_spec_struct* _temp290=( struct Cyc_Arg_Flag_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Flag_spec_struct)); _temp290[ 0]=({ struct Cyc_Arg_Flag_spec_struct
_temp291; _temp291.tag= Cyc_Arg_Flag_spec; _temp291.f1= Cyc_add_cpparg; _temp291;});
_temp290;}); _temp289->f5= _tag_arr("Pass definition to preprocessor", sizeof(
unsigned char), 32u); _temp289;}); _temp282[ 1u]=({ struct _tuple10* _temp286=(
struct _tuple10*) GC_malloc( sizeof( struct _tuple10)); _temp286->f1= _tag_arr("-o",
sizeof( unsigned char), 3u); _temp286->f2= 0; _temp286->f3= _tag_arr(" <file>",
sizeof( unsigned char), 8u); _temp286->f4=( void*)({ struct Cyc_Arg_String_spec_struct*
_temp287=( struct Cyc_Arg_String_spec_struct*) GC_malloc( sizeof( struct Cyc_Arg_String_spec_struct));
_temp287[ 0]=({ struct Cyc_Arg_String_spec_struct _temp288; _temp288.tag= Cyc_Arg_String_spec;
_temp288.f1= Cyc_set_output_file; _temp288;}); _temp287;}); _temp286->f5=
_tag_arr("Set output file name", sizeof( unsigned char), 21u); _temp286;});
_temp282[ 0u]=({ struct _tuple10* _temp283=( struct _tuple10*) GC_malloc(
sizeof( struct _tuple10)); _temp283->f1= _tag_arr("-v", sizeof( unsigned char),
3u); _temp283->f2= 0; _temp283->f3= _tag_arr("", sizeof( unsigned char), 1u);
_temp283->f4=( void*)({ struct Cyc_Arg_Set_spec_struct* _temp284=( struct Cyc_Arg_Set_spec_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Set_spec_struct)); _temp284[ 0]=({ struct Cyc_Arg_Set_spec_struct
_temp285; _temp285.tag= Cyc_Arg_Set_spec; _temp285.f1=& Cyc_v_r; _temp285;});
_temp284;}); _temp283->f5= _tag_arr("Print compilation stages verbosely",
sizeof( unsigned char), 35u); _temp283;});(( struct Cyc_List_List*(*)( struct
_tagged_arr)) Cyc_List_list)( _tag_arr( _temp282, sizeof( struct _tuple10*), 39u));});
Cyc_Arg_parse( options, Cyc_add_other, _tag_arr("Options:", sizeof(
unsigned char), 9u), argv);{ struct Cyc_List_List* _temp208=(( struct Cyc_List_List*(*)(
struct Cyc_List_List* x)) Cyc_List_rev)( Cyc_cyclone_files); for( 0; _temp208 != 
0; _temp208=(( struct Cyc_List_List*) _check_null( _temp208))->tl){ Cyc_process_file(*((
struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( _temp208))->hd)); if(
Cyc_compile_failure){ return 1;}}} if((( Cyc_stop_after_cpp_r? 1: Cyc_parseonly_r)?
1: Cyc_tc_r)? 1: Cyc_toc_r){ return 0;} if( Cyc_ccargs ==  0){ return 0;}{
struct _tagged_arr cyclone_exec_prefix=( struct _tagged_arr) Cyc_std_getenv(
_tag_arr("CYCLONE_EXEC_PREFIX", sizeof( unsigned char), 20u)); if(
cyclone_exec_prefix.curr != (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){
Cyc_cyclone_lib_path=({ struct Cyc_List_List* _temp209=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp209->hd=( void*)({ struct
_tagged_arr* _temp210=( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)); _temp210[ 0]= cyclone_exec_prefix; _temp210;}); _temp209->tl= Cyc_cyclone_lib_path;
_temp209;});}{ struct _tagged_arr def_lib_path=( struct _tagged_arr)
Cstring_to_string( Cdef_lib_path); if( Cyc_std_strlen( def_lib_path) >  0){ Cyc_cyclone_lib_path=({
struct Cyc_List_List* _temp211=( struct Cyc_List_List*) GC_malloc( sizeof(
struct Cyc_List_List)); _temp211->hd=( void*)({ struct _tagged_arr* _temp212=(
struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp212[ 0]=
def_lib_path; _temp212;}); _temp211->tl= Cyc_cyclone_lib_path; _temp211;});} Cyc_cyclone_lib_path=((
struct Cyc_List_List*(*)( struct Cyc_List_List* x)) Cyc_List_imp_rev)( Cyc_cyclone_lib_path);{
struct _tagged_arr _temp213= Cyc_do_find( Cyc_cyclone_lib_path, _tag_arr("include/cyc_include.h",
sizeof( unsigned char), 22u)); Cyc_ccargs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( Cyc_ccargs);{ struct _tagged_arr _temp214= Cyc_std_str_sepstr(((
struct Cyc_List_List*(*)( struct _tagged_arr*(* f)( struct _tagged_arr*), struct
Cyc_List_List* x)) Cyc_List_map)( Cyc_sh_escape_stringptr, Cyc_ccargs), _tag_arr(" ",
sizeof( unsigned char), 2u)); Cyc_libargs=(( struct Cyc_List_List*(*)( struct
Cyc_List_List* x)) Cyc_List_rev)( Cyc_libargs);{ struct _tagged_arr _temp215=
Cyc_std_str_sepstr(({ struct Cyc_List_List* _temp281=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp281->hd=( void*) _init_tag_arr((
struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)),"", sizeof(
unsigned char), 1u); _temp281->tl=(( struct Cyc_List_List*(*)( struct
_tagged_arr*(* f)( struct _tagged_arr*), struct Cyc_List_List* x)) Cyc_List_map)(
Cyc_sh_escape_stringptr, Cyc_libargs); _temp281;}), _tag_arr(" ", sizeof(
unsigned char), 2u)); struct Cyc_List_List* stdlib; struct _tagged_arr
stdlib_string; int _temp216=(( Cyc_stop_after_asmfile_r? 1: Cyc_stop_after_objectfile_r)?
1:( Cyc_output_file !=  0? Cyc_Filename_check_suffix(*(( struct _tagged_arr*)
_check_null( Cyc_output_file)), _tag_arr(".a", sizeof( unsigned char), 3u)): 0))?
1:( Cyc_output_file !=  0? Cyc_Filename_check_suffix(*(( struct _tagged_arr*)
_check_null( Cyc_output_file)), _tag_arr(".lib", sizeof( unsigned char), 5u)): 0);
if( _temp216){ stdlib= 0; stdlib_string= _tag_arr("", sizeof( unsigned char), 1u);}
else{ struct _tagged_arr libcyc_filename= Cyc_pa_r? _tag_arr("libcyc_a.a",
sizeof( unsigned char), 11u): _tag_arr("libcyc.a", sizeof( unsigned char), 9u);
struct _tagged_arr nogc_filename= Cyc_pa_r? _tag_arr("nogc_a.a", sizeof(
unsigned char), 9u): _tag_arr("nogc.a", sizeof( unsigned char), 7u); struct
_tagged_arr gc_filename= _tag_arr("gc.a", sizeof( unsigned char), 5u); struct
_tagged_arr _temp217= Cyc_do_find( Cyc_cyclone_lib_path, libcyc_filename);
struct _tagged_arr _temp218= Cyc_nogc_r? Cyc_do_find( Cyc_cyclone_lib_path,
nogc_filename): Cyc_do_find( Cyc_cyclone_lib_path, gc_filename); stdlib=({
struct _tagged_arr* _temp219[ 1u]; _temp219[ 0u]=({ struct _tagged_arr* _temp220=(
struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)); _temp220[ 0]=
_temp217; _temp220;});(( struct Cyc_List_List*(*)( struct _tagged_arr)) Cyc_List_list)(
_tag_arr( _temp219, sizeof( struct _tagged_arr*), 1u));}); stdlib_string=(
struct _tagged_arr)({ struct Cyc_std_String_pa_struct _temp223; _temp223.tag=
Cyc_std_String_pa; _temp223.f1=( struct _tagged_arr) _temp218;{ struct Cyc_std_String_pa_struct
_temp222; _temp222.tag= Cyc_std_String_pa; _temp222.f1=( struct _tagged_arr)
_temp217;{ void* _temp221[ 2u]={& _temp222,& _temp223}; Cyc_std_aprintf(
_tag_arr(" %s %s", sizeof( unsigned char), 7u), _tag_arr( _temp221, sizeof( void*),
2u));}}});} if( Cyc_ic_r){ struct _handler_cons _temp224; _push_handler(&
_temp224);{ int _temp226= 0; if( setjmp( _temp224.handler)){ _temp226= 1;} if( !
_temp226){ Cyc_ccargs=(( struct Cyc_List_List*(*)( int(* f)( struct _tagged_arr*),
struct Cyc_List_List* l)) Cyc_List_filter)( Cyc_is_cfile, Cyc_ccargs); Cyc_libargs=((
struct Cyc_List_List*(*)( int(* f)( struct _tagged_arr*), struct Cyc_List_List*
l)) Cyc_List_filter)( Cyc_is_cfile, Cyc_libargs);{ struct Cyc_List_List*
_temp227=(( struct Cyc_List_List*(*)( struct Cyc_List_List* x, struct Cyc_List_List*
y)) Cyc_List_append)( stdlib,(( struct Cyc_List_List*(*)( struct Cyc_List_List*
x, struct Cyc_List_List* y)) Cyc_List_append)( Cyc_ccargs, Cyc_libargs)); if( !
_temp216){ _temp227=({ struct Cyc_List_List* _temp228=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp228->hd=( void*) Cyc_final_strptr;
_temp228->tl= _temp227; _temp228;});}{ struct Cyc_Interface_I* _temp229=((
struct Cyc_Interface_I*(*)( struct Cyc_Interface_I*(* get)( struct _tagged_arr*),
struct Cyc_List_List* la, struct Cyc_List_List* linfo)) Cyc_Interface_get_and_merge_list)(
Cyc_read_cycio, _temp227, _temp227); if( _temp229 ==  0){({ void* _temp230[ 0u]={};
Cyc_std_fprintf( Cyc_std_stderr, _tag_arr("Error: interfaces incompatible\n",
sizeof( unsigned char), 32u), _tag_arr( _temp230, sizeof( void*), 0u));}); Cyc_compile_failure=
1; Cyc_remove_cfiles();{ int _temp231= 1; _npop_handler( 0u); return _temp231;}}
if( _temp216){ if( Cyc_output_file !=  0){ struct _tagged_arr _temp232=({ struct
Cyc_std_String_pa_struct _temp236; _temp236.tag= Cyc_std_String_pa; _temp236.f1=(
struct _tagged_arr) Cyc_Filename_chop_extension(*(( struct _tagged_arr*)
_check_null( Cyc_output_file)));{ void* _temp235[ 1u]={& _temp236}; Cyc_std_aprintf(
_tag_arr("%s.cycio", sizeof( unsigned char), 9u), _tag_arr( _temp235, sizeof(
void*), 1u));}}); struct Cyc_std___sFILE* _temp233= Cyc_try_file_open(( struct
_tagged_arr) _temp232, _tag_arr("w", sizeof( unsigned char), 2u), _tag_arr("interface object file",
sizeof( unsigned char), 22u)); if( _temp233 ==  0){ Cyc_compile_failure= 1; Cyc_remove_cfiles();{
int _temp234= 1; _npop_handler( 0u); return _temp234;}} Cyc_Interface_save((
struct Cyc_Interface_I*) _check_null( _temp229),( struct Cyc_std___sFILE*)
_check_null( _temp233)); Cyc_std_file_close(( struct Cyc_std___sFILE*)
_check_null( _temp233));}} else{ if( ! Cyc_Interface_is_subinterface( Cyc_Interface_empty(),(
struct Cyc_Interface_I*) _check_null( _temp229),({ struct _tuple8* _temp237=(
struct _tuple8*) GC_malloc( sizeof( struct _tuple8)); _temp237->f1= _tag_arr("empty interface",
sizeof( unsigned char), 16u); _temp237->f2= _tag_arr("global interface", sizeof(
unsigned char), 17u); _temp237;}))){({ void* _temp238[ 0u]={}; Cyc_std_fprintf(
Cyc_std_stderr, _tag_arr("Error: some objects are still undefined\n", sizeof(
unsigned char), 41u), _tag_arr( _temp238, sizeof( void*), 0u));}); Cyc_compile_failure=
1; Cyc_remove_cfiles();{ int _temp239= 1; _npop_handler( 0u); return _temp239;}}}}};
_pop_handler();} else{ void* _temp225=( void*) _exn_thrown; void* _temp241=
_temp225; struct _tagged_arr _temp255; struct _tagged_arr _temp257; struct
_tagged_arr _temp259; _LL243: if(*(( void**) _temp241) ==  Cyc_Core_Failure){
_LL256: _temp255=(( struct Cyc_Core_Failure_struct*) _temp241)->f1; goto _LL244;}
else{ goto _LL245;} _LL245: if(*(( void**) _temp241) ==  Cyc_Core_Impossible){
_LL258: _temp257=(( struct Cyc_Core_Impossible_struct*) _temp241)->f1; goto
_LL246;} else{ goto _LL247;} _LL247: if( _temp241 ==  Cyc_Dict_Absent){ goto
_LL248;} else{ goto _LL249;} _LL249: if(*(( void**) _temp241) ==  Cyc_Core_InvalidArg){
_LL260: _temp259=(( struct Cyc_Core_InvalidArg_struct*) _temp241)->f1; goto
_LL250;} else{ goto _LL251;} _LL251: goto _LL252; _LL253: goto _LL254; _LL244:({
struct Cyc_std_String_pa_struct _temp262; _temp262.tag= Cyc_std_String_pa;
_temp262.f1=( struct _tagged_arr) _temp255;{ void* _temp261[ 1u]={& _temp262};
Cyc_std_printf( _tag_arr("Exception Core::Failure %s\n", sizeof( unsigned char),
28u), _tag_arr( _temp261, sizeof( void*), 1u));}}); Cyc_compile_failure= 1; Cyc_remove_cfiles();
return 1; _LL246:({ struct Cyc_std_String_pa_struct _temp264; _temp264.tag= Cyc_std_String_pa;
_temp264.f1=( struct _tagged_arr) _temp257;{ void* _temp263[ 1u]={& _temp264};
Cyc_std_printf( _tag_arr("Exception Core::Impossible %s\n", sizeof(
unsigned char), 31u), _tag_arr( _temp263, sizeof( void*), 1u));}}); Cyc_compile_failure=
1; Cyc_remove_cfiles(); return 1; _LL248:({ void* _temp265[ 0u]={}; Cyc_std_printf(
_tag_arr("Exception Dict::Absent\n", sizeof( unsigned char), 24u), _tag_arr(
_temp265, sizeof( void*), 0u));}); Cyc_compile_failure= 1; Cyc_remove_cfiles();
return 1; _LL250:({ struct Cyc_std_String_pa_struct _temp267; _temp267.tag= Cyc_std_String_pa;
_temp267.f1=( struct _tagged_arr) _temp259;{ void* _temp266[ 1u]={& _temp267};
Cyc_std_printf( _tag_arr("Exception Core::InvalidArg %s\n", sizeof(
unsigned char), 31u), _tag_arr( _temp266, sizeof( void*), 1u));}}); Cyc_compile_failure=
1; Cyc_remove_cfiles(); return 1; _LL252:({ void* _temp268[ 0u]={}; Cyc_std_printf(
_tag_arr("Uncaught exception\n", sizeof( unsigned char), 20u), _tag_arr(
_temp268, sizeof( void*), 0u));}); Cyc_compile_failure= 1; Cyc_remove_cfiles();
return 1; _LL254:( void) _throw( _temp241); _LL242:;}}}{ struct _tagged_arr
_temp269=({ struct Cyc_std_String_pa_struct _temp278; _temp278.tag= Cyc_std_String_pa;
_temp278.f1=( struct _tagged_arr) _temp215;{ struct Cyc_std_String_pa_struct
_temp277; _temp277.tag= Cyc_std_String_pa; _temp277.f1=( struct _tagged_arr)
stdlib_string;{ struct Cyc_std_String_pa_struct _temp276; _temp276.tag= Cyc_std_String_pa;
_temp276.f1=( struct _tagged_arr) _temp214;{ struct Cyc_std_String_pa_struct
_temp275; _temp275.tag= Cyc_std_String_pa; _temp275.f1=( struct _tagged_arr)(
Cyc_output_file ==  0? _tag_arr("", sizeof( unsigned char), 1u):( struct
_tagged_arr)({ struct Cyc_std_String_pa_struct _temp280; _temp280.tag= Cyc_std_String_pa;
_temp280.f1=( struct _tagged_arr) Cyc_sh_escape_string(*(( struct _tagged_arr*)
_check_null( Cyc_output_file)));{ void* _temp279[ 1u]={& _temp280}; Cyc_std_aprintf(
_tag_arr(" -o %s", sizeof( unsigned char), 7u), _tag_arr( _temp279, sizeof( void*),
1u));}}));{ struct Cyc_std_String_pa_struct _temp274; _temp274.tag= Cyc_std_String_pa;
_temp274.f1=( struct _tagged_arr) _temp213;{ void* _temp273[ 5u]={& _temp274,&
_temp275,& _temp276,& _temp277,& _temp278}; Cyc_std_aprintf( _tag_arr("gcc -include %s%s %s%s%s",
sizeof( unsigned char), 25u), _tag_arr( _temp273, sizeof( void*), 5u));}}}}}});
if( Cyc_v_r){({ struct Cyc_std_String_pa_struct _temp271; _temp271.tag= Cyc_std_String_pa;
_temp271.f1=( struct _tagged_arr) _temp269;{ void* _temp270[ 1u]={& _temp271};
Cyc_std_fprintf( Cyc_std_stderr, _tag_arr("%s\n", sizeof( unsigned char), 4u),
_tag_arr( _temp270, sizeof( void*), 1u));}});} if( system( string_to_Cstring((
struct _tagged_arr) _temp269)) !=  0){({ void* _temp272[ 0u]={}; Cyc_std_fprintf(
Cyc_std_stderr, _tag_arr("Error: C compiler failed\n", sizeof( unsigned char),
26u), _tag_arr( _temp272, sizeof( void*), 0u));}); Cyc_compile_failure= 1; Cyc_remove_cfiles();
return 1;} Cyc_remove_cfiles(); return Cyc_compile_failure? 1: 0;}}}}}}}