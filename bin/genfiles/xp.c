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
typedef unsigned char _AliasQualHandle_t;

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

# 4 "ctype.h"
 extern int isalnum(int);
# 22
extern int isspace(int);
# 282 "cycboot.h"
extern int isalnum(int);
# 300
extern int isspace(int);
# 81 "string.h"
extern struct _fat_ptr Cyc__memcpy(struct _fat_ptr,struct _fat_ptr,unsigned long,unsigned);
# 86
extern struct _fat_ptr Cyc_memset(struct _fat_ptr,char,unsigned long);
# 29 "assert.h"
extern void*Cyc___assert_fail(struct _fat_ptr,struct _fat_ptr,unsigned);
# 15 "xp.h"
int Cyc_XP_quotient(int,struct _fat_ptr,struct _fat_ptr,int);static char _TmpG0[37U]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
# 7 "xp.cyc"
static struct _fat_ptr Cyc_digits={_TmpG0,_TmpG0,_TmpG0 + 37U};
static char Cyc_map[75U]={'\000','\001','\002','\003','\004','\005','\006','\a','\b','\t','$','$','$','$','$','$','$','\n','\v','\f','\r','\016','\017','\020','\021','\022','\023','\024','\025','\026','\027','\030','\031','\032','\033','\034','\035','\036','\037',' ','!','"','#','$','$','$','$','$','$','\n','\v','\f','\r','\016','\017','\020','\021','\022','\023','\024','\025','\026','\027','\030','\031','\032','\033','\034','\035','\036','\037',' ','!','"','#'};
# 17
unsigned long Cyc_XP_fromint(int n,struct _fat_ptr z,unsigned long u){
int i=0;
do{
*((unsigned char*)_check_fat_subscript(z,sizeof(unsigned char),i ++))=(unsigned char)(u % 256U);}while(
(u /=256U)> 0U && i < n);
for(1;i < n;++ i){
*((unsigned char*)_check_fat_subscript(z,sizeof(unsigned char),i))='\000';}
return u;}
# 26
unsigned long Cyc_XP_toint(int n,struct _fat_ptr x){
unsigned long u=0U;
int i=(int)sizeof(u);
if(i > n)
i=n;
while(-- i >= 0){
u=256U * u + (unsigned long)*((unsigned char*)_check_fat_subscript(x,sizeof(unsigned char),i));}
return u;}
# 35
int Cyc_XP_length(int n,struct _fat_ptr x){
while(n > 1 &&(int)*((unsigned char*)_check_fat_subscript(x,sizeof(unsigned char),n - 1))==0){
-- n;}
return n;}
# 40
int Cyc_XP_add(int n,struct _fat_ptr z,struct _fat_ptr x,struct _fat_ptr y,int carry){
int i;
for(i=0;i < n;++ i){
({int _Tmp0=({int _Tmp1=(int)*((unsigned char*)_check_fat_subscript(x,sizeof(unsigned char),i));_Tmp1 + (int)*((unsigned char*)_check_fat_subscript(y,sizeof(unsigned char),i));});carry +=_Tmp0;});
*((unsigned char*)_check_fat_subscript(z,sizeof(unsigned char),i))=(unsigned char)(carry % 256U);
carry /=256U;}
# 47
return carry;}
# 49
int Cyc_XP_sub(int n,struct _fat_ptr z,struct _fat_ptr x,struct _fat_ptr y,int borrow){
int i;
for(i=0;i < n;++ i){
int d=({int _Tmp0=((int)*((unsigned char*)_check_fat_subscript(x,sizeof(unsigned char),i))+ 256U)- borrow;_Tmp0 - (int)*((unsigned char*)_check_fat_subscript(y,sizeof(unsigned char),i));});
*((unsigned char*)_check_fat_subscript(z,sizeof(unsigned char),i))=(unsigned char)(d % 256U);
borrow=1 - d / 256U;}
# 56
return borrow;}
# 58
int Cyc_XP_sum(int n,struct _fat_ptr z,struct _fat_ptr x,int y){
int i;
for(i=0;i < n;++ i){
y +=(int)*((unsigned char*)_check_fat_subscript(x,sizeof(unsigned char),i));
*((unsigned char*)_check_fat_subscript(z,sizeof(unsigned char),i))=(unsigned char)(y % 256U);
y /=256U;}
# 65
return y;}
# 67
int Cyc_XP_diff(int n,struct _fat_ptr z,struct _fat_ptr x,int y){
int i;
for(i=0;i < n;++ i){
int d=((int)*((unsigned char*)_check_fat_subscript(x,sizeof(unsigned char),i))+ 256U)- y;
*((unsigned char*)_check_fat_subscript(z,sizeof(unsigned char),i))=(unsigned char)(d % 256U);
y=1 - d / 256U;}
# 74
return y;}
# 76
int Cyc_XP_neg(int n,struct _fat_ptr z,struct _fat_ptr x,int carry){
int i;
for(i=0;i < n;++ i){
carry +=(int)~(*((unsigned char*)_check_fat_subscript(x,sizeof(unsigned char),i)));
*((unsigned char*)_check_fat_subscript(z,sizeof(unsigned char),i))=(unsigned char)(carry % 256U);
carry /=256U;}
# 83
return carry;}
# 85
int Cyc_XP_mul(struct _fat_ptr z,int n,struct _fat_ptr x,int m,struct _fat_ptr y){
int i;int j;int carryout=0;
for(i=0;i < n;++ i){
unsigned carry=0U;
for(j=0;j < m;++ j){
({unsigned _Tmp0=(unsigned)({int _Tmp1=({int _Tmp2=(int)*((unsigned char*)_check_fat_subscript(x,sizeof(unsigned char),i));_Tmp2 * (int)*((unsigned char*)_check_fat_subscript(y,sizeof(unsigned char),j));});_Tmp1 + (int)*((unsigned char*)_check_fat_subscript(z,sizeof(unsigned char),i + j));});carry +=_Tmp0;});
((unsigned char*)z.curr)[i + j]=(unsigned char)(carry % 256U);
carry /=256U;}
# 94
for(1;j < (n + m)- i;++ j){
carry +=(unsigned)*((unsigned char*)_check_fat_subscript(z,sizeof(unsigned char),i + j));
((unsigned char*)z.curr)[i + j]=(unsigned char)(carry % 256U);
carry /=256U;}
# 99
carryout |=carry;}
# 101
return carryout;}
# 103
int Cyc_XP_product(int n,struct _fat_ptr z,struct _fat_ptr x,int y){
int i;
unsigned carry=0U;
for(i=0;i < n;++ i){
carry +=(unsigned)((int)*((unsigned char*)_check_fat_subscript(x,sizeof(unsigned char),i))* y);
*((unsigned char*)_check_fat_subscript(z,sizeof(unsigned char),i))=(unsigned char)(carry % 256U);
carry /=256U;}
# 111
return(int)carry;}
# 113
int Cyc_XP_div(int n,struct _fat_ptr q,struct _fat_ptr x,int m,struct _fat_ptr y,struct _fat_ptr r,struct _fat_ptr tmp){
int nx=n;int my=m;
n=Cyc_XP_length(n,x);
m=Cyc_XP_length(m,y);
if(m==1){
if((int)*((unsigned char*)_check_fat_subscript(y,sizeof(unsigned char),0))==0)
return 0;
({unsigned char _Tmp0=(unsigned char)Cyc_XP_quotient(nx,q,x,(int)((unsigned char*)y.curr)[0]);*((unsigned char*)_check_fat_subscript(r,sizeof(unsigned char),0))=_Tmp0;});
if(_get_fat_size(r,sizeof(unsigned char))> 1U)
({struct _fat_ptr _Tmp0=_fat_ptr_plus((struct _fat_ptr)r,sizeof(char),1);Cyc_memset(_Tmp0,'\000',(unsigned long)(my - 1));});}else{
if(m > n){
Cyc_memset(q,'\000',(unsigned long)nx);
Cyc__memcpy(r,x,(unsigned)n / sizeof(*((unsigned char*)x.curr))+ (unsigned)((unsigned)n % sizeof(*((unsigned char*)x.curr))==0U?0: 1),sizeof(*((unsigned char*)x.curr)));
({struct _fat_ptr _Tmp0=_fat_ptr_plus((struct _fat_ptr)r,sizeof(char),n);Cyc_memset(_Tmp0,'\000',(unsigned long)(my - n));});}else{
# 128
int k;
struct _fat_ptr rem=tmp;struct _fat_ptr dq=_fat_ptr_plus(_fat_ptr_plus(tmp,sizeof(unsigned char),n),sizeof(unsigned char),1);
2 <= m && m <= n?0:({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;})(_tag_fat("2 <= m && m <= n",sizeof(char),17U),_tag_fat("xp.cyc",sizeof(char),7U),130U);
Cyc__memcpy(rem,x,(unsigned)n / sizeof(*((unsigned char*)x.curr))+ (unsigned)((unsigned)n % sizeof(*((unsigned char*)x.curr))==0U?0: 1),sizeof(*((unsigned char*)x.curr)));
*((unsigned char*)_check_fat_subscript(rem,sizeof(unsigned char),n))='\000';
for(k=n - m;k >= 0;-- k){
int qk;
{
int i;
(2 <= m && m <= k + m)&& k + m <= n?0:({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;})(_tag_fat("2 <= m && m <= k+m && k+m <= n",sizeof(char),31U),_tag_fat("xp.cyc",sizeof(char),7U),137U);
{
int km=k + m;
unsigned long y2=(unsigned long)((int)*((unsigned char*)_check_fat_subscript(y,sizeof(unsigned char),m - 1))* 256U + (int)((unsigned char*)y.curr)[m - 2]);
unsigned long r3=(unsigned long)({int _Tmp0=({int _Tmp1=(int)*((unsigned char*)_check_fat_subscript(rem,sizeof(unsigned char),km))* 65536U;_Tmp1 + (int)*((unsigned char*)_check_fat_subscript(rem,sizeof(unsigned char),km - 1))* 256U;});_Tmp0 + (int)*((unsigned char*)_check_fat_subscript(rem,sizeof(unsigned char),km - 2));});
# 143
qk=(int)(r3 / y2);
if(qk >= 256U)
qk=255U;}
# 147
({unsigned char _Tmp0=(unsigned char)Cyc_XP_product(m,dq,y,qk);*((unsigned char*)_check_fat_subscript(dq,sizeof(unsigned char),m))=_Tmp0;});
for(i=m;i > 0;-- i){
if(({int _Tmp0=(int)*((unsigned char*)_check_fat_subscript(rem,sizeof(unsigned char),i + k));_Tmp0!=(int)*((unsigned char*)_check_fat_subscript(dq,sizeof(unsigned char),i));}))
break;}
if(({int _Tmp0=(int)*((unsigned char*)_check_fat_subscript(rem,sizeof(unsigned char),i + k));_Tmp0 < (int)*((unsigned char*)_check_fat_subscript(dq,sizeof(unsigned char),i));}))
({unsigned char _Tmp0=(unsigned char)Cyc_XP_product(m,dq,y,-- qk);((unsigned char*)dq.curr)[m]=_Tmp0;});}
# 154
*((unsigned char*)_check_fat_subscript(q,sizeof(unsigned char),k))=(unsigned char)qk;{
# 156
int borrow;
0 <= k && k <= k + m?0:({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;})(_tag_fat("0 <= k && k <= k+m",sizeof(char),19U),_tag_fat("xp.cyc",sizeof(char),7U),157U);
borrow=({int _Tmp0=m + 1;struct _fat_ptr _Tmp1=_fat_ptr_plus(rem,sizeof(unsigned char),k);struct _fat_ptr _Tmp2=_fat_ptr_plus(rem,sizeof(unsigned char),k);Cyc_XP_sub(_Tmp0,_Tmp1,_Tmp2,dq,0);});
borrow==0?0:({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;})(_tag_fat("borrow == 0",sizeof(char),12U),_tag_fat("xp.cyc",sizeof(char),7U),159U);}}
# 162
Cyc__memcpy(r,rem,(unsigned)m / sizeof(*((unsigned char*)rem.curr))+ (unsigned)((unsigned)m % sizeof(*((unsigned char*)rem.curr))==0U?0: 1),sizeof(*((unsigned char*)rem.curr)));{
# 164
int i;
for(i=(n - m)+ 1;i < nx;++ i){
*((unsigned char*)_check_fat_subscript(q,sizeof(unsigned char),i))='\000';}
for(i=m;i < my;++ i){
*((unsigned char*)_check_fat_subscript(r,sizeof(unsigned char),i))='\000';}}}}
# 171
return 1;}
# 173
int Cyc_XP_quotient(int n,struct _fat_ptr z,struct _fat_ptr x,int y){
int i;
unsigned carry=0U;
for(i=n - 1;i >= 0;-- i){
carry=carry * 256U + (unsigned)*((unsigned char*)_check_fat_subscript(x,sizeof(unsigned char),i));
*((unsigned char*)_check_fat_subscript(z,sizeof(unsigned char),i))=(unsigned char)(carry / (unsigned)y);
carry %=(unsigned)y;}
# 181
return(int)carry;}
# 183
int Cyc_XP_cmp(int n,struct _fat_ptr x,struct _fat_ptr y){
int i=n - 1;
while(i > 0 &&({int _Tmp0=(int)*((unsigned char*)_check_fat_subscript(x,sizeof(unsigned char),i));_Tmp0==(int)*((unsigned char*)_check_fat_subscript(y,sizeof(unsigned char),i));})){
-- i;}
return({int _Tmp0=(int)*((unsigned char*)_check_fat_subscript(x,sizeof(unsigned char),i));_Tmp0 - (int)*((unsigned char*)_check_fat_subscript(y,sizeof(unsigned char),i));});}
# 189
void Cyc_XP_lshift(int n,struct _fat_ptr z,int m,struct _fat_ptr x,int s,int fill){
fill=fill?255: 0;
{
int i;int j=n - 1;
if(n > m)
i=m - 1;else{
# 196
i=(n - s / 8)- 1;}
for(1;j >= m + s / 8;-- j){
*((unsigned char*)_check_fat_subscript(z,sizeof(unsigned char),j))='\000';}
for(1;i >= 0;(i --,j --)){
({unsigned char _Tmp0=*((unsigned char*)_check_fat_subscript(x,sizeof(unsigned char),i));*((unsigned char*)_check_fat_subscript(z,sizeof(unsigned char),j))=_Tmp0;});}
for(1;j >= 0;-- j){
*((unsigned char*)_check_fat_subscript(z,sizeof(unsigned char),j))=(unsigned char)fill;}}
# 204
s %=8;
if(s > 0){
# 207
Cyc_XP_product(n,z,z,1 << s);
*((unsigned char*)_check_fat_subscript(z,sizeof(unsigned char),0))|=fill >> 8 - s;}}
# 211
void Cyc_XP_rshift(int n,struct _fat_ptr z,int m,struct _fat_ptr x,int s,int fill){
fill=fill?255: 0;
{
int i;int j=0;
for(i=s / 8;i < m && j < n;(i ++,j ++)){
({unsigned char _Tmp0=*((unsigned char*)_check_fat_subscript(x,sizeof(unsigned char),i));*((unsigned char*)_check_fat_subscript(z,sizeof(unsigned char),j))=_Tmp0;});}
for(1;j < n;++ j){
*((unsigned char*)_check_fat_subscript(z,sizeof(unsigned char),j))=(unsigned char)fill;}}
# 220
s %=8;
if(s > 0){
# 223
Cyc_XP_quotient(n,z,z,1 << s);
*((unsigned char*)_check_fat_subscript(z,sizeof(unsigned char),n - 1))|=fill << 8 - s;}}
# 227
void Cyc_XP_and(int n,struct _fat_ptr z,struct _fat_ptr x,struct _fat_ptr y){
int i;
for(i=0;i < n;++ i){
({unsigned char _Tmp0=(unsigned char)({int _Tmp1=(int)*((unsigned char*)_check_fat_subscript(x,sizeof(unsigned char),i));_Tmp1 & (int)*((unsigned char*)_check_fat_subscript(y,sizeof(unsigned char),i));});*((unsigned char*)_check_fat_subscript(z,sizeof(unsigned char),i))=_Tmp0;});}}
# 232
void Cyc_XP_or(int n,struct _fat_ptr z,struct _fat_ptr x,struct _fat_ptr y){
int i;
for(i=0;i < n;++ i){
({unsigned char _Tmp0=(unsigned char)({int _Tmp1=(int)*((unsigned char*)_check_fat_subscript(x,sizeof(unsigned char),i));_Tmp1 | (int)*((unsigned char*)_check_fat_subscript(y,sizeof(unsigned char),i));});*((unsigned char*)_check_fat_subscript(z,sizeof(unsigned char),i))=_Tmp0;});}}
# 237
void Cyc_XP_xor(int n,struct _fat_ptr z,struct _fat_ptr x,struct _fat_ptr y){
int i;
for(i=0;i < n;++ i){
({unsigned char _Tmp0=(unsigned char)({int _Tmp1=(int)*((unsigned char*)_check_fat_subscript(x,sizeof(unsigned char),i));_Tmp1 ^ (int)*((unsigned char*)_check_fat_subscript(y,sizeof(unsigned char),i));});*((unsigned char*)_check_fat_subscript(z,sizeof(unsigned char),i))=_Tmp0;});}}
# 242
void Cyc_XP_not(int n,struct _fat_ptr z,struct _fat_ptr x){
int i;
for(i=0;i < n;++ i){
({unsigned char _Tmp0=~(*((unsigned char*)_check_fat_subscript(x,sizeof(unsigned char),i)));*((unsigned char*)_check_fat_subscript(z,sizeof(unsigned char),i))=_Tmp0;});}}
# 247
int Cyc_XP_fromstr(int n,struct _fat_ptr z,const char*str,int base){
# 249
const char*p=str;
(unsigned)p?0:({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;})(_tag_fat("p",sizeof(char),2U),_tag_fat("xp.cyc",sizeof(char),7U),250U);
base >= 2 && base <= 36?0:({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;})(_tag_fat("base >= 2 && base <= 36",sizeof(char),24U),_tag_fat("xp.cyc",sizeof(char),7U),251U);
while((int)*_check_null(p)&& isspace((int)*p)){
({const char**_Tmp0=& p;if(*(*_Tmp0)!=0)++(*_Tmp0);else{_throw_arraybounds();}*_Tmp0;});}
if(((int)*p && isalnum((int)*p))&&(int)*((char*)_check_known_subscript_notnull(Cyc_map,75U,sizeof(char),(int)*p - 48))< base){
int carry;
for(1;((int)*_check_null(p)&& isalnum((int)*p))&&(int)*((char*)_check_known_subscript_notnull(Cyc_map,75U,sizeof(char),(int)*p - 48))< base;({const char**_Tmp0=& p;if(*(*_Tmp0)!=0)++(*_Tmp0);else{_throw_arraybounds();}*_Tmp0;})){
carry=Cyc_XP_product(n,z,z,base);
if(carry)
break;
Cyc_XP_sum(n,z,z,(int)*((char*)_check_known_subscript_notnull(Cyc_map,75U,sizeof(char),(int)*p - 48)));}
# 262
return carry;}else{
# 264
return 0;}}
# 267
struct _fat_ptr Cyc_XP_tostr(struct _fat_ptr str,int size,int base,int n,struct _fat_ptr x){
# 269
int i=0;
(unsigned)str.curr?0:({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;})(_tag_fat("str",sizeof(char),4U),_tag_fat("xp.cyc",sizeof(char),7U),270U);
base >= 2 && base <= 36?0:({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;})(_tag_fat("base >= 2 && base <= 36",sizeof(char),24U),_tag_fat("xp.cyc",sizeof(char),7U),271U);
do{
int r=Cyc_XP_quotient(n,x,x,base);
i < size?0:({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;})(_tag_fat("i < size",sizeof(char),9U),_tag_fat("xp.cyc",sizeof(char),7U),274U);
({struct _fat_ptr _Tmp0=_fat_ptr_plus(str,sizeof(char),i ++);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=*((const char*)_check_fat_subscript(Cyc_digits,sizeof(char),r));if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
while(n > 1 &&(int)*((unsigned char*)_check_fat_subscript(x,sizeof(unsigned char),n - 1))==0){
-- n;}}while(
n > 1 ||(int)*((unsigned char*)_check_fat_subscript(x,sizeof(unsigned char),0))!=0);
i < size?0:({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;})(_tag_fat("i < size",sizeof(char),9U),_tag_fat("xp.cyc",sizeof(char),7U),279U);
({struct _fat_ptr _Tmp0=_fat_ptr_plus(str,sizeof(char),i);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='\000';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
{
int j;
for(j=0;j < -- i;++ j){
char c=*((char*)_check_fat_subscript(str,sizeof(char),j));
({struct _fat_ptr _Tmp0=_fat_ptr_plus(str,sizeof(char),j);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=((char*)str.curr)[i];if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(str,sizeof(char),i);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=c;if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}}
# 289
return str;}
