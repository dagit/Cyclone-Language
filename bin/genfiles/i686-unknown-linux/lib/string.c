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
  if (!_cks_index >= _cks_bound) _throw_arraybounds(); \
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
#define _zero_arr_plus(orig_x,orig_sz,orig_i) ({ \
  typedef _czs_tx = (*orig_x); \
  _czs_tx *_czs_x = (_czs_tx *)(orig_x); \
  unsigned int _czs_sz = (orig_sz); \
  int _czs_i = (orig_i); \
  unsigned int _czs_temp; \
  if ((_czs_x) == NULL) _throw_null(); \
  if (_czs_i < 0) _throw_arraybounds(); \
  for (_czs_temp=_czs_sz; _czs_temp < _czs_i; _czs_temp++) \
    if (_czs_x[_czs_temp] == 0) _throw_arraybounds(); \
  _czs_x+_czs_i; })

// Calculates the number of elements in a zero-terminated, thin array.
// If non-null, the array is guaranteed to have orig_offset elements.
#define _get_zero_arr_size(orig_x,orig_offset) ({ \
  typedef _gres_tx = (*orig_x); \
  _gres_tx *_gres_x = (_gres_tx *)(orig_x); \
  unsigned int _gres_offset = (orig_offset); \
  unsigned int _gres = 0; \
  if (_gres_x != NULL) { \
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
8];int toupper(int);struct _tagged_arr Cyc_Std_strerror(int);unsigned int Cyc_Std_strlen(
struct _tagged_arr s);int Cyc_Std_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);
int Cyc_Std_strptrcmp(struct _tagged_arr*s1,struct _tagged_arr*s2);int Cyc_Std_strncmp(
struct _tagged_arr s1,struct _tagged_arr s2,unsigned int len);int Cyc_Std_zstrcmp(
struct _tagged_arr,struct _tagged_arr);int Cyc_Std_zstrncmp(struct _tagged_arr s1,
struct _tagged_arr s2,unsigned int n);int Cyc_Std_zstrptrcmp(struct _tagged_arr*,
struct _tagged_arr*);int Cyc_Std_strcasecmp(struct _tagged_arr,struct _tagged_arr);
int Cyc_Std_strncasecmp(struct _tagged_arr s1,struct _tagged_arr s2,unsigned int len);
struct _tagged_arr Cyc_Std_strcat(struct _tagged_arr dest,struct _tagged_arr src);
struct _tagged_arr Cyc_Std_strconcat(struct _tagged_arr,struct _tagged_arr);struct
_tagged_arr Cyc_Std_rstrconcat(struct _RegionHandle*,struct _tagged_arr,struct
_tagged_arr);struct _tagged_arr Cyc_Std_strconcat_l(struct Cyc_List_List*);struct
_tagged_arr Cyc_Std_rstrconcat_l(struct _RegionHandle*,struct Cyc_List_List*);
struct _tagged_arr Cyc_Std_str_sepstr(struct Cyc_List_List*,struct _tagged_arr);
struct _tagged_arr Cyc_Std_rstr_sepstr(struct _RegionHandle*,struct Cyc_List_List*,
struct _tagged_arr);struct _tagged_arr Cyc_Std_strcpy(struct _tagged_arr dest,struct
_tagged_arr src);struct _tagged_arr Cyc_Std_strncpy(struct _tagged_arr,struct
_tagged_arr,unsigned int);struct _tagged_arr Cyc_Std_zstrncpy(struct _tagged_arr,
struct _tagged_arr,unsigned int);struct _tagged_arr Cyc_Std_realloc(struct
_tagged_arr,unsigned int);struct _tagged_arr Cyc_Std__memcpy(struct _tagged_arr d,
struct _tagged_arr s,unsigned int,unsigned int);struct _tagged_arr Cyc_Std__memmove(
struct _tagged_arr d,struct _tagged_arr s,unsigned int,unsigned int);int Cyc_Std_memcmp(
struct _tagged_arr s1,struct _tagged_arr s2,unsigned int n);struct _tagged_arr Cyc_Std_memchr(
struct _tagged_arr s,char c,unsigned int n);struct _tagged_arr Cyc_Std_mmemchr(struct
_tagged_arr s,char c,unsigned int n);struct _tagged_arr Cyc_Std_memset(struct
_tagged_arr s,char c,unsigned int n);void Cyc_Std_bzero(struct _tagged_arr s,
unsigned int n);void Cyc_Std__bcopy(struct _tagged_arr src,struct _tagged_arr dst,
unsigned int n,unsigned int sz);struct _tagged_arr Cyc_Std_expand(struct _tagged_arr s,
unsigned int sz);struct _tagged_arr Cyc_Std_rexpand(struct _RegionHandle*,struct
_tagged_arr s,unsigned int sz);struct _tagged_arr Cyc_Std_realloc_str(struct
_tagged_arr str,unsigned int sz);struct _tagged_arr Cyc_Std_rrealloc_str(struct
_RegionHandle*r,struct _tagged_arr str,unsigned int sz);struct _tagged_arr Cyc_Std_strdup(
struct _tagged_arr src);struct _tagged_arr Cyc_Std_rstrdup(struct _RegionHandle*,
struct _tagged_arr src);struct _tagged_arr Cyc_Std_substring(struct _tagged_arr,int
ofs,unsigned int n);struct _tagged_arr Cyc_Std_rsubstring(struct _RegionHandle*,
struct _tagged_arr,int ofs,unsigned int n);struct _tagged_arr Cyc_Std_replace_suffix(
struct _tagged_arr,struct _tagged_arr,struct _tagged_arr);struct _tagged_arr Cyc_Std_rreplace_suffix(
struct _RegionHandle*r,struct _tagged_arr src,struct _tagged_arr curr_suffix,struct
_tagged_arr new_suffix);struct _tagged_arr Cyc_Std_strchr(struct _tagged_arr s,char c);
struct _tagged_arr Cyc_Std_mstrchr(struct _tagged_arr s,char c);struct _tagged_arr Cyc_Std_mstrrchr(
struct _tagged_arr s,char c);struct _tagged_arr Cyc_Std_strrchr(struct _tagged_arr s,
char c);struct _tagged_arr Cyc_Std_mstrstr(struct _tagged_arr haystack,struct
_tagged_arr needle);struct _tagged_arr Cyc_Std_strstr(struct _tagged_arr haystack,
struct _tagged_arr needle);struct _tagged_arr Cyc_Std_strpbrk(struct _tagged_arr s,
struct _tagged_arr accept);struct _tagged_arr Cyc_Std_mstrpbrk(struct _tagged_arr s,
struct _tagged_arr accept);unsigned int Cyc_Std_strspn(struct _tagged_arr s,struct
_tagged_arr accept);unsigned int Cyc_Std_strcspn(struct _tagged_arr s,struct
_tagged_arr accept);struct _tagged_arr Cyc_Std_strtok(struct _tagged_arr s,struct
_tagged_arr delim);struct Cyc_List_List*Cyc_Std_explode(struct _tagged_arr s);struct
Cyc_List_List*Cyc_Std_rexplode(struct _RegionHandle*,struct _tagged_arr s);struct
_tagged_arr Cyc_Std_implode(struct Cyc_List_List*c);int Cyc_Std_strcasecmp(struct
_tagged_arr,struct _tagged_arr);int Cyc_Std_strncasecmp(struct _tagged_arr s1,struct
_tagged_arr s2,unsigned int len);void*Cyc_Std___assert_fail(struct _tagged_arr
assertion,struct _tagged_arr file,unsigned int line);char*strerror(int errnum);
struct _tagged_arr Cyc_Std_strerror(int errnum){return(struct _tagged_arr)
wrap_Cstring_as_string(strerror(errnum),- 1);}unsigned int Cyc_Std_strlen(struct
_tagged_arr s){unsigned int i;unsigned int sz=_get_arr_size(s,sizeof(char));for(i=0;
i < sz;i ++){if(((const char*)s.curr)[(int)i]== '\000')return i;}return i;}inline
static unsigned int Cyc_Std_int_strleno(struct _tagged_arr s,struct _tagged_arr error){
int i;unsigned int sz=_get_arr_size(s,sizeof(char));for(i=0;i < sz;i ++){if(((const
char*)s.curr)[i]== '\000')break;}return(unsigned int)i;}int Cyc_Std_strcmp(struct
_tagged_arr s1,struct _tagged_arr s2){if(s1.curr == s2.curr)return 0;{int i=0;
unsigned int sz1=_get_arr_size(s1,sizeof(char));unsigned int sz2=_get_arr_size(s2,
sizeof(char));unsigned int minsz=sz1 < sz2?sz1: sz2;(minsz <= sz1?minsz <= sz2: 0)?0:((
int(*)(struct _tagged_arr assertion,struct _tagged_arr file,unsigned int line))Cyc_Std___assert_fail)(({
const char*_tmp0="minsz <= sz1 && minsz <= sz2";_tag_arr(_tmp0,sizeof(char),
_get_zero_arr_size(_tmp0,29));}),({const char*_tmp1="string.cyc";_tag_arr(_tmp1,
sizeof(char),_get_zero_arr_size(_tmp1,11));}),84);while(i < minsz){char c1=*((
const char*)_check_unknown_subscript(s1,sizeof(char),i));char c2=*((const char*)
_check_unknown_subscript(s2,sizeof(char),i));if(c1 == '\000'){if(c2 == '\000')
return 0;else{return - 1;}}else{if(c2 == '\000')return 1;else{int diff=c1 - c2;if(diff
!= 0)return diff;}}++ i;}if(sz1 == sz2)return 0;if(minsz < sz2){if(*((const char*)
_check_unknown_subscript(s2,sizeof(char),i))== '\000')return 0;else{return - 1;}}
else{if(*((const char*)_check_unknown_subscript(s1,sizeof(char),i))== '\000')
return 0;else{return 1;}}}}int Cyc_Std_strptrcmp(struct _tagged_arr*s1,struct
_tagged_arr*s2){return Cyc_Std_strcmp(*((struct _tagged_arr*)s1),*((struct
_tagged_arr*)s2));}inline static int Cyc_Std_ncmp(struct _tagged_arr s1,unsigned int
len1,struct _tagged_arr s2,unsigned int len2,unsigned int n){if(n <= 0)return 0;{
unsigned int min_len=len1 > len2?len2: len1;unsigned int bound=min_len > n?n: min_len;(
bound <= _get_arr_size(s1,sizeof(char))?bound <= _get_arr_size(s2,sizeof(char)): 0)?
0:((int(*)(struct _tagged_arr assertion,struct _tagged_arr file,unsigned int line))
Cyc_Std___assert_fail)(({const char*_tmp2="bound <= s1.size && bound <= s2.size";
_tag_arr(_tmp2,sizeof(char),_get_zero_arr_size(_tmp2,37));}),({const char*_tmp3="string.cyc";
_tag_arr(_tmp3,sizeof(char),_get_zero_arr_size(_tmp3,11));}),120);{int i=0;for(0;
i < bound;i ++){int retc;if((retc=((const char*)s1.curr)[i]- ((const char*)s2.curr)[i])
!= 0)return retc;}}if(len1 < n?1: len2 < n)return(int)len1 - (int)len2;return 0;}}int
Cyc_Std_strncmp(struct _tagged_arr s1,struct _tagged_arr s2,unsigned int n){
unsigned int len1=Cyc_Std_int_strleno(s1,({const char*_tmp5="Std::strncmp";
_tag_arr(_tmp5,sizeof(char),_get_zero_arr_size(_tmp5,13));}));unsigned int len2=
Cyc_Std_int_strleno(s2,({const char*_tmp4="Std::strncmp";_tag_arr(_tmp4,sizeof(
char),_get_zero_arr_size(_tmp4,13));}));return Cyc_Std_ncmp(s1,len1,s2,len2,n);}
int Cyc_Std_zstrcmp(struct _tagged_arr a,struct _tagged_arr b){if(a.curr == b.curr)
return 0;{unsigned int as=_get_arr_size(a,sizeof(char));unsigned int bs=
_get_arr_size(b,sizeof(char));unsigned int min_length=as < bs?as: bs;int i=- 1;(
min_length <= _get_arr_size(a,sizeof(char))?min_length <= _get_arr_size(b,sizeof(
char)): 0)?0:((int(*)(struct _tagged_arr assertion,struct _tagged_arr file,
unsigned int line))Cyc_Std___assert_fail)(({const char*_tmp6="min_length <= a.size && min_length <= b.size";
_tag_arr(_tmp6,sizeof(char),_get_zero_arr_size(_tmp6,45));}),({const char*_tmp7="string.cyc";
_tag_arr(_tmp7,sizeof(char),_get_zero_arr_size(_tmp7,11));}),152);while((++ i,i < 
min_length)){int diff=(int)((const char*)a.curr)[i]- (int)((const char*)b.curr)[i];
if(diff != 0)return diff;}return(int)as - (int)bs;}}int Cyc_Std_zstrncmp(struct
_tagged_arr s1,struct _tagged_arr s2,unsigned int n){if(n <= 0)return 0;{unsigned int
s1size=_get_arr_size(s1,sizeof(char));unsigned int s2size=_get_arr_size(s2,
sizeof(char));unsigned int min_size=s1size > s2size?s2size: s1size;unsigned int
bound=min_size > n?n: min_size;(bound <= _get_arr_size(s1,sizeof(char))?bound <= 
_get_arr_size(s2,sizeof(char)): 0)?0:((int(*)(struct _tagged_arr assertion,struct
_tagged_arr file,unsigned int line))Cyc_Std___assert_fail)(({const char*_tmp8="bound <= s1.size && bound <= s2.size";
_tag_arr(_tmp8,sizeof(char),_get_zero_arr_size(_tmp8,37));}),({const char*_tmp9="string.cyc";
_tag_arr(_tmp9,sizeof(char),_get_zero_arr_size(_tmp9,11));}),170);{int i=0;for(0;
i < bound;i ++){if(((const char*)s1.curr)[i]< ((const char*)s2.curr)[i])return - 1;
else{if(((const char*)s2.curr)[i]< ((const char*)s1.curr)[i])return 1;}}}if(
min_size <= bound)return 0;if(s1size < s2size)return - 1;else{return 1;}}}int Cyc_Std_zstrptrcmp(
struct _tagged_arr*a,struct _tagged_arr*b){if((int)a == (int)b)return 0;return Cyc_Std_zstrcmp(*((
struct _tagged_arr*)a),*((struct _tagged_arr*)b));}inline static struct _tagged_arr
Cyc_Std_int_strcato(struct _tagged_arr dest,struct _tagged_arr src,struct _tagged_arr
error){int i;unsigned int dsize;unsigned int slen;unsigned int dlen;dsize=
_get_arr_size(dest,sizeof(char));dlen=Cyc_Std_strlen((struct _tagged_arr)dest);
slen=Cyc_Std_int_strleno(src,error);if(slen + dlen <= dsize){slen <= _get_arr_size(
src,sizeof(char))?0:((int(*)(struct _tagged_arr assertion,struct _tagged_arr file,
unsigned int line))Cyc_Std___assert_fail)(({const char*_tmpA="slen <= src.size";
_tag_arr(_tmpA,sizeof(char),_get_zero_arr_size(_tmpA,17));}),({const char*_tmpB="string.cyc";
_tag_arr(_tmpB,sizeof(char),_get_zero_arr_size(_tmpB,11));}),206);for(i=0;i < 
slen;i ++){({struct _tagged_arr _tmpC=_tagged_arr_plus(dest,sizeof(char),(int)(i + 
dlen));char _tmpD=*((char*)_check_unknown_subscript(_tmpC,sizeof(char),0));char
_tmpE=((const char*)src.curr)[i];if(_get_arr_size(_tmpC,sizeof(char))== 1?_tmpD == '\000'?
_tmpE != '\000': 0: 0)_throw_arraybounds();*((char*)_tmpC.curr)=_tmpE;});}if(i != 
dsize)({struct _tagged_arr _tmpF=_tagged_arr_plus(dest,sizeof(char),(int)(i + dlen));
char _tmp10=*((char*)_check_unknown_subscript(_tmpF,sizeof(char),0));char _tmp11='\000';
if(_get_arr_size(_tmpF,sizeof(char))== 1?_tmp10 == '\000'?_tmp11 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmpF.curr)=_tmp11;});}else{(int)_throw((void*)({
struct Cyc_Core_Invalid_argument_struct*_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12[
0]=({struct Cyc_Core_Invalid_argument_struct _tmp13;_tmp13.tag=Cyc_Core_Invalid_argument;
_tmp13.f1=error;_tmp13;});_tmp12;}));}return dest;}struct _tagged_arr Cyc_Std_strcat(
struct _tagged_arr dest,struct _tagged_arr src){return Cyc_Std_int_strcato(dest,src,({
const char*_tmp14="Std::strcat";_tag_arr(_tmp14,sizeof(char),_get_zero_arr_size(
_tmp14,12));}));}struct _tagged_arr Cyc_Std_rstrconcat(struct _RegionHandle*r,
struct _tagged_arr a,struct _tagged_arr b){unsigned int _tmp15=Cyc_Std_strlen(a);
unsigned int _tmp16=Cyc_Std_strlen(b);struct _tagged_arr ans=Cyc_Core_rnew_string(r,(
_tmp15 + _tmp16)+ 1);int i;int j;(_tmp15 <= _get_arr_size(ans,sizeof(char))?_tmp15 <= 
_get_arr_size(a,sizeof(char)): 0)?0:((int(*)(struct _tagged_arr assertion,struct
_tagged_arr file,unsigned int line))Cyc_Std___assert_fail)(({const char*_tmp17="alen <= ans.size && alen <= a.size";
_tag_arr(_tmp17,sizeof(char),_get_zero_arr_size(_tmp17,35));}),({const char*
_tmp18="string.cyc";_tag_arr(_tmp18,sizeof(char),_get_zero_arr_size(_tmp18,11));}),
230);for(i=0;i < _tmp15;++ i){({struct _tagged_arr _tmp19=_tagged_arr_plus(ans,
sizeof(char),i);char _tmp1A=*((char*)_check_unknown_subscript(_tmp19,sizeof(char),
0));char _tmp1B=((const char*)a.curr)[i];if(_get_arr_size(_tmp19,sizeof(char))== 1?
_tmp1A == '\000'?_tmp1B != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp19.curr)=
_tmp1B;});}_tmp16 <= _get_arr_size(b,sizeof(char))?0:((int(*)(struct _tagged_arr
assertion,struct _tagged_arr file,unsigned int line))Cyc_Std___assert_fail)(({const
char*_tmp1C="blen <= b.size";_tag_arr(_tmp1C,sizeof(char),_get_zero_arr_size(
_tmp1C,15));}),({const char*_tmp1D="string.cyc";_tag_arr(_tmp1D,sizeof(char),
_get_zero_arr_size(_tmp1D,11));}),232);for(j=0;j < _tmp16;++ j){({struct
_tagged_arr _tmp1E=_tagged_arr_plus(ans,sizeof(char),i + j);char _tmp1F=*((char*)
_check_unknown_subscript(_tmp1E,sizeof(char),0));char _tmp20=((const char*)b.curr)[
j];if(_get_arr_size(_tmp1E,sizeof(char))== 1?_tmp1F == '\000'?_tmp20 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp1E.curr)=_tmp20;});}return ans;}struct
_tagged_arr Cyc_Std_strconcat(struct _tagged_arr a,struct _tagged_arr b){return Cyc_Std_rstrconcat(
Cyc_Core_heap_region,a,b);}struct _tagged_arr Cyc_Std_rstrconcat_l(struct
_RegionHandle*r,struct Cyc_List_List*strs){unsigned int len;unsigned int total_len=
0;struct _tagged_arr ans;{struct _RegionHandle _tmp21=_new_region("temp");struct
_RegionHandle*temp=& _tmp21;_push_region(temp);{struct Cyc_List_List*lens=({struct
Cyc_List_List*_tmp24=_region_malloc(temp,sizeof(*_tmp24));_tmp24->hd=(void*)((
unsigned int)0);_tmp24->tl=0;_tmp24;});struct Cyc_List_List*end=lens;{struct Cyc_List_List*
p=strs;for(0;p != 0;p=p->tl){len=Cyc_Std_strlen(*((struct _tagged_arr*)((struct
_tagged_arr*)p->hd)));total_len +=len;((struct Cyc_List_List*)_check_null(end))->tl=({
struct Cyc_List_List*_tmp22=_region_malloc(temp,sizeof(*_tmp22));_tmp22->hd=(void*)
len;_tmp22->tl=0;_tmp22;});end=((struct Cyc_List_List*)_check_null(end))->tl;}}
lens=lens->tl;ans=Cyc_Core_rnew_string(r,total_len + 1);{unsigned int i=0;while(
strs != 0){struct _tagged_arr _tmp23=*((struct _tagged_arr*)((struct _tagged_arr*)
strs->hd));len=(unsigned int)((struct Cyc_List_List*)_check_null(lens))->hd;Cyc_Std_strncpy(
_tagged_arr_plus(ans,sizeof(char),(int)i),_tmp23,len);i +=len;strs=strs->tl;lens=
lens->tl;}}};_pop_region(temp);}return ans;}struct _tagged_arr Cyc_Std_strconcat_l(
struct Cyc_List_List*strs){return Cyc_Std_rstrconcat_l(Cyc_Core_heap_region,strs);}
struct _tagged_arr Cyc_Std_rstr_sepstr(struct _RegionHandle*r,struct Cyc_List_List*
strs,struct _tagged_arr separator){if(strs == 0)return Cyc_Core_rnew_string(r,0);if(
strs->tl == 0)return Cyc_Std_rstrdup(r,*((struct _tagged_arr*)((struct _tagged_arr*)
strs->hd)));{struct Cyc_List_List*_tmp25=strs;struct _RegionHandle _tmp26=
_new_region("temp");struct _RegionHandle*temp=& _tmp26;_push_region(temp);{struct
Cyc_List_List*lens=({struct Cyc_List_List*_tmp2A=_region_malloc(temp,sizeof(*
_tmp2A));_tmp2A->hd=(void*)((unsigned int)0);_tmp2A->tl=0;_tmp2A;});struct Cyc_List_List*
end=lens;unsigned int len;unsigned int total_len=0;unsigned int list_len=0;for(0;
_tmp25 != 0;_tmp25=_tmp25->tl){len=Cyc_Std_strlen(*((struct _tagged_arr*)((struct
_tagged_arr*)_tmp25->hd)));total_len +=len;((struct Cyc_List_List*)_check_null(end))->tl=({
struct Cyc_List_List*_tmp27=_region_malloc(temp,sizeof(*_tmp27));_tmp27->hd=(void*)
len;_tmp27->tl=0;_tmp27;});end=((struct Cyc_List_List*)_check_null(end))->tl;++
list_len;}lens=lens->tl;{unsigned int seplen=Cyc_Std_strlen(separator);total_len +=(
list_len - 1)* seplen;{struct _tagged_arr ans=Cyc_Core_rnew_string(r,total_len + 1);
unsigned int i=0;while(strs->tl != 0){struct _tagged_arr _tmp28=*((struct _tagged_arr*)((
struct _tagged_arr*)strs->hd));len=(unsigned int)((struct Cyc_List_List*)
_check_null(lens))->hd;Cyc_Std_strncpy(_tagged_arr_plus(ans,sizeof(char),(int)i),
_tmp28,len);i +=len;Cyc_Std_strncpy(_tagged_arr_plus(ans,sizeof(char),(int)i),
separator,seplen);i +=seplen;strs=strs->tl;lens=lens->tl;}Cyc_Std_strncpy(
_tagged_arr_plus(ans,sizeof(char),(int)i),*((struct _tagged_arr*)((struct
_tagged_arr*)strs->hd)),(unsigned int)((struct Cyc_List_List*)_check_null(lens))->hd);{
struct _tagged_arr _tmp29=ans;_npop_handler(0);return _tmp29;}}}};_pop_region(temp);}}
struct _tagged_arr Cyc_Std_str_sepstr(struct Cyc_List_List*strs,struct _tagged_arr
separator){return Cyc_Std_rstr_sepstr(Cyc_Core_heap_region,strs,separator);}
struct _tagged_arr Cyc_Std_strncpy(struct _tagged_arr dest,struct _tagged_arr src,
unsigned int n){int i;(n <= _get_arr_size(src,sizeof(char))?n <= _get_arr_size(dest,
sizeof(char)): 0)?0:((int(*)(struct _tagged_arr assertion,struct _tagged_arr file,
unsigned int line))Cyc_Std___assert_fail)(({const char*_tmp2B="n <= src.size && n <= dest.size";
_tag_arr(_tmp2B,sizeof(char),_get_zero_arr_size(_tmp2B,32));}),({const char*
_tmp2C="string.cyc";_tag_arr(_tmp2C,sizeof(char),_get_zero_arr_size(_tmp2C,11));}),
323);for(i=0;i < n;i ++){char _tmp2D=((const char*)src.curr)[i];if(_tmp2D == '\000')
break;({struct _tagged_arr _tmp2E=_tagged_arr_plus(dest,sizeof(char),i);char _tmp2F=*((
char*)_check_unknown_subscript(_tmp2E,sizeof(char),0));char _tmp30=_tmp2D;if(
_get_arr_size(_tmp2E,sizeof(char))== 1?_tmp2F == '\000'?_tmp30 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp2E.curr)=_tmp30;});}for(0;i < n;i ++){({struct
_tagged_arr _tmp31=_tagged_arr_plus(dest,sizeof(char),i);char _tmp32=*((char*)
_check_unknown_subscript(_tmp31,sizeof(char),0));char _tmp33='\000';if(
_get_arr_size(_tmp31,sizeof(char))== 1?_tmp32 == '\000'?_tmp33 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp31.curr)=_tmp33;});}return dest;}struct
_tagged_arr Cyc_Std_zstrncpy(struct _tagged_arr dest,struct _tagged_arr src,
unsigned int n){(n <= _get_arr_size(dest,sizeof(char))?n <= _get_arr_size(src,
sizeof(char)): 0)?0:((int(*)(struct _tagged_arr assertion,struct _tagged_arr file,
unsigned int line))Cyc_Std___assert_fail)(({const char*_tmp34="n <= dest.size && n <= src.size";
_tag_arr(_tmp34,sizeof(char),_get_zero_arr_size(_tmp34,32));}),({const char*
_tmp35="string.cyc";_tag_arr(_tmp35,sizeof(char),_get_zero_arr_size(_tmp35,11));}),
337);{int i;for(i=0;i < n;i ++){({struct _tagged_arr _tmp36=_tagged_arr_plus(dest,
sizeof(char),i);char _tmp37=*((char*)_check_unknown_subscript(_tmp36,sizeof(char),
0));char _tmp38=((const char*)src.curr)[i];if(_get_arr_size(_tmp36,sizeof(char))== 
1?_tmp37 == '\000'?_tmp38 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp36.curr)=
_tmp38;});}return dest;}}struct _tagged_arr Cyc_Std_strcpy(struct _tagged_arr dest,
struct _tagged_arr src){unsigned int ssz=_get_arr_size(src,sizeof(char));
unsigned int dsz=_get_arr_size(dest,sizeof(char));if(ssz <= dsz){unsigned int i;
for(i=0;i < ssz;i ++){char _tmp39=((const char*)src.curr)[(int)i];({struct
_tagged_arr _tmp3A=_tagged_arr_plus(dest,sizeof(char),(int)i);char _tmp3B=*((char*)
_check_unknown_subscript(_tmp3A,sizeof(char),0));char _tmp3C=_tmp39;if(
_get_arr_size(_tmp3A,sizeof(char))== 1?_tmp3B == '\000'?_tmp3C != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp3A.curr)=_tmp3C;});if(_tmp39 == '\000')break;}}
else{unsigned int len=Cyc_Std_strlen(src);Cyc_Std_strncpy(dest,src,len);if(len < 
_get_arr_size(dest,sizeof(char)))({struct _tagged_arr _tmp3D=_tagged_arr_plus(dest,
sizeof(char),(int)len);char _tmp3E=*((char*)_check_unknown_subscript(_tmp3D,
sizeof(char),0));char _tmp3F='\000';if(_get_arr_size(_tmp3D,sizeof(char))== 1?
_tmp3E == '\000'?_tmp3F != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp3D.curr)=
_tmp3F;});}return dest;}struct _tagged_arr Cyc_Std_rstrdup(struct _RegionHandle*r,
struct _tagged_arr src){unsigned int len;struct _tagged_arr temp;len=Cyc_Std_strlen(
src);temp=Cyc_Core_rnew_string(r,len + 1);Cyc_Std_strncpy(temp,src,len);return
temp;}struct _tagged_arr Cyc_Std_strdup(struct _tagged_arr src){return Cyc_Std_rstrdup(
Cyc_Core_heap_region,src);}struct _tagged_arr Cyc_Std_rexpand(struct _RegionHandle*
r,struct _tagged_arr s,unsigned int sz){struct _tagged_arr temp;unsigned int slen;slen=
Cyc_Std_strlen(s);sz=sz > slen?sz: slen;temp=Cyc_Core_rnew_string(r,sz);Cyc_Std_strncpy(
temp,s,slen);if(slen != _get_arr_size(s,sizeof(char)))({struct _tagged_arr _tmp40=
_tagged_arr_plus(temp,sizeof(char),(int)slen);char _tmp41=*((char*)
_check_unknown_subscript(_tmp40,sizeof(char),0));char _tmp42='\000';if(
_get_arr_size(_tmp40,sizeof(char))== 1?_tmp41 == '\000'?_tmp42 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp40.curr)=_tmp42;});return temp;}struct
_tagged_arr Cyc_Std_expand(struct _tagged_arr s,unsigned int sz){return Cyc_Std_rexpand(
Cyc_Core_heap_region,s,sz);}struct _tagged_arr Cyc_Std_rrealloc_str(struct
_RegionHandle*r,struct _tagged_arr str,unsigned int sz){unsigned int maxsizeP=
_get_arr_size(str,sizeof(char));if(maxsizeP == 0){maxsizeP=30 > sz?30: sz;str=Cyc_Core_rnew_string(
r,maxsizeP);({struct _tagged_arr _tmp43=_tagged_arr_plus(str,sizeof(char),0);char
_tmp44=*((char*)_check_unknown_subscript(_tmp43,sizeof(char),0));char _tmp45='\000';
if(_get_arr_size(_tmp43,sizeof(char))== 1?_tmp44 == '\000'?_tmp45 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp43.curr)=_tmp45;});}else{if(sz > maxsizeP){
maxsizeP=maxsizeP * 2 > (sz * 5)/ 4?maxsizeP * 2:(sz * 5)/ 4;str=Cyc_Std_rexpand(r,(
struct _tagged_arr)str,maxsizeP);}}return str;}struct _tagged_arr Cyc_Std_realloc_str(
struct _tagged_arr str,unsigned int sz){return Cyc_Std_rrealloc_str(Cyc_Core_heap_region,
str,sz);}struct _tagged_arr Cyc_Std_rsubstring(struct _RegionHandle*r,struct
_tagged_arr s,int start,unsigned int amt){struct _tagged_arr ans=Cyc_Core_rnew_string(
r,amt + 1);s=_tagged_arr_plus(s,sizeof(char),start);(amt < _get_arr_size(ans,
sizeof(char))?amt <= _get_arr_size(s,sizeof(char)): 0)?0:((int(*)(struct
_tagged_arr assertion,struct _tagged_arr file,unsigned int line))Cyc_Std___assert_fail)(({
const char*_tmp46="amt < ans.size && amt <= s.size";_tag_arr(_tmp46,sizeof(char),
_get_zero_arr_size(_tmp46,32));}),({const char*_tmp47="string.cyc";_tag_arr(
_tmp47,sizeof(char),_get_zero_arr_size(_tmp47,11));}),433);{unsigned int i=0;for(
0;i < amt;++ i){({struct _tagged_arr _tmp48=_tagged_arr_plus(ans,sizeof(char),(int)i);
char _tmp49=*((char*)_check_unknown_subscript(_tmp48,sizeof(char),0));char _tmp4A=((
const char*)s.curr)[(int)i];if(_get_arr_size(_tmp48,sizeof(char))== 1?_tmp49 == '\000'?
_tmp4A != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp48.curr)=_tmp4A;});}}({
struct _tagged_arr _tmp4B=_tagged_arr_plus(ans,sizeof(char),(int)amt);char _tmp4C=*((
char*)_check_unknown_subscript(_tmp4B,sizeof(char),0));char _tmp4D='\000';if(
_get_arr_size(_tmp4B,sizeof(char))== 1?_tmp4C == '\000'?_tmp4D != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp4B.curr)=_tmp4D;});return ans;}struct
_tagged_arr Cyc_Std_substring(struct _tagged_arr s,int start,unsigned int amt){return
Cyc_Std_rsubstring(Cyc_Core_heap_region,s,start,amt);}struct _tagged_arr Cyc_Std_rreplace_suffix(
struct _RegionHandle*r,struct _tagged_arr src,struct _tagged_arr curr_suffix,struct
_tagged_arr new_suffix){unsigned int m=_get_arr_size(src,sizeof(char));
unsigned int n=_get_arr_size(curr_suffix,sizeof(char));struct _tagged_arr err=({
const char*_tmp52="Std::replace_suffix";_tag_arr(_tmp52,sizeof(char),
_get_zero_arr_size(_tmp52,20));});if(m < n)(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp4F;_tmp4F.tag=Cyc_Core_Invalid_argument;_tmp4F.f1=err;_tmp4F;});_tmp4E;}));{
unsigned int i=1;for(0;i <= n;++ i){if(*((const char*)_check_unknown_subscript(src,
sizeof(char),(int)(m - i)))!= *((const char*)_check_unknown_subscript(curr_suffix,
sizeof(char),(int)(n - i))))(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp51;_tmp51.tag=Cyc_Core_Invalid_argument;_tmp51.f1=err;_tmp51;});_tmp50;}));}}{
struct _tagged_arr ans=Cyc_Core_rnew_string(r,(m - n)+ _get_arr_size(new_suffix,
sizeof(char)));Cyc_Std_strncpy(ans,src,m - n);Cyc_Std_strncpy(_tagged_arr_plus(
_tagged_arr_plus(ans,sizeof(char),(int)m),sizeof(char),-(int)n),new_suffix,
_get_arr_size(new_suffix,sizeof(char)));return ans;}}struct _tagged_arr Cyc_Std_replace_suffix(
struct _tagged_arr src,struct _tagged_arr curr_suffix,struct _tagged_arr new_suffix){
return Cyc_Std_rreplace_suffix(Cyc_Core_heap_region,src,curr_suffix,new_suffix);}
struct _tagged_arr Cyc_Std_strpbrk(struct _tagged_arr s,struct _tagged_arr accept){int
len=(int)_get_arr_size(s,sizeof(char));unsigned int asize=_get_arr_size(accept,
sizeof(char));char c;unsigned int i;for(i=0;i < len?(c=((const char*)s.curr)[(int)i])
!= 0: 0;i ++){unsigned int j=0;for(0;j < asize;j ++){if(c == ((const char*)accept.curr)[(
int)j])return _tagged_arr_plus(s,sizeof(char),(int)i);}}return(struct _tagged_arr)
_tag_arr(0,0,0);}struct _tagged_arr Cyc_Std_mstrpbrk(struct _tagged_arr s,struct
_tagged_arr accept){int len=(int)_get_arr_size(s,sizeof(char));unsigned int asize=
_get_arr_size(accept,sizeof(char));char c;unsigned int i;for(i=0;i < len?(c=((char*)
s.curr)[(int)i])!= 0: 0;i ++){unsigned int j=0;for(0;j < asize;j ++){if(c == ((const
char*)accept.curr)[(int)j])return _tagged_arr_plus(s,sizeof(char),(int)i);}}
return _tag_arr(0,0,0);}struct _tagged_arr Cyc_Std_mstrchr(struct _tagged_arr s,char c){
int len=(int)_get_arr_size(s,sizeof(char));char c2;unsigned int i;for(i=0;i < len?(
c2=((char*)s.curr)[(int)i])!= 0: 0;i ++){if(c2 == c)return _tagged_arr_plus(s,
sizeof(char),(int)i);}return _tag_arr(0,0,0);}struct _tagged_arr Cyc_Std_strchr(
struct _tagged_arr s,char c){int len=(int)_get_arr_size(s,sizeof(char));char c2;
unsigned int i;for(i=0;i < len?(c2=((const char*)s.curr)[(int)i])!= 0: 0;i ++){if(c2
== c)return _tagged_arr_plus(s,sizeof(char),(int)i);}return(struct _tagged_arr)
_tag_arr(0,0,0);}struct _tagged_arr Cyc_Std_strrchr(struct _tagged_arr s,char c){int
len=(int)Cyc_Std_int_strleno(s,({const char*_tmp53="Std::strrchr";_tag_arr(_tmp53,
sizeof(char),_get_zero_arr_size(_tmp53,13));}));int i=len - 1;
_tagged_arr_inplace_plus(& s,sizeof(char),i);for(0;i >= 0;(i --,
_tagged_arr_inplace_plus_post(& s,sizeof(char),-1))){if(*((const char*)
_check_unknown_subscript(s,sizeof(char),0))== c)return s;}return(struct
_tagged_arr)_tag_arr(0,0,0);}struct _tagged_arr Cyc_Std_mstrrchr(struct _tagged_arr
s,char c){int len=(int)Cyc_Std_int_strleno((struct _tagged_arr)s,({const char*_tmp54="Std::mstrrchr";
_tag_arr(_tmp54,sizeof(char),_get_zero_arr_size(_tmp54,14));}));int i=len - 1;
_tagged_arr_inplace_plus(& s,sizeof(char),i);for(0;i >= 0;(i --,
_tagged_arr_inplace_plus_post(& s,sizeof(char),-1))){if(*((char*)
_check_unknown_subscript(s,sizeof(char),0))== c)return s;}return _tag_arr(0,0,0);}
struct _tagged_arr Cyc_Std_strstr(struct _tagged_arr haystack,struct _tagged_arr
needle){if(!((unsigned int)haystack.curr)?1: !((unsigned int)needle.curr))(int)
_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp55=_cycalloc(sizeof(*
_tmp55));_tmp55[0]=({struct Cyc_Core_Invalid_argument_struct _tmp56;_tmp56.tag=Cyc_Core_Invalid_argument;
_tmp56.f1=({const char*_tmp57="Std::strstr";_tag_arr(_tmp57,sizeof(char),
_get_zero_arr_size(_tmp57,12));});_tmp56;});_tmp55;}));if(*((const char*)
_check_unknown_subscript(needle,sizeof(char),0))== '\000')return haystack;{int len=(
int)Cyc_Std_int_strleno(needle,({const char*_tmp58="Std::strstr";_tag_arr(_tmp58,
sizeof(char),_get_zero_arr_size(_tmp58,12));}));{struct _tagged_arr start=haystack;
for(0;(start=Cyc_Std_strchr(start,*((const char*)_check_unknown_subscript(needle,
sizeof(char),0)))).curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr;start=Cyc_Std_strchr(
_tagged_arr_plus(start,sizeof(char),1),*((const char*)_check_unknown_subscript(
needle,sizeof(char),0)))){if(Cyc_Std_strncmp(start,needle,(unsigned int)len)== 0)
return start;}}return(struct _tagged_arr)_tag_arr(0,0,0);}}struct _tagged_arr Cyc_Std_mstrstr(
struct _tagged_arr haystack,struct _tagged_arr needle){if(!((unsigned int)haystack.curr)?
1: !((unsigned int)needle.curr))(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp5A;_tmp5A.tag=Cyc_Core_Invalid_argument;_tmp5A.f1=({const char*_tmp5B="Std::mstrstr";
_tag_arr(_tmp5B,sizeof(char),_get_zero_arr_size(_tmp5B,13));});_tmp5A;});_tmp59;}));
if(*((const char*)_check_unknown_subscript(needle,sizeof(char),0))== '\000')
return haystack;{int len=(int)Cyc_Std_int_strleno(needle,({const char*_tmp5C="Std::mstrstr";
_tag_arr(_tmp5C,sizeof(char),_get_zero_arr_size(_tmp5C,13));}));{struct
_tagged_arr start=haystack;for(0;(start=Cyc_Std_mstrchr(start,*((const char*)
_check_unknown_subscript(needle,sizeof(char),0)))).curr != (_tag_arr(0,0,0)).curr;
start=Cyc_Std_mstrchr(_tagged_arr_plus(start,sizeof(char),1),*((const char*)
_check_unknown_subscript(needle,sizeof(char),0)))){if(Cyc_Std_strncmp((struct
_tagged_arr)start,needle,(unsigned int)len)== 0)return start;}}return _tag_arr(0,0,
0);}}unsigned int Cyc_Std_strspn(struct _tagged_arr s,struct _tagged_arr accept){
unsigned int len=Cyc_Std_int_strleno(s,({const char*_tmp5F="Std::strspn";_tag_arr(
_tmp5F,sizeof(char),_get_zero_arr_size(_tmp5F,12));}));unsigned int asize=
_get_arr_size(accept,sizeof(char));len <= _get_arr_size(s,sizeof(char))?0:((int(*)(
struct _tagged_arr assertion,struct _tagged_arr file,unsigned int line))Cyc_Std___assert_fail)(({
const char*_tmp5D="len <= s.size";_tag_arr(_tmp5D,sizeof(char),_get_zero_arr_size(
_tmp5D,14));}),({const char*_tmp5E="string.cyc";_tag_arr(_tmp5E,sizeof(char),
_get_zero_arr_size(_tmp5E,11));}),581);{unsigned int i=0;for(0;i < len;i ++){int j;
for(j=0;j < asize;j ++){if(((const char*)s.curr)[(int)i]== ((const char*)accept.curr)[
j])break;}if(j == asize)return i;}}return len;}unsigned int Cyc_Std_strcspn(struct
_tagged_arr s,struct _tagged_arr accept){unsigned int len=Cyc_Std_int_strleno(s,({
const char*_tmp62="Std::strspn";_tag_arr(_tmp62,sizeof(char),_get_zero_arr_size(
_tmp62,12));}));unsigned int asize=_get_arr_size(accept,sizeof(char));len <= 
_get_arr_size(s,sizeof(char))?0:((int(*)(struct _tagged_arr assertion,struct
_tagged_arr file,unsigned int line))Cyc_Std___assert_fail)(({const char*_tmp60="len <= s.size";
_tag_arr(_tmp60,sizeof(char),_get_zero_arr_size(_tmp60,14));}),({const char*
_tmp61="string.cyc";_tag_arr(_tmp61,sizeof(char),_get_zero_arr_size(_tmp61,11));}),
601);{unsigned int i=0;for(0;i < len;i ++){int j;for(j=0;j < asize;j ++){if(((const
char*)s.curr)[(int)i]!= ((const char*)accept.curr)[j])break;}if(j == asize)return i;}}
return len;}struct _tagged_arr Cyc_Std_strtok(struct _tagged_arr s,struct _tagged_arr
delim){static struct _tagged_arr olds={(void*)0,(void*)0,(void*)(0 + 0)};struct
_tagged_arr token;if(s.curr == (_tag_arr(0,0,0)).curr){if(olds.curr == (_tag_arr(0,
0,0)).curr)return _tag_arr(0,0,0);s=olds;}{unsigned int inc=Cyc_Std_strspn((struct
_tagged_arr)s,delim);if(inc >= _get_arr_size(s,sizeof(char))?1:*((char*)
_check_unknown_subscript(_tagged_arr_plus(s,sizeof(char),(int)inc),sizeof(char),
0))== '\000'){olds=_tag_arr(0,0,0);return _tag_arr(0,0,0);}else{
_tagged_arr_inplace_plus(& s,sizeof(char),(int)inc);}token=s;s=Cyc_Std_mstrpbrk(
token,delim);if(s.curr == (_tag_arr(0,0,0)).curr)olds=_tag_arr(0,0,0);else{({
struct _tagged_arr _tmp63=s;char _tmp64=*((char*)_check_unknown_subscript(_tmp63,
sizeof(char),0));char _tmp65='\000';if(_get_arr_size(_tmp63,sizeof(char))== 1?
_tmp64 == '\000'?_tmp65 != '\000': 0: 0)_throw_arraybounds();*((char*)_tmp63.curr)=
_tmp65;});olds=_tagged_arr_plus(s,sizeof(char),1);}return token;}}struct Cyc_List_List*
Cyc_Std_rexplode(struct _RegionHandle*r,struct _tagged_arr s){struct Cyc_List_List*
result=0;{int i=(int)(Cyc_Std_strlen(s)- 1);for(0;i >= 0;i --){result=({struct Cyc_List_List*
_tmp66=_region_malloc(r,sizeof(*_tmp66));_tmp66->hd=(void*)((int)*((const char*)
_check_unknown_subscript(s,sizeof(char),i)));_tmp66->tl=result;_tmp66;});}}
return result;}struct Cyc_List_List*Cyc_Std_explode(struct _tagged_arr s){return Cyc_Std_rexplode(
Cyc_Core_heap_region,s);}struct _tagged_arr Cyc_Std_implode(struct Cyc_List_List*
chars){struct _tagged_arr s=Cyc_Core_new_string((unsigned int)(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(chars)+ 1));unsigned int i=0;while(chars != 0){({struct
_tagged_arr _tmp67=_tagged_arr_plus(s,sizeof(char),(int)i ++);char _tmp68=*((char*)
_check_unknown_subscript(_tmp67,sizeof(char),0));char _tmp69=(char)((int)chars->hd);
if(_get_arr_size(_tmp67,sizeof(char))== 1?_tmp68 == '\000'?_tmp69 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp67.curr)=_tmp69;});chars=chars->tl;}return s;}
inline static int Cyc_Std_casecmp(struct _tagged_arr s1,unsigned int len1,struct
_tagged_arr s2,unsigned int len2){unsigned int min_length=len1 < len2?len1: len2;(
min_length <= _get_arr_size(s1,sizeof(char))?min_length <= _get_arr_size(s2,
sizeof(char)): 0)?0:((int(*)(struct _tagged_arr assertion,struct _tagged_arr file,
unsigned int line))Cyc_Std___assert_fail)(({const char*_tmp6A="min_length <= s1.size && min_length <= s2.size";
_tag_arr(_tmp6A,sizeof(char),_get_zero_arr_size(_tmp6A,47));}),({const char*
_tmp6B="string.cyc";_tag_arr(_tmp6B,sizeof(char),_get_zero_arr_size(_tmp6B,11));}),
682);{int i=- 1;while((++ i,i < min_length)){int diff=toupper((int)((const char*)s1.curr)[
i])- toupper((int)((const char*)s2.curr)[i]);if(diff != 0)return diff;}return(int)
len1 - (int)len2;}}int Cyc_Std_strcasecmp(struct _tagged_arr s1,struct _tagged_arr s2){
if(s1.curr == s2.curr)return 0;{unsigned int len1=Cyc_Std_int_strleno(s1,({const
char*_tmp6D="Std::strcasecmp";_tag_arr(_tmp6D,sizeof(char),_get_zero_arr_size(
_tmp6D,16));}));unsigned int len2=Cyc_Std_int_strleno(s2,({const char*_tmp6C="Std::strcasecmp";
_tag_arr(_tmp6C,sizeof(char),_get_zero_arr_size(_tmp6C,16));}));return Cyc_Std_casecmp(
s1,len1,s2,len2);}}inline static int Cyc_Std_caseless_ncmp(struct _tagged_arr s1,
unsigned int len1,struct _tagged_arr s2,unsigned int len2,unsigned int n){if(n <= 0)
return 0;{unsigned int min_len=len1 > len2?len2: len1;unsigned int bound=min_len > n?n:
min_len;(bound <= _get_arr_size(s1,sizeof(char))?bound <= _get_arr_size(s2,sizeof(
char)): 0)?0:((int(*)(struct _tagged_arr assertion,struct _tagged_arr file,
unsigned int line))Cyc_Std___assert_fail)(({const char*_tmp6E="bound <= s1.size && bound <= s2.size";
_tag_arr(_tmp6E,sizeof(char),_get_zero_arr_size(_tmp6E,37));}),({const char*
_tmp6F="string.cyc";_tag_arr(_tmp6F,sizeof(char),_get_zero_arr_size(_tmp6F,11));}),
709);{int i=0;for(0;i < bound;i ++){int retc;if((retc=toupper((int)((const char*)s1.curr)[
i])- toupper((int)((const char*)s2.curr)[i]))!= 0)return retc;}}if(len1 < n?1: len2 < 
n)return(int)len1 - (int)len2;return 0;}}int Cyc_Std_strncasecmp(struct _tagged_arr
s1,struct _tagged_arr s2,unsigned int n){unsigned int len1=Cyc_Std_int_strleno(s1,({
const char*_tmp71="Std::strncasecmp";_tag_arr(_tmp71,sizeof(char),
_get_zero_arr_size(_tmp71,17));}));unsigned int len2=Cyc_Std_int_strleno(s2,({
const char*_tmp70="Std::strncasecmp";_tag_arr(_tmp70,sizeof(char),
_get_zero_arr_size(_tmp70,17));}));return Cyc_Std_caseless_ncmp(s1,len1,s2,len2,n);}
void*memcpy(void*,const void*,unsigned int n);void*memmove(void*,const void*,
unsigned int n);int memcmp(const void*,const void*,unsigned int n);char*memchr(const
char*,char c,unsigned int n);void*memset(void*,int c,unsigned int n);void bcopy(const
void*src,void*dest,unsigned int n);void bzero(void*s,unsigned int n);char*
GC_realloc(char*,unsigned int n);struct _tagged_arr Cyc_Std_realloc(struct
_tagged_arr s,unsigned int n){char*_tmp72=GC_realloc((char*)_check_null(_untag_arr(
s,sizeof(char),1)),n);return wrap_Cstring_as_string(_tmp72,n);}struct _tagged_arr
Cyc_Std__memcpy(struct _tagged_arr d,struct _tagged_arr s,unsigned int n,unsigned int
sz){if(((d.curr == (_tag_arr(0,0,0)).curr?1: _get_arr_size(d,sizeof(void))< n)?1: s.curr
== ((struct _tagged_arr)_tag_arr(0,0,0)).curr)?1: _get_arr_size(s,sizeof(void))< n)(
int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp73=_cycalloc(
sizeof(*_tmp73));_tmp73[0]=({struct Cyc_Core_Invalid_argument_struct _tmp74;_tmp74.tag=
Cyc_Core_Invalid_argument;_tmp74.f1=({const char*_tmp75="Std::memcpy";_tag_arr(
_tmp75,sizeof(char),_get_zero_arr_size(_tmp75,12));});_tmp74;});_tmp73;}));
memcpy((void*)_check_null(_untag_arr(d,sizeof(void),1)),(const void*)_check_null(
_untag_arr(s,sizeof(void),1)),n * sz);return d;}struct _tagged_arr Cyc_Std__memmove(
struct _tagged_arr d,struct _tagged_arr s,unsigned int n,unsigned int sz){if(((d.curr
== (_tag_arr(0,0,0)).curr?1: _get_arr_size(d,sizeof(void))< n)?1: s.curr == ((
struct _tagged_arr)_tag_arr(0,0,0)).curr)?1: _get_arr_size(s,sizeof(void))< n)(int)
_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp76=_cycalloc(sizeof(*
_tmp76));_tmp76[0]=({struct Cyc_Core_Invalid_argument_struct _tmp77;_tmp77.tag=Cyc_Core_Invalid_argument;
_tmp77.f1=({const char*_tmp78="Std::memove";_tag_arr(_tmp78,sizeof(char),
_get_zero_arr_size(_tmp78,12));});_tmp77;});_tmp76;}));memmove((void*)
_check_null(_untag_arr(d,sizeof(void),1)),(const void*)_check_null(_untag_arr(s,
sizeof(void),1)),n * sz);return d;}int Cyc_Std_memcmp(struct _tagged_arr s1,struct
_tagged_arr s2,unsigned int n){if(((s1.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr?
1: s2.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr)?1: _get_arr_size(s1,
sizeof(char))>= n)?1: _get_arr_size(s2,sizeof(char))>= n)(int)_throw((void*)({
struct Cyc_Core_Invalid_argument_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79[
0]=({struct Cyc_Core_Invalid_argument_struct _tmp7A;_tmp7A.tag=Cyc_Core_Invalid_argument;
_tmp7A.f1=({const char*_tmp7B="Std::memcmp";_tag_arr(_tmp7B,sizeof(char),
_get_zero_arr_size(_tmp7B,12));});_tmp7A;});_tmp79;}));return memcmp((const void*)
_check_null(_untag_arr(s1,sizeof(char),1 + 1)),(const void*)_check_null(_untag_arr(
s2,sizeof(char),1 + 1)),n);}struct _tagged_arr Cyc_Std_memchr(struct _tagged_arr s,
char c,unsigned int n){unsigned int sz=_get_arr_size(s,sizeof(char));if(s.curr == ((
struct _tagged_arr)_tag_arr(0,0,0)).curr?1: n > sz)(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp7D;_tmp7D.tag=Cyc_Core_Invalid_argument;_tmp7D.f1=({const char*_tmp7E="Std::memchr";
_tag_arr(_tmp7E,sizeof(char),_get_zero_arr_size(_tmp7E,12));});_tmp7D;});_tmp7C;}));{
char*_tmp7F=memchr((const char*)_check_null(_untag_arr(s,sizeof(char),1)),c,n);
if(_tmp7F == 0)return(struct _tagged_arr)_tag_arr(0,0,0);{unsigned int _tmp80=(
unsigned int)((const char*)_check_null(_untag_arr(s,sizeof(char),1)));
unsigned int _tmp81=(unsigned int)((const char*)_tmp7F);unsigned int _tmp82=_tmp81 - 
_tmp80;return _tagged_arr_plus(s,sizeof(char),(int)_tmp82);}}}struct _tagged_arr
Cyc_Std_mmemchr(struct _tagged_arr s,char c,unsigned int n){unsigned int sz=
_get_arr_size(s,sizeof(char));if(s.curr == (_tag_arr(0,0,0)).curr?1: n > sz)(int)
_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp83=_cycalloc(sizeof(*
_tmp83));_tmp83[0]=({struct Cyc_Core_Invalid_argument_struct _tmp84;_tmp84.tag=Cyc_Core_Invalid_argument;
_tmp84.f1=({const char*_tmp85="Std::mmemchr";_tag_arr(_tmp85,sizeof(char),
_get_zero_arr_size(_tmp85,13));});_tmp84;});_tmp83;}));{char*_tmp86=memchr((
const char*)_check_null(_untag_arr(s,sizeof(char),1)),c,n);if(_tmp86 == 0)return
_tag_arr(0,0,0);{unsigned int _tmp87=(unsigned int)((const char*)_check_null(
_untag_arr(s,sizeof(char),1)));unsigned int _tmp88=(unsigned int)_tmp86;
unsigned int _tmp89=_tmp88 - _tmp87;return _tagged_arr_plus(s,sizeof(char),(int)
_tmp89);}}}struct _tagged_arr Cyc_Std_memset(struct _tagged_arr s,char c,unsigned int
n){if(s.curr == (_tag_arr(0,0,0)).curr?1: n > _get_arr_size(s,sizeof(char)))(int)
_throw((void*)({struct Cyc_Core_Invalid_argument_struct*_tmp8A=_cycalloc(sizeof(*
_tmp8A));_tmp8A[0]=({struct Cyc_Core_Invalid_argument_struct _tmp8B;_tmp8B.tag=Cyc_Core_Invalid_argument;
_tmp8B.f1=({const char*_tmp8C="Std::memset";_tag_arr(_tmp8C,sizeof(char),
_get_zero_arr_size(_tmp8C,12));});_tmp8B;});_tmp8A;}));memset((void*)((char*)
_check_null(_untag_arr(s,sizeof(char),1))),(int)c,n);return s;}void Cyc_Std_bzero(
struct _tagged_arr s,unsigned int n){if(s.curr == (_tag_arr(0,0,0)).curr?1:
_get_arr_size(s,sizeof(char))< n)(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp8E;_tmp8E.tag=Cyc_Core_Invalid_argument;_tmp8E.f1=({const char*_tmp8F="Std::bzero";
_tag_arr(_tmp8F,sizeof(char),_get_zero_arr_size(_tmp8F,11));});_tmp8E;});_tmp8D;}));((
void(*)(char*s,unsigned int n))bzero)((char*)_check_null(_untag_arr(s,sizeof(char),
1 + 1)),n);}void Cyc_Std__bcopy(struct _tagged_arr src,struct _tagged_arr dst,
unsigned int n,unsigned int sz){if(((src.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr?
1: _get_arr_size(src,sizeof(void))< n)?1: dst.curr == (_tag_arr(0,0,0)).curr)?1:
_get_arr_size(dst,sizeof(void))< n)(int)_throw((void*)({struct Cyc_Core_Invalid_argument_struct*
_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90[0]=({struct Cyc_Core_Invalid_argument_struct
_tmp91;_tmp91.tag=Cyc_Core_Invalid_argument;_tmp91.f1=({const char*_tmp92="Std::bcopy";
_tag_arr(_tmp92,sizeof(char),_get_zero_arr_size(_tmp92,11));});_tmp91;});_tmp90;}));
bcopy((const void*)_check_null(_untag_arr(src,sizeof(void),1)),(void*)_check_null(
_untag_arr(dst,sizeof(void),1)),n * sz);}
