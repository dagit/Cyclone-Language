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
#if(defined(__linux__) && defined(__KERNEL__))
  struct _RegionPage *vpage;
#endif 
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

/* Alias qualifier stuff */
typedef unsigned char _AliasQualHandle_t;


struct _RegionHandle _new_region(const char*);
void* _region_malloc(struct _RegionHandle*, _AliasQualHandle_t, unsigned);
void* _region_calloc(struct _RegionHandle*, _AliasQualHandle_t, unsigned t, unsigned n);
void* _region_vmalloc(struct _RegionHandle*, unsigned);
void * _aqual_malloc(_AliasQualHandle_t aq, unsigned int s);
void * _aqual_calloc(_AliasQualHandle_t aq, unsigned int n, unsigned int t);
void _free_region(struct _RegionHandle*);
struct _RegionHandle*_open_dynregion(struct _DynRegionFrame*,struct _DynRegionHandle*);
void _pop_dynregion();

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

#define _CYC_MAX_REGION_CONST 2
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
struct _RegionHandle _profile_new_region(const char*,const char*,const char*,int);
void _profile_free_region(struct _RegionHandle*,const char*,const char*,int);
#ifndef RUNTIME_CYC
#define _new_region(n) _profile_new_region(n,__FILE__,__FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__,__FUNCTION__,__LINE__)
#define _region_malloc(rh,aq,n) _profile_region_malloc(rh,aq,n,__FILE__,__FUNCTION__,__LINE__)
#define _region_calloc(rh,aq,n,t) _profile_region_calloc(rh,aq,n,t,__FILE__,__FUNCTION__,__LINE__)
#define _aqual_malloc(aq,n) _profile_region_malloc(aq,n,__FILE__,__FUNCTION__,__LINE__)
#define _aqual_calloc(aq,n,t) _profile_region_calloc(aq,n,t,__FILE__,__FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc(n,s) _profile_GC_calloc(n,s,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc_atomic(n,s) _profile_GC_calloc_atomic(n,s,__FILE__,__FUNCTION__,__LINE__)
#endif
#endif

# 95 "core.h"
 struct _fat_ptr Cyc_Core_new_string(unsigned);
# 99
struct _fat_ptr Cyc_Core_rnew_string(struct _RegionHandle*,unsigned);
# 170
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc___cycFILE;
# 276 "cycboot.h"
extern int Cyc_file_string_read(struct Cyc___cycFILE*,struct _fat_ptr,int,int);struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _fat_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _fat_ptr,int,void*);void*read_fun_state;};
# 73 "string.h"
extern struct _fat_ptr Cyc_zstrncpy(struct _fat_ptr,struct _fat_ptr,unsigned long);
# 104 "string.h"
extern struct _fat_ptr Cyc_strdup(struct _fat_ptr);char Cyc_Lexing_Error[6U]="Error";
# 73 "lexing.cyc"
static char Cyc_Lexing_aux_buffer_v[1U]={'\000'};
static struct _fat_ptr Cyc_Lexing_aux_buffer={(void*)Cyc_Lexing_aux_buffer_v,(void*)Cyc_Lexing_aux_buffer_v,(void*)(Cyc_Lexing_aux_buffer_v + 1U)};
# 76
void Cyc_Lexing_lex_refill(struct Cyc_Lexing_lexbuf*lexbuf){
if(_get_fat_size(Cyc_Lexing_aux_buffer,sizeof(char))==1U)Cyc_Lexing_aux_buffer=Cyc_Core_new_string(4097U);{
int read=
((struct Cyc_Lexing_function_lexbuf_state*)lexbuf->refill_state)->read_fun(Cyc_Lexing_aux_buffer,(int)(
_get_fat_size(Cyc_Lexing_aux_buffer,sizeof(char))- 1U),((struct Cyc_Lexing_function_lexbuf_state*)lexbuf->refill_state)->read_fun_state);
# 82
int n=read > 0?read:(lexbuf->lex_eof_reached=1,0);
if(lexbuf->lex_start_pos < n){
int oldlen=lexbuf->lex_buffer_len;
int newlen=oldlen * 2;
# 87
struct _fat_ptr newbuf=Cyc_Core_new_string((unsigned)(newlen + 1));
({struct _fat_ptr _Tmp0=({struct _fat_ptr _Tmp1=_fat_ptr_decrease_size(newbuf,sizeof(char),1U);_fat_ptr_plus(_Tmp1,sizeof(char),oldlen);});struct _fat_ptr _Tmp1=(struct _fat_ptr)lexbuf->lex_buffer;Cyc_zstrncpy(_Tmp0,_Tmp1,(unsigned long)oldlen);});
lexbuf->lex_buffer=newbuf;
lexbuf->lex_buffer_len=newlen;
lexbuf->lex_abs_pos=lexbuf->lex_abs_pos - oldlen;
lexbuf->lex_curr_pos=lexbuf->lex_curr_pos + oldlen;
lexbuf->lex_start_pos=lexbuf->lex_start_pos + oldlen;
lexbuf->lex_last_pos=lexbuf->lex_last_pos + oldlen;}
# 96
({struct _fat_ptr _Tmp0=_fat_ptr_decrease_size(lexbuf->lex_buffer,sizeof(char),1U);struct _fat_ptr _Tmp1=(struct _fat_ptr)
_fat_ptr_plus(lexbuf->lex_buffer,sizeof(char),n);
# 96
Cyc_zstrncpy(_Tmp0,_Tmp1,(unsigned long)(lexbuf->lex_buffer_len - n));});
# 99
({struct _fat_ptr _Tmp0=_fat_ptr_decrease_size(_fat_ptr_plus(lexbuf->lex_buffer,sizeof(char),lexbuf->lex_buffer_len - n),sizeof(char),1U);struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_Lexing_aux_buffer;Cyc_zstrncpy(_Tmp0,_Tmp1,(unsigned long)n);});
# 101
lexbuf->lex_abs_pos=lexbuf->lex_abs_pos + n;
lexbuf->lex_curr_pos=lexbuf->lex_curr_pos - n;
lexbuf->lex_start_pos=lexbuf->lex_start_pos - n;
lexbuf->lex_last_pos=lexbuf->lex_last_pos - n;}}
# 107
struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_function(int(*read_fun)(struct _fat_ptr,int,void*),void*read_fun_state){
# 109
return({struct Cyc_Lexing_lexbuf*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_lexbuf));_Tmp0->refill_buff=Cyc_Lexing_lex_refill,({
struct Cyc_Lexing_function_lexbuf_state*_Tmp1=({struct Cyc_Lexing_function_lexbuf_state*_Tmp2=_cycalloc(sizeof(struct Cyc_Lexing_function_lexbuf_state));_Tmp2->read_fun=read_fun,_Tmp2->read_fun_state=read_fun_state;_Tmp2;});_Tmp0->refill_state=_Tmp1;}),({
struct _fat_ptr _Tmp1=Cyc_Core_new_string(8193U);_Tmp0->lex_buffer=_Tmp1;}),_Tmp0->lex_buffer_len=8192,_Tmp0->lex_abs_pos=-8192,_Tmp0->lex_start_pos=8192,_Tmp0->lex_curr_pos=8192,_Tmp0->lex_last_pos=8192,_Tmp0->lex_last_action=0,_Tmp0->lex_eof_reached=0;_Tmp0;});}
# 118
static int Cyc_Lexing_read_from_file(struct _fat_ptr aux,int n,struct Cyc___cycFILE*f){
return Cyc_file_string_read(f,aux,0,n);}
# 123
struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*f){
return({(struct Cyc_Lexing_lexbuf*(*)(int(*)(struct _fat_ptr,int,struct Cyc___cycFILE*),struct Cyc___cycFILE*))Cyc_Lexing_from_function;})(Cyc_Lexing_read_from_file,f);}
# 127
static void Cyc_Lexing_set_eof(struct Cyc_Lexing_lexbuf*lbuf){
lbuf->lex_eof_reached=1;}
# 131
struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_string(struct _fat_ptr s){
return({struct Cyc_Lexing_lexbuf*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_lexbuf));_Tmp0->refill_buff=Cyc_Lexing_set_eof,_Tmp0->refill_state=(void*)0,({
# 134
struct _fat_ptr _Tmp1=Cyc_strdup((struct _fat_ptr)s);_Tmp0->lex_buffer=_Tmp1;}),_Tmp0->lex_buffer_len=(int)
_get_fat_size(s,sizeof(char)),_Tmp0->lex_abs_pos=0,_Tmp0->lex_start_pos=0,_Tmp0->lex_curr_pos=0,_Tmp0->lex_last_pos=0,_Tmp0->lex_last_action=0,_Tmp0->lex_eof_reached=1;_Tmp0;});}
# 140
struct _fat_ptr Cyc_Lexing_rlexeme(struct _RegionHandle*r,struct Cyc_Lexing_lexbuf*lbuf){
int len=lbuf->lex_curr_pos - lbuf->lex_start_pos;
struct _fat_ptr s=Cyc_Core_rnew_string(r,(unsigned)(len + 1));
({struct _fat_ptr _Tmp0=_fat_ptr_decrease_size(s,sizeof(char),1U);struct _fat_ptr _Tmp1=(struct _fat_ptr)_fat_ptr_plus(lbuf->lex_buffer,sizeof(char),lbuf->lex_start_pos);Cyc_zstrncpy(_Tmp0,_Tmp1,(unsigned long)len);});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(s,sizeof(char),len);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='\000';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
return s;}
# 148
struct _fat_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*lbuf){
return Cyc_Lexing_rlexeme(Cyc_Core_heap_region,lbuf);}
# 151
char Cyc_Lexing_lexeme_char(struct Cyc_Lexing_lexbuf*lbuf,int i){
return*((char*)_check_fat_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_start_pos + i));}
# 154
int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*lbuf){
return lbuf->lex_abs_pos + lbuf->lex_start_pos;}
# 157
int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*lbuf){
return lbuf->lex_abs_pos + lbuf->lex_curr_pos;}
