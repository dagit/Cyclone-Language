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
  if ((_czs_x) == 0) _throw_null(); \
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
extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _tagged_arr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _tagged_arr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[8];struct Cyc_Std___cycFILE;
extern struct Cyc_Std___cycFILE*Cyc_Std_stdout;extern struct Cyc_Std___cycFILE*Cyc_Std_stderr;
struct Cyc_Std_Cstdio___abstractFILE;struct Cyc_Std_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{
int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_Buffer_pa_struct{
int tag;struct _tagged_arr f1;};struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*
f1;};struct _tagged_arr Cyc_Std_aprintf(struct _tagged_arr,struct _tagged_arr);int
Cyc_Std_fflush(struct Cyc_Std___cycFILE*);int Cyc_Std_fprintf(struct Cyc_Std___cycFILE*,
struct _tagged_arr,struct _tagged_arr);struct Cyc_Std_ShortPtr_sa_struct{int tag;
short*f1;};struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct
Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;}
;struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_Std_CharPtr_sa_struct{int tag;struct _tagged_arr f1;};
extern char Cyc_Std_FileCloseError[19];extern char Cyc_Std_FileOpenError[18];struct
Cyc_Std_FileOpenError_struct{char*tag;struct _tagged_arr f1;};struct Cyc_Lineno_Pos{
struct _tagged_arr logical_file;struct _tagged_arr line;int line_no;int col;};struct
Cyc_Lineno_Pos*Cyc_Lineno_pos_of_abs(struct _tagged_arr,int);void Cyc_Lineno_poss_of_abss(
struct _tagged_arr filename,struct Cyc_List_List*places);extern char Cyc_Position_Exit[
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
struct Cyc_Position_Segment*,struct _tagged_arr);extern char Cyc_Position_Nocontext[
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
char Cyc_Position_Exit[9]="\000\000\000\000Exit\000";static char _tmp0[1]="";static
struct _tagged_arr Cyc_Position_source={_tmp0,_tmp0,_tmp0 + 1};struct Cyc_Position_Segment{
int start;int end;};struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(int
start,int end){return({struct Cyc_Position_Segment*_tmp1=_cycalloc_atomic(sizeof(*
_tmp1));_tmp1->start=start;_tmp1->end=end;_tmp1;});}struct Cyc_Position_Segment*
Cyc_Position_segment_join(struct Cyc_Position_Segment*s1,struct Cyc_Position_Segment*
s2){if(s1 == 0)return s2;if(s2 == 0)return s1;return({struct Cyc_Position_Segment*
_tmp2=_cycalloc_atomic(sizeof(*_tmp2));_tmp2->start=s1->start;_tmp2->end=s2->end;
_tmp2;});}struct _tagged_arr Cyc_Position_string_of_loc(int loc){struct Cyc_Lineno_Pos*
pos=Cyc_Lineno_pos_of_abs(Cyc_Position_source,loc);return({struct Cyc_Std_Int_pa_struct
_tmp7;_tmp7.tag=1;_tmp7.f1=(int)((unsigned int)pos->col);{struct Cyc_Std_Int_pa_struct
_tmp6;_tmp6.tag=1;_tmp6.f1=(int)((unsigned int)pos->line_no);{struct Cyc_Std_String_pa_struct
_tmp5;_tmp5.tag=0;_tmp5.f1=(struct _tagged_arr)pos->logical_file;{void*_tmp3[3]={&
_tmp5,& _tmp6,& _tmp7};Cyc_Std_aprintf(({const char*_tmp4="%s (%d:%d)";_tag_arr(
_tmp4,sizeof(char),_get_zero_arr_size(_tmp4,11));}),_tag_arr(_tmp3,sizeof(void*),
3));}}}});}static struct _tagged_arr Cyc_Position_string_of_pos_pr(struct Cyc_Lineno_Pos*
pos_s,struct Cyc_Lineno_Pos*pos_e){if(Cyc_Std_strcmp((struct _tagged_arr)pos_s->logical_file,(
struct _tagged_arr)pos_e->logical_file)== 0)return({struct Cyc_Std_Int_pa_struct
_tmpE;_tmpE.tag=1;_tmpE.f1=(int)((unsigned int)pos_e->col);{struct Cyc_Std_Int_pa_struct
_tmpD;_tmpD.tag=1;_tmpD.f1=(int)((unsigned int)pos_e->line_no);{struct Cyc_Std_Int_pa_struct
_tmpC;_tmpC.tag=1;_tmpC.f1=(int)((unsigned int)pos_s->col);{struct Cyc_Std_Int_pa_struct
_tmpB;_tmpB.tag=1;_tmpB.f1=(int)((unsigned int)pos_s->line_no);{struct Cyc_Std_String_pa_struct
_tmpA;_tmpA.tag=0;_tmpA.f1=(struct _tagged_arr)pos_s->logical_file;{void*_tmp8[5]={&
_tmpA,& _tmpB,& _tmpC,& _tmpD,& _tmpE};Cyc_Std_aprintf(({const char*_tmp9="%s(%d:%d-%d:%d)";
_tag_arr(_tmp9,sizeof(char),_get_zero_arr_size(_tmp9,16));}),_tag_arr(_tmp8,
sizeof(void*),5));}}}}}});else{return({struct Cyc_Std_Int_pa_struct _tmp16;_tmp16.tag=
1;_tmp16.f1=(int)((unsigned int)pos_e->col);{struct Cyc_Std_Int_pa_struct _tmp15;
_tmp15.tag=1;_tmp15.f1=(int)((unsigned int)pos_e->line_no);{struct Cyc_Std_String_pa_struct
_tmp14;_tmp14.tag=0;_tmp14.f1=(struct _tagged_arr)pos_e->logical_file;{struct Cyc_Std_Int_pa_struct
_tmp13;_tmp13.tag=1;_tmp13.f1=(int)((unsigned int)pos_s->col);{struct Cyc_Std_Int_pa_struct
_tmp12;_tmp12.tag=1;_tmp12.f1=(int)((unsigned int)pos_s->line_no);{struct Cyc_Std_String_pa_struct
_tmp11;_tmp11.tag=0;_tmp11.f1=(struct _tagged_arr)pos_s->logical_file;{void*_tmpF[
6]={& _tmp11,& _tmp12,& _tmp13,& _tmp14,& _tmp15,& _tmp16};Cyc_Std_aprintf(({const char*
_tmp10="%s(%d:%d)-%s(%d:%d)";_tag_arr(_tmp10,sizeof(char),_get_zero_arr_size(
_tmp10,20));}),_tag_arr(_tmpF,sizeof(void*),6));}}}}}}});}}struct _tagged_arr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*s){if(s == 0)return({struct Cyc_Std_String_pa_struct
_tmp19;_tmp19.tag=0;_tmp19.f1=(struct _tagged_arr)Cyc_Position_source;{void*
_tmp17[1]={& _tmp19};Cyc_Std_aprintf(({const char*_tmp18="%s";_tag_arr(_tmp18,
sizeof(char),_get_zero_arr_size(_tmp18,3));}),_tag_arr(_tmp17,sizeof(void*),1));}});{
struct Cyc_Lineno_Pos*pos_s=Cyc_Lineno_pos_of_abs(Cyc_Position_source,s->start);
struct Cyc_Lineno_Pos*pos_e=Cyc_Lineno_pos_of_abs(Cyc_Position_source,s->end);
return Cyc_Position_string_of_pos_pr(pos_s,pos_e);}}static struct Cyc_Lineno_Pos*
Cyc_Position_new_pos(){return({struct Cyc_Lineno_Pos*_tmp1A=_cycalloc(sizeof(*
_tmp1A));_tmp1A->logical_file=({const char*_tmp1B="";_tag_arr(_tmp1B,sizeof(char),
_get_zero_arr_size(_tmp1B,1));});_tmp1A->line=Cyc_Core_new_string(0);_tmp1A->line_no=
0;_tmp1A->col=0;_tmp1A;});}struct _tuple4{int f1;struct Cyc_Lineno_Pos*f2;};struct
Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*segs){struct
Cyc_List_List*places=0;{struct Cyc_List_List*_tmp1C=segs;for(0;_tmp1C != 0;_tmp1C=
_tmp1C->tl){if((struct Cyc_Position_Segment*)_tmp1C->hd == 0)continue;places=({
struct Cyc_List_List*_tmp1D=_cycalloc(sizeof(*_tmp1D));_tmp1D->hd=({struct _tuple4*
_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20->f1=((struct Cyc_Position_Segment*)
_check_null((struct Cyc_Position_Segment*)_tmp1C->hd))->end;_tmp20->f2=Cyc_Position_new_pos();
_tmp20;});_tmp1D->tl=({struct Cyc_List_List*_tmp1E=_cycalloc(sizeof(*_tmp1E));
_tmp1E->hd=({struct _tuple4*_tmp1F=_cycalloc(sizeof(*_tmp1F));_tmp1F->f1=((struct
Cyc_Position_Segment*)_check_null((struct Cyc_Position_Segment*)_tmp1C->hd))->start;
_tmp1F->f2=Cyc_Position_new_pos();_tmp1F;});_tmp1E->tl=places;_tmp1E;});_tmp1D;});}}
Cyc_Lineno_poss_of_abss(Cyc_Position_source,places);{struct Cyc_List_List*ans=0;
places=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(places);
for(0;segs != 0;segs=segs->tl){if((struct Cyc_Position_Segment*)segs->hd == 0)ans=({
struct Cyc_List_List*_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21->hd=({struct
_tagged_arr*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22[0]=({struct Cyc_Std_String_pa_struct
_tmp25;_tmp25.tag=0;_tmp25.f1=(struct _tagged_arr)Cyc_Position_source;{void*
_tmp23[1]={& _tmp25};Cyc_Std_aprintf(({const char*_tmp24="%s(unknown)";_tag_arr(
_tmp24,sizeof(char),_get_zero_arr_size(_tmp24,12));}),_tag_arr(_tmp23,sizeof(
void*),1));}});_tmp22;});_tmp21->tl=ans;_tmp21;});else{ans=({struct Cyc_List_List*
_tmp26=_cycalloc(sizeof(*_tmp26));_tmp26->hd=({struct _tagged_arr*_tmp27=
_cycalloc(sizeof(*_tmp27));_tmp27[0]=Cyc_Position_string_of_pos_pr((*((struct
_tuple4*)((struct _tuple4*)((struct Cyc_List_List*)_check_null(places))->hd))).f2,(*((
struct _tuple4*)((struct _tuple4*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(places))->tl))->hd))).f2);_tmp27;});_tmp26->tl=ans;_tmp26;});places=((
struct Cyc_List_List*)_check_null(places->tl))->tl;}}return ans;}}struct Cyc_Position_Error;
struct Cyc_Position_Error*Cyc_Position_mk_err_lex(struct Cyc_Position_Segment*l,
struct _tagged_arr desc){return({struct Cyc_Position_Error*_tmp28=_cycalloc(sizeof(*
_tmp28));_tmp28->source=Cyc_Position_source;_tmp28->seg=l;_tmp28->kind=(void*)((
void*)0);_tmp28->desc=desc;_tmp28;});}struct Cyc_Position_Error*Cyc_Position_mk_err_parse(
struct Cyc_Position_Segment*l,struct _tagged_arr desc){return({struct Cyc_Position_Error*
_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->source=Cyc_Position_source;_tmp29->seg=
l;_tmp29->kind=(void*)((void*)1);_tmp29->desc=desc;_tmp29;});}struct Cyc_Position_Error*
Cyc_Position_mk_err_elab(struct Cyc_Position_Segment*l,struct _tagged_arr desc){
return({struct Cyc_Position_Error*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A->source=
Cyc_Position_source;_tmp2A->seg=l;_tmp2A->kind=(void*)((void*)2);_tmp2A->desc=
desc;_tmp2A;});}char Cyc_Position_Nocontext[14]="\000\000\000\000Nocontext\000";
static struct _tagged_arr Cyc_Position_trunc(int n,struct _tagged_arr s){int len=(int)
Cyc_Std_strlen((struct _tagged_arr)s);if(len < n)return s;{int len_one=(n - 3)/ 2;int
len_two=(n - 3)- len_one;struct _tagged_arr ans=Cyc_Core_new_string((unsigned int)(
n + 1));Cyc_Std_strncpy(ans,(struct _tagged_arr)s,(unsigned int)len_one);Cyc_Std_strncpy(
_tagged_arr_plus(ans,sizeof(char),len_one),({const char*_tmp2B="...";_tag_arr(
_tmp2B,sizeof(char),_get_zero_arr_size(_tmp2B,4));}),3);Cyc_Std_strncpy(
_tagged_arr_plus(_tagged_arr_plus(ans,sizeof(char),len_one),sizeof(char),3),(
struct _tagged_arr)_tagged_arr_plus(_tagged_arr_plus(s,sizeof(char),len),sizeof(
char),- len_two),(unsigned int)len_two);return ans;}}static int Cyc_Position_line_length=
76;struct _tuple5{struct _tagged_arr f1;int f2;int f3;};static struct _tuple5*Cyc_Position_get_context(
struct Cyc_Position_Segment*seg){if(seg == 0)(int)_throw((void*)Cyc_Position_Nocontext);{
struct Cyc_Lineno_Pos*pos_s;struct Cyc_Lineno_Pos*pos_e;{struct _handler_cons _tmp2C;
_push_handler(& _tmp2C);{int _tmp2E=0;if(setjmp(_tmp2C.handler))_tmp2E=1;if(!
_tmp2E){pos_s=Cyc_Lineno_pos_of_abs(Cyc_Position_source,seg->start);pos_e=Cyc_Lineno_pos_of_abs(
Cyc_Position_source,seg->end);;_pop_handler();}else{void*_tmp2D=(void*)
_exn_thrown;void*_tmp30=_tmp2D;_LL1:;_LL2:(int)_throw((void*)Cyc_Position_Nocontext);
_LL3:;_LL4:(void)_throw(_tmp30);_LL0:;}}}{struct Cyc_Lineno_Pos _tmp32;struct
_tagged_arr _tmp33;int _tmp34;int _tmp35;struct Cyc_Lineno_Pos*_tmp31=pos_s;_tmp32=*
_tmp31;_tmp33=_tmp32.line;_tmp34=_tmp32.line_no;_tmp35=_tmp32.col;{struct Cyc_Lineno_Pos
_tmp37;struct _tagged_arr _tmp38;int _tmp39;int _tmp3A;struct Cyc_Lineno_Pos*_tmp36=
pos_e;_tmp37=*_tmp36;_tmp38=_tmp37.line;_tmp39=_tmp37.line_no;_tmp3A=_tmp37.col;
if(_tmp34 == _tmp39){int n=Cyc_Position_line_length / 3;struct _tagged_arr sec_one=
Cyc_Position_trunc(n,Cyc_Std_substring((struct _tagged_arr)_tmp33,0,(unsigned int)
_tmp35));struct _tagged_arr sec_two=Cyc_Position_trunc(n,Cyc_Std_substring((struct
_tagged_arr)_tmp33,_tmp35,(unsigned int)(_tmp3A - _tmp35)));struct _tagged_arr
sec_three=Cyc_Position_trunc(n,Cyc_Std_substring((struct _tagged_arr)_tmp33,
_tmp35,Cyc_Std_strlen((struct _tagged_arr)_tmp33)- _tmp3A));return({struct _tuple5*
_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B->f1=({struct Cyc_Std_String_pa_struct
_tmp40;_tmp40.tag=0;_tmp40.f1=(struct _tagged_arr)sec_three;{struct Cyc_Std_String_pa_struct
_tmp3F;_tmp3F.tag=0;_tmp3F.f1=(struct _tagged_arr)sec_two;{struct Cyc_Std_String_pa_struct
_tmp3E;_tmp3E.tag=0;_tmp3E.f1=(struct _tagged_arr)sec_one;{void*_tmp3C[3]={&
_tmp3E,& _tmp3F,& _tmp40};Cyc_Std_aprintf(({const char*_tmp3D="%s%s%s";_tag_arr(
_tmp3D,sizeof(char),_get_zero_arr_size(_tmp3D,7));}),_tag_arr(_tmp3C,sizeof(void*),
3));}}}});_tmp3B->f2=(int)Cyc_Std_strlen((struct _tagged_arr)sec_one);_tmp3B->f3=(
int)(Cyc_Std_strlen((struct _tagged_arr)sec_one)+ Cyc_Std_strlen((struct
_tagged_arr)sec_two));_tmp3B;});}else{int n=(Cyc_Position_line_length - 3)/ 4;
struct _tagged_arr sec_one=Cyc_Position_trunc(n,Cyc_Std_substring((struct
_tagged_arr)_tmp33,0,(unsigned int)_tmp35));struct _tagged_arr sec_two=Cyc_Position_trunc(
n,Cyc_Std_substring((struct _tagged_arr)_tmp33,_tmp35,Cyc_Std_strlen((struct
_tagged_arr)_tmp33)- _tmp35));struct _tagged_arr sec_three=Cyc_Position_trunc(n,
Cyc_Std_substring((struct _tagged_arr)_tmp38,0,(unsigned int)_tmp3A));struct
_tagged_arr sec_four=Cyc_Position_trunc(n,Cyc_Std_substring((struct _tagged_arr)
_tmp38,_tmp3A,Cyc_Std_strlen((struct _tagged_arr)_tmp38)- _tmp3A));return({struct
_tuple5*_tmp41=_cycalloc(sizeof(*_tmp41));_tmp41->f1=({struct Cyc_Std_String_pa_struct
_tmp47;_tmp47.tag=0;_tmp47.f1=(struct _tagged_arr)sec_four;{struct Cyc_Std_String_pa_struct
_tmp46;_tmp46.tag=0;_tmp46.f1=(struct _tagged_arr)sec_three;{struct Cyc_Std_String_pa_struct
_tmp45;_tmp45.tag=0;_tmp45.f1=(struct _tagged_arr)sec_two;{struct Cyc_Std_String_pa_struct
_tmp44;_tmp44.tag=0;_tmp44.f1=(struct _tagged_arr)sec_one;{void*_tmp42[4]={&
_tmp44,& _tmp45,& _tmp46,& _tmp47};Cyc_Std_aprintf(({const char*_tmp43="%s%s.\\.%s%s";
_tag_arr(_tmp43,sizeof(char),_get_zero_arr_size(_tmp43,12));}),_tag_arr(_tmp42,
sizeof(void*),4));}}}}});_tmp41->f2=(int)Cyc_Std_strlen((struct _tagged_arr)
sec_one);_tmp41->f3=(int)(((Cyc_Std_strlen((struct _tagged_arr)sec_one)+ Cyc_Std_strlen((
struct _tagged_arr)sec_two))+ 3)+ Cyc_Std_strlen((struct _tagged_arr)sec_three));
_tmp41;});}}}}}static int Cyc_Position_error_b=0;int Cyc_Position_error_p(){return
Cyc_Position_error_b;}char Cyc_Position_Error[10]="\000\000\000\000Error\000";
struct Cyc_Position_Error_struct{char*tag;struct Cyc_Position_Error*f1;};int Cyc_Position_print_context=
0;int Cyc_Position_first_error=1;int Cyc_Position_num_errors=0;int Cyc_Position_max_errors=
10;void Cyc_Position_post_error(struct Cyc_Position_Error*e){Cyc_Position_error_b=
1;Cyc_Std_fflush((struct Cyc_Std___cycFILE*)Cyc_Std_stdout);if(Cyc_Position_first_error){({
void*_tmp48[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp49="\n";
_tag_arr(_tmp49,sizeof(char),_get_zero_arr_size(_tmp49,2));}),_tag_arr(_tmp48,
sizeof(void*),0));});Cyc_Position_first_error=0;}if(Cyc_Position_num_errors <= 
Cyc_Position_max_errors){({struct Cyc_Std_String_pa_struct _tmp4D;_tmp4D.tag=0;
_tmp4D.f1=(struct _tagged_arr)e->desc;{struct Cyc_Std_String_pa_struct _tmp4C;
_tmp4C.tag=0;_tmp4C.f1=(struct _tagged_arr)Cyc_Position_string_of_segment(e->seg);{
void*_tmp4A[2]={& _tmp4C,& _tmp4D};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*
_tmp4B="%s: %s\n";_tag_arr(_tmp4B,sizeof(char),_get_zero_arr_size(_tmp4B,8));}),
_tag_arr(_tmp4A,sizeof(void*),2));}}});if(Cyc_Position_print_context){struct
_handler_cons _tmp4E;_push_handler(& _tmp4E);{int _tmp50=0;if(setjmp(_tmp4E.handler))
_tmp50=1;if(!_tmp50){{struct _tuple5*x=Cyc_Position_get_context(e->seg);struct
_tagged_arr marker_str=({unsigned int _tmp5B=(unsigned int)((*((struct _tuple5*)x)).f3
+ 1);char*_tmp5C=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp5B + 1));
struct _tagged_arr _tmp5E=_tag_arr(_tmp5C,sizeof(char),_tmp5B + 1);{unsigned int
_tmp5D=_tmp5B;unsigned int i;for(i=0;i < _tmp5D;i ++){_tmp5C[i]='\000';}_tmp5C[
_tmp5D]=(char)0;}_tmp5E;});int i=- 1;while(++ i < (*((struct _tuple5*)x)).f2){({
struct _tagged_arr _tmp51=_tagged_arr_plus(marker_str,sizeof(char),i);char _tmp52=*((
char*)_check_unknown_subscript(_tmp51,sizeof(char),0));char _tmp53=' ';if(
_get_arr_size(_tmp51,sizeof(char))== 1?_tmp52 == '\000'?_tmp53 != '\000': 0: 0)
_throw_arraybounds();*((char*)_tmp51.curr)=_tmp53;});}while(++ i < (*((struct
_tuple5*)x)).f3){({struct _tagged_arr _tmp54=_tagged_arr_plus(marker_str,sizeof(
char),i);char _tmp55=*((char*)_check_unknown_subscript(_tmp54,sizeof(char),0));
char _tmp56='^';if(_get_arr_size(_tmp54,sizeof(char))== 1?_tmp55 == '\000'?_tmp56
!= '\000': 0: 0)_throw_arraybounds();*((char*)_tmp54.curr)=_tmp56;});}({struct Cyc_Std_String_pa_struct
_tmp5A;_tmp5A.tag=0;_tmp5A.f1=(struct _tagged_arr)marker_str;{struct Cyc_Std_String_pa_struct
_tmp59;_tmp59.tag=0;_tmp59.f1=(struct _tagged_arr)(*((struct _tuple5*)x)).f1;{void*
_tmp57[2]={& _tmp59,& _tmp5A};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp58="  %s\n  %s\n";
_tag_arr(_tmp58,sizeof(char),_get_zero_arr_size(_tmp58,11));}),_tag_arr(_tmp57,
sizeof(void*),2));}}});};_pop_handler();}else{void*_tmp4F=(void*)_exn_thrown;
void*_tmp60=_tmp4F;_LL6: if(_tmp60 != Cyc_Position_Nocontext)goto _LL8;_LL7: goto
_LL5;_LL8:;_LL9:(void)_throw(_tmp60);_LL5:;}}}}if(Cyc_Position_num_errors == Cyc_Position_max_errors)({
void*_tmp61[0]={};Cyc_Std_fprintf(Cyc_Std_stderr,({const char*_tmp62="Too many error messages!\n";
_tag_arr(_tmp62,sizeof(char),_get_zero_arr_size(_tmp62,26));}),_tag_arr(_tmp61,
sizeof(void*),0));});Cyc_Std_fflush((struct Cyc_Std___cycFILE*)Cyc_Std_stderr);
Cyc_Position_num_errors ++;}void Cyc_Position_reset_position(struct _tagged_arr s){
Cyc_Position_source=s;Cyc_Position_error_b=0;}void Cyc_Position_set_position_file(
struct _tagged_arr s){Cyc_Position_source=s;Cyc_Position_error_b=0;}struct
_tagged_arr Cyc_Position_get_position_file(){return Cyc_Position_source;}struct
_tagged_arr Cyc_Position_get_line_directive(struct Cyc_Position_Segment*s){struct
Cyc_Lineno_Pos*pos_s=Cyc_Lineno_pos_of_abs(Cyc_Position_source,((struct Cyc_Position_Segment*)
_check_null(s))->start);if((struct Cyc_Lineno_Pos*)pos_s != 0)return(struct
_tagged_arr)({struct Cyc_Std_String_pa_struct _tmp66;_tmp66.tag=0;_tmp66.f1=(
struct _tagged_arr)pos_s->logical_file;{struct Cyc_Std_Int_pa_struct _tmp65;_tmp65.tag=
1;_tmp65.f1=(int)((unsigned int)pos_s->line_no);{void*_tmp63[2]={& _tmp65,& _tmp66};
Cyc_Std_aprintf(({const char*_tmp64="\n#line %d \"%s\"\n";_tag_arr(_tmp64,sizeof(
char),_get_zero_arr_size(_tmp64,16));}),_tag_arr(_tmp63,sizeof(void*),2));}}});
else{return(struct _tagged_arr)_tag_arr(0,0,0);}}extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Position_Segment_rep;static struct Cyc_Typerep_Int_struct Cyc__genrep_0={
0,1,32};static char _tmp68[8]="Segment";static struct _tagged_arr Cyc__genname_4={
_tmp68,_tmp68,_tmp68 + 8};static char _tmp69[6]="start";static struct _tuple0 Cyc__gentuple_1={
offsetof(struct Cyc_Position_Segment,start),{_tmp69,_tmp69,_tmp69 + 6},(void*)& Cyc__genrep_0};
static char _tmp6A[4]="end";static struct _tuple0 Cyc__gentuple_2={offsetof(struct Cyc_Position_Segment,end),{
_tmp6A,_tmp6A,_tmp6A + 4},(void*)& Cyc__genrep_0};static struct _tuple0*Cyc__genarr_3[
2]={& Cyc__gentuple_1,& Cyc__gentuple_2};struct Cyc_Typerep_Struct_struct Cyc_struct_Position_Segment_rep={
3,(struct _tagged_arr*)& Cyc__genname_4,sizeof(struct Cyc_Position_Segment),{(void*)((
struct _tuple0**)Cyc__genarr_3),(void*)((struct _tuple0**)Cyc__genarr_3),(void*)((
struct _tuple0**)Cyc__genarr_3 + 2)}};void*Cyc_segment_rep=(void*)& Cyc_struct_Position_Segment_rep;
