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
#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#define _check_null(ptr) \
  ({ void *_check_null_temp = (void*)(ptr); \
     if (!_check_null_temp) _throw_null(); \
     _check_null_temp; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  ((char *)ptr) + (elt_sz)*(index); })
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  void *_cks_ptr = (void*)(ptr); \
  unsigned _cks_bound = (bound); \
  unsigned _cks_elt_sz = (elt_sz); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (!_cks_index >= _cks_bound) _throw_arraybounds(); \
  ((char *)cks_ptr) + cks_elt_sz*cks_index; })
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
  _tag_arr_ans.base = _tag_arr_ans.curr = (void *)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })

#define _init_tag_arr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _tagged_arr *_itarr_ptr = (arr_ptr); \
  void * _itarr = (arr); \
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
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_Cstdio___abstractFILE;
struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*Cyc_Std_stdout;extern int
Cyc_Std_putc(int __c,struct Cyc_Std___cycFILE*__stream);extern unsigned char Cyc_Std_FileCloseError[
19];extern unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct
Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int
tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};extern int
Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,struct _tagged_arr fmt,struct _tagged_arr);
extern int Cyc_Std_printf(struct _tagged_arr fmt,struct _tagged_arr);extern int Cyc_Std_sprintf(
struct _tagged_arr s,struct _tagged_arr fmt,struct _tagged_arr);extern int Cyc_Std_snprintf(
struct _tagged_arr s,unsigned int n,struct _tagged_arr fmt,struct _tagged_arr ap);
extern struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,struct _tagged_arr);
extern struct _tagged_arr Cyc_Std_rprintf(struct _RegionHandle*,struct _tagged_arr fmt,
struct _tagged_arr ap);extern int Cyc_Std_vfprintf(struct Cyc_Std___cycFILE*,struct
_tagged_arr fmt,struct _tagged_arr ap);extern int Cyc_Std_vprintf(struct _tagged_arr
fmt,struct _tagged_arr);extern int Cyc_Std_vsprintf(struct _tagged_arr s,struct
_tagged_arr fmt,struct _tagged_arr);extern int Cyc_Std_vsnprintf(struct _tagged_arr s,
unsigned int n,struct _tagged_arr fmt,struct _tagged_arr ap);extern struct _tagged_arr
Cyc_Std_vrprintf(struct _RegionHandle*r1,struct _tagged_arr fmt,struct _tagged_arr ap);
struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct
Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};extern unsigned char Cyc_List_List_mismatch[18];extern
unsigned char Cyc_List_Nth[8];extern unsigned int Cyc_Std_strlen(struct _tagged_arr s);
extern unsigned char Cyc_Array_Array_mismatch[19];struct Cyc_Std__Div{int quot;int
rem;};struct Cyc_Std__Ldiv{int quot;int rem;};extern double modf(double,double*);
static struct _tagged_arr Cyc_Std_parg2string(void*x){void*_tmp0=x;_LL1: if(*((int*)
_tmp0)== 0){goto _LL2;}else{goto _LL3;}_LL3: if(*((int*)_tmp0)== 1){goto _LL4;}else{
goto _LL5;}_LL5: if(*((int*)_tmp0)== 2){goto _LL6;}else{goto _LL7;}_LL7: if(*((int*)
_tmp0)== 3){goto _LL8;}else{goto _LL9;}_LL9: if(*((int*)_tmp0)== 4){goto _LLA;}else{
goto _LL0;}_LL2: return _tag_arr("string",sizeof(unsigned char),7);_LL4: return
_tag_arr("int",sizeof(unsigned char),4);_LL6: return _tag_arr("double",sizeof(
unsigned char),7);_LL8: return _tag_arr("short *",sizeof(unsigned char),8);_LLA:
return _tag_arr("unsigned long *",sizeof(unsigned char),16);_LL0:;}static void*Cyc_Std_badarg(
struct _tagged_arr s){return(void*)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp2;_tmp2.tag=Cyc_Core_Invalid_argument;_tmp2.f1=s;_tmp2;});_tmp1;}));}static
int Cyc_Std_va_arg_int(struct _tagged_arr ap){void*_tmp3=*((void**)
_check_unknown_subscript(ap,sizeof(void*),0));unsigned int _tmp4;_LLC: if(*((int*)
_tmp3)== 1){_LL10: _tmp4=((struct Cyc_Std_Int_pa_struct*)_tmp3)->f1;goto _LLD;}
else{goto _LLE;}_LLE: goto _LLF;_LLD: return(int)_tmp4;_LLF: return((int(*)(struct
_tagged_arr s))Cyc_Std_badarg)(_tag_arr("printf expected int",sizeof(
unsigned char),20));_LLB:;}static int Cyc_Std_va_arg_long(struct _tagged_arr ap){
void*_tmp5=*((void**)_check_unknown_subscript(ap,sizeof(void*),0));unsigned int
_tmp6;_LL12: if(*((int*)_tmp5)== 1){_LL16: _tmp6=((struct Cyc_Std_Int_pa_struct*)
_tmp5)->f1;goto _LL13;}else{goto _LL14;}_LL14: goto _LL15;_LL13: return(int)_tmp6;
_LL15: return((int(*)(struct _tagged_arr s))Cyc_Std_badarg)(_tag_arr("printf expected int",
sizeof(unsigned char),20));_LL11:;}static unsigned int Cyc_Std_va_arg_ulong(struct
_tagged_arr ap){void*_tmp7=*((void**)_check_unknown_subscript(ap,sizeof(void*),0));
unsigned int _tmp8;_LL18: if(*((int*)_tmp7)== 1){_LL1C: _tmp8=((struct Cyc_Std_Int_pa_struct*)
_tmp7)->f1;goto _LL19;}else{goto _LL1A;}_LL1A: goto _LL1B;_LL19: return(unsigned int)
_tmp8;_LL1B: return((unsigned int(*)(struct _tagged_arr s))Cyc_Std_badarg)(_tag_arr("printf expected int",
sizeof(unsigned char),20));_LL17:;}static unsigned int Cyc_Std_va_arg_uint(struct
_tagged_arr ap){void*_tmp9=*((void**)_check_unknown_subscript(ap,sizeof(void*),0));
unsigned int _tmpA;_LL1E: if(*((int*)_tmp9)== 1){_LL22: _tmpA=((struct Cyc_Std_Int_pa_struct*)
_tmp9)->f1;goto _LL1F;}else{goto _LL20;}_LL20: goto _LL21;_LL1F: return(unsigned int)
_tmpA;_LL21: return((unsigned int(*)(struct _tagged_arr s))Cyc_Std_badarg)(_tag_arr("printf expected int",
sizeof(unsigned char),20));_LL1D:;}static double Cyc_Std_va_arg_double(struct
_tagged_arr ap){void*_tmpB=*((void**)_check_unknown_subscript(ap,sizeof(void*),0));
double _tmpC;_LL24: if(*((int*)_tmpB)== 2){_LL28: _tmpC=((struct Cyc_Std_Double_pa_struct*)
_tmpB)->f1;goto _LL25;}else{goto _LL26;}_LL26: goto _LL27;_LL25: return _tmpC;_LL27:(
int)_throw(Cyc_Std_badarg((struct _tagged_arr)({struct Cyc_Std_String_pa_struct
_tmpE;_tmpE.tag=0;_tmpE.f1=(struct _tagged_arr)Cyc_Std_parg2string(*((void**)
_check_unknown_subscript(ap,sizeof(void*),0)));{void*_tmpD[1]={& _tmpE};Cyc_Std_aprintf(
_tag_arr("printf expected double but found %s",sizeof(unsigned char),36),
_tag_arr(_tmpD,sizeof(void*),1));}})));_LL23:;}static short*Cyc_Std_va_arg_short_ptr(
struct _tagged_arr ap){void*_tmpF=*((void**)_check_unknown_subscript(ap,sizeof(
void*),0));short*_tmp10;_LL2A: if(*((int*)_tmpF)== 3){_LL2E: _tmp10=((struct Cyc_Std_ShortPtr_pa_struct*)
_tmpF)->f1;goto _LL2B;}else{goto _LL2C;}_LL2C: goto _LL2D;_LL2B: return _tmp10;_LL2D:(
int)_throw(Cyc_Std_badarg(_tag_arr("printf expected short pointer",sizeof(
unsigned char),30)));_LL29:;}static unsigned int*Cyc_Std_va_arg_int_ptr(struct
_tagged_arr ap){void*_tmp11=*((void**)_check_unknown_subscript(ap,sizeof(void*),0));
unsigned int*_tmp12;_LL30: if(*((int*)_tmp11)== 4){_LL34: _tmp12=((struct Cyc_Std_IntPtr_pa_struct*)
_tmp11)->f1;goto _LL31;}else{goto _LL32;}_LL32: goto _LL33;_LL31: return _tmp12;_LL33:(
int)_throw(Cyc_Std_badarg(_tag_arr("printf expected int pointer",sizeof(
unsigned char),28)));_LL2F:;}static struct _tagged_arr Cyc_Std_va_arg_string(struct
_tagged_arr ap){void*_tmp13=*((void**)_check_unknown_subscript(ap,sizeof(void*),0));
struct _tagged_arr _tmp14;_LL36: if(*((int*)_tmp13)== 0){_LL3A: _tmp14=((struct Cyc_Std_String_pa_struct*)
_tmp13)->f1;goto _LL37;}else{goto _LL38;}_LL38: goto _LL39;_LL37: return _tmp14;_LL39:(
int)_throw(Cyc_Std_badarg(_tag_arr("printf expected string",sizeof(unsigned char),
23)));_LL35:;}int Cyc_Std___cvt_double(double number,int prec,int flags,int*signp,
int fmtch,struct _tagged_arr startp,struct _tagged_arr endp);enum Cyc_Std_BASE{Cyc_Std_OCT
 = 0,Cyc_Std_DEC  = 1,Cyc_Std_HEX  = 2};static int Cyc_Std__IO_sputn(int(*ioputc)(
int,void*),void*ioputc_env,struct _tagged_arr s,int howmany){int i=0;while(i < 
howmany){if(_get_arr_size(s,sizeof(unsigned char))== 0? 1: ioputc((int)*((const
unsigned char*)_check_unknown_subscript(s,sizeof(unsigned char),0)),ioputc_env)
== - 1){return i;}_tagged_arr_inplace_plus_post(& s,sizeof(unsigned char),1);i ++;}
return i;}static int Cyc_Std__IO_padn(int(*ioputc)(int,void*),void*ioputc_env,
unsigned char c,int howmany){int i=0;while(i < howmany){if(ioputc((int)c,ioputc_env)
== - 1){return i;}i ++;}return i;}static struct _tagged_arr Cyc_Std_my_memchr(struct
_tagged_arr s,unsigned char c,int n){int sz=(int)_get_arr_size(s,sizeof(
unsigned char));n=n < sz? n: sz;for(0;n != 0;(n --,_tagged_arr_inplace_plus_post(& s,
sizeof(unsigned char),1))){if(*((const unsigned char*)_check_unknown_subscript(s,
sizeof(unsigned char),0))== c){return s;}}return(struct _tagged_arr)_tag_arr(0,0,0);}
int Cyc_Std__IO_vfprintf(int(*ioputc)(int,void*),void*ioputc_env,struct
_tagged_arr fmt0,struct _tagged_arr ap){struct _tagged_arr fmt;int ch;int n;struct
_tagged_arr cp=_tag_arr(0,0,0);struct _tagged_arr cp2=(struct _tagged_arr)_tag_arr(0,
0,0);int use_cp2;struct _tagged_arr fmark;int flags;int ret;int width;int prec;
unsigned char sign;int softsign=0;double _double;int fpprec;unsigned int _ulong;int
dprec;int dpad;int fieldsz;int size=0;unsigned char buf[348];unsigned char ox[2]={'\000','\000'};
enum Cyc_Std_BASE base;fmt=fmt0;ret=0;for(0;1;0){fmark=fmt;{unsigned int fmt_sz=
_get_arr_size(fmt,sizeof(unsigned char));for(n=0;(n < fmt_sz?(ch=(int)((const
unsigned char*)fmt.curr)[n])!= '\000': 0)? ch != '%': 0;n ++){;}fmt=_tagged_arr_plus(
fmt,sizeof(unsigned char),n);if((n=(fmt.curr - fmark.curr)/ sizeof(unsigned char))
!= 0){do {if(Cyc_Std__IO_sputn(ioputc,ioputc_env,fmark,n)!= n){goto error;}} while (
0);ret +=n;}if(ch == '\000'){goto done;}_tagged_arr_inplace_plus_post(& fmt,sizeof(
unsigned char),1);flags=0;dprec=0;fpprec=0;width=0;prec=- 1;sign='\000';rflag: ch=(
int)*((const unsigned char*)_check_unknown_subscript(
_tagged_arr_inplace_plus_post(& fmt,sizeof(unsigned char),1),sizeof(unsigned char),
0));reswitch: use_cp2=0;switch(ch){case ' ': _LL3B: if(!((int)sign)){sign=' ';}goto
rflag;case '#': _LL3C: flags |=8;goto rflag;case '*': _LL3D: width=Cyc_Std_va_arg_int(ap);
_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);if(width >= 0){goto rflag;}
width=- width;goto _LL3E;case '-': _LL3E: flags |=16;flags &=~ 32;goto rflag;case '+':
_LL3F: sign='+';goto rflag;case '.': _LL40: if((ch=(int)*((const unsigned char*)
_check_unknown_subscript(_tagged_arr_inplace_plus_post(& fmt,sizeof(unsigned char),
1),sizeof(unsigned char),0)))== '*'){n=Cyc_Std_va_arg_int(ap);
_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);prec=n < 0? - 1: n;goto rflag;}n=0;
while((unsigned int)(ch - '0')<= 9){n=10 * n + (ch - '0');ch=(int)*((const
unsigned char*)_check_unknown_subscript(_tagged_arr_inplace_plus_post(& fmt,
sizeof(unsigned char),1),sizeof(unsigned char),0));}prec=n < 0? - 1: n;goto reswitch;
case '0': _LL41: if(!(flags & 16)){flags |=32;}goto rflag;case '1': _LL42: goto _LL43;case
'2': _LL43: goto _LL44;case '3': _LL44: goto _LL45;case '4': _LL45: goto _LL46;case '5':
_LL46: goto _LL47;case '6': _LL47: goto _LL48;case '7': _LL48: goto _LL49;case '8': _LL49:
goto _LL4A;case '9': _LL4A: n=0;do {n=10 * n + (ch - '0');ch=(int)*((const unsigned char*)
_check_unknown_subscript(_tagged_arr_inplace_plus_post(& fmt,sizeof(unsigned char),
1),sizeof(unsigned char),0));} while ((unsigned int)(ch - '0')<= 9);width=n;goto
reswitch;case 'L': _LL4B: flags |=2;goto rflag;case 'h': _LL4C: flags |=4;goto rflag;case 'l':
_LL4D: flags |=1;goto rflag;case 'c': _LL4E: cp=_tag_arr(buf,sizeof(unsigned char),348);*((
unsigned char*)_check_unknown_subscript(cp,sizeof(unsigned char),0))=(
unsigned char)Cyc_Std_va_arg_int(ap);_tagged_arr_inplace_plus_post(& ap,sizeof(
void*),1);size=1;sign='\000';break;case 'D': _LL4F: flags |=1;goto _LL50;case 'd':
_LL50: goto _LL51;case 'i': _LL51: _ulong=(unsigned int)(flags & 1? Cyc_Std_va_arg_long(
ap):(flags & 4?(int)((short)Cyc_Std_va_arg_int(ap)):(int)Cyc_Std_va_arg_int(ap)));
_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);if((int)_ulong < 0){_ulong=-
_ulong;sign='-';}base=Cyc_Std_DEC;goto number;case 'e': _LL52: goto _LL53;case 'E':
_LL53: goto _LL54;case 'f': _LL54: goto _LL55;case 'F': _LL55: goto _LL56;case 'g': _LL56:
goto _LL57;case 'G': _LL57: _double=Cyc_Std_va_arg_double(ap);
_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);if(prec > 39){if((ch != 'g'? ch
!= 'G': 0)? 1: flags & 8){fpprec=prec - 39;}prec=39;}else{if(prec == - 1){prec=6;}}cp=
_tag_arr(buf,sizeof(unsigned char),348);*((unsigned char*)
_check_unknown_subscript(cp,sizeof(unsigned char),0))='\000';size=Cyc_Std___cvt_double(
_double,prec,flags,& softsign,ch,cp,_tagged_arr_plus(_tag_arr(buf,sizeof(
unsigned char),348),sizeof(unsigned char),(int)sizeof(buf)));if(softsign){sign='-';}
if(*((unsigned char*)_check_unknown_subscript(cp,sizeof(unsigned char),0))== '\000'){
_tagged_arr_inplace_plus_post(& cp,sizeof(unsigned char),1);}break;case 'n': _LL58:
if(flags & 1){*Cyc_Std_va_arg_int_ptr(ap)=(unsigned int)ret;}else{if(flags & 4){*
Cyc_Std_va_arg_short_ptr(ap)=(short)ret;}else{*Cyc_Std_va_arg_int_ptr(ap)=(
unsigned int)ret;}}_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);continue;
case 'O': _LL59: flags |=1;goto _LL5A;case 'o': _LL5A: _ulong=flags & 1? Cyc_Std_va_arg_ulong(
ap):(flags & 4?(unsigned int)((unsigned short)Cyc_Std_va_arg_int(ap)):(
unsigned int)Cyc_Std_va_arg_uint(ap));_tagged_arr_inplace_plus_post(& ap,sizeof(
void*),1);base=Cyc_Std_OCT;goto nosign;case 'p': _LL5B: _ulong=(unsigned int)Cyc_Std_va_arg_long(
ap);_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);base=Cyc_Std_HEX;flags |=
64;ch=(int)'x';goto nosign;case 's': _LL5C: use_cp2=1;cp2=Cyc_Std_va_arg_string(ap);
_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);if(cp2.curr == ((struct
_tagged_arr)_tag_arr(0,0,0)).curr){cp2=_tag_arr("(NULL)",sizeof(unsigned char),7);}
if(prec >= 0){struct _tagged_arr p=Cyc_Std_my_memchr(cp2,'\000',prec);if(p.curr != ((
struct _tagged_arr)_tag_arr(0,0,0)).curr){size=(p.curr - cp2.curr)/ sizeof(
unsigned char);if(size > prec){size=prec;}}else{size=prec;}}else{size=(int)Cyc_Std_strlen(
cp2);}sign='\000';break;case 'U': _LL5D: flags |=1;goto _LL5E;case 'u': _LL5E: _ulong=
flags & 1? Cyc_Std_va_arg_ulong(ap):(flags & 4?(unsigned int)((unsigned short)Cyc_Std_va_arg_int(
ap)):(unsigned int)Cyc_Std_va_arg_uint(ap));_tagged_arr_inplace_plus_post(& ap,
sizeof(void*),1);base=Cyc_Std_DEC;goto nosign;case 'X': _LL5F: goto _LL60;case 'x':
_LL60: _ulong=flags & 1? Cyc_Std_va_arg_ulong(ap):(flags & 4?(unsigned int)((
unsigned short)Cyc_Std_va_arg_int(ap)):(unsigned int)Cyc_Std_va_arg_uint(ap));
_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);base=Cyc_Std_HEX;if(flags & 8?
_ulong != 0: 0){flags |=64;}nosign: sign='\000';number: if((dprec=prec)>= 0){flags &=~
32;}cp=_tagged_arr_plus(_tag_arr(buf,sizeof(unsigned char),348),sizeof(
unsigned char),(308 + 39)+ 1);if(_ulong != 0? 1: prec != 0){struct _tagged_arr xdigs;
switch(base){case Cyc_Std_OCT: _LL62: do {*((unsigned char*)
_check_unknown_subscript(_tagged_arr_inplace_plus(& cp,sizeof(unsigned char),-1),
sizeof(unsigned char),0))=(unsigned char)((_ulong & 7)+ '0');_ulong >>=3;} while ((
int)_ulong);if(flags & 8?*((unsigned char*)_check_unknown_subscript(cp,sizeof(
unsigned char),0))!= '0': 0){*((unsigned char*)_check_unknown_subscript(
_tagged_arr_inplace_plus(& cp,sizeof(unsigned char),-1),sizeof(unsigned char),0))='0';}
break;case Cyc_Std_DEC: _LL63: while(_ulong >= 10){*((unsigned char*)
_check_unknown_subscript(_tagged_arr_inplace_plus(& cp,sizeof(unsigned char),-1),
sizeof(unsigned char),0))=(unsigned char)(_ulong % 10 + '0');_ulong /=10;}*((
unsigned char*)_check_unknown_subscript(_tagged_arr_inplace_plus(& cp,sizeof(
unsigned char),-1),sizeof(unsigned char),0))=(unsigned char)(_ulong + '0');break;
case Cyc_Std_HEX: _LL64: if(ch == 'X'){xdigs=_tag_arr("0123456789ABCDEF",sizeof(
unsigned char),17);}else{xdigs=_tag_arr("0123456789abcdef",sizeof(unsigned char),
17);}do {*((unsigned char*)_check_unknown_subscript(_tagged_arr_inplace_plus(& cp,
sizeof(unsigned char),-1),sizeof(unsigned char),0))=*((const unsigned char*)
_check_unknown_subscript(xdigs,sizeof(unsigned char),(int)(_ulong & 15)));_ulong
>>=4;} while ((int)_ulong);break;}}size=((_tagged_arr_plus(_tag_arr(buf,sizeof(
unsigned char),348),sizeof(unsigned char),(308 + 39)+ 1)).curr - cp.curr)/ sizeof(
unsigned char);skipsize: break;default: _LL61: if(ch == '\000'){goto done;}cp=
_tag_arr(buf,sizeof(unsigned char),348);*((unsigned char*)
_check_unknown_subscript(cp,sizeof(unsigned char),0))=(unsigned char)ch;size=1;
sign='\000';break;}fieldsz=size + fpprec;dpad=dprec - size;if(dpad < 0){dpad=0;}if((
int)sign){fieldsz ++;}else{if(flags & 64){fieldsz +=2;}}fieldsz +=dpad;if((flags & (
16 | 32))== 0){if(Cyc_Std__IO_padn(ioputc,ioputc_env,' ',width - fieldsz)< width - 
fieldsz){goto error;}}if((int)sign){do {if(Cyc_Std__IO_sputn(ioputc,ioputc_env,
_tag_arr(& sign,sizeof(unsigned char),1),1)!= 1){goto error;}} while (0);}else{if(
flags & 64){ox[0]='0';ox[1]=(unsigned char)ch;do {if(Cyc_Std__IO_sputn(ioputc,
ioputc_env,_tag_arr(ox,sizeof(unsigned char),2),2)!= 2){goto error;}} while (0);}}
if((flags & (16 | 32))== 32){if(Cyc_Std__IO_padn(ioputc,ioputc_env,'0',width - 
fieldsz)< width - fieldsz){goto error;}}if(Cyc_Std__IO_padn(ioputc,ioputc_env,'0',
dpad)< dpad){goto error;}if(use_cp2){do {if(Cyc_Std__IO_sputn(ioputc,ioputc_env,
cp2,size)!= size){goto error;}} while (0);}else{do {if(Cyc_Std__IO_sputn(ioputc,
ioputc_env,(struct _tagged_arr)cp,size)!= size){goto error;}} while (0);}if(Cyc_Std__IO_padn(
ioputc,ioputc_env,'0',fpprec)< fpprec){goto error;}if(flags & 16){if(Cyc_Std__IO_padn(
ioputc,ioputc_env,' ',width - fieldsz)< width - fieldsz){goto error;}}ret +=width > 
fieldsz? width: fieldsz;}}done: return ret;error: return - 1;}static struct _tagged_arr
Cyc_Std_exponent(struct _tagged_arr p,int exp,int fmtch){struct _tagged_arr t;
unsigned char expbuf[308];*((unsigned char*)_check_unknown_subscript(
_tagged_arr_inplace_plus_post(& p,sizeof(unsigned char),1),sizeof(unsigned char),
0))=(unsigned char)fmtch;if(exp < 0){exp=- exp;*((unsigned char*)
_check_unknown_subscript(_tagged_arr_inplace_plus_post(& p,sizeof(unsigned char),
1),sizeof(unsigned char),0))='-';}else{*((unsigned char*)
_check_unknown_subscript(_tagged_arr_inplace_plus_post(& p,sizeof(unsigned char),
1),sizeof(unsigned char),0))='+';}t=_tagged_arr_plus(_tag_arr(expbuf,sizeof(
unsigned char),308),sizeof(unsigned char),308);if(exp > 9){do {*((unsigned char*)
_check_unknown_subscript(_tagged_arr_inplace_plus(& t,sizeof(unsigned char),-1),
sizeof(unsigned char),0))=(unsigned char)(exp % 10 + '0');} while ((exp /=10)> 9);*((
unsigned char*)_check_unknown_subscript(_tagged_arr_inplace_plus(& t,sizeof(
unsigned char),-1),sizeof(unsigned char),0))=(unsigned char)(exp + '0');for(0;t.curr
< (_tagged_arr_plus(_tag_arr(expbuf,sizeof(unsigned char),308),sizeof(
unsigned char),308)).curr;*((unsigned char*)_check_unknown_subscript(
_tagged_arr_inplace_plus_post(& p,sizeof(unsigned char),1),sizeof(unsigned char),
0))=*((unsigned char*)_check_unknown_subscript(_tagged_arr_inplace_plus_post(& t,
sizeof(unsigned char),1),sizeof(unsigned char),0))){;}}else{*((unsigned char*)
_check_unknown_subscript(_tagged_arr_inplace_plus_post(& p,sizeof(unsigned char),
1),sizeof(unsigned char),0))='0';*((unsigned char*)_check_unknown_subscript(
_tagged_arr_inplace_plus_post(& p,sizeof(unsigned char),1),sizeof(unsigned char),
0))=(unsigned char)(exp + '0');}return p;}static struct _tagged_arr Cyc_Std_round(
double fract,int*exp,struct _tagged_arr start,struct _tagged_arr end,unsigned char ch,
int*signp){double tmp=(double)0.0;if(fract != 0.0){modf(fract * 10,& tmp);}else{tmp=(
double)(ch - '0');}if(tmp > 4){for(0;1;_tagged_arr_inplace_plus(& end,sizeof(
unsigned char),-1)){if(*((unsigned char*)_check_unknown_subscript(end,sizeof(
unsigned char),0))== '.'){_tagged_arr_inplace_plus(& end,sizeof(unsigned char),-1);}
if(++(*((unsigned char*)_check_unknown_subscript(end,sizeof(unsigned char),0)))
<= '9'){break;}*((unsigned char*)_check_unknown_subscript(end,sizeof(
unsigned char),0))='0';if(end.curr == start.curr){if((unsigned int)exp){*((
unsigned char*)_check_unknown_subscript(end,sizeof(unsigned char),0))='1';++(*
exp);}else{*((unsigned char*)_check_unknown_subscript(_tagged_arr_inplace_plus(&
end,sizeof(unsigned char),-1),sizeof(unsigned char),0))='1';
_tagged_arr_inplace_plus(& start,sizeof(unsigned char),-1);}break;}}}else{if(*
signp == '-'){for(0;1;_tagged_arr_inplace_plus(& end,sizeof(unsigned char),-1)){
if(*((unsigned char*)_check_unknown_subscript(end,sizeof(unsigned char),0))== '.'){
_tagged_arr_inplace_plus(& end,sizeof(unsigned char),-1);}if(*((unsigned char*)
_check_unknown_subscript(end,sizeof(unsigned char),0))!= '0'){break;}if(end.curr
== start.curr){*signp=0;}}}}return start;}int Cyc_Std___cvt_double(double number,
int prec,int flags,int*signp,int fmtch,struct _tagged_arr startp,struct _tagged_arr
endp){struct _tagged_arr p;struct _tagged_arr t;double fract;int dotrim=0;int expcnt;
int gformat=0;double integer=(double)0.0;double tmp=(double)0.0;expcnt=0;if(number < 
0){number=- number;*signp=(int)'-';}else{*signp=0;}fract=modf(number,& integer);t=
_tagged_arr_inplace_plus(& startp,sizeof(unsigned char),1);for(p=_tagged_arr_plus(
endp,sizeof(unsigned char),- 1);p.curr >= startp.curr? integer != 0.0: 0;++ expcnt){
tmp=modf(integer / 10,& integer);*((unsigned char*)_check_unknown_subscript(
_tagged_arr_inplace_plus_post(& p,sizeof(unsigned char),-1),sizeof(unsigned char),
0))=(unsigned char)((int)((tmp + .01)* 10)+ '0');}switch(fmtch){case 'f': _LL67:
goto _LL68;case 'F': _LL68: if(expcnt){for(0;(_tagged_arr_inplace_plus(& p,sizeof(
unsigned char),1)).curr < endp.curr;*((unsigned char*)_check_unknown_subscript(
_tagged_arr_inplace_plus_post(& t,sizeof(unsigned char),1),sizeof(unsigned char),
0))=*((unsigned char*)_check_unknown_subscript(p,sizeof(unsigned char),0))){;}}
else{*((unsigned char*)_check_unknown_subscript(_tagged_arr_inplace_plus_post(& t,
sizeof(unsigned char),1),sizeof(unsigned char),0))='0';}if(prec? 1: flags & 8){*((
unsigned char*)_check_unknown_subscript(_tagged_arr_inplace_plus_post(& t,sizeof(
unsigned char),1),sizeof(unsigned char),0))='.';}if(fract != 0.0){if(prec){do {
fract=modf(fract * 10,& tmp);*((unsigned char*)_check_unknown_subscript(
_tagged_arr_inplace_plus_post(& t,sizeof(unsigned char),1),sizeof(unsigned char),
0))=(unsigned char)((int)tmp + '0');} while (-- prec? fract != 0.0: 0);}if(fract != 
0.0){startp=Cyc_Std_round(fract,0,startp,_tagged_arr_plus(t,sizeof(unsigned char),
- 1),(unsigned char)0,signp);}}for(0;prec --;*((unsigned char*)
_check_unknown_subscript(_tagged_arr_inplace_plus_post(& t,sizeof(unsigned char),
1),sizeof(unsigned char),0))='0'){;}break;case 'e': _LL69: goto _LL6A;case 'E': _LL6A:
eformat: if(expcnt){*((unsigned char*)_check_unknown_subscript(
_tagged_arr_inplace_plus_post(& t,sizeof(unsigned char),1),sizeof(unsigned char),
0))=*((unsigned char*)_check_unknown_subscript(_tagged_arr_inplace_plus(& p,
sizeof(unsigned char),1),sizeof(unsigned char),0));if(prec? 1: flags & 8){*((
unsigned char*)_check_unknown_subscript(_tagged_arr_inplace_plus_post(& t,sizeof(
unsigned char),1),sizeof(unsigned char),0))='.';}for(0;prec?(
_tagged_arr_inplace_plus(& p,sizeof(unsigned char),1)).curr < endp.curr: 0;-- prec){*((
unsigned char*)_check_unknown_subscript(_tagged_arr_inplace_plus_post(& t,sizeof(
unsigned char),1),sizeof(unsigned char),0))=*((unsigned char*)
_check_unknown_subscript(p,sizeof(unsigned char),0));}if(! prec?(
_tagged_arr_inplace_plus(& p,sizeof(unsigned char),1)).curr < endp.curr: 0){fract=(
double)0;startp=Cyc_Std_round((double)0,(int*)& expcnt,startp,_tagged_arr_plus(t,
sizeof(unsigned char),- 1),*((unsigned char*)_check_unknown_subscript(p,sizeof(
unsigned char),0)),signp);}-- expcnt;}else{if(fract != 0.0){for(expcnt=- 1;1;--
expcnt){fract=modf(fract * 10,& tmp);if(tmp != 0.0){break;}}*((unsigned char*)
_check_unknown_subscript(_tagged_arr_inplace_plus_post(& t,sizeof(unsigned char),
1),sizeof(unsigned char),0))=(unsigned char)((int)tmp + '0');if(prec? 1: flags & 8){*((
unsigned char*)_check_unknown_subscript(_tagged_arr_inplace_plus_post(& t,sizeof(
unsigned char),1),sizeof(unsigned char),0))='.';}}else{*((unsigned char*)
_check_unknown_subscript(_tagged_arr_inplace_plus_post(& t,sizeof(unsigned char),
1),sizeof(unsigned char),0))='0';if(prec? 1: flags & 8){*((unsigned char*)
_check_unknown_subscript(_tagged_arr_inplace_plus_post(& t,sizeof(unsigned char),
1),sizeof(unsigned char),0))='.';}}}if(fract != 0.0){if(prec){do {fract=modf(
fract * 10,& tmp);*((unsigned char*)_check_unknown_subscript(
_tagged_arr_inplace_plus_post(& t,sizeof(unsigned char),1),sizeof(unsigned char),
0))=(unsigned char)((int)tmp + '0');} while (-- prec? fract != 0.0: 0);}if(fract != 
0.0){startp=Cyc_Std_round(fract,(int*)& expcnt,startp,_tagged_arr_plus(t,sizeof(
unsigned char),- 1),(unsigned char)0,signp);}}for(0;prec --;*((unsigned char*)
_check_unknown_subscript(_tagged_arr_inplace_plus_post(& t,sizeof(unsigned char),
1),sizeof(unsigned char),0))='0'){;}if(gformat? !(flags & 8): 0){while(t.curr > 
startp.curr?*((unsigned char*)_check_unknown_subscript(_tagged_arr_inplace_plus(&
t,sizeof(unsigned char),-1),sizeof(unsigned char),0))== '0': 0){;}if(*((
unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),0))== '.'){
_tagged_arr_inplace_plus(& t,sizeof(unsigned char),-1);}_tagged_arr_inplace_plus(&
t,sizeof(unsigned char),1);}t=Cyc_Std_exponent(t,expcnt,fmtch);break;case 'g':
_LL6B: goto _LL6C;case 'G': _LL6C: if(! prec){++ prec;}if(expcnt > prec? 1:((! expcnt?
fract != 0.0: 0)? fract < .0001: 0)){-- prec;fmtch -=2;gformat=1;goto eformat;}if(expcnt){
for(0;(_tagged_arr_inplace_plus(& p,sizeof(unsigned char),1)).curr < endp.curr;(*((
unsigned char*)_check_unknown_subscript(_tagged_arr_inplace_plus_post(& t,sizeof(
unsigned char),1),sizeof(unsigned char),0))=*((unsigned char*)
_check_unknown_subscript(p,sizeof(unsigned char),0)),-- prec)){;}}else{*((
unsigned char*)_check_unknown_subscript(_tagged_arr_inplace_plus_post(& t,sizeof(
unsigned char),1),sizeof(unsigned char),0))='0';}if(prec? 1: flags & 8){dotrim=1;*((
unsigned char*)_check_unknown_subscript(_tagged_arr_inplace_plus_post(& t,sizeof(
unsigned char),1),sizeof(unsigned char),0))='.';}else{dotrim=0;}if(fract != 0.0){
if(prec){do {fract=modf(fract * 10,& tmp);*((unsigned char*)
_check_unknown_subscript(_tagged_arr_inplace_plus_post(& t,sizeof(unsigned char),
1),sizeof(unsigned char),0))=(unsigned char)((int)tmp + '0');} while (tmp == 0.0? !
expcnt: 0);while(-- prec? fract != 0.0: 0){fract=modf(fract * 10,& tmp);*((
unsigned char*)_check_unknown_subscript(_tagged_arr_inplace_plus_post(& t,sizeof(
unsigned char),1),sizeof(unsigned char),0))=(unsigned char)((int)tmp + '0');}}if(
fract != 0.0){startp=Cyc_Std_round(fract,0,startp,_tagged_arr_plus(t,sizeof(
unsigned char),- 1),(unsigned char)0,signp);}}if(flags & 8){for(0;prec --;*((
unsigned char*)_check_unknown_subscript(_tagged_arr_inplace_plus_post(& t,sizeof(
unsigned char),1),sizeof(unsigned char),0))='0'){;}}else{if(dotrim){while(t.curr
> startp.curr?*((unsigned char*)_check_unknown_subscript(
_tagged_arr_inplace_plus(& t,sizeof(unsigned char),-1),sizeof(unsigned char),0))
== '0': 0){;}if(*((unsigned char*)_check_unknown_subscript(t,sizeof(unsigned char),
0))!= '.'){_tagged_arr_inplace_plus(& t,sizeof(unsigned char),1);}}}break;
default: _LL6D:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp15=
_cycalloc(sizeof(*_tmp15));_tmp15[0]=({struct Cyc_Core_Impossible_struct _tmp16;
_tmp16.tag=Cyc_Core_Impossible;_tmp16.f1=_tag_arr("Std::__cvt_double",sizeof(
unsigned char),18);_tmp16;});_tmp15;}));}return(t.curr - startp.curr)/ sizeof(
unsigned char);}int Cyc_Std_vfprintf(struct Cyc_Std___cycFILE*f,struct _tagged_arr
fmt,struct _tagged_arr ap){int ans;ans=((int(*)(int(*ioputc)(int,struct Cyc_Std___cycFILE*),
struct Cyc_Std___cycFILE*ioputc_env,struct _tagged_arr fmt0,struct _tagged_arr ap))
Cyc_Std__IO_vfprintf)(Cyc_Std_putc,f,fmt,ap);return ans;}int Cyc_Std_fprintf(
struct Cyc_Std___cycFILE*f,struct _tagged_arr fmt,struct _tagged_arr ap){return Cyc_Std_vfprintf(
f,fmt,ap);}int Cyc_Std_vprintf(struct _tagged_arr fmt,struct _tagged_arr ap){int ans;
ans=((int(*)(int(*ioputc)(int,struct Cyc_Std___cycFILE*),struct Cyc_Std___cycFILE*
ioputc_env,struct _tagged_arr fmt0,struct _tagged_arr ap))Cyc_Std__IO_vfprintf)(Cyc_Std_putc,
Cyc_Std_stdout,fmt,ap);return ans;}int Cyc_Std_printf(struct _tagged_arr fmt,struct
_tagged_arr ap){int ans;ans=Cyc_Std_vprintf(fmt,ap);return ans;}struct _tuple0{
struct _tagged_arr*f1;unsigned int*f2;};static int Cyc_Std_putc_string(int c,struct
_tuple0*sptr_n){struct _tuple0 _tmp18;unsigned int*_tmp19;struct _tagged_arr*_tmp1A;
struct _tuple0*_tmp17=sptr_n;_tmp18=*_tmp17;_LL71: _tmp1A=_tmp18.f1;goto _LL70;
_LL70: _tmp19=_tmp18.f2;goto _LL6F;_LL6F: {struct _tagged_arr s=*_tmp1A;unsigned int
n=*_tmp19;if(_get_arr_size(s,sizeof(unsigned char))== 0? 1: n == 0){return - 1;}*((
unsigned char*)_check_unknown_subscript(s,sizeof(unsigned char),0))=(
unsigned char)c;*_tmp1A=_tagged_arr_plus(s,sizeof(unsigned char),1);*_tmp19=n - 1;
return 1;}}int Cyc_Std_vsnprintf(struct _tagged_arr s,unsigned int n,struct
_tagged_arr fmt,struct _tagged_arr ap){int ans;struct _tagged_arr _tmp1B=s;
unsigned int _tmp1C=n;struct _tuple0 _tmp1D=({struct _tuple0 _tmp1E;_tmp1E.f1=& _tmp1B;
_tmp1E.f2=& _tmp1C;_tmp1E;});ans=((int(*)(int(*ioputc)(int,struct _tuple0*),struct
_tuple0*ioputc_env,struct _tagged_arr fmt0,struct _tagged_arr ap))Cyc_Std__IO_vfprintf)(
Cyc_Std_putc_string,& _tmp1D,fmt,ap);if(0 <= ans){((unsigned char*)s.curr)[ans]='\000';}
return ans;}int Cyc_Std_snprintf(struct _tagged_arr s,unsigned int n,struct
_tagged_arr fmt,struct _tagged_arr ap){return Cyc_Std_vsnprintf(s,n,fmt,ap);}int Cyc_Std_vsprintf(
struct _tagged_arr s,struct _tagged_arr fmt,struct _tagged_arr ap){return Cyc_Std_vsnprintf(
s,_get_arr_size(s,sizeof(unsigned char)),fmt,ap);}int Cyc_Std_sprintf(struct
_tagged_arr s,struct _tagged_arr fmt,struct _tagged_arr ap){return Cyc_Std_vsnprintf(s,
_get_arr_size(s,sizeof(unsigned char)),fmt,ap);}static int Cyc_Std_putc_void(int c,
int dummy){return 1;}struct _tagged_arr Cyc_Std_vrprintf(struct _RegionHandle*r1,
struct _tagged_arr fmt,struct _tagged_arr ap){int size=((int(*)(int(*ioputc)(int,int),
int ioputc_env,struct _tagged_arr fmt0,struct _tagged_arr ap))Cyc_Std__IO_vfprintf)(
Cyc_Std_putc_void,0,fmt,ap)+ 1;struct _tagged_arr s=({unsigned int _tmp1F=(
unsigned int)size;unsigned char*_tmp20=(unsigned char*)_region_malloc(r1,
_check_times(sizeof(unsigned char),_tmp1F));struct _tagged_arr _tmp22=_tag_arr(
_tmp20,sizeof(unsigned char),(unsigned int)size);{unsigned int _tmp21=_tmp1F;
unsigned int i;for(i=0;i < _tmp21;i ++){_tmp20[i]='\000';}};_tmp22;});Cyc_Std_vsprintf(
s,fmt,ap);return s;}struct _tagged_arr Cyc_Std_rprintf(struct _RegionHandle*r1,
struct _tagged_arr fmt,struct _tagged_arr ap){return Cyc_Std_vrprintf(r1,fmt,ap);}
struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr fmt,struct _tagged_arr ap){
return Cyc_Std_vrprintf(Cyc_Core_heap_region,fmt,ap);}
