#ifndef _SETJMP_H_
#define _SETJMP_H_
#ifndef ___sigset_t_def_
#define ___sigset_t_def_
typedef struct {unsigned long __val[1024 / (8 * sizeof(unsigned long))];} __sigset_t;
#endif
#ifndef ___jmp_buf_def_
#define ___jmp_buf_def_
typedef int __jmp_buf[6];
#endif
#ifndef ___jmp_buf_tag_def_
#define ___jmp_buf_tag_def_
struct __jmp_buf_tag{
  __jmp_buf __jmpbuf;
  int __mask_was_saved;
  __sigset_t __saved_mask;
};
#endif
#ifndef _jmp_buf_def_
#define _jmp_buf_def_
typedef struct __jmp_buf_tag jmp_buf[1];
#endif
extern int setjmp(jmp_buf);
#endif
/* This is a C header file to be used by the output of the Cyclone to
   C translator.  The corresponding definitions are in file
   lib/runtime_cyc.c
*/
#ifndef _CYC_INCLUDE_H_
#define _CYC_INCLUDE_H_

#ifdef NO_CYC_PREFIX
#define ADD_PREFIX(x) x
#else
#define ADD_PREFIX(x) Cyc_##x
#endif

#ifndef offsetof
/* should be size_t, but int is fine. */
#define offsetof(t,n) ((int)(&(((t *)0)->n)))
#endif

/* Tagged arrays */
struct _dyneither_ptr {
  unsigned char *curr; 
  unsigned char *base; 
  unsigned char *last_plus_one; 
};  

/* Discriminated Unions */
struct _xtunion_struct { char *tag; };

/* Need one of these per thread (we don't have threads)
   The runtime maintains a stack that contains either _handler_cons
   structs or _RegionHandle structs.  The tag is 0 for a handler_cons
   and 1 for a region handle.  */
struct _RuntimeStack {
  int tag; /* 0 for an exception handler, 1 for a region handle */
  struct _RuntimeStack *next;
};

/* Regions */
struct _RegionPage {
#ifdef CYC_REGION_PROFILE
  unsigned total_bytes;
  unsigned free_bytes;
#endif
  struct _RegionPage *next;
  char data[1];  /*FJS: used to be size 0, but that's forbidden in ansi c*/
};

struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
  struct _DynRegionHandle *sub_regions;
#ifdef CYC_REGION_PROFILE
  const char         *name;
#endif
};

struct _DynRegionFrame {
  struct _RuntimeStack s;
  struct _DynRegionHandle *x;
};

extern struct _RegionHandle _new_region(const char *);
extern void * _region_malloc(struct _RegionHandle *, unsigned);
extern void * _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
extern void   _free_region(struct _RegionHandle *);
extern void   _reset_region(struct _RegionHandle *);
extern struct _RegionHandle *_open_dynregion(struct _DynRegionFrame *f,
                                             struct _DynRegionHandle *h);
extern void   _pop_dynregion();

/* Exceptions */
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

/* Built-in Exceptions */
struct Cyc_Null_Exception_struct { char *tag; };
struct Cyc_Array_bounds_struct { char *tag; };
struct Cyc_Match_Exception_struct { char *tag; };
struct Cyc_Bad_alloc_struct { char *tag; };
extern char Cyc_Null_Exception[];
extern char Cyc_Array_bounds[];
extern char Cyc_Match_Exception[];
extern char Cyc_Bad_alloc[];

/* Built-in Run-time Checks and company */
#ifdef __APPLE__
#define _INLINE_FUNCTIONS
#endif

#ifdef CYC_ANSI_OUTPUT
#define _INLINE  
#define _INLINE_FUNCTIONS
#else
#define _INLINE inline
#endif

#ifdef VC_C
#define _CYC_U_LONG_LONG_T __int64
#else
#ifdef GCC_C
#define _CYC_U_LONG_LONG_T unsigned long long
#else
#define _CYC_U_LONG_LONG_T unsigned long long
#endif
#endif

#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE void *
_check_null(void *ptr) {
  void*_check_null_temp = (void*)(ptr);
  if (!_check_null_temp) _throw_null();
  return _check_null_temp;
}
#else
#define _check_null(ptr) \
  ({ void*_check_null_temp = (void*)(ptr); \
     if (!_check_null_temp) _throw_null(); \
     _check_null_temp; })
#endif
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  ((char *)ptr) + (elt_sz)*(index); })
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE char *
_check_known_subscript_null(void *ptr, unsigned bound, unsigned elt_sz, unsigned index) {
  void*_cks_ptr = (void*)(ptr);
  unsigned _cks_bound = (bound);
  unsigned _cks_elt_sz = (elt_sz);
  unsigned _cks_index = (index);
  if (!_cks_ptr) _throw_null();
  if (_cks_index >= _cks_bound) _throw_arraybounds();
  return ((char *)_cks_ptr) + _cks_elt_sz*_cks_index;
}
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
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_notnull(bound,index) (index)
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned
_check_known_subscript_notnull(unsigned bound,unsigned index) { 
  unsigned _cksnn_bound = (bound); 
  unsigned _cksnn_index = (index); 
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); 
  return _cksnn_index;
}
#else
#define _check_known_subscript_notnull(bound,index) ({ \
  unsigned _cksnn_bound = (bound); \
  unsigned _cksnn_index = (index); \
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); \
  _cksnn_index; })
#endif
#endif

/* Add i to zero-terminated pointer x.  Checks for x being null and
   ensures that x[0..i-1] are not 0. */
#ifdef NO_CYC_BOUNDS_CHECK
#define _zero_arr_plus_char(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_short(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_int(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_float(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_double(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_longdouble(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#define _zero_arr_plus_voidstar(orig_x,orig_sz,orig_i) ((orig_x)+(orig_i))
#else
static _INLINE char *
_zero_arr_plus_char(char *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE short *
_zero_arr_plus_short(short *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE int *
_zero_arr_plus_int(int *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE float *
_zero_arr_plus_float(float *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE double *
_zero_arr_plus_double(double *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE long double *
_zero_arr_plus_longdouble(long double *orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
static _INLINE void *
_zero_arr_plus_voidstar(void **orig_x, int orig_sz, int orig_i) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null();
  if (orig_i < 0) _throw_arraybounds();
  for (_czs_temp=orig_sz; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds();
  return orig_x + orig_i;
}
#endif


/* Calculates the number of elements in a zero-terminated, thin array.
   If non-null, the array is guaranteed to have orig_offset elements. */
static _INLINE int
_get_zero_arr_size_char(const char *orig_x, unsigned int orig_offset) {
  const char *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_short(const short *orig_x, unsigned int orig_offset) {
  const short *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_int(const int *orig_x, unsigned int orig_offset) {
  const int *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_float(const float *orig_x, unsigned int orig_offset) {
  const float *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_double(const double *orig_x, unsigned int orig_offset) {
  const double *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_longdouble(const long double *orig_x, unsigned int orig_offset) {
  const long double *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_voidstar(const void **orig_x, unsigned int orig_offset) {
  const void **_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}


/* Does in-place addition of a zero-terminated pointer (x += e and ++x).  
   Note that this expands to call _zero_arr_plus. */
/*#define _zero_arr_inplace_plus(x,orig_i) ({ \
  typedef _zap_tx = (*x); \
  _zap_tx **_zap_x = &((_zap_tx*)x); \
  *_zap_x = _zero_arr_plus(*_zap_x,1,(orig_i)); })
  */
static _INLINE void 
_zero_arr_inplace_plus_char(char *x, int orig_i) {
  char **_zap_x = &x;
  *_zap_x = _zero_arr_plus_char(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_short(short *x, int orig_i) {
  short **_zap_x = &x;
  *_zap_x = _zero_arr_plus_short(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_int(int *x, int orig_i) {
  int **_zap_x = &x;
  *_zap_x = _zero_arr_plus_int(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_float(float *x, int orig_i) {
  float **_zap_x = &x;
  *_zap_x = _zero_arr_plus_float(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_double(double *x, int orig_i) {
  double **_zap_x = &x;
  *_zap_x = _zero_arr_plus_double(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_longdouble(long double *x, int orig_i) {
  long double **_zap_x = &x;
  *_zap_x = _zero_arr_plus_longdouble(*_zap_x,1,orig_i);
}
static _INLINE void 
_zero_arr_inplace_plus_voidstar(void **x, int orig_i) {
  void ***_zap_x = &x;
  *_zap_x = _zero_arr_plus_voidstar(*_zap_x,1,orig_i);
}




/* Does in-place increment of a zero-terminated pointer (e.g., x++).
   Note that this expands to call _zero_arr_plus. */
/*#define _zero_arr_inplace_plus_post(x,orig_i) ({ \
  typedef _zap_tx = (*x); \
  _zap_tx **_zap_x = &((_zap_tx*)x); \
  _zap_tx *_zap_res = *_zap_x; \
  *_zap_x = _zero_arr_plus(_zap_res,1,(orig_i)); \
  _zap_res; })*/
  
static _INLINE char *
_zero_arr_inplace_plus_post_char(char *x, int orig_i){
  char ** _zap_x = &x;
  char * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_char(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE short *
_zero_arr_inplace_plus_post_short(short *x, int orig_i){
  short **_zap_x = &x;
  short * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_short(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE int *
_zero_arr_inplace_plus_post_int(int *x, int orig_i){
  int **_zap_x = &x;
  int * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_int(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE float *
_zero_arr_inplace_plus_post_float(float *x, int orig_i){
  float **_zap_x = &x;
  float * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_float(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE double *
_zero_arr_inplace_plus_post_double(double *x, int orig_i){
  double **_zap_x = &x;
  double * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_double(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE long double *
_zero_arr_inplace_plus_post_longdouble(long double *x, int orig_i){
  long double **_zap_x = &x;
  long double * _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_longdouble(_zap_res,1,orig_i);
  return _zap_res;
}
static _INLINE void **
_zero_arr_inplace_plus_post_voidstar(void **x, int orig_i){
  void ***_zap_x = &x;
  void ** _zap_res = *_zap_x;
  *_zap_x = _zero_arr_plus_voidstar(_zap_res,1,orig_i);
  return _zap_res;
}



/* functions for dealing with dynamically sized pointers */
#ifdef NO_CYC_BOUNDS_CHECKS
#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned char *
_check_dyneither_subscript(struct _dyneither_ptr arr,unsigned elt_sz,unsigned index) {
  struct _dyneither_ptr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  return _cus_ans;
}
#else
#define _check_dyneither_subscript(arr,elt_sz,index) ({ \
  struct _dyneither_ptr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  _cus_ans; })
#endif
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned char *
_check_dyneither_subscript(struct _dyneither_ptr arr,unsigned elt_sz,unsigned index) {
  struct _dyneither_ptr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  /* JGM: not needed! if (!_cus_arr.base) _throw_null(); */ 
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one)
    _throw_arraybounds();
  return _cus_ans;
}
#else
#define _check_dyneither_subscript(arr,elt_sz,index) ({ \
  struct _dyneither_ptr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  /* JGM: not needed! if (!_cus_arr.base) _throw_null();*/ \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#endif
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_tag_dyneither(const void *tcurr,unsigned elt_sz,unsigned num_elts) {
  struct _dyneither_ptr _tag_arr_ans;
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr);
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts);
  return _tag_arr_ans;
}
#else
#define _tag_dyneither(tcurr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr _tag_arr_ans; \
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr *
_init_dyneither_ptr(struct _dyneither_ptr *arr_ptr,
                    void *arr, unsigned elt_sz, unsigned num_elts) {
  struct _dyneither_ptr *_itarr_ptr = (arr_ptr);
  void* _itarr = (arr);
  _itarr_ptr->base = _itarr_ptr->curr = _itarr;
  _itarr_ptr->last_plus_one = ((unsigned char *)_itarr) + (elt_sz) * (num_elts);
  return _itarr_ptr;
}
#else
#define _init_dyneither_ptr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr *_itarr_ptr = (arr_ptr); \
  void* _itarr = (arr); \
  _itarr_ptr->base = _itarr_ptr->curr = _itarr; \
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts); \
  _itarr_ptr; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _untag_dyneither_ptr(arr,elt_sz,num_elts) ((arr).curr)
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned char *
_untag_dyneither_ptr(struct _dyneither_ptr arr, 
                     unsigned elt_sz,unsigned num_elts) {
  struct _dyneither_ptr _arr = (arr);
  unsigned char *_curr = _arr.curr;
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)
    _throw_arraybounds();
  return _curr;
}
#else
#define _untag_dyneither_ptr(arr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)\
    _throw_arraybounds(); \
  _curr; })
#endif
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned
_get_dyneither_size(struct _dyneither_ptr arr,unsigned elt_sz) {
  struct _dyneither_ptr _get_arr_size_temp = (arr);
  unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr;
  unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one;
  return (_get_arr_size_curr < _get_arr_size_temp.base ||
          _get_arr_size_curr >= _get_arr_size_last) ? 0 :
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));
}
#else
#define _get_dyneither_size(arr,elt_sz) \
  ({struct _dyneither_ptr _get_arr_size_temp = (arr); \
    unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr; \
    unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one; \
    (_get_arr_size_curr < _get_arr_size_temp.base || \
     _get_arr_size_curr >= _get_arr_size_last) ? 0 : \
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));})
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_dyneither_ptr_plus(struct _dyneither_ptr arr,unsigned elt_sz,int change) {
  struct _dyneither_ptr _ans = (arr);
  _ans.curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _dyneither_ptr_plus(arr,elt_sz,change) ({ \
  struct _dyneither_ptr _ans = (arr); \
  _ans.curr += ((int)(elt_sz))*(change); \
  _ans; })
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_dyneither_ptr_inplace_plus(struct _dyneither_ptr *arr_ptr,unsigned elt_sz,
                            int change) {
  struct _dyneither_ptr * _arr_ptr = (arr_ptr);
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return *_arr_ptr;
}
#else
#define _dyneither_ptr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _dyneither_ptr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  *_arr_ptr; })
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_dyneither_ptr_inplace_plus_post(struct _dyneither_ptr *arr_ptr,unsigned elt_sz,int change) {
  struct _dyneither_ptr * _arr_ptr = (arr_ptr);
  struct _dyneither_ptr _ans = *_arr_ptr;
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _dyneither_ptr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _dyneither_ptr * _arr_ptr = (arr_ptr); \
  struct _dyneither_ptr _ans = *_arr_ptr; \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  _ans; })
#endif

/* Decrease the upper bound on a fat pointer by numelts where sz is
   the size of the pointer's type.  Note that this can't be a macro
   if we're to get initializers right. */
static struct 
_dyneither_ptr _dyneither_ptr_decrease_size(struct _dyneither_ptr x,
                                            unsigned int sz,
                                            unsigned int numelts) {
  x.last_plus_one -= sz * numelts; 
  return x; 
}

/* Allocation */
extern void* GC_malloc(int);
extern void* GC_malloc_atomic(int);
extern void* GC_calloc(unsigned,unsigned);
extern void* GC_calloc_atomic(unsigned,unsigned);

static _INLINE void* _cycalloc(int n) {
  void * ans = (void *)GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static _INLINE void* _cycalloc_atomic(int n) {
  void * ans = (void *)GC_malloc_atomic(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static _INLINE void* _cyccalloc(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
static _INLINE void* _cyccalloc_atomic(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc_atomic(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
#define MAX_MALLOC_SIZE (1 << 28)
static _INLINE unsigned int _check_times(unsigned x, unsigned y) {
  _CYC_U_LONG_LONG_T whole_ans = 
    ((_CYC_U_LONG_LONG_T)x)*((_CYC_U_LONG_LONG_T)y);
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

/* the next two routines swap [x] and [y]; not thread safe! */
static _INLINE void _swap_word(void *x, void *y) {
  unsigned long *lx = (unsigned long *)x, *ly = (unsigned long *)y, tmp;
  tmp = *lx;
  *lx = *ly;
  *ly = tmp;
}
static _INLINE void _swap_dyneither(struct _dyneither_ptr *x, 
				   struct _dyneither_ptr *y) {
  struct _dyneither_ptr tmp = *x;
  *x = *y;
  *y = tmp;
}
 void exit(int);void*abort();struct Cyc_Core_NewRegion{struct _DynRegionHandle*
dynregion;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{char*tag;struct
_dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{
char*tag;};struct _dyneither_ptr wrap_Cbuffer_as_buffer(char*,unsigned int);struct
Cyc_in_addr{unsigned int s_addr;};unsigned int Cyc_htonl(unsigned int x);
unsigned short Cyc_htons(unsigned short x);unsigned int Cyc_ntohl(unsigned int x);
unsigned short Cyc_ntohs(unsigned short x);unsigned int __stub_htonl(unsigned int x);
unsigned short __stub_htons(unsigned short x);unsigned int __stub_ntohl(
unsigned int x);unsigned short __stub_ntohs(unsigned short x);unsigned int Cyc_htonl(
unsigned int x){return __stub_htonl(x);}unsigned short Cyc_htons(unsigned short x){
return __stub_htons(x);}unsigned int Cyc_ntohl(unsigned int x){return __stub_ntohl(x);}
unsigned short Cyc_ntohs(unsigned short x){return __stub_ntohs(x);}struct Cyc_dirent{
unsigned long d_ino;long d_off;unsigned short d_reclen;unsigned char d_type;char
d_name[256];};struct Cyc___cycDIR;int Cyc_closedir(struct Cyc___cycDIR*);struct Cyc___cycDIR*
Cyc_opendir(const char*);struct Cyc_dirent*Cyc_readdir(struct Cyc___cycDIR*);void
Cyc_rewinddir(struct Cyc___cycDIR*);void Cyc_seekdir(struct Cyc___cycDIR*,long);
long Cyc_telldir(struct Cyc___cycDIR*);struct Cyc_Cdirent___abstractDIR;struct Cyc___cycDIR{
struct Cyc_Cdirent___abstractDIR*dir;};int closedir(struct Cyc_Cdirent___abstractDIR*
d);int Cyc_closedir(struct Cyc___cycDIR*d){return closedir(d->dir);}struct Cyc_Cdirent___abstractDIR*
opendir(const char*);static char _tmp1[32]="opendir called with NULL string";static
struct Cyc_Core_Failure_struct Cyc___opendir_failure={Cyc_Core_Failure,{_tmp1,
_tmp1,_tmp1 + 32}};struct Cyc___cycDIR*Cyc_opendir(const char*f){struct Cyc_Cdirent___abstractDIR*
_tmp2=opendir(f);return(unsigned int)_tmp2?({struct Cyc___cycDIR*_tmp3=_cycalloc(
sizeof(*_tmp3));_tmp3->dir=(struct Cyc_Cdirent___abstractDIR*)_tmp2;_tmp3;}): 0;}
struct Cyc_dirent*readdir(struct Cyc_Cdirent___abstractDIR*);struct Cyc_dirent*Cyc_readdir(
struct Cyc___cycDIR*d){return readdir(d->dir);}void rewinddir(struct Cyc_Cdirent___abstractDIR*
d);void Cyc_rewinddir(struct Cyc___cycDIR*d){return rewinddir(d->dir);}void seekdir(
struct Cyc_Cdirent___abstractDIR*d,long);void Cyc_seekdir(struct Cyc___cycDIR*d,
long x){return seekdir(d->dir,x);}long telldir(struct Cyc_Cdirent___abstractDIR*d);
long Cyc_telldir(struct Cyc___cycDIR*d){return telldir(d->dir);}int*__CYCLONE_ERRNO();
struct Cyc_flock{short l_type;short l_whence;long l_start;long l_len;int l_pid;};
struct Cyc_Flock_struct{int tag;struct Cyc_flock*f1;};struct Cyc_Long_struct{int tag;
long f1;};int Cyc_fcntl(int fd,int cmd,struct _dyneither_ptr);int Cyc_open(const char*,
int,struct _dyneither_ptr);int fcntl(int fd,int cmd);int fcntl_with_arg(int fd,int cmd,
long arg);int fcntl_with_lock(int fd,int cmd,struct Cyc_flock*lock);int
open_without_mode(const char*,int);int open_with_mode(const char*,int,unsigned int);
static char _tmp5[21]="fcntl: too many args";static struct Cyc_Core_Failure_struct
Cyc___fcntl_failure={Cyc_Core_Failure,{_tmp5,_tmp5,_tmp5 + 21}};int Cyc_fcntl(int
fd,int cmd,struct _dyneither_ptr argv){if(_get_dyneither_size(argv,sizeof(void*))== 
0)return fcntl(fd,cmd);else{if(_get_dyneither_size(argv,sizeof(void*))!= 1)(int)
_throw((void*)& Cyc___fcntl_failure);else{void*_tmp6=*((void**)
_check_dyneither_subscript(argv,sizeof(void*),0));long _tmp8;struct Cyc_flock*
_tmpA;_LL1: {struct Cyc_Long_struct*_tmp7=(struct Cyc_Long_struct*)_tmp6;if(_tmp7->tag
!= 1)goto _LL3;else{_tmp8=_tmp7->f1;}}_LL2: return fcntl_with_arg(fd,cmd,_tmp8);
_LL3: {struct Cyc_Flock_struct*_tmp9=(struct Cyc_Flock_struct*)_tmp6;if(_tmp9->tag
!= 0)goto _LL0;else{_tmpA=_tmp9->f1;}}_LL4: return fcntl_with_lock(fd,cmd,(struct
Cyc_flock*)_tmpA);_LL0:;}}}int Cyc_open(const char*s,int i,struct _dyneither_ptr ms){
if(_get_dyneither_size(ms,sizeof(unsigned int))>= 1)return open_with_mode(s,i,*((
unsigned int*)_check_dyneither_subscript(ms,sizeof(unsigned int),0)));else{
return open_without_mode(s,i);}}enum Cyc___anonymous_enum_51__{Cyc_FE_INVALID  = 1,
Cyc___FE_DENORM  = 2,Cyc_FE_DIVBYZERO  = 4,Cyc_FE_OVERFLOW  = 8,Cyc_FE_UNDERFLOW  = 
16,Cyc_FE_INEXACT  = 32};enum Cyc___anonymous_enum_52__{Cyc_FE_TONEAREST  = 0,Cyc_FE_DOWNWARD
 = 1024,Cyc_FE_UPWARD  = 2048,Cyc_FE_TOWARDZERO  = 3072};typedef struct{
unsigned short __control_word;unsigned short __unused1;unsigned short __status_word;
unsigned short __unused2;unsigned short __tags;unsigned short __unused3;
unsigned int __eip;unsigned short __cs_selector;unsigned int __opcode: 11;
unsigned int __unused4: 5;unsigned int __data_offset;unsigned short __data_selector;
unsigned short __unused5;}Cyc_fenv_t;union Cyc___convert_long_double{unsigned int
__convert_long_double_i[4];long double __convert_long_double_d;};enum Cyc___anonymous_enum_61__{
Cyc_MM_HARD  = 1,Cyc_MM_SOFT  = 2,Cyc_MM_FIRM  = 4,Cyc_MM_APPL  = 8,Cyc_MM_UTIL  = 16,
Cyc_MM_OPSYS  = 32,Cyc_MM_RECOVER  = 64,Cyc_MM_NRECOV  = 128,Cyc_MM_PRINT  = 256,Cyc_MM_CONSOLE
 = 512};enum Cyc___anonymous_enum_62__{Cyc_MM_NOSEV  = 0,Cyc_MM_HALT  = 1,Cyc_MM_ERROR
 = 2,Cyc_MM_WARNING  = 3,Cyc_MM_INFO  = 4};enum Cyc___anonymous_enum_63__{Cyc_MM_NOTOK
 = -1,Cyc_MM_OK  = 0,Cyc_MM_NOMSG  = 1,Cyc_MM_NOCON  = 4};enum Cyc___anonymous_enum_119__{
Cyc_IPPORT_ECHO  = 7,Cyc_IPPORT_DISCARD  = 9,Cyc_IPPORT_SYSTAT  = 11,Cyc_IPPORT_DAYTIME
 = 13,Cyc_IPPORT_NETSTAT  = 15,Cyc_IPPORT_FTP  = 21,Cyc_IPPORT_TELNET  = 23,Cyc_IPPORT_SMTP
 = 25,Cyc_IPPORT_TIMESERVER  = 37,Cyc_IPPORT_NAMESERVER  = 42,Cyc_IPPORT_WHOIS  = 
43,Cyc_IPPORT_MTP  = 57,Cyc_IPPORT_TFTP  = 69,Cyc_IPPORT_RJE  = 77,Cyc_IPPORT_FINGER
 = 79,Cyc_IPPORT_TTYLINK  = 87,Cyc_IPPORT_SUPDUP  = 95,Cyc_IPPORT_EXECSERVER  = 512,
Cyc_IPPORT_LOGINSERVER  = 513,Cyc_IPPORT_CMDSERVER  = 514,Cyc_IPPORT_EFSSERVER  = 
520,Cyc_IPPORT_BIFFUDP  = 512,Cyc_IPPORT_WHOSERVER  = 513,Cyc_IPPORT_ROUTESERVER
 = 520,Cyc_IPPORT_RESERVED  = 1024,Cyc_IPPORT_USERRESERVED  = 5000};struct Cyc_servent{
char*s_name;char**s_aliases;int s_port;char*s_proto;};struct Cyc_protoent{char*
p_name;char**p_aliases;int p_proto;};struct Cyc_hostent{char*h_name;char**
h_aliases;int h_addrtype;int h_length;struct Cyc_in_addr**h_addr_list;};enum Cyc___anonymous_enum_122__{
Cyc_IPPROTO_IP  = 0,Cyc_IPPROTO_HOPOPTS  = 0,Cyc_IPPROTO_ICMP  = 1,Cyc_IPPROTO_IGMP
 = 2,Cyc_IPPROTO_IPIP  = 4,Cyc_IPPROTO_TCP  = 6,Cyc_IPPROTO_EGP  = 8,Cyc_IPPROTO_PUP
 = 12,Cyc_IPPROTO_UDP  = 17,Cyc_IPPROTO_IDP  = 22,Cyc_IPPROTO_TP  = 29,Cyc_IPPROTO_IPV6
 = 41,Cyc_IPPROTO_ROUTING  = 43,Cyc_IPPROTO_FRAGMENT  = 44,Cyc_IPPROTO_RSVP  = 46,
Cyc_IPPROTO_GRE  = 47,Cyc_IPPROTO_ESP  = 50,Cyc_IPPROTO_AH  = 51,Cyc_IPPROTO_ICMPV6
 = 58,Cyc_IPPROTO_NONE  = 59,Cyc_IPPROTO_DSTOPTS  = 60,Cyc_IPPROTO_MTP  = 92,Cyc_IPPROTO_ENCAP
 = 98,Cyc_IPPROTO_PIM  = 103,Cyc_IPPROTO_COMP  = 108,Cyc_IPPROTO_RAW  = 255,Cyc_IPPROTO_MAX
 = 256};struct Cyc_in6_addr{union{unsigned char u6_addr8[16];unsigned short
u6_addr16[8];unsigned int u6_addr32[4];}in6_u;};struct Cyc_sockaddr{unsigned short
sa_family;char sa_data[14];};struct Cyc_sockaddr_in{unsigned short sin_family;
unsigned short sin_port;struct Cyc_in_addr sin_addr;unsigned char sin_zero[((sizeof(
struct Cyc_sockaddr)- sizeof(unsigned short))- sizeof(unsigned short))- sizeof(
struct Cyc_in_addr)];};struct Cyc_sockaddr_in6{unsigned short sin6_family;
unsigned short sin6_port;unsigned int sin6_flowinfo;struct Cyc_in6_addr sin6_addr;
unsigned int sin6_scope_id;};struct Cyc_ipv6_mreq{struct Cyc_in6_addr
ipv6mr_multiaddr;unsigned int ipv6mr_interface;};struct Cyc_pollfd{int fd;short
events;short revents;};int Cyc_poll(struct _dyneither_ptr,unsigned long,int);int
poll(struct Cyc_pollfd*,unsigned long b,int c);static char _tmpC[22]="poll called with NULL";
static struct Cyc_Core_Failure_struct Cyc___poll_failure_1={Cyc_Core_Failure,{_tmpC,
_tmpC,_tmpC + 22}};static char _tmpE[39]="poll called with out-of-bounds pointer";
static struct Cyc_Core_Failure_struct Cyc___poll_failure_2={Cyc_Core_Failure,{_tmpE,
_tmpE,_tmpE + 39}};int Cyc_poll(struct _dyneither_ptr a,unsigned long b,int c){if(!((
unsigned int)a.curr))(int)_throw((void*)& Cyc___poll_failure_1);{int len=(int)
_get_dyneither_size(a,sizeof(struct Cyc_pollfd));if(len < 1)(int)_throw((void*)&
Cyc___poll_failure_2);return poll((struct Cyc_pollfd*)_untag_dyneither_ptr(a,
sizeof(struct Cyc_pollfd),0),b < (unsigned long)len?b:(unsigned long)len,c);};}
typedef struct{unsigned long __val[1024 / (8 * sizeof(unsigned long))];}Cyc___sigset_t;
typedef Cyc___sigset_t Cyc_sigset_t;enum Cyc___anonymous_enum_182__{Cyc_SIGEV_SIGNAL
 = 0,Cyc_SIGEV_NONE  = 1,Cyc_SIGEV_THREAD  = 2};enum Cyc_SigBuiltin{Cyc_SIG_DFL  = 
0,Cyc_SIG_ERR  = 1,Cyc_SIG_IGN  = 2};struct Cyc___SIG_BUILTIN_struct{int tag;enum 
Cyc_SigBuiltin f1;};struct Cyc___SIG_HANDLER_struct{int tag;void(*f1)(int);};void*
Cyc_signal(int,struct _dyneither_ptr);void*Cyc_Csignal_make_SIG_DFL(){return(void*)({
struct Cyc___SIG_BUILTIN_struct*_tmpF=_cycalloc_atomic(sizeof(*_tmpF));_tmpF[0]=({
struct Cyc___SIG_BUILTIN_struct _tmp10;_tmp10.tag=0;_tmp10.f1=Cyc_SIG_DFL;_tmp10;});
_tmpF;});}void*Cyc_Csignal_make_SIG_ERR(){return(void*)({struct Cyc___SIG_BUILTIN_struct*
_tmp11=_cycalloc_atomic(sizeof(*_tmp11));_tmp11[0]=({struct Cyc___SIG_BUILTIN_struct
_tmp12;_tmp12.tag=0;_tmp12.f1=Cyc_SIG_ERR;_tmp12;});_tmp11;});}void*Cyc_Csignal_make_SIG_IGN(){
return(void*)({struct Cyc___SIG_BUILTIN_struct*_tmp13=_cycalloc_atomic(sizeof(*
_tmp13));_tmp13[0]=({struct Cyc___SIG_BUILTIN_struct _tmp14;_tmp14.tag=0;_tmp14.f1=
Cyc_SIG_ERR;_tmp14;});_tmp13;});}void*Cyc_Csignal_make_SIG_HANDLER(void(*f)(int)){
return(void*)({struct Cyc___SIG_HANDLER_struct*_tmp15=_cycalloc(sizeof(*_tmp15));
_tmp15[0]=({struct Cyc___SIG_HANDLER_struct _tmp16;_tmp16.tag=1;_tmp16.f1=f;_tmp16;});
_tmp15;});}void*signal_SIG_DFL(int);void*signal_SIG_ERR(int);void*signal_SIG_IGN(
int);void*signal(int,void(*)(int));static char _tmp18[49]="signal called with the wrong number of arguments";
static struct Cyc_Core_Failure_struct Cyc___signal_failure={Cyc_Core_Failure,{
_tmp18,_tmp18,_tmp18 + 49}};void*Cyc_signal(int a,struct _dyneither_ptr b){if(
_get_dyneither_size(b,sizeof(void*))!= 1)(int)_throw((void*)& Cyc___signal_failure);{
void*_tmp19=*((void**)_check_dyneither_subscript(b,sizeof(void*),0));enum Cyc_SigBuiltin
_tmp1B;enum Cyc_SigBuiltin _tmp1D;enum Cyc_SigBuiltin _tmp1F;void(*_tmp21)(int);
_LL6: {struct Cyc___SIG_BUILTIN_struct*_tmp1A=(struct Cyc___SIG_BUILTIN_struct*)
_tmp19;if(_tmp1A->tag != 0)goto _LL8;else{_tmp1B=_tmp1A->f1;if(_tmp1B != Cyc_SIG_DFL)
goto _LL8;}}_LL7: return signal_SIG_DFL(a);_LL8: {struct Cyc___SIG_BUILTIN_struct*
_tmp1C=(struct Cyc___SIG_BUILTIN_struct*)_tmp19;if(_tmp1C->tag != 0)goto _LLA;else{
_tmp1D=_tmp1C->f1;if(_tmp1D != Cyc_SIG_ERR)goto _LLA;}}_LL9: return signal_SIG_ERR(a);
_LLA: {struct Cyc___SIG_BUILTIN_struct*_tmp1E=(struct Cyc___SIG_BUILTIN_struct*)
_tmp19;if(_tmp1E->tag != 0)goto _LLC;else{_tmp1F=_tmp1E->f1;if(_tmp1F != Cyc_SIG_IGN)
goto _LLC;}}_LLB: return signal_SIG_IGN(a);_LLC: {struct Cyc___SIG_HANDLER_struct*
_tmp20=(struct Cyc___SIG_HANDLER_struct*)_tmp19;if(_tmp20->tag != 1)goto _LL5;else{
_tmp21=_tmp20->f1;}}_LLD: return signal(a,_tmp21);_LL5:;};}typedef struct{int
__count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef
struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;
struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc___cycFILE*Cyc_fromCfile(
struct Cyc_Cstdio___abstractFILE*cf);struct Cyc_String_pa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_struct{
int tag;double f1;};struct Cyc_LongDouble_pa_struct{int tag;long double f1;};struct
Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{int tag;
unsigned long*f1;};void Cyc_clearerr(struct Cyc___cycFILE*);int Cyc_fclose(struct
Cyc___cycFILE*);struct Cyc___cycFILE*Cyc_fdopen(int,const char*);int Cyc_feof(
struct Cyc___cycFILE*);int Cyc_ferror(struct Cyc___cycFILE*);int Cyc_fflush(struct
Cyc___cycFILE*);int Cyc_fgetc(struct Cyc___cycFILE*);int Cyc_fgetpos(struct Cyc___cycFILE*,
Cyc_fpos_t*);struct _dyneither_ptr Cyc_fgets(struct _dyneither_ptr,int,struct Cyc___cycFILE*);
int Cyc_fileno(struct Cyc___cycFILE*);void Cyc_flockfile(struct Cyc___cycFILE*);
struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);int Cyc_fputc(int,struct Cyc___cycFILE*);
int Cyc_fputs(const char*,struct Cyc___cycFILE*);unsigned int Cyc_fread(struct
_dyneither_ptr,unsigned int,unsigned int,struct Cyc___cycFILE*);struct Cyc___cycFILE*
Cyc_freopen(const char*,const char*,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
int Cyc_fseek(struct Cyc___cycFILE*,long,int);int Cyc_fsetpos(struct Cyc___cycFILE*,
const Cyc_fpos_t*);long Cyc_ftell(struct Cyc___cycFILE*);int Cyc_ftrylockfile(struct
Cyc___cycFILE*);void Cyc_funlockfile(struct Cyc___cycFILE*);unsigned int Cyc_fwrite(
struct _dyneither_ptr,unsigned int,unsigned int,struct Cyc___cycFILE*);int Cyc_getc(
struct Cyc___cycFILE*);int Cyc_getchar();int Cyc_getc_unlocked(struct Cyc___cycFILE*);
int Cyc_getchar_unlocked();int Cyc_pclose(struct Cyc___cycFILE*);struct Cyc___cycFILE*
Cyc_popen(const char*,const char*);int Cyc_putc(int,struct Cyc___cycFILE*);int Cyc_putc_unlocked(
int,struct Cyc___cycFILE*);void Cyc_rewind(struct Cyc___cycFILE*);int Cyc_setvbuf(
struct Cyc___cycFILE*,struct _dyneither_ptr,int,unsigned int);void Cyc_setbuf(
struct Cyc___cycFILE*,struct _dyneither_ptr);int Cyc_ungetc(int,struct Cyc___cycFILE*);
int Cyc_getw(struct Cyc___cycFILE*);int Cyc_putw(int,struct Cyc___cycFILE*);extern
char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{char*tag;};extern char
Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*tag;struct
_dyneither_ptr f1;};struct Cyc___cycFILE*Cyc_file_open(struct _dyneither_ptr,struct
_dyneither_ptr);void Cyc_file_close(struct Cyc___cycFILE*);struct Cyc_Cstdio___abstractFILE;
struct Cyc___cycFILE{struct Cyc_Cstdio___abstractFILE*file;};struct Cyc___cycFILE*
Cyc_fromCfile(struct Cyc_Cstdio___abstractFILE*cf){return(unsigned int)cf?({
struct Cyc___cycFILE*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->file=(struct Cyc_Cstdio___abstractFILE*)
cf;_tmp22;}): 0;}void clearerr(struct Cyc_Cstdio___abstractFILE*);void Cyc_clearerr(
struct Cyc___cycFILE*f){return clearerr(f->file);}int fclose(struct Cyc_Cstdio___abstractFILE*);
int Cyc_fclose(struct Cyc___cycFILE*f){return fclose(f->file);}struct Cyc_Cstdio___abstractFILE*
fdopen(int,const char*);struct Cyc___cycFILE*Cyc_fdopen(int i,const char*s){struct
Cyc_Cstdio___abstractFILE*_tmp23=fdopen(i,s);return(unsigned int)_tmp23?({struct
Cyc___cycFILE*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24->file=(struct Cyc_Cstdio___abstractFILE*)
_tmp23;_tmp24;}): 0;}int feof(struct Cyc_Cstdio___abstractFILE*);int Cyc_feof(struct
Cyc___cycFILE*f){return feof(f->file);}int ferror(struct Cyc_Cstdio___abstractFILE*);
int Cyc_ferror(struct Cyc___cycFILE*f){return ferror(f->file);}int fflush(struct Cyc_Cstdio___abstractFILE*);
int Cyc_fflush(struct Cyc___cycFILE*f){if((unsigned int)f)return fflush((struct Cyc_Cstdio___abstractFILE*)
f->file);else{return fflush(0);}}int fgetc(struct Cyc_Cstdio___abstractFILE*);int
Cyc_fgetc(struct Cyc___cycFILE*f){return fgetc(f->file);}int fgetpos(struct Cyc_Cstdio___abstractFILE*,
Cyc_fpos_t*);int Cyc_fgetpos(struct Cyc___cycFILE*f,Cyc_fpos_t*x){return fgetpos(f->file,
x);}char*fgets(char*,int,struct Cyc_Cstdio___abstractFILE*);static char _tmp26[30]="fgets called with NULL string";
static struct Cyc_Core_Failure_struct Cyc___fgets_failure_1={Cyc_Core_Failure,{
_tmp26,_tmp26,_tmp26 + 30}};static char _tmp28[27]="fgets: buffer insufficient";
static struct Cyc_Core_Failure_struct Cyc___fgets_failure_2={Cyc_Core_Failure,{
_tmp28,_tmp28,_tmp28 + 27}};struct _dyneither_ptr Cyc_fgets(struct _dyneither_ptr s,
int n,struct Cyc___cycFILE*f){if(!((unsigned int)s.curr))(int)_throw((void*)& Cyc___fgets_failure_1);{
char*result;char*buffer=(char*)_check_null(_untag_dyneither_ptr(s,sizeof(char),1));
unsigned int len=_get_dyneither_size(s,sizeof(char));if(len < n)(int)_throw((void*)&
Cyc___fgets_failure_2);result=fgets(buffer,n,f->file);if(result == 0)return
_tag_dyneither(0,0,0);else{return s;}};}int fileno(struct Cyc_Cstdio___abstractFILE*);
int Cyc_fileno(struct Cyc___cycFILE*f){return fileno(f->file);}void flockfile(struct
Cyc_Cstdio___abstractFILE*);void Cyc_flockfile(struct Cyc___cycFILE*f){return
flockfile(f->file);}struct Cyc_Cstdio___abstractFILE*fopen(const char*,const char*);
struct Cyc___cycFILE*Cyc_fopen(const char*name,const char*type){struct Cyc_Cstdio___abstractFILE*
_tmp29=fopen(name,type);return(unsigned int)_tmp29?({struct Cyc___cycFILE*_tmp2A=
_cycalloc(sizeof(*_tmp2A));_tmp2A->file=(struct Cyc_Cstdio___abstractFILE*)_tmp29;
_tmp2A;}): 0;}int fputc(int,struct Cyc_Cstdio___abstractFILE*);int Cyc_fputc(int x,
struct Cyc___cycFILE*f){return fputc(x,f->file);}int fputs(const char*,struct Cyc_Cstdio___abstractFILE*);
int Cyc_fputs(const char*x,struct Cyc___cycFILE*f){return fputs(x,f->file);}
unsigned int fread(char*,unsigned int,unsigned int,struct Cyc_Cstdio___abstractFILE*);
static char _tmp2C[27]="fread: buffer insufficient";static struct Cyc_Core_Failure_struct
Cyc___fread_failure={Cyc_Core_Failure,{_tmp2C,_tmp2C,_tmp2C + 27}};unsigned int
Cyc_fread(struct _dyneither_ptr ptr,unsigned int size,unsigned int nmemb,struct Cyc___cycFILE*
f){if(size * nmemb > _get_dyneither_size(ptr,sizeof(char)))(int)_throw((void*)& Cyc___fread_failure);
return fread((char*)_check_null(_untag_dyneither_ptr(ptr,sizeof(char),1)),size,
nmemb,f->file);}struct Cyc_Cstdio___abstractFILE*freopen(const char*,const char*,
struct Cyc_Cstdio___abstractFILE*);struct Cyc___cycFILE*Cyc_freopen(const char*
filename,const char*modes,struct Cyc___cycFILE*f){struct Cyc_Cstdio___abstractFILE*
_tmp2D=freopen(filename,modes,f->file);return(unsigned int)_tmp2D?(struct Cyc___cycFILE*)({
struct Cyc___cycFILE*_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E->file=(struct Cyc_Cstdio___abstractFILE*)
_tmp2D;_tmp2E;}): 0;}int fseek(struct Cyc_Cstdio___abstractFILE*,long,int);int Cyc_fseek(
struct Cyc___cycFILE*a,long b,int c){return fseek(a->file,b,c);}int fsetpos(struct Cyc_Cstdio___abstractFILE*,
const Cyc_fpos_t*);int Cyc_fsetpos(struct Cyc___cycFILE*f,const Cyc_fpos_t*b){return
fsetpos(f->file,b);}long ftell(struct Cyc_Cstdio___abstractFILE*);long Cyc_ftell(
struct Cyc___cycFILE*f){return ftell(f->file);}int ftrylockfile(struct Cyc_Cstdio___abstractFILE*);
int Cyc_ftrylockfile(struct Cyc___cycFILE*f){return ftrylockfile(f->file);}void
funlockfile(struct Cyc_Cstdio___abstractFILE*);void Cyc_funlockfile(struct Cyc___cycFILE*
f){return funlockfile(f->file);}unsigned int fwrite(const char*,unsigned int,
unsigned int,struct Cyc_Cstdio___abstractFILE*);static char _tmp30[31]="fwrite called with NULL string";
static struct Cyc_Core_Failure_struct Cyc___fwrite_failure_1={Cyc_Core_Failure,{
_tmp30,_tmp30,_tmp30 + 31}};static char _tmp32[28]="fwrite: buffer insufficient";
static struct Cyc_Core_Failure_struct Cyc___fwrite_failure_2={Cyc_Core_Failure,{
_tmp32,_tmp32,_tmp32 + 28}};unsigned int Cyc_fwrite(struct _dyneither_ptr ptr,
unsigned int size,unsigned int nmemb,struct Cyc___cycFILE*f){if(!((unsigned int)ptr.curr))(
int)_throw((void*)& Cyc___fwrite_failure_1);else{if(size * nmemb > 
_get_dyneither_size(ptr,sizeof(char)))(int)_throw((void*)& Cyc___fwrite_failure_2);
return fwrite((const char*)_untag_dyneither_ptr(ptr,sizeof(char),1),size,nmemb,f->file);}}
int getc(struct Cyc_Cstdio___abstractFILE*);int Cyc_getc(struct Cyc___cycFILE*f){
return getc(f->file);}int getchar();int Cyc_getchar(){return getchar();}int
getc_unlocked(struct Cyc_Cstdio___abstractFILE*);int Cyc_getc_unlocked(struct Cyc___cycFILE*
f){return getc_unlocked(f->file);}int __CYCLONE_GETCHAR_UNLOCKED();int Cyc_getchar_unlocked(){
return __CYCLONE_GETCHAR_UNLOCKED();}int pclose(struct Cyc_Cstdio___abstractFILE*);
int Cyc_pclose(struct Cyc___cycFILE*f){return pclose(f->file);}struct Cyc_Cstdio___abstractFILE*
popen(const char*,const char*);struct Cyc___cycFILE*Cyc_popen(const char*name,const
char*type){struct Cyc_Cstdio___abstractFILE*_tmp33=popen(name,type);return(
unsigned int)_tmp33?({struct Cyc___cycFILE*_tmp34=_cycalloc(sizeof(*_tmp34));
_tmp34->file=(struct Cyc_Cstdio___abstractFILE*)_tmp33;_tmp34;}): 0;}int putc(int,
struct Cyc_Cstdio___abstractFILE*);int Cyc_putc(int x,struct Cyc___cycFILE*f){return
putc(x,f->file);}int putc_unlocked(int,struct Cyc_Cstdio___abstractFILE*);int Cyc_putc_unlocked(
int x,struct Cyc___cycFILE*f){return putc_unlocked(x,f->file);}void rewind(struct Cyc_Cstdio___abstractFILE*);
void Cyc_rewind(struct Cyc___cycFILE*f){return rewind(f->file);}int setvbuf(struct
Cyc_Cstdio___abstractFILE*,char*,int,unsigned int);static char _tmp36[24]="setvbuf: buffer is NULL";
static struct Cyc_Core_Failure_struct Cyc___setvbuf_failure_1={Cyc_Core_Failure,{
_tmp36,_tmp36,_tmp36 + 24}};static char _tmp38[29]="setvbuf: buffer insufficient";
static struct Cyc_Core_Failure_struct Cyc___setvbuf_failure_2={Cyc_Core_Failure,{
_tmp38,_tmp38,_tmp38 + 29}};int Cyc_setvbuf(struct Cyc___cycFILE*f,struct
_dyneither_ptr buf,int type,unsigned int size){if(!((unsigned int)buf.curr))(int)
_throw((void*)& Cyc___setvbuf_failure_1);if(_get_dyneither_size(buf,sizeof(char))
< size)(int)_throw((void*)& Cyc___setvbuf_failure_2);return setvbuf(f->file,(char*)
_check_null(_untag_dyneither_ptr(buf,sizeof(char),1)),type,size);}void Cyc_setbuf(
struct Cyc___cycFILE*f,struct _dyneither_ptr buf){Cyc_setvbuf(f,buf,(unsigned int)
buf.curr?0: 2,8192);return;}int ungetc(int,struct Cyc_Cstdio___abstractFILE*);int
Cyc_ungetc(int x,struct Cyc___cycFILE*f){return ungetc(x,f->file);}int getw(struct
Cyc_Cstdio___abstractFILE*);int Cyc_getw(struct Cyc___cycFILE*f){return getw(f->file);}
int putw(int,struct Cyc_Cstdio___abstractFILE*);int Cyc_putw(int x,struct Cyc___cycFILE*
f){return putw(x,f->file);}char Cyc_FileCloseError[15]="FileCloseError";char Cyc_FileOpenError[
14]="FileOpenError";struct Cyc___cycFILE*Cyc_file_open(struct _dyneither_ptr fname,
struct _dyneither_ptr mode){struct Cyc___cycFILE*f=Cyc_fopen((const char*)
_untag_dyneither_ptr(fname,sizeof(char),1),(const char*)_untag_dyneither_ptr(mode,
sizeof(char),1));if(f == 0){struct _dyneither_ptr fn=({unsigned int _tmp3B=
_get_dyneither_size(fname,sizeof(char));char*_tmp3C=(char*)_cycalloc_atomic(
_check_times(sizeof(char),_tmp3B + 1));struct _dyneither_ptr _tmp3E=_tag_dyneither(
_tmp3C,sizeof(char),_tmp3B + 1);{unsigned int _tmp3D=_tmp3B;unsigned int i;for(i=0;
i < _tmp3D;i ++){_tmp3C[i]=((const char*)fname.curr)[(int)i];}_tmp3C[_tmp3D]=(char)
0;}_tmp3E;});(int)_throw((void*)({struct Cyc_FileOpenError_struct*_tmp39=
_cycalloc(sizeof(*_tmp39));_tmp39[0]=({struct Cyc_FileOpenError_struct _tmp3A;
_tmp3A.tag=Cyc_FileOpenError;_tmp3A.f1=fn;_tmp3A;});_tmp39;}));}return(struct Cyc___cycFILE*)
f;}struct Cyc_FileCloseError_struct Cyc_FileCloseError_val={Cyc_FileCloseError};
void Cyc_file_close(struct Cyc___cycFILE*f){if(Cyc_fclose(f)!= 0)(int)_throw((void*)&
Cyc_FileCloseError_val);}typedef struct{int quot;int rem;}Cyc_div_t;typedef struct{
long quot;long rem;}Cyc_ldiv_t;void*abort();void exit(int);void Cyc_free(struct
_dyneither_ptr);void Cyc_qsort(struct _dyneither_ptr,unsigned int,unsigned int,int(*)(
const void*,const void*));void Cyc_free(struct _dyneither_ptr ptr){;}void qsort(void*,
unsigned int,unsigned int,int(*)(const void*,const void*));static char _tmp41[23]="qsort called with NULL";
static struct Cyc_Core_Failure_struct Cyc___qsort_failure_1={Cyc_Core_Failure,{
_tmp41,_tmp41,_tmp41 + 23}};static char _tmp43[42]="qsort called with insufficient array size";
static struct Cyc_Core_Failure_struct Cyc___qsort_failure_2={Cyc_Core_Failure,{
_tmp43,_tmp43,_tmp43 + 42}};void Cyc_qsort(struct _dyneither_ptr tab,unsigned int
nmemb,unsigned int szmemb,int(*compar)(const void*,const void*)){if(!((unsigned int)
tab.curr))(int)_throw((void*)& Cyc___qsort_failure_1);if(_get_dyneither_size(tab,
sizeof(void))< nmemb)(int)_throw((void*)& Cyc___qsort_failure_2);qsort((void*)
_untag_dyneither_ptr(tab,sizeof(void),1),nmemb,(unsigned int)szmemb,compar);}
struct Cyc_ipc_perm{int __key;unsigned int uid;unsigned int gid;unsigned int cuid;
unsigned int cgid;unsigned short mode;unsigned short __pad1;unsigned short __seq;
unsigned short __pad2;unsigned long __unused1;unsigned long __unused2;};struct
_dyneither_ptr Cyc_mmap(struct _dyneither_ptr,unsigned int length,int prot,int flags,
int fd,long offset);int Cyc_munmap(struct _dyneither_ptr start,unsigned int length);
char*__stub_mmap(char*start,unsigned int length,int prot,int flags,int fd,long offset);
int munmap(const char*start,unsigned int length);static char _tmp45[42]="mmap called with illegal start/prot/flags";
static struct Cyc_Core_Failure_struct Cyc___mmap_failure={Cyc_Core_Failure,{_tmp45,
_tmp45,_tmp45 + 42}};struct _dyneither_ptr Cyc_mmap(struct _dyneither_ptr ignored,
unsigned int length,int prot,int flags,int fd,long offset){if((ignored.curr == (
_tag_dyneither(0,0,0)).curr  && prot == 1) && (flags == 2  || flags == 1))return(
struct _dyneither_ptr)wrap_Cbuffer_as_buffer(__stub_mmap(0,length,prot,flags,fd,
offset),length);else{(int)_throw((void*)& Cyc___mmap_failure);}}int Cyc_munmap(
struct _dyneither_ptr start,unsigned int length){return munmap((const char*)
_check_null(_untag_dyneither_ptr(start,sizeof(char),1)),length);}struct Cyc_timeval{
long tv_sec;long tv_usec;};struct Cyc_rlimit{unsigned long rlim_cur;unsigned long
rlim_max;};enum Cyc___rusage_who{Cyc_RUSAGE_SELF  = 0,Cyc_RUSAGE_CHILDREN  = -1,
Cyc_RUSAGE_BOTH  = -2};struct Cyc_rusage{struct Cyc_timeval ru_utime;struct Cyc_timeval
ru_stime;long ru_maxrss;long ru_ixrss;long ru_idrss;long ru_isrss;long ru_minflt;long
ru_majflt;long ru_nswap;long ru_inblock;long ru_oublock;long ru_msgsnd;long ru_msgrcv;
long ru_nsignals;long ru_nvcsw;long ru_nivcsw;};enum Cyc___priority_which{Cyc_PRIO_PROCESS
 = 0,Cyc_PRIO_PGRP  = 1,Cyc_PRIO_USER  = 2};struct Cyc_timespec{long tv_sec;long
tv_nsec;};typedef struct{long __fds_bits[1024 / (8 * sizeof(long))];}Cyc_fd_set;int
Cyc_select(int a,Cyc_fd_set*b,Cyc_fd_set*c,Cyc_fd_set*d,struct Cyc_timeval*e);void
Cyc_FD_CLR(int,Cyc_fd_set*);int Cyc_FD_ISSET(int,Cyc_fd_set*);void Cyc_FD_SET(int,
Cyc_fd_set*);void Cyc_FD_ZERO(Cyc_fd_set*);int select(int,Cyc_fd_set*,Cyc_fd_set*,
Cyc_fd_set*,struct Cyc_timeval*);extern int Cyc_select(int a,Cyc_fd_set*b,Cyc_fd_set*
c,Cyc_fd_set*d,struct Cyc_timeval*e){if(a < 0  || a > 1024){*((int*)_check_null(
__CYCLONE_ERRNO()))=22;return - 1;}return select(a,b,c,d,e);}void __stub_FD_CLR(int,
Cyc_fd_set*);int __stub_FD_ISSET(int,Cyc_fd_set*);void __stub_FD_SET(int,Cyc_fd_set*);
void __stub_FD_ZERO(Cyc_fd_set*);void Cyc_FD_CLR(int a,Cyc_fd_set*b){if(a < 0  || a > 
1024)return;return __stub_FD_CLR(a,b);}int Cyc_FD_ISSET(int a,Cyc_fd_set*b){if(a < 0
 || a > 1024)return 0;return __stub_FD_ISSET(a,b);}void Cyc_FD_SET(int a,Cyc_fd_set*b){
if(a < 0  || a > 1024)return;return __stub_FD_SET(a,b);}void Cyc_FD_ZERO(Cyc_fd_set*a){
return __stub_FD_ZERO(a);}enum Cyc___socket_type{Cyc_SOCK_STREAM  = 1,Cyc_SOCK_DGRAM
 = 2,Cyc_SOCK_RAW  = 3,Cyc_SOCK_RDM  = 4,Cyc_SOCK_SEQPACKET  = 5,Cyc_SOCK_PACKET  = 
10};enum Cyc___anonymous_enum_271__{Cyc_MSG_OOB  = 1,Cyc_MSG_PEEK  = 2,Cyc_MSG_DONTROUTE
 = 4,Cyc_MSG_CTRUNC  = 8,Cyc_MSG_PROXY  = 16,Cyc_MSG_TRUNC  = 32,Cyc_MSG_DONTWAIT
 = 64,Cyc_MSG_EOR  = 128,Cyc_MSG_WAITALL  = 256,Cyc_MSG_FIN  = 512,Cyc_MSG_SYN  = 
1024,Cyc_MSG_CONFIRM  = 2048,Cyc_MSG_RST  = 4096,Cyc_MSG_ERRQUEUE  = 8192,Cyc_MSG_NOSIGNAL
 = 16384,Cyc_MSG_MORE  = 32768};struct Cyc_cmsghdr{unsigned int cmsg_len;int
cmsg_level;int cmsg_type;unsigned char __cmsg_data[0];};enum Cyc___anonymous_enum_272__{
Cyc_SCM_RIGHTS  = 1,Cyc_SCM_CREDENTIALS  = 2,Cyc___SCM_CONNECT  = 3};struct Cyc_linger{
int l_onoff;int l_linger;};enum Cyc___anonymous_enum_273__{Cyc_SHUT_RD  = 0,Cyc_SHUT_WR
 = 1,Cyc_SHUT_RDWR  = 2};struct Cyc_SA_sockaddr_in_struct{int tag;struct Cyc_sockaddr_in*
f1;};struct Cyc_SA_socklenptr_struct{int tag;unsigned int*f1;};struct Cyc_SA_socklen_struct{
int tag;unsigned int f1;};struct Cyc_SO_int_struct{int tag;int*f1;};struct Cyc_SO_timeval_struct{
int tag;struct Cyc_timeval*f1;};struct Cyc_SO_socklenptr_struct{int tag;unsigned int*
f1;};struct Cyc_SO_socklen_struct{int tag;unsigned int f1;};int Cyc_accept(int fd,
struct _dyneither_ptr);extern char Cyc_SocketError[12];struct Cyc_SocketError_struct{
char*tag;};int Cyc_bind(int fd,struct _dyneither_ptr);int Cyc_connect(int fd,struct
_dyneither_ptr);int Cyc_getpeername(int fd,struct _dyneither_ptr);int Cyc_getsockname(
int fd,struct _dyneither_ptr);int Cyc_getsockopt(int fd,int level,int optname,struct
_dyneither_ptr);int Cyc_recv(int fd,struct _dyneither_ptr buf,unsigned int n,int flags);
int Cyc_recvfrom(int fd,struct _dyneither_ptr buf,unsigned int n,int flags,struct
_dyneither_ptr);int Cyc_send(int fd,struct _dyneither_ptr buf,unsigned int n,int flags);
int Cyc_sendto(int fd,struct _dyneither_ptr buf,unsigned int n,int flags,struct
_dyneither_ptr);int Cyc_setsockopt(int fd,int level,int optname,struct _dyneither_ptr);
char Cyc_SocketError[12]="SocketError";struct Cyc_SocketError_struct Cyc_SocketError_val={
Cyc_SocketError};int accept_in(int,const struct Cyc_sockaddr_in*,unsigned int*);
struct _tuple0{void*f1;void*f2;};int Cyc_accept(int fd,struct _dyneither_ptr ap){if(
_get_dyneither_size(ap,sizeof(void*))!= 2)(int)_throw((void*)& Cyc_SocketError_val);{
struct _tuple0 _tmp48=({struct _tuple0 _tmp47;_tmp47.f1=*((void**)
_check_dyneither_subscript(ap,sizeof(void*),0));_tmp47.f2=*((void**)
_check_dyneither_subscript(ap,sizeof(void*),1));_tmp47;});void*_tmp49;struct Cyc_sockaddr_in*
_tmp4B;void*_tmp4C;unsigned int*_tmp4E;_LLF: _tmp49=_tmp48.f1;{struct Cyc_SA_sockaddr_in_struct*
_tmp4A=(struct Cyc_SA_sockaddr_in_struct*)_tmp49;if(_tmp4A->tag != 0)goto _LL11;
else{_tmp4B=_tmp4A->f1;}};_tmp4C=_tmp48.f2;{struct Cyc_SA_socklenptr_struct*
_tmp4D=(struct Cyc_SA_socklenptr_struct*)_tmp4C;if(_tmp4D->tag != 1)goto _LL11;
else{_tmp4E=_tmp4D->f1;}};_LL10: return accept_in(fd,(const struct Cyc_sockaddr_in*)
_tmp4B,_tmp4E);_LL11:;_LL12:(int)_throw((void*)& Cyc_SocketError_val);_LLE:;};}
int bind_in(int,const struct Cyc_sockaddr_in*,unsigned int);int Cyc_bind(int fd,
struct _dyneither_ptr ap){if(_get_dyneither_size(ap,sizeof(void*))!= 2)(int)_throw((
void*)& Cyc_SocketError_val);{struct _tuple0 _tmp50=({struct _tuple0 _tmp4F;_tmp4F.f1=*((
void**)_check_dyneither_subscript(ap,sizeof(void*),0));_tmp4F.f2=*((void**)
_check_dyneither_subscript(ap,sizeof(void*),1));_tmp4F;});void*_tmp51;struct Cyc_sockaddr_in*
_tmp53;void*_tmp54;unsigned int _tmp56;_LL14: _tmp51=_tmp50.f1;{struct Cyc_SA_sockaddr_in_struct*
_tmp52=(struct Cyc_SA_sockaddr_in_struct*)_tmp51;if(_tmp52->tag != 0)goto _LL16;
else{_tmp53=_tmp52->f1;}};_tmp54=_tmp50.f2;{struct Cyc_SA_socklen_struct*_tmp55=(
struct Cyc_SA_socklen_struct*)_tmp54;if(_tmp55->tag != 2)goto _LL16;else{_tmp56=
_tmp55->f1;}};_LL15: return bind_in(fd,(const struct Cyc_sockaddr_in*)_tmp53,_tmp56);
_LL16:;_LL17:(int)_throw((void*)& Cyc_SocketError_val);_LL13:;};}int connect_in(
int,const struct Cyc_sockaddr_in*,unsigned int);int Cyc_connect(int fd,struct
_dyneither_ptr ap){if(_get_dyneither_size(ap,sizeof(void*))!= 2)(int)_throw((void*)&
Cyc_SocketError_val);{struct _tuple0 _tmp58=({struct _tuple0 _tmp57;_tmp57.f1=*((
void**)_check_dyneither_subscript(ap,sizeof(void*),0));_tmp57.f2=*((void**)
_check_dyneither_subscript(ap,sizeof(void*),1));_tmp57;});void*_tmp59;struct Cyc_sockaddr_in*
_tmp5B;void*_tmp5C;unsigned int _tmp5E;_LL19: _tmp59=_tmp58.f1;{struct Cyc_SA_sockaddr_in_struct*
_tmp5A=(struct Cyc_SA_sockaddr_in_struct*)_tmp59;if(_tmp5A->tag != 0)goto _LL1B;
else{_tmp5B=_tmp5A->f1;}};_tmp5C=_tmp58.f2;{struct Cyc_SA_socklen_struct*_tmp5D=(
struct Cyc_SA_socklen_struct*)_tmp5C;if(_tmp5D->tag != 2)goto _LL1B;else{_tmp5E=
_tmp5D->f1;}};_LL1A: return connect_in(fd,(const struct Cyc_sockaddr_in*)_tmp5B,
_tmp5E);_LL1B:;_LL1C:(int)_throw((void*)& Cyc_SocketError_val);_LL18:;};}int
getpeername_in(int,const struct Cyc_sockaddr_in*,unsigned int*);int Cyc_getpeername(
int fd,struct _dyneither_ptr ap){if(_get_dyneither_size(ap,sizeof(void*))!= 2)(int)
_throw((void*)& Cyc_SocketError_val);{struct _tuple0 _tmp60=({struct _tuple0 _tmp5F;
_tmp5F.f1=*((void**)_check_dyneither_subscript(ap,sizeof(void*),0));_tmp5F.f2=*((
void**)_check_dyneither_subscript(ap,sizeof(void*),1));_tmp5F;});void*_tmp61;
struct Cyc_sockaddr_in*_tmp63;void*_tmp64;unsigned int*_tmp66;_LL1E: _tmp61=_tmp60.f1;{
struct Cyc_SA_sockaddr_in_struct*_tmp62=(struct Cyc_SA_sockaddr_in_struct*)_tmp61;
if(_tmp62->tag != 0)goto _LL20;else{_tmp63=_tmp62->f1;}};_tmp64=_tmp60.f2;{struct
Cyc_SA_socklenptr_struct*_tmp65=(struct Cyc_SA_socklenptr_struct*)_tmp64;if(
_tmp65->tag != 1)goto _LL20;else{_tmp66=_tmp65->f1;}};_LL1F: return getpeername_in(
fd,(const struct Cyc_sockaddr_in*)_tmp63,_tmp66);_LL20:;_LL21:(int)_throw((void*)&
Cyc_SocketError_val);_LL1D:;};}int getsockname_in(int,const struct Cyc_sockaddr_in*,
unsigned int*);int Cyc_getsockname(int fd,struct _dyneither_ptr ap){if(
_get_dyneither_size(ap,sizeof(void*))!= 2)(int)_throw((void*)& Cyc_SocketError_val);{
struct _tuple0 _tmp68=({struct _tuple0 _tmp67;_tmp67.f1=*((void**)
_check_dyneither_subscript(ap,sizeof(void*),0));_tmp67.f2=*((void**)
_check_dyneither_subscript(ap,sizeof(void*),1));_tmp67;});void*_tmp69;struct Cyc_sockaddr_in*
_tmp6B;void*_tmp6C;unsigned int*_tmp6E;_LL23: _tmp69=_tmp68.f1;{struct Cyc_SA_sockaddr_in_struct*
_tmp6A=(struct Cyc_SA_sockaddr_in_struct*)_tmp69;if(_tmp6A->tag != 0)goto _LL25;
else{_tmp6B=_tmp6A->f1;}};_tmp6C=_tmp68.f2;{struct Cyc_SA_socklenptr_struct*
_tmp6D=(struct Cyc_SA_socklenptr_struct*)_tmp6C;if(_tmp6D->tag != 1)goto _LL25;
else{_tmp6E=_tmp6D->f1;}};_LL24: return getsockname_in(fd,(const struct Cyc_sockaddr_in*)
_tmp6B,_tmp6E);_LL25:;_LL26:(int)_throw((void*)& Cyc_SocketError_val);_LL22:;};}
int getsockopt_int(int,int,int,int*,unsigned int*);int getsockopt_timeval(int,int,
int,struct Cyc_timeval*,unsigned int*);int Cyc_getsockopt(int fd,int level,int
optname,struct _dyneither_ptr ap){if(_get_dyneither_size(ap,sizeof(void*))!= 2)(
int)_throw((void*)& Cyc_SocketError_val);{struct _tuple0 _tmp70=({struct _tuple0
_tmp6F;_tmp6F.f1=*((void**)_check_dyneither_subscript(ap,sizeof(void*),0));
_tmp6F.f2=*((void**)_check_dyneither_subscript(ap,sizeof(void*),1));_tmp6F;});
void*_tmp71;int*_tmp73;void*_tmp74;unsigned int*_tmp76;void*_tmp77;int*_tmp79;
void*_tmp7A;int*_tmp7C;void*_tmp7D;struct Cyc_timeval*_tmp7F;void*_tmp80;
unsigned int*_tmp82;void*_tmp83;struct Cyc_timeval*_tmp85;void*_tmp86;int*_tmp88;
_LL28: _tmp71=_tmp70.f1;{struct Cyc_SO_int_struct*_tmp72=(struct Cyc_SO_int_struct*)
_tmp71;if(_tmp72->tag != 0)goto _LL2A;else{_tmp73=_tmp72->f1;}};_tmp74=_tmp70.f2;{
struct Cyc_SO_socklenptr_struct*_tmp75=(struct Cyc_SO_socklenptr_struct*)_tmp74;
if(_tmp75->tag != 2)goto _LL2A;else{_tmp76=_tmp75->f1;}};_LL29: return
getsockopt_int(fd,level,optname,_tmp73,_tmp76);_LL2A: _tmp77=_tmp70.f1;{struct Cyc_SO_int_struct*
_tmp78=(struct Cyc_SO_int_struct*)_tmp77;if(_tmp78->tag != 0)goto _LL2C;else{_tmp79=
_tmp78->f1;}};_tmp7A=_tmp70.f2;{struct Cyc_SO_int_struct*_tmp7B=(struct Cyc_SO_int_struct*)
_tmp7A;if(_tmp7B->tag != 0)goto _LL2C;else{_tmp7C=_tmp7B->f1;}};_LL2B: return
getsockopt_int(fd,level,optname,_tmp79,(unsigned int*)_tmp7C);_LL2C: _tmp7D=
_tmp70.f1;{struct Cyc_SO_timeval_struct*_tmp7E=(struct Cyc_SO_timeval_struct*)
_tmp7D;if(_tmp7E->tag != 1)goto _LL2E;else{_tmp7F=_tmp7E->f1;}};_tmp80=_tmp70.f2;{
struct Cyc_SO_socklenptr_struct*_tmp81=(struct Cyc_SO_socklenptr_struct*)_tmp80;
if(_tmp81->tag != 2)goto _LL2E;else{_tmp82=_tmp81->f1;}};_LL2D: return
getsockopt_timeval(fd,level,optname,_tmp7F,_tmp82);_LL2E: _tmp83=_tmp70.f1;{
struct Cyc_SO_timeval_struct*_tmp84=(struct Cyc_SO_timeval_struct*)_tmp83;if(
_tmp84->tag != 1)goto _LL30;else{_tmp85=_tmp84->f1;}};_tmp86=_tmp70.f2;{struct Cyc_SO_int_struct*
_tmp87=(struct Cyc_SO_int_struct*)_tmp86;if(_tmp87->tag != 0)goto _LL30;else{_tmp88=
_tmp87->f1;}};_LL2F: return getsockopt_timeval(fd,level,optname,_tmp85,(
unsigned int*)_tmp88);_LL30:;_LL31:(int)_throw((void*)& Cyc_SocketError_val);
_LL27:;};}int recv_wrapped(int,struct _dyneither_ptr,unsigned int,int);int Cyc_recv(
int fd,struct _dyneither_ptr buf,unsigned int n,int flags){return recv_wrapped(fd,buf,
n,flags);}int recvfrom_in(int,struct _dyneither_ptr,unsigned int,int,const struct
Cyc_sockaddr_in*,unsigned int*);int Cyc_recvfrom(int fd,struct _dyneither_ptr buf,
unsigned int n,int flags,struct _dyneither_ptr ap){if(_get_dyneither_size(ap,sizeof(
void*))!= 2)(int)_throw((void*)& Cyc_SocketError_val);{struct _tuple0 _tmp8A=({
struct _tuple0 _tmp89;_tmp89.f1=*((void**)_check_dyneither_subscript(ap,sizeof(
void*),0));_tmp89.f2=*((void**)_check_dyneither_subscript(ap,sizeof(void*),1));
_tmp89;});void*_tmp8B;struct Cyc_sockaddr_in*_tmp8D;void*_tmp8E;unsigned int*
_tmp90;_LL33: _tmp8B=_tmp8A.f1;{struct Cyc_SA_sockaddr_in_struct*_tmp8C=(struct Cyc_SA_sockaddr_in_struct*)
_tmp8B;if(_tmp8C->tag != 0)goto _LL35;else{_tmp8D=_tmp8C->f1;}};_tmp8E=_tmp8A.f2;{
struct Cyc_SA_socklenptr_struct*_tmp8F=(struct Cyc_SA_socklenptr_struct*)_tmp8E;
if(_tmp8F->tag != 1)goto _LL35;else{_tmp90=_tmp8F->f1;}};_LL34: return recvfrom_in(
fd,buf,n,flags,(const struct Cyc_sockaddr_in*)_tmp8D,_tmp90);_LL35:;_LL36:(int)
_throw((void*)& Cyc_SocketError_val);_LL32:;};}int send_wrapped(int,struct
_dyneither_ptr,unsigned int,int);int Cyc_send(int fd,struct _dyneither_ptr buf,
unsigned int n,int flags){return send_wrapped(fd,buf,n,flags);}int sendto_in(int,
struct _dyneither_ptr,unsigned int,int,const struct Cyc_sockaddr_in*,unsigned int);
int Cyc_sendto(int fd,struct _dyneither_ptr buf,unsigned int n,int flags,struct
_dyneither_ptr ap){if(_get_dyneither_size(ap,sizeof(void*))!= 2)(int)_throw((void*)&
Cyc_SocketError_val);{struct _tuple0 _tmp92=({struct _tuple0 _tmp91;_tmp91.f1=*((
void**)_check_dyneither_subscript(ap,sizeof(void*),0));_tmp91.f2=*((void**)
_check_dyneither_subscript(ap,sizeof(void*),1));_tmp91;});void*_tmp93;struct Cyc_sockaddr_in*
_tmp95;void*_tmp96;unsigned int _tmp98;_LL38: _tmp93=_tmp92.f1;{struct Cyc_SA_sockaddr_in_struct*
_tmp94=(struct Cyc_SA_sockaddr_in_struct*)_tmp93;if(_tmp94->tag != 0)goto _LL3A;
else{_tmp95=_tmp94->f1;}};_tmp96=_tmp92.f2;{struct Cyc_SA_socklen_struct*_tmp97=(
struct Cyc_SA_socklen_struct*)_tmp96;if(_tmp97->tag != 2)goto _LL3A;else{_tmp98=
_tmp97->f1;}};_LL39: return sendto_in(fd,buf,n,flags,(const struct Cyc_sockaddr_in*)
_tmp95,_tmp98);_LL3A:;_LL3B:(int)_throw((void*)& Cyc_SocketError_val);_LL37:;};}
int setsockopt_int(int,int,int,const int*,unsigned int);int setsockopt_timeval(int,
int,int,const struct Cyc_timeval*,unsigned int);int Cyc_setsockopt(int fd,int level,
int optname,struct _dyneither_ptr ap){if(_get_dyneither_size(ap,sizeof(void*))!= 2)(
int)_throw((void*)& Cyc_SocketError_val);{struct _tuple0 _tmp9A=({struct _tuple0
_tmp99;_tmp99.f1=*((void**)_check_dyneither_subscript(ap,sizeof(void*),0));
_tmp99.f2=*((void**)_check_dyneither_subscript(ap,sizeof(void*),1));_tmp99;});
void*_tmp9B;int*_tmp9D;void*_tmp9E;unsigned int _tmpA0;void*_tmpA1;struct Cyc_timeval*
_tmpA3;void*_tmpA4;unsigned int _tmpA6;_LL3D: _tmp9B=_tmp9A.f1;{struct Cyc_SO_int_struct*
_tmp9C=(struct Cyc_SO_int_struct*)_tmp9B;if(_tmp9C->tag != 0)goto _LL3F;else{_tmp9D=
_tmp9C->f1;}};_tmp9E=_tmp9A.f2;{struct Cyc_SO_socklen_struct*_tmp9F=(struct Cyc_SO_socklen_struct*)
_tmp9E;if(_tmp9F->tag != 3)goto _LL3F;else{_tmpA0=_tmp9F->f1;}};_LL3E: return
setsockopt_int(fd,level,optname,(const int*)_tmp9D,_tmpA0);_LL3F: _tmpA1=_tmp9A.f1;{
struct Cyc_SO_timeval_struct*_tmpA2=(struct Cyc_SO_timeval_struct*)_tmpA1;if(
_tmpA2->tag != 1)goto _LL41;else{_tmpA3=_tmpA2->f1;}};_tmpA4=_tmp9A.f2;{struct Cyc_SO_socklen_struct*
_tmpA5=(struct Cyc_SO_socklen_struct*)_tmpA4;if(_tmpA5->tag != 3)goto _LL41;else{
_tmpA6=_tmpA5->f1;}};_LL40: return setsockopt_timeval(fd,level,optname,(const
struct Cyc_timeval*)_tmpA3,_tmpA6);_LL41:;_LL42:(int)_throw((void*)& Cyc_SocketError_val);
_LL3C:;};}struct Cyc_stat{unsigned long long st_dev;unsigned short __pad1;
unsigned long st_ino;unsigned int st_mode;unsigned int st_nlink;unsigned int st_uid;
unsigned int st_gid;unsigned long long st_rdev;unsigned short __pad2;long st_size;
long st_blksize;long st_blocks;long st_atime;unsigned long __unused1;long st_mtime;
unsigned long __unused2;long st_ctime;unsigned long __unused3;unsigned long
__unused4;unsigned long __unused5;};struct Cyc_timezone{int tz_minuteswest;int
tz_dsttime;};enum Cyc___itimer_which{Cyc_ITIMER_REAL  = 0,Cyc_ITIMER_VIRTUAL  = 1,
Cyc_ITIMER_PROF  = 2};struct Cyc_itimerval{struct Cyc_timeval it_interval;struct Cyc_timeval
it_value;};int Cyc_gettimeofday(struct Cyc_timeval*,struct Cyc_timezone*);int Cyc_select(
int,Cyc_fd_set*,Cyc_fd_set*,Cyc_fd_set*,struct Cyc_timeval*);void Cyc_FD_CLR(int,
Cyc_fd_set*);int Cyc_FD_ISSET(int,Cyc_fd_set*);void Cyc_FD_SET(int,Cyc_fd_set*);
void Cyc_FD_ZERO(Cyc_fd_set*);int gettimeofday(struct Cyc_timeval*,struct Cyc_timezone*);
int Cyc_gettimeofday(struct Cyc_timeval*a,struct Cyc_timezone*b){return gettimeofday(
a,0);}struct Cyc_timeb{long time;unsigned short millitm;short timezone;short dstflag;
};struct Cyc_tms{long tms_utime;long tms_stime;long tms_cutime;long tms_cstime;};
struct Cyc_sockaddr_un{unsigned short sun_family;char sun_path[108];};struct Cyc_utsname{
char sysname[65];char nodename[65];char release[65];char version[65];char machine[65];
char __domainname[65];};typedef enum {Cyc_P_ALL  = 0,Cyc_P_PID  = 1,Cyc_P_PGID  = 2 }
Cyc_idtype_t;struct Cyc_tm{int tm_sec;int tm_min;int tm_hour;int tm_mday;int tm_mon;
int tm_year;int tm_wday;int tm_yday;int tm_isdst;long tm_gmtoff;const char*tm_zone;};
struct Cyc_itimerspec{struct Cyc_timespec it_interval;struct Cyc_timespec it_value;};
unsigned int Cyc_strftime(struct _dyneither_ptr,unsigned int,struct _dyneither_ptr,
const struct Cyc_tm*);unsigned int strftime(char*,unsigned int,const char*,const
struct Cyc_tm*);static char _tmpA8[27]="fgets: buffer insufficient";static struct Cyc_Core_Failure_struct
Cyc___strftime_failure={Cyc_Core_Failure,{_tmpA8,_tmpA8,_tmpA8 + 27}};
unsigned int Cyc_strftime(struct _dyneither_ptr s,unsigned int maxsize,struct
_dyneither_ptr fmt,const struct Cyc_tm*t){if(maxsize > _get_dyneither_size(s,sizeof(
char)))(int)_throw((void*)& Cyc___strftime_failure);return strftime((char*)
_check_null(_untag_dyneither_ptr(s,sizeof(char),1)),maxsize,(const char*)
_check_null(_untag_dyneither_ptr(fmt,sizeof(char),1)),t);}enum Cyc___anonymous_enum_368__{
Cyc__PC_LINK_MAX  = 0,Cyc__PC_MAX_CANON  = 1,Cyc__PC_MAX_INPUT  = 2,Cyc__PC_NAME_MAX
 = 3,Cyc__PC_PATH_MAX  = 4,Cyc__PC_PIPE_BUF  = 5,Cyc__PC_CHOWN_RESTRICTED  = 6,Cyc__PC_NO_TRUNC
 = 7,Cyc__PC_VDISABLE  = 8,Cyc__PC_SYNC_IO  = 9,Cyc__PC_ASYNC_IO  = 10,Cyc__PC_PRIO_IO
 = 11,Cyc__PC_SOCK_MAXBUF  = 12,Cyc__PC_FILESIZEBITS  = 13,Cyc__PC_REC_INCR_XFER_SIZE
 = 14,Cyc__PC_REC_MAX_XFER_SIZE  = 15,Cyc__PC_REC_MIN_XFER_SIZE  = 16,Cyc__PC_REC_XFER_ALIGN
 = 17,Cyc__PC_ALLOC_SIZE_MIN  = 18,Cyc__PC_SYMLINK_MAX  = 19};enum Cyc___anonymous_enum_370__{
Cyc__CS_PATH  = 0,Cyc__CS_V6_WIDTH_RESTRICTED_ENVS  = 1};struct Cyc_option{struct
_dyneither_ptr name;int has_arg;int*flag;int val;};int Cyc_execlp(const char*,const
char*,struct _dyneither_ptr);int execvp(const char*file,const char**argv);struct
_dyneither_ptr Cyc_getcwd(struct _dyneither_ptr buf,unsigned int size);int Cyc_gethostname(
struct _dyneither_ptr,unsigned int);int Cyc_read(int,struct _dyneither_ptr,
unsigned int);int Cyc_write(int,struct _dyneither_ptr,unsigned int);static char
_tmpAA[36]="execlp arg list not NULL-terminated";static struct Cyc_Core_Failure_struct
Cyc___execlp_failure={Cyc_Core_Failure,{_tmpAA,_tmpAA,_tmpAA + 36}};int Cyc_execlp(
const char*path,const char*arg0,struct _dyneither_ptr argv){if(*((const char**)
_check_dyneither_subscript(argv,sizeof(const char*),(int)(_get_dyneither_size(
argv,sizeof(const char*))- 1)))!= (const char*)0)(int)_throw((void*)& Cyc___execlp_failure);{
const char**newargs=(const char**)_untag_dyneither_ptr(({unsigned int _tmpB3=
_get_dyneither_size(argv,sizeof(const char*));const char**_tmpB4=(const char**)
_cycalloc(_check_times(sizeof(const char*),_tmpB3 + 1));struct _dyneither_ptr _tmpB6=
_tag_dyneither(_tmpB4,sizeof(const char*),_tmpB3 + 1);{unsigned int _tmpB5=_tmpB3;
unsigned int i;for(i=0;i < _tmpB5;i ++){_tmpB4[i]=(const char*)0;}_tmpB4[_tmpB5]=(
const char*)0;}_tmpB6;}),sizeof(const char*),1);({struct _dyneither_ptr _tmpAC=
_dyneither_ptr_plus(({const char**_tmpAB=newargs;_tag_dyneither(_tmpAB,sizeof(
const char*),_get_zero_arr_size_voidstar(_tmpAB,1));}),sizeof(const char*),0);
const char*_tmpAD=*((const char**)_check_dyneither_subscript(_tmpAC,sizeof(const
char*),0));const char*_tmpAE=(const char*)arg0;if(_get_dyneither_size(_tmpAC,
sizeof(const char*))== 1  && (_tmpAD == 0  && _tmpAE != 0))_throw_arraybounds();*((
const char**)_tmpAC.curr)=_tmpAE;});{int i=0;for(0;i < _get_dyneither_size(argv,
sizeof(const char*))- 1;++ i){({struct _dyneither_ptr _tmpB0=_dyneither_ptr_plus(({
const char**_tmpAF=newargs;_tag_dyneither(_tmpAF,sizeof(const char*),
_get_zero_arr_size_voidstar(_tmpAF,1));}),sizeof(const char*),i + 1);const char*
_tmpB1=*((const char**)_check_dyneither_subscript(_tmpB0,sizeof(const char*),0));
const char*_tmpB2=*((const char**)_check_dyneither_subscript(argv,sizeof(const char*),
i));if(_get_dyneither_size(_tmpB0,sizeof(const char*))== 1  && (_tmpB1 == 0  && 
_tmpB2 != 0))_throw_arraybounds();*((const char**)_tmpB0.curr)=_tmpB2;});}}return
execvp(path,(const char**)newargs);};}char*getcwd(char*buf,unsigned int size);
static char _tmpB8[29]="getcwd: invalid buf argument";static struct Cyc_Core_Failure_struct
Cyc___getcwd_failure={Cyc_Core_Failure,{_tmpB8,_tmpB8,_tmpB8 + 29}};struct
_dyneither_ptr Cyc_getcwd(struct _dyneither_ptr buf,unsigned int size){if(
_get_dyneither_size(buf,sizeof(char))< size)(int)_throw((void*)& Cyc___getcwd_failure);{
char*_tmpB9=getcwd((char*)_check_null(_untag_dyneither_ptr(buf,sizeof(char),1)),
size);return(unsigned int)_tmpB9?buf: _tag_dyneither(0,0,0);};}int gethostname(
char*,unsigned int);static char _tmpBB[34]="gethostname: called with NULL buf";
static struct Cyc_Core_Failure_struct Cyc___gethostname_failure_1={Cyc_Core_Failure,{
_tmpBB,_tmpBB,_tmpBB + 34}};static char _tmpBD[46]="gethostname: called with count > numelts(buf)";
static struct Cyc_Core_Failure_struct Cyc___gethostname_failure_2={Cyc_Core_Failure,{
_tmpBD,_tmpBD,_tmpBD + 46}};int Cyc_gethostname(struct _dyneither_ptr buf,
unsigned int count){if(!((unsigned int)buf.curr))(int)_throw((void*)& Cyc___gethostname_failure_1);
else{if(count > _get_dyneither_size(buf,sizeof(char)))(int)_throw((void*)& Cyc___gethostname_failure_2);
return gethostname((char*)_untag_dyneither_ptr(buf,sizeof(char),1),count);}}int
read(int fd,char*buf,unsigned int count);static char _tmpBF[39]="read: called with count > numelts(buf)";
static struct Cyc_Core_Failure_struct Cyc___read_failure={Cyc_Core_Failure,{_tmpBF,
_tmpBF,_tmpBF + 39}};int Cyc_read(int fd,struct _dyneither_ptr buf,unsigned int count){
if(count > _get_dyneither_size(buf,sizeof(char)))(int)_throw((void*)& Cyc___read_failure);
return read(fd,(char*)_check_null(_untag_dyneither_ptr(buf,sizeof(char),1)),count);}
int write(int fd,const char*buf,unsigned int count);static char _tmpC1[40]="write: called with count > numelts(buf)";
static struct Cyc_Core_Failure_struct Cyc___write_failure={Cyc_Core_Failure,{_tmpC1,
_tmpC1,_tmpC1 + 40}};int Cyc_write(int fd,struct _dyneither_ptr buf,unsigned int count){
if(count > _get_dyneither_size(buf,sizeof(char)))(int)_throw((void*)& Cyc___write_failure);
return write(fd,(const char*)_untag_dyneither_ptr(buf,sizeof(char),1),count);}
struct Cyc_utimbuf{long actime;long modtime;};
