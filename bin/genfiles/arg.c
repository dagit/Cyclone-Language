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
 struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};
# 197 "cycboot.h"
extern int Cyc_sscanf(struct _fat_ptr,struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];
# 300 "cycboot.h"
extern int isspace(int);extern char Cyc_Core_Invalid_argument[17U];extern char Cyc_Core_Failure[8U];extern char Cyc_Core_Impossible[11U];extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];
# 171 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];extern char Cyc_List_Nth[4U];
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
# 51
extern int Cyc_strncmp(struct _fat_ptr,struct _fat_ptr,unsigned long);
# 62
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);extern char Cyc_Arg_Bad[4U];struct Cyc_Arg_Bad_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Arg_Error[6U];struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)(void);};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};struct Cyc_Arg_FlagString_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr,struct _fat_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};struct Cyc_Arg_Rest_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};
# 66 "arg.h"
void Cyc_Arg_usage(struct Cyc_List_List*,struct _fat_ptr);
# 69
extern int Cyc_Arg_current;struct Cyc_Buffer_t;
# 50 "buffer.h"
extern struct Cyc_Buffer_t*Cyc_Buffer_create(unsigned);
# 58
extern struct _fat_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*);
# 83
extern void Cyc_Buffer_add_substring(struct Cyc_Buffer_t*,struct _fat_ptr,int,int);
# 92 "buffer.h"
extern void Cyc_Buffer_add_string(struct Cyc_Buffer_t*,struct _fat_ptr);
# 29 "assert.h"
extern void*Cyc___assert_fail(struct _fat_ptr,struct _fat_ptr,unsigned);char Cyc_Arg_Bad[4U]="Bad";char Cyc_Arg_Error[6U]="Error";struct Cyc_Arg_Unknown_Arg_error_struct{int tag;struct _fat_ptr f1;};struct Cyc_Arg_Missing_Arg_error_struct{int tag;struct _fat_ptr f1;};struct Cyc_Arg_Message_Arg_error_struct{int tag;struct _fat_ptr f1;};struct Cyc_Arg_Wrong_Arg_error_struct{int tag;struct _fat_ptr f1;struct _fat_ptr f2;struct _fat_ptr f3;};struct _tuple0{struct _fat_ptr f1;int f2;struct _fat_ptr f3;void*f4;struct _fat_ptr f5;};
# 68 "arg.cyc"
static void*Cyc_Arg_lookup(struct Cyc_List_List*l,struct _fat_ptr x){
while(l != 0){
struct _fat_ptr _tmp0=(*((struct _tuple0*)l->hd)).f1;struct _fat_ptr flag=_tmp0;
unsigned long _tmp1=Cyc_strlen((struct _fat_ptr)flag);unsigned long len=_tmp1;
if(len > 0U &&(*((struct _tuple0*)l->hd)).f2){
if(Cyc_strncmp((struct _fat_ptr)x,(struct _fat_ptr)(*((struct _tuple0*)l->hd)).f1,len)== 0)
return(*((struct _tuple0*)l->hd)).f4;}else{
# 76
if(Cyc_strcmp((struct _fat_ptr)x,(struct _fat_ptr)(*((struct _tuple0*)l->hd)).f1)== 0)
return(*((struct _tuple0*)l->hd)).f4;}
l=l->tl;}
# 80
(int)_throw((void*)({struct Cyc_Core_Not_found_exn_struct*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->tag=Cyc_Core_Not_found;_tmp2;}));}
# 89
static struct _fat_ptr Cyc_Arg_Justify_break_line(struct Cyc_Buffer_t*b,int howmuch,struct _fat_ptr s){
if((char*)s.curr == (char*)(_tag_fat(0,0,0)).curr)return _tag_fat(0,0,0);
if(howmuch < 0)howmuch=0;{
unsigned long _tmp3=Cyc_strlen((struct _fat_ptr)s);unsigned long len=_tmp3;
len <= _get_fat_size(s,sizeof(char))?0:({int(*_tmp5E)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmp5D=({const char*_tmp4="len <= numelts(s)";_tag_fat(_tmp4,sizeof(char),18U);});_tmp5E(_tmp5D,({const char*_tmp5="arg.cyc";_tag_fat(_tmp5,sizeof(char),8U);}),93U);});
if((unsigned long)howmuch > len){
Cyc_Buffer_add_string(b,s);
return _tag_fat(0,0,0);}{
# 100
int i;
for(i=howmuch - 1;i >= 0 && !isspace((int)((const char*)s.curr)[i]);-- i){
;}
# 105
if(i < 0)
for(i=howmuch?howmuch - 1: 0;((unsigned long)i < len &&(int)((const char*)s.curr)[i])&& !isspace((int)((const char*)s.curr)[i]);++ i){
;}
# 111
Cyc_Buffer_add_substring(b,s,0,i);{
# 114
struct _fat_ptr whatsleft=_tag_fat(0,0,0);
# 116
for(1;((unsigned long)i < len &&(int)((const char*)s.curr)[i])&& isspace((int)((const char*)s.curr)[i]);++ i){
;}
if((unsigned long)i < len &&(int)((const char*)s.curr)[i])whatsleft=_fat_ptr_plus(s,sizeof(char),i);
return whatsleft;}}}}
# 127
void Cyc_Arg_Justify_justify_b(struct Cyc_Buffer_t*b,int indent,int margin,struct _fat_ptr item,struct _fat_ptr desc){
# 129
if((char*)item.curr != (char*)(_tag_fat(0,0,0)).curr)Cyc_Buffer_add_string(b,item);
if((char*)desc.curr == (char*)(_tag_fat(0,0,0)).curr)return;
if(indent < 0)indent=0;
if(margin < 0)margin=0;{
# 135
struct _fat_ptr indentstr=({unsigned _tmp11=(unsigned)(indent + 2)+ 1U;char*_tmp10=_cycalloc_atomic(_check_times(_tmp11,sizeof(char)));({{unsigned _tmp4F=(unsigned)(indent + 2);unsigned i;for(i=0;i < _tmp4F;++ i){_tmp10[i]='\000';}_tmp10[_tmp4F]=0;}0;});_tag_fat(_tmp10,sizeof(char),_tmp11);});
{unsigned i=0U;for(0;i < (unsigned)(indent + 1);++ i){
({struct _fat_ptr _tmp6=_fat_ptr_plus(indentstr,sizeof(char),(int)i);char _tmp7=*((char*)_check_fat_subscript(_tmp6,sizeof(char),0U));char _tmp8=i == 0U?'\n':' ';if(_get_fat_size(_tmp6,sizeof(char))== 1U &&(_tmp7 == 0 && _tmp8 != 0))_throw_arraybounds();*((char*)_tmp6.curr)=_tmp8;});}}{
unsigned long _tmp9=Cyc_strlen((struct _fat_ptr)item);unsigned long itemlen=_tmp9;
struct _fat_ptr itemsep;
if(Cyc_strlen((struct _fat_ptr)desc)> 0U){
if(itemlen + 1U > (unsigned long)indent)
itemsep=indentstr;else{
# 144
struct _fat_ptr temp=({unsigned _tmpE=(((unsigned long)indent - itemlen)+ 1U)+ 1U;char*_tmpD=_cycalloc_atomic(_check_times(_tmpE,sizeof(char)));({{unsigned _tmp4E=((unsigned long)indent - itemlen)+ 1U;unsigned i;for(i=0;i < _tmp4E;++ i){_tmpD[i]='\000';}_tmpD[_tmp4E]=0;}0;});_tag_fat(_tmpD,sizeof(char),_tmpE);});
{unsigned i=0U;for(0;i < (unsigned long)indent - itemlen;++ i){({struct _fat_ptr _tmpA=_fat_ptr_plus(temp,sizeof(char),(int)i);char _tmpB=*((char*)_check_fat_subscript(_tmpA,sizeof(char),0U));char _tmpC=' ';if(_get_fat_size(_tmpA,sizeof(char))== 1U &&(_tmpB == 0 && _tmpC != 0))_throw_arraybounds();*((char*)_tmpA.curr)=_tmpC;});}}
itemsep=temp;}}else{
# 149
return;}
# 151
Cyc_Buffer_add_string(b,(struct _fat_ptr)itemsep);
# 153
while((char*)desc.curr != (char*)(_tag_fat(0,0,0)).curr){
desc=Cyc_Arg_Justify_break_line(b,margin - indent,desc);
if((char*)desc.curr != (char*)(_tag_fat(0,0,0)).curr)
Cyc_Buffer_add_string(b,(struct _fat_ptr)indentstr);else{
# 158
({struct Cyc_Buffer_t*_tmp5F=b;Cyc_Buffer_add_string(_tmp5F,({const char*_tmpF="\n";_tag_fat(_tmpF,sizeof(char),2U);}));});}}
# 160
return;}}}
# 164
void Cyc_Arg_usage(struct Cyc_List_List*speclist,struct _fat_ptr errmsg){
# 166
({struct Cyc_String_pa_PrintArg_struct _tmp14=({struct Cyc_String_pa_PrintArg_struct _tmp50;_tmp50.tag=0,_tmp50.f1=(struct _fat_ptr)((struct _fat_ptr)errmsg);_tmp50;});void*_tmp12[1];_tmp12[0]=& _tmp14;({struct Cyc___cycFILE*_tmp61=Cyc_stderr;struct _fat_ptr _tmp60=({const char*_tmp13="%s\n";_tag_fat(_tmp13,sizeof(char),4U);});Cyc_fprintf(_tmp61,_tmp60,_tag_fat(_tmp12,sizeof(void*),1));});});{
struct Cyc_Buffer_t*_tmp15=Cyc_Buffer_create(1024U);struct Cyc_Buffer_t*b=_tmp15;
while(speclist != 0){
({struct Cyc_Buffer_t*_tmp63=b;struct _fat_ptr _tmp62=(struct _fat_ptr)
Cyc_strconcat((struct _fat_ptr)(*((struct _tuple0*)speclist->hd)).f1,(struct _fat_ptr)(*((struct _tuple0*)speclist->hd)).f3);
# 169
Cyc_Arg_Justify_justify_b(_tmp63,12,72,_tmp62,(*((struct _tuple0*)speclist->hd)).f5);});
# 172
speclist=speclist->tl;}
# 174
({struct Cyc_String_pa_PrintArg_struct _tmp18=({struct Cyc_String_pa_PrintArg_struct _tmp51;_tmp51.tag=0,({struct _fat_ptr _tmp64=(struct _fat_ptr)((struct _fat_ptr)Cyc_Buffer_contents(b));_tmp51.f1=_tmp64;});_tmp51;});void*_tmp16[1];_tmp16[0]=& _tmp18;({struct Cyc___cycFILE*_tmp66=Cyc_stderr;struct _fat_ptr _tmp65=({const char*_tmp17="%s";_tag_fat(_tmp17,sizeof(char),3U);});Cyc_fprintf(_tmp66,_tmp65,_tag_fat(_tmp16,sizeof(void*),1));});});}}
# 177
int Cyc_Arg_current=0;
static struct _fat_ptr Cyc_Arg_args={(void*)0,(void*)0,(void*)(0 + 0)};
# 180
static void Cyc_Arg_stop(int prog_pos,void*e,struct Cyc_List_List*speclist,struct _fat_ptr errmsg){
# 183
struct _fat_ptr progname=(unsigned)prog_pos < _get_fat_size(Cyc_Arg_args,sizeof(struct _fat_ptr))?((struct _fat_ptr*)Cyc_Arg_args.curr)[prog_pos]:({const char*_tmp30="(?)";_tag_fat(_tmp30,sizeof(char),4U);});
{void*_tmp19=e;struct _fat_ptr _tmp1C;struct _fat_ptr _tmp1B;struct _fat_ptr _tmp1A;switch(*((int*)_tmp19)){case 0: _tmp1A=((struct Cyc_Arg_Unknown_Arg_error_struct*)_tmp19)->f1;{struct _fat_ptr s=_tmp1A;
# 186
if(({struct _fat_ptr _tmp67=(struct _fat_ptr)s;Cyc_strcmp(_tmp67,({const char*_tmp1D="-help";_tag_fat(_tmp1D,sizeof(char),6U);}));})!= 0)
({struct Cyc_String_pa_PrintArg_struct _tmp20=({struct Cyc_String_pa_PrintArg_struct _tmp53;_tmp53.tag=0,_tmp53.f1=(struct _fat_ptr)((struct _fat_ptr)progname);_tmp53;});struct Cyc_String_pa_PrintArg_struct _tmp21=({struct Cyc_String_pa_PrintArg_struct _tmp52;_tmp52.tag=0,_tmp52.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp52;});void*_tmp1E[2];_tmp1E[0]=& _tmp20,_tmp1E[1]=& _tmp21;({struct Cyc___cycFILE*_tmp69=Cyc_stderr;struct _fat_ptr _tmp68=({const char*_tmp1F="%s: unknown option `%s'.\n";_tag_fat(_tmp1F,sizeof(char),26U);});Cyc_fprintf(_tmp69,_tmp68,_tag_fat(_tmp1E,sizeof(void*),2));});});
goto _LL0;}case 1: _tmp1A=((struct Cyc_Arg_Missing_Arg_error_struct*)_tmp19)->f1;{struct _fat_ptr s=_tmp1A;
# 190
({struct Cyc_String_pa_PrintArg_struct _tmp24=({struct Cyc_String_pa_PrintArg_struct _tmp55;_tmp55.tag=0,_tmp55.f1=(struct _fat_ptr)((struct _fat_ptr)progname);_tmp55;});struct Cyc_String_pa_PrintArg_struct _tmp25=({struct Cyc_String_pa_PrintArg_struct _tmp54;_tmp54.tag=0,_tmp54.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp54;});void*_tmp22[2];_tmp22[0]=& _tmp24,_tmp22[1]=& _tmp25;({struct Cyc___cycFILE*_tmp6B=Cyc_stderr;struct _fat_ptr _tmp6A=({const char*_tmp23="%s: option `%s' needs an argument.\n";_tag_fat(_tmp23,sizeof(char),36U);});Cyc_fprintf(_tmp6B,_tmp6A,_tag_fat(_tmp22,sizeof(void*),2));});});
goto _LL0;}case 3: _tmp1A=((struct Cyc_Arg_Wrong_Arg_error_struct*)_tmp19)->f1;_tmp1B=((struct Cyc_Arg_Wrong_Arg_error_struct*)_tmp19)->f2;_tmp1C=((struct Cyc_Arg_Wrong_Arg_error_struct*)_tmp19)->f3;{struct _fat_ptr flag=_tmp1A;struct _fat_ptr cmd=_tmp1B;struct _fat_ptr t=_tmp1C;
# 193
({struct Cyc_String_pa_PrintArg_struct _tmp28=({struct Cyc_String_pa_PrintArg_struct _tmp59;_tmp59.tag=0,_tmp59.f1=(struct _fat_ptr)((struct _fat_ptr)progname);_tmp59;});struct Cyc_String_pa_PrintArg_struct _tmp29=({struct Cyc_String_pa_PrintArg_struct _tmp58;_tmp58.tag=0,_tmp58.f1=(struct _fat_ptr)((struct _fat_ptr)cmd);_tmp58;});struct Cyc_String_pa_PrintArg_struct _tmp2A=({struct Cyc_String_pa_PrintArg_struct _tmp57;_tmp57.tag=0,_tmp57.f1=(struct _fat_ptr)((struct _fat_ptr)flag);_tmp57;});struct Cyc_String_pa_PrintArg_struct _tmp2B=({struct Cyc_String_pa_PrintArg_struct _tmp56;_tmp56.tag=0,_tmp56.f1=(struct _fat_ptr)((struct _fat_ptr)t);_tmp56;});void*_tmp26[4];_tmp26[0]=& _tmp28,_tmp26[1]=& _tmp29,_tmp26[2]=& _tmp2A,_tmp26[3]=& _tmp2B;({struct Cyc___cycFILE*_tmp6D=Cyc_stderr;struct _fat_ptr _tmp6C=({const char*_tmp27="%s: wrong argument `%s'; option `%s' expects %s.\n";_tag_fat(_tmp27,sizeof(char),50U);});Cyc_fprintf(_tmp6D,_tmp6C,_tag_fat(_tmp26,sizeof(void*),4));});});
# 195
goto _LL0;}default: _tmp1A=((struct Cyc_Arg_Message_Arg_error_struct*)_tmp19)->f1;{struct _fat_ptr s=_tmp1A;
# 197
({struct Cyc_String_pa_PrintArg_struct _tmp2E=({struct Cyc_String_pa_PrintArg_struct _tmp5B;_tmp5B.tag=0,_tmp5B.f1=(struct _fat_ptr)((struct _fat_ptr)progname);_tmp5B;});struct Cyc_String_pa_PrintArg_struct _tmp2F=({struct Cyc_String_pa_PrintArg_struct _tmp5A;_tmp5A.tag=0,_tmp5A.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5A;});void*_tmp2C[2];_tmp2C[0]=& _tmp2E,_tmp2C[1]=& _tmp2F;({struct Cyc___cycFILE*_tmp6F=Cyc_stderr;struct _fat_ptr _tmp6E=({const char*_tmp2D="%s: %s.\n";_tag_fat(_tmp2D,sizeof(char),9U);});Cyc_fprintf(_tmp6F,_tmp6E,_tag_fat(_tmp2C,sizeof(void*),2));});});
goto _LL0;}}_LL0:;}
# 200
Cyc_Arg_usage(speclist,errmsg);
Cyc_Arg_current=(int)_get_fat_size(Cyc_Arg_args,sizeof(struct _fat_ptr));}
# 204
void Cyc_Arg_parse(struct Cyc_List_List*speclist,void(*anonfun)(struct _fat_ptr),int(*anonflagfun)(struct _fat_ptr),struct _fat_ptr errmsg,struct _fat_ptr orig_args){
# 209
Cyc_Arg_args=orig_args;{
# 211
int initpos=Cyc_Arg_current;
unsigned l=_get_fat_size(Cyc_Arg_args,sizeof(struct _fat_ptr));
# 214
if((char*)((struct _fat_ptr*)_check_fat_subscript(Cyc_Arg_args,sizeof(struct _fat_ptr),(int)(l - 1U)))->curr == (char*)(_tag_fat(0,0,0)).curr)l=l - 1U;
++ Cyc_Arg_current;
while((unsigned)Cyc_Arg_current < l){
struct _fat_ptr s=*((struct _fat_ptr*)_check_fat_subscript(Cyc_Arg_args,sizeof(struct _fat_ptr),Cyc_Arg_current));
if(((char*)s.curr != (char*)(_tag_fat(0,0,0)).curr && _get_fat_size(s,sizeof(char))>= 1U)&&(int)((const char*)s.curr)[0]== 45){
void*action;
{struct _handler_cons _tmp31;_push_handler(& _tmp31);{int _tmp33=0;if(setjmp(_tmp31.handler))_tmp33=1;if(!_tmp33){action=Cyc_Arg_lookup(speclist,s);;_pop_handler();}else{void*_tmp32=(void*)Cyc_Core_get_exn_thrown();void*_tmp34=_tmp32;void*_tmp35;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp34)->tag == Cyc_Core_Not_found){
# 224
if(!anonflagfun(s)){
({int _tmp72=initpos;void*_tmp71=(void*)({struct Cyc_Arg_Unknown_Arg_error_struct*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36->tag=0,_tmp36->f1=s;_tmp36;});struct Cyc_List_List*_tmp70=speclist;Cyc_Arg_stop(_tmp72,_tmp71,_tmp70,errmsg);});
return;}else{
# 229
++ Cyc_Arg_current;
continue;}}else{_tmp35=_tmp34;{void*exn=_tmp35;(int)_rethrow(exn);}};}}}
# 233
{struct _handler_cons _tmp37;_push_handler(& _tmp37);{int _tmp39=0;if(setjmp(_tmp37.handler))_tmp39=1;if(!_tmp39){
{void*_tmp3A=action;void*_tmp3B;switch(*((int*)_tmp3A)){case 0: _tmp3B=((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)_tmp3A)->f1;{void(*f)(void)=_tmp3B;
f();goto _LL5;}case 1: _tmp3B=((struct Cyc_Arg_Flag_spec_Arg_Spec_struct*)_tmp3A)->f1;{void(*f)(struct _fat_ptr)=_tmp3B;
f(s);goto _LL5;}case 3: _tmp3B=((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)_tmp3A)->f1;{int*r=_tmp3B;
*r=1;goto _LL5;}case 4: _tmp3B=((struct Cyc_Arg_Clear_spec_Arg_Spec_struct*)_tmp3A)->f1;{int*r=_tmp3B;
*r=0;goto _LL5;}case 2: _tmp3B=((struct Cyc_Arg_FlagString_spec_Arg_Spec_struct*)_tmp3A)->f1;{void(*f)(struct _fat_ptr,struct _fat_ptr)=_tmp3B;
# 240
if((unsigned)(Cyc_Arg_current + 1)< l){
f(s,*((struct _fat_ptr*)_check_fat_subscript(Cyc_Arg_args,sizeof(struct _fat_ptr),Cyc_Arg_current + 1)));
++ Cyc_Arg_current;}else{
# 244
({int _tmp75=initpos;void*_tmp74=(void*)({struct Cyc_Arg_Missing_Arg_error_struct*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C->tag=1,_tmp3C->f1=s;_tmp3C;});struct Cyc_List_List*_tmp73=speclist;Cyc_Arg_stop(_tmp75,_tmp74,_tmp73,errmsg);});}
goto _LL5;}case 5: _tmp3B=((struct Cyc_Arg_String_spec_Arg_Spec_struct*)_tmp3A)->f1;{void(*f)(struct _fat_ptr)=_tmp3B;
# 247
if((unsigned)(Cyc_Arg_current + 1)< l){
f(*((struct _fat_ptr*)_check_fat_subscript(Cyc_Arg_args,sizeof(struct _fat_ptr),Cyc_Arg_current + 1)));
++ Cyc_Arg_current;}else{
# 251
({int _tmp78=initpos;void*_tmp77=(void*)({struct Cyc_Arg_Missing_Arg_error_struct*_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D->tag=1,_tmp3D->f1=s;_tmp3D;});struct Cyc_List_List*_tmp76=speclist;Cyc_Arg_stop(_tmp78,_tmp77,_tmp76,errmsg);});}
goto _LL5;}case 6: _tmp3B=((struct Cyc_Arg_Int_spec_Arg_Spec_struct*)_tmp3A)->f1;{void(*f)(int)=_tmp3B;
# 254
struct _fat_ptr arg=*((struct _fat_ptr*)_check_fat_subscript(Cyc_Arg_args,sizeof(struct _fat_ptr),Cyc_Arg_current + 1));
int n=0;
if(({struct Cyc_IntPtr_sa_ScanfArg_struct _tmp40=({struct Cyc_IntPtr_sa_ScanfArg_struct _tmp5C;_tmp5C.tag=2,_tmp5C.f1=& n;_tmp5C;});void*_tmp3E[1];_tmp3E[0]=& _tmp40;({struct _fat_ptr _tmp7A=arg;struct _fat_ptr _tmp79=({const char*_tmp3F="%d";_tag_fat(_tmp3F,sizeof(char),3U);});Cyc_sscanf(_tmp7A,_tmp79,_tag_fat(_tmp3E,sizeof(void*),1));});})!= 1){
({int _tmp7E=initpos;void*_tmp7D=(void*)({struct Cyc_Arg_Wrong_Arg_error_struct*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->tag=3,_tmp42->f1=s,_tmp42->f2=arg,({struct _fat_ptr _tmp7B=({const char*_tmp41="an integer";_tag_fat(_tmp41,sizeof(char),11U);});_tmp42->f3=_tmp7B;});_tmp42;});struct Cyc_List_List*_tmp7C=speclist;Cyc_Arg_stop(_tmp7E,_tmp7D,_tmp7C,errmsg);});
_npop_handler(0);return;}
# 260
f(n);
++ Cyc_Arg_current;
goto _LL5;}default: _tmp3B=((struct Cyc_Arg_Rest_spec_Arg_Spec_struct*)_tmp3A)->f1;{void(*f)(struct _fat_ptr)=_tmp3B;
# 264
while((unsigned)Cyc_Arg_current < l - 1U){
f(*((struct _fat_ptr*)_check_fat_subscript(Cyc_Arg_args,sizeof(struct _fat_ptr),Cyc_Arg_current + 1)));
++ Cyc_Arg_current;}
# 268
goto _LL5;}}_LL5:;}
# 234
;_pop_handler();}else{void*_tmp38=(void*)Cyc_Core_get_exn_thrown();void*_tmp43=_tmp38;void*_tmp44;struct _fat_ptr _tmp45;if(((struct Cyc_Arg_Bad_exn_struct*)_tmp43)->tag == Cyc_Arg_Bad){_tmp45=((struct Cyc_Arg_Bad_exn_struct*)_tmp43)->f1;{struct _fat_ptr s2=_tmp45;
# 271
({int _tmp81=initpos;void*_tmp80=(void*)({struct Cyc_Arg_Message_Arg_error_struct*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46->tag=2,_tmp46->f1=s2;_tmp46;});struct Cyc_List_List*_tmp7F=speclist;Cyc_Arg_stop(_tmp81,_tmp80,_tmp7F,errmsg);});goto _LL16;}}else{_tmp44=_tmp43;{void*exn=_tmp44;(int)_rethrow(exn);}}_LL16:;}}}
# 273
++ Cyc_Arg_current;}else{
# 276
{struct _handler_cons _tmp47;_push_handler(& _tmp47);{int _tmp49=0;if(setjmp(_tmp47.handler))_tmp49=1;if(!_tmp49){anonfun(s);;_pop_handler();}else{void*_tmp48=(void*)Cyc_Core_get_exn_thrown();void*_tmp4A=_tmp48;void*_tmp4B;struct _fat_ptr _tmp4C;if(((struct Cyc_Arg_Bad_exn_struct*)_tmp4A)->tag == Cyc_Arg_Bad){_tmp4C=((struct Cyc_Arg_Bad_exn_struct*)_tmp4A)->f1;{struct _fat_ptr s2=_tmp4C;
# 278
({int _tmp84=initpos;void*_tmp83=(void*)({struct Cyc_Arg_Message_Arg_error_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->tag=2,_tmp4D->f1=s2;_tmp4D;});struct Cyc_List_List*_tmp82=speclist;Cyc_Arg_stop(_tmp84,_tmp83,_tmp82,errmsg);});goto _LL1B;}}else{_tmp4B=_tmp4A;{void*exn=_tmp4B;(int)_rethrow(exn);}}_LL1B:;}}}
# 280
++ Cyc_Arg_current;}}}}
