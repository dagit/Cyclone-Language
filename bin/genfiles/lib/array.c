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
unsigned char*); extern int* __errno(); struct Cyc_List_List{ void* hd; struct
Cyc_List_List* tl; } ; extern unsigned char Cyc_List_List_empty[ 15u]; extern
unsigned char Cyc_List_List_mismatch[ 18u]; extern unsigned char Cyc_List_Nth[ 8u];
extern struct _tagged_arr Cyc_List_to_array( struct Cyc_List_List* x); extern
struct Cyc_List_List* Cyc_List_from_array( struct _tagged_arr arr); extern void
Cyc_Array_qsort( int(* less_eq)( void*, void*), struct _tagged_arr x, int ofs,
int len); extern void Cyc_Array_msort( int(* less_eq)( void*, void*), struct
_tagged_arr x, int ofs, int len); extern struct _tagged_arr Cyc_Array_from_list(
struct Cyc_List_List* x); extern struct Cyc_List_List* Cyc_Array_to_list( struct
_tagged_arr x); extern struct _tagged_arr Cyc_Array_copy( struct _tagged_arr x);
extern struct _tagged_arr Cyc_Array_map( void*(* f)( void*), struct _tagged_arr
x); extern struct _tagged_arr Cyc_Array_map_c( void*(* f)( void*, void*), void*
env, struct _tagged_arr x); extern void Cyc_Array_imp_map( void*(* f)( void*),
struct _tagged_arr x); extern void Cyc_Array_imp_map_c( void*(* f)( void*, void*),
void* env, struct _tagged_arr x); extern unsigned char Cyc_Array_Array_mismatch[
19u]; extern struct _tagged_arr Cyc_Array_map2( void*(* f)( void*, void*),
struct _tagged_arr x, struct _tagged_arr y); extern void Cyc_Array_app( void*(*
f)( void*), struct _tagged_arr x); extern void Cyc_Array_app_c( void*(* f)( void*,
void*), void* env, struct _tagged_arr x); extern void Cyc_Array_iter( void(* f)(
void*), struct _tagged_arr x); extern void Cyc_Array_iter_c( void(* f)( void*,
void*), void* env, struct _tagged_arr x); extern void Cyc_Array_app2( void*(* f)(
void*, void*), struct _tagged_arr x, struct _tagged_arr y); extern void Cyc_Array_app2_c(
void*(* f)( void*, void*, void*), void* env, struct _tagged_arr x, struct
_tagged_arr y); extern void Cyc_Array_iter2( void(* f)( void*, void*), struct
_tagged_arr x, struct _tagged_arr y); extern void Cyc_Array_iter2_c( void(* f)(
void*, void*, void*), void* env, struct _tagged_arr x, struct _tagged_arr y);
extern void* Cyc_Array_fold_left( void*(* f)( void*, void*), void* accum, struct
_tagged_arr x); extern void* Cyc_Array_fold_left_c( void*(* f)( void*, void*,
void*), void* env, void* accum, struct _tagged_arr x); extern void* Cyc_Array_fold_right(
void*(* f)( void*, void*), struct _tagged_arr x, void* accum); extern void* Cyc_Array_fold_right_c(
void*(* f)( void*, void*, void*), void* env, struct _tagged_arr x, void* accum);
extern struct _tagged_arr Cyc_Array_rev_copy( struct _tagged_arr x); extern void
Cyc_Array_imp_rev( struct _tagged_arr x); extern int Cyc_Array_forall( int(*
pred)( void*), struct _tagged_arr x); extern int Cyc_Array_forall_c( int(* pred)(
void*, void*), void* env, struct _tagged_arr x); extern int Cyc_Array_exists(
int(* pred)( void*), struct _tagged_arr x); extern int Cyc_Array_exists_c( int(*
pred)( void*, void*), void* env, struct _tagged_arr x); struct _tuple0{ void* f1;
void* f2; } ; extern struct _tagged_arr Cyc_Array_zip( struct _tagged_arr x,
struct _tagged_arr y); struct _tuple1{ struct _tagged_arr f1; struct _tagged_arr
f2; } ; extern struct _tuple1 Cyc_Array_split( struct _tagged_arr x); extern int
Cyc_Array_memq( struct _tagged_arr l, void* x); extern int Cyc_Array_mem( int(*
compare)( void*, void*), struct _tagged_arr l, void* x); extern struct
_tagged_arr Cyc_Array_extract( struct _tagged_arr x, int start, int* n_opt);
static int Cyc_Array_sp[ 40u]={ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; void
Cyc_Array_qsort( int(* less_eq)( void*, void*), struct _tagged_arr arr, int
base_ofs, int len){ void* temp; int sp_ofs; int i; int j; int limit_ofs; if((
base_ofs <  0? 1: base_ofs +  len >  _get_arr_size( arr, sizeof( void*)))? 1:
len <  0){( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp0=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp0[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp1; _temp1.tag= Cyc_Core_InvalidArg;
_temp1.f1= _tag_arr("Array::qsort", sizeof( unsigned char), 13u); _temp1;});
_temp0;}));} limit_ofs= base_ofs +  len; sp_ofs= 0; for( 0; 1; 0){ if( limit_ofs
-  base_ofs >  3){ temp=*(( void**) _check_unknown_subscript( arr, sizeof( void*),(
limit_ofs -  base_ofs) /  2 +  base_ofs));*(( void**) _check_unknown_subscript(
arr, sizeof( void*),( limit_ofs -  base_ofs) /  2 +  base_ofs))=*(( void**)
_check_unknown_subscript( arr, sizeof( void*), base_ofs));*(( void**)
_check_unknown_subscript( arr, sizeof( void*), base_ofs))= temp; i= base_ofs + 
1; j= limit_ofs -  1; if( less_eq(*(( void**) _check_unknown_subscript( arr,
sizeof( void*), i)),*(( void**) _check_unknown_subscript( arr, sizeof( void*), j)))
>  0){ temp=*(( void**) _check_unknown_subscript( arr, sizeof( void*), i));*((
void**) _check_unknown_subscript( arr, sizeof( void*), i))=*(( void**)
_check_unknown_subscript( arr, sizeof( void*), j));*(( void**)
_check_unknown_subscript( arr, sizeof( void*), j))= temp;} if( less_eq(*(( void**)
_check_unknown_subscript( arr, sizeof( void*), base_ofs)),*(( void**)
_check_unknown_subscript( arr, sizeof( void*), j))) >  0){ temp=*(( void**)
_check_unknown_subscript( arr, sizeof( void*), base_ofs));*(( void**)
_check_unknown_subscript( arr, sizeof( void*), base_ofs))=*(( void**)
_check_unknown_subscript( arr, sizeof( void*), j));*(( void**)
_check_unknown_subscript( arr, sizeof( void*), j))= temp;} if( less_eq(*(( void**)
_check_unknown_subscript( arr, sizeof( void*), i)),*(( void**)
_check_unknown_subscript( arr, sizeof( void*), base_ofs))) >  0){ temp=*(( void**)
_check_unknown_subscript( arr, sizeof( void*), i));*(( void**)
_check_unknown_subscript( arr, sizeof( void*), i))=*(( void**)
_check_unknown_subscript( arr, sizeof( void*), base_ofs));*(( void**)
_check_unknown_subscript( arr, sizeof( void*), base_ofs))= temp;} for( 0; 1; 0){
do { i ++;} while ( less_eq(*(( void**) _check_unknown_subscript( arr, sizeof(
void*), i)),*(( void**) _check_unknown_subscript( arr, sizeof( void*), base_ofs)))
<  0); do { j --;} while ( less_eq(*(( void**) _check_unknown_subscript( arr,
sizeof( void*), j)),*(( void**) _check_unknown_subscript( arr, sizeof( void*),
base_ofs))) >  0); if( i >  j){ break;} temp=*(( void**)
_check_unknown_subscript( arr, sizeof( void*), i));*(( void**)
_check_unknown_subscript( arr, sizeof( void*), i))=*(( void**)
_check_unknown_subscript( arr, sizeof( void*), j));*(( void**)
_check_unknown_subscript( arr, sizeof( void*), j))= temp;;} temp=*(( void**)
_check_unknown_subscript( arr, sizeof( void*), base_ofs));*(( void**)
_check_unknown_subscript( arr, sizeof( void*), base_ofs))=*(( void**)
_check_unknown_subscript( arr, sizeof( void*), j));*(( void**)
_check_unknown_subscript( arr, sizeof( void*), j))= temp; if( j -  base_ofs > 
limit_ofs -  i){ Cyc_Array_sp[ _check_known_subscript_notnull( 40u, sp_ofs)]=
base_ofs; Cyc_Array_sp[ _check_known_subscript_notnull( 40u, sp_ofs +  1)]= j;
base_ofs= i;} else{ Cyc_Array_sp[ _check_known_subscript_notnull( 40u, sp_ofs)]=
i; Cyc_Array_sp[ _check_known_subscript_notnull( 40u, sp_ofs +  1)]= limit_ofs;
limit_ofs= j;} sp_ofs += 2;} else{ for(( j= base_ofs, i= j +  1); i <  limit_ofs;(
j= i, i ++)){ for( 0; less_eq(*(( void**) _check_unknown_subscript( arr, sizeof(
void*), j)),*(( void**) _check_unknown_subscript( arr, sizeof( void*), j +  1)))
>  0; j --){ temp=*(( void**) _check_unknown_subscript( arr, sizeof( void*), j));*((
void**) _check_unknown_subscript( arr, sizeof( void*), j))=*(( void**)
_check_unknown_subscript( arr, sizeof( void*), j +  1));*(( void**)
_check_unknown_subscript( arr, sizeof( void*), j +  1))= temp; if( j == 
base_ofs){ break;}}} if( sp_ofs !=  0){ sp_ofs -= 2; base_ofs= Cyc_Array_sp[
_check_known_subscript_notnull( 40u, sp_ofs)]; limit_ofs= Cyc_Array_sp[
_check_known_subscript_notnull( 40u, sp_ofs +  1)];} else{ break;}}}} void Cyc_Array_msort(
int(* less_eq)( void*, void*), struct _tagged_arr arr, int base_ofs, int len){
if(( base_ofs <  0? 1: base_ofs +  len >  _get_arr_size( arr, sizeof( void*)))?
1: len <  0){( int) _throw(( void*)({ struct Cyc_Core_InvalidArg_struct* _temp2=(
struct Cyc_Core_InvalidArg_struct*) GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct));
_temp2[ 0]=({ struct Cyc_Core_InvalidArg_struct _temp3; _temp3.tag= Cyc_Core_InvalidArg;
_temp3.f1= _tag_arr("Array::msort", sizeof( unsigned char), 13u); _temp3;});
_temp2;}));}{ struct _tagged_arr from=({ unsigned int _temp4=( unsigned int) len;
void** _temp5=( void**) GC_malloc( sizeof( void*) *  _temp4); struct _tagged_arr
_temp7= _tag_arr( _temp5, sizeof( void*),( unsigned int) len);{ unsigned int
_temp6= _temp4; unsigned int i; for( i= 0; i <  _temp6; i ++){ _temp5[ i]=*((
void**) _check_unknown_subscript( arr, sizeof( void*), 0));}}; _temp7;}); struct
_tagged_arr to= _tagged_arr_plus( arr, sizeof( void*), base_ofs); struct
_tagged_arr swap; int swapped= 0; int stepsize; int start; int lstart; int lend;
int rstart; int rend; int dest; for( stepsize= 1; stepsize <  len; stepsize=
stepsize *  2){ swap= from; from= to; to= swap; dest= 0; for( start= 0; start < 
len; start= start +  stepsize *  2){ lstart= start; rstart= start +  stepsize < 
len? start +  stepsize: len; lend= rstart; rend= start +  stepsize *  2 <  len?
start +  stepsize *  2: len; while( lstart <  lend? rstart <  rend: 0) { if(
less_eq(*(( void**) _check_unknown_subscript( from, sizeof( void*), lstart)),*((
void**) _check_unknown_subscript( from, sizeof( void*), rstart))) <=  0){*((
void**) _check_unknown_subscript( to, sizeof( void*), dest ++))=*(( void**)
_check_unknown_subscript( from, sizeof( void*), lstart ++));} else{*(( void**)
_check_unknown_subscript( to, sizeof( void*), dest ++))=*(( void**)
_check_unknown_subscript( from, sizeof( void*), rstart ++));}} while( lstart < 
lend) {*(( void**) _check_unknown_subscript( to, sizeof( void*), dest ++))=*((
void**) _check_unknown_subscript( from, sizeof( void*), lstart ++));} while(
rstart <  rend) {*(( void**) _check_unknown_subscript( to, sizeof( void*), dest
++))=*(( void**) _check_unknown_subscript( from, sizeof( void*), rstart ++));}}}
if( swapped){ int i= 0; for( 0; i <  len; i ++){*(( void**)
_check_unknown_subscript( from, sizeof( void*), i))=*(( void**)
_check_unknown_subscript( to, sizeof( void*), i));}}}} struct _tagged_arr Cyc_Array_from_list(
struct Cyc_List_List* x){ return Cyc_List_to_array( x);} struct Cyc_List_List*
Cyc_Array_to_list( struct _tagged_arr x){ return Cyc_List_from_array( x);}
struct _tagged_arr Cyc_Array_copy( struct _tagged_arr x){ int sx=( int)
_get_arr_size( x, sizeof( void*)); return({ unsigned int _temp8=( unsigned int)
sx; void** _temp9=( void**) GC_malloc( sizeof( void*) *  _temp8); struct
_tagged_arr _temp11= _tag_arr( _temp9, sizeof( void*),( unsigned int) sx);{
unsigned int _temp10= _temp8; unsigned int i; for( i= 0; i <  _temp10; i ++){
_temp9[ i]=*(( void**) _check_unknown_subscript( x, sizeof( void*),( int) i));}};
_temp11;});} struct _tagged_arr Cyc_Array_map( void*(* f)( void*), struct
_tagged_arr x){ int sx=( int) _get_arr_size( x, sizeof( void*)); return({
unsigned int _temp12=( unsigned int) sx; void** _temp13=( void**) GC_malloc(
sizeof( void*) *  _temp12); struct _tagged_arr _temp15= _tag_arr( _temp13,
sizeof( void*),( unsigned int) sx);{ unsigned int _temp14= _temp12; unsigned int
i; for( i= 0; i <  _temp14; i ++){ _temp13[ i]= f(*(( void**)
_check_unknown_subscript( x, sizeof( void*),( int) i)));}}; _temp15;});} struct
_tagged_arr Cyc_Array_map_c( void*(* f)( void*, void*), void* env, struct
_tagged_arr x){ int sx=( int) _get_arr_size( x, sizeof( void*)); return({
unsigned int _temp16=( unsigned int) sx; void** _temp17=( void**) GC_malloc(
sizeof( void*) *  _temp16); struct _tagged_arr _temp19= _tag_arr( _temp17,
sizeof( void*),( unsigned int) sx);{ unsigned int _temp18= _temp16; unsigned int
i; for( i= 0; i <  _temp18; i ++){ _temp17[ i]= f( env,*(( void**)
_check_unknown_subscript( x, sizeof( void*),( int) i)));}}; _temp19;});} void
Cyc_Array_imp_map( void*(* f)( void*), struct _tagged_arr x){ int sx=( int)
_get_arr_size( x, sizeof( void*)); int i= 0; for( 0; i <  sx; i ++){*(( void**)
_check_unknown_subscript( x, sizeof( void*), i))= f(*(( void**)
_check_unknown_subscript( x, sizeof( void*), i)));}} void Cyc_Array_imp_map_c(
void*(* f)( void*, void*), void* env, struct _tagged_arr x){ int sx=( int)
_get_arr_size( x, sizeof( void*)); int i= 0; for( 0; i <  sx; i ++){*(( void**)
_check_unknown_subscript( x, sizeof( void*), i))= f( env,*(( void**)
_check_unknown_subscript( x, sizeof( void*), i)));}} unsigned char Cyc_Array_Array_mismatch[
19u]="\000\000\000\000Array_mismatch"; struct _tagged_arr Cyc_Array_map2( void*(*
f)( void*, void*), struct _tagged_arr x, struct _tagged_arr y){ int sx=( int)
_get_arr_size( x, sizeof( void*)); if( sx !=  _get_arr_size( y, sizeof( void*))){(
int) _throw(( void*) Cyc_Array_Array_mismatch);} return({ unsigned int _temp20=(
unsigned int) sx; void** _temp21=( void**) GC_malloc( sizeof( void*) *  _temp20);
struct _tagged_arr _temp23= _tag_arr( _temp21, sizeof( void*),( unsigned int) sx);{
unsigned int _temp22= _temp20; unsigned int i; for( i= 0; i <  _temp22; i ++){
_temp21[ i]= f(*(( void**) _check_unknown_subscript( x, sizeof( void*),( int) i)),*((
void**) _check_unknown_subscript( y, sizeof( void*),( int) i)));}}; _temp23;});}
void Cyc_Array_app( void*(* f)( void*), struct _tagged_arr x){ int sx=( int)
_get_arr_size( x, sizeof( void*)); int i= 0; for( 0; i <  sx; i ++){ f(*(( void**)
_check_unknown_subscript( x, sizeof( void*), i)));}} void Cyc_Array_app_c( void*(*
f)( void*, void*), void* env, struct _tagged_arr x){ int sx=( int) _get_arr_size(
x, sizeof( void*)); int i= 0; for( 0; i <  sx; i ++){ f( env,*(( void**)
_check_unknown_subscript( x, sizeof( void*), i)));}} void Cyc_Array_iter( void(*
f)( void*), struct _tagged_arr x){ int sx=( int) _get_arr_size( x, sizeof( void*));
int i= 0; for( 0; i <  sx; i ++){ f(*(( void**) _check_unknown_subscript( x,
sizeof( void*), i)));}} void Cyc_Array_iter_c( void(* f)( void*, void*), void*
env, struct _tagged_arr x){ int sx=( int) _get_arr_size( x, sizeof( void*)); int
i= 0; for( 0; i <  sx; i ++){ f( env,*(( void**) _check_unknown_subscript( x,
sizeof( void*), i)));}} void Cyc_Array_app2( void*(* f)( void*, void*), struct
_tagged_arr x, struct _tagged_arr y){ int sx=( int) _get_arr_size( x, sizeof(
void*)); if( sx !=  _get_arr_size( y, sizeof( void*))){( int) _throw(( void*)
Cyc_Array_Array_mismatch);}{ int i= 0; for( 0; i <  sx; i ++){ f(*(( void**)
_check_unknown_subscript( x, sizeof( void*), i)),*(( void**)
_check_unknown_subscript( y, sizeof( void*), i)));}}} void Cyc_Array_app2_c(
void*(* f)( void*, void*, void*), void* env, struct _tagged_arr x, struct
_tagged_arr y){ int sx=( int) _get_arr_size( x, sizeof( void*)); if( sx != 
_get_arr_size( y, sizeof( void*))){( int) _throw(( void*) Cyc_Array_Array_mismatch);}{
int i= 0; for( 0; i <  sx; i ++){ f( env,*(( void**) _check_unknown_subscript( x,
sizeof( void*), i)),*(( void**) _check_unknown_subscript( y, sizeof( void*), i)));}}}
void Cyc_Array_iter2( void(* f)( void*, void*), struct _tagged_arr x, struct
_tagged_arr y){ int sx=( int) _get_arr_size( x, sizeof( void*)); if( sx != 
_get_arr_size( y, sizeof( void*))){( int) _throw(( void*) Cyc_Array_Array_mismatch);}{
int i= 0; for( 0; i <  sx; i ++){ f(*(( void**) _check_unknown_subscript( x,
sizeof( void*), i)),*(( void**) _check_unknown_subscript( y, sizeof( void*), i)));}}}
void Cyc_Array_iter2_c( void(* f)( void*, void*, void*), void* env, struct
_tagged_arr x, struct _tagged_arr y){ int sx=( int) _get_arr_size( x, sizeof(
void*)); if( sx !=  _get_arr_size( y, sizeof( void*))){( int) _throw(( void*)
Cyc_Array_Array_mismatch);}{ int i= 0; for( 0; i <  sx; i ++){ f( env,*(( void**)
_check_unknown_subscript( x, sizeof( void*), i)),*(( void**)
_check_unknown_subscript( y, sizeof( void*), i)));}}} void* Cyc_Array_fold_left(
void*(* f)( void*, void*), void* accum, struct _tagged_arr x){ int sx=( int)
_get_arr_size( x, sizeof( void*));{ int i= 0; for( 0; i <  sx; i ++){ accum= f(
accum,*(( void**) _check_unknown_subscript( x, sizeof( void*), i)));}} return
accum;} void* Cyc_Array_fold_left_c( void*(* f)( void*, void*, void*), void* env,
void* accum, struct _tagged_arr x){ int sx=( int) _get_arr_size( x, sizeof( void*));{
int i= 0; for( 0; i <  sx; i ++){ accum= f( env, accum,*(( void**)
_check_unknown_subscript( x, sizeof( void*), i)));}} return accum;} void* Cyc_Array_fold_right(
void*(* f)( void*, void*), struct _tagged_arr x, void* accum){{ int i=( int)(
_get_arr_size( x, sizeof( void*)) -  1); for( 0; i >=  0; i --){ accum= f(*((
void**) _check_unknown_subscript( x, sizeof( void*), i)), accum);}} return accum;}
void* Cyc_Array_fold_right_c( void*(* f)( void*, void*, void*), void* env,
struct _tagged_arr x, void* accum){{ int i=( int)( _get_arr_size( x, sizeof(
void*)) -  1); for( 0; i >=  0; i --){ accum= f( env,*(( void**)
_check_unknown_subscript( x, sizeof( void*), i)), accum);}} return accum;}
struct _tagged_arr Cyc_Array_rev_copy( struct _tagged_arr x){ int sx=( int)
_get_arr_size( x, sizeof( void*)); int n= sx -  1; return({ unsigned int _temp24=(
unsigned int) sx; void** _temp25=( void**) GC_malloc( sizeof( void*) *  _temp24);
struct _tagged_arr _temp27= _tag_arr( _temp25, sizeof( void*),( unsigned int) sx);{
unsigned int _temp26= _temp24; unsigned int i; for( i= 0; i <  _temp26; i ++){
_temp25[ i]=*(( void**) _check_unknown_subscript( x, sizeof( void*),( int)( n - 
i)));}}; _temp27;});} void Cyc_Array_imp_rev( struct _tagged_arr x){ void* temp;
int i= 0; int j=( int)( _get_arr_size( x, sizeof( void*)) -  1); while( i <  j) {
temp=*(( void**) _check_unknown_subscript( x, sizeof( void*), i));*(( void**)
_check_unknown_subscript( x, sizeof( void*), i))=*(( void**)
_check_unknown_subscript( x, sizeof( void*), j));*(( void**)
_check_unknown_subscript( x, sizeof( void*), j))= temp; i ++; j --;}} int Cyc_Array_forall(
int(* pred)( void*), struct _tagged_arr x){ int sx=( int) _get_arr_size( x,
sizeof( void*));{ int i= 0; for( 0; i <  sx; i ++){ if( ! pred(*(( void**)
_check_unknown_subscript( x, sizeof( void*), i)))){ return 0;}}} return 1;} int
Cyc_Array_forall_c( int(* pred)( void*, void*), void* env, struct _tagged_arr x){
int sx=( int) _get_arr_size( x, sizeof( void*));{ int i= 0; for( 0; i <  sx; i
++){ if( ! pred( env,*(( void**) _check_unknown_subscript( x, sizeof( void*), i)))){
return 0;}}} return 1;} int Cyc_Array_exists( int(* pred)( void*), struct
_tagged_arr x){ int sx=( int) _get_arr_size( x, sizeof( void*));{ int i= 0; for(
0; i <  sx; i ++){ if( pred(*(( void**) _check_unknown_subscript( x, sizeof(
void*), i)))){ return 1;}}} return 0;} int Cyc_Array_exists_c( int(* pred)( void*,
void*), void* env, struct _tagged_arr x){ int sx=( int) _get_arr_size( x,
sizeof( void*));{ int i= 0; for( 0; i <  sx; i ++){ if( pred( env,*(( void**)
_check_unknown_subscript( x, sizeof( void*), i)))){ return 1;}}} return 0;}
struct _tagged_arr Cyc_Array_zip( struct _tagged_arr x, struct _tagged_arr y){
int sx=( int) _get_arr_size( x, sizeof( void*)); if( sx !=  _get_arr_size( y,
sizeof( void*))){( int) _throw(( void*) Cyc_Array_Array_mismatch);} return({
unsigned int _temp28=( unsigned int) sx; struct _tuple0* _temp29=( struct
_tuple0*) GC_malloc( sizeof( struct _tuple0) *  _temp28); struct _tagged_arr
_temp32= _tag_arr( _temp29, sizeof( struct _tuple0),( unsigned int) sx);{
unsigned int _temp30= _temp28; unsigned int i; for( i= 0; i <  _temp30; i ++){
_temp29[ i]=({ struct _tuple0 _temp31; _temp31.f1=*(( void**)
_check_unknown_subscript( x, sizeof( void*),( int) i)); _temp31.f2=*(( void**)
_check_unknown_subscript( y, sizeof( void*),( int) i)); _temp31;});}}; _temp32;});}
struct _tuple1 Cyc_Array_split( struct _tagged_arr x){ int sx=( int)
_get_arr_size( x, sizeof( struct _tuple0)); return({ struct _tuple1 _temp33;
_temp33.f1=({ unsigned int _temp38=( unsigned int) sx; void** _temp39=( void**)
GC_malloc( sizeof( void*) *  _temp38); struct _tagged_arr _temp41= _tag_arr(
_temp39, sizeof( void*),( unsigned int) sx);{ unsigned int _temp40= _temp38;
unsigned int i; for( i= 0; i <  _temp40; i ++){ _temp39[ i]=(*(( struct _tuple0*)
_check_unknown_subscript( x, sizeof( struct _tuple0),( int) i))).f1;}}; _temp41;});
_temp33.f2=({ unsigned int _temp34=( unsigned int) sx; void** _temp35=( void**)
GC_malloc( sizeof( void*) *  _temp34); struct _tagged_arr _temp37= _tag_arr(
_temp35, sizeof( void*),( unsigned int) sx);{ unsigned int _temp36= _temp34;
unsigned int i; for( i= 0; i <  _temp36; i ++){ _temp35[ i]=(*(( struct _tuple0*)
_check_unknown_subscript( x, sizeof( struct _tuple0),( int) i))).f2;}}; _temp37;});
_temp33;});} int Cyc_Array_memq( struct _tagged_arr l, void* x){ int s=( int)
_get_arr_size( l, sizeof( void*));{ int i= 0; for( 0; i <  s; i ++){ if(*(( void**)
_check_unknown_subscript( l, sizeof( void*), i)) ==  x){ return 1;}}} return 0;}
int Cyc_Array_mem( int(* compare)( void*, void*), struct _tagged_arr l, void* x){
int s=( int) _get_arr_size( l, sizeof( void*));{ int i= 0; for( 0; i <  s; i ++){
if( 0 ==  compare(*(( void**) _check_unknown_subscript( l, sizeof( void*), i)),
x)){ return 1;}}} return 0;} struct _tagged_arr Cyc_Array_extract( struct
_tagged_arr x, int start, int* n_opt){ int sx=( int) _get_arr_size( x, sizeof(
void*)); int n= n_opt ==  0? sx -  start:*(( int*) _check_null( n_opt)); if((
start <  0? 1: n <=  0)? 1: start + ( n_opt ==  0? 0: n) >  sx){( int) _throw((
void*)({ struct Cyc_Core_InvalidArg_struct* _temp42=( struct Cyc_Core_InvalidArg_struct*)
GC_malloc( sizeof( struct Cyc_Core_InvalidArg_struct)); _temp42[ 0]=({ struct
Cyc_Core_InvalidArg_struct _temp43; _temp43.tag= Cyc_Core_InvalidArg; _temp43.f1=
_tag_arr("Array::extract", sizeof( unsigned char), 15u); _temp43;}); _temp42;}));}
return({ unsigned int _temp44=( unsigned int) n; void** _temp45=( void**)
GC_malloc( sizeof( void*) *  _temp44); struct _tagged_arr _temp47= _tag_arr(
_temp45, sizeof( void*),( unsigned int) n);{ unsigned int _temp46= _temp44;
unsigned int i; for( i= 0; i <  _temp46; i ++){ _temp45[ i]=*(( void**)
_check_unknown_subscript( x, sizeof( void*),( int)( start +  i)));}}; _temp47;});}
