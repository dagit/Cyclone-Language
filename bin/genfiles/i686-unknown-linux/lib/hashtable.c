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
extern void* _region_malloc(struct _RegionHandle *, unsigned);
extern void* _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
extern void  _free_region(struct _RegionHandle *);

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
  ((char *)cks_ptr) + cks_elt_sz*cks_index; })
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
 struct Cyc_Core_Opt{void*v;};extern unsigned char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Impossible[
15];struct Cyc_Core_Impossible_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Not_found[14];extern unsigned char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(struct
Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[18];extern
unsigned char Cyc_List_Nth[8];void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),
struct Cyc_List_List*l,void*x);struct Cyc_Hashtable_Table;struct Cyc_Hashtable_Table*
Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(
struct Cyc_Hashtable_Table*t,void*key,void*val);void*Cyc_Hashtable_lookup(struct
Cyc_Hashtable_Table*t,void*key);void Cyc_Hashtable_resize(struct Cyc_Hashtable_Table*
t);void Cyc_Hashtable_remove(struct Cyc_Hashtable_Table*t,void*key);int Cyc_Hashtable_hash_string(
struct _tagged_arr s);int Cyc_Hashtable_hash_stringptr(struct _tagged_arr*p);void Cyc_Hashtable_iter(
void(*f)(void*,void*),struct Cyc_Hashtable_Table*t);void Cyc_Hashtable_iter_c(void(*
f)(void*,void*,void*),struct Cyc_Hashtable_Table*t,void*env);void Cyc_Hashtable_print_table_map(
struct Cyc_Hashtable_Table*t,void(*prn_key)(void*),void(*prn_val)(void*));struct
Cyc_Cstdio___abstractFILE;struct Cyc_Std___cycFILE;extern unsigned char Cyc_Std_FileCloseError[
19];extern unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct
Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int
tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};int Cyc_Std_printf(
struct _tagged_arr fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;
short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct
Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;}
;struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_Hashtable_Table{int(*cmp)(void*,void*);int(*hash)(
void*);int max_len;struct _tagged_arr tab;};struct Cyc_Hashtable_Table*Cyc_Hashtable_create(
int sz,int(*cmp)(void*,void*),int(*hash)(void*)){struct Cyc_List_List*mt=0;return({
struct Cyc_Hashtable_Table*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0->cmp=cmp;_tmp0->hash=
hash;_tmp0->max_len=3;_tmp0->tab=({unsigned int _tmp1=(unsigned int)sz;struct Cyc_List_List**
_tmp2=(struct Cyc_List_List**)_cycalloc(_check_times(sizeof(struct Cyc_List_List*),
_tmp1));struct _tagged_arr _tmp4=_tag_arr(_tmp2,sizeof(struct Cyc_List_List*),(
unsigned int)sz);{unsigned int _tmp3=_tmp1;unsigned int i;for(i=0;i < _tmp3;i ++){
_tmp2[i]=mt;}}_tmp4;});_tmp0;});}struct _tuple0{void*f1;void*f2;};void Cyc_Hashtable_insert(
struct Cyc_Hashtable_Table*t,void*key,void*val){struct _tagged_arr tab=t->tab;int
bucket=(int)((*t->hash)(key)% _get_arr_size(tab,sizeof(struct Cyc_List_List*)));((
struct Cyc_List_List**)tab.curr)[bucket]=({struct Cyc_List_List*_tmp5=_cycalloc(
sizeof(*_tmp5));_tmp5->hd=({struct _tuple0*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->f1=
key;_tmp6->f2=val;_tmp6;});_tmp5->tl=((struct Cyc_List_List**)tab.curr)[bucket];
_tmp5;});if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_List_List**)
tab.curr)[bucket])> t->max_len)Cyc_Hashtable_resize(t);}void*Cyc_Hashtable_lookup(
struct Cyc_Hashtable_Table*t,void*key){struct _tagged_arr tab=t->tab;struct Cyc_List_List*
l=((struct Cyc_List_List**)tab.curr)[(int)((*t->hash)(key)% _get_arr_size(tab,
sizeof(struct Cyc_List_List*)))];return Cyc_List_assoc_cmp(t->cmp,l,key);}void Cyc_Hashtable_remove(
struct Cyc_Hashtable_Table*t,void*key){struct _tagged_arr tab=t->tab;int(*cmp)(void*,
void*)=t->cmp;int bucket=(int)((*t->hash)(key)% _get_arr_size(tab,sizeof(struct
Cyc_List_List*)));struct Cyc_List_List*l=((struct Cyc_List_List**)tab.curr)[bucket];
if(l == 0)return;if((*cmp)(key,(((struct _tuple0*)l->hd)[
_check_known_subscript_notnull(1,0)]).f1)== 0){((struct Cyc_List_List**)tab.curr)[
bucket]=l->tl;return;}{struct Cyc_List_List*prev=l;l=l->tl;for(0;l->tl != 0;(prev=
l,l=l->tl)){if((*cmp)(key,(((struct _tuple0*)l->hd)[
_check_known_subscript_notnull(1,0)]).f1)== 0){prev->tl=l->tl;return;}}}}int Cyc_Hashtable_hash_string(
struct _tagged_arr s){int ans=0;int sz=(int)_get_arr_size(s,sizeof(unsigned char));
int shift=0;{int i=0;for(0;i < sz;++ i){ans=ans ^ ((const unsigned char*)s.curr)[i]<< 
shift;shift +=8;if(shift == 32)shift=0;}}return ans;}int Cyc_Hashtable_hash_stringptr(
struct _tagged_arr*s){return Cyc_Hashtable_hash_string(*s);}void Cyc_Hashtable_insert_bucket(
struct _tagged_arr tab,int(*hash)(void*),struct Cyc_List_List*elems){if(elems == 0)
return;Cyc_Hashtable_insert_bucket(tab,hash,elems->tl);{void*key=(((struct
_tuple0*)elems->hd)[_check_known_subscript_notnull(1,0)]).f1;void*val=(((struct
_tuple0*)elems->hd)[_check_known_subscript_notnull(1,0)]).f2;int nidx=(int)((*
hash)(key)% _get_arr_size(tab,sizeof(struct Cyc_List_List*)));((struct Cyc_List_List**)
tab.curr)[nidx]=(struct Cyc_List_List*)({struct Cyc_List_List*_tmp7=_cycalloc(
sizeof(*_tmp7));_tmp7->hd=(struct _tuple0*)({struct _tuple0*_tmp8=_cycalloc(
sizeof(*_tmp8));_tmp8->f1=key;_tmp8->f2=val;_tmp8;});_tmp7->tl=((struct Cyc_List_List**)
tab.curr)[nidx];_tmp7;});}}void Cyc_Hashtable_resize(struct Cyc_Hashtable_Table*t){
struct _tagged_arr odata=t->tab;int osize=(int)_get_arr_size(odata,sizeof(struct Cyc_List_List*));
int nsize=2 * osize + 1;struct Cyc_List_List*mt=0;struct _tagged_arr ndata=({
unsigned int _tmp9=(unsigned int)nsize;struct Cyc_List_List**_tmpA=(struct Cyc_List_List**)
_cycalloc(_check_times(sizeof(struct Cyc_List_List*),_tmp9));struct _tagged_arr
_tmpC=_tag_arr(_tmpA,sizeof(struct Cyc_List_List*),(unsigned int)nsize);{
unsigned int _tmpB=_tmp9;unsigned int i;for(i=0;i < _tmpB;i ++){_tmpA[i]=mt;}}_tmpC;});{
int i=0;for(0;i < osize;i ++){Cyc_Hashtable_insert_bucket(ndata,t->hash,((struct Cyc_List_List**)
odata.curr)[i]);}}t->tab=ndata;t->max_len=2 * t->max_len;}void Cyc_Hashtable_iter(
void(*f)(void*,void*),struct Cyc_Hashtable_Table*t){struct _tagged_arr odata=t->tab;
int osize=(int)_get_arr_size(odata,sizeof(struct Cyc_List_List*));int i=0;for(0;i < 
osize;i ++){struct Cyc_List_List*iter=((struct Cyc_List_List**)odata.curr)[i];for(0;
iter != 0;iter=iter->tl){f((((struct _tuple0*)iter->hd)[
_check_known_subscript_notnull(1,0)]).f1,(((struct _tuple0*)iter->hd)[
_check_known_subscript_notnull(1,0)]).f2);}}}void Cyc_Hashtable_iter_c(void(*f)(
void*,void*,void*),struct Cyc_Hashtable_Table*t,void*env){struct _tagged_arr odata=
t->tab;int osize=(int)_get_arr_size(odata,sizeof(struct Cyc_List_List*));int i=0;
for(0;i < osize;i ++){struct Cyc_List_List*iter=((struct Cyc_List_List**)odata.curr)[
i];for(0;iter != 0;iter=iter->tl){f((((struct _tuple0*)iter->hd)[
_check_known_subscript_notnull(1,0)]).f1,(((struct _tuple0*)iter->hd)[
_check_known_subscript_notnull(1,0)]).f2,env);}}}void Cyc_Hashtable_print_table_map(
struct Cyc_Hashtable_Table*t,void(*prn_key)(void*),void(*prn_val)(void*)){struct
_tagged_arr odata=t->tab;int osize=(int)_get_arr_size(odata,sizeof(struct Cyc_List_List*));
int i=0;for(0;i < osize;i ++){({struct Cyc_Std_Int_pa_struct _tmpE;_tmpE.tag=1;_tmpE.f1=(
int)((unsigned int)i);{void*_tmpD[1]={& _tmpE};Cyc_Std_printf(_tag_arr("%d: ",
sizeof(unsigned char),5),_tag_arr(_tmpD,sizeof(void*),1));}});{struct Cyc_List_List*
iter=((struct Cyc_List_List**)odata.curr)[i];for(0;iter != 0;iter=iter->tl){({void*
_tmpF[0]={};Cyc_Std_printf(_tag_arr("(",sizeof(unsigned char),2),_tag_arr(_tmpF,
sizeof(void*),0));});prn_key((((struct _tuple0*)iter->hd)[
_check_known_subscript_notnull(1,0)]).f1);({void*_tmp10[0]={};Cyc_Std_printf(
_tag_arr(",",sizeof(unsigned char),2),_tag_arr(_tmp10,sizeof(void*),0));});
prn_val((((struct _tuple0*)iter->hd)[_check_known_subscript_notnull(1,0)]).f2);({
void*_tmp11[0]={};Cyc_Std_printf(_tag_arr(") ",sizeof(unsigned char),3),_tag_arr(
_tmp11,sizeof(void*),0));});}}({void*_tmp12[0]={};Cyc_Std_printf(_tag_arr("\n",
sizeof(unsigned char),2),_tag_arr(_tmp12,sizeof(void*),0));});}}
