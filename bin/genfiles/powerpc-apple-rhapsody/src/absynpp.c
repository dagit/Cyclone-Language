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
void*v;};struct _dynforward_ptr Cyc_Core_new_string(unsigned int);extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dynforward_ptr f1;};
extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[
16];struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dynforward_ptr,
struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[8];int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*
x);int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2);int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct
Cyc_List_List*l2);extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*
f);struct _dynforward_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);struct Cyc_PP_Doc*
Cyc_PP_nil_doc();struct Cyc_PP_Doc*Cyc_PP_blank_doc();struct Cyc_PP_Doc*Cyc_PP_line_doc();
struct Cyc_PP_Doc*Cyc_PP_text(struct _dynforward_ptr s);struct Cyc_PP_Doc*Cyc_PP_textptr(
struct _dynforward_ptr*p);struct Cyc_PP_Doc*Cyc_PP_text_width(struct
_dynforward_ptr s,int w);struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d);
struct Cyc_PP_Doc*Cyc_PP_cat(struct _dynforward_ptr);struct Cyc_PP_Doc*Cyc_PP_seq(
struct _dynforward_ptr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_ppseq(
struct Cyc_PP_Doc*(*pp)(void*),struct _dynforward_ptr sep,struct Cyc_List_List*l);
struct Cyc_PP_Doc*Cyc_PP_seql(struct _dynforward_ptr sep,struct Cyc_List_List*l0);
struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(void*),struct
_dynforward_ptr sep,struct Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_group(struct
_dynforward_ptr start,struct _dynforward_ptr stop,struct _dynforward_ptr sep,struct
Cyc_List_List*l);struct Cyc_PP_Doc*Cyc_PP_egroup(struct _dynforward_ptr start,
struct _dynforward_ptr stop,struct _dynforward_ptr sep,struct Cyc_List_List*l);struct
Cyc_Lineno_Pos{struct _dynforward_ptr logical_file;struct _dynforward_ptr line;int
line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _dynforward_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];
struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};
union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};struct _tuple0{union Cyc_Absyn_Nmspace_union
f1;struct _dynforward_ptr*f2;};struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int
print_const;int q_volatile;int q_restrict;int real_const;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_No_constr_struct{int tag;};
union Cyc_Absyn_Constraint_union{struct Cyc_Absyn_Eq_constr_struct Eq_constr;struct
Cyc_Absyn_Forward_constr_struct Forward_constr;struct Cyc_Absyn_No_constr_struct
No_constr;};struct Cyc_Absyn_Conref{union Cyc_Absyn_Constraint_union v;};struct Cyc_Absyn_Eq_kb_struct{
int tag;void*f1;};struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;
};struct Cyc_Absyn_Less_kb_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;};struct
Cyc_Absyn_Tvar{struct _dynforward_ptr*name;int identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AbsUpper_b_struct{int tag;void*f1;
};struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*
rgn_loc;struct Cyc_Position_Segment*zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;
struct Cyc_Absyn_Conref*nullable;struct Cyc_Absyn_Conref*bounds;struct Cyc_Absyn_Conref*
zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;
struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{
struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{
struct Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownTunionInfo{struct
_tuple0*name;int is_xtunion;int is_flat;};struct Cyc_Absyn_UnknownTunion_struct{int
tag;struct Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int
tag;struct Cyc_Absyn_Tuniondecl**f1;};union Cyc_Absyn_TunionInfoU_union{struct Cyc_Absyn_UnknownTunion_struct
UnknownTunion;struct Cyc_Absyn_KnownTunion_struct KnownTunion;};struct Cyc_Absyn_TunionInfo{
union Cyc_Absyn_TunionInfoU_union tunion_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
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
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;
};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple0*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void**f4;};struct Cyc_Absyn_TagType_struct{
int tag;void*f1;};struct Cyc_Absyn_TypeInt_struct{int tag;int f1;};struct Cyc_Absyn_AccessEff_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};struct Cyc_Absyn_NoTypes_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*
f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};
struct Cyc_Absyn_Aligned_att_struct{int tag;int f1;};struct Cyc_Absyn_Section_att_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Absyn_Format_att_struct{int tag;void*
f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{int tag;int f1;};struct Cyc_Absyn_Mode_att_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Absyn_Carray_mod_struct{int tag;struct
Cyc_Absyn_Conref*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Conref*f2;struct Cyc_Position_Segment*
f3;};struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct
Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int tag;struct Cyc_Position_Segment*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Char_c_struct{int tag;void*f1;char f2;
};struct Cyc_Absyn_Short_c_struct{int tag;void*f1;short f2;};struct Cyc_Absyn_Int_c_struct{
int tag;void*f1;int f2;};struct Cyc_Absyn_LongLong_c_struct{int tag;void*f1;
long long f2;};struct Cyc_Absyn_Float_c_struct{int tag;struct _dynforward_ptr f1;};
struct Cyc_Absyn_String_c_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Absyn_Null_c_struct{
int tag;};union Cyc_Absyn_Cnst_union{struct Cyc_Absyn_Char_c_struct Char_c;struct Cyc_Absyn_Short_c_struct
Short_c;struct Cyc_Absyn_Int_c_struct Int_c;struct Cyc_Absyn_LongLong_c_struct
LongLong_c;struct Cyc_Absyn_Float_c_struct Float_c;struct Cyc_Absyn_String_c_struct
String_c;struct Cyc_Absyn_Null_c_struct Null_c;};struct Cyc_Absyn_VarargCallInfo{
int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
struct Cyc_Absyn_StructField_struct{int tag;struct _dynforward_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{
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
_dynforward_ptr*f2;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _dynforward_ptr*f2;};struct Cyc_Absyn_Subscript_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{int tag;
struct Cyc_List_List*f1;};struct _tuple1{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple1*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
f2;struct Cyc_Absyn_Tunionfield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct
_tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{
int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;
void*r;struct Cyc_Position_Segment*loc;void*annot;};struct Cyc_Absyn_Exp_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple2{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple2 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dynforward_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple2 f2;struct _tuple2 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dynforward_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple2 f2;
};struct Cyc_Absyn_TryCatch_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Region_s_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*
f2;int f3;struct Cyc_Absyn_Exp*f4;struct Cyc_Absyn_Stmt*f5;};struct Cyc_Absyn_ResetRegion_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Alias_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;struct Cyc_Absyn_Stmt*f4;};
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
int tag;char f1;};struct Cyc_Absyn_Float_p_struct{int tag;struct _dynforward_ptr f1;};
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
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dynforward_ptr*
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
struct _dynforward_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*
k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_bounds_one;struct Cyc_Absyn_Exp*
Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct _dynforward_ptr Cyc_Absyn_attribute2string(void*);struct _tuple3{void*f1;
struct _tuple0*f2;};struct _tuple3 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU_union);
struct Cyc_Buffer_t;unsigned long Cyc_strlen(struct _dynforward_ptr s);int Cyc_strptrcmp(
struct _dynforward_ptr*s1,struct _dynforward_ptr*s2);struct _dynforward_ptr Cyc_strconcat(
struct _dynforward_ptr,struct _dynforward_ptr);struct _dynforward_ptr Cyc_str_sepstr(
struct Cyc_List_List*,struct _dynforward_ptr);struct _tuple4{unsigned int f1;int f2;}
;struct _tuple4 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple5{void*f1;void*f2;};struct _tuple5*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple5*Cyc_Dict_rchoose(
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
le;};void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};extern int Cyc_Absynpp_print_scopes;
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params
Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params
Cyc_Absynpp_tc_params_r;void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,
struct Cyc___cycFILE*f);struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*
d);struct _dynforward_ptr Cyc_Absynpp_typ2string(void*);struct _dynforward_ptr Cyc_Absynpp_typ2cstring(
void*);struct _dynforward_ptr Cyc_Absynpp_kind2string(void*);struct _dynforward_ptr
Cyc_Absynpp_kindbound2string(void*);struct _dynforward_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*);struct _dynforward_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct
_dynforward_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);struct
_dynforward_ptr Cyc_Absynpp_prim2string(void*p);struct _dynforward_ptr Cyc_Absynpp_pat2string(
struct Cyc_Absyn_Pat*p);struct _dynforward_ptr Cyc_Absynpp_scope2string(void*sc);
int Cyc_Absynpp_is_anon_aggrtype(void*t);extern struct _dynforward_ptr Cyc_Absynpp_cyc_string;
extern struct _dynforward_ptr*Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(
struct Cyc_Absyn_Exp*);struct _dynforward_ptr Cyc_Absynpp_char_escape(char);struct
_dynforward_ptr Cyc_Absynpp_string_escape(struct _dynforward_ptr);struct
_dynforward_ptr Cyc_Absynpp_prim2str(void*p);int Cyc_Absynpp_is_declaration(struct
Cyc_Absyn_Stmt*s);struct _tuple6{struct _dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;
void*f3;};struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple6*arg);struct _tuple7{
struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};struct _tuple7 Cyc_Absynpp_to_tms(
struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);extern void*Cyc_Cyclone_c_compiler;
struct _tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_PP_Doc*
Cyc_Absynpp_dp2doc(struct _tuple8*dp);struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List*cs);struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*);struct Cyc_PP_Doc*
Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(
void*);struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*);struct Cyc_PP_Doc*
Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(
int inprec,struct Cyc_Absyn_Exp*e);struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int
inprec,struct Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct
_tuple0*);struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*);struct
Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst_union);struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(
void*);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,void*p,struct Cyc_List_List*
es);struct _tuple9{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_PP_Doc*
Cyc_Absynpp_de2doc(struct _tuple9*de);struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_Core_Opt*dopt);struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po);struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(
struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct
Cyc_List_List*fs);struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*
vd);static int Cyc_Absynpp_expand_typedefs;static int Cyc_Absynpp_qvar_to_Cids;
static char _tmp0[4]="Cyc";struct _dynforward_ptr Cyc_Absynpp_cyc_string={_tmp0,
_tmp0 + 4};struct _dynforward_ptr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;
static int Cyc_Absynpp_add_cyc_prefix;static int Cyc_Absynpp_to_VC;static int Cyc_Absynpp_decls_first;
static int Cyc_Absynpp_rewrite_temp_tvars;static int Cyc_Absynpp_print_all_tvars;
static int Cyc_Absynpp_print_all_kinds;static int Cyc_Absynpp_print_all_effects;
static int Cyc_Absynpp_print_using_stmts;static int Cyc_Absynpp_print_externC_stmts;
static int Cyc_Absynpp_print_full_evars;static int Cyc_Absynpp_generate_line_directives;
static int Cyc_Absynpp_use_curr_namespace;static int Cyc_Absynpp_print_zeroterm;
static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=0;struct Cyc_Absynpp_Params;
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs){Cyc_Absynpp_expand_typedefs=fs->expand_typedefs;Cyc_Absynpp_qvar_to_Cids=
fs->qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=fs->add_cyc_prefix;Cyc_Absynpp_to_VC=
fs->to_VC;Cyc_Absynpp_decls_first=fs->decls_first;Cyc_Absynpp_rewrite_temp_tvars=
fs->rewrite_temp_tvars;Cyc_Absynpp_print_all_tvars=fs->print_all_tvars;Cyc_Absynpp_print_all_kinds=
fs->print_all_kinds;Cyc_Absynpp_print_all_effects=fs->print_all_effects;Cyc_Absynpp_print_using_stmts=
fs->print_using_stmts;Cyc_Absynpp_print_externC_stmts=fs->print_externC_stmts;
Cyc_Absynpp_print_full_evars=fs->print_full_evars;Cyc_Absynpp_print_zeroterm=fs->print_zeroterm;
Cyc_Absynpp_generate_line_directives=fs->generate_line_directives;Cyc_Absynpp_use_curr_namespace=
fs->use_curr_namespace;Cyc_Absynpp_curr_namespace=fs->curr_namespace;}struct Cyc_Absynpp_Params
Cyc_Absynpp_cyc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params
Cyc_Absynpp_cyci_params_r={1,0,0,0,0,1,0,0,1,1,1,0,1,0,1,0};struct Cyc_Absynpp_Params
Cyc_Absynpp_c_params_r={1,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0};struct Cyc_Absynpp_Params
Cyc_Absynpp_tc_params_r={0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0};static void Cyc_Absynpp_curr_namespace_add(
struct _dynforward_ptr*v);static void Cyc_Absynpp_curr_namespace_add(struct
_dynforward_ptr*v){struct Cyc_List_List*_tmp4EC;Cyc_Absynpp_curr_namespace=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
Cyc_Absynpp_curr_namespace,((_tmp4EC=_cycalloc(sizeof(*_tmp4EC)),((_tmp4EC->hd=v,((
_tmp4EC->tl=0,_tmp4EC)))))));}static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**
l);static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l){if(((struct Cyc_List_List*)
_check_null(*l))->tl == 0)*l=0;else{Cyc_Absynpp_suppr_last(&((struct Cyc_List_List*)
_check_null(*l))->tl);}}static void Cyc_Absynpp_curr_namespace_drop();static void
Cyc_Absynpp_curr_namespace_drop(){((void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(&
Cyc_Absynpp_curr_namespace);}struct _dynforward_ptr Cyc_Absynpp_char_escape(char c);
struct _dynforward_ptr Cyc_Absynpp_char_escape(char c){switch(c){case '\a': _LL0: {
const char*_tmp4ED;return(_tmp4ED="\\a",_tag_dynforward(_tmp4ED,sizeof(char),
_get_zero_arr_size_char(_tmp4ED,3)));}case '\b': _LL1: {const char*_tmp4EE;return(
_tmp4EE="\\b",_tag_dynforward(_tmp4EE,sizeof(char),_get_zero_arr_size_char(
_tmp4EE,3)));}case '\f': _LL2: {const char*_tmp4EF;return(_tmp4EF="\\f",
_tag_dynforward(_tmp4EF,sizeof(char),_get_zero_arr_size_char(_tmp4EF,3)));}case '\n':
_LL3: {const char*_tmp4F0;return(_tmp4F0="\\n",_tag_dynforward(_tmp4F0,sizeof(
char),_get_zero_arr_size_char(_tmp4F0,3)));}case '\r': _LL4: {const char*_tmp4F1;
return(_tmp4F1="\\r",_tag_dynforward(_tmp4F1,sizeof(char),
_get_zero_arr_size_char(_tmp4F1,3)));}case '\t': _LL5: {const char*_tmp4F2;return(
_tmp4F2="\\t",_tag_dynforward(_tmp4F2,sizeof(char),_get_zero_arr_size_char(
_tmp4F2,3)));}case '\v': _LL6: {const char*_tmp4F3;return(_tmp4F3="\\v",
_tag_dynforward(_tmp4F3,sizeof(char),_get_zero_arr_size_char(_tmp4F3,3)));}case '\\':
_LL7: {const char*_tmp4F4;return(_tmp4F4="\\\\",_tag_dynforward(_tmp4F4,sizeof(
char),_get_zero_arr_size_char(_tmp4F4,3)));}case '"': _LL8: {const char*_tmp4F5;
return(_tmp4F5="\"",_tag_dynforward(_tmp4F5,sizeof(char),_get_zero_arr_size_char(
_tmp4F5,2)));}case '\'': _LL9: {const char*_tmp4F6;return(_tmp4F6="\\'",
_tag_dynforward(_tmp4F6,sizeof(char),_get_zero_arr_size_char(_tmp4F6,3)));}
default: _LLA: if(c >= ' '  && c <= '~'){struct _dynforward_ptr _tmpC=Cyc_Core_new_string(
2);{char _tmp4F9;char _tmp4F8;struct _dynforward_ptr _tmp4F7;(_tmp4F7=
_dynforward_ptr_plus(_tmpC,sizeof(char),0),((_tmp4F8=*((char*)
_check_dynforward_subscript(_tmp4F7,sizeof(char),0)),((_tmp4F9=c,((
_get_dynforward_size(_tmp4F7,sizeof(char))== 1  && (_tmp4F8 == '\000'  && _tmp4F9 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp4F7.curr)=_tmp4F9)))))));}return(struct
_dynforward_ptr)_tmpC;}else{struct _dynforward_ptr _tmp10=Cyc_Core_new_string(5);
int j=0;{char _tmp4FC;char _tmp4FB;struct _dynforward_ptr _tmp4FA;(_tmp4FA=
_dynforward_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp4FB=*((char*)
_check_dynforward_subscript(_tmp4FA,sizeof(char),0)),((_tmp4FC='\\',((
_get_dynforward_size(_tmp4FA,sizeof(char))== 1  && (_tmp4FB == '\000'  && _tmp4FC != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp4FA.curr)=_tmp4FC)))))));}{char _tmp4FF;char
_tmp4FE;struct _dynforward_ptr _tmp4FD;(_tmp4FD=_dynforward_ptr_plus(_tmp10,
sizeof(char),j ++),((_tmp4FE=*((char*)_check_dynforward_subscript(_tmp4FD,sizeof(
char),0)),((_tmp4FF=(char)('0' + ((unsigned char)c >> 6 & 3)),((
_get_dynforward_size(_tmp4FD,sizeof(char))== 1  && (_tmp4FE == '\000'  && _tmp4FF != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp4FD.curr)=_tmp4FF)))))));}{char _tmp502;char
_tmp501;struct _dynforward_ptr _tmp500;(_tmp500=_dynforward_ptr_plus(_tmp10,
sizeof(char),j ++),((_tmp501=*((char*)_check_dynforward_subscript(_tmp500,sizeof(
char),0)),((_tmp502=(char)('0' + (c >> 3 & 7)),((_get_dynforward_size(_tmp500,
sizeof(char))== 1  && (_tmp501 == '\000'  && _tmp502 != '\000')?_throw_arraybounds():
1,*((char*)_tmp500.curr)=_tmp502)))))));}{char _tmp505;char _tmp504;struct
_dynforward_ptr _tmp503;(_tmp503=_dynforward_ptr_plus(_tmp10,sizeof(char),j ++),((
_tmp504=*((char*)_check_dynforward_subscript(_tmp503,sizeof(char),0)),((_tmp505=(
char)('0' + (c & 7)),((_get_dynforward_size(_tmp503,sizeof(char))== 1  && (_tmp504
== '\000'  && _tmp505 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp503.curr)=
_tmp505)))))));}return(struct _dynforward_ptr)_tmp10;}}}static int Cyc_Absynpp_special(
struct _dynforward_ptr s);static int Cyc_Absynpp_special(struct _dynforward_ptr s){int
sz=(int)(_get_dynforward_size(s,sizeof(char))- 1);{int i=0;for(0;i < sz;++ i){char c=*((
const char*)_check_dynforward_subscript(s,sizeof(char),i));if(((c <= ' '  || c >= '~')
 || c == '"') || c == '\\')return 1;}}return 0;}struct _dynforward_ptr Cyc_Absynpp_string_escape(
struct _dynforward_ptr s);struct _dynforward_ptr Cyc_Absynpp_string_escape(struct
_dynforward_ptr s){if(!Cyc_Absynpp_special(s))return s;{int n=(int)(
_get_dynforward_size(s,sizeof(char))- 1);if(n > 0  && *((const char*)
_check_dynforward_subscript(s,sizeof(char),n))== '\000')-- n;{int len=0;{int i=0;
for(0;i <= n;++ i){char _tmp1D=*((const char*)_check_dynforward_subscript(s,sizeof(
char),i));_LLD: if(_tmp1D != '\a')goto _LLF;_LLE: goto _LL10;_LLF: if(_tmp1D != '\b')
goto _LL11;_LL10: goto _LL12;_LL11: if(_tmp1D != '\f')goto _LL13;_LL12: goto _LL14;_LL13:
if(_tmp1D != '\n')goto _LL15;_LL14: goto _LL16;_LL15: if(_tmp1D != '\r')goto _LL17;
_LL16: goto _LL18;_LL17: if(_tmp1D != '\t')goto _LL19;_LL18: goto _LL1A;_LL19: if(_tmp1D
!= '\v')goto _LL1B;_LL1A: goto _LL1C;_LL1B: if(_tmp1D != '\\')goto _LL1D;_LL1C: goto
_LL1E;_LL1D: if(_tmp1D != '"')goto _LL1F;_LL1E: len +=2;goto _LLC;_LL1F:;_LL20: if(
_tmp1D >= ' '  && _tmp1D <= '~')++ len;else{len +=4;}goto _LLC;_LLC:;}}{struct
_dynforward_ptr t=Cyc_Core_new_string((unsigned int)(len + 1));int j=0;{int i=0;for(
0;i <= n;++ i){char _tmp1E=*((const char*)_check_dynforward_subscript(s,sizeof(char),
i));_LL22: if(_tmp1E != '\a')goto _LL24;_LL23:{char _tmp508;char _tmp507;struct
_dynforward_ptr _tmp506;(_tmp506=_dynforward_ptr_plus(t,sizeof(char),j ++),((
_tmp507=*((char*)_check_dynforward_subscript(_tmp506,sizeof(char),0)),((_tmp508='\\',((
_get_dynforward_size(_tmp506,sizeof(char))== 1  && (_tmp507 == '\000'  && _tmp508 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp506.curr)=_tmp508)))))));}{char _tmp50B;char
_tmp50A;struct _dynforward_ptr _tmp509;(_tmp509=_dynforward_ptr_plus(t,sizeof(char),
j ++),((_tmp50A=*((char*)_check_dynforward_subscript(_tmp509,sizeof(char),0)),((
_tmp50B='a',((_get_dynforward_size(_tmp509,sizeof(char))== 1  && (_tmp50A == '\000'
 && _tmp50B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp509.curr)=_tmp50B)))))));}
goto _LL21;_LL24: if(_tmp1E != '\b')goto _LL26;_LL25:{char _tmp50E;char _tmp50D;struct
_dynforward_ptr _tmp50C;(_tmp50C=_dynforward_ptr_plus(t,sizeof(char),j ++),((
_tmp50D=*((char*)_check_dynforward_subscript(_tmp50C,sizeof(char),0)),((_tmp50E='\\',((
_get_dynforward_size(_tmp50C,sizeof(char))== 1  && (_tmp50D == '\000'  && _tmp50E != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp50C.curr)=_tmp50E)))))));}{char _tmp511;char
_tmp510;struct _dynforward_ptr _tmp50F;(_tmp50F=_dynforward_ptr_plus(t,sizeof(char),
j ++),((_tmp510=*((char*)_check_dynforward_subscript(_tmp50F,sizeof(char),0)),((
_tmp511='b',((_get_dynforward_size(_tmp50F,sizeof(char))== 1  && (_tmp510 == '\000'
 && _tmp511 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp50F.curr)=_tmp511)))))));}
goto _LL21;_LL26: if(_tmp1E != '\f')goto _LL28;_LL27:{char _tmp514;char _tmp513;struct
_dynforward_ptr _tmp512;(_tmp512=_dynforward_ptr_plus(t,sizeof(char),j ++),((
_tmp513=*((char*)_check_dynforward_subscript(_tmp512,sizeof(char),0)),((_tmp514='\\',((
_get_dynforward_size(_tmp512,sizeof(char))== 1  && (_tmp513 == '\000'  && _tmp514 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp512.curr)=_tmp514)))))));}{char _tmp517;char
_tmp516;struct _dynforward_ptr _tmp515;(_tmp515=_dynforward_ptr_plus(t,sizeof(char),
j ++),((_tmp516=*((char*)_check_dynforward_subscript(_tmp515,sizeof(char),0)),((
_tmp517='f',((_get_dynforward_size(_tmp515,sizeof(char))== 1  && (_tmp516 == '\000'
 && _tmp517 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp515.curr)=_tmp517)))))));}
goto _LL21;_LL28: if(_tmp1E != '\n')goto _LL2A;_LL29:{char _tmp51A;char _tmp519;struct
_dynforward_ptr _tmp518;(_tmp518=_dynforward_ptr_plus(t,sizeof(char),j ++),((
_tmp519=*((char*)_check_dynforward_subscript(_tmp518,sizeof(char),0)),((_tmp51A='\\',((
_get_dynforward_size(_tmp518,sizeof(char))== 1  && (_tmp519 == '\000'  && _tmp51A != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp518.curr)=_tmp51A)))))));}{char _tmp51D;char
_tmp51C;struct _dynforward_ptr _tmp51B;(_tmp51B=_dynforward_ptr_plus(t,sizeof(char),
j ++),((_tmp51C=*((char*)_check_dynforward_subscript(_tmp51B,sizeof(char),0)),((
_tmp51D='n',((_get_dynforward_size(_tmp51B,sizeof(char))== 1  && (_tmp51C == '\000'
 && _tmp51D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp51B.curr)=_tmp51D)))))));}
goto _LL21;_LL2A: if(_tmp1E != '\r')goto _LL2C;_LL2B:{char _tmp520;char _tmp51F;struct
_dynforward_ptr _tmp51E;(_tmp51E=_dynforward_ptr_plus(t,sizeof(char),j ++),((
_tmp51F=*((char*)_check_dynforward_subscript(_tmp51E,sizeof(char),0)),((_tmp520='\\',((
_get_dynforward_size(_tmp51E,sizeof(char))== 1  && (_tmp51F == '\000'  && _tmp520 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp51E.curr)=_tmp520)))))));}{char _tmp523;char
_tmp522;struct _dynforward_ptr _tmp521;(_tmp521=_dynforward_ptr_plus(t,sizeof(char),
j ++),((_tmp522=*((char*)_check_dynforward_subscript(_tmp521,sizeof(char),0)),((
_tmp523='r',((_get_dynforward_size(_tmp521,sizeof(char))== 1  && (_tmp522 == '\000'
 && _tmp523 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp521.curr)=_tmp523)))))));}
goto _LL21;_LL2C: if(_tmp1E != '\t')goto _LL2E;_LL2D:{char _tmp526;char _tmp525;struct
_dynforward_ptr _tmp524;(_tmp524=_dynforward_ptr_plus(t,sizeof(char),j ++),((
_tmp525=*((char*)_check_dynforward_subscript(_tmp524,sizeof(char),0)),((_tmp526='\\',((
_get_dynforward_size(_tmp524,sizeof(char))== 1  && (_tmp525 == '\000'  && _tmp526 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp524.curr)=_tmp526)))))));}{char _tmp529;char
_tmp528;struct _dynforward_ptr _tmp527;(_tmp527=_dynforward_ptr_plus(t,sizeof(char),
j ++),((_tmp528=*((char*)_check_dynforward_subscript(_tmp527,sizeof(char),0)),((
_tmp529='t',((_get_dynforward_size(_tmp527,sizeof(char))== 1  && (_tmp528 == '\000'
 && _tmp529 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp527.curr)=_tmp529)))))));}
goto _LL21;_LL2E: if(_tmp1E != '\v')goto _LL30;_LL2F:{char _tmp52C;char _tmp52B;struct
_dynforward_ptr _tmp52A;(_tmp52A=_dynforward_ptr_plus(t,sizeof(char),j ++),((
_tmp52B=*((char*)_check_dynforward_subscript(_tmp52A,sizeof(char),0)),((_tmp52C='\\',((
_get_dynforward_size(_tmp52A,sizeof(char))== 1  && (_tmp52B == '\000'  && _tmp52C != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp52A.curr)=_tmp52C)))))));}{char _tmp52F;char
_tmp52E;struct _dynforward_ptr _tmp52D;(_tmp52D=_dynforward_ptr_plus(t,sizeof(char),
j ++),((_tmp52E=*((char*)_check_dynforward_subscript(_tmp52D,sizeof(char),0)),((
_tmp52F='v',((_get_dynforward_size(_tmp52D,sizeof(char))== 1  && (_tmp52E == '\000'
 && _tmp52F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp52D.curr)=_tmp52F)))))));}
goto _LL21;_LL30: if(_tmp1E != '\\')goto _LL32;_LL31:{char _tmp532;char _tmp531;struct
_dynforward_ptr _tmp530;(_tmp530=_dynforward_ptr_plus(t,sizeof(char),j ++),((
_tmp531=*((char*)_check_dynforward_subscript(_tmp530,sizeof(char),0)),((_tmp532='\\',((
_get_dynforward_size(_tmp530,sizeof(char))== 1  && (_tmp531 == '\000'  && _tmp532 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp530.curr)=_tmp532)))))));}{char _tmp535;char
_tmp534;struct _dynforward_ptr _tmp533;(_tmp533=_dynforward_ptr_plus(t,sizeof(char),
j ++),((_tmp534=*((char*)_check_dynforward_subscript(_tmp533,sizeof(char),0)),((
_tmp535='\\',((_get_dynforward_size(_tmp533,sizeof(char))== 1  && (_tmp534 == '\000'
 && _tmp535 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp533.curr)=_tmp535)))))));}
goto _LL21;_LL32: if(_tmp1E != '"')goto _LL34;_LL33:{char _tmp538;char _tmp537;struct
_dynforward_ptr _tmp536;(_tmp536=_dynforward_ptr_plus(t,sizeof(char),j ++),((
_tmp537=*((char*)_check_dynforward_subscript(_tmp536,sizeof(char),0)),((_tmp538='\\',((
_get_dynforward_size(_tmp536,sizeof(char))== 1  && (_tmp537 == '\000'  && _tmp538 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp536.curr)=_tmp538)))))));}{char _tmp53B;char
_tmp53A;struct _dynforward_ptr _tmp539;(_tmp539=_dynforward_ptr_plus(t,sizeof(char),
j ++),((_tmp53A=*((char*)_check_dynforward_subscript(_tmp539,sizeof(char),0)),((
_tmp53B='"',((_get_dynforward_size(_tmp539,sizeof(char))== 1  && (_tmp53A == '\000'
 && _tmp53B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp539.curr)=_tmp53B)))))));}
goto _LL21;_LL34:;_LL35: if(_tmp1E >= ' '  && _tmp1E <= '~'){char _tmp53E;char _tmp53D;
struct _dynforward_ptr _tmp53C;(_tmp53C=_dynforward_ptr_plus(t,sizeof(char),j ++),((
_tmp53D=*((char*)_check_dynforward_subscript(_tmp53C,sizeof(char),0)),((_tmp53E=
_tmp1E,((_get_dynforward_size(_tmp53C,sizeof(char))== 1  && (_tmp53D == '\000'  && 
_tmp53E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp53C.curr)=_tmp53E)))))));}
else{{char _tmp541;char _tmp540;struct _dynforward_ptr _tmp53F;(_tmp53F=
_dynforward_ptr_plus(t,sizeof(char),j ++),((_tmp540=*((char*)
_check_dynforward_subscript(_tmp53F,sizeof(char),0)),((_tmp541='\\',((
_get_dynforward_size(_tmp53F,sizeof(char))== 1  && (_tmp540 == '\000'  && _tmp541 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp53F.curr)=_tmp541)))))));}{char _tmp544;char
_tmp543;struct _dynforward_ptr _tmp542;(_tmp542=_dynforward_ptr_plus(t,sizeof(char),
j ++),((_tmp543=*((char*)_check_dynforward_subscript(_tmp542,sizeof(char),0)),((
_tmp544=(char)('0' + (_tmp1E >> 6 & 7)),((_get_dynforward_size(_tmp542,sizeof(char))
== 1  && (_tmp543 == '\000'  && _tmp544 != '\000')?_throw_arraybounds(): 1,*((char*)
_tmp542.curr)=_tmp544)))))));}{char _tmp547;char _tmp546;struct _dynforward_ptr
_tmp545;(_tmp545=_dynforward_ptr_plus(t,sizeof(char),j ++),((_tmp546=*((char*)
_check_dynforward_subscript(_tmp545,sizeof(char),0)),((_tmp547=(char)('0' + (
_tmp1E >> 3 & 7)),((_get_dynforward_size(_tmp545,sizeof(char))== 1  && (_tmp546 == '\000'
 && _tmp547 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp545.curr)=_tmp547)))))));}{
char _tmp54A;char _tmp549;struct _dynforward_ptr _tmp548;(_tmp548=
_dynforward_ptr_plus(t,sizeof(char),j ++),((_tmp549=*((char*)
_check_dynforward_subscript(_tmp548,sizeof(char),0)),((_tmp54A=(char)('0' + (
_tmp1E & 7)),((_get_dynforward_size(_tmp548,sizeof(char))== 1  && (_tmp549 == '\000'
 && _tmp54A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp548.curr)=_tmp54A)))))));}}
goto _LL21;_LL21:;}}return(struct _dynforward_ptr)t;}}}}static char _tmp64[9]="restrict";
static struct _dynforward_ptr Cyc_Absynpp_restrict_string={_tmp64,_tmp64 + 9};static
char _tmp65[9]="volatile";static struct _dynforward_ptr Cyc_Absynpp_volatile_string={
_tmp65,_tmp65 + 9};static char _tmp66[6]="const";static struct _dynforward_ptr Cyc_Absynpp_const_str={
_tmp66,_tmp66 + 6};static struct _dynforward_ptr*Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
static struct _dynforward_ptr*Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _dynforward_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;struct
Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq);struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(
struct Cyc_Absyn_Tqual tq){struct Cyc_List_List*l=0;if(tq.q_restrict){struct Cyc_List_List*
_tmp54B;l=((_tmp54B=_cycalloc(sizeof(*_tmp54B)),((_tmp54B->hd=Cyc_Absynpp_restrict_sp,((
_tmp54B->tl=l,_tmp54B))))));}if(tq.q_volatile){struct Cyc_List_List*_tmp54C;l=((
_tmp54C=_cycalloc(sizeof(*_tmp54C)),((_tmp54C->hd=Cyc_Absynpp_volatile_sp,((
_tmp54C->tl=l,_tmp54C))))));}if(tq.print_const){struct Cyc_List_List*_tmp54D;l=((
_tmp54D=_cycalloc(sizeof(*_tmp54D)),((_tmp54D->hd=Cyc_Absynpp_const_sp,((_tmp54D->tl=
l,_tmp54D))))));}{const char*_tmp550;const char*_tmp54F;const char*_tmp54E;return
Cyc_PP_egroup(((_tmp54E="",_tag_dynforward(_tmp54E,sizeof(char),
_get_zero_arr_size_char(_tmp54E,1)))),((_tmp54F=" ",_tag_dynforward(_tmp54F,
sizeof(char),_get_zero_arr_size_char(_tmp54F,2)))),((_tmp550=" ",_tag_dynforward(
_tmp550,sizeof(char),_get_zero_arr_size_char(_tmp550,2)))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,l));}}struct _dynforward_ptr Cyc_Absynpp_kind2string(void*k);struct
_dynforward_ptr Cyc_Absynpp_kind2string(void*k){void*_tmp6D=k;_LL37: if((int)
_tmp6D != 0)goto _LL39;_LL38: {const char*_tmp551;return(_tmp551="A",
_tag_dynforward(_tmp551,sizeof(char),_get_zero_arr_size_char(_tmp551,2)));}_LL39:
if((int)_tmp6D != 1)goto _LL3B;_LL3A: {const char*_tmp552;return(_tmp552="M",
_tag_dynforward(_tmp552,sizeof(char),_get_zero_arr_size_char(_tmp552,2)));}_LL3B:
if((int)_tmp6D != 2)goto _LL3D;_LL3C: {const char*_tmp553;return(_tmp553="B",
_tag_dynforward(_tmp553,sizeof(char),_get_zero_arr_size_char(_tmp553,2)));}_LL3D:
if((int)_tmp6D != 3)goto _LL3F;_LL3E: {const char*_tmp554;return(_tmp554="R",
_tag_dynforward(_tmp554,sizeof(char),_get_zero_arr_size_char(_tmp554,2)));}_LL3F:
if((int)_tmp6D != 4)goto _LL41;_LL40: {const char*_tmp555;return(_tmp555="UR",
_tag_dynforward(_tmp555,sizeof(char),_get_zero_arr_size_char(_tmp555,3)));}_LL41:
if((int)_tmp6D != 5)goto _LL43;_LL42: {const char*_tmp556;return(_tmp556="TR",
_tag_dynforward(_tmp556,sizeof(char),_get_zero_arr_size_char(_tmp556,3)));}_LL43:
if((int)_tmp6D != 6)goto _LL45;_LL44: {const char*_tmp557;return(_tmp557="E",
_tag_dynforward(_tmp557,sizeof(char),_get_zero_arr_size_char(_tmp557,2)));}_LL45:
if((int)_tmp6D != 7)goto _LL36;_LL46: {const char*_tmp558;return(_tmp558="I",
_tag_dynforward(_tmp558,sizeof(char),_get_zero_arr_size_char(_tmp558,2)));}_LL36:;}
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(void*k);struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(
void*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}struct _dynforward_ptr Cyc_Absynpp_kindbound2string(
void*c);struct _dynforward_ptr Cyc_Absynpp_kindbound2string(void*c){void*_tmp76=
Cyc_Absyn_compress_kb(c);void*_tmp77;void*_tmp78;_LL48: if(*((int*)_tmp76)!= 0)
goto _LL4A;_tmp77=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp76)->f1;_LL49: return
Cyc_Absynpp_kind2string(_tmp77);_LL4A: if(*((int*)_tmp76)!= 1)goto _LL4C;_LL4B: if(
Cyc_PP_tex_output){const char*_tmp559;return(_tmp559="{?}",_tag_dynforward(
_tmp559,sizeof(char),_get_zero_arr_size_char(_tmp559,4)));}else{const char*
_tmp55A;return(_tmp55A="?",_tag_dynforward(_tmp55A,sizeof(char),
_get_zero_arr_size_char(_tmp55A,2)));}_LL4C: if(*((int*)_tmp76)!= 2)goto _LL47;
_tmp78=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp76)->f2;_LL4D: return Cyc_Absynpp_kind2string(
_tmp78);_LL47:;}struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c);struct Cyc_PP_Doc*
Cyc_Absynpp_kindbound2doc(void*c){void*_tmp7B=Cyc_Absyn_compress_kb(c);void*
_tmp7C;void*_tmp7D;_LL4F: if(*((int*)_tmp7B)!= 0)goto _LL51;_tmp7C=(void*)((struct
Cyc_Absyn_Eq_kb_struct*)_tmp7B)->f1;_LL50: return Cyc_PP_text(Cyc_Absynpp_kind2string(
_tmp7C));_LL51: if(*((int*)_tmp7B)!= 1)goto _LL53;_LL52: if(Cyc_PP_tex_output){
const char*_tmp55B;return Cyc_PP_text_width(((_tmp55B="{?}",_tag_dynforward(
_tmp55B,sizeof(char),_get_zero_arr_size_char(_tmp55B,4)))),1);}else{const char*
_tmp55C;return Cyc_PP_text(((_tmp55C="?",_tag_dynforward(_tmp55C,sizeof(char),
_get_zero_arr_size_char(_tmp55C,2)))));}_LL53: if(*((int*)_tmp7B)!= 2)goto _LL4E;
_tmp7D=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp7B)->f2;_LL54: return Cyc_PP_text(
Cyc_Absynpp_kind2string(_tmp7D));_LL4E:;}struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(
struct Cyc_List_List*ts);struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*
ts){const char*_tmp55F;const char*_tmp55E;const char*_tmp55D;return Cyc_PP_egroup(((
_tmp55D="<",_tag_dynforward(_tmp55D,sizeof(char),_get_zero_arr_size_char(_tmp55D,
2)))),((_tmp55E=">",_tag_dynforward(_tmp55E,sizeof(char),_get_zero_arr_size_char(
_tmp55E,2)))),((_tmp55F=",",_tag_dynforward(_tmp55F,sizeof(char),
_get_zero_arr_size_char(_tmp55F,2)))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));}struct
Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv);struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(
struct Cyc_Absyn_Tvar*tv){void*_tmp83=Cyc_Absyn_compress_kb((void*)tv->kind);void*
_tmp84;void*_tmp85;void*_tmp86;_LL56: if(*((int*)_tmp83)!= 1)goto _LL58;_LL57: goto
_LL59;_LL58: if(*((int*)_tmp83)!= 0)goto _LL5A;_tmp84=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp83)->f1;if((int)_tmp84 != 2)goto _LL5A;_LL59: return Cyc_PP_textptr(tv->name);
_LL5A: if(*((int*)_tmp83)!= 2)goto _LL5C;_tmp85=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp83)->f2;_LL5B: _tmp86=_tmp85;goto _LL5D;_LL5C: if(*((int*)_tmp83)!= 0)goto _LL55;
_tmp86=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp83)->f1;_LL5D: {const char*
_tmp562;struct Cyc_PP_Doc*_tmp561[3];return(_tmp561[2]=Cyc_Absynpp_kind2doc(
_tmp86),((_tmp561[1]=Cyc_PP_text(((_tmp562="::",_tag_dynforward(_tmp562,sizeof(
char),_get_zero_arr_size_char(_tmp562,3))))),((_tmp561[0]=Cyc_PP_textptr(tv->name),
Cyc_PP_cat(_tag_dynforward(_tmp561,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL55:;}
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs);struct Cyc_PP_Doc*
Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){const char*_tmp565;const char*
_tmp564;const char*_tmp563;return Cyc_PP_egroup(((_tmp563="<",_tag_dynforward(
_tmp563,sizeof(char),_get_zero_arr_size_char(_tmp563,2)))),((_tmp564=">",
_tag_dynforward(_tmp564,sizeof(char),_get_zero_arr_size_char(_tmp564,2)))),((
_tmp565=",",_tag_dynforward(_tmp565,sizeof(char),_get_zero_arr_size_char(_tmp565,
2)))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));}static struct
_dynforward_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv);static struct
_dynforward_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv){return tv->name;}
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs);struct Cyc_PP_Doc*
Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);{const char*_tmp568;const char*_tmp567;const char*
_tmp566;return Cyc_PP_egroup(((_tmp566="<",_tag_dynforward(_tmp566,sizeof(char),
_get_zero_arr_size_char(_tmp566,2)))),((_tmp567=">",_tag_dynforward(_tmp567,
sizeof(char),_get_zero_arr_size_char(_tmp567,2)))),((_tmp568=",",_tag_dynforward(
_tmp568,sizeof(char),_get_zero_arr_size_char(_tmp568,2)))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dynforward_ptr*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,tvs)));}}struct
_tuple10{struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(
struct _tuple10*t);struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple10*t){return
Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(
struct Cyc_List_List*ts);struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*
ts){const char*_tmp56B;const char*_tmp56A;const char*_tmp569;return Cyc_PP_group(((
_tmp569="(",_tag_dynforward(_tmp569,sizeof(char),_get_zero_arr_size_char(_tmp569,
2)))),((_tmp56A=")",_tag_dynforward(_tmp56A,sizeof(char),_get_zero_arr_size_char(
_tmp56A,2)))),((_tmp56B=",",_tag_dynforward(_tmp56B,sizeof(char),
_get_zero_arr_size_char(_tmp56B,2)))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att);struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(
void*att){void*_tmp92=att;_LL5F: if((int)_tmp92 != 0)goto _LL61;_LL60: goto _LL62;
_LL61: if((int)_tmp92 != 1)goto _LL63;_LL62: goto _LL64;_LL63: if((int)_tmp92 != 2)goto
_LL65;_LL64: return Cyc_PP_nil_doc();_LL65:;_LL66: return Cyc_PP_text(Cyc_Absyn_attribute2string(
att));_LL5E:;}struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts);
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){for(0;atts != 
0;atts=atts->tl){void*_tmp93=(void*)atts->hd;_LL68: if((int)_tmp93 != 0)goto _LL6A;
_LL69: {const char*_tmp56C;return Cyc_PP_text(((_tmp56C=" _stdcall ",
_tag_dynforward(_tmp56C,sizeof(char),_get_zero_arr_size_char(_tmp56C,11)))));}
_LL6A: if((int)_tmp93 != 1)goto _LL6C;_LL6B: {const char*_tmp56D;return Cyc_PP_text(((
_tmp56D=" _cdecl ",_tag_dynforward(_tmp56D,sizeof(char),_get_zero_arr_size_char(
_tmp56D,9)))));}_LL6C: if((int)_tmp93 != 2)goto _LL6E;_LL6D: {const char*_tmp56E;
return Cyc_PP_text(((_tmp56E=" _fastcall ",_tag_dynforward(_tmp56E,sizeof(char),
_get_zero_arr_size_char(_tmp56E,12)))));}_LL6E:;_LL6F: goto _LL67;_LL67:;}return
Cyc_PP_nil_doc();}struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*
atts);struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){int
hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){void*
_tmp97=(void*)atts2->hd;_LL71: if((int)_tmp97 != 0)goto _LL73;_LL72: goto _LL74;_LL73:
if((int)_tmp97 != 1)goto _LL75;_LL74: goto _LL76;_LL75: if((int)_tmp97 != 2)goto _LL77;
_LL76: goto _LL70;_LL77:;_LL78: hasatt=1;goto _LL70;_LL70:;}}if(!hasatt)return Cyc_PP_nil_doc();{
const char*_tmp579;const char*_tmp578;const char*_tmp577;const char*_tmp576;const
char*_tmp575;struct Cyc_PP_Doc*_tmp574[3];return(_tmp574[2]=Cyc_PP_text(((_tmp579=")",
_tag_dynforward(_tmp579,sizeof(char),_get_zero_arr_size_char(_tmp579,2))))),((
_tmp574[1]=Cyc_PP_group(((_tmp578="",_tag_dynforward(_tmp578,sizeof(char),
_get_zero_arr_size_char(_tmp578,1)))),((_tmp577="",_tag_dynforward(_tmp577,
sizeof(char),_get_zero_arr_size_char(_tmp577,1)))),((_tmp576=" ",_tag_dynforward(
_tmp576,sizeof(char),_get_zero_arr_size_char(_tmp576,2)))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,
atts)),((_tmp574[0]=Cyc_PP_text(((_tmp575=" __declspec(",_tag_dynforward(_tmp575,
sizeof(char),_get_zero_arr_size_char(_tmp575,13))))),Cyc_PP_cat(_tag_dynforward(
_tmp574,sizeof(struct Cyc_PP_Doc*),3)))))));}}struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(
void*a);struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){return Cyc_PP_text(Cyc_Absyn_attribute2string(
a));}struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts);struct Cyc_PP_Doc*
Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){if(atts == 0)return Cyc_PP_nil_doc();{
void*_tmp9E=Cyc_Cyclone_c_compiler;_LL7A: if((int)_tmp9E != 1)goto _LL7C;_LL7B:
return Cyc_Absynpp_noncallconv2doc(atts);_LL7C: if((int)_tmp9E != 0)goto _LL79;_LL7D: {
const char*_tmp582;const char*_tmp581;const char*_tmp580;const char*_tmp57F;struct
Cyc_PP_Doc*_tmp57E[2];return(_tmp57E[1]=Cyc_PP_group(((_tmp582="((",
_tag_dynforward(_tmp582,sizeof(char),_get_zero_arr_size_char(_tmp582,3)))),((
_tmp581="))",_tag_dynforward(_tmp581,sizeof(char),_get_zero_arr_size_char(
_tmp581,3)))),((_tmp580=",",_tag_dynforward(_tmp580,sizeof(char),
_get_zero_arr_size_char(_tmp580,2)))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts)),((
_tmp57E[0]=Cyc_PP_text(((_tmp57F=" __attribute__",_tag_dynforward(_tmp57F,
sizeof(char),_get_zero_arr_size_char(_tmp57F,15))))),Cyc_PP_cat(_tag_dynforward(
_tmp57E,sizeof(struct Cyc_PP_Doc*),2)))));}_LL79:;}}int Cyc_Absynpp_next_is_pointer(
struct Cyc_List_List*tms);int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{void*_tmpA4=(void*)tms->hd;_LL7F: if(*((int*)_tmpA4)!= 2)goto
_LL81;_LL80: return 1;_LL81: if(*((int*)_tmpA4)!= 5)goto _LL83;_LL82: {void*_tmpA5=
Cyc_Cyclone_c_compiler;_LL86: if((int)_tmpA5 != 0)goto _LL88;_LL87: return 0;_LL88:;
_LL89: return Cyc_Absynpp_next_is_pointer(tms->tl);_LL85:;}_LL83:;_LL84: return 0;
_LL7E:;}}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);static struct Cyc_PP_Doc*
Cyc_Absynpp_cache_question=0;static struct Cyc_PP_Doc*Cyc_Absynpp_question();
static struct Cyc_PP_Doc*Cyc_Absynpp_question(){if(!((unsigned int)Cyc_Absynpp_cache_question)){
if(Cyc_PP_tex_output){const char*_tmp583;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)
Cyc_PP_text_width(((_tmp583="{?}",_tag_dynforward(_tmp583,sizeof(char),
_get_zero_arr_size_char(_tmp583,4)))),1);}else{const char*_tmp584;Cyc_Absynpp_cache_question=(
struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp584="?",_tag_dynforward(_tmp584,sizeof(char),
_get_zero_arr_size_char(_tmp584,2)))));}}return(struct Cyc_PP_Doc*)_check_null(
Cyc_Absynpp_cache_question);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb();static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){if(Cyc_PP_tex_output){const char*
_tmp585;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp585="{\\lb}",
_tag_dynforward(_tmp585,sizeof(char),_get_zero_arr_size_char(_tmp585,6)))),1);}
else{const char*_tmp586;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text(((
_tmp586="{",_tag_dynforward(_tmp586,sizeof(char),_get_zero_arr_size_char(_tmp586,
2)))));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}static
struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;static struct Cyc_PP_Doc*Cyc_Absynpp_rb();
static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){if(!((unsigned int)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output){const char*_tmp587;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)
Cyc_PP_text_width(((_tmp587="{\\rb}",_tag_dynforward(_tmp587,sizeof(char),
_get_zero_arr_size_char(_tmp587,6)))),1);}else{const char*_tmp588;Cyc_Absynpp_cache_rb=(
struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp588="}",_tag_dynforward(_tmp588,sizeof(char),
_get_zero_arr_size_char(_tmp588,2)))));}}return(struct Cyc_PP_Doc*)_check_null(
Cyc_Absynpp_cache_rb);}static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;static
struct Cyc_PP_Doc*Cyc_Absynpp_dollar();static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){if(Cyc_PP_tex_output){const char*
_tmp589;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp589="\\$",
_tag_dynforward(_tmp589,sizeof(char),_get_zero_arr_size_char(_tmp589,3)))),1);}
else{const char*_tmp58A;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text(((
_tmp58A="$",_tag_dynforward(_tmp58A,sizeof(char),_get_zero_arr_size_char(_tmp58A,
2)))));}}return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}struct
Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dynforward_ptr sep,struct Cyc_List_List*
ss);struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dynforward_ptr sep,struct Cyc_List_List*
ss){struct Cyc_PP_Doc*_tmp58B[3];return(_tmp58B[2]=Cyc_Absynpp_rb(),((_tmp58B[1]=
Cyc_PP_seq(sep,ss),((_tmp58B[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dynforward(
_tmp58B,sizeof(struct Cyc_PP_Doc*),3)))))));}static void Cyc_Absynpp_print_tms(
struct Cyc_List_List*tms);static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){void*_tmpAF=(void*)tms->hd;void*_tmpB0;struct Cyc_List_List*
_tmpB1;_LL8B: if(*((int*)_tmpAF)!= 0)goto _LL8D;_LL8C:{const char*_tmp58E;void*
_tmp58D;(_tmp58D=0,Cyc_fprintf(Cyc_stderr,((_tmp58E="Carray_mod ",
_tag_dynforward(_tmp58E,sizeof(char),_get_zero_arr_size_char(_tmp58E,12)))),
_tag_dynforward(_tmp58D,sizeof(void*),0)));}goto _LL8A;_LL8D: if(*((int*)_tmpAF)!= 
1)goto _LL8F;_LL8E:{const char*_tmp591;void*_tmp590;(_tmp590=0,Cyc_fprintf(Cyc_stderr,((
_tmp591="ConstArray_mod ",_tag_dynforward(_tmp591,sizeof(char),
_get_zero_arr_size_char(_tmp591,16)))),_tag_dynforward(_tmp590,sizeof(void*),0)));}
goto _LL8A;_LL8F: if(*((int*)_tmpAF)!= 3)goto _LL91;_tmpB0=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmpAF)->f1;if(*((int*)_tmpB0)!= 1)goto _LL91;_tmpB1=((struct Cyc_Absyn_WithTypes_struct*)
_tmpB0)->f1;_LL90:{const char*_tmp594;void*_tmp593;(_tmp593=0,Cyc_fprintf(Cyc_stderr,((
_tmp594="Function_mod(",_tag_dynforward(_tmp594,sizeof(char),
_get_zero_arr_size_char(_tmp594,14)))),_tag_dynforward(_tmp593,sizeof(void*),0)));}
for(0;_tmpB1 != 0;_tmpB1=_tmpB1->tl){struct Cyc_Core_Opt*_tmpB8=(*((struct _tuple1*)
_tmpB1->hd)).f1;if(_tmpB8 == 0){const char*_tmp597;void*_tmp596;(_tmp596=0,Cyc_fprintf(
Cyc_stderr,((_tmp597="?",_tag_dynforward(_tmp597,sizeof(char),
_get_zero_arr_size_char(_tmp597,2)))),_tag_dynforward(_tmp596,sizeof(void*),0)));}
else{void*_tmp598;(_tmp598=0,Cyc_fprintf(Cyc_stderr,*((struct _dynforward_ptr*)
_tmpB8->v),_tag_dynforward(_tmp598,sizeof(void*),0)));}if(_tmpB1->tl != 0){const
char*_tmp59B;void*_tmp59A;(_tmp59A=0,Cyc_fprintf(Cyc_stderr,((_tmp59B=",",
_tag_dynforward(_tmp59B,sizeof(char),_get_zero_arr_size_char(_tmp59B,2)))),
_tag_dynforward(_tmp59A,sizeof(void*),0)));}}{const char*_tmp59E;void*_tmp59D;(
_tmp59D=0,Cyc_fprintf(Cyc_stderr,((_tmp59E=") ",_tag_dynforward(_tmp59E,sizeof(
char),_get_zero_arr_size_char(_tmp59E,3)))),_tag_dynforward(_tmp59D,sizeof(void*),
0)));}goto _LL8A;_LL91: if(*((int*)_tmpAF)!= 3)goto _LL93;_LL92:{const char*_tmp5A1;
void*_tmp5A0;(_tmp5A0=0,Cyc_fprintf(Cyc_stderr,((_tmp5A1="Function_mod()",
_tag_dynforward(_tmp5A1,sizeof(char),_get_zero_arr_size_char(_tmp5A1,15)))),
_tag_dynforward(_tmp5A0,sizeof(void*),0)));}goto _LL8A;_LL93: if(*((int*)_tmpAF)!= 
5)goto _LL95;_LL94:{const char*_tmp5A4;void*_tmp5A3;(_tmp5A3=0,Cyc_fprintf(Cyc_stderr,((
_tmp5A4="Attributes_mod ",_tag_dynforward(_tmp5A4,sizeof(char),
_get_zero_arr_size_char(_tmp5A4,16)))),_tag_dynforward(_tmp5A3,sizeof(void*),0)));}
goto _LL8A;_LL95: if(*((int*)_tmpAF)!= 4)goto _LL97;_LL96:{const char*_tmp5A7;void*
_tmp5A6;(_tmp5A6=0,Cyc_fprintf(Cyc_stderr,((_tmp5A7="TypeParams_mod ",
_tag_dynforward(_tmp5A7,sizeof(char),_get_zero_arr_size_char(_tmp5A7,16)))),
_tag_dynforward(_tmp5A6,sizeof(void*),0)));}goto _LL8A;_LL97: if(*((int*)_tmpAF)!= 
2)goto _LL8A;_LL98:{const char*_tmp5AA;void*_tmp5A9;(_tmp5A9=0,Cyc_fprintf(Cyc_stderr,((
_tmp5AA="Pointer_mod ",_tag_dynforward(_tmp5AA,sizeof(char),
_get_zero_arr_size_char(_tmp5AA,13)))),_tag_dynforward(_tmp5A9,sizeof(void*),0)));}
goto _LL8A;_LL8A:;}{const char*_tmp5AD;void*_tmp5AC;(_tmp5AC=0,Cyc_fprintf(Cyc_stderr,((
_tmp5AD="\n",_tag_dynforward(_tmp5AD,sizeof(char),_get_zero_arr_size_char(
_tmp5AD,2)))),_tag_dynforward(_tmp5AC,sizeof(void*),0)));}}struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(
int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms);struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(
int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);const char*_tmp5B2;const
char*_tmp5B1;struct Cyc_PP_Doc*_tmp5B0[3];struct Cyc_PP_Doc*p_rest=(_tmp5B0[2]=Cyc_PP_text(((
_tmp5B2=")",_tag_dynforward(_tmp5B2,sizeof(char),_get_zero_arr_size_char(_tmp5B2,
2))))),((_tmp5B0[1]=rest,((_tmp5B0[0]=Cyc_PP_text(((_tmp5B1="(",_tag_dynforward(
_tmp5B1,sizeof(char),_get_zero_arr_size_char(_tmp5B1,2))))),Cyc_PP_cat(
_tag_dynforward(_tmp5B0,sizeof(struct Cyc_PP_Doc*),3)))))));void*_tmpCA=(void*)
tms->hd;struct Cyc_Absyn_Conref*_tmpCB;struct Cyc_Absyn_Exp*_tmpCC;struct Cyc_Absyn_Conref*
_tmpCD;void*_tmpCE;struct Cyc_List_List*_tmpCF;struct Cyc_List_List*_tmpD0;struct
Cyc_Position_Segment*_tmpD1;int _tmpD2;struct Cyc_Absyn_PtrAtts _tmpD3;void*_tmpD4;
struct Cyc_Absyn_Conref*_tmpD5;struct Cyc_Absyn_Conref*_tmpD6;struct Cyc_Absyn_Conref*
_tmpD7;struct Cyc_Absyn_Tqual _tmpD8;_LL9A: if(*((int*)_tmpCA)!= 0)goto _LL9C;_tmpCB=((
struct Cyc_Absyn_Carray_mod_struct*)_tmpCA)->f1;_LL9B: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;{const char*_tmp5B7;const char*_tmp5B6;struct Cyc_PP_Doc*
_tmp5B5[2];return(_tmp5B5[1]=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpCB)?Cyc_PP_text(((_tmp5B6="[]ZEROTERM ",_tag_dynforward(_tmp5B6,sizeof(
char),_get_zero_arr_size_char(_tmp5B6,12))))): Cyc_PP_text(((_tmp5B7="[]",
_tag_dynforward(_tmp5B7,sizeof(char),_get_zero_arr_size_char(_tmp5B7,3))))),((
_tmp5B5[0]=rest,Cyc_PP_cat(_tag_dynforward(_tmp5B5,sizeof(struct Cyc_PP_Doc*),2)))));}
_LL9C: if(*((int*)_tmpCA)!= 1)goto _LL9E;_tmpCC=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpCA)->f1;_tmpCD=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpCA)->f2;_LL9D:
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{const char*_tmp5BE;const char*
_tmp5BD;const char*_tmp5BC;struct Cyc_PP_Doc*_tmp5BB[4];return(_tmp5BB[3]=((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpCD)?Cyc_PP_text(((
_tmp5BD="]ZEROTERM ",_tag_dynforward(_tmp5BD,sizeof(char),
_get_zero_arr_size_char(_tmp5BD,11))))): Cyc_PP_text(((_tmp5BE="]",
_tag_dynforward(_tmp5BE,sizeof(char),_get_zero_arr_size_char(_tmp5BE,2))))),((
_tmp5BB[2]=Cyc_Absynpp_exp2doc(_tmpCC),((_tmp5BB[1]=Cyc_PP_text(((_tmp5BC="[",
_tag_dynforward(_tmp5BC,sizeof(char),_get_zero_arr_size_char(_tmp5BC,2))))),((
_tmp5BB[0]=rest,Cyc_PP_cat(_tag_dynforward(_tmp5BB,sizeof(struct Cyc_PP_Doc*),4)))))))));}
_LL9E: if(*((int*)_tmpCA)!= 3)goto _LLA0;_tmpCE=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmpCA)->f1;_LL9F: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{void*
_tmpE0=_tmpCE;struct Cyc_List_List*_tmpE1;int _tmpE2;struct Cyc_Absyn_VarargInfo*
_tmpE3;struct Cyc_Core_Opt*_tmpE4;struct Cyc_List_List*_tmpE5;struct Cyc_List_List*
_tmpE6;struct Cyc_Position_Segment*_tmpE7;_LLA7: if(*((int*)_tmpE0)!= 1)goto _LLA9;
_tmpE1=((struct Cyc_Absyn_WithTypes_struct*)_tmpE0)->f1;_tmpE2=((struct Cyc_Absyn_WithTypes_struct*)
_tmpE0)->f2;_tmpE3=((struct Cyc_Absyn_WithTypes_struct*)_tmpE0)->f3;_tmpE4=((
struct Cyc_Absyn_WithTypes_struct*)_tmpE0)->f4;_tmpE5=((struct Cyc_Absyn_WithTypes_struct*)
_tmpE0)->f5;_LLA8: {struct Cyc_PP_Doc*_tmp5BF[2];return(_tmp5BF[1]=Cyc_Absynpp_funargs2doc(
_tmpE1,_tmpE2,_tmpE3,_tmpE4,_tmpE5),((_tmp5BF[0]=rest,Cyc_PP_cat(_tag_dynforward(
_tmp5BF,sizeof(struct Cyc_PP_Doc*),2)))));}_LLA9: if(*((int*)_tmpE0)!= 0)goto _LLA6;
_tmpE6=((struct Cyc_Absyn_NoTypes_struct*)_tmpE0)->f1;_tmpE7=((struct Cyc_Absyn_NoTypes_struct*)
_tmpE0)->f2;_LLAA: {const char*_tmp5C6;const char*_tmp5C5;const char*_tmp5C4;struct
Cyc_PP_Doc*_tmp5C3[2];return(_tmp5C3[1]=Cyc_PP_group(((_tmp5C6="(",
_tag_dynforward(_tmp5C6,sizeof(char),_get_zero_arr_size_char(_tmp5C6,2)))),((
_tmp5C5=")",_tag_dynforward(_tmp5C5,sizeof(char),_get_zero_arr_size_char(_tmp5C5,
2)))),((_tmp5C4=",",_tag_dynforward(_tmp5C4,sizeof(char),_get_zero_arr_size_char(
_tmp5C4,2)))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct
_dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmpE6)),((
_tmp5C3[0]=rest,Cyc_PP_cat(_tag_dynforward(_tmp5C3,sizeof(struct Cyc_PP_Doc*),2)))));}
_LLA6:;}_LLA0: if(*((int*)_tmpCA)!= 5)goto _LLA2;_tmpCF=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmpCA)->f2;_LLA1: {void*_tmpED=Cyc_Cyclone_c_compiler;_LLAC: if((int)_tmpED != 0)
goto _LLAE;_LLAD: if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{struct Cyc_PP_Doc*
_tmp5C7[2];return(_tmp5C7[1]=Cyc_Absynpp_atts2doc(_tmpCF),((_tmp5C7[0]=rest,Cyc_PP_cat(
_tag_dynforward(_tmp5C7,sizeof(struct Cyc_PP_Doc*),2)))));}_LLAE: if((int)_tmpED != 
1)goto _LLAB;_LLAF: if(Cyc_Absynpp_next_is_pointer(tms->tl)){struct Cyc_PP_Doc*
_tmp5C8[2];return(_tmp5C8[1]=rest,((_tmp5C8[0]=Cyc_Absynpp_callconv2doc(_tmpCF),
Cyc_PP_cat(_tag_dynforward(_tmp5C8,sizeof(struct Cyc_PP_Doc*),2)))));}return rest;
_LLAB:;}_LLA2: if(*((int*)_tmpCA)!= 4)goto _LLA4;_tmpD0=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmpCA)->f1;_tmpD1=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCA)->f2;_tmpD2=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCA)->f3;_LLA3: if(Cyc_Absynpp_next_is_pointer(
tms->tl))rest=p_rest;if(_tmpD2){struct Cyc_PP_Doc*_tmp5C9[2];return(_tmp5C9[1]=
Cyc_Absynpp_ktvars2doc(_tmpD0),((_tmp5C9[0]=rest,Cyc_PP_cat(_tag_dynforward(
_tmp5C9,sizeof(struct Cyc_PP_Doc*),2)))));}else{struct Cyc_PP_Doc*_tmp5CA[2];
return(_tmp5CA[1]=Cyc_Absynpp_tvars2doc(_tmpD0),((_tmp5CA[0]=rest,Cyc_PP_cat(
_tag_dynforward(_tmp5CA,sizeof(struct Cyc_PP_Doc*),2)))));}_LLA4: if(*((int*)
_tmpCA)!= 2)goto _LL99;_tmpD3=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpCA)->f1;
_tmpD4=(void*)_tmpD3.rgn;_tmpD5=_tmpD3.nullable;_tmpD6=_tmpD3.bounds;_tmpD7=
_tmpD3.zero_term;_tmpD8=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpCA)->f2;_LLA5: {
struct Cyc_PP_Doc*ptr;{void*_tmpF2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmpD6);struct Cyc_Absyn_Exp*_tmpF3;void*_tmpF4;_LLB1: if((int)_tmpF2 != 0)goto
_LLB3;_LLB2: ptr=Cyc_Absynpp_question();goto _LLB0;_LLB3: if((int)_tmpF2 != 1)goto
_LLB5;_LLB4:{const char*_tmp5CD;struct Cyc_PP_Doc*_tmp5CC[2];ptr=((_tmp5CC[1]=Cyc_PP_text(((
_tmp5CD="-",_tag_dynforward(_tmp5CD,sizeof(char),_get_zero_arr_size_char(_tmp5CD,
2))))),((_tmp5CC[0]=Cyc_Absynpp_question(),Cyc_PP_cat(_tag_dynforward(_tmp5CC,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLB0;_LLB5: if(_tmpF2 <= (void*)2)goto
_LLB7;if(*((int*)_tmpF2)!= 0)goto _LLB7;_tmpF3=((struct Cyc_Absyn_Upper_b_struct*)
_tmpF2)->f1;_LLB6:{const char*_tmp5CF;const char*_tmp5CE;ptr=Cyc_PP_text(((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmpD5)?(_tmp5CF="*",
_tag_dynforward(_tmp5CF,sizeof(char),_get_zero_arr_size_char(_tmp5CF,2))):((
_tmp5CE="@",_tag_dynforward(_tmp5CE,sizeof(char),_get_zero_arr_size_char(_tmp5CE,
2)))));}{unsigned int _tmpFA;int _tmpFB;struct _tuple4 _tmpF9=Cyc_Evexp_eval_const_uint_exp(
_tmpF3);_tmpFA=_tmpF9.f1;_tmpFB=_tmpF9.f2;if(!_tmpFB  || _tmpFA != 1){struct Cyc_PP_Doc*
_tmp5D0[4];ptr=((_tmp5D0[3]=Cyc_Absynpp_rb(),((_tmp5D0[2]=Cyc_Absynpp_exp2doc(
_tmpF3),((_tmp5D0[1]=Cyc_Absynpp_lb(),((_tmp5D0[0]=ptr,Cyc_PP_cat(
_tag_dynforward(_tmp5D0,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLB0;}_LLB7:
if(_tmpF2 <= (void*)2)goto _LLB0;if(*((int*)_tmpF2)!= 1)goto _LLB0;_tmpF4=(void*)((
struct Cyc_Absyn_AbsUpper_b_struct*)_tmpF2)->f1;_LLB8:{const char*_tmp5D2;const
char*_tmp5D1;ptr=Cyc_PP_text(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
1,_tmpD5)?(_tmp5D2="*",_tag_dynforward(_tmp5D2,sizeof(char),
_get_zero_arr_size_char(_tmp5D2,2))):((_tmp5D1="@",_tag_dynforward(_tmp5D1,
sizeof(char),_get_zero_arr_size_char(_tmp5D1,2)))));}{struct Cyc_PP_Doc*_tmp5D3[4];
ptr=((_tmp5D3[3]=Cyc_Absynpp_rb(),((_tmp5D3[2]=Cyc_Absynpp_typ2doc(_tmpF4),((
_tmp5D3[1]=Cyc_Absynpp_lb(),((_tmp5D3[0]=ptr,Cyc_PP_cat(_tag_dynforward(_tmp5D3,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLB0;_LLB0:;}if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr  && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpD7)){const char*_tmp5D6;struct Cyc_PP_Doc*_tmp5D5[2];ptr=((_tmp5D5[1]=Cyc_PP_text(((
_tmp5D6="ZEROTERM ",_tag_dynforward(_tmp5D6,sizeof(char),_get_zero_arr_size_char(
_tmp5D6,10))))),((_tmp5D5[0]=ptr,Cyc_PP_cat(_tag_dynforward(_tmp5D5,sizeof(
struct Cyc_PP_Doc*),2))))));}else{if(is_char_ptr  && !((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmpD7)){const char*_tmp5D9;struct Cyc_PP_Doc*_tmp5D8[2];
ptr=((_tmp5D8[1]=Cyc_PP_text(((_tmp5D9="NOZEROTERM ",_tag_dynforward(_tmp5D9,
sizeof(char),_get_zero_arr_size_char(_tmp5D9,12))))),((_tmp5D8[0]=ptr,Cyc_PP_cat(
_tag_dynforward(_tmp5D8,sizeof(struct Cyc_PP_Doc*),2))))));}}}{void*_tmp104=Cyc_Tcutil_compress(
_tmpD4);_LLBA: if((int)_tmp104 != 2)goto _LLBC;_LLBB: goto _LLB9;_LLBC:;_LLBD: {const
char*_tmp5DC;struct Cyc_PP_Doc*_tmp5DB[3];ptr=((_tmp5DB[2]=Cyc_PP_text(((_tmp5DC=" ",
_tag_dynforward(_tmp5DC,sizeof(char),_get_zero_arr_size_char(_tmp5DC,2))))),((
_tmp5DB[1]=Cyc_Absynpp_typ2doc(_tmpD4),((_tmp5DB[0]=ptr,Cyc_PP_cat(
_tag_dynforward(_tmp5DB,sizeof(struct Cyc_PP_Doc*),3))))))));}_LLB9:;}{struct Cyc_PP_Doc*
_tmp5DD[2];ptr=((_tmp5DD[1]=Cyc_Absynpp_tqual2doc(_tmpD8),((_tmp5DD[0]=ptr,Cyc_PP_cat(
_tag_dynforward(_tmp5DD,sizeof(struct Cyc_PP_Doc*),2))))));}{struct Cyc_PP_Doc*
_tmp5DE[2];return(_tmp5DE[1]=rest,((_tmp5DE[0]=ptr,Cyc_PP_cat(_tag_dynforward(
_tmp5DE,sizeof(struct Cyc_PP_Doc*),2)))));}}_LL99:;}}struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){void*_tmp10C=Cyc_Tcutil_compress(
t);_LLBF: if((int)_tmp10C != 2)goto _LLC1;_LLC0: {const char*_tmp5DF;return Cyc_PP_text(((
_tmp5DF="`H",_tag_dynforward(_tmp5DF,sizeof(char),_get_zero_arr_size_char(
_tmp5DF,3)))));}_LLC1: if((int)_tmp10C != 3)goto _LLC3;_LLC2: {const char*_tmp5E0;
return Cyc_PP_text(((_tmp5E0="`U",_tag_dynforward(_tmp5E0,sizeof(char),
_get_zero_arr_size_char(_tmp5E0,3)))));}_LLC3:;_LLC4: return Cyc_Absynpp_ntyp2doc(
t);_LLBE:;}static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct
Cyc_List_List**effects,void*t);static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**
rgions,struct Cyc_List_List**effects,void*t){void*_tmp10F=Cyc_Tcutil_compress(t);
void*_tmp110;struct Cyc_List_List*_tmp111;_LLC6: if(_tmp10F <= (void*)4)goto _LLCA;
if(*((int*)_tmp10F)!= 20)goto _LLC8;_tmp110=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp10F)->f1;_LLC7:{struct Cyc_List_List*_tmp5E1;*rgions=((_tmp5E1=_cycalloc(
sizeof(*_tmp5E1)),((_tmp5E1->hd=Cyc_Absynpp_rgn2doc(_tmp110),((_tmp5E1->tl=*
rgions,_tmp5E1))))));}goto _LLC5;_LLC8: if(*((int*)_tmp10F)!= 21)goto _LLCA;_tmp111=((
struct Cyc_Absyn_JoinEff_struct*)_tmp10F)->f1;_LLC9: for(0;_tmp111 != 0;_tmp111=
_tmp111->tl){Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmp111->hd);}goto
_LLC5;_LLCA:;_LLCB:{struct Cyc_List_List*_tmp5E2;*effects=((_tmp5E2=_cycalloc(
sizeof(*_tmp5E2)),((_tmp5E2->hd=Cyc_Absynpp_typ2doc(t),((_tmp5E2->tl=*effects,
_tmp5E2))))));}goto _LLC5;_LLC5:;}struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t);
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){struct Cyc_List_List*rgions=0;struct
Cyc_List_List*effects=0;Cyc_Absynpp_effects2docs(& rgions,& effects,t);rgions=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);effects=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);if(
rgions == 0  && effects != 0){const char*_tmp5E5;const char*_tmp5E4;const char*_tmp5E3;
return Cyc_PP_group(((_tmp5E3="",_tag_dynforward(_tmp5E3,sizeof(char),
_get_zero_arr_size_char(_tmp5E3,1)))),((_tmp5E4="",_tag_dynforward(_tmp5E4,
sizeof(char),_get_zero_arr_size_char(_tmp5E4,1)))),((_tmp5E5="+",_tag_dynforward(
_tmp5E5,sizeof(char),_get_zero_arr_size_char(_tmp5E5,2)))),effects);}else{const
char*_tmp5E6;struct Cyc_PP_Doc*_tmp117=Cyc_Absynpp_group_braces(((_tmp5E6=",",
_tag_dynforward(_tmp5E6,sizeof(char),_get_zero_arr_size_char(_tmp5E6,2)))),
rgions);struct Cyc_List_List*_tmp5EA;const char*_tmp5E9;const char*_tmp5E8;const
char*_tmp5E7;return Cyc_PP_group(((_tmp5E7="",_tag_dynforward(_tmp5E7,sizeof(char),
_get_zero_arr_size_char(_tmp5E7,1)))),((_tmp5E8="",_tag_dynforward(_tmp5E8,
sizeof(char),_get_zero_arr_size_char(_tmp5E8,1)))),((_tmp5E9="+",_tag_dynforward(
_tmp5E9,sizeof(char),_get_zero_arr_size_char(_tmp5E9,2)))),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((
_tmp5EA=_cycalloc(sizeof(*_tmp5EA)),((_tmp5EA->hd=_tmp117,((_tmp5EA->tl=0,
_tmp5EA))))))));}}struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(void*k);struct Cyc_PP_Doc*
Cyc_Absynpp_aggr_kind2doc(void*k){void*_tmp11D=k;_LLCD: if((int)_tmp11D != 0)goto
_LLCF;_LLCE: {const char*_tmp5EB;return Cyc_PP_text(((_tmp5EB="struct ",
_tag_dynforward(_tmp5EB,sizeof(char),_get_zero_arr_size_char(_tmp5EB,8)))));}
_LLCF: if((int)_tmp11D != 1)goto _LLCC;_LLD0: {const char*_tmp5EC;return Cyc_PP_text(((
_tmp5EC="union ",_tag_dynforward(_tmp5EC,sizeof(char),_get_zero_arr_size_char(
_tmp5EC,7)))));}_LLCC:;}struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);struct Cyc_PP_Doc*
Cyc_Absynpp_ntyp2doc(void*t){struct Cyc_PP_Doc*s;{void*_tmp120=t;struct Cyc_Core_Opt*
_tmp121;struct Cyc_Core_Opt*_tmp122;int _tmp123;struct Cyc_Core_Opt*_tmp124;struct
Cyc_Absyn_Tvar*_tmp125;struct Cyc_Absyn_TunionInfo _tmp126;union Cyc_Absyn_TunionInfoU_union
_tmp127;struct Cyc_List_List*_tmp128;struct Cyc_Core_Opt*_tmp129;struct Cyc_Absyn_TunionFieldInfo
_tmp12A;union Cyc_Absyn_TunionFieldInfoU_union _tmp12B;struct Cyc_List_List*_tmp12C;
void*_tmp12D;void*_tmp12E;int _tmp12F;struct Cyc_List_List*_tmp130;struct Cyc_Absyn_AggrInfo
_tmp131;union Cyc_Absyn_AggrInfoU_union _tmp132;struct Cyc_List_List*_tmp133;void*
_tmp134;struct Cyc_List_List*_tmp135;struct Cyc_List_List*_tmp136;struct _tuple0*
_tmp137;void*_tmp138;struct _tuple0*_tmp139;struct Cyc_List_List*_tmp13A;struct Cyc_Absyn_Typedefdecl*
_tmp13B;void*_tmp13C;void*_tmp13D;void*_tmp13E;int _tmp13F;void*_tmp140;void*
_tmp141;_LLD2: if(_tmp120 <= (void*)4)goto _LLD8;if(*((int*)_tmp120)!= 7)goto _LLD4;
_LLD3: {const char*_tmp5ED;return Cyc_PP_text(((_tmp5ED="[[[array]]]",
_tag_dynforward(_tmp5ED,sizeof(char),_get_zero_arr_size_char(_tmp5ED,12)))));}
_LLD4: if(*((int*)_tmp120)!= 8)goto _LLD6;_LLD5: return Cyc_PP_nil_doc();_LLD6: if(*((
int*)_tmp120)!= 4)goto _LLD8;_LLD7: return Cyc_PP_nil_doc();_LLD8: if((int)_tmp120 != 
0)goto _LLDA;_LLD9:{const char*_tmp5EE;s=Cyc_PP_text(((_tmp5EE="void",
_tag_dynforward(_tmp5EE,sizeof(char),_get_zero_arr_size_char(_tmp5EE,5)))));}
goto _LLD1;_LLDA: if(_tmp120 <= (void*)4)goto _LLE4;if(*((int*)_tmp120)!= 0)goto
_LLDC;_tmp121=((struct Cyc_Absyn_Evar_struct*)_tmp120)->f1;_tmp122=((struct Cyc_Absyn_Evar_struct*)
_tmp120)->f2;_tmp123=((struct Cyc_Absyn_Evar_struct*)_tmp120)->f3;_tmp124=((
struct Cyc_Absyn_Evar_struct*)_tmp120)->f4;_LLDB: if(_tmp122 != 0)return Cyc_Absynpp_ntyp2doc((
void*)_tmp122->v);else{const char*_tmp5FC;struct Cyc_Int_pa_struct _tmp5FB;void*
_tmp5FA[1];const char*_tmp5F9;const char*_tmp5F8;const char*_tmp5F7;struct Cyc_PP_Doc*
_tmp5F6[6];s=((_tmp5F6[5]=_tmp121 == 0?Cyc_Absynpp_question(): Cyc_Absynpp_kind2doc((
void*)_tmp121->v),((_tmp5F6[4]=Cyc_PP_text(((_tmp5FC=")::",_tag_dynforward(
_tmp5FC,sizeof(char),_get_zero_arr_size_char(_tmp5FC,4))))),((_tmp5F6[3]=(!Cyc_Absynpp_print_full_evars
 || _tmp124 == 0)?Cyc_PP_nil_doc(): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)
_tmp124->v),((_tmp5F6[2]=Cyc_PP_text((struct _dynforward_ptr)((_tmp5FB.tag=1,((
_tmp5FB.f1=(unsigned long)_tmp123,((_tmp5FA[0]=& _tmp5FB,Cyc_aprintf(((_tmp5F9="%d",
_tag_dynforward(_tmp5F9,sizeof(char),_get_zero_arr_size_char(_tmp5F9,3)))),
_tag_dynforward(_tmp5FA,sizeof(void*),1))))))))),((_tmp5F6[1]=Cyc_PP_text(((
_tmp5F8="(",_tag_dynforward(_tmp5F8,sizeof(char),_get_zero_arr_size_char(_tmp5F8,
2))))),((_tmp5F6[0]=Cyc_PP_text(((_tmp5F7="%",_tag_dynforward(_tmp5F7,sizeof(
char),_get_zero_arr_size_char(_tmp5F7,2))))),Cyc_PP_cat(_tag_dynforward(_tmp5F6,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));}goto _LLD1;_LLDC: if(*((int*)_tmp120)!= 
1)goto _LLDE;_tmp125=((struct Cyc_Absyn_VarType_struct*)_tmp120)->f1;_LLDD: s=Cyc_PP_textptr(
_tmp125->name);if(Cyc_Absynpp_print_all_kinds){const char*_tmp5FF;struct Cyc_PP_Doc*
_tmp5FE[3];s=((_tmp5FE[2]=Cyc_Absynpp_kindbound2doc((void*)_tmp125->kind),((
_tmp5FE[1]=Cyc_PP_text(((_tmp5FF="::",_tag_dynforward(_tmp5FF,sizeof(char),
_get_zero_arr_size_char(_tmp5FF,3))))),((_tmp5FE[0]=s,Cyc_PP_cat(_tag_dynforward(
_tmp5FE,sizeof(struct Cyc_PP_Doc*),3))))))));}if(Cyc_Absynpp_rewrite_temp_tvars
 && Cyc_Tcutil_is_temp_tvar(_tmp125)){const char*_tmp604;const char*_tmp603;struct
Cyc_PP_Doc*_tmp602[3];s=((_tmp602[2]=Cyc_PP_text(((_tmp604=" */",_tag_dynforward(
_tmp604,sizeof(char),_get_zero_arr_size_char(_tmp604,4))))),((_tmp602[1]=s,((
_tmp602[0]=Cyc_PP_text(((_tmp603="_ /* ",_tag_dynforward(_tmp603,sizeof(char),
_get_zero_arr_size_char(_tmp603,6))))),Cyc_PP_cat(_tag_dynforward(_tmp602,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LLDE: if(*((int*)_tmp120)!= 2)
goto _LLE0;_tmp126=((struct Cyc_Absyn_TunionType_struct*)_tmp120)->f1;_tmp127=
_tmp126.tunion_info;_tmp128=_tmp126.targs;_tmp129=_tmp126.rgn;_LLDF:{union Cyc_Absyn_TunionInfoU_union
_tmp150=_tmp127;struct Cyc_Absyn_UnknownTunionInfo _tmp151;struct _tuple0*_tmp152;
int _tmp153;struct Cyc_Absyn_Tuniondecl**_tmp154;struct Cyc_Absyn_Tuniondecl*
_tmp155;struct Cyc_Absyn_Tuniondecl _tmp156;struct _tuple0*_tmp157;int _tmp158;
_LL109: if((_tmp150.UnknownTunion).tag != 0)goto _LL10B;_tmp151=(_tmp150.UnknownTunion).f1;
_tmp152=_tmp151.name;_tmp153=_tmp151.is_xtunion;_LL10A: _tmp157=_tmp152;_tmp158=
_tmp153;goto _LL10C;_LL10B: if((_tmp150.KnownTunion).tag != 1)goto _LL108;_tmp154=(
_tmp150.KnownTunion).f1;_tmp155=*_tmp154;_tmp156=*_tmp155;_tmp157=_tmp156.name;
_tmp158=_tmp156.is_xtunion;_LL10C: {const char*_tmp606;const char*_tmp605;struct
Cyc_PP_Doc*_tmp159=Cyc_PP_text(_tmp158?(_tmp606="xtunion ",_tag_dynforward(
_tmp606,sizeof(char),_get_zero_arr_size_char(_tmp606,9))):((_tmp605="tunion ",
_tag_dynforward(_tmp605,sizeof(char),_get_zero_arr_size_char(_tmp605,8)))));void*
r=(unsigned int)_tmp129?(void*)_tmp129->v:(void*)2;{void*_tmp15A=Cyc_Tcutil_compress(
r);_LL10E: if((int)_tmp15A != 2)goto _LL110;_LL10F:{struct Cyc_PP_Doc*_tmp607[3];s=((
_tmp607[2]=Cyc_Absynpp_tps2doc(_tmp128),((_tmp607[1]=Cyc_Absynpp_qvar2doc(
_tmp157),((_tmp607[0]=_tmp159,Cyc_PP_cat(_tag_dynforward(_tmp607,sizeof(struct
Cyc_PP_Doc*),3))))))));}goto _LL10D;_LL110:;_LL111:{const char*_tmp60A;struct Cyc_PP_Doc*
_tmp609[5];s=((_tmp609[4]=Cyc_Absynpp_tps2doc(_tmp128),((_tmp609[3]=Cyc_Absynpp_qvar2doc(
_tmp157),((_tmp609[2]=Cyc_PP_text(((_tmp60A=" ",_tag_dynforward(_tmp60A,sizeof(
char),_get_zero_arr_size_char(_tmp60A,2))))),((_tmp609[1]=Cyc_Absynpp_typ2doc(r),((
_tmp609[0]=_tmp159,Cyc_PP_cat(_tag_dynforward(_tmp609,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}goto _LL10D;_LL10D:;}goto _LL108;}_LL108:;}goto _LLD1;_LLE0: if(*((
int*)_tmp120)!= 3)goto _LLE2;_tmp12A=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp120)->f1;_tmp12B=_tmp12A.field_info;_tmp12C=_tmp12A.targs;_LLE1:{union Cyc_Absyn_TunionFieldInfoU_union
_tmp160=_tmp12B;struct Cyc_Absyn_UnknownTunionFieldInfo _tmp161;struct _tuple0*
_tmp162;struct _tuple0*_tmp163;int _tmp164;struct Cyc_Absyn_Tuniondecl*_tmp165;
struct Cyc_Absyn_Tuniondecl _tmp166;struct _tuple0*_tmp167;int _tmp168;struct Cyc_Absyn_Tunionfield*
_tmp169;struct Cyc_Absyn_Tunionfield _tmp16A;struct _tuple0*_tmp16B;_LL113: if((
_tmp160.UnknownTunionfield).tag != 0)goto _LL115;_tmp161=(_tmp160.UnknownTunionfield).f1;
_tmp162=_tmp161.tunion_name;_tmp163=_tmp161.field_name;_tmp164=_tmp161.is_xtunion;
_LL114: _tmp167=_tmp162;_tmp168=_tmp164;_tmp16B=_tmp163;goto _LL116;_LL115: if((
_tmp160.KnownTunionfield).tag != 1)goto _LL112;_tmp165=(_tmp160.KnownTunionfield).f1;
_tmp166=*_tmp165;_tmp167=_tmp166.name;_tmp168=_tmp166.is_xtunion;_tmp169=(
_tmp160.KnownTunionfield).f2;_tmp16A=*_tmp169;_tmp16B=_tmp16A.name;_LL116: {
const char*_tmp60C;const char*_tmp60B;struct Cyc_PP_Doc*_tmp16C=Cyc_PP_text(_tmp168?(
_tmp60C="xtunion ",_tag_dynforward(_tmp60C,sizeof(char),_get_zero_arr_size_char(
_tmp60C,9))):((_tmp60B="tunion ",_tag_dynforward(_tmp60B,sizeof(char),
_get_zero_arr_size_char(_tmp60B,8)))));{const char*_tmp60F;struct Cyc_PP_Doc*
_tmp60E[4];s=((_tmp60E[3]=Cyc_Absynpp_qvar2doc(_tmp16B),((_tmp60E[2]=Cyc_PP_text(((
_tmp60F=".",_tag_dynforward(_tmp60F,sizeof(char),_get_zero_arr_size_char(_tmp60F,
2))))),((_tmp60E[1]=Cyc_Absynpp_qvar2doc(_tmp167),((_tmp60E[0]=_tmp16C,Cyc_PP_cat(
_tag_dynforward(_tmp60E,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL112;}
_LL112:;}goto _LLD1;_LLE2: if(*((int*)_tmp120)!= 5)goto _LLE4;_tmp12D=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp120)->f1;_tmp12E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp120)->f2;_LLE3: {struct _dynforward_ptr sns;struct _dynforward_ptr ts;{void*
_tmp171=_tmp12D;_LL118: if((int)_tmp171 != 2)goto _LL11A;_LL119: goto _LL11B;_LL11A:
if((int)_tmp171 != 0)goto _LL11C;_LL11B:{const char*_tmp610;sns=((_tmp610="",
_tag_dynforward(_tmp610,sizeof(char),_get_zero_arr_size_char(_tmp610,1))));}goto
_LL117;_LL11C: if((int)_tmp171 != 1)goto _LL117;_LL11D:{const char*_tmp611;sns=((
_tmp611="unsigned ",_tag_dynforward(_tmp611,sizeof(char),_get_zero_arr_size_char(
_tmp611,10))));}goto _LL117;_LL117:;}{void*_tmp174=_tmp12E;_LL11F: if((int)_tmp174
!= 0)goto _LL121;_LL120:{void*_tmp175=_tmp12D;_LL12A: if((int)_tmp175 != 2)goto
_LL12C;_LL12B:{const char*_tmp612;sns=((_tmp612="",_tag_dynforward(_tmp612,
sizeof(char),_get_zero_arr_size_char(_tmp612,1))));}goto _LL129;_LL12C: if((int)
_tmp175 != 0)goto _LL12E;_LL12D:{const char*_tmp613;sns=((_tmp613="signed ",
_tag_dynforward(_tmp613,sizeof(char),_get_zero_arr_size_char(_tmp613,8))));}goto
_LL129;_LL12E: if((int)_tmp175 != 1)goto _LL129;_LL12F:{const char*_tmp614;sns=((
_tmp614="unsigned ",_tag_dynforward(_tmp614,sizeof(char),_get_zero_arr_size_char(
_tmp614,10))));}goto _LL129;_LL129:;}{const char*_tmp615;ts=((_tmp615="char",
_tag_dynforward(_tmp615,sizeof(char),_get_zero_arr_size_char(_tmp615,5))));}goto
_LL11E;_LL121: if((int)_tmp174 != 1)goto _LL123;_LL122:{const char*_tmp616;ts=((
_tmp616="short",_tag_dynforward(_tmp616,sizeof(char),_get_zero_arr_size_char(
_tmp616,6))));}goto _LL11E;_LL123: if((int)_tmp174 != 2)goto _LL125;_LL124:{const
char*_tmp617;ts=((_tmp617="int",_tag_dynforward(_tmp617,sizeof(char),
_get_zero_arr_size_char(_tmp617,4))));}goto _LL11E;_LL125: if((int)_tmp174 != 3)
goto _LL127;_LL126:{const char*_tmp618;ts=((_tmp618="long",_tag_dynforward(_tmp618,
sizeof(char),_get_zero_arr_size_char(_tmp618,5))));}goto _LL11E;_LL127: if((int)
_tmp174 != 4)goto _LL11E;_LL128:{void*_tmp17D=Cyc_Cyclone_c_compiler;_LL131: if((
int)_tmp17D != 0)goto _LL133;_LL132:{const char*_tmp619;ts=((_tmp619="long long",
_tag_dynforward(_tmp619,sizeof(char),_get_zero_arr_size_char(_tmp619,10))));}
goto _LL130;_LL133: if((int)_tmp17D != 1)goto _LL130;_LL134:{const char*_tmp61A;ts=((
_tmp61A="__int64",_tag_dynforward(_tmp61A,sizeof(char),_get_zero_arr_size_char(
_tmp61A,8))));}goto _LL130;_LL130:;}goto _LL11E;_LL11E:;}{const char*_tmp61F;void*
_tmp61E[2];struct Cyc_String_pa_struct _tmp61D;struct Cyc_String_pa_struct _tmp61C;s=
Cyc_PP_text((struct _dynforward_ptr)((_tmp61C.tag=0,((_tmp61C.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)ts),((_tmp61D.tag=0,((_tmp61D.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)sns),((_tmp61E[0]=& _tmp61D,((_tmp61E[1]=&
_tmp61C,Cyc_aprintf(((_tmp61F="%s%s",_tag_dynforward(_tmp61F,sizeof(char),
_get_zero_arr_size_char(_tmp61F,5)))),_tag_dynforward(_tmp61E,sizeof(void*),2)))))))))))))));}
goto _LLD1;}_LLE4: if((int)_tmp120 != 1)goto _LLE6;_LLE5:{const char*_tmp620;s=Cyc_PP_text(((
_tmp620="float",_tag_dynforward(_tmp620,sizeof(char),_get_zero_arr_size_char(
_tmp620,6)))));}goto _LLD1;_LLE6: if(_tmp120 <= (void*)4)goto _LLFE;if(*((int*)
_tmp120)!= 6)goto _LLE8;_tmp12F=((struct Cyc_Absyn_DoubleType_struct*)_tmp120)->f1;
_LLE7: if(_tmp12F){const char*_tmp621;s=Cyc_PP_text(((_tmp621="long double",
_tag_dynforward(_tmp621,sizeof(char),_get_zero_arr_size_char(_tmp621,12)))));}
else{const char*_tmp622;s=Cyc_PP_text(((_tmp622="double",_tag_dynforward(_tmp622,
sizeof(char),_get_zero_arr_size_char(_tmp622,7)))));}goto _LLD1;_LLE8: if(*((int*)
_tmp120)!= 9)goto _LLEA;_tmp130=((struct Cyc_Absyn_TupleType_struct*)_tmp120)->f1;
_LLE9:{struct Cyc_PP_Doc*_tmp623[2];s=((_tmp623[1]=Cyc_Absynpp_args2doc(_tmp130),((
_tmp623[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dynforward(_tmp623,sizeof(struct
Cyc_PP_Doc*),2))))));}goto _LLD1;_LLEA: if(*((int*)_tmp120)!= 10)goto _LLEC;_tmp131=((
struct Cyc_Absyn_AggrType_struct*)_tmp120)->f1;_tmp132=_tmp131.aggr_info;_tmp133=
_tmp131.targs;_LLEB: {void*_tmp189;struct _tuple0*_tmp18A;struct _tuple3 _tmp188=
Cyc_Absyn_aggr_kinded_name(_tmp132);_tmp189=_tmp188.f1;_tmp18A=_tmp188.f2;{
struct Cyc_PP_Doc*_tmp624[3];s=((_tmp624[2]=Cyc_Absynpp_tps2doc(_tmp133),((
_tmp624[1]=Cyc_Absynpp_qvar2doc(_tmp18A),((_tmp624[0]=Cyc_Absynpp_aggr_kind2doc(
_tmp189),Cyc_PP_cat(_tag_dynforward(_tmp624,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLD1;}_LLEC: if(*((int*)_tmp120)!= 11)goto _LLEE;_tmp134=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp120)->f1;_tmp135=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp120)->f2;_LLED:{
struct Cyc_PP_Doc*_tmp625[4];s=((_tmp625[3]=Cyc_Absynpp_rb(),((_tmp625[2]=Cyc_PP_nest(
2,Cyc_Absynpp_aggrfields2doc(_tmp135)),((_tmp625[1]=Cyc_Absynpp_lb(),((_tmp625[0]=
Cyc_Absynpp_aggr_kind2doc(_tmp134),Cyc_PP_cat(_tag_dynforward(_tmp625,sizeof(
struct Cyc_PP_Doc*),4))))))))));}goto _LLD1;_LLEE: if(*((int*)_tmp120)!= 13)goto
_LLF0;_tmp136=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp120)->f1;_LLEF:{const
char*_tmp628;struct Cyc_PP_Doc*_tmp627[4];s=((_tmp627[3]=Cyc_Absynpp_rb(),((
_tmp627[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp136)),((_tmp627[1]=Cyc_Absynpp_lb(),((
_tmp627[0]=Cyc_PP_text(((_tmp628="enum ",_tag_dynforward(_tmp628,sizeof(char),
_get_zero_arr_size_char(_tmp628,6))))),Cyc_PP_cat(_tag_dynforward(_tmp627,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLD1;_LLF0: if(*((int*)_tmp120)!= 12)
goto _LLF2;_tmp137=((struct Cyc_Absyn_EnumType_struct*)_tmp120)->f1;_LLF1:{const
char*_tmp62B;struct Cyc_PP_Doc*_tmp62A[2];s=((_tmp62A[1]=Cyc_Absynpp_qvar2doc(
_tmp137),((_tmp62A[0]=Cyc_PP_text(((_tmp62B="enum ",_tag_dynforward(_tmp62B,
sizeof(char),_get_zero_arr_size_char(_tmp62B,6))))),Cyc_PP_cat(_tag_dynforward(
_tmp62A,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLD1;_LLF2: if(*((int*)_tmp120)!= 
14)goto _LLF4;_tmp138=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp120)->f1;
_LLF3:{const char*_tmp630;const char*_tmp62F;struct Cyc_PP_Doc*_tmp62E[3];s=((
_tmp62E[2]=Cyc_PP_text(((_tmp630=">",_tag_dynforward(_tmp630,sizeof(char),
_get_zero_arr_size_char(_tmp630,2))))),((_tmp62E[1]=Cyc_Absynpp_typ2doc(_tmp138),((
_tmp62E[0]=Cyc_PP_text(((_tmp62F="sizeof_t<",_tag_dynforward(_tmp62F,sizeof(char),
_get_zero_arr_size_char(_tmp62F,10))))),Cyc_PP_cat(_tag_dynforward(_tmp62E,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LLF4: if(*((int*)_tmp120)!= 17)
goto _LLF6;_tmp139=((struct Cyc_Absyn_TypedefType_struct*)_tmp120)->f1;_tmp13A=((
struct Cyc_Absyn_TypedefType_struct*)_tmp120)->f2;_tmp13B=((struct Cyc_Absyn_TypedefType_struct*)
_tmp120)->f3;_LLF5:{struct Cyc_PP_Doc*_tmp631[2];s=((_tmp631[1]=Cyc_Absynpp_tps2doc(
_tmp13A),((_tmp631[0]=Cyc_Absynpp_qvar2doc(_tmp139),Cyc_PP_cat(_tag_dynforward(
_tmp631,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLD1;_LLF6: if(*((int*)_tmp120)!= 
15)goto _LLF8;_tmp13C=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp120)->f1;
_LLF7:{const char*_tmp636;const char*_tmp635;struct Cyc_PP_Doc*_tmp634[3];s=((
_tmp634[2]=Cyc_PP_text(((_tmp636=">",_tag_dynforward(_tmp636,sizeof(char),
_get_zero_arr_size_char(_tmp636,2))))),((_tmp634[1]=Cyc_Absynpp_rgn2doc(_tmp13C),((
_tmp634[0]=Cyc_PP_text(((_tmp635="region_t<",_tag_dynforward(_tmp635,sizeof(char),
_get_zero_arr_size_char(_tmp635,10))))),Cyc_PP_cat(_tag_dynforward(_tmp634,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LLF8: if(*((int*)_tmp120)!= 16)
goto _LLFA;_tmp13D=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp120)->f1;
_tmp13E=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp120)->f2;_LLF9:{const
char*_tmp63D;const char*_tmp63C;const char*_tmp63B;struct Cyc_PP_Doc*_tmp63A[5];s=((
_tmp63A[4]=Cyc_PP_text(((_tmp63D=">",_tag_dynforward(_tmp63D,sizeof(char),
_get_zero_arr_size_char(_tmp63D,2))))),((_tmp63A[3]=Cyc_Absynpp_rgn2doc(_tmp13E),((
_tmp63A[2]=Cyc_PP_text(((_tmp63C=",",_tag_dynforward(_tmp63C,sizeof(char),
_get_zero_arr_size_char(_tmp63C,2))))),((_tmp63A[1]=Cyc_Absynpp_rgn2doc(_tmp13D),((
_tmp63A[0]=Cyc_PP_text(((_tmp63B="dynregion_t<",_tag_dynforward(_tmp63B,sizeof(
char),_get_zero_arr_size_char(_tmp63B,13))))),Cyc_PP_cat(_tag_dynforward(_tmp63A,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LLD1;_LLFA: if(*((int*)_tmp120)!= 
19)goto _LLFC;_tmp13F=((struct Cyc_Absyn_TypeInt_struct*)_tmp120)->f1;_LLFB:{const
char*_tmp641;void*_tmp640[1];struct Cyc_Int_pa_struct _tmp63F;s=Cyc_PP_text((
struct _dynforward_ptr)((_tmp63F.tag=1,((_tmp63F.f1=(unsigned long)_tmp13F,((
_tmp640[0]=& _tmp63F,Cyc_aprintf(((_tmp641="`%d",_tag_dynforward(_tmp641,sizeof(
char),_get_zero_arr_size_char(_tmp641,4)))),_tag_dynforward(_tmp640,sizeof(void*),
1)))))))));}goto _LLD1;_LLFC: if(*((int*)_tmp120)!= 18)goto _LLFE;_tmp140=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp120)->f1;_LLFD:{const char*_tmp646;const char*
_tmp645;struct Cyc_PP_Doc*_tmp644[3];s=((_tmp644[2]=Cyc_PP_text(((_tmp646=">",
_tag_dynforward(_tmp646,sizeof(char),_get_zero_arr_size_char(_tmp646,2))))),((
_tmp644[1]=Cyc_Absynpp_typ2doc(_tmp140),((_tmp644[0]=Cyc_PP_text(((_tmp645="tag_t<",
_tag_dynforward(_tmp645,sizeof(char),_get_zero_arr_size_char(_tmp645,7))))),Cyc_PP_cat(
_tag_dynforward(_tmp644,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LLFE: if((
int)_tmp120 != 3)goto _LL100;_LLFF: goto _LL101;_LL100: if((int)_tmp120 != 2)goto
_LL102;_LL101: s=Cyc_Absynpp_rgn2doc(t);goto _LLD1;_LL102: if(_tmp120 <= (void*)4)
goto _LL104;if(*((int*)_tmp120)!= 22)goto _LL104;_tmp141=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp120)->f1;_LL103:{const char*_tmp64B;const char*_tmp64A;struct Cyc_PP_Doc*
_tmp649[3];s=((_tmp649[2]=Cyc_PP_text(((_tmp64B=")",_tag_dynforward(_tmp64B,
sizeof(char),_get_zero_arr_size_char(_tmp64B,2))))),((_tmp649[1]=Cyc_Absynpp_typ2doc(
_tmp141),((_tmp649[0]=Cyc_PP_text(((_tmp64A="regions(",_tag_dynforward(_tmp64A,
sizeof(char),_get_zero_arr_size_char(_tmp64A,9))))),Cyc_PP_cat(_tag_dynforward(
_tmp649,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LL104: if(_tmp120 <= (void*)
4)goto _LL106;if(*((int*)_tmp120)!= 20)goto _LL106;_LL105: goto _LL107;_LL106: if(
_tmp120 <= (void*)4)goto _LLD1;if(*((int*)_tmp120)!= 21)goto _LLD1;_LL107: s=Cyc_Absynpp_eff2doc(
t);goto _LLD1;_LLD1:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct Cyc_Core_Opt*
vo);struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct Cyc_Core_Opt*vo){return vo == 0?Cyc_PP_nil_doc():
Cyc_PP_text(*((struct _dynforward_ptr*)vo->v));}struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(
struct _tuple5*cmp);struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple5*cmp){
struct _tuple5 _tmp1A6;void*_tmp1A7;void*_tmp1A8;struct _tuple5*_tmp1A5=cmp;_tmp1A6=*
_tmp1A5;_tmp1A7=_tmp1A6.f1;_tmp1A8=_tmp1A6.f2;{const char*_tmp64E;struct Cyc_PP_Doc*
_tmp64D[3];return(_tmp64D[2]=Cyc_Absynpp_rgn2doc(_tmp1A8),((_tmp64D[1]=Cyc_PP_text(((
_tmp64E=" > ",_tag_dynforward(_tmp64E,sizeof(char),_get_zero_arr_size_char(
_tmp64E,4))))),((_tmp64D[0]=Cyc_Absynpp_rgn2doc(_tmp1A7),Cyc_PP_cat(
_tag_dynforward(_tmp64D,sizeof(struct Cyc_PP_Doc*),3)))))));}}struct Cyc_PP_Doc*
Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po);struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(
struct Cyc_List_List*po){const char*_tmp651;const char*_tmp650;const char*_tmp64F;
return Cyc_PP_group(((_tmp64F="",_tag_dynforward(_tmp64F,sizeof(char),
_get_zero_arr_size_char(_tmp64F,1)))),((_tmp650="",_tag_dynforward(_tmp650,
sizeof(char),_get_zero_arr_size_char(_tmp650,1)))),((_tmp651=",",_tag_dynforward(
_tmp651,sizeof(char),_get_zero_arr_size_char(_tmp651,2)))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,
po));}struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple1*t);struct Cyc_PP_Doc*
Cyc_Absynpp_funarg2doc(struct _tuple1*t){struct Cyc_Core_Opt*_tmp652;struct Cyc_Core_Opt*
dopt=(*t).f1 == 0?0:((_tmp652=_cycalloc(sizeof(*_tmp652)),((_tmp652->v=Cyc_PP_text(*((
struct _dynforward_ptr*)((struct Cyc_Core_Opt*)_check_null((*t).f1))->v)),_tmp652))));
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po);struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po){struct Cyc_List_List*
_tmp1AF=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple1*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);struct Cyc_PP_Doc*
eff_doc;if(c_varargs){const char*_tmp655;struct Cyc_List_List*_tmp654;_tmp1AF=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp1AF,((_tmp654=_cycalloc(sizeof(*_tmp654)),((_tmp654->hd=Cyc_PP_text(((
_tmp655="...",_tag_dynforward(_tmp655,sizeof(char),_get_zero_arr_size_char(
_tmp655,4))))),((_tmp654->tl=0,_tmp654)))))));}else{if(cyc_varargs != 0){struct
_tuple1*_tmp65E;const char*_tmp65D;const char*_tmp65C;const char*_tmp65B;struct Cyc_PP_Doc*
_tmp65A[3];struct Cyc_PP_Doc*_tmp1B2=(_tmp65A[2]=Cyc_Absynpp_funarg2doc(((_tmp65E=
_cycalloc(sizeof(*_tmp65E)),((_tmp65E->f1=cyc_varargs->name,((_tmp65E->f2=
cyc_varargs->tq,((_tmp65E->f3=(void*)cyc_varargs->type,_tmp65E))))))))),((
_tmp65A[1]=cyc_varargs->inject?Cyc_PP_text(((_tmp65C=" inject ",_tag_dynforward(
_tmp65C,sizeof(char),_get_zero_arr_size_char(_tmp65C,9))))): Cyc_PP_text(((
_tmp65D=" ",_tag_dynforward(_tmp65D,sizeof(char),_get_zero_arr_size_char(_tmp65D,
2))))),((_tmp65A[0]=Cyc_PP_text(((_tmp65B="...",_tag_dynforward(_tmp65B,sizeof(
char),_get_zero_arr_size_char(_tmp65B,4))))),Cyc_PP_cat(_tag_dynforward(_tmp65A,
sizeof(struct Cyc_PP_Doc*),3)))))));struct Cyc_List_List*_tmp65F;_tmp1AF=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp1AF,((_tmp65F=_cycalloc(sizeof(*_tmp65F)),((_tmp65F->hd=_tmp1B2,((_tmp65F->tl=
0,_tmp65F)))))));}}{const char*_tmp662;const char*_tmp661;const char*_tmp660;struct
Cyc_PP_Doc*_tmp1B9=Cyc_PP_group(((_tmp660="",_tag_dynforward(_tmp660,sizeof(char),
_get_zero_arr_size_char(_tmp660,1)))),((_tmp661="",_tag_dynforward(_tmp661,
sizeof(char),_get_zero_arr_size_char(_tmp661,1)))),((_tmp662=",",_tag_dynforward(
_tmp662,sizeof(char),_get_zero_arr_size_char(_tmp662,2)))),_tmp1AF);if(effopt != 
0  && Cyc_Absynpp_print_all_effects){const char*_tmp665;struct Cyc_PP_Doc*_tmp664[3];
_tmp1B9=((_tmp664[2]=Cyc_Absynpp_eff2doc((void*)effopt->v),((_tmp664[1]=Cyc_PP_text(((
_tmp665=";",_tag_dynforward(_tmp665,sizeof(char),_get_zero_arr_size_char(_tmp665,
2))))),((_tmp664[0]=_tmp1B9,Cyc_PP_cat(_tag_dynforward(_tmp664,sizeof(struct Cyc_PP_Doc*),
3))))))));}if(rgn_po != 0){const char*_tmp668;struct Cyc_PP_Doc*_tmp667[3];_tmp1B9=((
_tmp667[2]=Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp667[1]=Cyc_PP_text(((_tmp668=":",
_tag_dynforward(_tmp668,sizeof(char),_get_zero_arr_size_char(_tmp668,2))))),((
_tmp667[0]=_tmp1B9,Cyc_PP_cat(_tag_dynforward(_tmp667,sizeof(struct Cyc_PP_Doc*),
3))))))));}{const char*_tmp66D;const char*_tmp66C;struct Cyc_PP_Doc*_tmp66B[3];
return(_tmp66B[2]=Cyc_PP_text(((_tmp66D=")",_tag_dynforward(_tmp66D,sizeof(char),
_get_zero_arr_size_char(_tmp66D,2))))),((_tmp66B[1]=_tmp1B9,((_tmp66B[0]=Cyc_PP_text(((
_tmp66C="(",_tag_dynforward(_tmp66C,sizeof(char),_get_zero_arr_size_char(_tmp66C,
2))))),Cyc_PP_cat(_tag_dynforward(_tmp66B,sizeof(struct Cyc_PP_Doc*),3)))))));}}}
struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple6*arg);struct _tuple1*Cyc_Absynpp_arg_mk_opt(
struct _tuple6*arg){struct Cyc_Core_Opt*_tmp670;struct _tuple1*_tmp66F;return(
_tmp66F=_cycalloc(sizeof(*_tmp66F)),((_tmp66F->f1=((_tmp670=_cycalloc(sizeof(*
_tmp670)),((_tmp670->v=(*arg).f1,_tmp670)))),((_tmp66F->f2=(*arg).f2,((_tmp66F->f3=(*
arg).f3,_tmp66F)))))));}struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct
_dynforward_ptr*v);struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dynforward_ptr*v){
return Cyc_PP_text(*v);}struct _dynforward_ptr Cyc_Absynpp_qvar2string(struct
_tuple0*q);struct _dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){struct
Cyc_List_List*_tmp1C6=0;int match;{union Cyc_Absyn_Nmspace_union _tmp1C7=(*q).f1;
struct Cyc_List_List*_tmp1C8;struct Cyc_List_List*_tmp1C9;_LL136: if((_tmp1C7.Loc_n).tag
!= 0)goto _LL138;_LL137: _tmp1C8=0;goto _LL139;_LL138: if((_tmp1C7.Rel_n).tag != 1)
goto _LL13A;_tmp1C8=(_tmp1C7.Rel_n).f1;_LL139: match=0;_tmp1C6=_tmp1C8;goto _LL135;
_LL13A: if((_tmp1C7.Abs_n).tag != 2)goto _LL135;_tmp1C9=(_tmp1C7.Abs_n).f1;_LL13B:
match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dynforward_ptr*,
struct _dynforward_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(
Cyc_strptrcmp,_tmp1C9,Cyc_Absynpp_curr_namespace);{struct Cyc_List_List*_tmp671;
_tmp1C6=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(_tmp671=
_cycalloc(sizeof(*_tmp671)),((_tmp671->hd=Cyc_Absynpp_cyc_stringptr,((_tmp671->tl=
_tmp1C9,_tmp671))))): _tmp1C9;}goto _LL135;_LL135:;}if(Cyc_Absynpp_qvar_to_Cids){
const char*_tmp673;struct Cyc_List_List*_tmp672;return(struct _dynforward_ptr)Cyc_str_sepstr(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp1C6,((_tmp672=_cycalloc(sizeof(*_tmp672)),((_tmp672->hd=(*q).f2,((_tmp672->tl=
0,_tmp672))))))),((_tmp673="_",_tag_dynforward(_tmp673,sizeof(char),
_get_zero_arr_size_char(_tmp673,2)))));}else{if(match)return*(*q).f2;else{const
char*_tmp675;struct Cyc_List_List*_tmp674;return(struct _dynforward_ptr)Cyc_str_sepstr(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
_tmp1C6,((_tmp674=_cycalloc(sizeof(*_tmp674)),((_tmp674->hd=(*q).f2,((_tmp674->tl=
0,_tmp674))))))),((_tmp675="::",_tag_dynforward(_tmp675,sizeof(char),
_get_zero_arr_size_char(_tmp675,3)))));}}}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(
struct _tuple0*q);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){return
Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(
struct _tuple0*q);struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dynforward_ptr _tmp1CF=Cyc_Absynpp_qvar2string(q);if(Cyc_PP_tex_output){
const char*_tmp677;const char*_tmp676;return Cyc_PP_text_width((struct
_dynforward_ptr)Cyc_strconcat((struct _dynforward_ptr)Cyc_strconcat(((_tmp676="\\textbf{",
_tag_dynforward(_tmp676,sizeof(char),_get_zero_arr_size_char(_tmp676,9)))),(
struct _dynforward_ptr)_tmp1CF),((_tmp677="}",_tag_dynforward(_tmp677,sizeof(char),
_get_zero_arr_size_char(_tmp677,2))))),(int)Cyc_strlen((struct _dynforward_ptr)
_tmp1CF));}else{return Cyc_PP_text(_tmp1CF);}}struct _dynforward_ptr Cyc_Absynpp_typedef_name2string(
struct _tuple0*v);struct _dynforward_ptr Cyc_Absynpp_typedef_name2string(struct
_tuple0*v){if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace_union _tmp1D2=(*v).f1;struct Cyc_List_List*_tmp1D3;struct
Cyc_List_List*_tmp1D4;_LL13D: if((_tmp1D2.Loc_n).tag != 0)goto _LL13F;_LL13E: goto
_LL140;_LL13F: if((_tmp1D2.Rel_n).tag != 1)goto _LL141;_tmp1D3=(_tmp1D2.Rel_n).f1;
if(_tmp1D3 != 0)goto _LL141;_LL140: return*(*v).f2;_LL141: if((_tmp1D2.Abs_n).tag != 
2)goto _LL143;_tmp1D4=(_tmp1D2.Abs_n).f1;_LL142: if(((int(*)(int(*cmp)(struct
_dynforward_ptr*,struct _dynforward_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*
l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp1D4,Cyc_Absynpp_curr_namespace)== 0)
return*(*v).f2;else{goto _LL144;}_LL143:;_LL144: {const char*_tmp678;return(struct
_dynforward_ptr)Cyc_strconcat(((_tmp678="/* bad namespace : */ ",_tag_dynforward(
_tmp678,sizeof(char),_get_zero_arr_size_char(_tmp678,23)))),(struct
_dynforward_ptr)Cyc_Absynpp_qvar2string(v));}_LL13C:;}else{return*(*v).f2;}}
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v);struct Cyc_PP_Doc*
Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(
v));}struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v);struct
Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v){struct
_dynforward_ptr _tmp1D6=Cyc_Absynpp_typedef_name2string(v);if(Cyc_PP_tex_output){
const char*_tmp67A;const char*_tmp679;return Cyc_PP_text_width((struct
_dynforward_ptr)Cyc_strconcat((struct _dynforward_ptr)Cyc_strconcat(((_tmp679="\\textbf{",
_tag_dynforward(_tmp679,sizeof(char),_get_zero_arr_size_char(_tmp679,9)))),(
struct _dynforward_ptr)_tmp1D6),((_tmp67A="}",_tag_dynforward(_tmp67A,sizeof(char),
_get_zero_arr_size_char(_tmp67A,2))))),(int)Cyc_strlen((struct _dynforward_ptr)
_tmp1D6));}else{return Cyc_PP_text(_tmp1D6);}}struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(
void*t);struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){return Cyc_Absynpp_tqtd2doc(
Cyc_Absyn_empty_tqual(0),t,0);}int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e);
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){void*_tmp1D9=(void*)e->r;void*
_tmp1DA;struct Cyc_Absyn_Exp*_tmp1DB;struct Cyc_Absyn_Exp*_tmp1DC;_LL146: if(*((int*)
_tmp1D9)!= 0)goto _LL148;_LL147: goto _LL149;_LL148: if(*((int*)_tmp1D9)!= 1)goto
_LL14A;_LL149: goto _LL14B;_LL14A: if(*((int*)_tmp1D9)!= 2)goto _LL14C;_LL14B: return
10000;_LL14C: if(*((int*)_tmp1D9)!= 3)goto _LL14E;_tmp1DA=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp1D9)->f1;_LL14D: {void*_tmp1DD=_tmp1DA;_LL195: if((int)_tmp1DD != 0)goto _LL197;
_LL196: return 100;_LL197: if((int)_tmp1DD != 1)goto _LL199;_LL198: return 110;_LL199:
if((int)_tmp1DD != 2)goto _LL19B;_LL19A: return 100;_LL19B: if((int)_tmp1DD != 3)goto
_LL19D;_LL19C: goto _LL19E;_LL19D: if((int)_tmp1DD != 4)goto _LL19F;_LL19E: return 110;
_LL19F: if((int)_tmp1DD != 5)goto _LL1A1;_LL1A0: goto _LL1A2;_LL1A1: if((int)_tmp1DD != 
6)goto _LL1A3;_LL1A2: return 70;_LL1A3: if((int)_tmp1DD != 7)goto _LL1A5;_LL1A4: goto
_LL1A6;_LL1A5: if((int)_tmp1DD != 8)goto _LL1A7;_LL1A6: goto _LL1A8;_LL1A7: if((int)
_tmp1DD != 9)goto _LL1A9;_LL1A8: goto _LL1AA;_LL1A9: if((int)_tmp1DD != 10)goto _LL1AB;
_LL1AA: return 80;_LL1AB: if((int)_tmp1DD != 11)goto _LL1AD;_LL1AC: goto _LL1AE;_LL1AD:
if((int)_tmp1DD != 12)goto _LL1AF;_LL1AE: return 130;_LL1AF: if((int)_tmp1DD != 13)
goto _LL1B1;_LL1B0: return 60;_LL1B1: if((int)_tmp1DD != 14)goto _LL1B3;_LL1B2: return
40;_LL1B3: if((int)_tmp1DD != 15)goto _LL1B5;_LL1B4: return 50;_LL1B5: if((int)_tmp1DD
!= 16)goto _LL1B7;_LL1B6: return 90;_LL1B7: if((int)_tmp1DD != 17)goto _LL1B9;_LL1B8:
return 80;_LL1B9: if((int)_tmp1DD != 18)goto _LL1BB;_LL1BA: return 80;_LL1BB: if((int)
_tmp1DD != 19)goto _LL194;_LL1BC: return 140;_LL194:;}_LL14E: if(*((int*)_tmp1D9)!= 4)
goto _LL150;_LL14F: return 20;_LL150: if(*((int*)_tmp1D9)!= 5)goto _LL152;_LL151:
return 130;_LL152: if(*((int*)_tmp1D9)!= 6)goto _LL154;_LL153: return 30;_LL154: if(*((
int*)_tmp1D9)!= 7)goto _LL156;_LL155: return 35;_LL156: if(*((int*)_tmp1D9)!= 8)goto
_LL158;_LL157: return 30;_LL158: if(*((int*)_tmp1D9)!= 9)goto _LL15A;_LL159: return 10;
_LL15A: if(*((int*)_tmp1D9)!= 10)goto _LL15C;_LL15B: goto _LL15D;_LL15C: if(*((int*)
_tmp1D9)!= 11)goto _LL15E;_LL15D: return 140;_LL15E: if(*((int*)_tmp1D9)!= 12)goto
_LL160;_LL15F: return 130;_LL160: if(*((int*)_tmp1D9)!= 13)goto _LL162;_tmp1DB=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp1D9)->f1;_LL161: return Cyc_Absynpp_exp_prec(
_tmp1DB);_LL162: if(*((int*)_tmp1D9)!= 14)goto _LL164;_tmp1DC=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp1D9)->f1;_LL163: return Cyc_Absynpp_exp_prec(_tmp1DC);_LL164: if(*((int*)
_tmp1D9)!= 15)goto _LL166;_LL165: return 120;_LL166: if(*((int*)_tmp1D9)!= 17)goto
_LL168;_LL167: return 15;_LL168: if(*((int*)_tmp1D9)!= 16)goto _LL16A;_LL169: goto
_LL16B;_LL16A: if(*((int*)_tmp1D9)!= 18)goto _LL16C;_LL16B: goto _LL16D;_LL16C: if(*((
int*)_tmp1D9)!= 19)goto _LL16E;_LL16D: goto _LL16F;_LL16E: if(*((int*)_tmp1D9)!= 20)
goto _LL170;_LL16F: goto _LL171;_LL170: if(*((int*)_tmp1D9)!= 21)goto _LL172;_LL171:
goto _LL173;_LL172: if(*((int*)_tmp1D9)!= 22)goto _LL174;_LL173: return 130;_LL174:
if(*((int*)_tmp1D9)!= 23)goto _LL176;_LL175: goto _LL177;_LL176: if(*((int*)_tmp1D9)
!= 24)goto _LL178;_LL177: goto _LL179;_LL178: if(*((int*)_tmp1D9)!= 25)goto _LL17A;
_LL179: return 140;_LL17A: if(*((int*)_tmp1D9)!= 26)goto _LL17C;_LL17B: return 150;
_LL17C: if(*((int*)_tmp1D9)!= 27)goto _LL17E;_LL17D: goto _LL17F;_LL17E: if(*((int*)
_tmp1D9)!= 28)goto _LL180;_LL17F: goto _LL181;_LL180: if(*((int*)_tmp1D9)!= 29)goto
_LL182;_LL181: goto _LL183;_LL182: if(*((int*)_tmp1D9)!= 30)goto _LL184;_LL183: goto
_LL185;_LL184: if(*((int*)_tmp1D9)!= 31)goto _LL186;_LL185: goto _LL187;_LL186: if(*((
int*)_tmp1D9)!= 32)goto _LL188;_LL187: goto _LL189;_LL188: if(*((int*)_tmp1D9)!= 33)
goto _LL18A;_LL189: goto _LL18B;_LL18A: if(*((int*)_tmp1D9)!= 34)goto _LL18C;_LL18B:
goto _LL18D;_LL18C: if(*((int*)_tmp1D9)!= 35)goto _LL18E;_LL18D: goto _LL18F;_LL18E:
if(*((int*)_tmp1D9)!= 36)goto _LL190;_LL18F: goto _LL191;_LL190: if(*((int*)_tmp1D9)
!= 37)goto _LL192;_LL191: return 140;_LL192: if(*((int*)_tmp1D9)!= 38)goto _LL145;
_LL193: return 10000;_LL145:;}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*
e);struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){return Cyc_Absynpp_exp2doc_prec(
0,e);}struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e);
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){int
myprec=Cyc_Absynpp_exp_prec(e);struct Cyc_PP_Doc*s;{void*_tmp1DE=(void*)e->r;
union Cyc_Absyn_Cnst_union _tmp1DF;struct _tuple0*_tmp1E0;struct _tuple0*_tmp1E1;
void*_tmp1E2;struct Cyc_List_List*_tmp1E3;struct Cyc_Absyn_Exp*_tmp1E4;struct Cyc_Core_Opt*
_tmp1E5;struct Cyc_Absyn_Exp*_tmp1E6;struct Cyc_Absyn_Exp*_tmp1E7;void*_tmp1E8;
struct Cyc_Absyn_Exp*_tmp1E9;struct Cyc_Absyn_Exp*_tmp1EA;struct Cyc_Absyn_Exp*
_tmp1EB;struct Cyc_Absyn_Exp*_tmp1EC;struct Cyc_Absyn_Exp*_tmp1ED;struct Cyc_Absyn_Exp*
_tmp1EE;struct Cyc_Absyn_Exp*_tmp1EF;struct Cyc_Absyn_Exp*_tmp1F0;struct Cyc_Absyn_Exp*
_tmp1F1;struct Cyc_Absyn_Exp*_tmp1F2;struct Cyc_List_List*_tmp1F3;struct Cyc_Absyn_Exp*
_tmp1F4;struct Cyc_List_List*_tmp1F5;struct Cyc_Absyn_Exp*_tmp1F6;struct Cyc_Absyn_Exp*
_tmp1F7;struct Cyc_Absyn_Exp*_tmp1F8;void*_tmp1F9;struct Cyc_Absyn_Exp*_tmp1FA;
struct Cyc_Absyn_Exp*_tmp1FB;struct Cyc_Absyn_Exp*_tmp1FC;struct Cyc_Absyn_Exp*
_tmp1FD;void*_tmp1FE;struct Cyc_Absyn_Exp*_tmp1FF;void*_tmp200;void*_tmp201;
struct _dynforward_ptr*_tmp202;void*_tmp203;void*_tmp204;unsigned int _tmp205;
struct Cyc_List_List*_tmp206;void*_tmp207;struct Cyc_Absyn_Exp*_tmp208;struct Cyc_Absyn_Exp*
_tmp209;struct _dynforward_ptr*_tmp20A;struct Cyc_Absyn_Exp*_tmp20B;struct
_dynforward_ptr*_tmp20C;struct Cyc_Absyn_Exp*_tmp20D;struct Cyc_Absyn_Exp*_tmp20E;
struct Cyc_List_List*_tmp20F;struct _tuple1*_tmp210;struct Cyc_List_List*_tmp211;
struct Cyc_List_List*_tmp212;struct Cyc_Absyn_Vardecl*_tmp213;struct Cyc_Absyn_Exp*
_tmp214;struct Cyc_Absyn_Exp*_tmp215;struct _tuple0*_tmp216;struct Cyc_List_List*
_tmp217;struct Cyc_List_List*_tmp218;struct Cyc_List_List*_tmp219;struct Cyc_List_List*
_tmp21A;struct Cyc_Absyn_Tunionfield*_tmp21B;struct _tuple0*_tmp21C;struct _tuple0*
_tmp21D;struct Cyc_Absyn_MallocInfo _tmp21E;int _tmp21F;struct Cyc_Absyn_Exp*_tmp220;
void**_tmp221;struct Cyc_Absyn_Exp*_tmp222;struct Cyc_Absyn_Exp*_tmp223;struct Cyc_Absyn_Exp*
_tmp224;struct Cyc_Core_Opt*_tmp225;struct Cyc_List_List*_tmp226;struct Cyc_Absyn_Stmt*
_tmp227;_LL1BE: if(*((int*)_tmp1DE)!= 0)goto _LL1C0;_tmp1DF=((struct Cyc_Absyn_Const_e_struct*)
_tmp1DE)->f1;_LL1BF: s=Cyc_Absynpp_cnst2doc(_tmp1DF);goto _LL1BD;_LL1C0: if(*((int*)
_tmp1DE)!= 1)goto _LL1C2;_tmp1E0=((struct Cyc_Absyn_Var_e_struct*)_tmp1DE)->f1;
_LL1C1: _tmp1E1=_tmp1E0;goto _LL1C3;_LL1C2: if(*((int*)_tmp1DE)!= 2)goto _LL1C4;
_tmp1E1=((struct Cyc_Absyn_UnknownId_e_struct*)_tmp1DE)->f1;_LL1C3: s=Cyc_Absynpp_qvar2doc(
_tmp1E1);goto _LL1BD;_LL1C4: if(*((int*)_tmp1DE)!= 3)goto _LL1C6;_tmp1E2=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmp1DE)->f1;_tmp1E3=((struct Cyc_Absyn_Primop_e_struct*)
_tmp1DE)->f2;_LL1C5: s=Cyc_Absynpp_primapp2doc(myprec,_tmp1E2,_tmp1E3);goto _LL1BD;
_LL1C6: if(*((int*)_tmp1DE)!= 4)goto _LL1C8;_tmp1E4=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp1DE)->f1;_tmp1E5=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp1DE)->f2;_tmp1E6=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp1DE)->f3;_LL1C7:{const char*_tmp67F;const
char*_tmp67E;struct Cyc_PP_Doc*_tmp67D[5];s=((_tmp67D[4]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1E6),((_tmp67D[3]=Cyc_PP_text(((_tmp67F="= ",_tag_dynforward(_tmp67F,
sizeof(char),_get_zero_arr_size_char(_tmp67F,3))))),((_tmp67D[2]=_tmp1E5 == 0?Cyc_PP_nil_doc():
Cyc_Absynpp_prim2doc((void*)_tmp1E5->v),((_tmp67D[1]=Cyc_PP_text(((_tmp67E=" ",
_tag_dynforward(_tmp67E,sizeof(char),_get_zero_arr_size_char(_tmp67E,2))))),((
_tmp67D[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E4),Cyc_PP_cat(_tag_dynforward(
_tmp67D,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1BD;_LL1C8: if(*((int*)
_tmp1DE)!= 5)goto _LL1CA;_tmp1E7=((struct Cyc_Absyn_Increment_e_struct*)_tmp1DE)->f1;
_tmp1E8=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp1DE)->f2;_LL1C9: {
struct Cyc_PP_Doc*_tmp22B=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1E7);{void*_tmp22C=
_tmp1E8;_LL20F: if((int)_tmp22C != 0)goto _LL211;_LL210:{const char*_tmp682;struct
Cyc_PP_Doc*_tmp681[2];s=((_tmp681[1]=_tmp22B,((_tmp681[0]=Cyc_PP_text(((_tmp682="++",
_tag_dynforward(_tmp682,sizeof(char),_get_zero_arr_size_char(_tmp682,3))))),Cyc_PP_cat(
_tag_dynforward(_tmp681,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL20E;_LL211: if((
int)_tmp22C != 2)goto _LL213;_LL212:{const char*_tmp685;struct Cyc_PP_Doc*_tmp684[2];
s=((_tmp684[1]=_tmp22B,((_tmp684[0]=Cyc_PP_text(((_tmp685="--",_tag_dynforward(
_tmp685,sizeof(char),_get_zero_arr_size_char(_tmp685,3))))),Cyc_PP_cat(
_tag_dynforward(_tmp684,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL20E;_LL213: if((
int)_tmp22C != 1)goto _LL215;_LL214:{const char*_tmp688;struct Cyc_PP_Doc*_tmp687[2];
s=((_tmp687[1]=Cyc_PP_text(((_tmp688="++",_tag_dynforward(_tmp688,sizeof(char),
_get_zero_arr_size_char(_tmp688,3))))),((_tmp687[0]=_tmp22B,Cyc_PP_cat(
_tag_dynforward(_tmp687,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL20E;_LL215: if((
int)_tmp22C != 3)goto _LL20E;_LL216:{const char*_tmp68B;struct Cyc_PP_Doc*_tmp68A[2];
s=((_tmp68A[1]=Cyc_PP_text(((_tmp68B="--",_tag_dynforward(_tmp68B,sizeof(char),
_get_zero_arr_size_char(_tmp68B,3))))),((_tmp68A[0]=_tmp22B,Cyc_PP_cat(
_tag_dynforward(_tmp68A,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL20E;_LL20E:;}
goto _LL1BD;}_LL1CA: if(*((int*)_tmp1DE)!= 6)goto _LL1CC;_tmp1E9=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp1DE)->f1;_tmp1EA=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1DE)->f2;
_tmp1EB=((struct Cyc_Absyn_Conditional_e_struct*)_tmp1DE)->f3;_LL1CB:{const char*
_tmp690;const char*_tmp68F;struct Cyc_PP_Doc*_tmp68E[5];s=((_tmp68E[4]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1EB),((_tmp68E[3]=Cyc_PP_text(((_tmp690=" : ",_tag_dynforward(_tmp690,
sizeof(char),_get_zero_arr_size_char(_tmp690,4))))),((_tmp68E[2]=Cyc_Absynpp_exp2doc_prec(
0,_tmp1EA),((_tmp68E[1]=Cyc_PP_text(((_tmp68F=" ? ",_tag_dynforward(_tmp68F,
sizeof(char),_get_zero_arr_size_char(_tmp68F,4))))),((_tmp68E[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1E9),Cyc_PP_cat(_tag_dynforward(_tmp68E,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL1BD;_LL1CC: if(*((int*)_tmp1DE)!= 7)goto _LL1CE;_tmp1EC=((struct Cyc_Absyn_And_e_struct*)
_tmp1DE)->f1;_tmp1ED=((struct Cyc_Absyn_And_e_struct*)_tmp1DE)->f2;_LL1CD:{const
char*_tmp693;struct Cyc_PP_Doc*_tmp692[3];s=((_tmp692[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1ED),((_tmp692[1]=Cyc_PP_text(((_tmp693=" && ",_tag_dynforward(
_tmp693,sizeof(char),_get_zero_arr_size_char(_tmp693,5))))),((_tmp692[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1EC),Cyc_PP_cat(_tag_dynforward(_tmp692,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1BD;_LL1CE: if(*((int*)_tmp1DE)!= 8)goto _LL1D0;_tmp1EE=((struct Cyc_Absyn_Or_e_struct*)
_tmp1DE)->f1;_tmp1EF=((struct Cyc_Absyn_Or_e_struct*)_tmp1DE)->f2;_LL1CF:{const
char*_tmp696;struct Cyc_PP_Doc*_tmp695[3];s=((_tmp695[2]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1EF),((_tmp695[1]=Cyc_PP_text(((_tmp696=" || ",_tag_dynforward(
_tmp696,sizeof(char),_get_zero_arr_size_char(_tmp696,5))))),((_tmp695[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1EE),Cyc_PP_cat(_tag_dynforward(_tmp695,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1BD;_LL1D0: if(*((int*)_tmp1DE)!= 9)goto _LL1D2;_tmp1F0=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp1DE)->f1;_tmp1F1=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp1DE)->f2;_LL1D1:{
const char*_tmp69D;const char*_tmp69C;const char*_tmp69B;struct Cyc_PP_Doc*_tmp69A[5];
s=((_tmp69A[4]=Cyc_PP_text(((_tmp69D=")",_tag_dynforward(_tmp69D,sizeof(char),
_get_zero_arr_size_char(_tmp69D,2))))),((_tmp69A[3]=Cyc_Absynpp_exp2doc(_tmp1F1),((
_tmp69A[2]=Cyc_PP_text(((_tmp69C=", ",_tag_dynforward(_tmp69C,sizeof(char),
_get_zero_arr_size_char(_tmp69C,3))))),((_tmp69A[1]=Cyc_Absynpp_exp2doc(_tmp1F0),((
_tmp69A[0]=Cyc_PP_text(((_tmp69B="(",_tag_dynforward(_tmp69B,sizeof(char),
_get_zero_arr_size_char(_tmp69B,2))))),Cyc_PP_cat(_tag_dynforward(_tmp69A,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1BD;_LL1D2: if(*((int*)_tmp1DE)!= 
10)goto _LL1D4;_tmp1F2=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1DE)->f1;
_tmp1F3=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp1DE)->f2;_LL1D3:{const char*
_tmp6A2;const char*_tmp6A1;struct Cyc_PP_Doc*_tmp6A0[4];s=((_tmp6A0[3]=Cyc_PP_text(((
_tmp6A2=")",_tag_dynforward(_tmp6A2,sizeof(char),_get_zero_arr_size_char(_tmp6A2,
2))))),((_tmp6A0[2]=Cyc_Absynpp_exps2doc_prec(20,_tmp1F3),((_tmp6A0[1]=Cyc_PP_text(((
_tmp6A1="(",_tag_dynforward(_tmp6A1,sizeof(char),_get_zero_arr_size_char(_tmp6A1,
2))))),((_tmp6A0[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp1F2),Cyc_PP_cat(
_tag_dynforward(_tmp6A0,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1BD;_LL1D4:
if(*((int*)_tmp1DE)!= 11)goto _LL1D6;_tmp1F4=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp1DE)->f1;_tmp1F5=((struct Cyc_Absyn_FnCall_e_struct*)_tmp1DE)->f2;_LL1D5:{
const char*_tmp6A7;const char*_tmp6A6;struct Cyc_PP_Doc*_tmp6A5[4];s=((_tmp6A5[3]=
Cyc_PP_text(((_tmp6A7=")",_tag_dynforward(_tmp6A7,sizeof(char),
_get_zero_arr_size_char(_tmp6A7,2))))),((_tmp6A5[2]=Cyc_Absynpp_exps2doc_prec(20,
_tmp1F5),((_tmp6A5[1]=Cyc_PP_text(((_tmp6A6="(",_tag_dynforward(_tmp6A6,sizeof(
char),_get_zero_arr_size_char(_tmp6A6,2))))),((_tmp6A5[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1F4),Cyc_PP_cat(_tag_dynforward(_tmp6A5,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL1BD;_LL1D6: if(*((int*)_tmp1DE)!= 12)goto _LL1D8;_tmp1F6=((struct Cyc_Absyn_Throw_e_struct*)
_tmp1DE)->f1;_LL1D7:{const char*_tmp6AA;struct Cyc_PP_Doc*_tmp6A9[2];s=((_tmp6A9[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp1F6),((_tmp6A9[0]=Cyc_PP_text(((_tmp6AA="throw ",
_tag_dynforward(_tmp6AA,sizeof(char),_get_zero_arr_size_char(_tmp6AA,7))))),Cyc_PP_cat(
_tag_dynforward(_tmp6A9,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1BD;_LL1D8: if(*((
int*)_tmp1DE)!= 13)goto _LL1DA;_tmp1F7=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp1DE)->f1;_LL1D9: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1F7);goto _LL1BD;_LL1DA:
if(*((int*)_tmp1DE)!= 14)goto _LL1DC;_tmp1F8=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp1DE)->f1;_LL1DB: s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp1F8);goto _LL1BD;_LL1DC:
if(*((int*)_tmp1DE)!= 15)goto _LL1DE;_tmp1F9=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp1DE)->f1;_tmp1FA=((struct Cyc_Absyn_Cast_e_struct*)_tmp1DE)->f2;_LL1DD:{const
char*_tmp6AF;const char*_tmp6AE;struct Cyc_PP_Doc*_tmp6AD[4];s=((_tmp6AD[3]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1FA),((_tmp6AD[2]=Cyc_PP_text(((_tmp6AF=")",_tag_dynforward(_tmp6AF,
sizeof(char),_get_zero_arr_size_char(_tmp6AF,2))))),((_tmp6AD[1]=Cyc_Absynpp_typ2doc(
_tmp1F9),((_tmp6AD[0]=Cyc_PP_text(((_tmp6AE="(",_tag_dynforward(_tmp6AE,sizeof(
char),_get_zero_arr_size_char(_tmp6AE,2))))),Cyc_PP_cat(_tag_dynforward(_tmp6AD,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1BD;_LL1DE: if(*((int*)_tmp1DE)!= 
16)goto _LL1E0;_tmp1FB=((struct Cyc_Absyn_Address_e_struct*)_tmp1DE)->f1;_LL1DF:{
const char*_tmp6B2;struct Cyc_PP_Doc*_tmp6B1[2];s=((_tmp6B1[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1FB),((_tmp6B1[0]=Cyc_PP_text(((_tmp6B2="&",_tag_dynforward(_tmp6B2,
sizeof(char),_get_zero_arr_size_char(_tmp6B2,2))))),Cyc_PP_cat(_tag_dynforward(
_tmp6B1,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1BD;_LL1E0: if(*((int*)_tmp1DE)
!= 17)goto _LL1E2;_tmp1FC=((struct Cyc_Absyn_New_e_struct*)_tmp1DE)->f1;_tmp1FD=((
struct Cyc_Absyn_New_e_struct*)_tmp1DE)->f2;_LL1E1: if(_tmp1FC == 0){const char*
_tmp6B5;struct Cyc_PP_Doc*_tmp6B4[2];s=((_tmp6B4[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1FD),((_tmp6B4[0]=Cyc_PP_text(((_tmp6B5="new ",_tag_dynforward(
_tmp6B5,sizeof(char),_get_zero_arr_size_char(_tmp6B5,5))))),Cyc_PP_cat(
_tag_dynforward(_tmp6B4,sizeof(struct Cyc_PP_Doc*),2))))));}else{const char*
_tmp6BA;const char*_tmp6B9;struct Cyc_PP_Doc*_tmp6B8[4];s=((_tmp6B8[3]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp1FD),((_tmp6B8[2]=Cyc_PP_text(((_tmp6BA=") ",_tag_dynforward(_tmp6BA,
sizeof(char),_get_zero_arr_size_char(_tmp6BA,3))))),((_tmp6B8[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp1FC),((_tmp6B8[0]=Cyc_PP_text(((_tmp6B9="rnew(",
_tag_dynforward(_tmp6B9,sizeof(char),_get_zero_arr_size_char(_tmp6B9,6))))),Cyc_PP_cat(
_tag_dynforward(_tmp6B8,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1BD;_LL1E2:
if(*((int*)_tmp1DE)!= 18)goto _LL1E4;_tmp1FE=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp1DE)->f1;_LL1E3:{const char*_tmp6BF;const char*_tmp6BE;struct Cyc_PP_Doc*
_tmp6BD[3];s=((_tmp6BD[2]=Cyc_PP_text(((_tmp6BF=")",_tag_dynforward(_tmp6BF,
sizeof(char),_get_zero_arr_size_char(_tmp6BF,2))))),((_tmp6BD[1]=Cyc_Absynpp_typ2doc(
_tmp1FE),((_tmp6BD[0]=Cyc_PP_text(((_tmp6BE="sizeof(",_tag_dynforward(_tmp6BE,
sizeof(char),_get_zero_arr_size_char(_tmp6BE,8))))),Cyc_PP_cat(_tag_dynforward(
_tmp6BD,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1BD;_LL1E4: if(*((int*)
_tmp1DE)!= 19)goto _LL1E6;_tmp1FF=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp1DE)->f1;
_LL1E5:{const char*_tmp6C4;const char*_tmp6C3;struct Cyc_PP_Doc*_tmp6C2[3];s=((
_tmp6C2[2]=Cyc_PP_text(((_tmp6C4=")",_tag_dynforward(_tmp6C4,sizeof(char),
_get_zero_arr_size_char(_tmp6C4,2))))),((_tmp6C2[1]=Cyc_Absynpp_exp2doc(_tmp1FF),((
_tmp6C2[0]=Cyc_PP_text(((_tmp6C3="sizeof(",_tag_dynforward(_tmp6C3,sizeof(char),
_get_zero_arr_size_char(_tmp6C3,8))))),Cyc_PP_cat(_tag_dynforward(_tmp6C2,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1BD;_LL1E6: if(*((int*)_tmp1DE)!= 20)
goto _LL1E8;_tmp200=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1DE)->f1;
_tmp201=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1DE)->f2;if(*((int*)
_tmp201)!= 0)goto _LL1E8;_tmp202=((struct Cyc_Absyn_StructField_struct*)_tmp201)->f1;
_LL1E7:{const char*_tmp6CB;const char*_tmp6CA;const char*_tmp6C9;struct Cyc_PP_Doc*
_tmp6C8[5];s=((_tmp6C8[4]=Cyc_PP_text(((_tmp6CB=")",_tag_dynforward(_tmp6CB,
sizeof(char),_get_zero_arr_size_char(_tmp6CB,2))))),((_tmp6C8[3]=Cyc_PP_textptr(
_tmp202),((_tmp6C8[2]=Cyc_PP_text(((_tmp6CA=",",_tag_dynforward(_tmp6CA,sizeof(
char),_get_zero_arr_size_char(_tmp6CA,2))))),((_tmp6C8[1]=Cyc_Absynpp_typ2doc(
_tmp200),((_tmp6C8[0]=Cyc_PP_text(((_tmp6C9="offsetof(",_tag_dynforward(_tmp6C9,
sizeof(char),_get_zero_arr_size_char(_tmp6C9,10))))),Cyc_PP_cat(_tag_dynforward(
_tmp6C8,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1BD;_LL1E8: if(*((int*)
_tmp1DE)!= 20)goto _LL1EA;_tmp203=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp1DE)->f1;_tmp204=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp1DE)->f2;
if(*((int*)_tmp204)!= 1)goto _LL1EA;_tmp205=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp204)->f1;_LL1E9:{const char*_tmp6D9;struct Cyc_Int_pa_struct _tmp6D8;void*
_tmp6D7[1];const char*_tmp6D6;const char*_tmp6D5;const char*_tmp6D4;struct Cyc_PP_Doc*
_tmp6D3[5];s=((_tmp6D3[4]=Cyc_PP_text(((_tmp6D9=")",_tag_dynforward(_tmp6D9,
sizeof(char),_get_zero_arr_size_char(_tmp6D9,2))))),((_tmp6D3[3]=Cyc_PP_text((
struct _dynforward_ptr)((_tmp6D8.tag=1,((_tmp6D8.f1=(unsigned long)((int)_tmp205),((
_tmp6D7[0]=& _tmp6D8,Cyc_aprintf(((_tmp6D6="%d",_tag_dynforward(_tmp6D6,sizeof(
char),_get_zero_arr_size_char(_tmp6D6,3)))),_tag_dynforward(_tmp6D7,sizeof(void*),
1))))))))),((_tmp6D3[2]=Cyc_PP_text(((_tmp6D5=",",_tag_dynforward(_tmp6D5,
sizeof(char),_get_zero_arr_size_char(_tmp6D5,2))))),((_tmp6D3[1]=Cyc_Absynpp_typ2doc(
_tmp203),((_tmp6D3[0]=Cyc_PP_text(((_tmp6D4="offsetof(",_tag_dynforward(_tmp6D4,
sizeof(char),_get_zero_arr_size_char(_tmp6D4,10))))),Cyc_PP_cat(_tag_dynforward(
_tmp6D3,sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1BD;_LL1EA: if(*((int*)
_tmp1DE)!= 21)goto _LL1EC;_tmp206=((struct Cyc_Absyn_Gentyp_e_struct*)_tmp1DE)->f1;
_tmp207=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmp1DE)->f2;_LL1EB:{const char*
_tmp6DE;const char*_tmp6DD;struct Cyc_PP_Doc*_tmp6DC[4];s=((_tmp6DC[3]=Cyc_PP_text(((
_tmp6DE=")",_tag_dynforward(_tmp6DE,sizeof(char),_get_zero_arr_size_char(_tmp6DE,
2))))),((_tmp6DC[2]=Cyc_Absynpp_typ2doc(_tmp207),((_tmp6DC[1]=Cyc_Absynpp_tvars2doc(
_tmp206),((_tmp6DC[0]=Cyc_PP_text(((_tmp6DD="__gen(",_tag_dynforward(_tmp6DD,
sizeof(char),_get_zero_arr_size_char(_tmp6DD,7))))),Cyc_PP_cat(_tag_dynforward(
_tmp6DC,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1BD;_LL1EC: if(*((int*)
_tmp1DE)!= 22)goto _LL1EE;_tmp208=((struct Cyc_Absyn_Deref_e_struct*)_tmp1DE)->f1;
_LL1ED:{const char*_tmp6E1;struct Cyc_PP_Doc*_tmp6E0[2];s=((_tmp6E0[1]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp208),((_tmp6E0[0]=Cyc_PP_text(((_tmp6E1="*",_tag_dynforward(_tmp6E1,
sizeof(char),_get_zero_arr_size_char(_tmp6E1,2))))),Cyc_PP_cat(_tag_dynforward(
_tmp6E0,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1BD;_LL1EE: if(*((int*)_tmp1DE)
!= 23)goto _LL1F0;_tmp209=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1DE)->f1;
_tmp20A=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1DE)->f2;_LL1EF:{const char*
_tmp6E4;struct Cyc_PP_Doc*_tmp6E3[3];s=((_tmp6E3[2]=Cyc_PP_textptr(_tmp20A),((
_tmp6E3[1]=Cyc_PP_text(((_tmp6E4=".",_tag_dynforward(_tmp6E4,sizeof(char),
_get_zero_arr_size_char(_tmp6E4,2))))),((_tmp6E3[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp209),Cyc_PP_cat(_tag_dynforward(_tmp6E3,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1BD;_LL1F0: if(*((int*)_tmp1DE)!= 24)goto _LL1F2;_tmp20B=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp1DE)->f1;_tmp20C=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1DE)->f2;_LL1F1:{
const char*_tmp6E7;struct Cyc_PP_Doc*_tmp6E6[3];s=((_tmp6E6[2]=Cyc_PP_textptr(
_tmp20C),((_tmp6E6[1]=Cyc_PP_text(((_tmp6E7="->",_tag_dynforward(_tmp6E7,sizeof(
char),_get_zero_arr_size_char(_tmp6E7,3))))),((_tmp6E6[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp20B),Cyc_PP_cat(_tag_dynforward(_tmp6E6,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1BD;_LL1F2: if(*((int*)_tmp1DE)!= 25)goto _LL1F4;_tmp20D=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp1DE)->f1;_tmp20E=((struct Cyc_Absyn_Subscript_e_struct*)_tmp1DE)->f2;_LL1F3:{
const char*_tmp6EC;const char*_tmp6EB;struct Cyc_PP_Doc*_tmp6EA[4];s=((_tmp6EA[3]=
Cyc_PP_text(((_tmp6EC="]",_tag_dynforward(_tmp6EC,sizeof(char),
_get_zero_arr_size_char(_tmp6EC,2))))),((_tmp6EA[2]=Cyc_Absynpp_exp2doc(_tmp20E),((
_tmp6EA[1]=Cyc_PP_text(((_tmp6EB="[",_tag_dynforward(_tmp6EB,sizeof(char),
_get_zero_arr_size_char(_tmp6EB,2))))),((_tmp6EA[0]=Cyc_Absynpp_exp2doc_prec(
myprec,_tmp20D),Cyc_PP_cat(_tag_dynforward(_tmp6EA,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL1BD;_LL1F4: if(*((int*)_tmp1DE)!= 26)goto _LL1F6;_tmp20F=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp1DE)->f1;_LL1F5:{const char*_tmp6F1;const char*_tmp6F0;struct Cyc_PP_Doc*
_tmp6EF[4];s=((_tmp6EF[3]=Cyc_PP_text(((_tmp6F1=")",_tag_dynforward(_tmp6F1,
sizeof(char),_get_zero_arr_size_char(_tmp6F1,2))))),((_tmp6EF[2]=Cyc_Absynpp_exps2doc_prec(
20,_tmp20F),((_tmp6EF[1]=Cyc_PP_text(((_tmp6F0="(",_tag_dynforward(_tmp6F0,
sizeof(char),_get_zero_arr_size_char(_tmp6F0,2))))),((_tmp6EF[0]=Cyc_Absynpp_dollar(),
Cyc_PP_cat(_tag_dynforward(_tmp6EF,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto
_LL1BD;_LL1F6: if(*((int*)_tmp1DE)!= 27)goto _LL1F8;_tmp210=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp1DE)->f1;_tmp211=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp1DE)->f2;_LL1F7:{
const char*_tmp6F8;const char*_tmp6F7;const char*_tmp6F6;struct Cyc_PP_Doc*_tmp6F5[4];
s=((_tmp6F5[3]=Cyc_Absynpp_group_braces(((_tmp6F8=",",_tag_dynforward(_tmp6F8,
sizeof(char),_get_zero_arr_size_char(_tmp6F8,2)))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp211)),((_tmp6F5[2]=Cyc_PP_text(((_tmp6F7=")",_tag_dynforward(_tmp6F7,sizeof(
char),_get_zero_arr_size_char(_tmp6F7,2))))),((_tmp6F5[1]=Cyc_Absynpp_typ2doc((*
_tmp210).f3),((_tmp6F5[0]=Cyc_PP_text(((_tmp6F6="(",_tag_dynforward(_tmp6F6,
sizeof(char),_get_zero_arr_size_char(_tmp6F6,2))))),Cyc_PP_cat(_tag_dynforward(
_tmp6F5,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL1BD;_LL1F8: if(*((int*)
_tmp1DE)!= 28)goto _LL1FA;_tmp212=((struct Cyc_Absyn_Array_e_struct*)_tmp1DE)->f1;
_LL1F9:{const char*_tmp6F9;s=Cyc_Absynpp_group_braces(((_tmp6F9=",",
_tag_dynforward(_tmp6F9,sizeof(char),_get_zero_arr_size_char(_tmp6F9,2)))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp212));}goto _LL1BD;_LL1FA: if(*((int*)
_tmp1DE)!= 29)goto _LL1FC;_tmp213=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp1DE)->f1;_tmp214=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1DE)->f2;
_tmp215=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp1DE)->f3;_LL1FB:{const char*
_tmp700;const char*_tmp6FF;const char*_tmp6FE;struct Cyc_PP_Doc*_tmp6FD[8];s=((
_tmp6FD[7]=Cyc_Absynpp_rb(),((_tmp6FD[6]=Cyc_Absynpp_exp2doc(_tmp215),((_tmp6FD[
5]=Cyc_PP_text(((_tmp700=" : ",_tag_dynforward(_tmp700,sizeof(char),
_get_zero_arr_size_char(_tmp700,4))))),((_tmp6FD[4]=Cyc_Absynpp_exp2doc(_tmp214),((
_tmp6FD[3]=Cyc_PP_text(((_tmp6FF=" < ",_tag_dynforward(_tmp6FF,sizeof(char),
_get_zero_arr_size_char(_tmp6FF,4))))),((_tmp6FD[2]=Cyc_PP_text(*(*_tmp213->name).f2),((
_tmp6FD[1]=Cyc_PP_text(((_tmp6FE="for ",_tag_dynforward(_tmp6FE,sizeof(char),
_get_zero_arr_size_char(_tmp6FE,5))))),((_tmp6FD[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(
_tag_dynforward(_tmp6FD,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}goto
_LL1BD;_LL1FC: if(*((int*)_tmp1DE)!= 30)goto _LL1FE;_tmp216=((struct Cyc_Absyn_Struct_e_struct*)
_tmp1DE)->f1;_tmp217=((struct Cyc_Absyn_Struct_e_struct*)_tmp1DE)->f2;_tmp218=((
struct Cyc_Absyn_Struct_e_struct*)_tmp1DE)->f3;_LL1FD: {struct Cyc_List_List*
_tmp27B=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp218);{const char*_tmp705;
struct Cyc_List_List*_tmp704;struct Cyc_PP_Doc*_tmp703[2];s=((_tmp703[1]=Cyc_Absynpp_group_braces(((
_tmp705=",",_tag_dynforward(_tmp705,sizeof(char),_get_zero_arr_size_char(_tmp705,
2)))),_tmp217 != 0?(_tmp704=_cycalloc(sizeof(*_tmp704)),((_tmp704->hd=Cyc_Absynpp_tps2doc(
_tmp217),((_tmp704->tl=_tmp27B,_tmp704))))): _tmp27B),((_tmp703[0]=Cyc_Absynpp_qvar2doc(
_tmp216),Cyc_PP_cat(_tag_dynforward(_tmp703,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL1BD;}_LL1FE: if(*((int*)_tmp1DE)!= 31)goto _LL200;_tmp219=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp1DE)->f2;_LL1FF:{const char*_tmp706;s=Cyc_Absynpp_group_braces(((_tmp706=",",
_tag_dynforward(_tmp706,sizeof(char),_get_zero_arr_size_char(_tmp706,2)))),((
struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple9*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp219));}goto _LL1BD;_LL200: if(*((int*)
_tmp1DE)!= 32)goto _LL202;_tmp21A=((struct Cyc_Absyn_Tunion_e_struct*)_tmp1DE)->f1;
_tmp21B=((struct Cyc_Absyn_Tunion_e_struct*)_tmp1DE)->f3;_LL201: if(_tmp21A == 0)s=
Cyc_Absynpp_qvar2doc(_tmp21B->name);else{const char*_tmp70D;const char*_tmp70C;
const char*_tmp70B;struct Cyc_PP_Doc*_tmp70A[2];s=((_tmp70A[1]=Cyc_PP_egroup(((
_tmp70D="(",_tag_dynforward(_tmp70D,sizeof(char),_get_zero_arr_size_char(_tmp70D,
2)))),((_tmp70C=")",_tag_dynforward(_tmp70C,sizeof(char),_get_zero_arr_size_char(
_tmp70C,2)))),((_tmp70B=",",_tag_dynforward(_tmp70B,sizeof(char),
_get_zero_arr_size_char(_tmp70B,2)))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,
_tmp21A)),((_tmp70A[0]=Cyc_Absynpp_qvar2doc(_tmp21B->name),Cyc_PP_cat(
_tag_dynforward(_tmp70A,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL1BD;_LL202: if(*((
int*)_tmp1DE)!= 33)goto _LL204;_tmp21C=((struct Cyc_Absyn_Enum_e_struct*)_tmp1DE)->f1;
_LL203: s=Cyc_Absynpp_qvar2doc(_tmp21C);goto _LL1BD;_LL204: if(*((int*)_tmp1DE)!= 
34)goto _LL206;_tmp21D=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmp1DE)->f1;_LL205: s=
Cyc_Absynpp_qvar2doc(_tmp21D);goto _LL1BD;_LL206: if(*((int*)_tmp1DE)!= 35)goto
_LL208;_tmp21E=((struct Cyc_Absyn_Malloc_e_struct*)_tmp1DE)->f1;_tmp21F=_tmp21E.is_calloc;
_tmp220=_tmp21E.rgn;_tmp221=_tmp21E.elt_type;_tmp222=_tmp21E.num_elts;_LL207: if(
_tmp21F){struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(
_tmp221)),0);if(_tmp220 == 0){const char*_tmp714;const char*_tmp713;const char*
_tmp712;struct Cyc_PP_Doc*_tmp711[5];s=((_tmp711[4]=Cyc_PP_text(((_tmp714=")",
_tag_dynforward(_tmp714,sizeof(char),_get_zero_arr_size_char(_tmp714,2))))),((
_tmp711[3]=Cyc_Absynpp_exp2doc(st),((_tmp711[2]=Cyc_PP_text(((_tmp713=",",
_tag_dynforward(_tmp713,sizeof(char),_get_zero_arr_size_char(_tmp713,2))))),((
_tmp711[1]=Cyc_Absynpp_exp2doc(_tmp222),((_tmp711[0]=Cyc_PP_text(((_tmp712="calloc(",
_tag_dynforward(_tmp712,sizeof(char),_get_zero_arr_size_char(_tmp712,8))))),Cyc_PP_cat(
_tag_dynforward(_tmp711,sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{const char*
_tmp71D;const char*_tmp71C;const char*_tmp71B;const char*_tmp71A;struct Cyc_PP_Doc*
_tmp719[7];s=((_tmp719[6]=Cyc_PP_text(((_tmp71D=")",_tag_dynforward(_tmp71D,
sizeof(char),_get_zero_arr_size_char(_tmp71D,2))))),((_tmp719[5]=Cyc_Absynpp_exp2doc(
st),((_tmp719[4]=Cyc_PP_text(((_tmp71C=",",_tag_dynforward(_tmp71C,sizeof(char),
_get_zero_arr_size_char(_tmp71C,2))))),((_tmp719[3]=Cyc_Absynpp_exp2doc(_tmp222),((
_tmp719[2]=Cyc_PP_text(((_tmp71B=",",_tag_dynforward(_tmp71B,sizeof(char),
_get_zero_arr_size_char(_tmp71B,2))))),((_tmp719[1]=Cyc_Absynpp_exp2doc((struct
Cyc_Absyn_Exp*)_tmp220),((_tmp719[0]=Cyc_PP_text(((_tmp71A="rcalloc(",
_tag_dynforward(_tmp71A,sizeof(char),_get_zero_arr_size_char(_tmp71A,9))))),Cyc_PP_cat(
_tag_dynforward(_tmp719,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}else{
struct Cyc_Absyn_Exp*new_e;if(_tmp221 == 0)new_e=_tmp222;else{new_e=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmp221,0),_tmp222,0);}if(_tmp220 == 0){const char*
_tmp722;const char*_tmp721;struct Cyc_PP_Doc*_tmp720[3];s=((_tmp720[2]=Cyc_PP_text(((
_tmp722=")",_tag_dynforward(_tmp722,sizeof(char),_get_zero_arr_size_char(_tmp722,
2))))),((_tmp720[1]=Cyc_Absynpp_exp2doc(new_e),((_tmp720[0]=Cyc_PP_text(((
_tmp721="malloc(",_tag_dynforward(_tmp721,sizeof(char),_get_zero_arr_size_char(
_tmp721,8))))),Cyc_PP_cat(_tag_dynforward(_tmp720,sizeof(struct Cyc_PP_Doc*),3))))))));}
else{const char*_tmp729;const char*_tmp728;const char*_tmp727;struct Cyc_PP_Doc*
_tmp726[5];s=((_tmp726[4]=Cyc_PP_text(((_tmp729=")",_tag_dynforward(_tmp729,
sizeof(char),_get_zero_arr_size_char(_tmp729,2))))),((_tmp726[3]=Cyc_Absynpp_exp2doc(
new_e),((_tmp726[2]=Cyc_PP_text(((_tmp728=",",_tag_dynforward(_tmp728,sizeof(
char),_get_zero_arr_size_char(_tmp728,2))))),((_tmp726[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp220),((_tmp726[0]=Cyc_PP_text(((_tmp727="rmalloc(",
_tag_dynforward(_tmp727,sizeof(char),_get_zero_arr_size_char(_tmp727,9))))),Cyc_PP_cat(
_tag_dynforward(_tmp726,sizeof(struct Cyc_PP_Doc*),5))))))))))));}}goto _LL1BD;
_LL208: if(*((int*)_tmp1DE)!= 36)goto _LL20A;_tmp223=((struct Cyc_Absyn_Swap_e_struct*)
_tmp1DE)->f1;_tmp224=((struct Cyc_Absyn_Swap_e_struct*)_tmp1DE)->f2;_LL209:{const
char*_tmp730;const char*_tmp72F;const char*_tmp72E;struct Cyc_PP_Doc*_tmp72D[5];s=((
_tmp72D[4]=Cyc_PP_text(((_tmp730=")",_tag_dynforward(_tmp730,sizeof(char),
_get_zero_arr_size_char(_tmp730,2))))),((_tmp72D[3]=Cyc_Absynpp_exp2doc(_tmp224),((
_tmp72D[2]=Cyc_PP_text(((_tmp72F=",",_tag_dynforward(_tmp72F,sizeof(char),
_get_zero_arr_size_char(_tmp72F,2))))),((_tmp72D[1]=Cyc_Absynpp_exp2doc(_tmp223),((
_tmp72D[0]=Cyc_PP_text(((_tmp72E="cycswap(",_tag_dynforward(_tmp72E,sizeof(char),
_get_zero_arr_size_char(_tmp72E,9))))),Cyc_PP_cat(_tag_dynforward(_tmp72D,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL1BD;_LL20A: if(*((int*)_tmp1DE)!= 
37)goto _LL20C;_tmp225=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp1DE)->f1;
_tmp226=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp1DE)->f2;_LL20B:{const char*
_tmp731;s=Cyc_Absynpp_group_braces(((_tmp731=",",_tag_dynforward(_tmp731,sizeof(
char),_get_zero_arr_size_char(_tmp731,2)))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,
_tmp226));}goto _LL1BD;_LL20C: if(*((int*)_tmp1DE)!= 38)goto _LL1BD;_tmp227=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp1DE)->f1;_LL20D:{const char*_tmp736;const
char*_tmp735;struct Cyc_PP_Doc*_tmp734[7];s=((_tmp734[6]=Cyc_PP_text(((_tmp736=")",
_tag_dynforward(_tmp736,sizeof(char),_get_zero_arr_size_char(_tmp736,2))))),((
_tmp734[5]=Cyc_Absynpp_rb(),((_tmp734[4]=Cyc_PP_blank_doc(),((_tmp734[3]=Cyc_PP_nest(
2,Cyc_Absynpp_stmt2doc(_tmp227)),((_tmp734[2]=Cyc_PP_blank_doc(),((_tmp734[1]=
Cyc_Absynpp_lb(),((_tmp734[0]=Cyc_PP_text(((_tmp735="(",_tag_dynforward(_tmp735,
sizeof(char),_get_zero_arr_size_char(_tmp735,2))))),Cyc_PP_cat(_tag_dynforward(
_tmp734,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}goto _LL1BD;_LL1BD:;}if(
inprec >= myprec){const char*_tmp73B;const char*_tmp73A;struct Cyc_PP_Doc*_tmp739[3];
s=((_tmp739[2]=Cyc_PP_text(((_tmp73B=")",_tag_dynforward(_tmp73B,sizeof(char),
_get_zero_arr_size_char(_tmp73B,2))))),((_tmp739[1]=s,((_tmp739[0]=Cyc_PP_text(((
_tmp73A="(",_tag_dynforward(_tmp73A,sizeof(char),_get_zero_arr_size_char(_tmp73A,
2))))),Cyc_PP_cat(_tag_dynforward(_tmp739,sizeof(struct Cyc_PP_Doc*),3))))))));}
return s;}struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d);struct Cyc_PP_Doc*
Cyc_Absynpp_designator2doc(void*d){void*_tmp29F=d;struct Cyc_Absyn_Exp*_tmp2A0;
struct _dynforward_ptr*_tmp2A1;_LL218: if(*((int*)_tmp29F)!= 0)goto _LL21A;_tmp2A0=((
struct Cyc_Absyn_ArrayElement_struct*)_tmp29F)->f1;_LL219: {const char*_tmp740;
const char*_tmp73F;struct Cyc_PP_Doc*_tmp73E[3];return(_tmp73E[2]=Cyc_PP_text(((
_tmp740="]",_tag_dynforward(_tmp740,sizeof(char),_get_zero_arr_size_char(_tmp740,
2))))),((_tmp73E[1]=Cyc_Absynpp_exp2doc(_tmp2A0),((_tmp73E[0]=Cyc_PP_text(((
_tmp73F=".[",_tag_dynforward(_tmp73F,sizeof(char),_get_zero_arr_size_char(
_tmp73F,3))))),Cyc_PP_cat(_tag_dynforward(_tmp73E,sizeof(struct Cyc_PP_Doc*),3)))))));}
_LL21A: if(*((int*)_tmp29F)!= 1)goto _LL217;_tmp2A1=((struct Cyc_Absyn_FieldName_struct*)
_tmp29F)->f1;_LL21B: {const char*_tmp743;struct Cyc_PP_Doc*_tmp742[2];return(
_tmp742[1]=Cyc_PP_textptr(_tmp2A1),((_tmp742[0]=Cyc_PP_text(((_tmp743=".",
_tag_dynforward(_tmp743,sizeof(char),_get_zero_arr_size_char(_tmp743,2))))),Cyc_PP_cat(
_tag_dynforward(_tmp742,sizeof(struct Cyc_PP_Doc*),2)))));}_LL217:;}struct Cyc_PP_Doc*
Cyc_Absynpp_de2doc(struct _tuple9*de);struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct
_tuple9*de){if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{const char*
_tmp74A;const char*_tmp749;const char*_tmp748;struct Cyc_PP_Doc*_tmp747[2];return(
_tmp747[1]=Cyc_Absynpp_exp2doc((*de).f2),((_tmp747[0]=Cyc_PP_egroup(((_tmp74A="",
_tag_dynforward(_tmp74A,sizeof(char),_get_zero_arr_size_char(_tmp74A,1)))),((
_tmp749="=",_tag_dynforward(_tmp749,sizeof(char),_get_zero_arr_size_char(_tmp749,
2)))),((_tmp748="=",_tag_dynforward(_tmp748,sizeof(char),_get_zero_arr_size_char(
_tmp748,2)))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1)),Cyc_PP_cat(
_tag_dynforward(_tmp747,sizeof(struct Cyc_PP_Doc*),2)))));}}struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(
int inprec,struct Cyc_List_List*es);struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int
inprec,struct Cyc_List_List*es){const char*_tmp74D;const char*_tmp74C;const char*
_tmp74B;return Cyc_PP_group(((_tmp74B="",_tag_dynforward(_tmp74B,sizeof(char),
_get_zero_arr_size_char(_tmp74B,1)))),((_tmp74C="",_tag_dynforward(_tmp74C,
sizeof(char),_get_zero_arr_size_char(_tmp74C,1)))),((_tmp74D=",",_tag_dynforward(
_tmp74D,sizeof(char),_get_zero_arr_size_char(_tmp74D,2)))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))
Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(
union Cyc_Absyn_Cnst_union c);struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst_union
c){union Cyc_Absyn_Cnst_union _tmp2AE=c;void*_tmp2AF;char _tmp2B0;void*_tmp2B1;
short _tmp2B2;void*_tmp2B3;int _tmp2B4;void*_tmp2B5;int _tmp2B6;void*_tmp2B7;int
_tmp2B8;void*_tmp2B9;long long _tmp2BA;struct _dynforward_ptr _tmp2BB;struct
_dynforward_ptr _tmp2BC;_LL21D: if((_tmp2AE.Char_c).tag != 0)goto _LL21F;_tmp2AF=(
_tmp2AE.Char_c).f1;_tmp2B0=(_tmp2AE.Char_c).f2;_LL21E: {const char*_tmp751;void*
_tmp750[1];struct Cyc_String_pa_struct _tmp74F;return Cyc_PP_text((struct
_dynforward_ptr)((_tmp74F.tag=0,((_tmp74F.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_char_escape(_tmp2B0)),((_tmp750[0]=& _tmp74F,Cyc_aprintf(((
_tmp751="'%s'",_tag_dynforward(_tmp751,sizeof(char),_get_zero_arr_size_char(
_tmp751,5)))),_tag_dynforward(_tmp750,sizeof(void*),1)))))))));}_LL21F: if((
_tmp2AE.Short_c).tag != 1)goto _LL221;_tmp2B1=(_tmp2AE.Short_c).f1;_tmp2B2=(
_tmp2AE.Short_c).f2;_LL220: {const char*_tmp755;void*_tmp754[1];struct Cyc_Int_pa_struct
_tmp753;return Cyc_PP_text((struct _dynforward_ptr)((_tmp753.tag=1,((_tmp753.f1=(
unsigned long)((int)_tmp2B2),((_tmp754[0]=& _tmp753,Cyc_aprintf(((_tmp755="%d",
_tag_dynforward(_tmp755,sizeof(char),_get_zero_arr_size_char(_tmp755,3)))),
_tag_dynforward(_tmp754,sizeof(void*),1)))))))));}_LL221: if((_tmp2AE.Int_c).tag
!= 2)goto _LL223;_tmp2B3=(_tmp2AE.Int_c).f1;if((int)_tmp2B3 != 2)goto _LL223;
_tmp2B4=(_tmp2AE.Int_c).f2;_LL222: _tmp2B6=_tmp2B4;goto _LL224;_LL223: if((_tmp2AE.Int_c).tag
!= 2)goto _LL225;_tmp2B5=(_tmp2AE.Int_c).f1;if((int)_tmp2B5 != 0)goto _LL225;
_tmp2B6=(_tmp2AE.Int_c).f2;_LL224: {const char*_tmp759;void*_tmp758[1];struct Cyc_Int_pa_struct
_tmp757;return Cyc_PP_text((struct _dynforward_ptr)((_tmp757.tag=1,((_tmp757.f1=(
unsigned long)_tmp2B6,((_tmp758[0]=& _tmp757,Cyc_aprintf(((_tmp759="%d",
_tag_dynforward(_tmp759,sizeof(char),_get_zero_arr_size_char(_tmp759,3)))),
_tag_dynforward(_tmp758,sizeof(void*),1)))))))));}_LL225: if((_tmp2AE.Int_c).tag
!= 2)goto _LL227;_tmp2B7=(_tmp2AE.Int_c).f1;if((int)_tmp2B7 != 1)goto _LL227;
_tmp2B8=(_tmp2AE.Int_c).f2;_LL226: {const char*_tmp75D;void*_tmp75C[1];struct Cyc_Int_pa_struct
_tmp75B;return Cyc_PP_text((struct _dynforward_ptr)((_tmp75B.tag=1,((_tmp75B.f1=(
unsigned int)_tmp2B8,((_tmp75C[0]=& _tmp75B,Cyc_aprintf(((_tmp75D="%u",
_tag_dynforward(_tmp75D,sizeof(char),_get_zero_arr_size_char(_tmp75D,3)))),
_tag_dynforward(_tmp75C,sizeof(void*),1)))))))));}_LL227: if((_tmp2AE.LongLong_c).tag
!= 3)goto _LL229;_tmp2B9=(_tmp2AE.LongLong_c).f1;_tmp2BA=(_tmp2AE.LongLong_c).f2;
_LL228: {const char*_tmp75E;return Cyc_PP_text(((_tmp75E="<<FIX LONG LONG CONSTANT>>",
_tag_dynforward(_tmp75E,sizeof(char),_get_zero_arr_size_char(_tmp75E,27)))));}
_LL229: if((_tmp2AE.Float_c).tag != 4)goto _LL22B;_tmp2BB=(_tmp2AE.Float_c).f1;
_LL22A: return Cyc_PP_text(_tmp2BB);_LL22B: if((_tmp2AE.Null_c).tag != 6)goto _LL22D;
_LL22C: {const char*_tmp75F;return Cyc_PP_text(((_tmp75F="NULL",_tag_dynforward(
_tmp75F,sizeof(char),_get_zero_arr_size_char(_tmp75F,5)))));}_LL22D: if((_tmp2AE.String_c).tag
!= 5)goto _LL21C;_tmp2BC=(_tmp2AE.String_c).f1;_LL22E: {const char*_tmp764;const
char*_tmp763;struct Cyc_PP_Doc*_tmp762[3];return(_tmp762[2]=Cyc_PP_text(((_tmp764="\"",
_tag_dynforward(_tmp764,sizeof(char),_get_zero_arr_size_char(_tmp764,2))))),((
_tmp762[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp2BC)),((_tmp762[0]=Cyc_PP_text(((
_tmp763="\"",_tag_dynforward(_tmp763,sizeof(char),_get_zero_arr_size_char(
_tmp763,2))))),Cyc_PP_cat(_tag_dynforward(_tmp762,sizeof(struct Cyc_PP_Doc*),3)))))));}
_LL21C:;}struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,void*p,struct Cyc_List_List*
es);struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,void*p,struct Cyc_List_List*
es){struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);if(p == (void*)((void*)19)){if(es
== 0  || es->tl != 0){struct Cyc_Core_Failure_struct _tmp771;const char*_tmp770;void*
_tmp76F[1];struct Cyc_String_pa_struct _tmp76E;struct Cyc_Core_Failure_struct*
_tmp76D;(int)_throw((void*)((_tmp76D=_cycalloc(sizeof(*_tmp76D)),((_tmp76D[0]=((
_tmp771.tag=Cyc_Core_Failure,((_tmp771.f1=(struct _dynforward_ptr)((_tmp76E.tag=0,((
_tmp76E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_PP_string_of_doc(
ps,72)),((_tmp76F[0]=& _tmp76E,Cyc_aprintf(((_tmp770="Absynpp::primapp2doc Size: %s with bad args",
_tag_dynforward(_tmp770,sizeof(char),_get_zero_arr_size_char(_tmp770,44)))),
_tag_dynforward(_tmp76F,sizeof(void*),1)))))))),_tmp771)))),_tmp76D)))));}{
struct Cyc_PP_Doc*_tmp2D3=Cyc_Absynpp_exp2doc_prec(inprec,(struct Cyc_Absyn_Exp*)
es->hd);const char*_tmp774;struct Cyc_PP_Doc*_tmp773[2];return(_tmp773[1]=Cyc_PP_text(((
_tmp774=".size",_tag_dynforward(_tmp774,sizeof(char),_get_zero_arr_size_char(
_tmp774,6))))),((_tmp773[0]=_tmp2D3,Cyc_PP_cat(_tag_dynforward(_tmp773,sizeof(
struct Cyc_PP_Doc*),2)))));}}else{struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))
Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);if(ds == 0){struct Cyc_Core_Failure_struct
_tmp781;const char*_tmp780;void*_tmp77F[1];struct Cyc_String_pa_struct _tmp77E;
struct Cyc_Core_Failure_struct*_tmp77D;(int)_throw((void*)((_tmp77D=_cycalloc(
sizeof(*_tmp77D)),((_tmp77D[0]=((_tmp781.tag=Cyc_Core_Failure,((_tmp781.f1=(
struct _dynforward_ptr)((_tmp77E.tag=0,((_tmp77E.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp77F[0]=& _tmp77E,Cyc_aprintf(((
_tmp780="Absynpp::primapp2doc: %s with no args",_tag_dynforward(_tmp780,sizeof(
char),_get_zero_arr_size_char(_tmp780,38)))),_tag_dynforward(_tmp77F,sizeof(void*),
1)))))))),_tmp781)))),_tmp77D)))));}else{if(ds->tl == 0){const char*_tmp784;struct
Cyc_PP_Doc*_tmp783[3];return(_tmp783[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp783[1]=
Cyc_PP_text(((_tmp784=" ",_tag_dynforward(_tmp784,sizeof(char),
_get_zero_arr_size_char(_tmp784,2))))),((_tmp783[0]=ps,Cyc_PP_cat(
_tag_dynforward(_tmp783,sizeof(struct Cyc_PP_Doc*),3)))))));}else{if(((struct Cyc_List_List*)
_check_null(ds->tl))->tl != 0){struct Cyc_Core_Failure_struct _tmp791;const char*
_tmp790;void*_tmp78F[1];struct Cyc_String_pa_struct _tmp78E;struct Cyc_Core_Failure_struct*
_tmp78D;(int)_throw((void*)((_tmp78D=_cycalloc(sizeof(*_tmp78D)),((_tmp78D[0]=((
_tmp791.tag=Cyc_Core_Failure,((_tmp791.f1=(struct _dynforward_ptr)((_tmp78E.tag=0,((
_tmp78E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_PP_string_of_doc(
ps,72)),((_tmp78F[0]=& _tmp78E,Cyc_aprintf(((_tmp790="Absynpp::primapp2doc: %s with more than 2 args",
_tag_dynforward(_tmp790,sizeof(char),_get_zero_arr_size_char(_tmp790,47)))),
_tag_dynforward(_tmp78F,sizeof(void*),1)))))))),_tmp791)))),_tmp78D)))));}else{
const char*_tmp796;const char*_tmp795;struct Cyc_PP_Doc*_tmp794[5];return(_tmp794[4]=(
struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd,((_tmp794[3]=
Cyc_PP_text(((_tmp796=" ",_tag_dynforward(_tmp796,sizeof(char),
_get_zero_arr_size_char(_tmp796,2))))),((_tmp794[2]=ps,((_tmp794[1]=Cyc_PP_text(((
_tmp795=" ",_tag_dynforward(_tmp795,sizeof(char),_get_zero_arr_size_char(_tmp795,
2))))),((_tmp794[0]=(struct Cyc_PP_Doc*)ds->hd,Cyc_PP_cat(_tag_dynforward(_tmp794,
sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}struct _dynforward_ptr Cyc_Absynpp_prim2str(
void*p);struct _dynforward_ptr Cyc_Absynpp_prim2str(void*p){void*_tmp2E5=p;_LL230:
if((int)_tmp2E5 != 0)goto _LL232;_LL231: {const char*_tmp797;return(_tmp797="+",
_tag_dynforward(_tmp797,sizeof(char),_get_zero_arr_size_char(_tmp797,2)));}
_LL232: if((int)_tmp2E5 != 1)goto _LL234;_LL233: {const char*_tmp798;return(_tmp798="*",
_tag_dynforward(_tmp798,sizeof(char),_get_zero_arr_size_char(_tmp798,2)));}
_LL234: if((int)_tmp2E5 != 2)goto _LL236;_LL235: {const char*_tmp799;return(_tmp799="-",
_tag_dynforward(_tmp799,sizeof(char),_get_zero_arr_size_char(_tmp799,2)));}
_LL236: if((int)_tmp2E5 != 3)goto _LL238;_LL237: {const char*_tmp79A;return(_tmp79A="/",
_tag_dynforward(_tmp79A,sizeof(char),_get_zero_arr_size_char(_tmp79A,2)));}
_LL238: if((int)_tmp2E5 != 4)goto _LL23A;_LL239: {const char*_tmp79B;return(_tmp79B="%",
_tag_dynforward(_tmp79B,sizeof(char),_get_zero_arr_size_char(_tmp79B,2)));}
_LL23A: if((int)_tmp2E5 != 5)goto _LL23C;_LL23B: {const char*_tmp79C;return(_tmp79C="==",
_tag_dynforward(_tmp79C,sizeof(char),_get_zero_arr_size_char(_tmp79C,3)));}
_LL23C: if((int)_tmp2E5 != 6)goto _LL23E;_LL23D: {const char*_tmp79D;return(_tmp79D="!=",
_tag_dynforward(_tmp79D,sizeof(char),_get_zero_arr_size_char(_tmp79D,3)));}
_LL23E: if((int)_tmp2E5 != 7)goto _LL240;_LL23F: {const char*_tmp79E;return(_tmp79E=">",
_tag_dynforward(_tmp79E,sizeof(char),_get_zero_arr_size_char(_tmp79E,2)));}
_LL240: if((int)_tmp2E5 != 8)goto _LL242;_LL241: {const char*_tmp79F;return(_tmp79F="<",
_tag_dynforward(_tmp79F,sizeof(char),_get_zero_arr_size_char(_tmp79F,2)));}
_LL242: if((int)_tmp2E5 != 9)goto _LL244;_LL243: {const char*_tmp7A0;return(_tmp7A0=">=",
_tag_dynforward(_tmp7A0,sizeof(char),_get_zero_arr_size_char(_tmp7A0,3)));}
_LL244: if((int)_tmp2E5 != 10)goto _LL246;_LL245: {const char*_tmp7A1;return(_tmp7A1="<=",
_tag_dynforward(_tmp7A1,sizeof(char),_get_zero_arr_size_char(_tmp7A1,3)));}
_LL246: if((int)_tmp2E5 != 11)goto _LL248;_LL247: {const char*_tmp7A2;return(_tmp7A2="!",
_tag_dynforward(_tmp7A2,sizeof(char),_get_zero_arr_size_char(_tmp7A2,2)));}
_LL248: if((int)_tmp2E5 != 12)goto _LL24A;_LL249: {const char*_tmp7A3;return(_tmp7A3="~",
_tag_dynforward(_tmp7A3,sizeof(char),_get_zero_arr_size_char(_tmp7A3,2)));}
_LL24A: if((int)_tmp2E5 != 13)goto _LL24C;_LL24B: {const char*_tmp7A4;return(_tmp7A4="&",
_tag_dynforward(_tmp7A4,sizeof(char),_get_zero_arr_size_char(_tmp7A4,2)));}
_LL24C: if((int)_tmp2E5 != 14)goto _LL24E;_LL24D: {const char*_tmp7A5;return(_tmp7A5="|",
_tag_dynforward(_tmp7A5,sizeof(char),_get_zero_arr_size_char(_tmp7A5,2)));}
_LL24E: if((int)_tmp2E5 != 15)goto _LL250;_LL24F: {const char*_tmp7A6;return(_tmp7A6="^",
_tag_dynforward(_tmp7A6,sizeof(char),_get_zero_arr_size_char(_tmp7A6,2)));}
_LL250: if((int)_tmp2E5 != 16)goto _LL252;_LL251: {const char*_tmp7A7;return(_tmp7A7="<<",
_tag_dynforward(_tmp7A7,sizeof(char),_get_zero_arr_size_char(_tmp7A7,3)));}
_LL252: if((int)_tmp2E5 != 17)goto _LL254;_LL253: {const char*_tmp7A8;return(_tmp7A8=">>",
_tag_dynforward(_tmp7A8,sizeof(char),_get_zero_arr_size_char(_tmp7A8,3)));}
_LL254: if((int)_tmp2E5 != 18)goto _LL256;_LL255: {const char*_tmp7A9;return(_tmp7A9=">>>",
_tag_dynforward(_tmp7A9,sizeof(char),_get_zero_arr_size_char(_tmp7A9,4)));}
_LL256: if((int)_tmp2E5 != 19)goto _LL22F;_LL257: {const char*_tmp7AA;return(_tmp7AA="size",
_tag_dynforward(_tmp7AA,sizeof(char),_get_zero_arr_size_char(_tmp7AA,5)));}
_LL22F:;}struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(void*p);struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(
void*p){return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s);int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp2FA=(void*)s->r;_LL259: if(_tmp2FA <= (void*)1)goto _LL25B;if(*((int*)
_tmp2FA)!= 11)goto _LL25B;_LL25A: return 1;_LL25B:;_LL25C: return 0;_LL258:;}struct
Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st);struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(
struct Cyc_Absyn_Stmt*st){struct Cyc_PP_Doc*s;{void*_tmp2FB=(void*)st->r;struct Cyc_Absyn_Exp*
_tmp2FC;struct Cyc_Absyn_Stmt*_tmp2FD;struct Cyc_Absyn_Stmt*_tmp2FE;struct Cyc_Absyn_Exp*
_tmp2FF;struct Cyc_Absyn_Exp*_tmp300;struct Cyc_Absyn_Stmt*_tmp301;struct Cyc_Absyn_Stmt*
_tmp302;struct _tuple2 _tmp303;struct Cyc_Absyn_Exp*_tmp304;struct Cyc_Absyn_Stmt*
_tmp305;struct _dynforward_ptr*_tmp306;struct Cyc_Absyn_Exp*_tmp307;struct _tuple2
_tmp308;struct Cyc_Absyn_Exp*_tmp309;struct _tuple2 _tmp30A;struct Cyc_Absyn_Exp*
_tmp30B;struct Cyc_Absyn_Stmt*_tmp30C;struct Cyc_Absyn_Exp*_tmp30D;struct Cyc_List_List*
_tmp30E;struct Cyc_List_List*_tmp30F;struct Cyc_List_List*_tmp310;struct Cyc_Absyn_Decl*
_tmp311;struct Cyc_Absyn_Stmt*_tmp312;struct _dynforward_ptr*_tmp313;struct Cyc_Absyn_Stmt*
_tmp314;struct Cyc_Absyn_Stmt*_tmp315;struct _tuple2 _tmp316;struct Cyc_Absyn_Exp*
_tmp317;struct Cyc_Absyn_Stmt*_tmp318;struct Cyc_List_List*_tmp319;struct Cyc_Absyn_Tvar*
_tmp31A;struct Cyc_Absyn_Vardecl*_tmp31B;int _tmp31C;struct Cyc_Absyn_Exp*_tmp31D;
struct Cyc_Absyn_Stmt*_tmp31E;struct Cyc_Absyn_Exp*_tmp31F;struct Cyc_Absyn_Exp*
_tmp320;struct Cyc_Absyn_Tvar*_tmp321;struct Cyc_Absyn_Vardecl*_tmp322;struct Cyc_Absyn_Stmt*
_tmp323;_LL25E: if((int)_tmp2FB != 0)goto _LL260;_LL25F:{const char*_tmp7AB;s=Cyc_PP_text(((
_tmp7AB=";",_tag_dynforward(_tmp7AB,sizeof(char),_get_zero_arr_size_char(_tmp7AB,
2)))));}goto _LL25D;_LL260: if(_tmp2FB <= (void*)1)goto _LL262;if(*((int*)_tmp2FB)!= 
0)goto _LL262;_tmp2FC=((struct Cyc_Absyn_Exp_s_struct*)_tmp2FB)->f1;_LL261:{const
char*_tmp7AE;struct Cyc_PP_Doc*_tmp7AD[2];s=((_tmp7AD[1]=Cyc_PP_text(((_tmp7AE=";",
_tag_dynforward(_tmp7AE,sizeof(char),_get_zero_arr_size_char(_tmp7AE,2))))),((
_tmp7AD[0]=Cyc_Absynpp_exp2doc(_tmp2FC),Cyc_PP_cat(_tag_dynforward(_tmp7AD,
sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL25D;_LL262: if(_tmp2FB <= (void*)1)goto
_LL264;if(*((int*)_tmp2FB)!= 1)goto _LL264;_tmp2FD=((struct Cyc_Absyn_Seq_s_struct*)
_tmp2FB)->f1;_tmp2FE=((struct Cyc_Absyn_Seq_s_struct*)_tmp2FB)->f2;_LL263: if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp2FD)){struct Cyc_PP_Doc*_tmp7B1[5];struct Cyc_PP_Doc*
_tmp7B0[7];s=((_tmp7B0[6]=Cyc_Absynpp_is_declaration(_tmp2FE)?(_tmp7B1[4]=Cyc_PP_line_doc(),((
_tmp7B1[3]=Cyc_Absynpp_rb(),((_tmp7B1[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp2FE)),((_tmp7B1[1]=Cyc_PP_blank_doc(),((_tmp7B1[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(
_tag_dynforward(_tmp7B1,sizeof(struct Cyc_PP_Doc*),5))))))))))): Cyc_Absynpp_stmt2doc(
_tmp2FE),((_tmp7B0[5]=Cyc_PP_line_doc(),((_tmp7B0[4]=Cyc_Absynpp_rb(),((_tmp7B0[
3]=Cyc_PP_line_doc(),((_tmp7B0[2]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2FD)),((
_tmp7B0[1]=Cyc_PP_blank_doc(),((_tmp7B0[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(
_tag_dynforward(_tmp7B0,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{if(Cyc_Absynpp_is_declaration(
_tmp2FE)){struct Cyc_PP_Doc*_tmp7B2[7];s=((_tmp7B2[6]=Cyc_PP_line_doc(),((_tmp7B2[
5]=Cyc_Absynpp_rb(),((_tmp7B2[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2FE)),((
_tmp7B2[3]=Cyc_PP_blank_doc(),((_tmp7B2[2]=Cyc_Absynpp_lb(),((_tmp7B2[1]=Cyc_PP_line_doc(),((
_tmp7B2[0]=Cyc_Absynpp_stmt2doc(_tmp2FD),Cyc_PP_cat(_tag_dynforward(_tmp7B2,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{struct Cyc_List_List*_tmp7B6;
struct Cyc_List_List*_tmp7B5;const char*_tmp7B3;s=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Stmt*),struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_stmt2doc,((_tmp7B3="",_tag_dynforward(_tmp7B3,sizeof(char),
_get_zero_arr_size_char(_tmp7B3,1)))),((_tmp7B5=_cycalloc(sizeof(*_tmp7B5)),((
_tmp7B5->hd=_tmp2FD,((_tmp7B5->tl=((_tmp7B6=_cycalloc(sizeof(*_tmp7B6)),((
_tmp7B6->hd=_tmp2FE,((_tmp7B6->tl=0,_tmp7B6)))))),_tmp7B5)))))));}}}else{struct
Cyc_List_List*_tmp7BA;struct Cyc_List_List*_tmp7B9;const char*_tmp7B7;s=((struct
Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Stmt*),struct
_dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_stmt2doc,((
_tmp7B7="",_tag_dynforward(_tmp7B7,sizeof(char),_get_zero_arr_size_char(_tmp7B7,
1)))),((_tmp7B9=_cycalloc(sizeof(*_tmp7B9)),((_tmp7B9->hd=_tmp2FD,((_tmp7B9->tl=((
_tmp7BA=_cycalloc(sizeof(*_tmp7BA)),((_tmp7BA->hd=_tmp2FE,((_tmp7BA->tl=0,
_tmp7BA)))))),_tmp7B9)))))));}goto _LL25D;_LL264: if(_tmp2FB <= (void*)1)goto _LL266;
if(*((int*)_tmp2FB)!= 2)goto _LL266;_tmp2FF=((struct Cyc_Absyn_Return_s_struct*)
_tmp2FB)->f1;_LL265: if(_tmp2FF == 0){const char*_tmp7BB;s=Cyc_PP_text(((_tmp7BB="return;",
_tag_dynforward(_tmp7BB,sizeof(char),_get_zero_arr_size_char(_tmp7BB,8)))));}
else{const char*_tmp7C0;const char*_tmp7BF;struct Cyc_PP_Doc*_tmp7BE[3];s=((_tmp7BE[
2]=Cyc_PP_text(((_tmp7C0=";",_tag_dynforward(_tmp7C0,sizeof(char),
_get_zero_arr_size_char(_tmp7C0,2))))),((_tmp7BE[1]=_tmp2FF == 0?Cyc_PP_nil_doc():
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2FF),((_tmp7BE[0]=Cyc_PP_text(((
_tmp7BF="return ",_tag_dynforward(_tmp7BF,sizeof(char),_get_zero_arr_size_char(
_tmp7BF,8))))),Cyc_PP_cat(_tag_dynforward(_tmp7BE,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL25D;_LL266: if(_tmp2FB <= (void*)1)goto _LL268;if(*((int*)_tmp2FB)!= 3)goto
_LL268;_tmp300=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2FB)->f1;_tmp301=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp2FB)->f2;_tmp302=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp2FB)->f3;_LL267: {int print_else;{void*_tmp334=(void*)_tmp302->r;_LL287: if((
int)_tmp334 != 0)goto _LL289;_LL288: print_else=0;goto _LL286;_LL289:;_LL28A:
print_else=1;goto _LL286;_LL286:;}{struct Cyc_PP_Doc*_tmp7CF[6];const char*_tmp7CE;
struct Cyc_PP_Doc*_tmp7CD[2];struct Cyc_PP_Doc*_tmp7CC[2];const char*_tmp7CB;const
char*_tmp7CA;struct Cyc_PP_Doc*_tmp7C9[8];s=((_tmp7C9[7]=print_else?(_tmp7CF[5]=
Cyc_Absynpp_rb(),((_tmp7CF[4]=Cyc_PP_line_doc(),((_tmp7CF[3]=Cyc_PP_nest(2,((
_tmp7CD[1]=Cyc_Absynpp_stmt2doc(_tmp302),((_tmp7CD[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dynforward(_tmp7CD,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp7CF[2]=Cyc_Absynpp_lb(),((
_tmp7CF[1]=Cyc_PP_text(((_tmp7CE="else ",_tag_dynforward(_tmp7CE,sizeof(char),
_get_zero_arr_size_char(_tmp7CE,6))))),((_tmp7CF[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dynforward(_tmp7CF,sizeof(struct Cyc_PP_Doc*),6))))))))))))): Cyc_PP_nil_doc(),((
_tmp7C9[6]=Cyc_Absynpp_rb(),((_tmp7C9[5]=Cyc_PP_line_doc(),((_tmp7C9[4]=Cyc_PP_nest(
2,((_tmp7CC[1]=Cyc_Absynpp_stmt2doc(_tmp301),((_tmp7CC[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dynforward(_tmp7CC,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp7C9[3]=Cyc_Absynpp_lb(),((
_tmp7C9[2]=Cyc_PP_text(((_tmp7CB=") ",_tag_dynforward(_tmp7CB,sizeof(char),
_get_zero_arr_size_char(_tmp7CB,3))))),((_tmp7C9[1]=Cyc_Absynpp_exp2doc(_tmp300),((
_tmp7C9[0]=Cyc_PP_text(((_tmp7CA="if (",_tag_dynforward(_tmp7CA,sizeof(char),
_get_zero_arr_size_char(_tmp7CA,5))))),Cyc_PP_cat(_tag_dynforward(_tmp7C9,
sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}goto _LL25D;}_LL268: if(_tmp2FB <= (
void*)1)goto _LL26A;if(*((int*)_tmp2FB)!= 4)goto _LL26A;_tmp303=((struct Cyc_Absyn_While_s_struct*)
_tmp2FB)->f1;_tmp304=_tmp303.f1;_tmp305=((struct Cyc_Absyn_While_s_struct*)
_tmp2FB)->f2;_LL269:{struct Cyc_PP_Doc*_tmp7D6[2];const char*_tmp7D5;const char*
_tmp7D4;struct Cyc_PP_Doc*_tmp7D3[7];s=((_tmp7D3[6]=Cyc_Absynpp_rb(),((_tmp7D3[5]=
Cyc_PP_line_doc(),((_tmp7D3[4]=Cyc_PP_nest(2,((_tmp7D6[1]=Cyc_Absynpp_stmt2doc(
_tmp305),((_tmp7D6[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dynforward(_tmp7D6,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp7D3[3]=Cyc_Absynpp_lb(),((_tmp7D3[2]=
Cyc_PP_text(((_tmp7D5=") ",_tag_dynforward(_tmp7D5,sizeof(char),
_get_zero_arr_size_char(_tmp7D5,3))))),((_tmp7D3[1]=Cyc_Absynpp_exp2doc(_tmp304),((
_tmp7D3[0]=Cyc_PP_text(((_tmp7D4="while (",_tag_dynforward(_tmp7D4,sizeof(char),
_get_zero_arr_size_char(_tmp7D4,8))))),Cyc_PP_cat(_tag_dynforward(_tmp7D3,
sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}goto _LL25D;_LL26A: if(_tmp2FB <= (
void*)1)goto _LL26C;if(*((int*)_tmp2FB)!= 5)goto _LL26C;_LL26B:{const char*_tmp7D7;
s=Cyc_PP_text(((_tmp7D7="break;",_tag_dynforward(_tmp7D7,sizeof(char),
_get_zero_arr_size_char(_tmp7D7,7)))));}goto _LL25D;_LL26C: if(_tmp2FB <= (void*)1)
goto _LL26E;if(*((int*)_tmp2FB)!= 6)goto _LL26E;_LL26D:{const char*_tmp7D8;s=Cyc_PP_text(((
_tmp7D8="continue;",_tag_dynforward(_tmp7D8,sizeof(char),_get_zero_arr_size_char(
_tmp7D8,10)))));}goto _LL25D;_LL26E: if(_tmp2FB <= (void*)1)goto _LL270;if(*((int*)
_tmp2FB)!= 7)goto _LL270;_tmp306=((struct Cyc_Absyn_Goto_s_struct*)_tmp2FB)->f1;
_LL26F:{const char*_tmp7DC;void*_tmp7DB[1];struct Cyc_String_pa_struct _tmp7DA;s=
Cyc_PP_text((struct _dynforward_ptr)((_tmp7DA.tag=0,((_tmp7DA.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp306),((_tmp7DB[0]=& _tmp7DA,Cyc_aprintf(((
_tmp7DC="goto %s;",_tag_dynforward(_tmp7DC,sizeof(char),_get_zero_arr_size_char(
_tmp7DC,9)))),_tag_dynforward(_tmp7DB,sizeof(void*),1)))))))));}goto _LL25D;
_LL270: if(_tmp2FB <= (void*)1)goto _LL272;if(*((int*)_tmp2FB)!= 8)goto _LL272;
_tmp307=((struct Cyc_Absyn_For_s_struct*)_tmp2FB)->f1;_tmp308=((struct Cyc_Absyn_For_s_struct*)
_tmp2FB)->f2;_tmp309=_tmp308.f1;_tmp30A=((struct Cyc_Absyn_For_s_struct*)_tmp2FB)->f3;
_tmp30B=_tmp30A.f1;_tmp30C=((struct Cyc_Absyn_For_s_struct*)_tmp2FB)->f4;_LL271:{
struct Cyc_PP_Doc*_tmp7E7[2];const char*_tmp7E6;const char*_tmp7E5;const char*
_tmp7E4;const char*_tmp7E3;struct Cyc_PP_Doc*_tmp7E2[11];s=((_tmp7E2[10]=Cyc_Absynpp_rb(),((
_tmp7E2[9]=Cyc_PP_line_doc(),((_tmp7E2[8]=Cyc_PP_nest(2,((_tmp7E7[1]=Cyc_Absynpp_stmt2doc(
_tmp30C),((_tmp7E7[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dynforward(_tmp7E7,
sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp7E2[7]=Cyc_Absynpp_lb(),((_tmp7E2[6]=
Cyc_PP_text(((_tmp7E6=") ",_tag_dynforward(_tmp7E6,sizeof(char),
_get_zero_arr_size_char(_tmp7E6,3))))),((_tmp7E2[5]=Cyc_Absynpp_exp2doc(_tmp30B),((
_tmp7E2[4]=Cyc_PP_text(((_tmp7E5="; ",_tag_dynforward(_tmp7E5,sizeof(char),
_get_zero_arr_size_char(_tmp7E5,3))))),((_tmp7E2[3]=Cyc_Absynpp_exp2doc(_tmp309),((
_tmp7E2[2]=Cyc_PP_text(((_tmp7E4="; ",_tag_dynforward(_tmp7E4,sizeof(char),
_get_zero_arr_size_char(_tmp7E4,3))))),((_tmp7E2[1]=Cyc_Absynpp_exp2doc(_tmp307),((
_tmp7E2[0]=Cyc_PP_text(((_tmp7E3="for(",_tag_dynforward(_tmp7E3,sizeof(char),
_get_zero_arr_size_char(_tmp7E3,5))))),Cyc_PP_cat(_tag_dynforward(_tmp7E2,
sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}goto _LL25D;_LL272: if(
_tmp2FB <= (void*)1)goto _LL274;if(*((int*)_tmp2FB)!= 9)goto _LL274;_tmp30D=((
struct Cyc_Absyn_Switch_s_struct*)_tmp2FB)->f1;_tmp30E=((struct Cyc_Absyn_Switch_s_struct*)
_tmp2FB)->f2;_LL273:{const char*_tmp7EC;const char*_tmp7EB;struct Cyc_PP_Doc*
_tmp7EA[8];s=((_tmp7EA[7]=Cyc_Absynpp_rb(),((_tmp7EA[6]=Cyc_PP_line_doc(),((
_tmp7EA[5]=Cyc_Absynpp_switchclauses2doc(_tmp30E),((_tmp7EA[4]=Cyc_PP_line_doc(),((
_tmp7EA[3]=Cyc_Absynpp_lb(),((_tmp7EA[2]=Cyc_PP_text(((_tmp7EC=") ",
_tag_dynforward(_tmp7EC,sizeof(char),_get_zero_arr_size_char(_tmp7EC,3))))),((
_tmp7EA[1]=Cyc_Absynpp_exp2doc(_tmp30D),((_tmp7EA[0]=Cyc_PP_text(((_tmp7EB="switch (",
_tag_dynforward(_tmp7EB,sizeof(char),_get_zero_arr_size_char(_tmp7EB,9))))),Cyc_PP_cat(
_tag_dynforward(_tmp7EA,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}goto
_LL25D;_LL274: if(_tmp2FB <= (void*)1)goto _LL276;if(*((int*)_tmp2FB)!= 10)goto
_LL276;_tmp30F=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp2FB)->f1;if(_tmp30F != 0)
goto _LL276;_LL275:{const char*_tmp7ED;s=Cyc_PP_text(((_tmp7ED="fallthru;",
_tag_dynforward(_tmp7ED,sizeof(char),_get_zero_arr_size_char(_tmp7ED,10)))));}
goto _LL25D;_LL276: if(_tmp2FB <= (void*)1)goto _LL278;if(*((int*)_tmp2FB)!= 10)goto
_LL278;_tmp310=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp2FB)->f1;_LL277:{const
char*_tmp7F2;const char*_tmp7F1;struct Cyc_PP_Doc*_tmp7F0[3];s=((_tmp7F0[2]=Cyc_PP_text(((
_tmp7F2=");",_tag_dynforward(_tmp7F2,sizeof(char),_get_zero_arr_size_char(
_tmp7F2,3))))),((_tmp7F0[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp310),((_tmp7F0[0]=
Cyc_PP_text(((_tmp7F1="fallthru(",_tag_dynforward(_tmp7F1,sizeof(char),
_get_zero_arr_size_char(_tmp7F1,10))))),Cyc_PP_cat(_tag_dynforward(_tmp7F0,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL25D;_LL278: if(_tmp2FB <= (void*)1)
goto _LL27A;if(*((int*)_tmp2FB)!= 11)goto _LL27A;_tmp311=((struct Cyc_Absyn_Decl_s_struct*)
_tmp2FB)->f1;_tmp312=((struct Cyc_Absyn_Decl_s_struct*)_tmp2FB)->f2;_LL279:{
struct Cyc_PP_Doc*_tmp7F3[3];s=((_tmp7F3[2]=Cyc_Absynpp_stmt2doc(_tmp312),((
_tmp7F3[1]=Cyc_PP_line_doc(),((_tmp7F3[0]=Cyc_Absynpp_decl2doc(_tmp311),Cyc_PP_cat(
_tag_dynforward(_tmp7F3,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL25D;_LL27A:
if(_tmp2FB <= (void*)1)goto _LL27C;if(*((int*)_tmp2FB)!= 12)goto _LL27C;_tmp313=((
struct Cyc_Absyn_Label_s_struct*)_tmp2FB)->f1;_tmp314=((struct Cyc_Absyn_Label_s_struct*)
_tmp2FB)->f2;_LL27B: if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(
_tmp314)){const char*_tmp7F6;struct Cyc_PP_Doc*_tmp7F5[7];s=((_tmp7F5[6]=Cyc_Absynpp_rb(),((
_tmp7F5[5]=Cyc_PP_line_doc(),((_tmp7F5[4]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp314)),((_tmp7F5[3]=Cyc_PP_line_doc(),((_tmp7F5[2]=Cyc_Absynpp_lb(),((_tmp7F5[
1]=Cyc_PP_text(((_tmp7F6=": ",_tag_dynforward(_tmp7F6,sizeof(char),
_get_zero_arr_size_char(_tmp7F6,3))))),((_tmp7F5[0]=Cyc_PP_textptr(_tmp313),Cyc_PP_cat(
_tag_dynforward(_tmp7F5,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{const
char*_tmp7F9;struct Cyc_PP_Doc*_tmp7F8[3];s=((_tmp7F8[2]=Cyc_Absynpp_stmt2doc(
_tmp314),((_tmp7F8[1]=Cyc_PP_text(((_tmp7F9=": ",_tag_dynforward(_tmp7F9,sizeof(
char),_get_zero_arr_size_char(_tmp7F9,3))))),((_tmp7F8[0]=Cyc_PP_textptr(_tmp313),
Cyc_PP_cat(_tag_dynforward(_tmp7F8,sizeof(struct Cyc_PP_Doc*),3))))))));}goto
_LL25D;_LL27C: if(_tmp2FB <= (void*)1)goto _LL27E;if(*((int*)_tmp2FB)!= 13)goto
_LL27E;_tmp315=((struct Cyc_Absyn_Do_s_struct*)_tmp2FB)->f1;_tmp316=((struct Cyc_Absyn_Do_s_struct*)
_tmp2FB)->f2;_tmp317=_tmp316.f1;_LL27D:{const char*_tmp800;const char*_tmp7FF;
const char*_tmp7FE;struct Cyc_PP_Doc*_tmp7FD[9];s=((_tmp7FD[8]=Cyc_PP_text(((
_tmp800=");",_tag_dynforward(_tmp800,sizeof(char),_get_zero_arr_size_char(
_tmp800,3))))),((_tmp7FD[7]=Cyc_Absynpp_exp2doc(_tmp317),((_tmp7FD[6]=Cyc_PP_text(((
_tmp7FF=" while (",_tag_dynforward(_tmp7FF,sizeof(char),_get_zero_arr_size_char(
_tmp7FF,9))))),((_tmp7FD[5]=Cyc_Absynpp_rb(),((_tmp7FD[4]=Cyc_PP_line_doc(),((
_tmp7FD[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp315)),((_tmp7FD[2]=Cyc_PP_line_doc(),((
_tmp7FD[1]=Cyc_Absynpp_lb(),((_tmp7FD[0]=Cyc_PP_text(((_tmp7FE="do ",
_tag_dynforward(_tmp7FE,sizeof(char),_get_zero_arr_size_char(_tmp7FE,4))))),Cyc_PP_cat(
_tag_dynforward(_tmp7FD,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto
_LL25D;_LL27E: if(_tmp2FB <= (void*)1)goto _LL280;if(*((int*)_tmp2FB)!= 14)goto
_LL280;_tmp318=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp2FB)->f1;_tmp319=((
struct Cyc_Absyn_TryCatch_s_struct*)_tmp2FB)->f2;_LL27F:{const char*_tmp805;const
char*_tmp804;struct Cyc_PP_Doc*_tmp803[12];s=((_tmp803[11]=Cyc_Absynpp_rb(),((
_tmp803[10]=Cyc_PP_line_doc(),((_tmp803[9]=Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(
_tmp319)),((_tmp803[8]=Cyc_PP_line_doc(),((_tmp803[7]=Cyc_Absynpp_lb(),((_tmp803[
6]=Cyc_PP_text(((_tmp805=" catch ",_tag_dynforward(_tmp805,sizeof(char),
_get_zero_arr_size_char(_tmp805,8))))),((_tmp803[5]=Cyc_Absynpp_rb(),((_tmp803[4]=
Cyc_PP_line_doc(),((_tmp803[3]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp318)),((
_tmp803[2]=Cyc_PP_line_doc(),((_tmp803[1]=Cyc_Absynpp_lb(),((_tmp803[0]=Cyc_PP_text(((
_tmp804="try ",_tag_dynforward(_tmp804,sizeof(char),_get_zero_arr_size_char(
_tmp804,5))))),Cyc_PP_cat(_tag_dynforward(_tmp803,sizeof(struct Cyc_PP_Doc*),12))))))))))))))))))))))))));}
goto _LL25D;_LL280: if(_tmp2FB <= (void*)1)goto _LL282;if(*((int*)_tmp2FB)!= 15)goto
_LL282;_tmp31A=((struct Cyc_Absyn_Region_s_struct*)_tmp2FB)->f1;_tmp31B=((struct
Cyc_Absyn_Region_s_struct*)_tmp2FB)->f2;_tmp31C=((struct Cyc_Absyn_Region_s_struct*)
_tmp2FB)->f3;_tmp31D=((struct Cyc_Absyn_Region_s_struct*)_tmp2FB)->f4;_tmp31E=((
struct Cyc_Absyn_Region_s_struct*)_tmp2FB)->f5;_LL281:{struct Cyc_PP_Doc*_tmp816[3];
const char*_tmp815;const char*_tmp814;const char*_tmp813;const char*_tmp812;const
char*_tmp811;const char*_tmp810;struct Cyc_PP_Doc*_tmp80F[12];s=((_tmp80F[11]=Cyc_Absynpp_rb(),((
_tmp80F[10]=Cyc_PP_line_doc(),((_tmp80F[9]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp31E)),((_tmp80F[8]=Cyc_PP_line_doc(),((_tmp80F[7]=Cyc_Absynpp_lb(),((_tmp80F[
6]=_tmp31D != 0?(_tmp816[2]=Cyc_PP_text(((_tmp814=")",_tag_dynforward(_tmp814,
sizeof(char),_get_zero_arr_size_char(_tmp814,2))))),((_tmp816[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp31D),((_tmp816[0]=Cyc_PP_text(((_tmp815=" = open(",
_tag_dynforward(_tmp815,sizeof(char),_get_zero_arr_size_char(_tmp815,9))))),Cyc_PP_cat(
_tag_dynforward(_tmp816,sizeof(struct Cyc_PP_Doc*),3))))))): Cyc_PP_nil_doc(),((
_tmp80F[5]=Cyc_Absynpp_qvar2doc(_tmp31B->name),((_tmp80F[4]=Cyc_PP_text(((
_tmp813=">",_tag_dynforward(_tmp813,sizeof(char),_get_zero_arr_size_char(_tmp813,
2))))),((_tmp80F[3]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp31A)),((_tmp80F[2]=
Cyc_PP_text(((_tmp812="<",_tag_dynforward(_tmp812,sizeof(char),
_get_zero_arr_size_char(_tmp812,2))))),((_tmp80F[1]=_tmp31C?Cyc_PP_nil_doc(): Cyc_PP_text(((
_tmp811="[resetable]",_tag_dynforward(_tmp811,sizeof(char),
_get_zero_arr_size_char(_tmp811,12))))),((_tmp80F[0]=Cyc_PP_text(((_tmp810="region",
_tag_dynforward(_tmp810,sizeof(char),_get_zero_arr_size_char(_tmp810,7))))),Cyc_PP_cat(
_tag_dynforward(_tmp80F,sizeof(struct Cyc_PP_Doc*),12))))))))))))))))))))))))));}
goto _LL25D;_LL282: if(_tmp2FB <= (void*)1)goto _LL284;if(*((int*)_tmp2FB)!= 16)goto
_LL284;_tmp31F=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp2FB)->f1;_LL283:{
const char*_tmp81B;const char*_tmp81A;struct Cyc_PP_Doc*_tmp819[3];s=((_tmp819[2]=
Cyc_PP_text(((_tmp81B=");",_tag_dynforward(_tmp81B,sizeof(char),
_get_zero_arr_size_char(_tmp81B,3))))),((_tmp819[1]=Cyc_Absynpp_exp2doc(_tmp31F),((
_tmp819[0]=Cyc_PP_text(((_tmp81A="reset_region(",_tag_dynforward(_tmp81A,sizeof(
char),_get_zero_arr_size_char(_tmp81A,14))))),Cyc_PP_cat(_tag_dynforward(_tmp819,
sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL25D;_LL284: if(_tmp2FB <= (void*)1)
goto _LL25D;if(*((int*)_tmp2FB)!= 17)goto _LL25D;_tmp320=((struct Cyc_Absyn_Alias_s_struct*)
_tmp2FB)->f1;_tmp321=((struct Cyc_Absyn_Alias_s_struct*)_tmp2FB)->f2;_tmp322=((
struct Cyc_Absyn_Alias_s_struct*)_tmp2FB)->f3;_tmp323=((struct Cyc_Absyn_Alias_s_struct*)
_tmp2FB)->f4;_LL285:{const char*_tmp824;const char*_tmp823;const char*_tmp822;const
char*_tmp821;struct Cyc_PP_Doc*_tmp820[12];s=((_tmp820[11]=Cyc_Absynpp_rb(),((
_tmp820[10]=Cyc_PP_line_doc(),((_tmp820[9]=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(
_tmp323)),((_tmp820[8]=Cyc_PP_line_doc(),((_tmp820[7]=Cyc_Absynpp_lb(),((_tmp820[
6]=Cyc_Absynpp_qvar2doc(_tmp322->name),((_tmp820[5]=Cyc_PP_text(((_tmp824=">",
_tag_dynforward(_tmp824,sizeof(char),_get_zero_arr_size_char(_tmp824,2))))),((
_tmp820[4]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp321)),((_tmp820[3]=Cyc_PP_text(((
_tmp823="<",_tag_dynforward(_tmp823,sizeof(char),_get_zero_arr_size_char(_tmp823,
2))))),((_tmp820[2]=Cyc_PP_text(((_tmp822=") = ",_tag_dynforward(_tmp822,sizeof(
char),_get_zero_arr_size_char(_tmp822,5))))),((_tmp820[1]=Cyc_Absynpp_exp2doc(
_tmp320),((_tmp820[0]=Cyc_PP_text(((_tmp821="alias(",_tag_dynforward(_tmp821,
sizeof(char),_get_zero_arr_size_char(_tmp821,7))))),Cyc_PP_cat(_tag_dynforward(
_tmp820,sizeof(struct Cyc_PP_Doc*),12))))))))))))))))))))))))));}goto _LL25D;
_LL25D:;}return s;}struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p);
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){struct Cyc_PP_Doc*s;{
void*_tmp36E=(void*)p->r;void*_tmp36F;int _tmp370;char _tmp371;struct
_dynforward_ptr _tmp372;struct Cyc_Absyn_Vardecl*_tmp373;struct Cyc_Absyn_Pat*
_tmp374;struct Cyc_Absyn_Pat _tmp375;void*_tmp376;struct Cyc_Absyn_Vardecl*_tmp377;
struct Cyc_Absyn_Pat*_tmp378;struct Cyc_Absyn_Tvar*_tmp379;struct Cyc_Absyn_Vardecl*
_tmp37A;struct Cyc_List_List*_tmp37B;int _tmp37C;struct Cyc_Absyn_Pat*_tmp37D;
struct Cyc_Absyn_Vardecl*_tmp37E;struct Cyc_Absyn_Pat*_tmp37F;struct Cyc_Absyn_Pat
_tmp380;void*_tmp381;struct Cyc_Absyn_Vardecl*_tmp382;struct Cyc_Absyn_Pat*_tmp383;
struct _tuple0*_tmp384;struct _tuple0*_tmp385;struct Cyc_List_List*_tmp386;int
_tmp387;struct Cyc_Absyn_AggrInfo _tmp388;union Cyc_Absyn_AggrInfoU_union _tmp389;
struct Cyc_List_List*_tmp38A;struct Cyc_List_List*_tmp38B;int _tmp38C;struct Cyc_Absyn_Enumfield*
_tmp38D;struct Cyc_Absyn_Enumfield*_tmp38E;struct Cyc_Absyn_Tunionfield*_tmp38F;
struct Cyc_List_List*_tmp390;struct Cyc_Absyn_Tunionfield*_tmp391;struct Cyc_List_List*
_tmp392;int _tmp393;struct Cyc_Absyn_Exp*_tmp394;_LL28C: if((int)_tmp36E != 0)goto
_LL28E;_LL28D:{const char*_tmp825;s=Cyc_PP_text(((_tmp825="_",_tag_dynforward(
_tmp825,sizeof(char),_get_zero_arr_size_char(_tmp825,2)))));}goto _LL28B;_LL28E:
if((int)_tmp36E != 1)goto _LL290;_LL28F:{const char*_tmp826;s=Cyc_PP_text(((_tmp826="NULL",
_tag_dynforward(_tmp826,sizeof(char),_get_zero_arr_size_char(_tmp826,5)))));}
goto _LL28B;_LL290: if(_tmp36E <= (void*)2)goto _LL292;if(*((int*)_tmp36E)!= 7)goto
_LL292;_tmp36F=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp36E)->f1;_tmp370=((
struct Cyc_Absyn_Int_p_struct*)_tmp36E)->f2;_LL291: if(_tmp36F != (void*)((void*)1)){
const char*_tmp82A;void*_tmp829[1];struct Cyc_Int_pa_struct _tmp828;s=Cyc_PP_text((
struct _dynforward_ptr)((_tmp828.tag=1,((_tmp828.f1=(unsigned long)_tmp370,((
_tmp829[0]=& _tmp828,Cyc_aprintf(((_tmp82A="%d",_tag_dynforward(_tmp82A,sizeof(
char),_get_zero_arr_size_char(_tmp82A,3)))),_tag_dynforward(_tmp829,sizeof(void*),
1)))))))));}else{const char*_tmp82E;void*_tmp82D[1];struct Cyc_Int_pa_struct
_tmp82C;s=Cyc_PP_text((struct _dynforward_ptr)((_tmp82C.tag=1,((_tmp82C.f1=(
unsigned int)_tmp370,((_tmp82D[0]=& _tmp82C,Cyc_aprintf(((_tmp82E="%u",
_tag_dynforward(_tmp82E,sizeof(char),_get_zero_arr_size_char(_tmp82E,3)))),
_tag_dynforward(_tmp82D,sizeof(void*),1)))))))));}goto _LL28B;_LL292: if(_tmp36E <= (
void*)2)goto _LL294;if(*((int*)_tmp36E)!= 8)goto _LL294;_tmp371=((struct Cyc_Absyn_Char_p_struct*)
_tmp36E)->f1;_LL293:{const char*_tmp832;void*_tmp831[1];struct Cyc_String_pa_struct
_tmp830;s=Cyc_PP_text((struct _dynforward_ptr)((_tmp830.tag=0,((_tmp830.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_char_escape(_tmp371)),((
_tmp831[0]=& _tmp830,Cyc_aprintf(((_tmp832="'%s'",_tag_dynforward(_tmp832,sizeof(
char),_get_zero_arr_size_char(_tmp832,5)))),_tag_dynforward(_tmp831,sizeof(void*),
1)))))))));}goto _LL28B;_LL294: if(_tmp36E <= (void*)2)goto _LL296;if(*((int*)
_tmp36E)!= 9)goto _LL296;_tmp372=((struct Cyc_Absyn_Float_p_struct*)_tmp36E)->f1;
_LL295: s=Cyc_PP_text(_tmp372);goto _LL28B;_LL296: if(_tmp36E <= (void*)2)goto _LL298;
if(*((int*)_tmp36E)!= 0)goto _LL298;_tmp373=((struct Cyc_Absyn_Var_p_struct*)
_tmp36E)->f1;_tmp374=((struct Cyc_Absyn_Var_p_struct*)_tmp36E)->f2;_tmp375=*
_tmp374;_tmp376=(void*)_tmp375.r;if((int)_tmp376 != 0)goto _LL298;_LL297: s=Cyc_Absynpp_qvar2doc(
_tmp373->name);goto _LL28B;_LL298: if(_tmp36E <= (void*)2)goto _LL29A;if(*((int*)
_tmp36E)!= 0)goto _LL29A;_tmp377=((struct Cyc_Absyn_Var_p_struct*)_tmp36E)->f1;
_tmp378=((struct Cyc_Absyn_Var_p_struct*)_tmp36E)->f2;_LL299:{const char*_tmp835;
struct Cyc_PP_Doc*_tmp834[3];s=((_tmp834[2]=Cyc_Absynpp_pat2doc(_tmp378),((
_tmp834[1]=Cyc_PP_text(((_tmp835=" as ",_tag_dynforward(_tmp835,sizeof(char),
_get_zero_arr_size_char(_tmp835,5))))),((_tmp834[0]=Cyc_Absynpp_qvar2doc(_tmp377->name),
Cyc_PP_cat(_tag_dynforward(_tmp834,sizeof(struct Cyc_PP_Doc*),3))))))));}goto
_LL28B;_LL29A: if(_tmp36E <= (void*)2)goto _LL29C;if(*((int*)_tmp36E)!= 2)goto
_LL29C;_tmp379=((struct Cyc_Absyn_TagInt_p_struct*)_tmp36E)->f1;_tmp37A=((struct
Cyc_Absyn_TagInt_p_struct*)_tmp36E)->f2;_LL29B:{const char*_tmp83A;const char*
_tmp839;struct Cyc_PP_Doc*_tmp838[4];s=((_tmp838[3]=Cyc_PP_text(((_tmp83A=">",
_tag_dynforward(_tmp83A,sizeof(char),_get_zero_arr_size_char(_tmp83A,2))))),((
_tmp838[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp379)),((_tmp838[1]=Cyc_PP_text(((
_tmp839="<",_tag_dynforward(_tmp839,sizeof(char),_get_zero_arr_size_char(_tmp839,
2))))),((_tmp838[0]=Cyc_Absynpp_qvar2doc(_tmp37A->name),Cyc_PP_cat(
_tag_dynforward(_tmp838,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL28B;_LL29C:
if(_tmp36E <= (void*)2)goto _LL29E;if(*((int*)_tmp36E)!= 3)goto _LL29E;_tmp37B=((
struct Cyc_Absyn_Tuple_p_struct*)_tmp36E)->f1;_tmp37C=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp36E)->f2;_LL29D:{const char*_tmp843;const char*_tmp842;const char*_tmp841;const
char*_tmp840;struct Cyc_PP_Doc*_tmp83F[4];s=((_tmp83F[3]=_tmp37C?Cyc_PP_text(((
_tmp842=", ...)",_tag_dynforward(_tmp842,sizeof(char),_get_zero_arr_size_char(
_tmp842,7))))): Cyc_PP_text(((_tmp843=")",_tag_dynforward(_tmp843,sizeof(char),
_get_zero_arr_size_char(_tmp843,2))))),((_tmp83F[2]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dynforward_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp841=",",_tag_dynforward(_tmp841,
sizeof(char),_get_zero_arr_size_char(_tmp841,2)))),_tmp37B),((_tmp83F[1]=Cyc_PP_text(((
_tmp840="(",_tag_dynforward(_tmp840,sizeof(char),_get_zero_arr_size_char(_tmp840,
2))))),((_tmp83F[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dynforward(_tmp83F,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL28B;_LL29E: if(_tmp36E <= (void*)2)
goto _LL2A0;if(*((int*)_tmp36E)!= 4)goto _LL2A0;_tmp37D=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp36E)->f1;_LL29F:{const char*_tmp846;struct Cyc_PP_Doc*_tmp845[2];s=((_tmp845[1]=
Cyc_Absynpp_pat2doc(_tmp37D),((_tmp845[0]=Cyc_PP_text(((_tmp846="&",
_tag_dynforward(_tmp846,sizeof(char),_get_zero_arr_size_char(_tmp846,2))))),Cyc_PP_cat(
_tag_dynforward(_tmp845,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL28B;_LL2A0: if(
_tmp36E <= (void*)2)goto _LL2A2;if(*((int*)_tmp36E)!= 1)goto _LL2A2;_tmp37E=((
struct Cyc_Absyn_Reference_p_struct*)_tmp36E)->f1;_tmp37F=((struct Cyc_Absyn_Reference_p_struct*)
_tmp36E)->f2;_tmp380=*_tmp37F;_tmp381=(void*)_tmp380.r;if((int)_tmp381 != 0)goto
_LL2A2;_LL2A1:{const char*_tmp849;struct Cyc_PP_Doc*_tmp848[2];s=((_tmp848[1]=Cyc_Absynpp_qvar2doc(
_tmp37E->name),((_tmp848[0]=Cyc_PP_text(((_tmp849="*",_tag_dynforward(_tmp849,
sizeof(char),_get_zero_arr_size_char(_tmp849,2))))),Cyc_PP_cat(_tag_dynforward(
_tmp848,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL28B;_LL2A2: if(_tmp36E <= (void*)
2)goto _LL2A4;if(*((int*)_tmp36E)!= 1)goto _LL2A4;_tmp382=((struct Cyc_Absyn_Reference_p_struct*)
_tmp36E)->f1;_tmp383=((struct Cyc_Absyn_Reference_p_struct*)_tmp36E)->f2;_LL2A3:{
const char*_tmp84E;const char*_tmp84D;struct Cyc_PP_Doc*_tmp84C[4];s=((_tmp84C[3]=
Cyc_Absynpp_pat2doc(_tmp383),((_tmp84C[2]=Cyc_PP_text(((_tmp84E=" as ",
_tag_dynforward(_tmp84E,sizeof(char),_get_zero_arr_size_char(_tmp84E,5))))),((
_tmp84C[1]=Cyc_Absynpp_qvar2doc(_tmp382->name),((_tmp84C[0]=Cyc_PP_text(((
_tmp84D="*",_tag_dynforward(_tmp84D,sizeof(char),_get_zero_arr_size_char(_tmp84D,
2))))),Cyc_PP_cat(_tag_dynforward(_tmp84C,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL28B;_LL2A4: if(_tmp36E <= (void*)2)goto _LL2A6;if(*((int*)_tmp36E)!= 12)goto
_LL2A6;_tmp384=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp36E)->f1;_LL2A5: s=Cyc_Absynpp_qvar2doc(
_tmp384);goto _LL28B;_LL2A6: if(_tmp36E <= (void*)2)goto _LL2A8;if(*((int*)_tmp36E)
!= 13)goto _LL2A8;_tmp385=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp36E)->f1;
_tmp386=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp36E)->f2;_tmp387=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp36E)->f3;_LL2A7: {const char*_tmp850;const char*_tmp84F;struct _dynforward_ptr
term=_tmp387?(_tmp850=", ...)",_tag_dynforward(_tmp850,sizeof(char),
_get_zero_arr_size_char(_tmp850,7))):((_tmp84F=")",_tag_dynforward(_tmp84F,
sizeof(char),_get_zero_arr_size_char(_tmp84F,2))));{const char*_tmp855;const char*
_tmp854;struct Cyc_PP_Doc*_tmp853[2];s=((_tmp853[1]=Cyc_PP_group(((_tmp855="(",
_tag_dynforward(_tmp855,sizeof(char),_get_zero_arr_size_char(_tmp855,2)))),term,((
_tmp854=",",_tag_dynforward(_tmp854,sizeof(char),_get_zero_arr_size_char(_tmp854,
2)))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp386)),((_tmp853[0]=
Cyc_Absynpp_qvar2doc(_tmp385),Cyc_PP_cat(_tag_dynforward(_tmp853,sizeof(struct
Cyc_PP_Doc*),2))))));}goto _LL28B;}_LL2A8: if(_tmp36E <= (void*)2)goto _LL2AA;if(*((
int*)_tmp36E)!= 5)goto _LL2AA;_tmp388=((struct Cyc_Absyn_Aggr_p_struct*)_tmp36E)->f1;
_tmp389=_tmp388.aggr_info;_tmp38A=((struct Cyc_Absyn_Aggr_p_struct*)_tmp36E)->f2;
_tmp38B=((struct Cyc_Absyn_Aggr_p_struct*)_tmp36E)->f3;_tmp38C=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp36E)->f4;_LL2A9: {const char*_tmp857;const char*_tmp856;struct _dynforward_ptr
term=_tmp38C?(_tmp857=", ...}",_tag_dynforward(_tmp857,sizeof(char),
_get_zero_arr_size_char(_tmp857,7))):((_tmp856="}",_tag_dynforward(_tmp856,
sizeof(char),_get_zero_arr_size_char(_tmp856,2))));struct _tuple0*_tmp3B7;struct
_tuple3 _tmp3B6=Cyc_Absyn_aggr_kinded_name(_tmp389);_tmp3B7=_tmp3B6.f2;{const char*
_tmp862;const char*_tmp861;const char*_tmp860;const char*_tmp85F;const char*_tmp85E;
struct Cyc_PP_Doc*_tmp85D[4];s=((_tmp85D[3]=Cyc_PP_group(((_tmp862="",
_tag_dynforward(_tmp862,sizeof(char),_get_zero_arr_size_char(_tmp862,1)))),term,((
_tmp861=",",_tag_dynforward(_tmp861,sizeof(char),_get_zero_arr_size_char(_tmp861,
2)))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp38B)),((_tmp85D[2]=Cyc_PP_egroup(((
_tmp860="[",_tag_dynforward(_tmp860,sizeof(char),_get_zero_arr_size_char(_tmp860,
2)))),((_tmp85F="]",_tag_dynforward(_tmp85F,sizeof(char),_get_zero_arr_size_char(
_tmp85F,2)))),((_tmp85E=",",_tag_dynforward(_tmp85E,sizeof(char),
_get_zero_arr_size_char(_tmp85E,2)))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*
f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((
struct Cyc_List_List*(*)(struct _dynforward_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp38A))),((_tmp85D[1]=Cyc_Absynpp_lb(),((
_tmp85D[0]=Cyc_Absynpp_qvar2doc(_tmp3B7),Cyc_PP_cat(_tag_dynforward(_tmp85D,
sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL28B;}_LL2AA: if(_tmp36E <= (void*)2)
goto _LL2AC;if(*((int*)_tmp36E)!= 10)goto _LL2AC;_tmp38D=((struct Cyc_Absyn_Enum_p_struct*)
_tmp36E)->f2;_LL2AB: _tmp38E=_tmp38D;goto _LL2AD;_LL2AC: if(_tmp36E <= (void*)2)goto
_LL2AE;if(*((int*)_tmp36E)!= 11)goto _LL2AE;_tmp38E=((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp36E)->f2;_LL2AD: s=Cyc_Absynpp_qvar2doc(_tmp38E->name);goto _LL28B;_LL2AE: if(
_tmp36E <= (void*)2)goto _LL2B0;if(*((int*)_tmp36E)!= 6)goto _LL2B0;_tmp38F=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp36E)->f2;_tmp390=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp36E)->f3;if(_tmp390 != 0)goto _LL2B0;_LL2AF: s=Cyc_Absynpp_qvar2doc(_tmp38F->name);
goto _LL28B;_LL2B0: if(_tmp36E <= (void*)2)goto _LL2B2;if(*((int*)_tmp36E)!= 6)goto
_LL2B2;_tmp391=((struct Cyc_Absyn_Tunion_p_struct*)_tmp36E)->f2;_tmp392=((struct
Cyc_Absyn_Tunion_p_struct*)_tmp36E)->f3;_tmp393=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp36E)->f4;_LL2B1: {const char*_tmp864;const char*_tmp863;struct _dynforward_ptr
term=_tmp393?(_tmp864=", ...)",_tag_dynforward(_tmp864,sizeof(char),
_get_zero_arr_size_char(_tmp864,7))):((_tmp863=")",_tag_dynforward(_tmp863,
sizeof(char),_get_zero_arr_size_char(_tmp863,2))));{const char*_tmp869;const char*
_tmp868;struct Cyc_PP_Doc*_tmp867[2];s=((_tmp867[1]=Cyc_PP_egroup(((_tmp869="(",
_tag_dynforward(_tmp869,sizeof(char),_get_zero_arr_size_char(_tmp869,2)))),term,((
_tmp868=",",_tag_dynforward(_tmp868,sizeof(char),_get_zero_arr_size_char(_tmp868,
2)))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp392)),((_tmp867[0]=
Cyc_Absynpp_qvar2doc(_tmp391->name),Cyc_PP_cat(_tag_dynforward(_tmp867,sizeof(
struct Cyc_PP_Doc*),2))))));}goto _LL28B;}_LL2B2: if(_tmp36E <= (void*)2)goto _LL28B;
if(*((int*)_tmp36E)!= 14)goto _LL28B;_tmp394=((struct Cyc_Absyn_Exp_p_struct*)
_tmp36E)->f1;_LL2B3: s=Cyc_Absynpp_exp2doc(_tmp394);goto _LL28B;_LL28B:;}return s;}
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple8*dp);struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(
struct _tuple8*dp){const char*_tmp870;const char*_tmp86F;const char*_tmp86E;struct
Cyc_PP_Doc*_tmp86D[2];return(_tmp86D[1]=Cyc_Absynpp_pat2doc((*dp).f2),((_tmp86D[
0]=Cyc_PP_egroup(((_tmp870="",_tag_dynforward(_tmp870,sizeof(char),
_get_zero_arr_size_char(_tmp870,1)))),((_tmp86F="=",_tag_dynforward(_tmp86F,
sizeof(char),_get_zero_arr_size_char(_tmp86F,2)))),((_tmp86E="=",_tag_dynforward(
_tmp86E,sizeof(char),_get_zero_arr_size_char(_tmp86E,2)))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*
dp).f1)),Cyc_PP_cat(_tag_dynforward(_tmp86D,sizeof(struct Cyc_PP_Doc*),2)))));}
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c);
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
if(c->where_clause == 0  && (void*)(c->pattern)->r == (void*)((void*)0)){struct Cyc_PP_Doc*
_tmp875[2];const char*_tmp874;struct Cyc_PP_Doc*_tmp873[2];return(_tmp873[1]=Cyc_PP_nest(
2,((_tmp875[1]=Cyc_Absynpp_stmt2doc(c->body),((_tmp875[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dynforward(_tmp875,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp873[0]=Cyc_PP_text(((
_tmp874="default: ",_tag_dynforward(_tmp874,sizeof(char),_get_zero_arr_size_char(
_tmp874,10))))),Cyc_PP_cat(_tag_dynforward(_tmp873,sizeof(struct Cyc_PP_Doc*),2)))));}
else{if(c->where_clause == 0){struct Cyc_PP_Doc*_tmp87C[2];const char*_tmp87B;const
char*_tmp87A;struct Cyc_PP_Doc*_tmp879[4];return(_tmp879[3]=Cyc_PP_nest(2,((
_tmp87C[1]=Cyc_Absynpp_stmt2doc(c->body),((_tmp87C[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dynforward(_tmp87C,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp879[2]=Cyc_PP_text(((
_tmp87B=": ",_tag_dynforward(_tmp87B,sizeof(char),_get_zero_arr_size_char(
_tmp87B,3))))),((_tmp879[1]=Cyc_Absynpp_pat2doc(c->pattern),((_tmp879[0]=Cyc_PP_text(((
_tmp87A="case ",_tag_dynforward(_tmp87A,sizeof(char),_get_zero_arr_size_char(
_tmp87A,6))))),Cyc_PP_cat(_tag_dynforward(_tmp879,sizeof(struct Cyc_PP_Doc*),4)))))))));}
else{struct Cyc_PP_Doc*_tmp885[2];const char*_tmp884;const char*_tmp883;const char*
_tmp882;struct Cyc_PP_Doc*_tmp881[6];return(_tmp881[5]=Cyc_PP_nest(2,((_tmp885[1]=
Cyc_Absynpp_stmt2doc(c->body),((_tmp885[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dynforward(_tmp885,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp881[4]=Cyc_PP_text(((
_tmp884=": ",_tag_dynforward(_tmp884,sizeof(char),_get_zero_arr_size_char(
_tmp884,3))))),((_tmp881[3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)
_check_null(c->where_clause)),((_tmp881[2]=Cyc_PP_text(((_tmp883=" && ",
_tag_dynforward(_tmp883,sizeof(char),_get_zero_arr_size_char(_tmp883,5))))),((
_tmp881[1]=Cyc_Absynpp_pat2doc(c->pattern),((_tmp881[0]=Cyc_PP_text(((_tmp882="case ",
_tag_dynforward(_tmp882,sizeof(char),_get_zero_arr_size_char(_tmp882,6))))),Cyc_PP_cat(
_tag_dynforward(_tmp881,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}struct Cyc_PP_Doc*
Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs);struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(
struct Cyc_List_List*cs){const char*_tmp886;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Switch_clause*),struct _dynforward_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((_tmp886="",_tag_dynforward(
_tmp886,sizeof(char),_get_zero_arr_size_char(_tmp886,1)))),cs);}struct Cyc_PP_Doc*
Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f);struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(
struct Cyc_Absyn_Enumfield*f){if(f->tag == 0)return Cyc_Absynpp_qvar2doc(f->name);
else{const char*_tmp889;struct Cyc_PP_Doc*_tmp888[3];return(_tmp888[2]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp888[1]=Cyc_PP_text(((_tmp889=" = ",
_tag_dynforward(_tmp889,sizeof(char),_get_zero_arr_size_char(_tmp889,4))))),((
_tmp888[0]=Cyc_Absynpp_qvar2doc(f->name),Cyc_PP_cat(_tag_dynforward(_tmp888,
sizeof(struct Cyc_PP_Doc*),3)))))));}}struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(
struct Cyc_List_List*fs);struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*
fs){const char*_tmp88A;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct
Cyc_Absyn_Enumfield*),struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_enumfield2doc,((_tmp88A=",",_tag_dynforward(_tmp88A,sizeof(char),
_get_zero_arr_size_char(_tmp88A,2)))),fs);}static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(
struct Cyc_Absyn_Vardecl*v);static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*
v){return Cyc_Absynpp_qvar2doc(v->name);}static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(
struct Cyc_List_List*vds);static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*
vds){const char*_tmp88B;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(
struct Cyc_Absyn_Vardecl*),struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(
Cyc_Absynpp_id2doc,((_tmp88B=",",_tag_dynforward(_tmp88B,sizeof(char),
_get_zero_arr_size_char(_tmp88B,2)))),vds);}struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(
struct Cyc_Absyn_Vardecl*vd);struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*
vd){struct Cyc_Absyn_Vardecl _tmp3DB;void*_tmp3DC;struct _tuple0*_tmp3DD;struct Cyc_Absyn_Tqual
_tmp3DE;void*_tmp3DF;struct Cyc_Absyn_Exp*_tmp3E0;struct Cyc_List_List*_tmp3E1;
struct Cyc_Absyn_Vardecl*_tmp3DA=vd;_tmp3DB=*_tmp3DA;_tmp3DC=(void*)_tmp3DB.sc;
_tmp3DD=_tmp3DB.name;_tmp3DE=_tmp3DB.tq;_tmp3DF=(void*)_tmp3DB.type;_tmp3E0=
_tmp3DB.initializer;_tmp3E1=_tmp3DB.attributes;{struct Cyc_PP_Doc*s;struct Cyc_PP_Doc*
sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp3DD);struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(
_tmp3E1);struct Cyc_PP_Doc*beforenamedoc;{void*_tmp3E2=Cyc_Cyclone_c_compiler;
_LL2B5: if((int)_tmp3E2 != 0)goto _LL2B7;_LL2B6: beforenamedoc=attsdoc;goto _LL2B4;
_LL2B7: if((int)_tmp3E2 != 1)goto _LL2B4;_LL2B8:{void*_tmp3E3=Cyc_Tcutil_compress(
_tmp3DF);struct Cyc_Absyn_FnInfo _tmp3E4;struct Cyc_List_List*_tmp3E5;_LL2BA: if(
_tmp3E3 <= (void*)4)goto _LL2BC;if(*((int*)_tmp3E3)!= 8)goto _LL2BC;_tmp3E4=((
struct Cyc_Absyn_FnType_struct*)_tmp3E3)->f1;_tmp3E5=_tmp3E4.attributes;_LL2BB:
beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp3E5);goto _LL2B9;_LL2BC:;_LL2BD:
beforenamedoc=Cyc_PP_nil_doc();goto _LL2B9;_LL2B9:;}goto _LL2B4;_LL2B4:;}{struct
Cyc_PP_Doc*tmp_doc;{void*_tmp3E6=Cyc_Cyclone_c_compiler;_LL2BF: if((int)_tmp3E6 != 
0)goto _LL2C1;_LL2C0: tmp_doc=Cyc_PP_nil_doc();goto _LL2BE;_LL2C1: if((int)_tmp3E6 != 
1)goto _LL2BE;_LL2C2: tmp_doc=attsdoc;goto _LL2BE;_LL2BE:;}{const char*_tmp898;
struct Cyc_PP_Doc*_tmp897[2];const char*_tmp896;struct Cyc_Core_Opt*_tmp895;struct
Cyc_PP_Doc*_tmp894[2];struct Cyc_PP_Doc*_tmp893[5];s=((_tmp893[4]=Cyc_PP_text(((
_tmp898=";",_tag_dynforward(_tmp898,sizeof(char),_get_zero_arr_size_char(_tmp898,
2))))),((_tmp893[3]=_tmp3E0 == 0?Cyc_PP_nil_doc():((_tmp897[1]=Cyc_Absynpp_exp2doc((
struct Cyc_Absyn_Exp*)_tmp3E0),((_tmp897[0]=Cyc_PP_text(((_tmp896=" = ",
_tag_dynforward(_tmp896,sizeof(char),_get_zero_arr_size_char(_tmp896,4))))),Cyc_PP_cat(
_tag_dynforward(_tmp897,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp893[2]=Cyc_Absynpp_tqtd2doc(
_tmp3DE,_tmp3DF,((_tmp895=_cycalloc(sizeof(*_tmp895)),((_tmp895->v=((_tmp894[1]=
sn,((_tmp894[0]=beforenamedoc,Cyc_PP_cat(_tag_dynforward(_tmp894,sizeof(struct
Cyc_PP_Doc*),2)))))),_tmp895))))),((_tmp893[1]=Cyc_Absynpp_scope2doc(_tmp3DC),((
_tmp893[0]=tmp_doc,Cyc_PP_cat(_tag_dynforward(_tmp893,sizeof(struct Cyc_PP_Doc*),
5))))))))))));}return s;}}}struct _tuple11{struct Cyc_Position_Segment*f1;struct
_tuple0*f2;int f3;};struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple11*x);
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple11*x){struct _tuple0*_tmp3EE;
struct _tuple11 _tmp3ED=*x;_tmp3EE=_tmp3ED.f2;return Cyc_Absynpp_qvar2doc(_tmp3EE);}
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);struct Cyc_PP_Doc*
Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){struct Cyc_PP_Doc*s;{void*_tmp3EF=(
void*)d->r;struct Cyc_Absyn_Fndecl*_tmp3F0;struct Cyc_Absyn_Aggrdecl*_tmp3F1;
struct Cyc_Absyn_Vardecl*_tmp3F2;struct Cyc_Absyn_Tuniondecl*_tmp3F3;struct Cyc_Absyn_Tuniondecl
_tmp3F4;void*_tmp3F5;struct _tuple0*_tmp3F6;struct Cyc_List_List*_tmp3F7;struct Cyc_Core_Opt*
_tmp3F8;int _tmp3F9;int _tmp3FA;struct Cyc_Absyn_Pat*_tmp3FB;struct Cyc_Absyn_Exp*
_tmp3FC;struct Cyc_List_List*_tmp3FD;struct Cyc_Absyn_Enumdecl*_tmp3FE;struct Cyc_Absyn_Enumdecl
_tmp3FF;void*_tmp400;struct _tuple0*_tmp401;struct Cyc_Core_Opt*_tmp402;struct Cyc_Absyn_Typedefdecl*
_tmp403;struct _dynforward_ptr*_tmp404;struct Cyc_List_List*_tmp405;struct _tuple0*
_tmp406;struct Cyc_List_List*_tmp407;struct Cyc_List_List*_tmp408;struct Cyc_List_List*
_tmp409;struct Cyc_List_List*_tmp40A;_LL2C4: if(_tmp3EF <= (void*)2)goto _LL2DC;if(*((
int*)_tmp3EF)!= 1)goto _LL2C6;_tmp3F0=((struct Cyc_Absyn_Fn_d_struct*)_tmp3EF)->f1;
_LL2C5: {struct Cyc_Absyn_FnType_struct _tmp89E;struct Cyc_Absyn_FnInfo _tmp89D;
struct Cyc_Absyn_FnType_struct*_tmp89C;void*t=(void*)((_tmp89C=_cycalloc(sizeof(*
_tmp89C)),((_tmp89C[0]=((_tmp89E.tag=8,((_tmp89E.f1=((_tmp89D.tvars=_tmp3F0->tvs,((
_tmp89D.effect=_tmp3F0->effect,((_tmp89D.ret_typ=(void*)((void*)_tmp3F0->ret_type),((
_tmp89D.args=((struct Cyc_List_List*(*)(struct _tuple1*(*f)(struct _tuple6*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp3F0->args),((_tmp89D.c_varargs=
_tmp3F0->c_varargs,((_tmp89D.cyc_varargs=_tmp3F0->cyc_varargs,((_tmp89D.rgn_po=
_tmp3F0->rgn_po,((_tmp89D.attributes=0,_tmp89D)))))))))))))))),_tmp89E)))),
_tmp89C))));struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp3F0->attributes);
struct Cyc_PP_Doc*inlinedoc;if(_tmp3F0->is_inline){void*_tmp40B=Cyc_Cyclone_c_compiler;
_LL2E1: if((int)_tmp40B != 0)goto _LL2E3;_LL2E2:{const char*_tmp89F;inlinedoc=Cyc_PP_text(((
_tmp89F="inline ",_tag_dynforward(_tmp89F,sizeof(char),_get_zero_arr_size_char(
_tmp89F,8)))));}goto _LL2E0;_LL2E3: if((int)_tmp40B != 1)goto _LL2E0;_LL2E4:{const
char*_tmp8A0;inlinedoc=Cyc_PP_text(((_tmp8A0="__inline ",_tag_dynforward(_tmp8A0,
sizeof(char),_get_zero_arr_size_char(_tmp8A0,10)))));}goto _LL2E0;_LL2E0:;}else{
inlinedoc=Cyc_PP_nil_doc();}{struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc((
void*)_tmp3F0->sc);struct Cyc_PP_Doc*beforenamedoc;{void*_tmp40E=Cyc_Cyclone_c_compiler;
_LL2E6: if((int)_tmp40E != 0)goto _LL2E8;_LL2E7: beforenamedoc=attsdoc;goto _LL2E5;
_LL2E8: if((int)_tmp40E != 1)goto _LL2E5;_LL2E9: beforenamedoc=Cyc_Absynpp_callconv2doc(
_tmp3F0->attributes);goto _LL2E5;_LL2E5:;}{struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(
_tmp3F0->name);struct Cyc_PP_Doc*_tmp8A3[2];struct Cyc_Core_Opt*_tmp8A2;struct Cyc_PP_Doc*
tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,((_tmp8A2=_cycalloc(
sizeof(*_tmp8A2)),((_tmp8A2->v=((_tmp8A3[1]=namedoc,((_tmp8A3[0]=beforenamedoc,
Cyc_PP_cat(_tag_dynforward(_tmp8A3,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp8A2)))));
struct Cyc_PP_Doc*_tmp8A6[2];struct Cyc_PP_Doc*_tmp8A5[5];struct Cyc_PP_Doc*bodydoc=(
_tmp8A5[4]=Cyc_Absynpp_rb(),((_tmp8A5[3]=Cyc_PP_line_doc(),((_tmp8A5[2]=Cyc_PP_nest(
2,((_tmp8A6[1]=Cyc_Absynpp_stmt2doc(_tmp3F0->body),((_tmp8A6[0]=Cyc_PP_line_doc(),
Cyc_PP_cat(_tag_dynforward(_tmp8A6,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8A5[
1]=Cyc_Absynpp_lb(),((_tmp8A5[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dynforward(
_tmp8A5,sizeof(struct Cyc_PP_Doc*),5)))))))))));{struct Cyc_PP_Doc*_tmp8A7[4];s=((
_tmp8A7[3]=bodydoc,((_tmp8A7[2]=tqtddoc,((_tmp8A7[1]=scopedoc,((_tmp8A7[0]=
inlinedoc,Cyc_PP_cat(_tag_dynforward(_tmp8A7,sizeof(struct Cyc_PP_Doc*),4))))))))));}{
void*_tmp410=Cyc_Cyclone_c_compiler;_LL2EB: if((int)_tmp410 != 1)goto _LL2ED;_LL2EC:{
struct Cyc_PP_Doc*_tmp8A8[2];s=((_tmp8A8[1]=s,((_tmp8A8[0]=attsdoc,Cyc_PP_cat(
_tag_dynforward(_tmp8A8,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL2EA;_LL2ED:;
_LL2EE: goto _LL2EA;_LL2EA:;}goto _LL2C3;}}}_LL2C6: if(*((int*)_tmp3EF)!= 4)goto
_LL2C8;_tmp3F1=((struct Cyc_Absyn_Aggr_d_struct*)_tmp3EF)->f1;_LL2C7: if(_tmp3F1->impl
== 0){const char*_tmp8AB;struct Cyc_PP_Doc*_tmp8AA[5];s=((_tmp8AA[4]=Cyc_PP_text(((
_tmp8AB=";",_tag_dynforward(_tmp8AB,sizeof(char),_get_zero_arr_size_char(_tmp8AB,
2))))),((_tmp8AA[3]=Cyc_Absynpp_ktvars2doc(_tmp3F1->tvs),((_tmp8AA[2]=Cyc_Absynpp_qvar2bolddoc(
_tmp3F1->name),((_tmp8AA[1]=Cyc_Absynpp_aggr_kind2doc((void*)_tmp3F1->kind),((
_tmp8AA[0]=Cyc_Absynpp_scope2doc((void*)_tmp3F1->sc),Cyc_PP_cat(_tag_dynforward(
_tmp8AA,sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{const char*_tmp8B5;struct
Cyc_PP_Doc*_tmp8B4[2];struct Cyc_PP_Doc*_tmp8B3[2];const char*_tmp8B2;struct Cyc_PP_Doc*
_tmp8B1[12];s=((_tmp8B1[11]=Cyc_PP_text(((_tmp8B5=";",_tag_dynforward(_tmp8B5,
sizeof(char),_get_zero_arr_size_char(_tmp8B5,2))))),((_tmp8B1[10]=Cyc_Absynpp_atts2doc(
_tmp3F1->attributes),((_tmp8B1[9]=Cyc_Absynpp_rb(),((_tmp8B1[8]=Cyc_PP_line_doc(),((
_tmp8B1[7]=Cyc_PP_nest(2,((_tmp8B4[1]=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp3F1->impl))->fields),((_tmp8B4[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dynforward(_tmp8B4,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8B1[6]=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3F1->impl))->rgn_po == 0?Cyc_PP_nil_doc():((
_tmp8B3[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp3F1->impl))->rgn_po),((_tmp8B3[0]=Cyc_PP_text(((_tmp8B2=":",_tag_dynforward(
_tmp8B2,sizeof(char),_get_zero_arr_size_char(_tmp8B2,2))))),Cyc_PP_cat(
_tag_dynforward(_tmp8B3,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp8B1[5]=Cyc_Absynpp_ktvars2doc(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3F1->impl))->exist_vars),((_tmp8B1[
4]=Cyc_Absynpp_lb(),((_tmp8B1[3]=Cyc_PP_blank_doc(),((_tmp8B1[2]=Cyc_Absynpp_qvar2bolddoc(
_tmp3F1->name),((_tmp8B1[1]=Cyc_Absynpp_aggr_kind2doc((void*)_tmp3F1->kind),((
_tmp8B1[0]=Cyc_Absynpp_scope2doc((void*)_tmp3F1->sc),Cyc_PP_cat(_tag_dynforward(
_tmp8B1,sizeof(struct Cyc_PP_Doc*),12))))))))))))))))))))))))));}goto _LL2C3;
_LL2C8: if(*((int*)_tmp3EF)!= 0)goto _LL2CA;_tmp3F2=((struct Cyc_Absyn_Var_d_struct*)
_tmp3EF)->f1;_LL2C9: s=Cyc_Absynpp_vardecl2doc(_tmp3F2);goto _LL2C3;_LL2CA: if(*((
int*)_tmp3EF)!= 5)goto _LL2CC;_tmp3F3=((struct Cyc_Absyn_Tunion_d_struct*)_tmp3EF)->f1;
_tmp3F4=*_tmp3F3;_tmp3F5=(void*)_tmp3F4.sc;_tmp3F6=_tmp3F4.name;_tmp3F7=_tmp3F4.tvs;
_tmp3F8=_tmp3F4.fields;_tmp3F9=_tmp3F4.is_xtunion;_tmp3FA=_tmp3F4.is_flat;_LL2CB:
if(_tmp3F8 == 0){const char*_tmp8BE;const char*_tmp8BD;const char*_tmp8BC;const char*
_tmp8BB;struct Cyc_PP_Doc*_tmp8BA[6];s=((_tmp8BA[5]=Cyc_PP_text(((_tmp8BE=";",
_tag_dynforward(_tmp8BE,sizeof(char),_get_zero_arr_size_char(_tmp8BE,2))))),((
_tmp8BA[4]=Cyc_Absynpp_ktvars2doc(_tmp3F7),((_tmp8BA[3]=_tmp3F9?Cyc_Absynpp_qvar2bolddoc(
_tmp3F6): Cyc_Absynpp_typedef_name2bolddoc(_tmp3F6),((_tmp8BA[2]=_tmp3FA?Cyc_PP_text(((
_tmp8BD="__attribute__((flat)) ",_tag_dynforward(_tmp8BD,sizeof(char),
_get_zero_arr_size_char(_tmp8BD,23))))): Cyc_PP_blank_doc(),((_tmp8BA[1]=_tmp3F9?
Cyc_PP_text(((_tmp8BB="xtunion ",_tag_dynforward(_tmp8BB,sizeof(char),
_get_zero_arr_size_char(_tmp8BB,9))))): Cyc_PP_text(((_tmp8BC="tunion ",
_tag_dynforward(_tmp8BC,sizeof(char),_get_zero_arr_size_char(_tmp8BC,8))))),((
_tmp8BA[0]=Cyc_Absynpp_scope2doc(_tmp3F5),Cyc_PP_cat(_tag_dynforward(_tmp8BA,
sizeof(struct Cyc_PP_Doc*),6))))))))))))));}else{const char*_tmp8C9;struct Cyc_PP_Doc*
_tmp8C8[2];const char*_tmp8C7;const char*_tmp8C6;const char*_tmp8C5;struct Cyc_PP_Doc*
_tmp8C4[11];s=((_tmp8C4[10]=Cyc_PP_text(((_tmp8C9=";",_tag_dynforward(_tmp8C9,
sizeof(char),_get_zero_arr_size_char(_tmp8C9,2))))),((_tmp8C4[9]=Cyc_Absynpp_rb(),((
_tmp8C4[8]=Cyc_PP_line_doc(),((_tmp8C4[7]=Cyc_PP_nest(2,((_tmp8C8[1]=Cyc_Absynpp_tunionfields2doc((
struct Cyc_List_List*)_tmp3F8->v),((_tmp8C8[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dynforward(_tmp8C8,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8C4[6]=Cyc_Absynpp_lb(),((
_tmp8C4[5]=Cyc_PP_blank_doc(),((_tmp8C4[4]=_tmp3FA?Cyc_PP_text(((_tmp8C7="__attribute__((flat)) ",
_tag_dynforward(_tmp8C7,sizeof(char),_get_zero_arr_size_char(_tmp8C7,23))))): Cyc_PP_blank_doc(),((
_tmp8C4[3]=Cyc_Absynpp_ktvars2doc(_tmp3F7),((_tmp8C4[2]=_tmp3F9?Cyc_Absynpp_qvar2bolddoc(
_tmp3F6): Cyc_Absynpp_typedef_name2bolddoc(_tmp3F6),((_tmp8C4[1]=_tmp3F9?Cyc_PP_text(((
_tmp8C5="xtunion ",_tag_dynforward(_tmp8C5,sizeof(char),_get_zero_arr_size_char(
_tmp8C5,9))))): Cyc_PP_text(((_tmp8C6="tunion ",_tag_dynforward(_tmp8C6,sizeof(
char),_get_zero_arr_size_char(_tmp8C6,8))))),((_tmp8C4[0]=Cyc_Absynpp_scope2doc(
_tmp3F5),Cyc_PP_cat(_tag_dynforward(_tmp8C4,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL2C3;_LL2CC: if(*((int*)_tmp3EF)!= 2)goto _LL2CE;_tmp3FB=((struct Cyc_Absyn_Let_d_struct*)
_tmp3EF)->f1;_tmp3FC=((struct Cyc_Absyn_Let_d_struct*)_tmp3EF)->f3;_LL2CD:{const
char*_tmp8D0;const char*_tmp8CF;const char*_tmp8CE;struct Cyc_PP_Doc*_tmp8CD[5];s=((
_tmp8CD[4]=Cyc_PP_text(((_tmp8D0=";",_tag_dynforward(_tmp8D0,sizeof(char),
_get_zero_arr_size_char(_tmp8D0,2))))),((_tmp8CD[3]=Cyc_Absynpp_exp2doc(_tmp3FC),((
_tmp8CD[2]=Cyc_PP_text(((_tmp8CF=" = ",_tag_dynforward(_tmp8CF,sizeof(char),
_get_zero_arr_size_char(_tmp8CF,4))))),((_tmp8CD[1]=Cyc_Absynpp_pat2doc(_tmp3FB),((
_tmp8CD[0]=Cyc_PP_text(((_tmp8CE="let ",_tag_dynforward(_tmp8CE,sizeof(char),
_get_zero_arr_size_char(_tmp8CE,5))))),Cyc_PP_cat(_tag_dynforward(_tmp8CD,
sizeof(struct Cyc_PP_Doc*),5))))))))))));}goto _LL2C3;_LL2CE: if(*((int*)_tmp3EF)!= 
3)goto _LL2D0;_tmp3FD=((struct Cyc_Absyn_Letv_d_struct*)_tmp3EF)->f1;_LL2CF:{const
char*_tmp8D5;const char*_tmp8D4;struct Cyc_PP_Doc*_tmp8D3[3];s=((_tmp8D3[2]=Cyc_PP_text(((
_tmp8D5=";",_tag_dynforward(_tmp8D5,sizeof(char),_get_zero_arr_size_char(_tmp8D5,
2))))),((_tmp8D3[1]=Cyc_Absynpp_ids2doc(_tmp3FD),((_tmp8D3[0]=Cyc_PP_text(((
_tmp8D4="let ",_tag_dynforward(_tmp8D4,sizeof(char),_get_zero_arr_size_char(
_tmp8D4,5))))),Cyc_PP_cat(_tag_dynforward(_tmp8D3,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL2C3;_LL2D0: if(*((int*)_tmp3EF)!= 6)goto _LL2D2;_tmp3FE=((struct Cyc_Absyn_Enum_d_struct*)
_tmp3EF)->f1;_tmp3FF=*_tmp3FE;_tmp400=(void*)_tmp3FF.sc;_tmp401=_tmp3FF.name;
_tmp402=_tmp3FF.fields;_LL2D1: if(_tmp402 == 0){const char*_tmp8DA;const char*
_tmp8D9;struct Cyc_PP_Doc*_tmp8D8[4];s=((_tmp8D8[3]=Cyc_PP_text(((_tmp8DA=";",
_tag_dynforward(_tmp8DA,sizeof(char),_get_zero_arr_size_char(_tmp8DA,2))))),((
_tmp8D8[2]=Cyc_Absynpp_typedef_name2bolddoc(_tmp401),((_tmp8D8[1]=Cyc_PP_text(((
_tmp8D9="enum ",_tag_dynforward(_tmp8D9,sizeof(char),_get_zero_arr_size_char(
_tmp8D9,6))))),((_tmp8D8[0]=Cyc_Absynpp_scope2doc(_tmp400),Cyc_PP_cat(
_tag_dynforward(_tmp8D8,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{const char*
_tmp8E1;struct Cyc_PP_Doc*_tmp8E0[2];const char*_tmp8DF;struct Cyc_PP_Doc*_tmp8DE[9];
s=((_tmp8DE[8]=Cyc_PP_text(((_tmp8E1=";",_tag_dynforward(_tmp8E1,sizeof(char),
_get_zero_arr_size_char(_tmp8E1,2))))),((_tmp8DE[7]=Cyc_Absynpp_rb(),((_tmp8DE[6]=
Cyc_PP_line_doc(),((_tmp8DE[5]=Cyc_PP_nest(2,((_tmp8E0[1]=Cyc_Absynpp_enumfields2doc((
struct Cyc_List_List*)_tmp402->v),((_tmp8E0[0]=Cyc_PP_line_doc(),Cyc_PP_cat(
_tag_dynforward(_tmp8E0,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8DE[4]=Cyc_Absynpp_lb(),((
_tmp8DE[3]=Cyc_PP_blank_doc(),((_tmp8DE[2]=Cyc_Absynpp_qvar2bolddoc(_tmp401),((
_tmp8DE[1]=Cyc_PP_text(((_tmp8DF="enum ",_tag_dynforward(_tmp8DF,sizeof(char),
_get_zero_arr_size_char(_tmp8DF,6))))),((_tmp8DE[0]=Cyc_Absynpp_scope2doc(
_tmp400),Cyc_PP_cat(_tag_dynforward(_tmp8DE,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
goto _LL2C3;_LL2D2: if(*((int*)_tmp3EF)!= 7)goto _LL2D4;_tmp403=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp3EF)->f1;_LL2D3: {void*t;if(_tmp403->defn != 0)t=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp403->defn))->v;else{t=Cyc_Absyn_new_evar(_tmp403->kind,0);}{
const char*_tmp8EB;struct Cyc_Core_Opt*_tmp8EA;struct Cyc_PP_Doc*_tmp8E9[2];const
char*_tmp8E8;struct Cyc_PP_Doc*_tmp8E7[4];s=((_tmp8E7[3]=Cyc_PP_text(((_tmp8EB=";",
_tag_dynforward(_tmp8EB,sizeof(char),_get_zero_arr_size_char(_tmp8EB,2))))),((
_tmp8E7[2]=Cyc_Absynpp_atts2doc(_tmp403->atts),((_tmp8E7[1]=Cyc_Absynpp_tqtd2doc(
_tmp403->tq,t,((_tmp8EA=_cycalloc(sizeof(*_tmp8EA)),((_tmp8EA->v=((_tmp8E9[1]=
Cyc_Absynpp_tvars2doc(_tmp403->tvs),((_tmp8E9[0]=Cyc_Absynpp_typedef_name2bolddoc(
_tmp403->name),Cyc_PP_cat(_tag_dynforward(_tmp8E9,sizeof(struct Cyc_PP_Doc*),2)))))),
_tmp8EA))))),((_tmp8E7[0]=Cyc_PP_text(((_tmp8E8="typedef ",_tag_dynforward(
_tmp8E8,sizeof(char),_get_zero_arr_size_char(_tmp8E8,9))))),Cyc_PP_cat(
_tag_dynforward(_tmp8E7,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LL2C3;}
_LL2D4: if(*((int*)_tmp3EF)!= 8)goto _LL2D6;_tmp404=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp3EF)->f1;_tmp405=((struct Cyc_Absyn_Namespace_d_struct*)_tmp3EF)->f2;_LL2D5:
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp404);{const
char*_tmp8F0;const char*_tmp8EF;struct Cyc_PP_Doc*_tmp8EE[8];s=((_tmp8EE[7]=Cyc_Absynpp_rb(),((
_tmp8EE[6]=Cyc_PP_line_doc(),((_tmp8EE[5]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmp8F0="",_tag_dynforward(_tmp8F0,sizeof(char),
_get_zero_arr_size_char(_tmp8F0,1)))),_tmp405),((_tmp8EE[4]=Cyc_PP_line_doc(),((
_tmp8EE[3]=Cyc_Absynpp_lb(),((_tmp8EE[2]=Cyc_PP_blank_doc(),((_tmp8EE[1]=Cyc_PP_textptr(
_tmp404),((_tmp8EE[0]=Cyc_PP_text(((_tmp8EF="namespace ",_tag_dynforward(_tmp8EF,
sizeof(char),_get_zero_arr_size_char(_tmp8EF,11))))),Cyc_PP_cat(_tag_dynforward(
_tmp8EE,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}if(Cyc_Absynpp_use_curr_namespace)
Cyc_Absynpp_curr_namespace_drop();goto _LL2C3;_LL2D6: if(*((int*)_tmp3EF)!= 9)goto
_LL2D8;_tmp406=((struct Cyc_Absyn_Using_d_struct*)_tmp3EF)->f1;_tmp407=((struct
Cyc_Absyn_Using_d_struct*)_tmp3EF)->f2;_LL2D7: if(Cyc_Absynpp_print_using_stmts){
const char*_tmp8F5;const char*_tmp8F4;struct Cyc_PP_Doc*_tmp8F3[8];s=((_tmp8F3[7]=
Cyc_Absynpp_rb(),((_tmp8F3[6]=Cyc_PP_line_doc(),((_tmp8F3[5]=((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dynforward_ptr sep,struct Cyc_List_List*
l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmp8F5="",_tag_dynforward(_tmp8F5,
sizeof(char),_get_zero_arr_size_char(_tmp8F5,1)))),_tmp407),((_tmp8F3[4]=Cyc_PP_line_doc(),((
_tmp8F3[3]=Cyc_Absynpp_lb(),((_tmp8F3[2]=Cyc_PP_blank_doc(),((_tmp8F3[1]=Cyc_Absynpp_qvar2doc(
_tmp406),((_tmp8F3[0]=Cyc_PP_text(((_tmp8F4="using ",_tag_dynforward(_tmp8F4,
sizeof(char),_get_zero_arr_size_char(_tmp8F4,7))))),Cyc_PP_cat(_tag_dynforward(
_tmp8F3,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{const char*_tmp900;
const char*_tmp8FF;const char*_tmp8FE;const char*_tmp8FD;const char*_tmp8FC;struct
Cyc_PP_Doc*_tmp8FB[11];s=((_tmp8FB[10]=Cyc_PP_text(((_tmp900=" */",
_tag_dynforward(_tmp900,sizeof(char),_get_zero_arr_size_char(_tmp900,4))))),((
_tmp8FB[9]=Cyc_Absynpp_rb(),((_tmp8FB[8]=Cyc_PP_text(((_tmp8FF="/* ",
_tag_dynforward(_tmp8FF,sizeof(char),_get_zero_arr_size_char(_tmp8FF,4))))),((
_tmp8FB[7]=Cyc_PP_line_doc(),((_tmp8FB[6]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmp8FE="",_tag_dynforward(_tmp8FE,sizeof(char),
_get_zero_arr_size_char(_tmp8FE,1)))),_tmp407),((_tmp8FB[5]=Cyc_PP_line_doc(),((
_tmp8FB[4]=Cyc_PP_text(((_tmp8FD=" */",_tag_dynforward(_tmp8FD,sizeof(char),
_get_zero_arr_size_char(_tmp8FD,4))))),((_tmp8FB[3]=Cyc_Absynpp_lb(),((_tmp8FB[2]=
Cyc_PP_blank_doc(),((_tmp8FB[1]=Cyc_Absynpp_qvar2doc(_tmp406),((_tmp8FB[0]=Cyc_PP_text(((
_tmp8FC="/* using ",_tag_dynforward(_tmp8FC,sizeof(char),_get_zero_arr_size_char(
_tmp8FC,10))))),Cyc_PP_cat(_tag_dynforward(_tmp8FB,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
goto _LL2C3;_LL2D8: if(*((int*)_tmp3EF)!= 10)goto _LL2DA;_tmp408=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp3EF)->f1;_LL2D9: if(Cyc_Absynpp_print_externC_stmts){const char*_tmp905;const
char*_tmp904;struct Cyc_PP_Doc*_tmp903[6];s=((_tmp903[5]=Cyc_Absynpp_rb(),((
_tmp903[4]=Cyc_PP_line_doc(),((_tmp903[3]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmp905="",_tag_dynforward(_tmp905,sizeof(char),
_get_zero_arr_size_char(_tmp905,1)))),_tmp408),((_tmp903[2]=Cyc_PP_line_doc(),((
_tmp903[1]=Cyc_Absynpp_lb(),((_tmp903[0]=Cyc_PP_text(((_tmp904="extern \"C\" ",
_tag_dynforward(_tmp904,sizeof(char),_get_zero_arr_size_char(_tmp904,12))))),Cyc_PP_cat(
_tag_dynforward(_tmp903,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}else{const
char*_tmp910;const char*_tmp90F;const char*_tmp90E;const char*_tmp90D;const char*
_tmp90C;struct Cyc_PP_Doc*_tmp90B[9];s=((_tmp90B[8]=Cyc_PP_text(((_tmp910=" */",
_tag_dynforward(_tmp910,sizeof(char),_get_zero_arr_size_char(_tmp910,4))))),((
_tmp90B[7]=Cyc_Absynpp_rb(),((_tmp90B[6]=Cyc_PP_text(((_tmp90F="/* ",
_tag_dynforward(_tmp90F,sizeof(char),_get_zero_arr_size_char(_tmp90F,4))))),((
_tmp90B[5]=Cyc_PP_line_doc(),((_tmp90B[4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*
pp)(struct Cyc_Absyn_Decl*),struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(
Cyc_Absynpp_decl2doc,((_tmp90E="",_tag_dynforward(_tmp90E,sizeof(char),
_get_zero_arr_size_char(_tmp90E,1)))),_tmp408),((_tmp90B[3]=Cyc_PP_line_doc(),((
_tmp90B[2]=Cyc_PP_text(((_tmp90D=" */",_tag_dynforward(_tmp90D,sizeof(char),
_get_zero_arr_size_char(_tmp90D,4))))),((_tmp90B[1]=Cyc_Absynpp_lb(),((_tmp90B[0]=
Cyc_PP_text(((_tmp90C="/* extern \"C\" ",_tag_dynforward(_tmp90C,sizeof(char),
_get_zero_arr_size_char(_tmp90C,15))))),Cyc_PP_cat(_tag_dynforward(_tmp90B,
sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto _LL2C3;_LL2DA: if(*((int*)
_tmp3EF)!= 11)goto _LL2DC;_tmp409=((struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp3EF)->f1;_tmp40A=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp3EF)->f2;
_LL2DB: if(Cyc_Absynpp_print_externC_stmts){struct Cyc_PP_Doc*exs_doc;if(_tmp40A != 
0){const char*_tmp915;const char*_tmp914;struct Cyc_PP_Doc*_tmp913[7];exs_doc=((
_tmp913[6]=Cyc_Absynpp_rb(),((_tmp913[5]=Cyc_PP_line_doc(),((_tmp913[4]=((struct
Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple11*),struct _dynforward_ptr sep,
struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,((_tmp915=",",
_tag_dynforward(_tmp915,sizeof(char),_get_zero_arr_size_char(_tmp915,2)))),
_tmp40A),((_tmp913[3]=Cyc_PP_line_doc(),((_tmp913[2]=Cyc_Absynpp_lb(),((_tmp913[
1]=Cyc_PP_text(((_tmp914=" export ",_tag_dynforward(_tmp914,sizeof(char),
_get_zero_arr_size_char(_tmp914,9))))),((_tmp913[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(
_tag_dynforward(_tmp913,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{
exs_doc=Cyc_Absynpp_rb();}{const char*_tmp91A;const char*_tmp919;struct Cyc_PP_Doc*
_tmp918[6];s=((_tmp918[5]=exs_doc,((_tmp918[4]=Cyc_PP_line_doc(),((_tmp918[3]=((
struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct
_dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmp91A="",_tag_dynforward(_tmp91A,sizeof(char),_get_zero_arr_size_char(_tmp91A,
1)))),_tmp409),((_tmp918[2]=Cyc_PP_line_doc(),((_tmp918[1]=Cyc_Absynpp_lb(),((
_tmp918[0]=Cyc_PP_text(((_tmp919="extern \"C include\" ",_tag_dynforward(_tmp919,
sizeof(char),_get_zero_arr_size_char(_tmp919,20))))),Cyc_PP_cat(_tag_dynforward(
_tmp918,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}}else{const char*_tmp925;const
char*_tmp924;const char*_tmp923;const char*_tmp922;const char*_tmp921;struct Cyc_PP_Doc*
_tmp920[9];s=((_tmp920[8]=Cyc_PP_text(((_tmp925=" */",_tag_dynforward(_tmp925,
sizeof(char),_get_zero_arr_size_char(_tmp925,4))))),((_tmp920[7]=Cyc_Absynpp_rb(),((
_tmp920[6]=Cyc_PP_text(((_tmp924="/* ",_tag_dynforward(_tmp924,sizeof(char),
_get_zero_arr_size_char(_tmp924,4))))),((_tmp920[5]=Cyc_PP_line_doc(),((_tmp920[
4]=((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct
_dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((
_tmp923="",_tag_dynforward(_tmp923,sizeof(char),_get_zero_arr_size_char(_tmp923,
1)))),_tmp409),((_tmp920[3]=Cyc_PP_line_doc(),((_tmp920[2]=Cyc_PP_text(((_tmp922=" */",
_tag_dynforward(_tmp922,sizeof(char),_get_zero_arr_size_char(_tmp922,4))))),((
_tmp920[1]=Cyc_Absynpp_lb(),((_tmp920[0]=Cyc_PP_text(((_tmp921="/* extern \"C include\" ",
_tag_dynforward(_tmp921,sizeof(char),_get_zero_arr_size_char(_tmp921,23))))),Cyc_PP_cat(
_tag_dynforward(_tmp920,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}goto
_LL2C3;_LL2DC: if((int)_tmp3EF != 0)goto _LL2DE;_LL2DD:{const char*_tmp928;struct Cyc_PP_Doc*
_tmp927[2];s=((_tmp927[1]=Cyc_Absynpp_lb(),((_tmp927[0]=Cyc_PP_text(((_tmp928="__cyclone_port_on__;",
_tag_dynforward(_tmp928,sizeof(char),_get_zero_arr_size_char(_tmp928,21))))),Cyc_PP_cat(
_tag_dynforward(_tmp927,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL2C3;_LL2DE: if((
int)_tmp3EF != 1)goto _LL2C3;_LL2DF:{const char*_tmp92B;struct Cyc_PP_Doc*_tmp92A[2];
s=((_tmp92A[1]=Cyc_Absynpp_lb(),((_tmp92A[0]=Cyc_PP_text(((_tmp92B="__cyclone_port_off__;",
_tag_dynforward(_tmp92B,sizeof(char),_get_zero_arr_size_char(_tmp92B,22))))),Cyc_PP_cat(
_tag_dynforward(_tmp92A,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL2C3;_LL2C3:;}
return s;}int Cyc_Absynpp_print_scopes=1;struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(
void*sc);struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(void*sc){if(!Cyc_Absynpp_print_scopes)
return Cyc_PP_nil_doc();{void*_tmp463=sc;_LL2F0: if((int)_tmp463 != 0)goto _LL2F2;
_LL2F1: {const char*_tmp92C;return Cyc_PP_text(((_tmp92C="static ",_tag_dynforward(
_tmp92C,sizeof(char),_get_zero_arr_size_char(_tmp92C,8)))));}_LL2F2: if((int)
_tmp463 != 2)goto _LL2F4;_LL2F3: return Cyc_PP_nil_doc();_LL2F4: if((int)_tmp463 != 3)
goto _LL2F6;_LL2F5: {const char*_tmp92D;return Cyc_PP_text(((_tmp92D="extern ",
_tag_dynforward(_tmp92D,sizeof(char),_get_zero_arr_size_char(_tmp92D,8)))));}
_LL2F6: if((int)_tmp463 != 4)goto _LL2F8;_LL2F7: {const char*_tmp92E;return Cyc_PP_text(((
_tmp92E="extern \"C\" ",_tag_dynforward(_tmp92E,sizeof(char),
_get_zero_arr_size_char(_tmp92E,12)))));}_LL2F8: if((int)_tmp463 != 1)goto _LL2FA;
_LL2F9: {const char*_tmp92F;return Cyc_PP_text(((_tmp92F="abstract ",
_tag_dynforward(_tmp92F,sizeof(char),_get_zero_arr_size_char(_tmp92F,10)))));}
_LL2FA: if((int)_tmp463 != 5)goto _LL2EF;_LL2FB: {const char*_tmp930;return Cyc_PP_text(((
_tmp930="register ",_tag_dynforward(_tmp930,sizeof(char),_get_zero_arr_size_char(
_tmp930,10)))));}_LL2EF:;}}int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t);int
Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){void*_tmp469=t;struct Cyc_Absyn_Tvar*
_tmp46A;struct Cyc_List_List*_tmp46B;_LL2FD: if(_tmp469 <= (void*)4)goto _LL301;if(*((
int*)_tmp469)!= 1)goto _LL2FF;_tmp46A=((struct Cyc_Absyn_VarType_struct*)_tmp469)->f1;
_LL2FE: return Cyc_Tcutil_is_temp_tvar(_tmp46A);_LL2FF: if(*((int*)_tmp469)!= 21)
goto _LL301;_tmp46B=((struct Cyc_Absyn_JoinEff_struct*)_tmp469)->f1;_LL300: return
Cyc_List_exists(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp46B);_LL301:;_LL302:
return 0;_LL2FC:;}int Cyc_Absynpp_is_anon_aggrtype(void*t);int Cyc_Absynpp_is_anon_aggrtype(
void*t){void*_tmp46C=t;void**_tmp46D;void*_tmp46E;_LL304: if(_tmp46C <= (void*)4)
goto _LL30A;if(*((int*)_tmp46C)!= 11)goto _LL306;_LL305: return 1;_LL306: if(*((int*)
_tmp46C)!= 13)goto _LL308;_LL307: return 1;_LL308: if(*((int*)_tmp46C)!= 17)goto
_LL30A;_tmp46D=((struct Cyc_Absyn_TypedefType_struct*)_tmp46C)->f4;if(_tmp46D == 0)
goto _LL30A;_tmp46E=*_tmp46D;_LL309: return Cyc_Absynpp_is_anon_aggrtype(_tmp46E);
_LL30A:;_LL30B: return 0;_LL303:;}static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(
struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms);static struct Cyc_List_List*
Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*
tms){if(tms != 0  && tms->tl != 0){struct _tuple5 _tmp931;struct _tuple5 _tmp470=(
_tmp931.f1=(void*)tms->hd,((_tmp931.f2=(void*)((struct Cyc_List_List*)_check_null(
tms->tl))->hd,_tmp931)));void*_tmp471;void*_tmp472;_LL30D: _tmp471=_tmp470.f1;if(*((
int*)_tmp471)!= 2)goto _LL30F;_tmp472=_tmp470.f2;if(*((int*)_tmp472)!= 3)goto
_LL30F;_LL30E: {struct Cyc_List_List*_tmp934;struct Cyc_List_List*_tmp933;return(
_tmp933=_region_malloc(r,sizeof(*_tmp933)),((_tmp933->hd=(void*)((void*)tms->hd),((
_tmp933->tl=((_tmp934=_region_malloc(r,sizeof(*_tmp934)),((_tmp934->hd=(void*)((
void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd),((_tmp934->tl=Cyc_Absynpp_bubble_attributes(
r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmp934)))))),_tmp933)))));}
_LL30F:;_LL310: {struct Cyc_List_List*_tmp935;return(_tmp935=_region_malloc(r,
sizeof(*_tmp935)),((_tmp935->hd=(void*)atts,((_tmp935->tl=tms,_tmp935)))));}
_LL30C:;}else{struct Cyc_List_List*_tmp936;return(_tmp936=_region_malloc(r,
sizeof(*_tmp936)),((_tmp936->hd=(void*)atts,((_tmp936->tl=tms,_tmp936)))));}}
struct _tuple7 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,
void*t);struct _tuple7 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual
tq,void*t){void*_tmp477=t;struct Cyc_Absyn_ArrayInfo _tmp478;void*_tmp479;struct
Cyc_Absyn_Tqual _tmp47A;struct Cyc_Absyn_Exp*_tmp47B;struct Cyc_Absyn_Conref*
_tmp47C;struct Cyc_Position_Segment*_tmp47D;struct Cyc_Absyn_PtrInfo _tmp47E;void*
_tmp47F;struct Cyc_Absyn_Tqual _tmp480;struct Cyc_Absyn_PtrAtts _tmp481;struct Cyc_Absyn_FnInfo
_tmp482;struct Cyc_List_List*_tmp483;struct Cyc_Core_Opt*_tmp484;void*_tmp485;
struct Cyc_List_List*_tmp486;int _tmp487;struct Cyc_Absyn_VarargInfo*_tmp488;struct
Cyc_List_List*_tmp489;struct Cyc_List_List*_tmp48A;struct Cyc_Core_Opt*_tmp48B;
struct Cyc_Core_Opt*_tmp48C;int _tmp48D;struct _tuple0*_tmp48E;struct Cyc_List_List*
_tmp48F;void**_tmp490;_LL312: if(_tmp477 <= (void*)4)goto _LL31C;if(*((int*)_tmp477)
!= 7)goto _LL314;_tmp478=((struct Cyc_Absyn_ArrayType_struct*)_tmp477)->f1;_tmp479=(
void*)_tmp478.elt_type;_tmp47A=_tmp478.tq;_tmp47B=_tmp478.num_elts;_tmp47C=
_tmp478.zero_term;_tmp47D=_tmp478.zt_loc;_LL313: {struct Cyc_Absyn_Tqual _tmp492;
void*_tmp493;struct Cyc_List_List*_tmp494;struct _tuple7 _tmp491=Cyc_Absynpp_to_tms(
r,_tmp47A,_tmp479);_tmp492=_tmp491.f1;_tmp493=_tmp491.f2;_tmp494=_tmp491.f3;{
void*tm;if(_tmp47B == 0){struct Cyc_Absyn_Carray_mod_struct _tmp939;struct Cyc_Absyn_Carray_mod_struct*
_tmp938;tm=(void*)((_tmp938=_region_malloc(r,sizeof(*_tmp938)),((_tmp938[0]=((
_tmp939.tag=0,((_tmp939.f1=_tmp47C,((_tmp939.f2=_tmp47D,_tmp939)))))),_tmp938))));}
else{struct Cyc_Absyn_ConstArray_mod_struct _tmp93C;struct Cyc_Absyn_ConstArray_mod_struct*
_tmp93B;tm=(void*)((_tmp93B=_region_malloc(r,sizeof(*_tmp93B)),((_tmp93B[0]=((
_tmp93C.tag=1,((_tmp93C.f1=(struct Cyc_Absyn_Exp*)_tmp47B,((_tmp93C.f2=_tmp47C,((
_tmp93C.f3=_tmp47D,_tmp93C)))))))),_tmp93B))));}{struct Cyc_List_List*_tmp93F;
struct _tuple7 _tmp93E;return(_tmp93E.f1=_tmp492,((_tmp93E.f2=_tmp493,((_tmp93E.f3=((
_tmp93F=_region_malloc(r,sizeof(*_tmp93F)),((_tmp93F->hd=(void*)tm,((_tmp93F->tl=
_tmp494,_tmp93F)))))),_tmp93E)))));}}}_LL314: if(*((int*)_tmp477)!= 4)goto _LL316;
_tmp47E=((struct Cyc_Absyn_PointerType_struct*)_tmp477)->f1;_tmp47F=(void*)
_tmp47E.elt_typ;_tmp480=_tmp47E.elt_tq;_tmp481=_tmp47E.ptr_atts;_LL315: {struct
Cyc_Absyn_Tqual _tmp49C;void*_tmp49D;struct Cyc_List_List*_tmp49E;struct _tuple7
_tmp49B=Cyc_Absynpp_to_tms(r,_tmp480,_tmp47F);_tmp49C=_tmp49B.f1;_tmp49D=_tmp49B.f2;
_tmp49E=_tmp49B.f3;{struct Cyc_Absyn_Pointer_mod_struct*_tmp945;struct Cyc_Absyn_Pointer_mod_struct
_tmp944;struct Cyc_List_List*_tmp943;_tmp49E=((_tmp943=_region_malloc(r,sizeof(*
_tmp943)),((_tmp943->hd=(void*)((void*)((_tmp945=_region_malloc(r,sizeof(*
_tmp945)),((_tmp945[0]=((_tmp944.tag=2,((_tmp944.f1=_tmp481,((_tmp944.f2=tq,
_tmp944)))))),_tmp945))))),((_tmp943->tl=_tmp49E,_tmp943))))));}{struct _tuple7
_tmp946;return(_tmp946.f1=_tmp49C,((_tmp946.f2=_tmp49D,((_tmp946.f3=_tmp49E,
_tmp946)))));}}_LL316: if(*((int*)_tmp477)!= 8)goto _LL318;_tmp482=((struct Cyc_Absyn_FnType_struct*)
_tmp477)->f1;_tmp483=_tmp482.tvars;_tmp484=_tmp482.effect;_tmp485=(void*)_tmp482.ret_typ;
_tmp486=_tmp482.args;_tmp487=_tmp482.c_varargs;_tmp488=_tmp482.cyc_varargs;
_tmp489=_tmp482.rgn_po;_tmp48A=_tmp482.attributes;_LL317: if(!Cyc_Absynpp_print_all_tvars){
if(_tmp484 == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp484->v)){
_tmp484=0;_tmp483=0;}}else{if(Cyc_Absynpp_rewrite_temp_tvars)((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,
_tmp483);}{struct Cyc_Absyn_Tqual _tmp4A4;void*_tmp4A5;struct Cyc_List_List*_tmp4A6;
struct _tuple7 _tmp4A3=Cyc_Absynpp_to_tms(r,Cyc_Absyn_empty_tqual(0),_tmp485);
_tmp4A4=_tmp4A3.f1;_tmp4A5=_tmp4A3.f2;_tmp4A6=_tmp4A3.f3;{struct Cyc_List_List*
tms=_tmp4A6;{void*_tmp4A7=Cyc_Cyclone_c_compiler;_LL31F: if((int)_tmp4A7 != 0)goto
_LL321;_LL320: if(_tmp48A != 0){struct Cyc_Absyn_Attributes_mod_struct _tmp949;
struct Cyc_Absyn_Attributes_mod_struct*_tmp948;tms=Cyc_Absynpp_bubble_attributes(
r,(void*)((_tmp948=_region_malloc(r,sizeof(*_tmp948)),((_tmp948[0]=((_tmp949.tag=
5,((_tmp949.f1=0,((_tmp949.f2=_tmp48A,_tmp949)))))),_tmp948)))),tms);}{struct Cyc_Absyn_Function_mod_struct*
_tmp958;struct Cyc_Absyn_WithTypes_struct*_tmp957;struct Cyc_Absyn_WithTypes_struct
_tmp956;struct Cyc_Absyn_Function_mod_struct _tmp955;struct Cyc_List_List*_tmp954;
tms=((_tmp954=_region_malloc(r,sizeof(*_tmp954)),((_tmp954->hd=(void*)((void*)((
_tmp958=_region_malloc(r,sizeof(*_tmp958)),((_tmp958[0]=((_tmp955.tag=3,((
_tmp955.f1=(void*)((void*)((_tmp957=_region_malloc(r,sizeof(*_tmp957)),((_tmp957[
0]=((_tmp956.tag=1,((_tmp956.f1=_tmp486,((_tmp956.f2=_tmp487,((_tmp956.f3=
_tmp488,((_tmp956.f4=_tmp484,((_tmp956.f5=_tmp489,_tmp956)))))))))))),_tmp957))))),
_tmp955)))),_tmp958))))),((_tmp954->tl=tms,_tmp954))))));}goto _LL31E;_LL321: if((
int)_tmp4A7 != 1)goto _LL31E;_LL322:{struct Cyc_Absyn_Function_mod_struct*_tmp967;
struct Cyc_Absyn_WithTypes_struct*_tmp966;struct Cyc_Absyn_WithTypes_struct _tmp965;
struct Cyc_Absyn_Function_mod_struct _tmp964;struct Cyc_List_List*_tmp963;tms=((
_tmp963=_region_malloc(r,sizeof(*_tmp963)),((_tmp963->hd=(void*)((void*)((
_tmp967=_region_malloc(r,sizeof(*_tmp967)),((_tmp967[0]=((_tmp964.tag=3,((
_tmp964.f1=(void*)((void*)((_tmp966=_region_malloc(r,sizeof(*_tmp966)),((_tmp966[
0]=((_tmp965.tag=1,((_tmp965.f1=_tmp486,((_tmp965.f2=_tmp487,((_tmp965.f3=
_tmp488,((_tmp965.f4=_tmp484,((_tmp965.f5=_tmp489,_tmp965)))))))))))),_tmp966))))),
_tmp964)))),_tmp967))))),((_tmp963->tl=tms,_tmp963))))));}for(0;_tmp48A != 0;
_tmp48A=_tmp48A->tl){void*_tmp4B4=(void*)_tmp48A->hd;_LL324: if((int)_tmp4B4 != 0)
goto _LL326;_LL325: goto _LL327;_LL326: if((int)_tmp4B4 != 1)goto _LL328;_LL327: goto
_LL329;_LL328: if((int)_tmp4B4 != 2)goto _LL32A;_LL329:{struct Cyc_Absyn_Attributes_mod_struct*
_tmp971;struct Cyc_List_List*_tmp970;struct Cyc_Absyn_Attributes_mod_struct _tmp96F;
struct Cyc_List_List*_tmp96E;tms=((_tmp96E=_region_malloc(r,sizeof(*_tmp96E)),((
_tmp96E->hd=(void*)((void*)((_tmp971=_region_malloc(r,sizeof(*_tmp971)),((
_tmp971[0]=((_tmp96F.tag=5,((_tmp96F.f1=0,((_tmp96F.f2=((_tmp970=_cycalloc(
sizeof(*_tmp970)),((_tmp970->hd=(void*)((void*)_tmp48A->hd),((_tmp970->tl=0,
_tmp970)))))),_tmp96F)))))),_tmp971))))),((_tmp96E->tl=tms,_tmp96E))))));}goto
AfterAtts;_LL32A:;_LL32B: goto _LL323;_LL323:;}goto _LL31E;_LL31E:;}AfterAtts: if(
_tmp483 != 0){struct Cyc_Absyn_TypeParams_mod_struct*_tmp977;struct Cyc_Absyn_TypeParams_mod_struct
_tmp976;struct Cyc_List_List*_tmp975;tms=((_tmp975=_region_malloc(r,sizeof(*
_tmp975)),((_tmp975->hd=(void*)((void*)((_tmp977=_region_malloc(r,sizeof(*
_tmp977)),((_tmp977[0]=((_tmp976.tag=4,((_tmp976.f1=_tmp483,((_tmp976.f2=0,((
_tmp976.f3=1,_tmp976)))))))),_tmp977))))),((_tmp975->tl=tms,_tmp975))))));}{
struct _tuple7 _tmp978;return(_tmp978.f1=_tmp4A4,((_tmp978.f2=_tmp4A5,((_tmp978.f3=
tms,_tmp978)))));}}}_LL318: if(*((int*)_tmp477)!= 0)goto _LL31A;_tmp48B=((struct
Cyc_Absyn_Evar_struct*)_tmp477)->f1;_tmp48C=((struct Cyc_Absyn_Evar_struct*)
_tmp477)->f2;_tmp48D=((struct Cyc_Absyn_Evar_struct*)_tmp477)->f3;_LL319: if(
_tmp48C == 0){struct _tuple7 _tmp979;return(_tmp979.f1=tq,((_tmp979.f2=t,((_tmp979.f3=
0,_tmp979)))));}else{return Cyc_Absynpp_to_tms(r,tq,(void*)_tmp48C->v);}_LL31A:
if(*((int*)_tmp477)!= 17)goto _LL31C;_tmp48E=((struct Cyc_Absyn_TypedefType_struct*)
_tmp477)->f1;_tmp48F=((struct Cyc_Absyn_TypedefType_struct*)_tmp477)->f2;_tmp490=((
struct Cyc_Absyn_TypedefType_struct*)_tmp477)->f4;_LL31B: if((_tmp490 == 0  || !Cyc_Absynpp_expand_typedefs)
 || Cyc_Absynpp_is_anon_aggrtype(*_tmp490)){struct _tuple7 _tmp97A;return(_tmp97A.f1=
tq,((_tmp97A.f2=t,((_tmp97A.f3=0,_tmp97A)))));}else{return Cyc_Absynpp_to_tms(r,
tq,*_tmp490);}_LL31C:;_LL31D: {struct _tuple7 _tmp97B;return(_tmp97B.f1=tq,((
_tmp97B.f2=t,((_tmp97B.f3=0,_tmp97B)))));}_LL311:;}static int Cyc_Absynpp_is_char_ptr(
void*t);static int Cyc_Absynpp_is_char_ptr(void*t){void*_tmp4C0=t;struct Cyc_Core_Opt*
_tmp4C1;struct Cyc_Core_Opt _tmp4C2;void*_tmp4C3;struct Cyc_Absyn_PtrInfo _tmp4C4;
void*_tmp4C5;_LL32D: if(_tmp4C0 <= (void*)4)goto _LL331;if(*((int*)_tmp4C0)!= 0)
goto _LL32F;_tmp4C1=((struct Cyc_Absyn_Evar_struct*)_tmp4C0)->f2;if(_tmp4C1 == 0)
goto _LL32F;_tmp4C2=*_tmp4C1;_tmp4C3=(void*)_tmp4C2.v;_LL32E: return Cyc_Absynpp_is_char_ptr(
_tmp4C3);_LL32F: if(*((int*)_tmp4C0)!= 4)goto _LL331;_tmp4C4=((struct Cyc_Absyn_PointerType_struct*)
_tmp4C0)->f1;_tmp4C5=(void*)_tmp4C4.elt_typ;_LL330: L: {void*_tmp4C6=_tmp4C5;
struct Cyc_Core_Opt*_tmp4C7;struct Cyc_Core_Opt _tmp4C8;void*_tmp4C9;void**_tmp4CA;
void*_tmp4CB;void*_tmp4CC;_LL334: if(_tmp4C6 <= (void*)4)goto _LL33A;if(*((int*)
_tmp4C6)!= 0)goto _LL336;_tmp4C7=((struct Cyc_Absyn_Evar_struct*)_tmp4C6)->f2;if(
_tmp4C7 == 0)goto _LL336;_tmp4C8=*_tmp4C7;_tmp4C9=(void*)_tmp4C8.v;_LL335: _tmp4C5=
_tmp4C9;goto L;_LL336: if(*((int*)_tmp4C6)!= 17)goto _LL338;_tmp4CA=((struct Cyc_Absyn_TypedefType_struct*)
_tmp4C6)->f4;if(_tmp4CA == 0)goto _LL338;_tmp4CB=*_tmp4CA;_LL337: _tmp4C5=_tmp4CB;
goto L;_LL338: if(*((int*)_tmp4C6)!= 5)goto _LL33A;_tmp4CC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4C6)->f2;if((int)_tmp4CC != 0)goto _LL33A;_LL339: return 1;_LL33A:;_LL33B: return
0;_LL333:;}_LL331:;_LL332: return 0;_LL32C:;}struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt);struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(
struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){struct _RegionHandle
_tmp4CD=_new_region("temp");struct _RegionHandle*temp=& _tmp4CD;_push_region(temp);{
struct Cyc_Absyn_Tqual _tmp4CF;void*_tmp4D0;struct Cyc_List_List*_tmp4D1;struct
_tuple7 _tmp4CE=Cyc_Absynpp_to_tms(temp,tq,typ);_tmp4CF=_tmp4CE.f1;_tmp4D0=
_tmp4CE.f2;_tmp4D1=_tmp4CE.f3;_tmp4D1=Cyc_List_imp_rev(_tmp4D1);if(_tmp4D1 == 0
 && dopt == 0){struct Cyc_PP_Doc*_tmp97C[2];struct Cyc_PP_Doc*_tmp4D3=(_tmp97C[1]=
Cyc_Absynpp_ntyp2doc(_tmp4D0),((_tmp97C[0]=Cyc_Absynpp_tqual2doc(_tmp4CF),Cyc_PP_cat(
_tag_dynforward(_tmp97C,sizeof(struct Cyc_PP_Doc*),2)))));_npop_handler(0);return
_tmp4D3;}else{const char*_tmp97F;struct Cyc_PP_Doc*_tmp97E[4];struct Cyc_PP_Doc*
_tmp4D6=(_tmp97E[3]=Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?
Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp4D1),((_tmp97E[2]=Cyc_PP_text(((
_tmp97F=" ",_tag_dynforward(_tmp97F,sizeof(char),_get_zero_arr_size_char(_tmp97F,
2))))),((_tmp97E[1]=Cyc_Absynpp_ntyp2doc(_tmp4D0),((_tmp97E[0]=Cyc_Absynpp_tqual2doc(
_tmp4CF),Cyc_PP_cat(_tag_dynforward(_tmp97E,sizeof(struct Cyc_PP_Doc*),4)))))))));
_npop_handler(0);return _tmp4D6;}};_pop_region(temp);}struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(
struct Cyc_Absyn_Aggrfield*f);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct
Cyc_Absyn_Aggrfield*f){void*_tmp4D7=Cyc_Cyclone_c_compiler;_LL33D: if((int)
_tmp4D7 != 0)goto _LL33F;_LL33E: if(f->width != 0){const char*_tmp986;const char*
_tmp985;struct Cyc_Core_Opt*_tmp984;struct Cyc_PP_Doc*_tmp983[5];return(_tmp983[4]=
Cyc_PP_text(((_tmp986=";",_tag_dynforward(_tmp986,sizeof(char),
_get_zero_arr_size_char(_tmp986,2))))),((_tmp983[3]=Cyc_Absynpp_atts2doc(f->attributes),((
_tmp983[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((
_tmp983[1]=Cyc_PP_text(((_tmp985=":",_tag_dynforward(_tmp985,sizeof(char),
_get_zero_arr_size_char(_tmp985,2))))),((_tmp983[0]=Cyc_Absynpp_tqtd2doc(f->tq,(
void*)f->type,((_tmp984=_cycalloc(sizeof(*_tmp984)),((_tmp984->v=Cyc_PP_textptr(
f->name),_tmp984))))),Cyc_PP_cat(_tag_dynforward(_tmp983,sizeof(struct Cyc_PP_Doc*),
5)))))))))));}else{const char*_tmp98B;struct Cyc_Core_Opt*_tmp98A;struct Cyc_PP_Doc*
_tmp989[3];return(_tmp989[2]=Cyc_PP_text(((_tmp98B=";",_tag_dynforward(_tmp98B,
sizeof(char),_get_zero_arr_size_char(_tmp98B,2))))),((_tmp989[1]=Cyc_Absynpp_atts2doc(
f->attributes),((_tmp989[0]=Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,((_tmp98A=
_cycalloc(sizeof(*_tmp98A)),((_tmp98A->v=Cyc_PP_textptr(f->name),_tmp98A))))),
Cyc_PP_cat(_tag_dynforward(_tmp989,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL33F:
if((int)_tmp4D7 != 1)goto _LL33C;_LL340: if(f->width != 0){const char*_tmp992;const
char*_tmp991;struct Cyc_Core_Opt*_tmp990;struct Cyc_PP_Doc*_tmp98F[5];return(
_tmp98F[4]=Cyc_PP_text(((_tmp992=";",_tag_dynforward(_tmp992,sizeof(char),
_get_zero_arr_size_char(_tmp992,2))))),((_tmp98F[3]=Cyc_Absynpp_exp2doc((struct
Cyc_Absyn_Exp*)_check_null(f->width)),((_tmp98F[2]=Cyc_PP_text(((_tmp991=":",
_tag_dynforward(_tmp991,sizeof(char),_get_zero_arr_size_char(_tmp991,2))))),((
_tmp98F[1]=Cyc_Absynpp_tqtd2doc(f->tq,(void*)f->type,((_tmp990=_cycalloc(sizeof(*
_tmp990)),((_tmp990->v=Cyc_PP_textptr(f->name),_tmp990))))),((_tmp98F[0]=Cyc_Absynpp_atts2doc(
f->attributes),Cyc_PP_cat(_tag_dynforward(_tmp98F,sizeof(struct Cyc_PP_Doc*),5)))))))))));}
else{const char*_tmp997;struct Cyc_Core_Opt*_tmp996;struct Cyc_PP_Doc*_tmp995[3];
return(_tmp995[2]=Cyc_PP_text(((_tmp997=";",_tag_dynforward(_tmp997,sizeof(char),
_get_zero_arr_size_char(_tmp997,2))))),((_tmp995[1]=Cyc_Absynpp_tqtd2doc(f->tq,(
void*)f->type,((_tmp996=_cycalloc(sizeof(*_tmp996)),((_tmp996->v=Cyc_PP_textptr(
f->name),_tmp996))))),((_tmp995[0]=Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(
_tag_dynforward(_tmp995,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL33C:;}struct Cyc_PP_Doc*
Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields);struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(
struct Cyc_List_List*fields){const char*_tmp998;return((struct Cyc_PP_Doc*(*)(
struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dynforward_ptr sep,
struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((_tmp998="",
_tag_dynforward(_tmp998,sizeof(char),_get_zero_arr_size_char(_tmp998,1)))),
fields);}struct Cyc_PP_Doc*Cyc_Absynpp_tunionfield2doc(struct Cyc_Absyn_Tunionfield*
f);struct Cyc_PP_Doc*Cyc_Absynpp_tunionfield2doc(struct Cyc_Absyn_Tunionfield*f){
struct Cyc_PP_Doc*_tmp999[3];return(_tmp999[2]=f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(
f->typs),((_tmp999[1]=Cyc_Absynpp_typedef_name2doc(f->name),((_tmp999[0]=Cyc_Absynpp_scope2doc((
void*)f->sc),Cyc_PP_cat(_tag_dynforward(_tmp999,sizeof(struct Cyc_PP_Doc*),3)))))));}
struct Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(struct Cyc_List_List*fields);struct
Cyc_PP_Doc*Cyc_Absynpp_tunionfields2doc(struct Cyc_List_List*fields){const char*
_tmp99A;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Tunionfield*),
struct _dynforward_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_tunionfield2doc,((
_tmp99A=",",_tag_dynforward(_tmp99A,sizeof(char),_get_zero_arr_size_char(_tmp99A,
2)))),fields);}void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*
f);void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)
tdl->hd),72,f);{const char*_tmp99D;void*_tmp99C;(_tmp99C=0,Cyc_fprintf(f,((
_tmp99D="\n",_tag_dynforward(_tmp99D,sizeof(char),_get_zero_arr_size_char(
_tmp99D,2)))),_tag_dynforward(_tmp99C,sizeof(void*),0)));}}}struct
_dynforward_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);struct
_dynforward_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){const char*
_tmp99E;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmp99E="",_tag_dynforward(
_tmp99E,sizeof(char),_get_zero_arr_size_char(_tmp99E,1)))),((struct Cyc_List_List*(*)(
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Absynpp_decl2doc,tdl)),72);}struct _dynforward_ptr Cyc_Absynpp_exp2string(
struct Cyc_Absyn_Exp*e);struct _dynforward_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*
e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}struct _dynforward_ptr
Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s);struct _dynforward_ptr Cyc_Absynpp_stmt2string(
struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s),72);}
struct _dynforward_ptr Cyc_Absynpp_typ2string(void*t);struct _dynforward_ptr Cyc_Absynpp_typ2string(
void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}struct
_dynforward_ptr Cyc_Absynpp_typ2cstring(void*t);struct _dynforward_ptr Cyc_Absynpp_typ2cstring(
void*t){int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;
Cyc_Absynpp_qvar_to_Cids=1;Cyc_Absynpp_add_cyc_prefix=0;{struct _dynforward_ptr s=
Cyc_Absynpp_typ2string(t);Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;Cyc_Absynpp_add_cyc_prefix=
old_add_cyc_prefix;return s;}}struct _dynforward_ptr Cyc_Absynpp_prim2string(void*p);
struct _dynforward_ptr Cyc_Absynpp_prim2string(void*p){return Cyc_PP_string_of_doc(
Cyc_Absynpp_prim2doc(p),72);}struct _dynforward_ptr Cyc_Absynpp_pat2string(struct
Cyc_Absyn_Pat*p);struct _dynforward_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*
p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}struct _dynforward_ptr
Cyc_Absynpp_scope2string(void*sc);struct _dynforward_ptr Cyc_Absynpp_scope2string(
void*sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
