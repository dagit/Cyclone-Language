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
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern void* Cyc_std___assert_fail( struct _tagged_arr
assertion, struct _tagged_arr file, unsigned int line); struct Cyc_std___sFILE;
extern struct Cyc_std___sFILE* Cyc_std_stderr; extern int Cyc_std_fflush( struct
Cyc_std___sFILE*); extern unsigned char Cyc_std_FileCloseError[ 19u]; extern
unsigned char Cyc_std_FileOpenError[ 18u]; struct Cyc_std_FileOpenError_struct{
unsigned char* tag; struct _tagged_arr f1; } ; static const int Cyc_std_String_pa=
0; struct Cyc_std_String_pa_struct{ int tag; struct _tagged_arr f1; } ; static
const int Cyc_std_Int_pa= 1; struct Cyc_std_Int_pa_struct{ int tag; unsigned int
f1; } ; static const int Cyc_std_Double_pa= 2; struct Cyc_std_Double_pa_struct{
int tag; double f1; } ; static const int Cyc_std_ShortPtr_pa= 3; struct Cyc_std_ShortPtr_pa_struct{
int tag; short* f1; } ; static const int Cyc_std_IntPtr_pa= 4; struct Cyc_std_IntPtr_pa_struct{
int tag; unsigned int* f1; } ; extern int Cyc_std_fprintf( struct Cyc_std___sFILE*,
struct _tagged_arr fmt, struct _tagged_arr); static const int Cyc_std_ShortPtr_sa=
0; struct Cyc_std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int
Cyc_std_UShortPtr_sa= 1; struct Cyc_std_UShortPtr_sa_struct{ int tag;
unsigned short* f1; } ; static const int Cyc_std_IntPtr_sa= 2; struct Cyc_std_IntPtr_sa_struct{
int tag; int* f1; } ; static const int Cyc_std_UIntPtr_sa= 3; struct Cyc_std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_std_StringPtr_sa= 4; struct
Cyc_std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_std_DoublePtr_sa= 5; struct Cyc_std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_std_FloatPtr_sa= 6; struct Cyc_std_FloatPtr_sa_struct{
int tag; float* f1; } ; void* Cyc_std___assert_fail( struct _tagged_arr
assertion, struct _tagged_arr file, unsigned int line){({ struct Cyc_std_String_pa_struct
_temp3; _temp3.tag= Cyc_std_String_pa; _temp3.f1=( struct _tagged_arr) assertion;{
struct Cyc_std_Int_pa_struct _temp2; _temp2.tag= Cyc_std_Int_pa; _temp2.f1= line;{
struct Cyc_std_String_pa_struct _temp1; _temp1.tag= Cyc_std_String_pa; _temp1.f1=(
struct _tagged_arr) file;{ void* _temp0[ 3u]={& _temp1,& _temp2,& _temp3}; Cyc_std_fprintf(
Cyc_std_stderr, _tag_arr("%s:%u: Assertion `%s' failed.\n", sizeof(
unsigned char), 31u), _tag_arr( _temp0, sizeof( void*), 3u));}}}}); Cyc_std_fflush((
struct Cyc_std___sFILE*) Cyc_std_stderr); exit( - 1);( int) _throw(( void*)({
struct Cyc_Core_Unreachable_struct* _temp4=( struct Cyc_Core_Unreachable_struct*)
GC_malloc( sizeof( struct Cyc_Core_Unreachable_struct)); _temp4[ 0]=({ struct
Cyc_Core_Unreachable_struct _temp5; _temp5.tag= Cyc_Core_Unreachable; _temp5.f1=
_tag_arr("assert", sizeof( unsigned char), 7u); _temp5;}); _temp4;}));}
