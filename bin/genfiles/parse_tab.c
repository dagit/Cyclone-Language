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
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=msg;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(loc,_tag_fat(_Tmp1,sizeof(void*),1));});
(int)_throw((void*)({struct Cyc_Parse_Exit_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Parse_Exit_exn_struct));_Tmp0->tag=Cyc_Parse_Exit;_Tmp0;}));}
# 179
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned loc){
# 181
struct _tuple8*_Tmp0=tqt;void*_Tmp1;struct Cyc_Absyn_Tqual _Tmp2;_Tmp2=_Tmp0->f2;_Tmp1=_Tmp0->f3;{struct Cyc_Absyn_Tqual tq=_Tmp2;void*t=_Tmp1;
if((tq.print_const || tq.q_volatile)|| tq.q_restrict){
if(tq.loc != 0U)loc=tq.loc;
Cyc_Warn_warn(loc,({const char*_Tmp3="qualifier on type is ignored";_tag_fat(_Tmp3,sizeof(char),29U);}),_tag_fat(0U,sizeof(void*),0));}
# 186
return t;}}struct _tuple15{void*f1;void*f2;void*f3;void*f4;void*f5;};
# 189
static struct _tuple15 Cyc_Parse_collapse_pointer_quals(unsigned loc,void*nullable,void*bound,void*rgn,struct Cyc_List_List*pqs){
# 195
void*zeroterm=Cyc_Tcutil_any_bool(0);
void*autoreleased=Cyc_Tcutil_any_bool(0);
for(1;pqs != 0;pqs=pqs->tl){
void*_stmttmp0=(void*)pqs->hd;void*_Tmp0=_stmttmp0;void*_Tmp1;switch(*((int*)_Tmp0)){case 4:
 zeroterm=Cyc_Absyn_true_type;goto _LL0;case 5:
 zeroterm=Cyc_Absyn_false_type;goto _LL0;case 6:
 autoreleased=Cyc_Absyn_true_type;goto _LL0;case 8:
 nullable=Cyc_Absyn_true_type;goto _LL0;case 7:
 nullable=Cyc_Absyn_false_type;goto _LL0;case 3:
 bound=Cyc_Absyn_fat_bound_type;goto _LL0;case 2:
 bound=Cyc_Absyn_bounds_one();goto _LL0;case 0: _Tmp1=((struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp1;
bound=Cyc_Absyn_thin_bounds_exp(e);goto _LL0;}default: _Tmp1=(void*)((struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*)_Tmp0)->f1;{void*t=_Tmp1;
rgn=t;goto _LL0;}}_LL0:;}
# 209
return({struct _tuple15 _Tmp0;_Tmp0.f1=nullable,_Tmp0.f2=bound,_Tmp0.f3=zeroterm,_Tmp0.f4=rgn,_Tmp0.f5=autoreleased;_Tmp0;});}
# 215
static struct _tuple0*Cyc_Parse_gensym_enum (void){
# 217
static int enum_counter=0;
return({struct _tuple0*_Tmp0=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp1=Cyc_Absyn_Rel_n(0);_Tmp0->f1=_Tmp1;}),({
struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)enum_counter ++;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(({const char*_Tmp6="__anonymous_enum_%d__";_tag_fat(_Tmp6,sizeof(char),22U);}),_tag_fat(_Tmp5,sizeof(void*),1));});*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->f2=_Tmp1;});_Tmp0;});}struct _tuple16{unsigned f1;struct _tuple0*f2;struct Cyc_Absyn_Tqual f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct _tuple17{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple18{struct _tuple16*f1;struct _tuple17*f2;};
# 222
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned loc,struct _tuple18*field_info){
# 227
struct _tuple18*_Tmp0=field_info;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;struct Cyc_Absyn_Tqual _Tmp6;void*_Tmp7;unsigned _Tmp8;_Tmp8=(_Tmp0->f1)->f1;_Tmp7=(_Tmp0->f1)->f2;_Tmp6=(_Tmp0->f1)->f3;_Tmp5=(_Tmp0->f1)->f4;_Tmp4=(_Tmp0->f1)->f5;_Tmp3=(_Tmp0->f1)->f6;_Tmp2=(_Tmp0->f2)->f1;_Tmp1=(_Tmp0->f2)->f2;{unsigned varloc=_Tmp8;struct _tuple0*qid=_Tmp7;struct Cyc_Absyn_Tqual tq=_Tmp6;void*t=_Tmp5;struct Cyc_List_List*tvs=_Tmp4;struct Cyc_List_List*atts=_Tmp3;struct Cyc_Absyn_Exp*widthopt=_Tmp2;struct Cyc_Absyn_Exp*reqopt=_Tmp1;
if(tvs != 0)
Cyc_Warn_err(loc,({const char*_Tmp9="bad type params in struct field";_tag_fat(_Tmp9,sizeof(char),32U);}),_tag_fat(0U,sizeof(void*),0));
if(Cyc_Absyn_is_qvar_qualified(qid))
Cyc_Warn_err(loc,({const char*_Tmp9="struct or union field cannot be qualified with a namespace";_tag_fat(_Tmp9,sizeof(char),59U);}),_tag_fat(0U,sizeof(void*),0));
return({struct Cyc_Absyn_Aggrfield*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_Tmp9->name=(*qid).f2,_Tmp9->tq=tq,_Tmp9->type=t,_Tmp9->width=widthopt,_Tmp9->attributes=atts,_Tmp9->requires_clause=reqopt;_Tmp9;});}}
# 237
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned loc){
return({struct Cyc_Parse_Type_specifier _Tmp0;_Tmp0.Signed_spec=0,_Tmp0.Unsigned_spec=0,_Tmp0.Short_spec=0,_Tmp0.Long_spec=0,_Tmp0.Long_Long_spec=0,_Tmp0.Valid_type_spec=0,_Tmp0.Type_spec=Cyc_Absyn_sint_type,_Tmp0.loc=loc;_Tmp0;});}
# 248
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned loc){
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Type_spec=t;
s.Valid_type_spec=1;
return s;}
# 254
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned loc){
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Signed_spec=1;
return s;}
# 259
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned loc){
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Unsigned_spec=1;
return s;}
# 264
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned loc){
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Short_spec=1;
return s;}
# 269
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned loc){
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Long_spec=1;
return s;}
# 276
static void*Cyc_Parse_array2ptr(void*t,int argposn){
# 278
return Cyc_Tcutil_is_array_type(t)?({
void*_Tmp0=t;Cyc_Tcutil_promote_array(_Tmp0,argposn?Cyc_Absyn_new_evar(& Cyc_Kinds_rko,0): Cyc_Absyn_heap_rgn_type,0);}): t;}struct _tuple19{struct _fat_ptr*f1;void*f2;};
# 291 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*res=0;
for(1;x != 0;x=x->tl){
struct _tuple8*_stmttmp1=(struct _tuple8*)x->hd;struct _tuple8*_Tmp0=_stmttmp1;struct _fat_ptr _Tmp1;void*_Tmp2;void*_Tmp3;if(*((int*)((struct _tuple8*)_Tmp0)->f3)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f3)->f1)== 4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f3)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f3)->f2)->tl == 0){_Tmp3=_Tmp0->f1;_Tmp2=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0->f3)->f2)->hd;if((struct _fat_ptr*)_Tmp3 != 0){struct _fat_ptr*v=_Tmp3;void*i=_Tmp2;
# 296
{void*_Tmp4=i;void*_Tmp5;if(*((int*)_Tmp4)== 1){_Tmp5=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp4)->f2;{void**z=(void**)_Tmp5;
# 300
struct _fat_ptr*nm=({struct _fat_ptr*_Tmp6=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=*v;_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_aprintf(({const char*_TmpA="`%s";_tag_fat(_TmpA,sizeof(char),4U);}),_tag_fat(_Tmp9,sizeof(void*),1));});*_Tmp6=_Tmp7;});_Tmp6;});
({void*_Tmp6=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_Tmp7->name=nm,_Tmp7->identity=-1,({void*_Tmp8=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_Tmp9->tag=0,_Tmp9->f1=& Cyc_Kinds_ik;_Tmp9;});_Tmp7->kind=_Tmp8;});_Tmp7;}));*z=_Tmp6;});
goto _LL7;}}else{
goto _LL7;}_LL7:;}
# 305
res=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple19*_Tmp5=({struct _tuple19*_Tmp6=_cycalloc(sizeof(struct _tuple19));_Tmp6->f1=v,_Tmp6->f2=i;_Tmp6;});_Tmp4->hd=_Tmp5;}),_Tmp4->tl=res;_Tmp4;});goto _LL0;}else{if(((struct _tuple8*)_Tmp0)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_Tmp0)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_Tmp0)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_Tmp0)->f1 != 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f3)->f1)== 3){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f3)->f2 != 0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f3)->f2)->hd)== 1){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f3)->f2)->tl == 0){_Tmp1=*_Tmp0->f1;_Tmp3=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0->f3)->f2)->hd)->f2;{struct _fat_ptr v=_Tmp1;void**z=(void**)_Tmp3;
# 309
struct _fat_ptr*nm=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=v;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(({const char*_Tmp8="`%s";_tag_fat(_Tmp8,sizeof(char),4U);}),_tag_fat(_Tmp7,sizeof(void*),1));});*_Tmp4=_Tmp5;});_Tmp4;});
({void*_Tmp4=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_Tmp5->name=nm,_Tmp5->identity=-1,({void*_Tmp6=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_Tmp7->tag=0,_Tmp7->f1=& Cyc_Kinds_rk;_Tmp7;});_Tmp5->kind=_Tmp6;});_Tmp5;}));*z=_Tmp4;});
goto _LL0;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}}else{if(((struct _tuple8*)_Tmp0)->f1 != 0)goto _LL5;else{_LL5:
 goto _LL0;}}_LL0:;}
# 314
return res;}
# 318
static struct Cyc_List_List*Cyc_Parse_get_aggrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*res=0;
for(1;x != 0;x=x->tl){
void*_stmttmp2=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_Tmp0=_stmttmp2;void*_Tmp1;if(*((int*)_Tmp0)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)== 4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2)->tl == 0){_Tmp1=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2)->hd;{void*i=_Tmp1;
# 323
res=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple19*_Tmp3=({struct _tuple19*_Tmp4=_cycalloc(sizeof(struct _tuple19));_Tmp4->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,_Tmp4->f2=i;_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=res;_Tmp2;});goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}
# 326
return res;}
# 330
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_stmttmp3=e->r;void*_Tmp0=_stmttmp3;void*_Tmp1;if(*((int*)_Tmp0)== 1){if(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)== 0){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1)->f1).Rel_n).tag == 1){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1)->f1).Rel_n).val == 0){_Tmp1=(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1)->f2;{struct _fat_ptr*y=_Tmp1;
# 333
{struct Cyc_List_List*ts=tags;for(0;ts != 0;ts=ts->tl){
struct _tuple19*_stmttmp4=(struct _tuple19*)ts->hd;struct _tuple19*_Tmp2=_stmttmp4;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2->f1;_Tmp3=_Tmp2->f2;{struct _fat_ptr*x=_Tmp4;void*i=_Tmp3;
if(Cyc_strptrcmp(x,y)== 0)
return({void*_Tmp5=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct));_Tmp6->tag=39,({void*_Tmp7=Cyc_Tcutil_copy_type(i);_Tmp6->f1=_Tmp7;});_Tmp6;});Cyc_Absyn_new_exp(_Tmp5,e->loc);});}}}
# 338
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}
# 341
return e;}
# 346
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_Tmp0=t;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;unsigned _Tmp5;void*_Tmp6;void*_Tmp7;struct Cyc_Absyn_Tqual _Tmp8;void*_Tmp9;switch(*((int*)_Tmp0)){case 4: _Tmp9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).elt_type;_Tmp8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).tq;_Tmp7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).num_elts;_Tmp6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).zero_term;_Tmp5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).zt_loc;{void*et=_Tmp9;struct Cyc_Absyn_Tqual tq=_Tmp8;struct Cyc_Absyn_Exp*nelts=_Tmp7;void*zt=_Tmp6;unsigned ztloc=_Tmp5;
# 349
struct Cyc_Absyn_Exp*nelts2=nelts;
if(nelts != 0)
nelts2=Cyc_Parse_substitute_tags_exp(tags,nelts);{
void*et2=Cyc_Parse_substitute_tags(tags,et);
if(nelts != nelts2 || et != et2)
return Cyc_Absyn_array_type(et2,tq,nelts2,zt,ztloc);
goto _LL0;}}case 3: _Tmp9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).elt_type;_Tmp8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).elt_tq;_Tmp7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).ptr_atts).rgn;_Tmp6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).ptr_atts).nullable;_Tmp4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).ptr_atts).bounds;_Tmp3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).ptr_atts).zero_term;_Tmp2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).ptr_atts).ptrloc;_Tmp1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1).ptr_atts).autoreleased;{void*et=_Tmp9;struct Cyc_Absyn_Tqual tq=_Tmp8;void*r=_Tmp7;void*n=_Tmp6;void*b=_Tmp4;void*zt=_Tmp3;struct Cyc_Absyn_PtrLoc*pl=_Tmp2;void*rel=_Tmp1;
# 357
void*et2=Cyc_Parse_substitute_tags(tags,et);
void*b2=Cyc_Parse_substitute_tags(tags,b);
if(et2 != et || b2 != b)
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _TmpA;_TmpA.elt_type=et2,_TmpA.elt_tq=tq,(_TmpA.ptr_atts).rgn=r,(_TmpA.ptr_atts).nullable=n,(_TmpA.ptr_atts).bounds=b2,(_TmpA.ptr_atts).zero_term=zt,(_TmpA.ptr_atts).ptrloc=pl,(_TmpA.ptr_atts).autoreleased=rel;_TmpA;}));
goto _LL0;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)== 13){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2)->tl == 0){_Tmp9=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2)->hd;{void*t=_Tmp9;
# 363
void*t2=Cyc_Parse_substitute_tags(tags,t);
if(t != t2)return Cyc_Absyn_thin_bounds_type(t2);
goto _LL0;}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 9: _Tmp9=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp9;
# 367
struct Cyc_Absyn_Exp*e2=Cyc_Parse_substitute_tags_exp(tags,e);
if(e2 != e)return Cyc_Absyn_valueof_type(e2);
goto _LL0;}default: _LL9:
 goto _LL0;}_LL0:;}
# 372
return t;}
# 377
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
({void*_Tmp0=Cyc_Parse_substitute_tags(tags,x->type);x->type=_Tmp0;});}struct _tuple20{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple21{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 384
static struct _tuple20*Cyc_Parse_get_tqual_typ(unsigned loc,struct _tuple21*t){
return({struct _tuple20*_Tmp0=_cycalloc(sizeof(struct _tuple20));_Tmp0->f1=(*t).f2,_Tmp0->f2=(*t).f3;_Tmp0;});}
# 388
static int Cyc_Parse_is_typeparam(void*tm){
void*_Tmp0=tm;if(*((int*)_Tmp0)== 4)
return 1;else{
return 0;};}
# 397
static void*Cyc_Parse_id2type(struct _fat_ptr s,void*k){
if(Cyc_zstrcmp(s,({const char*_Tmp0="`H";_tag_fat(_Tmp0,sizeof(char),3U);}))== 0)
return Cyc_Absyn_heap_rgn_type;
if(Cyc_zstrcmp(s,({const char*_Tmp0="`U";_tag_fat(_Tmp0,sizeof(char),3U);}))== 0)
return Cyc_Absyn_unique_rgn_type;
if(Cyc_zstrcmp(s,({const char*_Tmp0="`RC";_tag_fat(_Tmp0,sizeof(char),4U);}))== 0)
return Cyc_Absyn_refcnt_rgn_type;
if(Cyc_zstrcmp(s,Cyc_CurRgn_curr_rgn_name)== 0)
return Cyc_CurRgn_curr_rgn_type();
return Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=s;_Tmp2;});_Tmp0->name=_Tmp1;}),_Tmp0->identity=-1,_Tmp0->kind=k;_Tmp0;}));}
# 409
static void Cyc_Parse_tvar_ok(struct _fat_ptr s,unsigned loc){
if(Cyc_zstrcmp(s,({const char*_Tmp0="`H";_tag_fat(_Tmp0,sizeof(char),3U);}))== 0)
Cyc_Warn_err(loc,({const char*_Tmp0="bad occurrence of heap region";_tag_fat(_Tmp0,sizeof(char),30U);}),_tag_fat(0U,sizeof(void*),0));
if(Cyc_zstrcmp(s,({const char*_Tmp0="`U";_tag_fat(_Tmp0,sizeof(char),3U);}))== 0)
Cyc_Warn_err(loc,({const char*_Tmp0="bad occurrence of unique region";_tag_fat(_Tmp0,sizeof(char),32U);}),_tag_fat(0U,sizeof(void*),0));
if(Cyc_zstrcmp(s,({const char*_Tmp0="`RC";_tag_fat(_Tmp0,sizeof(char),4U);}))== 0)
Cyc_Warn_err(loc,({const char*_Tmp0="bad occurrence of refcounted region";_tag_fat(_Tmp0,sizeof(char),36U);}),_tag_fat(0U,sizeof(void*),0));
if(Cyc_zstrcmp(s,Cyc_CurRgn_curr_rgn_name)== 0)
Cyc_Warn_err(loc,({const char*_Tmp0="bad occurrence of \"current\" region";_tag_fat(_Tmp0,sizeof(char),35U);}),_tag_fat(0U,sizeof(void*),0));}
# 424
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned loc,void*t){
void*_Tmp0=t;void*_Tmp1;if(*((int*)_Tmp0)== 2){_Tmp1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Tvar*pr=_Tmp1;
return pr;}}else{
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(loc,({const char*_Tmp2="expecting a list of type variables, not types";_tag_fat(_Tmp2,sizeof(char),46U);}));};}
# 432
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_stmttmp5=Cyc_Absyn_compress(t);void*_Tmp0=_stmttmp5;void*_Tmp1;if(*((int*)_Tmp0)== 2){_Tmp1=(void**)&(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1)->kind;{void**cptr=(void**)_Tmp1;
# 435
void*_stmttmp6=Cyc_Kinds_compress_kb(*cptr);void*_Tmp2=_stmttmp6;if(*((int*)_Tmp2)== 1){
# 437
({void*_Tmp3=leq?(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_Tmp4->tag=2,_Tmp4->f1=0,_Tmp4->f2=k;_Tmp4;}): Cyc_Kinds_kind_to_bound(k);*cptr=_Tmp3;});
return;}else{
return;};}}else{
# 441
return;};}
# 446
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned loc){
# 454
if(tms == 0)return 0;{
# 456
void*_stmttmp7=(void*)tms->hd;void*_Tmp0=_stmttmp7;void*_Tmp1;if(*((int*)_Tmp0)== 3){_Tmp1=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;{void*args=_Tmp1;
# 459
if(tms->tl == 0 ||
 Cyc_Parse_is_typeparam((void*)(_check_null(tms->tl))->hd)&&(_check_null(tms->tl))->tl == 0){
# 462
void*_Tmp2=args;void*_Tmp3;if(*((int*)_Tmp2)== 1){
# 464
Cyc_Warn_warn(loc,({const char*_Tmp4="function declaration with both new- and old-style parameter declarations; ignoring old-style";_tag_fat(_Tmp4,sizeof(char),93U);}),_tag_fat(0U,sizeof(void*),0));
# 466
return tms;}else{_Tmp3=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_Tmp2)->f1;{struct Cyc_List_List*ids=_Tmp3;
# 468
if(({int _Tmp4=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(ids);_Tmp4 != ({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(tds);}))
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(loc,({const char*_Tmp4="wrong number of parameter declarations in old-style function declaration";_tag_fat(_Tmp4,sizeof(char),73U);}));{
# 472
struct Cyc_List_List*rev_new_params=0;
for(1;ids != 0;ids=ids->tl){
struct Cyc_List_List*tds2=tds;
for(1;tds2 != 0;tds2=tds2->tl){
struct Cyc_Absyn_Decl*x=(struct Cyc_Absyn_Decl*)tds2->hd;
void*_stmttmp8=x->r;void*_Tmp4=_stmttmp8;void*_Tmp5;if(*((int*)_Tmp4)== 0){_Tmp5=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp4)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp5;
# 479
if(Cyc_zstrptrcmp((*vd->name).f2,(struct _fat_ptr*)ids->hd)!= 0)
continue;
if(vd->initializer != 0)
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(x->loc,({const char*_Tmp6="initializer found in parameter declaration";_tag_fat(_Tmp6,sizeof(char),43U);}));
if(Cyc_Absyn_is_qvar_qualified(vd->name))
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(x->loc,({const char*_Tmp6="namespaces forbidden in parameter declarations";_tag_fat(_Tmp6,sizeof(char),47U);}));
rev_new_params=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple8*_Tmp7=({struct _tuple8*_Tmp8=_cycalloc(sizeof(struct _tuple8));_Tmp8->f1=(*vd->name).f2,_Tmp8->f2=vd->tq,_Tmp8->f3=vd->type;_Tmp8;});_Tmp6->hd=_Tmp7;}),_Tmp6->tl=rev_new_params;_Tmp6;});
# 487
goto L;}}else{
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(x->loc,({const char*_Tmp6="nonvariable declaration in parameter type";_tag_fat(_Tmp6,sizeof(char),42U);}));};}
# 491
L: if(tds2 == 0)
({int(*_Tmp4)(unsigned,struct _fat_ptr)=({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;});unsigned _Tmp5=loc;_Tmp4(_Tmp5,Cyc_strconcat(*((struct _fat_ptr*)ids->hd),({const char*_Tmp6=" is not given a type";_tag_fat(_Tmp6,sizeof(char),21U);})));});}
# 494
return({struct Cyc_List_List*_Tmp4=_region_malloc(yy,sizeof(struct Cyc_List_List));
({void*_Tmp5=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp6=_region_malloc(yy,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp6->tag=3,({void*_Tmp7=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_Tmp8=_region_malloc(yy,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_Tmp8->tag=1,({struct Cyc_List_List*_Tmp9=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(rev_new_params);_Tmp8->f1=_Tmp9;}),_Tmp8->f2=0,_Tmp8->f3=0,_Tmp8->f4=0,_Tmp8->f5=0,_Tmp8->f6=0,_Tmp8->f7=0;_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;});_Tmp4->hd=_Tmp5;}),_Tmp4->tl=0;_Tmp4;});}}};}
# 501
goto _LL4;}}else{_LL4:
 return({struct Cyc_List_List*_Tmp2=_region_malloc(yy,sizeof(struct Cyc_List_List));_Tmp2->hd=(void*)tms->hd,({struct Cyc_List_List*_Tmp3=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc);_Tmp2->tl=_Tmp3;});_Tmp2;});};}}
# 509
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned loc){
# 513
if(tds != 0)
d=({struct Cyc_Parse_Declarator _Tmp0;_Tmp0.id=d.id,_Tmp0.varloc=d.varloc,({struct Cyc_List_List*_Tmp1=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc);_Tmp0.tms=_Tmp1;});_Tmp0;});{
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
enum Cyc_Parse_Storage_class _stmttmp9=dso->sc;enum Cyc_Parse_Storage_class _Tmp0=_stmttmp9;switch((int)_Tmp0){case Cyc_Parse_None_sc:
 goto _LL0;case Cyc_Parse_Extern_sc:
 sc=3U;goto _LL0;case Cyc_Parse_Static_sc:
 sc=0U;goto _LL0;default:
 Cyc_Warn_err(loc,({const char*_Tmp1="bad storage class on function";_tag_fat(_Tmp1,sizeof(char),30U);}),_tag_fat(0U,sizeof(void*),0));goto _LL0;}_LL0:;}}{
# 535
void*t=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple14 _stmttmpA=Cyc_Parse_apply_tms(tq,t,atts,d.tms);struct _tuple14 _Tmp0=_stmttmpA;void*_Tmp1;void*_Tmp2;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp0.f1;_Tmp3=_Tmp0.f2;_Tmp2=_Tmp0.f3;_Tmp1=_Tmp0.f4;{struct Cyc_Absyn_Tqual fn_tqual=_Tmp4;void*fn_type=_Tmp3;struct Cyc_List_List*x=_Tmp2;struct Cyc_List_List*out_atts=_Tmp1;
# 540
if(x != 0)
Cyc_Warn_warn(loc,({const char*_Tmp5="bad type params, ignoring";_tag_fat(_Tmp5,sizeof(char),26U);}),_tag_fat(0U,sizeof(void*),0));{
# 543
void*_Tmp5=fn_type;struct Cyc_Absyn_FnInfo _Tmp6;if(*((int*)_Tmp5)== 5){_Tmp6=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp5)->f1;{struct Cyc_Absyn_FnInfo i=_Tmp6;
# 545
{struct Cyc_List_List*args2=i.args;for(0;args2 != 0;args2=args2->tl){
if((*((struct _tuple8*)args2->hd)).f1 == 0){
Cyc_Warn_err(loc,({const char*_Tmp7="missing argument variable in function prototype";_tag_fat(_Tmp7,sizeof(char),48U);}),_tag_fat(0U,sizeof(void*),0));
({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));*_Tmp8=({const char*_Tmp9="?";_tag_fat(_Tmp9,sizeof(char),2U);});_Tmp8;});(*((struct _tuple8*)args2->hd)).f1=_Tmp7;});}}}
# 552
({struct Cyc_List_List*_Tmp7=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(i.attributes,out_atts);i.attributes=_Tmp7;});
return({struct Cyc_Absyn_Fndecl*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Fndecl));_Tmp7->sc=sc,_Tmp7->is_inline=is_inline,_Tmp7->name=d.id,_Tmp7->body=body,_Tmp7->i=i,_Tmp7->cached_type=0,_Tmp7->param_vardecls=0,_Tmp7->fn_vardecl=0,_Tmp7->orig_scope=sc;_Tmp7;});}}else{
# 556
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(loc,({const char*_Tmp7="declarator is not a function prototype";_tag_fat(_Tmp7,sizeof(char),39U);}));};}}}}}static char _TmpG0[76U]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 560
static struct _fat_ptr Cyc_Parse_msg1={_TmpG0,_TmpG0,_TmpG0 + 76U};static char _TmpG1[84U]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 562
static struct _fat_ptr Cyc_Parse_msg2={_TmpG1,_TmpG1,_TmpG1 + 84U};
# 569
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 572
if(s1.Signed_spec && s2.Signed_spec)
Cyc_Warn_warn(loc,Cyc_Parse_msg2,_tag_fat(0U,sizeof(void*),0));
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec && s2.Unsigned_spec)
Cyc_Warn_warn(loc,Cyc_Parse_msg2,_tag_fat(0U,sizeof(void*),0));
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec && s2.Short_spec)
Cyc_Warn_warn(loc,({const char*_Tmp0="too many occurrences of short in specifiers";_tag_fat(_Tmp0,sizeof(char),44U);}),_tag_fat(0U,sizeof(void*),0));
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec && s2.Long_Long_spec ||
 s1.Long_Long_spec && s2.Long_spec)||
 s2.Long_Long_spec && s1.Long_spec)
Cyc_Warn_warn(loc,({const char*_Tmp0="too many occurrences of long in specifiers";_tag_fat(_Tmp0,sizeof(char),43U);}),_tag_fat(0U,sizeof(void*),0));
s1.Long_Long_spec=
(s1.Long_Long_spec || s2.Long_Long_spec)|| s1.Long_spec && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec &&(s1.Long_spec || s2.Long_spec);
if(s1.Valid_type_spec && s2.Valid_type_spec)
Cyc_Warn_err(loc,Cyc_Parse_msg1,_tag_fat(0U,sizeof(void*),0));else{
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
Cyc_Warn_err(loc,Cyc_Parse_msg2,_tag_fat(0U,sizeof(void*),0));
if(ts.Unsigned_spec)sgn=1U;
if(ts.Short_spec &&(ts.Long_spec || ts.Long_Long_spec)||
 ts.Long_spec && ts.Long_Long_spec)
Cyc_Warn_err(loc,Cyc_Parse_msg2,_tag_fat(0U,sizeof(void*),0));
if(ts.Short_spec)sz=1U;
if(ts.Long_spec)sz=3U;
if(ts.Long_Long_spec)sz=4U;}
# 621
if(!seen_type){
if(!seen_sign && !seen_size)
Cyc_Warn_warn(loc,({const char*_Tmp0="missing type within specifier";_tag_fat(_Tmp0,sizeof(char),30U);}),_tag_fat(0U,sizeof(void*),0));
return Cyc_Absyn_int_type(sgn,sz);}
# 626
{void*_Tmp0=t;enum Cyc_Absyn_Size_of _Tmp1;enum Cyc_Absyn_Sign _Tmp2;if(*((int*)_Tmp0)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 1: _Tmp2=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;_Tmp1=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f2;{enum Cyc_Absyn_Sign sgn2=_Tmp2;enum Cyc_Absyn_Size_of sz2=_Tmp1;
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
Cyc_Warn_err(loc,({const char*_Tmp3="sign specification on non-integral type";_tag_fat(_Tmp3,sizeof(char),40U);}),_tag_fat(0U,sizeof(void*),0));
if(seen_size)
Cyc_Warn_err(loc,({const char*_Tmp3="size qualifier on non-integral type";_tag_fat(_Tmp3,sizeof(char),36U);}),_tag_fat(0U,sizeof(void*),0));
goto _LL0;}_LL0:;}
# 646
return t;}
# 649
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple13*ds,struct Cyc_List_List*shared_atts){
# 653
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*q=d.id;
unsigned varloc=d.varloc;
struct _tuple14 _stmttmpB=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);struct _tuple14 _Tmp0=_stmttmpB;void*_Tmp1;void*_Tmp2;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp0.f1;_Tmp3=_Tmp0.f2;_Tmp2=_Tmp0.f3;_Tmp1=_Tmp0.f4;{struct Cyc_Absyn_Tqual tq2=_Tmp4;void*new_typ=_Tmp3;struct Cyc_List_List*tvs=_Tmp2;struct Cyc_List_List*atts=_Tmp1;
# 660
struct Cyc_List_List*tl=ds->tl == 0?0:({struct _RegionHandle*_Tmp5=r;struct Cyc_Absyn_Tqual _Tmp6=tq;void*_Tmp7=Cyc_Tcutil_copy_type(t);struct _tuple13*_Tmp8=ds->tl;Cyc_Parse_apply_tmss(_Tmp5,_Tmp6,_Tmp7,_Tmp8,shared_atts);});
return({struct Cyc_List_List*_Tmp5=_region_malloc(r,sizeof(struct Cyc_List_List));({struct _tuple16*_Tmp6=({struct _tuple16*_Tmp7=_region_malloc(r,sizeof(struct _tuple16));_Tmp7->f1=varloc,_Tmp7->f2=q,_Tmp7->f3=tq2,_Tmp7->f4=new_typ,_Tmp7->f5=tvs,_Tmp7->f6=atts;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=tl;_Tmp5;});}}}
# 664
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 667
if(tms == 0)return({struct _tuple14 _Tmp0;_Tmp0.f1=tq,_Tmp0.f2=t,_Tmp0.f3=0,_Tmp0.f4=atts;_Tmp0;});{
void*_stmttmpC=(void*)tms->hd;void*_Tmp0=_stmttmpC;struct Cyc_Absyn_Tqual _Tmp1;struct Cyc_Absyn_PtrAtts _Tmp2;void*_Tmp3;unsigned _Tmp4;void*_Tmp5;switch(*((int*)_Tmp0)){case 0: _Tmp5=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{void*zeroterm=_Tmp5;unsigned ztloc=_Tmp4;
# 670
return({struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_empty_tqual(0U);void*_Tmp7=
Cyc_Absyn_array_type(t,tq,0,zeroterm,ztloc);
# 670
struct Cyc_List_List*_Tmp8=atts;Cyc_Parse_apply_tms(_Tmp6,_Tmp7,_Tmp8,tms->tl);});}case 1: _Tmp5=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp3=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp5;void*zeroterm=_Tmp3;unsigned ztloc=_Tmp4;
# 673
return({struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_empty_tqual(0U);void*_Tmp7=
Cyc_Absyn_array_type(t,tq,e,zeroterm,ztloc);
# 673
struct Cyc_List_List*_Tmp8=atts;Cyc_Parse_apply_tms(_Tmp6,_Tmp7,_Tmp8,tms->tl);});}case 3: _Tmp5=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;{void*args=_Tmp5;
# 676
void*_Tmp6=args;unsigned _Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;void*_TmpB;void*_TmpC;int _TmpD;void*_TmpE;if(*((int*)_Tmp6)== 1){_TmpE=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_Tmp6)->f1;_TmpD=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_Tmp6)->f2;_TmpC=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_Tmp6)->f3;_TmpB=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_Tmp6)->f4;_TmpA=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_Tmp6)->f5;_Tmp9=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_Tmp6)->f6;_Tmp8=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_Tmp6)->f7;{struct Cyc_List_List*args2=_TmpE;int c_vararg=_TmpD;struct Cyc_Absyn_VarargInfo*cyc_vararg=_TmpC;void*eff=_TmpB;struct Cyc_List_List*rgn_po=_TmpA;struct Cyc_Absyn_Exp*req=_Tmp9;struct Cyc_Absyn_Exp*ens=_Tmp8;
# 678
struct Cyc_List_List*typvars=0;
# 680
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Atts_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_TmpF=_cycalloc(sizeof(struct Cyc_List_List));_TmpF->hd=(void*)as->hd,_TmpF->tl=fn_atts;_TmpF;});else{
# 685
new_atts=({struct Cyc_List_List*_TmpF=_cycalloc(sizeof(struct Cyc_List_List));_TmpF->hd=(void*)as->hd,_TmpF->tl=new_atts;_TmpF;});}}}
# 687
if(tms->tl != 0){
void*_stmttmpD=(void*)(_check_null(tms->tl))->hd;void*_TmpF=_stmttmpD;void*_Tmp10;if(*((int*)_TmpF)== 4){_Tmp10=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_TmpF)->f1;{struct Cyc_List_List*ts=_Tmp10;
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
{struct Cyc_List_List*a=args2;for(0;a != 0;a=a->tl){
struct _tuple8*_stmttmpE=(struct _tuple8*)a->hd;struct _tuple8*_TmpF=_stmttmpE;void*_Tmp10;struct Cyc_Absyn_Tqual _Tmp11;void*_Tmp12;_Tmp12=_TmpF->f1;_Tmp11=_TmpF->f2;_Tmp10=(void**)& _TmpF->f3;{struct _fat_ptr*vopt=_Tmp12;struct Cyc_Absyn_Tqual tq=_Tmp11;void**t=(void**)_Tmp10;
void*_stmttmpF=*t;void*_Tmp13=_stmttmpF;unsigned _Tmp14;void*_Tmp15;void*_Tmp16;struct Cyc_Absyn_Tqual _Tmp17;void*_Tmp18;if(*((int*)_Tmp13)== 4){_Tmp18=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp13)->f1).elt_type;_Tmp17=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp13)->f1).tq;_Tmp16=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp13)->f1).num_elts;_Tmp15=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp13)->f1).zero_term;_Tmp14=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp13)->f1).zt_loc;{void*et=_Tmp18;struct Cyc_Absyn_Tqual tq=_Tmp17;struct Cyc_Absyn_Exp*neltsopt=_Tmp16;void*zt=_Tmp15;unsigned ztloc=_Tmp14;
# 711
if(neltsopt != 0 && vopt != 0){
struct _tuple0*v=({struct _tuple0*_Tmp19=_cycalloc(sizeof(struct _tuple0));((_Tmp19->f1).Loc_n).tag=4U,((_Tmp19->f1).Loc_n).val=0,_Tmp19->f2=vopt;_Tmp19;});
struct Cyc_Absyn_Exp*nelts=Cyc_Absyn_copy_exp(neltsopt);
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_primop_exp(18U,({struct Cyc_Absyn_Exp*_Tmp19[1];({struct Cyc_Absyn_Exp*_Tmp1A=Cyc_Absyn_var_exp(v,0U);_Tmp19[0]=_Tmp1A;});({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_Tmp19,sizeof(struct Cyc_Absyn_Exp*),1));}),0U);
struct Cyc_Absyn_Exp*new_req=Cyc_Absyn_lte_exp(nelts,e2,0U);
new_requires=({struct Cyc_List_List*_Tmp19=_cycalloc(sizeof(struct Cyc_List_List));_Tmp19->hd=new_req,_Tmp19->tl=new_requires;_Tmp19;});}
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
struct Cyc_List_List*tags=Cyc_Parse_get_arg_tags(args2);
# 738
if(tags != 0)
t=Cyc_Parse_substitute_tags(tags,t);
t=Cyc_Parse_array2ptr(t,0);
# 743
{struct Cyc_List_List*a=args2;for(0;a != 0;a=a->tl){
struct _tuple8*_stmttmp10=(struct _tuple8*)a->hd;struct _tuple8*_TmpF=_stmttmp10;void*_Tmp10;struct Cyc_Absyn_Tqual _Tmp11;void*_Tmp12;_Tmp12=_TmpF->f1;_Tmp11=_TmpF->f2;_Tmp10=(void**)& _TmpF->f3;{struct _fat_ptr*vopt=_Tmp12;struct Cyc_Absyn_Tqual tq=_Tmp11;void**t=(void**)_Tmp10;
if(tags != 0)
({void*_Tmp13=Cyc_Parse_substitute_tags(tags,*t);*t=_Tmp13;});
({void*_Tmp13=Cyc_Parse_array2ptr(*t,1);*t=_Tmp13;});}}}
# 755
return({struct Cyc_Absyn_Tqual _TmpF=Cyc_Absyn_empty_tqual(tq.loc);void*_Tmp10=
Cyc_Absyn_function_type(typvars,eff,tq,t,args2,c_vararg,cyc_vararg,rgn_po,fn_atts,req,ens);
# 755
struct Cyc_List_List*_Tmp11=new_atts;Cyc_Parse_apply_tms(_TmpF,_Tmp10,_Tmp11,(
# 760
_check_null(tms))->tl);});}}}}else{_Tmp7=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_Tmp6)->f2;{unsigned loc=_Tmp7;
# 762
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(loc,({const char*_TmpF="function declaration without parameter types";_tag_fat(_TmpF,sizeof(char),45U);}));}};}case 4: _Tmp5=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{struct Cyc_List_List*ts=_Tmp5;unsigned loc=_Tmp4;
# 769
if(tms->tl == 0)
return({struct _tuple14 _Tmp6;_Tmp6.f1=tq,_Tmp6.f2=t,_Tmp6.f3=ts,_Tmp6.f4=atts;_Tmp6;});
# 774
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(loc,({const char*_Tmp6="type parameters must appear before function arguments in declarator";_tag_fat(_Tmp6,sizeof(char),68U);}));}case 2: _Tmp2=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{struct Cyc_Absyn_PtrAtts ptratts=_Tmp2;struct Cyc_Absyn_Tqual tq2=_Tmp1;
# 777
return({struct Cyc_Absyn_Tqual _Tmp6=tq2;void*_Tmp7=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _Tmp8;_Tmp8.elt_type=t,_Tmp8.elt_tq=tq,_Tmp8.ptr_atts=ptratts;_Tmp8;}));struct Cyc_List_List*_Tmp8=atts;Cyc_Parse_apply_tms(_Tmp6,_Tmp7,_Tmp8,tms->tl);});}default: _Tmp4=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{unsigned loc=_Tmp4;struct Cyc_List_List*atts2=_Tmp5;
# 782
return({struct Cyc_Absyn_Tqual _Tmp6=tq;void*_Tmp7=t;struct Cyc_List_List*_Tmp8=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(atts,atts2);Cyc_Parse_apply_tms(_Tmp6,_Tmp7,_Tmp8,tms->tl);});}};}}
# 788
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 797
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned loc,struct _tuple16*t){
struct _tuple16*_Tmp0=t;void*_Tmp1;void*_Tmp2;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;void*_Tmp5;unsigned _Tmp6;_Tmp6=_Tmp0->f1;_Tmp5=_Tmp0->f2;_Tmp4=_Tmp0->f3;_Tmp3=_Tmp0->f4;_Tmp2=_Tmp0->f5;_Tmp1=_Tmp0->f6;{unsigned varloc=_Tmp6;struct _tuple0*x=_Tmp5;struct Cyc_Absyn_Tqual tq=_Tmp4;void*typ=_Tmp3;struct Cyc_List_List*tvs=_Tmp2;struct Cyc_List_List*atts=_Tmp1;
# 800
Cyc_Lex_register_typedef(x);{
# 802
struct Cyc_Core_Opt*kind;
void*type;
{void*_Tmp7=typ;void*_Tmp8;if(*((int*)_Tmp7)== 1){_Tmp8=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp7)->f1;{struct Cyc_Core_Opt*kopt=_Tmp8;
# 806
type=0;
kind=kopt == 0?& Cyc_Kinds_bko: kopt;
goto _LL3;}}else{
kind=0;type=typ;goto _LL3;}_LL3:;}
# 811
return({void*_Tmp7=(void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct));_Tmp8->tag=8,({struct Cyc_Absyn_Typedefdecl*_Tmp9=({struct Cyc_Absyn_Typedefdecl*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_Typedefdecl));_TmpA->name=x,_TmpA->tvs=tvs,_TmpA->kind=kind,_TmpA->defn=type,_TmpA->atts=atts,_TmpA->tq=tq,_TmpA->extern_c=0;_TmpA;});_Tmp8->f1=_Tmp9;});_Tmp8;});Cyc_Absyn_new_decl(_Tmp7,loc);});}}}
# 818
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
return({void*_Tmp0=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct));_Tmp1->tag=12,_Tmp1->f1=d,_Tmp1->f2=s;_Tmp1;});Cyc_Absyn_new_stmt(_Tmp0,d->loc);});}
# 822
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return({(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*,struct Cyc_Absyn_Stmt*))Cyc_List_fold_right;})(Cyc_Parse_flatten_decl,ds,s);}
# 826
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple11*ds,struct _tuple13**decls,struct Cyc_List_List**es){
# 829
struct _tuple13*declarators=0;
struct Cyc_List_List*exprs=0;
for(1;ds != 0;ds=ds->tl){
struct _tuple12 _stmttmp11=ds->hd;struct _tuple12 _Tmp0=_stmttmp11;void*_Tmp1;struct Cyc_Parse_Declarator _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{struct Cyc_Parse_Declarator d=_Tmp2;struct Cyc_Absyn_Exp*e=_Tmp1;
declarators=({struct _tuple13*_Tmp3=_region_malloc(r,sizeof(struct _tuple13));_Tmp3->tl=declarators,_Tmp3->hd=d;_Tmp3;});
exprs=({struct Cyc_List_List*_Tmp3=_region_malloc(r,sizeof(struct Cyc_List_List));_Tmp3->hd=e,_Tmp3->tl=exprs;_Tmp3;});}}
# 836
({struct Cyc_List_List*_Tmp0=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(exprs);*es=_Tmp0;});
({struct _tuple13*_Tmp0=({(struct _tuple13*(*)(struct _tuple13*))Cyc_Parse_flat_imp_rev;})(declarators);*decls=_Tmp0;});}
# 845
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple11*ids,unsigned tqual_loc,unsigned loc){
# 848
struct _RegionHandle _Tmp0=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _Tmp0;_push_region(mkrgn);
{struct Cyc_Parse_Declaration_spec _Tmp1=ds;void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.tq;_Tmp3=_Tmp1.type_specs;_Tmp2=_Tmp1.attributes;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tss=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
if(tq.loc == 0U)tq.loc=tqual_loc;
if(ds.is_inline)
Cyc_Warn_warn(loc,({const char*_Tmp5="inline qualifier on non-function definition";_tag_fat(_Tmp5,sizeof(char),44U);}),_tag_fat(0U,sizeof(void*),0));{
# 854
enum Cyc_Absyn_Scope s=2U;
int istypedef=0;
{enum Cyc_Parse_Storage_class _stmttmp12=ds.sc;enum Cyc_Parse_Storage_class _Tmp5=_stmttmp12;switch((int)_Tmp5){case Cyc_Parse_Typedef_sc:
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
void*base_type=Cyc_Parse_collapse_type_specifiers(tss,loc);
if(declarators == 0){
# 886
void*_Tmp5=base_type;int _Tmp6;void*_Tmp7;enum Cyc_Absyn_AggrKind _Tmp8;void*_Tmp9;switch(*((int*)_Tmp5)){case 10: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp5)->f1)->r)){case 0: _Tmp9=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp5)->f1)->r)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp9;
# 888
({struct Cyc_List_List*_TmpA=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(ad->attributes,atts);ad->attributes=_TmpA;});
ad->sc=s;{
struct Cyc_List_List*_TmpA=({struct Cyc_List_List*_TmpB=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_TmpC=({void*_TmpD=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_TmpE=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_TmpE->tag=5,_TmpE->f1=ad;_TmpE;});Cyc_Absyn_new_decl(_TmpD,loc);});_TmpB->hd=_TmpC;}),_TmpB->tl=0;_TmpB;});_npop_handler(0);return _TmpA;}}case 1: _Tmp9=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp5)->f1)->r)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp9;
# 892
if(atts != 0)Cyc_Warn_err(loc,({const char*_TmpA="attributes on enum not supported";_tag_fat(_TmpA,sizeof(char),33U);}),_tag_fat(0U,sizeof(void*),0));
ed->sc=s;{
struct Cyc_List_List*_TmpA=({struct Cyc_List_List*_TmpB=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_TmpC=({void*_TmpD=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TmpE=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_TmpE->tag=7,_TmpE->f1=ed;_TmpE;});Cyc_Absyn_new_decl(_TmpD,loc);});_TmpB->hd=_TmpC;}),_TmpB->tl=0;_TmpB;});_npop_handler(0);return _TmpA;}}default: _Tmp9=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp5)->f1)->r)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_Tmp9;
# 896
if(atts != 0)Cyc_Warn_err(loc,({const char*_TmpA="attributes on datatypes not supported";_tag_fat(_TmpA,sizeof(char),38U);}),_tag_fat(0U,sizeof(void*),0));
dd->sc=s;{
struct Cyc_List_List*_TmpA=({struct Cyc_List_List*_TmpB=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_TmpC=({void*_TmpD=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_TmpE=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_TmpE->tag=6,_TmpE->f1=dd;_TmpE;});Cyc_Absyn_new_decl(_TmpD,loc);});_TmpB->hd=_TmpC;}),_TmpB->tl=0;_TmpB;});_npop_handler(0);return _TmpA;}}}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f1)){case 20: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f1)->f1).UnknownAggr).tag == 1){_Tmp8=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f1)->f1).UnknownAggr).val).f1;_Tmp9=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f1)->f1).UnknownAggr).val).f2;_Tmp7=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f2;{enum Cyc_Absyn_AggrKind k=_Tmp8;struct _tuple0*n=_Tmp9;struct Cyc_List_List*ts=_Tmp7;
# 900
struct Cyc_List_List*ts2=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,loc,ts);
struct Cyc_Absyn_Aggrdecl*ad=({struct Cyc_Absyn_Aggrdecl*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));_TmpA->kind=k,_TmpA->sc=s,_TmpA->name=n,_TmpA->tvs=ts2,_TmpA->impl=0,_TmpA->attributes=0,_TmpA->expected_mem_kind=0;_TmpA;});
if(atts != 0)Cyc_Warn_err(loc,({const char*_TmpA="bad attributes on type declaration";_tag_fat(_TmpA,sizeof(char),35U);}),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_TmpA=({struct Cyc_List_List*_TmpB=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_TmpC=({void*_TmpD=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_TmpE=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_TmpE->tag=5,_TmpE->f1=ad;_TmpE;});Cyc_Absyn_new_decl(_TmpD,loc);});_TmpB->hd=_TmpC;}),_TmpB->tl=0;_TmpB;});_npop_handler(0);return _TmpA;}}}else{goto _LL25;}case 18: if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f1)->f1).KnownDatatype).tag == 2){_Tmp9=((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f1)->f1).KnownDatatype).val;{struct Cyc_Absyn_Datatypedecl**tudp=_Tmp9;
# 905
if(atts != 0)Cyc_Warn_err(loc,({const char*_TmpA="bad attributes on datatype";_tag_fat(_TmpA,sizeof(char),27U);}),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_TmpA=({struct Cyc_List_List*_TmpB=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_TmpC=({void*_TmpD=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_TmpE=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_TmpE->tag=6,_TmpE->f1=*tudp;_TmpE;});Cyc_Absyn_new_decl(_TmpD,loc);});_TmpB->hd=_TmpC;}),_TmpB->tl=0;_TmpB;});_npop_handler(0);return _TmpA;}}}else{_Tmp9=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f1)->f1).UnknownDatatype).val).name;_Tmp6=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f1)->f1).UnknownDatatype).val).is_extensible;_Tmp7=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f2;{struct _tuple0*n=_Tmp9;int isx=_Tmp6;struct Cyc_List_List*ts=_Tmp7;
# 908
struct Cyc_List_List*ts2=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,loc,ts);
struct Cyc_Absyn_Decl*tud=Cyc_Absyn_datatype_decl(s,n,ts2,0,isx,loc);
if(atts != 0)Cyc_Warn_err(loc,({const char*_TmpA="bad attributes on datatype";_tag_fat(_TmpA,sizeof(char),27U);}),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_TmpA=({struct Cyc_List_List*_TmpB=_cycalloc(sizeof(struct Cyc_List_List));_TmpB->hd=tud,_TmpB->tl=0;_TmpB;});_npop_handler(0);return _TmpA;}}}case 15: _Tmp9=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f1)->f1;{struct _tuple0*n=_Tmp9;
# 913
struct Cyc_Absyn_Enumdecl*ed=({struct Cyc_Absyn_Enumdecl*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));_TmpA->sc=s,_TmpA->name=n,_TmpA->fields=0;_TmpA;});
if(atts != 0)Cyc_Warn_err(loc,({const char*_TmpA="bad attributes on enum";_tag_fat(_TmpA,sizeof(char),23U);}),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_TmpA=({struct Cyc_List_List*_TmpB=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_TmpC=({struct Cyc_Absyn_Decl*_TmpD=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_TmpE=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TmpF=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_TmpF->tag=7,_TmpF->f1=ed;_TmpF;});_TmpD->r=_TmpE;}),_TmpD->loc=loc;_TmpD;});_TmpB->hd=_TmpC;}),_TmpB->tl=0;_TmpB;});_npop_handler(0);return _TmpA;}}case 16: _Tmp9=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f1)->f1;{struct Cyc_List_List*fs=_Tmp9;
# 919
struct Cyc_Absyn_Enumdecl*ed=({struct Cyc_Absyn_Enumdecl*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));_TmpA->sc=s,({struct _tuple0*_TmpB=Cyc_Parse_gensym_enum();_TmpA->name=_TmpB;}),({struct Cyc_Core_Opt*_TmpB=({struct Cyc_Core_Opt*_TmpC=_cycalloc(sizeof(struct Cyc_Core_Opt));_TmpC->v=fs;_TmpC;});_TmpA->fields=_TmpB;});_TmpA;});
if(atts != 0)Cyc_Warn_err(loc,({const char*_TmpA="bad attributes on enum";_tag_fat(_TmpA,sizeof(char),23U);}),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_TmpA=({struct Cyc_List_List*_TmpB=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_TmpC=({struct Cyc_Absyn_Decl*_TmpD=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_TmpE=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TmpF=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_TmpF->tag=7,_TmpF->f1=ed;_TmpF;});_TmpD->r=_TmpE;}),_TmpD->loc=loc;_TmpD;});_TmpB->hd=_TmpC;}),_TmpB->tl=0;_TmpB;});_npop_handler(0);return _TmpA;}}default: goto _LL25;}default: _LL25:
 Cyc_Warn_err(loc,({const char*_TmpA="missing declarator";_tag_fat(_TmpA,sizeof(char),19U);}),_tag_fat(0U,sizeof(void*),0));{struct Cyc_List_List*_TmpA=0;_npop_handler(0);return _TmpA;}};}{
# 926
struct Cyc_List_List*fields=Cyc_Parse_apply_tmss(mkrgn,tq,base_type,declarators,atts);
if(istypedef){
# 931
if(!exps_empty)
Cyc_Warn_err(loc,({const char*_Tmp5="initializer in typedef declaration";_tag_fat(_Tmp5,sizeof(char),35U);}),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*decls=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*)(unsigned,struct _tuple16*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_v_typ_to_typedef,loc,fields);
struct Cyc_List_List*_Tmp5=decls;_npop_handler(0);return _Tmp5;}}{
# 937
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*ds=fields;for(0;ds != 0;(ds=ds->tl,exprs=(_check_null(exprs))->tl)){
struct _tuple16*_stmttmp13=(struct _tuple16*)ds->hd;struct _tuple16*_Tmp5=_stmttmp13;void*_Tmp6;void*_Tmp7;void*_Tmp8;struct Cyc_Absyn_Tqual _Tmp9;void*_TmpA;unsigned _TmpB;_TmpB=_Tmp5->f1;_TmpA=_Tmp5->f2;_Tmp9=_Tmp5->f3;_Tmp8=_Tmp5->f4;_Tmp7=_Tmp5->f5;_Tmp6=_Tmp5->f6;{unsigned varloc=_TmpB;struct _tuple0*x=_TmpA;struct Cyc_Absyn_Tqual tq2=_Tmp9;void*t2=_Tmp8;struct Cyc_List_List*tvs2=_Tmp7;struct Cyc_List_List*atts2=_Tmp6;
if(tvs2 != 0)
Cyc_Warn_warn(loc,({const char*_TmpC="bad type params, ignoring";_tag_fat(_TmpC,sizeof(char),26U);}),_tag_fat(0U,sizeof(void*),0));
if(exprs == 0)
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(loc,({const char*_TmpC="unexpected NULL in parse!";_tag_fat(_TmpC,sizeof(char),26U);}));{
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(varloc,x,t2,(struct Cyc_Absyn_Exp*)(_check_null(exprs))->hd);
vd->tq=tq2;
vd->sc=s;
vd->attributes=atts2;
decls=({struct Cyc_List_List*_TmpC=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_TmpD=({struct Cyc_Absyn_Decl*_TmpE=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_TmpF=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp10=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp10->tag=0,_Tmp10->f1=vd;_Tmp10;});_TmpE->r=_TmpF;}),_TmpE->loc=loc;_TmpE;});_TmpC->hd=_TmpD;}),_TmpC->tl=decls;_TmpC;});}}}}{
# 950
struct Cyc_List_List*_Tmp5=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(decls);_npop_handler(0);return _Tmp5;}}}}}}}}}
# 849
;_pop_region();}
# 954
static unsigned Cyc_Parse_cnst2uint(unsigned loc,union Cyc_Absyn_Cnst x){
union Cyc_Absyn_Cnst _Tmp0=x;long long _Tmp1;char _Tmp2;int _Tmp3;switch((_Tmp0.LongLong_c).tag){case 5: _Tmp3=((_Tmp0.Int_c).val).f2;{int i=_Tmp3;
return(unsigned)i;}case 2: _Tmp2=((_Tmp0.Char_c).val).f2;{char c=_Tmp2;
return(unsigned)c;}case 6: _Tmp1=((_Tmp0.LongLong_c).val).f2;{long long x=_Tmp1;
# 959
unsigned long long y=(unsigned long long)x;
if(y > 4294967295U)
Cyc_Warn_err(loc,({const char*_Tmp4="integer constant too large";_tag_fat(_Tmp4,sizeof(char),27U);}),_tag_fat(0U,sizeof(void*),0));
return(unsigned)x;}default:
# 964
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_Absynpp_cnst2string(x);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_Warn_err(loc,({const char*_Tmp6="expected integer constant but found %s";_tag_fat(_Tmp6,sizeof(char),39U);}),_tag_fat(_Tmp5,sizeof(void*),1));});
return 0U;};}
# 970
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_stmttmp14=p->r;void*_Tmp0=_stmttmp14;void*_Tmp1;struct _fat_ptr _Tmp2;char _Tmp3;int _Tmp4;enum Cyc_Absyn_Sign _Tmp5;void*_Tmp6;switch(*((int*)_Tmp0)){case 15: _Tmp6=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct _tuple0*x=_Tmp6;
return Cyc_Absyn_unknownid_exp(x,p->loc);}case 3: if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2)->r)== 0){_Tmp6=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp6;
# 974
return({struct Cyc_Absyn_Exp*_Tmp7=Cyc_Absyn_unknownid_exp(vd->name,p->loc);Cyc_Absyn_deref_exp(_Tmp7,p->loc);});}}else{goto _LL13;}case 6: _Tmp6=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Pat*p2=_Tmp6;
return({struct Cyc_Absyn_Exp*_Tmp7=Cyc_Parse_pat2exp(p2);Cyc_Absyn_address_exp(_Tmp7,p->loc);});}case 9:
 return Cyc_Absyn_null_exp(p->loc);case 10: _Tmp5=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Sign s=_Tmp5;int i=_Tmp4;
return Cyc_Absyn_int_exp(s,i,p->loc);}case 11: _Tmp3=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{char c=_Tmp3;
return Cyc_Absyn_char_exp(c,p->loc);}case 12: _Tmp2=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct _fat_ptr s=_Tmp2;int i=_Tmp4;
return Cyc_Absyn_float_exp(s,i,p->loc);}case 16: if(((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f3 == 0){_Tmp6=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct _tuple0*x=_Tmp6;struct Cyc_List_List*ps=_Tmp1;
# 981
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(x,p->loc);
struct Cyc_List_List*es=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Parse_pat2exp,ps);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}}else{goto _LL13;}case 17: _Tmp6=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp6;
return e;}default: _LL13:
# 986
 Cyc_Warn_err(p->loc,({const char*_Tmp7="cannot mix patterns and expressions in case";_tag_fat(_Tmp7,sizeof(char),44U);}),_tag_fat(0U,sizeof(void*),0));
return Cyc_Absyn_null_exp(p->loc);};}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _fat_ptr val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple22{unsigned f1;void*f2;void*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple23{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _tuple25{int f1;enum Cyc_Absyn_AggrKind f2;};struct _union_YYSTYPE_YY23{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_Parse_Declarator val;};struct _tuple26{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY29{int tag;struct _tuple26*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY31{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY32{int tag;int val;};struct _union_YYSTYPE_YY33{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY35{int tag;struct Cyc_List_List*val;};struct _tuple27{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY36{int tag;struct _tuple27 val;};struct _union_YYSTYPE_YY37{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY38{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY39{int tag;struct Cyc_List_List*val;};struct _tuple28{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY40{int tag;struct _tuple28*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY43{int tag;void*val;};struct _union_YYSTYPE_YY44{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY45{int tag;void*val;};struct _union_YYSTYPE_YY46{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY47{int tag;void*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY49{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY50{int tag;void*val;};struct _union_YYSTYPE_YY51{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY52{int tag;void*val;};struct _union_YYSTYPE_YY53{int tag;struct Cyc_List_List*val;};struct _tuple29{struct Cyc_List_List*f1;unsigned f2;};struct _union_YYSTYPE_YY54{int tag;struct _tuple29*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY56{int tag;void*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY58{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY59{int tag;struct _tuple17 val;};struct _union_YYSTYPE_YY60{int tag;void*val;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY61{int tag;struct _tuple30*val;};struct _tuple31{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY62{int tag;struct _tuple31*val;};struct _union_YYSTYPE_YY63{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY64{int tag;struct Cyc_List_List*val;};struct _tuple32{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};struct _union_YYSTYPE_YY65{int tag;struct _tuple32*val;};struct _union_YYSTYPE_YY66{int tag;struct Cyc_Absyn_Exp*(*val)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YY58 YY58;struct _union_YYSTYPE_YY59 YY59;struct _union_YYSTYPE_YY60 YY60;struct _union_YYSTYPE_YY61 YY61;struct _union_YYSTYPE_YY62 YY62;struct _union_YYSTYPE_YY63 YY63;struct _union_YYSTYPE_YY64 YY64;struct _union_YYSTYPE_YY65 YY65;struct _union_YYSTYPE_YY66 YY66;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};
# 1072
static void Cyc_yythrowfail(struct _fat_ptr s){
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_Tmp0->tag=Cyc_Core_Failure,_Tmp0->f1=s;_Tmp0;}));}static char _TmpG2[7U]="cnst_t";
# 1041 "parse.y"
static union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2,_TmpG2,_TmpG2 + 7U};
union Cyc_YYSTYPE*_Tmp0=yy1;union Cyc_Absyn_Cnst _Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->Int_tok).tag == 1){_Tmp1=(_Tmp0->Int_tok).val;{union Cyc_Absyn_Cnst yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1048
static union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.Int_tok).tag=1U,(_Tmp0.Int_tok).val=yy1;_Tmp0;});}static char _TmpG3[5U]="char";
# 1042 "parse.y"
static char Cyc_yyget_Char_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3,_TmpG3,_TmpG3 + 5U};
union Cyc_YYSTYPE*_Tmp0=yy1;char _Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->Char_tok).tag == 2){_Tmp1=(_Tmp0->Char_tok).val;{char yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1049
static union Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.Char_tok).tag=2U,(_Tmp0.Char_tok).val=yy1;_Tmp0;});}static char _TmpG4[13U]="string_t<`H>";
# 1043 "parse.y"
static struct _fat_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG4,_TmpG4,_TmpG4 + 13U};
union Cyc_YYSTYPE*_Tmp0=yy1;struct _fat_ptr _Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->String_tok).tag == 3){_Tmp1=(_Tmp0->String_tok).val;{struct _fat_ptr yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1050
static union Cyc_YYSTYPE Cyc_String_tok(struct _fat_ptr yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.String_tok).tag=3U,(_Tmp0.String_tok).val=yy1;_Tmp0;});}static char _TmpG5[35U]="$(seg_t,booltype_t, ptrbound_t)@`H";
# 1046 "parse.y"
static struct _tuple22*Cyc_yyget_YY1(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG5,_TmpG5,_TmpG5 + 35U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY1).tag == 7){_Tmp1=(_Tmp0->YY1).val;{struct _tuple22*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1053
static union Cyc_YYSTYPE Cyc_YY1(struct _tuple22*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY1).tag=7U,(_Tmp0.YY1).val=yy1;_Tmp0;});}static char _TmpG6[11U]="ptrbound_t";
# 1047 "parse.y"
static void*Cyc_yyget_YY2(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG6,_TmpG6,_TmpG6 + 11U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY2).tag == 8){_Tmp1=(_Tmp0->YY2).val;{void*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1054
static union Cyc_YYSTYPE Cyc_YY2(void*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY2).tag=8U,(_Tmp0.YY2).val=yy1;_Tmp0;});}static char _TmpG7[28U]="list_t<offsetof_field_t,`H>";
# 1048 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG7,_TmpG7,_TmpG7 + 28U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY3).tag == 9){_Tmp1=(_Tmp0->YY3).val;{struct Cyc_List_List*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1055
static union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY3).tag=9U,(_Tmp0.YY3).val=yy1;_Tmp0;});}static char _TmpG8[6U]="exp_t";
# 1049 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG8,_TmpG8,_TmpG8 + 6U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->Exp_tok).tag == 5){_Tmp1=(_Tmp0->Exp_tok).val;{struct Cyc_Absyn_Exp*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1056
static union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.Exp_tok).tag=5U,(_Tmp0.Exp_tok).val=yy1;_Tmp0;});}static char _TmpG9[17U]="list_t<exp_t,`H>";
static struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG9,_TmpG9,_TmpG9 + 17U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY4).tag == 10){_Tmp1=(_Tmp0->YY4).val;{struct Cyc_List_List*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1064
static union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY4).tag=10U,(_Tmp0.YY4).val=yy1;_Tmp0;});}static char _TmpGA[47U]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1058 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGA,_TmpGA,_TmpGA + 47U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY5).tag == 11){_Tmp1=(_Tmp0->YY5).val;{struct Cyc_List_List*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1065
static union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY5).tag=11U,(_Tmp0.YY5).val=yy1;_Tmp0;});}static char _TmpGB[9U]="primop_t";
# 1059 "parse.y"
static enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGB,_TmpGB,_TmpGB + 9U};
union Cyc_YYSTYPE*_Tmp0=yy1;enum Cyc_Absyn_Primop _Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY6).tag == 12){_Tmp1=(_Tmp0->YY6).val;{enum Cyc_Absyn_Primop yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1066
static union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY6).tag=12U,(_Tmp0.YY6).val=yy1;_Tmp0;});}static char _TmpGC[19U]="opt_t<primop_t,`H>";
# 1060 "parse.y"
static struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGC,_TmpGC,_TmpGC + 19U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY7).tag == 13){_Tmp1=(_Tmp0->YY7).val;{struct Cyc_Core_Opt*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1067
static union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY7).tag=13U,(_Tmp0.YY7).val=yy1;_Tmp0;});}static char _TmpGD[7U]="qvar_t";
# 1061 "parse.y"
static struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGD,_TmpGD,_TmpGD + 7U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->QualId_tok).tag == 4){_Tmp1=(_Tmp0->QualId_tok).val;{struct _tuple0*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1068
static union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.QualId_tok).tag=4U,(_Tmp0.QualId_tok).val=yy1;_Tmp0;});}static char _TmpGE[7U]="stmt_t";
# 1064 "parse.y"
static struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGE,_TmpGE,_TmpGE + 7U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->Stmt_tok).tag == 6){_Tmp1=(_Tmp0->Stmt_tok).val;{struct Cyc_Absyn_Stmt*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1071
static union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.Stmt_tok).tag=6U,(_Tmp0.Stmt_tok).val=yy1;_Tmp0;});}static char _TmpGF[27U]="list_t<switch_clause_t,`H>";
# 1067 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGF,_TmpGF,_TmpGF + 27U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY8).tag == 14){_Tmp1=(_Tmp0->YY8).val;{struct Cyc_List_List*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1074
static union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY8).tag=14U,(_Tmp0.YY8).val=yy1;_Tmp0;});}static char _TmpG10[6U]="pat_t";
# 1068 "parse.y"
static struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG10,_TmpG10,_TmpG10 + 6U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY9).tag == 15){_Tmp1=(_Tmp0->YY9).val;{struct Cyc_Absyn_Pat*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1075
static union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY9).tag=15U,(_Tmp0.YY9).val=yy1;_Tmp0;});}static char _TmpG11[28U]="$(list_t<pat_t,`H>,bool)@`H";
# 1073 "parse.y"
static struct _tuple23*Cyc_yyget_YY10(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG11,_TmpG11,_TmpG11 + 28U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY10).tag == 16){_Tmp1=(_Tmp0->YY10).val;{struct _tuple23*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1080
static union Cyc_YYSTYPE Cyc_YY10(struct _tuple23*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY10).tag=16U,(_Tmp0.YY10).val=yy1;_Tmp0;});}static char _TmpG12[17U]="list_t<pat_t,`H>";
# 1074 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG12,_TmpG12,_TmpG12 + 17U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY11).tag == 17){_Tmp1=(_Tmp0->YY11).val;{struct Cyc_List_List*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1081
static union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY11).tag=17U,(_Tmp0.YY11).val=yy1;_Tmp0;});}static char _TmpG13[36U]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1075 "parse.y"
static struct _tuple24*Cyc_yyget_YY12(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG13,_TmpG13,_TmpG13 + 36U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY12).tag == 18){_Tmp1=(_Tmp0->YY12).val;{struct _tuple24*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1082
static union Cyc_YYSTYPE Cyc_YY12(struct _tuple24*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY12).tag=18U,(_Tmp0.YY12).val=yy1;_Tmp0;});}static char _TmpG14[47U]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1076 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG14,_TmpG14,_TmpG14 + 47U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY13).tag == 19){_Tmp1=(_Tmp0->YY13).val;{struct Cyc_List_List*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1083
static union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY13).tag=19U,(_Tmp0.YY13).val=yy1;_Tmp0;});}static char _TmpG15[58U]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1077 "parse.y"
static struct _tuple23*Cyc_yyget_YY14(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG15,_TmpG15,_TmpG15 + 58U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY14).tag == 20){_Tmp1=(_Tmp0->YY14).val;{struct _tuple23*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1084
static union Cyc_YYSTYPE Cyc_YY14(struct _tuple23*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY14).tag=20U,(_Tmp0.YY14).val=yy1;_Tmp0;});}static char _TmpG16[9U]="fndecl_t";
# 1078 "parse.y"
static struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG16,_TmpG16,_TmpG16 + 9U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY15).tag == 21){_Tmp1=(_Tmp0->YY15).val;{struct Cyc_Absyn_Fndecl*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1085
static union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY15).tag=21U,(_Tmp0.YY15).val=yy1;_Tmp0;});}static char _TmpG17[18U]="list_t<decl_t,`H>";
# 1079 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG17,_TmpG17,_TmpG17 + 18U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY16).tag == 22){_Tmp1=(_Tmp0->YY16).val;{struct Cyc_List_List*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1086
static union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY16).tag=22U,(_Tmp0.YY16).val=yy1;_Tmp0;});}static char _TmpG18[12U]="decl_spec_t";
# 1082 "parse.y"
static struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG18,_TmpG18,_TmpG18 + 12U};
union Cyc_YYSTYPE*_Tmp0=yy1;struct Cyc_Parse_Declaration_spec _Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY17).tag == 23){_Tmp1=(_Tmp0->YY17).val;{struct Cyc_Parse_Declaration_spec yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1089
static union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY17).tag=23U,(_Tmp0.YY17).val=yy1;_Tmp0;});}static char _TmpG19[31U]="$(declarator_t<`yy>,exp_opt_t)";
# 1083 "parse.y"
static struct _tuple12 Cyc_yyget_YY18(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG19,_TmpG19,_TmpG19 + 31U};
union Cyc_YYSTYPE*_Tmp0=yy1;struct _tuple12 _Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY18).tag == 24){_Tmp1=(_Tmp0->YY18).val;{struct _tuple12 yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1090
static union Cyc_YYSTYPE Cyc_YY18(struct _tuple12 yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY18).tag=24U,(_Tmp0.YY18).val=yy1;_Tmp0;});}static char _TmpG1A[23U]="declarator_list_t<`yy>";
# 1084 "parse.y"
static struct _tuple11*Cyc_yyget_YY19(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1A,_TmpG1A,_TmpG1A + 23U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY19).tag == 25){_Tmp1=(_Tmp0->YY19).val;{struct _tuple11*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1091
static union Cyc_YYSTYPE Cyc_YY19(struct _tuple11*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY19).tag=25U,(_Tmp0.YY19).val=yy1;_Tmp0;});}static char _TmpG1B[16U]="storage_class_t";
# 1085 "parse.y"
static enum Cyc_Parse_Storage_class Cyc_yyget_YY20(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1B,_TmpG1B,_TmpG1B + 16U};
union Cyc_YYSTYPE*_Tmp0=yy1;enum Cyc_Parse_Storage_class _Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY20).tag == 26){_Tmp1=(_Tmp0->YY20).val;{enum Cyc_Parse_Storage_class yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1092
static union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY20).tag=26U,(_Tmp0.YY20).val=yy1;_Tmp0;});}static char _TmpG1C[17U]="type_specifier_t";
# 1086 "parse.y"
static struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1C,_TmpG1C,_TmpG1C + 17U};
union Cyc_YYSTYPE*_Tmp0=yy1;struct Cyc_Parse_Type_specifier _Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY21).tag == 27){_Tmp1=(_Tmp0->YY21).val;{struct Cyc_Parse_Type_specifier yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1093
static union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY21).tag=27U,(_Tmp0.YY21).val=yy1;_Tmp0;});}static char _TmpG1D[12U]="aggr_kind_t";
# 1088 "parse.y"
static enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1D,_TmpG1D,_TmpG1D + 12U};
union Cyc_YYSTYPE*_Tmp0=yy1;enum Cyc_Absyn_AggrKind _Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY22).tag == 28){_Tmp1=(_Tmp0->YY22).val;{enum Cyc_Absyn_AggrKind yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1095
static union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY22).tag=28U,(_Tmp0.YY22).val=yy1;_Tmp0;});}static char _TmpG1E[20U]="$(bool,aggr_kind_t)";
# 1089 "parse.y"
static struct _tuple25 Cyc_yyget_YY23(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1E,_TmpG1E,_TmpG1E + 20U};
union Cyc_YYSTYPE*_Tmp0=yy1;struct _tuple25 _Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY23).tag == 29){_Tmp1=(_Tmp0->YY23).val;{struct _tuple25 yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1096
static union Cyc_YYSTYPE Cyc_YY23(struct _tuple25 yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY23).tag=29U,(_Tmp0.YY23).val=yy1;_Tmp0;});}static char _TmpG1F[8U]="tqual_t";
# 1090 "parse.y"
static struct Cyc_Absyn_Tqual Cyc_yyget_YY24(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1F,_TmpG1F,_TmpG1F + 8U};
union Cyc_YYSTYPE*_Tmp0=yy1;struct Cyc_Absyn_Tqual _Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY24).tag == 30){_Tmp1=(_Tmp0->YY24).val;{struct Cyc_Absyn_Tqual yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1097
static union Cyc_YYSTYPE Cyc_YY24(struct Cyc_Absyn_Tqual yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY24).tag=30U,(_Tmp0.YY24).val=yy1;_Tmp0;});}static char _TmpG20[23U]="list_t<aggrfield_t,`H>";
# 1091 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG20,_TmpG20,_TmpG20 + 23U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY25).tag == 31){_Tmp1=(_Tmp0->YY25).val;{struct Cyc_List_List*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1098
static union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY25).tag=31U,(_Tmp0.YY25).val=yy1;_Tmp0;});}static char _TmpG21[34U]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1092 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG21,_TmpG21,_TmpG21 + 34U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY26).tag == 32){_Tmp1=(_Tmp0->YY26).val;{struct Cyc_List_List*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1099
static union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY26).tag=32U,(_Tmp0.YY26).val=yy1;_Tmp0;});}static char _TmpG22[33U]="list_t<type_modifier_t<`yy>,`yy>";
# 1093 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG22,_TmpG22,_TmpG22 + 33U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY27).tag == 33){_Tmp1=(_Tmp0->YY27).val;{struct Cyc_List_List*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1100
static union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY27).tag=33U,(_Tmp0.YY27).val=yy1;_Tmp0;});}static char _TmpG23[18U]="declarator_t<`yy>";
# 1094 "parse.y"
static struct Cyc_Parse_Declarator Cyc_yyget_YY28(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG23,_TmpG23,_TmpG23 + 18U};
union Cyc_YYSTYPE*_Tmp0=yy1;struct Cyc_Parse_Declarator _Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY28).tag == 34){_Tmp1=(_Tmp0->YY28).val;{struct Cyc_Parse_Declarator yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1101
static union Cyc_YYSTYPE Cyc_YY28(struct Cyc_Parse_Declarator yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY28).tag=34U,(_Tmp0.YY28).val=yy1;_Tmp0;});}static char _TmpG24[45U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1095 "parse.y"
static struct _tuple26*Cyc_yyget_YY29(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG24,_TmpG24,_TmpG24 + 45U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY29).tag == 35){_Tmp1=(_Tmp0->YY29).val;{struct _tuple26*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1102
static union Cyc_YYSTYPE Cyc_YY29(struct _tuple26*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY29).tag=35U,(_Tmp0.YY29).val=yy1;_Tmp0;});}static char _TmpG25[57U]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1096 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY30(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG25,_TmpG25,_TmpG25 + 57U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY30).tag == 36){_Tmp1=(_Tmp0->YY30).val;{struct Cyc_List_List*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1103
static union Cyc_YYSTYPE Cyc_YY30(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY30).tag=36U,(_Tmp0.YY30).val=yy1;_Tmp0;});}static char _TmpG26[26U]="abstractdeclarator_t<`yy>";
# 1097 "parse.y"
static struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY31(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG26,_TmpG26,_TmpG26 + 26U};
union Cyc_YYSTYPE*_Tmp0=yy1;struct Cyc_Parse_Abstractdeclarator _Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY31).tag == 37){_Tmp1=(_Tmp0->YY31).val;{struct Cyc_Parse_Abstractdeclarator yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1104
static union Cyc_YYSTYPE Cyc_YY31(struct Cyc_Parse_Abstractdeclarator yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY31).tag=37U,(_Tmp0.YY31).val=yy1;_Tmp0;});}static char _TmpG27[5U]="bool";
# 1098 "parse.y"
static int Cyc_yyget_YY32(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG27,_TmpG27,_TmpG27 + 5U};
union Cyc_YYSTYPE*_Tmp0=yy1;int _Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY32).tag == 38){_Tmp1=(_Tmp0->YY32).val;{int yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1105
static union Cyc_YYSTYPE Cyc_YY32(int yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY32).tag=38U,(_Tmp0.YY32).val=yy1;_Tmp0;});}static char _TmpG28[8U]="scope_t";
# 1099 "parse.y"
static enum Cyc_Absyn_Scope Cyc_yyget_YY33(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG28,_TmpG28,_TmpG28 + 8U};
union Cyc_YYSTYPE*_Tmp0=yy1;enum Cyc_Absyn_Scope _Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY33).tag == 39){_Tmp1=(_Tmp0->YY33).val;{enum Cyc_Absyn_Scope yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1106
static union Cyc_YYSTYPE Cyc_YY33(enum Cyc_Absyn_Scope yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY33).tag=39U,(_Tmp0.YY33).val=yy1;_Tmp0;});}static char _TmpG29[16U]="datatypefield_t";
# 1100 "parse.y"
static struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY34(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG29,_TmpG29,_TmpG29 + 16U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY34).tag == 40){_Tmp1=(_Tmp0->YY34).val;{struct Cyc_Absyn_Datatypefield*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1107
static union Cyc_YYSTYPE Cyc_YY34(struct Cyc_Absyn_Datatypefield*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY34).tag=40U,(_Tmp0.YY34).val=yy1;_Tmp0;});}static char _TmpG2A[27U]="list_t<datatypefield_t,`H>";
# 1101 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY35(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2A,_TmpG2A,_TmpG2A + 27U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY35).tag == 41){_Tmp1=(_Tmp0->YY35).val;{struct Cyc_List_List*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1108
static union Cyc_YYSTYPE Cyc_YY35(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY35).tag=41U,(_Tmp0.YY35).val=yy1;_Tmp0;});}static char _TmpG2B[41U]="$(tqual_t,type_specifier_t,attributes_t)";
# 1102 "parse.y"
static struct _tuple27 Cyc_yyget_YY36(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2B,_TmpG2B,_TmpG2B + 41U};
union Cyc_YYSTYPE*_Tmp0=yy1;struct _tuple27 _Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY36).tag == 42){_Tmp1=(_Tmp0->YY36).val;{struct _tuple27 yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1109
static union Cyc_YYSTYPE Cyc_YY36(struct _tuple27 yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY36).tag=42U,(_Tmp0.YY36).val=yy1;_Tmp0;});}static char _TmpG2C[17U]="list_t<var_t,`H>";
# 1103 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY37(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2C,_TmpG2C,_TmpG2C + 17U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY37).tag == 43){_Tmp1=(_Tmp0->YY37).val;{struct Cyc_List_List*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1110
static union Cyc_YYSTYPE Cyc_YY37(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY37).tag=43U,(_Tmp0.YY37).val=yy1;_Tmp0;});}static char _TmpG2D[31U]="$(var_opt_t,tqual_t,type_t)@`H";
# 1104 "parse.y"
static struct _tuple8*Cyc_yyget_YY38(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2D,_TmpG2D,_TmpG2D + 31U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY38).tag == 44){_Tmp1=(_Tmp0->YY38).val;{struct _tuple8*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1111
static union Cyc_YYSTYPE Cyc_YY38(struct _tuple8*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY38).tag=44U,(_Tmp0.YY38).val=yy1;_Tmp0;});}static char _TmpG2E[42U]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1105 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY39(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2E,_TmpG2E,_TmpG2E + 42U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY39).tag == 45){_Tmp1=(_Tmp0->YY39).val;{struct Cyc_List_List*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1112
static union Cyc_YYSTYPE Cyc_YY39(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY39).tag=45U,(_Tmp0.YY39).val=yy1;_Tmp0;});}static char _TmpG2F[115U]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1106 "parse.y"
static struct _tuple28*Cyc_yyget_YY40(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2F,_TmpG2F,_TmpG2F + 115U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY40).tag == 46){_Tmp1=(_Tmp0->YY40).val;{struct _tuple28*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1113
static union Cyc_YYSTYPE Cyc_YY40(struct _tuple28*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY40).tag=46U,(_Tmp0.YY40).val=yy1;_Tmp0;});}static char _TmpG30[8U]="types_t";
# 1107 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG30,_TmpG30,_TmpG30 + 8U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY41).tag == 47){_Tmp1=(_Tmp0->YY41).val;{struct Cyc_List_List*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1114
static union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY41).tag=47U,(_Tmp0.YY41).val=yy1;_Tmp0;});}static char _TmpG31[24U]="list_t<designator_t,`H>";
# 1109 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY42(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG31,_TmpG31,_TmpG31 + 24U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY42).tag == 48){_Tmp1=(_Tmp0->YY42).val;{struct Cyc_List_List*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1116
static union Cyc_YYSTYPE Cyc_YY42(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY42).tag=48U,(_Tmp0.YY42).val=yy1;_Tmp0;});}static char _TmpG32[13U]="designator_t";
# 1110 "parse.y"
static void*Cyc_yyget_YY43(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG32,_TmpG32,_TmpG32 + 13U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY43).tag == 49){_Tmp1=(_Tmp0->YY43).val;{void*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1117
static union Cyc_YYSTYPE Cyc_YY43(void*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY43).tag=49U,(_Tmp0.YY43).val=yy1;_Tmp0;});}static char _TmpG33[7U]="kind_t";
# 1111 "parse.y"
static struct Cyc_Absyn_Kind*Cyc_yyget_YY44(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG33,_TmpG33,_TmpG33 + 7U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY44).tag == 50){_Tmp1=(_Tmp0->YY44).val;{struct Cyc_Absyn_Kind*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1118
static union Cyc_YYSTYPE Cyc_YY44(struct Cyc_Absyn_Kind*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY44).tag=50U,(_Tmp0.YY44).val=yy1;_Tmp0;});}static char _TmpG34[7U]="type_t";
# 1112 "parse.y"
static void*Cyc_yyget_YY45(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG34,_TmpG34,_TmpG34 + 7U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY45).tag == 51){_Tmp1=(_Tmp0->YY45).val;{void*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1119
static union Cyc_YYSTYPE Cyc_YY45(void*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY45).tag=51U,(_Tmp0.YY45).val=yy1;_Tmp0;});}static char _TmpG35[23U]="list_t<attribute_t,`H>";
# 1113 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY46(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG35,_TmpG35,_TmpG35 + 23U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY46).tag == 52){_Tmp1=(_Tmp0->YY46).val;{struct Cyc_List_List*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1120
static union Cyc_YYSTYPE Cyc_YY46(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY46).tag=52U,(_Tmp0.YY46).val=yy1;_Tmp0;});}static char _TmpG36[12U]="attribute_t";
# 1114 "parse.y"
static void*Cyc_yyget_YY47(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG36,_TmpG36,_TmpG36 + 12U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY47).tag == 53){_Tmp1=(_Tmp0->YY47).val;{void*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1121
static union Cyc_YYSTYPE Cyc_YY47(void*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY47).tag=53U,(_Tmp0.YY47).val=yy1;_Tmp0;});}static char _TmpG37[12U]="enumfield_t";
# 1115 "parse.y"
static struct Cyc_Absyn_Enumfield*Cyc_yyget_YY48(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG37,_TmpG37,_TmpG37 + 12U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY48).tag == 54){_Tmp1=(_Tmp0->YY48).val;{struct Cyc_Absyn_Enumfield*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1122
static union Cyc_YYSTYPE Cyc_YY48(struct Cyc_Absyn_Enumfield*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY48).tag=54U,(_Tmp0.YY48).val=yy1;_Tmp0;});}static char _TmpG38[23U]="list_t<enumfield_t,`H>";
# 1116 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY49(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG38,_TmpG38,_TmpG38 + 23U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY49).tag == 55){_Tmp1=(_Tmp0->YY49).val;{struct Cyc_List_List*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1123
static union Cyc_YYSTYPE Cyc_YY49(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY49).tag=55U,(_Tmp0.YY49).val=yy1;_Tmp0;});}static char _TmpG39[11U]="type_opt_t";
# 1117 "parse.y"
static void*Cyc_yyget_YY50(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG39,_TmpG39,_TmpG39 + 11U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY50).tag == 56){_Tmp1=(_Tmp0->YY50).val;{void*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1124
static union Cyc_YYSTYPE Cyc_YY50(void*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY50).tag=56U,(_Tmp0.YY50).val=yy1;_Tmp0;});}static char _TmpG3A[31U]="list_t<$(type_t,type_t)@`H,`H>";
# 1118 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY51(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3A,_TmpG3A,_TmpG3A + 31U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY51).tag == 57){_Tmp1=(_Tmp0->YY51).val;{struct Cyc_List_List*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1125
static union Cyc_YYSTYPE Cyc_YY51(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY51).tag=57U,(_Tmp0.YY51).val=yy1;_Tmp0;});}static char _TmpG3B[11U]="booltype_t";
# 1119 "parse.y"
static void*Cyc_yyget_YY52(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3B,_TmpG3B,_TmpG3B + 11U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY52).tag == 58){_Tmp1=(_Tmp0->YY52).val;{void*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1126
static union Cyc_YYSTYPE Cyc_YY52(void*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY52).tag=58U,(_Tmp0.YY52).val=yy1;_Tmp0;});}static char _TmpG3C[35U]="list_t<$(seg_t,qvar_t,bool)@`H,`H>";
# 1120 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY53(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3C,_TmpG3C,_TmpG3C + 35U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY53).tag == 59){_Tmp1=(_Tmp0->YY53).val;{struct Cyc_List_List*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1127
static union Cyc_YYSTYPE Cyc_YY53(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY53).tag=59U,(_Tmp0.YY53).val=yy1;_Tmp0;});}static char _TmpG3D[48U]="$(list_t<$(seg_t,qvar_t,bool)@`H,`H>, seg_t)@`H";
# 1121 "parse.y"
static struct _tuple29*Cyc_yyget_YY54(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3D,_TmpG3D,_TmpG3D + 48U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY54).tag == 60){_Tmp1=(_Tmp0->YY54).val;{struct _tuple29*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1128
static union Cyc_YYSTYPE Cyc_YY54(struct _tuple29*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY54).tag=60U,(_Tmp0.YY54).val=yy1;_Tmp0;});}static char _TmpG3E[18U]="list_t<qvar_t,`H>";
# 1122 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY55(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3E,_TmpG3E,_TmpG3E + 18U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY55).tag == 61){_Tmp1=(_Tmp0->YY55).val;{struct Cyc_List_List*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1129
static union Cyc_YYSTYPE Cyc_YY55(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY55).tag=61U,(_Tmp0.YY55).val=yy1;_Tmp0;});}static char _TmpG3F[20U]="pointer_qual_t<`yy>";
# 1123 "parse.y"
static void*Cyc_yyget_YY56(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3F,_TmpG3F,_TmpG3F + 20U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY56).tag == 62){_Tmp1=(_Tmp0->YY56).val;{void*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1130
static union Cyc_YYSTYPE Cyc_YY56(void*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY56).tag=62U,(_Tmp0.YY56).val=yy1;_Tmp0;});}static char _TmpG40[21U]="pointer_quals_t<`yy>";
# 1124 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY57(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG40,_TmpG40,_TmpG40 + 21U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY57).tag == 63){_Tmp1=(_Tmp0->YY57).val;{struct Cyc_List_List*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1131
static union Cyc_YYSTYPE Cyc_YY57(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY57).tag=63U,(_Tmp0.YY57).val=yy1;_Tmp0;});}static char _TmpG41[10U]="exp_opt_t";
# 1125 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_YY58(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG41,_TmpG41,_TmpG41 + 10U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY58).tag == 64){_Tmp1=(_Tmp0->YY58).val;{struct Cyc_Absyn_Exp*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1132
static union Cyc_YYSTYPE Cyc_YY58(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY58).tag=64U,(_Tmp0.YY58).val=yy1;_Tmp0;});}static char _TmpG42[23U]="$(exp_opt_t,exp_opt_t)";
# 1126 "parse.y"
static struct _tuple17 Cyc_yyget_YY59(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG42,_TmpG42,_TmpG42 + 23U};
union Cyc_YYSTYPE*_Tmp0=yy1;struct _tuple17 _Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY59).tag == 65){_Tmp1=(_Tmp0->YY59).val;{struct _tuple17 yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1133
static union Cyc_YYSTYPE Cyc_YY59(struct _tuple17 yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY59).tag=65U,(_Tmp0.YY59).val=yy1;_Tmp0;});}static char _TmpG43[10U]="raw_exp_t";
# 1127 "parse.y"
static void*Cyc_yyget_YY60(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG43,_TmpG43,_TmpG43 + 10U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY60).tag == 66){_Tmp1=(_Tmp0->YY60).val;{void*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1134
static union Cyc_YYSTYPE Cyc_YY60(void*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY60).tag=66U,(_Tmp0.YY60).val=yy1;_Tmp0;});}static char _TmpG44[112U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1128 "parse.y"
static struct _tuple30*Cyc_yyget_YY61(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG44,_TmpG44,_TmpG44 + 112U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY61).tag == 67){_Tmp1=(_Tmp0->YY61).val;{struct _tuple30*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1135
static union Cyc_YYSTYPE Cyc_YY61(struct _tuple30*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY61).tag=67U,(_Tmp0.YY61).val=yy1;_Tmp0;});}static char _TmpG45[73U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1129 "parse.y"
static struct _tuple31*Cyc_yyget_YY62(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG45,_TmpG45,_TmpG45 + 73U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY62).tag == 68){_Tmp1=(_Tmp0->YY62).val;{struct _tuple31*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1136
static union Cyc_YYSTYPE Cyc_YY62(struct _tuple31*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY62).tag=68U,(_Tmp0.YY62).val=yy1;_Tmp0;});}static char _TmpG46[28U]="list_t<string_t<`H>@`H, `H>";
# 1130 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY63(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG46,_TmpG46,_TmpG46 + 28U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY63).tag == 69){_Tmp1=(_Tmp0->YY63).val;{struct Cyc_List_List*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1137
static union Cyc_YYSTYPE Cyc_YY63(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY63).tag=69U,(_Tmp0.YY63).val=yy1;_Tmp0;});}static char _TmpG47[38U]="list_t<$(string_t<`H>, exp_t)@`H, `H>";
# 1131 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY64(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG47,_TmpG47,_TmpG47 + 38U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY64).tag == 70){_Tmp1=(_Tmp0->YY64).val;{struct Cyc_List_List*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1138
static union Cyc_YYSTYPE Cyc_YY64(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY64).tag=70U,(_Tmp0.YY64).val=yy1;_Tmp0;});}static char _TmpG48[26U]="$(string_t<`H>, exp_t)@`H";
# 1132 "parse.y"
static struct _tuple32*Cyc_yyget_YY65(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG48,_TmpG48,_TmpG48 + 26U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY65).tag == 71){_Tmp1=(_Tmp0->YY65).val;{struct _tuple32*yy=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1139
static union Cyc_YYSTYPE Cyc_YY65(struct _tuple32*yy1){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY65).tag=71U,(_Tmp0.YY65).val=yy1;_Tmp0;});}static char _TmpG49[16U]="exp_maker_fun_t";
# 1133 "parse.y"
static struct Cyc_Absyn_Exp*(*Cyc_yyget_YY66(union Cyc_YYSTYPE*yy1))(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned){
static struct _fat_ptr s={_TmpG49,_TmpG49,_TmpG49 + 16U};
union Cyc_YYSTYPE*_Tmp0=yy1;void*_Tmp1;if((((union Cyc_YYSTYPE*)_Tmp0)->YY66).tag == 72){_Tmp1=(_Tmp0->YY66).val;{struct Cyc_Absyn_Exp*(*yy)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=_Tmp1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1140
static union Cyc_YYSTYPE Cyc_YY66(struct Cyc_Absyn_Exp*(*yy1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)){return({union Cyc_YYSTYPE _Tmp0;(_Tmp0.YY66).tag=72U,(_Tmp0.YY66).val=yy1;_Tmp0;});}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1156
struct Cyc_Yyltype Cyc_yynewloc (void){
return({struct Cyc_Yyltype _Tmp0;_Tmp0.timestamp=0,_Tmp0.first_line=0,_Tmp0.first_column=0,_Tmp0.last_line=0,_Tmp0.last_column=0;_Tmp0;});}
# 1159
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1170 "parse.y"
static short Cyc_yytranslate[381U]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,152,2,2,137,150,147,2,134,135,130,144,129,148,139,149,2,2,2,2,2,2,2,2,2,2,138,126,132,131,133,143,142,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,140,2,141,146,136,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,127,145,128,151,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125};static char _TmpG4A[2U]="$";static char _TmpG4B[6U]="error";static char _TmpG4C[12U]="$undefined.";static char _TmpG4D[5U]="AUTO";static char _TmpG4E[9U]="REGISTER";static char _TmpG4F[7U]="STATIC";static char _TmpG50[7U]="EXTERN";static char _TmpG51[8U]="TYPEDEF";static char _TmpG52[5U]="VOID";static char _TmpG53[5U]="CHAR";static char _TmpG54[6U]="SHORT";static char _TmpG55[4U]="INT";static char _TmpG56[5U]="LONG";static char _TmpG57[6U]="FLOAT";static char _TmpG58[7U]="DOUBLE";static char _TmpG59[7U]="SIGNED";static char _TmpG5A[9U]="UNSIGNED";static char _TmpG5B[6U]="CONST";static char _TmpG5C[9U]="VOLATILE";static char _TmpG5D[9U]="RESTRICT";static char _TmpG5E[7U]="STRUCT";static char _TmpG5F[6U]="UNION";static char _TmpG60[5U]="CASE";static char _TmpG61[8U]="DEFAULT";static char _TmpG62[7U]="INLINE";static char _TmpG63[7U]="SIZEOF";static char _TmpG64[9U]="OFFSETOF";static char _TmpG65[3U]="IF";static char _TmpG66[5U]="ELSE";static char _TmpG67[7U]="SWITCH";static char _TmpG68[6U]="WHILE";static char _TmpG69[3U]="DO";static char _TmpG6A[4U]="FOR";static char _TmpG6B[5U]="GOTO";static char _TmpG6C[9U]="CONTINUE";static char _TmpG6D[6U]="BREAK";static char _TmpG6E[7U]="RETURN";static char _TmpG6F[5U]="ENUM";static char _TmpG70[7U]="TYPEOF";static char _TmpG71[16U]="BUILTIN_VA_LIST";static char _TmpG72[10U]="EXTENSION";static char _TmpG73[8U]="NULL_kw";static char _TmpG74[4U]="LET";static char _TmpG75[6U]="THROW";static char _TmpG76[4U]="TRY";static char _TmpG77[6U]="CATCH";static char _TmpG78[7U]="EXPORT";static char _TmpG79[9U]="OVERRIDE";static char _TmpG7A[5U]="HIDE";static char _TmpG7B[4U]="NEW";static char _TmpG7C[9U]="ABSTRACT";static char _TmpG7D[9U]="FALLTHRU";static char _TmpG7E[6U]="USING";static char _TmpG7F[10U]="NAMESPACE";static char _TmpG80[9U]="DATATYPE";static char _TmpG81[7U]="MALLOC";static char _TmpG82[8U]="RMALLOC";static char _TmpG83[15U]="RMALLOC_INLINE";static char _TmpG84[7U]="CALLOC";static char _TmpG85[8U]="RCALLOC";static char _TmpG86[5U]="SWAP";static char _TmpG87[9U]="REGION_T";static char _TmpG88[6U]="TAG_T";static char _TmpG89[7U]="REGION";static char _TmpG8A[5U]="RNEW";static char _TmpG8B[8U]="REGIONS";static char _TmpG8C[7U]="PORTON";static char _TmpG8D[8U]="PORTOFF";static char _TmpG8E[7U]="PRAGMA";static char _TmpG8F[10U]="TEMPESTON";static char _TmpG90[11U]="TEMPESTOFF";static char _TmpG91[8U]="NUMELTS";static char _TmpG92[8U]="VALUEOF";static char _TmpG93[10U]="VALUEOF_T";static char _TmpG94[9U]="TAGCHECK";static char _TmpG95[13U]="NUMELTS_QUAL";static char _TmpG96[10U]="THIN_QUAL";static char _TmpG97[9U]="FAT_QUAL";static char _TmpG98[13U]="NOTNULL_QUAL";static char _TmpG99[14U]="NULLABLE_QUAL";static char _TmpG9A[14U]="REQUIRES_QUAL";static char _TmpG9B[13U]="ENSURES_QUAL";static char _TmpG9C[12U]="REGION_QUAL";static char _TmpG9D[16U]="NOZEROTERM_QUAL";static char _TmpG9E[14U]="ZEROTERM_QUAL";static char _TmpG9F[12U]="TAGGED_QUAL";static char _TmpGA0[12U]="ASSERT_QUAL";static char _TmpGA1[16U]="EXTENSIBLE_QUAL";static char _TmpGA2[18U]="AUTORELEASED_QUAL";static char _TmpGA3[7U]="PTR_OP";static char _TmpGA4[7U]="INC_OP";static char _TmpGA5[7U]="DEC_OP";static char _TmpGA6[8U]="LEFT_OP";static char _TmpGA7[9U]="RIGHT_OP";static char _TmpGA8[6U]="LE_OP";static char _TmpGA9[6U]="GE_OP";static char _TmpGAA[6U]="EQ_OP";static char _TmpGAB[6U]="NE_OP";static char _TmpGAC[7U]="AND_OP";static char _TmpGAD[6U]="OR_OP";static char _TmpGAE[11U]="MUL_ASSIGN";static char _TmpGAF[11U]="DIV_ASSIGN";static char _TmpGB0[11U]="MOD_ASSIGN";static char _TmpGB1[11U]="ADD_ASSIGN";static char _TmpGB2[11U]="SUB_ASSIGN";static char _TmpGB3[12U]="LEFT_ASSIGN";static char _TmpGB4[13U]="RIGHT_ASSIGN";static char _TmpGB5[11U]="AND_ASSIGN";static char _TmpGB6[11U]="XOR_ASSIGN";static char _TmpGB7[10U]="OR_ASSIGN";static char _TmpGB8[9U]="ELLIPSIS";static char _TmpGB9[11U]="LEFT_RIGHT";static char _TmpGBA[12U]="COLON_COLON";static char _TmpGBB[11U]="IDENTIFIER";static char _TmpGBC[17U]="INTEGER_CONSTANT";static char _TmpGBD[7U]="STRING";static char _TmpGBE[8U]="WSTRING";static char _TmpGBF[19U]="CHARACTER_CONSTANT";static char _TmpGC0[20U]="WCHARACTER_CONSTANT";static char _TmpGC1[18U]="FLOATING_CONSTANT";static char _TmpGC2[9U]="TYPE_VAR";static char _TmpGC3[13U]="TYPEDEF_NAME";static char _TmpGC4[16U]="QUAL_IDENTIFIER";static char _TmpGC5[18U]="QUAL_TYPEDEF_NAME";static char _TmpGC6[10U]="ATTRIBUTE";static char _TmpGC7[8U]="ASM_TOK";static char _TmpGC8[4U]="';'";static char _TmpGC9[4U]="'{'";static char _TmpGCA[4U]="'}'";static char _TmpGCB[4U]="','";static char _TmpGCC[4U]="'*'";static char _TmpGCD[4U]="'='";static char _TmpGCE[4U]="'<'";static char _TmpGCF[4U]="'>'";static char _TmpGD0[4U]="'('";static char _TmpGD1[4U]="')'";static char _TmpGD2[4U]="'_'";static char _TmpGD3[4U]="'$'";static char _TmpGD4[4U]="':'";static char _TmpGD5[4U]="'.'";static char _TmpGD6[4U]="'['";static char _TmpGD7[4U]="']'";static char _TmpGD8[4U]="'@'";static char _TmpGD9[4U]="'?'";static char _TmpGDA[4U]="'+'";static char _TmpGDB[4U]="'|'";static char _TmpGDC[4U]="'^'";static char _TmpGDD[4U]="'&'";static char _TmpGDE[4U]="'-'";static char _TmpGDF[4U]="'/'";static char _TmpGE0[4U]="'%'";static char _TmpGE1[4U]="'~'";static char _TmpGE2[4U]="'!'";static char _TmpGE3[5U]="prog";static char _TmpGE4[17U]="translation_unit";static char _TmpGE5[18U]="tempest_on_action";static char _TmpGE6[19U]="tempest_off_action";static char _TmpGE7[16U]="extern_c_action";static char _TmpGE8[13U]="end_extern_c";static char _TmpGE9[14U]="hide_list_opt";static char _TmpGEA[17U]="hide_list_values";static char _TmpGEB[16U]="export_list_opt";static char _TmpGEC[12U]="export_list";static char _TmpGED[19U]="export_list_values";static char _TmpGEE[13U]="override_opt";static char _TmpGEF[21U]="external_declaration";static char _TmpGF0[14U]="optional_semi";static char _TmpGF1[20U]="function_definition";static char _TmpGF2[21U]="function_definition2";static char _TmpGF3[13U]="using_action";static char _TmpGF4[15U]="unusing_action";static char _TmpGF5[17U]="namespace_action";static char _TmpGF6[19U]="unnamespace_action";static char _TmpGF7[12U]="declaration";static char _TmpGF8[9U]="open_opt";static char _TmpGF9[17U]="declaration_list";static char _TmpGFA[23U]="declaration_specifiers";static char _TmpGFB[24U]="storage_class_specifier";static char _TmpGFC[15U]="attributes_opt";static char _TmpGFD[11U]="attributes";static char _TmpGFE[15U]="attribute_list";static char _TmpGFF[10U]="attribute";static char _TmpG100[15U]="type_specifier";static char _TmpG101[25U]="type_specifier_notypedef";static char _TmpG102[5U]="kind";static char _TmpG103[15U]="type_qualifier";static char _TmpG104[15U]="enum_specifier";static char _TmpG105[11U]="enum_field";static char _TmpG106[22U]="enum_declaration_list";static char _TmpG107[26U]="struct_or_union_specifier";static char _TmpG108[26U]="maybe_tagged_struct_union";static char _TmpG109[16U]="struct_or_union";static char _TmpG10A[16U]="type_params_opt";static char _TmpG10B[24U]="struct_declaration_list";static char _TmpG10C[25U]="struct_declaration_list0";static char _TmpG10D[25U]="init_declarator_list_rev";static char _TmpG10E[16U]="init_declarator";static char _TmpG10F[19U]="struct_declaration";static char _TmpG110[25U]="specifier_qualifier_list";static char _TmpG111[35U]="notypedef_specifier_qualifier_list";static char _TmpG112[27U]="struct_declarator_list_rev";static char _TmpG113[18U]="struct_declarator";static char _TmpG114[20U]="requires_clause_opt";static char _TmpG115[19U]="datatype_specifier";static char _TmpG116[14U]="qual_datatype";static char _TmpG117[19U]="datatypefield_list";static char _TmpG118[20U]="datatypefield_scope";static char _TmpG119[14U]="datatypefield";static char _TmpG11A[11U]="declarator";static char _TmpG11B[23U]="declarator_withtypedef";static char _TmpG11C[18U]="direct_declarator";static char _TmpG11D[30U]="direct_declarator_withtypedef";static char _TmpG11E[8U]="pointer";static char _TmpG11F[12U]="one_pointer";static char _TmpG120[14U]="pointer_quals";static char _TmpG121[13U]="pointer_qual";static char _TmpG122[23U]="pointer_null_and_bound";static char _TmpG123[14U]="pointer_bound";static char _TmpG124[18U]="zeroterm_qual_opt";static char _TmpG125[8U]="rgn_opt";static char _TmpG126[11U]="tqual_list";static char _TmpG127[20U]="parameter_type_list";static char _TmpG128[9U]="type_var";static char _TmpG129[16U]="optional_effect";static char _TmpG12A[19U]="optional_rgn_order";static char _TmpG12B[10U]="rgn_order";static char _TmpG12C[16U]="optional_inject";static char _TmpG12D[11U]="effect_set";static char _TmpG12E[14U]="atomic_effect";static char _TmpG12F[11U]="region_set";static char _TmpG130[15U]="parameter_list";static char _TmpG131[22U]="parameter_declaration";static char _TmpG132[16U]="identifier_list";static char _TmpG133[17U]="identifier_list0";static char _TmpG134[12U]="initializer";static char _TmpG135[18U]="array_initializer";static char _TmpG136[17U]="initializer_list";static char _TmpG137[12U]="designation";static char _TmpG138[16U]="designator_list";static char _TmpG139[11U]="designator";static char _TmpG13A[10U]="type_name";static char _TmpG13B[14U]="any_type_name";static char _TmpG13C[15U]="type_name_list";static char _TmpG13D[20U]="abstract_declarator";static char _TmpG13E[27U]="direct_abstract_declarator";static char _TmpG13F[25U]="requires_and_ensures_opt";static char _TmpG140[10U]="statement";static char _TmpG141[18U]="labeled_statement";static char _TmpG142[21U]="expression_statement";static char _TmpG143[19U]="compound_statement";static char _TmpG144[16U]="block_item_list";static char _TmpG145[20U]="selection_statement";static char _TmpG146[15U]="switch_clauses";static char _TmpG147[20U]="iteration_statement";static char _TmpG148[12U]="for_exp_opt";static char _TmpG149[15U]="jump_statement";static char _TmpG14A[12U]="exp_pattern";static char _TmpG14B[20U]="conditional_pattern";static char _TmpG14C[19U]="logical_or_pattern";static char _TmpG14D[20U]="logical_and_pattern";static char _TmpG14E[21U]="inclusive_or_pattern";static char _TmpG14F[21U]="exclusive_or_pattern";static char _TmpG150[12U]="and_pattern";static char _TmpG151[17U]="equality_pattern";static char _TmpG152[19U]="relational_pattern";static char _TmpG153[14U]="shift_pattern";static char _TmpG154[17U]="additive_pattern";static char _TmpG155[23U]="multiplicative_pattern";static char _TmpG156[13U]="cast_pattern";static char _TmpG157[14U]="unary_pattern";static char _TmpG158[16U]="postfix_pattern";static char _TmpG159[16U]="primary_pattern";static char _TmpG15A[8U]="pattern";static char _TmpG15B[19U]="tuple_pattern_list";static char _TmpG15C[20U]="tuple_pattern_list0";static char _TmpG15D[14U]="field_pattern";static char _TmpG15E[19U]="field_pattern_list";static char _TmpG15F[20U]="field_pattern_list0";static char _TmpG160[11U]="expression";static char _TmpG161[22U]="assignment_expression";static char _TmpG162[20U]="assignment_operator";static char _TmpG163[23U]="conditional_expression";static char _TmpG164[20U]="constant_expression";static char _TmpG165[22U]="logical_or_expression";static char _TmpG166[23U]="logical_and_expression";static char _TmpG167[24U]="inclusive_or_expression";static char _TmpG168[24U]="exclusive_or_expression";static char _TmpG169[15U]="and_expression";static char _TmpG16A[20U]="equality_expression";static char _TmpG16B[22U]="relational_expression";static char _TmpG16C[17U]="shift_expression";static char _TmpG16D[20U]="additive_expression";static char _TmpG16E[26U]="multiplicative_expression";static char _TmpG16F[12U]="equality_op";static char _TmpG170[14U]="relational_op";static char _TmpG171[12U]="additive_op";static char _TmpG172[18U]="multiplicative_op";static char _TmpG173[16U]="cast_expression";static char _TmpG174[17U]="unary_expression";static char _TmpG175[15U]="unary_operator";static char _TmpG176[9U]="asm_expr";static char _TmpG177[13U]="volatile_opt";static char _TmpG178[12U]="asm_out_opt";static char _TmpG179[12U]="asm_outlist";static char _TmpG17A[11U]="asm_in_opt";static char _TmpG17B[11U]="asm_inlist";static char _TmpG17C[11U]="asm_io_elt";static char _TmpG17D[16U]="asm_clobber_opt";static char _TmpG17E[17U]="asm_clobber_list";static char _TmpG17F[19U]="postfix_expression";static char _TmpG180[17U]="field_expression";static char _TmpG181[19U]="primary_expression";static char _TmpG182[25U]="argument_expression_list";static char _TmpG183[26U]="argument_expression_list0";static char _TmpG184[9U]="constant";static char _TmpG185[20U]="qual_opt_identifier";static char _TmpG186[17U]="qual_opt_typedef";static char _TmpG187[18U]="struct_union_name";static char _TmpG188[11U]="field_name";static char _TmpG189[12U]="right_angle";
# 1523 "parse.y"
static struct _fat_ptr Cyc_yytname[320U]={{_TmpG4A,_TmpG4A,_TmpG4A + 2U},{_TmpG4B,_TmpG4B,_TmpG4B + 6U},{_TmpG4C,_TmpG4C,_TmpG4C + 12U},{_TmpG4D,_TmpG4D,_TmpG4D + 5U},{_TmpG4E,_TmpG4E,_TmpG4E + 9U},{_TmpG4F,_TmpG4F,_TmpG4F + 7U},{_TmpG50,_TmpG50,_TmpG50 + 7U},{_TmpG51,_TmpG51,_TmpG51 + 8U},{_TmpG52,_TmpG52,_TmpG52 + 5U},{_TmpG53,_TmpG53,_TmpG53 + 5U},{_TmpG54,_TmpG54,_TmpG54 + 6U},{_TmpG55,_TmpG55,_TmpG55 + 4U},{_TmpG56,_TmpG56,_TmpG56 + 5U},{_TmpG57,_TmpG57,_TmpG57 + 6U},{_TmpG58,_TmpG58,_TmpG58 + 7U},{_TmpG59,_TmpG59,_TmpG59 + 7U},{_TmpG5A,_TmpG5A,_TmpG5A + 9U},{_TmpG5B,_TmpG5B,_TmpG5B + 6U},{_TmpG5C,_TmpG5C,_TmpG5C + 9U},{_TmpG5D,_TmpG5D,_TmpG5D + 9U},{_TmpG5E,_TmpG5E,_TmpG5E + 7U},{_TmpG5F,_TmpG5F,_TmpG5F + 6U},{_TmpG60,_TmpG60,_TmpG60 + 5U},{_TmpG61,_TmpG61,_TmpG61 + 8U},{_TmpG62,_TmpG62,_TmpG62 + 7U},{_TmpG63,_TmpG63,_TmpG63 + 7U},{_TmpG64,_TmpG64,_TmpG64 + 9U},{_TmpG65,_TmpG65,_TmpG65 + 3U},{_TmpG66,_TmpG66,_TmpG66 + 5U},{_TmpG67,_TmpG67,_TmpG67 + 7U},{_TmpG68,_TmpG68,_TmpG68 + 6U},{_TmpG69,_TmpG69,_TmpG69 + 3U},{_TmpG6A,_TmpG6A,_TmpG6A + 4U},{_TmpG6B,_TmpG6B,_TmpG6B + 5U},{_TmpG6C,_TmpG6C,_TmpG6C + 9U},{_TmpG6D,_TmpG6D,_TmpG6D + 6U},{_TmpG6E,_TmpG6E,_TmpG6E + 7U},{_TmpG6F,_TmpG6F,_TmpG6F + 5U},{_TmpG70,_TmpG70,_TmpG70 + 7U},{_TmpG71,_TmpG71,_TmpG71 + 16U},{_TmpG72,_TmpG72,_TmpG72 + 10U},{_TmpG73,_TmpG73,_TmpG73 + 8U},{_TmpG74,_TmpG74,_TmpG74 + 4U},{_TmpG75,_TmpG75,_TmpG75 + 6U},{_TmpG76,_TmpG76,_TmpG76 + 4U},{_TmpG77,_TmpG77,_TmpG77 + 6U},{_TmpG78,_TmpG78,_TmpG78 + 7U},{_TmpG79,_TmpG79,_TmpG79 + 9U},{_TmpG7A,_TmpG7A,_TmpG7A + 5U},{_TmpG7B,_TmpG7B,_TmpG7B + 4U},{_TmpG7C,_TmpG7C,_TmpG7C + 9U},{_TmpG7D,_TmpG7D,_TmpG7D + 9U},{_TmpG7E,_TmpG7E,_TmpG7E + 6U},{_TmpG7F,_TmpG7F,_TmpG7F + 10U},{_TmpG80,_TmpG80,_TmpG80 + 9U},{_TmpG81,_TmpG81,_TmpG81 + 7U},{_TmpG82,_TmpG82,_TmpG82 + 8U},{_TmpG83,_TmpG83,_TmpG83 + 15U},{_TmpG84,_TmpG84,_TmpG84 + 7U},{_TmpG85,_TmpG85,_TmpG85 + 8U},{_TmpG86,_TmpG86,_TmpG86 + 5U},{_TmpG87,_TmpG87,_TmpG87 + 9U},{_TmpG88,_TmpG88,_TmpG88 + 6U},{_TmpG89,_TmpG89,_TmpG89 + 7U},{_TmpG8A,_TmpG8A,_TmpG8A + 5U},{_TmpG8B,_TmpG8B,_TmpG8B + 8U},{_TmpG8C,_TmpG8C,_TmpG8C + 7U},{_TmpG8D,_TmpG8D,_TmpG8D + 8U},{_TmpG8E,_TmpG8E,_TmpG8E + 7U},{_TmpG8F,_TmpG8F,_TmpG8F + 10U},{_TmpG90,_TmpG90,_TmpG90 + 11U},{_TmpG91,_TmpG91,_TmpG91 + 8U},{_TmpG92,_TmpG92,_TmpG92 + 8U},{_TmpG93,_TmpG93,_TmpG93 + 10U},{_TmpG94,_TmpG94,_TmpG94 + 9U},{_TmpG95,_TmpG95,_TmpG95 + 13U},{_TmpG96,_TmpG96,_TmpG96 + 10U},{_TmpG97,_TmpG97,_TmpG97 + 9U},{_TmpG98,_TmpG98,_TmpG98 + 13U},{_TmpG99,_TmpG99,_TmpG99 + 14U},{_TmpG9A,_TmpG9A,_TmpG9A + 14U},{_TmpG9B,_TmpG9B,_TmpG9B + 13U},{_TmpG9C,_TmpG9C,_TmpG9C + 12U},{_TmpG9D,_TmpG9D,_TmpG9D + 16U},{_TmpG9E,_TmpG9E,_TmpG9E + 14U},{_TmpG9F,_TmpG9F,_TmpG9F + 12U},{_TmpGA0,_TmpGA0,_TmpGA0 + 12U},{_TmpGA1,_TmpGA1,_TmpGA1 + 16U},{_TmpGA2,_TmpGA2,_TmpGA2 + 18U},{_TmpGA3,_TmpGA3,_TmpGA3 + 7U},{_TmpGA4,_TmpGA4,_TmpGA4 + 7U},{_TmpGA5,_TmpGA5,_TmpGA5 + 7U},{_TmpGA6,_TmpGA6,_TmpGA6 + 8U},{_TmpGA7,_TmpGA7,_TmpGA7 + 9U},{_TmpGA8,_TmpGA8,_TmpGA8 + 6U},{_TmpGA9,_TmpGA9,_TmpGA9 + 6U},{_TmpGAA,_TmpGAA,_TmpGAA + 6U},{_TmpGAB,_TmpGAB,_TmpGAB + 6U},{_TmpGAC,_TmpGAC,_TmpGAC + 7U},{_TmpGAD,_TmpGAD,_TmpGAD + 6U},{_TmpGAE,_TmpGAE,_TmpGAE + 11U},{_TmpGAF,_TmpGAF,_TmpGAF + 11U},{_TmpGB0,_TmpGB0,_TmpGB0 + 11U},{_TmpGB1,_TmpGB1,_TmpGB1 + 11U},{_TmpGB2,_TmpGB2,_TmpGB2 + 11U},{_TmpGB3,_TmpGB3,_TmpGB3 + 12U},{_TmpGB4,_TmpGB4,_TmpGB4 + 13U},{_TmpGB5,_TmpGB5,_TmpGB5 + 11U},{_TmpGB6,_TmpGB6,_TmpGB6 + 11U},{_TmpGB7,_TmpGB7,_TmpGB7 + 10U},{_TmpGB8,_TmpGB8,_TmpGB8 + 9U},{_TmpGB9,_TmpGB9,_TmpGB9 + 11U},{_TmpGBA,_TmpGBA,_TmpGBA + 12U},{_TmpGBB,_TmpGBB,_TmpGBB + 11U},{_TmpGBC,_TmpGBC,_TmpGBC + 17U},{_TmpGBD,_TmpGBD,_TmpGBD + 7U},{_TmpGBE,_TmpGBE,_TmpGBE + 8U},{_TmpGBF,_TmpGBF,_TmpGBF + 19U},{_TmpGC0,_TmpGC0,_TmpGC0 + 20U},{_TmpGC1,_TmpGC1,_TmpGC1 + 18U},{_TmpGC2,_TmpGC2,_TmpGC2 + 9U},{_TmpGC3,_TmpGC3,_TmpGC3 + 13U},{_TmpGC4,_TmpGC4,_TmpGC4 + 16U},{_TmpGC5,_TmpGC5,_TmpGC5 + 18U},{_TmpGC6,_TmpGC6,_TmpGC6 + 10U},{_TmpGC7,_TmpGC7,_TmpGC7 + 8U},{_TmpGC8,_TmpGC8,_TmpGC8 + 4U},{_TmpGC9,_TmpGC9,_TmpGC9 + 4U},{_TmpGCA,_TmpGCA,_TmpGCA + 4U},{_TmpGCB,_TmpGCB,_TmpGCB + 4U},{_TmpGCC,_TmpGCC,_TmpGCC + 4U},{_TmpGCD,_TmpGCD,_TmpGCD + 4U},{_TmpGCE,_TmpGCE,_TmpGCE + 4U},{_TmpGCF,_TmpGCF,_TmpGCF + 4U},{_TmpGD0,_TmpGD0,_TmpGD0 + 4U},{_TmpGD1,_TmpGD1,_TmpGD1 + 4U},{_TmpGD2,_TmpGD2,_TmpGD2 + 4U},{_TmpGD3,_TmpGD3,_TmpGD3 + 4U},{_TmpGD4,_TmpGD4,_TmpGD4 + 4U},{_TmpGD5,_TmpGD5,_TmpGD5 + 4U},{_TmpGD6,_TmpGD6,_TmpGD6 + 4U},{_TmpGD7,_TmpGD7,_TmpGD7 + 4U},{_TmpGD8,_TmpGD8,_TmpGD8 + 4U},{_TmpGD9,_TmpGD9,_TmpGD9 + 4U},{_TmpGDA,_TmpGDA,_TmpGDA + 4U},{_TmpGDB,_TmpGDB,_TmpGDB + 4U},{_TmpGDC,_TmpGDC,_TmpGDC + 4U},{_TmpGDD,_TmpGDD,_TmpGDD + 4U},{_TmpGDE,_TmpGDE,_TmpGDE + 4U},{_TmpGDF,_TmpGDF,_TmpGDF + 4U},{_TmpGE0,_TmpGE0,_TmpGE0 + 4U},{_TmpGE1,_TmpGE1,_TmpGE1 + 4U},{_TmpGE2,_TmpGE2,_TmpGE2 + 4U},{_TmpGE3,_TmpGE3,_TmpGE3 + 5U},{_TmpGE4,_TmpGE4,_TmpGE4 + 17U},{_TmpGE5,_TmpGE5,_TmpGE5 + 18U},{_TmpGE6,_TmpGE6,_TmpGE6 + 19U},{_TmpGE7,_TmpGE7,_TmpGE7 + 16U},{_TmpGE8,_TmpGE8,_TmpGE8 + 13U},{_TmpGE9,_TmpGE9,_TmpGE9 + 14U},{_TmpGEA,_TmpGEA,_TmpGEA + 17U},{_TmpGEB,_TmpGEB,_TmpGEB + 16U},{_TmpGEC,_TmpGEC,_TmpGEC + 12U},{_TmpGED,_TmpGED,_TmpGED + 19U},{_TmpGEE,_TmpGEE,_TmpGEE + 13U},{_TmpGEF,_TmpGEF,_TmpGEF + 21U},{_TmpGF0,_TmpGF0,_TmpGF0 + 14U},{_TmpGF1,_TmpGF1,_TmpGF1 + 20U},{_TmpGF2,_TmpGF2,_TmpGF2 + 21U},{_TmpGF3,_TmpGF3,_TmpGF3 + 13U},{_TmpGF4,_TmpGF4,_TmpGF4 + 15U},{_TmpGF5,_TmpGF5,_TmpGF5 + 17U},{_TmpGF6,_TmpGF6,_TmpGF6 + 19U},{_TmpGF7,_TmpGF7,_TmpGF7 + 12U},{_TmpGF8,_TmpGF8,_TmpGF8 + 9U},{_TmpGF9,_TmpGF9,_TmpGF9 + 17U},{_TmpGFA,_TmpGFA,_TmpGFA + 23U},{_TmpGFB,_TmpGFB,_TmpGFB + 24U},{_TmpGFC,_TmpGFC,_TmpGFC + 15U},{_TmpGFD,_TmpGFD,_TmpGFD + 11U},{_TmpGFE,_TmpGFE,_TmpGFE + 15U},{_TmpGFF,_TmpGFF,_TmpGFF + 10U},{_TmpG100,_TmpG100,_TmpG100 + 15U},{_TmpG101,_TmpG101,_TmpG101 + 25U},{_TmpG102,_TmpG102,_TmpG102 + 5U},{_TmpG103,_TmpG103,_TmpG103 + 15U},{_TmpG104,_TmpG104,_TmpG104 + 15U},{_TmpG105,_TmpG105,_TmpG105 + 11U},{_TmpG106,_TmpG106,_TmpG106 + 22U},{_TmpG107,_TmpG107,_TmpG107 + 26U},{_TmpG108,_TmpG108,_TmpG108 + 26U},{_TmpG109,_TmpG109,_TmpG109 + 16U},{_TmpG10A,_TmpG10A,_TmpG10A + 16U},{_TmpG10B,_TmpG10B,_TmpG10B + 24U},{_TmpG10C,_TmpG10C,_TmpG10C + 25U},{_TmpG10D,_TmpG10D,_TmpG10D + 25U},{_TmpG10E,_TmpG10E,_TmpG10E + 16U},{_TmpG10F,_TmpG10F,_TmpG10F + 19U},{_TmpG110,_TmpG110,_TmpG110 + 25U},{_TmpG111,_TmpG111,_TmpG111 + 35U},{_TmpG112,_TmpG112,_TmpG112 + 27U},{_TmpG113,_TmpG113,_TmpG113 + 18U},{_TmpG114,_TmpG114,_TmpG114 + 20U},{_TmpG115,_TmpG115,_TmpG115 + 19U},{_TmpG116,_TmpG116,_TmpG116 + 14U},{_TmpG117,_TmpG117,_TmpG117 + 19U},{_TmpG118,_TmpG118,_TmpG118 + 20U},{_TmpG119,_TmpG119,_TmpG119 + 14U},{_TmpG11A,_TmpG11A,_TmpG11A + 11U},{_TmpG11B,_TmpG11B,_TmpG11B + 23U},{_TmpG11C,_TmpG11C,_TmpG11C + 18U},{_TmpG11D,_TmpG11D,_TmpG11D + 30U},{_TmpG11E,_TmpG11E,_TmpG11E + 8U},{_TmpG11F,_TmpG11F,_TmpG11F + 12U},{_TmpG120,_TmpG120,_TmpG120 + 14U},{_TmpG121,_TmpG121,_TmpG121 + 13U},{_TmpG122,_TmpG122,_TmpG122 + 23U},{_TmpG123,_TmpG123,_TmpG123 + 14U},{_TmpG124,_TmpG124,_TmpG124 + 18U},{_TmpG125,_TmpG125,_TmpG125 + 8U},{_TmpG126,_TmpG126,_TmpG126 + 11U},{_TmpG127,_TmpG127,_TmpG127 + 20U},{_TmpG128,_TmpG128,_TmpG128 + 9U},{_TmpG129,_TmpG129,_TmpG129 + 16U},{_TmpG12A,_TmpG12A,_TmpG12A + 19U},{_TmpG12B,_TmpG12B,_TmpG12B + 10U},{_TmpG12C,_TmpG12C,_TmpG12C + 16U},{_TmpG12D,_TmpG12D,_TmpG12D + 11U},{_TmpG12E,_TmpG12E,_TmpG12E + 14U},{_TmpG12F,_TmpG12F,_TmpG12F + 11U},{_TmpG130,_TmpG130,_TmpG130 + 15U},{_TmpG131,_TmpG131,_TmpG131 + 22U},{_TmpG132,_TmpG132,_TmpG132 + 16U},{_TmpG133,_TmpG133,_TmpG133 + 17U},{_TmpG134,_TmpG134,_TmpG134 + 12U},{_TmpG135,_TmpG135,_TmpG135 + 18U},{_TmpG136,_TmpG136,_TmpG136 + 17U},{_TmpG137,_TmpG137,_TmpG137 + 12U},{_TmpG138,_TmpG138,_TmpG138 + 16U},{_TmpG139,_TmpG139,_TmpG139 + 11U},{_TmpG13A,_TmpG13A,_TmpG13A + 10U},{_TmpG13B,_TmpG13B,_TmpG13B + 14U},{_TmpG13C,_TmpG13C,_TmpG13C + 15U},{_TmpG13D,_TmpG13D,_TmpG13D + 20U},{_TmpG13E,_TmpG13E,_TmpG13E + 27U},{_TmpG13F,_TmpG13F,_TmpG13F + 25U},{_TmpG140,_TmpG140,_TmpG140 + 10U},{_TmpG141,_TmpG141,_TmpG141 + 18U},{_TmpG142,_TmpG142,_TmpG142 + 21U},{_TmpG143,_TmpG143,_TmpG143 + 19U},{_TmpG144,_TmpG144,_TmpG144 + 16U},{_TmpG145,_TmpG145,_TmpG145 + 20U},{_TmpG146,_TmpG146,_TmpG146 + 15U},{_TmpG147,_TmpG147,_TmpG147 + 20U},{_TmpG148,_TmpG148,_TmpG148 + 12U},{_TmpG149,_TmpG149,_TmpG149 + 15U},{_TmpG14A,_TmpG14A,_TmpG14A + 12U},{_TmpG14B,_TmpG14B,_TmpG14B + 20U},{_TmpG14C,_TmpG14C,_TmpG14C + 19U},{_TmpG14D,_TmpG14D,_TmpG14D + 20U},{_TmpG14E,_TmpG14E,_TmpG14E + 21U},{_TmpG14F,_TmpG14F,_TmpG14F + 21U},{_TmpG150,_TmpG150,_TmpG150 + 12U},{_TmpG151,_TmpG151,_TmpG151 + 17U},{_TmpG152,_TmpG152,_TmpG152 + 19U},{_TmpG153,_TmpG153,_TmpG153 + 14U},{_TmpG154,_TmpG154,_TmpG154 + 17U},{_TmpG155,_TmpG155,_TmpG155 + 23U},{_TmpG156,_TmpG156,_TmpG156 + 13U},{_TmpG157,_TmpG157,_TmpG157 + 14U},{_TmpG158,_TmpG158,_TmpG158 + 16U},{_TmpG159,_TmpG159,_TmpG159 + 16U},{_TmpG15A,_TmpG15A,_TmpG15A + 8U},{_TmpG15B,_TmpG15B,_TmpG15B + 19U},{_TmpG15C,_TmpG15C,_TmpG15C + 20U},{_TmpG15D,_TmpG15D,_TmpG15D + 14U},{_TmpG15E,_TmpG15E,_TmpG15E + 19U},{_TmpG15F,_TmpG15F,_TmpG15F + 20U},{_TmpG160,_TmpG160,_TmpG160 + 11U},{_TmpG161,_TmpG161,_TmpG161 + 22U},{_TmpG162,_TmpG162,_TmpG162 + 20U},{_TmpG163,_TmpG163,_TmpG163 + 23U},{_TmpG164,_TmpG164,_TmpG164 + 20U},{_TmpG165,_TmpG165,_TmpG165 + 22U},{_TmpG166,_TmpG166,_TmpG166 + 23U},{_TmpG167,_TmpG167,_TmpG167 + 24U},{_TmpG168,_TmpG168,_TmpG168 + 24U},{_TmpG169,_TmpG169,_TmpG169 + 15U},{_TmpG16A,_TmpG16A,_TmpG16A + 20U},{_TmpG16B,_TmpG16B,_TmpG16B + 22U},{_TmpG16C,_TmpG16C,_TmpG16C + 17U},{_TmpG16D,_TmpG16D,_TmpG16D + 20U},{_TmpG16E,_TmpG16E,_TmpG16E + 26U},{_TmpG16F,_TmpG16F,_TmpG16F + 12U},{_TmpG170,_TmpG170,_TmpG170 + 14U},{_TmpG171,_TmpG171,_TmpG171 + 12U},{_TmpG172,_TmpG172,_TmpG172 + 18U},{_TmpG173,_TmpG173,_TmpG173 + 16U},{_TmpG174,_TmpG174,_TmpG174 + 17U},{_TmpG175,_TmpG175,_TmpG175 + 15U},{_TmpG176,_TmpG176,_TmpG176 + 9U},{_TmpG177,_TmpG177,_TmpG177 + 13U},{_TmpG178,_TmpG178,_TmpG178 + 12U},{_TmpG179,_TmpG179,_TmpG179 + 12U},{_TmpG17A,_TmpG17A,_TmpG17A + 11U},{_TmpG17B,_TmpG17B,_TmpG17B + 11U},{_TmpG17C,_TmpG17C,_TmpG17C + 11U},{_TmpG17D,_TmpG17D,_TmpG17D + 16U},{_TmpG17E,_TmpG17E,_TmpG17E + 17U},{_TmpG17F,_TmpG17F,_TmpG17F + 19U},{_TmpG180,_TmpG180,_TmpG180 + 17U},{_TmpG181,_TmpG181,_TmpG181 + 19U},{_TmpG182,_TmpG182,_TmpG182 + 25U},{_TmpG183,_TmpG183,_TmpG183 + 26U},{_TmpG184,_TmpG184,_TmpG184 + 9U},{_TmpG185,_TmpG185,_TmpG185 + 20U},{_TmpG186,_TmpG186,_TmpG186 + 17U},{_TmpG187,_TmpG187,_TmpG187 + 18U},{_TmpG188,_TmpG188,_TmpG188 + 11U},{_TmpG189,_TmpG189,_TmpG189 + 12U}};
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
struct _RegionHandle _Tmp0=_new_region("yyregion");struct _RegionHandle*yyregion=& _Tmp0;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 154
int yychar;
union Cyc_YYSTYPE yylval=({union Cyc_YYSTYPE _Tmp1;(_Tmp1.YYINITIALSVAL).tag=73U,(_Tmp1.YYINITIALSVAL).val=0;_Tmp1;});
int yynerrs;
# 158
struct Cyc_Yyltype yylloc;
# 162
int yyssp_offset;
# 164
struct _fat_ptr yyss=({unsigned _Tmp1=200U;_tag_fat(_region_calloc(yyregion,sizeof(short),_Tmp1),sizeof(short),_Tmp1);});
# 166
int yyvsp_offset;
# 168
struct _fat_ptr yyvs=
_tag_fat(({unsigned _Tmp1=200U;({struct Cyc_Yystacktype*_Tmp2=({struct _RegionHandle*_Tmp3=yyregion;_region_malloc(_Tmp3,_check_times(_Tmp1,sizeof(struct Cyc_Yystacktype)));});({{unsigned _Tmp3=200U;unsigned i;for(i=0;i < _Tmp3;++ i){(_Tmp2[i]).v=yylval,({struct Cyc_Yyltype _Tmp4=Cyc_yynewloc();(_Tmp2[i]).l=_Tmp4;});}}0;});_Tmp2;});}),sizeof(struct Cyc_Yystacktype),200U);
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
Cyc_yyerror(({const char*_Tmp1="parser stack overflow";_tag_fat(_Tmp1,sizeof(char),22U);}),yystate,yychar);
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 216
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
struct _fat_ptr yyss1=({unsigned _Tmp1=(unsigned)yystacksize;_tag_fat(({short*_Tmp2=({struct _RegionHandle*_Tmp3=yyregion;_region_malloc(_Tmp3,_check_times(_Tmp1,sizeof(short)));});({{unsigned _Tmp3=(unsigned)yystacksize;unsigned i;for(i=0;i < _Tmp3;++ i){i <= (unsigned)yyssp_offset?_Tmp2[i]=*((short*)_check_fat_subscript(yyss,sizeof(short),(int)i)):(_Tmp2[i]=0);}}0;});_Tmp2;}),sizeof(short),_Tmp1);});
# 222
struct _fat_ptr yyvs1=({unsigned _Tmp1=(unsigned)yystacksize;
_tag_fat(({struct Cyc_Yystacktype*_Tmp2=({struct _RegionHandle*_Tmp3=yyregion;_region_malloc(_Tmp3,_check_times(_Tmp1,sizeof(struct Cyc_Yystacktype)));});({{unsigned _Tmp3=(unsigned)yystacksize;unsigned i;for(i=0;i < _Tmp3;++ i){i <= (unsigned)yyssp_offset?_Tmp2[i]=*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(int)i)):(_Tmp2[i]=*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),0)));}}0;});_Tmp2;}),sizeof(struct Cyc_Yystacktype),_Tmp1);});
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
int _Tmp1=0;_npop_handler(0);return _Tmp1;}
# 328 "cycbison.simple"
if(yychar != 0)
yychar=-2;
# 332
({struct Cyc_Yystacktype _Tmp1=({struct Cyc_Yystacktype _Tmp2;_Tmp2.v=yylval,_Tmp2.l=yylloc;_Tmp2;});((struct Cyc_Yystacktype*)yyvs.curr)[++ yyvsp_offset]=_Tmp1;});
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
{int _Tmp1=yyn;switch((int)_Tmp1){case 1:
# 1141 "parse.y"
 yyval=(yyyvsp[0]).v;
Cyc_Parse_parse_result=Cyc_yyget_YY16(&(yyyvsp[0]).v);
# 1144
goto _LL0;case 2:
# 1147 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*(*_Tmp2)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append;});struct Cyc_List_List*_Tmp3=Cyc_yyget_YY16(&(yyyvsp[0]).v);_Tmp2(_Tmp3,Cyc_yyget_YY16(&(yyyvsp[1]).v));}));
goto _LL0;case 3:
# 1151 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp3=({struct Cyc_Absyn_Decl*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp5=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct));_Tmp6->tag=10,({struct _tuple0*_Tmp7=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_Tmp6->f1=_Tmp7;}),({struct Cyc_List_List*_Tmp7=Cyc_yyget_YY16(&(yyyvsp[2]).v);_Tmp6->f2=_Tmp7;});_Tmp6;});_Tmp4->r=_Tmp5;}),_Tmp4->loc=(unsigned)((yyyvsp[0]).l).first_line;_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
Cyc_Lex_leave_using();
# 1154
goto _LL0;case 4:
# 1155 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp3=({struct Cyc_Absyn_Decl*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp5=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct));_Tmp6->tag=10,({struct _tuple0*_Tmp7=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_Tmp6->f1=_Tmp7;}),({struct Cyc_List_List*_Tmp7=Cyc_yyget_YY16(&(yyyvsp[2]).v);_Tmp6->f2=_Tmp7;});_Tmp6;});_Tmp4->r=_Tmp5;}),_Tmp4->loc=(unsigned)((yyyvsp[0]).l).first_line;_Tmp4;});_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY16(&(yyyvsp[4]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 5:
# 1158
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp3=({struct Cyc_Absyn_Decl*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp5=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct));_Tmp6->tag=9,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f1=_Tmp7;}),({struct Cyc_List_List*_Tmp7=Cyc_yyget_YY16(&(yyyvsp[2]).v);_Tmp6->f2=_Tmp7;});_Tmp6;});_Tmp4->r=_Tmp5;}),_Tmp4->loc=(unsigned)((yyyvsp[0]).l).first_line;_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
Cyc_Lex_leave_namespace();
# 1161
goto _LL0;case 6:
# 1162 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp3=({struct Cyc_Absyn_Decl*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp5=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct));_Tmp6->tag=9,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f1=_Tmp7;}),({struct Cyc_List_List*_Tmp7=Cyc_yyget_YY16(&(yyyvsp[2]).v);_Tmp6->f2=_Tmp7;});_Tmp6;});_Tmp4->r=_Tmp5;}),_Tmp4->loc=(unsigned)((yyyvsp[0]).l).first_line;_Tmp4;});_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY16(&(yyyvsp[4]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 7:  {
# 1164 "parse.y"
int is_c_include=Cyc_yyget_YY32(&(yyyvsp[0]).v);
struct Cyc_List_List*cycdecls=Cyc_yyget_YY16(&(yyyvsp[4]).v);
struct _tuple29*_stmttmp15=Cyc_yyget_YY54(&(yyyvsp[5]).v);struct _tuple29*_Tmp2=_stmttmp15;unsigned _Tmp3;void*_Tmp4;_Tmp4=_Tmp2->f1;_Tmp3=_Tmp2->f2;{struct Cyc_List_List*exs=_Tmp4;unsigned wc=_Tmp3;
struct Cyc_List_List*hides=Cyc_yyget_YY55(&(yyyvsp[6]).v);
if(exs != 0 && hides != 0)
Cyc_Warn_err((unsigned)((yyyvsp[0]).l).first_line,({const char*_Tmp5="hide list can only be used with export { * }, or no export block";_tag_fat(_Tmp5,sizeof(char),65U);}),_tag_fat(0U,sizeof(void*),0));
if((unsigned)hides && !((int)wc))
wc=(unsigned)((yyyvsp[6]).l).first_line;
if(!is_c_include){
if(exs != 0 || cycdecls != 0){
Cyc_Warn_err((unsigned)((yyyvsp[0]).l).first_line,({const char*_Tmp5="expecting \"C include\"";_tag_fat(_Tmp5,sizeof(char),22U);}),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp6=({struct Cyc_Absyn_Decl*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp8=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct));_Tmp9->tag=12,({struct Cyc_List_List*_TmpA=Cyc_yyget_YY16(&(yyyvsp[2]).v);_Tmp9->f1=_TmpA;}),_Tmp9->f2=cycdecls,_Tmp9->f3=exs,({struct _tuple10*_TmpA=({struct _tuple10*_TmpB=_cycalloc(sizeof(struct _tuple10));_TmpB->f1=wc,_TmpB->f2=hides;_TmpB;});_Tmp9->f4=_TmpA;});_Tmp9;});_Tmp7->r=_Tmp8;}),_Tmp7->loc=(unsigned)((yyyvsp[0]).l).first_line;_Tmp7;});_Tmp5->hd=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY16(&(yyyvsp[7]).v);_Tmp5->tl=_Tmp6;});_Tmp5;}));}else{
# 1178
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp6=({struct Cyc_Absyn_Decl*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp8=(void*)({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct));_Tmp9->tag=11,({struct Cyc_List_List*_TmpA=Cyc_yyget_YY16(&(yyyvsp[2]).v);_Tmp9->f1=_TmpA;});_Tmp9;});_Tmp7->r=_Tmp8;}),_Tmp7->loc=(unsigned)((yyyvsp[0]).l).first_line;_Tmp7;});_Tmp5->hd=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY16(&(yyyvsp[7]).v);_Tmp5->tl=_Tmp6;});_Tmp5;}));}}else{
# 1181
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp6=({struct Cyc_Absyn_Decl*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp8=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct));_Tmp9->tag=12,({struct Cyc_List_List*_TmpA=Cyc_yyget_YY16(&(yyyvsp[2]).v);_Tmp9->f1=_TmpA;}),_Tmp9->f2=cycdecls,_Tmp9->f3=exs,({struct _tuple10*_TmpA=({struct _tuple10*_TmpB=_cycalloc(sizeof(struct _tuple10));_TmpB->f1=wc,_TmpB->f2=hides;_TmpB;});_Tmp9->f4=_TmpA;});_Tmp9;});_Tmp7->r=_Tmp8;}),_Tmp7->loc=(unsigned)((yyyvsp[0]).l).first_line;_Tmp7;});_Tmp5->hd=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY16(&(yyyvsp[7]).v);_Tmp5->tl=_Tmp6;});_Tmp5;}));}
# 1183
goto _LL0;}}case 8:
# 1184 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp3=({struct Cyc_Absyn_Decl*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp4->r=(void*)& Cyc_Absyn_Porton_d_val,_Tmp4->loc=(unsigned)((yyyvsp[0]).l).first_line;_Tmp4;});_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY16(&(yyyvsp[2]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 9:
# 1186 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp3=({struct Cyc_Absyn_Decl*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp4->r=(void*)& Cyc_Absyn_Portoff_d_val,_Tmp4->loc=(unsigned)((yyyvsp[0]).l).first_line;_Tmp4;});_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY16(&(yyyvsp[2]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 10:
# 1188 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp3=({struct Cyc_Absyn_Decl*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp4->r=(void*)& Cyc_Absyn_Tempeston_d_val,_Tmp4->loc=(unsigned)((yyyvsp[0]).l).first_line;_Tmp4;});_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY16(&(yyyvsp[2]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 11:
# 1190 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp3=({struct Cyc_Absyn_Decl*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp4->r=(void*)& Cyc_Absyn_Tempestoff_d_val,_Tmp4->loc=(unsigned)((yyyvsp[0]).l).first_line;_Tmp4;});_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY16(&(yyyvsp[2]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
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
if(({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_Tmp2,({const char*_Tmp3="C";_tag_fat(_Tmp3,sizeof(char),2U);}));})== 0)
yyval=Cyc_YY32(0);else{
if(({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_Tmp2,({const char*_Tmp3="C include";_tag_fat(_Tmp3,sizeof(char),10U);}));})== 0)
yyval=Cyc_YY32(1);else{
# 1209
Cyc_Warn_err((unsigned)((yyyvsp[0]).l).first_line,({const char*_Tmp2="expecting \"C\" or \"C include\"";_tag_fat(_Tmp2,sizeof(char),29U);}),_tag_fat(0U,sizeof(void*),0));
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
 yyval=Cyc_YY55(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp3=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 20:
# 1226 "parse.y"
 yyval=Cyc_YY55(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp3=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 21:
# 1227 "parse.y"
 yyval=Cyc_YY55(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp3=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY55(&(yyyvsp[2]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 22:
# 1231 "parse.y"
 yyval=Cyc_YY54(({struct _tuple29*_Tmp2=_cycalloc(sizeof(struct _tuple29));_Tmp2->f1=0,_Tmp2->f2=0U;_Tmp2;}));
goto _LL0;case 23:
# 1232 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 24:
# 1236 "parse.y"
 yyval=Cyc_YY54(({struct _tuple29*_Tmp2=_cycalloc(sizeof(struct _tuple29));({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY53(&(yyyvsp[2]).v);_Tmp2->f1=_Tmp3;}),_Tmp2->f2=0U;_Tmp2;}));
goto _LL0;case 25:
# 1237 "parse.y"
 yyval=Cyc_YY54(({struct _tuple29*_Tmp2=_cycalloc(sizeof(struct _tuple29));_Tmp2->f1=0,_Tmp2->f2=0U;_Tmp2;}));
goto _LL0;case 26:
# 1238 "parse.y"
 yyval=Cyc_YY54(({struct _tuple29*_Tmp2=_cycalloc(sizeof(struct _tuple29));_Tmp2->f1=0,_Tmp2->f2=(unsigned)((yyyvsp[0]).l).first_line;_Tmp2;}));
goto _LL0;case 27:
# 1243 "parse.y"
 yyval=Cyc_YY53(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple33*_Tmp3=({struct _tuple33*_Tmp4=_cycalloc(sizeof(struct _tuple33));_Tmp4->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_Tmp5=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_Tmp4->f2=_Tmp5;}),_Tmp4->f3=0;_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 28:
# 1245 "parse.y"
 yyval=Cyc_YY53(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple33*_Tmp3=({struct _tuple33*_Tmp4=_cycalloc(sizeof(struct _tuple33));_Tmp4->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_Tmp5=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_Tmp4->f2=_Tmp5;}),_Tmp4->f3=0;_Tmp4;});_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY53(&(yyyvsp[2]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 29:
# 1249 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 30:
# 1250 "parse.y"
 yyval=(yyyvsp[2]).v;
goto _LL0;case 31:
# 1254 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp3=({void*_Tmp4=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_Tmp5->tag=1,({struct Cyc_Absyn_Fndecl*_Tmp6=Cyc_yyget_YY15(&(yyyvsp[0]).v);_Tmp5->f1=_Tmp6;});_Tmp5;});Cyc_Absyn_new_decl(_Tmp4,(unsigned)((yyyvsp[0]).l).first_line);});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 32:
# 1255 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 33:
# 1256 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 36:
# 1266 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_Tmp2=yyr;struct Cyc_Parse_Declarator _Tmp3=Cyc_yyget_YY28(&(yyyvsp[0]).v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);Cyc_Parse_make_function(_Tmp2,0,_Tmp3,0,_Tmp4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 37:  {
# 1268 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(&(yyyvsp[0]).v);
yyval=Cyc_YY15(({struct _RegionHandle*_Tmp2=yyr;struct Cyc_Parse_Declarator _Tmp3=Cyc_yyget_YY28(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_Tmp2,& d,_Tmp3,0,_Tmp4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 38:
# 1280 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_Tmp2=yyr;struct Cyc_Parse_Declarator _Tmp3=Cyc_yyget_YY28(&(yyyvsp[0]).v);struct Cyc_List_List*_Tmp4=Cyc_yyget_YY16(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_Tmp5=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_Tmp2,0,_Tmp3,_Tmp4,_Tmp5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 39:  {
# 1282 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(&(yyyvsp[0]).v);
yyval=Cyc_YY15(({struct _RegionHandle*_Tmp2=yyr;struct Cyc_Parse_Declarator _Tmp3=Cyc_yyget_YY28(&(yyyvsp[1]).v);struct Cyc_List_List*_Tmp4=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_Tmp5=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);Cyc_Parse_make_function(_Tmp2,& d,_Tmp3,_Tmp4,_Tmp5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 40:  {
# 1290 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(&(yyyvsp[0]).v);
yyval=Cyc_YY15(({struct _RegionHandle*_Tmp2=yyr;struct Cyc_Parse_Declarator _Tmp3=Cyc_yyget_YY28(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_Tmp2,& d,_Tmp3,0,_Tmp4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 41:  {
# 1293 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(&(yyyvsp[0]).v);
yyval=Cyc_YY15(({struct _RegionHandle*_Tmp2=yyr;struct Cyc_Parse_Declarator _Tmp3=Cyc_yyget_YY28(&(yyyvsp[1]).v);struct Cyc_List_List*_Tmp4=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_Tmp5=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);Cyc_Parse_make_function(_Tmp2,& d,_Tmp3,_Tmp4,_Tmp5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 42:
# 1298 "parse.y"
 Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(&(yyyvsp[1]).v));yyval=(yyyvsp[1]).v;
goto _LL0;case 43:
# 1301
 Cyc_Lex_leave_using();
goto _LL0;case 44:
# 1304
 Cyc_Lex_enter_namespace(({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_Tmp2=_Tmp3;});_Tmp2;}));yyval=(yyyvsp[1]).v;
goto _LL0;case 45:
# 1307
 Cyc_Lex_leave_namespace();
goto _LL0;case 46:
# 1313 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _Tmp2=Cyc_yyget_YY17(&(yyyvsp[0]).v);unsigned _Tmp3=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Parse_make_declarations(_Tmp2,0,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 47:
# 1315 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _Tmp2=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct _tuple11*_Tmp3=({struct _tuple11*(*_Tmp4)(struct _tuple11*)=({(struct _tuple11*(*)(struct _tuple11*))Cyc_Parse_flat_imp_rev;});_Tmp4(Cyc_yyget_YY19(&(yyyvsp[1]).v));});unsigned _Tmp4=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Parse_make_declarations(_Tmp2,_Tmp3,_Tmp4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 48:
# 1318
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp3=({struct Cyc_Absyn_Pat*_Tmp4=Cyc_yyget_YY9(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_Tmp5=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_let_decl(_Tmp4,_Tmp5,(unsigned)((yyyvsp[0]).l).first_line);});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 49:  {
# 1320 "parse.y"
struct Cyc_List_List*vds=0;
{struct Cyc_List_List*ids=Cyc_yyget_YY37(&(yyyvsp[1]).v);for(0;ids != 0;ids=ids->tl){
struct _tuple0*qv=({struct _tuple0*_Tmp2=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp3=Cyc_Absyn_Rel_n(0);_Tmp2->f1=_Tmp3;}),_Tmp2->f2=(struct _fat_ptr*)ids->hd;_Tmp2;});
vds=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Vardecl*_Tmp3=({struct _tuple0*_Tmp4=qv;Cyc_Absyn_new_vardecl(0U,_Tmp4,Cyc_Absyn_wildtyp(0),0);});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=vds;_Tmp2;});}}
# 1325
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp3=({struct Cyc_List_List*_Tmp4=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(vds);Cyc_Absyn_letv_decl(_Tmp4,(unsigned)((yyyvsp[0]).l).first_line);});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
# 1327
goto _LL0;}case 50:
# 1330 "parse.y"
({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_tvar_ok(_Tmp2,(unsigned)((yyyvsp[2]).l).first_line);});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->name=_Tmp3;}),_Tmp2->identity=-1,({void*_Tmp3=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_Tmp2->kind=_Tmp3;});_Tmp2;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp2=(unsigned)((yyyvsp[4]).l).first_line;struct _tuple0*_Tmp3=({struct _tuple0*_Tmp4=_cycalloc(sizeof(struct _tuple0));_Tmp4->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp5=({struct _fat_ptr*_Tmp6=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp7=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_Tmp6=_Tmp7;});_Tmp6;});_Tmp4->f2=_Tmp5;});_Tmp4;});Cyc_Absyn_new_vardecl(_Tmp2,_Tmp3,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp3=Cyc_Absyn_region_decl(tv,vd,0,(unsigned)((yyyvsp[0]).l).first_line);_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
# 1336
goto _LL0;}case 51:  {
# 1338
struct _fat_ptr tvstring=({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(&(yyyvsp[1]).v);_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(({const char*_Tmp4="`%s";_tag_fat(_Tmp4,sizeof(char),4U);}),_tag_fat(_Tmp3,sizeof(void*),1));});
({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_Parse_tvar_ok(_Tmp2,(unsigned)((yyyvsp[1]).l).first_line);});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));*_Tmp4=tvstring;_Tmp4;});_Tmp2->name=_Tmp3;}),_Tmp2->identity=-1,({void*_Tmp3=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_Tmp2->kind=_Tmp3;});_Tmp2;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp2=(unsigned)((yyyvsp[1]).l).first_line;struct _tuple0*_Tmp3=({struct _tuple0*_Tmp4=_cycalloc(sizeof(struct _tuple0));_Tmp4->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp5=({struct _fat_ptr*_Tmp6=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp7=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_Tmp6=_Tmp7;});_Tmp6;});_Tmp4->f2=_Tmp5;});_Tmp4;});Cyc_Absyn_new_vardecl(_Tmp2,_Tmp3,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp3=({struct Cyc_Absyn_Tvar*_Tmp4=tv;struct Cyc_Absyn_Vardecl*_Tmp5=vd;struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_YY58(&(yyyvsp[2]).v);Cyc_Absyn_region_decl(_Tmp4,_Tmp5,_Tmp6,(unsigned)((yyyvsp[0]).l).first_line);});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
# 1345
goto _LL0;}}case 52:
# 1348 "parse.y"
 yyval=Cyc_YY58(0);
goto _LL0;case 53:
# 1350 "parse.y"
 if(({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_Tmp2,({const char*_Tmp3="open";_tag_fat(_Tmp3,sizeof(char),5U);}));})!= 0)Cyc_Warn_err((unsigned)((yyyvsp[3]).l).first_line,({const char*_Tmp2="expecting `open'";_tag_fat(_Tmp2,sizeof(char),17U);}),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY58(Cyc_yyget_Exp_tok(&(yyyvsp[3]).v));
goto _LL0;case 54:
# 1354
 yyval=(yyyvsp[0]).v;
goto _LL0;case 55:
# 1355 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*(*_Tmp2)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append;});struct Cyc_List_List*_Tmp3=Cyc_yyget_YY16(&(yyyvsp[0]).v);_Tmp2(_Tmp3,Cyc_yyget_YY16(&(yyyvsp[1]).v));}));
goto _LL0;case 56:
# 1361 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp2;({enum Cyc_Parse_Storage_class _Tmp3=Cyc_yyget_YY20(&(yyyvsp[0]).v);_Tmp2.sc=_Tmp3;}),({struct Cyc_Absyn_Tqual _Tmp3=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_Tmp2.tq=_Tmp3;}),({
struct Cyc_Parse_Type_specifier _Tmp3=Cyc_Parse_empty_spec(0U);_Tmp2.type_specs=_Tmp3;}),_Tmp2.is_inline=0,_Tmp2.attributes=0;_Tmp2;}));
goto _LL0;case 57:  {
# 1364 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(&(yyyvsp[1]).v);
if((int)two.sc != 7)
Cyc_Warn_warn((unsigned)((yyyvsp[0]).l).first_line,({const char*_Tmp2="Only one storage class is allowed in a declaration (missing ';' or ','?)";_tag_fat(_Tmp2,sizeof(char),73U);}),_tag_fat(0U,sizeof(void*),0));
# 1368
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp2;({enum Cyc_Parse_Storage_class _Tmp3=Cyc_yyget_YY20(&(yyyvsp[0]).v);_Tmp2.sc=_Tmp3;}),_Tmp2.tq=two.tq,_Tmp2.type_specs=two.type_specs,_Tmp2.is_inline=two.is_inline,_Tmp2.attributes=two.attributes;_Tmp2;}));
# 1371
goto _LL0;}case 58:
# 1372 "parse.y"
 Cyc_Warn_warn((unsigned)((yyyvsp[0]).l).first_line,({const char*_Tmp2="__extension__ keyword ignored in declaration";_tag_fat(_Tmp2,sizeof(char),45U);}),_tag_fat(0U,sizeof(void*),0));
yyval=(yyyvsp[1]).v;
# 1375
goto _LL0;case 59:
# 1376 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp2;_Tmp2.sc=7U,({struct Cyc_Absyn_Tqual _Tmp3=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_Tmp2.tq=_Tmp3;}),({
struct Cyc_Parse_Type_specifier _Tmp3=Cyc_yyget_YY21(&(yyyvsp[0]).v);_Tmp2.type_specs=_Tmp3;}),_Tmp2.is_inline=0,_Tmp2.attributes=0;_Tmp2;}));
goto _LL0;case 60:  {
# 1379 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(&(yyyvsp[1]).v);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp2;_Tmp2.sc=two.sc,_Tmp2.tq=two.tq,({
struct Cyc_Parse_Type_specifier _Tmp3=({unsigned _Tmp4=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _Tmp5=two.type_specs;Cyc_Parse_combine_specifiers(_Tmp4,_Tmp5,Cyc_yyget_YY21(&(yyyvsp[0]).v));});_Tmp2.type_specs=_Tmp3;}),_Tmp2.is_inline=two.is_inline,_Tmp2.attributes=two.attributes;_Tmp2;}));
# 1384
goto _LL0;}case 61:
# 1385 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp2;_Tmp2.sc=7U,({struct Cyc_Absyn_Tqual _Tmp3=Cyc_yyget_YY24(&(yyyvsp[0]).v);_Tmp2.tq=_Tmp3;}),({struct Cyc_Parse_Type_specifier _Tmp3=Cyc_Parse_empty_spec(0U);_Tmp2.type_specs=_Tmp3;}),_Tmp2.is_inline=0,_Tmp2.attributes=0;_Tmp2;}));
goto _LL0;case 62:  {
# 1387 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(&(yyyvsp[1]).v);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp2;_Tmp2.sc=two.sc,({struct Cyc_Absyn_Tqual _Tmp3=({struct Cyc_Absyn_Tqual _Tmp4=Cyc_yyget_YY24(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_Tmp4,two.tq);});_Tmp2.tq=_Tmp3;}),_Tmp2.type_specs=two.type_specs,_Tmp2.is_inline=two.is_inline,_Tmp2.attributes=two.attributes;_Tmp2;}));
# 1391
goto _LL0;}case 63:
# 1392 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp2;_Tmp2.sc=7U,({struct Cyc_Absyn_Tqual _Tmp3=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_Tmp2.tq=_Tmp3;}),({
struct Cyc_Parse_Type_specifier _Tmp3=Cyc_Parse_empty_spec(0U);_Tmp2.type_specs=_Tmp3;}),_Tmp2.is_inline=1,_Tmp2.attributes=0;_Tmp2;}));
goto _LL0;case 64:  {
# 1395 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(&(yyyvsp[1]).v);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp2;_Tmp2.sc=two.sc,_Tmp2.tq=two.tq,_Tmp2.type_specs=two.type_specs,_Tmp2.is_inline=1,_Tmp2.attributes=two.attributes;_Tmp2;}));
# 1398
goto _LL0;}case 65:
# 1399 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp2;_Tmp2.sc=7U,({struct Cyc_Absyn_Tqual _Tmp3=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_Tmp2.tq=_Tmp3;}),({
struct Cyc_Parse_Type_specifier _Tmp3=Cyc_Parse_empty_spec(0U);_Tmp2.type_specs=_Tmp3;}),_Tmp2.is_inline=0,({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY46(&(yyyvsp[0]).v);_Tmp2.attributes=_Tmp3;});_Tmp2;}));
goto _LL0;case 66:  {
# 1402 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(&(yyyvsp[1]).v);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp2;_Tmp2.sc=two.sc,_Tmp2.tq=two.tq,_Tmp2.type_specs=two.type_specs,_Tmp2.is_inline=two.is_inline,({
# 1405
struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*(*_Tmp4)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append;});struct Cyc_List_List*_Tmp5=Cyc_yyget_YY46(&(yyyvsp[0]).v);_Tmp4(_Tmp5,two.attributes);});_Tmp2.attributes=_Tmp3;});_Tmp2;}));
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
 if(({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_Tmp2,({const char*_Tmp3="C";_tag_fat(_Tmp3,sizeof(char),2U);}));})!= 0)
Cyc_Warn_err((unsigned)((yyyvsp[0]).l).first_line,({const char*_Tmp2="only extern or extern \"C\" is allowed";_tag_fat(_Tmp2,sizeof(char),37U);}),_tag_fat(0U,sizeof(void*),0));
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
 yyval=Cyc_YY46(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp3=Cyc_yyget_YY47(&(yyyvsp[0]).v);_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 78:
# 1434 "parse.y"
 yyval=Cyc_YY46(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp3=Cyc_yyget_YY47(&(yyyvsp[0]).v);_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY46(&(yyyvsp[2]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 79:
# 1437
 yyval=Cyc_YY47(({unsigned _Tmp2=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Atts_parse_nullary_att(_Tmp2,Cyc_yyget_String_tok(&(yyyvsp[0]).v));}));
goto _LL0;case 80:
# 1438 "parse.y"
 yyval=Cyc_YY47((void*)& Cyc_Atts_Const_att_val);
goto _LL0;case 81:
# 1440 "parse.y"
 yyval=Cyc_YY47(({unsigned _Tmp2=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(&(yyyvsp[0]).v);unsigned _Tmp4=(unsigned)((yyyvsp[2]).l).first_line;Cyc_Atts_parse_unary_att(_Tmp2,_Tmp3,_Tmp4,Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));}));
goto _LL0;case 82:
# 1442 "parse.y"
 yyval=Cyc_YY47(({unsigned _Tmp2=(unsigned)((yyyvsp[0]).l).first_line;unsigned _Tmp3=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(&(yyyvsp[2]).v);unsigned _Tmp6=({
unsigned _Tmp7=(unsigned)((yyyvsp[4]).l).first_line;Cyc_Parse_cnst2uint(_Tmp7,Cyc_yyget_Int_tok(&(yyyvsp[4]).v));});
# 1442
Cyc_Atts_parse_format_att(_Tmp2,_Tmp3,_Tmp4,_Tmp5,_Tmp6,({
# 1444
unsigned _Tmp7=(unsigned)((yyyvsp[6]).l).first_line;Cyc_Parse_cnst2uint(_Tmp7,Cyc_yyget_Int_tok(&(yyyvsp[6]).v));}));}));
goto _LL0;case 83:
# 1454 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 84:
# 1456 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp2=({struct _tuple0*_Tmp3=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);Cyc_Absyn_typedef_type(_Tmp3,Cyc_yyget_YY41(&(yyyvsp[1]).v),0,0);});Cyc_Parse_type_spec(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
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
 yyval=Cyc_YY21(({void*_Tmp2=Cyc_Absyn_typeof_type(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 97:
# 1474 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp2=Cyc_Absyn_builtin_type(({const char*_Tmp3="__builtin_va_list";_tag_fat(_Tmp3,sizeof(char),18U);}),& Cyc_Kinds_bk);Cyc_Parse_type_spec(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 98:
# 1476 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 99:
# 1478 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp2=Cyc_yyget_YY45(&(yyyvsp[0]).v);Cyc_Parse_type_spec(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 100:
# 1480 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp2=Cyc_Absyn_new_evar(0,0);Cyc_Parse_type_spec(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 101:
# 1482 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp2=Cyc_Absyn_new_evar(Cyc_Kinds_kind_to_opt(Cyc_yyget_YY44(&(yyyvsp[2]).v)),0);Cyc_Parse_type_spec(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 102:
# 1484 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp2=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_TupleType_Absyn_Type_struct));_Tmp3->tag=6,({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*(*_Tmp5)(struct _tuple20*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct _tuple20*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;});struct _tuple20*(*_Tmp6)(unsigned,struct _tuple8*)=({(struct _tuple20*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ;});unsigned _Tmp7=(unsigned)((yyyvsp[2]).l).first_line;_Tmp5(_Tmp6,_Tmp7,({struct Cyc_List_List*(*_Tmp8)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_Tmp8(Cyc_yyget_YY39(&(yyyvsp[2]).v));}));});_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Parse_type_spec(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1486
goto _LL0;case 103:
# 1487 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp2=Cyc_Absyn_rgn_handle_type(Cyc_yyget_YY45(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 104:
# 1489 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp2=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Kinds_rko,0));Cyc_Parse_type_spec(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 105:
# 1491 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp2=Cyc_Absyn_tag_type(Cyc_yyget_YY45(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 106:
# 1493 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp2=Cyc_Absyn_tag_type(Cyc_Absyn_new_evar(& Cyc_Kinds_iko,0));Cyc_Parse_type_spec(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 107:
# 1495 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp2=Cyc_Absyn_valueof_type(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 108:
# 1500 "parse.y"
 yyval=Cyc_YY44(({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Kinds_id_to_kind(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 109:  {
# 1504 "parse.y"
unsigned loc=(unsigned)(Cyc_Flags_porting_c_code?((yyyvsp[0]).l).first_line: 0);
yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _Tmp2;_Tmp2.print_const=1,_Tmp2.q_volatile=0,_Tmp2.q_restrict=0,_Tmp2.real_const=1,_Tmp2.loc=loc;_Tmp2;}));
goto _LL0;}case 110:
# 1506 "parse.y"
 yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _Tmp2;_Tmp2.print_const=0,_Tmp2.q_volatile=1,_Tmp2.q_restrict=0,_Tmp2.real_const=0,_Tmp2.loc=0U;_Tmp2;}));
goto _LL0;case 111:
# 1507 "parse.y"
 yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _Tmp2;_Tmp2.print_const=0,_Tmp2.q_volatile=0,_Tmp2.q_restrict=1,_Tmp2.real_const=0,_Tmp2.loc=0U;_Tmp2;}));
goto _LL0;case 112:  {
# 1513 "parse.y"
struct Cyc_Absyn_TypeDecl*ed=({struct Cyc_Absyn_TypeDecl*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_TypeDecl));({void*_Tmp3=(void*)({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct));_Tmp4->tag=1,({struct Cyc_Absyn_Enumdecl*_Tmp5=({struct Cyc_Absyn_Enumdecl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));_Tmp6->sc=2U,({struct _tuple0*_Tmp7=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_Tmp6->name=_Tmp7;}),({struct Cyc_Core_Opt*_Tmp7=({struct Cyc_Core_Opt*_Tmp8=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_List_List*_Tmp9=Cyc_yyget_YY49(&(yyyvsp[3]).v);_Tmp8->v=_Tmp9;});_Tmp8;});_Tmp6->fields=_Tmp7;});_Tmp6;});_Tmp4->f1=_Tmp5;});_Tmp4;});_Tmp2->r=_Tmp3;}),_Tmp2->loc=(unsigned)((yyyvsp[0]).l).first_line;_Tmp2;});
# 1515
yyval=Cyc_YY21(({void*_Tmp2=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_Tmp3->tag=10,_Tmp3->f1=ed,_Tmp3->f2=0;_Tmp3;});Cyc_Parse_type_spec(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1517
goto _LL0;}case 113:
# 1518 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp2=Cyc_Absyn_enum_type(Cyc_yyget_QualId_tok(&(yyyvsp[1]).v),0);Cyc_Parse_type_spec(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 114:
# 1520 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp2=Cyc_Absyn_anon_enum_type(Cyc_yyget_YY49(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 115:
# 1526 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_Absyn_Enumfield*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));({struct _tuple0*_Tmp3=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_Tmp2->name=_Tmp3;}),_Tmp2->tag=0,_Tmp2->loc=(unsigned)((yyyvsp[0]).l).first_line;_Tmp2;}));
goto _LL0;case 116:
# 1528 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_Absyn_Enumfield*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));({struct _tuple0*_Tmp3=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_Tmp2->name=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_Tmp2->tag=_Tmp3;}),_Tmp2->loc=(unsigned)((yyyvsp[0]).l).first_line;_Tmp2;}));
goto _LL0;case 117:
# 1532 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Enumfield*_Tmp3=Cyc_yyget_YY48(&(yyyvsp[0]).v);_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 118:
# 1533 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Enumfield*_Tmp3=Cyc_yyget_YY48(&(yyyvsp[0]).v);_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 119:
# 1534 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Enumfield*_Tmp3=Cyc_yyget_YY48(&(yyyvsp[0]).v);_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY49(&(yyyvsp[2]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 120:
# 1540 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp2=(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct));_Tmp3->tag=7,({enum Cyc_Absyn_AggrKind _Tmp4=Cyc_yyget_YY22(&(yyyvsp[0]).v);_Tmp3->f1=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY25(&(yyyvsp[2]).v);_Tmp3->f2=_Tmp4;});_Tmp3;});Cyc_Parse_type_spec(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 121:  {
# 1544 "parse.y"
struct Cyc_List_List*ts=({struct Cyc_List_List*(*_Tmp2)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;});unsigned _Tmp3=(unsigned)((yyyvsp[2]).l).first_line;_Tmp2(Cyc_Parse_typ2tvar,_Tmp3,Cyc_yyget_YY41(&(yyyvsp[2]).v));});
struct Cyc_List_List*exist_ts=({struct Cyc_List_List*(*_Tmp2)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;});unsigned _Tmp3=(unsigned)((yyyvsp[4]).l).first_line;_Tmp2(Cyc_Parse_typ2tvar,_Tmp3,Cyc_yyget_YY41(&(yyyvsp[4]).v));});
struct Cyc_Absyn_TypeDecl*td=({enum Cyc_Absyn_AggrKind _Tmp2=(Cyc_yyget_YY23(&(yyyvsp[0]).v)).f2;struct _tuple0*_Tmp3=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_Tmp4=ts;struct Cyc_Absyn_AggrdeclImpl*_Tmp5=({
struct Cyc_List_List*_Tmp6=exist_ts;struct Cyc_List_List*_Tmp7=Cyc_yyget_YY51(&(yyyvsp[5]).v);struct Cyc_List_List*_Tmp8=Cyc_yyget_YY25(&(yyyvsp[6]).v);Cyc_Absyn_aggrdecl_impl(_Tmp6,_Tmp7,_Tmp8,(Cyc_yyget_YY23(&(yyyvsp[0]).v)).f1);});
# 1546
Cyc_Absyn_aggr_tdecl(_Tmp2,2U,_Tmp3,_Tmp4,_Tmp5,0,(unsigned)((yyyvsp[0]).l).first_line);});
# 1549
yyval=Cyc_YY21(({void*_Tmp2=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_Tmp3->tag=10,_Tmp3->f1=td,_Tmp3->f2=0;_Tmp3;});Cyc_Parse_type_spec(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1551
goto _LL0;}case 122:
# 1552 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp2=({union Cyc_Absyn_AggrInfo _Tmp3=({enum Cyc_Absyn_AggrKind _Tmp4=(Cyc_yyget_YY23(&(yyyvsp[0]).v)).f2;struct _tuple0*_Tmp5=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);Cyc_Absyn_UnknownAggr(_Tmp4,_Tmp5,(Cyc_yyget_YY23(&(yyyvsp[0]).v)).f1?({struct Cyc_Core_Opt*_Tmp6=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp6->v=(void*)1;_Tmp6;}): 0);});Cyc_Absyn_aggr_type(_Tmp3,
Cyc_yyget_YY41(&(yyyvsp[2]).v));});
# 1552
Cyc_Parse_type_spec(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1554
goto _LL0;case 123:
# 1557 "parse.y"
 yyval=Cyc_YY23(({struct _tuple25 _Tmp2;_Tmp2.f1=1,({enum Cyc_Absyn_AggrKind _Tmp3=Cyc_yyget_YY22(&(yyyvsp[1]).v);_Tmp2.f2=_Tmp3;});_Tmp2;}));
goto _LL0;case 124:
# 1558 "parse.y"
 yyval=Cyc_YY23(({struct _tuple25 _Tmp2;_Tmp2.f1=0,({enum Cyc_Absyn_AggrKind _Tmp3=Cyc_yyget_YY22(&(yyyvsp[0]).v);_Tmp2.f2=_Tmp3;});_Tmp2;}));
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
 yyval=Cyc_YY41(({struct Cyc_List_List*(*_Tmp2)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_Tmp2(Cyc_yyget_YY41(&(yyyvsp[1]).v));}));
goto _LL0;case 129:
# 1571 "parse.y"
 yyval=Cyc_YY25(0);
goto _LL0;case 130:  {
# 1573 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*x=Cyc_yyget_YY26(&(yyyvsp[0]).v);for(0;x != 0;x=x->tl){
decls=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append;})((struct Cyc_List_List*)x->hd,decls);}}{
# 1577
struct Cyc_List_List*tags=Cyc_Parse_get_aggrfield_tags(decls);
if(tags != 0)
({(void(*)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_iter_c;})(Cyc_Parse_substitute_aggrfield_tags,tags,decls);
yyval=Cyc_YY25(decls);
# 1582
goto _LL0;}}case 131:
# 1586 "parse.y"
 yyval=Cyc_YY26(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY25(&(yyyvsp[0]).v);_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 132:
# 1587 "parse.y"
 yyval=Cyc_YY26(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY25(&(yyyvsp[1]).v);_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY26(&(yyyvsp[0]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 133:
# 1593 "parse.y"
 yyval=Cyc_YY19(({struct _tuple11*_Tmp2=_region_malloc(yyr,sizeof(struct _tuple11));_Tmp2->tl=0,({struct _tuple12 _Tmp3=Cyc_yyget_YY18(&(yyyvsp[0]).v);_Tmp2->hd=_Tmp3;});_Tmp2;}));
goto _LL0;case 134:
# 1595 "parse.y"
 yyval=Cyc_YY19(({struct _tuple11*_Tmp2=_region_malloc(yyr,sizeof(struct _tuple11));({struct _tuple11*_Tmp3=Cyc_yyget_YY19(&(yyyvsp[0]).v);_Tmp2->tl=_Tmp3;}),({struct _tuple12 _Tmp3=Cyc_yyget_YY18(&(yyyvsp[2]).v);_Tmp2->hd=_Tmp3;});_Tmp2;}));
goto _LL0;case 135:
# 1599 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _Tmp2;({struct Cyc_Parse_Declarator _Tmp3=Cyc_yyget_YY28(&(yyyvsp[0]).v);_Tmp2.f1=_Tmp3;}),_Tmp2.f2=0;_Tmp2;}));
goto _LL0;case 136:
# 1600 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _Tmp2;({struct Cyc_Parse_Declarator _Tmp3=Cyc_yyget_YY28(&(yyyvsp[0]).v);_Tmp2.f1=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_Tmp2.f2=_Tmp3;});_Tmp2;}));
goto _LL0;case 137:  {
# 1605 "parse.y"
struct _RegionHandle _Tmp2=_new_region("temp");struct _RegionHandle*temp=& _Tmp2;_push_region(temp);
{struct _tuple27 _stmttmp16=Cyc_yyget_YY36(&(yyyvsp[0]).v);struct _tuple27 _Tmp3=_stmttmp16;void*_Tmp4;struct Cyc_Parse_Type_specifier _Tmp5;struct Cyc_Absyn_Tqual _Tmp6;_Tmp6=_Tmp3.f1;_Tmp5=_Tmp3.f2;_Tmp4=_Tmp3.f3;{struct Cyc_Absyn_Tqual tq=_Tmp6;struct Cyc_Parse_Type_specifier tspecs=_Tmp5;struct Cyc_List_List*atts=_Tmp4;
if(tq.loc == 0U)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
struct _tuple13*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*x=Cyc_yyget_YY30(&(yyyvsp[1]).v);for(0;x != 0;x=x->tl){
struct _tuple26*_stmttmp17=(struct _tuple26*)x->hd;struct _tuple26*_Tmp7=_stmttmp17;void*_Tmp8;void*_Tmp9;struct Cyc_Parse_Declarator _TmpA;_TmpA=_Tmp7->f1;_Tmp9=_Tmp7->f2;_Tmp8=_Tmp7->f3;{struct Cyc_Parse_Declarator d=_TmpA;struct Cyc_Absyn_Exp*wd=_Tmp9;struct Cyc_Absyn_Exp*wh=_Tmp8;
decls=({struct _tuple13*_TmpB=_region_malloc(temp,sizeof(struct _tuple13));_TmpB->tl=decls,_TmpB->hd=d;_TmpB;});
widths_and_reqs=({struct Cyc_List_List*_TmpB=_region_malloc(temp,sizeof(struct Cyc_List_List));({struct _tuple17*_TmpC=({struct _tuple17*_TmpD=_region_malloc(temp,sizeof(struct _tuple17));_TmpD->f1=wd,_TmpD->f2=wh;_TmpD;});_TmpB->hd=_TmpC;}),_TmpB->tl=widths_and_reqs;_TmpB;});}}}{
# 1615
void*t=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);
struct Cyc_List_List*info=({struct Cyc_List_List*(*_Tmp7)(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_rzip;});struct _RegionHandle*_Tmp8=temp;struct _RegionHandle*_Tmp9=temp;struct Cyc_List_List*_TmpA=
Cyc_Parse_apply_tmss(temp,tq,t,decls,atts);
# 1616
_Tmp7(_Tmp8,_Tmp9,_TmpA,widths_and_reqs);});
# 1618
yyval=Cyc_YY25(({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple18*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_make_aggr_field,(unsigned)((yyyvsp[0]).l).first_line,info));
# 1620
_npop_handler(0);goto _LL0;}}}}
# 1606
;_pop_region();}case 138:
# 1627 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _Tmp2;({struct Cyc_Absyn_Tqual _Tmp3=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_Tmp2.f1=_Tmp3;}),({struct Cyc_Parse_Type_specifier _Tmp3=Cyc_yyget_YY21(&(yyyvsp[0]).v);_Tmp2.f2=_Tmp3;}),_Tmp2.f3=0;_Tmp2;}));
goto _LL0;case 139:  {
# 1629 "parse.y"
struct _tuple27 two=Cyc_yyget_YY36(&(yyyvsp[1]).v);yyval=Cyc_YY36(({struct _tuple27 _Tmp2;_Tmp2.f1=two.f1,({struct Cyc_Parse_Type_specifier _Tmp3=({unsigned _Tmp4=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _Tmp5=Cyc_yyget_YY21(&(yyyvsp[0]).v);Cyc_Parse_combine_specifiers(_Tmp4,_Tmp5,two.f2);});_Tmp2.f2=_Tmp3;}),_Tmp2.f3=two.f3;_Tmp2;}));
goto _LL0;}case 140:
# 1631 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _Tmp2;({struct Cyc_Absyn_Tqual _Tmp3=Cyc_yyget_YY24(&(yyyvsp[0]).v);_Tmp2.f1=_Tmp3;}),({struct Cyc_Parse_Type_specifier _Tmp3=Cyc_Parse_empty_spec(0U);_Tmp2.f2=_Tmp3;}),_Tmp2.f3=0;_Tmp2;}));
goto _LL0;case 141:  {
# 1633 "parse.y"
struct _tuple27 two=Cyc_yyget_YY36(&(yyyvsp[1]).v);yyval=Cyc_YY36(({struct _tuple27 _Tmp2;({struct Cyc_Absyn_Tqual _Tmp3=({struct Cyc_Absyn_Tqual _Tmp4=Cyc_yyget_YY24(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_Tmp4,two.f1);});_Tmp2.f1=_Tmp3;}),_Tmp2.f2=two.f2,_Tmp2.f3=two.f3;_Tmp2;}));
goto _LL0;}case 142:
# 1635 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _Tmp2;({struct Cyc_Absyn_Tqual _Tmp3=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_Tmp2.f1=_Tmp3;}),({struct Cyc_Parse_Type_specifier _Tmp3=Cyc_Parse_empty_spec(0U);_Tmp2.f2=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY46(&(yyyvsp[0]).v);_Tmp2.f3=_Tmp3;});_Tmp2;}));
goto _LL0;case 143:  {
# 1637 "parse.y"
struct _tuple27 two=Cyc_yyget_YY36(&(yyyvsp[1]).v);yyval=Cyc_YY36(({struct _tuple27 _Tmp2;_Tmp2.f1=two.f1,_Tmp2.f2=two.f2,({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*(*_Tmp4)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;});struct Cyc_List_List*_Tmp5=Cyc_yyget_YY46(&(yyyvsp[0]).v);_Tmp4(_Tmp5,two.f3);});_Tmp2.f3=_Tmp3;});_Tmp2;}));
goto _LL0;}case 144:
# 1643 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _Tmp2;({struct Cyc_Absyn_Tqual _Tmp3=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_Tmp2.f1=_Tmp3;}),({struct Cyc_Parse_Type_specifier _Tmp3=Cyc_yyget_YY21(&(yyyvsp[0]).v);_Tmp2.f2=_Tmp3;}),_Tmp2.f3=0;_Tmp2;}));
goto _LL0;case 145:  {
# 1645 "parse.y"
struct _tuple27 two=Cyc_yyget_YY36(&(yyyvsp[1]).v);yyval=Cyc_YY36(({struct _tuple27 _Tmp2;_Tmp2.f1=two.f1,({struct Cyc_Parse_Type_specifier _Tmp3=({unsigned _Tmp4=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _Tmp5=Cyc_yyget_YY21(&(yyyvsp[0]).v);Cyc_Parse_combine_specifiers(_Tmp4,_Tmp5,two.f2);});_Tmp2.f2=_Tmp3;}),_Tmp2.f3=two.f3;_Tmp2;}));
goto _LL0;}case 146:
# 1647 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _Tmp2;({struct Cyc_Absyn_Tqual _Tmp3=Cyc_yyget_YY24(&(yyyvsp[0]).v);_Tmp2.f1=_Tmp3;}),({struct Cyc_Parse_Type_specifier _Tmp3=Cyc_Parse_empty_spec(0U);_Tmp2.f2=_Tmp3;}),_Tmp2.f3=0;_Tmp2;}));
goto _LL0;case 147:  {
# 1649 "parse.y"
struct _tuple27 two=Cyc_yyget_YY36(&(yyyvsp[1]).v);yyval=Cyc_YY36(({struct _tuple27 _Tmp2;({struct Cyc_Absyn_Tqual _Tmp3=({struct Cyc_Absyn_Tqual _Tmp4=Cyc_yyget_YY24(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_Tmp4,two.f1);});_Tmp2.f1=_Tmp3;}),_Tmp2.f2=two.f2,_Tmp2.f3=two.f3;_Tmp2;}));
goto _LL0;}case 148:
# 1651 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _Tmp2;({struct Cyc_Absyn_Tqual _Tmp3=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_Tmp2.f1=_Tmp3;}),({struct Cyc_Parse_Type_specifier _Tmp3=Cyc_Parse_empty_spec(0U);_Tmp2.f2=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY46(&(yyyvsp[0]).v);_Tmp2.f3=_Tmp3;});_Tmp2;}));
goto _LL0;case 149:  {
# 1653 "parse.y"
struct _tuple27 two=Cyc_yyget_YY36(&(yyyvsp[1]).v);yyval=Cyc_YY36(({struct _tuple27 _Tmp2;_Tmp2.f1=two.f1,_Tmp2.f2=two.f2,({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*(*_Tmp4)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;});struct Cyc_List_List*_Tmp5=Cyc_yyget_YY46(&(yyyvsp[0]).v);_Tmp4(_Tmp5,two.f3);});_Tmp2.f3=_Tmp3;});_Tmp2;}));
goto _LL0;}case 150:
# 1659 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_List_List*_Tmp2=_region_malloc(yyr,sizeof(struct Cyc_List_List));({struct _tuple26*_Tmp3=Cyc_yyget_YY29(&(yyyvsp[0]).v);_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 151:
# 1661 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_List_List*_Tmp2=_region_malloc(yyr,sizeof(struct Cyc_List_List));({struct _tuple26*_Tmp3=Cyc_yyget_YY29(&(yyyvsp[2]).v);_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY30(&(yyyvsp[0]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 152:
# 1666 "parse.y"
 yyval=Cyc_YY29(({struct _tuple26*_Tmp2=_region_malloc(yyr,sizeof(struct _tuple26));({struct Cyc_Parse_Declarator _Tmp3=Cyc_yyget_YY28(&(yyyvsp[0]).v);_Tmp2->f1=_Tmp3;}),_Tmp2->f2=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_YY58(&(yyyvsp[1]).v);_Tmp2->f3=_Tmp3;});_Tmp2;}));
goto _LL0;case 153:
# 1670 "parse.y"
 yyval=Cyc_YY29(({struct _tuple26*_Tmp2=_region_malloc(yyr,sizeof(struct _tuple26));({struct _tuple0*_Tmp3=({struct _tuple0*_Tmp4=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp5=Cyc_Absyn_Rel_n(0);_Tmp4->f1=_Tmp5;}),({struct _fat_ptr*_Tmp5=({struct _fat_ptr*_Tmp6=_cycalloc(sizeof(struct _fat_ptr));*_Tmp6=({const char*_Tmp7="";_tag_fat(_Tmp7,sizeof(char),1U);});_Tmp6;});_Tmp4->f2=_Tmp5;});_Tmp4;});(_Tmp2->f1).id=_Tmp3;}),(_Tmp2->f1).varloc=0U,(_Tmp2->f1).tms=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_Tmp2->f2=_Tmp3;}),_Tmp2->f3=0;_Tmp2;}));
# 1672
goto _LL0;case 154:
# 1675 "parse.y"
 yyval=Cyc_YY29(({struct _tuple26*_Tmp2=_region_malloc(yyr,sizeof(struct _tuple26));({struct _tuple0*_Tmp3=({struct _tuple0*_Tmp4=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp5=Cyc_Absyn_Rel_n(0);_Tmp4->f1=_Tmp5;}),({struct _fat_ptr*_Tmp5=({struct _fat_ptr*_Tmp6=_cycalloc(sizeof(struct _fat_ptr));*_Tmp6=({const char*_Tmp7="";_tag_fat(_Tmp7,sizeof(char),1U);});_Tmp6;});_Tmp4->f2=_Tmp5;});_Tmp4;});(_Tmp2->f1).id=_Tmp3;}),(_Tmp2->f1).varloc=0U,(_Tmp2->f1).tms=0,_Tmp2->f2=0,_Tmp2->f3=0;_Tmp2;}));
# 1677
goto _LL0;case 155:
# 1678 "parse.y"
 yyval=Cyc_YY29(({struct _tuple26*_Tmp2=_region_malloc(yyr,sizeof(struct _tuple26));({struct Cyc_Parse_Declarator _Tmp3=Cyc_yyget_YY28(&(yyyvsp[0]).v);_Tmp2->f1=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_Tmp2->f2=_Tmp3;}),_Tmp2->f3=0;_Tmp2;}));
goto _LL0;case 156:
# 1682 "parse.y"
 yyval=Cyc_YY58(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));
goto _LL0;case 157:
# 1683 "parse.y"
 yyval=Cyc_YY58(0);
goto _LL0;case 158:  {
# 1689 "parse.y"
int is_extensible=Cyc_yyget_YY32(&(yyyvsp[0]).v);
struct Cyc_List_List*ts=({struct Cyc_List_List*(*_Tmp2)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;});unsigned _Tmp3=(unsigned)((yyyvsp[2]).l).first_line;_Tmp2(Cyc_Parse_typ2tvar,_Tmp3,Cyc_yyget_YY41(&(yyyvsp[2]).v));});
struct Cyc_Absyn_TypeDecl*dd=({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_Tmp3=ts;struct Cyc_Core_Opt*_Tmp4=({struct Cyc_Core_Opt*_Tmp5=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY35(&(yyyvsp[4]).v);_Tmp5->v=_Tmp6;});_Tmp5;});int _Tmp5=is_extensible;Cyc_Absyn_datatype_tdecl(2U,_Tmp2,_Tmp3,_Tmp4,_Tmp5,(unsigned)((yyyvsp[0]).l).first_line);});
# 1693
yyval=Cyc_YY21(({void*_Tmp2=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_Tmp3->tag=10,_Tmp3->f1=dd,_Tmp3->f2=0;_Tmp3;});Cyc_Parse_type_spec(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1695
goto _LL0;}case 159:  {
# 1696 "parse.y"
int is_extensible=Cyc_yyget_YY32(&(yyyvsp[0]).v);
yyval=Cyc_YY21(({void*_Tmp2=({union Cyc_Absyn_DatatypeInfo _Tmp3=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _Tmp4;({struct _tuple0*_Tmp5=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_Tmp4.name=_Tmp5;}),_Tmp4.is_extensible=is_extensible;_Tmp4;}));Cyc_Absyn_datatype_type(_Tmp3,Cyc_yyget_YY41(&(yyyvsp[2]).v));});Cyc_Parse_type_spec(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1699
goto _LL0;}case 160:  {
# 1700 "parse.y"
int is_extensible=Cyc_yyget_YY32(&(yyyvsp[0]).v);
yyval=Cyc_YY21(({void*_Tmp2=({union Cyc_Absyn_DatatypeFieldInfo _Tmp3=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _Tmp4;({struct _tuple0*_Tmp5=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_Tmp4.datatype_name=_Tmp5;}),({struct _tuple0*_Tmp5=Cyc_yyget_QualId_tok(&(yyyvsp[3]).v);_Tmp4.field_name=_Tmp5;}),_Tmp4.is_extensible=is_extensible;_Tmp4;}));Cyc_Absyn_datatype_field_type(_Tmp3,Cyc_yyget_YY41(&(yyyvsp[4]).v));});Cyc_Parse_type_spec(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1703
goto _LL0;}case 161:
# 1706 "parse.y"
 yyval=Cyc_YY32(0);
goto _LL0;case 162:
# 1707 "parse.y"
 yyval=Cyc_YY32(1);
goto _LL0;case 163:
# 1711 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp3=Cyc_yyget_YY34(&(yyyvsp[0]).v);_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 164:
# 1712 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp3=Cyc_yyget_YY34(&(yyyvsp[0]).v);_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 165:
# 1713 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp3=Cyc_yyget_YY34(&(yyyvsp[0]).v);_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY35(&(yyyvsp[2]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 166:
# 1714 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp3=Cyc_yyget_YY34(&(yyyvsp[0]).v);_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY35(&(yyyvsp[2]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
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
 yyval=Cyc_YY34(({struct Cyc_Absyn_Datatypefield*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));({struct _tuple0*_Tmp3=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_Tmp2->name=_Tmp3;}),_Tmp2->typs=0,_Tmp2->loc=(unsigned)((yyyvsp[0]).l).first_line,({enum Cyc_Absyn_Scope _Tmp3=Cyc_yyget_YY33(&(yyyvsp[0]).v);_Tmp2->sc=_Tmp3;});_Tmp2;}));
goto _LL0;case 171:  {
# 1726 "parse.y"
struct Cyc_List_List*typs=({struct Cyc_List_List*(*_Tmp2)(struct _tuple20*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct _tuple20*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;});struct _tuple20*(*_Tmp3)(unsigned,struct _tuple8*)=({(struct _tuple20*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ;});unsigned _Tmp4=(unsigned)((yyyvsp[3]).l).first_line;_Tmp2(_Tmp3,_Tmp4,({struct Cyc_List_List*(*_Tmp5)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_Tmp5(Cyc_yyget_YY39(&(yyyvsp[3]).v));}));});
yyval=Cyc_YY34(({struct Cyc_Absyn_Datatypefield*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));({struct _tuple0*_Tmp3=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_Tmp2->name=_Tmp3;}),_Tmp2->typs=typs,_Tmp2->loc=(unsigned)((yyyvsp[0]).l).first_line,({enum Cyc_Absyn_Scope _Tmp3=Cyc_yyget_YY33(&(yyyvsp[0]).v);_Tmp2->sc=_Tmp3;});_Tmp2;}));
goto _LL0;}case 172:
# 1732 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 173:  {
# 1734 "parse.y"
struct Cyc_Parse_Declarator two=Cyc_yyget_YY28(&(yyyvsp[1]).v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp2;_Tmp2.id=two.id,_Tmp2.varloc=two.varloc,({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*(*_Tmp4)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append;});struct Cyc_List_List*_Tmp5=Cyc_yyget_YY27(&(yyyvsp[0]).v);_Tmp4(_Tmp5,two.tms);});_Tmp2.tms=_Tmp3;});_Tmp2;}));
goto _LL0;}case 174:
# 1741 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 175:  {
# 1743 "parse.y"
struct Cyc_Parse_Declarator two=Cyc_yyget_YY28(&(yyyvsp[1]).v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp2;_Tmp2.id=two.id,_Tmp2.varloc=two.varloc,({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*(*_Tmp4)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append;});struct Cyc_List_List*_Tmp5=Cyc_yyget_YY27(&(yyyvsp[0]).v);_Tmp4(_Tmp5,two.tms);});_Tmp2.tms=_Tmp3;});_Tmp2;}));
goto _LL0;}case 176:
# 1748 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp2;({struct _tuple0*_Tmp3=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_Tmp2.id=_Tmp3;}),_Tmp2.varloc=(unsigned)((yyyvsp[0]).l).first_line,_Tmp2.tms=0;_Tmp2;}));
goto _LL0;case 177:
# 1749 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 178:  {
# 1753 "parse.y"
struct Cyc_Parse_Declarator d=Cyc_yyget_YY28(&(yyyvsp[2]).v);
({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp5->tag=5,_Tmp5->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY46(&(yyyvsp[1]).v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=d.tms;_Tmp3;});d.tms=_Tmp2;});
yyval=(yyyvsp[2]).v;
# 1757
goto _LL0;}case 179:
# 1758 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp2;({struct _tuple0*_Tmp3=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).id;_Tmp2.id=_Tmp3;}),({unsigned _Tmp3=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).varloc;_Tmp2.varloc=_Tmp3;}),({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp5=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp6=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp6->tag=0,({void*_Tmp7=Cyc_yyget_YY52(&(yyyvsp[3]).v);_Tmp6->f1=_Tmp7;}),_Tmp6->f2=(unsigned)((yyyvsp[3]).l).first_line;_Tmp6;});_Tmp4->hd=_Tmp5;}),({struct Cyc_List_List*_Tmp5=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).tms;_Tmp4->tl=_Tmp5;});_Tmp4;});_Tmp2.tms=_Tmp3;});_Tmp2;}));
goto _LL0;case 180:
# 1760 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp2;({struct _tuple0*_Tmp3=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).id;_Tmp2.id=_Tmp3;}),({unsigned _Tmp3=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).varloc;_Tmp2.varloc=_Tmp3;}),({
struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp5=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp6=_region_malloc(yyr,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp6->tag=1,({struct Cyc_Absyn_Exp*_Tmp7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_Tmp6->f1=_Tmp7;}),({void*_Tmp7=Cyc_yyget_YY52(&(yyyvsp[4]).v);_Tmp6->f2=_Tmp7;}),_Tmp6->f3=(unsigned)((yyyvsp[4]).l).first_line;_Tmp6;});_Tmp4->hd=_Tmp5;}),({struct Cyc_List_List*_Tmp5=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).tms;_Tmp4->tl=_Tmp5;});_Tmp4;});_Tmp2.tms=_Tmp3;});_Tmp2;}));
goto _LL0;case 181:  {
# 1763 "parse.y"
struct _tuple28*_stmttmp18=Cyc_yyget_YY40(&(yyyvsp[2]).v);struct _tuple28*_Tmp2=_stmttmp18;void*_Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;void*_Tmp7;_Tmp7=_Tmp2->f1;_Tmp6=_Tmp2->f2;_Tmp5=_Tmp2->f3;_Tmp4=_Tmp2->f4;_Tmp3=_Tmp2->f5;{struct Cyc_List_List*lis=_Tmp7;int b=_Tmp6;struct Cyc_Absyn_VarargInfo*c=_Tmp5;void*eff=_Tmp4;struct Cyc_List_List*po=_Tmp3;
struct Cyc_Absyn_Exp*req=(Cyc_yyget_YY59(&(yyyvsp[4]).v)).f1;
struct Cyc_Absyn_Exp*ens=(Cyc_yyget_YY59(&(yyyvsp[4]).v)).f2;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp8;({struct _tuple0*_Tmp9=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).id;_Tmp8.id=_Tmp9;}),({unsigned _Tmp9=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).varloc;_Tmp8.varloc=_Tmp9;}),({struct Cyc_List_List*_Tmp9=({struct Cyc_List_List*_TmpA=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_TmpB=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_TmpC=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_TmpC->tag=3,({void*_TmpD=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_TmpE=_region_malloc(yyr,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_TmpE->tag=1,_TmpE->f1=lis,_TmpE->f2=b,_TmpE->f3=c,_TmpE->f4=eff,_TmpE->f5=po,_TmpE->f6=req,_TmpE->f7=ens;_TmpE;});_TmpC->f1=_TmpD;});_TmpC;});_TmpA->hd=_TmpB;}),({struct Cyc_List_List*_TmpB=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).tms;_TmpA->tl=_TmpB;});_TmpA;});_Tmp8.tms=_Tmp9;});_Tmp8;}));
# 1768
goto _LL0;}}case 182:
# 1769 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp2;({struct _tuple0*_Tmp3=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).id;_Tmp2.id=_Tmp3;}),({unsigned _Tmp3=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).varloc;_Tmp2.varloc=_Tmp3;}),({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp5=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp6=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp6->tag=3,({void*_Tmp7=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_Tmp8=_region_malloc(yyr,sizeof(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct));_Tmp8->tag=0,({struct Cyc_List_List*_Tmp9=Cyc_yyget_YY37(&(yyyvsp[2]).v);_Tmp8->f1=_Tmp9;}),_Tmp8->f2=(unsigned)((yyyvsp[0]).l).first_line;_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;});_Tmp4->hd=_Tmp5;}),({struct Cyc_List_List*_Tmp5=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).tms;_Tmp4->tl=_Tmp5;});_Tmp4;});_Tmp2.tms=_Tmp3;});_Tmp2;}));
goto _LL0;case 183:  {
# 1772
struct Cyc_List_List*ts=({struct Cyc_List_List*(*_Tmp2)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;});unsigned _Tmp3=(unsigned)((yyyvsp[1]).l).first_line;_Tmp2(Cyc_Parse_typ2tvar,_Tmp3,({struct Cyc_List_List*(*_Tmp4)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_Tmp4(Cyc_yyget_YY41(&(yyyvsp[2]).v));}));});
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp2;({struct _tuple0*_Tmp3=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).id;_Tmp2.id=_Tmp3;}),({unsigned _Tmp3=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).varloc;_Tmp2.varloc=_Tmp3;}),({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp5=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_Tmp6=_region_malloc(yyr,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_Tmp6->tag=4,_Tmp6->f1=ts,_Tmp6->f2=(unsigned)((yyyvsp[0]).l).first_line,_Tmp6->f3=0;_Tmp6;});_Tmp4->hd=_Tmp5;}),({struct Cyc_List_List*_Tmp5=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).tms;_Tmp4->tl=_Tmp5;});_Tmp4;});_Tmp2.tms=_Tmp3;});_Tmp2;}));
# 1775
goto _LL0;}case 184:
# 1776 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp2;({struct _tuple0*_Tmp3=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).id;_Tmp2.id=_Tmp3;}),({unsigned _Tmp3=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).varloc;_Tmp2.varloc=_Tmp3;}),({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp5=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp6=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp6->tag=5,_Tmp6->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_Tmp7=Cyc_yyget_YY46(&(yyyvsp[1]).v);_Tmp6->f2=_Tmp7;});_Tmp6;});_Tmp4->hd=_Tmp5;}),({
struct Cyc_List_List*_Tmp5=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).tms;_Tmp4->tl=_Tmp5;});_Tmp4;});
# 1776
_Tmp2.tms=_Tmp3;});_Tmp2;}));
# 1779
goto _LL0;case 185:
# 1783 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp2;({struct _tuple0*_Tmp3=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_Tmp2.id=_Tmp3;}),_Tmp2.varloc=(unsigned)((yyyvsp[0]).l).first_line,_Tmp2.tms=0;_Tmp2;}));
goto _LL0;case 186:
# 1784 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp2;({struct _tuple0*_Tmp3=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_Tmp2.id=_Tmp3;}),_Tmp2.varloc=(unsigned)((yyyvsp[0]).l).first_line,_Tmp2.tms=0;_Tmp2;}));
goto _LL0;case 187:
# 1785 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 188:  {
# 1789 "parse.y"
struct Cyc_Parse_Declarator d=Cyc_yyget_YY28(&(yyyvsp[2]).v);
({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp5->tag=5,_Tmp5->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY46(&(yyyvsp[1]).v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=d.tms;_Tmp3;});d.tms=_Tmp2;});
yyval=(yyyvsp[2]).v;
# 1793
goto _LL0;}case 189:  {
# 1794 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(&(yyyvsp[0]).v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp2;_Tmp2.id=one.id,_Tmp2.varloc=one.varloc,({
struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp5=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp6=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp6->tag=0,({void*_Tmp7=Cyc_yyget_YY52(&(yyyvsp[3]).v);_Tmp6->f1=_Tmp7;}),_Tmp6->f2=(unsigned)((yyyvsp[3]).l).first_line;_Tmp6;});_Tmp4->hd=_Tmp5;}),_Tmp4->tl=one.tms;_Tmp4;});_Tmp2.tms=_Tmp3;});_Tmp2;}));
goto _LL0;}case 190:  {
# 1798 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(&(yyyvsp[0]).v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp2;_Tmp2.id=one.id,_Tmp2.varloc=one.varloc,({
struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp5=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp6=_region_malloc(yyr,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp6->tag=1,({struct Cyc_Absyn_Exp*_Tmp7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_Tmp6->f1=_Tmp7;}),({void*_Tmp7=Cyc_yyget_YY52(&(yyyvsp[4]).v);_Tmp6->f2=_Tmp7;}),_Tmp6->f3=(unsigned)((yyyvsp[4]).l).first_line;_Tmp6;});_Tmp4->hd=_Tmp5;}),_Tmp4->tl=one.tms;_Tmp4;});_Tmp2.tms=_Tmp3;});_Tmp2;}));
# 1802
goto _LL0;}case 191:  {
# 1803 "parse.y"
struct _tuple28*_stmttmp19=Cyc_yyget_YY40(&(yyyvsp[2]).v);struct _tuple28*_Tmp2=_stmttmp19;void*_Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;void*_Tmp7;_Tmp7=_Tmp2->f1;_Tmp6=_Tmp2->f2;_Tmp5=_Tmp2->f3;_Tmp4=_Tmp2->f4;_Tmp3=_Tmp2->f5;{struct Cyc_List_List*lis=_Tmp7;int b=_Tmp6;struct Cyc_Absyn_VarargInfo*c=_Tmp5;void*eff=_Tmp4;struct Cyc_List_List*po=_Tmp3;
struct Cyc_Absyn_Exp*req=(Cyc_yyget_YY59(&(yyyvsp[4]).v)).f1;
struct Cyc_Absyn_Exp*ens=(Cyc_yyget_YY59(&(yyyvsp[4]).v)).f2;
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(&(yyyvsp[0]).v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp8;_Tmp8.id=one.id,_Tmp8.varloc=one.varloc,({struct Cyc_List_List*_Tmp9=({struct Cyc_List_List*_TmpA=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_TmpB=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_TmpC=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_TmpC->tag=3,({void*_TmpD=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_TmpE=_region_malloc(yyr,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_TmpE->tag=1,_TmpE->f1=lis,_TmpE->f2=b,_TmpE->f3=c,_TmpE->f4=eff,_TmpE->f5=po,_TmpE->f6=req,_TmpE->f7=ens;_TmpE;});_TmpC->f1=_TmpD;});_TmpC;});_TmpA->hd=_TmpB;}),_TmpA->tl=one.tms;_TmpA;});_Tmp8.tms=_Tmp9;});_Tmp8;}));
# 1809
goto _LL0;}}case 192:  {
# 1810 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(&(yyyvsp[0]).v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp2;_Tmp2.id=one.id,_Tmp2.varloc=one.varloc,({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp5=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp6=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp6->tag=3,({void*_Tmp7=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_Tmp8=_region_malloc(yyr,sizeof(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct));_Tmp8->tag=0,({struct Cyc_List_List*_Tmp9=Cyc_yyget_YY37(&(yyyvsp[2]).v);_Tmp8->f1=_Tmp9;}),_Tmp8->f2=(unsigned)((yyyvsp[0]).l).first_line;_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;});_Tmp4->hd=_Tmp5;}),_Tmp4->tl=one.tms;_Tmp4;});_Tmp2.tms=_Tmp3;});_Tmp2;}));
goto _LL0;}case 193:  {
# 1814
struct Cyc_List_List*ts=({struct Cyc_List_List*(*_Tmp2)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;});unsigned _Tmp3=(unsigned)((yyyvsp[1]).l).first_line;_Tmp2(Cyc_Parse_typ2tvar,_Tmp3,({struct Cyc_List_List*(*_Tmp4)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_Tmp4(Cyc_yyget_YY41(&(yyyvsp[2]).v));}));});
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(&(yyyvsp[0]).v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp2;_Tmp2.id=one.id,_Tmp2.varloc=one.varloc,({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp5=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_Tmp6=_region_malloc(yyr,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_Tmp6->tag=4,_Tmp6->f1=ts,_Tmp6->f2=(unsigned)((yyyvsp[0]).l).first_line,_Tmp6->f3=0;_Tmp6;});_Tmp4->hd=_Tmp5;}),_Tmp4->tl=one.tms;_Tmp4;});_Tmp2.tms=_Tmp3;});_Tmp2;}));
goto _LL0;}case 194:  {
# 1818 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(&(yyyvsp[0]).v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp2;_Tmp2.id=one.id,_Tmp2.varloc=one.varloc,({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp5=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp6=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp6->tag=5,_Tmp6->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_Tmp7=Cyc_yyget_YY46(&(yyyvsp[1]).v);_Tmp6->f2=_Tmp7;});_Tmp6;});_Tmp4->hd=_Tmp5;}),_Tmp4->tl=one.tms;_Tmp4;});_Tmp2.tms=_Tmp3;});_Tmp2;}));
goto _LL0;}case 195:
# 1824 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 196:
# 1825 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_List_List*(*_Tmp2)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append;});struct Cyc_List_List*_Tmp3=Cyc_yyget_YY27(&(yyyvsp[0]).v);_Tmp2(_Tmp3,Cyc_yyget_YY27(&(yyyvsp[1]).v));}));
goto _LL0;case 197:  {
# 1829 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY46(&(yyyvsp[3]).v)!= 0)
ans=({struct Cyc_List_List*_Tmp2=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp3=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp4=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp4->tag=5,_Tmp4->f1=(unsigned)((yyyvsp[3]).l).first_line,({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY46(&(yyyvsp[3]).v);_Tmp4->f2=_Tmp5;});_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=ans;_Tmp2;});{
# 1833
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple22 _stmttmp1A=*Cyc_yyget_YY1(&(yyyvsp[0]).v);struct _tuple22 _Tmp2=_stmttmp1A;void*_Tmp3;void*_Tmp4;unsigned _Tmp5;_Tmp5=_Tmp2.f1;_Tmp4=_Tmp2.f2;_Tmp3=_Tmp2.f3;{unsigned ploc=_Tmp5;void*nullable=_Tmp4;void*bound=_Tmp3;
if(Cyc_Flags_porting_c_code)
ptrloc=({struct Cyc_Absyn_PtrLoc*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_PtrLoc));_Tmp6->ptr_loc=ploc,_Tmp6->rgn_loc=(unsigned)((yyyvsp[2]).l).first_line,_Tmp6->zt_loc=(unsigned)((yyyvsp[1]).l).first_line;_Tmp6;});{
struct _tuple15 _stmttmp1B=({unsigned _Tmp6=ploc;void*_Tmp7=nullable;void*_Tmp8=bound;void*_Tmp9=Cyc_yyget_YY45(&(yyyvsp[2]).v);Cyc_Parse_collapse_pointer_quals(_Tmp6,_Tmp7,_Tmp8,_Tmp9,Cyc_yyget_YY57(&(yyyvsp[1]).v));});struct _tuple15 _Tmp6=_stmttmp1B;void*_Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;void*_TmpB;_TmpB=_Tmp6.f1;_TmpA=_Tmp6.f2;_Tmp9=_Tmp6.f3;_Tmp8=_Tmp6.f4;_Tmp7=_Tmp6.f5;{void*nullable=_TmpB;void*bound=_TmpA;void*zeroterm=_Tmp9;void*rgn_opt=_Tmp8;void*autoreleased=_Tmp7;
ans=({struct Cyc_List_List*_TmpC=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_TmpD=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_TmpE=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct));_TmpE->tag=2,(_TmpE->f1).rgn=rgn_opt,(_TmpE->f1).nullable=nullable,(_TmpE->f1).bounds=bound,(_TmpE->f1).zero_term=zeroterm,(_TmpE->f1).ptrloc=ptrloc,(_TmpE->f1).autoreleased=autoreleased,({struct Cyc_Absyn_Tqual _TmpF=Cyc_yyget_YY24(&(yyyvsp[4]).v);_TmpE->f2=_TmpF;});_TmpE;});_TmpC->hd=_TmpD;}),_TmpC->tl=ans;_TmpC;});
yyval=Cyc_YY27(ans);
# 1841
goto _LL0;}}}}}case 198:
# 1843
 yyval=Cyc_YY57(0);
goto _LL0;case 199:
# 1844 "parse.y"
 yyval=Cyc_YY57(({struct Cyc_List_List*_Tmp2=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp3=Cyc_yyget_YY56(&(yyyvsp[0]).v);_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY57(&(yyyvsp[1]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 200:
# 1849 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_Tmp2=_region_malloc(yyr,sizeof(struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct));_Tmp2->tag=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_Tmp2->f1=_Tmp3;});_Tmp2;}));
goto _LL0;case 201:
# 1850 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*_Tmp2=_region_malloc(yyr,sizeof(struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct));_Tmp2->tag=1,({void*_Tmp3=Cyc_yyget_YY45(&(yyyvsp[2]).v);_Tmp2->f1=_Tmp3;});_Tmp2;}));
goto _LL0;case 202:
# 1851 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_Tmp2=_region_malloc(yyr,sizeof(struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct));_Tmp2->tag=2;_Tmp2;}));
goto _LL0;case 203:
# 1852 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_Tmp2=_region_malloc(yyr,sizeof(struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct));_Tmp2->tag=3;_Tmp2;}));
goto _LL0;case 204:
# 1853 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct*_Tmp2=_region_malloc(yyr,sizeof(struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct));_Tmp2->tag=6;_Tmp2;}));
goto _LL0;case 205:
# 1854 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_Tmp2=_region_malloc(yyr,sizeof(struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct));_Tmp2->tag=4;_Tmp2;}));
goto _LL0;case 206:
# 1855 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_Tmp2=_region_malloc(yyr,sizeof(struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct));_Tmp2->tag=5;_Tmp2;}));
goto _LL0;case 207:
# 1856 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_Tmp2=_region_malloc(yyr,sizeof(struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct));_Tmp2->tag=7;_Tmp2;}));
goto _LL0;case 208:
# 1857 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_Tmp2=_region_malloc(yyr,sizeof(struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct));_Tmp2->tag=8;_Tmp2;}));
goto _LL0;case 209:
# 1863 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_Tmp2=_cycalloc(sizeof(struct _tuple22));_Tmp2->f1=(unsigned)((yyyvsp[0]).l).first_line,_Tmp2->f2=Cyc_Absyn_true_type,Cyc_Parse_parsing_tempest?_Tmp2->f3=Cyc_Absyn_fat_bound_type:({void*_Tmp3=Cyc_yyget_YY2(&(yyyvsp[1]).v);_Tmp2->f3=_Tmp3;});_Tmp2;}));
goto _LL0;case 210:
# 1864 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_Tmp2=_cycalloc(sizeof(struct _tuple22));_Tmp2->f1=(unsigned)((yyyvsp[0]).l).first_line,_Tmp2->f2=Cyc_Absyn_false_type,({void*_Tmp3=Cyc_yyget_YY2(&(yyyvsp[1]).v);_Tmp2->f3=_Tmp3;});_Tmp2;}));
goto _LL0;case 211:
# 1865 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_Tmp2=_cycalloc(sizeof(struct _tuple22));_Tmp2->f1=(unsigned)((yyyvsp[0]).l).first_line,_Tmp2->f2=Cyc_Absyn_true_type,_Tmp2->f3=Cyc_Absyn_fat_bound_type;_Tmp2;}));
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
 yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _Tmp2=Cyc_yyget_YY24(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_Tmp2,Cyc_yyget_YY24(&(yyyvsp[1]).v));}));
goto _LL0;case 222:
# 1890 "parse.y"
 yyval=Cyc_YY40(({struct _tuple28*_Tmp2=_cycalloc(sizeof(struct _tuple28));_Tmp2->f1=0,_Tmp2->f2=0,_Tmp2->f3=0,({void*_Tmp3=Cyc_yyget_YY50(&(yyyvsp[0]).v);_Tmp2->f4=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY51(&(yyyvsp[1]).v);_Tmp2->f5=_Tmp3;});_Tmp2;}));
goto _LL0;case 223:
# 1892 "parse.y"
 yyval=Cyc_YY40(({struct _tuple28*_Tmp2=_cycalloc(sizeof(struct _tuple28));({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*(*_Tmp4)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_Tmp4(Cyc_yyget_YY39(&(yyyvsp[0]).v));});_Tmp2->f1=_Tmp3;}),_Tmp2->f2=0,_Tmp2->f3=0,({void*_Tmp3=Cyc_yyget_YY50(&(yyyvsp[1]).v);_Tmp2->f4=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY51(&(yyyvsp[2]).v);_Tmp2->f5=_Tmp3;});_Tmp2;}));
goto _LL0;case 224:
# 1894 "parse.y"
 yyval=Cyc_YY40(({struct _tuple28*_Tmp2=_cycalloc(sizeof(struct _tuple28));({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*(*_Tmp4)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_Tmp4(Cyc_yyget_YY39(&(yyyvsp[0]).v));});_Tmp2->f1=_Tmp3;}),_Tmp2->f2=1,_Tmp2->f3=0,({void*_Tmp3=Cyc_yyget_YY50(&(yyyvsp[3]).v);_Tmp2->f4=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY51(&(yyyvsp[4]).v);_Tmp2->f5=_Tmp3;});_Tmp2;}));
goto _LL0;case 225:  {
# 1897
struct _tuple8*_stmttmp1C=Cyc_yyget_YY38(&(yyyvsp[2]).v);struct _tuple8*_Tmp2=_stmttmp1C;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;void*_Tmp5;_Tmp5=_Tmp2->f1;_Tmp4=_Tmp2->f2;_Tmp3=_Tmp2->f3;{struct _fat_ptr*n=_Tmp5;struct Cyc_Absyn_Tqual tq=_Tmp4;void*t=_Tmp3;
struct Cyc_Absyn_VarargInfo*v=({struct Cyc_Absyn_VarargInfo*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo));_Tmp6->name=n,_Tmp6->tq=tq,_Tmp6->type=t,({int _Tmp7=Cyc_yyget_YY32(&(yyyvsp[1]).v);_Tmp6->inject=_Tmp7;});_Tmp6;});
yyval=Cyc_YY40(({struct _tuple28*_Tmp6=_cycalloc(sizeof(struct _tuple28));_Tmp6->f1=0,_Tmp6->f2=0,_Tmp6->f3=v,({void*_Tmp7=Cyc_yyget_YY50(&(yyyvsp[3]).v);_Tmp6->f4=_Tmp7;}),({struct Cyc_List_List*_Tmp7=Cyc_yyget_YY51(&(yyyvsp[4]).v);_Tmp6->f5=_Tmp7;});_Tmp6;}));
# 1901
goto _LL0;}}case 226:  {
# 1903
struct _tuple8*_stmttmp1D=Cyc_yyget_YY38(&(yyyvsp[4]).v);struct _tuple8*_Tmp2=_stmttmp1D;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;void*_Tmp5;_Tmp5=_Tmp2->f1;_Tmp4=_Tmp2->f2;_Tmp3=_Tmp2->f3;{struct _fat_ptr*n=_Tmp5;struct Cyc_Absyn_Tqual tq=_Tmp4;void*t=_Tmp3;
struct Cyc_Absyn_VarargInfo*v=({struct Cyc_Absyn_VarargInfo*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo));_Tmp6->name=n,_Tmp6->tq=tq,_Tmp6->type=t,({int _Tmp7=Cyc_yyget_YY32(&(yyyvsp[3]).v);_Tmp6->inject=_Tmp7;});_Tmp6;});
yyval=Cyc_YY40(({struct _tuple28*_Tmp6=_cycalloc(sizeof(struct _tuple28));({struct Cyc_List_List*_Tmp7=({struct Cyc_List_List*(*_Tmp8)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_Tmp8(Cyc_yyget_YY39(&(yyyvsp[0]).v));});_Tmp6->f1=_Tmp7;}),_Tmp6->f2=0,_Tmp6->f3=v,({void*_Tmp7=Cyc_yyget_YY50(&(yyyvsp[5]).v);_Tmp6->f4=_Tmp7;}),({struct Cyc_List_List*_Tmp7=Cyc_yyget_YY51(&(yyyvsp[6]).v);_Tmp6->f5=_Tmp7;});_Tmp6;}));
# 1907
goto _LL0;}}case 227:
# 1911 "parse.y"
 yyval=Cyc_YY45(({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Parse_id2type(_Tmp2,(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct));_Tmp3->tag=1,_Tmp3->f1=0;_Tmp3;}));}));
goto _LL0;case 228:
# 1912 "parse.y"
 yyval=Cyc_YY45(({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Parse_id2type(_Tmp2,Cyc_Kinds_kind_to_bound(Cyc_yyget_YY44(&(yyyvsp[2]).v)));}));
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
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_Tmp2->tag=2,_Tmp2->f1=0,_Tmp2->f2=& Cyc_Kinds_trk;_Tmp2;});
void*t=({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_Tmp2,(void*)kb);});
yyval=Cyc_YY51(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple34*_Tmp3=({struct _tuple34*_Tmp4=_cycalloc(sizeof(struct _tuple34));({void*_Tmp5=Cyc_Absyn_join_eff(Cyc_yyget_YY41(&(yyyvsp[0]).v));_Tmp4->f1=_Tmp5;}),_Tmp4->f2=t;_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
# 1933
goto _LL0;}case 234:  {
# 1935 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_Tmp2->tag=2,_Tmp2->f1=0,_Tmp2->f2=& Cyc_Kinds_trk;_Tmp2;});
void*t=({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_Tmp2,(void*)kb);});
yyval=Cyc_YY51(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple34*_Tmp3=({struct _tuple34*_Tmp4=_cycalloc(sizeof(struct _tuple34));({void*_Tmp5=Cyc_Absyn_join_eff(Cyc_yyget_YY41(&(yyyvsp[0]).v));_Tmp4->f1=_Tmp5;}),_Tmp4->f2=t;_Tmp4;});_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY51(&(yyyvsp[4]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
# 1939
goto _LL0;}case 235:
# 1943 "parse.y"
 yyval=Cyc_YY32(0);
goto _LL0;case 236:
# 1945 "parse.y"
 if(({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_zstrcmp(_Tmp2,({const char*_Tmp3="inject";_tag_fat(_Tmp3,sizeof(char),7U);}));})!= 0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4="missing type in function declaration";_tag_fat(_Tmp4,sizeof(char),37U);});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_Warn_err2((unsigned)((yyyvsp[0]).l).first_line,_tag_fat(_Tmp3,sizeof(void*),1));});
yyval=Cyc_YY32(1);
# 1949
goto _LL0;case 237:
# 1952 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 238:
# 1953 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*(*_Tmp2)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append;});struct Cyc_List_List*_Tmp3=Cyc_yyget_YY41(&(yyyvsp[0]).v);_Tmp2(_Tmp3,Cyc_yyget_YY41(&(yyyvsp[2]).v));}));
goto _LL0;case 239:
# 1957 "parse.y"
 yyval=Cyc_YY41(0);
goto _LL0;case 240:
# 1958 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 241:
# 1959 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp3=Cyc_Absyn_regionsof_eff(Cyc_yyget_YY45(&(yyyvsp[2]).v));_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 242:
# 1961 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY45(&(yyyvsp[0]).v),& Cyc_Kinds_ek,0);
yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp3=Cyc_yyget_YY45(&(yyyvsp[0]).v);_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
# 1964
goto _LL0;case 243:
# 1969 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp3=Cyc_Absyn_access_eff(({struct _tuple8*_Tmp4=Cyc_yyget_YY38(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_Tmp4,(unsigned)((yyyvsp[0]).l).first_line);}));_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 244:
# 1971 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp3=Cyc_Absyn_access_eff(({struct _tuple8*_Tmp4=Cyc_yyget_YY38(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_Tmp4,(unsigned)((yyyvsp[0]).l).first_line);}));_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY41(&(yyyvsp[2]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 245:
# 1976 "parse.y"
 yyval=Cyc_YY39(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple8*_Tmp3=Cyc_yyget_YY38(&(yyyvsp[0]).v);_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 246:
# 1977 "parse.y"
 yyval=Cyc_YY39(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple8*_Tmp3=Cyc_yyget_YY38(&(yyyvsp[2]).v);_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY39(&(yyyvsp[0]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 247:  {
# 1983 "parse.y"
struct _tuple27 _stmttmp1E=Cyc_yyget_YY36(&(yyyvsp[0]).v);struct _tuple27 _Tmp2=_stmttmp1E;void*_Tmp3;struct Cyc_Parse_Type_specifier _Tmp4;struct Cyc_Absyn_Tqual _Tmp5;_Tmp5=_Tmp2.f1;_Tmp4=_Tmp2.f2;_Tmp3=_Tmp2.f3;{struct Cyc_Absyn_Tqual tq=_Tmp5;struct Cyc_Parse_Type_specifier tspecs=_Tmp4;struct Cyc_List_List*atts=_Tmp3;
if(tq.loc == 0U)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
struct Cyc_Parse_Declarator _stmttmp1F=Cyc_yyget_YY28(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator _Tmp6=_stmttmp1F;void*_Tmp7;unsigned _Tmp8;void*_Tmp9;_Tmp9=_Tmp6.id;_Tmp8=_Tmp6.varloc;_Tmp7=_Tmp6.tms;{struct _tuple0*qv=_Tmp9;unsigned varloc=_Tmp8;struct Cyc_List_List*tms=_Tmp7;
void*t=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);
struct _tuple14 _stmttmp20=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 _TmpA=_stmttmp20;void*_TmpB;void*_TmpC;void*_TmpD;struct Cyc_Absyn_Tqual _TmpE;_TmpE=_TmpA.f1;_TmpD=_TmpA.f2;_TmpC=_TmpA.f3;_TmpB=_TmpA.f4;{struct Cyc_Absyn_Tqual tq2=_TmpE;void*t2=_TmpD;struct Cyc_List_List*tvs=_TmpC;struct Cyc_List_List*atts2=_TmpB;
if(tvs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpF=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp10;_Tmp10.tag=0,_Tmp10.f1=({const char*_Tmp11="parameter with bad type params";_tag_fat(_Tmp11,sizeof(char),31U);});_Tmp10;});void*_Tmp10[1];_Tmp10[0]=& _TmpF;Cyc_Warn_err2((unsigned)((yyyvsp[1]).l).first_line,_tag_fat(_Tmp10,sizeof(void*),1));});
if(Cyc_Absyn_is_qvar_qualified(qv))
({struct Cyc_Warn_String_Warn_Warg_struct _TmpF=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp10;_Tmp10.tag=0,_Tmp10.f1=({const char*_Tmp11="parameter cannot be qualified with a namespace";_tag_fat(_Tmp11,sizeof(char),47U);});_Tmp10;});void*_Tmp10[1];_Tmp10[0]=& _TmpF;Cyc_Warn_err2((unsigned)((yyyvsp[0]).l).first_line,_tag_fat(_Tmp10,sizeof(void*),1));});{
struct _fat_ptr*idopt=(*qv).f2;
if(atts2 != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpF=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp10;_Tmp10.tag=0,_Tmp10.f1=({const char*_Tmp11="extra attributes on parameter, ignoring";_tag_fat(_Tmp11,sizeof(char),40U);});_Tmp10;});void*_Tmp10[1];_Tmp10[0]=& _TmpF;Cyc_Warn_warn2((unsigned)((yyyvsp[0]).l).first_line,_tag_fat(_Tmp10,sizeof(void*),1));});
yyval=Cyc_YY38(({struct _tuple8*_TmpF=_cycalloc(sizeof(struct _tuple8));_TmpF->f1=idopt,_TmpF->f2=tq2,_TmpF->f3=t2;_TmpF;}));
# 1997
goto _LL0;}}}}}}case 248:  {
# 1998 "parse.y"
struct _tuple27 _stmttmp21=Cyc_yyget_YY36(&(yyyvsp[0]).v);struct _tuple27 _Tmp2=_stmttmp21;void*_Tmp3;struct Cyc_Parse_Type_specifier _Tmp4;struct Cyc_Absyn_Tqual _Tmp5;_Tmp5=_Tmp2.f1;_Tmp4=_Tmp2.f2;_Tmp3=_Tmp2.f3;{struct Cyc_Absyn_Tqual tq=_Tmp5;struct Cyc_Parse_Type_specifier tspecs=_Tmp4;struct Cyc_List_List*atts=_Tmp3;
if(tq.loc == 0U)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
void*t=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);
if(atts != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=({const char*_Tmp8="bad attributes on parameter, ignoring";_tag_fat(_Tmp8,sizeof(char),38U);});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_Warn_warn2((unsigned)((yyyvsp[0]).l).first_line,_tag_fat(_Tmp7,sizeof(void*),1));});
yyval=Cyc_YY38(({struct _tuple8*_Tmp6=_cycalloc(sizeof(struct _tuple8));_Tmp6->f1=0,_Tmp6->f2=tq,_Tmp6->f3=t;_Tmp6;}));
# 2005
goto _LL0;}}}case 249:  {
# 2006 "parse.y"
struct _tuple27 _stmttmp22=Cyc_yyget_YY36(&(yyyvsp[0]).v);struct _tuple27 _Tmp2=_stmttmp22;void*_Tmp3;struct Cyc_Parse_Type_specifier _Tmp4;struct Cyc_Absyn_Tqual _Tmp5;_Tmp5=_Tmp2.f1;_Tmp4=_Tmp2.f2;_Tmp3=_Tmp2.f3;{struct Cyc_Absyn_Tqual tq=_Tmp5;struct Cyc_Parse_Type_specifier tspecs=_Tmp4;struct Cyc_List_List*atts=_Tmp3;
if(tq.loc == 0U)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
void*t=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);
struct Cyc_List_List*tms=(Cyc_yyget_YY31(&(yyyvsp[1]).v)).tms;
struct _tuple14 _stmttmp23=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 _Tmp6=_stmttmp23;void*_Tmp7;void*_Tmp8;void*_Tmp9;struct Cyc_Absyn_Tqual _TmpA;_TmpA=_Tmp6.f1;_Tmp9=_Tmp6.f2;_Tmp8=_Tmp6.f3;_Tmp7=_Tmp6.f4;{struct Cyc_Absyn_Tqual tq2=_TmpA;void*t2=_Tmp9;struct Cyc_List_List*tvs=_Tmp8;struct Cyc_List_List*atts2=_Tmp7;
if(tvs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=({const char*_TmpD="bad type parameters on formal argument, ignoring";_tag_fat(_TmpD,sizeof(char),49U);});_TmpC;});void*_TmpC[1];_TmpC[0]=& _TmpB;Cyc_Warn_warn2((unsigned)((yyyvsp[0]).l).first_line,_tag_fat(_TmpC,sizeof(void*),1));});
# 2014
if(atts2 != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=({const char*_TmpD="bad attributes on parameter, ignoring";_tag_fat(_TmpD,sizeof(char),38U);});_TmpC;});void*_TmpC[1];_TmpC[0]=& _TmpB;Cyc_Warn_warn2((unsigned)((yyyvsp[0]).l).first_line,_tag_fat(_TmpC,sizeof(void*),1));});
yyval=Cyc_YY38(({struct _tuple8*_TmpB=_cycalloc(sizeof(struct _tuple8));_TmpB->f1=0,_TmpB->f2=tq2,_TmpB->f3=t2;_TmpB;}));
# 2018
goto _LL0;}}}}case 250:
# 2021 "parse.y"
 yyval=Cyc_YY37(({struct Cyc_List_List*(*_Tmp2)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_Tmp2(Cyc_yyget_YY37(&(yyyvsp[0]).v));}));
goto _LL0;case 251:
# 2024
 yyval=Cyc_YY37(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 252:
# 2025 "parse.y"
 yyval=Cyc_YY37(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY37(&(yyyvsp[0]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 253:
# 2029 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 254:
# 2030 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 255:
# 2035 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp2=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_Tmp3->tag=36,_Tmp3->f1=0,_Tmp3->f2=0;_Tmp3;});Cyc_Absyn_new_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 256:
# 2037 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp2=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_Tmp3->tag=36,_Tmp3->f1=0,({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*(*_Tmp5)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_Tmp5(Cyc_yyget_YY5(&(yyyvsp[1]).v));});_Tmp3->f2=_Tmp4;});_Tmp3;});Cyc_Absyn_new_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 257:
# 2039 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp2=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_Tmp3->tag=36,_Tmp3->f1=0,({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*(*_Tmp5)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_Tmp5(Cyc_yyget_YY5(&(yyyvsp[1]).v));});_Tmp3->f2=_Tmp4;});_Tmp3;});Cyc_Absyn_new_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 258:  {
# 2041 "parse.y"
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp2=(unsigned)((yyyvsp[2]).l).first_line;struct _tuple0*_Tmp3=({struct _tuple0*_Tmp4=_cycalloc(sizeof(struct _tuple0));_Tmp4->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp5=({struct _fat_ptr*_Tmp6=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp7=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_Tmp6=_Tmp7;});_Tmp6;});_Tmp4->f2=_Tmp5;});_Tmp4;});void*_Tmp4=Cyc_Absyn_uint_type;Cyc_Absyn_new_vardecl(_Tmp2,_Tmp3,_Tmp4,
Cyc_Absyn_uint_exp(0U,(unsigned)((yyyvsp[2]).l).first_line));});
# 2044
(vd->tq).real_const=1;
yyval=Cyc_Exp_tok(({void*_Tmp2=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct));_Tmp3->tag=27,_Tmp3->f1=vd,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);_Tmp3->f2=_Tmp4;}),({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(&(yyyvsp[6]).v);_Tmp3->f3=_Tmp4;}),_Tmp3->f4=0;_Tmp3;});Cyc_Absyn_new_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2047
goto _LL0;}case 259:  {
# 2049 "parse.y"
void*t=({struct _tuple8*_Tmp2=Cyc_yyget_YY38(&(yyyvsp[6]).v);Cyc_Parse_type_name_to_type(_Tmp2,(unsigned)((yyyvsp[6]).l).first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp2=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct));_Tmp3->tag=28,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);_Tmp3->f1=_Tmp4;}),_Tmp3->f2=t,_Tmp3->f3=0;_Tmp3;});Cyc_Absyn_new_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2052
goto _LL0;}case 260:
# 2057 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple35*_Tmp3=({struct _tuple35*_Tmp4=_cycalloc(sizeof(struct _tuple35));_Tmp4->f1=0,({struct Cyc_Absyn_Exp*_Tmp5=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);_Tmp4->f2=_Tmp5;});_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 261:
# 2059 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple35*_Tmp3=({struct _tuple35*_Tmp4=_cycalloc(sizeof(struct _tuple35));({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY42(&(yyyvsp[0]).v);_Tmp4->f1=_Tmp5;}),({struct Cyc_Absyn_Exp*_Tmp5=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_Tmp4->f2=_Tmp5;});_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 262:
# 2061 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple35*_Tmp3=({struct _tuple35*_Tmp4=_cycalloc(sizeof(struct _tuple35));_Tmp4->f1=0,({struct Cyc_Absyn_Exp*_Tmp5=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_Tmp4->f2=_Tmp5;});_Tmp4;});_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY5(&(yyyvsp[0]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 263:
# 2063 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple35*_Tmp3=({struct _tuple35*_Tmp4=_cycalloc(sizeof(struct _tuple35));({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY42(&(yyyvsp[2]).v);_Tmp4->f1=_Tmp5;}),({struct Cyc_Absyn_Exp*_Tmp5=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_Tmp4->f2=_Tmp5;});_Tmp4;});_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY5(&(yyyvsp[0]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 264:
# 2067 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*(*_Tmp2)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_Tmp2(Cyc_yyget_YY42(&(yyyvsp[0]).v));}));
goto _LL0;case 265:
# 2068 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp3=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_Tmp4->tag=1,({struct _fat_ptr*_Tmp5=({struct _fat_ptr*_Tmp6=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp7=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_Tmp6=_Tmp7;});_Tmp6;});_Tmp4->f1=_Tmp5;});_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 266:
# 2073 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp3=Cyc_yyget_YY43(&(yyyvsp[0]).v);_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 267:
# 2074 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp3=Cyc_yyget_YY43(&(yyyvsp[1]).v);_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY42(&(yyyvsp[0]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 268:
# 2078 "parse.y"
 yyval=Cyc_YY43((void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct));_Tmp2->tag=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_Tmp2->f1=_Tmp3;});_Tmp2;}));
goto _LL0;case 269:
# 2079 "parse.y"
 yyval=Cyc_YY43((void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_Tmp2->tag=1,({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;});_Tmp2;}));
goto _LL0;case 270:  {
# 2084 "parse.y"
struct _tuple27 _stmttmp24=Cyc_yyget_YY36(&(yyyvsp[0]).v);struct _tuple27 _Tmp2=_stmttmp24;void*_Tmp3;struct Cyc_Parse_Type_specifier _Tmp4;struct Cyc_Absyn_Tqual _Tmp5;_Tmp5=_Tmp2.f1;_Tmp4=_Tmp2.f2;_Tmp3=_Tmp2.f3;{struct Cyc_Absyn_Tqual tq=_Tmp5;struct Cyc_Parse_Type_specifier tss=_Tmp4;struct Cyc_List_List*atts=_Tmp3;
void*t=Cyc_Parse_speclist2typ(tss,(unsigned)((yyyvsp[0]).l).first_line);
if(atts != 0)
Cyc_Warn_warn((unsigned)((yyyvsp[0]).l).first_line,({const char*_Tmp6="ignoring attributes in type";_tag_fat(_Tmp6,sizeof(char),28U);}),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY38(({struct _tuple8*_Tmp6=_cycalloc(sizeof(struct _tuple8));_Tmp6->f1=0,_Tmp6->f2=tq,_Tmp6->f3=t;_Tmp6;}));
# 2090
goto _LL0;}}case 271:  {
# 2091 "parse.y"
struct _tuple27 _stmttmp25=Cyc_yyget_YY36(&(yyyvsp[0]).v);struct _tuple27 _Tmp2=_stmttmp25;void*_Tmp3;struct Cyc_Parse_Type_specifier _Tmp4;struct Cyc_Absyn_Tqual _Tmp5;_Tmp5=_Tmp2.f1;_Tmp4=_Tmp2.f2;_Tmp3=_Tmp2.f3;{struct Cyc_Absyn_Tqual tq=_Tmp5;struct Cyc_Parse_Type_specifier tss=_Tmp4;struct Cyc_List_List*atts=_Tmp3;
void*t=Cyc_Parse_speclist2typ(tss,(unsigned)((yyyvsp[0]).l).first_line);
struct Cyc_List_List*tms=(Cyc_yyget_YY31(&(yyyvsp[1]).v)).tms;
struct _tuple14 t_info=Cyc_Parse_apply_tms(tq,t,atts,tms);
if(t_info.f3 != 0)
Cyc_Warn_warn((unsigned)((yyyvsp[1]).l).first_line,({const char*_Tmp6="bad type params, ignoring";_tag_fat(_Tmp6,sizeof(char),26U);}),_tag_fat(0U,sizeof(void*),0));
if(t_info.f4 != 0)
Cyc_Warn_warn((unsigned)((yyyvsp[1]).l).first_line,({const char*_Tmp6="bad specifiers, ignoring";_tag_fat(_Tmp6,sizeof(char),25U);}),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY38(({struct _tuple8*_Tmp6=_cycalloc(sizeof(struct _tuple8));_Tmp6->f1=0,_Tmp6->f2=t_info.f1,_Tmp6->f3=t_info.f2;_Tmp6;}));
# 2101
goto _LL0;}}case 272:
# 2104 "parse.y"
 yyval=Cyc_YY45(({struct _tuple8*_Tmp2=Cyc_yyget_YY38(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
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
 yyval=Cyc_YY45(Cyc_Absyn_join_eff(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp3=Cyc_yyget_YY45(&(yyyvsp[0]).v);_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY41(&(yyyvsp[2]).v);_Tmp2->tl=_Tmp3;});_Tmp2;})));
goto _LL0;case 277:
# 2114 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp3=Cyc_yyget_YY45(&(yyyvsp[0]).v);_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 278:
# 2115 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp3=Cyc_yyget_YY45(&(yyyvsp[2]).v);_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY41(&(yyyvsp[0]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 279:
# 2119 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp2;({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY27(&(yyyvsp[0]).v);_Tmp2.tms=_Tmp3;});_Tmp2;}));
goto _LL0;case 280:
# 2120 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 281:
# 2122 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp2;({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*(*_Tmp4)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append;});struct Cyc_List_List*_Tmp5=Cyc_yyget_YY27(&(yyyvsp[0]).v);_Tmp4(_Tmp5,(Cyc_yyget_YY31(&(yyyvsp[1]).v)).tms);});_Tmp2.tms=_Tmp3;});_Tmp2;}));
goto _LL0;case 282:
# 2127 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 283:
# 2129 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp2;({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp5=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp6=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp6->tag=0,({void*_Tmp7=Cyc_yyget_YY52(&(yyyvsp[2]).v);_Tmp6->f1=_Tmp7;}),_Tmp6->f2=(unsigned)((yyyvsp[2]).l).first_line;_Tmp6;});_Tmp4->hd=_Tmp5;}),_Tmp4->tl=0;_Tmp4;});_Tmp2.tms=_Tmp3;});_Tmp2;}));
goto _LL0;case 284:
# 2131 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp2;({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp5=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp6=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp6->tag=0,({void*_Tmp7=Cyc_yyget_YY52(&(yyyvsp[3]).v);_Tmp6->f1=_Tmp7;}),_Tmp6->f2=(unsigned)((yyyvsp[3]).l).first_line;_Tmp6;});_Tmp4->hd=_Tmp5;}),({struct Cyc_List_List*_Tmp5=(Cyc_yyget_YY31(&(yyyvsp[0]).v)).tms;_Tmp4->tl=_Tmp5;});_Tmp4;});_Tmp2.tms=_Tmp3;});_Tmp2;}));
goto _LL0;case 285:
# 2133 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp2;({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp5=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp6=_region_malloc(yyr,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp6->tag=1,({struct Cyc_Absyn_Exp*_Tmp7=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_Tmp6->f1=_Tmp7;}),({void*_Tmp7=Cyc_yyget_YY52(&(yyyvsp[3]).v);_Tmp6->f2=_Tmp7;}),_Tmp6->f3=(unsigned)((yyyvsp[3]).l).first_line;_Tmp6;});_Tmp4->hd=_Tmp5;}),_Tmp4->tl=0;_Tmp4;});_Tmp2.tms=_Tmp3;});_Tmp2;}));
goto _LL0;case 286:
# 2135 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp2;({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp5=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp6=_region_malloc(yyr,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp6->tag=1,({struct Cyc_Absyn_Exp*_Tmp7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_Tmp6->f1=_Tmp7;}),({void*_Tmp7=Cyc_yyget_YY52(&(yyyvsp[4]).v);_Tmp6->f2=_Tmp7;}),_Tmp6->f3=(unsigned)((yyyvsp[4]).l).first_line;_Tmp6;});_Tmp4->hd=_Tmp5;}),({
struct Cyc_List_List*_Tmp5=(Cyc_yyget_YY31(&(yyyvsp[0]).v)).tms;_Tmp4->tl=_Tmp5;});_Tmp4;});
# 2135
_Tmp2.tms=_Tmp3;});_Tmp2;}));
# 2138
goto _LL0;case 287:  {
# 2139 "parse.y"
struct _tuple28*_stmttmp26=Cyc_yyget_YY40(&(yyyvsp[1]).v);struct _tuple28*_Tmp2=_stmttmp26;void*_Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;void*_Tmp7;_Tmp7=_Tmp2->f1;_Tmp6=_Tmp2->f2;_Tmp5=_Tmp2->f3;_Tmp4=_Tmp2->f4;_Tmp3=_Tmp2->f5;{struct Cyc_List_List*lis=_Tmp7;int b=_Tmp6;struct Cyc_Absyn_VarargInfo*c=_Tmp5;void*eff=_Tmp4;struct Cyc_List_List*po=_Tmp3;
yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp8;({struct Cyc_List_List*_Tmp9=({struct Cyc_List_List*_TmpA=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_TmpB=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_TmpC=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_TmpC->tag=3,({void*_TmpD=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_TmpE=_region_malloc(yyr,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_TmpE->tag=1,_TmpE->f1=lis,_TmpE->f2=b,_TmpE->f3=c,_TmpE->f4=eff,_TmpE->f5=po,({struct Cyc_Absyn_Exp*_TmpF=(Cyc_yyget_YY59(&(yyyvsp[3]).v)).f1;_TmpE->f6=_TmpF;}),({struct Cyc_Absyn_Exp*_TmpF=(Cyc_yyget_YY59(&(yyyvsp[3]).v)).f2;_TmpE->f7=_TmpF;});_TmpE;});_TmpC->f1=_TmpD;});_TmpC;});_TmpA->hd=_TmpB;}),_TmpA->tl=0;_TmpA;});_Tmp8.tms=_Tmp9;});_Tmp8;}));
# 2142
goto _LL0;}}case 288:  {
# 2143 "parse.y"
struct _tuple28*_stmttmp27=Cyc_yyget_YY40(&(yyyvsp[2]).v);struct _tuple28*_Tmp2=_stmttmp27;void*_Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;void*_Tmp7;_Tmp7=_Tmp2->f1;_Tmp6=_Tmp2->f2;_Tmp5=_Tmp2->f3;_Tmp4=_Tmp2->f4;_Tmp3=_Tmp2->f5;{struct Cyc_List_List*lis=_Tmp7;int b=_Tmp6;struct Cyc_Absyn_VarargInfo*c=_Tmp5;void*eff=_Tmp4;struct Cyc_List_List*po=_Tmp3;
yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp8;({struct Cyc_List_List*_Tmp9=({struct Cyc_List_List*_TmpA=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_TmpB=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_TmpC=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_TmpC->tag=3,({void*_TmpD=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_TmpE=_region_malloc(yyr,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_TmpE->tag=1,_TmpE->f1=lis,_TmpE->f2=b,_TmpE->f3=c,_TmpE->f4=eff,_TmpE->f5=po,({
struct Cyc_Absyn_Exp*_TmpF=(Cyc_yyget_YY59(&(yyyvsp[4]).v)).f1;_TmpE->f6=_TmpF;}),({struct Cyc_Absyn_Exp*_TmpF=(Cyc_yyget_YY59(&(yyyvsp[4]).v)).f2;_TmpE->f7=_TmpF;});_TmpE;});
# 2144
_TmpC->f1=_TmpD;});_TmpC;});_TmpA->hd=_TmpB;}),({
struct Cyc_List_List*_TmpB=(Cyc_yyget_YY31(&(yyyvsp[0]).v)).tms;_TmpA->tl=_TmpB;});_TmpA;});
# 2144
_Tmp8.tms=_Tmp9;});_Tmp8;}));
# 2147
goto _LL0;}}case 289:  {
# 2149
struct Cyc_List_List*ts=({struct Cyc_List_List*(*_Tmp2)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;});unsigned _Tmp3=(unsigned)((yyyvsp[1]).l).first_line;_Tmp2(Cyc_Parse_typ2tvar,_Tmp3,({struct Cyc_List_List*(*_Tmp4)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_Tmp4(Cyc_yyget_YY41(&(yyyvsp[2]).v));}));});
yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp2;({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp5=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_Tmp6=_region_malloc(yyr,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_Tmp6->tag=4,_Tmp6->f1=ts,_Tmp6->f2=(unsigned)((yyyvsp[1]).l).first_line,_Tmp6->f3=0;_Tmp6;});_Tmp4->hd=_Tmp5;}),({
struct Cyc_List_List*_Tmp5=(Cyc_yyget_YY31(&(yyyvsp[0]).v)).tms;_Tmp4->tl=_Tmp5;});_Tmp4;});
# 2150
_Tmp2.tms=_Tmp3;});_Tmp2;}));
# 2153
goto _LL0;}case 290:
# 2154 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp2;({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp5=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp6=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp6->tag=5,_Tmp6->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_Tmp7=Cyc_yyget_YY46(&(yyyvsp[1]).v);_Tmp6->f2=_Tmp7;});_Tmp6;});_Tmp4->hd=_Tmp5;}),({struct Cyc_List_List*_Tmp5=(Cyc_yyget_YY31(&(yyyvsp[0]).v)).tms;_Tmp4->tl=_Tmp5;});_Tmp4;});_Tmp2.tms=_Tmp3;});_Tmp2;}));
# 2156
goto _LL0;case 291:
# 2159 "parse.y"
 yyval=Cyc_YY59(({struct _tuple17 _Tmp2;_Tmp2.f1=0,_Tmp2.f2=0;_Tmp2;}));
goto _LL0;case 292:
# 2160 "parse.y"
 yyval=Cyc_YY59(({struct _tuple17 _Tmp2;({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_Tmp2.f1=_Tmp3;}),_Tmp2.f2=0;_Tmp2;}));
goto _LL0;case 293:
# 2161 "parse.y"
 yyval=Cyc_YY59(({struct _tuple17 _Tmp2;_Tmp2.f1=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_Tmp2.f2=_Tmp3;});_Tmp2;}));
goto _LL0;case 294:
# 2163 "parse.y"
 yyval=Cyc_YY59(({struct _tuple17 _Tmp2;({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_Tmp2.f1=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[6]).v);_Tmp2.f2=_Tmp3;});_Tmp2;}));
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
 yyval=Cyc_Stmt_tok(({void*_Tmp2=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_Tmp3->tag=13,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;}),({struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);_Tmp3->f2=_Tmp4;});_Tmp3;});Cyc_Absyn_new_stmt(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 302:
# 2182 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 303:
# 2183 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_exp_stmt(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 304:
# 2188 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 305:
# 2189 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 306:
# 2194 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(&(yyyvsp[0]).v);Cyc_Parse_flatten_declarations(_Tmp2,Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));}));
goto _LL0;case 307:
# 2195 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(&(yyyvsp[0]).v);Cyc_Parse_flatten_declarations(_Tmp2,Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v));}));
goto _LL0;case 308:
# 2197 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_Tmp2=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_Tmp3->tag=13,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;}),({struct Cyc_Absyn_Stmt*_Tmp4=({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_Tmp5,Cyc_Absyn_skip_stmt(0U));});_Tmp3->f2=_Tmp4;});_Tmp3;});Cyc_Absyn_new_stmt(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 309:
# 2199 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_Tmp2=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_Tmp3->tag=13,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;}),({struct Cyc_Absyn_Stmt*_Tmp4=({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_Tmp5,Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v));});_Tmp3->f2=_Tmp4;});_Tmp3;});Cyc_Absyn_new_stmt(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 310:
# 2200 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 311:
# 2201 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_Tmp2=Cyc_yyget_Stmt_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);Cyc_Absyn_seq_stmt(_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 312:
# 2202 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_Tmp2=({void*_Tmp3=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_Tmp4->tag=1,({struct Cyc_Absyn_Fndecl*_Tmp5=Cyc_yyget_YY15(&(yyyvsp[0]).v);_Tmp4->f1=_Tmp5;});_Tmp4;});Cyc_Absyn_new_decl(_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);});Cyc_Parse_flatten_decl(_Tmp2,
Cyc_Absyn_skip_stmt(0U));}));
goto _LL0;case 313:
# 2205 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_Tmp2=({void*_Tmp3=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_Tmp4->tag=1,({struct Cyc_Absyn_Fndecl*_Tmp5=Cyc_yyget_YY15(&(yyyvsp[0]).v);_Tmp4->f1=_Tmp5;});_Tmp4;});Cyc_Absyn_new_decl(_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);});Cyc_Parse_flatten_decl(_Tmp2,Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v));}));
goto _LL0;case 314:
# 2210 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_Absyn_skip_stmt(0U);Cyc_Absyn_ifthenelse_stmt(_Tmp2,_Tmp3,_Tmp4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 315:
# 2212 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);Cyc_Absyn_ifthenelse_stmt(_Tmp2,_Tmp3,_Tmp4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 316:
# 2218 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_List_List*_Tmp3=Cyc_yyget_YY8(&(yyyvsp[5]).v);Cyc_Absyn_switch_stmt(_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 317:
# 2221
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp2=({struct _tuple0*_Tmp3=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);Cyc_Absyn_unknownid_exp(_Tmp3,(unsigned)((yyyvsp[1]).l).first_line);});struct Cyc_List_List*_Tmp3=Cyc_yyget_YY8(&(yyyvsp[3]).v);Cyc_Absyn_switch_stmt(_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 318:
# 2224
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY4(&(yyyvsp[3]).v);Cyc_Absyn_tuple_exp(_Tmp3,(unsigned)((yyyvsp[1]).l).first_line);});struct Cyc_List_List*_Tmp3=Cyc_yyget_YY8(&(yyyvsp[6]).v);Cyc_Absyn_switch_stmt(_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 319:
# 2228 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_Tmp2=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_Tmp3=Cyc_yyget_YY8(&(yyyvsp[4]).v);Cyc_Absyn_trycatch_stmt(_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 320:
# 2242 "parse.y"
 yyval=Cyc_YY8(0);
goto _LL0;case 321:
# 2245 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp3=({struct Cyc_Absyn_Switch_clause*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp5=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[0]).l).first_line);_Tmp4->pattern=_Tmp5;}),_Tmp4->pat_vars=0,_Tmp4->where_clause=0,({
struct Cyc_Absyn_Stmt*_Tmp5=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);_Tmp4->body=_Tmp5;}),_Tmp4->loc=(unsigned)((yyyvsp[0]).l).first_line;_Tmp4;});
# 2245
_Tmp2->hd=_Tmp3;}),({
struct Cyc_List_List*_Tmp3=Cyc_yyget_YY8(&(yyyvsp[3]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 322:
# 2248 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp3=({struct Cyc_Absyn_Switch_clause*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp5=Cyc_yyget_YY9(&(yyyvsp[1]).v);_Tmp4->pattern=_Tmp5;}),_Tmp4->pat_vars=0,_Tmp4->where_clause=0,({
struct Cyc_Absyn_Stmt*_Tmp5=Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[2]).l).first_line);_Tmp4->body=_Tmp5;}),_Tmp4->loc=(unsigned)((yyyvsp[0]).l).first_line;_Tmp4;});
# 2248
_Tmp2->hd=_Tmp3;}),({
# 2250
struct Cyc_List_List*_Tmp3=Cyc_yyget_YY8(&(yyyvsp[3]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 323:
# 2252 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp3=({struct Cyc_Absyn_Switch_clause*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp5=Cyc_yyget_YY9(&(yyyvsp[1]).v);_Tmp4->pattern=_Tmp5;}),_Tmp4->pat_vars=0,_Tmp4->where_clause=0,({struct Cyc_Absyn_Stmt*_Tmp5=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);_Tmp4->body=_Tmp5;}),_Tmp4->loc=(unsigned)((yyyvsp[0]).l).first_line;_Tmp4;});_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY8(&(yyyvsp[4]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 324:
# 2254 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp3=({struct Cyc_Absyn_Switch_clause*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp5=Cyc_yyget_YY9(&(yyyvsp[1]).v);_Tmp4->pattern=_Tmp5;}),_Tmp4->pat_vars=0,({struct Cyc_Absyn_Exp*_Tmp5=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_Tmp4->where_clause=_Tmp5;}),({
struct Cyc_Absyn_Stmt*_Tmp5=Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[4]).l).first_line);_Tmp4->body=_Tmp5;}),_Tmp4->loc=(unsigned)((yyyvsp[0]).l).first_line;_Tmp4;});
# 2254
_Tmp2->hd=_Tmp3;}),({
# 2256
struct Cyc_List_List*_Tmp3=Cyc_yyget_YY8(&(yyyvsp[5]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 325:
# 2258 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp3=({struct Cyc_Absyn_Switch_clause*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp5=Cyc_yyget_YY9(&(yyyvsp[1]).v);_Tmp4->pattern=_Tmp5;}),_Tmp4->pat_vars=0,({struct Cyc_Absyn_Exp*_Tmp5=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_Tmp4->where_clause=_Tmp5;}),({struct Cyc_Absyn_Stmt*_Tmp5=Cyc_yyget_Stmt_tok(&(yyyvsp[5]).v);_Tmp4->body=_Tmp5;}),_Tmp4->loc=(unsigned)((yyyvsp[0]).l).first_line;_Tmp4;});_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY8(&(yyyvsp[6]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 326:
# 2265 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);Cyc_Absyn_while_stmt(_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 327:
# 2267 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_Tmp2=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_do_stmt(_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 328:
# 2269 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(&(yyyvsp[6]).v);struct Cyc_Absyn_Stmt*_Tmp5=Cyc_yyget_Stmt_tok(&(yyyvsp[8]).v);Cyc_Absyn_for_stmt(_Tmp2,_Tmp3,_Tmp4,_Tmp5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 329:  {
# 2271 "parse.y"
struct Cyc_Absyn_Stmt*s=({struct Cyc_Absyn_Exp*_Tmp2=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Stmt*_Tmp5=Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);Cyc_Absyn_for_stmt(_Tmp2,_Tmp3,_Tmp4,_Tmp5,(unsigned)((yyyvsp[0]).l).first_line);});
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_Tmp2,s);}));
goto _LL0;}case 330:
# 2275
 yyval=Cyc_Exp_tok(Cyc_Absyn_true_exp(0U));
goto _LL0;case 331:
# 2276 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 332:
# 2279
 yyval=Cyc_Stmt_tok(({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_Tmp3=_Tmp4;});_Tmp3;});Cyc_Absyn_goto_stmt(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
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
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_return_stmt(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 337:
# 2285 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 338:
# 2286 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 339:
# 2288 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_fallthru_stmt(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 340:
# 2297 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 341:
# 2300
 yyval=(yyyvsp[0]).v;
goto _LL0;case 342:
# 2302 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_conditional_exp(_Tmp2,_Tmp3,_Tmp4,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 343:
# 2305
 yyval=(yyyvsp[0]).v;
goto _LL0;case 344:
# 2307 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_or_exp(_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 345:
# 2310
 yyval=(yyyvsp[0]).v;
goto _LL0;case 346:
# 2312 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_and_exp(_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 347:
# 2315
 yyval=(yyyvsp[0]).v;
goto _LL0;case 348:
# 2317 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(14U,_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 349:
# 2320
 yyval=(yyyvsp[0]).v;
goto _LL0;case 350:
# 2322 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(15U,_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 351:
# 2325
 yyval=(yyyvsp[0]).v;
goto _LL0;case 352:
# 2327 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(13U,_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 353:
# 2330
 yyval=(yyyvsp[0]).v;
goto _LL0;case 354:
# 2332 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*(*_Tmp2)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY66(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_Tmp2(_Tmp3,_Tmp4,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 355:
# 2335
 yyval=(yyyvsp[0]).v;
goto _LL0;case 356:
# 2337 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*(*_Tmp2)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY66(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_Tmp2(_Tmp3,_Tmp4,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 357:
# 2340
 yyval=(yyyvsp[0]).v;
goto _LL0;case 358:
# 2342 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(16U,_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 359:
# 2344 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(17U,_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 360:
# 2347
 yyval=(yyyvsp[0]).v;
goto _LL0;case 361:
# 2349 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _Tmp2=Cyc_yyget_YY6(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(_Tmp2,_Tmp3,_Tmp4,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 362:
# 2352
 yyval=(yyyvsp[0]).v;
goto _LL0;case 363:
# 2354 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _Tmp2=Cyc_yyget_YY6(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(_Tmp2,_Tmp3,_Tmp4,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 364:
# 2357
 yyval=(yyyvsp[0]).v;
goto _LL0;case 365:  {
# 2359 "parse.y"
void*t=({struct _tuple8*_Tmp2=Cyc_yyget_YY38(&(yyyvsp[1]).v);Cyc_Parse_type_name_to_type(_Tmp2,(unsigned)((yyyvsp[1]).l).first_line);});
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_Tmp2=t;struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_cast_exp(_Tmp2,_Tmp3,1,0U,(unsigned)((yyyvsp[0]).l).first_line);})));
# 2362
goto _LL0;}case 366:
# 2365 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 367:
# 2369 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _Tmp2=Cyc_yyget_YY6(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 368:  {
# 2371 "parse.y"
void*t=({struct _tuple8*_Tmp2=Cyc_yyget_YY38(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_Tmp2,(unsigned)((yyyvsp[2]).l).first_line);});
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftype_exp(t,(unsigned)((yyyvsp[0]).l).first_line)));
goto _LL0;}case 369:
# 2374 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_sizeofexp_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 370:
# 2376 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_Tmp2=(*Cyc_yyget_YY38(&(yyyvsp[2]).v)).f3;struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*(*_Tmp4)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_Tmp4(Cyc_yyget_YY3(&(yyyvsp[4]).v));});Cyc_Absyn_offsetof_exp(_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);})));
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
 yyval=Cyc_YY9(({void*_Tmp2=(void*)({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct));_Tmp3->tag=15,({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_pat(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 376:
# 2396 "parse.y"
 yyval=Cyc_YY9(({void*_Tmp2=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_Tmp3->tag=6,({struct Cyc_Absyn_Pat*_Tmp4=Cyc_yyget_YY9(&(yyyvsp[1]).v);_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_pat(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 377:  {
# 2398 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);
{void*_stmttmp28=e->r;void*_Tmp2=_stmttmp28;struct _fat_ptr _Tmp3;int _Tmp4;short _Tmp5;char _Tmp6;enum Cyc_Absyn_Sign _Tmp7;if(*((int*)_Tmp2)== 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp2)->f1).LongLong_c).tag){case 2: _Tmp7=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp2)->f1).Char_c).val).f1;_Tmp6=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp2)->f1).Char_c).val).f2;{enum Cyc_Absyn_Sign s=_Tmp7;char i=_Tmp6;
# 2402
yyval=Cyc_YY9(({void*_Tmp8=(void*)({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct));_Tmp9->tag=11,_Tmp9->f1=i;_Tmp9;});Cyc_Absyn_new_pat(_Tmp8,e->loc);}));goto _LL47E;}case 4: _Tmp7=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp2)->f1).Short_c).val).f1;_Tmp5=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp2)->f1).Short_c).val).f2;{enum Cyc_Absyn_Sign s=_Tmp7;short i=_Tmp5;
# 2404
yyval=Cyc_YY9(({void*_Tmp8=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct));_Tmp9->tag=10,_Tmp9->f1=s,_Tmp9->f2=(int)i;_Tmp9;});Cyc_Absyn_new_pat(_Tmp8,e->loc);}));goto _LL47E;}case 5: _Tmp7=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp2)->f1).Int_c).val).f1;_Tmp4=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp2)->f1).Int_c).val).f2;{enum Cyc_Absyn_Sign s=_Tmp7;int i=_Tmp4;
# 2406
yyval=Cyc_YY9(({void*_Tmp8=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct));_Tmp9->tag=10,_Tmp9->f1=s,_Tmp9->f2=i;_Tmp9;});Cyc_Absyn_new_pat(_Tmp8,e->loc);}));goto _LL47E;}case 7: _Tmp3=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp2)->f1).Float_c).val).f1;_Tmp4=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp2)->f1).Float_c).val).f2;{struct _fat_ptr s=_Tmp3;int i=_Tmp4;
# 2408
yyval=Cyc_YY9(({void*_Tmp8=(void*)({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct));_Tmp9->tag=12,_Tmp9->f1=s,_Tmp9->f2=i;_Tmp9;});Cyc_Absyn_new_pat(_Tmp8,e->loc);}));goto _LL47E;}case 1:
# 2410
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL47E;case 8:
 goto _LL48C;case 9: _LL48C:
# 2413
 Cyc_Warn_err((unsigned)((yyyvsp[0]).l).first_line,({const char*_Tmp8="strings cannot occur within patterns";_tag_fat(_Tmp8,sizeof(char),37U);}),_tag_fat(0U,sizeof(void*),0));goto _LL47E;case 6:
# 2415
 Cyc_Warn_err((unsigned)((yyyvsp[0]).l).first_line,({const char*_Tmp8="long long's in patterns not yet implemented";_tag_fat(_Tmp8,sizeof(char),44U);}),_tag_fat(0U,sizeof(void*),0));goto _LL47E;default: goto _LL48F;}else{_LL48F:
# 2417
 Cyc_Warn_err((unsigned)((yyyvsp[0]).l).first_line,({const char*_Tmp8="bad constant in case";_tag_fat(_Tmp8,sizeof(char),21U);}),_tag_fat(0U,sizeof(void*),0));}_LL47E:;}
# 2420
goto _LL0;}case 378:
# 2421 "parse.y"
 if(({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_Tmp2,({const char*_Tmp3="as";_tag_fat(_Tmp3,sizeof(char),3U);}));})!= 0)
Cyc_Warn_err((unsigned)((yyyvsp[1]).l).first_line,({const char*_Tmp2="expecting `as'";_tag_fat(_Tmp2,sizeof(char),15U);}),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY9(({void*_Tmp2=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct));_Tmp3->tag=1,({struct Cyc_Absyn_Vardecl*_Tmp4=({unsigned _Tmp5=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_Tmp6=({struct _tuple0*_Tmp7=_cycalloc(sizeof(struct _tuple0));_Tmp7->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp8=({struct _fat_ptr*_Tmp9=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _TmpA=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_Tmp9=_TmpA;});_Tmp9;});_Tmp7->f2=_Tmp8;});_Tmp7;});Cyc_Absyn_new_vardecl(_Tmp5,_Tmp6,Cyc_Absyn_void_type,0);});_Tmp3->f1=_Tmp4;}),({
struct Cyc_Absyn_Pat*_Tmp4=Cyc_yyget_YY9(&(yyyvsp[2]).v);_Tmp3->f2=_Tmp4;});_Tmp3;});
# 2423
Cyc_Absyn_new_pat(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2426
goto _LL0;case 379:
# 2427 "parse.y"
 if(({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_strcmp(_Tmp2,({const char*_Tmp3="alias";_tag_fat(_Tmp3,sizeof(char),6U);}));})!= 0)
Cyc_Warn_err((unsigned)((yyyvsp[1]).l).first_line,({const char*_Tmp2="expecting `alias'";_tag_fat(_Tmp2,sizeof(char),18U);}),_tag_fat(0U,sizeof(void*),0));{
int location=((yyyvsp[0]).l).first_line;
({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_tvar_ok(_Tmp2,(unsigned)location);});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->name=_Tmp3;}),_Tmp2->identity=-1,({void*_Tmp3=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_Tmp4->tag=0,_Tmp4->f1=& Cyc_Kinds_rk;_Tmp4;});_Tmp2->kind=_Tmp3;});_Tmp2;});
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp2=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_Tmp3=({struct _tuple0*_Tmp4=_cycalloc(sizeof(struct _tuple0));_Tmp4->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp5=({struct _fat_ptr*_Tmp6=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp7=Cyc_yyget_String_tok(&(yyyvsp[5]).v);*_Tmp6=_Tmp7;});_Tmp6;});_Tmp4->f2=_Tmp5;});_Tmp4;});Cyc_Absyn_new_vardecl(_Tmp2,_Tmp3,({
struct _tuple8*_Tmp4=Cyc_yyget_YY38(&(yyyvsp[4]).v);Cyc_Parse_type_name_to_type(_Tmp4,(unsigned)((yyyvsp[4]).l).first_line);}),0);});
yyval=Cyc_YY9(({void*_Tmp2=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct));_Tmp3->tag=2,_Tmp3->f1=tv,_Tmp3->f2=vd;_Tmp3;});Cyc_Absyn_new_pat(_Tmp2,(unsigned)location);}));
# 2436
goto _LL0;}}case 380:
# 2437 "parse.y"
 if(({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_strcmp(_Tmp2,({const char*_Tmp3="alias";_tag_fat(_Tmp3,sizeof(char),6U);}));})!= 0)
Cyc_Warn_err((unsigned)((yyyvsp[1]).l).first_line,({const char*_Tmp2="expecting `alias'";_tag_fat(_Tmp2,sizeof(char),18U);}),_tag_fat(0U,sizeof(void*),0));{
int location=((yyyvsp[0]).l).first_line;
({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_tvar_ok(_Tmp2,(unsigned)location);});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->name=_Tmp3;}),_Tmp2->identity=-1,({void*_Tmp3=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_Tmp4->tag=0,_Tmp4->f1=& Cyc_Kinds_rk;_Tmp4;});_Tmp2->kind=_Tmp3;});_Tmp2;});
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp2=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_Tmp3=({struct _tuple0*_Tmp4=_cycalloc(sizeof(struct _tuple0));_Tmp4->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp5=({struct _fat_ptr*_Tmp6=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp7=Cyc_yyget_String_tok(&(yyyvsp[5]).v);*_Tmp6=_Tmp7;});_Tmp6;});_Tmp4->f2=_Tmp5;});_Tmp4;});Cyc_Absyn_new_vardecl(_Tmp2,_Tmp3,({
struct _tuple8*_Tmp4=Cyc_yyget_YY38(&(yyyvsp[4]).v);Cyc_Parse_type_name_to_type(_Tmp4,(unsigned)((yyyvsp[4]).l).first_line);}),0);});
yyval=Cyc_YY9(({void*_Tmp2=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct));_Tmp3->tag=2,_Tmp3->f1=tv,_Tmp3->f2=vd;_Tmp3;});Cyc_Absyn_new_pat(_Tmp2,(unsigned)location);}));
# 2446
goto _LL0;}}case 381:  {
# 2447 "parse.y"
struct _tuple23 _stmttmp29=*Cyc_yyget_YY10(&(yyyvsp[2]).v);struct _tuple23 _Tmp2=_stmttmp29;int _Tmp3;void*_Tmp4;_Tmp4=_Tmp2.f1;_Tmp3=_Tmp2.f2;{struct Cyc_List_List*ps=_Tmp4;int dots=_Tmp3;
yyval=Cyc_YY9(({void*_Tmp5=(void*)({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct));_Tmp6->tag=5,_Tmp6->f1=ps,_Tmp6->f2=dots;_Tmp6;});Cyc_Absyn_new_pat(_Tmp5,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2450
goto _LL0;}}case 382:  {
# 2451 "parse.y"
struct _tuple23 _stmttmp2A=*Cyc_yyget_YY10(&(yyyvsp[2]).v);struct _tuple23 _Tmp2=_stmttmp2A;int _Tmp3;void*_Tmp4;_Tmp4=_Tmp2.f1;_Tmp3=_Tmp2.f2;{struct Cyc_List_List*ps=_Tmp4;int dots=_Tmp3;
yyval=Cyc_YY9(({void*_Tmp5=(void*)({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct));_Tmp6->tag=16,({struct _tuple0*_Tmp7=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_Tmp6->f1=_Tmp7;}),_Tmp6->f2=ps,_Tmp6->f3=dots;_Tmp6;});Cyc_Absyn_new_pat(_Tmp5,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2454
goto _LL0;}}case 383:  {
# 2455 "parse.y"
struct _tuple23 _stmttmp2B=*Cyc_yyget_YY14(&(yyyvsp[3]).v);struct _tuple23 _Tmp2=_stmttmp2B;int _Tmp3;void*_Tmp4;_Tmp4=_Tmp2.f1;_Tmp3=_Tmp2.f2;{struct Cyc_List_List*fps=_Tmp4;int dots=_Tmp3;
struct Cyc_List_List*exist_ts=({struct Cyc_List_List*(*_Tmp5)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;});unsigned _Tmp6=(unsigned)((yyyvsp[2]).l).first_line;_Tmp5(Cyc_Parse_typ2tvar,_Tmp6,Cyc_yyget_YY41(&(yyyvsp[2]).v));});
yyval=Cyc_YY9(({void*_Tmp5=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_Tmp6->tag=7,({union Cyc_Absyn_AggrInfo*_Tmp7=({union Cyc_Absyn_AggrInfo*_Tmp8=_cycalloc(sizeof(union Cyc_Absyn_AggrInfo));({union Cyc_Absyn_AggrInfo _Tmp9=Cyc_Absyn_UnknownAggr(0U,Cyc_yyget_QualId_tok(&(yyyvsp[0]).v),0);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f1=_Tmp7;}),_Tmp6->f2=exist_ts,_Tmp6->f3=fps,_Tmp6->f4=dots;_Tmp6;});Cyc_Absyn_new_pat(_Tmp5,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2460
goto _LL0;}}case 384:  {
# 2461 "parse.y"
struct _tuple23 _stmttmp2C=*Cyc_yyget_YY14(&(yyyvsp[2]).v);struct _tuple23 _Tmp2=_stmttmp2C;int _Tmp3;void*_Tmp4;_Tmp4=_Tmp2.f1;_Tmp3=_Tmp2.f2;{struct Cyc_List_List*fps=_Tmp4;int dots=_Tmp3;
struct Cyc_List_List*exist_ts=({struct Cyc_List_List*(*_Tmp5)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;});unsigned _Tmp6=(unsigned)((yyyvsp[1]).l).first_line;_Tmp5(Cyc_Parse_typ2tvar,_Tmp6,Cyc_yyget_YY41(&(yyyvsp[1]).v));});
yyval=Cyc_YY9(({void*_Tmp5=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_Tmp6->tag=7,_Tmp6->f1=0,_Tmp6->f2=exist_ts,_Tmp6->f3=fps,_Tmp6->f4=dots;_Tmp6;});Cyc_Absyn_new_pat(_Tmp5,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2465
goto _LL0;}}case 385:
# 2466 "parse.y"
 yyval=Cyc_YY9(({void*_Tmp2=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_Tmp3->tag=6,({struct Cyc_Absyn_Pat*_Tmp4=({void*_Tmp5=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_Tmp6->tag=6,({struct Cyc_Absyn_Pat*_Tmp7=Cyc_yyget_YY9(&(yyyvsp[1]).v);_Tmp6->f1=_Tmp7;});_Tmp6;});Cyc_Absyn_new_pat(_Tmp5,(unsigned)((yyyvsp[0]).l).first_line);});_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_pat(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 386:
# 2468 "parse.y"
 yyval=Cyc_YY9(({void*_Tmp2=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct));_Tmp3->tag=3,({struct Cyc_Absyn_Vardecl*_Tmp4=({unsigned _Tmp5=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_Tmp6=({struct _tuple0*_Tmp7=_cycalloc(sizeof(struct _tuple0));_Tmp7->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp8=({struct _fat_ptr*_Tmp9=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _TmpA=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_Tmp9=_TmpA;});_Tmp9;});_Tmp7->f2=_Tmp8;});_Tmp7;});Cyc_Absyn_new_vardecl(_Tmp5,_Tmp6,Cyc_Absyn_void_type,0);});_Tmp3->f1=_Tmp4;}),({
# 2470
struct Cyc_Absyn_Pat*_Tmp4=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[1]).l).first_line);_Tmp3->f2=_Tmp4;});_Tmp3;});
# 2468
Cyc_Absyn_new_pat(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2472
goto _LL0;case 387:
# 2473 "parse.y"
 if(({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_strcmp(_Tmp2,({const char*_Tmp3="as";_tag_fat(_Tmp3,sizeof(char),3U);}));})!= 0)
Cyc_Warn_err((unsigned)((yyyvsp[2]).l).first_line,({const char*_Tmp2="expecting `as'";_tag_fat(_Tmp2,sizeof(char),15U);}),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY9(({void*_Tmp2=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct));_Tmp3->tag=3,({struct Cyc_Absyn_Vardecl*_Tmp4=({unsigned _Tmp5=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_Tmp6=({struct _tuple0*_Tmp7=_cycalloc(sizeof(struct _tuple0));_Tmp7->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp8=({struct _fat_ptr*_Tmp9=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _TmpA=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_Tmp9=_TmpA;});_Tmp9;});_Tmp7->f2=_Tmp8;});_Tmp7;});Cyc_Absyn_new_vardecl(_Tmp5,_Tmp6,Cyc_Absyn_void_type,0);});_Tmp3->f1=_Tmp4;}),({
# 2477
struct Cyc_Absyn_Pat*_Tmp4=Cyc_yyget_YY9(&(yyyvsp[3]).v);_Tmp3->f2=_Tmp4;});_Tmp3;});
# 2475
Cyc_Absyn_new_pat(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2479
goto _LL0;case 388:  {
# 2480 "parse.y"
void*tag=({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_Tmp2,Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik));});
yyval=Cyc_YY9(({void*_Tmp2=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct));_Tmp3->tag=4,({struct Cyc_Absyn_Tvar*_Tmp4=Cyc_Parse_typ2tvar((unsigned)((yyyvsp[2]).l).first_line,tag);_Tmp3->f1=_Tmp4;}),({
struct Cyc_Absyn_Vardecl*_Tmp4=({unsigned _Tmp5=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_Tmp6=({struct _tuple0*_Tmp7=_cycalloc(sizeof(struct _tuple0));_Tmp7->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp8=({struct _fat_ptr*_Tmp9=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _TmpA=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_Tmp9=_TmpA;});_Tmp9;});_Tmp7->f2=_Tmp8;});_Tmp7;});Cyc_Absyn_new_vardecl(_Tmp5,_Tmp6,
Cyc_Absyn_tag_type(tag),0);});
# 2482
_Tmp3->f2=_Tmp4;});_Tmp3;});
# 2481
Cyc_Absyn_new_pat(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2485
goto _LL0;}case 389:  {
# 2486 "parse.y"
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar(Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik));
yyval=Cyc_YY9(({void*_Tmp2=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct));_Tmp3->tag=4,_Tmp3->f1=tv,({
struct Cyc_Absyn_Vardecl*_Tmp4=({unsigned _Tmp5=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_Tmp6=({struct _tuple0*_Tmp7=_cycalloc(sizeof(struct _tuple0));_Tmp7->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp8=({struct _fat_ptr*_Tmp9=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _TmpA=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_Tmp9=_TmpA;});_Tmp9;});_Tmp7->f2=_Tmp8;});_Tmp7;});Cyc_Absyn_new_vardecl(_Tmp5,_Tmp6,
Cyc_Absyn_tag_type(Cyc_Absyn_var_type(tv)),0);});
# 2488
_Tmp3->f2=_Tmp4;});_Tmp3;});
# 2487
Cyc_Absyn_new_pat(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2491
goto _LL0;}case 390:
# 2494 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_Tmp2=_cycalloc(sizeof(struct _tuple23));({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*(*_Tmp4)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev;});_Tmp4(Cyc_yyget_YY11(&(yyyvsp[0]).v));});_Tmp2->f1=_Tmp3;}),_Tmp2->f2=0;_Tmp2;}));
goto _LL0;case 391:
# 2495 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_Tmp2=_cycalloc(sizeof(struct _tuple23));({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*(*_Tmp4)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev;});_Tmp4(Cyc_yyget_YY11(&(yyyvsp[0]).v));});_Tmp2->f1=_Tmp3;}),_Tmp2->f2=1;_Tmp2;}));
goto _LL0;case 392:
# 2496 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_Tmp2=_cycalloc(sizeof(struct _tuple23));_Tmp2->f1=0,_Tmp2->f2=1;_Tmp2;}));
goto _LL0;case 393:
# 2499
 yyval=Cyc_YY11(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Pat*_Tmp3=Cyc_yyget_YY9(&(yyyvsp[0]).v);_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 394:
# 2500 "parse.y"
 yyval=Cyc_YY11(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Pat*_Tmp3=Cyc_yyget_YY9(&(yyyvsp[2]).v);_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY11(&(yyyvsp[0]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 395:
# 2503
 yyval=Cyc_YY12(({struct _tuple24*_Tmp2=_cycalloc(sizeof(struct _tuple24));_Tmp2->f1=0,({struct Cyc_Absyn_Pat*_Tmp3=Cyc_yyget_YY9(&(yyyvsp[0]).v);_Tmp2->f2=_Tmp3;});_Tmp2;}));
goto _LL0;case 396:
# 2504 "parse.y"
 yyval=Cyc_YY12(({struct _tuple24*_Tmp2=_cycalloc(sizeof(struct _tuple24));({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY42(&(yyyvsp[0]).v);_Tmp2->f1=_Tmp3;}),({struct Cyc_Absyn_Pat*_Tmp3=Cyc_yyget_YY9(&(yyyvsp[1]).v);_Tmp2->f2=_Tmp3;});_Tmp2;}));
goto _LL0;case 397:
# 2507
 yyval=Cyc_YY14(({struct _tuple23*_Tmp2=_cycalloc(sizeof(struct _tuple23));({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*(*_Tmp4)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev;});_Tmp4(Cyc_yyget_YY13(&(yyyvsp[0]).v));});_Tmp2->f1=_Tmp3;}),_Tmp2->f2=0;_Tmp2;}));
goto _LL0;case 398:
# 2508 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_Tmp2=_cycalloc(sizeof(struct _tuple23));({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*(*_Tmp4)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev;});_Tmp4(Cyc_yyget_YY13(&(yyyvsp[0]).v));});_Tmp2->f1=_Tmp3;}),_Tmp2->f2=1;_Tmp2;}));
goto _LL0;case 399:
# 2509 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_Tmp2=_cycalloc(sizeof(struct _tuple23));_Tmp2->f1=0,_Tmp2->f2=1;_Tmp2;}));
goto _LL0;case 400:
# 2512
 yyval=Cyc_YY13(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple24*_Tmp3=Cyc_yyget_YY12(&(yyyvsp[0]).v);_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 401:
# 2513 "parse.y"
 yyval=Cyc_YY13(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple24*_Tmp3=Cyc_yyget_YY12(&(yyyvsp[2]).v);_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY13(&(yyyvsp[0]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 402:
# 2518 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 403:
# 2519 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_seq_exp(_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 404:
# 2523 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 405:
# 2525 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Core_Opt*_Tmp3=Cyc_yyget_YY7(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_assignop_exp(_Tmp2,_Tmp3,_Tmp4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 406:
# 2527 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_swap_exp(_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 407:
# 2531 "parse.y"
 yyval=Cyc_YY7(0);
goto _LL0;case 408:
# 2532 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp2=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp2->v=(void*)1U;_Tmp2;}));
goto _LL0;case 409:
# 2533 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp2=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp2->v=(void*)3U;_Tmp2;}));
goto _LL0;case 410:
# 2534 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp2=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp2->v=(void*)4U;_Tmp2;}));
goto _LL0;case 411:
# 2535 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp2=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp2->v=(void*)0U;_Tmp2;}));
goto _LL0;case 412:
# 2536 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp2=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp2->v=(void*)2U;_Tmp2;}));
goto _LL0;case 413:
# 2537 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp2=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp2->v=(void*)16U;_Tmp2;}));
goto _LL0;case 414:
# 2538 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp2=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp2->v=(void*)17U;_Tmp2;}));
goto _LL0;case 415:
# 2539 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp2=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp2->v=(void*)13U;_Tmp2;}));
goto _LL0;case 416:
# 2540 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp2=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp2->v=(void*)15U;_Tmp2;}));
goto _LL0;case 417:
# 2541 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp2=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp2->v=(void*)14U;_Tmp2;}));
goto _LL0;case 418:
# 2545 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 419:
# 2547 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_conditional_exp(_Tmp2,_Tmp3,_Tmp4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 420:
# 2549 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_throw_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 421:
# 2551 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_New_exp(0,_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 422:
# 2552 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_New_exp(0,_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 423:
# 2553 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_New_exp(_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 424:
# 2555 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_New_exp(_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 425:
# 2559 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 426:
# 2562
 yyval=(yyyvsp[0]).v;
goto _LL0;case 427:
# 2564 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_or_exp(_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 428:
# 2567
 yyval=(yyyvsp[0]).v;
goto _LL0;case 429:
# 2569 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_and_exp(_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 430:
# 2572
 yyval=(yyyvsp[0]).v;
goto _LL0;case 431:
# 2574 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(14U,_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 432:
# 2577
 yyval=(yyyvsp[0]).v;
goto _LL0;case 433:
# 2579 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(15U,_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 434:
# 2582
 yyval=(yyyvsp[0]).v;
goto _LL0;case 435:
# 2584 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(13U,_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 436:
# 2587
 yyval=(yyyvsp[0]).v;
goto _LL0;case 437:
# 2589 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*(*_Tmp2)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY66(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_Tmp2(_Tmp3,_Tmp4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 438:
# 2592
 yyval=(yyyvsp[0]).v;
goto _LL0;case 439:
# 2594 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*(*_Tmp2)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY66(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_Tmp2(_Tmp3,_Tmp4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 440:
# 2597
 yyval=(yyyvsp[0]).v;
goto _LL0;case 441:
# 2599 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(16U,_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 442:
# 2601 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(17U,_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 443:
# 2604
 yyval=(yyyvsp[0]).v;
goto _LL0;case 444:
# 2606 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _Tmp2=Cyc_yyget_YY6(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(_Tmp2,_Tmp3,_Tmp4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 445:
# 2609
 yyval=(yyyvsp[0]).v;
goto _LL0;case 446:
# 2611 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _Tmp2=Cyc_yyget_YY6(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(_Tmp2,_Tmp3,_Tmp4,(unsigned)((yyyvsp[0]).l).first_line);}));
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
void*t=({struct _tuple8*_Tmp2=Cyc_yyget_YY38(&(yyyvsp[1]).v);Cyc_Parse_type_name_to_type(_Tmp2,(unsigned)((yyyvsp[1]).l).first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp2=t;struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_cast_exp(_Tmp2,_Tmp3,1,0U,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 460:
# 2641 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 461:
# 2642 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_increment_exp(_Tmp2,0U,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 462:
# 2643 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_increment_exp(_Tmp2,2U,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 463:
# 2644 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_address_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 464:
# 2645 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_deref_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 465:
# 2646 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _Tmp2=Cyc_yyget_YY6(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 466:  {
# 2648 "parse.y"
void*t=({struct _tuple8*_Tmp2=Cyc_yyget_YY38(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_Tmp2,(unsigned)((yyyvsp[2]).l).first_line);});
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeoftype_exp(t,(unsigned)((yyyvsp[0]).l).first_line));
# 2651
goto _LL0;}case 467:
# 2651 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_sizeofexp_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 468:  {
# 2653 "parse.y"
void*t=({struct _tuple8*_Tmp2=Cyc_yyget_YY38(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_Tmp2,(unsigned)((yyyvsp[2]).l).first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp2=t;struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*(*_Tmp4)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_Tmp4(Cyc_yyget_YY3(&(yyyvsp[4]).v));});Cyc_Absyn_offsetof_exp(_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2656
goto _LL0;}case 469:
# 2658
 yyval=Cyc_Exp_tok(({void*_Tmp2=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp3->tag=34,(_Tmp3->f1).is_calloc=0,(_Tmp3->f1).rgn=0,(_Tmp3->f1).elt_type=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_Tmp3->f1).num_elts=_Tmp4;}),(_Tmp3->f1).fat_result=0,(_Tmp3->f1).inline_call=0;_Tmp3;});Cyc_Absyn_new_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2660
goto _LL0;case 470:
# 2661 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp2=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp3->tag=34,(_Tmp3->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_Tmp3->f1).rgn=_Tmp4;}),(_Tmp3->f1).elt_type=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_Tmp3->f1).num_elts=_Tmp4;}),(_Tmp3->f1).fat_result=0,(_Tmp3->f1).inline_call=0;_Tmp3;});Cyc_Absyn_new_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2663
goto _LL0;case 471:
# 2664 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp2=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp3->tag=34,(_Tmp3->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_Tmp3->f1).rgn=_Tmp4;}),(_Tmp3->f1).elt_type=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_Tmp3->f1).num_elts=_Tmp4;}),(_Tmp3->f1).fat_result=0,(_Tmp3->f1).inline_call=1;_Tmp3;});Cyc_Absyn_new_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2666
goto _LL0;case 472:  {
# 2667 "parse.y"
void*t=({struct _tuple8*_Tmp2=Cyc_yyget_YY38(&(yyyvsp[6]).v);Cyc_Parse_type_name_to_type(_Tmp2,(unsigned)((yyyvsp[6]).l).first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp2=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp3->tag=34,(_Tmp3->f1).is_calloc=1,(_Tmp3->f1).rgn=0,({void**_Tmp4=({void**_Tmp5=_cycalloc(sizeof(void*));*_Tmp5=t;_Tmp5;});(_Tmp3->f1).elt_type=_Tmp4;}),({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_Tmp3->f1).num_elts=_Tmp4;}),(_Tmp3->f1).fat_result=0,(_Tmp3->f1).inline_call=0;_Tmp3;});Cyc_Absyn_new_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2670
goto _LL0;}case 473:  {
# 2672
void*t=({struct _tuple8*_Tmp2=Cyc_yyget_YY38(&(yyyvsp[8]).v);Cyc_Parse_type_name_to_type(_Tmp2,(unsigned)((yyyvsp[8]).l).first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp2=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp3->tag=34,(_Tmp3->f1).is_calloc=1,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_Tmp3->f1).rgn=_Tmp4;}),({void**_Tmp4=({void**_Tmp5=_cycalloc(sizeof(void*));*_Tmp5=t;_Tmp5;});(_Tmp3->f1).elt_type=_Tmp4;}),({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_Tmp3->f1).num_elts=_Tmp4;}),(_Tmp3->f1).fat_result=0,(_Tmp3->f1).inline_call=0;_Tmp3;});Cyc_Absyn_new_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2675
goto _LL0;}case 474:
# 2676 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_Tmp2=({struct Cyc_Absyn_Exp*_Tmp3[1];({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_Tmp3[0]=_Tmp4;});({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_Tmp3,sizeof(struct Cyc_Absyn_Exp*),1));});Cyc_Absyn_primop_exp(18U,_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 475:
# 2678 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp2=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct));_Tmp3->tag=38,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_Tmp3->f1=_Tmp4;}),({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f2=_Tmp4;});_Tmp3;});Cyc_Absyn_new_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 476:
# 2680 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp2=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct));_Tmp3->tag=38,({struct Cyc_Absyn_Exp*_Tmp4=({struct Cyc_Absyn_Exp*_Tmp5=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_deref_exp(_Tmp5,(unsigned)((yyyvsp[2]).l).first_line);});_Tmp3->f1=_Tmp4;}),({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f2=_Tmp4;});_Tmp3;});Cyc_Absyn_new_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 477:  {
# 2682 "parse.y"
void*t=({struct _tuple8*_Tmp2=Cyc_yyget_YY38(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_Tmp2,(unsigned)((yyyvsp[2]).l).first_line);});
yyval=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(t,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;}case 478:
# 2684 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp2=Cyc_yyget_YY60(&(yyyvsp[1]).v);Cyc_Absyn_new_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 479:
# 2685 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_extension_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 480:
# 2686 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_assert_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
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
struct _tuple30*_stmttmp2D=Cyc_yyget_YY61(&(yyyvsp[3]).v);struct _tuple30*_Tmp2=_stmttmp2D;void*_Tmp3;void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp2->f1;_Tmp4=_Tmp2->f2;_Tmp3=_Tmp2->f3;{struct Cyc_List_List*outlist=_Tmp5;struct Cyc_List_List*inlist=_Tmp4;struct Cyc_List_List*clobbers=_Tmp3;
yyval=Cyc_YY60((void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct));_Tmp6->tag=40,({int _Tmp7=Cyc_yyget_YY32(&(yyyvsp[0]).v);_Tmp6->f1=_Tmp7;}),({struct _fat_ptr _Tmp7=Cyc_yyget_String_tok(&(yyyvsp[2]).v);_Tmp6->f2=_Tmp7;}),_Tmp6->f3=outlist,_Tmp6->f4=inlist,_Tmp6->f5=clobbers;_Tmp6;}));
goto _LL0;}}case 486:
# 2703 "parse.y"
 yyval=Cyc_YY32(0);
goto _LL0;case 487:
# 2704 "parse.y"
 yyval=Cyc_YY32(1);
goto _LL0;case 488:
# 2708 "parse.y"
 yyval=Cyc_YY61(({struct _tuple30*_Tmp2=_cycalloc(sizeof(struct _tuple30));_Tmp2->f1=0,_Tmp2->f2=0,_Tmp2->f3=0;_Tmp2;}));
goto _LL0;case 489:  {
# 2710 "parse.y"
struct _tuple31*_stmttmp2E=Cyc_yyget_YY62(&(yyyvsp[1]).v);struct _tuple31*_Tmp2=_stmttmp2E;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2->f1;_Tmp3=_Tmp2->f2;{struct Cyc_List_List*inlist=_Tmp4;struct Cyc_List_List*clobbers=_Tmp3;
yyval=Cyc_YY61(({struct _tuple30*_Tmp5=_cycalloc(sizeof(struct _tuple30));_Tmp5->f1=0,_Tmp5->f2=inlist,_Tmp5->f3=clobbers;_Tmp5;}));
goto _LL0;}}case 490:  {
# 2713 "parse.y"
struct _tuple31*_stmttmp2F=Cyc_yyget_YY62(&(yyyvsp[2]).v);struct _tuple31*_Tmp2=_stmttmp2F;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2->f1;_Tmp3=_Tmp2->f2;{struct Cyc_List_List*inlist=_Tmp4;struct Cyc_List_List*clobbers=_Tmp3;
yyval=Cyc_YY61(({struct _tuple30*_Tmp5=_cycalloc(sizeof(struct _tuple30));({struct Cyc_List_List*_Tmp6=({struct Cyc_List_List*(*_Tmp7)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_Tmp7(Cyc_yyget_YY64(&(yyyvsp[1]).v));});_Tmp5->f1=_Tmp6;}),_Tmp5->f2=inlist,_Tmp5->f3=clobbers;_Tmp5;}));
goto _LL0;}}case 491:
# 2717
 yyval=Cyc_YY64(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple32*_Tmp3=Cyc_yyget_YY65(&(yyyvsp[0]).v);_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 492:
# 2718 "parse.y"
 yyval=Cyc_YY64(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple32*_Tmp3=Cyc_yyget_YY65(&(yyyvsp[2]).v);_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY64(&(yyyvsp[0]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 493:
# 2722 "parse.y"
 yyval=Cyc_YY62(({struct _tuple31*_Tmp2=_cycalloc(sizeof(struct _tuple31));_Tmp2->f1=0,_Tmp2->f2=0;_Tmp2;}));
goto _LL0;case 494:
# 2723 "parse.y"
 yyval=Cyc_YY62(({struct _tuple31*_Tmp2=_cycalloc(sizeof(struct _tuple31));_Tmp2->f1=0,({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY63(&(yyyvsp[1]).v);_Tmp2->f2=_Tmp3;});_Tmp2;}));
goto _LL0;case 495:
# 2724 "parse.y"
 yyval=Cyc_YY62(({struct _tuple31*_Tmp2=_cycalloc(sizeof(struct _tuple31));({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*(*_Tmp4)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_Tmp4(Cyc_yyget_YY64(&(yyyvsp[1]).v));});_Tmp2->f1=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY63(&(yyyvsp[2]).v);_Tmp2->f2=_Tmp3;});_Tmp2;}));
goto _LL0;case 496:
# 2727
 yyval=Cyc_YY64(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple32*_Tmp3=Cyc_yyget_YY65(&(yyyvsp[0]).v);_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 497:
# 2728 "parse.y"
 yyval=Cyc_YY64(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple32*_Tmp3=Cyc_yyget_YY65(&(yyyvsp[2]).v);_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY64(&(yyyvsp[0]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 498:
# 2732 "parse.y"
 yyval=Cyc_YY65(({struct _tuple32*_Tmp2=_cycalloc(sizeof(struct _tuple32));({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(&(yyyvsp[0]).v);_Tmp2->f1=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_Tmp2->f2=_Tmp3;});_Tmp2;}));
goto _LL0;case 499:
# 2737 "parse.y"
 yyval=Cyc_YY63(0);
goto _LL0;case 500:
# 2738 "parse.y"
 yyval=Cyc_YY63(0);
goto _LL0;case 501:
# 2739 "parse.y"
 yyval=Cyc_YY63(({struct Cyc_List_List*(*_Tmp2)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_Tmp2(Cyc_yyget_YY63(&(yyyvsp[1]).v));}));
goto _LL0;case 502:
# 2742
 yyval=Cyc_YY63(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 503:
# 2743 "parse.y"
 yyval=Cyc_YY63(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY63(&(yyyvsp[0]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 504:
# 2748 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 505:
# 2750 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_subscript_exp(_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 506:
# 2752 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_unknowncall_exp(_Tmp2,0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 507:
# 2754 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_List_List*_Tmp3=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_unknowncall_exp(_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 508:
# 2756 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_Tmp4=_Tmp5;});_Tmp4;});Cyc_Absyn_aggrmember_exp(_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 509:
# 2758 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_Tmp4=_Tmp5;});_Tmp4;});Cyc_Absyn_aggrarrow_exp(_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 510:
# 2760 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_increment_exp(_Tmp2,1U,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 511:
# 2762 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_increment_exp(_Tmp2,3U,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 512:
# 2764 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp2=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_Tmp3->tag=25,({struct _tuple8*_Tmp4=Cyc_yyget_YY38(&(yyyvsp[1]).v);_Tmp3->f1=_Tmp4;}),_Tmp3->f2=0;_Tmp3;});Cyc_Absyn_new_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 513:
# 2766 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp2=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_Tmp3->tag=25,({struct _tuple8*_Tmp4=Cyc_yyget_YY38(&(yyyvsp[1]).v);_Tmp3->f1=_Tmp4;}),({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*(*_Tmp5)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_Tmp5(Cyc_yyget_YY5(&(yyyvsp[4]).v));});_Tmp3->f2=_Tmp4;});_Tmp3;});Cyc_Absyn_new_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 514:
# 2768 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp2=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_Tmp3->tag=25,({struct _tuple8*_Tmp4=Cyc_yyget_YY38(&(yyyvsp[1]).v);_Tmp3->f1=_Tmp4;}),({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*(*_Tmp5)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_Tmp5(Cyc_yyget_YY5(&(yyyvsp[4]).v));});_Tmp3->f2=_Tmp4;});_Tmp3;});Cyc_Absyn_new_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 515:
# 2773 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp3=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_Tmp4->tag=0,({struct _fat_ptr*_Tmp5=({struct _fat_ptr*_Tmp6=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp7=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_Tmp6=_Tmp7;});_Tmp6;});_Tmp4->f1=_Tmp5;});_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 516:
# 2776
 yyval=Cyc_YY3(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp3=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct));_Tmp4->tag=1,({unsigned _Tmp5=({unsigned _Tmp6=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Parse_cnst2uint(_Tmp6,Cyc_yyget_Int_tok(&(yyyvsp[0]).v));});_Tmp4->f1=_Tmp5;});_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 517:
# 2778 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp3=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_Tmp4->tag=0,({struct _fat_ptr*_Tmp5=({struct _fat_ptr*_Tmp6=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp7=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_Tmp6=_Tmp7;});_Tmp6;});_Tmp4->f1=_Tmp5;});_Tmp4;});_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY3(&(yyyvsp[0]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 518:
# 2781
 yyval=Cyc_YY3(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp3=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct));_Tmp4->tag=1,({unsigned _Tmp5=({unsigned _Tmp6=(unsigned)((yyyvsp[2]).l).first_line;Cyc_Parse_cnst2uint(_Tmp6,Cyc_yyget_Int_tok(&(yyyvsp[2]).v));});_Tmp4->f1=_Tmp5;});_Tmp4;});_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY3(&(yyyvsp[0]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 519:
# 2786 "parse.y"
 yyval=Cyc_Exp_tok(({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);Cyc_Absyn_unknownid_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 520:
# 2787 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Absyn_pragma_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 521:
# 2788 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 522:
# 2789 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_string_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 523:
# 2790 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_wstring_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 524:
# 2791 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 525:
# 2795 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_noinstantiate_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 526:
# 2798
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*(*_Tmp4)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_Tmp4(Cyc_yyget_YY41(&(yyyvsp[3]).v));});Cyc_Absyn_instantiate_exp(_Tmp2,_Tmp3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 527:
# 2801
 yyval=Cyc_Exp_tok(({void*_Tmp2=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_Tmp3->tag=29,({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_Tmp3->f1=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY41(&(yyyvsp[2]).v);_Tmp3->f2=_Tmp4;}),({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*(*_Tmp5)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_Tmp5(Cyc_yyget_YY5(&(yyyvsp[3]).v));});_Tmp3->f3=_Tmp4;}),_Tmp3->f4=0;_Tmp3;});Cyc_Absyn_new_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 528:
# 2803 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_tuple_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 529:
# 2805 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Stmt*_Tmp2=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Absyn_stmt_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 530:
# 2809 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*(*_Tmp2)(struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;});_Tmp2(Cyc_yyget_YY4(&(yyyvsp[0]).v));}));
goto _LL0;case 531:
# 2814 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);_Tmp2->hd=_Tmp3;}),_Tmp2->tl=0;_Tmp2;}));
goto _LL0;case 532:
# 2816 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_Tmp2->hd=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY4(&(yyyvsp[0]).v);_Tmp2->tl=_Tmp3;});_Tmp2;}));
goto _LL0;case 533:
# 2822 "parse.y"
 yyval=Cyc_Exp_tok(({union Cyc_Absyn_Cnst _Tmp2=Cyc_yyget_Int_tok(&(yyyvsp[0]).v);Cyc_Absyn_const_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 534:
# 2823 "parse.y"
 yyval=Cyc_Exp_tok(({char _Tmp2=Cyc_yyget_Char_tok(&(yyyvsp[0]).v);Cyc_Absyn_char_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 535:
# 2824 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_wchar_exp(_Tmp2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 536:
# 2826 "parse.y"
 yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 537:  {
# 2828 "parse.y"
struct _fat_ptr f=Cyc_yyget_String_tok(&(yyyvsp[0]).v);
int l=(int)Cyc_strlen(f);
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
 yyval=Cyc_QualId_tok(({struct _tuple0*_Tmp2=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp3=Cyc_Absyn_Rel_n(0);_Tmp2->f1=_Tmp3;}),({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f2=_Tmp3;});_Tmp2;}));
goto _LL0;case 539:
# 2842 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 540:
# 2845
 yyval=Cyc_QualId_tok(({struct _tuple0*_Tmp2=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp3=Cyc_Absyn_Rel_n(0);_Tmp2->f1=_Tmp3;}),({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f2=_Tmp3;});_Tmp2;}));
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
yystate=({int _Tmp1=(int)*((short*)_check_known_subscript_notnull(Cyc_yypgoto,167U,sizeof(short),yyn - 153));_Tmp1 + (int)*((short*)_check_fat_subscript(yyss,sizeof(short),yyssp_offset));});
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
({unsigned long _Tmp1=Cyc_strlen(*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,320U,sizeof(struct _fat_ptr),x)))+ 15U;sze +=_Tmp1;}),count ++;}
msg=({unsigned _Tmp1=(unsigned)(sze + 15)+ 1U;_tag_fat(({char*_Tmp2=({struct _RegionHandle*_Tmp3=yyregion;_region_malloc(_Tmp3,_check_times(_Tmp1,sizeof(char)));});({{unsigned _Tmp3=(unsigned)(sze + 15);unsigned i;for(i=0;i < _Tmp3;++ i){_Tmp2[i]='\000';}_Tmp2[_Tmp3]=0;}0;});_Tmp2;}),sizeof(char),_Tmp1);});
Cyc_strcpy(msg,({const char*_Tmp1="parse error";_tag_fat(_Tmp1,sizeof(char),12U);}));
# 444
if(count < 5){
# 446
count=0;
for(x=yyn < 0?- yyn: 0;(unsigned)x < 320U / sizeof(char*);++ x){
# 449
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,6509U,sizeof(short),x + yyn))== x){
# 451
Cyc_strcat(msg,count == 0?({const char*_Tmp1=", expecting `";_tag_fat(_Tmp1,sizeof(char),14U);}):({const char*_Tmp1=" or `";_tag_fat(_Tmp1,sizeof(char),6U);}));
# 454
Cyc_strcat(msg,*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,320U,sizeof(struct _fat_ptr),x)));
Cyc_strcat(msg,({const char*_Tmp1="'";_tag_fat(_Tmp1,sizeof(char),2U);}));
++ count;}}}
# 459
Cyc_yyerror(msg,yystate,yychar);}else{
# 463
Cyc_yyerror(({const char*_Tmp1="parse error";_tag_fat(_Tmp1,sizeof(char),12U);}),yystate,yychar);}}
# 465
goto yyerrlab1;
# 467
yyerrlab1:
# 469
 if(yyerrstatus == 3){
# 474
if(yychar == 0){
int _Tmp1=1;_npop_handler(0);return _Tmp1;}
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
 if(yyssp_offset == 0){int _Tmp1=1;_npop_handler(0);return _Tmp1;}
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
int _Tmp1=0;_npop_handler(0);return _Tmp1;}
# 546
({struct Cyc_Yystacktype _Tmp1=({struct Cyc_Yystacktype _Tmp2;_Tmp2.v=yylval,_Tmp2.l=yylloc;_Tmp2;});((struct Cyc_Yystacktype*)yyvs.curr)[++ yyvsp_offset]=_Tmp1;});
# 551
goto yynewstate;}
# 149 "cycbison.simple"
;_pop_region();}
# 2864 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _Tmp0=v;void*_Tmp1;struct _fat_ptr _Tmp2;char _Tmp3;union Cyc_Absyn_Cnst _Tmp4;switch((_Tmp0.Stmt_tok).tag){case 1: _Tmp4=(_Tmp0.Int_tok).val;{union Cyc_Absyn_Cnst c=_Tmp4;
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=Cyc_Absynpp_cnst2string(c);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_fprintf(Cyc_stderr,({const char*_Tmp7="%s";_tag_fat(_Tmp7,sizeof(char),3U);}),_tag_fat(_Tmp6,sizeof(void*),1));});goto _LL0;}case 2: _Tmp3=(_Tmp0.Char_tok).val;{char c=_Tmp3;
({struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=(unsigned long)((int)c);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_fprintf(Cyc_stderr,({const char*_Tmp7="%c";_tag_fat(_Tmp7,sizeof(char),3U);}),_tag_fat(_Tmp6,sizeof(void*),1));});goto _LL0;}case 3: _Tmp2=(_Tmp0.String_tok).val;{struct _fat_ptr s=_Tmp2;
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=s;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_fprintf(Cyc_stderr,({const char*_Tmp7="\"%s\"";_tag_fat(_Tmp7,sizeof(char),5U);}),_tag_fat(_Tmp6,sizeof(void*),1));});goto _LL0;}case 4: _Tmp1=(_Tmp0.QualId_tok).val;{struct _tuple0*q=_Tmp1;
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=Cyc_Absynpp_qvar2string(q);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_fprintf(Cyc_stderr,({const char*_Tmp7="%s";_tag_fat(_Tmp7,sizeof(char),3U);}),_tag_fat(_Tmp6,sizeof(void*),1));});goto _LL0;}case 5: _Tmp1=(_Tmp0.Exp_tok).val;{struct Cyc_Absyn_Exp*e=_Tmp1;
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=Cyc_Absynpp_exp2string(e);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_fprintf(Cyc_stderr,({const char*_Tmp7="%s";_tag_fat(_Tmp7,sizeof(char),3U);}),_tag_fat(_Tmp6,sizeof(void*),1));});goto _LL0;}case 6: _Tmp1=(_Tmp0.Stmt_tok).val;{struct Cyc_Absyn_Stmt*s=_Tmp1;
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=Cyc_Absynpp_stmt2string(s);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_fprintf(Cyc_stderr,({const char*_Tmp7="%s";_tag_fat(_Tmp7,sizeof(char),3U);}),_tag_fat(_Tmp6,sizeof(void*),1));});goto _LL0;}default:
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="?";_tag_fat(_Tmp5,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));goto _LL0;}_LL0:;}
# 2876
struct _fat_ptr Cyc_token2string(int token){
if(token <= 0)
return({const char*_Tmp0="end-of-file";_tag_fat(_Tmp0,sizeof(char),12U);});
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
struct _RegionHandle _Tmp0=_new_region("yyr");struct _RegionHandle*yyr=& _Tmp0;_push_region(yyr);
({struct _RegionHandle*_Tmp1=yyr;Cyc_yyparse(_Tmp1,Cyc_Lexing_from_file(f));});{
struct Cyc_List_List*_Tmp1=Cyc_Parse_parse_result;_npop_handler(0);return _Tmp1;}
# 2893
;_pop_region();}}
