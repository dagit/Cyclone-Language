 struct Cyc_timespec{ unsigned int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{
struct Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; extern void
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
unsigned char*); extern int* __errno(); extern int socket( int domain, int type,
int protocol); extern int listen( int fd, int n); extern int shutdown( int fd,
int how); struct Cyc_Inet_in_addr{ unsigned int s_addr; } ; struct Cyc_Inet_sockaddr_in{
unsigned short sin_family; unsigned short sin_port; struct Cyc_Inet_in_addr
sin_addr; unsigned char sin_zero[ 8u]; } ; extern unsigned char Cyc_Inet_sockaddr_in[
16u]; struct Cyc_Inet_sockaddr_in_struct{ unsigned char* tag; struct Cyc_Inet_sockaddr_in
f1; } ; struct Cyc_NetDB_servent{ struct _tagged_arr s_name; struct _tagged_arr
s_aliases; unsigned short s_port; struct _tagged_arr s_proto; } ; struct Cyc_NetDB_hostent{
struct _tagged_arr h_name; struct _tagged_arr h_aliases; int h_addrtype; int
h_length; struct _tagged_arr h_addr_list; } ; extern struct Cyc_NetDB_servent*
Cyc_NetDB_getservbyname( struct _tagged_arr name, struct _tagged_arr proto);
extern struct Cyc_NetDB_hostent* Cyc_NetDB_gethostbyname( struct _tagged_arr
name); struct Cyc_Cnetdb_Cservent{ unsigned char* s_name; unsigned char**
s_aliases; unsigned short s_port; unsigned char* s_proto; } ; struct Cyc_Cnetdb_Chostent{
unsigned char* h_name; unsigned char** h_aliases; short h_addrtype; short
h_length; struct Cyc_Inet_in_addr** h_addr_list; } ; extern struct Cyc_Cnetdb_Cservent*
getservbyname( unsigned char* name, unsigned char* proto); extern struct Cyc_Cnetdb_Chostent*
gethostbyname( unsigned char* name); extern struct _tagged_arr pntlp_toCyc(
struct Cyc_Inet_in_addr**); struct Cyc_NetDB_servent* Cyc_NetDB_getservbyname(
struct _tagged_arr name, struct _tagged_arr proto){ struct Cyc_Cnetdb_Cservent*
src= getservbyname( string_to_Cstring( name), string_to_Cstring( proto)); return(
unsigned int) src?({ struct Cyc_NetDB_servent* _temp0=( struct Cyc_NetDB_servent*)
GC_malloc( sizeof( struct Cyc_NetDB_servent)); _temp0->s_name= Cstring_to_string(((
struct Cyc_Cnetdb_Cservent*) _check_null( src))->s_name); _temp0->s_aliases=
ntCsl_to_ntsl((( struct Cyc_Cnetdb_Cservent*) _check_null( src))->s_aliases);
_temp0->s_port=(( struct Cyc_Cnetdb_Cservent*) _check_null( src))->s_port;
_temp0->s_proto= Cstring_to_string((( struct Cyc_Cnetdb_Cservent*) _check_null(
src))->s_proto); _temp0;}): 0;} struct Cyc_NetDB_hostent* Cyc_NetDB_gethostbyname(
struct _tagged_arr name){ struct Cyc_Cnetdb_Chostent* src= gethostbyname(
string_to_Cstring( name)); return( unsigned int) src?({ struct Cyc_NetDB_hostent*
_temp1=( struct Cyc_NetDB_hostent*) GC_malloc( sizeof( struct Cyc_NetDB_hostent));
_temp1->h_name= Cstring_to_string((( struct Cyc_Cnetdb_Chostent*) _check_null(
src))->h_name); _temp1->h_aliases= ntCsl_to_ntsl((( struct Cyc_Cnetdb_Chostent*)
_check_null( src))->h_aliases); _temp1->h_addrtype=( int)(( struct Cyc_Cnetdb_Chostent*)
_check_null( src))->h_addrtype; _temp1->h_length=( int)(( struct Cyc_Cnetdb_Chostent*)
_check_null( src))->h_length; _temp1->h_addr_list= pntlp_toCyc((( struct Cyc_Cnetdb_Chostent*)
_check_null( src))->h_addr_list); _temp1;}): 0;}