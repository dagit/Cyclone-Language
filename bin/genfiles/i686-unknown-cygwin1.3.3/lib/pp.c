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
static inline 
void * _check_null(void * ptr) {
#ifndef NO_CYC_NULL_CHECKS
  if(!ptr)
    _throw_null();
#endif
  return ptr;
}
static inline 
char * _check_known_subscript_null(void * ptr, unsigned bound, 
				   unsigned elt_sz, unsigned index) {
#ifndef NO_CYC_NULL_CHECKS
  if(!ptr)
    _throw_null();
#endif
#ifndef NO_CYC_BOUNDS_CHECKS
  if(index >= bound)
    _throw_arraybounds();
#endif
  return ((char *)ptr) + elt_sz*index;
}
static inline 
unsigned _check_known_subscript_notnull(unsigned bound, unsigned index) {
#ifndef NO_CYC_BOUNDS_CHECKS
  if(index >= bound)
    _throw_arraybounds();
#endif
  return index;
}
static inline 
char * _check_unknown_subscript(struct _tagged_arr arr,
				unsigned elt_sz, unsigned index) {
  // caller casts first argument and result
  // multiplication looks inefficient, but C compiler has to insert it otherwise
  // by inlining, it should be able to avoid actual multiplication
  unsigned char * ans = arr.curr + elt_sz * index;
  // might be faster not to distinguish these two cases. definitely would be
  // smaller.
#ifndef NO_CYC_NULL_CHECKS
  if(!arr.base) 
    _throw_null();
#endif
#ifndef NO_CYC_BOUNDS_CHECKS
  if(ans < arr.base || ans >= arr.last_plus_one)
    _throw_arraybounds();
#endif
  return ans;
}
static inline 
struct _tagged_arr _tag_arr(const void * curr, 
			    unsigned elt_sz, unsigned num_elts) {
  // beware the gcc bug, can happen with *temp = _tag_arr(...) in weird places!
  struct _tagged_arr ans;
  ans.base = (void *)curr;
  ans.curr = (void *)curr;
  ans.last_plus_one = ((char *)curr) + elt_sz * num_elts;
  return ans;
}
static inline
struct _tagged_arr * _init_tag_arr(struct _tagged_arr * arr_ptr, void * arr, 
				   unsigned elt_sz, unsigned num_elts) {
  // we use this to (hopefully) avoid the gcc bug
  arr_ptr->base = arr_ptr->curr = arr;
  arr_ptr->last_plus_one = ((char *)arr) + elt_sz * num_elts;
  return arr_ptr;
}

static inline
char * _untag_arr(struct _tagged_arr arr, unsigned elt_sz, unsigned num_elts) {
  // Note: if arr is "null" base and curr should both be null, so this
  //       is correct (caller checks for null if untagging to @ type)
  // base may not be null if you use t ? pointer subtraction to get 0 -- oh well
#ifndef NO_CYC_BOUNDS_CHECKS
  if(arr.curr < arr.base || arr.curr + elt_sz * num_elts > arr.last_plus_one)
    _throw_arraybounds();
#endif
  return arr.curr;
}
static inline 
unsigned _get_arr_size(struct _tagged_arr arr, unsigned elt_sz) {
  return (arr.curr<arr.base || arr.curr>=arr.last_plus_one) ? 0 : ((arr.last_plus_one - arr.curr) / elt_sz);
}
static inline
struct _tagged_arr _tagged_arr_plus(struct _tagged_arr arr, unsigned elt_sz,
				    int change) {
  struct _tagged_arr ans = arr;
  ans.curr += ((int)elt_sz)*change;
  return ans;
}
static inline
struct _tagged_arr _tagged_arr_inplace_plus(struct _tagged_arr * arr_ptr,
					    unsigned elt_sz, int change) {
  arr_ptr->curr += ((int)elt_sz)*change;
  return *arr_ptr;
}
static inline
struct _tagged_arr _tagged_arr_inplace_plus_post(struct _tagged_arr * arr_ptr,
						 unsigned elt_sz, int change) {
  struct _tagged_arr ans = *arr_ptr;
  arr_ptr->curr += ((int)elt_sz)*change;
  return ans;
}
				  
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
struct _tagged_arr Cyc_Core_new_string(unsigned int);extern void*Cyc_Core_identity(
void*);extern int Cyc_Core_intcmp(int,int);extern unsigned char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{unsigned char*tag;struct _tagged_arr f1;
};extern unsigned char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
unsigned char*tag;struct _tagged_arr f1;};extern unsigned char Cyc_Core_Impossible[
15];struct Cyc_Core_Impossible_struct{unsigned char*tag;struct _tagged_arr f1;};
extern unsigned char Cyc_Core_Not_found[14];extern unsigned char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{unsigned char*tag;struct _tagged_arr f1;};
struct Cyc_Cstdio___sFILE;struct Cyc_Std___sFILE;extern unsigned char Cyc_Std_FileCloseError[
19];extern unsigned char Cyc_Std_FileOpenError[18];struct Cyc_Std_FileOpenError_struct{
unsigned char*tag;struct _tagged_arr f1;};static const int Cyc_Std_String_pa=0;struct
Cyc_Std_String_pa_struct{int tag;struct _tagged_arr f1;};static const int Cyc_Std_Int_pa=
1;struct Cyc_Std_Int_pa_struct{int tag;unsigned int f1;};static const int Cyc_Std_Double_pa=
2;struct Cyc_Std_Double_pa_struct{int tag;double f1;};static const int Cyc_Std_ShortPtr_pa=
3;struct Cyc_Std_ShortPtr_pa_struct{int tag;short*f1;};static const int Cyc_Std_IntPtr_pa=
4;struct Cyc_Std_IntPtr_pa_struct{int tag;unsigned int*f1;};extern int Cyc_Std_fprintf(
struct Cyc_Std___sFILE*,struct _tagged_arr fmt,struct _tagged_arr);static const int Cyc_Std_ShortPtr_sa=
0;struct Cyc_Std_ShortPtr_sa_struct{int tag;short*f1;};static const int Cyc_Std_UShortPtr_sa=
1;struct Cyc_Std_UShortPtr_sa_struct{int tag;unsigned short*f1;};static const int Cyc_Std_IntPtr_sa=
2;struct Cyc_Std_IntPtr_sa_struct{int tag;int*f1;};static const int Cyc_Std_UIntPtr_sa=
3;struct Cyc_Std_UIntPtr_sa_struct{int tag;unsigned int*f1;};static const int Cyc_Std_StringPtr_sa=
4;struct Cyc_Std_StringPtr_sa_struct{int tag;struct _tagged_arr f1;};static const int
Cyc_Std_DoublePtr_sa=5;struct Cyc_Std_DoublePtr_sa_struct{int tag;double*f1;};
static const int Cyc_Std_FloatPtr_sa=6;struct Cyc_Std_FloatPtr_sa_struct{int tag;
float*f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern int Cyc_List_length(
struct Cyc_List_List*x);extern unsigned char Cyc_List_List_mismatch[18];extern
unsigned char Cyc_List_Nth[8];extern unsigned int Cyc_Std_strlen(struct _tagged_arr s);
extern int Cyc_Std_zstrptrcmp(struct _tagged_arr*,struct _tagged_arr*);extern struct
_tagged_arr Cyc_Std_str_sepstr(struct Cyc_List_List*,struct _tagged_arr);struct Cyc_Hashtable_Table;
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),
int(*hash)(void*));extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,
void*key,void*val);extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,
void*key);extern int Cyc_Hashtable_hash_stringptr(struct _tagged_arr*p);static const
int Cyc_Fn_Fun=0;struct Cyc_Fn_Fun_struct{int tag;void*(*f1)(void*,void*);void*f2;}
;extern void*Cyc_Fn_make_fn(void*(*f)(void*,void*),void*x);extern void*Cyc_Fn_fp2fn(
void*(*f)(void*));extern void*Cyc_Fn_apply(void*f,void*x);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;extern void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*
d,int w,struct Cyc_Std___sFILE*f);extern struct _tagged_arr Cyc_PP_string_of_doc(
struct Cyc_PP_Doc*d,int w);struct _tuple0{struct _tagged_arr f1;struct Cyc_List_List*
f2;};extern struct _tuple0*Cyc_PP_string_and_links(struct Cyc_PP_Doc*d,int w);extern
struct Cyc_PP_Doc*Cyc_PP_nil_doc();extern struct Cyc_PP_Doc*Cyc_PP_blank_doc();
extern struct Cyc_PP_Doc*Cyc_PP_line_doc();extern struct Cyc_PP_Doc*Cyc_PP_oline_doc();
extern struct Cyc_PP_Doc*Cyc_PP_text(struct _tagged_arr s);extern struct Cyc_PP_Doc*
Cyc_PP_textptr(struct _tagged_arr*p);extern struct Cyc_PP_Doc*Cyc_PP_hyperlink(
struct _tagged_arr shrt,struct _tagged_arr full);extern struct Cyc_PP_Doc*Cyc_PP_nest(
int k,struct Cyc_PP_Doc*d);extern struct Cyc_PP_Doc*Cyc_PP_cat(struct _tagged_arr);
extern struct Cyc_PP_Doc*Cyc_PP_cats(struct Cyc_List_List*doclist);extern struct Cyc_PP_Doc*
Cyc_PP_cats_arr(struct _tagged_arr docs);extern struct Cyc_PP_Doc*Cyc_PP_doc_union(
struct Cyc_PP_Doc*d1,struct Cyc_PP_Doc*d2);extern struct Cyc_PP_Doc*Cyc_PP_tab(
struct Cyc_PP_Doc*d);extern struct Cyc_PP_Doc*Cyc_PP_seq(struct _tagged_arr sep,
struct Cyc_List_List*l);extern struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(
void*),struct _tagged_arr sep,struct Cyc_List_List*l);extern struct Cyc_PP_Doc*Cyc_PP_seql(
struct _tagged_arr sep,struct Cyc_List_List*l0);extern struct Cyc_PP_Doc*Cyc_PP_ppseql(
struct Cyc_PP_Doc*(*pp)(void*),struct _tagged_arr sep,struct Cyc_List_List*l);extern
struct Cyc_PP_Doc*Cyc_PP_group(struct _tagged_arr start,struct _tagged_arr stop,
struct _tagged_arr sep,struct Cyc_List_List*l);extern struct Cyc_PP_Doc*Cyc_PP_groupl(
struct _tagged_arr start,struct _tagged_arr stop,struct _tagged_arr sep,struct Cyc_List_List*
l);extern struct Cyc_PP_Doc*Cyc_PP_egroup(struct _tagged_arr start,struct _tagged_arr
stop,struct _tagged_arr sep,struct Cyc_List_List*l);struct Cyc_Xarray_Xarray{struct
_tagged_arr elmts;int num_elmts;};extern int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
extern void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);extern void Cyc_Xarray_set(
struct Cyc_Xarray_Xarray*,int,void*);extern struct Cyc_Xarray_Xarray*Cyc_Xarray_create(
int,void*);extern void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);static const
int Cyc_PP_Empty=0;static const int Cyc_PP_Single=0;struct Cyc_PP_Single_struct{int
tag;void*f1;};static const int Cyc_PP_Append=1;struct Cyc_PP_Append_struct{int tag;
void*f1;void*f2;};void*Cyc_PP_append(void*a1,void*a2){if(a1 == (void*)Cyc_PP_Empty){
return a2;}else{if(a2 == (void*)Cyc_PP_Empty){return a1;}else{return(void*)({struct
Cyc_PP_Append_struct*_tmp0=_cycalloc(sizeof(struct Cyc_PP_Append_struct));_tmp0[0]=({
struct Cyc_PP_Append_struct _tmp1;_tmp1.tag=Cyc_PP_Append;_tmp1.f1=(void*)a1;_tmp1.f2=(
void*)a2;_tmp1;});_tmp0;});}}}struct Cyc_List_List*Cyc_PP_list_of_alist_f(void*y,
struct Cyc_List_List*l){void*_tmp2=y;void*_tmpA;void*_tmpC;void*_tmpE;_LL4: if(
_tmp2 == (void*)Cyc_PP_Empty){goto _LL5;}else{goto _LL6;}_LL6: if((unsigned int)
_tmp2 > 1?*((int*)_tmp2)== Cyc_PP_Single: 0){_LL11: _tmpA=(void*)((struct Cyc_PP_Single_struct*)
_tmp2)->f1;goto _LL7;}else{goto _LL8;}_LL8: if((unsigned int)_tmp2 > 1?*((int*)_tmp2)
== Cyc_PP_Append: 0){_LL15: _tmpE=(void*)((struct Cyc_PP_Append_struct*)_tmp2)->f1;
goto _LL13;_LL13: _tmpC=(void*)((struct Cyc_PP_Append_struct*)_tmp2)->f2;goto _LL9;}
else{goto _LL3;}_LL5: return l;_LL7: return({struct Cyc_List_List*_tmp10=_cycalloc(
sizeof(struct Cyc_List_List));_tmp10->hd=(void*)_tmpA;_tmp10->tl=l;_tmp10;});_LL9:
return Cyc_PP_list_of_alist_f(_tmpE,Cyc_PP_list_of_alist_f(_tmpC,l));_LL3:;}
struct Cyc_List_List*Cyc_PP_list_of_alist(void*x){return Cyc_PP_list_of_alist_f(x,
0);}struct Cyc_PP_Ppstate{int ci;int cc;int cl;int pw;int epw;};struct Cyc_PP_Out{int
newcc;int newcl;void*ppout;void*links;};struct Cyc_PP_Doc{int mwo;int mw;void*f;};
static void Cyc_PP_dump_out(struct Cyc_Std___sFILE*f,void*al){struct Cyc_Xarray_Xarray*
_tmp11=Cyc_Xarray_create(16,al);Cyc_Xarray_add(_tmp11,al);{int last=0;while(last
>= 0){void*_tmp12=Cyc_Xarray_get(_tmp11,last);struct _tagged_arr*_tmp1A;void*
_tmp1C;void*_tmp1E;_LL20: if(_tmp12 == (void*)Cyc_PP_Empty){goto _LL21;}else{goto
_LL22;}_LL22: if((unsigned int)_tmp12 > 1?*((int*)_tmp12)== Cyc_PP_Single: 0){_LL27:
_tmp1A=(struct _tagged_arr*)((struct Cyc_PP_Single_struct*)_tmp12)->f1;goto _LL23;}
else{goto _LL24;}_LL24: if((unsigned int)_tmp12 > 1?*((int*)_tmp12)== Cyc_PP_Append:
0){_LL31: _tmp1E=(void*)((struct Cyc_PP_Append_struct*)_tmp12)->f1;goto _LL29;_LL29:
_tmp1C=(void*)((struct Cyc_PP_Append_struct*)_tmp12)->f2;goto _LL25;}else{goto
_LL19;}_LL21: -- last;goto _LL19;_LL23: -- last;({struct Cyc_Std_String_pa_struct
_tmp21;_tmp21.tag=Cyc_Std_String_pa;_tmp21.f1=(struct _tagged_arr)*_tmp1A;{void*
_tmp20[1]={& _tmp21};Cyc_Std_fprintf(f,_tag_arr("%s",sizeof(unsigned char),3),
_tag_arr(_tmp20,sizeof(void*),1));}});goto _LL19;_LL25: Cyc_Xarray_set(_tmp11,last,
_tmp1C);if(last == Cyc_Xarray_length(_tmp11)- 1){Cyc_Xarray_add(_tmp11,_tmp1E);}
else{Cyc_Xarray_set(_tmp11,last + 1,_tmp1E);}++ last;goto _LL19;_LL19:;}}}void Cyc_PP_file_of_doc(
struct Cyc_PP_Doc*d,int w,struct Cyc_Std___sFILE*f){struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(
void*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)((void*)d->f,({struct Cyc_PP_Ppstate*
_tmp22=_cycalloc_atomic(sizeof(struct Cyc_PP_Ppstate));_tmp22->ci=0;_tmp22->cc=0;
_tmp22->cl=1;_tmp22->pw=w;_tmp22->epw=w;_tmp22;}));Cyc_PP_dump_out(f,(void*)o->ppout);}
struct _tagged_arr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w){struct Cyc_PP_Out*
o=((struct Cyc_PP_Out*(*)(void*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)((void*)d->f,({
struct Cyc_PP_Ppstate*_tmp23=_cycalloc_atomic(sizeof(struct Cyc_PP_Ppstate));
_tmp23->ci=0;_tmp23->cc=0;_tmp23->cl=1;_tmp23->pw=w;_tmp23->epw=w;_tmp23;}));
return(struct _tagged_arr)Cyc_Std_str_sepstr(((struct Cyc_List_List*(*)(void*x))
Cyc_PP_list_of_alist)((void*)o->ppout),_tag_arr("",sizeof(unsigned char),1));}
struct _tuple0*Cyc_PP_string_and_links(struct Cyc_PP_Doc*d,int w){struct Cyc_PP_Out*
o=((struct Cyc_PP_Out*(*)(void*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)((void*)d->f,({
struct Cyc_PP_Ppstate*_tmp25=_cycalloc_atomic(sizeof(struct Cyc_PP_Ppstate));
_tmp25->ci=0;_tmp25->cc=0;_tmp25->cl=1;_tmp25->pw=w;_tmp25->epw=w;_tmp25;}));
return({struct _tuple0*_tmp24=_cycalloc(sizeof(struct _tuple0));_tmp24->f1=(struct
_tagged_arr)Cyc_Std_str_sepstr(((struct Cyc_List_List*(*)(void*x))Cyc_PP_list_of_alist)((
void*)o->ppout),_tag_arr("",sizeof(unsigned char),1));_tmp24->f2=((struct Cyc_List_List*(*)(
void*x))Cyc_PP_list_of_alist)((void*)o->links);_tmp24;});}static struct Cyc_Core_Opt*
Cyc_PP_bhashtbl=0;struct _tagged_arr Cyc_PP_nlblanks(int i){if(Cyc_PP_bhashtbl == 0){
Cyc_PP_bhashtbl=({struct Cyc_Core_Opt*_tmp26=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp26->v=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(int,int),int(*hash)(
int)))Cyc_Hashtable_create)(61,Cyc_Core_intcmp,(int(*)(int))Cyc_Core_identity);
_tmp26;});}if(i < 0){(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp27=
_cycalloc(sizeof(struct Cyc_Core_Failure_struct));_tmp27[0]=({struct Cyc_Core_Failure_struct
_tmp28;_tmp28.tag=Cyc_Core_Failure;_tmp28.f1=_tag_arr("nlblanks",sizeof(
unsigned char),9);_tmp28;});_tmp27;}));}{struct _handler_cons _tmp29;_push_handler(&
_tmp29);{int _tmp2B=0;if(setjmp(_tmp29.handler)){_tmp2B=1;}if(! _tmp2B){{struct
_tagged_arr _tmp2C=*((struct _tagged_arr*(*)(struct Cyc_Hashtable_Table*t,int key))
Cyc_Hashtable_lookup)((struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)
_check_null(Cyc_PP_bhashtbl))->v,i);_npop_handler(0);return _tmp2C;};_pop_handler();}
else{void*_tmp2A=(void*)_exn_thrown;void*_tmp2E=_tmp2A;_LL48: if(_tmp2E == Cyc_Core_Not_found){
goto _LL49;}else{goto _LL50;}_LL50: goto _LL51;_LL49: {int _tmp34=i / 8;int _tmp35=i % 8;
int _tmp36=(1 + _tmp34)+ _tmp35;struct _tagged_arr nlb=Cyc_Core_new_string((
unsigned int)_tmp36);*((unsigned char*)_check_unknown_subscript(nlb,sizeof(
unsigned char),0))='\n';{int j=0;for(0;j < _tmp34;j ++){*((unsigned char*)
_check_unknown_subscript(nlb,sizeof(unsigned char),j + 1))='\t';}}{int j=0;for(0;j
< _tmp35;j ++){*((unsigned char*)_check_unknown_subscript(nlb,sizeof(
unsigned char),(j + 1)+ _tmp34))=' ';}}((void(*)(struct Cyc_Hashtable_Table*t,int
key,struct _tagged_arr*val))Cyc_Hashtable_insert)((struct Cyc_Hashtable_Table*)((
struct Cyc_Core_Opt*)_check_null(Cyc_PP_bhashtbl))->v,i,({struct _tagged_arr*
_tmp37=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp37[0]=(struct _tagged_arr)nlb;
_tmp37;}));return(struct _tagged_arr)nlb;}_LL51:(void)_throw(_tmp2E);_LL47:;}}}}
static struct Cyc_Core_Opt*Cyc_PP_str_hashtbl=0;int Cyc_PP_infinity=9999999;struct
_tuple1{int f1;struct _tagged_arr f2;};static struct Cyc_PP_Out*Cyc_PP_text_doc_f(
struct _tuple1*clo,struct Cyc_PP_Ppstate*st){struct _tuple1 _tmp3A;struct _tagged_arr
_tmp3B;int _tmp3D;struct _tuple1*_tmp38=clo;_tmp3A=*_tmp38;_LL62: _tmp3D=_tmp3A.f1;
goto _LL60;_LL60: _tmp3B=_tmp3A.f2;goto _LL57;_LL57: return({struct Cyc_PP_Out*_tmp3F=
_cycalloc(sizeof(struct Cyc_PP_Out));_tmp3F->newcc=st->cc + _tmp3D;_tmp3F->newcl=
st->cl;_tmp3F->ppout=(void*)((void*)({struct Cyc_PP_Single_struct*_tmp40=
_cycalloc(sizeof(struct Cyc_PP_Single_struct));_tmp40[0]=({struct Cyc_PP_Single_struct
_tmp41;_tmp41.tag=Cyc_PP_Single;_tmp41.f1=(void*)({struct _tagged_arr*_tmp42=
_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp42[0]=_tmp3B;_tmp42;});_tmp41;});
_tmp40;}));_tmp3F->links=(void*)((void*)Cyc_PP_Empty);_tmp3F;});}static struct Cyc_PP_Doc*
Cyc_PP_text_doc(struct _tagged_arr s){int slen=(int)Cyc_Std_strlen(s);return({
struct Cyc_PP_Doc*_tmp43=_cycalloc(sizeof(struct Cyc_PP_Doc));_tmp43->mwo=slen;
_tmp43->mw=Cyc_PP_infinity;_tmp43->f=(void*)((void*(*)(struct Cyc_PP_Out*(*f)(
struct _tuple1*,struct Cyc_PP_Ppstate*),struct _tuple1*x))Cyc_Fn_make_fn)(Cyc_PP_text_doc_f,({
struct _tuple1*_tmp44=_cycalloc(sizeof(struct _tuple1));_tmp44->f1=slen;_tmp44->f2=
s;_tmp44;}));_tmp43;});}struct Cyc_PP_Doc*Cyc_PP_text(struct _tagged_arr s){struct
Cyc_Hashtable_Table*t;if(Cyc_PP_str_hashtbl == 0){t=((struct Cyc_Hashtable_Table*(*)(
int sz,int(*cmp)(struct _tagged_arr*,struct _tagged_arr*),int(*hash)(struct
_tagged_arr*)))Cyc_Hashtable_create)(101,Cyc_Std_zstrptrcmp,Cyc_Hashtable_hash_stringptr);
Cyc_PP_str_hashtbl=({struct Cyc_Core_Opt*_tmp45=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp45->v=t;_tmp45;});}else{t=(struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)
_check_null(Cyc_PP_str_hashtbl))->v;}{struct _handler_cons _tmp46;_push_handler(&
_tmp46);{int _tmp48=0;if(setjmp(_tmp46.handler)){_tmp48=1;}if(! _tmp48){{struct Cyc_PP_Doc*
_tmp4A=((struct Cyc_PP_Doc*(*)(struct Cyc_Hashtable_Table*t,struct _tagged_arr*key))
Cyc_Hashtable_lookup)(t,({struct _tagged_arr*_tmp49=_cycalloc(sizeof(struct
_tagged_arr));_tmp49[0]=s;_tmp49;}));_npop_handler(0);return _tmp4A;};
_pop_handler();}else{void*_tmp47=(void*)_exn_thrown;void*_tmp4C=_tmp47;_LL78: if(
_tmp4C == Cyc_Core_Not_found){goto _LL79;}else{goto _LL80;}_LL80: goto _LL81;_LL79: {
struct Cyc_PP_Doc*d=Cyc_PP_text_doc(s);((void(*)(struct Cyc_Hashtable_Table*t,
struct _tagged_arr*key,struct Cyc_PP_Doc*val))Cyc_Hashtable_insert)(t,({struct
_tagged_arr*_tmp52=_cycalloc(sizeof(struct _tagged_arr));_tmp52[0]=s;_tmp52;}),d);
return d;}_LL81:(void)_throw(_tmp4C);_LL77:;}}}}struct Cyc_PP_Doc*Cyc_PP_textptr(
struct _tagged_arr*s){return Cyc_PP_text(*s);}struct Cyc_Core_Opt*Cyc_PP_nil_doc_opt=
0;struct Cyc_Core_Opt*Cyc_PP_blank_doc_opt=0;struct Cyc_Core_Opt*Cyc_PP_line_doc_opt=
0;struct Cyc_PP_Doc*Cyc_PP_nil_doc(){if(Cyc_PP_nil_doc_opt == 0){Cyc_PP_nil_doc_opt=({
struct Cyc_Core_Opt*_tmp53=_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp53->v=Cyc_PP_text(
_tag_arr("",sizeof(unsigned char),1));_tmp53;});}return(struct Cyc_PP_Doc*)((
struct Cyc_Core_Opt*)_check_null(Cyc_PP_nil_doc_opt))->v;}struct Cyc_PP_Doc*Cyc_PP_blank_doc(){
if(Cyc_PP_blank_doc_opt == 0){Cyc_PP_blank_doc_opt=({struct Cyc_Core_Opt*_tmp54=
_cycalloc(sizeof(struct Cyc_Core_Opt));_tmp54->v=Cyc_PP_text(_tag_arr("",sizeof(
unsigned char),1));_tmp54;});}return(struct Cyc_PP_Doc*)((struct Cyc_Core_Opt*)
_check_null(Cyc_PP_blank_doc_opt))->v;}struct _tuple2{int f1;struct _tagged_arr f2;
struct _tagged_arr f3;};struct _tuple3{int f1;int f2;int f3;struct _tagged_arr f4;};
static struct Cyc_PP_Out*Cyc_PP_hyperlink_f(struct _tuple2*clo,struct Cyc_PP_Ppstate*
st){struct _tuple2 _tmp57;struct _tagged_arr _tmp58;struct _tagged_arr _tmp5A;int
_tmp5C;struct _tuple2*_tmp55=clo;_tmp57=*_tmp55;_LL93: _tmp5C=_tmp57.f1;goto _LL91;
_LL91: _tmp5A=_tmp57.f2;goto _LL89;_LL89: _tmp58=_tmp57.f3;goto _LL86;_LL86: return({
struct Cyc_PP_Out*_tmp5E=_cycalloc(sizeof(struct Cyc_PP_Out));_tmp5E->newcc=st->cc
+ _tmp5C;_tmp5E->newcl=st->cl;_tmp5E->ppout=(void*)((void*)({struct Cyc_PP_Single_struct*
_tmp62=_cycalloc(sizeof(struct Cyc_PP_Single_struct));_tmp62[0]=({struct Cyc_PP_Single_struct
_tmp63;_tmp63.tag=Cyc_PP_Single;_tmp63.f1=(void*)({struct _tagged_arr*_tmp64=
_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp64[0]=_tmp5A;_tmp64;});_tmp63;});
_tmp62;}));_tmp5E->links=(void*)((void*)({struct Cyc_PP_Single_struct*_tmp5F=
_cycalloc(sizeof(struct Cyc_PP_Single_struct));_tmp5F[0]=({struct Cyc_PP_Single_struct
_tmp60;_tmp60.tag=Cyc_PP_Single;_tmp60.f1=(void*)({struct _tuple3*_tmp61=
_cycalloc(sizeof(struct _tuple3));_tmp61->f1=st->cl;_tmp61->f2=st->cc;_tmp61->f3=
_tmp5C;_tmp61->f4=_tmp58;_tmp61;});_tmp60;});_tmp5F;}));_tmp5E;});}struct Cyc_PP_Doc*
Cyc_PP_hyperlink(struct _tagged_arr shrt,struct _tagged_arr full){int slen=(int)Cyc_Std_strlen(
shrt);return({struct Cyc_PP_Doc*_tmp65=_cycalloc(sizeof(struct Cyc_PP_Doc));_tmp65->mwo=
slen;_tmp65->mw=Cyc_PP_infinity;_tmp65->f=(void*)((void*(*)(struct Cyc_PP_Out*(*f)(
struct _tuple2*,struct Cyc_PP_Ppstate*),struct _tuple2*x))Cyc_Fn_make_fn)(Cyc_PP_hyperlink_f,({
struct _tuple2*_tmp66=_cycalloc(sizeof(struct _tuple2));_tmp66->f1=slen;_tmp66->f2=
shrt;_tmp66->f3=full;_tmp66;}));_tmp65;});}static struct Cyc_PP_Out*Cyc_PP_line_f(
struct Cyc_PP_Ppstate*st){return({struct Cyc_PP_Out*_tmp67=_cycalloc(sizeof(struct
Cyc_PP_Out));_tmp67->newcc=st->ci;_tmp67->newcl=st->cl + 1;_tmp67->ppout=(void*)((
void*)({struct Cyc_PP_Single_struct*_tmp68=_cycalloc(sizeof(struct Cyc_PP_Single_struct));
_tmp68[0]=({struct Cyc_PP_Single_struct _tmp69;_tmp69.tag=Cyc_PP_Single;_tmp69.f1=(
void*)({struct _tagged_arr*_tmp6A=_cycalloc(sizeof(struct _tagged_arr)* 1);_tmp6A[
0]=Cyc_PP_nlblanks(st->ci);_tmp6A;});_tmp69;});_tmp68;}));_tmp67->links=(void*)((
void*)Cyc_PP_Empty);_tmp67;});}struct Cyc_PP_Doc*Cyc_PP_line_doc(){if(Cyc_PP_line_doc_opt
== 0){Cyc_PP_line_doc_opt=({struct Cyc_Core_Opt*_tmp6B=_cycalloc(sizeof(struct Cyc_Core_Opt));
_tmp6B->v=({struct Cyc_PP_Doc*_tmp6C=_cycalloc(sizeof(struct Cyc_PP_Doc));_tmp6C->mwo=
0;_tmp6C->mw=0;_tmp6C->f=(void*)((void*(*)(struct Cyc_PP_Out*(*f)(struct Cyc_PP_Ppstate*)))
Cyc_Fn_fp2fn)(Cyc_PP_line_f);_tmp6C;});_tmp6B;});}return(struct Cyc_PP_Doc*)((
struct Cyc_Core_Opt*)_check_null(Cyc_PP_line_doc_opt))->v;}struct _tuple4{int f1;
struct Cyc_PP_Doc*f2;};static struct Cyc_PP_Out*Cyc_PP_nest_f(struct _tuple4*clo,
struct Cyc_PP_Ppstate*st){struct _tuple4 _tmp6F;struct Cyc_PP_Doc*_tmp70;int _tmp72;
struct _tuple4*_tmp6D=clo;_tmp6F=*_tmp6D;_LL115: _tmp72=_tmp6F.f1;goto _LL113;
_LL113: _tmp70=_tmp6F.f2;goto _LL110;_LL110: {struct Cyc_PP_Ppstate*st2=({struct Cyc_PP_Ppstate*
_tmp74=_cycalloc_atomic(sizeof(struct Cyc_PP_Ppstate));_tmp74->ci=st->ci + _tmp72;
_tmp74->cc=st->cc;_tmp74->cl=st->cl;_tmp74->pw=st->pw;_tmp74->epw=st->epw;_tmp74;});
return((struct Cyc_PP_Out*(*)(void*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)((void*)
_tmp70->f,st2);}}struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d){return({
struct Cyc_PP_Doc*_tmp75=_cycalloc(sizeof(struct Cyc_PP_Doc));_tmp75->mwo=d->mwo;
_tmp75->mw=d->mw;_tmp75->f=(void*)((void*(*)(struct Cyc_PP_Out*(*f)(struct _tuple4*,
struct Cyc_PP_Ppstate*),struct _tuple4*x))Cyc_Fn_make_fn)(Cyc_PP_nest_f,({struct
_tuple4*_tmp76=_cycalloc(sizeof(struct _tuple4));_tmp76->f1=k;_tmp76->f2=d;_tmp76;}));
_tmp75;});}int Cyc_PP_min(int x,int y){if(x <= y){return x;}else{return y;}}int Cyc_PP_max(
int x,int y){if(x >= y){return x;}else{return y;}}struct _tuple5{struct Cyc_PP_Doc*f1;
struct Cyc_PP_Doc*f2;};static struct Cyc_PP_Out*Cyc_PP_concat_f(struct _tuple5*clo,
struct Cyc_PP_Ppstate*st){struct _tuple5 _tmp79;struct Cyc_PP_Doc*_tmp7A;struct Cyc_PP_Doc*
_tmp7C;struct _tuple5*_tmp77=clo;_tmp79=*_tmp77;_LL125: _tmp7C=_tmp79.f1;goto
_LL123;_LL123: _tmp7A=_tmp79.f2;goto _LL120;_LL120: {int epw2=Cyc_PP_max(st->pw - 
_tmp7A->mw,st->epw - _tmp7C->mwo);struct Cyc_PP_Ppstate*st2=({struct Cyc_PP_Ppstate*
_tmp80=_cycalloc_atomic(sizeof(struct Cyc_PP_Ppstate));_tmp80->ci=st->ci;_tmp80->cc=
st->cc;_tmp80->cl=st->cl;_tmp80->pw=st->pw;_tmp80->epw=epw2;_tmp80;});struct Cyc_PP_Out*
o1=((struct Cyc_PP_Out*(*)(void*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)((void*)
_tmp7C->f,st2);struct Cyc_PP_Ppstate*st3=({struct Cyc_PP_Ppstate*_tmp7F=
_cycalloc_atomic(sizeof(struct Cyc_PP_Ppstate));_tmp7F->ci=st->ci;_tmp7F->cc=o1->newcc;
_tmp7F->cl=o1->newcl;_tmp7F->pw=st->pw;_tmp7F->epw=epw2;_tmp7F;});struct Cyc_PP_Out*
o2=((struct Cyc_PP_Out*(*)(void*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)((void*)
_tmp7A->f,st3);struct Cyc_PP_Out*o3=({struct Cyc_PP_Out*_tmp7E=_cycalloc(sizeof(
struct Cyc_PP_Out));_tmp7E->newcc=o2->newcc;_tmp7E->newcl=o2->newcl;_tmp7E->ppout=(
void*)((void*(*)(void*a1,void*a2))Cyc_PP_append)((void*)o1->ppout,(void*)o2->ppout);
_tmp7E->links=(void*)((void*(*)(void*a1,void*a2))Cyc_PP_append)((void*)o1->links,(
void*)o2->links);_tmp7E;});return o3;}}static struct Cyc_PP_Doc*Cyc_PP_concat(
struct Cyc_PP_Doc*d1,struct Cyc_PP_Doc*d2){return({struct Cyc_PP_Doc*_tmp81=
_cycalloc(sizeof(struct Cyc_PP_Doc));_tmp81->mwo=Cyc_PP_min(d1->mw,d1->mwo + d2->mwo);
_tmp81->mw=Cyc_PP_min(d1->mw,d1->mwo + d2->mw);_tmp81->f=(void*)((void*(*)(struct
Cyc_PP_Out*(*f)(struct _tuple5*,struct Cyc_PP_Ppstate*),struct _tuple5*x))Cyc_Fn_make_fn)(
Cyc_PP_concat_f,({struct _tuple5*_tmp82=_cycalloc(sizeof(struct _tuple5));_tmp82->f1=
d1;_tmp82->f2=d2;_tmp82;}));_tmp81;});}struct Cyc_PP_Doc*Cyc_PP_cat(struct
_tagged_arr l){struct Cyc_PP_Doc*_tmp83=Cyc_PP_nil_doc();{int i=(int)(_get_arr_size(
l,sizeof(struct Cyc_PP_Doc*))- 1);for(0;i >= 0;i --){_tmp83=Cyc_PP_concat(((struct
Cyc_PP_Doc**)l.curr)[i],_tmp83);}}return _tmp83;}static struct Cyc_PP_Out*Cyc_PP_long_cats_f(
struct Cyc_List_List*ds0,struct Cyc_PP_Ppstate*st){struct Cyc_List_List*os=0;{
struct Cyc_List_List*_tmp84=ds0;for(0;_tmp84 != 0;_tmp84=_tmp84->tl){struct Cyc_PP_Doc*
d=(struct Cyc_PP_Doc*)_tmp84->hd;struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(void*f,
struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)((void*)d->f,st);st=({struct Cyc_PP_Ppstate*
_tmp85=_cycalloc_atomic(sizeof(struct Cyc_PP_Ppstate));_tmp85->ci=st->ci;_tmp85->cc=
o->newcc;_tmp85->cl=o->newcl;_tmp85->pw=st->pw;_tmp85->epw=st->epw - d->mwo;
_tmp85;});os=({struct Cyc_List_List*_tmp86=_cycalloc(sizeof(struct Cyc_List_List));
_tmp86->hd=o;_tmp86->tl=os;_tmp86;});}}{int newcc=((struct Cyc_PP_Out*)((struct Cyc_List_List*)
_check_null(os))->hd)->newcc;int newcl=((struct Cyc_PP_Out*)os->hd)->newcl;void*s=(
void*)Cyc_PP_Empty;void*links=(void*)Cyc_PP_Empty;for(0;os != 0;os=os->tl){s=((
void*(*)(void*a1,void*a2))Cyc_PP_append)((void*)((struct Cyc_PP_Out*)os->hd)->ppout,
s);links=((void*(*)(void*a1,void*a2))Cyc_PP_append)((void*)((struct Cyc_PP_Out*)
os->hd)->links,links);}return({struct Cyc_PP_Out*_tmp87=_cycalloc(sizeof(struct
Cyc_PP_Out));_tmp87->newcc=newcc;_tmp87->newcl=newcl;_tmp87->ppout=(void*)s;
_tmp87->links=(void*)links;_tmp87;});}}static struct Cyc_PP_Doc*Cyc_PP_long_cats(
struct Cyc_List_List*doclist){struct Cyc_List_List*orig=doclist;struct Cyc_PP_Doc*d=(
struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(doclist))->hd;doclist=
doclist->tl;{int mw=d->mw;int mwo=d->mw;{struct Cyc_List_List*_tmp88=doclist;for(0;
_tmp88 != 0;_tmp88=_tmp88->tl){int mw2=Cyc_PP_min(mw,mwo + ((struct Cyc_PP_Doc*)
_tmp88->hd)->mwo);int mwo2=Cyc_PP_min(mw,mwo + ((struct Cyc_PP_Doc*)_tmp88->hd)->mw);
mw=mw2;mwo=mwo2;}}return({struct Cyc_PP_Doc*_tmp89=_cycalloc(sizeof(struct Cyc_PP_Doc));
_tmp89->mwo=mw;_tmp89->mw=mwo;_tmp89->f=(void*)((void*(*)(struct Cyc_PP_Out*(*f)(
struct Cyc_List_List*,struct Cyc_PP_Ppstate*),struct Cyc_List_List*x))Cyc_Fn_make_fn)(
Cyc_PP_long_cats_f,orig);_tmp89;});}}struct Cyc_PP_Doc*Cyc_PP_cats(struct Cyc_List_List*
doclist){if(doclist == 0){return Cyc_PP_nil_doc();}else{if(doclist->tl == 0){return(
struct Cyc_PP_Doc*)doclist->hd;}else{if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
doclist)> 30){return Cyc_PP_long_cats(doclist);}else{return Cyc_PP_concat((struct
Cyc_PP_Doc*)doclist->hd,Cyc_PP_cats(doclist->tl));}}}}static struct Cyc_PP_Out*Cyc_PP_cats_arr_f(
struct _tagged_arr*docs_ptr,struct Cyc_PP_Ppstate*st){struct Cyc_List_List*os=0;
struct _tagged_arr docs=*docs_ptr;int sz=(int)_get_arr_size(docs,sizeof(struct Cyc_PP_Doc*));{
int i=0;for(0;i < sz;++ i){struct Cyc_PP_Doc*d=((struct Cyc_PP_Doc**)docs.curr)[i];
struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(void*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)((
void*)d->f,st);st=({struct Cyc_PP_Ppstate*_tmp8A=_cycalloc_atomic(sizeof(struct
Cyc_PP_Ppstate));_tmp8A->ci=st->ci;_tmp8A->cc=o->newcc;_tmp8A->cl=o->newcl;
_tmp8A->pw=st->pw;_tmp8A->epw=st->epw - d->mwo;_tmp8A;});os=({struct Cyc_List_List*
_tmp8B=_cycalloc(sizeof(struct Cyc_List_List));_tmp8B->hd=o;_tmp8B->tl=os;_tmp8B;});}}{
int newcc=((struct Cyc_PP_Out*)((struct Cyc_List_List*)_check_null(os))->hd)->newcc;
int newcl=((struct Cyc_PP_Out*)os->hd)->newcl;void*s=(void*)Cyc_PP_Empty;void*
links=(void*)Cyc_PP_Empty;for(0;os != 0;os=os->tl){s=((void*(*)(void*a1,void*a2))
Cyc_PP_append)((void*)((struct Cyc_PP_Out*)os->hd)->ppout,s);links=((void*(*)(
void*a1,void*a2))Cyc_PP_append)((void*)((struct Cyc_PP_Out*)os->hd)->links,links);}
return({struct Cyc_PP_Out*_tmp8C=_cycalloc(sizeof(struct Cyc_PP_Out));_tmp8C->newcc=
newcc;_tmp8C->newcl=newcl;_tmp8C->ppout=(void*)s;_tmp8C->links=(void*)links;
_tmp8C;});}}struct Cyc_PP_Doc*Cyc_PP_cats_arr(struct _tagged_arr docs){int sz=(int)
_get_arr_size(docs,sizeof(struct Cyc_PP_Doc*));if(sz == 0){(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmp8D=_cycalloc(sizeof(struct Cyc_Core_Failure_struct));
_tmp8D[0]=({struct Cyc_Core_Failure_struct _tmp8E;_tmp8E.tag=Cyc_Core_Failure;
_tmp8E.f1=_tag_arr("cats_arr -- size zero array",sizeof(unsigned char),28);
_tmp8E;});_tmp8D;}));}{struct Cyc_PP_Doc*d=*((struct Cyc_PP_Doc**)
_check_unknown_subscript(docs,sizeof(struct Cyc_PP_Doc*),0));int mw=d->mw;int mwo=d->mw;{
int i=1;for(0;i < sz;++ i){int mw2=Cyc_PP_min(mw,mwo + (((struct Cyc_PP_Doc**)docs.curr)[
i])->mwo);int mwo2=Cyc_PP_min(mw,mwo + (((struct Cyc_PP_Doc**)docs.curr)[i])->mw);
mw=mw2;mwo=mwo2;}}return({struct Cyc_PP_Doc*_tmp8F=_cycalloc(sizeof(struct Cyc_PP_Doc));
_tmp8F->mwo=mw;_tmp8F->mw=mwo;_tmp8F->f=(void*)((void*(*)(struct Cyc_PP_Out*(*f)(
struct _tagged_arr*,struct Cyc_PP_Ppstate*),struct _tagged_arr*x))Cyc_Fn_make_fn)(
Cyc_PP_cats_arr_f,({struct _tagged_arr*_tmp90=_cycalloc(sizeof(struct _tagged_arr)
* 1);_tmp90[0]=docs;_tmp90;}));_tmp8F;});}}static struct Cyc_PP_Out*Cyc_PP_doc_union_f(
struct _tuple5*clo,struct Cyc_PP_Ppstate*st){struct _tuple5 _tmp93;struct Cyc_PP_Doc*
_tmp94;struct Cyc_PP_Doc*_tmp96;struct _tuple5*_tmp91=clo;_tmp93=*_tmp91;_LL151:
_tmp96=_tmp93.f1;goto _LL149;_LL149: _tmp94=_tmp93.f2;goto _LL146;_LL146: {int dfits=
st->cc + _tmp96->mwo <= st->epw? 1: st->cc + _tmp96->mw <= st->pw;if(dfits){return((
struct Cyc_PP_Out*(*)(void*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)((void*)_tmp96->f,
st);}else{return((struct Cyc_PP_Out*(*)(void*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)((
void*)_tmp94->f,st);}}}struct Cyc_PP_Doc*Cyc_PP_doc_union(struct Cyc_PP_Doc*d,
struct Cyc_PP_Doc*d2){return({struct Cyc_PP_Doc*_tmp98=_cycalloc(sizeof(struct Cyc_PP_Doc));
_tmp98->mwo=Cyc_PP_min(d->mwo,d2->mwo);_tmp98->mw=Cyc_PP_min(d->mw,d2->mw);
_tmp98->f=(void*)((void*(*)(struct Cyc_PP_Out*(*f)(struct _tuple5*,struct Cyc_PP_Ppstate*),
struct _tuple5*x))Cyc_Fn_make_fn)(Cyc_PP_doc_union_f,({struct _tuple5*_tmp99=
_cycalloc(sizeof(struct _tuple5));_tmp99->f1=d;_tmp99->f2=d2;_tmp99;}));_tmp98;});}
struct Cyc_PP_Doc*Cyc_PP_oline_doc(){return Cyc_PP_doc_union(Cyc_PP_nil_doc(),Cyc_PP_line_doc());}
static struct Cyc_PP_Out*Cyc_PP_tab_f(struct Cyc_PP_Doc*d,struct Cyc_PP_Ppstate*st){
struct Cyc_PP_Ppstate*st2=({struct Cyc_PP_Ppstate*_tmp9A=_cycalloc_atomic(sizeof(
struct Cyc_PP_Ppstate));_tmp9A->ci=st->cc;_tmp9A->cc=st->cc;_tmp9A->cl=st->cl;
_tmp9A->pw=st->pw;_tmp9A->epw=st->epw;_tmp9A;});return((struct Cyc_PP_Out*(*)(
void*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)((void*)d->f,st2);}struct Cyc_PP_Doc*
Cyc_PP_tab(struct Cyc_PP_Doc*d){struct Cyc_PP_Doc*d2=({struct Cyc_PP_Doc*_tmp9B=
_cycalloc(sizeof(struct Cyc_PP_Doc));_tmp9B->mwo=d->mwo;_tmp9B->mw=d->mw;_tmp9B->f=(
void*)((void*(*)(struct Cyc_PP_Out*(*f)(struct Cyc_PP_Doc*,struct Cyc_PP_Ppstate*),
struct Cyc_PP_Doc*x))Cyc_Fn_make_fn)(Cyc_PP_tab_f,d);_tmp9B;});return d2;}static
struct Cyc_PP_Doc*Cyc_PP_ppseq_f(struct Cyc_PP_Doc*(*pp)(void*),struct _tagged_arr
sep,struct Cyc_List_List*l){if(l == 0){return Cyc_PP_nil_doc();}else{if(l->tl == 0){
return pp((void*)l->hd);}else{return({struct Cyc_PP_Doc*_tmp9C[4];_tmp9C[3]=Cyc_PP_ppseq_f(
pp,sep,l->tl);_tmp9C[2]=Cyc_PP_oline_doc();_tmp9C[1]=Cyc_PP_text(sep);_tmp9C[0]=
pp((void*)l->hd);Cyc_PP_cat(_tag_arr(_tmp9C,sizeof(struct Cyc_PP_Doc*),4));});}}}
struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _tagged_arr sep,
struct Cyc_List_List*l){return Cyc_PP_tab(Cyc_PP_ppseq_f(pp,sep,l));}struct Cyc_PP_Doc*
Cyc_PP_seq_f(struct _tagged_arr sep,struct Cyc_List_List*l){if(l == 0){return Cyc_PP_nil_doc();}
else{if(l->tl == 0){return(struct Cyc_PP_Doc*)l->hd;}else{struct Cyc_PP_Doc*sep2=
Cyc_PP_text(sep);struct Cyc_PP_Doc*oline=Cyc_PP_oline_doc();struct Cyc_List_List*
_tmp9D=l;while(_tmp9D->tl != 0){struct Cyc_List_List*_tmp9E=_tmp9D->tl;_tmp9D->tl=({
struct Cyc_List_List*_tmp9F=_cycalloc(sizeof(struct Cyc_List_List));_tmp9F->hd=
sep2;_tmp9F->tl=({struct Cyc_List_List*_tmpA0=_cycalloc(sizeof(struct Cyc_List_List));
_tmpA0->hd=oline;_tmpA0->tl=_tmp9E;_tmpA0;});_tmp9F;});_tmp9D=_tmp9E;}return Cyc_PP_cats(
l);}}}struct Cyc_PP_Doc*Cyc_PP_seq(struct _tagged_arr sep,struct Cyc_List_List*l){
return Cyc_PP_tab(Cyc_PP_seq_f(sep,l));}struct Cyc_PP_Doc*Cyc_PP_ppseql_f(struct
Cyc_PP_Doc*(*pp)(void*),struct _tagged_arr sep,struct Cyc_List_List*l){if(l == 0){
return Cyc_PP_nil_doc();}else{if(l->tl == 0){return pp((void*)l->hd);}else{return({
struct Cyc_PP_Doc*_tmpA1[4];_tmpA1[3]=Cyc_PP_ppseql_f(pp,sep,l->tl);_tmpA1[2]=Cyc_PP_line_doc();
_tmpA1[1]=Cyc_PP_text(sep);_tmpA1[0]=pp((void*)l->hd);Cyc_PP_cat(_tag_arr(_tmpA1,
sizeof(struct Cyc_PP_Doc*),4));});}}}struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*
pp)(void*),struct _tagged_arr sep,struct Cyc_List_List*l){return Cyc_PP_tab(Cyc_PP_ppseql_f(
pp,sep,l));}static struct Cyc_PP_Doc*Cyc_PP_seql_f(struct _tagged_arr sep,struct Cyc_List_List*
l){if(l == 0){return Cyc_PP_nil_doc();}else{if(l->tl == 0){return(struct Cyc_PP_Doc*)
l->hd;}else{return({struct Cyc_PP_Doc*_tmpA2[4];_tmpA2[3]=Cyc_PP_seql_f(sep,l->tl);
_tmpA2[2]=Cyc_PP_line_doc();_tmpA2[1]=Cyc_PP_text(sep);_tmpA2[0]=(struct Cyc_PP_Doc*)
l->hd;Cyc_PP_cat(_tag_arr(_tmpA2,sizeof(struct Cyc_PP_Doc*),4));});}}}struct Cyc_PP_Doc*
Cyc_PP_seql(struct _tagged_arr sep,struct Cyc_List_List*l0){return Cyc_PP_tab(Cyc_PP_seql_f(
sep,l0));}struct Cyc_PP_Doc*Cyc_PP_group(struct _tagged_arr start,struct _tagged_arr
stop,struct _tagged_arr sep,struct Cyc_List_List*ss){return({struct Cyc_PP_Doc*
_tmpA3[3];_tmpA3[2]=Cyc_PP_text(stop);_tmpA3[1]=Cyc_PP_seq(sep,ss);_tmpA3[0]=Cyc_PP_text(
start);Cyc_PP_cat(_tag_arr(_tmpA3,sizeof(struct Cyc_PP_Doc*),3));});}struct Cyc_PP_Doc*
Cyc_PP_egroup(struct _tagged_arr start,struct _tagged_arr stop,struct _tagged_arr sep,
struct Cyc_List_List*ss){if(ss == 0){return Cyc_PP_nil_doc();}else{return({struct
Cyc_PP_Doc*_tmpA4[3];_tmpA4[2]=Cyc_PP_text(stop);_tmpA4[1]=Cyc_PP_seq(sep,ss);
_tmpA4[0]=Cyc_PP_text(start);Cyc_PP_cat(_tag_arr(_tmpA4,sizeof(struct Cyc_PP_Doc*),
3));});}}struct Cyc_PP_Doc*Cyc_PP_groupl(struct _tagged_arr start,struct _tagged_arr
stop,struct _tagged_arr sep,struct Cyc_List_List*ss){return({struct Cyc_PP_Doc*
_tmpA5[3];_tmpA5[2]=Cyc_PP_text(stop);_tmpA5[1]=Cyc_PP_seql(sep,ss);_tmpA5[0]=
Cyc_PP_text(start);Cyc_PP_cat(_tag_arr(_tmpA5,sizeof(struct Cyc_PP_Doc*),3));});}
