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
void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[
16];struct Cyc_Typerep_Int_struct{int tag;int f1;unsigned int f2;};struct Cyc_Typerep_ThinPtr_struct{
int tag;unsigned int f1;void*f2;};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;}
;struct _tuple0{unsigned int f1;struct _dynforward_ptr f2;void*f3;};struct Cyc_Typerep_Struct_struct{
int tag;struct _dynforward_ptr*f1;unsigned int f2;struct _dynforward_ptr f3;};struct
_tuple1{unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;
unsigned int f1;struct _dynforward_ptr f2;};struct _tuple2{unsigned int f1;struct
_dynforward_ptr f2;};struct Cyc_Typerep_TUnion_struct{int tag;struct _dynforward_ptr
f1;struct _dynforward_ptr f2;struct _dynforward_ptr f3;};struct Cyc_Typerep_TUnionField_struct{
int tag;struct _dynforward_ptr f1;struct _dynforward_ptr f2;unsigned int f3;struct
_dynforward_ptr f4;};struct _tuple3{struct _dynforward_ptr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _dynforward_ptr f1;struct _dynforward_ptr f2;};struct Cyc_Typerep_Union_struct{
int tag;struct _dynforward_ptr*f1;int f2;struct _dynforward_ptr f3;};struct Cyc_Typerep_Enum_struct{
int tag;struct _dynforward_ptr*f1;int f2;struct _dynforward_ptr f3;};void Cyc_Typerep_print_typestruct(
void*rep);void Cyc_Typerep_xmlize_typestruct(void*rep);struct _tuple4{int f1;void*
f2;};struct _tuple4 Cyc_Typerep_get_unionbranch(unsigned int tag,struct
_dynforward_ptr l);struct _dynforward_ptr Cyc_Typerep_get_tagname(unsigned int tag,
struct _dynforward_ptr l);struct _dynforward_ptr Cyc_Typerep_get_tagname2(
unsigned int tag,struct _dynforward_ptr l);struct _tuple4 Cyc_Typerep_get_xtunionbranch(
struct _dynforward_ptr tag,struct _dynforward_ptr l);unsigned int Cyc_Typerep_size_type(
void*rep);struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;
};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct
Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dynforward_ptr f1;};int Cyc_printf(struct _dynforward_ptr,struct
_dynforward_ptr);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];
struct Cyc_FileOpenError_struct{char*tag;struct _dynforward_ptr f1;};struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[18];extern char
Cyc_List_Nth[8];struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple5{void*f1;void*f2;};struct _tuple5*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple5*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);int Cyc_strcmp(struct _dynforward_ptr s1,
struct _dynforward_ptr s2);struct _tuple4 Cyc_Typerep_tuple_tl(void*typ);static void
_tmp16D(int*pos1,unsigned int*_tmp16C,unsigned int*_tmp16B,struct _tuple1***
_tmp169,struct _dynforward_ptr*_tmp2){for(*_tmp16C=0;*_tmp16C < *_tmp16B;(*_tmp16C)
++){struct _tuple1*_tmp167;(*_tmp169)[*_tmp16C]=((_tmp167=_cycalloc(sizeof(*
_tmp167)),((_tmp167->f1=(*(*((struct _tuple1**)_check_dynforward_subscript(*_tmp2,
sizeof(struct _tuple1*),(int)(*_tmp16C + 1))))).f1 - *pos1,((_tmp167->f2=(*(*((
struct _tuple1**)_check_dynforward_subscript(*_tmp2,sizeof(struct _tuple1*),(int)(*
_tmp16C + 1))))).f2,_tmp167))))));}}struct _tuple4 Cyc_Typerep_tuple_tl(void*typ){
void*_tmp0=typ;unsigned int _tmp1;struct _dynforward_ptr _tmp2;_LL1: if(_tmp0 <= (
void*)2)goto _LL3;if(*((int*)_tmp0)!= 4)goto _LL3;_tmp1=((struct Cyc_Typerep_Tuple_struct*)
_tmp0)->f1;_tmp2=((struct Cyc_Typerep_Tuple_struct*)_tmp0)->f2;_LL2: if(
_get_dynforward_size(_tmp2,sizeof(struct _tuple1*))< 2)goto _LL4;{int pos1=(int)(*(*((
struct _tuple1**)_check_dynforward_subscript(_tmp2,sizeof(struct _tuple1*),1)))).f1;
unsigned int _tmp16C;unsigned int _tmp16B;struct _dynforward_ptr _tmp16A;struct
_tuple1**_tmp169;unsigned int _tmp168;struct _dynforward_ptr _tmp3=(_tmp168=
_get_dynforward_size(_tmp2,sizeof(struct _tuple1*))- 1,((_tmp169=(struct _tuple1**)
_cycalloc(_check_times(sizeof(struct _tuple1*),_tmp168)),((_tmp16A=
_tag_dynforward(_tmp169,sizeof(struct _tuple1*),_tmp168),((((_tmp16B=_tmp168,
_tmp16D(& pos1,& _tmp16C,& _tmp16B,& _tmp169,& _tmp2))),_tmp16A)))))));struct Cyc_Typerep_Tuple_struct*
_tmp173;struct Cyc_Typerep_Tuple_struct _tmp172;struct _tuple4 _tmp171;return(
_tmp171.f1=pos1,((_tmp171.f2=(void*)((_tmp173=_cycalloc(sizeof(*_tmp173)),((
_tmp173[0]=((_tmp172.tag=4,((_tmp172.f1=_tmp1 - pos1,((_tmp172.f2=_tmp3,_tmp172)))))),
_tmp173)))),_tmp171)));}_LL3:;_LL4: {struct Cyc_Core_Failure_struct _tmp179;const
char*_tmp178;struct Cyc_Core_Failure_struct*_tmp177;(int)_throw((void*)((_tmp177=
_cycalloc(sizeof(*_tmp177)),((_tmp177[0]=((_tmp179.tag=Cyc_Core_Failure,((
_tmp179.f1=((_tmp178="tuple_tl: expected tuple of size >= 2",_tag_dynforward(
_tmp178,sizeof(char),_get_zero_arr_size_char(_tmp178,38)))),_tmp179)))),_tmp177)))));}
_LL0:;}struct _tuple4 Cyc_Typerep_get_unionbranch(unsigned int tag,struct
_dynforward_ptr l);struct _tuple4 Cyc_Typerep_get_unionbranch(unsigned int tag,
struct _dynforward_ptr l){{int i=0;for(0;i < _get_dynforward_size(l,sizeof(struct
_tuple0*));++ i){unsigned int _tmp10;void*_tmp11;struct _tuple0 _tmpF=*(*((struct
_tuple0**)_check_dynforward_subscript(l,sizeof(struct _tuple0*),i)));_tmp10=_tmpF.f1;
_tmp11=_tmpF.f3;if(_tmp10 == tag)return Cyc_Typerep_tuple_tl(_tmp11);}}{struct Cyc_Core_Failure_struct
_tmp17F;const char*_tmp17E;struct Cyc_Core_Failure_struct*_tmp17D;(int)_throw((
void*)((_tmp17D=_cycalloc(sizeof(*_tmp17D)),((_tmp17D[0]=((_tmp17F.tag=Cyc_Core_Failure,((
_tmp17F.f1=((_tmp17E="Could not find tag in TUnion",_tag_dynforward(_tmp17E,
sizeof(char),_get_zero_arr_size_char(_tmp17E,29)))),_tmp17F)))),_tmp17D)))));}}
struct _dynforward_ptr Cyc_Typerep_get_tagname(unsigned int tag,struct
_dynforward_ptr l);struct _dynforward_ptr Cyc_Typerep_get_tagname(unsigned int tag,
struct _dynforward_ptr l){{int i=0;for(0;i < _get_dynforward_size(l,sizeof(struct
_tuple2*));++ i){unsigned int _tmp16;struct _dynforward_ptr _tmp17;struct _tuple2
_tmp15=*(*((struct _tuple2**)_check_dynforward_subscript(l,sizeof(struct _tuple2*),
i)));_tmp16=_tmp15.f1;_tmp17=_tmp15.f2;if(_tmp16 == tag)return _tmp17;}}{struct Cyc_Core_Failure_struct
_tmp185;const char*_tmp184;struct Cyc_Core_Failure_struct*_tmp183;(int)_throw((
void*)((_tmp183=_cycalloc(sizeof(*_tmp183)),((_tmp183[0]=((_tmp185.tag=Cyc_Core_Failure,((
_tmp185.f1=((_tmp184="Could not find name in TUnion",_tag_dynforward(_tmp184,
sizeof(char),_get_zero_arr_size_char(_tmp184,30)))),_tmp185)))),_tmp183)))));}}
struct _dynforward_ptr Cyc_Typerep_get_tagname2(unsigned int tag,struct
_dynforward_ptr l);struct _dynforward_ptr Cyc_Typerep_get_tagname2(unsigned int tag,
struct _dynforward_ptr l){{int i=0;for(0;i < _get_dynforward_size(l,sizeof(struct
_tuple0*));++ i){unsigned int _tmp1C;struct _dynforward_ptr _tmp1D;struct _tuple0
_tmp1B=*(*((struct _tuple0**)_check_dynforward_subscript(l,sizeof(struct _tuple0*),
i)));_tmp1C=_tmp1B.f1;_tmp1D=_tmp1B.f2;if(_tmp1C == tag)return _tmp1D;}}{struct Cyc_Core_Failure_struct
_tmp18B;const char*_tmp18A;struct Cyc_Core_Failure_struct*_tmp189;(int)_throw((
void*)((_tmp189=_cycalloc(sizeof(*_tmp189)),((_tmp189[0]=((_tmp18B.tag=Cyc_Core_Failure,((
_tmp18B.f1=((_tmp18A="Could not find name in TUnion",_tag_dynforward(_tmp18A,
sizeof(char),_get_zero_arr_size_char(_tmp18A,30)))),_tmp18B)))),_tmp189)))));}}
struct _tuple4 Cyc_Typerep_get_xtunionbranch(struct _dynforward_ptr tag,struct
_dynforward_ptr l);struct _tuple4 Cyc_Typerep_get_xtunionbranch(struct
_dynforward_ptr tag,struct _dynforward_ptr l){{int i=0;for(0;i < _get_dynforward_size(
l,sizeof(struct _tuple3*));++ i){struct _dynforward_ptr _tmp22;void*_tmp23;struct
_tuple3 _tmp21=*(*((struct _tuple3**)_check_dynforward_subscript(l,sizeof(struct
_tuple3*),i)));_tmp22=_tmp21.f1;_tmp23=_tmp21.f2;if(Cyc_strcmp((struct
_dynforward_ptr)_tmp22,(struct _dynforward_ptr)tag)== 0)return Cyc_Typerep_tuple_tl(
_tmp23);}}{struct Cyc_Core_Failure_struct _tmp191;const char*_tmp190;struct Cyc_Core_Failure_struct*
_tmp18F;(int)_throw((void*)((_tmp18F=_cycalloc(sizeof(*_tmp18F)),((_tmp18F[0]=((
_tmp191.tag=Cyc_Core_Failure,((_tmp191.f1=((_tmp190="Could not find tag in XTUnion",
_tag_dynforward(_tmp190,sizeof(char),_get_zero_arr_size_char(_tmp190,30)))),
_tmp191)))),_tmp18F)))));}}unsigned int Cyc_Typerep_size_type(void*rep);
unsigned int Cyc_Typerep_size_type(void*rep){void*_tmp27=rep;int _tmp28;
unsigned int _tmp29;unsigned int _tmp2A;unsigned int _tmp2B;unsigned int _tmp2C;int
_tmp2D;int _tmp2E;struct _dynforward_ptr _tmp2F;_LL6: if(_tmp27 <= (void*)2)goto _LL8;
if(*((int*)_tmp27)!= 0)goto _LL8;_tmp28=((struct Cyc_Typerep_Int_struct*)_tmp27)->f1;
_tmp29=((struct Cyc_Typerep_Int_struct*)_tmp27)->f2;_LL7: return _tmp29 >> 3;_LL8:
if((int)_tmp27 != 0)goto _LLA;_LL9: return sizeof(float);_LLA: if((int)_tmp27 != 1)
goto _LLC;_LLB: return sizeof(double);_LLC: if(_tmp27 <= (void*)2)goto _LLE;if(*((int*)
_tmp27)!= 1)goto _LLE;_LLD: return sizeof(void*);_LLE: if(_tmp27 <= (void*)2)goto
_LL10;if(*((int*)_tmp27)!= 2)goto _LL10;_LLF: return sizeof(struct _dynforward_ptr);
_LL10: if(_tmp27 <= (void*)2)goto _LL12;if(*((int*)_tmp27)!= 3)goto _LL12;_tmp2A=((
struct Cyc_Typerep_Struct_struct*)_tmp27)->f2;_LL11: return _tmp2A;_LL12: if(_tmp27
<= (void*)2)goto _LL14;if(*((int*)_tmp27)!= 4)goto _LL14;_tmp2B=((struct Cyc_Typerep_Tuple_struct*)
_tmp27)->f1;_LL13: return _tmp2B;_LL14: if(_tmp27 <= (void*)2)goto _LL16;if(*((int*)
_tmp27)!= 6)goto _LL16;_tmp2C=((struct Cyc_Typerep_TUnionField_struct*)_tmp27)->f3;
_LL15: return _tmp2C;_LL16: if(_tmp27 <= (void*)2)goto _LL18;if(*((int*)_tmp27)!= 5)
goto _LL18;_LL17: return sizeof(void*);_LL18: if(_tmp27 <= (void*)2)goto _LL1A;if(*((
int*)_tmp27)!= 7)goto _LL1A;_LL19: return sizeof(void*);_LL1A: if(_tmp27 <= (void*)2)
goto _LL1C;if(*((int*)_tmp27)!= 8)goto _LL1C;_tmp2D=((struct Cyc_Typerep_Union_struct*)
_tmp27)->f2;_LL1B: return(unsigned int)_tmp2D;_LL1C: if(_tmp27 <= (void*)2)goto _LL5;
if(*((int*)_tmp27)!= 9)goto _LL5;_tmp2E=((struct Cyc_Typerep_Enum_struct*)_tmp27)->f2;
_tmp2F=((struct Cyc_Typerep_Enum_struct*)_tmp27)->f3;_LL1D: return(unsigned int)
_tmp2E;_LL5:;}struct _dynforward_ptr Cyc_Typerep_anon_or_name(struct
_dynforward_ptr*name);struct _dynforward_ptr Cyc_Typerep_anon_or_name(struct
_dynforward_ptr*name){const char*_tmp192;return name == 0?(_tmp192="(anon)",
_tag_dynforward(_tmp192,sizeof(char),_get_zero_arr_size_char(_tmp192,7))):*name;}
void Cyc_Typerep_print_typestruct(void*rep);void Cyc_Typerep_print_typestruct(void*
rep){void*_tmp31=rep;int _tmp32;unsigned int _tmp33;unsigned int _tmp34;void*_tmp35;
void*_tmp36;struct _dynforward_ptr*_tmp37;unsigned int _tmp38;struct
_dynforward_ptr _tmp39;unsigned int _tmp3A;struct _dynforward_ptr _tmp3B;struct
_dynforward_ptr _tmp3C;struct _dynforward_ptr _tmp3D;unsigned int _tmp3E;struct
_dynforward_ptr _tmp3F;struct _dynforward_ptr _tmp40;struct _dynforward_ptr _tmp41;
struct _dynforward_ptr _tmp42;struct _dynforward_ptr _tmp43;struct _dynforward_ptr
_tmp44;struct _dynforward_ptr*_tmp45;int _tmp46;struct _dynforward_ptr _tmp47;struct
_dynforward_ptr*_tmp48;int _tmp49;struct _dynforward_ptr _tmp4A;_LL1F: if(_tmp31 <= (
void*)2)goto _LL21;if(*((int*)_tmp31)!= 0)goto _LL21;_tmp32=((struct Cyc_Typerep_Int_struct*)
_tmp31)->f1;_tmp33=((struct Cyc_Typerep_Int_struct*)_tmp31)->f2;_LL20:{const char*
_tmp19B;const char*_tmp19A;const char*_tmp199;void*_tmp198[2];struct Cyc_String_pa_struct
_tmp197;struct Cyc_Int_pa_struct _tmp196;(_tmp196.tag=1,((_tmp196.f1=(
unsigned long)((int)_tmp33),((_tmp197.tag=0,((_tmp197.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)(_tmp32?(struct _dynforward_ptr)((_tmp19A="signed",
_tag_dynforward(_tmp19A,sizeof(char),_get_zero_arr_size_char(_tmp19A,7)))):(
struct _dynforward_ptr)((_tmp19B="unsigned",_tag_dynforward(_tmp19B,sizeof(char),
_get_zero_arr_size_char(_tmp19B,9)))))),((_tmp198[0]=& _tmp197,((_tmp198[1]=&
_tmp196,Cyc_printf(((_tmp199="Int(%s,%d)",_tag_dynforward(_tmp199,sizeof(char),
_get_zero_arr_size_char(_tmp199,11)))),_tag_dynforward(_tmp198,sizeof(void*),2)))))))))))));}
goto _LL1E;_LL21: if((int)_tmp31 != 0)goto _LL23;_LL22:{const char*_tmp19E;void*
_tmp19D;(_tmp19D=0,Cyc_printf(((_tmp19E="Float",_tag_dynforward(_tmp19E,sizeof(
char),_get_zero_arr_size_char(_tmp19E,6)))),_tag_dynforward(_tmp19D,sizeof(void*),
0)));}goto _LL1E;_LL23: if((int)_tmp31 != 1)goto _LL25;_LL24:{const char*_tmp1A1;void*
_tmp1A0;(_tmp1A0=0,Cyc_printf(((_tmp1A1="Double",_tag_dynforward(_tmp1A1,sizeof(
char),_get_zero_arr_size_char(_tmp1A1,7)))),_tag_dynforward(_tmp1A0,sizeof(void*),
0)));}goto _LL1E;_LL25: if(_tmp31 <= (void*)2)goto _LL27;if(*((int*)_tmp31)!= 1)goto
_LL27;_tmp34=((struct Cyc_Typerep_ThinPtr_struct*)_tmp31)->f1;_tmp35=(void*)((
struct Cyc_Typerep_ThinPtr_struct*)_tmp31)->f2;_LL26:{const char*_tmp1A5;void*
_tmp1A4[1];struct Cyc_Int_pa_struct _tmp1A3;(_tmp1A3.tag=1,((_tmp1A3.f1=(
unsigned long)((int)_tmp34),((_tmp1A4[0]=& _tmp1A3,Cyc_printf(((_tmp1A5="ThinPtr(%d,",
_tag_dynforward(_tmp1A5,sizeof(char),_get_zero_arr_size_char(_tmp1A5,12)))),
_tag_dynforward(_tmp1A4,sizeof(void*),1)))))));}Cyc_Typerep_print_typestruct(
_tmp35);{const char*_tmp1A8;void*_tmp1A7;(_tmp1A7=0,Cyc_printf(((_tmp1A8=")",
_tag_dynforward(_tmp1A8,sizeof(char),_get_zero_arr_size_char(_tmp1A8,2)))),
_tag_dynforward(_tmp1A7,sizeof(void*),0)));}goto _LL1E;_LL27: if(_tmp31 <= (void*)2)
goto _LL29;if(*((int*)_tmp31)!= 2)goto _LL29;_tmp36=(void*)((struct Cyc_Typerep_FatPtr_struct*)
_tmp31)->f1;_LL28:{const char*_tmp1AB;void*_tmp1AA;(_tmp1AA=0,Cyc_printf(((
_tmp1AB="FatPtr(",_tag_dynforward(_tmp1AB,sizeof(char),_get_zero_arr_size_char(
_tmp1AB,8)))),_tag_dynforward(_tmp1AA,sizeof(void*),0)));}Cyc_Typerep_print_typestruct(
_tmp36);{const char*_tmp1AE;void*_tmp1AD;(_tmp1AD=0,Cyc_printf(((_tmp1AE=")",
_tag_dynforward(_tmp1AE,sizeof(char),_get_zero_arr_size_char(_tmp1AE,2)))),
_tag_dynforward(_tmp1AD,sizeof(void*),0)));}goto _LL1E;_LL29: if(_tmp31 <= (void*)2)
goto _LL2B;if(*((int*)_tmp31)!= 3)goto _LL2B;_tmp37=((struct Cyc_Typerep_Struct_struct*)
_tmp31)->f1;_tmp38=((struct Cyc_Typerep_Struct_struct*)_tmp31)->f2;_tmp39=((
struct Cyc_Typerep_Struct_struct*)_tmp31)->f3;_LL2A:{const char*_tmp1B3;void*
_tmp1B2[2];struct Cyc_String_pa_struct _tmp1B1;struct Cyc_Int_pa_struct _tmp1B0;(
_tmp1B0.tag=1,((_tmp1B0.f1=(unsigned long)((int)_tmp38),((_tmp1B1.tag=0,((
_tmp1B1.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Typerep_anon_or_name(
_tmp37)),((_tmp1B2[0]=& _tmp1B1,((_tmp1B2[1]=& _tmp1B0,Cyc_printf(((_tmp1B3="Struct(%s,%d,[",
_tag_dynforward(_tmp1B3,sizeof(char),_get_zero_arr_size_char(_tmp1B3,15)))),
_tag_dynforward(_tmp1B2,sizeof(void*),2)))))))))))));}{int i=0;for(0;i < 
_get_dynforward_size(_tmp39,sizeof(struct _tuple0*));++ i){unsigned int _tmp63;
struct _dynforward_ptr _tmp64;void*_tmp65;struct _tuple0 _tmp62=*(*((struct _tuple0**)
_check_dynforward_subscript(_tmp39,sizeof(struct _tuple0*),i)));_tmp63=_tmp62.f1;
_tmp64=_tmp62.f2;_tmp65=_tmp62.f3;{const char*_tmp1B8;void*_tmp1B7[2];struct Cyc_Int_pa_struct
_tmp1B6;struct Cyc_String_pa_struct _tmp1B5;(_tmp1B5.tag=0,((_tmp1B5.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp64),((_tmp1B6.tag=1,((_tmp1B6.f1=(
unsigned long)((int)_tmp63),((_tmp1B7[0]=& _tmp1B6,((_tmp1B7[1]=& _tmp1B5,Cyc_printf(((
_tmp1B8="$(%d,%s,",_tag_dynforward(_tmp1B8,sizeof(char),_get_zero_arr_size_char(
_tmp1B8,9)))),_tag_dynforward(_tmp1B7,sizeof(void*),2)))))))))))));}Cyc_Typerep_print_typestruct(
_tmp65);{const char*_tmp1BC;void*_tmp1BB[1];struct Cyc_Int_pa_struct _tmp1BA;(
_tmp1BA.tag=1,((_tmp1BA.f1=(unsigned long)((int)(i != _get_dynforward_size(_tmp39,
sizeof(struct _tuple0*))- 1?',':' ')),((_tmp1BB[0]=& _tmp1BA,Cyc_printf(((_tmp1BC=")%c",
_tag_dynforward(_tmp1BC,sizeof(char),_get_zero_arr_size_char(_tmp1BC,4)))),
_tag_dynforward(_tmp1BB,sizeof(void*),1)))))));}}}{const char*_tmp1BF;void*
_tmp1BE;(_tmp1BE=0,Cyc_printf(((_tmp1BF=" ]",_tag_dynforward(_tmp1BF,sizeof(char),
_get_zero_arr_size_char(_tmp1BF,3)))),_tag_dynforward(_tmp1BE,sizeof(void*),0)));}
goto _LL1E;_LL2B: if(_tmp31 <= (void*)2)goto _LL2D;if(*((int*)_tmp31)!= 4)goto _LL2D;
_tmp3A=((struct Cyc_Typerep_Tuple_struct*)_tmp31)->f1;_tmp3B=((struct Cyc_Typerep_Tuple_struct*)
_tmp31)->f2;_LL2C:{const char*_tmp1C3;void*_tmp1C2[1];struct Cyc_Int_pa_struct
_tmp1C1;(_tmp1C1.tag=1,((_tmp1C1.f1=(unsigned long)((int)_tmp3A),((_tmp1C2[0]=&
_tmp1C1,Cyc_printf(((_tmp1C3="Tuple(%d,[",_tag_dynforward(_tmp1C3,sizeof(char),
_get_zero_arr_size_char(_tmp1C3,11)))),_tag_dynforward(_tmp1C2,sizeof(void*),1)))))));}{
int i=0;for(0;i < _get_dynforward_size(_tmp3B,sizeof(struct _tuple1*));++ i){
unsigned int _tmp73;void*_tmp74;struct _tuple1 _tmp72=*(*((struct _tuple1**)
_check_dynforward_subscript(_tmp3B,sizeof(struct _tuple1*),i)));_tmp73=_tmp72.f1;
_tmp74=_tmp72.f2;{const char*_tmp1C7;void*_tmp1C6[1];struct Cyc_Int_pa_struct
_tmp1C5;(_tmp1C5.tag=1,((_tmp1C5.f1=(unsigned long)((int)_tmp73),((_tmp1C6[0]=&
_tmp1C5,Cyc_printf(((_tmp1C7="$(%d,",_tag_dynforward(_tmp1C7,sizeof(char),
_get_zero_arr_size_char(_tmp1C7,6)))),_tag_dynforward(_tmp1C6,sizeof(void*),1)))))));}
Cyc_Typerep_print_typestruct(_tmp74);{const char*_tmp1CB;void*_tmp1CA[1];struct
Cyc_Int_pa_struct _tmp1C9;(_tmp1C9.tag=1,((_tmp1C9.f1=(unsigned long)((int)(i != 
_get_dynforward_size(_tmp3B,sizeof(struct _tuple1*))- 1?',':' ')),((_tmp1CA[0]=&
_tmp1C9,Cyc_printf(((_tmp1CB=")%c",_tag_dynforward(_tmp1CB,sizeof(char),
_get_zero_arr_size_char(_tmp1CB,4)))),_tag_dynforward(_tmp1CA,sizeof(void*),1)))))));}}}{
const char*_tmp1CE;void*_tmp1CD;(_tmp1CD=0,Cyc_printf(((_tmp1CE=" ]",
_tag_dynforward(_tmp1CE,sizeof(char),_get_zero_arr_size_char(_tmp1CE,3)))),
_tag_dynforward(_tmp1CD,sizeof(void*),0)));}goto _LL1E;_LL2D: if(_tmp31 <= (void*)2)
goto _LL2F;if(*((int*)_tmp31)!= 6)goto _LL2F;_tmp3C=((struct Cyc_Typerep_TUnionField_struct*)
_tmp31)->f1;_tmp3D=((struct Cyc_Typerep_TUnionField_struct*)_tmp31)->f2;_tmp3E=((
struct Cyc_Typerep_TUnionField_struct*)_tmp31)->f3;_tmp3F=((struct Cyc_Typerep_TUnionField_struct*)
_tmp31)->f4;_LL2E:{const char*_tmp1D4;void*_tmp1D3[3];struct Cyc_String_pa_struct
_tmp1D2;struct Cyc_String_pa_struct _tmp1D1;struct Cyc_Int_pa_struct _tmp1D0;(
_tmp1D0.tag=1,((_tmp1D0.f1=(unsigned long)((int)_tmp3E),((_tmp1D1.tag=0,((
_tmp1D1.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmp3D),((_tmp1D2.tag=
0,((_tmp1D2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmp3C),((_tmp1D3[
0]=& _tmp1D2,((_tmp1D3[1]=& _tmp1D1,((_tmp1D3[2]=& _tmp1D0,Cyc_printf(((_tmp1D4="TUnionField(%s,%s,%d,[",
_tag_dynforward(_tmp1D4,sizeof(char),_get_zero_arr_size_char(_tmp1D4,23)))),
_tag_dynforward(_tmp1D3,sizeof(void*),3)))))))))))))))))));}{int i=0;for(0;i < 
_get_dynforward_size(_tmp3F,sizeof(struct _tuple1*));++ i){unsigned int _tmp83;void*
_tmp84;struct _tuple1 _tmp82=*(*((struct _tuple1**)_check_dynforward_subscript(
_tmp3F,sizeof(struct _tuple1*),i)));_tmp83=_tmp82.f1;_tmp84=_tmp82.f2;{const char*
_tmp1D8;void*_tmp1D7[1];struct Cyc_Int_pa_struct _tmp1D6;(_tmp1D6.tag=1,((_tmp1D6.f1=(
unsigned long)((int)_tmp83),((_tmp1D7[0]=& _tmp1D6,Cyc_printf(((_tmp1D8="$(%d,",
_tag_dynforward(_tmp1D8,sizeof(char),_get_zero_arr_size_char(_tmp1D8,6)))),
_tag_dynforward(_tmp1D7,sizeof(void*),1)))))));}Cyc_Typerep_print_typestruct(
_tmp84);{const char*_tmp1DC;void*_tmp1DB[1];struct Cyc_Int_pa_struct _tmp1DA;(
_tmp1DA.tag=1,((_tmp1DA.f1=(unsigned long)((int)(i != _get_dynforward_size(_tmp3F,
sizeof(struct _tuple1*))- 1?',':' ')),((_tmp1DB[0]=& _tmp1DA,Cyc_printf(((_tmp1DC=")%c",
_tag_dynforward(_tmp1DC,sizeof(char),_get_zero_arr_size_char(_tmp1DC,4)))),
_tag_dynforward(_tmp1DB,sizeof(void*),1)))))));}}}{const char*_tmp1DF;void*
_tmp1DE;(_tmp1DE=0,Cyc_printf(((_tmp1DF=" ]",_tag_dynforward(_tmp1DF,sizeof(char),
_get_zero_arr_size_char(_tmp1DF,3)))),_tag_dynforward(_tmp1DE,sizeof(void*),0)));}
goto _LL1E;_LL2F: if(_tmp31 <= (void*)2)goto _LL31;if(*((int*)_tmp31)!= 5)goto _LL31;
_tmp40=((struct Cyc_Typerep_TUnion_struct*)_tmp31)->f1;_tmp41=((struct Cyc_Typerep_TUnion_struct*)
_tmp31)->f2;_tmp42=((struct Cyc_Typerep_TUnion_struct*)_tmp31)->f3;_LL30:{const
char*_tmp1E3;void*_tmp1E2[1];struct Cyc_String_pa_struct _tmp1E1;(_tmp1E1.tag=0,((
_tmp1E1.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmp40),((_tmp1E2[0]=&
_tmp1E1,Cyc_printf(((_tmp1E3="TUnion(%s,[",_tag_dynforward(_tmp1E3,sizeof(char),
_get_zero_arr_size_char(_tmp1E3,12)))),_tag_dynforward(_tmp1E2,sizeof(void*),1)))))));}{
int i=0;for(0;i < _get_dynforward_size(_tmp41,sizeof(struct _tuple2*));++ i){
unsigned int _tmp91;struct _dynforward_ptr _tmp92;struct _tuple2 _tmp90=*(*((struct
_tuple2**)_check_dynforward_subscript(_tmp41,sizeof(struct _tuple2*),i)));_tmp91=
_tmp90.f1;_tmp92=_tmp90.f2;{const char*_tmp1E9;void*_tmp1E8[3];struct Cyc_Int_pa_struct
_tmp1E7;struct Cyc_String_pa_struct _tmp1E6;struct Cyc_Int_pa_struct _tmp1E5;(
_tmp1E5.tag=1,((_tmp1E5.f1=(unsigned long)((int)(i != _get_dynforward_size(_tmp41,
sizeof(struct _tuple2*))- 1?',':' ')),((_tmp1E6.tag=0,((_tmp1E6.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp92),((_tmp1E7.tag=1,((_tmp1E7.f1=(
unsigned long)((int)_tmp91),((_tmp1E8[0]=& _tmp1E7,((_tmp1E8[1]=& _tmp1E6,((
_tmp1E8[2]=& _tmp1E5,Cyc_printf(((_tmp1E9="$(%d,%s)%c",_tag_dynforward(_tmp1E9,
sizeof(char),_get_zero_arr_size_char(_tmp1E9,11)))),_tag_dynforward(_tmp1E8,
sizeof(void*),3)))))))))))))))))));}}}{const char*_tmp1EC;void*_tmp1EB;(_tmp1EB=0,
Cyc_printf(((_tmp1EC="],[",_tag_dynforward(_tmp1EC,sizeof(char),
_get_zero_arr_size_char(_tmp1EC,4)))),_tag_dynforward(_tmp1EB,sizeof(void*),0)));}{
int i=0;for(0;i < _get_dynforward_size(_tmp42,sizeof(struct _tuple0*));++ i){
unsigned int _tmp9B;struct _dynforward_ptr _tmp9C;void*_tmp9D;struct _tuple0 _tmp9A=*(*((
struct _tuple0**)_check_dynforward_subscript(_tmp42,sizeof(struct _tuple0*),i)));
_tmp9B=_tmp9A.f1;_tmp9C=_tmp9A.f2;_tmp9D=_tmp9A.f3;{const char*_tmp1F1;void*
_tmp1F0[2];struct Cyc_Int_pa_struct _tmp1EF;struct Cyc_String_pa_struct _tmp1EE;(
_tmp1EE.tag=0,((_tmp1EE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmp9C),((
_tmp1EF.tag=1,((_tmp1EF.f1=(unsigned long)((int)_tmp9B),((_tmp1F0[0]=& _tmp1EF,((
_tmp1F0[1]=& _tmp1EE,Cyc_printf(((_tmp1F1="$(%d,%s,",_tag_dynforward(_tmp1F1,
sizeof(char),_get_zero_arr_size_char(_tmp1F1,9)))),_tag_dynforward(_tmp1F0,
sizeof(void*),2)))))))))))));}Cyc_Typerep_print_typestruct(_tmp9D);{const char*
_tmp1F5;void*_tmp1F4[1];struct Cyc_Int_pa_struct _tmp1F3;(_tmp1F3.tag=1,((_tmp1F3.f1=(
unsigned long)((int)(i != _get_dynforward_size(_tmp42,sizeof(struct _tuple0*))- 1?',':' ')),((
_tmp1F4[0]=& _tmp1F3,Cyc_printf(((_tmp1F5=")%c",_tag_dynforward(_tmp1F5,sizeof(
char),_get_zero_arr_size_char(_tmp1F5,4)))),_tag_dynforward(_tmp1F4,sizeof(void*),
1)))))));}}}{const char*_tmp1F8;void*_tmp1F7;(_tmp1F7=0,Cyc_printf(((_tmp1F8="] )",
_tag_dynforward(_tmp1F8,sizeof(char),_get_zero_arr_size_char(_tmp1F8,4)))),
_tag_dynforward(_tmp1F7,sizeof(void*),0)));}goto _LL1E;_LL31: if(_tmp31 <= (void*)2)
goto _LL33;if(*((int*)_tmp31)!= 7)goto _LL33;_tmp43=((struct Cyc_Typerep_XTUnion_struct*)
_tmp31)->f1;_tmp44=((struct Cyc_Typerep_XTUnion_struct*)_tmp31)->f2;_LL32:{const
char*_tmp1FC;void*_tmp1FB[1];struct Cyc_String_pa_struct _tmp1FA;(_tmp1FA.tag=0,((
_tmp1FA.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmp43),((_tmp1FB[0]=&
_tmp1FA,Cyc_printf(((_tmp1FC="XTUnion(%s,",_tag_dynforward(_tmp1FC,sizeof(char),
_get_zero_arr_size_char(_tmp1FC,12)))),_tag_dynforward(_tmp1FB,sizeof(void*),1)))))));}{
int i=0;for(0;i < _get_dynforward_size(_tmp44,sizeof(struct _tuple3*));++ i){struct
_dynforward_ptr _tmpAB;void*_tmpAC;struct _tuple3 _tmpAA=*(*((struct _tuple3**)
_check_dynforward_subscript(_tmp44,sizeof(struct _tuple3*),i)));_tmpAB=_tmpAA.f1;
_tmpAC=_tmpAA.f2;{const char*_tmp200;void*_tmp1FF[1];struct Cyc_String_pa_struct
_tmp1FE;(_tmp1FE.tag=0,((_tmp1FE.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)_dynforward_ptr_plus(_tmpAB,sizeof(char),4)),((_tmp1FF[0]=&
_tmp1FE,Cyc_printf(((_tmp200="$(%s,",_tag_dynforward(_tmp200,sizeof(char),
_get_zero_arr_size_char(_tmp200,6)))),_tag_dynforward(_tmp1FF,sizeof(void*),1)))))));}
Cyc_Typerep_print_typestruct(_tmpAC);{const char*_tmp204;void*_tmp203[1];struct
Cyc_Int_pa_struct _tmp202;(_tmp202.tag=1,((_tmp202.f1=(unsigned long)((int)(i != 
_get_dynforward_size(_tmp44,sizeof(struct _tuple3*))- 1?',':' ')),((_tmp203[0]=&
_tmp202,Cyc_printf(((_tmp204=")%c",_tag_dynforward(_tmp204,sizeof(char),
_get_zero_arr_size_char(_tmp204,4)))),_tag_dynforward(_tmp203,sizeof(void*),1)))))));}}}{
const char*_tmp207;void*_tmp206;(_tmp206=0,Cyc_printf(((_tmp207=" )",
_tag_dynforward(_tmp207,sizeof(char),_get_zero_arr_size_char(_tmp207,3)))),
_tag_dynforward(_tmp206,sizeof(void*),0)));}goto _LL1E;_LL33: if(_tmp31 <= (void*)2)
goto _LL35;if(*((int*)_tmp31)!= 8)goto _LL35;_tmp45=((struct Cyc_Typerep_Union_struct*)
_tmp31)->f1;_tmp46=((struct Cyc_Typerep_Union_struct*)_tmp31)->f2;_tmp47=((struct
Cyc_Typerep_Union_struct*)_tmp31)->f3;_LL34:{const char*_tmp20C;void*_tmp20B[2];
struct Cyc_String_pa_struct _tmp20A;struct Cyc_Int_pa_struct _tmp209;(_tmp209.tag=1,((
_tmp209.f1=(unsigned long)_tmp46,((_tmp20A.tag=0,((_tmp20A.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Typerep_anon_or_name(_tmp45)),((
_tmp20B[0]=& _tmp20A,((_tmp20B[1]=& _tmp209,Cyc_printf(((_tmp20C="Union(%s,%d,",
_tag_dynforward(_tmp20C,sizeof(char),_get_zero_arr_size_char(_tmp20C,13)))),
_tag_dynforward(_tmp20B,sizeof(void*),2)))))))))))));}{int i=0;for(0;i < 
_get_dynforward_size(_tmp47,sizeof(struct _tuple3*));++ i){struct _dynforward_ptr
_tmpBA;void*_tmpBB;struct _tuple3 _tmpB9=*(*((struct _tuple3**)
_check_dynforward_subscript(_tmp47,sizeof(struct _tuple3*),i)));_tmpBA=_tmpB9.f1;
_tmpBB=_tmpB9.f2;{const char*_tmp210;void*_tmp20F[1];struct Cyc_String_pa_struct
_tmp20E;(_tmp20E.tag=0,((_tmp20E.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)_tmpBA),((_tmp20F[0]=& _tmp20E,Cyc_printf(((_tmp210="$(%s,",
_tag_dynforward(_tmp210,sizeof(char),_get_zero_arr_size_char(_tmp210,6)))),
_tag_dynforward(_tmp20F,sizeof(void*),1)))))));}Cyc_Typerep_print_typestruct(
_tmpBB);{const char*_tmp214;void*_tmp213[1];struct Cyc_Int_pa_struct _tmp212;(
_tmp212.tag=1,((_tmp212.f1=(unsigned long)((int)(i != _get_dynforward_size(_tmp47,
sizeof(struct _tuple3*))- 1?',':' ')),((_tmp213[0]=& _tmp212,Cyc_printf(((_tmp214=")%c",
_tag_dynforward(_tmp214,sizeof(char),_get_zero_arr_size_char(_tmp214,4)))),
_tag_dynforward(_tmp213,sizeof(void*),1)))))));}}}{const char*_tmp217;void*
_tmp216;(_tmp216=0,Cyc_printf(((_tmp217=")",_tag_dynforward(_tmp217,sizeof(char),
_get_zero_arr_size_char(_tmp217,2)))),_tag_dynforward(_tmp216,sizeof(void*),0)));}
goto _LL1E;_LL35: if(_tmp31 <= (void*)2)goto _LL1E;if(*((int*)_tmp31)!= 9)goto _LL1E;
_tmp48=((struct Cyc_Typerep_Enum_struct*)_tmp31)->f1;_tmp49=((struct Cyc_Typerep_Enum_struct*)
_tmp31)->f2;_tmp4A=((struct Cyc_Typerep_Enum_struct*)_tmp31)->f3;_LL36:{const char*
_tmp21C;void*_tmp21B[2];struct Cyc_String_pa_struct _tmp21A;struct Cyc_Int_pa_struct
_tmp219;(_tmp219.tag=1,((_tmp219.f1=(unsigned long)_tmp49,((_tmp21A.tag=0,((
_tmp21A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Typerep_anon_or_name(
_tmp48)),((_tmp21B[0]=& _tmp21A,((_tmp21B[1]=& _tmp219,Cyc_printf(((_tmp21C="Enum(%s,%d,",
_tag_dynforward(_tmp21C,sizeof(char),_get_zero_arr_size_char(_tmp21C,12)))),
_tag_dynforward(_tmp21B,sizeof(void*),2)))))))))))));}{int i=0;for(0;i < 
_get_dynforward_size(_tmp4A,sizeof(struct _tuple2*));++ i){unsigned int _tmpC9;
struct _dynforward_ptr _tmpCA;struct _tuple2 _tmpC8=*(*((struct _tuple2**)
_check_dynforward_subscript(_tmp4A,sizeof(struct _tuple2*),i)));_tmpC9=_tmpC8.f1;
_tmpCA=_tmpC8.f2;{const char*_tmp222;void*_tmp221[3];struct Cyc_Int_pa_struct
_tmp220;struct Cyc_String_pa_struct _tmp21F;struct Cyc_Int_pa_struct _tmp21E;(
_tmp21E.tag=1,((_tmp21E.f1=(unsigned long)((int)(i != _get_dynforward_size(_tmp4A,
sizeof(struct _tuple2*))- 1?',':' ')),((_tmp21F.tag=0,((_tmp21F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmpCA),((_tmp220.tag=1,((_tmp220.f1=(
unsigned long)((int)_tmpC9),((_tmp221[0]=& _tmp220,((_tmp221[1]=& _tmp21F,((
_tmp221[2]=& _tmp21E,Cyc_printf(((_tmp222="$(%d,%s)%c",_tag_dynforward(_tmp222,
sizeof(char),_get_zero_arr_size_char(_tmp222,11)))),_tag_dynforward(_tmp221,
sizeof(void*),3)))))))))))))))))));}}}{const char*_tmp225;void*_tmp224;(_tmp224=0,
Cyc_printf(((_tmp225=")",_tag_dynforward(_tmp225,sizeof(char),
_get_zero_arr_size_char(_tmp225,2)))),_tag_dynforward(_tmp224,sizeof(void*),0)));}
goto _LL1E;_LL1E:;}void Cyc_Typerep_xmlize_typestruct(void*rep);void Cyc_Typerep_xmlize_typestruct(
void*rep){void*_tmpD2=rep;int _tmpD3;unsigned int _tmpD4;unsigned int _tmpD5;void*
_tmpD6;void*_tmpD7;struct _dynforward_ptr*_tmpD8;unsigned int _tmpD9;struct
_dynforward_ptr _tmpDA;unsigned int _tmpDB;struct _dynforward_ptr _tmpDC;struct
_dynforward_ptr _tmpDD;struct _dynforward_ptr _tmpDE;unsigned int _tmpDF;struct
_dynforward_ptr _tmpE0;struct _dynforward_ptr _tmpE1;struct _dynforward_ptr _tmpE2;
struct _dynforward_ptr _tmpE3;struct _dynforward_ptr _tmpE4;struct _dynforward_ptr
_tmpE5;struct _dynforward_ptr*_tmpE6;int _tmpE7;struct _dynforward_ptr _tmpE8;struct
_dynforward_ptr*_tmpE9;int _tmpEA;struct _dynforward_ptr _tmpEB;_LL38: if(_tmpD2 <= (
void*)2)goto _LL3A;if(*((int*)_tmpD2)!= 0)goto _LL3A;_tmpD3=((struct Cyc_Typerep_Int_struct*)
_tmpD2)->f1;_tmpD4=((struct Cyc_Typerep_Int_struct*)_tmpD2)->f2;_LL39:{const char*
_tmp22A;void*_tmp229[2];struct Cyc_Int_pa_struct _tmp228;struct Cyc_Int_pa_struct
_tmp227;(_tmp227.tag=1,((_tmp227.f1=(unsigned long)((int)_tmpD4),((_tmp228.tag=1,((
_tmp228.f1=(unsigned long)_tmpD3,((_tmp229[0]=& _tmp228,((_tmp229[1]=& _tmp227,Cyc_printf(((
_tmp22A="<Int sn=%d sz=\"%d\"/>",_tag_dynforward(_tmp22A,sizeof(char),
_get_zero_arr_size_char(_tmp22A,21)))),_tag_dynforward(_tmp229,sizeof(void*),2)))))))))))));}
goto _LL37;_LL3A: if((int)_tmpD2 != 0)goto _LL3C;_LL3B:{const char*_tmp22D;void*
_tmp22C;(_tmp22C=0,Cyc_printf(((_tmp22D="<Float/>",_tag_dynforward(_tmp22D,
sizeof(char),_get_zero_arr_size_char(_tmp22D,9)))),_tag_dynforward(_tmp22C,
sizeof(void*),0)));}goto _LL37;_LL3C: if((int)_tmpD2 != 1)goto _LL3E;_LL3D:{const
char*_tmp230;void*_tmp22F;(_tmp22F=0,Cyc_printf(((_tmp230="<Double/>",
_tag_dynforward(_tmp230,sizeof(char),_get_zero_arr_size_char(_tmp230,10)))),
_tag_dynforward(_tmp22F,sizeof(void*),0)));}goto _LL37;_LL3E: if(_tmpD2 <= (void*)2)
goto _LL40;if(*((int*)_tmpD2)!= 1)goto _LL40;_tmpD5=((struct Cyc_Typerep_ThinPtr_struct*)
_tmpD2)->f1;_tmpD6=(void*)((struct Cyc_Typerep_ThinPtr_struct*)_tmpD2)->f2;_LL3F:{
const char*_tmp234;void*_tmp233[1];struct Cyc_Int_pa_struct _tmp232;(_tmp232.tag=1,((
_tmp232.f1=(unsigned long)((int)_tmpD5),((_tmp233[0]=& _tmp232,Cyc_printf(((
_tmp234="<ThinPtr sz=\"%d\">",_tag_dynforward(_tmp234,sizeof(char),
_get_zero_arr_size_char(_tmp234,18)))),_tag_dynforward(_tmp233,sizeof(void*),1)))))));}
Cyc_Typerep_xmlize_typestruct(_tmpD6);{const char*_tmp237;void*_tmp236;(_tmp236=0,
Cyc_printf(((_tmp237="</ThinPtr",_tag_dynforward(_tmp237,sizeof(char),
_get_zero_arr_size_char(_tmp237,10)))),_tag_dynforward(_tmp236,sizeof(void*),0)));}
goto _LL37;_LL40: if(_tmpD2 <= (void*)2)goto _LL42;if(*((int*)_tmpD2)!= 2)goto _LL42;
_tmpD7=(void*)((struct Cyc_Typerep_FatPtr_struct*)_tmpD2)->f1;_LL41:{const char*
_tmp23A;void*_tmp239;(_tmp239=0,Cyc_printf(((_tmp23A="<FatPtr>",_tag_dynforward(
_tmp23A,sizeof(char),_get_zero_arr_size_char(_tmp23A,9)))),_tag_dynforward(
_tmp239,sizeof(void*),0)));}Cyc_Typerep_xmlize_typestruct(_tmpD7);{const char*
_tmp23D;void*_tmp23C;(_tmp23C=0,Cyc_printf(((_tmp23D="</FatPtr>",_tag_dynforward(
_tmp23D,sizeof(char),_get_zero_arr_size_char(_tmp23D,10)))),_tag_dynforward(
_tmp23C,sizeof(void*),0)));}goto _LL37;_LL42: if(_tmpD2 <= (void*)2)goto _LL44;if(*((
int*)_tmpD2)!= 3)goto _LL44;_tmpD8=((struct Cyc_Typerep_Struct_struct*)_tmpD2)->f1;
_tmpD9=((struct Cyc_Typerep_Struct_struct*)_tmpD2)->f2;_tmpDA=((struct Cyc_Typerep_Struct_struct*)
_tmpD2)->f3;_LL43:{const char*_tmp242;void*_tmp241[2];struct Cyc_String_pa_struct
_tmp240;struct Cyc_Int_pa_struct _tmp23F;(_tmp23F.tag=1,((_tmp23F.f1=(
unsigned long)((int)_tmpD9),((_tmp240.tag=0,((_tmp240.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Typerep_anon_or_name(_tmpD8)),((_tmp241[0]=& _tmp240,((
_tmp241[1]=& _tmp23F,Cyc_printf(((_tmp242="<Struct name=\"%s\" sz=\"%d\">",
_tag_dynforward(_tmp242,sizeof(char),_get_zero_arr_size_char(_tmp242,27)))),
_tag_dynforward(_tmp241,sizeof(void*),2)))))))))))));}{int i=0;for(0;i < 
_get_dynforward_size(_tmpDA,sizeof(struct _tuple0*));++ i){unsigned int _tmp102;
struct _dynforward_ptr _tmp103;void*_tmp104;struct _tuple0 _tmp101=*(*((struct
_tuple0**)_check_dynforward_subscript(_tmpDA,sizeof(struct _tuple0*),i)));_tmp102=
_tmp101.f1;_tmp103=_tmp101.f2;_tmp104=_tmp101.f3;{const char*_tmp247;void*_tmp246[
2];struct Cyc_Int_pa_struct _tmp245;struct Cyc_String_pa_struct _tmp244;(_tmp244.tag=
0,((_tmp244.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmp103),((_tmp245.tag=
1,((_tmp245.f1=(unsigned long)((int)_tmp102),((_tmp246[0]=& _tmp245,((_tmp246[1]=&
_tmp244,Cyc_printf(((_tmp247="<Component ofs=\"%d\" nm=\"%s\">",_tag_dynforward(
_tmp247,sizeof(char),_get_zero_arr_size_char(_tmp247,29)))),_tag_dynforward(
_tmp246,sizeof(void*),2)))))))))))));}Cyc_Typerep_xmlize_typestruct(_tmp104);{
const char*_tmp24A;void*_tmp249;(_tmp249=0,Cyc_printf(((_tmp24A="</Component>",
_tag_dynforward(_tmp24A,sizeof(char),_get_zero_arr_size_char(_tmp24A,13)))),
_tag_dynforward(_tmp249,sizeof(void*),0)));}}}{const char*_tmp24D;void*_tmp24C;(
_tmp24C=0,Cyc_printf(((_tmp24D="</Struct>",_tag_dynforward(_tmp24D,sizeof(char),
_get_zero_arr_size_char(_tmp24D,10)))),_tag_dynforward(_tmp24C,sizeof(void*),0)));}
goto _LL37;_LL44: if(_tmpD2 <= (void*)2)goto _LL46;if(*((int*)_tmpD2)!= 4)goto _LL46;
_tmpDB=((struct Cyc_Typerep_Tuple_struct*)_tmpD2)->f1;_tmpDC=((struct Cyc_Typerep_Tuple_struct*)
_tmpD2)->f2;_LL45:{const char*_tmp251;void*_tmp250[1];struct Cyc_Int_pa_struct
_tmp24F;(_tmp24F.tag=1,((_tmp24F.f1=(unsigned long)((int)_tmpDB),((_tmp250[0]=&
_tmp24F,Cyc_printf(((_tmp251="<Tuple sz=\"%d\">",_tag_dynforward(_tmp251,sizeof(
char),_get_zero_arr_size_char(_tmp251,16)))),_tag_dynforward(_tmp250,sizeof(void*),
1)))))));}{int i=0;for(0;i < _get_dynforward_size(_tmpDC,sizeof(struct _tuple1*));
++ i){unsigned int _tmp111;void*_tmp112;struct _tuple1 _tmp110=*(*((struct _tuple1**)
_check_dynforward_subscript(_tmpDC,sizeof(struct _tuple1*),i)));_tmp111=_tmp110.f1;
_tmp112=_tmp110.f2;{const char*_tmp255;void*_tmp254[1];struct Cyc_Int_pa_struct
_tmp253;(_tmp253.tag=1,((_tmp253.f1=(unsigned long)((int)_tmp111),((_tmp254[0]=&
_tmp253,Cyc_printf(((_tmp255="<Component ofs=\"%d\">",_tag_dynforward(_tmp255,
sizeof(char),_get_zero_arr_size_char(_tmp255,21)))),_tag_dynforward(_tmp254,
sizeof(void*),1)))))));}Cyc_Typerep_xmlize_typestruct(_tmp112);{const char*
_tmp258;void*_tmp257;(_tmp257=0,Cyc_printf(((_tmp258="</Component>",
_tag_dynforward(_tmp258,sizeof(char),_get_zero_arr_size_char(_tmp258,13)))),
_tag_dynforward(_tmp257,sizeof(void*),0)));}}}{const char*_tmp25B;void*_tmp25A;(
_tmp25A=0,Cyc_printf(((_tmp25B="</Tuple>",_tag_dynforward(_tmp25B,sizeof(char),
_get_zero_arr_size_char(_tmp25B,9)))),_tag_dynforward(_tmp25A,sizeof(void*),0)));}
goto _LL37;_LL46: if(_tmpD2 <= (void*)2)goto _LL48;if(*((int*)_tmpD2)!= 6)goto _LL48;
_tmpDD=((struct Cyc_Typerep_TUnionField_struct*)_tmpD2)->f1;_tmpDE=((struct Cyc_Typerep_TUnionField_struct*)
_tmpD2)->f2;_tmpDF=((struct Cyc_Typerep_TUnionField_struct*)_tmpD2)->f3;_tmpE0=((
struct Cyc_Typerep_TUnionField_struct*)_tmpD2)->f4;_LL47:{const char*_tmp261;void*
_tmp260[3];struct Cyc_String_pa_struct _tmp25F;struct Cyc_String_pa_struct _tmp25E;
struct Cyc_Int_pa_struct _tmp25D;(_tmp25D.tag=1,((_tmp25D.f1=(unsigned long)((int)
_tmpDF),((_tmp25E.tag=0,((_tmp25E.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)_tmpDE),((_tmp25F.tag=0,((_tmp25F.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)_tmpDD),((_tmp260[0]=& _tmp25F,((_tmp260[1]=& _tmp25E,((
_tmp260[2]=& _tmp25D,Cyc_printf(((_tmp261="<TUnionField tname=\"%s\" fname=\"%s\" sz=\"%d\">",
_tag_dynforward(_tmp261,sizeof(char),_get_zero_arr_size_char(_tmp261,44)))),
_tag_dynforward(_tmp260,sizeof(void*),3)))))))))))))))))));}{int i=0;for(0;i < 
_get_dynforward_size(_tmpE0,sizeof(struct _tuple1*));++ i){unsigned int _tmp120;
void*_tmp121;struct _tuple1 _tmp11F=*(*((struct _tuple1**)
_check_dynforward_subscript(_tmpE0,sizeof(struct _tuple1*),i)));_tmp120=_tmp11F.f1;
_tmp121=_tmp11F.f2;{const char*_tmp265;void*_tmp264[1];struct Cyc_Int_pa_struct
_tmp263;(_tmp263.tag=1,((_tmp263.f1=(unsigned long)((int)_tmp120),((_tmp264[0]=&
_tmp263,Cyc_printf(((_tmp265="<Component ofs=\"%d\">",_tag_dynforward(_tmp265,
sizeof(char),_get_zero_arr_size_char(_tmp265,21)))),_tag_dynforward(_tmp264,
sizeof(void*),1)))))));}Cyc_Typerep_xmlize_typestruct(_tmp121);{const char*
_tmp268;void*_tmp267;(_tmp267=0,Cyc_printf(((_tmp268="</Component>",
_tag_dynforward(_tmp268,sizeof(char),_get_zero_arr_size_char(_tmp268,13)))),
_tag_dynforward(_tmp267,sizeof(void*),0)));}}}{const char*_tmp26B;void*_tmp26A;(
_tmp26A=0,Cyc_printf(((_tmp26B="</TUnionField>",_tag_dynforward(_tmp26B,sizeof(
char),_get_zero_arr_size_char(_tmp26B,15)))),_tag_dynforward(_tmp26A,sizeof(void*),
0)));}goto _LL37;_LL48: if(_tmpD2 <= (void*)2)goto _LL4A;if(*((int*)_tmpD2)!= 5)goto
_LL4A;_tmpE1=((struct Cyc_Typerep_TUnion_struct*)_tmpD2)->f1;_tmpE2=((struct Cyc_Typerep_TUnion_struct*)
_tmpD2)->f2;_tmpE3=((struct Cyc_Typerep_TUnion_struct*)_tmpD2)->f3;_LL49:{const
char*_tmp26F;void*_tmp26E[1];struct Cyc_String_pa_struct _tmp26D;(_tmp26D.tag=0,((
_tmp26D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmpE1),((_tmp26E[0]=&
_tmp26D,Cyc_printf(((_tmp26F="<TUnion name=\"%s\">",_tag_dynforward(_tmp26F,
sizeof(char),_get_zero_arr_size_char(_tmp26F,19)))),_tag_dynforward(_tmp26E,
sizeof(void*),1)))))));}{int i=0;for(0;i < _get_dynforward_size(_tmpE2,sizeof(
struct _tuple2*));++ i){unsigned int _tmp12D;struct _dynforward_ptr _tmp12E;struct
_tuple2 _tmp12C=*(*((struct _tuple2**)_check_dynforward_subscript(_tmpE2,sizeof(
struct _tuple2*),i)));_tmp12D=_tmp12C.f1;_tmp12E=_tmp12C.f2;{const char*_tmp274;
void*_tmp273[2];struct Cyc_Int_pa_struct _tmp272;struct Cyc_String_pa_struct _tmp271;(
_tmp271.tag=0,((_tmp271.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp12E),((_tmp272.tag=1,((_tmp272.f1=(unsigned long)((int)_tmp12D),((_tmp273[0]=&
_tmp272,((_tmp273[1]=& _tmp271,Cyc_printf(((_tmp274="<Tag tag=\"%d\" nm=\"%s\"/>",
_tag_dynforward(_tmp274,sizeof(char),_get_zero_arr_size_char(_tmp274,24)))),
_tag_dynforward(_tmp273,sizeof(void*),2)))))))))))));}}}{int i=0;for(0;i < 
_get_dynforward_size(_tmpE3,sizeof(struct _tuple0*));++ i){unsigned int _tmp134;
struct _dynforward_ptr _tmp135;void*_tmp136;struct _tuple0 _tmp133=*(*((struct
_tuple0**)_check_dynforward_subscript(_tmpE3,sizeof(struct _tuple0*),i)));_tmp134=
_tmp133.f1;_tmp135=_tmp133.f2;_tmp136=_tmp133.f3;{const char*_tmp279;void*_tmp278[
2];struct Cyc_Int_pa_struct _tmp277;struct Cyc_String_pa_struct _tmp276;(_tmp276.tag=
0,((_tmp276.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmp135),((_tmp277.tag=
1,((_tmp277.f1=(unsigned long)((int)_tmp134),((_tmp278[0]=& _tmp277,((_tmp278[1]=&
_tmp276,Cyc_printf(((_tmp279="<Tag tag=\"%d\" nm=\"%s\">",_tag_dynforward(
_tmp279,sizeof(char),_get_zero_arr_size_char(_tmp279,23)))),_tag_dynforward(
_tmp278,sizeof(void*),2)))))))))))));}Cyc_Typerep_xmlize_typestruct(_tmp136);{
const char*_tmp27C;void*_tmp27B;(_tmp27B=0,Cyc_printf(((_tmp27C="</Tag>",
_tag_dynforward(_tmp27C,sizeof(char),_get_zero_arr_size_char(_tmp27C,7)))),
_tag_dynforward(_tmp27B,sizeof(void*),0)));}}}{const char*_tmp27F;void*_tmp27E;(
_tmp27E=0,Cyc_printf(((_tmp27F="</Tunion>",_tag_dynforward(_tmp27F,sizeof(char),
_get_zero_arr_size_char(_tmp27F,10)))),_tag_dynforward(_tmp27E,sizeof(void*),0)));}
goto _LL37;_LL4A: if(_tmpD2 <= (void*)2)goto _LL4C;if(*((int*)_tmpD2)!= 7)goto _LL4C;
_tmpE4=((struct Cyc_Typerep_XTUnion_struct*)_tmpD2)->f1;_tmpE5=((struct Cyc_Typerep_XTUnion_struct*)
_tmpD2)->f2;_LL4B:{const char*_tmp283;void*_tmp282[1];struct Cyc_String_pa_struct
_tmp281;(_tmp281.tag=0,((_tmp281.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)_tmpE4),((_tmp282[0]=& _tmp281,Cyc_printf(((_tmp283="<XTUnion name=\"%s\">",
_tag_dynforward(_tmp283,sizeof(char),_get_zero_arr_size_char(_tmp283,20)))),
_tag_dynforward(_tmp282,sizeof(void*),1)))))));}{int i=0;for(0;i < 
_get_dynforward_size(_tmpE5,sizeof(struct _tuple3*));++ i){struct _dynforward_ptr
_tmp143;void*_tmp144;struct _tuple3 _tmp142=*(*((struct _tuple3**)
_check_dynforward_subscript(_tmpE5,sizeof(struct _tuple3*),i)));_tmp143=_tmp142.f1;
_tmp144=_tmp142.f2;{const char*_tmp287;void*_tmp286[1];struct Cyc_String_pa_struct
_tmp285;(_tmp285.tag=0,((_tmp285.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)_dynforward_ptr_plus(_tmp143,sizeof(char),4)),((_tmp286[0]=&
_tmp285,Cyc_printf(((_tmp287="<Tag tag=\"%s\">",_tag_dynforward(_tmp287,sizeof(
char),_get_zero_arr_size_char(_tmp287,15)))),_tag_dynforward(_tmp286,sizeof(void*),
1)))))));}Cyc_Typerep_xmlize_typestruct(_tmp144);{const char*_tmp28A;void*_tmp289;(
_tmp289=0,Cyc_printf(((_tmp28A="</Tag>",_tag_dynforward(_tmp28A,sizeof(char),
_get_zero_arr_size_char(_tmp28A,7)))),_tag_dynforward(_tmp289,sizeof(void*),0)));}}}{
const char*_tmp28D;void*_tmp28C;(_tmp28C=0,Cyc_printf(((_tmp28D="</XTunion>",
_tag_dynforward(_tmp28D,sizeof(char),_get_zero_arr_size_char(_tmp28D,11)))),
_tag_dynforward(_tmp28C,sizeof(void*),0)));}goto _LL37;_LL4C: if(_tmpD2 <= (void*)2)
goto _LL4E;if(*((int*)_tmpD2)!= 8)goto _LL4E;_tmpE6=((struct Cyc_Typerep_Union_struct*)
_tmpD2)->f1;_tmpE7=((struct Cyc_Typerep_Union_struct*)_tmpD2)->f2;_tmpE8=((struct
Cyc_Typerep_Union_struct*)_tmpD2)->f3;_LL4D:{const char*_tmp292;void*_tmp291[2];
struct Cyc_String_pa_struct _tmp290;struct Cyc_Int_pa_struct _tmp28F;(_tmp28F.tag=1,((
_tmp28F.f1=(unsigned long)_tmpE7,((_tmp290.tag=0,((_tmp290.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Typerep_anon_or_name(_tmpE6)),((
_tmp291[0]=& _tmp290,((_tmp291[1]=& _tmp28F,Cyc_printf(((_tmp292="<Union name=\"%s\" sz=\"%d\">",
_tag_dynforward(_tmp292,sizeof(char),_get_zero_arr_size_char(_tmp292,26)))),
_tag_dynforward(_tmp291,sizeof(void*),2)))))))))))));}{int i=0;for(0;i < 
_get_dynforward_size(_tmpE8,sizeof(struct _tuple3*));++ i){struct _dynforward_ptr
_tmp151;void*_tmp152;struct _tuple3 _tmp150=*(*((struct _tuple3**)
_check_dynforward_subscript(_tmpE8,sizeof(struct _tuple3*),i)));_tmp151=_tmp150.f1;
_tmp152=_tmp150.f2;{const char*_tmp296;void*_tmp295[1];struct Cyc_String_pa_struct
_tmp294;(_tmp294.tag=0,((_tmp294.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)_tmp151),((_tmp295[0]=& _tmp294,Cyc_printf(((_tmp296="<Case nm=\"%s\">",
_tag_dynforward(_tmp296,sizeof(char),_get_zero_arr_size_char(_tmp296,15)))),
_tag_dynforward(_tmp295,sizeof(void*),1)))))));}Cyc_Typerep_xmlize_typestruct(
_tmp152);{const char*_tmp299;void*_tmp298;(_tmp298=0,Cyc_printf(((_tmp299="</Case>",
_tag_dynforward(_tmp299,sizeof(char),_get_zero_arr_size_char(_tmp299,8)))),
_tag_dynforward(_tmp298,sizeof(void*),0)));}}}{const char*_tmp29C;void*_tmp29B;(
_tmp29B=0,Cyc_printf(((_tmp29C="</Union>",_tag_dynforward(_tmp29C,sizeof(char),
_get_zero_arr_size_char(_tmp29C,9)))),_tag_dynforward(_tmp29B,sizeof(void*),0)));}
goto _LL37;_LL4E: if(_tmpD2 <= (void*)2)goto _LL37;if(*((int*)_tmpD2)!= 9)goto _LL37;
_tmpE9=((struct Cyc_Typerep_Enum_struct*)_tmpD2)->f1;_tmpEA=((struct Cyc_Typerep_Enum_struct*)
_tmpD2)->f2;_tmpEB=((struct Cyc_Typerep_Enum_struct*)_tmpD2)->f3;_LL4F:{const char*
_tmp2A1;void*_tmp2A0[2];struct Cyc_String_pa_struct _tmp29F;struct Cyc_Int_pa_struct
_tmp29E;(_tmp29E.tag=1,((_tmp29E.f1=(unsigned long)_tmpEA,((_tmp29F.tag=0,((
_tmp29F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Typerep_anon_or_name(
_tmpE9)),((_tmp2A0[0]=& _tmp29F,((_tmp2A0[1]=& _tmp29E,Cyc_printf(((_tmp2A1="<Enum name=\"%s\" szb=\"%d\">",
_tag_dynforward(_tmp2A1,sizeof(char),_get_zero_arr_size_char(_tmp2A1,26)))),
_tag_dynforward(_tmp2A0,sizeof(void*),2)))))))))))));}{int i=0;for(0;i < 
_get_dynforward_size(_tmpEB,sizeof(struct _tuple2*));++ i){unsigned int _tmp15F;
struct _dynforward_ptr _tmp160;struct _tuple2 _tmp15E=*(*((struct _tuple2**)
_check_dynforward_subscript(_tmpEB,sizeof(struct _tuple2*),i)));_tmp15F=_tmp15E.f1;
_tmp160=_tmp15E.f2;{const char*_tmp2A6;void*_tmp2A5[2];struct Cyc_Int_pa_struct
_tmp2A4;struct Cyc_String_pa_struct _tmp2A3;(_tmp2A3.tag=0,((_tmp2A3.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp160),((_tmp2A4.tag=1,((_tmp2A4.f1=(
unsigned long)((int)_tmp15F),((_tmp2A5[0]=& _tmp2A4,((_tmp2A5[1]=& _tmp2A3,Cyc_printf(((
_tmp2A6="<Val val=\"%d\">%s</Val>",_tag_dynforward(_tmp2A6,sizeof(char),
_get_zero_arr_size_char(_tmp2A6,23)))),_tag_dynforward(_tmp2A5,sizeof(void*),2)))))))))))));}}}{
const char*_tmp2A9;void*_tmp2A8;(_tmp2A8=0,Cyc_printf(((_tmp2A9="</Enum>",
_tag_dynforward(_tmp2A9,sizeof(char),_get_zero_arr_size_char(_tmp2A9,8)))),
_tag_dynforward(_tmp2A8,sizeof(void*),0)));}goto _LL37;_LL37:;}
