// This is a C header file to be used by the output of the Cyclone
// to C translator.  The corresponding definitions are in file lib/runtime_cyc.c
#ifndef _CYC_INCLUDE_H_
#define _CYC_INCLUDE_H_

#include <setjmp.h>

#ifdef NO_CYC_PREFIX
#define ADD_PREFIX(x) x
#else
#define ADD_PREFIX(x) Cyc_##x
#endif

#ifndef offsetof
// should be size_t, but int is fine.
#define offsetof(t,n) ((int)(&(((t *)0)->n)))
#endif

//// Tagged arrays
struct _tagged_arr { 
  unsigned char *curr; 
  unsigned char *base; 
  unsigned char *last_plus_one; 
};

//// Discriminated Unions
struct _xtunion_struct { char *tag; };

// Need one of these per thread (we don't have threads)
// The runtime maintains a stack that contains either _handler_cons
// structs or _RegionHandle structs.  The tag is 0 for a handler_cons
// and 1 for a region handle.  
struct _RuntimeStack {
  int tag; // 0 for an exception handler, 1 for a region handle
  struct _RuntimeStack *next;
};

//// Regions
struct _RegionPage {
#ifdef CYC_REGION_PROFILE
  unsigned total_bytes;
  unsigned free_bytes;
#endif
  struct _RegionPage *next;
  char data[0];
};

struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
#ifdef CYC_REGION_PROFILE
  const char         *name;
#endif
};

extern struct _RegionHandle _new_region(const char *);
extern void * _region_malloc(struct _RegionHandle *, unsigned);
extern void * _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
extern void   _free_region(struct _RegionHandle *);
extern void   _reset_region(struct _RegionHandle *);

//// Exceptions 
struct _handler_cons {
  struct _RuntimeStack s;
  jmp_buf handler;
};
extern void _push_handler(struct _handler_cons *);
extern void _push_region(struct _RegionHandle *);
extern void _npop_handler(int);
extern void _pop_handler();
extern void _pop_region();

#ifndef _throw
extern int _throw_null();
extern int _throw_arraybounds();
extern int _throw_badalloc();
extern int _throw(void* e);
#endif

extern struct _xtunion_struct *_exn_thrown;

//// Built-in Exceptions
extern struct _xtunion_struct ADD_PREFIX(Null_Exception_struct);
extern struct _xtunion_struct * ADD_PREFIX(Null_Exception);
extern struct _xtunion_struct ADD_PREFIX(Array_bounds_struct);
extern struct _xtunion_struct * ADD_PREFIX(Array_bounds);
extern struct _xtunion_struct ADD_PREFIX(Match_Exception_struct);
extern struct _xtunion_struct * ADD_PREFIX(Match_Exception);
extern struct _xtunion_struct ADD_PREFIX(Bad_alloc_struct);
extern struct _xtunion_struct * ADD_PREFIX(Bad_alloc);

//// Built-in Run-time Checks and company
#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#define _check_null(ptr) \
  ({ void*_check_null_temp = (void*)(ptr); \
     if (!_check_null_temp) _throw_null(); \
     _check_null_temp; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  ((char *)ptr) + (elt_sz)*(index); })
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  void*_cks_ptr = (void*)(ptr); \
  unsigned _cks_bound = (bound); \
  unsigned _cks_elt_sz = (elt_sz); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (_cks_index >= _cks_bound) _throw_arraybounds(); \
  ((char *)_cks_ptr) + _cks_elt_sz*_cks_index; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_notnull(bound,index) (index)
#else
#define _check_known_subscript_notnull(bound,index) ({ \
  unsigned _cksnn_bound = (bound); \
  unsigned _cksnn_index = (index); \
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); \
  _cksnn_index; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_unknown_subscript(arr,elt_sz,index) ({ \
  struct _tagged_arr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  _cus_ans; })
#else
#define _check_unknown_subscript(arr,elt_sz,index) ({ \
  struct _tagged_arr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  if (!_cus_arr.base) _throw_null(); \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#endif

#define _tag_arr(tcurr,elt_sz,num_elts) ({ \
  struct _tagged_arr _tag_arr_ans; \
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })

#define _init_tag_arr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _tagged_arr *_itarr_ptr = (arr_ptr); \
  void* _itarr = (arr); \
  _itarr_ptr->base = _itarr_ptr->curr = _itarr; \
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts); \
  _itarr_ptr; })

#ifdef NO_CYC_BOUNDS_CHECKS
#define _untag_arr(arr,elt_sz,num_elts) ((arr).curr)
#else
#define _untag_arr(arr,elt_sz,num_elts) ({ \
  struct _tagged_arr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)\
    _throw_arraybounds(); \
  _curr; })
#endif

#define _get_arr_size(arr,elt_sz) \
  ({struct _tagged_arr _get_arr_size_temp = (arr); \
    unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr; \
    unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one; \
    (_get_arr_size_curr < _get_arr_size_temp.base || \
     _get_arr_size_curr >= _get_arr_size_last) ? 0 : \
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));})

#define _tagged_arr_plus(arr,elt_sz,change) ({ \
  struct _tagged_arr _ans = (arr); \
  _ans.curr += ((int)(elt_sz))*(change); \
  _ans; })

#define _tagged_arr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _tagged_arr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  *_arr_ptr; })

#define _tagged_arr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _tagged_arr * _arr_ptr = (arr_ptr); \
  struct _tagged_arr _ans = *_arr_ptr; \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  _ans; })

// Decrease the upper bound on a fat pointer by numelts where sz is
// the size of the pointer's type.  Note that this can't be a macro
// if we're to get initializers right.
static struct _tagged_arr _tagged_ptr_decrease_size(struct _tagged_arr x,
                                                    unsigned int sz,
                                                    unsigned int numelts) {
  x.last_plus_one -= sz * numelts; 
  return x; 
}

// Add i to zero-terminated pointer x.  Checks for x being null and
// ensures that x[0..i-1] are not 0.
#ifdef NO_CYC_BOUNDS_CHECK
#define _zero_arr_plus(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#else
#define _zero_arr_plus(orig_x,orig_sz,orig_i) ({ \
  typedef _czs_tx = (*orig_x); \
  _czs_tx *_czs_x = (_czs_tx *)(orig_x); \
  unsigned int _czs_sz = (orig_sz); \
  int _czs_i = (orig_i); \
  unsigned int _czs_temp; \
  if ((_czs_x) == 0) _throw_null(); \
  if (_czs_i < 0) _throw_arraybounds(); \
  for (_czs_temp=_czs_sz; _czs_temp < _czs_i; _czs_temp++) \
    if (_czs_x[_czs_temp] == 0) _throw_arraybounds(); \
  _czs_x+_czs_i; })
#endif

// Calculates the number of elements in a zero-terminated, thin array.
// If non-null, the array is guaranteed to have orig_offset elements.
#define _get_zero_arr_size(orig_x,orig_offset) ({ \
  typedef _gres_tx = (*orig_x); \
  _gres_tx *_gres_x = (_gres_tx *)(orig_x); \
  unsigned int _gres_offset = (orig_offset); \
  unsigned int _gres = 0; \
  if (_gres_x != 0) { \
     _gres = _gres_offset; \
     _gres_x += _gres_offset - 1; \
     while (*_gres_x != 0) { _gres_x++; _gres++; } \
  } _gres; })

// Does in-place addition of a zero-terminated pointer (x += e and ++x).  
// Note that this expands to call _zero_arr_plus.
#define _zero_arr_inplace_plus(x,orig_i) ({ \
  typedef _zap_tx = (*x); \
  _zap_tx **_zap_x = &((_zap_tx*)x); \
  *_zap_x = _zero_arr_plus(*_zap_x,1,(orig_i)); })

// Does in-place increment of a zero-terminated pointer (e.g., x++).
// Note that this expands to call _zero_arr_plus.
#define _zero_arr_inplace_plus_post(x,orig_i) ({ \
  typedef _zap_tx = (*x); \
  _zap_tx **_zap_x = &((_zap_tx*)x); \
  _zap_tx *_zap_res = *_zap_x; \
  *_zap_x = _zero_arr_plus(_zap_res,1,(orig_i)); \
  _zap_res; })
  
//// Allocation
extern void* GC_malloc(int);
extern void* GC_malloc_atomic(int);
extern void* GC_calloc(unsigned,unsigned);
extern void* GC_calloc_atomic(unsigned,unsigned);

static inline void* _cycalloc(int n) {
  void * ans = (void *)GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void* _cycalloc_atomic(int n) {
  void * ans = (void *)GC_malloc_atomic(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void* _cyccalloc(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
static inline void* _cyccalloc_atomic(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc_atomic(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
#define MAX_MALLOC_SIZE (1 << 28)
static inline unsigned int _check_times(unsigned x, unsigned y) {
  unsigned long long whole_ans = 
    ((unsigned long long)x)*((unsigned long long)y);
  unsigned word_ans = (unsigned)whole_ans;
  if(word_ans < whole_ans || word_ans > MAX_MALLOC_SIZE)
    _throw_badalloc();
  return word_ans;
}

#if defined(CYC_REGION_PROFILE) 
extern void* _profile_GC_malloc(int,char *file,int lineno);
extern void* _profile_GC_malloc_atomic(int,char *file,int lineno);
extern void* _profile_region_malloc(struct _RegionHandle *, unsigned,
                                     char *file,int lineno);
extern struct _RegionHandle _profile_new_region(const char *rgn_name,
						char *file,int lineno);
extern void _profile_free_region(struct _RegionHandle *,
				 char *file,int lineno);
#  if !defined(RUNTIME_CYC)
#define _new_region(n) _profile_new_region(n,__FILE__ ":" __FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__ ":" __FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__ ":" __FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__ ":" __FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__ ":" __FUNCTION__,__LINE__)
#endif
#endif
 struct Cyc_Core_Opt{void*v;};struct _tagged_arr Cyc_Core_new_string(unsigned int);
struct _tagged_arr Cyc_Core_rnew_string(struct _RegionHandle*,unsigned int);extern
char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{char*
tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;struct _tagged_arr wrap_Cstring_as_string(
char*,unsigned int);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(
struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];extern char Cyc_List_Nth[
8];int toupper(int);char*strerror(int errnum);unsigned int Cyc_strlen(struct
_tagged_arr s);int Cyc_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);int Cyc_strptrcmp(
struct _tagged_arr*s1,struct _tagged_arr*s2);int Cyc_strncmp(struct _tagged_arr s1,
struct _tagged_arr s2,unsigned int len);int Cyc_zstrcmp(struct _tagged_arr,struct
_tagged_arr);int Cyc_zstrncmp(struct _tagged_arr s1,struct _tagged_arr s2,
unsigned int n);int Cyc_zstrptrcmp(struct _tagged_arr*,struct _tagged_arr*);int Cyc_strcasecmp(
struct _tagged_arr,struct _tagged_arr);int Cyc_strncasecmp(struct _tagged_arr s1,
struct _tagged_arr s2,unsigned int len);struct _tagged_arr Cyc_strcat(struct
_tagged_arr dest,struct _tagged_arr src);struct _tagged_arr Cyc_strconcat(struct
_tagged_arr,struct _tagged_arr);struct _tagged_arr Cyc_rstrconcat(struct
_RegionHandle*,struct _tagged_arr,struct _tagged_arr);struct _tagged_arr Cyc_strconcat_l(
struct Cyc_List_List*);struct _tagged_arr Cyc_rstrconcat_l(struct _RegionHandle*,
struct Cyc_List_List*);struct _tagged_arr Cyc_str_sepstr(struct Cyc_List_List*,
struct _tagged_arr);struct _tagged_arr Cyc_rstr_sepstr(struct _RegionHandle*,struct
Cyc_List_List*,struct _tagged_arr);struct _tagged_arr Cyc_strcpy(struct _tagged_arr
dest,struct _tagged_arr src);struct _tagged_arr Cyc_strncpy(struct _tagged_arr,struct
_tagged_arr,unsigned int);struct _tagged_arr Cyc_zstrncpy(struct _tagged_arr,struct
_tagged_arr,unsigned int);struct _tagged_arr Cyc_realloc(struct _tagged_arr,
unsigned int);struct _tagged_arr Cyc__memcpy(struct _tagged_arr d,struct _tagged_arr s,
unsigned int,unsigned int);struct _tagged_arr Cyc__memmove(struct _tagged_arr d,
struct _tagged_arr s,unsigned int,unsigned int);int Cyc_memcmp(struct _tagged_arr s1,
struct _tagged_arr s2,unsigned int n);struct _tagged_arr Cyc_memchr(struct _tagged_arr
s,char c,unsigned int n);struct _tagged_arr Cyc_mmemchr(struct _tagged_arr s,char c,
unsigned int n);struct _tagged_arr Cyc_memset(struct _tagged_arr s,char c,unsigned int
n);void Cyc_bzero(struct _tagged_arr s,unsigned int n);void Cyc__bcopy(struct
_tagged_arr src,struct _tagged_arr dst,unsigned int n,unsigned int sz);struct
_tagged_arr Cyc_expand(struct _tagged_arr s,unsigned int sz);struct _tagged_arr Cyc_rexpand(
struct _RegionHandle*,struct _tagged_arr s,unsigned int sz);struct _tagged_arr Cyc_realloc_str(
struct _tagged_arr str,unsigned int sz);struct _tagged_arr Cyc_rrealloc_str(struct
_RegionHandle*r,struct _tagged_arr str,unsigned int sz);struct _tagged_arr Cyc_strdup(
struct _tagged_arr src);struct _tagged_arr Cyc_rstrdup(struct _RegionHandle*,struct
_tagged_arr src);struct _tagged_arr Cyc_substring(struct _tagged_arr,int ofs,
unsigned int n);struct _tagged_arr Cyc_rsubstring(struct _RegionHandle*,struct
_tagged_arr,int ofs,unsigned int n);struct _tagged_arr Cyc_replace_suffix(struct
_tagged_arr,struct _tagged_arr,struct _tagged_arr);struct _tagged_arr Cyc_rreplace_suffix(
struct _RegionHandle*r,struct _tagged_arr src,struct _tagged_arr curr_suffix,struct
_tagged_arr new_suffix);struct _tagged_arr Cyc_strchr(struct _tagged_arr s,char c);
struct _tagged_arr Cyc_mstrchr(struct _tagged_arr s,char c);struct _tagged_arr Cyc_mstrrchr(
struct _tagged_arr s,char c);struct _tagged_arr Cyc_strrchr(struct _tagged_arr s,char c);
struct _tagged_arr Cyc_mstrstr(struct _tagged_arr haystack,struct _tagged_arr needle);
struct _tagged_arr Cyc_strstr(struct _tagged_arr haystack,struct _tagged_arr needle);
struct _tagged_arr Cyc_strpbrk(struct _tagged_arr s,struct _tagged_arr accept);struct
_tagged_arr Cyc_mstrpbrk(struct _tagged_arr s,struct _tagged_arr accept);unsigned int
Cyc_strspn(struct _tagged_arr s,struct _tagged_arr accept);unsigned int Cyc_strcspn(
struct _tagged_arr s,struct _tagged_arr accept);struct _tagged_arr Cyc_strtok(struct
_tagged_arr s,struct _tagged_arr delim);struct Cyc_List_List*Cyc_explode(struct
_tagged_arr s);struct Cyc_List_List*Cyc_rexplode(struct _RegionHandle*,struct
_tagged_arr s);struct _tagged_arr Cyc_implode(struct Cyc_List_List*c);void*Cyc___assert_fail(
struct _tagged_arr assertion,struct _tagged_arr file,unsigned int line);char*strerror(
int errnum);unsigned int Cyc_strlen(struct _tagged_arr s){unsigned int i;unsigned int
sz=_get_arr_size(s,sizeof(char));for(i=0;i < sz;i ++){if(((const char*)s.curr)[(int)
i]== '\000')return i;}return i;}int Cyc_strcmp(struct _tagged_arr s1,struct
_tagged_arr s2){if(s1.curr == s2.curr)return 0;{int i=0;unsigned int sz1=
_get_arr_size(s1,sizeof(char));unsigned int sz2=_get_arr_size(s2,sizeof(char));
unsigned int minsz=sz1 < sz2?sz1: sz2;(minsz <= sz1?minsz <= sz2: 0)?0:((int(*)(struct
_tagged_arr assertion,struct _tagged_arr file,unsigned int line))Cyc___assert_fail)(({
const char*_tmp0="minsz <= sz1 && minsz <= sz2";_tag_arr(_tmp0,sizeof(char),
_get_zero_arr_size(_tmp0,29));}),({const char*_tmp1="string.cyc";_tag_arr(_tmp1,
sizeof(char),_get_zero_arr_size(_tmp1,11));}),60);while(i < minsz){char c1=*((
const char*)_check_unknown_subscript(s1,sizeof(char),i));char c2=*((const char*)
_check_unknown_subscript(s2,sizeof(char),i));if(c1 == '\000'){if(c2 == '\000')
return 0;else{return - 1;}}else{if(c2 == '\000')return 1;else{int diff=c1 - c2;if(diff
!= 0)return diff;}}++ i;}if(sz1 == sz2)return 0;if(minsz < sz2){if(*((const char*)
_check_unknown_subscript(s2,sizeof(char),i))== '\000')return 0;else{return - 1;}}
else{if(*((const char*)_check_unknown_subscript(s1,sizeof(char),i))== '\000')
return 0;else{return 1;}}}}int Cyc_strptrcmp(struct _tagged_arr*s1,struct _tagged_arr*
s2){return Cyc_strcmp((struct _tagged_arr)*((struct _tagged_arr*)s1),(struct
_tagged_arr)*((struct _tagged_arr*)s2));}inline static int Cyc_ncmp(struct
_tagged_arr s1,unsigned int len1,struct _tagged_arr s2,unsigned int len2,unsigned int
n){if(n <= 0)return 0;{unsigned int min_len=len1 > len2?len2: len1;unsigned int bound=
min_len > n?n: min_len;(bound <= _get_arr_size(s1,sizeof(char))?bound <= 
_get_arr_size(s2,sizeof(char)): 0)?0:((int(*)(struct _tagged_arr assertion,struct
_tagged_arr file,unsigned int line))Cyc___assert_fail)(({const char*_tmp2="bound <= s1.size && bound <= s2.size";
_tag_arr(_tmp2,sizeof(char),_get_zero_arr_size(_tmp2,37));}),({const char*_tmp3="string.cyc";
_tag_arr(_tmp3,sizeof(char),_get_zero_arr_size(_tmp3,11));}),96);{int i=0;for(0;i
< bound;i ++){int retc;if((retc=((const char*)s1.curr)[i]- ((const char*)s2.curr)[i])
!= 0)return retc;}}if(len1 < n?1: len2 < n)return(int)len1 - (int)len2;return 0;}}int
Cyc_strncmp(struct _tagged_arr s1,struct _tagged_arr s2,unsigned int n){unsigned int
len1=Cyc_strlen(s1);unsigned int len2=Cyc_strlen(s2);return Cyc_ncmp(s1,len1,s2,
len2,n);}int Cyc_zstrcmp(struct _tagged_arr a,struct _tagged_arr b){if(a.curr == b.curr)
return 0;{unsigned int as=_get_arr_size(a,sizeof(char));unsigned int bs=
_get_arr_size(b,sizeof(char));unsigned int min_length=as < bs?as: bs;int i=- 1;(
min_length <= _get_arr_size(a,sizeof(char))?min_length <= _get_arr_size(b,sizeof(
char)): 0)?0:((int(*)(struct _tagged_arr assertion,struct _tagged_arr file,
unsigned int line))Cyc___assert_fail)(({const char*_tmp4="min_length <= a.size && min_length <= b.size";
_tag_arr(_tmp4,sizeof(char),_get_zero_arr_size(_tmp4,45));}),({const char*_tmp5="string.cyc";
_tag_arr(_tmp5,sizeof(char),_get_zero_arr_size(_tmp5,11));}),128);while((++ i,i < 
min_length)){int diff=(int)((const char*)a.curr)[i]- (int)((const char*)b.curr)[i];
if(diff != 0)return diff;}return(int)as - (int)bs;}}int Cyc_zstrncmp(struct
_tagged_arr s1,struct _tagged_arr s2,unsigned int n){if(n <= 0)return 0;{unsigned int
s1size=_get_arr_size(s1,sizeof(char));unsigned int s2size=_get_arr_size(s2,
sizeof(char));unsigned int min_size=s1size > s2size?s2size: s1size;unsigned int
bound=min_size > n?n: min_size;(bound <= _get_arr_size(s1,sizeof(char))?bound <= 
_get_arr_size(s2,sizeof(char)): 0)?0:((int(*)(struct _tagged_arr assertion,struct
_tagged_arr file,unsigned int line))Cyc___assert_fail)(({const char*_tmp6="bound <= s1.size && bound <= s2.size";
_tag_arr(_tmp6,sizeof(char),_get_zero_arr_size(_tmp6,37));}),({const char*_tmp7="string.cyc";
_tag_arr(_tmp7,sizeof(char),_get_zero_arr_size(_tmp7,11));}),146);{int i=0;for(0;
i < bound;i ++){if(((const char*)s1.curr)[i]< ((const char*)s2.curr)[i])return - 1;
else{if(((const char*)s2.curr)[i]< ((const char*)s1.curr)[i])return 1;}}}if(
min_size <= bound)return 0;if(s1size < s2size)return - 1;else{return 1;}}}int Cyc_zstrptrcmp(
struct _tagged_arr*a,struct _tagged_arr*b){if((int)a == (int)b)return 0;return Cyc_zstrcmp((
struct _tagged_arr)*((struct _tagged_arr*)a),(struct _tagged_arr)*((struct
_tagged_arr*)b));}inline static struct _tagged_arr Cyc_int_strcato(struct _tagged_arr
dest,struct _tagged_arr src){int i;unsigned int dsize;unsigned int slen;unsigned int
dlen;dsize=_get_arr_size(dest,sizeof(char));dlen=Cyc_strlen((struct _tagged_arr)
dest);slen=Cyc_strlen(src);if(slen + dlen <= dsize){slen <= _get_arr_size(src,
sizeof(char))?0:((int(*)(struct _tagged_arr assertion,struct _tagged_arr file,
unsigned int line))Cyc___assert_fail)(({const char*_tmp8="slen <= src.size";
_tag_arr(_tmp8,sizeof(char),_get_zero_arr_size(_tmp8,17));}),({const char*_tmp9="string.cyc";
_tag_arr(_tmp9,sizeof(char),_get_zero_arr_size(_tmp9,11));}),182);for(i=0;i < 
slen;i ++){({struct _tagged_arr _tmpA=_tagged_arr_plus(dest,sizeof(char),(int)(i + 
dlen));char _tmpB=*((char*)_check_unknown_subscript(_tmpA,sizeof(char),0));char
_tmpC=((const char*)src.curr)[i];if(_get_arr_size(_tmpA,sizeof(char))== 1?_tmpB == '\000'?
_tmpC != '\000': 0: 0)_throw_arraybounds();*((char*)_tmpA.curr)=_tmpC;});}if(i != 
dsize)({struct _tagged_arr _tmpD=_tagged_arr_plus(dest,sizeof(char),(int)(i + dlen));
char _tmpE=*((char*)_check_unknown_subscript(_tmpD,sizeof(char),0));char _tmpF='\000';
if(_get_arr_size(_tmpD,sizeof(char))== 1?_tmpE == '\000'?_tmpF != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmpD.curr)=_tmpF;});}else{(int)_throw((void*)({
struct Cyc_Core_Invalid_argument_struct*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10[
0]=({struct Cyc_Core_Invalid_argument_struct _tmp11;_tmp11.tag=Cyc_Core_Invalid_argument;
_tmp11.f1=({const char*_tmp12="strcat";_tag_arr(_tmp12,sizeof(char),
_get_zero_arr_size(_tmp12,7));});_tmp11;});_tmp10;}));}return dest;}struct
_tagged_arr Cyc_strcat(struct _tagged_arr dest,struct _tagged_arr src){return Cyc_int_strcato(
dest,src);}struct _tagged_arr Cyc_rstrconcat(struct _RegionHandle*r,struct
_tagged_arr a,struct _tagged_arr b){unsigned int _tmp13=Cyc_strlen(a);unsigned int
_tmp14=Cyc_strlen(b);struct _tagged_arr ans=Cyc_Core_rnew_string(r,(_tmp13 + _tmp14)
+ 1);int i;int j;(_tmp13 <= _get_arr_size(ans,sizeof(char))?_tmp13 <= _get_arr_size(
a,sizeof(char)): 0)?0:((int(*)(struct _tagged_arr assertion,struct _tagged_arr file,
unsigned int line))Cyc___assert_fail)(({const char*_tmp15="alen <= ans.size && alen <= a.size";
_tag_arr(_tmp15,sizeof(char),_get_zero_arr_size(_tmp15,35));}),({const char*
_tmp16="string.cyc";_tag_arr(_tmp16,sizeof(char),_get_zero_arr_size(_tmp16,11));}),
206);for(i=0;i < _tmp13;++ i){({struct _tagged_arr _tmp17=_tagged_arr_plus(ans,
sizeof(char),i);char _tmp18=*((char*)_check_unknown_subscript(_tmp17,sizeof(char),
0));char _tmp19=((const char*)a.curr)[i];if(_get_arr_size(_tmp17,sizeof(char))== 1?
_tmp18 == '\000'?_tmp19 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp17.curr)=
_tmp19;});}_tmp14 <= _get_arr_size(b,sizeof(char))?0:((int(*)(struct _tagged_arr
assertion,struct _tagged_arr file,unsigned int line))Cyc___assert_fail)(({const char*
_tmp1A="blen <= b.size";_tag_arr(_tmp1A,sizeof(char),_get_zero_arr_size(_tmp1A,
15));}),({const char*_tmp1B="string.cyc";_tag_arr(_tmp1B,sizeof(char),
_get_zero_arr_size(_tmp1B,11));}),208);for(j=0;j < _tmp14;++ j){({struct
_tagged_arr _tmp1C=_tagged_arr_plus(ans,sizeof(char),i + j);char _tmp1D=*((char*)
_check_unknown_subscript(_tmp1C,sizeof(char),0));char _tmp1E=((const char*)b.curr)[
j];if(_get_arr_size(_tmp1C,sizeof(char))== 1?_tmp1D == '\000'?_tmp1E != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp1C.curr)=_tmp1E;});}return ans;}struct
_tagged_arr Cyc_strconcat(struct _tagged_arr a,struct _tagged_arr b){return Cyc_rstrconcat(
Cyc_Core_heap_region,a,b);}struct _tagged_arr Cyc_rstrconcat_l(struct _RegionHandle*
r,struct Cyc_List_List*strs){unsigned int len;unsigned int total_len=0;struct
_tagged_arr ans;{struct _RegionHandle _tmp1F=_new_region("temp");struct
_RegionHandle*temp=& _tmp1F;_push_region(temp);{struct Cyc_List_List*lens=({struct
Cyc_List_List*_tmp22=_region_malloc(temp,sizeof(*_tmp22));_tmp22->hd=(void*)((
unsigned int)0);_tmp22->tl=0;_tmp22;});struct Cyc_List_List*end=lens;{struct Cyc_List_List*
p=strs;for(0;p != 0;p=p->tl){len=Cyc_strlen((struct _tagged_arr)*((struct
_tagged_arr*)((struct _tagged_arr*)p->hd)));total_len +=len;((struct Cyc_List_List*)
_check_null(end))->tl=({struct Cyc_List_List*_tmp20=_region_malloc(temp,sizeof(*
_tmp20));_tmp20->hd=(void*)len;_tmp20->tl=0;_tmp20;});end=((struct Cyc_List_List*)
_check_null(end))->tl;}}lens=lens->tl;ans=Cyc_Core_rnew_string(r,total_len + 1);{
unsigned int i=0;while(strs != 0){struct _tagged_arr _tmp21=*((struct _tagged_arr*)((
struct _tagged_arr*)strs->hd));len=(unsigned int)((struct Cyc_List_List*)
_check_null(lens))->hd;Cyc_strncpy(_tagged_arr_plus(ans,sizeof(char),(int)i),(
struct _tagged_arr)_tmp21,len);i +=len;strs=strs->tl;lens=lens->tl;}}};_pop_region(
temp);}return ans;}struct _tagged_arr Cyc_strconcat_l(struct Cyc_List_List*strs){
return Cyc_rstrconcat_l(Cyc_Core_heap_region,strs);}struct _tagged_arr Cyc_rstr_sepstr(
struct _RegionHandle*r,struct Cyc_List_List*strs,struct _tagged_arr separator){if(
strs == 0)return Cyc_Core_rnew_string(r,0);if(strs->tl == 0)return Cyc_rstrdup(r,(
struct _tagged_arr)*((struct _tagged_arr*)((struct _tagged_arr*)strs->hd)));{struct
Cyc_List_List*_tmp23=strs;struct _RegionHandle _tmp24=_new_region("temp");struct
_RegionHandle*temp=& _tmp24;_push_region(temp);{struct Cyc_List_List*lens=({struct
Cyc_List_List*_tmp28=_region_malloc(temp,sizeof(*_tmp28));_tmp28->hd=(void*)((
unsigned int)0);_tmp28->tl=0;_tmp28;});struct Cyc_List_List*end=lens;unsigned int
len;unsigned int total_len=0;unsigned int list_len=0;for(0;_tmp23 != 0;_tmp23=
_tmp23->tl){len=Cyc_strlen((struct _tagged_arr)*((struct _tagged_arr*)((struct
_tagged_arr*)_tmp23->hd)));total_len +=len;((struct Cyc_List_List*)_check_null(end))->tl=({
struct Cyc_List_List*_tmp25=_region_malloc(temp,sizeof(*_tmp25));_tmp25->hd=(void*)
len;_tmp25->tl=0;_tmp25;});end=((struct Cyc_List_List*)_check_null(end))->tl;++
list_len;}lens=lens->tl;{unsigned int seplen=Cyc_strlen(separator);total_len +=(
list_len - 1)* seplen;{struct _tagged_arr ans=Cyc_Core_rnew_string(r,total_len + 1);
unsigned int i=0;while(strs->tl != 0){struct _tagged_arr _tmp26=*((struct _tagged_arr*)((
struct _tagged_arr*)strs->hd));len=(unsigned int)((struct Cyc_List_List*)
_check_null(lens))->hd;Cyc_strncpy(_tagged_arr_plus(ans,sizeof(char),(int)i),(
struct _tagged_arr)_tmp26,len);i +=len;Cyc_strncpy(_tagged_arr_plus(ans,sizeof(
char),(int)i),separator,seplen);i +=seplen;strs=strs->tl;lens=lens->tl;}Cyc_strncpy(
_tagged_arr_plus(ans,sizeof(char),(int)i),(struct _tagged_arr)*((struct
_tagged_arr*)((struct _tagged_arr*)strs->hd)),(unsigned int)((struct Cyc_List_List*)
_check_null(lens))->hd);{struct _tagged_arr _tmp27=ans;_npop_handler(0);return
_tmp27;}}}};_pop_region(temp);}}struct _tagged_arr Cyc_str_sepstr(struct Cyc_List_List*
strs,struct _tagged_arr separator){return Cyc_rstr_sepstr(Cyc_Core_heap_region,strs,
separator);}struct _tagged_arr Cyc_strncpy(struct _tagged_arr dest,struct _tagged_arr
src,unsigned int n){int i;(n <= _get_arr_size(src,sizeof(char))?n <= _get_arr_size(
dest,sizeof(char)): 0)?0:((int(*)(struct _tagged_arr assertion,struct _tagged_arr
file,unsigned int line))Cyc___assert_fail)(({const char*_tmp29="n <= src.size && n <= dest.size";
_tag_arr(_tmp29,sizeof(char),_get_zero_arr_size(_tmp29,32));}),({const char*
_tmp2A="string.cyc";_tag_arr(_tmp2A,sizeof(char),_get_zero_arr_size(_tmp2A,11));}),
299);for(i=0;i < n;i ++){char _tmp2B=((const char*)src.curr)[i];if(_tmp2B == '\000')
break;({struct _tagged_arr _tmp2C=_tagged_arr_plus(dest,sizeof(char),i);char _tmp2D=*((
char*)_check_unknown_subscript(_tmp2C,sizeof(char),0));char _tmp2E=_tmp2B;if(
_get_arr_size(_tmp2C,sizeof(char))== 1?_tmp2D == '\000'?_tmp2E != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp2C.curr)=_tmp2E;});}for(0;i < n;i ++){({struct
_tagged_arr _tmp2F=_tagged_arr_plus(dest,sizeof(char),i);char _tmp30=*((char*)
_check_unknown_subscript(_tmp2F,sizeof(char),0));char _tmp31='\000';if(
_get_arr_size(_tmp2F,sizeof(char))== 1?_tmp30 == '\000'?_tmp31 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp2F.curr)=_tmp31;});}return dest;}struct
_tagged_arr Cyc_zstrncpy(struct _tagged_arr dest,struct _tagged_arr src,unsigned int n){(
n <= _get_arr_size(dest,sizeof(char))?n <= _get_arr_size(src,sizeof(char)): 0)?0:((
int(*)(struct _tagged_arr assertion,struct _tagged_arr file,unsigned int line))Cyc___assert_fail)(({
const char*_tmp32="n <= dest.size && n <= src.size";_tag_arr(_tmp32,sizeof(char),
_get_zero_arr_size(_tmp32,32));}),({const char*_tmp33="string.cyc";_tag_arr(
_tmp33,sizeof(char),_get_zero_arr_size(_tmp33,11));}),313);{int i;for(i=0;i < n;i
++){({struct _tagged_arr _tmp34=_tagged_arr_plus(dest,sizeof(char),i);char _tmp35=*((
char*)_check_unknown_subscript(_tmp34,sizeof(char),0));char _tmp36=((const char*)
src.curr)[i];if(_get_arr_size(_tmp34,sizeof(char))== 1?_tmp35 == '\000'?_tmp36 != '\000':
0: 0)_throw_arraybounds();*((char*)_tmp34.curr)=_tmp36;});}return dest;}}struct
_tagged_arr Cyc_strcpy(struct _tagged_arr dest,struct _tagged_arr src){unsigned int
ssz=_get_arr_size(src,sizeof(char));unsigned int dsz=_get_arr_size(dest,sizeof(
char));if(ssz <= dsz){unsigned int i;for(i=0;i < ssz;i ++){char _tmp37=((const char*)
src.curr)[(int)i];({struct _tagged_arr _tmp38=_tagged_arr_plus(dest,sizeof(char),(
int)i);char _tmp39=*((char*)_check_unknown_subscript(_tmp38,sizeof(char),0));char
_tmp3A=_tmp37;if(_get_arr_size(_tmp38,sizeof(char))== 1?_tmp39 == '\000'?_tmp3A != '\000':
0: 0)_throw_arraybounds();*((char*)_tmp38.curr)=_tmp3A;});if(_tmp37 == '\000')
break;}}else{unsigned int len=Cyc_strlen(src);Cyc_strncpy(dest,src,len);if(len < 
_get_arr_size(dest,sizeof(char)))({struct _tagged_arr _tmp3B=_tagged_arr_plus(dest,
sizeof(char),(int)len);char _tmp3C=*((char*)_check_unknown_subscript(_tmp3B,
sizeof(char),0));char _tmp3D='\000';if(_get_arr_size(_tmp3B,sizeof(char))== 1?
_tmp3C == '\000'?_tmp3D != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp3B.curr)=
_tmp3D;});}return dest;}struct _tagged_arr Cyc_rstrdup(struct _RegionHandle*r,struct
_tagged_arr src){unsigned int len;struct _tagged_arr temp;len=Cyc_strlen(src);temp=
Cyc_Core_rnew_string(r,len + 1);Cyc_strncpy(temp,src,len);return temp;}struct
_tagged_arr Cyc_strdup(struct _tagged_arr src){return Cyc_rstrdup(Cyc_Core_heap_region,
src);}struct _tagged_arr Cyc_rexpand(struct _RegionHandle*r,struct _tagged_arr s,
unsigned int sz){struct _tagged_arr temp;unsigned int slen;slen=Cyc_strlen(s);sz=sz > 
slen?sz: slen;temp=Cyc_Core_rnew_string(r,sz);Cyc_strncpy(temp,s,slen);if(slen != 
_get_arr_size(s,sizeof(char)))({struct _tagged_arr _tmp3E=_tagged_arr_plus(temp,
sizeof(char),(int)slen);char _tmp3F=*((char*)_check_unknown_subscript(_tmp3E,
sizeof(char),0));char _tmp40='\000';if(_get_arr_size(_tmp3E,sizeof(char))== 1?
_tmp3F == '\000'?_tmp40 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp3E.curr)=
_tmp40;});return temp;}struct _tagged_arr Cyc_expand(struct _tagged_arr s,
unsigned int sz){return Cyc_rexpand(Cyc_Core_heap_region,s,sz);}struct _tagged_arr
Cyc_rrealloc_str(struct _RegionHandle*r,struct _tagged_arr str,unsigned int sz){
unsigned int maxsizeP=_get_arr_size(str,sizeof(char));if(maxsizeP == 0){maxsizeP=
30 > sz?30: sz;str=Cyc_Core_rnew_string(r,maxsizeP);({struct _tagged_arr _tmp41=
_tagged_arr_plus(str,sizeof(char),0);char _tmp42=*((char*)
_check_unknown_subscript(_tmp41,sizeof(char),0));char _tmp43='\000';if(
_get_arr_size(_tmp41,sizeof(char))== 1?_tmp42 == '\000'?_tmp43 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp41.curr)=_tmp43;});}else{if(sz > maxsizeP){
maxsizeP=maxsizeP * 2 > (sz * 5)/ 4?maxsizeP * 2:(sz * 5)/ 4;str=Cyc_rexpand(r,(
struct _tagged_arr)str,maxsizeP);}}return str;}struct _tagged_arr Cyc_realloc_str(
struct _tagged_arr str,unsigned int sz){return Cyc_rrealloc_str(Cyc_Core_heap_region,
str,sz);}struct _tagged_arr Cyc_rsubstring(struct _RegionHandle*r,struct _tagged_arr
s,int start,unsigned int amt){struct _tagged_arr ans=Cyc_Core_rnew_string(r,amt + 1);
s=_tagged_arr_plus(s,sizeof(char),start);(amt < _get_arr_size(ans,sizeof(char))?
amt <= _get_arr_size(s,sizeof(char)): 0)?0:((int(*)(struct _tagged_arr assertion,
struct _tagged_arr file,unsigned int line))Cyc___assert_fail)(({const char*_tmp44="amt < ans.size && amt <= s.size";
_tag_arr(_tmp44,sizeof(char),_get_zero_arr_size(_tmp44,32));}),({const char*
_tmp45="string.cyc";_tag_arr(_tmp45,sizeof(char),_get_zero_arr_size(_tmp45,11));}),
409);{unsigned int i=0;for(0;i < amt;++ i){({struct _tagged_arr _tmp46=
_tagged_arr_plus(ans,sizeof(char),(int)i);char _tmp47=*((char*)
_check_unknown_subscript(_tmp46,sizeof(char),0));char _tmp48=((const char*)s.curr)[(
int)i];if(_get_arr_size(_tmp46,sizeof(char))== 1?_tmp47 == '\000'?_tmp48 != '\000':
0: 0)_throw_arraybounds();*((char*)_tmp46.curr)=_tmp48;});}}({struct _tagged_arr
_tmp49=_tagged_arr_plus(ans,sizeof(char),(int)amt);char _tmp4A=*((char*)
_check_unknown_subscript(_tmp49,sizeof(char),0));char _tmp4B='\000';if(
_get_arr_size(_tmp49,sizeof(char))== 1?_tmp4A == '\000'?_tmp4B != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp49.curr)=_tmp4B;});return ans;}struct
_tagged_arr Cyc_substring(struct _tagged_arr s,int start,unsigned int amt){return Cyc_rsubstring(
Cyc_Core_heap_region,s,start,amt);}struct _tagged_arr Cyc_rreplace_suffix(struct
_RegionHandle*r,struct _tagged_arr src,struct _tagged_arr curr_suffix,struct
_tagged_arr new_suffix){unsigned int m=_get_arr_size(src,sizeof(char));
unsigned int n=_get_arr_size(curr_suffix,sizeof(char));struct _tagged_arr err=({
const char*_tmp50="replace_suffix";_tag_arr(_tmp50,sizeof(char),
_get_zero_arr_size(_tmp50,15));});if(m < n)(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp4D;_tmp4D.tag=Cyc_Core_Invalid_argument;_tmp4D.f1=err;_tmp4D;});_tmp4C;}));{
unsigned int i=1;for(0;i <= n;++ i){if(*((const char*)_check_unknown_subscript(src,
sizeof(char),(int)(m - i)))!= *((const char*)_check_unknown_subscript(curr_suffix,
sizeof(char),(int)(n - i))))(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp4F;_tmp4F.tag=Cyc_Core_Invalid_argument;_tmp4F.f1=err;_tmp4F;});_tmp4E;}));}}{
struct _tagged_arr ans=Cyc_Core_rnew_string(r,(m - n)+ _get_arr_size(new_suffix,
sizeof(char)));Cyc_strncpy(ans,src,m - n);Cyc_strncpy(_tagged_arr_plus(
_tagged_arr_plus(ans,sizeof(char),(int)m),sizeof(char),-(int)n),new_suffix,
_get_arr_size(new_suffix,sizeof(char)));return ans;}}struct _tagged_arr Cyc_replace_suffix(
struct _tagged_arr src,struct _tagged_arr curr_suffix,struct _tagged_arr new_suffix){
return Cyc_rreplace_suffix(Cyc_Core_heap_region,src,curr_suffix,new_suffix);}
struct _tagged_arr Cyc_strpbrk(struct _tagged_arr s,struct _tagged_arr accept){int len=(
int)_get_arr_size(s,sizeof(char));unsigned int asize=_get_arr_size(accept,sizeof(
char));char c;unsigned int i;for(i=0;i < len?(c=((const char*)s.curr)[(int)i])!= 0: 0;
i ++){unsigned int j=0;for(0;j < asize;j ++){if(c == ((const char*)accept.curr)[(int)j])
return _tagged_arr_plus(s,sizeof(char),(int)i);}}return(struct _tagged_arr)
_tag_arr(0,0,0);}struct _tagged_arr Cyc_mstrpbrk(struct _tagged_arr s,struct
_tagged_arr accept){int len=(int)_get_arr_size(s,sizeof(char));unsigned int asize=
_get_arr_size(accept,sizeof(char));char c;unsigned int i;for(i=0;i < len?(c=((char*)
s.curr)[(int)i])!= 0: 0;i ++){unsigned int j=0;for(0;j < asize;j ++){if(c == ((const
char*)accept.curr)[(int)j])return _tagged_arr_plus(s,sizeof(char),(int)i);}}
return _tag_arr(0,0,0);}struct _tagged_arr Cyc_mstrchr(struct _tagged_arr s,char c){
int len=(int)_get_arr_size(s,sizeof(char));char c2;unsigned int i;for(i=0;i < len?(
c2=((char*)s.curr)[(int)i])!= 0: 0;i ++){if(c2 == c)return _tagged_arr_plus(s,
sizeof(char),(int)i);}return _tag_arr(0,0,0);}struct _tagged_arr Cyc_strchr(struct
_tagged_arr s,char c){int len=(int)_get_arr_size(s,sizeof(char));char c2;
unsigned int i;for(i=0;i < len?(c2=((const char*)s.curr)[(int)i])!= 0: 0;i ++){if(c2
== c)return _tagged_arr_plus(s,sizeof(char),(int)i);}return(struct _tagged_arr)
_tag_arr(0,0,0);}struct _tagged_arr Cyc_strrchr(struct _tagged_arr s,char c){int len=(
int)Cyc_strlen((struct _tagged_arr)s);int i=len - 1;_tagged_arr_inplace_plus(& s,
sizeof(char),i);for(0;i >= 0;(i --,_tagged_arr_inplace_plus_post(& s,sizeof(char),
-1))){if(*((const char*)_check_unknown_subscript(s,sizeof(char),0))== c)return s;}
return(struct _tagged_arr)_tag_arr(0,0,0);}struct _tagged_arr Cyc_mstrrchr(struct
_tagged_arr s,char c){int len=(int)Cyc_strlen((struct _tagged_arr)s);int i=len - 1;
_tagged_arr_inplace_plus(& s,sizeof(char),i);for(0;i >= 0;(i --,
_tagged_arr_inplace_plus_post(& s,sizeof(char),-1))){if(*((char*)
_check_unknown_subscript(s,sizeof(char),0))== c)return s;}return _tag_arr(0,0,0);}
struct _tagged_arr Cyc_strstr(struct _tagged_arr haystack,struct _tagged_arr needle){
if(!((unsigned int)haystack.curr)?1: !((unsigned int)needle.curr))(int)_throw((
void*)({struct Cyc_Core_Invalid_argument_struct*_tmp51=_cycalloc(sizeof(*_tmp51));
_tmp51[0]=({struct Cyc_Core_Invalid_argument_struct _tmp52;_tmp52.tag=Cyc_Core_Invalid_argument;
_tmp52.f1=({const char*_tmp53="strstr";_tag_arr(_tmp53,sizeof(char),
_get_zero_arr_size(_tmp53,7));});_tmp52;});_tmp51;}));if(*((const char*)
_check_unknown_subscript(needle,sizeof(char),0))== '\000')return haystack;{int len=(
int)Cyc_strlen((struct _tagged_arr)needle);{struct _tagged_arr start=haystack;for(0;(
start=Cyc_strchr(start,*((const char*)_check_unknown_subscript(needle,sizeof(char),
0)))).curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr;start=Cyc_strchr(
_tagged_arr_plus(start,sizeof(char),1),*((const char*)_check_unknown_subscript(
needle,sizeof(char),0)))){if(Cyc_strncmp((struct _tagged_arr)start,(struct
_tagged_arr)needle,(unsigned int)len)== 0)return start;}}return(struct _tagged_arr)
_tag_arr(0,0,0);}}struct _tagged_arr Cyc_mstrstr(struct _tagged_arr haystack,struct
_tagged_arr needle){if(!((unsigned int)haystack.curr)?1: !((unsigned int)needle.curr))(
int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp54=_cycalloc(
sizeof(*_tmp54));_tmp54[0]=({struct Cyc_Core_Invalid_argument_struct _tmp55;_tmp55.tag=
Cyc_Core_Invalid_argument;_tmp55.f1=({const char*_tmp56="mstrstr";_tag_arr(_tmp56,
sizeof(char),_get_zero_arr_size(_tmp56,8));});_tmp55;});_tmp54;}));if(*((const
char*)_check_unknown_subscript(needle,sizeof(char),0))== '\000')return haystack;{
int len=(int)Cyc_strlen((struct _tagged_arr)needle);{struct _tagged_arr start=
haystack;for(0;(start=Cyc_mstrchr(start,*((const char*)_check_unknown_subscript(
needle,sizeof(char),0)))).curr != (_tag_arr(0,0,0)).curr;start=Cyc_mstrchr(
_tagged_arr_plus(start,sizeof(char),1),*((const char*)_check_unknown_subscript(
needle,sizeof(char),0)))){if(Cyc_strncmp((struct _tagged_arr)start,(struct
_tagged_arr)needle,(unsigned int)len)== 0)return start;}}return _tag_arr(0,0,0);}}
unsigned int Cyc_strspn(struct _tagged_arr s,struct _tagged_arr accept){unsigned int
len=Cyc_strlen((struct _tagged_arr)s);unsigned int asize=_get_arr_size(accept,
sizeof(char));len <= _get_arr_size(s,sizeof(char))?0:((int(*)(struct _tagged_arr
assertion,struct _tagged_arr file,unsigned int line))Cyc___assert_fail)(({const char*
_tmp57="len <= s.size";_tag_arr(_tmp57,sizeof(char),_get_zero_arr_size(_tmp57,14));}),({
const char*_tmp58="string.cyc";_tag_arr(_tmp58,sizeof(char),_get_zero_arr_size(
_tmp58,11));}),557);{unsigned int i=0;for(0;i < len;i ++){int j;for(j=0;j < asize;j ++){
if(((const char*)s.curr)[(int)i]== ((const char*)accept.curr)[j])break;}if(j == 
asize)return i;}}return len;}unsigned int Cyc_strcspn(struct _tagged_arr s,struct
_tagged_arr accept){unsigned int len=Cyc_strlen((struct _tagged_arr)s);unsigned int
asize=_get_arr_size(accept,sizeof(char));len <= _get_arr_size(s,sizeof(char))?0:((
int(*)(struct _tagged_arr assertion,struct _tagged_arr file,unsigned int line))Cyc___assert_fail)(({
const char*_tmp59="len <= s.size";_tag_arr(_tmp59,sizeof(char),_get_zero_arr_size(
_tmp59,14));}),({const char*_tmp5A="string.cyc";_tag_arr(_tmp5A,sizeof(char),
_get_zero_arr_size(_tmp5A,11));}),577);{unsigned int i=0;for(0;i < len;i ++){int j;
for(j=0;j < asize;j ++){if(((const char*)s.curr)[(int)i]!= ((const char*)accept.curr)[
j])break;}if(j == asize)return i;}}return len;}struct _tagged_arr Cyc_strtok(struct
_tagged_arr s,struct _tagged_arr delim){static struct _tagged_arr olds={(void*)0,(void*)
0,(void*)(0 + 0)};struct _tagged_arr token;if(s.curr == (_tag_arr(0,0,0)).curr){if(
olds.curr == (_tag_arr(0,0,0)).curr)return _tag_arr(0,0,0);s=olds;}{unsigned int
inc=Cyc_strspn((struct _tagged_arr)s,delim);if(inc >= _get_arr_size(s,sizeof(char))?
1:*((char*)_check_unknown_subscript(_tagged_arr_plus(s,sizeof(char),(int)inc),
sizeof(char),0))== '\000'){olds=_tag_arr(0,0,0);return _tag_arr(0,0,0);}else{
_tagged_arr_inplace_plus(& s,sizeof(char),(int)inc);}token=s;s=Cyc_mstrpbrk(token,(
struct _tagged_arr)delim);if(s.curr == (_tag_arr(0,0,0)).curr)olds=_tag_arr(0,0,0);
else{({struct _tagged_arr _tmp5B=s;char _tmp5C=*((char*)_check_unknown_subscript(
_tmp5B,sizeof(char),0));char _tmp5D='\000';if(_get_arr_size(_tmp5B,sizeof(char))
== 1?_tmp5C == '\000'?_tmp5D != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp5B.curr)=
_tmp5D;});olds=_tagged_arr_plus(s,sizeof(char),1);}return token;}}struct Cyc_List_List*
Cyc_rexplode(struct _RegionHandle*r,struct _tagged_arr s){struct Cyc_List_List*
result=0;{int i=(int)(Cyc_strlen(s)- 1);for(0;i >= 0;i --){result=({struct Cyc_List_List*
_tmp5E=_region_malloc(r,sizeof(*_tmp5E));_tmp5E->hd=(void*)((int)*((const char*)
_check_unknown_subscript(s,sizeof(char),i)));_tmp5E->tl=result;_tmp5E;});}}
return result;}struct Cyc_List_List*Cyc_explode(struct _tagged_arr s){return Cyc_rexplode(
Cyc_Core_heap_region,s);}struct _tagged_arr Cyc_implode(struct Cyc_List_List*chars){
struct _tagged_arr s=Cyc_Core_new_string((unsigned int)(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(chars)+ 1));unsigned int i=0;while(chars != 0){({struct
_tagged_arr _tmp5F=_tagged_arr_plus(s,sizeof(char),(int)i ++);char _tmp60=*((char*)
_check_unknown_subscript(_tmp5F,sizeof(char),0));char _tmp61=(char)((int)chars->hd);
if(_get_arr_size(_tmp5F,sizeof(char))== 1?_tmp60 == '\000'?_tmp61 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp5F.curr)=_tmp61;});chars=chars->tl;}return s;}
inline static int Cyc_casecmp(struct _tagged_arr s1,unsigned int len1,struct
_tagged_arr s2,unsigned int len2){unsigned int min_length=len1 < len2?len1: len2;(
min_length <= _get_arr_size(s1,sizeof(char))?min_length <= _get_arr_size(s2,
sizeof(char)): 0)?0:((int(*)(struct _tagged_arr assertion,struct _tagged_arr file,
unsigned int line))Cyc___assert_fail)(({const char*_tmp62="min_length <= s1.size && min_length <= s2.size";
_tag_arr(_tmp62,sizeof(char),_get_zero_arr_size(_tmp62,47));}),({const char*
_tmp63="string.cyc";_tag_arr(_tmp63,sizeof(char),_get_zero_arr_size(_tmp63,11));}),
658);{int i=- 1;while((++ i,i < min_length)){int diff=toupper((int)((const char*)s1.curr)[
i])- toupper((int)((const char*)s2.curr)[i]);if(diff != 0)return diff;}return(int)
len1 - (int)len2;}}int Cyc_strcasecmp(struct _tagged_arr s1,struct _tagged_arr s2){if(
s1.curr == s2.curr)return 0;{unsigned int len1=Cyc_strlen(s1);unsigned int len2=Cyc_strlen(
s2);return Cyc_casecmp(s1,len1,s2,len2);}}inline static int Cyc_caseless_ncmp(struct
_tagged_arr s1,unsigned int len1,struct _tagged_arr s2,unsigned int len2,unsigned int
n){if(n <= 0)return 0;{unsigned int min_len=len1 > len2?len2: len1;unsigned int bound=
min_len > n?n: min_len;(bound <= _get_arr_size(s1,sizeof(char))?bound <= 
_get_arr_size(s2,sizeof(char)): 0)?0:((int(*)(struct _tagged_arr assertion,struct
_tagged_arr file,unsigned int line))Cyc___assert_fail)(({const char*_tmp64="bound <= s1.size && bound <= s2.size";
_tag_arr(_tmp64,sizeof(char),_get_zero_arr_size(_tmp64,37));}),({const char*
_tmp65="string.cyc";_tag_arr(_tmp65,sizeof(char),_get_zero_arr_size(_tmp65,11));}),
685);{int i=0;for(0;i < bound;i ++){int retc;if((retc=toupper((int)((const char*)s1.curr)[
i])- toupper((int)((const char*)s2.curr)[i]))!= 0)return retc;}}if(len1 < n?1: len2 < 
n)return(int)len1 - (int)len2;return 0;}}int Cyc_strncasecmp(struct _tagged_arr s1,
struct _tagged_arr s2,unsigned int n){unsigned int len1=Cyc_strlen(s1);unsigned int
len2=Cyc_strlen(s2);return Cyc_caseless_ncmp(s1,len1,s2,len2,n);}void*memcpy(void*,
const void*,unsigned int n);void*memmove(void*,const void*,unsigned int n);int memcmp(
const void*,const void*,unsigned int n);char*memchr(const char*,char c,unsigned int n);
void*memset(void*,int c,unsigned int n);void bcopy(const void*src,void*dest,
unsigned int n);void bzero(void*s,unsigned int n);char*GC_realloc(char*,
unsigned int n);struct _tagged_arr Cyc_realloc(struct _tagged_arr s,unsigned int n){
char*_tmp66=GC_realloc((char*)_check_null(_untag_arr(s,sizeof(char),1)),n);
return wrap_Cstring_as_string(_tmp66,n);}struct _tagged_arr Cyc__memcpy(struct
_tagged_arr d,struct _tagged_arr s,unsigned int n,unsigned int sz){if(((d.curr == (
_tag_arr(0,0,0)).curr?1: _get_arr_size(d,sizeof(void))< n)?1: s.curr == ((struct
_tagged_arr)_tag_arr(0,0,0)).curr)?1: _get_arr_size(s,sizeof(void))< n)(int)
_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp67=_cycalloc(sizeof(*
_tmp67));_tmp67[0]=({struct Cyc_Core_Invalid_argument_struct _tmp68;_tmp68.tag=Cyc_Core_Invalid_argument;
_tmp68.f1=({const char*_tmp69="memcpy";_tag_arr(_tmp69,sizeof(char),
_get_zero_arr_size(_tmp69,7));});_tmp68;});_tmp67;}));memcpy((void*)_check_null(
_untag_arr(d,sizeof(void),1)),(const void*)_check_null(_untag_arr(s,sizeof(void),
1)),n * sz);return d;}struct _tagged_arr Cyc__memmove(struct _tagged_arr d,struct
_tagged_arr s,unsigned int n,unsigned int sz){if(((d.curr == (_tag_arr(0,0,0)).curr?
1: _get_arr_size(d,sizeof(void))< n)?1: s.curr == ((struct _tagged_arr)_tag_arr(0,0,
0)).curr)?1: _get_arr_size(s,sizeof(void))< n)(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp6B;_tmp6B.tag=Cyc_Core_Invalid_argument;_tmp6B.f1=({const char*_tmp6C="memove";
_tag_arr(_tmp6C,sizeof(char),_get_zero_arr_size(_tmp6C,7));});_tmp6B;});_tmp6A;}));
memmove((void*)_check_null(_untag_arr(d,sizeof(void),1)),(const void*)_check_null(
_untag_arr(s,sizeof(void),1)),n * sz);return d;}int Cyc_memcmp(struct _tagged_arr s1,
struct _tagged_arr s2,unsigned int n){if(((s1.curr == ((struct _tagged_arr)_tag_arr(0,
0,0)).curr?1: s2.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr)?1:
_get_arr_size(s1,sizeof(char))>= n)?1: _get_arr_size(s2,sizeof(char))>= n)(int)
_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp6D=_cycalloc(sizeof(*
_tmp6D));_tmp6D[0]=({struct Cyc_Core_Invalid_argument_struct _tmp6E;_tmp6E.tag=Cyc_Core_Invalid_argument;
_tmp6E.f1=({const char*_tmp6F="memcmp";_tag_arr(_tmp6F,sizeof(char),
_get_zero_arr_size(_tmp6F,7));});_tmp6E;});_tmp6D;}));return memcmp((const void*)
_check_null(_untag_arr(s1,sizeof(char),1)),(const void*)_check_null(_untag_arr(s2,
sizeof(char),1)),n);}struct _tagged_arr Cyc_memchr(struct _tagged_arr s,char c,
unsigned int n){unsigned int sz=_get_arr_size(s,sizeof(char));if(s.curr == ((struct
_tagged_arr)_tag_arr(0,0,0)).curr?1: n > sz)(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp71;_tmp71.tag=Cyc_Core_Invalid_argument;_tmp71.f1=({const char*_tmp72="memchr";
_tag_arr(_tmp72,sizeof(char),_get_zero_arr_size(_tmp72,7));});_tmp71;});_tmp70;}));{
char*_tmp73=memchr((const char*)_check_null(_untag_arr(s,sizeof(char),1)),c,n);
if(_tmp73 == 0)return(struct _tagged_arr)_tag_arr(0,0,0);{unsigned int _tmp74=(
unsigned int)((const char*)_check_null(_untag_arr(s,sizeof(char),1)));
unsigned int _tmp75=(unsigned int)((const char*)_tmp73);unsigned int _tmp76=_tmp75 - 
_tmp74;return _tagged_arr_plus(s,sizeof(char),(int)_tmp76);}}}struct _tagged_arr
Cyc_mmemchr(struct _tagged_arr s,char c,unsigned int n){unsigned int sz=_get_arr_size(
s,sizeof(char));if(s.curr == (_tag_arr(0,0,0)).curr?1: n > sz)(int)_throw((void*)({
struct Cyc_Core_Invalid_argument_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77[
0]=({struct Cyc_Core_Invalid_argument_struct _tmp78;_tmp78.tag=Cyc_Core_Invalid_argument;
_tmp78.f1=({const char*_tmp79="mmemchr";_tag_arr(_tmp79,sizeof(char),
_get_zero_arr_size(_tmp79,8));});_tmp78;});_tmp77;}));{char*_tmp7A=memchr((const
char*)_check_null(_untag_arr(s,sizeof(char),1)),c,n);if(_tmp7A == 0)return
_tag_arr(0,0,0);{unsigned int _tmp7B=(unsigned int)((const char*)_check_null(
_untag_arr(s,sizeof(char),1)));unsigned int _tmp7C=(unsigned int)_tmp7A;
unsigned int _tmp7D=_tmp7C - _tmp7B;return _tagged_arr_plus(s,sizeof(char),(int)
_tmp7D);}}}struct _tagged_arr Cyc_memset(struct _tagged_arr s,char c,unsigned int n){
if(s.curr == (_tag_arr(0,0,0)).curr?1: n > _get_arr_size(s,sizeof(char)))(int)
_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp7E=_cycalloc(sizeof(*
_tmp7E));_tmp7E[0]=({struct Cyc_Core_Invalid_argument_struct _tmp7F;_tmp7F.tag=Cyc_Core_Invalid_argument;
_tmp7F.f1=({const char*_tmp80="memset";_tag_arr(_tmp80,sizeof(char),
_get_zero_arr_size(_tmp80,7));});_tmp7F;});_tmp7E;}));memset((void*)((char*)
_check_null(_untag_arr(s,sizeof(char),1))),(int)c,n);return s;}void Cyc_bzero(
struct _tagged_arr s,unsigned int n){if(s.curr == (_tag_arr(0,0,0)).curr?1:
_get_arr_size(s,sizeof(char))< n)(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp82;_tmp82.tag=Cyc_Core_Invalid_argument;_tmp82.f1=({const char*_tmp83="bzero";
_tag_arr(_tmp83,sizeof(char),_get_zero_arr_size(_tmp83,6));});_tmp82;});_tmp81;}));((
void(*)(char*s,unsigned int n))bzero)((char*)_check_null(_untag_arr(s,sizeof(char),
1)),n);}void Cyc__bcopy(struct _tagged_arr src,struct _tagged_arr dst,unsigned int n,
unsigned int sz){if(((src.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr?1:
_get_arr_size(src,sizeof(void))< n)?1: dst.curr == (_tag_arr(0,0,0)).curr)?1:
_get_arr_size(dst,sizeof(void))< n)(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp85;_tmp85.tag=Cyc_Core_Invalid_argument;_tmp85.f1=({const char*_tmp86="bcopy";
_tag_arr(_tmp86,sizeof(char),_get_zero_arr_size(_tmp86,6));});_tmp85;});_tmp84;}));
bcopy((const void*)_check_null(_untag_arr(src,sizeof(void),1)),(void*)_check_null(
_untag_arr(dst,sizeof(void),1)),n * sz);}
