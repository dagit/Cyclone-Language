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
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); struct Cyc_Std_group{
struct _tagged_arr gr_name; struct _tagged_arr gr_passwd; unsigned int gr_gid;
struct _tagged_arr gr_mem; } ; extern struct Cyc_Std_group* Cyc_Std_getgrnam(
struct _tagged_arr name); extern struct Cyc_Std_group* Cyc_Std_getgrgid(
unsigned int uid); extern int Cyc_Std_initgroups( struct _tagged_arr user,
unsigned int group); struct Cyc_Cgrp_Cgroup{ unsigned char* gr_name;
unsigned char* gr_passwd; unsigned int gr_gid; unsigned char** gr_mem; } ;
extern struct Cyc_Cgrp_Cgroup* getgrnam( unsigned char* name); extern struct Cyc_Cgrp_Cgroup*
getgrgid( unsigned int gid); extern int initgroups( unsigned char* user,
unsigned int group); struct Cyc_Std_group* Cyc_Std_getgrnam( struct _tagged_arr
name){ struct Cyc_Cgrp_Cgroup* src= getgrnam( string_to_Cstring( name)); return(
unsigned int) src?({ struct Cyc_Std_group* _temp0=( struct Cyc_Std_group*)
_cycalloc( sizeof( struct Cyc_Std_group)); _temp0->gr_name=( struct _tagged_arr)
Cstring_to_string((( struct Cyc_Cgrp_Cgroup*) _check_null( src))->gr_name);
_temp0->gr_passwd=( struct _tagged_arr) Cstring_to_string((( struct Cyc_Cgrp_Cgroup*)
_check_null( src))->gr_passwd); _temp0->gr_gid=(( struct Cyc_Cgrp_Cgroup*)
_check_null( src))->gr_gid; _temp0->gr_mem= ntCsl_to_ntsl((( struct Cyc_Cgrp_Cgroup*)
_check_null( src))->gr_mem); _temp0;}): 0;} struct Cyc_Std_group* Cyc_Std_getgrgid(
unsigned int gid){ struct Cyc_Cgrp_Cgroup* src= getgrgid( gid); return(
unsigned int) src?({ struct Cyc_Std_group* _temp1=( struct Cyc_Std_group*)
_cycalloc( sizeof( struct Cyc_Std_group)); _temp1->gr_name=( struct _tagged_arr)
Cstring_to_string((( struct Cyc_Cgrp_Cgroup*) _check_null( src))->gr_name);
_temp1->gr_passwd=( struct _tagged_arr) Cstring_to_string((( struct Cyc_Cgrp_Cgroup*)
_check_null( src))->gr_passwd); _temp1->gr_gid=(( struct Cyc_Cgrp_Cgroup*)
_check_null( src))->gr_gid; _temp1->gr_mem= ntCsl_to_ntsl((( struct Cyc_Cgrp_Cgroup*)
_check_null( src))->gr_mem); _temp1;}): 0;} int Cyc_Std_initgroups( struct
_tagged_arr user, unsigned int group){ return initgroups( string_to_Cstring(
user), group);}
