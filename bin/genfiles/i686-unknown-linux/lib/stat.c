 extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{ void* v; } ;
extern unsigned char Cyc_Core_Invalid_argument[ 21u]; struct Cyc_Core_Invalid_argument_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr wrap_Cstring_as_string( unsigned char*, unsigned int);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_Std_tm{
int tm_sec; int tm_min; int tm_hour; int tm_mday; int tm_mon; int tm_year; int
tm_wday; int tm_yday; int tm_isdst; } ; extern int time( int* t); extern struct
Cyc_Std_tm* gmtime( const int* timep); extern struct Cyc_Std_tm* localtime(
const int* timep); extern int timezone; extern int daylight; struct Cyc_Std_stat_t{
unsigned long long st_dev; unsigned int st_ino; unsigned int st_mode;
unsigned int st_nlink; unsigned int st_uid; unsigned int st_gid;
unsigned long long st_rdev; int st_size; unsigned int st_blksize; unsigned int
st_blocks; int st_atime; int st_mtime; int st_ctime; } ; extern int Cyc_Std_open(
struct _tagged_arr pathname, int flags, struct _tagged_arr mode); extern int Cyc_Std_stat(
struct _tagged_arr filename, struct Cyc_Std_stat_t* buf); extern int fstat( int
fd, struct Cyc_Std_stat_t* buf); extern int Cyc_Std_lstat( struct _tagged_arr
filename, struct Cyc_Std_stat_t* buf); extern unsigned int umask( unsigned int
mask); extern int open( unsigned char* pathname, int flags, unsigned int mode);
extern int stat( unsigned char* filename, struct Cyc_Std_stat_t* buf); extern
int fstat( int fd, struct Cyc_Std_stat_t* buf); extern int lstat( unsigned char*
filename, struct Cyc_Std_stat_t* buf); int Cyc_Std_open( struct _tagged_arr
pathname, int flags, struct _tagged_arr mode){ unsigned int my_mode; if(
_get_arr_size( mode, sizeof( unsigned int)) ==  0){ my_mode= - 1;} else{ if(
_get_arr_size( mode, sizeof( unsigned int)) ==  1){ my_mode=*(( unsigned int*)
_check_unknown_subscript( mode, sizeof( unsigned int), 0));} else{( int) _throw((
void*)({ struct Cyc_Core_Failure_struct* _temp0=( struct Cyc_Core_Failure_struct*)
_cycalloc( sizeof( struct Cyc_Core_Failure_struct)); _temp0[ 0]=({ struct Cyc_Core_Failure_struct
_temp1; _temp1.tag= Cyc_Core_Failure; _temp1.f1= _tag_arr("open: too many args",
sizeof( unsigned char), 20u); _temp1;}); _temp0;}));}} return open(
string_to_Cstring( pathname), flags, my_mode);} int Cyc_Std_stat( struct
_tagged_arr filename, struct Cyc_Std_stat_t* buf){ return stat(
string_to_Cstring( filename), buf);} int Cyc_Std_lstat( struct _tagged_arr
filename, struct Cyc_Std_stat_t* buf){ return lstat( string_to_Cstring( filename),
buf);}
