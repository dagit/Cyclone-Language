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
int protocol); extern int Cyc_Socket_connect( int fd, void* addr); extern int
listen( int fd, int n); extern int shutdown( int fd, int how); struct Cyc_Inet_in_addr{
unsigned int s_addr; } ; struct Cyc_Inet_sockaddr_in{ unsigned short sin_family;
unsigned short sin_port; struct Cyc_Inet_in_addr sin_addr; unsigned char
sin_zero[ 8u]; } ; extern unsigned char Cyc_Inet_sockaddr_in[ 16u]; struct Cyc_Inet_sockaddr_in_struct{
unsigned char* tag; struct Cyc_Inet_sockaddr_in f1; } ; struct Cyc_Csocket_Csockaddr;
extern int connect( int, const struct Cyc_Csocket_Csockaddr* addr, int len);
extern struct Cyc_Csocket_Csockaddr* sockaddr_to_Csockaddr( void* addr); struct
_tuple0{ struct Cyc_Csocket_Csockaddr* f1; int f2; } ; struct _tuple0 Cyc_Csocket_sa2Csa(
void* addr){ struct Cyc_Csocket_Csockaddr* ptr= sockaddr_to_Csockaddr( addr);
void* _temp0= addr; struct Cyc_Inet_sockaddr_in _temp6; _LL2: if(*(( void**)
_temp0) == Cyc_Inet_sockaddr_in){ _LL7: _temp6=(( struct Cyc_Inet_sockaddr_in_struct*)
_temp0)->f1; goto _LL3;} else{ goto _LL4;} _LL4: goto _LL5; _LL3: return({
struct _tuple0 _temp8; _temp8.f1= ptr; _temp8.f2=( int) sizeof( struct Cyc_Inet_sockaddr_in);
_temp8;}); _LL5:( void) _throw(( void*)({ struct Cyc_Core_Failure_struct* _temp9=(
struct Cyc_Core_Failure_struct*) GC_malloc( sizeof( struct Cyc_Core_Failure_struct));
_temp9[ 0]=({ struct Cyc_Core_Failure_struct _temp10; _temp10.tag= Cyc_Core_Failure;
_temp10.f1=({ unsigned char* _temp11=( unsigned char*)"Unexpected xtunion sockaddr found";
struct _tagged_string _temp12; _temp12.curr= _temp11; _temp12.base= _temp11;
_temp12.last_plus_one= _temp11 + 34; _temp12;}); _temp10;}); _temp9;})); _LL1:;}
int Cyc_Socket_connect( int fd, void* addr){ int _temp15; struct Cyc_Csocket_Csockaddr*
_temp17; struct _tuple0 _temp13= Cyc_Csocket_sa2Csa( addr); _LL18: _temp17=
_temp13.f1; goto _LL16; _LL16: _temp15= _temp13.f2; goto _LL14; _LL14: return
connect( fd,( const struct Cyc_Csocket_Csockaddr*) _temp17, _temp15);}