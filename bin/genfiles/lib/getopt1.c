 struct Cyc_Getopt_option{ struct _tagged_arr name; int has_arg; int* flag; int
val; } ; extern int Cyc_Getopt_getopt_long( int argc, struct _tagged_arr argv,
struct _tagged_arr shortopts, struct _tagged_arr longopts, int* longind); extern
int Cyc_Getopt_getopt_long_only( int argc, struct _tagged_arr argv, struct
_tagged_arr shortopts, struct _tagged_arr longopts, int* longind); extern int
Cyc_Getopt__getopt_internal( int argc, struct _tagged_arr argv, struct
_tagged_arr shortopts, struct _tagged_arr longopts, int* longind, int long_only);
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
unsigned char*); extern int* __errno(); struct Cyc_Stdio___sFILE; extern
unsigned char Cyc_Stdio_FileCloseError[ 19u]; extern unsigned char Cyc_Stdio_FileOpenError[
18u]; struct Cyc_Stdio_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_Stdio_String_pa= 0; struct Cyc_Stdio_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_Stdio_Int_pa= 1; struct
Cyc_Stdio_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_Stdio_Double_pa=
2; struct Cyc_Stdio_Double_pa_struct{ int tag; double f1; } ; static const int
Cyc_Stdio_ShortPtr_pa= 3; struct Cyc_Stdio_ShortPtr_pa_struct{ int tag; short*
f1; } ; static const int Cyc_Stdio_IntPtr_pa= 4; struct Cyc_Stdio_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_ShortPtr_sa= 0; struct
Cyc_Stdio_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int Cyc_Stdio_UShortPtr_sa=
1; struct Cyc_Stdio_UShortPtr_sa_struct{ int tag; unsigned short* f1; } ; static
const int Cyc_Stdio_IntPtr_sa= 2; struct Cyc_Stdio_IntPtr_sa_struct{ int tag;
int* f1; } ; static const int Cyc_Stdio_UIntPtr_sa= 3; struct Cyc_Stdio_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_Stdio_StringPtr_sa= 4;
struct Cyc_Stdio_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_Stdio_DoublePtr_sa= 5; struct Cyc_Stdio_DoublePtr_sa_struct{ int
tag; double* f1; } ; static const int Cyc_Stdio_FloatPtr_sa= 6; struct Cyc_Stdio_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_Getopt_Stdlib__Div{ int quot; int rem; } ;
struct Cyc_Getopt_Stdlib__Ldiv{ int quot; int rem; } ; extern int abs( int __x);
extern int atexit( void(* __func)()); extern struct Cyc_Getopt_Stdlib__Div div(
int __numer, int __denom); extern struct Cyc_Getopt_Stdlib__Ldiv ldiv( int
__numer, int __denom); extern int random(); extern void srandom( unsigned int
__seed); extern int rand(); extern void srand( unsigned int __seed); extern int
rand_r( unsigned int* __seed); extern int grantpt( int __fd); extern int
unlockpt( int __fd); int Cyc_Getopt_getopt_long( int argc, struct _tagged_arr
argv, struct _tagged_arr options, struct _tagged_arr long_options, int*
opt_index){ return Cyc_Getopt__getopt_internal( argc, argv, options,
long_options, opt_index, 0);} int Cyc_Getopt_getopt_long_only( int argc, struct
_tagged_arr argv, struct _tagged_arr options, struct _tagged_arr long_options,
int* opt_index){ return Cyc_Getopt__getopt_internal( argc, argv, options,
long_options, opt_index, 1);}
