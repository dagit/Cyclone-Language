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
 struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};struct Cyc_Core_Opt{
void*v;};struct _dynforward_ptr Cyc_Core_new_string(unsigned int);struct
_dynforward_ptr Cyc_Core_rnew_string(struct _RegionHandle*,unsigned int);extern
char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{char*
tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;extern char Cyc_Core_Open_Region[
16];extern char Cyc_Core_Free_Region[16];struct _dynforward_ptr
wrap_Cbuffer_as_buffer(char*,unsigned long);struct Cyc_List_List{void*hd;struct
Cyc_List_List*tl;};int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];extern char Cyc_List_Nth[8];int toupper(int);char*strerror(int errnum);
unsigned long Cyc_strlen(struct _dynforward_ptr s);int Cyc_strcmp(struct
_dynforward_ptr s1,struct _dynforward_ptr s2);int Cyc_strptrcmp(struct
_dynforward_ptr*s1,struct _dynforward_ptr*s2);int Cyc_strncmp(struct
_dynforward_ptr s1,struct _dynforward_ptr s2,unsigned long len);int Cyc_zstrcmp(
struct _dynforward_ptr,struct _dynforward_ptr);int Cyc_zstrncmp(struct
_dynforward_ptr s1,struct _dynforward_ptr s2,unsigned long n);int Cyc_zstrptrcmp(
struct _dynforward_ptr*,struct _dynforward_ptr*);int Cyc_strcasecmp(struct
_dynforward_ptr,struct _dynforward_ptr);int Cyc_strncasecmp(struct _dynforward_ptr
s1,struct _dynforward_ptr s2,unsigned long len);struct _dynforward_ptr Cyc_strcat(
struct _dynforward_ptr dest,struct _dynforward_ptr src);struct _dynforward_ptr Cyc_strconcat(
struct _dynforward_ptr,struct _dynforward_ptr);struct _dynforward_ptr Cyc_rstrconcat(
struct _RegionHandle*,struct _dynforward_ptr,struct _dynforward_ptr);struct
_dynforward_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct _dynforward_ptr Cyc_rstrconcat_l(
struct _RegionHandle*,struct Cyc_List_List*);struct _dynforward_ptr Cyc_str_sepstr(
struct Cyc_List_List*,struct _dynforward_ptr);struct _dynforward_ptr Cyc_rstr_sepstr(
struct _RegionHandle*,struct Cyc_List_List*,struct _dynforward_ptr);struct
_dynforward_ptr Cyc_strcpy(struct _dynforward_ptr dest,struct _dynforward_ptr src);
struct _dynforward_ptr Cyc_strncpy(struct _dynforward_ptr,struct _dynforward_ptr,
unsigned long);struct _dynforward_ptr Cyc_zstrncpy(struct _dynforward_ptr,struct
_dynforward_ptr,unsigned long);struct _dynforward_ptr Cyc_realloc(struct
_dynforward_ptr,unsigned long);struct _dynforward_ptr Cyc__memcpy(struct
_dynforward_ptr d,struct _dynforward_ptr s,unsigned long,unsigned int);struct
_dynforward_ptr Cyc__memmove(struct _dynforward_ptr d,struct _dynforward_ptr s,
unsigned long,unsigned int);int Cyc_memcmp(struct _dynforward_ptr s1,struct
_dynforward_ptr s2,unsigned long n);struct _dynforward_ptr Cyc_memchr(struct
_dynforward_ptr s,char c,unsigned long n);struct _dynforward_ptr Cyc_mmemchr(struct
_dynforward_ptr s,char c,unsigned long n);struct _dynforward_ptr Cyc_memset(struct
_dynforward_ptr s,char c,unsigned long n);void Cyc_bzero(struct _dynforward_ptr s,
unsigned long n);void Cyc__bcopy(struct _dynforward_ptr src,struct _dynforward_ptr dst,
unsigned long n,unsigned int sz);struct _dynforward_ptr Cyc_expand(struct
_dynforward_ptr s,unsigned long sz);struct _dynforward_ptr Cyc_rexpand(struct
_RegionHandle*,struct _dynforward_ptr s,unsigned long sz);struct _dynforward_ptr Cyc_realloc_str(
struct _dynforward_ptr str,unsigned long sz);struct _dynforward_ptr Cyc_rrealloc_str(
struct _RegionHandle*r,struct _dynforward_ptr str,unsigned long sz);struct
_dynforward_ptr Cyc_strdup(struct _dynforward_ptr src);struct _dynforward_ptr Cyc_rstrdup(
struct _RegionHandle*,struct _dynforward_ptr src);struct _dynforward_ptr Cyc_substring(
struct _dynforward_ptr,int ofs,unsigned long n);struct _dynforward_ptr Cyc_rsubstring(
struct _RegionHandle*,struct _dynforward_ptr,int ofs,unsigned long n);struct
_dynforward_ptr Cyc_replace_suffix(struct _dynforward_ptr,struct _dynforward_ptr,
struct _dynforward_ptr);struct _dynforward_ptr Cyc_rreplace_suffix(struct
_RegionHandle*r,struct _dynforward_ptr src,struct _dynforward_ptr curr_suffix,struct
_dynforward_ptr new_suffix);struct _dynforward_ptr Cyc_strchr(struct _dynforward_ptr
s,char c);struct _dynforward_ptr Cyc_mstrchr(struct _dynforward_ptr s,char c);struct
_dynforward_ptr Cyc_mstrrchr(struct _dynforward_ptr s,char c);struct _dynforward_ptr
Cyc_strrchr(struct _dynforward_ptr s,char c);struct _dynforward_ptr Cyc_mstrstr(
struct _dynforward_ptr haystack,struct _dynforward_ptr needle);struct _dynforward_ptr
Cyc_strstr(struct _dynforward_ptr haystack,struct _dynforward_ptr needle);struct
_dynforward_ptr Cyc_strpbrk(struct _dynforward_ptr s,struct _dynforward_ptr accept);
struct _dynforward_ptr Cyc_mstrpbrk(struct _dynforward_ptr s,struct _dynforward_ptr
accept);unsigned long Cyc_strspn(struct _dynforward_ptr s,struct _dynforward_ptr
accept);unsigned long Cyc_strcspn(struct _dynforward_ptr s,struct _dynforward_ptr
accept);struct _dynforward_ptr Cyc_strtok(struct _dynforward_ptr s,struct
_dynforward_ptr delim);struct Cyc_List_List*Cyc_explode(struct _dynforward_ptr s);
struct Cyc_List_List*Cyc_rexplode(struct _RegionHandle*,struct _dynforward_ptr s);
struct _dynforward_ptr Cyc_implode(struct Cyc_List_List*c);int Cyc_strcasecmp(struct
_dynforward_ptr,struct _dynforward_ptr);int Cyc_strncasecmp(struct _dynforward_ptr
s1,struct _dynforward_ptr s2,unsigned long len);void*Cyc___assert_fail(struct
_dynforward_ptr assertion,struct _dynforward_ptr file,unsigned int line);char*
strerror(int errnum);unsigned long Cyc_strlen(struct _dynforward_ptr s);
unsigned long Cyc_strlen(struct _dynforward_ptr s){unsigned long i;unsigned int sz=
_get_dynforward_size(s,sizeof(char));for(i=0;i < sz;++ i){if(*((const char*)
_check_dynforward_subscript(s,sizeof(char),(int)i))== '\000')return i;}return i;}
int Cyc_strcmp(struct _dynforward_ptr s1,struct _dynforward_ptr s2);int Cyc_strcmp(
struct _dynforward_ptr s1,struct _dynforward_ptr s2){if(s1.curr == s2.curr)return 0;{
int i=0;unsigned int sz1=_get_dynforward_size(s1,sizeof(char));unsigned int sz2=
_get_dynforward_size(s2,sizeof(char));unsigned int minsz=sz1 < sz2?sz1: sz2;{const
char*_tmp7F;const char*_tmp7E;minsz <= _get_dynforward_size(s1,sizeof(char)) && 
minsz <= _get_dynforward_size(s2,sizeof(char))?0:((int(*)(struct _dynforward_ptr
assertion,struct _dynforward_ptr file,unsigned int line))Cyc___assert_fail)(((
_tmp7E="minsz <= s1.size && minsz <= s2.size",_tag_dynforward(_tmp7E,sizeof(char),
_get_zero_arr_size_char(_tmp7E,37)))),((_tmp7F="string.cyc",_tag_dynforward(
_tmp7F,sizeof(char),_get_zero_arr_size_char(_tmp7F,11)))),60);}while(i < minsz){
char c1=*((const char*)_check_dynforward_subscript(s1,sizeof(char),i));char c2=*((
const char*)_check_dynforward_subscript(s2,sizeof(char),i));if(c1 == '\000'){if(c2
== '\000')return 0;else{return - 1;}}else{if(c2 == '\000')return 1;else{int diff=c1 - 
c2;if(diff != 0)return diff;}}++ i;}if(sz1 == sz2)return 0;if(minsz < sz2){if(*((const
char*)_check_dynforward_subscript(s2,sizeof(char),i))== '\000')return 0;else{
return - 1;}}else{if(*((const char*)_check_dynforward_subscript(s1,sizeof(char),i))
== '\000')return 0;else{return 1;}}}}int Cyc_strptrcmp(struct _dynforward_ptr*s1,
struct _dynforward_ptr*s2);int Cyc_strptrcmp(struct _dynforward_ptr*s1,struct
_dynforward_ptr*s2){return Cyc_strcmp((struct _dynforward_ptr)*s1,(struct
_dynforward_ptr)*s2);}static int Cyc_ncmp(struct _dynforward_ptr s1,unsigned long
len1,struct _dynforward_ptr s2,unsigned long len2,unsigned long n);inline static int
Cyc_ncmp(struct _dynforward_ptr s1,unsigned long len1,struct _dynforward_ptr s2,
unsigned long len2,unsigned long n){if(n <= 0)return 0;{unsigned long min_len=len1 > 
len2?len2: len1;unsigned long bound=min_len > n?n: min_len;{const char*_tmp81;const
char*_tmp80;bound <= _get_dynforward_size(s1,sizeof(char)) && bound <= 
_get_dynforward_size(s2,sizeof(char))?0:((int(*)(struct _dynforward_ptr assertion,
struct _dynforward_ptr file,unsigned int line))Cyc___assert_fail)(((_tmp80="bound <= s1.size && bound <= s2.size",
_tag_dynforward(_tmp80,sizeof(char),_get_zero_arr_size_char(_tmp80,37)))),((
_tmp81="string.cyc",_tag_dynforward(_tmp81,sizeof(char),_get_zero_arr_size_char(
_tmp81,11)))),96);}{int i=0;for(0;i < bound;++ i){int retc;if((retc=*((const char*)
_check_dynforward_subscript(s1,sizeof(char),i))- *((const char*)
_check_dynforward_subscript(s2,sizeof(char),i)))!= 0)return retc;}}if(len1 < n  || 
len2 < n)return(int)len1 - (int)len2;return 0;}}int Cyc_strncmp(struct
_dynforward_ptr s1,struct _dynforward_ptr s2,unsigned long n);int Cyc_strncmp(struct
_dynforward_ptr s1,struct _dynforward_ptr s2,unsigned long n){unsigned long len1=Cyc_strlen(
s1);unsigned long len2=Cyc_strlen(s2);return Cyc_ncmp(s1,len1,s2,len2,n);}int Cyc_zstrcmp(
struct _dynforward_ptr a,struct _dynforward_ptr b);int Cyc_zstrcmp(struct
_dynforward_ptr a,struct _dynforward_ptr b){if(a.curr == b.curr)return 0;{
unsigned long as=_get_dynforward_size(a,sizeof(char));unsigned long bs=
_get_dynforward_size(b,sizeof(char));unsigned long min_length=as < bs?as: bs;int i=-
1;{const char*_tmp83;const char*_tmp82;min_length <= _get_dynforward_size(a,sizeof(
char)) && min_length <= _get_dynforward_size(b,sizeof(char))?0:((int(*)(struct
_dynforward_ptr assertion,struct _dynforward_ptr file,unsigned int line))Cyc___assert_fail)(((
_tmp82="min_length <= a.size && min_length <= b.size",_tag_dynforward(_tmp82,
sizeof(char),_get_zero_arr_size_char(_tmp82,45)))),((_tmp83="string.cyc",
_tag_dynforward(_tmp83,sizeof(char),_get_zero_arr_size_char(_tmp83,11)))),128);}
while((++ i,i < min_length)){int diff=(int)*((const char*)
_check_dynforward_subscript(a,sizeof(char),i))- (int)*((const char*)
_check_dynforward_subscript(b,sizeof(char),i));if(diff != 0)return diff;}return(
int)as - (int)bs;}}int Cyc_zstrncmp(struct _dynforward_ptr s1,struct _dynforward_ptr
s2,unsigned long n);int Cyc_zstrncmp(struct _dynforward_ptr s1,struct _dynforward_ptr
s2,unsigned long n){if(n <= 0)return 0;{unsigned long s1size=_get_dynforward_size(s1,
sizeof(char));unsigned long s2size=_get_dynforward_size(s2,sizeof(char));
unsigned long min_size=s1size > s2size?s2size: s1size;unsigned long bound=min_size > 
n?n: min_size;{const char*_tmp85;const char*_tmp84;bound <= _get_dynforward_size(s1,
sizeof(char)) && bound <= _get_dynforward_size(s2,sizeof(char))?0:((int(*)(struct
_dynforward_ptr assertion,struct _dynforward_ptr file,unsigned int line))Cyc___assert_fail)(((
_tmp84="bound <= s1.size && bound <= s2.size",_tag_dynforward(_tmp84,sizeof(char),
_get_zero_arr_size_char(_tmp84,37)))),((_tmp85="string.cyc",_tag_dynforward(
_tmp85,sizeof(char),_get_zero_arr_size_char(_tmp85,11)))),146);}{int i=0;for(0;i < 
bound;++ i){if(*((const char*)_check_dynforward_subscript(s1,sizeof(char),i))< *((
const char*)_check_dynforward_subscript(s2,sizeof(char),i)))return - 1;else{if(*((
const char*)_check_dynforward_subscript(s2,sizeof(char),i))< *((const char*)
_check_dynforward_subscript(s1,sizeof(char),i)))return 1;}}}if(min_size <= bound)
return 0;if(s1size < s2size)return - 1;else{return 1;}}}int Cyc_zstrptrcmp(struct
_dynforward_ptr*a,struct _dynforward_ptr*b);int Cyc_zstrptrcmp(struct
_dynforward_ptr*a,struct _dynforward_ptr*b){if((int)a == (int)b)return 0;return Cyc_zstrcmp((
struct _dynforward_ptr)*a,(struct _dynforward_ptr)*b);}static struct _dynforward_ptr
Cyc_int_strcato(struct _dynforward_ptr dest,struct _dynforward_ptr src);inline static
struct _dynforward_ptr Cyc_int_strcato(struct _dynforward_ptr dest,struct
_dynforward_ptr src){int i;unsigned long dsize;unsigned long slen;unsigned long dlen;
dsize=_get_dynforward_size(dest,sizeof(char));dlen=Cyc_strlen((struct
_dynforward_ptr)dest);slen=Cyc_strlen(src);if(slen + dlen <= dsize){{const char*
_tmp87;const char*_tmp86;slen <= _get_dynforward_size(src,sizeof(char))?0:((int(*)(
struct _dynforward_ptr assertion,struct _dynforward_ptr file,unsigned int line))Cyc___assert_fail)(((
_tmp86="slen <= src.size",_tag_dynforward(_tmp86,sizeof(char),
_get_zero_arr_size_char(_tmp86,17)))),((_tmp87="string.cyc",_tag_dynforward(
_tmp87,sizeof(char),_get_zero_arr_size_char(_tmp87,11)))),182);}for(i=0;i < slen;
++ i){char _tmp8A;char _tmp89;struct _dynforward_ptr _tmp88;(_tmp88=
_dynforward_ptr_plus(dest,sizeof(char),(int)(i + dlen)),((_tmp89=*((char*)
_check_dynforward_subscript(_tmp88,sizeof(char),0)),((_tmp8A=*((const char*)
_check_dynforward_subscript(src,sizeof(char),i)),((_get_dynforward_size(_tmp88,
sizeof(char))== 1  && (_tmp89 == '\000'  && _tmp8A != '\000')?_throw_arraybounds(): 1,*((
char*)_tmp88.curr)=_tmp8A)))))));}if(i != dsize){char _tmp8D;char _tmp8C;struct
_dynforward_ptr _tmp8B;(_tmp8B=_dynforward_ptr_plus(dest,sizeof(char),(int)(i + 
dlen)),((_tmp8C=*((char*)_check_dynforward_subscript(_tmp8B,sizeof(char),0)),((
_tmp8D='\000',((_get_dynforward_size(_tmp8B,sizeof(char))== 1  && (_tmp8C == '\000'
 && _tmp8D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp8B.curr)=_tmp8D)))))));}}
else{struct Cyc_Core_Invalid_argument_struct _tmp93;const char*_tmp92;struct Cyc_Core_Invalid_argument_struct*
_tmp91;(int)_throw((void*)((_tmp91=_cycalloc(sizeof(*_tmp91)),((_tmp91[0]=((
_tmp93.tag=Cyc_Core_Invalid_argument,((_tmp93.f1=((_tmp92="strcat",
_tag_dynforward(_tmp92,sizeof(char),_get_zero_arr_size_char(_tmp92,7)))),_tmp93)))),
_tmp91)))));}return dest;}struct _dynforward_ptr Cyc_strcat(struct _dynforward_ptr
dest,struct _dynforward_ptr src);struct _dynforward_ptr Cyc_strcat(struct
_dynforward_ptr dest,struct _dynforward_ptr src){return Cyc_int_strcato(dest,src);}
struct _dynforward_ptr Cyc_rstrconcat(struct _RegionHandle*r,struct _dynforward_ptr a,
struct _dynforward_ptr b);struct _dynforward_ptr Cyc_rstrconcat(struct _RegionHandle*
r,struct _dynforward_ptr a,struct _dynforward_ptr b){unsigned long _tmp13=Cyc_strlen(
a);unsigned long _tmp14=Cyc_strlen(b);struct _dynforward_ptr ans=Cyc_Core_rnew_string(
r,(_tmp13 + _tmp14)+ 1);int i;int j;{const char*_tmp95;const char*_tmp94;_tmp13 <= 
_get_dynforward_size(ans,sizeof(char)) && _tmp13 <= _get_dynforward_size(a,
sizeof(char))?0:((int(*)(struct _dynforward_ptr assertion,struct _dynforward_ptr
file,unsigned int line))Cyc___assert_fail)(((_tmp94="alen <= ans.size && alen <= a.size",
_tag_dynforward(_tmp94,sizeof(char),_get_zero_arr_size_char(_tmp94,35)))),((
_tmp95="string.cyc",_tag_dynforward(_tmp95,sizeof(char),_get_zero_arr_size_char(
_tmp95,11)))),206);}for(i=0;i < _tmp13;++ i){char _tmp98;char _tmp97;struct
_dynforward_ptr _tmp96;(_tmp96=_dynforward_ptr_plus(ans,sizeof(char),i),((_tmp97=*((
char*)_check_dynforward_subscript(_tmp96,sizeof(char),0)),((_tmp98=*((const char*)
_check_dynforward_subscript(a,sizeof(char),i)),((_get_dynforward_size(_tmp96,
sizeof(char))== 1  && (_tmp97 == '\000'  && _tmp98 != '\000')?_throw_arraybounds(): 1,*((
char*)_tmp96.curr)=_tmp98)))))));}{const char*_tmp9A;const char*_tmp99;_tmp14 <= 
_get_dynforward_size(b,sizeof(char))?0:((int(*)(struct _dynforward_ptr assertion,
struct _dynforward_ptr file,unsigned int line))Cyc___assert_fail)(((_tmp99="blen <= b.size",
_tag_dynforward(_tmp99,sizeof(char),_get_zero_arr_size_char(_tmp99,15)))),((
_tmp9A="string.cyc",_tag_dynforward(_tmp9A,sizeof(char),_get_zero_arr_size_char(
_tmp9A,11)))),208);}for(j=0;j < _tmp14;++ j){char _tmp9D;char _tmp9C;struct
_dynforward_ptr _tmp9B;(_tmp9B=_dynforward_ptr_plus(ans,sizeof(char),i + j),((
_tmp9C=*((char*)_check_dynforward_subscript(_tmp9B,sizeof(char),0)),((_tmp9D=*((
const char*)_check_dynforward_subscript(b,sizeof(char),j)),((_get_dynforward_size(
_tmp9B,sizeof(char))== 1  && (_tmp9C == '\000'  && _tmp9D != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp9B.curr)=_tmp9D)))))));}return ans;}struct
_dynforward_ptr Cyc_strconcat(struct _dynforward_ptr a,struct _dynforward_ptr b);
struct _dynforward_ptr Cyc_strconcat(struct _dynforward_ptr a,struct _dynforward_ptr b){
return Cyc_rstrconcat(Cyc_Core_heap_region,a,b);}struct _dynforward_ptr Cyc_rstrconcat_l(
struct _RegionHandle*r,struct Cyc_List_List*strs);struct _dynforward_ptr Cyc_rstrconcat_l(
struct _RegionHandle*r,struct Cyc_List_List*strs){unsigned long len;unsigned long
total_len=0;struct _dynforward_ptr ans;{struct _RegionHandle _tmp1F=_new_region("temp");
struct _RegionHandle*temp=& _tmp1F;_push_region(temp);{struct Cyc_List_List*_tmp9E;
struct Cyc_List_List*lens=(_tmp9E=_region_malloc(temp,sizeof(*_tmp9E)),((_tmp9E->hd=(
void*)((unsigned long)0),((_tmp9E->tl=0,_tmp9E)))));struct Cyc_List_List*end=lens;{
struct Cyc_List_List*p=strs;for(0;p != 0;p=p->tl){len=Cyc_strlen((struct
_dynforward_ptr)*((struct _dynforward_ptr*)p->hd));total_len +=len;{struct Cyc_List_List*
_tmp9F;((struct Cyc_List_List*)_check_null(end))->tl=((_tmp9F=_region_malloc(temp,
sizeof(*_tmp9F)),((_tmp9F->hd=(void*)len,((_tmp9F->tl=0,_tmp9F))))));}end=((
struct Cyc_List_List*)_check_null(end))->tl;}}lens=lens->tl;ans=Cyc_Core_rnew_string(
r,total_len + 1);{unsigned long i=0;while(strs != 0){struct _dynforward_ptr _tmp21=*((
struct _dynforward_ptr*)strs->hd);len=(unsigned long)((struct Cyc_List_List*)
_check_null(lens))->hd;Cyc_strncpy(_dynforward_ptr_decrease_size(
_dynforward_ptr_plus(ans,sizeof(char),(int)i),sizeof(char),1),(struct
_dynforward_ptr)_tmp21,len);i +=len;strs=strs->tl;lens=lens->tl;}}};_pop_region(
temp);}return ans;}struct _dynforward_ptr Cyc_strconcat_l(struct Cyc_List_List*strs);
struct _dynforward_ptr Cyc_strconcat_l(struct Cyc_List_List*strs){return Cyc_rstrconcat_l(
Cyc_Core_heap_region,strs);}struct _dynforward_ptr Cyc_rstr_sepstr(struct
_RegionHandle*r,struct Cyc_List_List*strs,struct _dynforward_ptr separator);struct
_dynforward_ptr Cyc_rstr_sepstr(struct _RegionHandle*r,struct Cyc_List_List*strs,
struct _dynforward_ptr separator){if(strs == 0)return Cyc_Core_rnew_string(r,0);if(
strs->tl == 0)return Cyc_rstrdup(r,(struct _dynforward_ptr)*((struct _dynforward_ptr*)
strs->hd));{struct Cyc_List_List*_tmp23=strs;struct _RegionHandle _tmp24=
_new_region("temp");struct _RegionHandle*temp=& _tmp24;_push_region(temp);{struct
Cyc_List_List*_tmpA0;struct Cyc_List_List*lens=(_tmpA0=_region_malloc(temp,
sizeof(*_tmpA0)),((_tmpA0->hd=(void*)((unsigned long)0),((_tmpA0->tl=0,_tmpA0)))));
struct Cyc_List_List*end=lens;unsigned long len;unsigned long total_len=0;
unsigned long list_len=0;for(0;_tmp23 != 0;_tmp23=_tmp23->tl){len=Cyc_strlen((
struct _dynforward_ptr)*((struct _dynforward_ptr*)_tmp23->hd));total_len +=len;{
struct Cyc_List_List*_tmpA1;((struct Cyc_List_List*)_check_null(end))->tl=((_tmpA1=
_region_malloc(temp,sizeof(*_tmpA1)),((_tmpA1->hd=(void*)len,((_tmpA1->tl=0,
_tmpA1))))));}end=((struct Cyc_List_List*)_check_null(end))->tl;++ list_len;}lens=
lens->tl;{unsigned long seplen=Cyc_strlen(separator);total_len +=(list_len - 1)* 
seplen;{struct _dynforward_ptr ans=Cyc_Core_rnew_string(r,total_len + 1);
unsigned long i=0;while(strs->tl != 0){struct _dynforward_ptr _tmp26=*((struct
_dynforward_ptr*)strs->hd);len=(unsigned long)((struct Cyc_List_List*)_check_null(
lens))->hd;Cyc_strncpy(_dynforward_ptr_decrease_size(_dynforward_ptr_plus(ans,
sizeof(char),(int)i),sizeof(char),1),(struct _dynforward_ptr)_tmp26,len);i +=len;
Cyc_strncpy(_dynforward_ptr_decrease_size(_dynforward_ptr_plus(ans,sizeof(char),(
int)i),sizeof(char),1),separator,seplen);i +=seplen;strs=strs->tl;lens=lens->tl;}
Cyc_strncpy(_dynforward_ptr_decrease_size(_dynforward_ptr_plus(ans,sizeof(char),(
int)i),sizeof(char),1),(struct _dynforward_ptr)*((struct _dynforward_ptr*)strs->hd),(
unsigned long)((struct Cyc_List_List*)_check_null(lens))->hd);{struct
_dynforward_ptr _tmp27=ans;_npop_handler(0);return _tmp27;}}}};_pop_region(temp);}}
struct _dynforward_ptr Cyc_str_sepstr(struct Cyc_List_List*strs,struct
_dynforward_ptr separator);struct _dynforward_ptr Cyc_str_sepstr(struct Cyc_List_List*
strs,struct _dynforward_ptr separator){return Cyc_rstr_sepstr(Cyc_Core_heap_region,
strs,separator);}struct _dynforward_ptr Cyc_strncpy(struct _dynforward_ptr dest,
struct _dynforward_ptr src,unsigned long n);struct _dynforward_ptr Cyc_strncpy(struct
_dynforward_ptr dest,struct _dynforward_ptr src,unsigned long n){int i;{const char*
_tmpA3;const char*_tmpA2;n <= _get_dynforward_size(src,sizeof(char)) && n <= 
_get_dynforward_size(dest,sizeof(char))?0:((int(*)(struct _dynforward_ptr
assertion,struct _dynforward_ptr file,unsigned int line))Cyc___assert_fail)(((
_tmpA2="n <= src.size && n <= dest.size",_tag_dynforward(_tmpA2,sizeof(char),
_get_zero_arr_size_char(_tmpA2,32)))),((_tmpA3="string.cyc",_tag_dynforward(
_tmpA3,sizeof(char),_get_zero_arr_size_char(_tmpA3,11)))),299);}for(i=0;i < n;++ i){
char _tmp2B=*((const char*)_check_dynforward_subscript(src,sizeof(char),i));if(
_tmp2B == '\000')break;((char*)dest.curr)[i]=_tmp2B;}for(0;i < n;++ i){((char*)dest.curr)[
i]='\000';}return dest;}struct _dynforward_ptr Cyc_zstrncpy(struct _dynforward_ptr
dest,struct _dynforward_ptr src,unsigned long n);struct _dynforward_ptr Cyc_zstrncpy(
struct _dynforward_ptr dest,struct _dynforward_ptr src,unsigned long n){{const char*
_tmpA5;const char*_tmpA4;n <= _get_dynforward_size(dest,sizeof(char)) && n <= 
_get_dynforward_size(src,sizeof(char))?0:((int(*)(struct _dynforward_ptr assertion,
struct _dynforward_ptr file,unsigned int line))Cyc___assert_fail)(((_tmpA4="n <= dest.size && n <= src.size",
_tag_dynforward(_tmpA4,sizeof(char),_get_zero_arr_size_char(_tmpA4,32)))),((
_tmpA5="string.cyc",_tag_dynforward(_tmpA5,sizeof(char),_get_zero_arr_size_char(
_tmpA5,11)))),313);}{int i;for(i=0;i < n;++ i){((char*)dest.curr)[i]=*((const char*)
_check_dynforward_subscript(src,sizeof(char),i));}return dest;}}struct
_dynforward_ptr Cyc_strcpy(struct _dynforward_ptr dest,struct _dynforward_ptr src);
struct _dynforward_ptr Cyc_strcpy(struct _dynforward_ptr dest,struct _dynforward_ptr
src){unsigned int ssz=_get_dynforward_size(src,sizeof(char));unsigned int dsz=
_get_dynforward_size(dest,sizeof(char));if(ssz <= dsz){unsigned int i;for(i=0;i < 
ssz;++ i){char _tmp2E=*((const char*)_check_dynforward_subscript(src,sizeof(char),(
int)i));{char _tmpA8;char _tmpA7;struct _dynforward_ptr _tmpA6;(_tmpA6=
_dynforward_ptr_plus(dest,sizeof(char),(int)i),((_tmpA7=*((char*)
_check_dynforward_subscript(_tmpA6,sizeof(char),0)),((_tmpA8=_tmp2E,((
_get_dynforward_size(_tmpA6,sizeof(char))== 1  && (_tmpA7 == '\000'  && _tmpA8 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmpA6.curr)=_tmpA8)))))));}if(_tmp2E == '\000')
break;}}else{unsigned long len=Cyc_strlen(src);Cyc_strncpy(
_dynforward_ptr_decrease_size(dest,sizeof(char),1),src,len);if(len < 
_get_dynforward_size(dest,sizeof(char))){char _tmpAB;char _tmpAA;struct
_dynforward_ptr _tmpA9;(_tmpA9=_dynforward_ptr_plus(dest,sizeof(char),(int)len),((
_tmpAA=*((char*)_check_dynforward_subscript(_tmpA9,sizeof(char),0)),((_tmpAB='\000',((
_get_dynforward_size(_tmpA9,sizeof(char))== 1  && (_tmpAA == '\000'  && _tmpAB != '\000')?
_throw_arraybounds(): 1,*((char*)_tmpA9.curr)=_tmpAB)))))));}}return dest;}struct
_dynforward_ptr Cyc_rstrdup(struct _RegionHandle*r,struct _dynforward_ptr src);
struct _dynforward_ptr Cyc_rstrdup(struct _RegionHandle*r,struct _dynforward_ptr src){
unsigned long len;struct _dynforward_ptr temp;len=Cyc_strlen(src);temp=Cyc_Core_rnew_string(
r,len + 1);Cyc_strncpy(_dynforward_ptr_decrease_size(temp,sizeof(char),1),src,len);
return temp;}struct _dynforward_ptr Cyc_strdup(struct _dynforward_ptr src);struct
_dynforward_ptr Cyc_strdup(struct _dynforward_ptr src){return Cyc_rstrdup(Cyc_Core_heap_region,
src);}struct _dynforward_ptr Cyc_rexpand(struct _RegionHandle*r,struct
_dynforward_ptr s,unsigned long sz);struct _dynforward_ptr Cyc_rexpand(struct
_RegionHandle*r,struct _dynforward_ptr s,unsigned long sz){struct _dynforward_ptr
temp;unsigned long slen;slen=Cyc_strlen(s);sz=sz > slen?sz: slen;temp=Cyc_Core_rnew_string(
r,sz);Cyc_strncpy(_dynforward_ptr_decrease_size(temp,sizeof(char),1),s,slen);if(
slen != _get_dynforward_size(s,sizeof(char))){char _tmpAE;char _tmpAD;struct
_dynforward_ptr _tmpAC;(_tmpAC=_dynforward_ptr_plus(temp,sizeof(char),(int)slen),((
_tmpAD=*((char*)_check_dynforward_subscript(_tmpAC,sizeof(char),0)),((_tmpAE='\000',((
_get_dynforward_size(_tmpAC,sizeof(char))== 1  && (_tmpAD == '\000'  && _tmpAE != '\000')?
_throw_arraybounds(): 1,*((char*)_tmpAC.curr)=_tmpAE)))))));}return temp;}struct
_dynforward_ptr Cyc_expand(struct _dynforward_ptr s,unsigned long sz);struct
_dynforward_ptr Cyc_expand(struct _dynforward_ptr s,unsigned long sz){return Cyc_rexpand(
Cyc_Core_heap_region,s,sz);}struct _dynforward_ptr Cyc_rrealloc_str(struct
_RegionHandle*r,struct _dynforward_ptr str,unsigned long sz);struct _dynforward_ptr
Cyc_rrealloc_str(struct _RegionHandle*r,struct _dynforward_ptr str,unsigned long sz){
unsigned long maxsizeP=_get_dynforward_size(str,sizeof(char));if(maxsizeP == 0){
maxsizeP=30 > sz?30: sz;str=Cyc_Core_rnew_string(r,maxsizeP);{char _tmpB1;char
_tmpB0;struct _dynforward_ptr _tmpAF;(_tmpAF=_dynforward_ptr_plus(str,sizeof(char),
0),((_tmpB0=*((char*)_check_dynforward_subscript(_tmpAF,sizeof(char),0)),((
_tmpB1='\000',((_get_dynforward_size(_tmpAF,sizeof(char))== 1  && (_tmpB0 == '\000'
 && _tmpB1 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpAF.curr)=_tmpB1)))))));}}
else{if(sz > maxsizeP){maxsizeP=maxsizeP * 2 > (sz * 5)/ 4?maxsizeP * 2:(sz * 5)/ 4;
str=Cyc_rexpand(r,(struct _dynforward_ptr)str,maxsizeP);}}return str;}struct
_dynforward_ptr Cyc_realloc_str(struct _dynforward_ptr str,unsigned long sz);struct
_dynforward_ptr Cyc_realloc_str(struct _dynforward_ptr str,unsigned long sz){return
Cyc_rrealloc_str(Cyc_Core_heap_region,str,sz);}struct _dynforward_ptr Cyc_rsubstring(
struct _RegionHandle*r,struct _dynforward_ptr s,int start,unsigned long amt);struct
_dynforward_ptr Cyc_rsubstring(struct _RegionHandle*r,struct _dynforward_ptr s,int
start,unsigned long amt){struct _dynforward_ptr ans=Cyc_Core_rnew_string(r,amt + 1);
s=_dynforward_ptr_plus(s,sizeof(char),start);{const char*_tmpB3;const char*_tmpB2;
amt < _get_dynforward_size(ans,sizeof(char)) && amt <= _get_dynforward_size(s,
sizeof(char))?0:((int(*)(struct _dynforward_ptr assertion,struct _dynforward_ptr
file,unsigned int line))Cyc___assert_fail)(((_tmpB2="amt < ans.size && amt <= s.size",
_tag_dynforward(_tmpB2,sizeof(char),_get_zero_arr_size_char(_tmpB2,32)))),((
_tmpB3="string.cyc",_tag_dynforward(_tmpB3,sizeof(char),_get_zero_arr_size_char(
_tmpB3,11)))),409);}{unsigned long i=0;for(0;i < amt;++ i){char _tmpB6;char _tmpB5;
struct _dynforward_ptr _tmpB4;(_tmpB4=_dynforward_ptr_plus(ans,sizeof(char),(int)i),((
_tmpB5=*((char*)_check_dynforward_subscript(_tmpB4,sizeof(char),0)),((_tmpB6=*((
const char*)_check_dynforward_subscript(s,sizeof(char),(int)i)),((
_get_dynforward_size(_tmpB4,sizeof(char))== 1  && (_tmpB5 == '\000'  && _tmpB6 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmpB4.curr)=_tmpB6)))))));}}{char _tmpB9;char
_tmpB8;struct _dynforward_ptr _tmpB7;(_tmpB7=_dynforward_ptr_plus(ans,sizeof(char),(
int)amt),((_tmpB8=*((char*)_check_dynforward_subscript(_tmpB7,sizeof(char),0)),((
_tmpB9='\000',((_get_dynforward_size(_tmpB7,sizeof(char))== 1  && (_tmpB8 == '\000'
 && _tmpB9 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpB7.curr)=_tmpB9)))))));}
return ans;}struct _dynforward_ptr Cyc_substring(struct _dynforward_ptr s,int start,
unsigned long amt);struct _dynforward_ptr Cyc_substring(struct _dynforward_ptr s,int
start,unsigned long amt){return Cyc_rsubstring(Cyc_Core_heap_region,s,start,amt);}
struct _dynforward_ptr Cyc_rreplace_suffix(struct _RegionHandle*r,struct
_dynforward_ptr src,struct _dynforward_ptr curr_suffix,struct _dynforward_ptr
new_suffix);struct _dynforward_ptr Cyc_rreplace_suffix(struct _RegionHandle*r,
struct _dynforward_ptr src,struct _dynforward_ptr curr_suffix,struct _dynforward_ptr
new_suffix){unsigned long m=_get_dynforward_size(src,sizeof(char));unsigned long n=
_get_dynforward_size(curr_suffix,sizeof(char));const char*_tmpBA;struct
_dynforward_ptr err=(_tmpBA="replace_suffix",_tag_dynforward(_tmpBA,sizeof(char),
_get_zero_arr_size_char(_tmpBA,15)));if(m < n){struct Cyc_Core_Invalid_argument_struct
_tmpBD;struct Cyc_Core_Invalid_argument_struct*_tmpBC;(int)_throw((void*)((_tmpBC=
_cycalloc(sizeof(*_tmpBC)),((_tmpBC[0]=((_tmpBD.tag=Cyc_Core_Invalid_argument,((
_tmpBD.f1=err,_tmpBD)))),_tmpBC)))));}{unsigned long i=1;for(0;i <= n;++ i){if(*((
const char*)_check_dynforward_subscript(src,sizeof(char),(int)(m - i)))!= *((const
char*)_check_dynforward_subscript(curr_suffix,sizeof(char),(int)(n - i)))){struct
Cyc_Core_Invalid_argument_struct _tmpC0;struct Cyc_Core_Invalid_argument_struct*
_tmpBF;(int)_throw((void*)((_tmpBF=_cycalloc(sizeof(*_tmpBF)),((_tmpBF[0]=((
_tmpC0.tag=Cyc_Core_Invalid_argument,((_tmpC0.f1=err,_tmpC0)))),_tmpBF)))));}}}{
struct _dynforward_ptr ans=Cyc_Core_rnew_string(r,((m - n)+ _get_dynforward_size(
new_suffix,sizeof(char)))+ 1);Cyc_strncpy(_dynforward_ptr_decrease_size(ans,
sizeof(char),1),src,m - n);Cyc_strncpy(_dynforward_ptr_decrease_size(
_dynforward_ptr_plus(ans,sizeof(char),(int)(m - n)),sizeof(char),1),new_suffix,
_get_dynforward_size(new_suffix,sizeof(char)));return ans;}}struct _dynforward_ptr
Cyc_replace_suffix(struct _dynforward_ptr src,struct _dynforward_ptr curr_suffix,
struct _dynforward_ptr new_suffix);struct _dynforward_ptr Cyc_replace_suffix(struct
_dynforward_ptr src,struct _dynforward_ptr curr_suffix,struct _dynforward_ptr
new_suffix){return Cyc_rreplace_suffix(Cyc_Core_heap_region,src,curr_suffix,
new_suffix);}struct _dynforward_ptr Cyc_strpbrk(struct _dynforward_ptr s,struct
_dynforward_ptr accept);struct _dynforward_ptr Cyc_strpbrk(struct _dynforward_ptr s,
struct _dynforward_ptr accept){int len=(int)_get_dynforward_size(s,sizeof(char));
unsigned int asize=_get_dynforward_size(accept,sizeof(char));char c;unsigned int i;
for(i=0;i < len  && (c=*((const char*)_check_dynforward_subscript(s,sizeof(char),(
int)i)))!= 0;++ i){unsigned int j=0;for(0;j < asize;++ j){if(c == *((const char*)
_check_dynforward_subscript(accept,sizeof(char),(int)j)))return
_dynforward_ptr_plus(s,sizeof(char),(int)i);}}return(struct _dynforward_ptr)
_tag_dynforward(0,0,0);}struct _dynforward_ptr Cyc_mstrpbrk(struct _dynforward_ptr s,
struct _dynforward_ptr accept);struct _dynforward_ptr Cyc_mstrpbrk(struct
_dynforward_ptr s,struct _dynforward_ptr accept){int len=(int)_get_dynforward_size(s,
sizeof(char));unsigned int asize=_get_dynforward_size(accept,sizeof(char));char c;
unsigned int i;for(i=0;i < len  && (c=*((char*)_check_dynforward_subscript(s,
sizeof(char),(int)i)))!= 0;++ i){unsigned int j=0;for(0;j < asize;++ j){if(c == *((
const char*)_check_dynforward_subscript(accept,sizeof(char),(int)j)))return
_dynforward_ptr_plus(s,sizeof(char),(int)i);}}return _tag_dynforward(0,0,0);}
struct _dynforward_ptr Cyc_mstrchr(struct _dynforward_ptr s,char c);struct
_dynforward_ptr Cyc_mstrchr(struct _dynforward_ptr s,char c){int len=(int)
_get_dynforward_size(s,sizeof(char));char c2;unsigned int i;for(i=0;i < len  && (c2=*((
char*)_check_dynforward_subscript(s,sizeof(char),(int)i)))!= 0;++ i){if(c2 == c)
return _dynforward_ptr_plus(s,sizeof(char),(int)i);}return _tag_dynforward(0,0,0);}
struct _dynforward_ptr Cyc_strchr(struct _dynforward_ptr s,char c);struct
_dynforward_ptr Cyc_strchr(struct _dynforward_ptr s,char c){int len=(int)
_get_dynforward_size(s,sizeof(char));char c2;unsigned int i;for(i=0;i < len  && (c2=*((
const char*)_check_dynforward_subscript(s,sizeof(char),(int)i)))!= 0;++ i){if(c2 == 
c)return _dynforward_ptr_plus(s,sizeof(char),(int)i);}return(struct
_dynforward_ptr)_tag_dynforward(0,0,0);}struct _dynforward_ptr Cyc_strrchr(struct
_dynforward_ptr s0,char c);struct _dynforward_ptr Cyc_strrchr(struct _dynforward_ptr
s0,char c){int len=(int)Cyc_strlen((struct _dynforward_ptr)s0);int i=len - 1;struct
_dyneither_ptr s=_dynforward_to_dyneither(s0);_dyneither_ptr_inplace_plus(& s,
sizeof(char),i);for(0;i >= 0;(i --,_dyneither_ptr_inplace_plus_post(& s,sizeof(char),
-1))){if(*((const char*)_check_dyneither_subscript(s,sizeof(char),0))== c)return
_dyneither_to_dynforward(s);}return(struct _dynforward_ptr)_tag_dynforward(0,0,0);}
struct _dynforward_ptr Cyc_mstrrchr(struct _dynforward_ptr s0,char c);struct
_dynforward_ptr Cyc_mstrrchr(struct _dynforward_ptr s0,char c){int len=(int)Cyc_strlen((
struct _dynforward_ptr)s0);int i=len - 1;struct _dyneither_ptr s=
_dynforward_to_dyneither(s0);_dyneither_ptr_inplace_plus(& s,sizeof(char),i);for(
0;i >= 0;(i --,_dyneither_ptr_inplace_plus_post(& s,sizeof(char),-1))){if(*((char*)
_check_dyneither_subscript(s,sizeof(char),0))== c)return _dyneither_to_dynforward(
s);}return _tag_dynforward(0,0,0);}struct _dynforward_ptr Cyc_strstr(struct
_dynforward_ptr haystack,struct _dynforward_ptr needle);struct _dynforward_ptr Cyc_strstr(
struct _dynforward_ptr haystack,struct _dynforward_ptr needle){if(!((unsigned int)
haystack.curr) || !((unsigned int)needle.curr)){struct Cyc_Core_Invalid_argument_struct
_tmpC6;const char*_tmpC5;struct Cyc_Core_Invalid_argument_struct*_tmpC4;(int)
_throw((void*)((_tmpC4=_cycalloc(sizeof(*_tmpC4)),((_tmpC4[0]=((_tmpC6.tag=Cyc_Core_Invalid_argument,((
_tmpC6.f1=((_tmpC5="strstr",_tag_dynforward(_tmpC5,sizeof(char),
_get_zero_arr_size_char(_tmpC5,7)))),_tmpC6)))),_tmpC4)))));}if(*((const char*)
_check_dynforward_subscript(needle,sizeof(char),0))== '\000')return haystack;{int
len=(int)Cyc_strlen((struct _dynforward_ptr)needle);{struct _dynforward_ptr start=
haystack;for(0;(start=Cyc_strchr(start,*((const char*)_check_dynforward_subscript(
needle,sizeof(char),0)))).curr != ((struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr;
start=Cyc_strchr(_dynforward_ptr_plus(start,sizeof(char),1),*((const char*)
_check_dynforward_subscript(needle,sizeof(char),0)))){if(Cyc_strncmp((struct
_dynforward_ptr)start,(struct _dynforward_ptr)needle,(unsigned long)len)== 0)
return start;}}return(struct _dynforward_ptr)_tag_dynforward(0,0,0);}}struct
_dynforward_ptr Cyc_mstrstr(struct _dynforward_ptr haystack,struct _dynforward_ptr
needle);struct _dynforward_ptr Cyc_mstrstr(struct _dynforward_ptr haystack,struct
_dynforward_ptr needle){if(!((unsigned int)haystack.curr) || !((unsigned int)
needle.curr)){struct Cyc_Core_Invalid_argument_struct _tmpCC;const char*_tmpCB;
struct Cyc_Core_Invalid_argument_struct*_tmpCA;(int)_throw((void*)((_tmpCA=
_cycalloc(sizeof(*_tmpCA)),((_tmpCA[0]=((_tmpCC.tag=Cyc_Core_Invalid_argument,((
_tmpCC.f1=((_tmpCB="mstrstr",_tag_dynforward(_tmpCB,sizeof(char),
_get_zero_arr_size_char(_tmpCB,8)))),_tmpCC)))),_tmpCA)))));}if(*((const char*)
_check_dynforward_subscript(needle,sizeof(char),0))== '\000')return haystack;{int
len=(int)Cyc_strlen((struct _dynforward_ptr)needle);{struct _dynforward_ptr start=
haystack;for(0;(start=Cyc_mstrchr(start,*((const char*)
_check_dynforward_subscript(needle,sizeof(char),0)))).curr != (_tag_dynforward(0,
0,0)).curr;start=Cyc_mstrchr(_dynforward_ptr_plus(start,sizeof(char),1),*((const
char*)_check_dynforward_subscript(needle,sizeof(char),0)))){if(Cyc_strncmp((
struct _dynforward_ptr)start,(struct _dynforward_ptr)needle,(unsigned long)len)== 
0)return start;}}return _tag_dynforward(0,0,0);}}unsigned long Cyc_strspn(struct
_dynforward_ptr s,struct _dynforward_ptr accept);unsigned long Cyc_strspn(struct
_dynforward_ptr s,struct _dynforward_ptr accept){unsigned long len=Cyc_strlen((
struct _dynforward_ptr)s);unsigned int asize=_get_dynforward_size(accept,sizeof(
char));{const char*_tmpCE;const char*_tmpCD;len <= _get_dynforward_size(s,sizeof(
char))?0:((int(*)(struct _dynforward_ptr assertion,struct _dynforward_ptr file,
unsigned int line))Cyc___assert_fail)(((_tmpCD="len <= s.size",_tag_dynforward(
_tmpCD,sizeof(char),_get_zero_arr_size_char(_tmpCD,14)))),((_tmpCE="string.cyc",
_tag_dynforward(_tmpCE,sizeof(char),_get_zero_arr_size_char(_tmpCE,11)))),559);}{
unsigned long i=0;for(0;i < len;++ i){int j;for(j=0;j < asize;++ j){if(*((const char*)
_check_dynforward_subscript(s,sizeof(char),(int)i))== *((const char*)
_check_dynforward_subscript(accept,sizeof(char),j)))break;}if(j == asize)return i;}}
return len;}unsigned long Cyc_strcspn(struct _dynforward_ptr s,struct _dynforward_ptr
accept);unsigned long Cyc_strcspn(struct _dynforward_ptr s,struct _dynforward_ptr
accept){unsigned long len=Cyc_strlen((struct _dynforward_ptr)s);unsigned int asize=
_get_dynforward_size(accept,sizeof(char));{const char*_tmpD0;const char*_tmpCF;len
<= _get_dynforward_size(s,sizeof(char))?0:((int(*)(struct _dynforward_ptr
assertion,struct _dynforward_ptr file,unsigned int line))Cyc___assert_fail)(((
_tmpCF="len <= s.size",_tag_dynforward(_tmpCF,sizeof(char),
_get_zero_arr_size_char(_tmpCF,14)))),((_tmpD0="string.cyc",_tag_dynforward(
_tmpD0,sizeof(char),_get_zero_arr_size_char(_tmpD0,11)))),579);}{unsigned long i=
0;for(0;i < len;++ i){int j;for(j=0;j < asize;++ j){if(*((const char*)
_check_dynforward_subscript(s,sizeof(char),(int)i))!= *((const char*)
_check_dynforward_subscript(accept,sizeof(char),j)))break;}if(j == asize)return i;}}
return len;}struct _dynforward_ptr Cyc_strtok(struct _dynforward_ptr s,struct
_dynforward_ptr delim);struct _dynforward_ptr Cyc_strtok(struct _dynforward_ptr s,
struct _dynforward_ptr delim){static struct _dynforward_ptr olds={(void*)0,(void*)(0 + 
0)};struct _dynforward_ptr token;if(s.curr == (_tag_dynforward(0,0,0)).curr){if(
olds.curr == (_tag_dynforward(0,0,0)).curr)return _tag_dynforward(0,0,0);s=olds;}{
unsigned long inc=Cyc_strspn((struct _dynforward_ptr)s,delim);if(inc >= 
_get_dynforward_size(s,sizeof(char)) || *((char*)_check_dynforward_subscript(
_dynforward_ptr_plus(s,sizeof(char),(int)inc),sizeof(char),0))== '\000'){olds=
_tag_dynforward(0,0,0);return _tag_dynforward(0,0,0);}else{
_dynforward_ptr_inplace_plus(& s,sizeof(char),(int)inc);}token=s;s=Cyc_mstrpbrk(
token,(struct _dynforward_ptr)delim);if(s.curr == (_tag_dynforward(0,0,0)).curr)
olds=_tag_dynforward(0,0,0);else{{char _tmpD3;char _tmpD2;struct _dynforward_ptr
_tmpD1;(_tmpD1=s,((_tmpD2=*((char*)_check_dynforward_subscript(_tmpD1,sizeof(
char),0)),((_tmpD3='\000',((_get_dynforward_size(_tmpD1,sizeof(char))== 1  && (
_tmpD2 == '\000'  && _tmpD3 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpD1.curr)=
_tmpD3)))))));}olds=_dynforward_ptr_plus(s,sizeof(char),1);}return token;}}struct
Cyc_List_List*Cyc_rexplode(struct _RegionHandle*r,struct _dynforward_ptr s);struct
Cyc_List_List*Cyc_rexplode(struct _RegionHandle*r,struct _dynforward_ptr s){struct
Cyc_List_List*result=0;{int i=(int)(Cyc_strlen(s)- 1);for(0;i >= 0;-- i){struct Cyc_List_List*
_tmpD4;result=((_tmpD4=_region_malloc(r,sizeof(*_tmpD4)),((_tmpD4->hd=(void*)((
int)*((const char*)_check_dynforward_subscript(s,sizeof(char),i))),((_tmpD4->tl=
result,_tmpD4))))));}}return result;}struct Cyc_List_List*Cyc_explode(struct
_dynforward_ptr s);struct Cyc_List_List*Cyc_explode(struct _dynforward_ptr s){return
Cyc_rexplode(Cyc_Core_heap_region,s);}struct _dynforward_ptr Cyc_implode(struct Cyc_List_List*
chars);struct _dynforward_ptr Cyc_implode(struct Cyc_List_List*chars){struct
_dynforward_ptr s=Cyc_Core_new_string((unsigned int)(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(chars)+ 1));unsigned long i=0;while(chars != 0){{char _tmpD7;
char _tmpD6;struct _dynforward_ptr _tmpD5;(_tmpD5=_dynforward_ptr_plus(s,sizeof(
char),(int)i ++),((_tmpD6=*((char*)_check_dynforward_subscript(_tmpD5,sizeof(char),
0)),((_tmpD7=(char)((int)chars->hd),((_get_dynforward_size(_tmpD5,sizeof(char))
== 1  && (_tmpD6 == '\000'  && _tmpD7 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmpD5.curr)=_tmpD7)))))));}chars=chars->tl;}return s;}static int Cyc_casecmp(
struct _dynforward_ptr s1,unsigned long len1,struct _dynforward_ptr s2,unsigned long
len2);inline static int Cyc_casecmp(struct _dynforward_ptr s1,unsigned long len1,
struct _dynforward_ptr s2,unsigned long len2){unsigned long min_length=len1 < len2?
len1: len2;{const char*_tmpD9;const char*_tmpD8;min_length <= _get_dynforward_size(
s1,sizeof(char)) && min_length <= _get_dynforward_size(s2,sizeof(char))?0:((int(*)(
struct _dynforward_ptr assertion,struct _dynforward_ptr file,unsigned int line))Cyc___assert_fail)(((
_tmpD8="min_length <= s1.size && min_length <= s2.size",_tag_dynforward(_tmpD8,
sizeof(char),_get_zero_arr_size_char(_tmpD8,47)))),((_tmpD9="string.cyc",
_tag_dynforward(_tmpD9,sizeof(char),_get_zero_arr_size_char(_tmpD9,11)))),660);}{
int i=- 1;while((++ i,i < min_length)){int diff=toupper((int)*((const char*)
_check_dynforward_subscript(s1,sizeof(char),i)))- toupper((int)*((const char*)
_check_dynforward_subscript(s2,sizeof(char),i)));if(diff != 0)return diff;}return(
int)len1 - (int)len2;}}int Cyc_strcasecmp(struct _dynforward_ptr s1,struct
_dynforward_ptr s2);int Cyc_strcasecmp(struct _dynforward_ptr s1,struct
_dynforward_ptr s2){if(s1.curr == s2.curr)return 0;{unsigned long len1=Cyc_strlen(s1);
unsigned long len2=Cyc_strlen(s2);return Cyc_casecmp(s1,len1,s2,len2);}}static int
Cyc_caseless_ncmp(struct _dynforward_ptr s1,unsigned long len1,struct
_dynforward_ptr s2,unsigned long len2,unsigned long n);inline static int Cyc_caseless_ncmp(
struct _dynforward_ptr s1,unsigned long len1,struct _dynforward_ptr s2,unsigned long
len2,unsigned long n){if(n <= 0)return 0;{unsigned long min_len=len1 > len2?len2: len1;
unsigned long bound=min_len > n?n: min_len;{const char*_tmpDB;const char*_tmpDA;bound
<= _get_dynforward_size(s1,sizeof(char)) && bound <= _get_dynforward_size(s2,
sizeof(char))?0:((int(*)(struct _dynforward_ptr assertion,struct _dynforward_ptr
file,unsigned int line))Cyc___assert_fail)(((_tmpDA="bound <= s1.size && bound <= s2.size",
_tag_dynforward(_tmpDA,sizeof(char),_get_zero_arr_size_char(_tmpDA,37)))),((
_tmpDB="string.cyc",_tag_dynforward(_tmpDB,sizeof(char),_get_zero_arr_size_char(
_tmpDB,11)))),687);}{int i=0;for(0;i < bound;++ i){int retc;if((retc=toupper((int)*((
const char*)_check_dynforward_subscript(s1,sizeof(char),i)))- toupper((int)*((
const char*)_check_dynforward_subscript(s2,sizeof(char),i))))!= 0)return retc;}}
if(len1 < n  || len2 < n)return(int)len1 - (int)len2;return 0;}}int Cyc_strncasecmp(
struct _dynforward_ptr s1,struct _dynforward_ptr s2,unsigned long n);int Cyc_strncasecmp(
struct _dynforward_ptr s1,struct _dynforward_ptr s2,unsigned long n){unsigned long
len1=Cyc_strlen(s1);unsigned long len2=Cyc_strlen(s2);return Cyc_caseless_ncmp(s1,
len1,s2,len2,n);}void*memcpy(void*,const void*,unsigned long n);void*memmove(void*,
const void*,unsigned long n);int memcmp(const void*,const void*,unsigned long n);char*
memchr(const char*,char c,unsigned long n);void*memset(void*,int c,unsigned long n);
void bcopy(const void*src,void*dest,unsigned long n);void bzero(void*s,unsigned long
n);char*GC_realloc(char*,unsigned long n);struct _dynforward_ptr Cyc_realloc(struct
_dynforward_ptr s,unsigned long n);struct _dynforward_ptr Cyc_realloc(struct
_dynforward_ptr s,unsigned long n){char*_tmp5D=GC_realloc((char*)_check_null(
_untag_dynforward_ptr(s,sizeof(char),1)),n);return wrap_Cbuffer_as_buffer(_tmp5D,
n);}struct _dynforward_ptr Cyc__memcpy(struct _dynforward_ptr d,struct
_dynforward_ptr s,unsigned long n,unsigned int sz);struct _dynforward_ptr Cyc__memcpy(
struct _dynforward_ptr d,struct _dynforward_ptr s,unsigned long n,unsigned int sz){if(((
d.curr == (_tag_dynforward(0,0,0)).curr  || _get_dynforward_size(d,sizeof(void))< 
n) || s.curr == ((struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr) || 
_get_dynforward_size(s,sizeof(void))< n){struct Cyc_Core_Invalid_argument_struct
_tmpE1;const char*_tmpE0;struct Cyc_Core_Invalid_argument_struct*_tmpDF;(int)
_throw((void*)((_tmpDF=_cycalloc(sizeof(*_tmpDF)),((_tmpDF[0]=((_tmpE1.tag=Cyc_Core_Invalid_argument,((
_tmpE1.f1=((_tmpE0="memcpy",_tag_dynforward(_tmpE0,sizeof(char),
_get_zero_arr_size_char(_tmpE0,7)))),_tmpE1)))),_tmpDF)))));}memcpy((void*)
_check_null(_untag_dynforward_ptr(d,sizeof(void),1)),(const void*)_check_null(
_untag_dynforward_ptr(s,sizeof(void),1)),n * sz);return d;}struct _dynforward_ptr
Cyc__memmove(struct _dynforward_ptr d,struct _dynforward_ptr s,unsigned long n,
unsigned int sz);struct _dynforward_ptr Cyc__memmove(struct _dynforward_ptr d,struct
_dynforward_ptr s,unsigned long n,unsigned int sz){if(((d.curr == (_tag_dynforward(0,
0,0)).curr  || _get_dynforward_size(d,sizeof(void))< n) || s.curr == ((struct
_dynforward_ptr)_tag_dynforward(0,0,0)).curr) || _get_dynforward_size(s,sizeof(
void))< n){struct Cyc_Core_Invalid_argument_struct _tmpE7;const char*_tmpE6;struct
Cyc_Core_Invalid_argument_struct*_tmpE5;(int)_throw((void*)((_tmpE5=_cycalloc(
sizeof(*_tmpE5)),((_tmpE5[0]=((_tmpE7.tag=Cyc_Core_Invalid_argument,((_tmpE7.f1=((
_tmpE6="memove",_tag_dynforward(_tmpE6,sizeof(char),_get_zero_arr_size_char(
_tmpE6,7)))),_tmpE7)))),_tmpE5)))));}memmove((void*)_check_null(
_untag_dynforward_ptr(d,sizeof(void),1)),(const void*)_check_null(
_untag_dynforward_ptr(s,sizeof(void),1)),n * sz);return d;}int Cyc_memcmp(struct
_dynforward_ptr s1,struct _dynforward_ptr s2,unsigned long n);int Cyc_memcmp(struct
_dynforward_ptr s1,struct _dynforward_ptr s2,unsigned long n){if(((s1.curr == ((
struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr  || s2.curr == ((struct
_dynforward_ptr)_tag_dynforward(0,0,0)).curr) || _get_dynforward_size(s1,sizeof(
char))>= n) || _get_dynforward_size(s2,sizeof(char))>= n){struct Cyc_Core_Invalid_argument_struct
_tmpED;const char*_tmpEC;struct Cyc_Core_Invalid_argument_struct*_tmpEB;(int)
_throw((void*)((_tmpEB=_cycalloc(sizeof(*_tmpEB)),((_tmpEB[0]=((_tmpED.tag=Cyc_Core_Invalid_argument,((
_tmpED.f1=((_tmpEC="memcmp",_tag_dynforward(_tmpEC,sizeof(char),
_get_zero_arr_size_char(_tmpEC,7)))),_tmpED)))),_tmpEB)))));}return memcmp((const
void*)_check_null(_untag_dynforward_ptr(s1,sizeof(char),1)),(const void*)
_check_null(_untag_dynforward_ptr(s2,sizeof(char),1)),n);}struct _dynforward_ptr
Cyc_memchr(struct _dynforward_ptr s,char c,unsigned long n);struct _dynforward_ptr Cyc_memchr(
struct _dynforward_ptr s,char c,unsigned long n){unsigned int sz=_get_dynforward_size(
s,sizeof(char));if(s.curr == ((struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr
 || n > sz){struct Cyc_Core_Invalid_argument_struct _tmpF3;const char*_tmpF2;struct
Cyc_Core_Invalid_argument_struct*_tmpF1;(int)_throw((void*)((_tmpF1=_cycalloc(
sizeof(*_tmpF1)),((_tmpF1[0]=((_tmpF3.tag=Cyc_Core_Invalid_argument,((_tmpF3.f1=((
_tmpF2="memchr",_tag_dynforward(_tmpF2,sizeof(char),_get_zero_arr_size_char(
_tmpF2,7)))),_tmpF3)))),_tmpF1)))));}{char*_tmp6A=memchr((const char*)_check_null(
_untag_dynforward_ptr(s,sizeof(char),1)),c,n);if(_tmp6A == 0)return(struct
_dynforward_ptr)_tag_dynforward(0,0,0);{unsigned int _tmp6B=(unsigned int)((const
char*)_check_null(_untag_dynforward_ptr(s,sizeof(char),1)));unsigned int _tmp6C=(
unsigned int)((const char*)_tmp6A);unsigned int _tmp6D=_tmp6C - _tmp6B;return
_dynforward_ptr_plus(s,sizeof(char),(int)_tmp6D);}}}struct _dynforward_ptr Cyc_mmemchr(
struct _dynforward_ptr s,char c,unsigned long n);struct _dynforward_ptr Cyc_mmemchr(
struct _dynforward_ptr s,char c,unsigned long n){unsigned int sz=_get_dynforward_size(
s,sizeof(char));if(s.curr == (_tag_dynforward(0,0,0)).curr  || n > sz){struct Cyc_Core_Invalid_argument_struct
_tmpF9;const char*_tmpF8;struct Cyc_Core_Invalid_argument_struct*_tmpF7;(int)
_throw((void*)((_tmpF7=_cycalloc(sizeof(*_tmpF7)),((_tmpF7[0]=((_tmpF9.tag=Cyc_Core_Invalid_argument,((
_tmpF9.f1=((_tmpF8="mmemchr",_tag_dynforward(_tmpF8,sizeof(char),
_get_zero_arr_size_char(_tmpF8,8)))),_tmpF9)))),_tmpF7)))));}{char*_tmp71=memchr((
const char*)_check_null(_untag_dynforward_ptr(s,sizeof(char),1)),c,n);if(_tmp71 == 
0)return _tag_dynforward(0,0,0);{unsigned int _tmp72=(unsigned int)((const char*)
_check_null(_untag_dynforward_ptr(s,sizeof(char),1)));unsigned int _tmp73=(
unsigned int)_tmp71;unsigned int _tmp74=_tmp73 - _tmp72;return _dynforward_ptr_plus(
s,sizeof(char),(int)_tmp74);}}}struct _dynforward_ptr Cyc_memset(struct
_dynforward_ptr s,char c,unsigned long n);struct _dynforward_ptr Cyc_memset(struct
_dynforward_ptr s,char c,unsigned long n){if(s.curr == (_tag_dynforward(0,0,0)).curr
 || n > _get_dynforward_size(s,sizeof(char))){struct Cyc_Core_Invalid_argument_struct
_tmpFF;const char*_tmpFE;struct Cyc_Core_Invalid_argument_struct*_tmpFD;(int)
_throw((void*)((_tmpFD=_cycalloc(sizeof(*_tmpFD)),((_tmpFD[0]=((_tmpFF.tag=Cyc_Core_Invalid_argument,((
_tmpFF.f1=((_tmpFE="memset",_tag_dynforward(_tmpFE,sizeof(char),
_get_zero_arr_size_char(_tmpFE,7)))),_tmpFF)))),_tmpFD)))));}memset((void*)((
char*)_check_null(_untag_dynforward_ptr(s,sizeof(char),1))),(int)c,n);return s;}
void Cyc_bzero(struct _dynforward_ptr s,unsigned long n);void Cyc_bzero(struct
_dynforward_ptr s,unsigned long n){if(s.curr == (_tag_dynforward(0,0,0)).curr  || 
_get_dynforward_size(s,sizeof(char))< n){struct Cyc_Core_Invalid_argument_struct
_tmp105;const char*_tmp104;struct Cyc_Core_Invalid_argument_struct*_tmp103;(int)
_throw((void*)((_tmp103=_cycalloc(sizeof(*_tmp103)),((_tmp103[0]=((_tmp105.tag=
Cyc_Core_Invalid_argument,((_tmp105.f1=((_tmp104="bzero",_tag_dynforward(_tmp104,
sizeof(char),_get_zero_arr_size_char(_tmp104,6)))),_tmp105)))),_tmp103)))));}((
void(*)(char*s,unsigned long n))bzero)((char*)_check_null(_untag_dynforward_ptr(s,
sizeof(char),1)),n);}void Cyc__bcopy(struct _dynforward_ptr src,struct
_dynforward_ptr dst,unsigned long n,unsigned int sz);void Cyc__bcopy(struct
_dynforward_ptr src,struct _dynforward_ptr dst,unsigned long n,unsigned int sz){if(((
src.curr == ((struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr  || 
_get_dynforward_size(src,sizeof(void))< n) || dst.curr == (_tag_dynforward(0,0,0)).curr)
 || _get_dynforward_size(dst,sizeof(void))< n){struct Cyc_Core_Invalid_argument_struct
_tmp10B;const char*_tmp10A;struct Cyc_Core_Invalid_argument_struct*_tmp109;(int)
_throw((void*)((_tmp109=_cycalloc(sizeof(*_tmp109)),((_tmp109[0]=((_tmp10B.tag=
Cyc_Core_Invalid_argument,((_tmp10B.f1=((_tmp10A="bcopy",_tag_dynforward(_tmp10A,
sizeof(char),_get_zero_arr_size_char(_tmp10A,6)))),_tmp10B)))),_tmp109)))));}
bcopy((const void*)_check_null(_untag_dynforward_ptr(src,sizeof(void),1)),(void*)
_check_null(_untag_dynforward_ptr(dst,sizeof(void),1)),n * sz);}
