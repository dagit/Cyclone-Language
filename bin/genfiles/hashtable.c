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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 336
extern void*Cyc_List_assoc_cmp(int(*)(void*,void*),struct Cyc_List_List*,void*);struct Cyc_Hashtable_Table;
# 47 "hashtable.h"
struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(struct _RegionHandle*,int,int(*)(void*,void*),int(*)(void*));
# 72
void Cyc_Hashtable_resize(struct Cyc_Hashtable_Table*);
# 82
int Cyc_Hashtable_hash_string(struct _fat_ptr);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};
# 157 "cycboot.h"
extern int Cyc_printf(struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_Hashtable_Table{struct _RegionHandle*r;int(*cmp)(void*,void*);int(*hash)(void*);int max_len;struct _fat_ptr tab;};
# 38 "hashtable.cyc"
struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(struct _RegionHandle*r,int sz,int(*cmp)(void*,void*),int(*hash)(void*)){
# 40
return({struct Cyc_Hashtable_Table*_tmp1=_region_malloc(r,sizeof(*_tmp1));_tmp1->r=r,_tmp1->cmp=cmp,_tmp1->hash=hash,_tmp1->max_len=3,({struct _fat_ptr _tmp2B=({unsigned _tmp0=sz;_tag_fat(_region_calloc(r,sizeof(struct Cyc_List_List*),_tmp0),sizeof(struct Cyc_List_List*),_tmp0);});_tmp1->tab=_tmp2B;});_tmp1;});}
# 43
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*)){
return Cyc_Hashtable_rcreate(Cyc_Core_heap_region,sz,cmp,hash);}struct _tuple0{void*f1;void*f2;};
# 47
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val){
struct _fat_ptr tab=t->tab;
int bucket=(int)({unsigned _tmp2C=(unsigned)((t->hash))(key);_tmp2C % _get_fat_size(tab,sizeof(struct Cyc_List_List*));});
({struct Cyc_List_List*_tmp2E=({struct Cyc_List_List*_tmp3=_region_malloc(t->r,sizeof(*_tmp3));({struct _tuple0*_tmp2D=({struct _tuple0*_tmp2=_region_malloc(t->r,sizeof(*_tmp2));_tmp2->f1=key,_tmp2->f2=val;_tmp2;});_tmp3->hd=_tmp2D;}),_tmp3->tl=*((struct Cyc_List_List**)_check_fat_subscript(tab,sizeof(struct Cyc_List_List*),bucket));_tmp3;});((struct Cyc_List_List**)tab.curr)[bucket]=_tmp2E;});
if(({int _tmp2F=((int(*)(struct Cyc_List_List*))Cyc_List_length)(((struct Cyc_List_List**)tab.curr)[bucket]);_tmp2F > t->max_len;}))
Cyc_Hashtable_resize(t);}
# 55
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key){
struct _fat_ptr tab=t->tab;
struct Cyc_List_List*l=({struct Cyc_List_List**_tmp31=(struct Cyc_List_List**)tab.curr;_tmp31[(int)({unsigned _tmp30=(unsigned)((t->hash))(key);_tmp30 % _get_fat_size(tab,sizeof(struct Cyc_List_List*));})];});
return Cyc_List_assoc_cmp(t->cmp,l,key);}
# 61
void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key){
struct _fat_ptr _tmp4=t->tab;struct _fat_ptr tab=_tmp4;
struct Cyc_List_List*_tmp5=({struct Cyc_List_List**_tmp33=(struct Cyc_List_List**)tab.curr;_tmp33[(int)({unsigned _tmp32=(unsigned)((t->hash))(key);_tmp32 % _get_fat_size(tab,sizeof(struct Cyc_List_List*));})];});struct Cyc_List_List*l=_tmp5;
int(*_tmp6)(void*,void*)=t->cmp;int(*cmp)(void*,void*)=_tmp6;
for(1;l != 0;l=l->tl){
struct _tuple0*_tmp7=(struct _tuple0*)l->hd;struct _tuple0*_stmttmp0=_tmp7;struct _tuple0*_tmp8=_stmttmp0;void*_tmpA;void*_tmp9;_tmp9=_tmp8->f1;_tmpA=(void**)& _tmp8->f2;{void*k=_tmp9;void**v=(void**)_tmpA;
if(cmp(key,k)== 0)return v;}}
# 69
return 0;}
# 76
void**Cyc_Hashtable_lookup_other_opt(struct Cyc_Hashtable_Table*t,void*key,int(*cmp)(void*,void*),int(*hash)(void*)){
# 78
struct _fat_ptr _tmpB=t->tab;struct _fat_ptr tab=_tmpB;
struct Cyc_List_List*_tmpC=({struct Cyc_List_List**_tmp35=(struct Cyc_List_List**)tab.curr;_tmp35[(int)({unsigned _tmp34=(unsigned)hash(key);_tmp34 % _get_fat_size(tab,sizeof(struct Cyc_List_List*));})];});struct Cyc_List_List*l=_tmpC;
for(1;l != 0;l=l->tl){
struct _tuple0*_tmpD=(struct _tuple0*)l->hd;struct _tuple0*_stmttmp1=_tmpD;struct _tuple0*_tmpE=_stmttmp1;void*_tmp10;void*_tmpF;_tmpF=_tmpE->f1;_tmp10=(void**)& _tmpE->f2;{void*k=_tmpF;void**v=(void**)_tmp10;
if(cmp(key,k)== 0)return v;}}
# 84
return 0;}
# 87
int Cyc_Hashtable_try_lookup(struct Cyc_Hashtable_Table*t,void*key,void**data){
struct _fat_ptr _tmp11=t->tab;struct _fat_ptr tab=_tmp11;
struct Cyc_List_List*_tmp12=({struct Cyc_List_List**_tmp37=(struct Cyc_List_List**)tab.curr;_tmp37[(int)({unsigned _tmp36=(unsigned)((t->hash))(key);_tmp36 % _get_fat_size(tab,sizeof(struct Cyc_List_List*));})];});struct Cyc_List_List*l=_tmp12;
int(*_tmp13)(void*,void*)=t->cmp;int(*cmp)(void*,void*)=_tmp13;
for(1;l != 0;l=l->tl){
struct _tuple0 _tmp14=*((struct _tuple0*)l->hd);struct _tuple0 _stmttmp2=_tmp14;struct _tuple0 _tmp15=_stmttmp2;void*_tmp17;void*_tmp16;_tmp16=_tmp15.f1;_tmp17=_tmp15.f2;{void*k=_tmp16;void*v=_tmp17;
if(cmp(key,k)== 0){
*data=v;
return 1;}}}
# 98
return 0;}
# 101
void Cyc_Hashtable_remove(struct Cyc_Hashtable_Table*t,void*key){
# 104
struct _fat_ptr _tmp18=t->tab;struct _fat_ptr tab=_tmp18;
int(*_tmp19)(void*,void*)=t->cmp;int(*cmp)(void*,void*)=_tmp19;
int bucket=(int)({unsigned _tmp38=(unsigned)((t->hash))(key);_tmp38 % _get_fat_size(tab,sizeof(struct Cyc_List_List*));});
struct Cyc_List_List*_tmp1A=*((struct Cyc_List_List**)_check_fat_subscript(tab,sizeof(struct Cyc_List_List*),bucket));struct Cyc_List_List*l=_tmp1A;
if(l == 0)return;
if(cmp(key,(((struct _tuple0*)l->hd)[0]).f1)== 0){
((struct Cyc_List_List**)tab.curr)[bucket]=l->tl;
return;}{
# 113
struct Cyc_List_List*_tmp1B=l->tl;struct Cyc_List_List*next=_tmp1B;for(0;((struct Cyc_List_List*)_check_null(l))->tl != 0;(l=l->tl,next=next->tl)){
# 115
if(cmp(key,(((struct _tuple0*)((struct Cyc_List_List*)_check_null(next))->hd)[0]).f1)== 0){
l->tl=next->tl;
return;}}}}
# 121
int Cyc_Hashtable_hash_string(struct _fat_ptr s){
# 123
int ans=0;
int sz=(int)_get_fat_size(s,sizeof(char));
int shift=0;
{int i=0;for(0;i < sz;++ i){
ans=ans ^ (int)*((const char*)_check_fat_subscript(s,sizeof(char),i))<< shift;
shift +=8;
if(shift == 32)
shift=0;}}
# 132
return ans;}
# 135
int Cyc_Hashtable_hash_stringptr(struct _fat_ptr*s){
return Cyc_Hashtable_hash_string(*s);}
# 140
void Cyc_Hashtable_insert_bucket(struct _RegionHandle*r,struct _fat_ptr tab,int(*hash)(void*),struct Cyc_List_List*elems){
# 143
if(elems == 0)return;
Cyc_Hashtable_insert_bucket(r,tab,hash,elems->tl);{
void*key=(((struct _tuple0*)elems->hd)[0]).f1;
void*val=(((struct _tuple0*)elems->hd)[0]).f2;
int nidx=(int)({unsigned _tmp39=(unsigned)hash(key);_tmp39 % _get_fat_size(tab,sizeof(struct Cyc_List_List*));});
({struct Cyc_List_List*_tmp3B=({struct Cyc_List_List*_tmp1D=_region_malloc(r,sizeof(*_tmp1D));({struct _tuple0*_tmp3A=({struct _tuple0*_tmp1C=_region_malloc(r,sizeof(*_tmp1C));_tmp1C->f1=key,_tmp1C->f2=val;_tmp1C;});_tmp1D->hd=_tmp3A;}),_tmp1D->tl=*((struct Cyc_List_List**)_check_fat_subscript(tab,sizeof(struct Cyc_List_List*),nidx));_tmp1D;});((struct Cyc_List_List**)tab.curr)[nidx]=_tmp3B;});}}
# 151
void Cyc_Hashtable_resize(struct Cyc_Hashtable_Table*t){
struct _fat_ptr odata=t->tab;
int osize=(int)_get_fat_size(odata,sizeof(struct Cyc_List_List*));
int nsize=2 * osize + 1;
struct _fat_ptr ndata=({unsigned _tmp1E=nsize;_tag_fat(_region_calloc(t->r,sizeof(struct Cyc_List_List*),_tmp1E),sizeof(struct Cyc_List_List*),_tmp1E);});
{int i=0;for(0;i < osize;++ i){
Cyc_Hashtable_insert_bucket(t->r,ndata,t->hash,*((struct Cyc_List_List**)_check_fat_subscript(odata,sizeof(struct Cyc_List_List*),i)));}}
t->tab=ndata;
t->max_len=2 * t->max_len;}
# 163
void Cyc_Hashtable_iter(void(*f)(void*,void*),struct Cyc_Hashtable_Table*t){
struct _fat_ptr odata=t->tab;
int osize=(int)_get_fat_size(odata,sizeof(struct Cyc_List_List*));
int i=0;for(0;i < osize;++ i){
struct Cyc_List_List*iter=*((struct Cyc_List_List**)_check_fat_subscript(odata,sizeof(struct Cyc_List_List*),i));for(0;iter != 0;iter=iter->tl){
f((((struct _tuple0*)iter->hd)[0]).f1,(((struct _tuple0*)iter->hd)[0]).f2);}}}
# 171
void Cyc_Hashtable_iter_c(void(*f)(void*,void*,void*),struct Cyc_Hashtable_Table*t,void*env){
struct _fat_ptr odata=t->tab;
int osize=(int)_get_fat_size(odata,sizeof(struct Cyc_List_List*));
int i=0;for(0;i < osize;++ i){
struct Cyc_List_List*iter=*((struct Cyc_List_List**)_check_fat_subscript(odata,sizeof(struct Cyc_List_List*),i));for(0;iter != 0;iter=iter->tl){
f((((struct _tuple0*)iter->hd)[0]).f1,(((struct _tuple0*)iter->hd)[0]).f2,env);}}}
# 180
void Cyc_Hashtable_print_table_map(struct Cyc_Hashtable_Table*t,void(*prn_key)(void*),void(*prn_val)(void*)){
# 182
struct _fat_ptr odata=t->tab;
int osize=(int)_get_fat_size(odata,sizeof(struct Cyc_List_List*));
int i=0;for(0;i < osize;++ i){
({struct Cyc_Int_pa_PrintArg_struct _tmp21=({struct Cyc_Int_pa_PrintArg_struct _tmp2A;_tmp2A.tag=1,_tmp2A.f1=(unsigned long)i;_tmp2A;});void*_tmp1F[1];_tmp1F[0]=& _tmp21;({struct _fat_ptr _tmp3C=({const char*_tmp20="%d: ";_tag_fat(_tmp20,sizeof(char),5U);});Cyc_printf(_tmp3C,_tag_fat(_tmp1F,sizeof(void*),1));});});
{struct Cyc_List_List*iter=*((struct Cyc_List_List**)_check_fat_subscript(odata,sizeof(struct Cyc_List_List*),i));for(0;iter != 0;iter=iter->tl){
({void*_tmp22=0U;({struct _fat_ptr _tmp3D=({const char*_tmp23="(";_tag_fat(_tmp23,sizeof(char),2U);});Cyc_printf(_tmp3D,_tag_fat(_tmp22,sizeof(void*),0));});});
prn_key((((struct _tuple0*)iter->hd)[0]).f1);
({void*_tmp24=0U;({struct _fat_ptr _tmp3E=({const char*_tmp25=",";_tag_fat(_tmp25,sizeof(char),2U);});Cyc_printf(_tmp3E,_tag_fat(_tmp24,sizeof(void*),0));});});
prn_val((((struct _tuple0*)iter->hd)[0]).f2);
({void*_tmp26=0U;({struct _fat_ptr _tmp3F=({const char*_tmp27=") ";_tag_fat(_tmp27,sizeof(char),3U);});Cyc_printf(_tmp3F,_tag_fat(_tmp26,sizeof(void*),0));});});}}
# 193
({void*_tmp28=0U;({struct _fat_ptr _tmp40=({const char*_tmp29="\n";_tag_fat(_tmp29,sizeof(char),2U);});Cyc_printf(_tmp40,_tag_fat(_tmp28,sizeof(void*),0));});});}}
