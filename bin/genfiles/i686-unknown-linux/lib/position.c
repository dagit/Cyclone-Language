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
 struct Cyc_Core_Opt{void*v;};struct _tagged_arr Cyc_Core_new_string(unsigned int);
extern unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct{unsigned char*tag;struct _tagged_arr f1;};extern
unsigned char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Not_found[14];
extern unsigned char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};extern unsigned char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);extern unsigned char Cyc_List_Nth[8];struct Cyc_Cstdio___abstractFILE;
struct Cyc_Std___cycFILE;extern struct Cyc_Std___cycFILE*Cyc_Std_stdout;extern
struct Cyc_Std___cycFILE*Cyc_Std_stderr;int Cyc_Std_fflush(struct Cyc_Std___cycFILE*);
extern unsigned char Cyc_Std_FileCloseError[19];extern unsigned char Cyc_Std_FileOpenError[
18];struct Cyc_Std_FileOpenError_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_Int_pa_struct{
int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{int tag;double f1;};struct
Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{int
tag;unsigned int*f1;};int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,struct
_tagged_arr fmt,struct _tagged_arr);struct _tagged_arr Cyc_Std_aprintf(struct
_tagged_arr fmt,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};struct
Cyc_Lineno_Pos*Cyc_Lineno_pos_of_abs(struct _tagged_arr,int);void Cyc_Lineno_poss_of_abss(
struct _tagged_arr filename,struct Cyc_List_List*places);extern unsigned char Cyc_Position_Exit[
9];void Cyc_Position_reset_position(struct _tagged_arr);void Cyc_Position_set_position_file(
struct _tagged_arr);struct _tagged_arr Cyc_Position_get_position_file();struct Cyc_Position_Segment;
struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(int,int);struct Cyc_Position_Segment*
Cyc_Position_segment_join(struct Cyc_Position_Segment*,struct Cyc_Position_Segment*);
struct _tagged_arr Cyc_Position_string_of_loc(int);struct _tagged_arr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_List_List*Cyc_Position_strings_of_segments(
struct Cyc_List_List*);struct Cyc_Position_Error{struct _tagged_arr source;struct Cyc_Position_Segment*
seg;void*kind;struct _tagged_arr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_lex(
struct Cyc_Position_Segment*,struct _tagged_arr);struct Cyc_Position_Error*Cyc_Position_mk_err_parse(
struct Cyc_Position_Segment*,struct _tagged_arr);struct Cyc_Position_Error*Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*,struct _tagged_arr);extern unsigned char Cyc_Position_Nocontext[
14];extern int Cyc_Position_print_context;extern int Cyc_Position_num_errors;extern
int Cyc_Position_max_errors;void Cyc_Position_post_error(struct Cyc_Position_Error*);
int Cyc_Position_error_p();struct _tagged_arr Cyc_Position_get_line_directive(
struct Cyc_Position_Segment*loc);unsigned int Cyc_Std_strlen(struct _tagged_arr s);
int Cyc_Std_strcmp(struct _tagged_arr s1,struct _tagged_arr s2);struct _tagged_arr Cyc_Std_strncpy(
struct _tagged_arr,struct _tagged_arr,unsigned int);struct _tagged_arr Cyc_Std_substring(
struct _tagged_arr,int ofs,unsigned int n);struct Cyc_Typerep_Int_struct{int tag;int
f1;unsigned int f2;};struct Cyc_Typerep_ThinPtr_struct{int tag;unsigned int f1;void*
f2;};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;};struct _tuple0{unsigned int
f1;struct _tagged_arr f2;void*f3;};struct Cyc_Typerep_Struct_struct{int tag;struct
_tagged_arr*f1;unsigned int f2;struct _tagged_arr f3;};struct _tuple1{unsigned int f1;
void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;unsigned int f1;struct _tagged_arr
f2;};struct _tuple2{unsigned int f1;struct _tagged_arr f2;};struct Cyc_Typerep_TUnion_struct{
int tag;struct _tagged_arr f1;struct _tagged_arr f2;struct _tagged_arr f3;};struct Cyc_Typerep_TUnionField_struct{
int tag;struct _tagged_arr f1;struct _tagged_arr f2;unsigned int f3;struct _tagged_arr
f4;};struct _tuple3{struct _tagged_arr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _tagged_arr f1;struct _tagged_arr f2;};struct Cyc_Typerep_Union_struct{
int tag;struct _tagged_arr*f1;int f2;struct _tagged_arr f3;};struct Cyc_Typerep_Enum_struct{
int tag;struct _tagged_arr*f1;int f2;struct _tagged_arr f3;};unsigned int Cyc_Typerep_size_type(
void*rep);extern struct Cyc_Typerep_Struct_struct Cyc_struct_Position_Segment_rep;
unsigned char Cyc_Position_Exit[9]="\000\000\000\000Exit";static unsigned char
_tmp0[1]="";static struct _tagged_arr Cyc_Position_source={_tmp0,_tmp0,_tmp0 + 1};
struct Cyc_Position_Segment{int start;int end;};struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(
int start,int end){return({struct Cyc_Position_Segment*_tmp1=_cycalloc_atomic(
sizeof(*_tmp1));_tmp1->start=start;_tmp1->end=end;_tmp1;});}struct Cyc_Position_Segment*
Cyc_Position_segment_join(struct Cyc_Position_Segment*s1,struct Cyc_Position_Segment*
s2){if(s1 == 0)return s2;if(s2 == 0)return s1;return({struct Cyc_Position_Segment*
_tmp2=_cycalloc_atomic(sizeof(*_tmp2));_tmp2->start=s1->start;_tmp2->end=s2->end;
_tmp2;});}struct _tagged_arr Cyc_Position_string_of_loc(int loc){struct Cyc_Lineno_Pos*
pos=Cyc_Lineno_pos_of_abs(Cyc_Position_source,loc);return({struct Cyc_Std_Int_pa_struct
_tmp6;_tmp6.tag=1;_tmp6.f1=(int)((unsigned int)pos->col);{struct Cyc_Std_Int_pa_struct
_tmp5;_tmp5.tag=1;_tmp5.f1=(int)((unsigned int)pos->line_no);{struct Cyc_Std_String_pa_struct
_tmp4;_tmp4.tag=0;_tmp4.f1=(struct _tagged_arr)pos->logical_file;{void*_tmp3[3]={&
_tmp4,& _tmp5,& _tmp6};Cyc_Std_aprintf(_tag_arr("%s (%d:%d)",sizeof(unsigned char),
11),_tag_arr(_tmp3,sizeof(void*),3));}}}});}static struct _tagged_arr Cyc_Position_string_of_pos_pr(
struct Cyc_Lineno_Pos*pos_s,struct Cyc_Lineno_Pos*pos_e){if(Cyc_Std_strcmp(pos_s->logical_file,
pos_e->logical_file)== 0)return({struct Cyc_Std_Int_pa_struct _tmpC;_tmpC.tag=1;
_tmpC.f1=(int)((unsigned int)pos_e->col);{struct Cyc_Std_Int_pa_struct _tmpB;_tmpB.tag=
1;_tmpB.f1=(int)((unsigned int)pos_e->line_no);{struct Cyc_Std_Int_pa_struct _tmpA;
_tmpA.tag=1;_tmpA.f1=(int)((unsigned int)pos_s->col);{struct Cyc_Std_Int_pa_struct
_tmp9;_tmp9.tag=1;_tmp9.f1=(int)((unsigned int)pos_s->line_no);{struct Cyc_Std_String_pa_struct
_tmp8;_tmp8.tag=0;_tmp8.f1=(struct _tagged_arr)pos_s->logical_file;{void*_tmp7[5]={&
_tmp8,& _tmp9,& _tmpA,& _tmpB,& _tmpC};Cyc_Std_aprintf(_tag_arr("%s(%d:%d-%d:%d)",
sizeof(unsigned char),16),_tag_arr(_tmp7,sizeof(void*),5));}}}}}});else{return({
struct Cyc_Std_Int_pa_struct _tmp13;_tmp13.tag=1;_tmp13.f1=(int)((unsigned int)
pos_e->col);{struct Cyc_Std_Int_pa_struct _tmp12;_tmp12.tag=1;_tmp12.f1=(int)((
unsigned int)pos_e->line_no);{struct Cyc_Std_String_pa_struct _tmp11;_tmp11.tag=0;
_tmp11.f1=(struct _tagged_arr)pos_e->logical_file;{struct Cyc_Std_Int_pa_struct
_tmp10;_tmp10.tag=1;_tmp10.f1=(int)((unsigned int)pos_s->col);{struct Cyc_Std_Int_pa_struct
_tmpF;_tmpF.tag=1;_tmpF.f1=(int)((unsigned int)pos_s->line_no);{struct Cyc_Std_String_pa_struct
_tmpE;_tmpE.tag=0;_tmpE.f1=(struct _tagged_arr)pos_s->logical_file;{void*_tmpD[6]={&
_tmpE,& _tmpF,& _tmp10,& _tmp11,& _tmp12,& _tmp13};Cyc_Std_aprintf(_tag_arr("%s(%d:%d)-%s(%d:%d)",
sizeof(unsigned char),20),_tag_arr(_tmpD,sizeof(void*),6));}}}}}}});}}struct
_tagged_arr Cyc_Position_string_of_segment(struct Cyc_Position_Segment*s){if(s == 0)
return({struct Cyc_Std_String_pa_struct _tmp15;_tmp15.tag=0;_tmp15.f1=(struct
_tagged_arr)Cyc_Position_source;{void*_tmp14[1]={& _tmp15};Cyc_Std_aprintf(
_tag_arr("%s",sizeof(unsigned char),3),_tag_arr(_tmp14,sizeof(void*),1));}});{
struct Cyc_Lineno_Pos*pos_s=Cyc_Lineno_pos_of_abs(Cyc_Position_source,s->start);
struct Cyc_Lineno_Pos*pos_e=Cyc_Lineno_pos_of_abs(Cyc_Position_source,s->end);
return Cyc_Position_string_of_pos_pr(pos_s,pos_e);}}static struct Cyc_Lineno_Pos*
Cyc_Position_new_pos(){return({struct Cyc_Lineno_Pos*_tmp16=_cycalloc(sizeof(*
_tmp16));_tmp16->logical_file=_tag_arr("",sizeof(unsigned char),1);_tmp16->line=
Cyc_Core_new_string(0);_tmp16->line_no=0;_tmp16->col=0;_tmp16;});}struct _tuple4{
int f1;struct Cyc_Lineno_Pos*f2;};struct Cyc_List_List*Cyc_Position_strings_of_segments(
struct Cyc_List_List*segs){struct Cyc_List_List*places=0;{struct Cyc_List_List*
_tmp17=segs;for(0;_tmp17 != 0;_tmp17=_tmp17->tl){if((struct Cyc_Position_Segment*)
_tmp17->hd == 0)continue;places=({struct Cyc_List_List*_tmp18=_cycalloc(sizeof(*
_tmp18));_tmp18->hd=({struct _tuple4*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->f1=((
struct Cyc_Position_Segment*)_check_null((struct Cyc_Position_Segment*)_tmp17->hd))->end;
_tmp1B->f2=Cyc_Position_new_pos();_tmp1B;});_tmp18->tl=({struct Cyc_List_List*
_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19->hd=({struct _tuple4*_tmp1A=_cycalloc(
sizeof(*_tmp1A));_tmp1A->f1=((struct Cyc_Position_Segment*)_check_null((struct Cyc_Position_Segment*)
_tmp17->hd))->start;_tmp1A->f2=Cyc_Position_new_pos();_tmp1A;});_tmp19->tl=
places;_tmp19;});_tmp18;});}}Cyc_Lineno_poss_of_abss(Cyc_Position_source,places);{
struct Cyc_List_List*ans=0;places=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(places);for(0;segs != 0;segs=segs->tl){if((struct Cyc_Position_Segment*)
segs->hd == 0)ans=({struct Cyc_List_List*_tmp1C=_cycalloc(sizeof(*_tmp1C));_tmp1C->hd=({
struct _tagged_arr*_tmp1D=_cycalloc(sizeof(*_tmp1D));_tmp1D[0]=({struct Cyc_Std_String_pa_struct
_tmp1F;_tmp1F.tag=0;_tmp1F.f1=(struct _tagged_arr)Cyc_Position_source;{void*
_tmp1E[1]={& _tmp1F};Cyc_Std_aprintf(_tag_arr("%s(unknown)",sizeof(unsigned char),
12),_tag_arr(_tmp1E,sizeof(void*),1));}});_tmp1D;});_tmp1C->tl=ans;_tmp1C;});
else{ans=({struct Cyc_List_List*_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20->hd=({
struct _tagged_arr*_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21[0]=Cyc_Position_string_of_pos_pr((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(places))->hd)).f2,(*((struct
_tuple4*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(
places))->tl))->hd)).f2);_tmp21;});_tmp20->tl=ans;_tmp20;});places=((struct Cyc_List_List*)
_check_null(places->tl))->tl;}}return ans;}}struct Cyc_Position_Error;struct Cyc_Position_Error*
Cyc_Position_mk_err_lex(struct Cyc_Position_Segment*l,struct _tagged_arr desc){
return({struct Cyc_Position_Error*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->source=
Cyc_Position_source;_tmp22->seg=l;_tmp22->kind=(void*)((void*)0);_tmp22->desc=
desc;_tmp22;});}struct Cyc_Position_Error*Cyc_Position_mk_err_parse(struct Cyc_Position_Segment*
l,struct _tagged_arr desc){return({struct Cyc_Position_Error*_tmp23=_cycalloc(
sizeof(*_tmp23));_tmp23->source=Cyc_Position_source;_tmp23->seg=l;_tmp23->kind=(
void*)((void*)1);_tmp23->desc=desc;_tmp23;});}struct Cyc_Position_Error*Cyc_Position_mk_err_elab(
struct Cyc_Position_Segment*l,struct _tagged_arr desc){return({struct Cyc_Position_Error*
_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24->source=Cyc_Position_source;_tmp24->seg=
l;_tmp24->kind=(void*)((void*)2);_tmp24->desc=desc;_tmp24;});}unsigned char Cyc_Position_Nocontext[
14]="\000\000\000\000Nocontext";static struct _tagged_arr Cyc_Position_trunc(int n,
struct _tagged_arr s){int len=(int)Cyc_Std_strlen((struct _tagged_arr)s);if(len < n)
return s;{int len_one=(n - 3)/ 2;int len_two=(n - 3)- len_one;struct _tagged_arr ans=
Cyc_Core_new_string((unsigned int)(n + 1));Cyc_Std_strncpy(ans,(struct _tagged_arr)
s,(unsigned int)len_one);Cyc_Std_strncpy(_tagged_arr_plus(ans,sizeof(
unsigned char),len_one),_tag_arr("...",sizeof(unsigned char),4),3);Cyc_Std_strncpy(
_tagged_arr_plus(_tagged_arr_plus(ans,sizeof(unsigned char),len_one),sizeof(
unsigned char),3),(struct _tagged_arr)_tagged_arr_plus(_tagged_arr_plus(s,sizeof(
unsigned char),len),sizeof(unsigned char),- len_two),(unsigned int)len_two);
return ans;}}static int Cyc_Position_line_length=76;struct _tuple5{struct _tagged_arr
f1;int f2;int f3;};static struct _tuple5*Cyc_Position_get_context(struct Cyc_Position_Segment*
seg){if(seg == 0)(int)_throw((void*)Cyc_Position_Nocontext);{struct Cyc_Lineno_Pos*
pos_s;struct Cyc_Lineno_Pos*pos_e;{struct _handler_cons _tmp25;_push_handler(&
_tmp25);{int _tmp27=0;if(setjmp(_tmp25.handler))_tmp27=1;if(!_tmp27){pos_s=Cyc_Lineno_pos_of_abs(
Cyc_Position_source,seg->start);pos_e=Cyc_Lineno_pos_of_abs(Cyc_Position_source,
seg->end);;_pop_handler();}else{void*_tmp26=(void*)_exn_thrown;void*_tmp29=
_tmp26;_LL1:;_LL2:(int)_throw((void*)Cyc_Position_Nocontext);_LL3:;_LL4:(void)
_throw(_tmp29);_LL0:;}}}{struct Cyc_Lineno_Pos _tmp2B;struct _tagged_arr _tmp2C;int
_tmp2D;int _tmp2E;struct Cyc_Lineno_Pos*_tmp2A=pos_s;_tmp2B=*_tmp2A;_tmp2C=_tmp2B.line;
_tmp2D=_tmp2B.line_no;_tmp2E=_tmp2B.col;{struct Cyc_Lineno_Pos _tmp30;struct
_tagged_arr _tmp31;int _tmp32;int _tmp33;struct Cyc_Lineno_Pos*_tmp2F=pos_e;_tmp30=*
_tmp2F;_tmp31=_tmp30.line;_tmp32=_tmp30.line_no;_tmp33=_tmp30.col;if(_tmp2D == 
_tmp32){int n=Cyc_Position_line_length / 3;struct _tagged_arr sec_one=Cyc_Position_trunc(
n,Cyc_Std_substring((struct _tagged_arr)_tmp2C,0,(unsigned int)_tmp2E));struct
_tagged_arr sec_two=Cyc_Position_trunc(n,Cyc_Std_substring((struct _tagged_arr)
_tmp2C,_tmp2E,(unsigned int)(_tmp33 - _tmp2E)));struct _tagged_arr sec_three=Cyc_Position_trunc(
n,Cyc_Std_substring((struct _tagged_arr)_tmp2C,_tmp2E,Cyc_Std_strlen((struct
_tagged_arr)_tmp2C)- _tmp33));return({struct _tuple5*_tmp34=_cycalloc(sizeof(*
_tmp34));_tmp34->f1=({struct Cyc_Std_String_pa_struct _tmp38;_tmp38.tag=0;_tmp38.f1=(
struct _tagged_arr)sec_three;{struct Cyc_Std_String_pa_struct _tmp37;_tmp37.tag=0;
_tmp37.f1=(struct _tagged_arr)sec_two;{struct Cyc_Std_String_pa_struct _tmp36;
_tmp36.tag=0;_tmp36.f1=(struct _tagged_arr)sec_one;{void*_tmp35[3]={& _tmp36,&
_tmp37,& _tmp38};Cyc_Std_aprintf(_tag_arr("%s%s%s",sizeof(unsigned char),7),
_tag_arr(_tmp35,sizeof(void*),3));}}}});_tmp34->f2=(int)Cyc_Std_strlen((struct
_tagged_arr)sec_one);_tmp34->f3=(int)(Cyc_Std_strlen((struct _tagged_arr)sec_one)
+ Cyc_Std_strlen((struct _tagged_arr)sec_two));_tmp34;});}else{int n=(Cyc_Position_line_length
- 3)/ 4;struct _tagged_arr sec_one=Cyc_Position_trunc(n,Cyc_Std_substring((struct
_tagged_arr)_tmp2C,0,(unsigned int)_tmp2E));struct _tagged_arr sec_two=Cyc_Position_trunc(
n,Cyc_Std_substring((struct _tagged_arr)_tmp2C,_tmp2E,Cyc_Std_strlen((struct
_tagged_arr)_tmp2C)- _tmp2E));struct _tagged_arr sec_three=Cyc_Position_trunc(n,
Cyc_Std_substring((struct _tagged_arr)_tmp31,0,(unsigned int)_tmp33));struct
_tagged_arr sec_four=Cyc_Position_trunc(n,Cyc_Std_substring((struct _tagged_arr)
_tmp31,_tmp33,Cyc_Std_strlen((struct _tagged_arr)_tmp31)- _tmp33));return({struct
_tuple5*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->f1=({struct Cyc_Std_String_pa_struct
_tmp3E;_tmp3E.tag=0;_tmp3E.f1=(struct _tagged_arr)sec_four;{struct Cyc_Std_String_pa_struct
_tmp3D;_tmp3D.tag=0;_tmp3D.f1=(struct _tagged_arr)sec_three;{struct Cyc_Std_String_pa_struct
_tmp3C;_tmp3C.tag=0;_tmp3C.f1=(struct _tagged_arr)sec_two;{struct Cyc_Std_String_pa_struct
_tmp3B;_tmp3B.tag=0;_tmp3B.f1=(struct _tagged_arr)sec_one;{void*_tmp3A[4]={&
_tmp3B,& _tmp3C,& _tmp3D,& _tmp3E};Cyc_Std_aprintf(_tag_arr("%s%s.\\.%s%s",sizeof(
unsigned char),12),_tag_arr(_tmp3A,sizeof(void*),4));}}}}});_tmp39->f2=(int)Cyc_Std_strlen((
struct _tagged_arr)sec_one);_tmp39->f3=(int)(((Cyc_Std_strlen((struct _tagged_arr)
sec_one)+ Cyc_Std_strlen((struct _tagged_arr)sec_two))+ 3)+ Cyc_Std_strlen((
struct _tagged_arr)sec_three));_tmp39;});}}}}}static int Cyc_Position_error_b=0;int
Cyc_Position_error_p(){return Cyc_Position_error_b;}unsigned char Cyc_Position_Error[
10]="\000\000\000\000Error";struct Cyc_Position_Error_struct{unsigned char*tag;
struct Cyc_Position_Error*f1;};int Cyc_Position_print_context=0;int Cyc_Position_first_error=
1;int Cyc_Position_num_errors=0;int Cyc_Position_max_errors=10;void Cyc_Position_post_error(
struct Cyc_Position_Error*e){Cyc_Position_error_b=1;Cyc_Std_fflush((struct Cyc_Std___cycFILE*)
Cyc_Std_stdout);if(Cyc_Position_first_error){({void*_tmp3F[0]={};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("\n",sizeof(unsigned char),2),_tag_arr(_tmp3F,sizeof(
void*),0));});Cyc_Position_first_error=0;}if(Cyc_Position_num_errors <= Cyc_Position_max_errors){({
struct Cyc_Std_String_pa_struct _tmp42;_tmp42.tag=0;_tmp42.f1=(struct _tagged_arr)e->desc;{
struct Cyc_Std_String_pa_struct _tmp41;_tmp41.tag=0;_tmp41.f1=(struct _tagged_arr)
Cyc_Position_string_of_segment(e->seg);{void*_tmp40[2]={& _tmp41,& _tmp42};Cyc_Std_fprintf(
Cyc_Std_stderr,_tag_arr("%s: %s\n",sizeof(unsigned char),8),_tag_arr(_tmp40,
sizeof(void*),2));}}});if(Cyc_Position_print_context){struct _handler_cons _tmp43;
_push_handler(& _tmp43);{int _tmp45=0;if(setjmp(_tmp43.handler))_tmp45=1;if(!
_tmp45){{struct _tuple5*x=Cyc_Position_get_context(e->seg);struct _tagged_arr
marker_str=({unsigned int _tmp49=(unsigned int)((*x).f3 + 1);unsigned char*_tmp4A=(
unsigned char*)_cycalloc_atomic(_check_times(sizeof(unsigned char),_tmp49));
struct _tagged_arr _tmp4C=_tag_arr(_tmp4A,sizeof(unsigned char),(unsigned int)((*x).f3
+ 1));{unsigned int _tmp4B=_tmp49;unsigned int i;for(i=0;i < _tmp4B;i ++){_tmp4A[i]='\000';}}
_tmp4C;});int i=- 1;while(++ i < (*x).f2){((unsigned char*)marker_str.curr)[i]=' ';}
while(++ i < (*x).f3){((unsigned char*)marker_str.curr)[i]='^';}({struct Cyc_Std_String_pa_struct
_tmp48;_tmp48.tag=0;_tmp48.f1=(struct _tagged_arr)marker_str;{struct Cyc_Std_String_pa_struct
_tmp47;_tmp47.tag=0;_tmp47.f1=(struct _tagged_arr)(*x).f1;{void*_tmp46[2]={&
_tmp47,& _tmp48};Cyc_Std_fprintf(Cyc_Std_stderr,_tag_arr("  %s\n  %s\n",sizeof(
unsigned char),11),_tag_arr(_tmp46,sizeof(void*),2));}}});};_pop_handler();}
else{void*_tmp44=(void*)_exn_thrown;void*_tmp4E=_tmp44;_LL6: if(_tmp4E != Cyc_Position_Nocontext)
goto _LL8;_LL7: goto _LL5;_LL8:;_LL9:(void)_throw(_tmp4E);_LL5:;}}}}if(Cyc_Position_num_errors
== Cyc_Position_max_errors)({void*_tmp4F[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,
_tag_arr("Too many error messages!\n",sizeof(unsigned char),26),_tag_arr(_tmp4F,
sizeof(void*),0));});Cyc_Std_fflush((struct Cyc_Std___cycFILE*)Cyc_Std_stderr);
Cyc_Position_num_errors ++;}void Cyc_Position_reset_position(struct _tagged_arr s){
Cyc_Position_source=s;Cyc_Position_error_b=0;}void Cyc_Position_set_position_file(
struct _tagged_arr s){Cyc_Position_source=s;Cyc_Position_error_b=0;}struct
_tagged_arr Cyc_Position_get_position_file(){return Cyc_Position_source;}struct
_tagged_arr Cyc_Position_get_line_directive(struct Cyc_Position_Segment*s){struct
Cyc_Lineno_Pos*pos_s=Cyc_Lineno_pos_of_abs(Cyc_Position_source,((struct Cyc_Position_Segment*)
_check_null(s))->start);if((struct Cyc_Lineno_Pos*)pos_s != 0)return(struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp52;_tmp52.tag=0;_tmp52.f1=(
struct _tagged_arr)pos_s->logical_file;{struct Cyc_Std_Int_pa_struct _tmp51;_tmp51.tag=
1;_tmp51.f1=(int)((unsigned int)pos_s->line_no);{void*_tmp50[2]={& _tmp51,& _tmp52};
Cyc_Std_aprintf(_tag_arr("\n#line %d \"%s\"\n",sizeof(unsigned char),16),
_tag_arr(_tmp50,sizeof(void*),2));}}});else{return(struct _tagged_arr)_tag_arr(0,
0,0);}}extern struct Cyc_Typerep_Struct_struct Cyc_struct_Position_Segment_rep;
static struct Cyc_Typerep_Int_struct Cyc__genrep_0={0,1,32};static unsigned char
_tmp54[8]="Segment";static struct _tagged_arr Cyc__genname_4={_tmp54,_tmp54,_tmp54 + 
8};static unsigned char _tmp55[6]="start";static struct _tuple0 Cyc__gentuple_1={
offsetof(struct Cyc_Position_Segment,start),{_tmp55,_tmp55,_tmp55 + 6},(void*)& Cyc__genrep_0};
static unsigned char _tmp56[4]="end";static struct _tuple0 Cyc__gentuple_2={offsetof(
struct Cyc_Position_Segment,end),{_tmp56,_tmp56,_tmp56 + 4},(void*)& Cyc__genrep_0};
static struct _tuple0*Cyc__genarr_3[2]={& Cyc__gentuple_1,& Cyc__gentuple_2};struct
Cyc_Typerep_Struct_struct Cyc_struct_Position_Segment_rep={3,(struct _tagged_arr*)&
Cyc__genname_4,sizeof(struct Cyc_Position_Segment),{(void*)((struct _tuple0**)Cyc__genarr_3),(
void*)((struct _tuple0**)Cyc__genarr_3),(void*)((struct _tuple0**)Cyc__genarr_3 + 2)}};
void*Cyc_segment_rep=(void*)& Cyc_struct_Position_Segment_rep;
