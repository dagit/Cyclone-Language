#ifndef _SETJMP_H_
#define _SETJMP_H_
#ifndef _jmp_buf_def_
#define _jmp_buf_def_
typedef int jmp_buf[192];
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
struct _dynforward_ptr {
  unsigned char *curr;
  unsigned char *last_plus_one;
};

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
static _INLINE unsigned char *
_check_dynforward_subscript(struct _dynforward_ptr arr,unsigned elt_sz,unsigned index) {
  struct _dynforward_ptr _cus_arr = (arr);
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
#define _check_dynforward_subscript(arr,elt_sz,index) ({ \
  struct _dynforward_ptr _cus_arr = (arr); \
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
  if (!_cus_arr.base) _throw_null();
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one)
    _throw_arraybounds();
  return _cus_ans;
}
static _INLINE unsigned char *
_check_dynforward_subscript(struct _dynforward_ptr arr,unsigned elt_sz,unsigned index) {
  struct _dynforward_ptr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_curr = _cus_arr.curr;
  unsigned char *_cus_ans = _cus_curr + _cus_elt_sz * _cus_index;
  if (!_cus_arr.last_plus_one) _throw_null();
  if (_cus_ans < _cus_curr || _cus_ans >= _cus_arr.last_plus_one)
    _throw_arraybounds();
  return _cus_ans;
}
#else
#define _check_dyneither_subscript(arr,elt_sz,index) ({ \
  struct _dyneither_ptr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  if (!_cus_arr.base) _throw_null(); \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#define _check_dynforward_subscript(arr,elt_sz,index) ({ \
  struct _dynforward_ptr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_curr = _cus_arr.curr; \
  unsigned char *_cus_ans = _cus_curr + _cus_elt_sz * _cus_index; \
  if (!_cus_arr.last_plus_one) _throw_null(); \
  if (_cus_ans < _cus_curr || _cus_ans >= _cus_arr.last_plus_one) \
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
static _INLINE struct _dynforward_ptr
_tag_dynforward(const void *tcurr,unsigned elt_sz,unsigned num_elts) {
  struct _dynforward_ptr _tag_arr_ans;
  _tag_arr_ans.curr = (void*)(tcurr);
  _tag_arr_ans.last_plus_one = _tag_arr_ans.curr + (elt_sz) * (num_elts);
  return _tag_arr_ans;
}
#else
#define _tag_dyneither(tcurr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr _tag_arr_ans; \
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })
#define _tag_dynforward(tcurr,elt_sz,num_elts) ({ \
  struct _dynforward_ptr _tag_arr_ans; \
  _tag_arr_ans.curr = (void*)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.curr + (elt_sz) * (num_elts); \
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
static _INLINE struct _dynforward_ptr *
_init_dynforward_ptr(struct _dynforward_ptr *arr_ptr,
                    void *arr, unsigned elt_sz, unsigned num_elts) {
  struct _dynforward_ptr *_itarr_ptr = (arr_ptr);
  void* _itarr = (arr);
  _itarr_ptr->curr = _itarr;
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
#define _init_dynforward_ptr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _dynforward_ptr *_itarr_ptr = (arr_ptr); \
  void* _itarr = (arr); \
  _itarr_ptr->curr = _itarr; \
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts); \
  _itarr_ptr; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _untag_dynforward_ptr(arr,elt_sz,num_elts) ((arr).curr)
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
static _INLINE unsigned char *
_untag_dynforward_ptr(struct _dynforward_ptr arr, 
                      unsigned elt_sz,unsigned num_elts) {
  struct _dynforward_ptr _arr = (arr);
  unsigned char *_curr = _arr.curr;
  if (_curr + (elt_sz) * (num_elts) > _arr.last_plus_one)
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
#define _untag_dynforward_ptr(arr,elt_sz,num_elts) ({ \
  struct _dynforward_ptr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if (_curr + (elt_sz) * (num_elts) > _arr.last_plus_one)\
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
static _INLINE unsigned
_get_dynforward_size(struct _dynforward_ptr arr,unsigned elt_sz) {
  struct _dynforward_ptr _get_arr_size_temp = (arr);
  unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr;
  unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one;
  return (_get_arr_size_curr >= _get_arr_size_last) ? 0 :
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
#define _get_dynforward_size(arr,elt_sz) \
  ({struct _dynforward_ptr _get_arr_size_temp = (arr); \
    unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr; \
    unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one; \
    (_get_arr_size_curr >= _get_arr_size_last) ? 0 : \
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));})
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_dyneither_ptr_plus(struct _dyneither_ptr arr,unsigned elt_sz,int change) {
  struct _dyneither_ptr _ans = (arr);
  _ans.curr += ((int)(elt_sz))*(change);
  return _ans;
}
/* Here we have to worry about wrapping around, so if we go past the
 * end, we set the end to 0. */
static _INLINE struct _dynforward_ptr
_dynforward_ptr_plus(struct _dynforward_ptr arr,unsigned elt_sz,int change) {
  struct _dynforward_ptr _ans = (arr);
  unsigned int _dfpp_elts = (((unsigned)_ans.last_plus_one) - 
                             ((unsigned)_ans.curr)) / elt_sz;
  if (change < 0 || ((unsigned)change) > _dfpp_elts)
    _ans.last_plus_one = 0;
  _ans.curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _dyneither_ptr_plus(arr,elt_sz,change) ({ \
  struct _dyneither_ptr _ans = (arr); \
  _ans.curr += ((int)(elt_sz))*(change); \
  _ans; })
#define _dynforward_ptr_plus(arr,elt_sz,change) ({ \
  struct _dynforward_ptr _ans = (arr); \
  unsigned _dfpp_elt_sz = (elt_sz); \
  int _dfpp_change = (change); \
  unsigned int _dfpp_elts = (((unsigned)_ans.last_plus_one) - \
                            ((unsigned)_ans.curr)) / _dfpp_elt_sz; \
  if (_dfpp_change < 0 || ((unsigned)_dfpp_change) > _dfpp_elts) \
    _ans.last_plus_one = 0; \
  _ans.curr += ((int)(_dfpp_elt_sz))*(_dfpp_change); \
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
static _INLINE struct _dynforward_ptr
_dynforward_ptr_inplace_plus(struct _dynforward_ptr *arr_ptr,unsigned elt_sz,
                             int change) {
  struct _dynforward_ptr * _arr_ptr = (arr_ptr);
  unsigned int _dfpp_elts = (((unsigned)_arr_ptr->last_plus_one) - 
                             ((unsigned)_arr_ptr->curr)) / elt_sz;
  if (change < 0 || ((unsigned)change) > _dfpp_elts) 
    _arr_ptr->last_plus_one = 0;
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return *_arr_ptr;
}
#else
#define _dyneither_ptr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _dyneither_ptr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  *_arr_ptr; })
#define _dynforward_ptr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _dynforward_ptr * _arr_ptr = (arr_ptr); \
  unsigned _dfpp_elt_sz = (elt_sz); \
  int _dfpp_change = (change); \
  unsigned int _dfpp_elts = (((unsigned)_arr_ptr->last_plus_one) - \
                            ((unsigned)_arr_ptr->curr)) / _dfpp_elt_sz; \
  if (_dfpp_change < 0 || ((unsigned)_dfpp_change) > _dfpp_elts) \
    _arr_ptr->last_plus_one = 0; \
  _arr_ptr->curr += ((int)(_dfpp_elt_sz))*(_dfpp_change); \
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
static _INLINE struct _dynforward_ptr
_dynforward_ptr_inplace_plus_post(struct _dynforward_ptr *arr_ptr,unsigned elt_sz,int change) {
  struct _dynforward_ptr * _arr_ptr = (arr_ptr);
  struct _dynforward_ptr _ans = *_arr_ptr;
  unsigned int _dfpp_elts = (((unsigned)_arr_ptr->last_plus_one) - 
                            ((unsigned)_arr_ptr->curr)) / elt_sz; 
  if (change < 0 || ((unsigned)change) > _dfpp_elts) 
    _arr_ptr->last_plus_one = 0; 
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _dyneither_ptr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _dyneither_ptr * _arr_ptr = (arr_ptr); \
  struct _dyneither_ptr _ans = *_arr_ptr; \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  _ans; })
#define _dynforward_ptr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _dynforward_ptr * _arr_ptr = (arr_ptr); \
  struct _dynforward_ptr _ans = *_arr_ptr; \
  unsigned _dfpp_elt_sz = (elt_sz); \
  int _dfpp_change = (change); \
  unsigned int _dfpp_elts = (((unsigned)_arr_ptr->last_plus_one) - \
                            ((unsigned)_arr_ptr->curr)) / _dfpp_elt_sz; \
  if (_dfpp_change < 0 || ((unsigned)_dfpp_change) > _dfpp_elts) \
    _arr_ptr->last_plus_one = 0; \
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
static struct 
_dynforward_ptr _dynforward_ptr_decrease_size(struct _dynforward_ptr x,
                                            unsigned int sz,
                                            unsigned int numelts) {
  if (x.last_plus_one != 0)
    x.last_plus_one -= sz * numelts; 
  return x; 
}

/* Convert between the two forms of dynamic pointers */
#ifdef _INLINE_FUNCTIONS 
static struct _dynforward_ptr
_dyneither_to_dynforward(struct _dyneither_ptr p) {
  struct _dynforward_ptr res;
  res.curr = p.curr;
  res.last_plus_one = (p.base == 0) ? 0 : p.last_plus_one;
  return res;
}
static struct _dyneither_ptr
_dynforward_to_dyneither(struct _dynforward_ptr p) {
  struct _dyneither_ptr res;
  res.base = res.curr = p.curr;
  res.last_plus_one = p.last_plus_one;
  if (p.last_plus_one == 0) 
    res.base = 0;
  return res;
}
#else 
#define _dyneither_to_dynforward(_dnfptr) ({ \
  struct _dyneither_ptr _dnfp = (_dnfptr); \
  struct _dynforward_ptr _dnfpres; \
  _dnfpres.curr = _dnfp.curr; \
  _dnfpres.last_plus_one = (_dnfp.base == 0) ? 0 : _dnfp.last_plus_one; \
  _dnfpres; })
#define _dynforward_to_dyneither(_dfnptr) ({ \
  struct _dynforward_ptr _dfnp = (_dfnptr); \
  struct _dyneither_ptr _dfnres; \
  _dfnres.base = _dfnres.curr = _dfnp.curr; \
  _dfnres.last_plus_one = _dfnp.last_plus_one; \
  if (_dfnp.last_plus_one == 0) \
    _dfnres.base = 0; \
  _dfnres; })
#endif 

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

/* the next three routines swap [x] and [y]; not thread safe! */
static _INLINE void _swap_word(void *x, void *y) {
  unsigned long *lx = (unsigned long *)x, *ly = (unsigned long *)y, tmp;
  tmp = *lx;
  *lx = *ly;
  *ly = tmp;
}
static _INLINE void _swap_dynforward(struct _dynforward_ptr *x, 
				    struct _dynforward_ptr *y) {
  struct _dynforward_ptr tmp = *x;
  *x = *y;
  *y = tmp;
}
static _INLINE void _swap_dyneither(struct _dyneither_ptr *x, 
				   struct _dyneither_ptr *y) {
  struct _dyneither_ptr tmp = *x;
  *x = *y;
  *y = tmp;
}
 void exit(int);void*abort();struct Cyc_Core_NewRegion{struct _DynRegionHandle*
dynregion;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dynforward_ptr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[
16];struct _dynforward_ptr wrap_Cbuffer_as_buffer(char*,unsigned long);struct Cyc_in_addr{
unsigned int s_addr;};unsigned int Cyc_htonl(unsigned int x);unsigned short Cyc_htons(
unsigned short x);unsigned int Cyc_ntohl(unsigned int x);unsigned short Cyc_ntohs(
unsigned short x);unsigned int __stub_htonl(unsigned int x);unsigned short
__stub_htons(unsigned short x);unsigned int __stub_ntohl(unsigned int x);
unsigned short __stub_ntohs(unsigned short x);unsigned int Cyc_htonl(unsigned int x);
unsigned int Cyc_htonl(unsigned int x){return __stub_htonl(x);}unsigned short Cyc_htons(
unsigned short x);unsigned short Cyc_htons(unsigned short x){return __stub_htons(x);}
unsigned int Cyc_ntohl(unsigned int x);unsigned int Cyc_ntohl(unsigned int x){return
__stub_ntohl(x);}unsigned short Cyc_ntohs(unsigned short x);unsigned short Cyc_ntohs(
unsigned short x){return __stub_ntohs(x);}struct Cyc_dirent{unsigned int d_fileno;
unsigned short d_reclen;unsigned char d_type;unsigned char d_namlen;char d_name[256];
};struct Cyc___cycDIR;int Cyc_closedir(struct Cyc___cycDIR*);struct Cyc___cycDIR*Cyc_opendir(
const char*);struct Cyc_dirent*Cyc_readdir(struct Cyc___cycDIR*);void Cyc_rewinddir(
struct Cyc___cycDIR*);void Cyc_seekdir(struct Cyc___cycDIR*,long);long Cyc_telldir(
struct Cyc___cycDIR*);struct Cyc_Cdirent___abstractDIR;struct Cyc___cycDIR{struct
Cyc_Cdirent___abstractDIR*dir;};int closedir(struct Cyc_Cdirent___abstractDIR*d);
int Cyc_closedir(struct Cyc___cycDIR*d);int Cyc_closedir(struct Cyc___cycDIR*d){
return closedir(d->dir);}struct Cyc_Cdirent___abstractDIR*opendir(const char*);
static char _tmp1[32]="opendir called with NULL string";static struct Cyc_Core_Failure_struct
Cyc___opendir_failure={Cyc_Core_Failure,{_tmp1,_tmp1 + 32}};struct Cyc___cycDIR*
Cyc_opendir(const char*f);struct Cyc___cycDIR*Cyc_opendir(const char*f){struct Cyc_Cdirent___abstractDIR*
_tmp2=opendir(f);struct Cyc___cycDIR*_tmp9C;return(unsigned int)_tmp2?(_tmp9C=
_cycalloc(sizeof(*_tmp9C)),((_tmp9C->dir=(struct Cyc_Cdirent___abstractDIR*)_tmp2,
_tmp9C))): 0;}struct Cyc_dirent*readdir(struct Cyc_Cdirent___abstractDIR*);struct
Cyc_dirent*Cyc_readdir(struct Cyc___cycDIR*d);struct Cyc_dirent*Cyc_readdir(struct
Cyc___cycDIR*d){return readdir(d->dir);}void rewinddir(struct Cyc_Cdirent___abstractDIR*
d);void Cyc_rewinddir(struct Cyc___cycDIR*d);void Cyc_rewinddir(struct Cyc___cycDIR*
d){return rewinddir(d->dir);}void seekdir(struct Cyc_Cdirent___abstractDIR*d,long);
void Cyc_seekdir(struct Cyc___cycDIR*d,long x);void Cyc_seekdir(struct Cyc___cycDIR*d,
long x){return seekdir(d->dir,x);}long telldir(struct Cyc_Cdirent___abstractDIR*d);
long Cyc_telldir(struct Cyc___cycDIR*d);long Cyc_telldir(struct Cyc___cycDIR*d){
return telldir(d->dir);}int*__CYCLONE_ERRNO();struct Cyc_flock{long long l_start;
long long l_len;int l_pid;short l_type;short l_whence;};struct Cyc_Flock_struct{int
tag;struct Cyc_flock*f1;};struct Cyc_Long_struct{int tag;long f1;};int Cyc_fcntl(int
fd,int cmd,struct _dynforward_ptr);int Cyc_open(const char*,int,struct
_dynforward_ptr);int fcntl(int fd,int cmd);int fcntl_with_arg(int fd,int cmd,long arg);
int fcntl_with_lock(int fd,int cmd,struct Cyc_flock*lock);int open_without_mode(const
char*,int);int open_with_mode(const char*,int,unsigned short);static char _tmp5[21]="fcntl: too many args";
static struct Cyc_Core_Failure_struct Cyc___fcntl_failure={Cyc_Core_Failure,{_tmp5,
_tmp5 + 21}};int Cyc_fcntl(int fd,int cmd,struct _dynforward_ptr argv);int Cyc_fcntl(
int fd,int cmd,struct _dynforward_ptr argv){if(_get_dynforward_size(argv,sizeof(void*))
== 0)return fcntl(fd,cmd);else{if(_get_dynforward_size(argv,sizeof(void*))!= 1)(
int)_throw((void*)& Cyc___fcntl_failure);else{void*_tmp6=*((void**)
_check_dynforward_subscript(argv,sizeof(void*),0));long _tmp7;struct Cyc_flock*
_tmp8;_LL1: if(*((int*)_tmp6)!= 1)goto _LL3;_tmp7=((struct Cyc_Long_struct*)_tmp6)->f1;
_LL2: return fcntl_with_arg(fd,cmd,_tmp7);_LL3: if(*((int*)_tmp6)!= 0)goto _LL0;
_tmp8=((struct Cyc_Flock_struct*)_tmp6)->f1;_LL4: return fcntl_with_lock(fd,cmd,(
struct Cyc_flock*)_tmp8);_LL0:;}}}int Cyc_open(const char*s,int i,struct
_dynforward_ptr ms);int Cyc_open(const char*s,int i,struct _dynforward_ptr ms){if(
_get_dynforward_size(ms,sizeof(unsigned short))>= 1)return open_with_mode(s,i,*((
unsigned short*)_check_dynforward_subscript(ms,sizeof(unsigned short),0)));else{
return open_without_mode(s,i);}}struct Cyc_servent{char*s_name;char**s_aliases;int
s_port;char*s_proto;};struct Cyc_protoent{char*p_name;char**p_aliases;int p_proto;
};struct Cyc_hostent{char*h_name;char**h_aliases;int h_addrtype;int h_length;struct
Cyc_in_addr**h_addr_list;};struct Cyc_sockaddr_in{unsigned char sin_len;
unsigned char sin_family;unsigned short sin_port;struct Cyc_in_addr sin_addr;char
sin_zero[8];};struct Cyc_in6_addr{union{unsigned char __u6_addr8[16];
unsigned short __u6_addr16[8];unsigned int __u6_addr32[4];}__u6_addr;};struct Cyc_sockaddr_in6{
unsigned char sin6_len;unsigned char sin6_family;unsigned short sin6_port;
unsigned int sin6_flowinfo;struct Cyc_in6_addr sin6_addr;unsigned int sin6_scope_id;
};struct Cyc_ipv6_mreq{struct Cyc_in6_addr ipv6mr_multiaddr;unsigned int
ipv6mr_interface;};struct Cyc___SIG_BUILTIN_struct{int tag;void*f1;};struct Cyc___SIG_HANDLER_struct{
int tag;void(*f1)(int);};void*Cyc_signal(int,struct _dynforward_ptr);void*Cyc_Csignal_make_SIG_DFL();
void*Cyc_Csignal_make_SIG_DFL(){struct Cyc___SIG_BUILTIN_struct _tmp9F;struct Cyc___SIG_BUILTIN_struct*
_tmp9E;return(void*)((_tmp9E=_cycalloc(sizeof(*_tmp9E)),((_tmp9E[0]=((_tmp9F.tag=
0,((_tmp9F.f1=(void*)((void*)0),_tmp9F)))),_tmp9E))));}void*Cyc_Csignal_make_SIG_ERR();
void*Cyc_Csignal_make_SIG_ERR(){struct Cyc___SIG_BUILTIN_struct _tmpA2;struct Cyc___SIG_BUILTIN_struct*
_tmpA1;return(void*)((_tmpA1=_cycalloc(sizeof(*_tmpA1)),((_tmpA1[0]=((_tmpA2.tag=
0,((_tmpA2.f1=(void*)((void*)1),_tmpA2)))),_tmpA1))));}void*Cyc_Csignal_make_SIG_IGN();
void*Cyc_Csignal_make_SIG_IGN(){struct Cyc___SIG_BUILTIN_struct _tmpA5;struct Cyc___SIG_BUILTIN_struct*
_tmpA4;return(void*)((_tmpA4=_cycalloc(sizeof(*_tmpA4)),((_tmpA4[0]=((_tmpA5.tag=
0,((_tmpA5.f1=(void*)((void*)1),_tmpA5)))),_tmpA4))));}void*Cyc_Csignal_make_SIG_HANDLER(
void(*f)(int));void*Cyc_Csignal_make_SIG_HANDLER(void(*f)(int)){struct Cyc___SIG_HANDLER_struct
_tmpA8;struct Cyc___SIG_HANDLER_struct*_tmpA7;return(void*)((_tmpA7=_cycalloc(
sizeof(*_tmpA7)),((_tmpA7[0]=((_tmpA8.tag=1,((_tmpA8.f1=f,_tmpA8)))),_tmpA7))));}
void*signal_SIG_DFL(int);void*signal_SIG_ERR(int);void*signal_SIG_IGN(int);void*
signal(int,void(*)(int));static char _tmp12[49]="signal called with the wrong number of arguments";
static struct Cyc_Core_Failure_struct Cyc___signal_failure={Cyc_Core_Failure,{
_tmp12,_tmp12 + 49}};void*Cyc_signal(int a,struct _dynforward_ptr b);void*Cyc_signal(
int a,struct _dynforward_ptr b){if(_get_dynforward_size(b,sizeof(void*))!= 1)(int)
_throw((void*)& Cyc___signal_failure);{void*_tmp13=*((void**)
_check_dynforward_subscript(b,sizeof(void*),0));void*_tmp14;void*_tmp15;void*
_tmp16;void(*_tmp17)(int);_LL6: if(*((int*)_tmp13)!= 0)goto _LL8;_tmp14=(void*)((
struct Cyc___SIG_BUILTIN_struct*)_tmp13)->f1;if((int)_tmp14 != 0)goto _LL8;_LL7:
return signal_SIG_DFL(a);_LL8: if(*((int*)_tmp13)!= 0)goto _LLA;_tmp15=(void*)((
struct Cyc___SIG_BUILTIN_struct*)_tmp13)->f1;if((int)_tmp15 != 1)goto _LLA;_LL9:
return signal_SIG_ERR(a);_LLA: if(*((int*)_tmp13)!= 0)goto _LLC;_tmp16=(void*)((
struct Cyc___SIG_BUILTIN_struct*)_tmp13)->f1;if((int)_tmp16 != 2)goto _LLC;_LLB:
return signal_SIG_IGN(a);_LLC: if(*((int*)_tmp13)!= 1)goto _LL5;_tmp17=((struct Cyc___SIG_HANDLER_struct*)
_tmp13)->f1;_LLD: return signal(a,_tmp17);_LL5:;}}struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;
struct Cyc___cycFILE*Cyc_fromCfile(struct Cyc_Cstdio___abstractFILE*cf);struct Cyc_String_pa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;
};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};void Cyc_clearerr(struct Cyc___cycFILE*);int Cyc_fclose(
struct Cyc___cycFILE*);struct Cyc___cycFILE*Cyc_fdopen(int,const char*);int Cyc_feof(
struct Cyc___cycFILE*);int Cyc_ferror(struct Cyc___cycFILE*);int Cyc_fflush(struct
Cyc___cycFILE*);int Cyc_fgetc(struct Cyc___cycFILE*);int Cyc_fgetpos(struct Cyc___cycFILE*,
long long*);struct _dynforward_ptr Cyc_fgets(struct _dynforward_ptr,int,struct Cyc___cycFILE*);
int Cyc_fileno(struct Cyc___cycFILE*);struct Cyc___cycFILE*Cyc_fopen(const char*,
const char*);int Cyc_fputc(int,struct Cyc___cycFILE*);int Cyc_fputs(const char*,
struct Cyc___cycFILE*);unsigned long Cyc_fread(struct _dynforward_ptr,unsigned long,
unsigned long,struct Cyc___cycFILE*);struct Cyc___cycFILE*Cyc_freopen(const char*,
const char*,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};
struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dynforward_ptr f1;};int Cyc_fseek(struct Cyc___cycFILE*,long,int);int Cyc_fseeko(
struct Cyc___cycFILE*,long long,int);int Cyc_fsetpos(struct Cyc___cycFILE*,const
long long*);long Cyc_ftell(struct Cyc___cycFILE*);long long Cyc_ftello(struct Cyc___cycFILE*);
unsigned long Cyc_fwrite(struct _dynforward_ptr,unsigned long,unsigned long,struct
Cyc___cycFILE*);int Cyc_getc(struct Cyc___cycFILE*);int Cyc_getchar();int Cyc_pclose(
struct Cyc___cycFILE*);struct Cyc___cycFILE*Cyc_popen(const char*,const char*);int
Cyc_putc(int,struct Cyc___cycFILE*);void Cyc_rewind(struct Cyc___cycFILE*);int Cyc_setvbuf(
struct Cyc___cycFILE*,struct _dynforward_ptr,int,unsigned long);void Cyc_setbuf(
struct Cyc___cycFILE*,struct _dynforward_ptr);int Cyc_ungetc(int,struct Cyc___cycFILE*);
int Cyc_getw(struct Cyc___cycFILE*);int Cyc_putw(int,struct Cyc___cycFILE*);extern
char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc___cycFILE*Cyc_file_open(struct
_dynforward_ptr,struct _dynforward_ptr);void Cyc_file_close(struct Cyc___cycFILE*);
struct Cyc_Cstdio___abstractFILE;struct Cyc___cycFILE{struct Cyc_Cstdio___abstractFILE*
file;};struct Cyc___cycFILE*Cyc_fromCfile(struct Cyc_Cstdio___abstractFILE*cf);
struct Cyc___cycFILE*Cyc_fromCfile(struct Cyc_Cstdio___abstractFILE*cf){struct Cyc___cycFILE*
_tmpA9;return(unsigned int)cf?(_tmpA9=_cycalloc(sizeof(*_tmpA9)),((_tmpA9->file=(
struct Cyc_Cstdio___abstractFILE*)cf,_tmpA9))): 0;}void clearerr(struct Cyc_Cstdio___abstractFILE*);
void Cyc_clearerr(struct Cyc___cycFILE*f);void Cyc_clearerr(struct Cyc___cycFILE*f){
return clearerr(f->file);}int fclose(struct Cyc_Cstdio___abstractFILE*);int Cyc_fclose(
struct Cyc___cycFILE*f);int Cyc_fclose(struct Cyc___cycFILE*f){return fclose(f->file);}
struct Cyc_Cstdio___abstractFILE*fdopen(int,const char*);struct Cyc___cycFILE*Cyc_fdopen(
int i,const char*s);struct Cyc___cycFILE*Cyc_fdopen(int i,const char*s){struct Cyc_Cstdio___abstractFILE*
_tmp19=fdopen(i,s);struct Cyc___cycFILE*_tmpAA;return(unsigned int)_tmp19?(_tmpAA=
_cycalloc(sizeof(*_tmpAA)),((_tmpAA->file=(struct Cyc_Cstdio___abstractFILE*)
_tmp19,_tmpAA))): 0;}int feof(struct Cyc_Cstdio___abstractFILE*);int Cyc_feof(struct
Cyc___cycFILE*f);int Cyc_feof(struct Cyc___cycFILE*f){return feof(f->file);}int
ferror(struct Cyc_Cstdio___abstractFILE*);int Cyc_ferror(struct Cyc___cycFILE*f);
int Cyc_ferror(struct Cyc___cycFILE*f){return ferror(f->file);}int fflush(struct Cyc_Cstdio___abstractFILE*);
int Cyc_fflush(struct Cyc___cycFILE*f);int Cyc_fflush(struct Cyc___cycFILE*f){if((
unsigned int)f)return fflush((struct Cyc_Cstdio___abstractFILE*)f->file);else{
return fflush(0);}}int fgetc(struct Cyc_Cstdio___abstractFILE*);int Cyc_fgetc(struct
Cyc___cycFILE*f);int Cyc_fgetc(struct Cyc___cycFILE*f){return fgetc(f->file);}int
fgetpos(struct Cyc_Cstdio___abstractFILE*,long long*);int Cyc_fgetpos(struct Cyc___cycFILE*
f,long long*x);int Cyc_fgetpos(struct Cyc___cycFILE*f,long long*x){return fgetpos(f->file,
x);}char*fgets(char*,int,struct Cyc_Cstdio___abstractFILE*);static char _tmp1C[30]="fgets called with NULL string";
static struct Cyc_Core_Failure_struct Cyc___fgets_failure_1={Cyc_Core_Failure,{
_tmp1C,_tmp1C + 30}};static char _tmp1E[27]="fgets: buffer insufficient";static
struct Cyc_Core_Failure_struct Cyc___fgets_failure_2={Cyc_Core_Failure,{_tmp1E,
_tmp1E + 27}};struct _dynforward_ptr Cyc_fgets(struct _dynforward_ptr s,int n,struct
Cyc___cycFILE*f);struct _dynforward_ptr Cyc_fgets(struct _dynforward_ptr s,int n,
struct Cyc___cycFILE*f){if(!((unsigned int)s.curr))(int)_throw((void*)& Cyc___fgets_failure_1);{
char*result;char*buffer=(char*)_check_null(_untag_dynforward_ptr(s,sizeof(char),
1));unsigned int len=_get_dynforward_size(s,sizeof(char));if(len < n)(int)_throw((
void*)& Cyc___fgets_failure_2);result=fgets(buffer,n,f->file);if(result == 0)
return _tag_dynforward(0,0,0);else{return s;}}}int fileno(struct Cyc_Cstdio___abstractFILE*);
int Cyc_fileno(struct Cyc___cycFILE*f);int Cyc_fileno(struct Cyc___cycFILE*f){return
fileno(f->file);}struct Cyc_Cstdio___abstractFILE*fopen(const char*,const char*);
struct Cyc___cycFILE*Cyc_fopen(const char*name,const char*type);struct Cyc___cycFILE*
Cyc_fopen(const char*name,const char*type){struct Cyc_Cstdio___abstractFILE*_tmp1F=
fopen(name,type);struct Cyc___cycFILE*_tmpAB;return(unsigned int)_tmp1F?(_tmpAB=
_cycalloc(sizeof(*_tmpAB)),((_tmpAB->file=(struct Cyc_Cstdio___abstractFILE*)
_tmp1F,_tmpAB))): 0;}int fputc(int,struct Cyc_Cstdio___abstractFILE*);int Cyc_fputc(
int x,struct Cyc___cycFILE*f);int Cyc_fputc(int x,struct Cyc___cycFILE*f){return fputc(
x,f->file);}int fputs(const char*,struct Cyc_Cstdio___abstractFILE*);int Cyc_fputs(
const char*x,struct Cyc___cycFILE*f);int Cyc_fputs(const char*x,struct Cyc___cycFILE*
f){return fputs(x,f->file);}unsigned long fread(char*,unsigned long,unsigned long,
struct Cyc_Cstdio___abstractFILE*);static char _tmp22[27]="fread: buffer insufficient";
static struct Cyc_Core_Failure_struct Cyc___fread_failure={Cyc_Core_Failure,{_tmp22,
_tmp22 + 27}};unsigned long Cyc_fread(struct _dynforward_ptr ptr,unsigned long size,
unsigned long nmemb,struct Cyc___cycFILE*f);unsigned long Cyc_fread(struct
_dynforward_ptr ptr,unsigned long size,unsigned long nmemb,struct Cyc___cycFILE*f){
if(size * nmemb > _get_dynforward_size(ptr,sizeof(char)))(int)_throw((void*)& Cyc___fread_failure);
return fread((char*)_check_null(_untag_dynforward_ptr(ptr,sizeof(char),1)),size,
nmemb,f->file);}struct Cyc_Cstdio___abstractFILE*freopen(const char*,const char*,
struct Cyc_Cstdio___abstractFILE*);struct Cyc___cycFILE*Cyc_freopen(const char*
filename,const char*modes,struct Cyc___cycFILE*f);struct Cyc___cycFILE*Cyc_freopen(
const char*filename,const char*modes,struct Cyc___cycFILE*f){struct Cyc_Cstdio___abstractFILE*
_tmp23=freopen(filename,modes,f->file);struct Cyc___cycFILE*_tmpAC;return(
unsigned int)_tmp23?(struct Cyc___cycFILE*)((_tmpAC=_cycalloc(sizeof(*_tmpAC)),((
_tmpAC->file=(struct Cyc_Cstdio___abstractFILE*)_tmp23,_tmpAC)))): 0;}int fseek(
struct Cyc_Cstdio___abstractFILE*,long,int);int Cyc_fseek(struct Cyc___cycFILE*a,
long b,int c);int Cyc_fseek(struct Cyc___cycFILE*a,long b,int c){return fseek(a->file,b,
c);}int fseeko(struct Cyc_Cstdio___abstractFILE*,long long,int);int Cyc_fseeko(
struct Cyc___cycFILE*a,long long b,int c);int Cyc_fseeko(struct Cyc___cycFILE*a,
long long b,int c){return fseeko(a->file,b,c);}int fsetpos(struct Cyc_Cstdio___abstractFILE*,
const long long*);int Cyc_fsetpos(struct Cyc___cycFILE*f,const long long*b);int Cyc_fsetpos(
struct Cyc___cycFILE*f,const long long*b){return fsetpos(f->file,b);}long ftell(
struct Cyc_Cstdio___abstractFILE*);long Cyc_ftell(struct Cyc___cycFILE*f);long Cyc_ftell(
struct Cyc___cycFILE*f){return ftell(f->file);}long long ftello(struct Cyc_Cstdio___abstractFILE*);
long long Cyc_ftello(struct Cyc___cycFILE*f);long long Cyc_ftello(struct Cyc___cycFILE*
f){return ftello(f->file);}unsigned long fwrite(const char*,unsigned long,
unsigned long,struct Cyc_Cstdio___abstractFILE*);static char _tmp26[31]="fwrite called with NULL string";
static struct Cyc_Core_Failure_struct Cyc___fwrite_failure_1={Cyc_Core_Failure,{
_tmp26,_tmp26 + 31}};static char _tmp28[28]="fwrite: buffer insufficient";static
struct Cyc_Core_Failure_struct Cyc___fwrite_failure_2={Cyc_Core_Failure,{_tmp28,
_tmp28 + 28}};unsigned long Cyc_fwrite(struct _dynforward_ptr ptr,unsigned long size,
unsigned long nmemb,struct Cyc___cycFILE*f);unsigned long Cyc_fwrite(struct
_dynforward_ptr ptr,unsigned long size,unsigned long nmemb,struct Cyc___cycFILE*f){
if(!((unsigned int)ptr.curr))(int)_throw((void*)& Cyc___fwrite_failure_1);else{
if(size * nmemb > _get_dynforward_size(ptr,sizeof(char)))(int)_throw((void*)& Cyc___fwrite_failure_2);
return fwrite((const char*)_untag_dynforward_ptr(ptr,sizeof(char),1),size,nmemb,f->file);}}
int getc(struct Cyc_Cstdio___abstractFILE*);int Cyc_getc(struct Cyc___cycFILE*f);int
Cyc_getc(struct Cyc___cycFILE*f){return getc(f->file);}int getchar();int Cyc_getchar();
int Cyc_getchar(){return getchar();}int pclose(struct Cyc_Cstdio___abstractFILE*);
int Cyc_pclose(struct Cyc___cycFILE*f);int Cyc_pclose(struct Cyc___cycFILE*f){return
pclose(f->file);}struct Cyc_Cstdio___abstractFILE*popen(const char*,const char*);
struct Cyc___cycFILE*Cyc_popen(const char*name,const char*type);struct Cyc___cycFILE*
Cyc_popen(const char*name,const char*type){struct Cyc_Cstdio___abstractFILE*_tmp29=
popen(name,type);struct Cyc___cycFILE*_tmpAD;return(unsigned int)_tmp29?(_tmpAD=
_cycalloc(sizeof(*_tmpAD)),((_tmpAD->file=(struct Cyc_Cstdio___abstractFILE*)
_tmp29,_tmpAD))): 0;}int putc(int,struct Cyc_Cstdio___abstractFILE*);int Cyc_putc(
int x,struct Cyc___cycFILE*f);int Cyc_putc(int x,struct Cyc___cycFILE*f){return putc(x,
f->file);}void rewind(struct Cyc_Cstdio___abstractFILE*);void Cyc_rewind(struct Cyc___cycFILE*
f);void Cyc_rewind(struct Cyc___cycFILE*f){return rewind(f->file);}int setvbuf(
struct Cyc_Cstdio___abstractFILE*,char*,int,unsigned long);static char _tmp2C[24]="setvbuf: buffer is NULL";
static struct Cyc_Core_Failure_struct Cyc___setvbuf_failure_1={Cyc_Core_Failure,{
_tmp2C,_tmp2C + 24}};static char _tmp2E[29]="setvbuf: buffer insufficient";static
struct Cyc_Core_Failure_struct Cyc___setvbuf_failure_2={Cyc_Core_Failure,{_tmp2E,
_tmp2E + 29}};int Cyc_setvbuf(struct Cyc___cycFILE*f,struct _dynforward_ptr buf,int
type,unsigned long size);int Cyc_setvbuf(struct Cyc___cycFILE*f,struct
_dynforward_ptr buf,int type,unsigned long size){if(!((unsigned int)buf.curr))(int)
_throw((void*)& Cyc___setvbuf_failure_1);if(_get_dynforward_size(buf,sizeof(char))
< size)(int)_throw((void*)& Cyc___setvbuf_failure_2);return setvbuf(f->file,(char*)
_check_null(_untag_dynforward_ptr(buf,sizeof(char),1)),type,size);}void Cyc_setbuf(
struct Cyc___cycFILE*f,struct _dynforward_ptr buf);void Cyc_setbuf(struct Cyc___cycFILE*
f,struct _dynforward_ptr buf){Cyc_setvbuf(f,buf,(unsigned int)buf.curr?0: 2,1024);
return;}int ungetc(int,struct Cyc_Cstdio___abstractFILE*);int Cyc_ungetc(int x,
struct Cyc___cycFILE*f);int Cyc_ungetc(int x,struct Cyc___cycFILE*f){return ungetc(x,
f->file);}int getw(struct Cyc_Cstdio___abstractFILE*);int Cyc_getw(struct Cyc___cycFILE*
f);int Cyc_getw(struct Cyc___cycFILE*f){return getw(f->file);}int putw(int,struct Cyc_Cstdio___abstractFILE*);
int Cyc_putw(int x,struct Cyc___cycFILE*f);int Cyc_putw(int x,struct Cyc___cycFILE*f){
return putw(x,f->file);}char Cyc_FileCloseError[19]="\000\000\000\000FileCloseError\000";
char Cyc_FileOpenError[18]="\000\000\000\000FileOpenError\000";struct Cyc___cycFILE*
Cyc_file_open(struct _dynforward_ptr fname,struct _dynforward_ptr mode);static void
_tmpB3(struct _dynforward_ptr*fname,unsigned int*_tmpB2,unsigned int*_tmpB1,char**
_tmpAF){for(*_tmpB2=0;*_tmpB2 < *_tmpB1;(*_tmpB2)++){(*_tmpAF)[*_tmpB2]=*((const
char*)_check_dynforward_subscript(*fname,sizeof(char),(int)*_tmpB2));}}struct Cyc___cycFILE*
Cyc_file_open(struct _dynforward_ptr fname,struct _dynforward_ptr mode){struct Cyc___cycFILE*
f=Cyc_fopen((const char*)_untag_dynforward_ptr(fname,sizeof(char),1),(const char*)
_untag_dynforward_ptr(mode,sizeof(char),1));if(f == 0){unsigned int _tmpB2;
unsigned int _tmpB1;struct _dynforward_ptr _tmpB0;char*_tmpAF;unsigned int _tmpAE;
struct _dynforward_ptr fn=(_tmpAE=_get_dynforward_size(fname,sizeof(char)),((
_tmpAF=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmpAE + 1)),((_tmpB0=
_tag_dynforward(_tmpAF,sizeof(char),_tmpAE + 1),((((_tmpB1=_tmpAE,((_tmpB3(& fname,&
_tmpB2,& _tmpB1,& _tmpAF),_tmpAF[_tmpB1]=(char)0)))),_tmpB0)))))));struct Cyc_FileOpenError_struct
_tmpB6;struct Cyc_FileOpenError_struct*_tmpB5;(int)_throw((void*)((_tmpB5=
_cycalloc(sizeof(*_tmpB5)),((_tmpB5[0]=((_tmpB6.tag=Cyc_FileOpenError,((_tmpB6.f1=
fn,_tmpB6)))),_tmpB5)))));}return(struct Cyc___cycFILE*)f;}void Cyc_file_close(
struct Cyc___cycFILE*f);void Cyc_file_close(struct Cyc___cycFILE*f){if(Cyc_fclose(f)
!= 0)(int)_throw((void*)Cyc_FileCloseError);}typedef struct{int quot;int rem;}Cyc_div_t;
typedef struct{long quot;long rem;}Cyc_ldiv_t;void*abort();void exit(int);void Cyc_free(
struct _dynforward_ptr);void Cyc_qsort(struct _dynforward_ptr,unsigned long,
unsigned int,int(*)(const void*,const void*));void Cyc_free(struct _dynforward_ptr
ptr);void Cyc_free(struct _dynforward_ptr ptr){;}void qsort(void*,unsigned long,
unsigned long,int(*)(const void*,const void*));static char _tmp36[23]="qsort called with NULL";
static struct Cyc_Core_Failure_struct Cyc___qsort_failure_1={Cyc_Core_Failure,{
_tmp36,_tmp36 + 23}};static char _tmp38[42]="qsort called with insufficient array size";
static struct Cyc_Core_Failure_struct Cyc___qsort_failure_2={Cyc_Core_Failure,{
_tmp38,_tmp38 + 42}};void Cyc_qsort(struct _dynforward_ptr tab,unsigned long nmemb,
unsigned int szmemb,int(*compar)(const void*,const void*));void Cyc_qsort(struct
_dynforward_ptr tab,unsigned long nmemb,unsigned int szmemb,int(*compar)(const void*,
const void*)){if(!((unsigned int)tab.curr))(int)_throw((void*)& Cyc___qsort_failure_1);
if(_get_dynforward_size(tab,sizeof(void))< nmemb)(int)_throw((void*)& Cyc___qsort_failure_2);
qsort((void*)_untag_dynforward_ptr(tab,sizeof(void),1),nmemb,(unsigned long)
szmemb,compar);}struct Cyc_ipc_perm{unsigned short cuid;unsigned short cgid;
unsigned short uid;unsigned short gid;unsigned short mode;unsigned short seq;long key;
};struct _dynforward_ptr Cyc_mmap(struct _dynforward_ptr,unsigned long length,int
prot,int flags,int fd,long long offset);int Cyc_munmap(struct _dynforward_ptr start,
unsigned long length);char*__stub_mmap(char*start,unsigned long length,int prot,int
flags,int fd,long long offset);int munmap(const char*start,unsigned long length);
static char _tmp3A[42]="mmap called with illegal start/prot/flags";static struct Cyc_Core_Failure_struct
Cyc___mmap_failure={Cyc_Core_Failure,{_tmp3A,_tmp3A + 42}};struct _dynforward_ptr
Cyc_mmap(struct _dynforward_ptr ignored,unsigned long length,int prot,int flags,int fd,
long long offset);struct _dynforward_ptr Cyc_mmap(struct _dynforward_ptr ignored,
unsigned long length,int prot,int flags,int fd,long long offset){if((ignored.curr == (
_tag_dynforward(0,0,0)).curr  && prot == 1) && (flags == 2  || flags == 1))return(
struct _dynforward_ptr)wrap_Cbuffer_as_buffer(__stub_mmap(0,length,prot,flags,fd,
offset),length);else{(int)_throw((void*)& Cyc___mmap_failure);}}int Cyc_munmap(
struct _dynforward_ptr start,unsigned long length);int Cyc_munmap(struct
_dynforward_ptr start,unsigned long length){return munmap((const char*)_check_null(
_untag_dynforward_ptr(start,sizeof(char),1)),length);}struct Cyc_timeval{int
tv_sec;int tv_usec;};struct Cyc_rusage{struct Cyc_timeval ru_utime;struct Cyc_timeval
ru_stime;long ru_maxrss;long ru_ixrss;long ru_idrss;long ru_isrss;long ru_minflt;long
ru_majflt;long ru_nswap;long ru_inblock;long ru_oublock;long ru_msgsnd;long ru_msgrcv;
long ru_nsignals;long ru_nvcsw;long ru_nivcsw;};struct Cyc_rlimit{long long rlim_cur;
long long rlim_max;};struct Cyc_fd_set{int fds_bits[(1024 + (sizeof(int)* 8 - 1))/ (
sizeof(int)* 8)];};struct Cyc_timespec{long tv_sec;int tv_nsec;};int Cyc_select(int a,
struct Cyc_fd_set*b,struct Cyc_fd_set*c,struct Cyc_fd_set*d,struct Cyc_timeval*e);
void Cyc_FD_CLR(int,struct Cyc_fd_set*);int Cyc_FD_ISSET(int,struct Cyc_fd_set*);
void Cyc_FD_SET(int,struct Cyc_fd_set*);void Cyc_FD_ZERO(struct Cyc_fd_set*);int
select(int,struct Cyc_fd_set*,struct Cyc_fd_set*,struct Cyc_fd_set*,struct Cyc_timeval*);
int Cyc_select(int a,struct Cyc_fd_set*b,struct Cyc_fd_set*c,struct Cyc_fd_set*d,
struct Cyc_timeval*e);extern int Cyc_select(int a,struct Cyc_fd_set*b,struct Cyc_fd_set*
c,struct Cyc_fd_set*d,struct Cyc_timeval*e){if(a < 0  || a > 1024){*((int*)
_check_null(__CYCLONE_ERRNO()))=22;return - 1;}return select(a,b,c,d,e);}void
__stub_FD_CLR(int,struct Cyc_fd_set*);int __stub_FD_ISSET(int,struct Cyc_fd_set*);
void __stub_FD_SET(int,struct Cyc_fd_set*);void __stub_FD_ZERO(struct Cyc_fd_set*);
void Cyc_FD_CLR(int a,struct Cyc_fd_set*b);void Cyc_FD_CLR(int a,struct Cyc_fd_set*b){
if(a < 0  || a > 1024)return;return __stub_FD_CLR(a,b);}int Cyc_FD_ISSET(int a,struct
Cyc_fd_set*b);int Cyc_FD_ISSET(int a,struct Cyc_fd_set*b){if(a < 0  || a > 1024)return
0;return __stub_FD_ISSET(a,b);}void Cyc_FD_SET(int a,struct Cyc_fd_set*b);void Cyc_FD_SET(
int a,struct Cyc_fd_set*b){if(a < 0  || a > 1024)return;return __stub_FD_SET(a,b);}
void Cyc_FD_ZERO(struct Cyc_fd_set*a);void Cyc_FD_ZERO(struct Cyc_fd_set*a){return
__stub_FD_ZERO(a);}struct Cyc_cmsghdr;struct Cyc_linger{int l_onoff;int l_linger;};
struct Cyc_SA_sockaddr_in_struct{int tag;struct Cyc_sockaddr_in*f1;};struct Cyc_SA_socklenptr_struct{
int tag;int*f1;};struct Cyc_SA_socklen_struct{int tag;int f1;};struct Cyc_SO_int_struct{
int tag;int*f1;};struct Cyc_SO_timeval_struct{int tag;struct Cyc_timeval*f1;};struct
Cyc_SO_socklenptr_struct{int tag;int*f1;};struct Cyc_SO_socklen_struct{int tag;int
f1;};int Cyc_accept(int fd,struct _dynforward_ptr);extern char Cyc_SocketError[16];
int Cyc_bind(int fd,struct _dynforward_ptr);int Cyc_connect(int fd,struct
_dynforward_ptr);int Cyc_getpeername(int fd,struct _dynforward_ptr);int Cyc_getsockname(
int fd,struct _dynforward_ptr);int Cyc_getsockopt(int fd,int level,int optname,struct
_dynforward_ptr);int Cyc_recv(int fd,struct _dynforward_ptr buf,unsigned long n,int
flags);int Cyc_recvfrom(int fd,struct _dynforward_ptr buf,unsigned long n,int flags,
struct _dynforward_ptr);int Cyc_send(int fd,struct _dynforward_ptr buf,unsigned long n,
int flags);int Cyc_sendto(int fd,struct _dynforward_ptr buf,unsigned long n,int flags,
struct _dynforward_ptr);int Cyc_setsockopt(int fd,int level,int optname,struct
_dynforward_ptr);char Cyc_SocketError[16]="\000\000\000\000SocketError\000";int
accept_in(int,const struct Cyc_sockaddr_in*,int*);struct _tuple0{void*f1;void*f2;};
int Cyc_accept(int fd,struct _dynforward_ptr ap);int Cyc_accept(int fd,struct
_dynforward_ptr ap){if(_get_dynforward_size(ap,sizeof(void*))!= 2)(int)_throw((
void*)Cyc_SocketError);{struct _tuple0 _tmpB7;struct _tuple0 _tmp3C=(_tmpB7.f1=*((
void**)_check_dynforward_subscript(ap,sizeof(void*),0)),((_tmpB7.f2=*((void**)
_check_dynforward_subscript(ap,sizeof(void*),1)),_tmpB7)));void*_tmp3D;struct Cyc_sockaddr_in*
_tmp3E;void*_tmp3F;int*_tmp40;_LLF: _tmp3D=_tmp3C.f1;if(*((int*)_tmp3D)!= 0)goto
_LL11;_tmp3E=((struct Cyc_SA_sockaddr_in_struct*)_tmp3D)->f1;_tmp3F=_tmp3C.f2;if(*((
int*)_tmp3F)!= 1)goto _LL11;_tmp40=((struct Cyc_SA_socklenptr_struct*)_tmp3F)->f1;
_LL10: return accept_in(fd,(const struct Cyc_sockaddr_in*)_tmp3E,_tmp40);_LL11:;
_LL12:(int)_throw((void*)Cyc_SocketError);_LLE:;}}int bind_in(int,const struct Cyc_sockaddr_in*,
int);int Cyc_bind(int fd,struct _dynforward_ptr ap);int Cyc_bind(int fd,struct
_dynforward_ptr ap){if(_get_dynforward_size(ap,sizeof(void*))!= 2)(int)_throw((
void*)Cyc_SocketError);{struct _tuple0 _tmpB8;struct _tuple0 _tmp42=(_tmpB8.f1=*((
void**)_check_dynforward_subscript(ap,sizeof(void*),0)),((_tmpB8.f2=*((void**)
_check_dynforward_subscript(ap,sizeof(void*),1)),_tmpB8)));void*_tmp43;struct Cyc_sockaddr_in*
_tmp44;void*_tmp45;int _tmp46;_LL14: _tmp43=_tmp42.f1;if(*((int*)_tmp43)!= 0)goto
_LL16;_tmp44=((struct Cyc_SA_sockaddr_in_struct*)_tmp43)->f1;_tmp45=_tmp42.f2;if(*((
int*)_tmp45)!= 2)goto _LL16;_tmp46=((struct Cyc_SA_socklen_struct*)_tmp45)->f1;
_LL15: return bind_in(fd,(const struct Cyc_sockaddr_in*)_tmp44,_tmp46);_LL16:;_LL17:(
int)_throw((void*)Cyc_SocketError);_LL13:;}}int connect_in(int,const struct Cyc_sockaddr_in*,
int);int Cyc_connect(int fd,struct _dynforward_ptr ap);int Cyc_connect(int fd,struct
_dynforward_ptr ap){if(_get_dynforward_size(ap,sizeof(void*))!= 2)(int)_throw((
void*)Cyc_SocketError);{struct _tuple0 _tmpB9;struct _tuple0 _tmp48=(_tmpB9.f1=*((
void**)_check_dynforward_subscript(ap,sizeof(void*),0)),((_tmpB9.f2=*((void**)
_check_dynforward_subscript(ap,sizeof(void*),1)),_tmpB9)));void*_tmp49;struct Cyc_sockaddr_in*
_tmp4A;void*_tmp4B;int _tmp4C;_LL19: _tmp49=_tmp48.f1;if(*((int*)_tmp49)!= 0)goto
_LL1B;_tmp4A=((struct Cyc_SA_sockaddr_in_struct*)_tmp49)->f1;_tmp4B=_tmp48.f2;if(*((
int*)_tmp4B)!= 2)goto _LL1B;_tmp4C=((struct Cyc_SA_socklen_struct*)_tmp4B)->f1;
_LL1A: return connect_in(fd,(const struct Cyc_sockaddr_in*)_tmp4A,_tmp4C);_LL1B:;
_LL1C:(int)_throw((void*)Cyc_SocketError);_LL18:;}}int getpeername_in(int,const
struct Cyc_sockaddr_in*,int*);int Cyc_getpeername(int fd,struct _dynforward_ptr ap);
int Cyc_getpeername(int fd,struct _dynforward_ptr ap){if(_get_dynforward_size(ap,
sizeof(void*))!= 2)(int)_throw((void*)Cyc_SocketError);{struct _tuple0 _tmpBA;
struct _tuple0 _tmp4E=(_tmpBA.f1=*((void**)_check_dynforward_subscript(ap,sizeof(
void*),0)),((_tmpBA.f2=*((void**)_check_dynforward_subscript(ap,sizeof(void*),1)),
_tmpBA)));void*_tmp4F;struct Cyc_sockaddr_in*_tmp50;void*_tmp51;int*_tmp52;_LL1E:
_tmp4F=_tmp4E.f1;if(*((int*)_tmp4F)!= 0)goto _LL20;_tmp50=((struct Cyc_SA_sockaddr_in_struct*)
_tmp4F)->f1;_tmp51=_tmp4E.f2;if(*((int*)_tmp51)!= 1)goto _LL20;_tmp52=((struct Cyc_SA_socklenptr_struct*)
_tmp51)->f1;_LL1F: return getpeername_in(fd,(const struct Cyc_sockaddr_in*)_tmp50,
_tmp52);_LL20:;_LL21:(int)_throw((void*)Cyc_SocketError);_LL1D:;}}int
getsockname_in(int,const struct Cyc_sockaddr_in*,int*);int Cyc_getsockname(int fd,
struct _dynforward_ptr ap);int Cyc_getsockname(int fd,struct _dynforward_ptr ap){if(
_get_dynforward_size(ap,sizeof(void*))!= 2)(int)_throw((void*)Cyc_SocketError);{
struct _tuple0 _tmpBB;struct _tuple0 _tmp54=(_tmpBB.f1=*((void**)
_check_dynforward_subscript(ap,sizeof(void*),0)),((_tmpBB.f2=*((void**)
_check_dynforward_subscript(ap,sizeof(void*),1)),_tmpBB)));void*_tmp55;struct Cyc_sockaddr_in*
_tmp56;void*_tmp57;int*_tmp58;_LL23: _tmp55=_tmp54.f1;if(*((int*)_tmp55)!= 0)goto
_LL25;_tmp56=((struct Cyc_SA_sockaddr_in_struct*)_tmp55)->f1;_tmp57=_tmp54.f2;if(*((
int*)_tmp57)!= 1)goto _LL25;_tmp58=((struct Cyc_SA_socklenptr_struct*)_tmp57)->f1;
_LL24: return getsockname_in(fd,(const struct Cyc_sockaddr_in*)_tmp56,_tmp58);_LL25:;
_LL26:(int)_throw((void*)Cyc_SocketError);_LL22:;}}int getsockopt_int(int,int,int,
int*,int*);int getsockopt_timeval(int,int,int,struct Cyc_timeval*,int*);int Cyc_getsockopt(
int fd,int level,int optname,struct _dynforward_ptr ap);int Cyc_getsockopt(int fd,int
level,int optname,struct _dynforward_ptr ap){if(_get_dynforward_size(ap,sizeof(void*))
!= 2)(int)_throw((void*)Cyc_SocketError);{struct _tuple0 _tmpBC;struct _tuple0
_tmp5A=(_tmpBC.f1=*((void**)_check_dynforward_subscript(ap,sizeof(void*),0)),((
_tmpBC.f2=*((void**)_check_dynforward_subscript(ap,sizeof(void*),1)),_tmpBC)));
void*_tmp5B;int*_tmp5C;void*_tmp5D;int*_tmp5E;void*_tmp5F;int*_tmp60;void*_tmp61;
int*_tmp62;void*_tmp63;struct Cyc_timeval*_tmp64;void*_tmp65;int*_tmp66;void*
_tmp67;struct Cyc_timeval*_tmp68;void*_tmp69;int*_tmp6A;_LL28: _tmp5B=_tmp5A.f1;
if(*((int*)_tmp5B)!= 0)goto _LL2A;_tmp5C=((struct Cyc_SO_int_struct*)_tmp5B)->f1;
_tmp5D=_tmp5A.f2;if(*((int*)_tmp5D)!= 2)goto _LL2A;_tmp5E=((struct Cyc_SO_socklenptr_struct*)
_tmp5D)->f1;_LL29: return getsockopt_int(fd,level,optname,_tmp5C,_tmp5E);_LL2A:
_tmp5F=_tmp5A.f1;if(*((int*)_tmp5F)!= 0)goto _LL2C;_tmp60=((struct Cyc_SO_int_struct*)
_tmp5F)->f1;_tmp61=_tmp5A.f2;if(*((int*)_tmp61)!= 0)goto _LL2C;_tmp62=((struct Cyc_SO_int_struct*)
_tmp61)->f1;_LL2B: return getsockopt_int(fd,level,optname,_tmp60,(int*)_tmp62);
_LL2C: _tmp63=_tmp5A.f1;if(*((int*)_tmp63)!= 1)goto _LL2E;_tmp64=((struct Cyc_SO_timeval_struct*)
_tmp63)->f1;_tmp65=_tmp5A.f2;if(*((int*)_tmp65)!= 2)goto _LL2E;_tmp66=((struct Cyc_SO_socklenptr_struct*)
_tmp65)->f1;_LL2D: return getsockopt_timeval(fd,level,optname,_tmp64,_tmp66);_LL2E:
_tmp67=_tmp5A.f1;if(*((int*)_tmp67)!= 1)goto _LL30;_tmp68=((struct Cyc_SO_timeval_struct*)
_tmp67)->f1;_tmp69=_tmp5A.f2;if(*((int*)_tmp69)!= 0)goto _LL30;_tmp6A=((struct Cyc_SO_int_struct*)
_tmp69)->f1;_LL2F: return getsockopt_timeval(fd,level,optname,_tmp68,(int*)_tmp6A);
_LL30:;_LL31:(int)_throw((void*)Cyc_SocketError);_LL27:;}}int recv_wrapped(int,
struct _dynforward_ptr,unsigned long,int);int Cyc_recv(int fd,struct _dynforward_ptr
buf,unsigned long n,int flags);int Cyc_recv(int fd,struct _dynforward_ptr buf,
unsigned long n,int flags){return recv_wrapped(fd,buf,n,flags);}int recvfrom_in(int,
struct _dynforward_ptr,unsigned long,int,const struct Cyc_sockaddr_in*,int*);int Cyc_recvfrom(
int fd,struct _dynforward_ptr buf,unsigned long n,int flags,struct _dynforward_ptr ap);
int Cyc_recvfrom(int fd,struct _dynforward_ptr buf,unsigned long n,int flags,struct
_dynforward_ptr ap){if(_get_dynforward_size(ap,sizeof(void*))!= 2)(int)_throw((
void*)Cyc_SocketError);{struct _tuple0 _tmpBD;struct _tuple0 _tmp6C=(_tmpBD.f1=*((
void**)_check_dynforward_subscript(ap,sizeof(void*),0)),((_tmpBD.f2=*((void**)
_check_dynforward_subscript(ap,sizeof(void*),1)),_tmpBD)));void*_tmp6D;struct Cyc_sockaddr_in*
_tmp6E;void*_tmp6F;int*_tmp70;_LL33: _tmp6D=_tmp6C.f1;if(*((int*)_tmp6D)!= 0)goto
_LL35;_tmp6E=((struct Cyc_SA_sockaddr_in_struct*)_tmp6D)->f1;_tmp6F=_tmp6C.f2;if(*((
int*)_tmp6F)!= 1)goto _LL35;_tmp70=((struct Cyc_SA_socklenptr_struct*)_tmp6F)->f1;
_LL34: return recvfrom_in(fd,buf,n,flags,(const struct Cyc_sockaddr_in*)_tmp6E,
_tmp70);_LL35:;_LL36:(int)_throw((void*)Cyc_SocketError);_LL32:;}}int
send_wrapped(int,struct _dynforward_ptr,unsigned long,int);int Cyc_send(int fd,
struct _dynforward_ptr buf,unsigned long n,int flags);int Cyc_send(int fd,struct
_dynforward_ptr buf,unsigned long n,int flags){return send_wrapped(fd,buf,n,flags);}
int sendto_in(int,struct _dynforward_ptr,unsigned long,int,const struct Cyc_sockaddr_in*,
int);int Cyc_sendto(int fd,struct _dynforward_ptr buf,unsigned long n,int flags,struct
_dynforward_ptr ap);int Cyc_sendto(int fd,struct _dynforward_ptr buf,unsigned long n,
int flags,struct _dynforward_ptr ap){if(_get_dynforward_size(ap,sizeof(void*))!= 2)(
int)_throw((void*)Cyc_SocketError);{struct _tuple0 _tmpBE;struct _tuple0 _tmp72=(
_tmpBE.f1=*((void**)_check_dynforward_subscript(ap,sizeof(void*),0)),((_tmpBE.f2=*((
void**)_check_dynforward_subscript(ap,sizeof(void*),1)),_tmpBE)));void*_tmp73;
struct Cyc_sockaddr_in*_tmp74;void*_tmp75;int _tmp76;_LL38: _tmp73=_tmp72.f1;if(*((
int*)_tmp73)!= 0)goto _LL3A;_tmp74=((struct Cyc_SA_sockaddr_in_struct*)_tmp73)->f1;
_tmp75=_tmp72.f2;if(*((int*)_tmp75)!= 2)goto _LL3A;_tmp76=((struct Cyc_SA_socklen_struct*)
_tmp75)->f1;_LL39: return sendto_in(fd,buf,n,flags,(const struct Cyc_sockaddr_in*)
_tmp74,_tmp76);_LL3A:;_LL3B:(int)_throw((void*)Cyc_SocketError);_LL37:;}}int
setsockopt_int(int,int,int,const int*,int);int setsockopt_timeval(int,int,int,
const struct Cyc_timeval*,int);int Cyc_setsockopt(int fd,int level,int optname,struct
_dynforward_ptr ap);int Cyc_setsockopt(int fd,int level,int optname,struct
_dynforward_ptr ap){if(_get_dynforward_size(ap,sizeof(void*))!= 2)(int)_throw((
void*)Cyc_SocketError);{struct _tuple0 _tmpBF;struct _tuple0 _tmp78=(_tmpBF.f1=*((
void**)_check_dynforward_subscript(ap,sizeof(void*),0)),((_tmpBF.f2=*((void**)
_check_dynforward_subscript(ap,sizeof(void*),1)),_tmpBF)));void*_tmp79;int*
_tmp7A;void*_tmp7B;int _tmp7C;void*_tmp7D;struct Cyc_timeval*_tmp7E;void*_tmp7F;
int _tmp80;_LL3D: _tmp79=_tmp78.f1;if(*((int*)_tmp79)!= 0)goto _LL3F;_tmp7A=((
struct Cyc_SO_int_struct*)_tmp79)->f1;_tmp7B=_tmp78.f2;if(*((int*)_tmp7B)!= 3)
goto _LL3F;_tmp7C=((struct Cyc_SO_socklen_struct*)_tmp7B)->f1;_LL3E: return
setsockopt_int(fd,level,optname,(const int*)_tmp7A,_tmp7C);_LL3F: _tmp7D=_tmp78.f1;
if(*((int*)_tmp7D)!= 1)goto _LL41;_tmp7E=((struct Cyc_SO_timeval_struct*)_tmp7D)->f1;
_tmp7F=_tmp78.f2;if(*((int*)_tmp7F)!= 3)goto _LL41;_tmp80=((struct Cyc_SO_socklen_struct*)
_tmp7F)->f1;_LL40: return setsockopt_timeval(fd,level,optname,(const struct Cyc_timeval*)
_tmp7E,_tmp80);_LL41:;_LL42:(int)_throw((void*)Cyc_SocketError);_LL3C:;}}struct
Cyc_stat{int st_dev;unsigned int st_ino;unsigned short st_mode;unsigned short
st_nlink;unsigned int st_uid;unsigned int st_gid;int st_rdev;struct Cyc_timespec
st_atimespec;struct Cyc_timespec st_mtimespec;struct Cyc_timespec st_ctimespec;
long long st_size;long long st_blocks;unsigned int st_blksize;unsigned int st_flags;
unsigned int st_gen;int st_lspare;long long st_qspare[2];};struct Cyc_timezone{int
tz_minuteswest;int tz_dsttime;};struct Cyc_itimerval{struct Cyc_timeval it_interval;
struct Cyc_timeval it_value;};int Cyc_gettimeofday(struct Cyc_timeval*,struct Cyc_timezone*);
int Cyc_select(int,struct Cyc_fd_set*,struct Cyc_fd_set*,struct Cyc_fd_set*,struct
Cyc_timeval*);void Cyc_FD_CLR(int,struct Cyc_fd_set*);int Cyc_FD_ISSET(int,struct
Cyc_fd_set*);void Cyc_FD_SET(int,struct Cyc_fd_set*);void Cyc_FD_ZERO(struct Cyc_fd_set*);
int gettimeofday(struct Cyc_timeval*,struct Cyc_timezone*);int Cyc_gettimeofday(
struct Cyc_timeval*a,struct Cyc_timezone*b);int Cyc_gettimeofday(struct Cyc_timeval*
a,struct Cyc_timezone*b){return gettimeofday(a,0);}struct Cyc_timeb{long time;
unsigned short millitm;short timezone;short dstflag;};struct Cyc_tms{unsigned long
tms_utime;unsigned long tms_stime;unsigned long tms_cutime;unsigned long tms_cstime;
};struct Cyc_sockaddr_un{unsigned char sun_len;unsigned char sun_family;char
sun_path[104];};struct Cyc_utsname{char sysname[256];char nodename[256];char release[
256];char version[256];char machine[256];};struct Cyc_tm{int tm_sec;int tm_min;int
tm_hour;int tm_mday;int tm_mon;int tm_year;int tm_wday;int tm_yday;int tm_isdst;long
tm_gmtoff;char*tm_zone;};unsigned long Cyc_strftime(struct _dynforward_ptr,
unsigned long,struct _dynforward_ptr,const struct Cyc_tm*);unsigned long strftime(
char*,unsigned long,const char*,const struct Cyc_tm*);static char _tmp82[27]="fgets: buffer insufficient";
static struct Cyc_Core_Failure_struct Cyc___strftime_failure={Cyc_Core_Failure,{
_tmp82,_tmp82 + 27}};unsigned long Cyc_strftime(struct _dynforward_ptr s,
unsigned long maxsize,struct _dynforward_ptr fmt,const struct Cyc_tm*t);unsigned long
Cyc_strftime(struct _dynforward_ptr s,unsigned long maxsize,struct _dynforward_ptr
fmt,const struct Cyc_tm*t){if(maxsize > _get_dynforward_size(s,sizeof(char)))(int)
_throw((void*)& Cyc___strftime_failure);return strftime((char*)_check_null(
_untag_dynforward_ptr(s,sizeof(char),1)),maxsize,(const char*)_check_null(
_untag_dynforward_ptr(fmt,sizeof(char),1)),t);}struct Cyc_option{struct
_dynforward_ptr name;int has_arg;int*flag;int val;};int Cyc_execlp(const char*,const
char*,struct _dynforward_ptr);int execvp(const char*file,const char**argv);struct
_dynforward_ptr Cyc_getcwd(struct _dynforward_ptr buf,unsigned long size);int Cyc_gethostname(
struct _dynforward_ptr,unsigned long);int Cyc_read(int,struct _dynforward_ptr,
unsigned long);int Cyc_write(int,struct _dynforward_ptr,unsigned long);static char
_tmp84[36]="execlp arg list not NULL-terminated";static struct Cyc_Core_Failure_struct
Cyc___execlp_failure={Cyc_Core_Failure,{_tmp84,_tmp84 + 36}};int Cyc_execlp(const
char*path,const char*arg0,struct _dynforward_ptr argv);static void _tmpC5(
unsigned int*_tmpC4,unsigned int*_tmpC3,const char***_tmpC1){for(*_tmpC4=0;*
_tmpC4 < *_tmpC3;(*_tmpC4)++){(*_tmpC1)[*_tmpC4]=(const char*)0;}}int Cyc_execlp(
const char*path,const char*arg0,struct _dynforward_ptr argv){if(*((const char**)
_check_dynforward_subscript(argv,sizeof(const char*),(int)(_get_dynforward_size(
argv,sizeof(const char*))- 1)))!= (const char*)0)(int)_throw((void*)& Cyc___execlp_failure);{
unsigned int _tmpC4;unsigned int _tmpC3;struct _dynforward_ptr _tmpC2;const char**
_tmpC1;unsigned int _tmpC0;const char**newargs=(const char**)_untag_dynforward_ptr(((
_tmpC0=_get_dynforward_size(argv,sizeof(const char*)),((_tmpC1=(const char**)
_cycalloc(_check_times(sizeof(const char*),_tmpC0 + 1)),((_tmpC2=_tag_dynforward(
_tmpC1,sizeof(const char*),_tmpC0 + 1),((((_tmpC3=_tmpC0,((_tmpC5(& _tmpC4,& _tmpC3,&
_tmpC1),_tmpC1[_tmpC3]=(const char*)0)))),_tmpC2)))))))),sizeof(const char*),1);{
const char*_tmpCA;const char*_tmpC9;struct _dyneither_ptr _tmpC8;const char**_tmpC7;(
_tmpC8=_dyneither_ptr_plus(((_tmpC7=newargs,_tag_dyneither(_tmpC7,sizeof(const
char*),_get_zero_arr_size_voidstar(_tmpC7,1)))),sizeof(const char*),0),((_tmpC9=*((
const char**)_check_dyneither_subscript(_tmpC8,sizeof(const char*),0)),((_tmpCA=(
const char*)arg0,((_get_dyneither_size(_tmpC8,sizeof(const char*))== 1  && (_tmpC9
== 0  && _tmpCA != 0)?_throw_arraybounds(): 1,*((const char**)_tmpC8.curr)=_tmpCA)))))));}{
int i=0;for(0;i < _get_dynforward_size(argv,sizeof(const char*))- 1;++ i){const char*
_tmpCF;const char*_tmpCE;struct _dyneither_ptr _tmpCD;const char**_tmpCC;(_tmpCD=
_dyneither_ptr_plus(((_tmpCC=newargs,_tag_dyneither(_tmpCC,sizeof(const char*),
_get_zero_arr_size_voidstar(_tmpCC,1)))),sizeof(const char*),i + 1),((_tmpCE=*((
const char**)_check_dyneither_subscript(_tmpCD,sizeof(const char*),0)),((_tmpCF=*((
const char**)_check_dynforward_subscript(argv,sizeof(const char*),i)),((
_get_dyneither_size(_tmpCD,sizeof(const char*))== 1  && (_tmpCE == 0  && _tmpCF != 0)?
_throw_arraybounds(): 1,*((const char**)_tmpCD.curr)=_tmpCF)))))));}}return execvp(
path,(const char**)newargs);}}char*getcwd(char*buf,unsigned long size);static char
_tmp92[29]="getcwd: invalid buf argument";static struct Cyc_Core_Failure_struct Cyc___getcwd_failure={
Cyc_Core_Failure,{_tmp92,_tmp92 + 29}};struct _dynforward_ptr Cyc_getcwd(struct
_dynforward_ptr buf,unsigned long size);struct _dynforward_ptr Cyc_getcwd(struct
_dynforward_ptr buf,unsigned long size){if(_get_dynforward_size(buf,sizeof(char))< 
size)(int)_throw((void*)& Cyc___getcwd_failure);{char*_tmp93=getcwd((char*)
_check_null(_untag_dynforward_ptr(buf,sizeof(char),1)),size);return(unsigned int)
_tmp93?buf: _tag_dynforward(0,0,0);}}int gethostname(char*,unsigned long);static
char _tmp95[34]="gethostname: called with NULL buf";static struct Cyc_Core_Failure_struct
Cyc___gethostname_failure_1={Cyc_Core_Failure,{_tmp95,_tmp95 + 34}};static char
_tmp97[42]="gethostname: called with count > buf.size";static struct Cyc_Core_Failure_struct
Cyc___gethostname_failure_2={Cyc_Core_Failure,{_tmp97,_tmp97 + 42}};int Cyc_gethostname(
struct _dynforward_ptr buf,unsigned long count);int Cyc_gethostname(struct
_dynforward_ptr buf,unsigned long count){if(!((unsigned int)buf.curr))(int)_throw((
void*)& Cyc___gethostname_failure_1);else{if(count > _get_dynforward_size(buf,
sizeof(char)))(int)_throw((void*)& Cyc___gethostname_failure_2);return gethostname((
char*)_untag_dynforward_ptr(buf,sizeof(char),1),count);}}int read(int fd,char*buf,
unsigned long count);static char _tmp99[35]="read: called with count > buf.size";
static struct Cyc_Core_Failure_struct Cyc___read_failure={Cyc_Core_Failure,{_tmp99,
_tmp99 + 35}};int Cyc_read(int fd,struct _dynforward_ptr buf,unsigned long count);int
Cyc_read(int fd,struct _dynforward_ptr buf,unsigned long count){if(count > 
_get_dynforward_size(buf,sizeof(char)))(int)_throw((void*)& Cyc___read_failure);
return read(fd,(char*)_check_null(_untag_dynforward_ptr(buf,sizeof(char),1)),
count);}int write(int fd,const char*buf,unsigned long count);static char _tmp9B[36]="write: called with count > buf.size";
static struct Cyc_Core_Failure_struct Cyc___write_failure={Cyc_Core_Failure,{_tmp9B,
_tmp9B + 36}};int Cyc_write(int fd,struct _dynforward_ptr buf,unsigned long count);int
Cyc_write(int fd,struct _dynforward_ptr buf,unsigned long count){if(count > 
_get_dynforward_size(buf,sizeof(char)))(int)_throw((void*)& Cyc___write_failure);
return write(fd,(const char*)_untag_dynforward_ptr(buf,sizeof(char),1),count);}
struct Cyc_utimbuf{long actime;long modtime;};
