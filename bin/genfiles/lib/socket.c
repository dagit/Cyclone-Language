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
unsigned char*); extern int* __errno(); extern int socket( int domain, int type,
int protocol); extern int Cyc_Socket_bind( int fd, void* addr); extern int Cyc_Socket_connect(
int fd, void* addr); extern int Cyc_Socket_setsockopt( int fd, int level, int
optname, struct _tagged_arr optval, unsigned int optlen); extern int listen( int
fd, int n); struct _tuple0{ int f1; void* f2; } ; extern struct _tuple0 Cyc_Socket_accept(
int fd); extern int shutdown( int fd, int how); struct Cyc_Inet_in_addr{
unsigned int s_addr; } ; struct Cyc_Inet_sockaddr_in{ unsigned short sin_family;
unsigned short sin_port; struct Cyc_Inet_in_addr sin_addr; unsigned char
sin_zero[ 8u]; } ; extern unsigned char Cyc_Inet_sockaddr_in[ 16u]; struct Cyc_Inet_sockaddr_in_struct{
unsigned char* tag; struct Cyc_Inet_sockaddr_in f1; } ; extern unsigned int
htonl( unsigned int); extern unsigned short htons( unsigned short); extern
unsigned int ntohl( unsigned int); extern unsigned short ntohs( unsigned short);
extern int _sys_nerr  __attribute__(( dllimport )) ; extern int bind( int fd,
const struct Cyc_Inet_sockaddr_in* addr, unsigned int len); extern int connect(
int fd, const struct Cyc_Inet_sockaddr_in* addr, unsigned int len); extern int
accept( int fd, struct Cyc_Inet_sockaddr_in* addr, unsigned int* len); extern
struct Cyc_Inet_sockaddr_in* sockaddr_to_Csockaddr( void* addr); extern void*
Csockaddr_to_sockaddr( const struct Cyc_Inet_sockaddr_in*, unsigned int); struct
_tuple1{ struct Cyc_Inet_sockaddr_in* f1; unsigned int f2; } ; struct _tuple1
Cyc_Csocket_sa2Csa( void* addr){ struct Cyc_Inet_sockaddr_in* ptr=
sockaddr_to_Csockaddr( addr); void* _temp0= addr; struct Cyc_Inet_sockaddr_in
_temp6; _LL2: if(*(( void**) _temp0) ==  Cyc_Inet_sockaddr_in){ _LL7: _temp6=((
struct Cyc_Inet_sockaddr_in_struct*) _temp0)->f1; goto _LL3;} else{ goto _LL4;}
_LL4: goto _LL5; _LL3: return({ struct _tuple1 _temp8; _temp8.f1= ptr; _temp8.f2=(
unsigned int) sizeof( struct Cyc_Inet_sockaddr_in); _temp8;}); _LL5:( int)
_throw(( void*)({ struct Cyc_Core_Failure_struct* _temp9=( struct Cyc_Core_Failure_struct*)
GC_malloc( sizeof( struct Cyc_Core_Failure_struct)); _temp9[ 0]=({ struct Cyc_Core_Failure_struct
_temp10; _temp10.tag= Cyc_Core_Failure; _temp10.f1= _tag_arr("Unexpected xtunion sockaddr found",
sizeof( unsigned char), 34u); _temp10;}); _temp9;})); _LL1:;} void* Cyc_Csocket_Csa2sa(
const struct Cyc_Inet_sockaddr_in* addr, unsigned int len){ return
Csockaddr_to_sockaddr( addr, len);} int Cyc_Socket_bind( int fd, void* addr){
unsigned int _temp13; struct Cyc_Inet_sockaddr_in* _temp15; struct _tuple1
_temp11= Cyc_Csocket_sa2Csa( addr); _LL16: _temp15= _temp11.f1; goto _LL14;
_LL14: _temp13= _temp11.f2; goto _LL12; _LL12: return bind( fd,( const struct
Cyc_Inet_sockaddr_in*) _temp15, _temp13);} int Cyc_Socket_connect( int fd, void*
addr){ unsigned int _temp19; struct Cyc_Inet_sockaddr_in* _temp21; struct
_tuple1 _temp17= Cyc_Csocket_sa2Csa( addr); _LL22: _temp21= _temp17.f1; goto
_LL20; _LL20: _temp19= _temp17.f2; goto _LL18; _LL18: return connect( fd,( const
struct Cyc_Inet_sockaddr_in*) _temp21, _temp19);} int Cyc_Socket_setsockopt( int
fd, int level, int optname, struct _tagged_arr optval, unsigned int optlen){*
__errno()= 109; return - 1;} struct _tuple0 Cyc_Socket_accept( int fd){ struct
Cyc_Inet_sockaddr_in Caddr=({ struct Cyc_Inet_sockaddr_in _temp24; _temp24.sin_family=
0; _temp24.sin_port= 0; _temp24.sin_addr=({ struct Cyc_Inet_in_addr _temp26;
_temp26.s_addr= 0; _temp26;});{ unsigned int _temp25= 8u; unsigned int i; for( i=
0; i <  _temp25; i ++){( _temp24.sin_zero)[ i]='\000';}} _temp24;});
unsigned int len= sizeof( struct Cyc_Inet_sockaddr_in); int result= accept( fd,(
struct Cyc_Inet_sockaddr_in*)& Caddr,( unsigned int*)& len); return({ struct
_tuple0 _temp23; _temp23.f1= result; _temp23.f2= Cyc_Csocket_Csa2sa(( const
struct Cyc_Inet_sockaddr_in*)& Caddr, len); _temp23;});}
