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
  unsigned int total_bytes;
  unsigned int free_bytes;
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
extern int _throw(void * e);
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
  ({ void *_check_null_temp = (void*)(ptr); \
     if (!_check_null_temp) _throw_null(); \
     _check_null_temp; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  ((char *)ptr) + (elt_sz)*(index); })
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  void *_cks_ptr = (void*)(ptr); \
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
  _tag_arr_ans.base = _tag_arr_ans.curr = (void *)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })

#define _init_tag_arr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _tagged_arr *_itarr_ptr = (arr_ptr); \
  void * _itarr = (arr); \
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
extern void * GC_malloc(int);
extern void * GC_malloc_atomic(int);
extern void * GC_calloc(unsigned int,unsigned int);
extern void * GC_calloc_atomic(unsigned int,unsigned int);

static inline void * _cycalloc(int n) {
  void * ans = (void *)GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cycalloc_atomic(int n) {
  void * ans = (void *)GC_malloc_atomic(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cyccalloc(unsigned int n, unsigned int s) {
  void * ans = (void *)GC_calloc(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
static inline void * _cyccalloc_atomic(unsigned int n, unsigned int s) {
  void * ans = (void *)GC_calloc_atomic(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
#define MAX_MALLOC_SIZE (1 << 28)
static inline unsigned int _check_times(unsigned int x, unsigned int y) {
  unsigned long long whole_ans = 
    ((unsigned long long)x)*((unsigned long long)y);
  unsigned int word_ans = (unsigned int)whole_ans;
  if(word_ans < whole_ans || word_ans > MAX_MALLOC_SIZE)
    _throw_badalloc();
  return word_ans;
}

#if defined(CYC_REGION_PROFILE) 
extern void * _profile_GC_malloc(int,char *file,int lineno);
extern void * _profile_GC_malloc_atomic(int,char *file,int lineno);
extern void * _profile_region_malloc(struct _RegionHandle *, unsigned int,
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
struct Cyc_Typerep_Int_struct{int tag;int f1;unsigned int f2;};struct Cyc_Typerep_ThinPtr_struct{
int tag;unsigned int f1;void*f2;};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;}
;struct _tuple0{unsigned int f1;struct _tagged_arr f2;void*f3;};struct Cyc_Typerep_Struct_struct{
int tag;struct _tagged_arr*f1;unsigned int f2;struct _tagged_arr f3;};struct _tuple1{
unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;unsigned int f1;
struct _tagged_arr f2;};struct _tuple2{unsigned int f1;struct _tagged_arr f2;};struct
Cyc_Typerep_TUnion_struct{int tag;struct _tagged_arr f1;struct _tagged_arr f2;struct
_tagged_arr f3;};struct Cyc_Typerep_TUnionField_struct{int tag;struct _tagged_arr f1;
struct _tagged_arr f2;unsigned int f3;struct _tagged_arr f4;};struct _tuple3{struct
_tagged_arr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{int tag;struct
_tagged_arr f1;struct _tagged_arr f2;};struct Cyc_Typerep_Union_struct{int tag;struct
_tagged_arr*f1;int f2;struct _tagged_arr f3;};struct Cyc_Typerep_Enum_struct{int tag;
struct _tagged_arr*f1;int f2;struct _tagged_arr f3;};extern void Cyc_Typerep_print_typestruct(
void*rep);extern void Cyc_Typerep_xmlize_typestruct(void*rep);struct _tuple4{int f1;
void*f2;};extern struct _tuple4 Cyc_Typerep_get_unionbranch(unsigned int tag,struct
_tagged_arr l);extern struct _tagged_arr Cyc_Typerep_get_tagname(unsigned int tag,
struct _tagged_arr l);extern struct _tagged_arr Cyc_Typerep_get_tagname2(unsigned int
tag,struct _tagged_arr l);extern struct _tuple4 Cyc_Typerep_get_xtunionbranch(struct
_tagged_arr tag,struct _tagged_arr l);unsigned int Cyc_Typerep_size_type(void*rep);
struct Cyc_Cstdio___abstractFILE;struct Cyc_Std___cycFILE;extern unsigned char Cyc_Std_FileCloseError[
19];extern unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Std_String_pa_struct{int tag;
struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct
Cyc_Std_Double_pa_struct{int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int
tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};extern int
Cyc_Std_printf(struct _tagged_arr fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern
unsigned char Cyc_List_List_mismatch[18];extern unsigned char Cyc_List_Nth[8];
struct Cyc_Dict_Dict;extern unsigned char Cyc_Dict_Present[12];extern unsigned char
Cyc_Dict_Absent[11];extern int Cyc_Std_strcmp(struct _tagged_arr s1,struct
_tagged_arr s2);struct _tuple4 Cyc_Typerep_tuple_tl(void*typ){void*_tmp0=typ;struct
_tagged_arr _tmp1;unsigned int _tmp2;_LL1: if((unsigned int)_tmp0 > 2?*((int*)_tmp0)
== 4: 0){_LL6: _tmp2=((struct Cyc_Typerep_Tuple_struct*)_tmp0)->f1;goto _LL5;_LL5:
_tmp1=((struct Cyc_Typerep_Tuple_struct*)_tmp0)->f2;goto _LL2;}else{goto _LL3;}_LL3:
goto _LL4;_LL2: if(_get_arr_size(_tmp1,sizeof(struct _tuple1*))< 2){goto _LL4;}{int
pos1=(int)(*(*((struct _tuple1**)_check_unknown_subscript(_tmp1,sizeof(struct
_tuple1*),1)))).f1;struct _tagged_arr _tmp3=({unsigned int _tmp7=_get_arr_size(
_tmp1,sizeof(struct _tuple1*))- 1;struct _tuple1**_tmp8=(struct _tuple1**)_cycalloc(
_check_times(sizeof(struct _tuple1*),_tmp7));struct _tagged_arr _tmpB=_tag_arr(
_tmp8,sizeof(struct _tuple1*),_get_arr_size(_tmp1,sizeof(struct _tuple1*))- 1);{
unsigned int _tmp9=_tmp7;unsigned int i;for(i=0;i < _tmp9;i ++){_tmp8[i]=({struct
_tuple1*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA->f1=(*(*((struct _tuple1**)
_check_unknown_subscript(_tmp1,sizeof(struct _tuple1*),(int)(i + 1))))).f1 - pos1;
_tmpA->f2=(*(*((struct _tuple1**)_check_unknown_subscript(_tmp1,sizeof(struct
_tuple1*),(int)(i + 1))))).f2;_tmpA;});}};_tmpB;});return({struct _tuple4 _tmp4;
_tmp4.f1=pos1;_tmp4.f2=(void*)({struct Cyc_Typerep_Tuple_struct*_tmp5=_cycalloc(
sizeof(*_tmp5));_tmp5[0]=({struct Cyc_Typerep_Tuple_struct _tmp6;_tmp6.tag=4;_tmp6.f1=
_tmp2 - pos1;_tmp6.f2=_tmp3;_tmp6;});_tmp5;});_tmp4;});}_LL4:(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmpC=_cycalloc(sizeof(*_tmpC));_tmpC[0]=({struct
Cyc_Core_Failure_struct _tmpD;_tmpD.tag=Cyc_Core_Failure;_tmpD.f1=_tag_arr("tuple_tl: expected tuple of size >= 2",
sizeof(unsigned char),38);_tmpD;});_tmpC;}));_LL0:;}struct _tuple4 Cyc_Typerep_get_unionbranch(
unsigned int tag,struct _tagged_arr l){{int i=0;for(0;i < _get_arr_size(l,sizeof(
struct _tuple0*));i ++){void*_tmpF;unsigned int _tmp10;struct _tuple0 _tmpE=*((struct
_tuple0**)l.curr)[i];_LL9: _tmp10=_tmpE.f1;goto _LL8;_LL8: _tmpF=_tmpE.f3;goto _LL7;
_LL7: if(_tmp10 == tag){return Cyc_Typerep_tuple_tl(_tmpF);}}}(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11[0]=({
struct Cyc_Core_Failure_struct _tmp12;_tmp12.tag=Cyc_Core_Failure;_tmp12.f1=
_tag_arr("Could not find tag in TUnion",sizeof(unsigned char),29);_tmp12;});
_tmp11;}));}struct _tagged_arr Cyc_Typerep_get_tagname(unsigned int tag,struct
_tagged_arr l){{int i=0;for(0;i < _get_arr_size(l,sizeof(struct _tuple2*));i ++){
struct _tagged_arr _tmp14;unsigned int _tmp15;struct _tuple2 _tmp13=*((struct _tuple2**)
l.curr)[i];_LLC: _tmp15=_tmp13.f1;goto _LLB;_LLB: _tmp14=_tmp13.f2;goto _LLA;_LLA:
if(_tmp15 == tag){return _tmp14;}}}(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16[0]=({struct Cyc_Core_Failure_struct
_tmp17;_tmp17.tag=Cyc_Core_Failure;_tmp17.f1=_tag_arr("Could not find name in TUnion",
sizeof(unsigned char),30);_tmp17;});_tmp16;}));}struct _tagged_arr Cyc_Typerep_get_tagname2(
unsigned int tag,struct _tagged_arr l){{int i=0;for(0;i < _get_arr_size(l,sizeof(
struct _tuple0*));i ++){struct _tagged_arr _tmp19;unsigned int _tmp1A;struct _tuple0
_tmp18=*((struct _tuple0**)l.curr)[i];_LLF: _tmp1A=_tmp18.f1;goto _LLE;_LLE: _tmp19=
_tmp18.f2;goto _LLD;_LLD: if(_tmp1A == tag){return _tmp19;}}}(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B[0]=({
struct Cyc_Core_Failure_struct _tmp1C;_tmp1C.tag=Cyc_Core_Failure;_tmp1C.f1=
_tag_arr("Could not find name in TUnion",sizeof(unsigned char),30);_tmp1C;});
_tmp1B;}));}struct _tuple4 Cyc_Typerep_get_xtunionbranch(struct _tagged_arr tag,
struct _tagged_arr l){{int i=0;for(0;i < _get_arr_size(l,sizeof(struct _tuple3*));i ++){
void*_tmp1E;struct _tagged_arr _tmp1F;struct _tuple3 _tmp1D=*((struct _tuple3**)l.curr)[
i];_LL12: _tmp1F=_tmp1D.f1;goto _LL11;_LL11: _tmp1E=_tmp1D.f2;goto _LL10;_LL10: if(
Cyc_Std_strcmp(_tmp1F,tag)== 0){return Cyc_Typerep_tuple_tl(_tmp1E);}}}(int)
_throw((void*)({struct Cyc_Core_Failure_struct*_tmp20=_cycalloc(sizeof(*_tmp20));
_tmp20[0]=({struct Cyc_Core_Failure_struct _tmp21;_tmp21.tag=Cyc_Core_Failure;
_tmp21.f1=_tag_arr("Could not find tag in XTUnion",sizeof(unsigned char),30);
_tmp21;});_tmp20;}));}unsigned int Cyc_Typerep_size_type(void*rep){void*_tmp22=
rep;unsigned int _tmp23;int _tmp24;unsigned int _tmp25;unsigned int _tmp26;
unsigned int _tmp27;int _tmp28;struct _tagged_arr _tmp29;int _tmp2A;_LL14: if((
unsigned int)_tmp22 > 2?*((int*)_tmp22)== 0: 0){_LL2D: _tmp24=((struct Cyc_Typerep_Int_struct*)
_tmp22)->f1;goto _LL2C;_LL2C: _tmp23=((struct Cyc_Typerep_Int_struct*)_tmp22)->f2;
goto _LL15;}else{goto _LL16;}_LL16: if((int)_tmp22 == 0){goto _LL17;}else{goto _LL18;}
_LL18: if((int)_tmp22 == 1){goto _LL19;}else{goto _LL1A;}_LL1A: if((unsigned int)
_tmp22 > 2?*((int*)_tmp22)== 1: 0){goto _LL1B;}else{goto _LL1C;}_LL1C: if((
unsigned int)_tmp22 > 2?*((int*)_tmp22)== 2: 0){goto _LL1D;}else{goto _LL1E;}_LL1E:
if((unsigned int)_tmp22 > 2?*((int*)_tmp22)== 3: 0){_LL2E: _tmp25=((struct Cyc_Typerep_Struct_struct*)
_tmp22)->f2;goto _LL1F;}else{goto _LL20;}_LL20: if((unsigned int)_tmp22 > 2?*((int*)
_tmp22)== 4: 0){_LL2F: _tmp26=((struct Cyc_Typerep_Tuple_struct*)_tmp22)->f1;goto
_LL21;}else{goto _LL22;}_LL22: if((unsigned int)_tmp22 > 2?*((int*)_tmp22)== 6: 0){
_LL30: _tmp27=((struct Cyc_Typerep_TUnionField_struct*)_tmp22)->f3;goto _LL23;}
else{goto _LL24;}_LL24: if((unsigned int)_tmp22 > 2?*((int*)_tmp22)== 5: 0){goto
_LL25;}else{goto _LL26;}_LL26: if((unsigned int)_tmp22 > 2?*((int*)_tmp22)== 7: 0){
goto _LL27;}else{goto _LL28;}_LL28: if((unsigned int)_tmp22 > 2?*((int*)_tmp22)== 8:
0){_LL31: _tmp28=((struct Cyc_Typerep_Union_struct*)_tmp22)->f2;goto _LL29;}else{
goto _LL2A;}_LL2A: if((unsigned int)_tmp22 > 2?*((int*)_tmp22)== 9: 0){_LL33: _tmp2A=((
struct Cyc_Typerep_Enum_struct*)_tmp22)->f2;goto _LL32;_LL32: _tmp29=((struct Cyc_Typerep_Enum_struct*)
_tmp22)->f3;goto _LL2B;}else{goto _LL13;}_LL15: return _tmp23 >> 3;_LL17: return
sizeof(float);_LL19: return sizeof(double);_LL1B: return sizeof(void*);_LL1D: return
sizeof(struct _tagged_arr);_LL1F: return _tmp25;_LL21: return _tmp26;_LL23: return
_tmp27;_LL25: return sizeof(void*);_LL27: return sizeof(void*);_LL29: return(
unsigned int)_tmp28;_LL2B: return(unsigned int)_tmp2A;_LL13:;}struct _tagged_arr
Cyc_Typerep_anon_or_name(struct _tagged_arr*name){return name == 0? _tag_arr("(anon)",
sizeof(unsigned char),7):*name;}void Cyc_Typerep_print_typestruct(void*rep){void*
_tmp2B=rep;unsigned int _tmp2C;int _tmp2D;void*_tmp2E;unsigned int _tmp2F;void*
_tmp30;struct _tagged_arr _tmp31;unsigned int _tmp32;struct _tagged_arr*_tmp33;
struct _tagged_arr _tmp34;unsigned int _tmp35;struct _tagged_arr _tmp36;unsigned int
_tmp37;struct _tagged_arr _tmp38;struct _tagged_arr _tmp39;struct _tagged_arr _tmp3A;
struct _tagged_arr _tmp3B;struct _tagged_arr _tmp3C;struct _tagged_arr _tmp3D;struct
_tagged_arr _tmp3E;struct _tagged_arr _tmp3F;int _tmp40;struct _tagged_arr*_tmp41;
struct _tagged_arr _tmp42;int _tmp43;struct _tagged_arr*_tmp44;_LL35: if((
unsigned int)_tmp2B > 2?*((int*)_tmp2B)== 0: 0){_LL4E: _tmp2D=((struct Cyc_Typerep_Int_struct*)
_tmp2B)->f1;goto _LL4D;_LL4D: _tmp2C=((struct Cyc_Typerep_Int_struct*)_tmp2B)->f2;
goto _LL36;}else{goto _LL37;}_LL37: if((int)_tmp2B == 0){goto _LL38;}else{goto _LL39;}
_LL39: if((int)_tmp2B == 1){goto _LL3A;}else{goto _LL3B;}_LL3B: if((unsigned int)
_tmp2B > 2?*((int*)_tmp2B)== 1: 0){_LL50: _tmp2F=((struct Cyc_Typerep_ThinPtr_struct*)
_tmp2B)->f1;goto _LL4F;_LL4F: _tmp2E=(void*)((struct Cyc_Typerep_ThinPtr_struct*)
_tmp2B)->f2;goto _LL3C;}else{goto _LL3D;}_LL3D: if((unsigned int)_tmp2B > 2?*((int*)
_tmp2B)== 2: 0){_LL51: _tmp30=(void*)((struct Cyc_Typerep_FatPtr_struct*)_tmp2B)->f1;
goto _LL3E;}else{goto _LL3F;}_LL3F: if((unsigned int)_tmp2B > 2?*((int*)_tmp2B)== 3:
0){_LL54: _tmp33=((struct Cyc_Typerep_Struct_struct*)_tmp2B)->f1;goto _LL53;_LL53:
_tmp32=((struct Cyc_Typerep_Struct_struct*)_tmp2B)->f2;goto _LL52;_LL52: _tmp31=((
struct Cyc_Typerep_Struct_struct*)_tmp2B)->f3;goto _LL40;}else{goto _LL41;}_LL41:
if((unsigned int)_tmp2B > 2?*((int*)_tmp2B)== 4: 0){_LL56: _tmp35=((struct Cyc_Typerep_Tuple_struct*)
_tmp2B)->f1;goto _LL55;_LL55: _tmp34=((struct Cyc_Typerep_Tuple_struct*)_tmp2B)->f2;
goto _LL42;}else{goto _LL43;}_LL43: if((unsigned int)_tmp2B > 2?*((int*)_tmp2B)== 6:
0){_LL5A: _tmp39=((struct Cyc_Typerep_TUnionField_struct*)_tmp2B)->f1;goto _LL59;
_LL59: _tmp38=((struct Cyc_Typerep_TUnionField_struct*)_tmp2B)->f2;goto _LL58;_LL58:
_tmp37=((struct Cyc_Typerep_TUnionField_struct*)_tmp2B)->f3;goto _LL57;_LL57:
_tmp36=((struct Cyc_Typerep_TUnionField_struct*)_tmp2B)->f4;goto _LL44;}else{goto
_LL45;}_LL45: if((unsigned int)_tmp2B > 2?*((int*)_tmp2B)== 5: 0){_LL5D: _tmp3C=((
struct Cyc_Typerep_TUnion_struct*)_tmp2B)->f1;goto _LL5C;_LL5C: _tmp3B=((struct Cyc_Typerep_TUnion_struct*)
_tmp2B)->f2;goto _LL5B;_LL5B: _tmp3A=((struct Cyc_Typerep_TUnion_struct*)_tmp2B)->f3;
goto _LL46;}else{goto _LL47;}_LL47: if((unsigned int)_tmp2B > 2?*((int*)_tmp2B)== 7:
0){_LL5F: _tmp3E=((struct Cyc_Typerep_XTUnion_struct*)_tmp2B)->f1;goto _LL5E;_LL5E:
_tmp3D=((struct Cyc_Typerep_XTUnion_struct*)_tmp2B)->f2;goto _LL48;}else{goto _LL49;}
_LL49: if((unsigned int)_tmp2B > 2?*((int*)_tmp2B)== 8: 0){_LL62: _tmp41=((struct Cyc_Typerep_Union_struct*)
_tmp2B)->f1;goto _LL61;_LL61: _tmp40=((struct Cyc_Typerep_Union_struct*)_tmp2B)->f2;
goto _LL60;_LL60: _tmp3F=((struct Cyc_Typerep_Union_struct*)_tmp2B)->f3;goto _LL4A;}
else{goto _LL4B;}_LL4B: if((unsigned int)_tmp2B > 2?*((int*)_tmp2B)== 9: 0){_LL65:
_tmp44=((struct Cyc_Typerep_Enum_struct*)_tmp2B)->f1;goto _LL64;_LL64: _tmp43=((
struct Cyc_Typerep_Enum_struct*)_tmp2B)->f2;goto _LL63;_LL63: _tmp42=((struct Cyc_Typerep_Enum_struct*)
_tmp2B)->f3;goto _LL4C;}else{goto _LL34;}_LL36:({struct Cyc_Std_Int_pa_struct _tmp47;
_tmp47.tag=1;_tmp47.f1=(int)_tmp2C;{struct Cyc_Std_String_pa_struct _tmp46;_tmp46.tag=
0;_tmp46.f1=(struct _tagged_arr)(_tmp2D? _tag_arr("signed",sizeof(unsigned char),7):
_tag_arr("unsigned",sizeof(unsigned char),9));{void*_tmp45[2]={& _tmp46,& _tmp47};
Cyc_Std_printf(_tag_arr("Int(%s,%d)",sizeof(unsigned char),11),_tag_arr(_tmp45,
sizeof(void*),2));}}});goto _LL34;_LL38:({void*_tmp48[0]={};Cyc_Std_printf(
_tag_arr("Float",sizeof(unsigned char),6),_tag_arr(_tmp48,sizeof(void*),0));});
goto _LL34;_LL3A:({void*_tmp49[0]={};Cyc_Std_printf(_tag_arr("Double",sizeof(
unsigned char),7),_tag_arr(_tmp49,sizeof(void*),0));});goto _LL34;_LL3C:({struct
Cyc_Std_Int_pa_struct _tmp4B;_tmp4B.tag=1;_tmp4B.f1=(int)_tmp2F;{void*_tmp4A[1]={&
_tmp4B};Cyc_Std_printf(_tag_arr("ThinPtr(%d,",sizeof(unsigned char),12),_tag_arr(
_tmp4A,sizeof(void*),1));}});Cyc_Typerep_print_typestruct(_tmp2E);({void*_tmp4C[
0]={};Cyc_Std_printf(_tag_arr(")",sizeof(unsigned char),2),_tag_arr(_tmp4C,
sizeof(void*),0));});goto _LL34;_LL3E:({void*_tmp4D[0]={};Cyc_Std_printf(_tag_arr("FatPtr(",
sizeof(unsigned char),8),_tag_arr(_tmp4D,sizeof(void*),0));});Cyc_Typerep_print_typestruct(
_tmp30);({void*_tmp4E[0]={};Cyc_Std_printf(_tag_arr(")",sizeof(unsigned char),2),
_tag_arr(_tmp4E,sizeof(void*),0));});goto _LL34;_LL40:({struct Cyc_Std_Int_pa_struct
_tmp51;_tmp51.tag=1;_tmp51.f1=(int)_tmp32;{struct Cyc_Std_String_pa_struct _tmp50;
_tmp50.tag=0;_tmp50.f1=(struct _tagged_arr)Cyc_Typerep_anon_or_name(_tmp33);{void*
_tmp4F[2]={& _tmp50,& _tmp51};Cyc_Std_printf(_tag_arr("Struct(%s,%d,[",sizeof(
unsigned char),15),_tag_arr(_tmp4F,sizeof(void*),2));}}});{int i=0;for(0;i < 
_get_arr_size(_tmp31,sizeof(struct _tuple0*));i ++){void*_tmp53;struct _tagged_arr
_tmp54;unsigned int _tmp55;struct _tuple0 _tmp52=*((struct _tuple0**)_tmp31.curr)[i];
_LL69: _tmp55=_tmp52.f1;goto _LL68;_LL68: _tmp54=_tmp52.f2;goto _LL67;_LL67: _tmp53=
_tmp52.f3;goto _LL66;_LL66:({struct Cyc_Std_String_pa_struct _tmp58;_tmp58.tag=0;
_tmp58.f1=(struct _tagged_arr)_tmp54;{struct Cyc_Std_Int_pa_struct _tmp57;_tmp57.tag=
1;_tmp57.f1=(int)_tmp55;{void*_tmp56[2]={& _tmp57,& _tmp58};Cyc_Std_printf(
_tag_arr("$(%d,%s,",sizeof(unsigned char),9),_tag_arr(_tmp56,sizeof(void*),2));}}});
Cyc_Typerep_print_typestruct(_tmp53);({struct Cyc_Std_Int_pa_struct _tmp5A;_tmp5A.tag=
1;_tmp5A.f1=(int)((unsigned int)((int)(i != _get_arr_size(_tmp31,sizeof(struct
_tuple0*))- 1?',':' ')));{void*_tmp59[1]={& _tmp5A};Cyc_Std_printf(_tag_arr(")%c",
sizeof(unsigned char),4),_tag_arr(_tmp59,sizeof(void*),1));}});}}({void*_tmp5B[0]={};
Cyc_Std_printf(_tag_arr(" ]",sizeof(unsigned char),3),_tag_arr(_tmp5B,sizeof(
void*),0));});goto _LL34;_LL42:({struct Cyc_Std_Int_pa_struct _tmp5D;_tmp5D.tag=1;
_tmp5D.f1=(int)_tmp35;{void*_tmp5C[1]={& _tmp5D};Cyc_Std_printf(_tag_arr("Tuple(%d,[",
sizeof(unsigned char),11),_tag_arr(_tmp5C,sizeof(void*),1));}});{int i=0;for(0;i < 
_get_arr_size(_tmp34,sizeof(struct _tuple1*));i ++){void*_tmp5F;unsigned int _tmp60;
struct _tuple1 _tmp5E=*((struct _tuple1**)_tmp34.curr)[i];_LL6C: _tmp60=_tmp5E.f1;
goto _LL6B;_LL6B: _tmp5F=_tmp5E.f2;goto _LL6A;_LL6A:({struct Cyc_Std_Int_pa_struct
_tmp62;_tmp62.tag=1;_tmp62.f1=(int)_tmp60;{void*_tmp61[1]={& _tmp62};Cyc_Std_printf(
_tag_arr("$(%d,",sizeof(unsigned char),6),_tag_arr(_tmp61,sizeof(void*),1));}});
Cyc_Typerep_print_typestruct(_tmp5F);({struct Cyc_Std_Int_pa_struct _tmp64;_tmp64.tag=
1;_tmp64.f1=(int)((unsigned int)((int)(i != _get_arr_size(_tmp34,sizeof(struct
_tuple1*))- 1?',':' ')));{void*_tmp63[1]={& _tmp64};Cyc_Std_printf(_tag_arr(")%c",
sizeof(unsigned char),4),_tag_arr(_tmp63,sizeof(void*),1));}});}}({void*_tmp65[0]={};
Cyc_Std_printf(_tag_arr(" ]",sizeof(unsigned char),3),_tag_arr(_tmp65,sizeof(
void*),0));});goto _LL34;_LL44:({struct Cyc_Std_Int_pa_struct _tmp69;_tmp69.tag=1;
_tmp69.f1=(int)_tmp37;{struct Cyc_Std_String_pa_struct _tmp68;_tmp68.tag=0;_tmp68.f1=(
struct _tagged_arr)_tmp38;{struct Cyc_Std_String_pa_struct _tmp67;_tmp67.tag=0;
_tmp67.f1=(struct _tagged_arr)_tmp39;{void*_tmp66[3]={& _tmp67,& _tmp68,& _tmp69};
Cyc_Std_printf(_tag_arr("TUnionField(%s,%s,%d,[",sizeof(unsigned char),23),
_tag_arr(_tmp66,sizeof(void*),3));}}}});{int i=0;for(0;i < _get_arr_size(_tmp36,
sizeof(struct _tuple1*));i ++){void*_tmp6B;unsigned int _tmp6C;struct _tuple1 _tmp6A=*((
struct _tuple1**)_tmp36.curr)[i];_LL6F: _tmp6C=_tmp6A.f1;goto _LL6E;_LL6E: _tmp6B=
_tmp6A.f2;goto _LL6D;_LL6D:({struct Cyc_Std_Int_pa_struct _tmp6E;_tmp6E.tag=1;
_tmp6E.f1=(int)_tmp6C;{void*_tmp6D[1]={& _tmp6E};Cyc_Std_printf(_tag_arr("$(%d,",
sizeof(unsigned char),6),_tag_arr(_tmp6D,sizeof(void*),1));}});Cyc_Typerep_print_typestruct(
_tmp6B);({struct Cyc_Std_Int_pa_struct _tmp70;_tmp70.tag=1;_tmp70.f1=(int)((
unsigned int)((int)(i != _get_arr_size(_tmp36,sizeof(struct _tuple1*))- 1?',':' ')));{
void*_tmp6F[1]={& _tmp70};Cyc_Std_printf(_tag_arr(")%c",sizeof(unsigned char),4),
_tag_arr(_tmp6F,sizeof(void*),1));}});}}({void*_tmp71[0]={};Cyc_Std_printf(
_tag_arr(" ]",sizeof(unsigned char),3),_tag_arr(_tmp71,sizeof(void*),0));});goto
_LL34;_LL46:({struct Cyc_Std_String_pa_struct _tmp73;_tmp73.tag=0;_tmp73.f1=(
struct _tagged_arr)_tmp3C;{void*_tmp72[1]={& _tmp73};Cyc_Std_printf(_tag_arr("TUnion(%s,[",
sizeof(unsigned char),12),_tag_arr(_tmp72,sizeof(void*),1));}});{int i=0;for(0;i < 
_get_arr_size(_tmp3B,sizeof(struct _tuple2*));i ++){struct _tagged_arr _tmp75;
unsigned int _tmp76;struct _tuple2 _tmp74=*((struct _tuple2**)_tmp3B.curr)[i];_LL72:
_tmp76=_tmp74.f1;goto _LL71;_LL71: _tmp75=_tmp74.f2;goto _LL70;_LL70:({struct Cyc_Std_Int_pa_struct
_tmp7A;_tmp7A.tag=1;_tmp7A.f1=(int)((unsigned int)((int)(i != _get_arr_size(
_tmp3B,sizeof(struct _tuple2*))- 1?',':' ')));{struct Cyc_Std_String_pa_struct
_tmp79;_tmp79.tag=0;_tmp79.f1=(struct _tagged_arr)_tmp75;{struct Cyc_Std_Int_pa_struct
_tmp78;_tmp78.tag=1;_tmp78.f1=(int)_tmp76;{void*_tmp77[3]={& _tmp78,& _tmp79,&
_tmp7A};Cyc_Std_printf(_tag_arr("$(%d,%s)%c",sizeof(unsigned char),11),_tag_arr(
_tmp77,sizeof(void*),3));}}}});}}({void*_tmp7B[0]={};Cyc_Std_printf(_tag_arr("],[",
sizeof(unsigned char),4),_tag_arr(_tmp7B,sizeof(void*),0));});{int i=0;for(0;i < 
_get_arr_size(_tmp3A,sizeof(struct _tuple0*));i ++){void*_tmp7D;struct _tagged_arr
_tmp7E;unsigned int _tmp7F;struct _tuple0 _tmp7C=*((struct _tuple0**)_tmp3A.curr)[i];
_LL76: _tmp7F=_tmp7C.f1;goto _LL75;_LL75: _tmp7E=_tmp7C.f2;goto _LL74;_LL74: _tmp7D=
_tmp7C.f3;goto _LL73;_LL73:({struct Cyc_Std_String_pa_struct _tmp82;_tmp82.tag=0;
_tmp82.f1=(struct _tagged_arr)_tmp7E;{struct Cyc_Std_Int_pa_struct _tmp81;_tmp81.tag=
1;_tmp81.f1=(int)_tmp7F;{void*_tmp80[2]={& _tmp81,& _tmp82};Cyc_Std_printf(
_tag_arr("$(%d,%s,",sizeof(unsigned char),9),_tag_arr(_tmp80,sizeof(void*),2));}}});
Cyc_Typerep_print_typestruct(_tmp7D);({struct Cyc_Std_Int_pa_struct _tmp84;_tmp84.tag=
1;_tmp84.f1=(int)((unsigned int)((int)(i != _get_arr_size(_tmp3A,sizeof(struct
_tuple0*))- 1?',':' ')));{void*_tmp83[1]={& _tmp84};Cyc_Std_printf(_tag_arr(")%c",
sizeof(unsigned char),4),_tag_arr(_tmp83,sizeof(void*),1));}});}}({void*_tmp85[0]={};
Cyc_Std_printf(_tag_arr("] )",sizeof(unsigned char),4),_tag_arr(_tmp85,sizeof(
void*),0));});goto _LL34;_LL48:({struct Cyc_Std_String_pa_struct _tmp87;_tmp87.tag=
0;_tmp87.f1=(struct _tagged_arr)_tmp3E;{void*_tmp86[1]={& _tmp87};Cyc_Std_printf(
_tag_arr("XTUnion(%s,",sizeof(unsigned char),12),_tag_arr(_tmp86,sizeof(void*),1));}});{
int i=0;for(0;i < _get_arr_size(_tmp3D,sizeof(struct _tuple3*));i ++){void*_tmp89;
struct _tagged_arr _tmp8A;struct _tuple3 _tmp88=*((struct _tuple3**)_tmp3D.curr)[i];
_LL79: _tmp8A=_tmp88.f1;goto _LL78;_LL78: _tmp89=_tmp88.f2;goto _LL77;_LL77:({struct
Cyc_Std_String_pa_struct _tmp8C;_tmp8C.tag=0;_tmp8C.f1=(struct _tagged_arr)
_tagged_arr_plus(_tmp8A,sizeof(unsigned char),4);{void*_tmp8B[1]={& _tmp8C};Cyc_Std_printf(
_tag_arr("$(%s,",sizeof(unsigned char),6),_tag_arr(_tmp8B,sizeof(void*),1));}});
Cyc_Typerep_print_typestruct(_tmp89);({struct Cyc_Std_Int_pa_struct _tmp8E;_tmp8E.tag=
1;_tmp8E.f1=(int)((unsigned int)((int)(i != _get_arr_size(_tmp3D,sizeof(struct
_tuple3*))- 1?',':' ')));{void*_tmp8D[1]={& _tmp8E};Cyc_Std_printf(_tag_arr(")%c",
sizeof(unsigned char),4),_tag_arr(_tmp8D,sizeof(void*),1));}});}}({void*_tmp8F[0]={};
Cyc_Std_printf(_tag_arr(" )",sizeof(unsigned char),3),_tag_arr(_tmp8F,sizeof(
void*),0));});goto _LL34;_LL4A:({struct Cyc_Std_Int_pa_struct _tmp92;_tmp92.tag=1;
_tmp92.f1=(int)((unsigned int)_tmp40);{struct Cyc_Std_String_pa_struct _tmp91;
_tmp91.tag=0;_tmp91.f1=(struct _tagged_arr)Cyc_Typerep_anon_or_name(_tmp41);{void*
_tmp90[2]={& _tmp91,& _tmp92};Cyc_Std_printf(_tag_arr("Union(%s,%d,",sizeof(
unsigned char),13),_tag_arr(_tmp90,sizeof(void*),2));}}});{int i=0;for(0;i < 
_get_arr_size(_tmp3F,sizeof(struct _tuple3*));i ++){void*_tmp94;struct _tagged_arr
_tmp95;struct _tuple3 _tmp93=*((struct _tuple3**)_tmp3F.curr)[i];_LL7C: _tmp95=
_tmp93.f1;goto _LL7B;_LL7B: _tmp94=_tmp93.f2;goto _LL7A;_LL7A:({struct Cyc_Std_String_pa_struct
_tmp97;_tmp97.tag=0;_tmp97.f1=(struct _tagged_arr)_tmp95;{void*_tmp96[1]={& _tmp97};
Cyc_Std_printf(_tag_arr("$(%s,",sizeof(unsigned char),6),_tag_arr(_tmp96,sizeof(
void*),1));}});Cyc_Typerep_print_typestruct(_tmp94);({struct Cyc_Std_Int_pa_struct
_tmp99;_tmp99.tag=1;_tmp99.f1=(int)((unsigned int)((int)(i != _get_arr_size(
_tmp3F,sizeof(struct _tuple3*))- 1?',':' ')));{void*_tmp98[1]={& _tmp99};Cyc_Std_printf(
_tag_arr(")%c",sizeof(unsigned char),4),_tag_arr(_tmp98,sizeof(void*),1));}});}}({
void*_tmp9A[0]={};Cyc_Std_printf(_tag_arr(")",sizeof(unsigned char),2),_tag_arr(
_tmp9A,sizeof(void*),0));});goto _LL34;_LL4C:({struct Cyc_Std_Int_pa_struct _tmp9D;
_tmp9D.tag=1;_tmp9D.f1=(int)((unsigned int)_tmp43);{struct Cyc_Std_String_pa_struct
_tmp9C;_tmp9C.tag=0;_tmp9C.f1=(struct _tagged_arr)Cyc_Typerep_anon_or_name(_tmp44);{
void*_tmp9B[2]={& _tmp9C,& _tmp9D};Cyc_Std_printf(_tag_arr("Enum(%s,%d,",sizeof(
unsigned char),12),_tag_arr(_tmp9B,sizeof(void*),2));}}});{int i=0;for(0;i < 
_get_arr_size(_tmp42,sizeof(struct _tuple2*));i ++){struct _tagged_arr _tmp9F;
unsigned int _tmpA0;struct _tuple2 _tmp9E=*((struct _tuple2**)_tmp42.curr)[i];_LL7F:
_tmpA0=_tmp9E.f1;goto _LL7E;_LL7E: _tmp9F=_tmp9E.f2;goto _LL7D;_LL7D:({struct Cyc_Std_Int_pa_struct
_tmpA4;_tmpA4.tag=1;_tmpA4.f1=(int)((unsigned int)((int)(i != _get_arr_size(
_tmp42,sizeof(struct _tuple2*))- 1?',':' ')));{struct Cyc_Std_String_pa_struct
_tmpA3;_tmpA3.tag=0;_tmpA3.f1=(struct _tagged_arr)_tmp9F;{struct Cyc_Std_Int_pa_struct
_tmpA2;_tmpA2.tag=1;_tmpA2.f1=(int)_tmpA0;{void*_tmpA1[3]={& _tmpA2,& _tmpA3,&
_tmpA4};Cyc_Std_printf(_tag_arr("$(%d,%s)%c",sizeof(unsigned char),11),_tag_arr(
_tmpA1,sizeof(void*),3));}}}});}}({void*_tmpA5[0]={};Cyc_Std_printf(_tag_arr(")",
sizeof(unsigned char),2),_tag_arr(_tmpA5,sizeof(void*),0));});goto _LL34;_LL34:;}
void Cyc_Typerep_xmlize_typestruct(void*rep){void*_tmpA6=rep;unsigned int _tmpA7;
int _tmpA8;void*_tmpA9;unsigned int _tmpAA;void*_tmpAB;struct _tagged_arr _tmpAC;
unsigned int _tmpAD;struct _tagged_arr*_tmpAE;struct _tagged_arr _tmpAF;unsigned int
_tmpB0;struct _tagged_arr _tmpB1;unsigned int _tmpB2;struct _tagged_arr _tmpB3;struct
_tagged_arr _tmpB4;struct _tagged_arr _tmpB5;struct _tagged_arr _tmpB6;struct
_tagged_arr _tmpB7;struct _tagged_arr _tmpB8;struct _tagged_arr _tmpB9;struct
_tagged_arr _tmpBA;int _tmpBB;struct _tagged_arr*_tmpBC;struct _tagged_arr _tmpBD;int
_tmpBE;struct _tagged_arr*_tmpBF;_LL81: if((unsigned int)_tmpA6 > 2?*((int*)_tmpA6)
== 0: 0){_LL9A: _tmpA8=((struct Cyc_Typerep_Int_struct*)_tmpA6)->f1;goto _LL99;_LL99:
_tmpA7=((struct Cyc_Typerep_Int_struct*)_tmpA6)->f2;goto _LL82;}else{goto _LL83;}
_LL83: if((int)_tmpA6 == 0){goto _LL84;}else{goto _LL85;}_LL85: if((int)_tmpA6 == 1){
goto _LL86;}else{goto _LL87;}_LL87: if((unsigned int)_tmpA6 > 2?*((int*)_tmpA6)== 1:
0){_LL9C: _tmpAA=((struct Cyc_Typerep_ThinPtr_struct*)_tmpA6)->f1;goto _LL9B;_LL9B:
_tmpA9=(void*)((struct Cyc_Typerep_ThinPtr_struct*)_tmpA6)->f2;goto _LL88;}else{
goto _LL89;}_LL89: if((unsigned int)_tmpA6 > 2?*((int*)_tmpA6)== 2: 0){_LL9D: _tmpAB=(
void*)((struct Cyc_Typerep_FatPtr_struct*)_tmpA6)->f1;goto _LL8A;}else{goto _LL8B;}
_LL8B: if((unsigned int)_tmpA6 > 2?*((int*)_tmpA6)== 3: 0){_LLA0: _tmpAE=((struct Cyc_Typerep_Struct_struct*)
_tmpA6)->f1;goto _LL9F;_LL9F: _tmpAD=((struct Cyc_Typerep_Struct_struct*)_tmpA6)->f2;
goto _LL9E;_LL9E: _tmpAC=((struct Cyc_Typerep_Struct_struct*)_tmpA6)->f3;goto _LL8C;}
else{goto _LL8D;}_LL8D: if((unsigned int)_tmpA6 > 2?*((int*)_tmpA6)== 4: 0){_LLA2:
_tmpB0=((struct Cyc_Typerep_Tuple_struct*)_tmpA6)->f1;goto _LLA1;_LLA1: _tmpAF=((
struct Cyc_Typerep_Tuple_struct*)_tmpA6)->f2;goto _LL8E;}else{goto _LL8F;}_LL8F: if((
unsigned int)_tmpA6 > 2?*((int*)_tmpA6)== 6: 0){_LLA6: _tmpB4=((struct Cyc_Typerep_TUnionField_struct*)
_tmpA6)->f1;goto _LLA5;_LLA5: _tmpB3=((struct Cyc_Typerep_TUnionField_struct*)
_tmpA6)->f2;goto _LLA4;_LLA4: _tmpB2=((struct Cyc_Typerep_TUnionField_struct*)
_tmpA6)->f3;goto _LLA3;_LLA3: _tmpB1=((struct Cyc_Typerep_TUnionField_struct*)
_tmpA6)->f4;goto _LL90;}else{goto _LL91;}_LL91: if((unsigned int)_tmpA6 > 2?*((int*)
_tmpA6)== 5: 0){_LLA9: _tmpB7=((struct Cyc_Typerep_TUnion_struct*)_tmpA6)->f1;goto
_LLA8;_LLA8: _tmpB6=((struct Cyc_Typerep_TUnion_struct*)_tmpA6)->f2;goto _LLA7;
_LLA7: _tmpB5=((struct Cyc_Typerep_TUnion_struct*)_tmpA6)->f3;goto _LL92;}else{goto
_LL93;}_LL93: if((unsigned int)_tmpA6 > 2?*((int*)_tmpA6)== 7: 0){_LLAB: _tmpB9=((
struct Cyc_Typerep_XTUnion_struct*)_tmpA6)->f1;goto _LLAA;_LLAA: _tmpB8=((struct Cyc_Typerep_XTUnion_struct*)
_tmpA6)->f2;goto _LL94;}else{goto _LL95;}_LL95: if((unsigned int)_tmpA6 > 2?*((int*)
_tmpA6)== 8: 0){_LLAE: _tmpBC=((struct Cyc_Typerep_Union_struct*)_tmpA6)->f1;goto
_LLAD;_LLAD: _tmpBB=((struct Cyc_Typerep_Union_struct*)_tmpA6)->f2;goto _LLAC;_LLAC:
_tmpBA=((struct Cyc_Typerep_Union_struct*)_tmpA6)->f3;goto _LL96;}else{goto _LL97;}
_LL97: if((unsigned int)_tmpA6 > 2?*((int*)_tmpA6)== 9: 0){_LLB1: _tmpBF=((struct Cyc_Typerep_Enum_struct*)
_tmpA6)->f1;goto _LLB0;_LLB0: _tmpBE=((struct Cyc_Typerep_Enum_struct*)_tmpA6)->f2;
goto _LLAF;_LLAF: _tmpBD=((struct Cyc_Typerep_Enum_struct*)_tmpA6)->f3;goto _LL98;}
else{goto _LL80;}_LL82:({struct Cyc_Std_Int_pa_struct _tmpC2;_tmpC2.tag=1;_tmpC2.f1=(
int)_tmpA7;{struct Cyc_Std_Int_pa_struct _tmpC1;_tmpC1.tag=1;_tmpC1.f1=(int)((
unsigned int)_tmpA8);{void*_tmpC0[2]={& _tmpC1,& _tmpC2};Cyc_Std_printf(_tag_arr("<Int sn=%d sz=\"%d\"/>",
sizeof(unsigned char),21),_tag_arr(_tmpC0,sizeof(void*),2));}}});goto _LL80;_LL84:({
void*_tmpC3[0]={};Cyc_Std_printf(_tag_arr("<Float/>",sizeof(unsigned char),9),
_tag_arr(_tmpC3,sizeof(void*),0));});goto _LL80;_LL86:({void*_tmpC4[0]={};Cyc_Std_printf(
_tag_arr("<Double/>",sizeof(unsigned char),10),_tag_arr(_tmpC4,sizeof(void*),0));});
goto _LL80;_LL88:({struct Cyc_Std_Int_pa_struct _tmpC6;_tmpC6.tag=1;_tmpC6.f1=(int)
_tmpAA;{void*_tmpC5[1]={& _tmpC6};Cyc_Std_printf(_tag_arr("<ThinPtr sz=\"%d\">",
sizeof(unsigned char),18),_tag_arr(_tmpC5,sizeof(void*),1));}});Cyc_Typerep_xmlize_typestruct(
_tmpA9);({void*_tmpC7[0]={};Cyc_Std_printf(_tag_arr("</ThinPtr",sizeof(
unsigned char),10),_tag_arr(_tmpC7,sizeof(void*),0));});goto _LL80;_LL8A:({void*
_tmpC8[0]={};Cyc_Std_printf(_tag_arr("<FatPtr>",sizeof(unsigned char),9),
_tag_arr(_tmpC8,sizeof(void*),0));});Cyc_Typerep_xmlize_typestruct(_tmpAB);({
void*_tmpC9[0]={};Cyc_Std_printf(_tag_arr("</FatPtr>",sizeof(unsigned char),10),
_tag_arr(_tmpC9,sizeof(void*),0));});goto _LL80;_LL8C:({struct Cyc_Std_Int_pa_struct
_tmpCC;_tmpCC.tag=1;_tmpCC.f1=(int)_tmpAD;{struct Cyc_Std_String_pa_struct _tmpCB;
_tmpCB.tag=0;_tmpCB.f1=(struct _tagged_arr)Cyc_Typerep_anon_or_name(_tmpAE);{void*
_tmpCA[2]={& _tmpCB,& _tmpCC};Cyc_Std_printf(_tag_arr("<Struct name=\"%s\" sz=\"%d\">",
sizeof(unsigned char),27),_tag_arr(_tmpCA,sizeof(void*),2));}}});{int i=0;for(0;i
< _get_arr_size(_tmpAC,sizeof(struct _tuple0*));i ++){void*_tmpCE;struct
_tagged_arr _tmpCF;unsigned int _tmpD0;struct _tuple0 _tmpCD=*((struct _tuple0**)
_tmpAC.curr)[i];_LLB5: _tmpD0=_tmpCD.f1;goto _LLB4;_LLB4: _tmpCF=_tmpCD.f2;goto
_LLB3;_LLB3: _tmpCE=_tmpCD.f3;goto _LLB2;_LLB2:({struct Cyc_Std_String_pa_struct
_tmpD3;_tmpD3.tag=0;_tmpD3.f1=(struct _tagged_arr)_tmpCF;{struct Cyc_Std_Int_pa_struct
_tmpD2;_tmpD2.tag=1;_tmpD2.f1=(int)_tmpD0;{void*_tmpD1[2]={& _tmpD2,& _tmpD3};Cyc_Std_printf(
_tag_arr("<Component ofs=\"%d\" nm=\"%s\">",sizeof(unsigned char),29),_tag_arr(
_tmpD1,sizeof(void*),2));}}});Cyc_Typerep_xmlize_typestruct(_tmpCE);({void*
_tmpD4[0]={};Cyc_Std_printf(_tag_arr("</Component>",sizeof(unsigned char),13),
_tag_arr(_tmpD4,sizeof(void*),0));});}}({void*_tmpD5[0]={};Cyc_Std_printf(
_tag_arr("</Struct>",sizeof(unsigned char),10),_tag_arr(_tmpD5,sizeof(void*),0));});
goto _LL80;_LL8E:({struct Cyc_Std_Int_pa_struct _tmpD7;_tmpD7.tag=1;_tmpD7.f1=(int)
_tmpB0;{void*_tmpD6[1]={& _tmpD7};Cyc_Std_printf(_tag_arr("<Tuple sz=\"%d\">",
sizeof(unsigned char),16),_tag_arr(_tmpD6,sizeof(void*),1));}});{int i=0;for(0;i < 
_get_arr_size(_tmpAF,sizeof(struct _tuple1*));i ++){void*_tmpD9;unsigned int _tmpDA;
struct _tuple1 _tmpD8=*((struct _tuple1**)_tmpAF.curr)[i];_LLB8: _tmpDA=_tmpD8.f1;
goto _LLB7;_LLB7: _tmpD9=_tmpD8.f2;goto _LLB6;_LLB6:({struct Cyc_Std_Int_pa_struct
_tmpDC;_tmpDC.tag=1;_tmpDC.f1=(int)_tmpDA;{void*_tmpDB[1]={& _tmpDC};Cyc_Std_printf(
_tag_arr("<Component ofs=\"%d\">",sizeof(unsigned char),21),_tag_arr(_tmpDB,
sizeof(void*),1));}});Cyc_Typerep_xmlize_typestruct(_tmpD9);({void*_tmpDD[0]={};
Cyc_Std_printf(_tag_arr("</Component>",sizeof(unsigned char),13),_tag_arr(_tmpDD,
sizeof(void*),0));});}}({void*_tmpDE[0]={};Cyc_Std_printf(_tag_arr("</Tuple>",
sizeof(unsigned char),9),_tag_arr(_tmpDE,sizeof(void*),0));});goto _LL80;_LL90:({
struct Cyc_Std_Int_pa_struct _tmpE2;_tmpE2.tag=1;_tmpE2.f1=(int)_tmpB2;{struct Cyc_Std_String_pa_struct
_tmpE1;_tmpE1.tag=0;_tmpE1.f1=(struct _tagged_arr)_tmpB3;{struct Cyc_Std_String_pa_struct
_tmpE0;_tmpE0.tag=0;_tmpE0.f1=(struct _tagged_arr)_tmpB4;{void*_tmpDF[3]={& _tmpE0,&
_tmpE1,& _tmpE2};Cyc_Std_printf(_tag_arr("<TUnionField tname=\"%s\" fname=\"%s\" sz=\"%d\">",
sizeof(unsigned char),44),_tag_arr(_tmpDF,sizeof(void*),3));}}}});{int i=0;for(0;
i < _get_arr_size(_tmpB1,sizeof(struct _tuple1*));i ++){void*_tmpE4;unsigned int
_tmpE5;struct _tuple1 _tmpE3=*((struct _tuple1**)_tmpB1.curr)[i];_LLBB: _tmpE5=
_tmpE3.f1;goto _LLBA;_LLBA: _tmpE4=_tmpE3.f2;goto _LLB9;_LLB9:({struct Cyc_Std_Int_pa_struct
_tmpE7;_tmpE7.tag=1;_tmpE7.f1=(int)_tmpE5;{void*_tmpE6[1]={& _tmpE7};Cyc_Std_printf(
_tag_arr("<Component ofs=\"%d\">",sizeof(unsigned char),21),_tag_arr(_tmpE6,
sizeof(void*),1));}});Cyc_Typerep_xmlize_typestruct(_tmpE4);({void*_tmpE8[0]={};
Cyc_Std_printf(_tag_arr("</Component>",sizeof(unsigned char),13),_tag_arr(_tmpE8,
sizeof(void*),0));});}}({void*_tmpE9[0]={};Cyc_Std_printf(_tag_arr("</TUnionField>",
sizeof(unsigned char),15),_tag_arr(_tmpE9,sizeof(void*),0));});goto _LL80;_LL92:({
struct Cyc_Std_String_pa_struct _tmpEB;_tmpEB.tag=0;_tmpEB.f1=(struct _tagged_arr)
_tmpB7;{void*_tmpEA[1]={& _tmpEB};Cyc_Std_printf(_tag_arr("<TUnion name=\"%s\">",
sizeof(unsigned char),19),_tag_arr(_tmpEA,sizeof(void*),1));}});{int i=0;for(0;i < 
_get_arr_size(_tmpB6,sizeof(struct _tuple2*));i ++){struct _tagged_arr _tmpED;
unsigned int _tmpEE;struct _tuple2 _tmpEC=*((struct _tuple2**)_tmpB6.curr)[i];_LLBE:
_tmpEE=_tmpEC.f1;goto _LLBD;_LLBD: _tmpED=_tmpEC.f2;goto _LLBC;_LLBC:({struct Cyc_Std_String_pa_struct
_tmpF1;_tmpF1.tag=0;_tmpF1.f1=(struct _tagged_arr)_tmpED;{struct Cyc_Std_Int_pa_struct
_tmpF0;_tmpF0.tag=1;_tmpF0.f1=(int)_tmpEE;{void*_tmpEF[2]={& _tmpF0,& _tmpF1};Cyc_Std_printf(
_tag_arr("<Tag tag=\"%d\" nm=\"%s\"/>",sizeof(unsigned char),24),_tag_arr(_tmpEF,
sizeof(void*),2));}}});}}{int i=0;for(0;i < _get_arr_size(_tmpB5,sizeof(struct
_tuple0*));i ++){void*_tmpF3;struct _tagged_arr _tmpF4;unsigned int _tmpF5;struct
_tuple0 _tmpF2=*((struct _tuple0**)_tmpB5.curr)[i];_LLC2: _tmpF5=_tmpF2.f1;goto
_LLC1;_LLC1: _tmpF4=_tmpF2.f2;goto _LLC0;_LLC0: _tmpF3=_tmpF2.f3;goto _LLBF;_LLBF:({
struct Cyc_Std_String_pa_struct _tmpF8;_tmpF8.tag=0;_tmpF8.f1=(struct _tagged_arr)
_tmpF4;{struct Cyc_Std_Int_pa_struct _tmpF7;_tmpF7.tag=1;_tmpF7.f1=(int)_tmpF5;{
void*_tmpF6[2]={& _tmpF7,& _tmpF8};Cyc_Std_printf(_tag_arr("<Tag tag=\"%d\" nm=\"%s\">",
sizeof(unsigned char),23),_tag_arr(_tmpF6,sizeof(void*),2));}}});Cyc_Typerep_xmlize_typestruct(
_tmpF3);({void*_tmpF9[0]={};Cyc_Std_printf(_tag_arr("</Tag>",sizeof(
unsigned char),7),_tag_arr(_tmpF9,sizeof(void*),0));});}}({void*_tmpFA[0]={};Cyc_Std_printf(
_tag_arr("</Tunion>",sizeof(unsigned char),10),_tag_arr(_tmpFA,sizeof(void*),0));});
goto _LL80;_LL94:({struct Cyc_Std_String_pa_struct _tmpFC;_tmpFC.tag=0;_tmpFC.f1=(
struct _tagged_arr)_tmpB9;{void*_tmpFB[1]={& _tmpFC};Cyc_Std_printf(_tag_arr("<XTUnion name=\"%s\">",
sizeof(unsigned char),20),_tag_arr(_tmpFB,sizeof(void*),1));}});{int i=0;for(0;i < 
_get_arr_size(_tmpB8,sizeof(struct _tuple3*));i ++){void*_tmpFE;struct _tagged_arr
_tmpFF;struct _tuple3 _tmpFD=*((struct _tuple3**)_tmpB8.curr)[i];_LLC5: _tmpFF=
_tmpFD.f1;goto _LLC4;_LLC4: _tmpFE=_tmpFD.f2;goto _LLC3;_LLC3:({struct Cyc_Std_String_pa_struct
_tmp101;_tmp101.tag=0;_tmp101.f1=(struct _tagged_arr)_tagged_arr_plus(_tmpFF,
sizeof(unsigned char),4);{void*_tmp100[1]={& _tmp101};Cyc_Std_printf(_tag_arr("<Tag tag=\"%s\">",
sizeof(unsigned char),15),_tag_arr(_tmp100,sizeof(void*),1));}});Cyc_Typerep_xmlize_typestruct(
_tmpFE);({void*_tmp102[0]={};Cyc_Std_printf(_tag_arr("</Tag>",sizeof(
unsigned char),7),_tag_arr(_tmp102,sizeof(void*),0));});}}({void*_tmp103[0]={};
Cyc_Std_printf(_tag_arr("</XTunion>",sizeof(unsigned char),11),_tag_arr(_tmp103,
sizeof(void*),0));});goto _LL80;_LL96:({struct Cyc_Std_Int_pa_struct _tmp106;
_tmp106.tag=1;_tmp106.f1=(int)((unsigned int)_tmpBB);{struct Cyc_Std_String_pa_struct
_tmp105;_tmp105.tag=0;_tmp105.f1=(struct _tagged_arr)Cyc_Typerep_anon_or_name(
_tmpBC);{void*_tmp104[2]={& _tmp105,& _tmp106};Cyc_Std_printf(_tag_arr("<Union name=\"%s\" sz=\"%d\">",
sizeof(unsigned char),26),_tag_arr(_tmp104,sizeof(void*),2));}}});{int i=0;for(0;
i < _get_arr_size(_tmpBA,sizeof(struct _tuple3*));i ++){void*_tmp108;struct
_tagged_arr _tmp109;struct _tuple3 _tmp107=*((struct _tuple3**)_tmpBA.curr)[i];_LLC8:
_tmp109=_tmp107.f1;goto _LLC7;_LLC7: _tmp108=_tmp107.f2;goto _LLC6;_LLC6:({struct
Cyc_Std_String_pa_struct _tmp10B;_tmp10B.tag=0;_tmp10B.f1=(struct _tagged_arr)
_tmp109;{void*_tmp10A[1]={& _tmp10B};Cyc_Std_printf(_tag_arr("<Case nm=\"%s\">",
sizeof(unsigned char),15),_tag_arr(_tmp10A,sizeof(void*),1));}});Cyc_Typerep_xmlize_typestruct(
_tmp108);({void*_tmp10C[0]={};Cyc_Std_printf(_tag_arr("</Case>",sizeof(
unsigned char),8),_tag_arr(_tmp10C,sizeof(void*),0));});}}({void*_tmp10D[0]={};
Cyc_Std_printf(_tag_arr("</Union>",sizeof(unsigned char),9),_tag_arr(_tmp10D,
sizeof(void*),0));});goto _LL80;_LL98:({struct Cyc_Std_Int_pa_struct _tmp110;
_tmp110.tag=1;_tmp110.f1=(int)((unsigned int)_tmpBE);{struct Cyc_Std_String_pa_struct
_tmp10F;_tmp10F.tag=0;_tmp10F.f1=(struct _tagged_arr)Cyc_Typerep_anon_or_name(
_tmpBF);{void*_tmp10E[2]={& _tmp10F,& _tmp110};Cyc_Std_printf(_tag_arr("<Enum name=\"%s\" szb=\"%d\">",
sizeof(unsigned char),26),_tag_arr(_tmp10E,sizeof(void*),2));}}});{int i=0;for(0;
i < _get_arr_size(_tmpBD,sizeof(struct _tuple2*));i ++){struct _tagged_arr _tmp112;
unsigned int _tmp113;struct _tuple2 _tmp111=*((struct _tuple2**)_tmpBD.curr)[i];
_LLCB: _tmp113=_tmp111.f1;goto _LLCA;_LLCA: _tmp112=_tmp111.f2;goto _LLC9;_LLC9:({
struct Cyc_Std_String_pa_struct _tmp116;_tmp116.tag=0;_tmp116.f1=(struct
_tagged_arr)_tmp112;{struct Cyc_Std_Int_pa_struct _tmp115;_tmp115.tag=1;_tmp115.f1=(
int)_tmp113;{void*_tmp114[2]={& _tmp115,& _tmp116};Cyc_Std_printf(_tag_arr("<Val val=\"%d\">%s</Val>",
sizeof(unsigned char),23),_tag_arr(_tmp114,sizeof(void*),2));}}});}}({void*
_tmp117[0]={};Cyc_Std_printf(_tag_arr("</Enum>",sizeof(unsigned char),8),
_tag_arr(_tmp117,sizeof(void*),0));});goto _LL80;_LL80:;}
