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
_tagged_arr f3;};struct _tuple3{struct _tagged_arr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _tagged_arr f1;struct _tagged_arr f2;};struct Cyc_Typerep_Union_struct{
int tag;struct _tagged_arr*f1;struct _tagged_arr f2;};extern void Cyc_Typerep_print_typestruct(
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
rep;unsigned int _tmp23;int _tmp24;unsigned int _tmp25;unsigned int _tmp26;struct
_tagged_arr _tmp27;_LL14: if((unsigned int)_tmp22 > 2?*((int*)_tmp22)== 0: 0){_LL29:
_tmp24=((struct Cyc_Typerep_Int_struct*)_tmp22)->f1;goto _LL28;_LL28: _tmp23=((
struct Cyc_Typerep_Int_struct*)_tmp22)->f2;goto _LL15;}else{goto _LL16;}_LL16: if((
int)_tmp22 == 0){goto _LL17;}else{goto _LL18;}_LL18: if((int)_tmp22 == 1){goto _LL19;}
else{goto _LL1A;}_LL1A: if((unsigned int)_tmp22 > 2?*((int*)_tmp22)== 1: 0){goto
_LL1B;}else{goto _LL1C;}_LL1C: if((unsigned int)_tmp22 > 2?*((int*)_tmp22)== 2: 0){
goto _LL1D;}else{goto _LL1E;}_LL1E: if((unsigned int)_tmp22 > 2?*((int*)_tmp22)== 3:
0){_LL2A: _tmp25=((struct Cyc_Typerep_Struct_struct*)_tmp22)->f2;goto _LL1F;}else{
goto _LL20;}_LL20: if((unsigned int)_tmp22 > 2?*((int*)_tmp22)== 4: 0){_LL2B: _tmp26=((
struct Cyc_Typerep_Tuple_struct*)_tmp22)->f1;goto _LL21;}else{goto _LL22;}_LL22: if((
unsigned int)_tmp22 > 2?*((int*)_tmp22)== 5: 0){goto _LL23;}else{goto _LL24;}_LL24:
if((unsigned int)_tmp22 > 2?*((int*)_tmp22)== 6: 0){goto _LL25;}else{goto _LL26;}
_LL26: if((unsigned int)_tmp22 > 2?*((int*)_tmp22)== 7: 0){_LL2C: _tmp27=((struct Cyc_Typerep_Union_struct*)
_tmp22)->f2;goto _LL27;}else{goto _LL13;}_LL15: return _tmp23 >> 3;_LL17: return
sizeof(float);_LL19: return sizeof(double);_LL1B: return sizeof(void*);_LL1D: return
sizeof(struct _tagged_arr);_LL1F: return _tmp25;_LL21: return _tmp26;_LL23: return
sizeof(void*);_LL25: return sizeof(void*);_LL27: {int max=0;{int i=0;for(0;i < 
_get_arr_size(_tmp27,sizeof(struct _tuple3*));i ++){struct _tuple3 _tmp29;void*
_tmp2A;struct _tagged_arr _tmp2B;struct _tuple3*_tmp28=((struct _tuple3**)_tmp27.curr)[
i];_tmp29=*_tmp28;_LL2F: _tmp2B=_tmp29.f1;goto _LL2E;_LL2E: _tmp2A=_tmp29.f2;goto
_LL2D;_LL2D: {unsigned int _tmp2C=Cyc_Typerep_size_type(_tmp2A);max=(int)(max < 
_tmp2C? _tmp2C:(unsigned int)max);}}}return(unsigned int)max;}_LL13:;}struct
_tagged_arr Cyc_Typerep_anon_or_name(struct _tagged_arr*name){return name == 0?
_tag_arr("(anon)",sizeof(unsigned char),7):*name;}void Cyc_Typerep_print_typestruct(
void*rep){void*_tmp2D=rep;unsigned int _tmp2E;int _tmp2F;void*_tmp30;unsigned int
_tmp31;void*_tmp32;struct _tagged_arr _tmp33;unsigned int _tmp34;struct _tagged_arr*
_tmp35;struct _tagged_arr _tmp36;unsigned int _tmp37;struct _tagged_arr _tmp38;struct
_tagged_arr _tmp39;struct _tagged_arr _tmp3A;struct _tagged_arr _tmp3B;struct
_tagged_arr _tmp3C;struct _tagged_arr _tmp3D;struct _tagged_arr*_tmp3E;_LL31: if((
unsigned int)_tmp2D > 2?*((int*)_tmp2D)== 0: 0){_LL46: _tmp2F=((struct Cyc_Typerep_Int_struct*)
_tmp2D)->f1;goto _LL45;_LL45: _tmp2E=((struct Cyc_Typerep_Int_struct*)_tmp2D)->f2;
goto _LL32;}else{goto _LL33;}_LL33: if((int)_tmp2D == 0){goto _LL34;}else{goto _LL35;}
_LL35: if((int)_tmp2D == 1){goto _LL36;}else{goto _LL37;}_LL37: if((unsigned int)
_tmp2D > 2?*((int*)_tmp2D)== 1: 0){_LL48: _tmp31=((struct Cyc_Typerep_ThinPtr_struct*)
_tmp2D)->f1;goto _LL47;_LL47: _tmp30=(void*)((struct Cyc_Typerep_ThinPtr_struct*)
_tmp2D)->f2;goto _LL38;}else{goto _LL39;}_LL39: if((unsigned int)_tmp2D > 2?*((int*)
_tmp2D)== 2: 0){_LL49: _tmp32=(void*)((struct Cyc_Typerep_FatPtr_struct*)_tmp2D)->f1;
goto _LL3A;}else{goto _LL3B;}_LL3B: if((unsigned int)_tmp2D > 2?*((int*)_tmp2D)== 3:
0){_LL4C: _tmp35=((struct Cyc_Typerep_Struct_struct*)_tmp2D)->f1;goto _LL4B;_LL4B:
_tmp34=((struct Cyc_Typerep_Struct_struct*)_tmp2D)->f2;goto _LL4A;_LL4A: _tmp33=((
struct Cyc_Typerep_Struct_struct*)_tmp2D)->f3;goto _LL3C;}else{goto _LL3D;}_LL3D:
if((unsigned int)_tmp2D > 2?*((int*)_tmp2D)== 4: 0){_LL4E: _tmp37=((struct Cyc_Typerep_Tuple_struct*)
_tmp2D)->f1;goto _LL4D;_LL4D: _tmp36=((struct Cyc_Typerep_Tuple_struct*)_tmp2D)->f2;
goto _LL3E;}else{goto _LL3F;}_LL3F: if((unsigned int)_tmp2D > 2?*((int*)_tmp2D)== 5:
0){_LL51: _tmp3A=((struct Cyc_Typerep_TUnion_struct*)_tmp2D)->f1;goto _LL50;_LL50:
_tmp39=((struct Cyc_Typerep_TUnion_struct*)_tmp2D)->f2;goto _LL4F;_LL4F: _tmp38=((
struct Cyc_Typerep_TUnion_struct*)_tmp2D)->f3;goto _LL40;}else{goto _LL41;}_LL41:
if((unsigned int)_tmp2D > 2?*((int*)_tmp2D)== 6: 0){_LL53: _tmp3C=((struct Cyc_Typerep_XTUnion_struct*)
_tmp2D)->f1;goto _LL52;_LL52: _tmp3B=((struct Cyc_Typerep_XTUnion_struct*)_tmp2D)->f2;
goto _LL42;}else{goto _LL43;}_LL43: if((unsigned int)_tmp2D > 2?*((int*)_tmp2D)== 7:
0){_LL55: _tmp3E=((struct Cyc_Typerep_Union_struct*)_tmp2D)->f1;goto _LL54;_LL54:
_tmp3D=((struct Cyc_Typerep_Union_struct*)_tmp2D)->f2;goto _LL44;}else{goto _LL30;}
_LL32:({struct Cyc_Std_Int_pa_struct _tmp41;_tmp41.tag=1;_tmp41.f1=(int)_tmp2E;{
struct Cyc_Std_String_pa_struct _tmp40;_tmp40.tag=0;_tmp40.f1=(struct _tagged_arr)(
_tmp2F? _tag_arr("signed",sizeof(unsigned char),7): _tag_arr("unsigned",sizeof(
unsigned char),9));{void*_tmp3F[2]={& _tmp40,& _tmp41};Cyc_Std_printf(_tag_arr("Int(%s,%d)",
sizeof(unsigned char),11),_tag_arr(_tmp3F,sizeof(void*),2));}}});goto _LL30;_LL34:({
void*_tmp42[0]={};Cyc_Std_printf(_tag_arr("Float",sizeof(unsigned char),6),
_tag_arr(_tmp42,sizeof(void*),0));});goto _LL30;_LL36:({void*_tmp43[0]={};Cyc_Std_printf(
_tag_arr("Double",sizeof(unsigned char),7),_tag_arr(_tmp43,sizeof(void*),0));});
goto _LL30;_LL38:({struct Cyc_Std_Int_pa_struct _tmp45;_tmp45.tag=1;_tmp45.f1=(int)
_tmp31;{void*_tmp44[1]={& _tmp45};Cyc_Std_printf(_tag_arr("ThinPtr(%d,",sizeof(
unsigned char),12),_tag_arr(_tmp44,sizeof(void*),1));}});Cyc_Typerep_print_typestruct(
_tmp30);({void*_tmp46[0]={};Cyc_Std_printf(_tag_arr(")",sizeof(unsigned char),2),
_tag_arr(_tmp46,sizeof(void*),0));});goto _LL30;_LL3A:({void*_tmp47[0]={};Cyc_Std_printf(
_tag_arr("FatPtr(",sizeof(unsigned char),8),_tag_arr(_tmp47,sizeof(void*),0));});
Cyc_Typerep_print_typestruct(_tmp32);({void*_tmp48[0]={};Cyc_Std_printf(_tag_arr(")",
sizeof(unsigned char),2),_tag_arr(_tmp48,sizeof(void*),0));});goto _LL30;_LL3C:({
struct Cyc_Std_Int_pa_struct _tmp4B;_tmp4B.tag=1;_tmp4B.f1=(int)_tmp34;{struct Cyc_Std_String_pa_struct
_tmp4A;_tmp4A.tag=0;_tmp4A.f1=(struct _tagged_arr)Cyc_Typerep_anon_or_name(_tmp35);{
void*_tmp49[2]={& _tmp4A,& _tmp4B};Cyc_Std_printf(_tag_arr("Struct(%s,%d,[",
sizeof(unsigned char),15),_tag_arr(_tmp49,sizeof(void*),2));}}});{int i=0;for(0;i
< _get_arr_size(_tmp33,sizeof(struct _tuple0*));i ++){void*_tmp4D;struct
_tagged_arr _tmp4E;unsigned int _tmp4F;struct _tuple0 _tmp4C=*((struct _tuple0**)
_tmp33.curr)[i];_LL59: _tmp4F=_tmp4C.f1;goto _LL58;_LL58: _tmp4E=_tmp4C.f2;goto
_LL57;_LL57: _tmp4D=_tmp4C.f3;goto _LL56;_LL56:({struct Cyc_Std_String_pa_struct
_tmp52;_tmp52.tag=0;_tmp52.f1=(struct _tagged_arr)_tmp4E;{struct Cyc_Std_Int_pa_struct
_tmp51;_tmp51.tag=1;_tmp51.f1=(int)_tmp4F;{void*_tmp50[2]={& _tmp51,& _tmp52};Cyc_Std_printf(
_tag_arr("$(%d,%s,",sizeof(unsigned char),9),_tag_arr(_tmp50,sizeof(void*),2));}}});
Cyc_Typerep_print_typestruct(_tmp4D);({struct Cyc_Std_Int_pa_struct _tmp54;_tmp54.tag=
1;_tmp54.f1=(int)((unsigned int)((int)(i != _get_arr_size(_tmp33,sizeof(struct
_tuple0*))- 1?',':' ')));{void*_tmp53[1]={& _tmp54};Cyc_Std_printf(_tag_arr(")%c",
sizeof(unsigned char),4),_tag_arr(_tmp53,sizeof(void*),1));}});}}({void*_tmp55[0]={};
Cyc_Std_printf(_tag_arr(" ]",sizeof(unsigned char),3),_tag_arr(_tmp55,sizeof(
void*),0));});goto _LL30;_LL3E:({struct Cyc_Std_Int_pa_struct _tmp57;_tmp57.tag=1;
_tmp57.f1=(int)_tmp37;{void*_tmp56[1]={& _tmp57};Cyc_Std_printf(_tag_arr("Tuple(%d,[",
sizeof(unsigned char),11),_tag_arr(_tmp56,sizeof(void*),1));}});{int i=0;for(0;i < 
_get_arr_size(_tmp36,sizeof(struct _tuple1*));i ++){void*_tmp59;unsigned int _tmp5A;
struct _tuple1 _tmp58=*((struct _tuple1**)_tmp36.curr)[i];_LL5C: _tmp5A=_tmp58.f1;
goto _LL5B;_LL5B: _tmp59=_tmp58.f2;goto _LL5A;_LL5A:({struct Cyc_Std_Int_pa_struct
_tmp5C;_tmp5C.tag=1;_tmp5C.f1=(int)_tmp5A;{void*_tmp5B[1]={& _tmp5C};Cyc_Std_printf(
_tag_arr("$(%d,",sizeof(unsigned char),6),_tag_arr(_tmp5B,sizeof(void*),1));}});
Cyc_Typerep_print_typestruct(_tmp59);({struct Cyc_Std_Int_pa_struct _tmp5E;_tmp5E.tag=
1;_tmp5E.f1=(int)((unsigned int)((int)(i != _get_arr_size(_tmp36,sizeof(struct
_tuple1*))- 1?',':' ')));{void*_tmp5D[1]={& _tmp5E};Cyc_Std_printf(_tag_arr(")%c",
sizeof(unsigned char),4),_tag_arr(_tmp5D,sizeof(void*),1));}});}}({void*_tmp5F[0]={};
Cyc_Std_printf(_tag_arr(" ]",sizeof(unsigned char),3),_tag_arr(_tmp5F,sizeof(
void*),0));});goto _LL30;_LL40:({struct Cyc_Std_String_pa_struct _tmp61;_tmp61.tag=
0;_tmp61.f1=(struct _tagged_arr)_tmp3A;{void*_tmp60[1]={& _tmp61};Cyc_Std_printf(
_tag_arr("TUnion(%s,[",sizeof(unsigned char),12),_tag_arr(_tmp60,sizeof(void*),1));}});{
int i=0;for(0;i < _get_arr_size(_tmp39,sizeof(struct _tuple2*));i ++){struct
_tagged_arr _tmp63;unsigned int _tmp64;struct _tuple2 _tmp62=*((struct _tuple2**)
_tmp39.curr)[i];_LL5F: _tmp64=_tmp62.f1;goto _LL5E;_LL5E: _tmp63=_tmp62.f2;goto
_LL5D;_LL5D:({struct Cyc_Std_Int_pa_struct _tmp68;_tmp68.tag=1;_tmp68.f1=(int)((
unsigned int)((int)(i != _get_arr_size(_tmp39,sizeof(struct _tuple2*))- 1?',':' ')));{
struct Cyc_Std_String_pa_struct _tmp67;_tmp67.tag=0;_tmp67.f1=(struct _tagged_arr)
_tmp63;{struct Cyc_Std_Int_pa_struct _tmp66;_tmp66.tag=1;_tmp66.f1=(int)_tmp64;{
void*_tmp65[3]={& _tmp66,& _tmp67,& _tmp68};Cyc_Std_printf(_tag_arr("$(%d,%s)%c",
sizeof(unsigned char),11),_tag_arr(_tmp65,sizeof(void*),3));}}}});}}({void*
_tmp69[0]={};Cyc_Std_printf(_tag_arr("],[",sizeof(unsigned char),4),_tag_arr(
_tmp69,sizeof(void*),0));});{int i=0;for(0;i < _get_arr_size(_tmp38,sizeof(struct
_tuple0*));i ++){void*_tmp6B;struct _tagged_arr _tmp6C;unsigned int _tmp6D;struct
_tuple0 _tmp6A=*((struct _tuple0**)_tmp38.curr)[i];_LL63: _tmp6D=_tmp6A.f1;goto
_LL62;_LL62: _tmp6C=_tmp6A.f2;goto _LL61;_LL61: _tmp6B=_tmp6A.f3;goto _LL60;_LL60:({
struct Cyc_Std_String_pa_struct _tmp70;_tmp70.tag=0;_tmp70.f1=(struct _tagged_arr)
_tmp6C;{struct Cyc_Std_Int_pa_struct _tmp6F;_tmp6F.tag=1;_tmp6F.f1=(int)_tmp6D;{
void*_tmp6E[2]={& _tmp6F,& _tmp70};Cyc_Std_printf(_tag_arr("$(%d,%s,",sizeof(
unsigned char),9),_tag_arr(_tmp6E,sizeof(void*),2));}}});Cyc_Typerep_print_typestruct(
_tmp6B);({struct Cyc_Std_Int_pa_struct _tmp72;_tmp72.tag=1;_tmp72.f1=(int)((
unsigned int)((int)(i != _get_arr_size(_tmp38,sizeof(struct _tuple0*))- 1?',':' ')));{
void*_tmp71[1]={& _tmp72};Cyc_Std_printf(_tag_arr(")%c",sizeof(unsigned char),4),
_tag_arr(_tmp71,sizeof(void*),1));}});}}({void*_tmp73[0]={};Cyc_Std_printf(
_tag_arr("] )",sizeof(unsigned char),4),_tag_arr(_tmp73,sizeof(void*),0));});
goto _LL30;_LL42:({struct Cyc_Std_String_pa_struct _tmp75;_tmp75.tag=0;_tmp75.f1=(
struct _tagged_arr)_tmp3C;{void*_tmp74[1]={& _tmp75};Cyc_Std_printf(_tag_arr("XTUnion(%s,",
sizeof(unsigned char),12),_tag_arr(_tmp74,sizeof(void*),1));}});{int i=0;for(0;i < 
_get_arr_size(_tmp3B,sizeof(struct _tuple3*));i ++){void*_tmp77;struct _tagged_arr
_tmp78;struct _tuple3 _tmp76=*((struct _tuple3**)_tmp3B.curr)[i];_LL66: _tmp78=
_tmp76.f1;goto _LL65;_LL65: _tmp77=_tmp76.f2;goto _LL64;_LL64:({struct Cyc_Std_String_pa_struct
_tmp7A;_tmp7A.tag=0;_tmp7A.f1=(struct _tagged_arr)_tagged_arr_plus(_tmp78,sizeof(
unsigned char),4);{void*_tmp79[1]={& _tmp7A};Cyc_Std_printf(_tag_arr("$(%s,",
sizeof(unsigned char),6),_tag_arr(_tmp79,sizeof(void*),1));}});Cyc_Typerep_print_typestruct(
_tmp77);({struct Cyc_Std_Int_pa_struct _tmp7C;_tmp7C.tag=1;_tmp7C.f1=(int)((
unsigned int)((int)(i != _get_arr_size(_tmp3B,sizeof(struct _tuple3*))- 1?',':' ')));{
void*_tmp7B[1]={& _tmp7C};Cyc_Std_printf(_tag_arr(")%c",sizeof(unsigned char),4),
_tag_arr(_tmp7B,sizeof(void*),1));}});}}({void*_tmp7D[0]={};Cyc_Std_printf(
_tag_arr(" )",sizeof(unsigned char),3),_tag_arr(_tmp7D,sizeof(void*),0));});goto
_LL30;_LL44:({struct Cyc_Std_String_pa_struct _tmp7F;_tmp7F.tag=0;_tmp7F.f1=(
struct _tagged_arr)Cyc_Typerep_anon_or_name(_tmp3E);{void*_tmp7E[1]={& _tmp7F};Cyc_Std_printf(
_tag_arr("Union(%s,",sizeof(unsigned char),10),_tag_arr(_tmp7E,sizeof(void*),1));}});{
int i=0;for(0;i < _get_arr_size(_tmp3D,sizeof(struct _tuple3*));i ++){void*_tmp81;
struct _tagged_arr _tmp82;struct _tuple3 _tmp80=*((struct _tuple3**)_tmp3D.curr)[i];
_LL69: _tmp82=_tmp80.f1;goto _LL68;_LL68: _tmp81=_tmp80.f2;goto _LL67;_LL67:({struct
Cyc_Std_String_pa_struct _tmp84;_tmp84.tag=0;_tmp84.f1=(struct _tagged_arr)_tmp82;{
void*_tmp83[1]={& _tmp84};Cyc_Std_printf(_tag_arr("$(%s,",sizeof(unsigned char),6),
_tag_arr(_tmp83,sizeof(void*),1));}});Cyc_Typerep_print_typestruct(_tmp81);({
struct Cyc_Std_Int_pa_struct _tmp86;_tmp86.tag=1;_tmp86.f1=(int)((unsigned int)((
int)(i != _get_arr_size(_tmp3D,sizeof(struct _tuple3*))- 1?',':' ')));{void*_tmp85[
1]={& _tmp86};Cyc_Std_printf(_tag_arr(")%c",sizeof(unsigned char),4),_tag_arr(
_tmp85,sizeof(void*),1));}});}}({void*_tmp87[0]={};Cyc_Std_printf(_tag_arr(")",
sizeof(unsigned char),2),_tag_arr(_tmp87,sizeof(void*),0));});goto _LL30;_LL30:;}
void Cyc_Typerep_xmlize_typestruct(void*rep){void*_tmp88=rep;unsigned int _tmp89;
int _tmp8A;void*_tmp8B;unsigned int _tmp8C;void*_tmp8D;struct _tagged_arr _tmp8E;
unsigned int _tmp8F;struct _tagged_arr*_tmp90;struct _tagged_arr _tmp91;unsigned int
_tmp92;struct _tagged_arr _tmp93;struct _tagged_arr _tmp94;struct _tagged_arr _tmp95;
struct _tagged_arr _tmp96;struct _tagged_arr _tmp97;struct _tagged_arr _tmp98;struct
_tagged_arr*_tmp99;_LL6B: if((unsigned int)_tmp88 > 2?*((int*)_tmp88)== 0: 0){_LL80:
_tmp8A=((struct Cyc_Typerep_Int_struct*)_tmp88)->f1;goto _LL7F;_LL7F: _tmp89=((
struct Cyc_Typerep_Int_struct*)_tmp88)->f2;goto _LL6C;}else{goto _LL6D;}_LL6D: if((
int)_tmp88 == 0){goto _LL6E;}else{goto _LL6F;}_LL6F: if((int)_tmp88 == 1){goto _LL70;}
else{goto _LL71;}_LL71: if((unsigned int)_tmp88 > 2?*((int*)_tmp88)== 1: 0){_LL82:
_tmp8C=((struct Cyc_Typerep_ThinPtr_struct*)_tmp88)->f1;goto _LL81;_LL81: _tmp8B=(
void*)((struct Cyc_Typerep_ThinPtr_struct*)_tmp88)->f2;goto _LL72;}else{goto _LL73;}
_LL73: if((unsigned int)_tmp88 > 2?*((int*)_tmp88)== 2: 0){_LL83: _tmp8D=(void*)((
struct Cyc_Typerep_FatPtr_struct*)_tmp88)->f1;goto _LL74;}else{goto _LL75;}_LL75:
if((unsigned int)_tmp88 > 2?*((int*)_tmp88)== 3: 0){_LL86: _tmp90=((struct Cyc_Typerep_Struct_struct*)
_tmp88)->f1;goto _LL85;_LL85: _tmp8F=((struct Cyc_Typerep_Struct_struct*)_tmp88)->f2;
goto _LL84;_LL84: _tmp8E=((struct Cyc_Typerep_Struct_struct*)_tmp88)->f3;goto _LL76;}
else{goto _LL77;}_LL77: if((unsigned int)_tmp88 > 2?*((int*)_tmp88)== 4: 0){_LL88:
_tmp92=((struct Cyc_Typerep_Tuple_struct*)_tmp88)->f1;goto _LL87;_LL87: _tmp91=((
struct Cyc_Typerep_Tuple_struct*)_tmp88)->f2;goto _LL78;}else{goto _LL79;}_LL79: if((
unsigned int)_tmp88 > 2?*((int*)_tmp88)== 5: 0){_LL8B: _tmp95=((struct Cyc_Typerep_TUnion_struct*)
_tmp88)->f1;goto _LL8A;_LL8A: _tmp94=((struct Cyc_Typerep_TUnion_struct*)_tmp88)->f2;
goto _LL89;_LL89: _tmp93=((struct Cyc_Typerep_TUnion_struct*)_tmp88)->f3;goto _LL7A;}
else{goto _LL7B;}_LL7B: if((unsigned int)_tmp88 > 2?*((int*)_tmp88)== 6: 0){_LL8D:
_tmp97=((struct Cyc_Typerep_XTUnion_struct*)_tmp88)->f1;goto _LL8C;_LL8C: _tmp96=((
struct Cyc_Typerep_XTUnion_struct*)_tmp88)->f2;goto _LL7C;}else{goto _LL7D;}_LL7D:
if((unsigned int)_tmp88 > 2?*((int*)_tmp88)== 7: 0){_LL8F: _tmp99=((struct Cyc_Typerep_Union_struct*)
_tmp88)->f1;goto _LL8E;_LL8E: _tmp98=((struct Cyc_Typerep_Union_struct*)_tmp88)->f2;
goto _LL7E;}else{goto _LL6A;}_LL6C:({struct Cyc_Std_Int_pa_struct _tmp9C;_tmp9C.tag=
1;_tmp9C.f1=(int)_tmp89;{struct Cyc_Std_Int_pa_struct _tmp9B;_tmp9B.tag=1;_tmp9B.f1=(
int)((unsigned int)_tmp8A);{void*_tmp9A[2]={& _tmp9B,& _tmp9C};Cyc_Std_printf(
_tag_arr("<Int sn=%d sz=\"%d\"/>",sizeof(unsigned char),21),_tag_arr(_tmp9A,
sizeof(void*),2));}}});goto _LL6A;_LL6E:({void*_tmp9D[0]={};Cyc_Std_printf(
_tag_arr("<Float/>",sizeof(unsigned char),9),_tag_arr(_tmp9D,sizeof(void*),0));});
goto _LL6A;_LL70:({void*_tmp9E[0]={};Cyc_Std_printf(_tag_arr("<Double/>",sizeof(
unsigned char),10),_tag_arr(_tmp9E,sizeof(void*),0));});goto _LL6A;_LL72:({struct
Cyc_Std_Int_pa_struct _tmpA0;_tmpA0.tag=1;_tmpA0.f1=(int)_tmp8C;{void*_tmp9F[1]={&
_tmpA0};Cyc_Std_printf(_tag_arr("<ThinPtr sz=\"%d\">",sizeof(unsigned char),18),
_tag_arr(_tmp9F,sizeof(void*),1));}});Cyc_Typerep_xmlize_typestruct(_tmp8B);({
void*_tmpA1[0]={};Cyc_Std_printf(_tag_arr("</ThinPtr",sizeof(unsigned char),10),
_tag_arr(_tmpA1,sizeof(void*),0));});goto _LL6A;_LL74:({void*_tmpA2[0]={};Cyc_Std_printf(
_tag_arr("<FatPtr>",sizeof(unsigned char),9),_tag_arr(_tmpA2,sizeof(void*),0));});
Cyc_Typerep_xmlize_typestruct(_tmp8D);({void*_tmpA3[0]={};Cyc_Std_printf(
_tag_arr("</FatPtr>",sizeof(unsigned char),10),_tag_arr(_tmpA3,sizeof(void*),0));});
goto _LL6A;_LL76:({struct Cyc_Std_Int_pa_struct _tmpA6;_tmpA6.tag=1;_tmpA6.f1=(int)
_tmp8F;{struct Cyc_Std_String_pa_struct _tmpA5;_tmpA5.tag=0;_tmpA5.f1=(struct
_tagged_arr)Cyc_Typerep_anon_or_name(_tmp90);{void*_tmpA4[2]={& _tmpA5,& _tmpA6};
Cyc_Std_printf(_tag_arr("<Struct name=\"%s\" sz=\"%d\">",sizeof(unsigned char),
27),_tag_arr(_tmpA4,sizeof(void*),2));}}});{int i=0;for(0;i < _get_arr_size(_tmp8E,
sizeof(struct _tuple0*));i ++){void*_tmpA8;struct _tagged_arr _tmpA9;unsigned int
_tmpAA;struct _tuple0 _tmpA7=*((struct _tuple0**)_tmp8E.curr)[i];_LL93: _tmpAA=
_tmpA7.f1;goto _LL92;_LL92: _tmpA9=_tmpA7.f2;goto _LL91;_LL91: _tmpA8=_tmpA7.f3;goto
_LL90;_LL90:({struct Cyc_Std_String_pa_struct _tmpAD;_tmpAD.tag=0;_tmpAD.f1=(
struct _tagged_arr)_tmpA9;{struct Cyc_Std_Int_pa_struct _tmpAC;_tmpAC.tag=1;_tmpAC.f1=(
int)_tmpAA;{void*_tmpAB[2]={& _tmpAC,& _tmpAD};Cyc_Std_printf(_tag_arr("<Component ofs=\"%d\" nm=\"%s\">",
sizeof(unsigned char),29),_tag_arr(_tmpAB,sizeof(void*),2));}}});Cyc_Typerep_xmlize_typestruct(
_tmpA8);({void*_tmpAE[0]={};Cyc_Std_printf(_tag_arr("</Component>",sizeof(
unsigned char),13),_tag_arr(_tmpAE,sizeof(void*),0));});}}({void*_tmpAF[0]={};
Cyc_Std_printf(_tag_arr("</Tuple>",sizeof(unsigned char),9),_tag_arr(_tmpAF,
sizeof(void*),0));});goto _LL6A;_LL78:({struct Cyc_Std_Int_pa_struct _tmpB1;_tmpB1.tag=
1;_tmpB1.f1=(int)_tmp92;{void*_tmpB0[1]={& _tmpB1};Cyc_Std_printf(_tag_arr("<Tuple sz=\"%d\">",
sizeof(unsigned char),16),_tag_arr(_tmpB0,sizeof(void*),1));}});{int i=0;for(0;i < 
_get_arr_size(_tmp91,sizeof(struct _tuple1*));i ++){void*_tmpB3;unsigned int _tmpB4;
struct _tuple1 _tmpB2=*((struct _tuple1**)_tmp91.curr)[i];_LL96: _tmpB4=_tmpB2.f1;
goto _LL95;_LL95: _tmpB3=_tmpB2.f2;goto _LL94;_LL94:({struct Cyc_Std_Int_pa_struct
_tmpB6;_tmpB6.tag=1;_tmpB6.f1=(int)_tmpB4;{void*_tmpB5[1]={& _tmpB6};Cyc_Std_printf(
_tag_arr("<Component ofs=\"%d\">",sizeof(unsigned char),21),_tag_arr(_tmpB5,
sizeof(void*),1));}});Cyc_Typerep_xmlize_typestruct(_tmpB3);({void*_tmpB7[0]={};
Cyc_Std_printf(_tag_arr("</Component>",sizeof(unsigned char),13),_tag_arr(_tmpB7,
sizeof(void*),0));});}}({void*_tmpB8[0]={};Cyc_Std_printf(_tag_arr("</Tuple>",
sizeof(unsigned char),9),_tag_arr(_tmpB8,sizeof(void*),0));});goto _LL6A;_LL7A:({
struct Cyc_Std_String_pa_struct _tmpBA;_tmpBA.tag=0;_tmpBA.f1=(struct _tagged_arr)
_tmp95;{void*_tmpB9[1]={& _tmpBA};Cyc_Std_printf(_tag_arr("<TUnion name=\"%s\">",
sizeof(unsigned char),19),_tag_arr(_tmpB9,sizeof(void*),1));}});{int i=0;for(0;i < 
_get_arr_size(_tmp94,sizeof(struct _tuple2*));i ++){struct _tagged_arr _tmpBC;
unsigned int _tmpBD;struct _tuple2 _tmpBB=*((struct _tuple2**)_tmp94.curr)[i];_LL99:
_tmpBD=_tmpBB.f1;goto _LL98;_LL98: _tmpBC=_tmpBB.f2;goto _LL97;_LL97:({struct Cyc_Std_String_pa_struct
_tmpC0;_tmpC0.tag=0;_tmpC0.f1=(struct _tagged_arr)_tmpBC;{struct Cyc_Std_Int_pa_struct
_tmpBF;_tmpBF.tag=1;_tmpBF.f1=(int)_tmpBD;{void*_tmpBE[2]={& _tmpBF,& _tmpC0};Cyc_Std_printf(
_tag_arr("<Tag tag=\"%d\" nm=\"%s\"/>",sizeof(unsigned char),24),_tag_arr(_tmpBE,
sizeof(void*),2));}}});}}{int i=0;for(0;i < _get_arr_size(_tmp93,sizeof(struct
_tuple0*));i ++){void*_tmpC2;struct _tagged_arr _tmpC3;unsigned int _tmpC4;struct
_tuple0 _tmpC1=*((struct _tuple0**)_tmp93.curr)[i];_LL9D: _tmpC4=_tmpC1.f1;goto
_LL9C;_LL9C: _tmpC3=_tmpC1.f2;goto _LL9B;_LL9B: _tmpC2=_tmpC1.f3;goto _LL9A;_LL9A:({
struct Cyc_Std_String_pa_struct _tmpC7;_tmpC7.tag=0;_tmpC7.f1=(struct _tagged_arr)
_tmpC3;{struct Cyc_Std_Int_pa_struct _tmpC6;_tmpC6.tag=1;_tmpC6.f1=(int)_tmpC4;{
void*_tmpC5[2]={& _tmpC6,& _tmpC7};Cyc_Std_printf(_tag_arr("<Tag tag=\"%d\" nm=\"%s\">",
sizeof(unsigned char),23),_tag_arr(_tmpC5,sizeof(void*),2));}}});Cyc_Typerep_xmlize_typestruct(
_tmpC2);({void*_tmpC8[0]={};Cyc_Std_printf(_tag_arr("</Tag>",sizeof(
unsigned char),7),_tag_arr(_tmpC8,sizeof(void*),0));});}}({void*_tmpC9[0]={};Cyc_Std_printf(
_tag_arr("</Tunion>",sizeof(unsigned char),10),_tag_arr(_tmpC9,sizeof(void*),0));});
goto _LL6A;_LL7C:({struct Cyc_Std_String_pa_struct _tmpCB;_tmpCB.tag=0;_tmpCB.f1=(
struct _tagged_arr)_tmp97;{void*_tmpCA[1]={& _tmpCB};Cyc_Std_printf(_tag_arr("<XTUnion name=\"%s\">",
sizeof(unsigned char),20),_tag_arr(_tmpCA,sizeof(void*),1));}});{int i=0;for(0;i < 
_get_arr_size(_tmp96,sizeof(struct _tuple3*));i ++){void*_tmpCD;struct _tagged_arr
_tmpCE;struct _tuple3 _tmpCC=*((struct _tuple3**)_tmp96.curr)[i];_LLA0: _tmpCE=
_tmpCC.f1;goto _LL9F;_LL9F: _tmpCD=_tmpCC.f2;goto _LL9E;_LL9E:({struct Cyc_Std_String_pa_struct
_tmpD0;_tmpD0.tag=0;_tmpD0.f1=(struct _tagged_arr)_tagged_arr_plus(_tmpCE,sizeof(
unsigned char),4);{void*_tmpCF[1]={& _tmpD0};Cyc_Std_printf(_tag_arr("<Tag tag=\"%s\">",
sizeof(unsigned char),15),_tag_arr(_tmpCF,sizeof(void*),1));}});Cyc_Typerep_xmlize_typestruct(
_tmpCD);({void*_tmpD1[0]={};Cyc_Std_printf(_tag_arr("</Tag>",sizeof(
unsigned char),7),_tag_arr(_tmpD1,sizeof(void*),0));});}}({void*_tmpD2[0]={};Cyc_Std_printf(
_tag_arr("</XTunion>",sizeof(unsigned char),11),_tag_arr(_tmpD2,sizeof(void*),0));});
goto _LL6A;_LL7E:({struct Cyc_Std_String_pa_struct _tmpD4;_tmpD4.tag=0;_tmpD4.f1=(
struct _tagged_arr)Cyc_Typerep_anon_or_name(_tmp99);{void*_tmpD3[1]={& _tmpD4};Cyc_Std_printf(
_tag_arr("<Union name=\"%s\">",sizeof(unsigned char),18),_tag_arr(_tmpD3,sizeof(
void*),1));}});{int i=0;for(0;i < _get_arr_size(_tmp98,sizeof(struct _tuple3*));i ++){
void*_tmpD6;struct _tagged_arr _tmpD7;struct _tuple3 _tmpD5=*((struct _tuple3**)
_tmp98.curr)[i];_LLA3: _tmpD7=_tmpD5.f1;goto _LLA2;_LLA2: _tmpD6=_tmpD5.f2;goto
_LLA1;_LLA1:({struct Cyc_Std_String_pa_struct _tmpD9;_tmpD9.tag=0;_tmpD9.f1=(
struct _tagged_arr)_tmpD7;{void*_tmpD8[1]={& _tmpD9};Cyc_Std_printf(_tag_arr("<Case nm=\"%s\">",
sizeof(unsigned char),15),_tag_arr(_tmpD8,sizeof(void*),1));}});Cyc_Typerep_xmlize_typestruct(
_tmpD6);({void*_tmpDA[0]={};Cyc_Std_printf(_tag_arr("</Case>",sizeof(
unsigned char),8),_tag_arr(_tmpDA,sizeof(void*),0));});}}({void*_tmpDB[0]={};Cyc_Std_printf(
_tag_arr("</Union>",sizeof(unsigned char),9),_tag_arr(_tmpDB,sizeof(void*),0));});
goto _LL6A;_LL6A:;}
