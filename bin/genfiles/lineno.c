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
void* _throw_assert_fn(const char *,unsigned);
void* _throw_fn(void*,const char*,unsigned);
void* _rethrow(void*);
#define _throw_null() (_throw_null_fn(__FILE__,__LINE__))
#define _throw_arraybounds() (_throw_arraybounds_fn(__FILE__,__LINE__))
#define _throw_badalloc() (_throw_badalloc_fn(__FILE__,__LINE__))
#define _throw_match() (_throw_match_fn(__FILE__,__LINE__))
#define _throw_assert() (_throw_assert_fn(__FILE__,__LINE__))
#define _throw(e) (_throw_fn((e),__FILE__,__LINE__))
#endif

void* Cyc_Core_get_exn_thrown();
/* Built-in Exceptions */
struct Cyc_Null_Exception_exn_struct { char *tag; };
struct Cyc_Array_bounds_exn_struct { char *tag; };
struct Cyc_Match_Exception_exn_struct { char *tag; };
struct Cyc_Bad_alloc_exn_struct { char *tag; };
struct Cyc_Assert_exn_struct { char *tag; };
extern char Cyc_Null_Exception[];
extern char Cyc_Array_bounds[];
extern char Cyc_Match_Exception[];
extern char Cyc_Bad_alloc[];
extern char Cyc_Assert[];

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
#define _untag_fat_ptr_check_bound(arr,elt_sz,num_elts) ((arr).curr)
#define _check_fat_at_base(arr) (arr)
#else
#define _check_fat_subscript(arr,elt_sz,index) ({ \
  struct _fat_ptr _cus_arr = (arr); \
  unsigned char *_cus_ans = _cus_arr.curr + (elt_sz) * (index); \
  /* JGM: not needed! if (!_cus_arr.base) _throw_null();*/ \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#define _untag_fat_ptr(arr,elt_sz,num_elts) ((arr).curr)
#define _untag_fat_ptr_check_bound(arr,elt_sz,num_elts) ({ \
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

# 95 "core.h"
 struct _fat_ptr Cyc_Core_new_string(unsigned);
# 121
int Cyc_Core_intcmp(int,int);struct Cyc___cycFILE;struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};
# 197 "cycboot.h"
extern int Cyc_sscanf(struct _fat_ptr,struct _fat_ptr,struct _fat_ptr);
# 271 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_file_open(struct _fat_ptr,struct _fat_ptr);
extern void Cyc_file_close(struct Cyc___cycFILE*);extern char Cyc_Lexing_Error[6U];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _fat_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 78 "lexing.h"
extern struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
# 82
extern struct _fat_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);
# 85
extern int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 210 "list.h"
extern struct Cyc_List_List*Cyc_List_merge_sort(int(*)(void*,void*),struct Cyc_List_List*);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 105 "string.h"
extern struct _fat_ptr Cyc_strdup(struct _fat_ptr);
# 110
extern struct _fat_ptr Cyc_substring(struct _fat_ptr,int,unsigned long);struct Cyc_Lineno_Pos{struct _fat_ptr logical_file;struct _fat_ptr line;int line_no;int col;};
# 26 "lineno.cyl"
enum Cyc_Lineno_token_val{Cyc_Lineno_NEWLINE =0U,Cyc_Lineno_LINEDEF =1U,Cyc_Lineno_END =2U};
# 34
const int Cyc_Lineno_lex_base[10U]={0,1,- 2,2,6,- 3,16,7,- 1,8};
const int Cyc_Lineno_lex_backtrk[10U]={- 1,- 1,- 1,1,- 1,- 1,- 1,- 1,- 1,0};
const int Cyc_Lineno_lex_default[10U]={1,1,0,- 1,1,0,7,7,0,- 1};
const int Cyc_Lineno_lex_trans[273U]={0,0,0,0,0,0,0,0,0,0,2,2,2,3,3,4,2,8,8,3,9,0,0,0,0,0,8,0,0,9,0,0,0,0,0,4,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,5,0,0,0,0,5,5,0,0,0,0,0,0,0,0,5};
const int Cyc_Lineno_lex_check[273U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,1,3,0,1,4,4,7,9,4,7,- 1,- 1,- 1,- 1,- 1,6,- 1,- 1,6,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,4,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,4,4,4,4,4,4,4,4,4,4,6,6,6,6,6,6,6,6,6,6,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,1,- 1,- 1,- 1,- 1,4,7,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6};
int Cyc_Lineno_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){struct Cyc_Lexing_lexbuf*_T0;struct Cyc_Lexing_lexbuf*_T1;struct Cyc_Lexing_lexbuf*_T2;struct Cyc_Lexing_lexbuf*_T3;int _T4;const int*_T5;int _T6;const char*_T7;const int*_T8;int _T9;int _TA;const int*_TB;int _TC;const char*_TD;const int*_TE;struct Cyc_Lexing_lexbuf*_TF;struct Cyc_Lexing_lexbuf*_T10;struct Cyc_Lexing_lexbuf*_T11;struct Cyc_Lexing_lexbuf*_T12;int _T13;struct Cyc_Lexing_lexbuf*_T14;int _T15;struct Cyc_Lexing_lexbuf*_T16;int _T17;int _T18;int _T19;struct Cyc_Lexing_lexbuf*_T1A;struct _fat_ptr _T1B;struct Cyc_Lexing_lexbuf*_T1C;int _T1D;int _T1E;char*_T1F;char*_T20;char _T21;const int*_T22;int _T23;const char*_T24;const int*_T25;int _T26;int _T27;const int*_T28;int _T29;const char*_T2A;const int*_T2B;const int*_T2C;int _T2D;const char*_T2E;const int*_T2F;struct Cyc_Lexing_lexbuf*_T30;struct Cyc_Lexing_lexbuf*_T31;struct Cyc_Lexing_lexbuf*_T32;int _T33;struct Cyc_Lexing_Error_exn_struct*_T34;void*_T35;struct Cyc_Lexing_lexbuf*_T36;int _T37;struct Cyc_Lexing_lexbuf*_T38;
# 41
int state;int base;int backtrk;
int c;
state=start_state;
# 45
if(state < 0)goto _TL0;_T0=lbuf;_T1=lbuf;_T2=lbuf;
_T1->lex_start_pos=_T2->lex_curr_pos;_T0->lex_last_pos=_T1->lex_start_pos;_T3=lbuf;
_T3->lex_last_action=-1;goto _TL1;
# 49
_TL0: _T4=- state;state=_T4 - 1;_TL1:
# 51
 _TL2: if(1)goto _TL3;else{goto _TL4;}
_TL3: _T5=Cyc_Lineno_lex_base;_T6=state;_T7=_check_known_subscript_notnull(_T5,10U,sizeof(int),_T6);_T8=(const int*)_T7;base=*_T8;
if(base >= 0)goto _TL5;_T9=- base;_TA=_T9 - 1;return _TA;_TL5: _TB=Cyc_Lineno_lex_backtrk;_TC=state;_TD=_check_known_subscript_notnull(_TB,10U,sizeof(int),_TC);_TE=(const int*)_TD;
backtrk=*_TE;
if(backtrk < 0)goto _TL7;_TF=lbuf;_T10=lbuf;
_TF->lex_last_pos=_T10->lex_curr_pos;_T11=lbuf;
_T11->lex_last_action=backtrk;goto _TL8;_TL7: _TL8: _T12=lbuf;_T13=_T12->lex_curr_pos;_T14=lbuf;_T15=_T14->lex_buffer_len;
# 59
if(_T13 < _T15)goto _TL9;_T16=lbuf;_T17=_T16->lex_eof_reached;
if(_T17)goto _TLB;else{goto _TLD;}
_TLD: _T18=- state;_T19=_T18 - 1;return _T19;
# 63
_TLB: c=256;goto _TLA;
# 65
_TL9: _T1A=lbuf;_T1B=_T1A->lex_buffer;_T1C=lbuf;_T1D=_T1C->lex_curr_pos;_T1C->lex_curr_pos=_T1D + 1;_T1E=_T1D;_T1F=_check_fat_subscript(_T1B,sizeof(char),_T1E);_T20=(char*)_T1F;_T21=*_T20;c=(int)_T21;
if(c!=-1)goto _TLE;c=256;goto _TLF;
_TLE: if(c >= 0)goto _TL10;c=256 + c;goto _TL11;_TL10: _TL11: _TLF: _TLA: _T22=Cyc_Lineno_lex_check;_T23=base + c;_T24=_check_known_subscript_notnull(_T22,273U,sizeof(int),_T23);_T25=(const int*)_T24;_T26=*_T25;_T27=state;
# 69
if(_T26!=_T27)goto _TL12;_T28=Cyc_Lineno_lex_trans;_T29=base + c;_T2A=_check_known_subscript_notnull(_T28,273U,sizeof(int),_T29);_T2B=(const int*)_T2A;
state=*_T2B;goto _TL13;
# 72
_TL12: _T2C=Cyc_Lineno_lex_default;_T2D=state;_T2E=_check_known_subscript_notnull(_T2C,10U,sizeof(int),_T2D);_T2F=(const int*)_T2E;state=*_T2F;_TL13:
 if(state >= 0)goto _TL14;_T30=lbuf;_T31=lbuf;
_T30->lex_curr_pos=_T31->lex_last_pos;_T32=lbuf;_T33=_T32->lex_last_action;
if(_T33!=-1)goto _TL16;{struct Cyc_Lexing_Error_exn_struct*_T39=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_T39->tag=Cyc_Lexing_Error;
_T39->f1=_tag_fat("empty token",sizeof(char),12U);_T34=(struct Cyc_Lexing_Error_exn_struct*)_T39;}_T35=(void*)_T34;_throw(_T35);goto _TL17;
# 78
_TL16: _T36=lbuf;_T37=_T36->lex_last_action;return _T37;_TL17: goto _TL15;
# 81
_TL14: if(c!=256)goto _TL18;_T38=lbuf;_T38->lex_eof_reached=0;goto _TL19;_TL18: _TL19: _TL15: goto _TL2;_TL4:;}
# 85
enum Cyc_Lineno_token_val Cyc_Lineno_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){int _T0;int _T1;struct Cyc_Lexing_lexbuf*_T2;void(*_T3)(struct Cyc_Lexing_lexbuf*);enum Cyc_Lineno_token_val _T4;struct Cyc_Lexing_Error_exn_struct*_T5;void*_T6;
lexstate=Cyc_Lineno_lex_engine(lexstate,lexbuf);_T0=lexstate;_T1=(int)_T0;switch(_T1){case 0:
# 48 "lineno.cyl"
 return 1U;case 1:
# 49 "lineno.cyl"
 return 0U;case 2:
# 50 "lineno.cyl"
 return 2U;default: _T2=lexbuf;_T3=_T2->refill_buff;
_T3(lexbuf);_T4=
Cyc_Lineno_token_rec(lexbuf,lexstate);return _T4;}{struct Cyc_Lexing_Error_exn_struct*_T7=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_T7->tag=Cyc_Lexing_Error;
# 54
_T7->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_T5=(struct Cyc_Lexing_Error_exn_struct*)_T7;}_T6=(void*)_T5;_throw(_T6);}
# 56
enum Cyc_Lineno_token_val Cyc_Lineno_token(struct Cyc_Lexing_lexbuf*lexbuf){enum Cyc_Lineno_token_val _T0;_T0=Cyc_Lineno_token_rec(lexbuf,0);return _T0;}struct _tuple0{struct _fat_ptr f0;int f1;};
# 63 "lineno.cyl"
static struct _tuple0*Cyc_Lineno_parse_linedef(struct _fat_ptr line){struct _handler_cons*_T0;int*_T1;int _T2;int _T3;unsigned _T4;struct _fat_ptr _T5;unsigned _T6;struct _fat_ptr _T7;unsigned char*_T8;char*_T9;int _TA;char _TB;int _TC;struct _fat_ptr _TD;unsigned char*_TE;char*_TF;int _T10;char _T11;int _T12;int _T13;unsigned _T14;struct _fat_ptr _T15;unsigned _T16;struct _fat_ptr _T17;unsigned char*_T18;char*_T19;int _T1A;char _T1B;int _T1C;struct _fat_ptr _T1D;unsigned char*_T1E;char*_T1F;int _T20;char _T21;int _T22;int _T23;unsigned _T24;struct _fat_ptr _T25;unsigned _T26;int _T27;struct Cyc_IntPtr_sa_ScanfArg_struct _T28;struct _fat_ptr _T29;int _T2A;int _T2B;unsigned long _T2C;struct _fat_ptr _T2D;struct _fat_ptr _T2E;struct _fat_ptr _T2F;int _T30;unsigned _T31;struct _fat_ptr _T32;unsigned _T33;struct _fat_ptr _T34;unsigned char*_T35;char*_T36;int _T37;char _T38;int _T39;int _T3A;unsigned _T3B;struct _fat_ptr _T3C;unsigned _T3D;struct _fat_ptr _T3E;unsigned char*_T3F;char*_T40;int _T41;char _T42;int _T43;int _T44;unsigned _T45;struct _fat_ptr _T46;unsigned _T47;int _T48;unsigned _T49;struct _fat_ptr _T4A;unsigned _T4B;struct _fat_ptr _T4C;int _T4D;int _T4E;unsigned long _T4F;void*_T50;struct _handler_cons _T51;_T0=& _T51;_push_handler(_T0);{int _T52=0;_T1=_T51.handler;_T2=setjmp(_T1);if(!_T2)goto _TL1B;_T52=1;goto _TL1C;_TL1B: _TL1C: if(_T52)goto _TL1D;else{goto _TL1F;}_TL1F:{
# 65
int i=0;
_TL20: _T3=i;_T4=(unsigned)_T3;_T5=line;_T6=_get_fat_size(_T5,sizeof(char));if(_T4 < _T6)goto _TL23;else{goto _TL22;}_TL23: _T7=line;_T8=_T7.curr;_T9=(char*)_T8;_TA=i;_TB=_T9[_TA];_TC=(int)_TB;if(_TC < 48)goto _TL21;else{goto _TL24;}_TL24: _TD=line;_TE=_TD.curr;_TF=(char*)_TE;_T10=i;_T11=_TF[_T10];_T12=(int)_T11;if(_T12 > 57)goto _TL21;else{goto _TL22;}_TL21: i=i + 1;goto _TL20;_TL22: {
int j=i;
_TL25: _T13=j;_T14=(unsigned)_T13;_T15=line;_T16=_get_fat_size(_T15,sizeof(char));if(_T14 < _T16)goto _TL29;else{goto _TL27;}_TL29: _T17=line;_T18=_T17.curr;_T19=(char*)_T18;_T1A=j;_T1B=_T19[_T1A];_T1C=(int)_T1B;if(_T1C >= 48)goto _TL28;else{goto _TL27;}_TL28: _T1D=line;_T1E=_T1D.curr;_T1F=(char*)_T1E;_T20=j;_T21=_T1F[_T20];_T22=(int)_T21;if(_T22 <= 57)goto _TL26;else{goto _TL27;}_TL26: j=j + 1;goto _TL25;_TL27: _T23=i;_T24=(unsigned)_T23;_T25=line;_T26=
_get_fat_size(_T25,sizeof(char));if(_T24!=_T26)goto _TL2A;{struct _tuple0*_T53=0;_npop_handler(0);return _T53;}_TL2A: {
int number=0;{struct Cyc_IntPtr_sa_ScanfArg_struct _T53;_T53.tag=2;
_T53.f1=& number;_T28=_T53;}{struct Cyc_IntPtr_sa_ScanfArg_struct _T53=_T28;void*_T54[1];_T54[0]=& _T53;_T29=line;_T2A=i;_T2B=j - i;_T2C=(unsigned long)_T2B;_T2D=Cyc_substring(_T29,_T2A,_T2C);_T2E=_tag_fat("%d",sizeof(char),3U);_T2F=_tag_fat(_T54,sizeof(void*),1);_T27=Cyc_sscanf(_T2D,_T2E,_T2F);}if(_T27==1)goto _TL2C;{struct _tuple0*_T53=0;_npop_handler(0);return _T53;}_TL2C:
# 73
 _TL2E: _T30=j;_T31=(unsigned)_T30;_T32=line;_T33=_get_fat_size(_T32,sizeof(char));if(_T31 < _T33)goto _TL31;else{goto _TL30;}_TL31: _T34=line;_T35=_T34.curr;_T36=(char*)_T35;_T37=j;_T38=_T36[_T37];_T39=(int)_T38;if(_T39!=34)goto _TL2F;else{goto _TL30;}_TL2F: j=j + 1;goto _TL2E;_TL30:
 j=j + 1;{int k=j;
_TL32: _T3A=k;_T3B=(unsigned)_T3A;_T3C=line;_T3D=_get_fat_size(_T3C,sizeof(char));if(_T3B < _T3D)goto _TL35;else{goto _TL34;}_TL35: _T3E=line;_T3F=_T3E.curr;_T40=(char*)_T3F;_T41=k;_T42=_T40[_T41];_T43=(int)_T42;if(_T43!=34)goto _TL33;else{goto _TL34;}_TL33: k=k + 1;goto _TL32;_TL34: _T44=j;_T45=(unsigned)_T44;_T46=line;_T47=
_get_fat_size(_T46,sizeof(char));if(_T45==_T47)goto _TL38;else{goto _TL39;}_TL39: _T48=k;_T49=(unsigned)_T48;_T4A=line;_T4B=_get_fat_size(_T4A,sizeof(char));if(_T49==_T4B)goto _TL38;else{goto _TL36;}_TL38:{struct _tuple0*_T53=0;_npop_handler(0);return _T53;}_TL36: _T4C=line;_T4D=j;_T4E=k - j;_T4F=(unsigned long)_T4E;{
struct _fat_ptr fname=Cyc_substring(_T4C,_T4D,_T4F);struct _tuple0*_T53;_T53=_cycalloc(sizeof(struct _tuple0));
_T53->f0=fname;_T53->f1=number;_npop_handler(0);return _T53;}}}}}_pop_handler();goto _TL1E;_TL1D: _T50=Cyc_Core_get_exn_thrown();{void*_T53=(void*)_T50;
return 0;;}_TL1E:;}}struct _tuple1{int f0;struct Cyc_Lineno_Pos*f1;};
# 82
int Cyc_Lineno_place_cmp(struct _tuple1*place1,struct _tuple1*place2){struct _tuple1*_T0;struct _tuple1 _T1;int _T2;struct _tuple1*_T3;struct _tuple1 _T4;int _T5;int _T6;_T0=place1;_T1=*_T0;_T2=_T1.f0;_T3=place2;_T4=*_T3;_T5=_T4.f0;_T6=
Cyc_Core_intcmp(_T2,_T5);return _T6;}
# 87
void Cyc_Lineno_poss_of_abss(struct _fat_ptr filename,struct Cyc_List_List*places){struct Cyc_List_List*(*_T0)(int(*)(struct _tuple1*,struct _tuple1*),struct Cyc_List_List*);struct Cyc_List_List*(*_T1)(int(*)(void*,void*),struct Cyc_List_List*);struct Cyc_List_List*_T2;struct _fat_ptr _T3;struct _fat_ptr _T4;struct _handler_cons*_T5;int*_T6;int _T7;enum Cyc_Lineno_token_val _T8;int _T9;int _TA;struct Cyc_List_List*_TB;void*_TC;struct _tuple1*_TD;struct _tuple1 _TE;int _TF;enum Cyc_Lineno_token_val _T10;int _T11;struct _tuple0*_T12;struct _tuple0 _T13;struct _tuple0*_T14;struct _tuple0 _T15;enum Cyc_Lineno_token_val _T16;int _T17;int _T18;struct Cyc_List_List*_T19;void*_T1A;struct _tuple1*_T1B;struct _tuple1 _T1C;int _T1D;struct Cyc_List_List*_T1E;void*_T1F;struct _tuple1*_T20;struct _tuple1 _T21;struct Cyc_Lineno_Pos*_T22;struct Cyc_Lineno_Pos*_T23;struct Cyc_Lineno_Pos*_T24;struct Cyc_Lineno_Pos*_T25;unsigned long _T26;int _T27;struct Cyc_List_List*_T28;void*_T29;struct _tuple1*_T2A;struct _tuple1 _T2B;int _T2C;int _T2D;unsigned long _T2E;unsigned long _T2F;struct Cyc_Lineno_Pos*_T30;int _T31;struct Cyc_Lineno_Pos*_T32;struct Cyc_List_List*_T33;void*_T34;_T1=Cyc_List_merge_sort;{
struct Cyc_List_List*(*_T35)(int(*)(struct _tuple1*,struct _tuple1*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct _tuple1*,struct _tuple1*),struct Cyc_List_List*))_T1;_T0=_T35;}_T2=places;places=_T0(Cyc_Lineno_place_cmp,_T2);_T3=filename;_T4=
_tag_fat("r",sizeof(char),2U);{struct Cyc___cycFILE*f=Cyc_file_open(_T3,_T4);{struct _handler_cons _T35;_T5=& _T35;_push_handler(_T5);{int _T36=0;_T6=_T35.handler;_T7=setjmp(_T6);if(!_T7)goto _TL3A;_T36=1;goto _TL3B;_TL3A: _TL3B: if(_T36)goto _TL3C;else{goto _TL3E;}_TL3E:{
# 91
struct Cyc_Lexing_lexbuf*lbuf=Cyc_Lexing_from_file(f);
struct _fat_ptr source_file=filename;
int line=1;
struct _fat_ptr this_line;
int eol;
enum Cyc_Lineno_token_val next;
# 98
_TL3F: if(places!=0)goto _TL40;else{goto _TL41;}
_TL40: _TL42: if(1)goto _TL43;else{goto _TL44;}
_TL43: next=Cyc_Lineno_token(lbuf);
eol=Cyc_Lexing_lexeme_end(lbuf);
# 103
this_line=Cyc_Lexing_lexeme(lbuf);_T8=next;_T9=(int)_T8;
# 105
if(_T9==2)goto _TL47;else{goto _TL48;}_TL48: _TA=eol;_TB=places;_TC=_TB->hd;_TD=(struct _tuple1*)_TC;_TE=*_TD;_TF=_TE.f0;if(_TA > _TF)goto _TL47;else{goto _TL45;}
_TL47: goto _TL44;_TL45: _T10=next;_T11=(int)_T10;
# 108
if(_T11!=0)goto _TL49;line=line + 1;goto _TL4A;
# 110
_TL49:{struct _tuple0*fno=Cyc_Lineno_parse_linedef(this_line);
if(fno!=0)goto _TL4B;
line=line + 1;goto _TL4C;
# 116
_TL4B: _T12=fno;_T13=*_T12;source_file=_T13.f0;_T14=fno;_T15=*_T14;
line=_T15.f1;_TL4C:;}_TL4A: goto _TL42;_TL44:
# 121
 _TL4D: if(places!=0)goto _TL50;else{goto _TL4F;}_TL50: _T16=next;_T17=(int)_T16;if(_T17==2)goto _TL4E;else{goto _TL51;}_TL51: _T18=eol;_T19=places;_T1A=_T19->hd;_T1B=(struct _tuple1*)_T1A;_T1C=*_T1B;_T1D=_T1C.f0;if(_T18 > _T1D)goto _TL4E;else{goto _TL4F;}
_TL4E: _T1E=places;_T1F=_T1E->hd;_T20=(struct _tuple1*)_T1F;_T21=*_T20;{struct Cyc_Lineno_Pos*p=_T21.f1;_T22=p;
_T22->logical_file=Cyc_strdup(source_file);_T23=p;
_T23->line=this_line;_T24=p;
_T24->line_no=line;_T25=p;_T26=
Cyc_strlen(this_line);_T27=eol;_T28=places;_T29=_T28->hd;_T2A=(struct _tuple1*)_T29;_T2B=*_T2A;_T2C=_T2B.f0;_T2D=_T27 - _T2C;_T2E=(unsigned long)_T2D;_T2F=_T26 - _T2E;_T25->col=(int)_T2F;_T30=p;_T31=_T30->col;
if(_T31 >= 0)goto _TL52;_T32=p;_T32->col=0;goto _TL53;_TL52: _TL53: _T33=places;
places=_T33->tl;}goto _TL4D;_TL4F:
# 130
 line=line + 1;goto _TL3F;_TL41:;}_pop_handler();goto _TL3D;_TL3C: _T34=Cyc_Core_get_exn_thrown();{void*_T37=(void*)_T34;void*_T38;_T38=_T37;{void*y=_T38;
# 132
Cyc_file_close(f);_throw(y);};}_TL3D:;}}
Cyc_file_close(f);
return;}}
# 137
struct Cyc_Lineno_Pos*Cyc_Lineno_pos_of_abs(struct _fat_ptr filename,int abs){struct Cyc_Lineno_Pos*_T0;struct Cyc_Lineno_Pos*_T1;struct Cyc_Lineno_Pos*_T2;struct Cyc_Lineno_Pos*_T3;struct _fat_ptr _T4;struct Cyc_List_List*_T5;struct _tuple1*_T6;struct Cyc_Lineno_Pos*_T7;
struct Cyc_Lineno_Pos*ans;ans=_cycalloc(sizeof(struct Cyc_Lineno_Pos));_T0=ans;_T0->logical_file=_tag_fat("",sizeof(char),1U);_T1=ans;_T1->line=Cyc_Core_new_string(0U);_T2=ans;_T2->line_no=0;_T3=ans;_T3->col=0;_T4=filename;{struct Cyc_List_List*_T8=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple1*_T9=_cycalloc(sizeof(struct _tuple1));
_T9->f0=abs;_T9->f1=ans;_T6=(struct _tuple1*)_T9;}_T8->hd=_T6;_T8->tl=0;_T5=(struct Cyc_List_List*)_T8;}Cyc_Lineno_poss_of_abss(_T4,_T5);_T7=ans;
return _T7;}
