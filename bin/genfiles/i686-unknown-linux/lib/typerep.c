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
extern struct _xtunion_struct ADD_PREFIX(Null_Exception_struct);
extern struct _xtunion_struct * ADD_PREFIX(Null_Exception);
extern struct _xtunion_struct ADD_PREFIX(Array_bounds_struct);
extern struct _xtunion_struct * ADD_PREFIX(Array_bounds);
extern struct _xtunion_struct ADD_PREFIX(Match_Exception_struct);
extern struct _xtunion_struct * ADD_PREFIX(Match_Exception);
extern struct _xtunion_struct ADD_PREFIX(Bad_alloc_struct);
extern struct _xtunion_struct * ADD_PREFIX(Bad_alloc);

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
 struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};struct Cyc_Core_Opt{
void*v;};extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_struct{
char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_struct{
char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_struct{
char*tag;};struct Cyc_Typerep_Int_struct{int tag;int f1;unsigned int f2;};struct Cyc_Typerep_Float_struct{
int tag;};struct Cyc_Typerep_Double_struct{int tag;};struct Cyc_Typerep_ThinPtr_struct{
int tag;unsigned int f1;void*f2;};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;}
;struct _tuple0{unsigned int f1;struct _dyneither_ptr f2;void*f3;};struct Cyc_Typerep_Struct_struct{
int tag;struct _dyneither_ptr*f1;unsigned int f2;struct _dyneither_ptr f3;};struct
_tuple1{unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;
unsigned int f1;struct _dyneither_ptr f2;};struct _tuple2{unsigned int f1;struct
_dyneither_ptr f2;};struct Cyc_Typerep_TUnion_struct{int tag;struct _dyneither_ptr f1;
struct _dyneither_ptr f2;struct _dyneither_ptr f3;};struct Cyc_Typerep_TUnionField_struct{
int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;unsigned int f3;struct
_dyneither_ptr f4;};struct _tuple3{struct _dyneither_ptr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;};struct Cyc_Typerep_Union_struct{
int tag;struct _dyneither_ptr*f1;int f2;struct _dyneither_ptr f3;};struct Cyc_Typerep_Enum_struct{
int tag;struct _dyneither_ptr*f1;int f2;struct _dyneither_ptr f3;};void Cyc_Typerep_print_typestruct(
void*rep);void Cyc_Typerep_xmlize_typestruct(void*rep);struct _tuple4{int f1;void*
f2;};struct _tuple4 Cyc_Typerep_get_unionbranch(unsigned int tag,struct
_dyneither_ptr l);struct _dyneither_ptr Cyc_Typerep_get_tagname(unsigned int tag,
struct _dyneither_ptr l);struct _dyneither_ptr Cyc_Typerep_get_tagname2(unsigned int
tag,struct _dyneither_ptr l);struct _tuple4 Cyc_Typerep_get_xtunionbranch(struct
_dyneither_ptr tag,struct _dyneither_ptr l);unsigned int Cyc_Typerep_size_type(void*
rep);typedef struct{int __count;union{unsigned int __wch;char __wchb[4];}__value;}
Cyc___mbstate_t;typedef struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;
typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct
Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;}
;struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dyneither_ptr f1;};int Cyc_printf(struct _dyneither_ptr,struct
_dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_struct{
char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_struct{char*
tag;struct _dyneither_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;
};extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_struct{char*
tag;};extern char Cyc_List_Nth[4];struct Cyc_List_Nth_struct{char*tag;};struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_struct{
char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_struct{char*tag;};
struct _tuple5{void*f1;void*f2;};struct _tuple5*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple5*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);int Cyc_strcmp(struct _dyneither_ptr s1,struct
_dyneither_ptr s2);struct _tuple4 Cyc_Typerep_tuple_tl(void*typ){void*_tmp0=typ;
unsigned int _tmp2;struct _dyneither_ptr _tmp3;_LL1: {struct Cyc_Typerep_Tuple_struct*
_tmp1=(struct Cyc_Typerep_Tuple_struct*)_tmp0;if(_tmp1->tag != 6)goto _LL3;else{
_tmp2=_tmp1->f1;_tmp3=_tmp1->f2;}}_LL2: if(_get_dyneither_size(_tmp3,sizeof(
struct _tuple1*))< 2)goto _LL4;{int pos1=(int)(*(*((struct _tuple1**)
_check_dyneither_subscript(_tmp3,sizeof(struct _tuple1*),1)))).f1;struct
_dyneither_ptr _tmp4=({unsigned int _tmp8=_get_dyneither_size(_tmp3,sizeof(struct
_tuple1*))- 1;struct _tuple1**_tmp9=(struct _tuple1**)_cycalloc(_check_times(
sizeof(struct _tuple1*),_tmp8));struct _dyneither_ptr _tmpC=_tag_dyneither(_tmp9,
sizeof(struct _tuple1*),_tmp8);{unsigned int _tmpA=_tmp8;unsigned int i;for(i=0;i < 
_tmpA;i ++){_tmp9[i]=({struct _tuple1*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->f1=(*(*((
struct _tuple1**)_check_dyneither_subscript(_tmp3,sizeof(struct _tuple1*),(int)(i + 
1))))).f1 - pos1;_tmpB->f2=(*(*((struct _tuple1**)_check_dyneither_subscript(_tmp3,
sizeof(struct _tuple1*),(int)(i + 1))))).f2;_tmpB;});}}_tmpC;});return({struct
_tuple4 _tmp5;_tmp5.f1=pos1;_tmp5.f2=(void*)({struct Cyc_Typerep_Tuple_struct*
_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6[0]=({struct Cyc_Typerep_Tuple_struct _tmp7;
_tmp7.tag=6;_tmp7.f1=_tmp2 - pos1;_tmp7.f2=_tmp4;_tmp7;});_tmp6;});_tmp5;});}_LL3:;
_LL4:(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmpD=_cycalloc(sizeof(*
_tmpD));_tmpD[0]=({struct Cyc_Core_Failure_struct _tmpE;_tmpE.tag=Cyc_Core_Failure;
_tmpE.f1=({const char*_tmpF="tuple_tl: expected tuple of size >= 2";
_tag_dyneither(_tmpF,sizeof(char),38);});_tmpE;});_tmpD;}));_LL0:;}struct _tuple4
Cyc_Typerep_get_unionbranch(unsigned int tag,struct _dyneither_ptr l){{int i=0;for(0;
i < _get_dyneither_size(l,sizeof(struct _tuple0*));++ i){unsigned int _tmp11;void*
_tmp12;struct _tuple0 _tmp10=*((struct _tuple0**)l.curr)[i];_tmp11=_tmp10.f1;_tmp12=
_tmp10.f3;if(_tmp11 == tag)return Cyc_Typerep_tuple_tl(_tmp12);}}(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13[0]=({
struct Cyc_Core_Failure_struct _tmp14;_tmp14.tag=Cyc_Core_Failure;_tmp14.f1=({
const char*_tmp15="Could not find tag in TUnion";_tag_dyneither(_tmp15,sizeof(
char),29);});_tmp14;});_tmp13;}));}struct _dyneither_ptr Cyc_Typerep_get_tagname(
unsigned int tag,struct _dyneither_ptr l){{int i=0;for(0;i < _get_dyneither_size(l,
sizeof(struct _tuple2*));++ i){unsigned int _tmp17;struct _dyneither_ptr _tmp18;
struct _tuple2 _tmp16=*((struct _tuple2**)l.curr)[i];_tmp17=_tmp16.f1;_tmp18=_tmp16.f2;
if(_tmp17 == tag)return _tmp18;}}(int)_throw((void*)({struct Cyc_Core_Failure_struct*
_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19[0]=({struct Cyc_Core_Failure_struct
_tmp1A;_tmp1A.tag=Cyc_Core_Failure;_tmp1A.f1=({const char*_tmp1B="Could not find name in TUnion";
_tag_dyneither(_tmp1B,sizeof(char),30);});_tmp1A;});_tmp19;}));}struct
_dyneither_ptr Cyc_Typerep_get_tagname2(unsigned int tag,struct _dyneither_ptr l){{
int i=0;for(0;i < _get_dyneither_size(l,sizeof(struct _tuple0*));++ i){unsigned int
_tmp1D;struct _dyneither_ptr _tmp1E;struct _tuple0 _tmp1C=*((struct _tuple0**)l.curr)[
i];_tmp1D=_tmp1C.f1;_tmp1E=_tmp1C.f2;if(_tmp1D == tag)return _tmp1E;}}(int)_throw((
void*)({struct Cyc_Core_Failure_struct*_tmp1F=_cycalloc(sizeof(*_tmp1F));_tmp1F[0]=({
struct Cyc_Core_Failure_struct _tmp20;_tmp20.tag=Cyc_Core_Failure;_tmp20.f1=({
const char*_tmp21="Could not find name in TUnion";_tag_dyneither(_tmp21,sizeof(
char),30);});_tmp20;});_tmp1F;}));}struct _tuple4 Cyc_Typerep_get_xtunionbranch(
struct _dyneither_ptr tag,struct _dyneither_ptr l){{int i=0;for(0;i < 
_get_dyneither_size(l,sizeof(struct _tuple3*));++ i){struct _dyneither_ptr _tmp23;
void*_tmp24;struct _tuple3 _tmp22=*((struct _tuple3**)l.curr)[i];_tmp23=_tmp22.f1;
_tmp24=_tmp22.f2;if(Cyc_strcmp((struct _dyneither_ptr)_tmp23,(struct
_dyneither_ptr)tag)== 0)return Cyc_Typerep_tuple_tl(_tmp24);}}(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmp25=_cycalloc(sizeof(*_tmp25));_tmp25[0]=({
struct Cyc_Core_Failure_struct _tmp26;_tmp26.tag=Cyc_Core_Failure;_tmp26.f1=({
const char*_tmp27="Could not find tag in XTUnion";_tag_dyneither(_tmp27,sizeof(
char),30);});_tmp26;});_tmp25;}));}unsigned int Cyc_Typerep_size_type(void*rep){
void*_tmp28=rep;int _tmp2A;unsigned int _tmp2B;unsigned int _tmp31;unsigned int
_tmp33;unsigned int _tmp35;int _tmp39;int _tmp3B;struct _dyneither_ptr _tmp3C;_LL6: {
struct Cyc_Typerep_Int_struct*_tmp29=(struct Cyc_Typerep_Int_struct*)_tmp28;if(
_tmp29->tag != 0)goto _LL8;else{_tmp2A=_tmp29->f1;_tmp2B=_tmp29->f2;}}_LL7: return
_tmp2B >> 3;_LL8: {struct Cyc_Typerep_Float_struct*_tmp2C=(struct Cyc_Typerep_Float_struct*)
_tmp28;if(_tmp2C->tag != 1)goto _LLA;}_LL9: return sizeof(float);_LLA: {struct Cyc_Typerep_Double_struct*
_tmp2D=(struct Cyc_Typerep_Double_struct*)_tmp28;if(_tmp2D->tag != 2)goto _LLC;}
_LLB: return sizeof(double);_LLC: {struct Cyc_Typerep_ThinPtr_struct*_tmp2E=(struct
Cyc_Typerep_ThinPtr_struct*)_tmp28;if(_tmp2E->tag != 3)goto _LLE;}_LLD: return
sizeof(void*);_LLE: {struct Cyc_Typerep_FatPtr_struct*_tmp2F=(struct Cyc_Typerep_FatPtr_struct*)
_tmp28;if(_tmp2F->tag != 4)goto _LL10;}_LLF: return sizeof(struct _dyneither_ptr);
_LL10: {struct Cyc_Typerep_Struct_struct*_tmp30=(struct Cyc_Typerep_Struct_struct*)
_tmp28;if(_tmp30->tag != 5)goto _LL12;else{_tmp31=_tmp30->f2;}}_LL11: return _tmp31;
_LL12: {struct Cyc_Typerep_Tuple_struct*_tmp32=(struct Cyc_Typerep_Tuple_struct*)
_tmp28;if(_tmp32->tag != 6)goto _LL14;else{_tmp33=_tmp32->f1;}}_LL13: return _tmp33;
_LL14: {struct Cyc_Typerep_TUnionField_struct*_tmp34=(struct Cyc_Typerep_TUnionField_struct*)
_tmp28;if(_tmp34->tag != 8)goto _LL16;else{_tmp35=_tmp34->f3;}}_LL15: return _tmp35;
_LL16: {struct Cyc_Typerep_TUnion_struct*_tmp36=(struct Cyc_Typerep_TUnion_struct*)
_tmp28;if(_tmp36->tag != 7)goto _LL18;}_LL17: return sizeof(void*);_LL18: {struct Cyc_Typerep_XTUnion_struct*
_tmp37=(struct Cyc_Typerep_XTUnion_struct*)_tmp28;if(_tmp37->tag != 9)goto _LL1A;}
_LL19: return sizeof(void*);_LL1A: {struct Cyc_Typerep_Union_struct*_tmp38=(struct
Cyc_Typerep_Union_struct*)_tmp28;if(_tmp38->tag != 10)goto _LL1C;else{_tmp39=
_tmp38->f2;}}_LL1B: return(unsigned int)_tmp39;_LL1C: {struct Cyc_Typerep_Enum_struct*
_tmp3A=(struct Cyc_Typerep_Enum_struct*)_tmp28;if(_tmp3A->tag != 11)goto _LL5;else{
_tmp3B=_tmp3A->f2;_tmp3C=_tmp3A->f3;}}_LL1D: return(unsigned int)_tmp3B;_LL5:;}
struct _dyneither_ptr Cyc_Typerep_anon_or_name(struct _dyneither_ptr*name){return
name == 0?({const char*_tmp3D="(anon)";_tag_dyneither(_tmp3D,sizeof(char),7);}):*
name;}void Cyc_Typerep_print_typestruct(void*rep){void*_tmp3E=rep;int _tmp40;
unsigned int _tmp41;unsigned int _tmp45;void*_tmp46;void*_tmp48;struct
_dyneither_ptr*_tmp4A;unsigned int _tmp4B;struct _dyneither_ptr _tmp4C;unsigned int
_tmp4E;struct _dyneither_ptr _tmp4F;struct _dyneither_ptr _tmp51;struct
_dyneither_ptr _tmp52;unsigned int _tmp53;struct _dyneither_ptr _tmp54;struct
_dyneither_ptr _tmp56;struct _dyneither_ptr _tmp57;struct _dyneither_ptr _tmp58;
struct _dyneither_ptr _tmp5A;struct _dyneither_ptr _tmp5B;struct _dyneither_ptr*
_tmp5D;int _tmp5E;struct _dyneither_ptr _tmp5F;struct _dyneither_ptr*_tmp61;int
_tmp62;struct _dyneither_ptr _tmp63;_LL1F: {struct Cyc_Typerep_Int_struct*_tmp3F=(
struct Cyc_Typerep_Int_struct*)_tmp3E;if(_tmp3F->tag != 0)goto _LL21;else{_tmp40=
_tmp3F->f1;_tmp41=_tmp3F->f2;}}_LL20:({struct Cyc_Int_pa_struct _tmp67;_tmp67.tag=
1;_tmp67.f1=(unsigned long)((int)_tmp41);{struct Cyc_String_pa_struct _tmp66;
_tmp66.tag=0;_tmp66.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(_tmp40?(
struct _dyneither_ptr)({const char*_tmp68="signed";_tag_dyneither(_tmp68,sizeof(
char),7);}):(struct _dyneither_ptr)({const char*_tmp69="unsigned";_tag_dyneither(
_tmp69,sizeof(char),9);})));{void*_tmp64[2]={& _tmp66,& _tmp67};Cyc_printf(({const
char*_tmp65="Int(%s,%d)";_tag_dyneither(_tmp65,sizeof(char),11);}),
_tag_dyneither(_tmp64,sizeof(void*),2));}}});goto _LL1E;_LL21: {struct Cyc_Typerep_Float_struct*
_tmp42=(struct Cyc_Typerep_Float_struct*)_tmp3E;if(_tmp42->tag != 1)goto _LL23;}
_LL22:({void*_tmp6A=0;Cyc_printf(({const char*_tmp6B="Float";_tag_dyneither(
_tmp6B,sizeof(char),6);}),_tag_dyneither(_tmp6A,sizeof(void*),0));});goto _LL1E;
_LL23: {struct Cyc_Typerep_Double_struct*_tmp43=(struct Cyc_Typerep_Double_struct*)
_tmp3E;if(_tmp43->tag != 2)goto _LL25;}_LL24:({void*_tmp6C=0;Cyc_printf(({const
char*_tmp6D="Double";_tag_dyneither(_tmp6D,sizeof(char),7);}),_tag_dyneither(
_tmp6C,sizeof(void*),0));});goto _LL1E;_LL25: {struct Cyc_Typerep_ThinPtr_struct*
_tmp44=(struct Cyc_Typerep_ThinPtr_struct*)_tmp3E;if(_tmp44->tag != 3)goto _LL27;
else{_tmp45=_tmp44->f1;_tmp46=(void*)_tmp44->f2;}}_LL26:({struct Cyc_Int_pa_struct
_tmp70;_tmp70.tag=1;_tmp70.f1=(unsigned long)((int)_tmp45);{void*_tmp6E[1]={&
_tmp70};Cyc_printf(({const char*_tmp6F="ThinPtr(%d,";_tag_dyneither(_tmp6F,
sizeof(char),12);}),_tag_dyneither(_tmp6E,sizeof(void*),1));}});Cyc_Typerep_print_typestruct(
_tmp46);({void*_tmp71=0;Cyc_printf(({const char*_tmp72=")";_tag_dyneither(_tmp72,
sizeof(char),2);}),_tag_dyneither(_tmp71,sizeof(void*),0));});goto _LL1E;_LL27: {
struct Cyc_Typerep_FatPtr_struct*_tmp47=(struct Cyc_Typerep_FatPtr_struct*)_tmp3E;
if(_tmp47->tag != 4)goto _LL29;else{_tmp48=(void*)_tmp47->f1;}}_LL28:({void*_tmp73=
0;Cyc_printf(({const char*_tmp74="FatPtr(";_tag_dyneither(_tmp74,sizeof(char),8);}),
_tag_dyneither(_tmp73,sizeof(void*),0));});Cyc_Typerep_print_typestruct(_tmp48);({
void*_tmp75=0;Cyc_printf(({const char*_tmp76=")";_tag_dyneither(_tmp76,sizeof(
char),2);}),_tag_dyneither(_tmp75,sizeof(void*),0));});goto _LL1E;_LL29: {struct
Cyc_Typerep_Struct_struct*_tmp49=(struct Cyc_Typerep_Struct_struct*)_tmp3E;if(
_tmp49->tag != 5)goto _LL2B;else{_tmp4A=_tmp49->f1;_tmp4B=_tmp49->f2;_tmp4C=_tmp49->f3;}}
_LL2A:({struct Cyc_Int_pa_struct _tmp7A;_tmp7A.tag=1;_tmp7A.f1=(unsigned long)((
int)_tmp4B);{struct Cyc_String_pa_struct _tmp79;_tmp79.tag=0;_tmp79.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Typerep_anon_or_name(_tmp4A));{void*
_tmp77[2]={& _tmp79,& _tmp7A};Cyc_printf(({const char*_tmp78="Struct(%s,%d,[";
_tag_dyneither(_tmp78,sizeof(char),15);}),_tag_dyneither(_tmp77,sizeof(void*),2));}}});{
int i=0;for(0;i < _get_dyneither_size(_tmp4C,sizeof(struct _tuple0*));++ i){
unsigned int _tmp7C;struct _dyneither_ptr _tmp7D;void*_tmp7E;struct _tuple0 _tmp7B=*((
struct _tuple0**)_tmp4C.curr)[i];_tmp7C=_tmp7B.f1;_tmp7D=_tmp7B.f2;_tmp7E=_tmp7B.f3;({
struct Cyc_String_pa_struct _tmp82;_tmp82.tag=0;_tmp82.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp7D);{struct Cyc_Int_pa_struct _tmp81;_tmp81.tag=1;_tmp81.f1=(
unsigned long)((int)_tmp7C);{void*_tmp7F[2]={& _tmp81,& _tmp82};Cyc_printf(({const
char*_tmp80="$(%d,%s,";_tag_dyneither(_tmp80,sizeof(char),9);}),_tag_dyneither(
_tmp7F,sizeof(void*),2));}}});Cyc_Typerep_print_typestruct(_tmp7E);({struct Cyc_Int_pa_struct
_tmp85;_tmp85.tag=1;_tmp85.f1=(unsigned long)((int)(i != _get_dyneither_size(
_tmp4C,sizeof(struct _tuple0*))- 1?',':' '));{void*_tmp83[1]={& _tmp85};Cyc_printf(({
const char*_tmp84=")%c";_tag_dyneither(_tmp84,sizeof(char),4);}),_tag_dyneither(
_tmp83,sizeof(void*),1));}});}}({void*_tmp86=0;Cyc_printf(({const char*_tmp87=" ]";
_tag_dyneither(_tmp87,sizeof(char),3);}),_tag_dyneither(_tmp86,sizeof(void*),0));});
goto _LL1E;_LL2B: {struct Cyc_Typerep_Tuple_struct*_tmp4D=(struct Cyc_Typerep_Tuple_struct*)
_tmp3E;if(_tmp4D->tag != 6)goto _LL2D;else{_tmp4E=_tmp4D->f1;_tmp4F=_tmp4D->f2;}}
_LL2C:({struct Cyc_Int_pa_struct _tmp8A;_tmp8A.tag=1;_tmp8A.f1=(unsigned long)((
int)_tmp4E);{void*_tmp88[1]={& _tmp8A};Cyc_printf(({const char*_tmp89="Tuple(%d,[";
_tag_dyneither(_tmp89,sizeof(char),11);}),_tag_dyneither(_tmp88,sizeof(void*),1));}});{
int i=0;for(0;i < _get_dyneither_size(_tmp4F,sizeof(struct _tuple1*));++ i){
unsigned int _tmp8C;void*_tmp8D;struct _tuple1 _tmp8B=*((struct _tuple1**)_tmp4F.curr)[
i];_tmp8C=_tmp8B.f1;_tmp8D=_tmp8B.f2;({struct Cyc_Int_pa_struct _tmp90;_tmp90.tag=
1;_tmp90.f1=(unsigned long)((int)_tmp8C);{void*_tmp8E[1]={& _tmp90};Cyc_printf(({
const char*_tmp8F="$(%d,";_tag_dyneither(_tmp8F,sizeof(char),6);}),_tag_dyneither(
_tmp8E,sizeof(void*),1));}});Cyc_Typerep_print_typestruct(_tmp8D);({struct Cyc_Int_pa_struct
_tmp93;_tmp93.tag=1;_tmp93.f1=(unsigned long)((int)(i != _get_dyneither_size(
_tmp4F,sizeof(struct _tuple1*))- 1?',':' '));{void*_tmp91[1]={& _tmp93};Cyc_printf(({
const char*_tmp92=")%c";_tag_dyneither(_tmp92,sizeof(char),4);}),_tag_dyneither(
_tmp91,sizeof(void*),1));}});}}({void*_tmp94=0;Cyc_printf(({const char*_tmp95=" ]";
_tag_dyneither(_tmp95,sizeof(char),3);}),_tag_dyneither(_tmp94,sizeof(void*),0));});
goto _LL1E;_LL2D: {struct Cyc_Typerep_TUnionField_struct*_tmp50=(struct Cyc_Typerep_TUnionField_struct*)
_tmp3E;if(_tmp50->tag != 8)goto _LL2F;else{_tmp51=_tmp50->f1;_tmp52=_tmp50->f2;
_tmp53=_tmp50->f3;_tmp54=_tmp50->f4;}}_LL2E:({struct Cyc_Int_pa_struct _tmp9A;
_tmp9A.tag=1;_tmp9A.f1=(unsigned long)((int)_tmp53);{struct Cyc_String_pa_struct
_tmp99;_tmp99.tag=0;_tmp99.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp52);{struct Cyc_String_pa_struct _tmp98;_tmp98.tag=0;_tmp98.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmp51);{void*_tmp96[3]={& _tmp98,& _tmp99,&
_tmp9A};Cyc_printf(({const char*_tmp97="TUnionField(%s,%s,%d,[";_tag_dyneither(
_tmp97,sizeof(char),23);}),_tag_dyneither(_tmp96,sizeof(void*),3));}}}});{int i=0;
for(0;i < _get_dyneither_size(_tmp54,sizeof(struct _tuple1*));++ i){unsigned int
_tmp9C;void*_tmp9D;struct _tuple1 _tmp9B=*((struct _tuple1**)_tmp54.curr)[i];_tmp9C=
_tmp9B.f1;_tmp9D=_tmp9B.f2;({struct Cyc_Int_pa_struct _tmpA0;_tmpA0.tag=1;_tmpA0.f1=(
unsigned long)((int)_tmp9C);{void*_tmp9E[1]={& _tmpA0};Cyc_printf(({const char*
_tmp9F="$(%d,";_tag_dyneither(_tmp9F,sizeof(char),6);}),_tag_dyneither(_tmp9E,
sizeof(void*),1));}});Cyc_Typerep_print_typestruct(_tmp9D);({struct Cyc_Int_pa_struct
_tmpA3;_tmpA3.tag=1;_tmpA3.f1=(unsigned long)((int)(i != _get_dyneither_size(
_tmp54,sizeof(struct _tuple1*))- 1?',':' '));{void*_tmpA1[1]={& _tmpA3};Cyc_printf(({
const char*_tmpA2=")%c";_tag_dyneither(_tmpA2,sizeof(char),4);}),_tag_dyneither(
_tmpA1,sizeof(void*),1));}});}}({void*_tmpA4=0;Cyc_printf(({const char*_tmpA5=" ]";
_tag_dyneither(_tmpA5,sizeof(char),3);}),_tag_dyneither(_tmpA4,sizeof(void*),0));});
goto _LL1E;_LL2F: {struct Cyc_Typerep_TUnion_struct*_tmp55=(struct Cyc_Typerep_TUnion_struct*)
_tmp3E;if(_tmp55->tag != 7)goto _LL31;else{_tmp56=_tmp55->f1;_tmp57=_tmp55->f2;
_tmp58=_tmp55->f3;}}_LL30:({struct Cyc_String_pa_struct _tmpA8;_tmpA8.tag=0;_tmpA8.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp56);{void*_tmpA6[1]={& _tmpA8};
Cyc_printf(({const char*_tmpA7="TUnion(%s,[";_tag_dyneither(_tmpA7,sizeof(char),
12);}),_tag_dyneither(_tmpA6,sizeof(void*),1));}});{int i=0;for(0;i < 
_get_dyneither_size(_tmp57,sizeof(struct _tuple2*));++ i){unsigned int _tmpAA;
struct _dyneither_ptr _tmpAB;struct _tuple2 _tmpA9=*((struct _tuple2**)_tmp57.curr)[i];
_tmpAA=_tmpA9.f1;_tmpAB=_tmpA9.f2;({struct Cyc_Int_pa_struct _tmpB0;_tmpB0.tag=1;
_tmpB0.f1=(unsigned long)((int)(i != _get_dyneither_size(_tmp57,sizeof(struct
_tuple2*))- 1?',':' '));{struct Cyc_String_pa_struct _tmpAF;_tmpAF.tag=0;_tmpAF.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmpAB);{struct Cyc_Int_pa_struct
_tmpAE;_tmpAE.tag=1;_tmpAE.f1=(unsigned long)((int)_tmpAA);{void*_tmpAC[3]={&
_tmpAE,& _tmpAF,& _tmpB0};Cyc_printf(({const char*_tmpAD="$(%d,%s)%c";
_tag_dyneither(_tmpAD,sizeof(char),11);}),_tag_dyneither(_tmpAC,sizeof(void*),3));}}}});}}({
void*_tmpB1=0;Cyc_printf(({const char*_tmpB2="],[";_tag_dyneither(_tmpB2,sizeof(
char),4);}),_tag_dyneither(_tmpB1,sizeof(void*),0));});{int i=0;for(0;i < 
_get_dyneither_size(_tmp58,sizeof(struct _tuple0*));++ i){unsigned int _tmpB4;
struct _dyneither_ptr _tmpB5;void*_tmpB6;struct _tuple0 _tmpB3=*((struct _tuple0**)
_tmp58.curr)[i];_tmpB4=_tmpB3.f1;_tmpB5=_tmpB3.f2;_tmpB6=_tmpB3.f3;({struct Cyc_String_pa_struct
_tmpBA;_tmpBA.tag=0;_tmpBA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmpB5);{struct Cyc_Int_pa_struct _tmpB9;_tmpB9.tag=1;_tmpB9.f1=(unsigned long)((
int)_tmpB4);{void*_tmpB7[2]={& _tmpB9,& _tmpBA};Cyc_printf(({const char*_tmpB8="$(%d,%s,";
_tag_dyneither(_tmpB8,sizeof(char),9);}),_tag_dyneither(_tmpB7,sizeof(void*),2));}}});
Cyc_Typerep_print_typestruct(_tmpB6);({struct Cyc_Int_pa_struct _tmpBD;_tmpBD.tag=
1;_tmpBD.f1=(unsigned long)((int)(i != _get_dyneither_size(_tmp58,sizeof(struct
_tuple0*))- 1?',':' '));{void*_tmpBB[1]={& _tmpBD};Cyc_printf(({const char*_tmpBC=")%c";
_tag_dyneither(_tmpBC,sizeof(char),4);}),_tag_dyneither(_tmpBB,sizeof(void*),1));}});}}({
void*_tmpBE=0;Cyc_printf(({const char*_tmpBF="] )";_tag_dyneither(_tmpBF,sizeof(
char),4);}),_tag_dyneither(_tmpBE,sizeof(void*),0));});goto _LL1E;_LL31: {struct
Cyc_Typerep_XTUnion_struct*_tmp59=(struct Cyc_Typerep_XTUnion_struct*)_tmp3E;if(
_tmp59->tag != 9)goto _LL33;else{_tmp5A=_tmp59->f1;_tmp5B=_tmp59->f2;}}_LL32:({
struct Cyc_String_pa_struct _tmpC2;_tmpC2.tag=0;_tmpC2.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp5A);{void*_tmpC0[1]={& _tmpC2};Cyc_printf(({const char*
_tmpC1="XTUnion(%s,";_tag_dyneither(_tmpC1,sizeof(char),12);}),_tag_dyneither(
_tmpC0,sizeof(void*),1));}});{int i=0;for(0;i < _get_dyneither_size(_tmp5B,sizeof(
struct _tuple3*));++ i){struct _dyneither_ptr _tmpC4;void*_tmpC5;struct _tuple3 _tmpC3=*((
struct _tuple3**)_tmp5B.curr)[i];_tmpC4=_tmpC3.f1;_tmpC5=_tmpC3.f2;({struct Cyc_String_pa_struct
_tmpC8;_tmpC8.tag=0;_tmpC8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_dyneither_ptr_plus(_tmpC4,sizeof(char),4));{void*_tmpC6[1]={& _tmpC8};Cyc_printf(({
const char*_tmpC7="$(%s,";_tag_dyneither(_tmpC7,sizeof(char),6);}),_tag_dyneither(
_tmpC6,sizeof(void*),1));}});Cyc_Typerep_print_typestruct(_tmpC5);({struct Cyc_Int_pa_struct
_tmpCB;_tmpCB.tag=1;_tmpCB.f1=(unsigned long)((int)(i != _get_dyneither_size(
_tmp5B,sizeof(struct _tuple3*))- 1?',':' '));{void*_tmpC9[1]={& _tmpCB};Cyc_printf(({
const char*_tmpCA=")%c";_tag_dyneither(_tmpCA,sizeof(char),4);}),_tag_dyneither(
_tmpC9,sizeof(void*),1));}});}}({void*_tmpCC=0;Cyc_printf(({const char*_tmpCD=" )";
_tag_dyneither(_tmpCD,sizeof(char),3);}),_tag_dyneither(_tmpCC,sizeof(void*),0));});
goto _LL1E;_LL33: {struct Cyc_Typerep_Union_struct*_tmp5C=(struct Cyc_Typerep_Union_struct*)
_tmp3E;if(_tmp5C->tag != 10)goto _LL35;else{_tmp5D=_tmp5C->f1;_tmp5E=_tmp5C->f2;
_tmp5F=_tmp5C->f3;}}_LL34:({struct Cyc_Int_pa_struct _tmpD1;_tmpD1.tag=1;_tmpD1.f1=(
unsigned long)_tmp5E;{struct Cyc_String_pa_struct _tmpD0;_tmpD0.tag=0;_tmpD0.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Typerep_anon_or_name(_tmp5D));{
void*_tmpCE[2]={& _tmpD0,& _tmpD1};Cyc_printf(({const char*_tmpCF="Union(%s,%d,";
_tag_dyneither(_tmpCF,sizeof(char),13);}),_tag_dyneither(_tmpCE,sizeof(void*),2));}}});{
int i=0;for(0;i < _get_dyneither_size(_tmp5F,sizeof(struct _tuple3*));++ i){struct
_dyneither_ptr _tmpD3;void*_tmpD4;struct _tuple3 _tmpD2=*((struct _tuple3**)_tmp5F.curr)[
i];_tmpD3=_tmpD2.f1;_tmpD4=_tmpD2.f2;({struct Cyc_String_pa_struct _tmpD7;_tmpD7.tag=
0;_tmpD7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD3);{void*_tmpD5[1]={&
_tmpD7};Cyc_printf(({const char*_tmpD6="$(%s,";_tag_dyneither(_tmpD6,sizeof(char),
6);}),_tag_dyneither(_tmpD5,sizeof(void*),1));}});Cyc_Typerep_print_typestruct(
_tmpD4);({struct Cyc_Int_pa_struct _tmpDA;_tmpDA.tag=1;_tmpDA.f1=(unsigned long)((
int)(i != _get_dyneither_size(_tmp5F,sizeof(struct _tuple3*))- 1?',':' '));{void*
_tmpD8[1]={& _tmpDA};Cyc_printf(({const char*_tmpD9=")%c";_tag_dyneither(_tmpD9,
sizeof(char),4);}),_tag_dyneither(_tmpD8,sizeof(void*),1));}});}}({void*_tmpDB=0;
Cyc_printf(({const char*_tmpDC=")";_tag_dyneither(_tmpDC,sizeof(char),2);}),
_tag_dyneither(_tmpDB,sizeof(void*),0));});goto _LL1E;_LL35: {struct Cyc_Typerep_Enum_struct*
_tmp60=(struct Cyc_Typerep_Enum_struct*)_tmp3E;if(_tmp60->tag != 11)goto _LL1E;
else{_tmp61=_tmp60->f1;_tmp62=_tmp60->f2;_tmp63=_tmp60->f3;}}_LL36:({struct Cyc_Int_pa_struct
_tmpE0;_tmpE0.tag=1;_tmpE0.f1=(unsigned long)_tmp62;{struct Cyc_String_pa_struct
_tmpDF;_tmpDF.tag=0;_tmpDF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Typerep_anon_or_name(
_tmp61));{void*_tmpDD[2]={& _tmpDF,& _tmpE0};Cyc_printf(({const char*_tmpDE="Enum(%s,%d,";
_tag_dyneither(_tmpDE,sizeof(char),12);}),_tag_dyneither(_tmpDD,sizeof(void*),2));}}});{
int i=0;for(0;i < _get_dyneither_size(_tmp63,sizeof(struct _tuple2*));++ i){
unsigned int _tmpE2;struct _dyneither_ptr _tmpE3;struct _tuple2 _tmpE1=*((struct
_tuple2**)_tmp63.curr)[i];_tmpE2=_tmpE1.f1;_tmpE3=_tmpE1.f2;({struct Cyc_Int_pa_struct
_tmpE8;_tmpE8.tag=1;_tmpE8.f1=(unsigned long)((int)(i != _get_dyneither_size(
_tmp63,sizeof(struct _tuple2*))- 1?',':' '));{struct Cyc_String_pa_struct _tmpE7;
_tmpE7.tag=0;_tmpE7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpE3);{
struct Cyc_Int_pa_struct _tmpE6;_tmpE6.tag=1;_tmpE6.f1=(unsigned long)((int)_tmpE2);{
void*_tmpE4[3]={& _tmpE6,& _tmpE7,& _tmpE8};Cyc_printf(({const char*_tmpE5="$(%d,%s)%c";
_tag_dyneither(_tmpE5,sizeof(char),11);}),_tag_dyneither(_tmpE4,sizeof(void*),3));}}}});}}({
void*_tmpE9=0;Cyc_printf(({const char*_tmpEA=")";_tag_dyneither(_tmpEA,sizeof(
char),2);}),_tag_dyneither(_tmpE9,sizeof(void*),0));});goto _LL1E;_LL1E:;}void Cyc_Typerep_xmlize_typestruct(
void*rep){void*_tmpEB=rep;int _tmpED;unsigned int _tmpEE;unsigned int _tmpF2;void*
_tmpF3;void*_tmpF5;struct _dyneither_ptr*_tmpF7;unsigned int _tmpF8;struct
_dyneither_ptr _tmpF9;unsigned int _tmpFB;struct _dyneither_ptr _tmpFC;struct
_dyneither_ptr _tmpFE;struct _dyneither_ptr _tmpFF;unsigned int _tmp100;struct
_dyneither_ptr _tmp101;struct _dyneither_ptr _tmp103;struct _dyneither_ptr _tmp104;
struct _dyneither_ptr _tmp105;struct _dyneither_ptr _tmp107;struct _dyneither_ptr
_tmp108;struct _dyneither_ptr*_tmp10A;int _tmp10B;struct _dyneither_ptr _tmp10C;
struct _dyneither_ptr*_tmp10E;int _tmp10F;struct _dyneither_ptr _tmp110;_LL38: {
struct Cyc_Typerep_Int_struct*_tmpEC=(struct Cyc_Typerep_Int_struct*)_tmpEB;if(
_tmpEC->tag != 0)goto _LL3A;else{_tmpED=_tmpEC->f1;_tmpEE=_tmpEC->f2;}}_LL39:({
struct Cyc_Int_pa_struct _tmp114;_tmp114.tag=1;_tmp114.f1=(unsigned long)((int)
_tmpEE);{struct Cyc_Int_pa_struct _tmp113;_tmp113.tag=1;_tmp113.f1=(unsigned long)
_tmpED;{void*_tmp111[2]={& _tmp113,& _tmp114};Cyc_printf(({const char*_tmp112="<Int sn=%d sz=\"%d\"/>";
_tag_dyneither(_tmp112,sizeof(char),21);}),_tag_dyneither(_tmp111,sizeof(void*),
2));}}});goto _LL37;_LL3A: {struct Cyc_Typerep_Float_struct*_tmpEF=(struct Cyc_Typerep_Float_struct*)
_tmpEB;if(_tmpEF->tag != 1)goto _LL3C;}_LL3B:({void*_tmp115=0;Cyc_printf(({const
char*_tmp116="<Float/>";_tag_dyneither(_tmp116,sizeof(char),9);}),_tag_dyneither(
_tmp115,sizeof(void*),0));});goto _LL37;_LL3C: {struct Cyc_Typerep_Double_struct*
_tmpF0=(struct Cyc_Typerep_Double_struct*)_tmpEB;if(_tmpF0->tag != 2)goto _LL3E;}
_LL3D:({void*_tmp117=0;Cyc_printf(({const char*_tmp118="<Double/>";_tag_dyneither(
_tmp118,sizeof(char),10);}),_tag_dyneither(_tmp117,sizeof(void*),0));});goto
_LL37;_LL3E: {struct Cyc_Typerep_ThinPtr_struct*_tmpF1=(struct Cyc_Typerep_ThinPtr_struct*)
_tmpEB;if(_tmpF1->tag != 3)goto _LL40;else{_tmpF2=_tmpF1->f1;_tmpF3=(void*)_tmpF1->f2;}}
_LL3F:({struct Cyc_Int_pa_struct _tmp11B;_tmp11B.tag=1;_tmp11B.f1=(unsigned long)((
int)_tmpF2);{void*_tmp119[1]={& _tmp11B};Cyc_printf(({const char*_tmp11A="<ThinPtr sz=\"%d\">";
_tag_dyneither(_tmp11A,sizeof(char),18);}),_tag_dyneither(_tmp119,sizeof(void*),
1));}});Cyc_Typerep_xmlize_typestruct(_tmpF3);({void*_tmp11C=0;Cyc_printf(({
const char*_tmp11D="</ThinPtr";_tag_dyneither(_tmp11D,sizeof(char),10);}),
_tag_dyneither(_tmp11C,sizeof(void*),0));});goto _LL37;_LL40: {struct Cyc_Typerep_FatPtr_struct*
_tmpF4=(struct Cyc_Typerep_FatPtr_struct*)_tmpEB;if(_tmpF4->tag != 4)goto _LL42;
else{_tmpF5=(void*)_tmpF4->f1;}}_LL41:({void*_tmp11E=0;Cyc_printf(({const char*
_tmp11F="<FatPtr>";_tag_dyneither(_tmp11F,sizeof(char),9);}),_tag_dyneither(
_tmp11E,sizeof(void*),0));});Cyc_Typerep_xmlize_typestruct(_tmpF5);({void*
_tmp120=0;Cyc_printf(({const char*_tmp121="</FatPtr>";_tag_dyneither(_tmp121,
sizeof(char),10);}),_tag_dyneither(_tmp120,sizeof(void*),0));});goto _LL37;_LL42: {
struct Cyc_Typerep_Struct_struct*_tmpF6=(struct Cyc_Typerep_Struct_struct*)_tmpEB;
if(_tmpF6->tag != 5)goto _LL44;else{_tmpF7=_tmpF6->f1;_tmpF8=_tmpF6->f2;_tmpF9=
_tmpF6->f3;}}_LL43:({struct Cyc_Int_pa_struct _tmp125;_tmp125.tag=1;_tmp125.f1=(
unsigned long)((int)_tmpF8);{struct Cyc_String_pa_struct _tmp124;_tmp124.tag=0;
_tmp124.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Typerep_anon_or_name(
_tmpF7));{void*_tmp122[2]={& _tmp124,& _tmp125};Cyc_printf(({const char*_tmp123="<Struct name=\"%s\" sz=\"%d\">";
_tag_dyneither(_tmp123,sizeof(char),27);}),_tag_dyneither(_tmp122,sizeof(void*),
2));}}});{int i=0;for(0;i < _get_dyneither_size(_tmpF9,sizeof(struct _tuple0*));++ i){
unsigned int _tmp127;struct _dyneither_ptr _tmp128;void*_tmp129;struct _tuple0
_tmp126=*((struct _tuple0**)_tmpF9.curr)[i];_tmp127=_tmp126.f1;_tmp128=_tmp126.f2;
_tmp129=_tmp126.f3;({struct Cyc_String_pa_struct _tmp12D;_tmp12D.tag=0;_tmp12D.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp128);{struct Cyc_Int_pa_struct
_tmp12C;_tmp12C.tag=1;_tmp12C.f1=(unsigned long)((int)_tmp127);{void*_tmp12A[2]={&
_tmp12C,& _tmp12D};Cyc_printf(({const char*_tmp12B="<Component ofs=\"%d\" nm=\"%s\">";
_tag_dyneither(_tmp12B,sizeof(char),29);}),_tag_dyneither(_tmp12A,sizeof(void*),
2));}}});Cyc_Typerep_xmlize_typestruct(_tmp129);({void*_tmp12E=0;Cyc_printf(({
const char*_tmp12F="</Component>";_tag_dyneither(_tmp12F,sizeof(char),13);}),
_tag_dyneither(_tmp12E,sizeof(void*),0));});}}({void*_tmp130=0;Cyc_printf(({
const char*_tmp131="</Struct>";_tag_dyneither(_tmp131,sizeof(char),10);}),
_tag_dyneither(_tmp130,sizeof(void*),0));});goto _LL37;_LL44: {struct Cyc_Typerep_Tuple_struct*
_tmpFA=(struct Cyc_Typerep_Tuple_struct*)_tmpEB;if(_tmpFA->tag != 6)goto _LL46;
else{_tmpFB=_tmpFA->f1;_tmpFC=_tmpFA->f2;}}_LL45:({struct Cyc_Int_pa_struct
_tmp134;_tmp134.tag=1;_tmp134.f1=(unsigned long)((int)_tmpFB);{void*_tmp132[1]={&
_tmp134};Cyc_printf(({const char*_tmp133="<Tuple sz=\"%d\">";_tag_dyneither(
_tmp133,sizeof(char),16);}),_tag_dyneither(_tmp132,sizeof(void*),1));}});{int i=0;
for(0;i < _get_dyneither_size(_tmpFC,sizeof(struct _tuple1*));++ i){unsigned int
_tmp136;void*_tmp137;struct _tuple1 _tmp135=*((struct _tuple1**)_tmpFC.curr)[i];
_tmp136=_tmp135.f1;_tmp137=_tmp135.f2;({struct Cyc_Int_pa_struct _tmp13A;_tmp13A.tag=
1;_tmp13A.f1=(unsigned long)((int)_tmp136);{void*_tmp138[1]={& _tmp13A};Cyc_printf(({
const char*_tmp139="<Component ofs=\"%d\">";_tag_dyneither(_tmp139,sizeof(char),
21);}),_tag_dyneither(_tmp138,sizeof(void*),1));}});Cyc_Typerep_xmlize_typestruct(
_tmp137);({void*_tmp13B=0;Cyc_printf(({const char*_tmp13C="</Component>";
_tag_dyneither(_tmp13C,sizeof(char),13);}),_tag_dyneither(_tmp13B,sizeof(void*),
0));});}}({void*_tmp13D=0;Cyc_printf(({const char*_tmp13E="</Tuple>";
_tag_dyneither(_tmp13E,sizeof(char),9);}),_tag_dyneither(_tmp13D,sizeof(void*),0));});
goto _LL37;_LL46: {struct Cyc_Typerep_TUnionField_struct*_tmpFD=(struct Cyc_Typerep_TUnionField_struct*)
_tmpEB;if(_tmpFD->tag != 8)goto _LL48;else{_tmpFE=_tmpFD->f1;_tmpFF=_tmpFD->f2;
_tmp100=_tmpFD->f3;_tmp101=_tmpFD->f4;}}_LL47:({struct Cyc_Int_pa_struct _tmp143;
_tmp143.tag=1;_tmp143.f1=(unsigned long)((int)_tmp100);{struct Cyc_String_pa_struct
_tmp142;_tmp142.tag=0;_tmp142.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmpFF);{struct Cyc_String_pa_struct _tmp141;_tmp141.tag=0;_tmp141.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)_tmpFE);{void*_tmp13F[3]={& _tmp141,&
_tmp142,& _tmp143};Cyc_printf(({const char*_tmp140="<TUnionField tname=\"%s\" fname=\"%s\" sz=\"%d\">";
_tag_dyneither(_tmp140,sizeof(char),44);}),_tag_dyneither(_tmp13F,sizeof(void*),
3));}}}});{int i=0;for(0;i < _get_dyneither_size(_tmp101,sizeof(struct _tuple1*));
++ i){unsigned int _tmp145;void*_tmp146;struct _tuple1 _tmp144=*((struct _tuple1**)
_tmp101.curr)[i];_tmp145=_tmp144.f1;_tmp146=_tmp144.f2;({struct Cyc_Int_pa_struct
_tmp149;_tmp149.tag=1;_tmp149.f1=(unsigned long)((int)_tmp145);{void*_tmp147[1]={&
_tmp149};Cyc_printf(({const char*_tmp148="<Component ofs=\"%d\">";_tag_dyneither(
_tmp148,sizeof(char),21);}),_tag_dyneither(_tmp147,sizeof(void*),1));}});Cyc_Typerep_xmlize_typestruct(
_tmp146);({void*_tmp14A=0;Cyc_printf(({const char*_tmp14B="</Component>";
_tag_dyneither(_tmp14B,sizeof(char),13);}),_tag_dyneither(_tmp14A,sizeof(void*),
0));});}}({void*_tmp14C=0;Cyc_printf(({const char*_tmp14D="</TUnionField>";
_tag_dyneither(_tmp14D,sizeof(char),15);}),_tag_dyneither(_tmp14C,sizeof(void*),
0));});goto _LL37;_LL48: {struct Cyc_Typerep_TUnion_struct*_tmp102=(struct Cyc_Typerep_TUnion_struct*)
_tmpEB;if(_tmp102->tag != 7)goto _LL4A;else{_tmp103=_tmp102->f1;_tmp104=_tmp102->f2;
_tmp105=_tmp102->f3;}}_LL49:({struct Cyc_String_pa_struct _tmp150;_tmp150.tag=0;
_tmp150.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp103);{void*_tmp14E[1]={&
_tmp150};Cyc_printf(({const char*_tmp14F="<TUnion name=\"%s\">";_tag_dyneither(
_tmp14F,sizeof(char),19);}),_tag_dyneither(_tmp14E,sizeof(void*),1));}});{int i=0;
for(0;i < _get_dyneither_size(_tmp104,sizeof(struct _tuple2*));++ i){unsigned int
_tmp152;struct _dyneither_ptr _tmp153;struct _tuple2 _tmp151=*((struct _tuple2**)
_tmp104.curr)[i];_tmp152=_tmp151.f1;_tmp153=_tmp151.f2;({struct Cyc_String_pa_struct
_tmp157;_tmp157.tag=0;_tmp157.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp153);{struct Cyc_Int_pa_struct _tmp156;_tmp156.tag=1;_tmp156.f1=(unsigned long)((
int)_tmp152);{void*_tmp154[2]={& _tmp156,& _tmp157};Cyc_printf(({const char*_tmp155="<Tag tag=\"%d\" nm=\"%s\"/>";
_tag_dyneither(_tmp155,sizeof(char),24);}),_tag_dyneither(_tmp154,sizeof(void*),
2));}}});}}{int i=0;for(0;i < _get_dyneither_size(_tmp105,sizeof(struct _tuple0*));
++ i){unsigned int _tmp159;struct _dyneither_ptr _tmp15A;void*_tmp15B;struct _tuple0
_tmp158=*((struct _tuple0**)_tmp105.curr)[i];_tmp159=_tmp158.f1;_tmp15A=_tmp158.f2;
_tmp15B=_tmp158.f3;({struct Cyc_String_pa_struct _tmp15F;_tmp15F.tag=0;_tmp15F.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp15A);{struct Cyc_Int_pa_struct
_tmp15E;_tmp15E.tag=1;_tmp15E.f1=(unsigned long)((int)_tmp159);{void*_tmp15C[2]={&
_tmp15E,& _tmp15F};Cyc_printf(({const char*_tmp15D="<Tag tag=\"%d\" nm=\"%s\">";
_tag_dyneither(_tmp15D,sizeof(char),23);}),_tag_dyneither(_tmp15C,sizeof(void*),
2));}}});Cyc_Typerep_xmlize_typestruct(_tmp15B);({void*_tmp160=0;Cyc_printf(({
const char*_tmp161="</Tag>";_tag_dyneither(_tmp161,sizeof(char),7);}),
_tag_dyneither(_tmp160,sizeof(void*),0));});}}({void*_tmp162=0;Cyc_printf(({
const char*_tmp163="</Tunion>";_tag_dyneither(_tmp163,sizeof(char),10);}),
_tag_dyneither(_tmp162,sizeof(void*),0));});goto _LL37;_LL4A: {struct Cyc_Typerep_XTUnion_struct*
_tmp106=(struct Cyc_Typerep_XTUnion_struct*)_tmpEB;if(_tmp106->tag != 9)goto _LL4C;
else{_tmp107=_tmp106->f1;_tmp108=_tmp106->f2;}}_LL4B:({struct Cyc_String_pa_struct
_tmp166;_tmp166.tag=0;_tmp166.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp107);{void*_tmp164[1]={& _tmp166};Cyc_printf(({const char*_tmp165="<XTUnion name=\"%s\">";
_tag_dyneither(_tmp165,sizeof(char),20);}),_tag_dyneither(_tmp164,sizeof(void*),
1));}});{int i=0;for(0;i < _get_dyneither_size(_tmp108,sizeof(struct _tuple3*));++ i){
struct _dyneither_ptr _tmp168;void*_tmp169;struct _tuple3 _tmp167=*((struct _tuple3**)
_tmp108.curr)[i];_tmp168=_tmp167.f1;_tmp169=_tmp167.f2;({struct Cyc_String_pa_struct
_tmp16C;_tmp16C.tag=0;_tmp16C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_dyneither_ptr_plus(_tmp168,sizeof(char),4));{void*_tmp16A[1]={& _tmp16C};Cyc_printf(({
const char*_tmp16B="<Tag tag=\"%s\">";_tag_dyneither(_tmp16B,sizeof(char),15);}),
_tag_dyneither(_tmp16A,sizeof(void*),1));}});Cyc_Typerep_xmlize_typestruct(
_tmp169);({void*_tmp16D=0;Cyc_printf(({const char*_tmp16E="</Tag>";_tag_dyneither(
_tmp16E,sizeof(char),7);}),_tag_dyneither(_tmp16D,sizeof(void*),0));});}}({void*
_tmp16F=0;Cyc_printf(({const char*_tmp170="</XTunion>";_tag_dyneither(_tmp170,
sizeof(char),11);}),_tag_dyneither(_tmp16F,sizeof(void*),0));});goto _LL37;_LL4C: {
struct Cyc_Typerep_Union_struct*_tmp109=(struct Cyc_Typerep_Union_struct*)_tmpEB;
if(_tmp109->tag != 10)goto _LL4E;else{_tmp10A=_tmp109->f1;_tmp10B=_tmp109->f2;
_tmp10C=_tmp109->f3;}}_LL4D:({struct Cyc_Int_pa_struct _tmp174;_tmp174.tag=1;
_tmp174.f1=(unsigned long)_tmp10B;{struct Cyc_String_pa_struct _tmp173;_tmp173.tag=
0;_tmp173.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Typerep_anon_or_name(
_tmp10A));{void*_tmp171[2]={& _tmp173,& _tmp174};Cyc_printf(({const char*_tmp172="<Union name=\"%s\" sz=\"%d\">";
_tag_dyneither(_tmp172,sizeof(char),26);}),_tag_dyneither(_tmp171,sizeof(void*),
2));}}});{int i=0;for(0;i < _get_dyneither_size(_tmp10C,sizeof(struct _tuple3*));++
i){struct _dyneither_ptr _tmp176;void*_tmp177;struct _tuple3 _tmp175=*((struct
_tuple3**)_tmp10C.curr)[i];_tmp176=_tmp175.f1;_tmp177=_tmp175.f2;({struct Cyc_String_pa_struct
_tmp17A;_tmp17A.tag=0;_tmp17A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp176);{void*_tmp178[1]={& _tmp17A};Cyc_printf(({const char*_tmp179="<Case nm=\"%s\">";
_tag_dyneither(_tmp179,sizeof(char),15);}),_tag_dyneither(_tmp178,sizeof(void*),
1));}});Cyc_Typerep_xmlize_typestruct(_tmp177);({void*_tmp17B=0;Cyc_printf(({
const char*_tmp17C="</Case>";_tag_dyneither(_tmp17C,sizeof(char),8);}),
_tag_dyneither(_tmp17B,sizeof(void*),0));});}}({void*_tmp17D=0;Cyc_printf(({
const char*_tmp17E="</Union>";_tag_dyneither(_tmp17E,sizeof(char),9);}),
_tag_dyneither(_tmp17D,sizeof(void*),0));});goto _LL37;_LL4E: {struct Cyc_Typerep_Enum_struct*
_tmp10D=(struct Cyc_Typerep_Enum_struct*)_tmpEB;if(_tmp10D->tag != 11)goto _LL37;
else{_tmp10E=_tmp10D->f1;_tmp10F=_tmp10D->f2;_tmp110=_tmp10D->f3;}}_LL4F:({
struct Cyc_Int_pa_struct _tmp182;_tmp182.tag=1;_tmp182.f1=(unsigned long)_tmp10F;{
struct Cyc_String_pa_struct _tmp181;_tmp181.tag=0;_tmp181.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Typerep_anon_or_name(_tmp10E));{void*_tmp17F[2]={&
_tmp181,& _tmp182};Cyc_printf(({const char*_tmp180="<Enum name=\"%s\" szb=\"%d\">";
_tag_dyneither(_tmp180,sizeof(char),26);}),_tag_dyneither(_tmp17F,sizeof(void*),
2));}}});{int i=0;for(0;i < _get_dyneither_size(_tmp110,sizeof(struct _tuple2*));++
i){unsigned int _tmp184;struct _dyneither_ptr _tmp185;struct _tuple2 _tmp183=*((
struct _tuple2**)_tmp110.curr)[i];_tmp184=_tmp183.f1;_tmp185=_tmp183.f2;({struct
Cyc_String_pa_struct _tmp189;_tmp189.tag=0;_tmp189.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp185);{struct Cyc_Int_pa_struct _tmp188;_tmp188.tag=1;
_tmp188.f1=(unsigned long)((int)_tmp184);{void*_tmp186[2]={& _tmp188,& _tmp189};
Cyc_printf(({const char*_tmp187="<Val val=\"%d\">%s</Val>";_tag_dyneither(_tmp187,
sizeof(char),23);}),_tag_dyneither(_tmp186,sizeof(void*),2));}}});}}({void*
_tmp18A=0;Cyc_printf(({const char*_tmp18B="</Enum>";_tag_dyneither(_tmp18B,
sizeof(char),8);}),_tag_dyneither(_tmp18A,sizeof(void*),0));});goto _LL37;_LL37:;}
