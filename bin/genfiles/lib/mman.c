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
unsigned char*); extern struct _tagged_arr Cyc_Mman_mmap( unsigned int length,
int prot, int flags, int fd, int offset); extern int Cyc_Mman_munmap( struct
_tagged_arr start, unsigned int length); extern unsigned char* mmap(
unsigned char* start, unsigned int length, int prot, int flags, int fd, int
offset); extern int munmap( unsigned char* start, unsigned int length); struct
_tagged_arr Cyc_Mman_mmap( unsigned int length, int prot, int flags, int fd, int
offset){ return wrap_Cstring_as_string( mmap( 0, length, prot, flags, fd, offset),
length);} int Cyc_Mman_munmap( struct _tagged_arr start, unsigned int length){
return munmap( underlying_Cstring(( struct _tagged_arr) start), length);}
