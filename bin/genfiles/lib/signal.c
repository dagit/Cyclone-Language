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
unsigned char*); extern int* __errno(); void(* Cyc_Signal_signal( int sig, void(*
func)( int)))( int); extern int raise( int sig); struct Cyc_Csignal_Csig_func_ptr;
extern struct Cyc_Csignal_Csig_func_ptr* signal( int sig, struct Cyc_Csignal_Csig_func_ptr*
func); extern struct Cyc_Csignal_Csig_func_ptr* signal_toC( void(*)( int));
extern void(* signal_fromC( struct Cyc_Csignal_Csig_func_ptr* func))( int); void
Cyc_Signal__SIG_DFL( int n){;} void Cyc_Signal__SIG_IGN( int n){;} void Cyc_Signal__SIG_ERR(
int n){;} void(* Cyc_Signal_signal( int sig, void(* func)( int)))( int){ return
signal_fromC( signal( sig, signal_toC( func)));}