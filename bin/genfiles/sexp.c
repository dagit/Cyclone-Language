#include <setjmp.h>
/* This is a C header used by the output of the Cyclone to
   C translator.  Corresponding definitions are in file lib/runtime_*.c */
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
/* should be size_t but int is fine */
#define offsetof(t,n) ((int)(&(((t*)0)->n)))
#endif

/* Fat pointers */
struct _fat_ptr {
  unsigned char *curr; 
  unsigned char *base; 
  unsigned char *last_plus_one; 
};  

/* Regions */
struct _RegionPage
{ 
#ifdef CYC_REGION_PROFILE
  unsigned total_bytes;
  unsigned free_bytes;
#endif
  struct _RegionPage *next;
  char data[1];
};

struct _pool;
struct bget_region_key;
struct _RegionAllocFunctions;

struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
#if(defined(__linux__) && defined(__KERNEL__))
  struct _RegionPage *vpage;
#endif 
  struct _RegionAllocFunctions *fcns;
  char               *offset;
  char               *last_plus_one;
  struct _pool *released_ptrs;
  struct bget_region_key *key;
#ifdef CYC_REGION_PROFILE
  const char *name;
#endif
  unsigned used_bytes;
  unsigned wasted_bytes;
};


// A dynamic region is just a region handle.  The wrapper struct is for type
// abstraction.
struct Cyc_Core_DynamicRegion {
  struct _RegionHandle h;
};

/* Alias qualifier stuff */
typedef unsigned int _AliasQualHandle_t; // must match aqualt_type() in toc.cyc

struct _RegionHandle _new_region(unsigned int, const char*);
void* _region_malloc(struct _RegionHandle*, _AliasQualHandle_t, unsigned);
void* _region_calloc(struct _RegionHandle*, _AliasQualHandle_t, unsigned t, unsigned n);
void* _region_vmalloc(struct _RegionHandle*, unsigned);
void * _aqual_malloc(_AliasQualHandle_t aq, unsigned int s);
void * _aqual_calloc(_AliasQualHandle_t aq, unsigned int n, unsigned int t);
void _free_region(struct _RegionHandle*);

/* Exceptions */
struct _handler_cons {
  struct _RuntimeStack s;
  jmp_buf handler;
};
void _push_handler(struct _handler_cons*);
void _push_region(struct _RegionHandle*);
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
#define _zero_arr_plus_char_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_other_fn(t_sz,orig_x,orig_sz,orig_i,f,l)((orig_x)+(orig_i))
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
void* _zero_arr_plus_other_fn(unsigned,void*,unsigned,int,const char*,unsigned);
#endif

/* _get_zero_arr_size_*(x,sz) returns the number of elements in a
   zero-terminated array that is NULL or has at least sz elements */
unsigned _get_zero_arr_size_char(const char*,unsigned);
unsigned _get_zero_arr_size_other(unsigned,const void*,unsigned);

/* _zero_arr_inplace_plus_*_fn(x,i,filename,lineno) sets
   zero-terminated pointer *x to *x + i */
char* _zero_arr_inplace_plus_char_fn(char**,int,const char*,unsigned);
char* _zero_arr_inplace_plus_post_char_fn(char**,int,const char*,unsigned);
// note: must cast result in toc.cyc
void* _zero_arr_inplace_plus_other_fn(unsigned,void**,int,const char*,unsigned);
void* _zero_arr_inplace_plus_post_other_fn(unsigned,void**,int,const char*,unsigned);
#define _zero_arr_plus_char(x,s,i) \
  (_zero_arr_plus_char_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_inplace_plus_char(x,i) \
  _zero_arr_inplace_plus_char_fn((char**)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_char(x,i) \
  _zero_arr_inplace_plus_post_char_fn((char**)(x),(i),__FILE__,__LINE__)
#define _zero_arr_plus_other(t,x,s,i) \
  (_zero_arr_plus_other_fn(t,x,s,i,__FILE__,__LINE__))
#define _zero_arr_inplace_plus_other(t,x,i) \
  _zero_arr_inplace_plus_other_fn(t,(void**)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_other(t,x,i) \
  _zero_arr_inplace_plus_post_other_fn(t,(void**)(x),(i),__FILE__,__LINE__)

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_fat_subscript(arr,elt_sz,index) ((arr).curr + (elt_sz) * (index))
#define _untag_fat_ptr(arr,elt_sz,num_elts) ((arr).curr)
#define _check_fat_at_base(arr) (arr)
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
      _curr != (unsigned char*)0) \
    _throw_arraybounds(); \
  _curr; })
#define _check_fat_at_base(arr) ({ \
  struct _fat_ptr _arr = (arr); \
  if (_arr.base != _arr.curr) _throw_arraybounds(); \
  _arr; })
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

#ifdef CYC_GC_PTHREAD_REDIRECTS
# define pthread_create GC_pthread_create
# define pthread_sigmask GC_pthread_sigmask
# define pthread_join GC_pthread_join
# define pthread_detach GC_pthread_detach
# define dlopen GC_dlopen
#endif
/* Allocation */
void* GC_malloc(int);
void* GC_malloc_atomic(int);
void* GC_calloc(unsigned,unsigned);
void* GC_calloc_atomic(unsigned,unsigned);

#if(defined(__linux__) && defined(__KERNEL__))
void *cyc_vmalloc(unsigned);
void cyc_vfree(void*);
#endif
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

#define _CYC_MAX_REGION_CONST 0
#define _CYC_MIN_ALIGNMENT (sizeof(double))

#ifdef CYC_REGION_PROFILE
extern int rgn_total_bytes;
#endif

static inline void*_fast_region_malloc(struct _RegionHandle*r, _AliasQualHandle_t aq, unsigned orig_s) {  
  if (r > (struct _RegionHandle*)_CYC_MAX_REGION_CONST && r->curr != 0) { 
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
  return _region_malloc(r,aq,orig_s); 
}

//doesn't make sense to fast malloc with reaps
#ifndef DISABLE_REAPS
#define _fast_region_malloc _region_malloc
#endif

#ifdef CYC_REGION_PROFILE
/* see macros below for usage. defined in runtime_memory.c */
void* _profile_GC_malloc(int,const char*,const char*,int);
void* _profile_GC_malloc_atomic(int,const char*,const char*,int);
void* _profile_GC_calloc(unsigned,unsigned,const char*,const char*,int);
void* _profile_GC_calloc_atomic(unsigned,unsigned,const char*,const char*,int);
void* _profile_region_malloc(struct _RegionHandle*,_AliasQualHandle_t,unsigned,const char*,const char*,int);
void* _profile_region_calloc(struct _RegionHandle*,_AliasQualHandle_t,unsigned,unsigned,const char *,const char*,int);
void * _profile_aqual_malloc(_AliasQualHandle_t aq, unsigned int s,const char *file, const char *func, int lineno);
void * _profile_aqual_calloc(_AliasQualHandle_t aq, unsigned int t1,unsigned int t2,const char *file, const char *func, int lineno);
struct _RegionHandle _profile_new_region(unsigned int i, const char*,const char*,const char*,int);
void _profile_free_region(struct _RegionHandle*,const char*,const char*,int);
#ifndef RUNTIME_CYC
#define _new_region(i,n) _profile_new_region(i,n,__FILE__,__FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__,__FUNCTION__,__LINE__)
#define _region_malloc(rh,aq,n) _profile_region_malloc(rh,aq,n,__FILE__,__FUNCTION__,__LINE__)
#define _region_calloc(rh,aq,n,t) _profile_region_calloc(rh,aq,n,t,__FILE__,__FUNCTION__,__LINE__)
#define _aqual_malloc(aq,n) _profile_aqual_malloc(aq,n,__FILE__,__FUNCTION__,__LINE__)
#define _aqual_calloc(aq,n,t) _profile_aqual_calloc(aq,n,t,__FILE__,__FUNCTION__,__LINE__)
#endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc(n,s) _profile_GC_calloc(n,s,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc_atomic(n,s) _profile_GC_calloc_atomic(n,s,__FILE__,__FUNCTION__,__LINE__)
#endif //CYC_REGION_PROFILE
#endif //_CYC_INCLUDE_H
 struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 79
extern int Cyc_fclose(struct Cyc___cycFILE*);
# 98
extern struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 142 "cycboot.h"
extern int Cyc_getc(struct Cyc___cycFILE*);
# 222 "cycboot.h"
extern int Cyc_ungetc(int,struct Cyc___cycFILE*);extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};
# 318 "core.h"
void Cyc_Core_rethrow(void*);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 59
extern void**Cyc_Hashtable_lookup_other_opt(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*));
# 82
extern int Cyc_Hashtable_hash_string(struct _fat_ptr);struct Cyc_Sexp_Class;struct Cyc_Sexp_Obj;struct Cyc_Sexp_Object;struct Cyc_Sexp_Visitor;struct Cyc_Sexp_Parser{void*env;int(*getc)(void*);int(*ungetc)(int,void*);void(*error)(void*,int,struct _fat_ptr);};struct Cyc_Sexp_Printer{void*env;void(*print)(void*,struct _fat_ptr);};struct Cyc_Sexp_Class{struct Cyc_Sexp_Obj*cast_value;char tag;struct _fat_ptr name;void(*print)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Printer*);struct Cyc_Sexp_Obj*(*parse)(struct Cyc_Sexp_Parser*);int(*cmp)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*);int(*hash)(struct Cyc_Sexp_Obj*);void*(*accept)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Visitor*,void*);struct Cyc_Hashtable_Table*hash_table;};struct Cyc_Sexp_Obj{struct Cyc_Sexp_Class*vtable;void*v[0U] __attribute__((aligned )) ;};struct Cyc_Sexp_Object{struct Cyc_Sexp_Obj*self;};
# 107 "sexp.h"
extern struct Cyc_Sexp_Class Cyc_Sexp_uchar_class;
extern struct Cyc_Sexp_Class Cyc_Sexp_schar_class;
extern struct Cyc_Sexp_Class Cyc_Sexp_ushort_class;
extern struct Cyc_Sexp_Class Cyc_Sexp_sshort_class;
extern struct Cyc_Sexp_Class Cyc_Sexp_uint_class;
extern struct Cyc_Sexp_Class Cyc_Sexp_sint_class;
extern struct Cyc_Sexp_Class Cyc_Sexp_ulonglong_class;
extern struct Cyc_Sexp_Class Cyc_Sexp_slonglong_class;
extern struct Cyc_Sexp_Class Cyc_Sexp_float_class;
extern struct Cyc_Sexp_Class Cyc_Sexp_double_class;
extern struct Cyc_Sexp_Class Cyc_Sexp_str_class;
extern struct Cyc_Sexp_Class Cyc_Sexp_symbol_class;
extern struct Cyc_Sexp_Class Cyc_Sexp_tuple_class;struct _tuple0{struct Cyc_Sexp_Class*vtable;unsigned char v  __attribute__((aligned )) ;};
# 122
struct _tuple0*Cyc_Sexp_mk_uchar(unsigned char);struct _tuple1{struct Cyc_Sexp_Class*vtable;signed char v  __attribute__((aligned )) ;};
struct _tuple1*Cyc_Sexp_mk_schar(signed char);struct _tuple2{struct Cyc_Sexp_Class*vtable;unsigned short v  __attribute__((aligned )) ;};
struct _tuple2*Cyc_Sexp_mk_ushort(unsigned short);struct _tuple3{struct Cyc_Sexp_Class*vtable;short v  __attribute__((aligned )) ;};
struct _tuple3*Cyc_Sexp_mk_sshort(short);struct _tuple4{struct Cyc_Sexp_Class*vtable;unsigned v  __attribute__((aligned )) ;};
struct _tuple4*Cyc_Sexp_mk_uint(unsigned);struct _tuple5{struct Cyc_Sexp_Class*vtable;int v  __attribute__((aligned )) ;};
struct _tuple5*Cyc_Sexp_mk_sint(int);struct _tuple6{struct Cyc_Sexp_Class*vtable;unsigned long long v  __attribute__((aligned )) ;};
struct _tuple6*Cyc_Sexp_mk_ulonglong(unsigned long long);struct _tuple7{struct Cyc_Sexp_Class*vtable;long long v  __attribute__((aligned )) ;};
struct _tuple7*Cyc_Sexp_mk_slonglong(long long);struct _tuple8{struct Cyc_Sexp_Class*vtable;float v  __attribute__((aligned )) ;};
struct _tuple8*Cyc_Sexp_mk_float(float);struct _tuple9{struct Cyc_Sexp_Class*vtable;double v  __attribute__((aligned )) ;};
struct _tuple9*Cyc_Sexp_mk_double(double);struct _tuple10{struct Cyc_Sexp_Class*vtable;struct _fat_ptr v  __attribute__((aligned )) ;};
struct _tuple10*Cyc_Sexp_mk_str(struct _fat_ptr);
struct _tuple10*Cyc_Sexp_mk_symbol(struct _fat_ptr);
struct _tuple10*Cyc_Sexp_mk_tuple(struct _fat_ptr);struct Cyc_Sexp_Visitor{void*(*visit_uchar)(void*,struct _tuple0*,struct Cyc_Sexp_Visitor*);void*(*visit_schar)(void*,struct _tuple1*,struct Cyc_Sexp_Visitor*);void*(*visit_ushort)(void*,struct _tuple2*,struct Cyc_Sexp_Visitor*);void*(*visit_sshort)(void*,struct _tuple3*,struct Cyc_Sexp_Visitor*);void*(*visit_uint)(void*,struct _tuple4*,struct Cyc_Sexp_Visitor*);void*(*visit_sint)(void*,struct _tuple5*,struct Cyc_Sexp_Visitor*);void*(*visit_ulonglong)(void*,struct _tuple6*,struct Cyc_Sexp_Visitor*);void*(*visit_slonglong)(void*,struct _tuple7*,struct Cyc_Sexp_Visitor*);void*(*visit_float)(void*,struct _tuple8*,struct Cyc_Sexp_Visitor*);void*(*visit_double)(void*,struct _tuple9*,struct Cyc_Sexp_Visitor*);void*(*visit_symbol)(void*,struct _tuple10*,struct Cyc_Sexp_Visitor*);void*(*visit_str)(void*,struct _tuple10*,struct Cyc_Sexp_Visitor*);void*(*visit_tuple)(void*,struct _tuple10*,struct Cyc_Sexp_Visitor*);void*(*visit_default)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*);};struct Cyc_Xarray_Xarray{struct _fat_ptr elmts;int num_elmts;};
# 54 "xarray.h"
extern struct Cyc_Xarray_Xarray*Cyc_Xarray_create_empty (void);
# 66
extern void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);
# 104
extern void Cyc_Xarray_iter_c(void(*)(void*,void*),void*,struct Cyc_Xarray_Xarray*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
# 55 "sexp.cyc"
struct Cyc_Sexp_Object Cyc_Sexp_up(struct Cyc_Sexp_Obj*self){
return({struct Cyc_Sexp_Object _Tmp0;_Tmp0.self=self;_Tmp0;});}
# 60
struct Cyc_Sexp_Obj*Cyc_Sexp_down(struct Cyc_Sexp_Class*B,struct Cyc_Sexp_Object x){
void*_Tmp0;_Tmp0=x.self;{struct Cyc_Sexp_Obj*self=_Tmp0;
B->cast_value=0;
self->vtable->cast_value=self;
return B->cast_value;}}
# 68
int Cyc_Sexp_hash(struct Cyc_Sexp_Object self){
void*_Tmp0;_Tmp0=self.self;{struct Cyc_Sexp_Obj*self=_Tmp0;
return self->vtable->hash(self);}}
# 74
int Cyc_Sexp_cmp(struct Cyc_Sexp_Object x,struct Cyc_Sexp_Object y){
void*_Tmp0;_Tmp0=x.self;{struct Cyc_Sexp_Obj*xself=_Tmp0;
void*_Tmp1;_Tmp1=y.self;{struct Cyc_Sexp_Obj*yself=_Tmp1;
if((void*)xself==(void*)yself)return 0;{
# 79
int diff=(int)xself->vtable - (int)yself->vtable;
if(diff!=0)return diff;{
# 82
struct Cyc_Sexp_Obj*yasx=Cyc_Sexp_down(xself->vtable,y);
return xself->vtable->cmp(xself,_check_null(yasx));}}}}}
# 87
void*Cyc_Sexp_visit(struct Cyc_Sexp_Object x,struct Cyc_Sexp_Visitor*v,void*env){
void*_Tmp0;_Tmp0=x.self;{struct Cyc_Sexp_Obj*xself=_Tmp0;
return xself->vtable->accept(xself,v,env);}}
# 93
void Cyc_Sexp_print(struct Cyc_Sexp_Printer*p,struct Cyc_Sexp_Object x){
void*_Tmp0;_Tmp0=x.self;{struct Cyc_Sexp_Obj*xself=_Tmp0;
({void(*_Tmp1)(void*,struct _fat_ptr)=p->print;void*_Tmp2=p->env;_Tmp1(_Tmp2,({struct Cyc_Int_pa_PrintArg_struct _Tmp3=({struct Cyc_Int_pa_PrintArg_struct _Tmp4;_Tmp4.tag=1,_Tmp4.f1=(unsigned long)((int)xself->vtable->tag);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_aprintf(_tag_fat("%c(",sizeof(char),4U),_tag_fat(_Tmp4,sizeof(void*),1));}));});
xself->vtable->print(xself,p);
p->print(p->env,_tag_fat(")",sizeof(char),2U));}}
# 102
static void Cyc_Sexp_printfile(struct Cyc___cycFILE*f,struct _fat_ptr s){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)s;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(f,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp1,sizeof(void*),1));});}
# 107
void Cyc_Sexp_tofile(struct Cyc___cycFILE*f,struct Cyc_Sexp_Object x){
struct Cyc_Sexp_Printer p=({struct Cyc_Sexp_Printer _Tmp0;_Tmp0.env=f,_Tmp0.print=Cyc_Sexp_printfile;_Tmp0;});
Cyc_Sexp_print(& p,x);}
# 113
void Cyc_Sexp_tofilename(const char*filename,struct Cyc_Sexp_Object x){
struct Cyc___cycFILE*fopt=Cyc_fopen(filename,"w");
if(fopt==0)({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2=filename;_tag_fat((void*)_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("unable to open file %s\n",sizeof(char),24U),_tag_fat(_Tmp1,sizeof(void*),1));});
Cyc_Sexp_tofile(_check_null(fopt),x);
Cyc_fclose(fopt);}
# 121
void Cyc_Sexp_printstring(struct Cyc_Xarray_Xarray*strings,struct _fat_ptr s){
# 123
({void(*_Tmp0)(struct Cyc_Xarray_Xarray*,const char*)=(void(*)(struct Cyc_Xarray_Xarray*,const char*))Cyc_Xarray_add;_Tmp0;})(strings,(const char*)_check_null(_untag_fat_ptr(s,sizeof(char),1U)));}
# 127
static void Cyc_Sexp_addlength(int*c,const char*s){
for(1;(int)*s!=0;({const char**_Tmp0=& s;if(*(*_Tmp0)!=0)++(*_Tmp0);else{_throw_arraybounds();}*_Tmp0;})){*c=*c + 1;}}
# 131
static void Cyc_Sexp_addstring(struct _fat_ptr*p,const char*s){
for(1;(int)*s!=0;({const char**_Tmp0=& s;if(*(*_Tmp0)!=0)++(*_Tmp0);else{_throw_arraybounds();}*_Tmp0;})){
({struct _fat_ptr _Tmp0=*p;char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=*s;if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(*p,sizeof(char),1);*p=_Tmp0;});}}
# 139
struct _fat_ptr Cyc_Sexp_tostring(struct Cyc_Sexp_Object v){
struct Cyc_Xarray_Xarray*x=Cyc_Xarray_create_empty();
struct Cyc_Sexp_Printer p=({struct Cyc_Sexp_Printer _Tmp0;_Tmp0.env=x,_Tmp0.print=Cyc_Sexp_printstring;_Tmp0;});
# 143
Cyc_Sexp_print(& p,v);{
int len=1;
({void(*_Tmp0)(void(*)(int*,const char*),int*,struct Cyc_Xarray_Xarray*)=(void(*)(void(*)(int*,const char*),int*,struct Cyc_Xarray_Xarray*))Cyc_Xarray_iter_c;_Tmp0;})(Cyc_Sexp_addlength,& len,x);{
struct _fat_ptr res=({unsigned _Tmp0=len;_tag_fat(_cyccalloc_atomic(sizeof(char),_Tmp0),sizeof(char),_Tmp0);});
struct _fat_ptr p=res;
({void(*_Tmp0)(void(*)(struct _fat_ptr*,const char*),struct _fat_ptr*,struct Cyc_Xarray_Xarray*)=(void(*)(void(*)(struct _fat_ptr*,const char*),struct _fat_ptr*,struct Cyc_Xarray_Xarray*))Cyc_Xarray_iter_c;_Tmp0;})(Cyc_Sexp_addstring,& p,x);
return res;}}}struct Cyc_Sexp_Cls{struct Cyc_Sexp_Class*vtable;};
# 160
extern struct Cyc_List_List*Cyc_Sexp_classes;
# 163
void Cyc_Sexp_register_class(struct Cyc_Sexp_Class*c){
Cyc_Sexp_classes=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Sexp_Cls*_Tmp1=({struct Cyc_Sexp_Cls*_Tmp2=_cycalloc(sizeof(struct Cyc_Sexp_Cls));_Tmp2->vtable=c;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=Cyc_Sexp_classes;_Tmp0;});}
# 169
static int Cyc_Sexp_pgetc(struct Cyc_Sexp_Parser*p){
int res=p->getc(p->env);
return res;}
# 175
static void Cyc_Sexp_pungetc(struct Cyc_Sexp_Parser*p,int ch){
p->ungetc(ch,p->env);}
# 180
static void Cyc_Sexp_perror(struct Cyc_Sexp_Parser*p,int ch,struct _fat_ptr s){
# 182
p->error(p->env,ch,s);}
# 186
static int Cyc_Sexp_whitespace(int ch){
return((ch==32 || ch==9)|| ch==10)|| ch==13;}
# 191
static void Cyc_Sexp_expectws(struct Cyc_Sexp_Parser*p,int expected_ch){
int ch=Cyc_Sexp_pgetc(p);
while(Cyc_Sexp_whitespace(ch)){ch=Cyc_Sexp_pgetc(p);1U;}
if(ch!=expected_ch)({struct Cyc_Sexp_Parser*_Tmp0=p;int _Tmp1=ch;Cyc_Sexp_perror(_Tmp0,_Tmp1,({struct Cyc_Int_pa_PrintArg_struct _Tmp2=({struct Cyc_Int_pa_PrintArg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=(unsigned long)expected_ch;_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(_tag_fat("expected '%c'",sizeof(char),14U),_tag_fat(_Tmp3,sizeof(void*),1));}));});}
# 198
static void Cyc_Sexp_expect(struct Cyc_Sexp_Parser*p,int expected_ch){
int ch=Cyc_Sexp_pgetc(p);
if(ch!=expected_ch)({struct Cyc_Sexp_Parser*_Tmp0=p;int _Tmp1=ch;Cyc_Sexp_perror(_Tmp0,_Tmp1,({struct Cyc_Int_pa_PrintArg_struct _Tmp2=({struct Cyc_Int_pa_PrintArg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=(unsigned long)expected_ch;_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(_tag_fat("expected '%c'",sizeof(char),14U),_tag_fat(_Tmp3,sizeof(void*),1));}));});}
# 206
struct Cyc_Sexp_Object Cyc_Sexp_parse(struct Cyc_Sexp_Parser*p){
int ch=Cyc_Sexp_pgetc(p);
while(Cyc_Sexp_whitespace(ch)){ch=Cyc_Sexp_pgetc(p);1U;}
{struct Cyc_List_List*cs=Cyc_Sexp_classes;for(0;cs!=0;cs=cs->tl){
struct Cyc_Sexp_Cls*_Tmp0=(struct Cyc_Sexp_Cls*)cs->hd;void*_Tmp1;_Tmp1=_Tmp0->vtable;{struct Cyc_Sexp_Class*v=_Tmp1;
if(ch==(int)v->tag){
Cyc_Sexp_expectws(p,40);{
struct Cyc_Sexp_Obj*obj=v->parse(p);
Cyc_Sexp_expectws(p,41);
return Cyc_Sexp_up(obj);}}}}}
# 218
p->error(p->env,ch,_tag_fat("unexpected tag",sizeof(char),15U));}
# 223
static void Cyc_Sexp_file_error(struct Cyc___cycFILE*f,int ch,struct _fat_ptr msg){
# 226
({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,_Tmp1.f1=(unsigned long)ch;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=(struct _fat_ptr)msg;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_fprintf(Cyc_stderr,_tag_fat("found '%c'.  %s\n",sizeof(char),17U),_tag_fat(_Tmp2,sizeof(void*),2));});
Cyc_fclose(f);
_throw((void*)({struct Cyc_Core_Failure_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_Tmp0->tag=Cyc_Core_Failure,_Tmp0->f1=_tag_fat("Sexp::file2object error",sizeof(char),24U);_Tmp0;}));}
# 232
struct Cyc_Sexp_Object Cyc_Sexp_fromfile(struct Cyc___cycFILE*f){
struct Cyc_Sexp_Parser p=({struct Cyc_Sexp_Parser _Tmp0;_Tmp0.env=f,_Tmp0.getc=Cyc_getc,_Tmp0.ungetc=Cyc_ungetc,_Tmp0.error=Cyc_Sexp_file_error;_Tmp0;});
# 235
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{struct Cyc_Sexp_Object s=Cyc_Sexp_parse(& p);
Cyc_fclose(f);{
struct Cyc_Sexp_Object _Tmp2=s;_npop_handler(0);return _Tmp2;}}
# 236
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;_Tmp3=_Tmp2;{void*e=_Tmp3;
# 240
Cyc_Core_rethrow(e);};}}}
# 245
struct Cyc_Sexp_Object Cyc_Sexp_fromfilename(const char*filename){
struct Cyc___cycFILE*fopt=Cyc_fopen(filename,"r");
if(fopt==0)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_Tmp0->tag=Cyc_Core_Failure,_Tmp0->f1=_tag_fat("file not found",sizeof(char),15U);_Tmp0;}));{
struct Cyc_Sexp_Object res=Cyc_Sexp_fromfile(fopt);
Cyc_fclose(fopt);
return res;}}struct Cyc_Sexp_StringEnv{struct _fat_ptr str;unsigned length;unsigned offset;};
# 261
static int Cyc_Sexp_string_getc(struct Cyc_Sexp_StringEnv*env){
if(env->offset >= env->length)return -1;{
int ch=(int)*((const char*)_check_fat_subscript(env->str,sizeof(char),(int)env->offset));
++ env->offset;
return ch;}}
# 269
static int Cyc_Sexp_string_ungetc(int ch,struct Cyc_Sexp_StringEnv*env){
if(ch==-1)return -1;
-- env->offset;
return 0;}
# 276
static void Cyc_Sexp_string_error(struct Cyc_Sexp_StringEnv*env,int ch,struct _fat_ptr msg){
# 279
({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,_Tmp1.f1=(unsigned long)ch;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=(struct _fat_ptr)msg;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_fprintf(Cyc_stderr,_tag_fat("found '%c', %s\n",sizeof(char),16U),_tag_fat(_Tmp2,sizeof(void*),2));});
_throw((void*)({struct Cyc_Core_Failure_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_Tmp0->tag=Cyc_Core_Failure,_Tmp0->f1=_tag_fat("Sexp::string2object error",sizeof(char),26U);_Tmp0;}));}
# 284
struct Cyc_Sexp_Object Cyc_Sexp_fromstring(struct _fat_ptr str){
struct Cyc_Sexp_StringEnv env=({struct Cyc_Sexp_StringEnv _Tmp0;_Tmp0.str=str,({unsigned long _Tmp1=Cyc_strlen((struct _fat_ptr)str);_Tmp0.length=_Tmp1;}),_Tmp0.offset=0U;_Tmp0;});
struct Cyc_Sexp_Parser p=({struct Cyc_Sexp_Parser _Tmp0;_Tmp0.env=& env,_Tmp0.getc=Cyc_Sexp_string_getc,_Tmp0.ungetc=Cyc_Sexp_string_ungetc,_Tmp0.error=Cyc_Sexp_string_error;_Tmp0;});
# 289
return Cyc_Sexp_parse(& p);}
# 296
static int Cyc_Sexp_hash_ulonglong(struct _tuple6*self){return(int)self->v;}
static int Cyc_Sexp_hash_slonglong(struct _tuple7*self){return(int)self->v;}
static int Cyc_Sexp_hash_uint(struct _tuple4*self){return(int)self->v;}
static int Cyc_Sexp_hash_sint(struct _tuple5*self){return self->v;}
static int Cyc_Sexp_hash_ushort(struct _tuple2*self){return(int)self->v;}
static int Cyc_Sexp_hash_sshort(struct _tuple3*self){return(int)self->v;}
static int Cyc_Sexp_hash_uchar(struct _tuple0*self){return(int)self->v;}
static int Cyc_Sexp_hash_schar(struct _tuple1*self){return(int)self->v;}
# 311
static int Cyc_Sexp_cmp_uint(struct _tuple4*x,struct _tuple4*y){return(int)(x->v - y->v);}
static int Cyc_Sexp_cmp_sint(struct _tuple5*x,struct _tuple5*y){return x->v - y->v;}
static int Cyc_Sexp_cmp_ushort(struct _tuple2*x,struct _tuple2*y){return(int)x->v - (int)y->v;}
static int Cyc_Sexp_cmp_sshort(struct _tuple3*x,struct _tuple3*y){return(int)x->v - (int)y->v;}
static int Cyc_Sexp_cmp_uchar(struct _tuple0*x,struct _tuple0*y){return(int)x->v - (int)y->v;}
static int Cyc_Sexp_cmp_schar(struct _tuple1*x,struct _tuple1*y){return(int)x->v - (int)y->v;}
# 331 "sexp.cyc"
static void*Cyc_Sexp_accept_uchar(struct _tuple0*self,struct Cyc_Sexp_Visitor*visitor,void*env){void*(*f)(void*,struct _tuple0*,struct Cyc_Sexp_Visitor*)=visitor->visit_uchar;if(f==0)return({void*(*_Tmp0)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*)=_check_null(visitor->visit_default);void*_Tmp1=env;struct Cyc_Sexp_Object _Tmp2=({struct Cyc_Sexp_Object(*_Tmp3)(struct _tuple0*)=(struct Cyc_Sexp_Object(*)(struct _tuple0*))Cyc_Sexp_up;_Tmp3;})(self);_Tmp0(_Tmp1,_Tmp2,visitor);});else{return f(env,self,visitor);}}
static void*Cyc_Sexp_accept_schar(struct _tuple1*self,struct Cyc_Sexp_Visitor*visitor,void*env){void*(*f)(void*,struct _tuple1*,struct Cyc_Sexp_Visitor*)=visitor->visit_schar;if(f==0)return({void*(*_Tmp0)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*)=_check_null(visitor->visit_default);void*_Tmp1=env;struct Cyc_Sexp_Object _Tmp2=({struct Cyc_Sexp_Object(*_Tmp3)(struct _tuple1*)=(struct Cyc_Sexp_Object(*)(struct _tuple1*))Cyc_Sexp_up;_Tmp3;})(self);_Tmp0(_Tmp1,_Tmp2,visitor);});else{return f(env,self,visitor);}}
static void*Cyc_Sexp_accept_ushort(struct _tuple2*self,struct Cyc_Sexp_Visitor*visitor,void*env){void*(*f)(void*,struct _tuple2*,struct Cyc_Sexp_Visitor*)=visitor->visit_ushort;if(f==0)return({void*(*_Tmp0)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*)=_check_null(visitor->visit_default);void*_Tmp1=env;struct Cyc_Sexp_Object _Tmp2=({struct Cyc_Sexp_Object(*_Tmp3)(struct _tuple2*)=(struct Cyc_Sexp_Object(*)(struct _tuple2*))Cyc_Sexp_up;_Tmp3;})(self);_Tmp0(_Tmp1,_Tmp2,visitor);});else{return f(env,self,visitor);}}
static void*Cyc_Sexp_accept_sshort(struct _tuple3*self,struct Cyc_Sexp_Visitor*visitor,void*env){void*(*f)(void*,struct _tuple3*,struct Cyc_Sexp_Visitor*)=visitor->visit_sshort;if(f==0)return({void*(*_Tmp0)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*)=_check_null(visitor->visit_default);void*_Tmp1=env;struct Cyc_Sexp_Object _Tmp2=({struct Cyc_Sexp_Object(*_Tmp3)(struct _tuple3*)=(struct Cyc_Sexp_Object(*)(struct _tuple3*))Cyc_Sexp_up;_Tmp3;})(self);_Tmp0(_Tmp1,_Tmp2,visitor);});else{return f(env,self,visitor);}}
static void*Cyc_Sexp_accept_uint(struct _tuple4*self,struct Cyc_Sexp_Visitor*visitor,void*env){void*(*f)(void*,struct _tuple4*,struct Cyc_Sexp_Visitor*)=visitor->visit_uint;if(f==0)return({void*(*_Tmp0)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*)=_check_null(visitor->visit_default);void*_Tmp1=env;struct Cyc_Sexp_Object _Tmp2=({struct Cyc_Sexp_Object(*_Tmp3)(struct _tuple4*)=(struct Cyc_Sexp_Object(*)(struct _tuple4*))Cyc_Sexp_up;_Tmp3;})(self);_Tmp0(_Tmp1,_Tmp2,visitor);});else{return f(env,self,visitor);}}
static void*Cyc_Sexp_accept_sint(struct _tuple5*self,struct Cyc_Sexp_Visitor*visitor,void*env){void*(*f)(void*,struct _tuple5*,struct Cyc_Sexp_Visitor*)=visitor->visit_sint;if(f==0)return({void*(*_Tmp0)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*)=_check_null(visitor->visit_default);void*_Tmp1=env;struct Cyc_Sexp_Object _Tmp2=({struct Cyc_Sexp_Object(*_Tmp3)(struct _tuple5*)=(struct Cyc_Sexp_Object(*)(struct _tuple5*))Cyc_Sexp_up;_Tmp3;})(self);_Tmp0(_Tmp1,_Tmp2,visitor);});else{return f(env,self,visitor);}}
static void*Cyc_Sexp_accept_ulonglong(struct _tuple6*self,struct Cyc_Sexp_Visitor*visitor,void*env){void*(*f)(void*,struct _tuple6*,struct Cyc_Sexp_Visitor*)=visitor->visit_ulonglong;if(f==0)return({void*(*_Tmp0)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*)=_check_null(visitor->visit_default);void*_Tmp1=env;struct Cyc_Sexp_Object _Tmp2=({struct Cyc_Sexp_Object(*_Tmp3)(struct _tuple6*)=(struct Cyc_Sexp_Object(*)(struct _tuple6*))Cyc_Sexp_up;_Tmp3;})(self);_Tmp0(_Tmp1,_Tmp2,visitor);});else{return f(env,self,visitor);}}
static void*Cyc_Sexp_accept_slonglong(struct _tuple7*self,struct Cyc_Sexp_Visitor*visitor,void*env){void*(*f)(void*,struct _tuple7*,struct Cyc_Sexp_Visitor*)=visitor->visit_slonglong;if(f==0)return({void*(*_Tmp0)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*)=_check_null(visitor->visit_default);void*_Tmp1=env;struct Cyc_Sexp_Object _Tmp2=({struct Cyc_Sexp_Object(*_Tmp3)(struct _tuple7*)=(struct Cyc_Sexp_Object(*)(struct _tuple7*))Cyc_Sexp_up;_Tmp3;})(self);_Tmp0(_Tmp1,_Tmp2,visitor);});else{return f(env,self,visitor);}}
static void*Cyc_Sexp_accept_float(struct _tuple8*self,struct Cyc_Sexp_Visitor*visitor,void*env){void*(*f)(void*,struct _tuple8*,struct Cyc_Sexp_Visitor*)=visitor->visit_float;if(f==0)return({void*(*_Tmp0)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*)=_check_null(visitor->visit_default);void*_Tmp1=env;struct Cyc_Sexp_Object _Tmp2=({struct Cyc_Sexp_Object(*_Tmp3)(struct _tuple8*)=(struct Cyc_Sexp_Object(*)(struct _tuple8*))Cyc_Sexp_up;_Tmp3;})(self);_Tmp0(_Tmp1,_Tmp2,visitor);});else{return f(env,self,visitor);}}
static void*Cyc_Sexp_accept_double(struct _tuple9*self,struct Cyc_Sexp_Visitor*visitor,void*env){void*(*f)(void*,struct _tuple9*,struct Cyc_Sexp_Visitor*)=visitor->visit_double;if(f==0)return({void*(*_Tmp0)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*)=_check_null(visitor->visit_default);void*_Tmp1=env;struct Cyc_Sexp_Object _Tmp2=({struct Cyc_Sexp_Object(*_Tmp3)(struct _tuple9*)=(struct Cyc_Sexp_Object(*)(struct _tuple9*))Cyc_Sexp_up;_Tmp3;})(self);_Tmp0(_Tmp1,_Tmp2,visitor);});else{return f(env,self,visitor);}}
static void*Cyc_Sexp_accept_str(struct _tuple10*self,struct Cyc_Sexp_Visitor*visitor,void*env){void*(*f)(void*,struct _tuple10*,struct Cyc_Sexp_Visitor*)=visitor->visit_str;if(f==0)return({void*(*_Tmp0)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*)=_check_null(visitor->visit_default);void*_Tmp1=env;struct Cyc_Sexp_Object _Tmp2=({struct Cyc_Sexp_Object(*_Tmp3)(struct _tuple10*)=(struct Cyc_Sexp_Object(*)(struct _tuple10*))Cyc_Sexp_up;_Tmp3;})(self);_Tmp0(_Tmp1,_Tmp2,visitor);});else{return f(env,self,visitor);}}
static void*Cyc_Sexp_accept_symbol(struct _tuple10*self,struct Cyc_Sexp_Visitor*visitor,void*env){void*(*f)(void*,struct _tuple10*,struct Cyc_Sexp_Visitor*)=visitor->visit_symbol;if(f==0)return({void*(*_Tmp0)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*)=_check_null(visitor->visit_default);void*_Tmp1=env;struct Cyc_Sexp_Object _Tmp2=({struct Cyc_Sexp_Object(*_Tmp3)(struct _tuple10*)=(struct Cyc_Sexp_Object(*)(struct _tuple10*))Cyc_Sexp_up;_Tmp3;})(self);_Tmp0(_Tmp1,_Tmp2,visitor);});else{return f(env,self,visitor);}}
static void*Cyc_Sexp_accept_tuple(struct _tuple10*self,struct Cyc_Sexp_Visitor*visitor,void*env){void*(*f)(void*,struct _tuple10*,struct Cyc_Sexp_Visitor*)=visitor->visit_tuple;if(f==0)return({void*(*_Tmp0)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*)=_check_null(visitor->visit_default);void*_Tmp1=env;struct Cyc_Sexp_Object _Tmp2=({struct Cyc_Sexp_Object(*_Tmp3)(struct _tuple10*)=(struct Cyc_Sexp_Object(*)(struct _tuple10*))Cyc_Sexp_up;_Tmp3;})(self);_Tmp0(_Tmp1,_Tmp2,visitor);});else{return f(env,self,visitor);}}
# 348
static unsigned Cyc_Sexp_hex2value(struct Cyc_Sexp_Parser*p,int ch){
if(ch >= 48 && ch <= 57)
return(unsigned)(ch - 48);
if(ch >= 65 && ch <= 70)
return(unsigned)(10 + (ch - 65));
if(ch >= 97 && ch <= 102)
return(unsigned)(10 + (ch - 97));
Cyc_Sexp_perror(p,ch,_tag_fat("expecting hex digit",sizeof(char),20U));}
# 359
static unsigned Cyc_Sexp_nibble(unsigned i,unsigned x){
for(1;i > 0U;-- i){
x=x >> 4U;}
return x & 15U;}
# 366
static unsigned long long Cyc_Sexp_parse_longlong(struct Cyc_Sexp_Parser*p){
unsigned long long res=0U;
{unsigned i=0U;for(0;i < 16U;++ i){
int ch=Cyc_Sexp_pgetc(p);
unsigned long long v=(unsigned long long)Cyc_Sexp_hex2value(p,ch);
res=res << 4U | v;}}
# 373
return res;}
# 377
static void Cyc_Sexp_print_ulonglong(struct _tuple6*self,struct Cyc_Sexp_Printer*p){
# 379
unsigned long long x=self->v;
({void(*_Tmp0)(void*,struct _fat_ptr)=p->print;void*_Tmp1=p->env;_Tmp0(_Tmp1,({struct Cyc_Int_pa_PrintArg_struct _Tmp2=({struct Cyc_Int_pa_PrintArg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=(unsigned)(x >> 32U);_Tmp3;});struct Cyc_Int_pa_PrintArg_struct _Tmp3=({struct Cyc_Int_pa_PrintArg_struct _Tmp4;_Tmp4.tag=1,_Tmp4.f1=(unsigned)x;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_aprintf(_tag_fat("%08x%08x",sizeof(char),9U),_tag_fat(_Tmp4,sizeof(void*),2));}));});}
# 383
static struct _tuple6*Cyc_Sexp_parse_ulonglong(struct Cyc_Sexp_Parser*p){
return Cyc_Sexp_mk_ulonglong(Cyc_Sexp_parse_longlong(p));}
# 387
static void Cyc_Sexp_print_slonglong(struct _tuple7*self,struct Cyc_Sexp_Printer*p){
# 389
long long x=self->v;
({void(*_Tmp0)(void*,struct _fat_ptr)=p->print;void*_Tmp1=p->env;_Tmp0(_Tmp1,({struct Cyc_Int_pa_PrintArg_struct _Tmp2=({struct Cyc_Int_pa_PrintArg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=(unsigned)(x >> 32);_Tmp3;});struct Cyc_Int_pa_PrintArg_struct _Tmp3=({struct Cyc_Int_pa_PrintArg_struct _Tmp4;_Tmp4.tag=1,_Tmp4.f1=(unsigned)x;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_aprintf(_tag_fat("%08x%08x",sizeof(char),9U),_tag_fat(_Tmp4,sizeof(void*),2));}));});}
# 392
static struct _tuple7*Cyc_Sexp_parse_slonglong(struct Cyc_Sexp_Parser*p){
return Cyc_Sexp_mk_slonglong((long long)Cyc_Sexp_parse_longlong(p));}
# 396
static int Cyc_Sexp_cmp_ulonglong(struct _tuple6*x,struct _tuple6*y){
# 398
if(x->v > y->v)return 1;else{
if(x->v < y->v)return -1;else{
return 0;}}}
# 403
static int Cyc_Sexp_cmp_slonglong(struct _tuple7*x,struct _tuple7*y){
# 405
if(x->v > y->v)return 1;else{
if(x->v < y->v)return -1;else{
return 0;}}}
# 411
static unsigned Cyc_Sexp_parse_int(struct Cyc_Sexp_Parser*p){
unsigned res=0U;
{unsigned i=0U;for(0;i < 8U;++ i){
int ch=Cyc_Sexp_pgetc(p);
unsigned v=Cyc_Sexp_hex2value(p,ch);
res=res << 4U | v;}}
# 418
return res;}
# 421
static void Cyc_Sexp_print_uint(struct _tuple4*self,struct Cyc_Sexp_Printer*p){
unsigned x=self->v;
({void(*_Tmp0)(void*,struct _fat_ptr)=p->print;void*_Tmp1=p->env;_Tmp0(_Tmp1,({struct Cyc_Int_pa_PrintArg_struct _Tmp2=({struct Cyc_Int_pa_PrintArg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=x;_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(_tag_fat("%08x",sizeof(char),5U),_tag_fat(_Tmp3,sizeof(void*),1));}));});}
# 426
static struct _tuple4*Cyc_Sexp_parse_uint(struct Cyc_Sexp_Parser*p){
return Cyc_Sexp_mk_uint(Cyc_Sexp_parse_int(p));}
# 430
static void Cyc_Sexp_print_sint(struct _tuple5*self,struct Cyc_Sexp_Printer*p){
int x=self->v;
({void(*_Tmp0)(void*,struct _fat_ptr)=p->print;void*_Tmp1=p->env;_Tmp0(_Tmp1,({struct Cyc_Int_pa_PrintArg_struct _Tmp2=({struct Cyc_Int_pa_PrintArg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=(unsigned)x;_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(_tag_fat("%08x",sizeof(char),5U),_tag_fat(_Tmp3,sizeof(void*),1));}));});}
# 435
static struct _tuple5*Cyc_Sexp_parse_sint(struct Cyc_Sexp_Parser*p){
return Cyc_Sexp_mk_sint((int)Cyc_Sexp_parse_int(p));}
# 440
static unsigned short Cyc_Sexp_parse_short(struct Cyc_Sexp_Parser*p){
unsigned res=0U;
{unsigned i=0U;for(0;i < 4U;++ i){
int ch=Cyc_Sexp_pgetc(p);
unsigned v=Cyc_Sexp_hex2value(p,ch);
res=res << 4U | v;}}
# 447
return(unsigned short)res;}
# 451
static void Cyc_Sexp_print_ushort(struct _tuple2*self,struct Cyc_Sexp_Printer*p){
unsigned short x=self->v;
({void(*_Tmp0)(void*,struct _fat_ptr)=p->print;void*_Tmp1=p->env;_Tmp0(_Tmp1,({struct Cyc_Int_pa_PrintArg_struct _Tmp2=({struct Cyc_Int_pa_PrintArg_struct _Tmp3;_Tmp3.tag=1,({
unsigned _Tmp4=Cyc_Sexp_nibble(3U,(unsigned)x);_Tmp3.f1=_Tmp4;});_Tmp3;});struct Cyc_Int_pa_PrintArg_struct _Tmp3=({struct Cyc_Int_pa_PrintArg_struct _Tmp4;_Tmp4.tag=1,({unsigned _Tmp5=Cyc_Sexp_nibble(2U,(unsigned)x);_Tmp4.f1=_Tmp5;});_Tmp4;});struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,({unsigned _Tmp6=Cyc_Sexp_nibble(1U,(unsigned)x);_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,({unsigned _Tmp7=Cyc_Sexp_nibble(0U,(unsigned)x);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[4];_Tmp6[0]=& _Tmp2,_Tmp6[1]=& _Tmp3,_Tmp6[2]=& _Tmp4,_Tmp6[3]=& _Tmp5;Cyc_aprintf(_tag_fat("%x%x%x%x",sizeof(char),9U),_tag_fat(_Tmp6,sizeof(void*),4));}));});}
# 457
static struct _tuple2*Cyc_Sexp_parse_ushort(struct Cyc_Sexp_Parser*p){
return Cyc_Sexp_mk_ushort(Cyc_Sexp_parse_short(p));}
# 461
static void Cyc_Sexp_print_sshort(struct _tuple3*self,struct Cyc_Sexp_Printer*p){
short x=self->v;
({void(*_Tmp0)(void*,struct _fat_ptr)=p->print;void*_Tmp1=p->env;_Tmp0(_Tmp1,({struct Cyc_Int_pa_PrintArg_struct _Tmp2=({struct Cyc_Int_pa_PrintArg_struct _Tmp3;_Tmp3.tag=1,({
unsigned _Tmp4=Cyc_Sexp_nibble(3U,(unsigned)x);_Tmp3.f1=_Tmp4;});_Tmp3;});struct Cyc_Int_pa_PrintArg_struct _Tmp3=({struct Cyc_Int_pa_PrintArg_struct _Tmp4;_Tmp4.tag=1,({unsigned _Tmp5=Cyc_Sexp_nibble(2U,(unsigned)x);_Tmp4.f1=_Tmp5;});_Tmp4;});struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,({unsigned _Tmp6=Cyc_Sexp_nibble(1U,(unsigned)x);_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,({unsigned _Tmp7=Cyc_Sexp_nibble(0U,(unsigned)x);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[4];_Tmp6[0]=& _Tmp2,_Tmp6[1]=& _Tmp3,_Tmp6[2]=& _Tmp4,_Tmp6[3]=& _Tmp5;Cyc_aprintf(_tag_fat("%x%x%x%x",sizeof(char),9U),_tag_fat(_Tmp6,sizeof(void*),4));}));});}
# 467
static struct _tuple3*Cyc_Sexp_parse_sshort(struct Cyc_Sexp_Parser*p){
return Cyc_Sexp_mk_sshort((short)Cyc_Sexp_parse_short(p));}
# 472
static unsigned char Cyc_Sexp_parse_char(struct Cyc_Sexp_Parser*p){
unsigned res=0U;
{unsigned i=0U;for(0;i < 2U;++ i){
int ch=Cyc_Sexp_pgetc(p);
unsigned v=Cyc_Sexp_hex2value(p,ch);
res=res << 4U | v;}}
# 479
return(unsigned char)res;}
# 482
static void Cyc_Sexp_print_uchar(struct _tuple0*self,struct Cyc_Sexp_Printer*p){
unsigned char x=self->v;
({void(*_Tmp0)(void*,struct _fat_ptr)=p->print;void*_Tmp1=p->env;_Tmp0(_Tmp1,({struct Cyc_Int_pa_PrintArg_struct _Tmp2=({struct Cyc_Int_pa_PrintArg_struct _Tmp3;_Tmp3.tag=1,({unsigned _Tmp4=Cyc_Sexp_nibble(1U,(unsigned)x);_Tmp3.f1=_Tmp4;});_Tmp3;});struct Cyc_Int_pa_PrintArg_struct _Tmp3=({struct Cyc_Int_pa_PrintArg_struct _Tmp4;_Tmp4.tag=1,({unsigned _Tmp5=Cyc_Sexp_nibble(0U,(unsigned)x);_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_aprintf(_tag_fat("%x%x",sizeof(char),5U),_tag_fat(_Tmp4,sizeof(void*),2));}));});}
# 487
static struct _tuple0*Cyc_Sexp_parse_uchar(struct Cyc_Sexp_Parser*p){
return Cyc_Sexp_mk_uchar(Cyc_Sexp_parse_char(p));}
# 491
static void Cyc_Sexp_print_schar(struct _tuple1*self,struct Cyc_Sexp_Printer*p){
signed char x=self->v;
({void(*_Tmp0)(void*,struct _fat_ptr)=p->print;void*_Tmp1=p->env;_Tmp0(_Tmp1,({struct Cyc_Int_pa_PrintArg_struct _Tmp2=({struct Cyc_Int_pa_PrintArg_struct _Tmp3;_Tmp3.tag=1,({unsigned _Tmp4=Cyc_Sexp_nibble(1U,(unsigned)x);_Tmp3.f1=_Tmp4;});_Tmp3;});struct Cyc_Int_pa_PrintArg_struct _Tmp3=({struct Cyc_Int_pa_PrintArg_struct _Tmp4;_Tmp4.tag=1,({unsigned _Tmp5=Cyc_Sexp_nibble(0U,(unsigned)x);_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_aprintf(_tag_fat("%x%x",sizeof(char),5U),_tag_fat(_Tmp4,sizeof(void*),2));}));});}
# 496
static struct _tuple1*Cyc_Sexp_parse_schar(struct Cyc_Sexp_Parser*p){
return Cyc_Sexp_mk_schar((signed char)Cyc_Sexp_parse_char(p));}union Cyc_Sexp_IntFloat{unsigned i;float f;};
# 503
static float Cyc_Sexp_int2float(unsigned i){
union Cyc_Sexp_IntFloat u=({union Cyc_Sexp_IntFloat _Tmp0;_Tmp0.i=i;_Tmp0;});return u.f;}
# 506
static unsigned Cyc_Sexp_float2int(float f){
union Cyc_Sexp_IntFloat u=({union Cyc_Sexp_IntFloat _Tmp0;_Tmp0.f=f;_Tmp0;});return u.i;}union Cyc_Sexp_DoubleLongLong{unsigned long long x;double d;};
# 510
static double Cyc_Sexp_longlong2double(unsigned long long x){
union Cyc_Sexp_DoubleLongLong u=({union Cyc_Sexp_DoubleLongLong _Tmp0;_Tmp0.x=x;_Tmp0;});return u.d;}
# 513
static unsigned long long Cyc_Sexp_double2longlong(double d){
union Cyc_Sexp_DoubleLongLong u=({union Cyc_Sexp_DoubleLongLong _Tmp0;_Tmp0.d=d;_Tmp0;});return u.x;}
# 518
static int Cyc_Sexp_hash_float(struct _tuple8*self){return(int)Cyc_Sexp_float2int(self->v);}
# 520
static void Cyc_Sexp_print_float(struct _tuple8*self,struct Cyc_Sexp_Printer*p){
float x=self->v;
({void(*_Tmp0)(void*,struct _fat_ptr)=p->print;void*_Tmp1=p->env;_Tmp0(_Tmp1,({struct Cyc_Int_pa_PrintArg_struct _Tmp2=({struct Cyc_Int_pa_PrintArg_struct _Tmp3;_Tmp3.tag=1,({unsigned _Tmp4=Cyc_Sexp_float2int(x);_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(_tag_fat("%08x",sizeof(char),5U),_tag_fat(_Tmp3,sizeof(void*),1));}));});}
# 525
static struct _tuple8*Cyc_Sexp_parse_float(struct Cyc_Sexp_Parser*p){
return Cyc_Sexp_mk_float(Cyc_Sexp_int2float(Cyc_Sexp_parse_int(p)));}
# 529
static int Cyc_Sexp_cmp_float(struct _tuple8*x,struct _tuple8*y){
float diff=x->v - y->v;
if((double)diff < 0.0)return -1;else{
if((double)diff > 0.0)return 1;else{
return 0;}}}
# 537
static int Cyc_Sexp_hash_double(struct _tuple9*self){
return(int)((unsigned)Cyc_Sexp_double2longlong(self->v));}
# 541
static void Cyc_Sexp_print_double(struct _tuple9*self,struct Cyc_Sexp_Printer*p){
double d=self->v;
unsigned long long x=Cyc_Sexp_double2longlong(d);
({void(*_Tmp0)(void*,struct _fat_ptr)=p->print;void*_Tmp1=p->env;_Tmp0(_Tmp1,({struct Cyc_Int_pa_PrintArg_struct _Tmp2=({struct Cyc_Int_pa_PrintArg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=(unsigned)(x >> 32U);_Tmp3;});struct Cyc_Int_pa_PrintArg_struct _Tmp3=({struct Cyc_Int_pa_PrintArg_struct _Tmp4;_Tmp4.tag=1,_Tmp4.f1=(unsigned)x;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_aprintf(_tag_fat("%08x%08x",sizeof(char),9U),_tag_fat(_Tmp4,sizeof(void*),2));}));});}
# 548
static struct _tuple9*Cyc_Sexp_parse_double(struct Cyc_Sexp_Parser*p){
return Cyc_Sexp_mk_double(Cyc_Sexp_longlong2double(Cyc_Sexp_parse_longlong(p)));}
# 552
static int Cyc_Sexp_cmp_double(struct _tuple9*x,struct _tuple9*y){
double diff=x->v - y->v;
if(diff < 0.0)return -1;else{
if(diff > 0.0)return 1;else{
return 0;}}}
# 560
static int Cyc_Sexp_hash_str(struct _tuple10*self){
return Cyc_Hashtable_hash_string(self->v);}
# 564
static int Cyc_Sexp_hash_symbol(struct _tuple10*self){
return Cyc_Hashtable_hash_string(self->v);}
# 568
static void Cyc_Sexp_print_symbol(struct _tuple10*self,struct Cyc_Sexp_Printer*p){
# 570
p->print(p->env,self->v);}
# 576
static struct _tuple10*Cyc_Sexp_parse_symbol(struct Cyc_Sexp_Parser*p){
struct Cyc_List_List*chars=0;
int ch=Cyc_Sexp_pgetc(p);
while(Cyc_Sexp_whitespace(ch)){ch=Cyc_Sexp_pgetc(p);1U;}
while(1){
if(ch==-1)Cyc_Sexp_perror(p,ch,_tag_fat("unexpected end of file",sizeof(char),23U));
if(Cyc_Sexp_whitespace(ch)|| ch==41){
Cyc_Sexp_pungetc(p,ch);
break;}
# 586
chars=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=(void*)ch,_Tmp0->tl=chars;_Tmp0;});
ch=Cyc_Sexp_pgetc(p);
# 581
1U;}
# 589
chars=Cyc_List_imp_rev(chars);{
struct _fat_ptr buf=({unsigned _Tmp0=Cyc_List_length(chars)+ 1;_tag_fat(_cyccalloc_atomic(sizeof(char),_Tmp0),sizeof(char),_Tmp0);});
{unsigned i=0U;for(0;chars!=0;(chars=chars->tl,++ i)){
({struct _fat_ptr _Tmp0=_fat_ptr_plus(buf,sizeof(char),(int)i);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=(char)((int)chars->hd);if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}}
return Cyc_Sexp_mk_symbol(buf);}}
# 597
static struct _fat_ptr Cyc_Sexp_escape(struct _fat_ptr s){
unsigned n=Cyc_strlen(s);
int escapes=0;
{unsigned i=0U;for(0;i < n;++ i){
if((int)((const char*)s.curr)[(int)i]==34 ||(int)((const char*)s.curr)[(int)i]==92)++ escapes;}}
# 603
if(escapes==0)return s;{
struct _fat_ptr news=({unsigned _Tmp0=((n + (unsigned)escapes)+ 1U)+ 1U;_tag_fat(({char*_Tmp1=_cycalloc_atomic(_check_times(_Tmp0,sizeof(char)));({{unsigned _Tmp2=(n + (unsigned)escapes)+ 1U;unsigned i;for(i=0;i < _Tmp2;++ i){_Tmp1[i]='\000';}_Tmp1[_Tmp2]=0;}0;});_Tmp1;}),sizeof(char),_Tmp0);});
unsigned pos=0U;
{unsigned i=0U;for(0;i < n;++ i){
char ch=((const char*)s.curr)[(int)i];
if((int)((const char*)s.curr)[(int)i]==34 ||(int)((const char*)s.curr)[(int)i]==92){
({struct _fat_ptr _Tmp0=_fat_ptr_plus(news,sizeof(char),(int)pos);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='\\';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
++ pos;}
# 612
({struct _fat_ptr _Tmp0=_fat_ptr_plus(news,sizeof(char),(int)pos);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=ch;if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
++ pos;}}
# 615
return news;}}
# 619
static void Cyc_Sexp_print_str(struct _tuple10*self,struct Cyc_Sexp_Printer*p){
p->print(p->env,_tag_fat("\"",sizeof(char),2U));
({void(*_Tmp0)(void*,struct _fat_ptr)=p->print;void*_Tmp1=p->env;_Tmp0(_Tmp1,Cyc_Sexp_escape(self->v));});
p->print(p->env,_tag_fat("\"",sizeof(char),2U));}
# 626
static int Cyc_Sexp_cmp_str(struct _tuple10*x,struct _tuple10*y){
return Cyc_strcmp(x->v,y->v);}
# 630
static int Cyc_Sexp_cmp_symbol(struct _tuple10*x,struct _tuple10*y){
return Cyc_strcmp(x->v,y->v);}
# 637
static struct _tuple10*Cyc_Sexp_parse_str(struct Cyc_Sexp_Parser*p){
Cyc_Sexp_expectws(p,34);{
struct Cyc_List_List*chars=0;
while(1){
{int ch=Cyc_Sexp_pgetc(p);
if(ch==-1)Cyc_Sexp_perror(p,ch,_tag_fat("unexpected end of file",sizeof(char),23U));
if(ch==34)break;
if(ch==92){
ch=Cyc_Sexp_pgetc(p);
if(ch!=34 && ch!=92)Cyc_Sexp_perror(p,ch,_tag_fat("expected '\"' or '\\'",sizeof(char),20U));}
# 648
chars=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=(void*)ch,_Tmp0->tl=chars;_Tmp0;});}
# 641
1U;}
# 650
chars=Cyc_List_imp_rev(chars);{
struct _fat_ptr buf=({unsigned _Tmp0=Cyc_List_length(chars)+ 1;_tag_fat(_cyccalloc_atomic(sizeof(char),_Tmp0),sizeof(char),_Tmp0);});
{unsigned i=0U;for(0;chars!=0;(chars=chars->tl,++ i)){
({struct _fat_ptr _Tmp0=_fat_ptr_plus(buf,sizeof(char),(int)i);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=(char)((int)chars->hd);if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}}
return Cyc_Sexp_mk_str(buf);}}}
# 658
static void Cyc_Sexp_print_tuple(struct _tuple10*self,struct Cyc_Sexp_Printer*p){
# 660
struct _fat_ptr vs=self->v;
unsigned n=_get_fat_size(vs,sizeof(struct Cyc_Sexp_Object));
unsigned i=0U;for(0;i < n;++ i){
Cyc_Sexp_print(p,*((struct Cyc_Sexp_Object*)_check_fat_subscript(vs,sizeof(struct Cyc_Sexp_Object),(int)i)));}}
# 669
static int Cyc_Sexp_hash_tuple(struct _tuple10*x){
unsigned res=0U;
struct _fat_ptr vs=x->v;
unsigned n=_get_fat_size(vs,sizeof(struct Cyc_Sexp_Object));
{unsigned i=0U;for(0;i < n;++ i){
struct Cyc_Sexp_Object _Tmp0=((struct Cyc_Sexp_Object*)vs.curr)[(int)i];void*_Tmp1;_Tmp1=_Tmp0.self;{struct Cyc_Sexp_Obj*v=_Tmp1;
res=res << 8U | (unsigned)v & 255U;}}}
# 677
return(int)res;}
# 681
static int Cyc_Sexp_cmp_tuple(struct _tuple10*x,struct _tuple10*y){
if(x==y)return 0;{
struct _fat_ptr xs=x->v;
struct _fat_ptr ys=y->v;
unsigned nx=_get_fat_size(xs,sizeof(struct Cyc_Sexp_Object));
unsigned ny=_get_fat_size(ys,sizeof(struct Cyc_Sexp_Object));
if(nx < ny)return -1;
if(nx > ny)return 1;
{unsigned i=0U;for(0;i < nx;++ i){
int c=({struct Cyc_Sexp_Object _Tmp0=*((struct Cyc_Sexp_Object*)_check_fat_subscript(xs,sizeof(struct Cyc_Sexp_Object),(int)i));Cyc_Sexp_cmp(_Tmp0,*((struct Cyc_Sexp_Object*)_check_fat_subscript(ys,sizeof(struct Cyc_Sexp_Object),(int)i)));});
if(c!=0)return c;}}
# 693
return 0;}}
# 697
static struct Cyc_Sexp_Object Cyc_Sexp_next_list(struct Cyc_List_List**xs){
struct Cyc_Sexp_Object*h=(struct Cyc_Sexp_Object*)_check_null(*xs)->hd;
*xs=(*xs)->tl;
return*h;}
# 704
static struct _tuple10*Cyc_Sexp_parse_tuple(struct Cyc_Sexp_Parser*p){
struct Cyc_List_List*xs=0;
Next: {
int ch=Cyc_Sexp_pgetc(p);
while(Cyc_Sexp_whitespace(ch)){ch=Cyc_Sexp_pgetc(p);1U;}
if(ch==41){
Cyc_Sexp_pungetc(p,ch);
xs=Cyc_List_imp_rev(xs);{
int len=Cyc_List_length(xs);
struct _fat_ptr vs=({unsigned _Tmp0=(unsigned)len;_tag_fat(({struct Cyc_Sexp_Object*_Tmp1=_cycalloc(_check_times(_Tmp0,sizeof(struct Cyc_Sexp_Object)));({{unsigned _Tmp2=(unsigned)len;unsigned i;for(i=0;i < _Tmp2;++ i){({struct Cyc_Sexp_Object _Tmp3=Cyc_Sexp_next_list(& xs);_Tmp1[i]=_Tmp3;});}}0;});_Tmp1;}),sizeof(struct Cyc_Sexp_Object),_Tmp0);});
return Cyc_Sexp_mk_tuple(vs);}}
# 716
{struct Cyc_List_List*cs=Cyc_Sexp_classes;for(0;cs!=0;cs=cs->tl){
struct Cyc_Sexp_Cls*_Tmp0=(struct Cyc_Sexp_Cls*)cs->hd;void*_Tmp1;_Tmp1=_Tmp0->vtable;{struct Cyc_Sexp_Class*v=_Tmp1;
if(ch==(int)v->tag){
Cyc_Sexp_expectws(p,40);{
struct Cyc_Sexp_Obj*obj=v->parse(p);
Cyc_Sexp_expectws(p,41);
xs=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Sexp_Object*_Tmp3=({struct Cyc_Sexp_Object*_Tmp4=_cycalloc(sizeof(struct Cyc_Sexp_Object));({struct Cyc_Sexp_Object _Tmp5=Cyc_Sexp_up(obj);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=xs;_Tmp2;});
goto Next;}}}}}
# 726
p->error(p->env,ch,_tag_fat("unexpected tag",sizeof(char),15U));}}static char _TmpG0[14U]="unsigned char";
# 743 "sexp.cyc"
struct Cyc_Sexp_Class Cyc_Sexp_uchar_class={(struct Cyc_Sexp_Obj*)0,'C',{_TmpG0,_TmpG0,_TmpG0 + 14U},(void(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Printer*))Cyc_Sexp_print_uchar,(struct Cyc_Sexp_Obj*(*)(struct Cyc_Sexp_Parser*))Cyc_Sexp_parse_uchar,(int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*))Cyc_Sexp_cmp_uchar,(int(*)(struct Cyc_Sexp_Obj*))Cyc_Sexp_hash_uchar,(void*(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Visitor*,void*))Cyc_Sexp_accept_uchar,0};static char _TmpG1[12U]="signed char";
struct Cyc_Sexp_Class Cyc_Sexp_schar_class={(struct Cyc_Sexp_Obj*)0,'c',{_TmpG1,_TmpG1,_TmpG1 + 12U},(void(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Printer*))Cyc_Sexp_print_schar,(struct Cyc_Sexp_Obj*(*)(struct Cyc_Sexp_Parser*))Cyc_Sexp_parse_schar,(int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*))Cyc_Sexp_cmp_schar,(int(*)(struct Cyc_Sexp_Obj*))Cyc_Sexp_hash_schar,(void*(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Visitor*,void*))Cyc_Sexp_accept_schar,0};static char _TmpG2[15U]="unsigned short";
struct Cyc_Sexp_Class Cyc_Sexp_ushort_class={(struct Cyc_Sexp_Obj*)0,'S',{_TmpG2,_TmpG2,_TmpG2 + 15U},(void(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Printer*))Cyc_Sexp_print_ushort,(struct Cyc_Sexp_Obj*(*)(struct Cyc_Sexp_Parser*))Cyc_Sexp_parse_ushort,(int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*))Cyc_Sexp_cmp_ushort,(int(*)(struct Cyc_Sexp_Obj*))Cyc_Sexp_hash_ushort,(void*(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Visitor*,void*))Cyc_Sexp_accept_ushort,0};static char _TmpG3[13U]="signed short";
struct Cyc_Sexp_Class Cyc_Sexp_sshort_class={(struct Cyc_Sexp_Obj*)0,'s',{_TmpG3,_TmpG3,_TmpG3 + 13U},(void(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Printer*))Cyc_Sexp_print_sshort,(struct Cyc_Sexp_Obj*(*)(struct Cyc_Sexp_Parser*))Cyc_Sexp_parse_sshort,(int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*))Cyc_Sexp_cmp_sshort,(int(*)(struct Cyc_Sexp_Obj*))Cyc_Sexp_hash_sshort,(void*(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Visitor*,void*))Cyc_Sexp_accept_sshort,0};static char _TmpG4[13U]="unsigned int";
struct Cyc_Sexp_Class Cyc_Sexp_uint_class={(struct Cyc_Sexp_Obj*)0,'I',{_TmpG4,_TmpG4,_TmpG4 + 13U},(void(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Printer*))Cyc_Sexp_print_uint,(struct Cyc_Sexp_Obj*(*)(struct Cyc_Sexp_Parser*))Cyc_Sexp_parse_uint,(int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*))Cyc_Sexp_cmp_uint,(int(*)(struct Cyc_Sexp_Obj*))Cyc_Sexp_hash_uint,(void*(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Visitor*,void*))Cyc_Sexp_accept_uint,0};static char _TmpG5[11U]="signed int";
struct Cyc_Sexp_Class Cyc_Sexp_sint_class={(struct Cyc_Sexp_Obj*)0,'i',{_TmpG5,_TmpG5,_TmpG5 + 11U},(void(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Printer*))Cyc_Sexp_print_sint,(struct Cyc_Sexp_Obj*(*)(struct Cyc_Sexp_Parser*))Cyc_Sexp_parse_sint,(int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*))Cyc_Sexp_cmp_sint,(int(*)(struct Cyc_Sexp_Obj*))Cyc_Sexp_hash_sint,(void*(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Visitor*,void*))Cyc_Sexp_accept_sint,0};static char _TmpG6[19U]="unsigned long long";
struct Cyc_Sexp_Class Cyc_Sexp_ulonglong_class={(struct Cyc_Sexp_Obj*)0,'L',{_TmpG6,_TmpG6,_TmpG6 + 19U},(void(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Printer*))Cyc_Sexp_print_ulonglong,(struct Cyc_Sexp_Obj*(*)(struct Cyc_Sexp_Parser*))Cyc_Sexp_parse_ulonglong,(int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*))Cyc_Sexp_cmp_ulonglong,(int(*)(struct Cyc_Sexp_Obj*))Cyc_Sexp_hash_ulonglong,(void*(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Visitor*,void*))Cyc_Sexp_accept_ulonglong,0};static char _TmpG7[17U]="signed long long";
struct Cyc_Sexp_Class Cyc_Sexp_slonglong_class={(struct Cyc_Sexp_Obj*)0,'l',{_TmpG7,_TmpG7,_TmpG7 + 17U},(void(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Printer*))Cyc_Sexp_print_slonglong,(struct Cyc_Sexp_Obj*(*)(struct Cyc_Sexp_Parser*))Cyc_Sexp_parse_slonglong,(int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*))Cyc_Sexp_cmp_slonglong,(int(*)(struct Cyc_Sexp_Obj*))Cyc_Sexp_hash_slonglong,(void*(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Visitor*,void*))Cyc_Sexp_accept_slonglong,0};static char _TmpG8[6U]="float";
struct Cyc_Sexp_Class Cyc_Sexp_float_class={(struct Cyc_Sexp_Obj*)0,'f',{_TmpG8,_TmpG8,_TmpG8 + 6U},(void(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Printer*))Cyc_Sexp_print_float,(struct Cyc_Sexp_Obj*(*)(struct Cyc_Sexp_Parser*))Cyc_Sexp_parse_float,(int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*))Cyc_Sexp_cmp_float,(int(*)(struct Cyc_Sexp_Obj*))Cyc_Sexp_hash_float,(void*(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Visitor*,void*))Cyc_Sexp_accept_float,0};static char _TmpG9[7U]="double";
struct Cyc_Sexp_Class Cyc_Sexp_double_class={(struct Cyc_Sexp_Obj*)0,'d',{_TmpG9,_TmpG9,_TmpG9 + 7U},(void(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Printer*))Cyc_Sexp_print_double,(struct Cyc_Sexp_Obj*(*)(struct Cyc_Sexp_Parser*))Cyc_Sexp_parse_double,(int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*))Cyc_Sexp_cmp_double,(int(*)(struct Cyc_Sexp_Obj*))Cyc_Sexp_hash_double,(void*(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Visitor*,void*))Cyc_Sexp_accept_double,0};static char _TmpGA[7U]="string";
struct Cyc_Sexp_Class Cyc_Sexp_str_class={(struct Cyc_Sexp_Obj*)0,'r',{_TmpGA,_TmpGA,_TmpGA + 7U},(void(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Printer*))Cyc_Sexp_print_str,(struct Cyc_Sexp_Obj*(*)(struct Cyc_Sexp_Parser*))Cyc_Sexp_parse_str,(int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*))Cyc_Sexp_cmp_str,(int(*)(struct Cyc_Sexp_Obj*))Cyc_Sexp_hash_str,(void*(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Visitor*,void*))Cyc_Sexp_accept_str,0};static char _TmpGB[7U]="symbol";
struct Cyc_Sexp_Class Cyc_Sexp_symbol_class={(struct Cyc_Sexp_Obj*)0,'y',{_TmpGB,_TmpGB,_TmpGB + 7U},(void(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Printer*))Cyc_Sexp_print_symbol,(struct Cyc_Sexp_Obj*(*)(struct Cyc_Sexp_Parser*))Cyc_Sexp_parse_symbol,(int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*))Cyc_Sexp_cmp_symbol,(int(*)(struct Cyc_Sexp_Obj*))Cyc_Sexp_hash_symbol,(void*(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Visitor*,void*))Cyc_Sexp_accept_symbol,0};static char _TmpGC[6U]="tuple";
struct Cyc_Sexp_Class Cyc_Sexp_tuple_class={(struct Cyc_Sexp_Obj*)0,'t',{_TmpGC,_TmpGC,_TmpGC + 6U},(void(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Printer*))Cyc_Sexp_print_tuple,(struct Cyc_Sexp_Obj*(*)(struct Cyc_Sexp_Parser*))Cyc_Sexp_parse_tuple,(int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*))Cyc_Sexp_cmp_tuple,(int(*)(struct Cyc_Sexp_Obj*))Cyc_Sexp_hash_tuple,(void*(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Visitor*,void*))Cyc_Sexp_accept_tuple,0};
# 763
static struct Cyc_Sexp_Obj**Cyc_Sexp_hashcons(struct Cyc_Sexp_Obj*x){
struct Cyc_Sexp_Class*c=x->vtable;
struct Cyc_Hashtable_Table*topt=c->hash_table;
struct Cyc_Hashtable_Table*t;
if(topt==0){
t=({struct Cyc_Hashtable_Table*(*_Tmp0)(int,int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*),int(*)(struct Cyc_Sexp_Obj*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*),int(*)(struct Cyc_Sexp_Obj*)))Cyc_Hashtable_create;_Tmp0;})(101,c->cmp,c->hash);
c->hash_table=t;}else{
# 771
t=topt;}
# 773
return({struct Cyc_Sexp_Obj**(*_Tmp0)(struct Cyc_Hashtable_Table*,struct Cyc_Sexp_Obj*,int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*),int(*)(struct Cyc_Sexp_Obj*))=(struct Cyc_Sexp_Obj**(*)(struct Cyc_Hashtable_Table*,struct Cyc_Sexp_Obj*,int(*)(struct Cyc_Sexp_Obj*,struct Cyc_Sexp_Obj*),int(*)(struct Cyc_Sexp_Obj*)))Cyc_Hashtable_lookup_other_opt;_Tmp0;})(t,x,c->cmp,c->hash);}
# 778
struct _tuple0*Cyc_Sexp_mk_uchar(unsigned char v){
struct _tuple0 obj=({struct _tuple0 _Tmp0;_Tmp0.vtable=& Cyc_Sexp_uchar_class,_Tmp0.v=v;_Tmp0;});
struct _tuple0**objopt=({struct _tuple0**(*_Tmp0)(struct _tuple0*)=(struct _tuple0**(*)(struct _tuple0*))Cyc_Sexp_hashcons;_Tmp0;})(& obj);
if(objopt!=0)return*objopt;{
struct _tuple0*objp;objp=_cycalloc(sizeof(struct _tuple0)),*objp=obj;
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,struct _tuple0*,struct _tuple0*)=(void(*)(struct Cyc_Hashtable_Table*,struct _tuple0*,struct _tuple0*))Cyc_Hashtable_insert;_Tmp0;})(_check_null(obj.vtable->hash_table),objp,objp);
return objp;}}
# 786
struct Cyc_Sexp_Object Cyc_Sexp_obj_uchar(unsigned char v){return({struct Cyc_Sexp_Object(*_Tmp0)(struct _tuple0*)=({struct Cyc_Sexp_Object(*_Tmp1)(struct _tuple0*)=(struct Cyc_Sexp_Object(*)(struct _tuple0*))Cyc_Sexp_up;_Tmp1;});_Tmp0(Cyc_Sexp_mk_uchar(v));});}
struct Cyc_Sexp_Cls Cyc_Sexp_uchar_class_w={& Cyc_Sexp_uchar_class};
# 802 "sexp.cyc"
struct _tuple1*Cyc_Sexp_mk_schar(signed char v){struct _tuple1 obj=({struct _tuple1 _Tmp0;_Tmp0.vtable=& Cyc_Sexp_schar_class,_Tmp0.v=v;_Tmp0;});struct _tuple1**objopt=({struct _tuple1**(*_Tmp0)(struct _tuple1*)=(struct _tuple1**(*)(struct _tuple1*))Cyc_Sexp_hashcons;_Tmp0;})(& obj);if(objopt!=0)return*objopt;{struct _tuple1*objp;objp=_cycalloc(sizeof(struct _tuple1)),*objp=obj;({void(*_Tmp0)(struct Cyc_Hashtable_Table*,struct _tuple1*,struct _tuple1*)=(void(*)(struct Cyc_Hashtable_Table*,struct _tuple1*,struct _tuple1*))Cyc_Hashtable_insert;_Tmp0;})(_check_null(obj.vtable->hash_table),objp,objp);return objp;}}struct Cyc_Sexp_Object Cyc_Sexp_obj_schar(signed char v){return({struct Cyc_Sexp_Object(*_Tmp0)(struct _tuple1*)=({struct Cyc_Sexp_Object(*_Tmp1)(struct _tuple1*)=(struct Cyc_Sexp_Object(*)(struct _tuple1*))Cyc_Sexp_up;_Tmp1;});_Tmp0(Cyc_Sexp_mk_schar(v));});}struct Cyc_Sexp_Cls Cyc_Sexp_schar_class_w={& Cyc_Sexp_schar_class};
struct _tuple2*Cyc_Sexp_mk_ushort(unsigned short v){struct _tuple2 obj=({struct _tuple2 _Tmp0;_Tmp0.vtable=& Cyc_Sexp_ushort_class,_Tmp0.v=v;_Tmp0;});struct _tuple2**objopt=({struct _tuple2**(*_Tmp0)(struct _tuple2*)=(struct _tuple2**(*)(struct _tuple2*))Cyc_Sexp_hashcons;_Tmp0;})(& obj);if(objopt!=0)return*objopt;{struct _tuple2*objp;objp=_cycalloc(sizeof(struct _tuple2)),*objp=obj;({void(*_Tmp0)(struct Cyc_Hashtable_Table*,struct _tuple2*,struct _tuple2*)=(void(*)(struct Cyc_Hashtable_Table*,struct _tuple2*,struct _tuple2*))Cyc_Hashtable_insert;_Tmp0;})(_check_null(obj.vtable->hash_table),objp,objp);return objp;}}struct Cyc_Sexp_Object Cyc_Sexp_obj_ushort(unsigned short v){return({struct Cyc_Sexp_Object(*_Tmp0)(struct _tuple2*)=({struct Cyc_Sexp_Object(*_Tmp1)(struct _tuple2*)=(struct Cyc_Sexp_Object(*)(struct _tuple2*))Cyc_Sexp_up;_Tmp1;});_Tmp0(Cyc_Sexp_mk_ushort(v));});}struct Cyc_Sexp_Cls Cyc_Sexp_ushort_class_w={& Cyc_Sexp_ushort_class};
struct _tuple3*Cyc_Sexp_mk_sshort(short v){struct _tuple3 obj=({struct _tuple3 _Tmp0;_Tmp0.vtable=& Cyc_Sexp_sshort_class,_Tmp0.v=v;_Tmp0;});struct _tuple3**objopt=({struct _tuple3**(*_Tmp0)(struct _tuple3*)=(struct _tuple3**(*)(struct _tuple3*))Cyc_Sexp_hashcons;_Tmp0;})(& obj);if(objopt!=0)return*objopt;{struct _tuple3*objp;objp=_cycalloc(sizeof(struct _tuple3)),*objp=obj;({void(*_Tmp0)(struct Cyc_Hashtable_Table*,struct _tuple3*,struct _tuple3*)=(void(*)(struct Cyc_Hashtable_Table*,struct _tuple3*,struct _tuple3*))Cyc_Hashtable_insert;_Tmp0;})(_check_null(obj.vtable->hash_table),objp,objp);return objp;}}struct Cyc_Sexp_Object Cyc_Sexp_obj_sshort(short v){return({struct Cyc_Sexp_Object(*_Tmp0)(struct _tuple3*)=({struct Cyc_Sexp_Object(*_Tmp1)(struct _tuple3*)=(struct Cyc_Sexp_Object(*)(struct _tuple3*))Cyc_Sexp_up;_Tmp1;});_Tmp0(Cyc_Sexp_mk_sshort(v));});}struct Cyc_Sexp_Cls Cyc_Sexp_sshort_class_w={& Cyc_Sexp_sshort_class};
struct _tuple4*Cyc_Sexp_mk_uint(unsigned v){struct _tuple4 obj=({struct _tuple4 _Tmp0;_Tmp0.vtable=& Cyc_Sexp_uint_class,_Tmp0.v=v;_Tmp0;});struct _tuple4**objopt=({struct _tuple4**(*_Tmp0)(struct _tuple4*)=(struct _tuple4**(*)(struct _tuple4*))Cyc_Sexp_hashcons;_Tmp0;})(& obj);if(objopt!=0)return*objopt;{struct _tuple4*objp;objp=_cycalloc(sizeof(struct _tuple4)),*objp=obj;({void(*_Tmp0)(struct Cyc_Hashtable_Table*,struct _tuple4*,struct _tuple4*)=(void(*)(struct Cyc_Hashtable_Table*,struct _tuple4*,struct _tuple4*))Cyc_Hashtable_insert;_Tmp0;})(_check_null(obj.vtable->hash_table),objp,objp);return objp;}}struct Cyc_Sexp_Object Cyc_Sexp_obj_uint(unsigned v){return({struct Cyc_Sexp_Object(*_Tmp0)(struct _tuple4*)=({struct Cyc_Sexp_Object(*_Tmp1)(struct _tuple4*)=(struct Cyc_Sexp_Object(*)(struct _tuple4*))Cyc_Sexp_up;_Tmp1;});_Tmp0(Cyc_Sexp_mk_uint(v));});}struct Cyc_Sexp_Cls Cyc_Sexp_uint_class_w={& Cyc_Sexp_uint_class};
struct _tuple5*Cyc_Sexp_mk_sint(int v){struct _tuple5 obj=({struct _tuple5 _Tmp0;_Tmp0.vtable=& Cyc_Sexp_sint_class,_Tmp0.v=v;_Tmp0;});struct _tuple5**objopt=({struct _tuple5**(*_Tmp0)(struct _tuple5*)=(struct _tuple5**(*)(struct _tuple5*))Cyc_Sexp_hashcons;_Tmp0;})(& obj);if(objopt!=0)return*objopt;{struct _tuple5*objp;objp=_cycalloc(sizeof(struct _tuple5)),*objp=obj;({void(*_Tmp0)(struct Cyc_Hashtable_Table*,struct _tuple5*,struct _tuple5*)=(void(*)(struct Cyc_Hashtable_Table*,struct _tuple5*,struct _tuple5*))Cyc_Hashtable_insert;_Tmp0;})(_check_null(obj.vtable->hash_table),objp,objp);return objp;}}struct Cyc_Sexp_Object Cyc_Sexp_obj_sint(int v){return({struct Cyc_Sexp_Object(*_Tmp0)(struct _tuple5*)=({struct Cyc_Sexp_Object(*_Tmp1)(struct _tuple5*)=(struct Cyc_Sexp_Object(*)(struct _tuple5*))Cyc_Sexp_up;_Tmp1;});_Tmp0(Cyc_Sexp_mk_sint(v));});}struct Cyc_Sexp_Cls Cyc_Sexp_sint_class_w={& Cyc_Sexp_sint_class};
struct _tuple6*Cyc_Sexp_mk_ulonglong(unsigned long long v){struct _tuple6 obj=({struct _tuple6 _Tmp0;_Tmp0.vtable=& Cyc_Sexp_ulonglong_class,_Tmp0.v=v;_Tmp0;});struct _tuple6**objopt=({struct _tuple6**(*_Tmp0)(struct _tuple6*)=(struct _tuple6**(*)(struct _tuple6*))Cyc_Sexp_hashcons;_Tmp0;})(& obj);if(objopt!=0)return*objopt;{struct _tuple6*objp;objp=_cycalloc(sizeof(struct _tuple6)),*objp=obj;({void(*_Tmp0)(struct Cyc_Hashtable_Table*,struct _tuple6*,struct _tuple6*)=(void(*)(struct Cyc_Hashtable_Table*,struct _tuple6*,struct _tuple6*))Cyc_Hashtable_insert;_Tmp0;})(_check_null(obj.vtable->hash_table),objp,objp);return objp;}}struct Cyc_Sexp_Object Cyc_Sexp_obj_ulonglong(unsigned long long v){return({struct Cyc_Sexp_Object(*_Tmp0)(struct _tuple6*)=({struct Cyc_Sexp_Object(*_Tmp1)(struct _tuple6*)=(struct Cyc_Sexp_Object(*)(struct _tuple6*))Cyc_Sexp_up;_Tmp1;});_Tmp0(Cyc_Sexp_mk_ulonglong(v));});}struct Cyc_Sexp_Cls Cyc_Sexp_ulonglong_class_w={& Cyc_Sexp_ulonglong_class};
struct _tuple7*Cyc_Sexp_mk_slonglong(long long v){struct _tuple7 obj=({struct _tuple7 _Tmp0;_Tmp0.vtable=& Cyc_Sexp_slonglong_class,_Tmp0.v=v;_Tmp0;});struct _tuple7**objopt=({struct _tuple7**(*_Tmp0)(struct _tuple7*)=(struct _tuple7**(*)(struct _tuple7*))Cyc_Sexp_hashcons;_Tmp0;})(& obj);if(objopt!=0)return*objopt;{struct _tuple7*objp;objp=_cycalloc(sizeof(struct _tuple7)),*objp=obj;({void(*_Tmp0)(struct Cyc_Hashtable_Table*,struct _tuple7*,struct _tuple7*)=(void(*)(struct Cyc_Hashtable_Table*,struct _tuple7*,struct _tuple7*))Cyc_Hashtable_insert;_Tmp0;})(_check_null(obj.vtable->hash_table),objp,objp);return objp;}}struct Cyc_Sexp_Object Cyc_Sexp_obj_slonglong(long long v){return({struct Cyc_Sexp_Object(*_Tmp0)(struct _tuple7*)=({struct Cyc_Sexp_Object(*_Tmp1)(struct _tuple7*)=(struct Cyc_Sexp_Object(*)(struct _tuple7*))Cyc_Sexp_up;_Tmp1;});_Tmp0(Cyc_Sexp_mk_slonglong(v));});}struct Cyc_Sexp_Cls Cyc_Sexp_slonglong_class_w={& Cyc_Sexp_slonglong_class};
struct _tuple8*Cyc_Sexp_mk_float(float v){struct _tuple8 obj=({struct _tuple8 _Tmp0;_Tmp0.vtable=& Cyc_Sexp_float_class,_Tmp0.v=v;_Tmp0;});struct _tuple8**objopt=({struct _tuple8**(*_Tmp0)(struct _tuple8*)=(struct _tuple8**(*)(struct _tuple8*))Cyc_Sexp_hashcons;_Tmp0;})(& obj);if(objopt!=0)return*objopt;{struct _tuple8*objp;objp=_cycalloc(sizeof(struct _tuple8)),*objp=obj;({void(*_Tmp0)(struct Cyc_Hashtable_Table*,struct _tuple8*,struct _tuple8*)=(void(*)(struct Cyc_Hashtable_Table*,struct _tuple8*,struct _tuple8*))Cyc_Hashtable_insert;_Tmp0;})(_check_null(obj.vtable->hash_table),objp,objp);return objp;}}struct Cyc_Sexp_Object Cyc_Sexp_obj_float(float v){return({struct Cyc_Sexp_Object(*_Tmp0)(struct _tuple8*)=({struct Cyc_Sexp_Object(*_Tmp1)(struct _tuple8*)=(struct Cyc_Sexp_Object(*)(struct _tuple8*))Cyc_Sexp_up;_Tmp1;});_Tmp0(Cyc_Sexp_mk_float(v));});}struct Cyc_Sexp_Cls Cyc_Sexp_float_class_w={& Cyc_Sexp_float_class};
struct _tuple9*Cyc_Sexp_mk_double(double v){struct _tuple9 obj=({struct _tuple9 _Tmp0;_Tmp0.vtable=& Cyc_Sexp_double_class,_Tmp0.v=v;_Tmp0;});struct _tuple9**objopt=({struct _tuple9**(*_Tmp0)(struct _tuple9*)=(struct _tuple9**(*)(struct _tuple9*))Cyc_Sexp_hashcons;_Tmp0;})(& obj);if(objopt!=0)return*objopt;{struct _tuple9*objp;objp=_cycalloc(sizeof(struct _tuple9)),*objp=obj;({void(*_Tmp0)(struct Cyc_Hashtable_Table*,struct _tuple9*,struct _tuple9*)=(void(*)(struct Cyc_Hashtable_Table*,struct _tuple9*,struct _tuple9*))Cyc_Hashtable_insert;_Tmp0;})(_check_null(obj.vtable->hash_table),objp,objp);return objp;}}struct Cyc_Sexp_Object Cyc_Sexp_obj_double(double v){return({struct Cyc_Sexp_Object(*_Tmp0)(struct _tuple9*)=({struct Cyc_Sexp_Object(*_Tmp1)(struct _tuple9*)=(struct Cyc_Sexp_Object(*)(struct _tuple9*))Cyc_Sexp_up;_Tmp1;});_Tmp0(Cyc_Sexp_mk_double(v));});}struct Cyc_Sexp_Cls Cyc_Sexp_double_class_w={& Cyc_Sexp_double_class};
struct _tuple10*Cyc_Sexp_mk_str(struct _fat_ptr v){struct _tuple10 obj=({struct _tuple10 _Tmp0;_Tmp0.vtable=& Cyc_Sexp_str_class,_Tmp0.v=v;_Tmp0;});struct _tuple10**objopt=({struct _tuple10**(*_Tmp0)(struct _tuple10*)=(struct _tuple10**(*)(struct _tuple10*))Cyc_Sexp_hashcons;_Tmp0;})(& obj);if(objopt!=0)return*objopt;{struct _tuple10*objp;objp=_cycalloc(sizeof(struct _tuple10)),*objp=obj;({void(*_Tmp0)(struct Cyc_Hashtable_Table*,struct _tuple10*,struct _tuple10*)=(void(*)(struct Cyc_Hashtable_Table*,struct _tuple10*,struct _tuple10*))Cyc_Hashtable_insert;_Tmp0;})(_check_null(obj.vtable->hash_table),objp,objp);return objp;}}struct Cyc_Sexp_Object Cyc_Sexp_obj_str(struct _fat_ptr v){return({struct Cyc_Sexp_Object(*_Tmp0)(struct _tuple10*)=({struct Cyc_Sexp_Object(*_Tmp1)(struct _tuple10*)=(struct Cyc_Sexp_Object(*)(struct _tuple10*))Cyc_Sexp_up;_Tmp1;});_Tmp0(Cyc_Sexp_mk_str(v));});}struct Cyc_Sexp_Cls Cyc_Sexp_str_class_w={& Cyc_Sexp_str_class};
struct _tuple10*Cyc_Sexp_mk_symbol(struct _fat_ptr v){struct _tuple10 obj=({struct _tuple10 _Tmp0;_Tmp0.vtable=& Cyc_Sexp_symbol_class,_Tmp0.v=v;_Tmp0;});struct _tuple10**objopt=({struct _tuple10**(*_Tmp0)(struct _tuple10*)=(struct _tuple10**(*)(struct _tuple10*))Cyc_Sexp_hashcons;_Tmp0;})(& obj);if(objopt!=0)return*objopt;{struct _tuple10*objp;objp=_cycalloc(sizeof(struct _tuple10)),*objp=obj;({void(*_Tmp0)(struct Cyc_Hashtable_Table*,struct _tuple10*,struct _tuple10*)=(void(*)(struct Cyc_Hashtable_Table*,struct _tuple10*,struct _tuple10*))Cyc_Hashtable_insert;_Tmp0;})(_check_null(obj.vtable->hash_table),objp,objp);return objp;}}struct Cyc_Sexp_Object Cyc_Sexp_obj_symbol(struct _fat_ptr v){return({struct Cyc_Sexp_Object(*_Tmp0)(struct _tuple10*)=({struct Cyc_Sexp_Object(*_Tmp1)(struct _tuple10*)=(struct Cyc_Sexp_Object(*)(struct _tuple10*))Cyc_Sexp_up;_Tmp1;});_Tmp0(Cyc_Sexp_mk_symbol(v));});}struct Cyc_Sexp_Cls Cyc_Sexp_symbol_class_w={& Cyc_Sexp_symbol_class};
struct _tuple10*Cyc_Sexp_mk_tuple(struct _fat_ptr v){struct _tuple10 obj=({struct _tuple10 _Tmp0;_Tmp0.vtable=& Cyc_Sexp_tuple_class,_Tmp0.v=v;_Tmp0;});struct _tuple10**objopt=({struct _tuple10**(*_Tmp0)(struct _tuple10*)=(struct _tuple10**(*)(struct _tuple10*))Cyc_Sexp_hashcons;_Tmp0;})(& obj);if(objopt!=0)return*objopt;{struct _tuple10*objp;objp=_cycalloc(sizeof(struct _tuple10)),*objp=obj;({void(*_Tmp0)(struct Cyc_Hashtable_Table*,struct _tuple10*,struct _tuple10*)=(void(*)(struct Cyc_Hashtable_Table*,struct _tuple10*,struct _tuple10*))Cyc_Hashtable_insert;_Tmp0;})(_check_null(obj.vtable->hash_table),objp,objp);return objp;}}struct Cyc_Sexp_Object Cyc_Sexp_obj_tuple(struct _fat_ptr v){return({struct Cyc_Sexp_Object(*_Tmp0)(struct _tuple10*)=({struct Cyc_Sexp_Object(*_Tmp1)(struct _tuple10*)=(struct Cyc_Sexp_Object(*)(struct _tuple10*))Cyc_Sexp_up;_Tmp1;});_Tmp0(Cyc_Sexp_mk_tuple(v));});}struct Cyc_Sexp_Cls Cyc_Sexp_tuple_class_w={& Cyc_Sexp_tuple_class};
# 817
struct Cyc_Sexp_Object Cyc_Sexp_tuple(struct _fat_ptr objs){
return Cyc_Sexp_obj_tuple(({unsigned _Tmp0=_get_fat_size(objs,sizeof(struct Cyc_Sexp_Object));_tag_fat(({struct Cyc_Sexp_Object*_Tmp1=_cycalloc(_check_times(_Tmp0,sizeof(struct Cyc_Sexp_Object)));({{unsigned _Tmp2=_get_fat_size(objs,sizeof(struct Cyc_Sexp_Object));unsigned i;for(i=0;i < _Tmp2;++ i){_Tmp1[i]=((struct Cyc_Sexp_Object*)objs.curr)[(int)i];}}0;});_Tmp1;}),sizeof(struct Cyc_Sexp_Object),_Tmp0);}));}
# 823
static struct Cyc_List_List Cyc_Sexp_c0={(void*)& Cyc_Sexp_uchar_class_w,0};
static struct Cyc_List_List Cyc_Sexp_c1={(void*)& Cyc_Sexp_schar_class_w,& Cyc_Sexp_c0};
static struct Cyc_List_List Cyc_Sexp_c2={(void*)& Cyc_Sexp_ushort_class_w,& Cyc_Sexp_c1};
static struct Cyc_List_List Cyc_Sexp_c3={(void*)& Cyc_Sexp_sshort_class_w,& Cyc_Sexp_c2};
static struct Cyc_List_List Cyc_Sexp_c4={(void*)& Cyc_Sexp_uint_class_w,& Cyc_Sexp_c3};
static struct Cyc_List_List Cyc_Sexp_c5={(void*)& Cyc_Sexp_sint_class_w,& Cyc_Sexp_c4};
static struct Cyc_List_List Cyc_Sexp_c6={(void*)& Cyc_Sexp_ulonglong_class_w,& Cyc_Sexp_c5};
static struct Cyc_List_List Cyc_Sexp_c7={(void*)& Cyc_Sexp_slonglong_class_w,& Cyc_Sexp_c6};
static struct Cyc_List_List Cyc_Sexp_c8={(void*)& Cyc_Sexp_float_class_w,& Cyc_Sexp_c7};
static struct Cyc_List_List Cyc_Sexp_c9={(void*)& Cyc_Sexp_double_class_w,& Cyc_Sexp_c8};
static struct Cyc_List_List Cyc_Sexp_c10={(void*)& Cyc_Sexp_symbol_class_w,& Cyc_Sexp_c9};
static struct Cyc_List_List Cyc_Sexp_c11={(void*)& Cyc_Sexp_str_class_w,& Cyc_Sexp_c10};
static struct Cyc_List_List Cyc_Sexp_c12={(void*)& Cyc_Sexp_tuple_class_w,& Cyc_Sexp_c11};
# 837
struct Cyc_List_List*Cyc_Sexp_classes=& Cyc_Sexp_c12;
# 840
struct Cyc_Sexp_Visitor Cyc_Sexp_empty_visitor (void){
return({struct Cyc_Sexp_Visitor _Tmp0;_Tmp0.visit_uchar=0,_Tmp0.visit_schar=0,_Tmp0.visit_ushort=0,_Tmp0.visit_sshort=0,_Tmp0.visit_uint=0,_Tmp0.visit_sint=0,_Tmp0.visit_ulonglong=0,_Tmp0.visit_slonglong=0,_Tmp0.visit_float=0,_Tmp0.visit_double=0,_Tmp0.visit_symbol=0,_Tmp0.visit_str=0,_Tmp0.visit_tuple=0,_Tmp0.visit_default=0;_Tmp0;});}
# 858
struct Cyc_Sexp_Visitor Cyc_Sexp_default_visitor(void*(*def)(void*,struct Cyc_Sexp_Object,struct Cyc_Sexp_Visitor*)){
struct Cyc_Sexp_Visitor v=Cyc_Sexp_empty_visitor();
v.visit_default=def;
return v;}
