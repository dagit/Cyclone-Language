#include <setjmp.h>
/* This is a C header file to be used by the output of the Cyclone to
   C translator.  The corresponding definitions are in file lib/runtime_*.c */
#ifndef _CYC_INCLUDE_H_
#define _CYC_INCLUDE_H_

/* Need one of these per thread (see runtime_stack.c). The runtime maintains 
   a stack that contains either _handler_cons structs or _RegionHandle structs.
   The tag is 0 for a handler_cons and 1 for a region handle.  */
struct _RuntimeStack {
  int tag; 
  struct _RuntimeStack *next;
  void (*cleanup)(struct _RuntimeStack *frame);
};

#ifndef offsetof
/* should be size_t, but int is fine. */
#define offsetof(t,n) ((int)(&(((t *)0)->n)))
#endif

/* Fat pointers */
struct _fat_ptr {
  unsigned char *curr; 
  unsigned char *base; 
  unsigned char *last_plus_one; 
};  

/* Discriminated Unions */
struct _xtunion_struct { char *tag; };

/* Regions */
struct _RegionPage
#ifdef CYC_REGION_PROFILE
{ unsigned total_bytes;
  unsigned free_bytes;
  struct _RegionPage *next;
  char data[1];
}
#endif
; // abstract -- defined in runtime_memory.c
struct _pool;
struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
  struct _DynRegionHandle *sub_regions;
  struct _pool *released_ptrs;
#ifdef CYC_REGION_PROFILE
  const char         *name;
#else
  unsigned used_bytes;
  unsigned wasted_bytes;
#endif
};
struct _DynRegionFrame {
  struct _RuntimeStack s;
  struct _DynRegionHandle *x;
};
// A dynamic region is just a region handle.  The wrapper struct is for type
// abstraction.
struct Cyc_Core_DynamicRegion {
  struct _RegionHandle h;
};

struct _RegionHandle _new_region(const char*);
void* _region_malloc(struct _RegionHandle*, unsigned);
void* _region_calloc(struct _RegionHandle*, unsigned t, unsigned n);
void   _free_region(struct _RegionHandle*);
struct _RegionHandle*_open_dynregion(struct _DynRegionFrame*,struct _DynRegionHandle*);
void   _pop_dynregion();

/* Exceptions */
struct _handler_cons {
  struct _RuntimeStack s;
  jmp_buf handler;
};
void _push_handler(struct _handler_cons *);
void _push_region(struct _RegionHandle *);
void _npop_handler(int);
void _pop_handler();
void _pop_region();

#ifndef _throw
void* _throw_null_fn(const char*,unsigned);
void* _throw_arraybounds_fn(const char*,unsigned);
void* _throw_badalloc_fn(const char*,unsigned);
void* _throw_match_fn(const char*,unsigned);
void* _throw_fn(void*,const char*,unsigned);
void* _rethrow(void*);
#define _throw_null() (_throw_null_fn(__FILE__,__LINE__))
#define _throw_arraybounds() (_throw_arraybounds_fn(__FILE__,__LINE__))
#define _throw_badalloc() (_throw_badalloc_fn(__FILE__,__LINE__))
#define _throw_match() (_throw_match_fn(__FILE__,__LINE__))
#define _throw(e) (_throw_fn((e),__FILE__,__LINE__))
#endif

struct _xtunion_struct* Cyc_Core_get_exn_thrown();
/* Built-in Exceptions */
struct Cyc_Null_Exception_exn_struct { char *tag; };
struct Cyc_Array_bounds_exn_struct { char *tag; };
struct Cyc_Match_Exception_exn_struct { char *tag; };
struct Cyc_Bad_alloc_exn_struct { char *tag; };
extern char Cyc_Null_Exception[];
extern char Cyc_Array_bounds[];
extern char Cyc_Match_Exception[];
extern char Cyc_Bad_alloc[];

/* Built-in Run-time Checks and company */
#ifdef CYC_ANSI_OUTPUT
#define _INLINE  
#else
#define _INLINE inline
#endif

#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#define _check_null(ptr) \
  ({ void*_cks_null = (void*)(ptr); \
     if (!_cks_null) _throw_null(); \
     _cks_null; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_notnull(ptr,bound,elt_sz,index)\
   (((char*)ptr) + (elt_sz)*(index))
#ifdef NO_CYC_NULL_CHECKS
#define _check_known_subscript_null _check_known_subscript_notnull
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  char*_cks_ptr = (char*)(ptr);\
  int _index = (index);\
  if (!_cks_ptr) _throw_null(); \
  _cks_ptr + (elt_sz)*_index; })
#endif
#define _zero_arr_plus_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_char_fn _zero_arr_plus_fn
#define _zero_arr_plus_short_fn _zero_arr_plus_fn
#define _zero_arr_plus_int_fn _zero_arr_plus_fn
#define _zero_arr_plus_float_fn _zero_arr_plus_fn
#define _zero_arr_plus_double_fn _zero_arr_plus_fn
#define _zero_arr_plus_longdouble_fn _zero_arr_plus_fn
#define _zero_arr_plus_voidstar_fn _zero_arr_plus_fn
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  char*_cks_ptr = (char*)(ptr); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (_cks_index >= (bound)) _throw_arraybounds(); \
  _cks_ptr + (elt_sz)*_cks_index; })
#define _check_known_subscript_notnull(ptr,bound,elt_sz,index) ({ \
  char*_cks_ptr = (char*)(ptr); \
  unsigned _cks_index = (index); \
  if (_cks_index >= (bound)) _throw_arraybounds(); \
  _cks_ptr + (elt_sz)*_cks_index; })

/* _zero_arr_plus_*_fn(x,sz,i,filename,lineno) adds i to zero-terminated ptr
   x that has at least sz elements */
char* _zero_arr_plus_char_fn(char*,unsigned,int,const char*,unsigned);
short* _zero_arr_plus_short_fn(short*,unsigned,int,const char*,unsigned);
int* _zero_arr_plus_int_fn(int*,unsigned,int,const char*,unsigned);
float* _zero_arr_plus_float_fn(float*,unsigned,int,const char*,unsigned);
double* _zero_arr_plus_double_fn(double*,unsigned,int,const char*,unsigned);
long double* _zero_arr_plus_longdouble_fn(long double*,unsigned,int,const char*, unsigned);
void** _zero_arr_plus_voidstar_fn(void**,unsigned,int,const char*,unsigned);
#endif

/* _get_zero_arr_size_*(x,sz) returns the number of elements in a
   zero-terminated array that is NULL or has at least sz elements */
int _get_zero_arr_size_char(const char*,unsigned);
int _get_zero_arr_size_short(const short*,unsigned);
int _get_zero_arr_size_int(const int*,unsigned);
int _get_zero_arr_size_float(const float*,unsigned);
int _get_zero_arr_size_double(const double*,unsigned);
int _get_zero_arr_size_longdouble(const long double*,unsigned);
int _get_zero_arr_size_voidstar(const void**,unsigned);

/* _zero_arr_inplace_plus_*_fn(x,i,filename,lineno) sets
   zero-terminated pointer *x to *x + i */
char* _zero_arr_inplace_plus_char_fn(char**,int,const char*,unsigned);
short* _zero_arr_inplace_plus_short_fn(short**,int,const char*,unsigned);
int* _zero_arr_inplace_plus_int(int**,int,const char*,unsigned);
float* _zero_arr_inplace_plus_float_fn(float**,int,const char*,unsigned);
double* _zero_arr_inplace_plus_double_fn(double**,int,const char*,unsigned);
long double* _zero_arr_inplace_plus_longdouble_fn(long double**,int,const char*,unsigned);
void** _zero_arr_inplace_plus_voidstar_fn(void***,int,const char*,unsigned);
/* like the previous functions, but does post-addition (as in e++) */
char* _zero_arr_inplace_plus_post_char_fn(char**,int,const char*,unsigned);
short* _zero_arr_inplace_plus_post_short_fn(short**x,int,const char*,unsigned);
int* _zero_arr_inplace_plus_post_int_fn(int**,int,const char*,unsigned);
float* _zero_arr_inplace_plus_post_float_fn(float**,int,const char*,unsigned);
double* _zero_arr_inplace_plus_post_double_fn(double**,int,const char*,unsigned);
long double* _zero_arr_inplace_plus_post_longdouble_fn(long double**,int,const char *,unsigned);
void** _zero_arr_inplace_plus_post_voidstar_fn(void***,int,const char*,unsigned);
#define _zero_arr_plus_char(x,s,i) \
  (_zero_arr_plus_char_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_short(x,s,i) \
  (_zero_arr_plus_short_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_int(x,s,i) \
  (_zero_arr_plus_int_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_float(x,s,i) \
  (_zero_arr_plus_float_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_double(x,s,i) \
  (_zero_arr_plus_double_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_longdouble(x,s,i) \
  (_zero_arr_plus_longdouble_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_voidstar(x,s,i) \
  (_zero_arr_plus_voidstar_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_inplace_plus_char(x,i) \
  _zero_arr_inplace_plus_char_fn((char **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_short(x,i) \
  _zero_arr_inplace_plus_short_fn((short **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_int(x,i) \
  _zero_arr_inplace_plus_int_fn((int **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_float(x,i) \
  _zero_arr_inplace_plus_float_fn((float **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_double(x,i) \
  _zero_arr_inplace_plus_double_fn((double **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_longdouble(x,i) \
  _zero_arr_inplace_plus_longdouble_fn((long double **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_voidstar(x,i) \
  _zero_arr_inplace_plus_voidstar_fn((void ***)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_char(x,i) \
  _zero_arr_inplace_plus_post_char_fn((char **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_short(x,i) \
  _zero_arr_inplace_plus_post_short_fn((short **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_int(x,i) \
  _zero_arr_inplace_plus_post_int_fn((int **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_float(x,i) \
  _zero_arr_inplace_plus_post_float_fn((float **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_double(x,i) \
  _zero_arr_inplace_plus_post_double_fn((double **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_longdouble(x,i) \
  _zero_arr_inplace_plus_post_longdouble_fn((long double **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_voidstar(x,i) \
  _zero_arr_inplace_plus_post_voidstar_fn((void***)(x),(i),__FILE__,__LINE__)

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_fat_subscript(arr,elt_sz,index) ((arr).curr + (elt_sz) * (index))
#define _untag_fat_ptr(arr,elt_sz,num_elts) ((arr).curr)
#else
#define _check_fat_subscript(arr,elt_sz,index) ({ \
  struct _fat_ptr _cus_arr = (arr); \
  unsigned char *_cus_ans = _cus_arr.curr + (elt_sz) * (index); \
  /* JGM: not needed! if (!_cus_arr.base) _throw_null();*/ \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#define _untag_fat_ptr(arr,elt_sz,num_elts) ({ \
  struct _fat_ptr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if ((_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one) &&\
      _curr != (unsigned char *)0) \
    _throw_arraybounds(); \
  _curr; })
#endif

#define _tag_fat(tcurr,elt_sz,num_elts) ({ \
  struct _fat_ptr _ans; \
  unsigned _num_elts = (num_elts);\
  _ans.base = _ans.curr = (void*)(tcurr); \
  /* JGM: if we're tagging NULL, ignore num_elts */ \
  _ans.last_plus_one = _ans.base ? (_ans.base + (elt_sz) * _num_elts) : 0; \
  _ans; })

#define _get_fat_size(arr,elt_sz) \
  ({struct _fat_ptr _arr = (arr); \
    unsigned char *_arr_curr=_arr.curr; \
    unsigned char *_arr_last=_arr.last_plus_one; \
    (_arr_curr < _arr.base || _arr_curr >= _arr_last) ? 0 : \
    ((_arr_last - _arr_curr) / (elt_sz));})

#define _fat_ptr_plus(arr,elt_sz,change) ({ \
  struct _fat_ptr _ans = (arr); \
  int _change = (change);\
  _ans.curr += (elt_sz) * _change;\
  _ans; })
#define _fat_ptr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _fat_ptr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += (elt_sz) * (change);\
  *_arr_ptr; })
#define _fat_ptr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _fat_ptr * _arr_ptr = (arr_ptr); \
  struct _fat_ptr _ans = *_arr_ptr; \
  _arr_ptr->curr += (elt_sz) * (change);\
  _ans; })

//Not a macro since initialization order matters. Defined in runtime_zeroterm.c.
struct _fat_ptr _fat_ptr_decrease_size(struct _fat_ptr,unsigned sz,unsigned numelts);

/* Allocation */
void* GC_malloc(int);
void* GC_malloc_atomic(int);
void* GC_calloc(unsigned,unsigned);
void* GC_calloc_atomic(unsigned,unsigned);
// bound the allocation size to be < MAX_ALLOC_SIZE. See macros below for usage.
#define MAX_MALLOC_SIZE (1 << 28)
void* _bounded_GC_malloc(int,const char*,int);
void* _bounded_GC_malloc_atomic(int,const char*,int);
void* _bounded_GC_calloc(unsigned,unsigned,const char*,int);
void* _bounded_GC_calloc_atomic(unsigned,unsigned,const char*,int);
/* these macros are overridden below ifdef CYC_REGION_PROFILE */
#ifndef CYC_REGION_PROFILE
#define _cycalloc(n) _bounded_GC_malloc(n,__FILE__,__LINE__)
#define _cycalloc_atomic(n) _bounded_GC_malloc_atomic(n,__FILE__,__LINE__)
#define _cyccalloc(n,s) _bounded_GC_calloc(n,s,__FILE__,__LINE__)
#define _cyccalloc_atomic(n,s) _bounded_GC_calloc_atomic(n,s,__FILE__,__LINE__)
#endif

static _INLINE unsigned int _check_times(unsigned x, unsigned y) {
  unsigned long long whole_ans = 
    ((unsigned long long) x)*((unsigned long long)y);
  unsigned word_ans = (unsigned)whole_ans;
  if(word_ans < whole_ans || word_ans > MAX_MALLOC_SIZE)
    _throw_badalloc();
  return word_ans;
}

#define _CYC_MAX_REGION_CONST 2
#define _CYC_MIN_ALIGNMENT (sizeof(double))

#ifdef CYC_REGION_PROFILE
extern int rgn_total_bytes;
#endif

static _INLINE void *_fast_region_malloc(struct _RegionHandle *r, unsigned orig_s) {  
  if (r > (struct _RegionHandle *)_CYC_MAX_REGION_CONST && r->curr != 0) { 
#ifdef CYC_NOALIGN
    unsigned s =  orig_s;
#else
    unsigned s =  (orig_s + _CYC_MIN_ALIGNMENT - 1) & (~(_CYC_MIN_ALIGNMENT -1)); 
#endif
    char *result; 
    result = r->offset; 
    if (s <= (r->last_plus_one - result)) {
      r->offset = result + s; 
#ifdef CYC_REGION_PROFILE
    r->curr->free_bytes = r->curr->free_bytes - s;
    rgn_total_bytes += s;
#endif
      return result;
    }
  } 
  return _region_malloc(r,orig_s); 
}

#ifdef CYC_REGION_PROFILE
/* see macros below for usage. defined in runtime_memory.c */
void* _profile_GC_malloc(int,const char*,const char*,int);
void* _profile_GC_malloc_atomic(int,const char*,const char*,int);
void* _profile_GC_calloc(unsigned,unsigned,const char*,const char*,int);
void* _profile_GC_calloc_atomic(unsigned,unsigned,const char*,const char*,int);
void* _profile_region_malloc(struct _RegionHandle*,unsigned,const char*,const char*,int);
void* _profile_region_calloc(struct _RegionHandle*,unsigned,unsigned,const char *,const char*,int);
struct _RegionHandle _profile_new_region(const char*,const char*,const char*,int);
void _profile_free_region(struct _RegionHandle*,const char*,const char*,int);
#ifndef RUNTIME_CYC
#define _new_region(n) _profile_new_region(n,__FILE__,__FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__,__FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__,__FUNCTION__,__LINE__)
#define _region_calloc(rh,n,t) _profile_region_calloc(rh,n,t,__FILE__,__FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc(n,s) _profile_GC_calloc(n,s,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc_atomic(n,s) _profile_GC_calloc_atomic(n,s,__FILE__,__FUNCTION__,__LINE__)
#endif
#endif
 struct Cyc_Core_Opt{void*v;};
# 95 "core.h"
struct _fat_ptr Cyc_Core_new_string(unsigned);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc___cycFILE;
# 51 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stdout;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 88
extern int Cyc_fflush(struct Cyc___cycFILE*);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr s);
# 72 "string.h"
extern struct _fat_ptr Cyc_strncpy(struct _fat_ptr,struct _fat_ptr,unsigned long);struct Cyc_Lineno_Pos{struct _fat_ptr logical_file;struct _fat_ptr line;int line_no;int col;};
# 31 "lineno.h"
struct Cyc_Lineno_Pos*Cyc_Lineno_pos_of_abs(struct _fat_ptr,int);
# 36 "position.h"
struct _fat_ptr Cyc_Position_string_of_loc(unsigned);
struct _fat_ptr Cyc_Position_string_of_segment(unsigned);struct Cyc_Position_Error;
# 46
extern int Cyc_Position_use_gcc_style_location;
extern int Cyc_Position_num_errors;
extern int Cyc_Position_max_errors;
# 32 "position.cyc"
int Cyc_Position_use_gcc_style_location=1;static char _tmp0[1U]="";
# 35
static struct _fat_ptr Cyc_Position_source={_tmp0,_tmp0,_tmp0 + 1U};
# 37
unsigned Cyc_Position_segment_join(unsigned s1,unsigned s2){
if(s1 == (unsigned)0)return s2;
if(s2 == (unsigned)0)return s1;
return s1;}
# 42
int Cyc_Position_segment_equals(unsigned s1,unsigned s2){
return s1 == s2;}
# 46
struct _fat_ptr Cyc_Position_string_of_loc(unsigned loc){
struct Cyc_Lineno_Pos*pos=Cyc_Lineno_pos_of_abs(Cyc_Position_source,(int)loc);
if(Cyc_Position_use_gcc_style_location)
return({struct Cyc_String_pa_PrintArg_struct _tmp3=({struct Cyc_String_pa_PrintArg_struct _tmp26;_tmp26.tag=0U,_tmp26.f1=(struct _fat_ptr)((struct _fat_ptr)pos->logical_file);_tmp26;});struct Cyc_Int_pa_PrintArg_struct _tmp4=({struct Cyc_Int_pa_PrintArg_struct _tmp25;_tmp25.tag=1U,_tmp25.f1=(unsigned long)pos->line_no;_tmp25;});void*_tmp1[2U];_tmp1[0]=& _tmp3,_tmp1[1]=& _tmp4;({struct _fat_ptr _tmp33=({const char*_tmp2="%s:%d";_tag_fat(_tmp2,sizeof(char),6U);});Cyc_aprintf(_tmp33,_tag_fat(_tmp1,sizeof(void*),2U));});});else{
# 51
return({struct Cyc_String_pa_PrintArg_struct _tmp7=({struct Cyc_String_pa_PrintArg_struct _tmp29;_tmp29.tag=0U,_tmp29.f1=(struct _fat_ptr)((struct _fat_ptr)pos->logical_file);_tmp29;});struct Cyc_Int_pa_PrintArg_struct _tmp8=({struct Cyc_Int_pa_PrintArg_struct _tmp28;_tmp28.tag=1U,_tmp28.f1=(unsigned long)pos->line_no;_tmp28;});struct Cyc_Int_pa_PrintArg_struct _tmp9=({struct Cyc_Int_pa_PrintArg_struct _tmp27;_tmp27.tag=1U,_tmp27.f1=(unsigned long)pos->col;_tmp27;});void*_tmp5[3U];_tmp5[0]=& _tmp7,_tmp5[1]=& _tmp8,_tmp5[2]=& _tmp9;({struct _fat_ptr _tmp34=({const char*_tmp6="%s:(%d:%d)";_tag_fat(_tmp6,sizeof(char),11U);});Cyc_aprintf(_tmp34,_tag_fat(_tmp5,sizeof(void*),3U));});});}}
# 54
static struct _fat_ptr Cyc_Position_string_of_pos_pr(struct Cyc_Lineno_Pos*pos){
if(Cyc_Position_use_gcc_style_location)
return({struct Cyc_String_pa_PrintArg_struct _tmpC=({struct Cyc_String_pa_PrintArg_struct _tmp2B;_tmp2B.tag=0U,_tmp2B.f1=(struct _fat_ptr)((struct _fat_ptr)pos->logical_file);_tmp2B;});struct Cyc_Int_pa_PrintArg_struct _tmpD=({struct Cyc_Int_pa_PrintArg_struct _tmp2A;_tmp2A.tag=1U,_tmp2A.f1=(unsigned long)pos->line_no;_tmp2A;});void*_tmpA[2U];_tmpA[0]=& _tmpC,_tmpA[1]=& _tmpD;({struct _fat_ptr _tmp35=({const char*_tmpB="%s:%d";_tag_fat(_tmpB,sizeof(char),6U);});Cyc_aprintf(_tmp35,_tag_fat(_tmpA,sizeof(void*),2U));});});else{
# 58
return({struct Cyc_String_pa_PrintArg_struct _tmp10=({struct Cyc_String_pa_PrintArg_struct _tmp2E;_tmp2E.tag=0U,_tmp2E.f1=(struct _fat_ptr)((struct _fat_ptr)pos->logical_file);_tmp2E;});struct Cyc_Int_pa_PrintArg_struct _tmp11=({struct Cyc_Int_pa_PrintArg_struct _tmp2D;_tmp2D.tag=1U,_tmp2D.f1=(unsigned long)pos->line_no;_tmp2D;});struct Cyc_Int_pa_PrintArg_struct _tmp12=({struct Cyc_Int_pa_PrintArg_struct _tmp2C;_tmp2C.tag=1U,_tmp2C.f1=(unsigned long)pos->col;_tmp2C;});void*_tmpE[3U];_tmpE[0]=& _tmp10,_tmpE[1]=& _tmp11,_tmpE[2]=& _tmp12;({struct _fat_ptr _tmp36=({const char*_tmpF="%s:(%d:%d)";_tag_fat(_tmpF,sizeof(char),11U);});Cyc_aprintf(_tmp36,_tag_fat(_tmpE,sizeof(void*),3U));});});}}
# 61
struct _fat_ptr Cyc_Position_string_of_segment(unsigned s){
return Cyc_Position_string_of_loc(s);}
# 65
static struct Cyc_Lineno_Pos*Cyc_Position_new_pos (void){
return({struct Cyc_Lineno_Pos*_tmp14=_cycalloc(sizeof(*_tmp14));({struct _fat_ptr _tmp38=({const char*_tmp13="";_tag_fat(_tmp13,sizeof(char),1U);});_tmp14->logical_file=_tmp38;}),({struct _fat_ptr _tmp37=Cyc_Core_new_string(0U);_tmp14->line=_tmp37;}),_tmp14->line_no=0,_tmp14->col=0;_tmp14;});}
# 69
struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*segs){
# 71
struct Cyc_List_List*ans=0;
for(0;segs != 0;segs=segs->tl){
ans=({struct Cyc_List_List*_tmp16=_cycalloc(sizeof(*_tmp16));({struct _fat_ptr*_tmp3A=({struct _fat_ptr*_tmp15=_cycalloc(sizeof(*_tmp15));({struct _fat_ptr _tmp39=Cyc_Position_string_of_segment((unsigned)segs->hd);*_tmp15=_tmp39;});_tmp15;});_tmp16->hd=_tmp3A;}),_tmp16->tl=ans;_tmp16;});}
return ans;}struct Cyc_Position_Error{struct _fat_ptr source;unsigned seg;struct _fat_ptr desc;};
# 97 "position.cyc"
struct Cyc_Position_Error*Cyc_Position_mk_err(unsigned l,struct _fat_ptr desc){
return({struct Cyc_Position_Error*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->source=Cyc_Position_source,_tmp17->seg=l,_tmp17->desc=desc;_tmp17;});}
# 103
static struct _fat_ptr Cyc_Position_trunc(int n,struct _fat_ptr s){
int len=(int)Cyc_strlen((struct _fat_ptr)s);
if(len < n)
return s;{
int len_one=(n - 3)/ 2;
int len_two=(n - 3)- len_one;
struct _fat_ptr mans=Cyc_Core_new_string((unsigned)(n + 1));
struct _fat_ptr ans=_fat_ptr_decrease_size(mans,sizeof(char),1U);
Cyc_strncpy(ans,(struct _fat_ptr)s,(unsigned long)len_one);
({struct _fat_ptr _tmp3B=_fat_ptr_plus(ans,sizeof(char),len_one);Cyc_strncpy(_tmp3B,({const char*_tmp18="...";_tag_fat(_tmp18,sizeof(char),4U);}),3U);});
({struct _fat_ptr _tmp3D=_fat_ptr_plus(ans,sizeof(char),len_one + 3);struct _fat_ptr _tmp3C=(struct _fat_ptr)_fat_ptr_plus(s,sizeof(char),len - len_two);Cyc_strncpy(_tmp3D,_tmp3C,(unsigned long)len_two);});
return mans;}}
# 117
static int Cyc_Position_line_length=76;
# 119
static int Cyc_Position_error_b=0;
int Cyc_Position_error_p (void){return Cyc_Position_error_b;}
# 122
int Cyc_Position_print_context=0;
# 124
int Cyc_Position_first_error=1;
# 126
int Cyc_Position_num_errors=0;
int Cyc_Position_max_errors=10;
# 129
void Cyc_Position_post_error(struct Cyc_Position_Error*e){
Cyc_Position_error_b=1;
Cyc_fflush(Cyc_stdout);
if(Cyc_Position_first_error){
({void*_tmp19=0U;({struct Cyc___cycFILE*_tmp3F=Cyc_stderr;struct _fat_ptr _tmp3E=({const char*_tmp1A="\n";_tag_fat(_tmp1A,sizeof(char),2U);});Cyc_fprintf(_tmp3F,_tmp3E,_tag_fat(_tmp19,sizeof(void*),0U));});});
Cyc_Position_first_error=0;}
# 136
if(Cyc_Position_num_errors <= Cyc_Position_max_errors)
# 138
({struct Cyc_String_pa_PrintArg_struct _tmp1D=({struct Cyc_String_pa_PrintArg_struct _tmp30;_tmp30.tag=0U,({struct _fat_ptr _tmp40=(struct _fat_ptr)((struct _fat_ptr)Cyc_Position_string_of_segment(e->seg));_tmp30.f1=_tmp40;});_tmp30;});struct Cyc_String_pa_PrintArg_struct _tmp1E=({struct Cyc_String_pa_PrintArg_struct _tmp2F;_tmp2F.tag=0U,_tmp2F.f1=(struct _fat_ptr)((struct _fat_ptr)e->desc);_tmp2F;});void*_tmp1B[2U];_tmp1B[0]=& _tmp1D,_tmp1B[1]=& _tmp1E;({struct Cyc___cycFILE*_tmp42=Cyc_stderr;struct _fat_ptr _tmp41=({const char*_tmp1C="%s: %s\n";_tag_fat(_tmp1C,sizeof(char),8U);});Cyc_fprintf(_tmp42,_tmp41,_tag_fat(_tmp1B,sizeof(void*),2U));});});
# 140
if(Cyc_Position_num_errors == Cyc_Position_max_errors)
({void*_tmp1F=0U;({struct Cyc___cycFILE*_tmp44=Cyc_stderr;struct _fat_ptr _tmp43=({const char*_tmp20="Too many error messages!\n";_tag_fat(_tmp20,sizeof(char),26U);});Cyc_fprintf(_tmp44,_tmp43,_tag_fat(_tmp1F,sizeof(void*),0U));});});
# 143
Cyc_fflush(Cyc_stderr);
++ Cyc_Position_num_errors;}
# 148
void Cyc_Position_reset_position(struct _fat_ptr s){Cyc_Position_source=s;Cyc_Position_error_b=0;}
void Cyc_Position_set_position_file(struct _fat_ptr s){Cyc_Position_source=s;Cyc_Position_error_b=0;}
struct _fat_ptr Cyc_Position_get_position_file (void){return Cyc_Position_source;}
# 152
struct _fat_ptr Cyc_Position_get_line_directive(unsigned s){
struct Cyc_Lineno_Pos*pos_s=Cyc_Lineno_pos_of_abs(Cyc_Position_source,(int)s);
if(pos_s != 0)
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp23=({struct Cyc_Int_pa_PrintArg_struct _tmp32;_tmp32.tag=1U,_tmp32.f1=(unsigned long)pos_s->line_no;_tmp32;});struct Cyc_String_pa_PrintArg_struct _tmp24=({struct Cyc_String_pa_PrintArg_struct _tmp31;_tmp31.tag=0U,_tmp31.f1=(struct _fat_ptr)((struct _fat_ptr)pos_s->logical_file);_tmp31;});void*_tmp21[2U];_tmp21[0]=& _tmp23,_tmp21[1]=& _tmp24;({struct _fat_ptr _tmp45=({const char*_tmp22="\n#line %d \"%s\"\n";_tag_fat(_tmp22,sizeof(char),16U);});Cyc_aprintf(_tmp45,_tag_fat(_tmp21,sizeof(void*),2U));});});else{
# 157
return _tag_fat(0,0,0);}}
