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

struct _RegionHandle _new_region(const char*);
void* _region_malloc(struct _RegionHandle*, unsigned);
void* _region_calloc(struct _RegionHandle*, unsigned t, unsigned n);
void* _region_vmalloc(struct _RegionHandle*, unsigned);
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

static inline void*_fast_region_malloc(struct _RegionHandle*r, unsigned orig_s) {  
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

# 95 "core.h"
 struct _fat_ptr Cyc_Core_new_string(unsigned);
# 99
struct _fat_ptr Cyc_Core_rnew_string(struct _RegionHandle*,unsigned);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};
# 168
extern struct _RegionHandle*Cyc_Core_heap_region;
# 322 "core.h"
struct _fat_ptr Cyc_Core_mkfat(void*,unsigned,unsigned);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);
# 310 "cycboot.h"
extern int toupper(int);
# 72 "string.h"
struct _fat_ptr Cyc_strncpy(struct _fat_ptr,struct _fat_ptr,unsigned long);
# 105 "string.h"
struct _fat_ptr Cyc_rstrdup(struct _RegionHandle*,struct _fat_ptr);
# 29 "assert.h"
extern void*Cyc___assert_fail(struct _fat_ptr,struct _fat_ptr,unsigned);
# 37 "string.cyc"
unsigned long Cyc_strlen(struct _fat_ptr s){
# 39
unsigned long i;
unsigned sz=_get_fat_size(s,sizeof(char));
for(i=0U;1 && i < sz;++ i){
if((int)((const char*)s.curr)[(int)i]==0)
return i;}
return i;}
# 47
inline static unsigned Cyc_umin(unsigned i,unsigned j){
# 49
if(i < j)return i;else{return j;}}
# 51
inline static unsigned Cyc_umax(unsigned i,unsigned j){
# 53
if(i > j)return i;else{return j;}}
# 59
int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2){
if((char*)s1.curr==(char*)s2.curr)
return 0;{
int i=0;
unsigned sz1=_get_fat_size(s1,sizeof(char));
unsigned sz2=_get_fat_size(s2,sizeof(char));
unsigned minsz=Cyc_umin(sz1,sz2);
# 67
for(1;1 &&(unsigned)i < minsz;++ i){
char c1=((const char*)s1.curr)[i];
char c2=((const char*)s2.curr)[i];
if((int)c1==0)
return(int)c2==0?0: -1;
if((int)c2==0)return 1;{
int diff=(int)c1 - (int)c2;
if(diff!=0)return diff;}}
# 76
if(sz1==sz2)return 0;
if(sz1 < sz2)return(int)((const char*)s2.curr)[i]==0?0: -1;
1;
return(int)((const char*)s1.curr)[i]==0?0: 1;}}
# 82
int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2){
return Cyc_strcmp(*s1,*s2);}
# 86
inline static int Cyc_ncmp(struct _fat_ptr s1,unsigned long len1,struct _fat_ptr s2,unsigned long len2,unsigned long n){
# 89
if(n <= 0U)return 0;{
# 91
unsigned long min_len=Cyc_umin(len1,len2);
unsigned long bound=Cyc_umin(min_len,n);
# 94
{unsigned i=0U;for(0;i < bound;++ i){
int retc;
if((retc=(int)((const char*)s1.curr)[(int)i]- (int)((const char*)s2.curr)[(int)i])!=0)
return retc;}}
# 99
if(len1 < n || len2 < n)
return(int)len1 - (int)len2;
return 0;}}
# 106
int Cyc_strncmp(struct _fat_ptr s1,struct _fat_ptr s2,unsigned long n){
unsigned long len1=Cyc_strlen(s1);
unsigned long len2=Cyc_strlen(s2);
1;
return Cyc_ncmp(s1,len1,s2,len2,n);}
# 117
int Cyc_zstrcmp(struct _fat_ptr a,struct _fat_ptr b){
if((char*)a.curr==(char*)b.curr)
return 0;{
unsigned long as=_get_fat_size(a,sizeof(char));
unsigned long bs=_get_fat_size(b,sizeof(char));
unsigned long min_length=Cyc_umin(as,bs);
int i=-1;
# 125
1;
# 127
while((++ i,(unsigned long)i < min_length)){
int diff=(int)((const char*)a.curr)[i]- (int)((const char*)b.curr)[i];
if(diff!=0)
return diff;}
# 132
return(int)as - (int)bs;}}
# 135
int Cyc_zstrncmp(struct _fat_ptr s1,struct _fat_ptr s2,unsigned long n){
if(n <= 0U)return 0;{
# 138
unsigned long s1size=_get_fat_size(s1,sizeof(char));
unsigned long s2size=_get_fat_size(s2,sizeof(char));
unsigned long min_size=Cyc_umin(s1size,s2size);
unsigned long bound=Cyc_umin(n,min_size);
# 143
1;
# 145
{int i=0;for(0;(unsigned long)i < bound;++ i){
if((int)((const char*)s1.curr)[i]< (int)((const char*)s2.curr)[i])
return -1;
if((int)((const char*)s2.curr)[i]< (int)((const char*)s1.curr)[i])
return 1;}}
# 151
if(min_size <= bound)
return 0;
if(s1size < s2size)
return -1;
return 1;}}
# 159
int Cyc_zstrptrcmp(struct _fat_ptr*a,struct _fat_ptr*b){
if((int)a==(int)b)return 0;
return Cyc_zstrcmp(*a,*b);}
# 169
inline static struct _fat_ptr Cyc_int_strcato(struct _fat_ptr dest,struct _fat_ptr src){
int i;
unsigned long dsize;unsigned long slen;unsigned long dlen;
# 173
dsize=_get_fat_size(dest,sizeof(char));
dlen=Cyc_strlen(dest);
slen=Cyc_strlen(src);
# 177
if(slen + dlen <= dsize){
1;
for(i=0;(unsigned long)i < slen;++ i){
({struct _fat_ptr _Tmp0=_fat_ptr_plus(dest,sizeof(char),(int)((unsigned long)i + dlen));char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=((const char*)src.curr)[i];if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}
# 182
if((unsigned long)i + dlen < dsize)
({struct _fat_ptr _Tmp0=_fat_ptr_plus(dest,sizeof(char),(int)((unsigned long)i + dlen));char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='\000';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}else{
# 186
(void*)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_Tmp0->tag=Cyc_Core_Invalid_argument,_Tmp0->f1=({const char*_Tmp1="strcat";_tag_fat(_Tmp1,sizeof(char),7U);});_Tmp0;}));}
return dest;}
# 192
struct _fat_ptr Cyc_strcat(struct _fat_ptr dest,struct _fat_ptr src){
return Cyc_int_strcato(dest,src);}
# 197
struct _fat_ptr Cyc_rstrconcat(struct _RegionHandle*r,struct _fat_ptr a,struct _fat_ptr b){
unsigned long alen=Cyc_strlen(a);
unsigned long blen=Cyc_strlen(b);
struct _fat_ptr ans=Cyc_Core_rnew_string(r,(alen + blen)+ 1U);
int i;int j;
alen <= _get_fat_size(ans,sizeof(char))?0:({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;})(({const char*_Tmp0="alen <= numelts(ans)";_tag_fat(_Tmp0,sizeof(char),21U);}),({const char*_Tmp0="string.cyc";_tag_fat(_Tmp0,sizeof(char),11U);}),202U);
1;
for(i=0;(unsigned long)i < alen;++ i){({struct _fat_ptr _Tmp0=_fat_ptr_plus(ans,sizeof(char),i);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=((const char*)a.curr)[i];if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}
1;
for(j=0;(unsigned long)j < blen;++ j){({struct _fat_ptr _Tmp0=_fat_ptr_plus(ans,sizeof(char),i + j);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=((const char*)b.curr)[j];if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}
return ans;}
# 210
struct _fat_ptr Cyc_strconcat(struct _fat_ptr a,struct _fat_ptr b){
return Cyc_rstrconcat(Cyc_Core_heap_region,a,b);}
# 215
struct _fat_ptr Cyc_rstrconcat_l(struct _RegionHandle*r,struct Cyc_List_List*strs){
# 217
unsigned long len;
unsigned long total_len=0U;
struct _fat_ptr ans;
struct _RegionHandle _Tmp0=_new_region("temp");struct _RegionHandle*temp=& _Tmp0;_push_region(temp);{
struct Cyc_List_List*lens;
lens=_region_malloc(temp,sizeof(struct Cyc_List_List)),lens->hd=(void*)0U,lens->tl=0;{
# 224
struct Cyc_List_List*end=lens;
{struct Cyc_List_List*p=strs;for(0;p!=0;p=p->tl){
len=Cyc_strlen(*((struct _fat_ptr*)p->hd));
total_len +=len;
({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_region_malloc(temp,sizeof(struct Cyc_List_List));_Tmp2->hd=(void*)len,_Tmp2->tl=0;_Tmp2;});_check_null(end)->tl=_Tmp1;});
end=end->tl;}}
# 231
lens=lens->tl;
ans=Cyc_Core_rnew_string(r,total_len + 1U);{
unsigned long i=0U;
while(strs!=0){
struct _fat_ptr next=*((struct _fat_ptr*)strs->hd);
len=(unsigned long)_check_null(lens)->hd;
({struct _fat_ptr _Tmp1=_fat_ptr_decrease_size(_fat_ptr_plus(ans,sizeof(char),(int)i),sizeof(char),1U);struct _fat_ptr _Tmp2=next;Cyc_strncpy(_Tmp1,_Tmp2,len);});
i +=len;
strs=strs->tl;
lens=lens->tl;}}}}{
# 243
struct _fat_ptr _Tmp1=ans;_npop_handler(0);return _Tmp1;}
# 220
;_pop_region();}
# 246
struct _fat_ptr Cyc_strconcat_l(struct Cyc_List_List*strs){
return Cyc_rstrconcat_l(Cyc_Core_heap_region,strs);}
# 252
struct _fat_ptr Cyc_rstr_sepstr(struct _RegionHandle*r,struct Cyc_List_List*strs,struct _fat_ptr separator){
if(strs==0)return Cyc_Core_rnew_string(r,0U);
if(strs->tl==0)return Cyc_rstrdup(r,*((struct _fat_ptr*)strs->hd));{
struct Cyc_List_List*p=strs;
struct _RegionHandle _Tmp0=_new_region("temp");struct _RegionHandle*temp=& _Tmp0;_push_region(temp);
{struct Cyc_List_List*lens;lens=_region_malloc(temp,sizeof(struct Cyc_List_List)),lens->hd=(void*)0U,lens->tl=0;{
struct Cyc_List_List*end=lens;
unsigned long len;
unsigned long total_len=0U;
unsigned long list_len=0U;
for(1;p!=0;p=p->tl){
len=Cyc_strlen(*((struct _fat_ptr*)p->hd));
total_len +=len;
({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_region_malloc(temp,sizeof(struct Cyc_List_List));_Tmp2->hd=(void*)len,_Tmp2->tl=0;_Tmp2;});_check_null(end)->tl=_Tmp1;});
end=end->tl;
++ list_len;}
# 269
lens=lens->tl;{
unsigned long seplen=Cyc_strlen(separator);
total_len +=(list_len - 1U)* seplen;{
struct _fat_ptr ans=Cyc_Core_rnew_string(r,total_len + 1U);
unsigned long i=0U;
while(_check_null(strs)->tl!=0){
struct _fat_ptr next=*((struct _fat_ptr*)strs->hd);
len=(unsigned long)_check_null(lens)->hd;
({struct _fat_ptr _Tmp1=_fat_ptr_decrease_size(_fat_ptr_plus(ans,sizeof(char),(int)i),sizeof(char),1U);struct _fat_ptr _Tmp2=next;Cyc_strncpy(_Tmp1,_Tmp2,len);});
i +=len;
({struct _fat_ptr _Tmp1=_fat_ptr_decrease_size(_fat_ptr_plus(ans,sizeof(char),(int)i),sizeof(char),1U);struct _fat_ptr _Tmp2=separator;Cyc_strncpy(_Tmp1,_Tmp2,seplen);});
i +=seplen;
strs=strs->tl;
lens=lens->tl;}
# 284
({struct _fat_ptr _Tmp1=_fat_ptr_decrease_size(_fat_ptr_plus(ans,sizeof(char),(int)i),sizeof(char),1U);struct _fat_ptr _Tmp2=*((struct _fat_ptr*)strs->hd);Cyc_strncpy(_Tmp1,_Tmp2,(unsigned long)_check_null(lens)->hd);});{
struct _fat_ptr _Tmp1=ans;_npop_handler(0);return _Tmp1;}}}}}
# 257
;_pop_region();}}
# 289
struct _fat_ptr Cyc_str_sepstr(struct Cyc_List_List*strs,struct _fat_ptr separator){
return Cyc_rstr_sepstr(Cyc_Core_heap_region,strs,separator);}
# 294
struct _fat_ptr Cyc_strncpy(struct _fat_ptr dest,struct _fat_ptr src,unsigned long n){
int i;
n <= _get_fat_size(dest,sizeof(char))?0:({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;})(({const char*_Tmp0="n <= numelts(dest)";_tag_fat(_Tmp0,sizeof(char),19U);}),({const char*_Tmp0="string.cyc";_tag_fat(_Tmp0,sizeof(char),11U);}),296U);
for(i=0;(unsigned long)i < n;++ i){
char srcChar=((const char*)src.curr)[i];
if((int)srcChar==0)break;
((char*)dest.curr)[i]=srcChar;}
# 302
for(1;(unsigned long)i < n;++ i){
((char*)dest.curr)[i]='\000';}
# 305
return dest;}
# 309
struct _fat_ptr Cyc_zstrncpy(struct _fat_ptr dest,struct _fat_ptr src,unsigned long n){
n <= _get_fat_size(dest,sizeof(char))&& n <= _get_fat_size(src,sizeof(char))?0:({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;})(({const char*_Tmp0="n <= numelts(dest) && n <= numelts(src)";_tag_fat(_Tmp0,sizeof(char),40U);}),({const char*_Tmp0="string.cyc";_tag_fat(_Tmp0,sizeof(char),11U);}),310U);{
int i;
for(i=0;(unsigned long)i < n;++ i){
((char*)dest.curr)[i]=((const char*)src.curr)[i];}
return dest;}}
# 317
struct _fat_ptr Cyc_strcpy(struct _fat_ptr dest,struct _fat_ptr src){
unsigned ssz=_get_fat_size(src,sizeof(char));
unsigned dsz=_get_fat_size(dest,sizeof(char));
if(ssz <= dsz){
unsigned i;
for(i=0U;i < ssz;++ i){
char srcChar=((const char*)src.curr)[(int)i];
({struct _fat_ptr _Tmp0=_fat_ptr_plus(dest,sizeof(char),(int)i);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=srcChar;if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
if((int)srcChar==0)break;}}else{
# 329
unsigned long len=Cyc_strlen(src);
({struct _fat_ptr _Tmp0=_fat_ptr_decrease_size(dest,sizeof(char),1U);struct _fat_ptr _Tmp1=src;Cyc_strncpy(_Tmp0,_Tmp1,len);});
if(len < _get_fat_size(dest,sizeof(char)))
({struct _fat_ptr _Tmp0=_fat_ptr_plus(dest,sizeof(char),(int)len);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='\000';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}
# 334
return dest;}
# 340
struct _fat_ptr Cyc_rstrdup(struct _RegionHandle*r,struct _fat_ptr src){
unsigned long len;
struct _fat_ptr temp;
# 344
len=Cyc_strlen(src);
temp=Cyc_Core_rnew_string(r,len + 1U);
{struct _fat_ptr temp2=_fat_ptr_decrease_size(temp,sizeof(char),1U);
struct _fat_ptr _Tmp0;_Tmp0=temp2;{struct _fat_ptr dst=_Tmp0;
Cyc_strncpy(dst,src,len);}}
# 350
return temp;}
# 353
struct _fat_ptr Cyc_strdup(struct _fat_ptr src){
return Cyc_rstrdup(Cyc_Core_heap_region,src);}
# 357
struct _fat_ptr Cyc_rrealloc(struct _RegionHandle*r,struct _fat_ptr s,unsigned long sz){
struct _fat_ptr temp;
unsigned long slen;
# 361
slen=_get_fat_size(s,sizeof(char));
sz=sz > slen?sz: slen;
temp=({unsigned _Tmp0=sz;_tag_fat(_region_calloc(r,sizeof(char),_Tmp0),sizeof(char),_Tmp0);});
# 365
{struct _fat_ptr _Tmp0;_Tmp0=temp;{struct _fat_ptr dst=_Tmp0;
Cyc_strncpy(dst,s,slen);}}
# 369
return temp;}
# 372
struct _fat_ptr Cyc_rexpand(struct _RegionHandle*r,struct _fat_ptr s,unsigned long sz){
struct _fat_ptr temp;
unsigned long slen;
# 376
slen=Cyc_strlen(s);
sz=sz > slen?sz: slen;
temp=Cyc_Core_rnew_string(r,sz);
# 380
{struct _fat_ptr _Tmp0;_Tmp0=temp;{struct _fat_ptr dst=_Tmp0;
Cyc_strncpy(dst,s,slen);}}
# 384
if(slen!=_get_fat_size(s,sizeof(char)))
({struct _fat_ptr _Tmp0=_fat_ptr_plus(temp,sizeof(char),(int)slen);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='\000';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
# 387
return temp;}
# 390
struct _fat_ptr Cyc_expand(struct _fat_ptr s,unsigned long sz){
return Cyc_rexpand(Cyc_Core_heap_region,s,sz);}
# 394
struct _fat_ptr Cyc_cond_rrealloc_str(struct _RegionHandle*r,struct _fat_ptr str,unsigned long sz){
unsigned long maxsizeP=_get_fat_size(str,sizeof(char));
struct _fat_ptr res=_tag_fat(0,0,0);
# 398
if(maxsizeP==0U){
maxsizeP=Cyc_umax(30U,sz);
res=Cyc_Core_rnew_string(r,maxsizeP);
({struct _fat_ptr _Tmp0=_fat_ptr_plus(res,sizeof(char),0);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='\000';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}else{
# 403
if(sz > maxsizeP){
maxsizeP=maxsizeP * 2U > (sz * 5U)/ 4U?maxsizeP * 2U:(sz * 5U)/ 4U;{
struct _fat_ptr _Tmp0;_Tmp0=str;{struct _fat_ptr dst=_Tmp0;
res=Cyc_rexpand(r,dst,maxsizeP);}}}}
# 410
return res;}
# 413
struct _fat_ptr Cyc_realloc_str(struct _fat_ptr str,unsigned long sz){
struct _fat_ptr res=Cyc_cond_rrealloc_str(Cyc_Core_heap_region,str,sz);
if((char*)res.curr==(char*)_tag_fat(0,0,0).curr)return str;else{
return res;}}
# 424
struct _fat_ptr Cyc_rsubstring(struct _RegionHandle*r,struct _fat_ptr s,int start,unsigned long amt){
# 428
struct _fat_ptr ans=Cyc_Core_rnew_string(r,amt + 1U);
s=_fat_ptr_plus(s,sizeof(char),start);
amt < _get_fat_size(ans,sizeof(char))&& amt <= _get_fat_size(s,sizeof(char))?0:({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;})(({const char*_Tmp0="amt < numelts(ans) && amt <= numelts(s)";_tag_fat(_Tmp0,sizeof(char),40U);}),({const char*_Tmp0="string.cyc";_tag_fat(_Tmp0,sizeof(char),11U);}),430U);
{unsigned long i=0U;for(0;i < amt;++ i){
({struct _fat_ptr _Tmp0=_fat_ptr_plus(ans,sizeof(char),(int)i);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=((const char*)s.curr)[(int)i];if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}}
({struct _fat_ptr _Tmp0=_fat_ptr_plus(ans,sizeof(char),(int)amt);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='\000';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
return ans;}
# 437
struct _fat_ptr Cyc_substring(struct _fat_ptr s,int start,unsigned long amt){
return Cyc_rsubstring(Cyc_Core_heap_region,s,start,amt);}
# 443
struct _fat_ptr Cyc_rreplace_suffix(struct _RegionHandle*r,struct _fat_ptr src,struct _fat_ptr curr_suffix,struct _fat_ptr new_suffix){
unsigned long m=_get_fat_size(src,sizeof(char));
unsigned long n=_get_fat_size(curr_suffix,sizeof(char));
struct _fat_ptr err=({const char*_Tmp0="replace_suffix";_tag_fat(_Tmp0,sizeof(char),15U);});
if(m < n)
(void*)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_Tmp0->tag=Cyc_Core_Invalid_argument,_Tmp0->f1=err;_Tmp0;}));
{unsigned long i=1U;for(0;i <= n;++ i){
if(({int _Tmp0=(int)*((const char*)_check_fat_subscript(src,sizeof(char),(int)(m - i)));_Tmp0!=(int)*((const char*)_check_fat_subscript(curr_suffix,sizeof(char),(int)(n - i)));}))
(void*)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_Tmp0->tag=Cyc_Core_Invalid_argument,_Tmp0->f1=err;_Tmp0;}));}}{
struct _fat_ptr ans=Cyc_Core_rnew_string(r,((m - n)+ _get_fat_size(new_suffix,sizeof(char)))+ 1U);
({struct _fat_ptr _Tmp0=_fat_ptr_decrease_size(ans,sizeof(char),1U);struct _fat_ptr _Tmp1=src;Cyc_strncpy(_Tmp0,_Tmp1,m - n);});
({struct _fat_ptr _Tmp0=_fat_ptr_decrease_size(_fat_ptr_plus(ans,sizeof(char),(int)(m - n)),sizeof(char),1U);struct _fat_ptr _Tmp1=new_suffix;Cyc_strncpy(_Tmp0,_Tmp1,_get_fat_size(new_suffix,sizeof(char)));});
return ans;}}
# 458
struct _fat_ptr Cyc_replace_suffix(struct _fat_ptr src,struct _fat_ptr curr_suffix,struct _fat_ptr new_suffix){
return Cyc_rreplace_suffix(Cyc_Core_heap_region,src,curr_suffix,new_suffix);}
# 465
struct _fat_ptr Cyc_strpbrk(struct _fat_ptr s,struct _fat_ptr accept){
int len=(int)_get_fat_size(s,sizeof(char));
unsigned asize=_get_fat_size(accept,sizeof(char));
char c;
unsigned i;
for(i=0U;i < (unsigned)len &&(int)(c=((const char*)s.curr)[(int)i])!=0;++ i){
unsigned j=0U;for(0;j < asize;++ j){
if((int)c==(int)((const char*)accept.curr)[(int)j])
return _fat_ptr_plus(s,sizeof(char),(int)i);}}
# 475
return _tag_fat(0,0,0);}
# 478
struct _fat_ptr Cyc_mstrpbrk(struct _fat_ptr s,struct _fat_ptr accept){
int len=(int)_get_fat_size(s,sizeof(char));
unsigned asize=_get_fat_size(accept,sizeof(char));
char c;
unsigned i;
for(i=0U;i < (unsigned)len &&(int)(c=((char*)s.curr)[(int)i])!=0;++ i){
unsigned j=0U;for(0;j < asize;++ j){
if((int)c==(int)((const char*)accept.curr)[(int)j])
return _fat_ptr_plus(s,sizeof(char),(int)i);}}
# 488
return _tag_fat(0,0,0);}
# 493
struct _fat_ptr Cyc_mstrchr(struct _fat_ptr s,char c){
int len=(int)_get_fat_size(s,sizeof(char));
char c2;
unsigned i;
# 498
for(i=0U;i < (unsigned)len &&(int)(c2=((char*)s.curr)[(int)i])!=0;++ i){
if((int)c2==(int)c)return _fat_ptr_plus(s,sizeof(char),(int)i);}
# 501
return _tag_fat(0,0,0);}
# 504
struct _fat_ptr Cyc_strchr(struct _fat_ptr s,char c){
int len=(int)_get_fat_size(s,sizeof(char));
char c2;
unsigned i;
# 509
for(i=0U;i < (unsigned)len &&(int)(c2=((const char*)s.curr)[(int)i])!=0;++ i){
if((int)c2==(int)c)return _fat_ptr_plus(s,sizeof(char),(int)i);}
# 512
return _tag_fat(0,0,0);}
# 517
struct _fat_ptr Cyc_strrchr(struct _fat_ptr s0,char c){
int len=(int)Cyc_strlen(s0);
int i=len - 1;
struct _fat_ptr s=s0;
_fat_ptr_inplace_plus(& s,sizeof(char),i);
# 523
for(1;i >= 0;(i --,_fat_ptr_inplace_plus_post(& s,sizeof(char),-1))){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),0U))==(int)c)
return s;}
# 527
return _tag_fat(0,0,0);}
# 530
struct _fat_ptr Cyc_mstrrchr(struct _fat_ptr s0,char c){
int len=(int)Cyc_strlen(s0);
int i=len - 1;
struct _fat_ptr s=s0;
_fat_ptr_inplace_plus(& s,sizeof(char),i);
# 536
for(1;i >= 0;(i --,_fat_ptr_inplace_plus_post(& s,sizeof(char),-1))){
if((int)*((char*)_check_fat_subscript(s,sizeof(char),0U))==(int)c)
return s;}
# 540
return _tag_fat(0,0,0);}
# 545
struct _fat_ptr Cyc_strstr(struct _fat_ptr haystack,struct _fat_ptr needle){
if(!((unsigned)haystack.curr)|| !((unsigned)needle.curr))(void*)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_Tmp0->tag=Cyc_Core_Invalid_argument,_Tmp0->f1=({const char*_Tmp1="strstr";_tag_fat(_Tmp1,sizeof(char),7U);});_Tmp0;}));
if((int)*((const char*)_check_fat_subscript(needle,sizeof(char),0U))==0)return haystack;{
# 549
int len=(int)Cyc_strlen(needle);
{struct _fat_ptr start=haystack;for(0;({
char*_Tmp0=(char*)(start=Cyc_strchr(start,*((const char*)needle.curr))).curr;_Tmp0!=(char*)_tag_fat(0,0,0).curr;});start=({
struct _fat_ptr _Tmp0=_fat_ptr_plus(start,sizeof(char),1);Cyc_strchr(_Tmp0,*((const char*)needle.curr));})){
if(Cyc_strncmp(start,needle,(unsigned long)len)==0)
return start;}}
# 556
return _tag_fat(0,0,0);}}
# 559
struct _fat_ptr Cyc_mstrstr(struct _fat_ptr haystack,struct _fat_ptr needle){
if(!((unsigned)haystack.curr)|| !((unsigned)needle.curr))(void*)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_Tmp0->tag=Cyc_Core_Invalid_argument,_Tmp0->f1=({const char*_Tmp1="mstrstr";_tag_fat(_Tmp1,sizeof(char),8U);});_Tmp0;}));
if((int)*((const char*)_check_fat_subscript(needle,sizeof(char),0U))==0)return haystack;{
# 563
int len=(int)Cyc_strlen(needle);
{struct _fat_ptr start=haystack;for(0;({
char*_Tmp0=(char*)(start=Cyc_mstrchr(start,*((const char*)needle.curr))).curr;_Tmp0!=(char*)_tag_fat(0,0,0).curr;});start=({
struct _fat_ptr _Tmp0=_fat_ptr_plus(start,sizeof(char),1);Cyc_mstrchr(_Tmp0,*((const char*)needle.curr));})){
if(Cyc_strncmp(start,needle,(unsigned long)len)==0)
return start;}}
# 570
return _tag_fat(0,0,0);}}
# 576
unsigned long Cyc_strspn(struct _fat_ptr s,struct _fat_ptr accept){
struct _fat_ptr b=s;
unsigned long len=Cyc_strlen(b);
unsigned asize=_get_fat_size(accept,sizeof(char));
1;
{unsigned long i=0U;for(0;i < len;++ i){
int j;
for(j=0;(unsigned)j < asize;++ j){
if((int)((const char*)b.curr)[(int)i]==(int)((const char*)accept.curr)[j])
break;}
if((unsigned)j==asize)
return i;}}
# 590
return len;}
# 596
unsigned long Cyc_strcspn(struct _fat_ptr s,struct _fat_ptr accept){
struct _fat_ptr b=s;
unsigned long len=Cyc_strlen(b);
unsigned asize=_get_fat_size(accept,sizeof(char));
# 601
1;
{unsigned long i=0U;for(0;i < len;++ i){
int j;
for(j=0;(unsigned)j < asize;++ j){
if((int)((const char*)b.curr)[(int)i]==(int)((const char*)accept.curr)[j])return i;}}}
# 607
return len;}
# 614
struct _fat_ptr Cyc_strtok(struct _fat_ptr s,struct _fat_ptr delim){
# 616
static struct _fat_ptr olds={(void*)0,(void*)0,(void*)(0 + 0)};
struct _fat_ptr token;
# 619
if((char*)s.curr==(char*)_tag_fat(0,0,0).curr){
if((char*)olds.curr==(char*)_tag_fat(0,0,0).curr)
return _tag_fat(0,0,0);
s=olds;}{
# 626
unsigned long inc=Cyc_strspn(s,delim);
if(inc >= _get_fat_size(s,sizeof(char))||(int)*((char*)_check_fat_subscript(_fat_ptr_plus(s,sizeof(char),(int)inc),sizeof(char),0U))==0){
# 629
olds=_tag_fat(0,0,0);
return _tag_fat(0,0,0);}else{
# 633
_fat_ptr_inplace_plus(& s,sizeof(char),(int)inc);}
# 636
token=s;
s=Cyc_mstrpbrk(token,delim);
if((char*)s.curr==(char*)_tag_fat(0,0,0).curr)
# 640
olds=_tag_fat(0,0,0);else{
# 644
({struct _fat_ptr _Tmp0=s;char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='\000';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
olds=_fat_ptr_plus(s,sizeof(char),1);}
# 647
return token;}}
# 651
struct Cyc_List_List*Cyc_rexplode(struct _RegionHandle*r,struct _fat_ptr s){
struct Cyc_List_List*result=0;
{int i=(int)(Cyc_strlen(s)- 1U);for(0;i >= 0;-- i){
result=({struct Cyc_List_List*_Tmp0=_region_malloc(r,sizeof(struct Cyc_List_List));_Tmp0->hd=(void*)((int)*((const char*)_check_fat_subscript(s,sizeof(char),i))),_Tmp0->tl=result;_Tmp0;});}}
return result;}
# 658
struct Cyc_List_List*Cyc_explode(struct _fat_ptr s){
return Cyc_rexplode(Cyc_Core_heap_region,s);}
# 662
struct _fat_ptr Cyc_implode(struct Cyc_List_List*chars){
struct _fat_ptr s=Cyc_Core_new_string((unsigned)(Cyc_List_length(chars)+ 1));
unsigned long i=0U;
while(chars!=0){
({struct _fat_ptr _Tmp0=_fat_ptr_plus(s,sizeof(char),(int)i ++);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=(char)((int)chars->hd);if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
chars=chars->tl;}
# 669
return s;}
# 673
inline static int Cyc_casecmp(struct _fat_ptr s1,unsigned long len1,struct _fat_ptr s2,unsigned long len2){
# 677
unsigned long min_length=Cyc_umin(len1,len2);
# 679
1;{
# 681
int i=-1;
while((++ i,(unsigned long)i < min_length)){
int diff=({int _Tmp0=toupper((int)((const char*)s1.curr)[i]);_Tmp0 - toupper((int)((const char*)s2.curr)[i]);});
if(diff!=0)
return diff;}
# 687
return(int)len1 - (int)len2;}}
# 690
int Cyc_strcasecmp(struct _fat_ptr s1,struct _fat_ptr s2){
if((char*)s1.curr==(char*)s2.curr)
return 0;{
unsigned long len1=Cyc_strlen(s1);
unsigned long len2=Cyc_strlen(s2);
return Cyc_casecmp(s1,len1,s2,len2);}}
# 698
inline static int Cyc_caseless_ncmp(struct _fat_ptr s1,unsigned long len1,struct _fat_ptr s2,unsigned long len2,unsigned long n){
# 702
if(n <= 0U)return 0;{
# 704
unsigned long min_len=Cyc_umin(len1,len2);
unsigned long bound=Cyc_umin(min_len,n);
# 707
1;
# 709
{int i=0;for(0;(unsigned long)i < bound;++ i){
int retc;
if((retc=({int _Tmp0=toupper((int)((const char*)s1.curr)[i]);_Tmp0 - toupper((int)((const char*)s2.curr)[i]);}))!=0)
return retc;}}
# 714
if(len1 < n || len2 < n)
return(int)len1 - (int)len2;
return 0;}}
# 720
int Cyc_strncasecmp(struct _fat_ptr s1,struct _fat_ptr s2,unsigned long n){
unsigned long len1=Cyc_strlen(s1);
unsigned long len2=Cyc_strlen(s2);
return Cyc_caseless_ncmp(s1,len1,s2,len2,n);}
# 729
extern void*memcpy(void*,const void*,unsigned long);
extern void*memmove(void*,const void*,unsigned long);
extern int memcmp(const void*,const void*,unsigned long);
extern char*memchr(const char*,char,unsigned long);
extern void*memset(void*,int,unsigned long);
extern void bcopy(const void*,void*,unsigned long);
extern void bzero(void*,unsigned long);
extern char*GC_realloc_hint(char*,unsigned,unsigned);
# 742
struct _fat_ptr Cyc_realloc(struct _fat_ptr s,unsigned long n){
unsigned _Tmp0=_get_fat_size(s,sizeof(char));unsigned _Tmp1;_Tmp1=_Tmp0;{unsigned oldsz=_Tmp1;
unsigned _Tmp2;_Tmp2=n;{unsigned l=_Tmp2;
char*res=GC_realloc_hint((char*)_untag_fat_ptr(s,sizeof(char),1U),oldsz,l);
return({(struct _fat_ptr(*)(char*,unsigned,unsigned))Cyc_Core_mkfat;})(_check_null(res),sizeof(char),l);}}}
# 749
struct _fat_ptr Cyc__memcpy(struct _fat_ptr d,struct _fat_ptr s,unsigned long n,unsigned sz){
if((((void*)d.curr==(void*)_tag_fat(0,0,0).curr || _get_fat_size(d,sizeof(void))< n)||(void*)s.curr==(void*)_tag_fat(0,0,0).curr)|| _get_fat_size(s,sizeof(void))< n)
(void*)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_Tmp0->tag=Cyc_Core_Invalid_argument,_Tmp0->f1=({const char*_Tmp1="memcpy";_tag_fat(_Tmp1,sizeof(char),7U);});_Tmp0;}));
({void*_Tmp0=(void*)_untag_fat_ptr(d,sizeof(void),1U);const void*_Tmp1=(const void*)_untag_fat_ptr(s,sizeof(void),1U);memcpy(_Tmp0,_Tmp1,n * sz);});
return d;}
# 756
struct _fat_ptr Cyc__memmove(struct _fat_ptr d,struct _fat_ptr s,unsigned long n,unsigned sz){
if((((void*)d.curr==(void*)_tag_fat(0,0,0).curr || _get_fat_size(d,sizeof(void))< n)||(void*)s.curr==(void*)_tag_fat(0,0,0).curr)|| _get_fat_size(s,sizeof(void))< n)
(void*)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_Tmp0->tag=Cyc_Core_Invalid_argument,_Tmp0->f1=({const char*_Tmp1="memove";_tag_fat(_Tmp1,sizeof(char),7U);});_Tmp0;}));
({void*_Tmp0=(void*)_untag_fat_ptr(d,sizeof(void),1U);const void*_Tmp1=(const void*)_untag_fat_ptr(s,sizeof(void),1U);memmove(_Tmp0,_Tmp1,n * sz);});
return d;}
# 763
int Cyc_memcmp(struct _fat_ptr s1,struct _fat_ptr s2,unsigned long n){
if((((char*)s1.curr==(char*)_tag_fat(0,0,0).curr ||(char*)s2.curr==(char*)_tag_fat(0,0,0).curr)|| _get_fat_size(s1,sizeof(char))< n)|| _get_fat_size(s2,sizeof(char))< n)
(void*)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_Tmp0->tag=Cyc_Core_Invalid_argument,_Tmp0->f1=({const char*_Tmp1="memcmp";_tag_fat(_Tmp1,sizeof(char),7U);});_Tmp0;}));
return({const void*_Tmp0=(const void*)_untag_fat_ptr(s1,sizeof(char),1U);const void*_Tmp1=(const void*)_untag_fat_ptr(s2,sizeof(char),1U);memcmp(_Tmp0,_Tmp1,n);});}
# 769
struct _fat_ptr Cyc_memchr(struct _fat_ptr s,char c,unsigned long n){
unsigned sz=_get_fat_size(s,sizeof(char));
if((char*)s.curr==(char*)_tag_fat(0,0,0).curr || n > sz)
(void*)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_Tmp0->tag=Cyc_Core_Invalid_argument,_Tmp0->f1=({const char*_Tmp1="memchr";_tag_fat(_Tmp1,sizeof(char),7U);});_Tmp0;}));{
char*p=memchr((const char*)_untag_fat_ptr(s,sizeof(char),1U),c,n);
if(p==0)return _tag_fat(0,0,0);{
unsigned sval=(unsigned)((const char*)_untag_fat_ptr(s,sizeof(char),1U));
unsigned pval=(unsigned)((const char*)p);
unsigned delta=pval - sval;
return _fat_ptr_plus(s,sizeof(char),(int)delta);}}}
# 781
struct _fat_ptr Cyc_mmemchr(struct _fat_ptr s,char c,unsigned long n){
unsigned sz=_get_fat_size(s,sizeof(char));
if((char*)s.curr==(char*)_tag_fat(0,0,0).curr || n > sz)
(void*)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_Tmp0->tag=Cyc_Core_Invalid_argument,_Tmp0->f1=({const char*_Tmp1="mmemchr";_tag_fat(_Tmp1,sizeof(char),8U);});_Tmp0;}));{
char*p=memchr((const char*)_untag_fat_ptr(s,sizeof(char),1U),c,n);
if(p==0)return _tag_fat(0,0,0);{
unsigned sval=(unsigned)((const char*)_untag_fat_ptr(s,sizeof(char),1U));
unsigned pval=(unsigned)p;
unsigned delta=pval - sval;
return _fat_ptr_plus(s,sizeof(char),(int)delta);}}}
# 793
struct _fat_ptr Cyc_memset(struct _fat_ptr s,char c,unsigned long n){
if((char*)s.curr==(char*)_tag_fat(0,0,0).curr || n > _get_fat_size(s,sizeof(char)))
(void*)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_Tmp0->tag=Cyc_Core_Invalid_argument,_Tmp0->f1=({const char*_Tmp1="memset";_tag_fat(_Tmp1,sizeof(char),7U);});_Tmp0;}));
memset((void*)((char*)_untag_fat_ptr(s,sizeof(char),1U)),(int)c,n);
return s;}
# 800
void Cyc_bzero(struct _fat_ptr s,unsigned long n){
if((char*)s.curr==(char*)_tag_fat(0,0,0).curr || _get_fat_size(s,sizeof(char))< n)
(void*)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_Tmp0->tag=Cyc_Core_Invalid_argument,_Tmp0->f1=({const char*_Tmp1="bzero";_tag_fat(_Tmp1,sizeof(char),6U);});_Tmp0;}));
({(void(*)(char*,unsigned long))bzero;})((char*)_untag_fat_ptr(s,sizeof(char),1U),n);}
# 806
void Cyc__bcopy(struct _fat_ptr src,struct _fat_ptr dst,unsigned long n,unsigned sz){
if((((void*)src.curr==(void*)_tag_fat(0,0,0).curr || _get_fat_size(src,sizeof(void))< n)||(void*)dst.curr==(void*)_tag_fat(0,0,0).curr)|| _get_fat_size(dst,sizeof(void))< n)
(void*)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_Tmp0->tag=Cyc_Core_Invalid_argument,_Tmp0->f1=({const char*_Tmp1="bcopy";_tag_fat(_Tmp1,sizeof(char),6U);});_Tmp0;}));
({const void*_Tmp0=(const void*)_untag_fat_ptr(src,sizeof(void),1U);void*_Tmp1=(void*)_untag_fat_ptr(dst,sizeof(void),1U);bcopy(_Tmp0,_Tmp1,n * sz);});}
