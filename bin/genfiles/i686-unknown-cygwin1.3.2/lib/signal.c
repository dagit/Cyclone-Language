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
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); void(* Cyc_std_signal(
int sig, void(* func)( int)))( int); extern int raise( int sig); extern void Cyc_std__SIG_DFL(
int); extern void Cyc_std__SIG_IGN( int); extern void Cyc_std__SIG_ERR( int);
extern int kill( int, int); extern void(* signal_func( int sig, void(* func)(
int)))( int); void Cyc_std__SIG_DFL( int n){;} void Cyc_std__SIG_IGN( int n){;}
void Cyc_std__SIG_ERR( int n){;} void(* Cyc_std_signal( int sig, void(* func)(
int)))( int){ return signal_func( sig, func);}
