 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 2u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_arr f1;
} ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_arr f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_arr); extern unsigned char* underlying_Cstring(
struct _tagged_arr); extern struct _tagged_arr Cstring_to_string( unsigned char*);
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern int* __errno(); extern void* Cyc_Assert_AssertFail(
struct _tagged_arr msg); struct Cyc_Stdio___sFILE; extern struct Cyc_Stdio___sFILE*
Cyc_Stdio_stderr; extern int Cyc_Stdio_fputs( struct _tagged_arr, struct Cyc_Stdio___sFILE*);
extern unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; void* Cyc_Assert_AssertFail( struct _tagged_arr mesg){ Cyc_Stdio_fputs(
mesg, Cyc_Stdio_stderr); Cyc_Stdio_fputs( _tag_arr(" -- assertion failed\n",
sizeof( unsigned char), 22u), Cyc_Stdio_stderr); exit( - 1);( int) _throw(( void*)({
struct Cyc_Core_Unreachable_struct* _temp0=( struct Cyc_Core_Unreachable_struct*)
GC_malloc( sizeof( struct Cyc_Core_Unreachable_struct)); _temp0[ 0]=({ struct
Cyc_Core_Unreachable_struct _temp1; _temp1.tag= Cyc_Core_Unreachable; _temp1.f1=
_tag_arr("assert", sizeof( unsigned char), 7u); _temp1;}); _temp0;}));}