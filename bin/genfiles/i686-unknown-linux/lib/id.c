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
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_std___sFILE;
extern unsigned char Cyc_std_FileCloseError[ 19u]; extern unsigned char Cyc_std_FileOpenError[
18u]; struct Cyc_std_FileOpenError_struct{ unsigned char* tag; struct
_tagged_arr f1; } ; static const int Cyc_std_String_pa= 0; struct Cyc_std_String_pa_struct{
int tag; struct _tagged_arr f1; } ; static const int Cyc_std_Int_pa= 1; struct
Cyc_std_Int_pa_struct{ int tag; unsigned int f1; } ; static const int Cyc_std_Double_pa=
2; struct Cyc_std_Double_pa_struct{ int tag; double f1; } ; static const int Cyc_std_ShortPtr_pa=
3; struct Cyc_std_ShortPtr_pa_struct{ int tag; short* f1; } ; static const int
Cyc_std_IntPtr_pa= 4; struct Cyc_std_IntPtr_pa_struct{ int tag; unsigned int* f1;
} ; extern int Cyc_std_fprintf( struct Cyc_std___sFILE*, struct _tagged_arr fmt,
struct _tagged_arr); extern struct _tagged_arr Cyc_std_aprintf( struct
_tagged_arr fmt, struct _tagged_arr); static const int Cyc_std_ShortPtr_sa= 0;
struct Cyc_std_ShortPtr_sa_struct{ int tag; short* f1; } ; static const int Cyc_std_UShortPtr_sa=
1; struct Cyc_std_UShortPtr_sa_struct{ int tag; unsigned short* f1; } ; static
const int Cyc_std_IntPtr_sa= 2; struct Cyc_std_IntPtr_sa_struct{ int tag; int*
f1; } ; static const int Cyc_std_UIntPtr_sa= 3; struct Cyc_std_UIntPtr_sa_struct{
int tag; unsigned int* f1; } ; static const int Cyc_std_StringPtr_sa= 4; struct
Cyc_std_StringPtr_sa_struct{ int tag; struct _tagged_arr f1; } ; static const
int Cyc_std_DoublePtr_sa= 5; struct Cyc_std_DoublePtr_sa_struct{ int tag; double*
f1; } ; static const int Cyc_std_FloatPtr_sa= 6; struct Cyc_std_FloatPtr_sa_struct{
int tag; float* f1; } ; struct Cyc_List_List{ void* hd; struct Cyc_List_List* tl;
} ; extern unsigned char Cyc_List_List_empty[ 15u]; extern unsigned char Cyc_List_List_mismatch[
18u]; extern unsigned char Cyc_List_Nth[ 8u]; extern int Cyc_std_zstrcmp( struct
_tagged_arr, struct _tagged_arr); extern struct _tagged_arr Cyc_std_strconcat_l(
struct Cyc_List_List*); extern struct _tagged_arr Cyc_std_strdup( struct
_tagged_arr src); struct Cyc_Id_id; extern struct Cyc_Id_id* Cyc_Id_id_make(
struct _tagged_arr, int); extern struct Cyc_Id_id* Cyc_Id_id_new( struct
_tagged_arr); extern struct Cyc_Id_id* Cyc_Id_id_renew( struct Cyc_Id_id*);
extern struct Cyc_Id_id* Cyc_Id_id_of_string( struct _tagged_arr); extern struct
Cyc_Id_id* Cyc_Id_id_unique( struct _tagged_arr); extern struct _tagged_arr Cyc_Id_id_to_string(
struct Cyc_Id_id*); extern void Cyc_Id_id_prn( struct Cyc_std___sFILE*, struct
Cyc_Id_id*); extern struct _tagged_arr Cyc_Id_id_to_source( struct Cyc_Id_id*);
extern int Cyc_Id_id_compare( struct Cyc_Id_id*, struct Cyc_Id_id*); struct Cyc_Id_id{
struct _tagged_arr s; int i; } ; struct Cyc_Id_id* Cyc_Id_id_make( struct
_tagged_arr s, int i){ return({ struct Cyc_Id_id* _temp0=( struct Cyc_Id_id*)
GC_malloc( sizeof( struct Cyc_Id_id)); _temp0->s= s; _temp0->i= i; _temp0;});}
static int Cyc_Id_counter= 0; struct Cyc_Id_id* Cyc_Id_id_new( struct
_tagged_arr s){ ++ Cyc_Id_counter; return Cyc_Id_id_make( s, Cyc_Id_counter);}
struct Cyc_Id_id* Cyc_Id_id_renew( struct Cyc_Id_id* i){ ++ Cyc_Id_counter;
return Cyc_Id_id_make( i->s, Cyc_Id_counter);} struct Cyc_Id_id* Cyc_Id_id_of_string(
struct _tagged_arr s){ return Cyc_Id_id_make( s, - 1);} struct Cyc_Id_id* Cyc_Id_id_unique(
struct _tagged_arr s){ return Cyc_Id_id_new( s);} struct _tagged_arr Cyc_Id_id_to_string(
struct Cyc_Id_id* x){ if( x->i ==  - 1){ return Cyc_std_strdup( x->s);} return
Cyc_std_strconcat_l(({ struct Cyc_List_List* _temp1=( struct Cyc_List_List*)
GC_malloc( sizeof( struct Cyc_List_List)); _temp1->hd=( void*)({ struct
_tagged_arr* _temp7=( struct _tagged_arr*) GC_malloc( sizeof( struct _tagged_arr)
*  1); _temp7[ 0]= x->s; _temp7;}); _temp1->tl=({ struct Cyc_List_List* _temp2=(
struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List)); _temp2->hd=(
void*) _init_tag_arr(( struct _tagged_arr*) GC_malloc( sizeof( struct
_tagged_arr)),"$", sizeof( unsigned char), 2u); _temp2->tl=({ struct Cyc_List_List*
_temp3=( struct Cyc_List_List*) GC_malloc( sizeof( struct Cyc_List_List));
_temp3->hd=( void*)({ struct _tagged_arr* _temp4=( struct _tagged_arr*)
GC_malloc( sizeof( struct _tagged_arr) *  1); _temp4[ 0]=( struct _tagged_arr)({
struct Cyc_std_Int_pa_struct _temp6; _temp6.tag= Cyc_std_Int_pa; _temp6.f1=( int)((
unsigned int) x->i);{ void* _temp5[ 1u]={& _temp6}; Cyc_std_aprintf( _tag_arr("%d",
sizeof( unsigned char), 3u), _tag_arr( _temp5, sizeof( void*), 1u));}}); _temp4;});
_temp3->tl= 0; _temp3;}); _temp2;}); _temp1;}));} void Cyc_Id_id_prn( struct Cyc_std___sFILE*
f, struct Cyc_Id_id* x){({ struct Cyc_std_String_pa_struct _temp9; _temp9.tag=
Cyc_std_String_pa; _temp9.f1=( struct _tagged_arr) Cyc_Id_id_to_string( x);{
void* _temp8[ 1u]={& _temp9}; Cyc_std_fprintf( f, _tag_arr("%s", sizeof(
unsigned char), 3u), _tag_arr( _temp8, sizeof( void*), 1u));}});} struct
_tagged_arr Cyc_Id_id_to_source( struct Cyc_Id_id* x){ return Cyc_std_strdup( x->s);}
int Cyc_Id_id_compare( struct Cyc_Id_id* x1, struct Cyc_Id_id* x2){ if( x1->i == 
x2->i){ return Cyc_std_zstrcmp( x1->s, x2->s);} else{ if( x1->i <  x2->i){
return - 1;} else{ return 1;}}}
