 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 8u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_string); extern unsigned char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( unsigned char*); struct _tagged_ptr0{ struct _tagged_string*
curr; struct _tagged_string* base; struct _tagged_string* last_plus_one; } ;
extern struct _tagged_ptr0 ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern int* __errno(); extern void* Cyc_Assert_AssertFail(
struct _tagged_string msg); struct Cyc_Stdio___sFILE; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stderr; extern int Cyc_Stdio_fputs( struct _tagged_string, struct Cyc_Stdio___sFILE*);
extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_string f1; } ; void* Cyc_Assert_AssertFail( struct _tagged_string mesg){
Cyc_Stdio_fputs( mesg, Cyc_Stdio_stderr); Cyc_Stdio_fputs(({ unsigned char*
_temp0=( unsigned char*)" -- assertion failed\n"; struct _tagged_string _temp1;
_temp1.curr= _temp0; _temp1.base= _temp0; _temp1.last_plus_one= _temp0 + 22;
_temp1;}), Cyc_Stdio_stderr); exit( - 1);( void) _throw(( void*)({ struct Cyc_Core_Unreachable_struct*
_temp2=( struct Cyc_Core_Unreachable_struct*) GC_malloc( sizeof( struct Cyc_Core_Unreachable_struct));
_temp2[ 0]=({ struct Cyc_Core_Unreachable_struct _temp3; _temp3.tag= Cyc_Core_Unreachable;
_temp3.f1=({ unsigned char* _temp4=( unsigned char*)"assert"; struct
_tagged_string _temp5; _temp5.curr= _temp4; _temp5.base= _temp4; _temp5.last_plus_one=
_temp4 + 7; _temp5;}); _temp3;}); _temp2;}));}