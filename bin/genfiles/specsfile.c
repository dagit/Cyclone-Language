#include <setjmp.h>
/* This is a C header file to be used by the output of the Cyclone to
   C translator  The corresponding definitions are in file lib/runtime_*.c */
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

struct _RegionHandle _new_region(const char*);
void* _region_malloc(struct _RegionHandle*, unsigned);
void* _region_calloc(struct _RegionHandle*, unsigned t, unsigned n);
void* _region_vmalloc(struct _RegionHandle*, unsigned);
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
  _zero_arr_inplace_plus_char_fn((char**)(x),i,__FILE__,__LINE__)
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
      _curr != (unsigned char*)0) \
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
 struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 79
extern int Cyc_fclose(struct Cyc___cycFILE*);
# 88
extern int Cyc_fflush(struct Cyc___cycFILE*);
# 90
extern int Cyc_fgetc(struct Cyc___cycFILE*);
# 98
extern struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 271 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_file_open(struct _fat_ptr,struct _fat_ptr);
# 300
extern int isspace(int);
# 313
extern char*getenv(const char*);extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 178 "list.h"
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 336
extern void*Cyc_List_assoc_cmp(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 364
extern struct _fat_ptr Cyc_List_to_array(struct Cyc_List_List*);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
extern int Cyc_strncmp(struct _fat_ptr,struct _fat_ptr,unsigned long);
# 62
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);
# 104 "string.h"
extern struct _fat_ptr Cyc_strdup(struct _fat_ptr);
# 109
extern struct _fat_ptr Cyc_substring(struct _fat_ptr,int,unsigned long);
# 30 "filename.h"
extern struct _fat_ptr Cyc_Filename_concat(struct _fat_ptr,struct _fat_ptr);
# 69 "arg.h"
extern int Cyc_Arg_current;
# 71
extern void Cyc_Arg_parse(struct Cyc_List_List*,void(*)(struct _fat_ptr),int(*)(struct _fat_ptr),struct _fat_ptr,struct _fat_ptr);
# 29 "specsfile.h"
struct _fat_ptr Cyc_Specsfile_target_arch;
# 31
struct Cyc_List_List*Cyc_Specsfile_cyclone_exec_path;
# 37
struct Cyc_List_List*Cyc_Specsfile_cyclone_arch_path;
struct _fat_ptr Cyc_Specsfile_def_lib_path;
# 31 "specsfile.cyc"
extern char*Cdef_lib_path;
extern char*Carch;
# 38
struct _fat_ptr Cyc_Specsfile_target_arch={(void*)0,(void*)0,(void*)(0 + 0)};
void Cyc_Specsfile_set_target_arch(struct _fat_ptr s){
Cyc_Specsfile_target_arch=s;}
# 43
struct Cyc_List_List*Cyc_Specsfile_cyclone_exec_path=0;
void Cyc_Specsfile_add_cyclone_exec_path(struct _fat_ptr s){
unsigned long len=Cyc_strlen(s);
if(len <= 2U)return;{
struct _fat_ptr dir=Cyc_substring(s,2,len - 2U);
Cyc_Specsfile_cyclone_exec_path=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=dir;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=Cyc_Specsfile_cyclone_exec_path;_Tmp0;});}}struct _tuple0{struct _fat_ptr*f1;struct _fat_ptr*f2;};
# 57
struct Cyc_List_List*Cyc_Specsfile_read_specs(struct _fat_ptr file){
struct Cyc_List_List*result=0;
int c;
int i;
char strname[256U];
char strvalue[4096U];
struct Cyc___cycFILE*spec_file=Cyc_fopen((const char*)_check_null(_untag_fat_ptr(file,sizeof(char),1U)),"r");
# 65
if(spec_file==0){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=file;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error opening spec file %s\n";_tag_fat(_Tmp2,sizeof(char),28U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
Cyc_fflush(Cyc_stderr);
return 0;}
# 71
while(1){
while(1){
c=Cyc_fgetc(spec_file);
if(isspace(c))continue;
if(c==42)break;
if(c!=-1){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=file;_Tmp1;});struct Cyc_Int_pa_PrintArg_struct _Tmp1=({struct Cyc_Int_pa_PrintArg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=(unsigned long)c;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="Error reading spec file %s: unexpected character '%c'\n";_tag_fat(_Tmp3,sizeof(char),55U);}),_tag_fat(_Tmp2,sizeof(void*),2));});
# 80
Cyc_fflush(Cyc_stderr);}
# 82
goto CLEANUP_AND_RETURN;}
# 84
JUST_AFTER_STAR:
 i=0;
while(1){
c=Cyc_fgetc(spec_file);
if(c==-1){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=file;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error reading spec file %s: unexpected EOF\n";_tag_fat(_Tmp2,sizeof(char),44U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
# 92
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}
# 95
if(c==58){
*((char*)_check_known_subscript_notnull(strname,256U,sizeof(char),i))='\000';
break;}
# 99
*((char*)_check_known_subscript_notnull(strname,256U,sizeof(char),i))=(char)c;
++ i;
if(i >= 256){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=file;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error reading spec file %s: string name too long\n";_tag_fat(_Tmp2,sizeof(char),50U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
# 105
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}}
# 109
while(1){
c=Cyc_fgetc(spec_file);
if(isspace(c))continue;
break;}
# 114
if(c==42){
# 116
result=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp1=({struct _tuple0*_Tmp2=_cycalloc(sizeof(struct _tuple0));({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_strdup(_tag_fat(strname,sizeof(char),256U));*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;}),({
struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_strdup(({const char*_Tmp6="";_tag_fat(_Tmp6,sizeof(char),1U);}));*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f2=_Tmp3;});_Tmp2;});
# 116
_Tmp0->hd=_Tmp1;}),_Tmp0->tl=result;_Tmp0;});
# 118
goto JUST_AFTER_STAR;}
# 121
strvalue[0]=(char)c;
i=1;
while(1){
c=Cyc_fgetc(spec_file);
if((c==-1 || c==10)|| c==13){*((char*)_check_known_subscript_notnull(strvalue,4096U,sizeof(char),i))='\000';break;}
*((char*)_check_known_subscript_notnull(strvalue,4096U,sizeof(char),i))=(char)c;
++ i;
if(i >= 4096){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=file;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=
# 131
_tag_fat(strname,sizeof(char),256U);_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="Error reading spec file %s: value of %s too long\n";_tag_fat(_Tmp3,sizeof(char),50U);}),_tag_fat(_Tmp2,sizeof(void*),2));});
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}}
# 137
result=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp1=({struct _tuple0*_Tmp2=_cycalloc(sizeof(struct _tuple0));({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_strdup(_tag_fat(strname,sizeof(char),256U));*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;}),({
struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_strdup(_tag_fat(strvalue,sizeof(char),4096U));*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f2=_Tmp3;});_Tmp2;});
# 137
_Tmp0->hd=_Tmp1;}),_Tmp0->tl=result;_Tmp0;});
# 139
if(c==-1)goto CLEANUP_AND_RETURN;}
# 142
CLEANUP_AND_RETURN:
 Cyc_fclose(spec_file);
return result;}
# 148
struct _fat_ptr Cyc_Specsfile_split_specs(struct _fat_ptr cmdline){
if((char*)cmdline.curr==(char*)_tag_fat(0,0,0).curr)return _tag_fat(0,0,0);{
unsigned long n=Cyc_strlen(cmdline);
struct Cyc_List_List*l=0;
char buf[4096U];
int i=0;
int j=0;
if(n > 4096U)goto DONE;
while(1){
# 158
while(1){
if((unsigned long)i >= n)goto DONE;
if((int)*((const char*)_check_fat_subscript(cmdline,sizeof(char),i))==0)goto DONE;
if(!isspace((int)((const char*)cmdline.curr)[i]))break;
++ i;}
# 164
j=0;
# 169
while(1){
if((unsigned long)i >= n)break;
if((int)*((const char*)_check_fat_subscript(cmdline,sizeof(char),i))==0)break;
if(isspace((int)((const char*)cmdline.curr)[i]))break;
if((int)((const char*)cmdline.curr)[i]==92){
# 175
++ i;
if((unsigned long)i >= n)break;
if((int)*((const char*)_check_fat_subscript(cmdline,sizeof(char),i))==0)break;
*((char*)_check_known_subscript_notnull(buf,4096U,sizeof(char),j))=((const char*)cmdline.curr)[i];
++ j;}else{
# 182
*((char*)_check_known_subscript_notnull(buf,4096U,sizeof(char),j))=((const char*)cmdline.curr)[i];
++ j;}
# 185
++ i;}
# 187
if(j < 4096)
*((char*)_check_known_subscript_notnull(buf,4096U,sizeof(char),j))='\000';
# 190
l=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_strdup(_tag_fat(buf,sizeof(char),4096U));*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=l;_Tmp0;});}
# 192
DONE:
 l=Cyc_List_imp_rev(l);
l=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=({const char*_Tmp3="";_tag_fat(_Tmp3,sizeof(char),1U);});_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=l;_Tmp0;});{
struct _fat_ptr ptrarray=Cyc_List_to_array(l);
struct _fat_ptr result=({unsigned _Tmp0=_get_fat_size(ptrarray,sizeof(struct _fat_ptr*));_tag_fat(({struct _fat_ptr*_Tmp1=_cycalloc(_check_times(_Tmp0,sizeof(struct _fat_ptr)));({{unsigned _Tmp2=_get_fat_size(ptrarray,sizeof(struct _fat_ptr*));unsigned k;for(k=0;k < _Tmp2;++ k){_Tmp1[k]=*((struct _fat_ptr**)ptrarray.curr)[(int)k];}}0;});_Tmp1;}),sizeof(struct _fat_ptr),_Tmp0);});
return result;}}}
# 200
struct _fat_ptr Cyc_Specsfile_get_spec(struct Cyc_List_List*specs,struct _fat_ptr spec_name){
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{struct _fat_ptr _Tmp2=*({(struct _fat_ptr*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))Cyc_List_assoc_cmp;})(Cyc_strptrcmp,specs,& spec_name);_npop_handler(0);return _Tmp2;};_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp2)->tag==Cyc_Core_Not_found)
# 205
return _tag_fat(0,0,0);else{_Tmp3=_Tmp2;{void*exn=_Tmp3;(void*)_rethrow(exn);}};}}}
# 209
struct Cyc_List_List*Cyc_Specsfile_cyclone_arch_path=0;
struct _fat_ptr Cyc_Specsfile_def_lib_path={(void*)0,(void*)0,(void*)(0 + 0)};
# 214
static struct Cyc_List_List*Cyc_Specsfile_also_subdir(struct Cyc_List_List*dirs,struct _fat_ptr subdir){
# 216
struct Cyc_List_List*l=0;
for(1;dirs!=0;dirs=dirs->tl){
l=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=(struct _fat_ptr*)dirs->hd,_Tmp0->tl=l;_Tmp0;});
l=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_Filename_concat(*((struct _fat_ptr*)dirs->hd),subdir);*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=l;_Tmp0;});}
# 221
l=Cyc_List_imp_rev(l);
return l;}
# 236 "specsfile.cyc"
struct _fat_ptr Cyc_Specsfile_parse_b(struct Cyc_List_List*specs,void(*anonfun)(struct _fat_ptr),int(*anonflagfun)(struct _fat_ptr),struct _fat_ptr errmsg,struct _fat_ptr argv){
# 244
int argc=(int)_get_fat_size(argv,sizeof(struct _fat_ptr));
struct _fat_ptr bindices=({unsigned _Tmp0=(unsigned)argc;_tag_fat(({int*_Tmp1=_cycalloc_atomic(_check_times(_Tmp0,sizeof(int)));({{unsigned _Tmp2=(unsigned)argc;unsigned i;for(i=0;i < _Tmp2;++ i){_Tmp1[i]=0;}}0;});_Tmp1;}),sizeof(int),_Tmp0);});
int numbindices=0;
int i;int j;int k;
for(i=1;i < argc;++ i){
if(Cyc_strncmp(({const char*_Tmp0="-B";_tag_fat(_Tmp0,sizeof(char),3U);}),*((struct _fat_ptr*)_check_fat_subscript(argv,sizeof(struct _fat_ptr),i)),2U)==0){
*((int*)_check_fat_subscript(bindices,sizeof(int),i))=1;
++ numbindices;}else{
# 253
if(Cyc_strcmp(({const char*_Tmp0="-b";_tag_fat(_Tmp0,sizeof(char),3U);}),((struct _fat_ptr*)argv.curr)[i])==0){
*((int*)_check_fat_subscript(bindices,sizeof(int),i))=1;
++ numbindices;
++ i;if(i >= argc)break;
*((int*)_check_fat_subscript(bindices,sizeof(int),i))=1;
++ numbindices;}}}{
# 265
struct _fat_ptr bargs=({unsigned _Tmp0=(unsigned)(numbindices + 1);_tag_fat(({struct _fat_ptr*_Tmp1=_cycalloc(_check_times(_Tmp0,sizeof(struct _fat_ptr)));({{unsigned _Tmp2=(unsigned)(numbindices + 1);unsigned n;for(n=0;n < _Tmp2;++ n){_Tmp1[n]=_tag_fat(0,0,0);}}0;});_Tmp1;}),sizeof(struct _fat_ptr),_Tmp0);});
struct _fat_ptr otherargs=({unsigned _Tmp0=(unsigned)(argc - numbindices);_tag_fat(({struct _fat_ptr*_Tmp1=_cycalloc(_check_times(_Tmp0,sizeof(struct _fat_ptr)));({{unsigned _Tmp2=(unsigned)(argc - numbindices);unsigned n;for(n=0;n < _Tmp2;++ n){_Tmp1[n]=_tag_fat(0,0,0);}}0;});_Tmp1;}),sizeof(struct _fat_ptr),_Tmp0);});
({struct _fat_ptr _Tmp0=({struct _fat_ptr _Tmp1=*((struct _fat_ptr*)_check_fat_subscript(argv,sizeof(struct _fat_ptr),0));*((struct _fat_ptr*)_check_fat_subscript(otherargs,sizeof(struct _fat_ptr),0))=_Tmp1;});*((struct _fat_ptr*)_check_fat_subscript(bargs,sizeof(struct _fat_ptr),0))=_Tmp0;});
for(i=(j=(k=1));i < argc;++ i){
if(*((int*)_check_fat_subscript(bindices,sizeof(int),i)))({struct _fat_ptr _Tmp0=*((struct _fat_ptr*)_check_fat_subscript(argv,sizeof(struct _fat_ptr),i));*((struct _fat_ptr*)_check_fat_subscript(bargs,sizeof(struct _fat_ptr),j ++))=_Tmp0;});else{
({struct _fat_ptr _Tmp0=*((struct _fat_ptr*)_check_fat_subscript(argv,sizeof(struct _fat_ptr),i));*((struct _fat_ptr*)_check_fat_subscript(otherargs,sizeof(struct _fat_ptr),k ++))=_Tmp0;});}}
# 273
Cyc_Arg_current=0;
Cyc_Arg_parse(specs,anonfun,anonflagfun,errmsg,bargs);
# 278
if((char*)Cyc_Specsfile_target_arch.curr==(char*)_tag_fat(0,0,0).curr)Cyc_Specsfile_target_arch=({char*_Tmp0=Carch;_tag_fat(_Tmp0,sizeof(char),_get_zero_arr_size_char((void*)_Tmp0,1U));});{
# 287 "specsfile.cyc"
struct _fat_ptr cyclone_exec_prefix=({char*_Tmp0=getenv("CYCLONE_EXEC_PREFIX");_tag_fat(_Tmp0,sizeof(char),_get_zero_arr_size_char((void*)_Tmp0,1U));});
if((char*)cyclone_exec_prefix.curr!=(char*)_tag_fat(0,0,0).curr)
Cyc_Specsfile_cyclone_exec_path=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=cyclone_exec_prefix;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=Cyc_Specsfile_cyclone_exec_path;_Tmp0;});
Cyc_Specsfile_def_lib_path=({char*_Tmp0=Cdef_lib_path;_tag_fat(_Tmp0,sizeof(char),_get_zero_arr_size_char((void*)_Tmp0,1U));});
if(Cyc_strlen(Cyc_Specsfile_def_lib_path)> 0U)
Cyc_Specsfile_cyclone_exec_path=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));
({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_Filename_concat(Cyc_Specsfile_def_lib_path,({const char*_Tmp4="cyc-lib";_tag_fat(_Tmp4,sizeof(char),8U);}));*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=Cyc_Specsfile_cyclone_exec_path;_Tmp0;});
# 295
Cyc_Specsfile_cyclone_exec_path=Cyc_List_imp_rev(Cyc_Specsfile_cyclone_exec_path);
Cyc_Specsfile_cyclone_arch_path=Cyc_Specsfile_also_subdir(Cyc_Specsfile_cyclone_exec_path,Cyc_Specsfile_target_arch);
return otherargs;}}}
# 301
static int Cyc_Specsfile_file_exists(struct _fat_ptr file){
struct Cyc___cycFILE*f=0;
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){f=Cyc_file_open(file,({const char*_Tmp2="r";_tag_fat(_Tmp2,sizeof(char),2U);}));;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();goto _LL0;_LL0:;}}}
if(f==0)return 0;else{
Cyc_fclose(f);return 1;}}
# 310
static struct _fat_ptr*Cyc_Specsfile_find(struct Cyc_List_List*dirs,struct _fat_ptr file){
if((char*)file.curr==(char*)_tag_fat(0,0,0).curr)return 0;
for(1;dirs!=0;dirs=dirs->tl){
struct _fat_ptr dir=*((struct _fat_ptr*)dirs->hd);
if((char*)dir.curr==(char*)_tag_fat(0,0,0).curr || Cyc_strlen(dir)==0U)continue;{
struct _fat_ptr s=Cyc_Filename_concat(dir,file);
if(Cyc_Specsfile_file_exists(s))return({struct _fat_ptr*_Tmp0=_cycalloc(sizeof(struct _fat_ptr));*_Tmp0=s;_Tmp0;});}}
# 318
return 0;}
# 322
static struct _fat_ptr Cyc_Specsfile_sprint_list(struct Cyc_List_List*dirs){
struct _fat_ptr tmp=({const char*_Tmp0="";_tag_fat(_Tmp0,sizeof(char),1U);});
for(1;dirs!=0;dirs=dirs->tl){
struct _fat_ptr dir=*((struct _fat_ptr*)dirs->hd);
if((char*)dir.curr==(char*)_tag_fat(0,0,0).curr || Cyc_strlen(dir)==0U)continue;
dir=Cyc_strconcat(dir,({const char*_Tmp0=":";_tag_fat(_Tmp0,sizeof(char),2U);}));
tmp=Cyc_strconcat(dir,tmp);}
# 330
return tmp;}
# 333
static struct _fat_ptr Cyc_Specsfile_do_find(struct Cyc_List_List*dirs,struct _fat_ptr file){
struct _fat_ptr*f=Cyc_Specsfile_find(dirs,file);
if(f==0){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=file;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({
struct _fat_ptr _Tmp3=Cyc_Specsfile_sprint_list(dirs);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="Error: can't find internal compiler file %s in path %s\n";_tag_fat(_Tmp3,sizeof(char),56U);}),_tag_fat(_Tmp2,sizeof(void*),2));});
Cyc_fflush(Cyc_stderr);
return _tag_fat(0,0,0);}
# 341
return*f;}
# 344
struct _fat_ptr Cyc_Specsfile_find_in_arch_path(struct _fat_ptr s){
return Cyc_Specsfile_do_find(Cyc_Specsfile_cyclone_arch_path,s);}
# 348
struct _fat_ptr Cyc_Specsfile_find_in_exec_path(struct _fat_ptr s){
return Cyc_Specsfile_do_find(Cyc_Specsfile_cyclone_exec_path,s);}
