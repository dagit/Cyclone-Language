 struct Cyc_timespec{ int tv_sec; int tv_nsec; } ; struct Cyc_itimerspec{ struct
Cyc_timespec it_interval; struct Cyc_timespec it_value; } ; struct Cyc__types_fd_set{
int fds_bits[ 8u]; } ; extern void exit( int); extern void* abort(); struct Cyc_Core_Opt{
void* v; } ; extern unsigned char Cyc_Core_InvalidArg[ 15u]; struct Cyc_Core_InvalidArg_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Failure[
12u]; struct Cyc_Core_Failure_struct{ unsigned char* tag; struct _tagged_string
f1; } ; extern unsigned char Cyc_Core_Impossible[ 15u]; struct Cyc_Core_Impossible_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char Cyc_Core_Not_found[
14u]; extern unsigned char Cyc_Core_Unreachable[ 16u]; struct Cyc_Core_Unreachable_struct{
unsigned char* tag; struct _tagged_string f1; } ; extern unsigned char*
string_to_Cstring( struct _tagged_string); extern unsigned char*
underlying_Cstring( struct _tagged_string); extern struct _tagged_string
Cstring_to_string( unsigned char*); struct _tagged_ptr0{ struct _tagged_string*
curr; struct _tagged_string* base; struct _tagged_string* last_plus_one; } ;
extern struct _tagged_ptr0 ntCsl_to_ntsl( unsigned char**); extern int system(
unsigned char*); extern int* __errno(); extern int socket( int domain, int type,
int protocol); extern int listen( int fd, int n); extern int shutdown( int fd,
int how); struct Cyc_Inet_in_addr{ unsigned int s_addr; } ; struct Cyc_Inet_sockaddr_in{
unsigned short sin_family; unsigned short sin_port; struct Cyc_Inet_in_addr
sin_addr; unsigned char sin_zero[ 8u]; } ; extern unsigned char Cyc_Inet_sockaddr_in[
16u]; struct Cyc_Inet_sockaddr_in_struct{ unsigned char* tag; struct Cyc_Inet_sockaddr_in
f1; } ; unsigned char Cyc_Inet_sockaddr_in[ 16u]="\000\000\000\000sockaddr_in";