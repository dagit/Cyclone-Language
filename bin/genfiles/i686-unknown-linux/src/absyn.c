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
void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),
struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[8];int Cyc_List_list_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{
struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Segment*
Cyc_Position_segment_join(struct Cyc_Position_Segment*,struct Cyc_Position_Segment*);
struct Cyc_Position_Error{struct _dyneither_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dyneither_ptr desc;};extern char Cyc_Position_Nocontext[14];
struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};
union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};struct _tuple0{union Cyc_Absyn_Nmspace_union
f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int
print_const;int q_volatile;int q_restrict;int real_const;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_No_constr_struct{int tag;};
union Cyc_Absyn_Constraint_union{struct Cyc_Absyn_Eq_constr_struct Eq_constr;struct
Cyc_Absyn_Forward_constr_struct Forward_constr;struct Cyc_Absyn_No_constr_struct
No_constr;};struct Cyc_Absyn_Conref{union Cyc_Absyn_Constraint_union v;};struct Cyc_Absyn_Eq_kb_struct{
int tag;void*f1;};struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;
};struct Cyc_Absyn_Less_kb_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;};struct
Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*
ptr_loc;struct Cyc_Position_Segment*rgn_loc;struct Cyc_Position_Segment*zt_loc;};
struct Cyc_Absyn_PtrAtts{void*rgn;struct Cyc_Absyn_Conref*nullable;struct Cyc_Absyn_Conref*
bounds;struct Cyc_Absyn_Conref*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct
Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts
ptr_atts;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual
tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;struct
Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_UnknownTunionInfo{struct _tuple0*name;int is_xtunion;int is_flat;};struct
Cyc_Absyn_UnknownTunion_struct{int tag;struct Cyc_Absyn_UnknownTunionInfo f1;};
struct Cyc_Absyn_KnownTunion_struct{int tag;struct Cyc_Absyn_Tuniondecl**f1;};union
Cyc_Absyn_TunionInfoU_union{struct Cyc_Absyn_UnknownTunion_struct UnknownTunion;
struct Cyc_Absyn_KnownTunion_struct KnownTunion;};struct Cyc_Absyn_TunionInfo{union
Cyc_Absyn_TunionInfoU_union tunion_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple0*tunion_name;struct
_tuple0*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{int
tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};union Cyc_Absyn_TunionFieldInfoU_union{
struct Cyc_Absyn_UnknownTunionfield_struct UnknownTunionfield;struct Cyc_Absyn_KnownTunionfield_struct
KnownTunionfield;};struct Cyc_Absyn_TunionFieldInfo{union Cyc_Absyn_TunionFieldInfoU_union
field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{int tag;
void*f1;struct _tuple0*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct Cyc_Absyn_Aggrdecl**
f1;};union Cyc_Absyn_AggrInfoU_union{struct Cyc_Absyn_UnknownAggr_struct
UnknownAggr;struct Cyc_Absyn_KnownAggr_struct KnownAggr;};struct Cyc_Absyn_AggrInfo{
union Cyc_Absyn_AggrInfoU_union aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{
void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;struct Cyc_Absyn_Conref*
zero_term;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_Evar_struct{int tag;
struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;};struct
Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_TunionType_struct{
int tag;struct Cyc_Absyn_TunionInfo f1;};struct Cyc_Absyn_TunionFieldType_struct{int
tag;struct Cyc_Absyn_TunionFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{int
tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;void*f1;
void*f2;};struct Cyc_Absyn_DoubleType_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{
int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_struct{int tag;struct
Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct
Cyc_Absyn_AnonAggrType_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_struct{
int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*
f1;};struct Cyc_Absyn_DynRgnType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
void**f4;};struct Cyc_Absyn_ValueofType_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_AccessEff_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};struct Cyc_Absyn_NoTypes_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*
f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};
struct Cyc_Absyn_Aligned_att_struct{int tag;int f1;};struct Cyc_Absyn_Section_att_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Format_att_struct{int tag;void*f1;
int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{int tag;int f1;};struct Cyc_Absyn_Mode_att_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_struct{int tag;struct
Cyc_Absyn_Conref*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Conref*f2;struct Cyc_Position_Segment*
f3;};struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct
Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int tag;struct Cyc_Position_Segment*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Char_c_struct{int tag;void*f1;char f2;
};struct Cyc_Absyn_Short_c_struct{int tag;void*f1;short f2;};struct Cyc_Absyn_Int_c_struct{
int tag;void*f1;int f2;};struct Cyc_Absyn_LongLong_c_struct{int tag;void*f1;
long long f2;};struct Cyc_Absyn_Float_c_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_Absyn_String_c_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Null_c_struct{
int tag;};union Cyc_Absyn_Cnst_union{struct Cyc_Absyn_Char_c_struct Char_c;struct Cyc_Absyn_Short_c_struct
Short_c;struct Cyc_Absyn_Int_c_struct Int_c;struct Cyc_Absyn_LongLong_c_struct
LongLong_c;struct Cyc_Absyn_Float_c_struct Float_c;struct Cyc_Absyn_String_c_struct
String_c;struct Cyc_Absyn_Null_c_struct Null_c;};struct Cyc_Absyn_VarargCallInfo{
int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
struct Cyc_Absyn_StructField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{
int tag;unsigned int f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*
rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{
int tag;union Cyc_Absyn_Cnst_union f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct
_tuple0*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple0*f1;
};struct Cyc_Absyn_Primop_e_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct
Cyc_Absyn_AssignOp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;
struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;void*f2;};struct Cyc_Absyn_Conditional_e_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_struct{int
tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnknownCall_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*
f3;};struct Cyc_Absyn_Throw_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{
int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;void*f4;};struct Cyc_Absyn_Address_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{int tag;void*f1;};
struct Cyc_Absyn_Sizeofexp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_Gentyp_e_struct{int tag;struct Cyc_List_List*
f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_AggrMember_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_dyneither_ptr*f2;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Subscript_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple1{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;
int f4;};struct Cyc_Absyn_Struct_e_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Tunion_e_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*f2;struct Cyc_Absyn_Tunionfield*
f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*
f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{int tag;
struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Valueof_e_struct{int tag;void*f1;
};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*
loc;void*annot;};struct Cyc_Absyn_Exp_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};
struct _tuple2{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
int tag;struct _tuple2 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct _dyneither_ptr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tuple2 f2;struct _tuple2 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple2 f2;};struct Cyc_Absyn_TryCatch_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Region_s_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*
f4;struct Cyc_Absyn_Stmt*f5;};struct Cyc_Absyn_ResetRegion_s_struct{int tag;struct
Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Alias_s_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;struct Cyc_Absyn_Stmt*f4;};
struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*loc;struct Cyc_List_List*
non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Var_p_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_struct{
int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_struct{int tag;
struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_struct{int tag;struct Cyc_Absyn_AggrInfo
f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Tunion_p_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;struct Cyc_List_List*
f3;int f4;};struct Cyc_Absyn_Int_p_struct{int tag;void*f1;int f2;};struct Cyc_Absyn_Char_p_struct{
int tag;char f1;};struct Cyc_Absyn_Float_p_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*
topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;};struct Cyc_Absyn_Aggrdecl{void*
kind;void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple0*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;int is_flat;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};char Cyc_Absyn_EmptyAnnot[15]="\000\000\000\000EmptyAnnot\000";
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*);int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);struct Cyc_Absyn_Tqual
Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);
struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);
void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*
Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern
struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_force_kb(
void*kb);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);void*Cyc_Absyn_int_typ(void*,void*);
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;extern void*Cyc_Absyn_float_typ;
void*Cyc_Absyn_double_typ(int);extern void*Cyc_Absyn_empty_effect;extern struct
_tuple0*Cyc_Absyn_exn_name;extern struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud;
extern void*Cyc_Absyn_exn_typ;extern struct _tuple0*Cyc_Absyn_tunion_print_arg_qvar;
extern struct _tuple0*Cyc_Absyn_tunion_scanf_arg_qvar;void*Cyc_Absyn_string_typ(
void*rgn);void*Cyc_Absyn_const_string_typ(void*rgn);void*Cyc_Absyn_file_typ();
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,struct
Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,void*b,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_star_typ(void*t,void*
rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_at_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dyneither_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct _dyneither_ptr*name);void*
Cyc_Absyn_strctq(struct _tuple0*name);void*Cyc_Absyn_unionq_typ(struct _tuple0*
name);void*Cyc_Absyn_union_typ(struct _dyneither_ptr*name);void*Cyc_Absyn_array_typ(
void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*
zero_term,struct Cyc_Position_Segment*ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct
Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst_union,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_bool_exp(int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*,int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(void*,struct Cyc_List_List*es,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct
Cyc_Absyn_Exp*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(
struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(struct Cyc_List_List*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(
struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(
struct _dyneither_ptr*lab,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct
Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(
void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(
struct Cyc_List_List*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_AggrdeclImpl*
Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct
Cyc_List_List*fs);struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*k,void*s,struct
_tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(
void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_union_decl(void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*
i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_tunion_decl(void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*
fs,int is_xtunion,int is_flat,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts);void*Cyc_Absyn_pointer_expand(void*,int
fresh_evar);int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
struct _tuple3{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple3*Cyc_Absyn_lookup_tuple_field(
struct Cyc_List_List*,int);struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
int Cyc_Absyn_fntype_att(void*a);struct _dyneither_ptr*Cyc_Absyn_fieldname(int);
struct _tuple4{void*f1;struct _tuple0*f2;};struct _tuple4 Cyc_Absyn_aggr_kinded_name(
union Cyc_Absyn_AggrInfoU_union);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU_union info);int Cyc_Absyn_is_union_type(void*);int Cyc_Absyn_is_aggr_type(
void*t);void Cyc_Absyn_print_decls(struct Cyc_List_List*);extern int Cyc_Absyn_porting_c_code;
extern int Cyc_Absyn_no_regions;struct Cyc_Typerep_Int_struct{int tag;int f1;
unsigned int f2;};struct Cyc_Typerep_ThinPtr_struct{int tag;unsigned int f1;void*f2;
};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;};struct _tuple5{unsigned int f1;
struct _dyneither_ptr f2;void*f3;};struct Cyc_Typerep_Struct_struct{int tag;struct
_dyneither_ptr*f1;unsigned int f2;struct _dyneither_ptr f3;};struct _tuple6{
unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;unsigned int f1;
struct _dyneither_ptr f2;};struct _tuple7{unsigned int f1;struct _dyneither_ptr f2;};
struct Cyc_Typerep_TUnion_struct{int tag;struct _dyneither_ptr f1;struct
_dyneither_ptr f2;struct _dyneither_ptr f3;};struct Cyc_Typerep_TUnionField_struct{
int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;unsigned int f3;struct
_dyneither_ptr f4;};struct _tuple8{struct _dyneither_ptr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;};struct Cyc_Typerep_Union_struct{
int tag;struct _dyneither_ptr*f1;int f2;struct _dyneither_ptr f3;};struct Cyc_Typerep_Enum_struct{
int tag;struct _dyneither_ptr*f1;int f2;struct _dyneither_ptr f3;};unsigned int Cyc_Typerep_size_type(
void*rep);extern void*Cyc_decls_rep;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Position_Segment_rep;
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);typedef struct{
int __count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;
typedef struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t
Cyc_fpos_t;struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;}
;struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[
19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;
struct _dyneither_ptr f1;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char
Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);
struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern
char Cyc_Dict_Absent[11];struct _tuple9{void*f1;void*f2;};struct _tuple9*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple9*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct
Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct
_RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,struct Cyc_Position_Segment*
loc);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int
Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*
r);int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*
constraints,void*default_bound,int do_pin);int Cyc_RgnOrder_eff_outlives_eff(
struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);void Cyc_RgnOrder_print_region_po(
struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Tcenv_CList{void*hd;struct Cyc_Tcenv_CList*
tl;};struct Cyc_Tcenv_VarRes_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;
struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict tuniondecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*
le;int allow_valueof;};void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);void*Cyc_Tcutil_kind_to_bound(
void*k);void Cyc_Marshal_print_type(void*rep,void*val);struct _tuple10{struct Cyc_Dict_Dict
f1;int f2;};struct _tuple11{struct _dyneither_ptr f1;int f2;};extern int Cyc_Cyclone_tovc_r;
extern void*Cyc_Cyclone_c_compiler;int Cyc_Cyclone_tovc_r=0;void*Cyc_Cyclone_c_compiler=(
void*)0;static int Cyc_Absyn_strlist_cmp(struct Cyc_List_List*ss1,struct Cyc_List_List*
ss2){return((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,
ss1,ss2);}int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*vs1,struct Cyc_List_List*
vs2){if((int)vs1 == (int)vs2)return 0;return Cyc_Absyn_strlist_cmp(vs1,vs2);}struct
_tuple12{union Cyc_Absyn_Nmspace_union f1;union Cyc_Absyn_Nmspace_union f2;};int Cyc_Absyn_qvar_cmp(
struct _tuple0*q1,struct _tuple0*q2){union Cyc_Absyn_Nmspace_union _tmp0=(*q1).f1;
union Cyc_Absyn_Nmspace_union _tmp1=(*q2).f1;{struct _tuple12 _tmp3=({struct _tuple12
_tmp2;_tmp2.f1=_tmp0;_tmp2.f2=_tmp1;_tmp2;});union Cyc_Absyn_Nmspace_union _tmp4;
union Cyc_Absyn_Nmspace_union _tmp5;union Cyc_Absyn_Nmspace_union _tmp6;struct Cyc_List_List*
_tmp7;union Cyc_Absyn_Nmspace_union _tmp8;struct Cyc_List_List*_tmp9;union Cyc_Absyn_Nmspace_union
_tmpA;struct Cyc_List_List*_tmpB;union Cyc_Absyn_Nmspace_union _tmpC;struct Cyc_List_List*
_tmpD;union Cyc_Absyn_Nmspace_union _tmpE;union Cyc_Absyn_Nmspace_union _tmpF;union
Cyc_Absyn_Nmspace_union _tmp10;union Cyc_Absyn_Nmspace_union _tmp11;_LL1: _tmp4=
_tmp3.f1;if(((_tmp3.f1).Loc_n).tag != 0)goto _LL3;_tmp5=_tmp3.f2;if(((_tmp3.f2).Loc_n).tag
!= 0)goto _LL3;_LL2: goto _LL0;_LL3: _tmp6=_tmp3.f1;if(((_tmp3.f1).Rel_n).tag != 1)
goto _LL5;_tmp7=(_tmp6.Rel_n).f1;_tmp8=_tmp3.f2;if(((_tmp3.f2).Rel_n).tag != 1)
goto _LL5;_tmp9=(_tmp8.Rel_n).f1;_LL4: _tmpB=_tmp7;_tmpD=_tmp9;goto _LL6;_LL5: _tmpA=
_tmp3.f1;if(((_tmp3.f1).Abs_n).tag != 2)goto _LL7;_tmpB=(_tmpA.Abs_n).f1;_tmpC=
_tmp3.f2;if(((_tmp3.f2).Abs_n).tag != 2)goto _LL7;_tmpD=(_tmpC.Abs_n).f1;_LL6: {
int i=Cyc_Absyn_strlist_cmp(_tmpB,_tmpD);if(i != 0)return i;goto _LL0;}_LL7: _tmpE=
_tmp3.f1;if(((_tmp3.f1).Loc_n).tag != 0)goto _LL9;_LL8: return - 1;_LL9: _tmpF=_tmp3.f2;
if(((_tmp3.f2).Loc_n).tag != 0)goto _LLB;_LLA: return 1;_LLB: _tmp10=_tmp3.f1;if(((
_tmp3.f1).Rel_n).tag != 1)goto _LLD;_LLC: return - 1;_LLD: _tmp11=_tmp3.f2;if(((_tmp3.f2).Rel_n).tag
!= 1)goto _LL0;_LLE: return 1;_LL0:;}return Cyc_strptrcmp((*q1).f2,(*q2).f2);}int Cyc_Absyn_tvar_cmp(
struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){int i=Cyc_strptrcmp(tv1->name,
tv2->name);if(i != 0)return i;return tv1->identity - tv2->identity;}union Cyc_Absyn_Nmspace_union
Cyc_Absyn_rel_ns_null=(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Rel_n_struct){
1,0});int Cyc_Absyn_is_qvar_qualified(struct _tuple0*qv){union Cyc_Absyn_Nmspace_union
_tmp13=(*qv).f1;struct Cyc_List_List*_tmp14;struct Cyc_List_List*_tmp15;_LL10: if((
_tmp13.Rel_n).tag != 1)goto _LL12;_tmp14=(_tmp13.Rel_n).f1;if(_tmp14 != 0)goto _LL12;
_LL11: goto _LL13;_LL12: if((_tmp13.Abs_n).tag != 2)goto _LL14;_tmp15=(_tmp13.Abs_n).f1;
if(_tmp15 != 0)goto _LL14;_LL13: goto _LL15;_LL14: if((_tmp13.Loc_n).tag != 0)goto
_LL16;_LL15: return 0;_LL16:;_LL17: return 1;_LLF:;}static int Cyc_Absyn_new_type_counter=
0;void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*env){return(
void*)({struct Cyc_Absyn_Evar_struct*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16[0]=({
struct Cyc_Absyn_Evar_struct _tmp17;_tmp17.tag=0;_tmp17.f1=k;_tmp17.f2=0;_tmp17.f3=
Cyc_Absyn_new_type_counter ++;_tmp17.f4=env;_tmp17;});_tmp16;});}static struct Cyc_Core_Opt
Cyc_Absyn_mk={(void*)((void*)1)};void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){
return Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Absyn_mk,tenv);}struct Cyc_Absyn_Tqual
Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){return({
struct Cyc_Absyn_Tqual _tmp18;_tmp18.print_const=x.print_const  || y.print_const;
_tmp18.q_volatile=x.q_volatile  || y.q_volatile;_tmp18.q_restrict=x.q_restrict
 || y.q_restrict;_tmp18.real_const=x.real_const  || y.real_const;_tmp18.loc=Cyc_Position_segment_join(
x.loc,y.loc);_tmp18;});}struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*
loc){return({struct Cyc_Absyn_Tqual _tmp19;_tmp19.print_const=0;_tmp19.q_volatile=
0;_tmp19.q_restrict=0;_tmp19.real_const=0;_tmp19.loc=loc;_tmp19;});}struct Cyc_Absyn_Tqual
Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*loc){return({struct Cyc_Absyn_Tqual
_tmp1A;_tmp1A.print_const=1;_tmp1A.q_volatile=0;_tmp1A.q_restrict=0;_tmp1A.real_const=
1;_tmp1A.loc=loc;_tmp1A;});}static struct Cyc_Absyn_Const_e_struct Cyc_Absyn_one_b_raw={
0,(union Cyc_Absyn_Cnst_union)((struct Cyc_Absyn_Int_c_struct){2,(void*)((void*)1),
1})};struct Cyc_Absyn_Exp Cyc_Absyn_exp_unsigned_one_v={0,(void*)((void*)& Cyc_Absyn_one_b_raw),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one=&
Cyc_Absyn_exp_unsigned_one_v;static struct Cyc_Absyn_Upper_b_struct Cyc_Absyn_one_bt={
0,& Cyc_Absyn_exp_unsigned_one_v};void*Cyc_Absyn_bounds_one=(void*)& Cyc_Absyn_one_bt;
struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x){return({struct Cyc_Absyn_Conref*
_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->v=(union Cyc_Absyn_Constraint_union)({
union Cyc_Absyn_Constraint_union _tmp1F;(_tmp1F.Eq_constr).tag=0;(_tmp1F.Eq_constr).f1=(
void*)x;_tmp1F;});_tmp1E;});}struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref(){
return({struct Cyc_Absyn_Conref*_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20->v=(
union Cyc_Absyn_Constraint_union)({union Cyc_Absyn_Constraint_union _tmp21;(_tmp21.No_constr).tag=
2;_tmp21;});_tmp20;});}static struct Cyc_Absyn_Conref Cyc_Absyn_true_conref_v={(
union Cyc_Absyn_Constraint_union)((struct Cyc_Absyn_Eq_constr_struct){0,(void*)1})};
struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref=& Cyc_Absyn_true_conref_v;static
struct Cyc_Absyn_Conref Cyc_Absyn_false_conref_v={(union Cyc_Absyn_Constraint_union)((
struct Cyc_Absyn_Eq_constr_struct){0,(void*)0})};struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref=&
Cyc_Absyn_false_conref_v;static struct Cyc_Absyn_Conref Cyc_Absyn_bounds_one_conref_v={(
union Cyc_Absyn_Constraint_union)((struct Cyc_Absyn_Eq_constr_struct){0,(void*)((
void*)& Cyc_Absyn_one_bt)})};struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref=&
Cyc_Absyn_bounds_one_conref_v;static struct Cyc_Absyn_Conref Cyc_Absyn_bounds_dyneither_conref_v={(
union Cyc_Absyn_Constraint_union)((struct Cyc_Absyn_Eq_constr_struct){0,(void*)((
void*)0)})};struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_dyneither_conref=& Cyc_Absyn_bounds_dyneither_conref_v;
struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x){union
Cyc_Absyn_Constraint_union _tmp26=x->v;struct Cyc_Absyn_Conref*_tmp27;_LL19: if((
_tmp26.No_constr).tag != 2)goto _LL1B;_LL1A: goto _LL1C;_LL1B: if((_tmp26.Eq_constr).tag
!= 0)goto _LL1D;_LL1C: return x;_LL1D: if((_tmp26.Forward_constr).tag != 1)goto _LL18;
_tmp27=(_tmp26.Forward_constr).f1;_LL1E: {struct Cyc_Absyn_Conref*_tmp28=Cyc_Absyn_compress_conref(
_tmp27);x->v=(union Cyc_Absyn_Constraint_union)({union Cyc_Absyn_Constraint_union
_tmp29;(_tmp29.Forward_constr).tag=1;(_tmp29.Forward_constr).f1=_tmp28;_tmp29;});
return _tmp28;}_LL18:;}void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x){union
Cyc_Absyn_Constraint_union _tmp2A=(Cyc_Absyn_compress_conref(x))->v;void*_tmp2B;
_LL20: if((_tmp2A.Eq_constr).tag != 0)goto _LL22;_tmp2B=(_tmp2A.Eq_constr).f1;_LL21:
return _tmp2B;_LL22:;_LL23:({void*_tmp2C=0;Cyc_Tcutil_impos(({const char*_tmp2D="conref_val";
_tag_dyneither(_tmp2D,sizeof(char),11);}),_tag_dyneither(_tmp2C,sizeof(void*),0));});
_LL1F:;}void*Cyc_Absyn_conref_def(void*y,struct Cyc_Absyn_Conref*x){union Cyc_Absyn_Constraint_union
_tmp2E=(Cyc_Absyn_compress_conref(x))->v;void*_tmp2F;_LL25: if((_tmp2E.Eq_constr).tag
!= 0)goto _LL27;_tmp2F=(_tmp2E.Eq_constr).f1;_LL26: return _tmp2F;_LL27:;_LL28:
return y;_LL24:;}void*Cyc_Absyn_compress_kb(void*k){void*_tmp30=k;struct Cyc_Core_Opt*
_tmp31;struct Cyc_Core_Opt*_tmp32;struct Cyc_Core_Opt*_tmp33;struct Cyc_Core_Opt
_tmp34;void*_tmp35;void**_tmp36;struct Cyc_Core_Opt*_tmp37;struct Cyc_Core_Opt
_tmp38;void*_tmp39;void**_tmp3A;_LL2A: if(*((int*)_tmp30)!= 0)goto _LL2C;_LL2B:
goto _LL2D;_LL2C: if(*((int*)_tmp30)!= 1)goto _LL2E;_tmp31=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp30)->f1;if(_tmp31 != 0)goto _LL2E;_LL2D: goto _LL2F;_LL2E: if(*((int*)_tmp30)!= 2)
goto _LL30;_tmp32=((struct Cyc_Absyn_Less_kb_struct*)_tmp30)->f1;if(_tmp32 != 0)
goto _LL30;_LL2F: return k;_LL30: if(*((int*)_tmp30)!= 1)goto _LL32;_tmp33=((struct
Cyc_Absyn_Unknown_kb_struct*)_tmp30)->f1;if(_tmp33 == 0)goto _LL32;_tmp34=*_tmp33;
_tmp35=(void*)_tmp34.v;_tmp36=(void**)&(*((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp30)->f1).v;_LL31: _tmp3A=_tmp36;goto _LL33;_LL32: if(*((int*)_tmp30)!= 2)goto
_LL29;_tmp37=((struct Cyc_Absyn_Less_kb_struct*)_tmp30)->f1;if(_tmp37 == 0)goto
_LL29;_tmp38=*_tmp37;_tmp39=(void*)_tmp38.v;_tmp3A=(void**)&(*((struct Cyc_Absyn_Less_kb_struct*)
_tmp30)->f1).v;_LL33:*_tmp3A=Cyc_Absyn_compress_kb(*_tmp3A);return*_tmp3A;_LL29:;}
void*Cyc_Absyn_force_kb(void*kb){void*_tmp3B=Cyc_Absyn_compress_kb(kb);void*
_tmp3C;struct Cyc_Core_Opt*_tmp3D;struct Cyc_Core_Opt**_tmp3E;struct Cyc_Core_Opt*
_tmp3F;struct Cyc_Core_Opt**_tmp40;void*_tmp41;_LL35: if(*((int*)_tmp3B)!= 0)goto
_LL37;_tmp3C=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp3B)->f1;_LL36: return
_tmp3C;_LL37: if(*((int*)_tmp3B)!= 1)goto _LL39;_tmp3D=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp3B)->f1;_tmp3E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp3B)->f1;_LL38: _tmp40=_tmp3E;_tmp41=(void*)2;goto _LL3A;_LL39: if(*((int*)
_tmp3B)!= 2)goto _LL34;_tmp3F=((struct Cyc_Absyn_Less_kb_struct*)_tmp3B)->f1;
_tmp40=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp3B)->f1;
_tmp41=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp3B)->f2;_LL3A:*_tmp40=({
struct Cyc_Core_Opt*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->v=(void*)Cyc_Tcutil_kind_to_bound(
_tmp41);_tmp42;});return _tmp41;_LL34:;}static struct Cyc_Absyn_IntType_struct Cyc_Absyn_char_tt={
5,(void*)((void*)2),(void*)((void*)0)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_uchar_tt={
5,(void*)((void*)1),(void*)((void*)0)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_ushort_tt={
5,(void*)((void*)1),(void*)((void*)1)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_uint_tt={
5,(void*)((void*)1),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_ulong_tt={
5,(void*)((void*)1),(void*)((void*)3)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_ulonglong_tt={
5,(void*)((void*)1),(void*)((void*)4)};void*Cyc_Absyn_char_typ=(void*)& Cyc_Absyn_char_tt;
void*Cyc_Absyn_uchar_typ=(void*)& Cyc_Absyn_uchar_tt;void*Cyc_Absyn_ushort_typ=(
void*)& Cyc_Absyn_ushort_tt;void*Cyc_Absyn_uint_typ=(void*)& Cyc_Absyn_uint_tt;
void*Cyc_Absyn_ulong_typ=(void*)& Cyc_Absyn_ulong_tt;void*Cyc_Absyn_ulonglong_typ=(
void*)& Cyc_Absyn_ulonglong_tt;static struct Cyc_Absyn_IntType_struct Cyc_Absyn_schar_tt={
5,(void*)((void*)0),(void*)((void*)0)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_sshort_tt={
5,(void*)((void*)0),(void*)((void*)1)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_sint_tt={
5,(void*)((void*)0),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_slong_tt={
5,(void*)((void*)0),(void*)((void*)3)};static struct Cyc_Absyn_IntType_struct Cyc_Absyn_slonglong_tt={
5,(void*)((void*)0),(void*)((void*)4)};void*Cyc_Absyn_schar_typ=(void*)& Cyc_Absyn_schar_tt;
void*Cyc_Absyn_sshort_typ=(void*)& Cyc_Absyn_sshort_tt;void*Cyc_Absyn_sint_typ=(
void*)& Cyc_Absyn_sint_tt;void*Cyc_Absyn_slong_typ=(void*)& Cyc_Absyn_slong_tt;
void*Cyc_Absyn_slonglong_typ=(void*)& Cyc_Absyn_slonglong_tt;static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_nshort_tt={5,(void*)((void*)2),(void*)((void*)1)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_nint_tt={5,(void*)((void*)2),(void*)((void*)2)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_nlong_tt={5,(void*)((void*)2),(void*)((void*)3)};static struct Cyc_Absyn_IntType_struct
Cyc_Absyn_nlonglong_tt={5,(void*)((void*)2),(void*)((void*)4)};void*Cyc_Absyn_nshort_typ=(
void*)& Cyc_Absyn_nshort_tt;void*Cyc_Absyn_nint_typ=(void*)& Cyc_Absyn_nint_tt;
void*Cyc_Absyn_nlong_typ=(void*)& Cyc_Absyn_nlong_tt;void*Cyc_Absyn_nlonglong_typ=(
void*)& Cyc_Absyn_nlonglong_tt;void*Cyc_Absyn_int_typ(void*sn,void*sz){void*
_tmp52=sn;_LL3C: if((int)_tmp52 != 0)goto _LL3E;_LL3D: {void*_tmp53=sz;_LL43: if((
int)_tmp53 != 0)goto _LL45;_LL44: return Cyc_Absyn_schar_typ;_LL45: if((int)_tmp53 != 
1)goto _LL47;_LL46: return Cyc_Absyn_sshort_typ;_LL47: if((int)_tmp53 != 2)goto _LL49;
_LL48: return Cyc_Absyn_sint_typ;_LL49: if((int)_tmp53 != 3)goto _LL4B;_LL4A: return
Cyc_Absyn_slong_typ;_LL4B: if((int)_tmp53 != 4)goto _LL42;_LL4C: return Cyc_Absyn_slonglong_typ;
_LL42:;}_LL3E: if((int)_tmp52 != 1)goto _LL40;_LL3F: {void*_tmp54=sz;_LL4E: if((int)
_tmp54 != 0)goto _LL50;_LL4F: return Cyc_Absyn_uchar_typ;_LL50: if((int)_tmp54 != 1)
goto _LL52;_LL51: return Cyc_Absyn_ushort_typ;_LL52: if((int)_tmp54 != 2)goto _LL54;
_LL53: return Cyc_Absyn_uint_typ;_LL54: if((int)_tmp54 != 3)goto _LL56;_LL55: return
Cyc_Absyn_ulong_typ;_LL56: if((int)_tmp54 != 4)goto _LL4D;_LL57: return Cyc_Absyn_ulonglong_typ;
_LL4D:;}_LL40: if((int)_tmp52 != 2)goto _LL3B;_LL41: {void*_tmp55=sz;_LL59: if((int)
_tmp55 != 0)goto _LL5B;_LL5A: return Cyc_Absyn_char_typ;_LL5B: if((int)_tmp55 != 1)
goto _LL5D;_LL5C: return Cyc_Absyn_nshort_typ;_LL5D: if((int)_tmp55 != 2)goto _LL5F;
_LL5E: return Cyc_Absyn_nint_typ;_LL5F: if((int)_tmp55 != 3)goto _LL61;_LL60: return
Cyc_Absyn_nlong_typ;_LL61: if((int)_tmp55 != 4)goto _LL58;_LL62: return Cyc_Absyn_nlonglong_typ;
_LL58:;}_LL3B:;}void*Cyc_Absyn_float_typ=(void*)1;void*Cyc_Absyn_double_typ(int b){
static struct Cyc_Absyn_DoubleType_struct dt={6,1};static struct Cyc_Absyn_DoubleType_struct
df={6,0};return(void*)(b?& dt:& df);}static char _tmp58[4]="exn";static struct
_dyneither_ptr Cyc_Absyn_exn_str={_tmp58,_tmp58,_tmp58 + 4};static struct _tuple0 Cyc_Absyn_exn_name_v={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Abs_n_struct){2,0}),& Cyc_Absyn_exn_str};
struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;static char _tmp5A[15]="Null_Exception";
static struct _dyneither_ptr Cyc_Absyn_Null_Exception_str={_tmp5A,_tmp5A,_tmp5A + 15};
static struct _tuple0 Cyc_Absyn_Null_Exception_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Abs_n_struct){2,0}),& Cyc_Absyn_Null_Exception_str};struct _tuple0*
Cyc_Absyn_Null_Exception_name=& Cyc_Absyn_Null_Exception_pr;static struct Cyc_Absyn_Tunionfield
Cyc_Absyn_Null_Exception_tuf_v={& Cyc_Absyn_Null_Exception_pr,0,0,(void*)((void*)
3)};struct Cyc_Absyn_Tunionfield*Cyc_Absyn_Null_Exception_tuf=& Cyc_Absyn_Null_Exception_tuf_v;
static char _tmp5C[13]="Array_bounds";static struct _dyneither_ptr Cyc_Absyn_Array_bounds_str={
_tmp5C,_tmp5C,_tmp5C + 13};static struct _tuple0 Cyc_Absyn_Array_bounds_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Abs_n_struct){2,0}),& Cyc_Absyn_Array_bounds_str};
struct _tuple0*Cyc_Absyn_Array_bounds_name=& Cyc_Absyn_Array_bounds_pr;static
struct Cyc_Absyn_Tunionfield Cyc_Absyn_Array_bounds_tuf_v={& Cyc_Absyn_Array_bounds_pr,
0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*Cyc_Absyn_Array_bounds_tuf=&
Cyc_Absyn_Array_bounds_tuf_v;static char _tmp5E[16]="Match_Exception";static struct
_dyneither_ptr Cyc_Absyn_Match_Exception_str={_tmp5E,_tmp5E,_tmp5E + 16};static
struct _tuple0 Cyc_Absyn_Match_Exception_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Abs_n_struct){2,0}),& Cyc_Absyn_Match_Exception_str};struct
_tuple0*Cyc_Absyn_Match_Exception_name=& Cyc_Absyn_Match_Exception_pr;static
struct Cyc_Absyn_Tunionfield Cyc_Absyn_Match_Exception_tuf_v={& Cyc_Absyn_Match_Exception_pr,
0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*Cyc_Absyn_Match_Exception_tuf=&
Cyc_Absyn_Match_Exception_tuf_v;static char _tmp60[10]="Bad_alloc";static struct
_dyneither_ptr Cyc_Absyn_Bad_alloc_str={_tmp60,_tmp60,_tmp60 + 10};static struct
_tuple0 Cyc_Absyn_Bad_alloc_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Abs_n_struct){
2,0}),& Cyc_Absyn_Bad_alloc_str};struct _tuple0*Cyc_Absyn_Bad_alloc_name=& Cyc_Absyn_Bad_alloc_pr;
static struct Cyc_Absyn_Tunionfield Cyc_Absyn_Bad_alloc_tuf_v={& Cyc_Absyn_Bad_alloc_pr,
0,0,(void*)((void*)3)};struct Cyc_Absyn_Tunionfield*Cyc_Absyn_Bad_alloc_tuf=& Cyc_Absyn_Bad_alloc_tuf_v;
static struct Cyc_List_List Cyc_Absyn_exn_l0={(void*)& Cyc_Absyn_Null_Exception_tuf_v,
0};static struct Cyc_List_List Cyc_Absyn_exn_l1={(void*)& Cyc_Absyn_Array_bounds_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l0};static struct Cyc_List_List Cyc_Absyn_exn_l2={(
void*)& Cyc_Absyn_Match_Exception_tuf_v,(struct Cyc_List_List*)& Cyc_Absyn_exn_l1};
static struct Cyc_List_List Cyc_Absyn_exn_l3={(void*)& Cyc_Absyn_Bad_alloc_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l2};static struct Cyc_Core_Opt Cyc_Absyn_exn_ol={(
void*)((struct Cyc_List_List*)& Cyc_Absyn_exn_l3)};static struct Cyc_Absyn_Tuniondecl
Cyc_Absyn_exn_tud_v={(void*)((void*)3),& Cyc_Absyn_exn_name_v,0,(struct Cyc_Core_Opt*)&
Cyc_Absyn_exn_ol,1,0};struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud=& Cyc_Absyn_exn_tud_v;
static struct Cyc_Core_Opt Cyc_Absyn_heap_opt={(void*)((void*)2)};static struct Cyc_Absyn_TunionType_struct
Cyc_Absyn_exn_typ_tt={2,{(union Cyc_Absyn_TunionInfoU_union)((struct Cyc_Absyn_KnownTunion_struct){
1,& Cyc_Absyn_exn_tud}),0,(struct Cyc_Core_Opt*)& Cyc_Absyn_heap_opt}};void*Cyc_Absyn_exn_typ=(
void*)& Cyc_Absyn_exn_typ_tt;static char _tmp64[9]="PrintArg";static struct
_dyneither_ptr Cyc_Absyn_printarg_str={_tmp64,_tmp64,_tmp64 + 9};static char _tmp65[
9]="ScanfArg";static struct _dyneither_ptr Cyc_Absyn_scanfarg_str={_tmp65,_tmp65,
_tmp65 + 9};static struct _tuple0 Cyc_Absyn_tunion_print_arg_qvar_p={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Abs_n_struct){2,0}),& Cyc_Absyn_printarg_str};static struct _tuple0
Cyc_Absyn_tunion_scanf_arg_qvar_p={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Abs_n_struct){
2,0}),& Cyc_Absyn_scanfarg_str};struct _tuple0*Cyc_Absyn_tunion_print_arg_qvar=&
Cyc_Absyn_tunion_print_arg_qvar_p;struct _tuple0*Cyc_Absyn_tunion_scanf_arg_qvar=&
Cyc_Absyn_tunion_scanf_arg_qvar_p;static void**Cyc_Absyn_string_t_opt=0;void*Cyc_Absyn_string_typ(
void*rgn){if(rgn != (void*)2)return Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_empty_tqual(
0),(void*)0,Cyc_Absyn_true_conref);if(Cyc_Absyn_string_t_opt == 0){void*t=Cyc_Absyn_starb_typ(
Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_empty_tqual(0),(void*)0,Cyc_Absyn_true_conref);
Cyc_Absyn_string_t_opt=({void**_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68[0]=t;
_tmp68;});}return*((void**)_check_null(Cyc_Absyn_string_t_opt));}static void**Cyc_Absyn_const_string_t_opt=
0;void*Cyc_Absyn_const_string_typ(void*rgn){if(rgn != (void*)2)return Cyc_Absyn_starb_typ(
Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)0,Cyc_Absyn_true_conref);
if(Cyc_Absyn_const_string_t_opt == 0){void*t=Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Absyn_const_tqual(0),(void*)0,Cyc_Absyn_true_conref);Cyc_Absyn_const_string_t_opt=({
void**_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69[0]=t;_tmp69;});}return*((void**)
_check_null(Cyc_Absyn_const_string_t_opt));}void*Cyc_Absyn_starb_typ(void*t,void*
r,struct Cyc_Absyn_Tqual tq,void*b,struct Cyc_Absyn_Conref*zeroterm){return(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A[0]=({
struct Cyc_Absyn_PointerType_struct _tmp6B;_tmp6B.tag=4;_tmp6B.f1=({struct Cyc_Absyn_PtrInfo
_tmp6C;_tmp6C.elt_typ=(void*)t;_tmp6C.elt_tq=tq;_tmp6C.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp6D;_tmp6D.rgn=(void*)r;_tmp6D.nullable=Cyc_Absyn_true_conref;_tmp6D.bounds=
Cyc_Absyn_new_conref(b);_tmp6D.zero_term=zeroterm;_tmp6D.ptrloc=0;_tmp6D;});
_tmp6C;});_tmp6B;});_tmp6A;});}void*Cyc_Absyn_atb_typ(void*t,void*r,struct Cyc_Absyn_Tqual
tq,void*b,struct Cyc_Absyn_Conref*zeroterm){return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E[0]=({struct Cyc_Absyn_PointerType_struct
_tmp6F;_tmp6F.tag=4;_tmp6F.f1=({struct Cyc_Absyn_PtrInfo _tmp70;_tmp70.elt_typ=(
void*)t;_tmp70.elt_tq=tq;_tmp70.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp71;_tmp71.rgn=(
void*)r;_tmp71.nullable=Cyc_Absyn_false_conref;_tmp71.bounds=Cyc_Absyn_new_conref(
b);_tmp71.zero_term=zeroterm;_tmp71.ptrloc=0;_tmp71;});_tmp70;});_tmp6F;});
_tmp6E;});}void*Cyc_Absyn_star_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Conref*zeroterm){return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72[0]=({struct Cyc_Absyn_PointerType_struct
_tmp73;_tmp73.tag=4;_tmp73.f1=({struct Cyc_Absyn_PtrInfo _tmp74;_tmp74.elt_typ=(
void*)t;_tmp74.elt_tq=tq;_tmp74.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp75;_tmp75.rgn=(
void*)r;_tmp75.nullable=Cyc_Absyn_true_conref;_tmp75.bounds=Cyc_Absyn_bounds_one_conref;
_tmp75.zero_term=zeroterm;_tmp75.ptrloc=0;_tmp75;});_tmp74;});_tmp73;});_tmp72;});}
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq){return(void*)({struct
Cyc_Absyn_PointerType_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76[0]=({
struct Cyc_Absyn_PointerType_struct _tmp77;_tmp77.tag=4;_tmp77.f1=({struct Cyc_Absyn_PtrInfo
_tmp78;_tmp78.elt_typ=(void*)t;_tmp78.elt_tq=tq;_tmp78.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp79;_tmp79.rgn=(void*)((void*)2);_tmp79.nullable=Cyc_Absyn_true_conref;_tmp79.bounds=
Cyc_Absyn_bounds_one_conref;_tmp79.zero_term=Cyc_Absyn_false_conref;_tmp79.ptrloc=
0;_tmp79;});_tmp78;});_tmp77;});_tmp76;});}void*Cyc_Absyn_at_typ(void*t,void*r,
struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zeroterm){return(void*)({struct
Cyc_Absyn_PointerType_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A[0]=({
struct Cyc_Absyn_PointerType_struct _tmp7B;_tmp7B.tag=4;_tmp7B.f1=({struct Cyc_Absyn_PtrInfo
_tmp7C;_tmp7C.elt_typ=(void*)t;_tmp7C.elt_tq=tq;_tmp7C.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp7D;_tmp7D.rgn=(void*)r;_tmp7D.nullable=Cyc_Absyn_false_conref;_tmp7D.bounds=
Cyc_Absyn_bounds_one_conref;_tmp7D.zero_term=zeroterm;_tmp7D.ptrloc=0;_tmp7D;});
_tmp7C;});_tmp7B;});_tmp7A;});}void*Cyc_Absyn_dyneither_typ(void*t,void*r,struct
Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zeroterm){return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E[0]=({struct Cyc_Absyn_PointerType_struct
_tmp7F;_tmp7F.tag=4;_tmp7F.f1=({struct Cyc_Absyn_PtrInfo _tmp80;_tmp80.elt_typ=(
void*)t;_tmp80.elt_tq=tq;_tmp80.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp81;_tmp81.rgn=(
void*)r;_tmp81.nullable=Cyc_Absyn_true_conref;_tmp81.bounds=Cyc_Absyn_bounds_dyneither_conref;
_tmp81.zero_term=zeroterm;_tmp81.ptrloc=0;_tmp81;});_tmp80;});_tmp7F;});_tmp7E;});}
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*ztloc){
return(void*)({struct Cyc_Absyn_ArrayType_struct*_tmp82=_cycalloc(sizeof(*_tmp82));
_tmp82[0]=({struct Cyc_Absyn_ArrayType_struct _tmp83;_tmp83.tag=7;_tmp83.f1=({
struct Cyc_Absyn_ArrayInfo _tmp84;_tmp84.elt_type=(void*)elt_type;_tmp84.tq=tq;
_tmp84.num_elts=num_elts;_tmp84.zero_term=zero_term;_tmp84.zt_loc=ztloc;_tmp84;});
_tmp83;});_tmp82;});}static char _tmp8E[8]="__sFILE";void*Cyc_Absyn_file_typ(){
static void**file_t_opt=0;static struct _dyneither_ptr sf_str={_tmp8E,_tmp8E,_tmp8E + 
8};static struct _dyneither_ptr*sf=& sf_str;if(file_t_opt == 0){struct _tuple0*
file_t_name=({struct _tuple0*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->f1=(union
Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp8D;(_tmp8D.Abs_n).tag=
2;(_tmp8D.Abs_n).f1=0;_tmp8D;});_tmp8C->f2=sf;_tmp8C;});struct Cyc_Absyn_Aggrdecl*
sd=({struct Cyc_Absyn_Aggrdecl*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->kind=(
void*)((void*)0);_tmp8B->sc=(void*)((void*)1);_tmp8B->name=file_t_name;_tmp8B->tvs=
0;_tmp8B->impl=0;_tmp8B->attributes=0;_tmp8B;});void*file_struct_typ=(void*)({
struct Cyc_Absyn_AggrType_struct*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86[0]=({
struct Cyc_Absyn_AggrType_struct _tmp87;_tmp87.tag=10;_tmp87.f1=({struct Cyc_Absyn_AggrInfo
_tmp88;_tmp88.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union
_tmp89;(_tmp89.KnownAggr).tag=1;(_tmp89.KnownAggr).f1=({struct Cyc_Absyn_Aggrdecl**
_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A[0]=sd;_tmp8A;});_tmp89;});_tmp88.targs=
0;_tmp88;});_tmp87;});_tmp86;});file_t_opt=({void**_tmp85=_cycalloc(sizeof(*
_tmp85));_tmp85[0]=Cyc_Absyn_at_typ(file_struct_typ,(void*)2,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_false_conref);_tmp85;});}return*file_t_opt;}void*Cyc_Absyn_void_star_typ(){
static void**void_star_t_opt=0;if(void_star_t_opt == 0)void_star_t_opt=({void**
_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F[0]=Cyc_Absyn_star_typ((void*)0,(void*)2,
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);_tmp8F;});return*
void_star_t_opt;}static struct Cyc_Absyn_JoinEff_struct Cyc_Absyn_empty_eff={20,0};
void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_eff;void*Cyc_Absyn_aggr_typ(
void*k,struct _dyneither_ptr*name){return(void*)({struct Cyc_Absyn_AggrType_struct*
_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91[0]=({struct Cyc_Absyn_AggrType_struct
_tmp92;_tmp92.tag=10;_tmp92.f1=({struct Cyc_Absyn_AggrInfo _tmp93;_tmp93.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmp94;(_tmp94.UnknownAggr).tag=
0;(_tmp94.UnknownAggr).f1=(void*)k;(_tmp94.UnknownAggr).f2=({struct _tuple0*
_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->f1=Cyc_Absyn_rel_ns_null;_tmp95->f2=
name;_tmp95;});_tmp94;});_tmp93.targs=0;_tmp93;});_tmp92;});_tmp91;});}void*Cyc_Absyn_strct(
struct _dyneither_ptr*name){return Cyc_Absyn_aggr_typ((void*)0,name);}void*Cyc_Absyn_union_typ(
struct _dyneither_ptr*name){return Cyc_Absyn_aggr_typ((void*)1,name);}void*Cyc_Absyn_strctq(
struct _tuple0*name){return(void*)({struct Cyc_Absyn_AggrType_struct*_tmp96=
_cycalloc(sizeof(*_tmp96));_tmp96[0]=({struct Cyc_Absyn_AggrType_struct _tmp97;
_tmp97.tag=10;_tmp97.f1=({struct Cyc_Absyn_AggrInfo _tmp98;_tmp98.aggr_info=(union
Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmp99;(_tmp99.UnknownAggr).tag=
0;(_tmp99.UnknownAggr).f1=(void*)((void*)0);(_tmp99.UnknownAggr).f2=name;_tmp99;});
_tmp98.targs=0;_tmp98;});_tmp97;});_tmp96;});}void*Cyc_Absyn_unionq_typ(struct
_tuple0*name){return(void*)({struct Cyc_Absyn_AggrType_struct*_tmp9A=_cycalloc(
sizeof(*_tmp9A));_tmp9A[0]=({struct Cyc_Absyn_AggrType_struct _tmp9B;_tmp9B.tag=10;
_tmp9B.f1=({struct Cyc_Absyn_AggrInfo _tmp9C;_tmp9C.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({
union Cyc_Absyn_AggrInfoU_union _tmp9D;(_tmp9D.UnknownAggr).tag=0;(_tmp9D.UnknownAggr).f1=(
void*)((void*)1);(_tmp9D.UnknownAggr).f2=name;_tmp9D;});_tmp9C.targs=0;_tmp9C;});
_tmp9B;});_tmp9A;});}struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*r,struct Cyc_Position_Segment*
loc){return({struct Cyc_Absyn_Exp*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->topt=
0;_tmp9E->r=(void*)r;_tmp9E->loc=loc;_tmp9E->annot=(void*)((void*)Cyc_Absyn_EmptyAnnot);
_tmp9E;});}struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_New_e_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F[0]=({
struct Cyc_Absyn_New_e_struct _tmpA0;_tmpA0.tag=17;_tmpA0.f1=rgn_handle;_tmpA0.f2=
e;_tmpA0;});_tmp9F;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*
e){return({struct Cyc_Absyn_Exp*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1[0]=*e;
_tmpA1;});}struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst_union c,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Const_e_struct*
_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2[0]=({struct Cyc_Absyn_Const_e_struct
_tmpA3;_tmpA3.tag=0;_tmpA3.f1=c;_tmpA3;});_tmpA2;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp(struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Const_e_struct*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4[0]=({
struct Cyc_Absyn_Const_e_struct _tmpA5;_tmpA5.tag=0;_tmpA5.f1=(union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmpA6;(_tmpA6.Null_c).tag=6;_tmpA6;});_tmpA5;});_tmpA4;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*s,int i,struct Cyc_Position_Segment*
seg){return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmpA7;(_tmpA7.Int_c).tag=2;(_tmpA7.Int_c).f1=(void*)s;(_tmpA7.Int_c).f2=i;
_tmpA7;}),seg);}struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int i,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_int_exp((void*)0,i,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int i,struct Cyc_Position_Segment*loc){return Cyc_Absyn_int_exp((void*)1,(
int)i,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int b,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(
struct Cyc_Position_Segment*loc){return Cyc_Absyn_bool_exp(1,loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_false_exp(struct Cyc_Position_Segment*loc){return Cyc_Absyn_bool_exp(0,
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmpA8;(_tmpA8.Char_c).tag=0;(_tmpA8.Char_c).f1=(void*)((void*)2);(_tmpA8.Char_c).f2=
c;_tmpA8;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmpA9;(_tmpA9.Float_c).tag=4;(_tmpA9.Float_c).f1=f;
_tmpA9;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmpAA;(_tmpAA.String_c).tag=5;(_tmpAA.String_c).f1=s;
_tmpAA;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*q,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Var_e_struct*_tmpAB=
_cycalloc(sizeof(*_tmpAB));_tmpAB[0]=({struct Cyc_Absyn_Var_e_struct _tmpAC;_tmpAC.tag=
1;_tmpAC.f1=q;_tmpAC.f2=(void*)((void*)0);_tmpAC;});_tmpAB;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_varb_exp(struct _tuple0*q,void*b,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Var_e_struct*_tmpAD=_cycalloc(sizeof(*
_tmpAD));_tmpAD[0]=({struct Cyc_Absyn_Var_e_struct _tmpAE;_tmpAE.tag=1;_tmpAE.f1=q;
_tmpAE.f2=(void*)b;_tmpAE;});_tmpAD;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(
struct _tuple0*q,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_UnknownId_e_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmpB0;_tmpB0.tag=2;_tmpB0.f1=q;_tmpB0;});
_tmpAF;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(void*p,struct Cyc_List_List*
es,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1[0]=({struct Cyc_Absyn_Primop_e_struct
_tmpB2;_tmpB2.tag=3;_tmpB2.f1=(void*)p;_tmpB2.f2=es;_tmpB2;});_tmpB1;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(void*p,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_primop_exp(p,({struct Cyc_List_List*_tmpB3=_cycalloc(sizeof(*
_tmpB3));_tmpB3->hd=e;_tmpB3->tl=0;_tmpB3;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_primop_exp(p,({struct Cyc_List_List*_tmpB4=_cycalloc(sizeof(*
_tmpB4));_tmpB4->hd=e1;_tmpB4->tl=({struct Cyc_List_List*_tmpB5=_cycalloc(sizeof(*
_tmpB5));_tmpB5->hd=e2;_tmpB5->tl=0;_tmpB5;});_tmpB4;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_prim2_exp((void*)0,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)1,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)3,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)5,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)6,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)7,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)8,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)9,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_prim2_exp((void*)10,e1,e2,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AssignOp_e_struct*_tmpB6=
_cycalloc(sizeof(*_tmpB6));_tmpB6[0]=({struct Cyc_Absyn_AssignOp_e_struct _tmpB7;
_tmpB7.tag=4;_tmpB7.f1=e1;_tmpB7.f2=popt;_tmpB7.f3=e2;_tmpB7;});_tmpB6;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,void*i,struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_struct*
_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8[0]=({struct Cyc_Absyn_Increment_e_struct
_tmpB9;_tmpB9.tag=5;_tmpB9.f1=e;_tmpB9.f2=(void*)i;_tmpB9;});_tmpB8;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,(void*)1,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(
e,(void*)0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(e,(void*)2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,(void*)3,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Conditional_e_struct*
_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA[0]=({struct Cyc_Absyn_Conditional_e_struct
_tmpBB;_tmpBB.tag=6;_tmpBB.f1=e1;_tmpBB.f2=e2;_tmpBB.f3=e3;_tmpBB;});_tmpBA;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_And_e_struct*
_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=({struct Cyc_Absyn_And_e_struct _tmpBD;
_tmpBD.tag=7;_tmpBD.f1=e1;_tmpBD.f2=e2;_tmpBD;});_tmpBC;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Or_e_struct*_tmpBE=
_cycalloc(sizeof(*_tmpBE));_tmpBE[0]=({struct Cyc_Absyn_Or_e_struct _tmpBF;_tmpBF.tag=
8;_tmpBF.f1=e1;_tmpBF.f2=e2;_tmpBF;});_tmpBE;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_SeqExp_e_struct*_tmpC0=
_cycalloc(sizeof(*_tmpC0));_tmpC0[0]=({struct Cyc_Absyn_SeqExp_e_struct _tmpC1;
_tmpC1.tag=9;_tmpC1.f1=e1;_tmpC1.f2=e2;_tmpC1;});_tmpC0;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownCall_e_struct*
_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2[0]=({struct Cyc_Absyn_UnknownCall_e_struct
_tmpC3;_tmpC3.tag=10;_tmpC3.f1=e;_tmpC3.f2=es;_tmpC3;});_tmpC2;}),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmpC5;_tmpC5.tag=11;_tmpC5.f1=e;_tmpC5.f2=es;_tmpC5.f3=0;_tmpC5;});_tmpC4;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_NoInstantiate_e_struct
_tmpC7;_tmpC7.tag=13;_tmpC7.f1=e;_tmpC7;});_tmpC6;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Instantiate_e_struct*
_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8[0]=({struct Cyc_Absyn_Instantiate_e_struct
_tmpC9;_tmpC9.tag=14;_tmpC9.f1=e;_tmpC9.f2=ts;_tmpC9;});_tmpC8;}),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,int user_cast,void*
c,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Cast_e_struct*
_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA[0]=({struct Cyc_Absyn_Cast_e_struct
_tmpCB;_tmpCB.tag=15;_tmpCB.f1=(void*)t;_tmpCB.f2=e;_tmpCB.f3=user_cast;_tmpCB.f4=(
void*)c;_tmpCB;});_tmpCA;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Throw_e_struct*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC[0]=({
struct Cyc_Absyn_Throw_e_struct _tmpCD;_tmpCD.tag=12;_tmpCD.f1=e;_tmpCD;});_tmpCC;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Address_e_struct*_tmpCE=
_cycalloc(sizeof(*_tmpCE));_tmpCE[0]=({struct Cyc_Absyn_Address_e_struct _tmpCF;
_tmpCF.tag=16;_tmpCF.f1=e;_tmpCF;});_tmpCE;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Sizeoftyp_e_struct*_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0[0]=({
struct Cyc_Absyn_Sizeoftyp_e_struct _tmpD1;_tmpD1.tag=18;_tmpD1.f1=(void*)t;_tmpD1;});
_tmpD0;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Sizeofexp_e_struct*
_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2[0]=({struct Cyc_Absyn_Sizeofexp_e_struct
_tmpD3;_tmpD3.tag=19;_tmpD3.f1=e;_tmpD3;});_tmpD2;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_offsetof_exp(void*t,void*of,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmpD4=_cycalloc(sizeof(*_tmpD4));
_tmpD4[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmpD5;_tmpD5.tag=20;_tmpD5.f1=(
void*)t;_tmpD5.f2=(void*)of;_tmpD5;});_tmpD4;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(
struct Cyc_List_List*tvs,void*t,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Gentyp_e_struct*_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6[
0]=({struct Cyc_Absyn_Gentyp_e_struct _tmpD7;_tmpD7.tag=21;_tmpD7.f1=tvs;_tmpD7.f2=(
void*)t;_tmpD7;});_tmpD6;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Deref_e_struct*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8[0]=({
struct Cyc_Absyn_Deref_e_struct _tmpD9;_tmpD9.tag=22;_tmpD9.f1=e;_tmpD9;});_tmpD8;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*n,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_AggrMember_e_struct*_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA[0]=({
struct Cyc_Absyn_AggrMember_e_struct _tmpDB;_tmpDB.tag=23;_tmpDB.f1=e;_tmpDB.f2=n;
_tmpDB;});_tmpDA;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*
e,struct _dyneither_ptr*n,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_AggrArrow_e_struct*_tmpDC=_cycalloc(sizeof(*_tmpDC));
_tmpDC[0]=({struct Cyc_Absyn_AggrArrow_e_struct _tmpDD;_tmpDD.tag=24;_tmpDD.f1=e;
_tmpDD.f2=n;_tmpDD;});_tmpDC;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Subscript_e_struct*_tmpDE=
_cycalloc(sizeof(*_tmpDE));_tmpDE[0]=({struct Cyc_Absyn_Subscript_e_struct _tmpDF;
_tmpDF.tag=25;_tmpDF.f1=e1;_tmpDF.f2=e2;_tmpDF;});_tmpDE;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Tuple_e_struct*_tmpE0=_cycalloc(
sizeof(*_tmpE0));_tmpE0[0]=({struct Cyc_Absyn_Tuple_e_struct _tmpE1;_tmpE1.tag=26;
_tmpE1.f1=es;_tmpE1;});_tmpE0;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(
struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_StmtExp_e_struct*_tmpE2=_cycalloc(sizeof(*_tmpE2));
_tmpE2[0]=({struct Cyc_Absyn_StmtExp_e_struct _tmpE3;_tmpE3.tag=38;_tmpE3.f1=s;
_tmpE3;});_tmpE2;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*
loc){return Cyc_Absyn_var_exp(Cyc_Absyn_Match_Exception_name,loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_valueof_exp(void*t,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Valueof_e_struct*_tmpE4=_cycalloc(sizeof(*_tmpE4));
_tmpE4[0]=({struct Cyc_Absyn_Valueof_e_struct _tmpE5;_tmpE5.tag=39;_tmpE5.f1=(void*)
t;_tmpE5;});_tmpE4;}),loc);}struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*es,struct Cyc_Position_Segment*
loc){struct Cyc_List_List*dles=0;for(0;es != 0;es=es->tl){dles=({struct Cyc_List_List*
_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6->hd=({struct _tuple13*_tmpE7=_cycalloc(
sizeof(*_tmpE7));_tmpE7->f1=0;_tmpE7->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmpE7;});
_tmpE6->tl=dles;_tmpE6;});}dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(dles);return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Array_e_struct*
_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8[0]=({struct Cyc_Absyn_Array_e_struct
_tmpE9;_tmpE9.tag=28;_tmpE9.f1=dles;_tmpE9;});_tmpE8;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpEB;_tmpEB.tag=37;_tmpEB.f1=n;_tmpEB.f2=dles;_tmpEB;});_tmpEA;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc){return({
struct Cyc_Absyn_Stmt*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC->r=(void*)s;_tmpEC->loc=
loc;_tmpEC->non_local_preds=0;_tmpEC->try_depth=0;_tmpEC->annot=(void*)((void*)
Cyc_Absyn_EmptyAnnot);_tmpEC;});}struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)0,loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Exp_s_struct*_tmpED=_cycalloc(
sizeof(*_tmpED));_tmpED[0]=({struct Cyc_Absyn_Exp_s_struct _tmpEE;_tmpEE.tag=0;
_tmpEE.f1=e;_tmpEE;});_tmpED;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(
struct Cyc_List_List*ss,struct Cyc_Position_Segment*loc){if(ss == 0)return Cyc_Absyn_skip_stmt(
loc);else{if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)ss->hd;else{return Cyc_Absyn_seq_stmt((
struct Cyc_Absyn_Stmt*)ss->hd,Cyc_Absyn_seq_stmts(ss->tl,loc),loc);}}}struct Cyc_Absyn_Stmt*
Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Return_s_struct*_tmpEF=
_cycalloc(sizeof(*_tmpEF));_tmpEF[0]=({struct Cyc_Absyn_Return_s_struct _tmpF0;
_tmpF0.tag=2;_tmpF0.f1=e;_tmpF0;});_tmpEF;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_IfThenElse_s_struct*
_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1[0]=({struct Cyc_Absyn_IfThenElse_s_struct
_tmpF2;_tmpF2.tag=3;_tmpF2.f1=e;_tmpF2.f2=s1;_tmpF2.f3=s2;_tmpF2;});_tmpF1;}),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_While_s_struct*
_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3[0]=({struct Cyc_Absyn_While_s_struct
_tmpF4;_tmpF4.tag=4;_tmpF4.f1=({struct _tuple2 _tmpF5;_tmpF5.f1=e;_tmpF5.f2=Cyc_Absyn_skip_stmt(
e->loc);_tmpF5;});_tmpF4.f2=s;_tmpF4;});_tmpF3;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Break_s_struct*
_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6[0]=({struct Cyc_Absyn_Break_s_struct
_tmpF7;_tmpF7.tag=5;_tmpF7.f1=0;_tmpF7;});_tmpF6;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Continue_s_struct*_tmpF8=_cycalloc(sizeof(*_tmpF8));
_tmpF8[0]=({struct Cyc_Absyn_Continue_s_struct _tmpF9;_tmpF9.tag=6;_tmpF9.f1=0;
_tmpF9;});_tmpF8;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_For_s_struct*
_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA[0]=({struct Cyc_Absyn_For_s_struct _tmpFB;
_tmpFB.tag=8;_tmpFB.f1=e1;_tmpFB.f2=({struct _tuple2 _tmpFC;_tmpFC.f1=e2;_tmpFC.f2=
Cyc_Absyn_skip_stmt(e3->loc);_tmpFC;});_tmpFB.f3=({struct _tuple2 _tmpFD;_tmpFD.f1=
e3;_tmpFD.f2=Cyc_Absyn_skip_stmt(e3->loc);_tmpFD;});_tmpFB.f4=s;_tmpFB;});_tmpFA;}),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
scs,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_struct*
_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE[0]=({struct Cyc_Absyn_Switch_s_struct
_tmpFF;_tmpFF.tag=9;_tmpFF.f1=e;_tmpFF.f2=scs;_tmpFF;});_tmpFE;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2,struct Cyc_Position_Segment*loc){struct _tuple9 _tmp101=({struct _tuple9 _tmp100;
_tmp100.f1=(void*)s1->r;_tmp100.f2=(void*)s2->r;_tmp100;});void*_tmp102;void*
_tmp103;_LL64: _tmp102=_tmp101.f1;if((int)_tmp102 != 0)goto _LL66;_LL65: return s2;
_LL66: _tmp103=_tmp101.f2;if((int)_tmp103 != 0)goto _LL68;_LL67: return s1;_LL68:;
_LL69: return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Seq_s_struct*_tmp104=
_cycalloc(sizeof(*_tmp104));_tmp104[0]=({struct Cyc_Absyn_Seq_s_struct _tmp105;
_tmp105.tag=1;_tmp105.f1=s1;_tmp105.f2=s2;_tmp105;});_tmp104;}),loc);_LL63:;}
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Fallthru_s_struct*_tmp106=
_cycalloc(sizeof(*_tmp106));_tmp106[0]=({struct Cyc_Absyn_Fallthru_s_struct
_tmp107;_tmp107.tag=10;_tmp107.f1=el;_tmp107.f2=0;_tmp107;});_tmp106;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108[0]=({struct Cyc_Absyn_Decl_s_struct
_tmp109;_tmp109.tag=11;_tmp109.f1=d;_tmp109.f2=s;_tmp109;});_tmp108;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Decl*
d=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*_tmp10C=_cycalloc(
sizeof(*_tmp10C));_tmp10C[0]=({struct Cyc_Absyn_Var_d_struct _tmp10D;_tmp10D.tag=0;
_tmp10D.f1=Cyc_Absyn_new_vardecl(x,t,init);_tmp10D;});_tmp10C;}),loc);return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Decl_s_struct*_tmp10A=_cycalloc(sizeof(*_tmp10A));
_tmp10A[0]=({struct Cyc_Absyn_Decl_s_struct _tmp10B;_tmp10B.tag=11;_tmp10B.f1=d;
_tmp10B.f2=s;_tmp10B;});_tmp10A;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(
struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Label_s_struct*_tmp10E=
_cycalloc(sizeof(*_tmp10E));_tmp10E[0]=({struct Cyc_Absyn_Label_s_struct _tmp10F;
_tmp10F.tag=12;_tmp10F.f1=v;_tmp10F.f2=s;_tmp10F;});_tmp10E;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Do_s_struct*_tmp110=
_cycalloc(sizeof(*_tmp110));_tmp110[0]=({struct Cyc_Absyn_Do_s_struct _tmp111;
_tmp111.tag=13;_tmp111.f1=s;_tmp111.f2=({struct _tuple2 _tmp112;_tmp112.f1=e;
_tmp112.f2=Cyc_Absyn_skip_stmt(e->loc);_tmp112;});_tmp111;});_tmp110;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
scs,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_TryCatch_s_struct*
_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113[0]=({struct Cyc_Absyn_TryCatch_s_struct
_tmp114;_tmp114.tag=14;_tmp114.f1=s;_tmp114.f2=scs;_tmp114;});_tmp113;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Goto_s_struct*_tmp115=
_cycalloc(sizeof(*_tmp115));_tmp115[0]=({struct Cyc_Absyn_Goto_s_struct _tmp116;
_tmp116.tag=7;_tmp116.f1=lab;_tmp116.f2=0;_tmp116;});_tmp115;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(e1,e2,loc),loc);}struct Cyc_Absyn_Pat*
Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s){return({struct Cyc_Absyn_Pat*
_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117->r=(void*)p;_tmp117->topt=0;_tmp117->loc=
s;_tmp117;});}struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_struct*_tmp118=_cycalloc(
sizeof(*_tmp118));_tmp118[0]=({struct Cyc_Absyn_Exp_p_struct _tmp119;_tmp119.tag=
14;_tmp119.f1=e;_tmp119;});_tmp118;}),e->loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(
void*r,struct Cyc_Position_Segment*loc){return({struct Cyc_Absyn_Decl*_tmp11A=
_cycalloc(sizeof(*_tmp11A));_tmp11A->r=(void*)r;_tmp11A->loc=loc;_tmp11A;});}
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Let_d_struct*
_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B[0]=({struct Cyc_Absyn_Let_d_struct
_tmp11C;_tmp11C.tag=2;_tmp11C.f1=p;_tmp11C.f2=0;_tmp11C.f3=e;_tmp11C;});_tmp11B;}),
loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Letv_d_struct*_tmp11D=
_cycalloc(sizeof(*_tmp11D));_tmp11D[0]=({struct Cyc_Absyn_Letv_d_struct _tmp11E;
_tmp11E.tag=3;_tmp11E.f1=vds;_tmp11E;});_tmp11D;}),loc);}struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){return({
struct Cyc_Absyn_Vardecl*_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F->sc=(void*)((
void*)2);_tmp11F->name=x;_tmp11F->tq=Cyc_Absyn_empty_tqual(0);_tmp11F->type=(
void*)t;_tmp11F->initializer=init;_tmp11F->rgn=0;_tmp11F->attributes=0;_tmp11F->escapes=
0;_tmp11F;});}struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,
void*t,struct Cyc_Absyn_Exp*init){return({struct Cyc_Absyn_Vardecl*_tmp120=
_cycalloc(sizeof(*_tmp120));_tmp120->sc=(void*)((void*)0);_tmp120->name=x;
_tmp120->tq=Cyc_Absyn_empty_tqual(0);_tmp120->type=(void*)t;_tmp120->initializer=
init;_tmp120->rgn=0;_tmp120->attributes=0;_tmp120->escapes=0;_tmp120;});}struct
Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct
Cyc_List_List*po,struct Cyc_List_List*fs){return({struct Cyc_Absyn_AggrdeclImpl*
_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121->exist_vars=exists;_tmp121->rgn_po=
po;_tmp121->fields=fs;_tmp121;});}struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*
k,void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp122=_cycalloc(sizeof(*_tmp122));
_tmp122[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp123;_tmp123.tag=4;_tmp123.f1=({
struct Cyc_Absyn_Aggrdecl*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->kind=(void*)
k;_tmp124->sc=(void*)s;_tmp124->name=n;_tmp124->tvs=ts;_tmp124->impl=i;_tmp124->attributes=
atts;_tmp124;});_tmp123;});_tmp122;}),loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(
void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl((
void*)0,s,n,ts,i,atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(void*s,
struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl((void*)1,s,n,ts,i,
atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(void*s,struct _tuple0*n,
struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_xtunion,int is_flat,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Tunion_d_struct*_tmp125=
_cycalloc(sizeof(*_tmp125));_tmp125[0]=({struct Cyc_Absyn_Tunion_d_struct _tmp126;
_tmp126.tag=5;_tmp126.f1=({struct Cyc_Absyn_Tuniondecl*_tmp127=_cycalloc(sizeof(*
_tmp127));_tmp127->sc=(void*)s;_tmp127->name=n;_tmp127->tvs=ts;_tmp127->fields=
fs;_tmp127->is_xtunion=is_xtunion;_tmp127->is_flat=is_flat;_tmp127;});_tmp126;});
_tmp125;}),loc);}static struct _tuple1*Cyc_Absyn_expand_arg(struct _tuple1*a){
return({struct _tuple1*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->f1=(*a).f1;
_tmp128->f2=(*a).f2;_tmp128->f3=Cyc_Absyn_pointer_expand((*a).f3,1);_tmp128;});}
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,
void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*
cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts){return(void*)({
struct Cyc_Absyn_FnType_struct*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129[0]=({
struct Cyc_Absyn_FnType_struct _tmp12A;_tmp12A.tag=8;_tmp12A.f1=({struct Cyc_Absyn_FnInfo
_tmp12B;_tmp12B.tvars=tvs;_tmp12B.ret_typ=(void*)Cyc_Absyn_pointer_expand(
ret_typ,0);_tmp12B.effect=eff_typ;_tmp12B.args=((struct Cyc_List_List*(*)(struct
_tuple1*(*f)(struct _tuple1*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absyn_expand_arg,
args);_tmp12B.c_varargs=c_varargs;_tmp12B.cyc_varargs=cyc_varargs;_tmp12B.rgn_po=
rgn_po;_tmp12B.attributes=atts;_tmp12B;});_tmp12A;});_tmp129;});}void*Cyc_Absyn_pointer_expand(
void*t,int fresh_evar){void*_tmp12C=Cyc_Tcutil_compress(t);_LL6B: if(_tmp12C <= (
void*)4)goto _LL6D;if(*((int*)_tmp12C)!= 8)goto _LL6D;_LL6C: return Cyc_Absyn_at_typ(
t,fresh_evar?Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp12D=_cycalloc(sizeof(*
_tmp12D));_tmp12D->v=(void*)((void*)3);_tmp12D;}),0):(void*)2,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_false_conref);_LL6D:;_LL6E: return t;_LL6A:;}int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*e){void*_tmp12E=(void*)e->r;void*_tmp12F;void*_tmp130;struct
Cyc_Absyn_Vardecl*_tmp131;void*_tmp132;struct Cyc_Absyn_Vardecl*_tmp133;struct Cyc_Absyn_Exp*
_tmp134;struct Cyc_Absyn_Exp*_tmp135;struct Cyc_Absyn_Exp*_tmp136;_LL70: if(*((int*)
_tmp12E)!= 1)goto _LL72;_tmp12F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp12E)->f2;
if(_tmp12F <= (void*)1)goto _LL72;if(*((int*)_tmp12F)!= 1)goto _LL72;_LL71: return 0;
_LL72: if(*((int*)_tmp12E)!= 1)goto _LL74;_tmp130=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp12E)->f2;if(_tmp130 <= (void*)1)goto _LL74;if(*((int*)_tmp130)!= 0)goto _LL74;
_tmp131=((struct Cyc_Absyn_Global_b_struct*)_tmp130)->f1;_LL73: _tmp133=_tmp131;
goto _LL75;_LL74: if(*((int*)_tmp12E)!= 1)goto _LL76;_tmp132=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp12E)->f2;if(_tmp132 <= (void*)1)goto _LL76;if(*((int*)_tmp132)!= 3)goto _LL76;
_tmp133=((struct Cyc_Absyn_Local_b_struct*)_tmp132)->f1;_LL75: {void*_tmp137=Cyc_Tcutil_compress((
void*)_tmp133->type);_LL87: if(_tmp137 <= (void*)4)goto _LL89;if(*((int*)_tmp137)!= 
7)goto _LL89;_LL88: return 0;_LL89:;_LL8A: return 1;_LL86:;}_LL76: if(*((int*)_tmp12E)
!= 1)goto _LL78;_LL77: goto _LL79;_LL78: if(*((int*)_tmp12E)!= 24)goto _LL7A;_LL79:
goto _LL7B;_LL7A: if(*((int*)_tmp12E)!= 22)goto _LL7C;_LL7B: goto _LL7D;_LL7C: if(*((
int*)_tmp12E)!= 25)goto _LL7E;_LL7D: return 1;_LL7E: if(*((int*)_tmp12E)!= 23)goto
_LL80;_tmp134=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp12E)->f1;_LL7F: return
Cyc_Absyn_is_lvalue(_tmp134);_LL80: if(*((int*)_tmp12E)!= 14)goto _LL82;_tmp135=((
struct Cyc_Absyn_Instantiate_e_struct*)_tmp12E)->f1;_LL81: return Cyc_Absyn_is_lvalue(
_tmp135);_LL82: if(*((int*)_tmp12E)!= 13)goto _LL84;_tmp136=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp12E)->f1;_LL83: return Cyc_Absyn_is_lvalue(_tmp136);_LL84:;_LL85: return 0;_LL6F:;}
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,
struct _dyneither_ptr*v){{struct Cyc_List_List*_tmp138=fields;for(0;_tmp138 != 0;
_tmp138=_tmp138->tl){if(Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)_tmp138->hd)->name,
v)== 0)return(struct Cyc_Absyn_Aggrfield*)((struct Cyc_Absyn_Aggrfield*)_tmp138->hd);}}
return 0;}struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*
ad,struct _dyneither_ptr*v){return ad->impl == 0?0: Cyc_Absyn_lookup_field(((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}struct _tuple3*Cyc_Absyn_lookup_tuple_field(
struct Cyc_List_List*ts,int i){for(0;i != 0;-- i){if(ts == 0)return 0;ts=ts->tl;}if(ts
== 0)return 0;return(struct _tuple3*)((struct _tuple3*)ts->hd);}struct
_dyneither_ptr Cyc_Absyn_attribute2string(void*a){void*_tmp139=a;int _tmp13A;int
_tmp13B;struct _dyneither_ptr _tmp13C;void*_tmp13D;int _tmp13E;int _tmp13F;void*
_tmp140;int _tmp141;int _tmp142;int _tmp143;struct _dyneither_ptr _tmp144;_LL8C: if(
_tmp139 <= (void*)17)goto _LL8E;if(*((int*)_tmp139)!= 0)goto _LL8E;_tmp13A=((struct
Cyc_Absyn_Regparm_att_struct*)_tmp139)->f1;_LL8D: return(struct _dyneither_ptr)({
struct Cyc_Int_pa_struct _tmp147;_tmp147.tag=1;_tmp147.f1=(unsigned long)_tmp13A;{
void*_tmp145[1]={& _tmp147};Cyc_aprintf(({const char*_tmp146="regparm(%d)";
_tag_dyneither(_tmp146,sizeof(char),12);}),_tag_dyneither(_tmp145,sizeof(void*),
1));}});_LL8E: if((int)_tmp139 != 0)goto _LL90;_LL8F: return({const char*_tmp148="stdcall";
_tag_dyneither(_tmp148,sizeof(char),8);});_LL90: if((int)_tmp139 != 1)goto _LL92;
_LL91: return({const char*_tmp149="cdecl";_tag_dyneither(_tmp149,sizeof(char),6);});
_LL92: if((int)_tmp139 != 2)goto _LL94;_LL93: return({const char*_tmp14A="fastcall";
_tag_dyneither(_tmp14A,sizeof(char),9);});_LL94: if((int)_tmp139 != 3)goto _LL96;
_LL95: return({const char*_tmp14B="noreturn";_tag_dyneither(_tmp14B,sizeof(char),9);});
_LL96: if((int)_tmp139 != 4)goto _LL98;_LL97: return({const char*_tmp14C="const";
_tag_dyneither(_tmp14C,sizeof(char),6);});_LL98: if(_tmp139 <= (void*)17)goto _LL9A;
if(*((int*)_tmp139)!= 1)goto _LL9A;_tmp13B=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp139)->f1;_LL99: if(_tmp13B == - 1)return({const char*_tmp14D="aligned";
_tag_dyneither(_tmp14D,sizeof(char),8);});else{void*_tmp14E=Cyc_Cyclone_c_compiler;
_LLBD: if((int)_tmp14E != 0)goto _LLBF;_LLBE: return(struct _dyneither_ptr)({struct
Cyc_Int_pa_struct _tmp151;_tmp151.tag=1;_tmp151.f1=(unsigned long)_tmp13B;{void*
_tmp14F[1]={& _tmp151};Cyc_aprintf(({const char*_tmp150="aligned(%d)";
_tag_dyneither(_tmp150,sizeof(char),12);}),_tag_dyneither(_tmp14F,sizeof(void*),
1));}});_LLBF: if((int)_tmp14E != 1)goto _LLBC;_LLC0: return(struct _dyneither_ptr)({
struct Cyc_Int_pa_struct _tmp154;_tmp154.tag=1;_tmp154.f1=(unsigned long)_tmp13B;{
void*_tmp152[1]={& _tmp154};Cyc_aprintf(({const char*_tmp153="align(%d)";
_tag_dyneither(_tmp153,sizeof(char),10);}),_tag_dyneither(_tmp152,sizeof(void*),
1));}});_LLBC:;}_LL9A: if((int)_tmp139 != 5)goto _LL9C;_LL9B: return({const char*
_tmp155="packed";_tag_dyneither(_tmp155,sizeof(char),7);});_LL9C: if(_tmp139 <= (
void*)17)goto _LL9E;if(*((int*)_tmp139)!= 2)goto _LL9E;_tmp13C=((struct Cyc_Absyn_Section_att_struct*)
_tmp139)->f1;_LL9D: return(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp158;_tmp158.tag=0;_tmp158.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tmp13C);{void*_tmp156[1]={& _tmp158};Cyc_aprintf(({const char*_tmp157="section(\"%s\")";
_tag_dyneither(_tmp157,sizeof(char),14);}),_tag_dyneither(_tmp156,sizeof(void*),
1));}});_LL9E: if((int)_tmp139 != 6)goto _LLA0;_LL9F: return({const char*_tmp159="nocommon";
_tag_dyneither(_tmp159,sizeof(char),9);});_LLA0: if((int)_tmp139 != 7)goto _LLA2;
_LLA1: return({const char*_tmp15A="shared";_tag_dyneither(_tmp15A,sizeof(char),7);});
_LLA2: if((int)_tmp139 != 8)goto _LLA4;_LLA3: return({const char*_tmp15B="unused";
_tag_dyneither(_tmp15B,sizeof(char),7);});_LLA4: if((int)_tmp139 != 9)goto _LLA6;
_LLA5: return({const char*_tmp15C="weak";_tag_dyneither(_tmp15C,sizeof(char),5);});
_LLA6: if((int)_tmp139 != 10)goto _LLA8;_LLA7: return({const char*_tmp15D="dllimport";
_tag_dyneither(_tmp15D,sizeof(char),10);});_LLA8: if((int)_tmp139 != 11)goto _LLAA;
_LLA9: return({const char*_tmp15E="dllexport";_tag_dyneither(_tmp15E,sizeof(char),
10);});_LLAA: if((int)_tmp139 != 12)goto _LLAC;_LLAB: return({const char*_tmp15F="no_instrument_function";
_tag_dyneither(_tmp15F,sizeof(char),23);});_LLAC: if((int)_tmp139 != 13)goto _LLAE;
_LLAD: return({const char*_tmp160="constructor";_tag_dyneither(_tmp160,sizeof(char),
12);});_LLAE: if((int)_tmp139 != 14)goto _LLB0;_LLAF: return({const char*_tmp161="destructor";
_tag_dyneither(_tmp161,sizeof(char),11);});_LLB0: if((int)_tmp139 != 15)goto _LLB2;
_LLB1: return({const char*_tmp162="no_check_memory_usage";_tag_dyneither(_tmp162,
sizeof(char),22);});_LLB2: if(_tmp139 <= (void*)17)goto _LLB8;if(*((int*)_tmp139)!= 
3)goto _LLB4;_tmp13D=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp139)->f1;if((
int)_tmp13D != 0)goto _LLB4;_tmp13E=((struct Cyc_Absyn_Format_att_struct*)_tmp139)->f2;
_tmp13F=((struct Cyc_Absyn_Format_att_struct*)_tmp139)->f3;_LLB3: return(struct
_dyneither_ptr)({struct Cyc_Int_pa_struct _tmp166;_tmp166.tag=1;_tmp166.f1=(
unsigned int)_tmp13F;{struct Cyc_Int_pa_struct _tmp165;_tmp165.tag=1;_tmp165.f1=(
unsigned int)_tmp13E;{void*_tmp163[2]={& _tmp165,& _tmp166};Cyc_aprintf(({const
char*_tmp164="format(printf,%u,%u)";_tag_dyneither(_tmp164,sizeof(char),21);}),
_tag_dyneither(_tmp163,sizeof(void*),2));}}});_LLB4: if(*((int*)_tmp139)!= 3)goto
_LLB6;_tmp140=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp139)->f1;if((int)
_tmp140 != 1)goto _LLB6;_tmp141=((struct Cyc_Absyn_Format_att_struct*)_tmp139)->f2;
_tmp142=((struct Cyc_Absyn_Format_att_struct*)_tmp139)->f3;_LLB5: return(struct
_dyneither_ptr)({struct Cyc_Int_pa_struct _tmp16A;_tmp16A.tag=1;_tmp16A.f1=(
unsigned int)_tmp142;{struct Cyc_Int_pa_struct _tmp169;_tmp169.tag=1;_tmp169.f1=(
unsigned int)_tmp141;{void*_tmp167[2]={& _tmp169,& _tmp16A};Cyc_aprintf(({const
char*_tmp168="format(scanf,%u,%u)";_tag_dyneither(_tmp168,sizeof(char),20);}),
_tag_dyneither(_tmp167,sizeof(void*),2));}}});_LLB6: if(*((int*)_tmp139)!= 4)goto
_LLB8;_tmp143=((struct Cyc_Absyn_Initializes_att_struct*)_tmp139)->f1;_LLB7:
return(struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp16D;_tmp16D.tag=1;
_tmp16D.f1=(unsigned long)_tmp143;{void*_tmp16B[1]={& _tmp16D};Cyc_aprintf(({
const char*_tmp16C="initializes(%d)";_tag_dyneither(_tmp16C,sizeof(char),16);}),
_tag_dyneither(_tmp16B,sizeof(void*),1));}});_LLB8: if((int)_tmp139 != 16)goto
_LLBA;_LLB9: return({const char*_tmp16E="pure";_tag_dyneither(_tmp16E,sizeof(char),
5);});_LLBA: if(_tmp139 <= (void*)17)goto _LL8B;if(*((int*)_tmp139)!= 5)goto _LL8B;
_tmp144=((struct Cyc_Absyn_Mode_att_struct*)_tmp139)->f1;_LLBB: return(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp171;_tmp171.tag=0;_tmp171.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp144);{void*_tmp16F[1]={& _tmp171};
Cyc_aprintf(({const char*_tmp170="__mode__(\"%s\")";_tag_dyneither(_tmp170,
sizeof(char),15);}),_tag_dyneither(_tmp16F,sizeof(void*),1));}});_LL8B:;}int Cyc_Absyn_fntype_att(
void*a){void*_tmp172=a;_LLC2: if(_tmp172 <= (void*)17)goto _LLC4;if(*((int*)_tmp172)
!= 0)goto _LLC4;_LLC3: goto _LLC5;_LLC4: if((int)_tmp172 != 2)goto _LLC6;_LLC5: goto
_LLC7;_LLC6: if((int)_tmp172 != 0)goto _LLC8;_LLC7: goto _LLC9;_LLC8: if((int)_tmp172
!= 1)goto _LLCA;_LLC9: goto _LLCB;_LLCA: if((int)_tmp172 != 3)goto _LLCC;_LLCB: goto
_LLCD;_LLCC: if((int)_tmp172 != 16)goto _LLCE;_LLCD: goto _LLCF;_LLCE: if(_tmp172 <= (
void*)17)goto _LLD0;if(*((int*)_tmp172)!= 3)goto _LLD0;_LLCF: goto _LLD1;_LLD0: if((
int)_tmp172 != 4)goto _LLD2;_LLD1: return 1;_LLD2: if(_tmp172 <= (void*)17)goto _LLD4;
if(*((int*)_tmp172)!= 4)goto _LLD4;_LLD3: return 1;_LLD4:;_LLD5: return 0;_LLC1:;}
static char _tmp173[3]="f0";static struct _dyneither_ptr Cyc_Absyn_f0={_tmp173,
_tmp173,_tmp173 + 3};static struct _dyneither_ptr*Cyc_Absyn_field_names_v[1]={& Cyc_Absyn_f0};
static struct _dyneither_ptr Cyc_Absyn_field_names={(void*)((struct _dyneither_ptr**)
Cyc_Absyn_field_names_v),(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(
void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v + 1)};struct _dyneither_ptr*
Cyc_Absyn_fieldname(int i){unsigned int fsz=_get_dyneither_size(Cyc_Absyn_field_names,
sizeof(struct _dyneither_ptr*));if(i >= fsz)Cyc_Absyn_field_names=({unsigned int
_tmp174=(unsigned int)(i + 1);struct _dyneither_ptr**_tmp175=(struct _dyneither_ptr**)
_cycalloc(_check_times(sizeof(struct _dyneither_ptr*),_tmp174));struct
_dyneither_ptr _tmp17B=_tag_dyneither(_tmp175,sizeof(struct _dyneither_ptr*),
_tmp174);{unsigned int _tmp176=_tmp174;unsigned int j;for(j=0;j < _tmp176;j ++){
_tmp175[j]=j < fsz?*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,
sizeof(struct _dyneither_ptr*),(int)j)):({struct _dyneither_ptr*_tmp177=_cycalloc(
sizeof(*_tmp177));_tmp177[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_struct
_tmp17A;_tmp17A.tag=1;_tmp17A.f1=(unsigned long)((int)j);{void*_tmp178[1]={&
_tmp17A};Cyc_aprintf(({const char*_tmp179="f%d";_tag_dyneither(_tmp179,sizeof(
char),4);}),_tag_dyneither(_tmp178,sizeof(void*),1));}});_tmp177;});}}_tmp17B;});
return*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,
sizeof(struct _dyneither_ptr*),i));}struct _tuple4 Cyc_Absyn_aggr_kinded_name(union
Cyc_Absyn_AggrInfoU_union info){union Cyc_Absyn_AggrInfoU_union _tmp17C=info;void*
_tmp17D;struct _tuple0*_tmp17E;struct Cyc_Absyn_Aggrdecl**_tmp17F;struct Cyc_Absyn_Aggrdecl*
_tmp180;struct Cyc_Absyn_Aggrdecl _tmp181;void*_tmp182;struct _tuple0*_tmp183;_LLD7:
if((_tmp17C.UnknownAggr).tag != 0)goto _LLD9;_tmp17D=(_tmp17C.UnknownAggr).f1;
_tmp17E=(_tmp17C.UnknownAggr).f2;_LLD8: return({struct _tuple4 _tmp184;_tmp184.f1=
_tmp17D;_tmp184.f2=_tmp17E;_tmp184;});_LLD9: if((_tmp17C.KnownAggr).tag != 1)goto
_LLD6;_tmp17F=(_tmp17C.KnownAggr).f1;_tmp180=*_tmp17F;_tmp181=*_tmp180;_tmp182=(
void*)_tmp181.kind;_tmp183=_tmp181.name;_LLDA: return({struct _tuple4 _tmp185;
_tmp185.f1=_tmp182;_tmp185.f2=_tmp183;_tmp185;});_LLD6:;}struct Cyc_Absyn_Aggrdecl*
Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU_union info){union Cyc_Absyn_AggrInfoU_union
_tmp186=info;void*_tmp187;struct _tuple0*_tmp188;struct Cyc_Absyn_Aggrdecl**
_tmp189;struct Cyc_Absyn_Aggrdecl*_tmp18A;_LLDC: if((_tmp186.UnknownAggr).tag != 0)
goto _LLDE;_tmp187=(_tmp186.UnknownAggr).f1;_tmp188=(_tmp186.UnknownAggr).f2;
_LLDD:({void*_tmp18B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp18C="unchecked aggrdecl";_tag_dyneither(
_tmp18C,sizeof(char),19);}),_tag_dyneither(_tmp18B,sizeof(void*),0));});_LLDE:
if((_tmp186.KnownAggr).tag != 1)goto _LLDB;_tmp189=(_tmp186.KnownAggr).f1;_tmp18A=*
_tmp189;_LLDF: return _tmp18A;_LLDB:;}int Cyc_Absyn_is_union_type(void*t){void*
_tmp18D=Cyc_Tcutil_compress(t);void*_tmp18E;struct Cyc_Absyn_AggrInfo _tmp18F;
union Cyc_Absyn_AggrInfoU_union _tmp190;_LLE1: if(_tmp18D <= (void*)4)goto _LLE5;if(*((
int*)_tmp18D)!= 11)goto _LLE3;_tmp18E=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp18D)->f1;if((int)_tmp18E != 1)goto _LLE3;_LLE2: return 1;_LLE3: if(*((int*)
_tmp18D)!= 10)goto _LLE5;_tmp18F=((struct Cyc_Absyn_AggrType_struct*)_tmp18D)->f1;
_tmp190=_tmp18F.aggr_info;_LLE4: return(Cyc_Absyn_aggr_kinded_name(_tmp190)).f1 == (
void*)1;_LLE5:;_LLE6: return 0;_LLE0:;}int Cyc_Absyn_is_aggr_type(void*t){void*
_tmp191=Cyc_Tcutil_compress(t);_LLE8: if(_tmp191 <= (void*)4)goto _LLF2;if(*((int*)
_tmp191)!= 2)goto _LLEA;_LLE9: goto _LLEB;_LLEA: if(*((int*)_tmp191)!= 3)goto _LLEC;
_LLEB: goto _LLED;_LLEC: if(*((int*)_tmp191)!= 9)goto _LLEE;_LLED: goto _LLEF;_LLEE:
if(*((int*)_tmp191)!= 10)goto _LLF0;_LLEF: goto _LLF1;_LLF0: if(*((int*)_tmp191)!= 
11)goto _LLF2;_LLF1: return 1;_LLF2:;_LLF3: return 0;_LLE7:;}void Cyc_Absyn_print_decls(
struct Cyc_List_List*decls){((void(*)(void*rep,struct Cyc_List_List**val))Cyc_Marshal_print_type)(
Cyc_decls_rep,& decls);({void*_tmp192=0;Cyc_printf(({const char*_tmp193="\n";
_tag_dyneither(_tmp193,sizeof(char),2);}),_tag_dyneither(_tmp192,sizeof(void*),0));});}
int Cyc_Absyn_porting_c_code=0;int Cyc_Absyn_no_regions=0;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_0;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_decl_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Decl_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_decl_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_817;static struct Cyc_Typerep_Int_struct
Cyc__genrep_24={0,0,32};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_143;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Vardecl_rep;extern struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_scope_t_rep;static char _tmp195[7]="Static";
static struct _tuple7 Cyc__gentuple_152={0,(struct _dyneither_ptr){_tmp195,_tmp195,
_tmp195 + 7}};static char _tmp196[9]="Abstract";static struct _tuple7 Cyc__gentuple_153={
1,(struct _dyneither_ptr){_tmp196,_tmp196,_tmp196 + 9}};static char _tmp197[7]="Public";
static struct _tuple7 Cyc__gentuple_154={2,(struct _dyneither_ptr){_tmp197,_tmp197,
_tmp197 + 7}};static char _tmp198[7]="Extern";static struct _tuple7 Cyc__gentuple_155={
3,(struct _dyneither_ptr){_tmp198,_tmp198,_tmp198 + 7}};static char _tmp199[8]="ExternC";
static struct _tuple7 Cyc__gentuple_156={4,(struct _dyneither_ptr){_tmp199,_tmp199,
_tmp199 + 8}};static char _tmp19A[9]="Register";static struct _tuple7 Cyc__gentuple_157={
5,(struct _dyneither_ptr){_tmp19A,_tmp19A,_tmp19A + 9}};static struct _tuple7*Cyc__genarr_158[
6]={& Cyc__gentuple_152,& Cyc__gentuple_153,& Cyc__gentuple_154,& Cyc__gentuple_155,&
Cyc__gentuple_156,& Cyc__gentuple_157};static struct _tuple5*Cyc__genarr_159[0]={};
static char _tmp19C[6]="Scope";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_scope_t_rep={
5,(struct _dyneither_ptr){_tmp19C,_tmp19C,_tmp19C + 6},{(void*)((struct _tuple7**)
Cyc__genarr_158),(void*)((struct _tuple7**)Cyc__genarr_158),(void*)((struct
_tuple7**)Cyc__genarr_158 + 6)},{(void*)((struct _tuple5**)Cyc__genarr_159),(void*)((
struct _tuple5**)Cyc__genarr_159),(void*)((struct _tuple5**)Cyc__genarr_159 + 0)}};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_11;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_12;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_nmspace_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_18;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_19;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_var_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_13;extern struct Cyc_Typerep_FatPtr_struct
Cyc__genrep_14;static struct Cyc_Typerep_Int_struct Cyc__genrep_15={0,0,8};static
struct Cyc_Typerep_FatPtr_struct Cyc__genrep_14={2,(void*)((void*)& Cyc__genrep_15)};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_13={1,1,(void*)((void*)& Cyc__genrep_14)};
static char _tmp1A0[5]="List";static struct _dyneither_ptr Cyc__genname_23=(struct
_dyneither_ptr){_tmp1A0,_tmp1A0,_tmp1A0 + 5};static char _tmp1A1[3]="hd";static
struct _tuple5 Cyc__gentuple_20={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp1A1,_tmp1A1,_tmp1A1 + 3},(void*)& Cyc__genrep_13};static char
_tmp1A2[3]="tl";static struct _tuple5 Cyc__gentuple_21={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp1A2,_tmp1A2,_tmp1A2 + 3},(void*)& Cyc__genrep_19};static
struct _tuple5*Cyc__genarr_22[2]={& Cyc__gentuple_20,& Cyc__gentuple_21};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_var_t46H2_rep={3,(struct _dyneither_ptr*)& Cyc__genname_23,
sizeof(struct Cyc_List_List),{(void*)((struct _tuple5**)Cyc__genarr_22),(void*)((
struct _tuple5**)Cyc__genarr_22),(void*)((struct _tuple5**)Cyc__genarr_22 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_19={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_var_t46H2_rep)};
struct _tuple14{unsigned int f1;struct Cyc_List_List*f2;};static struct _tuple6 Cyc__gentuple_25={
offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_26={
offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_19};static struct _tuple6*Cyc__genarr_27[
2]={& Cyc__gentuple_25,& Cyc__gentuple_26};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_18={
4,sizeof(struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_27),(void*)((
struct _tuple6**)Cyc__genarr_27),(void*)((struct _tuple6**)Cyc__genarr_27 + 2)}};
static char _tmp1A6[6]="Loc_n";static struct _tuple7 Cyc__gentuple_16={0,(struct
_dyneither_ptr){_tmp1A6,_tmp1A6,_tmp1A6 + 6}};static struct _tuple7*Cyc__genarr_17[
1]={& Cyc__gentuple_16};static char _tmp1A7[6]="Rel_n";static struct _tuple5 Cyc__gentuple_28={
0,(struct _dyneither_ptr){_tmp1A7,_tmp1A7,_tmp1A7 + 6},(void*)& Cyc__genrep_18};
static char _tmp1A8[6]="Abs_n";static struct _tuple5 Cyc__gentuple_29={1,(struct
_dyneither_ptr){_tmp1A8,_tmp1A8,_tmp1A8 + 6},(void*)& Cyc__genrep_18};static struct
_tuple5*Cyc__genarr_30[2]={& Cyc__gentuple_28,& Cyc__gentuple_29};static char
_tmp1AA[8]="Nmspace";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_nmspace_t_rep={5,(
struct _dyneither_ptr){_tmp1AA,_tmp1AA,_tmp1AA + 8},{(void*)((struct _tuple7**)Cyc__genarr_17),(
void*)((struct _tuple7**)Cyc__genarr_17),(void*)((struct _tuple7**)Cyc__genarr_17 + 
1)},{(void*)((struct _tuple5**)Cyc__genarr_30),(void*)((struct _tuple5**)Cyc__genarr_30),(
void*)((struct _tuple5**)Cyc__genarr_30 + 2)}};static struct _tuple6 Cyc__gentuple_31={
offsetof(struct _tuple0,f1),(void*)& Cyc_Absyn_nmspace_t_rep};static struct _tuple6
Cyc__gentuple_32={offsetof(struct _tuple0,f2),(void*)& Cyc__genrep_13};static
struct _tuple6*Cyc__genarr_33[2]={& Cyc__gentuple_31,& Cyc__gentuple_32};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_12={4,sizeof(struct _tuple0),{(void*)((
struct _tuple6**)Cyc__genarr_33),(void*)((struct _tuple6**)Cyc__genarr_33),(void*)((
struct _tuple6**)Cyc__genarr_33 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_11={
1,1,(void*)((void*)& Cyc__genrep_12)};extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_tqual_t_rep;
static struct Cyc_Typerep_Int_struct Cyc__genrep_10={0,1,32};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_2;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Position_Segment_rep;
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_2={1,1,(void*)((void*)& Cyc_struct_Position_Segment_rep)};
static char _tmp1AF[6]="Tqual";static struct _dyneither_ptr Cyc__genname_151=(struct
_dyneither_ptr){_tmp1AF,_tmp1AF,_tmp1AF + 6};static char _tmp1B0[12]="print_const";
static struct _tuple5 Cyc__gentuple_145={offsetof(struct Cyc_Absyn_Tqual,print_const),(
struct _dyneither_ptr){_tmp1B0,_tmp1B0,_tmp1B0 + 12},(void*)& Cyc__genrep_10};
static char _tmp1B1[11]="q_volatile";static struct _tuple5 Cyc__gentuple_146={
offsetof(struct Cyc_Absyn_Tqual,q_volatile),(struct _dyneither_ptr){_tmp1B1,
_tmp1B1,_tmp1B1 + 11},(void*)& Cyc__genrep_10};static char _tmp1B2[11]="q_restrict";
static struct _tuple5 Cyc__gentuple_147={offsetof(struct Cyc_Absyn_Tqual,q_restrict),(
struct _dyneither_ptr){_tmp1B2,_tmp1B2,_tmp1B2 + 11},(void*)& Cyc__genrep_10};
static char _tmp1B3[11]="real_const";static struct _tuple5 Cyc__gentuple_148={
offsetof(struct Cyc_Absyn_Tqual,real_const),(struct _dyneither_ptr){_tmp1B3,
_tmp1B3,_tmp1B3 + 11},(void*)& Cyc__genrep_10};static char _tmp1B4[4]="loc";static
struct _tuple5 Cyc__gentuple_149={offsetof(struct Cyc_Absyn_Tqual,loc),(struct
_dyneither_ptr){_tmp1B4,_tmp1B4,_tmp1B4 + 4},(void*)& Cyc__genrep_2};static struct
_tuple5*Cyc__genarr_150[5]={& Cyc__gentuple_145,& Cyc__gentuple_146,& Cyc__gentuple_147,&
Cyc__gentuple_148,& Cyc__gentuple_149};struct Cyc_Typerep_Struct_struct Cyc_Absyn_tqual_t_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_151,sizeof(struct Cyc_Absyn_Tqual),{(void*)((
struct _tuple5**)Cyc__genarr_150),(void*)((struct _tuple5**)Cyc__genarr_150),(void*)((
struct _tuple5**)Cyc__genarr_150 + 5)}};extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_type_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1119;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1124;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_kind_t2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kind_t_rep;static char _tmp1B6[8]="AnyKind";
static struct _tuple7 Cyc__gentuple_173={0,(struct _dyneither_ptr){_tmp1B6,_tmp1B6,
_tmp1B6 + 8}};static char _tmp1B7[8]="MemKind";static struct _tuple7 Cyc__gentuple_174={
1,(struct _dyneither_ptr){_tmp1B7,_tmp1B7,_tmp1B7 + 8}};static char _tmp1B8[8]="BoxKind";
static struct _tuple7 Cyc__gentuple_175={2,(struct _dyneither_ptr){_tmp1B8,_tmp1B8,
_tmp1B8 + 8}};static char _tmp1B9[8]="RgnKind";static struct _tuple7 Cyc__gentuple_176={
3,(struct _dyneither_ptr){_tmp1B9,_tmp1B9,_tmp1B9 + 8}};static char _tmp1BA[14]="UniqueRgnKind";
static struct _tuple7 Cyc__gentuple_177={4,(struct _dyneither_ptr){_tmp1BA,_tmp1BA,
_tmp1BA + 14}};static char _tmp1BB[11]="TopRgnKind";static struct _tuple7 Cyc__gentuple_178={
5,(struct _dyneither_ptr){_tmp1BB,_tmp1BB,_tmp1BB + 11}};static char _tmp1BC[8]="EffKind";
static struct _tuple7 Cyc__gentuple_179={6,(struct _dyneither_ptr){_tmp1BC,_tmp1BC,
_tmp1BC + 8}};static char _tmp1BD[8]="IntKind";static struct _tuple7 Cyc__gentuple_180={
7,(struct _dyneither_ptr){_tmp1BD,_tmp1BD,_tmp1BD + 8}};static struct _tuple7*Cyc__genarr_181[
8]={& Cyc__gentuple_173,& Cyc__gentuple_174,& Cyc__gentuple_175,& Cyc__gentuple_176,&
Cyc__gentuple_177,& Cyc__gentuple_178,& Cyc__gentuple_179,& Cyc__gentuple_180};
static struct _tuple5*Cyc__genarr_182[0]={};static char _tmp1BF[5]="Kind";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_kind_t_rep={5,(struct _dyneither_ptr){_tmp1BF,_tmp1BF,_tmp1BF + 5},{(
void*)((struct _tuple7**)Cyc__genarr_181),(void*)((struct _tuple7**)Cyc__genarr_181),(
void*)((struct _tuple7**)Cyc__genarr_181 + 8)},{(void*)((struct _tuple5**)Cyc__genarr_182),(
void*)((struct _tuple5**)Cyc__genarr_182),(void*)((struct _tuple5**)Cyc__genarr_182
+ 0)}};static char _tmp1C0[4]="Opt";static struct _dyneither_ptr Cyc__genname_1127=(
struct _dyneither_ptr){_tmp1C0,_tmp1C0,_tmp1C0 + 4};static char _tmp1C1[2]="v";
static struct _tuple5 Cyc__gentuple_1125={offsetof(struct Cyc_Core_Opt,v),(struct
_dyneither_ptr){_tmp1C1,_tmp1C1,_tmp1C1 + 2},(void*)& Cyc_Absyn_kind_t_rep};static
struct _tuple5*Cyc__genarr_1126[1]={& Cyc__gentuple_1125};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_kind_t2_rep={3,(struct _dyneither_ptr*)& Cyc__genname_1127,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_1126),(void*)((
struct _tuple5**)Cyc__genarr_1126),(void*)((struct _tuple5**)Cyc__genarr_1126 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1124={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_kind_t2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_110;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_type_t2_rep;static char _tmp1C4[4]="Opt";static struct
_dyneither_ptr Cyc__genname_1159=(struct _dyneither_ptr){_tmp1C4,_tmp1C4,_tmp1C4 + 
4};static char _tmp1C5[2]="v";static struct _tuple5 Cyc__gentuple_1157={offsetof(
struct Cyc_Core_Opt,v),(struct _dyneither_ptr){_tmp1C5,_tmp1C5,_tmp1C5 + 2},(void*)&
Cyc_Absyn_type_t_rep};static struct _tuple5*Cyc__genarr_1158[1]={& Cyc__gentuple_1157};
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_type_t2_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_1159,sizeof(struct Cyc_Core_Opt),{(void*)((struct
_tuple5**)Cyc__genarr_1158),(void*)((struct _tuple5**)Cyc__genarr_1158),(void*)((
struct _tuple5**)Cyc__genarr_1158 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_110={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_type_t2_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1120;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_326;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_tvar_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_170;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tvar_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kindbound_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_195;static struct _tuple6 Cyc__gentuple_196={offsetof(struct _tuple6,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_197={offsetof(struct
_tuple6,f2),(void*)& Cyc_Absyn_kind_t_rep};static struct _tuple6*Cyc__genarr_198[2]={&
Cyc__gentuple_196,& Cyc__gentuple_197};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_195={
4,sizeof(struct _tuple6),{(void*)((struct _tuple6**)Cyc__genarr_198),(void*)((
struct _tuple6**)Cyc__genarr_198),(void*)((struct _tuple6**)Cyc__genarr_198 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_191;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_183;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep;
static char _tmp1C9[4]="Opt";static struct _dyneither_ptr Cyc__genname_186=(struct
_dyneither_ptr){_tmp1C9,_tmp1C9,_tmp1C9 + 4};static char _tmp1CA[2]="v";static
struct _tuple5 Cyc__gentuple_184={offsetof(struct Cyc_Core_Opt,v),(struct
_dyneither_ptr){_tmp1CA,_tmp1CA,_tmp1CA + 2},(void*)& Cyc_Absyn_kindbound_t_rep};
static struct _tuple5*Cyc__genarr_185[1]={& Cyc__gentuple_184};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep={3,(struct _dyneither_ptr*)& Cyc__genname_186,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_185),(void*)((
struct _tuple5**)Cyc__genarr_185),(void*)((struct _tuple5**)Cyc__genarr_185 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_183={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_kindbound_t2_rep)};
struct _tuple15{unsigned int f1;struct Cyc_Core_Opt*f2;};static struct _tuple6 Cyc__gentuple_192={
offsetof(struct _tuple15,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_193={
offsetof(struct _tuple15,f2),(void*)& Cyc__genrep_183};static struct _tuple6*Cyc__genarr_194[
2]={& Cyc__gentuple_192,& Cyc__gentuple_193};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_191={4,sizeof(struct _tuple15),{(void*)((struct _tuple6**)Cyc__genarr_194),(
void*)((struct _tuple6**)Cyc__genarr_194),(void*)((struct _tuple6**)Cyc__genarr_194
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_172;struct _tuple16{
unsigned int f1;struct Cyc_Core_Opt*f2;void*f3;};static struct _tuple6 Cyc__gentuple_187={
offsetof(struct _tuple16,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_188={
offsetof(struct _tuple16,f2),(void*)& Cyc__genrep_183};static struct _tuple6 Cyc__gentuple_189={
offsetof(struct _tuple16,f3),(void*)& Cyc_Absyn_kind_t_rep};static struct _tuple6*
Cyc__genarr_190[3]={& Cyc__gentuple_187,& Cyc__gentuple_188,& Cyc__gentuple_189};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_172={4,sizeof(struct _tuple16),{(
void*)((struct _tuple6**)Cyc__genarr_190),(void*)((struct _tuple6**)Cyc__genarr_190),(
void*)((struct _tuple6**)Cyc__genarr_190 + 3)}};static struct _tuple7*Cyc__genarr_171[
0]={};static char _tmp1CF[6]="Eq_kb";static struct _tuple5 Cyc__gentuple_199={0,(
struct _dyneither_ptr){_tmp1CF,_tmp1CF,_tmp1CF + 6},(void*)& Cyc__genrep_195};
static char _tmp1D0[11]="Unknown_kb";static struct _tuple5 Cyc__gentuple_200={1,(
struct _dyneither_ptr){_tmp1D0,_tmp1D0,_tmp1D0 + 11},(void*)& Cyc__genrep_191};
static char _tmp1D1[8]="Less_kb";static struct _tuple5 Cyc__gentuple_201={2,(struct
_dyneither_ptr){_tmp1D1,_tmp1D1,_tmp1D1 + 8},(void*)& Cyc__genrep_172};static
struct _tuple5*Cyc__genarr_202[3]={& Cyc__gentuple_199,& Cyc__gentuple_200,& Cyc__gentuple_201};
static char _tmp1D3[10]="KindBound";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_kindbound_t_rep={
5,(struct _dyneither_ptr){_tmp1D3,_tmp1D3,_tmp1D3 + 10},{(void*)((struct _tuple7**)
Cyc__genarr_171),(void*)((struct _tuple7**)Cyc__genarr_171),(void*)((struct
_tuple7**)Cyc__genarr_171 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_202),(void*)((
struct _tuple5**)Cyc__genarr_202),(void*)((struct _tuple5**)Cyc__genarr_202 + 3)}};
static char _tmp1D4[5]="Tvar";static struct _dyneither_ptr Cyc__genname_207=(struct
_dyneither_ptr){_tmp1D4,_tmp1D4,_tmp1D4 + 5};static char _tmp1D5[5]="name";static
struct _tuple5 Cyc__gentuple_203={offsetof(struct Cyc_Absyn_Tvar,name),(struct
_dyneither_ptr){_tmp1D5,_tmp1D5,_tmp1D5 + 5},(void*)& Cyc__genrep_13};static char
_tmp1D6[9]="identity";static struct _tuple5 Cyc__gentuple_204={offsetof(struct Cyc_Absyn_Tvar,identity),(
struct _dyneither_ptr){_tmp1D6,_tmp1D6,_tmp1D6 + 9},(void*)& Cyc__genrep_10};static
char _tmp1D7[5]="kind";static struct _tuple5 Cyc__gentuple_205={offsetof(struct Cyc_Absyn_Tvar,kind),(
struct _dyneither_ptr){_tmp1D7,_tmp1D7,_tmp1D7 + 5},(void*)& Cyc_Absyn_kindbound_t_rep};
static struct _tuple5*Cyc__genarr_206[3]={& Cyc__gentuple_203,& Cyc__gentuple_204,&
Cyc__gentuple_205};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tvar_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_207,sizeof(struct Cyc_Absyn_Tvar),{(void*)((
struct _tuple5**)Cyc__genarr_206),(void*)((struct _tuple5**)Cyc__genarr_206),(void*)((
struct _tuple5**)Cyc__genarr_206 + 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_170={
1,1,(void*)((void*)& Cyc_struct_Absyn_Tvar_rep)};static char _tmp1DA[5]="List";
static struct _dyneither_ptr Cyc__genname_330=(struct _dyneither_ptr){_tmp1DA,
_tmp1DA,_tmp1DA + 5};static char _tmp1DB[3]="hd";static struct _tuple5 Cyc__gentuple_327={
offsetof(struct Cyc_List_List,hd),(struct _dyneither_ptr){_tmp1DB,_tmp1DB,_tmp1DB + 
3},(void*)& Cyc__genrep_170};static char _tmp1DC[3]="tl";static struct _tuple5 Cyc__gentuple_328={
offsetof(struct Cyc_List_List,tl),(struct _dyneither_ptr){_tmp1DC,_tmp1DC,_tmp1DC + 
3},(void*)& Cyc__genrep_326};static struct _tuple5*Cyc__genarr_329[2]={& Cyc__gentuple_327,&
Cyc__gentuple_328};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_tvar_t46H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_330,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_329),(void*)((struct _tuple5**)Cyc__genarr_329),(void*)((
struct _tuple5**)Cyc__genarr_329 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_326={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_tvar_t46H2_rep)};static char
_tmp1DF[4]="Opt";static struct _dyneither_ptr Cyc__genname_1123=(struct
_dyneither_ptr){_tmp1DF,_tmp1DF,_tmp1DF + 4};static char _tmp1E0[2]="v";static
struct _tuple5 Cyc__gentuple_1121={offsetof(struct Cyc_Core_Opt,v),(struct
_dyneither_ptr){_tmp1E0,_tmp1E0,_tmp1E0 + 2},(void*)& Cyc__genrep_326};static
struct _tuple5*Cyc__genarr_1122[1]={& Cyc__gentuple_1121};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep={3,(struct _dyneither_ptr*)&
Cyc__genname_1123,sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_1122),(
void*)((struct _tuple5**)Cyc__genarr_1122),(void*)((struct _tuple5**)Cyc__genarr_1122
+ 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1120={1,1,(void*)((void*)&
Cyc_struct_Core_Opt0List_list_t0Absyn_tvar_t46H22_rep)};struct _tuple17{
unsigned int f1;struct Cyc_Core_Opt*f2;struct Cyc_Core_Opt*f3;int f4;struct Cyc_Core_Opt*
f5;};static struct _tuple6 Cyc__gentuple_1128={offsetof(struct _tuple17,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1129={offsetof(struct _tuple17,f2),(
void*)& Cyc__genrep_1124};static struct _tuple6 Cyc__gentuple_1130={offsetof(struct
_tuple17,f3),(void*)& Cyc__genrep_110};static struct _tuple6 Cyc__gentuple_1131={
offsetof(struct _tuple17,f4),(void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_1132={
offsetof(struct _tuple17,f5),(void*)& Cyc__genrep_1120};static struct _tuple6*Cyc__genarr_1133[
5]={& Cyc__gentuple_1128,& Cyc__gentuple_1129,& Cyc__gentuple_1130,& Cyc__gentuple_1131,&
Cyc__gentuple_1132};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1119={4,
sizeof(struct _tuple17),{(void*)((struct _tuple6**)Cyc__genarr_1133),(void*)((
struct _tuple6**)Cyc__genarr_1133),(void*)((struct _tuple6**)Cyc__genarr_1133 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1115;struct _tuple18{unsigned int
f1;struct Cyc_Absyn_Tvar*f2;};static struct _tuple6 Cyc__gentuple_1116={offsetof(
struct _tuple18,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1117={
offsetof(struct _tuple18,f2),(void*)& Cyc__genrep_170};static struct _tuple6*Cyc__genarr_1118[
2]={& Cyc__gentuple_1116,& Cyc__gentuple_1117};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1115={4,sizeof(struct _tuple18),{(void*)((struct _tuple6**)Cyc__genarr_1118),(
void*)((struct _tuple6**)Cyc__genarr_1118),(void*)((struct _tuple6**)Cyc__genarr_1118
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1088;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_tunion_info_t_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1095;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionInfo_rep;static char _tmp1E5[18]="UnknownTunionInfo";
static struct _dyneither_ptr Cyc__genname_1100=(struct _dyneither_ptr){_tmp1E5,
_tmp1E5,_tmp1E5 + 18};static char _tmp1E6[5]="name";static struct _tuple5 Cyc__gentuple_1096={
offsetof(struct Cyc_Absyn_UnknownTunionInfo,name),(struct _dyneither_ptr){_tmp1E6,
_tmp1E6,_tmp1E6 + 5},(void*)& Cyc__genrep_11};static char _tmp1E7[11]="is_xtunion";
static struct _tuple5 Cyc__gentuple_1097={offsetof(struct Cyc_Absyn_UnknownTunionInfo,is_xtunion),(
struct _dyneither_ptr){_tmp1E7,_tmp1E7,_tmp1E7 + 11},(void*)& Cyc__genrep_10};
static char _tmp1E8[8]="is_flat";static struct _tuple5 Cyc__gentuple_1098={offsetof(
struct Cyc_Absyn_UnknownTunionInfo,is_flat),(struct _dyneither_ptr){_tmp1E8,
_tmp1E8,_tmp1E8 + 8},(void*)& Cyc__genrep_10};static struct _tuple5*Cyc__genarr_1099[
3]={& Cyc__gentuple_1096,& Cyc__gentuple_1097,& Cyc__gentuple_1098};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionInfo_rep={3,(struct _dyneither_ptr*)& Cyc__genname_1100,
sizeof(struct Cyc_Absyn_UnknownTunionInfo),{(void*)((struct _tuple5**)Cyc__genarr_1099),(
void*)((struct _tuple5**)Cyc__genarr_1099),(void*)((struct _tuple5**)Cyc__genarr_1099
+ 3)}};struct _tuple19{unsigned int f1;struct Cyc_Absyn_UnknownTunionInfo f2;};
static struct _tuple6 Cyc__gentuple_1101={offsetof(struct _tuple19,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1102={offsetof(struct _tuple19,f2),(void*)& Cyc_struct_Absyn_UnknownTunionInfo_rep};
static struct _tuple6*Cyc__genarr_1103[2]={& Cyc__gentuple_1101,& Cyc__gentuple_1102};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1095={4,sizeof(struct _tuple19),{(
void*)((struct _tuple6**)Cyc__genarr_1103),(void*)((struct _tuple6**)Cyc__genarr_1103),(
void*)((struct _tuple6**)Cyc__genarr_1103 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1090;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1091;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_316;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Tuniondecl_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_317;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_318;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_299;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tunionfield_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_300;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_301;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_302;static struct
_tuple6 Cyc__gentuple_303={offsetof(struct _tuple3,f1),(void*)& Cyc_Absyn_tqual_t_rep};
static struct _tuple6 Cyc__gentuple_304={offsetof(struct _tuple3,f2),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_305[2]={& Cyc__gentuple_303,&
Cyc__gentuple_304};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_302={4,
sizeof(struct _tuple3),{(void*)((struct _tuple6**)Cyc__genarr_305),(void*)((struct
_tuple6**)Cyc__genarr_305),(void*)((struct _tuple6**)Cyc__genarr_305 + 2)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_301={1,1,(void*)((void*)& Cyc__genrep_302)};
static char _tmp1ED[5]="List";static struct _dyneither_ptr Cyc__genname_309=(struct
_dyneither_ptr){_tmp1ED,_tmp1ED,_tmp1ED + 5};static char _tmp1EE[3]="hd";static
struct _tuple5 Cyc__gentuple_306={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp1EE,_tmp1EE,_tmp1EE + 3},(void*)& Cyc__genrep_301};static char
_tmp1EF[3]="tl";static struct _tuple5 Cyc__gentuple_307={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp1EF,_tmp1EF,_tmp1EF + 3},(void*)& Cyc__genrep_300};
static struct _tuple5*Cyc__genarr_308[2]={& Cyc__gentuple_306,& Cyc__gentuple_307};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_309,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_308),(void*)((struct _tuple5**)Cyc__genarr_308),(void*)((
struct _tuple5**)Cyc__genarr_308 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_300={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_tqual_t4Absyn_type_t1_446H2_rep)};
static char _tmp1F2[12]="Tunionfield";static struct _dyneither_ptr Cyc__genname_315=(
struct _dyneither_ptr){_tmp1F2,_tmp1F2,_tmp1F2 + 12};static char _tmp1F3[5]="name";
static struct _tuple5 Cyc__gentuple_310={offsetof(struct Cyc_Absyn_Tunionfield,name),(
struct _dyneither_ptr){_tmp1F3,_tmp1F3,_tmp1F3 + 5},(void*)& Cyc__genrep_11};static
char _tmp1F4[5]="typs";static struct _tuple5 Cyc__gentuple_311={offsetof(struct Cyc_Absyn_Tunionfield,typs),(
struct _dyneither_ptr){_tmp1F4,_tmp1F4,_tmp1F4 + 5},(void*)& Cyc__genrep_300};
static char _tmp1F5[4]="loc";static struct _tuple5 Cyc__gentuple_312={offsetof(struct
Cyc_Absyn_Tunionfield,loc),(struct _dyneither_ptr){_tmp1F5,_tmp1F5,_tmp1F5 + 4},(
void*)& Cyc__genrep_2};static char _tmp1F6[3]="sc";static struct _tuple5 Cyc__gentuple_313={
offsetof(struct Cyc_Absyn_Tunionfield,sc),(struct _dyneither_ptr){_tmp1F6,_tmp1F6,
_tmp1F6 + 3},(void*)& Cyc_Absyn_scope_t_rep};static struct _tuple5*Cyc__genarr_314[4]={&
Cyc__gentuple_310,& Cyc__gentuple_311,& Cyc__gentuple_312,& Cyc__gentuple_313};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Tunionfield_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_315,sizeof(struct Cyc_Absyn_Tunionfield),{(void*)((
struct _tuple5**)Cyc__genarr_314),(void*)((struct _tuple5**)Cyc__genarr_314),(void*)((
struct _tuple5**)Cyc__genarr_314 + 4)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_299={
1,1,(void*)((void*)& Cyc_struct_Absyn_Tunionfield_rep)};static char _tmp1F9[5]="List";
static struct _dyneither_ptr Cyc__genname_322=(struct _dyneither_ptr){_tmp1F9,
_tmp1F9,_tmp1F9 + 5};static char _tmp1FA[3]="hd";static struct _tuple5 Cyc__gentuple_319={
offsetof(struct Cyc_List_List,hd),(struct _dyneither_ptr){_tmp1FA,_tmp1FA,_tmp1FA + 
3},(void*)& Cyc__genrep_299};static char _tmp1FB[3]="tl";static struct _tuple5 Cyc__gentuple_320={
offsetof(struct Cyc_List_List,tl),(struct _dyneither_ptr){_tmp1FB,_tmp1FB,_tmp1FB + 
3},(void*)& Cyc__genrep_318};static struct _tuple5*Cyc__genarr_321[2]={& Cyc__gentuple_319,&
Cyc__gentuple_320};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_322,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_321),(void*)((struct _tuple5**)Cyc__genarr_321),(void*)((
struct _tuple5**)Cyc__genarr_321 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_318={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_tunionfield_t46H2_rep)};static
char _tmp1FE[4]="Opt";static struct _dyneither_ptr Cyc__genname_325=(struct
_dyneither_ptr){_tmp1FE,_tmp1FE,_tmp1FE + 4};static char _tmp1FF[2]="v";static
struct _tuple5 Cyc__gentuple_323={offsetof(struct Cyc_Core_Opt,v),(struct
_dyneither_ptr){_tmp1FF,_tmp1FF,_tmp1FF + 2},(void*)& Cyc__genrep_318};static
struct _tuple5*Cyc__genarr_324[1]={& Cyc__gentuple_323};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_325,sizeof(struct Cyc_Core_Opt),{(void*)((struct
_tuple5**)Cyc__genarr_324),(void*)((struct _tuple5**)Cyc__genarr_324),(void*)((
struct _tuple5**)Cyc__genarr_324 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_317={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_tunionfield_t46H22_rep)};
static char _tmp202[11]="Tuniondecl";static struct _dyneither_ptr Cyc__genname_338=(
struct _dyneither_ptr){_tmp202,_tmp202,_tmp202 + 11};static char _tmp203[3]="sc";
static struct _tuple5 Cyc__gentuple_331={offsetof(struct Cyc_Absyn_Tuniondecl,sc),(
struct _dyneither_ptr){_tmp203,_tmp203,_tmp203 + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp204[5]="name";static struct _tuple5 Cyc__gentuple_332={offsetof(
struct Cyc_Absyn_Tuniondecl,name),(struct _dyneither_ptr){_tmp204,_tmp204,_tmp204 + 
5},(void*)& Cyc__genrep_11};static char _tmp205[4]="tvs";static struct _tuple5 Cyc__gentuple_333={
offsetof(struct Cyc_Absyn_Tuniondecl,tvs),(struct _dyneither_ptr){_tmp205,_tmp205,
_tmp205 + 4},(void*)& Cyc__genrep_326};static char _tmp206[7]="fields";static struct
_tuple5 Cyc__gentuple_334={offsetof(struct Cyc_Absyn_Tuniondecl,fields),(struct
_dyneither_ptr){_tmp206,_tmp206,_tmp206 + 7},(void*)& Cyc__genrep_317};static char
_tmp207[11]="is_xtunion";static struct _tuple5 Cyc__gentuple_335={offsetof(struct
Cyc_Absyn_Tuniondecl,is_xtunion),(struct _dyneither_ptr){_tmp207,_tmp207,_tmp207 + 
11},(void*)& Cyc__genrep_10};static char _tmp208[8]="is_flat";static struct _tuple5
Cyc__gentuple_336={offsetof(struct Cyc_Absyn_Tuniondecl,is_flat),(struct
_dyneither_ptr){_tmp208,_tmp208,_tmp208 + 8},(void*)& Cyc__genrep_10};static struct
_tuple5*Cyc__genarr_337[6]={& Cyc__gentuple_331,& Cyc__gentuple_332,& Cyc__gentuple_333,&
Cyc__gentuple_334,& Cyc__gentuple_335,& Cyc__gentuple_336};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Tuniondecl_rep={3,(struct _dyneither_ptr*)& Cyc__genname_338,
sizeof(struct Cyc_Absyn_Tuniondecl),{(void*)((struct _tuple5**)Cyc__genarr_337),(
void*)((struct _tuple5**)Cyc__genarr_337),(void*)((struct _tuple5**)Cyc__genarr_337
+ 6)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_316={1,1,(void*)((void*)&
Cyc_struct_Absyn_Tuniondecl_rep)};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1091={
1,1,(void*)((void*)& Cyc__genrep_316)};struct _tuple20{unsigned int f1;struct Cyc_Absyn_Tuniondecl**
f2;};static struct _tuple6 Cyc__gentuple_1092={offsetof(struct _tuple20,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1093={offsetof(struct _tuple20,f2),(
void*)& Cyc__genrep_1091};static struct _tuple6*Cyc__genarr_1094[2]={& Cyc__gentuple_1092,&
Cyc__gentuple_1093};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1090={4,
sizeof(struct _tuple20),{(void*)((struct _tuple6**)Cyc__genarr_1094),(void*)((
struct _tuple6**)Cyc__genarr_1094),(void*)((struct _tuple6**)Cyc__genarr_1094 + 2)}};
static struct _tuple7*Cyc__genarr_1089[0]={};static char _tmp20D[14]="UnknownTunion";
static struct _tuple5 Cyc__gentuple_1104={0,(struct _dyneither_ptr){_tmp20D,_tmp20D,
_tmp20D + 14},(void*)& Cyc__genrep_1095};static char _tmp20E[12]="KnownTunion";
static struct _tuple5 Cyc__gentuple_1105={1,(struct _dyneither_ptr){_tmp20E,_tmp20E,
_tmp20E + 12},(void*)& Cyc__genrep_1090};static struct _tuple5*Cyc__genarr_1106[2]={&
Cyc__gentuple_1104,& Cyc__gentuple_1105};static char _tmp210[12]="TunionInfoU";
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionInfoU_rep={5,(struct
_dyneither_ptr){_tmp210,_tmp210,_tmp210 + 12},{(void*)((struct _tuple7**)Cyc__genarr_1089),(
void*)((struct _tuple7**)Cyc__genarr_1089),(void*)((struct _tuple7**)Cyc__genarr_1089
+ 0)},{(void*)((struct _tuple5**)Cyc__genarr_1106),(void*)((struct _tuple5**)Cyc__genarr_1106),(
void*)((struct _tuple5**)Cyc__genarr_1106 + 2)}};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_121;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_type_t46H2_rep;
static char _tmp211[5]="List";static struct _dyneither_ptr Cyc__genname_125=(struct
_dyneither_ptr){_tmp211,_tmp211,_tmp211 + 5};static char _tmp212[3]="hd";static
struct _tuple5 Cyc__gentuple_122={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp212,_tmp212,_tmp212 + 3},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp213[3]="tl";static struct _tuple5 Cyc__gentuple_123={offsetof(struct
Cyc_List_List,tl),(struct _dyneither_ptr){_tmp213,_tmp213,_tmp213 + 3},(void*)& Cyc__genrep_121};
static struct _tuple5*Cyc__genarr_124[2]={& Cyc__gentuple_122,& Cyc__gentuple_123};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_type_t46H2_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_125,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_124),(void*)((struct _tuple5**)Cyc__genarr_124),(void*)((
struct _tuple5**)Cyc__genarr_124 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_121={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_type_t46H2_rep)};static char
_tmp216[11]="TunionInfo";static struct _dyneither_ptr Cyc__genname_1111=(struct
_dyneither_ptr){_tmp216,_tmp216,_tmp216 + 11};static char _tmp217[12]="tunion_info";
static struct _tuple5 Cyc__gentuple_1107={offsetof(struct Cyc_Absyn_TunionInfo,tunion_info),(
struct _dyneither_ptr){_tmp217,_tmp217,_tmp217 + 12},(void*)& Cyc_tunion_Absyn_TunionInfoU_rep};
static char _tmp218[6]="targs";static struct _tuple5 Cyc__gentuple_1108={offsetof(
struct Cyc_Absyn_TunionInfo,targs),(struct _dyneither_ptr){_tmp218,_tmp218,_tmp218
+ 6},(void*)& Cyc__genrep_121};static char _tmp219[4]="rgn";static struct _tuple5 Cyc__gentuple_1109={
offsetof(struct Cyc_Absyn_TunionInfo,rgn),(struct _dyneither_ptr){_tmp219,_tmp219,
_tmp219 + 4},(void*)& Cyc__genrep_110};static struct _tuple5*Cyc__genarr_1110[3]={&
Cyc__gentuple_1107,& Cyc__gentuple_1108,& Cyc__gentuple_1109};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_tunion_info_t_rep={3,(struct _dyneither_ptr*)& Cyc__genname_1111,sizeof(
struct Cyc_Absyn_TunionInfo),{(void*)((struct _tuple5**)Cyc__genarr_1110),(void*)((
struct _tuple5**)Cyc__genarr_1110),(void*)((struct _tuple5**)Cyc__genarr_1110 + 3)}};
struct _tuple21{unsigned int f1;struct Cyc_Absyn_TunionInfo f2;};static struct _tuple6
Cyc__gentuple_1112={offsetof(struct _tuple21,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_1113={offsetof(struct _tuple21,f2),(void*)& Cyc_Absyn_tunion_info_t_rep};
static struct _tuple6*Cyc__genarr_1114[2]={& Cyc__gentuple_1112,& Cyc__gentuple_1113};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1088={4,sizeof(struct _tuple21),{(
void*)((struct _tuple6**)Cyc__genarr_1114),(void*)((struct _tuple6**)Cyc__genarr_1114),(
void*)((struct _tuple6**)Cyc__genarr_1114 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1062;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_tunion_field_info_t_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionFieldInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1069;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_UnknownTunionFieldInfo_rep;static char _tmp21C[23]="UnknownTunionFieldInfo";
static struct _dyneither_ptr Cyc__genname_1074=(struct _dyneither_ptr){_tmp21C,
_tmp21C,_tmp21C + 23};static char _tmp21D[12]="tunion_name";static struct _tuple5 Cyc__gentuple_1070={
offsetof(struct Cyc_Absyn_UnknownTunionFieldInfo,tunion_name),(struct
_dyneither_ptr){_tmp21D,_tmp21D,_tmp21D + 12},(void*)& Cyc__genrep_11};static char
_tmp21E[11]="field_name";static struct _tuple5 Cyc__gentuple_1071={offsetof(struct
Cyc_Absyn_UnknownTunionFieldInfo,field_name),(struct _dyneither_ptr){_tmp21E,
_tmp21E,_tmp21E + 11},(void*)& Cyc__genrep_11};static char _tmp21F[11]="is_xtunion";
static struct _tuple5 Cyc__gentuple_1072={offsetof(struct Cyc_Absyn_UnknownTunionFieldInfo,is_xtunion),(
struct _dyneither_ptr){_tmp21F,_tmp21F,_tmp21F + 11},(void*)& Cyc__genrep_10};
static struct _tuple5*Cyc__genarr_1073[3]={& Cyc__gentuple_1070,& Cyc__gentuple_1071,&
Cyc__gentuple_1072};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_UnknownTunionFieldInfo_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_1074,sizeof(struct Cyc_Absyn_UnknownTunionFieldInfo),{(
void*)((struct _tuple5**)Cyc__genarr_1073),(void*)((struct _tuple5**)Cyc__genarr_1073),(
void*)((struct _tuple5**)Cyc__genarr_1073 + 3)}};struct _tuple22{unsigned int f1;
struct Cyc_Absyn_UnknownTunionFieldInfo f2;};static struct _tuple6 Cyc__gentuple_1075={
offsetof(struct _tuple22,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1076={
offsetof(struct _tuple22,f2),(void*)& Cyc_struct_Absyn_UnknownTunionFieldInfo_rep};
static struct _tuple6*Cyc__genarr_1077[2]={& Cyc__gentuple_1075,& Cyc__gentuple_1076};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1069={4,sizeof(struct _tuple22),{(
void*)((struct _tuple6**)Cyc__genarr_1077),(void*)((struct _tuple6**)Cyc__genarr_1077),(
void*)((struct _tuple6**)Cyc__genarr_1077 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1064;struct _tuple23{unsigned int f1;struct Cyc_Absyn_Tuniondecl*f2;
struct Cyc_Absyn_Tunionfield*f3;};static struct _tuple6 Cyc__gentuple_1065={
offsetof(struct _tuple23,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1066={
offsetof(struct _tuple23,f2),(void*)((void*)& Cyc__genrep_316)};static struct
_tuple6 Cyc__gentuple_1067={offsetof(struct _tuple23,f3),(void*)& Cyc__genrep_299};
static struct _tuple6*Cyc__genarr_1068[3]={& Cyc__gentuple_1065,& Cyc__gentuple_1066,&
Cyc__gentuple_1067};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1064={4,
sizeof(struct _tuple23),{(void*)((struct _tuple6**)Cyc__genarr_1068),(void*)((
struct _tuple6**)Cyc__genarr_1068),(void*)((struct _tuple6**)Cyc__genarr_1068 + 3)}};
static struct _tuple7*Cyc__genarr_1063[0]={};static char _tmp223[19]="UnknownTunionfield";
static struct _tuple5 Cyc__gentuple_1078={0,(struct _dyneither_ptr){_tmp223,_tmp223,
_tmp223 + 19},(void*)& Cyc__genrep_1069};static char _tmp224[17]="KnownTunionfield";
static struct _tuple5 Cyc__gentuple_1079={1,(struct _dyneither_ptr){_tmp224,_tmp224,
_tmp224 + 17},(void*)& Cyc__genrep_1064};static struct _tuple5*Cyc__genarr_1080[2]={&
Cyc__gentuple_1078,& Cyc__gentuple_1079};static char _tmp226[17]="TunionFieldInfoU";
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_TunionFieldInfoU_rep={5,(struct
_dyneither_ptr){_tmp226,_tmp226,_tmp226 + 17},{(void*)((struct _tuple7**)Cyc__genarr_1063),(
void*)((struct _tuple7**)Cyc__genarr_1063),(void*)((struct _tuple7**)Cyc__genarr_1063
+ 0)},{(void*)((struct _tuple5**)Cyc__genarr_1080),(void*)((struct _tuple5**)Cyc__genarr_1080),(
void*)((struct _tuple5**)Cyc__genarr_1080 + 2)}};static char _tmp227[16]="TunionFieldInfo";
static struct _dyneither_ptr Cyc__genname_1084=(struct _dyneither_ptr){_tmp227,
_tmp227,_tmp227 + 16};static char _tmp228[11]="field_info";static struct _tuple5 Cyc__gentuple_1081={
offsetof(struct Cyc_Absyn_TunionFieldInfo,field_info),(struct _dyneither_ptr){
_tmp228,_tmp228,_tmp228 + 11},(void*)& Cyc_tunion_Absyn_TunionFieldInfoU_rep};
static char _tmp229[6]="targs";static struct _tuple5 Cyc__gentuple_1082={offsetof(
struct Cyc_Absyn_TunionFieldInfo,targs),(struct _dyneither_ptr){_tmp229,_tmp229,
_tmp229 + 6},(void*)& Cyc__genrep_121};static struct _tuple5*Cyc__genarr_1083[2]={&
Cyc__gentuple_1081,& Cyc__gentuple_1082};struct Cyc_Typerep_Struct_struct Cyc_Absyn_tunion_field_info_t_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_1084,sizeof(struct Cyc_Absyn_TunionFieldInfo),{(
void*)((struct _tuple5**)Cyc__genarr_1083),(void*)((struct _tuple5**)Cyc__genarr_1083),(
void*)((struct _tuple5**)Cyc__genarr_1083 + 2)}};struct _tuple24{unsigned int f1;
struct Cyc_Absyn_TunionFieldInfo f2;};static struct _tuple6 Cyc__gentuple_1085={
offsetof(struct _tuple24,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1086={
offsetof(struct _tuple24,f2),(void*)& Cyc_Absyn_tunion_field_info_t_rep};static
struct _tuple6*Cyc__genarr_1087[2]={& Cyc__gentuple_1085,& Cyc__gentuple_1086};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1062={4,sizeof(struct _tuple24),{(
void*)((struct _tuple6**)Cyc__genarr_1087),(void*)((struct _tuple6**)Cyc__genarr_1087),(
void*)((struct _tuple6**)Cyc__genarr_1087 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1036;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_info_t_rep;
extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_atts_t_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1001;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Conref0bool2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0bool2_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_85;struct _tuple25{unsigned int f1;
int f2;};static struct _tuple6 Cyc__gentuple_86={offsetof(struct _tuple25,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_87={offsetof(struct _tuple25,f2),(
void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_88[2]={& Cyc__gentuple_86,&
Cyc__gentuple_87};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_85={4,sizeof(
struct _tuple25),{(void*)((struct _tuple6**)Cyc__genarr_88),(void*)((struct _tuple6**)
Cyc__genarr_88),(void*)((struct _tuple6**)Cyc__genarr_88 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1004;struct _tuple26{unsigned int f1;struct Cyc_Absyn_Conref*f2;};
static struct _tuple6 Cyc__gentuple_1005={offsetof(struct _tuple26,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1006={offsetof(struct _tuple26,f2),(void*)& Cyc__genrep_1001};
static struct _tuple6*Cyc__genarr_1007[2]={& Cyc__gentuple_1005,& Cyc__gentuple_1006};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1004={4,sizeof(struct _tuple26),{(
void*)((struct _tuple6**)Cyc__genarr_1007),(void*)((struct _tuple6**)Cyc__genarr_1007),(
void*)((struct _tuple6**)Cyc__genarr_1007 + 2)}};static char _tmp22E[10]="No_constr";
static struct _tuple7 Cyc__gentuple_1002={0,(struct _dyneither_ptr){_tmp22E,_tmp22E,
_tmp22E + 10}};static struct _tuple7*Cyc__genarr_1003[1]={& Cyc__gentuple_1002};
static char _tmp22F[10]="Eq_constr";static struct _tuple5 Cyc__gentuple_1008={0,(
struct _dyneither_ptr){_tmp22F,_tmp22F,_tmp22F + 10},(void*)& Cyc__genrep_85};
static char _tmp230[15]="Forward_constr";static struct _tuple5 Cyc__gentuple_1009={1,(
struct _dyneither_ptr){_tmp230,_tmp230,_tmp230 + 15},(void*)& Cyc__genrep_1004};
static struct _tuple5*Cyc__genarr_1010[2]={& Cyc__gentuple_1008,& Cyc__gentuple_1009};
static char _tmp232[11]="Constraint";struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Constraint0bool2_rep={
5,(struct _dyneither_ptr){_tmp232,_tmp232,_tmp232 + 11},{(void*)((struct _tuple7**)
Cyc__genarr_1003),(void*)((struct _tuple7**)Cyc__genarr_1003),(void*)((struct
_tuple7**)Cyc__genarr_1003 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_1010),(
void*)((struct _tuple5**)Cyc__genarr_1010),(void*)((struct _tuple5**)Cyc__genarr_1010
+ 2)}};static char _tmp233[7]="Conref";static struct _dyneither_ptr Cyc__genname_1013=(
struct _dyneither_ptr){_tmp233,_tmp233,_tmp233 + 7};static char _tmp234[2]="v";
static struct _tuple5 Cyc__gentuple_1011={offsetof(struct Cyc_Absyn_Conref,v),(
struct _dyneither_ptr){_tmp234,_tmp234,_tmp234 + 2},(void*)& Cyc_tunion_Absyn_Constraint0bool2_rep};
static struct _tuple5*Cyc__genarr_1012[1]={& Cyc__gentuple_1011};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0bool2_rep={3,(struct _dyneither_ptr*)& Cyc__genname_1013,
sizeof(struct Cyc_Absyn_Conref),{(void*)((struct _tuple5**)Cyc__genarr_1012),(void*)((
struct _tuple5**)Cyc__genarr_1012),(void*)((struct _tuple5**)Cyc__genarr_1012 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1001={1,1,(void*)((void*)& Cyc_struct_Absyn_Conref0bool2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1043;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep;static char _tmp237[7]="Conref";static
struct _dyneither_ptr Cyc__genname_1046=(struct _dyneither_ptr){_tmp237,_tmp237,
_tmp237 + 7};static char _tmp238[2]="v";static struct _tuple5 Cyc__gentuple_1044={
offsetof(struct Cyc_Absyn_Conref,v),(struct _dyneither_ptr){_tmp238,_tmp238,
_tmp238 + 2},(void*)& Cyc_tunion_Absyn_Constraint0bool2_rep};static struct _tuple5*
Cyc__genarr_1045[1]={& Cyc__gentuple_1044};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_1046,sizeof(struct Cyc_Absyn_Conref),{(
void*)((struct _tuple5**)Cyc__genarr_1045),(void*)((struct _tuple5**)Cyc__genarr_1045),(
void*)((struct _tuple5**)Cyc__genarr_1045 + 1)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1043={1,1,(void*)((void*)& Cyc_struct_Absyn_Conref0Absyn_bounds_t2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_1037;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_PtrLoc_rep;static char _tmp23B[7]="PtrLoc";static struct
_dyneither_ptr Cyc__genname_1042=(struct _dyneither_ptr){_tmp23B,_tmp23B,_tmp23B + 
7};static char _tmp23C[8]="ptr_loc";static struct _tuple5 Cyc__gentuple_1038={
offsetof(struct Cyc_Absyn_PtrLoc,ptr_loc),(struct _dyneither_ptr){_tmp23C,_tmp23C,
_tmp23C + 8},(void*)& Cyc__genrep_2};static char _tmp23D[8]="rgn_loc";static struct
_tuple5 Cyc__gentuple_1039={offsetof(struct Cyc_Absyn_PtrLoc,rgn_loc),(struct
_dyneither_ptr){_tmp23D,_tmp23D,_tmp23D + 8},(void*)& Cyc__genrep_2};static char
_tmp23E[7]="zt_loc";static struct _tuple5 Cyc__gentuple_1040={offsetof(struct Cyc_Absyn_PtrLoc,zt_loc),(
struct _dyneither_ptr){_tmp23E,_tmp23E,_tmp23E + 7},(void*)& Cyc__genrep_2};static
struct _tuple5*Cyc__genarr_1041[3]={& Cyc__gentuple_1038,& Cyc__gentuple_1039,& Cyc__gentuple_1040};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_PtrLoc_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_1042,sizeof(struct Cyc_Absyn_PtrLoc),{(void*)((
struct _tuple5**)Cyc__genarr_1041),(void*)((struct _tuple5**)Cyc__genarr_1041),(
void*)((struct _tuple5**)Cyc__genarr_1041 + 3)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1037={1,1,(void*)((void*)& Cyc_struct_Absyn_PtrLoc_rep)};static char
_tmp241[8]="PtrAtts";static struct _dyneither_ptr Cyc__genname_1053=(struct
_dyneither_ptr){_tmp241,_tmp241,_tmp241 + 8};static char _tmp242[4]="rgn";static
struct _tuple5 Cyc__gentuple_1047={offsetof(struct Cyc_Absyn_PtrAtts,rgn),(struct
_dyneither_ptr){_tmp242,_tmp242,_tmp242 + 4},(void*)((void*)& Cyc_Absyn_type_t_rep)};
static char _tmp243[9]="nullable";static struct _tuple5 Cyc__gentuple_1048={offsetof(
struct Cyc_Absyn_PtrAtts,nullable),(struct _dyneither_ptr){_tmp243,_tmp243,_tmp243
+ 9},(void*)& Cyc__genrep_1001};static char _tmp244[7]="bounds";static struct _tuple5
Cyc__gentuple_1049={offsetof(struct Cyc_Absyn_PtrAtts,bounds),(struct
_dyneither_ptr){_tmp244,_tmp244,_tmp244 + 7},(void*)& Cyc__genrep_1043};static char
_tmp245[10]="zero_term";static struct _tuple5 Cyc__gentuple_1050={offsetof(struct
Cyc_Absyn_PtrAtts,zero_term),(struct _dyneither_ptr){_tmp245,_tmp245,_tmp245 + 10},(
void*)& Cyc__genrep_1001};static char _tmp246[7]="ptrloc";static struct _tuple5 Cyc__gentuple_1051={
offsetof(struct Cyc_Absyn_PtrAtts,ptrloc),(struct _dyneither_ptr){_tmp246,_tmp246,
_tmp246 + 7},(void*)& Cyc__genrep_1037};static struct _tuple5*Cyc__genarr_1052[5]={&
Cyc__gentuple_1047,& Cyc__gentuple_1048,& Cyc__gentuple_1049,& Cyc__gentuple_1050,&
Cyc__gentuple_1051};struct Cyc_Typerep_Struct_struct Cyc_Absyn_ptr_atts_t_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_1053,sizeof(struct Cyc_Absyn_PtrAtts),{(void*)((
struct _tuple5**)Cyc__genarr_1052),(void*)((struct _tuple5**)Cyc__genarr_1052),(
void*)((struct _tuple5**)Cyc__genarr_1052 + 5)}};static char _tmp248[8]="PtrInfo";
static struct _dyneither_ptr Cyc__genname_1058=(struct _dyneither_ptr){_tmp248,
_tmp248,_tmp248 + 8};static char _tmp249[8]="elt_typ";static struct _tuple5 Cyc__gentuple_1054={
offsetof(struct Cyc_Absyn_PtrInfo,elt_typ),(struct _dyneither_ptr){_tmp249,_tmp249,
_tmp249 + 8},(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp24A[7]="elt_tq";
static struct _tuple5 Cyc__gentuple_1055={offsetof(struct Cyc_Absyn_PtrInfo,elt_tq),(
struct _dyneither_ptr){_tmp24A,_tmp24A,_tmp24A + 7},(void*)& Cyc_Absyn_tqual_t_rep};
static char _tmp24B[9]="ptr_atts";static struct _tuple5 Cyc__gentuple_1056={offsetof(
struct Cyc_Absyn_PtrInfo,ptr_atts),(struct _dyneither_ptr){_tmp24B,_tmp24B,_tmp24B
+ 9},(void*)& Cyc_Absyn_ptr_atts_t_rep};static struct _tuple5*Cyc__genarr_1057[3]={&
Cyc__gentuple_1054,& Cyc__gentuple_1055,& Cyc__gentuple_1056};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_ptr_info_t_rep={3,(struct _dyneither_ptr*)& Cyc__genname_1058,sizeof(
struct Cyc_Absyn_PtrInfo),{(void*)((struct _tuple5**)Cyc__genarr_1057),(void*)((
struct _tuple5**)Cyc__genarr_1057),(void*)((struct _tuple5**)Cyc__genarr_1057 + 3)}};
struct _tuple27{unsigned int f1;struct Cyc_Absyn_PtrInfo f2;};static struct _tuple6 Cyc__gentuple_1059={
offsetof(struct _tuple27,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1060={
offsetof(struct _tuple27,f2),(void*)& Cyc_Absyn_ptr_info_t_rep};static struct
_tuple6*Cyc__genarr_1061[2]={& Cyc__gentuple_1059,& Cyc__gentuple_1060};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1036={4,sizeof(struct _tuple27),{(void*)((
struct _tuple6**)Cyc__genarr_1061),(void*)((struct _tuple6**)Cyc__genarr_1061),(
void*)((struct _tuple6**)Cyc__genarr_1061 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1024;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_sign_t_rep;
static char _tmp24E[7]="Signed";static struct _tuple7 Cyc__gentuple_289={0,(struct
_dyneither_ptr){_tmp24E,_tmp24E,_tmp24E + 7}};static char _tmp24F[9]="Unsigned";
static struct _tuple7 Cyc__gentuple_290={1,(struct _dyneither_ptr){_tmp24F,_tmp24F,
_tmp24F + 9}};static char _tmp250[5]="None";static struct _tuple7 Cyc__gentuple_291={2,(
struct _dyneither_ptr){_tmp250,_tmp250,_tmp250 + 5}};static struct _tuple7*Cyc__genarr_292[
3]={& Cyc__gentuple_289,& Cyc__gentuple_290,& Cyc__gentuple_291};static struct
_tuple5*Cyc__genarr_293[0]={};static char _tmp252[5]="Sign";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_sign_t_rep={5,(struct _dyneither_ptr){_tmp252,_tmp252,_tmp252 + 5},{(
void*)((struct _tuple7**)Cyc__genarr_292),(void*)((struct _tuple7**)Cyc__genarr_292),(
void*)((struct _tuple7**)Cyc__genarr_292 + 3)},{(void*)((struct _tuple5**)Cyc__genarr_293),(
void*)((struct _tuple5**)Cyc__genarr_293),(void*)((struct _tuple5**)Cyc__genarr_293
+ 0)}};extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_size_of_t_rep;static char
_tmp253[8]="Char_sz";static struct _tuple7 Cyc__gentuple_1025={0,(struct
_dyneither_ptr){_tmp253,_tmp253,_tmp253 + 8}};static char _tmp254[9]="Short_sz";
static struct _tuple7 Cyc__gentuple_1026={1,(struct _dyneither_ptr){_tmp254,_tmp254,
_tmp254 + 9}};static char _tmp255[7]="Int_sz";static struct _tuple7 Cyc__gentuple_1027={
2,(struct _dyneither_ptr){_tmp255,_tmp255,_tmp255 + 7}};static char _tmp256[8]="Long_sz";
static struct _tuple7 Cyc__gentuple_1028={3,(struct _dyneither_ptr){_tmp256,_tmp256,
_tmp256 + 8}};static char _tmp257[12]="LongLong_sz";static struct _tuple7 Cyc__gentuple_1029={
4,(struct _dyneither_ptr){_tmp257,_tmp257,_tmp257 + 12}};static struct _tuple7*Cyc__genarr_1030[
5]={& Cyc__gentuple_1025,& Cyc__gentuple_1026,& Cyc__gentuple_1027,& Cyc__gentuple_1028,&
Cyc__gentuple_1029};static struct _tuple5*Cyc__genarr_1031[0]={};static char _tmp259[
8]="Size_of";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_size_of_t_rep={5,(struct
_dyneither_ptr){_tmp259,_tmp259,_tmp259 + 8},{(void*)((struct _tuple7**)Cyc__genarr_1030),(
void*)((struct _tuple7**)Cyc__genarr_1030),(void*)((struct _tuple7**)Cyc__genarr_1030
+ 5)},{(void*)((struct _tuple5**)Cyc__genarr_1031),(void*)((struct _tuple5**)Cyc__genarr_1031),(
void*)((struct _tuple5**)Cyc__genarr_1031 + 0)}};struct _tuple28{unsigned int f1;
void*f2;void*f3;};static struct _tuple6 Cyc__gentuple_1032={offsetof(struct _tuple28,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1033={offsetof(struct
_tuple28,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_1034={
offsetof(struct _tuple28,f3),(void*)& Cyc_Absyn_size_of_t_rep};static struct _tuple6*
Cyc__genarr_1035[3]={& Cyc__gentuple_1032,& Cyc__gentuple_1033,& Cyc__gentuple_1034};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1024={4,sizeof(struct _tuple28),{(
void*)((struct _tuple6**)Cyc__genarr_1035),(void*)((struct _tuple6**)Cyc__genarr_1035),(
void*)((struct _tuple6**)Cyc__genarr_1035 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1000;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_array_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_144;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Exp_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_exp_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_866;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_cnst_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_881;struct
_tuple29{unsigned int f1;void*f2;char f3;};static struct _tuple6 Cyc__gentuple_882={
offsetof(struct _tuple29,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_883={
offsetof(struct _tuple29,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_884={
offsetof(struct _tuple29,f3),(void*)((void*)& Cyc__genrep_15)};static struct _tuple6*
Cyc__genarr_885[3]={& Cyc__gentuple_882,& Cyc__gentuple_883,& Cyc__gentuple_884};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_881={4,sizeof(struct _tuple29),{(
void*)((struct _tuple6**)Cyc__genarr_885),(void*)((struct _tuple6**)Cyc__genarr_885),(
void*)((struct _tuple6**)Cyc__genarr_885 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_875;static struct Cyc_Typerep_Int_struct Cyc__genrep_876={0,1,16};
struct _tuple30{unsigned int f1;void*f2;short f3;};static struct _tuple6 Cyc__gentuple_877={
offsetof(struct _tuple30,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_878={
offsetof(struct _tuple30,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_879={
offsetof(struct _tuple30,f3),(void*)& Cyc__genrep_876};static struct _tuple6*Cyc__genarr_880[
3]={& Cyc__gentuple_877,& Cyc__gentuple_878,& Cyc__gentuple_879};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_875={4,sizeof(struct _tuple30),{(void*)((struct _tuple6**)Cyc__genarr_880),(
void*)((struct _tuple6**)Cyc__genarr_880),(void*)((struct _tuple6**)Cyc__genarr_880
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_288;struct _tuple31{
unsigned int f1;void*f2;int f3;};static struct _tuple6 Cyc__gentuple_294={offsetof(
struct _tuple31,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_295={
offsetof(struct _tuple31,f2),(void*)& Cyc_Absyn_sign_t_rep};static struct _tuple6 Cyc__gentuple_296={
offsetof(struct _tuple31,f3),(void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_297[
3]={& Cyc__gentuple_294,& Cyc__gentuple_295,& Cyc__gentuple_296};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_288={4,sizeof(struct _tuple31),{(void*)((struct _tuple6**)Cyc__genarr_297),(
void*)((struct _tuple6**)Cyc__genarr_297),(void*)((struct _tuple6**)Cyc__genarr_297
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_869;static struct Cyc_Typerep_Int_struct
Cyc__genrep_870={0,1,64};struct _tuple32{unsigned int f1;void*f2;long long f3;};
static struct _tuple6 Cyc__gentuple_871={offsetof(struct _tuple32,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_872={offsetof(struct _tuple32,f2),(void*)& Cyc_Absyn_sign_t_rep};
static struct _tuple6 Cyc__gentuple_873={offsetof(struct _tuple32,f3),(void*)& Cyc__genrep_870};
static struct _tuple6*Cyc__genarr_874[3]={& Cyc__gentuple_871,& Cyc__gentuple_872,&
Cyc__gentuple_873};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_869={4,
sizeof(struct _tuple32),{(void*)((struct _tuple6**)Cyc__genarr_874),(void*)((
struct _tuple6**)Cyc__genarr_874),(void*)((struct _tuple6**)Cyc__genarr_874 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_81;static struct _tuple6 Cyc__gentuple_82={
offsetof(struct _tuple7,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_83={
offsetof(struct _tuple7,f2),(void*)((void*)& Cyc__genrep_14)};static struct _tuple6*
Cyc__genarr_84[2]={& Cyc__gentuple_82,& Cyc__gentuple_83};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_81={4,sizeof(struct _tuple7),{(void*)((struct _tuple6**)Cyc__genarr_84),(
void*)((struct _tuple6**)Cyc__genarr_84),(void*)((struct _tuple6**)Cyc__genarr_84 + 
2)}};static char _tmp262[7]="Null_c";static struct _tuple7 Cyc__gentuple_867={0,(
struct _dyneither_ptr){_tmp262,_tmp262,_tmp262 + 7}};static struct _tuple7*Cyc__genarr_868[
1]={& Cyc__gentuple_867};static char _tmp263[7]="Char_c";static struct _tuple5 Cyc__gentuple_886={
0,(struct _dyneither_ptr){_tmp263,_tmp263,_tmp263 + 7},(void*)& Cyc__genrep_881};
static char _tmp264[8]="Short_c";static struct _tuple5 Cyc__gentuple_887={1,(struct
_dyneither_ptr){_tmp264,_tmp264,_tmp264 + 8},(void*)& Cyc__genrep_875};static char
_tmp265[6]="Int_c";static struct _tuple5 Cyc__gentuple_888={2,(struct _dyneither_ptr){
_tmp265,_tmp265,_tmp265 + 6},(void*)& Cyc__genrep_288};static char _tmp266[11]="LongLong_c";
static struct _tuple5 Cyc__gentuple_889={3,(struct _dyneither_ptr){_tmp266,_tmp266,
_tmp266 + 11},(void*)& Cyc__genrep_869};static char _tmp267[8]="Float_c";static
struct _tuple5 Cyc__gentuple_890={4,(struct _dyneither_ptr){_tmp267,_tmp267,_tmp267
+ 8},(void*)& Cyc__genrep_81};static char _tmp268[9]="String_c";static struct _tuple5
Cyc__gentuple_891={5,(struct _dyneither_ptr){_tmp268,_tmp268,_tmp268 + 9},(void*)&
Cyc__genrep_81};static struct _tuple5*Cyc__genarr_892[6]={& Cyc__gentuple_886,& Cyc__gentuple_887,&
Cyc__gentuple_888,& Cyc__gentuple_889,& Cyc__gentuple_890,& Cyc__gentuple_891};
static char _tmp26A[5]="Cnst";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_cnst_t_rep={
5,(struct _dyneither_ptr){_tmp26A,_tmp26A,_tmp26A + 5},{(void*)((struct _tuple7**)
Cyc__genarr_868),(void*)((struct _tuple7**)Cyc__genarr_868),(void*)((struct
_tuple7**)Cyc__genarr_868 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_892),(void*)((
struct _tuple5**)Cyc__genarr_892),(void*)((struct _tuple5**)Cyc__genarr_892 + 6)}};
struct _tuple33{unsigned int f1;union Cyc_Absyn_Cnst_union f2;};static struct _tuple6
Cyc__gentuple_893={offsetof(struct _tuple33,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_894={offsetof(struct _tuple33,f2),(void*)& Cyc_Absyn_cnst_t_rep};
static struct _tuple6*Cyc__genarr_895[2]={& Cyc__gentuple_893,& Cyc__gentuple_894};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_866={4,sizeof(struct _tuple33),{(
void*)((struct _tuple6**)Cyc__genarr_895),(void*)((struct _tuple6**)Cyc__genarr_895),(
void*)((struct _tuple6**)Cyc__genarr_895 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_814;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_binding_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_821;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_822;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Fndecl_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_825;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep;extern
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_826;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_827;struct _tuple34{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;
void*f3;};static struct _tuple6 Cyc__gentuple_828={offsetof(struct _tuple34,f1),(
void*)& Cyc__genrep_13};static struct _tuple6 Cyc__gentuple_829={offsetof(struct
_tuple34,f2),(void*)& Cyc_Absyn_tqual_t_rep};static struct _tuple6 Cyc__gentuple_830={
offsetof(struct _tuple34,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_831[3]={& Cyc__gentuple_828,& Cyc__gentuple_829,& Cyc__gentuple_830};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_827={4,sizeof(struct _tuple34),{(
void*)((struct _tuple6**)Cyc__genarr_831),(void*)((struct _tuple6**)Cyc__genarr_831),(
void*)((struct _tuple6**)Cyc__genarr_831 + 3)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_826={1,1,(void*)((void*)& Cyc__genrep_827)};static char _tmp26E[5]="List";
static struct _dyneither_ptr Cyc__genname_835=(struct _dyneither_ptr){_tmp26E,
_tmp26E,_tmp26E + 5};static char _tmp26F[3]="hd";static struct _tuple5 Cyc__gentuple_832={
offsetof(struct Cyc_List_List,hd),(struct _dyneither_ptr){_tmp26F,_tmp26F,_tmp26F + 
3},(void*)& Cyc__genrep_826};static char _tmp270[3]="tl";static struct _tuple5 Cyc__gentuple_833={
offsetof(struct Cyc_List_List,tl),(struct _dyneither_ptr){_tmp270,_tmp270,_tmp270 + 
3},(void*)& Cyc__genrep_825};static struct _tuple5*Cyc__genarr_834[2]={& Cyc__gentuple_832,&
Cyc__gentuple_833};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_835,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_834),(void*)((struct _tuple5**)Cyc__genarr_834),(void*)((
struct _tuple5**)Cyc__genarr_834 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_825={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_var_t4Absyn_tqual_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_824;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_vararg_info_t_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_674;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_var_t2_rep;static
char _tmp273[4]="Opt";static struct _dyneither_ptr Cyc__genname_677=(struct
_dyneither_ptr){_tmp273,_tmp273,_tmp273 + 4};static char _tmp274[2]="v";static
struct _tuple5 Cyc__gentuple_675={offsetof(struct Cyc_Core_Opt,v),(struct
_dyneither_ptr){_tmp274,_tmp274,_tmp274 + 2},(void*)& Cyc__genrep_13};static struct
_tuple5*Cyc__genarr_676[1]={& Cyc__gentuple_675};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_var_t2_rep={3,(struct _dyneither_ptr*)& Cyc__genname_677,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_676),(void*)((
struct _tuple5**)Cyc__genarr_676),(void*)((struct _tuple5**)Cyc__genarr_676 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_674={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_var_t2_rep)};
static char _tmp277[11]="VarargInfo";static struct _dyneither_ptr Cyc__genname_744=(
struct _dyneither_ptr){_tmp277,_tmp277,_tmp277 + 11};static char _tmp278[5]="name";
static struct _tuple5 Cyc__gentuple_739={offsetof(struct Cyc_Absyn_VarargInfo,name),(
struct _dyneither_ptr){_tmp278,_tmp278,_tmp278 + 5},(void*)& Cyc__genrep_674};
static char _tmp279[3]="tq";static struct _tuple5 Cyc__gentuple_740={offsetof(struct
Cyc_Absyn_VarargInfo,tq),(struct _dyneither_ptr){_tmp279,_tmp279,_tmp279 + 3},(
void*)& Cyc_Absyn_tqual_t_rep};static char _tmp27A[5]="type";static struct _tuple5 Cyc__gentuple_741={
offsetof(struct Cyc_Absyn_VarargInfo,type),(struct _dyneither_ptr){_tmp27A,_tmp27A,
_tmp27A + 5},(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp27B[7]="inject";
static struct _tuple5 Cyc__gentuple_742={offsetof(struct Cyc_Absyn_VarargInfo,inject),(
struct _dyneither_ptr){_tmp27B,_tmp27B,_tmp27B + 7},(void*)& Cyc__genrep_10};static
struct _tuple5*Cyc__genarr_743[4]={& Cyc__gentuple_739,& Cyc__gentuple_740,& Cyc__gentuple_741,&
Cyc__gentuple_742};struct Cyc_Typerep_Struct_struct Cyc_Absyn_vararg_info_t_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_744,sizeof(struct Cyc_Absyn_VarargInfo),{(
void*)((struct _tuple5**)Cyc__genarr_743),(void*)((struct _tuple5**)Cyc__genarr_743),(
void*)((struct _tuple5**)Cyc__genarr_743 + 4)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_824={1,1,(void*)((void*)& Cyc_Absyn_vararg_info_t_rep)};extern struct
Cyc_Typerep_ThinPtr_struct Cyc__genrep_387;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_388;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_389;static struct
_tuple6 Cyc__gentuple_390={offsetof(struct _tuple9,f1),(void*)((void*)& Cyc_Absyn_type_t_rep)};
static struct _tuple6 Cyc__gentuple_391={offsetof(struct _tuple9,f2),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_392[2]={& Cyc__gentuple_390,&
Cyc__gentuple_391};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_389={4,
sizeof(struct _tuple9),{(void*)((struct _tuple6**)Cyc__genarr_392),(void*)((struct
_tuple6**)Cyc__genarr_392),(void*)((struct _tuple6**)Cyc__genarr_392 + 2)}};static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_388={1,1,(void*)((void*)& Cyc__genrep_389)};
static char _tmp280[5]="List";static struct _dyneither_ptr Cyc__genname_396=(struct
_dyneither_ptr){_tmp280,_tmp280,_tmp280 + 5};static char _tmp281[3]="hd";static
struct _tuple5 Cyc__gentuple_393={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp281,_tmp281,_tmp281 + 3},(void*)& Cyc__genrep_388};static char
_tmp282[3]="tl";static struct _tuple5 Cyc__gentuple_394={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp282,_tmp282,_tmp282 + 3},(void*)& Cyc__genrep_387};
static struct _tuple5*Cyc__genarr_395[2]={& Cyc__gentuple_393,& Cyc__gentuple_394};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_396,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_395),(void*)((struct _tuple5**)Cyc__genarr_395),(void*)((
struct _tuple5**)Cyc__genarr_395 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_387={
1,1,(void*)((void*)& Cyc_struct_List_List060Absyn_type_t4Absyn_type_t1_446H2_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_134;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Stmt_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_stmt_t_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_129;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_130;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_130={1,1,(void*)((
void*)& Cyc_struct_Absyn_Exp_rep)};struct _tuple35{unsigned int f1;struct Cyc_Absyn_Exp*
f2;};static struct _tuple6 Cyc__gentuple_943={offsetof(struct _tuple35,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_944={offsetof(struct _tuple35,f2),(
void*)& Cyc__genrep_130};static struct _tuple6*Cyc__genarr_945[2]={& Cyc__gentuple_943,&
Cyc__gentuple_944};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_129={4,
sizeof(struct _tuple35),{(void*)((struct _tuple6**)Cyc__genarr_945),(void*)((
struct _tuple6**)Cyc__genarr_945),(void*)((struct _tuple6**)Cyc__genarr_945 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_557;struct _tuple36{unsigned int
f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};static struct _tuple6 Cyc__gentuple_558={
offsetof(struct _tuple36,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_559={
offsetof(struct _tuple36,f2),(void*)& Cyc__genrep_134};static struct _tuple6 Cyc__gentuple_560={
offsetof(struct _tuple36,f3),(void*)& Cyc__genrep_134};static struct _tuple6*Cyc__genarr_561[
3]={& Cyc__gentuple_558,& Cyc__gentuple_559,& Cyc__gentuple_560};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_557={4,sizeof(struct _tuple36),{(void*)((struct _tuple6**)Cyc__genarr_561),(
void*)((struct _tuple6**)Cyc__genarr_561),(void*)((struct _tuple6**)Cyc__genarr_561
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_553;static struct _tuple6
Cyc__gentuple_554={offsetof(struct _tuple35,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_555={offsetof(struct _tuple35,f2),(void*)& Cyc__genrep_144};
static struct _tuple6*Cyc__genarr_556[2]={& Cyc__gentuple_554,& Cyc__gentuple_555};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_553={4,sizeof(struct _tuple35),{(
void*)((struct _tuple6**)Cyc__genarr_556),(void*)((struct _tuple6**)Cyc__genarr_556),(
void*)((struct _tuple6**)Cyc__genarr_556 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_547;struct _tuple37{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Stmt*
f3;struct Cyc_Absyn_Stmt*f4;};static struct _tuple6 Cyc__gentuple_548={offsetof(
struct _tuple37,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_549={
offsetof(struct _tuple37,f2),(void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_550={
offsetof(struct _tuple37,f3),(void*)& Cyc__genrep_134};static struct _tuple6 Cyc__gentuple_551={
offsetof(struct _tuple37,f4),(void*)& Cyc__genrep_134};static struct _tuple6*Cyc__genarr_552[
4]={& Cyc__gentuple_548,& Cyc__gentuple_549,& Cyc__gentuple_550,& Cyc__gentuple_551};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_547={4,sizeof(struct _tuple37),{(
void*)((struct _tuple6**)Cyc__genarr_552),(void*)((struct _tuple6**)Cyc__genarr_552),(
void*)((struct _tuple6**)Cyc__genarr_552 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_542;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_491;static struct
_tuple6 Cyc__gentuple_492={offsetof(struct _tuple2,f1),(void*)& Cyc__genrep_130};
static struct _tuple6 Cyc__gentuple_493={offsetof(struct _tuple2,f2),(void*)& Cyc__genrep_134};
static struct _tuple6*Cyc__genarr_494[2]={& Cyc__gentuple_492,& Cyc__gentuple_493};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_491={4,sizeof(struct _tuple2),{(
void*)((struct _tuple6**)Cyc__genarr_494),(void*)((struct _tuple6**)Cyc__genarr_494),(
void*)((struct _tuple6**)Cyc__genarr_494 + 2)}};struct _tuple38{unsigned int f1;
struct _tuple2 f2;struct Cyc_Absyn_Stmt*f3;};static struct _tuple6 Cyc__gentuple_543={
offsetof(struct _tuple38,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_544={
offsetof(struct _tuple38,f2),(void*)& Cyc__genrep_491};static struct _tuple6 Cyc__gentuple_545={
offsetof(struct _tuple38,f3),(void*)& Cyc__genrep_134};static struct _tuple6*Cyc__genarr_546[
3]={& Cyc__gentuple_543,& Cyc__gentuple_544,& Cyc__gentuple_545};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_542={4,sizeof(struct _tuple38),{(void*)((struct _tuple6**)Cyc__genarr_546),(
void*)((struct _tuple6**)Cyc__genarr_546),(void*)((struct _tuple6**)Cyc__genarr_546
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_538;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_533;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_533={1,1,(void*)((
void*)& Cyc_struct_Absyn_Stmt_rep)};struct _tuple39{unsigned int f1;struct Cyc_Absyn_Stmt*
f2;};static struct _tuple6 Cyc__gentuple_539={offsetof(struct _tuple39,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_540={offsetof(struct _tuple39,f2),(
void*)& Cyc__genrep_533};static struct _tuple6*Cyc__genarr_541[2]={& Cyc__gentuple_539,&
Cyc__gentuple_540};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_538={4,
sizeof(struct _tuple39),{(void*)((struct _tuple6**)Cyc__genarr_541),(void*)((
struct _tuple6**)Cyc__genarr_541),(void*)((struct _tuple6**)Cyc__genarr_541 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_532;struct _tuple40{unsigned int
f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};static struct _tuple6 Cyc__gentuple_534={
offsetof(struct _tuple40,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_535={
offsetof(struct _tuple40,f2),(void*)& Cyc__genrep_13};static struct _tuple6 Cyc__gentuple_536={
offsetof(struct _tuple40,f3),(void*)& Cyc__genrep_533};static struct _tuple6*Cyc__genarr_537[
3]={& Cyc__gentuple_534,& Cyc__gentuple_535,& Cyc__gentuple_536};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_532={4,sizeof(struct _tuple40),{(void*)((struct _tuple6**)Cyc__genarr_537),(
void*)((struct _tuple6**)Cyc__genarr_537),(void*)((struct _tuple6**)Cyc__genarr_537
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_525;struct _tuple41{
unsigned int f1;struct Cyc_Absyn_Exp*f2;struct _tuple2 f3;struct _tuple2 f4;struct Cyc_Absyn_Stmt*
f5;};static struct _tuple6 Cyc__gentuple_526={offsetof(struct _tuple41,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_527={offsetof(struct _tuple41,f2),(
void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_528={offsetof(struct
_tuple41,f3),(void*)& Cyc__genrep_491};static struct _tuple6 Cyc__gentuple_529={
offsetof(struct _tuple41,f4),(void*)& Cyc__genrep_491};static struct _tuple6 Cyc__gentuple_530={
offsetof(struct _tuple41,f5),(void*)& Cyc__genrep_134};static struct _tuple6*Cyc__genarr_531[
5]={& Cyc__gentuple_526,& Cyc__gentuple_527,& Cyc__gentuple_528,& Cyc__gentuple_529,&
Cyc__gentuple_530};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_525={4,
sizeof(struct _tuple41),{(void*)((struct _tuple6**)Cyc__genarr_531),(void*)((
struct _tuple6**)Cyc__genarr_531),(void*)((struct _tuple6**)Cyc__genarr_531 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_520;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_223;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_224;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Switch_clause_rep;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_234;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Pat_rep;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_pat_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_449;
struct _tuple42{unsigned int f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Pat*f3;
};static struct _tuple6 Cyc__gentuple_450={offsetof(struct _tuple42,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_451={offsetof(struct _tuple42,f2),(void*)& Cyc__genrep_143};
static struct _tuple6 Cyc__gentuple_452={offsetof(struct _tuple42,f3),(void*)& Cyc__genrep_234};
static struct _tuple6*Cyc__genarr_453[3]={& Cyc__gentuple_450,& Cyc__gentuple_451,&
Cyc__gentuple_452};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_449={4,
sizeof(struct _tuple42),{(void*)((struct _tuple6**)Cyc__genarr_453),(void*)((
struct _tuple6**)Cyc__genarr_453),(void*)((struct _tuple6**)Cyc__genarr_453 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_444;struct _tuple43{unsigned int
f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};static struct _tuple6 Cyc__gentuple_445={
offsetof(struct _tuple43,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_446={
offsetof(struct _tuple43,f2),(void*)& Cyc__genrep_170};static struct _tuple6 Cyc__gentuple_447={
offsetof(struct _tuple43,f3),(void*)& Cyc__genrep_143};static struct _tuple6*Cyc__genarr_448[
3]={& Cyc__gentuple_445,& Cyc__gentuple_446,& Cyc__gentuple_447};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_444={4,sizeof(struct _tuple43),{(void*)((struct _tuple6**)Cyc__genarr_448),(
void*)((struct _tuple6**)Cyc__genarr_448),(void*)((struct _tuple6**)Cyc__genarr_448
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_439;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_239;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_pat_t46H2_rep;
static char _tmp292[5]="List";static struct _dyneither_ptr Cyc__genname_243=(struct
_dyneither_ptr){_tmp292,_tmp292,_tmp292 + 5};static char _tmp293[3]="hd";static
struct _tuple5 Cyc__gentuple_240={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp293,_tmp293,_tmp293 + 3},(void*)& Cyc__genrep_234};static char
_tmp294[3]="tl";static struct _tuple5 Cyc__gentuple_241={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp294,_tmp294,_tmp294 + 3},(void*)& Cyc__genrep_239};
static struct _tuple5*Cyc__genarr_242[2]={& Cyc__gentuple_240,& Cyc__gentuple_241};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_pat_t46H2_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_243,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_242),(void*)((struct _tuple5**)Cyc__genarr_242),(void*)((
struct _tuple5**)Cyc__genarr_242 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_239={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_pat_t46H2_rep)};struct _tuple44{
unsigned int f1;struct Cyc_List_List*f2;int f3;};static struct _tuple6 Cyc__gentuple_440={
offsetof(struct _tuple44,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_441={
offsetof(struct _tuple44,f2),(void*)& Cyc__genrep_239};static struct _tuple6 Cyc__gentuple_442={
offsetof(struct _tuple44,f3),(void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_443[
3]={& Cyc__gentuple_440,& Cyc__gentuple_441,& Cyc__gentuple_442};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_439={4,sizeof(struct _tuple44),{(void*)((struct _tuple6**)Cyc__genarr_443),(
void*)((struct _tuple6**)Cyc__genarr_443),(void*)((struct _tuple6**)Cyc__genarr_443
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_435;struct _tuple45{
unsigned int f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple6 Cyc__gentuple_436={
offsetof(struct _tuple45,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_437={
offsetof(struct _tuple45,f2),(void*)& Cyc__genrep_234};static struct _tuple6*Cyc__genarr_438[
2]={& Cyc__gentuple_436,& Cyc__gentuple_437};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_435={4,sizeof(struct _tuple45),{(void*)((struct _tuple6**)Cyc__genarr_438),(
void*)((struct _tuple6**)Cyc__genarr_438),(void*)((struct _tuple6**)Cyc__genarr_438
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_345;extern struct Cyc_Typerep_Struct_struct
Cyc_Absyn_aggr_info_t_rep;extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_AggrInfoU_rep;
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_417;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_aggr_kind_t_rep;static char _tmp299[8]="StructA";static struct _tuple7 Cyc__gentuple_402={
0,(struct _dyneither_ptr){_tmp299,_tmp299,_tmp299 + 8}};static char _tmp29A[7]="UnionA";
static struct _tuple7 Cyc__gentuple_403={1,(struct _dyneither_ptr){_tmp29A,_tmp29A,
_tmp29A + 7}};static struct _tuple7*Cyc__genarr_404[2]={& Cyc__gentuple_402,& Cyc__gentuple_403};
static struct _tuple5*Cyc__genarr_405[0]={};static char _tmp29C[9]="AggrKind";struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_aggr_kind_t_rep={5,(struct _dyneither_ptr){
_tmp29C,_tmp29C,_tmp29C + 9},{(void*)((struct _tuple7**)Cyc__genarr_404),(void*)((
struct _tuple7**)Cyc__genarr_404),(void*)((struct _tuple7**)Cyc__genarr_404 + 2)},{(
void*)((struct _tuple5**)Cyc__genarr_405),(void*)((struct _tuple5**)Cyc__genarr_405),(
void*)((struct _tuple5**)Cyc__genarr_405 + 0)}};struct _tuple46{unsigned int f1;void*
f2;struct _tuple0*f3;};static struct _tuple6 Cyc__gentuple_418={offsetof(struct
_tuple46,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_419={
offsetof(struct _tuple46,f2),(void*)& Cyc_Absyn_aggr_kind_t_rep};static struct
_tuple6 Cyc__gentuple_420={offsetof(struct _tuple46,f3),(void*)& Cyc__genrep_11};
static struct _tuple6*Cyc__genarr_421[3]={& Cyc__gentuple_418,& Cyc__gentuple_419,&
Cyc__gentuple_420};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_417={4,
sizeof(struct _tuple46),{(void*)((struct _tuple6**)Cyc__genarr_421),(void*)((
struct _tuple6**)Cyc__genarr_421),(void*)((struct _tuple6**)Cyc__genarr_421 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_370;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_371;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_372;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrdecl_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_373;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_AggrdeclImpl_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_374;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_375;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrfield_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_62;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_attribute_t46H2_rep;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_attribute_t_rep;extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_89;
extern struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Format_Type_rep;static char
_tmp29E[10]="Printf_ft";static struct _tuple7 Cyc__gentuple_90={0,(struct
_dyneither_ptr){_tmp29E,_tmp29E,_tmp29E + 10}};static char _tmp29F[9]="Scanf_ft";
static struct _tuple7 Cyc__gentuple_91={1,(struct _dyneither_ptr){_tmp29F,_tmp29F,
_tmp29F + 9}};static struct _tuple7*Cyc__genarr_92[2]={& Cyc__gentuple_90,& Cyc__gentuple_91};
static struct _tuple5*Cyc__genarr_93[0]={};static char _tmp2A1[12]="Format_Type";
struct Cyc_Typerep_TUnion_struct Cyc_tunion_Absyn_Format_Type_rep={5,(struct
_dyneither_ptr){_tmp2A1,_tmp2A1,_tmp2A1 + 12},{(void*)((struct _tuple7**)Cyc__genarr_92),(
void*)((struct _tuple7**)Cyc__genarr_92),(void*)((struct _tuple7**)Cyc__genarr_92 + 
2)},{(void*)((struct _tuple5**)Cyc__genarr_93),(void*)((struct _tuple5**)Cyc__genarr_93),(
void*)((struct _tuple5**)Cyc__genarr_93 + 0)}};struct _tuple47{unsigned int f1;void*
f2;int f3;int f4;};static struct _tuple6 Cyc__gentuple_94={offsetof(struct _tuple47,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_95={offsetof(struct
_tuple47,f2),(void*)& Cyc_tunion_Absyn_Format_Type_rep};static struct _tuple6 Cyc__gentuple_96={
offsetof(struct _tuple47,f3),(void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_97={
offsetof(struct _tuple47,f4),(void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_98[
4]={& Cyc__gentuple_94,& Cyc__gentuple_95,& Cyc__gentuple_96,& Cyc__gentuple_97};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_89={4,sizeof(struct _tuple47),{(
void*)((struct _tuple6**)Cyc__genarr_98),(void*)((struct _tuple6**)Cyc__genarr_98),(
void*)((struct _tuple6**)Cyc__genarr_98 + 4)}};static char _tmp2A3[12]="Stdcall_att";
static struct _tuple7 Cyc__gentuple_63={0,(struct _dyneither_ptr){_tmp2A3,_tmp2A3,
_tmp2A3 + 12}};static char _tmp2A4[10]="Cdecl_att";static struct _tuple7 Cyc__gentuple_64={
1,(struct _dyneither_ptr){_tmp2A4,_tmp2A4,_tmp2A4 + 10}};static char _tmp2A5[13]="Fastcall_att";
static struct _tuple7 Cyc__gentuple_65={2,(struct _dyneither_ptr){_tmp2A5,_tmp2A5,
_tmp2A5 + 13}};static char _tmp2A6[13]="Noreturn_att";static struct _tuple7 Cyc__gentuple_66={
3,(struct _dyneither_ptr){_tmp2A6,_tmp2A6,_tmp2A6 + 13}};static char _tmp2A7[10]="Const_att";
static struct _tuple7 Cyc__gentuple_67={4,(struct _dyneither_ptr){_tmp2A7,_tmp2A7,
_tmp2A7 + 10}};static char _tmp2A8[11]="Packed_att";static struct _tuple7 Cyc__gentuple_68={
5,(struct _dyneither_ptr){_tmp2A8,_tmp2A8,_tmp2A8 + 11}};static char _tmp2A9[13]="Nocommon_att";
static struct _tuple7 Cyc__gentuple_69={6,(struct _dyneither_ptr){_tmp2A9,_tmp2A9,
_tmp2A9 + 13}};static char _tmp2AA[11]="Shared_att";static struct _tuple7 Cyc__gentuple_70={
7,(struct _dyneither_ptr){_tmp2AA,_tmp2AA,_tmp2AA + 11}};static char _tmp2AB[11]="Unused_att";
static struct _tuple7 Cyc__gentuple_71={8,(struct _dyneither_ptr){_tmp2AB,_tmp2AB,
_tmp2AB + 11}};static char _tmp2AC[9]="Weak_att";static struct _tuple7 Cyc__gentuple_72={
9,(struct _dyneither_ptr){_tmp2AC,_tmp2AC,_tmp2AC + 9}};static char _tmp2AD[14]="Dllimport_att";
static struct _tuple7 Cyc__gentuple_73={10,(struct _dyneither_ptr){_tmp2AD,_tmp2AD,
_tmp2AD + 14}};static char _tmp2AE[14]="Dllexport_att";static struct _tuple7 Cyc__gentuple_74={
11,(struct _dyneither_ptr){_tmp2AE,_tmp2AE,_tmp2AE + 14}};static char _tmp2AF[27]="No_instrument_function_att";
static struct _tuple7 Cyc__gentuple_75={12,(struct _dyneither_ptr){_tmp2AF,_tmp2AF,
_tmp2AF + 27}};static char _tmp2B0[16]="Constructor_att";static struct _tuple7 Cyc__gentuple_76={
13,(struct _dyneither_ptr){_tmp2B0,_tmp2B0,_tmp2B0 + 16}};static char _tmp2B1[15]="Destructor_att";
static struct _tuple7 Cyc__gentuple_77={14,(struct _dyneither_ptr){_tmp2B1,_tmp2B1,
_tmp2B1 + 15}};static char _tmp2B2[26]="No_check_memory_usage_att";static struct
_tuple7 Cyc__gentuple_78={15,(struct _dyneither_ptr){_tmp2B2,_tmp2B2,_tmp2B2 + 26}};
static char _tmp2B3[9]="Pure_att";static struct _tuple7 Cyc__gentuple_79={16,(struct
_dyneither_ptr){_tmp2B3,_tmp2B3,_tmp2B3 + 9}};static struct _tuple7*Cyc__genarr_80[
17]={& Cyc__gentuple_63,& Cyc__gentuple_64,& Cyc__gentuple_65,& Cyc__gentuple_66,&
Cyc__gentuple_67,& Cyc__gentuple_68,& Cyc__gentuple_69,& Cyc__gentuple_70,& Cyc__gentuple_71,&
Cyc__gentuple_72,& Cyc__gentuple_73,& Cyc__gentuple_74,& Cyc__gentuple_75,& Cyc__gentuple_76,&
Cyc__gentuple_77,& Cyc__gentuple_78,& Cyc__gentuple_79};static char _tmp2B4[12]="Regparm_att";
static struct _tuple5 Cyc__gentuple_99={0,(struct _dyneither_ptr){_tmp2B4,_tmp2B4,
_tmp2B4 + 12},(void*)& Cyc__genrep_85};static char _tmp2B5[12]="Aligned_att";static
struct _tuple5 Cyc__gentuple_100={1,(struct _dyneither_ptr){_tmp2B5,_tmp2B5,_tmp2B5
+ 12},(void*)& Cyc__genrep_85};static char _tmp2B6[12]="Section_att";static struct
_tuple5 Cyc__gentuple_101={2,(struct _dyneither_ptr){_tmp2B6,_tmp2B6,_tmp2B6 + 12},(
void*)& Cyc__genrep_81};static char _tmp2B7[11]="Format_att";static struct _tuple5 Cyc__gentuple_102={
3,(struct _dyneither_ptr){_tmp2B7,_tmp2B7,_tmp2B7 + 11},(void*)& Cyc__genrep_89};
static char _tmp2B8[16]="Initializes_att";static struct _tuple5 Cyc__gentuple_103={4,(
struct _dyneither_ptr){_tmp2B8,_tmp2B8,_tmp2B8 + 16},(void*)& Cyc__genrep_85};
static char _tmp2B9[9]="Mode_att";static struct _tuple5 Cyc__gentuple_104={5,(struct
_dyneither_ptr){_tmp2B9,_tmp2B9,_tmp2B9 + 9},(void*)& Cyc__genrep_81};static struct
_tuple5*Cyc__genarr_105[6]={& Cyc__gentuple_99,& Cyc__gentuple_100,& Cyc__gentuple_101,&
Cyc__gentuple_102,& Cyc__gentuple_103,& Cyc__gentuple_104};static char _tmp2BB[10]="Attribute";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_attribute_t_rep={5,(struct
_dyneither_ptr){_tmp2BB,_tmp2BB,_tmp2BB + 10},{(void*)((struct _tuple7**)Cyc__genarr_80),(
void*)((struct _tuple7**)Cyc__genarr_80),(void*)((struct _tuple7**)Cyc__genarr_80 + 
17)},{(void*)((struct _tuple5**)Cyc__genarr_105),(void*)((struct _tuple5**)Cyc__genarr_105),(
void*)((struct _tuple5**)Cyc__genarr_105 + 6)}};static char _tmp2BC[5]="List";static
struct _dyneither_ptr Cyc__genname_109=(struct _dyneither_ptr){_tmp2BC,_tmp2BC,
_tmp2BC + 5};static char _tmp2BD[3]="hd";static struct _tuple5 Cyc__gentuple_106={
offsetof(struct Cyc_List_List,hd),(struct _dyneither_ptr){_tmp2BD,_tmp2BD,_tmp2BD + 
3},(void*)& Cyc_Absyn_attribute_t_rep};static char _tmp2BE[3]="tl";static struct
_tuple5 Cyc__gentuple_107={offsetof(struct Cyc_List_List,tl),(struct _dyneither_ptr){
_tmp2BE,_tmp2BE,_tmp2BE + 3},(void*)& Cyc__genrep_62};static struct _tuple5*Cyc__genarr_108[
2]={& Cyc__gentuple_106,& Cyc__gentuple_107};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_attribute_t46H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_109,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_108),(void*)((struct _tuple5**)Cyc__genarr_108),(void*)((
struct _tuple5**)Cyc__genarr_108 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_62={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_attribute_t46H2_rep)};static char
_tmp2C1[10]="Aggrfield";static struct _dyneither_ptr Cyc__genname_382=(struct
_dyneither_ptr){_tmp2C1,_tmp2C1,_tmp2C1 + 10};static char _tmp2C2[5]="name";static
struct _tuple5 Cyc__gentuple_376={offsetof(struct Cyc_Absyn_Aggrfield,name),(struct
_dyneither_ptr){_tmp2C2,_tmp2C2,_tmp2C2 + 5},(void*)& Cyc__genrep_13};static char
_tmp2C3[3]="tq";static struct _tuple5 Cyc__gentuple_377={offsetof(struct Cyc_Absyn_Aggrfield,tq),(
struct _dyneither_ptr){_tmp2C3,_tmp2C3,_tmp2C3 + 3},(void*)& Cyc_Absyn_tqual_t_rep};
static char _tmp2C4[5]="type";static struct _tuple5 Cyc__gentuple_378={offsetof(
struct Cyc_Absyn_Aggrfield,type),(struct _dyneither_ptr){_tmp2C4,_tmp2C4,_tmp2C4 + 
5},(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp2C5[6]="width";static
struct _tuple5 Cyc__gentuple_379={offsetof(struct Cyc_Absyn_Aggrfield,width),(
struct _dyneither_ptr){_tmp2C5,_tmp2C5,_tmp2C5 + 6},(void*)& Cyc__genrep_144};
static char _tmp2C6[11]="attributes";static struct _tuple5 Cyc__gentuple_380={
offsetof(struct Cyc_Absyn_Aggrfield,attributes),(struct _dyneither_ptr){_tmp2C6,
_tmp2C6,_tmp2C6 + 11},(void*)& Cyc__genrep_62};static struct _tuple5*Cyc__genarr_381[
5]={& Cyc__gentuple_376,& Cyc__gentuple_377,& Cyc__gentuple_378,& Cyc__gentuple_379,&
Cyc__gentuple_380};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrfield_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_382,sizeof(struct Cyc_Absyn_Aggrfield),{(
void*)((struct _tuple5**)Cyc__genarr_381),(void*)((struct _tuple5**)Cyc__genarr_381),(
void*)((struct _tuple5**)Cyc__genarr_381 + 5)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_375={1,1,(void*)((void*)& Cyc_struct_Absyn_Aggrfield_rep)};static char
_tmp2C9[5]="List";static struct _dyneither_ptr Cyc__genname_386=(struct
_dyneither_ptr){_tmp2C9,_tmp2C9,_tmp2C9 + 5};static char _tmp2CA[3]="hd";static
struct _tuple5 Cyc__gentuple_383={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp2CA,_tmp2CA,_tmp2CA + 3},(void*)& Cyc__genrep_375};static char
_tmp2CB[3]="tl";static struct _tuple5 Cyc__gentuple_384={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp2CB,_tmp2CB,_tmp2CB + 3},(void*)& Cyc__genrep_374};
static struct _tuple5*Cyc__genarr_385[2]={& Cyc__gentuple_383,& Cyc__gentuple_384};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_386,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_385),(void*)((struct _tuple5**)Cyc__genarr_385),(void*)((
struct _tuple5**)Cyc__genarr_385 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_374={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_aggrfield_t46H2_rep)};static char
_tmp2CE[13]="AggrdeclImpl";static struct _dyneither_ptr Cyc__genname_401=(struct
_dyneither_ptr){_tmp2CE,_tmp2CE,_tmp2CE + 13};static char _tmp2CF[11]="exist_vars";
static struct _tuple5 Cyc__gentuple_397={offsetof(struct Cyc_Absyn_AggrdeclImpl,exist_vars),(
struct _dyneither_ptr){_tmp2CF,_tmp2CF,_tmp2CF + 11},(void*)& Cyc__genrep_326};
static char _tmp2D0[7]="rgn_po";static struct _tuple5 Cyc__gentuple_398={offsetof(
struct Cyc_Absyn_AggrdeclImpl,rgn_po),(struct _dyneither_ptr){_tmp2D0,_tmp2D0,
_tmp2D0 + 7},(void*)& Cyc__genrep_387};static char _tmp2D1[7]="fields";static struct
_tuple5 Cyc__gentuple_399={offsetof(struct Cyc_Absyn_AggrdeclImpl,fields),(struct
_dyneither_ptr){_tmp2D1,_tmp2D1,_tmp2D1 + 7},(void*)& Cyc__genrep_374};static
struct _tuple5*Cyc__genarr_400[3]={& Cyc__gentuple_397,& Cyc__gentuple_398,& Cyc__gentuple_399};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_AggrdeclImpl_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_401,sizeof(struct Cyc_Absyn_AggrdeclImpl),{(void*)((
struct _tuple5**)Cyc__genarr_400),(void*)((struct _tuple5**)Cyc__genarr_400),(void*)((
struct _tuple5**)Cyc__genarr_400 + 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_373={
1,1,(void*)((void*)& Cyc_struct_Absyn_AggrdeclImpl_rep)};static char _tmp2D4[9]="Aggrdecl";
static struct _dyneither_ptr Cyc__genname_413=(struct _dyneither_ptr){_tmp2D4,
_tmp2D4,_tmp2D4 + 9};static char _tmp2D5[5]="kind";static struct _tuple5 Cyc__gentuple_406={
offsetof(struct Cyc_Absyn_Aggrdecl,kind),(struct _dyneither_ptr){_tmp2D5,_tmp2D5,
_tmp2D5 + 5},(void*)& Cyc_Absyn_aggr_kind_t_rep};static char _tmp2D6[3]="sc";static
struct _tuple5 Cyc__gentuple_407={offsetof(struct Cyc_Absyn_Aggrdecl,sc),(struct
_dyneither_ptr){_tmp2D6,_tmp2D6,_tmp2D6 + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp2D7[5]="name";static struct _tuple5 Cyc__gentuple_408={offsetof(
struct Cyc_Absyn_Aggrdecl,name),(struct _dyneither_ptr){_tmp2D7,_tmp2D7,_tmp2D7 + 5},(
void*)& Cyc__genrep_11};static char _tmp2D8[4]="tvs";static struct _tuple5 Cyc__gentuple_409={
offsetof(struct Cyc_Absyn_Aggrdecl,tvs),(struct _dyneither_ptr){_tmp2D8,_tmp2D8,
_tmp2D8 + 4},(void*)& Cyc__genrep_326};static char _tmp2D9[5]="impl";static struct
_tuple5 Cyc__gentuple_410={offsetof(struct Cyc_Absyn_Aggrdecl,impl),(struct
_dyneither_ptr){_tmp2D9,_tmp2D9,_tmp2D9 + 5},(void*)& Cyc__genrep_373};static char
_tmp2DA[11]="attributes";static struct _tuple5 Cyc__gentuple_411={offsetof(struct
Cyc_Absyn_Aggrdecl,attributes),(struct _dyneither_ptr){_tmp2DA,_tmp2DA,_tmp2DA + 
11},(void*)& Cyc__genrep_62};static struct _tuple5*Cyc__genarr_412[6]={& Cyc__gentuple_406,&
Cyc__gentuple_407,& Cyc__gentuple_408,& Cyc__gentuple_409,& Cyc__gentuple_410,& Cyc__gentuple_411};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Aggrdecl_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_413,sizeof(struct Cyc_Absyn_Aggrdecl),{(void*)((
struct _tuple5**)Cyc__genarr_412),(void*)((struct _tuple5**)Cyc__genarr_412),(void*)((
struct _tuple5**)Cyc__genarr_412 + 6)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_372={
1,1,(void*)((void*)& Cyc_struct_Absyn_Aggrdecl_rep)};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_371={1,1,(void*)((void*)& Cyc__genrep_372)};struct _tuple48{
unsigned int f1;struct Cyc_Absyn_Aggrdecl**f2;};static struct _tuple6 Cyc__gentuple_414={
offsetof(struct _tuple48,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_415={
offsetof(struct _tuple48,f2),(void*)& Cyc__genrep_371};static struct _tuple6*Cyc__genarr_416[
2]={& Cyc__gentuple_414,& Cyc__gentuple_415};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_370={4,sizeof(struct _tuple48),{(void*)((struct _tuple6**)Cyc__genarr_416),(
void*)((struct _tuple6**)Cyc__genarr_416),(void*)((struct _tuple6**)Cyc__genarr_416
+ 2)}};static struct _tuple7*Cyc__genarr_369[0]={};static char _tmp2DF[12]="UnknownAggr";
static struct _tuple5 Cyc__gentuple_422={0,(struct _dyneither_ptr){_tmp2DF,_tmp2DF,
_tmp2DF + 12},(void*)& Cyc__genrep_417};static char _tmp2E0[10]="KnownAggr";static
struct _tuple5 Cyc__gentuple_423={1,(struct _dyneither_ptr){_tmp2E0,_tmp2E0,_tmp2E0
+ 10},(void*)& Cyc__genrep_370};static struct _tuple5*Cyc__genarr_424[2]={& Cyc__gentuple_422,&
Cyc__gentuple_423};static char _tmp2E2[10]="AggrInfoU";struct Cyc_Typerep_TUnion_struct
Cyc_tunion_Absyn_AggrInfoU_rep={5,(struct _dyneither_ptr){_tmp2E2,_tmp2E2,_tmp2E2
+ 10},{(void*)((struct _tuple7**)Cyc__genarr_369),(void*)((struct _tuple7**)Cyc__genarr_369),(
void*)((struct _tuple7**)Cyc__genarr_369 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_424),(
void*)((struct _tuple5**)Cyc__genarr_424),(void*)((struct _tuple5**)Cyc__genarr_424
+ 2)}};static char _tmp2E3[9]="AggrInfo";static struct _dyneither_ptr Cyc__genname_428=(
struct _dyneither_ptr){_tmp2E3,_tmp2E3,_tmp2E3 + 9};static char _tmp2E4[10]="aggr_info";
static struct _tuple5 Cyc__gentuple_425={offsetof(struct Cyc_Absyn_AggrInfo,aggr_info),(
struct _dyneither_ptr){_tmp2E4,_tmp2E4,_tmp2E4 + 10},(void*)& Cyc_tunion_Absyn_AggrInfoU_rep};
static char _tmp2E5[6]="targs";static struct _tuple5 Cyc__gentuple_426={offsetof(
struct Cyc_Absyn_AggrInfo,targs),(struct _dyneither_ptr){_tmp2E5,_tmp2E5,_tmp2E5 + 
6},(void*)& Cyc__genrep_121};static struct _tuple5*Cyc__genarr_427[2]={& Cyc__gentuple_425,&
Cyc__gentuple_426};struct Cyc_Typerep_Struct_struct Cyc_Absyn_aggr_info_t_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_428,sizeof(struct Cyc_Absyn_AggrInfo),{(void*)((
struct _tuple5**)Cyc__genarr_427),(void*)((struct _tuple5**)Cyc__genarr_427),(void*)((
struct _tuple5**)Cyc__genarr_427 + 2)}};extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_346;
extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_347;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_348;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_349;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_designator_t46H2_rep;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_designator_t_rep;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_351;struct _tuple49{unsigned int f1;struct _dyneither_ptr*f2;};static
struct _tuple6 Cyc__gentuple_352={offsetof(struct _tuple49,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_353={offsetof(struct _tuple49,f2),(void*)& Cyc__genrep_13};
static struct _tuple6*Cyc__genarr_354[2]={& Cyc__gentuple_352,& Cyc__gentuple_353};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_351={4,sizeof(struct _tuple49),{(
void*)((struct _tuple6**)Cyc__genarr_354),(void*)((struct _tuple6**)Cyc__genarr_354),(
void*)((struct _tuple6**)Cyc__genarr_354 + 2)}};static struct _tuple7*Cyc__genarr_350[
0]={};static char _tmp2E8[13]="ArrayElement";static struct _tuple5 Cyc__gentuple_355={
0,(struct _dyneither_ptr){_tmp2E8,_tmp2E8,_tmp2E8 + 13},(void*)& Cyc__genrep_129};
static char _tmp2E9[10]="FieldName";static struct _tuple5 Cyc__gentuple_356={1,(
struct _dyneither_ptr){_tmp2E9,_tmp2E9,_tmp2E9 + 10},(void*)& Cyc__genrep_351};
static struct _tuple5*Cyc__genarr_357[2]={& Cyc__gentuple_355,& Cyc__gentuple_356};
static char _tmp2EB[11]="Designator";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_designator_t_rep={
5,(struct _dyneither_ptr){_tmp2EB,_tmp2EB,_tmp2EB + 11},{(void*)((struct _tuple7**)
Cyc__genarr_350),(void*)((struct _tuple7**)Cyc__genarr_350),(void*)((struct
_tuple7**)Cyc__genarr_350 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_357),(void*)((
struct _tuple5**)Cyc__genarr_357),(void*)((struct _tuple5**)Cyc__genarr_357 + 2)}};
static char _tmp2EC[5]="List";static struct _dyneither_ptr Cyc__genname_361=(struct
_dyneither_ptr){_tmp2EC,_tmp2EC,_tmp2EC + 5};static char _tmp2ED[3]="hd";static
struct _tuple5 Cyc__gentuple_358={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp2ED,_tmp2ED,_tmp2ED + 3},(void*)& Cyc_Absyn_designator_t_rep};
static char _tmp2EE[3]="tl";static struct _tuple5 Cyc__gentuple_359={offsetof(struct
Cyc_List_List,tl),(struct _dyneither_ptr){_tmp2EE,_tmp2EE,_tmp2EE + 3},(void*)& Cyc__genrep_349};
static struct _tuple5*Cyc__genarr_360[2]={& Cyc__gentuple_358,& Cyc__gentuple_359};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_designator_t46H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_361,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_360),(void*)((struct _tuple5**)Cyc__genarr_360),(void*)((
struct _tuple5**)Cyc__genarr_360 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_349={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_designator_t46H2_rep)};struct
_tuple50{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple6 Cyc__gentuple_362={
offsetof(struct _tuple50,f1),(void*)& Cyc__genrep_349};static struct _tuple6 Cyc__gentuple_363={
offsetof(struct _tuple50,f2),(void*)& Cyc__genrep_234};static struct _tuple6*Cyc__genarr_364[
2]={& Cyc__gentuple_362,& Cyc__gentuple_363};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_348={4,sizeof(struct _tuple50),{(void*)((struct _tuple6**)Cyc__genarr_364),(
void*)((struct _tuple6**)Cyc__genarr_364),(void*)((struct _tuple6**)Cyc__genarr_364
+ 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_347={1,1,(void*)((void*)&
Cyc__genrep_348)};static char _tmp2F3[5]="List";static struct _dyneither_ptr Cyc__genname_368=(
struct _dyneither_ptr){_tmp2F3,_tmp2F3,_tmp2F3 + 5};static char _tmp2F4[3]="hd";
static struct _tuple5 Cyc__gentuple_365={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp2F4,_tmp2F4,_tmp2F4 + 3},(void*)& Cyc__genrep_347};static char
_tmp2F5[3]="tl";static struct _tuple5 Cyc__gentuple_366={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp2F5,_tmp2F5,_tmp2F5 + 3},(void*)& Cyc__genrep_346};
static struct _tuple5*Cyc__genarr_367[2]={& Cyc__gentuple_365,& Cyc__gentuple_366};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_368,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_367),(void*)((struct _tuple5**)Cyc__genarr_367),(void*)((
struct _tuple5**)Cyc__genarr_367 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_346={
1,1,(void*)((void*)& Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_pat_t1_446H2_rep)};
struct _tuple51{unsigned int f1;struct Cyc_Absyn_AggrInfo f2;struct Cyc_List_List*f3;
struct Cyc_List_List*f4;int f5;};static struct _tuple6 Cyc__gentuple_429={offsetof(
struct _tuple51,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_430={
offsetof(struct _tuple51,f2),(void*)& Cyc_Absyn_aggr_info_t_rep};static struct
_tuple6 Cyc__gentuple_431={offsetof(struct _tuple51,f3),(void*)& Cyc__genrep_326};
static struct _tuple6 Cyc__gentuple_432={offsetof(struct _tuple51,f4),(void*)& Cyc__genrep_346};
static struct _tuple6 Cyc__gentuple_433={offsetof(struct _tuple51,f5),(void*)& Cyc__genrep_10};
static struct _tuple6*Cyc__genarr_434[5]={& Cyc__gentuple_429,& Cyc__gentuple_430,&
Cyc__gentuple_431,& Cyc__gentuple_432,& Cyc__gentuple_433};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_345={4,sizeof(struct _tuple51),{(void*)((struct _tuple6**)Cyc__genarr_434),(
void*)((struct _tuple6**)Cyc__genarr_434),(void*)((struct _tuple6**)Cyc__genarr_434
+ 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_298;struct _tuple52{
unsigned int f1;struct Cyc_Absyn_Tuniondecl*f2;struct Cyc_Absyn_Tunionfield*f3;
struct Cyc_List_List*f4;int f5;};static struct _tuple6 Cyc__gentuple_339={offsetof(
struct _tuple52,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_340={
offsetof(struct _tuple52,f2),(void*)((void*)& Cyc__genrep_316)};static struct
_tuple6 Cyc__gentuple_341={offsetof(struct _tuple52,f3),(void*)& Cyc__genrep_299};
static struct _tuple6 Cyc__gentuple_342={offsetof(struct _tuple52,f4),(void*)& Cyc__genrep_239};
static struct _tuple6 Cyc__gentuple_343={offsetof(struct _tuple52,f5),(void*)& Cyc__genrep_10};
static struct _tuple6*Cyc__genarr_344[5]={& Cyc__gentuple_339,& Cyc__gentuple_340,&
Cyc__gentuple_341,& Cyc__gentuple_342,& Cyc__gentuple_343};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_298={4,sizeof(struct _tuple52),{(void*)((struct _tuple6**)Cyc__genarr_344),(
void*)((struct _tuple6**)Cyc__genarr_344),(void*)((struct _tuple6**)Cyc__genarr_344
+ 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_284;struct _tuple53{
unsigned int f1;char f2;};static struct _tuple6 Cyc__gentuple_285={offsetof(struct
_tuple53,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_286={
offsetof(struct _tuple53,f2),(void*)((void*)& Cyc__genrep_15)};static struct _tuple6*
Cyc__genarr_287[2]={& Cyc__gentuple_285,& Cyc__gentuple_286};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_284={4,sizeof(struct _tuple53),{(void*)((struct _tuple6**)Cyc__genarr_287),(
void*)((struct _tuple6**)Cyc__genarr_287),(void*)((struct _tuple6**)Cyc__genarr_287
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_264;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_265;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumdecl_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_266;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_267;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_enumfield_t46H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_254;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Enumfield_rep;static char _tmp2FB[10]="Enumfield";static struct
_dyneither_ptr Cyc__genname_259=(struct _dyneither_ptr){_tmp2FB,_tmp2FB,_tmp2FB + 
10};static char _tmp2FC[5]="name";static struct _tuple5 Cyc__gentuple_255={offsetof(
struct Cyc_Absyn_Enumfield,name),(struct _dyneither_ptr){_tmp2FC,_tmp2FC,_tmp2FC + 
5},(void*)& Cyc__genrep_11};static char _tmp2FD[4]="tag";static struct _tuple5 Cyc__gentuple_256={
offsetof(struct Cyc_Absyn_Enumfield,tag),(struct _dyneither_ptr){_tmp2FD,_tmp2FD,
_tmp2FD + 4},(void*)& Cyc__genrep_144};static char _tmp2FE[4]="loc";static struct
_tuple5 Cyc__gentuple_257={offsetof(struct Cyc_Absyn_Enumfield,loc),(struct
_dyneither_ptr){_tmp2FE,_tmp2FE,_tmp2FE + 4},(void*)& Cyc__genrep_2};static struct
_tuple5*Cyc__genarr_258[3]={& Cyc__gentuple_255,& Cyc__gentuple_256,& Cyc__gentuple_257};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Enumfield_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_259,sizeof(struct Cyc_Absyn_Enumfield),{(void*)((
struct _tuple5**)Cyc__genarr_258),(void*)((struct _tuple5**)Cyc__genarr_258),(void*)((
struct _tuple5**)Cyc__genarr_258 + 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_254={
1,1,(void*)((void*)& Cyc_struct_Absyn_Enumfield_rep)};static char _tmp301[5]="List";
static struct _dyneither_ptr Cyc__genname_271=(struct _dyneither_ptr){_tmp301,
_tmp301,_tmp301 + 5};static char _tmp302[3]="hd";static struct _tuple5 Cyc__gentuple_268={
offsetof(struct Cyc_List_List,hd),(struct _dyneither_ptr){_tmp302,_tmp302,_tmp302 + 
3},(void*)& Cyc__genrep_254};static char _tmp303[3]="tl";static struct _tuple5 Cyc__gentuple_269={
offsetof(struct Cyc_List_List,tl),(struct _dyneither_ptr){_tmp303,_tmp303,_tmp303 + 
3},(void*)& Cyc__genrep_267};static struct _tuple5*Cyc__genarr_270[2]={& Cyc__gentuple_268,&
Cyc__gentuple_269};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_enumfield_t46H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_271,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_270),(void*)((struct _tuple5**)Cyc__genarr_270),(void*)((
struct _tuple5**)Cyc__genarr_270 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_267={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_enumfield_t46H2_rep)};static char
_tmp306[4]="Opt";static struct _dyneither_ptr Cyc__genname_274=(struct
_dyneither_ptr){_tmp306,_tmp306,_tmp306 + 4};static char _tmp307[2]="v";static
struct _tuple5 Cyc__gentuple_272={offsetof(struct Cyc_Core_Opt,v),(struct
_dyneither_ptr){_tmp307,_tmp307,_tmp307 + 2},(void*)& Cyc__genrep_267};static
struct _tuple5*Cyc__genarr_273[1]={& Cyc__gentuple_272};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_274,sizeof(struct Cyc_Core_Opt),{(void*)((struct
_tuple5**)Cyc__genarr_273),(void*)((struct _tuple5**)Cyc__genarr_273),(void*)((
struct _tuple5**)Cyc__genarr_273 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_266={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_enumfield_t46H22_rep)};
static char _tmp30A[9]="Enumdecl";static struct _dyneither_ptr Cyc__genname_279=(
struct _dyneither_ptr){_tmp30A,_tmp30A,_tmp30A + 9};static char _tmp30B[3]="sc";
static struct _tuple5 Cyc__gentuple_275={offsetof(struct Cyc_Absyn_Enumdecl,sc),(
struct _dyneither_ptr){_tmp30B,_tmp30B,_tmp30B + 3},(void*)& Cyc_Absyn_scope_t_rep};
static char _tmp30C[5]="name";static struct _tuple5 Cyc__gentuple_276={offsetof(
struct Cyc_Absyn_Enumdecl,name),(struct _dyneither_ptr){_tmp30C,_tmp30C,_tmp30C + 5},(
void*)& Cyc__genrep_11};static char _tmp30D[7]="fields";static struct _tuple5 Cyc__gentuple_277={
offsetof(struct Cyc_Absyn_Enumdecl,fields),(struct _dyneither_ptr){_tmp30D,_tmp30D,
_tmp30D + 7},(void*)& Cyc__genrep_266};static struct _tuple5*Cyc__genarr_278[3]={&
Cyc__gentuple_275,& Cyc__gentuple_276,& Cyc__gentuple_277};struct Cyc_Typerep_Struct_struct
Cyc_struct_Absyn_Enumdecl_rep={3,(struct _dyneither_ptr*)& Cyc__genname_279,
sizeof(struct Cyc_Absyn_Enumdecl),{(void*)((struct _tuple5**)Cyc__genarr_278),(
void*)((struct _tuple5**)Cyc__genarr_278),(void*)((struct _tuple5**)Cyc__genarr_278
+ 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_265={1,1,(void*)((void*)&
Cyc_struct_Absyn_Enumdecl_rep)};struct _tuple54{unsigned int f1;struct Cyc_Absyn_Enumdecl*
f2;struct Cyc_Absyn_Enumfield*f3;};static struct _tuple6 Cyc__gentuple_280={
offsetof(struct _tuple54,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_281={
offsetof(struct _tuple54,f2),(void*)& Cyc__genrep_265};static struct _tuple6 Cyc__gentuple_282={
offsetof(struct _tuple54,f3),(void*)& Cyc__genrep_254};static struct _tuple6*Cyc__genarr_283[
3]={& Cyc__gentuple_280,& Cyc__gentuple_281,& Cyc__gentuple_282};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_264={4,sizeof(struct _tuple54),{(void*)((struct _tuple6**)Cyc__genarr_283),(
void*)((struct _tuple6**)Cyc__genarr_283),(void*)((struct _tuple6**)Cyc__genarr_283
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_253;struct _tuple55{
unsigned int f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};static struct _tuple6 Cyc__gentuple_260={
offsetof(struct _tuple55,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_261={
offsetof(struct _tuple55,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_262={offsetof(struct _tuple55,f3),(void*)& Cyc__genrep_254};
static struct _tuple6*Cyc__genarr_263[3]={& Cyc__gentuple_260,& Cyc__gentuple_261,&
Cyc__gentuple_262};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_253={4,
sizeof(struct _tuple55),{(void*)((struct _tuple6**)Cyc__genarr_263),(void*)((
struct _tuple6**)Cyc__genarr_263),(void*)((struct _tuple6**)Cyc__genarr_263 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_249;struct _tuple56{unsigned int
f1;struct _tuple0*f2;};static struct _tuple6 Cyc__gentuple_250={offsetof(struct
_tuple56,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_251={
offsetof(struct _tuple56,f2),(void*)& Cyc__genrep_11};static struct _tuple6*Cyc__genarr_252[
2]={& Cyc__gentuple_250,& Cyc__gentuple_251};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_249={4,sizeof(struct _tuple56),{(void*)((struct _tuple6**)Cyc__genarr_252),(
void*)((struct _tuple6**)Cyc__genarr_252),(void*)((struct _tuple6**)Cyc__genarr_252
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_238;struct _tuple57{
unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;int f4;};static struct
_tuple6 Cyc__gentuple_244={offsetof(struct _tuple57,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_245={offsetof(struct _tuple57,f2),(void*)& Cyc__genrep_11};
static struct _tuple6 Cyc__gentuple_246={offsetof(struct _tuple57,f3),(void*)& Cyc__genrep_239};
static struct _tuple6 Cyc__gentuple_247={offsetof(struct _tuple57,f4),(void*)& Cyc__genrep_10};
static struct _tuple6*Cyc__genarr_248[4]={& Cyc__gentuple_244,& Cyc__gentuple_245,&
Cyc__gentuple_246,& Cyc__gentuple_247};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_238={
4,sizeof(struct _tuple57),{(void*)((struct _tuple6**)Cyc__genarr_248),(void*)((
struct _tuple6**)Cyc__genarr_248),(void*)((struct _tuple6**)Cyc__genarr_248 + 4)}};
static char _tmp314[7]="Wild_p";static struct _tuple7 Cyc__gentuple_235={0,(struct
_dyneither_ptr){_tmp314,_tmp314,_tmp314 + 7}};static char _tmp315[7]="Null_p";
static struct _tuple7 Cyc__gentuple_236={1,(struct _dyneither_ptr){_tmp315,_tmp315,
_tmp315 + 7}};static struct _tuple7*Cyc__genarr_237[2]={& Cyc__gentuple_235,& Cyc__gentuple_236};
static char _tmp316[6]="Var_p";static struct _tuple5 Cyc__gentuple_454={0,(struct
_dyneither_ptr){_tmp316,_tmp316,_tmp316 + 6},(void*)& Cyc__genrep_449};static char
_tmp317[12]="Reference_p";static struct _tuple5 Cyc__gentuple_455={1,(struct
_dyneither_ptr){_tmp317,_tmp317,_tmp317 + 12},(void*)& Cyc__genrep_449};static char
_tmp318[9]="TagInt_p";static struct _tuple5 Cyc__gentuple_456={2,(struct
_dyneither_ptr){_tmp318,_tmp318,_tmp318 + 9},(void*)& Cyc__genrep_444};static char
_tmp319[8]="Tuple_p";static struct _tuple5 Cyc__gentuple_457={3,(struct
_dyneither_ptr){_tmp319,_tmp319,_tmp319 + 8},(void*)& Cyc__genrep_439};static char
_tmp31A[10]="Pointer_p";static struct _tuple5 Cyc__gentuple_458={4,(struct
_dyneither_ptr){_tmp31A,_tmp31A,_tmp31A + 10},(void*)& Cyc__genrep_435};static char
_tmp31B[7]="Aggr_p";static struct _tuple5 Cyc__gentuple_459={5,(struct
_dyneither_ptr){_tmp31B,_tmp31B,_tmp31B + 7},(void*)& Cyc__genrep_345};static char
_tmp31C[9]="Tunion_p";static struct _tuple5 Cyc__gentuple_460={6,(struct
_dyneither_ptr){_tmp31C,_tmp31C,_tmp31C + 9},(void*)& Cyc__genrep_298};static char
_tmp31D[6]="Int_p";static struct _tuple5 Cyc__gentuple_461={7,(struct _dyneither_ptr){
_tmp31D,_tmp31D,_tmp31D + 6},(void*)& Cyc__genrep_288};static char _tmp31E[7]="Char_p";
static struct _tuple5 Cyc__gentuple_462={8,(struct _dyneither_ptr){_tmp31E,_tmp31E,
_tmp31E + 7},(void*)& Cyc__genrep_284};static char _tmp31F[8]="Float_p";static struct
_tuple5 Cyc__gentuple_463={9,(struct _dyneither_ptr){_tmp31F,_tmp31F,_tmp31F + 8},(
void*)& Cyc__genrep_81};static char _tmp320[7]="Enum_p";static struct _tuple5 Cyc__gentuple_464={
10,(struct _dyneither_ptr){_tmp320,_tmp320,_tmp320 + 7},(void*)& Cyc__genrep_264};
static char _tmp321[11]="AnonEnum_p";static struct _tuple5 Cyc__gentuple_465={11,(
struct _dyneither_ptr){_tmp321,_tmp321,_tmp321 + 11},(void*)& Cyc__genrep_253};
static char _tmp322[12]="UnknownId_p";static struct _tuple5 Cyc__gentuple_466={12,(
struct _dyneither_ptr){_tmp322,_tmp322,_tmp322 + 12},(void*)& Cyc__genrep_249};
static char _tmp323[14]="UnknownCall_p";static struct _tuple5 Cyc__gentuple_467={13,(
struct _dyneither_ptr){_tmp323,_tmp323,_tmp323 + 14},(void*)& Cyc__genrep_238};
static char _tmp324[6]="Exp_p";static struct _tuple5 Cyc__gentuple_468={14,(struct
_dyneither_ptr){_tmp324,_tmp324,_tmp324 + 6},(void*)& Cyc__genrep_129};static
struct _tuple5*Cyc__genarr_469[15]={& Cyc__gentuple_454,& Cyc__gentuple_455,& Cyc__gentuple_456,&
Cyc__gentuple_457,& Cyc__gentuple_458,& Cyc__gentuple_459,& Cyc__gentuple_460,& Cyc__gentuple_461,&
Cyc__gentuple_462,& Cyc__gentuple_463,& Cyc__gentuple_464,& Cyc__gentuple_465,& Cyc__gentuple_466,&
Cyc__gentuple_467,& Cyc__gentuple_468};static char _tmp326[8]="Raw_pat";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_pat_t_rep={5,(struct _dyneither_ptr){_tmp326,_tmp326,_tmp326 + 8},{(
void*)((struct _tuple7**)Cyc__genarr_237),(void*)((struct _tuple7**)Cyc__genarr_237),(
void*)((struct _tuple7**)Cyc__genarr_237 + 2)},{(void*)((struct _tuple5**)Cyc__genarr_469),(
void*)((struct _tuple5**)Cyc__genarr_469),(void*)((struct _tuple5**)Cyc__genarr_469
+ 15)}};static char _tmp327[4]="Pat";static struct _dyneither_ptr Cyc__genname_474=(
struct _dyneither_ptr){_tmp327,_tmp327,_tmp327 + 4};static char _tmp328[2]="r";
static struct _tuple5 Cyc__gentuple_470={offsetof(struct Cyc_Absyn_Pat,r),(struct
_dyneither_ptr){_tmp328,_tmp328,_tmp328 + 2},(void*)& Cyc_Absyn_raw_pat_t_rep};
static char _tmp329[5]="topt";static struct _tuple5 Cyc__gentuple_471={offsetof(
struct Cyc_Absyn_Pat,topt),(struct _dyneither_ptr){_tmp329,_tmp329,_tmp329 + 5},(
void*)& Cyc__genrep_110};static char _tmp32A[4]="loc";static struct _tuple5 Cyc__gentuple_472={
offsetof(struct Cyc_Absyn_Pat,loc),(struct _dyneither_ptr){_tmp32A,_tmp32A,_tmp32A
+ 4},(void*)& Cyc__genrep_2};static struct _tuple5*Cyc__genarr_473[3]={& Cyc__gentuple_470,&
Cyc__gentuple_471,& Cyc__gentuple_472};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Pat_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_474,sizeof(struct Cyc_Absyn_Pat),{(void*)((
struct _tuple5**)Cyc__genarr_473),(void*)((struct _tuple5**)Cyc__genarr_473),(void*)((
struct _tuple5**)Cyc__genarr_473 + 3)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_234={
1,1,(void*)((void*)& Cyc_struct_Absyn_Pat_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_225;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_226;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_vardecl_t46H2_rep;static char _tmp32D[5]="List";static
struct _dyneither_ptr Cyc__genname_230=(struct _dyneither_ptr){_tmp32D,_tmp32D,
_tmp32D + 5};static char _tmp32E[3]="hd";static struct _tuple5 Cyc__gentuple_227={
offsetof(struct Cyc_List_List,hd),(struct _dyneither_ptr){_tmp32E,_tmp32E,_tmp32E + 
3},(void*)& Cyc__genrep_143};static char _tmp32F[3]="tl";static struct _tuple5 Cyc__gentuple_228={
offsetof(struct Cyc_List_List,tl),(struct _dyneither_ptr){_tmp32F,_tmp32F,_tmp32F + 
3},(void*)& Cyc__genrep_226};static struct _tuple5*Cyc__genarr_229[2]={& Cyc__gentuple_227,&
Cyc__gentuple_228};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_vardecl_t46H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_230,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_229),(void*)((struct _tuple5**)Cyc__genarr_229),(void*)((
struct _tuple5**)Cyc__genarr_229 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_226={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_vardecl_t46H2_rep)};static char
_tmp332[4]="Opt";static struct _dyneither_ptr Cyc__genname_233=(struct
_dyneither_ptr){_tmp332,_tmp332,_tmp332 + 4};static char _tmp333[2]="v";static
struct _tuple5 Cyc__gentuple_231={offsetof(struct Cyc_Core_Opt,v),(struct
_dyneither_ptr){_tmp333,_tmp333,_tmp333 + 2},(void*)& Cyc__genrep_226};static
struct _tuple5*Cyc__genarr_232[1]={& Cyc__gentuple_231};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_233,sizeof(struct Cyc_Core_Opt),{(void*)((struct
_tuple5**)Cyc__genarr_232),(void*)((struct _tuple5**)Cyc__genarr_232),(void*)((
struct _tuple5**)Cyc__genarr_232 + 1)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_225={
1,1,(void*)((void*)& Cyc_struct_Core_Opt0List_list_t0Absyn_vardecl_t46H22_rep)};
static char _tmp336[14]="Switch_clause";static struct _dyneither_ptr Cyc__genname_481=(
struct _dyneither_ptr){_tmp336,_tmp336,_tmp336 + 14};static char _tmp337[8]="pattern";
static struct _tuple5 Cyc__gentuple_475={offsetof(struct Cyc_Absyn_Switch_clause,pattern),(
struct _dyneither_ptr){_tmp337,_tmp337,_tmp337 + 8},(void*)& Cyc__genrep_234};
static char _tmp338[9]="pat_vars";static struct _tuple5 Cyc__gentuple_476={offsetof(
struct Cyc_Absyn_Switch_clause,pat_vars),(struct _dyneither_ptr){_tmp338,_tmp338,
_tmp338 + 9},(void*)& Cyc__genrep_225};static char _tmp339[13]="where_clause";static
struct _tuple5 Cyc__gentuple_477={offsetof(struct Cyc_Absyn_Switch_clause,where_clause),(
struct _dyneither_ptr){_tmp339,_tmp339,_tmp339 + 13},(void*)& Cyc__genrep_144};
static char _tmp33A[5]="body";static struct _tuple5 Cyc__gentuple_478={offsetof(
struct Cyc_Absyn_Switch_clause,body),(struct _dyneither_ptr){_tmp33A,_tmp33A,
_tmp33A + 5},(void*)& Cyc__genrep_134};static char _tmp33B[4]="loc";static struct
_tuple5 Cyc__gentuple_479={offsetof(struct Cyc_Absyn_Switch_clause,loc),(struct
_dyneither_ptr){_tmp33B,_tmp33B,_tmp33B + 4},(void*)& Cyc__genrep_2};static struct
_tuple5*Cyc__genarr_480[5]={& Cyc__gentuple_475,& Cyc__gentuple_476,& Cyc__gentuple_477,&
Cyc__gentuple_478,& Cyc__gentuple_479};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Switch_clause_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_481,sizeof(struct Cyc_Absyn_Switch_clause),{(
void*)((struct _tuple5**)Cyc__genarr_480),(void*)((struct _tuple5**)Cyc__genarr_480),(
void*)((struct _tuple5**)Cyc__genarr_480 + 5)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_224={1,1,(void*)((void*)& Cyc_struct_Absyn_Switch_clause_rep)};static
char _tmp33E[5]="List";static struct _dyneither_ptr Cyc__genname_485=(struct
_dyneither_ptr){_tmp33E,_tmp33E,_tmp33E + 5};static char _tmp33F[3]="hd";static
struct _tuple5 Cyc__gentuple_482={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp33F,_tmp33F,_tmp33F + 3},(void*)((void*)& Cyc__genrep_224)};
static char _tmp340[3]="tl";static struct _tuple5 Cyc__gentuple_483={offsetof(struct
Cyc_List_List,tl),(struct _dyneither_ptr){_tmp340,_tmp340,_tmp340 + 3},(void*)& Cyc__genrep_223};
static struct _tuple5*Cyc__genarr_484[2]={& Cyc__gentuple_482,& Cyc__gentuple_483};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_485,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_484),(void*)((struct _tuple5**)Cyc__genarr_484),(void*)((
struct _tuple5**)Cyc__genarr_484 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_223={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_switch_clause_t46H2_rep)};struct
_tuple58{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_521={offsetof(struct _tuple58,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_522={offsetof(struct _tuple58,f2),(void*)& Cyc__genrep_130};
static struct _tuple6 Cyc__gentuple_523={offsetof(struct _tuple58,f3),(void*)& Cyc__genrep_223};
static struct _tuple6*Cyc__genarr_524[3]={& Cyc__gentuple_521,& Cyc__gentuple_522,&
Cyc__gentuple_523};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_520={4,
sizeof(struct _tuple58),{(void*)((struct _tuple6**)Cyc__genarr_524),(void*)((
struct _tuple6**)Cyc__genarr_524),(void*)((struct _tuple6**)Cyc__genarr_524 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_509;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_511;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_exp_t46H2_rep;
static char _tmp344[5]="List";static struct _dyneither_ptr Cyc__genname_515=(struct
_dyneither_ptr){_tmp344,_tmp344,_tmp344 + 5};static char _tmp345[3]="hd";static
struct _tuple5 Cyc__gentuple_512={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp345,_tmp345,_tmp345 + 3},(void*)& Cyc__genrep_130};static char
_tmp346[3]="tl";static struct _tuple5 Cyc__gentuple_513={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp346,_tmp346,_tmp346 + 3},(void*)& Cyc__genrep_511};
static struct _tuple5*Cyc__genarr_514[2]={& Cyc__gentuple_512,& Cyc__gentuple_513};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_exp_t46H2_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_515,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_514),(void*)((struct _tuple5**)Cyc__genarr_514),(void*)((
struct _tuple5**)Cyc__genarr_514 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_511={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_exp_t46H2_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_510;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_510={1,1,(void*)((
void*)& Cyc__genrep_224)};struct _tuple59{unsigned int f1;struct Cyc_List_List*f2;
struct Cyc_Absyn_Switch_clause**f3;};static struct _tuple6 Cyc__gentuple_516={
offsetof(struct _tuple59,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_517={
offsetof(struct _tuple59,f2),(void*)& Cyc__genrep_511};static struct _tuple6 Cyc__gentuple_518={
offsetof(struct _tuple59,f3),(void*)& Cyc__genrep_510};static struct _tuple6*Cyc__genarr_519[
3]={& Cyc__gentuple_516,& Cyc__gentuple_517,& Cyc__gentuple_518};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_509={4,sizeof(struct _tuple59),{(void*)((struct _tuple6**)Cyc__genarr_519),(
void*)((struct _tuple6**)Cyc__genarr_519),(void*)((struct _tuple6**)Cyc__genarr_519
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_504;struct _tuple60{
unsigned int f1;struct Cyc_Absyn_Decl*f2;struct Cyc_Absyn_Stmt*f3;};static struct
_tuple6 Cyc__gentuple_505={offsetof(struct _tuple60,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_506={offsetof(struct _tuple60,f2),(void*)& Cyc__genrep_1};
static struct _tuple6 Cyc__gentuple_507={offsetof(struct _tuple60,f3),(void*)& Cyc__genrep_134};
static struct _tuple6*Cyc__genarr_508[3]={& Cyc__gentuple_505,& Cyc__gentuple_506,&
Cyc__gentuple_507};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_504={4,
sizeof(struct _tuple60),{(void*)((struct _tuple6**)Cyc__genarr_508),(void*)((
struct _tuple6**)Cyc__genarr_508),(void*)((struct _tuple6**)Cyc__genarr_508 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_499;static struct _tuple6 Cyc__gentuple_500={
offsetof(struct _tuple40,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_501={
offsetof(struct _tuple40,f2),(void*)& Cyc__genrep_13};static struct _tuple6 Cyc__gentuple_502={
offsetof(struct _tuple40,f3),(void*)& Cyc__genrep_134};static struct _tuple6*Cyc__genarr_503[
3]={& Cyc__gentuple_500,& Cyc__gentuple_501,& Cyc__gentuple_502};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_499={4,sizeof(struct _tuple40),{(void*)((struct _tuple6**)Cyc__genarr_503),(
void*)((struct _tuple6**)Cyc__genarr_503),(void*)((struct _tuple6**)Cyc__genarr_503
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_490;struct _tuple61{
unsigned int f1;struct Cyc_Absyn_Stmt*f2;struct _tuple2 f3;};static struct _tuple6 Cyc__gentuple_495={
offsetof(struct _tuple61,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_496={
offsetof(struct _tuple61,f2),(void*)& Cyc__genrep_134};static struct _tuple6 Cyc__gentuple_497={
offsetof(struct _tuple61,f3),(void*)& Cyc__genrep_491};static struct _tuple6*Cyc__genarr_498[
3]={& Cyc__gentuple_495,& Cyc__gentuple_496,& Cyc__gentuple_497};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_490={4,sizeof(struct _tuple61),{(void*)((struct _tuple6**)Cyc__genarr_498),(
void*)((struct _tuple6**)Cyc__genarr_498),(void*)((struct _tuple6**)Cyc__genarr_498
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_222;struct _tuple62{
unsigned int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;};static struct
_tuple6 Cyc__gentuple_486={offsetof(struct _tuple62,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_487={offsetof(struct _tuple62,f2),(void*)& Cyc__genrep_134};
static struct _tuple6 Cyc__gentuple_488={offsetof(struct _tuple62,f3),(void*)& Cyc__genrep_223};
static struct _tuple6*Cyc__genarr_489[3]={& Cyc__gentuple_486,& Cyc__gentuple_487,&
Cyc__gentuple_488};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_222={4,
sizeof(struct _tuple62),{(void*)((struct _tuple6**)Cyc__genarr_489),(void*)((
struct _tuple6**)Cyc__genarr_489),(void*)((struct _tuple6**)Cyc__genarr_489 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_214;struct _tuple63{unsigned int
f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;int f4;struct Cyc_Absyn_Exp*
f5;struct Cyc_Absyn_Stmt*f6;};static struct _tuple6 Cyc__gentuple_215={offsetof(
struct _tuple63,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_216={
offsetof(struct _tuple63,f2),(void*)& Cyc__genrep_170};static struct _tuple6 Cyc__gentuple_217={
offsetof(struct _tuple63,f3),(void*)& Cyc__genrep_143};static struct _tuple6 Cyc__gentuple_218={
offsetof(struct _tuple63,f4),(void*)& Cyc__genrep_10};static struct _tuple6 Cyc__gentuple_219={
offsetof(struct _tuple63,f5),(void*)& Cyc__genrep_144};static struct _tuple6 Cyc__gentuple_220={
offsetof(struct _tuple63,f6),(void*)& Cyc__genrep_134};static struct _tuple6*Cyc__genarr_221[
6]={& Cyc__gentuple_215,& Cyc__gentuple_216,& Cyc__gentuple_217,& Cyc__gentuple_218,&
Cyc__gentuple_219,& Cyc__gentuple_220};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_214={
4,sizeof(struct _tuple63),{(void*)((struct _tuple6**)Cyc__genarr_221),(void*)((
struct _tuple6**)Cyc__genarr_221),(void*)((struct _tuple6**)Cyc__genarr_221 + 6)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_142;struct _tuple64{unsigned int
f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Tvar*f3;struct Cyc_Absyn_Vardecl*f4;
struct Cyc_Absyn_Stmt*f5;};static struct _tuple6 Cyc__gentuple_208={offsetof(struct
_tuple64,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_209={
offsetof(struct _tuple64,f2),(void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_210={
offsetof(struct _tuple64,f3),(void*)& Cyc__genrep_170};static struct _tuple6 Cyc__gentuple_211={
offsetof(struct _tuple64,f4),(void*)& Cyc__genrep_143};static struct _tuple6 Cyc__gentuple_212={
offsetof(struct _tuple64,f5),(void*)& Cyc__genrep_134};static struct _tuple6*Cyc__genarr_213[
5]={& Cyc__gentuple_208,& Cyc__gentuple_209,& Cyc__gentuple_210,& Cyc__gentuple_211,&
Cyc__gentuple_212};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_142={4,
sizeof(struct _tuple64),{(void*)((struct _tuple6**)Cyc__genarr_213),(void*)((
struct _tuple6**)Cyc__genarr_213),(void*)((struct _tuple6**)Cyc__genarr_213 + 5)}};
static char _tmp351[7]="Skip_s";static struct _tuple7 Cyc__gentuple_140={0,(struct
_dyneither_ptr){_tmp351,_tmp351,_tmp351 + 7}};static struct _tuple7*Cyc__genarr_141[
1]={& Cyc__gentuple_140};static char _tmp352[6]="Exp_s";static struct _tuple5 Cyc__gentuple_562={
0,(struct _dyneither_ptr){_tmp352,_tmp352,_tmp352 + 6},(void*)& Cyc__genrep_129};
static char _tmp353[6]="Seq_s";static struct _tuple5 Cyc__gentuple_563={1,(struct
_dyneither_ptr){_tmp353,_tmp353,_tmp353 + 6},(void*)& Cyc__genrep_557};static char
_tmp354[9]="Return_s";static struct _tuple5 Cyc__gentuple_564={2,(struct
_dyneither_ptr){_tmp354,_tmp354,_tmp354 + 9},(void*)& Cyc__genrep_553};static char
_tmp355[13]="IfThenElse_s";static struct _tuple5 Cyc__gentuple_565={3,(struct
_dyneither_ptr){_tmp355,_tmp355,_tmp355 + 13},(void*)& Cyc__genrep_547};static char
_tmp356[8]="While_s";static struct _tuple5 Cyc__gentuple_566={4,(struct
_dyneither_ptr){_tmp356,_tmp356,_tmp356 + 8},(void*)& Cyc__genrep_542};static char
_tmp357[8]="Break_s";static struct _tuple5 Cyc__gentuple_567={5,(struct
_dyneither_ptr){_tmp357,_tmp357,_tmp357 + 8},(void*)& Cyc__genrep_538};static char
_tmp358[11]="Continue_s";static struct _tuple5 Cyc__gentuple_568={6,(struct
_dyneither_ptr){_tmp358,_tmp358,_tmp358 + 11},(void*)& Cyc__genrep_538};static char
_tmp359[7]="Goto_s";static struct _tuple5 Cyc__gentuple_569={7,(struct
_dyneither_ptr){_tmp359,_tmp359,_tmp359 + 7},(void*)& Cyc__genrep_532};static char
_tmp35A[6]="For_s";static struct _tuple5 Cyc__gentuple_570={8,(struct _dyneither_ptr){
_tmp35A,_tmp35A,_tmp35A + 6},(void*)& Cyc__genrep_525};static char _tmp35B[9]="Switch_s";
static struct _tuple5 Cyc__gentuple_571={9,(struct _dyneither_ptr){_tmp35B,_tmp35B,
_tmp35B + 9},(void*)& Cyc__genrep_520};static char _tmp35C[11]="Fallthru_s";static
struct _tuple5 Cyc__gentuple_572={10,(struct _dyneither_ptr){_tmp35C,_tmp35C,
_tmp35C + 11},(void*)& Cyc__genrep_509};static char _tmp35D[7]="Decl_s";static struct
_tuple5 Cyc__gentuple_573={11,(struct _dyneither_ptr){_tmp35D,_tmp35D,_tmp35D + 7},(
void*)& Cyc__genrep_504};static char _tmp35E[8]="Label_s";static struct _tuple5 Cyc__gentuple_574={
12,(struct _dyneither_ptr){_tmp35E,_tmp35E,_tmp35E + 8},(void*)& Cyc__genrep_499};
static char _tmp35F[5]="Do_s";static struct _tuple5 Cyc__gentuple_575={13,(struct
_dyneither_ptr){_tmp35F,_tmp35F,_tmp35F + 5},(void*)& Cyc__genrep_490};static char
_tmp360[11]="TryCatch_s";static struct _tuple5 Cyc__gentuple_576={14,(struct
_dyneither_ptr){_tmp360,_tmp360,_tmp360 + 11},(void*)& Cyc__genrep_222};static char
_tmp361[9]="Region_s";static struct _tuple5 Cyc__gentuple_577={15,(struct
_dyneither_ptr){_tmp361,_tmp361,_tmp361 + 9},(void*)& Cyc__genrep_214};static char
_tmp362[14]="ResetRegion_s";static struct _tuple5 Cyc__gentuple_578={16,(struct
_dyneither_ptr){_tmp362,_tmp362,_tmp362 + 14},(void*)& Cyc__genrep_129};static char
_tmp363[8]="Alias_s";static struct _tuple5 Cyc__gentuple_579={17,(struct
_dyneither_ptr){_tmp363,_tmp363,_tmp363 + 8},(void*)& Cyc__genrep_142};static
struct _tuple5*Cyc__genarr_580[18]={& Cyc__gentuple_562,& Cyc__gentuple_563,& Cyc__gentuple_564,&
Cyc__gentuple_565,& Cyc__gentuple_566,& Cyc__gentuple_567,& Cyc__gentuple_568,& Cyc__gentuple_569,&
Cyc__gentuple_570,& Cyc__gentuple_571,& Cyc__gentuple_572,& Cyc__gentuple_573,& Cyc__gentuple_574,&
Cyc__gentuple_575,& Cyc__gentuple_576,& Cyc__gentuple_577,& Cyc__gentuple_578,& Cyc__gentuple_579};
static char _tmp365[9]="Raw_stmt";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_raw_stmt_t_rep={
5,(struct _dyneither_ptr){_tmp365,_tmp365,_tmp365 + 9},{(void*)((struct _tuple7**)
Cyc__genarr_141),(void*)((struct _tuple7**)Cyc__genarr_141),(void*)((struct
_tuple7**)Cyc__genarr_141 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_580),(void*)((
struct _tuple5**)Cyc__genarr_580),(void*)((struct _tuple5**)Cyc__genarr_580 + 18)}};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_135;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List0Absyn_stmt_t46H2_rep;static char _tmp366[5]="List";static
struct _dyneither_ptr Cyc__genname_139=(struct _dyneither_ptr){_tmp366,_tmp366,
_tmp366 + 5};static char _tmp367[3]="hd";static struct _tuple5 Cyc__gentuple_136={
offsetof(struct Cyc_List_List,hd),(struct _dyneither_ptr){_tmp367,_tmp367,_tmp367 + 
3},(void*)& Cyc__genrep_134};static char _tmp368[3]="tl";static struct _tuple5 Cyc__gentuple_137={
offsetof(struct Cyc_List_List,tl),(struct _dyneither_ptr){_tmp368,_tmp368,_tmp368 + 
3},(void*)& Cyc__genrep_135};static struct _tuple5*Cyc__genarr_138[2]={& Cyc__gentuple_136,&
Cyc__gentuple_137};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_stmt_t46H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_139,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_138),(void*)((struct _tuple5**)Cyc__genarr_138),(void*)((
struct _tuple5**)Cyc__genarr_138 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_135={
1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_stmt_t46H2_rep)};extern struct Cyc_Typerep_XTUnion_struct
Cyc_Absyn_absyn_annot_t_rep;static struct _tuple8*Cyc__genarr_131[0]={};static char
_tmp36C[11]="AbsynAnnot";struct Cyc_Typerep_XTUnion_struct Cyc_Absyn_absyn_annot_t_rep={
7,(struct _dyneither_ptr){_tmp36C,_tmp36C,_tmp36C + 11},{(void*)((struct _tuple8**)
Cyc__genarr_131),(void*)((struct _tuple8**)Cyc__genarr_131),(void*)((struct
_tuple8**)Cyc__genarr_131 + 0)}};static char _tmp36D[5]="Stmt";static struct
_dyneither_ptr Cyc__genname_587=(struct _dyneither_ptr){_tmp36D,_tmp36D,_tmp36D + 5};
static char _tmp36E[2]="r";static struct _tuple5 Cyc__gentuple_581={offsetof(struct
Cyc_Absyn_Stmt,r),(struct _dyneither_ptr){_tmp36E,_tmp36E,_tmp36E + 2},(void*)& Cyc_Absyn_raw_stmt_t_rep};
static char _tmp36F[4]="loc";static struct _tuple5 Cyc__gentuple_582={offsetof(struct
Cyc_Absyn_Stmt,loc),(struct _dyneither_ptr){_tmp36F,_tmp36F,_tmp36F + 4},(void*)&
Cyc__genrep_2};static char _tmp370[16]="non_local_preds";static struct _tuple5 Cyc__gentuple_583={
offsetof(struct Cyc_Absyn_Stmt,non_local_preds),(struct _dyneither_ptr){_tmp370,
_tmp370,_tmp370 + 16},(void*)& Cyc__genrep_135};static char _tmp371[10]="try_depth";
static struct _tuple5 Cyc__gentuple_584={offsetof(struct Cyc_Absyn_Stmt,try_depth),(
struct _dyneither_ptr){_tmp371,_tmp371,_tmp371 + 10},(void*)& Cyc__genrep_10};
static char _tmp372[6]="annot";static struct _tuple5 Cyc__gentuple_585={offsetof(
struct Cyc_Absyn_Stmt,annot),(struct _dyneither_ptr){_tmp372,_tmp372,_tmp372 + 6},(
void*)& Cyc_Absyn_absyn_annot_t_rep};static struct _tuple5*Cyc__genarr_586[5]={& Cyc__gentuple_581,&
Cyc__gentuple_582,& Cyc__gentuple_583,& Cyc__gentuple_584,& Cyc__gentuple_585};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Stmt_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_587,sizeof(struct Cyc_Absyn_Stmt),{(void*)((struct
_tuple5**)Cyc__genarr_586),(void*)((struct _tuple5**)Cyc__genarr_586),(void*)((
struct _tuple5**)Cyc__genarr_586 + 5)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_134={
1,1,(void*)((void*)& Cyc_struct_Absyn_Stmt_rep)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_823;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_823={1,1,(void*)((
void*)& Cyc_struct_Absyn_Vardecl_rep)};static char _tmp376[7]="Fndecl";static struct
_dyneither_ptr Cyc__genname_852=(struct _dyneither_ptr){_tmp376,_tmp376,_tmp376 + 7};
static char _tmp377[3]="sc";static struct _tuple5 Cyc__gentuple_836={offsetof(struct
Cyc_Absyn_Fndecl,sc),(struct _dyneither_ptr){_tmp377,_tmp377,_tmp377 + 3},(void*)&
Cyc_Absyn_scope_t_rep};static char _tmp378[10]="is_inline";static struct _tuple5 Cyc__gentuple_837={
offsetof(struct Cyc_Absyn_Fndecl,is_inline),(struct _dyneither_ptr){_tmp378,
_tmp378,_tmp378 + 10},(void*)& Cyc__genrep_10};static char _tmp379[5]="name";static
struct _tuple5 Cyc__gentuple_838={offsetof(struct Cyc_Absyn_Fndecl,name),(struct
_dyneither_ptr){_tmp379,_tmp379,_tmp379 + 5},(void*)& Cyc__genrep_11};static char
_tmp37A[4]="tvs";static struct _tuple5 Cyc__gentuple_839={offsetof(struct Cyc_Absyn_Fndecl,tvs),(
struct _dyneither_ptr){_tmp37A,_tmp37A,_tmp37A + 4},(void*)& Cyc__genrep_326};
static char _tmp37B[7]="effect";static struct _tuple5 Cyc__gentuple_840={offsetof(
struct Cyc_Absyn_Fndecl,effect),(struct _dyneither_ptr){_tmp37B,_tmp37B,_tmp37B + 7},(
void*)& Cyc__genrep_110};static char _tmp37C[9]="ret_type";static struct _tuple5 Cyc__gentuple_841={
offsetof(struct Cyc_Absyn_Fndecl,ret_type),(struct _dyneither_ptr){_tmp37C,_tmp37C,
_tmp37C + 9},(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp37D[5]="args";
static struct _tuple5 Cyc__gentuple_842={offsetof(struct Cyc_Absyn_Fndecl,args),(
struct _dyneither_ptr){_tmp37D,_tmp37D,_tmp37D + 5},(void*)& Cyc__genrep_825};
static char _tmp37E[10]="c_varargs";static struct _tuple5 Cyc__gentuple_843={
offsetof(struct Cyc_Absyn_Fndecl,c_varargs),(struct _dyneither_ptr){_tmp37E,
_tmp37E,_tmp37E + 10},(void*)& Cyc__genrep_10};static char _tmp37F[12]="cyc_varargs";
static struct _tuple5 Cyc__gentuple_844={offsetof(struct Cyc_Absyn_Fndecl,cyc_varargs),(
struct _dyneither_ptr){_tmp37F,_tmp37F,_tmp37F + 12},(void*)& Cyc__genrep_824};
static char _tmp380[7]="rgn_po";static struct _tuple5 Cyc__gentuple_845={offsetof(
struct Cyc_Absyn_Fndecl,rgn_po),(struct _dyneither_ptr){_tmp380,_tmp380,_tmp380 + 7},(
void*)& Cyc__genrep_387};static char _tmp381[5]="body";static struct _tuple5 Cyc__gentuple_846={
offsetof(struct Cyc_Absyn_Fndecl,body),(struct _dyneither_ptr){_tmp381,_tmp381,
_tmp381 + 5},(void*)& Cyc__genrep_134};static char _tmp382[11]="cached_typ";static
struct _tuple5 Cyc__gentuple_847={offsetof(struct Cyc_Absyn_Fndecl,cached_typ),(
struct _dyneither_ptr){_tmp382,_tmp382,_tmp382 + 11},(void*)& Cyc__genrep_110};
static char _tmp383[15]="param_vardecls";static struct _tuple5 Cyc__gentuple_848={
offsetof(struct Cyc_Absyn_Fndecl,param_vardecls),(struct _dyneither_ptr){_tmp383,
_tmp383,_tmp383 + 15},(void*)& Cyc__genrep_225};static char _tmp384[11]="fn_vardecl";
static struct _tuple5 Cyc__gentuple_849={offsetof(struct Cyc_Absyn_Fndecl,fn_vardecl),(
struct _dyneither_ptr){_tmp384,_tmp384,_tmp384 + 11},(void*)& Cyc__genrep_823};
static char _tmp385[11]="attributes";static struct _tuple5 Cyc__gentuple_850={
offsetof(struct Cyc_Absyn_Fndecl,attributes),(struct _dyneither_ptr){_tmp385,
_tmp385,_tmp385 + 11},(void*)& Cyc__genrep_62};static struct _tuple5*Cyc__genarr_851[
15]={& Cyc__gentuple_836,& Cyc__gentuple_837,& Cyc__gentuple_838,& Cyc__gentuple_839,&
Cyc__gentuple_840,& Cyc__gentuple_841,& Cyc__gentuple_842,& Cyc__gentuple_843,& Cyc__gentuple_844,&
Cyc__gentuple_845,& Cyc__gentuple_846,& Cyc__gentuple_847,& Cyc__gentuple_848,& Cyc__gentuple_849,&
Cyc__gentuple_850};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Fndecl_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_852,sizeof(struct Cyc_Absyn_Fndecl),{(void*)((
struct _tuple5**)Cyc__genarr_851),(void*)((struct _tuple5**)Cyc__genarr_851),(void*)((
struct _tuple5**)Cyc__genarr_851 + 15)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_822={
1,1,(void*)((void*)& Cyc_struct_Absyn_Fndecl_rep)};struct _tuple65{unsigned int f1;
struct Cyc_Absyn_Fndecl*f2;};static struct _tuple6 Cyc__gentuple_853={offsetof(
struct _tuple65,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_854={
offsetof(struct _tuple65,f2),(void*)& Cyc__genrep_822};static struct _tuple6*Cyc__genarr_855[
2]={& Cyc__gentuple_853,& Cyc__gentuple_854};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_821={4,sizeof(struct _tuple65),{(void*)((struct _tuple6**)Cyc__genarr_855),(
void*)((struct _tuple6**)Cyc__genarr_855),(void*)((struct _tuple6**)Cyc__genarr_855
+ 2)}};static char _tmp389[13]="Unresolved_b";static struct _tuple7 Cyc__gentuple_815={
0,(struct _dyneither_ptr){_tmp389,_tmp389,_tmp389 + 13}};static struct _tuple7*Cyc__genarr_816[
1]={& Cyc__gentuple_815};static char _tmp38A[9]="Global_b";static struct _tuple5 Cyc__gentuple_856={
0,(struct _dyneither_ptr){_tmp38A,_tmp38A,_tmp38A + 9},(void*)& Cyc__genrep_817};
static char _tmp38B[10]="Funname_b";static struct _tuple5 Cyc__gentuple_857={1,(
struct _dyneither_ptr){_tmp38B,_tmp38B,_tmp38B + 10},(void*)& Cyc__genrep_821};
static char _tmp38C[8]="Param_b";static struct _tuple5 Cyc__gentuple_858={2,(struct
_dyneither_ptr){_tmp38C,_tmp38C,_tmp38C + 8},(void*)& Cyc__genrep_817};static char
_tmp38D[8]="Local_b";static struct _tuple5 Cyc__gentuple_859={3,(struct
_dyneither_ptr){_tmp38D,_tmp38D,_tmp38D + 8},(void*)& Cyc__genrep_817};static char
_tmp38E[6]="Pat_b";static struct _tuple5 Cyc__gentuple_860={4,(struct _dyneither_ptr){
_tmp38E,_tmp38E,_tmp38E + 6},(void*)& Cyc__genrep_817};static struct _tuple5*Cyc__genarr_861[
5]={& Cyc__gentuple_856,& Cyc__gentuple_857,& Cyc__gentuple_858,& Cyc__gentuple_859,&
Cyc__gentuple_860};static char _tmp390[8]="Binding";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_binding_t_rep={5,(struct _dyneither_ptr){_tmp390,_tmp390,_tmp390 + 8},{(
void*)((struct _tuple7**)Cyc__genarr_816),(void*)((struct _tuple7**)Cyc__genarr_816),(
void*)((struct _tuple7**)Cyc__genarr_816 + 1)},{(void*)((struct _tuple5**)Cyc__genarr_861),(
void*)((struct _tuple5**)Cyc__genarr_861),(void*)((struct _tuple5**)Cyc__genarr_861
+ 5)}};struct _tuple66{unsigned int f1;struct _tuple0*f2;void*f3;};static struct
_tuple6 Cyc__gentuple_862={offsetof(struct _tuple66,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_863={offsetof(struct _tuple66,f2),(void*)& Cyc__genrep_11};
static struct _tuple6 Cyc__gentuple_864={offsetof(struct _tuple66,f3),(void*)& Cyc_Absyn_binding_t_rep};
static struct _tuple6*Cyc__genarr_865[3]={& Cyc__gentuple_862,& Cyc__gentuple_863,&
Cyc__gentuple_864};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_814={4,
sizeof(struct _tuple66),{(void*)((struct _tuple6**)Cyc__genarr_865),(void*)((
struct _tuple6**)Cyc__genarr_865),(void*)((struct _tuple6**)Cyc__genarr_865 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_809;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_primop_t_rep;static char _tmp392[5]="Plus";static struct _tuple7 Cyc__gentuple_779={
0,(struct _dyneither_ptr){_tmp392,_tmp392,_tmp392 + 5}};static char _tmp393[6]="Times";
static struct _tuple7 Cyc__gentuple_780={1,(struct _dyneither_ptr){_tmp393,_tmp393,
_tmp393 + 6}};static char _tmp394[6]="Minus";static struct _tuple7 Cyc__gentuple_781={
2,(struct _dyneither_ptr){_tmp394,_tmp394,_tmp394 + 6}};static char _tmp395[4]="Div";
static struct _tuple7 Cyc__gentuple_782={3,(struct _dyneither_ptr){_tmp395,_tmp395,
_tmp395 + 4}};static char _tmp396[4]="Mod";static struct _tuple7 Cyc__gentuple_783={4,(
struct _dyneither_ptr){_tmp396,_tmp396,_tmp396 + 4}};static char _tmp397[3]="Eq";
static struct _tuple7 Cyc__gentuple_784={5,(struct _dyneither_ptr){_tmp397,_tmp397,
_tmp397 + 3}};static char _tmp398[4]="Neq";static struct _tuple7 Cyc__gentuple_785={6,(
struct _dyneither_ptr){_tmp398,_tmp398,_tmp398 + 4}};static char _tmp399[3]="Gt";
static struct _tuple7 Cyc__gentuple_786={7,(struct _dyneither_ptr){_tmp399,_tmp399,
_tmp399 + 3}};static char _tmp39A[3]="Lt";static struct _tuple7 Cyc__gentuple_787={8,(
struct _dyneither_ptr){_tmp39A,_tmp39A,_tmp39A + 3}};static char _tmp39B[4]="Gte";
static struct _tuple7 Cyc__gentuple_788={9,(struct _dyneither_ptr){_tmp39B,_tmp39B,
_tmp39B + 4}};static char _tmp39C[4]="Lte";static struct _tuple7 Cyc__gentuple_789={10,(
struct _dyneither_ptr){_tmp39C,_tmp39C,_tmp39C + 4}};static char _tmp39D[4]="Not";
static struct _tuple7 Cyc__gentuple_790={11,(struct _dyneither_ptr){_tmp39D,_tmp39D,
_tmp39D + 4}};static char _tmp39E[7]="Bitnot";static struct _tuple7 Cyc__gentuple_791={
12,(struct _dyneither_ptr){_tmp39E,_tmp39E,_tmp39E + 7}};static char _tmp39F[7]="Bitand";
static struct _tuple7 Cyc__gentuple_792={13,(struct _dyneither_ptr){_tmp39F,_tmp39F,
_tmp39F + 7}};static char _tmp3A0[6]="Bitor";static struct _tuple7 Cyc__gentuple_793={
14,(struct _dyneither_ptr){_tmp3A0,_tmp3A0,_tmp3A0 + 6}};static char _tmp3A1[7]="Bitxor";
static struct _tuple7 Cyc__gentuple_794={15,(struct _dyneither_ptr){_tmp3A1,_tmp3A1,
_tmp3A1 + 7}};static char _tmp3A2[10]="Bitlshift";static struct _tuple7 Cyc__gentuple_795={
16,(struct _dyneither_ptr){_tmp3A2,_tmp3A2,_tmp3A2 + 10}};static char _tmp3A3[11]="Bitlrshift";
static struct _tuple7 Cyc__gentuple_796={17,(struct _dyneither_ptr){_tmp3A3,_tmp3A3,
_tmp3A3 + 11}};static char _tmp3A4[11]="Bitarshift";static struct _tuple7 Cyc__gentuple_797={
18,(struct _dyneither_ptr){_tmp3A4,_tmp3A4,_tmp3A4 + 11}};static char _tmp3A5[8]="Numelts";
static struct _tuple7 Cyc__gentuple_798={19,(struct _dyneither_ptr){_tmp3A5,_tmp3A5,
_tmp3A5 + 8}};static struct _tuple7*Cyc__genarr_799[20]={& Cyc__gentuple_779,& Cyc__gentuple_780,&
Cyc__gentuple_781,& Cyc__gentuple_782,& Cyc__gentuple_783,& Cyc__gentuple_784,& Cyc__gentuple_785,&
Cyc__gentuple_786,& Cyc__gentuple_787,& Cyc__gentuple_788,& Cyc__gentuple_789,& Cyc__gentuple_790,&
Cyc__gentuple_791,& Cyc__gentuple_792,& Cyc__gentuple_793,& Cyc__gentuple_794,& Cyc__gentuple_795,&
Cyc__gentuple_796,& Cyc__gentuple_797,& Cyc__gentuple_798};static struct _tuple5*Cyc__genarr_800[
0]={};static char _tmp3A7[7]="Primop";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_primop_t_rep={
5,(struct _dyneither_ptr){_tmp3A7,_tmp3A7,_tmp3A7 + 7},{(void*)((struct _tuple7**)
Cyc__genarr_799),(void*)((struct _tuple7**)Cyc__genarr_799),(void*)((struct
_tuple7**)Cyc__genarr_799 + 20)},{(void*)((struct _tuple5**)Cyc__genarr_800),(void*)((
struct _tuple5**)Cyc__genarr_800),(void*)((struct _tuple5**)Cyc__genarr_800 + 0)}};
struct _tuple67{unsigned int f1;void*f2;struct Cyc_List_List*f3;};static struct
_tuple6 Cyc__gentuple_810={offsetof(struct _tuple67,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_811={offsetof(struct _tuple67,f2),(void*)& Cyc_Absyn_primop_t_rep};
static struct _tuple6 Cyc__gentuple_812={offsetof(struct _tuple67,f3),(void*)& Cyc__genrep_511};
static struct _tuple6*Cyc__genarr_813[3]={& Cyc__gentuple_810,& Cyc__gentuple_811,&
Cyc__gentuple_812};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_809={4,
sizeof(struct _tuple67),{(void*)((struct _tuple6**)Cyc__genarr_813),(void*)((
struct _tuple6**)Cyc__genarr_813),(void*)((struct _tuple6**)Cyc__genarr_813 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_777;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_778;extern struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_primop_t2_rep;
static char _tmp3A9[4]="Opt";static struct _dyneither_ptr Cyc__genname_803=(struct
_dyneither_ptr){_tmp3A9,_tmp3A9,_tmp3A9 + 4};static char _tmp3AA[2]="v";static
struct _tuple5 Cyc__gentuple_801={offsetof(struct Cyc_Core_Opt,v),(struct
_dyneither_ptr){_tmp3AA,_tmp3AA,_tmp3AA + 2},(void*)& Cyc_Absyn_primop_t_rep};
static struct _tuple5*Cyc__genarr_802[1]={& Cyc__gentuple_801};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_primop_t2_rep={3,(struct _dyneither_ptr*)& Cyc__genname_803,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_802),(void*)((
struct _tuple5**)Cyc__genarr_802),(void*)((struct _tuple5**)Cyc__genarr_802 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_778={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_primop_t2_rep)};
struct _tuple68{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Core_Opt*f3;
struct Cyc_Absyn_Exp*f4;};static struct _tuple6 Cyc__gentuple_804={offsetof(struct
_tuple68,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_805={
offsetof(struct _tuple68,f2),(void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_806={
offsetof(struct _tuple68,f3),(void*)& Cyc__genrep_778};static struct _tuple6 Cyc__gentuple_807={
offsetof(struct _tuple68,f4),(void*)& Cyc__genrep_130};static struct _tuple6*Cyc__genarr_808[
4]={& Cyc__gentuple_804,& Cyc__gentuple_805,& Cyc__gentuple_806,& Cyc__gentuple_807};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_777={4,sizeof(struct _tuple68),{(
void*)((struct _tuple6**)Cyc__genarr_808),(void*)((struct _tuple6**)Cyc__genarr_808),(
void*)((struct _tuple6**)Cyc__genarr_808 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_766;extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_incrementor_t_rep;
static char _tmp3AE[7]="PreInc";static struct _tuple7 Cyc__gentuple_767={0,(struct
_dyneither_ptr){_tmp3AE,_tmp3AE,_tmp3AE + 7}};static char _tmp3AF[8]="PostInc";
static struct _tuple7 Cyc__gentuple_768={1,(struct _dyneither_ptr){_tmp3AF,_tmp3AF,
_tmp3AF + 8}};static char _tmp3B0[7]="PreDec";static struct _tuple7 Cyc__gentuple_769={
2,(struct _dyneither_ptr){_tmp3B0,_tmp3B0,_tmp3B0 + 7}};static char _tmp3B1[8]="PostDec";
static struct _tuple7 Cyc__gentuple_770={3,(struct _dyneither_ptr){_tmp3B1,_tmp3B1,
_tmp3B1 + 8}};static struct _tuple7*Cyc__genarr_771[4]={& Cyc__gentuple_767,& Cyc__gentuple_768,&
Cyc__gentuple_769,& Cyc__gentuple_770};static struct _tuple5*Cyc__genarr_772[0]={};
static char _tmp3B3[12]="Incrementor";struct Cyc_Typerep_TUnion_struct Cyc_Absyn_incrementor_t_rep={
5,(struct _dyneither_ptr){_tmp3B3,_tmp3B3,_tmp3B3 + 12},{(void*)((struct _tuple7**)
Cyc__genarr_771),(void*)((struct _tuple7**)Cyc__genarr_771),(void*)((struct
_tuple7**)Cyc__genarr_771 + 4)},{(void*)((struct _tuple5**)Cyc__genarr_772),(void*)((
struct _tuple5**)Cyc__genarr_772),(void*)((struct _tuple5**)Cyc__genarr_772 + 0)}};
struct _tuple69{unsigned int f1;struct Cyc_Absyn_Exp*f2;void*f3;};static struct
_tuple6 Cyc__gentuple_773={offsetof(struct _tuple69,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_774={offsetof(struct _tuple69,f2),(void*)& Cyc__genrep_130};
static struct _tuple6 Cyc__gentuple_775={offsetof(struct _tuple69,f3),(void*)& Cyc_Absyn_incrementor_t_rep};
static struct _tuple6*Cyc__genarr_776[3]={& Cyc__gentuple_773,& Cyc__gentuple_774,&
Cyc__gentuple_775};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_766={4,
sizeof(struct _tuple69),{(void*)((struct _tuple6**)Cyc__genarr_776),(void*)((
struct _tuple6**)Cyc__genarr_776),(void*)((struct _tuple6**)Cyc__genarr_776 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_760;struct _tuple70{unsigned int
f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;struct Cyc_Absyn_Exp*f4;};static
struct _tuple6 Cyc__gentuple_761={offsetof(struct _tuple70,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_762={offsetof(struct _tuple70,f2),(void*)& Cyc__genrep_130};
static struct _tuple6 Cyc__gentuple_763={offsetof(struct _tuple70,f3),(void*)& Cyc__genrep_130};
static struct _tuple6 Cyc__gentuple_764={offsetof(struct _tuple70,f4),(void*)& Cyc__genrep_130};
static struct _tuple6*Cyc__genarr_765[4]={& Cyc__gentuple_761,& Cyc__gentuple_762,&
Cyc__gentuple_763,& Cyc__gentuple_764};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_760={
4,sizeof(struct _tuple70),{(void*)((struct _tuple6**)Cyc__genarr_765),(void*)((
struct _tuple6**)Cyc__genarr_765),(void*)((struct _tuple6**)Cyc__genarr_765 + 4)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_610;struct _tuple71{unsigned int
f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};static struct _tuple6 Cyc__gentuple_611={
offsetof(struct _tuple71,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_612={
offsetof(struct _tuple71,f2),(void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_613={
offsetof(struct _tuple71,f3),(void*)& Cyc__genrep_130};static struct _tuple6*Cyc__genarr_614[
3]={& Cyc__gentuple_611,& Cyc__gentuple_612,& Cyc__gentuple_613};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_610={4,sizeof(struct _tuple71),{(void*)((struct _tuple6**)Cyc__genarr_614),(
void*)((struct _tuple6**)Cyc__genarr_614),(void*)((struct _tuple6**)Cyc__genarr_614
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_755;static struct _tuple6
Cyc__gentuple_756={offsetof(struct _tuple58,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_757={offsetof(struct _tuple58,f2),(void*)& Cyc__genrep_130};
static struct _tuple6 Cyc__gentuple_758={offsetof(struct _tuple58,f3),(void*)& Cyc__genrep_511};
static struct _tuple6*Cyc__genarr_759[3]={& Cyc__gentuple_756,& Cyc__gentuple_757,&
Cyc__gentuple_758};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_755={4,
sizeof(struct _tuple58),{(void*)((struct _tuple6**)Cyc__genarr_759),(void*)((
struct _tuple6**)Cyc__genarr_759),(void*)((struct _tuple6**)Cyc__genarr_759 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_736;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_737;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_vararg_call_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_738;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_738={1,1,(void*)((void*)& Cyc_Absyn_vararg_info_t_rep)};static char
_tmp3B9[15]="VarargCallInfo";static struct _dyneither_ptr Cyc__genname_749=(struct
_dyneither_ptr){_tmp3B9,_tmp3B9,_tmp3B9 + 15};static char _tmp3BA[12]="num_varargs";
static struct _tuple5 Cyc__gentuple_745={offsetof(struct Cyc_Absyn_VarargCallInfo,num_varargs),(
struct _dyneither_ptr){_tmp3BA,_tmp3BA,_tmp3BA + 12},(void*)& Cyc__genrep_10};
static char _tmp3BB[10]="injectors";static struct _tuple5 Cyc__gentuple_746={
offsetof(struct Cyc_Absyn_VarargCallInfo,injectors),(struct _dyneither_ptr){
_tmp3BB,_tmp3BB,_tmp3BB + 10},(void*)& Cyc__genrep_318};static char _tmp3BC[4]="vai";
static struct _tuple5 Cyc__gentuple_747={offsetof(struct Cyc_Absyn_VarargCallInfo,vai),(
struct _dyneither_ptr){_tmp3BC,_tmp3BC,_tmp3BC + 4},(void*)& Cyc__genrep_738};
static struct _tuple5*Cyc__genarr_748[3]={& Cyc__gentuple_745,& Cyc__gentuple_746,&
Cyc__gentuple_747};struct Cyc_Typerep_Struct_struct Cyc_Absyn_vararg_call_info_t_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_749,sizeof(struct Cyc_Absyn_VarargCallInfo),{(
void*)((struct _tuple5**)Cyc__genarr_748),(void*)((struct _tuple5**)Cyc__genarr_748),(
void*)((struct _tuple5**)Cyc__genarr_748 + 3)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_737={1,1,(void*)((void*)& Cyc_Absyn_vararg_call_info_t_rep)};struct
_tuple72{unsigned int f1;struct Cyc_Absyn_Exp*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_VarargCallInfo*
f4;};static struct _tuple6 Cyc__gentuple_750={offsetof(struct _tuple72,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_751={offsetof(struct _tuple72,f2),(
void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_752={offsetof(struct
_tuple72,f3),(void*)& Cyc__genrep_511};static struct _tuple6 Cyc__gentuple_753={
offsetof(struct _tuple72,f4),(void*)& Cyc__genrep_737};static struct _tuple6*Cyc__genarr_754[
4]={& Cyc__gentuple_750,& Cyc__gentuple_751,& Cyc__gentuple_752,& Cyc__gentuple_753};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_736={4,sizeof(struct _tuple72),{(
void*)((struct _tuple6**)Cyc__genarr_754),(void*)((struct _tuple6**)Cyc__genarr_754),(
void*)((struct _tuple6**)Cyc__genarr_754 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_731;static struct _tuple6 Cyc__gentuple_732={offsetof(struct _tuple58,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_733={offsetof(struct
_tuple58,f2),(void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_734={
offsetof(struct _tuple58,f3),(void*)& Cyc__genrep_121};static struct _tuple6*Cyc__genarr_735[
3]={& Cyc__gentuple_732,& Cyc__gentuple_733,& Cyc__gentuple_734};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_731={4,sizeof(struct _tuple58),{(void*)((struct _tuple6**)Cyc__genarr_735),(
void*)((struct _tuple6**)Cyc__genarr_735),(void*)((struct _tuple6**)Cyc__genarr_735
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_718;extern struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_coercion_t_rep;static char _tmp3C1[17]="Unknown_coercion";static struct
_tuple7 Cyc__gentuple_719={0,(struct _dyneither_ptr){_tmp3C1,_tmp3C1,_tmp3C1 + 17}};
static char _tmp3C2[12]="No_coercion";static struct _tuple7 Cyc__gentuple_720={1,(
struct _dyneither_ptr){_tmp3C2,_tmp3C2,_tmp3C2 + 12}};static char _tmp3C3[16]="NonNull_to_Null";
static struct _tuple7 Cyc__gentuple_721={2,(struct _dyneither_ptr){_tmp3C3,_tmp3C3,
_tmp3C3 + 16}};static char _tmp3C4[15]="Other_coercion";static struct _tuple7 Cyc__gentuple_722={
3,(struct _dyneither_ptr){_tmp3C4,_tmp3C4,_tmp3C4 + 15}};static struct _tuple7*Cyc__genarr_723[
4]={& Cyc__gentuple_719,& Cyc__gentuple_720,& Cyc__gentuple_721,& Cyc__gentuple_722};
static struct _tuple5*Cyc__genarr_724[0]={};static char _tmp3C6[9]="Coercion";struct
Cyc_Typerep_TUnion_struct Cyc_Absyn_coercion_t_rep={5,(struct _dyneither_ptr){
_tmp3C6,_tmp3C6,_tmp3C6 + 9},{(void*)((struct _tuple7**)Cyc__genarr_723),(void*)((
struct _tuple7**)Cyc__genarr_723),(void*)((struct _tuple7**)Cyc__genarr_723 + 4)},{(
void*)((struct _tuple5**)Cyc__genarr_724),(void*)((struct _tuple5**)Cyc__genarr_724),(
void*)((struct _tuple5**)Cyc__genarr_724 + 0)}};struct _tuple73{unsigned int f1;void*
f2;struct Cyc_Absyn_Exp*f3;int f4;void*f5;};static struct _tuple6 Cyc__gentuple_725={
offsetof(struct _tuple73,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_726={
offsetof(struct _tuple73,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_727={offsetof(struct _tuple73,f3),(void*)& Cyc__genrep_130};
static struct _tuple6 Cyc__gentuple_728={offsetof(struct _tuple73,f4),(void*)& Cyc__genrep_10};
static struct _tuple6 Cyc__gentuple_729={offsetof(struct _tuple73,f5),(void*)& Cyc_Absyn_coercion_t_rep};
static struct _tuple6*Cyc__genarr_730[5]={& Cyc__gentuple_725,& Cyc__gentuple_726,&
Cyc__gentuple_727,& Cyc__gentuple_728,& Cyc__gentuple_729};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_718={4,sizeof(struct _tuple73),{(void*)((struct _tuple6**)Cyc__genarr_730),(
void*)((struct _tuple6**)Cyc__genarr_730),(void*)((struct _tuple6**)Cyc__genarr_730
+ 5)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_713;static struct _tuple6
Cyc__gentuple_714={offsetof(struct _tuple71,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_715={offsetof(struct _tuple71,f2),(void*)& Cyc__genrep_144};
static struct _tuple6 Cyc__gentuple_716={offsetof(struct _tuple71,f3),(void*)& Cyc__genrep_130};
static struct _tuple6*Cyc__genarr_717[3]={& Cyc__gentuple_714,& Cyc__gentuple_715,&
Cyc__gentuple_716};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_713={4,
sizeof(struct _tuple71),{(void*)((struct _tuple6**)Cyc__genarr_717),(void*)((
struct _tuple6**)Cyc__genarr_717),(void*)((struct _tuple6**)Cyc__genarr_717 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_116;static struct _tuple6 Cyc__gentuple_117={
offsetof(struct _tuple6,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_118={
offsetof(struct _tuple6,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_119[2]={& Cyc__gentuple_117,& Cyc__gentuple_118};static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_116={4,sizeof(struct _tuple6),{(void*)((
struct _tuple6**)Cyc__genarr_119),(void*)((struct _tuple6**)Cyc__genarr_119),(void*)((
struct _tuple6**)Cyc__genarr_119 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_700;
extern struct Cyc_Typerep_TUnion_struct Cyc_Absyn_offsetof_field_t_rep;extern struct
Cyc_Typerep_Tuple_struct Cyc__genrep_702;struct _tuple74{unsigned int f1;
unsigned int f2;};static struct _tuple6 Cyc__gentuple_703={offsetof(struct _tuple74,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_704={offsetof(struct
_tuple74,f2),(void*)& Cyc__genrep_24};static struct _tuple6*Cyc__genarr_705[2]={&
Cyc__gentuple_703,& Cyc__gentuple_704};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_702={
4,sizeof(struct _tuple74),{(void*)((struct _tuple6**)Cyc__genarr_705),(void*)((
struct _tuple6**)Cyc__genarr_705),(void*)((struct _tuple6**)Cyc__genarr_705 + 2)}};
static struct _tuple7*Cyc__genarr_701[0]={};static char _tmp3CB[12]="StructField";
static struct _tuple5 Cyc__gentuple_706={0,(struct _dyneither_ptr){_tmp3CB,_tmp3CB,
_tmp3CB + 12},(void*)& Cyc__genrep_351};static char _tmp3CC[11]="TupleIndex";static
struct _tuple5 Cyc__gentuple_707={1,(struct _dyneither_ptr){_tmp3CC,_tmp3CC,_tmp3CC
+ 11},(void*)& Cyc__genrep_702};static struct _tuple5*Cyc__genarr_708[2]={& Cyc__gentuple_706,&
Cyc__gentuple_707};static char _tmp3CE[14]="OffsetofField";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_offsetof_field_t_rep={5,(struct _dyneither_ptr){_tmp3CE,_tmp3CE,_tmp3CE
+ 14},{(void*)((struct _tuple7**)Cyc__genarr_701),(void*)((struct _tuple7**)Cyc__genarr_701),(
void*)((struct _tuple7**)Cyc__genarr_701 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_708),(
void*)((struct _tuple5**)Cyc__genarr_708),(void*)((struct _tuple5**)Cyc__genarr_708
+ 2)}};static struct _tuple6 Cyc__gentuple_709={offsetof(struct _tuple28,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_710={offsetof(struct _tuple28,f2),(
void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_711={
offsetof(struct _tuple28,f3),(void*)& Cyc_Absyn_offsetof_field_t_rep};static struct
_tuple6*Cyc__genarr_712[3]={& Cyc__gentuple_709,& Cyc__gentuple_710,& Cyc__gentuple_711};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_700={4,sizeof(struct _tuple28),{(
void*)((struct _tuple6**)Cyc__genarr_712),(void*)((struct _tuple6**)Cyc__genarr_712),(
void*)((struct _tuple6**)Cyc__genarr_712 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_695;struct _tuple75{unsigned int f1;struct Cyc_List_List*f2;void*f3;};
static struct _tuple6 Cyc__gentuple_696={offsetof(struct _tuple75,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_697={offsetof(struct _tuple75,f2),(void*)& Cyc__genrep_326};
static struct _tuple6 Cyc__gentuple_698={offsetof(struct _tuple75,f3),(void*)((void*)&
Cyc_Absyn_type_t_rep)};static struct _tuple6*Cyc__genarr_699[3]={& Cyc__gentuple_696,&
Cyc__gentuple_697,& Cyc__gentuple_698};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_695={
4,sizeof(struct _tuple75),{(void*)((struct _tuple6**)Cyc__genarr_699),(void*)((
struct _tuple6**)Cyc__genarr_699),(void*)((struct _tuple6**)Cyc__genarr_699 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_690;struct _tuple76{unsigned int
f1;struct Cyc_Absyn_Exp*f2;struct _dyneither_ptr*f3;};static struct _tuple6 Cyc__gentuple_691={
offsetof(struct _tuple76,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_692={
offsetof(struct _tuple76,f2),(void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_693={
offsetof(struct _tuple76,f3),(void*)& Cyc__genrep_13};static struct _tuple6*Cyc__genarr_694[
3]={& Cyc__gentuple_691,& Cyc__gentuple_692,& Cyc__gentuple_693};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_690={4,sizeof(struct _tuple76),{(void*)((struct _tuple6**)Cyc__genarr_694),(
void*)((struct _tuple6**)Cyc__genarr_694),(void*)((struct _tuple6**)Cyc__genarr_694
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_686;static struct _tuple6
Cyc__gentuple_687={offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_688={offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_511};
static struct _tuple6*Cyc__genarr_689[2]={& Cyc__gentuple_687,& Cyc__gentuple_688};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_686={4,sizeof(struct _tuple14),{(
void*)((struct _tuple6**)Cyc__genarr_689),(void*)((struct _tuple6**)Cyc__genarr_689),(
void*)((struct _tuple6**)Cyc__genarr_689 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_671;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_672;extern
struct Cyc_Typerep_Tuple_struct Cyc__genrep_673;static struct _tuple6 Cyc__gentuple_678={
offsetof(struct _tuple1,f1),(void*)& Cyc__genrep_674};static struct _tuple6 Cyc__gentuple_679={
offsetof(struct _tuple1,f2),(void*)& Cyc_Absyn_tqual_t_rep};static struct _tuple6 Cyc__gentuple_680={
offsetof(struct _tuple1,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_681[3]={& Cyc__gentuple_678,& Cyc__gentuple_679,& Cyc__gentuple_680};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_673={4,sizeof(struct _tuple1),{(
void*)((struct _tuple6**)Cyc__genarr_681),(void*)((struct _tuple6**)Cyc__genarr_681),(
void*)((struct _tuple6**)Cyc__genarr_681 + 3)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_672={1,1,(void*)((void*)& Cyc__genrep_673)};extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_592;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_593;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_594;static struct _tuple6 Cyc__gentuple_595={offsetof(struct _tuple13,f1),(
void*)& Cyc__genrep_349};static struct _tuple6 Cyc__gentuple_596={offsetof(struct
_tuple13,f2),(void*)& Cyc__genrep_130};static struct _tuple6*Cyc__genarr_597[2]={&
Cyc__gentuple_595,& Cyc__gentuple_596};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_594={
4,sizeof(struct _tuple13),{(void*)((struct _tuple6**)Cyc__genarr_597),(void*)((
struct _tuple6**)Cyc__genarr_597),(void*)((struct _tuple6**)Cyc__genarr_597 + 2)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_593={1,1,(void*)((void*)& Cyc__genrep_594)};
static char _tmp3D7[5]="List";static struct _dyneither_ptr Cyc__genname_601=(struct
_dyneither_ptr){_tmp3D7,_tmp3D7,_tmp3D7 + 5};static char _tmp3D8[3]="hd";static
struct _tuple5 Cyc__gentuple_598={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp3D8,_tmp3D8,_tmp3D8 + 3},(void*)& Cyc__genrep_593};static char
_tmp3D9[3]="tl";static struct _tuple5 Cyc__gentuple_599={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp3D9,_tmp3D9,_tmp3D9 + 3},(void*)& Cyc__genrep_592};
static struct _tuple5*Cyc__genarr_600[2]={& Cyc__gentuple_598,& Cyc__gentuple_599};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_601,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_600),(void*)((struct _tuple5**)Cyc__genarr_600),(void*)((
struct _tuple5**)Cyc__genarr_600 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_592={
1,1,(void*)((void*)& Cyc_struct_List_List060List_list_t0Absyn_designator_t46H24Absyn_exp_t1_446H2_rep)};
struct _tuple77{unsigned int f1;struct _tuple1*f2;struct Cyc_List_List*f3;};static
struct _tuple6 Cyc__gentuple_682={offsetof(struct _tuple77,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_683={offsetof(struct _tuple77,f2),(void*)& Cyc__genrep_672};
static struct _tuple6 Cyc__gentuple_684={offsetof(struct _tuple77,f3),(void*)& Cyc__genrep_592};
static struct _tuple6*Cyc__genarr_685[3]={& Cyc__gentuple_682,& Cyc__gentuple_683,&
Cyc__gentuple_684};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_671={4,
sizeof(struct _tuple77),{(void*)((struct _tuple6**)Cyc__genarr_685),(void*)((
struct _tuple6**)Cyc__genarr_685),(void*)((struct _tuple6**)Cyc__genarr_685 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_667;static struct _tuple6 Cyc__gentuple_668={
offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_669={
offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_592};static struct _tuple6*Cyc__genarr_670[
2]={& Cyc__gentuple_668,& Cyc__gentuple_669};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_667={4,sizeof(struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_670),(
void*)((struct _tuple6**)Cyc__genarr_670),(void*)((struct _tuple6**)Cyc__genarr_670
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_660;struct _tuple78{
unsigned int f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;struct Cyc_Absyn_Exp*
f4;int f5;};static struct _tuple6 Cyc__gentuple_661={offsetof(struct _tuple78,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_662={offsetof(struct
_tuple78,f2),(void*)& Cyc__genrep_143};static struct _tuple6 Cyc__gentuple_663={
offsetof(struct _tuple78,f3),(void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_664={
offsetof(struct _tuple78,f4),(void*)& Cyc__genrep_130};static struct _tuple6 Cyc__gentuple_665={
offsetof(struct _tuple78,f5),(void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_666[
5]={& Cyc__gentuple_661,& Cyc__gentuple_662,& Cyc__gentuple_663,& Cyc__gentuple_664,&
Cyc__gentuple_665};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_660={4,
sizeof(struct _tuple78),{(void*)((struct _tuple6**)Cyc__genarr_666),(void*)((
struct _tuple6**)Cyc__genarr_666),(void*)((struct _tuple6**)Cyc__genarr_666 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_652;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_653;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_653={1,1,(void*)((
void*)& Cyc_struct_Absyn_Aggrdecl_rep)};struct _tuple79{unsigned int f1;struct
_tuple0*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_Absyn_Aggrdecl*
f5;};static struct _tuple6 Cyc__gentuple_654={offsetof(struct _tuple79,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_655={offsetof(struct _tuple79,f2),(
void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_656={offsetof(struct
_tuple79,f3),(void*)& Cyc__genrep_121};static struct _tuple6 Cyc__gentuple_657={
offsetof(struct _tuple79,f4),(void*)& Cyc__genrep_592};static struct _tuple6 Cyc__gentuple_658={
offsetof(struct _tuple79,f5),(void*)& Cyc__genrep_653};static struct _tuple6*Cyc__genarr_659[
5]={& Cyc__gentuple_654,& Cyc__gentuple_655,& Cyc__gentuple_656,& Cyc__gentuple_657,&
Cyc__gentuple_658};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_652={4,
sizeof(struct _tuple79),{(void*)((struct _tuple6**)Cyc__genarr_659),(void*)((
struct _tuple6**)Cyc__genarr_659),(void*)((struct _tuple6**)Cyc__genarr_659 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_647;static struct _tuple6 Cyc__gentuple_648={
offsetof(struct _tuple67,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_649={
offsetof(struct _tuple67,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6 Cyc__gentuple_650={offsetof(struct _tuple67,f3),(void*)& Cyc__genrep_592};
static struct _tuple6*Cyc__genarr_651[3]={& Cyc__gentuple_648,& Cyc__gentuple_649,&
Cyc__gentuple_650};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_647={4,
sizeof(struct _tuple67),{(void*)((struct _tuple6**)Cyc__genarr_651),(void*)((
struct _tuple6**)Cyc__genarr_651),(void*)((struct _tuple6**)Cyc__genarr_651 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_641;struct _tuple80{unsigned int
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Tuniondecl*f3;struct Cyc_Absyn_Tunionfield*
f4;};static struct _tuple6 Cyc__gentuple_642={offsetof(struct _tuple80,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_643={offsetof(struct _tuple80,f2),(
void*)& Cyc__genrep_511};static struct _tuple6 Cyc__gentuple_644={offsetof(struct
_tuple80,f3),(void*)((void*)& Cyc__genrep_316)};static struct _tuple6 Cyc__gentuple_645={
offsetof(struct _tuple80,f4),(void*)& Cyc__genrep_299};static struct _tuple6*Cyc__genarr_646[
4]={& Cyc__gentuple_642,& Cyc__gentuple_643,& Cyc__gentuple_644,& Cyc__gentuple_645};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_641={4,sizeof(struct _tuple80),{(
void*)((struct _tuple6**)Cyc__genarr_646),(void*)((struct _tuple6**)Cyc__genarr_646),(
void*)((struct _tuple6**)Cyc__genarr_646 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_634;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_635;static
struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_635={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumdecl_rep)};
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_628;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_628={1,1,(void*)((void*)& Cyc_struct_Absyn_Enumfield_rep)};struct
_tuple81{unsigned int f1;struct _tuple0*f2;struct Cyc_Absyn_Enumdecl*f3;struct Cyc_Absyn_Enumfield*
f4;};static struct _tuple6 Cyc__gentuple_636={offsetof(struct _tuple81,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_637={offsetof(struct _tuple81,f2),(
void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_638={offsetof(struct
_tuple81,f3),(void*)& Cyc__genrep_635};static struct _tuple6 Cyc__gentuple_639={
offsetof(struct _tuple81,f4),(void*)& Cyc__genrep_628};static struct _tuple6*Cyc__genarr_640[
4]={& Cyc__gentuple_636,& Cyc__gentuple_637,& Cyc__gentuple_638,& Cyc__gentuple_639};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_634={4,sizeof(struct _tuple81),{(
void*)((struct _tuple6**)Cyc__genarr_640),(void*)((struct _tuple6**)Cyc__genarr_640),(
void*)((struct _tuple6**)Cyc__genarr_640 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_627;struct _tuple82{unsigned int f1;struct _tuple0*f2;void*f3;struct Cyc_Absyn_Enumfield*
f4;};static struct _tuple6 Cyc__gentuple_629={offsetof(struct _tuple82,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_630={offsetof(struct _tuple82,f2),(
void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_631={offsetof(struct
_tuple82,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_632={
offsetof(struct _tuple82,f4),(void*)& Cyc__genrep_628};static struct _tuple6*Cyc__genarr_633[
4]={& Cyc__gentuple_629,& Cyc__gentuple_630,& Cyc__gentuple_631,& Cyc__gentuple_632};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_627={4,sizeof(struct _tuple82),{(
void*)((struct _tuple6**)Cyc__genarr_633),(void*)((struct _tuple6**)Cyc__genarr_633),(
void*)((struct _tuple6**)Cyc__genarr_633 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_615;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_malloc_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_616;static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_616={1,1,(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp3E8[11]="MallocInfo";
static struct _dyneither_ptr Cyc__genname_623=(struct _dyneither_ptr){_tmp3E8,
_tmp3E8,_tmp3E8 + 11};static char _tmp3E9[10]="is_calloc";static struct _tuple5 Cyc__gentuple_617={
offsetof(struct Cyc_Absyn_MallocInfo,is_calloc),(struct _dyneither_ptr){_tmp3E9,
_tmp3E9,_tmp3E9 + 10},(void*)& Cyc__genrep_10};static char _tmp3EA[4]="rgn";static
struct _tuple5 Cyc__gentuple_618={offsetof(struct Cyc_Absyn_MallocInfo,rgn),(struct
_dyneither_ptr){_tmp3EA,_tmp3EA,_tmp3EA + 4},(void*)& Cyc__genrep_144};static char
_tmp3EB[9]="elt_type";static struct _tuple5 Cyc__gentuple_619={offsetof(struct Cyc_Absyn_MallocInfo,elt_type),(
struct _dyneither_ptr){_tmp3EB,_tmp3EB,_tmp3EB + 9},(void*)& Cyc__genrep_616};
static char _tmp3EC[9]="num_elts";static struct _tuple5 Cyc__gentuple_620={offsetof(
struct Cyc_Absyn_MallocInfo,num_elts),(struct _dyneither_ptr){_tmp3EC,_tmp3EC,
_tmp3EC + 9},(void*)& Cyc__genrep_130};static char _tmp3ED[11]="fat_result";static
struct _tuple5 Cyc__gentuple_621={offsetof(struct Cyc_Absyn_MallocInfo,fat_result),(
struct _dyneither_ptr){_tmp3ED,_tmp3ED,_tmp3ED + 11},(void*)& Cyc__genrep_10};
static struct _tuple5*Cyc__genarr_622[5]={& Cyc__gentuple_617,& Cyc__gentuple_618,&
Cyc__gentuple_619,& Cyc__gentuple_620,& Cyc__gentuple_621};struct Cyc_Typerep_Struct_struct
Cyc_Absyn_malloc_info_t_rep={3,(struct _dyneither_ptr*)& Cyc__genname_623,sizeof(
struct Cyc_Absyn_MallocInfo),{(void*)((struct _tuple5**)Cyc__genarr_622),(void*)((
struct _tuple5**)Cyc__genarr_622),(void*)((struct _tuple5**)Cyc__genarr_622 + 5)}};
struct _tuple83{unsigned int f1;struct Cyc_Absyn_MallocInfo f2;};static struct _tuple6
Cyc__gentuple_624={offsetof(struct _tuple83,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_625={offsetof(struct _tuple83,f2),(void*)& Cyc_Absyn_malloc_info_t_rep};
static struct _tuple6*Cyc__genarr_626[2]={& Cyc__gentuple_624,& Cyc__gentuple_625};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_615={4,sizeof(struct _tuple83),{(
void*)((struct _tuple6**)Cyc__genarr_626),(void*)((struct _tuple6**)Cyc__genarr_626),(
void*)((struct _tuple6**)Cyc__genarr_626 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_591;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_602;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep;
static char _tmp3F0[4]="Opt";static struct _dyneither_ptr Cyc__genname_605=(struct
_dyneither_ptr){_tmp3F0,_tmp3F0,_tmp3F0 + 4};static char _tmp3F1[2]="v";static
struct _tuple5 Cyc__gentuple_603={offsetof(struct Cyc_Core_Opt,v),(struct
_dyneither_ptr){_tmp3F1,_tmp3F1,_tmp3F1 + 2},(void*)& Cyc__genrep_11};static struct
_tuple5*Cyc__genarr_604[1]={& Cyc__gentuple_603};struct Cyc_Typerep_Struct_struct
Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep={3,(struct _dyneither_ptr*)& Cyc__genname_605,
sizeof(struct Cyc_Core_Opt),{(void*)((struct _tuple5**)Cyc__genarr_604),(void*)((
struct _tuple5**)Cyc__genarr_604),(void*)((struct _tuple5**)Cyc__genarr_604 + 1)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_602={1,1,(void*)((void*)& Cyc_struct_Core_Opt0Absyn_typedef_name_t2_rep)};
struct _tuple84{unsigned int f1;struct Cyc_Core_Opt*f2;struct Cyc_List_List*f3;};
static struct _tuple6 Cyc__gentuple_606={offsetof(struct _tuple84,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_607={offsetof(struct _tuple84,f2),(void*)& Cyc__genrep_602};
static struct _tuple6 Cyc__gentuple_608={offsetof(struct _tuple84,f3),(void*)& Cyc__genrep_592};
static struct _tuple6*Cyc__genarr_609[3]={& Cyc__gentuple_606,& Cyc__gentuple_607,&
Cyc__gentuple_608};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_591={4,
sizeof(struct _tuple84),{(void*)((struct _tuple6**)Cyc__genarr_609),(void*)((
struct _tuple6**)Cyc__genarr_609),(void*)((struct _tuple6**)Cyc__genarr_609 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_133;static struct _tuple6 Cyc__gentuple_588={
offsetof(struct _tuple39,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_589={
offsetof(struct _tuple39,f2),(void*)& Cyc__genrep_134};static struct _tuple6*Cyc__genarr_590[
2]={& Cyc__gentuple_588,& Cyc__gentuple_589};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_133={4,sizeof(struct _tuple39),{(void*)((struct _tuple6**)Cyc__genarr_590),(
void*)((struct _tuple6**)Cyc__genarr_590),(void*)((struct _tuple6**)Cyc__genarr_590
+ 2)}};static struct _tuple7*Cyc__genarr_132[0]={};static char _tmp3F6[8]="Const_e";
static struct _tuple5 Cyc__gentuple_896={0,(struct _dyneither_ptr){_tmp3F6,_tmp3F6,
_tmp3F6 + 8},(void*)& Cyc__genrep_866};static char _tmp3F7[6]="Var_e";static struct
_tuple5 Cyc__gentuple_897={1,(struct _dyneither_ptr){_tmp3F7,_tmp3F7,_tmp3F7 + 6},(
void*)& Cyc__genrep_814};static char _tmp3F8[12]="UnknownId_e";static struct _tuple5
Cyc__gentuple_898={2,(struct _dyneither_ptr){_tmp3F8,_tmp3F8,_tmp3F8 + 12},(void*)&
Cyc__genrep_249};static char _tmp3F9[9]="Primop_e";static struct _tuple5 Cyc__gentuple_899={
3,(struct _dyneither_ptr){_tmp3F9,_tmp3F9,_tmp3F9 + 9},(void*)& Cyc__genrep_809};
static char _tmp3FA[11]="AssignOp_e";static struct _tuple5 Cyc__gentuple_900={4,(
struct _dyneither_ptr){_tmp3FA,_tmp3FA,_tmp3FA + 11},(void*)& Cyc__genrep_777};
static char _tmp3FB[12]="Increment_e";static struct _tuple5 Cyc__gentuple_901={5,(
struct _dyneither_ptr){_tmp3FB,_tmp3FB,_tmp3FB + 12},(void*)& Cyc__genrep_766};
static char _tmp3FC[14]="Conditional_e";static struct _tuple5 Cyc__gentuple_902={6,(
struct _dyneither_ptr){_tmp3FC,_tmp3FC,_tmp3FC + 14},(void*)& Cyc__genrep_760};
static char _tmp3FD[6]="And_e";static struct _tuple5 Cyc__gentuple_903={7,(struct
_dyneither_ptr){_tmp3FD,_tmp3FD,_tmp3FD + 6},(void*)& Cyc__genrep_610};static char
_tmp3FE[5]="Or_e";static struct _tuple5 Cyc__gentuple_904={8,(struct _dyneither_ptr){
_tmp3FE,_tmp3FE,_tmp3FE + 5},(void*)& Cyc__genrep_610};static char _tmp3FF[9]="SeqExp_e";
static struct _tuple5 Cyc__gentuple_905={9,(struct _dyneither_ptr){_tmp3FF,_tmp3FF,
_tmp3FF + 9},(void*)& Cyc__genrep_610};static char _tmp400[14]="UnknownCall_e";
static struct _tuple5 Cyc__gentuple_906={10,(struct _dyneither_ptr){_tmp400,_tmp400,
_tmp400 + 14},(void*)& Cyc__genrep_755};static char _tmp401[9]="FnCall_e";static
struct _tuple5 Cyc__gentuple_907={11,(struct _dyneither_ptr){_tmp401,_tmp401,
_tmp401 + 9},(void*)& Cyc__genrep_736};static char _tmp402[8]="Throw_e";static struct
_tuple5 Cyc__gentuple_908={12,(struct _dyneither_ptr){_tmp402,_tmp402,_tmp402 + 8},(
void*)& Cyc__genrep_129};static char _tmp403[16]="NoInstantiate_e";static struct
_tuple5 Cyc__gentuple_909={13,(struct _dyneither_ptr){_tmp403,_tmp403,_tmp403 + 16},(
void*)& Cyc__genrep_129};static char _tmp404[14]="Instantiate_e";static struct
_tuple5 Cyc__gentuple_910={14,(struct _dyneither_ptr){_tmp404,_tmp404,_tmp404 + 14},(
void*)& Cyc__genrep_731};static char _tmp405[7]="Cast_e";static struct _tuple5 Cyc__gentuple_911={
15,(struct _dyneither_ptr){_tmp405,_tmp405,_tmp405 + 7},(void*)& Cyc__genrep_718};
static char _tmp406[10]="Address_e";static struct _tuple5 Cyc__gentuple_912={16,(
struct _dyneither_ptr){_tmp406,_tmp406,_tmp406 + 10},(void*)& Cyc__genrep_129};
static char _tmp407[6]="New_e";static struct _tuple5 Cyc__gentuple_913={17,(struct
_dyneither_ptr){_tmp407,_tmp407,_tmp407 + 6},(void*)& Cyc__genrep_713};static char
_tmp408[12]="Sizeoftyp_e";static struct _tuple5 Cyc__gentuple_914={18,(struct
_dyneither_ptr){_tmp408,_tmp408,_tmp408 + 12},(void*)& Cyc__genrep_116};static char
_tmp409[12]="Sizeofexp_e";static struct _tuple5 Cyc__gentuple_915={19,(struct
_dyneither_ptr){_tmp409,_tmp409,_tmp409 + 12},(void*)& Cyc__genrep_129};static char
_tmp40A[11]="Offsetof_e";static struct _tuple5 Cyc__gentuple_916={20,(struct
_dyneither_ptr){_tmp40A,_tmp40A,_tmp40A + 11},(void*)& Cyc__genrep_700};static char
_tmp40B[9]="Gentyp_e";static struct _tuple5 Cyc__gentuple_917={21,(struct
_dyneither_ptr){_tmp40B,_tmp40B,_tmp40B + 9},(void*)& Cyc__genrep_695};static char
_tmp40C[8]="Deref_e";static struct _tuple5 Cyc__gentuple_918={22,(struct
_dyneither_ptr){_tmp40C,_tmp40C,_tmp40C + 8},(void*)& Cyc__genrep_129};static char
_tmp40D[13]="AggrMember_e";static struct _tuple5 Cyc__gentuple_919={23,(struct
_dyneither_ptr){_tmp40D,_tmp40D,_tmp40D + 13},(void*)& Cyc__genrep_690};static char
_tmp40E[12]="AggrArrow_e";static struct _tuple5 Cyc__gentuple_920={24,(struct
_dyneither_ptr){_tmp40E,_tmp40E,_tmp40E + 12},(void*)& Cyc__genrep_690};static char
_tmp40F[12]="Subscript_e";static struct _tuple5 Cyc__gentuple_921={25,(struct
_dyneither_ptr){_tmp40F,_tmp40F,_tmp40F + 12},(void*)& Cyc__genrep_610};static char
_tmp410[8]="Tuple_e";static struct _tuple5 Cyc__gentuple_922={26,(struct
_dyneither_ptr){_tmp410,_tmp410,_tmp410 + 8},(void*)& Cyc__genrep_686};static char
_tmp411[14]="CompoundLit_e";static struct _tuple5 Cyc__gentuple_923={27,(struct
_dyneither_ptr){_tmp411,_tmp411,_tmp411 + 14},(void*)& Cyc__genrep_671};static char
_tmp412[8]="Array_e";static struct _tuple5 Cyc__gentuple_924={28,(struct
_dyneither_ptr){_tmp412,_tmp412,_tmp412 + 8},(void*)& Cyc__genrep_667};static char
_tmp413[16]="Comprehension_e";static struct _tuple5 Cyc__gentuple_925={29,(struct
_dyneither_ptr){_tmp413,_tmp413,_tmp413 + 16},(void*)& Cyc__genrep_660};static char
_tmp414[9]="Struct_e";static struct _tuple5 Cyc__gentuple_926={30,(struct
_dyneither_ptr){_tmp414,_tmp414,_tmp414 + 9},(void*)& Cyc__genrep_652};static char
_tmp415[13]="AnonStruct_e";static struct _tuple5 Cyc__gentuple_927={31,(struct
_dyneither_ptr){_tmp415,_tmp415,_tmp415 + 13},(void*)& Cyc__genrep_647};static char
_tmp416[9]="Tunion_e";static struct _tuple5 Cyc__gentuple_928={32,(struct
_dyneither_ptr){_tmp416,_tmp416,_tmp416 + 9},(void*)& Cyc__genrep_641};static char
_tmp417[7]="Enum_e";static struct _tuple5 Cyc__gentuple_929={33,(struct
_dyneither_ptr){_tmp417,_tmp417,_tmp417 + 7},(void*)& Cyc__genrep_634};static char
_tmp418[11]="AnonEnum_e";static struct _tuple5 Cyc__gentuple_930={34,(struct
_dyneither_ptr){_tmp418,_tmp418,_tmp418 + 11},(void*)& Cyc__genrep_627};static char
_tmp419[9]="Malloc_e";static struct _tuple5 Cyc__gentuple_931={35,(struct
_dyneither_ptr){_tmp419,_tmp419,_tmp419 + 9},(void*)& Cyc__genrep_615};static char
_tmp41A[7]="Swap_e";static struct _tuple5 Cyc__gentuple_932={36,(struct
_dyneither_ptr){_tmp41A,_tmp41A,_tmp41A + 7},(void*)& Cyc__genrep_610};static char
_tmp41B[16]="UnresolvedMem_e";static struct _tuple5 Cyc__gentuple_933={37,(struct
_dyneither_ptr){_tmp41B,_tmp41B,_tmp41B + 16},(void*)& Cyc__genrep_591};static char
_tmp41C[10]="StmtExp_e";static struct _tuple5 Cyc__gentuple_934={38,(struct
_dyneither_ptr){_tmp41C,_tmp41C,_tmp41C + 10},(void*)& Cyc__genrep_133};static char
_tmp41D[10]="Valueof_e";static struct _tuple5 Cyc__gentuple_935={39,(struct
_dyneither_ptr){_tmp41D,_tmp41D,_tmp41D + 10},(void*)& Cyc__genrep_116};static
struct _tuple5*Cyc__genarr_936[40]={& Cyc__gentuple_896,& Cyc__gentuple_897,& Cyc__gentuple_898,&
Cyc__gentuple_899,& Cyc__gentuple_900,& Cyc__gentuple_901,& Cyc__gentuple_902,& Cyc__gentuple_903,&
Cyc__gentuple_904,& Cyc__gentuple_905,& Cyc__gentuple_906,& Cyc__gentuple_907,& Cyc__gentuple_908,&
Cyc__gentuple_909,& Cyc__gentuple_910,& Cyc__gentuple_911,& Cyc__gentuple_912,& Cyc__gentuple_913,&
Cyc__gentuple_914,& Cyc__gentuple_915,& Cyc__gentuple_916,& Cyc__gentuple_917,& Cyc__gentuple_918,&
Cyc__gentuple_919,& Cyc__gentuple_920,& Cyc__gentuple_921,& Cyc__gentuple_922,& Cyc__gentuple_923,&
Cyc__gentuple_924,& Cyc__gentuple_925,& Cyc__gentuple_926,& Cyc__gentuple_927,& Cyc__gentuple_928,&
Cyc__gentuple_929,& Cyc__gentuple_930,& Cyc__gentuple_931,& Cyc__gentuple_932,& Cyc__gentuple_933,&
Cyc__gentuple_934,& Cyc__gentuple_935};static char _tmp41F[8]="Raw_exp";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_exp_t_rep={5,(struct _dyneither_ptr){_tmp41F,_tmp41F,_tmp41F + 8},{(
void*)((struct _tuple7**)Cyc__genarr_132),(void*)((struct _tuple7**)Cyc__genarr_132),(
void*)((struct _tuple7**)Cyc__genarr_132 + 0)},{(void*)((struct _tuple5**)Cyc__genarr_936),(
void*)((struct _tuple5**)Cyc__genarr_936),(void*)((struct _tuple5**)Cyc__genarr_936
+ 40)}};static char _tmp420[4]="Exp";static struct _dyneither_ptr Cyc__genname_942=(
struct _dyneither_ptr){_tmp420,_tmp420,_tmp420 + 4};static char _tmp421[5]="topt";
static struct _tuple5 Cyc__gentuple_937={offsetof(struct Cyc_Absyn_Exp,topt),(struct
_dyneither_ptr){_tmp421,_tmp421,_tmp421 + 5},(void*)& Cyc__genrep_110};static char
_tmp422[2]="r";static struct _tuple5 Cyc__gentuple_938={offsetof(struct Cyc_Absyn_Exp,r),(
struct _dyneither_ptr){_tmp422,_tmp422,_tmp422 + 2},(void*)& Cyc_Absyn_raw_exp_t_rep};
static char _tmp423[4]="loc";static struct _tuple5 Cyc__gentuple_939={offsetof(struct
Cyc_Absyn_Exp,loc),(struct _dyneither_ptr){_tmp423,_tmp423,_tmp423 + 4},(void*)&
Cyc__genrep_2};static char _tmp424[6]="annot";static struct _tuple5 Cyc__gentuple_940={
offsetof(struct Cyc_Absyn_Exp,annot),(struct _dyneither_ptr){_tmp424,_tmp424,
_tmp424 + 6},(void*)& Cyc_Absyn_absyn_annot_t_rep};static struct _tuple5*Cyc__genarr_941[
4]={& Cyc__gentuple_937,& Cyc__gentuple_938,& Cyc__gentuple_939,& Cyc__gentuple_940};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Exp_rep={3,(struct _dyneither_ptr*)&
Cyc__genname_942,sizeof(struct Cyc_Absyn_Exp),{(void*)((struct _tuple5**)Cyc__genarr_941),(
void*)((struct _tuple5**)Cyc__genarr_941),(void*)((struct _tuple5**)Cyc__genarr_941
+ 4)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_144={1,1,(void*)((void*)&
Cyc_struct_Absyn_Exp_rep)};static char _tmp427[10]="ArrayInfo";static struct
_dyneither_ptr Cyc__genname_1020=(struct _dyneither_ptr){_tmp427,_tmp427,_tmp427 + 
10};static char _tmp428[9]="elt_type";static struct _tuple5 Cyc__gentuple_1014={
offsetof(struct Cyc_Absyn_ArrayInfo,elt_type),(struct _dyneither_ptr){_tmp428,
_tmp428,_tmp428 + 9},(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp429[3]="tq";
static struct _tuple5 Cyc__gentuple_1015={offsetof(struct Cyc_Absyn_ArrayInfo,tq),(
struct _dyneither_ptr){_tmp429,_tmp429,_tmp429 + 3},(void*)& Cyc_Absyn_tqual_t_rep};
static char _tmp42A[9]="num_elts";static struct _tuple5 Cyc__gentuple_1016={offsetof(
struct Cyc_Absyn_ArrayInfo,num_elts),(struct _dyneither_ptr){_tmp42A,_tmp42A,
_tmp42A + 9},(void*)& Cyc__genrep_144};static char _tmp42B[10]="zero_term";static
struct _tuple5 Cyc__gentuple_1017={offsetof(struct Cyc_Absyn_ArrayInfo,zero_term),(
struct _dyneither_ptr){_tmp42B,_tmp42B,_tmp42B + 10},(void*)& Cyc__genrep_1001};
static char _tmp42C[7]="zt_loc";static struct _tuple5 Cyc__gentuple_1018={offsetof(
struct Cyc_Absyn_ArrayInfo,zt_loc),(struct _dyneither_ptr){_tmp42C,_tmp42C,_tmp42C
+ 7},(void*)& Cyc__genrep_2};static struct _tuple5*Cyc__genarr_1019[5]={& Cyc__gentuple_1014,&
Cyc__gentuple_1015,& Cyc__gentuple_1016,& Cyc__gentuple_1017,& Cyc__gentuple_1018};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_array_info_t_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_1020,sizeof(struct Cyc_Absyn_ArrayInfo),{(void*)((
struct _tuple5**)Cyc__genarr_1019),(void*)((struct _tuple5**)Cyc__genarr_1019),(
void*)((struct _tuple5**)Cyc__genarr_1019 + 5)}};struct _tuple85{unsigned int f1;
struct Cyc_Absyn_ArrayInfo f2;};static struct _tuple6 Cyc__gentuple_1021={offsetof(
struct _tuple85,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1022={
offsetof(struct _tuple85,f2),(void*)& Cyc_Absyn_array_info_t_rep};static struct
_tuple6*Cyc__genarr_1023[2]={& Cyc__gentuple_1021,& Cyc__gentuple_1022};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1000={4,sizeof(struct _tuple85),{(void*)((
struct _tuple6**)Cyc__genarr_1023),(void*)((struct _tuple6**)Cyc__genarr_1023),(
void*)((struct _tuple6**)Cyc__genarr_1023 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_981;extern struct Cyc_Typerep_Struct_struct Cyc_Absyn_fn_info_t_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_982;extern struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep;
static char _tmp42F[5]="List";static struct _dyneither_ptr Cyc__genname_986=(struct
_dyneither_ptr){_tmp42F,_tmp42F,_tmp42F + 5};static char _tmp430[3]="hd";static
struct _tuple5 Cyc__gentuple_983={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp430,_tmp430,_tmp430 + 3},(void*)& Cyc__genrep_672};static char
_tmp431[3]="tl";static struct _tuple5 Cyc__gentuple_984={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp431,_tmp431,_tmp431 + 3},(void*)& Cyc__genrep_982};
static struct _tuple5*Cyc__genarr_985[2]={& Cyc__gentuple_983,& Cyc__gentuple_984};
struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_986,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_985),(void*)((struct _tuple5**)Cyc__genarr_985),(void*)((
struct _tuple5**)Cyc__genarr_985 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_982={
1,1,(void*)((void*)& Cyc_struct_List_List060Core_opt_t0Absyn_var_t46H24Absyn_tqual_t4Absyn_type_t1_44099_6H2_rep)};
static char _tmp434[7]="FnInfo";static struct _dyneither_ptr Cyc__genname_996=(struct
_dyneither_ptr){_tmp434,_tmp434,_tmp434 + 7};static char _tmp435[6]="tvars";static
struct _tuple5 Cyc__gentuple_987={offsetof(struct Cyc_Absyn_FnInfo,tvars),(struct
_dyneither_ptr){_tmp435,_tmp435,_tmp435 + 6},(void*)& Cyc__genrep_326};static char
_tmp436[7]="effect";static struct _tuple5 Cyc__gentuple_988={offsetof(struct Cyc_Absyn_FnInfo,effect),(
struct _dyneither_ptr){_tmp436,_tmp436,_tmp436 + 7},(void*)& Cyc__genrep_110};
static char _tmp437[8]="ret_typ";static struct _tuple5 Cyc__gentuple_989={offsetof(
struct Cyc_Absyn_FnInfo,ret_typ),(struct _dyneither_ptr){_tmp437,_tmp437,_tmp437 + 
8},(void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp438[5]="args";static
struct _tuple5 Cyc__gentuple_990={offsetof(struct Cyc_Absyn_FnInfo,args),(struct
_dyneither_ptr){_tmp438,_tmp438,_tmp438 + 5},(void*)& Cyc__genrep_982};static char
_tmp439[10]="c_varargs";static struct _tuple5 Cyc__gentuple_991={offsetof(struct Cyc_Absyn_FnInfo,c_varargs),(
struct _dyneither_ptr){_tmp439,_tmp439,_tmp439 + 10},(void*)& Cyc__genrep_10};
static char _tmp43A[12]="cyc_varargs";static struct _tuple5 Cyc__gentuple_992={
offsetof(struct Cyc_Absyn_FnInfo,cyc_varargs),(struct _dyneither_ptr){_tmp43A,
_tmp43A,_tmp43A + 12},(void*)& Cyc__genrep_824};static char _tmp43B[7]="rgn_po";
static struct _tuple5 Cyc__gentuple_993={offsetof(struct Cyc_Absyn_FnInfo,rgn_po),(
struct _dyneither_ptr){_tmp43B,_tmp43B,_tmp43B + 7},(void*)& Cyc__genrep_387};
static char _tmp43C[11]="attributes";static struct _tuple5 Cyc__gentuple_994={
offsetof(struct Cyc_Absyn_FnInfo,attributes),(struct _dyneither_ptr){_tmp43C,
_tmp43C,_tmp43C + 11},(void*)& Cyc__genrep_62};static struct _tuple5*Cyc__genarr_995[
8]={& Cyc__gentuple_987,& Cyc__gentuple_988,& Cyc__gentuple_989,& Cyc__gentuple_990,&
Cyc__gentuple_991,& Cyc__gentuple_992,& Cyc__gentuple_993,& Cyc__gentuple_994};
struct Cyc_Typerep_Struct_struct Cyc_Absyn_fn_info_t_rep={3,(struct _dyneither_ptr*)&
Cyc__genname_996,sizeof(struct Cyc_Absyn_FnInfo),{(void*)((struct _tuple5**)Cyc__genarr_995),(
void*)((struct _tuple5**)Cyc__genarr_995),(void*)((struct _tuple5**)Cyc__genarr_995
+ 8)}};struct _tuple86{unsigned int f1;struct Cyc_Absyn_FnInfo f2;};static struct
_tuple6 Cyc__gentuple_997={offsetof(struct _tuple86,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_998={offsetof(struct _tuple86,f2),(void*)& Cyc_Absyn_fn_info_t_rep};
static struct _tuple6*Cyc__genarr_999[2]={& Cyc__gentuple_997,& Cyc__gentuple_998};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_981={4,sizeof(struct _tuple86),{(
void*)((struct _tuple6**)Cyc__genarr_999),(void*)((struct _tuple6**)Cyc__genarr_999),(
void*)((struct _tuple6**)Cyc__genarr_999 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_977;static struct _tuple6 Cyc__gentuple_978={offsetof(struct _tuple14,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_979={offsetof(struct
_tuple14,f2),(void*)& Cyc__genrep_300};static struct _tuple6*Cyc__genarr_980[2]={&
Cyc__gentuple_978,& Cyc__gentuple_979};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_977={
4,sizeof(struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_980),(void*)((
struct _tuple6**)Cyc__genarr_980),(void*)((struct _tuple6**)Cyc__genarr_980 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_973;struct _tuple87{unsigned int
f1;struct Cyc_Absyn_AggrInfo f2;};static struct _tuple6 Cyc__gentuple_974={offsetof(
struct _tuple87,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_975={
offsetof(struct _tuple87,f2),(void*)& Cyc_Absyn_aggr_info_t_rep};static struct
_tuple6*Cyc__genarr_976[2]={& Cyc__gentuple_974,& Cyc__gentuple_975};static struct
Cyc_Typerep_Tuple_struct Cyc__genrep_973={4,sizeof(struct _tuple87),{(void*)((
struct _tuple6**)Cyc__genarr_976),(void*)((struct _tuple6**)Cyc__genarr_976),(void*)((
struct _tuple6**)Cyc__genarr_976 + 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_968;
static struct _tuple6 Cyc__gentuple_969={offsetof(struct _tuple67,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_970={offsetof(struct _tuple67,f2),(void*)& Cyc_Absyn_aggr_kind_t_rep};
static struct _tuple6 Cyc__gentuple_971={offsetof(struct _tuple67,f3),(void*)& Cyc__genrep_374};
static struct _tuple6*Cyc__genarr_972[3]={& Cyc__gentuple_969,& Cyc__gentuple_970,&
Cyc__gentuple_971};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_968={4,
sizeof(struct _tuple67),{(void*)((struct _tuple6**)Cyc__genarr_972),(void*)((
struct _tuple6**)Cyc__genarr_972),(void*)((struct _tuple6**)Cyc__genarr_972 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_963;struct _tuple88{unsigned int
f1;struct _tuple0*f2;struct Cyc_Absyn_Enumdecl*f3;};static struct _tuple6 Cyc__gentuple_964={
offsetof(struct _tuple88,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_965={
offsetof(struct _tuple88,f2),(void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_966={
offsetof(struct _tuple88,f3),(void*)& Cyc__genrep_635};static struct _tuple6*Cyc__genarr_967[
3]={& Cyc__gentuple_964,& Cyc__gentuple_965,& Cyc__gentuple_966};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_963={4,sizeof(struct _tuple88),{(void*)((struct _tuple6**)Cyc__genarr_967),(
void*)((struct _tuple6**)Cyc__genarr_967),(void*)((struct _tuple6**)Cyc__genarr_967
+ 3)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_959;static struct _tuple6
Cyc__gentuple_960={offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};static
struct _tuple6 Cyc__gentuple_961={offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_267};
static struct _tuple6*Cyc__genarr_962[2]={& Cyc__gentuple_960,& Cyc__gentuple_961};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_959={4,sizeof(struct _tuple14),{(
void*)((struct _tuple6**)Cyc__genarr_962),(void*)((struct _tuple6**)Cyc__genarr_962),(
void*)((struct _tuple6**)Cyc__genarr_962 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_954;static struct _tuple6 Cyc__gentuple_955={offsetof(struct _tuple28,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_956={offsetof(struct
_tuple28,f2),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct _tuple6 Cyc__gentuple_957={
offsetof(struct _tuple28,f3),(void*)((void*)& Cyc_Absyn_type_t_rep)};static struct
_tuple6*Cyc__genarr_958[3]={& Cyc__gentuple_955,& Cyc__gentuple_956,& Cyc__gentuple_957};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_954={4,sizeof(struct _tuple28),{(
void*)((struct _tuple6**)Cyc__genarr_958),(void*)((struct _tuple6**)Cyc__genarr_958),(
void*)((struct _tuple6**)Cyc__genarr_958 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_946;extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_947;extern
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Typedefdecl_rep;static char
_tmp445[12]="Typedefdecl";static struct _dyneither_ptr Cyc__genname_1167=(struct
_dyneither_ptr){_tmp445,_tmp445,_tmp445 + 12};static char _tmp446[5]="name";static
struct _tuple5 Cyc__gentuple_1160={offsetof(struct Cyc_Absyn_Typedefdecl,name),(
struct _dyneither_ptr){_tmp446,_tmp446,_tmp446 + 5},(void*)& Cyc__genrep_11};static
char _tmp447[3]="tq";static struct _tuple5 Cyc__gentuple_1161={offsetof(struct Cyc_Absyn_Typedefdecl,tq),(
struct _dyneither_ptr){_tmp447,_tmp447,_tmp447 + 3},(void*)& Cyc_Absyn_tqual_t_rep};
static char _tmp448[4]="tvs";static struct _tuple5 Cyc__gentuple_1162={offsetof(
struct Cyc_Absyn_Typedefdecl,tvs),(struct _dyneither_ptr){_tmp448,_tmp448,_tmp448 + 
4},(void*)& Cyc__genrep_326};static char _tmp449[5]="kind";static struct _tuple5 Cyc__gentuple_1163={
offsetof(struct Cyc_Absyn_Typedefdecl,kind),(struct _dyneither_ptr){_tmp449,
_tmp449,_tmp449 + 5},(void*)& Cyc__genrep_1124};static char _tmp44A[5]="defn";static
struct _tuple5 Cyc__gentuple_1164={offsetof(struct Cyc_Absyn_Typedefdecl,defn),(
struct _dyneither_ptr){_tmp44A,_tmp44A,_tmp44A + 5},(void*)& Cyc__genrep_110};
static char _tmp44B[5]="atts";static struct _tuple5 Cyc__gentuple_1165={offsetof(
struct Cyc_Absyn_Typedefdecl,atts),(struct _dyneither_ptr){_tmp44B,_tmp44B,_tmp44B
+ 5},(void*)& Cyc__genrep_62};static struct _tuple5*Cyc__genarr_1166[6]={& Cyc__gentuple_1160,&
Cyc__gentuple_1161,& Cyc__gentuple_1162,& Cyc__gentuple_1163,& Cyc__gentuple_1164,&
Cyc__gentuple_1165};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Typedefdecl_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_1167,sizeof(struct Cyc_Absyn_Typedefdecl),{(
void*)((struct _tuple5**)Cyc__genarr_1166),(void*)((struct _tuple5**)Cyc__genarr_1166),(
void*)((struct _tuple5**)Cyc__genarr_1166 + 6)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_947={1,1,(void*)((void*)& Cyc_struct_Absyn_Typedefdecl_rep)};struct
_tuple89{unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Typedefdecl*
f4;void**f5;};static struct _tuple6 Cyc__gentuple_948={offsetof(struct _tuple89,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_949={offsetof(struct
_tuple89,f2),(void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_950={
offsetof(struct _tuple89,f3),(void*)& Cyc__genrep_121};static struct _tuple6 Cyc__gentuple_951={
offsetof(struct _tuple89,f4),(void*)& Cyc__genrep_947};static struct _tuple6 Cyc__gentuple_952={
offsetof(struct _tuple89,f5),(void*)& Cyc__genrep_616};static struct _tuple6*Cyc__genarr_953[
5]={& Cyc__gentuple_948,& Cyc__gentuple_949,& Cyc__gentuple_950,& Cyc__gentuple_951,&
Cyc__gentuple_952};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_946={4,
sizeof(struct _tuple89),{(void*)((struct _tuple6**)Cyc__genarr_953),(void*)((
struct _tuple6**)Cyc__genarr_953),(void*)((struct _tuple6**)Cyc__genarr_953 + 5)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_120;static struct _tuple6 Cyc__gentuple_126={
offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_127={
offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_121};static struct _tuple6*Cyc__genarr_128[
2]={& Cyc__gentuple_126,& Cyc__gentuple_127};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_120={4,sizeof(struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_128),(
void*)((struct _tuple6**)Cyc__genarr_128),(void*)((struct _tuple6**)Cyc__genarr_128
+ 2)}};static char _tmp450[9]="VoidType";static struct _tuple7 Cyc__gentuple_111={0,(
struct _dyneither_ptr){_tmp450,_tmp450,_tmp450 + 9}};static char _tmp451[10]="FloatType";
static struct _tuple7 Cyc__gentuple_112={1,(struct _dyneither_ptr){_tmp451,_tmp451,
_tmp451 + 10}};static char _tmp452[8]="HeapRgn";static struct _tuple7 Cyc__gentuple_113={
2,(struct _dyneither_ptr){_tmp452,_tmp452,_tmp452 + 8}};static char _tmp453[10]="UniqueRgn";
static struct _tuple7 Cyc__gentuple_114={3,(struct _dyneither_ptr){_tmp453,_tmp453,
_tmp453 + 10}};static struct _tuple7*Cyc__genarr_115[4]={& Cyc__gentuple_111,& Cyc__gentuple_112,&
Cyc__gentuple_113,& Cyc__gentuple_114};static char _tmp454[5]="Evar";static struct
_tuple5 Cyc__gentuple_1134={0,(struct _dyneither_ptr){_tmp454,_tmp454,_tmp454 + 5},(
void*)& Cyc__genrep_1119};static char _tmp455[8]="VarType";static struct _tuple5 Cyc__gentuple_1135={
1,(struct _dyneither_ptr){_tmp455,_tmp455,_tmp455 + 8},(void*)& Cyc__genrep_1115};
static char _tmp456[11]="TunionType";static struct _tuple5 Cyc__gentuple_1136={2,(
struct _dyneither_ptr){_tmp456,_tmp456,_tmp456 + 11},(void*)& Cyc__genrep_1088};
static char _tmp457[16]="TunionFieldType";static struct _tuple5 Cyc__gentuple_1137={3,(
struct _dyneither_ptr){_tmp457,_tmp457,_tmp457 + 16},(void*)& Cyc__genrep_1062};
static char _tmp458[12]="PointerType";static struct _tuple5 Cyc__gentuple_1138={4,(
struct _dyneither_ptr){_tmp458,_tmp458,_tmp458 + 12},(void*)& Cyc__genrep_1036};
static char _tmp459[8]="IntType";static struct _tuple5 Cyc__gentuple_1139={5,(struct
_dyneither_ptr){_tmp459,_tmp459,_tmp459 + 8},(void*)& Cyc__genrep_1024};static char
_tmp45A[11]="DoubleType";static struct _tuple5 Cyc__gentuple_1140={6,(struct
_dyneither_ptr){_tmp45A,_tmp45A,_tmp45A + 11},(void*)& Cyc__genrep_85};static char
_tmp45B[10]="ArrayType";static struct _tuple5 Cyc__gentuple_1141={7,(struct
_dyneither_ptr){_tmp45B,_tmp45B,_tmp45B + 10},(void*)& Cyc__genrep_1000};static
char _tmp45C[7]="FnType";static struct _tuple5 Cyc__gentuple_1142={8,(struct
_dyneither_ptr){_tmp45C,_tmp45C,_tmp45C + 7},(void*)& Cyc__genrep_981};static char
_tmp45D[10]="TupleType";static struct _tuple5 Cyc__gentuple_1143={9,(struct
_dyneither_ptr){_tmp45D,_tmp45D,_tmp45D + 10},(void*)& Cyc__genrep_977};static char
_tmp45E[9]="AggrType";static struct _tuple5 Cyc__gentuple_1144={10,(struct
_dyneither_ptr){_tmp45E,_tmp45E,_tmp45E + 9},(void*)& Cyc__genrep_973};static char
_tmp45F[13]="AnonAggrType";static struct _tuple5 Cyc__gentuple_1145={11,(struct
_dyneither_ptr){_tmp45F,_tmp45F,_tmp45F + 13},(void*)& Cyc__genrep_968};static char
_tmp460[9]="EnumType";static struct _tuple5 Cyc__gentuple_1146={12,(struct
_dyneither_ptr){_tmp460,_tmp460,_tmp460 + 9},(void*)& Cyc__genrep_963};static char
_tmp461[13]="AnonEnumType";static struct _tuple5 Cyc__gentuple_1147={13,(struct
_dyneither_ptr){_tmp461,_tmp461,_tmp461 + 13},(void*)& Cyc__genrep_959};static char
_tmp462[14]="RgnHandleType";static struct _tuple5 Cyc__gentuple_1148={14,(struct
_dyneither_ptr){_tmp462,_tmp462,_tmp462 + 14},(void*)& Cyc__genrep_116};static char
_tmp463[11]="DynRgnType";static struct _tuple5 Cyc__gentuple_1149={15,(struct
_dyneither_ptr){_tmp463,_tmp463,_tmp463 + 11},(void*)& Cyc__genrep_954};static char
_tmp464[12]="TypedefType";static struct _tuple5 Cyc__gentuple_1150={16,(struct
_dyneither_ptr){_tmp464,_tmp464,_tmp464 + 12},(void*)& Cyc__genrep_946};static char
_tmp465[12]="ValueofType";static struct _tuple5 Cyc__gentuple_1151={17,(struct
_dyneither_ptr){_tmp465,_tmp465,_tmp465 + 12},(void*)& Cyc__genrep_129};static char
_tmp466[8]="TagType";static struct _tuple5 Cyc__gentuple_1152={18,(struct
_dyneither_ptr){_tmp466,_tmp466,_tmp466 + 8},(void*)& Cyc__genrep_116};static char
_tmp467[10]="AccessEff";static struct _tuple5 Cyc__gentuple_1153={19,(struct
_dyneither_ptr){_tmp467,_tmp467,_tmp467 + 10},(void*)& Cyc__genrep_116};static char
_tmp468[8]="JoinEff";static struct _tuple5 Cyc__gentuple_1154={20,(struct
_dyneither_ptr){_tmp468,_tmp468,_tmp468 + 8},(void*)& Cyc__genrep_120};static char
_tmp469[8]="RgnsEff";static struct _tuple5 Cyc__gentuple_1155={21,(struct
_dyneither_ptr){_tmp469,_tmp469,_tmp469 + 8},(void*)& Cyc__genrep_116};static
struct _tuple5*Cyc__genarr_1156[22]={& Cyc__gentuple_1134,& Cyc__gentuple_1135,& Cyc__gentuple_1136,&
Cyc__gentuple_1137,& Cyc__gentuple_1138,& Cyc__gentuple_1139,& Cyc__gentuple_1140,&
Cyc__gentuple_1141,& Cyc__gentuple_1142,& Cyc__gentuple_1143,& Cyc__gentuple_1144,&
Cyc__gentuple_1145,& Cyc__gentuple_1146,& Cyc__gentuple_1147,& Cyc__gentuple_1148,&
Cyc__gentuple_1149,& Cyc__gentuple_1150,& Cyc__gentuple_1151,& Cyc__gentuple_1152,&
Cyc__gentuple_1153,& Cyc__gentuple_1154,& Cyc__gentuple_1155};static char _tmp46B[5]="Type";
struct Cyc_Typerep_TUnion_struct Cyc_Absyn_type_t_rep={5,(struct _dyneither_ptr){
_tmp46B,_tmp46B,_tmp46B + 5},{(void*)((struct _tuple7**)Cyc__genarr_115),(void*)((
struct _tuple7**)Cyc__genarr_115),(void*)((struct _tuple7**)Cyc__genarr_115 + 4)},{(
void*)((struct _tuple5**)Cyc__genarr_1156),(void*)((struct _tuple5**)Cyc__genarr_1156),(
void*)((struct _tuple5**)Cyc__genarr_1156 + 22)}};static char _tmp46C[8]="Vardecl";
static struct _dyneither_ptr Cyc__genname_169=(struct _dyneither_ptr){_tmp46C,
_tmp46C,_tmp46C + 8};static char _tmp46D[3]="sc";static struct _tuple5 Cyc__gentuple_160={
offsetof(struct Cyc_Absyn_Vardecl,sc),(struct _dyneither_ptr){_tmp46D,_tmp46D,
_tmp46D + 3},(void*)& Cyc_Absyn_scope_t_rep};static char _tmp46E[5]="name";static
struct _tuple5 Cyc__gentuple_161={offsetof(struct Cyc_Absyn_Vardecl,name),(struct
_dyneither_ptr){_tmp46E,_tmp46E,_tmp46E + 5},(void*)& Cyc__genrep_11};static char
_tmp46F[3]="tq";static struct _tuple5 Cyc__gentuple_162={offsetof(struct Cyc_Absyn_Vardecl,tq),(
struct _dyneither_ptr){_tmp46F,_tmp46F,_tmp46F + 3},(void*)& Cyc_Absyn_tqual_t_rep};
static char _tmp470[5]="type";static struct _tuple5 Cyc__gentuple_163={offsetof(
struct Cyc_Absyn_Vardecl,type),(struct _dyneither_ptr){_tmp470,_tmp470,_tmp470 + 5},(
void*)((void*)& Cyc_Absyn_type_t_rep)};static char _tmp471[12]="initializer";static
struct _tuple5 Cyc__gentuple_164={offsetof(struct Cyc_Absyn_Vardecl,initializer),(
struct _dyneither_ptr){_tmp471,_tmp471,_tmp471 + 12},(void*)& Cyc__genrep_144};
static char _tmp472[4]="rgn";static struct _tuple5 Cyc__gentuple_165={offsetof(struct
Cyc_Absyn_Vardecl,rgn),(struct _dyneither_ptr){_tmp472,_tmp472,_tmp472 + 4},(void*)&
Cyc__genrep_110};static char _tmp473[11]="attributes";static struct _tuple5 Cyc__gentuple_166={
offsetof(struct Cyc_Absyn_Vardecl,attributes),(struct _dyneither_ptr){_tmp473,
_tmp473,_tmp473 + 11},(void*)& Cyc__genrep_62};static char _tmp474[8]="escapes";
static struct _tuple5 Cyc__gentuple_167={offsetof(struct Cyc_Absyn_Vardecl,escapes),(
struct _dyneither_ptr){_tmp474,_tmp474,_tmp474 + 8},(void*)& Cyc__genrep_10};static
struct _tuple5*Cyc__genarr_168[8]={& Cyc__gentuple_160,& Cyc__gentuple_161,& Cyc__gentuple_162,&
Cyc__gentuple_163,& Cyc__gentuple_164,& Cyc__gentuple_165,& Cyc__gentuple_166,& Cyc__gentuple_167};
struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Vardecl_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_169,sizeof(struct Cyc_Absyn_Vardecl),{(void*)((
struct _tuple5**)Cyc__genarr_168),(void*)((struct _tuple5**)Cyc__genarr_168),(void*)((
struct _tuple5**)Cyc__genarr_168 + 8)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_143={
1,1,(void*)((void*)& Cyc_struct_Absyn_Vardecl_rep)};struct _tuple90{unsigned int f1;
struct Cyc_Absyn_Vardecl*f2;};static struct _tuple6 Cyc__gentuple_818={offsetof(
struct _tuple90,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_819={
offsetof(struct _tuple90,f2),(void*)& Cyc__genrep_143};static struct _tuple6*Cyc__genarr_820[
2]={& Cyc__gentuple_818,& Cyc__gentuple_819};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_817={4,sizeof(struct _tuple90),{(void*)((struct _tuple6**)Cyc__genarr_820),(
void*)((struct _tuple6**)Cyc__genarr_820),(void*)((struct _tuple6**)Cyc__genarr_820
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1187;struct _tuple91{
unsigned int f1;struct Cyc_Absyn_Pat*f2;struct Cyc_Core_Opt*f3;struct Cyc_Absyn_Exp*
f4;};static struct _tuple6 Cyc__gentuple_1188={offsetof(struct _tuple91,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1189={offsetof(struct _tuple91,f2),(
void*)& Cyc__genrep_234};static struct _tuple6 Cyc__gentuple_1190={offsetof(struct
_tuple91,f3),(void*)& Cyc__genrep_225};static struct _tuple6 Cyc__gentuple_1191={
offsetof(struct _tuple91,f4),(void*)& Cyc__genrep_130};static struct _tuple6*Cyc__genarr_1192[
4]={& Cyc__gentuple_1188,& Cyc__gentuple_1189,& Cyc__gentuple_1190,& Cyc__gentuple_1191};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1187={4,sizeof(struct _tuple91),{(
void*)((struct _tuple6**)Cyc__genarr_1192),(void*)((struct _tuple6**)Cyc__genarr_1192),(
void*)((struct _tuple6**)Cyc__genarr_1192 + 4)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1183;static struct _tuple6 Cyc__gentuple_1184={offsetof(struct _tuple14,f1),(
void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1185={offsetof(struct
_tuple14,f2),(void*)& Cyc__genrep_226};static struct _tuple6*Cyc__genarr_1186[2]={&
Cyc__gentuple_1184,& Cyc__gentuple_1185};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1183={
4,sizeof(struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_1186),(void*)((
struct _tuple6**)Cyc__genarr_1186),(void*)((struct _tuple6**)Cyc__genarr_1186 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1179;struct _tuple92{unsigned int
f1;struct Cyc_Absyn_Aggrdecl*f2;};static struct _tuple6 Cyc__gentuple_1180={
offsetof(struct _tuple92,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1181={
offsetof(struct _tuple92,f2),(void*)((void*)& Cyc__genrep_372)};static struct
_tuple6*Cyc__genarr_1182[2]={& Cyc__gentuple_1180,& Cyc__gentuple_1181};static
struct Cyc_Typerep_Tuple_struct Cyc__genrep_1179={4,sizeof(struct _tuple92),{(void*)((
struct _tuple6**)Cyc__genarr_1182),(void*)((struct _tuple6**)Cyc__genarr_1182),(
void*)((struct _tuple6**)Cyc__genarr_1182 + 2)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1175;struct _tuple93{unsigned int f1;struct Cyc_Absyn_Tuniondecl*f2;};
static struct _tuple6 Cyc__gentuple_1176={offsetof(struct _tuple93,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_1177={offsetof(struct _tuple93,f2),(void*)((void*)&
Cyc__genrep_316)};static struct _tuple6*Cyc__genarr_1178[2]={& Cyc__gentuple_1176,&
Cyc__gentuple_1177};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_1175={4,
sizeof(struct _tuple93),{(void*)((struct _tuple6**)Cyc__genarr_1178),(void*)((
struct _tuple6**)Cyc__genarr_1178),(void*)((struct _tuple6**)Cyc__genarr_1178 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_1171;struct _tuple94{unsigned int
f1;struct Cyc_Absyn_Enumdecl*f2;};static struct _tuple6 Cyc__gentuple_1172={
offsetof(struct _tuple94,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1173={
offsetof(struct _tuple94,f2),(void*)& Cyc__genrep_265};static struct _tuple6*Cyc__genarr_1174[
2]={& Cyc__gentuple_1172,& Cyc__gentuple_1173};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_1171={4,sizeof(struct _tuple94),{(void*)((struct _tuple6**)Cyc__genarr_1174),(
void*)((struct _tuple6**)Cyc__genarr_1174),(void*)((struct _tuple6**)Cyc__genarr_1174
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_60;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_61;static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_61={1,1,(void*)((
void*)& Cyc_struct_Absyn_Typedefdecl_rep)};struct _tuple95{unsigned int f1;struct
Cyc_Absyn_Typedefdecl*f2;};static struct _tuple6 Cyc__gentuple_1168={offsetof(
struct _tuple95,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_1169={
offsetof(struct _tuple95,f2),(void*)& Cyc__genrep_61};static struct _tuple6*Cyc__genarr_1170[
2]={& Cyc__gentuple_1168,& Cyc__gentuple_1169};static struct Cyc_Typerep_Tuple_struct
Cyc__genrep_60={4,sizeof(struct _tuple95),{(void*)((struct _tuple6**)Cyc__genarr_1170),(
void*)((struct _tuple6**)Cyc__genarr_1170),(void*)((struct _tuple6**)Cyc__genarr_1170
+ 2)}};extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_55;struct _tuple96{
unsigned int f1;struct _dyneither_ptr*f2;struct Cyc_List_List*f3;};static struct
_tuple6 Cyc__gentuple_56={offsetof(struct _tuple96,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_57={offsetof(struct _tuple96,f2),(void*)& Cyc__genrep_13};
static struct _tuple6 Cyc__gentuple_58={offsetof(struct _tuple96,f3),(void*)& Cyc__genrep_0};
static struct _tuple6*Cyc__genarr_59[3]={& Cyc__gentuple_56,& Cyc__gentuple_57,& Cyc__gentuple_58};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_55={4,sizeof(struct _tuple96),{(
void*)((struct _tuple6**)Cyc__genarr_59),(void*)((struct _tuple6**)Cyc__genarr_59),(
void*)((struct _tuple6**)Cyc__genarr_59 + 3)}};extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_50;struct _tuple97{unsigned int f1;struct _tuple0*f2;struct Cyc_List_List*
f3;};static struct _tuple6 Cyc__gentuple_51={offsetof(struct _tuple97,f1),(void*)&
Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_52={offsetof(struct _tuple97,f2),(
void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_53={offsetof(struct
_tuple97,f3),(void*)& Cyc__genrep_0};static struct _tuple6*Cyc__genarr_54[3]={& Cyc__gentuple_51,&
Cyc__gentuple_52,& Cyc__gentuple_53};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_50={
4,sizeof(struct _tuple97),{(void*)((struct _tuple6**)Cyc__genarr_54),(void*)((
struct _tuple6**)Cyc__genarr_54),(void*)((struct _tuple6**)Cyc__genarr_54 + 3)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_46;static struct _tuple6 Cyc__gentuple_47={
offsetof(struct _tuple14,f1),(void*)& Cyc__genrep_24};static struct _tuple6 Cyc__gentuple_48={
offsetof(struct _tuple14,f2),(void*)& Cyc__genrep_0};static struct _tuple6*Cyc__genarr_49[
2]={& Cyc__gentuple_47,& Cyc__gentuple_48};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_46={
4,sizeof(struct _tuple14),{(void*)((struct _tuple6**)Cyc__genarr_49),(void*)((
struct _tuple6**)Cyc__genarr_49),(void*)((struct _tuple6**)Cyc__genarr_49 + 2)}};
extern struct Cyc_Typerep_Tuple_struct Cyc__genrep_6;extern struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_7;extern struct Cyc_Typerep_Struct_struct Cyc_struct_List_List060Position_seg_t4Absyn_qvar_t4bool1_446H2_rep;
extern struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_8;extern struct Cyc_Typerep_Tuple_struct
Cyc__genrep_9;struct _tuple98{struct Cyc_Position_Segment*f1;struct _tuple0*f2;int
f3;};static struct _tuple6 Cyc__gentuple_34={offsetof(struct _tuple98,f1),(void*)&
Cyc__genrep_2};static struct _tuple6 Cyc__gentuple_35={offsetof(struct _tuple98,f2),(
void*)& Cyc__genrep_11};static struct _tuple6 Cyc__gentuple_36={offsetof(struct
_tuple98,f3),(void*)& Cyc__genrep_10};static struct _tuple6*Cyc__genarr_37[3]={& Cyc__gentuple_34,&
Cyc__gentuple_35,& Cyc__gentuple_36};static struct Cyc_Typerep_Tuple_struct Cyc__genrep_9={
4,sizeof(struct _tuple98),{(void*)((struct _tuple6**)Cyc__genarr_37),(void*)((
struct _tuple6**)Cyc__genarr_37),(void*)((struct _tuple6**)Cyc__genarr_37 + 3)}};
static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_8={1,1,(void*)((void*)& Cyc__genrep_9)};
static char _tmp484[5]="List";static struct _dyneither_ptr Cyc__genname_41=(struct
_dyneither_ptr){_tmp484,_tmp484,_tmp484 + 5};static char _tmp485[3]="hd";static
struct _tuple5 Cyc__gentuple_38={offsetof(struct Cyc_List_List,hd),(struct
_dyneither_ptr){_tmp485,_tmp485,_tmp485 + 3},(void*)& Cyc__genrep_8};static char
_tmp486[3]="tl";static struct _tuple5 Cyc__gentuple_39={offsetof(struct Cyc_List_List,tl),(
struct _dyneither_ptr){_tmp486,_tmp486,_tmp486 + 3},(void*)& Cyc__genrep_7};static
struct _tuple5*Cyc__genarr_40[2]={& Cyc__gentuple_38,& Cyc__gentuple_39};struct Cyc_Typerep_Struct_struct
Cyc_struct_List_List060Position_seg_t4Absyn_qvar_t4bool1_446H2_rep={3,(struct
_dyneither_ptr*)& Cyc__genname_41,sizeof(struct Cyc_List_List),{(void*)((struct
_tuple5**)Cyc__genarr_40),(void*)((struct _tuple5**)Cyc__genarr_40),(void*)((
struct _tuple5**)Cyc__genarr_40 + 2)}};static struct Cyc_Typerep_ThinPtr_struct Cyc__genrep_7={
1,1,(void*)((void*)& Cyc_struct_List_List060Position_seg_t4Absyn_qvar_t4bool1_446H2_rep)};
struct _tuple99{unsigned int f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
static struct _tuple6 Cyc__gentuple_42={offsetof(struct _tuple99,f1),(void*)& Cyc__genrep_24};
static struct _tuple6 Cyc__gentuple_43={offsetof(struct _tuple99,f2),(void*)& Cyc__genrep_0};
static struct _tuple6 Cyc__gentuple_44={offsetof(struct _tuple99,f3),(void*)& Cyc__genrep_7};
static struct _tuple6*Cyc__genarr_45[3]={& Cyc__gentuple_42,& Cyc__gentuple_43,& Cyc__gentuple_44};
static struct Cyc_Typerep_Tuple_struct Cyc__genrep_6={4,sizeof(struct _tuple99),{(
void*)((struct _tuple6**)Cyc__genarr_45),(void*)((struct _tuple6**)Cyc__genarr_45),(
void*)((struct _tuple6**)Cyc__genarr_45 + 3)}};static char _tmp48A[9]="Porton_d";
static struct _tuple7 Cyc__gentuple_3={0,(struct _dyneither_ptr){_tmp48A,_tmp48A,
_tmp48A + 9}};static char _tmp48B[10]="Portoff_d";static struct _tuple7 Cyc__gentuple_4={
1,(struct _dyneither_ptr){_tmp48B,_tmp48B,_tmp48B + 10}};static struct _tuple7*Cyc__genarr_5[
2]={& Cyc__gentuple_3,& Cyc__gentuple_4};static char _tmp48C[6]="Var_d";static struct
_tuple5 Cyc__gentuple_1193={0,(struct _dyneither_ptr){_tmp48C,_tmp48C,_tmp48C + 6},(
void*)& Cyc__genrep_817};static char _tmp48D[5]="Fn_d";static struct _tuple5 Cyc__gentuple_1194={
1,(struct _dyneither_ptr){_tmp48D,_tmp48D,_tmp48D + 5},(void*)& Cyc__genrep_821};
static char _tmp48E[6]="Let_d";static struct _tuple5 Cyc__gentuple_1195={2,(struct
_dyneither_ptr){_tmp48E,_tmp48E,_tmp48E + 6},(void*)& Cyc__genrep_1187};static char
_tmp48F[7]="Letv_d";static struct _tuple5 Cyc__gentuple_1196={3,(struct
_dyneither_ptr){_tmp48F,_tmp48F,_tmp48F + 7},(void*)& Cyc__genrep_1183};static char
_tmp490[7]="Aggr_d";static struct _tuple5 Cyc__gentuple_1197={4,(struct
_dyneither_ptr){_tmp490,_tmp490,_tmp490 + 7},(void*)& Cyc__genrep_1179};static char
_tmp491[9]="Tunion_d";static struct _tuple5 Cyc__gentuple_1198={5,(struct
_dyneither_ptr){_tmp491,_tmp491,_tmp491 + 9},(void*)& Cyc__genrep_1175};static char
_tmp492[7]="Enum_d";static struct _tuple5 Cyc__gentuple_1199={6,(struct
_dyneither_ptr){_tmp492,_tmp492,_tmp492 + 7},(void*)& Cyc__genrep_1171};static char
_tmp493[10]="Typedef_d";static struct _tuple5 Cyc__gentuple_1200={7,(struct
_dyneither_ptr){_tmp493,_tmp493,_tmp493 + 10},(void*)& Cyc__genrep_60};static char
_tmp494[12]="Namespace_d";static struct _tuple5 Cyc__gentuple_1201={8,(struct
_dyneither_ptr){_tmp494,_tmp494,_tmp494 + 12},(void*)& Cyc__genrep_55};static char
_tmp495[8]="Using_d";static struct _tuple5 Cyc__gentuple_1202={9,(struct
_dyneither_ptr){_tmp495,_tmp495,_tmp495 + 8},(void*)& Cyc__genrep_50};static char
_tmp496[10]="ExternC_d";static struct _tuple5 Cyc__gentuple_1203={10,(struct
_dyneither_ptr){_tmp496,_tmp496,_tmp496 + 10},(void*)& Cyc__genrep_46};static char
_tmp497[17]="ExternCinclude_d";static struct _tuple5 Cyc__gentuple_1204={11,(struct
_dyneither_ptr){_tmp497,_tmp497,_tmp497 + 17},(void*)& Cyc__genrep_6};static struct
_tuple5*Cyc__genarr_1205[12]={& Cyc__gentuple_1193,& Cyc__gentuple_1194,& Cyc__gentuple_1195,&
Cyc__gentuple_1196,& Cyc__gentuple_1197,& Cyc__gentuple_1198,& Cyc__gentuple_1199,&
Cyc__gentuple_1200,& Cyc__gentuple_1201,& Cyc__gentuple_1202,& Cyc__gentuple_1203,&
Cyc__gentuple_1204};static char _tmp499[9]="Raw_decl";struct Cyc_Typerep_TUnion_struct
Cyc_Absyn_raw_decl_t_rep={5,(struct _dyneither_ptr){_tmp499,_tmp499,_tmp499 + 9},{(
void*)((struct _tuple7**)Cyc__genarr_5),(void*)((struct _tuple7**)Cyc__genarr_5),(
void*)((struct _tuple7**)Cyc__genarr_5 + 2)},{(void*)((struct _tuple5**)Cyc__genarr_1205),(
void*)((struct _tuple5**)Cyc__genarr_1205),(void*)((struct _tuple5**)Cyc__genarr_1205
+ 12)}};static char _tmp49A[5]="Decl";static struct _dyneither_ptr Cyc__genname_1209=(
struct _dyneither_ptr){_tmp49A,_tmp49A,_tmp49A + 5};static char _tmp49B[2]="r";
static struct _tuple5 Cyc__gentuple_1206={offsetof(struct Cyc_Absyn_Decl,r),(struct
_dyneither_ptr){_tmp49B,_tmp49B,_tmp49B + 2},(void*)& Cyc_Absyn_raw_decl_t_rep};
static char _tmp49C[4]="loc";static struct _tuple5 Cyc__gentuple_1207={offsetof(
struct Cyc_Absyn_Decl,loc),(struct _dyneither_ptr){_tmp49C,_tmp49C,_tmp49C + 4},(
void*)& Cyc__genrep_2};static struct _tuple5*Cyc__genarr_1208[2]={& Cyc__gentuple_1206,&
Cyc__gentuple_1207};struct Cyc_Typerep_Struct_struct Cyc_struct_Absyn_Decl_rep={3,(
struct _dyneither_ptr*)& Cyc__genname_1209,sizeof(struct Cyc_Absyn_Decl),{(void*)((
struct _tuple5**)Cyc__genarr_1208),(void*)((struct _tuple5**)Cyc__genarr_1208),(
void*)((struct _tuple5**)Cyc__genarr_1208 + 2)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_1={1,1,(void*)((void*)& Cyc_struct_Absyn_Decl_rep)};static char _tmp49F[
5]="List";static struct _dyneither_ptr Cyc__genname_1213=(struct _dyneither_ptr){
_tmp49F,_tmp49F,_tmp49F + 5};static char _tmp4A0[3]="hd";static struct _tuple5 Cyc__gentuple_1210={
offsetof(struct Cyc_List_List,hd),(struct _dyneither_ptr){_tmp4A0,_tmp4A0,_tmp4A0 + 
3},(void*)& Cyc__genrep_1};static char _tmp4A1[3]="tl";static struct _tuple5 Cyc__gentuple_1211={
offsetof(struct Cyc_List_List,tl),(struct _dyneither_ptr){_tmp4A1,_tmp4A1,_tmp4A1 + 
3},(void*)& Cyc__genrep_0};static struct _tuple5*Cyc__genarr_1212[2]={& Cyc__gentuple_1210,&
Cyc__gentuple_1211};struct Cyc_Typerep_Struct_struct Cyc_struct_List_List0Absyn_decl_t46H2_rep={
3,(struct _dyneither_ptr*)& Cyc__genname_1213,sizeof(struct Cyc_List_List),{(void*)((
struct _tuple5**)Cyc__genarr_1212),(void*)((struct _tuple5**)Cyc__genarr_1212),(
void*)((struct _tuple5**)Cyc__genarr_1212 + 2)}};static struct Cyc_Typerep_ThinPtr_struct
Cyc__genrep_0={1,1,(void*)((void*)& Cyc_struct_List_List0Absyn_decl_t46H2_rep)};
void*Cyc_decls_rep=(void*)& Cyc__genrep_0;
