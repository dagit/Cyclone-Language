 struct Cyc_timeval{ int tv_sec; int tv_usec; } ; struct Cyc_timespec{
unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct Cyc_timespec
it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{ int
fds_bits[ 2u]; } ; struct Cyc_dirent{ int d_ino; int d_off; unsigned short
d_reclen; unsigned char d_type; unsigned char d_name[ 256u]; } ; struct Cyc_Fcntl_flock{
short l_type; int l_start; short l_whence; int l_len; int l_pid; } ; static
const int Cyc_Fcntl_Long= 0; struct Cyc_Fcntl_Long_struct{ int tag; int f1; } ;
static const int Cyc_Fcntl_Flock= 1; struct Cyc_Fcntl_Flock_struct{ int tag;
struct Cyc_Fcntl_flock* f1; } ; extern int Cyc_Fcntl_fcntl( int fd, int cmd,
struct _tagged_arr argv); extern void exit( int); extern void* abort(); struct
Cyc_Core_Opt{ void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u];
struct Cyc_Core_InvalidArg_struct{ unsigned char* tag; struct _tagged_arr f1; }
; extern unsigned char Cyc_Core_Failure[ 12u]; struct Cyc_Core_Failure_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Impossible[
15u]; struct Cyc_Core_Impossible_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char Cyc_Core_Not_found[ 14u]; extern unsigned char Cyc_Core_Unreachable[
16u]; struct Cyc_Core_Unreachable_struct{ unsigned char* tag; struct _tagged_arr
f1; } ; extern unsigned char* string_to_Cstring( struct _tagged_arr); extern
unsigned char* underlying_Cstring( struct _tagged_arr); extern struct
_tagged_arr Cstring_to_string( unsigned char*); extern struct _tagged_arr
wrap_Cstring_as_string( unsigned char*, int); extern struct _tagged_arr
ntCsl_to_ntsl( unsigned char**); extern int system( unsigned char*); extern int*
__errno(); extern int fcntl( int fd, int cmd); extern int fcntl_with_arg( int fd,
int cmd, int arg); extern int fcntl_with_lock( int fd, int cmd, struct Cyc_Fcntl_flock*
lock); int Cyc_Fcntl_fcntl( int fd, int cmd, struct _tagged_arr argv){ if(
_get_arr_size( argv, sizeof( void*)) == 0){ return fcntl( fd, cmd);} else{ if(
_get_arr_size( argv, sizeof( void*)) != 1){( int) _throw(( void*)({ struct Cyc_Core_Failure_struct*
_temp0=( struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp0[ 0]=({ struct Cyc_Core_Failure_struct _temp1; _temp1.tag= Cyc_Core_Failure;
_temp1.f1= _tag_arr("fcntl: too many args", sizeof( unsigned char), 21u); _temp1;});
_temp0;}));} else{ void* _temp2=*(( void**) _check_unknown_subscript( argv,
sizeof( void*), 0)); int _temp8; struct Cyc_Fcntl_flock* _temp10; _LL4: if(*((
int*) _temp2) == Cyc_Fcntl_Long){ _LL9: _temp8=(( struct Cyc_Fcntl_Long_struct*)
_temp2)->f1; goto _LL5;} else{ goto _LL6;} _LL6: if(*(( int*) _temp2) == Cyc_Fcntl_Flock){
_LL11: _temp10=(( struct Cyc_Fcntl_Flock_struct*) _temp2)->f1; goto _LL7;} else{
goto _LL3;} _LL5: return fcntl_with_arg( fd, cmd, _temp8); _LL7: return
fcntl_with_lock( fd, cmd, _temp10); _LL3:;}}}