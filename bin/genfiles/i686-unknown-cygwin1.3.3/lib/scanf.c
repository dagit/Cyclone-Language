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
int);int isupper(int);typedef struct{int quot;int rem;}Cyc_Std_div_t;typedef struct{
int quot;int rem;}Cyc_Std_ldiv_t;double Cyc_Std_atof(struct _tagged_arr);int Cyc_Std_strtol(
struct _tagged_arr,struct _tagged_arr*,int);unsigned int Cyc_Std_strtoul(struct
_tagged_arr,struct _tagged_arr*,int);struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*
Cyc_Std_stdin;struct Cyc_Std_Cstdio___abstractFILE;struct Cyc_Std_String_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;}
;struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{
int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};int Cyc_Std_fgetc(
struct Cyc_Std___cycFILE*);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};
struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};int Cyc_Std_fscanf(
struct Cyc_Std___cycFILE*,struct _tagged_arr,struct _tagged_arr);int Cyc_Std_getc(
struct Cyc_Std___cycFILE*);int Cyc_Std_scanf(struct _tagged_arr,struct _tagged_arr);
int Cyc_Std_sscanf(struct _tagged_arr,struct _tagged_arr,struct _tagged_arr);int Cyc_Std_ungetc(
int,struct Cyc_Std___cycFILE*);int Cyc_Std_vfscanf(struct Cyc_Std___cycFILE*,struct
_tagged_arr,struct _tagged_arr);int Cyc_Std_vsscanf(struct _tagged_arr,struct
_tagged_arr,struct _tagged_arr);extern char Cyc_Std_FileCloseError[19];extern char
Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{char*tag;struct
_tagged_arr f1;};static struct _tagged_arr Cyc_Std___sccl(struct _tagged_arr tab,
struct _tagged_arr fmt);static short*Cyc_Std_va_arg_short_ptr(void*a){void*_tmp0=a;
short*_tmp1;unsigned short*_tmp2;_LL1: if(*((int*)_tmp0)!= 0)goto _LL3;_tmp1=((
struct Cyc_Std_ShortPtr_sa_struct*)_tmp0)->f1;_LL2: return _tmp1;_LL3: if(*((int*)
_tmp0)!= 1)goto _LL5;_tmp2=((struct Cyc_Std_UShortPtr_sa_struct*)_tmp0)->f1;_LL4:
return(short*)_tmp2;_LL5:;_LL6:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp4;_tmp4.tag=Cyc_Core_Invalid_argument;_tmp4.f1=_tag_arr("scan expects short pointer",
sizeof(char),27);_tmp4;});_tmp3;}));_LL0:;}static int*Cyc_Std_va_arg_int_ptr(void*
a){void*_tmp5=a;int*_tmp6;unsigned int*_tmp7;_LL8: if(*((int*)_tmp5)!= 2)goto _LLA;
_tmp6=((struct Cyc_Std_IntPtr_sa_struct*)_tmp5)->f1;_LL9: return _tmp6;_LLA: if(*((
int*)_tmp5)!= 3)goto _LLC;_tmp7=((struct Cyc_Std_UIntPtr_sa_struct*)_tmp5)->f1;
_LLB: return(int*)_tmp7;_LLC:;_LLD:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp8=_cycalloc(sizeof(*_tmp8));_tmp8[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp9;_tmp9.tag=Cyc_Core_Invalid_argument;_tmp9.f1=_tag_arr("scan expects int pointer",
sizeof(char),25);_tmp9;});_tmp8;}));_LL7:;}static struct _tagged_arr Cyc_Std_va_arg_string_ptr(
void*a){void*_tmpA=a;struct _tagged_arr _tmpB;_LLF: if(*((int*)_tmpA)!= 4)goto _LL11;
_tmpB=((struct Cyc_Std_StringPtr_sa_struct*)_tmpA)->f1;_LL10: return _tmpB;_LL11:;
_LL12:(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmpC=
_cycalloc(sizeof(*_tmpC));_tmpC[0]=({struct Cyc_Core_Invalid_argument_struct _tmpD;
_tmpD.tag=Cyc_Core_Invalid_argument;_tmpD.f1=_tag_arr("scan expects char pointer",
sizeof(char),26);_tmpD;});_tmpC;}));_LLE:;}static double*Cyc_Std_va_arg_double_ptr(
void*a){void*_tmpE=a;double*_tmpF;_LL14: if(*((int*)_tmpE)!= 5)goto _LL16;_tmpF=((
struct Cyc_Std_DoublePtr_sa_struct*)_tmpE)->f1;_LL15: return _tmpF;_LL16:;_LL17:(
int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp10=_cycalloc(
sizeof(*_tmp10));_tmp10[0]=({struct Cyc_Core_Invalid_argument_struct _tmp11;_tmp11.tag=
Cyc_Core_Invalid_argument;_tmp11.f1=_tag_arr("scan expects double pointer",
sizeof(char),28);_tmp11;});_tmp10;}));_LL13:;}static float*Cyc_Std_va_arg_float_ptr(
void*a){void*_tmp12=a;float*_tmp13;_LL19: if(*((int*)_tmp12)!= 6)goto _LL1B;_tmp13=((
struct Cyc_Std_FloatPtr_sa_struct*)_tmp12)->f1;_LL1A: return _tmp13;_LL1B:;_LL1C:(
int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp14=_cycalloc(
sizeof(*_tmp14));_tmp14[0]=({struct Cyc_Core_Invalid_argument_struct _tmp15;_tmp15.tag=
Cyc_Core_Invalid_argument;_tmp15.f1=_tag_arr("scan expects float pointer",
sizeof(char),27);_tmp15;});_tmp14;}));_LL18:;}int Cyc_Std__IO_vfscanf(int(*
_IO_getc)(void*),int(*_IO_ungetc)(int,void*),int(*_IO_peekc)(void*),void*fp,
struct _tagged_arr fmt0,struct _tagged_arr ap,int*errp){struct _tagged_arr fmt=fmt0;
int c;long long width;struct _tagged_arr p=_tag_arr(0,0,0);int n;int flags=0;struct
_tagged_arr p0=_tag_arr(0,0,0);int nassigned;int nread;int base=0;int use_strtoul=0;
char ccltab[256];char buf[350];int seen_eof=0;static short basefix[17]={10,1,2,3,4,5,
6,7,8,9,10,11,12,13,14,15,16};nassigned=0;nread=0;for(0;1;0){c=(int)*((const char*)
_check_unknown_subscript(_tagged_arr_inplace_plus_post(& fmt,sizeof(char),1),
sizeof(char),0));if(c == 0)goto done;if(isspace(c)){for(0;1;0){c=_IO_getc(fp);if(c
== - 1){seen_eof ++;break;}if(!isspace(c)){_IO_ungetc(c,fp);break;}nread ++;}
continue;}if(c != '%')goto literal;width=(long long)0;flags=0;again: c=(int)*((
const char*)_check_unknown_subscript(_tagged_arr_inplace_plus_post(& fmt,sizeof(
char),1),sizeof(char),0));switch(c){case '%': _LL1D: literal: n=_IO_getc(fp);if(n == 
- 1)goto eof_failure;if(n != c){_IO_ungetc(n,fp);goto match_failure;}nread ++;
continue;case '*': _LL1E: if(flags)goto control_failure;flags=8;goto again;case 'l':
_LL1F: if(flags & ~(8 | 64))goto control_failure;flags |=1;goto again;case 'L': _LL20:
if(flags & ~(8 | 64))goto control_failure;flags |=2;goto again;case 'h': _LL21: if(flags
& ~(8 | 64))goto control_failure;flags |=4;goto again;case '0': _LL22: goto _LL23;case '1':
_LL23: goto _LL24;case '2': _LL24: goto _LL25;case '3': _LL25: goto _LL26;case '4': _LL26:
goto _LL27;case '5': _LL27: goto _LL28;case '6': _LL28: goto _LL29;case '7': _LL29: goto
_LL2A;case '8': _LL2A: goto _LL2B;case '9': _LL2B: if(flags & ~(8 | 64))goto
control_failure;flags |=64;width=(width * 10 + c)- '0';goto again;case 'D': _LL2C:
flags |=1;goto _LL2D;case 'd': _LL2D: c=3;use_strtoul=0;base=10;break;case 'i': _LL2E: c=
3;use_strtoul=0;base=0;break;case 'O': _LL2F: flags |=1;goto _LL30;case 'o': _LL30: c=3;
use_strtoul=1;base=8;break;case 'u': _LL31: c=3;use_strtoul=1;base=10;break;case 'X':
_LL32: goto _LL33;case 'x': _LL33: flags |=256;c=3;use_strtoul=1;base=16;break;case 'E':
_LL34: goto _LL35;case 'F': _LL35: goto _LL36;case 'e': _LL36: goto _LL37;case 'f': _LL37:
goto _LL38;case 'g': _LL38: c=4;break;case 's': _LL39: c=2;break;case '[': _LL3A: fmt=Cyc_Std___sccl(
_tag_arr(ccltab,sizeof(char),256),fmt);flags |=32;c=1;break;case 'c': _LL3B: flags |=
32;c=0;break;case 'p': _LL3C: flags |=16 | 256;c=3;use_strtoul=1;base=16;break;case 'n':
_LL3D: if(flags & 8)continue;if(flags & 4)*Cyc_Std_va_arg_short_ptr(*((void**)
_check_unknown_subscript(ap,sizeof(void*),0)))=(short)nread;else{if(flags & 1)*
Cyc_Std_va_arg_int_ptr(*((void**)_check_unknown_subscript(ap,sizeof(void*),0)))=(
int)nread;else{*Cyc_Std_va_arg_int_ptr(*((void**)_check_unknown_subscript(ap,
sizeof(void*),0)))=(int)nread;}}_tagged_arr_inplace_plus_post(& ap,sizeof(void*),
1);continue;case '\000': _LL3E: nassigned=- 1;goto done;default: _LL3F: if(isupper(c))
flags |=1;c=3;use_strtoul=0;base=10;break;}if(_IO_peekc(fp)== - 1)goto eof_failure;
if((flags & 32)== 0){n=_IO_peekc(fp);while(isspace(n)){n=_IO_getc(fp);nread ++;n=
_IO_peekc(fp);if(n == - 1)goto eof_failure;}}switch(c){case 0: _LL41: if(width == 0)
width=(long long)1;if(flags & 8){long long sum=(long long)0;for(0;1;0){n=_IO_getc(
fp);if(n == - 1?width != 0: 0)goto eof_failure;else{if(n == - 1){seen_eof ++;break;}}sum
++;width --;}nread +=sum;}else{long long sum=(long long)0;struct _tagged_arr _tmp16=
Cyc_Std_va_arg_string_ptr(*((void**)_check_unknown_subscript(ap,sizeof(void*),0)));
_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);for(0;1;0){n=_IO_getc(fp);if(
n == - 1?width != 0: 0)goto eof_failure;else{if(n == - 1){seen_eof ++;break;}}if(
_get_arr_size(_tmp16,sizeof(char))== 0)goto eof_failure;*((char*)
_check_unknown_subscript(_tmp16,sizeof(char),0))=(char)n;sum ++;width --;
_tagged_arr_inplace_plus_post(& _tmp16,sizeof(char),1);}nread +=sum;if(
_get_arr_size(_tmp16,sizeof(char))!= 0)*((char*)_check_unknown_subscript(_tmp16,
sizeof(char),0))='\000';nassigned ++;}break;case 1: _LL42: if(width == 0)width=(
long long)~ 0;if(flags & 8){n=0;{int c=_IO_peekc(fp);while((int)ccltab[
_check_known_subscript_notnull(256,(int)((char)c))]){n ++;_IO_getc(fp);if(-- width
== 0)break;if((c=_IO_peekc(fp))== - 1){if(n == 0)goto eof_failure;seen_eof ++;break;}}
if(n == 0)goto match_failure;}}else{struct _tagged_arr _tmp17=Cyc_Std_va_arg_string_ptr(*((
void**)_check_unknown_subscript(ap,sizeof(void*),0)));
_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);{struct _tagged_arr _tmp18=
_tmp17;int c=_IO_peekc(fp);while((int)ccltab[_check_known_subscript_notnull(256,(
int)((char)c))]){if(_get_arr_size(_tmp18,sizeof(char))== 0)goto eof_failure;*((
char*)_check_unknown_subscript(_tmp18,sizeof(char),0))=(char)c;
_tagged_arr_inplace_plus_post(& _tmp18,sizeof(char),1);_IO_getc(fp);if(-- width == 
0)break;if((c=_IO_peekc(fp))== - 1){if(_tmp18.curr == p0.curr)goto eof_failure;
seen_eof ++;break;}}n=(_tmp18.curr - _tmp17.curr)/ sizeof(char);if(n == 0)goto
match_failure;if(_get_arr_size(_tmp18,sizeof(char))== 0)goto eof_failure;*((char*)
_check_unknown_subscript(_tmp18,sizeof(char),0))='\000';nassigned ++;}}nread +=n;
break;case 2: _LL43: if(width == 0)width=(long long)~ 0;if(flags & 8){n=0;{int c=
_IO_peekc(fp);while(!isspace((int)((unsigned char)c))){n ++;_IO_getc(fp);if(--
width == 0)break;if((c=_IO_peekc(fp))== - 1){seen_eof ++;break;}}nread +=n;}}else{
struct _tagged_arr _tmp19=Cyc_Std_va_arg_string_ptr(*((void**)
_check_unknown_subscript(ap,sizeof(void*),0)));_tagged_arr_inplace_plus_post(& ap,
sizeof(void*),1);{struct _tagged_arr _tmp1A=_tmp19;int c=_IO_peekc(fp);while(!
isspace((int)((unsigned char)c))){c=_IO_getc(fp);if(_get_arr_size(_tmp1A,sizeof(
char))== 0)goto eof_failure;*((char*)_check_unknown_subscript(_tmp1A,sizeof(char),
0))=(char)c;_tagged_arr_inplace_plus_post(& _tmp1A,sizeof(char),1);if(-- width == 0)
break;if((c=_IO_peekc(fp))== - 1){seen_eof ++;break;}}if(_get_arr_size(_tmp1A,
sizeof(char))== 0)goto eof_failure;*((char*)_check_unknown_subscript(_tmp1A,
sizeof(char),0))='\000';nread +=(_tmp1A.curr - _tmp19.curr)/ sizeof(char);
nassigned ++;}}continue;case 3: _LL44: if(width == 0?1: width > sizeof(buf)- 1)width=(
long long)(sizeof(buf)- 1);flags |=(64 | 128)| 512;for(p=_tag_arr(buf,sizeof(char),
350);width != 0;width --){c=(int)((unsigned char)_IO_peekc(fp));switch(c){case '0':
_LL46: if(base == 0){base=8;flags |=256;}if(flags & 512)flags &=~((64 | 512)| 128);
else{flags &=~((64 | 256)| 128);}goto ok;case '1': _LL47: goto _LL48;case '2': _LL48: goto
_LL49;case '3': _LL49: goto _LL4A;case '4': _LL4A: goto _LL4B;case '5': _LL4B: goto _LL4C;
case '6': _LL4C: goto _LL4D;case '7': _LL4D: base=(int)basefix[
_check_known_subscript_notnull(17,base)];flags &=~((64 | 256)| 128);goto ok;case '8':
_LL4E: goto _LL4F;case '9': _LL4F: base=(int)basefix[_check_known_subscript_notnull(
17,base)];if(base <= 8)break;flags &=~((64 | 256)| 128);goto ok;case 'A': _LL50: goto
_LL51;case 'B': _LL51: goto _LL52;case 'C': _LL52: goto _LL53;case 'D': _LL53: goto _LL54;
case 'E': _LL54: goto _LL55;case 'F': _LL55: goto _LL56;case 'a': _LL56: goto _LL57;case 'b':
_LL57: goto _LL58;case 'c': _LL58: goto _LL59;case 'd': _LL59: goto _LL5A;case 'e': _LL5A:
goto _LL5B;case 'f': _LL5B: if(base <= 10)break;flags &=~((64 | 256)| 128);goto ok;case '+':
_LL5C: goto _LL5D;case '-': _LL5D: if(flags & 64){flags &=~ 64;goto ok;}break;case 'x':
_LL5E: goto _LL5F;case 'X': _LL5F: if(flags & 256?p.curr == (_tagged_arr_plus(_tag_arr(
buf,sizeof(char),350),sizeof(char),1)).curr: 0){base=16;flags &=~ 256;goto ok;}
break;default: _LL60: break;}break;ok:*((char*)_check_unknown_subscript(
_tagged_arr_inplace_plus_post(& p,sizeof(char),1),sizeof(char),0))=(char)c;
_IO_getc(fp);if(_IO_peekc(fp)== - 1){seen_eof ++;break;}}if(flags & 128){if(p.curr > (
_tag_arr(buf,sizeof(char),350)).curr){_tagged_arr_inplace_plus(& p,sizeof(char),
-1);_IO_ungetc((int)*((char*)_check_unknown_subscript(p,sizeof(char),0)),fp);}
goto match_failure;}c=(int)*((char*)_check_unknown_subscript(p,sizeof(char),-1));
if(c == 'x'?1: c == 'X'){_tagged_arr_inplace_plus(& p,sizeof(char),-1);_IO_ungetc(c,
fp);}if((flags & 8)== 0){unsigned int res;*((char*)_check_unknown_subscript(p,
sizeof(char),0))='\000';if(use_strtoul)res=Cyc_Std_strtoul(_tag_arr(buf,sizeof(
char),350),0,base);else{res=(unsigned int)Cyc_Std_strtol(_tag_arr(buf,sizeof(
char),350),0,base);}if(flags & 16)*Cyc_Std_va_arg_int_ptr(*((void**)
_check_unknown_subscript(ap,sizeof(void*),0)))=(int)res;else{if(flags & 4)*Cyc_Std_va_arg_short_ptr(*((
void**)_check_unknown_subscript(ap,sizeof(void*),0)))=(short)res;else{if(flags & 
1)*Cyc_Std_va_arg_int_ptr(*((void**)_check_unknown_subscript(ap,sizeof(void*),0)))=(
int)res;else{*Cyc_Std_va_arg_int_ptr(*((void**)_check_unknown_subscript(ap,
sizeof(void*),0)))=(int)res;}}}_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);
nassigned ++;}nread +=(p.curr - (_tag_arr(buf,sizeof(char),350)).curr)/ sizeof(char);
break;case 4: _LL45: if(width == 0?1: width > sizeof(buf)- 1)width=(long long)(sizeof(
buf)- 1);flags |=((64 | 128)| 256)| 512;for(p=_tag_arr(buf,sizeof(char),350);width
!= 0;width --){c=_IO_peekc(fp);switch(c){case '0': _LL63: goto _LL64;case '1': _LL64:
goto _LL65;case '2': _LL65: goto _LL66;case '3': _LL66: goto _LL67;case '4': _LL67: goto
_LL68;case '5': _LL68: goto _LL69;case '6': _LL69: goto _LL6A;case '7': _LL6A: goto _LL6B;
case '8': _LL6B: goto _LL6C;case '9': _LL6C: flags &=~(64 | 128);goto fok;case '+': _LL6D:
goto _LL6E;case '-': _LL6E: if(flags & 64){flags &=~ 64;goto fok;}break;case '.': _LL6F:
if(flags & 256){flags &=~(64 | 256);goto fok;}break;case 'e': _LL70: goto _LL71;case 'E':
_LL71: if((flags & (128 | 512))== 512){flags=(flags & ~(512 | 256)| 64)| 128;goto fok;}
break;default: _LL72: break;}break;fok:*((char*)_check_unknown_subscript(
_tagged_arr_inplace_plus_post(& p,sizeof(char),1),sizeof(char),0))=(char)c;
_IO_getc(fp);if(_IO_peekc(fp)== - 1){seen_eof ++;break;}}if(flags & 128){if(flags & 
512){while(p.curr > (_tag_arr(buf,sizeof(char),350)).curr){
_tagged_arr_inplace_plus(& p,sizeof(char),-1);_IO_ungetc((int)*((char*)
_check_unknown_subscript(p,sizeof(char),0)),fp);}goto match_failure;}
_tagged_arr_inplace_plus(& p,sizeof(char),-1);c=(int)*((char*)
_check_unknown_subscript(p,sizeof(char),0));if(c != 'e'?c != 'E': 0){_IO_ungetc(c,
fp);_tagged_arr_inplace_plus(& p,sizeof(char),-1);c=(int)*((char*)
_check_unknown_subscript(p,sizeof(char),0));}_IO_ungetc(c,fp);}if((flags & 8)== 0){
double res;*((char*)_check_unknown_subscript(p,sizeof(char),0))='\000';res=Cyc_Std_atof(
_tag_arr(buf,sizeof(char),350));if(flags & 1)*Cyc_Std_va_arg_double_ptr(*((void**)
_check_unknown_subscript(ap,sizeof(void*),0)))=res;else{*Cyc_Std_va_arg_float_ptr(*((
void**)_check_unknown_subscript(ap,sizeof(void*),0)))=(float)res;}
_tagged_arr_inplace_plus_post(& ap,sizeof(void*),1);nassigned ++;}nread +=(p.curr - (
_tag_arr(buf,sizeof(char),350)).curr)/ sizeof(char);break;default: _LL62:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp1B=_cycalloc(sizeof(*_tmp1B));
_tmp1B[0]=({struct Cyc_Core_Impossible_struct _tmp1C;_tmp1C.tag=Cyc_Core_Impossible;
_tmp1C.f1=_tag_arr("scanf3",sizeof(char),7);_tmp1C;});_tmp1B;}));}}eof_failure:
seen_eof ++;input_failure: if(nassigned == 0)nassigned=- 1;control_failure:
match_failure: if((unsigned int)errp)*errp |=2;done: if((unsigned int)errp?seen_eof:
0)*errp |=1;return nassigned;}static struct _tagged_arr Cyc_Std___sccl(struct
_tagged_arr tab,struct _tagged_arr fmt){int c;int n;int v;c=(int)*((const char*)
_check_unknown_subscript(_tagged_arr_inplace_plus_post(& fmt,sizeof(char),1),
sizeof(char),0));if(c == '^'){v=1;c=(int)*((const char*)_check_unknown_subscript(
_tagged_arr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0));}else{v=0;}
for(n=0;n < 256;n ++){*((char*)_check_unknown_subscript(tab,sizeof(char),n))=(char)
v;}if(c == 0)return _tagged_arr_plus(fmt,sizeof(char),- 1);v=1 - v;for(0;1;0){*((
char*)_check_unknown_subscript(tab,sizeof(char),c))=(char)v;doswitch: n=(int)*((
const char*)_check_unknown_subscript(_tagged_arr_inplace_plus_post(& fmt,sizeof(
char),1),sizeof(char),0));switch(n){case 0: _LL75: return _tagged_arr_plus(fmt,
sizeof(char),- 1);case '-': _LL76: n=(int)*((const char*)_check_unknown_subscript(fmt,
sizeof(char),0));if(n == ']'?1: n < c){c=(int)'-';break;}
_tagged_arr_inplace_plus_post(& fmt,sizeof(char),1);do{*((char*)
_check_unknown_subscript(tab,sizeof(char),++ c))=(char)v;}while(c < n);goto
doswitch;break;case ']': _LL77: return fmt;default: _LL78: c=n;break;}}}static int Cyc_Std_string_getc(
struct _tagged_arr*sptr){char c;struct _tagged_arr s=*sptr;if((s.curr == ((struct
_tagged_arr)_tag_arr(0,0,0)).curr?1: _get_arr_size(s,sizeof(char))== 0)?1:(c=*((
const char*)_check_unknown_subscript(s,sizeof(char),0)))== '\000')return - 1;*sptr=
_tagged_arr_plus(s,sizeof(char),1);return(int)c;}static int Cyc_Std_string_ungetc(
int ignore,struct _tagged_arr*sptr){*sptr=_tagged_arr_plus(*sptr,sizeof(char),- 1);
return 0;}static int Cyc_Std_string_peekc(struct _tagged_arr*sptr){char c;struct
_tagged_arr s=*sptr;if((s.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr?1:
_get_arr_size(s,sizeof(char))== 0)?1:(c=*((const char*)_check_unknown_subscript(s,
sizeof(char),0)))== '\000')return - 1;return(int)c;}int Cyc_Std_vsscanf(struct
_tagged_arr src,struct _tagged_arr fmt,struct _tagged_arr ap){int err=0;int(*sgc)(
struct _tagged_arr*)=Cyc_Std_string_getc;return((int(*)(int(*_IO_getc)(struct
_tagged_arr*),int(*_IO_ungetc)(int,struct _tagged_arr*),int(*_IO_peekc)(struct
_tagged_arr*),struct _tagged_arr*fp,struct _tagged_arr fmt0,struct _tagged_arr ap,int*
errp))Cyc_Std__IO_vfscanf)(Cyc_Std_string_getc,Cyc_Std_string_ungetc,Cyc_Std_string_peekc,&
src,fmt,ap,(int*)& err);}int Cyc_Std_sscanf(struct _tagged_arr src,struct _tagged_arr
fmt,struct _tagged_arr ap){return Cyc_Std_vsscanf(src,fmt,ap);}int Cyc_Std_peekc(
struct Cyc_Std___cycFILE*stream){int c=Cyc_Std_fgetc(stream);Cyc_Std_ungetc(c,
stream);return c;}int Cyc_Std_vfscanf(struct Cyc_Std___cycFILE*stream,struct
_tagged_arr fmt,struct _tagged_arr ap){int err=0;return((int(*)(int(*_IO_getc)(
struct Cyc_Std___cycFILE*),int(*_IO_ungetc)(int,struct Cyc_Std___cycFILE*),int(*
_IO_peekc)(struct Cyc_Std___cycFILE*),struct Cyc_Std___cycFILE*fp,struct
_tagged_arr fmt0,struct _tagged_arr ap,int*errp))Cyc_Std__IO_vfscanf)(Cyc_Std_getc,
Cyc_Std_ungetc,Cyc_Std_peekc,stream,fmt,ap,(int*)& err);}int Cyc_Std_fscanf(struct
Cyc_Std___cycFILE*stream,struct _tagged_arr fmt,struct _tagged_arr ap){return Cyc_Std_vfscanf(
stream,fmt,ap);}int Cyc_Std_scanf(struct _tagged_arr fmt,struct _tagged_arr ap){
return Cyc_Std_vfscanf(Cyc_Std_stdin,fmt,ap);}
