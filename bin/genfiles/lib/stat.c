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
unsigned char*); extern int* __errno(); struct Cyc_Time_tm{ int tm_sec; int
tm_min; int tm_hour; int tm_mday; int tm_mon; int tm_year; int tm_wday; int
tm_yday; int tm_isdst; } ; extern unsigned int time( unsigned int* t); extern
struct Cyc_Time_tm* gmtime( const unsigned int* timep); extern struct Cyc_Time_tm*
localtime( const unsigned int* timep); extern int timezone; extern int daylight;
struct Cyc_Stat_stat_t{ short st_dev; unsigned int st_ino; unsigned int st_mode;
unsigned short st_nlink; unsigned short st_uid; unsigned short st_gid; short
st_rdev; int st_size; unsigned int st_blksize; unsigned int st_blocks;
unsigned int st_atime; unsigned int st_mtime; unsigned int st_ctime; } ; extern
int Cyc_Stat_open( struct _tagged_arr pathname, int flags, struct _tagged_arr
mode); extern int Cyc_Stat_stat( struct _tagged_arr filename, struct Cyc_Stat_stat_t*
buf); extern int fstat( int fd, struct Cyc_Stat_stat_t* buf); extern int Cyc_Stat_lstat(
struct _tagged_arr filename, struct Cyc_Stat_stat_t* buf); extern unsigned int
umask( unsigned int mask); extern int open( unsigned char* pathname, int flags,
unsigned int mode); extern int stat( unsigned char* filename, struct Cyc_Stat_stat_t*
buf); extern int fstat( int fd, struct Cyc_Stat_stat_t* buf); extern int lstat(
unsigned char* filename, struct Cyc_Stat_stat_t* buf); int Cyc_Stat_open( struct
_tagged_arr pathname, int flags, struct _tagged_arr mode){ unsigned int my_mode;
if( _get_arr_size( mode, sizeof( unsigned int)) ==  0){ my_mode= - 1;} else{ if(
_get_arr_size( mode, sizeof( unsigned int)) ==  1){ my_mode=*(( unsigned int*)
_check_unknown_subscript( mode, sizeof( unsigned int), 0));} else{( int) _throw((
void*)({ struct Cyc_Core_Failure_struct* _temp0=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp0[ 0]=({ struct Cyc_Core_Failure_struct
_temp1; _temp1.tag= Cyc_Core_Failure; _temp1.f1= _tag_arr("open: too many args",
sizeof( unsigned char), 20u); _temp1;}); _temp0;}));}} return open(
string_to_Cstring( pathname), flags, my_mode);} int Cyc_Stat_stat( struct
_tagged_arr filename, struct Cyc_Stat_stat_t* buf){ return stat(
string_to_Cstring( filename), buf);} int Cyc_Stat_lstat( struct _tagged_arr
filename, struct Cyc_Stat_stat_t* buf){ return lstat( string_to_Cstring(
filename), buf);}
