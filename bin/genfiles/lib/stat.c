 struct Cyc_timespec{ unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{
struct Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; extern void
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
tm_yday; int tm_isdst; } ; extern unsigned int time( unsigned int* t); struct
Cyc_Stat_stat_t{ short st_dev; unsigned int st_ino; int st_mode; unsigned short
st_nlink; unsigned short st_uid; unsigned short st_gid; short st_rdev; int
st_size; unsigned int st_blksize; unsigned int st_blocks; unsigned int st_atime;
unsigned int st_mtime; unsigned int st_ctime; } ; extern int Cyc_Stat_open(
struct _tagged_arr pathname, int flags); extern int Cyc_Stat_stat( struct
_tagged_arr filename, struct Cyc_Stat_stat_t* buf); extern int fstat( int fd,
struct Cyc_Stat_stat_t* buf); extern int Cyc_Stat_lstat( struct _tagged_arr
filename, struct Cyc_Stat_stat_t* buf); extern int umask( int mask); extern int
open( unsigned char* pathname, int flags); extern int stat( unsigned char*
filename, struct Cyc_Stat_stat_t* buf); extern int fstat( int fd, struct Cyc_Stat_stat_t*
buf); extern int lstat( unsigned char* filename, struct Cyc_Stat_stat_t* buf);
int Cyc_Stat_open( struct _tagged_arr pathname, int flags){ return open(
string_to_Cstring( pathname), flags);} int Cyc_Stat_stat( struct _tagged_arr
filename, struct Cyc_Stat_stat_t* buf){ return stat( string_to_Cstring( filename),
buf);} int Cyc_Stat_lstat( struct _tagged_arr filename, struct Cyc_Stat_stat_t*
buf){ return lstat( string_to_Cstring( filename), buf);}