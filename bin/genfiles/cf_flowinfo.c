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

void* Cyc_Core_get_exn_thrown();
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
#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#define _check_null(ptr) \
  ({ typeof(ptr) _cks_null = (ptr); \
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

static inline unsigned int _check_times(unsigned x, unsigned y) {
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

static inline void *_fast_region_malloc(struct _RegionHandle *r, unsigned orig_s) {  
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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];extern char Cyc_Core_Failure[8U];extern char Cyc_Core_Impossible[11U];extern char Cyc_Core_Not_found[10U];extern char Cyc_Core_Unreachable[12U];
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);
# 86
extern struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*)(void*,void*),void*,struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);extern char Cyc_List_Nth[4U];
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 383
extern int Cyc_List_list_cmp(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);
# 394
extern struct Cyc_List_List*Cyc_List_filter_c(int(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);extern char Cyc_Set_Absent[7U];struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_empty(int(*)(void*,void*));
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 110
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict,void*);
# 147 "dict.h"
extern void Cyc_Dict_iter(void(*)(void*,void*),struct Cyc_Dict_Dict);
# 212
extern struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*)(void*,void*,void*,void*),void*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 222
extern int Cyc_Dict_forall_intersect(int(*)(void*,void*,void*),struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 283
extern struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*,struct Cyc_Dict_Dict);
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 171
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 836 "absyn.h"
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 863
void*Cyc_Absyn_compress(void*);
# 894
extern void*Cyc_Absyn_void_type;
# 1120
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1122
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);
# 114 "relations-ap.h"
int Cyc_Relations_relns_approx(struct Cyc_List_List*,struct Cyc_List_List*);
# 116
struct Cyc_List_List*Cyc_Relations_join_relns(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 118
void Cyc_Relations_print_relns(struct Cyc___cycFILE*,struct Cyc_List_List*);
# 58 "tcutil.h"
int Cyc_Tcutil_is_bits_only_type(void*);
# 131
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 137
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 180
int Cyc_Tcutil_is_noalias_pointer(void*,int);
# 235
int Cyc_Tcutil_force_type2bool(int,void*);
# 30 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 73
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*);
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 69
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 33 "warn.h"
void Cyc_Warn_verr(unsigned,struct _fat_ptr,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned,struct _fat_ptr,struct _fat_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL =0U,Cyc_CfFlowInfo_AllIL =1U};char Cyc_CfFlowInfo_IsZero[7U]="IsZero";struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};char Cyc_CfFlowInfo_NotZero[8U]="NotZero";char Cyc_CfFlowInfo_UnknownZ[9U]="UnknownZ";struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _fat_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;int f4;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple11{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple11 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 143 "cf_flowinfo.h"
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 160
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*,struct _fat_ptr*);
# 162
int Cyc_CfFlowInfo_root_cmp(void*,void*);
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);
# 165
struct _fat_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int,void*);
# 167
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*,void*,struct Cyc_Absyn_Exp*,int,void*,int);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*,int,void*,int*);
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*,void*);
# 172
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict,void*);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*);
# 175
int Cyc_CfFlowInfo_is_init_pointer(void*);
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);
# 178
void Cyc_CfFlowInfo_print_absrval(void*);
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel);
void Cyc_CfFlowInfo_print_root(void*);
void Cyc_CfFlowInfo_print_path(struct Cyc_List_List*);
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*);
# 184
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict);
# 207 "cf_flowinfo.h"
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);
# 38 "cf_flowinfo.cyc"
int Cyc_CfFlowInfo_anal_error=0;
void Cyc_CfFlowInfo_aerr(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){
# 41
Cyc_CfFlowInfo_anal_error=1;
Cyc_Warn_verr(loc,fmt,ap);}
# 46
struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val={Cyc_CfFlowInfo_IsZero};
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*x){
return({union Cyc_CfFlowInfo_AbsLVal _Tmp0;(_Tmp0.PlaceL).tag=1U,(_Tmp0.PlaceL).val=x;_Tmp0;});}
# 50
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL (void){
return({union Cyc_CfFlowInfo_AbsLVal _Tmp0;(_Tmp0.UnknownL).tag=2U,(_Tmp0.UnknownL).val=0;_Tmp0;});}
# 53
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL (void){return({union Cyc_CfFlowInfo_FlowInfo _Tmp0;(_Tmp0.BottomFL).tag=1U,(_Tmp0.BottomFL).val=0;_Tmp0;});}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,struct Cyc_List_List*r){
return({union Cyc_CfFlowInfo_FlowInfo _Tmp0;(_Tmp0.ReachableFL).tag=2U,((_Tmp0.ReachableFL).val).f1=fd,((_Tmp0.ReachableFL).val).f2=r;_Tmp0;});}
# 58
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env (void){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct dummy_rawexp={0,{.Null_c={1,0}}};
static struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};
return({struct Cyc_CfFlowInfo_FlowEnv*_Tmp0=_cycalloc(sizeof(struct Cyc_CfFlowInfo_FlowEnv));
({void*_Tmp1=(void*)({struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct));_Tmp2->tag=0;_Tmp2;});_Tmp0->zero=_Tmp1;}),({
void*_Tmp1=(void*)({struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct));_Tmp2->tag=1;_Tmp2;});_Tmp0->notzeroall=_Tmp1;}),({
void*_Tmp1=(void*)({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct));_Tmp2->tag=2,_Tmp2->f1=0U;_Tmp2;});_Tmp0->unknown_none=_Tmp1;}),({
void*_Tmp1=(void*)({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct));_Tmp2->tag=2,_Tmp2->f1=1U;_Tmp2;});_Tmp0->unknown_all=_Tmp1;}),({
void*_Tmp1=(void*)({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct));_Tmp2->tag=3,_Tmp2->f1=0U;_Tmp2;});_Tmp0->esc_none=_Tmp1;}),({
void*_Tmp1=(void*)({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct));_Tmp2->tag=3,_Tmp2->f1=1U;_Tmp2;});_Tmp0->esc_all=_Tmp1;}),({
struct Cyc_Dict_Dict _Tmp1=({(struct Cyc_Dict_Dict(*)(int(*)(void*,void*)))Cyc_Dict_empty;})(Cyc_CfFlowInfo_root_cmp);_Tmp0->mt_flowdict=_Tmp1;}),({
struct Cyc_CfFlowInfo_Place*_Tmp1=({struct Cyc_CfFlowInfo_Place*_Tmp2=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));({void*_Tmp3=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct));_Tmp4->tag=1,_Tmp4->f1=& dummy_exp,_Tmp4->f2=Cyc_Absyn_void_type;_Tmp4;});_Tmp2->root=_Tmp3;}),_Tmp2->path=0;_Tmp2;});_Tmp0->dummy_place=_Tmp1;});_Tmp0;});}
# 74
struct _fat_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _stmttmp0=*place;struct Cyc_CfFlowInfo_Place _Tmp0=_stmttmp0;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.root;_Tmp1=_Tmp0.path;{void*root=_Tmp2;struct Cyc_List_List*fields=_Tmp1;
void*_Tmp3=root;void*_Tmp4;if(*((int*)_Tmp3)== 0){_Tmp4=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_Tmp3)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 78
if(fields == 0)
return Cyc_Absynpp_qvar2string(vd->name);else{
# 81
return({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=Cyc_Absynpp_qvar2string(vd->name);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(({const char*_Tmp7="reachable from %s";_tag_fat(_Tmp7,sizeof(char),18U);}),_tag_fat(_Tmp6,sizeof(void*),1));});}}}else{
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp5="error locations not for VarRoots";_tag_fat(_Tmp5,sizeof(char),33U);}),_tag_fat(0U,sizeof(void*),0));};}}
# 87
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _fat_ptr*f){
int n=0;
for(1;(unsigned)fs;fs=fs->tl){
struct _fat_ptr*f2=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;
if(Cyc_strptrcmp(f2,f)== 0)return n;
n=n + 1;}
# 94
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=*f;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp2="get_field_index_fs failed: %s";_tag_fat(_Tmp2,sizeof(char),30U);}),_tag_fat(_Tmp1,sizeof(void*),1));});}
# 97
int Cyc_CfFlowInfo_get_field_index(void*t,struct _fat_ptr*f){
void*_stmttmp1=Cyc_Absyn_compress(t);void*_Tmp0=_stmttmp1;void*_Tmp1;union Cyc_Absyn_AggrInfo _Tmp2;switch(*((int*)_Tmp0)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)== 20){_Tmp2=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_Tmp2;
# 100
struct Cyc_List_List*fs=(_check_null((Cyc_Absyn_get_known_aggrdecl(info))->impl))->fields;
_Tmp1=fs;goto _LL4;}}else{goto _LL5;}case 7: _Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f2;_LL4: {struct Cyc_List_List*fs=_Tmp1;
# 103
return Cyc_CfFlowInfo_get_field_index_fs(fs,f);}default: _LL5:
# 105
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=Cyc_Absynpp_typ2string(t);_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp5="get_field_index failed: %s";_tag_fat(_Tmp5,sizeof(char),27U);}),_tag_fat(_Tmp4,sizeof(void*),1));});};}struct _tuple12{void*f1;void*f2;};
# 109
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){
if((int)r1 == (int)r2)
return 0;{
struct _tuple12 _stmttmp2=({struct _tuple12 _Tmp0;_Tmp0.f1=r1,_Tmp0.f2=r2;_Tmp0;});struct _tuple12 _Tmp0=_stmttmp2;int _Tmp1;int _Tmp2;void*_Tmp3;void*_Tmp4;if(*((int*)_Tmp0.f1)== 0){if(*((int*)_Tmp0.f2)== 0){_Tmp4=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_Tmp0.f1)->f1;_Tmp3=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_Tmp0.f2)->f1;{struct Cyc_Absyn_Vardecl*vd1=_Tmp4;struct Cyc_Absyn_Vardecl*vd2=_Tmp3;
return(int)vd1 - (int)vd2;}}else{
return -1;}}else{if(*((int*)_Tmp0.f2)== 0)
return 1;else{if(*((int*)_Tmp0.f1)== 1){if(*((int*)_Tmp0.f2)== 1){_Tmp4=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_Tmp0.f1)->f1;_Tmp3=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_Tmp0.f2)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
return(int)e1 - (int)e2;}}else{
return -1;}}else{if(*((int*)_Tmp0.f2)== 1)
return 1;else{_Tmp2=((struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_Tmp0.f2)->f1;{int i1=_Tmp2;int i2=_Tmp1;
return i1 - i2;}}}}};}}
# 123
static int Cyc_CfFlowInfo_pathcon_cmp(void*p1,void*p2){
struct _tuple12 _stmttmp3=({struct _tuple12 _Tmp0;_Tmp0.f1=p1,_Tmp0.f2=p2;_Tmp0;});struct _tuple12 _Tmp0=_stmttmp3;int _Tmp1;int _Tmp2;if(*((int*)_Tmp0.f1)== 0){if(*((int*)_Tmp0.f2)== 0){_Tmp2=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_Tmp0.f2)->f1;{int i1=_Tmp2;int i2=_Tmp1;
# 126
if(i1 == i2)return 0;
if(i1 < i2)return -1;else{
return 1;}}}else{
# 131
return 1;}}else{if(*((int*)_Tmp0.f2)== 1)
# 129
return 0;else{
return -1;}};}
# 135
static int Cyc_CfFlowInfo_path_cmp(struct Cyc_List_List*path1,struct Cyc_List_List*path2){
return({(int(*)(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp;})(Cyc_CfFlowInfo_pathcon_cmp,path1,path2);}
# 139
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2){
if((int)p1 == (int)p2)
return 0;{
struct Cyc_CfFlowInfo_Place _stmttmp4=*p1;struct Cyc_CfFlowInfo_Place _Tmp0=_stmttmp4;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.root;_Tmp1=_Tmp0.path;{void*root1=_Tmp2;struct Cyc_List_List*path1=_Tmp1;
struct Cyc_CfFlowInfo_Place _stmttmp5=*p2;struct Cyc_CfFlowInfo_Place _Tmp3=_stmttmp5;void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp3.root;_Tmp4=_Tmp3.path;{void*root2=_Tmp5;struct Cyc_List_List*path2=_Tmp4;
int i=Cyc_CfFlowInfo_root_cmp(root1,root2);
if(i != 0)
return i;
return Cyc_CfFlowInfo_path_cmp(path1,path2);}}}}
# 160 "cf_flowinfo.cyc"
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,int,int,void*,void*);
# 165
struct _fat_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 171
unsigned sz=(unsigned)({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(fs);
struct _fat_ptr d=({unsigned _Tmp0=sz;_tag_fat(({void**_Tmp1=_cycalloc(_check_times(_Tmp0,sizeof(void*)));({{unsigned _Tmp2=sz;unsigned i;for(i=0;i < _Tmp2;++ i){_Tmp1[i]=fenv->unknown_all;}}0;});_Tmp1;}),sizeof(void*),_Tmp0);});
{int i=0;for(0;(unsigned)i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield*_stmttmp6=(struct Cyc_Absyn_Aggrfield*)(_check_null(fs))->hd;struct Cyc_Absyn_Aggrfield*_Tmp0=_stmttmp6;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->name;_Tmp1=_Tmp0->type;{struct _fat_ptr*n=_Tmp2;void*t2=_Tmp1;
if(_get_fat_size(*n,sizeof(char))!= 1U)
({void*_Tmp3=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,t2,leafval);*((void**)_check_fat_subscript(d,sizeof(void*),i))=_Tmp3;});}}}
# 179
return d;}struct _tuple13{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
# 183
static struct Cyc_Absyn_Aggrfield*Cyc_CfFlowInfo_substitute_field(struct _tuple13*env,struct Cyc_Absyn_Aggrfield*f){
# 187
struct _tuple13*_Tmp0=env;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f1;_Tmp1=_Tmp0->f2;{struct _RegionHandle*t=_Tmp2;struct Cyc_List_List*inst=_Tmp1;
void*new_typ=Cyc_Tcutil_rsubstitute(t,inst,f->type);
struct Cyc_Absyn_Exp*r=f->requires_clause;
struct Cyc_Absyn_Exp*new_r=r == 0?0: Cyc_Tcutil_rsubsexp(t,inst,r);
return({struct Cyc_Absyn_Aggrfield*_Tmp3=_region_malloc(t,sizeof(struct Cyc_Absyn_Aggrfield));_Tmp3->name=f->name,_Tmp3->tq=f->tq,_Tmp3->type=new_typ,_Tmp3->width=f->width,_Tmp3->attributes=f->attributes,_Tmp3->requires_clause=new_r;_Tmp3;});}}struct _tuple14{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 198
static struct _fat_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*targs,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 204
struct _RegionHandle _Tmp0=_new_region("temp");struct _RegionHandle*temp=& _Tmp0;_push_region(temp);
# 208
{struct Cyc_List_List*inst=0;
for(1;tvs != 0;(tvs=tvs->tl,targs=targs->tl)){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*t=(void*)(_check_null(targs))->hd;
{struct Cyc_Absyn_Kind*_stmttmp7=Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk);struct Cyc_Absyn_Kind*_Tmp1=_stmttmp7;switch((int)((struct Cyc_Absyn_Kind*)_Tmp1)->kind){case Cyc_Absyn_RgnKind:
 goto _LL4;case Cyc_Absyn_EffKind: _LL4:
 continue;default:
 goto _LL0;}_LL0:;}
# 217
inst=({struct Cyc_List_List*_Tmp1=_region_malloc(temp,sizeof(struct Cyc_List_List));({struct _tuple14*_Tmp2=({struct _tuple14*_Tmp3=_region_malloc(temp,sizeof(struct _tuple14));_Tmp3->f1=tv,_Tmp3->f2=t;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=inst;_Tmp1;});}
# 219
if(inst != 0){
struct _tuple13 env=({struct _tuple13 _Tmp1;_Tmp1.f1=temp,({struct Cyc_List_List*_Tmp2=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(inst);_Tmp1.f2=_Tmp2;});_Tmp1;});
struct Cyc_List_List*subs_fs=({(struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*)(struct _tuple13*,struct Cyc_Absyn_Aggrfield*),struct _tuple13*,struct Cyc_List_List*))Cyc_List_rmap_c;})(temp,Cyc_CfFlowInfo_substitute_field,& env,fs);
struct _fat_ptr _Tmp1=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,subs_fs,no_init_bits_only,leafval);_npop_handler(0);return _Tmp1;}else{
# 224
struct _fat_ptr _Tmp1=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,no_init_bits_only,leafval);_npop_handler(0);return _Tmp1;}}
# 208
;_pop_region();}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 228
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval){
# 234
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(t))return fenv->unknown_all;
# 236
{void*_stmttmp8=Cyc_Absyn_compress(t);void*_Tmp0=_stmttmp8;void*_Tmp1;enum Cyc_Absyn_AggrKind _Tmp2;union Cyc_Absyn_AggrInfo _Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 19: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1).KnownDatatypefield).tag == 2){_Tmp4=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1).KnownDatatypefield).val).f2;{struct Cyc_Absyn_Datatypefield*tuf=_Tmp4;
# 238
if(tuf->typs == 0)
return leafval;
_Tmp4=tuf->typs;goto _LL4;}}else{goto _LLF;}case 20: _Tmp3=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;_Tmp4=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{union Cyc_Absyn_AggrInfo info=_Tmp3;struct Cyc_List_List*targs=_Tmp4;
# 250
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);
if(ad->impl == 0)
goto _LL0;{
struct Cyc_List_List*fields=(_check_null(ad->impl))->fields;
if(targs == 0){_Tmp2=ad->kind;_Tmp4=fields;goto _LL8;}
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_Tmp5->tag=6,(_Tmp5->f1).is_union=(int)ad->kind == 1,(_Tmp5->f1).fieldnum=-1,({
struct _fat_ptr _Tmp6=Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(fenv,ad->tvs,targs,fields,(int)ad->kind == 1,leafval);_Tmp5->f2=_Tmp6;});_Tmp5;});}}case 4: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2 != 0){_Tmp4=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2)->hd;{void*t=_Tmp4;
# 273
return leafval;}}else{goto _LLF;}default: goto _LLF;}case 6: _Tmp4=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_Tmp0)->f1;_LL4: {struct Cyc_List_List*tqts=_Tmp4;
# 242
unsigned sz=(unsigned)({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(tqts);
struct _fat_ptr d=({unsigned _Tmp5=sz;_tag_fat(({void**_Tmp6=_cycalloc(_check_times(_Tmp5,sizeof(void*)));({{unsigned _Tmp7=sz;unsigned i;for(i=0;i < _Tmp7;++ i){_Tmp6[i]=fenv->unknown_all;}}0;});_Tmp6;}),sizeof(void*),_Tmp5);});
{int i=0;for(0;(unsigned)i < sz;++ i){
({void*_Tmp5=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,(*((struct _tuple15*)(_check_null(tqts))->hd)).f2,leafval);*((void**)_check_fat_subscript(d,sizeof(void*),i))=_Tmp5;});
tqts=tqts->tl;}}
# 248
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_Tmp5->tag=6,(_Tmp5->f1).is_union=0,(_Tmp5->f1).fieldnum=-1,_Tmp5->f2=d;_Tmp5;});}case 7: _Tmp2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f2;_LL8: {enum Cyc_Absyn_AggrKind k=_Tmp2;struct Cyc_List_List*fs=_Tmp4;
# 265
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_Tmp5->tag=6,(_Tmp5->f1).is_union=(int)k == 1,(_Tmp5->f1).fieldnum=-1,({
struct _fat_ptr _Tmp6=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,(int)k == 1,leafval);_Tmp5->f2=_Tmp6;});_Tmp5;});}case 4: _Tmp4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).elt_type;_Tmp1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).zero_term;{void*et=_Tmp4;void*zeroterm=_Tmp1;
# 268
if(Cyc_Tcutil_force_type2bool(0,zeroterm))
# 271
return(allow_zeroterm && !no_init_bits_only)&& Cyc_Tcutil_is_bits_only_type(et)?fenv->unknown_all: leafval;
goto _LL0;}case 3: _Tmp4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).ptr_atts).nullable;{void*nbl=_Tmp4;
# 275
if(!Cyc_Tcutil_force_type2bool(0,nbl)){
void*_Tmp5=leafval;if(*((int*)_Tmp5)== 2){if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_Tmp5)->f1 == Cyc_CfFlowInfo_AllIL)
return fenv->notzeroall;else{goto _LL14;}}else{_LL14:
 goto _LL11;}_LL11:;}
# 280
goto _LL0;}default: _LLF:
 goto _LL0;}_LL0:;}
# 284
return !no_init_bits_only && Cyc_Tcutil_is_bits_only_type(t)?fenv->unknown_all: leafval;}
# 287
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,int no_init_bits_only,void*leafval){
return Cyc_CfFlowInfo_i_typ_to_absrval(fenv,1,no_init_bits_only,t,leafval);}
# 293
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume){
void*_Tmp0=r;struct _fat_ptr _Tmp1;int _Tmp2;void*_Tmp3;int _Tmp4;void*_Tmp5;switch(*((int*)_Tmp0)){case 7: _Tmp5=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f2;_Tmp3=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f3;_Tmp2=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*consumer=_Tmp5;int iteration=_Tmp4;void*r=_Tmp3;int local=_Tmp2;
# 296
if(consumer == e && iteration == env_iteration){
*needs_unconsume=1;
return 0;}
# 300
return 1;}case 6: _Tmp4=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1).is_union;_Tmp2=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1).fieldnum;_Tmp1=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f2;{int is_union=_Tmp4;int field_no=_Tmp2;struct _fat_ptr d=_Tmp1;
# 302
if(!is_union || field_no == -1){
unsigned sz=_get_fat_size(d,sizeof(void*));
{int i=0;for(0;(unsigned)i < sz;++ i){
if(Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,((void**)d.curr)[i],needs_unconsume))
return 1;}}
# 308
return 0;}else{
# 311
return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,((void**)d.curr)[field_no],needs_unconsume);}}case 8: _Tmp5=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f2;{void*r=_Tmp5;
# 313
return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,r,needs_unconsume);}default:
 return 0;};}
# 320
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r){
void*_Tmp0=r;struct _fat_ptr _Tmp1;struct Cyc_CfFlowInfo_UnionRInfo _Tmp2;int _Tmp3;void*_Tmp4;int _Tmp5;void*_Tmp6;switch(*((int*)_Tmp0)){case 7: _Tmp6=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f2;_Tmp4=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f3;_Tmp3=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*consumer=_Tmp6;int iteration=_Tmp5;void*r=_Tmp4;int local=_Tmp3;
# 323
return r;}case 6: _Tmp2=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f2;{struct Cyc_CfFlowInfo_UnionRInfo uinfo=_Tmp2;struct _fat_ptr d=_Tmp1;
# 325
unsigned sz=_get_fat_size(d,sizeof(void*));
int change=0;
struct _fat_ptr d2=({unsigned _Tmp7=sz;_tag_fat(({void**_Tmp8=_cycalloc(_check_times(_Tmp7,sizeof(void*)));({{unsigned _Tmp9=sz;unsigned i;for(i=0;i < _Tmp9;++ i){_Tmp8[i]=((void**)d.curr)[(int)i];}}0;});_Tmp8;}),sizeof(void*),_Tmp7);});
{int i=0;for(0;(unsigned)i < sz;++ i){
({void*_Tmp7=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,((void**)d.curr)[i]);*((void**)_check_fat_subscript(d2,sizeof(void*),i))=_Tmp7;});
if(((void**)d2.curr)[i]!= ((void**)d.curr)[i])
change=1;}}
# 333
if(change)
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_Tmp7->tag=6,_Tmp7->f1=uinfo,_Tmp7->f2=d2;_Tmp7;});else{
return r;}}case 8: _Tmp6=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1;_Tmp4=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*n=_Tmp6;void*r2=_Tmp4;
# 337
void*r3=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,r2);
if(r3 != r2)
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_Tmp7->tag=8,_Tmp7->f1=n,_Tmp7->f2=r3;_Tmp7;});else{
return r;}}default:
 return r;};}
# 347
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r,int local_alias){
struct _tuple12 _stmttmp9=({struct _tuple12 _Tmp0;({void*_Tmp1=Cyc_Absyn_compress(t);_Tmp0.f1=_Tmp1;}),_Tmp0.f2=r;_Tmp0;});struct _tuple12 _Tmp0=_stmttmp9;enum Cyc_Absyn_AggrKind _Tmp1;union Cyc_Absyn_AggrInfo _Tmp2;struct _fat_ptr _Tmp3;struct Cyc_CfFlowInfo_UnionRInfo _Tmp4;void*_Tmp5;void*_Tmp6;if(*((int*)_Tmp0.f2)== 8){_Tmp6=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1;_Tmp5=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f2;{struct Cyc_Absyn_Vardecl*s=_Tmp6;void*r2=_Tmp5;
# 350
void*r3=Cyc_CfFlowInfo_make_unique_consumed(fenv,t,consumer,iteration,r2,local_alias);
if(r3 != r2)return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_Tmp7->tag=8,_Tmp7->f1=s,_Tmp7->f2=r3;_Tmp7;});else{
return r;}}}else{switch(*((int*)_Tmp0.f1)){case 6: if(*((int*)_Tmp0.f2)== 6){_Tmp6=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_Tmp0.f1)->f1;_Tmp4=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1;_Tmp3=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f2;{struct Cyc_List_List*tqts=_Tmp6;struct Cyc_CfFlowInfo_UnionRInfo b=_Tmp4;struct _fat_ptr d=_Tmp3;
# 354
unsigned sz=_get_fat_size(d,sizeof(void*));
struct _fat_ptr d2=({unsigned _Tmp7=sz;_tag_fat(({void**_Tmp8=_cycalloc(_check_times(_Tmp7,sizeof(void*)));({{unsigned _Tmp9=sz;unsigned i;for(i=0;i < _Tmp9;++ i){_Tmp8[i]=fenv->unknown_all;}}0;});_Tmp8;}),sizeof(void*),_Tmp7);});
{int i=0;for(0;(unsigned)i < sz;++ i){
({void*_Tmp7=Cyc_CfFlowInfo_make_unique_consumed(fenv,(*((struct _tuple15*)(_check_null(tqts))->hd)).f2,consumer,iteration,((void**)d.curr)[i],local_alias);*((void**)_check_fat_subscript(d2,sizeof(void*),i))=_Tmp7;});
tqts=tqts->tl;}}
# 360
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_Tmp7->tag=6,_Tmp7->f1=b,_Tmp7->f2=d2;_Tmp7;});}}else{goto _LL9;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)== 20){if(*((int*)_Tmp0.f2)== 6){_Tmp2=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1;_Tmp4=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1;_Tmp3=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f2;{union Cyc_Absyn_AggrInfo info=_Tmp2;struct Cyc_CfFlowInfo_UnionRInfo b=_Tmp4;struct _fat_ptr d=_Tmp3;
# 362
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);
if(ad->impl == 0)return r;
_Tmp1=ad->kind;_Tmp6=(_check_null(ad->impl))->fields;_Tmp4=b;_Tmp3=d;goto _LL8;}}else{goto _LL9;}}else{goto _LL9;}case 7: if(*((int*)_Tmp0.f2)== 6){_Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0.f1)->f1;_Tmp6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0.f1)->f2;_Tmp4=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1;_Tmp3=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f2;_LL8: {enum Cyc_Absyn_AggrKind k=_Tmp1;struct Cyc_List_List*fs=_Tmp6;struct Cyc_CfFlowInfo_UnionRInfo b=_Tmp4;struct _fat_ptr d=_Tmp3;
# 366
struct _fat_ptr d2=({unsigned _Tmp7=_get_fat_size(d,sizeof(void*));_tag_fat(({void**_Tmp8=_cycalloc(_check_times(_Tmp7,sizeof(void*)));({{unsigned _Tmp9=_get_fat_size(d,sizeof(void*));unsigned i;for(i=0;i < _Tmp9;++ i){_Tmp8[i]=((void**)d.curr)[(int)i];}}0;});_Tmp8;}),sizeof(void*),_Tmp7);});
unsigned sz=(unsigned)({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(fs);
{int i=0;for(0;(unsigned)i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield*_stmttmpA=(struct Cyc_Absyn_Aggrfield*)(_check_null(fs))->hd;struct Cyc_Absyn_Aggrfield*_Tmp7=_stmttmpA;void*_Tmp8;void*_Tmp9;_Tmp9=_Tmp7->name;_Tmp8=_Tmp7->type;{struct _fat_ptr*n=_Tmp9;void*t2=_Tmp8;
if(_get_fat_size(*n,sizeof(char))!= 1U)
({void*_TmpA=Cyc_CfFlowInfo_make_unique_consumed(fenv,t2,consumer,iteration,*((void**)_check_fat_subscript(d,sizeof(void*),i)),local_alias);*((void**)_check_fat_subscript(d2,sizeof(void*),i))=_TmpA;});}}}
# 373
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_Tmp7->tag=6,_Tmp7->f1=b,_Tmp7->f2=d2;_Tmp7;});}}else{goto _LL9;}default: _LL9:
# 375
 if(Cyc_Tcutil_is_noalias_pointer(t,0))
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct));_Tmp7->tag=7,_Tmp7->f1=consumer,_Tmp7->f2=iteration,_Tmp7->f3=r,_Tmp7->f4=local_alias;_Tmp7;});
return r;}};}struct _tuple16{struct Cyc_CfFlowInfo_Place*f1;unsigned f2;};
# 381
static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set){
# 383
struct _RegionHandle _Tmp0=_new_region("r");struct _RegionHandle*r=& _Tmp0;_push_region(r);
{struct _tuple16 elem=({struct _tuple16 _Tmp1;_Tmp1.f1=place,_Tmp1.f2=0U;_Tmp1;});
struct Cyc_Iter_Iter iter=({(struct Cyc_Iter_Iter(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_make_iter;})(r,set);
while(({(int(*)(struct Cyc_Iter_Iter,struct _tuple16*))Cyc_Iter_next;})(iter,& elem)){
struct Cyc_CfFlowInfo_Place*place2=elem.f1;
struct Cyc_CfFlowInfo_Place _stmttmpB=*place;struct Cyc_CfFlowInfo_Place _Tmp1=_stmttmpB;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.root;_Tmp2=_Tmp1.path;{void*root1=_Tmp3;struct Cyc_List_List*fs1=_Tmp2;
struct Cyc_CfFlowInfo_Place _stmttmpC=*place2;struct Cyc_CfFlowInfo_Place _Tmp4=_stmttmpC;void*_Tmp5;void*_Tmp6;_Tmp6=_Tmp4.root;_Tmp5=_Tmp4.path;{void*root2=_Tmp6;struct Cyc_List_List*fs2=_Tmp5;
if(Cyc_CfFlowInfo_root_cmp(root1,root2)!= 0)
continue;
for(1;fs1 != 0 && fs2 != 0;(fs1=fs1->tl,fs2=fs2->tl)){
if((void*)fs1->hd != (void*)fs2->hd)break;}
if(fs1 == 0){
int _Tmp7=1;_npop_handler(0);return _Tmp7;}}}}{
# 397
int _Tmp1=0;_npop_handler(0);return _Tmp1;}}
# 384
;_pop_region();}struct Cyc_CfFlowInfo_EscPile{struct Cyc_List_List*places;};
# 407
static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*place){
# 410
if(!({(int(*)(int(*)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*,struct Cyc_CfFlowInfo_Place*))Cyc_List_mem;})(Cyc_CfFlowInfo_place_cmp,pile->places,place))
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=place,_Tmp1->tl=pile->places;_Tmp1;});pile->places=_Tmp0;});}
# 413
static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*pile,void*r){
void*_Tmp0=r;struct _fat_ptr _Tmp1;struct Cyc_CfFlowInfo_UnionRInfo _Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 7: _Tmp3=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f3;{void*r=_Tmp3;
Cyc_CfFlowInfo_add_places(pile,r);return;}case 8: _Tmp3=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f2;{void*r=_Tmp3;
Cyc_CfFlowInfo_add_places(pile,r);return;}case 4: _Tmp3=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1;{struct Cyc_CfFlowInfo_Place*p=_Tmp3;
Cyc_CfFlowInfo_add_place(pile,p);return;}case 6: _Tmp2=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f2;{struct Cyc_CfFlowInfo_UnionRInfo b=_Tmp2;struct _fat_ptr d=_Tmp1;
# 419
{int i=0;for(0;(unsigned)i < _get_fat_size(d,sizeof(void*));++ i){
Cyc_CfFlowInfo_add_places(pile,((void**)d.curr)[i]);}}
return;}default:
 return;};}
# 429
static void*Cyc_CfFlowInfo_insert_place_inner(void*new_val,void*old_val){
void*_Tmp0=old_val;int _Tmp1;void*_Tmp2;void*_Tmp3;struct _fat_ptr _Tmp4;int _Tmp5;switch(*((int*)_Tmp0)){case 6: _Tmp5=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1).is_union;_Tmp4=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f2;{int is_union=_Tmp5;struct _fat_ptr d=_Tmp4;
# 432
struct _fat_ptr d2=({unsigned _Tmp6=_get_fat_size(d,sizeof(void*));_tag_fat(({void**_Tmp7=_cycalloc(_check_times(_Tmp6,sizeof(void*)));({{unsigned _Tmp8=_get_fat_size(d,sizeof(void*));unsigned i;for(i=0;i < _Tmp8;++ i){({void*_Tmp9=Cyc_CfFlowInfo_insert_place_inner(new_val,((void**)d.curr)[(int)i]);_Tmp7[i]=_Tmp9;});}}0;});_Tmp7;}),sizeof(void*),_Tmp6);});
# 436
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_Tmp6->tag=6,(_Tmp6->f1).is_union=is_union,(_Tmp6->f1).fieldnum=-1,_Tmp6->f2=d2;_Tmp6;});}case 7: _Tmp3=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f2;_Tmp2=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f3;_Tmp1=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e=_Tmp3;int i=_Tmp5;void*rval=_Tmp2;int l=_Tmp1;
# 438
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct));_Tmp6->tag=7,_Tmp6->f1=e,_Tmp6->f2=i,({void*_Tmp7=Cyc_CfFlowInfo_insert_place_inner(new_val,rval);_Tmp6->f3=_Tmp7;}),_Tmp6->f4=l;_Tmp6;});}case 8: _Tmp3=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1;_Tmp2=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*n=_Tmp3;void*rval=_Tmp2;
# 440
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_Tmp6->tag=8,_Tmp6->f1=n,({void*_Tmp7=Cyc_CfFlowInfo_insert_place_inner(new_val,rval);_Tmp6->f2=_Tmp7;});_Tmp6;});}default:
 return new_val;};}
# 447
static struct _fat_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _fat_ptr d,int n,void*rval){
void*old_rval=((void**)d.curr)[n];
if(old_rval == rval)return d;{
struct _fat_ptr res=({unsigned _Tmp0=_get_fat_size(d,sizeof(void*));_tag_fat(({void**_Tmp1=_cycalloc(_check_times(_Tmp0,sizeof(void*)));({{unsigned _Tmp2=_get_fat_size(d,sizeof(void*));unsigned i;for(i=0;i < _Tmp2;++ i){_Tmp1[i]=((void**)d.curr)[(int)i];}}0;});_Tmp1;}),sizeof(void*),_Tmp0);});
((void**)res.curr)[n]=rval;
return res;}}struct _tuple17{struct Cyc_List_List*f1;void*f2;};
# 460
static void*Cyc_CfFlowInfo_insert_place_outer(struct Cyc_List_List*path,void*old_val,void*new_val){
# 462
if(path == 0)
return Cyc_CfFlowInfo_insert_place_inner(new_val,old_val);{
struct _tuple17 _stmttmpD=({struct _tuple17 _Tmp0;_Tmp0.f1=path,_Tmp0.f2=old_val;_Tmp0;});struct _tuple17 _Tmp0=_stmttmpD;void*_Tmp1;struct _fat_ptr _Tmp2;int _Tmp3;void*_Tmp4;int _Tmp5;if(_Tmp0.f1 != 0){if(*((int*)((struct Cyc_List_List*)_Tmp0.f1)->hd)== 0)switch(*((int*)_Tmp0.f2)){case 6: _Tmp5=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)(_Tmp0.f1)->hd)->f1;_Tmp4=(_Tmp0.f1)->tl;_Tmp3=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1).is_union;_Tmp2=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f2;{int i=_Tmp5;struct Cyc_List_List*tl=_Tmp4;int is_union=_Tmp3;struct _fat_ptr d=_Tmp2;
# 466
void*new_child=Cyc_CfFlowInfo_insert_place_outer(tl,((void**)d.curr)[i],new_val);
struct _fat_ptr new_d=Cyc_CfFlowInfo_aggr_dict_insert(d,i,new_child);
if((void**)new_d.curr == (void**)d.curr)return old_val;else{
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_Tmp6->tag=6,(_Tmp6->f1).is_union=is_union,(_Tmp6->f1).fieldnum=-1,_Tmp6->f2=new_d;_Tmp6;});}}case 8: goto _LL5;default: goto _LL7;}else{switch(*((int*)_Tmp0.f2)){case 5: _Tmp4=(_Tmp0.f1)->tl;_Tmp1=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1;{struct Cyc_List_List*tl=_Tmp4;void*rval=_Tmp1;
# 471
void*new_rval=Cyc_CfFlowInfo_insert_place_outer(tl,rval,new_val);
if(new_rval == rval)return old_val;else{
return(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct));_Tmp6->tag=5,_Tmp6->f1=new_rval;_Tmp6;});}}case 8: goto _LL5;default: goto _LL7;}}}else{if(*((int*)_Tmp0.f2)== 8){_LL5: _Tmp4=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1;_Tmp1=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f2;{struct Cyc_Absyn_Vardecl*n=_Tmp4;void*rval=_Tmp1;
# 475
void*new_rval=Cyc_CfFlowInfo_insert_place_outer(path,rval,new_val);
if(new_rval == rval)return old_val;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_Tmp6->tag=8,_Tmp6->f1=n,_Tmp6->f2=new_rval;_Tmp6;});}}}else{_LL7:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp6="bad insert place";_tag_fat(_Tmp6,sizeof(char),17U);}),_tag_fat(0U,sizeof(void*),0));}};}}
# 485
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict d){
while(pile->places != 0){
struct Cyc_CfFlowInfo_Place*place=(struct Cyc_CfFlowInfo_Place*)(_check_null(pile->places))->hd;
pile->places=(_check_null(pile->places))->tl;{
void*oldval;void*newval;
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){oldval=Cyc_CfFlowInfo_lookup_place(d,place);;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3=_Tmp2;void*_Tmp4;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp3)->tag == Cyc_Dict_Absent)
continue;else{_Tmp4=_Tmp3;{void*exn=_Tmp4;(int)_rethrow(exn);}};}}}
{enum Cyc_CfFlowInfo_InitLevel _stmttmpE=Cyc_CfFlowInfo_initlevel(fenv,d,oldval);enum Cyc_CfFlowInfo_InitLevel _Tmp0=_stmttmpE;if(_Tmp0 == Cyc_CfFlowInfo_AllIL){
newval=fenv->esc_all;goto _LL5;}else{
newval=fenv->esc_none;goto _LL5;}_LL5:;}
# 496
Cyc_CfFlowInfo_add_places(pile,oldval);{
struct Cyc_CfFlowInfo_Place _stmttmpF=*place;struct Cyc_CfFlowInfo_Place _Tmp0=_stmttmpF;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.root;_Tmp1=_Tmp0.path;{void*root=_Tmp2;struct Cyc_List_List*path=_Tmp1;
d=({struct Cyc_Dict_Dict(*_Tmp3)(struct Cyc_Dict_Dict,void*,void*)=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,void*))Cyc_Dict_insert;});struct Cyc_Dict_Dict _Tmp4=d;void*_Tmp5=root;_Tmp3(_Tmp4,_Tmp5,({
struct Cyc_List_List*_Tmp6=path;void*_Tmp7=
({(void*(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup;})(d,root);
# 499
Cyc_CfFlowInfo_insert_place_outer(_Tmp6,_Tmp7,newval);}));});}}}}
# 503
return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};
# 513
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(void*r){
void*_Tmp0=r;void*_Tmp1;enum Cyc_CfFlowInfo_InitLevel _Tmp2;switch(*((int*)_Tmp0)){case 2: _Tmp2=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_Tmp2;
return il;}case 3: _Tmp2=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_Tmp2;
return il;}case 0:
 goto _LL8;case 1: _LL8:
 return 1U;case 7: _Tmp1=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f3;{void*r=_Tmp1;
return 0U;}default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp3="initlevel_approx";_tag_fat(_Tmp3,sizeof(char),17U);}),_tag_fat(0U,sizeof(void*),0));};}
# 523
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,enum Cyc_CfFlowInfo_InitLevel acc){
# 525
enum Cyc_CfFlowInfo_InitLevel this_ans;
if((int)acc == 0)return 0U;
{void*_Tmp0=r;struct _fat_ptr _Tmp1;int _Tmp2;int _Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 8: _Tmp4=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f2;{void*r=_Tmp4;
return Cyc_CfFlowInfo_initlevel_rec(env,r,acc);}case 6: _Tmp3=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1).is_union;_Tmp2=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1).fieldnum;_Tmp1=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f2;if(_Tmp3){int iu=_Tmp3;int f=_Tmp2;struct _fat_ptr d=_Tmp1;
# 533
unsigned sz=_get_fat_size(d,sizeof(void*));
this_ans=0U;
if(f == -1){
int i=0;for(0;(unsigned)i < sz;++ i){
if((int)Cyc_CfFlowInfo_initlevel_rec(env,((void**)d.curr)[i],1U)== 1){
this_ans=1U;
break;}}}else{
# 543
if((int)Cyc_CfFlowInfo_initlevel_rec(env,((void**)d.curr)[f],1U)== 1)
this_ans=1U;}
goto _LL0;}else{_Tmp1=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f2;{struct _fat_ptr d=_Tmp1;
# 547
unsigned sz=_get_fat_size(d,sizeof(void*));
this_ans=1U;
{int i=0;for(0;(unsigned)i < sz;++ i){
this_ans=Cyc_CfFlowInfo_initlevel_rec(env,((void**)d.curr)[i],this_ans);}}
goto _LL0;}}case 4: _Tmp4=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1;{struct Cyc_CfFlowInfo_Place*p=_Tmp4;
# 553
if(({(int(*)(int(*)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*,struct Cyc_CfFlowInfo_Place*))Cyc_List_mem;})(Cyc_CfFlowInfo_place_cmp,env->seen,p))
this_ans=1U;else{
# 556
({struct Cyc_List_List*_Tmp5=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));_Tmp6->hd=p,_Tmp6->tl=env->seen;_Tmp6;});env->seen=_Tmp5;});
this_ans=({struct Cyc_CfFlowInfo_InitlevelEnv*_Tmp5=env;Cyc_CfFlowInfo_initlevel_rec(_Tmp5,Cyc_CfFlowInfo_lookup_place(env->d,p),1U);});
env->seen=(_check_null(env->seen))->tl;}
# 560
goto _LL0;}case 5: _Tmp4=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1;{void*r=_Tmp4;
this_ans=Cyc_CfFlowInfo_initlevel_rec(env,r,1U);goto _LL0;}default:
 this_ans=Cyc_CfFlowInfo_initlevel_approx(r);goto _LL0;}_LL0:;}
# 564
return this_ans;}
# 566
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*env,struct Cyc_Dict_Dict d,void*r){
struct Cyc_CfFlowInfo_InitlevelEnv env2=({struct Cyc_CfFlowInfo_InitlevelEnv _Tmp0;_Tmp0.d=d,_Tmp0.seen=0;_Tmp0;});
return Cyc_CfFlowInfo_initlevel_rec(& env2,r,1U);}
# 571
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _stmttmp10=*place;struct Cyc_CfFlowInfo_Place _Tmp0=_stmttmp10;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.root;_Tmp1=_Tmp0.path;{void*root=_Tmp2;struct Cyc_List_List*path=_Tmp1;
void*ans=({(void*(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup;})(d,root);
for(1;path != 0;path=path->tl){
retry: {struct _tuple12 _stmttmp11=({struct _tuple12 _Tmp3;_Tmp3.f1=ans,_Tmp3.f2=(void*)path->hd;_Tmp3;});struct _tuple12 _Tmp3=_stmttmp11;int _Tmp4;struct _fat_ptr _Tmp5;struct Cyc_CfFlowInfo_UnionRInfo _Tmp6;void*_Tmp7;switch(*((int*)_Tmp3.f1)){case 8: _Tmp7=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp3.f1)->f2;{void*rval=_Tmp7;
# 577
ans=rval;goto retry;}case 7: _Tmp7=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp3.f1)->f3;{void*rval=_Tmp7;
# 580
ans=rval;goto retry;}case 6: if(*((int*)_Tmp3.f2)== 0){_Tmp6=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp3.f1)->f1;_Tmp5=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp3.f1)->f2;_Tmp4=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_Tmp3.f2)->f1;{struct Cyc_CfFlowInfo_UnionRInfo is_union=_Tmp6;struct _fat_ptr d2=_Tmp5;int fname=_Tmp4;
# 582
ans=*((void**)_check_fat_subscript(d2,sizeof(void*),fname));
goto _LL3;}}else{goto _LLC;}case 5: if(*((int*)_Tmp3.f2)== 1){_Tmp7=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_Tmp3.f1)->f1;{void*rval=_Tmp7;
# 585
ans=rval;
goto _LL3;}}else{goto _LLC;}default: _LLC:
# 593
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp8="bad lookup_place";_tag_fat(_Tmp8,sizeof(char),17U);}),_tag_fat(0U,sizeof(void*),0));}_LL3:;}}
# 595
return ans;}}
# 598
static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){
void*_Tmp0=rval;struct _fat_ptr _Tmp1;int _Tmp2;int _Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 3:
 return 0;case 7: _Tmp4=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f3;{void*r=_Tmp4;
return Cyc_CfFlowInfo_is_rval_unescaped(r);}case 8: _Tmp4=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f2;{void*r=_Tmp4;
return Cyc_CfFlowInfo_is_rval_unescaped(r);}case 6: _Tmp3=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1).is_union;_Tmp2=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1).fieldnum;_Tmp1=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f2;{int is_union=_Tmp3;int field_no=_Tmp2;struct _fat_ptr d=_Tmp1;
# 604
if(is_union && field_no != -1)
return !Cyc_CfFlowInfo_is_rval_unescaped(*((void**)_check_fat_subscript(d,sizeof(void*),field_no)));else{
# 607
unsigned sz=_get_fat_size(d,sizeof(void*));
{int i=0;for(0;(unsigned)i < sz;++ i){
if(!Cyc_CfFlowInfo_is_rval_unescaped(((void**)d.curr)[i]))return 0;}}
return 1;}}default:
# 612
 return 1;};}
# 615
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
return Cyc_CfFlowInfo_is_rval_unescaped(Cyc_CfFlowInfo_lookup_place(d,place));}
# 618
int Cyc_CfFlowInfo_is_init_pointer(void*rval){
void*_Tmp0=rval;void*_Tmp1;switch(*((int*)_Tmp0)){case 8: _Tmp1=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f2;{void*r=_Tmp1;
return Cyc_CfFlowInfo_is_init_pointer(r);}case 4:
 goto _LL6;case 5: _LL6:
 return 1;default:
 return 0;};}
# 628
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,void*r){
struct _RegionHandle _Tmp0=_new_region("rgn");struct _RegionHandle*rgn=& _Tmp0;_push_region(rgn);
{struct Cyc_CfFlowInfo_EscPile*pile=({struct Cyc_CfFlowInfo_EscPile*_Tmp1=_cycalloc(sizeof(struct Cyc_CfFlowInfo_EscPile));_Tmp1->places=0;_Tmp1;});
Cyc_CfFlowInfo_add_places(pile,r);{
struct Cyc_Dict_Dict _Tmp1=Cyc_CfFlowInfo_escape_these(fenv,pile,d);_npop_handler(0);return _Tmp1;}}
# 630
;_pop_region();}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;unsigned loc;struct Cyc_CfFlowInfo_Place*root_place;};
# 642
static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval){
# 647
struct _tuple12 _stmttmp12=({struct _tuple12 _Tmp0;_Tmp0.f1=oldval,_Tmp0.f2=newval;_Tmp0;});struct _tuple12 _Tmp0=_stmttmp12;enum Cyc_CfFlowInfo_InitLevel _Tmp1;struct _fat_ptr _Tmp2;struct Cyc_CfFlowInfo_UnionRInfo _Tmp3;struct _fat_ptr _Tmp4;struct Cyc_CfFlowInfo_UnionRInfo _Tmp5;void*_Tmp6;void*_Tmp7;if(*((int*)_Tmp0.f1)== 8){_Tmp7=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;{void*r1=_Tmp7;
# 649
return Cyc_CfFlowInfo_assign_place_inner(env,r1,newval);}}else{if(*((int*)_Tmp0.f2)== 8){_Tmp7=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1;_Tmp6=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f2;{struct Cyc_Absyn_Vardecl*n=_Tmp7;void*r=_Tmp6;
# 651
void*new_rval=Cyc_CfFlowInfo_assign_place_inner(env,oldval,r);
if(new_rval == r)return newval;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_Tmp8->tag=8,_Tmp8->f1=n,_Tmp8->f2=new_rval;_Tmp8;});}}}else{switch(*((int*)_Tmp0.f1)){case 3: if(*((int*)_Tmp0.f2)== 4){_Tmp7=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1;{struct Cyc_CfFlowInfo_Place*p=_Tmp7;
Cyc_CfFlowInfo_add_place(env->pile,p);goto _LL8;}}else{_LL8:
# 656
 if((int)Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= 1)
Cyc_CfFlowInfo_aerr(env->loc,({const char*_Tmp8="assignment puts possibly-uninitialized data in an escaped location";_tag_fat(_Tmp8,sizeof(char),67U);}),_tag_fat(0U,sizeof(void*),0));
# 659
return(env->fenv)->esc_all;}case 6: switch(*((int*)_Tmp0.f2)){case 6: _Tmp5=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;_Tmp4=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;_Tmp3=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1;_Tmp2=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f2;{struct Cyc_CfFlowInfo_UnionRInfo is_union1=_Tmp5;struct _fat_ptr d1=_Tmp4;struct Cyc_CfFlowInfo_UnionRInfo is_union2=_Tmp3;struct _fat_ptr d2=_Tmp2;
# 661
struct _fat_ptr new_d=({unsigned _Tmp8=
_get_fat_size(d1,sizeof(void*));_tag_fat(({void**_Tmp9=_cycalloc(_check_times(_Tmp8,sizeof(void*)));({{unsigned _TmpA=_get_fat_size(d1,sizeof(void*));unsigned i;for(i=0;i < _TmpA;++ i){({void*_TmpB=Cyc_CfFlowInfo_assign_place_inner(env,((void**)d1.curr)[(int)i],*((void**)_check_fat_subscript(d2,sizeof(void*),(int)i)));_Tmp9[i]=_TmpB;});}}0;});_Tmp9;}),sizeof(void*),_Tmp8);});
# 665
int change=0;
{int i=0;for(0;(unsigned)i < _get_fat_size(d1,sizeof(void*));++ i){
if(*((void**)_check_fat_subscript(new_d,sizeof(void*),i))!= ((void**)d1.curr)[i]){
change=1;break;}}}
# 670
if(!change){
if(!is_union1.is_union)return oldval;
new_d=d1;}else{
# 675
change=0;
{int i=0;for(0;(unsigned)i < _get_fat_size(d1,sizeof(void*));++ i){
if(({void*_Tmp8=*((void**)_check_fat_subscript(new_d,sizeof(void*),i));_Tmp8 != *((void**)_check_fat_subscript(d2,sizeof(void*),i));})){
change=1;break;}}}
# 680
if(!change){
if(!is_union1.is_union)return newval;
new_d=d2;}}
# 685
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_Tmp8->tag=6,_Tmp8->f1=is_union2,_Tmp8->f2=new_d;_Tmp8;});}case 3: goto _LLB;default: goto _LLD;}default: if(*((int*)_Tmp0.f2)== 3){_LLB: _Tmp1=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_Tmp1;
# 687
enum Cyc_CfFlowInfo_InitLevel _Tmp8=il;if(_Tmp8 == Cyc_CfFlowInfo_NoneIL)
return(env->fenv)->unknown_none;else{
return(env->fenv)->unknown_all;};}}else{_LLD:
# 691
 return newval;}}}};}
# 697
static int Cyc_CfFlowInfo_nprefix(int*n,void*unused){
if(*n > 0){*n=*n - 1;return 1;}else{
return 0;}}
# 701
static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*path,int path_prefix,void*oldval,void*newval){
# 712
if(path == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,newval);{
struct _tuple17 _stmttmp13=({struct _tuple17 _Tmp0;_Tmp0.f1=path,_Tmp0.f2=oldval;_Tmp0;});struct _tuple17 _Tmp0=_stmttmp13;struct _fat_ptr _Tmp1;int _Tmp2;int _Tmp3;int _Tmp4;void*_Tmp5;void*_Tmp6;switch(*((int*)_Tmp0.f2)){case 8: _Tmp6=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1;_Tmp5=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f2;{struct Cyc_Absyn_Vardecl*n=_Tmp6;void*r=_Tmp5;
# 715
void*new_r=Cyc_CfFlowInfo_assign_place_outer(env,path,path_prefix,r,newval);
if(new_r == r)return oldval;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_Tmp7->tag=8,_Tmp7->f1=n,_Tmp7->f2=new_r;_Tmp7;});}}case 7: _Tmp6=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1;_Tmp4=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f2;_Tmp5=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f3;_Tmp3=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f4;{struct Cyc_Absyn_Exp*x=_Tmp6;int y=_Tmp4;void*r=_Tmp5;int l=_Tmp3;
# 719
void*new_r=Cyc_CfFlowInfo_assign_place_outer(env,path,path_prefix,r,newval);
if(new_r == r)return oldval;else{
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct));_Tmp7->tag=7,_Tmp7->f1=x,_Tmp7->f2=y,_Tmp7->f3=new_r,_Tmp7->f4=l;_Tmp7;});}}default: if(_Tmp0.f1 != 0){if(*((int*)((struct Cyc_List_List*)_Tmp0.f1)->hd)== 1){if(*((int*)_Tmp0.f2)== 5){_Tmp6=(_Tmp0.f1)->tl;_Tmp5=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1;{struct Cyc_List_List*tl=_Tmp6;void*r=_Tmp5;
# 723
void*new_r=Cyc_CfFlowInfo_assign_place_outer(env,tl,path_prefix + 1,r,newval);
if(new_r == r)return oldval;else{
return(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct));_Tmp7->tag=5,_Tmp7->f1=new_r;_Tmp7;});}}}else{goto _LL9;}}else{if(*((int*)_Tmp0.f2)== 6){_Tmp4=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)(_Tmp0.f1)->hd)->f1;_Tmp6=(_Tmp0.f1)->tl;_Tmp3=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1).is_union;_Tmp2=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1).fieldnum;_Tmp1=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f2;{int fnum=_Tmp4;struct Cyc_List_List*tl=_Tmp6;int is_union=_Tmp3;int fldnum=_Tmp2;struct _fat_ptr d=_Tmp1;
# 727
void*new_child=Cyc_CfFlowInfo_assign_place_outer(env,tl,path_prefix + 1,((void**)d.curr)[fnum],newval);
# 729
struct _fat_ptr new_child_agg=Cyc_CfFlowInfo_aggr_dict_insert(d,fnum,new_child);
if((void**)new_child_agg.curr == (void**)d.curr &&(!is_union || fldnum == fnum))return oldval;
d=new_child_agg;
# 734
if(is_union){
int changed=0;
int sz=(int)_get_fat_size(d,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(i != fnum){
struct _fat_ptr new_d=({
struct _fat_ptr _Tmp7=d;int _Tmp8=i;Cyc_CfFlowInfo_aggr_dict_insert(_Tmp7,_Tmp8,
Cyc_CfFlowInfo_insert_place_inner((env->fenv)->unknown_all,((void**)d.curr)[i]));});
if((void**)new_d.curr != (void**)d.curr){
d=new_d;
changed=1;}}}}
# 750
if(changed){
struct Cyc_CfFlowInfo_Place*_stmttmp14=env->root_place;struct Cyc_CfFlowInfo_Place*_Tmp7=_stmttmp14;void*_Tmp8;void*_Tmp9;_Tmp9=_Tmp7->root;_Tmp8=_Tmp7->path;{void*root=_Tmp9;struct Cyc_List_List*path=_Tmp8;
struct Cyc_List_List*new_path=({struct Cyc_List_List*(*_TmpA)(int(*)(int*,void*),int*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(int(*)(int*,void*),int*,struct Cyc_List_List*))Cyc_List_filter_c;});int(*_TmpB)(int*,void*)=({(int(*)(int*,void*))Cyc_CfFlowInfo_nprefix;});int*_TmpC=({int*_TmpD=_cycalloc_atomic(sizeof(int));*_TmpD=path_prefix;_TmpD;});_TmpA(_TmpB,_TmpC,path);});
struct Cyc_CfFlowInfo_Place*curr_place=({struct Cyc_CfFlowInfo_Place*_TmpA=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));_TmpA->root=root,_TmpA->path=new_path;_TmpA;});;}}}
# 756
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_Tmp7->tag=6,(_Tmp7->f1).is_union=is_union,(_Tmp7->f1).fieldnum=fnum,_Tmp7->f2=d;_Tmp7;});}}else{goto _LL9;}}}else{_LL9:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp7="bad assign place";_tag_fat(_Tmp7,sizeof(char),17U);}),_tag_fat(0U,sizeof(void*),0));}};}}
# 760
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned loc,struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place,void*r){
# 768
struct Cyc_CfFlowInfo_Place*_Tmp0=place;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->root;_Tmp1=_Tmp0->path;{void*root=_Tmp2;struct Cyc_List_List*path=_Tmp1;
struct Cyc_CfFlowInfo_AssignEnv env=({struct Cyc_CfFlowInfo_AssignEnv _Tmp3;_Tmp3.fenv=fenv,({struct Cyc_CfFlowInfo_EscPile*_Tmp4=({struct Cyc_CfFlowInfo_EscPile*_Tmp5=_cycalloc(sizeof(struct Cyc_CfFlowInfo_EscPile));_Tmp5->places=0;_Tmp5;});_Tmp3.pile=_Tmp4;}),_Tmp3.d=d,_Tmp3.loc=loc,_Tmp3.root_place=place;_Tmp3;});
void*newval=({struct Cyc_List_List*_Tmp3=path;void*_Tmp4=({(void*(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup;})(d,root);Cyc_CfFlowInfo_assign_place_outer(& env,_Tmp3,0,_Tmp4,r);});
return({struct Cyc_CfFlowInfo_FlowEnv*_Tmp3=fenv;struct Cyc_CfFlowInfo_EscPile*_Tmp4=env.pile;Cyc_CfFlowInfo_escape_these(_Tmp3,_Tmp4,({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,void*))Cyc_Dict_insert;})(d,root,newval));});}}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;int is_try_flow;};
# 784
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*,void*,void*);
# 786
static int Cyc_CfFlowInfo_contains_local_consumed(void*r){
void*_Tmp0=r;void*_Tmp1;int _Tmp2;switch(*((int*)_Tmp0)){case 7: _Tmp2=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f4;{int l=_Tmp2;
# 789
return l;}case 5: _Tmp1=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1;{void*r1=_Tmp1;
# 791
return Cyc_CfFlowInfo_contains_local_consumed(r1);}case 8: _Tmp1=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f2;{void*ac=_Tmp1;
# 793
return Cyc_CfFlowInfo_contains_local_consumed(ac);}case 6:
# 795
 return 0;default:
# 797
 return 0;};}
# 802
static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*);struct _tuple18{int f1;int f2;};
# 808
static void*Cyc_CfFlowInfo_join_absRval_tryflow(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){
if(r1 == r2)return r1;{
struct _tuple18 _stmttmp15=({struct _tuple18 _Tmp0;({int _Tmp1=Cyc_CfFlowInfo_contains_local_consumed(r1);_Tmp0.f1=_Tmp1;}),({int _Tmp1=Cyc_CfFlowInfo_contains_local_consumed(r2);_Tmp0.f2=_Tmp1;});_Tmp0;});struct _tuple18 _Tmp0=_stmttmp15;switch((int)_Tmp0.f1){case 1: if(_Tmp0.f2 == 0)
return r2;else{goto _LL5;}case 0: if(_Tmp0.f2 == 1)
return r1;else{goto _LL5;}default: _LL5:
# 814
 return Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);};}}struct _tuple19{enum Cyc_CfFlowInfo_InitLevel f1;enum Cyc_CfFlowInfo_InitLevel f2;};
# 824
static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){
if(r1 == r2)return r1;
# 827
{struct _tuple12 _stmttmp16=({struct _tuple12 _Tmp0;_Tmp0.f1=r1,_Tmp0.f2=r2;_Tmp0;});struct _tuple12 _Tmp0=_stmttmp16;struct _fat_ptr _Tmp1;struct _fat_ptr _Tmp2;int _Tmp3;int _Tmp4;int _Tmp5;int _Tmp6;void*_Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;if(*((int*)_Tmp0.f1)== 8){if(*((int*)_Tmp0.f2)== 8){_TmpA=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;_Tmp9=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;_Tmp8=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1;_Tmp7=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f2;{struct Cyc_Absyn_Vardecl*n1=_TmpA;void*r1=_Tmp9;struct Cyc_Absyn_Vardecl*n2=_Tmp8;void*r2=_Tmp7;
# 830
if(n1 == n2)
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_TmpB=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_TmpB->tag=8,_TmpB->f1=n1,({void*_TmpC=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_TmpB->f2=_TmpC;});_TmpB;});else{
# 833
return Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);}}}else{_TmpA=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;_Tmp9=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;{struct Cyc_Absyn_Vardecl*n1=_TmpA;void*r1=_Tmp9;
return Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);}}}else{if(*((int*)_Tmp0.f2)== 8){_TmpA=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1;_Tmp9=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f2;{struct Cyc_Absyn_Vardecl*n2=_TmpA;void*r2=_Tmp9;
return Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);}}else{if(*((int*)_Tmp0.f1)== 7){if(*((int*)_Tmp0.f2)== 7){_TmpA=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;_Tmp6=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;_Tmp9=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f3;_Tmp5=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f4;_Tmp8=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1;_Tmp4=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f2;_Tmp7=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f3;_Tmp3=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f4;{struct Cyc_Absyn_Exp*e1=_TmpA;int i1=_Tmp6;void*r1=_Tmp9;int l1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp8;int i2=_Tmp4;void*r2=_Tmp7;int l2=_Tmp3;
# 838
if(e1 == e2)
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_TmpB=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct));_TmpB->tag=7,_TmpB->f1=e1,i1 > i2?_TmpB->f2=i1:(_TmpB->f2=i2),({void*_TmpC=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_TmpB->f3=_TmpC;}),_TmpB->f4=l1 && l2;_TmpB;});
{void*_TmpB=r1;void*_TmpC;if(*((int*)_TmpB)== 4){_TmpC=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_TmpB)->f1;{struct Cyc_CfFlowInfo_Place*p=_TmpC;
Cyc_CfFlowInfo_add_place(env->pile,p);goto _LL21;}}else{
goto _LL21;}_LL21:;}
# 844
{void*_TmpB=r2;void*_TmpC;if(*((int*)_TmpB)== 4){_TmpC=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_TmpB)->f1;{struct Cyc_CfFlowInfo_Place*p=_TmpC;
Cyc_CfFlowInfo_add_place(env->pile,p);goto _LL26;}}else{
goto _LL26;}_LL26:;}
# 848
goto _LL0;}}else{_TmpA=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;_Tmp6=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;_Tmp9=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f3;_Tmp5=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f4;{struct Cyc_Absyn_Exp*e1=_TmpA;int i1=_Tmp6;void*r1=_Tmp9;int l1=_Tmp5;
# 850
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_TmpB=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct));_TmpB->tag=7,_TmpB->f1=e1,_TmpB->f2=i1,({void*_TmpC=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_TmpB->f3=_TmpC;}),_TmpB->f4=l1;_TmpB;});}}}else{if(*((int*)_Tmp0.f2)== 7){_TmpA=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1;_Tmp6=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f2;_Tmp9=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f3;_Tmp5=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f4;{struct Cyc_Absyn_Exp*e2=_TmpA;int i2=_Tmp6;void*r2=_Tmp9;int l2=_Tmp5;
# 852
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_TmpB=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct));_TmpB->tag=7,_TmpB->f1=e2,_TmpB->f2=i2,({void*_TmpC=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_TmpB->f3=_TmpC;}),_TmpB->f4=l2;_TmpB;});}}else{switch(*((int*)_Tmp0.f1)){case 4: switch(*((int*)_Tmp0.f2)){case 4: _TmpA=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;_Tmp9=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1;{struct Cyc_CfFlowInfo_Place*p1=_TmpA;struct Cyc_CfFlowInfo_Place*p2=_Tmp9;
# 855
if(Cyc_CfFlowInfo_place_cmp(p1,p2)== 0)return r1;
Cyc_CfFlowInfo_add_place(env->pile,p1);
Cyc_CfFlowInfo_add_place(env->pile,p2);
goto _LL0;}case 1: _TmpA=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;{struct Cyc_CfFlowInfo_Place*p=_TmpA;
# 861
Cyc_CfFlowInfo_add_place(env->pile,p);{
enum Cyc_CfFlowInfo_InitLevel _stmttmp17=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);enum Cyc_CfFlowInfo_InitLevel _TmpB=_stmttmp17;if(_TmpB == Cyc_CfFlowInfo_AllIL)
return(env->fenv)->notzeroall;else{
return(env->fenv)->unknown_none;};}}default: _TmpA=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;{struct Cyc_CfFlowInfo_Place*p=_TmpA;
# 887
Cyc_CfFlowInfo_add_place(env->pile,p);goto _LL0;}}case 1: switch(*((int*)_Tmp0.f2)){case 4: _TmpA=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1;{struct Cyc_CfFlowInfo_Place*p=_TmpA;
# 867
Cyc_CfFlowInfo_add_place(env->pile,p);{
enum Cyc_CfFlowInfo_InitLevel _stmttmp18=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);enum Cyc_CfFlowInfo_InitLevel _TmpB=_stmttmp18;if(_TmpB == Cyc_CfFlowInfo_AllIL)
return(env->fenv)->notzeroall;else{
return(env->fenv)->unknown_none;};}}case 5: _TmpA=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1;{void*r2=_TmpA;
# 882
enum Cyc_CfFlowInfo_InitLevel _stmttmp1A=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);enum Cyc_CfFlowInfo_InitLevel _TmpB=_stmttmp1A;if(_TmpB == Cyc_CfFlowInfo_AllIL)
return(env->fenv)->notzeroall;else{
return(env->fenv)->unknown_none;};}default: goto _LL1F;}case 5: switch(*((int*)_Tmp0.f2)){case 5: _TmpA=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;_Tmp9=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1;{void*r1=_TmpA;void*r2=_Tmp9;
# 874
return(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_TmpB=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct));_TmpB->tag=5,({void*_TmpC=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_TmpB->f1=_TmpC;});_TmpB;});}case 1: _TmpA=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;{void*r1=_TmpA;
# 877
enum Cyc_CfFlowInfo_InitLevel _stmttmp19=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);enum Cyc_CfFlowInfo_InitLevel _TmpB=_stmttmp19;if(_TmpB == Cyc_CfFlowInfo_AllIL)
return(env->fenv)->notzeroall;else{
return(env->fenv)->unknown_none;};}case 4: goto _LL1B;default: goto _LL1F;}default: if(*((int*)_Tmp0.f2)== 4){_LL1B: _TmpA=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1;{struct Cyc_CfFlowInfo_Place*p=_TmpA;
# 888
Cyc_CfFlowInfo_add_place(env->pile,p);goto _LL0;}}else{if(*((int*)_Tmp0.f1)== 6){if(*((int*)_Tmp0.f2)== 6){_Tmp6=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1).is_union;_Tmp5=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1).fieldnum;_Tmp2=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;_Tmp4=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1).is_union;_Tmp3=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1).fieldnum;_Tmp1=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f2;{int is_union1=_Tmp6;int field_no1=_Tmp5;struct _fat_ptr d1=_Tmp2;int is_union2=_Tmp4;int field_no2=_Tmp3;struct _fat_ptr d2=_Tmp1;
# 891
struct _fat_ptr new_d=({unsigned _TmpB=
_get_fat_size(d1,sizeof(void*));_tag_fat(({void**_TmpC=_cycalloc(_check_times(_TmpB,sizeof(void*)));({{unsigned _TmpD=_get_fat_size(d1,sizeof(void*));unsigned i;for(i=0;i < _TmpD;++ i){env->is_try_flow?({void*_TmpE=({(void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,int,void*,void*))Cyc_CfFlowInfo_join_absRval_tryflow;})(env,0,((void**)d1.curr)[(int)i],((void**)d2.curr)[(int)i]);_TmpC[i]=_TmpE;}):({void*_TmpE=Cyc_CfFlowInfo_join_absRval(env,a,((void**)d1.curr)[(int)i],((void**)d2.curr)[(int)i]);_TmpC[i]=_TmpE;});}}0;});_TmpC;}),sizeof(void*),_TmpB);});
# 897
int change=0;
{int i=0;for(0;(unsigned)i < _get_fat_size(d1,sizeof(void*));++ i){
if(((void**)new_d.curr)[i]!= ((void**)d1.curr)[i]){
change=1;break;}}}
# 902
if(!change){
if(!is_union1)return r1;
new_d=d1;}else{
# 907
change=0;
{int i=0;for(0;(unsigned)i < _get_fat_size(d1,sizeof(void*));++ i){
if(((void**)new_d.curr)[i]!= ((void**)d2.curr)[i]){
change=1;break;}}}
# 912
if(!change){
if(!is_union1)return r2;
new_d=d2;}}
# 917
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_TmpB=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_TmpB->tag=6,(_TmpB->f1).is_union=is_union1,field_no1 == field_no2?(_TmpB->f1).fieldnum=field_no1:((_TmpB->f1).fieldnum=-1),_TmpB->f2=new_d;_TmpB;});}}else{goto _LL1F;}}else{_LL1F:
# 919
 goto _LL0;}}}}}}}_LL0:;}{
# 921
enum Cyc_CfFlowInfo_InitLevel il1=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);
enum Cyc_CfFlowInfo_InitLevel il2=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);
struct _tuple12 _stmttmp1B=({struct _tuple12 _Tmp0;_Tmp0.f1=r1,_Tmp0.f2=r2;_Tmp0;});struct _tuple12 _Tmp0=_stmttmp1B;if(*((int*)_Tmp0.f1)== 3)
goto _LL43;else{if(*((int*)_Tmp0.f2)== 3){_LL43: {
# 926
struct _tuple19 _stmttmp1C=({struct _tuple19 _Tmp1;_Tmp1.f1=il1,_Tmp1.f2=il2;_Tmp1;});struct _tuple19 _Tmp1=_stmttmp1C;if(_Tmp1.f2 == Cyc_CfFlowInfo_NoneIL)
goto _LL4A;else{if(_Tmp1.f1 == Cyc_CfFlowInfo_NoneIL){_LL4A: return(env->fenv)->esc_none;}else{
return(env->fenv)->esc_all;}};}}else{
# 931
struct _tuple19 _stmttmp1D=({struct _tuple19 _Tmp1;_Tmp1.f1=il1,_Tmp1.f2=il2;_Tmp1;});struct _tuple19 _Tmp1=_stmttmp1D;if(_Tmp1.f2 == Cyc_CfFlowInfo_NoneIL)
goto _LL51;else{if(_Tmp1.f1 == Cyc_CfFlowInfo_NoneIL){_LL51: return(env->fenv)->unknown_none;}else{
return(env->fenv)->unknown_all;}};}};}}struct _tuple20{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
# 942
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_tryflow(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo newflow,union Cyc_CfFlowInfo_FlowInfo oldflow){
# 944
struct _tuple20 _stmttmp1E=({struct _tuple20 _Tmp0;_Tmp0.f1=newflow,_Tmp0.f2=oldflow;_Tmp0;});struct _tuple20 _Tmp0=_stmttmp1E;void*_Tmp1;struct Cyc_Dict_Dict _Tmp2;void*_Tmp3;struct Cyc_Dict_Dict _Tmp4;if(((_Tmp0.f1).BottomFL).tag == 1)
return oldflow;else{if(((_Tmp0.f2).BottomFL).tag == 1)
return newflow;else{_Tmp4=(((_Tmp0.f1).ReachableFL).val).f1;_Tmp3=(((_Tmp0.f1).ReachableFL).val).f2;_Tmp2=(((_Tmp0.f2).ReachableFL).val).f1;_Tmp1=(((_Tmp0.f2).ReachableFL).val).f2;{struct Cyc_Dict_Dict d1=_Tmp4;struct Cyc_List_List*r1=_Tmp3;struct Cyc_Dict_Dict d2=_Tmp2;struct Cyc_List_List*r2=_Tmp1;
# 950
if(d1.t == d2.t && r1 == r2)return newflow;
if(Cyc_CfFlowInfo_flow_lessthan_approx(newflow,oldflow))return oldflow;
if(Cyc_CfFlowInfo_flow_lessthan_approx(oldflow,newflow))return newflow;{
struct Cyc_CfFlowInfo_JoinEnv env=({struct Cyc_CfFlowInfo_JoinEnv _Tmp5;_Tmp5.fenv=fenv,({struct Cyc_CfFlowInfo_EscPile*_Tmp6=({struct Cyc_CfFlowInfo_EscPile*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_EscPile));_Tmp7->places=0;_Tmp7;});_Tmp5.pile=_Tmp6;}),_Tmp5.d1=d1,_Tmp5.d2=d2,_Tmp5.is_try_flow=1;_Tmp5;});
struct Cyc_Dict_Dict outdict=({(struct Cyc_Dict_Dict(*)(void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict))Cyc_Dict_intersect_c;})(({(void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*))Cyc_CfFlowInfo_join_absRval_tryflow;}),& env,d1,d2);
struct Cyc_List_List*r=Cyc_Relations_join_relns(Cyc_Core_heap_region,r1,r2);
return({struct Cyc_Dict_Dict _Tmp5=Cyc_CfFlowInfo_escape_these(fenv,env.pile,outdict);Cyc_CfFlowInfo_ReachableFL(_Tmp5,r);});}}}};}
# 960
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 962
struct _tuple20 _stmttmp1F=({struct _tuple20 _Tmp0;_Tmp0.f1=f1,_Tmp0.f2=f2;_Tmp0;});struct _tuple20 _Tmp0=_stmttmp1F;void*_Tmp1;struct Cyc_Dict_Dict _Tmp2;void*_Tmp3;struct Cyc_Dict_Dict _Tmp4;if(((_Tmp0.f1).BottomFL).tag == 1)
return f2;else{if(((_Tmp0.f2).BottomFL).tag == 1)
return f1;else{_Tmp4=(((_Tmp0.f1).ReachableFL).val).f1;_Tmp3=(((_Tmp0.f1).ReachableFL).val).f2;_Tmp2=(((_Tmp0.f2).ReachableFL).val).f1;_Tmp1=(((_Tmp0.f2).ReachableFL).val).f2;{struct Cyc_Dict_Dict d1=_Tmp4;struct Cyc_List_List*r1=_Tmp3;struct Cyc_Dict_Dict d2=_Tmp2;struct Cyc_List_List*r2=_Tmp1;
# 968
if(d1.t == d2.t && r1 == r2)return f1;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))return f2;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{
struct Cyc_CfFlowInfo_JoinEnv env=({struct Cyc_CfFlowInfo_JoinEnv _Tmp5;_Tmp5.fenv=fenv,({struct Cyc_CfFlowInfo_EscPile*_Tmp6=({struct Cyc_CfFlowInfo_EscPile*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_EscPile));_Tmp7->places=0;_Tmp7;});_Tmp5.pile=_Tmp6;}),_Tmp5.d1=d1,_Tmp5.d2=d2,_Tmp5.is_try_flow=0;_Tmp5;});
struct Cyc_Dict_Dict outdict=({(struct Cyc_Dict_Dict(*)(void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict))Cyc_Dict_intersect_c;})(({(void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*))Cyc_CfFlowInfo_join_absRval;}),& env,d1,d2);
struct Cyc_List_List*r=Cyc_Relations_join_relns(Cyc_Core_heap_region,r1,r2);
return({struct Cyc_Dict_Dict _Tmp5=Cyc_CfFlowInfo_escape_these(fenv,env.pile,outdict);Cyc_CfFlowInfo_ReachableFL(_Tmp5,r);});}}}};}struct _tuple21{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};struct _tuple22{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;union Cyc_CfFlowInfo_FlowInfo f3;};
# 978
struct _tuple21 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct _tuple21 pr1,struct _tuple21 pr2){
# 983
struct _tuple21 _Tmp0=pr1;void*_Tmp1;union Cyc_CfFlowInfo_FlowInfo _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{union Cyc_CfFlowInfo_FlowInfo f1=_Tmp2;void*r1=_Tmp1;
struct _tuple21 _Tmp3=pr2;void*_Tmp4;union Cyc_CfFlowInfo_FlowInfo _Tmp5;_Tmp5=_Tmp3.f1;_Tmp4=_Tmp3.f2;{union Cyc_CfFlowInfo_FlowInfo f2=_Tmp5;void*r2=_Tmp4;
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,f1,f2);
struct _tuple22 _stmttmp20=({struct _tuple22 _Tmp6;_Tmp6.f1=f1,_Tmp6.f2=f2,_Tmp6.f3=outflow;_Tmp6;});struct _tuple22 _Tmp6=_stmttmp20;void*_Tmp7;struct Cyc_Dict_Dict _Tmp8;struct Cyc_Dict_Dict _Tmp9;struct Cyc_Dict_Dict _TmpA;if(((_Tmp6.f1).BottomFL).tag == 1)
return({struct _tuple21 _TmpB;_TmpB.f1=outflow,_TmpB.f2=r2;_TmpB;});else{if(((_Tmp6.f2).BottomFL).tag == 1)
return({struct _tuple21 _TmpB;_TmpB.f1=outflow,_TmpB.f2=r1;_TmpB;});else{if(((_Tmp6.f3).ReachableFL).tag == 2){_TmpA=(((_Tmp6.f1).ReachableFL).val).f1;_Tmp9=(((_Tmp6.f2).ReachableFL).val).f1;_Tmp8=(((_Tmp6.f3).ReachableFL).val).f1;_Tmp7=(((_Tmp6.f3).ReachableFL).val).f2;{struct Cyc_Dict_Dict d1=_TmpA;struct Cyc_Dict_Dict d2=_Tmp9;struct Cyc_Dict_Dict outd=_Tmp8;struct Cyc_List_List*relns=_Tmp7;
# 991
if(({(int(*)(int,void*,void*))Cyc_CfFlowInfo_absRval_lessthan_approx;})(0,r1,r2))return({struct _tuple21 _TmpB;_TmpB.f1=outflow,_TmpB.f2=r2;_TmpB;});
if(({(int(*)(int,void*,void*))Cyc_CfFlowInfo_absRval_lessthan_approx;})(0,r2,r1))return({struct _tuple21 _TmpB;_TmpB.f1=outflow,_TmpB.f2=r1;_TmpB;});{
struct Cyc_CfFlowInfo_JoinEnv env=({struct Cyc_CfFlowInfo_JoinEnv _TmpB;_TmpB.fenv=fenv,({struct Cyc_CfFlowInfo_EscPile*_TmpC=({struct Cyc_CfFlowInfo_EscPile*_TmpD=_cycalloc(sizeof(struct Cyc_CfFlowInfo_EscPile));_TmpD->places=0;_TmpD;});_TmpB.pile=_TmpC;}),_TmpB.d1=d1,_TmpB.d2=d2,_TmpB.is_try_flow=0;_TmpB;});
void*outr=({(void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,int,void*,void*))Cyc_CfFlowInfo_join_absRval;})(& env,0,r1,r2);
return({struct _tuple21 _TmpB;({union Cyc_CfFlowInfo_FlowInfo _TmpC=({struct Cyc_Dict_Dict _TmpD=Cyc_CfFlowInfo_escape_these(fenv,env.pile,outd);Cyc_CfFlowInfo_ReachableFL(_TmpD,relns);});_TmpB.f1=_TmpC;}),_TmpB.f2=outr;_TmpB;});}}}else{
# 997
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_TmpB="join_flow_and_rval: BottomFL outflow";_tag_fat(_TmpB,sizeof(char),37U);}),_tag_fat(0U,sizeof(void*),0));}}};}}}
# 1002
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2){
if(r1 == r2)return 1;
# 1005
{struct _tuple12 _stmttmp21=({struct _tuple12 _Tmp0;_Tmp0.f1=r1,_Tmp0.f2=r2;_Tmp0;});struct _tuple12 _Tmp0=_stmttmp21;struct _fat_ptr _Tmp1;int _Tmp2;int _Tmp3;struct _fat_ptr _Tmp4;int _Tmp5;int _Tmp6;void*_Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;if(*((int*)_Tmp0.f1)== 8){if(*((int*)_Tmp0.f2)== 8){_TmpA=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;_Tmp9=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;_Tmp8=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1;_Tmp7=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f2;{struct Cyc_Absyn_Vardecl*n1=_TmpA;void*r1=_Tmp9;struct Cyc_Absyn_Vardecl*n2=_Tmp8;void*r2=_Tmp7;
# 1007
return n1 == n2 && Cyc_CfFlowInfo_absRval_lessthan_approx(ignore,r1,r2);}}else{
goto _LL6;}}else{if(*((int*)_Tmp0.f2)== 8){_LL6:
 return 0;}else{if(*((int*)_Tmp0.f1)== 4){if(*((int*)_Tmp0.f2)== 4){_TmpA=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;_Tmp9=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1;{struct Cyc_CfFlowInfo_Place*p1=_TmpA;struct Cyc_CfFlowInfo_Place*p2=_Tmp9;
return Cyc_CfFlowInfo_place_cmp(p1,p2)== 0;}}else{
goto _LLC;}}else{if(*((int*)_Tmp0.f2)== 4){_LLC:
 return 0;}else{if(*((int*)_Tmp0.f1)== 5){if(*((int*)_Tmp0.f2)== 5){_TmpA=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;_Tmp9=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1;{void*r1=_TmpA;void*r2=_Tmp9;
return Cyc_CfFlowInfo_absRval_lessthan_approx(ignore,r1,r2);}}else{
goto _LL12;}}else{if(*((int*)_Tmp0.f2)== 5){_LL12:
 return 0;}else{if(*((int*)_Tmp0.f1)== 6)switch(*((int*)_Tmp0.f2)){case 6: _Tmp6=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1).is_union;_Tmp5=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1).fieldnum;_Tmp4=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;_Tmp3=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1).is_union;_Tmp2=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f1).fieldnum;_Tmp1=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f2)->f2;{int is_union1=_Tmp6;int fld1=_Tmp5;struct _fat_ptr d1=_Tmp4;int is_union2=_Tmp3;int fld2=_Tmp2;struct _fat_ptr d2=_Tmp1;
# 1018
if(is_union1 && fld1 != fld2)return 0;
if((void**)d1.curr == (void**)d2.curr)return 1;
{int i=0;for(0;(unsigned)i < _get_fat_size(d1,sizeof(void*));++ i){
if(!({(int(*)(int,void*,void*))Cyc_CfFlowInfo_absRval_lessthan_approx;})(0,((void**)d1.curr)[i],((void**)d2.curr)[i]))return 0;}}
return 1;}case 0: goto _LL15;case 1: goto _LL17;default: goto _LL1D;}else{switch(*((int*)_Tmp0.f2)){case 0: _LL15:
 goto _LL18;case 1: _LL17: _LL18:
 return 0;default: if(*((int*)_Tmp0.f1)== 3){if(*((int*)_Tmp0.f2)== 3)
goto _LL0;else{
return 0;}}else{_LL1D:
# 1033
 goto _LL0;}}}}}}}}}_LL0:;}{
# 1035
struct _tuple19 _stmttmp22=({struct _tuple19 _Tmp0;({enum Cyc_CfFlowInfo_InitLevel _Tmp1=Cyc_CfFlowInfo_initlevel_approx(r1);_Tmp0.f1=_Tmp1;}),({enum Cyc_CfFlowInfo_InitLevel _Tmp1=Cyc_CfFlowInfo_initlevel_approx(r2);_Tmp0.f2=_Tmp1;});_Tmp0;});struct _tuple19 _Tmp0=_stmttmp22;if(_Tmp0.f1 == Cyc_CfFlowInfo_AllIL)
goto _LL23;else{if(_Tmp0.f2 == Cyc_CfFlowInfo_NoneIL){_LL23:
 return 1;}else{
return 0;}};}}
# 1048
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 1050
struct _tuple20 _stmttmp23=({struct _tuple20 _Tmp0;_Tmp0.f1=f1,_Tmp0.f2=f2;_Tmp0;});struct _tuple20 _Tmp0=_stmttmp23;void*_Tmp1;struct Cyc_Dict_Dict _Tmp2;void*_Tmp3;struct Cyc_Dict_Dict _Tmp4;if(((_Tmp0.f1).BottomFL).tag == 1)
return 1;else{if(((_Tmp0.f2).BottomFL).tag == 1)
return 0;else{_Tmp4=(((_Tmp0.f1).ReachableFL).val).f1;_Tmp3=(((_Tmp0.f1).ReachableFL).val).f2;_Tmp2=(((_Tmp0.f2).ReachableFL).val).f1;_Tmp1=(((_Tmp0.f2).ReachableFL).val).f2;{struct Cyc_Dict_Dict d1=_Tmp4;struct Cyc_List_List*r1=_Tmp3;struct Cyc_Dict_Dict d2=_Tmp2;struct Cyc_List_List*r2=_Tmp1;
# 1054
if(d1.t == d2.t && r1 == r2)return 1;
return({(int(*)(int(*)(void*,void*,void*),struct Cyc_Dict_Dict,struct Cyc_Dict_Dict))Cyc_Dict_forall_intersect;})(({(int(*)(void*,void*,void*))Cyc_CfFlowInfo_absRval_lessthan_approx;}),d1,d2)&&
 Cyc_Relations_relns_approx(r1,r2);}}};}struct _tuple23{void*f1;struct Cyc_List_List*f2;};
# 1060
struct _tuple23 Cyc_CfFlowInfo_unname_rval(void*rv){
struct Cyc_List_List*names=0;
int done=0;
while(!done){
void*_Tmp0=rv;void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0)== 8){_Tmp2=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1;_Tmp1=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;void*rv2=_Tmp1;
# 1066
names=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=vd,_Tmp3->tl=names;_Tmp3;});rv=rv2;goto _LL0;}}else{
# 1068
done=1;goto _LL0;}_LL0:;}
# 1071
return({struct _tuple23 _Tmp0;_Tmp0.f1=rv,_Tmp0.f2=names;_Tmp0;});}
# 1074
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il){
enum Cyc_CfFlowInfo_InitLevel _Tmp0=il;if(_Tmp0 == Cyc_CfFlowInfo_NoneIL){
Cyc_fprintf(Cyc_stderr,({const char*_Tmp1="uninitialized";_tag_fat(_Tmp1,sizeof(char),14U);}),_tag_fat(0U,sizeof(void*),0));goto _LL0;}else{
Cyc_fprintf(Cyc_stderr,({const char*_Tmp1="all-initialized";_tag_fat(_Tmp1,sizeof(char),16U);}),_tag_fat(0U,sizeof(void*),0));goto _LL0;}_LL0:;}
# 1081
void Cyc_CfFlowInfo_print_root(void*root){
void*_Tmp0=root;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 0: _Tmp2=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;
# 1084
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=Cyc_Absynpp_qvar2string(vd->name);_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="Root(%s)";_tag_fat(_Tmp5,sizeof(char),9U);}),_tag_fat(_Tmp4,sizeof(void*),1));});goto _LL0;}case 1: _Tmp2=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_Tmp0)->f1;_Tmp1=(void*)((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp2;void*t=_Tmp1;
# 1086
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({
struct _fat_ptr _Tmp5=Cyc_Absynpp_exp2string(e);_Tmp4.f1=_Tmp5;});_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_Absynpp_typ2string(t);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="MallocPt(%s,%s)";_tag_fat(_Tmp6,sizeof(char),16U);}),_tag_fat(_Tmp5,sizeof(void*),2));});goto _LL0;}default:
# 1089
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="InitParam(_,_)";_tag_fat(_Tmp3,sizeof(char),15U);}),_tag_fat(0U,sizeof(void*),0));goto _LL0;}_LL0:;}
# 1093
void Cyc_CfFlowInfo_print_path(struct Cyc_List_List*p){
{struct Cyc_List_List*x=p;for(0;x != 0;x=(_check_null(x))->tl){
void*_stmttmp24=(void*)x->hd;void*_Tmp0=_stmttmp24;int _Tmp1;if(*((int*)_Tmp0)== 0){_Tmp1=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_Tmp0)->f1;{int i=_Tmp1;
# 1097
({struct Cyc_Int_pa_PrintArg_struct _Tmp2=({struct Cyc_Int_pa_PrintArg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=(unsigned long)i;_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_fprintf(Cyc_stderr,({const char*_Tmp4=".%d";_tag_fat(_Tmp4,sizeof(char),4U);}),_tag_fat(_Tmp3,sizeof(void*),1));});
goto _LL0;}}else{
# 1100
if(x->tl != 0){
void*_stmttmp25=(void*)(_check_null(x->tl))->hd;void*_Tmp2=_stmttmp25;int _Tmp3;if(*((int*)_Tmp2)== 0){_Tmp3=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_Tmp2)->f1;{int i=_Tmp3;
# 1103
x=x->tl;
({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)i;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="->%d";_tag_fat(_Tmp6,sizeof(char),5U);}),_tag_fat(_Tmp5,sizeof(void*),1));});
continue;}}else{
goto _LL5;}_LL5:;}
# 1109
Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="*";_tag_fat(_Tmp2,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));}_LL0:;}}
# 1112
Cyc_fprintf(Cyc_stderr,({const char*_Tmp0=" ";_tag_fat(_Tmp0,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));}
# 1115
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p){
Cyc_CfFlowInfo_print_root(p->root);
Cyc_CfFlowInfo_print_path(p->path);}
# 1120
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*pr)(void*)){
int first=1;
while(x != 0){
if(!first){Cyc_fprintf(Cyc_stderr,({const char*_Tmp0=", ";_tag_fat(_Tmp0,sizeof(char),3U);}),_tag_fat(0U,sizeof(void*),0));first=0;}
pr(x->hd);
x=x->tl;}
# 1127
Cyc_fprintf(Cyc_stderr,({const char*_Tmp0="\n";_tag_fat(_Tmp0,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));}
# 1130
void Cyc_CfFlowInfo_print_absrval(void*rval){
void*_Tmp0=rval;void*_Tmp1;struct _fat_ptr _Tmp2;int _Tmp3;int _Tmp4;void*_Tmp5;enum Cyc_CfFlowInfo_InitLevel _Tmp6;switch(*((int*)_Tmp0)){case 0:
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp7="Zero";_tag_fat(_Tmp7,sizeof(char),5U);}),_tag_fat(0U,sizeof(void*),0));goto _LL0;case 1:
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp7="NotZeroAll";_tag_fat(_Tmp7,sizeof(char),11U);}),_tag_fat(0U,sizeof(void*),0));goto _LL0;case 2: _Tmp6=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_Tmp6;
Cyc_fprintf(Cyc_stderr,({const char*_Tmp7="Unknown(";_tag_fat(_Tmp7,sizeof(char),9U);}),_tag_fat(0U,sizeof(void*),0));Cyc_CfFlowInfo_print_initlevel(il);
Cyc_fprintf(Cyc_stderr,({const char*_Tmp7=")";_tag_fat(_Tmp7,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));goto _LL0;}case 3: _Tmp6=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_Tmp6;
Cyc_fprintf(Cyc_stderr,({const char*_Tmp7="Esc(";_tag_fat(_Tmp7,sizeof(char),5U);}),_tag_fat(0U,sizeof(void*),0));Cyc_CfFlowInfo_print_initlevel(il);
Cyc_fprintf(Cyc_stderr,({const char*_Tmp7=")";_tag_fat(_Tmp7,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));goto _LL0;}case 4: _Tmp5=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1;{struct Cyc_CfFlowInfo_Place*p=_Tmp5;
Cyc_fprintf(Cyc_stderr,({const char*_Tmp7="AddrOf(";_tag_fat(_Tmp7,sizeof(char),8U);}),_tag_fat(0U,sizeof(void*),0));Cyc_CfFlowInfo_print_place(p);
Cyc_fprintf(Cyc_stderr,({const char*_Tmp7=")";_tag_fat(_Tmp7,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));goto _LL0;}case 5: _Tmp5=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1;{void*r=_Tmp5;
Cyc_fprintf(Cyc_stderr,({const char*_Tmp7="UniquePtr(";_tag_fat(_Tmp7,sizeof(char),11U);}),_tag_fat(0U,sizeof(void*),0));Cyc_CfFlowInfo_print_absrval(r);
Cyc_fprintf(Cyc_stderr,({const char*_Tmp7=")";_tag_fat(_Tmp7,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));goto _LL0;}case 6: _Tmp4=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1).is_union;_Tmp3=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1).fieldnum;_Tmp2=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f2;{int is_union=_Tmp4;int f=_Tmp3;struct _fat_ptr d=_Tmp2;
# 1143
if(is_union){
Cyc_fprintf(Cyc_stderr,({const char*_Tmp7="AggrUnion{";_tag_fat(_Tmp7,sizeof(char),11U);}),_tag_fat(0U,sizeof(void*),0));
if(f != -1)
({struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned long)f;_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;Cyc_fprintf(Cyc_stderr,({const char*_Tmp9="tag == %d;";_tag_fat(_Tmp9,sizeof(char),11U);}),_tag_fat(_Tmp8,sizeof(void*),1));});}else{
# 1148
Cyc_fprintf(Cyc_stderr,({const char*_Tmp7="AggrStruct{";_tag_fat(_Tmp7,sizeof(char),12U);}),_tag_fat(0U,sizeof(void*),0));}
{int i=0;for(0;(unsigned)i < _get_fat_size(d,sizeof(void*));++ i){
Cyc_CfFlowInfo_print_absrval(((void**)d.curr)[i]);
if((unsigned)(i + 1)< _get_fat_size(d,sizeof(void*)))Cyc_fprintf(Cyc_stderr,({const char*_Tmp7=",";_tag_fat(_Tmp7,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));}}
# 1153
Cyc_fprintf(Cyc_stderr,({const char*_Tmp7="}";_tag_fat(_Tmp7,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));
goto _LL0;}case 7: _Tmp5=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f2;_Tmp1=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f3;_Tmp3=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e=_Tmp5;int i=_Tmp4;void*r=_Tmp1;int l=_Tmp3;
# 1156
({struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=Cyc_Absynpp_exp2string(e);_Tmp8.f1=_Tmp9;});_Tmp8;});struct Cyc_Int_pa_PrintArg_struct _Tmp8=({struct Cyc_Int_pa_PrintArg_struct _Tmp9;_Tmp9.tag=1,_Tmp9.f1=(unsigned long)i;_Tmp9;});void*_Tmp9[2];_Tmp9[0]=& _Tmp7,_Tmp9[1]=& _Tmp8;Cyc_fprintf(Cyc_stderr,({const char*_TmpA="[Consumed(%s,%d,";_tag_fat(_TmpA,sizeof(char),17U);}),_tag_fat(_Tmp9,sizeof(void*),2));});
Cyc_CfFlowInfo_print_absrval(r);({struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=l?({const char*_Tmp9="local";_tag_fat(_Tmp9,sizeof(char),6U);}):({const char*_Tmp9="";_tag_fat(_Tmp9,sizeof(char),1U);});_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;Cyc_fprintf(Cyc_stderr,({const char*_Tmp9=",%s)]";_tag_fat(_Tmp9,sizeof(char),6U);}),_tag_fat(_Tmp8,sizeof(void*),1));});
goto _LL0;}default: _Tmp5=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f1;_Tmp1=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*n=_Tmp5;void*r=_Tmp1;
# 1160
({struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=Cyc_Absynpp_qvar2string(n->name);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;Cyc_fprintf(Cyc_stderr,({const char*_Tmp9="[NamedLocation(%s,";_tag_fat(_Tmp9,sizeof(char),19U);}),_tag_fat(_Tmp8,sizeof(void*),1));});
Cyc_CfFlowInfo_print_absrval(r);Cyc_fprintf(Cyc_stderr,({const char*_Tmp7=")]";_tag_fat(_Tmp7,sizeof(char),3U);}),_tag_fat(0U,sizeof(void*),0));
goto _LL0;}}_LL0:;}
# 1166
static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*rval){
Cyc_fprintf(Cyc_stderr,({const char*_Tmp0="    ";_tag_fat(_Tmp0,sizeof(char),5U);}),_tag_fat(0U,sizeof(void*),0));
Cyc_CfFlowInfo_print_root(root);
Cyc_fprintf(Cyc_stderr,({const char*_Tmp0=" --> ";_tag_fat(_Tmp0,sizeof(char),6U);}),_tag_fat(0U,sizeof(void*),0));
Cyc_CfFlowInfo_print_absrval(rval);
Cyc_fprintf(Cyc_stderr,({const char*_Tmp0="\n";_tag_fat(_Tmp0,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));}
# 1174
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){
({(void(*)(void(*)(void*,void*),struct Cyc_Dict_Dict))Cyc_Dict_iter;})(Cyc_CfFlowInfo_print_flow_mapping,d);}
# 1178
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){
union Cyc_CfFlowInfo_FlowInfo _Tmp0=f;void*_Tmp1;struct Cyc_Dict_Dict _Tmp2;if((_Tmp0.BottomFL).tag == 1){
Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="  BottomFL\n";_tag_fat(_Tmp3,sizeof(char),12U);}),_tag_fat(0U,sizeof(void*),0));goto _LL0;}else{_Tmp2=((_Tmp0.ReachableFL).val).f1;_Tmp1=((_Tmp0.ReachableFL).val).f2;{struct Cyc_Dict_Dict fd=_Tmp2;struct Cyc_List_List*rlns=_Tmp1;
# 1182
Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="  ReachableFL:\n";_tag_fat(_Tmp3,sizeof(char),16U);}),_tag_fat(0U,sizeof(void*),0));
Cyc_CfFlowInfo_print_flowdict(fd);
Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="\n  Relations: ";_tag_fat(_Tmp3,sizeof(char),15U);}),_tag_fat(0U,sizeof(void*),0));
Cyc_Relations_print_relns(Cyc_stderr,rlns);
Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="\n";_tag_fat(_Tmp3,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));
goto _LL0;}}_LL0:;}
