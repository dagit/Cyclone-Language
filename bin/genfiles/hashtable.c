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
  /* MWH: wish we didn't have to include the stuff below ... */
  struct _RegionPage *next;
  char data[1];
}
#endif
; // abstract -- defined in runtime_memory.c
struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
  struct _DynRegionHandle *sub_regions;
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
# 165 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 336
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc_Hashtable_Table;
# 47 "hashtable.h"
struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(struct _RegionHandle*r,int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 64
void Cyc_Hashtable_resize(struct Cyc_Hashtable_Table*t);
# 74
int Cyc_Hashtable_hash_string(struct _fat_ptr s);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};
# 157 "cycboot.h"
int Cyc_printf(struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_Hashtable_Table{struct _RegionHandle*r;int(*cmp)(void*,void*);int(*hash)(void*);int max_len;struct _fat_ptr tab;};
# 39 "hashtable.cyc"
struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(struct _RegionHandle*r,int sz,int(*cmp)(void*,void*),int(*hash)(void*)){
# 41
return({struct Cyc_Hashtable_Table*_tmp1=_region_malloc(r,sizeof(*_tmp1));_tmp1->r=r,_tmp1->cmp=cmp,_tmp1->hash=hash,_tmp1->max_len=3,({struct _fat_ptr _tmp25=({unsigned _tmp0=sz;_tag_fat(_region_calloc(r,sizeof(struct Cyc_List_List*),_tmp0),sizeof(struct Cyc_List_List*),_tmp0);});_tmp1->tab=_tmp25;});_tmp1;});}
# 44
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*)){
# 46
return Cyc_Hashtable_rcreate(Cyc_Core_heap_region,sz,cmp,hash);}struct _tuple0{void*f1;void*f2;};
# 49
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val){
struct _fat_ptr tab=t->tab;
int bucket=(int)({unsigned _tmp26=(unsigned)(t->hash)(key);_tmp26 % _get_fat_size(tab,sizeof(struct Cyc_List_List*));});
({struct Cyc_List_List*_tmp28=({struct Cyc_List_List*_tmp3=_region_malloc(t->r,sizeof(*_tmp3));({struct _tuple0*_tmp27=({struct _tuple0*_tmp2=_region_malloc(t->r,sizeof(*_tmp2));_tmp2->f1=key,_tmp2->f2=val;_tmp2;});_tmp3->hd=_tmp27;}),_tmp3->tl=*((struct Cyc_List_List**)_check_fat_subscript(tab,sizeof(struct Cyc_List_List*),bucket));_tmp3;});((struct Cyc_List_List**)tab.curr)[bucket]=_tmp28;});
if(({int _tmp29=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_List_List**)tab.curr)[bucket]);_tmp29 > t->max_len;}))
Cyc_Hashtable_resize(t);}
# 57
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key){
struct _fat_ptr tab=t->tab;
struct Cyc_List_List*l=({struct Cyc_List_List**_tmp2B=(struct Cyc_List_List**)tab.curr;_tmp2B[(int)({unsigned _tmp2A=(unsigned)(t->hash)(key);_tmp2A % _get_fat_size(tab,sizeof(struct Cyc_List_List*));})];});
return Cyc_List_assoc_cmp(t->cmp,l,key);}
# 63
void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key){
struct _fat_ptr _tmp4=t->tab;struct _fat_ptr tab=_tmp4;
struct Cyc_List_List*_tmp5=({struct Cyc_List_List**_tmp2D=(struct Cyc_List_List**)tab.curr;_tmp2D[(int)({unsigned _tmp2C=(unsigned)(t->hash)(key);_tmp2C % _get_fat_size(tab,sizeof(struct Cyc_List_List*));})];});struct Cyc_List_List*l=_tmp5;
int(*_tmp6)(void*,void*)=t->cmp;int(*cmp)(void*,void*)=_tmp6;
for(0;l != 0;l=l->tl){
struct _tuple0*_tmp7=(struct _tuple0*)l->hd;struct _tuple0*_stmttmp0=_tmp7;struct _tuple0*_tmp8=_stmttmp0;void**_tmpA;void*_tmp9;_LL1: _tmp9=_tmp8->f1;_tmpA=(void**)& _tmp8->f2;_LL2: {void*k=_tmp9;void**v=_tmpA;
if(cmp(key,k)== 0)return v;}}
# 71
return 0;}
# 74
int Cyc_Hashtable_try_lookup(struct Cyc_Hashtable_Table*t,void*key,void**data){
struct _fat_ptr _tmpB=t->tab;struct _fat_ptr tab=_tmpB;
struct Cyc_List_List*_tmpC=({struct Cyc_List_List**_tmp2F=(struct Cyc_List_List**)tab.curr;_tmp2F[(int)({unsigned _tmp2E=(unsigned)(t->hash)(key);_tmp2E % _get_fat_size(tab,sizeof(struct Cyc_List_List*));})];});struct Cyc_List_List*l=_tmpC;
int(*_tmpD)(void*,void*)=t->cmp;int(*cmp)(void*,void*)=_tmpD;
for(0;l != 0;l=l->tl){
struct _tuple0 _tmpE=*((struct _tuple0*)l->hd);struct _tuple0 _stmttmp1=_tmpE;struct _tuple0 _tmpF=_stmttmp1;void*_tmp11;void*_tmp10;_LL1: _tmp10=_tmpF.f1;_tmp11=_tmpF.f2;_LL2: {void*k=_tmp10;void*v=_tmp11;
if(cmp(key,k)== 0){
*data=v;
return 1;}}}
# 85
return 0;}
# 88
void Cyc_Hashtable_remove(struct Cyc_Hashtable_Table*t,void*key){
# 91
struct _fat_ptr _tmp12=t->tab;struct _fat_ptr tab=_tmp12;
int(*_tmp13)(void*,void*)=t->cmp;int(*cmp)(void*,void*)=_tmp13;
int bucket=(int)({unsigned _tmp30=(unsigned)(t->hash)(key);_tmp30 % _get_fat_size(tab,sizeof(struct Cyc_List_List*));});
struct Cyc_List_List*_tmp14=*((struct Cyc_List_List**)_check_fat_subscript(tab,sizeof(struct Cyc_List_List*),bucket));struct Cyc_List_List*l=_tmp14;
if(l == 0)return;
if(cmp(key,(((struct _tuple0*)l->hd)[0]).f1)== 0){
((struct Cyc_List_List**)tab.curr)[bucket]=l->tl;
return;}{
# 100
struct Cyc_List_List*_tmp15=l->tl;struct Cyc_List_List*next=_tmp15;for(0;((struct Cyc_List_List*)_check_null(l))->tl != 0;(l=l->tl,next=next->tl)){
# 102
if(cmp(key,(((struct _tuple0*)((struct Cyc_List_List*)_check_null(next))->hd)[0]).f1)== 0){
l->tl=next->tl;
return;}}}}
# 108
int Cyc_Hashtable_hash_string(struct _fat_ptr s){
# 110
int ans=0;
int sz=(int)_get_fat_size(s,sizeof(char));
int shift=0;
{int i=0;for(0;i < sz;++ i){
ans=ans ^ (int)*((const char*)_check_fat_subscript(s,sizeof(char),i))<< shift;
shift +=8;
if(shift == 32)
shift=0;}}
# 119
return ans;}
# 122
int Cyc_Hashtable_hash_stringptr(struct _fat_ptr*s){
return Cyc_Hashtable_hash_string(*s);}
# 127
void Cyc_Hashtable_insert_bucket(struct _RegionHandle*r,struct _fat_ptr tab,int(*hash)(void*),struct Cyc_List_List*elems){
# 130
if(elems == 0)return;
Cyc_Hashtable_insert_bucket(r,tab,hash,elems->tl);{
void*key=(((struct _tuple0*)elems->hd)[0]).f1;
void*val=(((struct _tuple0*)elems->hd)[0]).f2;
int nidx=(int)({unsigned _tmp31=(unsigned)hash(key);_tmp31 % _get_fat_size(tab,sizeof(struct Cyc_List_List*));});
({struct Cyc_List_List*_tmp33=({struct Cyc_List_List*_tmp17=_region_malloc(r,sizeof(*_tmp17));({struct _tuple0*_tmp32=({struct _tuple0*_tmp16=_region_malloc(r,sizeof(*_tmp16));_tmp16->f1=key,_tmp16->f2=val;_tmp16;});_tmp17->hd=_tmp32;}),_tmp17->tl=*((struct Cyc_List_List**)_check_fat_subscript(tab,sizeof(struct Cyc_List_List*),nidx));_tmp17;});((struct Cyc_List_List**)tab.curr)[nidx]=_tmp33;});}}
# 138
void Cyc_Hashtable_resize(struct Cyc_Hashtable_Table*t){
struct _fat_ptr odata=t->tab;
int osize=(int)_get_fat_size(odata,sizeof(struct Cyc_List_List*));
int nsize=2 * osize + 1;
struct _fat_ptr ndata=({unsigned _tmp18=nsize;_tag_fat(_region_calloc(t->r,sizeof(struct Cyc_List_List*),_tmp18),sizeof(struct Cyc_List_List*),_tmp18);});
{int i=0;for(0;i < osize;++ i){
Cyc_Hashtable_insert_bucket(t->r,ndata,t->hash,*((struct Cyc_List_List**)_check_fat_subscript(odata,sizeof(struct Cyc_List_List*),i)));}}
t->tab=ndata;
t->max_len=2 * t->max_len;}
# 150
void Cyc_Hashtable_iter(void(*f)(void*,void*),struct Cyc_Hashtable_Table*t){
struct _fat_ptr odata=t->tab;
int osize=(int)_get_fat_size(odata,sizeof(struct Cyc_List_List*));
int i=0;for(0;i < osize;++ i){
struct Cyc_List_List*iter=*((struct Cyc_List_List**)_check_fat_subscript(odata,sizeof(struct Cyc_List_List*),i));for(0;iter != 0;iter=iter->tl){
f((((struct _tuple0*)iter->hd)[0]).f1,(((struct _tuple0*)iter->hd)[0]).f2);}}}
# 158
void Cyc_Hashtable_iter_c(void(*f)(void*,void*,void*),struct Cyc_Hashtable_Table*t,void*env){
struct _fat_ptr odata=t->tab;
int osize=(int)_get_fat_size(odata,sizeof(struct Cyc_List_List*));
int i=0;for(0;i < osize;++ i){
struct Cyc_List_List*iter=*((struct Cyc_List_List**)_check_fat_subscript(odata,sizeof(struct Cyc_List_List*),i));for(0;iter != 0;iter=iter->tl){
f((((struct _tuple0*)iter->hd)[0]).f1,(((struct _tuple0*)iter->hd)[0]).f2,env);}}}
# 167
void Cyc_Hashtable_print_table_map(struct Cyc_Hashtable_Table*t,void(*prn_key)(void*),void(*prn_val)(void*)){
# 169
struct _fat_ptr odata=t->tab;
int osize=(int)_get_fat_size(odata,sizeof(struct Cyc_List_List*));
int i=0;for(0;i < osize;++ i){
({struct Cyc_Int_pa_PrintArg_struct _tmp1B=({struct Cyc_Int_pa_PrintArg_struct _tmp24;_tmp24.tag=1U,_tmp24.f1=(unsigned long)i;_tmp24;});void*_tmp19[1U];_tmp19[0]=& _tmp1B;({struct _fat_ptr _tmp34=({const char*_tmp1A="%d: ";_tag_fat(_tmp1A,sizeof(char),5U);});Cyc_printf(_tmp34,_tag_fat(_tmp19,sizeof(void*),1U));});});
{struct Cyc_List_List*iter=*((struct Cyc_List_List**)_check_fat_subscript(odata,sizeof(struct Cyc_List_List*),i));for(0;iter != 0;iter=iter->tl){
({void*_tmp1C=0U;({struct _fat_ptr _tmp35=({const char*_tmp1D="(";_tag_fat(_tmp1D,sizeof(char),2U);});Cyc_printf(_tmp35,_tag_fat(_tmp1C,sizeof(void*),0U));});});
prn_key((((struct _tuple0*)iter->hd)[0]).f1);
({void*_tmp1E=0U;({struct _fat_ptr _tmp36=({const char*_tmp1F=",";_tag_fat(_tmp1F,sizeof(char),2U);});Cyc_printf(_tmp36,_tag_fat(_tmp1E,sizeof(void*),0U));});});
prn_val((((struct _tuple0*)iter->hd)[0]).f2);
({void*_tmp20=0U;({struct _fat_ptr _tmp37=({const char*_tmp21=") ";_tag_fat(_tmp21,sizeof(char),3U);});Cyc_printf(_tmp37,_tag_fat(_tmp20,sizeof(void*),0U));});});}}
# 180
({void*_tmp22=0U;({struct _fat_ptr _tmp38=({const char*_tmp23="\n";_tag_fat(_tmp23,sizeof(char),2U);});Cyc_printf(_tmp38,_tag_fat(_tmp22,sizeof(void*),0U));});});}}
