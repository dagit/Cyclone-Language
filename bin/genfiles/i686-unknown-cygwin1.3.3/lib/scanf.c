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
  if (_cks_index >= _cks_bound) _throw_arraybounds(); \
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
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};int isspace(
int);int isupper(int);typedef struct{int quot;int rem;}Cyc_div_t;typedef struct{int
quot;int rem;}Cyc_ldiv_t;double atof(const char*);int strtol(char*,char**,int);
unsigned int strtoul(char*,char**,int);struct Cyc___cycFILE;extern struct Cyc___cycFILE*
Cyc_stdin;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Buffer_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_IntPtr_pa_struct{int tag;unsigned int*f1;}
;int Cyc_fgetc(struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;
};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};
struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _tagged_arr f1;};int Cyc_fscanf(struct Cyc___cycFILE*,struct _tagged_arr,
struct _tagged_arr);int Cyc_getc(struct Cyc___cycFILE*);int Cyc_scanf(struct
_tagged_arr,struct _tagged_arr);int Cyc_sscanf(struct _tagged_arr,struct _tagged_arr,
struct _tagged_arr);int Cyc_ungetc(int,struct Cyc___cycFILE*);int Cyc_vfscanf(struct
Cyc___cycFILE*,struct _tagged_arr,struct _tagged_arr);int Cyc_vsscanf(struct
_tagged_arr,struct _tagged_arr,struct _tagged_arr);extern char Cyc_FileCloseError[19];
extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;struct
_tagged_arr f1;};static struct _tagged_arr Cyc___sccl(char*tab,struct _tagged_arr fmt);
static short*Cyc_va_arg_short_ptr(void*a){void*_tmp0=a;short*_tmp1;unsigned short*
_tmp2;_LL1: if(*((int*)_tmp0)!= 0)goto _LL3;_tmp1=((struct Cyc_ShortPtr_sa_struct*)
_tmp0)->f1;_LL2: return _tmp1;_LL3: if(*((int*)_tmp0)!= 1)goto _LL5;_tmp2=((struct
Cyc_UShortPtr_sa_struct*)_tmp0)->f1;_LL4: return(short*)_tmp2;_LL5:;_LL6:(int)
_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp3=_cycalloc(sizeof(*
_tmp3));_tmp3[0]=({struct Cyc_Core_Invalid_argument_struct _tmp4;_tmp4.tag=Cyc_Core_Invalid_argument;
_tmp4.f1=({const char*_tmp5="scan expects short pointer";_tag_arr(_tmp5,sizeof(
char),_get_zero_arr_size(_tmp5,27));});_tmp4;});_tmp3;}));_LL0:;}static int*Cyc_va_arg_int_ptr(
void*a){void*_tmp6=a;int*_tmp7;unsigned int*_tmp8;_LL8: if(*((int*)_tmp6)!= 2)
goto _LLA;_tmp7=((struct Cyc_IntPtr_sa_struct*)_tmp6)->f1;_LL9: return _tmp7;_LLA:
if(*((int*)_tmp6)!= 3)goto _LLC;_tmp8=((struct Cyc_UIntPtr_sa_struct*)_tmp6)->f1;
_LLB: return(int*)_tmp8;_LLC:;_LLD:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9[0]=({struct Cyc_Core_Invalid_argument_struct
_tmpA;_tmpA.tag=Cyc_Core_Invalid_argument;_tmpA.f1=({const char*_tmpB="scan expects int pointer";
_tag_arr(_tmpB,sizeof(char),_get_zero_arr_size(_tmpB,25));});_tmpA;});_tmp9;}));
_LL7:;}static struct _tagged_arr Cyc_va_arg_string_ptr(void*a){void*_tmpC=a;struct
_tagged_arr _tmpD;_LLF: if(*((int*)_tmpC)!= 4)goto _LL11;_tmpD=((struct Cyc_StringPtr_sa_struct*)
_tmpC)->f1;_LL10: return _tmpD;_LL11:;_LL12:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE[0]=({struct Cyc_Core_Invalid_argument_struct
_tmpF;_tmpF.tag=Cyc_Core_Invalid_argument;_tmpF.f1=({const char*_tmp10="scan expects char pointer";
_tag_arr(_tmp10,sizeof(char),_get_zero_arr_size(_tmp10,26));});_tmpF;});_tmpE;}));
_LLE:;}static double*Cyc_va_arg_double_ptr(void*a){void*_tmp11=a;double*_tmp12;
_LL14: if(*((int*)_tmp11)!= 5)goto _LL16;_tmp12=((struct Cyc_DoublePtr_sa_struct*)
_tmp11)->f1;_LL15: return _tmp12;_LL16:;_LL17:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp14;_tmp14.tag=Cyc_Core_Invalid_argument;_tmp14.f1=({const char*_tmp15="scan expects double pointer";
_tag_arr(_tmp15,sizeof(char),_get_zero_arr_size(_tmp15,28));});_tmp14;});_tmp13;}));
_LL13:;}static float*Cyc_va_arg_float_ptr(void*a){void*_tmp16=a;float*_tmp17;
_LL19: if(*((int*)_tmp16)!= 6)goto _LL1B;_tmp17=((struct Cyc_FloatPtr_sa_struct*)
_tmp16)->f1;_LL1A: return _tmp17;_LL1B:;_LL1C:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp19;_tmp19.tag=Cyc_Core_Invalid_argument;_tmp19.f1=({const char*_tmp1A="scan expects float pointer";
_tag_arr(_tmp1A,sizeof(char),_get_zero_arr_size(_tmp1A,27));});_tmp19;});_tmp18;}));
_LL18:;}static struct _tagged_arr Cyc_va_arg_char_ptr(void*a){void*_tmp1B=a;struct
_tagged_arr _tmp1C;struct _tagged_arr _tmp1D;_LL1E: if(*((int*)_tmp1B)!= 7)goto _LL20;
_tmp1C=((struct Cyc_CharPtr_sa_struct*)_tmp1B)->f1;_LL1F: return _tmp1C;_LL20: if(*((
int*)_tmp1B)!= 4)goto _LL22;_tmp1D=((struct Cyc_StringPtr_sa_struct*)_tmp1B)->f1;
_LL21: return _tagged_ptr_decrease_size(_tmp1D,sizeof(char),1);_LL22:;_LL23:(int)
_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp1E=_cycalloc(sizeof(*
_tmp1E));_tmp1E[0]=({struct Cyc_Core_Invalid_argument_struct _tmp1F;_tmp1F.tag=Cyc_Core_Invalid_argument;
_tmp1F.f1=({const char*_tmp20="scan expects char pointer";_tag_arr(_tmp20,sizeof(
char),_get_zero_arr_size(_tmp20,26));});_tmp1F;});_tmp1E;}));_LL1D:;}int Cyc__IO_vfscanf(
int(*_IO_getc)(void*),int(*_IO_ungetc)(int,void*),int(*_IO_peekc)(void*),void*fp,
struct _tagged_arr fmt0,struct _tagged_arr ap,int*errp){struct _tagged_arr fmt=fmt0;
int c;long long width;struct _tagged_arr p=_tag_arr(0,0,0);int n;int flags=0;struct
_tagged_arr p0=_tag_arr(0,0,0);int nassigned;int nread;int base=0;int use_strtoul=0;
char ccltab[256];char buf[351];{unsigned int _tmp48=350;unsigned int i;for(i=0;i < 
_tmp48;i ++){buf[i]='0';}buf[_tmp48]=(char)0;}{int seen_eof=0;static short basefix[
17]={10,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};nassigned=0;nread=0;for(0;1;0){c=(
int)*((const char*)_check_unknown_subscript(_tagged_arr_inplace_plus_post(& fmt,
sizeof(char),1),sizeof(char),0));if(c == 0)goto done;if(isspace(c)){for(0;1;0){c=
_IO_getc(fp);if(c == - 1){seen_eof ++;break;}if(!isspace(c)){_IO_ungetc(c,fp);
break;}nread ++;}continue;}if(c != '%')goto literal;width=(long long)0;flags=0;
again: c=(int)*((const char*)_check_unknown_subscript(
_tagged_arr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0));switch(c){
case '%': _LL24: literal: n=_IO_getc(fp);if(n == - 1)goto eof_failure;if(n != c){
_IO_ungetc(n,fp);goto match_failure;}nread ++;continue;case '*': _LL25: if(flags)goto
control_failure;flags=8;goto again;case 'l': _LL26: if(flags & ~(8 | 64))goto
control_failure;flags |=1;goto again;case 'L': _LL27: if(flags & ~(8 | 64))goto
control_failure;flags |=2;goto again;case 'h': _LL28: if(flags & ~(8 | 64))goto
control_failure;flags |=4;goto again;case '0': _LL29: goto _LL2A;case '1': _LL2A: goto
_LL2B;case '2': _LL2B: goto _LL2C;case '3': _LL2C: goto _LL2D;case '4': _LL2D: goto _LL2E;
case '5': _LL2E: goto _LL2F;case '6': _LL2F: goto _LL30;case '7': _LL30: goto _LL31;case '8':
_LL31: goto _LL32;case '9': _LL32: if(flags & ~(8 | 64))goto control_failure;flags |=64;
width=(width * 10 + c)- '0';goto again;case 'D': _LL33: flags |=1;goto _LL34;case 'd':
_LL34: c=3;use_strtoul=0;base=10;break;case 'i': _LL35: c=3;use_strtoul=0;base=0;
break;case 'O': _LL36: flags |=1;goto _LL37;case 'o': _LL37: c=3;use_strtoul=1;base=8;
break;case 'u': _LL38: c=3;use_strtoul=1;base=10;break;case 'X': _LL39: goto _LL3A;case
'x': _LL3A: flags |=256;c=3;use_strtoul=1;base=16;break;case 'E': _LL3B: goto _LL3C;
case 'F': _LL3C: goto _LL3D;case 'e': _LL3D: goto _LL3E;case 'f': _LL3E: goto _LL3F;case 'g':
_LL3F: c=4;break;case 's': _LL40: c=2;break;case '[': _LL41: fmt=Cyc___sccl(ccltab,fmt);
flags |=32;c=1;break;case 'c': _LL42: flags |=32;c=0;break;case 'p': _LL43: flags |=16 | 
256;c=3;use_strtoul=1;base=16;break;case 'n': _LL44: if(flags & 8)continue;if(flags & 
4)*((short*)Cyc_va_arg_short_ptr(*((void**)_check_unknown_subscript(ap,sizeof(
void*),0))))=(short)nread;else{if(flags & 1)*((int*)Cyc_va_arg_int_ptr(*((void**)
_check_unknown_subscript(ap,sizeof(void*),0))))=(int)nread;else{*((int*)Cyc_va_arg_int_ptr(*((
void**)_check_unknown_subscript(ap,sizeof(void*),0))))=(int)nread;}}
_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);continue;case '\000': _LL45:
nassigned=- 1;goto done;default: _LL46: if(isupper(c))flags |=1;c=3;use_strtoul=0;
base=10;break;}if(_IO_peekc(fp)== - 1)goto eof_failure;if((flags & 32)== 0){n=
_IO_peekc(fp);while(isspace(n)){n=_IO_getc(fp);nread ++;n=_IO_peekc(fp);if(n == - 1)
goto eof_failure;}}switch(c){case 0: _LL48: if(width == 0)width=(long long)1;if(flags
& 8){long long sum=(long long)0;for(0;width > 0;0){n=_IO_getc(fp);if(n == - 1?width
!= 0: 0)goto eof_failure;else{if(n == - 1){seen_eof ++;break;}}sum ++;width --;}nread +=
sum;}else{long long sum=(long long)0;struct _tagged_arr _tmp21=Cyc_va_arg_char_ptr(*((
void**)_check_unknown_subscript(ap,sizeof(void*),0)));
_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);for(0;width > 0;0){n=_IO_getc(
fp);if(n == - 1?width != 0: 0)goto eof_failure;else{if(n == - 1){seen_eof ++;break;}}*((
char*)_check_unknown_subscript(_tmp21,sizeof(char),0))=(char)n;
_tagged_arr_inplace_plus_post(& _tmp21,sizeof(char),1);sum ++;width --;}nread +=sum;
nassigned ++;}break;case 1: _LL49: if(width == 0)width=(long long)~ 0;if(flags & 8){n=0;{
int c=_IO_peekc(fp);while((int)ccltab[_check_known_subscript_notnull(256,(int)((
char)c))]){n ++;_IO_getc(fp);if(-- width == 0)break;if((c=_IO_peekc(fp))== - 1){if(n
== 0)goto eof_failure;seen_eof ++;break;}}if(n == 0)goto match_failure;}}else{struct
_tagged_arr _tmp22=Cyc_va_arg_string_ptr(*((void**)_check_unknown_subscript(ap,
sizeof(void*),0)));_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);{struct
_tagged_arr _tmp23=_tmp22;int c=_IO_peekc(fp);while((int)ccltab[
_check_known_subscript_notnull(256,(int)((char)c))]){if(_get_arr_size(_tmp23,
sizeof(char))== 0)goto eof_failure;({struct _tagged_arr _tmp24=_tmp23;char _tmp25=*((
char*)_check_unknown_subscript(_tmp24,sizeof(char),0));char _tmp26=(char)c;if(
_get_arr_size(_tmp24,sizeof(char))== 1?_tmp25 == '\000'?_tmp26 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp24.curr)=_tmp26;});
_tagged_arr_inplace_plus_post(& _tmp23,sizeof(char),1);_IO_getc(fp);if(-- width == 
0)break;if((c=_IO_peekc(fp))== - 1){if(_tmp23.curr == p0.curr)goto eof_failure;
seen_eof ++;break;}}n=(_tmp23.curr - _tmp22.curr)/ sizeof(char);if(n == 0)goto
match_failure;if(_get_arr_size(_tmp23,sizeof(char))== 0)goto eof_failure;({struct
_tagged_arr _tmp27=_tmp23;char _tmp28=*((char*)_check_unknown_subscript(_tmp27,
sizeof(char),0));char _tmp29='\000';if(_get_arr_size(_tmp27,sizeof(char))== 1?
_tmp28 == '\000'?_tmp29 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp27.curr)=
_tmp29;});nassigned ++;}}nread +=n;break;case 2: _LL4A: if(width == 0)width=(long long)
~ 0;if(flags & 8){n=0;{int c=_IO_peekc(fp);while(!isspace((int)((unsigned char)c))){
n ++;_IO_getc(fp);if(-- width == 0)break;if((c=_IO_peekc(fp))== - 1){seen_eof ++;
break;}}nread +=n;}}else{struct _tagged_arr _tmp2A=Cyc_va_arg_string_ptr(*((void**)
_check_unknown_subscript(ap,sizeof(void*),0)));_tagged_arr_inplace_plus_post(& ap,
sizeof(void*),1);{struct _tagged_arr _tmp2B=_tmp2A;int c=_IO_peekc(fp);while(!
isspace((int)((unsigned char)c))){c=_IO_getc(fp);if(_get_arr_size(_tmp2B,sizeof(
char))== 0)goto eof_failure;({struct _tagged_arr _tmp2C=_tmp2B;char _tmp2D=*((char*)
_check_unknown_subscript(_tmp2C,sizeof(char),0));char _tmp2E=(char)c;if(
_get_arr_size(_tmp2C,sizeof(char))== 1?_tmp2D == '\000'?_tmp2E != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp2C.curr)=_tmp2E;});
_tagged_arr_inplace_plus_post(& _tmp2B,sizeof(char),1);if(-- width == 0)break;if((c=
_IO_peekc(fp))== - 1){seen_eof ++;break;}}if(_get_arr_size(_tmp2B,sizeof(char))== 
0)goto eof_failure;({struct _tagged_arr _tmp2F=_tmp2B;char _tmp30=*((char*)
_check_unknown_subscript(_tmp2F,sizeof(char),0));char _tmp31='\000';if(
_get_arr_size(_tmp2F,sizeof(char))== 1?_tmp30 == '\000'?_tmp31 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp2F.curr)=_tmp31;});nread +=(_tmp2B.curr - _tmp2A.curr)
/ sizeof(char);nassigned ++;}}continue;case 3: _LL4B: if(width == 0?1: width > sizeof(
buf)- 1)width=(long long)(sizeof(buf)- 1);flags |=(64 | 128)| 512;for(p=({char*
_tmp32=buf;_tag_arr(_tmp32,sizeof(char),_get_zero_arr_size(_tmp32,351));});width
!= 0;width --){c=(int)((unsigned char)_IO_peekc(fp));switch(c){case '0': _LL4D: if(
base == 0){base=8;flags |=256;}if(flags & 512)flags &=~((64 | 512)| 128);else{flags &=
~((64 | 256)| 128);}goto ok;case '1': _LL4E: goto _LL4F;case '2': _LL4F: goto _LL50;case '3':
_LL50: goto _LL51;case '4': _LL51: goto _LL52;case '5': _LL52: goto _LL53;case '6': _LL53:
goto _LL54;case '7': _LL54: base=(int)basefix[_check_known_subscript_notnull(17,base)];
flags &=~((64 | 256)| 128);goto ok;case '8': _LL55: goto _LL56;case '9': _LL56: base=(int)
basefix[_check_known_subscript_notnull(17,base)];if(base <= 8)break;flags &=~((64 | 
256)| 128);goto ok;case 'A': _LL57: goto _LL58;case 'B': _LL58: goto _LL59;case 'C': _LL59:
goto _LL5A;case 'D': _LL5A: goto _LL5B;case 'E': _LL5B: goto _LL5C;case 'F': _LL5C: goto
_LL5D;case 'a': _LL5D: goto _LL5E;case 'b': _LL5E: goto _LL5F;case 'c': _LL5F: goto _LL60;
case 'd': _LL60: goto _LL61;case 'e': _LL61: goto _LL62;case 'f': _LL62: if(base <= 10)
break;flags &=~((64 | 256)| 128);goto ok;case '+': _LL63: goto _LL64;case '-': _LL64: if(
flags & 64){flags &=~ 64;goto ok;}break;case 'x': _LL65: goto _LL66;case 'X': _LL66: if(
flags & 256?p.curr == (_tagged_arr_plus(({char*_tmp33=buf;_tag_arr(_tmp33,sizeof(
char),_get_zero_arr_size(_tmp33,351));}),sizeof(char),1)).curr: 0){base=16;flags &=
~ 256;goto ok;}break;default: _LL67: break;}break;ok:({struct _tagged_arr _tmp34=
_tagged_arr_inplace_plus_post(& p,sizeof(char),1);char _tmp35=*((char*)
_check_unknown_subscript(_tmp34,sizeof(char),0));char _tmp36=(char)c;if(
_get_arr_size(_tmp34,sizeof(char))== 1?_tmp35 == '\000'?_tmp36 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp34.curr)=_tmp36;});_IO_getc(fp);if(_IO_peekc(
fp)== - 1){seen_eof ++;break;}}if(flags & 128){if(p.curr > ({char*_tmp37=buf;
_tag_arr(_tmp37,sizeof(char),_get_zero_arr_size(_tmp37,351));}).curr){
_tagged_arr_inplace_plus(& p,sizeof(char),-1);_IO_ungetc((int)*((char*)
_check_unknown_subscript(p,sizeof(char),0)),fp);}goto match_failure;}c=(int)*((
char*)_check_unknown_subscript(p,sizeof(char),- 1));if(c == 'x'?1: c == 'X'){
_tagged_arr_inplace_plus(& p,sizeof(char),-1);_IO_ungetc(c,fp);}if((flags & 8)== 0){
unsigned int res;({struct _tagged_arr _tmp38=p;char _tmp39=*((char*)
_check_unknown_subscript(_tmp38,sizeof(char),0));char _tmp3A='\000';if(
_get_arr_size(_tmp38,sizeof(char))== 1?_tmp39 == '\000'?_tmp3A != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp38.curr)=_tmp3A;});if(use_strtoul)res=strtoul((
char*)buf,0,base);else{res=(unsigned int)strtol((char*)buf,0,base);}if(flags & 16)*((
int*)Cyc_va_arg_int_ptr(*((void**)_check_unknown_subscript(ap,sizeof(void*),0))))=(
int)res;else{if(flags & 4)*((short*)Cyc_va_arg_short_ptr(*((void**)
_check_unknown_subscript(ap,sizeof(void*),0))))=(short)res;else{if(flags & 1)*((
int*)Cyc_va_arg_int_ptr(*((void**)_check_unknown_subscript(ap,sizeof(void*),0))))=(
int)res;else{*((int*)Cyc_va_arg_int_ptr(*((void**)_check_unknown_subscript(ap,
sizeof(void*),0))))=(int)res;}}}_tagged_arr_inplace_plus_post(& ap,sizeof(void*),
1);nassigned ++;}nread +=(p.curr - ({char*_tmp3B=buf;_tag_arr(_tmp3B,sizeof(char),
_get_zero_arr_size(_tmp3B,351));}).curr)/ sizeof(char);break;case 4: _LL4C: if(
width == 0?1: width > sizeof(buf)- 1)width=(long long)(sizeof(buf)- 1);flags |=((64 | 
128)| 256)| 512;for(p=({char*_tmp3C=buf;_tag_arr(_tmp3C,sizeof(char),
_get_zero_arr_size(_tmp3C,351));});width != 0;width --){c=_IO_peekc(fp);switch(c){
case '0': _LL6A: goto _LL6B;case '1': _LL6B: goto _LL6C;case '2': _LL6C: goto _LL6D;case '3':
_LL6D: goto _LL6E;case '4': _LL6E: goto _LL6F;case '5': _LL6F: goto _LL70;case '6': _LL70:
goto _LL71;case '7': _LL71: goto _LL72;case '8': _LL72: goto _LL73;case '9': _LL73: flags &=~(
64 | 128);goto fok;case '+': _LL74: goto _LL75;case '-': _LL75: if(flags & 64){flags &=~ 64;
goto fok;}break;case '.': _LL76: if(flags & 256){flags &=~(64 | 256);goto fok;}break;
case 'e': _LL77: goto _LL78;case 'E': _LL78: if((flags & (128 | 512))== 512){flags=(flags
& ~(512 | 256)| 64)| 128;goto fok;}break;default: _LL79: break;}break;fok:({struct
_tagged_arr _tmp3D=_tagged_arr_inplace_plus_post(& p,sizeof(char),1);char _tmp3E=*((
char*)_check_unknown_subscript(_tmp3D,sizeof(char),0));char _tmp3F=(char)c;if(
_get_arr_size(_tmp3D,sizeof(char))== 1?_tmp3E == '\000'?_tmp3F != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp3D.curr)=_tmp3F;});_IO_getc(fp);if(_IO_peekc(
fp)== - 1){seen_eof ++;break;}}if(flags & 128){if(flags & 512){while(p.curr > ({char*
_tmp40=buf;_tag_arr(_tmp40,sizeof(char),_get_zero_arr_size(_tmp40,351));}).curr){
_tagged_arr_inplace_plus(& p,sizeof(char),-1);_IO_ungetc((int)*((char*)
_check_unknown_subscript(p,sizeof(char),0)),fp);}goto match_failure;}
_tagged_arr_inplace_plus(& p,sizeof(char),-1);c=(int)*((char*)
_check_unknown_subscript(p,sizeof(char),0));if(c != 'e'?c != 'E': 0){_IO_ungetc(c,
fp);_tagged_arr_inplace_plus(& p,sizeof(char),-1);c=(int)*((char*)
_check_unknown_subscript(p,sizeof(char),0));}_IO_ungetc(c,fp);}if((flags & 8)== 0){
double res;({struct _tagged_arr _tmp41=p;char _tmp42=*((char*)
_check_unknown_subscript(_tmp41,sizeof(char),0));char _tmp43='\000';if(
_get_arr_size(_tmp41,sizeof(char))== 1?_tmp42 == '\000'?_tmp43 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp41.curr)=_tmp43;});res=atof((const char*)buf);
if(flags & 1)*((double*)Cyc_va_arg_double_ptr(*((void**)_check_unknown_subscript(
ap,sizeof(void*),0))))=res;else{*((float*)Cyc_va_arg_float_ptr(*((void**)
_check_unknown_subscript(ap,sizeof(void*),0))))=(float)res;}
_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);nassigned ++;}nread +=(p.curr - ({
char*_tmp44=buf;_tag_arr(_tmp44,sizeof(char),_get_zero_arr_size(_tmp44,351));}).curr)
/ sizeof(char);break;default: _LL69:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45[0]=({struct Cyc_Core_Impossible_struct
_tmp46;_tmp46.tag=Cyc_Core_Impossible;_tmp46.f1=({const char*_tmp47="scanf3";
_tag_arr(_tmp47,sizeof(char),_get_zero_arr_size(_tmp47,7));});_tmp46;});_tmp45;}));}}
eof_failure: seen_eof ++;input_failure: if(nassigned == 0)nassigned=- 1;
control_failure: match_failure: if((unsigned int)errp)*((int*)errp)|=2;done: if((
unsigned int)errp?seen_eof: 0)*((int*)errp)|=1;return nassigned;}}static struct
_tagged_arr Cyc___sccl(char*tab,struct _tagged_arr fmt){int c;int n;int v;c=(int)*((
const char*)_check_unknown_subscript(_tagged_arr_inplace_plus_post(& fmt,sizeof(
char),1),sizeof(char),0));if(c == '^'){v=1;c=(int)*((const char*)
_check_unknown_subscript(_tagged_arr_inplace_plus_post(& fmt,sizeof(char),1),
sizeof(char),0));}else{v=0;}for(n=0;n < 256;n ++){tab[
_check_known_subscript_notnull(256,n)]=(char)v;}if(c == 0)return _tagged_arr_plus(
fmt,sizeof(char),- 1);v=1 - v;for(0;1;0){tab[_check_known_subscript_notnull(256,c)]=(
char)v;doswitch: n=(int)*((const char*)_check_unknown_subscript(
_tagged_arr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0));switch(n){
case 0: _LL7C: return _tagged_arr_plus(fmt,sizeof(char),- 1);case '-': _LL7D: n=(int)*((
const char*)_check_unknown_subscript(fmt,sizeof(char),0));if(n == ']'?1: n < c){c=(
int)'-';break;}_tagged_arr_inplace_plus_post(& fmt,sizeof(char),1);do{tab[
_check_known_subscript_notnull(256,++ c)]=(char)v;}while(c < n);goto doswitch;
break;case ']': _LL7E: return fmt;default: _LL7F: c=n;break;}}}static int Cyc_string_getc(
struct _tagged_arr*sptr){char c;struct _tagged_arr s=*((struct _tagged_arr*)sptr);if((
s.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr?1: _get_arr_size(s,sizeof(char))
== 0)?1:(c=*((const char*)_check_unknown_subscript(s,sizeof(char),0)))== '\000')
return - 1;*((struct _tagged_arr*)sptr)=_tagged_arr_plus(s,sizeof(char),1);return(
int)c;}static int Cyc_string_ungetc(int ignore,struct _tagged_arr*sptr){*((struct
_tagged_arr*)sptr)=_tagged_arr_plus(*((struct _tagged_arr*)sptr),sizeof(char),- 1);
return 0;}static int Cyc_string_peekc(struct _tagged_arr*sptr){char c;struct
_tagged_arr s=*((struct _tagged_arr*)sptr);if((s.curr == ((struct _tagged_arr)
_tag_arr(0,0,0)).curr?1: _get_arr_size(s,sizeof(char))== 0)?1:(c=*((const char*)
_check_unknown_subscript(s,sizeof(char),0)))== '\000')return - 1;return(int)c;}int
Cyc_vsscanf(struct _tagged_arr src,struct _tagged_arr fmt,struct _tagged_arr ap){int
err=0;return((int(*)(int(*_IO_getc)(struct _tagged_arr*),int(*_IO_ungetc)(int,
struct _tagged_arr*),int(*_IO_peekc)(struct _tagged_arr*),struct _tagged_arr*fp,
struct _tagged_arr fmt0,struct _tagged_arr ap,int*errp))Cyc__IO_vfscanf)(Cyc_string_getc,
Cyc_string_ungetc,Cyc_string_peekc,& src,fmt,ap,(int*)& err);}int Cyc_sscanf(struct
_tagged_arr src,struct _tagged_arr fmt,struct _tagged_arr ap){return Cyc_vsscanf(src,
fmt,ap);}int Cyc_peekc(struct Cyc___cycFILE*stream){int c=Cyc_fgetc(stream);Cyc_ungetc(
c,stream);return c;}int Cyc_vfscanf(struct Cyc___cycFILE*stream,struct _tagged_arr
fmt,struct _tagged_arr ap){int err=0;return((int(*)(int(*_IO_getc)(struct Cyc___cycFILE*),
int(*_IO_ungetc)(int,struct Cyc___cycFILE*),int(*_IO_peekc)(struct Cyc___cycFILE*),
struct Cyc___cycFILE*fp,struct _tagged_arr fmt0,struct _tagged_arr ap,int*errp))Cyc__IO_vfscanf)(
Cyc_getc,Cyc_ungetc,Cyc_peekc,stream,fmt,ap,(int*)& err);}int Cyc_fscanf(struct Cyc___cycFILE*
stream,struct _tagged_arr fmt,struct _tagged_arr ap){return Cyc_vfscanf(stream,fmt,ap);}
int Cyc_scanf(struct _tagged_arr fmt,struct _tagged_arr ap){return Cyc_vfscanf(Cyc_stdin,
fmt,ap);}
