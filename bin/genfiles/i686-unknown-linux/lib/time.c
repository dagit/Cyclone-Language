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
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_std_tm{
int tm_sec; int tm_min; int tm_hour; int tm_mday; int tm_mon; int tm_year; int
tm_wday; int tm_yday; int tm_isdst; } ; extern int time( int* t); extern struct
_tagged_arr Cyc_std_asctime( const struct Cyc_std_tm* timeptr); extern struct
_tagged_arr Cyc_std_ctime( const int* timep); extern struct Cyc_std_tm* gmtime(
const int* timep); extern struct Cyc_std_tm* localtime( const int* timep);
extern unsigned int Cyc_std_strftime( struct _tagged_arr s, unsigned int maxsize,
struct _tagged_arr fmt, const struct Cyc_std_tm* t); extern struct _tagged_arr
Cyc_std_asctime_r( const struct Cyc_std_tm*, struct _tagged_arr); extern struct
_tagged_arr Cyc_std_ctime_r( const int*, struct _tagged_arr); extern int
timezone; extern int daylight; extern unsigned char* asctime( const struct Cyc_std_tm*
timeptr); extern unsigned char* ctime( const int* timep); extern unsigned int
strftime( unsigned char* s, unsigned int maxsize, unsigned char* fmt, const
struct Cyc_std_tm* t); extern unsigned char* asctime_r( const struct Cyc_std_tm*,
unsigned char*); extern unsigned char* ctime_r( const int*, unsigned char*);
struct _tagged_arr Cyc_std_asctime( const struct Cyc_std_tm* timeptr){ return
Cstring_to_string( asctime( timeptr));} struct _tagged_arr Cyc_std_ctime( const
int* timep){ return Cstring_to_string( ctime( timep));} unsigned int Cyc_std_strftime(
struct _tagged_arr s, unsigned int maxsize, struct _tagged_arr fmt, const struct
Cyc_std_tm* t){ unsigned int m= _get_arr_size( s, sizeof( unsigned char)) < 
maxsize? _get_arr_size( s, sizeof( unsigned char)): maxsize; return strftime(
underlying_Cstring( s), m, underlying_Cstring( fmt), t);} struct _tagged_arr Cyc_std_asctime_r(
const struct Cyc_std_tm* t, struct _tagged_arr s){ if( _get_arr_size( s, sizeof(
unsigned char)) <  50){( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp0=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp0[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp1; _temp1.tag= Cyc_Core_InvalidArg;
_temp1.f1= _tag_arr("Time::asctime_r: string too small (< 50)", sizeof(
unsigned char), 41u); _temp1;}); _temp0;}));} return Cstring_to_string(
asctime_r( t, underlying_Cstring(( struct _tagged_arr) s)));} struct _tagged_arr
Cyc_std_ctime_r( const int* t, struct _tagged_arr s){ if( _get_arr_size( s,
sizeof( unsigned char)) <  50){( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct*
_temp2=( struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp2[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp3; _temp3.tag= Cyc_Core_InvalidArg;
_temp3.f1= _tag_arr("Time::ctime_r: string too small (< 50)", sizeof(
unsigned char), 39u); _temp3;}); _temp2;}));} return Cstring_to_string( ctime_r(
t, underlying_Cstring(( struct _tagged_arr) s)));}
