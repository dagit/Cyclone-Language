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
 struct Cyc_Std__types_fd_set{int fds_bits[2];};struct Cyc_Core_Opt{void*v;};extern
unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct{unsigned char*tag;struct _tagged_arr f1;};extern
unsigned char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Not_found[14];
extern unsigned char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Cstdio___abstractFILE;struct
Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*Cyc_Std_stderr;extern
unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};extern int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,struct
_tagged_arr fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};extern int
Cyc_Std_sscanf(struct _tagged_arr src,struct _tagged_arr fmt,struct _tagged_arr);
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern unsigned char Cyc_List_List_mismatch[
18];extern unsigned char Cyc_List_Nth[8];extern unsigned int Cyc_Std_strlen(struct
_tagged_arr s);extern int Cyc_Std_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);
extern int Cyc_Std_strncmp(struct _tagged_arr s1,struct _tagged_arr s2,unsigned int len);
extern struct _tagged_arr Cyc_Std_strconcat(struct _tagged_arr,struct _tagged_arr);
extern unsigned char Cyc_Arg_Bad[8];struct Cyc_Arg_Bad_struct{unsigned char*tag;
struct _tagged_arr f1;};extern unsigned char Cyc_Arg_Error[10];struct Cyc_Arg_Unit_spec_struct{
int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_struct{int tag;void(*f1)(struct
_tagged_arr);};struct Cyc_Arg_FlagString_spec_struct{int tag;void(*f1)(struct
_tagged_arr,struct _tagged_arr);};struct Cyc_Arg_Set_spec_struct{int tag;int*f1;};
struct Cyc_Arg_Clear_spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_struct{
int tag;void(*f1)(struct _tagged_arr);};struct Cyc_Arg_Int_spec_struct{int tag;void(*
f1)(int);};struct Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct _tagged_arr);};
extern void Cyc_Arg_usage(struct Cyc_List_List*,struct _tagged_arr);extern int Cyc_Arg_current;
extern void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct
_tagged_arr),struct _tagged_arr errmsg,struct _tagged_arr args);struct Cyc_Buffer_t;
extern struct Cyc_Buffer_t*Cyc_Buffer_create(unsigned int n);extern struct
_tagged_arr Cyc_Buffer_contents(struct Cyc_Buffer_t*);extern void Cyc_Buffer_add_substring(
struct Cyc_Buffer_t*,struct _tagged_arr,int offset,int len);extern void Cyc_Buffer_add_string(
struct Cyc_Buffer_t*,struct _tagged_arr);extern int isspace(int);extern void*Cyc_Std___assert_fail(
struct _tagged_arr assertion,struct _tagged_arr file,unsigned int line);unsigned char
Cyc_Arg_Bad[8]="\000\000\000\000Bad";unsigned char Cyc_Arg_Error[10]="\000\000\000\000Error";
struct Cyc_Arg_Prefix_struct{int tag;struct _tagged_arr f1;};struct Cyc_Arg_Exact_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Arg_Unknown_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Arg_Missing_struct{int tag;struct _tagged_arr f1;};struct
Cyc_Arg_Message_struct{int tag;struct _tagged_arr f1;};struct Cyc_Arg_Wrong_struct{
int tag;struct _tagged_arr f1;struct _tagged_arr f2;struct _tagged_arr f3;};struct
_tuple0{struct _tagged_arr f1;int f2;struct _tagged_arr f3;void*f4;struct _tagged_arr
f5;};static void*Cyc_Arg_lookup(struct Cyc_List_List*l,struct _tagged_arr x){while(l
!= 0){struct _tagged_arr _tmp0=(*((struct _tuple0*)l->hd)).f1;unsigned int _tmp1=Cyc_Std_strlen(
_tmp0);if(_tmp1 > 0?(*((struct _tuple0*)l->hd)).f2: 0){if(Cyc_Std_strncmp(x,(*((
struct _tuple0*)l->hd)).f1,_tmp1)== 0){return(*((struct _tuple0*)l->hd)).f4;}}
else{if(Cyc_Std_strcmp(x,(*((struct _tuple0*)l->hd)).f1)== 0){return(*((struct
_tuple0*)l->hd)).f4;}}l=l->tl;}(int)_throw((void*)Cyc_Core_Not_found);}static
struct _tagged_arr Cyc_Arg_Justify_break_line(struct Cyc_Buffer_t*b,int howmuch,
struct _tagged_arr s){if(s.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr){
return(struct _tagged_arr)_tag_arr(0,0,0);}if(howmuch < 0){howmuch=0;}{
unsigned int _tmp2=Cyc_Std_strlen(s);_tmp2 <= _get_arr_size(s,sizeof(unsigned char))?
0:((int(*)(struct _tagged_arr assertion,struct _tagged_arr file,unsigned int line))
Cyc_Std___assert_fail)(_tag_arr("len <= s.size",sizeof(unsigned char),14),
_tag_arr("arg.cyc",sizeof(unsigned char),8),94);if(howmuch > _tmp2){Cyc_Buffer_add_string(
b,s);return(struct _tagged_arr)_tag_arr(0,0,0);}{int i;for(i=howmuch - 1;i >= 0? !
isspace((int)((const unsigned char*)s.curr)[i]): 0;i --){;}if(i < 0){for(i=howmuch?
howmuch - 1: 0;(i < _tmp2?(int)((const unsigned char*)s.curr)[i]: 0)? ! isspace((int)((
const unsigned char*)s.curr)[i]): 0;i ++){;}}Cyc_Buffer_add_substring(b,s,0,i);{
struct _tagged_arr whatsleft=(struct _tagged_arr)_tag_arr(0,0,0);for(0;(i < _tmp2?(
int)((const unsigned char*)s.curr)[i]: 0)? isspace((int)((const unsigned char*)s.curr)[
i]): 0;i ++){;}if(i < _tmp2?(int)((const unsigned char*)s.curr)[i]: 0){whatsleft=
_tagged_arr_plus(s,sizeof(unsigned char),i);}return whatsleft;}}}}void Cyc_Arg_Justify_justify_b(
struct Cyc_Buffer_t*b,int indent,int margin,struct _tagged_arr item,struct _tagged_arr
desc){if(item.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr){Cyc_Buffer_add_string(
b,item);}if(desc.curr == ((struct _tagged_arr)_tag_arr(0,0,0)).curr){return;}if(
indent < 0){indent=0;}if(margin < 0){margin=0;}{struct _tagged_arr indentstr=({
unsigned int _tmp8=(unsigned int)(indent + 1);unsigned char*_tmp9=(unsigned char*)
_cycalloc_atomic(_check_times(sizeof(unsigned char),_tmp8));struct _tagged_arr
_tmpB=_tag_arr(_tmp9,sizeof(unsigned char),(unsigned int)(indent + 1));{
unsigned int _tmpA=_tmp8;unsigned int i;for(i=0;i < _tmpA;i ++){_tmp9[i]=i == 0?'\n':' ';}};
_tmpB;});unsigned int _tmp3=Cyc_Std_strlen(item);struct _tagged_arr itemsep;if(Cyc_Std_strlen(
desc)> 0){if(_tmp3 + 1 > indent){itemsep=indentstr;}else{itemsep=({unsigned int
_tmp4=indent - _tmp3;unsigned char*_tmp5=(unsigned char*)_cycalloc_atomic(
_check_times(sizeof(unsigned char),_tmp4));struct _tagged_arr _tmp7=_tag_arr(_tmp5,
sizeof(unsigned char),indent - _tmp3);{unsigned int _tmp6=_tmp4;unsigned int i;for(
i=0;i < _tmp6;i ++){_tmp5[i]=' ';}};_tmp7;});}}else{return;}Cyc_Buffer_add_string(
b,itemsep);while(desc.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr){desc=Cyc_Arg_Justify_break_line(
b,margin - indent,desc);if(desc.curr != ((struct _tagged_arr)_tag_arr(0,0,0)).curr){
Cyc_Buffer_add_string(b,indentstr);}else{Cyc_Buffer_add_string(b,_tag_arr("\n",
sizeof(unsigned char),2));}}return;}}void Cyc_Arg_usage(struct Cyc_List_List*
speclist,struct _tagged_arr errmsg){({struct Cyc_Std_String_pa_struct _tmpD;_tmpD.tag=
0;_tmpD.f1=(struct _tagged_arr)errmsg;{void*_tmpC[1]={& _tmpD};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("%s\n",sizeof(unsigned char),4),_tag_arr(_tmpC,sizeof(void*),1));}});{
struct Cyc_Buffer_t*_tmpE=Cyc_Buffer_create(1024);while(speclist != 0){Cyc_Arg_Justify_justify_b(
_tmpE,12,72,(struct _tagged_arr)Cyc_Std_strconcat((*((struct _tuple0*)speclist->hd)).f1,(*((
struct _tuple0*)speclist->hd)).f3),(*((struct _tuple0*)speclist->hd)).f5);speclist=
speclist->tl;}({struct Cyc_Std_String_pa_struct _tmp10;_tmp10.tag=0;_tmp10.f1=(
struct _tagged_arr)Cyc_Buffer_contents(_tmpE);{void*_tmpF[1]={& _tmp10};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(_tmpF,sizeof(void*),
1));}});}}int Cyc_Arg_current=0;static struct _tagged_arr Cyc_Arg_args={(void*)0,(
void*)0,(void*)(0 + 0)};static void Cyc_Arg_stop(int prog_pos,void*e,struct Cyc_List_List*
speclist,struct _tagged_arr errmsg){struct _tagged_arr progname=prog_pos < 
_get_arr_size(Cyc_Arg_args,sizeof(struct _tagged_arr))?*((struct _tagged_arr*)
_check_unknown_subscript(Cyc_Arg_args,sizeof(struct _tagged_arr),prog_pos)):
_tag_arr("(?)",sizeof(unsigned char),4);{void*_tmp11=e;struct _tagged_arr _tmp12;
struct _tagged_arr _tmp13;struct _tagged_arr _tmp14;struct _tagged_arr _tmp15;struct
_tagged_arr _tmp16;struct _tagged_arr _tmp17;_LL1: if(*((int*)_tmp11)== 0){_LL9:
_tmp12=((struct Cyc_Arg_Unknown_struct*)_tmp11)->f1;goto _LL2;}else{goto _LL3;}_LL3:
if(*((int*)_tmp11)== 1){_LLA: _tmp13=((struct Cyc_Arg_Missing_struct*)_tmp11)->f1;
goto _LL4;}else{goto _LL5;}_LL5: if(*((int*)_tmp11)== 3){_LLD: _tmp16=((struct Cyc_Arg_Wrong_struct*)
_tmp11)->f1;goto _LLC;_LLC: _tmp15=((struct Cyc_Arg_Wrong_struct*)_tmp11)->f2;goto
_LLB;_LLB: _tmp14=((struct Cyc_Arg_Wrong_struct*)_tmp11)->f3;goto _LL6;}else{goto
_LL7;}_LL7: if(*((int*)_tmp11)== 2){_LLE: _tmp17=((struct Cyc_Arg_Message_struct*)
_tmp11)->f1;goto _LL8;}else{goto _LL0;}_LL2: if(Cyc_Std_strcmp(_tmp12,_tag_arr("-help",
sizeof(unsigned char),6))!= 0){({struct Cyc_Std_String_pa_struct _tmp1A;_tmp1A.tag=
0;_tmp1A.f1=(struct _tagged_arr)_tmp12;{struct Cyc_Std_String_pa_struct _tmp19;
_tmp19.tag=0;_tmp19.f1=(struct _tagged_arr)progname;{void*_tmp18[2]={& _tmp19,&
_tmp1A};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s: unknown option `%s'.\n",
sizeof(unsigned char),26),_tag_arr(_tmp18,sizeof(void*),2));}}});}goto _LL0;_LL4:({
struct Cyc_Std_String_pa_struct _tmp1D;_tmp1D.tag=0;_tmp1D.f1=(struct _tagged_arr)
_tmp13;{struct Cyc_Std_String_pa_struct _tmp1C;_tmp1C.tag=0;_tmp1C.f1=(struct
_tagged_arr)progname;{void*_tmp1B[2]={& _tmp1C,& _tmp1D};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("%s: option `%s' needs an argument.\n",sizeof(unsigned char),36),
_tag_arr(_tmp1B,sizeof(void*),2));}}});goto _LL0;_LL6:({struct Cyc_Std_String_pa_struct
_tmp22;_tmp22.tag=0;_tmp22.f1=(struct _tagged_arr)_tmp14;{struct Cyc_Std_String_pa_struct
_tmp21;_tmp21.tag=0;_tmp21.f1=(struct _tagged_arr)_tmp16;{struct Cyc_Std_String_pa_struct
_tmp20;_tmp20.tag=0;_tmp20.f1=(struct _tagged_arr)_tmp15;{struct Cyc_Std_String_pa_struct
_tmp1F;_tmp1F.tag=0;_tmp1F.f1=(struct _tagged_arr)progname;{void*_tmp1E[4]={&
_tmp1F,& _tmp20,& _tmp21,& _tmp22};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("%s: wrong argument `%s'; option `%s' expects %s.\n",
sizeof(unsigned char),50),_tag_arr(_tmp1E,sizeof(void*),4));}}}}});goto _LL0;_LL8:({
struct Cyc_Std_String_pa_struct _tmp25;_tmp25.tag=0;_tmp25.f1=(struct _tagged_arr)
_tmp17;{struct Cyc_Std_String_pa_struct _tmp24;_tmp24.tag=0;_tmp24.f1=(struct
_tagged_arr)progname;{void*_tmp23[2]={& _tmp24,& _tmp25};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("%s: %s.\n",sizeof(unsigned char),9),_tag_arr(_tmp23,sizeof(void*),2));}}});
goto _LL0;_LL0:;}Cyc_Arg_usage(speclist,errmsg);Cyc_Arg_current=(int)
_get_arr_size(Cyc_Arg_args,sizeof(struct _tagged_arr));}void Cyc_Arg_parse(struct
Cyc_List_List*speclist,void(*anonfun)(struct _tagged_arr),struct _tagged_arr errmsg,
struct _tagged_arr orig_args){Cyc_Arg_args=orig_args;{int initpos=Cyc_Arg_current;
unsigned int l=_get_arr_size(Cyc_Arg_args,sizeof(struct _tagged_arr));++ Cyc_Arg_current;
while(Cyc_Arg_current < l){struct _tagged_arr s=*((struct _tagged_arr*)
_check_unknown_subscript(Cyc_Arg_args,sizeof(struct _tagged_arr),Cyc_Arg_current));
if(_get_arr_size(s,sizeof(unsigned char))>= 1?*((const unsigned char*)
_check_unknown_subscript(s,sizeof(unsigned char),0))== '-': 0){void*action;{
struct _handler_cons _tmp26;_push_handler(& _tmp26);{int _tmp28=0;if(setjmp(_tmp26.handler)){
_tmp28=1;}if(! _tmp28){action=Cyc_Arg_lookup(speclist,s);;_pop_handler();}else{
void*_tmp27=(void*)_exn_thrown;void*_tmp2A=_tmp27;_LL10: if(_tmp2A == Cyc_Core_Not_found){
goto _LL11;}else{goto _LL12;}_LL12: goto _LL13;_LL11: Cyc_Arg_stop(initpos,(void*)({
struct Cyc_Arg_Unknown_struct*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B[0]=({
struct Cyc_Arg_Unknown_struct _tmp2C;_tmp2C.tag=0;_tmp2C.f1=s;_tmp2C;});_tmp2B;}),
speclist,errmsg);return;_LL13:(void)_throw(_tmp2A);_LLF:;}}}{struct _handler_cons
_tmp2D;_push_handler(& _tmp2D);{int _tmp2F=0;if(setjmp(_tmp2D.handler)){_tmp2F=1;}
if(! _tmp2F){{void*_tmp30=action;void(*_tmp31)();void(*_tmp32)(struct _tagged_arr);
int*_tmp33;int*_tmp34;void(*_tmp35)(struct _tagged_arr,struct _tagged_arr);void(*
_tmp36)(struct _tagged_arr);void(*_tmp37)(int);void(*_tmp38)(struct _tagged_arr);
_LL15: if(*((int*)_tmp30)== 0){_LL25: _tmp31=((struct Cyc_Arg_Unit_spec_struct*)
_tmp30)->f1;goto _LL16;}else{goto _LL17;}_LL17: if(*((int*)_tmp30)== 1){_LL26:
_tmp32=((struct Cyc_Arg_Flag_spec_struct*)_tmp30)->f1;goto _LL18;}else{goto _LL19;}
_LL19: if(*((int*)_tmp30)== 3){_LL27: _tmp33=((struct Cyc_Arg_Set_spec_struct*)
_tmp30)->f1;goto _LL1A;}else{goto _LL1B;}_LL1B: if(*((int*)_tmp30)== 4){_LL28:
_tmp34=((struct Cyc_Arg_Clear_spec_struct*)_tmp30)->f1;goto _LL1C;}else{goto _LL1D;}
_LL1D: if(*((int*)_tmp30)== 2){_LL29: _tmp35=((struct Cyc_Arg_FlagString_spec_struct*)
_tmp30)->f1;goto _LL1E;}else{goto _LL1F;}_LL1F: if(*((int*)_tmp30)== 5){_LL2A:
_tmp36=((struct Cyc_Arg_String_spec_struct*)_tmp30)->f1;goto _LL20;}else{goto _LL21;}
_LL21: if(*((int*)_tmp30)== 6){_LL2B: _tmp37=((struct Cyc_Arg_Int_spec_struct*)
_tmp30)->f1;goto _LL22;}else{goto _LL23;}_LL23: if(*((int*)_tmp30)== 7){_LL2C:
_tmp38=((struct Cyc_Arg_Rest_spec_struct*)_tmp30)->f1;goto _LL24;}else{goto _LL14;}
_LL16: _tmp31();goto _LL14;_LL18: _tmp32(s);goto _LL14;_LL1A:*_tmp33=1;goto _LL14;
_LL1C:*_tmp34=0;goto _LL14;_LL1E: if(Cyc_Arg_current + 1 < l){_tmp35(s,*((struct
_tagged_arr*)_check_unknown_subscript(Cyc_Arg_args,sizeof(struct _tagged_arr),Cyc_Arg_current
+ 1)));++ Cyc_Arg_current;}else{Cyc_Arg_stop(initpos,(void*)({struct Cyc_Arg_Missing_struct*
_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39[0]=({struct Cyc_Arg_Missing_struct _tmp3A;
_tmp3A.tag=1;_tmp3A.f1=s;_tmp3A;});_tmp39;}),speclist,errmsg);}goto _LL14;_LL20:
if(Cyc_Arg_current + 1 < l){_tmp36(*((struct _tagged_arr*)_check_unknown_subscript(
Cyc_Arg_args,sizeof(struct _tagged_arr),Cyc_Arg_current + 1)));++ Cyc_Arg_current;}
else{Cyc_Arg_stop(initpos,(void*)({struct Cyc_Arg_Missing_struct*_tmp3B=_cycalloc(
sizeof(*_tmp3B));_tmp3B[0]=({struct Cyc_Arg_Missing_struct _tmp3C;_tmp3C.tag=1;
_tmp3C.f1=s;_tmp3C;});_tmp3B;}),speclist,errmsg);}goto _LL14;_LL22: {struct
_tagged_arr arg=*((struct _tagged_arr*)_check_unknown_subscript(Cyc_Arg_args,
sizeof(struct _tagged_arr),Cyc_Arg_current + 1));int n=0;if(({struct Cyc_Std_IntPtr_sa_struct
_tmp3E;_tmp3E.tag=2;_tmp3E.f1=& n;{void*_tmp3D[1]={& _tmp3E};Cyc_Std_sscanf(arg,
_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(_tmp3D,sizeof(void*),1));}})!= 1){
Cyc_Arg_stop(initpos,(void*)({struct Cyc_Arg_Wrong_struct*_tmp3F=_cycalloc(
sizeof(*_tmp3F));_tmp3F[0]=({struct Cyc_Arg_Wrong_struct _tmp40;_tmp40.tag=3;
_tmp40.f1=s;_tmp40.f2=arg;_tmp40.f3=_tag_arr("an integer",sizeof(unsigned char),
11);_tmp40;});_tmp3F;}),speclist,errmsg);_npop_handler(0);return;}_tmp37(n);++
Cyc_Arg_current;goto _LL14;}_LL24: while(Cyc_Arg_current < l - 1){_tmp38(*((struct
_tagged_arr*)_check_unknown_subscript(Cyc_Arg_args,sizeof(struct _tagged_arr),Cyc_Arg_current
+ 1)));++ Cyc_Arg_current;}goto _LL14;_LL14:;};_pop_handler();}else{void*_tmp2E=(
void*)_exn_thrown;void*_tmp42=_tmp2E;struct _tagged_arr _tmp43;_LL2E: if(*((void**)
_tmp42)== Cyc_Arg_Bad){_LL32: _tmp43=((struct Cyc_Arg_Bad_struct*)_tmp42)->f1;goto
_LL2F;}else{goto _LL30;}_LL30: goto _LL31;_LL2F: Cyc_Arg_stop(initpos,(void*)({
struct Cyc_Arg_Message_struct*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44[0]=({
struct Cyc_Arg_Message_struct _tmp45;_tmp45.tag=2;_tmp45.f1=_tmp43;_tmp45;});
_tmp44;}),speclist,errmsg);goto _LL2D;_LL31:(void)_throw(_tmp42);_LL2D:;}}}++ Cyc_Arg_current;}
else{{struct _handler_cons _tmp46;_push_handler(& _tmp46);{int _tmp48=0;if(setjmp(
_tmp46.handler)){_tmp48=1;}if(! _tmp48){anonfun(s);;_pop_handler();}else{void*
_tmp47=(void*)_exn_thrown;void*_tmp4A=_tmp47;struct _tagged_arr _tmp4B;_LL34: if(*((
void**)_tmp4A)== Cyc_Arg_Bad){_LL38: _tmp4B=((struct Cyc_Arg_Bad_struct*)_tmp4A)->f1;
goto _LL35;}else{goto _LL36;}_LL36: goto _LL37;_LL35: Cyc_Arg_stop(initpos,(void*)({
struct Cyc_Arg_Message_struct*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C[0]=({
struct Cyc_Arg_Message_struct _tmp4D;_tmp4D.tag=2;_tmp4D.f1=_tmp4B;_tmp4D;});
_tmp4C;}),speclist,errmsg);goto _LL33;_LL37:(void)_throw(_tmp4A);_LL33:;}}}++ Cyc_Arg_current;}}}}
