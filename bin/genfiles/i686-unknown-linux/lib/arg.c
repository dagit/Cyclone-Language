// This is a C header file to be used by the output of the Cyclone
// to C translator.  The corresponding definitions are in file lib/runtime_cyc.c
#ifndef _CYC_INCLUDE_H_
#define _CYC_INCLUDE_H_

#include <setjmp.h>

#ifdef NO_CYC_PREFIX
#define ADD_PREFIX(x) x
#else
#define ADD_PREFIX(x) Cyc_##x
#endif

#ifndef offsetof
// should be size_t, but int is fine.
#define offsetof(t,n) ((int)(&(((t *)0)->n)))
#endif

//// Tagged arrays
struct _tagged_arr { 
  unsigned char *curr; 
  unsigned char *base; 
  unsigned char *last_plus_one; 
};

//// Discriminated Unions
struct _xtunion_struct { char *tag; };

// Need one of these per thread (we don't have threads)
// The runtime maintains a stack that contains either _handler_cons
// structs or _RegionHandle structs.  The tag is 0 for a handler_cons
// and 1 for a region handle.  
struct _RuntimeStack {
  int tag; // 0 for an exception handler, 1 for a region handle
  struct _RuntimeStack *next;
};

//// Regions
struct _RegionPage {
#ifdef CYC_REGION_PROFILE
  unsigned int total_bytes;
  unsigned int free_bytes;
#endif
  struct _RegionPage *next;
  char data[0];
};

struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
#ifdef CYC_REGION_PROFILE
  const char         *name;
#endif
};

extern struct _RegionHandle _new_region(const char *);
extern void * _region_malloc(struct _RegionHandle *, unsigned);
extern void * _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
extern void   _free_region(struct _RegionHandle *);

//// Exceptions 
struct _handler_cons {
  struct _RuntimeStack s;
  jmp_buf handler;
};
extern void _push_handler(struct _handler_cons *);
extern void _push_region(struct _RegionHandle *);
extern void _npop_handler(int);
extern void _pop_handler();
extern void _pop_region();

#ifndef _throw
extern int _throw_null();
extern int _throw_arraybounds();
extern int _throw_badalloc();
extern int _throw(void * e);
#endif

extern struct _xtunion_struct *_exn_thrown;

//// Built-in Exceptions
extern struct _xtunion_struct ADD_PREFIX(Null_Exception_struct);
extern struct _xtunion_struct * ADD_PREFIX(Null_Exception);
extern struct _xtunion_struct ADD_PREFIX(Array_bounds_struct);
extern struct _xtunion_struct * ADD_PREFIX(Array_bounds);
extern struct _xtunion_struct ADD_PREFIX(Match_Exception_struct);
extern struct _xtunion_struct * ADD_PREFIX(Match_Exception);
extern struct _xtunion_struct ADD_PREFIX(Bad_alloc_struct);
extern struct _xtunion_struct * ADD_PREFIX(Bad_alloc);

//// Built-in Run-time Checks and company
static inline 
void * _check_null(void * ptr) {
#ifndef NO_CYC_NULL_CHECKS
  if(!ptr)
    _throw_null();
#endif
  return ptr;
}
static inline 
char * _check_known_subscript_null(void * ptr, unsigned bound, 
				   unsigned elt_sz, unsigned index) {
#ifndef NO_CYC_NULL_CHECKS
  if(!ptr)
    _throw_null();
#endif
#ifndef NO_CYC_BOUNDS_CHECKS
  if(index >= bound)
    _throw_arraybounds();
#endif
  return ((char *)ptr) + elt_sz*index;
}
static inline 
unsigned _check_known_subscript_notnull(unsigned bound, unsigned index) {
#ifndef NO_CYC_BOUNDS_CHECKS
  if(index >= bound)
    _throw_arraybounds();
#endif
  return index;
}
static inline 
char * _check_unknown_subscript(struct _tagged_arr arr,
				unsigned elt_sz, unsigned index) {
  // caller casts first argument and result
  // multiplication looks inefficient, but C compiler has to insert it otherwise
  // by inlining, it should be able to avoid actual multiplication
  unsigned char * ans = arr.curr + elt_sz * index;
  // might be faster not to distinguish these two cases. definitely would be
  // smaller.
#ifndef NO_CYC_NULL_CHECKS
  if(!arr.base) 
    _throw_null();
#endif
#ifndef NO_CYC_BOUNDS_CHECKS
  if(ans < arr.base || ans >= arr.last_plus_one)
    _throw_arraybounds();
#endif
  return ans;
}
static inline 
struct _tagged_arr _tag_arr(const void * curr, 
			    unsigned elt_sz, unsigned num_elts) {
  // beware the gcc bug, can happen with *temp = _tag_arr(...) in weird places!
  struct _tagged_arr ans;
  ans.base = (void *)curr;
  ans.curr = (void *)curr;
  ans.last_plus_one = ((char *)curr) + elt_sz * num_elts;
  return ans;
}
static inline
struct _tagged_arr * _init_tag_arr(struct _tagged_arr * arr_ptr, void * arr, 
				   unsigned elt_sz, unsigned num_elts) {
  // we use this to (hopefully) avoid the gcc bug
  arr_ptr->base = arr_ptr->curr = arr;
  arr_ptr->last_plus_one = ((char *)arr) + elt_sz * num_elts;
  return arr_ptr;
}

static inline
char * _untag_arr(struct _tagged_arr arr, unsigned elt_sz, unsigned num_elts) {
  // Note: if arr is "null" base and curr should both be null, so this
  //       is correct (caller checks for null if untagging to @ type)
  // base may not be null if you use t ? pointer subtraction to get 0 -- oh well
#ifndef NO_CYC_BOUNDS_CHECKS
  if(arr.curr < arr.base || arr.curr + elt_sz * num_elts > arr.last_plus_one)
    _throw_arraybounds();
#endif
  return arr.curr;
}
static inline 
unsigned _get_arr_size(struct _tagged_arr arr, unsigned elt_sz) {
  return (arr.curr<arr.base || arr.curr>=arr.last_plus_one) ? 0 : ((arr.last_plus_one - arr.curr) / elt_sz);
}
static inline
struct _tagged_arr _tagged_arr_plus(struct _tagged_arr arr, unsigned elt_sz,
				    int change) {
  struct _tagged_arr ans = arr;
  ans.curr += ((int)elt_sz)*change;
  return ans;
}
static inline
struct _tagged_arr _tagged_arr_inplace_plus(struct _tagged_arr * arr_ptr,
					    unsigned elt_sz, int change) {
  arr_ptr->curr += ((int)elt_sz)*change;
  return *arr_ptr;
}
static inline
struct _tagged_arr _tagged_arr_inplace_plus_post(struct _tagged_arr * arr_ptr,
						 unsigned elt_sz, int change) {
  struct _tagged_arr ans = *arr_ptr;
  arr_ptr->curr += ((int)elt_sz)*change;
  return ans;
}
				  
//// Allocation
extern void * GC_malloc(int);
extern void * GC_malloc_atomic(int);
extern void * GC_calloc(unsigned int,unsigned int);
extern void * GC_calloc_atomic(unsigned int,unsigned int);

static inline void * _cycalloc(int n) {
  void * ans = (void *)GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cycalloc_atomic(int n) {
  void * ans = (void *)GC_malloc_atomic(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cyccalloc(unsigned int n, unsigned int s) {
  void * ans = (void *)GC_calloc(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cyccalloc_atomic(unsigned int n, unsigned int s) {
  void * ans = (void *)GC_calloc_atomic(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
#define MAX_MALLOC_SIZE (1 << 28)
static inline unsigned int _check_times(unsigned int x, unsigned int y) {
  unsigned long long whole_ans = 
    ((unsigned long long)x)*((unsigned long long)y);
  unsigned int word_ans = (unsigned int)whole_ans;
  if(word_ans < whole_ans || word_ans > MAX_MALLOC_SIZE)
    _throw_badalloc();
  return word_ans;
}

#if defined(CYC_REGION_PROFILE) 
extern void * _profile_GC_malloc(int,char *file,int lineno);
extern void * _profile_GC_malloc_atomic(int,char *file,int lineno);
extern void * _profile_region_malloc(struct _RegionHandle *, unsigned int,
                                     char *file,int lineno);
extern struct _RegionHandle _profile_new_region(const char *rgn_name,
						char *file,int lineno);
extern void _profile_free_region(struct _RegionHandle *,
				 char *file,int lineno);
#  if !defined(RUNTIME_CYC)
#define _new_region(n) _profile_new_region(n,__FILE__ ":" __FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__ ":" __FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__ ":" __FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__ ":" __FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__ ":" __FUNCTION__,__LINE__)
#endif
#endif
 struct Cyc_Core_Opt{void*v;};extern unsigned char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Impossible[
15];struct Cyc_Core_Impossible_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Not_found[14];extern unsigned char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_Cstdio___sFILE;struct Cyc_Std___sFILE;extern struct Cyc_Std___sFILE*Cyc_Std_stderr;
extern unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
static const int Cyc_Std_String_pa=0;struct Cyc_Std_String_pa_struct{int tag;struct
_tagged_arr f1;};static const int Cyc_Std_Int_pa=1;struct Cyc_Std_Int_pa_struct{int
tag;unsigned int f1;};static const int Cyc_Std_Double_pa=2;struct Cyc_Std_Double_pa_struct{
int tag;double f1;};static const int Cyc_Std_ShortPtr_pa=3;struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};static const int Cyc_Std_IntPtr_pa=4;struct Cyc_Std_IntPtr_pa_struct{
int tag;unsigned int*f1;};extern int Cyc_Std_fprintf(struct Cyc_Std___sFILE*,struct
_tagged_arr fmt,struct _tagged_arr);static const int Cyc_Std_ShortPtr_sa=0;struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};static const int Cyc_Std_UShortPtr_sa=1;struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};static const int Cyc_Std_IntPtr_sa=2;struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};static const int Cyc_Std_UIntPtr_sa=3;struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};static const int Cyc_Std_StringPtr_sa=4;struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};static const int Cyc_Std_DoublePtr_sa=5;struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};static const int Cyc_Std_FloatPtr_sa=6;struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};extern int Cyc_Std_sscanf(struct _tagged_arr src,struct _tagged_arr
fmt,struct _tagged_arr);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
extern unsigned char Cyc_List_List_mismatch[18];extern unsigned char Cyc_List_Nth[8];
extern unsigned int Cyc_Std_strlen(struct _tagged_arr s);extern int Cyc_Std_strcmp(
struct _tagged_arr s1,struct _tagged_arr s2);extern int Cyc_Std_strncmp(struct
_tagged_arr s1,struct _tagged_arr s2,unsigned int len);extern struct _tagged_arr Cyc_Std_strconcat(
struct _tagged_arr,struct _tagged_arr);extern unsigned char Cyc_Arg_Bad[8];struct Cyc_Arg_Bad_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Arg_Error[10];
static const int Cyc_Arg_Unit_spec=0;struct Cyc_Arg_Unit_spec_struct{int tag;void(*f1)();
};static const int Cyc_Arg_Flag_spec=1;struct Cyc_Arg_Flag_spec_struct{int tag;void(*
f1)(struct _tagged_arr);};static const int Cyc_Arg_FlagString_spec=2;struct Cyc_Arg_FlagString_spec_struct{
int tag;void(*f1)(struct _tagged_arr,struct _tagged_arr);};static const int Cyc_Arg_Set_spec=
3;struct Cyc_Arg_Set_spec_struct{int tag;int*f1;};static const int Cyc_Arg_Clear_spec=
4;struct Cyc_Arg_Clear_spec_struct{int tag;int*f1;};static const int Cyc_Arg_String_spec=
5;struct Cyc_Arg_String_spec_struct{int tag;void(*f1)(struct _tagged_arr);};static
const int Cyc_Arg_Int_spec=6;struct Cyc_Arg_Int_spec_struct{int tag;void(*f1)(int);}
;static const int Cyc_Arg_Rest_spec=7;struct Cyc_Arg_Rest_spec_struct{int tag;void(*
f1)(struct _tagged_arr);};extern void Cyc_Arg_usage(struct Cyc_List_List*,struct
_tagged_arr);extern int Cyc_Arg_current;extern void Cyc_Arg_parse(struct Cyc_List_List*
specs,void(*anonfun)(struct _tagged_arr),struct _tagged_arr errmsg,struct
_tagged_arr args);struct Cyc_Buffer_t;extern struct Cyc_Buffer_t*Cyc_Buffer_create(
unsigned int n);extern struct _tagged_arr Cyc_Buffer_contents(struct Cyc_Buffer_t*);
extern void Cyc_Buffer_add_substring(struct Cyc_Buffer_t*,struct _tagged_arr,int
offset,int len);extern void Cyc_Buffer_add_string(struct Cyc_Buffer_t*,struct
_tagged_arr);extern int isalnum(int);extern int isalpha(int);extern int isascii(int);
extern int iscntrl(int);extern int isdigit(int);extern int isgraph(int);extern int
islower(int);extern int isprint(int);extern int ispunct(int);extern int isspace(int);
extern int isupper(int);extern int isxdigit(int);extern int toascii(int);extern int
tolower(int);extern int toupper(int);extern int _tolower(int);extern int _toupper(int);
extern void*Cyc_Std___assert_fail(struct _tagged_arr assertion,struct _tagged_arr
file,unsigned int line);unsigned char Cyc_Arg_Bad[8]="\000\000\000\000Bad";
unsigned char Cyc_Arg_Error[10]="\000\000\000\000Error";static const int Cyc_Arg_Prefix=
0;struct Cyc_Arg_Prefix_struct{int tag;struct _tagged_arr f1;};static const int Cyc_Arg_Exact=
1;struct Cyc_Arg_Exact_struct{int tag;struct _tagged_arr f1;};static const int Cyc_Arg_Unknown=
0;struct Cyc_Arg_Unknown_struct{int tag;struct _tagged_arr f1;};static const int Cyc_Arg_Missing=
1;struct Cyc_Arg_Missing_struct{int tag;struct _tagged_arr f1;};static const int Cyc_Arg_Message=
2;struct Cyc_Arg_Message_struct{int tag;struct _tagged_arr f1;};static const int Cyc_Arg_Wrong=
3;struct Cyc_Arg_Wrong_struct{int tag;struct _tagged_arr f1;struct _tagged_arr f2;
struct _tagged_arr f3;};struct _tuple0{struct _tagged_arr f1;int f2;struct _tagged_arr
f3;void*f4;struct _tagged_arr f5;};static void*Cyc_Arg_lookup(struct Cyc_List_List*l,
struct _tagged_arr x){while(l != 0){struct _tagged_arr _tmp0=(*((struct _tuple0*)l->hd)).f1;
unsigned int _tmp1=Cyc_Std_strlen(_tmp0);if(_tmp1 > 0?(*((struct _tuple0*)l->hd)).f2:
0){if(Cyc_Std_strncmp(x,(*((struct _tuple0*)l->hd)).f1,_tmp1)== 0){return(*((
struct _tuple0*)l->hd)).f4;}}else{if(Cyc_Std_strcmp(x,(*((struct _tuple0*)l->hd)).f1)
== 0){return(*((struct _tuple0*)l->hd)).f4;}}l=l->tl;}(int)_throw((void*)Cyc_Core_Not_found);}
static struct _tagged_arr Cyc_Arg_Justify_break_line(struct Cyc_Buffer_t*b,int
howmuch,struct _tagged_arr s){if(s.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr){
return(struct _tagged_arr)_tag_arr(0,0,0);}if(howmuch < 0){howmuch=0;}{
unsigned int _tmp2=Cyc_Std_strlen(s);_tmp2 <= _get_arr_size(s,sizeof(unsigned char))?
0:((int(*)(struct _tagged_arr assertion,struct _tagged_arr file,unsigned int line))
Cyc_Std___assert_fail)(_tag_arr("len <= s.size",sizeof(unsigned char),14),
_tag_arr("arg.cyc",sizeof(unsigned char),8),94);if(howmuch > _tmp2){Cyc_Buffer_add_string(
b,s);return(struct _tagged_arr)_tag_arr(0,0,0);}{int i;for(i=howmuch - 1;i >= 0? !
isspace((int)((const unsigned char*)s.curr)[i]): 0;i --){;}if(i < 0){for(i=howmuch?
howmuch - 1: 0;(i < _tmp2?(int)((const unsigned char*)s.curr)[i]: 0)? ! isspace((int)((
const unsigned char*)s.curr)[i]): 0;i ++){;}}Cyc_Buffer_add_substring(b,s,0,i);{
struct _tagged_arr whatsleft=(struct _tagged_arr)_tag_arr(0,0,0);for(0;(i < _tmp2?(
int)((const unsigned char*)s.curr)[i]: 0)? isspace((int)((const unsigned char*)s.curr)[
i]): 0;i ++){;}if(i < _tmp2?(int)((const unsigned char*)s.curr)[i]: 0){whatsleft=
_tagged_arr_plus(s,sizeof(unsigned char),i);}return whatsleft;}}}}void Cyc_Arg_Justify_justify_b(
struct Cyc_Buffer_t*b,int indent,int margin,struct _tagged_arr item,struct _tagged_arr
desc){if(item.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr){Cyc_Buffer_add_string(
b,item);}if(desc.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr){return;}if(
indent < 0){indent=0;}if(margin < 0){margin=0;}{struct _tagged_arr indentstr=({
unsigned int _tmp8=(unsigned int)(indent + 1);unsigned char*_tmp9=(unsigned char*)
_cycalloc_atomic(_check_times(sizeof(unsigned char),_tmp8));struct _tagged_arr
_tmpB=_tag_arr(_tmp9,sizeof(unsigned char),(unsigned int)(indent + 1));{
unsigned int _tmpA=_tmp8;unsigned int i;for(i=0;i < _tmpA;i ++){_tmp9[i]=i == 0?'\n':' ';}};
_tmpB;});unsigned int _tmp3=Cyc_Std_strlen(item);struct _tagged_arr itemsep;if(Cyc_Std_strlen(
desc)> 0){if(_tmp3 + 1 > indent){itemsep=indentstr;}else{itemsep=({unsigned int
_tmp4=indent - _tmp3;unsigned char*_tmp5=(unsigned char*)_cycalloc_atomic(
_check_times(sizeof(unsigned char),_tmp4));struct _tagged_arr _tmp7=_tag_arr(_tmp5,
sizeof(unsigned char),indent - _tmp3);{unsigned int _tmp6=_tmp4;unsigned int i;for(
i=0;i < _tmp6;i ++){_tmp5[i]=' ';}};_tmp7;});}}else{return;}Cyc_Buffer_add_string(
b,itemsep);while(desc.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr){desc=Cyc_Arg_Justify_break_line(
b,margin - indent,desc);if(desc.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr){
Cyc_Buffer_add_string(b,indentstr);}else{Cyc_Buffer_add_string(b,_tag_arr("\n",
sizeof(unsigned char),2));}}return;}}void Cyc_Arg_usage(struct Cyc_List_List*
speclist,struct _tagged_arr errmsg){({struct Cyc_Std_String_pa_struct _tmpD;_tmpD.tag=
Cyc_Std_String_pa;_tmpD.f1=(struct _tagged_arr)errmsg;{void*_tmpC[1]={& _tmpD};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("%s\n",sizeof(unsigned char),4),_tag_arr(_tmpC,sizeof(
void*),1));}});{struct Cyc_Buffer_t*_tmpE=Cyc_Buffer_create(1024);while(speclist
!= 0){Cyc_Arg_Justify_justify_b(_tmpE,12,72,(struct _tagged_arr)Cyc_Std_strconcat((*((
struct _tuple0*)speclist->hd)).f1,(*((struct _tuple0*)speclist->hd)).f3),(*((
struct _tuple0*)speclist->hd)).f5);speclist=speclist->tl;}({struct Cyc_Std_String_pa_struct
_tmp10;_tmp10.tag=Cyc_Std_String_pa;_tmp10.f1=(struct _tagged_arr)Cyc_Buffer_contents(
_tmpE);{void*_tmpF[1]={& _tmp10};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s",
sizeof(unsigned char),3),_tag_arr(_tmpF,sizeof(void*),1));}});}}int Cyc_Arg_current=
0;static struct _tagged_arr Cyc_Arg_args={(void*)0,(void*)0,(void*)(0 + 0)};static
void Cyc_Arg_stop(int prog_pos,void*e,struct Cyc_List_List*speclist,struct
_tagged_arr errmsg){struct _tagged_arr progname=prog_pos < _get_arr_size(Cyc_Arg_args,
sizeof(struct _tagged_arr))?*((struct _tagged_arr*)_check_unknown_subscript(Cyc_Arg_args,
sizeof(struct _tagged_arr),prog_pos)): _tag_arr("(?)",sizeof(unsigned char),4);{
void*_tmp11=e;struct _tagged_arr _tmp1B;struct _tagged_arr _tmp1D;struct _tagged_arr
_tmp1F;struct _tagged_arr _tmp21;struct _tagged_arr _tmp23;struct _tagged_arr _tmp25;
_LL19: if(*((int*)_tmp11)== Cyc_Arg_Unknown){_LL28: _tmp1B=((struct Cyc_Arg_Unknown_struct*)
_tmp11)->f1;goto _LL20;}else{goto _LL21;}_LL21: if(*((int*)_tmp11)== Cyc_Arg_Missing){
_LL30: _tmp1D=((struct Cyc_Arg_Missing_struct*)_tmp11)->f1;goto _LL22;}else{goto
_LL23;}_LL23: if(*((int*)_tmp11)== Cyc_Arg_Wrong){_LL36: _tmp23=((struct Cyc_Arg_Wrong_struct*)
_tmp11)->f1;goto _LL34;_LL34: _tmp21=((struct Cyc_Arg_Wrong_struct*)_tmp11)->f2;
goto _LL32;_LL32: _tmp1F=((struct Cyc_Arg_Wrong_struct*)_tmp11)->f3;goto _LL24;}
else{goto _LL25;}_LL25: if(*((int*)_tmp11)== Cyc_Arg_Message){_LL38: _tmp25=((
struct Cyc_Arg_Message_struct*)_tmp11)->f1;goto _LL26;}else{goto _LL18;}_LL20: if(
Cyc_Std_strcmp(_tmp1B,_tag_arr("-help",sizeof(unsigned char),6))!= 0){({struct
Cyc_Std_String_pa_struct _tmp29;_tmp29.tag=Cyc_Std_String_pa;_tmp29.f1=(struct
_tagged_arr)_tmp1B;{struct Cyc_Std_String_pa_struct _tmp28;_tmp28.tag=Cyc_Std_String_pa;
_tmp28.f1=(struct _tagged_arr)progname;{void*_tmp27[2]={& _tmp28,& _tmp29};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("%s: unknown option `%s'.\n",sizeof(unsigned char),26),
_tag_arr(_tmp27,sizeof(void*),2));}}});}goto _LL18;_LL22:({struct Cyc_Std_String_pa_struct
_tmp2C;_tmp2C.tag=Cyc_Std_String_pa;_tmp2C.f1=(struct _tagged_arr)_tmp1D;{struct
Cyc_Std_String_pa_struct _tmp2B;_tmp2B.tag=Cyc_Std_String_pa;_tmp2B.f1=(struct
_tagged_arr)progname;{void*_tmp2A[2]={& _tmp2B,& _tmp2C};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("%s: option `%s' needs an argument.\n",sizeof(unsigned char),36),
_tag_arr(_tmp2A,sizeof(void*),2));}}});goto _LL18;_LL24:({struct Cyc_Std_String_pa_struct
_tmp31;_tmp31.tag=Cyc_Std_String_pa;_tmp31.f1=(struct _tagged_arr)_tmp1F;{struct
Cyc_Std_String_pa_struct _tmp30;_tmp30.tag=Cyc_Std_String_pa;_tmp30.f1=(struct
_tagged_arr)_tmp23;{struct Cyc_Std_String_pa_struct _tmp2F;_tmp2F.tag=Cyc_Std_String_pa;
_tmp2F.f1=(struct _tagged_arr)_tmp21;{struct Cyc_Std_String_pa_struct _tmp2E;_tmp2E.tag=
Cyc_Std_String_pa;_tmp2E.f1=(struct _tagged_arr)progname;{void*_tmp2D[4]={& _tmp2E,&
_tmp2F,& _tmp30,& _tmp31};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s: wrong argument `%s'; option `%s' expects %s.\n",
sizeof(unsigned char),50),_tag_arr(_tmp2D,sizeof(void*),4));}}}}});goto _LL18;
_LL26:({struct Cyc_Std_String_pa_struct _tmp34;_tmp34.tag=Cyc_Std_String_pa;_tmp34.f1=(
struct _tagged_arr)_tmp25;{struct Cyc_Std_String_pa_struct _tmp33;_tmp33.tag=Cyc_Std_String_pa;
_tmp33.f1=(struct _tagged_arr)progname;{void*_tmp32[2]={& _tmp33,& _tmp34};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("%s: %s.\n",sizeof(unsigned char),9),_tag_arr(_tmp32,
sizeof(void*),2));}}});goto _LL18;_LL18:;}Cyc_Arg_usage(speclist,errmsg);Cyc_Arg_current=(
int)_get_arr_size(Cyc_Arg_args,sizeof(struct _tagged_arr));}void Cyc_Arg_parse(
struct Cyc_List_List*speclist,void(*anonfun)(struct _tagged_arr),struct _tagged_arr
errmsg,struct _tagged_arr orig_args){Cyc_Arg_args=orig_args;{int initpos=Cyc_Arg_current;
unsigned int l=_get_arr_size(Cyc_Arg_args,sizeof(struct _tagged_arr));++ Cyc_Arg_current;
while(Cyc_Arg_current < l){struct _tagged_arr s=*((struct _tagged_arr*)
_check_unknown_subscript(Cyc_Arg_args,sizeof(struct _tagged_arr),Cyc_Arg_current));
if(_get_arr_size(s,sizeof(unsigned char))>= 1?*((const unsigned char*)
_check_unknown_subscript(s,sizeof(unsigned char),0))== '-': 0){void*action;{
struct _handler_cons _tmp35;_push_handler(& _tmp35);{int _tmp37=0;if(setjmp(_tmp35.handler)){
_tmp37=1;}if(! _tmp37){action=Cyc_Arg_lookup(speclist,s);;_pop_handler();}else{
void*_tmp36=(void*)_exn_thrown;void*_tmp39=_tmp36;_LL59: if(_tmp39 == Cyc_Core_Not_found){
goto _LL60;}else{goto _LL61;}_LL61: goto _LL62;_LL60: Cyc_Arg_stop(initpos,(void*)({
struct Cyc_Arg_Unknown_struct*_tmp3F=_cycalloc(sizeof(struct Cyc_Arg_Unknown_struct));
_tmp3F[0]=({struct Cyc_Arg_Unknown_struct _tmp40;_tmp40.tag=Cyc_Arg_Unknown;_tmp40.f1=
s;_tmp40;});_tmp3F;}),speclist,errmsg);return;_LL62:(void)_throw(_tmp39);_LL58:;}}}{
struct _handler_cons _tmp41;_push_handler(& _tmp41);{int _tmp43=0;if(setjmp(_tmp41.handler)){
_tmp43=1;}if(! _tmp43){{void*_tmp44=action;void(*_tmp56)();void(*_tmp58)(struct
_tagged_arr);int*_tmp5A;int*_tmp5C;void(*_tmp5E)(struct _tagged_arr,struct
_tagged_arr);void(*_tmp60)(struct _tagged_arr);void(*_tmp62)(int);void(*_tmp64)(
struct _tagged_arr);_LL70: if(*((int*)_tmp44)== Cyc_Arg_Unit_spec){_LL87: _tmp56=((
struct Cyc_Arg_Unit_spec_struct*)_tmp44)->f1;goto _LL71;}else{goto _LL72;}_LL72: if(*((
int*)_tmp44)== Cyc_Arg_Flag_spec){_LL89: _tmp58=((struct Cyc_Arg_Flag_spec_struct*)
_tmp44)->f1;goto _LL73;}else{goto _LL74;}_LL74: if(*((int*)_tmp44)== Cyc_Arg_Set_spec){
_LL91: _tmp5A=((struct Cyc_Arg_Set_spec_struct*)_tmp44)->f1;goto _LL75;}else{goto
_LL76;}_LL76: if(*((int*)_tmp44)== Cyc_Arg_Clear_spec){_LL93: _tmp5C=((struct Cyc_Arg_Clear_spec_struct*)
_tmp44)->f1;goto _LL77;}else{goto _LL78;}_LL78: if(*((int*)_tmp44)== Cyc_Arg_FlagString_spec){
_LL95: _tmp5E=((struct Cyc_Arg_FlagString_spec_struct*)_tmp44)->f1;goto _LL79;}
else{goto _LL80;}_LL80: if(*((int*)_tmp44)== Cyc_Arg_String_spec){_LL97: _tmp60=((
struct Cyc_Arg_String_spec_struct*)_tmp44)->f1;goto _LL81;}else{goto _LL82;}_LL82:
if(*((int*)_tmp44)== Cyc_Arg_Int_spec){_LL99: _tmp62=((struct Cyc_Arg_Int_spec_struct*)
_tmp44)->f1;goto _LL83;}else{goto _LL84;}_LL84: if(*((int*)_tmp44)== Cyc_Arg_Rest_spec){
_LL101: _tmp64=((struct Cyc_Arg_Rest_spec_struct*)_tmp44)->f1;goto _LL85;}else{goto
_LL69;}_LL71: _tmp56();goto _LL69;_LL73: _tmp58(s);goto _LL69;_LL75:*_tmp5A=1;goto
_LL69;_LL77:*_tmp5C=0;goto _LL69;_LL79: if(Cyc_Arg_current + 1 < l){_tmp5E(s,*((
struct _tagged_arr*)_check_unknown_subscript(Cyc_Arg_args,sizeof(struct
_tagged_arr),Cyc_Arg_current + 1)));++ Cyc_Arg_current;}else{Cyc_Arg_stop(initpos,(
void*)({struct Cyc_Arg_Missing_struct*_tmp66=_cycalloc(sizeof(struct Cyc_Arg_Missing_struct));
_tmp66[0]=({struct Cyc_Arg_Missing_struct _tmp67;_tmp67.tag=Cyc_Arg_Missing;_tmp67.f1=
s;_tmp67;});_tmp66;}),speclist,errmsg);}goto _LL69;_LL81: if(Cyc_Arg_current + 1 < l){
_tmp60(*((struct _tagged_arr*)_check_unknown_subscript(Cyc_Arg_args,sizeof(struct
_tagged_arr),Cyc_Arg_current + 1)));++ Cyc_Arg_current;}else{Cyc_Arg_stop(initpos,(
void*)({struct Cyc_Arg_Missing_struct*_tmp68=_cycalloc(sizeof(struct Cyc_Arg_Missing_struct));
_tmp68[0]=({struct Cyc_Arg_Missing_struct _tmp69;_tmp69.tag=Cyc_Arg_Missing;_tmp69.f1=
s;_tmp69;});_tmp68;}),speclist,errmsg);}goto _LL69;_LL83: {struct _tagged_arr arg=*((
struct _tagged_arr*)_check_unknown_subscript(Cyc_Arg_args,sizeof(struct
_tagged_arr),Cyc_Arg_current + 1));int n=0;if(({struct Cyc_Std_IntPtr_sa_struct
_tmp6B;_tmp6B.tag=Cyc_Std_IntPtr_sa;_tmp6B.f1=& n;{void*_tmp6A[1]={& _tmp6B};Cyc_Std_sscanf(
arg,_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmp6A,sizeof(void*),1));}})
!= 1){Cyc_Arg_stop(initpos,(void*)({struct Cyc_Arg_Wrong_struct*_tmp6C=_cycalloc(
sizeof(struct Cyc_Arg_Wrong_struct));_tmp6C[0]=({struct Cyc_Arg_Wrong_struct _tmp6D;
_tmp6D.tag=Cyc_Arg_Wrong;_tmp6D.f1=s;_tmp6D.f2=arg;_tmp6D.f3=_tag_arr("an integer",
sizeof(unsigned char),11);_tmp6D;});_tmp6C;}),speclist,errmsg);_npop_handler(0);
return;}_tmp62(n);++ Cyc_Arg_current;goto _LL69;}_LL85: while(Cyc_Arg_current < l - 1){
_tmp64(*((struct _tagged_arr*)_check_unknown_subscript(Cyc_Arg_args,sizeof(struct
_tagged_arr),Cyc_Arg_current + 1)));++ Cyc_Arg_current;}goto _LL69;_LL69:;};
_pop_handler();}else{void*_tmp42=(void*)_exn_thrown;void*_tmp6F=_tmp42;struct
_tagged_arr _tmp75;_LL113: if(*((void**)_tmp6F)== Cyc_Arg_Bad){_LL118: _tmp75=((
struct Cyc_Arg_Bad_struct*)_tmp6F)->f1;goto _LL114;}else{goto _LL115;}_LL115: goto
_LL116;_LL114: Cyc_Arg_stop(initpos,(void*)({struct Cyc_Arg_Message_struct*_tmp77=
_cycalloc(sizeof(struct Cyc_Arg_Message_struct));_tmp77[0]=({struct Cyc_Arg_Message_struct
_tmp78;_tmp78.tag=Cyc_Arg_Message;_tmp78.f1=_tmp75;_tmp78;});_tmp77;}),speclist,
errmsg);goto _LL112;_LL116:(void)_throw(_tmp6F);_LL112:;}}}++ Cyc_Arg_current;}
else{{struct _handler_cons _tmp79;_push_handler(& _tmp79);{int _tmp7B=0;if(setjmp(
_tmp79.handler)){_tmp7B=1;}if(! _tmp7B){anonfun(s);;_pop_handler();}else{void*
_tmp7A=(void*)_exn_thrown;void*_tmp7D=_tmp7A;struct _tagged_arr _tmp83;_LL127: if(*((
void**)_tmp7D)== Cyc_Arg_Bad){_LL132: _tmp83=((struct Cyc_Arg_Bad_struct*)_tmp7D)->f1;
goto _LL128;}else{goto _LL129;}_LL129: goto _LL130;_LL128: Cyc_Arg_stop(initpos,(void*)({
struct Cyc_Arg_Message_struct*_tmp85=_cycalloc(sizeof(struct Cyc_Arg_Message_struct));
_tmp85[0]=({struct Cyc_Arg_Message_struct _tmp86;_tmp86.tag=Cyc_Arg_Message;_tmp86.f1=
_tmp83;_tmp86;});_tmp85;}),speclist,errmsg);goto _LL126;_LL130:(void)_throw(
_tmp7D);_LL126:;}}}++ Cyc_Arg_current;}}}}
