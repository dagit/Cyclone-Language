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
struct _tagged_arr Cyc_Core_new_string(unsigned int);extern int Cyc_Core_intcmp(int,
int);extern unsigned char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Failure[12];
struct Cyc_Core_Failure_struct{unsigned char*tag;struct _tagged_arr f1;};extern
unsigned char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Not_found[14];
extern unsigned char Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Cstdio___abstractFILE;struct
Cyc_Std___cycFILE;extern unsigned char Cyc_Std_FileCloseError[19];extern
unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};extern struct Cyc_Std___cycFILE*Cyc_Std_file_open(
struct _tagged_arr fname,struct _tagged_arr mode);extern void Cyc_Std_file_close(
struct Cyc_Std___cycFILE*);struct Cyc_Std_String_pa_struct{int tag;struct
_tagged_arr f1;};struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};struct Cyc_Std_Double_pa_struct{
int tag;double f1;};struct Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_Std_IntPtr_pa_struct{
int tag;unsigned int*f1;};struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};
struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_Std_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct
Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};struct Cyc_Std_DoublePtr_sa_struct{
int tag;double*f1;};struct Cyc_Std_FloatPtr_sa_struct{int tag;float*f1;};extern int
Cyc_Std_sscanf(struct _tagged_arr src,struct _tagged_arr fmt,struct _tagged_arr);
extern unsigned char Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{
unsigned char*tag;struct _tagged_arr f1;};struct Cyc_Lexing_lexbuf{void(*
refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _tagged_arr
lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int
lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _tagged_arr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _tagged_arr lex_base;struct _tagged_arr lex_backtrk;struct _tagged_arr
lex_default;struct _tagged_arr lex_trans;struct _tagged_arr lex_check;};extern struct
Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc_Std___cycFILE*);extern struct
_tagged_arr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);extern int Cyc_Lexing_lexeme_end(
struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
extern unsigned char Cyc_List_List_mismatch[18];extern struct Cyc_List_List*Cyc_List_merge_sort(
int(*cmp)(void*,void*),struct Cyc_List_List*x);extern unsigned char Cyc_List_Nth[8];
extern unsigned int Cyc_Std_strlen(struct _tagged_arr s);extern struct _tagged_arr Cyc_Std_strdup(
struct _tagged_arr src);extern struct _tagged_arr Cyc_Std_substring(struct _tagged_arr,
int ofs,unsigned int n);struct Cyc_Lineno_Pos{struct _tagged_arr logical_file;struct
_tagged_arr line;int line_no;int col;};extern struct Cyc_Lineno_Pos*Cyc_Lineno_pos_of_abs(
struct _tagged_arr,int);extern void Cyc_Lineno_poss_of_abss(struct _tagged_arr
filename,struct Cyc_List_List*places);const int Cyc_Lineno_lex_base[8]=(const int[8]){
0,1,- 2,2,5,- 3,- 1,6};const int Cyc_Lineno_lex_backtrk[8]=(const int[8]){- 1,- 1,- 1,1,-
1,- 1,- 1,0};const int Cyc_Lineno_lex_default[8]=(const int[8]){1,1,0,- 1,4,0,0,- 1};
const int Cyc_Lineno_lex_trans[263]=(const int[263]){0,0,0,0,0,0,0,0,0,0,2,2,2,3,3,
6,6,0,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,5,5,0,0,0,5,0};const int Cyc_Lineno_lex_check[263]=(const int[263]){- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,0,1,3,0,1,4,7,- 1,4,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,0,1,- 1,- 1,- 1,4,- 1};int Cyc_Lineno_lex_engine(int start_state,
struct Cyc_Lexing_lexbuf*lbuf){int state;int base;int backtrk;int c;state=start_state;
if(state >= 0){lbuf->lex_last_pos=(lbuf->lex_start_pos=lbuf->lex_curr_pos);lbuf->lex_last_action=
- 1;}else{state=(- state)- 1;}while(1){base=Cyc_Lineno_lex_base[state];if(base < 0){
return(- base)- 1;}backtrk=Cyc_Lineno_lex_backtrk[state];if(backtrk >= 0){lbuf->lex_last_pos=
lbuf->lex_curr_pos;lbuf->lex_last_action=backtrk;}if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(! lbuf->lex_eof_reached){return(- state)- 1;}else{c=256;}}else{c=(int)*((
unsigned char*)_check_unknown_subscript(lbuf->lex_buffer,sizeof(unsigned char),
lbuf->lex_curr_pos ++));if(c == - 1){c=256;}}if(Cyc_Lineno_lex_check[
_check_known_subscript_notnull(263,base + c)]== state){state=Cyc_Lineno_lex_trans[
_check_known_subscript_notnull(263,base + c)];}else{state=Cyc_Lineno_lex_default[
state];}if(state < 0){lbuf->lex_curr_pos=lbuf->lex_last_pos;if(lbuf->lex_last_action
== - 1){(int)_throw((void*)({struct Cyc_Lexing_Error_struct*_tmp0=_cycalloc(
sizeof(*_tmp0));_tmp0[0]=({struct Cyc_Lexing_Error_struct _tmp1;_tmp1.tag=Cyc_Lexing_Error;
_tmp1.f1=_tag_arr("empty token",sizeof(unsigned char),12);_tmp1;});_tmp0;}));}
else{return lbuf->lex_last_action;}}else{if(c == 256){lbuf->lex_eof_reached=0;}}}}
void*Cyc_Lineno_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=
Cyc_Lineno_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL0: return(void*)
1;case 1: _LL1: return(void*)0;case 2: _LL2: return(void*)2;default: _LL3:(lexbuf->refill_buff)(
lexbuf);return Cyc_Lineno_token_rec(lexbuf,lexstate);}(int)_throw((void*)({struct
Cyc_Lexing_Error_struct*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2[0]=({struct Cyc_Lexing_Error_struct
_tmp3;_tmp3.tag=Cyc_Lexing_Error;_tmp3.f1=_tag_arr("some action didn't return!",
sizeof(unsigned char),27);_tmp3;});_tmp2;}));}void*Cyc_Lineno_token(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_Lineno_token_rec(lexbuf,0);}struct Cyc_Lineno_Pos;struct _tuple0{
struct _tagged_arr f1;int f2;};static struct Cyc_Core_Opt*Cyc_Lineno_parse_linedef(
struct _tagged_arr line){struct _handler_cons _tmp4;_push_handler(& _tmp4);{int _tmp6=
0;if(setjmp(_tmp4.handler)){_tmp6=1;}if(! _tmp6){{int i=0;while(i < _get_arr_size(
line,sizeof(unsigned char))?((unsigned char*)line.curr)[i]< '0'? 1:((
unsigned char*)line.curr)[i]> '9': 0){++ i;}{int j=i;while((j < _get_arr_size(line,
sizeof(unsigned char))?((unsigned char*)line.curr)[j]>= '0': 0)?((unsigned char*)
line.curr)[j]<= '9': 0){++ j;}if(i == _get_arr_size(line,sizeof(unsigned char))){
struct Cyc_Core_Opt*_tmp7=0;_npop_handler(0);return _tmp7;}{int number=0;if(({
struct Cyc_Std_IntPtr_sa_struct _tmp9;_tmp9.tag=2;_tmp9.f1=& number;{void*_tmp8[1]={&
_tmp9};Cyc_Std_sscanf((struct _tagged_arr)Cyc_Std_substring((struct _tagged_arr)
line,i,(unsigned int)(j - i)),_tag_arr("%d",sizeof(unsigned char),3),_tag_arr(
_tmp8,sizeof(void*),1));}})!= 1){struct Cyc_Core_Opt*_tmpA=0;_npop_handler(0);
return _tmpA;}while(j < _get_arr_size(line,sizeof(unsigned char))?((unsigned char*)
line.curr)[j]!= '"': 0){++ j;}{int k=++ j;while(k < _get_arr_size(line,sizeof(
unsigned char))?((unsigned char*)line.curr)[k]!= '"': 0){++ k;}if(j == 
_get_arr_size(line,sizeof(unsigned char))? 1: k == _get_arr_size(line,sizeof(
unsigned char))){struct Cyc_Core_Opt*_tmpB=0;_npop_handler(0);return _tmpB;}{
struct _tagged_arr fname=Cyc_Std_substring((struct _tagged_arr)line,j,(unsigned int)(
k - j));struct Cyc_Core_Opt*_tmpE=({struct Cyc_Core_Opt*_tmpC=_cycalloc(sizeof(*
_tmpC));_tmpC->v=({struct _tuple0*_tmpD=_cycalloc(sizeof(*_tmpD));_tmpD->f1=fname;
_tmpD->f2=number;_tmpD;});_tmpC;});_npop_handler(0);return _tmpE;}}}}};
_pop_handler();}else{void*_tmp5=(void*)_exn_thrown;void*_tmp10=_tmp5;_LL6: goto
_LL7;_LL8: goto _LL9;_LL7: return 0;_LL9:(void)_throw(_tmp10);_LL5:;}}}struct _tuple1{
int f1;struct Cyc_Lineno_Pos*f2;};int Cyc_Lineno_place_cmp(struct _tuple1*place1,
struct _tuple1*place2){return Cyc_Core_intcmp((*place1).f1,(*place2).f1);}void Cyc_Lineno_poss_of_abss(
struct _tagged_arr filename,struct Cyc_List_List*places){places=((struct Cyc_List_List*(*)(
int(*cmp)(struct _tuple1*,struct _tuple1*),struct Cyc_List_List*x))Cyc_List_merge_sort)(
Cyc_Lineno_place_cmp,places);{struct Cyc_Std___cycFILE*f=Cyc_Std_file_open(
filename,_tag_arr("r",sizeof(unsigned char),2));{struct _handler_cons _tmp11;
_push_handler(& _tmp11);{int _tmp13=0;if(setjmp(_tmp11.handler)){_tmp13=1;}if(!
_tmp13){{struct Cyc_Lexing_lexbuf*lbuf=Cyc_Lexing_from_file(f);struct _tagged_arr
_tmp14=filename;int _tmp15=1;struct _tagged_arr this_line;int eol;void*next;while(
places != 0){while(1){next=((void*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_Lineno_token)(
lbuf);eol=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lexing_lexeme_end)(lbuf);
this_line=((struct _tagged_arr(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lexing_lexeme)(
lbuf);if(next == (void*)2? 1: eol > (*((struct _tuple1*)places->hd)).f1){break;}if(
next == (void*)0){++ _tmp15;}else{struct Cyc_Core_Opt*fno=Cyc_Lineno_parse_linedef(
this_line);if(fno == 0){++ _tmp15;}else{_tmp14=(struct _tagged_arr)(*((struct
_tuple0*)fno->v)).f1;_tmp15=(*((struct _tuple0*)fno->v)).f2;}}}while(places != 0?
next == (void*)2? 1: eol > (*((struct _tuple1*)places->hd)).f1: 0){struct Cyc_Lineno_Pos*
_tmp16=(*((struct _tuple1*)places->hd)).f2;_tmp16->logical_file=(struct
_tagged_arr)Cyc_Std_strdup(_tmp14);_tmp16->line=this_line;_tmp16->line_no=_tmp15;
_tmp16->col=(int)(Cyc_Std_strlen((struct _tagged_arr)this_line)- (eol - (*((struct
_tuple1*)places->hd)).f1));if(_tmp16->col < 0){_tmp16->col=0;}places=places->tl;}
++ _tmp15;}};_pop_handler();}else{void*_tmp12=(void*)_exn_thrown;void*_tmp18=
_tmp12;_LLB: goto _LLC;_LLD: goto _LLE;_LLC: Cyc_Std_file_close(f);(int)_throw(_tmp18);
_LLE:(void)_throw(_tmp18);_LLA:;}}}Cyc_Std_file_close(f);return;}}struct Cyc_Lineno_Pos*
Cyc_Lineno_pos_of_abs(struct _tagged_arr filename,int abs){struct Cyc_Lineno_Pos*ans=({
struct Cyc_Lineno_Pos*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->logical_file=
_tag_arr("",sizeof(unsigned char),1);_tmp1B->line=Cyc_Core_new_string(0);_tmp1B->line_no=
0;_tmp1B->col=0;_tmp1B;});Cyc_Lineno_poss_of_abss(filename,({struct Cyc_List_List*
_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19->hd=({struct _tuple1*_tmp1A=_cycalloc(
sizeof(*_tmp1A));_tmp1A->f1=abs;_tmp1A->f2=ans;_tmp1A;});_tmp19->tl=0;_tmp19;}));
return ans;}
