 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; extern void
exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ; extern
unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, int); extern
struct _tagged_arr ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern int* __errno(); struct Cyc_Getopt_option{ struct
_tagged_arr name; int has_arg; int* flag; int val; } ; extern int close( int);
extern int getpid(); extern int getppid(); extern int fork(); extern int fchdir(
int); extern int dup( int); extern int dup2( int, int); extern unsigned short
getuid(); extern unsigned short geteuid(); extern unsigned short getgid();
extern unsigned short getegid(); extern int select( int n, struct Cyc__types_fd_set*
readfds, struct Cyc__types_fd_set* writefds, struct Cyc__types_fd_set* exceptfds,
struct Cyc_timeval* timeout); extern int pipe( int* filedes); extern int lseek(
int filedes, int offset, int whence); int Cyc_Unistd_chdir( struct _tagged_arr);
struct _tagged_arr Cyc_Unistd_getcwd( struct _tagged_arr buf, int size); int Cyc_Unistd_execl(
struct _tagged_arr path, struct _tagged_arr arg0, struct _tagged_arr argv); int
Cyc_Unistd_execve( struct _tagged_arr filename, struct _tagged_arr argv, struct
_tagged_arr envp); int Cyc_Unistd_read( int fd, struct _tagged_arr buf, int
count); int Cyc_Unistd_write( int fd, struct _tagged_arr buf, int count); int
Cyc_Unistd_unlink( struct _tagged_arr pathname); extern int chdir( unsigned char*);
extern unsigned char* getcwd( unsigned char* buf, int size); extern int execv(
unsigned char* path, unsigned char** argv); extern int execve( unsigned char*
path, unsigned char** argv, unsigned char** envp); extern int read( int fd,
unsigned char* buf, int count); extern int write( int fd, unsigned char* buf,
int count); extern int unlink( unsigned char* pathname); int Cyc_Unistd_chdir(
struct _tagged_arr path){ return chdir( string_to_Cstring( path));} struct
_tagged_arr Cyc_Unistd_getcwd( struct _tagged_arr buf, int size){ if( !((
unsigned int) buf.curr)? 1: _get_arr_size( buf, sizeof( unsigned char)) < size){(
int) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp0=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp0[ 0]=({ struct Cyc_Core_Failure_struct
_temp1; _temp1.tag= Cyc_Core_Failure; _temp1.f1= _tag_arr("getcwd: invalid buf argument",
sizeof( unsigned char), 29u); _temp1;}); _temp0;}));}{ unsigned char* response=
getcwd(( unsigned char*) _check_null( _untag_arr( buf, sizeof( unsigned char), 0u)),
size); return( unsigned int) response? buf: _tag_arr( 0u, 0u, 0u);}} int Cyc_Unistd_execl(
struct _tagged_arr path, struct _tagged_arr arg0, struct _tagged_arr argv){ if((*((
struct _tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr),(
int)( _get_arr_size( argv, sizeof( struct _tagged_arr)) - 1)))).curr !=(( struct
_tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp2=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp2[ 0]=({ struct Cyc_Core_Failure_struct _temp3; _temp3.tag= Cyc_Core_Failure;
_temp3.f1= _tag_arr("execl: arg list must be null-terminated", sizeof(
unsigned char), 40u); _temp3;}); _temp2;}));}{ struct _tagged_arr newargs=({
unsigned int _temp4= 1 + _get_arr_size( argv, sizeof( struct _tagged_arr));
unsigned char** _temp5=( unsigned char**) GC_malloc( sizeof( unsigned char*) *
_temp4); struct _tagged_arr _temp7= _tag_arr( _temp5, sizeof( unsigned char*), 1
+ _get_arr_size( argv, sizeof( struct _tagged_arr)));{ unsigned int _temp6=
_temp4; unsigned int i; for( i= 0; i < _temp6; i ++){ _temp5[ i]= 0;}}; _temp7;});*((
unsigned char**) _check_unknown_subscript( newargs, sizeof( unsigned char*), 0))=
string_to_Cstring( arg0);{ int i= 0; for( 0; i < _get_arr_size( argv, sizeof(
struct _tagged_arr)); i ++){*(( unsigned char**) _check_unknown_subscript(
newargs, sizeof( unsigned char*), i + 1))= string_to_Cstring(*(( struct
_tagged_arr*) _check_unknown_subscript( argv, sizeof( struct _tagged_arr), i)));}}
return execv( string_to_Cstring( path),( unsigned char**) _check_null(
_untag_arr( newargs, sizeof( unsigned char*), 1u)));}} int Cyc_Unistd_execve(
struct _tagged_arr filename, struct _tagged_arr argv, struct _tagged_arr envp){
if((*(( struct _tagged_arr*) _check_unknown_subscript( argv, sizeof( struct
_tagged_arr),( int)( _get_arr_size( argv, sizeof( struct _tagged_arr)) - 1)))).curr
!=(( struct _tagged_arr) _tag_arr( 0u, 0u, 0u)).curr){( int) _throw(( void*)({
struct Cyc_Core_Failure_struct* _temp8=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp8[ 0]=({ struct Cyc_Core_Failure_struct
_temp9; _temp9.tag= Cyc_Core_Failure; _temp9.f1= _tag_arr("execve: arg list must be null-terminated",
sizeof( unsigned char), 41u); _temp9;}); _temp8;}));}{ struct _tagged_arr
newargs=({ unsigned int _temp14= _get_arr_size( argv, sizeof( struct _tagged_arr));
unsigned char** _temp15=( unsigned char**) GC_malloc( sizeof( unsigned char*) *
_temp14); struct _tagged_arr _temp17= _tag_arr( _temp15, sizeof( unsigned char*),
_get_arr_size( argv, sizeof( struct _tagged_arr)));{ unsigned int _temp16=
_temp14; unsigned int i; for( i= 0; i < _temp16; i ++){ _temp15[ i]= 0;}};
_temp17;});{ int i= 0; for( 0; i < _get_arr_size( argv, sizeof( struct
_tagged_arr)); i ++){*(( unsigned char**) _check_unknown_subscript( newargs,
sizeof( unsigned char*), i))= string_to_Cstring(*(( struct _tagged_arr*)
_check_unknown_subscript( argv, sizeof( struct _tagged_arr), i)));}}{ struct
_tagged_arr newenvp=({ unsigned int _temp10= _get_arr_size( envp, sizeof( struct
_tagged_arr)); unsigned char** _temp11=( unsigned char**) GC_malloc( sizeof(
unsigned char*) * _temp10); struct _tagged_arr _temp13= _tag_arr( _temp11,
sizeof( unsigned char*), _get_arr_size( envp, sizeof( struct _tagged_arr)));{
unsigned int _temp12= _temp10; unsigned int i; for( i= 0; i < _temp12; i ++){
_temp11[ i]= 0;}}; _temp13;});{ int i= 0; for( 0; i < _get_arr_size( envp,
sizeof( struct _tagged_arr)); i ++){*(( unsigned char**)
_check_unknown_subscript( newenvp, sizeof( unsigned char*), i))=
string_to_Cstring(*(( struct _tagged_arr*) _check_unknown_subscript( envp,
sizeof( struct _tagged_arr), i)));}} return execve( string_to_Cstring( filename),(
unsigned char**) _check_null( _untag_arr( newargs, sizeof( unsigned char*), 1u)),(
unsigned char**) _check_null( _untag_arr( newenvp, sizeof( unsigned char*), 1u)));}}}
int Cyc_Unistd_read( int fd, struct _tagged_arr buf, int count){ if( count >
_get_arr_size( buf, sizeof( unsigned char))){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp18=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp18[ 0]=({ struct Cyc_Core_Failure_struct _temp19; _temp19.tag= Cyc_Core_Failure;
_temp19.f1= _tag_arr("read: called with count > buf.size", sizeof( unsigned char),
35u); _temp19;}); _temp18;}));} return read( fd, underlying_Cstring(( struct
_tagged_arr) buf), count);} int Cyc_Unistd_write( int fd, struct _tagged_arr buf,
int count){ if( count > _get_arr_size( buf, sizeof( unsigned char))){( int)
_throw(( void*)({ struct Cyc_Core_Failure_struct* _temp20=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp20[ 0]=({ struct Cyc_Core_Failure_struct
_temp21; _temp21.tag= Cyc_Core_Failure; _temp21.f1= _tag_arr("write: called with count > buf.size",
sizeof( unsigned char), 36u); _temp21;}); _temp20;}));} return write( fd,
string_to_Cstring( buf), count);} int Cyc_Unistd_unlink( struct _tagged_arr
pathname){ return unlink( string_to_Cstring( pathname));}