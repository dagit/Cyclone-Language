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
  unsigned total_bytes;
  unsigned free_bytes;
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
extern void   _reset_region(struct _RegionHandle *);

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
extern int _throw(void* e);
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
#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#define _check_null(ptr) \
  ({ void*_check_null_temp = (void*)(ptr); \
     if (!_check_null_temp) _throw_null(); \
     _check_null_temp; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  ((char *)ptr) + (elt_sz)*(index); })
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  void*_cks_ptr = (void*)(ptr); \
  unsigned _cks_bound = (bound); \
  unsigned _cks_elt_sz = (elt_sz); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (!_cks_index >= _cks_bound) _throw_arraybounds(); \
  ((char *)_cks_ptr) + _cks_elt_sz*_cks_index; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_notnull(bound,index) (index)
#else
#define _check_known_subscript_notnull(bound,index) ({ \
  unsigned _cksnn_bound = (bound); \
  unsigned _cksnn_index = (index); \
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); \
  _cksnn_index; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_unknown_subscript(arr,elt_sz,index) ({ \
  struct _tagged_arr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  _cus_ans; })
#else
#define _check_unknown_subscript(arr,elt_sz,index) ({ \
  struct _tagged_arr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  if (!_cus_arr.base) _throw_null(); \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#endif

#define _tag_arr(tcurr,elt_sz,num_elts) ({ \
  struct _tagged_arr _tag_arr_ans; \
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })

#define _init_tag_arr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _tagged_arr *_itarr_ptr = (arr_ptr); \
  void* _itarr = (arr); \
  _itarr_ptr->base = _itarr_ptr->curr = _itarr; \
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts); \
  _itarr_ptr; })

#ifdef NO_CYC_BOUNDS_CHECKS
#define _untag_arr(arr,elt_sz,num_elts) ((arr).curr)
#else
#define _untag_arr(arr,elt_sz,num_elts) ({ \
  struct _tagged_arr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)\
    _throw_arraybounds(); \
  _curr; })
#endif

#define _get_arr_size(arr,elt_sz) \
  ({struct _tagged_arr _get_arr_size_temp = (arr); \
    unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr; \
    unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one; \
    (_get_arr_size_curr < _get_arr_size_temp.base || \
     _get_arr_size_curr >= _get_arr_size_last) ? 0 : \
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));})

#define _tagged_arr_plus(arr,elt_sz,change) ({ \
  struct _tagged_arr _ans = (arr); \
  _ans.curr += ((int)(elt_sz))*(change); \
  _ans; })

#define _tagged_arr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _tagged_arr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  *_arr_ptr; })

#define _tagged_arr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _tagged_arr * _arr_ptr = (arr_ptr); \
  struct _tagged_arr _ans = *_arr_ptr; \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  _ans; })

// Decrease the upper bound on a fat pointer by numelts where sz is
// the size of the pointer's type.  Note that this can't be a macro
// if we're to get initializers right.
static struct _tagged_arr _tagged_ptr_decrease_size(struct _tagged_arr x,
                                                    unsigned int sz,
                                                    unsigned int numelts) {
  x.last_plus_one -= sz * numelts; 
  return x; 
}

// Add i to zero-terminated pointer x.  Checks for x being null and
// ensures that x[0..i-1] are not 0.
#define _zero_arr_plus(orig_x,orig_sz,orig_i) ({ \
  typedef _czs_tx = (*orig_x); \
  _czs_tx *_czs_x = (_czs_tx *)(orig_x); \
  unsigned int _czs_sz = (orig_sz); \
  int _czs_i = (orig_i); \
  unsigned int _czs_temp; \
  if ((_czs_x) == 0) _throw_null(); \
  if (_czs_i < 0) _throw_arraybounds(); \
  for (_czs_temp=_czs_sz; _czs_temp < _czs_i; _czs_temp++) \
    if (_czs_x[_czs_temp] == 0) _throw_arraybounds(); \
  _czs_x+_czs_i; })

// Calculates the number of elements in a zero-terminated, thin array.
// If non-null, the array is guaranteed to have orig_offset elements.
#define _get_zero_arr_size(orig_x,orig_offset) ({ \
  typedef _gres_tx = (*orig_x); \
  _gres_tx *_gres_x = (_gres_tx *)(orig_x); \
  unsigned int _gres_offset = (orig_offset); \
  unsigned int _gres = 0; \
  if (_gres_x != 0) { \
     _gres = _gres_offset; \
     _gres_x += _gres_offset - 1; \
     while (*_gres_x != 0) { _gres_x++; _gres++; } \
  } _gres; })

// Does in-place addition of a zero-terminated pointer (x += e and ++x).  
// Note that this expands to call _zero_arr_plus.
#define _zero_arr_inplace_plus(x,orig_i) ({ \
  typedef _zap_tx = (*x); \
  _zap_tx **_zap_x = &((_zap_tx*)x); \
  *_zap_x = _zero_arr_plus(*_zap_x,1,(orig_i)); })

// Does in-place increment of a zero-terminated pointer (e.g., x++).
// Note that this expands to call _zero_arr_plus.
#define _zero_arr_inplace_plus_post(x,orig_i) ({ \
  typedef _zap_tx = (*x); \
  _zap_tx **_zap_x = &((_zap_tx*)x); \
  _zap_tx *_zap_res = *_zap_x; \
  *_zap_x = _zero_arr_plus(_zap_res,1,(orig_i)); \
  _zap_res; })
  
//// Allocation
extern void* GC_malloc(int);
extern void* GC_malloc_atomic(int);
extern void* GC_calloc(unsigned,unsigned);
extern void* GC_calloc_atomic(unsigned,unsigned);

static inline void* _cycalloc(int n) {
  void * ans = (void *)GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void* _cycalloc_atomic(int n) {
  void * ans = (void *)GC_malloc_atomic(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void* _cyccalloc(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
static inline void* _cyccalloc_atomic(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc_atomic(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
#define MAX_MALLOC_SIZE (1 << 28)
static inline unsigned int _check_times(unsigned x, unsigned y) {
  unsigned long long whole_ans = 
    ((unsigned long long)x)*((unsigned long long)y);
  unsigned word_ans = (unsigned)whole_ans;
  if(word_ans < whole_ans || word_ans > MAX_MALLOC_SIZE)
    _throw_badalloc();
  return word_ans;
}

#if defined(CYC_REGION_PROFILE) 
extern void* _profile_GC_malloc(int,char *file,int lineno);
extern void* _profile_GC_malloc_atomic(int,char *file,int lineno);
extern void* _profile_region_malloc(struct _RegionHandle *, unsigned,
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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;typedef struct{int __count;union{
unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef struct{int __pos;
Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;
extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_ShortPtr_pa_struct{int tag;
short*f1;};struct Cyc_Buffer_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct _tagged_arr Cyc_aprintf(struct _tagged_arr,struct
_tagged_arr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _tagged_arr,struct
_tagged_arr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _tagged_arr
f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};int
Cyc_printf(struct _tagged_arr,struct _tagged_arr);int Cyc_putc(int,struct Cyc___cycFILE*);
struct _tagged_arr Cyc_rprintf(struct _RegionHandle*,struct _tagged_arr,struct
_tagged_arr);int Cyc_snprintf(struct _tagged_arr,unsigned int,struct _tagged_arr,
struct _tagged_arr);int Cyc_sprintf(struct _tagged_arr,struct _tagged_arr,struct
_tagged_arr);int Cyc_vfprintf(struct Cyc___cycFILE*,struct _tagged_arr,struct
_tagged_arr);int Cyc_vprintf(struct _tagged_arr,struct _tagged_arr);struct
_tagged_arr Cyc_vrprintf(struct _RegionHandle*,struct _tagged_arr,struct _tagged_arr);
int Cyc_vsnprintf(struct _tagged_arr,unsigned int,struct _tagged_arr,struct
_tagged_arr);int Cyc_vsprintf(struct _tagged_arr,struct _tagged_arr,struct
_tagged_arr);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];
struct Cyc_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[18];extern char
Cyc_List_Nth[8];unsigned int Cyc_strlen(struct _tagged_arr s);typedef struct{int quot;
int rem;}Cyc_div_t;typedef struct{int quot;int rem;}Cyc_ldiv_t;double modf(double,
double*);static struct _tagged_arr Cyc_parg2string(void*x){void*_tmp0=x;_LL1: if(*((
int*)_tmp0)!= 0)goto _LL3;_LL2: return({const char*_tmp1="string";_tag_arr(_tmp1,
sizeof(char),_get_zero_arr_size(_tmp1,7));});_LL3: if(*((int*)_tmp0)!= 1)goto _LL5;
_LL4: return({const char*_tmp2="int";_tag_arr(_tmp2,sizeof(char),
_get_zero_arr_size(_tmp2,4));});_LL5: if(*((int*)_tmp0)!= 2)goto _LL7;_LL6: return({
const char*_tmp3="double";_tag_arr(_tmp3,sizeof(char),_get_zero_arr_size(_tmp3,7));});
_LL7: if(*((int*)_tmp0)!= 3)goto _LL9;_LL8: return({const char*_tmp4="short *";
_tag_arr(_tmp4,sizeof(char),_get_zero_arr_size(_tmp4,8));});_LL9: if(*((int*)
_tmp0)!= 4)goto _LLB;_LLA: return({const char*_tmp5="buffer";_tag_arr(_tmp5,sizeof(
char),_get_zero_arr_size(_tmp5,7));});_LLB: if(*((int*)_tmp0)!= 5)goto _LL0;_LLC:
return({const char*_tmp6="unsigned long *";_tag_arr(_tmp6,sizeof(char),
_get_zero_arr_size(_tmp6,16));});_LL0:;}static void*Cyc_badarg(struct _tagged_arr s){
return(void*)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp7=
_cycalloc(sizeof(*_tmp7));_tmp7[0]=({struct Cyc_Core_Invalid_argument_struct _tmp8;
_tmp8.tag=Cyc_Core_Invalid_argument;_tmp8.f1=s;_tmp8;});_tmp7;}));}static int Cyc_va_arg_int(
struct _tagged_arr ap){void*_tmp9=*((void**)_check_unknown_subscript(ap,sizeof(
void*),0));unsigned int _tmpA;_LLE: if(*((int*)_tmp9)!= 1)goto _LL10;_tmpA=((struct
Cyc_Int_pa_struct*)_tmp9)->f1;_LLF: return(int)_tmpA;_LL10:;_LL11: return((int(*)(
struct _tagged_arr s))Cyc_badarg)(({const char*_tmpB="printf expected int";_tag_arr(
_tmpB,sizeof(char),_get_zero_arr_size(_tmpB,20));}));_LLD:;}static int Cyc_va_arg_long(
struct _tagged_arr ap){void*_tmpC=*((void**)_check_unknown_subscript(ap,sizeof(
void*),0));unsigned int _tmpD;_LL13: if(*((int*)_tmpC)!= 1)goto _LL15;_tmpD=((
struct Cyc_Int_pa_struct*)_tmpC)->f1;_LL14: return(int)_tmpD;_LL15:;_LL16: return((
int(*)(struct _tagged_arr s))Cyc_badarg)(({const char*_tmpE="printf expected int";
_tag_arr(_tmpE,sizeof(char),_get_zero_arr_size(_tmpE,20));}));_LL12:;}static
unsigned int Cyc_va_arg_ulong(struct _tagged_arr ap){void*_tmpF=*((void**)
_check_unknown_subscript(ap,sizeof(void*),0));unsigned int _tmp10;_LL18: if(*((int*)
_tmpF)!= 1)goto _LL1A;_tmp10=((struct Cyc_Int_pa_struct*)_tmpF)->f1;_LL19: return(
unsigned int)_tmp10;_LL1A:;_LL1B: return((unsigned int(*)(struct _tagged_arr s))Cyc_badarg)(({
const char*_tmp11="printf expected int";_tag_arr(_tmp11,sizeof(char),
_get_zero_arr_size(_tmp11,20));}));_LL17:;}static unsigned int Cyc_va_arg_uint(
struct _tagged_arr ap){void*_tmp12=*((void**)_check_unknown_subscript(ap,sizeof(
void*),0));unsigned int _tmp13;_LL1D: if(*((int*)_tmp12)!= 1)goto _LL1F;_tmp13=((
struct Cyc_Int_pa_struct*)_tmp12)->f1;_LL1E: return(unsigned int)_tmp13;_LL1F:;
_LL20: return((unsigned int(*)(struct _tagged_arr s))Cyc_badarg)(({const char*_tmp14="printf expected int";
_tag_arr(_tmp14,sizeof(char),_get_zero_arr_size(_tmp14,20));}));_LL1C:;}static
double Cyc_va_arg_double(struct _tagged_arr ap){void*_tmp15=*((void**)
_check_unknown_subscript(ap,sizeof(void*),0));double _tmp16;_LL22: if(*((int*)
_tmp15)!= 2)goto _LL24;_tmp16=((struct Cyc_Double_pa_struct*)_tmp15)->f1;_LL23:
return _tmp16;_LL24:;_LL25:(int)_throw(Cyc_badarg((struct _tagged_arr)({struct Cyc_String_pa_struct
_tmp19;_tmp19.tag=0;_tmp19.f1=(struct _tagged_arr)Cyc_parg2string(*((void**)
_check_unknown_subscript(ap,sizeof(void*),0)));{void*_tmp17[1]={& _tmp19};Cyc_aprintf(({
const char*_tmp18="printf expected double but found %s";_tag_arr(_tmp18,sizeof(
char),_get_zero_arr_size(_tmp18,36));}),_tag_arr(_tmp17,sizeof(void*),1));}})));
_LL21:;}static short*Cyc_va_arg_short_ptr(struct _tagged_arr ap){void*_tmp1A=*((
void**)_check_unknown_subscript(ap,sizeof(void*),0));short*_tmp1B;_LL27: if(*((
int*)_tmp1A)!= 3)goto _LL29;_tmp1B=((struct Cyc_ShortPtr_pa_struct*)_tmp1A)->f1;
_LL28: return _tmp1B;_LL29:;_LL2A:(int)_throw(Cyc_badarg(({const char*_tmp1C="printf expected short pointer";
_tag_arr(_tmp1C,sizeof(char),_get_zero_arr_size(_tmp1C,30));})));_LL26:;}static
unsigned int*Cyc_va_arg_int_ptr(struct _tagged_arr ap){void*_tmp1D=*((void**)
_check_unknown_subscript(ap,sizeof(void*),0));unsigned int*_tmp1E;_LL2C: if(*((
int*)_tmp1D)!= 5)goto _LL2E;_tmp1E=((struct Cyc_IntPtr_pa_struct*)_tmp1D)->f1;
_LL2D: return _tmp1E;_LL2E:;_LL2F:(int)_throw(Cyc_badarg(({const char*_tmp1F="printf expected int pointer";
_tag_arr(_tmp1F,sizeof(char),_get_zero_arr_size(_tmp1F,28));})));_LL2B:;}static
struct _tagged_arr Cyc_va_arg_string(struct _tagged_arr ap){void*_tmp20=*((void**)
_check_unknown_subscript(ap,sizeof(void*),0));struct _tagged_arr _tmp21;_LL31: if(*((
int*)_tmp20)!= 0)goto _LL33;_tmp21=((struct Cyc_String_pa_struct*)_tmp20)->f1;
_LL32: return _tmp21;_LL33:;_LL34:(int)_throw(Cyc_badarg(({const char*_tmp22="printf expected string";
_tag_arr(_tmp22,sizeof(char),_get_zero_arr_size(_tmp22,23));})));_LL30:;}int Cyc___cvt_double(
double number,int prec,int flags,int*signp,int fmtch,struct _tagged_arr startp,struct
_tagged_arr endp);enum Cyc_BASE{Cyc_OCT  = 0,Cyc_DEC  = 1,Cyc_HEX  = 2};static int Cyc__IO_sputn(
int(*ioputc)(int,void*),void*ioputc_env,struct _tagged_arr s,int howmany){int i=0;
while(i < howmany){if(_get_arr_size(s,sizeof(char))== 0?1: ioputc((int)*((const
char*)_check_unknown_subscript(s,sizeof(char),0)),ioputc_env)== - 1)return i;
_tagged_arr_inplace_plus_post(& s,sizeof(char),1);i ++;}return i;}static int Cyc__IO_nzsputn(
int(*ioputc)(int,void*),void*ioputc_env,struct _tagged_arr s,int howmany){int i=0;
while(i < howmany){char c;if((_get_arr_size(s,sizeof(char))== 0?1:(c=*((const char*)
_check_unknown_subscript(s,sizeof(char),0)))== 0)?1: ioputc((int)c,ioputc_env)== 
- 1)return i;_tagged_arr_inplace_plus_post(& s,sizeof(char),1);i ++;}return i;}static
int Cyc__IO_padn(int(*ioputc)(int,void*),void*ioputc_env,char c,int howmany){int i=0;
while(i < howmany){if(ioputc((int)c,ioputc_env)== - 1)return i;i ++;}return i;}static
struct _tagged_arr Cyc_my_memchr(struct _tagged_arr s,char c,int n){int sz=(int)
_get_arr_size(s,sizeof(char));n=n < sz?n: sz;for(0;n != 0;(n --,
_tagged_arr_inplace_plus_post(& s,sizeof(char),1))){if(*((const char*)
_check_unknown_subscript(s,sizeof(char),0))== c)return s;}return(struct
_tagged_arr)_tag_arr(0,0,0);}static struct _tagged_arr Cyc_my_nzmemchr(struct
_tagged_arr s,char c,int n){int sz=(int)_get_arr_size(s,sizeof(char));n=n < sz?n: sz;
for(0;n != 0;(n --,_tagged_arr_inplace_plus_post(& s,sizeof(char),1))){if(*((const
char*)_check_unknown_subscript(s,sizeof(char),0))== c)return s;}return(struct
_tagged_arr)_tag_arr(0,0,0);}static unsigned int Cyc_my_strlen(struct _tagged_arr s){
unsigned int sz=_get_arr_size(s,sizeof(char));unsigned int i=0;while(i < sz?((const
char*)s.curr)[(int)i]!= 0: 0){i ++;}return i;}int Cyc__IO_vfprintf(int(*ioputc)(int,
void*),void*ioputc_env,struct _tagged_arr fmt0,struct _tagged_arr ap){struct
_tagged_arr fmt;register int ch;register int n;struct _tagged_arr cp=_tag_arr(0,0,0);
struct _tagged_arr cp2=(struct _tagged_arr)_tag_arr(0,0,0);struct _tagged_arr cp3=(
struct _tagged_arr)_tag_arr(0,0,0);int which_cp;struct _tagged_arr fmark;register int
flags;int ret;int width;int prec;char sign;char sign_string[2]={'\000','\000'};int
softsign=0;double _double;int fpprec;unsigned int _ulong;int dprec;int dpad;int
fieldsz;int size=0;char buf[349];{unsigned int _tmp56=348;unsigned int i;for(i=0;i < 
_tmp56;i ++){buf[i]='\000';}buf[_tmp56]=(char)0;}{char ox[2]={'\000','\000'};enum 
Cyc_BASE base;fmt=fmt0;ret=0;for(0;1;0){fmark=fmt;{unsigned int fmt_sz=
_get_arr_size(fmt,sizeof(char));for(n=0;(n < fmt_sz?(ch=(int)((const char*)fmt.curr)[
n])!= '\000': 0)?ch != '%': 0;n ++){;}fmt=_tagged_arr_plus(fmt,sizeof(char),n);if((n=(
fmt.curr - fmark.curr)/ sizeof(char))!= 0){do{if(Cyc__IO_sputn(ioputc,ioputc_env,
fmark,n)!= n)goto error;}while(0);ret +=n;}if(ch == '\000')goto done;
_tagged_arr_inplace_plus_post(& fmt,sizeof(char),1);flags=0;dprec=0;fpprec=0;
width=0;prec=- 1;sign='\000';rflag: ch=(int)*((const char*)_check_unknown_subscript(
_tagged_arr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0));reswitch:
which_cp=0;switch(ch){case ' ': _LL35: if(!((int)sign))sign=' ';goto rflag;case '#':
_LL36: flags |=8;goto rflag;case '*': _LL37: width=Cyc_va_arg_int(ap);
_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);if(width >= 0)goto rflag;width=
- width;goto _LL38;case '-': _LL38: flags |=16;flags &=~ 32;goto rflag;case '+': _LL39: sign='+';
goto rflag;case '.': _LL3A: if((ch=(int)*((const char*)_check_unknown_subscript(
_tagged_arr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0)))== '*'){n=
Cyc_va_arg_int(ap);_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);prec=n < 0?
- 1: n;goto rflag;}n=0;while((unsigned int)(ch - '0')<= 9){n=10 * n + (ch - '0');ch=(
int)*((const char*)_check_unknown_subscript(_tagged_arr_inplace_plus_post(& fmt,
sizeof(char),1),sizeof(char),0));}prec=n < 0?- 1: n;goto reswitch;case '0': _LL3B: if(!(
flags & 16))flags |=32;goto rflag;case '1': _LL3C: goto _LL3D;case '2': _LL3D: goto _LL3E;
case '3': _LL3E: goto _LL3F;case '4': _LL3F: goto _LL40;case '5': _LL40: goto _LL41;case '6':
_LL41: goto _LL42;case '7': _LL42: goto _LL43;case '8': _LL43: goto _LL44;case '9': _LL44: n=
0;do{n=10 * n + (ch - '0');ch=(int)*((const char*)_check_unknown_subscript(
_tagged_arr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0));}while((
unsigned int)(ch - '0')<= 9);width=n;goto reswitch;case 'L': _LL45: flags |=2;goto
rflag;case 'h': _LL46: flags |=4;goto rflag;case 'l': _LL47: flags |=1;goto rflag;case 'c':
_LL48: cp=({char*_tmp23=buf;_tag_arr(_tmp23,sizeof(char),_get_zero_arr_size(
_tmp23,349));});({struct _tagged_arr _tmp24=cp;char _tmp25=*((char*)
_check_unknown_subscript(_tmp24,sizeof(char),0));char _tmp26=(char)Cyc_va_arg_int(
ap);if(_get_arr_size(_tmp24,sizeof(char))== 1?_tmp25 == '\000'?_tmp26 != '\000': 0:
0)_throw_arraybounds();*((char*)_tmp24.curr)=_tmp26;});
_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);size=1;sign='\000';break;case
'D': _LL49: flags |=1;goto _LL4A;case 'd': _LL4A: goto _LL4B;case 'i': _LL4B: _ulong=(
unsigned int)(flags & 1?Cyc_va_arg_long(ap):(flags & 4?(int)((short)Cyc_va_arg_int(
ap)):(int)Cyc_va_arg_int(ap)));_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);
if((int)_ulong < 0){_ulong=- _ulong;sign='-';}base=Cyc_DEC;goto number;case 'e':
_LL4C: goto _LL4D;case 'E': _LL4D: goto _LL4E;case 'f': _LL4E: goto _LL4F;case 'F': _LL4F:
goto _LL50;case 'g': _LL50: goto _LL51;case 'G': _LL51: _double=Cyc_va_arg_double(ap);
_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);if(prec > 39){if((ch != 'g'?ch
!= 'G': 0)?1: flags & 8)fpprec=prec - 39;prec=39;}else{if(prec == - 1)prec=6;}cp=({
char*_tmp27=buf;_tag_arr(_tmp27,sizeof(char),_get_zero_arr_size(_tmp27,349));});({
struct _tagged_arr _tmp28=cp;char _tmp29=*((char*)_check_unknown_subscript(_tmp28,
sizeof(char),0));char _tmp2A='\000';if(_get_arr_size(_tmp28,sizeof(char))== 1?
_tmp29 == '\000'?_tmp2A != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp28.curr)=
_tmp2A;});size=Cyc___cvt_double(_double,prec,flags,& softsign,ch,cp,
_tagged_arr_plus(({char*_tmp2B=buf;_tag_arr(_tmp2B,sizeof(char),
_get_zero_arr_size(_tmp2B,349));}),sizeof(char),(int)(sizeof(buf)- 1)));if(
softsign)sign='-';if(*((char*)_check_unknown_subscript(cp,sizeof(char),0))== '\000')
_tagged_arr_inplace_plus_post(& cp,sizeof(char),1);break;case 'n': _LL52: if(flags & 
1)*((unsigned int*)Cyc_va_arg_int_ptr(ap))=(unsigned int)ret;else{if(flags & 4)*((
short*)Cyc_va_arg_short_ptr(ap))=(short)ret;else{*((unsigned int*)Cyc_va_arg_int_ptr(
ap))=(unsigned int)ret;}}_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);
continue;case 'O': _LL53: flags |=1;goto _LL54;case 'o': _LL54: _ulong=flags & 1?Cyc_va_arg_ulong(
ap):(flags & 4?(unsigned int)((unsigned short)Cyc_va_arg_int(ap)):(unsigned int)
Cyc_va_arg_uint(ap));_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);base=Cyc_OCT;
goto nosign;case 'p': _LL55: _ulong=(unsigned int)Cyc_va_arg_long(ap);
_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);base=Cyc_HEX;flags |=64;ch=(
int)'x';goto nosign;case 's': _LL56:{void*_tmp2C=*((void**)_check_unknown_subscript(
ap,sizeof(void*),0));struct _tagged_arr _tmp2D;struct _tagged_arr _tmp2E;_LL59: if(*((
int*)_tmp2C)!= 0)goto _LL5B;_tmp2D=((struct Cyc_String_pa_struct*)_tmp2C)->f1;
_LL5A: which_cp=2;cp2=_tmp2D;if(cp2.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr)
cp2=({const char*_tmp2F="(NULL)";_tag_arr(_tmp2F,sizeof(char),_get_zero_arr_size(
_tmp2F,7));});if(prec >= 0){struct _tagged_arr p=Cyc_my_memchr(cp2,'\000',prec);if(
p.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr){size=(p.curr - cp2.curr)/ 
sizeof(char);if(size > prec)size=prec;}else{size=prec;}}else{size=(int)Cyc_strlen((
struct _tagged_arr)cp2);}goto _LL58;_LL5B: if(*((int*)_tmp2C)!= 4)goto _LL5D;_tmp2E=((
struct Cyc_Buffer_pa_struct*)_tmp2C)->f1;_LL5C: which_cp=3;cp3=_tmp2E;if(prec >= 0){
struct _tagged_arr p=Cyc_my_nzmemchr(cp3,'\000',prec);if(p.curr != ((struct
_tagged_arr)_tag_arr(0,0,0)).curr){size=(p.curr - cp3.curr)/ sizeof(char);if(size
> prec)size=prec;}else{size=prec;}}else{size=(int)Cyc_my_strlen(cp3);}goto _LL58;
_LL5D:;_LL5E:(int)_throw(Cyc_badarg(({const char*_tmp30="printf expected string";
_tag_arr(_tmp30,sizeof(char),_get_zero_arr_size(_tmp30,23));})));_LL58:;}
_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);sign='\000';break;case 'U':
_LL57: flags |=1;goto _LL5F;case 'u': _LL5F: _ulong=flags & 1?Cyc_va_arg_ulong(ap):(
flags & 4?(unsigned int)((unsigned short)Cyc_va_arg_int(ap)):(unsigned int)Cyc_va_arg_uint(
ap));_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);base=Cyc_DEC;goto nosign;
case 'X': _LL60: goto _LL61;case 'x': _LL61: _ulong=flags & 1?Cyc_va_arg_ulong(ap):(
flags & 4?(unsigned int)((unsigned short)Cyc_va_arg_int(ap)):(unsigned int)Cyc_va_arg_uint(
ap));_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);base=Cyc_HEX;if(flags & 8?
_ulong != 0: 0)flags |=64;nosign: sign='\000';number: if((dprec=prec)>= 0)flags &=~ 32;
cp=_tagged_arr_plus(({char*_tmp31=buf;_tag_arr(_tmp31,sizeof(char),
_get_zero_arr_size(_tmp31,349));}),sizeof(char),(308 + 39)+ 1);if(_ulong != 0?1:
prec != 0){struct _tagged_arr xdigs;switch(base){case Cyc_OCT: _LL63: do{({struct
_tagged_arr _tmp32=_tagged_arr_inplace_plus(& cp,sizeof(char),-1);char _tmp33=*((
char*)_check_unknown_subscript(_tmp32,sizeof(char),0));char _tmp34=(char)((_ulong
& 7)+ '0');if(_get_arr_size(_tmp32,sizeof(char))== 1?_tmp33 == '\000'?_tmp34 != '\000':
0: 0)_throw_arraybounds();*((char*)_tmp32.curr)=_tmp34;});_ulong >>=3;}while((int)
_ulong);if(flags & 8?*((char*)_check_unknown_subscript(cp,sizeof(char),0))!= '0':
0)({struct _tagged_arr _tmp35=_tagged_arr_inplace_plus(& cp,sizeof(char),-1);char
_tmp36=*((char*)_check_unknown_subscript(_tmp35,sizeof(char),0));char _tmp37='0';
if(_get_arr_size(_tmp35,sizeof(char))== 1?_tmp36 == '\000'?_tmp37 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp35.curr)=_tmp37;});break;case Cyc_DEC: _LL64:
while(_ulong >= 10){({struct _tagged_arr _tmp38=_tagged_arr_inplace_plus(& cp,
sizeof(char),-1);char _tmp39=*((char*)_check_unknown_subscript(_tmp38,sizeof(char),
0));char _tmp3A=(char)(_ulong % 10 + '0');if(_get_arr_size(_tmp38,sizeof(char))== 1?
_tmp39 == '\000'?_tmp3A != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp38.curr)=
_tmp3A;});_ulong /=10;}({struct _tagged_arr _tmp3B=_tagged_arr_inplace_plus(& cp,
sizeof(char),-1);char _tmp3C=*((char*)_check_unknown_subscript(_tmp3B,sizeof(char),
0));char _tmp3D=(char)(_ulong + '0');if(_get_arr_size(_tmp3B,sizeof(char))== 1?
_tmp3C == '\000'?_tmp3D != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp3B.curr)=
_tmp3D;});break;case Cyc_HEX: _LL65: if(ch == 'X')xdigs=({const char*_tmp3E="0123456789ABCDEF";
_tag_arr(_tmp3E,sizeof(char),_get_zero_arr_size(_tmp3E,17));});else{xdigs=({
const char*_tmp3F="0123456789abcdef";_tag_arr(_tmp3F,sizeof(char),
_get_zero_arr_size(_tmp3F,17));});}do{({struct _tagged_arr _tmp40=
_tagged_arr_inplace_plus(& cp,sizeof(char),-1);char _tmp41=*((char*)
_check_unknown_subscript(_tmp40,sizeof(char),0));char _tmp42=*((const char*)
_check_unknown_subscript(xdigs,sizeof(char),(int)(_ulong & 15)));if(_get_arr_size(
_tmp40,sizeof(char))== 1?_tmp41 == '\000'?_tmp42 != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp40.curr)=_tmp42;});_ulong >>=4;}while((int)_ulong);break;}}size=((
_tagged_arr_plus(({char*_tmp43=buf;_tag_arr(_tmp43,sizeof(char),
_get_zero_arr_size(_tmp43,349));}),sizeof(char),(308 + 39)+ 1)).curr - cp.curr)/ 
sizeof(char);skipsize: break;default: _LL62: if(ch == '\000')goto done;cp=({char*
_tmp44=buf;_tag_arr(_tmp44,sizeof(char),_get_zero_arr_size(_tmp44,349));});({
struct _tagged_arr _tmp45=cp;char _tmp46=*((char*)_check_unknown_subscript(_tmp45,
sizeof(char),0));char _tmp47=(char)ch;if(_get_arr_size(_tmp45,sizeof(char))== 1?
_tmp46 == '\000'?_tmp47 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp45.curr)=
_tmp47;});size=1;sign='\000';break;}fieldsz=size + fpprec;dpad=dprec - size;if(
dpad < 0)dpad=0;if((int)sign)fieldsz ++;else{if(flags & 64)fieldsz +=2;}fieldsz +=
dpad;if((flags & (16 | 32))== 0){if(Cyc__IO_padn(ioputc,ioputc_env,' ',width - 
fieldsz)< width - fieldsz)goto error;}if((int)sign){({struct _tagged_arr _tmp49=
_tagged_arr_plus(({char*_tmp48=sign_string;_tag_arr(_tmp48,sizeof(char),
_get_zero_arr_size(_tmp48,2));}),sizeof(char),0);char _tmp4A=*((char*)
_check_unknown_subscript(_tmp49,sizeof(char),0));char _tmp4B=sign;if(
_get_arr_size(_tmp49,sizeof(char))== 1?_tmp4A == '\000'?_tmp4B != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp49.curr)=_tmp4B;});do{if(Cyc__IO_sputn(ioputc,
ioputc_env,(struct _tagged_arr)({char*_tmp4C=sign_string;_tag_arr(_tmp4C,sizeof(
char),_get_zero_arr_size(_tmp4C,2));}),1)!= 1)goto error;}while(0);}else{if(flags
& 64){({struct _tagged_arr _tmp4E=_tagged_arr_plus(({char*_tmp4D=ox;_tag_arr(
_tmp4D,sizeof(char),_get_zero_arr_size(_tmp4D,2));}),sizeof(char),0);char _tmp4F=*((
char*)_check_unknown_subscript(_tmp4E,sizeof(char),0));char _tmp50='0';if(
_get_arr_size(_tmp4E,sizeof(char))== 1?_tmp4F == '\000'?_tmp50 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp4E.curr)=_tmp50;});({struct _tagged_arr _tmp52=
_tagged_arr_plus(({char*_tmp51=ox;_tag_arr(_tmp51,sizeof(char),
_get_zero_arr_size(_tmp51,2));}),sizeof(char),1);char _tmp53=*((char*)
_check_unknown_subscript(_tmp52,sizeof(char),0));char _tmp54=(char)ch;if(
_get_arr_size(_tmp52,sizeof(char))== 1?_tmp53 == '\000'?_tmp54 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp52.curr)=_tmp54;});do{if(Cyc__IO_sputn(ioputc,
ioputc_env,({char*_tmp55=ox;_tag_arr(_tmp55,sizeof(char),_get_zero_arr_size(
_tmp55,2));}),2)!= 2)goto error;}while(0);}}if((flags & (16 | 32))== 32){if(Cyc__IO_padn(
ioputc,ioputc_env,'0',width - fieldsz)< width - fieldsz)goto error;}if(Cyc__IO_padn(
ioputc,ioputc_env,'0',dpad)< dpad)goto error;if(which_cp == 0)do{if(Cyc__IO_sputn(
ioputc,ioputc_env,(struct _tagged_arr)cp,size)!= size)goto error;}while(0);else{
if(which_cp == 2)do{if(Cyc__IO_sputn(ioputc,ioputc_env,cp2,size)!= size)goto error;}while(
0);else{if(which_cp == 3)do{if(Cyc__IO_nzsputn(ioputc,ioputc_env,cp3,size)!= size)
goto error;}while(0);}}if(Cyc__IO_padn(ioputc,ioputc_env,'0',fpprec)< fpprec)goto
error;if(flags & 16){if(Cyc__IO_padn(ioputc,ioputc_env,' ',width - fieldsz)< width
- fieldsz)goto error;}ret +=width > fieldsz?width: fieldsz;}}done: return ret;error:
return - 1;}}static struct _tagged_arr Cyc_exponent(struct _tagged_arr p,int exp,int
fmtch){struct _tagged_arr t;char expbuf[309];{unsigned int _tmp71=308;unsigned int i;
for(i=0;i < _tmp71;i ++){expbuf[i]='0';}expbuf[_tmp71]=(char)0;}({struct
_tagged_arr _tmp57=_tagged_arr_inplace_plus_post(& p,sizeof(char),1);char _tmp58=*((
char*)_check_unknown_subscript(_tmp57,sizeof(char),0));char _tmp59=(char)fmtch;
if(_get_arr_size(_tmp57,sizeof(char))== 1?_tmp58 == '\000'?_tmp59 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp57.curr)=_tmp59;});if(exp < 0){exp=- exp;({
struct _tagged_arr _tmp5A=_tagged_arr_inplace_plus_post(& p,sizeof(char),1);char
_tmp5B=*((char*)_check_unknown_subscript(_tmp5A,sizeof(char),0));char _tmp5C='-';
if(_get_arr_size(_tmp5A,sizeof(char))== 1?_tmp5B == '\000'?_tmp5C != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp5A.curr)=_tmp5C;});}else{({struct _tagged_arr
_tmp5D=_tagged_arr_inplace_plus_post(& p,sizeof(char),1);char _tmp5E=*((char*)
_check_unknown_subscript(_tmp5D,sizeof(char),0));char _tmp5F='+';if(_get_arr_size(
_tmp5D,sizeof(char))== 1?_tmp5E == '\000'?_tmp5F != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp5D.curr)=_tmp5F;});}t=_tagged_arr_plus(({char*_tmp60=expbuf;_tag_arr(
_tmp60,sizeof(char),_get_zero_arr_size(_tmp60,309));}),sizeof(char),308);if(exp > 
9){do{({struct _tagged_arr _tmp61=_tagged_arr_inplace_plus(& t,sizeof(char),-1);
char _tmp62=*((char*)_check_unknown_subscript(_tmp61,sizeof(char),0));char _tmp63=(
char)(exp % 10 + '0');if(_get_arr_size(_tmp61,sizeof(char))== 1?_tmp62 == '\000'?
_tmp63 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp61.curr)=_tmp63;});}while((
exp /=10)> 9);({struct _tagged_arr _tmp64=_tagged_arr_inplace_plus(& t,sizeof(char),
-1);char _tmp65=*((char*)_check_unknown_subscript(_tmp64,sizeof(char),0));char
_tmp66=(char)(exp + '0');if(_get_arr_size(_tmp64,sizeof(char))== 1?_tmp65 == '\000'?
_tmp66 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp64.curr)=_tmp66;});for(0;t.curr
< (_tagged_arr_plus(({char*_tmp67=expbuf;_tag_arr(_tmp67,sizeof(char),
_get_zero_arr_size(_tmp67,309));}),sizeof(char),308)).curr;({struct _tagged_arr
_tmp68=_tagged_arr_inplace_plus_post(& p,sizeof(char),1);char _tmp69=*((char*)
_check_unknown_subscript(_tmp68,sizeof(char),0));char _tmp6A=*((char*)
_check_unknown_subscript(_tagged_arr_inplace_plus_post(& t,sizeof(char),1),
sizeof(char),0));if(_get_arr_size(_tmp68,sizeof(char))== 1?_tmp69 == '\000'?
_tmp6A != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp68.curr)=_tmp6A;})){;}}
else{({struct _tagged_arr _tmp6B=_tagged_arr_inplace_plus_post(& p,sizeof(char),1);
char _tmp6C=*((char*)_check_unknown_subscript(_tmp6B,sizeof(char),0));char _tmp6D='0';
if(_get_arr_size(_tmp6B,sizeof(char))== 1?_tmp6C == '\000'?_tmp6D != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp6B.curr)=_tmp6D;});({struct _tagged_arr _tmp6E=
_tagged_arr_inplace_plus_post(& p,sizeof(char),1);char _tmp6F=*((char*)
_check_unknown_subscript(_tmp6E,sizeof(char),0));char _tmp70=(char)(exp + '0');if(
_get_arr_size(_tmp6E,sizeof(char))== 1?_tmp6F == '\000'?_tmp70 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp6E.curr)=_tmp70;});}return p;}static struct
_tagged_arr Cyc_round(double fract,int*exp,struct _tagged_arr start,struct
_tagged_arr end,char ch,int*signp){double tmp=(double)0.0;if(fract != 0.0)modf(fract
* 10,& tmp);else{tmp=(double)(ch - '0');}if(tmp > 4)for(0;1;
_tagged_arr_inplace_plus(& end,sizeof(char),-1)){if(*((char*)
_check_unknown_subscript(end,sizeof(char),0))== '.')_tagged_arr_inplace_plus(&
end,sizeof(char),-1);if(({struct _tagged_arr _tmp72=end;char _tmp73=*((char*)
_check_unknown_subscript(_tmp72,sizeof(char),0));char _tmp74=_tmp73 + '\001';if(
_get_arr_size(_tmp72,sizeof(char))== 1?_tmp73 == '\000'?_tmp74 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp72.curr)=_tmp74;})<= '9')break;({struct
_tagged_arr _tmp75=end;char _tmp76=*((char*)_check_unknown_subscript(_tmp75,
sizeof(char),0));char _tmp77='0';if(_get_arr_size(_tmp75,sizeof(char))== 1?_tmp76
== '\000'?_tmp77 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp75.curr)=_tmp77;});
if(end.curr == start.curr){if((unsigned int)exp){({struct _tagged_arr _tmp78=end;
char _tmp79=*((char*)_check_unknown_subscript(_tmp78,sizeof(char),0));char _tmp7A='1';
if(_get_arr_size(_tmp78,sizeof(char))== 1?_tmp79 == '\000'?_tmp7A != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp78.curr)=_tmp7A;});++(*((int*)exp));}else{({
struct _tagged_arr _tmp7B=_tagged_arr_inplace_plus(& end,sizeof(char),-1);char
_tmp7C=*((char*)_check_unknown_subscript(_tmp7B,sizeof(char),0));char _tmp7D='1';
if(_get_arr_size(_tmp7B,sizeof(char))== 1?_tmp7C == '\000'?_tmp7D != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp7B.curr)=_tmp7D;});_tagged_arr_inplace_plus(&
start,sizeof(char),-1);}break;}}else{if(*((int*)signp)== '-')for(0;1;
_tagged_arr_inplace_plus(& end,sizeof(char),-1)){if(*((char*)
_check_unknown_subscript(end,sizeof(char),0))== '.')_tagged_arr_inplace_plus(&
end,sizeof(char),-1);if(*((char*)_check_unknown_subscript(end,sizeof(char),0))!= '0')
break;if(end.curr == start.curr)*((int*)signp)=0;}}return start;}int Cyc___cvt_double(
double number,int prec,int flags,int*signp,int fmtch,struct _tagged_arr startp,struct
_tagged_arr endp){struct _tagged_arr p;struct _tagged_arr t;register double fract;int
dotrim=0;int expcnt;int gformat=0;double integer=(double)0.0;double tmp=(double)0.0;
expcnt=0;if(number < 0){number=- number;*((int*)signp)=(int)'-';}else{*((int*)
signp)=0;}fract=modf(number,& integer);t=_tagged_arr_inplace_plus(& startp,sizeof(
char),1);for(p=_tagged_arr_plus(endp,sizeof(char),- 1);p.curr >= startp.curr?
integer != 0.0: 0;++ expcnt){tmp=modf(integer / 10,& integer);({struct _tagged_arr
_tmp7E=_tagged_arr_inplace_plus_post(& p,sizeof(char),-1);char _tmp7F=*((char*)
_check_unknown_subscript(_tmp7E,sizeof(char),0));char _tmp80=(char)((int)((tmp + 
.01)* 10)+ '0');if(_get_arr_size(_tmp7E,sizeof(char))== 1?_tmp7F == '\000'?_tmp80
!= '\000': 0: 0)_throw_arraybounds();*((char*)_tmp7E.curr)=_tmp80;});}switch(fmtch){
case 'f': _LL68: goto _LL69;case 'F': _LL69: if(expcnt)for(0;(_tagged_arr_inplace_plus(&
p,sizeof(char),1)).curr < endp.curr;({struct _tagged_arr _tmp81=
_tagged_arr_inplace_plus_post(& t,sizeof(char),1);char _tmp82=*((char*)
_check_unknown_subscript(_tmp81,sizeof(char),0));char _tmp83=*((char*)
_check_unknown_subscript(p,sizeof(char),0));if(_get_arr_size(_tmp81,sizeof(char))
== 1?_tmp82 == '\000'?_tmp83 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp81.curr)=
_tmp83;})){;}else{({struct _tagged_arr _tmp84=_tagged_arr_inplace_plus_post(& t,
sizeof(char),1);char _tmp85=*((char*)_check_unknown_subscript(_tmp84,sizeof(char),
0));char _tmp86='0';if(_get_arr_size(_tmp84,sizeof(char))== 1?_tmp85 == '\000'?
_tmp86 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp84.curr)=_tmp86;});}if(
prec?1: flags & 8)({struct _tagged_arr _tmp87=_tagged_arr_inplace_plus_post(& t,
sizeof(char),1);char _tmp88=*((char*)_check_unknown_subscript(_tmp87,sizeof(char),
0));char _tmp89='.';if(_get_arr_size(_tmp87,sizeof(char))== 1?_tmp88 == '\000'?
_tmp89 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp87.curr)=_tmp89;});if(
fract != 0.0){if(prec)do{fract=modf(fract * 10,& tmp);({struct _tagged_arr _tmp8A=
_tagged_arr_inplace_plus_post(& t,sizeof(char),1);char _tmp8B=*((char*)
_check_unknown_subscript(_tmp8A,sizeof(char),0));char _tmp8C=(char)((int)tmp + '0');
if(_get_arr_size(_tmp8A,sizeof(char))== 1?_tmp8B == '\000'?_tmp8C != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp8A.curr)=_tmp8C;});}while(-- prec?fract != 0.0: 0);
if(fract != 0.0)startp=Cyc_round(fract,0,startp,_tagged_arr_plus(t,sizeof(char),-
1),(char)0,signp);}for(0;prec --;({struct _tagged_arr _tmp8D=
_tagged_arr_inplace_plus_post(& t,sizeof(char),1);char _tmp8E=*((char*)
_check_unknown_subscript(_tmp8D,sizeof(char),0));char _tmp8F='0';if(_get_arr_size(
_tmp8D,sizeof(char))== 1?_tmp8E == '\000'?_tmp8F != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp8D.curr)=_tmp8F;})){;}break;case 'e': _LL6A: goto _LL6B;case 'E': _LL6B:
eformat: if(expcnt){({struct _tagged_arr _tmp90=_tagged_arr_inplace_plus_post(& t,
sizeof(char),1);char _tmp91=*((char*)_check_unknown_subscript(_tmp90,sizeof(char),
0));char _tmp92=*((char*)_check_unknown_subscript(_tagged_arr_inplace_plus(& p,
sizeof(char),1),sizeof(char),0));if(_get_arr_size(_tmp90,sizeof(char))== 1?
_tmp91 == '\000'?_tmp92 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp90.curr)=
_tmp92;});if(prec?1: flags & 8)({struct _tagged_arr _tmp93=
_tagged_arr_inplace_plus_post(& t,sizeof(char),1);char _tmp94=*((char*)
_check_unknown_subscript(_tmp93,sizeof(char),0));char _tmp95='.';if(_get_arr_size(
_tmp93,sizeof(char))== 1?_tmp94 == '\000'?_tmp95 != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp93.curr)=_tmp95;});for(0;prec?(_tagged_arr_inplace_plus(& p,sizeof(char),
1)).curr < endp.curr: 0;-- prec){({struct _tagged_arr _tmp96=
_tagged_arr_inplace_plus_post(& t,sizeof(char),1);char _tmp97=*((char*)
_check_unknown_subscript(_tmp96,sizeof(char),0));char _tmp98=*((char*)
_check_unknown_subscript(p,sizeof(char),0));if(_get_arr_size(_tmp96,sizeof(char))
== 1?_tmp97 == '\000'?_tmp98 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp96.curr)=
_tmp98;});}if(!prec?(_tagged_arr_inplace_plus(& p,sizeof(char),1)).curr < endp.curr:
0){fract=(double)0;startp=Cyc_round((double)0,(int*)& expcnt,startp,
_tagged_arr_plus(t,sizeof(char),- 1),*((char*)_check_unknown_subscript(p,sizeof(
char),0)),signp);}-- expcnt;}else{if(fract != 0.0){for(expcnt=- 1;1;-- expcnt){fract=
modf(fract * 10,& tmp);if(tmp != 0.0)break;}({struct _tagged_arr _tmp99=
_tagged_arr_inplace_plus_post(& t,sizeof(char),1);char _tmp9A=*((char*)
_check_unknown_subscript(_tmp99,sizeof(char),0));char _tmp9B=(char)((int)tmp + '0');
if(_get_arr_size(_tmp99,sizeof(char))== 1?_tmp9A == '\000'?_tmp9B != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp99.curr)=_tmp9B;});if(prec?1: flags & 8)({struct
_tagged_arr _tmp9C=_tagged_arr_inplace_plus_post(& t,sizeof(char),1);char _tmp9D=*((
char*)_check_unknown_subscript(_tmp9C,sizeof(char),0));char _tmp9E='.';if(
_get_arr_size(_tmp9C,sizeof(char))== 1?_tmp9D == '\000'?_tmp9E != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp9C.curr)=_tmp9E;});}else{({struct _tagged_arr
_tmp9F=_tagged_arr_inplace_plus_post(& t,sizeof(char),1);char _tmpA0=*((char*)
_check_unknown_subscript(_tmp9F,sizeof(char),0));char _tmpA1='0';if(_get_arr_size(
_tmp9F,sizeof(char))== 1?_tmpA0 == '\000'?_tmpA1 != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmp9F.curr)=_tmpA1;});if(prec?1: flags & 8)({struct _tagged_arr _tmpA2=
_tagged_arr_inplace_plus_post(& t,sizeof(char),1);char _tmpA3=*((char*)
_check_unknown_subscript(_tmpA2,sizeof(char),0));char _tmpA4='.';if(_get_arr_size(
_tmpA2,sizeof(char))== 1?_tmpA3 == '\000'?_tmpA4 != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmpA2.curr)=_tmpA4;});}}if(fract != 0.0){if(prec)do{fract=modf(fract * 10,&
tmp);({struct _tagged_arr _tmpA5=_tagged_arr_inplace_plus_post(& t,sizeof(char),1);
char _tmpA6=*((char*)_check_unknown_subscript(_tmpA5,sizeof(char),0));char _tmpA7=(
char)((int)tmp + '0');if(_get_arr_size(_tmpA5,sizeof(char))== 1?_tmpA6 == '\000'?
_tmpA7 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmpA5.curr)=_tmpA7;});}while(
-- prec?fract != 0.0: 0);if(fract != 0.0)startp=Cyc_round(fract,(int*)& expcnt,startp,
_tagged_arr_plus(t,sizeof(char),- 1),(char)0,signp);}for(0;prec --;({struct
_tagged_arr _tmpA8=_tagged_arr_inplace_plus_post(& t,sizeof(char),1);char _tmpA9=*((
char*)_check_unknown_subscript(_tmpA8,sizeof(char),0));char _tmpAA='0';if(
_get_arr_size(_tmpA8,sizeof(char))== 1?_tmpA9 == '\000'?_tmpAA != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmpA8.curr)=_tmpAA;})){;}if(gformat?!(flags & 8): 0){
while(t.curr > startp.curr?*((char*)_check_unknown_subscript(
_tagged_arr_inplace_plus(& t,sizeof(char),-1),sizeof(char),0))== '0': 0){;}if(*((
char*)_check_unknown_subscript(t,sizeof(char),0))== '.')_tagged_arr_inplace_plus(&
t,sizeof(char),-1);_tagged_arr_inplace_plus(& t,sizeof(char),1);}t=Cyc_exponent(t,
expcnt,fmtch);break;case 'g': _LL6C: goto _LL6D;case 'G': _LL6D: if(!prec)++ prec;if(
expcnt > prec?1:((!expcnt?fract != 0.0: 0)?fract < .0001: 0)){-- prec;fmtch -=2;gformat=
1;goto eformat;}if(expcnt)for(0;(_tagged_arr_inplace_plus(& p,sizeof(char),1)).curr
< endp.curr;(({struct _tagged_arr _tmpAB=_tagged_arr_inplace_plus_post(& t,sizeof(
char),1);char _tmpAC=*((char*)_check_unknown_subscript(_tmpAB,sizeof(char),0));
char _tmpAD=*((char*)_check_unknown_subscript(p,sizeof(char),0));if(_get_arr_size(
_tmpAB,sizeof(char))== 1?_tmpAC == '\000'?_tmpAD != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmpAB.curr)=_tmpAD;}),-- prec)){;}else{({struct _tagged_arr _tmpAE=
_tagged_arr_inplace_plus_post(& t,sizeof(char),1);char _tmpAF=*((char*)
_check_unknown_subscript(_tmpAE,sizeof(char),0));char _tmpB0='0';if(_get_arr_size(
_tmpAE,sizeof(char))== 1?_tmpAF == '\000'?_tmpB0 != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmpAE.curr)=_tmpB0;});}if(prec?1: flags & 8){dotrim=1;({struct _tagged_arr
_tmpB1=_tagged_arr_inplace_plus_post(& t,sizeof(char),1);char _tmpB2=*((char*)
_check_unknown_subscript(_tmpB1,sizeof(char),0));char _tmpB3='.';if(_get_arr_size(
_tmpB1,sizeof(char))== 1?_tmpB2 == '\000'?_tmpB3 != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmpB1.curr)=_tmpB3;});}else{dotrim=0;}if(fract != 0.0){if(prec){do{fract=
modf(fract * 10,& tmp);({struct _tagged_arr _tmpB4=_tagged_arr_inplace_plus_post(& t,
sizeof(char),1);char _tmpB5=*((char*)_check_unknown_subscript(_tmpB4,sizeof(char),
0));char _tmpB6=(char)((int)tmp + '0');if(_get_arr_size(_tmpB4,sizeof(char))== 1?
_tmpB5 == '\000'?_tmpB6 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmpB4.curr)=
_tmpB6;});}while(tmp == 0.0?!expcnt: 0);while(-- prec?fract != 0.0: 0){fract=modf(
fract * 10,& tmp);({struct _tagged_arr _tmpB7=_tagged_arr_inplace_plus_post(& t,
sizeof(char),1);char _tmpB8=*((char*)_check_unknown_subscript(_tmpB7,sizeof(char),
0));char _tmpB9=(char)((int)tmp + '0');if(_get_arr_size(_tmpB7,sizeof(char))== 1?
_tmpB8 == '\000'?_tmpB9 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmpB7.curr)=
_tmpB9;});}}if(fract != 0.0)startp=Cyc_round(fract,0,startp,_tagged_arr_plus(t,
sizeof(char),- 1),(char)0,signp);}if(flags & 8)for(0;prec --;({struct _tagged_arr
_tmpBA=_tagged_arr_inplace_plus_post(& t,sizeof(char),1);char _tmpBB=*((char*)
_check_unknown_subscript(_tmpBA,sizeof(char),0));char _tmpBC='0';if(_get_arr_size(
_tmpBA,sizeof(char))== 1?_tmpBB == '\000'?_tmpBC != '\000': 0: 0)_throw_arraybounds();*((
char*)_tmpBA.curr)=_tmpBC;})){;}else{if(dotrim){while(t.curr > startp.curr?*((
char*)_check_unknown_subscript(_tagged_arr_inplace_plus(& t,sizeof(char),-1),
sizeof(char),0))== '0': 0){;}if(*((char*)_check_unknown_subscript(t,sizeof(char),
0))!= '.')_tagged_arr_inplace_plus(& t,sizeof(char),1);}}break;default: _LL6E:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpBD=_cycalloc(sizeof(*_tmpBD));
_tmpBD[0]=({struct Cyc_Core_Impossible_struct _tmpBE;_tmpBE.tag=Cyc_Core_Impossible;
_tmpBE.f1=({const char*_tmpBF="__cvt_double";_tag_arr(_tmpBF,sizeof(char),
_get_zero_arr_size(_tmpBF,13));});_tmpBE;});_tmpBD;}));}return(t.curr - startp.curr)
/ sizeof(char);}int Cyc_vfprintf(struct Cyc___cycFILE*f,struct _tagged_arr fmt,
struct _tagged_arr ap){int ans;ans=((int(*)(int(*ioputc)(int,struct Cyc___cycFILE*),
struct Cyc___cycFILE*ioputc_env,struct _tagged_arr fmt0,struct _tagged_arr ap))Cyc__IO_vfprintf)(
Cyc_putc,f,fmt,ap);return ans;}int Cyc_fprintf(struct Cyc___cycFILE*f,struct
_tagged_arr fmt,struct _tagged_arr ap){return Cyc_vfprintf(f,fmt,ap);}int Cyc_vprintf(
struct _tagged_arr fmt,struct _tagged_arr ap){int ans;ans=((int(*)(int(*ioputc)(int,
struct Cyc___cycFILE*),struct Cyc___cycFILE*ioputc_env,struct _tagged_arr fmt0,
struct _tagged_arr ap))Cyc__IO_vfprintf)(Cyc_putc,Cyc_stdout,fmt,ap);return ans;}
int Cyc_printf(struct _tagged_arr fmt,struct _tagged_arr ap){int ans;ans=Cyc_vprintf(
fmt,ap);return ans;}struct _tuple0{struct _tagged_arr*f1;unsigned int*f2;};static
int Cyc_putc_string(int c,struct _tuple0*sptr_n){struct _tuple0 _tmpC1;struct
_tagged_arr*_tmpC2;unsigned int*_tmpC3;struct _tuple0*_tmpC0=sptr_n;_tmpC1=*
_tmpC0;_tmpC2=_tmpC1.f1;_tmpC3=_tmpC1.f2;{struct _tagged_arr s=*((struct
_tagged_arr*)_tmpC2);unsigned int n=*((unsigned int*)_tmpC3);if(_get_arr_size(s,
sizeof(char))== 0?1: n == 0)return - 1;({struct _tagged_arr _tmpC4=s;char _tmpC5=*((
char*)_check_unknown_subscript(_tmpC4,sizeof(char),0));char _tmpC6=(char)c;if(
_get_arr_size(_tmpC4,sizeof(char))== 1?_tmpC5 == '\000'?_tmpC6 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmpC4.curr)=_tmpC6;});*((struct _tagged_arr*)
_tmpC2)=_tagged_arr_plus(s,sizeof(char),1);*((unsigned int*)_tmpC3)=n - 1;return 1;}}
int Cyc_vsnprintf(struct _tagged_arr s,unsigned int n,struct _tagged_arr fmt,struct
_tagged_arr ap){int ans;struct _tagged_arr _tmpC7=s;unsigned int _tmpC8=n;struct
_tuple0 _tmpC9=({struct _tuple0 _tmpCD;_tmpCD.f1=& _tmpC7;_tmpCD.f2=& _tmpC8;_tmpCD;});
ans=((int(*)(int(*ioputc)(int,struct _tuple0*),struct _tuple0*ioputc_env,struct
_tagged_arr fmt0,struct _tagged_arr ap))Cyc__IO_vfprintf)(Cyc_putc_string,& _tmpC9,
fmt,ap);if(0 <= ans)({struct _tagged_arr _tmpCA=_tagged_arr_plus(s,sizeof(char),ans);
char _tmpCB=*((char*)_check_unknown_subscript(_tmpCA,sizeof(char),0));char _tmpCC='\000';
if(_get_arr_size(_tmpCA,sizeof(char))== 1?_tmpCB == '\000'?_tmpCC != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmpCA.curr)=_tmpCC;});return ans;}int Cyc_snprintf(
struct _tagged_arr s,unsigned int n,struct _tagged_arr fmt,struct _tagged_arr ap){
return Cyc_vsnprintf(s,n,fmt,ap);}int Cyc_vsprintf(struct _tagged_arr s,struct
_tagged_arr fmt,struct _tagged_arr ap){return Cyc_vsnprintf(s,_get_arr_size(s,
sizeof(char)),fmt,ap);}int Cyc_sprintf(struct _tagged_arr s,struct _tagged_arr fmt,
struct _tagged_arr ap){return Cyc_vsnprintf(s,_get_arr_size(s,sizeof(char)),fmt,ap);}
static int Cyc_putc_void(int c,int dummy){return 1;}struct _tagged_arr Cyc_vrprintf(
struct _RegionHandle*r1,struct _tagged_arr fmt,struct _tagged_arr ap){int size=((int(*)(
int(*ioputc)(int,int),int ioputc_env,struct _tagged_arr fmt0,struct _tagged_arr ap))
Cyc__IO_vfprintf)(Cyc_putc_void,0,fmt,ap)+ 1;struct _tagged_arr s=({unsigned int
_tmpCE=(unsigned int)size;char*_tmpCF=(char*)_region_malloc(r1,_check_times(
sizeof(char),_tmpCE + 1));struct _tagged_arr _tmpD1=_tag_arr(_tmpCF,sizeof(char),
_tmpCE + 1);{unsigned int _tmpD0=_tmpCE;unsigned int i;for(i=0;i < _tmpD0;i ++){
_tmpCF[i]='\000';}_tmpCF[_tmpD0]=(char)0;}_tmpD1;});Cyc_vsprintf(s,fmt,ap);
return s;}struct _tagged_arr Cyc_rprintf(struct _RegionHandle*r1,struct _tagged_arr
fmt,struct _tagged_arr ap){return Cyc_vrprintf(r1,fmt,ap);}struct _tagged_arr Cyc_aprintf(
struct _tagged_arr fmt,struct _tagged_arr ap){return Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap);}
