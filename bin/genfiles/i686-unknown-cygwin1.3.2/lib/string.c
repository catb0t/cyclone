 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern struct _tagged_arr Cyc_Core_new_string( int); extern struct _tagged_arr
Cyc_Core_rnew_string( struct _RegionHandle*, int); extern unsigned char Cyc_Core_InvalidArg[
15u]; struct Cyc_Core_InvalidArg_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern struct _RegionHandle* Cyc_Core_heap_region; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, unsigned int);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_List_List{
void* hd; struct Cyc_List_List* tl; } ; extern int Cyc_List_length( struct Cyc_List_List*
x); extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern unsigned char Cyc_List_Nth[ 8u]; extern int isalnum( int); extern
int isalpha( int); extern int isascii( int); extern int iscntrl( int); extern
int isdigit( int); extern int isgraph( int); extern int islower( int); extern
int isprint( int); extern int ispunct( int); extern int isspace( int); extern
int isupper( int); extern int isxdigit( int); extern int toascii( int); extern
int tolower( int); extern int toupper( int); extern int _tolower( int); extern
int _toupper( int); extern struct _tagged_arr Cyc_std_strerror( int); extern
unsigned int Cyc_std_strlen( struct _tagged_arr s); extern int Cyc_std_strcmp(
struct _tagged_arr s1, struct _tagged_arr s2); extern int Cyc_std_strptrcmp(
struct _tagged_arr* s1, struct _tagged_arr* s2); extern int Cyc_std_strncmp(
struct _tagged_arr s1, struct _tagged_arr s2, unsigned int len); extern int Cyc_std_zstrcmp(
struct _tagged_arr, struct _tagged_arr); extern int Cyc_std_zstrncmp( struct
_tagged_arr s1, struct _tagged_arr s2, unsigned int n); extern int Cyc_std_zstrptrcmp(
struct _tagged_arr*, struct _tagged_arr*); extern struct _tagged_arr Cyc_std_strcat(
struct _tagged_arr dest, struct _tagged_arr src); extern struct _tagged_arr Cyc_std_strconcat(
struct _tagged_arr, struct _tagged_arr); extern struct _tagged_arr Cyc_std_rstrconcat(
struct _RegionHandle*, struct _tagged_arr, struct _tagged_arr); extern struct
_tagged_arr Cyc_std_strconcat_l( struct Cyc_List_List*); extern struct
_tagged_arr Cyc_std_rstrconcat_l( struct _RegionHandle*, struct Cyc_List_List*);
extern struct _tagged_arr Cyc_std_str_sepstr( struct Cyc_List_List*, struct
_tagged_arr); extern struct _tagged_arr Cyc_std_rstr_sepstr( struct
_RegionHandle*, struct Cyc_List_List*, struct _tagged_arr); extern struct
_tagged_arr Cyc_std_strcpy( struct _tagged_arr dest, struct _tagged_arr src);
extern struct _tagged_arr Cyc_std_strncpy( struct _tagged_arr, struct
_tagged_arr, unsigned int); extern struct _tagged_arr Cyc_std_zstrncpy( struct
_tagged_arr, struct _tagged_arr, unsigned int); extern struct _tagged_arr Cyc_std_expand(
struct _tagged_arr s, unsigned int sz); extern struct _tagged_arr Cyc_std_rexpand(
struct _RegionHandle*, struct _tagged_arr s, unsigned int sz); extern struct
_tagged_arr Cyc_std_realloc_str( struct _tagged_arr str, unsigned int sz);
extern struct _tagged_arr Cyc_std_rrealloc_str( struct _RegionHandle* r, struct
_tagged_arr str, unsigned int sz); extern struct _tagged_arr Cyc_std_strdup(
struct _tagged_arr src); extern struct _tagged_arr Cyc_std_rstrdup( struct
_RegionHandle*, struct _tagged_arr src); extern struct _tagged_arr Cyc_std_substring(
struct _tagged_arr, int ofs, unsigned int n); extern struct _tagged_arr Cyc_std_rsubstring(
struct _RegionHandle*, struct _tagged_arr, int ofs, unsigned int n); extern
struct _tagged_arr Cyc_std_replace_suffix( struct _tagged_arr, struct
_tagged_arr, struct _tagged_arr); extern struct _tagged_arr Cyc_std_rreplace_suffix(
struct _RegionHandle* r, struct _tagged_arr src, struct _tagged_arr curr_suffix,
struct _tagged_arr new_suffix); extern struct _tagged_arr Cyc_std_strchr( struct
_tagged_arr s, unsigned char c); extern struct _tagged_arr Cyc_std_strrchr(
struct _tagged_arr s, unsigned char c); extern struct _tagged_arr Cyc_std_strstr(
struct _tagged_arr haystack, struct _tagged_arr needle); extern struct
_tagged_arr Cyc_std_strpbrk( struct _tagged_arr s, struct _tagged_arr accept);
extern unsigned int Cyc_std_strspn( struct _tagged_arr s, struct _tagged_arr
accept); extern struct Cyc_List_List* Cyc_std_explode( struct _tagged_arr s);
extern struct Cyc_List_List* Cyc_std_rexplode( struct _RegionHandle*, struct
_tagged_arr s); extern struct _tagged_arr Cyc_std_implode( struct Cyc_List_List*
c); extern int Cyc_std_to_int( struct _tagged_arr s, int* offset); extern double
Cyc_std_to_double( struct _tagged_arr s, int* offset); extern int Cyc_std_strcasecmp(
struct _tagged_arr, struct _tagged_arr); extern int Cyc_std_strncasecmp( struct
_tagged_arr s1, struct _tagged_arr s2, unsigned int len); extern double acos(
double); extern float acosf( float); extern double acosh( double); extern float
acoshf( float); extern double asin( double); extern float asinf( float); extern
double asinh( double); extern float asinhf( float); extern double atan( double);
extern double atan2( double, double); extern float atan2f( float, float); extern
float atanf( float); extern double atanh( double); extern float atanhf( float);
extern double cbrt( double); extern float cbrtf( float); extern double ceil(
double); extern float ceilf( float); extern double copysign( double, double);
extern float copysignf( float, float); extern double cos( double); extern float
cosf( float); extern double cosh( double); extern float coshf( float); extern
double drem( double, double); extern float dremf( float, float); extern double
erf( double); extern double erfc( double); extern float erfcf( float); extern
float erff( float); extern double exp( double); extern float expf( float);
extern double expm1( double); extern float expm1f( float); extern double fabs(
double); extern float fabsf( float); extern int finite( double); extern int
finitef( float); extern double floor( double); extern float floorf( float);
extern double fmod( double, double); extern float fmodf( float, float); extern
double frexp( double, int*); extern float frexpf( float, int*); extern double
gamma( double); extern float gammaf( float); extern double hypot( double, double);
extern float hypotf( float, float); extern int ilogb( double); extern int ilogbf(
float); extern int isinf( double); extern int isinff( float); extern int isnan(
double); extern int isnanf( float); extern double j0( double); extern float j0f(
float); extern double j1( double); extern float j1f( float); extern double jn(
int, double); extern float jnf( int, float); extern double ldexp( double, int);
extern float ldexpf( float, int); extern double lgamma( double); extern float
lgammaf( float); extern double log( double); extern float logf( float); extern
double log10( double); extern float log10f( float); extern double log1p( double);
extern float log1pf( float); extern double modf( double, double*); extern float
modff( float, float*); extern double nextafter( double, double); extern float
nextafterf( float, float); extern double pow( double, double); extern float powf(
float, float); extern double remainder( double, double); extern float remainderf(
float, float); extern double rint( double); extern float rintf( float); extern
double scalbn( double, int); extern float scalbnf( float, int); extern double
sin( double); extern float sinf( float); extern double sinh( double); extern
float sinhf( float); extern double sqrt( double); extern float sqrtf( float);
extern double tan( double); extern float tanf( float); extern double tanh(
double); extern float tanhf( float); extern double y0( double); extern float y0f(
float); extern double y1( double); extern float y1f( float); extern double yn(
int, double); extern float ynf( int, float); extern unsigned char* strerror( int
errnum); struct _tagged_arr Cyc_std_strerror( int errnum){ return( struct
_tagged_arr) Cstring_to_string( strerror( errnum));} unsigned int Cyc_std_strlen(
struct _tagged_arr s){ unsigned int i; for( i= 0; i <  _get_arr_size( s, sizeof(
unsigned char)); i ++){ if(*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char),( int) i)) == '\000'){ return i;}} return i;} static
unsigned int Cyc_std_int_strleno( struct _tagged_arr s, struct _tagged_arr error){
int i; for( i= 0; i <  _get_arr_size( s, sizeof( unsigned char)); i ++){ if(*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i))
== '\000'){ break;}} return( unsigned int) i;} static int Cyc_std_case_cmp(
unsigned char c1, unsigned char c2){ return c1 -  c2;} static int Cyc_std_cmp(
struct _tagged_arr s1, unsigned int len1, struct _tagged_arr s2, unsigned int
len2, int(* f)( unsigned char, unsigned char)){ unsigned int min_length= len1 < 
len2? len1: len2; int i= - 1; while( ++ i <  min_length) { int diff= f(*(( const
unsigned char*) _check_unknown_subscript( s1, sizeof( unsigned char), i)),*((
const unsigned char*) _check_unknown_subscript( s2, sizeof( unsigned char), i)));
if( diff !=  0){ return diff;}} return( int) len1 - ( int) len2;} int Cyc_std_strcmp(
struct _tagged_arr s1, struct _tagged_arr s2){ if( s1.curr ==  s2.curr){ return
0;}{ unsigned int len1= Cyc_std_int_strleno( s1, _tag_arr("std::strcmp", sizeof(
unsigned char), 12u)); unsigned int len2= Cyc_std_int_strleno( s2, _tag_arr("std::strcmp",
sizeof( unsigned char), 12u)); return Cyc_std_cmp( s1, len1, s2, len2, Cyc_std_case_cmp);}}
int Cyc_std_strptrcmp( struct _tagged_arr* s1, struct _tagged_arr* s2){ return
Cyc_std_strcmp(* s1,* s2);} static int Cyc_std_ncmp( struct _tagged_arr s1,
unsigned int len1, struct _tagged_arr s2, unsigned int len2, unsigned int n, int(*
f)( unsigned char, unsigned char)){ if( n <=  0){ return 0;}{ unsigned int
min_len= len1 >  len2? len2: len1; unsigned int bound= min_len >  n? n: min_len;{
int i= 0; for( 0; i <  bound; i ++){ int retc; if(( retc= f(*(( const
unsigned char*) _check_unknown_subscript( s1, sizeof( unsigned char), i)),*((
const unsigned char*) _check_unknown_subscript( s2, sizeof( unsigned char), i))))
!=  0){ return retc;}}} return 0;}} int Cyc_std_strncmp( struct _tagged_arr s1,
struct _tagged_arr s2, unsigned int n){ unsigned int len1= Cyc_std_int_strleno(
s1, _tag_arr("std::strncmp", sizeof( unsigned char), 13u)); unsigned int len2=
Cyc_std_int_strleno( s2, _tag_arr("std::strncmp", sizeof( unsigned char), 13u));
return Cyc_std_ncmp( s1, len1, s2, len2, n, Cyc_std_case_cmp);} int Cyc_std_zstrcmp(
struct _tagged_arr a, struct _tagged_arr b){ if( a.curr ==  b.curr){ return 0;}{
unsigned int as= _get_arr_size( a, sizeof( unsigned char)); unsigned int bs=
_get_arr_size( b, sizeof( unsigned char)); unsigned int min_length= as <  bs? as:
bs; int i= - 1; while( ++ i <  min_length) { int diff=( int)*(( const
unsigned char*) _check_unknown_subscript( a, sizeof( unsigned char), i)) - ( int)*((
const unsigned char*) _check_unknown_subscript( b, sizeof( unsigned char), i));
if( diff !=  0){ return diff;}} return( int) as - ( int) bs;}} int Cyc_std_zstrncmp(
struct _tagged_arr s1, struct _tagged_arr s2, unsigned int n){ if( n <=  0){
return 0;}{ unsigned int s1size= _get_arr_size( s1, sizeof( unsigned char));
unsigned int s2size= _get_arr_size( s2, sizeof( unsigned char)); unsigned int
min_size= s1size >  s2size? s2size: s1size; unsigned int bound= min_size >  n? n:
min_size;{ int i= 0; for( 0; i <  bound; i ++){ if(*(( const unsigned char*)
_check_unknown_subscript( s1, sizeof( unsigned char), i)) < *(( const
unsigned char*) _check_unknown_subscript( s2, sizeof( unsigned char), i))){
return - 1;} else{ if(*(( const unsigned char*) _check_unknown_subscript( s2,
sizeof( unsigned char), i)) < *(( const unsigned char*) _check_unknown_subscript(
s1, sizeof( unsigned char), i))){ return 1;}}}} if( min_size <=  bound){ return
0;} if( s1size <  s2size){ return - 1;} else{ return 1;}}} int Cyc_std_zstrptrcmp(
struct _tagged_arr* a, struct _tagged_arr* b){ return Cyc_std_zstrcmp(* a,* b);}
static struct _tagged_arr Cyc_std_int_strcato( struct _tagged_arr dest, struct
_tagged_arr src, struct _tagged_arr error){ int i; unsigned int dsize;
unsigned int slen; unsigned int dlen; dsize= _get_arr_size( dest, sizeof(
unsigned char)); dlen= Cyc_std_strlen(( struct _tagged_arr) dest); slen= Cyc_std_int_strleno(
src, error); if( slen +  dlen <=  dsize){ for( i= 0; i <  slen; i ++){*((
unsigned char*) _check_unknown_subscript( dest, sizeof( unsigned char),( int)( i
+  dlen)))=*(( const unsigned char*) _check_unknown_subscript( src, sizeof(
unsigned char), i));} if( i !=  dsize){*(( unsigned char*)
_check_unknown_subscript( dest, sizeof( unsigned char),( int)( i +  dlen)))='\000';}}
else{( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp0=( struct
Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp0[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp1; _temp1.tag= Cyc_Core_InvalidArg;
_temp1.f1= error; _temp1;}); _temp0;}));} return dest;} struct _tagged_arr Cyc_std_strcat(
struct _tagged_arr dest, struct _tagged_arr src){ return Cyc_std_int_strcato(
dest, src, _tag_arr("std::strcat", sizeof( unsigned char), 12u));} struct
_tagged_arr Cyc_std_rstrconcat( struct _RegionHandle* r, struct _tagged_arr a,
struct _tagged_arr b){ unsigned int _temp2= Cyc_std_strlen( a); unsigned int
_temp3= Cyc_std_strlen( b); struct _tagged_arr ans= Cyc_Core_rnew_string( r,(
int)(( _temp2 +  _temp3) +  1)); int i; int j; for( i= 0; i <  _temp2; ++ i){*((
unsigned char*) _check_unknown_subscript( ans, sizeof( unsigned char), i))=*((
const unsigned char*) _check_unknown_subscript( a, sizeof( unsigned char), i));}
for( j= 0; j <  _temp3; ++ j){*(( unsigned char*) _check_unknown_subscript( ans,
sizeof( unsigned char), i +  j))=*(( const unsigned char*)
_check_unknown_subscript( b, sizeof( unsigned char), j));} return ans;} struct
_tagged_arr Cyc_std_strconcat( struct _tagged_arr a, struct _tagged_arr b){
return Cyc_std_rstrconcat( Cyc_Core_heap_region, a, b);} struct _tagged_arr Cyc_std_rstrconcat_l(
struct _RegionHandle* r, struct Cyc_List_List* strs){ unsigned int len;
unsigned int total_len= 0; struct _tagged_arr ans;{ struct _RegionHandle _temp4=
_new_region(); struct _RegionHandle* temp=& _temp4; _push_region( temp);{ struct
Cyc_List_List* lens=({ struct Cyc_List_List* _temp7=( struct Cyc_List_List*)
_region_malloc( temp, sizeof( struct Cyc_List_List)); _temp7->hd=( void*)((
unsigned int) 0); _temp7->tl= 0; _temp7;}); struct Cyc_List_List* end= lens;{
struct Cyc_List_List* p= strs; for( 0; p !=  0; p=(( struct Cyc_List_List*)
_check_null( p))->tl){ len= Cyc_std_strlen(*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( p))->hd)); total_len += len;(( struct Cyc_List_List*) _check_null(
end))->tl=({ struct Cyc_List_List* _temp5=( struct Cyc_List_List*)
_region_malloc( temp, sizeof( struct Cyc_List_List)); _temp5->hd=( void*) len;
_temp5->tl= 0; _temp5;}); end=(( struct Cyc_List_List*) _check_null( end))->tl;}}
lens=(( struct Cyc_List_List*) _check_null( lens))->tl; ans= Cyc_Core_rnew_string(
r,( int)( total_len +  1));{ unsigned int i= 0; while( strs !=  0) { struct
_tagged_arr _temp6=*(( struct _tagged_arr*)(( struct Cyc_List_List*) _check_null(
strs))->hd); len=( unsigned int)(( struct Cyc_List_List*) _check_null( lens))->hd;
Cyc_std_strncpy( _tagged_arr_plus( ans, sizeof( unsigned char),( int) i), _temp6,
len); i += len; strs=(( struct Cyc_List_List*) _check_null( strs))->tl; lens=((
struct Cyc_List_List*) _check_null( lens))->tl;}}}; _pop_region( temp);} return
ans;} struct _tagged_arr Cyc_std_strconcat_l( struct Cyc_List_List* strs){
return Cyc_std_rstrconcat_l( Cyc_Core_heap_region, strs);} struct _tagged_arr
Cyc_std_rstr_sepstr( struct _RegionHandle* r, struct Cyc_List_List* strs, struct
_tagged_arr separator){ if( strs ==  0){ return Cyc_Core_rnew_string( r, 0);}
if((( struct Cyc_List_List*) _check_null( strs))->tl ==  0){ return Cyc_std_rstrdup(
r,*(( struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( strs))->hd));}{
struct Cyc_List_List* _temp8= strs; struct _RegionHandle _temp9= _new_region();
struct _RegionHandle* temp=& _temp9; _push_region( temp);{ struct Cyc_List_List*
lens=({ struct Cyc_List_List* _temp13=( struct Cyc_List_List*) _region_malloc(
temp, sizeof( struct Cyc_List_List)); _temp13->hd=( void*)(( unsigned int) 0);
_temp13->tl= 0; _temp13;}); struct Cyc_List_List* end= lens; unsigned int len= 0;
unsigned int total_len= 0; unsigned int list_len= 0; for( 0; _temp8 !=  0;
_temp8=(( struct Cyc_List_List*) _check_null( _temp8))->tl){ len= Cyc_std_strlen(*((
struct _tagged_arr*)(( struct Cyc_List_List*) _check_null( _temp8))->hd));
total_len += len;(( struct Cyc_List_List*) _check_null( end))->tl=({ struct Cyc_List_List*
_temp10=( struct Cyc_List_List*) _region_malloc( temp, sizeof( struct Cyc_List_List));
_temp10->hd=( void*) len; _temp10->tl= 0; _temp10;}); end=(( struct Cyc_List_List*)
_check_null( end))->tl; ++ list_len;} lens=(( struct Cyc_List_List*) _check_null(
lens))->tl;{ unsigned int seplen= Cyc_std_strlen( separator); total_len +=(
list_len -  1) *  seplen;{ struct _tagged_arr ans= Cyc_Core_rnew_string( r,( int)
total_len); unsigned int i= 0; while((( struct Cyc_List_List*) _check_null( strs))->tl
!=  0) { struct _tagged_arr _temp11=*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( strs))->hd); len=( unsigned int)(( struct Cyc_List_List*)
_check_null( lens))->hd; Cyc_std_strncpy( _tagged_arr_plus( ans, sizeof(
unsigned char),( int) i), _temp11, len); i += len; Cyc_std_strncpy(
_tagged_arr_plus( ans, sizeof( unsigned char),( int) i), separator, seplen); i +=
seplen; strs=(( struct Cyc_List_List*) _check_null( strs))->tl; lens=(( struct
Cyc_List_List*) _check_null( lens))->tl;} Cyc_std_strncpy( _tagged_arr_plus( ans,
sizeof( unsigned char),( int) i),*(( struct _tagged_arr*)(( struct Cyc_List_List*)
_check_null( strs))->hd),( unsigned int)(( struct Cyc_List_List*) _check_null(
lens))->hd);{ struct _tagged_arr _temp12= ans; _npop_handler( 0u); return
_temp12;}}}}; _pop_region( temp);}} struct _tagged_arr Cyc_std_str_sepstr(
struct Cyc_List_List* strs, struct _tagged_arr separator){ return Cyc_std_rstr_sepstr(
Cyc_Core_heap_region, strs, separator);} struct _tagged_arr Cyc_std_strncpy(
struct _tagged_arr dest, struct _tagged_arr src, unsigned int n){ unsigned int
_temp14= _get_arr_size( dest, sizeof( unsigned char)); unsigned int _temp15=
_get_arr_size( src, sizeof( unsigned char)); if( n <=  0){ return dest;} if( n > 
_temp14? 1: n >  _temp15){( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp16=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp16[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp17; _temp17.tag= Cyc_Core_InvalidArg;
_temp17.f1= _tag_arr("std::strncpy", sizeof( unsigned char), 13u); _temp17;});
_temp16;}));}{ int i; for( i= 0; i <  n; i ++){ unsigned char _temp18=*(( const
unsigned char*) _check_unknown_subscript( src, sizeof( unsigned char), i)); if(
_temp18 == '\000'){ break;}*(( unsigned char*) _check_unknown_subscript( dest,
sizeof( unsigned char), i))= _temp18;} for( 0; i <  n; i ++){*(( unsigned char*)
_check_unknown_subscript( dest, sizeof( unsigned char), i))='\000';} return dest;}}
struct _tagged_arr Cyc_std_zstrncpy( struct _tagged_arr dest, struct _tagged_arr
src, unsigned int n){ unsigned int _temp19= _get_arr_size( dest, sizeof(
unsigned char)); unsigned int _temp20= _get_arr_size( src, sizeof( unsigned char));
if( n <=  0){ return dest;} if( n >  _temp19? 1: n >  _temp20){( int) _throw((
void*)({ struct Cyc_Core_InvalidArg_struct* _temp21=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp21[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp22; _temp22.tag= Cyc_Core_InvalidArg; _temp22.f1=
_tag_arr("std::zstrncpy", sizeof( unsigned char), 14u); _temp22;}); _temp21;}));}{
int i; for( i= 0; i <  n; i ++){*(( unsigned char*) _check_unknown_subscript(
dest, sizeof( unsigned char), i))=*(( const unsigned char*)
_check_unknown_subscript( src, sizeof( unsigned char), i));} return dest;}}
struct _tagged_arr Cyc_std_strcpy( struct _tagged_arr dest, struct _tagged_arr
src){ unsigned int len= Cyc_std_strlen( src); Cyc_std_strncpy( dest, src, len);
if( len !=  _get_arr_size( dest, sizeof( unsigned char))){*(( unsigned char*)
_check_unknown_subscript( dest, sizeof( unsigned char),( int) len))='\000';}
return dest;} struct _tagged_arr Cyc_std_rstrdup( struct _RegionHandle* r,
struct _tagged_arr src){ unsigned int len; struct _tagged_arr temp; len= Cyc_std_strlen(
src); temp= Cyc_Core_rnew_string( r,( int)( len +  1)); Cyc_std_strncpy( temp,
src, len); return temp;} struct _tagged_arr Cyc_std_strdup( struct _tagged_arr
src){ return Cyc_std_rstrdup( Cyc_Core_heap_region, src);} struct _tagged_arr
Cyc_std_rexpand( struct _RegionHandle* r, struct _tagged_arr s, unsigned int sz){
struct _tagged_arr temp; unsigned int slen; slen= Cyc_std_strlen( s); sz= sz > 
slen? sz: slen; temp= Cyc_Core_rnew_string( r,( int) sz); Cyc_std_strncpy( temp,
s, slen); if( slen !=  _get_arr_size( s, sizeof( unsigned char))){*((
unsigned char*) _check_unknown_subscript( temp, sizeof( unsigned char),( int)
slen))='\000';} return temp;} struct _tagged_arr Cyc_std_expand( struct
_tagged_arr s, unsigned int sz){ return Cyc_std_rexpand( Cyc_Core_heap_region, s,
sz);} struct _tagged_arr Cyc_std_rrealloc_str( struct _RegionHandle* r, struct
_tagged_arr str, unsigned int sz){ unsigned int maxsizeP= _get_arr_size( str,
sizeof( unsigned char)); if( maxsizeP ==  0){ maxsizeP= 30 >  sz? 30: sz; str=
Cyc_Core_rnew_string( r,( int) maxsizeP);*(( unsigned char*)
_check_unknown_subscript( str, sizeof( unsigned char), 0))='\000';} else{ if( sz
>  maxsizeP){ maxsizeP= maxsizeP *  2 > ( sz *  5) /  4? maxsizeP *  2:( sz *  5)
/  4; str= Cyc_std_rexpand( r,( struct _tagged_arr) str, maxsizeP);}} return str;}
struct _tagged_arr Cyc_std_realloc_str( struct _tagged_arr str, unsigned int sz){
return Cyc_std_rrealloc_str( Cyc_Core_heap_region, str, sz);} struct _tagged_arr
Cyc_std_rsubstring( struct _RegionHandle* r, struct _tagged_arr s, int start,
unsigned int amt){ if(( start <  0? 1: amt <  0)? 1:( start + ( int) amt) -  1 > (
int) _get_arr_size( s, sizeof( unsigned char))){( int) _throw(( void*)({ struct
Cyc_Core_InvalidArg_struct* _temp23=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp23[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp24; _temp24.tag= Cyc_Core_InvalidArg; _temp24.f1=
_tag_arr("std::substring", sizeof( unsigned char), 15u); _temp24;}); _temp23;}));}{
struct _tagged_arr ans= Cyc_Core_rnew_string( r,( int)( amt +  1));{
unsigned int i= 0; for( 0; i <  amt; ++ i){*(( unsigned char*)
_check_unknown_subscript( ans, sizeof( unsigned char),( int) i))=*(( const
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char),( int)(
start +  i)));}}*(( unsigned char*) _check_unknown_subscript( ans, sizeof(
unsigned char),( int) amt))='\000'; return ans;}} struct _tagged_arr Cyc_std_substring(
struct _tagged_arr s, int start, unsigned int amt){ return Cyc_std_rsubstring(
Cyc_Core_heap_region, s, start, amt);} struct _tagged_arr Cyc_std_rreplace_suffix(
struct _RegionHandle* r, struct _tagged_arr src, struct _tagged_arr curr_suffix,
struct _tagged_arr new_suffix){ unsigned int m= _get_arr_size( src, sizeof(
unsigned char)); unsigned int n= _get_arr_size( curr_suffix, sizeof(
unsigned char)); struct _tagged_arr err= _tag_arr("std::replace_suffix", sizeof(
unsigned char), 20u); if( m <  n){( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp25=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp25[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp26; _temp26.tag= Cyc_Core_InvalidArg;
_temp26.f1= err; _temp26;}); _temp25;}));}{ unsigned int i= 1; for( 0; i <=  n;
++ i){ if(*(( const unsigned char*) _check_unknown_subscript( src, sizeof(
unsigned char),( int)( m -  i))) != *(( const unsigned char*)
_check_unknown_subscript( curr_suffix, sizeof( unsigned char),( int)( n -  i)))){(
int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp27=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp27[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp28; _temp28.tag= Cyc_Core_InvalidArg; _temp28.f1=
err; _temp28;}); _temp27;}));}}}{ struct _tagged_arr ans= Cyc_Core_rnew_string(
r,( int)(( m -  n) +  _get_arr_size( new_suffix, sizeof( unsigned char)))); Cyc_std_strncpy(
ans, src, m -  n); Cyc_std_strncpy( _tagged_arr_plus( _tagged_arr_plus( ans,
sizeof( unsigned char),( int) m), sizeof( unsigned char), -( int) n), new_suffix,
_get_arr_size( new_suffix, sizeof( unsigned char))); return ans;}} struct
_tagged_arr Cyc_std_replace_suffix( struct _tagged_arr src, struct _tagged_arr
curr_suffix, struct _tagged_arr new_suffix){ return Cyc_std_rreplace_suffix( Cyc_Core_heap_region,
src, curr_suffix, new_suffix);} struct _tagged_arr Cyc_std_strpbrk( struct
_tagged_arr s, struct _tagged_arr accept){ int len=( int) Cyc_std_int_strleno((
struct _tagged_arr) s, _tag_arr("std::strpbrk", sizeof( unsigned char), 13u));
unsigned char c='\000'; for( 0; len >  0?( c=*(( unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), 0u))) !=  0: 0;( len --,
_tagged_arr_inplace_plus_post(& s, sizeof( unsigned char), 1))){ int j= 0; for(
0; j <  _get_arr_size( accept, sizeof( unsigned char)); j ++){ if( c == *((
const unsigned char*) _check_unknown_subscript( accept, sizeof( unsigned char),
j))){ return s;}}} return _tag_arr( 0u, 0u, 0u);} struct _tagged_arr Cyc_std_strchr(
struct _tagged_arr s, unsigned char c){ int len=( int) Cyc_std_int_strleno( s,
_tag_arr("std::strchr", sizeof( unsigned char), 12u)); unsigned char c2='\000';
for( 0; len >  0?( c2=*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), 0u))) !=  0: 0;( len --, _tagged_arr_inplace_plus_post(&
s, sizeof( unsigned char), 1))){ if( c2 ==  c){ return s;}} return( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u);} struct _tagged_arr Cyc_std_strrchr( struct
_tagged_arr s, unsigned char c){ int len=( int) Cyc_std_int_strleno(( struct
_tagged_arr) s, _tag_arr("std::strrchr", sizeof( unsigned char), 13u)); int i=
len -  1; _tagged_arr_inplace_plus(& s, sizeof( unsigned char), i); for( 0; i >= 
0;( i --, _tagged_arr_inplace_plus_post(& s, sizeof( unsigned char), -1))){ if(*((
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), 0u)) ==  c){
return s;}} return _tag_arr( 0u, 0u, 0u);} struct _tagged_arr Cyc_std_strstr(
struct _tagged_arr haystack, struct _tagged_arr needle){ if( !(( unsigned int)
haystack.curr)? 1: !(( unsigned int) needle.curr)){( int) _throw(( void*)({
struct Cyc_Core_InvalidArg_struct* _temp29=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp29[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp30; _temp30.tag= Cyc_Core_InvalidArg; _temp30.f1=
_tag_arr("std::strstr", sizeof( unsigned char), 12u); _temp30;}); _temp29;}));}
if(*(( const unsigned char*) _check_unknown_subscript( needle, sizeof(
unsigned char), 0u)) == '\000'){ return haystack;}{ int len=( int) Cyc_std_int_strleno(
needle, _tag_arr("std::strstr", sizeof( unsigned char), 12u));{ struct
_tagged_arr start= haystack; for( 0;( start= Cyc_std_strchr( start,*(( const
unsigned char*) _check_unknown_subscript( needle, sizeof( unsigned char), 0u)))).curr
!= (( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr; start= Cyc_std_strchr(
_tagged_arr_plus( start, sizeof( unsigned char), 1),*(( const unsigned char*)
_check_unknown_subscript( needle, sizeof( unsigned char), 0u)))){ if( Cyc_std_strncmp(
start, needle,( unsigned int) len) ==  0){ return start;}}} return( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u);}} unsigned int Cyc_std_strspn( struct
_tagged_arr s, struct _tagged_arr accept){ unsigned int len= Cyc_std_int_strleno(
s, _tag_arr("std::strspn", sizeof( unsigned char), 12u));{ unsigned int i= 0;
for( 0; i <  len; i ++){ int j; for( j= 0; j <  _get_arr_size( accept, sizeof(
unsigned char)); j ++){ if(*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char),( int) i)) == *(( const unsigned char*)
_check_unknown_subscript( accept, sizeof( unsigned char), j))){ break;}} if( j
==  _get_arr_size( accept, sizeof( unsigned char))){ return i;}}} return len;}
struct Cyc_List_List* Cyc_std_rexplode( struct _RegionHandle* r, struct
_tagged_arr s){ struct Cyc_List_List* result= 0;{ int i=( int)( Cyc_std_strlen(
s) -  1); for( 0; i >=  0; i --){ result=({ struct Cyc_List_List* _temp31=(
struct Cyc_List_List*) _region_malloc( r, sizeof( struct Cyc_List_List));
_temp31->hd=( void*)(( int)*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), i))); _temp31->tl= result; _temp31;});}} return result;}
struct Cyc_List_List* Cyc_std_explode( struct _tagged_arr s){ return Cyc_std_rexplode(
Cyc_Core_heap_region, s);} struct _tagged_arr Cyc_std_implode( struct Cyc_List_List*
chars){ struct _tagged_arr s= Cyc_Core_new_string((( int(*)( struct Cyc_List_List*
x)) Cyc_List_length)( chars)); unsigned int i= 0; while( chars !=  0) {*((
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char),( int) i ++))=(
unsigned char)(( int)(( struct Cyc_List_List*) _check_null( chars))->hd); chars=((
struct Cyc_List_List*) _check_null( chars))->tl;} return s;} int Cyc_std_to_int(
struct _tagged_arr s, int* offset){ int ans= 0; int sn= 1; int digit_seen= 0;
int i= offset ==  0? 0:*(( int*) _check_null( offset)); while( i < 
_get_arr_size( s, sizeof( unsigned char))? isspace(( int)*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i))): 0) { i ++;} if( i < 
_get_arr_size( s, sizeof( unsigned char))?*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)) == '-': 0){ i ++; sn= -
1;} while( i <  _get_arr_size( s, sizeof( unsigned char))? isspace(( int)*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i))):
0) { i ++;} while( i <  _get_arr_size( s, sizeof( unsigned char))? isdigit(( int)*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i))):
0) { digit_seen= 1; ans= ans *  10 + ( int)(*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)) - '0'); i ++;} ans= ans
*  sn; if( ! digit_seen){( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp32=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp32[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp33; _temp33.tag= Cyc_Core_InvalidArg;
_temp33.f1= _tag_arr("std::to_int", sizeof( unsigned char), 12u); _temp33;});
_temp32;}));} if( offset !=  0){*(( int*) _check_null( offset))= i;} return ans;}
double Cyc_std_to_double( struct _tagged_arr s, int* offset){ double ans=(
double) 0.0; int sn= 1; int digit_seen= 0; int i= offset ==  0? 0:*(( int*)
_check_null( offset)); while( i <  _get_arr_size( s, sizeof( unsigned char))?
isspace(( int)*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), i))): 0) { i ++;} if( i <  _get_arr_size( s, sizeof(
unsigned char))?*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), i)) == '-': 0){ i ++; sn= - 1;} while( i <  _get_arr_size( s,
sizeof( unsigned char))? isdigit(( int)*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i))): 0) { digit_seen= 1;
ans= ans *  10.0 + ( double)(*(( const unsigned char*) _check_unknown_subscript(
s, sizeof( unsigned char), i)) - '0'); i ++;} if( i <  _get_arr_size( s, sizeof(
unsigned char))?*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), i)) == '.': 0){ i ++;{ double divisor=( double) 0.1; while( i < 
_get_arr_size( s, sizeof( unsigned char))? isdigit(( int)*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i))): 0) { digit_seen= 1;
ans= ans + ( double)(*(( const unsigned char*) _check_unknown_subscript( s,
sizeof( unsigned char), i)) - '0') *  divisor; divisor= divisor /  10.0; i ++;}}}
if( ! digit_seen){( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp34=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp34[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp35; _temp35.tag= Cyc_Core_InvalidArg;
_temp35.f1= _tag_arr("std::to_double", sizeof( unsigned char), 15u); _temp35;});
_temp34;}));} if( i <  _get_arr_size( s, sizeof( unsigned char))?*(( const
unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i)) == 'e'?
1:*(( const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char),
i)) == 'E': 0){ i ++;{ int exponent= 0; int exp_sign= 1; digit_seen= 0; if( i < 
_get_arr_size( s, sizeof( unsigned char))?*(( const unsigned char*)
_check_unknown_subscript( s, sizeof( unsigned char), i)) == '-': 0){ i ++;
exp_sign= - 1;} else{ if( i <  _get_arr_size( s, sizeof( unsigned char))?*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i))
== '+': 0){ i ++;}} while( i <  _get_arr_size( s, sizeof( unsigned char))?
isdigit(( int)*(( const unsigned char*) _check_unknown_subscript( s, sizeof(
unsigned char), i))): 0) { digit_seen= 1; exponent= exponent *  10 + ( int)(*((
const unsigned char*) _check_unknown_subscript( s, sizeof( unsigned char), i)) - '0');
i ++;} if( ! digit_seen){( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp36=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp36[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp37; _temp37.tag= Cyc_Core_InvalidArg;
_temp37.f1= _tag_arr("std::to_double", sizeof( unsigned char), 15u); _temp37;});
_temp36;}));} ans= ans *  pow(( double) 10.0,( double)( exponent *  exp_sign));}}
if( offset !=  0){*(( int*) _check_null( offset))= i;} return ans *  sn;} int
Cyc_std_strcasecmp( struct _tagged_arr s1, struct _tagged_arr s2){ if( s1.curr
==  s2.curr){ return 0;}{ unsigned int len1= Cyc_std_int_strleno( s1, _tag_arr("std::strcasecmp",
sizeof( unsigned char), 16u)); unsigned int len2= Cyc_std_int_strleno( s2,
_tag_arr("std::strcasecmp", sizeof( unsigned char), 16u)); return Cyc_std_cmp(
s1, len1, s2, len2, Cyc_std_case_cmp);}} static int Cyc_std_nocase_cmp(
unsigned char c1, unsigned char c2){ return Cyc_std_case_cmp(( unsigned char)
toupper(( int) c1),( unsigned char) toupper(( int) c2));} int Cyc_std_strncasecmp(
struct _tagged_arr s1, struct _tagged_arr s2, unsigned int n){ unsigned int len1=
Cyc_std_int_strleno( s1, _tag_arr("std::strncasecmp", sizeof( unsigned char), 17u));
unsigned int len2= Cyc_std_int_strleno( s2, _tag_arr("std::strncasecmp", sizeof(
unsigned char), 17u)); return Cyc_std_ncmp( s1, len1, s2, len2, n, Cyc_std_nocase_cmp);}