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
extern struct _tagged_arr ntCsl_to_ntsl( unsigned char**); extern int socket(
int domain, int type, int protocol); extern int listen( int fd, int n); extern
int shutdown( int fd, int how); struct Cyc_Std_in_addr{ unsigned int s_addr; } ;
struct Cyc_Std_sockaddr_in{ unsigned short sin_family; unsigned short sin_port;
struct Cyc_Std_in_addr sin_addr; unsigned char sin_zero[ 8u]; } ; extern
unsigned char Cyc_Std_sockaddr_in[ 16u]; struct Cyc_Std_sockaddr_in_struct{
unsigned char* tag; struct Cyc_Std_sockaddr_in f1; } ; extern unsigned int htonl(
unsigned int); extern unsigned short htons( unsigned short); extern unsigned int
ntohl( unsigned int); extern unsigned short ntohs( unsigned short);
unsigned char Cyc_Std_sockaddr_in[ 16u]="\000\000\000\000sockaddr_in";
