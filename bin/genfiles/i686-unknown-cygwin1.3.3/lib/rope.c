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
 struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{void*v;};extern
struct _tagged_arr Cyc_Core_new_string(unsigned int);extern unsigned char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{unsigned char*tag;struct _tagged_arr f1;
};extern unsigned char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Impossible[
15];struct Cyc_Core_Impossible_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Not_found[14];extern unsigned char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern int Cyc_List_length(
struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[18];extern
unsigned char Cyc_List_Nth[8];struct Cyc_Rope_Rope_node;extern struct Cyc_Rope_Rope_node*
Cyc_Rope_from_string(struct _tagged_arr);extern struct _tagged_arr Cyc_Rope_to_string(
struct Cyc_Rope_Rope_node*);extern struct Cyc_Rope_Rope_node*Cyc_Rope_concat(struct
Cyc_Rope_Rope_node*,struct Cyc_Rope_Rope_node*);extern struct Cyc_Rope_Rope_node*
Cyc_Rope_concata(struct _tagged_arr);extern struct Cyc_Rope_Rope_node*Cyc_Rope_concatl(
struct Cyc_List_List*);extern unsigned int Cyc_Rope_length(struct Cyc_Rope_Rope_node*);
extern int Cyc_Rope_cmp(struct Cyc_Rope_Rope_node*,struct Cyc_Rope_Rope_node*);
extern unsigned int Cyc_Std_strlen(struct _tagged_arr s);extern int Cyc_Std_strcmp(
struct _tagged_arr s1,struct _tagged_arr s2);extern struct _tagged_arr Cyc_Std_strncpy(
struct _tagged_arr,struct _tagged_arr,unsigned int);struct Cyc_Rope_String_rope_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Rope_Array_rope_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Rope_Rope_node{void*v;};struct Cyc_Rope_Rope_node*Cyc_Rope_from_string(
struct _tagged_arr s){return({struct Cyc_Rope_Rope_node*_tmp0=_cycalloc(sizeof(
struct Cyc_Rope_Rope_node));_tmp0->v=(void*)((void*)({struct Cyc_Rope_String_rope_struct*
_tmp1=_cycalloc(sizeof(struct Cyc_Rope_String_rope_struct));_tmp1[0]=({struct Cyc_Rope_String_rope_struct
_tmp2;_tmp2.tag=0;_tmp2.f1=s;_tmp2;});_tmp1;}));_tmp0;});}struct Cyc_Rope_Rope_node*
Cyc_Rope_concat(struct Cyc_Rope_Rope_node*r1,struct Cyc_Rope_Rope_node*r2){return({
struct Cyc_Rope_Rope_node*_tmp3=_cycalloc(sizeof(struct Cyc_Rope_Rope_node));_tmp3->v=(
void*)((void*)({struct Cyc_Rope_Array_rope_struct*_tmp4=_cycalloc(sizeof(struct
Cyc_Rope_Array_rope_struct));_tmp4[0]=({struct Cyc_Rope_Array_rope_struct _tmp5;
_tmp5.tag=1;_tmp5.f1=_tag_arr(({struct Cyc_Rope_Rope_node**_tmp6=_cycalloc(
sizeof(struct Cyc_Rope_Rope_node*)* 2);_tmp6[0]=r1;_tmp6[1]=r2;_tmp6;}),sizeof(
struct Cyc_Rope_Rope_node*),2);_tmp5;});_tmp4;}));_tmp3;});}struct Cyc_Rope_Rope_node*
Cyc_Rope_concata(struct _tagged_arr rs){return({struct Cyc_Rope_Rope_node*_tmp7=
_cycalloc(sizeof(struct Cyc_Rope_Rope_node));_tmp7->v=(void*)((void*)({struct Cyc_Rope_Array_rope_struct*
_tmp8=_cycalloc(sizeof(struct Cyc_Rope_Array_rope_struct));_tmp8[0]=({struct Cyc_Rope_Array_rope_struct
_tmp9;_tmp9.tag=1;_tmp9.f1=rs;_tmp9;});_tmp8;}));_tmp7;});}struct Cyc_Rope_Rope_node*
Cyc_Rope_concatl(struct Cyc_List_List*l){return({struct Cyc_Rope_Rope_node*_tmpA=
_cycalloc(sizeof(struct Cyc_Rope_Rope_node));_tmpA->v=(void*)((void*)({struct Cyc_Rope_Array_rope_struct*
_tmpB=_cycalloc(sizeof(struct Cyc_Rope_Array_rope_struct));_tmpB[0]=({struct Cyc_Rope_Array_rope_struct
_tmpC;_tmpC.tag=1;_tmpC.f1=({unsigned int _tmpD=(unsigned int)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(l);struct Cyc_Rope_Rope_node**_tmpE=(struct Cyc_Rope_Rope_node**)
_cycalloc(_check_times(sizeof(struct Cyc_Rope_Rope_node*),_tmpD));struct
_tagged_arr _tmp10=_tag_arr(_tmpE,sizeof(struct Cyc_Rope_Rope_node*),(unsigned int)((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(l));{unsigned int _tmpF=_tmpD;
unsigned int i;for(i=0;i < _tmpF;i ++){_tmpE[i]=({struct Cyc_Rope_Rope_node*r=(
struct Cyc_Rope_Rope_node*)((struct Cyc_List_List*)_check_null(l))->hd;l=l->tl;r;});}};
_tmp10;});_tmpC;});_tmpB;}));_tmpA;});}unsigned int Cyc_Rope_length(struct Cyc_Rope_Rope_node*
r){void*_tmp11=(void*)r->v;struct _tagged_arr _tmp12;struct _tagged_arr _tmp13;_LL1:
if(*((int*)_tmp11)== 0){_LL5: _tmp12=((struct Cyc_Rope_String_rope_struct*)_tmp11)->f1;
goto _LL2;}else{goto _LL3;}_LL3: if(*((int*)_tmp11)== 1){_LL6: _tmp13=((struct Cyc_Rope_Array_rope_struct*)
_tmp11)->f1;goto _LL4;}else{goto _LL0;}_LL2: return(unsigned int)Cyc_Std_strlen(
_tmp12);_LL4: {unsigned int total=0;unsigned int sz=_get_arr_size(_tmp13,sizeof(
struct Cyc_Rope_Rope_node*));{unsigned int i=0;for(0;i < sz;i ++){total +=Cyc_Rope_length(((
struct Cyc_Rope_Rope_node**)_tmp13.curr)[(int)i]);}}return total;}_LL0:;}static
unsigned int Cyc_Rope_flatten_it(struct _tagged_arr s,unsigned int i,struct Cyc_Rope_Rope_node*
r){void*_tmp14=(void*)r->v;struct _tagged_arr _tmp15;struct _tagged_arr _tmp16;_LL8:
if(*((int*)_tmp14)== 0){_LLC: _tmp15=((struct Cyc_Rope_String_rope_struct*)_tmp14)->f1;
goto _LL9;}else{goto _LLA;}_LLA: if(*((int*)_tmp14)== 1){_LLD: _tmp16=((struct Cyc_Rope_Array_rope_struct*)
_tmp14)->f1;goto _LLB;}else{goto _LL7;}_LL9: {unsigned int _tmp17=Cyc_Std_strlen(
_tmp15);Cyc_Std_strncpy(_tagged_arr_plus(s,sizeof(unsigned char),(int)i),_tmp15,
_tmp17);return i + _tmp17;}_LLB: {unsigned int _tmp18=_get_arr_size(_tmp16,sizeof(
struct Cyc_Rope_Rope_node*));{int j=0;for(0;j < _tmp18;j ++){i=Cyc_Rope_flatten_it(s,
i,((struct Cyc_Rope_Rope_node**)_tmp16.curr)[j]);}}return i;}_LL7:;}struct
_tagged_arr Cyc_Rope_to_string(struct Cyc_Rope_Rope_node*r){struct _tagged_arr s=Cyc_Core_new_string(
Cyc_Rope_length(r));Cyc_Rope_flatten_it(s,0,r);(void*)(r->v=(void*)((void*)({
struct Cyc_Rope_String_rope_struct*_tmp19=_cycalloc(sizeof(struct Cyc_Rope_String_rope_struct));
_tmp19[0]=({struct Cyc_Rope_String_rope_struct _tmp1A;_tmp1A.tag=0;_tmp1A.f1=(
struct _tagged_arr)s;_tmp1A;});_tmp19;})));return s;}int Cyc_Rope_cmp(struct Cyc_Rope_Rope_node*
r1,struct Cyc_Rope_Rope_node*r2){return Cyc_Std_strcmp((struct _tagged_arr)Cyc_Rope_to_string(
r1),(struct _tagged_arr)Cyc_Rope_to_string(r2));}
