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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];extern char Cyc_Core_Not_found[10U];extern char Cyc_Core_Unreachable[12U];
# 171 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];extern char Cyc_Lexing_Error[6U];struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _fat_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 78 "lexing.h"
extern struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 61
extern int Cyc_List_length(struct Cyc_List_List*);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];
# 135
extern void Cyc_List_iter_c(void(*)(void*,void*),void*,struct Cyc_List_List*);
# 153
extern void*Cyc_List_fold_right(void*(*)(void*,void*),struct Cyc_List_List*,void*);
# 172
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];
# 276
extern struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 52
extern int Cyc_zstrcmp(struct _fat_ptr,struct _fat_ptr);
# 54
extern int Cyc_zstrptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 60
extern struct _fat_ptr Cyc_strcat(struct _fat_ptr,struct _fat_ptr);
# 62
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);
# 71
extern struct _fat_ptr Cyc_strcpy(struct _fat_ptr,struct _fat_ptr);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 283
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 296
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 303
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 464 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 471
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 489
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 662 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};
# 824
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;
extern struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val;
extern struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];
# 849
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 857
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Tqual);
# 863
void*Cyc_Absyn_compress(void*);
# 876
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 878
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 881
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 883
extern void*Cyc_Absyn_sint_type;
# 885
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 888
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 892
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 894
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_tag_type(void*);extern void*Cyc_Absyn_rgn_handle_type(void*);extern void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_access_eff(void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);extern void*Cyc_Absyn_enum_type(struct _tuple0*,struct Cyc_Absyn_Enumdecl*);extern void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*);extern void*Cyc_Absyn_builtin_type(struct _fat_ptr,struct Cyc_Absyn_Kind*);extern void*Cyc_Absyn_typedef_type(struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_Typedefdecl*,void*);
# 919
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 923
void*Cyc_Absyn_bounds_one (void);
# 925
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 945
void*Cyc_Absyn_array_type(void*,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*,void*,unsigned);
# 948
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 963
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 969
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
# 973
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned);
# 981
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 990
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 998
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1005
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1007
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int,enum Cyc_Absyn_Coercion,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1022
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1026
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1035
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1040
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*,unsigned);
# 1051
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*,unsigned);
# 1054
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned);
# 1057
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1061
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned);
# 1066
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple0*,void*,struct Cyc_Absyn_Exp*);
# 1068
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,int);
# 1075
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind,enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_AggrdeclImpl*,struct Cyc_List_List*,unsigned);
# 1082
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Core_Opt*,int,unsigned);
# 1085
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Core_Opt*,int,unsigned);
# 1090
void*Cyc_Absyn_function_type(struct Cyc_List_List*,void*,struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,int,struct Cyc_Absyn_VarargInfo*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 35
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 26 "flags.h"
extern int Cyc_Flags_porting_c_code;
# 34
extern int Cyc_Flags_no_register;
# 28 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_rk;
# 30
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 32
extern struct Cyc_Absyn_Kind Cyc_Kinds_ek;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ik;
# 37
extern struct Cyc_Absyn_Kind Cyc_Kinds_trk;
# 47
extern struct Cyc_Core_Opt Cyc_Kinds_rko;
# 49
extern struct Cyc_Core_Opt Cyc_Kinds_bko;
# 51
extern struct Cyc_Core_Opt Cyc_Kinds_iko;
# 56
extern struct Cyc_Core_Opt Cyc_Kinds_trko;
# 66
struct Cyc_Core_Opt*Cyc_Kinds_kind_to_opt(struct Cyc_Absyn_Kind*);
void*Cyc_Kinds_kind_to_bound(struct Cyc_Absyn_Kind*);
# 70
struct Cyc_Absyn_Kind*Cyc_Kinds_id_to_kind(struct _fat_ptr,unsigned);
# 78
void*Cyc_Kinds_compress_kb(void*);
# 46 "tcutil.h"
int Cyc_Tcutil_is_array_type(void*);
# 93
void*Cyc_Tcutil_copy_type(void*);
# 200
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 228
void*Cyc_Tcutil_promote_array(void*,void*,int);
# 238
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);
# 29 "attributes.h"
extern struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Atts_Const_att_val;
# 46
void*Cyc_Atts_parse_nullary_att(unsigned,struct _fat_ptr);
void*Cyc_Atts_parse_unary_att(unsigned,struct _fat_ptr,unsigned,struct Cyc_Absyn_Exp*);
void*Cyc_Atts_parse_format_att(unsigned,unsigned,struct _fat_ptr,struct _fat_ptr,unsigned,unsigned);
# 68
int Cyc_Atts_fntype_att(void*);extern char Cyc_Dict_Present[8U];extern char Cyc_Dict_Absent[7U];extern char Cyc_Tcenv_Env_error[10U];
# 29 "currgn.h"
struct _fat_ptr Cyc_CurRgn_curr_rgn_name;
# 31
void*Cyc_CurRgn_curr_rgn_type (void);
# 68 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst);
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);extern char Cyc_Parse_Exit[5U];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_Parse_FlatList{struct Cyc_Parse_FlatList*tl;void*hd[0U] __attribute__((aligned )) ;};struct Cyc_Parse_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned loc;};struct Cyc_Parse_Declarator{struct _tuple0*id;unsigned varloc;struct Cyc_List_List*tms;};struct _tuple12{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple11{struct _tuple11*tl;struct _tuple12 hd  __attribute__((aligned )) ;};struct _tuple13{struct _tuple13*tl;struct Cyc_Parse_Declarator hd  __attribute__((aligned )) ;};
# 54 "parse.h"
enum Cyc_Parse_Storage_class{Cyc_Parse_Typedef_sc =0U,Cyc_Parse_Extern_sc =1U,Cyc_Parse_ExternC_sc =2U,Cyc_Parse_Static_sc =3U,Cyc_Parse_Auto_sc =4U,Cyc_Parse_Register_sc =5U,Cyc_Parse_Abstract_sc =6U,Cyc_Parse_None_sc =7U};struct Cyc_Parse_Declaration_spec{enum Cyc_Parse_Storage_class sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Parse_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};
# 73 "parse.y"
void Cyc_Lex_register_typedef(struct _tuple0*);
void Cyc_Lex_enter_namespace(struct _fat_ptr*);
void Cyc_Lex_leave_namespace (void);
void Cyc_Lex_enter_using(struct _tuple0*);
void Cyc_Lex_leave_using (void);
void Cyc_Lex_enter_extern_c (void);
void Cyc_Lex_leave_extern_c (void);
extern struct _tuple0*Cyc_Lex_token_qvar;
extern struct _fat_ptr Cyc_Lex_token_string;
# 97 "parse.y"
int Cyc_Parse_parsing_tempest=0;
# 103
struct Cyc_Parse_FlatList*Cyc_Parse_flat_imp_rev(struct Cyc_Parse_FlatList*x){
if(x == 0)return x;{
struct Cyc_Parse_FlatList*first=x;
struct Cyc_Parse_FlatList*second=x->tl;
x->tl=0;
while(second != 0){
struct Cyc_Parse_FlatList*temp=second->tl;
second->tl=first;
first=second;
second=temp;}
# 114
return first;}}char Cyc_Parse_Exit[5U]="Exit";struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct{int tag;};
# 167
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier,unsigned);struct _tuple14{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
# 172
static struct Cyc_List_List*Cyc_Parse_parse_result=0;
# 174
static void*Cyc_Parse_parse_abort(unsigned loc,struct _fat_ptr msg){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63E;_tmp63E.tag=0,_tmp63E.f1=msg;_tmp63E;});void*_tmp0[1];_tmp0[0]=& _tmp1;Cyc_Warn_err2(loc,_tag_fat(_tmp0,sizeof(void*),1));});
(int)_throw((void*)({struct Cyc_Parse_Exit_exn_struct*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->tag=Cyc_Parse_Exit;_tmp2;}));}
# 179
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned loc){
# 181
struct _tuple8*_tmp3=tqt;void*_tmp5;struct Cyc_Absyn_Tqual _tmp4;_tmp4=_tmp3->f2;_tmp5=_tmp3->f3;{struct Cyc_Absyn_Tqual tq=_tmp4;void*t=_tmp5;
if((tq.print_const || tq.q_volatile)|| tq.q_restrict){
if(tq.loc != 0U)loc=tq.loc;
({void*_tmp6=0U;({unsigned _tmp6E7=loc;struct _fat_ptr _tmp6E6=({const char*_tmp7="qualifier on type is ignored";_tag_fat(_tmp7,sizeof(char),29U);});Cyc_Warn_warn(_tmp6E7,_tmp6E6,_tag_fat(_tmp6,sizeof(void*),0));});});}
# 186
return t;}}struct _tuple15{void*f1;void*f2;void*f3;void*f4;void*f5;};
# 189
static struct _tuple15 Cyc_Parse_collapse_pointer_quals(unsigned loc,void*nullable,void*bound,void*rgn,struct Cyc_List_List*pqs){
# 195
void*zeroterm=Cyc_Tcutil_any_bool(0);
void*autoreleased=Cyc_Tcutil_any_bool(0);
for(1;pqs != 0;pqs=pqs->tl){
void*_tmp8=(void*)pqs->hd;void*_stmttmp0=_tmp8;void*_tmp9=_stmttmp0;void*_tmpA;switch(*((int*)_tmp9)){case 4:
 zeroterm=Cyc_Absyn_true_type;goto _LL0;case 5:
 zeroterm=Cyc_Absyn_false_type;goto _LL0;case 6:
 autoreleased=Cyc_Absyn_true_type;goto _LL0;case 8:
 nullable=Cyc_Absyn_true_type;goto _LL0;case 7:
 nullable=Cyc_Absyn_false_type;goto _LL0;case 3:
 bound=Cyc_Absyn_fat_bound_type;goto _LL0;case 2:
 bound=Cyc_Absyn_bounds_one();goto _LL0;case 0: _tmpA=((struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*)_tmp9)->f1;{struct Cyc_Absyn_Exp*e=_tmpA;
bound=Cyc_Absyn_thin_bounds_exp(e);goto _LL0;}default: _tmpA=(void*)((struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*)_tmp9)->f1;{void*t=_tmpA;
rgn=t;goto _LL0;}}_LL0:;}
# 209
return({struct _tuple15 _tmp63F;_tmp63F.f1=nullable,_tmp63F.f2=bound,_tmp63F.f3=zeroterm,_tmp63F.f4=rgn,_tmp63F.f5=autoreleased;_tmp63F;});}
# 215
static struct _tuple0*Cyc_Parse_gensym_enum (void){
# 217
static int enum_counter=0;
return({struct _tuple0*_tmpF=_cycalloc(sizeof(*_tmpF));({union Cyc_Absyn_Nmspace _tmp6EB=Cyc_Absyn_Rel_n(0);_tmpF->f1=_tmp6EB;}),({
struct _fat_ptr*_tmp6EA=({struct _fat_ptr*_tmpE=_cycalloc(sizeof(*_tmpE));({struct _fat_ptr _tmp6E9=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD=({struct Cyc_Int_pa_PrintArg_struct _tmp640;_tmp640.tag=1,_tmp640.f1=(unsigned long)enum_counter ++;_tmp640;});void*_tmpB[1];_tmpB[0]=& _tmpD;({struct _fat_ptr _tmp6E8=({const char*_tmpC="__anonymous_enum_%d__";_tag_fat(_tmpC,sizeof(char),22U);});Cyc_aprintf(_tmp6E8,_tag_fat(_tmpB,sizeof(void*),1));});});*_tmpE=_tmp6E9;});_tmpE;});_tmpF->f2=_tmp6EA;});_tmpF;});}struct _tuple16{unsigned f1;struct _tuple0*f2;struct Cyc_Absyn_Tqual f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct _tuple17{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple18{struct _tuple16*f1;struct _tuple17*f2;};
# 222
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned loc,struct _tuple18*field_info){
# 227
struct _tuple18*_tmp10=field_info;void*_tmp18;void*_tmp17;void*_tmp16;void*_tmp15;void*_tmp14;struct Cyc_Absyn_Tqual _tmp13;void*_tmp12;unsigned _tmp11;_tmp11=(_tmp10->f1)->f1;_tmp12=(_tmp10->f1)->f2;_tmp13=(_tmp10->f1)->f3;_tmp14=(_tmp10->f1)->f4;_tmp15=(_tmp10->f1)->f5;_tmp16=(_tmp10->f1)->f6;_tmp17=(_tmp10->f2)->f1;_tmp18=(_tmp10->f2)->f2;{unsigned varloc=_tmp11;struct _tuple0*qid=_tmp12;struct Cyc_Absyn_Tqual tq=_tmp13;void*t=_tmp14;struct Cyc_List_List*tvs=_tmp15;struct Cyc_List_List*atts=_tmp16;struct Cyc_Absyn_Exp*widthopt=_tmp17;struct Cyc_Absyn_Exp*reqopt=_tmp18;
if(tvs != 0)
({void*_tmp19=0U;({unsigned _tmp6ED=loc;struct _fat_ptr _tmp6EC=({const char*_tmp1A="bad type params in struct field";_tag_fat(_tmp1A,sizeof(char),32U);});Cyc_Warn_err(_tmp6ED,_tmp6EC,_tag_fat(_tmp19,sizeof(void*),0));});});
if(Cyc_Absyn_is_qvar_qualified(qid))
({void*_tmp1B=0U;({unsigned _tmp6EF=loc;struct _fat_ptr _tmp6EE=({const char*_tmp1C="struct or union field cannot be qualified with a namespace";_tag_fat(_tmp1C,sizeof(char),59U);});Cyc_Warn_err(_tmp6EF,_tmp6EE,_tag_fat(_tmp1B,sizeof(void*),0));});});
return({struct Cyc_Absyn_Aggrfield*_tmp1D=_cycalloc(sizeof(*_tmp1D));_tmp1D->name=(*qid).f2,_tmp1D->tq=tq,_tmp1D->type=t,_tmp1D->width=widthopt,_tmp1D->attributes=atts,_tmp1D->requires_clause=reqopt;_tmp1D;});}}
# 237
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned loc){
return({struct Cyc_Parse_Type_specifier _tmp641;_tmp641.Signed_spec=0,_tmp641.Unsigned_spec=0,_tmp641.Short_spec=0,_tmp641.Long_spec=0,_tmp641.Long_Long_spec=0,_tmp641.Valid_type_spec=0,_tmp641.Type_spec=Cyc_Absyn_sint_type,_tmp641.loc=loc;_tmp641;});}
# 248
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned loc){
struct Cyc_Parse_Type_specifier _tmp1E=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp1E;
s.Type_spec=t;
s.Valid_type_spec=1;
return s;}
# 254
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _tmp1F=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp1F;
s.Signed_spec=1;
return s;}
# 259
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _tmp20=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp20;
s.Unsigned_spec=1;
return s;}
# 264
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _tmp21=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp21;
s.Short_spec=1;
return s;}
# 269
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _tmp22=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp22;
s.Long_spec=1;
return s;}
# 276
static void*Cyc_Parse_array2ptr(void*t,int argposn){
# 278
return Cyc_Tcutil_is_array_type(t)?({
void*_tmp6F0=t;Cyc_Tcutil_promote_array(_tmp6F0,argposn?Cyc_Absyn_new_evar(& Cyc_Kinds_rko,0): Cyc_Absyn_heap_rgn_type,0);}): t;}struct _tuple19{struct _fat_ptr*f1;void*f2;};
# 291 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp23=0;struct Cyc_List_List*res=_tmp23;
for(1;x != 0;x=x->tl){
struct _tuple8*_tmp24=(struct _tuple8*)x->hd;struct _tuple8*_stmttmp1=_tmp24;struct _tuple8*_tmp25=_stmttmp1;struct _fat_ptr _tmp26;void*_tmp28;void*_tmp27;if(*((int*)((struct _tuple8*)_tmp25)->f3)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp25)->f3)->f1)== 4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp25)->f3)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp25)->f3)->f2)->tl == 0){_tmp27=_tmp25->f1;_tmp28=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25->f3)->f2)->hd;if((struct _fat_ptr*)_tmp27 != 0){struct _fat_ptr*v=_tmp27;void*i=_tmp28;
# 296
{void*_tmp29=i;void*_tmp2A;if(*((int*)_tmp29)== 1){_tmp2A=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp29)->f2;{void**z=(void**)_tmp2A;
# 300
struct _fat_ptr*nm=({struct _fat_ptr*_tmp30=_cycalloc(sizeof(*_tmp30));({struct _fat_ptr _tmp6F2=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2F=({struct Cyc_String_pa_PrintArg_struct _tmp642;_tmp642.tag=0,_tmp642.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp642;});void*_tmp2D[1];_tmp2D[0]=& _tmp2F;({struct _fat_ptr _tmp6F1=({const char*_tmp2E="`%s";_tag_fat(_tmp2E,sizeof(char),4U);});Cyc_aprintf(_tmp6F1,_tag_fat(_tmp2D,sizeof(void*),1));});});*_tmp30=_tmp6F2;});_tmp30;});
({void*_tmp6F4=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->name=nm,_tmp2C->identity=-1,({void*_tmp6F3=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->tag=0,_tmp2B->f1=& Cyc_Kinds_ik;_tmp2B;});_tmp2C->kind=_tmp6F3;});_tmp2C;}));*z=_tmp6F4;});
goto _LL7;}}else{
goto _LL7;}_LL7:;}
# 305
res=({struct Cyc_List_List*_tmp32=_cycalloc(sizeof(*_tmp32));({struct _tuple19*_tmp6F5=({struct _tuple19*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->f1=v,_tmp31->f2=i;_tmp31;});_tmp32->hd=_tmp6F5;}),_tmp32->tl=res;_tmp32;});goto _LL0;}else{if(((struct _tuple8*)_tmp25)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp25)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp25)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp25)->f1 != 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp25)->f3)->f1)== 3){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp25)->f3)->f2 != 0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp25)->f3)->f2)->hd)== 1){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp25)->f3)->f2)->tl == 0){_tmp26=*_tmp25->f1;_tmp27=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25->f3)->f2)->hd)->f2;{struct _fat_ptr v=_tmp26;void**z=(void**)_tmp27;
# 309
struct _fat_ptr*nm=({struct _fat_ptr*_tmp38=_cycalloc(sizeof(*_tmp38));({struct _fat_ptr _tmp6F7=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp37=({struct Cyc_String_pa_PrintArg_struct _tmp643;_tmp643.tag=0,_tmp643.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp643;});void*_tmp35[1];_tmp35[0]=& _tmp37;({struct _fat_ptr _tmp6F6=({const char*_tmp36="`%s";_tag_fat(_tmp36,sizeof(char),4U);});Cyc_aprintf(_tmp6F6,_tag_fat(_tmp35,sizeof(void*),1));});});*_tmp38=_tmp6F7;});_tmp38;});
({void*_tmp6F9=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34->name=nm,_tmp34->identity=-1,({void*_tmp6F8=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->tag=0,_tmp33->f1=& Cyc_Kinds_rk;_tmp33;});_tmp34->kind=_tmp6F8;});_tmp34;}));*z=_tmp6F9;});
goto _LL0;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp25)->f1 != 0)goto _LL5;else{_LL5:
 goto _LL0;}}_LL0:;}
# 314
return res;}
# 318
static struct Cyc_List_List*Cyc_Parse_get_aggrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp39=0;struct Cyc_List_List*res=_tmp39;
for(1;x != 0;x=x->tl){
void*_tmp3A=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_stmttmp2=_tmp3A;void*_tmp3B=_stmttmp2;void*_tmp3C;if(*((int*)_tmp3B)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B)->f1)== 4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B)->f2)->tl == 0){_tmp3C=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B)->f2)->hd;{void*i=_tmp3C;
# 323
res=({struct Cyc_List_List*_tmp3E=_cycalloc(sizeof(*_tmp3E));({struct _tuple19*_tmp6FA=({struct _tuple19*_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,_tmp3D->f2=i;_tmp3D;});_tmp3E->hd=_tmp6FA;}),_tmp3E->tl=res;_tmp3E;});goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}
# 326
return res;}
# 330
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_tmp3F=e->r;void*_stmttmp3=_tmp3F;void*_tmp40=_stmttmp3;void*_tmp41;if(*((int*)_tmp40)== 1){if(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp40)->f1)== 0){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp40)->f1)->f1)->f1).Rel_n).tag == 1){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp40)->f1)->f1)->f1).Rel_n).val == 0){_tmp41=(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp40)->f1)->f1)->f2;{struct _fat_ptr*y=_tmp41;
# 333
{struct Cyc_List_List*_tmp42=tags;struct Cyc_List_List*ts=_tmp42;for(0;ts != 0;ts=ts->tl){
struct _tuple19*_tmp43=(struct _tuple19*)ts->hd;struct _tuple19*_stmttmp4=_tmp43;struct _tuple19*_tmp44=_stmttmp4;void*_tmp46;void*_tmp45;_tmp45=_tmp44->f1;_tmp46=_tmp44->f2;{struct _fat_ptr*x=_tmp45;void*i=_tmp46;
if(Cyc_strptrcmp(x,y)== 0)
return({void*_tmp6FC=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47->tag=39,({void*_tmp6FB=Cyc_Tcutil_copy_type(i);_tmp47->f1=_tmp6FB;});_tmp47;});Cyc_Absyn_new_exp(_tmp6FC,e->loc);});}}}
# 338
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}
# 341
return e;}
# 346
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_tmp48=t;void*_tmp50;void*_tmp4F;void*_tmp4E;void*_tmp4D;unsigned _tmp51;void*_tmp4C;void*_tmp4B;struct Cyc_Absyn_Tqual _tmp4A;void*_tmp49;switch(*((int*)_tmp48)){case 4: _tmp49=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp48)->f1).elt_type;_tmp4A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp48)->f1).tq;_tmp4B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp48)->f1).num_elts;_tmp4C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp48)->f1).zero_term;_tmp51=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp48)->f1).zt_loc;{void*et=_tmp49;struct Cyc_Absyn_Tqual tq=_tmp4A;struct Cyc_Absyn_Exp*nelts=_tmp4B;void*zt=_tmp4C;unsigned ztloc=_tmp51;
# 349
struct Cyc_Absyn_Exp*nelts2=nelts;
if(nelts != 0)
nelts2=Cyc_Parse_substitute_tags_exp(tags,nelts);{
void*_tmp52=Cyc_Parse_substitute_tags(tags,et);void*et2=_tmp52;
if(nelts != nelts2 || et != et2)
return Cyc_Absyn_array_type(et2,tq,nelts2,zt,ztloc);
goto _LL0;}}case 3: _tmp49=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48)->f1).elt_type;_tmp4A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48)->f1).elt_tq;_tmp4B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48)->f1).ptr_atts).rgn;_tmp4C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48)->f1).ptr_atts).nullable;_tmp4D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48)->f1).ptr_atts).bounds;_tmp4E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48)->f1).ptr_atts).zero_term;_tmp4F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48)->f1).ptr_atts).ptrloc;_tmp50=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48)->f1).ptr_atts).autoreleased;{void*et=_tmp49;struct Cyc_Absyn_Tqual tq=_tmp4A;void*r=_tmp4B;void*n=_tmp4C;void*b=_tmp4D;void*zt=_tmp4E;struct Cyc_Absyn_PtrLoc*pl=_tmp4F;void*rel=_tmp50;
# 357
void*_tmp53=Cyc_Parse_substitute_tags(tags,et);void*et2=_tmp53;
void*_tmp54=Cyc_Parse_substitute_tags(tags,b);void*b2=_tmp54;
if(et2 != et || b2 != b)
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp644;_tmp644.elt_type=et2,_tmp644.elt_tq=tq,(_tmp644.ptr_atts).rgn=r,(_tmp644.ptr_atts).nullable=n,(_tmp644.ptr_atts).bounds=b2,(_tmp644.ptr_atts).zero_term=zt,(_tmp644.ptr_atts).ptrloc=pl,(_tmp644.ptr_atts).autoreleased=rel;_tmp644;}));
goto _LL0;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48)->f1)== 13){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48)->f2)->tl == 0){_tmp49=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48)->f2)->hd;{void*t=_tmp49;
# 363
void*_tmp55=Cyc_Parse_substitute_tags(tags,t);void*t2=_tmp55;
if(t != t2)return Cyc_Absyn_thin_bounds_type(t2);
goto _LL0;}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 9: _tmp49=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp48)->f1;{struct Cyc_Absyn_Exp*e=_tmp49;
# 367
struct Cyc_Absyn_Exp*_tmp56=Cyc_Parse_substitute_tags_exp(tags,e);struct Cyc_Absyn_Exp*e2=_tmp56;
if(e2 != e)return Cyc_Absyn_valueof_type(e2);
goto _LL0;}default: _LL9:
 goto _LL0;}_LL0:;}
# 372
return t;}
# 377
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
({void*_tmp6FD=Cyc_Parse_substitute_tags(tags,x->type);x->type=_tmp6FD;});}struct _tuple20{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple21{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 384
static struct _tuple20*Cyc_Parse_get_tqual_typ(unsigned loc,struct _tuple21*t){
return({struct _tuple20*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->f1=(*t).f2,_tmp57->f2=(*t).f3;_tmp57;});}
# 388
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp58=tm;if(*((int*)_tmp58)== 4)
return 1;else{
return 0;};}
# 397
static void*Cyc_Parse_id2type(struct _fat_ptr s,void*k){
if(({struct _fat_ptr _tmp6FE=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp6FE,({const char*_tmp59="`H";_tag_fat(_tmp59,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_heap_rgn_type;
if(({struct _fat_ptr _tmp6FF=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp6FF,({const char*_tmp5A="`U";_tag_fat(_tmp5A,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_unique_rgn_type;
if(({struct _fat_ptr _tmp700=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp700,({const char*_tmp5B="`RC";_tag_fat(_tmp5B,sizeof(char),4U);}));})== 0)
return Cyc_Absyn_refcnt_rgn_type;
if(Cyc_zstrcmp((struct _fat_ptr)s,(struct _fat_ptr)Cyc_CurRgn_curr_rgn_name)== 0)
return Cyc_CurRgn_curr_rgn_type();
return Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp5D=_cycalloc(sizeof(*_tmp5D));({struct _fat_ptr*_tmp701=({struct _fat_ptr*_tmp5C=_cycalloc(sizeof(*_tmp5C));*_tmp5C=s;_tmp5C;});_tmp5D->name=_tmp701;}),_tmp5D->identity=-1,_tmp5D->kind=k;_tmp5D;}));}
# 409
static void Cyc_Parse_tvar_ok(struct _fat_ptr s,unsigned loc){
if(({struct _fat_ptr _tmp702=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp702,({const char*_tmp5E="`H";_tag_fat(_tmp5E,sizeof(char),3U);}));})== 0)
({void*_tmp5F=0U;({unsigned _tmp704=loc;struct _fat_ptr _tmp703=({const char*_tmp60="bad occurrence of heap region";_tag_fat(_tmp60,sizeof(char),30U);});Cyc_Warn_err(_tmp704,_tmp703,_tag_fat(_tmp5F,sizeof(void*),0));});});
if(({struct _fat_ptr _tmp705=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp705,({const char*_tmp61="`U";_tag_fat(_tmp61,sizeof(char),3U);}));})== 0)
({void*_tmp62=0U;({unsigned _tmp707=loc;struct _fat_ptr _tmp706=({const char*_tmp63="bad occurrence of unique region";_tag_fat(_tmp63,sizeof(char),32U);});Cyc_Warn_err(_tmp707,_tmp706,_tag_fat(_tmp62,sizeof(void*),0));});});
if(({struct _fat_ptr _tmp708=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp708,({const char*_tmp64="`RC";_tag_fat(_tmp64,sizeof(char),4U);}));})== 0)
({void*_tmp65=0U;({unsigned _tmp70A=loc;struct _fat_ptr _tmp709=({const char*_tmp66="bad occurrence of refcounted region";_tag_fat(_tmp66,sizeof(char),36U);});Cyc_Warn_err(_tmp70A,_tmp709,_tag_fat(_tmp65,sizeof(void*),0));});});
if(Cyc_zstrcmp((struct _fat_ptr)s,(struct _fat_ptr)Cyc_CurRgn_curr_rgn_name)== 0)
({void*_tmp67=0U;({unsigned _tmp70C=loc;struct _fat_ptr _tmp70B=({const char*_tmp68="bad occurrence of \"current\" region";_tag_fat(_tmp68,sizeof(char),35U);});Cyc_Warn_err(_tmp70C,_tmp70B,_tag_fat(_tmp67,sizeof(void*),0));});});}
# 424
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned loc,void*t){
void*_tmp69=t;void*_tmp6A;if(*((int*)_tmp69)== 2){_tmp6A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp69)->f1;{struct Cyc_Absyn_Tvar*pr=_tmp6A;
return pr;}}else{
({int(*_tmp70E)(unsigned,struct _fat_ptr)=({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;});unsigned _tmp70D=loc;_tmp70E(_tmp70D,({const char*_tmp6B="expecting a list of type variables, not types";_tag_fat(_tmp6B,sizeof(char),46U);}));});};}
# 432
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmp6C=Cyc_Absyn_compress(t);void*_stmttmp5=_tmp6C;void*_tmp6D=_stmttmp5;void*_tmp6E;if(*((int*)_tmp6D)== 2){_tmp6E=(void**)&(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6D)->f1)->kind;{void**cptr=(void**)_tmp6E;
# 435
void*_tmp6F=Cyc_Kinds_compress_kb(*cptr);void*_stmttmp6=_tmp6F;void*_tmp70=_stmttmp6;if(*((int*)_tmp70)== 1){
# 437
({void*_tmp70F=leq?(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->tag=2,_tmp71->f1=0,_tmp71->f2=k;_tmp71;}): Cyc_Kinds_kind_to_bound(k);*cptr=_tmp70F;});
return;}else{
return;};}}else{
# 441
return;};}
# 446
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned loc){
# 454
if(tms == 0)return 0;{
# 456
void*_tmp72=(void*)tms->hd;void*_stmttmp7=_tmp72;void*_tmp73=_stmttmp7;void*_tmp74;if(*((int*)_tmp73)== 3){_tmp74=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp73)->f1;{void*args=_tmp74;
# 459
if(tms->tl == 0 ||
 Cyc_Parse_is_typeparam((void*)(_check_null(tms->tl))->hd)&&(_check_null(tms->tl))->tl == 0){
# 462
void*_tmp75=args;void*_tmp76;if(*((int*)_tmp75)== 1){
# 464
({void*_tmp77=0U;({unsigned _tmp711=loc;struct _fat_ptr _tmp710=({const char*_tmp78="function declaration with both new- and old-style parameter declarations; ignoring old-style";_tag_fat(_tmp78,sizeof(char),93U);});Cyc_Warn_warn(_tmp711,_tmp710,_tag_fat(_tmp77,sizeof(void*),0));});});
# 466
return tms;}else{_tmp76=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp75)->f1;{struct Cyc_List_List*ids=_tmp76;
# 468
if(({int _tmp712=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(ids);_tmp712 != ({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(tds);}))
({int(*_tmp714)(unsigned,struct _fat_ptr)=({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;});unsigned _tmp713=loc;_tmp714(_tmp713,({const char*_tmp79="wrong number of parameter declarations in old-style function declaration";_tag_fat(_tmp79,sizeof(char),73U);}));});{
# 472
struct Cyc_List_List*rev_new_params=0;
for(1;ids != 0;ids=ids->tl){
struct Cyc_List_List*tds2=tds;
for(1;tds2 != 0;tds2=tds2->tl){
struct Cyc_Absyn_Decl*_tmp7A=(struct Cyc_Absyn_Decl*)tds2->hd;struct Cyc_Absyn_Decl*x=_tmp7A;
void*_tmp7B=x->r;void*_stmttmp8=_tmp7B;void*_tmp7C=_stmttmp8;void*_tmp7D;if(*((int*)_tmp7C)== 0){_tmp7D=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp7C)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp7D;
# 479
if(Cyc_zstrptrcmp((*vd->name).f2,(struct _fat_ptr*)ids->hd)!= 0)
continue;
if(vd->initializer != 0)
({int(*_tmp716)(unsigned,struct _fat_ptr)=({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;});unsigned _tmp715=x->loc;_tmp716(_tmp715,({const char*_tmp7E="initializer found in parameter declaration";_tag_fat(_tmp7E,sizeof(char),43U);}));});
if(Cyc_Absyn_is_qvar_qualified(vd->name))
({int(*_tmp718)(unsigned,struct _fat_ptr)=({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;});unsigned _tmp717=x->loc;_tmp718(_tmp717,({const char*_tmp7F="namespaces forbidden in parameter declarations";_tag_fat(_tmp7F,sizeof(char),47U);}));});
rev_new_params=({struct Cyc_List_List*_tmp81=_cycalloc(sizeof(*_tmp81));({struct _tuple8*_tmp719=({struct _tuple8*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->f1=(*vd->name).f2,_tmp80->f2=vd->tq,_tmp80->f3=vd->type;_tmp80;});_tmp81->hd=_tmp719;}),_tmp81->tl=rev_new_params;_tmp81;});
# 487
goto L;}}else{
({int(*_tmp71B)(unsigned,struct _fat_ptr)=({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;});unsigned _tmp71A=x->loc;_tmp71B(_tmp71A,({const char*_tmp82="nonvariable declaration in parameter type";_tag_fat(_tmp82,sizeof(char),42U);}));});};}
# 491
L: if(tds2 == 0)
({int(*_tmp71E)(unsigned,struct _fat_ptr)=({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;});unsigned _tmp71D=loc;_tmp71E(_tmp71D,(struct _fat_ptr)({struct _fat_ptr _tmp71C=(struct _fat_ptr)*((struct _fat_ptr*)ids->hd);Cyc_strconcat(_tmp71C,({const char*_tmp83=" is not given a type";_tag_fat(_tmp83,sizeof(char),21U);}));}));});}
# 494
return({struct Cyc_List_List*_tmp86=_region_malloc(yy,sizeof(*_tmp86));
({void*_tmp721=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp85=_region_malloc(yy,sizeof(*_tmp85));_tmp85->tag=3,({void*_tmp720=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp84=_region_malloc(yy,sizeof(*_tmp84));_tmp84->tag=1,({struct Cyc_List_List*_tmp71F=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(rev_new_params);_tmp84->f1=_tmp71F;}),_tmp84->f2=0,_tmp84->f3=0,_tmp84->f4=0,_tmp84->f5=0,_tmp84->f6=0,_tmp84->f7=0;_tmp84;});_tmp85->f1=_tmp720;});_tmp85;});_tmp86->hd=_tmp721;}),_tmp86->tl=0;_tmp86;});}}};}
# 501
goto _LL4;}}else{_LL4:
 return({struct Cyc_List_List*_tmp87=_region_malloc(yy,sizeof(*_tmp87));_tmp87->hd=(void*)tms->hd,({struct Cyc_List_List*_tmp722=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc);_tmp87->tl=_tmp722;});_tmp87;});};}}
# 509
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned loc){
# 513
if(tds != 0)
d=({struct Cyc_Parse_Declarator _tmp645;_tmp645.id=d.id,_tmp645.varloc=d.varloc,({struct Cyc_List_List*_tmp723=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc);_tmp645.tms=_tmp723;});_tmp645;});{
# 516
enum Cyc_Absyn_Scope sc=2U;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 522
if(dso != 0){
tss=dso->type_specs;
tq=dso->tq;
is_inline=dso->is_inline;
atts=dso->attributes;{
# 528
enum Cyc_Parse_Storage_class _tmp88=dso->sc;enum Cyc_Parse_Storage_class _stmttmp9=_tmp88;enum Cyc_Parse_Storage_class _tmp89=_stmttmp9;switch((int)_tmp89){case Cyc_Parse_None_sc:
 goto _LL0;case Cyc_Parse_Extern_sc:
 sc=3U;goto _LL0;case Cyc_Parse_Static_sc:
 sc=0U;goto _LL0;default:
({void*_tmp8A=0U;({unsigned _tmp725=loc;struct _fat_ptr _tmp724=({const char*_tmp8B="bad storage class on function";_tag_fat(_tmp8B,sizeof(char),30U);});Cyc_Warn_err(_tmp725,_tmp724,_tag_fat(_tmp8A,sizeof(void*),0));});});goto _LL0;}_LL0:;}}{
# 535
void*_tmp8C=Cyc_Parse_collapse_type_specifiers(tss,loc);void*t=_tmp8C;
struct _tuple14 _tmp8D=Cyc_Parse_apply_tms(tq,t,atts,d.tms);struct _tuple14 _stmttmpA=_tmp8D;struct _tuple14 _tmp8E=_stmttmpA;void*_tmp92;void*_tmp91;void*_tmp90;struct Cyc_Absyn_Tqual _tmp8F;_tmp8F=_tmp8E.f1;_tmp90=_tmp8E.f2;_tmp91=_tmp8E.f3;_tmp92=_tmp8E.f4;{struct Cyc_Absyn_Tqual fn_tqual=_tmp8F;void*fn_type=_tmp90;struct Cyc_List_List*x=_tmp91;struct Cyc_List_List*out_atts=_tmp92;
# 540
if(x != 0)
({void*_tmp93=0U;({unsigned _tmp727=loc;struct _fat_ptr _tmp726=({const char*_tmp94="bad type params, ignoring";_tag_fat(_tmp94,sizeof(char),26U);});Cyc_Warn_warn(_tmp727,_tmp726,_tag_fat(_tmp93,sizeof(void*),0));});});{
# 543
void*_tmp95=fn_type;struct Cyc_Absyn_FnInfo _tmp96;if(*((int*)_tmp95)== 5){_tmp96=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95)->f1;{struct Cyc_Absyn_FnInfo i=_tmp96;
# 545
{struct Cyc_List_List*_tmp97=i.args;struct Cyc_List_List*args2=_tmp97;for(0;args2 != 0;args2=args2->tl){
if((*((struct _tuple8*)args2->hd)).f1 == 0){
({void*_tmp98=0U;({unsigned _tmp729=loc;struct _fat_ptr _tmp728=({const char*_tmp99="missing argument variable in function prototype";_tag_fat(_tmp99,sizeof(char),48U);});Cyc_Warn_err(_tmp729,_tmp728,_tag_fat(_tmp98,sizeof(void*),0));});});
({struct _fat_ptr*_tmp72B=({struct _fat_ptr*_tmp9B=_cycalloc(sizeof(*_tmp9B));({struct _fat_ptr _tmp72A=({const char*_tmp9A="?";_tag_fat(_tmp9A,sizeof(char),2U);});*_tmp9B=_tmp72A;});_tmp9B;});(*((struct _tuple8*)args2->hd)).f1=_tmp72B;});}}}
# 552
({struct Cyc_List_List*_tmp72C=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(i.attributes,out_atts);i.attributes=_tmp72C;});
return({struct Cyc_Absyn_Fndecl*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->sc=sc,_tmp9C->is_inline=is_inline,_tmp9C->name=d.id,_tmp9C->body=body,_tmp9C->i=i,_tmp9C->cached_type=0,_tmp9C->param_vardecls=0,_tmp9C->fn_vardecl=0,_tmp9C->orig_scope=sc;_tmp9C;});}}else{
# 556
({int(*_tmp72E)(unsigned,struct _fat_ptr)=({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;});unsigned _tmp72D=loc;_tmp72E(_tmp72D,({const char*_tmp9D="declarator is not a function prototype";_tag_fat(_tmp9D,sizeof(char),39U);}));});};}}}}}static char _tmp9E[76U]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 560
static struct _fat_ptr Cyc_Parse_msg1={_tmp9E,_tmp9E,_tmp9E + 76U};static char _tmp9F[84U]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 562
static struct _fat_ptr Cyc_Parse_msg2={_tmp9F,_tmp9F,_tmp9F + 84U};
# 569
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 572
if(s1.Signed_spec && s2.Signed_spec)
({void*_tmpA0=0U;Cyc_Warn_warn(loc,Cyc_Parse_msg2,_tag_fat(_tmpA0,sizeof(void*),0));});
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec && s2.Unsigned_spec)
({void*_tmpA1=0U;Cyc_Warn_warn(loc,Cyc_Parse_msg2,_tag_fat(_tmpA1,sizeof(void*),0));});
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec && s2.Short_spec)
({void*_tmpA2=0U;({unsigned _tmp730=loc;struct _fat_ptr _tmp72F=({const char*_tmpA3="too many occurrences of short in specifiers";_tag_fat(_tmpA3,sizeof(char),44U);});Cyc_Warn_warn(_tmp730,_tmp72F,_tag_fat(_tmpA2,sizeof(void*),0));});});
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec && s2.Long_Long_spec ||
 s1.Long_Long_spec && s2.Long_spec)||
 s2.Long_Long_spec && s1.Long_spec)
({void*_tmpA4=0U;({unsigned _tmp732=loc;struct _fat_ptr _tmp731=({const char*_tmpA5="too many occurrences of long in specifiers";_tag_fat(_tmpA5,sizeof(char),43U);});Cyc_Warn_warn(_tmp732,_tmp731,_tag_fat(_tmpA4,sizeof(void*),0));});});
s1.Long_Long_spec=
(s1.Long_Long_spec || s2.Long_Long_spec)|| s1.Long_spec && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec &&(s1.Long_spec || s2.Long_spec);
if(s1.Valid_type_spec && s2.Valid_type_spec)
({void*_tmpA6=0U;Cyc_Warn_err(loc,Cyc_Parse_msg1,_tag_fat(_tmpA6,sizeof(void*),0));});else{
if(s2.Valid_type_spec){
s1.Type_spec=s2.Type_spec;
s1.Valid_type_spec=1;}}
# 594
return s1;}
# 600
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned loc){
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec || ts.Unsigned_spec;
int seen_size=(ts.Short_spec || ts.Long_spec)|| ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec: Cyc_Absyn_void_type;
enum Cyc_Absyn_Size_of sz=2U;
enum Cyc_Absyn_Sign sgn=0U;
# 608
if(seen_size || seen_sign){
if(ts.Signed_spec && ts.Unsigned_spec)
({void*_tmpA7=0U;Cyc_Warn_err(loc,Cyc_Parse_msg2,_tag_fat(_tmpA7,sizeof(void*),0));});
if(ts.Unsigned_spec)sgn=1U;
if(ts.Short_spec &&(ts.Long_spec || ts.Long_Long_spec)||
 ts.Long_spec && ts.Long_Long_spec)
({void*_tmpA8=0U;Cyc_Warn_err(loc,Cyc_Parse_msg2,_tag_fat(_tmpA8,sizeof(void*),0));});
if(ts.Short_spec)sz=1U;
if(ts.Long_spec)sz=3U;
if(ts.Long_Long_spec)sz=4U;}
# 621
if(!seen_type){
if(!seen_sign && !seen_size)
({void*_tmpA9=0U;({unsigned _tmp734=loc;struct _fat_ptr _tmp733=({const char*_tmpAA="missing type within specifier";_tag_fat(_tmpAA,sizeof(char),30U);});Cyc_Warn_warn(_tmp734,_tmp733,_tag_fat(_tmpA9,sizeof(void*),0));});});
return Cyc_Absyn_int_type(sgn,sz);}
# 626
{void*_tmpAB=t;enum Cyc_Absyn_Size_of _tmpAD;enum Cyc_Absyn_Sign _tmpAC;if(*((int*)_tmpAB)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpAB)->f1)){case 1: _tmpAC=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpAB)->f1)->f1;_tmpAD=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpAB)->f1)->f2;{enum Cyc_Absyn_Sign sgn2=_tmpAC;enum Cyc_Absyn_Size_of sz2=_tmpAD;
# 628
if(seen_sign &&(int)sgn2 != (int)sgn){
sgn2=sgn;
t=Cyc_Absyn_int_type(sgn,sz2);}
# 632
if(seen_size &&(int)sz2 != (int)sz)
t=Cyc_Absyn_int_type(sgn2,sz);
goto _LL0;}case 2:
# 636
 if(seen_size)
t=Cyc_Absyn_long_double_type;
goto _LL0;default: goto _LL5;}else{_LL5:
# 640
 if(seen_sign)
({void*_tmpAE=0U;({unsigned _tmp736=loc;struct _fat_ptr _tmp735=({const char*_tmpAF="sign specification on non-integral type";_tag_fat(_tmpAF,sizeof(char),40U);});Cyc_Warn_err(_tmp736,_tmp735,_tag_fat(_tmpAE,sizeof(void*),0));});});
if(seen_size)
({void*_tmpB0=0U;({unsigned _tmp738=loc;struct _fat_ptr _tmp737=({const char*_tmpB1="size qualifier on non-integral type";_tag_fat(_tmpB1,sizeof(char),36U);});Cyc_Warn_err(_tmp738,_tmp737,_tag_fat(_tmpB0,sizeof(void*),0));});});
goto _LL0;}_LL0:;}
# 646
return t;}
# 649
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple13*ds,struct Cyc_List_List*shared_atts){
# 653
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmpB2=d.id;struct _tuple0*q=_tmpB2;
unsigned _tmpB3=d.varloc;unsigned varloc=_tmpB3;
struct _tuple14 _tmpB4=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);struct _tuple14 _stmttmpB=_tmpB4;struct _tuple14 _tmpB5=_stmttmpB;void*_tmpB9;void*_tmpB8;void*_tmpB7;struct Cyc_Absyn_Tqual _tmpB6;_tmpB6=_tmpB5.f1;_tmpB7=_tmpB5.f2;_tmpB8=_tmpB5.f3;_tmpB9=_tmpB5.f4;{struct Cyc_Absyn_Tqual tq2=_tmpB6;void*new_typ=_tmpB7;struct Cyc_List_List*tvs=_tmpB8;struct Cyc_List_List*atts=_tmpB9;
# 660
struct Cyc_List_List*_tmpBA=ds->tl == 0?0:({struct _RegionHandle*_tmp73C=r;struct Cyc_Absyn_Tqual _tmp73B=tq;void*_tmp73A=Cyc_Tcutil_copy_type(t);struct _tuple13*_tmp739=ds->tl;Cyc_Parse_apply_tmss(_tmp73C,_tmp73B,_tmp73A,_tmp739,shared_atts);});struct Cyc_List_List*tl=_tmpBA;
return({struct Cyc_List_List*_tmpBC=_region_malloc(r,sizeof(*_tmpBC));({struct _tuple16*_tmp73D=({struct _tuple16*_tmpBB=_region_malloc(r,sizeof(*_tmpBB));_tmpBB->f1=varloc,_tmpBB->f2=q,_tmpBB->f3=tq2,_tmpBB->f4=new_typ,_tmpBB->f5=tvs,_tmpBB->f6=atts;_tmpBB;});_tmpBC->hd=_tmp73D;}),_tmpBC->tl=tl;_tmpBC;});}}}
# 664
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 667
if(tms == 0)return({struct _tuple14 _tmp646;_tmp646.f1=tq,_tmp646.f2=t,_tmp646.f3=0,_tmp646.f4=atts;_tmp646;});{
void*_tmpBD=(void*)tms->hd;void*_stmttmpC=_tmpBD;void*_tmpBE=_stmttmpC;struct Cyc_Absyn_Tqual _tmpC2;struct Cyc_Absyn_PtrAtts _tmpC1;void*_tmpC3;unsigned _tmpBF;void*_tmpC0;switch(*((int*)_tmpBE)){case 0: _tmpC0=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpBE)->f1;_tmpBF=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpBE)->f2;{void*zeroterm=_tmpC0;unsigned ztloc=_tmpBF;
# 670
return({struct Cyc_Absyn_Tqual _tmp740=Cyc_Absyn_empty_tqual(0U);void*_tmp73F=
Cyc_Absyn_array_type(t,tq,0,zeroterm,ztloc);
# 670
struct Cyc_List_List*_tmp73E=atts;Cyc_Parse_apply_tms(_tmp740,_tmp73F,_tmp73E,tms->tl);});}case 1: _tmpC0=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpBE)->f1;_tmpC3=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpBE)->f2;_tmpBF=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpBE)->f3;{struct Cyc_Absyn_Exp*e=_tmpC0;void*zeroterm=_tmpC3;unsigned ztloc=_tmpBF;
# 673
return({struct Cyc_Absyn_Tqual _tmp743=Cyc_Absyn_empty_tqual(0U);void*_tmp742=
Cyc_Absyn_array_type(t,tq,e,zeroterm,ztloc);
# 673
struct Cyc_List_List*_tmp741=atts;Cyc_Parse_apply_tms(_tmp743,_tmp742,_tmp741,tms->tl);});}case 3: _tmpC0=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpBE)->f1;{void*args=_tmpC0;
# 676
void*_tmpC4=args;unsigned _tmpC5;void*_tmpCC;void*_tmpCB;void*_tmpCA;void*_tmpC9;void*_tmpC8;int _tmpC7;void*_tmpC6;if(*((int*)_tmpC4)== 1){_tmpC6=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpC4)->f1;_tmpC7=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpC4)->f2;_tmpC8=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpC4)->f3;_tmpC9=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpC4)->f4;_tmpCA=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpC4)->f5;_tmpCB=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpC4)->f6;_tmpCC=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpC4)->f7;{struct Cyc_List_List*args2=_tmpC6;int c_vararg=_tmpC7;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmpC8;void*eff=_tmpC9;struct Cyc_List_List*rgn_po=_tmpCA;struct Cyc_Absyn_Exp*req=_tmpCB;struct Cyc_Absyn_Exp*ens=_tmpCC;
# 678
struct Cyc_List_List*typvars=0;
# 680
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Atts_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD->hd=(void*)as->hd,_tmpCD->tl=fn_atts;_tmpCD;});else{
# 685
new_atts=({struct Cyc_List_List*_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE->hd=(void*)as->hd,_tmpCE->tl=new_atts;_tmpCE;});}}}
# 687
if(tms->tl != 0){
void*_tmpCF=(void*)(_check_null(tms->tl))->hd;void*_stmttmpD=_tmpCF;void*_tmpD0=_stmttmpD;void*_tmpD1;if(*((int*)_tmpD0)== 4){_tmpD1=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpD0)->f1;{struct Cyc_List_List*ts=_tmpD1;
# 690
typvars=ts;
tms=tms->tl;
goto _LL12;}}else{
goto _LL12;}_LL12:;}
# 696
if(((((!c_vararg && cyc_vararg == 0)&& args2 != 0)&& args2->tl == 0)&&(*((struct _tuple8*)args2->hd)).f1 == 0)&&(*((struct _tuple8*)args2->hd)).f3 == Cyc_Absyn_void_type)
# 701
args2=0;{
# 706
struct Cyc_List_List*new_requires=0;
{struct Cyc_List_List*_tmpD2=args2;struct Cyc_List_List*a=_tmpD2;for(0;a != 0;a=a->tl){
struct _tuple8*_tmpD3=(struct _tuple8*)a->hd;struct _tuple8*_stmttmpE=_tmpD3;struct _tuple8*_tmpD4=_stmttmpE;void*_tmpD7;struct Cyc_Absyn_Tqual _tmpD6;void*_tmpD5;_tmpD5=_tmpD4->f1;_tmpD6=_tmpD4->f2;_tmpD7=(void**)& _tmpD4->f3;{struct _fat_ptr*vopt=_tmpD5;struct Cyc_Absyn_Tqual tq=_tmpD6;void**t=(void**)_tmpD7;
void*_tmpD8=*t;void*_stmttmpF=_tmpD8;void*_tmpD9=_stmttmpF;unsigned _tmpDE;void*_tmpDD;void*_tmpDC;struct Cyc_Absyn_Tqual _tmpDB;void*_tmpDA;if(*((int*)_tmpD9)== 4){_tmpDA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD9)->f1).elt_type;_tmpDB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD9)->f1).tq;_tmpDC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD9)->f1).num_elts;_tmpDD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD9)->f1).zero_term;_tmpDE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD9)->f1).zt_loc;{void*et=_tmpDA;struct Cyc_Absyn_Tqual tq=_tmpDB;struct Cyc_Absyn_Exp*neltsopt=_tmpDC;void*zt=_tmpDD;unsigned ztloc=_tmpDE;
# 711
if(neltsopt != 0 && vopt != 0){
struct _tuple0*v=({struct _tuple0*_tmpE1=_cycalloc(sizeof(*_tmpE1));((_tmpE1->f1).Loc_n).tag=4U,((_tmpE1->f1).Loc_n).val=0,_tmpE1->f2=vopt;_tmpE1;});
struct Cyc_Absyn_Exp*nelts=Cyc_Absyn_copy_exp(neltsopt);
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_primop_exp(18U,({struct Cyc_Absyn_Exp*_tmpE0[1];({struct Cyc_Absyn_Exp*_tmp744=Cyc_Absyn_var_exp(v,0U);_tmpE0[0]=_tmp744;});({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_tmpE0,sizeof(struct Cyc_Absyn_Exp*),1));}),0U);
struct Cyc_Absyn_Exp*new_req=Cyc_Absyn_lte_exp(nelts,e2,0U);
new_requires=({struct Cyc_List_List*_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF->hd=new_req,_tmpDF->tl=new_requires;_tmpDF;});}
# 718
goto _LL1A;}}else{
goto _LL1A;}_LL1A:;}}}
# 722
if(new_requires != 0){
struct Cyc_Absyn_Exp*r;
if(req != 0)
r=req;else{
# 727
r=(struct Cyc_Absyn_Exp*)new_requires->hd;
new_requires=new_requires->tl;}
# 730
for(1;new_requires != 0;new_requires=new_requires->tl){
r=Cyc_Absyn_and_exp(r,(struct Cyc_Absyn_Exp*)new_requires->hd,0U);}
req=r;}{
# 736
struct Cyc_List_List*_tmpE2=Cyc_Parse_get_arg_tags(args2);struct Cyc_List_List*tags=_tmpE2;
# 738
if(tags != 0)
t=Cyc_Parse_substitute_tags(tags,t);
t=Cyc_Parse_array2ptr(t,0);
# 743
{struct Cyc_List_List*_tmpE3=args2;struct Cyc_List_List*a=_tmpE3;for(0;a != 0;a=a->tl){
struct _tuple8*_tmpE4=(struct _tuple8*)a->hd;struct _tuple8*_stmttmp10=_tmpE4;struct _tuple8*_tmpE5=_stmttmp10;void*_tmpE8;struct Cyc_Absyn_Tqual _tmpE7;void*_tmpE6;_tmpE6=_tmpE5->f1;_tmpE7=_tmpE5->f2;_tmpE8=(void**)& _tmpE5->f3;{struct _fat_ptr*vopt=_tmpE6;struct Cyc_Absyn_Tqual tq=_tmpE7;void**t=(void**)_tmpE8;
if(tags != 0)
({void*_tmp745=Cyc_Parse_substitute_tags(tags,*t);*t=_tmp745;});
({void*_tmp746=Cyc_Parse_array2ptr(*t,1);*t=_tmp746;});}}}
# 755
return({struct Cyc_Absyn_Tqual _tmp749=Cyc_Absyn_empty_tqual(tq.loc);void*_tmp748=
Cyc_Absyn_function_type(typvars,eff,tq,t,args2,c_vararg,cyc_vararg,rgn_po,fn_atts,req,ens);
# 755
struct Cyc_List_List*_tmp747=new_atts;Cyc_Parse_apply_tms(_tmp749,_tmp748,_tmp747,(
# 760
_check_null(tms))->tl);});}}}}else{_tmpC5=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpC4)->f2;{unsigned loc=_tmpC5;
# 762
({int(*_tmp74B)(unsigned,struct _fat_ptr)=({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;});unsigned _tmp74A=loc;_tmp74B(_tmp74A,({const char*_tmpE9="function declaration without parameter types";_tag_fat(_tmpE9,sizeof(char),45U);}));});}};}case 4: _tmpC0=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpBE)->f1;_tmpBF=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpBE)->f2;{struct Cyc_List_List*ts=_tmpC0;unsigned loc=_tmpBF;
# 769
if(tms->tl == 0)
return({struct _tuple14 _tmp647;_tmp647.f1=tq,_tmp647.f2=t,_tmp647.f3=ts,_tmp647.f4=atts;_tmp647;});
# 774
({int(*_tmp74D)(unsigned,struct _fat_ptr)=({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;});unsigned _tmp74C=loc;_tmp74D(_tmp74C,({const char*_tmpEA="type parameters must appear before function arguments in declarator";_tag_fat(_tmpEA,sizeof(char),68U);}));});}case 2: _tmpC1=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpBE)->f1;_tmpC2=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpBE)->f2;{struct Cyc_Absyn_PtrAtts ptratts=_tmpC1;struct Cyc_Absyn_Tqual tq2=_tmpC2;
# 777
return({struct Cyc_Absyn_Tqual _tmp750=tq2;void*_tmp74F=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp648;_tmp648.elt_type=t,_tmp648.elt_tq=tq,_tmp648.ptr_atts=ptratts;_tmp648;}));struct Cyc_List_List*_tmp74E=atts;Cyc_Parse_apply_tms(_tmp750,_tmp74F,_tmp74E,tms->tl);});}default: _tmpBF=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpBE)->f1;_tmpC0=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpBE)->f2;{unsigned loc=_tmpBF;struct Cyc_List_List*atts2=_tmpC0;
# 782
return({struct Cyc_Absyn_Tqual _tmp753=tq;void*_tmp752=t;struct Cyc_List_List*_tmp751=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(atts,atts2);Cyc_Parse_apply_tms(_tmp753,_tmp752,_tmp751,tms->tl);});}};}}
# 788
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 797
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned loc,struct _tuple16*t){
struct _tuple16*_tmpEB=t;void*_tmpF1;void*_tmpF0;void*_tmpEF;struct Cyc_Absyn_Tqual _tmpEE;void*_tmpED;unsigned _tmpEC;_tmpEC=_tmpEB->f1;_tmpED=_tmpEB->f2;_tmpEE=_tmpEB->f3;_tmpEF=_tmpEB->f4;_tmpF0=_tmpEB->f5;_tmpF1=_tmpEB->f6;{unsigned varloc=_tmpEC;struct _tuple0*x=_tmpED;struct Cyc_Absyn_Tqual tq=_tmpEE;void*typ=_tmpEF;struct Cyc_List_List*tvs=_tmpF0;struct Cyc_List_List*atts=_tmpF1;
# 800
Cyc_Lex_register_typedef(x);{
# 802
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmpF2=typ;void*_tmpF3;if(*((int*)_tmpF2)== 1){_tmpF3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpF2)->f1;{struct Cyc_Core_Opt*kopt=_tmpF3;
# 806
type=0;
kind=kopt == 0?& Cyc_Kinds_bko: kopt;
goto _LL3;}}else{
kind=0;type=typ;goto _LL3;}_LL3:;}
# 811
return({void*_tmp755=(void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5->tag=8,({struct Cyc_Absyn_Typedefdecl*_tmp754=({struct Cyc_Absyn_Typedefdecl*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->name=x,_tmpF4->tvs=tvs,_tmpF4->kind=kind,_tmpF4->defn=type,_tmpF4->atts=atts,_tmpF4->tq=tq,_tmpF4->extern_c=0;_tmpF4;});_tmpF5->f1=_tmp754;});_tmpF5;});Cyc_Absyn_new_decl(_tmp755,loc);});}}}
# 818
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
return({void*_tmp756=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6->tag=12,_tmpF6->f1=d,_tmpF6->f2=s;_tmpF6;});Cyc_Absyn_new_stmt(_tmp756,d->loc);});}
# 822
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return({(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*,struct Cyc_Absyn_Stmt*))Cyc_List_fold_right;})(Cyc_Parse_flatten_decl,ds,s);}
# 826
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple11*ds,struct _tuple13**decls,struct Cyc_List_List**es){
# 829
struct _tuple13*declarators=0;
struct Cyc_List_List*exprs=0;
for(1;ds != 0;ds=ds->tl){
struct _tuple12 _tmpF7=ds->hd;struct _tuple12 _stmttmp11=_tmpF7;struct _tuple12 _tmpF8=_stmttmp11;void*_tmpFA;struct Cyc_Parse_Declarator _tmpF9;_tmpF9=_tmpF8.f1;_tmpFA=_tmpF8.f2;{struct Cyc_Parse_Declarator d=_tmpF9;struct Cyc_Absyn_Exp*e=_tmpFA;
declarators=({struct _tuple13*_tmpFB=_region_malloc(r,sizeof(*_tmpFB));_tmpFB->tl=declarators,_tmpFB->hd=d;_tmpFB;});
exprs=({struct Cyc_List_List*_tmpFC=_region_malloc(r,sizeof(*_tmpFC));_tmpFC->hd=e,_tmpFC->tl=exprs;_tmpFC;});}}
# 836
({struct Cyc_List_List*_tmp757=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(exprs);*es=_tmp757;});
({struct _tuple13*_tmp758=({(struct _tuple13*(*)(struct _tuple13*))Cyc_Parse_flat_imp_rev;})(declarators);*decls=_tmp758;});}
# 845
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple11*ids,unsigned tqual_loc,unsigned loc){
# 848
struct _RegionHandle _tmpFD=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmpFD;_push_region(mkrgn);
{struct Cyc_Parse_Declaration_spec _tmpFE=ds;void*_tmp101;struct Cyc_Parse_Type_specifier _tmp100;struct Cyc_Absyn_Tqual _tmpFF;_tmpFF=_tmpFE.tq;_tmp100=_tmpFE.type_specs;_tmp101=_tmpFE.attributes;{struct Cyc_Absyn_Tqual tq=_tmpFF;struct Cyc_Parse_Type_specifier tss=_tmp100;struct Cyc_List_List*atts=_tmp101;
if(tq.loc == 0U)tq.loc=tqual_loc;
if(ds.is_inline)
({void*_tmp102=0U;({unsigned _tmp75A=loc;struct _fat_ptr _tmp759=({const char*_tmp103="inline qualifier on non-function definition";_tag_fat(_tmp103,sizeof(char),44U);});Cyc_Warn_warn(_tmp75A,_tmp759,_tag_fat(_tmp102,sizeof(void*),0));});});{
# 854
enum Cyc_Absyn_Scope s=2U;
int istypedef=0;
{enum Cyc_Parse_Storage_class _tmp104=ds.sc;enum Cyc_Parse_Storage_class _stmttmp12=_tmp104;enum Cyc_Parse_Storage_class _tmp105=_stmttmp12;switch((int)_tmp105){case Cyc_Parse_Typedef_sc:
 istypedef=1;goto _LL3;case Cyc_Parse_Extern_sc:
 s=3U;goto _LL3;case Cyc_Parse_ExternC_sc:
 s=4U;goto _LL3;case Cyc_Parse_Static_sc:
 s=0U;goto _LL3;case Cyc_Parse_Auto_sc:
 s=2U;goto _LL3;case Cyc_Parse_Register_sc:
 s=Cyc_Flags_no_register?2U: 5U;goto _LL3;case Cyc_Parse_Abstract_sc:
 s=1U;goto _LL3;default:
 goto _LL3;}_LL3:;}{
# 870
struct _tuple13*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 874
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es != 0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;
break;}}}{
# 882
void*_tmp106=Cyc_Parse_collapse_type_specifiers(tss,loc);void*base_type=_tmp106;
if(declarators == 0){
# 886
void*_tmp107=base_type;int _tmp109;void*_tmp10A;enum Cyc_Absyn_AggrKind _tmp10B;void*_tmp108;switch(*((int*)_tmp107)){case 10: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp107)->f1)->r)){case 0: _tmp108=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp107)->f1)->r)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmp108;
# 888
({struct Cyc_List_List*_tmp75B=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(ad->attributes,atts);ad->attributes=_tmp75B;});
ad->sc=s;{
struct Cyc_List_List*_tmp10E=({struct Cyc_List_List*_tmp10D=_cycalloc(sizeof(*_tmp10D));({struct Cyc_Absyn_Decl*_tmp75D=({void*_tmp75C=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C->tag=5,_tmp10C->f1=ad;_tmp10C;});Cyc_Absyn_new_decl(_tmp75C,loc);});_tmp10D->hd=_tmp75D;}),_tmp10D->tl=0;_tmp10D;});_npop_handler(0);return _tmp10E;}}case 1: _tmp108=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp107)->f1)->r)->f1;{struct Cyc_Absyn_Enumdecl*ed=_tmp108;
# 892
if(atts != 0)({void*_tmp10F=0U;({unsigned _tmp75F=loc;struct _fat_ptr _tmp75E=({const char*_tmp110="attributes on enum not supported";_tag_fat(_tmp110,sizeof(char),33U);});Cyc_Warn_err(_tmp75F,_tmp75E,_tag_fat(_tmp10F,sizeof(void*),0));});});
ed->sc=s;{
struct Cyc_List_List*_tmp113=({struct Cyc_List_List*_tmp112=_cycalloc(sizeof(*_tmp112));({struct Cyc_Absyn_Decl*_tmp761=({void*_tmp760=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111->tag=7,_tmp111->f1=ed;_tmp111;});Cyc_Absyn_new_decl(_tmp760,loc);});_tmp112->hd=_tmp761;}),_tmp112->tl=0;_tmp112;});_npop_handler(0);return _tmp113;}}default: _tmp108=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp107)->f1)->r)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_tmp108;
# 896
if(atts != 0)({void*_tmp114=0U;({unsigned _tmp763=loc;struct _fat_ptr _tmp762=({const char*_tmp115="attributes on datatypes not supported";_tag_fat(_tmp115,sizeof(char),38U);});Cyc_Warn_err(_tmp763,_tmp762,_tag_fat(_tmp114,sizeof(void*),0));});});
dd->sc=s;{
struct Cyc_List_List*_tmp118=({struct Cyc_List_List*_tmp117=_cycalloc(sizeof(*_tmp117));({struct Cyc_Absyn_Decl*_tmp765=({void*_tmp764=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp116=_cycalloc(sizeof(*_tmp116));_tmp116->tag=6,_tmp116->f1=dd;_tmp116;});Cyc_Absyn_new_decl(_tmp764,loc);});_tmp117->hd=_tmp765;}),_tmp117->tl=0;_tmp117;});_npop_handler(0);return _tmp118;}}}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)){case 20: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)->f1).UnknownAggr).tag == 1){_tmp10B=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)->f1).UnknownAggr).val).f1;_tmp108=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)->f1).UnknownAggr).val).f2;_tmp10A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f2;{enum Cyc_Absyn_AggrKind k=_tmp10B;struct _tuple0*n=_tmp108;struct Cyc_List_List*ts=_tmp10A;
# 900
struct Cyc_List_List*_tmp119=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,loc,ts);struct Cyc_List_List*ts2=_tmp119;
struct Cyc_Absyn_Aggrdecl*_tmp11A=({struct Cyc_Absyn_Aggrdecl*_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120->kind=k,_tmp120->sc=s,_tmp120->name=n,_tmp120->tvs=ts2,_tmp120->impl=0,_tmp120->attributes=0,_tmp120->expected_mem_kind=0;_tmp120;});struct Cyc_Absyn_Aggrdecl*ad=_tmp11A;
if(atts != 0)({void*_tmp11B=0U;({unsigned _tmp767=loc;struct _fat_ptr _tmp766=({const char*_tmp11C="bad attributes on type declaration";_tag_fat(_tmp11C,sizeof(char),35U);});Cyc_Warn_err(_tmp767,_tmp766,_tag_fat(_tmp11B,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp11F=({struct Cyc_List_List*_tmp11E=_cycalloc(sizeof(*_tmp11E));({struct Cyc_Absyn_Decl*_tmp769=({void*_tmp768=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->tag=5,_tmp11D->f1=ad;_tmp11D;});Cyc_Absyn_new_decl(_tmp768,loc);});_tmp11E->hd=_tmp769;}),_tmp11E->tl=0;_tmp11E;});_npop_handler(0);return _tmp11F;}}}else{goto _LL25;}case 18: if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)->f1).KnownDatatype).tag == 2){_tmp108=((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)->f1).KnownDatatype).val;{struct Cyc_Absyn_Datatypedecl**tudp=_tmp108;
# 905
if(atts != 0)({void*_tmp121=0U;({unsigned _tmp76B=loc;struct _fat_ptr _tmp76A=({const char*_tmp122="bad attributes on datatype";_tag_fat(_tmp122,sizeof(char),27U);});Cyc_Warn_err(_tmp76B,_tmp76A,_tag_fat(_tmp121,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp125=({struct Cyc_List_List*_tmp124=_cycalloc(sizeof(*_tmp124));({struct Cyc_Absyn_Decl*_tmp76D=({void*_tmp76C=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->tag=6,_tmp123->f1=*tudp;_tmp123;});Cyc_Absyn_new_decl(_tmp76C,loc);});_tmp124->hd=_tmp76D;}),_tmp124->tl=0;_tmp124;});_npop_handler(0);return _tmp125;}}}else{_tmp108=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)->f1).UnknownDatatype).val).name;_tmp109=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)->f1).UnknownDatatype).val).is_extensible;_tmp10A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f2;{struct _tuple0*n=_tmp108;int isx=_tmp109;struct Cyc_List_List*ts=_tmp10A;
# 908
struct Cyc_List_List*_tmp126=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,loc,ts);struct Cyc_List_List*ts2=_tmp126;
struct Cyc_Absyn_Decl*_tmp127=Cyc_Absyn_datatype_decl(s,n,ts2,0,isx,loc);struct Cyc_Absyn_Decl*tud=_tmp127;
if(atts != 0)({void*_tmp128=0U;({unsigned _tmp76F=loc;struct _fat_ptr _tmp76E=({const char*_tmp129="bad attributes on datatype";_tag_fat(_tmp129,sizeof(char),27U);});Cyc_Warn_err(_tmp76F,_tmp76E,_tag_fat(_tmp128,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp12B=({struct Cyc_List_List*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->hd=tud,_tmp12A->tl=0;_tmp12A;});_npop_handler(0);return _tmp12B;}}}case 15: _tmp108=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)->f1;{struct _tuple0*n=_tmp108;
# 913
struct Cyc_Absyn_Enumdecl*_tmp12C=({struct Cyc_Absyn_Enumdecl*_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->sc=s,_tmp133->name=n,_tmp133->fields=0;_tmp133;});struct Cyc_Absyn_Enumdecl*ed=_tmp12C;
if(atts != 0)({void*_tmp12D=0U;({unsigned _tmp771=loc;struct _fat_ptr _tmp770=({const char*_tmp12E="bad attributes on enum";_tag_fat(_tmp12E,sizeof(char),23U);});Cyc_Warn_err(_tmp771,_tmp770,_tag_fat(_tmp12D,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp132=({struct Cyc_List_List*_tmp131=_cycalloc(sizeof(*_tmp131));({struct Cyc_Absyn_Decl*_tmp773=({struct Cyc_Absyn_Decl*_tmp130=_cycalloc(sizeof(*_tmp130));({void*_tmp772=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F->tag=7,_tmp12F->f1=ed;_tmp12F;});_tmp130->r=_tmp772;}),_tmp130->loc=loc;_tmp130;});_tmp131->hd=_tmp773;}),_tmp131->tl=0;_tmp131;});_npop_handler(0);return _tmp132;}}case 16: _tmp108=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)->f1;{struct Cyc_List_List*fs=_tmp108;
# 919
struct Cyc_Absyn_Enumdecl*_tmp134=({struct Cyc_Absyn_Enumdecl*_tmp13C=_cycalloc(sizeof(*_tmp13C));_tmp13C->sc=s,({struct _tuple0*_tmp775=Cyc_Parse_gensym_enum();_tmp13C->name=_tmp775;}),({struct Cyc_Core_Opt*_tmp774=({struct Cyc_Core_Opt*_tmp13B=_cycalloc(sizeof(*_tmp13B));_tmp13B->v=fs;_tmp13B;});_tmp13C->fields=_tmp774;});_tmp13C;});struct Cyc_Absyn_Enumdecl*ed=_tmp134;
if(atts != 0)({void*_tmp135=0U;({unsigned _tmp777=loc;struct _fat_ptr _tmp776=({const char*_tmp136="bad attributes on enum";_tag_fat(_tmp136,sizeof(char),23U);});Cyc_Warn_err(_tmp777,_tmp776,_tag_fat(_tmp135,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp13A=({struct Cyc_List_List*_tmp139=_cycalloc(sizeof(*_tmp139));({struct Cyc_Absyn_Decl*_tmp779=({struct Cyc_Absyn_Decl*_tmp138=_cycalloc(sizeof(*_tmp138));({void*_tmp778=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp137=_cycalloc(sizeof(*_tmp137));_tmp137->tag=7,_tmp137->f1=ed;_tmp137;});_tmp138->r=_tmp778;}),_tmp138->loc=loc;_tmp138;});_tmp139->hd=_tmp779;}),_tmp139->tl=0;_tmp139;});_npop_handler(0);return _tmp13A;}}default: goto _LL25;}default: _LL25:
({void*_tmp13D=0U;({unsigned _tmp77B=loc;struct _fat_ptr _tmp77A=({const char*_tmp13E="missing declarator";_tag_fat(_tmp13E,sizeof(char),19U);});Cyc_Warn_err(_tmp77B,_tmp77A,_tag_fat(_tmp13D,sizeof(void*),0));});});{struct Cyc_List_List*_tmp13F=0;_npop_handler(0);return _tmp13F;}};}{
# 926
struct Cyc_List_List*_tmp140=Cyc_Parse_apply_tmss(mkrgn,tq,base_type,declarators,atts);struct Cyc_List_List*fields=_tmp140;
if(istypedef){
# 931
if(!exps_empty)
({void*_tmp141=0U;({unsigned _tmp77D=loc;struct _fat_ptr _tmp77C=({const char*_tmp142="initializer in typedef declaration";_tag_fat(_tmp142,sizeof(char),35U);});Cyc_Warn_err(_tmp77D,_tmp77C,_tag_fat(_tmp141,sizeof(void*),0));});});{
struct Cyc_List_List*decls=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*)(unsigned,struct _tuple16*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_v_typ_to_typedef,loc,fields);
struct Cyc_List_List*_tmp143=decls;_npop_handler(0);return _tmp143;}}{
# 937
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp144=fields;struct Cyc_List_List*ds=_tmp144;for(0;ds != 0;(ds=ds->tl,exprs=(_check_null(exprs))->tl)){
struct _tuple16*_tmp145=(struct _tuple16*)ds->hd;struct _tuple16*_stmttmp13=_tmp145;struct _tuple16*_tmp146=_stmttmp13;void*_tmp14C;void*_tmp14B;void*_tmp14A;struct Cyc_Absyn_Tqual _tmp149;void*_tmp148;unsigned _tmp147;_tmp147=_tmp146->f1;_tmp148=_tmp146->f2;_tmp149=_tmp146->f3;_tmp14A=_tmp146->f4;_tmp14B=_tmp146->f5;_tmp14C=_tmp146->f6;{unsigned varloc=_tmp147;struct _tuple0*x=_tmp148;struct Cyc_Absyn_Tqual tq2=_tmp149;void*t2=_tmp14A;struct Cyc_List_List*tvs2=_tmp14B;struct Cyc_List_List*atts2=_tmp14C;
if(tvs2 != 0)
({void*_tmp14D=0U;({unsigned _tmp77F=loc;struct _fat_ptr _tmp77E=({const char*_tmp14E="bad type params, ignoring";_tag_fat(_tmp14E,sizeof(char),26U);});Cyc_Warn_warn(_tmp77F,_tmp77E,_tag_fat(_tmp14D,sizeof(void*),0));});});
if(exprs == 0)
({int(*_tmp781)(unsigned,struct _fat_ptr)=({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;});unsigned _tmp780=loc;_tmp781(_tmp780,({const char*_tmp14F="unexpected NULL in parse!";_tag_fat(_tmp14F,sizeof(char),26U);}));});{
struct Cyc_Absyn_Vardecl*_tmp150=Cyc_Absyn_new_vardecl(varloc,x,t2,(struct Cyc_Absyn_Exp*)(_check_null(exprs))->hd);struct Cyc_Absyn_Vardecl*vd=_tmp150;
vd->tq=tq2;
vd->sc=s;
vd->attributes=atts2;
decls=({struct Cyc_List_List*_tmp153=_cycalloc(sizeof(*_tmp153));({struct Cyc_Absyn_Decl*_tmp783=({struct Cyc_Absyn_Decl*_tmp152=_cycalloc(sizeof(*_tmp152));({void*_tmp782=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp151=_cycalloc(sizeof(*_tmp151));_tmp151->tag=0,_tmp151->f1=vd;_tmp151;});_tmp152->r=_tmp782;}),_tmp152->loc=loc;_tmp152;});_tmp153->hd=_tmp783;}),_tmp153->tl=decls;_tmp153;});}}}}{
# 950
struct Cyc_List_List*_tmp154=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(decls);_npop_handler(0);return _tmp154;}}}}}}}}}
# 849
;_pop_region();}
# 954
static unsigned Cyc_Parse_cnst2uint(unsigned loc,union Cyc_Absyn_Cnst x){
union Cyc_Absyn_Cnst _tmp155=x;long long _tmp156;char _tmp157;int _tmp158;switch((_tmp155.LongLong_c).tag){case 5: _tmp158=((_tmp155.Int_c).val).f2;{int i=_tmp158;
return(unsigned)i;}case 2: _tmp157=((_tmp155.Char_c).val).f2;{char c=_tmp157;
return(unsigned)c;}case 6: _tmp156=((_tmp155.LongLong_c).val).f2;{long long x=_tmp156;
# 959
unsigned long long y=(unsigned long long)x;
if(y > 4294967295U)
({void*_tmp159=0U;({unsigned _tmp785=loc;struct _fat_ptr _tmp784=({const char*_tmp15A="integer constant too large";_tag_fat(_tmp15A,sizeof(char),27U);});Cyc_Warn_err(_tmp785,_tmp784,_tag_fat(_tmp159,sizeof(void*),0));});});
return(unsigned)x;}default:
# 964
({struct Cyc_String_pa_PrintArg_struct _tmp15D=({struct Cyc_String_pa_PrintArg_struct _tmp649;_tmp649.tag=0,({struct _fat_ptr _tmp786=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_cnst2string(x));_tmp649.f1=_tmp786;});_tmp649;});void*_tmp15B[1];_tmp15B[0]=& _tmp15D;({unsigned _tmp788=loc;struct _fat_ptr _tmp787=({const char*_tmp15C="expected integer constant but found %s";_tag_fat(_tmp15C,sizeof(char),39U);});Cyc_Warn_err(_tmp788,_tmp787,_tag_fat(_tmp15B,sizeof(void*),1));});});
return 0U;};}
# 970
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp15E=p->r;void*_stmttmp14=_tmp15E;void*_tmp15F=_stmttmp14;void*_tmp161;struct _fat_ptr _tmp162;char _tmp164;int _tmp163;enum Cyc_Absyn_Sign _tmp165;void*_tmp160;switch(*((int*)_tmp15F)){case 15: _tmp160=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp15F)->f1;{struct _tuple0*x=_tmp160;
return Cyc_Absyn_unknownid_exp(x,p->loc);}case 3: if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp15F)->f2)->r)== 0){_tmp160=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp15F)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp160;
# 974
return({struct Cyc_Absyn_Exp*_tmp789=Cyc_Absyn_unknownid_exp(vd->name,p->loc);Cyc_Absyn_deref_exp(_tmp789,p->loc);});}}else{goto _LL13;}case 6: _tmp160=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp15F)->f1;{struct Cyc_Absyn_Pat*p2=_tmp160;
return({struct Cyc_Absyn_Exp*_tmp78A=Cyc_Parse_pat2exp(p2);Cyc_Absyn_address_exp(_tmp78A,p->loc);});}case 9:
 return Cyc_Absyn_null_exp(p->loc);case 10: _tmp165=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp15F)->f1;_tmp163=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp15F)->f2;{enum Cyc_Absyn_Sign s=_tmp165;int i=_tmp163;
return Cyc_Absyn_int_exp(s,i,p->loc);}case 11: _tmp164=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp15F)->f1;{char c=_tmp164;
return Cyc_Absyn_char_exp(c,p->loc);}case 12: _tmp162=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp15F)->f1;_tmp163=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp15F)->f2;{struct _fat_ptr s=_tmp162;int i=_tmp163;
return Cyc_Absyn_float_exp(s,i,p->loc);}case 16: if(((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp15F)->f3 == 0){_tmp160=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp15F)->f1;_tmp161=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp15F)->f2;{struct _tuple0*x=_tmp160;struct Cyc_List_List*ps=_tmp161;
# 981
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(x,p->loc);
struct Cyc_List_List*es=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Parse_pat2exp,ps);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}}else{goto _LL13;}case 17: _tmp160=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp15F)->f1;{struct Cyc_Absyn_Exp*e=_tmp160;
return e;}default: _LL13:
# 986
({void*_tmp166=0U;({unsigned _tmp78C=p->loc;struct _fat_ptr _tmp78B=({const char*_tmp167="cannot mix patterns and expressions in case";_tag_fat(_tmp167,sizeof(char),44U);});Cyc_Warn_err(_tmp78C,_tmp78B,_tag_fat(_tmp166,sizeof(void*),0));});});
return Cyc_Absyn_null_exp(p->loc);};}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _fat_ptr val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple22{unsigned f1;void*f2;void*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple23{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _tuple25{int f1;enum Cyc_Absyn_AggrKind f2;};struct _union_YYSTYPE_YY23{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_Parse_Declarator val;};struct _tuple26{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY29{int tag;struct _tuple26*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY31{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY32{int tag;int val;};struct _union_YYSTYPE_YY33{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY35{int tag;struct Cyc_List_List*val;};struct _tuple27{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY36{int tag;struct _tuple27 val;};struct _union_YYSTYPE_YY37{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY38{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY39{int tag;struct Cyc_List_List*val;};struct _tuple28{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY40{int tag;struct _tuple28*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY43{int tag;void*val;};struct _union_YYSTYPE_YY44{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY45{int tag;void*val;};struct _union_YYSTYPE_YY46{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY47{int tag;void*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY49{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY50{int tag;void*val;};struct _union_YYSTYPE_YY51{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY52{int tag;void*val;};struct _union_YYSTYPE_YY53{int tag;struct Cyc_List_List*val;};struct _tuple29{struct Cyc_List_List*f1;unsigned f2;};struct _union_YYSTYPE_YY54{int tag;struct _tuple29*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY56{int tag;void*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY58{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY59{int tag;struct _tuple17 val;};struct _union_YYSTYPE_YY60{int tag;void*val;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY61{int tag;struct _tuple30*val;};struct _tuple31{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY62{int tag;struct _tuple31*val;};struct _union_YYSTYPE_YY63{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY64{int tag;struct Cyc_List_List*val;};struct _tuple32{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};struct _union_YYSTYPE_YY65{int tag;struct _tuple32*val;};struct _union_YYSTYPE_YY66{int tag;struct Cyc_Absyn_Exp*(*val)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YY58 YY58;struct _union_YYSTYPE_YY59 YY59;struct _union_YYSTYPE_YY60 YY60;struct _union_YYSTYPE_YY61 YY61;struct _union_YYSTYPE_YY62 YY62;struct _union_YYSTYPE_YY63 YY63;struct _union_YYSTYPE_YY64 YY64;struct _union_YYSTYPE_YY65 YY65;struct _union_YYSTYPE_YY66 YY66;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};
# 1072
static void Cyc_yythrowfail(struct _fat_ptr s){
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp168=_cycalloc(sizeof(*_tmp168));_tmp168->tag=Cyc_Core_Failure,_tmp168->f1=s;_tmp168;}));}static char _tmp16B[7U]="cnst_t";
# 1041 "parse.y"
static union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp16B,_tmp16B,_tmp16B + 7U};
union Cyc_YYSTYPE*_tmp169=yy1;union Cyc_Absyn_Cnst _tmp16A;if((((union Cyc_YYSTYPE*)_tmp169)->Int_tok).tag == 1){_tmp16A=(_tmp169->Int_tok).val;{union Cyc_Absyn_Cnst yy=_tmp16A;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1048
static union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){return({union Cyc_YYSTYPE _tmp64A;(_tmp64A.Int_tok).tag=1U,(_tmp64A.Int_tok).val=yy1;_tmp64A;});}static char _tmp16E[5U]="char";
# 1042 "parse.y"
static char Cyc_yyget_Char_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp16E,_tmp16E,_tmp16E + 5U};
union Cyc_YYSTYPE*_tmp16C=yy1;char _tmp16D;if((((union Cyc_YYSTYPE*)_tmp16C)->Char_tok).tag == 2){_tmp16D=(_tmp16C->Char_tok).val;{char yy=_tmp16D;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1049
static union Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _tmp64B;(_tmp64B.Char_tok).tag=2U,(_tmp64B.Char_tok).val=yy1;_tmp64B;});}static char _tmp171[13U]="string_t<`H>";
# 1043 "parse.y"
static struct _fat_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp171,_tmp171,_tmp171 + 13U};
union Cyc_YYSTYPE*_tmp16F=yy1;struct _fat_ptr _tmp170;if((((union Cyc_YYSTYPE*)_tmp16F)->String_tok).tag == 3){_tmp170=(_tmp16F->String_tok).val;{struct _fat_ptr yy=_tmp170;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1050
static union Cyc_YYSTYPE Cyc_String_tok(struct _fat_ptr yy1){return({union Cyc_YYSTYPE _tmp64C;(_tmp64C.String_tok).tag=3U,(_tmp64C.String_tok).val=yy1;_tmp64C;});}static char _tmp174[35U]="$(seg_t,booltype_t, ptrbound_t)@`H";
# 1046 "parse.y"
static struct _tuple22*Cyc_yyget_YY1(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp174,_tmp174,_tmp174 + 35U};
union Cyc_YYSTYPE*_tmp172=yy1;void*_tmp173;if((((union Cyc_YYSTYPE*)_tmp172)->YY1).tag == 7){_tmp173=(_tmp172->YY1).val;{struct _tuple22*yy=_tmp173;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1053
static union Cyc_YYSTYPE Cyc_YY1(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp64D;(_tmp64D.YY1).tag=7U,(_tmp64D.YY1).val=yy1;_tmp64D;});}static char _tmp177[11U]="ptrbound_t";
# 1047 "parse.y"
static void*Cyc_yyget_YY2(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp177,_tmp177,_tmp177 + 11U};
union Cyc_YYSTYPE*_tmp175=yy1;void*_tmp176;if((((union Cyc_YYSTYPE*)_tmp175)->YY2).tag == 8){_tmp176=(_tmp175->YY2).val;{void*yy=_tmp176;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1054
static union Cyc_YYSTYPE Cyc_YY2(void*yy1){return({union Cyc_YYSTYPE _tmp64E;(_tmp64E.YY2).tag=8U,(_tmp64E.YY2).val=yy1;_tmp64E;});}static char _tmp17A[28U]="list_t<offsetof_field_t,`H>";
# 1048 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp17A,_tmp17A,_tmp17A + 28U};
union Cyc_YYSTYPE*_tmp178=yy1;void*_tmp179;if((((union Cyc_YYSTYPE*)_tmp178)->YY3).tag == 9){_tmp179=(_tmp178->YY3).val;{struct Cyc_List_List*yy=_tmp179;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1055
static union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp64F;(_tmp64F.YY3).tag=9U,(_tmp64F.YY3).val=yy1;_tmp64F;});}static char _tmp17D[6U]="exp_t";
# 1049 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp17D,_tmp17D,_tmp17D + 6U};
union Cyc_YYSTYPE*_tmp17B=yy1;void*_tmp17C;if((((union Cyc_YYSTYPE*)_tmp17B)->Exp_tok).tag == 5){_tmp17C=(_tmp17B->Exp_tok).val;{struct Cyc_Absyn_Exp*yy=_tmp17C;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1056
static union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp650;(_tmp650.Exp_tok).tag=5U,(_tmp650.Exp_tok).val=yy1;_tmp650;});}static char _tmp180[17U]="list_t<exp_t,`H>";
static struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp180,_tmp180,_tmp180 + 17U};
union Cyc_YYSTYPE*_tmp17E=yy1;void*_tmp17F;if((((union Cyc_YYSTYPE*)_tmp17E)->YY4).tag == 10){_tmp17F=(_tmp17E->YY4).val;{struct Cyc_List_List*yy=_tmp17F;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1064
static union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp651;(_tmp651.YY4).tag=10U,(_tmp651.YY4).val=yy1;_tmp651;});}static char _tmp183[47U]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1058 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp183,_tmp183,_tmp183 + 47U};
union Cyc_YYSTYPE*_tmp181=yy1;void*_tmp182;if((((union Cyc_YYSTYPE*)_tmp181)->YY5).tag == 11){_tmp182=(_tmp181->YY5).val;{struct Cyc_List_List*yy=_tmp182;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1065
static union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp652;(_tmp652.YY5).tag=11U,(_tmp652.YY5).val=yy1;_tmp652;});}static char _tmp186[9U]="primop_t";
# 1059 "parse.y"
static enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp186,_tmp186,_tmp186 + 9U};
union Cyc_YYSTYPE*_tmp184=yy1;enum Cyc_Absyn_Primop _tmp185;if((((union Cyc_YYSTYPE*)_tmp184)->YY6).tag == 12){_tmp185=(_tmp184->YY6).val;{enum Cyc_Absyn_Primop yy=_tmp185;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1066
static union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE _tmp653;(_tmp653.YY6).tag=12U,(_tmp653.YY6).val=yy1;_tmp653;});}static char _tmp189[19U]="opt_t<primop_t,`H>";
# 1060 "parse.y"
static struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp189,_tmp189,_tmp189 + 19U};
union Cyc_YYSTYPE*_tmp187=yy1;void*_tmp188;if((((union Cyc_YYSTYPE*)_tmp187)->YY7).tag == 13){_tmp188=(_tmp187->YY7).val;{struct Cyc_Core_Opt*yy=_tmp188;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1067
static union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _tmp654;(_tmp654.YY7).tag=13U,(_tmp654.YY7).val=yy1;_tmp654;});}static char _tmp18C[7U]="qvar_t";
# 1061 "parse.y"
static struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp18C,_tmp18C,_tmp18C + 7U};
union Cyc_YYSTYPE*_tmp18A=yy1;void*_tmp18B;if((((union Cyc_YYSTYPE*)_tmp18A)->QualId_tok).tag == 4){_tmp18B=(_tmp18A->QualId_tok).val;{struct _tuple0*yy=_tmp18B;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1068
static union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){return({union Cyc_YYSTYPE _tmp655;(_tmp655.QualId_tok).tag=4U,(_tmp655.QualId_tok).val=yy1;_tmp655;});}static char _tmp18F[7U]="stmt_t";
# 1064 "parse.y"
static struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp18F,_tmp18F,_tmp18F + 7U};
union Cyc_YYSTYPE*_tmp18D=yy1;void*_tmp18E;if((((union Cyc_YYSTYPE*)_tmp18D)->Stmt_tok).tag == 6){_tmp18E=(_tmp18D->Stmt_tok).val;{struct Cyc_Absyn_Stmt*yy=_tmp18E;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1071
static union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){return({union Cyc_YYSTYPE _tmp656;(_tmp656.Stmt_tok).tag=6U,(_tmp656.Stmt_tok).val=yy1;_tmp656;});}static char _tmp192[27U]="list_t<switch_clause_t,`H>";
# 1067 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp192,_tmp192,_tmp192 + 27U};
union Cyc_YYSTYPE*_tmp190=yy1;void*_tmp191;if((((union Cyc_YYSTYPE*)_tmp190)->YY8).tag == 14){_tmp191=(_tmp190->YY8).val;{struct Cyc_List_List*yy=_tmp191;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1074
static union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp657;(_tmp657.YY8).tag=14U,(_tmp657.YY8).val=yy1;_tmp657;});}static char _tmp195[6U]="pat_t";
# 1068 "parse.y"
static struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp195,_tmp195,_tmp195 + 6U};
union Cyc_YYSTYPE*_tmp193=yy1;void*_tmp194;if((((union Cyc_YYSTYPE*)_tmp193)->YY9).tag == 15){_tmp194=(_tmp193->YY9).val;{struct Cyc_Absyn_Pat*yy=_tmp194;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1075
static union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){return({union Cyc_YYSTYPE _tmp658;(_tmp658.YY9).tag=15U,(_tmp658.YY9).val=yy1;_tmp658;});}static char _tmp198[28U]="$(list_t<pat_t,`H>,bool)@`H";
# 1073 "parse.y"
static struct _tuple23*Cyc_yyget_YY10(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp198,_tmp198,_tmp198 + 28U};
union Cyc_YYSTYPE*_tmp196=yy1;void*_tmp197;if((((union Cyc_YYSTYPE*)_tmp196)->YY10).tag == 16){_tmp197=(_tmp196->YY10).val;{struct _tuple23*yy=_tmp197;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1080
static union Cyc_YYSTYPE Cyc_YY10(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp659;(_tmp659.YY10).tag=16U,(_tmp659.YY10).val=yy1;_tmp659;});}static char _tmp19B[17U]="list_t<pat_t,`H>";
# 1074 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp19B,_tmp19B,_tmp19B + 17U};
union Cyc_YYSTYPE*_tmp199=yy1;void*_tmp19A;if((((union Cyc_YYSTYPE*)_tmp199)->YY11).tag == 17){_tmp19A=(_tmp199->YY11).val;{struct Cyc_List_List*yy=_tmp19A;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1081
static union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp65A;(_tmp65A.YY11).tag=17U,(_tmp65A.YY11).val=yy1;_tmp65A;});}static char _tmp19E[36U]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1075 "parse.y"
static struct _tuple24*Cyc_yyget_YY12(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp19E,_tmp19E,_tmp19E + 36U};
union Cyc_YYSTYPE*_tmp19C=yy1;void*_tmp19D;if((((union Cyc_YYSTYPE*)_tmp19C)->YY12).tag == 18){_tmp19D=(_tmp19C->YY12).val;{struct _tuple24*yy=_tmp19D;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1082
static union Cyc_YYSTYPE Cyc_YY12(struct _tuple24*yy1){return({union Cyc_YYSTYPE _tmp65B;(_tmp65B.YY12).tag=18U,(_tmp65B.YY12).val=yy1;_tmp65B;});}static char _tmp1A1[47U]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1076 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A1,_tmp1A1,_tmp1A1 + 47U};
union Cyc_YYSTYPE*_tmp19F=yy1;void*_tmp1A0;if((((union Cyc_YYSTYPE*)_tmp19F)->YY13).tag == 19){_tmp1A0=(_tmp19F->YY13).val;{struct Cyc_List_List*yy=_tmp1A0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1083
static union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp65C;(_tmp65C.YY13).tag=19U,(_tmp65C.YY13).val=yy1;_tmp65C;});}static char _tmp1A4[58U]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1077 "parse.y"
static struct _tuple23*Cyc_yyget_YY14(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A4,_tmp1A4,_tmp1A4 + 58U};
union Cyc_YYSTYPE*_tmp1A2=yy1;void*_tmp1A3;if((((union Cyc_YYSTYPE*)_tmp1A2)->YY14).tag == 20){_tmp1A3=(_tmp1A2->YY14).val;{struct _tuple23*yy=_tmp1A3;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1084
static union Cyc_YYSTYPE Cyc_YY14(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp65D;(_tmp65D.YY14).tag=20U,(_tmp65D.YY14).val=yy1;_tmp65D;});}static char _tmp1A7[9U]="fndecl_t";
# 1078 "parse.y"
static struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A7,_tmp1A7,_tmp1A7 + 9U};
union Cyc_YYSTYPE*_tmp1A5=yy1;void*_tmp1A6;if((((union Cyc_YYSTYPE*)_tmp1A5)->YY15).tag == 21){_tmp1A6=(_tmp1A5->YY15).val;{struct Cyc_Absyn_Fndecl*yy=_tmp1A6;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1085
static union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE _tmp65E;(_tmp65E.YY15).tag=21U,(_tmp65E.YY15).val=yy1;_tmp65E;});}static char _tmp1AA[18U]="list_t<decl_t,`H>";
# 1079 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1AA,_tmp1AA,_tmp1AA + 18U};
union Cyc_YYSTYPE*_tmp1A8=yy1;void*_tmp1A9;if((((union Cyc_YYSTYPE*)_tmp1A8)->YY16).tag == 22){_tmp1A9=(_tmp1A8->YY16).val;{struct Cyc_List_List*yy=_tmp1A9;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1086
static union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp65F;(_tmp65F.YY16).tag=22U,(_tmp65F.YY16).val=yy1;_tmp65F;});}static char _tmp1AD[12U]="decl_spec_t";
# 1082 "parse.y"
static struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1AD,_tmp1AD,_tmp1AD + 12U};
union Cyc_YYSTYPE*_tmp1AB=yy1;struct Cyc_Parse_Declaration_spec _tmp1AC;if((((union Cyc_YYSTYPE*)_tmp1AB)->YY17).tag == 23){_tmp1AC=(_tmp1AB->YY17).val;{struct Cyc_Parse_Declaration_spec yy=_tmp1AC;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1089
static union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){return({union Cyc_YYSTYPE _tmp660;(_tmp660.YY17).tag=23U,(_tmp660.YY17).val=yy1;_tmp660;});}static char _tmp1B0[31U]="$(declarator_t<`yy>,exp_opt_t)";
# 1083 "parse.y"
static struct _tuple12 Cyc_yyget_YY18(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B0,_tmp1B0,_tmp1B0 + 31U};
union Cyc_YYSTYPE*_tmp1AE=yy1;struct _tuple12 _tmp1AF;if((((union Cyc_YYSTYPE*)_tmp1AE)->YY18).tag == 24){_tmp1AF=(_tmp1AE->YY18).val;{struct _tuple12 yy=_tmp1AF;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1090
static union Cyc_YYSTYPE Cyc_YY18(struct _tuple12 yy1){return({union Cyc_YYSTYPE _tmp661;(_tmp661.YY18).tag=24U,(_tmp661.YY18).val=yy1;_tmp661;});}static char _tmp1B3[23U]="declarator_list_t<`yy>";
# 1084 "parse.y"
static struct _tuple11*Cyc_yyget_YY19(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B3,_tmp1B3,_tmp1B3 + 23U};
union Cyc_YYSTYPE*_tmp1B1=yy1;void*_tmp1B2;if((((union Cyc_YYSTYPE*)_tmp1B1)->YY19).tag == 25){_tmp1B2=(_tmp1B1->YY19).val;{struct _tuple11*yy=_tmp1B2;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1091
static union Cyc_YYSTYPE Cyc_YY19(struct _tuple11*yy1){return({union Cyc_YYSTYPE _tmp662;(_tmp662.YY19).tag=25U,(_tmp662.YY19).val=yy1;_tmp662;});}static char _tmp1B6[16U]="storage_class_t";
# 1085 "parse.y"
static enum Cyc_Parse_Storage_class Cyc_yyget_YY20(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B6,_tmp1B6,_tmp1B6 + 16U};
union Cyc_YYSTYPE*_tmp1B4=yy1;enum Cyc_Parse_Storage_class _tmp1B5;if((((union Cyc_YYSTYPE*)_tmp1B4)->YY20).tag == 26){_tmp1B5=(_tmp1B4->YY20).val;{enum Cyc_Parse_Storage_class yy=_tmp1B5;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1092
static union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class yy1){return({union Cyc_YYSTYPE _tmp663;(_tmp663.YY20).tag=26U,(_tmp663.YY20).val=yy1;_tmp663;});}static char _tmp1B9[17U]="type_specifier_t";
# 1086 "parse.y"
static struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B9,_tmp1B9,_tmp1B9 + 17U};
union Cyc_YYSTYPE*_tmp1B7=yy1;struct Cyc_Parse_Type_specifier _tmp1B8;if((((union Cyc_YYSTYPE*)_tmp1B7)->YY21).tag == 27){_tmp1B8=(_tmp1B7->YY21).val;{struct Cyc_Parse_Type_specifier yy=_tmp1B8;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1093
static union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){return({union Cyc_YYSTYPE _tmp664;(_tmp664.YY21).tag=27U,(_tmp664.YY21).val=yy1;_tmp664;});}static char _tmp1BC[12U]="aggr_kind_t";
# 1088 "parse.y"
static enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1BC,_tmp1BC,_tmp1BC + 12U};
union Cyc_YYSTYPE*_tmp1BA=yy1;enum Cyc_Absyn_AggrKind _tmp1BB;if((((union Cyc_YYSTYPE*)_tmp1BA)->YY22).tag == 28){_tmp1BB=(_tmp1BA->YY22).val;{enum Cyc_Absyn_AggrKind yy=_tmp1BB;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1095
static union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE _tmp665;(_tmp665.YY22).tag=28U,(_tmp665.YY22).val=yy1;_tmp665;});}static char _tmp1BF[20U]="$(bool,aggr_kind_t)";
# 1089 "parse.y"
static struct _tuple25 Cyc_yyget_YY23(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1BF,_tmp1BF,_tmp1BF + 20U};
union Cyc_YYSTYPE*_tmp1BD=yy1;struct _tuple25 _tmp1BE;if((((union Cyc_YYSTYPE*)_tmp1BD)->YY23).tag == 29){_tmp1BE=(_tmp1BD->YY23).val;{struct _tuple25 yy=_tmp1BE;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1096
static union Cyc_YYSTYPE Cyc_YY23(struct _tuple25 yy1){return({union Cyc_YYSTYPE _tmp666;(_tmp666.YY23).tag=29U,(_tmp666.YY23).val=yy1;_tmp666;});}static char _tmp1C2[8U]="tqual_t";
# 1090 "parse.y"
static struct Cyc_Absyn_Tqual Cyc_yyget_YY24(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C2,_tmp1C2,_tmp1C2 + 8U};
union Cyc_YYSTYPE*_tmp1C0=yy1;struct Cyc_Absyn_Tqual _tmp1C1;if((((union Cyc_YYSTYPE*)_tmp1C0)->YY24).tag == 30){_tmp1C1=(_tmp1C0->YY24).val;{struct Cyc_Absyn_Tqual yy=_tmp1C1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1097
static union Cyc_YYSTYPE Cyc_YY24(struct Cyc_Absyn_Tqual yy1){return({union Cyc_YYSTYPE _tmp667;(_tmp667.YY24).tag=30U,(_tmp667.YY24).val=yy1;_tmp667;});}static char _tmp1C5[23U]="list_t<aggrfield_t,`H>";
# 1091 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C5,_tmp1C5,_tmp1C5 + 23U};
union Cyc_YYSTYPE*_tmp1C3=yy1;void*_tmp1C4;if((((union Cyc_YYSTYPE*)_tmp1C3)->YY25).tag == 31){_tmp1C4=(_tmp1C3->YY25).val;{struct Cyc_List_List*yy=_tmp1C4;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1098
static union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp668;(_tmp668.YY25).tag=31U,(_tmp668.YY25).val=yy1;_tmp668;});}static char _tmp1C8[34U]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1092 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C8,_tmp1C8,_tmp1C8 + 34U};
union Cyc_YYSTYPE*_tmp1C6=yy1;void*_tmp1C7;if((((union Cyc_YYSTYPE*)_tmp1C6)->YY26).tag == 32){_tmp1C7=(_tmp1C6->YY26).val;{struct Cyc_List_List*yy=_tmp1C7;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1099
static union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp669;(_tmp669.YY26).tag=32U,(_tmp669.YY26).val=yy1;_tmp669;});}static char _tmp1CB[33U]="list_t<type_modifier_t<`yy>,`yy>";
# 1093 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1CB,_tmp1CB,_tmp1CB + 33U};
union Cyc_YYSTYPE*_tmp1C9=yy1;void*_tmp1CA;if((((union Cyc_YYSTYPE*)_tmp1C9)->YY27).tag == 33){_tmp1CA=(_tmp1C9->YY27).val;{struct Cyc_List_List*yy=_tmp1CA;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1100
static union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp66A;(_tmp66A.YY27).tag=33U,(_tmp66A.YY27).val=yy1;_tmp66A;});}static char _tmp1CE[18U]="declarator_t<`yy>";
# 1094 "parse.y"
static struct Cyc_Parse_Declarator Cyc_yyget_YY28(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1CE,_tmp1CE,_tmp1CE + 18U};
union Cyc_YYSTYPE*_tmp1CC=yy1;struct Cyc_Parse_Declarator _tmp1CD;if((((union Cyc_YYSTYPE*)_tmp1CC)->YY28).tag == 34){_tmp1CD=(_tmp1CC->YY28).val;{struct Cyc_Parse_Declarator yy=_tmp1CD;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1101
static union Cyc_YYSTYPE Cyc_YY28(struct Cyc_Parse_Declarator yy1){return({union Cyc_YYSTYPE _tmp66B;(_tmp66B.YY28).tag=34U,(_tmp66B.YY28).val=yy1;_tmp66B;});}static char _tmp1D1[45U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1095 "parse.y"
static struct _tuple26*Cyc_yyget_YY29(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D1,_tmp1D1,_tmp1D1 + 45U};
union Cyc_YYSTYPE*_tmp1CF=yy1;void*_tmp1D0;if((((union Cyc_YYSTYPE*)_tmp1CF)->YY29).tag == 35){_tmp1D0=(_tmp1CF->YY29).val;{struct _tuple26*yy=_tmp1D0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1102
static union Cyc_YYSTYPE Cyc_YY29(struct _tuple26*yy1){return({union Cyc_YYSTYPE _tmp66C;(_tmp66C.YY29).tag=35U,(_tmp66C.YY29).val=yy1;_tmp66C;});}static char _tmp1D4[57U]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1096 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY30(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D4,_tmp1D4,_tmp1D4 + 57U};
union Cyc_YYSTYPE*_tmp1D2=yy1;void*_tmp1D3;if((((union Cyc_YYSTYPE*)_tmp1D2)->YY30).tag == 36){_tmp1D3=(_tmp1D2->YY30).val;{struct Cyc_List_List*yy=_tmp1D3;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1103
static union Cyc_YYSTYPE Cyc_YY30(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp66D;(_tmp66D.YY30).tag=36U,(_tmp66D.YY30).val=yy1;_tmp66D;});}static char _tmp1D7[26U]="abstractdeclarator_t<`yy>";
# 1097 "parse.y"
static struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY31(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D7,_tmp1D7,_tmp1D7 + 26U};
union Cyc_YYSTYPE*_tmp1D5=yy1;struct Cyc_Parse_Abstractdeclarator _tmp1D6;if((((union Cyc_YYSTYPE*)_tmp1D5)->YY31).tag == 37){_tmp1D6=(_tmp1D5->YY31).val;{struct Cyc_Parse_Abstractdeclarator yy=_tmp1D6;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1104
static union Cyc_YYSTYPE Cyc_YY31(struct Cyc_Parse_Abstractdeclarator yy1){return({union Cyc_YYSTYPE _tmp66E;(_tmp66E.YY31).tag=37U,(_tmp66E.YY31).val=yy1;_tmp66E;});}static char _tmp1DA[5U]="bool";
# 1098 "parse.y"
static int Cyc_yyget_YY32(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1DA,_tmp1DA,_tmp1DA + 5U};
union Cyc_YYSTYPE*_tmp1D8=yy1;int _tmp1D9;if((((union Cyc_YYSTYPE*)_tmp1D8)->YY32).tag == 38){_tmp1D9=(_tmp1D8->YY32).val;{int yy=_tmp1D9;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1105
static union Cyc_YYSTYPE Cyc_YY32(int yy1){return({union Cyc_YYSTYPE _tmp66F;(_tmp66F.YY32).tag=38U,(_tmp66F.YY32).val=yy1;_tmp66F;});}static char _tmp1DD[8U]="scope_t";
# 1099 "parse.y"
static enum Cyc_Absyn_Scope Cyc_yyget_YY33(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1DD,_tmp1DD,_tmp1DD + 8U};
union Cyc_YYSTYPE*_tmp1DB=yy1;enum Cyc_Absyn_Scope _tmp1DC;if((((union Cyc_YYSTYPE*)_tmp1DB)->YY33).tag == 39){_tmp1DC=(_tmp1DB->YY33).val;{enum Cyc_Absyn_Scope yy=_tmp1DC;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1106
static union Cyc_YYSTYPE Cyc_YY33(enum Cyc_Absyn_Scope yy1){return({union Cyc_YYSTYPE _tmp670;(_tmp670.YY33).tag=39U,(_tmp670.YY33).val=yy1;_tmp670;});}static char _tmp1E0[16U]="datatypefield_t";
# 1100 "parse.y"
static struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY34(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E0,_tmp1E0,_tmp1E0 + 16U};
union Cyc_YYSTYPE*_tmp1DE=yy1;void*_tmp1DF;if((((union Cyc_YYSTYPE*)_tmp1DE)->YY34).tag == 40){_tmp1DF=(_tmp1DE->YY34).val;{struct Cyc_Absyn_Datatypefield*yy=_tmp1DF;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1107
static union Cyc_YYSTYPE Cyc_YY34(struct Cyc_Absyn_Datatypefield*yy1){return({union Cyc_YYSTYPE _tmp671;(_tmp671.YY34).tag=40U,(_tmp671.YY34).val=yy1;_tmp671;});}static char _tmp1E3[27U]="list_t<datatypefield_t,`H>";
# 1101 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY35(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E3,_tmp1E3,_tmp1E3 + 27U};
union Cyc_YYSTYPE*_tmp1E1=yy1;void*_tmp1E2;if((((union Cyc_YYSTYPE*)_tmp1E1)->YY35).tag == 41){_tmp1E2=(_tmp1E1->YY35).val;{struct Cyc_List_List*yy=_tmp1E2;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1108
static union Cyc_YYSTYPE Cyc_YY35(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp672;(_tmp672.YY35).tag=41U,(_tmp672.YY35).val=yy1;_tmp672;});}static char _tmp1E6[41U]="$(tqual_t,type_specifier_t,attributes_t)";
# 1102 "parse.y"
static struct _tuple27 Cyc_yyget_YY36(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E6,_tmp1E6,_tmp1E6 + 41U};
union Cyc_YYSTYPE*_tmp1E4=yy1;struct _tuple27 _tmp1E5;if((((union Cyc_YYSTYPE*)_tmp1E4)->YY36).tag == 42){_tmp1E5=(_tmp1E4->YY36).val;{struct _tuple27 yy=_tmp1E5;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1109
static union Cyc_YYSTYPE Cyc_YY36(struct _tuple27 yy1){return({union Cyc_YYSTYPE _tmp673;(_tmp673.YY36).tag=42U,(_tmp673.YY36).val=yy1;_tmp673;});}static char _tmp1E9[17U]="list_t<var_t,`H>";
# 1103 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY37(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E9,_tmp1E9,_tmp1E9 + 17U};
union Cyc_YYSTYPE*_tmp1E7=yy1;void*_tmp1E8;if((((union Cyc_YYSTYPE*)_tmp1E7)->YY37).tag == 43){_tmp1E8=(_tmp1E7->YY37).val;{struct Cyc_List_List*yy=_tmp1E8;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1110
static union Cyc_YYSTYPE Cyc_YY37(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp674;(_tmp674.YY37).tag=43U,(_tmp674.YY37).val=yy1;_tmp674;});}static char _tmp1EC[31U]="$(var_opt_t,tqual_t,type_t)@`H";
# 1104 "parse.y"
static struct _tuple8*Cyc_yyget_YY38(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1EC,_tmp1EC,_tmp1EC + 31U};
union Cyc_YYSTYPE*_tmp1EA=yy1;void*_tmp1EB;if((((union Cyc_YYSTYPE*)_tmp1EA)->YY38).tag == 44){_tmp1EB=(_tmp1EA->YY38).val;{struct _tuple8*yy=_tmp1EB;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1111
static union Cyc_YYSTYPE Cyc_YY38(struct _tuple8*yy1){return({union Cyc_YYSTYPE _tmp675;(_tmp675.YY38).tag=44U,(_tmp675.YY38).val=yy1;_tmp675;});}static char _tmp1EF[42U]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1105 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY39(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1EF,_tmp1EF,_tmp1EF + 42U};
union Cyc_YYSTYPE*_tmp1ED=yy1;void*_tmp1EE;if((((union Cyc_YYSTYPE*)_tmp1ED)->YY39).tag == 45){_tmp1EE=(_tmp1ED->YY39).val;{struct Cyc_List_List*yy=_tmp1EE;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1112
static union Cyc_YYSTYPE Cyc_YY39(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp676;(_tmp676.YY39).tag=45U,(_tmp676.YY39).val=yy1;_tmp676;});}static char _tmp1F2[115U]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1106 "parse.y"
static struct _tuple28*Cyc_yyget_YY40(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F2,_tmp1F2,_tmp1F2 + 115U};
union Cyc_YYSTYPE*_tmp1F0=yy1;void*_tmp1F1;if((((union Cyc_YYSTYPE*)_tmp1F0)->YY40).tag == 46){_tmp1F1=(_tmp1F0->YY40).val;{struct _tuple28*yy=_tmp1F1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1113
static union Cyc_YYSTYPE Cyc_YY40(struct _tuple28*yy1){return({union Cyc_YYSTYPE _tmp677;(_tmp677.YY40).tag=46U,(_tmp677.YY40).val=yy1;_tmp677;});}static char _tmp1F5[8U]="types_t";
# 1107 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F5,_tmp1F5,_tmp1F5 + 8U};
union Cyc_YYSTYPE*_tmp1F3=yy1;void*_tmp1F4;if((((union Cyc_YYSTYPE*)_tmp1F3)->YY41).tag == 47){_tmp1F4=(_tmp1F3->YY41).val;{struct Cyc_List_List*yy=_tmp1F4;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1114
static union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp678;(_tmp678.YY41).tag=47U,(_tmp678.YY41).val=yy1;_tmp678;});}static char _tmp1F8[24U]="list_t<designator_t,`H>";
# 1109 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY42(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F8,_tmp1F8,_tmp1F8 + 24U};
union Cyc_YYSTYPE*_tmp1F6=yy1;void*_tmp1F7;if((((union Cyc_YYSTYPE*)_tmp1F6)->YY42).tag == 48){_tmp1F7=(_tmp1F6->YY42).val;{struct Cyc_List_List*yy=_tmp1F7;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1116
static union Cyc_YYSTYPE Cyc_YY42(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp679;(_tmp679.YY42).tag=48U,(_tmp679.YY42).val=yy1;_tmp679;});}static char _tmp1FB[13U]="designator_t";
# 1110 "parse.y"
static void*Cyc_yyget_YY43(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1FB,_tmp1FB,_tmp1FB + 13U};
union Cyc_YYSTYPE*_tmp1F9=yy1;void*_tmp1FA;if((((union Cyc_YYSTYPE*)_tmp1F9)->YY43).tag == 49){_tmp1FA=(_tmp1F9->YY43).val;{void*yy=_tmp1FA;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1117
static union Cyc_YYSTYPE Cyc_YY43(void*yy1){return({union Cyc_YYSTYPE _tmp67A;(_tmp67A.YY43).tag=49U,(_tmp67A.YY43).val=yy1;_tmp67A;});}static char _tmp1FE[7U]="kind_t";
# 1111 "parse.y"
static struct Cyc_Absyn_Kind*Cyc_yyget_YY44(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1FE,_tmp1FE,_tmp1FE + 7U};
union Cyc_YYSTYPE*_tmp1FC=yy1;void*_tmp1FD;if((((union Cyc_YYSTYPE*)_tmp1FC)->YY44).tag == 50){_tmp1FD=(_tmp1FC->YY44).val;{struct Cyc_Absyn_Kind*yy=_tmp1FD;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1118
static union Cyc_YYSTYPE Cyc_YY44(struct Cyc_Absyn_Kind*yy1){return({union Cyc_YYSTYPE _tmp67B;(_tmp67B.YY44).tag=50U,(_tmp67B.YY44).val=yy1;_tmp67B;});}static char _tmp201[7U]="type_t";
# 1112 "parse.y"
static void*Cyc_yyget_YY45(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp201,_tmp201,_tmp201 + 7U};
union Cyc_YYSTYPE*_tmp1FF=yy1;void*_tmp200;if((((union Cyc_YYSTYPE*)_tmp1FF)->YY45).tag == 51){_tmp200=(_tmp1FF->YY45).val;{void*yy=_tmp200;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1119
static union Cyc_YYSTYPE Cyc_YY45(void*yy1){return({union Cyc_YYSTYPE _tmp67C;(_tmp67C.YY45).tag=51U,(_tmp67C.YY45).val=yy1;_tmp67C;});}static char _tmp204[23U]="list_t<attribute_t,`H>";
# 1113 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY46(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp204,_tmp204,_tmp204 + 23U};
union Cyc_YYSTYPE*_tmp202=yy1;void*_tmp203;if((((union Cyc_YYSTYPE*)_tmp202)->YY46).tag == 52){_tmp203=(_tmp202->YY46).val;{struct Cyc_List_List*yy=_tmp203;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1120
static union Cyc_YYSTYPE Cyc_YY46(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp67D;(_tmp67D.YY46).tag=52U,(_tmp67D.YY46).val=yy1;_tmp67D;});}static char _tmp207[12U]="attribute_t";
# 1114 "parse.y"
static void*Cyc_yyget_YY47(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp207,_tmp207,_tmp207 + 12U};
union Cyc_YYSTYPE*_tmp205=yy1;void*_tmp206;if((((union Cyc_YYSTYPE*)_tmp205)->YY47).tag == 53){_tmp206=(_tmp205->YY47).val;{void*yy=_tmp206;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1121
static union Cyc_YYSTYPE Cyc_YY47(void*yy1){return({union Cyc_YYSTYPE _tmp67E;(_tmp67E.YY47).tag=53U,(_tmp67E.YY47).val=yy1;_tmp67E;});}static char _tmp20A[12U]="enumfield_t";
# 1115 "parse.y"
static struct Cyc_Absyn_Enumfield*Cyc_yyget_YY48(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp20A,_tmp20A,_tmp20A + 12U};
union Cyc_YYSTYPE*_tmp208=yy1;void*_tmp209;if((((union Cyc_YYSTYPE*)_tmp208)->YY48).tag == 54){_tmp209=(_tmp208->YY48).val;{struct Cyc_Absyn_Enumfield*yy=_tmp209;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1122
static union Cyc_YYSTYPE Cyc_YY48(struct Cyc_Absyn_Enumfield*yy1){return({union Cyc_YYSTYPE _tmp67F;(_tmp67F.YY48).tag=54U,(_tmp67F.YY48).val=yy1;_tmp67F;});}static char _tmp20D[23U]="list_t<enumfield_t,`H>";
# 1116 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY49(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp20D,_tmp20D,_tmp20D + 23U};
union Cyc_YYSTYPE*_tmp20B=yy1;void*_tmp20C;if((((union Cyc_YYSTYPE*)_tmp20B)->YY49).tag == 55){_tmp20C=(_tmp20B->YY49).val;{struct Cyc_List_List*yy=_tmp20C;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1123
static union Cyc_YYSTYPE Cyc_YY49(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp680;(_tmp680.YY49).tag=55U,(_tmp680.YY49).val=yy1;_tmp680;});}static char _tmp210[11U]="type_opt_t";
# 1117 "parse.y"
static void*Cyc_yyget_YY50(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp210,_tmp210,_tmp210 + 11U};
union Cyc_YYSTYPE*_tmp20E=yy1;void*_tmp20F;if((((union Cyc_YYSTYPE*)_tmp20E)->YY50).tag == 56){_tmp20F=(_tmp20E->YY50).val;{void*yy=_tmp20F;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1124
static union Cyc_YYSTYPE Cyc_YY50(void*yy1){return({union Cyc_YYSTYPE _tmp681;(_tmp681.YY50).tag=56U,(_tmp681.YY50).val=yy1;_tmp681;});}static char _tmp213[31U]="list_t<$(type_t,type_t)@`H,`H>";
# 1118 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY51(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp213,_tmp213,_tmp213 + 31U};
union Cyc_YYSTYPE*_tmp211=yy1;void*_tmp212;if((((union Cyc_YYSTYPE*)_tmp211)->YY51).tag == 57){_tmp212=(_tmp211->YY51).val;{struct Cyc_List_List*yy=_tmp212;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1125
static union Cyc_YYSTYPE Cyc_YY51(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp682;(_tmp682.YY51).tag=57U,(_tmp682.YY51).val=yy1;_tmp682;});}static char _tmp216[11U]="booltype_t";
# 1119 "parse.y"
static void*Cyc_yyget_YY52(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp216,_tmp216,_tmp216 + 11U};
union Cyc_YYSTYPE*_tmp214=yy1;void*_tmp215;if((((union Cyc_YYSTYPE*)_tmp214)->YY52).tag == 58){_tmp215=(_tmp214->YY52).val;{void*yy=_tmp215;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1126
static union Cyc_YYSTYPE Cyc_YY52(void*yy1){return({union Cyc_YYSTYPE _tmp683;(_tmp683.YY52).tag=58U,(_tmp683.YY52).val=yy1;_tmp683;});}static char _tmp219[35U]="list_t<$(seg_t,qvar_t,bool)@`H,`H>";
# 1120 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY53(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp219,_tmp219,_tmp219 + 35U};
union Cyc_YYSTYPE*_tmp217=yy1;void*_tmp218;if((((union Cyc_YYSTYPE*)_tmp217)->YY53).tag == 59){_tmp218=(_tmp217->YY53).val;{struct Cyc_List_List*yy=_tmp218;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1127
static union Cyc_YYSTYPE Cyc_YY53(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp684;(_tmp684.YY53).tag=59U,(_tmp684.YY53).val=yy1;_tmp684;});}static char _tmp21C[48U]="$(list_t<$(seg_t,qvar_t,bool)@`H,`H>, seg_t)@`H";
# 1121 "parse.y"
static struct _tuple29*Cyc_yyget_YY54(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp21C,_tmp21C,_tmp21C + 48U};
union Cyc_YYSTYPE*_tmp21A=yy1;void*_tmp21B;if((((union Cyc_YYSTYPE*)_tmp21A)->YY54).tag == 60){_tmp21B=(_tmp21A->YY54).val;{struct _tuple29*yy=_tmp21B;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1128
static union Cyc_YYSTYPE Cyc_YY54(struct _tuple29*yy1){return({union Cyc_YYSTYPE _tmp685;(_tmp685.YY54).tag=60U,(_tmp685.YY54).val=yy1;_tmp685;});}static char _tmp21F[18U]="list_t<qvar_t,`H>";
# 1122 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY55(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp21F,_tmp21F,_tmp21F + 18U};
union Cyc_YYSTYPE*_tmp21D=yy1;void*_tmp21E;if((((union Cyc_YYSTYPE*)_tmp21D)->YY55).tag == 61){_tmp21E=(_tmp21D->YY55).val;{struct Cyc_List_List*yy=_tmp21E;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1129
static union Cyc_YYSTYPE Cyc_YY55(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp686;(_tmp686.YY55).tag=61U,(_tmp686.YY55).val=yy1;_tmp686;});}static char _tmp222[20U]="pointer_qual_t<`yy>";
# 1123 "parse.y"
static void*Cyc_yyget_YY56(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp222,_tmp222,_tmp222 + 20U};
union Cyc_YYSTYPE*_tmp220=yy1;void*_tmp221;if((((union Cyc_YYSTYPE*)_tmp220)->YY56).tag == 62){_tmp221=(_tmp220->YY56).val;{void*yy=_tmp221;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1130
static union Cyc_YYSTYPE Cyc_YY56(void*yy1){return({union Cyc_YYSTYPE _tmp687;(_tmp687.YY56).tag=62U,(_tmp687.YY56).val=yy1;_tmp687;});}static char _tmp225[21U]="pointer_quals_t<`yy>";
# 1124 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY57(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp225,_tmp225,_tmp225 + 21U};
union Cyc_YYSTYPE*_tmp223=yy1;void*_tmp224;if((((union Cyc_YYSTYPE*)_tmp223)->YY57).tag == 63){_tmp224=(_tmp223->YY57).val;{struct Cyc_List_List*yy=_tmp224;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1131
static union Cyc_YYSTYPE Cyc_YY57(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp688;(_tmp688.YY57).tag=63U,(_tmp688.YY57).val=yy1;_tmp688;});}static char _tmp228[10U]="exp_opt_t";
# 1125 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_YY58(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp228,_tmp228,_tmp228 + 10U};
union Cyc_YYSTYPE*_tmp226=yy1;void*_tmp227;if((((union Cyc_YYSTYPE*)_tmp226)->YY58).tag == 64){_tmp227=(_tmp226->YY58).val;{struct Cyc_Absyn_Exp*yy=_tmp227;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1132
static union Cyc_YYSTYPE Cyc_YY58(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp689;(_tmp689.YY58).tag=64U,(_tmp689.YY58).val=yy1;_tmp689;});}static char _tmp22B[23U]="$(exp_opt_t,exp_opt_t)";
# 1126 "parse.y"
static struct _tuple17 Cyc_yyget_YY59(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp22B,_tmp22B,_tmp22B + 23U};
union Cyc_YYSTYPE*_tmp229=yy1;struct _tuple17 _tmp22A;if((((union Cyc_YYSTYPE*)_tmp229)->YY59).tag == 65){_tmp22A=(_tmp229->YY59).val;{struct _tuple17 yy=_tmp22A;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1133
static union Cyc_YYSTYPE Cyc_YY59(struct _tuple17 yy1){return({union Cyc_YYSTYPE _tmp68A;(_tmp68A.YY59).tag=65U,(_tmp68A.YY59).val=yy1;_tmp68A;});}static char _tmp22E[10U]="raw_exp_t";
# 1127 "parse.y"
static void*Cyc_yyget_YY60(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp22E,_tmp22E,_tmp22E + 10U};
union Cyc_YYSTYPE*_tmp22C=yy1;void*_tmp22D;if((((union Cyc_YYSTYPE*)_tmp22C)->YY60).tag == 66){_tmp22D=(_tmp22C->YY60).val;{void*yy=_tmp22D;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1134
static union Cyc_YYSTYPE Cyc_YY60(void*yy1){return({union Cyc_YYSTYPE _tmp68B;(_tmp68B.YY60).tag=66U,(_tmp68B.YY60).val=yy1;_tmp68B;});}static char _tmp231[112U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1128 "parse.y"
static struct _tuple30*Cyc_yyget_YY61(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp231,_tmp231,_tmp231 + 112U};
union Cyc_YYSTYPE*_tmp22F=yy1;void*_tmp230;if((((union Cyc_YYSTYPE*)_tmp22F)->YY61).tag == 67){_tmp230=(_tmp22F->YY61).val;{struct _tuple30*yy=_tmp230;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1135
static union Cyc_YYSTYPE Cyc_YY61(struct _tuple30*yy1){return({union Cyc_YYSTYPE _tmp68C;(_tmp68C.YY61).tag=67U,(_tmp68C.YY61).val=yy1;_tmp68C;});}static char _tmp234[73U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1129 "parse.y"
static struct _tuple31*Cyc_yyget_YY62(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp234,_tmp234,_tmp234 + 73U};
union Cyc_YYSTYPE*_tmp232=yy1;void*_tmp233;if((((union Cyc_YYSTYPE*)_tmp232)->YY62).tag == 68){_tmp233=(_tmp232->YY62).val;{struct _tuple31*yy=_tmp233;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1136
static union Cyc_YYSTYPE Cyc_YY62(struct _tuple31*yy1){return({union Cyc_YYSTYPE _tmp68D;(_tmp68D.YY62).tag=68U,(_tmp68D.YY62).val=yy1;_tmp68D;});}static char _tmp237[28U]="list_t<string_t<`H>@`H, `H>";
# 1130 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY63(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp237,_tmp237,_tmp237 + 28U};
union Cyc_YYSTYPE*_tmp235=yy1;void*_tmp236;if((((union Cyc_YYSTYPE*)_tmp235)->YY63).tag == 69){_tmp236=(_tmp235->YY63).val;{struct Cyc_List_List*yy=_tmp236;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1137
static union Cyc_YYSTYPE Cyc_YY63(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp68E;(_tmp68E.YY63).tag=69U,(_tmp68E.YY63).val=yy1;_tmp68E;});}static char _tmp23A[38U]="list_t<$(string_t<`H>, exp_t)@`H, `H>";
# 1131 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY64(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp23A,_tmp23A,_tmp23A + 38U};
union Cyc_YYSTYPE*_tmp238=yy1;void*_tmp239;if((((union Cyc_YYSTYPE*)_tmp238)->YY64).tag == 70){_tmp239=(_tmp238->YY64).val;{struct Cyc_List_List*yy=_tmp239;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1138
static union Cyc_YYSTYPE Cyc_YY64(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp68F;(_tmp68F.YY64).tag=70U,(_tmp68F.YY64).val=yy1;_tmp68F;});}static char _tmp23D[26U]="$(string_t<`H>, exp_t)@`H";
# 1132 "parse.y"
static struct _tuple32*Cyc_yyget_YY65(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp23D,_tmp23D,_tmp23D + 26U};
union Cyc_YYSTYPE*_tmp23B=yy1;void*_tmp23C;if((((union Cyc_YYSTYPE*)_tmp23B)->YY65).tag == 71){_tmp23C=(_tmp23B->YY65).val;{struct _tuple32*yy=_tmp23C;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1139
static union Cyc_YYSTYPE Cyc_YY65(struct _tuple32*yy1){return({union Cyc_YYSTYPE _tmp690;(_tmp690.YY65).tag=71U,(_tmp690.YY65).val=yy1;_tmp690;});}static char _tmp240[16U]="exp_maker_fun_t";
# 1133 "parse.y"
static struct Cyc_Absyn_Exp*(*Cyc_yyget_YY66(union Cyc_YYSTYPE*yy1))(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned){
static struct _fat_ptr s={_tmp240,_tmp240,_tmp240 + 16U};
union Cyc_YYSTYPE*_tmp23E=yy1;void*_tmp23F;if((((union Cyc_YYSTYPE*)_tmp23E)->YY66).tag == 72){_tmp23F=(_tmp23E->YY66).val;{struct Cyc_Absyn_Exp*(*yy)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=_tmp23F;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1140
static union Cyc_YYSTYPE Cyc_YY66(struct Cyc_Absyn_Exp*(*yy1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)){return({union Cyc_YYSTYPE _tmp691;(_tmp691.YY66).tag=72U,(_tmp691.YY66).val=yy1;_tmp691;});}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1156
struct Cyc_Yyltype Cyc_yynewloc (void){
return({struct Cyc_Yyltype _tmp692;_tmp692.timestamp=0,_tmp692.first_line=0,_tmp692.first_column=0,_tmp692.last_line=0,_tmp692.last_column=0;_tmp692;});}
# 1159
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1170 "parse.y"
static short Cyc_yytranslate[381U]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,152,2,2,137,150,147,2,134,135,130,144,129,148,139,149,2,2,2,2,2,2,2,2,2,2,138,126,132,131,133,143,142,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,140,2,141,146,136,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,127,145,128,151,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125};static char _tmp241[2U]="$";static char _tmp242[6U]="error";static char _tmp243[12U]="$undefined.";static char _tmp244[5U]="AUTO";static char _tmp245[9U]="REGISTER";static char _tmp246[7U]="STATIC";static char _tmp247[7U]="EXTERN";static char _tmp248[8U]="TYPEDEF";static char _tmp249[5U]="VOID";static char _tmp24A[5U]="CHAR";static char _tmp24B[6U]="SHORT";static char _tmp24C[4U]="INT";static char _tmp24D[5U]="LONG";static char _tmp24E[6U]="FLOAT";static char _tmp24F[7U]="DOUBLE";static char _tmp250[7U]="SIGNED";static char _tmp251[9U]="UNSIGNED";static char _tmp252[6U]="CONST";static char _tmp253[9U]="VOLATILE";static char _tmp254[9U]="RESTRICT";static char _tmp255[7U]="STRUCT";static char _tmp256[6U]="UNION";static char _tmp257[5U]="CASE";static char _tmp258[8U]="DEFAULT";static char _tmp259[7U]="INLINE";static char _tmp25A[7U]="SIZEOF";static char _tmp25B[9U]="OFFSETOF";static char _tmp25C[3U]="IF";static char _tmp25D[5U]="ELSE";static char _tmp25E[7U]="SWITCH";static char _tmp25F[6U]="WHILE";static char _tmp260[3U]="DO";static char _tmp261[4U]="FOR";static char _tmp262[5U]="GOTO";static char _tmp263[9U]="CONTINUE";static char _tmp264[6U]="BREAK";static char _tmp265[7U]="RETURN";static char _tmp266[5U]="ENUM";static char _tmp267[7U]="TYPEOF";static char _tmp268[16U]="BUILTIN_VA_LIST";static char _tmp269[10U]="EXTENSION";static char _tmp26A[8U]="NULL_kw";static char _tmp26B[4U]="LET";static char _tmp26C[6U]="THROW";static char _tmp26D[4U]="TRY";static char _tmp26E[6U]="CATCH";static char _tmp26F[7U]="EXPORT";static char _tmp270[9U]="OVERRIDE";static char _tmp271[5U]="HIDE";static char _tmp272[4U]="NEW";static char _tmp273[9U]="ABSTRACT";static char _tmp274[9U]="FALLTHRU";static char _tmp275[6U]="USING";static char _tmp276[10U]="NAMESPACE";static char _tmp277[9U]="DATATYPE";static char _tmp278[7U]="MALLOC";static char _tmp279[8U]="RMALLOC";static char _tmp27A[15U]="RMALLOC_INLINE";static char _tmp27B[7U]="CALLOC";static char _tmp27C[8U]="RCALLOC";static char _tmp27D[5U]="SWAP";static char _tmp27E[9U]="REGION_T";static char _tmp27F[6U]="TAG_T";static char _tmp280[7U]="REGION";static char _tmp281[5U]="RNEW";static char _tmp282[8U]="REGIONS";static char _tmp283[7U]="PORTON";static char _tmp284[8U]="PORTOFF";static char _tmp285[7U]="PRAGMA";static char _tmp286[10U]="TEMPESTON";static char _tmp287[11U]="TEMPESTOFF";static char _tmp288[8U]="NUMELTS";static char _tmp289[8U]="VALUEOF";static char _tmp28A[10U]="VALUEOF_T";static char _tmp28B[9U]="TAGCHECK";static char _tmp28C[13U]="NUMELTS_QUAL";static char _tmp28D[10U]="THIN_QUAL";static char _tmp28E[9U]="FAT_QUAL";static char _tmp28F[13U]="NOTNULL_QUAL";static char _tmp290[14U]="NULLABLE_QUAL";static char _tmp291[14U]="REQUIRES_QUAL";static char _tmp292[13U]="ENSURES_QUAL";static char _tmp293[12U]="REGION_QUAL";static char _tmp294[16U]="NOZEROTERM_QUAL";static char _tmp295[14U]="ZEROTERM_QUAL";static char _tmp296[12U]="TAGGED_QUAL";static char _tmp297[12U]="ASSERT_QUAL";static char _tmp298[16U]="EXTENSIBLE_QUAL";static char _tmp299[18U]="AUTORELEASED_QUAL";static char _tmp29A[7U]="PTR_OP";static char _tmp29B[7U]="INC_OP";static char _tmp29C[7U]="DEC_OP";static char _tmp29D[8U]="LEFT_OP";static char _tmp29E[9U]="RIGHT_OP";static char _tmp29F[6U]="LE_OP";static char _tmp2A0[6U]="GE_OP";static char _tmp2A1[6U]="EQ_OP";static char _tmp2A2[6U]="NE_OP";static char _tmp2A3[7U]="AND_OP";static char _tmp2A4[6U]="OR_OP";static char _tmp2A5[11U]="MUL_ASSIGN";static char _tmp2A6[11U]="DIV_ASSIGN";static char _tmp2A7[11U]="MOD_ASSIGN";static char _tmp2A8[11U]="ADD_ASSIGN";static char _tmp2A9[11U]="SUB_ASSIGN";static char _tmp2AA[12U]="LEFT_ASSIGN";static char _tmp2AB[13U]="RIGHT_ASSIGN";static char _tmp2AC[11U]="AND_ASSIGN";static char _tmp2AD[11U]="XOR_ASSIGN";static char _tmp2AE[10U]="OR_ASSIGN";static char _tmp2AF[9U]="ELLIPSIS";static char _tmp2B0[11U]="LEFT_RIGHT";static char _tmp2B1[12U]="COLON_COLON";static char _tmp2B2[11U]="IDENTIFIER";static char _tmp2B3[17U]="INTEGER_CONSTANT";static char _tmp2B4[7U]="STRING";static char _tmp2B5[8U]="WSTRING";static char _tmp2B6[19U]="CHARACTER_CONSTANT";static char _tmp2B7[20U]="WCHARACTER_CONSTANT";static char _tmp2B8[18U]="FLOATING_CONSTANT";static char _tmp2B9[9U]="TYPE_VAR";static char _tmp2BA[13U]="TYPEDEF_NAME";static char _tmp2BB[16U]="QUAL_IDENTIFIER";static char _tmp2BC[18U]="QUAL_TYPEDEF_NAME";static char _tmp2BD[10U]="ATTRIBUTE";static char _tmp2BE[8U]="ASM_TOK";static char _tmp2BF[4U]="';'";static char _tmp2C0[4U]="'{'";static char _tmp2C1[4U]="'}'";static char _tmp2C2[4U]="','";static char _tmp2C3[4U]="'*'";static char _tmp2C4[4U]="'='";static char _tmp2C5[4U]="'<'";static char _tmp2C6[4U]="'>'";static char _tmp2C7[4U]="'('";static char _tmp2C8[4U]="')'";static char _tmp2C9[4U]="'_'";static char _tmp2CA[4U]="'$'";static char _tmp2CB[4U]="':'";static char _tmp2CC[4U]="'.'";static char _tmp2CD[4U]="'['";static char _tmp2CE[4U]="']'";static char _tmp2CF[4U]="'@'";static char _tmp2D0[4U]="'?'";static char _tmp2D1[4U]="'+'";static char _tmp2D2[4U]="'|'";static char _tmp2D3[4U]="'^'";static char _tmp2D4[4U]="'&'";static char _tmp2D5[4U]="'-'";static char _tmp2D6[4U]="'/'";static char _tmp2D7[4U]="'%'";static char _tmp2D8[4U]="'~'";static char _tmp2D9[4U]="'!'";static char _tmp2DA[5U]="prog";static char _tmp2DB[17U]="translation_unit";static char _tmp2DC[18U]="tempest_on_action";static char _tmp2DD[19U]="tempest_off_action";static char _tmp2DE[16U]="extern_c_action";static char _tmp2DF[13U]="end_extern_c";static char _tmp2E0[14U]="hide_list_opt";static char _tmp2E1[17U]="hide_list_values";static char _tmp2E2[16U]="export_list_opt";static char _tmp2E3[12U]="export_list";static char _tmp2E4[19U]="export_list_values";static char _tmp2E5[13U]="override_opt";static char _tmp2E6[21U]="external_declaration";static char _tmp2E7[14U]="optional_semi";static char _tmp2E8[20U]="function_definition";static char _tmp2E9[21U]="function_definition2";static char _tmp2EA[13U]="using_action";static char _tmp2EB[15U]="unusing_action";static char _tmp2EC[17U]="namespace_action";static char _tmp2ED[19U]="unnamespace_action";static char _tmp2EE[12U]="declaration";static char _tmp2EF[9U]="open_opt";static char _tmp2F0[17U]="declaration_list";static char _tmp2F1[23U]="declaration_specifiers";static char _tmp2F2[24U]="storage_class_specifier";static char _tmp2F3[15U]="attributes_opt";static char _tmp2F4[11U]="attributes";static char _tmp2F5[15U]="attribute_list";static char _tmp2F6[10U]="attribute";static char _tmp2F7[15U]="type_specifier";static char _tmp2F8[25U]="type_specifier_notypedef";static char _tmp2F9[5U]="kind";static char _tmp2FA[15U]="type_qualifier";static char _tmp2FB[15U]="enum_specifier";static char _tmp2FC[11U]="enum_field";static char _tmp2FD[22U]="enum_declaration_list";static char _tmp2FE[26U]="struct_or_union_specifier";static char _tmp2FF[26U]="maybe_tagged_struct_union";static char _tmp300[16U]="struct_or_union";static char _tmp301[16U]="type_params_opt";static char _tmp302[24U]="struct_declaration_list";static char _tmp303[25U]="struct_declaration_list0";static char _tmp304[25U]="init_declarator_list_rev";static char _tmp305[16U]="init_declarator";static char _tmp306[19U]="struct_declaration";static char _tmp307[25U]="specifier_qualifier_list";static char _tmp308[35U]="notypedef_specifier_qualifier_list";static char _tmp309[27U]="struct_declarator_list_rev";static char _tmp30A[18U]="struct_declarator";static char _tmp30B[20U]="requires_clause_opt";static char _tmp30C[19U]="datatype_specifier";static char _tmp30D[14U]="qual_datatype";static char _tmp30E[19U]="datatypefield_list";static char _tmp30F[20U]="datatypefield_scope";static char _tmp310[14U]="datatypefield";static char _tmp311[11U]="declarator";static char _tmp312[23U]="declarator_withtypedef";static char _tmp313[18U]="direct_declarator";static char _tmp314[30U]="direct_declarator_withtypedef";static char _tmp315[8U]="pointer";static char _tmp316[12U]="one_pointer";static char _tmp317[14U]="pointer_quals";static char _tmp318[13U]="pointer_qual";static char _tmp319[23U]="pointer_null_and_bound";static char _tmp31A[14U]="pointer_bound";static char _tmp31B[18U]="zeroterm_qual_opt";static char _tmp31C[8U]="rgn_opt";static char _tmp31D[11U]="tqual_list";static char _tmp31E[20U]="parameter_type_list";static char _tmp31F[9U]="type_var";static char _tmp320[16U]="optional_effect";static char _tmp321[19U]="optional_rgn_order";static char _tmp322[10U]="rgn_order";static char _tmp323[16U]="optional_inject";static char _tmp324[11U]="effect_set";static char _tmp325[14U]="atomic_effect";static char _tmp326[11U]="region_set";static char _tmp327[15U]="parameter_list";static char _tmp328[22U]="parameter_declaration";static char _tmp329[16U]="identifier_list";static char _tmp32A[17U]="identifier_list0";static char _tmp32B[12U]="initializer";static char _tmp32C[18U]="array_initializer";static char _tmp32D[17U]="initializer_list";static char _tmp32E[12U]="designation";static char _tmp32F[16U]="designator_list";static char _tmp330[11U]="designator";static char _tmp331[10U]="type_name";static char _tmp332[14U]="any_type_name";static char _tmp333[15U]="type_name_list";static char _tmp334[20U]="abstract_declarator";static char _tmp335[27U]="direct_abstract_declarator";static char _tmp336[25U]="requires_and_ensures_opt";static char _tmp337[10U]="statement";static char _tmp338[18U]="labeled_statement";static char _tmp339[21U]="expression_statement";static char _tmp33A[19U]="compound_statement";static char _tmp33B[16U]="block_item_list";static char _tmp33C[20U]="selection_statement";static char _tmp33D[15U]="switch_clauses";static char _tmp33E[20U]="iteration_statement";static char _tmp33F[12U]="for_exp_opt";static char _tmp340[15U]="jump_statement";static char _tmp341[12U]="exp_pattern";static char _tmp342[20U]="conditional_pattern";static char _tmp343[19U]="logical_or_pattern";static char _tmp344[20U]="logical_and_pattern";static char _tmp345[21U]="inclusive_or_pattern";static char _tmp346[21U]="exclusive_or_pattern";static char _tmp347[12U]="and_pattern";static char _tmp348[17U]="equality_pattern";static char _tmp349[19U]="relational_pattern";static char _tmp34A[14U]="shift_pattern";static char _tmp34B[17U]="additive_pattern";static char _tmp34C[23U]="multiplicative_pattern";static char _tmp34D[13U]="cast_pattern";static char _tmp34E[14U]="unary_pattern";static char _tmp34F[16U]="postfix_pattern";static char _tmp350[16U]="primary_pattern";static char _tmp351[8U]="pattern";static char _tmp352[19U]="tuple_pattern_list";static char _tmp353[20U]="tuple_pattern_list0";static char _tmp354[14U]="field_pattern";static char _tmp355[19U]="field_pattern_list";static char _tmp356[20U]="field_pattern_list0";static char _tmp357[11U]="expression";static char _tmp358[22U]="assignment_expression";static char _tmp359[20U]="assignment_operator";static char _tmp35A[23U]="conditional_expression";static char _tmp35B[20U]="constant_expression";static char _tmp35C[22U]="logical_or_expression";static char _tmp35D[23U]="logical_and_expression";static char _tmp35E[24U]="inclusive_or_expression";static char _tmp35F[24U]="exclusive_or_expression";static char _tmp360[15U]="and_expression";static char _tmp361[20U]="equality_expression";static char _tmp362[22U]="relational_expression";static char _tmp363[17U]="shift_expression";static char _tmp364[20U]="additive_expression";static char _tmp365[26U]="multiplicative_expression";static char _tmp366[12U]="equality_op";static char _tmp367[14U]="relational_op";static char _tmp368[12U]="additive_op";static char _tmp369[18U]="multiplicative_op";static char _tmp36A[16U]="cast_expression";static char _tmp36B[17U]="unary_expression";static char _tmp36C[15U]="unary_operator";static char _tmp36D[9U]="asm_expr";static char _tmp36E[13U]="volatile_opt";static char _tmp36F[12U]="asm_out_opt";static char _tmp370[12U]="asm_outlist";static char _tmp371[11U]="asm_in_opt";static char _tmp372[11U]="asm_inlist";static char _tmp373[11U]="asm_io_elt";static char _tmp374[16U]="asm_clobber_opt";static char _tmp375[17U]="asm_clobber_list";static char _tmp376[19U]="postfix_expression";static char _tmp377[17U]="field_expression";static char _tmp378[19U]="primary_expression";static char _tmp379[25U]="argument_expression_list";static char _tmp37A[26U]="argument_expression_list0";static char _tmp37B[9U]="constant";static char _tmp37C[20U]="qual_opt_identifier";static char _tmp37D[17U]="qual_opt_typedef";static char _tmp37E[18U]="struct_union_name";static char _tmp37F[11U]="field_name";static char _tmp380[12U]="right_angle";
# 1523 "parse.y"
static struct _fat_ptr Cyc_yytname[320U]={{_tmp241,_tmp241,_tmp241 + 2U},{_tmp242,_tmp242,_tmp242 + 6U},{_tmp243,_tmp243,_tmp243 + 12U},{_tmp244,_tmp244,_tmp244 + 5U},{_tmp245,_tmp245,_tmp245 + 9U},{_tmp246,_tmp246,_tmp246 + 7U},{_tmp247,_tmp247,_tmp247 + 7U},{_tmp248,_tmp248,_tmp248 + 8U},{_tmp249,_tmp249,_tmp249 + 5U},{_tmp24A,_tmp24A,_tmp24A + 5U},{_tmp24B,_tmp24B,_tmp24B + 6U},{_tmp24C,_tmp24C,_tmp24C + 4U},{_tmp24D,_tmp24D,_tmp24D + 5U},{_tmp24E,_tmp24E,_tmp24E + 6U},{_tmp24F,_tmp24F,_tmp24F + 7U},{_tmp250,_tmp250,_tmp250 + 7U},{_tmp251,_tmp251,_tmp251 + 9U},{_tmp252,_tmp252,_tmp252 + 6U},{_tmp253,_tmp253,_tmp253 + 9U},{_tmp254,_tmp254,_tmp254 + 9U},{_tmp255,_tmp255,_tmp255 + 7U},{_tmp256,_tmp256,_tmp256 + 6U},{_tmp257,_tmp257,_tmp257 + 5U},{_tmp258,_tmp258,_tmp258 + 8U},{_tmp259,_tmp259,_tmp259 + 7U},{_tmp25A,_tmp25A,_tmp25A + 7U},{_tmp25B,_tmp25B,_tmp25B + 9U},{_tmp25C,_tmp25C,_tmp25C + 3U},{_tmp25D,_tmp25D,_tmp25D + 5U},{_tmp25E,_tmp25E,_tmp25E + 7U},{_tmp25F,_tmp25F,_tmp25F + 6U},{_tmp260,_tmp260,_tmp260 + 3U},{_tmp261,_tmp261,_tmp261 + 4U},{_tmp262,_tmp262,_tmp262 + 5U},{_tmp263,_tmp263,_tmp263 + 9U},{_tmp264,_tmp264,_tmp264 + 6U},{_tmp265,_tmp265,_tmp265 + 7U},{_tmp266,_tmp266,_tmp266 + 5U},{_tmp267,_tmp267,_tmp267 + 7U},{_tmp268,_tmp268,_tmp268 + 16U},{_tmp269,_tmp269,_tmp269 + 10U},{_tmp26A,_tmp26A,_tmp26A + 8U},{_tmp26B,_tmp26B,_tmp26B + 4U},{_tmp26C,_tmp26C,_tmp26C + 6U},{_tmp26D,_tmp26D,_tmp26D + 4U},{_tmp26E,_tmp26E,_tmp26E + 6U},{_tmp26F,_tmp26F,_tmp26F + 7U},{_tmp270,_tmp270,_tmp270 + 9U},{_tmp271,_tmp271,_tmp271 + 5U},{_tmp272,_tmp272,_tmp272 + 4U},{_tmp273,_tmp273,_tmp273 + 9U},{_tmp274,_tmp274,_tmp274 + 9U},{_tmp275,_tmp275,_tmp275 + 6U},{_tmp276,_tmp276,_tmp276 + 10U},{_tmp277,_tmp277,_tmp277 + 9U},{_tmp278,_tmp278,_tmp278 + 7U},{_tmp279,_tmp279,_tmp279 + 8U},{_tmp27A,_tmp27A,_tmp27A + 15U},{_tmp27B,_tmp27B,_tmp27B + 7U},{_tmp27C,_tmp27C,_tmp27C + 8U},{_tmp27D,_tmp27D,_tmp27D + 5U},{_tmp27E,_tmp27E,_tmp27E + 9U},{_tmp27F,_tmp27F,_tmp27F + 6U},{_tmp280,_tmp280,_tmp280 + 7U},{_tmp281,_tmp281,_tmp281 + 5U},{_tmp282,_tmp282,_tmp282 + 8U},{_tmp283,_tmp283,_tmp283 + 7U},{_tmp284,_tmp284,_tmp284 + 8U},{_tmp285,_tmp285,_tmp285 + 7U},{_tmp286,_tmp286,_tmp286 + 10U},{_tmp287,_tmp287,_tmp287 + 11U},{_tmp288,_tmp288,_tmp288 + 8U},{_tmp289,_tmp289,_tmp289 + 8U},{_tmp28A,_tmp28A,_tmp28A + 10U},{_tmp28B,_tmp28B,_tmp28B + 9U},{_tmp28C,_tmp28C,_tmp28C + 13U},{_tmp28D,_tmp28D,_tmp28D + 10U},{_tmp28E,_tmp28E,_tmp28E + 9U},{_tmp28F,_tmp28F,_tmp28F + 13U},{_tmp290,_tmp290,_tmp290 + 14U},{_tmp291,_tmp291,_tmp291 + 14U},{_tmp292,_tmp292,_tmp292 + 13U},{_tmp293,_tmp293,_tmp293 + 12U},{_tmp294,_tmp294,_tmp294 + 16U},{_tmp295,_tmp295,_tmp295 + 14U},{_tmp296,_tmp296,_tmp296 + 12U},{_tmp297,_tmp297,_tmp297 + 12U},{_tmp298,_tmp298,_tmp298 + 16U},{_tmp299,_tmp299,_tmp299 + 18U},{_tmp29A,_tmp29A,_tmp29A + 7U},{_tmp29B,_tmp29B,_tmp29B + 7U},{_tmp29C,_tmp29C,_tmp29C + 7U},{_tmp29D,_tmp29D,_tmp29D + 8U},{_tmp29E,_tmp29E,_tmp29E + 9U},{_tmp29F,_tmp29F,_tmp29F + 6U},{_tmp2A0,_tmp2A0,_tmp2A0 + 6U},{_tmp2A1,_tmp2A1,_tmp2A1 + 6U},{_tmp2A2,_tmp2A2,_tmp2A2 + 6U},{_tmp2A3,_tmp2A3,_tmp2A3 + 7U},{_tmp2A4,_tmp2A4,_tmp2A4 + 6U},{_tmp2A5,_tmp2A5,_tmp2A5 + 11U},{_tmp2A6,_tmp2A6,_tmp2A6 + 11U},{_tmp2A7,_tmp2A7,_tmp2A7 + 11U},{_tmp2A8,_tmp2A8,_tmp2A8 + 11U},{_tmp2A9,_tmp2A9,_tmp2A9 + 11U},{_tmp2AA,_tmp2AA,_tmp2AA + 12U},{_tmp2AB,_tmp2AB,_tmp2AB + 13U},{_tmp2AC,_tmp2AC,_tmp2AC + 11U},{_tmp2AD,_tmp2AD,_tmp2AD + 11U},{_tmp2AE,_tmp2AE,_tmp2AE + 10U},{_tmp2AF,_tmp2AF,_tmp2AF + 9U},{_tmp2B0,_tmp2B0,_tmp2B0 + 11U},{_tmp2B1,_tmp2B1,_tmp2B1 + 12U},{_tmp2B2,_tmp2B2,_tmp2B2 + 11U},{_tmp2B3,_tmp2B3,_tmp2B3 + 17U},{_tmp2B4,_tmp2B4,_tmp2B4 + 7U},{_tmp2B5,_tmp2B5,_tmp2B5 + 8U},{_tmp2B6,_tmp2B6,_tmp2B6 + 19U},{_tmp2B7,_tmp2B7,_tmp2B7 + 20U},{_tmp2B8,_tmp2B8,_tmp2B8 + 18U},{_tmp2B9,_tmp2B9,_tmp2B9 + 9U},{_tmp2BA,_tmp2BA,_tmp2BA + 13U},{_tmp2BB,_tmp2BB,_tmp2BB + 16U},{_tmp2BC,_tmp2BC,_tmp2BC + 18U},{_tmp2BD,_tmp2BD,_tmp2BD + 10U},{_tmp2BE,_tmp2BE,_tmp2BE + 8U},{_tmp2BF,_tmp2BF,_tmp2BF + 4U},{_tmp2C0,_tmp2C0,_tmp2C0 + 4U},{_tmp2C1,_tmp2C1,_tmp2C1 + 4U},{_tmp2C2,_tmp2C2,_tmp2C2 + 4U},{_tmp2C3,_tmp2C3,_tmp2C3 + 4U},{_tmp2C4,_tmp2C4,_tmp2C4 + 4U},{_tmp2C5,_tmp2C5,_tmp2C5 + 4U},{_tmp2C6,_tmp2C6,_tmp2C6 + 4U},{_tmp2C7,_tmp2C7,_tmp2C7 + 4U},{_tmp2C8,_tmp2C8,_tmp2C8 + 4U},{_tmp2C9,_tmp2C9,_tmp2C9 + 4U},{_tmp2CA,_tmp2CA,_tmp2CA + 4U},{_tmp2CB,_tmp2CB,_tmp2CB + 4U},{_tmp2CC,_tmp2CC,_tmp2CC + 4U},{_tmp2CD,_tmp2CD,_tmp2CD + 4U},{_tmp2CE,_tmp2CE,_tmp2CE + 4U},{_tmp2CF,_tmp2CF,_tmp2CF + 4U},{_tmp2D0,_tmp2D0,_tmp2D0 + 4U},{_tmp2D1,_tmp2D1,_tmp2D1 + 4U},{_tmp2D2,_tmp2D2,_tmp2D2 + 4U},{_tmp2D3,_tmp2D3,_tmp2D3 + 4U},{_tmp2D4,_tmp2D4,_tmp2D4 + 4U},{_tmp2D5,_tmp2D5,_tmp2D5 + 4U},{_tmp2D6,_tmp2D6,_tmp2D6 + 4U},{_tmp2D7,_tmp2D7,_tmp2D7 + 4U},{_tmp2D8,_tmp2D8,_tmp2D8 + 4U},{_tmp2D9,_tmp2D9,_tmp2D9 + 4U},{_tmp2DA,_tmp2DA,_tmp2DA + 5U},{_tmp2DB,_tmp2DB,_tmp2DB + 17U},{_tmp2DC,_tmp2DC,_tmp2DC + 18U},{_tmp2DD,_tmp2DD,_tmp2DD + 19U},{_tmp2DE,_tmp2DE,_tmp2DE + 16U},{_tmp2DF,_tmp2DF,_tmp2DF + 13U},{_tmp2E0,_tmp2E0,_tmp2E0 + 14U},{_tmp2E1,_tmp2E1,_tmp2E1 + 17U},{_tmp2E2,_tmp2E2,_tmp2E2 + 16U},{_tmp2E3,_tmp2E3,_tmp2E3 + 12U},{_tmp2E4,_tmp2E4,_tmp2E4 + 19U},{_tmp2E5,_tmp2E5,_tmp2E5 + 13U},{_tmp2E6,_tmp2E6,_tmp2E6 + 21U},{_tmp2E7,_tmp2E7,_tmp2E7 + 14U},{_tmp2E8,_tmp2E8,_tmp2E8 + 20U},{_tmp2E9,_tmp2E9,_tmp2E9 + 21U},{_tmp2EA,_tmp2EA,_tmp2EA + 13U},{_tmp2EB,_tmp2EB,_tmp2EB + 15U},{_tmp2EC,_tmp2EC,_tmp2EC + 17U},{_tmp2ED,_tmp2ED,_tmp2ED + 19U},{_tmp2EE,_tmp2EE,_tmp2EE + 12U},{_tmp2EF,_tmp2EF,_tmp2EF + 9U},{_tmp2F0,_tmp2F0,_tmp2F0 + 17U},{_tmp2F1,_tmp2F1,_tmp2F1 + 23U},{_tmp2F2,_tmp2F2,_tmp2F2 + 24U},{_tmp2F3,_tmp2F3,_tmp2F3 + 15U},{_tmp2F4,_tmp2F4,_tmp2F4 + 11U},{_tmp2F5,_tmp2F5,_tmp2F5 + 15U},{_tmp2F6,_tmp2F6,_tmp2F6 + 10U},{_tmp2F7,_tmp2F7,_tmp2F7 + 15U},{_tmp2F8,_tmp2F8,_tmp2F8 + 25U},{_tmp2F9,_tmp2F9,_tmp2F9 + 5U},{_tmp2FA,_tmp2FA,_tmp2FA + 15U},{_tmp2FB,_tmp2FB,_tmp2FB + 15U},{_tmp2FC,_tmp2FC,_tmp2FC + 11U},{_tmp2FD,_tmp2FD,_tmp2FD + 22U},{_tmp2FE,_tmp2FE,_tmp2FE + 26U},{_tmp2FF,_tmp2FF,_tmp2FF + 26U},{_tmp300,_tmp300,_tmp300 + 16U},{_tmp301,_tmp301,_tmp301 + 16U},{_tmp302,_tmp302,_tmp302 + 24U},{_tmp303,_tmp303,_tmp303 + 25U},{_tmp304,_tmp304,_tmp304 + 25U},{_tmp305,_tmp305,_tmp305 + 16U},{_tmp306,_tmp306,_tmp306 + 19U},{_tmp307,_tmp307,_tmp307 + 25U},{_tmp308,_tmp308,_tmp308 + 35U},{_tmp309,_tmp309,_tmp309 + 27U},{_tmp30A,_tmp30A,_tmp30A + 18U},{_tmp30B,_tmp30B,_tmp30B + 20U},{_tmp30C,_tmp30C,_tmp30C + 19U},{_tmp30D,_tmp30D,_tmp30D + 14U},{_tmp30E,_tmp30E,_tmp30E + 19U},{_tmp30F,_tmp30F,_tmp30F + 20U},{_tmp310,_tmp310,_tmp310 + 14U},{_tmp311,_tmp311,_tmp311 + 11U},{_tmp312,_tmp312,_tmp312 + 23U},{_tmp313,_tmp313,_tmp313 + 18U},{_tmp314,_tmp314,_tmp314 + 30U},{_tmp315,_tmp315,_tmp315 + 8U},{_tmp316,_tmp316,_tmp316 + 12U},{_tmp317,_tmp317,_tmp317 + 14U},{_tmp318,_tmp318,_tmp318 + 13U},{_tmp319,_tmp319,_tmp319 + 23U},{_tmp31A,_tmp31A,_tmp31A + 14U},{_tmp31B,_tmp31B,_tmp31B + 18U},{_tmp31C,_tmp31C,_tmp31C + 8U},{_tmp31D,_tmp31D,_tmp31D + 11U},{_tmp31E,_tmp31E,_tmp31E + 20U},{_tmp31F,_tmp31F,_tmp31F + 9U},{_tmp320,_tmp320,_tmp320 + 16U},{_tmp321,_tmp321,_tmp321 + 19U},{_tmp322,_tmp322,_tmp322 + 10U},{_tmp323,_tmp323,_tmp323 + 16U},{_tmp324,_tmp324,_tmp324 + 11U},{_tmp325,_tmp325,_tmp325 + 14U},{_tmp326,_tmp326,_tmp326 + 11U},{_tmp327,_tmp327,_tmp327 + 15U},{_tmp328,_tmp328,_tmp328 + 22U},{_tmp329,_tmp329,_tmp329 + 16U},{_tmp32A,_tmp32A,_tmp32A + 17U},{_tmp32B,_tmp32B,_tmp32B + 12U},{_tmp32C,_tmp32C,_tmp32C + 18U},{_tmp32D,_tmp32D,_tmp32D + 17U},{_tmp32E,_tmp32E,_tmp32E + 12U},{_tmp32F,_tmp32F,_tmp32F + 16U},{_tmp330,_tmp330,_tmp330 + 11U},{_tmp331,_tmp331,_tmp331 + 10U},{_tmp332,_tmp332,_tmp332 + 14U},{_tmp333,_tmp333,_tmp333 + 15U},{_tmp334,_tmp334,_tmp334 + 20U},{_tmp335,_tmp335,_tmp335 + 27U},{_tmp336,_tmp336,_tmp336 + 25U},{_tmp337,_tmp337,_tmp337 + 10U},{_tmp338,_tmp338,_tmp338 + 18U},{_tmp339,_tmp339,_tmp339 + 21U},{_tmp33A,_tmp33A,_tmp33A + 19U},{_tmp33B,_tmp33B,_tmp33B + 16U},{_tmp33C,_tmp33C,_tmp33C + 20U},{_tmp33D,_tmp33D,_tmp33D + 15U},{_tmp33E,_tmp33E,_tmp33E + 20U},{_tmp33F,_tmp33F,_tmp33F + 12U},{_tmp340,_tmp340,_tmp340 + 15U},{_tmp341,_tmp341,_tmp341 + 12U},{_tmp342,_tmp342,_tmp342 + 20U},{_tmp343,_tmp343,_tmp343 + 19U},{_tmp344,_tmp344,_tmp344 + 20U},{_tmp345,_tmp345,_tmp345 + 21U},{_tmp346,_tmp346,_tmp346 + 21U},{_tmp347,_tmp347,_tmp347 + 12U},{_tmp348,_tmp348,_tmp348 + 17U},{_tmp349,_tmp349,_tmp349 + 19U},{_tmp34A,_tmp34A,_tmp34A + 14U},{_tmp34B,_tmp34B,_tmp34B + 17U},{_tmp34C,_tmp34C,_tmp34C + 23U},{_tmp34D,_tmp34D,_tmp34D + 13U},{_tmp34E,_tmp34E,_tmp34E + 14U},{_tmp34F,_tmp34F,_tmp34F + 16U},{_tmp350,_tmp350,_tmp350 + 16U},{_tmp351,_tmp351,_tmp351 + 8U},{_tmp352,_tmp352,_tmp352 + 19U},{_tmp353,_tmp353,_tmp353 + 20U},{_tmp354,_tmp354,_tmp354 + 14U},{_tmp355,_tmp355,_tmp355 + 19U},{_tmp356,_tmp356,_tmp356 + 20U},{_tmp357,_tmp357,_tmp357 + 11U},{_tmp358,_tmp358,_tmp358 + 22U},{_tmp359,_tmp359,_tmp359 + 20U},{_tmp35A,_tmp35A,_tmp35A + 23U},{_tmp35B,_tmp35B,_tmp35B + 20U},{_tmp35C,_tmp35C,_tmp35C + 22U},{_tmp35D,_tmp35D,_tmp35D + 23U},{_tmp35E,_tmp35E,_tmp35E + 24U},{_tmp35F,_tmp35F,_tmp35F + 24U},{_tmp360,_tmp360,_tmp360 + 15U},{_tmp361,_tmp361,_tmp361 + 20U},{_tmp362,_tmp362,_tmp362 + 22U},{_tmp363,_tmp363,_tmp363 + 17U},{_tmp364,_tmp364,_tmp364 + 20U},{_tmp365,_tmp365,_tmp365 + 26U},{_tmp366,_tmp366,_tmp366 + 12U},{_tmp367,_tmp367,_tmp367 + 14U},{_tmp368,_tmp368,_tmp368 + 12U},{_tmp369,_tmp369,_tmp369 + 18U},{_tmp36A,_tmp36A,_tmp36A + 16U},{_tmp36B,_tmp36B,_tmp36B + 17U},{_tmp36C,_tmp36C,_tmp36C + 15U},{_tmp36D,_tmp36D,_tmp36D + 9U},{_tmp36E,_tmp36E,_tmp36E + 13U},{_tmp36F,_tmp36F,_tmp36F + 12U},{_tmp370,_tmp370,_tmp370 + 12U},{_tmp371,_tmp371,_tmp371 + 11U},{_tmp372,_tmp372,_tmp372 + 11U},{_tmp373,_tmp373,_tmp373 + 11U},{_tmp374,_tmp374,_tmp374 + 16U},{_tmp375,_tmp375,_tmp375 + 17U},{_tmp376,_tmp376,_tmp376 + 19U},{_tmp377,_tmp377,_tmp377 + 17U},{_tmp378,_tmp378,_tmp378 + 19U},{_tmp379,_tmp379,_tmp379 + 25U},{_tmp37A,_tmp37A,_tmp37A + 26U},{_tmp37B,_tmp37B,_tmp37B + 9U},{_tmp37C,_tmp37C,_tmp37C + 20U},{_tmp37D,_tmp37D,_tmp37D + 17U},{_tmp37E,_tmp37E,_tmp37E + 18U},{_tmp37F,_tmp37F,_tmp37F + 11U},{_tmp380,_tmp380,_tmp380 + 12U}};
# 1583
static short Cyc_yyr1[548U]={0,153,154,154,154,154,154,154,154,154,154,154,154,155,156,157,158,159,159,160,160,160,161,161,162,162,162,163,163,164,164,165,165,165,166,166,167,167,167,167,168,168,169,170,171,172,173,173,173,173,173,173,174,174,175,175,176,176,176,176,176,176,176,176,176,176,176,177,177,177,177,177,177,177,178,178,179,180,180,181,181,181,181,182,182,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,184,185,185,185,186,186,186,187,187,188,188,188,189,189,189,190,190,191,191,192,192,193,193,194,194,195,195,196,196,197,198,198,198,198,198,198,199,199,199,199,199,199,200,200,201,201,201,201,202,202,203,203,203,204,204,205,205,205,205,206,206,206,207,207,208,208,209,209,210,210,210,210,210,210,210,210,210,211,211,211,211,211,211,211,211,211,211,212,212,213,214,214,215,215,215,215,215,215,215,215,215,216,216,216,217,217,218,218,218,219,219,219,220,220,221,221,221,221,221,222,222,223,223,224,224,225,225,226,226,227,227,228,228,228,228,229,229,230,230,231,231,231,232,233,233,234,234,235,235,235,235,235,236,236,236,236,237,237,238,238,239,239,240,240,241,241,241,241,241,242,242,243,243,243,244,244,244,244,244,244,244,244,244,245,245,245,245,246,246,246,246,246,246,247,248,248,249,249,250,250,250,250,250,250,250,250,251,251,251,251,251,251,252,252,252,252,252,252,253,253,253,253,254,254,255,255,255,255,255,255,255,255,256,257,257,258,258,259,259,260,260,261,261,262,262,263,263,264,264,265,265,265,266,266,267,267,268,268,269,269,269,269,269,270,271,272,272,272,272,272,272,272,272,272,272,272,272,272,272,272,272,272,273,273,273,274,274,275,275,276,276,276,277,277,278,278,279,279,279,280,280,280,280,280,280,280,280,280,280,280,281,281,281,281,281,281,281,282,283,283,284,284,285,285,286,286,287,287,288,288,289,289,290,290,290,291,291,292,292,293,293,294,294,294,294,295,295,296,296,296,297,297,298,298,298,298,298,298,298,298,298,298,298,298,298,298,298,298,298,298,298,298,298,299,299,299,299,300,301,301,302,302,302,303,303,304,304,304,305,305,306,307,307,307,308,308,309,309,309,309,309,309,309,309,309,309,309,310,310,310,310,311,311,311,311,311,311,311,311,311,311,311,312,313,313,314,314,314,314,314,315,315,316,316,317,317,318,318,319,319};
# 1641
static short Cyc_yyr2[548U]={0,1,2,3,5,3,5,8,3,3,3,3,0,1,1,2,1,0,4,1,2,3,0,1,4,3,4,2,3,0,4,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,6,4,0,5,1,2,1,2,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,3,4,4,1,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,8,3,2,1,1,1,0,3,0,1,1,2,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,3,2,2,0,3,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,5,4,4,2,1,1,3,4,4,5,5,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,5,4,2,0,4,4,8,1,1,1,1,1,1,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,9,8,0,1,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,1,3,1,4,1,2,4,2,6,1,1,1,3,1,2,1,3,6,6,4,4,5,4,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,1,2,2,2,2,2,4,2,6,4,6,6,9,11,4,6,6,4,2,2,4,1,1,1,1,5,0,1,0,2,3,1,3,0,2,3,1,3,4,0,1,2,1,3,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,4,1,1,1,3,2,5,5,4,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1699
static short Cyc_yydefact[1067U]={0,33,67,68,69,70,72,85,86,87,88,89,90,91,92,93,109,110,111,125,126,63,0,0,97,0,0,73,0,0,161,104,106,0,0,0,13,14,0,0,0,538,227,540,539,541,0,212,0,100,0,212,211,1,0,0,0,0,31,0,0,32,0,56,65,59,83,61,94,95,0,124,98,0,0,172,0,195,198,99,176,127,71,70,64,0,113,0,58,536,0,538,533,534,535,537,0,127,0,0,373,0,0,0,250,0,377,375,42,44,0,0,52,0,0,0,0,123,162,0,0,0,209,0,0,0,0,210,0,0,0,2,0,0,0,0,46,0,133,135,57,66,60,62,542,543,127,129,127,0,54,0,0,35,0,229,0,184,173,196,0,202,203,207,208,0,206,205,204,217,198,0,84,71,117,0,115,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,522,523,486,0,0,0,484,0,483,481,482,0,402,404,418,426,428,430,432,434,436,438,440,443,445,458,0,460,504,521,519,538,385,0,0,0,0,386,0,0,376,49,0,0,127,0,0,0,142,138,140,270,272,0,0,0,0,0,8,9,0,544,545,228,108,0,0,0,177,101,248,0,245,10,11,0,3,0,5,0,47,0,0,0,35,122,0,130,131,154,0,159,0,0,0,0,0,0,0,0,0,0,0,0,538,302,304,0,312,306,0,310,295,296,297,0,298,299,300,0,55,35,135,34,36,277,0,235,251,0,0,231,229,0,214,0,0,0,219,74,218,199,0,118,114,0,0,0,467,0,0,479,420,458,0,421,422,0,0,0,0,0,0,0,0,0,0,0,461,462,487,478,0,464,0,0,0,0,463,0,96,0,0,0,0,0,0,447,448,0,451,452,449,450,0,0,0,453,454,0,455,456,457,0,0,408,409,410,411,412,413,414,415,416,417,407,0,465,0,510,511,0,0,0,525,0,127,378,0,0,0,399,538,545,0,0,0,0,266,395,400,0,397,0,0,374,392,393,0,390,252,0,0,0,0,273,0,243,143,148,144,146,139,141,229,0,279,271,280,547,546,0,103,105,0,51,0,107,80,79,0,77,213,178,229,247,174,279,249,185,186,0,102,16,29,43,0,45,0,134,136,254,253,35,37,127,120,132,0,0,0,150,157,0,127,167,0,0,0,0,0,538,0,330,0,333,334,335,0,0,337,0,0,0,313,307,135,311,305,303,38,0,183,236,0,0,0,242,230,237,291,0,222,0,231,182,216,215,179,214,0,0,220,75,128,119,425,116,112,0,0,0,0,538,255,260,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,524,531,0,530,403,427,0,429,431,433,435,437,439,441,442,444,446,406,405,509,506,0,508,0,0,0,388,389,0,269,0,396,264,267,384,0,265,387,381,0,48,0,382,0,274,0,149,145,147,0,0,214,0,281,0,229,0,290,276,0,0,0,0,0,142,0,127,0,229,0,194,175,246,0,22,4,6,39,231,0,153,137,154,0,0,152,160,169,168,0,0,163,0,0,0,320,0,0,0,330,0,331,332,336,0,0,0,308,301,0,40,278,229,0,239,0,0,0,0,181,232,0,235,223,180,200,201,220,197,466,0,0,0,256,0,261,469,0,0,0,0,0,520,474,477,0,0,480,488,0,0,459,528,0,0,507,505,0,0,0,0,268,398,401,391,394,383,275,244,291,282,283,214,0,0,214,0,0,50,538,0,76,78,0,187,0,0,0,214,0,0,0,17,23,129,151,0,155,158,170,167,167,0,0,0,0,0,0,0,0,0,330,320,338,0,309,41,231,0,240,238,0,0,0,231,0,221,516,0,515,0,257,262,0,0,0,0,0,423,424,509,508,493,0,529,512,0,532,419,526,527,0,379,380,287,285,289,291,284,214,53,0,81,188,193,291,192,189,214,0,0,0,0,0,0,0,166,165,314,320,0,0,0,0,0,340,341,343,345,347,349,351,353,355,357,360,362,364,366,371,372,0,0,317,326,0,330,0,0,339,225,241,0,0,233,224,229,468,0,0,263,470,471,0,0,476,475,0,499,493,489,491,485,513,0,288,286,0,191,190,30,25,0,0,35,0,7,121,156,0,0,0,320,0,369,0,0,320,0,0,0,0,0,0,0,0,0,0,0,0,0,367,320,0,0,330,319,292,293,0,231,518,517,0,0,0,0,500,499,496,494,0,490,514,0,26,24,0,27,0,19,171,315,316,0,0,0,0,320,322,344,0,346,348,350,352,354,356,358,359,361,363,0,321,327,0,0,0,234,226,0,0,0,0,0,502,501,0,495,492,0,28,18,20,0,318,368,0,365,323,0,320,329,0,0,259,258,472,0,498,0,497,82,21,0,342,320,324,328,0,0,503,370,325,294,473,0,0,0};
# 1809
static short Cyc_yydefgoto[167U]={1064,53,54,55,56,490,860,987,780,781,931,662,57,320,58,304,59,492,60,494,61,252,151,62,63,558,244,476,477,245,66,259,246,68,174,175,69,70,71,172,282,283,137,138,284,247,458,506,507,673,72,73,677,678,679,74,508,75,482,76,77,169,170,78,122,554,335,716,637,79,327,548,708,540,544,545,452,328,268,103,104,571,497,572,432,433,434,248,321,322,638,464,707,307,308,309,310,311,312,795,313,688,314,872,873,874,875,876,877,878,879,880,881,882,883,884,885,886,887,435,444,445,436,437,438,315,208,412,209,563,210,211,212,213,214,215,216,217,218,219,385,390,395,399,220,221,222,367,368,831,917,918,976,919,978,1024,223,816,224,590,591,225,226,81,932,439,468};
# 1829
static short Cyc_yypact[1067U]={2868,- -32768,- -32768,- -32768,- -32768,58,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3682,162,- 56,- -32768,3682,3076,- -32768,- 36,13,- -32768,105,124,115,94,174,- -32768,- -32768,221,297,234,- -32768,264,- -32768,- -32768,- -32768,246,286,849,311,300,286,- -32768,- -32768,313,318,340,2724,- -32768,396,446,- -32768,867,3682,3682,3682,- -32768,3682,- -32768,- -32768,493,357,- -32768,- 36,3591,345,92,231,959,- -32768,- -32768,368,376,433,- -32768,- 36,463,5793,- -32768,- -32768,3128,253,- -32768,- -32768,- -32768,- -32768,428,368,482,5793,- -32768,477,3128,487,500,506,- -32768,- 7,- -32768,- -32768,3842,3842,523,540,2724,2724,5793,- -32768,- -32768,200,528,5793,- -32768,610,538,200,4162,- -32768,2724,2724,3011,- -32768,2724,3011,2724,3011,- -32768,249,- -32768,3456,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,368,4162,- 51,1681,- -32768,3591,867,555,3842,3762,4889,- -32768,345,- -32768,553,- -32768,- -32768,- -32768,- -32768,560,- -32768,- -32768,- -32768,67,959,3842,- -32768,- -32768,572,579,578,- 36,6108,593,6206,5793,5958,611,617,628,635,659,665,671,673,675,676,679,6206,6206,- -32768,- -32768,771,6258,2426,680,- -32768,6258,- -32768,- -32768,- -32768,48,- -32768,- -32768,- 52,714,672,670,682,607,- 23,416,- 4,143,- -32768,986,6258,180,- 55,- -32768,691,117,- -32768,3128,88,707,1071,710,325,2829,- -32768,- -32768,713,5793,368,2829,705,3922,4162,4242,4162,542,- -32768,- 8,- 8,727,720,709,- -32768,- -32768,358,- -32768,- -32768,- -32768,- -32768,44,719,715,- -32768,- -32768,229,378,- -32768,- -32768,- -32768,723,- -32768,725,- -32768,726,- -32768,610,5002,3591,555,728,729,4162,- -32768,634,- 36,731,733,126,734,4312,735,746,736,744,5115,2281,4312,284,737,- -32768,- -32768,738,1831,1831,867,1831,- -32768,- -32768,- -32768,743,- -32768,- -32768,- -32768,339,- -32768,555,730,- -32768,- -32768,739,78,760,- -32768,66,747,740,408,765,622,754,5793,3842,- -32768,750,- -32768,- -32768,78,- 36,- -32768,5793,751,2426,- -32768,4162,2426,- -32768,- -32768,- -32768,1298,- -32768,782,5793,5793,5793,5793,5793,5793,786,5793,4162,796,5793,- -32768,- -32768,- -32768,- -32768,768,- -32768,1831,769,382,5793,- -32768,5793,- -32768,6258,5793,6258,6258,6258,6258,- -32768,- -32768,6258,- -32768,- -32768,- -32768,- -32768,6258,6258,6258,- -32768,- -32768,6258,- -32768,- -32768,- -32768,6258,5793,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,5793,- -32768,200,- -32768,- -32768,5228,200,5793,- -32768,773,368,- -32768,770,789,791,- -32768,43,428,200,5793,3128,171,- -32768,- -32768,- -32768,797,790,788,3128,- -32768,- -32768,- -32768,792,800,- -32768,429,1071,799,3842,- -32768,804,808,- -32768,4242,4242,4242,- -32768,- -32768,3283,5341,280,- -32768,464,- -32768,- -32768,66,- -32768,- -32768,807,- -32768,831,- -32768,- -32768,812,813,818,- -32768,- -32768,3147,- -32768,584,285,- -32768,- -32768,- -32768,4162,- -32768,- -32768,902,- -32768,2724,- -32768,2724,- -32768,- -32768,- -32768,- -32768,555,- -32768,368,- -32768,- -32768,658,5793,430,- -32768,- 27,164,368,706,5793,5793,817,826,5793,816,925,2131,830,- -32768,- -32768,- -32768,513,915,- -32768,5454,1981,2571,- -32768,- -32768,3456,- -32768,- -32768,- -32768,- -32768,3842,- -32768,- -32768,4162,827,4002,- -32768,- -32768,819,678,66,- -32768,4082,740,- -32768,- -32768,- -32768,- -32768,622,832,35,373,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,833,840,839,862,838,- -32768,- -32768,637,5002,842,852,853,855,857,391,859,860,861,602,863,872,874,6056,- -32768,- -32768,865,875,- -32768,714,187,672,670,682,607,- 23,416,- 4,- 4,143,- -32768,- -32768,- -32768,- -32768,- -32768,868,- -32768,154,3842,4773,4162,- -32768,4162,- -32768,864,- -32768,- -32768,- -32768,- -32768,2685,- -32768,- -32768,- -32768,2972,- -32768,878,- -32768,137,- -32768,4162,- -32768,- -32768,- -32768,873,876,622,871,464,3842,1012,5567,- -32768,- -32768,5793,887,5906,879,44,3315,881,368,3842,3762,5680,- -32768,584,- -32768,890,972,- -32768,- -32768,- -32768,740,764,- -32768,- -32768,634,885,5793,- -32768,- -32768,- -32768,- -32768,916,- 36,520,401,410,5793,716,412,4312,911,5793,922,923,- -32768,- -32768,926,929,921,1831,- -32768,3591,- -32768,739,931,3842,- -32768,930,66,927,928,- -32768,- -32768,934,96,- -32768,- -32768,- -32768,- -32768,373,- -32768,932,141,932,936,- -32768,4425,- -32768,- -32768,5793,5793,1035,5793,5958,- -32768,- -32768,- -32768,200,200,- -32768,938,942,4541,- -32768,- -32768,5793,5793,- -32768,- -32768,78,646,950,951,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,678,- -32768,- -32768,622,78,943,622,939,440,- -32768,940,944,- -32768,- -32768,946,- -32768,78,947,948,622,955,3011,971,1053,- -32768,4162,- -32768,5793,- -32768,- -32768,968,63,706,4312,976,969,1349,967,978,4312,5793,981,5793,716,- -32768,982,- -32768,- -32768,740,209,- -32768,- -32768,5793,5793,990,740,4162,- -32768,- -32768,86,- -32768,5793,- -32768,- -32768,5002,979,985,987,984,- -32768,782,988,989,- 26,992,- -32768,- -32768,655,- -32768,- -32768,- -32768,- -32768,4773,- -32768,- -32768,- -32768,- -32768,- -32768,678,- -32768,622,- -32768,1002,- -32768,- -32768,- -32768,678,- -32768,- -32768,622,1000,226,1004,2724,1001,999,4162,- -32768,- -32768,1117,716,1019,6356,1017,2571,1014,- -32768,- 37,- -32768,1055,1009,639,607,- 23,703,- 4,143,- -32768,- -32768,- -32768,- -32768,1057,6258,1831,- -32768,- -32768,457,5793,1030,1029,- -32768,- -32768,- -32768,1023,1024,1032,- -32768,931,- -32768,281,302,- -32768,- -32768,- -32768,4162,1138,- -32768,- -32768,1031,59,309,- -32768,- -32768,- -32768,- -32768,4657,- -32768,- -32768,1037,- -32768,- -32768,- -32768,- -32768,1039,1040,535,493,- -32768,- -32768,- -32768,492,4312,1042,716,2426,- -32768,4162,1036,1526,6258,5793,6258,6258,6258,6258,6258,6258,6258,6258,6258,6258,5793,- -32768,716,1046,1038,5793,- -32768,1093,- -32768,66,740,- -32768,- -32768,2571,1041,1043,5793,1060,320,- -32768,- -32768,1063,- -32768,- -32768,1065,- -32768,- -32768,493,- -32768,1052,545,- -32768,- -32768,- -32768,1058,1056,1067,6258,716,- -32768,714,333,672,670,670,607,- 23,416,- 4,- 4,143,- -32768,337,- -32768,- -32768,4312,1059,1066,- -32768,- -32768,1069,669,1068,4162,501,- -32768,1073,1063,- -32768,- -32768,1074,- -32768,- -32768,- -32768,493,- -32768,932,141,- -32768,- -32768,5793,1526,- -32768,4312,5793,- -32768,- -32768,- -32768,1077,- -32768,1098,- -32768,- -32768,- -32768,197,- -32768,716,- -32768,- -32768,1079,1080,- -32768,- -32768,- -32768,- -32768,- -32768,1216,1217,- -32768};
# 1939
static short Cyc_yypgoto[167U]={- -32768,- 11,- -32768,- -32768,- -32768,- -32768,- -32768,188,- -32768,- -32768,236,- -32768,- -32768,- 267,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 69,- -32768,- 97,125,- -32768,- -32768,0,568,- -32768,194,- 190,1097,33,- -32768,- -32768,- 138,- -32768,- -32768,1184,56,442,- -32768,- -32768,952,945,- 103,243,- -32768,557,- -32768,- -32768,- -32768,14,- -32768,- -32768,90,- 251,1149,- 410,269,- -32768,1061,- -32768,- -32768,1179,- 395,- -32768,517,- 114,- 161,- 317,- 471,267,525,532,- 453,- 491,- 120,- 371,- 112,- -32768,- 176,- 162,- 579,- 333,- -32768,805,374,- 92,- 140,- 171,- 277,- 708,- 75,- -32768,- -32768,- 46,- 278,- -32768,- 104,- -32768,- 603,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,11,998,- -32768,619,795,- -32768,312,787,- -32768,- 141,- 422,- 150,- 376,- 343,- 356,866,- 344,- 355,- 368,- 332,- 388,361,364,367,363,- 187,453,458,- -32768,- -32768,- -32768,- -32768,335,- -32768,- 871,274,- -32768,892,220,- -32768,- 351,- -32768,192,448,- 68,- 67,- 115,- 95};
# 1963
static short Cyc_yytable[6509U]={64,593,145,146,260,150,267,603,336,618,260,550,369,500,646,481,374,573,249,250,351,64,600,266,596,64,530,531,153,533,599,338,352,67,746,413,595,105,598,342,348,326,279,329,285,977,131,377,123,842,536,703,266,671,67,456,420,64,67,601,602,474,946,64,64,64,609,64,675,676,150,386,387,659,64,157,463,41,87,711,305,171,316,668,798,465,44,421,286,915,67,378,586,280,709,484,67,67,67,659,67,228,496,254,255,317,947,67,1027,388,389,672,916,236,64,64,660,269,270,271,240,272,273,274,275,466,109,241,64,64,64,541,64,64,64,64,467,923,124,64,393,454,756,459,394,926,84,67,67,64,88,64,139,232,1049,469,229,475,157,834,712,67,67,67,543,67,67,67,67,700,714,465,67,82,915,230,694,375,660,467,285,- 544,67,376,67,641,42,42,140,141,142,- 164,143,542,65,782,895,975,486,152,562,561,281,334,287,41,641,537,424,539,316,466,604,263,44,65,518,486,106,65,114,905,325,525,425,906,48,538,112,653,229,- 229,665,499,- 229,305,305,110,305,41,423,557,318,560,759,455,443,113,44,230,785,65,443,653,257,815,111,65,65,65,513,65,258,514,152,456,456,456,65,414,415,416,755,396,306,41,152,41,457,64,573,467,106,375,44,43,44,45,118,85,562,962,397,398,106,744,448,64,504,607,115,305,620,610,64,64,543,64,65,65,430,431,67,257,417,617,375,19,20,418,419,258,65,65,65,742,65,65,65,65,67,792,1060,65,898,559,906,67,67,41,67,903,41,65,899,65,159,43,44,45,43,44,45,467,929,116,930,266,631,47,1014,47,862,480,562,843,229,318,846,461,64,51,52,51,52,276,119,266,277,- 251,120,855,- 251,805,266,230,543,900,901,821,16,17,18,812,257,969,532,723,41,207,739,771,258,67,152,573,43,44,45,267,526,234,654,121,460,486,771,803,527,480,461,106,88,125,106,461,266,106,256,306,306,375,306,106,126,697,486,266,979,128,971,486,904,619,129,699,266,916,80,1025,687,625,924,696,375,455,455,455,975,695,441,927,375,150,645,535,375,130,375,46,86,1038,745,65,107,1039,108,154,613,155,652,663,658,664,147,156,698,375,457,457,457,65,64,473,64,306,80,1017,65,65,171,65,761,- 15,667,80,821,487,391,392,80,375,372,488,709,773,462,588,144,64,375,148,132,133,80,67,729,67,64,762,375,562,64,176,325,483,790,549,107,375,266,375,774,543,775,791,820,796,173,454,107,447,67,266,509,628,669,666,375,670,231,67,80,80,65,67,674,826,1008,375,998,80,134,135,371,848,80,80,80,827,80,80,80,80,654,1005,375,968,46,821,177,715,961,1001,1002,233,642,1004,643,486,80,836,486,817,644,1000,41,1003,524,806,696,235,960,237,43,44,45,453,828,829,1057,487,1006,1007,106,176,305,989,316,238,375,344,106,347,349,349,1047,239,753,691,106,645,375,562,152,908,788,364,365,789,837,804,349,152,251,372,349,152,372,658,253,319,261,820,985,986,844,996,562,562,579,1031,47,264,1032,349,460,107,852,285,107,319,461,107,51,52,65,332,65,107,594,733,415,416,333,64,896,64,634,635,636,339,959,383,384,552,553,340,46,341,266,675,676,65,485,866,655,565,656,566,567,892,65,41,657,80,65,345,67,462,67,611,44,485,510,582,417,515,793,794,47,734,419,937,48,353,820,41,715,483,347,354,51,52,80,43,44,45,705,706,266,1054,355,939,47,721,722,857,504,356,1009,41,505,509,838,839,51,52,64,43,44,45,46,921,922,950,951,176,47,366,145,970,504,357,349,954,955,1044,375,358,51,52,864,865,888,359,543,360,1036,361,362,67,379,363,373,106,381,380,422,106,306,305,152,440,680,681,446,426,684,382,349,689,349,349,349,349,992,89,349,450,470,997,472,349,349,349,471,478,349,934,479,489,349,491,493,501,1011,502,511,520,64,278,521,990,189,145,988,512,516,519,522,534,529,539,46,528,305,41,547,564,107,377,546,467,349,43,44,45,107,65,64,65,1037,67,47,555,107,1053,504,580,551,562,585,614,587,612,51,52,262,41,92,196,197,93,94,95,453,145,44,623,817,509,615,67,616,622,624,626,485,627,346,485,632,201,630,1055,509,633,1040,509,80,647,80,331,648,64,649,651,650,661,1061,682,485,683,685,686,690,485,349,765,692,701,41,704,145,988,1056,713,717,718,305,44,65,46,719,720,- 544,724,67,47,41,725,726,48,727,106,728,736,747,44,748,51,52,136,730,731,732,47,735,689,740,48,737,743,741,749,754,453,757,51,52,758,760,766,769,306,772,778,779,784,7,8,9,10,11,12,13,14,15,16,17,18,19,20,160,161,162,163,164,64,349,165,166,167,786,797,400,168,799,22,23,24,375,800,65,801,802,325,807,738,824,809,810,840,841,498,30,811,818,849,306,107,67,31,32,107,830,832,845,850,847,851,853,854,65,38,401,402,403,404,405,406,407,408,409,410,856,39,858,40,485,859,863,867,868,890,891,894,897,893,902,689,89,912,909,485,925,411,485,556,910,911,323,913,914,349,787,920,928,935,907,933,42,43,936,45,46,498,325,65,574,575,576,577,578,938,940,581,49,50,584,943,945,948,949,958,963,964,965,966,589,967,592,973,306,974,982,983,984,90,991,995,1012,1013,1015,1023,1020,1021,915,1028,1030,427,349,234,428,92,1033,605,93,94,95,1034,429,44,1041,349,1035,1043,97,606,1042,98,1048,1045,589,99,689,100,101,1050,430,431,1058,1059,1062,1063,1065,1066,102,770,1051,1029,265,117,861,158,80,783,503,495,127,337,814,65,1016,813,808,349,621,449,952,107,751,629,953,944,957,597,640,956,1026,889,980,372,583,1052,0,0,0,999,0,0,349,349,0,0,0,0,0,0,1010,0,0,0,0,689,0,0,0,0,0,0,0,1019,0,972,1022,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,144,0,80,0,0,0,0,0,589,993,589,994,0,0,0,0,942,178,179,0,0,0,0,0,568,0,0,0,0,0,0,0,180,89,0,181,349,0,0,1018,0,182,0,0,0,0,0,183,184,185,186,187,0,0,498,0,188,0,0,0,189,0,0,190,191,0,192,0,869,870,0,0,0,0,0,144,0,0,193,0,0,0,194,195,89,0,0,0,0,1046,0,0,0,349,498,349,349,349,349,349,349,349,349,349,349,569,92,196,197,93,94,95,0,258,44,0,0,198,0,350,570,0,199,0,0,764,200,144,0,201,768,430,431,0,0,0,202,0,777,203,204,90,349,205,206,0,0,0,0,0,0,0,0,0,0,0,227,92,0,0,93,94,95,589,96,44,0,0,0,0,97,0,0,98,144,0,0,871,0,100,101,0,0,0,0,349,0,202,0,349,102,204,0,0,205,206,0,0,0,0,0,0,0,498,0,0,822,823,0,825,0,0,0,0,0,0,0,0,0,498,0,0,835,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,793,794,21,178,179,288,0,289,290,291,292,293,294,295,296,22,23,24,297,89,26,181,298,0,0,0,0,182,27,299,0,0,30,183,184,185,186,187,0,31,32,33,188,0,0,0,189,0,0,190,191,38,192,0,0,0,0,0,0,0,498,0,0,39,193,40,0,0,194,195,0,0,0,0,0,0,0,0,498,0,0,0,0,0,0,0,0,0,0,0,0,300,92,196,197,93,94,95,42,43,44,45,46,198,301,149,0,0,199,0,0,0,200,0,49,303,0,0,0,0,0,0,202,0,0,203,204,0,0,205,206,0,0,0,0,0,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,178,179,288,498,289,290,291,292,293,294,295,296,22,23,24,297,89,26,181,298,0,0,0,0,182,27,299,0,0,30,183,184,185,186,187,0,31,32,33,188,0,0,0,189,0,0,190,191,38,192,0,0,0,0,0,0,0,0,0,0,39,193,40,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,300,92,196,197,93,94,95,42,43,44,45,46,198,301,149,302,0,199,0,0,0,200,0,49,303,0,0,0,0,0,0,202,0,0,203,204,0,0,205,206,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,178,179,288,0,289,290,291,292,293,294,295,296,22,23,24,297,89,26,181,298,0,0,0,0,182,27,299,0,0,30,183,184,185,186,187,0,31,32,33,188,0,0,0,189,0,0,190,191,38,192,0,0,0,0,0,0,0,0,0,0,39,193,40,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,300,92,196,197,93,94,95,42,43,44,45,46,198,301,149,0,0,199,0,0,0,200,0,49,303,0,0,0,0,0,0,202,0,0,203,204,0,0,205,206,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,178,179,288,0,289,290,291,292,293,294,295,296,22,23,24,297,89,26,181,298,0,0,0,0,182,27,299,0,0,30,183,184,185,186,187,0,31,32,33,188,0,0,0,189,0,0,190,191,38,192,0,0,0,0,0,0,0,0,0,0,39,193,40,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,517,92,196,197,93,94,95,42,43,44,45,46,198,301,149,0,0,199,0,0,0,200,0,49,303,0,0,0,0,0,0,202,0,0,203,204,0,0,205,206,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,178,179,0,0,0,0,0,0,0,0,0,0,22,23,24,297,89,26,181,0,0,0,0,0,182,27,0,0,0,30,183,184,185,186,187,0,31,32,33,188,0,0,0,189,0,0,190,191,38,192,0,0,0,0,0,0,0,0,0,0,39,193,40,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,42,43,44,45,46,198,0,0,0,0,199,0,0,0,200,0,49,303,0,0,0,0,0,0,202,0,0,203,204,0,0,205,206,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,178,179,0,0,0,0,0,0,0,0,0,0,22,23,24,297,89,0,0,0,0,0,0,0,0,27,0,0,0,30,183,184,185,186,187,0,31,32,0,0,0,0,0,189,0,0,190,191,38,192,0,0,0,0,0,0,0,0,0,0,39,193,40,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,42,43,44,45,46,198,0,0,0,0,199,0,0,0,346,0,49,303,0,0,0,0,0,0,202,0,0,203,204,0,0,205,206,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,178,179,0,0,0,0,0,0,0,0,0,0,22,23,24,180,89,0,181,0,0,0,0,0,182,0,0,0,0,30,183,184,185,186,187,0,31,32,0,188,0,0,0,189,0,0,190,191,38,192,0,0,0,0,0,0,0,0,0,0,39,193,40,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,42,43,44,45,46,198,0,370,0,0,199,0,0,0,200,0,49,303,0,0,0,0,0,0,202,0,0,203,204,0,0,205,206,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,178,179,0,0,0,0,0,0,0,0,0,0,22,23,24,180,89,0,181,0,0,0,0,0,182,0,0,0,0,30,183,184,185,186,187,0,31,32,0,188,0,0,0,189,0,0,190,191,38,192,0,0,0,0,0,0,0,0,0,0,39,193,40,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,42,43,44,45,46,198,0,0,0,0,199,0,0,0,200,0,49,303,0,0,0,0,0,0,202,0,0,203,204,0,0,205,206,- 12,1,89,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,90,0,31,32,33,0,0,34,35,0,36,37,750,0,38,428,92,0,0,93,94,95,0,429,44,0,39,0,40,97,0,0,98,0,0,0,99,0,100,101,0,430,431,0,0,0,0,0,0,102,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,- 12,0,47,0,0,0,48,0,49,50,0,0,0,0,51,52,- 12,1,89,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,90,0,31,32,33,0,0,34,35,0,36,37,442,0,38,227,92,0,0,93,94,95,0,96,44,0,39,0,40,97,0,0,98,0,0,0,99,0,100,101,0,0,0,0,0,0,0,0,0,102,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,0,0,47,0,0,0,48,0,49,50,0,0,0,0,51,52,1,89,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,90,0,31,32,33,0,0,34,35,0,36,37,752,0,38,227,92,0,0,93,94,95,0,96,44,0,39,0,40,97,0,0,98,0,0,0,99,0,100,101,0,0,0,0,0,0,0,89,0,102,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,- 12,0,47,0,0,0,48,0,49,50,0,0,0,0,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,89,0,0,0,0,90,0,0,0,0,0,0,0,0,0,22,23,24,0,0,91,92,0,0,93,94,95,0,96,44,0,0,30,0,97,0,0,98,0,31,32,99,0,100,101,0,0,0,0,0,0,38,0,0,102,0,0,90,0,0,0,0,0,39,0,40,0,0,0,0,0,0,227,92,0,0,93,94,95,0,96,44,0,0,0,0,97,0,323,98,0,41,0,99,0,100,101,0,42,43,44,45,46,0,325,0,102,0,47,0,0,0,480,0,49,50,0,0,461,0,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,7,8,9,10,11,12,13,14,15,16,17,18,19,20,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,22,23,24,0,38,0,0,0,0,0,0,0,0,0,0,0,39,30,40,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,323,0,0,0,0,0,0,39,0,40,42,43,0,45,46,0,325,0,0,0,47,0,0,0,460,0,49,50,0,0,461,0,51,52,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,0,0,47,0,0,0,504,0,49,50,0,0,0,0,51,52,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,0,0,149,0,0,0,278,0,0,0,0,49,50,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,42,43,0,45,46,0,0,149,22,23,24,25,0,0,0,0,49,50,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,42,43,0,45,46,0,0,0,0,0,0,0,0,0,30,0,49,50,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,323,0,0,324,0,0,0,22,23,24,42,43,0,45,46,0,325,0,0,0,0,0,0,0,30,0,49,50,0,0,0,31,32,0,0,242,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,42,43,0,45,46,0,0,243,0,0,0,0,0,0,30,0,49,50,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,42,43,0,45,46,0,0,0,451,0,0,0,0,0,30,0,49,50,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,42,43,0,45,46,0,0,0,702,0,0,0,0,0,30,0,49,50,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,710,0,0,0,0,0,0,22,23,24,42,43,0,45,46,0,0,0,0,0,0,0,0,0,30,0,49,50,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,42,43,0,45,46,0,0,0,0,0,0,0,0,0,30,0,49,50,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,178,179,288,0,289,290,291,292,293,294,295,296,0,0,0,180,89,0,181,298,0,0,0,0,182,42,299,0,0,46,183,184,185,186,187,0,0,0,0,188,0,49,50,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,517,92,196,197,93,94,95,0,0,44,0,0,198,301,149,0,0,199,0,0,0,200,0,0,201,178,179,0,0,0,0,202,0,0,203,204,0,0,205,206,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,569,92,196,197,93,94,95,0,258,44,0,0,198,0,350,819,0,199,0,0,0,200,0,0,201,0,430,431,178,179,0,202,0,0,203,204,0,0,205,206,0,0,0,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,569,92,196,197,93,94,95,0,258,44,0,0,198,0,350,833,0,199,0,0,0,200,0,0,201,0,430,431,178,179,0,202,0,0,203,204,0,0,205,206,0,0,0,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,569,92,196,197,93,94,95,0,258,44,0,0,198,0,350,981,0,199,0,0,0,200,0,0,201,0,430,431,178,179,0,202,0,0,203,204,0,0,205,206,0,0,0,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,569,92,196,197,93,94,95,0,258,44,0,0,198,0,350,0,0,199,0,0,0,200,0,0,201,0,430,431,178,179,0,202,0,0,203,204,0,0,205,206,0,0,0,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,0,0,44,0,0,198,0,0,0,0,199,0,0,0,200,0,0,201,178,179,0,330,0,0,202,0,0,203,204,0,0,205,206,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,0,0,44,0,0,198,0,350,0,0,199,0,0,0,200,0,0,201,178,179,0,0,0,0,202,0,0,203,204,0,0,205,206,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,0,0,44,0,0,198,523,0,0,0,199,0,0,0,200,0,0,201,178,179,0,0,0,0,202,0,0,203,204,0,0,205,206,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,0,0,44,0,0,198,0,0,0,0,199,0,0,0,200,608,0,201,178,179,0,0,0,0,202,0,0,203,204,0,0,205,206,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,0,0,44,0,0,198,0,0,0,0,199,0,0,0,200,0,0,201,178,179,0,639,0,0,202,0,0,203,204,0,0,205,206,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,0,0,44,0,0,198,0,0,0,0,199,0,0,0,200,693,0,201,178,179,0,0,0,0,202,0,0,203,204,0,0,205,206,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,0,0,44,0,0,198,0,0,0,0,199,0,0,0,200,0,0,201,178,179,0,763,0,0,202,0,0,203,204,0,0,205,206,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,0,0,44,0,0,198,0,0,0,0,199,0,0,0,200,0,0,201,178,179,0,776,0,0,202,0,0,203,204,0,0,205,206,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,0,0,44,0,0,198,0,0,0,0,199,0,0,0,200,0,0,201,178,179,0,0,0,0,202,0,0,203,204,0,0,205,206,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,178,179,0,0,0,0,0,0,0,193,0,0,0,194,195,180,89,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,185,186,187,0,767,92,196,197,93,94,95,189,0,44,190,191,198,192,0,0,0,199,0,0,0,200,0,0,201,193,0,0,0,194,195,202,0,0,203,204,0,0,205,206,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,0,0,44,178,179,198,0,350,0,0,199,0,0,0,200,0,0,201,180,89,0,0,0,0,202,0,0,203,204,0,0,205,206,183,184,185,186,187,0,0,0,0,0,0,0,0,189,0,0,190,191,0,192,0,0,178,179,0,0,0,0,0,0,0,193,0,0,0,194,195,180,89,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,185,186,187,0,41,92,196,197,93,94,95,189,0,44,190,191,198,192,738,0,0,199,0,0,0,200,0,0,201,193,0,0,0,194,195,202,0,0,203,204,0,0,205,206,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,0,0,44,178,179,198,0,0,0,0,199,0,0,0,343,0,0,201,180,89,0,0,0,0,202,0,0,203,204,0,0,205,206,183,184,185,186,187,0,0,0,0,0,0,0,0,189,0,0,190,191,0,192,0,0,178,179,0,0,0,0,0,0,0,193,0,0,0,194,195,180,89,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,185,186,187,0,41,92,196,197,93,94,95,189,0,44,190,191,198,192,0,0,0,199,0,0,0,346,0,0,201,193,0,0,0,194,195,202,0,0,203,204,0,0,205,206,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,0,0,44,178,179,198,0,0,0,0,199,0,0,0,200,0,0,201,180,89,0,0,0,0,202,0,0,203,204,0,0,205,206,183,184,185,186,187,0,0,0,0,0,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,0,0,44,0,0,198,0,0,0,0,199,0,0,0,941,0,0,201,0,0,0,0,0,0,202,0,0,203,204,0,0,205,206};
# 2617
static short Cyc_yycheck[6509U]={0,377,70,70,119,74,126,395,169,431,125,328,199,280,467,266,203,350,110,111,182,21,390,126,380,25,304,305,74,307,385,171,182,0,613,222,379,26,382,177,181,155,139,155,147,916,57,99,48,757,317,542,155,80,21,245,111,57,25,391,392,17,99,63,64,65,417,67,5,6,139,94,95,483,74,75,247,113,134,550,149,132,151,505,687,93,122,142,139,115,57,143,370,139,547,266,63,64,65,509,67,90,278,114,115,151,143,74,979,132,133,138,138,102,114,115,487,128,129,130,127,132,133,134,135,133,113,134,128,129,130,65,132,133,134,135,144,845,48,139,144,244,633,246,148,853,21,114,115,149,25,151,62,97,1025,250,113,113,158,738,555,128,129,130,325,132,133,134,135,540,135,93,139,115,115,132,527,129,549,144,283,138,149,135,151,462,120,120,63,64,65,128,67,127,0,666,799,138,266,74,341,339,146,136,148,113,483,129,120,113,279,133,399,123,122,21,291,285,26,25,126,135,126,298,136,139,134,322,113,480,113,135,499,279,138,304,305,132,307,113,229,333,152,338,639,245,235,132,122,132,672,57,241,504,113,114,132,63,64,65,134,67,121,137,139,455,456,457,74,89,90,91,135,130,149,113,151,113,245,279,613,144,90,129,122,121,122,123,54,127,431,894,149,150,102,141,240,297,134,414,126,370,131,418,304,305,467,307,114,115,139,140,279,113,134,430,129,20,21,139,140,121,128,129,130,138,132,133,134,135,297,682,135,139,805,335,139,304,305,113,307,812,113,149,135,151,77,121,122,123,121,122,123,144,128,134,130,460,450,130,963,130,784,134,505,760,113,277,763,140,370,142,143,142,143,126,112,480,129,126,134,776,129,700,487,132,547,809,810,722,17,18,19,710,113,114,306,573,113,87,587,652,121,370,279,738,121,122,123,529,126,99,480,127,134,483,667,695,134,134,140,229,297,112,232,140,529,235,116,304,305,129,307,241,134,532,504,540,129,126,138,509,813,432,126,537,549,138,0,129,519,440,847,528,129,455,456,457,138,528,135,856,129,532,464,126,129,127,129,124,22,138,612,279,26,138,28,132,422,134,480,492,482,494,127,140,532,129,455,456,457,297,492,135,494,370,48,968,304,305,132,307,642,127,504,57,839,129,92,93,62,129,200,135,967,655,247,135,70,519,129,73,126,127,76,492,135,494,528,643,129,672,532,85,126,266,135,129,90,129,643,129,656,704,656,135,722,135,115,652,102,239,519,656,285,126,126,501,129,129,132,528,114,115,370,532,510,729,956,129,946,123,126,127,200,135,128,129,130,729,132,133,134,135,652,953,129,904,124,922,127,558,135,949,950,113,132,952,134,667,152,742,670,718,140,948,113,951,296,701,685,134,890,126,121,122,123,243,733,734,1042,129,954,955,432,177,695,135,697,129,129,178,440,180,181,182,135,131,627,126,448,641,129,784,519,821,126,194,195,129,745,697,199,528,131,343,203,532,346,659,120,126,134,839,129,932,761,945,809,810,358,126,130,135,129,222,134,229,773,782,232,126,140,235,142,143,492,134,494,241,378,89,90,91,134,695,800,697,455,456,457,129,889,96,97,83,84,128,124,131,813,5,6,519,266,790,132,343,134,345,346,796,528,113,140,277,532,134,695,460,697,419,122,285,286,361,134,289,22,23,130,139,140,863,134,134,922,113,715,480,297,134,142,143,306,121,122,123,80,81,863,1039,134,867,130,128,129,778,134,134,957,113,138,504,128,129,142,143,778,121,122,123,124,128,129,146,147,339,130,18,858,906,134,134,341,92,93,128,129,134,142,143,788,789,793,134,967,134,995,134,134,778,98,134,134,623,146,145,127,627,695,890,697,113,512,513,113,120,516,147,377,519,379,380,381,382,940,41,385,134,113,945,133,390,391,392,126,128,395,860,135,128,399,128,128,127,960,128,127,113,860,131,126,938,68,933,933,134,134,134,126,128,134,113,124,138,945,113,138,128,432,99,135,144,431,121,122,123,440,695,890,697,996,860,130,141,448,1038,134,113,135,1042,134,133,135,132,142,143,121,113,114,115,116,117,118,119,542,985,122,129,1035,652,133,890,133,128,138,135,480,129,134,483,128,137,135,1039,667,129,1013,670,492,134,494,156,113,945,134,129,135,47,1054,134,504,127,138,30,126,509,505,647,45,134,113,144,1032,1032,1041,135,135,129,1039,122,778,124,135,113,138,135,945,130,113,129,129,134,129,793,129,115,614,122,616,142,143,126,135,135,135,130,135,687,135,134,128,135,129,141,128,633,135,142,143,135,141,126,135,890,135,127,46,134,8,9,10,11,12,13,14,15,16,17,18,19,20,21,75,76,77,78,79,1039,587,82,83,84,128,134,60,88,126,37,38,39,129,127,860,126,135,126,128,127,25,134,134,113,113,278,54,133,132,129,945,623,1039,61,62,627,138,135,135,135,141,135,135,135,890,73,100,101,102,103,104,105,106,107,108,109,141,85,127,87,652,48,134,127,135,138,128,126,126,797,120,799,41,129,135,667,114,131,670,332,135,134,110,135,135,672,678,135,128,128,818,127,120,121,135,123,124,350,126,945,353,354,355,356,357,28,127,360,136,137,363,134,138,98,145,98,126,128,135,135,373,129,375,25,1039,134,129,128,128,98,128,135,126,135,81,115,135,134,115,114,128,110,729,871,113,114,128,400,117,118,119,135,121,122,135,742,129,128,127,412,134,130,129,135,417,134,894,136,137,135,139,140,135,115,135,135,0,0,147,651,1032,985,125,39,782,76,778,670,283,277,51,170,715,1039,967,710,704,784,433,241,879,793,623,448,880,871,883,381,461,882,976,793,917,941,362,1035,- 1,- 1,- 1,947,- 1,- 1,809,810,- 1,- 1,- 1,- 1,- 1,- 1,958,- 1,- 1,- 1,- 1,963,- 1,- 1,- 1,- 1,- 1,- 1,- 1,971,- 1,911,974,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,858,- 1,860,- 1,- 1,- 1,- 1,- 1,527,941,529,943,- 1,- 1,- 1,- 1,869,25,26,- 1,- 1,- 1,- 1,- 1,32,- 1,- 1,- 1,- 1,- 1,- 1,- 1,40,41,- 1,43,889,- 1,- 1,971,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,573,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,25,26,- 1,- 1,- 1,- 1,- 1,933,- 1,- 1,86,- 1,- 1,- 1,90,91,41,- 1,- 1,- 1,- 1,1021,- 1,- 1,- 1,946,613,948,949,950,951,952,953,954,955,956,957,113,114,115,116,117,118,119,- 1,121,122,- 1,- 1,125,- 1,127,128,- 1,130,- 1,- 1,644,134,985,- 1,137,649,139,140,- 1,- 1,- 1,144,- 1,657,147,148,98,995,151,152,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,- 1,117,118,119,682,121,122,- 1,- 1,- 1,- 1,127,- 1,- 1,130,1032,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,1038,- 1,144,- 1,1042,147,148,- 1,- 1,151,152,- 1,- 1,- 1,- 1,- 1,- 1,- 1,722,- 1,- 1,725,726,- 1,728,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,738,- 1,- 1,741,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,821,- 1,- 1,85,86,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,839,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,922,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,43,- 1,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,0,1,41,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,98,- 1,61,62,63,- 1,- 1,66,67,- 1,69,70,110,- 1,73,113,114,- 1,- 1,117,118,119,- 1,121,122,- 1,85,- 1,87,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,139,140,- 1,- 1,- 1,- 1,- 1,- 1,147,- 1,- 1,- 1,- 1,113,- 1,- 1,- 1,- 1,- 1,- 1,120,121,122,123,124,- 1,- 1,- 1,128,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,142,143,0,1,41,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,98,- 1,61,62,63,- 1,- 1,66,67,- 1,69,70,110,- 1,73,113,114,- 1,- 1,117,118,119,- 1,121,122,- 1,85,- 1,87,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,147,- 1,- 1,- 1,- 1,113,- 1,- 1,- 1,- 1,- 1,- 1,120,121,122,123,124,- 1,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,142,143,1,41,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,98,- 1,61,62,63,- 1,- 1,66,67,- 1,69,70,110,- 1,73,113,114,- 1,- 1,117,118,119,- 1,121,122,- 1,85,- 1,87,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,41,- 1,147,- 1,- 1,- 1,- 1,113,- 1,- 1,- 1,- 1,- 1,- 1,120,121,122,123,124,- 1,- 1,- 1,128,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,142,143,8,9,10,11,12,13,14,15,16,17,18,19,20,21,41,- 1,- 1,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,113,114,- 1,- 1,117,118,119,- 1,121,122,- 1,- 1,54,- 1,127,- 1,- 1,130,- 1,61,62,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,147,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,- 1,117,118,119,- 1,121,122,- 1,- 1,- 1,- 1,127,- 1,110,130,- 1,113,- 1,134,- 1,136,137,- 1,120,121,122,123,124,- 1,126,- 1,147,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,140,- 1,142,143,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,8,9,10,11,12,13,14,15,16,17,18,19,20,21,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,54,87,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,120,121,- 1,123,124,- 1,126,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,140,- 1,142,143,- 1,113,- 1,- 1,- 1,- 1,- 1,- 1,120,121,122,123,124,- 1,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,142,143,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,120,121,- 1,123,124,- 1,- 1,127,- 1,- 1,- 1,131,- 1,- 1,- 1,- 1,136,137,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,120,121,- 1,123,124,- 1,- 1,127,37,38,39,40,- 1,- 1,- 1,- 1,136,137,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,120,121,- 1,123,124,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,136,137,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,- 1,- 1,113,- 1,- 1,- 1,37,38,39,120,121,- 1,123,124,- 1,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,136,137,- 1,- 1,- 1,61,62,- 1,- 1,65,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,120,121,- 1,123,124,- 1,- 1,127,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,136,137,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,120,121,- 1,123,124,- 1,- 1,- 1,128,- 1,- 1,- 1,- 1,- 1,54,- 1,136,137,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,120,121,- 1,123,124,- 1,- 1,- 1,128,- 1,- 1,- 1,- 1,- 1,54,- 1,136,137,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,120,121,- 1,123,124,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,136,137,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,120,121,- 1,123,124,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,136,137,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,- 1,- 1,40,41,- 1,43,44,- 1,- 1,- 1,- 1,49,120,51,- 1,- 1,124,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,136,137,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,126,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,121,122,- 1,- 1,125,- 1,127,128,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,- 1,139,140,25,26,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,121,122,- 1,- 1,125,- 1,127,128,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,- 1,139,140,25,26,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,121,122,- 1,- 1,125,- 1,127,128,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,- 1,139,140,25,26,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,121,122,- 1,- 1,125,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,- 1,139,140,25,26,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,141,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,126,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,141,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,141,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,141,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,113,114,115,116,117,118,119,68,- 1,122,71,72,125,74,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,86,- 1,- 1,- 1,90,91,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,25,26,125,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,40,41,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,113,114,115,116,117,118,119,68,- 1,122,71,72,125,74,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,86,- 1,- 1,- 1,90,91,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,25,26,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,40,41,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,113,114,115,116,117,118,119,68,- 1,122,71,72,125,74,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,86,- 1,- 1,- 1,90,91,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,25,26,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,40,41,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152};char Cyc_Yystack_overflow[17U]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
extern void Cyc_yyerror(struct _fat_ptr,int,int);
# 82 "cycbison.simple"
extern int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*,struct Cyc_Yyltype*);struct Cyc_Yystacktype{union Cyc_YYSTYPE v;struct Cyc_Yyltype l;};struct _tuple33{unsigned f1;struct _tuple0*f2;int f3;};struct _tuple34{void*f1;void*f2;};struct _tuple35{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 145 "cycbison.simple"
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 148
struct _RegionHandle _tmp381=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp381;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 154
int yychar;
union Cyc_YYSTYPE yylval=({union Cyc_YYSTYPE _tmp6DF;(_tmp6DF.YYINITIALSVAL).tag=73U,(_tmp6DF.YYINITIALSVAL).val=0;_tmp6DF;});
int yynerrs;
# 158
struct Cyc_Yyltype yylloc;
# 162
int yyssp_offset;
# 164
struct _fat_ptr yyss=({unsigned _tmp621=200U;_tag_fat(_region_calloc(yyregion,sizeof(short),_tmp621),sizeof(short),_tmp621);});
# 166
int yyvsp_offset;
# 168
struct _fat_ptr yyvs=
_tag_fat(({unsigned _tmp620=200U;struct Cyc_Yystacktype*_tmp61F=({struct _RegionHandle*_tmp78D=yyregion;_region_malloc(_tmp78D,_check_times(_tmp620,sizeof(struct Cyc_Yystacktype)));});({{unsigned _tmp6DE=200U;unsigned i;for(i=0;i < _tmp6DE;++ i){(_tmp61F[i]).v=yylval,({struct Cyc_Yyltype _tmp78E=Cyc_yynewloc();(_tmp61F[i]).l=_tmp78E;});}}0;});_tmp61F;}),sizeof(struct Cyc_Yystacktype),200U);
# 174
struct Cyc_Yystacktype*yyyvsp;
# 177
int yystacksize=200;
# 179
union Cyc_YYSTYPE yyval=yylval;
# 183
int yylen;
# 190
yystate=0;
yyerrstatus=0;
yynerrs=0;
yychar=-2;
# 200
yyssp_offset=-1;
yyvsp_offset=0;
# 206
yynewstate:
# 208
*((short*)_check_fat_subscript(yyss,sizeof(short),++ yyssp_offset))=(short)yystate;
# 210
if(yyssp_offset >= (yystacksize - 1)- 12){
# 212
if(yystacksize >= 10000){
({struct _fat_ptr _tmp790=({const char*_tmp382="parser stack overflow";_tag_fat(_tmp382,sizeof(char),22U);});int _tmp78F=yystate;Cyc_yyerror(_tmp790,_tmp78F,yychar);});
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 216
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
struct _fat_ptr yyss1=({unsigned _tmp386=(unsigned)yystacksize;short*_tmp385=({struct _RegionHandle*_tmp791=yyregion;_region_malloc(_tmp791,_check_times(_tmp386,sizeof(short)));});({{unsigned _tmp694=(unsigned)yystacksize;unsigned i;for(i=0;i < _tmp694;++ i){
i <= (unsigned)yyssp_offset?_tmp385[i]=*((short*)_check_fat_subscript(yyss,sizeof(short),(int)i)):(_tmp385[i]=0);}}0;});_tag_fat(_tmp385,sizeof(short),_tmp386);});
# 222
struct _fat_ptr yyvs1=({unsigned _tmp384=(unsigned)yystacksize;struct Cyc_Yystacktype*_tmp383=({struct _RegionHandle*_tmp792=yyregion;_region_malloc(_tmp792,_check_times(_tmp384,sizeof(struct Cyc_Yystacktype)));});({{unsigned _tmp693=(unsigned)yystacksize;unsigned i;for(i=0;i < _tmp693;++ i){
# 224
i <= (unsigned)yyssp_offset?_tmp383[i]=*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(int)i)):(_tmp383[i]=*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),0)));}}0;});_tag_fat(_tmp383,sizeof(struct Cyc_Yystacktype),_tmp384);});
# 230
yyss=yyss1;
yyvs=yyvs1;}}
# 240
goto yybackup;
# 242
yybackup:
# 254 "cycbison.simple"
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1067U,sizeof(short),yystate));
if(yyn == -32768)goto yydefault;
# 261
if(yychar == -2)
# 267
yychar=Cyc_yylex(yylex_buf,& yylval,& yylloc);
# 271
if(yychar <= 0){
# 273
yychar1=0;
yychar=0;}else{
# 282
yychar1=yychar > 0 && yychar <= 380?(int)Cyc_yytranslate[yychar]: 320;}
# 299 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0 || yyn > 6508)||(int)Cyc_yycheck[yyn]!= yychar1)goto yydefault;
# 302
yyn=(int)Cyc_yytable[yyn];
# 309
if(yyn < 0){
# 311
if(yyn == -32768)goto yyerrlab;
yyn=- yyn;
goto yyreduce;}else{
# 315
if(yyn == 0)goto yyerrlab;}
# 317
if(yyn == 1066){
int _tmp387=0;_npop_handler(0);return _tmp387;}
# 328 "cycbison.simple"
if(yychar != 0)
yychar=-2;
# 332
({struct Cyc_Yystacktype _tmp793=({struct Cyc_Yystacktype _tmp695;_tmp695.v=yylval,_tmp695.l=yylloc;_tmp695;});((struct Cyc_Yystacktype*)yyvs.curr)[++ yyvsp_offset]=_tmp793;});
# 338
if(yyerrstatus != 0)-- yyerrstatus;
# 340
yystate=yyn;
goto yynewstate;
# 344
yydefault:
# 346
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yydefact,1067U,sizeof(short),yystate));
if(yyn == 0)goto yyerrlab;
# 351
yyreduce:
# 353
 yylen=(int)*((short*)_check_known_subscript_notnull(Cyc_yyr2,548U,sizeof(short),yyn));
yyyvsp=(struct Cyc_Yystacktype*)_check_null(_untag_fat_ptr(_fat_ptr_plus(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + 1)- yylen),sizeof(struct Cyc_Yystacktype),12U));
if(yylen > 0)
yyval=(yyyvsp[0]).v;
# 370 "cycbison.simple"
{int _tmp388=yyn;switch((int)_tmp388){case 1:
# 1141 "parse.y"
 yyval=(yyyvsp[0]).v;
Cyc_Parse_parse_result=Cyc_yyget_YY16(&(yyyvsp[0]).v);
# 1144
goto _LL0;case 2:
# 1147 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*(*_tmp795)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append;});struct Cyc_List_List*_tmp794=Cyc_yyget_YY16(&(yyyvsp[0]).v);_tmp795(_tmp794,Cyc_yyget_YY16(&(yyyvsp[1]).v));}));
goto _LL0;case 3:
# 1151 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp38B=_cycalloc(sizeof(*_tmp38B));({struct Cyc_Absyn_Decl*_tmp799=({struct Cyc_Absyn_Decl*_tmp38A=_cycalloc(sizeof(*_tmp38A));({void*_tmp798=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp389=_cycalloc(sizeof(*_tmp389));_tmp389->tag=10,({struct _tuple0*_tmp797=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp389->f1=_tmp797;}),({struct Cyc_List_List*_tmp796=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp389->f2=_tmp796;});_tmp389;});_tmp38A->r=_tmp798;}),_tmp38A->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp38A;});_tmp38B->hd=_tmp799;}),_tmp38B->tl=0;_tmp38B;}));
Cyc_Lex_leave_using();
# 1154
goto _LL0;case 4:
# 1155 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp38E=_cycalloc(sizeof(*_tmp38E));({struct Cyc_Absyn_Decl*_tmp79E=({struct Cyc_Absyn_Decl*_tmp38D=_cycalloc(sizeof(*_tmp38D));({void*_tmp79D=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp38C=_cycalloc(sizeof(*_tmp38C));_tmp38C->tag=10,({struct _tuple0*_tmp79C=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp38C->f1=_tmp79C;}),({struct Cyc_List_List*_tmp79B=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp38C->f2=_tmp79B;});_tmp38C;});_tmp38D->r=_tmp79D;}),_tmp38D->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp38D;});_tmp38E->hd=_tmp79E;}),({struct Cyc_List_List*_tmp79A=Cyc_yyget_YY16(&(yyyvsp[4]).v);_tmp38E->tl=_tmp79A;});_tmp38E;}));
goto _LL0;case 5:
# 1158
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp392=_cycalloc(sizeof(*_tmp392));({struct Cyc_Absyn_Decl*_tmp7A3=({struct Cyc_Absyn_Decl*_tmp391=_cycalloc(sizeof(*_tmp391));({void*_tmp7A2=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp390=_cycalloc(sizeof(*_tmp390));_tmp390->tag=9,({struct _fat_ptr*_tmp7A1=({struct _fat_ptr*_tmp38F=_cycalloc(sizeof(*_tmp38F));({struct _fat_ptr _tmp7A0=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp38F=_tmp7A0;});_tmp38F;});_tmp390->f1=_tmp7A1;}),({struct Cyc_List_List*_tmp79F=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp390->f2=_tmp79F;});_tmp390;});_tmp391->r=_tmp7A2;}),_tmp391->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp391;});_tmp392->hd=_tmp7A3;}),_tmp392->tl=0;_tmp392;}));
Cyc_Lex_leave_namespace();
# 1161
goto _LL0;case 6:
# 1162 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp396=_cycalloc(sizeof(*_tmp396));({struct Cyc_Absyn_Decl*_tmp7A9=({struct Cyc_Absyn_Decl*_tmp395=_cycalloc(sizeof(*_tmp395));({void*_tmp7A8=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp394=_cycalloc(sizeof(*_tmp394));_tmp394->tag=9,({struct _fat_ptr*_tmp7A7=({struct _fat_ptr*_tmp393=_cycalloc(sizeof(*_tmp393));({struct _fat_ptr _tmp7A6=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp393=_tmp7A6;});_tmp393;});_tmp394->f1=_tmp7A7;}),({struct Cyc_List_List*_tmp7A5=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp394->f2=_tmp7A5;});_tmp394;});_tmp395->r=_tmp7A8;}),_tmp395->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp395;});_tmp396->hd=_tmp7A9;}),({struct Cyc_List_List*_tmp7A4=Cyc_yyget_YY16(&(yyyvsp[4]).v);_tmp396->tl=_tmp7A4;});_tmp396;}));
goto _LL0;case 7:  {
# 1164 "parse.y"
int _tmp397=Cyc_yyget_YY32(&(yyyvsp[0]).v);int is_c_include=_tmp397;
struct Cyc_List_List*cycdecls=Cyc_yyget_YY16(&(yyyvsp[4]).v);
struct _tuple29*_tmp398=Cyc_yyget_YY54(&(yyyvsp[5]).v);struct _tuple29*_stmttmp15=_tmp398;struct _tuple29*_tmp399=_stmttmp15;unsigned _tmp39B;void*_tmp39A;_tmp39A=_tmp399->f1;_tmp39B=_tmp399->f2;{struct Cyc_List_List*exs=_tmp39A;unsigned wc=_tmp39B;
struct Cyc_List_List*_tmp39C=Cyc_yyget_YY55(&(yyyvsp[6]).v);struct Cyc_List_List*hides=_tmp39C;
if(exs != 0 && hides != 0)
({void*_tmp39D=0U;({unsigned _tmp7AB=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp7AA=({const char*_tmp39E="hide list can only be used with export { * }, or no export block";_tag_fat(_tmp39E,sizeof(char),65U);});Cyc_Warn_err(_tmp7AB,_tmp7AA,_tag_fat(_tmp39D,sizeof(void*),0));});});
if((unsigned)hides && !((int)wc))
wc=(unsigned)((yyyvsp[6]).l).first_line;
if(!is_c_include){
if(exs != 0 || cycdecls != 0){
({void*_tmp39F=0U;({unsigned _tmp7AD=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp7AC=({const char*_tmp3A0="expecting \"C include\"";_tag_fat(_tmp3A0,sizeof(char),22U);});Cyc_Warn_err(_tmp7AD,_tmp7AC,_tag_fat(_tmp39F,sizeof(void*),0));});});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3A4=_cycalloc(sizeof(*_tmp3A4));({struct Cyc_Absyn_Decl*_tmp7B2=({struct Cyc_Absyn_Decl*_tmp3A3=_cycalloc(sizeof(*_tmp3A3));({void*_tmp7B1=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3A2=_cycalloc(sizeof(*_tmp3A2));_tmp3A2->tag=12,({struct Cyc_List_List*_tmp7B0=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3A2->f1=_tmp7B0;}),_tmp3A2->f2=cycdecls,_tmp3A2->f3=exs,({struct _tuple10*_tmp7AF=({struct _tuple10*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));_tmp3A1->f1=wc,_tmp3A1->f2=hides;_tmp3A1;});_tmp3A2->f4=_tmp7AF;});_tmp3A2;});_tmp3A3->r=_tmp7B1;}),_tmp3A3->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3A3;});_tmp3A4->hd=_tmp7B2;}),({struct Cyc_List_List*_tmp7AE=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp3A4->tl=_tmp7AE;});_tmp3A4;}));}else{
# 1178
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3A7=_cycalloc(sizeof(*_tmp3A7));({struct Cyc_Absyn_Decl*_tmp7B6=({struct Cyc_Absyn_Decl*_tmp3A6=_cycalloc(sizeof(*_tmp3A6));({void*_tmp7B5=(void*)({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));_tmp3A5->tag=11,({struct Cyc_List_List*_tmp7B4=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3A5->f1=_tmp7B4;});_tmp3A5;});_tmp3A6->r=_tmp7B5;}),_tmp3A6->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3A6;});_tmp3A7->hd=_tmp7B6;}),({struct Cyc_List_List*_tmp7B3=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp3A7->tl=_tmp7B3;});_tmp3A7;}));}}else{
# 1181
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3AB=_cycalloc(sizeof(*_tmp3AB));({struct Cyc_Absyn_Decl*_tmp7BB=({struct Cyc_Absyn_Decl*_tmp3AA=_cycalloc(sizeof(*_tmp3AA));({void*_tmp7BA=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3A9=_cycalloc(sizeof(*_tmp3A9));_tmp3A9->tag=12,({struct Cyc_List_List*_tmp7B9=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3A9->f1=_tmp7B9;}),_tmp3A9->f2=cycdecls,_tmp3A9->f3=exs,({struct _tuple10*_tmp7B8=({struct _tuple10*_tmp3A8=_cycalloc(sizeof(*_tmp3A8));_tmp3A8->f1=wc,_tmp3A8->f2=hides;_tmp3A8;});_tmp3A9->f4=_tmp7B8;});_tmp3A9;});_tmp3AA->r=_tmp7BA;}),_tmp3AA->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3AA;});_tmp3AB->hd=_tmp7BB;}),({struct Cyc_List_List*_tmp7B7=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp3AB->tl=_tmp7B7;});_tmp3AB;}));}
# 1183
goto _LL0;}}case 8:
# 1184 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3AD=_cycalloc(sizeof(*_tmp3AD));({struct Cyc_Absyn_Decl*_tmp7BD=({struct Cyc_Absyn_Decl*_tmp3AC=_cycalloc(sizeof(*_tmp3AC));_tmp3AC->r=(void*)& Cyc_Absyn_Porton_d_val,_tmp3AC->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3AC;});_tmp3AD->hd=_tmp7BD;}),({struct Cyc_List_List*_tmp7BC=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3AD->tl=_tmp7BC;});_tmp3AD;}));
goto _LL0;case 9:
# 1186 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3AF=_cycalloc(sizeof(*_tmp3AF));({struct Cyc_Absyn_Decl*_tmp7BF=({struct Cyc_Absyn_Decl*_tmp3AE=_cycalloc(sizeof(*_tmp3AE));_tmp3AE->r=(void*)& Cyc_Absyn_Portoff_d_val,_tmp3AE->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3AE;});_tmp3AF->hd=_tmp7BF;}),({struct Cyc_List_List*_tmp7BE=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3AF->tl=_tmp7BE;});_tmp3AF;}));
goto _LL0;case 10:
# 1188 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3B1=_cycalloc(sizeof(*_tmp3B1));({struct Cyc_Absyn_Decl*_tmp7C1=({struct Cyc_Absyn_Decl*_tmp3B0=_cycalloc(sizeof(*_tmp3B0));_tmp3B0->r=(void*)& Cyc_Absyn_Tempeston_d_val,_tmp3B0->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3B0;});_tmp3B1->hd=_tmp7C1;}),({struct Cyc_List_List*_tmp7C0=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3B1->tl=_tmp7C0;});_tmp3B1;}));
goto _LL0;case 11:
# 1190 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3B3=_cycalloc(sizeof(*_tmp3B3));({struct Cyc_Absyn_Decl*_tmp7C3=({struct Cyc_Absyn_Decl*_tmp3B2=_cycalloc(sizeof(*_tmp3B2));_tmp3B2->r=(void*)& Cyc_Absyn_Tempestoff_d_val,_tmp3B2->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3B2;});_tmp3B3->hd=_tmp7C3;}),({struct Cyc_List_List*_tmp7C2=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3B3->tl=_tmp7C2;});_tmp3B3;}));
goto _LL0;case 12:
# 1191 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 13:
# 1195 "parse.y"
 Cyc_Parse_parsing_tempest=1;
goto _LL0;case 14:
# 1198
 Cyc_Parse_parsing_tempest=0;
goto _LL0;case 15:
# 1203 "parse.y"
 Cyc_Lex_enter_extern_c();
if(({struct _fat_ptr _tmp7C4=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmp7C4,({const char*_tmp3B4="C";_tag_fat(_tmp3B4,sizeof(char),2U);}));})== 0)
yyval=Cyc_YY32(0);else{
if(({struct _fat_ptr _tmp7C5=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmp7C5,({const char*_tmp3B5="C include";_tag_fat(_tmp3B5,sizeof(char),10U);}));})== 0)
yyval=Cyc_YY32(1);else{
# 1209
({void*_tmp3B6=0U;({unsigned _tmp7C7=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp7C6=({const char*_tmp3B7="expecting \"C\" or \"C include\"";_tag_fat(_tmp3B7,sizeof(char),29U);});Cyc_Warn_err(_tmp7C7,_tmp7C6,_tag_fat(_tmp3B6,sizeof(void*),0));});});
yyval=Cyc_YY32(1);}}
# 1213
goto _LL0;case 16:
# 1216 "parse.y"
 Cyc_Lex_leave_extern_c();
goto _LL0;case 17:
# 1220 "parse.y"
 yyval=Cyc_YY55(0);
goto _LL0;case 18:
# 1221 "parse.y"
 yyval=(yyyvsp[2]).v;
goto _LL0;case 19:
# 1225 "parse.y"
 yyval=Cyc_YY55(({struct Cyc_List_List*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));({struct _tuple0*_tmp7C8=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3B8->hd=_tmp7C8;}),_tmp3B8->tl=0;_tmp3B8;}));
goto _LL0;case 20:
# 1226 "parse.y"
 yyval=Cyc_YY55(({struct Cyc_List_List*_tmp3B9=_cycalloc(sizeof(*_tmp3B9));({struct _tuple0*_tmp7C9=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3B9->hd=_tmp7C9;}),_tmp3B9->tl=0;_tmp3B9;}));
goto _LL0;case 21:
# 1227 "parse.y"
 yyval=Cyc_YY55(({struct Cyc_List_List*_tmp3BA=_cycalloc(sizeof(*_tmp3BA));({struct _tuple0*_tmp7CB=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3BA->hd=_tmp7CB;}),({struct Cyc_List_List*_tmp7CA=Cyc_yyget_YY55(&(yyyvsp[2]).v);_tmp3BA->tl=_tmp7CA;});_tmp3BA;}));
goto _LL0;case 22:
# 1231 "parse.y"
 yyval=Cyc_YY54(({struct _tuple29*_tmp3BB=_cycalloc(sizeof(*_tmp3BB));_tmp3BB->f1=0,_tmp3BB->f2=0U;_tmp3BB;}));
goto _LL0;case 23:
# 1232 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 24:
# 1236 "parse.y"
 yyval=Cyc_YY54(({struct _tuple29*_tmp3BC=_cycalloc(sizeof(*_tmp3BC));({struct Cyc_List_List*_tmp7CC=Cyc_yyget_YY53(&(yyyvsp[2]).v);_tmp3BC->f1=_tmp7CC;}),_tmp3BC->f2=0U;_tmp3BC;}));
goto _LL0;case 25:
# 1237 "parse.y"
 yyval=Cyc_YY54(({struct _tuple29*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD->f1=0,_tmp3BD->f2=0U;_tmp3BD;}));
goto _LL0;case 26:
# 1238 "parse.y"
 yyval=Cyc_YY54(({struct _tuple29*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE->f1=0,_tmp3BE->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp3BE;}));
goto _LL0;case 27:
# 1243 "parse.y"
 yyval=Cyc_YY53(({struct Cyc_List_List*_tmp3C0=_cycalloc(sizeof(*_tmp3C0));({struct _tuple33*_tmp7CE=({struct _tuple33*_tmp3BF=_cycalloc(sizeof(*_tmp3BF));_tmp3BF->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_tmp7CD=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3BF->f2=_tmp7CD;}),_tmp3BF->f3=0;_tmp3BF;});_tmp3C0->hd=_tmp7CE;}),_tmp3C0->tl=0;_tmp3C0;}));
goto _LL0;case 28:
# 1245 "parse.y"
 yyval=Cyc_YY53(({struct Cyc_List_List*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));({struct _tuple33*_tmp7D1=({struct _tuple33*_tmp3C1=_cycalloc(sizeof(*_tmp3C1));_tmp3C1->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_tmp7D0=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3C1->f2=_tmp7D0;}),_tmp3C1->f3=0;_tmp3C1;});_tmp3C2->hd=_tmp7D1;}),({struct Cyc_List_List*_tmp7CF=Cyc_yyget_YY53(&(yyyvsp[2]).v);_tmp3C2->tl=_tmp7CF;});_tmp3C2;}));
goto _LL0;case 29:
# 1249 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 30:
# 1250 "parse.y"
 yyval=(yyyvsp[2]).v;
goto _LL0;case 31:
# 1254 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C4=_cycalloc(sizeof(*_tmp3C4));({struct Cyc_Absyn_Decl*_tmp7D4=({void*_tmp7D3=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp3C3=_cycalloc(sizeof(*_tmp3C3));_tmp3C3->tag=1,({struct Cyc_Absyn_Fndecl*_tmp7D2=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp3C3->f1=_tmp7D2;});_tmp3C3;});Cyc_Absyn_new_decl(_tmp7D3,(unsigned)((yyyvsp[0]).l).first_line);});_tmp3C4->hd=_tmp7D4;}),_tmp3C4->tl=0;_tmp3C4;}));
goto _LL0;case 32:
# 1255 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 33:
# 1256 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 36:
# 1266 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_tmp7D7=yyr;struct Cyc_Parse_Declarator _tmp7D6=Cyc_yyget_YY28(&(yyyvsp[0]).v);struct Cyc_Absyn_Stmt*_tmp7D5=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);Cyc_Parse_make_function(_tmp7D7,0,_tmp7D6,0,_tmp7D5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 37:  {
# 1268 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3C5=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3C5;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp7DA=yyr;struct Cyc_Parse_Declarator _tmp7D9=Cyc_yyget_YY28(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp7D8=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp7DA,& d,_tmp7D9,0,_tmp7D8,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 38:
# 1280 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_tmp7DE=yyr;struct Cyc_Parse_Declarator _tmp7DD=Cyc_yyget_YY28(&(yyyvsp[0]).v);struct Cyc_List_List*_tmp7DC=Cyc_yyget_YY16(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp7DB=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp7DE,0,_tmp7DD,_tmp7DC,_tmp7DB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 39:  {
# 1282 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3C6=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3C6;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp7E2=yyr;struct Cyc_Parse_Declarator _tmp7E1=Cyc_yyget_YY28(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp7E0=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp7DF=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);Cyc_Parse_make_function(_tmp7E2,& d,_tmp7E1,_tmp7E0,_tmp7DF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 40:  {
# 1290 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3C7=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3C7;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp7E5=yyr;struct Cyc_Parse_Declarator _tmp7E4=Cyc_yyget_YY28(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp7E3=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp7E5,& d,_tmp7E4,0,_tmp7E3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 41:  {
# 1293 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3C8=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3C8;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp7E9=yyr;struct Cyc_Parse_Declarator _tmp7E8=Cyc_yyget_YY28(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp7E7=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp7E6=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);Cyc_Parse_make_function(_tmp7E9,& d,_tmp7E8,_tmp7E7,_tmp7E6,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 42:
# 1298 "parse.y"
 Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(&(yyyvsp[1]).v));yyval=(yyyvsp[1]).v;
goto _LL0;case 43:
# 1301
 Cyc_Lex_leave_using();
goto _LL0;case 44:
# 1304
 Cyc_Lex_enter_namespace(({struct _fat_ptr*_tmp3C9=_cycalloc(sizeof(*_tmp3C9));({struct _fat_ptr _tmp7EA=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp3C9=_tmp7EA;});_tmp3C9;}));yyval=(yyyvsp[1]).v;
goto _LL0;case 45:
# 1307
 Cyc_Lex_leave_namespace();
goto _LL0;case 46:
# 1313 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp7EC=Cyc_yyget_YY17(&(yyyvsp[0]).v);unsigned _tmp7EB=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Parse_make_declarations(_tmp7EC,0,_tmp7EB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 47:
# 1315 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp7F0=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct _tuple11*_tmp7EF=({struct _tuple11*(*_tmp7ED)(struct _tuple11*)=({(struct _tuple11*(*)(struct _tuple11*))Cyc_Parse_flat_imp_rev;});_tmp7ED(Cyc_yyget_YY19(&(yyyvsp[1]).v));});unsigned _tmp7EE=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Parse_make_declarations(_tmp7F0,_tmp7EF,_tmp7EE,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 48:
# 1318
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3CA=_cycalloc(sizeof(*_tmp3CA));({struct Cyc_Absyn_Decl*_tmp7F3=({struct Cyc_Absyn_Pat*_tmp7F2=Cyc_yyget_YY9(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmp7F1=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_let_decl(_tmp7F2,_tmp7F1,(unsigned)((yyyvsp[0]).l).first_line);});_tmp3CA->hd=_tmp7F3;}),_tmp3CA->tl=0;_tmp3CA;}));
goto _LL0;case 49:  {
# 1320 "parse.y"
struct Cyc_List_List*_tmp3CB=0;struct Cyc_List_List*vds=_tmp3CB;
{struct Cyc_List_List*_tmp3CC=Cyc_yyget_YY37(&(yyyvsp[1]).v);struct Cyc_List_List*ids=_tmp3CC;for(0;ids != 0;ids=ids->tl){
struct _tuple0*qv=({struct _tuple0*_tmp3CE=_cycalloc(sizeof(*_tmp3CE));({union Cyc_Absyn_Nmspace _tmp7F4=Cyc_Absyn_Rel_n(0);_tmp3CE->f1=_tmp7F4;}),_tmp3CE->f2=(struct _fat_ptr*)ids->hd;_tmp3CE;});
vds=({struct Cyc_List_List*_tmp3CD=_cycalloc(sizeof(*_tmp3CD));({struct Cyc_Absyn_Vardecl*_tmp7F6=({struct _tuple0*_tmp7F5=qv;Cyc_Absyn_new_vardecl(0U,_tmp7F5,Cyc_Absyn_wildtyp(0),0);});_tmp3CD->hd=_tmp7F6;}),_tmp3CD->tl=vds;_tmp3CD;});}}
# 1325
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3CF=_cycalloc(sizeof(*_tmp3CF));({struct Cyc_Absyn_Decl*_tmp7F8=({struct Cyc_List_List*_tmp7F7=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(vds);Cyc_Absyn_letv_decl(_tmp7F7,(unsigned)((yyyvsp[0]).l).first_line);});_tmp3CF->hd=_tmp7F8;}),_tmp3CF->tl=0;_tmp3CF;}));
# 1327
goto _LL0;}case 50:
# 1330 "parse.y"
({struct _fat_ptr _tmp7F9=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_tvar_ok(_tmp7F9,(unsigned)((yyyvsp[2]).l).first_line);});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));({struct _fat_ptr*_tmp7FC=({struct _fat_ptr*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));({struct _fat_ptr _tmp7FB=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp3D3=_tmp7FB;});_tmp3D3;});_tmp3D4->name=_tmp7FC;}),_tmp3D4->identity=-1,({void*_tmp7FA=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_tmp3D4->kind=_tmp7FA;});_tmp3D4;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp800=(unsigned)((yyyvsp[4]).l).first_line;struct _tuple0*_tmp7FF=({struct _tuple0*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));_tmp3D2->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp7FE=({struct _fat_ptr*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));({struct _fat_ptr _tmp7FD=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp3D1=_tmp7FD;});_tmp3D1;});_tmp3D2->f2=_tmp7FE;});_tmp3D2;});Cyc_Absyn_new_vardecl(_tmp800,_tmp7FF,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3D0=_cycalloc(sizeof(*_tmp3D0));({struct Cyc_Absyn_Decl*_tmp801=Cyc_Absyn_region_decl(tv,vd,0,(unsigned)((yyyvsp[0]).l).first_line);_tmp3D0->hd=_tmp801;}),_tmp3D0->tl=0;_tmp3D0;}));
# 1336
goto _LL0;}case 51:  {
# 1338
struct _fat_ptr tvstring=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3DC=({struct Cyc_String_pa_PrintArg_struct _tmp696;_tmp696.tag=0,({struct _fat_ptr _tmp802=(struct _fat_ptr)((struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v));_tmp696.f1=_tmp802;});_tmp696;});void*_tmp3DA[1];_tmp3DA[0]=& _tmp3DC;({struct _fat_ptr _tmp803=({const char*_tmp3DB="`%s";_tag_fat(_tmp3DB,sizeof(char),4U);});Cyc_aprintf(_tmp803,_tag_fat(_tmp3DA,sizeof(void*),1));});});
({struct _fat_ptr _tmp804=Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_Parse_tvar_ok(_tmp804,(unsigned)((yyyvsp[1]).l).first_line);});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3D9=_cycalloc(sizeof(*_tmp3D9));({struct _fat_ptr*_tmp806=({struct _fat_ptr*_tmp3D8=_cycalloc(sizeof(*_tmp3D8));*_tmp3D8=tvstring;_tmp3D8;});_tmp3D9->name=_tmp806;}),_tmp3D9->identity=-1,({void*_tmp805=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_tmp3D9->kind=_tmp805;});_tmp3D9;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp80A=(unsigned)((yyyvsp[1]).l).first_line;struct _tuple0*_tmp809=({struct _tuple0*_tmp3D7=_cycalloc(sizeof(*_tmp3D7));_tmp3D7->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp808=({struct _fat_ptr*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));({struct _fat_ptr _tmp807=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp3D6=_tmp807;});_tmp3D6;});_tmp3D7->f2=_tmp808;});_tmp3D7;});Cyc_Absyn_new_vardecl(_tmp80A,_tmp809,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3D5=_cycalloc(sizeof(*_tmp3D5));({struct Cyc_Absyn_Decl*_tmp80E=({struct Cyc_Absyn_Tvar*_tmp80D=tv;struct Cyc_Absyn_Vardecl*_tmp80C=vd;struct Cyc_Absyn_Exp*_tmp80B=Cyc_yyget_YY58(&(yyyvsp[2]).v);Cyc_Absyn_region_decl(_tmp80D,_tmp80C,_tmp80B,(unsigned)((yyyvsp[0]).l).first_line);});_tmp3D5->hd=_tmp80E;}),_tmp3D5->tl=0;_tmp3D5;}));
# 1345
goto _LL0;}}case 52:
# 1348 "parse.y"
 yyval=Cyc_YY58(0);
goto _LL0;case 53:
# 1350 "parse.y"
 if(({struct _fat_ptr _tmp80F=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmp80F,({const char*_tmp3DD="open";_tag_fat(_tmp3DD,sizeof(char),5U);}));})!= 0)({void*_tmp3DE=0U;({unsigned _tmp811=(unsigned)((yyyvsp[3]).l).first_line;struct _fat_ptr _tmp810=({const char*_tmp3DF="expecting `open'";_tag_fat(_tmp3DF,sizeof(char),17U);});Cyc_Warn_err(_tmp811,_tmp810,_tag_fat(_tmp3DE,sizeof(void*),0));});});
yyval=Cyc_YY58(Cyc_yyget_Exp_tok(&(yyyvsp[3]).v));
goto _LL0;case 54:
# 1354
 yyval=(yyyvsp[0]).v;
goto _LL0;case 55:
# 1355 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*(*_tmp813)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append;});struct Cyc_List_List*_tmp812=Cyc_yyget_YY16(&(yyyvsp[0]).v);_tmp813(_tmp812,Cyc_yyget_YY16(&(yyyvsp[1]).v));}));
goto _LL0;case 56:
# 1361 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp697;({enum Cyc_Parse_Storage_class _tmp816=Cyc_yyget_YY20(&(yyyvsp[0]).v);_tmp697.sc=_tmp816;}),({struct Cyc_Absyn_Tqual _tmp815=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp697.tq=_tmp815;}),({
struct Cyc_Parse_Type_specifier _tmp814=Cyc_Parse_empty_spec(0U);_tmp697.type_specs=_tmp814;}),_tmp697.is_inline=0,_tmp697.attributes=0;_tmp697;}));
goto _LL0;case 57:  {
# 1364 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3E0=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp3E0;
if((int)two.sc != 7)
({void*_tmp3E1=0U;({unsigned _tmp818=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp817=({const char*_tmp3E2="Only one storage class is allowed in a declaration (missing ';' or ','?)";_tag_fat(_tmp3E2,sizeof(char),73U);});Cyc_Warn_warn(_tmp818,_tmp817,_tag_fat(_tmp3E1,sizeof(void*),0));});});
# 1368
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp698;({enum Cyc_Parse_Storage_class _tmp819=Cyc_yyget_YY20(&(yyyvsp[0]).v);_tmp698.sc=_tmp819;}),_tmp698.tq=two.tq,_tmp698.type_specs=two.type_specs,_tmp698.is_inline=two.is_inline,_tmp698.attributes=two.attributes;_tmp698;}));
# 1371
goto _LL0;}case 58:
# 1372 "parse.y"
({void*_tmp3E3=0U;({unsigned _tmp81B=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp81A=({const char*_tmp3E4="__extension__ keyword ignored in declaration";_tag_fat(_tmp3E4,sizeof(char),45U);});Cyc_Warn_warn(_tmp81B,_tmp81A,_tag_fat(_tmp3E3,sizeof(void*),0));});});
yyval=(yyyvsp[1]).v;
# 1375
goto _LL0;case 59:
# 1376 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp699;_tmp699.sc=7U,({struct Cyc_Absyn_Tqual _tmp81D=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp699.tq=_tmp81D;}),({
struct Cyc_Parse_Type_specifier _tmp81C=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp699.type_specs=_tmp81C;}),_tmp699.is_inline=0,_tmp699.attributes=0;_tmp699;}));
goto _LL0;case 60:  {
# 1379 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3E5=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp3E5;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp69A;_tmp69A.sc=two.sc,_tmp69A.tq=two.tq,({
struct Cyc_Parse_Type_specifier _tmp820=({unsigned _tmp81F=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp81E=two.type_specs;Cyc_Parse_combine_specifiers(_tmp81F,_tmp81E,Cyc_yyget_YY21(&(yyyvsp[0]).v));});_tmp69A.type_specs=_tmp820;}),_tmp69A.is_inline=two.is_inline,_tmp69A.attributes=two.attributes;_tmp69A;}));
# 1384
goto _LL0;}case 61:
# 1385 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp69B;_tmp69B.sc=7U,({struct Cyc_Absyn_Tqual _tmp822=Cyc_yyget_YY24(&(yyyvsp[0]).v);_tmp69B.tq=_tmp822;}),({struct Cyc_Parse_Type_specifier _tmp821=Cyc_Parse_empty_spec(0U);_tmp69B.type_specs=_tmp821;}),_tmp69B.is_inline=0,_tmp69B.attributes=0;_tmp69B;}));
goto _LL0;case 62:  {
# 1387 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3E6=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp3E6;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp69C;_tmp69C.sc=two.sc,({struct Cyc_Absyn_Tqual _tmp824=({struct Cyc_Absyn_Tqual _tmp823=Cyc_yyget_YY24(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp823,two.tq);});_tmp69C.tq=_tmp824;}),_tmp69C.type_specs=two.type_specs,_tmp69C.is_inline=two.is_inline,_tmp69C.attributes=two.attributes;_tmp69C;}));
# 1391
goto _LL0;}case 63:
# 1392 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp69D;_tmp69D.sc=7U,({struct Cyc_Absyn_Tqual _tmp826=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp69D.tq=_tmp826;}),({
struct Cyc_Parse_Type_specifier _tmp825=Cyc_Parse_empty_spec(0U);_tmp69D.type_specs=_tmp825;}),_tmp69D.is_inline=1,_tmp69D.attributes=0;_tmp69D;}));
goto _LL0;case 64:  {
# 1395 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3E7=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp3E7;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp69E;_tmp69E.sc=two.sc,_tmp69E.tq=two.tq,_tmp69E.type_specs=two.type_specs,_tmp69E.is_inline=1,_tmp69E.attributes=two.attributes;_tmp69E;}));
# 1398
goto _LL0;}case 65:
# 1399 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp69F;_tmp69F.sc=7U,({struct Cyc_Absyn_Tqual _tmp829=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp69F.tq=_tmp829;}),({
struct Cyc_Parse_Type_specifier _tmp828=Cyc_Parse_empty_spec(0U);_tmp69F.type_specs=_tmp828;}),_tmp69F.is_inline=0,({struct Cyc_List_List*_tmp827=Cyc_yyget_YY46(&(yyyvsp[0]).v);_tmp69F.attributes=_tmp827;});_tmp69F;}));
goto _LL0;case 66:  {
# 1402 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3E8=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp3E8;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6A0;_tmp6A0.sc=two.sc,_tmp6A0.tq=two.tq,_tmp6A0.type_specs=two.type_specs,_tmp6A0.is_inline=two.is_inline,({
# 1405
struct Cyc_List_List*_tmp82C=({struct Cyc_List_List*(*_tmp82B)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append;});struct Cyc_List_List*_tmp82A=Cyc_yyget_YY46(&(yyyvsp[0]).v);_tmp82B(_tmp82A,two.attributes);});_tmp6A0.attributes=_tmp82C;});_tmp6A0;}));
goto _LL0;}case 67:
# 1409 "parse.y"
 yyval=Cyc_YY20(4U);
goto _LL0;case 68:
# 1410 "parse.y"
 yyval=Cyc_YY20(5U);
goto _LL0;case 69:
# 1411 "parse.y"
 yyval=Cyc_YY20(3U);
goto _LL0;case 70:
# 1412 "parse.y"
 yyval=Cyc_YY20(1U);
goto _LL0;case 71:
# 1414 "parse.y"
 if(({struct _fat_ptr _tmp82D=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmp82D,({const char*_tmp3E9="C";_tag_fat(_tmp3E9,sizeof(char),2U);}));})!= 0)
({void*_tmp3EA=0U;({unsigned _tmp82F=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp82E=({const char*_tmp3EB="only extern or extern \"C\" is allowed";_tag_fat(_tmp3EB,sizeof(char),37U);});Cyc_Warn_err(_tmp82F,_tmp82E,_tag_fat(_tmp3EA,sizeof(void*),0));});});
yyval=Cyc_YY20(2U);
# 1418
goto _LL0;case 72:
# 1418 "parse.y"
 yyval=Cyc_YY20(0U);
goto _LL0;case 73:
# 1420 "parse.y"
 yyval=Cyc_YY20(6U);
goto _LL0;case 74:
# 1425 "parse.y"
 yyval=Cyc_YY46(0);
goto _LL0;case 75:
# 1426 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 76:
# 1430 "parse.y"
 yyval=(yyyvsp[3]).v;
goto _LL0;case 77:
# 1433
 yyval=Cyc_YY46(({struct Cyc_List_List*_tmp3EC=_cycalloc(sizeof(*_tmp3EC));({void*_tmp830=Cyc_yyget_YY47(&(yyyvsp[0]).v);_tmp3EC->hd=_tmp830;}),_tmp3EC->tl=0;_tmp3EC;}));
goto _LL0;case 78:
# 1434 "parse.y"
 yyval=Cyc_YY46(({struct Cyc_List_List*_tmp3ED=_cycalloc(sizeof(*_tmp3ED));({void*_tmp832=Cyc_yyget_YY47(&(yyyvsp[0]).v);_tmp3ED->hd=_tmp832;}),({struct Cyc_List_List*_tmp831=Cyc_yyget_YY46(&(yyyvsp[2]).v);_tmp3ED->tl=_tmp831;});_tmp3ED;}));
goto _LL0;case 79:
# 1437
 yyval=Cyc_YY47(({unsigned _tmp833=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Atts_parse_nullary_att(_tmp833,Cyc_yyget_String_tok(&(yyyvsp[0]).v));}));
goto _LL0;case 80:
# 1438 "parse.y"
 yyval=Cyc_YY47((void*)& Cyc_Atts_Const_att_val);
goto _LL0;case 81:
# 1440 "parse.y"
 yyval=Cyc_YY47(({unsigned _tmp836=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp835=Cyc_yyget_String_tok(&(yyyvsp[0]).v);unsigned _tmp834=(unsigned)((yyyvsp[2]).l).first_line;Cyc_Atts_parse_unary_att(_tmp836,_tmp835,_tmp834,Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));}));
goto _LL0;case 82:
# 1442 "parse.y"
 yyval=Cyc_YY47(({unsigned _tmp83D=(unsigned)((yyyvsp[0]).l).first_line;unsigned _tmp83C=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmp83B=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr _tmp83A=Cyc_yyget_String_tok(&(yyyvsp[2]).v);unsigned _tmp839=({
unsigned _tmp837=(unsigned)((yyyvsp[4]).l).first_line;Cyc_Parse_cnst2uint(_tmp837,Cyc_yyget_Int_tok(&(yyyvsp[4]).v));});
# 1442
Cyc_Atts_parse_format_att(_tmp83D,_tmp83C,_tmp83B,_tmp83A,_tmp839,({
# 1444
unsigned _tmp838=(unsigned)((yyyvsp[6]).l).first_line;Cyc_Parse_cnst2uint(_tmp838,Cyc_yyget_Int_tok(&(yyyvsp[6]).v));}));}));
goto _LL0;case 83:
# 1454 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 84:
# 1456 "parse.y"
 yyval=Cyc_YY21(({void*_tmp83F=({struct _tuple0*_tmp83E=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);Cyc_Absyn_typedef_type(_tmp83E,Cyc_yyget_YY41(&(yyyvsp[1]).v),0,0);});Cyc_Parse_type_spec(_tmp83F,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 85:
# 1460 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_void_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 86:
# 1461 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 87:
# 1462 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_short_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 88:
# 1463 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 89:
# 1464 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_long_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 90:
# 1465 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 91:
# 1466 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_double_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 92:
# 1467 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_signed_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 93:
# 1468 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_unsigned_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 94:
# 1469 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 95:
# 1470 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 96:
# 1472 "parse.y"
 yyval=Cyc_YY21(({void*_tmp840=Cyc_Absyn_typeof_type(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp840,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 97:
# 1474 "parse.y"
 yyval=Cyc_YY21(({void*_tmp841=Cyc_Absyn_builtin_type(({const char*_tmp3EE="__builtin_va_list";_tag_fat(_tmp3EE,sizeof(char),18U);}),& Cyc_Kinds_bk);Cyc_Parse_type_spec(_tmp841,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 98:
# 1476 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 99:
# 1478 "parse.y"
 yyval=Cyc_YY21(({void*_tmp842=Cyc_yyget_YY45(&(yyyvsp[0]).v);Cyc_Parse_type_spec(_tmp842,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 100:
# 1480 "parse.y"
 yyval=Cyc_YY21(({void*_tmp843=Cyc_Absyn_new_evar(0,0);Cyc_Parse_type_spec(_tmp843,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 101:
# 1482 "parse.y"
 yyval=Cyc_YY21(({void*_tmp844=Cyc_Absyn_new_evar(Cyc_Kinds_kind_to_opt(Cyc_yyget_YY44(&(yyyvsp[2]).v)),0);Cyc_Parse_type_spec(_tmp844,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 102:
# 1484 "parse.y"
 yyval=Cyc_YY21(({void*_tmp84A=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3EF=_cycalloc(sizeof(*_tmp3EF));_tmp3EF->tag=6,({struct Cyc_List_List*_tmp849=({struct Cyc_List_List*(*_tmp848)(struct _tuple20*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct _tuple20*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;});struct _tuple20*(*_tmp847)(unsigned,struct _tuple8*)=({(struct _tuple20*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ;});unsigned _tmp846=(unsigned)((yyyvsp[2]).l).first_line;_tmp848(_tmp847,_tmp846,({struct Cyc_List_List*(*_tmp845)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_tmp845(Cyc_yyget_YY39(&(yyyvsp[2]).v));}));});_tmp3EF->f1=_tmp849;});_tmp3EF;});Cyc_Parse_type_spec(_tmp84A,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1486
goto _LL0;case 103:
# 1487 "parse.y"
 yyval=Cyc_YY21(({void*_tmp84B=Cyc_Absyn_rgn_handle_type(Cyc_yyget_YY45(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp84B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 104:
# 1489 "parse.y"
 yyval=Cyc_YY21(({void*_tmp84C=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Kinds_rko,0));Cyc_Parse_type_spec(_tmp84C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 105:
# 1491 "parse.y"
 yyval=Cyc_YY21(({void*_tmp84D=Cyc_Absyn_tag_type(Cyc_yyget_YY45(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp84D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 106:
# 1493 "parse.y"
 yyval=Cyc_YY21(({void*_tmp84E=Cyc_Absyn_tag_type(Cyc_Absyn_new_evar(& Cyc_Kinds_iko,0));Cyc_Parse_type_spec(_tmp84E,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 107:
# 1495 "parse.y"
 yyval=Cyc_YY21(({void*_tmp84F=Cyc_Absyn_valueof_type(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp84F,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 108:
# 1500 "parse.y"
 yyval=Cyc_YY44(({struct _fat_ptr _tmp850=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Kinds_id_to_kind(_tmp850,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 109:  {
# 1504 "parse.y"
unsigned loc=(unsigned)(Cyc_Flags_porting_c_code?((yyyvsp[0]).l).first_line: 0);
yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _tmp6A1;_tmp6A1.print_const=1,_tmp6A1.q_volatile=0,_tmp6A1.q_restrict=0,_tmp6A1.real_const=1,_tmp6A1.loc=loc;_tmp6A1;}));
goto _LL0;}case 110:
# 1506 "parse.y"
 yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _tmp6A2;_tmp6A2.print_const=0,_tmp6A2.q_volatile=1,_tmp6A2.q_restrict=0,_tmp6A2.real_const=0,_tmp6A2.loc=0U;_tmp6A2;}));
goto _LL0;case 111:
# 1507 "parse.y"
 yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _tmp6A3;_tmp6A3.print_const=0,_tmp6A3.q_volatile=0,_tmp6A3.q_restrict=1,_tmp6A3.real_const=0,_tmp6A3.loc=0U;_tmp6A3;}));
goto _LL0;case 112:  {
# 1513 "parse.y"
struct Cyc_Absyn_TypeDecl*_tmp3F0=({struct Cyc_Absyn_TypeDecl*_tmp3F5=_cycalloc(sizeof(*_tmp3F5));({void*_tmp855=(void*)({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp3F4=_cycalloc(sizeof(*_tmp3F4));_tmp3F4->tag=1,({struct Cyc_Absyn_Enumdecl*_tmp854=({struct Cyc_Absyn_Enumdecl*_tmp3F3=_cycalloc(sizeof(*_tmp3F3));_tmp3F3->sc=2U,({struct _tuple0*_tmp853=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp3F3->name=_tmp853;}),({struct Cyc_Core_Opt*_tmp852=({struct Cyc_Core_Opt*_tmp3F2=_cycalloc(sizeof(*_tmp3F2));({struct Cyc_List_List*_tmp851=Cyc_yyget_YY49(&(yyyvsp[3]).v);_tmp3F2->v=_tmp851;});_tmp3F2;});_tmp3F3->fields=_tmp852;});_tmp3F3;});_tmp3F4->f1=_tmp854;});_tmp3F4;});_tmp3F5->r=_tmp855;}),_tmp3F5->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3F5;});struct Cyc_Absyn_TypeDecl*ed=_tmp3F0;
# 1515
yyval=Cyc_YY21(({void*_tmp856=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp3F1=_cycalloc(sizeof(*_tmp3F1));_tmp3F1->tag=10,_tmp3F1->f1=ed,_tmp3F1->f2=0;_tmp3F1;});Cyc_Parse_type_spec(_tmp856,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1517
goto _LL0;}case 113:
# 1518 "parse.y"
 yyval=Cyc_YY21(({void*_tmp857=Cyc_Absyn_enum_type(Cyc_yyget_QualId_tok(&(yyyvsp[1]).v),0);Cyc_Parse_type_spec(_tmp857,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 114:
# 1520 "parse.y"
 yyval=Cyc_YY21(({void*_tmp858=Cyc_Absyn_anon_enum_type(Cyc_yyget_YY49(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp858,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 115:
# 1526 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_Absyn_Enumfield*_tmp3F6=_cycalloc(sizeof(*_tmp3F6));({struct _tuple0*_tmp859=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3F6->name=_tmp859;}),_tmp3F6->tag=0,_tmp3F6->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3F6;}));
goto _LL0;case 116:
# 1528 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_Absyn_Enumfield*_tmp3F7=_cycalloc(sizeof(*_tmp3F7));({struct _tuple0*_tmp85B=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3F7->name=_tmp85B;}),({struct Cyc_Absyn_Exp*_tmp85A=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp3F7->tag=_tmp85A;}),_tmp3F7->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3F7;}));
goto _LL0;case 117:
# 1532 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_List_List*_tmp3F8=_cycalloc(sizeof(*_tmp3F8));({struct Cyc_Absyn_Enumfield*_tmp85C=Cyc_yyget_YY48(&(yyyvsp[0]).v);_tmp3F8->hd=_tmp85C;}),_tmp3F8->tl=0;_tmp3F8;}));
goto _LL0;case 118:
# 1533 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_List_List*_tmp3F9=_cycalloc(sizeof(*_tmp3F9));({struct Cyc_Absyn_Enumfield*_tmp85D=Cyc_yyget_YY48(&(yyyvsp[0]).v);_tmp3F9->hd=_tmp85D;}),_tmp3F9->tl=0;_tmp3F9;}));
goto _LL0;case 119:
# 1534 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_List_List*_tmp3FA=_cycalloc(sizeof(*_tmp3FA));({struct Cyc_Absyn_Enumfield*_tmp85F=Cyc_yyget_YY48(&(yyyvsp[0]).v);_tmp3FA->hd=_tmp85F;}),({struct Cyc_List_List*_tmp85E=Cyc_yyget_YY49(&(yyyvsp[2]).v);_tmp3FA->tl=_tmp85E;});_tmp3FA;}));
goto _LL0;case 120:
# 1540 "parse.y"
 yyval=Cyc_YY21(({void*_tmp862=(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp3FB=_cycalloc(sizeof(*_tmp3FB));_tmp3FB->tag=7,({enum Cyc_Absyn_AggrKind _tmp861=Cyc_yyget_YY22(&(yyyvsp[0]).v);_tmp3FB->f1=_tmp861;}),({struct Cyc_List_List*_tmp860=Cyc_yyget_YY25(&(yyyvsp[2]).v);_tmp3FB->f2=_tmp860;});_tmp3FB;});Cyc_Parse_type_spec(_tmp862,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 121:  {
# 1544 "parse.y"
struct Cyc_List_List*_tmp3FC=({struct Cyc_List_List*(*_tmp864)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;});unsigned _tmp863=(unsigned)((yyyvsp[2]).l).first_line;_tmp864(Cyc_Parse_typ2tvar,_tmp863,Cyc_yyget_YY41(&(yyyvsp[2]).v));});struct Cyc_List_List*ts=_tmp3FC;
struct Cyc_List_List*_tmp3FD=({struct Cyc_List_List*(*_tmp866)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;});unsigned _tmp865=(unsigned)((yyyvsp[4]).l).first_line;_tmp866(Cyc_Parse_typ2tvar,_tmp865,Cyc_yyget_YY41(&(yyyvsp[4]).v));});struct Cyc_List_List*exist_ts=_tmp3FD;
struct Cyc_Absyn_TypeDecl*_tmp3FE=({enum Cyc_Absyn_AggrKind _tmp86D=(Cyc_yyget_YY23(&(yyyvsp[0]).v)).f2;struct _tuple0*_tmp86C=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp86B=ts;struct Cyc_Absyn_AggrdeclImpl*_tmp86A=({
struct Cyc_List_List*_tmp869=exist_ts;struct Cyc_List_List*_tmp868=Cyc_yyget_YY51(&(yyyvsp[5]).v);struct Cyc_List_List*_tmp867=Cyc_yyget_YY25(&(yyyvsp[6]).v);Cyc_Absyn_aggrdecl_impl(_tmp869,_tmp868,_tmp867,(Cyc_yyget_YY23(&(yyyvsp[0]).v)).f1);});
# 1546
Cyc_Absyn_aggr_tdecl(_tmp86D,2U,_tmp86C,_tmp86B,_tmp86A,0,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_TypeDecl*td=_tmp3FE;
# 1549
yyval=Cyc_YY21(({void*_tmp86E=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp3FF=_cycalloc(sizeof(*_tmp3FF));_tmp3FF->tag=10,_tmp3FF->f1=td,_tmp3FF->f2=0;_tmp3FF;});Cyc_Parse_type_spec(_tmp86E,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1551
goto _LL0;}case 122:
# 1552 "parse.y"
 yyval=Cyc_YY21(({void*_tmp872=({union Cyc_Absyn_AggrInfo _tmp871=({enum Cyc_Absyn_AggrKind _tmp870=(Cyc_yyget_YY23(&(yyyvsp[0]).v)).f2;struct _tuple0*_tmp86F=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);Cyc_Absyn_UnknownAggr(_tmp870,_tmp86F,(Cyc_yyget_YY23(&(yyyvsp[0]).v)).f1?({struct Cyc_Core_Opt*_tmp400=_cycalloc(sizeof(*_tmp400));_tmp400->v=(void*)1;_tmp400;}): 0);});Cyc_Absyn_aggr_type(_tmp871,
Cyc_yyget_YY41(&(yyyvsp[2]).v));});
# 1552
Cyc_Parse_type_spec(_tmp872,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1554
goto _LL0;case 123:
# 1557 "parse.y"
 yyval=Cyc_YY23(({struct _tuple25 _tmp6A4;_tmp6A4.f1=1,({enum Cyc_Absyn_AggrKind _tmp873=Cyc_yyget_YY22(&(yyyvsp[1]).v);_tmp6A4.f2=_tmp873;});_tmp6A4;}));
goto _LL0;case 124:
# 1558 "parse.y"
 yyval=Cyc_YY23(({struct _tuple25 _tmp6A5;_tmp6A5.f1=0,({enum Cyc_Absyn_AggrKind _tmp874=Cyc_yyget_YY22(&(yyyvsp[0]).v);_tmp6A5.f2=_tmp874;});_tmp6A5;}));
goto _LL0;case 125:
# 1561
 yyval=Cyc_YY22(0U);
goto _LL0;case 126:
# 1562 "parse.y"
 yyval=Cyc_YY22(1U);
goto _LL0;case 127:
# 1565
 yyval=Cyc_YY41(0);
goto _LL0;case 128:
# 1566 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*(*_tmp875)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_tmp875(Cyc_yyget_YY41(&(yyyvsp[1]).v));}));
goto _LL0;case 129:
# 1571 "parse.y"
 yyval=Cyc_YY25(0);
goto _LL0;case 130:  {
# 1573 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp401=Cyc_yyget_YY26(&(yyyvsp[0]).v);struct Cyc_List_List*x=_tmp401;for(0;x != 0;x=x->tl){
decls=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append;})((struct Cyc_List_List*)x->hd,decls);}}{
# 1577
struct Cyc_List_List*_tmp402=Cyc_Parse_get_aggrfield_tags(decls);struct Cyc_List_List*tags=_tmp402;
if(tags != 0)
({(void(*)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_iter_c;})(Cyc_Parse_substitute_aggrfield_tags,tags,decls);
yyval=Cyc_YY25(decls);
# 1582
goto _LL0;}}case 131:
# 1586 "parse.y"
 yyval=Cyc_YY26(({struct Cyc_List_List*_tmp403=_cycalloc(sizeof(*_tmp403));({struct Cyc_List_List*_tmp876=Cyc_yyget_YY25(&(yyyvsp[0]).v);_tmp403->hd=_tmp876;}),_tmp403->tl=0;_tmp403;}));
goto _LL0;case 132:
# 1587 "parse.y"
 yyval=Cyc_YY26(({struct Cyc_List_List*_tmp404=_cycalloc(sizeof(*_tmp404));({struct Cyc_List_List*_tmp878=Cyc_yyget_YY25(&(yyyvsp[1]).v);_tmp404->hd=_tmp878;}),({struct Cyc_List_List*_tmp877=Cyc_yyget_YY26(&(yyyvsp[0]).v);_tmp404->tl=_tmp877;});_tmp404;}));
goto _LL0;case 133:
# 1593 "parse.y"
 yyval=Cyc_YY19(({struct _tuple11*_tmp405=_region_malloc(yyr,sizeof(*_tmp405));_tmp405->tl=0,({struct _tuple12 _tmp879=Cyc_yyget_YY18(&(yyyvsp[0]).v);_tmp405->hd=_tmp879;});_tmp405;}));
goto _LL0;case 134:
# 1595 "parse.y"
 yyval=Cyc_YY19(({struct _tuple11*_tmp406=_region_malloc(yyr,sizeof(*_tmp406));({struct _tuple11*_tmp87B=Cyc_yyget_YY19(&(yyyvsp[0]).v);_tmp406->tl=_tmp87B;}),({struct _tuple12 _tmp87A=Cyc_yyget_YY18(&(yyyvsp[2]).v);_tmp406->hd=_tmp87A;});_tmp406;}));
goto _LL0;case 135:
# 1599 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _tmp6A6;({struct Cyc_Parse_Declarator _tmp87C=Cyc_yyget_YY28(&(yyyvsp[0]).v);_tmp6A6.f1=_tmp87C;}),_tmp6A6.f2=0;_tmp6A6;}));
goto _LL0;case 136:
# 1600 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _tmp6A7;({struct Cyc_Parse_Declarator _tmp87E=Cyc_yyget_YY28(&(yyyvsp[0]).v);_tmp6A7.f1=_tmp87E;}),({struct Cyc_Absyn_Exp*_tmp87D=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp6A7.f2=_tmp87D;});_tmp6A7;}));
goto _LL0;case 137:  {
# 1605 "parse.y"
struct _RegionHandle _tmp407=_new_region("temp");struct _RegionHandle*temp=& _tmp407;_push_region(temp);
{struct _tuple27 _tmp408=Cyc_yyget_YY36(&(yyyvsp[0]).v);struct _tuple27 _stmttmp16=_tmp408;struct _tuple27 _tmp409=_stmttmp16;void*_tmp40C;struct Cyc_Parse_Type_specifier _tmp40B;struct Cyc_Absyn_Tqual _tmp40A;_tmp40A=_tmp409.f1;_tmp40B=_tmp409.f2;_tmp40C=_tmp409.f3;{struct Cyc_Absyn_Tqual tq=_tmp40A;struct Cyc_Parse_Type_specifier tspecs=_tmp40B;struct Cyc_List_List*atts=_tmp40C;
if(tq.loc == 0U)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
struct _tuple13*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp40D=Cyc_yyget_YY30(&(yyyvsp[1]).v);struct Cyc_List_List*x=_tmp40D;for(0;x != 0;x=x->tl){
struct _tuple26*_tmp40E=(struct _tuple26*)x->hd;struct _tuple26*_stmttmp17=_tmp40E;struct _tuple26*_tmp40F=_stmttmp17;void*_tmp412;void*_tmp411;struct Cyc_Parse_Declarator _tmp410;_tmp410=_tmp40F->f1;_tmp411=_tmp40F->f2;_tmp412=_tmp40F->f3;{struct Cyc_Parse_Declarator d=_tmp410;struct Cyc_Absyn_Exp*wd=_tmp411;struct Cyc_Absyn_Exp*wh=_tmp412;
decls=({struct _tuple13*_tmp413=_region_malloc(temp,sizeof(*_tmp413));_tmp413->tl=decls,_tmp413->hd=d;_tmp413;});
widths_and_reqs=({struct Cyc_List_List*_tmp415=_region_malloc(temp,sizeof(*_tmp415));({struct _tuple17*_tmp87F=({struct _tuple17*_tmp414=_region_malloc(temp,sizeof(*_tmp414));_tmp414->f1=wd,_tmp414->f2=wh;_tmp414;});_tmp415->hd=_tmp87F;}),_tmp415->tl=widths_and_reqs;_tmp415;});}}}{
# 1615
void*_tmp416=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp416;
struct Cyc_List_List*_tmp417=({struct Cyc_List_List*(*_tmp883)(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_rzip;});struct _RegionHandle*_tmp882=temp;struct _RegionHandle*_tmp881=temp;struct Cyc_List_List*_tmp880=
Cyc_Parse_apply_tmss(temp,tq,t,decls,atts);
# 1616
_tmp883(_tmp882,_tmp881,_tmp880,widths_and_reqs);});struct Cyc_List_List*info=_tmp417;
# 1618
yyval=Cyc_YY25(({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple18*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_make_aggr_field,(unsigned)((yyyvsp[0]).l).first_line,info));
# 1620
_npop_handler(0);goto _LL0;}}}}
# 1606
;_pop_region();}case 138:
# 1627 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _tmp6A8;({struct Cyc_Absyn_Tqual _tmp885=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp6A8.f1=_tmp885;}),({struct Cyc_Parse_Type_specifier _tmp884=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp6A8.f2=_tmp884;}),_tmp6A8.f3=0;_tmp6A8;}));
goto _LL0;case 139:  {
# 1629 "parse.y"
struct _tuple27 _tmp418=Cyc_yyget_YY36(&(yyyvsp[1]).v);struct _tuple27 two=_tmp418;yyval=Cyc_YY36(({struct _tuple27 _tmp6A9;_tmp6A9.f1=two.f1,({struct Cyc_Parse_Type_specifier _tmp888=({unsigned _tmp887=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp886=Cyc_yyget_YY21(&(yyyvsp[0]).v);Cyc_Parse_combine_specifiers(_tmp887,_tmp886,two.f2);});_tmp6A9.f2=_tmp888;}),_tmp6A9.f3=two.f3;_tmp6A9;}));
goto _LL0;}case 140:
# 1631 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _tmp6AA;({struct Cyc_Absyn_Tqual _tmp88A=Cyc_yyget_YY24(&(yyyvsp[0]).v);_tmp6AA.f1=_tmp88A;}),({struct Cyc_Parse_Type_specifier _tmp889=Cyc_Parse_empty_spec(0U);_tmp6AA.f2=_tmp889;}),_tmp6AA.f3=0;_tmp6AA;}));
goto _LL0;case 141:  {
# 1633 "parse.y"
struct _tuple27 _tmp419=Cyc_yyget_YY36(&(yyyvsp[1]).v);struct _tuple27 two=_tmp419;yyval=Cyc_YY36(({struct _tuple27 _tmp6AB;({struct Cyc_Absyn_Tqual _tmp88C=({struct Cyc_Absyn_Tqual _tmp88B=Cyc_yyget_YY24(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp88B,two.f1);});_tmp6AB.f1=_tmp88C;}),_tmp6AB.f2=two.f2,_tmp6AB.f3=two.f3;_tmp6AB;}));
goto _LL0;}case 142:
# 1635 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _tmp6AC;({struct Cyc_Absyn_Tqual _tmp88F=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp6AC.f1=_tmp88F;}),({struct Cyc_Parse_Type_specifier _tmp88E=Cyc_Parse_empty_spec(0U);_tmp6AC.f2=_tmp88E;}),({struct Cyc_List_List*_tmp88D=Cyc_yyget_YY46(&(yyyvsp[0]).v);_tmp6AC.f3=_tmp88D;});_tmp6AC;}));
goto _LL0;case 143:  {
# 1637 "parse.y"
struct _tuple27 _tmp41A=Cyc_yyget_YY36(&(yyyvsp[1]).v);struct _tuple27 two=_tmp41A;yyval=Cyc_YY36(({struct _tuple27 _tmp6AD;_tmp6AD.f1=two.f1,_tmp6AD.f2=two.f2,({struct Cyc_List_List*_tmp892=({struct Cyc_List_List*(*_tmp891)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;});struct Cyc_List_List*_tmp890=Cyc_yyget_YY46(&(yyyvsp[0]).v);_tmp891(_tmp890,two.f3);});_tmp6AD.f3=_tmp892;});_tmp6AD;}));
goto _LL0;}case 144:
# 1643 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _tmp6AE;({struct Cyc_Absyn_Tqual _tmp894=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp6AE.f1=_tmp894;}),({struct Cyc_Parse_Type_specifier _tmp893=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp6AE.f2=_tmp893;}),_tmp6AE.f3=0;_tmp6AE;}));
goto _LL0;case 145:  {
# 1645 "parse.y"
struct _tuple27 _tmp41B=Cyc_yyget_YY36(&(yyyvsp[1]).v);struct _tuple27 two=_tmp41B;yyval=Cyc_YY36(({struct _tuple27 _tmp6AF;_tmp6AF.f1=two.f1,({struct Cyc_Parse_Type_specifier _tmp897=({unsigned _tmp896=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp895=Cyc_yyget_YY21(&(yyyvsp[0]).v);Cyc_Parse_combine_specifiers(_tmp896,_tmp895,two.f2);});_tmp6AF.f2=_tmp897;}),_tmp6AF.f3=two.f3;_tmp6AF;}));
goto _LL0;}case 146:
# 1647 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _tmp6B0;({struct Cyc_Absyn_Tqual _tmp899=Cyc_yyget_YY24(&(yyyvsp[0]).v);_tmp6B0.f1=_tmp899;}),({struct Cyc_Parse_Type_specifier _tmp898=Cyc_Parse_empty_spec(0U);_tmp6B0.f2=_tmp898;}),_tmp6B0.f3=0;_tmp6B0;}));
goto _LL0;case 147:  {
# 1649 "parse.y"
struct _tuple27 _tmp41C=Cyc_yyget_YY36(&(yyyvsp[1]).v);struct _tuple27 two=_tmp41C;yyval=Cyc_YY36(({struct _tuple27 _tmp6B1;({struct Cyc_Absyn_Tqual _tmp89B=({struct Cyc_Absyn_Tqual _tmp89A=Cyc_yyget_YY24(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp89A,two.f1);});_tmp6B1.f1=_tmp89B;}),_tmp6B1.f2=two.f2,_tmp6B1.f3=two.f3;_tmp6B1;}));
goto _LL0;}case 148:
# 1651 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _tmp6B2;({struct Cyc_Absyn_Tqual _tmp89E=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp6B2.f1=_tmp89E;}),({struct Cyc_Parse_Type_specifier _tmp89D=Cyc_Parse_empty_spec(0U);_tmp6B2.f2=_tmp89D;}),({struct Cyc_List_List*_tmp89C=Cyc_yyget_YY46(&(yyyvsp[0]).v);_tmp6B2.f3=_tmp89C;});_tmp6B2;}));
goto _LL0;case 149:  {
# 1653 "parse.y"
struct _tuple27 _tmp41D=Cyc_yyget_YY36(&(yyyvsp[1]).v);struct _tuple27 two=_tmp41D;yyval=Cyc_YY36(({struct _tuple27 _tmp6B3;_tmp6B3.f1=two.f1,_tmp6B3.f2=two.f2,({struct Cyc_List_List*_tmp8A1=({struct Cyc_List_List*(*_tmp8A0)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;});struct Cyc_List_List*_tmp89F=Cyc_yyget_YY46(&(yyyvsp[0]).v);_tmp8A0(_tmp89F,two.f3);});_tmp6B3.f3=_tmp8A1;});_tmp6B3;}));
goto _LL0;}case 150:
# 1659 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_List_List*_tmp41E=_region_malloc(yyr,sizeof(*_tmp41E));({struct _tuple26*_tmp8A2=Cyc_yyget_YY29(&(yyyvsp[0]).v);_tmp41E->hd=_tmp8A2;}),_tmp41E->tl=0;_tmp41E;}));
goto _LL0;case 151:
# 1661 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_List_List*_tmp41F=_region_malloc(yyr,sizeof(*_tmp41F));({struct _tuple26*_tmp8A4=Cyc_yyget_YY29(&(yyyvsp[2]).v);_tmp41F->hd=_tmp8A4;}),({struct Cyc_List_List*_tmp8A3=Cyc_yyget_YY30(&(yyyvsp[0]).v);_tmp41F->tl=_tmp8A3;});_tmp41F;}));
goto _LL0;case 152:
# 1666 "parse.y"
 yyval=Cyc_YY29(({struct _tuple26*_tmp420=_region_malloc(yyr,sizeof(*_tmp420));({struct Cyc_Parse_Declarator _tmp8A6=Cyc_yyget_YY28(&(yyyvsp[0]).v);_tmp420->f1=_tmp8A6;}),_tmp420->f2=0,({struct Cyc_Absyn_Exp*_tmp8A5=Cyc_yyget_YY58(&(yyyvsp[1]).v);_tmp420->f3=_tmp8A5;});_tmp420;}));
goto _LL0;case 153:
# 1670 "parse.y"
 yyval=Cyc_YY29(({struct _tuple26*_tmp424=_region_malloc(yyr,sizeof(*_tmp424));({struct _tuple0*_tmp8AB=({struct _tuple0*_tmp423=_cycalloc(sizeof(*_tmp423));({union Cyc_Absyn_Nmspace _tmp8AA=Cyc_Absyn_Rel_n(0);_tmp423->f1=_tmp8AA;}),({struct _fat_ptr*_tmp8A9=({struct _fat_ptr*_tmp422=_cycalloc(sizeof(*_tmp422));({struct _fat_ptr _tmp8A8=({const char*_tmp421="";_tag_fat(_tmp421,sizeof(char),1U);});*_tmp422=_tmp8A8;});_tmp422;});_tmp423->f2=_tmp8A9;});_tmp423;});(_tmp424->f1).id=_tmp8AB;}),(_tmp424->f1).varloc=0U,(_tmp424->f1).tms=0,({struct Cyc_Absyn_Exp*_tmp8A7=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp424->f2=_tmp8A7;}),_tmp424->f3=0;_tmp424;}));
# 1672
goto _LL0;case 154:
# 1675 "parse.y"
 yyval=Cyc_YY29(({struct _tuple26*_tmp428=_region_malloc(yyr,sizeof(*_tmp428));({struct _tuple0*_tmp8AF=({struct _tuple0*_tmp427=_cycalloc(sizeof(*_tmp427));({union Cyc_Absyn_Nmspace _tmp8AE=Cyc_Absyn_Rel_n(0);_tmp427->f1=_tmp8AE;}),({struct _fat_ptr*_tmp8AD=({struct _fat_ptr*_tmp426=_cycalloc(sizeof(*_tmp426));({struct _fat_ptr _tmp8AC=({const char*_tmp425="";_tag_fat(_tmp425,sizeof(char),1U);});*_tmp426=_tmp8AC;});_tmp426;});_tmp427->f2=_tmp8AD;});_tmp427;});(_tmp428->f1).id=_tmp8AF;}),(_tmp428->f1).varloc=0U,(_tmp428->f1).tms=0,_tmp428->f2=0,_tmp428->f3=0;_tmp428;}));
# 1677
goto _LL0;case 155:
# 1678 "parse.y"
 yyval=Cyc_YY29(({struct _tuple26*_tmp429=_region_malloc(yyr,sizeof(*_tmp429));({struct Cyc_Parse_Declarator _tmp8B1=Cyc_yyget_YY28(&(yyyvsp[0]).v);_tmp429->f1=_tmp8B1;}),({struct Cyc_Absyn_Exp*_tmp8B0=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp429->f2=_tmp8B0;}),_tmp429->f3=0;_tmp429;}));
goto _LL0;case 156:
# 1682 "parse.y"
 yyval=Cyc_YY58(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));
goto _LL0;case 157:
# 1683 "parse.y"
 yyval=Cyc_YY58(0);
goto _LL0;case 158:  {
# 1689 "parse.y"
int _tmp42A=Cyc_yyget_YY32(&(yyyvsp[0]).v);int is_extensible=_tmp42A;
struct Cyc_List_List*_tmp42B=({struct Cyc_List_List*(*_tmp8B3)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;});unsigned _tmp8B2=(unsigned)((yyyvsp[2]).l).first_line;_tmp8B3(Cyc_Parse_typ2tvar,_tmp8B2,Cyc_yyget_YY41(&(yyyvsp[2]).v));});struct Cyc_List_List*ts=_tmp42B;
struct Cyc_Absyn_TypeDecl*_tmp42C=({struct _tuple0*_tmp8B8=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp8B7=ts;struct Cyc_Core_Opt*_tmp8B6=({struct Cyc_Core_Opt*_tmp42E=_cycalloc(sizeof(*_tmp42E));({struct Cyc_List_List*_tmp8B4=Cyc_yyget_YY35(&(yyyvsp[4]).v);_tmp42E->v=_tmp8B4;});_tmp42E;});int _tmp8B5=is_extensible;Cyc_Absyn_datatype_tdecl(2U,_tmp8B8,_tmp8B7,_tmp8B6,_tmp8B5,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_TypeDecl*dd=_tmp42C;
# 1693
yyval=Cyc_YY21(({void*_tmp8B9=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp42D=_cycalloc(sizeof(*_tmp42D));_tmp42D->tag=10,_tmp42D->f1=dd,_tmp42D->f2=0;_tmp42D;});Cyc_Parse_type_spec(_tmp8B9,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1695
goto _LL0;}case 159:  {
# 1696 "parse.y"
int _tmp42F=Cyc_yyget_YY32(&(yyyvsp[0]).v);int is_extensible=_tmp42F;
yyval=Cyc_YY21(({void*_tmp8BC=({union Cyc_Absyn_DatatypeInfo _tmp8BB=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp6B4;({struct _tuple0*_tmp8BA=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp6B4.name=_tmp8BA;}),_tmp6B4.is_extensible=is_extensible;_tmp6B4;}));Cyc_Absyn_datatype_type(_tmp8BB,Cyc_yyget_YY41(&(yyyvsp[2]).v));});Cyc_Parse_type_spec(_tmp8BC,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1699
goto _LL0;}case 160:  {
# 1700 "parse.y"
int _tmp430=Cyc_yyget_YY32(&(yyyvsp[0]).v);int is_extensible=_tmp430;
yyval=Cyc_YY21(({void*_tmp8C0=({union Cyc_Absyn_DatatypeFieldInfo _tmp8BF=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp6B5;({struct _tuple0*_tmp8BE=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp6B5.datatype_name=_tmp8BE;}),({struct _tuple0*_tmp8BD=Cyc_yyget_QualId_tok(&(yyyvsp[3]).v);_tmp6B5.field_name=_tmp8BD;}),_tmp6B5.is_extensible=is_extensible;_tmp6B5;}));Cyc_Absyn_datatype_field_type(_tmp8BF,Cyc_yyget_YY41(&(yyyvsp[4]).v));});Cyc_Parse_type_spec(_tmp8C0,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1703
goto _LL0;}case 161:
# 1706 "parse.y"
 yyval=Cyc_YY32(0);
goto _LL0;case 162:
# 1707 "parse.y"
 yyval=Cyc_YY32(1);
goto _LL0;case 163:
# 1711 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_tmp431=_cycalloc(sizeof(*_tmp431));({struct Cyc_Absyn_Datatypefield*_tmp8C1=Cyc_yyget_YY34(&(yyyvsp[0]).v);_tmp431->hd=_tmp8C1;}),_tmp431->tl=0;_tmp431;}));
goto _LL0;case 164:
# 1712 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_tmp432=_cycalloc(sizeof(*_tmp432));({struct Cyc_Absyn_Datatypefield*_tmp8C2=Cyc_yyget_YY34(&(yyyvsp[0]).v);_tmp432->hd=_tmp8C2;}),_tmp432->tl=0;_tmp432;}));
goto _LL0;case 165:
# 1713 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_tmp433=_cycalloc(sizeof(*_tmp433));({struct Cyc_Absyn_Datatypefield*_tmp8C4=Cyc_yyget_YY34(&(yyyvsp[0]).v);_tmp433->hd=_tmp8C4;}),({struct Cyc_List_List*_tmp8C3=Cyc_yyget_YY35(&(yyyvsp[2]).v);_tmp433->tl=_tmp8C3;});_tmp433;}));
goto _LL0;case 166:
# 1714 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_tmp434=_cycalloc(sizeof(*_tmp434));({struct Cyc_Absyn_Datatypefield*_tmp8C6=Cyc_yyget_YY34(&(yyyvsp[0]).v);_tmp434->hd=_tmp8C6;}),({struct Cyc_List_List*_tmp8C5=Cyc_yyget_YY35(&(yyyvsp[2]).v);_tmp434->tl=_tmp8C5;});_tmp434;}));
goto _LL0;case 167:
# 1718 "parse.y"
 yyval=Cyc_YY33(2U);
goto _LL0;case 168:
# 1719 "parse.y"
 yyval=Cyc_YY33(3U);
goto _LL0;case 169:
# 1720 "parse.y"
 yyval=Cyc_YY33(0U);
goto _LL0;case 170:
# 1724 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_Absyn_Datatypefield*_tmp435=_cycalloc(sizeof(*_tmp435));({struct _tuple0*_tmp8C8=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp435->name=_tmp8C8;}),_tmp435->typs=0,_tmp435->loc=(unsigned)((yyyvsp[0]).l).first_line,({enum Cyc_Absyn_Scope _tmp8C7=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp435->sc=_tmp8C7;});_tmp435;}));
goto _LL0;case 171:  {
# 1726 "parse.y"
struct Cyc_List_List*_tmp436=({struct Cyc_List_List*(*_tmp8CC)(struct _tuple20*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct _tuple20*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;});struct _tuple20*(*_tmp8CB)(unsigned,struct _tuple8*)=({(struct _tuple20*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ;});unsigned _tmp8CA=(unsigned)((yyyvsp[3]).l).first_line;_tmp8CC(_tmp8CB,_tmp8CA,({struct Cyc_List_List*(*_tmp8C9)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_tmp8C9(Cyc_yyget_YY39(&(yyyvsp[3]).v));}));});struct Cyc_List_List*typs=_tmp436;
yyval=Cyc_YY34(({struct Cyc_Absyn_Datatypefield*_tmp437=_cycalloc(sizeof(*_tmp437));({struct _tuple0*_tmp8CE=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp437->name=_tmp8CE;}),_tmp437->typs=typs,_tmp437->loc=(unsigned)((yyyvsp[0]).l).first_line,({enum Cyc_Absyn_Scope _tmp8CD=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp437->sc=_tmp8CD;});_tmp437;}));
goto _LL0;}case 172:
# 1732 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 173:  {
# 1734 "parse.y"
struct Cyc_Parse_Declarator _tmp438=Cyc_yyget_YY28(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator two=_tmp438;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6B6;_tmp6B6.id=two.id,_tmp6B6.varloc=two.varloc,({struct Cyc_List_List*_tmp8D1=({struct Cyc_List_List*(*_tmp8D0)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append;});struct Cyc_List_List*_tmp8CF=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp8D0(_tmp8CF,two.tms);});_tmp6B6.tms=_tmp8D1;});_tmp6B6;}));
goto _LL0;}case 174:
# 1741 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 175:  {
# 1743 "parse.y"
struct Cyc_Parse_Declarator _tmp439=Cyc_yyget_YY28(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator two=_tmp439;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6B7;_tmp6B7.id=two.id,_tmp6B7.varloc=two.varloc,({struct Cyc_List_List*_tmp8D4=({struct Cyc_List_List*(*_tmp8D3)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append;});struct Cyc_List_List*_tmp8D2=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp8D3(_tmp8D2,two.tms);});_tmp6B7.tms=_tmp8D4;});_tmp6B7;}));
goto _LL0;}case 176:
# 1748 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6B8;({struct _tuple0*_tmp8D5=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp6B8.id=_tmp8D5;}),_tmp6B8.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp6B8.tms=0;_tmp6B8;}));
goto _LL0;case 177:
# 1749 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 178:  {
# 1753 "parse.y"
struct Cyc_Parse_Declarator _tmp43A=Cyc_yyget_YY28(&(yyyvsp[2]).v);struct Cyc_Parse_Declarator d=_tmp43A;
({struct Cyc_List_List*_tmp8D8=({struct Cyc_List_List*_tmp43C=_region_malloc(yyr,sizeof(*_tmp43C));({void*_tmp8D7=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp43B=_region_malloc(yyr,sizeof(*_tmp43B));_tmp43B->tag=5,_tmp43B->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp8D6=Cyc_yyget_YY46(&(yyyvsp[1]).v);_tmp43B->f2=_tmp8D6;});_tmp43B;});_tmp43C->hd=_tmp8D7;}),_tmp43C->tl=d.tms;_tmp43C;});d.tms=_tmp8D8;});
yyval=(yyyvsp[2]).v;
# 1757
goto _LL0;}case 179:
# 1758 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6B9;({struct _tuple0*_tmp8DE=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).id;_tmp6B9.id=_tmp8DE;}),({unsigned _tmp8DD=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).varloc;_tmp6B9.varloc=_tmp8DD;}),({struct Cyc_List_List*_tmp8DC=({struct Cyc_List_List*_tmp43E=_region_malloc(yyr,sizeof(*_tmp43E));({void*_tmp8DB=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp43D=_region_malloc(yyr,sizeof(*_tmp43D));_tmp43D->tag=0,({void*_tmp8DA=Cyc_yyget_YY52(&(yyyvsp[3]).v);_tmp43D->f1=_tmp8DA;}),_tmp43D->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp43D;});_tmp43E->hd=_tmp8DB;}),({struct Cyc_List_List*_tmp8D9=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).tms;_tmp43E->tl=_tmp8D9;});_tmp43E;});_tmp6B9.tms=_tmp8DC;});_tmp6B9;}));
goto _LL0;case 180:
# 1760 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6BA;({struct _tuple0*_tmp8E5=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).id;_tmp6BA.id=_tmp8E5;}),({unsigned _tmp8E4=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).varloc;_tmp6BA.varloc=_tmp8E4;}),({
struct Cyc_List_List*_tmp8E3=({struct Cyc_List_List*_tmp440=_region_malloc(yyr,sizeof(*_tmp440));({void*_tmp8E2=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp43F=_region_malloc(yyr,sizeof(*_tmp43F));_tmp43F->tag=1,({struct Cyc_Absyn_Exp*_tmp8E1=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp43F->f1=_tmp8E1;}),({void*_tmp8E0=Cyc_yyget_YY52(&(yyyvsp[4]).v);_tmp43F->f2=_tmp8E0;}),_tmp43F->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp43F;});_tmp440->hd=_tmp8E2;}),({struct Cyc_List_List*_tmp8DF=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).tms;_tmp440->tl=_tmp8DF;});_tmp440;});_tmp6BA.tms=_tmp8E3;});_tmp6BA;}));
goto _LL0;case 181:  {
# 1763 "parse.y"
struct _tuple28*_tmp441=Cyc_yyget_YY40(&(yyyvsp[2]).v);struct _tuple28*_stmttmp18=_tmp441;struct _tuple28*_tmp442=_stmttmp18;void*_tmp447;void*_tmp446;void*_tmp445;int _tmp444;void*_tmp443;_tmp443=_tmp442->f1;_tmp444=_tmp442->f2;_tmp445=_tmp442->f3;_tmp446=_tmp442->f4;_tmp447=_tmp442->f5;{struct Cyc_List_List*lis=_tmp443;int b=_tmp444;struct Cyc_Absyn_VarargInfo*c=_tmp445;void*eff=_tmp446;struct Cyc_List_List*po=_tmp447;
struct Cyc_Absyn_Exp*_tmp448=(Cyc_yyget_YY59(&(yyyvsp[4]).v)).f1;struct Cyc_Absyn_Exp*req=_tmp448;
struct Cyc_Absyn_Exp*_tmp449=(Cyc_yyget_YY59(&(yyyvsp[4]).v)).f2;struct Cyc_Absyn_Exp*ens=_tmp449;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6BB;({struct _tuple0*_tmp8EB=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).id;_tmp6BB.id=_tmp8EB;}),({unsigned _tmp8EA=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).varloc;_tmp6BB.varloc=_tmp8EA;}),({struct Cyc_List_List*_tmp8E9=({struct Cyc_List_List*_tmp44C=_region_malloc(yyr,sizeof(*_tmp44C));({void*_tmp8E8=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp44B=_region_malloc(yyr,sizeof(*_tmp44B));_tmp44B->tag=3,({void*_tmp8E7=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp44A=_region_malloc(yyr,sizeof(*_tmp44A));_tmp44A->tag=1,_tmp44A->f1=lis,_tmp44A->f2=b,_tmp44A->f3=c,_tmp44A->f4=eff,_tmp44A->f5=po,_tmp44A->f6=req,_tmp44A->f7=ens;_tmp44A;});_tmp44B->f1=_tmp8E7;});_tmp44B;});_tmp44C->hd=_tmp8E8;}),({struct Cyc_List_List*_tmp8E6=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).tms;_tmp44C->tl=_tmp8E6;});_tmp44C;});_tmp6BB.tms=_tmp8E9;});_tmp6BB;}));
# 1768
goto _LL0;}}case 182:
# 1769 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6BC;({struct _tuple0*_tmp8F2=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).id;_tmp6BC.id=_tmp8F2;}),({unsigned _tmp8F1=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).varloc;_tmp6BC.varloc=_tmp8F1;}),({struct Cyc_List_List*_tmp8F0=({struct Cyc_List_List*_tmp44F=_region_malloc(yyr,sizeof(*_tmp44F));({void*_tmp8EF=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp44E=_region_malloc(yyr,sizeof(*_tmp44E));_tmp44E->tag=3,({void*_tmp8EE=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp44D=_region_malloc(yyr,sizeof(*_tmp44D));_tmp44D->tag=0,({struct Cyc_List_List*_tmp8ED=Cyc_yyget_YY37(&(yyyvsp[2]).v);_tmp44D->f1=_tmp8ED;}),_tmp44D->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp44D;});_tmp44E->f1=_tmp8EE;});_tmp44E;});_tmp44F->hd=_tmp8EF;}),({struct Cyc_List_List*_tmp8EC=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).tms;_tmp44F->tl=_tmp8EC;});_tmp44F;});_tmp6BC.tms=_tmp8F0;});_tmp6BC;}));
goto _LL0;case 183:  {
# 1772
struct Cyc_List_List*_tmp450=({struct Cyc_List_List*(*_tmp8F5)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;});unsigned _tmp8F4=(unsigned)((yyyvsp[1]).l).first_line;_tmp8F5(Cyc_Parse_typ2tvar,_tmp8F4,({struct Cyc_List_List*(*_tmp8F3)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_tmp8F3(Cyc_yyget_YY41(&(yyyvsp[2]).v));}));});struct Cyc_List_List*ts=_tmp450;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6BD;({struct _tuple0*_tmp8FA=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).id;_tmp6BD.id=_tmp8FA;}),({unsigned _tmp8F9=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).varloc;_tmp6BD.varloc=_tmp8F9;}),({struct Cyc_List_List*_tmp8F8=({struct Cyc_List_List*_tmp452=_region_malloc(yyr,sizeof(*_tmp452));({void*_tmp8F7=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp451=_region_malloc(yyr,sizeof(*_tmp451));_tmp451->tag=4,_tmp451->f1=ts,_tmp451->f2=(unsigned)((yyyvsp[0]).l).first_line,_tmp451->f3=0;_tmp451;});_tmp452->hd=_tmp8F7;}),({struct Cyc_List_List*_tmp8F6=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).tms;_tmp452->tl=_tmp8F6;});_tmp452;});_tmp6BD.tms=_tmp8F8;});_tmp6BD;}));
# 1775
goto _LL0;}case 184:
# 1776 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6BE;({struct _tuple0*_tmp900=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).id;_tmp6BE.id=_tmp900;}),({unsigned _tmp8FF=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).varloc;_tmp6BE.varloc=_tmp8FF;}),({struct Cyc_List_List*_tmp8FE=({struct Cyc_List_List*_tmp454=_region_malloc(yyr,sizeof(*_tmp454));({void*_tmp8FD=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp453=_region_malloc(yyr,sizeof(*_tmp453));_tmp453->tag=5,_tmp453->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp8FC=Cyc_yyget_YY46(&(yyyvsp[1]).v);_tmp453->f2=_tmp8FC;});_tmp453;});_tmp454->hd=_tmp8FD;}),({
struct Cyc_List_List*_tmp8FB=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).tms;_tmp454->tl=_tmp8FB;});_tmp454;});
# 1776
_tmp6BE.tms=_tmp8FE;});_tmp6BE;}));
# 1779
goto _LL0;case 185:
# 1783 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6BF;({struct _tuple0*_tmp901=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp6BF.id=_tmp901;}),_tmp6BF.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp6BF.tms=0;_tmp6BF;}));
goto _LL0;case 186:
# 1784 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6C0;({struct _tuple0*_tmp902=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp6C0.id=_tmp902;}),_tmp6C0.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp6C0.tms=0;_tmp6C0;}));
goto _LL0;case 187:
# 1785 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 188:  {
# 1789 "parse.y"
struct Cyc_Parse_Declarator _tmp455=Cyc_yyget_YY28(&(yyyvsp[2]).v);struct Cyc_Parse_Declarator d=_tmp455;
({struct Cyc_List_List*_tmp905=({struct Cyc_List_List*_tmp457=_region_malloc(yyr,sizeof(*_tmp457));({void*_tmp904=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp456=_region_malloc(yyr,sizeof(*_tmp456));_tmp456->tag=5,_tmp456->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp903=Cyc_yyget_YY46(&(yyyvsp[1]).v);_tmp456->f2=_tmp903;});_tmp456;});_tmp457->hd=_tmp904;}),_tmp457->tl=d.tms;_tmp457;});d.tms=_tmp905;});
yyval=(yyyvsp[2]).v;
# 1793
goto _LL0;}case 189:  {
# 1794 "parse.y"
struct Cyc_Parse_Declarator _tmp458=Cyc_yyget_YY28(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp458;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6C1;_tmp6C1.id=one.id,_tmp6C1.varloc=one.varloc,({
struct Cyc_List_List*_tmp908=({struct Cyc_List_List*_tmp45A=_region_malloc(yyr,sizeof(*_tmp45A));({void*_tmp907=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp459=_region_malloc(yyr,sizeof(*_tmp459));_tmp459->tag=0,({void*_tmp906=Cyc_yyget_YY52(&(yyyvsp[3]).v);_tmp459->f1=_tmp906;}),_tmp459->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp459;});_tmp45A->hd=_tmp907;}),_tmp45A->tl=one.tms;_tmp45A;});_tmp6C1.tms=_tmp908;});_tmp6C1;}));
goto _LL0;}case 190:  {
# 1798 "parse.y"
struct Cyc_Parse_Declarator _tmp45B=Cyc_yyget_YY28(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp45B;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6C2;_tmp6C2.id=one.id,_tmp6C2.varloc=one.varloc,({
struct Cyc_List_List*_tmp90C=({struct Cyc_List_List*_tmp45D=_region_malloc(yyr,sizeof(*_tmp45D));({void*_tmp90B=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp45C=_region_malloc(yyr,sizeof(*_tmp45C));_tmp45C->tag=1,({struct Cyc_Absyn_Exp*_tmp90A=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp45C->f1=_tmp90A;}),({void*_tmp909=Cyc_yyget_YY52(&(yyyvsp[4]).v);_tmp45C->f2=_tmp909;}),_tmp45C->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp45C;});_tmp45D->hd=_tmp90B;}),_tmp45D->tl=one.tms;_tmp45D;});_tmp6C2.tms=_tmp90C;});_tmp6C2;}));
# 1802
goto _LL0;}case 191:  {
# 1803 "parse.y"
struct _tuple28*_tmp45E=Cyc_yyget_YY40(&(yyyvsp[2]).v);struct _tuple28*_stmttmp19=_tmp45E;struct _tuple28*_tmp45F=_stmttmp19;void*_tmp464;void*_tmp463;void*_tmp462;int _tmp461;void*_tmp460;_tmp460=_tmp45F->f1;_tmp461=_tmp45F->f2;_tmp462=_tmp45F->f3;_tmp463=_tmp45F->f4;_tmp464=_tmp45F->f5;{struct Cyc_List_List*lis=_tmp460;int b=_tmp461;struct Cyc_Absyn_VarargInfo*c=_tmp462;void*eff=_tmp463;struct Cyc_List_List*po=_tmp464;
struct Cyc_Absyn_Exp*_tmp465=(Cyc_yyget_YY59(&(yyyvsp[4]).v)).f1;struct Cyc_Absyn_Exp*req=_tmp465;
struct Cyc_Absyn_Exp*_tmp466=(Cyc_yyget_YY59(&(yyyvsp[4]).v)).f2;struct Cyc_Absyn_Exp*ens=_tmp466;
struct Cyc_Parse_Declarator _tmp467=Cyc_yyget_YY28(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp467;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6C3;_tmp6C3.id=one.id,_tmp6C3.varloc=one.varloc,({struct Cyc_List_List*_tmp90F=({struct Cyc_List_List*_tmp46A=_region_malloc(yyr,sizeof(*_tmp46A));({void*_tmp90E=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp469=_region_malloc(yyr,sizeof(*_tmp469));_tmp469->tag=3,({void*_tmp90D=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp468=_region_malloc(yyr,sizeof(*_tmp468));_tmp468->tag=1,_tmp468->f1=lis,_tmp468->f2=b,_tmp468->f3=c,_tmp468->f4=eff,_tmp468->f5=po,_tmp468->f6=req,_tmp468->f7=ens;_tmp468;});_tmp469->f1=_tmp90D;});_tmp469;});_tmp46A->hd=_tmp90E;}),_tmp46A->tl=one.tms;_tmp46A;});_tmp6C3.tms=_tmp90F;});_tmp6C3;}));
# 1809
goto _LL0;}}case 192:  {
# 1810 "parse.y"
struct Cyc_Parse_Declarator _tmp46B=Cyc_yyget_YY28(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp46B;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6C4;_tmp6C4.id=one.id,_tmp6C4.varloc=one.varloc,({struct Cyc_List_List*_tmp913=({struct Cyc_List_List*_tmp46E=_region_malloc(yyr,sizeof(*_tmp46E));({void*_tmp912=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp46D=_region_malloc(yyr,sizeof(*_tmp46D));_tmp46D->tag=3,({void*_tmp911=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp46C=_region_malloc(yyr,sizeof(*_tmp46C));_tmp46C->tag=0,({struct Cyc_List_List*_tmp910=Cyc_yyget_YY37(&(yyyvsp[2]).v);_tmp46C->f1=_tmp910;}),_tmp46C->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp46C;});_tmp46D->f1=_tmp911;});_tmp46D;});_tmp46E->hd=_tmp912;}),_tmp46E->tl=one.tms;_tmp46E;});_tmp6C4.tms=_tmp913;});_tmp6C4;}));
goto _LL0;}case 193:  {
# 1814
struct Cyc_List_List*_tmp46F=({struct Cyc_List_List*(*_tmp916)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;});unsigned _tmp915=(unsigned)((yyyvsp[1]).l).first_line;_tmp916(Cyc_Parse_typ2tvar,_tmp915,({struct Cyc_List_List*(*_tmp914)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_tmp914(Cyc_yyget_YY41(&(yyyvsp[2]).v));}));});struct Cyc_List_List*ts=_tmp46F;
struct Cyc_Parse_Declarator _tmp470=Cyc_yyget_YY28(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp470;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6C5;_tmp6C5.id=one.id,_tmp6C5.varloc=one.varloc,({struct Cyc_List_List*_tmp918=({struct Cyc_List_List*_tmp472=_region_malloc(yyr,sizeof(*_tmp472));({void*_tmp917=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp471=_region_malloc(yyr,sizeof(*_tmp471));_tmp471->tag=4,_tmp471->f1=ts,_tmp471->f2=(unsigned)((yyyvsp[0]).l).first_line,_tmp471->f3=0;_tmp471;});_tmp472->hd=_tmp917;}),_tmp472->tl=one.tms;_tmp472;});_tmp6C5.tms=_tmp918;});_tmp6C5;}));
goto _LL0;}case 194:  {
# 1818 "parse.y"
struct Cyc_Parse_Declarator _tmp473=Cyc_yyget_YY28(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp473;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6C6;_tmp6C6.id=one.id,_tmp6C6.varloc=one.varloc,({struct Cyc_List_List*_tmp91B=({struct Cyc_List_List*_tmp475=_region_malloc(yyr,sizeof(*_tmp475));({void*_tmp91A=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp474=_region_malloc(yyr,sizeof(*_tmp474));_tmp474->tag=5,_tmp474->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp919=Cyc_yyget_YY46(&(yyyvsp[1]).v);_tmp474->f2=_tmp919;});_tmp474;});_tmp475->hd=_tmp91A;}),_tmp475->tl=one.tms;_tmp475;});_tmp6C6.tms=_tmp91B;});_tmp6C6;}));
goto _LL0;}case 195:
# 1824 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 196:
# 1825 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_List_List*(*_tmp91D)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append;});struct Cyc_List_List*_tmp91C=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp91D(_tmp91C,Cyc_yyget_YY27(&(yyyvsp[1]).v));}));
goto _LL0;case 197:  {
# 1829 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY46(&(yyyvsp[3]).v)!= 0)
ans=({struct Cyc_List_List*_tmp477=_region_malloc(yyr,sizeof(*_tmp477));({void*_tmp91F=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp476=_region_malloc(yyr,sizeof(*_tmp476));_tmp476->tag=5,_tmp476->f1=(unsigned)((yyyvsp[3]).l).first_line,({struct Cyc_List_List*_tmp91E=Cyc_yyget_YY46(&(yyyvsp[3]).v);_tmp476->f2=_tmp91E;});_tmp476;});_tmp477->hd=_tmp91F;}),_tmp477->tl=ans;_tmp477;});{
# 1833
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple22 _tmp478=*Cyc_yyget_YY1(&(yyyvsp[0]).v);struct _tuple22 _stmttmp1A=_tmp478;struct _tuple22 _tmp479=_stmttmp1A;void*_tmp47C;void*_tmp47B;unsigned _tmp47A;_tmp47A=_tmp479.f1;_tmp47B=_tmp479.f2;_tmp47C=_tmp479.f3;{unsigned ploc=_tmp47A;void*nullable=_tmp47B;void*bound=_tmp47C;
if(Cyc_Flags_porting_c_code)
ptrloc=({struct Cyc_Absyn_PtrLoc*_tmp47D=_cycalloc(sizeof(*_tmp47D));_tmp47D->ptr_loc=ploc,_tmp47D->rgn_loc=(unsigned)((yyyvsp[2]).l).first_line,_tmp47D->zt_loc=(unsigned)((yyyvsp[1]).l).first_line;_tmp47D;});{
struct _tuple15 _tmp47E=({unsigned _tmp923=ploc;void*_tmp922=nullable;void*_tmp921=bound;void*_tmp920=Cyc_yyget_YY45(&(yyyvsp[2]).v);Cyc_Parse_collapse_pointer_quals(_tmp923,_tmp922,_tmp921,_tmp920,Cyc_yyget_YY57(&(yyyvsp[1]).v));});struct _tuple15 _stmttmp1B=_tmp47E;struct _tuple15 _tmp47F=_stmttmp1B;void*_tmp484;void*_tmp483;void*_tmp482;void*_tmp481;void*_tmp480;_tmp480=_tmp47F.f1;_tmp481=_tmp47F.f2;_tmp482=_tmp47F.f3;_tmp483=_tmp47F.f4;_tmp484=_tmp47F.f5;{void*nullable=_tmp480;void*bound=_tmp481;void*zeroterm=_tmp482;void*rgn_opt=_tmp483;void*autoreleased=_tmp484;
ans=({struct Cyc_List_List*_tmp486=_region_malloc(yyr,sizeof(*_tmp486));({void*_tmp925=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp485=_region_malloc(yyr,sizeof(*_tmp485));_tmp485->tag=2,(_tmp485->f1).rgn=rgn_opt,(_tmp485->f1).nullable=nullable,(_tmp485->f1).bounds=bound,(_tmp485->f1).zero_term=zeroterm,(_tmp485->f1).ptrloc=ptrloc,(_tmp485->f1).autoreleased=autoreleased,({struct Cyc_Absyn_Tqual _tmp924=Cyc_yyget_YY24(&(yyyvsp[4]).v);_tmp485->f2=_tmp924;});_tmp485;});_tmp486->hd=_tmp925;}),_tmp486->tl=ans;_tmp486;});
yyval=Cyc_YY27(ans);
# 1841
goto _LL0;}}}}}case 198:
# 1843
 yyval=Cyc_YY57(0);
goto _LL0;case 199:
# 1844 "parse.y"
 yyval=Cyc_YY57(({struct Cyc_List_List*_tmp487=_region_malloc(yyr,sizeof(*_tmp487));({void*_tmp927=Cyc_yyget_YY56(&(yyyvsp[0]).v);_tmp487->hd=_tmp927;}),({struct Cyc_List_List*_tmp926=Cyc_yyget_YY57(&(yyyvsp[1]).v);_tmp487->tl=_tmp926;});_tmp487;}));
goto _LL0;case 200:
# 1849 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_tmp488=_region_malloc(yyr,sizeof(*_tmp488));_tmp488->tag=0,({struct Cyc_Absyn_Exp*_tmp928=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp488->f1=_tmp928;});_tmp488;}));
goto _LL0;case 201:
# 1850 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*_tmp489=_region_malloc(yyr,sizeof(*_tmp489));_tmp489->tag=1,({void*_tmp929=Cyc_yyget_YY45(&(yyyvsp[2]).v);_tmp489->f1=_tmp929;});_tmp489;}));
goto _LL0;case 202:
# 1851 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_tmp48A=_region_malloc(yyr,sizeof(*_tmp48A));_tmp48A->tag=2;_tmp48A;}));
goto _LL0;case 203:
# 1852 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_tmp48B=_region_malloc(yyr,sizeof(*_tmp48B));_tmp48B->tag=3;_tmp48B;}));
goto _LL0;case 204:
# 1853 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct*_tmp48C=_region_malloc(yyr,sizeof(*_tmp48C));_tmp48C->tag=6;_tmp48C;}));
goto _LL0;case 205:
# 1854 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp48D=_region_malloc(yyr,sizeof(*_tmp48D));_tmp48D->tag=4;_tmp48D;}));
goto _LL0;case 206:
# 1855 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp48E=_region_malloc(yyr,sizeof(*_tmp48E));_tmp48E->tag=5;_tmp48E;}));
goto _LL0;case 207:
# 1856 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_tmp48F=_region_malloc(yyr,sizeof(*_tmp48F));_tmp48F->tag=7;_tmp48F;}));
goto _LL0;case 208:
# 1857 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_tmp490=_region_malloc(yyr,sizeof(*_tmp490));_tmp490->tag=8;_tmp490;}));
goto _LL0;case 209:
# 1863 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_tmp491=_cycalloc(sizeof(*_tmp491));_tmp491->f1=(unsigned)((yyyvsp[0]).l).first_line,_tmp491->f2=Cyc_Absyn_true_type,Cyc_Parse_parsing_tempest?_tmp491->f3=Cyc_Absyn_fat_bound_type:({void*_tmp92A=Cyc_yyget_YY2(&(yyyvsp[1]).v);_tmp491->f3=_tmp92A;});_tmp491;}));
goto _LL0;case 210:
# 1864 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_tmp492=_cycalloc(sizeof(*_tmp492));_tmp492->f1=(unsigned)((yyyvsp[0]).l).first_line,_tmp492->f2=Cyc_Absyn_false_type,({void*_tmp92B=Cyc_yyget_YY2(&(yyyvsp[1]).v);_tmp492->f3=_tmp92B;});_tmp492;}));
goto _LL0;case 211:
# 1865 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_tmp493=_cycalloc(sizeof(*_tmp493));_tmp493->f1=(unsigned)((yyyvsp[0]).l).first_line,_tmp493->f2=Cyc_Absyn_true_type,_tmp493->f3=Cyc_Absyn_fat_bound_type;_tmp493;}));
goto _LL0;case 212:
# 1868
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one());
goto _LL0;case 213:
# 1869 "parse.y"
 yyval=Cyc_YY2(Cyc_Absyn_thin_bounds_exp(Cyc_yyget_Exp_tok(&(yyyvsp[1]).v)));
goto _LL0;case 214:
# 1872
 yyval=Cyc_YY52(Cyc_Tcutil_any_bool(0));
goto _LL0;case 215:
# 1873 "parse.y"
 yyval=Cyc_YY52(Cyc_Absyn_true_type);
goto _LL0;case 216:
# 1874 "parse.y"
 yyval=Cyc_YY52(Cyc_Absyn_false_type);
goto _LL0;case 217:
# 1879 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_new_evar(& Cyc_Kinds_trko,0));
goto _LL0;case 218:
# 1880 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY45(&(yyyvsp[0]).v),& Cyc_Kinds_trk,1);yyval=(yyyvsp[0]).v;
goto _LL0;case 219:
# 1881 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_new_evar(& Cyc_Kinds_trko,0));
goto _LL0;case 220:
# 1885 "parse.y"
 yyval=Cyc_YY24(Cyc_Absyn_empty_tqual((unsigned)((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset + 1))).l).first_line));
goto _LL0;case 221:
# 1886 "parse.y"
 yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _tmp92C=Cyc_yyget_YY24(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp92C,Cyc_yyget_YY24(&(yyyvsp[1]).v));}));
goto _LL0;case 222:
# 1890 "parse.y"
 yyval=Cyc_YY40(({struct _tuple28*_tmp494=_cycalloc(sizeof(*_tmp494));_tmp494->f1=0,_tmp494->f2=0,_tmp494->f3=0,({void*_tmp92E=Cyc_yyget_YY50(&(yyyvsp[0]).v);_tmp494->f4=_tmp92E;}),({struct Cyc_List_List*_tmp92D=Cyc_yyget_YY51(&(yyyvsp[1]).v);_tmp494->f5=_tmp92D;});_tmp494;}));
goto _LL0;case 223:
# 1892 "parse.y"
 yyval=Cyc_YY40(({struct _tuple28*_tmp495=_cycalloc(sizeof(*_tmp495));({struct Cyc_List_List*_tmp932=({struct Cyc_List_List*(*_tmp931)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_tmp931(Cyc_yyget_YY39(&(yyyvsp[0]).v));});_tmp495->f1=_tmp932;}),_tmp495->f2=0,_tmp495->f3=0,({void*_tmp930=Cyc_yyget_YY50(&(yyyvsp[1]).v);_tmp495->f4=_tmp930;}),({struct Cyc_List_List*_tmp92F=Cyc_yyget_YY51(&(yyyvsp[2]).v);_tmp495->f5=_tmp92F;});_tmp495;}));
goto _LL0;case 224:
# 1894 "parse.y"
 yyval=Cyc_YY40(({struct _tuple28*_tmp496=_cycalloc(sizeof(*_tmp496));({struct Cyc_List_List*_tmp936=({struct Cyc_List_List*(*_tmp935)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_tmp935(Cyc_yyget_YY39(&(yyyvsp[0]).v));});_tmp496->f1=_tmp936;}),_tmp496->f2=1,_tmp496->f3=0,({void*_tmp934=Cyc_yyget_YY50(&(yyyvsp[3]).v);_tmp496->f4=_tmp934;}),({struct Cyc_List_List*_tmp933=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp496->f5=_tmp933;});_tmp496;}));
goto _LL0;case 225:  {
# 1897
struct _tuple8*_tmp497=Cyc_yyget_YY38(&(yyyvsp[2]).v);struct _tuple8*_stmttmp1C=_tmp497;struct _tuple8*_tmp498=_stmttmp1C;void*_tmp49B;struct Cyc_Absyn_Tqual _tmp49A;void*_tmp499;_tmp499=_tmp498->f1;_tmp49A=_tmp498->f2;_tmp49B=_tmp498->f3;{struct _fat_ptr*n=_tmp499;struct Cyc_Absyn_Tqual tq=_tmp49A;void*t=_tmp49B;
struct Cyc_Absyn_VarargInfo*_tmp49C=({struct Cyc_Absyn_VarargInfo*_tmp49E=_cycalloc(sizeof(*_tmp49E));_tmp49E->name=n,_tmp49E->tq=tq,_tmp49E->type=t,({int _tmp937=Cyc_yyget_YY32(&(yyyvsp[1]).v);_tmp49E->inject=_tmp937;});_tmp49E;});struct Cyc_Absyn_VarargInfo*v=_tmp49C;
yyval=Cyc_YY40(({struct _tuple28*_tmp49D=_cycalloc(sizeof(*_tmp49D));_tmp49D->f1=0,_tmp49D->f2=0,_tmp49D->f3=v,({void*_tmp939=Cyc_yyget_YY50(&(yyyvsp[3]).v);_tmp49D->f4=_tmp939;}),({struct Cyc_List_List*_tmp938=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp49D->f5=_tmp938;});_tmp49D;}));
# 1901
goto _LL0;}}case 226:  {
# 1903
struct _tuple8*_tmp49F=Cyc_yyget_YY38(&(yyyvsp[4]).v);struct _tuple8*_stmttmp1D=_tmp49F;struct _tuple8*_tmp4A0=_stmttmp1D;void*_tmp4A3;struct Cyc_Absyn_Tqual _tmp4A2;void*_tmp4A1;_tmp4A1=_tmp4A0->f1;_tmp4A2=_tmp4A0->f2;_tmp4A3=_tmp4A0->f3;{struct _fat_ptr*n=_tmp4A1;struct Cyc_Absyn_Tqual tq=_tmp4A2;void*t=_tmp4A3;
struct Cyc_Absyn_VarargInfo*_tmp4A4=({struct Cyc_Absyn_VarargInfo*_tmp4A6=_cycalloc(sizeof(*_tmp4A6));_tmp4A6->name=n,_tmp4A6->tq=tq,_tmp4A6->type=t,({int _tmp93A=Cyc_yyget_YY32(&(yyyvsp[3]).v);_tmp4A6->inject=_tmp93A;});_tmp4A6;});struct Cyc_Absyn_VarargInfo*v=_tmp4A4;
yyval=Cyc_YY40(({struct _tuple28*_tmp4A5=_cycalloc(sizeof(*_tmp4A5));({struct Cyc_List_List*_tmp93E=({struct Cyc_List_List*(*_tmp93D)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_tmp93D(Cyc_yyget_YY39(&(yyyvsp[0]).v));});_tmp4A5->f1=_tmp93E;}),_tmp4A5->f2=0,_tmp4A5->f3=v,({void*_tmp93C=Cyc_yyget_YY50(&(yyyvsp[5]).v);_tmp4A5->f4=_tmp93C;}),({struct Cyc_List_List*_tmp93B=Cyc_yyget_YY51(&(yyyvsp[6]).v);_tmp4A5->f5=_tmp93B;});_tmp4A5;}));
# 1907
goto _LL0;}}case 227:
# 1911 "parse.y"
 yyval=Cyc_YY45(({struct _fat_ptr _tmp93F=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Parse_id2type(_tmp93F,(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp4A7=_cycalloc(sizeof(*_tmp4A7));_tmp4A7->tag=1,_tmp4A7->f1=0;_tmp4A7;}));}));
goto _LL0;case 228:
# 1912 "parse.y"
 yyval=Cyc_YY45(({struct _fat_ptr _tmp940=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Parse_id2type(_tmp940,Cyc_Kinds_kind_to_bound(Cyc_yyget_YY44(&(yyyvsp[2]).v)));}));
goto _LL0;case 229:
# 1915
 yyval=Cyc_YY50(0);
goto _LL0;case 230:
# 1916 "parse.y"
 yyval=Cyc_YY50(Cyc_Absyn_join_eff(Cyc_yyget_YY41(&(yyyvsp[1]).v)));
goto _LL0;case 231:
# 1920 "parse.y"
 yyval=Cyc_YY51(0);
goto _LL0;case 232:
# 1921 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 233:  {
# 1929 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp4A8=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp4AC=_cycalloc(sizeof(*_tmp4AC));_tmp4AC->tag=2,_tmp4AC->f1=0,_tmp4AC->f2=& Cyc_Kinds_trk;_tmp4AC;});struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb=_tmp4A8;
void*_tmp4A9=({struct _fat_ptr _tmp941=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_tmp941,(void*)kb);});void*t=_tmp4A9;
yyval=Cyc_YY51(({struct Cyc_List_List*_tmp4AB=_cycalloc(sizeof(*_tmp4AB));({struct _tuple34*_tmp943=({struct _tuple34*_tmp4AA=_cycalloc(sizeof(*_tmp4AA));({void*_tmp942=Cyc_Absyn_join_eff(Cyc_yyget_YY41(&(yyyvsp[0]).v));_tmp4AA->f1=_tmp942;}),_tmp4AA->f2=t;_tmp4AA;});_tmp4AB->hd=_tmp943;}),_tmp4AB->tl=0;_tmp4AB;}));
# 1933
goto _LL0;}case 234:  {
# 1935 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp4AD=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp4B1=_cycalloc(sizeof(*_tmp4B1));_tmp4B1->tag=2,_tmp4B1->f1=0,_tmp4B1->f2=& Cyc_Kinds_trk;_tmp4B1;});struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb=_tmp4AD;
void*_tmp4AE=({struct _fat_ptr _tmp944=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_tmp944,(void*)kb);});void*t=_tmp4AE;
yyval=Cyc_YY51(({struct Cyc_List_List*_tmp4B0=_cycalloc(sizeof(*_tmp4B0));({struct _tuple34*_tmp947=({struct _tuple34*_tmp4AF=_cycalloc(sizeof(*_tmp4AF));({void*_tmp946=Cyc_Absyn_join_eff(Cyc_yyget_YY41(&(yyyvsp[0]).v));_tmp4AF->f1=_tmp946;}),_tmp4AF->f2=t;_tmp4AF;});_tmp4B0->hd=_tmp947;}),({struct Cyc_List_List*_tmp945=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp4B0->tl=_tmp945;});_tmp4B0;}));
# 1939
goto _LL0;}case 235:
# 1943 "parse.y"
 yyval=Cyc_YY32(0);
goto _LL0;case 236:
# 1945 "parse.y"
 if(({struct _fat_ptr _tmp948=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_zstrcmp(_tmp948,({const char*_tmp4B2="inject";_tag_fat(_tmp4B2,sizeof(char),7U);}));})!= 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6C7;_tmp6C7.tag=0,({struct _fat_ptr _tmp949=({const char*_tmp4B5="missing type in function declaration";_tag_fat(_tmp4B5,sizeof(char),37U);});_tmp6C7.f1=_tmp949;});_tmp6C7;});void*_tmp4B3[1];_tmp4B3[0]=& _tmp4B4;Cyc_Warn_err2((unsigned)((yyyvsp[0]).l).first_line,_tag_fat(_tmp4B3,sizeof(void*),1));});
yyval=Cyc_YY32(1);
# 1949
goto _LL0;case 237:
# 1952 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 238:
# 1953 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*(*_tmp94B)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append;});struct Cyc_List_List*_tmp94A=Cyc_yyget_YY41(&(yyyvsp[0]).v);_tmp94B(_tmp94A,Cyc_yyget_YY41(&(yyyvsp[2]).v));}));
goto _LL0;case 239:
# 1957 "parse.y"
 yyval=Cyc_YY41(0);
goto _LL0;case 240:
# 1958 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 241:
# 1959 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp4B6=_cycalloc(sizeof(*_tmp4B6));({void*_tmp94C=Cyc_Absyn_regionsof_eff(Cyc_yyget_YY45(&(yyyvsp[2]).v));_tmp4B6->hd=_tmp94C;}),_tmp4B6->tl=0;_tmp4B6;}));
goto _LL0;case 242:
# 1961 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY45(&(yyyvsp[0]).v),& Cyc_Kinds_ek,0);
yyval=Cyc_YY41(({struct Cyc_List_List*_tmp4B7=_cycalloc(sizeof(*_tmp4B7));({void*_tmp94D=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp4B7->hd=_tmp94D;}),_tmp4B7->tl=0;_tmp4B7;}));
# 1964
goto _LL0;case 243:
# 1969 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp4B8=_cycalloc(sizeof(*_tmp4B8));({void*_tmp94F=Cyc_Absyn_access_eff(({struct _tuple8*_tmp94E=Cyc_yyget_YY38(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmp94E,(unsigned)((yyyvsp[0]).l).first_line);}));_tmp4B8->hd=_tmp94F;}),_tmp4B8->tl=0;_tmp4B8;}));
goto _LL0;case 244:
# 1971 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp4B9=_cycalloc(sizeof(*_tmp4B9));({void*_tmp952=Cyc_Absyn_access_eff(({struct _tuple8*_tmp951=Cyc_yyget_YY38(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmp951,(unsigned)((yyyvsp[0]).l).first_line);}));_tmp4B9->hd=_tmp952;}),({struct Cyc_List_List*_tmp950=Cyc_yyget_YY41(&(yyyvsp[2]).v);_tmp4B9->tl=_tmp950;});_tmp4B9;}));
goto _LL0;case 245:
# 1976 "parse.y"
 yyval=Cyc_YY39(({struct Cyc_List_List*_tmp4BA=_cycalloc(sizeof(*_tmp4BA));({struct _tuple8*_tmp953=Cyc_yyget_YY38(&(yyyvsp[0]).v);_tmp4BA->hd=_tmp953;}),_tmp4BA->tl=0;_tmp4BA;}));
goto _LL0;case 246:
# 1977 "parse.y"
 yyval=Cyc_YY39(({struct Cyc_List_List*_tmp4BB=_cycalloc(sizeof(*_tmp4BB));({struct _tuple8*_tmp955=Cyc_yyget_YY38(&(yyyvsp[2]).v);_tmp4BB->hd=_tmp955;}),({struct Cyc_List_List*_tmp954=Cyc_yyget_YY39(&(yyyvsp[0]).v);_tmp4BB->tl=_tmp954;});_tmp4BB;}));
goto _LL0;case 247:  {
# 1983 "parse.y"
struct _tuple27 _tmp4BC=Cyc_yyget_YY36(&(yyyvsp[0]).v);struct _tuple27 _stmttmp1E=_tmp4BC;struct _tuple27 _tmp4BD=_stmttmp1E;void*_tmp4C0;struct Cyc_Parse_Type_specifier _tmp4BF;struct Cyc_Absyn_Tqual _tmp4BE;_tmp4BE=_tmp4BD.f1;_tmp4BF=_tmp4BD.f2;_tmp4C0=_tmp4BD.f3;{struct Cyc_Absyn_Tqual tq=_tmp4BE;struct Cyc_Parse_Type_specifier tspecs=_tmp4BF;struct Cyc_List_List*atts=_tmp4C0;
if(tq.loc == 0U)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
struct Cyc_Parse_Declarator _tmp4C1=Cyc_yyget_YY28(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator _stmttmp1F=_tmp4C1;struct Cyc_Parse_Declarator _tmp4C2=_stmttmp1F;void*_tmp4C5;unsigned _tmp4C4;void*_tmp4C3;_tmp4C3=_tmp4C2.id;_tmp4C4=_tmp4C2.varloc;_tmp4C5=_tmp4C2.tms;{struct _tuple0*qv=_tmp4C3;unsigned varloc=_tmp4C4;struct Cyc_List_List*tms=_tmp4C5;
void*_tmp4C6=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp4C6;
struct _tuple14 _tmp4C7=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 _stmttmp20=_tmp4C7;struct _tuple14 _tmp4C8=_stmttmp20;void*_tmp4CC;void*_tmp4CB;void*_tmp4CA;struct Cyc_Absyn_Tqual _tmp4C9;_tmp4C9=_tmp4C8.f1;_tmp4CA=_tmp4C8.f2;_tmp4CB=_tmp4C8.f3;_tmp4CC=_tmp4C8.f4;{struct Cyc_Absyn_Tqual tq2=_tmp4C9;void*t2=_tmp4CA;struct Cyc_List_List*tvs=_tmp4CB;struct Cyc_List_List*atts2=_tmp4CC;
if(tvs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4CE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6C8;_tmp6C8.tag=0,({struct _fat_ptr _tmp956=({const char*_tmp4CF="parameter with bad type params";_tag_fat(_tmp4CF,sizeof(char),31U);});_tmp6C8.f1=_tmp956;});_tmp6C8;});void*_tmp4CD[1];_tmp4CD[0]=& _tmp4CE;Cyc_Warn_err2((unsigned)((yyyvsp[1]).l).first_line,_tag_fat(_tmp4CD,sizeof(void*),1));});
if(Cyc_Absyn_is_qvar_qualified(qv))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4D1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6C9;_tmp6C9.tag=0,({struct _fat_ptr _tmp957=({const char*_tmp4D2="parameter cannot be qualified with a namespace";_tag_fat(_tmp4D2,sizeof(char),47U);});_tmp6C9.f1=_tmp957;});_tmp6C9;});void*_tmp4D0[1];_tmp4D0[0]=& _tmp4D1;Cyc_Warn_err2((unsigned)((yyyvsp[0]).l).first_line,_tag_fat(_tmp4D0,sizeof(void*),1));});{
struct _fat_ptr*idopt=(*qv).f2;
if(atts2 != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4D4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6CA;_tmp6CA.tag=0,({struct _fat_ptr _tmp958=({const char*_tmp4D5="extra attributes on parameter, ignoring";_tag_fat(_tmp4D5,sizeof(char),40U);});_tmp6CA.f1=_tmp958;});_tmp6CA;});void*_tmp4D3[1];_tmp4D3[0]=& _tmp4D4;Cyc_Warn_warn2((unsigned)((yyyvsp[0]).l).first_line,_tag_fat(_tmp4D3,sizeof(void*),1));});
yyval=Cyc_YY38(({struct _tuple8*_tmp4D6=_cycalloc(sizeof(*_tmp4D6));_tmp4D6->f1=idopt,_tmp4D6->f2=tq2,_tmp4D6->f3=t2;_tmp4D6;}));
# 1997
goto _LL0;}}}}}}case 248:  {
# 1998 "parse.y"
struct _tuple27 _tmp4D7=Cyc_yyget_YY36(&(yyyvsp[0]).v);struct _tuple27 _stmttmp21=_tmp4D7;struct _tuple27 _tmp4D8=_stmttmp21;void*_tmp4DB;struct Cyc_Parse_Type_specifier _tmp4DA;struct Cyc_Absyn_Tqual _tmp4D9;_tmp4D9=_tmp4D8.f1;_tmp4DA=_tmp4D8.f2;_tmp4DB=_tmp4D8.f3;{struct Cyc_Absyn_Tqual tq=_tmp4D9;struct Cyc_Parse_Type_specifier tspecs=_tmp4DA;struct Cyc_List_List*atts=_tmp4DB;
if(tq.loc == 0U)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
void*_tmp4DC=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp4DC;
if(atts != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4DE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6CB;_tmp6CB.tag=0,({struct _fat_ptr _tmp959=({const char*_tmp4DF="bad attributes on parameter, ignoring";_tag_fat(_tmp4DF,sizeof(char),38U);});_tmp6CB.f1=_tmp959;});_tmp6CB;});void*_tmp4DD[1];_tmp4DD[0]=& _tmp4DE;Cyc_Warn_warn2((unsigned)((yyyvsp[0]).l).first_line,_tag_fat(_tmp4DD,sizeof(void*),1));});
yyval=Cyc_YY38(({struct _tuple8*_tmp4E0=_cycalloc(sizeof(*_tmp4E0));_tmp4E0->f1=0,_tmp4E0->f2=tq,_tmp4E0->f3=t;_tmp4E0;}));
# 2005
goto _LL0;}}}case 249:  {
# 2006 "parse.y"
struct _tuple27 _tmp4E1=Cyc_yyget_YY36(&(yyyvsp[0]).v);struct _tuple27 _stmttmp22=_tmp4E1;struct _tuple27 _tmp4E2=_stmttmp22;void*_tmp4E5;struct Cyc_Parse_Type_specifier _tmp4E4;struct Cyc_Absyn_Tqual _tmp4E3;_tmp4E3=_tmp4E2.f1;_tmp4E4=_tmp4E2.f2;_tmp4E5=_tmp4E2.f3;{struct Cyc_Absyn_Tqual tq=_tmp4E3;struct Cyc_Parse_Type_specifier tspecs=_tmp4E4;struct Cyc_List_List*atts=_tmp4E5;
if(tq.loc == 0U)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
void*_tmp4E6=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp4E6;
struct Cyc_List_List*_tmp4E7=(Cyc_yyget_YY31(&(yyyvsp[1]).v)).tms;struct Cyc_List_List*tms=_tmp4E7;
struct _tuple14 _tmp4E8=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 _stmttmp23=_tmp4E8;struct _tuple14 _tmp4E9=_stmttmp23;void*_tmp4ED;void*_tmp4EC;void*_tmp4EB;struct Cyc_Absyn_Tqual _tmp4EA;_tmp4EA=_tmp4E9.f1;_tmp4EB=_tmp4E9.f2;_tmp4EC=_tmp4E9.f3;_tmp4ED=_tmp4E9.f4;{struct Cyc_Absyn_Tqual tq2=_tmp4EA;void*t2=_tmp4EB;struct Cyc_List_List*tvs=_tmp4EC;struct Cyc_List_List*atts2=_tmp4ED;
if(tvs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4EF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6CC;_tmp6CC.tag=0,({
struct _fat_ptr _tmp95A=({const char*_tmp4F0="bad type parameters on formal argument, ignoring";_tag_fat(_tmp4F0,sizeof(char),49U);});_tmp6CC.f1=_tmp95A;});_tmp6CC;});void*_tmp4EE[1];_tmp4EE[0]=& _tmp4EF;Cyc_Warn_warn2((unsigned)((yyyvsp[0]).l).first_line,_tag_fat(_tmp4EE,sizeof(void*),1));});
if(atts2 != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4F2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6CD;_tmp6CD.tag=0,({struct _fat_ptr _tmp95B=({const char*_tmp4F3="bad attributes on parameter, ignoring";_tag_fat(_tmp4F3,sizeof(char),38U);});_tmp6CD.f1=_tmp95B;});_tmp6CD;});void*_tmp4F1[1];_tmp4F1[0]=& _tmp4F2;Cyc_Warn_warn2((unsigned)((yyyvsp[0]).l).first_line,_tag_fat(_tmp4F1,sizeof(void*),1));});
yyval=Cyc_YY38(({struct _tuple8*_tmp4F4=_cycalloc(sizeof(*_tmp4F4));_tmp4F4->f1=0,_tmp4F4->f2=tq2,_tmp4F4->f3=t2;_tmp4F4;}));
# 2018
goto _LL0;}}}}case 250:
# 2021 "parse.y"
 yyval=Cyc_YY37(({struct Cyc_List_List*(*_tmp95C)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_tmp95C(Cyc_yyget_YY37(&(yyyvsp[0]).v));}));
goto _LL0;case 251:
# 2024
 yyval=Cyc_YY37(({struct Cyc_List_List*_tmp4F6=_cycalloc(sizeof(*_tmp4F6));({struct _fat_ptr*_tmp95E=({struct _fat_ptr*_tmp4F5=_cycalloc(sizeof(*_tmp4F5));({struct _fat_ptr _tmp95D=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp4F5=_tmp95D;});_tmp4F5;});_tmp4F6->hd=_tmp95E;}),_tmp4F6->tl=0;_tmp4F6;}));
goto _LL0;case 252:
# 2025 "parse.y"
 yyval=Cyc_YY37(({struct Cyc_List_List*_tmp4F8=_cycalloc(sizeof(*_tmp4F8));({struct _fat_ptr*_tmp961=({struct _fat_ptr*_tmp4F7=_cycalloc(sizeof(*_tmp4F7));({struct _fat_ptr _tmp960=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp4F7=_tmp960;});_tmp4F7;});_tmp4F8->hd=_tmp961;}),({struct Cyc_List_List*_tmp95F=Cyc_yyget_YY37(&(yyyvsp[0]).v);_tmp4F8->tl=_tmp95F;});_tmp4F8;}));
goto _LL0;case 253:
# 2029 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 254:
# 2030 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 255:
# 2035 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmp962=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp4F9=_cycalloc(sizeof(*_tmp4F9));_tmp4F9->tag=36,_tmp4F9->f1=0,_tmp4F9->f2=0;_tmp4F9;});Cyc_Absyn_new_exp(_tmp962,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 256:
# 2037 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmp965=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp4FA=_cycalloc(sizeof(*_tmp4FA));_tmp4FA->tag=36,_tmp4FA->f1=0,({struct Cyc_List_List*_tmp964=({struct Cyc_List_List*(*_tmp963)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_tmp963(Cyc_yyget_YY5(&(yyyvsp[1]).v));});_tmp4FA->f2=_tmp964;});_tmp4FA;});Cyc_Absyn_new_exp(_tmp965,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 257:
# 2039 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmp968=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp4FB=_cycalloc(sizeof(*_tmp4FB));_tmp4FB->tag=36,_tmp4FB->f1=0,({struct Cyc_List_List*_tmp967=({struct Cyc_List_List*(*_tmp966)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_tmp966(Cyc_yyget_YY5(&(yyyvsp[1]).v));});_tmp4FB->f2=_tmp967;});_tmp4FB;});Cyc_Absyn_new_exp(_tmp968,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 258:  {
# 2041 "parse.y"
struct Cyc_Absyn_Vardecl*_tmp4FC=({unsigned _tmp96D=(unsigned)((yyyvsp[2]).l).first_line;struct _tuple0*_tmp96C=({struct _tuple0*_tmp4FF=_cycalloc(sizeof(*_tmp4FF));_tmp4FF->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp96A=({struct _fat_ptr*_tmp4FE=_cycalloc(sizeof(*_tmp4FE));({struct _fat_ptr _tmp969=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp4FE=_tmp969;});_tmp4FE;});_tmp4FF->f2=_tmp96A;});_tmp4FF;});void*_tmp96B=Cyc_Absyn_uint_type;Cyc_Absyn_new_vardecl(_tmp96D,_tmp96C,_tmp96B,
Cyc_Absyn_uint_exp(0U,(unsigned)((yyyvsp[2]).l).first_line));});
# 2041
struct Cyc_Absyn_Vardecl*vd=_tmp4FC;
# 2044
(vd->tq).real_const=1;
yyval=Cyc_Exp_tok(({void*_tmp970=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp4FD=_cycalloc(sizeof(*_tmp4FD));_tmp4FD->tag=27,_tmp4FD->f1=vd,({struct Cyc_Absyn_Exp*_tmp96F=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);_tmp4FD->f2=_tmp96F;}),({struct Cyc_Absyn_Exp*_tmp96E=Cyc_yyget_Exp_tok(&(yyyvsp[6]).v);_tmp4FD->f3=_tmp96E;}),_tmp4FD->f4=0;_tmp4FD;});Cyc_Absyn_new_exp(_tmp970,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2047
goto _LL0;}case 259:  {
# 2049 "parse.y"
void*_tmp500=({struct _tuple8*_tmp971=Cyc_yyget_YY38(&(yyyvsp[6]).v);Cyc_Parse_type_name_to_type(_tmp971,(unsigned)((yyyvsp[6]).l).first_line);});void*t=_tmp500;
yyval=Cyc_Exp_tok(({void*_tmp973=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp501=_cycalloc(sizeof(*_tmp501));_tmp501->tag=28,({struct Cyc_Absyn_Exp*_tmp972=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);_tmp501->f1=_tmp972;}),_tmp501->f2=t,_tmp501->f3=0;_tmp501;});Cyc_Absyn_new_exp(_tmp973,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2052
goto _LL0;}case 260:
# 2057 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp503=_cycalloc(sizeof(*_tmp503));({struct _tuple35*_tmp975=({struct _tuple35*_tmp502=_cycalloc(sizeof(*_tmp502));_tmp502->f1=0,({struct Cyc_Absyn_Exp*_tmp974=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);_tmp502->f2=_tmp974;});_tmp502;});_tmp503->hd=_tmp975;}),_tmp503->tl=0;_tmp503;}));
goto _LL0;case 261:
# 2059 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp505=_cycalloc(sizeof(*_tmp505));({struct _tuple35*_tmp978=({struct _tuple35*_tmp504=_cycalloc(sizeof(*_tmp504));({struct Cyc_List_List*_tmp977=Cyc_yyget_YY42(&(yyyvsp[0]).v);_tmp504->f1=_tmp977;}),({struct Cyc_Absyn_Exp*_tmp976=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp504->f2=_tmp976;});_tmp504;});_tmp505->hd=_tmp978;}),_tmp505->tl=0;_tmp505;}));
goto _LL0;case 262:
# 2061 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp507=_cycalloc(sizeof(*_tmp507));({struct _tuple35*_tmp97B=({struct _tuple35*_tmp506=_cycalloc(sizeof(*_tmp506));_tmp506->f1=0,({struct Cyc_Absyn_Exp*_tmp97A=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp506->f2=_tmp97A;});_tmp506;});_tmp507->hd=_tmp97B;}),({struct Cyc_List_List*_tmp979=Cyc_yyget_YY5(&(yyyvsp[0]).v);_tmp507->tl=_tmp979;});_tmp507;}));
goto _LL0;case 263:
# 2063 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp509=_cycalloc(sizeof(*_tmp509));({struct _tuple35*_tmp97F=({struct _tuple35*_tmp508=_cycalloc(sizeof(*_tmp508));({struct Cyc_List_List*_tmp97E=Cyc_yyget_YY42(&(yyyvsp[2]).v);_tmp508->f1=_tmp97E;}),({struct Cyc_Absyn_Exp*_tmp97D=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp508->f2=_tmp97D;});_tmp508;});_tmp509->hd=_tmp97F;}),({struct Cyc_List_List*_tmp97C=Cyc_yyget_YY5(&(yyyvsp[0]).v);_tmp509->tl=_tmp97C;});_tmp509;}));
goto _LL0;case 264:
# 2067 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*(*_tmp980)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_tmp980(Cyc_yyget_YY42(&(yyyvsp[0]).v));}));
goto _LL0;case 265:
# 2068 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_tmp50C=_cycalloc(sizeof(*_tmp50C));({void*_tmp983=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp50B=_cycalloc(sizeof(*_tmp50B));_tmp50B->tag=1,({struct _fat_ptr*_tmp982=({struct _fat_ptr*_tmp50A=_cycalloc(sizeof(*_tmp50A));({struct _fat_ptr _tmp981=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp50A=_tmp981;});_tmp50A;});_tmp50B->f1=_tmp982;});_tmp50B;});_tmp50C->hd=_tmp983;}),_tmp50C->tl=0;_tmp50C;}));
goto _LL0;case 266:
# 2073 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_tmp50D=_cycalloc(sizeof(*_tmp50D));({void*_tmp984=Cyc_yyget_YY43(&(yyyvsp[0]).v);_tmp50D->hd=_tmp984;}),_tmp50D->tl=0;_tmp50D;}));
goto _LL0;case 267:
# 2074 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_tmp50E=_cycalloc(sizeof(*_tmp50E));({void*_tmp986=Cyc_yyget_YY43(&(yyyvsp[1]).v);_tmp50E->hd=_tmp986;}),({struct Cyc_List_List*_tmp985=Cyc_yyget_YY42(&(yyyvsp[0]).v);_tmp50E->tl=_tmp985;});_tmp50E;}));
goto _LL0;case 268:
# 2078 "parse.y"
 yyval=Cyc_YY43((void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp50F=_cycalloc(sizeof(*_tmp50F));_tmp50F->tag=0,({struct Cyc_Absyn_Exp*_tmp987=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp50F->f1=_tmp987;});_tmp50F;}));
goto _LL0;case 269:
# 2079 "parse.y"
 yyval=Cyc_YY43((void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp511=_cycalloc(sizeof(*_tmp511));_tmp511->tag=1,({struct _fat_ptr*_tmp989=({struct _fat_ptr*_tmp510=_cycalloc(sizeof(*_tmp510));({struct _fat_ptr _tmp988=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp510=_tmp988;});_tmp510;});_tmp511->f1=_tmp989;});_tmp511;}));
goto _LL0;case 270:  {
# 2084 "parse.y"
struct _tuple27 _tmp512=Cyc_yyget_YY36(&(yyyvsp[0]).v);struct _tuple27 _stmttmp24=_tmp512;struct _tuple27 _tmp513=_stmttmp24;void*_tmp516;struct Cyc_Parse_Type_specifier _tmp515;struct Cyc_Absyn_Tqual _tmp514;_tmp514=_tmp513.f1;_tmp515=_tmp513.f2;_tmp516=_tmp513.f3;{struct Cyc_Absyn_Tqual tq=_tmp514;struct Cyc_Parse_Type_specifier tss=_tmp515;struct Cyc_List_List*atts=_tmp516;
void*_tmp517=Cyc_Parse_speclist2typ(tss,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp517;
if(atts != 0)
({void*_tmp518=0U;({unsigned _tmp98B=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp98A=({const char*_tmp519="ignoring attributes in type";_tag_fat(_tmp519,sizeof(char),28U);});Cyc_Warn_warn(_tmp98B,_tmp98A,_tag_fat(_tmp518,sizeof(void*),0));});});
yyval=Cyc_YY38(({struct _tuple8*_tmp51A=_cycalloc(sizeof(*_tmp51A));_tmp51A->f1=0,_tmp51A->f2=tq,_tmp51A->f3=t;_tmp51A;}));
# 2090
goto _LL0;}}case 271:  {
# 2091 "parse.y"
struct _tuple27 _tmp51B=Cyc_yyget_YY36(&(yyyvsp[0]).v);struct _tuple27 _stmttmp25=_tmp51B;struct _tuple27 _tmp51C=_stmttmp25;void*_tmp51F;struct Cyc_Parse_Type_specifier _tmp51E;struct Cyc_Absyn_Tqual _tmp51D;_tmp51D=_tmp51C.f1;_tmp51E=_tmp51C.f2;_tmp51F=_tmp51C.f3;{struct Cyc_Absyn_Tqual tq=_tmp51D;struct Cyc_Parse_Type_specifier tss=_tmp51E;struct Cyc_List_List*atts=_tmp51F;
void*_tmp520=Cyc_Parse_speclist2typ(tss,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp520;
struct Cyc_List_List*_tmp521=(Cyc_yyget_YY31(&(yyyvsp[1]).v)).tms;struct Cyc_List_List*tms=_tmp521;
struct _tuple14 _tmp522=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 t_info=_tmp522;
if(t_info.f3 != 0)
({void*_tmp523=0U;({unsigned _tmp98D=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmp98C=({const char*_tmp524="bad type params, ignoring";_tag_fat(_tmp524,sizeof(char),26U);});Cyc_Warn_warn(_tmp98D,_tmp98C,_tag_fat(_tmp523,sizeof(void*),0));});});
if(t_info.f4 != 0)
({void*_tmp525=0U;({unsigned _tmp98F=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmp98E=({const char*_tmp526="bad specifiers, ignoring";_tag_fat(_tmp526,sizeof(char),25U);});Cyc_Warn_warn(_tmp98F,_tmp98E,_tag_fat(_tmp525,sizeof(void*),0));});});
yyval=Cyc_YY38(({struct _tuple8*_tmp527=_cycalloc(sizeof(*_tmp527));_tmp527->f1=0,_tmp527->f2=t_info.f1,_tmp527->f3=t_info.f2;_tmp527;}));
# 2101
goto _LL0;}}case 272:
# 2104 "parse.y"
 yyval=Cyc_YY45(({struct _tuple8*_tmp990=Cyc_yyget_YY38(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmp990,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 273:
# 2105 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_join_eff(0));
goto _LL0;case 274:
# 2106 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_join_eff(Cyc_yyget_YY41(&(yyyvsp[1]).v)));
goto _LL0;case 275:
# 2107 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_regionsof_eff(Cyc_yyget_YY45(&(yyyvsp[2]).v)));
goto _LL0;case 276:
# 2108 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmp528=_cycalloc(sizeof(*_tmp528));({void*_tmp992=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp528->hd=_tmp992;}),({struct Cyc_List_List*_tmp991=Cyc_yyget_YY41(&(yyyvsp[2]).v);_tmp528->tl=_tmp991;});_tmp528;})));
goto _LL0;case 277:
# 2114 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp529=_cycalloc(sizeof(*_tmp529));({void*_tmp993=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp529->hd=_tmp993;}),_tmp529->tl=0;_tmp529;}));
goto _LL0;case 278:
# 2115 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp52A=_cycalloc(sizeof(*_tmp52A));({void*_tmp995=Cyc_yyget_YY45(&(yyyvsp[2]).v);_tmp52A->hd=_tmp995;}),({struct Cyc_List_List*_tmp994=Cyc_yyget_YY41(&(yyyvsp[0]).v);_tmp52A->tl=_tmp994;});_tmp52A;}));
goto _LL0;case 279:
# 2119 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6CE;({struct Cyc_List_List*_tmp996=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp6CE.tms=_tmp996;});_tmp6CE;}));
goto _LL0;case 280:
# 2120 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 281:
# 2122 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6CF;({struct Cyc_List_List*_tmp999=({struct Cyc_List_List*(*_tmp998)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append;});struct Cyc_List_List*_tmp997=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp998(_tmp997,(Cyc_yyget_YY31(&(yyyvsp[1]).v)).tms);});_tmp6CF.tms=_tmp999;});_tmp6CF;}));
goto _LL0;case 282:
# 2127 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 283:
# 2129 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6D0;({struct Cyc_List_List*_tmp99C=({struct Cyc_List_List*_tmp52C=_region_malloc(yyr,sizeof(*_tmp52C));({void*_tmp99B=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp52B=_region_malloc(yyr,sizeof(*_tmp52B));_tmp52B->tag=0,({void*_tmp99A=Cyc_yyget_YY52(&(yyyvsp[2]).v);_tmp52B->f1=_tmp99A;}),_tmp52B->f2=(unsigned)((yyyvsp[2]).l).first_line;_tmp52B;});_tmp52C->hd=_tmp99B;}),_tmp52C->tl=0;_tmp52C;});_tmp6D0.tms=_tmp99C;});_tmp6D0;}));
goto _LL0;case 284:
# 2131 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6D1;({struct Cyc_List_List*_tmp9A0=({struct Cyc_List_List*_tmp52E=_region_malloc(yyr,sizeof(*_tmp52E));({void*_tmp99F=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp52D=_region_malloc(yyr,sizeof(*_tmp52D));_tmp52D->tag=0,({void*_tmp99E=Cyc_yyget_YY52(&(yyyvsp[3]).v);_tmp52D->f1=_tmp99E;}),_tmp52D->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp52D;});_tmp52E->hd=_tmp99F;}),({struct Cyc_List_List*_tmp99D=(Cyc_yyget_YY31(&(yyyvsp[0]).v)).tms;_tmp52E->tl=_tmp99D;});_tmp52E;});_tmp6D1.tms=_tmp9A0;});_tmp6D1;}));
goto _LL0;case 285:
# 2133 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6D2;({struct Cyc_List_List*_tmp9A4=({struct Cyc_List_List*_tmp530=_region_malloc(yyr,sizeof(*_tmp530));({void*_tmp9A3=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp52F=_region_malloc(yyr,sizeof(*_tmp52F));_tmp52F->tag=1,({struct Cyc_Absyn_Exp*_tmp9A2=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp52F->f1=_tmp9A2;}),({void*_tmp9A1=Cyc_yyget_YY52(&(yyyvsp[3]).v);_tmp52F->f2=_tmp9A1;}),_tmp52F->f3=(unsigned)((yyyvsp[3]).l).first_line;_tmp52F;});_tmp530->hd=_tmp9A3;}),_tmp530->tl=0;_tmp530;});_tmp6D2.tms=_tmp9A4;});_tmp6D2;}));
goto _LL0;case 286:
# 2135 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6D3;({struct Cyc_List_List*_tmp9A9=({struct Cyc_List_List*_tmp532=_region_malloc(yyr,sizeof(*_tmp532));({void*_tmp9A8=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp531=_region_malloc(yyr,sizeof(*_tmp531));_tmp531->tag=1,({struct Cyc_Absyn_Exp*_tmp9A7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp531->f1=_tmp9A7;}),({void*_tmp9A6=Cyc_yyget_YY52(&(yyyvsp[4]).v);_tmp531->f2=_tmp9A6;}),_tmp531->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp531;});_tmp532->hd=_tmp9A8;}),({
struct Cyc_List_List*_tmp9A5=(Cyc_yyget_YY31(&(yyyvsp[0]).v)).tms;_tmp532->tl=_tmp9A5;});_tmp532;});
# 2135
_tmp6D3.tms=_tmp9A9;});_tmp6D3;}));
# 2138
goto _LL0;case 287:  {
# 2139 "parse.y"
struct _tuple28*_tmp533=Cyc_yyget_YY40(&(yyyvsp[1]).v);struct _tuple28*_stmttmp26=_tmp533;struct _tuple28*_tmp534=_stmttmp26;void*_tmp539;void*_tmp538;void*_tmp537;int _tmp536;void*_tmp535;_tmp535=_tmp534->f1;_tmp536=_tmp534->f2;_tmp537=_tmp534->f3;_tmp538=_tmp534->f4;_tmp539=_tmp534->f5;{struct Cyc_List_List*lis=_tmp535;int b=_tmp536;struct Cyc_Absyn_VarargInfo*c=_tmp537;void*eff=_tmp538;struct Cyc_List_List*po=_tmp539;
yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6D4;({struct Cyc_List_List*_tmp9AE=({struct Cyc_List_List*_tmp53C=_region_malloc(yyr,sizeof(*_tmp53C));({void*_tmp9AD=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp53B=_region_malloc(yyr,sizeof(*_tmp53B));_tmp53B->tag=3,({void*_tmp9AC=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp53A=_region_malloc(yyr,sizeof(*_tmp53A));_tmp53A->tag=1,_tmp53A->f1=lis,_tmp53A->f2=b,_tmp53A->f3=c,_tmp53A->f4=eff,_tmp53A->f5=po,({struct Cyc_Absyn_Exp*_tmp9AB=(Cyc_yyget_YY59(&(yyyvsp[3]).v)).f1;_tmp53A->f6=_tmp9AB;}),({struct Cyc_Absyn_Exp*_tmp9AA=(Cyc_yyget_YY59(&(yyyvsp[3]).v)).f2;_tmp53A->f7=_tmp9AA;});_tmp53A;});_tmp53B->f1=_tmp9AC;});_tmp53B;});_tmp53C->hd=_tmp9AD;}),_tmp53C->tl=0;_tmp53C;});_tmp6D4.tms=_tmp9AE;});_tmp6D4;}));
# 2142
goto _LL0;}}case 288:  {
# 2143 "parse.y"
struct _tuple28*_tmp53D=Cyc_yyget_YY40(&(yyyvsp[2]).v);struct _tuple28*_stmttmp27=_tmp53D;struct _tuple28*_tmp53E=_stmttmp27;void*_tmp543;void*_tmp542;void*_tmp541;int _tmp540;void*_tmp53F;_tmp53F=_tmp53E->f1;_tmp540=_tmp53E->f2;_tmp541=_tmp53E->f3;_tmp542=_tmp53E->f4;_tmp543=_tmp53E->f5;{struct Cyc_List_List*lis=_tmp53F;int b=_tmp540;struct Cyc_Absyn_VarargInfo*c=_tmp541;void*eff=_tmp542;struct Cyc_List_List*po=_tmp543;
yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6D5;({struct Cyc_List_List*_tmp9B4=({struct Cyc_List_List*_tmp546=_region_malloc(yyr,sizeof(*_tmp546));({void*_tmp9B3=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp545=_region_malloc(yyr,sizeof(*_tmp545));_tmp545->tag=3,({void*_tmp9B2=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp544=_region_malloc(yyr,sizeof(*_tmp544));_tmp544->tag=1,_tmp544->f1=lis,_tmp544->f2=b,_tmp544->f3=c,_tmp544->f4=eff,_tmp544->f5=po,({
struct Cyc_Absyn_Exp*_tmp9B1=(Cyc_yyget_YY59(&(yyyvsp[4]).v)).f1;_tmp544->f6=_tmp9B1;}),({struct Cyc_Absyn_Exp*_tmp9B0=(Cyc_yyget_YY59(&(yyyvsp[4]).v)).f2;_tmp544->f7=_tmp9B0;});_tmp544;});
# 2144
_tmp545->f1=_tmp9B2;});_tmp545;});_tmp546->hd=_tmp9B3;}),({
struct Cyc_List_List*_tmp9AF=(Cyc_yyget_YY31(&(yyyvsp[0]).v)).tms;_tmp546->tl=_tmp9AF;});_tmp546;});
# 2144
_tmp6D5.tms=_tmp9B4;});_tmp6D5;}));
# 2147
goto _LL0;}}case 289:  {
# 2149
struct Cyc_List_List*_tmp547=({struct Cyc_List_List*(*_tmp9B7)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;});unsigned _tmp9B6=(unsigned)((yyyvsp[1]).l).first_line;_tmp9B7(Cyc_Parse_typ2tvar,_tmp9B6,({struct Cyc_List_List*(*_tmp9B5)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_tmp9B5(Cyc_yyget_YY41(&(yyyvsp[2]).v));}));});struct Cyc_List_List*ts=_tmp547;
yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6D6;({struct Cyc_List_List*_tmp9BA=({struct Cyc_List_List*_tmp549=_region_malloc(yyr,sizeof(*_tmp549));({void*_tmp9B9=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp548=_region_malloc(yyr,sizeof(*_tmp548));_tmp548->tag=4,_tmp548->f1=ts,_tmp548->f2=(unsigned)((yyyvsp[1]).l).first_line,_tmp548->f3=0;_tmp548;});_tmp549->hd=_tmp9B9;}),({
struct Cyc_List_List*_tmp9B8=(Cyc_yyget_YY31(&(yyyvsp[0]).v)).tms;_tmp549->tl=_tmp9B8;});_tmp549;});
# 2150
_tmp6D6.tms=_tmp9BA;});_tmp6D6;}));
# 2153
goto _LL0;}case 290:
# 2154 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6D7;({struct Cyc_List_List*_tmp9BE=({struct Cyc_List_List*_tmp54B=_region_malloc(yyr,sizeof(*_tmp54B));({void*_tmp9BD=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp54A=_region_malloc(yyr,sizeof(*_tmp54A));_tmp54A->tag=5,_tmp54A->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp9BC=Cyc_yyget_YY46(&(yyyvsp[1]).v);_tmp54A->f2=_tmp9BC;});_tmp54A;});_tmp54B->hd=_tmp9BD;}),({struct Cyc_List_List*_tmp9BB=(Cyc_yyget_YY31(&(yyyvsp[0]).v)).tms;_tmp54B->tl=_tmp9BB;});_tmp54B;});_tmp6D7.tms=_tmp9BE;});_tmp6D7;}));
# 2156
goto _LL0;case 291:
# 2159 "parse.y"
 yyval=Cyc_YY59(({struct _tuple17 _tmp6D8;_tmp6D8.f1=0,_tmp6D8.f2=0;_tmp6D8;}));
goto _LL0;case 292:
# 2160 "parse.y"
 yyval=Cyc_YY59(({struct _tuple17 _tmp6D9;({struct Cyc_Absyn_Exp*_tmp9BF=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp6D9.f1=_tmp9BF;}),_tmp6D9.f2=0;_tmp6D9;}));
goto _LL0;case 293:
# 2161 "parse.y"
 yyval=Cyc_YY59(({struct _tuple17 _tmp6DA;_tmp6DA.f1=0,({struct Cyc_Absyn_Exp*_tmp9C0=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp6DA.f2=_tmp9C0;});_tmp6DA;}));
goto _LL0;case 294:
# 2163 "parse.y"
 yyval=Cyc_YY59(({struct _tuple17 _tmp6DB;({struct Cyc_Absyn_Exp*_tmp9C2=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp6DB.f1=_tmp9C2;}),({struct Cyc_Absyn_Exp*_tmp9C1=Cyc_yyget_Exp_tok(&(yyyvsp[6]).v);_tmp6DB.f2=_tmp9C1;});_tmp6DB;}));
goto _LL0;case 295:
# 2167 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 296:
# 2168 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 297:
# 2169 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 298:
# 2170 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 299:
# 2171 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 300:
# 2172 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 301:
# 2178 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmp9C6=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp54D=_cycalloc(sizeof(*_tmp54D));_tmp54D->tag=13,({struct _fat_ptr*_tmp9C5=({struct _fat_ptr*_tmp54C=_cycalloc(sizeof(*_tmp54C));({struct _fat_ptr _tmp9C4=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp54C=_tmp9C4;});_tmp54C;});_tmp54D->f1=_tmp9C5;}),({struct Cyc_Absyn_Stmt*_tmp9C3=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);_tmp54D->f2=_tmp9C3;});_tmp54D;});Cyc_Absyn_new_stmt(_tmp9C6,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 302:
# 2182 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 303:
# 2183 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmp9C7=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_exp_stmt(_tmp9C7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 304:
# 2188 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 305:
# 2189 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 306:
# 2194 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmp9C8=Cyc_yyget_YY16(&(yyyvsp[0]).v);Cyc_Parse_flatten_declarations(_tmp9C8,Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));}));
goto _LL0;case 307:
# 2195 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmp9C9=Cyc_yyget_YY16(&(yyyvsp[0]).v);Cyc_Parse_flatten_declarations(_tmp9C9,Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v));}));
goto _LL0;case 308:
# 2197 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmp9CE=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp54F=_cycalloc(sizeof(*_tmp54F));_tmp54F->tag=13,({struct _fat_ptr*_tmp9CD=({struct _fat_ptr*_tmp54E=_cycalloc(sizeof(*_tmp54E));({struct _fat_ptr _tmp9CC=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp54E=_tmp9CC;});_tmp54E;});_tmp54F->f1=_tmp9CD;}),({struct Cyc_Absyn_Stmt*_tmp9CB=({struct Cyc_List_List*_tmp9CA=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmp9CA,Cyc_Absyn_skip_stmt(0U));});_tmp54F->f2=_tmp9CB;});_tmp54F;});Cyc_Absyn_new_stmt(_tmp9CE,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 309:
# 2199 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmp9D3=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp551=_cycalloc(sizeof(*_tmp551));_tmp551->tag=13,({struct _fat_ptr*_tmp9D2=({struct _fat_ptr*_tmp550=_cycalloc(sizeof(*_tmp550));({struct _fat_ptr _tmp9D1=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp550=_tmp9D1;});_tmp550;});_tmp551->f1=_tmp9D2;}),({struct Cyc_Absyn_Stmt*_tmp9D0=({struct Cyc_List_List*_tmp9CF=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmp9CF,Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v));});_tmp551->f2=_tmp9D0;});_tmp551;});Cyc_Absyn_new_stmt(_tmp9D3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 310:
# 2200 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 311:
# 2201 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmp9D5=Cyc_yyget_Stmt_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Stmt*_tmp9D4=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);Cyc_Absyn_seq_stmt(_tmp9D5,_tmp9D4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 312:
# 2202 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmp9D8=({void*_tmp9D7=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp552=_cycalloc(sizeof(*_tmp552));_tmp552->tag=1,({struct Cyc_Absyn_Fndecl*_tmp9D6=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp552->f1=_tmp9D6;});_tmp552;});Cyc_Absyn_new_decl(_tmp9D7,(unsigned)((yyyvsp[0]).l).first_line);});Cyc_Parse_flatten_decl(_tmp9D8,
Cyc_Absyn_skip_stmt(0U));}));
goto _LL0;case 313:
# 2205 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmp9DB=({void*_tmp9DA=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp553=_cycalloc(sizeof(*_tmp553));_tmp553->tag=1,({struct Cyc_Absyn_Fndecl*_tmp9D9=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp553->f1=_tmp9D9;});_tmp553;});Cyc_Absyn_new_decl(_tmp9DA,(unsigned)((yyyvsp[0]).l).first_line);});Cyc_Parse_flatten_decl(_tmp9DB,Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v));}));
goto _LL0;case 314:
# 2210 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmp9DE=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp9DD=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmp9DC=Cyc_Absyn_skip_stmt(0U);Cyc_Absyn_ifthenelse_stmt(_tmp9DE,_tmp9DD,_tmp9DC,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 315:
# 2212 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmp9E1=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp9E0=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmp9DF=Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);Cyc_Absyn_ifthenelse_stmt(_tmp9E1,_tmp9E0,_tmp9DF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 316:
# 2218 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmp9E3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_List_List*_tmp9E2=Cyc_yyget_YY8(&(yyyvsp[5]).v);Cyc_Absyn_switch_stmt(_tmp9E3,_tmp9E2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 317:
# 2221
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmp9E6=({struct _tuple0*_tmp9E4=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);Cyc_Absyn_unknownid_exp(_tmp9E4,(unsigned)((yyyvsp[1]).l).first_line);});struct Cyc_List_List*_tmp9E5=Cyc_yyget_YY8(&(yyyvsp[3]).v);Cyc_Absyn_switch_stmt(_tmp9E6,_tmp9E5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 318:
# 2224
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmp9E9=({struct Cyc_List_List*_tmp9E7=Cyc_yyget_YY4(&(yyyvsp[3]).v);Cyc_Absyn_tuple_exp(_tmp9E7,(unsigned)((yyyvsp[1]).l).first_line);});struct Cyc_List_List*_tmp9E8=Cyc_yyget_YY8(&(yyyvsp[6]).v);Cyc_Absyn_switch_stmt(_tmp9E9,_tmp9E8,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 319:
# 2228 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmp9EB=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp9EA=Cyc_yyget_YY8(&(yyyvsp[4]).v);Cyc_Absyn_trycatch_stmt(_tmp9EB,_tmp9EA,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 320:
# 2242 "parse.y"
 yyval=Cyc_YY8(0);
goto _LL0;case 321:
# 2245 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp555=_cycalloc(sizeof(*_tmp555));({struct Cyc_Absyn_Switch_clause*_tmp9EF=({struct Cyc_Absyn_Switch_clause*_tmp554=_cycalloc(sizeof(*_tmp554));({struct Cyc_Absyn_Pat*_tmp9EE=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[0]).l).first_line);_tmp554->pattern=_tmp9EE;}),_tmp554->pat_vars=0,_tmp554->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmp9ED=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);_tmp554->body=_tmp9ED;}),_tmp554->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp554;});
# 2245
_tmp555->hd=_tmp9EF;}),({
struct Cyc_List_List*_tmp9EC=Cyc_yyget_YY8(&(yyyvsp[3]).v);_tmp555->tl=_tmp9EC;});_tmp555;}));
goto _LL0;case 322:
# 2248 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp557=_cycalloc(sizeof(*_tmp557));({struct Cyc_Absyn_Switch_clause*_tmp9F3=({struct Cyc_Absyn_Switch_clause*_tmp556=_cycalloc(sizeof(*_tmp556));({struct Cyc_Absyn_Pat*_tmp9F2=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp556->pattern=_tmp9F2;}),_tmp556->pat_vars=0,_tmp556->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmp9F1=Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[2]).l).first_line);_tmp556->body=_tmp9F1;}),_tmp556->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp556;});
# 2248
_tmp557->hd=_tmp9F3;}),({
# 2250
struct Cyc_List_List*_tmp9F0=Cyc_yyget_YY8(&(yyyvsp[3]).v);_tmp557->tl=_tmp9F0;});_tmp557;}));
goto _LL0;case 323:
# 2252 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp559=_cycalloc(sizeof(*_tmp559));({struct Cyc_Absyn_Switch_clause*_tmp9F7=({struct Cyc_Absyn_Switch_clause*_tmp558=_cycalloc(sizeof(*_tmp558));({struct Cyc_Absyn_Pat*_tmp9F6=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp558->pattern=_tmp9F6;}),_tmp558->pat_vars=0,_tmp558->where_clause=0,({struct Cyc_Absyn_Stmt*_tmp9F5=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);_tmp558->body=_tmp9F5;}),_tmp558->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp558;});_tmp559->hd=_tmp9F7;}),({struct Cyc_List_List*_tmp9F4=Cyc_yyget_YY8(&(yyyvsp[4]).v);_tmp559->tl=_tmp9F4;});_tmp559;}));
goto _LL0;case 324:
# 2254 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp55B=_cycalloc(sizeof(*_tmp55B));({struct Cyc_Absyn_Switch_clause*_tmp9FC=({struct Cyc_Absyn_Switch_clause*_tmp55A=_cycalloc(sizeof(*_tmp55A));({struct Cyc_Absyn_Pat*_tmp9FB=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp55A->pattern=_tmp9FB;}),_tmp55A->pat_vars=0,({struct Cyc_Absyn_Exp*_tmp9FA=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp55A->where_clause=_tmp9FA;}),({
struct Cyc_Absyn_Stmt*_tmp9F9=Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[4]).l).first_line);_tmp55A->body=_tmp9F9;}),_tmp55A->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp55A;});
# 2254
_tmp55B->hd=_tmp9FC;}),({
# 2256
struct Cyc_List_List*_tmp9F8=Cyc_yyget_YY8(&(yyyvsp[5]).v);_tmp55B->tl=_tmp9F8;});_tmp55B;}));
goto _LL0;case 325:
# 2258 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp55D=_cycalloc(sizeof(*_tmp55D));({struct Cyc_Absyn_Switch_clause*_tmpA01=({struct Cyc_Absyn_Switch_clause*_tmp55C=_cycalloc(sizeof(*_tmp55C));({struct Cyc_Absyn_Pat*_tmpA00=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp55C->pattern=_tmpA00;}),_tmp55C->pat_vars=0,({struct Cyc_Absyn_Exp*_tmp9FF=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp55C->where_clause=_tmp9FF;}),({struct Cyc_Absyn_Stmt*_tmp9FE=Cyc_yyget_Stmt_tok(&(yyyvsp[5]).v);_tmp55C->body=_tmp9FE;}),_tmp55C->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp55C;});_tmp55D->hd=_tmpA01;}),({struct Cyc_List_List*_tmp9FD=Cyc_yyget_YY8(&(yyyvsp[6]).v);_tmp55D->tl=_tmp9FD;});_tmp55D;}));
goto _LL0;case 326:
# 2265 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA03=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmpA02=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);Cyc_Absyn_while_stmt(_tmpA03,_tmpA02,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 327:
# 2267 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpA05=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpA04=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_do_stmt(_tmpA05,_tmpA04,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 328:
# 2269 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA09=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpA08=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*_tmpA07=Cyc_yyget_Exp_tok(&(yyyvsp[6]).v);struct Cyc_Absyn_Stmt*_tmpA06=Cyc_yyget_Stmt_tok(&(yyyvsp[8]).v);Cyc_Absyn_for_stmt(_tmpA09,_tmpA08,_tmpA07,_tmpA06,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 329:  {
# 2271 "parse.y"
struct Cyc_Absyn_Stmt*_tmp55E=({struct Cyc_Absyn_Exp*_tmpA0D=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpA0C=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpA0B=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Stmt*_tmpA0A=Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);Cyc_Absyn_for_stmt(_tmpA0D,_tmpA0C,_tmpA0B,_tmpA0A,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp55E;
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpA0E=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpA0E,s);}));
goto _LL0;}case 330:
# 2275
 yyval=Cyc_Exp_tok(Cyc_Absyn_true_exp(0U));
goto _LL0;case 331:
# 2276 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 332:
# 2279
 yyval=Cyc_Stmt_tok(({struct _fat_ptr*_tmpA10=({struct _fat_ptr*_tmp55F=_cycalloc(sizeof(*_tmp55F));({struct _fat_ptr _tmpA0F=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp55F=_tmpA0F;});_tmp55F;});Cyc_Absyn_goto_stmt(_tmpA10,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 333:
# 2280 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 334:
# 2281 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_break_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 335:
# 2282 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 336:
# 2283 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA11=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_return_stmt(_tmpA11,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 337:
# 2285 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 338:
# 2286 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 339:
# 2288 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpA12=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_fallthru_stmt(_tmpA12,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 340:
# 2297 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 341:
# 2300
 yyval=(yyyvsp[0]).v;
goto _LL0;case 342:
# 2302 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpA15=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA14=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpA13=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_conditional_exp(_tmpA15,_tmpA14,_tmpA13,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 343:
# 2305
 yyval=(yyyvsp[0]).v;
goto _LL0;case 344:
# 2307 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpA17=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA16=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_or_exp(_tmpA17,_tmpA16,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 345:
# 2310
 yyval=(yyyvsp[0]).v;
goto _LL0;case 346:
# 2312 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpA19=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA18=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_and_exp(_tmpA19,_tmpA18,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 347:
# 2315
 yyval=(yyyvsp[0]).v;
goto _LL0;case 348:
# 2317 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpA1B=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA1A=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(14U,_tmpA1B,_tmpA1A,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 349:
# 2320
 yyval=(yyyvsp[0]).v;
goto _LL0;case 350:
# 2322 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpA1D=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA1C=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(15U,_tmpA1D,_tmpA1C,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 351:
# 2325
 yyval=(yyyvsp[0]).v;
goto _LL0;case 352:
# 2327 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpA1F=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA1E=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(13U,_tmpA1F,_tmpA1E,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 353:
# 2330
 yyval=(yyyvsp[0]).v;
goto _LL0;case 354:
# 2332 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*(*_tmpA22)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY66(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpA21=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA20=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmpA22(_tmpA21,_tmpA20,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 355:
# 2335
 yyval=(yyyvsp[0]).v;
goto _LL0;case 356:
# 2337 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*(*_tmpA25)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY66(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpA24=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA23=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmpA25(_tmpA24,_tmpA23,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 357:
# 2340
 yyval=(yyyvsp[0]).v;
goto _LL0;case 358:
# 2342 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpA27=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA26=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(16U,_tmpA27,_tmpA26,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 359:
# 2344 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpA29=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA28=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(17U,_tmpA29,_tmpA28,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 360:
# 2347
 yyval=(yyyvsp[0]).v;
goto _LL0;case 361:
# 2349 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _tmpA2C=Cyc_yyget_YY6(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpA2B=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA2A=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(_tmpA2C,_tmpA2B,_tmpA2A,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 362:
# 2352
 yyval=(yyyvsp[0]).v;
goto _LL0;case 363:
# 2354 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _tmpA2F=Cyc_yyget_YY6(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpA2E=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA2D=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(_tmpA2F,_tmpA2E,_tmpA2D,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 364:
# 2357
 yyval=(yyyvsp[0]).v;
goto _LL0;case 365:  {
# 2359 "parse.y"
void*_tmp560=({struct _tuple8*_tmpA30=Cyc_yyget_YY38(&(yyyvsp[1]).v);Cyc_Parse_type_name_to_type(_tmpA30,(unsigned)((yyyvsp[1]).l).first_line);});void*t=_tmp560;
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpA32=t;struct Cyc_Absyn_Exp*_tmpA31=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_cast_exp(_tmpA32,_tmpA31,1,0U,(unsigned)((yyyvsp[0]).l).first_line);})));
# 2362
goto _LL0;}case 366:
# 2365 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 367:
# 2369 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _tmpA34=Cyc_yyget_YY6(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpA33=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(_tmpA34,_tmpA33,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 368:  {
# 2371 "parse.y"
void*_tmp561=({struct _tuple8*_tmpA35=Cyc_yyget_YY38(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpA35,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp561;
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftype_exp(t,(unsigned)((yyyvsp[0]).l).first_line)));
goto _LL0;}case 369:
# 2374 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpA36=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_sizeofexp_exp(_tmpA36,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 370:
# 2376 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpA39=(*Cyc_yyget_YY38(&(yyyvsp[2]).v)).f3;struct Cyc_List_List*_tmpA38=({struct Cyc_List_List*(*_tmpA37)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_tmpA37(Cyc_yyget_YY3(&(yyyvsp[4]).v));});Cyc_Absyn_offsetof_exp(_tmpA39,_tmpA38,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 371:
# 2381 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 372:
# 2389 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 373:
# 2393 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 374:
# 2394 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(&(yyyvsp[1]).v)));
goto _LL0;case 375:
# 2395 "parse.y"
 yyval=Cyc_YY9(({void*_tmpA3B=(void*)({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp562=_cycalloc(sizeof(*_tmp562));_tmp562->tag=15,({struct _tuple0*_tmpA3A=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp562->f1=_tmpA3A;});_tmp562;});Cyc_Absyn_new_pat(_tmpA3B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 376:
# 2396 "parse.y"
 yyval=Cyc_YY9(({void*_tmpA3D=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp563=_cycalloc(sizeof(*_tmp563));_tmp563->tag=6,({struct Cyc_Absyn_Pat*_tmpA3C=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp563->f1=_tmpA3C;});_tmp563;});Cyc_Absyn_new_pat(_tmpA3D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 377:  {
# 2398 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);
{void*_tmp564=e->r;void*_stmttmp28=_tmp564;void*_tmp565=_stmttmp28;struct _fat_ptr _tmp566;int _tmp567;short _tmp569;char _tmp56A;enum Cyc_Absyn_Sign _tmp568;if(*((int*)_tmp565)== 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp565)->f1).LongLong_c).tag){case 2: _tmp568=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp565)->f1).Char_c).val).f1;_tmp56A=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp565)->f1).Char_c).val).f2;{enum Cyc_Absyn_Sign s=_tmp568;char i=_tmp56A;
# 2402
yyval=Cyc_YY9(({void*_tmpA3E=(void*)({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp56B=_cycalloc(sizeof(*_tmp56B));_tmp56B->tag=11,_tmp56B->f1=i;_tmp56B;});Cyc_Absyn_new_pat(_tmpA3E,e->loc);}));goto _LL47E;}case 4: _tmp568=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp565)->f1).Short_c).val).f1;_tmp569=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp565)->f1).Short_c).val).f2;{enum Cyc_Absyn_Sign s=_tmp568;short i=_tmp569;
# 2404
yyval=Cyc_YY9(({void*_tmpA3F=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp56C=_cycalloc(sizeof(*_tmp56C));_tmp56C->tag=10,_tmp56C->f1=s,_tmp56C->f2=(int)i;_tmp56C;});Cyc_Absyn_new_pat(_tmpA3F,e->loc);}));goto _LL47E;}case 5: _tmp568=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp565)->f1).Int_c).val).f1;_tmp567=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp565)->f1).Int_c).val).f2;{enum Cyc_Absyn_Sign s=_tmp568;int i=_tmp567;
# 2406
yyval=Cyc_YY9(({void*_tmpA40=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp56D=_cycalloc(sizeof(*_tmp56D));_tmp56D->tag=10,_tmp56D->f1=s,_tmp56D->f2=i;_tmp56D;});Cyc_Absyn_new_pat(_tmpA40,e->loc);}));goto _LL47E;}case 7: _tmp566=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp565)->f1).Float_c).val).f1;_tmp567=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp565)->f1).Float_c).val).f2;{struct _fat_ptr s=_tmp566;int i=_tmp567;
# 2408
yyval=Cyc_YY9(({void*_tmpA41=(void*)({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp56E=_cycalloc(sizeof(*_tmp56E));_tmp56E->tag=12,_tmp56E->f1=s,_tmp56E->f2=i;_tmp56E;});Cyc_Absyn_new_pat(_tmpA41,e->loc);}));goto _LL47E;}case 1:
# 2410
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL47E;case 8:
 goto _LL48C;case 9: _LL48C:
# 2413
({void*_tmp56F=0U;({unsigned _tmpA43=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpA42=({const char*_tmp570="strings cannot occur within patterns";_tag_fat(_tmp570,sizeof(char),37U);});Cyc_Warn_err(_tmpA43,_tmpA42,_tag_fat(_tmp56F,sizeof(void*),0));});});goto _LL47E;case 6:
# 2415
({void*_tmp571=0U;({unsigned _tmpA45=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpA44=({const char*_tmp572="long long's in patterns not yet implemented";_tag_fat(_tmp572,sizeof(char),44U);});Cyc_Warn_err(_tmpA45,_tmpA44,_tag_fat(_tmp571,sizeof(void*),0));});});goto _LL47E;default: goto _LL48F;}else{_LL48F:
# 2417
({void*_tmp573=0U;({unsigned _tmpA47=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpA46=({const char*_tmp574="bad constant in case";_tag_fat(_tmp574,sizeof(char),21U);});Cyc_Warn_err(_tmpA47,_tmpA46,_tag_fat(_tmp573,sizeof(void*),0));});});}_LL47E:;}
# 2420
goto _LL0;}case 378:
# 2421 "parse.y"
 if(({struct _fat_ptr _tmpA48=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmpA48,({const char*_tmp575="as";_tag_fat(_tmp575,sizeof(char),3U);}));})!= 0)
({void*_tmp576=0U;({unsigned _tmpA4A=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpA49=({const char*_tmp577="expecting `as'";_tag_fat(_tmp577,sizeof(char),15U);});Cyc_Warn_err(_tmpA4A,_tmpA49,_tag_fat(_tmp576,sizeof(void*),0));});});
yyval=Cyc_YY9(({void*_tmpA51=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp57A=_cycalloc(sizeof(*_tmp57A));_tmp57A->tag=1,({struct Cyc_Absyn_Vardecl*_tmpA50=({unsigned _tmpA4F=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpA4E=({struct _tuple0*_tmp579=_cycalloc(sizeof(*_tmp579));_tmp579->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpA4D=({struct _fat_ptr*_tmp578=_cycalloc(sizeof(*_tmp578));({struct _fat_ptr _tmpA4C=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp578=_tmpA4C;});_tmp578;});_tmp579->f2=_tmpA4D;});_tmp579;});Cyc_Absyn_new_vardecl(_tmpA4F,_tmpA4E,Cyc_Absyn_void_type,0);});_tmp57A->f1=_tmpA50;}),({
struct Cyc_Absyn_Pat*_tmpA4B=Cyc_yyget_YY9(&(yyyvsp[2]).v);_tmp57A->f2=_tmpA4B;});_tmp57A;});
# 2423
Cyc_Absyn_new_pat(_tmpA51,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2426
goto _LL0;case 379:
# 2427 "parse.y"
 if(({struct _fat_ptr _tmpA52=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_strcmp(_tmpA52,({const char*_tmp57B="alias";_tag_fat(_tmp57B,sizeof(char),6U);}));})!= 0)
({void*_tmp57C=0U;({unsigned _tmpA54=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpA53=({const char*_tmp57D="expecting `alias'";_tag_fat(_tmp57D,sizeof(char),18U);});Cyc_Warn_err(_tmpA54,_tmpA53,_tag_fat(_tmp57C,sizeof(void*),0));});});{
int _tmp57E=((yyyvsp[0]).l).first_line;int location=_tmp57E;
({struct _fat_ptr _tmpA55=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_tvar_ok(_tmpA55,(unsigned)location);});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp584=_cycalloc(sizeof(*_tmp584));({struct _fat_ptr*_tmpA58=({struct _fat_ptr*_tmp582=_cycalloc(sizeof(*_tmp582));({struct _fat_ptr _tmpA57=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp582=_tmpA57;});_tmp582;});_tmp584->name=_tmpA58;}),_tmp584->identity=-1,({void*_tmpA56=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp583=_cycalloc(sizeof(*_tmp583));_tmp583->tag=0,_tmp583->f1=& Cyc_Kinds_rk;_tmp583;});_tmp584->kind=_tmpA56;});_tmp584;});
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmpA5D=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpA5C=({struct _tuple0*_tmp581=_cycalloc(sizeof(*_tmp581));_tmp581->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpA5A=({struct _fat_ptr*_tmp580=_cycalloc(sizeof(*_tmp580));({struct _fat_ptr _tmpA59=Cyc_yyget_String_tok(&(yyyvsp[5]).v);*_tmp580=_tmpA59;});_tmp580;});_tmp581->f2=_tmpA5A;});_tmp581;});Cyc_Absyn_new_vardecl(_tmpA5D,_tmpA5C,({
struct _tuple8*_tmpA5B=Cyc_yyget_YY38(&(yyyvsp[4]).v);Cyc_Parse_type_name_to_type(_tmpA5B,(unsigned)((yyyvsp[4]).l).first_line);}),0);});
yyval=Cyc_YY9(({void*_tmpA5E=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp57F=_cycalloc(sizeof(*_tmp57F));_tmp57F->tag=2,_tmp57F->f1=tv,_tmp57F->f2=vd;_tmp57F;});Cyc_Absyn_new_pat(_tmpA5E,(unsigned)location);}));
# 2436
goto _LL0;}}case 380:
# 2437 "parse.y"
 if(({struct _fat_ptr _tmpA5F=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_strcmp(_tmpA5F,({const char*_tmp585="alias";_tag_fat(_tmp585,sizeof(char),6U);}));})!= 0)
({void*_tmp586=0U;({unsigned _tmpA61=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpA60=({const char*_tmp587="expecting `alias'";_tag_fat(_tmp587,sizeof(char),18U);});Cyc_Warn_err(_tmpA61,_tmpA60,_tag_fat(_tmp586,sizeof(void*),0));});});{
int _tmp588=((yyyvsp[0]).l).first_line;int location=_tmp588;
({struct _fat_ptr _tmpA62=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_tvar_ok(_tmpA62,(unsigned)location);});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp58E=_cycalloc(sizeof(*_tmp58E));({struct _fat_ptr*_tmpA65=({struct _fat_ptr*_tmp58C=_cycalloc(sizeof(*_tmp58C));({struct _fat_ptr _tmpA64=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp58C=_tmpA64;});_tmp58C;});_tmp58E->name=_tmpA65;}),_tmp58E->identity=-1,({void*_tmpA63=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp58D=_cycalloc(sizeof(*_tmp58D));_tmp58D->tag=0,_tmp58D->f1=& Cyc_Kinds_rk;_tmp58D;});_tmp58E->kind=_tmpA63;});_tmp58E;});
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmpA6A=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpA69=({struct _tuple0*_tmp58B=_cycalloc(sizeof(*_tmp58B));_tmp58B->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpA67=({struct _fat_ptr*_tmp58A=_cycalloc(sizeof(*_tmp58A));({struct _fat_ptr _tmpA66=Cyc_yyget_String_tok(&(yyyvsp[5]).v);*_tmp58A=_tmpA66;});_tmp58A;});_tmp58B->f2=_tmpA67;});_tmp58B;});Cyc_Absyn_new_vardecl(_tmpA6A,_tmpA69,({
struct _tuple8*_tmpA68=Cyc_yyget_YY38(&(yyyvsp[4]).v);Cyc_Parse_type_name_to_type(_tmpA68,(unsigned)((yyyvsp[4]).l).first_line);}),0);});
yyval=Cyc_YY9(({void*_tmpA6B=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp589=_cycalloc(sizeof(*_tmp589));_tmp589->tag=2,_tmp589->f1=tv,_tmp589->f2=vd;_tmp589;});Cyc_Absyn_new_pat(_tmpA6B,(unsigned)location);}));
# 2446
goto _LL0;}}case 381:  {
# 2447 "parse.y"
struct _tuple23 _tmp58F=*Cyc_yyget_YY10(&(yyyvsp[2]).v);struct _tuple23 _stmttmp29=_tmp58F;struct _tuple23 _tmp590=_stmttmp29;int _tmp592;void*_tmp591;_tmp591=_tmp590.f1;_tmp592=_tmp590.f2;{struct Cyc_List_List*ps=_tmp591;int dots=_tmp592;
yyval=Cyc_YY9(({void*_tmpA6C=(void*)({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp593=_cycalloc(sizeof(*_tmp593));_tmp593->tag=5,_tmp593->f1=ps,_tmp593->f2=dots;_tmp593;});Cyc_Absyn_new_pat(_tmpA6C,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2450
goto _LL0;}}case 382:  {
# 2451 "parse.y"
struct _tuple23 _tmp594=*Cyc_yyget_YY10(&(yyyvsp[2]).v);struct _tuple23 _stmttmp2A=_tmp594;struct _tuple23 _tmp595=_stmttmp2A;int _tmp597;void*_tmp596;_tmp596=_tmp595.f1;_tmp597=_tmp595.f2;{struct Cyc_List_List*ps=_tmp596;int dots=_tmp597;
yyval=Cyc_YY9(({void*_tmpA6E=(void*)({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp598=_cycalloc(sizeof(*_tmp598));_tmp598->tag=16,({struct _tuple0*_tmpA6D=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp598->f1=_tmpA6D;}),_tmp598->f2=ps,_tmp598->f3=dots;_tmp598;});Cyc_Absyn_new_pat(_tmpA6E,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2454
goto _LL0;}}case 383:  {
# 2455 "parse.y"
struct _tuple23 _tmp599=*Cyc_yyget_YY14(&(yyyvsp[3]).v);struct _tuple23 _stmttmp2B=_tmp599;struct _tuple23 _tmp59A=_stmttmp2B;int _tmp59C;void*_tmp59B;_tmp59B=_tmp59A.f1;_tmp59C=_tmp59A.f2;{struct Cyc_List_List*fps=_tmp59B;int dots=_tmp59C;
struct Cyc_List_List*_tmp59D=({struct Cyc_List_List*(*_tmpA70)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;});unsigned _tmpA6F=(unsigned)((yyyvsp[2]).l).first_line;_tmpA70(Cyc_Parse_typ2tvar,_tmpA6F,Cyc_yyget_YY41(&(yyyvsp[2]).v));});struct Cyc_List_List*exist_ts=_tmp59D;
yyval=Cyc_YY9(({void*_tmpA73=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp59F=_cycalloc(sizeof(*_tmp59F));_tmp59F->tag=7,({union Cyc_Absyn_AggrInfo*_tmpA72=({union Cyc_Absyn_AggrInfo*_tmp59E=_cycalloc(sizeof(*_tmp59E));({union Cyc_Absyn_AggrInfo _tmpA71=Cyc_Absyn_UnknownAggr(0U,Cyc_yyget_QualId_tok(&(yyyvsp[0]).v),0);*_tmp59E=_tmpA71;});_tmp59E;});_tmp59F->f1=_tmpA72;}),_tmp59F->f2=exist_ts,_tmp59F->f3=fps,_tmp59F->f4=dots;_tmp59F;});Cyc_Absyn_new_pat(_tmpA73,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2460
goto _LL0;}}case 384:  {
# 2461 "parse.y"
struct _tuple23 _tmp5A0=*Cyc_yyget_YY14(&(yyyvsp[2]).v);struct _tuple23 _stmttmp2C=_tmp5A0;struct _tuple23 _tmp5A1=_stmttmp2C;int _tmp5A3;void*_tmp5A2;_tmp5A2=_tmp5A1.f1;_tmp5A3=_tmp5A1.f2;{struct Cyc_List_List*fps=_tmp5A2;int dots=_tmp5A3;
struct Cyc_List_List*_tmp5A4=({struct Cyc_List_List*(*_tmpA75)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;});unsigned _tmpA74=(unsigned)((yyyvsp[1]).l).first_line;_tmpA75(Cyc_Parse_typ2tvar,_tmpA74,Cyc_yyget_YY41(&(yyyvsp[1]).v));});struct Cyc_List_List*exist_ts=_tmp5A4;
yyval=Cyc_YY9(({void*_tmpA76=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp5A5=_cycalloc(sizeof(*_tmp5A5));_tmp5A5->tag=7,_tmp5A5->f1=0,_tmp5A5->f2=exist_ts,_tmp5A5->f3=fps,_tmp5A5->f4=dots;_tmp5A5;});Cyc_Absyn_new_pat(_tmpA76,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2465
goto _LL0;}}case 385:
# 2466 "parse.y"
 yyval=Cyc_YY9(({void*_tmpA7A=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp5A7=_cycalloc(sizeof(*_tmp5A7));_tmp5A7->tag=6,({struct Cyc_Absyn_Pat*_tmpA79=({void*_tmpA78=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp5A6=_cycalloc(sizeof(*_tmp5A6));_tmp5A6->tag=6,({struct Cyc_Absyn_Pat*_tmpA77=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5A6->f1=_tmpA77;});_tmp5A6;});Cyc_Absyn_new_pat(_tmpA78,(unsigned)((yyyvsp[0]).l).first_line);});_tmp5A7->f1=_tmpA79;});_tmp5A7;});Cyc_Absyn_new_pat(_tmpA7A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 386:
# 2468 "parse.y"
 yyval=Cyc_YY9(({void*_tmpA81=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp5AA=_cycalloc(sizeof(*_tmp5AA));_tmp5AA->tag=3,({struct Cyc_Absyn_Vardecl*_tmpA80=({unsigned _tmpA7F=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpA7E=({struct _tuple0*_tmp5A9=_cycalloc(sizeof(*_tmp5A9));_tmp5A9->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpA7D=({struct _fat_ptr*_tmp5A8=_cycalloc(sizeof(*_tmp5A8));({struct _fat_ptr _tmpA7C=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp5A8=_tmpA7C;});_tmp5A8;});_tmp5A9->f2=_tmpA7D;});_tmp5A9;});Cyc_Absyn_new_vardecl(_tmpA7F,_tmpA7E,Cyc_Absyn_void_type,0);});_tmp5AA->f1=_tmpA80;}),({
# 2470
struct Cyc_Absyn_Pat*_tmpA7B=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[1]).l).first_line);_tmp5AA->f2=_tmpA7B;});_tmp5AA;});
# 2468
Cyc_Absyn_new_pat(_tmpA81,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2472
goto _LL0;case 387:
# 2473 "parse.y"
 if(({struct _fat_ptr _tmpA82=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_strcmp(_tmpA82,({const char*_tmp5AB="as";_tag_fat(_tmp5AB,sizeof(char),3U);}));})!= 0)
({void*_tmp5AC=0U;({unsigned _tmpA84=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmpA83=({const char*_tmp5AD="expecting `as'";_tag_fat(_tmp5AD,sizeof(char),15U);});Cyc_Warn_err(_tmpA84,_tmpA83,_tag_fat(_tmp5AC,sizeof(void*),0));});});
yyval=Cyc_YY9(({void*_tmpA8B=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp5B0=_cycalloc(sizeof(*_tmp5B0));_tmp5B0->tag=3,({struct Cyc_Absyn_Vardecl*_tmpA8A=({unsigned _tmpA89=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpA88=({struct _tuple0*_tmp5AF=_cycalloc(sizeof(*_tmp5AF));_tmp5AF->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpA87=({struct _fat_ptr*_tmp5AE=_cycalloc(sizeof(*_tmp5AE));({struct _fat_ptr _tmpA86=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp5AE=_tmpA86;});_tmp5AE;});_tmp5AF->f2=_tmpA87;});_tmp5AF;});Cyc_Absyn_new_vardecl(_tmpA89,_tmpA88,Cyc_Absyn_void_type,0);});_tmp5B0->f1=_tmpA8A;}),({
# 2477
struct Cyc_Absyn_Pat*_tmpA85=Cyc_yyget_YY9(&(yyyvsp[3]).v);_tmp5B0->f2=_tmpA85;});_tmp5B0;});
# 2475
Cyc_Absyn_new_pat(_tmpA8B,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2479
goto _LL0;case 388:  {
# 2480 "parse.y"
void*_tmp5B1=({struct _fat_ptr _tmpA8C=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_tmpA8C,Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik));});void*tag=_tmp5B1;
yyval=Cyc_YY9(({void*_tmpA93=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp5B4=_cycalloc(sizeof(*_tmp5B4));_tmp5B4->tag=4,({struct Cyc_Absyn_Tvar*_tmpA92=Cyc_Parse_typ2tvar((unsigned)((yyyvsp[2]).l).first_line,tag);_tmp5B4->f1=_tmpA92;}),({
struct Cyc_Absyn_Vardecl*_tmpA91=({unsigned _tmpA90=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpA8F=({struct _tuple0*_tmp5B3=_cycalloc(sizeof(*_tmp5B3));_tmp5B3->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpA8E=({struct _fat_ptr*_tmp5B2=_cycalloc(sizeof(*_tmp5B2));({struct _fat_ptr _tmpA8D=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5B2=_tmpA8D;});_tmp5B2;});_tmp5B3->f2=_tmpA8E;});_tmp5B3;});Cyc_Absyn_new_vardecl(_tmpA90,_tmpA8F,
Cyc_Absyn_tag_type(tag),0);});
# 2482
_tmp5B4->f2=_tmpA91;});_tmp5B4;});
# 2481
Cyc_Absyn_new_pat(_tmpA93,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2485
goto _LL0;}case 389:  {
# 2486 "parse.y"
struct Cyc_Absyn_Tvar*_tmp5B5=Cyc_Tcutil_new_tvar(Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik));struct Cyc_Absyn_Tvar*tv=_tmp5B5;
yyval=Cyc_YY9(({void*_tmpA99=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp5B8=_cycalloc(sizeof(*_tmp5B8));_tmp5B8->tag=4,_tmp5B8->f1=tv,({
struct Cyc_Absyn_Vardecl*_tmpA98=({unsigned _tmpA97=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpA96=({struct _tuple0*_tmp5B7=_cycalloc(sizeof(*_tmp5B7));_tmp5B7->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpA95=({struct _fat_ptr*_tmp5B6=_cycalloc(sizeof(*_tmp5B6));({struct _fat_ptr _tmpA94=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5B6=_tmpA94;});_tmp5B6;});_tmp5B7->f2=_tmpA95;});_tmp5B7;});Cyc_Absyn_new_vardecl(_tmpA97,_tmpA96,
Cyc_Absyn_tag_type(Cyc_Absyn_var_type(tv)),0);});
# 2488
_tmp5B8->f2=_tmpA98;});_tmp5B8;});
# 2487
Cyc_Absyn_new_pat(_tmpA99,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2491
goto _LL0;}case 390:
# 2494 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp5B9=_cycalloc(sizeof(*_tmp5B9));({struct Cyc_List_List*_tmpA9B=({struct Cyc_List_List*(*_tmpA9A)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev;});_tmpA9A(Cyc_yyget_YY11(&(yyyvsp[0]).v));});_tmp5B9->f1=_tmpA9B;}),_tmp5B9->f2=0;_tmp5B9;}));
goto _LL0;case 391:
# 2495 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp5BA=_cycalloc(sizeof(*_tmp5BA));({struct Cyc_List_List*_tmpA9D=({struct Cyc_List_List*(*_tmpA9C)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev;});_tmpA9C(Cyc_yyget_YY11(&(yyyvsp[0]).v));});_tmp5BA->f1=_tmpA9D;}),_tmp5BA->f2=1;_tmp5BA;}));
goto _LL0;case 392:
# 2496 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp5BB=_cycalloc(sizeof(*_tmp5BB));_tmp5BB->f1=0,_tmp5BB->f2=1;_tmp5BB;}));
goto _LL0;case 393:
# 2499
 yyval=Cyc_YY11(({struct Cyc_List_List*_tmp5BC=_cycalloc(sizeof(*_tmp5BC));({struct Cyc_Absyn_Pat*_tmpA9E=Cyc_yyget_YY9(&(yyyvsp[0]).v);_tmp5BC->hd=_tmpA9E;}),_tmp5BC->tl=0;_tmp5BC;}));
goto _LL0;case 394:
# 2500 "parse.y"
 yyval=Cyc_YY11(({struct Cyc_List_List*_tmp5BD=_cycalloc(sizeof(*_tmp5BD));({struct Cyc_Absyn_Pat*_tmpAA0=Cyc_yyget_YY9(&(yyyvsp[2]).v);_tmp5BD->hd=_tmpAA0;}),({struct Cyc_List_List*_tmpA9F=Cyc_yyget_YY11(&(yyyvsp[0]).v);_tmp5BD->tl=_tmpA9F;});_tmp5BD;}));
goto _LL0;case 395:
# 2503
 yyval=Cyc_YY12(({struct _tuple24*_tmp5BE=_cycalloc(sizeof(*_tmp5BE));_tmp5BE->f1=0,({struct Cyc_Absyn_Pat*_tmpAA1=Cyc_yyget_YY9(&(yyyvsp[0]).v);_tmp5BE->f2=_tmpAA1;});_tmp5BE;}));
goto _LL0;case 396:
# 2504 "parse.y"
 yyval=Cyc_YY12(({struct _tuple24*_tmp5BF=_cycalloc(sizeof(*_tmp5BF));({struct Cyc_List_List*_tmpAA3=Cyc_yyget_YY42(&(yyyvsp[0]).v);_tmp5BF->f1=_tmpAA3;}),({struct Cyc_Absyn_Pat*_tmpAA2=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5BF->f2=_tmpAA2;});_tmp5BF;}));
goto _LL0;case 397:
# 2507
 yyval=Cyc_YY14(({struct _tuple23*_tmp5C0=_cycalloc(sizeof(*_tmp5C0));({struct Cyc_List_List*_tmpAA5=({struct Cyc_List_List*(*_tmpAA4)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev;});_tmpAA4(Cyc_yyget_YY13(&(yyyvsp[0]).v));});_tmp5C0->f1=_tmpAA5;}),_tmp5C0->f2=0;_tmp5C0;}));
goto _LL0;case 398:
# 2508 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_tmp5C1=_cycalloc(sizeof(*_tmp5C1));({struct Cyc_List_List*_tmpAA7=({struct Cyc_List_List*(*_tmpAA6)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev;});_tmpAA6(Cyc_yyget_YY13(&(yyyvsp[0]).v));});_tmp5C1->f1=_tmpAA7;}),_tmp5C1->f2=1;_tmp5C1;}));
goto _LL0;case 399:
# 2509 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_tmp5C2=_cycalloc(sizeof(*_tmp5C2));_tmp5C2->f1=0,_tmp5C2->f2=1;_tmp5C2;}));
goto _LL0;case 400:
# 2512
 yyval=Cyc_YY13(({struct Cyc_List_List*_tmp5C3=_cycalloc(sizeof(*_tmp5C3));({struct _tuple24*_tmpAA8=Cyc_yyget_YY12(&(yyyvsp[0]).v);_tmp5C3->hd=_tmpAA8;}),_tmp5C3->tl=0;_tmp5C3;}));
goto _LL0;case 401:
# 2513 "parse.y"
 yyval=Cyc_YY13(({struct Cyc_List_List*_tmp5C4=_cycalloc(sizeof(*_tmp5C4));({struct _tuple24*_tmpAAA=Cyc_yyget_YY12(&(yyyvsp[2]).v);_tmp5C4->hd=_tmpAAA;}),({struct Cyc_List_List*_tmpAA9=Cyc_yyget_YY13(&(yyyvsp[0]).v);_tmp5C4->tl=_tmpAA9;});_tmp5C4;}));
goto _LL0;case 402:
# 2518 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 403:
# 2519 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAAC=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpAAB=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_seq_exp(_tmpAAC,_tmpAAB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 404:
# 2523 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 405:
# 2525 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAAF=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Core_Opt*_tmpAAE=Cyc_yyget_YY7(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpAAD=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_assignop_exp(_tmpAAF,_tmpAAE,_tmpAAD,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 406:
# 2527 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAB1=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpAB0=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_swap_exp(_tmpAB1,_tmpAB0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 407:
# 2531 "parse.y"
 yyval=Cyc_YY7(0);
goto _LL0;case 408:
# 2532 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5C5=_cycalloc(sizeof(*_tmp5C5));_tmp5C5->v=(void*)1U;_tmp5C5;}));
goto _LL0;case 409:
# 2533 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5C6=_cycalloc(sizeof(*_tmp5C6));_tmp5C6->v=(void*)3U;_tmp5C6;}));
goto _LL0;case 410:
# 2534 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5C7=_cycalloc(sizeof(*_tmp5C7));_tmp5C7->v=(void*)4U;_tmp5C7;}));
goto _LL0;case 411:
# 2535 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5C8=_cycalloc(sizeof(*_tmp5C8));_tmp5C8->v=(void*)0U;_tmp5C8;}));
goto _LL0;case 412:
# 2536 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5C9=_cycalloc(sizeof(*_tmp5C9));_tmp5C9->v=(void*)2U;_tmp5C9;}));
goto _LL0;case 413:
# 2537 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5CA=_cycalloc(sizeof(*_tmp5CA));_tmp5CA->v=(void*)16U;_tmp5CA;}));
goto _LL0;case 414:
# 2538 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5CB=_cycalloc(sizeof(*_tmp5CB));_tmp5CB->v=(void*)17U;_tmp5CB;}));
goto _LL0;case 415:
# 2539 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5CC=_cycalloc(sizeof(*_tmp5CC));_tmp5CC->v=(void*)13U;_tmp5CC;}));
goto _LL0;case 416:
# 2540 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5CD=_cycalloc(sizeof(*_tmp5CD));_tmp5CD->v=(void*)15U;_tmp5CD;}));
goto _LL0;case 417:
# 2541 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5CE=_cycalloc(sizeof(*_tmp5CE));_tmp5CE->v=(void*)14U;_tmp5CE;}));
goto _LL0;case 418:
# 2545 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 419:
# 2547 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAB4=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpAB3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpAB2=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_conditional_exp(_tmpAB4,_tmpAB3,_tmpAB2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 420:
# 2549 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAB5=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_throw_exp(_tmpAB5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 421:
# 2551 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAB6=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_New_exp(0,_tmpAB6,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 422:
# 2552 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAB7=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_New_exp(0,_tmpAB7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 423:
# 2553 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAB9=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpAB8=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_New_exp(_tmpAB9,_tmpAB8,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 424:
# 2555 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpABB=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpABA=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_New_exp(_tmpABB,_tmpABA,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 425:
# 2559 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 426:
# 2562
 yyval=(yyyvsp[0]).v;
goto _LL0;case 427:
# 2564 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpABD=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpABC=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_or_exp(_tmpABD,_tmpABC,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 428:
# 2567
 yyval=(yyyvsp[0]).v;
goto _LL0;case 429:
# 2569 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpABF=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpABE=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_and_exp(_tmpABF,_tmpABE,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 430:
# 2572
 yyval=(yyyvsp[0]).v;
goto _LL0;case 431:
# 2574 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAC1=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpAC0=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(14U,_tmpAC1,_tmpAC0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 432:
# 2577
 yyval=(yyyvsp[0]).v;
goto _LL0;case 433:
# 2579 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAC3=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpAC2=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(15U,_tmpAC3,_tmpAC2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 434:
# 2582
 yyval=(yyyvsp[0]).v;
goto _LL0;case 435:
# 2584 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAC5=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpAC4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(13U,_tmpAC5,_tmpAC4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 436:
# 2587
 yyval=(yyyvsp[0]).v;
goto _LL0;case 437:
# 2589 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*(*_tmpAC8)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY66(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpAC7=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpAC6=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmpAC8(_tmpAC7,_tmpAC6,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 438:
# 2592
 yyval=(yyyvsp[0]).v;
goto _LL0;case 439:
# 2594 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*(*_tmpACB)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY66(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpACA=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpAC9=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmpACB(_tmpACA,_tmpAC9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 440:
# 2597
 yyval=(yyyvsp[0]).v;
goto _LL0;case 441:
# 2599 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpACD=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpACC=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(16U,_tmpACD,_tmpACC,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 442:
# 2601 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpACF=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpACE=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(17U,_tmpACF,_tmpACE,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 443:
# 2604
 yyval=(yyyvsp[0]).v;
goto _LL0;case 444:
# 2606 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _tmpAD2=Cyc_yyget_YY6(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpAD1=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpAD0=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(_tmpAD2,_tmpAD1,_tmpAD0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 445:
# 2609
 yyval=(yyyvsp[0]).v;
goto _LL0;case 446:
# 2611 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _tmpAD5=Cyc_yyget_YY6(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpAD4=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpAD3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(_tmpAD5,_tmpAD4,_tmpAD3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 447:
# 2614
 yyval=Cyc_YY66(Cyc_Absyn_eq_exp);
goto _LL0;case 448:
# 2615 "parse.y"
 yyval=Cyc_YY66(Cyc_Absyn_neq_exp);
goto _LL0;case 449:
# 2618
 yyval=Cyc_YY66(Cyc_Absyn_lt_exp);
goto _LL0;case 450:
# 2619 "parse.y"
 yyval=Cyc_YY66(Cyc_Absyn_gt_exp);
goto _LL0;case 451:
# 2620 "parse.y"
 yyval=Cyc_YY66(Cyc_Absyn_lte_exp);
goto _LL0;case 452:
# 2621 "parse.y"
 yyval=Cyc_YY66(Cyc_Absyn_gte_exp);
goto _LL0;case 453:
# 2624
 yyval=Cyc_YY6(0U);
goto _LL0;case 454:
# 2625 "parse.y"
 yyval=Cyc_YY6(2U);
goto _LL0;case 455:
# 2628
 yyval=Cyc_YY6(1U);
goto _LL0;case 456:
# 2629 "parse.y"
 yyval=Cyc_YY6(3U);
goto _LL0;case 457:
# 2630 "parse.y"
 yyval=Cyc_YY6(4U);
goto _LL0;case 458:
# 2634 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 459:  {
# 2636 "parse.y"
void*_tmp5CF=({struct _tuple8*_tmpAD6=Cyc_yyget_YY38(&(yyyvsp[1]).v);Cyc_Parse_type_name_to_type(_tmpAD6,(unsigned)((yyyvsp[1]).l).first_line);});void*t=_tmp5CF;
yyval=Cyc_Exp_tok(({void*_tmpAD8=t;struct Cyc_Absyn_Exp*_tmpAD7=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_cast_exp(_tmpAD8,_tmpAD7,1,0U,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 460:
# 2641 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 461:
# 2642 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAD9=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_increment_exp(_tmpAD9,0U,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 462:
# 2643 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpADA=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_increment_exp(_tmpADA,2U,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 463:
# 2644 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpADB=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_address_exp(_tmpADB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 464:
# 2645 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpADC=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_deref_exp(_tmpADC,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 465:
# 2646 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _tmpADE=Cyc_yyget_YY6(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpADD=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(_tmpADE,_tmpADD,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 466:  {
# 2648 "parse.y"
void*_tmp5D0=({struct _tuple8*_tmpADF=Cyc_yyget_YY38(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpADF,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp5D0;
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeoftype_exp(t,(unsigned)((yyyvsp[0]).l).first_line));
# 2651
goto _LL0;}case 467:
# 2651 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAE0=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_sizeofexp_exp(_tmpAE0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 468:  {
# 2653 "parse.y"
void*_tmp5D1=({struct _tuple8*_tmpAE1=Cyc_yyget_YY38(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpAE1,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp5D1;
yyval=Cyc_Exp_tok(({void*_tmpAE4=t;struct Cyc_List_List*_tmpAE3=({struct Cyc_List_List*(*_tmpAE2)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_tmpAE2(Cyc_yyget_YY3(&(yyyvsp[4]).v));});Cyc_Absyn_offsetof_exp(_tmpAE4,_tmpAE3,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2656
goto _LL0;}case 469:
# 2658
 yyval=Cyc_Exp_tok(({void*_tmpAE6=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp5D2=_cycalloc(sizeof(*_tmp5D2));_tmp5D2->tag=34,(_tmp5D2->f1).is_calloc=0,(_tmp5D2->f1).rgn=0,(_tmp5D2->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpAE5=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp5D2->f1).num_elts=_tmpAE5;}),(_tmp5D2->f1).fat_result=0,(_tmp5D2->f1).inline_call=0;_tmp5D2;});Cyc_Absyn_new_exp(_tmpAE6,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2660
goto _LL0;case 470:
# 2661 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpAE9=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp5D3=_cycalloc(sizeof(*_tmp5D3));_tmp5D3->tag=34,(_tmp5D3->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpAE8=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp5D3->f1).rgn=_tmpAE8;}),(_tmp5D3->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpAE7=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp5D3->f1).num_elts=_tmpAE7;}),(_tmp5D3->f1).fat_result=0,(_tmp5D3->f1).inline_call=0;_tmp5D3;});Cyc_Absyn_new_exp(_tmpAE9,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2663
goto _LL0;case 471:
# 2664 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpAEC=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp5D4=_cycalloc(sizeof(*_tmp5D4));_tmp5D4->tag=34,(_tmp5D4->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpAEB=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp5D4->f1).rgn=_tmpAEB;}),(_tmp5D4->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpAEA=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp5D4->f1).num_elts=_tmpAEA;}),(_tmp5D4->f1).fat_result=0,(_tmp5D4->f1).inline_call=1;_tmp5D4;});Cyc_Absyn_new_exp(_tmpAEC,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2666
goto _LL0;case 472:  {
# 2667 "parse.y"
void*_tmp5D5=({struct _tuple8*_tmpAED=Cyc_yyget_YY38(&(yyyvsp[6]).v);Cyc_Parse_type_name_to_type(_tmpAED,(unsigned)((yyyvsp[6]).l).first_line);});void*t=_tmp5D5;
yyval=Cyc_Exp_tok(({void*_tmpAF0=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp5D7=_cycalloc(sizeof(*_tmp5D7));_tmp5D7->tag=34,(_tmp5D7->f1).is_calloc=1,(_tmp5D7->f1).rgn=0,({void**_tmpAEF=({void**_tmp5D6=_cycalloc(sizeof(*_tmp5D6));*_tmp5D6=t;_tmp5D6;});(_tmp5D7->f1).elt_type=_tmpAEF;}),({struct Cyc_Absyn_Exp*_tmpAEE=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp5D7->f1).num_elts=_tmpAEE;}),(_tmp5D7->f1).fat_result=0,(_tmp5D7->f1).inline_call=0;_tmp5D7;});Cyc_Absyn_new_exp(_tmpAF0,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2670
goto _LL0;}case 473:  {
# 2672
void*_tmp5D8=({struct _tuple8*_tmpAF1=Cyc_yyget_YY38(&(yyyvsp[8]).v);Cyc_Parse_type_name_to_type(_tmpAF1,(unsigned)((yyyvsp[8]).l).first_line);});void*t=_tmp5D8;
yyval=Cyc_Exp_tok(({void*_tmpAF5=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp5DA=_cycalloc(sizeof(*_tmp5DA));_tmp5DA->tag=34,(_tmp5DA->f1).is_calloc=1,({struct Cyc_Absyn_Exp*_tmpAF4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp5DA->f1).rgn=_tmpAF4;}),({void**_tmpAF3=({void**_tmp5D9=_cycalloc(sizeof(*_tmp5D9));*_tmp5D9=t;_tmp5D9;});(_tmp5DA->f1).elt_type=_tmpAF3;}),({struct Cyc_Absyn_Exp*_tmpAF2=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp5DA->f1).num_elts=_tmpAF2;}),(_tmp5DA->f1).fat_result=0,(_tmp5DA->f1).inline_call=0;_tmp5DA;});Cyc_Absyn_new_exp(_tmpAF5,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2675
goto _LL0;}case 474:
# 2676 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_tmpAF7=({struct Cyc_Absyn_Exp*_tmp5DB[1];({struct Cyc_Absyn_Exp*_tmpAF6=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp5DB[0]=_tmpAF6;});({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_tmp5DB,sizeof(struct Cyc_Absyn_Exp*),1));});Cyc_Absyn_primop_exp(18U,_tmpAF7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 475:
# 2678 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpAFB=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp5DD=_cycalloc(sizeof(*_tmp5DD));_tmp5DD->tag=38,({struct Cyc_Absyn_Exp*_tmpAFA=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp5DD->f1=_tmpAFA;}),({struct _fat_ptr*_tmpAF9=({struct _fat_ptr*_tmp5DC=_cycalloc(sizeof(*_tmp5DC));({struct _fat_ptr _tmpAF8=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp5DC=_tmpAF8;});_tmp5DC;});_tmp5DD->f2=_tmpAF9;});_tmp5DD;});Cyc_Absyn_new_exp(_tmpAFB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 476:
# 2680 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpB00=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp5DF=_cycalloc(sizeof(*_tmp5DF));_tmp5DF->tag=38,({struct Cyc_Absyn_Exp*_tmpAFF=({struct Cyc_Absyn_Exp*_tmpAFE=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_deref_exp(_tmpAFE,(unsigned)((yyyvsp[2]).l).first_line);});_tmp5DF->f1=_tmpAFF;}),({struct _fat_ptr*_tmpAFD=({struct _fat_ptr*_tmp5DE=_cycalloc(sizeof(*_tmp5DE));({struct _fat_ptr _tmpAFC=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp5DE=_tmpAFC;});_tmp5DE;});_tmp5DF->f2=_tmpAFD;});_tmp5DF;});Cyc_Absyn_new_exp(_tmpB00,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 477:  {
# 2682 "parse.y"
void*_tmp5E0=({struct _tuple8*_tmpB01=Cyc_yyget_YY38(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpB01,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp5E0;
yyval=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(t,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;}case 478:
# 2684 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpB02=Cyc_yyget_YY60(&(yyyvsp[1]).v);Cyc_Absyn_new_exp(_tmpB02,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 479:
# 2685 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB03=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_extension_exp(_tmpB03,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 480:
# 2686 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB04=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_assert_exp(_tmpB04,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 481:
# 2690 "parse.y"
 yyval=Cyc_YY6(12U);
goto _LL0;case 482:
# 2691 "parse.y"
 yyval=Cyc_YY6(11U);
goto _LL0;case 483:
# 2692 "parse.y"
 yyval=Cyc_YY6(2U);
goto _LL0;case 484:
# 2693 "parse.y"
 yyval=Cyc_YY6(0U);
goto _LL0;case 485:  {
# 2698 "parse.y"
struct _tuple30*_tmp5E1=Cyc_yyget_YY61(&(yyyvsp[3]).v);struct _tuple30*_stmttmp2D=_tmp5E1;struct _tuple30*_tmp5E2=_stmttmp2D;void*_tmp5E5;void*_tmp5E4;void*_tmp5E3;_tmp5E3=_tmp5E2->f1;_tmp5E4=_tmp5E2->f2;_tmp5E5=_tmp5E2->f3;{struct Cyc_List_List*outlist=_tmp5E3;struct Cyc_List_List*inlist=_tmp5E4;struct Cyc_List_List*clobbers=_tmp5E5;
yyval=Cyc_YY60((void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp5E6=_cycalloc(sizeof(*_tmp5E6));_tmp5E6->tag=40,({int _tmpB06=Cyc_yyget_YY32(&(yyyvsp[0]).v);_tmp5E6->f1=_tmpB06;}),({struct _fat_ptr _tmpB05=Cyc_yyget_String_tok(&(yyyvsp[2]).v);_tmp5E6->f2=_tmpB05;}),_tmp5E6->f3=outlist,_tmp5E6->f4=inlist,_tmp5E6->f5=clobbers;_tmp5E6;}));
goto _LL0;}}case 486:
# 2703 "parse.y"
 yyval=Cyc_YY32(0);
goto _LL0;case 487:
# 2704 "parse.y"
 yyval=Cyc_YY32(1);
goto _LL0;case 488:
# 2708 "parse.y"
 yyval=Cyc_YY61(({struct _tuple30*_tmp5E7=_cycalloc(sizeof(*_tmp5E7));_tmp5E7->f1=0,_tmp5E7->f2=0,_tmp5E7->f3=0;_tmp5E7;}));
goto _LL0;case 489:  {
# 2710 "parse.y"
struct _tuple31*_tmp5E8=Cyc_yyget_YY62(&(yyyvsp[1]).v);struct _tuple31*_stmttmp2E=_tmp5E8;struct _tuple31*_tmp5E9=_stmttmp2E;void*_tmp5EB;void*_tmp5EA;_tmp5EA=_tmp5E9->f1;_tmp5EB=_tmp5E9->f2;{struct Cyc_List_List*inlist=_tmp5EA;struct Cyc_List_List*clobbers=_tmp5EB;
yyval=Cyc_YY61(({struct _tuple30*_tmp5EC=_cycalloc(sizeof(*_tmp5EC));_tmp5EC->f1=0,_tmp5EC->f2=inlist,_tmp5EC->f3=clobbers;_tmp5EC;}));
goto _LL0;}}case 490:  {
# 2713 "parse.y"
struct _tuple31*_tmp5ED=Cyc_yyget_YY62(&(yyyvsp[2]).v);struct _tuple31*_stmttmp2F=_tmp5ED;struct _tuple31*_tmp5EE=_stmttmp2F;void*_tmp5F0;void*_tmp5EF;_tmp5EF=_tmp5EE->f1;_tmp5F0=_tmp5EE->f2;{struct Cyc_List_List*inlist=_tmp5EF;struct Cyc_List_List*clobbers=_tmp5F0;
yyval=Cyc_YY61(({struct _tuple30*_tmp5F1=_cycalloc(sizeof(*_tmp5F1));({struct Cyc_List_List*_tmpB08=({struct Cyc_List_List*(*_tmpB07)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_tmpB07(Cyc_yyget_YY64(&(yyyvsp[1]).v));});_tmp5F1->f1=_tmpB08;}),_tmp5F1->f2=inlist,_tmp5F1->f3=clobbers;_tmp5F1;}));
goto _LL0;}}case 491:
# 2717
 yyval=Cyc_YY64(({struct Cyc_List_List*_tmp5F2=_cycalloc(sizeof(*_tmp5F2));({struct _tuple32*_tmpB09=Cyc_yyget_YY65(&(yyyvsp[0]).v);_tmp5F2->hd=_tmpB09;}),_tmp5F2->tl=0;_tmp5F2;}));
goto _LL0;case 492:
# 2718 "parse.y"
 yyval=Cyc_YY64(({struct Cyc_List_List*_tmp5F3=_cycalloc(sizeof(*_tmp5F3));({struct _tuple32*_tmpB0B=Cyc_yyget_YY65(&(yyyvsp[2]).v);_tmp5F3->hd=_tmpB0B;}),({struct Cyc_List_List*_tmpB0A=Cyc_yyget_YY64(&(yyyvsp[0]).v);_tmp5F3->tl=_tmpB0A;});_tmp5F3;}));
goto _LL0;case 493:
# 2722 "parse.y"
 yyval=Cyc_YY62(({struct _tuple31*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));_tmp5F4->f1=0,_tmp5F4->f2=0;_tmp5F4;}));
goto _LL0;case 494:
# 2723 "parse.y"
 yyval=Cyc_YY62(({struct _tuple31*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));_tmp5F5->f1=0,({struct Cyc_List_List*_tmpB0C=Cyc_yyget_YY63(&(yyyvsp[1]).v);_tmp5F5->f2=_tmpB0C;});_tmp5F5;}));
goto _LL0;case 495:
# 2724 "parse.y"
 yyval=Cyc_YY62(({struct _tuple31*_tmp5F6=_cycalloc(sizeof(*_tmp5F6));({struct Cyc_List_List*_tmpB0F=({struct Cyc_List_List*(*_tmpB0E)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_tmpB0E(Cyc_yyget_YY64(&(yyyvsp[1]).v));});_tmp5F6->f1=_tmpB0F;}),({struct Cyc_List_List*_tmpB0D=Cyc_yyget_YY63(&(yyyvsp[2]).v);_tmp5F6->f2=_tmpB0D;});_tmp5F6;}));
goto _LL0;case 496:
# 2727
 yyval=Cyc_YY64(({struct Cyc_List_List*_tmp5F7=_cycalloc(sizeof(*_tmp5F7));({struct _tuple32*_tmpB10=Cyc_yyget_YY65(&(yyyvsp[0]).v);_tmp5F7->hd=_tmpB10;}),_tmp5F7->tl=0;_tmp5F7;}));
goto _LL0;case 497:
# 2728 "parse.y"
 yyval=Cyc_YY64(({struct Cyc_List_List*_tmp5F8=_cycalloc(sizeof(*_tmp5F8));({struct _tuple32*_tmpB12=Cyc_yyget_YY65(&(yyyvsp[2]).v);_tmp5F8->hd=_tmpB12;}),({struct Cyc_List_List*_tmpB11=Cyc_yyget_YY64(&(yyyvsp[0]).v);_tmp5F8->tl=_tmpB11;});_tmp5F8;}));
goto _LL0;case 498:
# 2732 "parse.y"
 yyval=Cyc_YY65(({struct _tuple32*_tmp5F9=_cycalloc(sizeof(*_tmp5F9));({struct _fat_ptr _tmpB14=Cyc_yyget_String_tok(&(yyyvsp[0]).v);_tmp5F9->f1=_tmpB14;}),({struct Cyc_Absyn_Exp*_tmpB13=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp5F9->f2=_tmpB13;});_tmp5F9;}));
goto _LL0;case 499:
# 2737 "parse.y"
 yyval=Cyc_YY63(0);
goto _LL0;case 500:
# 2738 "parse.y"
 yyval=Cyc_YY63(0);
goto _LL0;case 501:
# 2739 "parse.y"
 yyval=Cyc_YY63(({struct Cyc_List_List*(*_tmpB15)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_tmpB15(Cyc_yyget_YY63(&(yyyvsp[1]).v));}));
goto _LL0;case 502:
# 2742
 yyval=Cyc_YY63(({struct Cyc_List_List*_tmp5FB=_cycalloc(sizeof(*_tmp5FB));({struct _fat_ptr*_tmpB17=({struct _fat_ptr*_tmp5FA=_cycalloc(sizeof(*_tmp5FA));({struct _fat_ptr _tmpB16=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5FA=_tmpB16;});_tmp5FA;});_tmp5FB->hd=_tmpB17;}),_tmp5FB->tl=0;_tmp5FB;}));
goto _LL0;case 503:
# 2743 "parse.y"
 yyval=Cyc_YY63(({struct Cyc_List_List*_tmp5FD=_cycalloc(sizeof(*_tmp5FD));({struct _fat_ptr*_tmpB1A=({struct _fat_ptr*_tmp5FC=_cycalloc(sizeof(*_tmp5FC));({struct _fat_ptr _tmpB19=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp5FC=_tmpB19;});_tmp5FC;});_tmp5FD->hd=_tmpB1A;}),({struct Cyc_List_List*_tmpB18=Cyc_yyget_YY63(&(yyyvsp[0]).v);_tmp5FD->tl=_tmpB18;});_tmp5FD;}));
goto _LL0;case 504:
# 2748 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 505:
# 2750 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB1C=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB1B=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_subscript_exp(_tmpB1C,_tmpB1B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 506:
# 2752 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB1D=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_unknowncall_exp(_tmpB1D,0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 507:
# 2754 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB1F=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_List_List*_tmpB1E=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_unknowncall_exp(_tmpB1F,_tmpB1E,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 508:
# 2756 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB22=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct _fat_ptr*_tmpB21=({struct _fat_ptr*_tmp5FE=_cycalloc(sizeof(*_tmp5FE));({struct _fat_ptr _tmpB20=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp5FE=_tmpB20;});_tmp5FE;});Cyc_Absyn_aggrmember_exp(_tmpB22,_tmpB21,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 509:
# 2758 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB25=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct _fat_ptr*_tmpB24=({struct _fat_ptr*_tmp5FF=_cycalloc(sizeof(*_tmp5FF));({struct _fat_ptr _tmpB23=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp5FF=_tmpB23;});_tmp5FF;});Cyc_Absyn_aggrarrow_exp(_tmpB25,_tmpB24,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 510:
# 2760 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB26=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_increment_exp(_tmpB26,1U,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 511:
# 2762 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB27=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_increment_exp(_tmpB27,3U,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 512:
# 2764 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpB29=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp600=_cycalloc(sizeof(*_tmp600));_tmp600->tag=25,({struct _tuple8*_tmpB28=Cyc_yyget_YY38(&(yyyvsp[1]).v);_tmp600->f1=_tmpB28;}),_tmp600->f2=0;_tmp600;});Cyc_Absyn_new_exp(_tmpB29,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 513:
# 2766 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpB2D=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp601=_cycalloc(sizeof(*_tmp601));_tmp601->tag=25,({struct _tuple8*_tmpB2C=Cyc_yyget_YY38(&(yyyvsp[1]).v);_tmp601->f1=_tmpB2C;}),({struct Cyc_List_List*_tmpB2B=({struct Cyc_List_List*(*_tmpB2A)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_tmpB2A(Cyc_yyget_YY5(&(yyyvsp[4]).v));});_tmp601->f2=_tmpB2B;});_tmp601;});Cyc_Absyn_new_exp(_tmpB2D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 514:
# 2768 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpB31=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp602=_cycalloc(sizeof(*_tmp602));_tmp602->tag=25,({struct _tuple8*_tmpB30=Cyc_yyget_YY38(&(yyyvsp[1]).v);_tmp602->f1=_tmpB30;}),({struct Cyc_List_List*_tmpB2F=({struct Cyc_List_List*(*_tmpB2E)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_tmpB2E(Cyc_yyget_YY5(&(yyyvsp[4]).v));});_tmp602->f2=_tmpB2F;});_tmp602;});Cyc_Absyn_new_exp(_tmpB31,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 515:
# 2773 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp605=_cycalloc(sizeof(*_tmp605));({void*_tmpB34=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp604=_cycalloc(sizeof(*_tmp604));_tmp604->tag=0,({struct _fat_ptr*_tmpB33=({struct _fat_ptr*_tmp603=_cycalloc(sizeof(*_tmp603));({struct _fat_ptr _tmpB32=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp603=_tmpB32;});_tmp603;});_tmp604->f1=_tmpB33;});_tmp604;});_tmp605->hd=_tmpB34;}),_tmp605->tl=0;_tmp605;}));
goto _LL0;case 516:
# 2776
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp607=_cycalloc(sizeof(*_tmp607));({void*_tmpB37=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp606=_cycalloc(sizeof(*_tmp606));_tmp606->tag=1,({unsigned _tmpB36=({unsigned _tmpB35=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Parse_cnst2uint(_tmpB35,Cyc_yyget_Int_tok(&(yyyvsp[0]).v));});_tmp606->f1=_tmpB36;});_tmp606;});_tmp607->hd=_tmpB37;}),_tmp607->tl=0;_tmp607;}));
goto _LL0;case 517:
# 2778 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp60A=_cycalloc(sizeof(*_tmp60A));({void*_tmpB3B=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp609=_cycalloc(sizeof(*_tmp609));_tmp609->tag=0,({struct _fat_ptr*_tmpB3A=({struct _fat_ptr*_tmp608=_cycalloc(sizeof(*_tmp608));({struct _fat_ptr _tmpB39=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp608=_tmpB39;});_tmp608;});_tmp609->f1=_tmpB3A;});_tmp609;});_tmp60A->hd=_tmpB3B;}),({struct Cyc_List_List*_tmpB38=Cyc_yyget_YY3(&(yyyvsp[0]).v);_tmp60A->tl=_tmpB38;});_tmp60A;}));
goto _LL0;case 518:
# 2781
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp60C=_cycalloc(sizeof(*_tmp60C));({void*_tmpB3F=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp60B=_cycalloc(sizeof(*_tmp60B));_tmp60B->tag=1,({unsigned _tmpB3E=({unsigned _tmpB3D=(unsigned)((yyyvsp[2]).l).first_line;Cyc_Parse_cnst2uint(_tmpB3D,Cyc_yyget_Int_tok(&(yyyvsp[2]).v));});_tmp60B->f1=_tmpB3E;});_tmp60B;});_tmp60C->hd=_tmpB3F;}),({struct Cyc_List_List*_tmpB3C=Cyc_yyget_YY3(&(yyyvsp[0]).v);_tmp60C->tl=_tmpB3C;});_tmp60C;}));
goto _LL0;case 519:
# 2786 "parse.y"
 yyval=Cyc_Exp_tok(({struct _tuple0*_tmpB40=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);Cyc_Absyn_unknownid_exp(_tmpB40,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 520:
# 2787 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpB41=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Absyn_pragma_exp(_tmpB41,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 521:
# 2788 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 522:
# 2789 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpB42=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_string_exp(_tmpB42,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 523:
# 2790 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpB43=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_wstring_exp(_tmpB43,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 524:
# 2791 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 525:
# 2795 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB44=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_noinstantiate_exp(_tmpB44,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 526:
# 2798
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB47=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_List_List*_tmpB46=({struct Cyc_List_List*(*_tmpB45)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_tmpB45(Cyc_yyget_YY41(&(yyyvsp[3]).v));});Cyc_Absyn_instantiate_exp(_tmpB47,_tmpB46,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 527:
# 2801
 yyval=Cyc_Exp_tok(({void*_tmpB4C=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp60D=_cycalloc(sizeof(*_tmp60D));_tmp60D->tag=29,({struct _tuple0*_tmpB4B=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp60D->f1=_tmpB4B;}),({struct Cyc_List_List*_tmpB4A=Cyc_yyget_YY41(&(yyyvsp[2]).v);_tmp60D->f2=_tmpB4A;}),({struct Cyc_List_List*_tmpB49=({struct Cyc_List_List*(*_tmpB48)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_tmpB48(Cyc_yyget_YY5(&(yyyvsp[3]).v));});_tmp60D->f3=_tmpB49;}),_tmp60D->f4=0;_tmp60D;});Cyc_Absyn_new_exp(_tmpB4C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 528:
# 2803 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_tmpB4D=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_tuple_exp(_tmpB4D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 529:
# 2805 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Stmt*_tmpB4E=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Absyn_stmt_exp(_tmpB4E,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 530:
# 2809 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*(*_tmpB4F)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_tmpB4F(Cyc_yyget_YY4(&(yyyvsp[0]).v));}));
goto _LL0;case 531:
# 2814 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_tmp60E=_cycalloc(sizeof(*_tmp60E));({struct Cyc_Absyn_Exp*_tmpB50=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);_tmp60E->hd=_tmpB50;}),_tmp60E->tl=0;_tmp60E;}));
goto _LL0;case 532:
# 2816 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_tmp60F=_cycalloc(sizeof(*_tmp60F));({struct Cyc_Absyn_Exp*_tmpB52=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp60F->hd=_tmpB52;}),({struct Cyc_List_List*_tmpB51=Cyc_yyget_YY4(&(yyyvsp[0]).v);_tmp60F->tl=_tmpB51;});_tmp60F;}));
goto _LL0;case 533:
# 2822 "parse.y"
 yyval=Cyc_Exp_tok(({union Cyc_Absyn_Cnst _tmpB53=Cyc_yyget_Int_tok(&(yyyvsp[0]).v);Cyc_Absyn_const_exp(_tmpB53,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 534:
# 2823 "parse.y"
 yyval=Cyc_Exp_tok(({char _tmpB54=Cyc_yyget_Char_tok(&(yyyvsp[0]).v);Cyc_Absyn_char_exp(_tmpB54,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 535:
# 2824 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpB55=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_wchar_exp(_tmpB55,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 536:
# 2826 "parse.y"
 yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 537:  {
# 2828 "parse.y"
struct _fat_ptr _tmp610=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr f=_tmp610;
int l=(int)Cyc_strlen((struct _fat_ptr)f);
int i=1;
if(l > 0){
char c=*((const char*)_check_fat_subscript(f,sizeof(char),l - 1));
if((int)c == 102 ||(int)c == 70)i=0;else{
if((int)c == 108 ||(int)c == 76)i=2;}}
# 2836
yyval=Cyc_Exp_tok(Cyc_Absyn_float_exp(f,i,(unsigned)((yyyvsp[0]).l).first_line));
# 2838
goto _LL0;}case 538:
# 2841 "parse.y"
 yyval=Cyc_QualId_tok(({struct _tuple0*_tmp612=_cycalloc(sizeof(*_tmp612));({union Cyc_Absyn_Nmspace _tmpB58=Cyc_Absyn_Rel_n(0);_tmp612->f1=_tmpB58;}),({struct _fat_ptr*_tmpB57=({struct _fat_ptr*_tmp611=_cycalloc(sizeof(*_tmp611));({struct _fat_ptr _tmpB56=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp611=_tmpB56;});_tmp611;});_tmp612->f2=_tmpB57;});_tmp612;}));
goto _LL0;case 539:
# 2842 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 540:
# 2845
 yyval=Cyc_QualId_tok(({struct _tuple0*_tmp614=_cycalloc(sizeof(*_tmp614));({union Cyc_Absyn_Nmspace _tmpB5B=Cyc_Absyn_Rel_n(0);_tmp614->f1=_tmpB5B;}),({struct _fat_ptr*_tmpB5A=({struct _fat_ptr*_tmp613=_cycalloc(sizeof(*_tmp613));({struct _fat_ptr _tmpB59=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp613=_tmpB59;});_tmp613;});_tmp614->f2=_tmpB5A;});_tmp614;}));
goto _LL0;case 541:
# 2846 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 542:
# 2851 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 543:
# 2852 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 544:
# 2855
 yyval=(yyyvsp[0]).v;
goto _LL0;case 545:
# 2856 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 546:
# 2861 "parse.y"
 goto _LL0;case 547:
# 2861 "parse.y"
 yylex_buf->lex_curr_pos -=1;
goto _LL0;default:
# 2865
 goto _LL0;}_LL0:;}
# 375 "cycbison.simple"
yyvsp_offset -=yylen;
yyssp_offset -=yylen;
# 389 "cycbison.simple"
(*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))).v=yyval;
# 392
if(yylen == 0){
struct Cyc_Yystacktype*p=(struct Cyc_Yystacktype*)_check_null(_untag_fat_ptr(_fat_ptr_plus(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset - 1),sizeof(struct Cyc_Yystacktype),2U));
((p[1]).l).first_line=yylloc.first_line;
((p[1]).l).first_column=yylloc.first_column;
((p[1]).l).last_line=((p[0]).l).last_line;
((p[1]).l).last_column=((p[0]).l).last_column;}else{
# 399
((((struct Cyc_Yystacktype*)yyvs.curr)[yyvsp_offset]).l).last_line=((((struct Cyc_Yystacktype*)yyvs.curr)[(yyvsp_offset + yylen)- 1]).l).last_line;
((((struct Cyc_Yystacktype*)yyvs.curr)[yyvsp_offset]).l).last_column=((((struct Cyc_Yystacktype*)yyvs.curr)[(yyvsp_offset + yylen)- 1]).l).last_column;}
# 409
yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yyr1,548U,sizeof(short),yyn));
# 411
yystate=({int _tmpB5C=(int)*((short*)_check_known_subscript_notnull(Cyc_yypgoto,167U,sizeof(short),yyn - 153));_tmpB5C + (int)*((short*)_check_fat_subscript(yyss,sizeof(short),yyssp_offset));});
if((yystate >= 0 && yystate <= 6508)&&(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,6509U,sizeof(short),yystate))== (int)((short*)yyss.curr)[yyssp_offset])
yystate=(int)*((short*)_check_known_subscript_notnull(Cyc_yytable,6509U,sizeof(short),yystate));else{
# 415
yystate=(int)*((short*)_check_known_subscript_notnull(Cyc_yydefgoto,167U,sizeof(short),yyn - 153));}
# 417
goto yynewstate;
# 419
yyerrlab:
# 421
 if(yyerrstatus == 0){
# 424
++ yynerrs;
# 427
yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1067U,sizeof(short),yystate));
# 429
if(yyn > -32768 && yyn < 6508){
# 431
int sze=0;
struct _fat_ptr msg;
int x;int count;
# 435
count=0;
# 437
for(x=yyn < 0?- yyn: 0;(unsigned)x < 320U / sizeof(char*);++ x){
# 439
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,6509U,sizeof(short),x + yyn))== x)
({unsigned long _tmpB5D=Cyc_strlen((struct _fat_ptr)*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,320U,sizeof(struct _fat_ptr),x)))+ 15U;sze +=_tmpB5D;}),count ++;}
msg=({unsigned _tmp616=(unsigned)(sze + 15)+ 1U;char*_tmp615=({struct _RegionHandle*_tmpB5E=yyregion;_region_malloc(_tmpB5E,_check_times(_tmp616,sizeof(char)));});({{unsigned _tmp6DC=(unsigned)(sze + 15);unsigned i;for(i=0;i < _tmp6DC;++ i){_tmp615[i]='\000';}_tmp615[_tmp6DC]=0;}0;});_tag_fat(_tmp615,sizeof(char),_tmp616);});
({struct _fat_ptr _tmpB5F=msg;Cyc_strcpy(_tmpB5F,({const char*_tmp617="parse error";_tag_fat(_tmp617,sizeof(char),12U);}));});
# 444
if(count < 5){
# 446
count=0;
for(x=yyn < 0?- yyn: 0;(unsigned)x < 320U / sizeof(char*);++ x){
# 449
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,6509U,sizeof(short),x + yyn))== x){
# 451
({struct _fat_ptr _tmpB60=msg;Cyc_strcat(_tmpB60,(struct _fat_ptr)(count == 0?(struct _fat_ptr)({const char*_tmp618=", expecting `";_tag_fat(_tmp618,sizeof(char),14U);}):(struct _fat_ptr)({const char*_tmp619=" or `";_tag_fat(_tmp619,sizeof(char),6U);})));});
# 454
Cyc_strcat(msg,(struct _fat_ptr)*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,320U,sizeof(struct _fat_ptr),x)));
({struct _fat_ptr _tmpB61=msg;Cyc_strcat(_tmpB61,({const char*_tmp61A="'";_tag_fat(_tmp61A,sizeof(char),2U);}));});
++ count;}}}
# 459
Cyc_yyerror((struct _fat_ptr)msg,yystate,yychar);}else{
# 463
({struct _fat_ptr _tmpB63=({const char*_tmp61B="parse error";_tag_fat(_tmp61B,sizeof(char),12U);});int _tmpB62=yystate;Cyc_yyerror(_tmpB63,_tmpB62,yychar);});}}
# 465
goto yyerrlab1;
# 467
yyerrlab1:
# 469
 if(yyerrstatus == 3){
# 474
if(yychar == 0){
int _tmp61C=1;_npop_handler(0);return _tmp61C;}
# 483
yychar=-2;}
# 489
yyerrstatus=3;
# 491
goto yyerrhandle;
# 493
yyerrdefault:
# 503 "cycbison.simple"
 yyerrpop:
# 505
 if(yyssp_offset == 0){int _tmp61D=1;_npop_handler(0);return _tmp61D;}
-- yyvsp_offset;
yystate=(int)((short*)yyss.curr)[-- yyssp_offset];
# 521 "cycbison.simple"
yyerrhandle:
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1067U,sizeof(short),yystate));
if(yyn == -32768)goto yyerrdefault;
# 525
yyn +=1;
if((yyn < 0 || yyn > 6508)||(int)Cyc_yycheck[yyn]!= 1)goto yyerrdefault;
# 528
yyn=(int)Cyc_yytable[yyn];
if(yyn < 0){
# 531
if(yyn == -32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 535
if(yyn == 0)goto yyerrpop;}
# 537
if(yyn == 1066){
int _tmp61E=0;_npop_handler(0);return _tmp61E;}
# 546
({struct Cyc_Yystacktype _tmpB64=({struct Cyc_Yystacktype _tmp6DD;_tmp6DD.v=yylval,_tmp6DD.l=yylloc;_tmp6DD;});((struct Cyc_Yystacktype*)yyvs.curr)[++ yyvsp_offset]=_tmpB64;});
# 551
goto yynewstate;}
# 149 "cycbison.simple"
;_pop_region();}
# 2864 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp622=v;void*_tmp623;struct _fat_ptr _tmp624;char _tmp625;union Cyc_Absyn_Cnst _tmp626;switch((_tmp622.Stmt_tok).tag){case 1: _tmp626=(_tmp622.Int_tok).val;{union Cyc_Absyn_Cnst c=_tmp626;
({struct Cyc_String_pa_PrintArg_struct _tmp629=({struct Cyc_String_pa_PrintArg_struct _tmp6E0;_tmp6E0.tag=0,({struct _fat_ptr _tmpB65=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_cnst2string(c));_tmp6E0.f1=_tmpB65;});_tmp6E0;});void*_tmp627[1];_tmp627[0]=& _tmp629;({struct Cyc___cycFILE*_tmpB67=Cyc_stderr;struct _fat_ptr _tmpB66=({const char*_tmp628="%s";_tag_fat(_tmp628,sizeof(char),3U);});Cyc_fprintf(_tmpB67,_tmpB66,_tag_fat(_tmp627,sizeof(void*),1));});});goto _LL0;}case 2: _tmp625=(_tmp622.Char_tok).val;{char c=_tmp625;
({struct Cyc_Int_pa_PrintArg_struct _tmp62C=({struct Cyc_Int_pa_PrintArg_struct _tmp6E1;_tmp6E1.tag=1,_tmp6E1.f1=(unsigned long)((int)c);_tmp6E1;});void*_tmp62A[1];_tmp62A[0]=& _tmp62C;({struct Cyc___cycFILE*_tmpB69=Cyc_stderr;struct _fat_ptr _tmpB68=({const char*_tmp62B="%c";_tag_fat(_tmp62B,sizeof(char),3U);});Cyc_fprintf(_tmpB69,_tmpB68,_tag_fat(_tmp62A,sizeof(void*),1));});});goto _LL0;}case 3: _tmp624=(_tmp622.String_tok).val;{struct _fat_ptr s=_tmp624;
({struct Cyc_String_pa_PrintArg_struct _tmp62F=({struct Cyc_String_pa_PrintArg_struct _tmp6E2;_tmp6E2.tag=0,_tmp6E2.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp6E2;});void*_tmp62D[1];_tmp62D[0]=& _tmp62F;({struct Cyc___cycFILE*_tmpB6B=Cyc_stderr;struct _fat_ptr _tmpB6A=({const char*_tmp62E="\"%s\"";_tag_fat(_tmp62E,sizeof(char),5U);});Cyc_fprintf(_tmpB6B,_tmpB6A,_tag_fat(_tmp62D,sizeof(void*),1));});});goto _LL0;}case 4: _tmp623=(_tmp622.QualId_tok).val;{struct _tuple0*q=_tmp623;
({struct Cyc_String_pa_PrintArg_struct _tmp632=({struct Cyc_String_pa_PrintArg_struct _tmp6E3;_tmp6E3.tag=0,({struct _fat_ptr _tmpB6C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(q));_tmp6E3.f1=_tmpB6C;});_tmp6E3;});void*_tmp630[1];_tmp630[0]=& _tmp632;({struct Cyc___cycFILE*_tmpB6E=Cyc_stderr;struct _fat_ptr _tmpB6D=({const char*_tmp631="%s";_tag_fat(_tmp631,sizeof(char),3U);});Cyc_fprintf(_tmpB6E,_tmpB6D,_tag_fat(_tmp630,sizeof(void*),1));});});goto _LL0;}case 5: _tmp623=(_tmp622.Exp_tok).val;{struct Cyc_Absyn_Exp*e=_tmp623;
({struct Cyc_String_pa_PrintArg_struct _tmp635=({struct Cyc_String_pa_PrintArg_struct _tmp6E4;_tmp6E4.tag=0,({struct _fat_ptr _tmpB6F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp6E4.f1=_tmpB6F;});_tmp6E4;});void*_tmp633[1];_tmp633[0]=& _tmp635;({struct Cyc___cycFILE*_tmpB71=Cyc_stderr;struct _fat_ptr _tmpB70=({const char*_tmp634="%s";_tag_fat(_tmp634,sizeof(char),3U);});Cyc_fprintf(_tmpB71,_tmpB70,_tag_fat(_tmp633,sizeof(void*),1));});});goto _LL0;}case 6: _tmp623=(_tmp622.Stmt_tok).val;{struct Cyc_Absyn_Stmt*s=_tmp623;
({struct Cyc_String_pa_PrintArg_struct _tmp638=({struct Cyc_String_pa_PrintArg_struct _tmp6E5;_tmp6E5.tag=0,({struct _fat_ptr _tmpB72=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_stmt2string(s));_tmp6E5.f1=_tmpB72;});_tmp6E5;});void*_tmp636[1];_tmp636[0]=& _tmp638;({struct Cyc___cycFILE*_tmpB74=Cyc_stderr;struct _fat_ptr _tmpB73=({const char*_tmp637="%s";_tag_fat(_tmp637,sizeof(char),3U);});Cyc_fprintf(_tmpB74,_tmpB73,_tag_fat(_tmp636,sizeof(void*),1));});});goto _LL0;}default:
({void*_tmp639=0U;({struct Cyc___cycFILE*_tmpB76=Cyc_stderr;struct _fat_ptr _tmpB75=({const char*_tmp63A="?";_tag_fat(_tmp63A,sizeof(char),2U);});Cyc_fprintf(_tmpB76,_tmpB75,_tag_fat(_tmp639,sizeof(void*),0));});});goto _LL0;}_LL0:;}
# 2876
struct _fat_ptr Cyc_token2string(int token){
if(token <= 0)
return({const char*_tmp63B="end-of-file";_tag_fat(_tmp63B,sizeof(char),12U);});
if(token == 368)
return Cyc_Lex_token_string;
if(token == 377)
return Cyc_Absynpp_qvar2string(Cyc_Lex_token_qvar);{
int z=token > 0 && token <= 380?(int)Cyc_yytranslate[token]: 320;
if((unsigned)z < 320U)
return Cyc_yytname[z];
return _tag_fat(0,0,0);}}
# 2890
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp63C=_new_region("yyr");struct _RegionHandle*yyr=& _tmp63C;_push_region(yyr);
({struct _RegionHandle*_tmpB77=yyr;Cyc_yyparse(_tmpB77,Cyc_Lexing_from_file(f));});{
struct Cyc_List_List*_tmp63D=Cyc_Parse_parse_result;_npop_handler(0);return _tmp63D;}
# 2893
;_pop_region();}}
