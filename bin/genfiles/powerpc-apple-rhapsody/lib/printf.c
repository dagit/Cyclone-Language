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
_dynforward_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;extern char Cyc_Core_Open_Region[
16];extern char Cyc_Core_Free_Region[16];struct Cyc___cycFILE;extern struct Cyc___cycFILE*
Cyc_stdout;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;
struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dynforward_ptr,
struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
int Cyc_printf(struct _dynforward_ptr,struct _dynforward_ptr);int Cyc_putc(int,
struct Cyc___cycFILE*);struct _dynforward_ptr Cyc_rprintf(struct _RegionHandle*,
struct _dynforward_ptr,struct _dynforward_ptr);int Cyc_snprintf(struct
_dynforward_ptr,unsigned long,struct _dynforward_ptr,struct _dynforward_ptr);int
Cyc_sprintf(struct _dynforward_ptr,struct _dynforward_ptr,struct _dynforward_ptr);
int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dynforward_ptr,struct _dynforward_ptr);
int Cyc_vprintf(struct _dynforward_ptr,struct _dynforward_ptr);struct
_dynforward_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dynforward_ptr,struct
_dynforward_ptr);int Cyc_vsnprintf(struct _dynforward_ptr,unsigned long,struct
_dynforward_ptr,struct _dynforward_ptr);int Cyc_vsprintf(struct _dynforward_ptr,
struct _dynforward_ptr,struct _dynforward_ptr);extern char Cyc_FileCloseError[19];
extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;struct
_dynforward_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern
char Cyc_List_List_mismatch[18];extern char Cyc_List_Nth[8];typedef struct{int quot;
int rem;}Cyc_div_t;typedef struct{long quot;long rem;}Cyc_ldiv_t;void*abort();void
exit(int);double modf(double,double*);static struct _dynforward_ptr Cyc_parg2string(
void*x);static struct _dynforward_ptr Cyc_parg2string(void*x){void*_tmp0=x;_LL1: if(*((
int*)_tmp0)!= 0)goto _LL3;_LL2: {const char*_tmpBE;return(_tmpBE="string",
_tag_dynforward(_tmpBE,sizeof(char),_get_zero_arr_size_char(_tmpBE,7)));}_LL3:
if(*((int*)_tmp0)!= 1)goto _LL5;_LL4: {const char*_tmpBF;return(_tmpBF="int",
_tag_dynforward(_tmpBF,sizeof(char),_get_zero_arr_size_char(_tmpBF,4)));}_LL5:
if(*((int*)_tmp0)!= 2)goto _LL7;_LL6: {const char*_tmpC0;return(_tmpC0="double",
_tag_dynforward(_tmpC0,sizeof(char),_get_zero_arr_size_char(_tmpC0,7)));}_LL7:
if(*((int*)_tmp0)!= 3)goto _LL9;_LL8: {const char*_tmpC1;return(_tmpC1="long double",
_tag_dynforward(_tmpC1,sizeof(char),_get_zero_arr_size_char(_tmpC1,12)));}_LL9:
if(*((int*)_tmp0)!= 4)goto _LLB;_LLA: {const char*_tmpC2;return(_tmpC2="short *",
_tag_dynforward(_tmpC2,sizeof(char),_get_zero_arr_size_char(_tmpC2,8)));}_LLB:
if(*((int*)_tmp0)!= 5)goto _LL0;_LLC: {const char*_tmpC3;return(_tmpC3="unsigned long *",
_tag_dynforward(_tmpC3,sizeof(char),_get_zero_arr_size_char(_tmpC3,16)));}_LL0:;}
static void*Cyc_badarg(struct _dynforward_ptr s);static void*Cyc_badarg(struct
_dynforward_ptr s){struct Cyc_Core_Invalid_argument_struct _tmpC6;struct Cyc_Core_Invalid_argument_struct*
_tmpC5;return(void*)_throw((void*)((_tmpC5=_cycalloc(sizeof(*_tmpC5)),((_tmpC5[0]=((
_tmpC6.tag=Cyc_Core_Invalid_argument,((_tmpC6.f1=s,_tmpC6)))),_tmpC5)))));}
static int Cyc_va_arg_int(struct _dynforward_ptr ap);static int Cyc_va_arg_int(struct
_dynforward_ptr ap){void*_tmp9=*((void**)_check_dynforward_subscript(ap,sizeof(
void*),0));unsigned long _tmpA;_LLE: if(*((int*)_tmp9)!= 1)goto _LL10;_tmpA=((
struct Cyc_Int_pa_struct*)_tmp9)->f1;_LLF: return(int)_tmpA;_LL10:;_LL11: {const
char*_tmpC7;return((int(*)(struct _dynforward_ptr s))Cyc_badarg)(((_tmpC7="printf expected int",
_tag_dynforward(_tmpC7,sizeof(char),_get_zero_arr_size_char(_tmpC7,20)))));}_LLD:;}
static long Cyc_va_arg_long(struct _dynforward_ptr ap);static long Cyc_va_arg_long(
struct _dynforward_ptr ap){void*_tmpC=*((void**)_check_dynforward_subscript(ap,
sizeof(void*),0));unsigned long _tmpD;_LL13: if(*((int*)_tmpC)!= 1)goto _LL15;_tmpD=((
struct Cyc_Int_pa_struct*)_tmpC)->f1;_LL14: return(long)_tmpD;_LL15:;_LL16: {const
char*_tmpC8;return((long(*)(struct _dynforward_ptr s))Cyc_badarg)(((_tmpC8="printf expected int",
_tag_dynforward(_tmpC8,sizeof(char),_get_zero_arr_size_char(_tmpC8,20)))));}
_LL12:;}static unsigned long Cyc_va_arg_ulong(struct _dynforward_ptr ap);static
unsigned long Cyc_va_arg_ulong(struct _dynforward_ptr ap){void*_tmpF=*((void**)
_check_dynforward_subscript(ap,sizeof(void*),0));unsigned long _tmp10;_LL18: if(*((
int*)_tmpF)!= 1)goto _LL1A;_tmp10=((struct Cyc_Int_pa_struct*)_tmpF)->f1;_LL19:
return(unsigned long)_tmp10;_LL1A:;_LL1B: {const char*_tmpC9;return((
unsigned long(*)(struct _dynforward_ptr s))Cyc_badarg)(((_tmpC9="printf expected int",
_tag_dynforward(_tmpC9,sizeof(char),_get_zero_arr_size_char(_tmpC9,20)))));}
_LL17:;}static unsigned long Cyc_va_arg_uint(struct _dynforward_ptr ap);static
unsigned long Cyc_va_arg_uint(struct _dynforward_ptr ap){void*_tmp12=*((void**)
_check_dynforward_subscript(ap,sizeof(void*),0));unsigned long _tmp13;_LL1D: if(*((
int*)_tmp12)!= 1)goto _LL1F;_tmp13=((struct Cyc_Int_pa_struct*)_tmp12)->f1;_LL1E:
return(unsigned int)_tmp13;_LL1F:;_LL20: {const char*_tmpCA;return((unsigned long(*)(
struct _dynforward_ptr s))Cyc_badarg)(((_tmpCA="printf expected int",
_tag_dynforward(_tmpCA,sizeof(char),_get_zero_arr_size_char(_tmpCA,20)))));}
_LL1C:;}static double Cyc_va_arg_double(struct _dynforward_ptr ap);static double Cyc_va_arg_double(
struct _dynforward_ptr ap){void*_tmp15=*((void**)_check_dynforward_subscript(ap,
sizeof(void*),0));double _tmp16;long double _tmp17;_LL22: if(*((int*)_tmp15)!= 2)
goto _LL24;_tmp16=((struct Cyc_Double_pa_struct*)_tmp15)->f1;_LL23: return _tmp16;
_LL24: if(*((int*)_tmp15)!= 3)goto _LL26;_tmp17=((struct Cyc_LongDouble_pa_struct*)
_tmp15)->f1;_LL25: return(double)_tmp17;_LL26:;_LL27: {const char*_tmpCE;void*
_tmpCD[1];struct Cyc_String_pa_struct _tmpCC;(int)_throw(Cyc_badarg((struct
_dynforward_ptr)((_tmpCC.tag=0,((_tmpCC.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_parg2string(*((void**)_check_dynforward_subscript(ap,sizeof(
void*),0)))),((_tmpCD[0]=& _tmpCC,Cyc_aprintf(((_tmpCE="printf expected double but found %s",
_tag_dynforward(_tmpCE,sizeof(char),_get_zero_arr_size_char(_tmpCE,36)))),
_tag_dynforward(_tmpCD,sizeof(void*),1))))))))));}_LL21:;}static short*Cyc_va_arg_short_ptr(
struct _dynforward_ptr ap);static short*Cyc_va_arg_short_ptr(struct _dynforward_ptr
ap){void*_tmp1B=*((void**)_check_dynforward_subscript(ap,sizeof(void*),0));short*
_tmp1C;_LL29: if(*((int*)_tmp1B)!= 4)goto _LL2B;_tmp1C=((struct Cyc_ShortPtr_pa_struct*)
_tmp1B)->f1;_LL2A: return _tmp1C;_LL2B:;_LL2C: {const char*_tmpCF;(int)_throw(Cyc_badarg(((
_tmpCF="printf expected short pointer",_tag_dynforward(_tmpCF,sizeof(char),
_get_zero_arr_size_char(_tmpCF,30))))));}_LL28:;}static unsigned long*Cyc_va_arg_int_ptr(
struct _dynforward_ptr ap);static unsigned long*Cyc_va_arg_int_ptr(struct
_dynforward_ptr ap){void*_tmp1E=*((void**)_check_dynforward_subscript(ap,sizeof(
void*),0));unsigned long*_tmp1F;_LL2E: if(*((int*)_tmp1E)!= 5)goto _LL30;_tmp1F=((
struct Cyc_IntPtr_pa_struct*)_tmp1E)->f1;_LL2F: return _tmp1F;_LL30:;_LL31: {const
char*_tmpD0;(int)_throw(Cyc_badarg(((_tmpD0="printf expected int pointer",
_tag_dynforward(_tmpD0,sizeof(char),_get_zero_arr_size_char(_tmpD0,28))))));}
_LL2D:;}static struct _dynforward_ptr Cyc_va_arg_string(struct _dynforward_ptr ap);
static struct _dynforward_ptr Cyc_va_arg_string(struct _dynforward_ptr ap){void*
_tmp21=*((void**)_check_dynforward_subscript(ap,sizeof(void*),0));struct
_dynforward_ptr _tmp22;_LL33: if(*((int*)_tmp21)!= 0)goto _LL35;_tmp22=((struct Cyc_String_pa_struct*)
_tmp21)->f1;_LL34: return _tmp22;_LL35:;_LL36: {const char*_tmpD1;(int)_throw(Cyc_badarg(((
_tmpD1="printf expected string",_tag_dynforward(_tmpD1,sizeof(char),
_get_zero_arr_size_char(_tmpD1,23))))));}_LL32:;}int Cyc___cvt_double(double
number,int prec,int flags,int*signp,int fmtch,struct _dyneither_ptr startp,struct
_dyneither_ptr endp);enum Cyc_BASE{Cyc_OCT  = 0,Cyc_DEC  = 1,Cyc_HEX  = 2};static int
Cyc__IO_sputn(int(*ioputc)(int,void*),void*ioputc_env,struct _dynforward_ptr s,int
howmany);static int Cyc__IO_sputn(int(*ioputc)(int,void*),void*ioputc_env,struct
_dynforward_ptr s,int howmany){int i=0;while(i < howmany){if(_get_dynforward_size(s,
sizeof(char))== 0  || ioputc((int)*((const char*)_check_dynforward_subscript(s,
sizeof(char),0)),ioputc_env)== - 1)return i;_dynforward_ptr_inplace_plus(& s,
sizeof(char),1);++ i;}return i;}static int Cyc__IO_nzsputn(int(*ioputc)(int,void*),
void*ioputc_env,struct _dynforward_ptr s,int howmany);static int Cyc__IO_nzsputn(int(*
ioputc)(int,void*),void*ioputc_env,struct _dynforward_ptr s,int howmany){int i=0;
while(i < howmany){char c;if((_get_dynforward_size(s,sizeof(char))== 0  || (c=*((
const char*)_check_dynforward_subscript(s,sizeof(char),0)))== 0) || ioputc((int)c,
ioputc_env)== - 1)return i;_dynforward_ptr_inplace_plus(& s,sizeof(char),1);++ i;}
return i;}static int Cyc__IO_padn(int(*ioputc)(int,void*),void*ioputc_env,char c,int
howmany);static int Cyc__IO_padn(int(*ioputc)(int,void*),void*ioputc_env,char c,int
howmany){int i=0;while(i < howmany){if(ioputc((int)c,ioputc_env)== - 1)return i;++ i;}
return i;}static struct _dynforward_ptr Cyc_my_memchr(struct _dynforward_ptr s,char c,
int n);static struct _dynforward_ptr Cyc_my_memchr(struct _dynforward_ptr s,char c,int n){
int sz=(int)_get_dynforward_size(s,sizeof(char));n=n < sz?n: sz;for(0;n != 0;(n --,
_dynforward_ptr_inplace_plus_post(& s,sizeof(char),1))){if(*((const char*)
_check_dynforward_subscript(s,sizeof(char),0))== c)return s;}return(struct
_dynforward_ptr)_tag_dynforward(0,0,0);}static struct _dynforward_ptr Cyc_my_nzmemchr(
struct _dynforward_ptr s,char c,int n);static struct _dynforward_ptr Cyc_my_nzmemchr(
struct _dynforward_ptr s,char c,int n){int sz=(int)_get_dynforward_size(s,sizeof(char));
n=n < sz?n: sz;for(0;n != 0;(n --,_dynforward_ptr_inplace_plus_post(& s,sizeof(char),
1))){if(*((const char*)_check_dynforward_subscript(s,sizeof(char),0))== c)return s;}
return(struct _dynforward_ptr)_tag_dynforward(0,0,0);}static unsigned long Cyc_my_strlen(
struct _dynforward_ptr s);static unsigned long Cyc_my_strlen(struct _dynforward_ptr s){
unsigned int sz=_get_dynforward_size(s,sizeof(char));unsigned int i=0;while(i < sz
 && ((const char*)s.curr)[(int)i]!= 0){++ i;}return i;}int Cyc__IO_vfprintf(int(*
ioputc)(int,void*),void*ioputc_env,struct _dynforward_ptr fmt0,struct
_dynforward_ptr ap);int Cyc__IO_vfprintf(int(*ioputc)(int,void*),void*ioputc_env,
struct _dynforward_ptr fmt0,struct _dynforward_ptr ap){struct _dynforward_ptr fmt;
register int ch;register int n;struct _dyneither_ptr cp=_tag_dyneither(0,0,0);struct
_dynforward_ptr cp2=(struct _dynforward_ptr)_tag_dynforward(0,0,0);struct
_dynforward_ptr cp3=(struct _dynforward_ptr)_tag_dynforward(0,0,0);int which_cp;
struct _dynforward_ptr fmark;register int flags;int ret;int width;int prec;char sign;
char sign_string[2]={'\000','\000'};int softsign=0;double _double;int fpprec;
unsigned long _ulong;int dprec;int dpad;int fieldsz;int size=0;char buf[349];{
unsigned int _tmp49=348;unsigned int i;for(i=0;i < _tmp49;i ++){buf[i]='\000';}buf[
_tmp49]=(char)0;}{char ox[2]={'\000','\000'};enum Cyc_BASE base;fmt=fmt0;ret=0;
for(0;1;0){fmark=fmt;{unsigned int fmt_sz=_get_dynforward_size(fmt,sizeof(char));
for(n=0;(n < fmt_sz  && (ch=(int)*((const char*)_check_dynforward_subscript(fmt,
sizeof(char),n)))!= '\000') && ch != '%';++ n){;}fmt=_dynforward_ptr_plus(fmt,
sizeof(char),n);if((n=(fmt.curr - fmark.curr)/ sizeof(char))!= 0){do{if(Cyc__IO_sputn(
ioputc,ioputc_env,(struct _dynforward_ptr)fmark,n)!= n)goto error;}while(0);ret +=n;}
if(ch == '\000')goto done;_dynforward_ptr_inplace_plus(& fmt,sizeof(char),1);flags=
0;dprec=0;fpprec=0;width=0;prec=- 1;sign='\000';rflag: ch=(int)*((const char*)
_check_dynforward_subscript(_dynforward_ptr_inplace_plus_post(& fmt,sizeof(char),
1),sizeof(char),0));reswitch: which_cp=0;switch(ch){case ' ': _LL37: if(!((int)sign))
sign=' ';goto rflag;case '#': _LL38: flags |=8;goto rflag;case '*': _LL39: width=Cyc_va_arg_int(
ap);_dynforward_ptr_inplace_plus(& ap,sizeof(void*),1);if(width >= 0)goto rflag;
width=- width;goto _LL3A;case '-': _LL3A: flags |=16;flags &=~ 32;goto rflag;case '+':
_LL3B: sign='+';goto rflag;case '.': _LL3C: if((ch=(int)*((const char*)
_check_dynforward_subscript(_dynforward_ptr_inplace_plus_post(& fmt,sizeof(char),
1),sizeof(char),0)))== '*'){n=Cyc_va_arg_int(ap);_dynforward_ptr_inplace_plus(&
ap,sizeof(void*),1);prec=n < 0?- 1: n;goto rflag;}n=0;while((unsigned int)(ch - '0')
<= 9){n=10 * n + (ch - '0');ch=(int)*((const char*)_check_dynforward_subscript(
_dynforward_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0));}prec=n < 
0?- 1: n;goto reswitch;case '0': _LL3D: if(!(flags & 16))flags |=32;goto rflag;case '1':
_LL3E: goto _LL3F;case '2': _LL3F: goto _LL40;case '3': _LL40: goto _LL41;case '4': _LL41:
goto _LL42;case '5': _LL42: goto _LL43;case '6': _LL43: goto _LL44;case '7': _LL44: goto
_LL45;case '8': _LL45: goto _LL46;case '9': _LL46: n=0;do{n=10 * n + (ch - '0');ch=(int)*((
const char*)_check_dynforward_subscript(_dynforward_ptr_inplace_plus_post(& fmt,
sizeof(char),1),sizeof(char),0));}while((unsigned int)(ch - '0')<= 9);width=n;
goto reswitch;case 'L': _LL47: flags |=2;goto rflag;case 'h': _LL48: flags |=4;goto rflag;
case 'l': _LL49: flags |=1;goto rflag;case 'c': _LL4A:{char*_tmpD2;cp=((_tmpD2=buf,
_tag_dyneither(_tmpD2,sizeof(char),_get_zero_arr_size_char(_tmpD2,349))));}{char
_tmpD5;char _tmpD4;struct _dyneither_ptr _tmpD3;(_tmpD3=cp,((_tmpD4=*((char*)
_check_dyneither_subscript(_tmpD3,sizeof(char),0)),((_tmpD5=(char)Cyc_va_arg_int(
ap),((_get_dyneither_size(_tmpD3,sizeof(char))== 1  && (_tmpD4 == '\000'  && _tmpD5
!= '\000')?_throw_arraybounds(): 1,*((char*)_tmpD3.curr)=_tmpD5)))))));}
_dynforward_ptr_inplace_plus(& ap,sizeof(void*),1);size=1;sign='\000';break;case 'D':
_LL4B: flags |=1;goto _LL4C;case 'd': _LL4C: goto _LL4D;case 'i': _LL4D: _ulong=(
unsigned long)(flags & 1?Cyc_va_arg_long(ap):(flags & 4?(long)((short)Cyc_va_arg_int(
ap)):(long)Cyc_va_arg_int(ap)));_dynforward_ptr_inplace_plus(& ap,sizeof(void*),1);
if((long)_ulong < 0){_ulong=- _ulong;sign='-';}base=Cyc_DEC;goto number;case 'e':
_LL4E: goto _LL4F;case 'E': _LL4F: goto _LL50;case 'f': _LL50: goto _LL51;case 'F': _LL51:
goto _LL52;case 'g': _LL52: goto _LL53;case 'G': _LL53: _double=Cyc_va_arg_double(ap);
_dynforward_ptr_inplace_plus(& ap,sizeof(void*),1);if(prec > 39){if(ch != 'g'  && ch
!= 'G'  || flags & 8)fpprec=prec - 39;prec=39;}else{if(prec == - 1)prec=6;}{char*
_tmpD6;cp=((_tmpD6=buf,_tag_dyneither(_tmpD6,sizeof(char),
_get_zero_arr_size_char(_tmpD6,349))));}{char _tmpD9;char _tmpD8;struct
_dyneither_ptr _tmpD7;(_tmpD7=cp,((_tmpD8=*((char*)_check_dyneither_subscript(
_tmpD7,sizeof(char),0)),((_tmpD9='\000',((_get_dyneither_size(_tmpD7,sizeof(char))
== 1  && (_tmpD8 == '\000'  && _tmpD9 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmpD7.curr)=_tmpD9)))))));}{char*_tmpDA;size=Cyc___cvt_double(_double,prec,
flags,& softsign,ch,cp,_dyneither_ptr_plus(((_tmpDA=buf,_tag_dyneither(_tmpDA,
sizeof(char),_get_zero_arr_size_char(_tmpDA,349)))),sizeof(char),(int)(sizeof(
buf)- 1)));}if(softsign)sign='-';if(*((char*)_check_dyneither_subscript(cp,
sizeof(char),0))== '\000')_dyneither_ptr_inplace_plus(& cp,sizeof(char),1);break;
case 'n': _LL54: if(flags & 1)*Cyc_va_arg_int_ptr(ap)=(unsigned long)ret;else{if(
flags & 4)*Cyc_va_arg_short_ptr(ap)=(short)ret;else{*Cyc_va_arg_int_ptr(ap)=(
unsigned long)ret;}}_dynforward_ptr_inplace_plus(& ap,sizeof(void*),1);continue;
case 'O': _LL55: flags |=1;goto _LL56;case 'o': _LL56: _ulong=flags & 1?Cyc_va_arg_ulong(
ap):(flags & 4?(unsigned long)((unsigned short)Cyc_va_arg_int(ap)):(unsigned long)
Cyc_va_arg_uint(ap));_dynforward_ptr_inplace_plus(& ap,sizeof(void*),1);base=Cyc_OCT;
goto nosign;case 'p': _LL57: _ulong=(unsigned long)Cyc_va_arg_long(ap);
_dynforward_ptr_inplace_plus(& ap,sizeof(void*),1);base=Cyc_HEX;flags |=64;ch=(int)'x';
goto nosign;case 's': _LL58: {struct _dynforward_ptr b=Cyc_va_arg_string(ap);
_dynforward_ptr_inplace_plus(& ap,sizeof(void*),1);which_cp=3;cp3=b;if(prec >= 0){
struct _dynforward_ptr p=Cyc_my_nzmemchr(cp3,'\000',prec);if(p.curr != ((struct
_dynforward_ptr)_tag_dynforward(0,0,0)).curr){size=(p.curr - cp3.curr)/ sizeof(
char);if(size > prec)size=prec;}else{size=prec;}}else{size=(int)Cyc_my_strlen(cp3);}
sign='\000';break;}case 'U': _LL59: flags |=1;goto _LL5A;case 'u': _LL5A: _ulong=flags & 
1?Cyc_va_arg_ulong(ap):(flags & 4?(unsigned long)((unsigned short)Cyc_va_arg_int(
ap)):(unsigned long)Cyc_va_arg_uint(ap));_dynforward_ptr_inplace_plus(& ap,
sizeof(void*),1);base=Cyc_DEC;goto nosign;case 'X': _LL5B: goto _LL5C;case 'x': _LL5C:
_ulong=flags & 1?Cyc_va_arg_ulong(ap):(flags & 4?(unsigned long)((unsigned short)
Cyc_va_arg_int(ap)):(unsigned long)Cyc_va_arg_uint(ap));
_dynforward_ptr_inplace_plus(& ap,sizeof(void*),1);base=Cyc_HEX;if(flags & 8  && 
_ulong != 0)flags |=64;nosign: sign='\000';number: if((dprec=prec)>= 0)flags &=~ 32;{
char*_tmpDB;cp=_dyneither_ptr_plus(((_tmpDB=buf,_tag_dyneither(_tmpDB,sizeof(
char),_get_zero_arr_size_char(_tmpDB,349)))),sizeof(char),(308 + 39)+ 1);}if(
_ulong != 0  || prec != 0){struct _dynforward_ptr xdigs;switch(base){case Cyc_OCT:
_LL5E: do{{char _tmpDE;char _tmpDD;struct _dyneither_ptr _tmpDC;(_tmpDC=
_dyneither_ptr_inplace_plus(& cp,sizeof(char),-1),((_tmpDD=*((char*)
_check_dyneither_subscript(_tmpDC,sizeof(char),0)),((_tmpDE=(char)((_ulong & 7)+ '0'),((
_get_dyneither_size(_tmpDC,sizeof(char))== 1  && (_tmpDD == '\000'  && _tmpDE != '\000')?
_throw_arraybounds(): 1,*((char*)_tmpDC.curr)=_tmpDE)))))));}_ulong >>=3;}while((
int)_ulong);if(flags & 8  && *((char*)_check_dyneither_subscript(cp,sizeof(char),0))
!= '0'){char _tmpE1;char _tmpE0;struct _dyneither_ptr _tmpDF;(_tmpDF=
_dyneither_ptr_inplace_plus(& cp,sizeof(char),-1),((_tmpE0=*((char*)
_check_dyneither_subscript(_tmpDF,sizeof(char),0)),((_tmpE1='0',((
_get_dyneither_size(_tmpDF,sizeof(char))== 1  && (_tmpE0 == '\000'  && _tmpE1 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmpDF.curr)=_tmpE1)))))));}break;case Cyc_DEC:
_LL5F: while(_ulong >= 10){{char _tmpE4;char _tmpE3;struct _dyneither_ptr _tmpE2;(
_tmpE2=_dyneither_ptr_inplace_plus(& cp,sizeof(char),-1),((_tmpE3=*((char*)
_check_dyneither_subscript(_tmpE2,sizeof(char),0)),((_tmpE4=(char)(_ulong % 10 + '0'),((
_get_dyneither_size(_tmpE2,sizeof(char))== 1  && (_tmpE3 == '\000'  && _tmpE4 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmpE2.curr)=_tmpE4)))))));}_ulong /=10;}{char
_tmpE7;char _tmpE6;struct _dyneither_ptr _tmpE5;(_tmpE5=_dyneither_ptr_inplace_plus(&
cp,sizeof(char),-1),((_tmpE6=*((char*)_check_dyneither_subscript(_tmpE5,sizeof(
char),0)),((_tmpE7=(char)(_ulong + '0'),((_get_dyneither_size(_tmpE5,sizeof(char))
== 1  && (_tmpE6 == '\000'  && _tmpE7 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmpE5.curr)=_tmpE7)))))));}break;case Cyc_HEX: _LL60: if(ch == 'X'){const char*
_tmpE8;xdigs=((_tmpE8="0123456789ABCDEF",_tag_dynforward(_tmpE8,sizeof(char),
_get_zero_arr_size_char(_tmpE8,17))));}else{const char*_tmpE9;xdigs=((_tmpE9="0123456789abcdef",
_tag_dynforward(_tmpE9,sizeof(char),_get_zero_arr_size_char(_tmpE9,17))));}do{{
char _tmpEC;char _tmpEB;struct _dyneither_ptr _tmpEA;(_tmpEA=
_dyneither_ptr_inplace_plus(& cp,sizeof(char),-1),((_tmpEB=*((char*)
_check_dyneither_subscript(_tmpEA,sizeof(char),0)),((_tmpEC=*((const char*)
_check_dynforward_subscript(xdigs,sizeof(char),(int)(_ulong & 15))),((
_get_dyneither_size(_tmpEA,sizeof(char))== 1  && (_tmpEB == '\000'  && _tmpEC != '\000')?
_throw_arraybounds(): 1,*((char*)_tmpEA.curr)=_tmpEC)))))));}_ulong >>=4;}while((
int)_ulong);break;}}{char*_tmpED;size=((_dyneither_ptr_plus(((_tmpED=buf,
_tag_dyneither(_tmpED,sizeof(char),_get_zero_arr_size_char(_tmpED,349)))),
sizeof(char),(308 + 39)+ 1)).curr - cp.curr)/ sizeof(char);}skipsize: break;
default: _LL5D: if(ch == '\000')goto done;{char*_tmpEE;cp=((_tmpEE=buf,
_tag_dyneither(_tmpEE,sizeof(char),_get_zero_arr_size_char(_tmpEE,349))));}{char
_tmpF1;char _tmpF0;struct _dyneither_ptr _tmpEF;(_tmpEF=cp,((_tmpF0=*((char*)
_check_dyneither_subscript(_tmpEF,sizeof(char),0)),((_tmpF1=(char)ch,((
_get_dyneither_size(_tmpEF,sizeof(char))== 1  && (_tmpF0 == '\000'  && _tmpF1 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmpEF.curr)=_tmpF1)))))));}size=1;sign='\000';
break;}fieldsz=size + fpprec;dpad=dprec - size;if(dpad < 0)dpad=0;if((int)sign)++
fieldsz;else{if(flags & 64)fieldsz +=2;}fieldsz +=dpad;if((flags & (16 | 32))== 0){
if(Cyc__IO_padn(ioputc,ioputc_env,' ',width - fieldsz)< width - fieldsz)goto error;}
if((int)sign){{char _tmpF6;char _tmpF5;struct _dyneither_ptr _tmpF4;char*_tmpF3;(
_tmpF4=_dyneither_ptr_plus(((_tmpF3=sign_string,_tag_dyneither(_tmpF3,sizeof(
char),_get_zero_arr_size_char(_tmpF3,2)))),sizeof(char),0),((_tmpF5=*((char*)
_check_dyneither_subscript(_tmpF4,sizeof(char),0)),((_tmpF6=sign,((
_get_dyneither_size(_tmpF4,sizeof(char))== 1  && (_tmpF5 == '\000'  && _tmpF6 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmpF4.curr)=_tmpF6)))))));}do{char*_tmpF7;if(
Cyc__IO_sputn(ioputc,ioputc_env,(struct _dynforward_ptr)((_tmpF7=sign_string,
_tag_dynforward(_tmpF7,sizeof(char),_get_zero_arr_size_char(_tmpF7,2)))),1)!= 1)
goto error;}while(0);}else{if(flags & 64){ox[0]='0';ox[1]=(char)ch;do{if(Cyc__IO_nzsputn(
ioputc,ioputc_env,_tag_dynforward(ox,sizeof(char),2),2)!= 2)goto error;}while(0);}}
if((flags & (16 | 32))== 32){if(Cyc__IO_padn(ioputc,ioputc_env,'0',width - fieldsz)
< width - fieldsz)goto error;}if(Cyc__IO_padn(ioputc,ioputc_env,'0',dpad)< dpad)
goto error;if(which_cp == 0)do{if(Cyc__IO_sputn(ioputc,ioputc_env,
_dyneither_to_dynforward(cp),size)!= size)goto error;}while(0);else{if(which_cp == 
2)do{if(Cyc__IO_sputn(ioputc,ioputc_env,(struct _dynforward_ptr)cp2,size)!= size)
goto error;}while(0);else{if(which_cp == 3)do{if(Cyc__IO_nzsputn(ioputc,ioputc_env,(
struct _dynforward_ptr)cp3,size)!= size)goto error;}while(0);}}if(Cyc__IO_padn(
ioputc,ioputc_env,'0',fpprec)< fpprec)goto error;if(flags & 16){if(Cyc__IO_padn(
ioputc,ioputc_env,' ',width - fieldsz)< width - fieldsz)goto error;}ret +=width > 
fieldsz?width: fieldsz;}}done: return ret;error: return - 1;}}static struct
_dyneither_ptr Cyc_exponent(struct _dyneither_ptr p,int exp,int fmtch);static struct
_dyneither_ptr Cyc_exponent(struct _dyneither_ptr p,int exp,int fmtch){struct
_dyneither_ptr t;char expbuffer[309];{unsigned int _tmp63=308;unsigned int i;for(i=0;
i < _tmp63;i ++){expbuffer[i]='0';}expbuffer[_tmp63]=(char)0;}{char*_tmpF8;struct
_dynforward_ptr expbuf=(_tmpF8=(char*)expbuffer,_tag_dynforward(_tmpF8,sizeof(
char),_get_zero_arr_size_char(_tmpF8,309)));{char _tmpFB;char _tmpFA;struct
_dyneither_ptr _tmpF9;(_tmpF9=_dyneither_ptr_inplace_plus_post(& p,sizeof(char),1),((
_tmpFA=*((char*)_check_dyneither_subscript(_tmpF9,sizeof(char),0)),((_tmpFB=(
char)fmtch,((_get_dyneither_size(_tmpF9,sizeof(char))== 1  && (_tmpFA == '\000'
 && _tmpFB != '\000')?_throw_arraybounds(): 1,*((char*)_tmpF9.curr)=_tmpFB)))))));}
if(exp < 0){exp=- exp;{char _tmpFE;char _tmpFD;struct _dyneither_ptr _tmpFC;(_tmpFC=
_dyneither_ptr_inplace_plus_post(& p,sizeof(char),1),((_tmpFD=*((char*)
_check_dyneither_subscript(_tmpFC,sizeof(char),0)),((_tmpFE='-',((
_get_dyneither_size(_tmpFC,sizeof(char))== 1  && (_tmpFD == '\000'  && _tmpFE != '\000')?
_throw_arraybounds(): 1,*((char*)_tmpFC.curr)=_tmpFE)))))));}}else{char _tmp101;
char _tmp100;struct _dyneither_ptr _tmpFF;(_tmpFF=_dyneither_ptr_inplace_plus_post(&
p,sizeof(char),1),((_tmp100=*((char*)_check_dyneither_subscript(_tmpFF,sizeof(
char),0)),((_tmp101='+',((_get_dyneither_size(_tmpFF,sizeof(char))== 1  && (
_tmp100 == '\000'  && _tmp101 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpFF.curr)=
_tmp101)))))));}t=_dynforward_to_dyneither(_dynforward_ptr_plus(expbuf,sizeof(
char),308));if(exp > 9){do{char _tmp104;char _tmp103;struct _dyneither_ptr _tmp102;(
_tmp102=_dyneither_ptr_inplace_plus(& t,sizeof(char),-1),((_tmp103=*((char*)
_check_dyneither_subscript(_tmp102,sizeof(char),0)),((_tmp104=(char)(exp % 10 + '0'),((
_get_dyneither_size(_tmp102,sizeof(char))== 1  && (_tmp103 == '\000'  && _tmp104 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp102.curr)=_tmp104)))))));}while((exp /=10)> 9);{
char _tmp107;char _tmp106;struct _dyneither_ptr _tmp105;(_tmp105=
_dyneither_ptr_inplace_plus(& t,sizeof(char),-1),((_tmp106=*((char*)
_check_dyneither_subscript(_tmp105,sizeof(char),0)),((_tmp107=(char)(exp + '0'),((
_get_dyneither_size(_tmp105,sizeof(char))== 1  && (_tmp106 == '\000'  && _tmp107 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp105.curr)=_tmp107)))))));}{char _tmp10A;char
_tmp109;struct _dyneither_ptr _tmp108;for(0;t.curr < (_dynforward_to_dyneither(
_dynforward_ptr_plus(expbuf,sizeof(char),308))).curr;(_tmp108=
_dyneither_ptr_inplace_plus_post(& p,sizeof(char),1),((_tmp109=*((char*)
_check_dyneither_subscript(_tmp108,sizeof(char),0)),((_tmp10A=*((char*)
_check_dyneither_subscript(_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),
sizeof(char),0)),((_get_dyneither_size(_tmp108,sizeof(char))== 1  && (_tmp109 == '\000'
 && _tmp10A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp108.curr)=_tmp10A)))))))){;}}}
else{{char _tmp10D;char _tmp10C;struct _dyneither_ptr _tmp10B;(_tmp10B=
_dyneither_ptr_inplace_plus_post(& p,sizeof(char),1),((_tmp10C=*((char*)
_check_dyneither_subscript(_tmp10B,sizeof(char),0)),((_tmp10D='0',((
_get_dyneither_size(_tmp10B,sizeof(char))== 1  && (_tmp10C == '\000'  && _tmp10D != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp10B.curr)=_tmp10D)))))));}{char _tmp110;char
_tmp10F;struct _dyneither_ptr _tmp10E;(_tmp10E=_dyneither_ptr_inplace_plus_post(& p,
sizeof(char),1),((_tmp10F=*((char*)_check_dyneither_subscript(_tmp10E,sizeof(
char),0)),((_tmp110=(char)(exp + '0'),((_get_dyneither_size(_tmp10E,sizeof(char))
== 1  && (_tmp10F == '\000'  && _tmp110 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp10E.curr)=_tmp110)))))));}}return p;}}static struct _dyneither_ptr Cyc_sround(
double fract,int*exp,struct _dyneither_ptr start,struct _dyneither_ptr end,char ch,int*
signp);static struct _dyneither_ptr Cyc_sround(double fract,int*exp,struct
_dyneither_ptr start,struct _dyneither_ptr end,char ch,int*signp){double tmp=(double)
0.0;if(fract != 0.0)modf(fract * 10,& tmp);else{tmp=(double)(ch - '0');}if(tmp > 4)
for(0;1;_dyneither_ptr_inplace_plus(& end,sizeof(char),-1)){if(*((char*)
_check_dyneither_subscript(end,sizeof(char),0))== '.')
_dyneither_ptr_inplace_plus(& end,sizeof(char),-1);{char _tmp113;char _tmp112;
struct _dyneither_ptr _tmp111;if(((_tmp111=end,((_tmp112=*((char*)
_check_dyneither_subscript(_tmp111,sizeof(char),0)),((_tmp113=_tmp112 + '\001',((
_get_dyneither_size(_tmp111,sizeof(char))== 1  && (_tmp112 == '\000'  && _tmp113 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp111.curr)=_tmp113))))))))<= '9')break;}{char
_tmp116;char _tmp115;struct _dyneither_ptr _tmp114;(_tmp114=end,((_tmp115=*((char*)
_check_dyneither_subscript(_tmp114,sizeof(char),0)),((_tmp116='0',((
_get_dyneither_size(_tmp114,sizeof(char))== 1  && (_tmp115 == '\000'  && _tmp116 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp114.curr)=_tmp116)))))));}if(end.curr == 
start.curr){if((unsigned int)exp){{char _tmp119;char _tmp118;struct _dyneither_ptr
_tmp117;(_tmp117=end,((_tmp118=*((char*)_check_dyneither_subscript(_tmp117,
sizeof(char),0)),((_tmp119='1',((_get_dyneither_size(_tmp117,sizeof(char))== 1
 && (_tmp118 == '\000'  && _tmp119 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp117.curr)=_tmp119)))))));}++(*exp);}else{{char _tmp11C;char _tmp11B;struct
_dyneither_ptr _tmp11A;(_tmp11A=_dyneither_ptr_inplace_plus(& end,sizeof(char),-1),((
_tmp11B=*((char*)_check_dyneither_subscript(_tmp11A,sizeof(char),0)),((_tmp11C='1',((
_get_dyneither_size(_tmp11A,sizeof(char))== 1  && (_tmp11B == '\000'  && _tmp11C != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp11A.curr)=_tmp11C)))))));}
_dyneither_ptr_inplace_plus(& start,sizeof(char),-1);}break;}}else{if(*signp == '-')
for(0;1;_dyneither_ptr_inplace_plus(& end,sizeof(char),-1)){if(*((char*)
_check_dyneither_subscript(end,sizeof(char),0))== '.')
_dyneither_ptr_inplace_plus(& end,sizeof(char),-1);if(*((char*)
_check_dyneither_subscript(end,sizeof(char),0))!= '0')break;if(end.curr == start.curr)*
signp=0;}}return start;}int Cyc___cvt_double(double number,int prec,int flags,int*
signp,int fmtch,struct _dyneither_ptr startp,struct _dyneither_ptr endp);int Cyc___cvt_double(
double number,int prec,int flags,int*signp,int fmtch,struct _dyneither_ptr startp,
struct _dyneither_ptr endp){struct _dyneither_ptr p;struct _dyneither_ptr t;register
double fract;int dotrim=0;int expcnt;int gformat=0;double integer=(double)0.0;double
tmp=(double)0.0;expcnt=0;if(number < 0){number=- number;*signp=(int)'-';}else{*
signp=0;}fract=modf(number,& integer);t=_dyneither_ptr_inplace_plus(& startp,
sizeof(char),1);for(p=_dyneither_ptr_plus(endp,sizeof(char),- 1);p.curr >= startp.curr
 && integer != 0.0;++ expcnt){tmp=modf(integer / 10,& integer);{char _tmp11F;char
_tmp11E;struct _dyneither_ptr _tmp11D;(_tmp11D=_dyneither_ptr_inplace_plus_post(& p,
sizeof(char),-1),((_tmp11E=*((char*)_check_dyneither_subscript(_tmp11D,sizeof(
char),0)),((_tmp11F=(char)((int)((tmp + .01)* 10)+ '0'),((_get_dyneither_size(
_tmp11D,sizeof(char))== 1  && (_tmp11E == '\000'  && _tmp11F != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp11D.curr)=_tmp11F)))))));}}switch(fmtch){
case 'f': _LL63: goto _LL64;case 'F': _LL64: if(expcnt){char _tmp122;char _tmp121;struct
_dyneither_ptr _tmp120;for(0;(_dyneither_ptr_inplace_plus(& p,sizeof(char),1)).curr
< endp.curr;(_tmp120=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((
_tmp121=*((char*)_check_dyneither_subscript(_tmp120,sizeof(char),0)),((_tmp122=*((
char*)_check_dyneither_subscript(p,sizeof(char),0)),((_get_dyneither_size(
_tmp120,sizeof(char))== 1  && (_tmp121 == '\000'  && _tmp122 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp120.curr)=_tmp122)))))))){;}}else{char
_tmp125;char _tmp124;struct _dyneither_ptr _tmp123;(_tmp123=
_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp124=*((char*)
_check_dyneither_subscript(_tmp123,sizeof(char),0)),((_tmp125='0',((
_get_dyneither_size(_tmp123,sizeof(char))== 1  && (_tmp124 == '\000'  && _tmp125 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp123.curr)=_tmp125)))))));}if(prec  || flags & 
8){char _tmp128;char _tmp127;struct _dyneither_ptr _tmp126;(_tmp126=
_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp127=*((char*)
_check_dyneither_subscript(_tmp126,sizeof(char),0)),((_tmp128='.',((
_get_dyneither_size(_tmp126,sizeof(char))== 1  && (_tmp127 == '\000'  && _tmp128 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp126.curr)=_tmp128)))))));}if(fract != 0.0){
if(prec)do{fract=modf(fract * 10,& tmp);{char _tmp12B;char _tmp12A;struct
_dyneither_ptr _tmp129;(_tmp129=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),
1),((_tmp12A=*((char*)_check_dyneither_subscript(_tmp129,sizeof(char),0)),((
_tmp12B=(char)((int)tmp + '0'),((_get_dyneither_size(_tmp129,sizeof(char))== 1
 && (_tmp12A == '\000'  && _tmp12B != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp129.curr)=_tmp12B)))))));}}while(-- prec  && fract != 0.0);if(fract != 0.0)
startp=Cyc_sround(fract,0,startp,_dyneither_ptr_plus(t,sizeof(char),- 1),(char)'\000',
signp);}{char _tmp12E;char _tmp12D;struct _dyneither_ptr _tmp12C;for(0;prec --;(
_tmp12C=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp12D=*((char*)
_check_dyneither_subscript(_tmp12C,sizeof(char),0)),((_tmp12E='0',((
_get_dyneither_size(_tmp12C,sizeof(char))== 1  && (_tmp12D == '\000'  && _tmp12E != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp12C.curr)=_tmp12E)))))))){;}}break;case 'e':
_LL65: goto _LL66;case 'E': _LL66: eformat: if(expcnt){{char _tmp131;char _tmp130;struct
_dyneither_ptr _tmp12F;(_tmp12F=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),
1),((_tmp130=*((char*)_check_dyneither_subscript(_tmp12F,sizeof(char),0)),((
_tmp131=*((char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus(& p,
sizeof(char),1),sizeof(char),0)),((_get_dyneither_size(_tmp12F,sizeof(char))== 1
 && (_tmp130 == '\000'  && _tmp131 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp12F.curr)=_tmp131)))))));}if(prec  || flags & 8){char _tmp134;char _tmp133;
struct _dyneither_ptr _tmp132;(_tmp132=_dyneither_ptr_inplace_plus_post(& t,sizeof(
char),1),((_tmp133=*((char*)_check_dyneither_subscript(_tmp132,sizeof(char),0)),((
_tmp134='.',((_get_dyneither_size(_tmp132,sizeof(char))== 1  && (_tmp133 == '\000'
 && _tmp134 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp132.curr)=_tmp134)))))));}
for(0;prec  && (_dyneither_ptr_inplace_plus(& p,sizeof(char),1)).curr < endp.curr;
-- prec){char _tmp137;char _tmp136;struct _dyneither_ptr _tmp135;(_tmp135=
_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp136=*((char*)
_check_dyneither_subscript(_tmp135,sizeof(char),0)),((_tmp137=*((char*)
_check_dyneither_subscript(p,sizeof(char),0)),((_get_dyneither_size(_tmp135,
sizeof(char))== 1  && (_tmp136 == '\000'  && _tmp137 != '\000')?_throw_arraybounds():
1,*((char*)_tmp135.curr)=_tmp137)))))));}if(!prec  && (
_dyneither_ptr_inplace_plus(& p,sizeof(char),1)).curr < endp.curr){fract=(double)0;
startp=Cyc_sround((double)0,(int*)& expcnt,startp,_dyneither_ptr_plus(t,sizeof(
char),- 1),*((char*)_check_dyneither_subscript(p,sizeof(char),0)),signp);}--
expcnt;}else{if(fract != 0.0){for(expcnt=- 1;1;-- expcnt){fract=modf(fract * 10,& tmp);
if(tmp != 0.0)break;}{char _tmp13A;char _tmp139;struct _dyneither_ptr _tmp138;(
_tmp138=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp139=*((char*)
_check_dyneither_subscript(_tmp138,sizeof(char),0)),((_tmp13A=(char)((int)tmp + '0'),((
_get_dyneither_size(_tmp138,sizeof(char))== 1  && (_tmp139 == '\000'  && _tmp13A != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp138.curr)=_tmp13A)))))));}if(prec  || flags & 
8){char _tmp13D;char _tmp13C;struct _dyneither_ptr _tmp13B;(_tmp13B=
_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp13C=*((char*)
_check_dyneither_subscript(_tmp13B,sizeof(char),0)),((_tmp13D='.',((
_get_dyneither_size(_tmp13B,sizeof(char))== 1  && (_tmp13C == '\000'  && _tmp13D != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp13B.curr)=_tmp13D)))))));}}else{{char _tmp140;
char _tmp13F;struct _dyneither_ptr _tmp13E;(_tmp13E=
_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp13F=*((char*)
_check_dyneither_subscript(_tmp13E,sizeof(char),0)),((_tmp140='0',((
_get_dyneither_size(_tmp13E,sizeof(char))== 1  && (_tmp13F == '\000'  && _tmp140 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp13E.curr)=_tmp140)))))));}if(prec  || flags & 
8){char _tmp143;char _tmp142;struct _dyneither_ptr _tmp141;(_tmp141=
_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp142=*((char*)
_check_dyneither_subscript(_tmp141,sizeof(char),0)),((_tmp143='.',((
_get_dyneither_size(_tmp141,sizeof(char))== 1  && (_tmp142 == '\000'  && _tmp143 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp141.curr)=_tmp143)))))));}}}if(fract != 0.0){
if(prec)do{fract=modf(fract * 10,& tmp);{char _tmp146;char _tmp145;struct
_dyneither_ptr _tmp144;(_tmp144=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),
1),((_tmp145=*((char*)_check_dyneither_subscript(_tmp144,sizeof(char),0)),((
_tmp146=(char)((int)tmp + '0'),((_get_dyneither_size(_tmp144,sizeof(char))== 1
 && (_tmp145 == '\000'  && _tmp146 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp144.curr)=_tmp146)))))));}}while(-- prec  && fract != 0.0);if(fract != 0.0)
startp=Cyc_sround(fract,(int*)& expcnt,startp,_dyneither_ptr_plus(t,sizeof(char),
- 1),(char)'\000',signp);}{char _tmp149;char _tmp148;struct _dyneither_ptr _tmp147;
for(0;prec --;(_tmp147=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((
_tmp148=*((char*)_check_dyneither_subscript(_tmp147,sizeof(char),0)),((_tmp149='0',((
_get_dyneither_size(_tmp147,sizeof(char))== 1  && (_tmp148 == '\000'  && _tmp149 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp147.curr)=_tmp149)))))))){;}}if(gformat  && !(
flags & 8)){while(t.curr > startp.curr  && *((char*)_check_dyneither_subscript(
_dyneither_ptr_inplace_plus(& t,sizeof(char),-1),sizeof(char),0))== '0'){;}if(*((
char*)_check_dyneither_subscript(t,sizeof(char),0))== '.')
_dyneither_ptr_inplace_plus(& t,sizeof(char),-1);_dyneither_ptr_inplace_plus(& t,
sizeof(char),1);}t=Cyc_exponent(t,expcnt,fmtch);break;case 'g': _LL67: goto _LL68;
case 'G': _LL68: if(!prec)++ prec;if(expcnt > prec  || (!expcnt  && fract != 0.0) && 
fract < .0001){-- prec;fmtch -=2;gformat=1;goto eformat;}if(expcnt){char _tmp14C;char
_tmp14B;struct _dyneither_ptr _tmp14A;for(0;(_dyneither_ptr_inplace_plus(& p,
sizeof(char),1)).curr < endp.curr;(((_tmp14A=_dyneither_ptr_inplace_plus_post(& t,
sizeof(char),1),((_tmp14B=*((char*)_check_dyneither_subscript(_tmp14A,sizeof(
char),0)),((_tmp14C=*((char*)_check_dyneither_subscript(p,sizeof(char),0)),((
_get_dyneither_size(_tmp14A,sizeof(char))== 1  && (_tmp14B == '\000'  && _tmp14C != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp14A.curr)=_tmp14C)))))))),-- prec)){;}}else{
char _tmp14F;char _tmp14E;struct _dyneither_ptr _tmp14D;(_tmp14D=
_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp14E=*((char*)
_check_dyneither_subscript(_tmp14D,sizeof(char),0)),((_tmp14F='0',((
_get_dyneither_size(_tmp14D,sizeof(char))== 1  && (_tmp14E == '\000'  && _tmp14F != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp14D.curr)=_tmp14F)))))));}if(prec  || flags & 
8){dotrim=1;{char _tmp152;char _tmp151;struct _dyneither_ptr _tmp150;(_tmp150=
_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp151=*((char*)
_check_dyneither_subscript(_tmp150,sizeof(char),0)),((_tmp152='.',((
_get_dyneither_size(_tmp150,sizeof(char))== 1  && (_tmp151 == '\000'  && _tmp152 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp150.curr)=_tmp152)))))));}}else{dotrim=0;}
if(fract != 0.0){if(prec){do{fract=modf(fract * 10,& tmp);{char _tmp155;char _tmp154;
struct _dyneither_ptr _tmp153;(_tmp153=_dyneither_ptr_inplace_plus_post(& t,sizeof(
char),1),((_tmp154=*((char*)_check_dyneither_subscript(_tmp153,sizeof(char),0)),((
_tmp155=(char)((int)tmp + '0'),((_get_dyneither_size(_tmp153,sizeof(char))== 1
 && (_tmp154 == '\000'  && _tmp155 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp153.curr)=_tmp155)))))));}}while(tmp == 0.0  && !expcnt);while(-- prec  && fract
!= 0.0){fract=modf(fract * 10,& tmp);{char _tmp158;char _tmp157;struct _dyneither_ptr
_tmp156;(_tmp156=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp157=*((
char*)_check_dyneither_subscript(_tmp156,sizeof(char),0)),((_tmp158=(char)((int)
tmp + '0'),((_get_dyneither_size(_tmp156,sizeof(char))== 1  && (_tmp157 == '\000'
 && _tmp158 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp156.curr)=_tmp158)))))));}}}
if(fract != 0.0)startp=Cyc_sround(fract,0,startp,_dyneither_ptr_plus(t,sizeof(
char),- 1),(char)'\000',signp);}if(flags & 8){char _tmp15B;char _tmp15A;struct
_dyneither_ptr _tmp159;for(0;prec --;(_tmp159=_dyneither_ptr_inplace_plus_post(& t,
sizeof(char),1),((_tmp15A=*((char*)_check_dyneither_subscript(_tmp159,sizeof(
char),0)),((_tmp15B='0',((_get_dyneither_size(_tmp159,sizeof(char))== 1  && (
_tmp15A == '\000'  && _tmp15B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp159.curr)=
_tmp15B)))))))){;}}else{if(dotrim){while(t.curr > startp.curr  && *((char*)
_check_dyneither_subscript(_dyneither_ptr_inplace_plus(& t,sizeof(char),-1),
sizeof(char),0))== '0'){;}if(*((char*)_check_dyneither_subscript(t,sizeof(char),
0))!= '.')_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}}break;default: _LL69: {
struct Cyc_Core_Impossible_struct _tmp161;const char*_tmp160;struct Cyc_Core_Impossible_struct*
_tmp15F;(int)_throw((void*)((_tmp15F=_cycalloc(sizeof(*_tmp15F)),((_tmp15F[0]=((
_tmp161.tag=Cyc_Core_Impossible,((_tmp161.f1=((_tmp160="__cvt_double",
_tag_dynforward(_tmp160,sizeof(char),_get_zero_arr_size_char(_tmp160,13)))),
_tmp161)))),_tmp15F)))));}}return(t.curr - startp.curr)/ sizeof(char);}int Cyc_vfprintf(
struct Cyc___cycFILE*f,struct _dynforward_ptr fmt,struct _dynforward_ptr ap);int Cyc_vfprintf(
struct Cyc___cycFILE*f,struct _dynforward_ptr fmt,struct _dynforward_ptr ap){int ans;
ans=((int(*)(int(*ioputc)(int,struct Cyc___cycFILE*),struct Cyc___cycFILE*
ioputc_env,struct _dynforward_ptr fmt0,struct _dynforward_ptr ap))Cyc__IO_vfprintf)(
Cyc_putc,f,fmt,ap);return ans;}int Cyc_fprintf(struct Cyc___cycFILE*f,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap);int Cyc_fprintf(struct Cyc___cycFILE*f,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap){return Cyc_vfprintf(f,fmt,ap);}
int Cyc_vprintf(struct _dynforward_ptr fmt,struct _dynforward_ptr ap);int Cyc_vprintf(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap){int ans;ans=((int(*)(int(*
ioputc)(int,struct Cyc___cycFILE*),struct Cyc___cycFILE*ioputc_env,struct
_dynforward_ptr fmt0,struct _dynforward_ptr ap))Cyc__IO_vfprintf)(Cyc_putc,Cyc_stdout,
fmt,ap);return ans;}int Cyc_printf(struct _dynforward_ptr fmt,struct _dynforward_ptr
ap);int Cyc_printf(struct _dynforward_ptr fmt,struct _dynforward_ptr ap){int ans;ans=
Cyc_vprintf(fmt,ap);return ans;}struct _tuple0{struct _dynforward_ptr*f1;
unsigned long*f2;};static int Cyc_putc_string(int c,struct _tuple0*sptr_n);static int
Cyc_putc_string(int c,struct _tuple0*sptr_n){struct _tuple0 _tmpB3;struct
_dynforward_ptr*_tmpB4;unsigned long*_tmpB5;struct _tuple0*_tmpB2=sptr_n;_tmpB3=*
_tmpB2;_tmpB4=_tmpB3.f1;_tmpB5=_tmpB3.f2;{struct _dynforward_ptr s=*_tmpB4;
unsigned long n=*_tmpB5;if(_get_dynforward_size(s,sizeof(char))== 0  || n == 0)
return - 1;*((char*)_check_dynforward_subscript(s,sizeof(char),0))=(char)c;*_tmpB4=
_dynforward_ptr_plus(s,sizeof(char),1);*_tmpB5=n - 1;return 1;}}int Cyc_vsnprintf(
struct _dynforward_ptr s,unsigned long n,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);int Cyc_vsnprintf(struct _dynforward_ptr s,unsigned long n,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap){int ans;struct _dynforward_ptr _tmpB6=s;
unsigned long _tmpB7=n;struct _tuple0 _tmp162;struct _tuple0 _tmpB8=(_tmp162.f1=&
_tmpB6,((_tmp162.f2=& _tmpB7,_tmp162)));ans=((int(*)(int(*ioputc)(int,struct
_tuple0*),struct _tuple0*ioputc_env,struct _dynforward_ptr fmt0,struct
_dynforward_ptr ap))Cyc__IO_vfprintf)(Cyc_putc_string,& _tmpB8,fmt,ap);if(0 <= ans)*((
char*)_check_dynforward_subscript(s,sizeof(char),ans))='\000';return ans;}int Cyc_snprintf(
struct _dynforward_ptr s,unsigned long n,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);int Cyc_snprintf(struct _dynforward_ptr s,unsigned long n,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap){return Cyc_vsnprintf(s,n,fmt,ap);}int
Cyc_vsprintf(struct _dynforward_ptr s,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);int Cyc_vsprintf(struct _dynforward_ptr s,struct _dynforward_ptr
fmt,struct _dynforward_ptr ap){return Cyc_vsnprintf(s,_get_dynforward_size(s,
sizeof(char)),fmt,ap);}int Cyc_sprintf(struct _dynforward_ptr s,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap);int Cyc_sprintf(struct _dynforward_ptr
s,struct _dynforward_ptr fmt,struct _dynforward_ptr ap){return Cyc_vsnprintf(s,
_get_dynforward_size(s,sizeof(char)),fmt,ap);}static int Cyc_putc_void(int c,int
dummy);static int Cyc_putc_void(int c,int dummy){return 1;}struct _dynforward_ptr Cyc_vrprintf(
struct _RegionHandle*r1,struct _dynforward_ptr fmt,struct _dynforward_ptr ap);static
void _tmp168(unsigned int*_tmp167,unsigned int*_tmp166,char**_tmp164){for(*
_tmp167=0;*_tmp167 < *_tmp166;(*_tmp167)++){(*_tmp164)[*_tmp167]='\000';}}struct
_dynforward_ptr Cyc_vrprintf(struct _RegionHandle*r1,struct _dynforward_ptr fmt,
struct _dynforward_ptr ap){int size=((int(*)(int(*ioputc)(int,int),int ioputc_env,
struct _dynforward_ptr fmt0,struct _dynforward_ptr ap))Cyc__IO_vfprintf)(Cyc_putc_void,
0,fmt,ap)+ 1;unsigned int _tmp167;unsigned int _tmp166;struct _dynforward_ptr
_tmp165;char*_tmp164;unsigned int _tmp163;struct _dynforward_ptr s=(_tmp163=(
unsigned int)size,((_tmp164=(char*)_region_malloc(r1,_check_times(sizeof(char),
_tmp163 + 1)),((_tmp165=_tag_dynforward(_tmp164,sizeof(char),_tmp163 + 1),((((
_tmp166=_tmp163,((_tmp168(& _tmp167,& _tmp166,& _tmp164),_tmp164[_tmp166]=(char)0)))),
_tmp165)))))));Cyc_vsprintf(_dynforward_ptr_decrease_size(s,sizeof(char),1),fmt,
ap);return s;}struct _dynforward_ptr Cyc_rprintf(struct _RegionHandle*r1,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap);struct _dynforward_ptr Cyc_rprintf(
struct _RegionHandle*r1,struct _dynforward_ptr fmt,struct _dynforward_ptr ap){return
Cyc_vrprintf(r1,fmt,ap);}struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap);struct _dynforward_ptr Cyc_aprintf(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap){return Cyc_vrprintf(Cyc_Core_heap_region,
fmt,ap);}
