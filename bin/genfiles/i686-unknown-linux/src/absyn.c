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
struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{
int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct
_union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace
Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*);struct _tuple0{union
Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Tqual{int
print_const;int q_volatile;int q_restrict;int real_const;struct Cyc_Position_Segment*
loc;};struct _union_Constraint_Eq_constr{int tag;void*val;};struct
_union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct
_union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr
Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_struct{
int tag;void*f1;};struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;
};struct Cyc_Absyn_Less_kb_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;};struct
Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*
ptr_loc;struct Cyc_Position_Segment*rgn_loc;struct Cyc_Position_Segment*zt_loc;};
struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*
bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};
struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts
ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_struct{int tag;struct Cyc_Absyn_Exp*f1;
};struct Cyc_Absyn_Region_ptrqual_struct{int tag;void*f1;};struct Cyc_Absyn_VarargInfo{
struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{
struct Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{
struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{
int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct
_union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};
union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);union Cyc_Absyn_DatatypeInfoU
Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{
union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
rgn;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;
struct _tuple0*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};union Cyc_Absyn_DatatypeFieldInfoU
Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);union
Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,
struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU
field_info;struct Cyc_List_List*targs;};struct _tuple2{void*f1;struct _tuple0*f2;
struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2
val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};
union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct
_union_AggrInfoU_KnownAggr KnownAggr;};union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(
void*,struct _tuple0*,struct Cyc_Core_Opt*);union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(
struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU
aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;
struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*
zero_term;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_Evar_struct{int tag;
struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;int f3;struct Cyc_Core_Opt*f4;};struct
Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_struct{
int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_struct{
int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_struct{
int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_struct{int tag;void*f1;
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
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_struct{int tag;union
Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;struct Cyc_Position_Segment*
f3;};struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct
Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int tag;struct Cyc_Position_Segment*
f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct
_tuple3{void*f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};
struct _tuple4{void*f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4
val;};struct _tuple5{void*f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5
val;};struct _tuple6{void*f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;
struct _tuple6 val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr val;};
struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;};extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;union
Cyc_Absyn_Cnst Cyc_Absyn_Char_c(void*,char);union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(
void*,short);union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(void*,int);union Cyc_Absyn_Cnst
Cyc_Absyn_LongLong_c(void*,long long);union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(
struct _dyneither_ptr);union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _dyneither_ptr);
struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;
struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_struct{int tag;
struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{int tag;unsigned int
f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**
elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{
int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple0*
f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple0*f1;};struct
Cyc_Absyn_Primop_e_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_Increment_e_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};
struct Cyc_Absyn_Conditional_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnknownCall_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;
};struct Cyc_Absyn_Throw_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{
int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;void*f4;};struct Cyc_Absyn_Address_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{int tag;void*f1;};
struct Cyc_Absyn_Sizeofexp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_Gentyp_e_struct{int tag;struct Cyc_List_List*
f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_AggrMember_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple7{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple7*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*
f3;};struct Cyc_Absyn_Malloc_e_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};
struct Cyc_Absyn_Swap_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_UnresolvedMem_e_struct{int tag;struct Cyc_Core_Opt*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_Absyn_Tagcheck_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_struct{int tag;void*f1;};struct Cyc_Absyn_Exp{
struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*loc;void*annot;};
struct Cyc_Absyn_Exp_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct
_tuple8{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
int tag;struct _tuple8 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct _dyneither_ptr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tuple8 f2;struct _tuple8 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple8 f2;};struct Cyc_Absyn_TryCatch_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*
loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Var_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_struct{
int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_struct{int tag;
struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_struct{int tag;struct Cyc_Absyn_AggrInfo*
f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct
Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Int_p_struct{int tag;void*f1;int f2;};
struct Cyc_Absyn_Char_p_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_struct{int
tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;
struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct _tuple0*f1;
struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*
pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Vardecl{void*sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;
void*type;struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{void*sc;int is_inline;struct
_tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;void*ret_type;
struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;
struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
void*kind;void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct
_tuple0*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};
struct Cyc_Absyn_Datatypedecl{void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;
struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct
_tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Region_d_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*
f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};
struct Cyc_Absyn_Namespace_d_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*
r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};char Cyc_Absyn_EmptyAnnot[15]="\000\000\000\000EmptyAnnot\000";
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*);int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);struct Cyc_Absyn_Tqual
Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*
x);union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();union Cyc_Absyn_Constraint*
Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_val(
union Cyc_Absyn_Constraint*x);void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*
x);void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);extern union
Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;extern union Cyc_Absyn_Constraint*
Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_force_kb(
void*kb);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);void*Cyc_Absyn_int_typ(void*,void*);
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;extern void*Cyc_Absyn_float_typ;
void*Cyc_Absyn_double_typ(int);extern void*Cyc_Absyn_empty_effect;extern struct
_tuple0*Cyc_Absyn_exn_name;extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
extern void*Cyc_Absyn_exn_typ;extern struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar;void*Cyc_Absyn_string_typ(
void*rgn);void*Cyc_Absyn_const_string_typ(void*rgn);void*Cyc_Absyn_file_typ();
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*
zero_term);void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,
union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_star_typ(void*t,void*rgn,
struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);void*Cyc_Absyn_at_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dyneither_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
void*Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple0*name);void*Cyc_Absyn_unionq_typ(struct
_tuple0*name);void*Cyc_Absyn_union_typ(struct _dyneither_ptr*name);void*Cyc_Absyn_array_typ(
void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*
zero_term,struct Cyc_Position_Segment*ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct
Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(
int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);
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
struct Cyc_List_List*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,int,struct Cyc_Absyn_Exp*,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*
init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,
struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(
struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int
tagged);struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*k,void*s,struct _tuple0*n,
struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(void*
s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct
Cyc_List_List*atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(
void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,
struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_datatype_decl(void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*
fs,int is_extensible,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts);void*Cyc_Absyn_pointer_expand(void*,int
fresh_evar);int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple9*Cyc_Absyn_lookup_tuple_field(
struct Cyc_List_List*,int);struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
int Cyc_Absyn_fntype_att(void*a);struct _dyneither_ptr*Cyc_Absyn_fieldname(int);
struct _tuple10{void*f1;struct _tuple0*f2;};struct _tuple10 Cyc_Absyn_aggr_kinded_name(
union Cyc_Absyn_AggrInfoU);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU info);int Cyc_Absyn_is_union_type(void*);int Cyc_Absyn_is_nontagged_union_type(
void*);int Cyc_Absyn_is_aggr_type(void*t);extern int Cyc_Absyn_porting_c_code;
extern int Cyc_Absyn_no_regions;struct Cyc_Typerep_Int_struct{int tag;int f1;
unsigned int f2;};struct Cyc_Typerep_ThinPtr_struct{int tag;unsigned int f1;void*f2;
};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;};struct _tuple11{unsigned int f1;
struct _dyneither_ptr f2;void*f3;};struct Cyc_Typerep_Struct_struct{int tag;struct
_dyneither_ptr*f1;unsigned int f2;struct _dyneither_ptr f3;};struct _tuple12{
unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;unsigned int f1;
struct _dyneither_ptr f2;};struct _tuple13{unsigned int f1;struct _dyneither_ptr f2;};
struct Cyc_Typerep_TUnion_struct{int tag;struct _dyneither_ptr f1;struct
_dyneither_ptr f2;struct _dyneither_ptr f3;};struct Cyc_Typerep_TUnionField_struct{
int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;unsigned int f3;struct
_dyneither_ptr f4;};struct _tuple14{struct _dyneither_ptr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _dyneither_ptr f1;struct _dyneither_ptr f2;};struct Cyc_Typerep_Union_struct{
int tag;struct _dyneither_ptr*f1;int f2;struct _dyneither_ptr f3;};struct Cyc_Typerep_Enum_struct{
int tag;struct _dyneither_ptr*f1;int f2;struct _dyneither_ptr f3;};unsigned int Cyc_Typerep_size_type(
void*rep);int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
typedef struct{int __count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;
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
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_Iter_Iter{void*env;int(*next)(void*
env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct _tuple15{void*f1;void*f2;};struct _tuple15*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple15*Cyc_Dict_rchoose(
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
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*
le;int allow_valueof;};void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);void*Cyc_Tcutil_kind_to_bound(
void*k);struct _tuple16{struct Cyc_Dict_Dict f1;int f2;};struct _tuple17{struct
_dyneither_ptr f1;int f2;};extern int Cyc_Cyclone_tovc_r;extern void*Cyc_Cyclone_c_compiler;
int Cyc_Cyclone_tovc_r=0;void*Cyc_Cyclone_c_compiler=(void*)0;static int Cyc_Absyn_strlist_cmp(
struct Cyc_List_List*ss1,struct Cyc_List_List*ss2){return((int(*)(int(*cmp)(struct
_dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_strptrcmp,ss1,ss2);}int Cyc_Absyn_varlist_cmp(struct
Cyc_List_List*vs1,struct Cyc_List_List*vs2){if((int)vs1 == (int)vs2)return 0;return
Cyc_Absyn_strlist_cmp(vs1,vs2);}struct _tuple18{union Cyc_Absyn_Nmspace f1;union Cyc_Absyn_Nmspace
f2;};int Cyc_Absyn_qvar_cmp(struct _tuple0*q1,struct _tuple0*q2){union Cyc_Absyn_Nmspace
_tmp0=(*q1).f1;union Cyc_Absyn_Nmspace _tmp1=(*q2).f1;{struct _tuple18 _tmp3=({
struct _tuple18 _tmp2;_tmp2.f1=_tmp0;_tmp2.f2=_tmp1;_tmp2;});union Cyc_Absyn_Nmspace
_tmp4;int _tmp5;union Cyc_Absyn_Nmspace _tmp6;int _tmp7;union Cyc_Absyn_Nmspace _tmp8;
struct Cyc_List_List*_tmp9;union Cyc_Absyn_Nmspace _tmpA;struct Cyc_List_List*_tmpB;
union Cyc_Absyn_Nmspace _tmpC;struct Cyc_List_List*_tmpD;union Cyc_Absyn_Nmspace
_tmpE;struct Cyc_List_List*_tmpF;union Cyc_Absyn_Nmspace _tmp10;int _tmp11;union Cyc_Absyn_Nmspace
_tmp12;int _tmp13;union Cyc_Absyn_Nmspace _tmp14;struct Cyc_List_List*_tmp15;union
Cyc_Absyn_Nmspace _tmp16;struct Cyc_List_List*_tmp17;_LL1: _tmp4=_tmp3.f1;if((_tmp4.Loc_n).tag
!= 3)goto _LL3;_tmp5=(int)(_tmp4.Loc_n).val;_tmp6=_tmp3.f2;if((_tmp6.Loc_n).tag != 
3)goto _LL3;_tmp7=(int)(_tmp6.Loc_n).val;_LL2: goto _LL0;_LL3: _tmp8=_tmp3.f1;if((
_tmp8.Rel_n).tag != 1)goto _LL5;_tmp9=(struct Cyc_List_List*)(_tmp8.Rel_n).val;
_tmpA=_tmp3.f2;if((_tmpA.Rel_n).tag != 1)goto _LL5;_tmpB=(struct Cyc_List_List*)(
_tmpA.Rel_n).val;_LL4: _tmpD=_tmp9;_tmpF=_tmpB;goto _LL6;_LL5: _tmpC=_tmp3.f1;if((
_tmpC.Abs_n).tag != 2)goto _LL7;_tmpD=(struct Cyc_List_List*)(_tmpC.Abs_n).val;
_tmpE=_tmp3.f2;if((_tmpE.Abs_n).tag != 2)goto _LL7;_tmpF=(struct Cyc_List_List*)(
_tmpE.Abs_n).val;_LL6: {int i=Cyc_Absyn_strlist_cmp(_tmpD,_tmpF);if(i != 0)return i;
goto _LL0;}_LL7: _tmp10=_tmp3.f1;if((_tmp10.Loc_n).tag != 3)goto _LL9;_tmp11=(int)(
_tmp10.Loc_n).val;_LL8: return - 1;_LL9: _tmp12=_tmp3.f2;if((_tmp12.Loc_n).tag != 3)
goto _LLB;_tmp13=(int)(_tmp12.Loc_n).val;_LLA: return 1;_LLB: _tmp14=_tmp3.f1;if((
_tmp14.Rel_n).tag != 1)goto _LLD;_tmp15=(struct Cyc_List_List*)(_tmp14.Rel_n).val;
_LLC: return - 1;_LLD: _tmp16=_tmp3.f2;if((_tmp16.Rel_n).tag != 1)goto _LL0;_tmp17=(
struct Cyc_List_List*)(_tmp16.Rel_n).val;_LLE: return 1;_LL0:;}return Cyc_strptrcmp((*
q1).f2,(*q2).f2);}int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*
tv2){int i=Cyc_strptrcmp(tv1->name,tv2->name);if(i != 0)return i;return tv1->identity
- tv2->identity;}union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n={.Loc_n={3,0}};union Cyc_Absyn_Nmspace
Cyc_Absyn_Abs_n(struct Cyc_List_List*x){return({union Cyc_Absyn_Nmspace _tmp18;(
_tmp18.Abs_n).val=x;(_tmp18.Abs_n).tag=2;_tmp18;});}union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(
struct Cyc_List_List*x){return({union Cyc_Absyn_Nmspace _tmp19;(_tmp19.Rel_n).val=x;(
_tmp19.Rel_n).tag=1;_tmp19;});}union Cyc_Absyn_Nmspace Cyc_Absyn_rel_ns_null={.Rel_n={
1,0}};int Cyc_Absyn_is_qvar_qualified(struct _tuple0*qv){union Cyc_Absyn_Nmspace
_tmp1A=(*qv).f1;struct Cyc_List_List*_tmp1B;struct Cyc_List_List*_tmp1C;int _tmp1D;
_LL10: if((_tmp1A.Rel_n).tag != 1)goto _LL12;_tmp1B=(struct Cyc_List_List*)(_tmp1A.Rel_n).val;
if(_tmp1B != 0)goto _LL12;_LL11: goto _LL13;_LL12: if((_tmp1A.Abs_n).tag != 2)goto
_LL14;_tmp1C=(struct Cyc_List_List*)(_tmp1A.Abs_n).val;if(_tmp1C != 0)goto _LL14;
_LL13: goto _LL15;_LL14: if((_tmp1A.Loc_n).tag != 3)goto _LL16;_tmp1D=(int)(_tmp1A.Loc_n).val;
_LL15: return 0;_LL16:;_LL17: return 1;_LLF:;}static int Cyc_Absyn_new_type_counter=0;
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*env){return(void*)({
struct Cyc_Absyn_Evar_struct*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E[0]=({struct
Cyc_Absyn_Evar_struct _tmp1F;_tmp1F.tag=0;_tmp1F.f1=k;_tmp1F.f2=0;_tmp1F.f3=Cyc_Absyn_new_type_counter
++;_tmp1F.f4=env;_tmp1F;});_tmp1E;});}static struct Cyc_Core_Opt Cyc_Absyn_mk={(
void*)((void*)1)};void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){return Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Absyn_mk,tenv);}struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(
struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){return({struct Cyc_Absyn_Tqual
_tmp20;_tmp20.print_const=x.print_const  || y.print_const;_tmp20.q_volatile=x.q_volatile
 || y.q_volatile;_tmp20.q_restrict=x.q_restrict  || y.q_restrict;_tmp20.real_const=
x.real_const  || y.real_const;_tmp20.loc=Cyc_Position_segment_join(x.loc,y.loc);
_tmp20;});}struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*
loc){return({struct Cyc_Absyn_Tqual _tmp21;_tmp21.print_const=0;_tmp21.q_volatile=
0;_tmp21.q_restrict=0;_tmp21.real_const=0;_tmp21.loc=loc;_tmp21;});}struct Cyc_Absyn_Tqual
Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*loc){return({struct Cyc_Absyn_Tqual
_tmp22;_tmp22.print_const=1;_tmp22.q_volatile=0;_tmp22.q_restrict=0;_tmp22.real_const=
1;_tmp22.loc=loc;_tmp22;});}static struct Cyc_Absyn_Const_e_struct Cyc_Absyn_one_b_raw={
0,{.Int_c={4,{(void*)1,1}}}};struct Cyc_Absyn_Exp Cyc_Absyn_exp_unsigned_one_v={0,(
void*)& Cyc_Absyn_one_b_raw,0,(void*)Cyc_Absyn_EmptyAnnot};struct Cyc_Absyn_Exp*
Cyc_Absyn_exp_unsigned_one=& Cyc_Absyn_exp_unsigned_one_v;static struct Cyc_Absyn_Upper_b_struct
Cyc_Absyn_one_bt={0,& Cyc_Absyn_exp_unsigned_one_v};void*Cyc_Absyn_bounds_one=(
void*)& Cyc_Absyn_one_bt;union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(
struct Cyc_Absyn_UnknownDatatypeInfo udi){return({union Cyc_Absyn_DatatypeInfoU
_tmp25;(_tmp25.UnknownDatatype).val=udi;(_tmp25.UnknownDatatype).tag=1;_tmp25;});}
extern union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**
d){return({union Cyc_Absyn_DatatypeInfoU _tmp26;(_tmp26.KnownDatatype).val=d;(
_tmp26.KnownDatatype).tag=2;_tmp26;});}union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(
struct Cyc_Absyn_UnknownDatatypeFieldInfo s){return({union Cyc_Absyn_DatatypeFieldInfoU
_tmp27;(_tmp27.UnknownDatatypefield).val=s;(_tmp27.UnknownDatatypefield).tag=1;
_tmp27;});}union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct
Cyc_Absyn_Datatypedecl*dd,struct Cyc_Absyn_Datatypefield*df){return({union Cyc_Absyn_DatatypeFieldInfoU
_tmp28;(_tmp28.KnownDatatypefield).val=({struct _tuple1 _tmp29;_tmp29.f1=dd;_tmp29.f2=
df;_tmp29;});(_tmp28.KnownDatatypefield).tag=2;_tmp28;});}union Cyc_Absyn_AggrInfoU
Cyc_Absyn_UnknownAggr(void*ak,struct _tuple0*n,struct Cyc_Core_Opt*tagged){return({
union Cyc_Absyn_AggrInfoU _tmp2A;(_tmp2A.UnknownAggr).val=({struct _tuple2 _tmp2B;
_tmp2B.f1=ak;_tmp2B.f2=n;_tmp2B.f3=tagged;_tmp2B;});(_tmp2A.UnknownAggr).tag=1;
_tmp2A;});}union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**
ad){return({union Cyc_Absyn_AggrInfoU _tmp2C;(_tmp2C.KnownAggr).val=ad;(_tmp2C.KnownAggr).tag=
2;_tmp2C;});}union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x){return({
union Cyc_Absyn_Constraint*_tmp2D=_cycalloc(sizeof(*_tmp2D));(_tmp2D->Eq_constr).val=(
void*)x;(_tmp2D->Eq_constr).tag=1;_tmp2D;});}union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref(){
return({union Cyc_Absyn_Constraint*_tmp2E=_cycalloc_atomic(sizeof(*_tmp2E));(
_tmp2E->No_constr).val=0;(_tmp2E->No_constr).tag=3;_tmp2E;});}static union Cyc_Absyn_Constraint
Cyc_Absyn_true_conref_v={.Eq_constr={1,(void*)1}};union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref=&
Cyc_Absyn_true_conref_v;static union Cyc_Absyn_Constraint Cyc_Absyn_false_conref_v={.Eq_constr={
1,(void*)0}};union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref=& Cyc_Absyn_false_conref_v;
static union Cyc_Absyn_Constraint Cyc_Absyn_bounds_one_conref_v={.Eq_constr={1,(
void*)((void*)& Cyc_Absyn_one_bt)}};union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref=&
Cyc_Absyn_bounds_one_conref_v;static union Cyc_Absyn_Constraint Cyc_Absyn_bounds_dyneither_conref_v={.Eq_constr={
1,(void*)((void*)0)}};union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref=&
Cyc_Absyn_bounds_dyneither_conref_v;union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(
union Cyc_Absyn_Constraint*x){union Cyc_Absyn_Constraint*_tmp2F=x;union Cyc_Absyn_Constraint
_tmp30;int _tmp31;union Cyc_Absyn_Constraint _tmp32;void*_tmp33;union Cyc_Absyn_Constraint
_tmp34;union Cyc_Absyn_Constraint*_tmp35;_LL19: _tmp30=*_tmp2F;if((_tmp30.No_constr).tag
!= 3)goto _LL1B;_tmp31=(int)(_tmp30.No_constr).val;_LL1A: goto _LL1C;_LL1B: _tmp32=*
_tmp2F;if((_tmp32.Eq_constr).tag != 1)goto _LL1D;_tmp33=(void*)(_tmp32.Eq_constr).val;
_LL1C: return x;_LL1D: _tmp34=*_tmp2F;if((_tmp34.Forward_constr).tag != 2)goto _LL18;
_tmp35=(union Cyc_Absyn_Constraint*)(_tmp34.Forward_constr).val;_LL1E: {union Cyc_Absyn_Constraint*
_tmp36=Cyc_Absyn_compress_conref(_tmp35);({struct
_union_Constraint_Forward_constr*_tmp37=& x->Forward_constr;_tmp37->tag=2;_tmp37->val=
_tmp36;});return _tmp36;}_LL18:;}void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*
x){union Cyc_Absyn_Constraint*_tmp38=Cyc_Absyn_compress_conref(x);union Cyc_Absyn_Constraint
_tmp39;void*_tmp3A;_LL20: _tmp39=*_tmp38;if((_tmp39.Eq_constr).tag != 1)goto _LL22;
_tmp3A=(void*)(_tmp39.Eq_constr).val;_LL21: return _tmp3A;_LL22:;_LL23:({void*
_tmp3B=0;Cyc_Tcutil_impos(({const char*_tmp3C="conref_val";_tag_dyneither(_tmp3C,
sizeof(char),11);}),_tag_dyneither(_tmp3B,sizeof(void*),0));});_LL1F:;}void*Cyc_Absyn_conref_def(
void*y,union Cyc_Absyn_Constraint*x){union Cyc_Absyn_Constraint*_tmp3D=Cyc_Absyn_compress_conref(
x);union Cyc_Absyn_Constraint _tmp3E;void*_tmp3F;_LL25: _tmp3E=*_tmp3D;if((_tmp3E.Eq_constr).tag
!= 1)goto _LL27;_tmp3F=(void*)(_tmp3E.Eq_constr).val;_LL26: return _tmp3F;_LL27:;
_LL28: return y;_LL24:;}void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*
x){x=Cyc_Absyn_compress_conref(x);{union Cyc_Absyn_Constraint*_tmp40=x;union Cyc_Absyn_Constraint
_tmp41;void*_tmp42;_LL2A: _tmp41=*_tmp40;if((_tmp41.Eq_constr).tag != 1)goto _LL2C;
_tmp42=(void*)(_tmp41.Eq_constr).val;_LL2B: return _tmp42;_LL2C:;_LL2D:({struct
_union_Constraint_Eq_constr*_tmp43=& x->Eq_constr;_tmp43->tag=1;_tmp43->val=y;});
return y;_LL29:;}}void*Cyc_Absyn_compress_kb(void*k){void*_tmp44=k;struct Cyc_Core_Opt*
_tmp45;struct Cyc_Core_Opt*_tmp46;struct Cyc_Core_Opt*_tmp47;struct Cyc_Core_Opt
_tmp48;void*_tmp49;void**_tmp4A;struct Cyc_Core_Opt*_tmp4B;struct Cyc_Core_Opt
_tmp4C;void*_tmp4D;void**_tmp4E;_LL2F: if(*((int*)_tmp44)!= 0)goto _LL31;_LL30:
goto _LL32;_LL31: if(*((int*)_tmp44)!= 1)goto _LL33;_tmp45=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp44)->f1;if(_tmp45 != 0)goto _LL33;_LL32: goto _LL34;_LL33: if(*((int*)_tmp44)!= 2)
goto _LL35;_tmp46=((struct Cyc_Absyn_Less_kb_struct*)_tmp44)->f1;if(_tmp46 != 0)
goto _LL35;_LL34: return k;_LL35: if(*((int*)_tmp44)!= 1)goto _LL37;_tmp47=((struct
Cyc_Absyn_Unknown_kb_struct*)_tmp44)->f1;if(_tmp47 == 0)goto _LL37;_tmp48=*_tmp47;
_tmp49=(void*)_tmp48.v;_tmp4A=(void**)&(*((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp44)->f1).v;_LL36: _tmp4E=_tmp4A;goto _LL38;_LL37: if(*((int*)_tmp44)!= 2)goto
_LL2E;_tmp4B=((struct Cyc_Absyn_Less_kb_struct*)_tmp44)->f1;if(_tmp4B == 0)goto
_LL2E;_tmp4C=*_tmp4B;_tmp4D=(void*)_tmp4C.v;_tmp4E=(void**)&(*((struct Cyc_Absyn_Less_kb_struct*)
_tmp44)->f1).v;_LL38:*_tmp4E=Cyc_Absyn_compress_kb(*_tmp4E);return*_tmp4E;_LL2E:;}
void*Cyc_Absyn_force_kb(void*kb){void*_tmp4F=Cyc_Absyn_compress_kb(kb);void*
_tmp50;struct Cyc_Core_Opt*_tmp51;struct Cyc_Core_Opt**_tmp52;struct Cyc_Core_Opt*
_tmp53;struct Cyc_Core_Opt**_tmp54;void*_tmp55;_LL3A: if(*((int*)_tmp4F)!= 0)goto
_LL3C;_tmp50=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp4F)->f1;_LL3B: return
_tmp50;_LL3C: if(*((int*)_tmp4F)!= 1)goto _LL3E;_tmp51=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp4F)->f1;_tmp52=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmp4F)->f1;_LL3D: _tmp54=_tmp52;_tmp55=(void*)2;goto _LL3F;_LL3E: if(*((int*)
_tmp4F)!= 2)goto _LL39;_tmp53=((struct Cyc_Absyn_Less_kb_struct*)_tmp4F)->f1;
_tmp54=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp4F)->f1;
_tmp55=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp4F)->f2;_LL3F:*_tmp54=({
struct Cyc_Core_Opt*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->v=(void*)Cyc_Tcutil_kind_to_bound(
_tmp55);_tmp56;});return _tmp55;_LL39:;}static struct Cyc_Absyn_IntType_struct Cyc_Absyn_char_tt={
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
_tmp66=sn;_LL41: if((int)_tmp66 != 0)goto _LL43;_LL42: {void*_tmp67=sz;_LL48: if((
int)_tmp67 != 0)goto _LL4A;_LL49: return Cyc_Absyn_schar_typ;_LL4A: if((int)_tmp67 != 
1)goto _LL4C;_LL4B: return Cyc_Absyn_sshort_typ;_LL4C: if((int)_tmp67 != 2)goto _LL4E;
_LL4D: return Cyc_Absyn_sint_typ;_LL4E: if((int)_tmp67 != 3)goto _LL50;_LL4F: return
Cyc_Absyn_slong_typ;_LL50: if((int)_tmp67 != 4)goto _LL47;_LL51: return Cyc_Absyn_slonglong_typ;
_LL47:;}_LL43: if((int)_tmp66 != 1)goto _LL45;_LL44: {void*_tmp68=sz;_LL53: if((int)
_tmp68 != 0)goto _LL55;_LL54: return Cyc_Absyn_uchar_typ;_LL55: if((int)_tmp68 != 1)
goto _LL57;_LL56: return Cyc_Absyn_ushort_typ;_LL57: if((int)_tmp68 != 2)goto _LL59;
_LL58: return Cyc_Absyn_uint_typ;_LL59: if((int)_tmp68 != 3)goto _LL5B;_LL5A: return
Cyc_Absyn_ulong_typ;_LL5B: if((int)_tmp68 != 4)goto _LL52;_LL5C: return Cyc_Absyn_ulonglong_typ;
_LL52:;}_LL45: if((int)_tmp66 != 2)goto _LL40;_LL46: {void*_tmp69=sz;_LL5E: if((int)
_tmp69 != 0)goto _LL60;_LL5F: return Cyc_Absyn_char_typ;_LL60: if((int)_tmp69 != 1)
goto _LL62;_LL61: return Cyc_Absyn_nshort_typ;_LL62: if((int)_tmp69 != 2)goto _LL64;
_LL63: return Cyc_Absyn_nint_typ;_LL64: if((int)_tmp69 != 3)goto _LL66;_LL65: return
Cyc_Absyn_nlong_typ;_LL66: if((int)_tmp69 != 4)goto _LL5D;_LL67: return Cyc_Absyn_nlonglong_typ;
_LL5D:;}_LL40:;}void*Cyc_Absyn_float_typ=(void*)1;void*Cyc_Absyn_double_typ(int b){
static struct Cyc_Absyn_DoubleType_struct dt={6,1};static struct Cyc_Absyn_DoubleType_struct
df={6,0};return(void*)(b?& dt:& df);}static char _tmp6C[4]="exn";static struct
_dyneither_ptr Cyc_Absyn_exn_str={_tmp6C,_tmp6C,_tmp6C + 4};static struct _tuple0 Cyc_Absyn_exn_name_v={{.Abs_n={
2,0}},& Cyc_Absyn_exn_str};struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;
static char _tmp6D[15]="Null_Exception";static struct _dyneither_ptr Cyc_Absyn_Null_Exception_str={
_tmp6D,_tmp6D,_tmp6D + 15};static struct _tuple0 Cyc_Absyn_Null_Exception_pr={{.Abs_n={
2,0}},& Cyc_Absyn_Null_Exception_str};struct _tuple0*Cyc_Absyn_Null_Exception_name=&
Cyc_Absyn_Null_Exception_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Null_Exception_tuf_v={&
Cyc_Absyn_Null_Exception_pr,0,0,(void*)3};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Null_Exception_tuf=&
Cyc_Absyn_Null_Exception_tuf_v;static char _tmp6E[13]="Array_bounds";static struct
_dyneither_ptr Cyc_Absyn_Array_bounds_str={_tmp6E,_tmp6E,_tmp6E + 13};static struct
_tuple0 Cyc_Absyn_Array_bounds_pr={{.Abs_n={2,0}},& Cyc_Absyn_Array_bounds_str};
struct _tuple0*Cyc_Absyn_Array_bounds_name=& Cyc_Absyn_Array_bounds_pr;static
struct Cyc_Absyn_Datatypefield Cyc_Absyn_Array_bounds_tuf_v={& Cyc_Absyn_Array_bounds_pr,
0,0,(void*)3};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Array_bounds_tuf=& Cyc_Absyn_Array_bounds_tuf_v;
static char _tmp6F[16]="Match_Exception";static struct _dyneither_ptr Cyc_Absyn_Match_Exception_str={
_tmp6F,_tmp6F,_tmp6F + 16};static struct _tuple0 Cyc_Absyn_Match_Exception_pr={{.Abs_n={
2,0}},& Cyc_Absyn_Match_Exception_str};struct _tuple0*Cyc_Absyn_Match_Exception_name=&
Cyc_Absyn_Match_Exception_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Match_Exception_tuf_v={&
Cyc_Absyn_Match_Exception_pr,0,0,(void*)3};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Match_Exception_tuf=&
Cyc_Absyn_Match_Exception_tuf_v;static char _tmp70[10]="Bad_alloc";static struct
_dyneither_ptr Cyc_Absyn_Bad_alloc_str={_tmp70,_tmp70,_tmp70 + 10};static struct
_tuple0 Cyc_Absyn_Bad_alloc_pr={{.Abs_n={2,0}},& Cyc_Absyn_Bad_alloc_str};struct
_tuple0*Cyc_Absyn_Bad_alloc_name=& Cyc_Absyn_Bad_alloc_pr;static struct Cyc_Absyn_Datatypefield
Cyc_Absyn_Bad_alloc_tuf_v={& Cyc_Absyn_Bad_alloc_pr,0,0,(void*)3};struct Cyc_Absyn_Datatypefield*
Cyc_Absyn_Bad_alloc_tuf=& Cyc_Absyn_Bad_alloc_tuf_v;static struct Cyc_List_List Cyc_Absyn_exn_l0={(
void*)& Cyc_Absyn_Null_Exception_tuf_v,0};static struct Cyc_List_List Cyc_Absyn_exn_l1={(
void*)& Cyc_Absyn_Array_bounds_tuf_v,(struct Cyc_List_List*)& Cyc_Absyn_exn_l0};
static struct Cyc_List_List Cyc_Absyn_exn_l2={(void*)& Cyc_Absyn_Match_Exception_tuf_v,(
struct Cyc_List_List*)& Cyc_Absyn_exn_l1};static struct Cyc_List_List Cyc_Absyn_exn_l3={(
void*)& Cyc_Absyn_Bad_alloc_tuf_v,(struct Cyc_List_List*)& Cyc_Absyn_exn_l2};static
struct Cyc_Core_Opt Cyc_Absyn_exn_ol={(void*)((struct Cyc_List_List*)& Cyc_Absyn_exn_l3)};
static struct Cyc_Absyn_Datatypedecl Cyc_Absyn_exn_tud_v={(void*)3,& Cyc_Absyn_exn_name_v,
0,(struct Cyc_Core_Opt*)& Cyc_Absyn_exn_ol,1};struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud=&
Cyc_Absyn_exn_tud_v;static struct Cyc_Core_Opt Cyc_Absyn_heap_opt={(void*)((void*)2)};
static struct Cyc_Absyn_DatatypeType_struct Cyc_Absyn_exn_typ_tt={2,{{.KnownDatatype={
2,& Cyc_Absyn_exn_tud}},0,(struct Cyc_Core_Opt*)& Cyc_Absyn_heap_opt}};void*Cyc_Absyn_exn_typ=(
void*)& Cyc_Absyn_exn_typ_tt;static char _tmp72[9]="PrintArg";static struct
_dyneither_ptr Cyc_Absyn_printarg_str={_tmp72,_tmp72,_tmp72 + 9};static char _tmp73[
9]="ScanfArg";static struct _dyneither_ptr Cyc_Absyn_scanfarg_str={_tmp73,_tmp73,
_tmp73 + 9};static struct _tuple0 Cyc_Absyn_datatype_print_arg_qvar_p={{.Abs_n={2,0}},&
Cyc_Absyn_printarg_str};static struct _tuple0 Cyc_Absyn_datatype_scanf_arg_qvar_p={{.Abs_n={
2,0}},& Cyc_Absyn_scanfarg_str};struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar=&
Cyc_Absyn_datatype_print_arg_qvar_p;struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar=&
Cyc_Absyn_datatype_scanf_arg_qvar_p;static void**Cyc_Absyn_string_t_opt=0;void*
Cyc_Absyn_string_typ(void*rgn){if(rgn != (void*)2)return Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,
rgn,Cyc_Absyn_empty_tqual(0),(void*)0,Cyc_Absyn_true_conref);if(Cyc_Absyn_string_t_opt
== 0){void*t=Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_empty_tqual(
0),(void*)0,Cyc_Absyn_true_conref);Cyc_Absyn_string_t_opt=({void**_tmp74=
_cycalloc(sizeof(*_tmp74));_tmp74[0]=t;_tmp74;});}return*((void**)_check_null(
Cyc_Absyn_string_t_opt));}static void**Cyc_Absyn_const_string_t_opt=0;void*Cyc_Absyn_const_string_typ(
void*rgn){if(rgn != (void*)2)return Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(
0),(void*)0,Cyc_Absyn_true_conref);if(Cyc_Absyn_const_string_t_opt == 0){void*t=
Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(void*)2,Cyc_Absyn_const_tqual(0),(void*)
0,Cyc_Absyn_true_conref);Cyc_Absyn_const_string_t_opt=({void**_tmp75=_cycalloc(
sizeof(*_tmp75));_tmp75[0]=t;_tmp75;});}return*((void**)_check_null(Cyc_Absyn_const_string_t_opt));}
void*Cyc_Absyn_starb_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*
zeroterm){return(void*)({struct Cyc_Absyn_PointerType_struct*_tmp76=_cycalloc(
sizeof(*_tmp76));_tmp76[0]=({struct Cyc_Absyn_PointerType_struct _tmp77;_tmp77.tag=
4;_tmp77.f1=({struct Cyc_Absyn_PtrInfo _tmp78;_tmp78.elt_typ=t;_tmp78.elt_tq=tq;
_tmp78.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp79;_tmp79.rgn=r;_tmp79.nullable=Cyc_Absyn_true_conref;
_tmp79.bounds=Cyc_Absyn_new_conref(b);_tmp79.zero_term=zeroterm;_tmp79.ptrloc=0;
_tmp79;});_tmp78;});_tmp77;});_tmp76;});}void*Cyc_Absyn_atb_typ(void*t,void*r,
struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zeroterm){return(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A[0]=({
struct Cyc_Absyn_PointerType_struct _tmp7B;_tmp7B.tag=4;_tmp7B.f1=({struct Cyc_Absyn_PtrInfo
_tmp7C;_tmp7C.elt_typ=t;_tmp7C.elt_tq=tq;_tmp7C.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp7D;_tmp7D.rgn=r;_tmp7D.nullable=Cyc_Absyn_false_conref;_tmp7D.bounds=Cyc_Absyn_new_conref(
b);_tmp7D.zero_term=zeroterm;_tmp7D.ptrloc=0;_tmp7D;});_tmp7C;});_tmp7B;});
_tmp7A;});}void*Cyc_Absyn_star_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union
Cyc_Absyn_Constraint*zeroterm){return(void*)({struct Cyc_Absyn_PointerType_struct*
_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E[0]=({struct Cyc_Absyn_PointerType_struct
_tmp7F;_tmp7F.tag=4;_tmp7F.f1=({struct Cyc_Absyn_PtrInfo _tmp80;_tmp80.elt_typ=t;
_tmp80.elt_tq=tq;_tmp80.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp81;_tmp81.rgn=r;
_tmp81.nullable=Cyc_Absyn_true_conref;_tmp81.bounds=Cyc_Absyn_bounds_one_conref;
_tmp81.zero_term=zeroterm;_tmp81.ptrloc=0;_tmp81;});_tmp80;});_tmp7F;});_tmp7E;});}
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq){return(void*)({struct
Cyc_Absyn_PointerType_struct*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82[0]=({
struct Cyc_Absyn_PointerType_struct _tmp83;_tmp83.tag=4;_tmp83.f1=({struct Cyc_Absyn_PtrInfo
_tmp84;_tmp84.elt_typ=t;_tmp84.elt_tq=tq;_tmp84.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp85;_tmp85.rgn=(void*)2;_tmp85.nullable=Cyc_Absyn_true_conref;_tmp85.bounds=
Cyc_Absyn_bounds_one_conref;_tmp85.zero_term=Cyc_Absyn_false_conref;_tmp85.ptrloc=
0;_tmp85;});_tmp84;});_tmp83;});_tmp82;});}void*Cyc_Absyn_at_typ(void*t,void*r,
struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zeroterm){return(void*)({
struct Cyc_Absyn_PointerType_struct*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86[0]=({
struct Cyc_Absyn_PointerType_struct _tmp87;_tmp87.tag=4;_tmp87.f1=({struct Cyc_Absyn_PtrInfo
_tmp88;_tmp88.elt_typ=t;_tmp88.elt_tq=tq;_tmp88.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmp89;_tmp89.rgn=r;_tmp89.nullable=Cyc_Absyn_false_conref;_tmp89.bounds=Cyc_Absyn_bounds_one_conref;
_tmp89.zero_term=zeroterm;_tmp89.ptrloc=0;_tmp89;});_tmp88;});_tmp87;});_tmp86;});}
void*Cyc_Absyn_dyneither_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*
zeroterm){return(void*)({struct Cyc_Absyn_PointerType_struct*_tmp8A=_cycalloc(
sizeof(*_tmp8A));_tmp8A[0]=({struct Cyc_Absyn_PointerType_struct _tmp8B;_tmp8B.tag=
4;_tmp8B.f1=({struct Cyc_Absyn_PtrInfo _tmp8C;_tmp8C.elt_typ=t;_tmp8C.elt_tq=tq;
_tmp8C.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp8D;_tmp8D.rgn=r;_tmp8D.nullable=Cyc_Absyn_true_conref;
_tmp8D.bounds=Cyc_Absyn_bounds_dyneither_conref;_tmp8D.zero_term=zeroterm;_tmp8D.ptrloc=
0;_tmp8D;});_tmp8C;});_tmp8B;});_tmp8A;});}void*Cyc_Absyn_array_typ(void*
elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*
zero_term,struct Cyc_Position_Segment*ztloc){return(void*)({struct Cyc_Absyn_ArrayType_struct*
_tmp8E=_cycalloc(sizeof(*_tmp8E));_tmp8E[0]=({struct Cyc_Absyn_ArrayType_struct
_tmp8F;_tmp8F.tag=7;_tmp8F.f1=({struct Cyc_Absyn_ArrayInfo _tmp90;_tmp90.elt_type=
elt_type;_tmp90.tq=tq;_tmp90.num_elts=num_elts;_tmp90.zero_term=zero_term;_tmp90.zt_loc=
ztloc;_tmp90;});_tmp8F;});_tmp8E;});}static char _tmp98[8]="__sFILE";void*Cyc_Absyn_file_typ(){
static void**file_t_opt=0;static struct _dyneither_ptr sf_str={_tmp98,_tmp98,_tmp98 + 
8};static struct _dyneither_ptr*sf=& sf_str;if(file_t_opt == 0){struct _tuple0*
file_t_name=({struct _tuple0*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->f1=Cyc_Absyn_Abs_n(
0);_tmp97->f2=sf;_tmp97;});struct Cyc_Absyn_Aggrdecl*sd=({struct Cyc_Absyn_Aggrdecl*
_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96->kind=(void*)0;_tmp96->sc=(void*)1;
_tmp96->name=file_t_name;_tmp96->tvs=0;_tmp96->impl=0;_tmp96->attributes=0;
_tmp96;});void*file_struct_typ=(void*)({struct Cyc_Absyn_AggrType_struct*_tmp92=
_cycalloc(sizeof(*_tmp92));_tmp92[0]=({struct Cyc_Absyn_AggrType_struct _tmp93;
_tmp93.tag=10;_tmp93.f1=({struct Cyc_Absyn_AggrInfo _tmp94;_tmp94.aggr_info=Cyc_Absyn_KnownAggr(({
struct Cyc_Absyn_Aggrdecl**_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95[0]=sd;_tmp95;}));
_tmp94.targs=0;_tmp94;});_tmp93;});_tmp92;});file_t_opt=({void**_tmp91=_cycalloc(
sizeof(*_tmp91));_tmp91[0]=Cyc_Absyn_at_typ(file_struct_typ,(void*)2,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_false_conref);_tmp91;});}return*file_t_opt;}void*Cyc_Absyn_void_star_typ(){
static void**void_star_t_opt=0;if(void_star_t_opt == 0)void_star_t_opt=({void**
_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99[0]=Cyc_Absyn_star_typ((void*)0,(void*)2,
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);_tmp99;});return*
void_star_t_opt;}static struct Cyc_Absyn_JoinEff_struct Cyc_Absyn_empty_eff={20,0};
void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_eff;void*Cyc_Absyn_aggr_typ(
void*k,struct _dyneither_ptr*name){return(void*)({struct Cyc_Absyn_AggrType_struct*
_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B[0]=({struct Cyc_Absyn_AggrType_struct
_tmp9C;_tmp9C.tag=10;_tmp9C.f1=({struct Cyc_Absyn_AggrInfo _tmp9D;_tmp9D.aggr_info=
Cyc_Absyn_UnknownAggr(k,({struct _tuple0*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->f1=
Cyc_Absyn_rel_ns_null;_tmp9E->f2=name;_tmp9E;}),0);_tmp9D.targs=0;_tmp9D;});
_tmp9C;});_tmp9B;});}void*Cyc_Absyn_strct(struct _dyneither_ptr*name){return Cyc_Absyn_aggr_typ((
void*)0,name);}void*Cyc_Absyn_union_typ(struct _dyneither_ptr*name){return Cyc_Absyn_aggr_typ((
void*)1,name);}void*Cyc_Absyn_strctq(struct _tuple0*name){return(void*)({struct
Cyc_Absyn_AggrType_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F[0]=({struct
Cyc_Absyn_AggrType_struct _tmpA0;_tmpA0.tag=10;_tmpA0.f1=({struct Cyc_Absyn_AggrInfo
_tmpA1;_tmpA1.aggr_info=Cyc_Absyn_UnknownAggr((void*)0,name,0);_tmpA1.targs=0;
_tmpA1;});_tmpA0;});_tmp9F;});}void*Cyc_Absyn_unionq_typ(struct _tuple0*name){
return(void*)({struct Cyc_Absyn_AggrType_struct*_tmpA2=_cycalloc(sizeof(*_tmpA2));
_tmpA2[0]=({struct Cyc_Absyn_AggrType_struct _tmpA3;_tmpA3.tag=10;_tmpA3.f1=({
struct Cyc_Absyn_AggrInfo _tmpA4;_tmpA4.aggr_info=Cyc_Absyn_UnknownAggr((void*)1,
name,0);_tmpA4.targs=0;_tmpA4;});_tmpA3;});_tmpA2;});}union Cyc_Absyn_Cnst Cyc_Absyn_Null_c={.Null_c={
1,0}};union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(void*sn,char c){return({union Cyc_Absyn_Cnst
_tmpA5;(_tmpA5.Char_c).val=({struct _tuple3 _tmpA6;_tmpA6.f1=sn;_tmpA6.f2=c;_tmpA6;});(
_tmpA5.Char_c).tag=2;_tmpA5;});}union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(void*sn,
short s){return({union Cyc_Absyn_Cnst _tmpA7;(_tmpA7.Short_c).val=({struct _tuple4
_tmpA8;_tmpA8.f1=sn;_tmpA8.f2=s;_tmpA8;});(_tmpA7.Short_c).tag=3;_tmpA7;});}
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(void*sn,int i){return({union Cyc_Absyn_Cnst
_tmpA9;(_tmpA9.Int_c).val=({struct _tuple5 _tmpAA;_tmpAA.f1=sn;_tmpAA.f2=i;_tmpAA;});(
_tmpA9.Int_c).tag=4;_tmpA9;});}union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(void*sn,
long long l){return({union Cyc_Absyn_Cnst _tmpAB;(_tmpAB.LongLong_c).val=({struct
_tuple6 _tmpAC;_tmpAC.f1=sn;_tmpAC.f2=l;_tmpAC;});(_tmpAB.LongLong_c).tag=5;
_tmpAB;});}union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr s){return({
union Cyc_Absyn_Cnst _tmpAD;(_tmpAD.Float_c).val=s;(_tmpAD.Float_c).tag=6;_tmpAD;});}
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _dyneither_ptr s){return({union Cyc_Absyn_Cnst
_tmpAE;(_tmpAE.String_c).val=s;(_tmpAE.String_c).tag=7;_tmpAE;});}struct Cyc_Absyn_Exp*
Cyc_Absyn_new_exp(void*r,struct Cyc_Position_Segment*loc){return({struct Cyc_Absyn_Exp*
_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->topt=0;_tmpAF->r=r;_tmpAF->loc=loc;
_tmpAF->annot=(void*)Cyc_Absyn_EmptyAnnot;_tmpAF;});}struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(
struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_New_e_struct*_tmpB0=
_cycalloc(sizeof(*_tmpB0));_tmpB0[0]=({struct Cyc_Absyn_New_e_struct _tmpB1;_tmpB1.tag=
17;_tmpB1.f1=rgn_handle;_tmpB1.f2=e;_tmpB1;});_tmpB0;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e){return({struct Cyc_Absyn_Exp*_tmpB2=
_cycalloc(sizeof(*_tmpB2));_tmpB2[0]=*e;_tmpB2;});}struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(
union Cyc_Absyn_Cnst c,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Const_e_struct*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3[
0]=({struct Cyc_Absyn_Const_e_struct _tmpB4;_tmpB4.tag=0;_tmpB4.f1=c;_tmpB4;});
_tmpB3;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Const_e_struct*_tmpB5=
_cycalloc(sizeof(*_tmpB5));_tmpB5[0]=({struct Cyc_Absyn_Const_e_struct _tmpB6;
_tmpB6.tag=0;_tmpB6.f1=Cyc_Absyn_Null_c;_tmpB6;});_tmpB5;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_int_exp(void*s,int i,struct Cyc_Position_Segment*seg){return Cyc_Absyn_const_exp(
Cyc_Absyn_Int_c(s,i),seg);}struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int i,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_int_exp((void*)0,i,loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int i,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_int_exp((void*)1,(int)i,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(
int b,struct Cyc_Position_Segment*loc){return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(struct Cyc_Position_Segment*loc){return Cyc_Absyn_bool_exp(
1,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*loc){
return Cyc_Absyn_bool_exp(0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp(Cyc_Absyn_Char_c((void*)
2,c),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,struct
Cyc_Position_Segment*loc){return Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(f),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_const_exp(Cyc_Absyn_String_c(s),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_var_exp(struct _tuple0*q,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Var_e_struct*_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7[0]=({
struct Cyc_Absyn_Var_e_struct _tmpB8;_tmpB8.tag=1;_tmpB8.f1=q;_tmpB8.f2=(void*)((
void*)0);_tmpB8;});_tmpB7;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct
_tuple0*q,void*b,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Var_e_struct*_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9[0]=({
struct Cyc_Absyn_Var_e_struct _tmpBA;_tmpBA.tag=1;_tmpBA.f1=q;_tmpBA.f2=(void*)b;
_tmpBA;});_tmpB9;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct
_tuple0*q,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_UnknownId_e_struct*_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmpBC;_tmpBC.tag=2;_tmpBC.f1=q;_tmpBC;});
_tmpBB;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(void*p,struct Cyc_List_List*
es,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Primop_e_struct*
_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD[0]=({struct Cyc_Absyn_Primop_e_struct
_tmpBE;_tmpBE.tag=3;_tmpBE.f1=(void*)p;_tmpBE.f2=es;_tmpBE;});_tmpBD;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(void*p,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_primop_exp(p,({struct Cyc_List_List*_tmpBF=_cycalloc(sizeof(*
_tmpBF));_tmpBF->hd=e;_tmpBF->tl=0;_tmpBF;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_primop_exp(p,({struct Cyc_List_List*_tmpC0=_cycalloc(sizeof(*
_tmpC0));_tmpC0->hd=e1;_tmpC0->tl=({struct Cyc_List_List*_tmpC1=_cycalloc(sizeof(*
_tmpC1));_tmpC1->hd=e2;_tmpC1->tl=0;_tmpC1;});_tmpC0;}),loc);}struct Cyc_Absyn_Exp*
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
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AssignOp_e_struct*_tmpC2=
_cycalloc(sizeof(*_tmpC2));_tmpC2[0]=({struct Cyc_Absyn_AssignOp_e_struct _tmpC3;
_tmpC3.tag=4;_tmpC3.f1=e1;_tmpC3.f2=popt;_tmpC3.f3=e2;_tmpC3;});_tmpC2;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,void*i,struct
Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_struct*
_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4[0]=({struct Cyc_Absyn_Increment_e_struct
_tmpC5;_tmpC5.tag=5;_tmpC5.f1=e;_tmpC5.f2=(void*)i;_tmpC5;});_tmpC4;}),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,(void*)1,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(
e,(void*)0,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_increment_exp(e,(void*)2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_increment_exp(e,(void*)3,loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Conditional_e_struct*
_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6[0]=({struct Cyc_Absyn_Conditional_e_struct
_tmpC7;_tmpC7.tag=6;_tmpC7.f1=e1;_tmpC7.f2=e2;_tmpC7.f3=e3;_tmpC7;});_tmpC6;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_And_e_struct*
_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8[0]=({struct Cyc_Absyn_And_e_struct _tmpC9;
_tmpC9.tag=7;_tmpC9.f1=e1;_tmpC9.f2=e2;_tmpC9;});_tmpC8;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Or_e_struct*_tmpCA=
_cycalloc(sizeof(*_tmpCA));_tmpCA[0]=({struct Cyc_Absyn_Or_e_struct _tmpCB;_tmpCB.tag=
8;_tmpCB.f1=e1;_tmpCB.f2=e2;_tmpCB;});_tmpCA;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_SeqExp_e_struct*_tmpCC=
_cycalloc(sizeof(*_tmpCC));_tmpCC[0]=({struct Cyc_Absyn_SeqExp_e_struct _tmpCD;
_tmpCD.tag=9;_tmpCD.f1=e1;_tmpCD.f2=e2;_tmpCD;});_tmpCC;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownCall_e_struct*
_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE[0]=({struct Cyc_Absyn_UnknownCall_e_struct
_tmpCF;_tmpCF.tag=10;_tmpCF.f1=e;_tmpCF.f2=es;_tmpCF;});_tmpCE;}),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmpD1;_tmpD1.tag=11;_tmpD1.f1=e;_tmpD1.f2=es;_tmpD1.f3=0;_tmpD1;});_tmpD0;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_NoInstantiate_e_struct*
_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2[0]=({struct Cyc_Absyn_NoInstantiate_e_struct
_tmpD3;_tmpD3.tag=13;_tmpD3.f1=e;_tmpD3;});_tmpD2;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Instantiate_e_struct*
_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4[0]=({struct Cyc_Absyn_Instantiate_e_struct
_tmpD5;_tmpD5.tag=14;_tmpD5.f1=e;_tmpD5.f2=ts;_tmpD5;});_tmpD4;}),loc);}struct
Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,int user_cast,void*
c,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Cast_e_struct*
_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6[0]=({struct Cyc_Absyn_Cast_e_struct
_tmpD7;_tmpD7.tag=15;_tmpD7.f1=(void*)t;_tmpD7.f2=e;_tmpD7.f3=user_cast;_tmpD7.f4=(
void*)c;_tmpD7;});_tmpD6;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Throw_e_struct*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8[0]=({
struct Cyc_Absyn_Throw_e_struct _tmpD9;_tmpD9.tag=12;_tmpD9.f1=e;_tmpD9;});_tmpD8;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Address_e_struct*_tmpDA=
_cycalloc(sizeof(*_tmpDA));_tmpDA[0]=({struct Cyc_Absyn_Address_e_struct _tmpDB;
_tmpDB.tag=16;_tmpDB.f1=e;_tmpDB;});_tmpDA;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Sizeoftyp_e_struct*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC[0]=({
struct Cyc_Absyn_Sizeoftyp_e_struct _tmpDD;_tmpDD.tag=18;_tmpDD.f1=(void*)t;_tmpDD;});
_tmpDC;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Sizeofexp_e_struct*
_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE[0]=({struct Cyc_Absyn_Sizeofexp_e_struct
_tmpDF;_tmpDF.tag=19;_tmpDF.f1=e;_tmpDF;});_tmpDE;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_offsetof_exp(void*t,void*of,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmpE0=_cycalloc(sizeof(*_tmpE0));
_tmpE0[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmpE1;_tmpE1.tag=20;_tmpE1.f1=(
void*)t;_tmpE1.f2=(void*)of;_tmpE1;});_tmpE0;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(
struct Cyc_List_List*tvs,void*t,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_Gentyp_e_struct*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2[
0]=({struct Cyc_Absyn_Gentyp_e_struct _tmpE3;_tmpE3.tag=21;_tmpE3.f1=tvs;_tmpE3.f2=(
void*)t;_tmpE3;});_tmpE2;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Deref_e_struct*_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4[0]=({
struct Cyc_Absyn_Deref_e_struct _tmpE5;_tmpE5.tag=22;_tmpE5.f1=e;_tmpE5;});_tmpE4;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct
_dyneither_ptr*n,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_AggrMember_e_struct*_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6[0]=({
struct Cyc_Absyn_AggrMember_e_struct _tmpE7;_tmpE7.tag=23;_tmpE7.f1=e;_tmpE7.f2=n;
_tmpE7.f3=0;_tmpE7.f4=0;_tmpE7;});_tmpE6;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AggrArrow_e_struct*_tmpE8=
_cycalloc(sizeof(*_tmpE8));_tmpE8[0]=({struct Cyc_Absyn_AggrArrow_e_struct _tmpE9;
_tmpE9.tag=24;_tmpE9.f1=e;_tmpE9.f2=n;_tmpE9.f3=0;_tmpE9.f4=0;_tmpE9;});_tmpE8;}),
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_Subscript_e_struct*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA[0]=({
struct Cyc_Absyn_Subscript_e_struct _tmpEB;_tmpEB.tag=25;_tmpEB.f1=e1;_tmpEB.f2=e2;
_tmpEB;});_tmpEA;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*
es,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Tuple_e_struct*
_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC[0]=({struct Cyc_Absyn_Tuple_e_struct
_tmpED;_tmpED.tag=26;_tmpED.f1=es;_tmpED;});_tmpEC;}),loc);}struct Cyc_Absyn_Exp*
Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_StmtExp_e_struct*_tmpEE=_cycalloc(
sizeof(*_tmpEE));_tmpEE[0]=({struct Cyc_Absyn_StmtExp_e_struct _tmpEF;_tmpEF.tag=
38;_tmpEF.f1=s;_tmpEF;});_tmpEE;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(
struct Cyc_Position_Segment*loc){return Cyc_Absyn_var_exp(Cyc_Absyn_Match_Exception_name,
loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*t,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Valueof_e_struct*_tmpF0=
_cycalloc(sizeof(*_tmpF0));_tmpF0[0]=({struct Cyc_Absyn_Valueof_e_struct _tmpF1;
_tmpF1.tag=40;_tmpF1.f1=(void*)t;_tmpF1;});_tmpF0;}),loc);}struct _tuple19{struct
Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(
struct Cyc_List_List*es,struct Cyc_Position_Segment*loc){struct Cyc_List_List*dles=
0;for(0;es != 0;es=es->tl){dles=({struct Cyc_List_List*_tmpF2=_cycalloc(sizeof(*
_tmpF2));_tmpF2->hd=({struct _tuple19*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3->f1=
0;_tmpF3->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmpF3;});_tmpF2->tl=dles;_tmpF2;});}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Array_e_struct*_tmpF4=_cycalloc(
sizeof(*_tmpF4));_tmpF4[0]=({struct Cyc_Absyn_Array_e_struct _tmpF5;_tmpF5.tag=28;
_tmpF5.f1=dles;_tmpF5;});_tmpF4;}),loc);}struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpF6=
_cycalloc(sizeof(*_tmpF6));_tmpF6[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpF7;_tmpF7.tag=37;_tmpF7.f1=n;_tmpF7.f2=dles;_tmpF7;});_tmpF6;}),loc);}struct
Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc){return({
struct Cyc_Absyn_Stmt*_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8->r=s;_tmpF8->loc=
loc;_tmpF8->non_local_preds=0;_tmpF8->try_depth=0;_tmpF8->annot=(void*)Cyc_Absyn_EmptyAnnot;
_tmpF8;});}struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)0,loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Exp_s_struct*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9[0]=({
struct Cyc_Absyn_Exp_s_struct _tmpFA;_tmpFA.tag=0;_tmpFA.f1=e;_tmpFA;});_tmpF9;}),
loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*ss,struct Cyc_Position_Segment*
loc){if(ss == 0)return Cyc_Absyn_skip_stmt(loc);else{if(ss->tl == 0)return(struct
Cyc_Absyn_Stmt*)ss->hd;else{return Cyc_Absyn_seq_stmt((struct Cyc_Absyn_Stmt*)ss->hd,
Cyc_Absyn_seq_stmts(ss->tl,loc),loc);}}}struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Return_s_struct*_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB[
0]=({struct Cyc_Absyn_Return_s_struct _tmpFC;_tmpFC.tag=2;_tmpFC.f1=e;_tmpFC;});
_tmpFB;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_IfThenElse_s_struct*_tmpFD=
_cycalloc(sizeof(*_tmpFD));_tmpFD[0]=({struct Cyc_Absyn_IfThenElse_s_struct _tmpFE;
_tmpFE.tag=3;_tmpFE.f1=e;_tmpFE.f2=s1;_tmpFE.f3=s2;_tmpFE;});_tmpFD;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_While_s_struct*
_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF[0]=({struct Cyc_Absyn_While_s_struct
_tmp100;_tmp100.tag=4;_tmp100.f1=({struct _tuple8 _tmp101;_tmp101.f1=e;_tmp101.f2=
Cyc_Absyn_skip_stmt(e->loc);_tmp101;});_tmp100.f2=s;_tmp100;});_tmpFF;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(struct Cyc_Position_Segment*loc){return
Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Break_s_struct*_tmp102=_cycalloc(
sizeof(*_tmp102));_tmp102[0]=({struct Cyc_Absyn_Break_s_struct _tmp103;_tmp103.tag=
5;_tmp103.f1=0;_tmp103;});_tmp102;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Continue_s_struct*
_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104[0]=({struct Cyc_Absyn_Continue_s_struct
_tmp105;_tmp105.tag=6;_tmp105.f1=0;_tmp105;});_tmp104;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*
e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_For_s_struct*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106[
0]=({struct Cyc_Absyn_For_s_struct _tmp107;_tmp107.tag=8;_tmp107.f1=e1;_tmp107.f2=({
struct _tuple8 _tmp108;_tmp108.f1=e2;_tmp108.f2=Cyc_Absyn_skip_stmt(e3->loc);
_tmp108;});_tmp107.f3=({struct _tuple8 _tmp109;_tmp109.f1=e3;_tmp109.f2=Cyc_Absyn_skip_stmt(
e3->loc);_tmp109;});_tmp107.f4=s;_tmp107;});_tmp106;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_struct*_tmp10A=
_cycalloc(sizeof(*_tmp10A));_tmp10A[0]=({struct Cyc_Absyn_Switch_s_struct _tmp10B;
_tmp10B.tag=9;_tmp10B.f1=e;_tmp10B.f2=scs;_tmp10B;});_tmp10A;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*
loc){struct _tuple15 _tmp10D=({struct _tuple15 _tmp10C;_tmp10C.f1=s1->r;_tmp10C.f2=
s2->r;_tmp10C;});void*_tmp10E;void*_tmp10F;_LL69: _tmp10E=_tmp10D.f1;if((int)
_tmp10E != 0)goto _LL6B;_LL6A: return s2;_LL6B: _tmp10F=_tmp10D.f2;if((int)_tmp10F != 
0)goto _LL6D;_LL6C: return s1;_LL6D:;_LL6E: return Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Seq_s_struct*_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110[0]=({struct
Cyc_Absyn_Seq_s_struct _tmp111;_tmp111.tag=1;_tmp111.f1=s1;_tmp111.f2=s2;_tmp111;});
_tmp110;}),loc);_LL68:;}struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*
el,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Fallthru_s_struct*
_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112[0]=({struct Cyc_Absyn_Fallthru_s_struct
_tmp113;_tmp113.tag=10;_tmp113.f1=el;_tmp113.f2=0;_tmp113;});_tmp112;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*
s,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114[0]=({struct Cyc_Absyn_Decl_s_struct
_tmp115;_tmp115.tag=11;_tmp115.f1=d;_tmp115.f2=s;_tmp115;});_tmp114;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Decl*
d=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*_tmp118=_cycalloc(
sizeof(*_tmp118));_tmp118[0]=({struct Cyc_Absyn_Var_d_struct _tmp119;_tmp119.tag=0;
_tmp119.f1=Cyc_Absyn_new_vardecl(x,t,init);_tmp119;});_tmp118;}),loc);return Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Decl_s_struct*_tmp116=_cycalloc(sizeof(*_tmp116));
_tmp116[0]=({struct Cyc_Absyn_Decl_s_struct _tmp117;_tmp117.tag=11;_tmp117.f1=d;
_tmp117.f2=s;_tmp117;});_tmp116;}),loc);}struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(
struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc){
return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Label_s_struct*_tmp11A=
_cycalloc(sizeof(*_tmp11A));_tmp11A[0]=({struct Cyc_Absyn_Label_s_struct _tmp11B;
_tmp11B.tag=12;_tmp11B.f1=v;_tmp11B.f2=s;_tmp11B;});_tmp11A;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Do_s_struct*_tmp11C=
_cycalloc(sizeof(*_tmp11C));_tmp11C[0]=({struct Cyc_Absyn_Do_s_struct _tmp11D;
_tmp11D.tag=13;_tmp11D.f1=s;_tmp11D.f2=({struct _tuple8 _tmp11E;_tmp11E.f1=e;
_tmp11E.f2=Cyc_Absyn_skip_stmt(e->loc);_tmp11E;});_tmp11D;});_tmp11C;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
scs,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_TryCatch_s_struct*
_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F[0]=({struct Cyc_Absyn_TryCatch_s_struct
_tmp120;_tmp120.tag=14;_tmp120.f1=s;_tmp120.f2=scs;_tmp120;});_tmp11F;}),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Goto_s_struct*_tmp121=
_cycalloc(sizeof(*_tmp121));_tmp121[0]=({struct Cyc_Absyn_Goto_s_struct _tmp122;
_tmp122.tag=7;_tmp122.f1=lab;_tmp122.f2=0;_tmp122;});_tmp121;}),loc);}struct Cyc_Absyn_Stmt*
Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(e1,e2,loc),loc);}struct Cyc_Absyn_Pat*
Cyc_Absyn_new_pat(void*p,struct Cyc_Position_Segment*s){return({struct Cyc_Absyn_Pat*
_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->r=p;_tmp123->topt=0;_tmp123->loc=s;
_tmp123;});}struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e){return
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_struct*_tmp124=_cycalloc(
sizeof(*_tmp124));_tmp124[0]=({struct Cyc_Absyn_Exp_p_struct _tmp125;_tmp125.tag=
14;_tmp125.f1=e;_tmp125;});_tmp124;}),e->loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(
void*r,struct Cyc_Position_Segment*loc){return({struct Cyc_Absyn_Decl*_tmp126=
_cycalloc(sizeof(*_tmp126));_tmp126->r=r;_tmp126->loc=loc;_tmp126;});}struct Cyc_Absyn_Decl*
Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Let_d_struct*_tmp127=
_cycalloc(sizeof(*_tmp127));_tmp127[0]=({struct Cyc_Absyn_Let_d_struct _tmp128;
_tmp128.tag=2;_tmp128.f1=p;_tmp128.f2=0;_tmp128.f3=e;_tmp128;});_tmp127;}),loc);}
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Letv_d_struct*_tmp129=
_cycalloc(sizeof(*_tmp129));_tmp129[0]=({struct Cyc_Absyn_Letv_d_struct _tmp12A;
_tmp12A.tag=3;_tmp12A.f1=vds;_tmp12A;});_tmp129;}),loc);}struct Cyc_Absyn_Decl*
Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,int
resetable,struct Cyc_Absyn_Exp*eo,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Region_d_struct*_tmp12B=_cycalloc(sizeof(*_tmp12B));
_tmp12B[0]=({struct Cyc_Absyn_Region_d_struct _tmp12C;_tmp12C.tag=4;_tmp12C.f1=tv;
_tmp12C.f2=vd;_tmp12C.f3=resetable;_tmp12C.f4=eo;_tmp12C;});_tmp12B;}),loc);}
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Tvar*
tv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Alias_d_struct*_tmp12D=_cycalloc(sizeof(*_tmp12D));
_tmp12D[0]=({struct Cyc_Absyn_Alias_d_struct _tmp12E;_tmp12E.tag=5;_tmp12E.f1=e;
_tmp12E.f2=tv;_tmp12E.f3=vd;_tmp12E;});_tmp12D;}),loc);}struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){return({
struct Cyc_Absyn_Vardecl*_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F->sc=(void*)2;
_tmp12F->name=x;_tmp12F->tq=Cyc_Absyn_empty_tqual(0);_tmp12F->type=t;_tmp12F->initializer=
init;_tmp12F->rgn=0;_tmp12F->attributes=0;_tmp12F->escapes=0;_tmp12F;});}struct
Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*
init){return({struct Cyc_Absyn_Vardecl*_tmp130=_cycalloc(sizeof(*_tmp130));
_tmp130->sc=(void*)0;_tmp130->name=x;_tmp130->tq=Cyc_Absyn_empty_tqual(0);
_tmp130->type=t;_tmp130->initializer=init;_tmp130->rgn=0;_tmp130->attributes=0;
_tmp130->escapes=0;_tmp130;});}struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(
struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int
tagged){return({struct Cyc_Absyn_AggrdeclImpl*_tmp131=_cycalloc(sizeof(*_tmp131));
_tmp131->exist_vars=exists;_tmp131->rgn_po=po;_tmp131->fields=fs;_tmp131->tagged=
tagged;_tmp131;});}struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*k,void*s,struct
_tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_struct*
_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132[0]=({struct Cyc_Absyn_Aggr_d_struct
_tmp133;_tmp133.tag=6;_tmp133.f1=({struct Cyc_Absyn_Aggrdecl*_tmp134=_cycalloc(
sizeof(*_tmp134));_tmp134->kind=k;_tmp134->sc=s;_tmp134->name=n;_tmp134->tvs=ts;
_tmp134->impl=i;_tmp134->attributes=atts;_tmp134;});_tmp133;});_tmp132;}),loc);}
struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(void*s,struct _tuple0*n,struct Cyc_List_List*
ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*
loc){return Cyc_Absyn_aggr_decl((void*)0,s,n,ts,i,atts,loc);}struct Cyc_Absyn_Decl*
Cyc_Absyn_union_decl(void*s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*
i,struct Cyc_List_List*atts,struct Cyc_Position_Segment*loc){return Cyc_Absyn_aggr_decl((
void*)1,s,n,ts,i,atts,loc);}struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(void*s,
struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,
struct Cyc_Position_Segment*loc){return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Datatype_d_struct*
_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135[0]=({struct Cyc_Absyn_Datatype_d_struct
_tmp136;_tmp136.tag=7;_tmp136.f1=({struct Cyc_Absyn_Datatypedecl*_tmp137=
_cycalloc(sizeof(*_tmp137));_tmp137->sc=s;_tmp137->name=n;_tmp137->tvs=ts;
_tmp137->fields=fs;_tmp137->is_extensible=is_extensible;_tmp137;});_tmp136;});
_tmp135;}),loc);}static struct _tuple7*Cyc_Absyn_expand_arg(struct _tuple7*a){
return({struct _tuple7*_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138->f1=(*a).f1;
_tmp138->f2=(*a).f2;_tmp138->f3=Cyc_Absyn_pointer_expand((*a).f3,1);_tmp138;});}
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,
void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*
cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts){return(void*)({
struct Cyc_Absyn_FnType_struct*_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139[0]=({
struct Cyc_Absyn_FnType_struct _tmp13A;_tmp13A.tag=8;_tmp13A.f1=({struct Cyc_Absyn_FnInfo
_tmp13B;_tmp13B.tvars=tvs;_tmp13B.ret_typ=Cyc_Absyn_pointer_expand(ret_typ,0);
_tmp13B.effect=eff_typ;_tmp13B.args=((struct Cyc_List_List*(*)(struct _tuple7*(*f)(
struct _tuple7*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absyn_expand_arg,args);
_tmp13B.c_varargs=c_varargs;_tmp13B.cyc_varargs=cyc_varargs;_tmp13B.rgn_po=
rgn_po;_tmp13B.attributes=atts;_tmp13B;});_tmp13A;});_tmp139;});}void*Cyc_Absyn_pointer_expand(
void*t,int fresh_evar){void*_tmp13C=Cyc_Tcutil_compress(t);_LL70: if(_tmp13C <= (
void*)4)goto _LL72;if(*((int*)_tmp13C)!= 8)goto _LL72;_LL71: return Cyc_Absyn_at_typ(
t,fresh_evar?Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp13D=_cycalloc(sizeof(*
_tmp13D));_tmp13D->v=(void*)((void*)3);_tmp13D;}),0):(void*)2,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_false_conref);_LL72:;_LL73: return t;_LL6F:;}int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*e){void*_tmp13E=e->r;void*_tmp13F;void*_tmp140;struct Cyc_Absyn_Vardecl*
_tmp141;void*_tmp142;struct Cyc_Absyn_Vardecl*_tmp143;struct Cyc_Absyn_Exp*_tmp144;
struct Cyc_Absyn_Exp*_tmp145;struct Cyc_Absyn_Exp*_tmp146;_LL75: if(*((int*)_tmp13E)
!= 1)goto _LL77;_tmp13F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp13E)->f2;if(
_tmp13F <= (void*)1)goto _LL77;if(*((int*)_tmp13F)!= 1)goto _LL77;_LL76: return 0;
_LL77: if(*((int*)_tmp13E)!= 1)goto _LL79;_tmp140=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp13E)->f2;if(_tmp140 <= (void*)1)goto _LL79;if(*((int*)_tmp140)!= 0)goto _LL79;
_tmp141=((struct Cyc_Absyn_Global_b_struct*)_tmp140)->f1;_LL78: _tmp143=_tmp141;
goto _LL7A;_LL79: if(*((int*)_tmp13E)!= 1)goto _LL7B;_tmp142=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp13E)->f2;if(_tmp142 <= (void*)1)goto _LL7B;if(*((int*)_tmp142)!= 3)goto _LL7B;
_tmp143=((struct Cyc_Absyn_Local_b_struct*)_tmp142)->f1;_LL7A: {void*_tmp147=Cyc_Tcutil_compress(
_tmp143->type);_LL8C: if(_tmp147 <= (void*)4)goto _LL8E;if(*((int*)_tmp147)!= 7)
goto _LL8E;_LL8D: return 0;_LL8E:;_LL8F: return 1;_LL8B:;}_LL7B: if(*((int*)_tmp13E)!= 
1)goto _LL7D;_LL7C: goto _LL7E;_LL7D: if(*((int*)_tmp13E)!= 24)goto _LL7F;_LL7E: goto
_LL80;_LL7F: if(*((int*)_tmp13E)!= 22)goto _LL81;_LL80: goto _LL82;_LL81: if(*((int*)
_tmp13E)!= 25)goto _LL83;_LL82: return 1;_LL83: if(*((int*)_tmp13E)!= 23)goto _LL85;
_tmp144=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp13E)->f1;_LL84: return Cyc_Absyn_is_lvalue(
_tmp144);_LL85: if(*((int*)_tmp13E)!= 14)goto _LL87;_tmp145=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp13E)->f1;_LL86: return Cyc_Absyn_is_lvalue(_tmp145);_LL87: if(*((int*)_tmp13E)
!= 13)goto _LL89;_tmp146=((struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp13E)->f1;
_LL88: return Cyc_Absyn_is_lvalue(_tmp146);_LL89:;_LL8A: return 0;_LL74:;}struct Cyc_Absyn_Aggrfield*
Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct _dyneither_ptr*v){{
struct Cyc_List_List*_tmp148=fields;for(0;_tmp148 != 0;_tmp148=_tmp148->tl){if(Cyc_strptrcmp(((
struct Cyc_Absyn_Aggrfield*)_tmp148->hd)->name,v)== 0)return(struct Cyc_Absyn_Aggrfield*)((
struct Cyc_Absyn_Aggrfield*)_tmp148->hd);}}return 0;}struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*v){return ad->impl == 0?0: Cyc_Absyn_lookup_field(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}struct _tuple9*
Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*ts,int i){for(0;i != 0;-- i){if(ts
== 0)return 0;ts=ts->tl;}if(ts == 0)return 0;return(struct _tuple9*)((struct _tuple9*)
ts->hd);}struct _dyneither_ptr Cyc_Absyn_attribute2string(void*a){void*_tmp149=a;
int _tmp14A;int _tmp14B;struct _dyneither_ptr _tmp14C;void*_tmp14D;int _tmp14E;int
_tmp14F;void*_tmp150;int _tmp151;int _tmp152;int _tmp153;struct _dyneither_ptr
_tmp154;_LL91: if(_tmp149 <= (void*)17)goto _LL93;if(*((int*)_tmp149)!= 0)goto _LL93;
_tmp14A=((struct Cyc_Absyn_Regparm_att_struct*)_tmp149)->f1;_LL92: return(struct
_dyneither_ptr)({struct Cyc_Int_pa_struct _tmp157;_tmp157.tag=1;_tmp157.f1=(
unsigned long)_tmp14A;{void*_tmp155[1]={& _tmp157};Cyc_aprintf(({const char*
_tmp156="regparm(%d)";_tag_dyneither(_tmp156,sizeof(char),12);}),_tag_dyneither(
_tmp155,sizeof(void*),1));}});_LL93: if((int)_tmp149 != 0)goto _LL95;_LL94: return({
const char*_tmp158="stdcall";_tag_dyneither(_tmp158,sizeof(char),8);});_LL95: if((
int)_tmp149 != 1)goto _LL97;_LL96: return({const char*_tmp159="cdecl";_tag_dyneither(
_tmp159,sizeof(char),6);});_LL97: if((int)_tmp149 != 2)goto _LL99;_LL98: return({
const char*_tmp15A="fastcall";_tag_dyneither(_tmp15A,sizeof(char),9);});_LL99: if((
int)_tmp149 != 3)goto _LL9B;_LL9A: return({const char*_tmp15B="noreturn";
_tag_dyneither(_tmp15B,sizeof(char),9);});_LL9B: if((int)_tmp149 != 4)goto _LL9D;
_LL9C: return({const char*_tmp15C="const";_tag_dyneither(_tmp15C,sizeof(char),6);});
_LL9D: if(_tmp149 <= (void*)17)goto _LL9F;if(*((int*)_tmp149)!= 1)goto _LL9F;_tmp14B=((
struct Cyc_Absyn_Aligned_att_struct*)_tmp149)->f1;_LL9E: if(_tmp14B == - 1)return({
const char*_tmp15D="aligned";_tag_dyneither(_tmp15D,sizeof(char),8);});else{void*
_tmp15E=Cyc_Cyclone_c_compiler;_LLC2: if((int)_tmp15E != 0)goto _LLC4;_LLC3: return(
struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp161;_tmp161.tag=1;_tmp161.f1=(
unsigned long)_tmp14B;{void*_tmp15F[1]={& _tmp161};Cyc_aprintf(({const char*
_tmp160="aligned(%d)";_tag_dyneither(_tmp160,sizeof(char),12);}),_tag_dyneither(
_tmp15F,sizeof(void*),1));}});_LLC4: if((int)_tmp15E != 1)goto _LLC1;_LLC5: return(
struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp164;_tmp164.tag=1;_tmp164.f1=(
unsigned long)_tmp14B;{void*_tmp162[1]={& _tmp164};Cyc_aprintf(({const char*
_tmp163="align(%d)";_tag_dyneither(_tmp163,sizeof(char),10);}),_tag_dyneither(
_tmp162,sizeof(void*),1));}});_LLC1:;}_LL9F: if((int)_tmp149 != 5)goto _LLA1;_LLA0:
return({const char*_tmp165="packed";_tag_dyneither(_tmp165,sizeof(char),7);});
_LLA1: if(_tmp149 <= (void*)17)goto _LLA3;if(*((int*)_tmp149)!= 2)goto _LLA3;_tmp14C=((
struct Cyc_Absyn_Section_att_struct*)_tmp149)->f1;_LLA2: return(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp168;_tmp168.tag=0;_tmp168.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp14C);{void*_tmp166[1]={& _tmp168};
Cyc_aprintf(({const char*_tmp167="section(\"%s\")";_tag_dyneither(_tmp167,sizeof(
char),14);}),_tag_dyneither(_tmp166,sizeof(void*),1));}});_LLA3: if((int)_tmp149
!= 6)goto _LLA5;_LLA4: return({const char*_tmp169="nocommon";_tag_dyneither(_tmp169,
sizeof(char),9);});_LLA5: if((int)_tmp149 != 7)goto _LLA7;_LLA6: return({const char*
_tmp16A="shared";_tag_dyneither(_tmp16A,sizeof(char),7);});_LLA7: if((int)_tmp149
!= 8)goto _LLA9;_LLA8: return({const char*_tmp16B="unused";_tag_dyneither(_tmp16B,
sizeof(char),7);});_LLA9: if((int)_tmp149 != 9)goto _LLAB;_LLAA: return({const char*
_tmp16C="weak";_tag_dyneither(_tmp16C,sizeof(char),5);});_LLAB: if((int)_tmp149 != 
10)goto _LLAD;_LLAC: return({const char*_tmp16D="dllimport";_tag_dyneither(_tmp16D,
sizeof(char),10);});_LLAD: if((int)_tmp149 != 11)goto _LLAF;_LLAE: return({const char*
_tmp16E="dllexport";_tag_dyneither(_tmp16E,sizeof(char),10);});_LLAF: if((int)
_tmp149 != 12)goto _LLB1;_LLB0: return({const char*_tmp16F="no_instrument_function";
_tag_dyneither(_tmp16F,sizeof(char),23);});_LLB1: if((int)_tmp149 != 13)goto _LLB3;
_LLB2: return({const char*_tmp170="constructor";_tag_dyneither(_tmp170,sizeof(char),
12);});_LLB3: if((int)_tmp149 != 14)goto _LLB5;_LLB4: return({const char*_tmp171="destructor";
_tag_dyneither(_tmp171,sizeof(char),11);});_LLB5: if((int)_tmp149 != 15)goto _LLB7;
_LLB6: return({const char*_tmp172="no_check_memory_usage";_tag_dyneither(_tmp172,
sizeof(char),22);});_LLB7: if(_tmp149 <= (void*)17)goto _LLBD;if(*((int*)_tmp149)!= 
3)goto _LLB9;_tmp14D=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp149)->f1;if((
int)_tmp14D != 0)goto _LLB9;_tmp14E=((struct Cyc_Absyn_Format_att_struct*)_tmp149)->f2;
_tmp14F=((struct Cyc_Absyn_Format_att_struct*)_tmp149)->f3;_LLB8: return(struct
_dyneither_ptr)({struct Cyc_Int_pa_struct _tmp176;_tmp176.tag=1;_tmp176.f1=(
unsigned int)_tmp14F;{struct Cyc_Int_pa_struct _tmp175;_tmp175.tag=1;_tmp175.f1=(
unsigned int)_tmp14E;{void*_tmp173[2]={& _tmp175,& _tmp176};Cyc_aprintf(({const
char*_tmp174="format(printf,%u,%u)";_tag_dyneither(_tmp174,sizeof(char),21);}),
_tag_dyneither(_tmp173,sizeof(void*),2));}}});_LLB9: if(*((int*)_tmp149)!= 3)goto
_LLBB;_tmp150=(void*)((struct Cyc_Absyn_Format_att_struct*)_tmp149)->f1;if((int)
_tmp150 != 1)goto _LLBB;_tmp151=((struct Cyc_Absyn_Format_att_struct*)_tmp149)->f2;
_tmp152=((struct Cyc_Absyn_Format_att_struct*)_tmp149)->f3;_LLBA: return(struct
_dyneither_ptr)({struct Cyc_Int_pa_struct _tmp17A;_tmp17A.tag=1;_tmp17A.f1=(
unsigned int)_tmp152;{struct Cyc_Int_pa_struct _tmp179;_tmp179.tag=1;_tmp179.f1=(
unsigned int)_tmp151;{void*_tmp177[2]={& _tmp179,& _tmp17A};Cyc_aprintf(({const
char*_tmp178="format(scanf,%u,%u)";_tag_dyneither(_tmp178,sizeof(char),20);}),
_tag_dyneither(_tmp177,sizeof(void*),2));}}});_LLBB: if(*((int*)_tmp149)!= 4)goto
_LLBD;_tmp153=((struct Cyc_Absyn_Initializes_att_struct*)_tmp149)->f1;_LLBC:
return(struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp17D;_tmp17D.tag=1;
_tmp17D.f1=(unsigned long)_tmp153;{void*_tmp17B[1]={& _tmp17D};Cyc_aprintf(({
const char*_tmp17C="initializes(%d)";_tag_dyneither(_tmp17C,sizeof(char),16);}),
_tag_dyneither(_tmp17B,sizeof(void*),1));}});_LLBD: if((int)_tmp149 != 16)goto
_LLBF;_LLBE: return({const char*_tmp17E="pure";_tag_dyneither(_tmp17E,sizeof(char),
5);});_LLBF: if(_tmp149 <= (void*)17)goto _LL90;if(*((int*)_tmp149)!= 5)goto _LL90;
_tmp154=((struct Cyc_Absyn_Mode_att_struct*)_tmp149)->f1;_LLC0: return(struct
_dyneither_ptr)({struct Cyc_String_pa_struct _tmp181;_tmp181.tag=0;_tmp181.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)_tmp154);{void*_tmp17F[1]={& _tmp181};
Cyc_aprintf(({const char*_tmp180="__mode__(\"%s\")";_tag_dyneither(_tmp180,
sizeof(char),15);}),_tag_dyneither(_tmp17F,sizeof(void*),1));}});_LL90:;}int Cyc_Absyn_fntype_att(
void*a){void*_tmp182=a;_LLC7: if(_tmp182 <= (void*)17)goto _LLC9;if(*((int*)_tmp182)
!= 0)goto _LLC9;_LLC8: goto _LLCA;_LLC9: if((int)_tmp182 != 2)goto _LLCB;_LLCA: goto
_LLCC;_LLCB: if((int)_tmp182 != 0)goto _LLCD;_LLCC: goto _LLCE;_LLCD: if((int)_tmp182
!= 1)goto _LLCF;_LLCE: goto _LLD0;_LLCF: if((int)_tmp182 != 3)goto _LLD1;_LLD0: goto
_LLD2;_LLD1: if((int)_tmp182 != 16)goto _LLD3;_LLD2: goto _LLD4;_LLD3: if(_tmp182 <= (
void*)17)goto _LLD5;if(*((int*)_tmp182)!= 3)goto _LLD5;_LLD4: goto _LLD6;_LLD5: if((
int)_tmp182 != 4)goto _LLD7;_LLD6: return 1;_LLD7: if(_tmp182 <= (void*)17)goto _LLD9;
if(*((int*)_tmp182)!= 4)goto _LLD9;_LLD8: return 1;_LLD9:;_LLDA: return 0;_LLC6:;}
static char _tmp183[3]="f0";static struct _dyneither_ptr Cyc_Absyn_f0={_tmp183,
_tmp183,_tmp183 + 3};static struct _dyneither_ptr*Cyc_Absyn_field_names_v[1]={& Cyc_Absyn_f0};
static struct _dyneither_ptr Cyc_Absyn_field_names={(void*)((struct _dyneither_ptr**)
Cyc_Absyn_field_names_v),(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(
void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v + 1)};struct _dyneither_ptr*
Cyc_Absyn_fieldname(int i){unsigned int fsz=_get_dyneither_size(Cyc_Absyn_field_names,
sizeof(struct _dyneither_ptr*));if(i >= fsz)Cyc_Absyn_field_names=({unsigned int
_tmp184=(unsigned int)(i + 1);struct _dyneither_ptr**_tmp185=(struct _dyneither_ptr**)
_cycalloc(_check_times(sizeof(struct _dyneither_ptr*),_tmp184));struct
_dyneither_ptr _tmp18B=_tag_dyneither(_tmp185,sizeof(struct _dyneither_ptr*),
_tmp184);{unsigned int _tmp186=_tmp184;unsigned int j;for(j=0;j < _tmp186;j ++){
_tmp185[j]=j < fsz?*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,
sizeof(struct _dyneither_ptr*),(int)j)):({struct _dyneither_ptr*_tmp187=_cycalloc(
sizeof(*_tmp187));_tmp187[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_struct
_tmp18A;_tmp18A.tag=1;_tmp18A.f1=(unsigned long)((int)j);{void*_tmp188[1]={&
_tmp18A};Cyc_aprintf(({const char*_tmp189="f%d";_tag_dyneither(_tmp189,sizeof(
char),4);}),_tag_dyneither(_tmp188,sizeof(void*),1));}});_tmp187;});}}_tmp18B;});
return*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,
sizeof(struct _dyneither_ptr*),i));}struct _tuple10 Cyc_Absyn_aggr_kinded_name(
union Cyc_Absyn_AggrInfoU info){union Cyc_Absyn_AggrInfoU _tmp18C=info;struct _tuple2
_tmp18D;void*_tmp18E;struct _tuple0*_tmp18F;struct Cyc_Absyn_Aggrdecl**_tmp190;
struct Cyc_Absyn_Aggrdecl*_tmp191;struct Cyc_Absyn_Aggrdecl _tmp192;void*_tmp193;
struct _tuple0*_tmp194;_LLDC: if((_tmp18C.UnknownAggr).tag != 1)goto _LLDE;_tmp18D=(
struct _tuple2)(_tmp18C.UnknownAggr).val;_tmp18E=_tmp18D.f1;_tmp18F=_tmp18D.f2;
_LLDD: return({struct _tuple10 _tmp195;_tmp195.f1=_tmp18E;_tmp195.f2=_tmp18F;
_tmp195;});_LLDE: if((_tmp18C.KnownAggr).tag != 2)goto _LLDB;_tmp190=(struct Cyc_Absyn_Aggrdecl**)(
_tmp18C.KnownAggr).val;_tmp191=*_tmp190;_tmp192=*_tmp191;_tmp193=_tmp192.kind;
_tmp194=_tmp192.name;_LLDF: return({struct _tuple10 _tmp196;_tmp196.f1=_tmp193;
_tmp196.f2=_tmp194;_tmp196;});_LLDB:;}struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU info){union Cyc_Absyn_AggrInfoU _tmp197=info;struct _tuple2
_tmp198;struct Cyc_Absyn_Aggrdecl**_tmp199;struct Cyc_Absyn_Aggrdecl*_tmp19A;_LLE1:
if((_tmp197.UnknownAggr).tag != 1)goto _LLE3;_tmp198=(struct _tuple2)(_tmp197.UnknownAggr).val;
_LLE2:({void*_tmp19B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp19C="unchecked aggrdecl";_tag_dyneither(
_tmp19C,sizeof(char),19);}),_tag_dyneither(_tmp19B,sizeof(void*),0));});_LLE3:
if((_tmp197.KnownAggr).tag != 2)goto _LLE0;_tmp199=(struct Cyc_Absyn_Aggrdecl**)(
_tmp197.KnownAggr).val;_tmp19A=*_tmp199;_LLE4: return _tmp19A;_LLE0:;}int Cyc_Absyn_is_union_type(
void*t){void*_tmp19D=Cyc_Tcutil_compress(t);void*_tmp19E;struct Cyc_Absyn_AggrInfo
_tmp19F;union Cyc_Absyn_AggrInfoU _tmp1A0;_LLE6: if(_tmp19D <= (void*)4)goto _LLEA;
if(*((int*)_tmp19D)!= 11)goto _LLE8;_tmp19E=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp19D)->f1;if((int)_tmp19E != 1)goto _LLE8;_LLE7: return 1;_LLE8: if(*((int*)
_tmp19D)!= 10)goto _LLEA;_tmp19F=((struct Cyc_Absyn_AggrType_struct*)_tmp19D)->f1;
_tmp1A0=_tmp19F.aggr_info;_LLE9: return(Cyc_Absyn_aggr_kinded_name(_tmp1A0)).f1 == (
void*)1;_LLEA:;_LLEB: return 0;_LLE5:;}int Cyc_Absyn_is_nontagged_union_type(void*t){
void*_tmp1A1=Cyc_Tcutil_compress(t);void*_tmp1A2;struct Cyc_Absyn_AggrInfo _tmp1A3;
union Cyc_Absyn_AggrInfoU _tmp1A4;_LLED: if(_tmp1A1 <= (void*)4)goto _LLF1;if(*((int*)
_tmp1A1)!= 11)goto _LLEF;_tmp1A2=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1A1)->f1;if((int)_tmp1A2 != 1)goto _LLEF;_LLEE: return 1;_LLEF: if(*((int*)
_tmp1A1)!= 10)goto _LLF1;_tmp1A3=((struct Cyc_Absyn_AggrType_struct*)_tmp1A1)->f1;
_tmp1A4=_tmp1A3.aggr_info;_LLF0: {union Cyc_Absyn_AggrInfoU _tmp1A5=_tmp1A4;struct
Cyc_Absyn_Aggrdecl**_tmp1A6;struct Cyc_Absyn_Aggrdecl*_tmp1A7;struct _tuple2
_tmp1A8;void*_tmp1A9;struct Cyc_Core_Opt*_tmp1AA;struct _tuple2 _tmp1AB;void*
_tmp1AC;struct Cyc_Core_Opt*_tmp1AD;struct Cyc_Core_Opt _tmp1AE;int _tmp1AF;_LLF4:
if((_tmp1A5.KnownAggr).tag != 2)goto _LLF6;_tmp1A6=(struct Cyc_Absyn_Aggrdecl**)(
_tmp1A5.KnownAggr).val;_tmp1A7=*_tmp1A6;_LLF5: return _tmp1A7->kind == (void*)1  && ((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1A7->impl))->tagged == 0;_LLF6: if((
_tmp1A5.UnknownAggr).tag != 1)goto _LLF8;_tmp1A8=(struct _tuple2)(_tmp1A5.UnknownAggr).val;
_tmp1A9=_tmp1A8.f1;_tmp1AA=_tmp1A8.f3;if(_tmp1AA != 0)goto _LLF8;_LLF7: return
_tmp1A9 == (void*)1;_LLF8: if((_tmp1A5.UnknownAggr).tag != 1)goto _LLF3;_tmp1AB=(
struct _tuple2)(_tmp1A5.UnknownAggr).val;_tmp1AC=_tmp1AB.f1;_tmp1AD=_tmp1AB.f3;
if(_tmp1AD == 0)goto _LLF3;_tmp1AE=*_tmp1AD;_tmp1AF=(int)_tmp1AE.v;_LLF9: return
_tmp1AC == (void*)1  && !_tmp1AF;_LLF3:;}_LLF1:;_LLF2: return 0;_LLEC:;}int Cyc_Absyn_is_aggr_type(
void*t){void*_tmp1B0=Cyc_Tcutil_compress(t);_LLFB: if(_tmp1B0 <= (void*)4)goto
_LL105;if(*((int*)_tmp1B0)!= 2)goto _LLFD;_LLFC: goto _LLFE;_LLFD: if(*((int*)
_tmp1B0)!= 3)goto _LLFF;_LLFE: goto _LL100;_LLFF: if(*((int*)_tmp1B0)!= 9)goto _LL101;
_LL100: goto _LL102;_LL101: if(*((int*)_tmp1B0)!= 10)goto _LL103;_LL102: goto _LL104;
_LL103: if(*((int*)_tmp1B0)!= 11)goto _LL105;_LL104: return 1;_LL105:;_LL106: return 0;
_LLFA:;}int Cyc_Absyn_porting_c_code=0;int Cyc_Absyn_no_regions=0;
