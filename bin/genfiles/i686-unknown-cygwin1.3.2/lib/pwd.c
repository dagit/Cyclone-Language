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
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_Std_passwd{
struct _tagged_arr pw_name; struct _tagged_arr pw_passwd; unsigned short pw_uid;
unsigned short pw_gid; struct _tagged_arr pw_gecos; struct _tagged_arr pw_dir;
struct _tagged_arr pw_shell; } ; extern struct Cyc_Std_passwd* Cyc_Std_getpwnam(
struct _tagged_arr name); extern struct Cyc_Std_passwd* Cyc_Std_getpwuid(
unsigned short uid); struct Cyc_Cpwd_Cpasswd{ unsigned char* pw_name;
unsigned char* pw_passwd; unsigned short pw_uid; unsigned short pw_gid;
unsigned char* pw_gecos; unsigned char* pw_dir; unsigned char* pw_shell; } ;
extern struct Cyc_Cpwd_Cpasswd* getpwnam( unsigned char* name); extern struct
Cyc_Cpwd_Cpasswd* getpwuid( unsigned short uid); struct Cyc_Std_passwd* Cyc_Std_getpwnam(
struct _tagged_arr name){ struct Cyc_Cpwd_Cpasswd* src= getpwnam(
string_to_Cstring( name)); return( unsigned int) src?({ struct Cyc_Std_passwd*
_temp0=( struct Cyc_Std_passwd*) _cycalloc( sizeof( struct Cyc_Std_passwd));
_temp0->pw_name=( struct _tagged_arr) Cstring_to_string((( struct Cyc_Cpwd_Cpasswd*)
_check_null( src))->pw_name); _temp0->pw_passwd=( struct _tagged_arr)
Cstring_to_string((( struct Cyc_Cpwd_Cpasswd*) _check_null( src))->pw_passwd);
_temp0->pw_uid=(( struct Cyc_Cpwd_Cpasswd*) _check_null( src))->pw_uid; _temp0->pw_gid=((
struct Cyc_Cpwd_Cpasswd*) _check_null( src))->pw_gid; _temp0->pw_gecos=( struct
_tagged_arr) Cstring_to_string((( struct Cyc_Cpwd_Cpasswd*) _check_null( src))->pw_gecos);
_temp0->pw_dir=( struct _tagged_arr) Cstring_to_string((( struct Cyc_Cpwd_Cpasswd*)
_check_null( src))->pw_dir); _temp0->pw_shell=( struct _tagged_arr)
Cstring_to_string((( struct Cyc_Cpwd_Cpasswd*) _check_null( src))->pw_shell);
_temp0;}): 0;} struct Cyc_Std_passwd* Cyc_Std_getpwuid( unsigned short uid){
struct Cyc_Cpwd_Cpasswd* src= getpwuid( uid); return( unsigned int) src?({
struct Cyc_Std_passwd* _temp1=( struct Cyc_Std_passwd*) _cycalloc( sizeof(
struct Cyc_Std_passwd)); _temp1->pw_name=( struct _tagged_arr) Cstring_to_string(((
struct Cyc_Cpwd_Cpasswd*) _check_null( src))->pw_name); _temp1->pw_passwd=(
struct _tagged_arr) Cstring_to_string((( struct Cyc_Cpwd_Cpasswd*) _check_null(
src))->pw_passwd); _temp1->pw_uid=(( struct Cyc_Cpwd_Cpasswd*) _check_null( src))->pw_uid;
_temp1->pw_gid=(( struct Cyc_Cpwd_Cpasswd*) _check_null( src))->pw_gid; _temp1->pw_gecos=(
struct _tagged_arr) Cstring_to_string((( struct Cyc_Cpwd_Cpasswd*) _check_null(
src))->pw_gecos); _temp1->pw_dir=( struct _tagged_arr) Cstring_to_string(((
struct Cyc_Cpwd_Cpasswd*) _check_null( src))->pw_dir); _temp1->pw_shell=( struct
_tagged_arr) Cstring_to_string((( struct Cyc_Cpwd_Cpasswd*) _check_null( src))->pw_shell);
_temp1;}): 0;}
