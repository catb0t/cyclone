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
tag; struct _tagged_string f1; } ; extern int Cyc_Core_int_of_string( struct
_tagged_string); extern char* string_to_Cstring( struct _tagged_string); extern
char* underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( char*); extern int system( char*); struct Cyc_Stdio___sFILE;
typedef struct Cyc_Stdio___sFILE Cyc_Stdio_FILE; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stderr; typedef int Cyc_Stdio_fpos_t; extern char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ char* tag; struct _tagged_string f1;
} ; extern char Cyc_Stdio_FileCloseError[ 19u]; struct Cyc_List_List{ void* hd;
struct Cyc_List_List* tl; } ; typedef struct Cyc_List_List* Cyc_List_glist_t;
typedef struct Cyc_List_List* Cyc_List_list_t; typedef struct Cyc_List_List* Cyc_List_List_t;
extern char Cyc_List_List_empty[ 15u]; extern char Cyc_List_List_mismatch[ 18u];
extern char Cyc_List_Nth[ 8u]; extern int Cyc_String_strcmp( struct
_tagged_string s1, struct _tagged_string s2); extern char Cyc_Arg_Error[ 10u];
extern char Cyc_Arg_Bad[ 8u]; struct Cyc_Arg_Bad_struct{ char* tag; struct
_tagged_string f1; } ; static const int Cyc_Arg_Unit_spec= 0; struct Cyc_Arg_Unit_spec_struct{
int tag; void(* f1)(); } ; static const int Cyc_Arg_Set_spec= 1; struct Cyc_Arg_Set_spec_struct{
int tag; int* f1; } ; static const int Cyc_Arg_Clear_spec= 2; struct Cyc_Arg_Clear_spec_struct{
int tag; int* f1; } ; static const int Cyc_Arg_String_spec= 3; struct Cyc_Arg_String_spec_struct{
int tag; void(* f1)( struct _tagged_string); } ; static const int Cyc_Arg_Int_spec=
4; struct Cyc_Arg_Int_spec_struct{ int tag; void(* f1)( int); } ; static const
int Cyc_Arg_Rest_spec= 5; struct Cyc_Arg_Rest_spec_struct{ int tag; void(* f1)(
struct _tagged_string); } ; typedef void* Cyc_Arg_gspec_t; typedef void* Cyc_Arg_spec_t;
extern void Cyc_Arg_usage( struct Cyc_List_List*, struct _tagged_string); extern
int Cyc_Arg_current; struct _tagged_ptr0{ struct _tagged_string* curr; struct
_tagged_string* base; struct _tagged_string* last_plus_one; } ; extern void Cyc_Arg_parse(
struct Cyc_List_List* specs, void(* anonfun)( struct _tagged_string), struct
_tagged_string errmsg, struct _tagged_ptr0 args); char Cyc_Arg_Error[ 10u]; char
Cyc_Arg_Bad[ 8u]; static const int Cyc_Arg_Unknown= 0; struct Cyc_Arg_Unknown_struct{
int tag; struct _tagged_string f1; } ; static const int Cyc_Arg_Missing= 1;
struct Cyc_Arg_Missing_struct{ int tag; struct _tagged_string f1; } ; static
const int Cyc_Arg_Message= 2; struct Cyc_Arg_Message_struct{ int tag; struct
_tagged_string f1; } ; static const int Cyc_Arg_Wrong= 3; struct Cyc_Arg_Wrong_struct{
int tag; struct _tagged_string f1; struct _tagged_string f2; struct
_tagged_string f3; } ; struct _tuple0{ struct _tagged_string f1; void* f2;
struct _tagged_string f3; } ; static void* Cyc_Arg_lookup( struct Cyc_List_List*
l, struct _tagged_string x){ while( l != 0) { if( Cyc_String_strcmp( x,(*((
struct _tuple0*)({ struct Cyc_List_List* _temp0= l; if( _temp0 == 0){ _throw(
Null_Exception);} _temp0->hd;}))).f1) == 0){ return(*(( struct _tuple0*)({
struct Cyc_List_List* _temp1= l; if( _temp1 == 0){ _throw( Null_Exception);}
_temp1->hd;}))).f2;} l=({ struct Cyc_List_List* _temp2= l; if( _temp2 == 0){
_throw( Null_Exception);} _temp2->tl;});}( void) _throw(( void*) Cyc_Core_Not_found);}
void Cyc_Arg_usage( struct Cyc_List_List* speclist, struct _tagged_string errmsg){({
struct _tagged_string _temp3= errmsg; fprintf( Cyc_Stdio_stderr,"%.*s\n", _temp3.last_plus_one
- _temp3.curr, _temp3.curr);}); while( speclist != 0) {({ struct _tagged_string
_temp6=(*(( struct _tuple0*)({ struct Cyc_List_List* _temp4= speclist; if(
_temp4 == 0){ _throw( Null_Exception);} _temp4->hd;}))).f1; struct
_tagged_string _temp7=(*(( struct _tuple0*)({ struct Cyc_List_List* _temp5=
speclist; if( _temp5 == 0){ _throw( Null_Exception);} _temp5->hd;}))).f3;
fprintf( Cyc_Stdio_stderr," %.*s %.*s\n", _temp6.last_plus_one - _temp6.curr,
_temp6.curr, _temp7.last_plus_one - _temp7.curr, _temp7.curr);}); speclist=({
struct Cyc_List_List* _temp8= speclist; if( _temp8 == 0){ _throw( Null_Exception);}
_temp8->tl;});}} int Cyc_Arg_current= 0; static struct _tagged_ptr0 Cyc_Arg_args={
0, 0, 0}; static void Cyc_Arg_stop( int prog_pos, void* e, struct Cyc_List_List*
speclist, struct _tagged_string errmsg){ struct _tagged_string progname=
prog_pos <({ struct _tagged_ptr0 _temp44= Cyc_Arg_args;( unsigned int)( _temp44.last_plus_one
- _temp44.curr);})?({ struct _tagged_ptr0 _temp45= Cyc_Arg_args; struct
_tagged_string* _temp47= _temp45.curr + prog_pos; if( _temp45.base == 0? 1:(
_temp47 < _temp45.base? 1: _temp47 >= _temp45.last_plus_one)){ _throw(
Null_Exception);}* _temp47;}):( struct _tagged_string)({ char* _temp48=( char*)"(?)";
struct _tagged_string _temp49; _temp49.curr= _temp48; _temp49.base= _temp48;
_temp49.last_plus_one= _temp48 + 4; _temp49;});{ void* _temp9= e; struct
_tagged_string _temp19; struct _tagged_string _temp21; struct _tagged_string
_temp23; struct _tagged_string _temp25; struct _tagged_string _temp27; struct
_tagged_string _temp29; _LL11: if(*(( int*) _temp9) == Cyc_Arg_Unknown){ _LL20:
_temp19=( struct _tagged_string)(( struct Cyc_Arg_Unknown_struct*) _temp9)->f1;
goto _LL12;} else{ goto _LL13;} _LL13: if(*(( int*) _temp9) == Cyc_Arg_Missing){
_LL22: _temp21=( struct _tagged_string)(( struct Cyc_Arg_Missing_struct*) _temp9)->f1;
goto _LL14;} else{ goto _LL15;} _LL15: if(*(( int*) _temp9) == Cyc_Arg_Wrong){
_LL28: _temp27=( struct _tagged_string)(( struct Cyc_Arg_Wrong_struct*) _temp9)->f1;
goto _LL26; _LL26: _temp25=( struct _tagged_string)(( struct Cyc_Arg_Wrong_struct*)
_temp9)->f2; goto _LL24; _LL24: _temp23=( struct _tagged_string)(( struct Cyc_Arg_Wrong_struct*)
_temp9)->f3; goto _LL16;} else{ goto _LL17;} _LL17: if(*(( int*) _temp9) == Cyc_Arg_Message){
_LL30: _temp29=( struct _tagged_string)(( struct Cyc_Arg_Message_struct*) _temp9)->f1;
goto _LL18;} else{ goto _LL10;} _LL12: if( Cyc_String_strcmp( _temp19,( struct
_tagged_string)({ char* _temp31=( char*)"-help"; struct _tagged_string _temp32;
_temp32.curr= _temp31; _temp32.base= _temp31; _temp32.last_plus_one= _temp31 + 6;
_temp32;})) != 0){({ struct _tagged_string _temp33= progname; struct
_tagged_string _temp34= _temp19; fprintf( Cyc_Stdio_stderr,"%.*s: unknown option `%.*s'.\n",
_temp33.last_plus_one - _temp33.curr, _temp33.curr, _temp34.last_plus_one -
_temp34.curr, _temp34.curr);});} goto _LL10; _LL14:({ struct _tagged_string
_temp35= progname; struct _tagged_string _temp36= _temp21; fprintf( Cyc_Stdio_stderr,"%.*s: option `%.*s' needs an argument.\n",
_temp35.last_plus_one - _temp35.curr, _temp35.curr, _temp36.last_plus_one -
_temp36.curr, _temp36.curr);}); goto _LL10; _LL16:({ struct _tagged_string
_temp37= progname; struct _tagged_string _temp38= _temp25; struct _tagged_string
_temp39= _temp27; struct _tagged_string _temp40= _temp23; fprintf( Cyc_Stdio_stderr,"%.*s: wrong argument `%.*s'; option `%.*s' expects %.*s.\n",
_temp37.last_plus_one - _temp37.curr, _temp37.curr, _temp38.last_plus_one -
_temp38.curr, _temp38.curr, _temp39.last_plus_one - _temp39.curr, _temp39.curr,
_temp40.last_plus_one - _temp40.curr, _temp40.curr);}); goto _LL10; _LL18:({
struct _tagged_string _temp41= progname; struct _tagged_string _temp42= _temp29;
fprintf( Cyc_Stdio_stderr,"%.*s: %.*s.\n", _temp41.last_plus_one - _temp41.curr,
_temp41.curr, _temp42.last_plus_one - _temp42.curr, _temp42.curr);}); goto _LL10;
_LL10:;} Cyc_Arg_usage( speclist, errmsg); Cyc_Arg_current=( int)({ struct
_tagged_ptr0 _temp43= Cyc_Arg_args;( unsigned int)( _temp43.last_plus_one -
_temp43.curr);});} void Cyc_Arg_parse( struct Cyc_List_List* speclist, void(*
anonfun)( struct _tagged_string), struct _tagged_string errmsg, struct
_tagged_ptr0 orig_args){ Cyc_Arg_args= orig_args;{ int initpos= Cyc_Arg_current;
int l=( int)({ struct _tagged_ptr0 _temp146= Cyc_Arg_args;( unsigned int)(
_temp146.last_plus_one - _temp146.curr);}); ++ Cyc_Arg_current; while( Cyc_Arg_current
< l) { struct _tagged_string s=({ struct _tagged_ptr0 _temp143= Cyc_Arg_args;
struct _tagged_string* _temp145= _temp143.curr + Cyc_Arg_current; if( _temp143.base
== 0? 1:( _temp145 < _temp143.base? 1: _temp145 >= _temp143.last_plus_one)){
_throw( Null_Exception);}* _temp145;}); if(({ struct _tagged_string _temp50= s;(
unsigned int)( _temp50.last_plus_one - _temp50.curr);}) >= 1?({ struct
_tagged_string _temp51= s; char* _temp53= _temp51.curr + 0; if( _temp51.base ==
0? 1:( _temp53 < _temp51.base? 1: _temp53 >= _temp51.last_plus_one)){ _throw(
Null_Exception);}* _temp53;}) =='-': 0){ void* action;{ struct _handler_cons
_temp54; _push_handler(& _temp54);{ void* _temp55=( void*) setjmp( _temp54.handler);
if( ! _temp55){ action= Cyc_Arg_lookup( speclist, s);; _pop_handler();} else{
void* _temp57= _temp55; _LL59: if( _temp57 == Cyc_Core_Not_found){ goto _LL60;}
else{ goto _LL61;} _LL61: goto _LL62; _LL60: Cyc_Arg_stop( initpos,( void*)({
struct Cyc_Arg_Unknown_struct* _temp63=( struct Cyc_Arg_Unknown_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Unknown_struct)); _temp63[ 0]=({ struct Cyc_Arg_Unknown_struct
_temp64; _temp64.tag= Cyc_Arg_Unknown; _temp64.f1= s; _temp64;}); _temp63;}),
speclist, errmsg); return; _LL62:( void) _throw( _temp57); _LL58:;}}}{ struct
_handler_cons _temp65; _push_handler(& _temp65);{ void* _temp66=( void*) setjmp(
_temp65.handler); if( ! _temp66){{ void* _temp67= action; void(* _temp81)(); int*
_temp83; int* _temp85; void(* _temp87)( struct _tagged_string); void(* _temp89)(
int); void(* _temp91)( struct _tagged_string); _LL69: if(*(( int*) _temp67) ==
Cyc_Arg_Unit_spec){ _LL82: _temp81=( void(*)())(( struct Cyc_Arg_Unit_spec_struct*)
_temp67)->f1; goto _LL70;} else{ goto _LL71;} _LL71: if(*(( int*) _temp67) ==
Cyc_Arg_Set_spec){ _LL84: _temp83=( int*)(( struct Cyc_Arg_Set_spec_struct*)
_temp67)->f1; goto _LL72;} else{ goto _LL73;} _LL73: if(*(( int*) _temp67) ==
Cyc_Arg_Clear_spec){ _LL86: _temp85=( int*)(( struct Cyc_Arg_Clear_spec_struct*)
_temp67)->f1; goto _LL74;} else{ goto _LL75;} _LL75: if(*(( int*) _temp67) ==
Cyc_Arg_String_spec){ _LL88: _temp87=( void(*)( struct _tagged_string))(( struct
Cyc_Arg_String_spec_struct*) _temp67)->f1; goto _LL76;} else{ goto _LL77;} _LL77:
if(*(( int*) _temp67) == Cyc_Arg_Int_spec){ _LL90: _temp89=( void(*)( int))((
struct Cyc_Arg_Int_spec_struct*) _temp67)->f1; goto _LL78;} else{ goto _LL79;}
_LL79: if(*(( int*) _temp67) == Cyc_Arg_Rest_spec){ _LL92: _temp91=( void(*)(
struct _tagged_string))(( struct Cyc_Arg_Rest_spec_struct*) _temp67)->f1; goto
_LL80;} else{ goto _LL68;} _LL70: _temp81(); goto _LL68; _LL72:* _temp83= 1;
goto _LL68; _LL74:* _temp85= 0; goto _LL68; _LL76: if( Cyc_Arg_current + 1 < l){
_temp87(({ struct _tagged_ptr0 _temp93= Cyc_Arg_args; struct _tagged_string*
_temp95= _temp93.curr +( Cyc_Arg_current + 1); if( _temp93.base == 0? 1:(
_temp95 < _temp93.base? 1: _temp95 >= _temp93.last_plus_one)){ _throw(
Null_Exception);}* _temp95;})); ++ Cyc_Arg_current;} else{ Cyc_Arg_stop( initpos,(
void*)({ struct Cyc_Arg_Missing_struct* _temp96=( struct Cyc_Arg_Missing_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Missing_struct)); _temp96[ 0]=({ struct Cyc_Arg_Missing_struct
_temp97; _temp97.tag= Cyc_Arg_Missing; _temp97.f1= s; _temp97;}); _temp96;}),
speclist, errmsg);} goto _LL68; _LL78: { struct _tagged_string arg=({ struct
_tagged_ptr0 _temp113= Cyc_Arg_args; struct _tagged_string* _temp115= _temp113.curr
+( Cyc_Arg_current + 1); if( _temp113.base == 0? 1:( _temp115 < _temp113.base? 1:
_temp115 >= _temp113.last_plus_one)){ _throw( Null_Exception);}* _temp115;});
int n;{ struct _handler_cons _temp98; _push_handler(& _temp98);{ void* _temp99=(
void*) setjmp( _temp98.handler); if( ! _temp99){ n= Cyc_Core_int_of_string( arg);;
_pop_handler();} else{ void* _temp101= _temp99; struct _tagged_string _temp107;
_LL103: if(*(( void**) _temp101) == Cyc_Core_InvalidArg){ _LL108: _temp107=(
struct _tagged_string)(( struct Cyc_Core_InvalidArg_struct*) _temp101)->f1; goto
_LL104;} else{ goto _LL105;} _LL105: goto _LL106; _LL104: Cyc_Arg_stop( initpos,(
void*)({ struct Cyc_Arg_Wrong_struct* _temp109=( struct Cyc_Arg_Wrong_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Wrong_struct)); _temp109[ 0]=({ struct Cyc_Arg_Wrong_struct
_temp110; _temp110.tag= Cyc_Arg_Wrong; _temp110.f1= s; _temp110.f2= arg;
_temp110.f3=( struct _tagged_string)({ char* _temp111=( char*)"an integer";
struct _tagged_string _temp112; _temp112.curr= _temp111; _temp112.base= _temp111;
_temp112.last_plus_one= _temp111 + 11; _temp112;}); _temp110;}); _temp109;}),
speclist, errmsg); _npop_handler( 0u); return; _LL106:( void) _throw( _temp101);
_LL102:;}}} _temp89( n); ++ Cyc_Arg_current; goto _LL68;} _LL80: while( Cyc_Arg_current
< l - 1) { _temp91(({ struct _tagged_ptr0 _temp116= Cyc_Arg_args; struct
_tagged_string* _temp118= _temp116.curr +( Cyc_Arg_current + 1); if( _temp116.base
== 0? 1:( _temp118 < _temp116.base? 1: _temp118 >= _temp116.last_plus_one)){
_throw( Null_Exception);}* _temp118;})); ++ Cyc_Arg_current;} goto _LL68; _LL68:;};
_pop_handler();} else{ void* _temp120= _temp66; struct _tagged_string _temp126;
_LL122: if(*(( void**) _temp120) == Cyc_Arg_Bad){ _LL127: _temp126=( struct
_tagged_string)(( struct Cyc_Arg_Bad_struct*) _temp120)->f1; goto _LL123;} else{
goto _LL124;} _LL124: goto _LL125; _LL123: Cyc_Arg_stop( initpos,( void*)({
struct Cyc_Arg_Message_struct* _temp128=( struct Cyc_Arg_Message_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Message_struct)); _temp128[ 0]=({ struct Cyc_Arg_Message_struct
_temp129; _temp129.tag= Cyc_Arg_Message; _temp129.f1= _temp126; _temp129;});
_temp128;}), speclist, errmsg); goto _LL121; _LL125:( void) _throw( _temp120);
_LL121:;}}} ++ Cyc_Arg_current;} else{{ struct _handler_cons _temp130;
_push_handler(& _temp130);{ void* _temp131=( void*) setjmp( _temp130.handler);
if( ! _temp131){ anonfun( s);; _pop_handler();} else{ void* _temp133= _temp131;
struct _tagged_string _temp139; _LL135: if(*(( void**) _temp133) == Cyc_Arg_Bad){
_LL140: _temp139=( struct _tagged_string)(( struct Cyc_Arg_Bad_struct*) _temp133)->f1;
goto _LL136;} else{ goto _LL137;} _LL137: goto _LL138; _LL136: Cyc_Arg_stop(
initpos,( void*)({ struct Cyc_Arg_Message_struct* _temp141=( struct Cyc_Arg_Message_struct*)
GC_malloc( sizeof( struct Cyc_Arg_Message_struct)); _temp141[ 0]=({ struct Cyc_Arg_Message_struct
_temp142; _temp142.tag= Cyc_Arg_Message; _temp142.f1= _temp139; _temp142;});
_temp141;}), speclist, errmsg); goto _LL134; _LL138:( void) _throw( _temp133);
_LL134:;}}} ++ Cyc_Arg_current;}}}}