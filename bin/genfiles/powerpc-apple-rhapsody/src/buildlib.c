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
16];extern char Cyc_Core_Free_Region[16];struct _dynforward_ptr Cstring_to_string(
char*);struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stdout;extern struct Cyc___cycFILE*
Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;
struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;};
struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_fclose(struct Cyc___cycFILE*);int Cyc_fflush(struct
Cyc___cycFILE*);struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);int Cyc_fprintf(
struct Cyc___cycFILE*,struct _dynforward_ptr,struct _dynforward_ptr);int Cyc_fputc(
int,struct Cyc___cycFILE*);int Cyc_fputs(const char*,struct Cyc___cycFILE*);struct
Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;
unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
int remove(const char*);int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dynforward_ptr,
struct _dynforward_ptr);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _dynforward_ptr f1;};extern char
Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{char*tag;struct
_dynforward_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);
void*refill_state;struct _dynforward_ptr lex_buffer;int lex_buffer_len;int
lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;
int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(
struct _dynforward_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dynforward_ptr lex_base;struct _dynforward_ptr lex_backtrk;struct
_dynforward_ptr lex_default;struct _dynforward_ptr lex_trans;struct _dynforward_ptr
lex_check;};struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
struct _dynforward_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);char Cyc_Lexing_lexeme_char(
struct Cyc_Lexing_lexbuf*,int);int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct _dynforward_ptr);
extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
8];int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_empty(int(*
cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);struct
Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*s,void*elt);int Cyc_Set_cardinality(
struct Cyc_Set_Set*s);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char
Cyc_Set_Absent[11];struct Cyc_Iter_Iter Cyc_Set_make_iter(struct _RegionHandle*rgn,
struct Cyc_Set_Set*s);unsigned long Cyc_strlen(struct _dynforward_ptr s);int Cyc_strptrcmp(
struct _dynforward_ptr*s1,struct _dynforward_ptr*s2);struct _dynforward_ptr Cyc_strconcat(
struct _dynforward_ptr,struct _dynforward_ptr);struct _dynforward_ptr Cyc_strdup(
struct _dynforward_ptr src);struct _dynforward_ptr Cyc_substring(struct
_dynforward_ptr,int ofs,unsigned long n);struct Cyc_Hashtable_Table;struct Cyc_Hashtable_Table*
Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(
struct Cyc_Hashtable_Table*t,void*key,void*val);void*Cyc_Hashtable_lookup(struct
Cyc_Hashtable_Table*t,void*key);int Cyc_Hashtable_hash_stringptr(struct
_dynforward_ptr*p);struct _dynforward_ptr Cyc_Filename_concat(struct
_dynforward_ptr,struct _dynforward_ptr);struct _dynforward_ptr Cyc_Filename_chop_extension(
struct _dynforward_ptr);struct _dynforward_ptr Cyc_Filename_dirname(struct
_dynforward_ptr);struct _dynforward_ptr Cyc_Filename_basename(struct
_dynforward_ptr);struct Cyc_Lineno_Pos{struct _dynforward_ptr logical_file;struct
_dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[9];void Cyc_Position_reset_position(
struct _dynforward_ptr);struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _dynforward_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];extern char Cyc_Arg_Bad[
8];struct Cyc_Arg_Bad_struct{char*tag;struct _dynforward_ptr f1;};extern char Cyc_Arg_Error[
10];struct Cyc_Arg_Unit_spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_struct{
int tag;void(*f1)(struct _dynforward_ptr);};struct Cyc_Arg_FlagString_spec_struct{
int tag;void(*f1)(struct _dynforward_ptr,struct _dynforward_ptr);};struct Cyc_Arg_Set_spec_struct{
int tag;int*f1;};struct Cyc_Arg_Clear_spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_struct{
int tag;void(*f1)(struct _dynforward_ptr);};struct Cyc_Arg_Int_spec_struct{int tag;
void(*f1)(int);};struct Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct
_dynforward_ptr);};void Cyc_Arg_usage(struct Cyc_List_List*,struct _dynforward_ptr);
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _dynforward_ptr),
struct _dynforward_ptr errmsg,struct _dynforward_ptr args);struct Cyc_Buffer_t;struct
Cyc_Buffer_t*Cyc_Buffer_create(unsigned int n);struct _dynforward_ptr Cyc_Buffer_contents(
struct Cyc_Buffer_t*);void Cyc_Buffer_add_char(struct Cyc_Buffer_t*,char);void Cyc_Buffer_add_string(
struct Cyc_Buffer_t*,struct _dynforward_ptr);struct Cyc_Absyn_Loc_n_struct{int tag;}
;struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct
Loc_n;struct Cyc_Absyn_Rel_n_struct Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};
struct _tuple0{union Cyc_Absyn_Nmspace_union f1;struct _dynforward_ptr*f2;};struct
Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int
q_restrict;int real_const;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Eq_constr_struct{
int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{int tag;struct Cyc_Absyn_Conref*
f1;};struct Cyc_Absyn_No_constr_struct{int tag;};union Cyc_Absyn_Constraint_union{
struct Cyc_Absyn_Eq_constr_struct Eq_constr;struct Cyc_Absyn_Forward_constr_struct
Forward_constr;struct Cyc_Absyn_No_constr_struct No_constr;};struct Cyc_Absyn_Conref{
union Cyc_Absyn_Constraint_union v;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;}
;struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;void*f2;};struct Cyc_Absyn_Tvar{struct
_dynforward_ptr*name;int identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AbsUpper_b_struct{int tag;void*f1;};
struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct _tuple3{void*f1;
struct _tuple0*f2;};struct _tuple3 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU_union);
void Cyc_Absyn_print_decls(struct Cyc_List_List*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*
f);struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);struct Cyc_Declaration_spec;
struct Cyc_Declarator;struct Cyc_Abstractdeclarator;struct _tuple4{void*f1;int f2;};
struct Cyc_Int_tok_struct{int tag;struct _tuple4 f1;};struct Cyc_Char_tok_struct{int
tag;char f1;};struct Cyc_String_tok_struct{int tag;struct _dynforward_ptr f1;};struct
Cyc_Stringopt_tok_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_QualId_tok_struct{
int tag;struct _tuple0*f1;};struct _tuple5{struct Cyc_Position_Segment*f1;struct Cyc_Absyn_Conref*
f2;struct Cyc_Absyn_Conref*f3;};struct Cyc_YY1_struct{int tag;struct _tuple5*f1;};
struct Cyc_YY2_struct{int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_YY3_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_YY4_struct{int tag;struct Cyc_Absyn_Exp*f1;
};struct Cyc_YY5_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY6_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY7_struct{int tag;void*f1;};struct Cyc_YY8_struct{
int tag;struct Cyc_Core_Opt*f1;};struct Cyc_YY9_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_YY10_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY11_struct{
int tag;struct Cyc_Absyn_Pat*f1;};struct _tuple6{struct Cyc_List_List*f1;int f2;};
struct Cyc_YY12_struct{int tag;struct _tuple6*f1;};struct Cyc_YY13_struct{int tag;
struct Cyc_List_List*f1;};struct _tuple7{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};struct Cyc_YY14_struct{int tag;struct _tuple7*f1;};struct Cyc_YY15_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY16_struct{int tag;struct _tuple6*f1;};
struct Cyc_YY17_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_YY18_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY19_struct{int tag;struct Cyc_Declaration_spec*
f1;};struct _tuple8{struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_YY20_struct{
int tag;struct _tuple8*f1;};struct Cyc_YY21_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY22_struct{int tag;void*f1;};struct Cyc_YY23_struct{int tag;void*f1;};
struct Cyc_YY24_struct{int tag;void*f1;};struct Cyc_YY25_struct{int tag;struct Cyc_Absyn_Tqual
f1;};struct Cyc_YY26_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY27_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY28_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_YY29_struct{int tag;struct Cyc_Declarator*f1;};struct Cyc_YY30_struct{
int tag;struct Cyc_Abstractdeclarator*f1;};struct Cyc_YY31_struct{int tag;int f1;};
struct Cyc_YY32_struct{int tag;void*f1;};struct Cyc_YY33_struct{int tag;struct Cyc_Absyn_Tunionfield*
f1;};struct Cyc_YY34_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct
Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_YY35_struct{
int tag;struct _tuple9*f1;};struct Cyc_YY36_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY37_struct{int tag;struct _tuple1*f1;};struct Cyc_YY38_struct{int tag;
struct Cyc_List_List*f1;};struct _tuple10{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*
f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct Cyc_YY39_struct{int tag;
struct _tuple10*f1;};struct Cyc_YY40_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_YY41_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY42_struct{int tag;
void*f1;};struct Cyc_YY43_struct{int tag;void*f1;};struct Cyc_YY44_struct{int tag;
void*f1;};struct Cyc_YY45_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY46_struct{
int tag;void*f1;};struct Cyc_YY47_struct{int tag;struct Cyc_Absyn_Enumfield*f1;};
struct Cyc_YY48_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY49_struct{int
tag;struct Cyc_Core_Opt*f1;};struct Cyc_YY50_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY51_struct{int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_YY52_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YYINITIALSVAL_struct{int tag;};union Cyc_YYSTYPE_union{
struct Cyc_Int_tok_struct Int_tok;struct Cyc_Char_tok_struct Char_tok;struct Cyc_String_tok_struct
String_tok;struct Cyc_Stringopt_tok_struct Stringopt_tok;struct Cyc_QualId_tok_struct
QualId_tok;struct Cyc_YY1_struct YY1;struct Cyc_YY2_struct YY2;struct Cyc_YY3_struct
YY3;struct Cyc_YY4_struct YY4;struct Cyc_YY5_struct YY5;struct Cyc_YY6_struct YY6;
struct Cyc_YY7_struct YY7;struct Cyc_YY8_struct YY8;struct Cyc_YY9_struct YY9;struct
Cyc_YY10_struct YY10;struct Cyc_YY11_struct YY11;struct Cyc_YY12_struct YY12;struct
Cyc_YY13_struct YY13;struct Cyc_YY14_struct YY14;struct Cyc_YY15_struct YY15;struct
Cyc_YY16_struct YY16;struct Cyc_YY17_struct YY17;struct Cyc_YY18_struct YY18;struct
Cyc_YY19_struct YY19;struct Cyc_YY20_struct YY20;struct Cyc_YY21_struct YY21;struct
Cyc_YY22_struct YY22;struct Cyc_YY23_struct YY23;struct Cyc_YY24_struct YY24;struct
Cyc_YY25_struct YY25;struct Cyc_YY26_struct YY26;struct Cyc_YY27_struct YY27;struct
Cyc_YY28_struct YY28;struct Cyc_YY29_struct YY29;struct Cyc_YY30_struct YY30;struct
Cyc_YY31_struct YY31;struct Cyc_YY32_struct YY32;struct Cyc_YY33_struct YY33;struct
Cyc_YY34_struct YY34;struct Cyc_YY35_struct YY35;struct Cyc_YY36_struct YY36;struct
Cyc_YY37_struct YY37;struct Cyc_YY38_struct YY38;struct Cyc_YY39_struct YY39;struct
Cyc_YY40_struct YY40;struct Cyc_YY41_struct YY41;struct Cyc_YY42_struct YY42;struct
Cyc_YY43_struct YY43;struct Cyc_YY44_struct YY44;struct Cyc_YY45_struct YY45;struct
Cyc_YY46_struct YY46;struct Cyc_YY47_struct YY47;struct Cyc_YY48_struct YY48;struct
Cyc_YY49_struct YY49;struct Cyc_YY50_struct YY50;struct Cyc_YY51_struct YY51;struct
Cyc_YY52_struct YY52;struct Cyc_YYINITIALSVAL_struct YYINITIALSVAL;};struct Cyc_Yyltype{
int timestamp;int first_line;int first_column;int last_line;int last_column;};struct
Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;
struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct _tuple11{void*f1;void*f2;};struct _tuple11*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple11*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
le;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);void Cyc_Tc_tc(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*
ds);typedef struct{int quot;int rem;}Cyc_div_t;typedef struct{long quot;long rem;}Cyc_ldiv_t;
void*abort();void exit(int);int system(const char*);struct Cyc_timespec{long tv_sec;
int tv_nsec;};struct Cyc_stat{int st_dev;unsigned int st_ino;unsigned short st_mode;
unsigned short st_nlink;unsigned int st_uid;unsigned int st_gid;int st_rdev;struct
Cyc_timespec st_atimespec;struct Cyc_timespec st_mtimespec;struct Cyc_timespec
st_ctimespec;long long st_size;long long st_blocks;unsigned int st_blksize;
unsigned int st_flags;unsigned int st_gen;int st_lspare;long long st_qspare[2];};int
mkdir(const char*pathname,unsigned short mode);struct Cyc_flock{long long l_start;
long long l_len;int l_pid;short l_type;short l_whence;};struct Cyc_Flock_struct{int
tag;struct Cyc_flock*f1;};struct Cyc_Long_struct{int tag;long f1;};int Cyc_open(const
char*,int,struct _dynforward_ptr);struct Cyc_option{struct _dynforward_ptr name;int
has_arg;int*flag;int val;};int chdir(const char*);int close(int);struct
_dynforward_ptr Cyc_getcwd(struct _dynforward_ptr buf,unsigned long size);int isspace(
int);int toupper(int);void Cyc_Lex_lex_init(int use_cyclone_keywords);extern char*
Ccomp;static int Cyc_do_setjmp=0;struct Cyc___cycFILE*Cyc_log_file=0;struct Cyc___cycFILE*
Cyc_cstubs_file=0;struct Cyc___cycFILE*Cyc_cycstubs_file=0;int Cyc_log(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap);int Cyc_log(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap){if(Cyc_log_file == 0){{const char*_tmp326;void*_tmp325;(
_tmp325=0,Cyc_fprintf(Cyc_stderr,((_tmp326="Internal error: log file is NULL\n",
_tag_dynforward(_tmp326,sizeof(char),_get_zero_arr_size_char(_tmp326,34)))),
_tag_dynforward(_tmp325,sizeof(void*),0)));}exit(1);}{int _tmp2=Cyc_vfprintf((
struct Cyc___cycFILE*)_check_null(Cyc_log_file),fmt,ap);Cyc_fflush((struct Cyc___cycFILE*)((
struct Cyc___cycFILE*)_check_null(Cyc_log_file)));return _tmp2;}}static struct
_dynforward_ptr*Cyc_current_source=0;static struct Cyc_List_List*Cyc_current_args=
0;static struct Cyc_Set_Set**Cyc_current_targets=0;static void Cyc_add_target(struct
_dynforward_ptr*sptr);static void Cyc_add_target(struct _dynforward_ptr*sptr){
struct Cyc_Set_Set**_tmp327;Cyc_current_targets=((_tmp327=_cycalloc(sizeof(*
_tmp327)),((_tmp327[0]=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct
_dynforward_ptr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),
sptr),_tmp327))));}struct _tuple12{struct _dynforward_ptr*f1;struct Cyc_Set_Set*f2;
};struct _tuple12*Cyc_line(struct Cyc_Lexing_lexbuf*);int Cyc_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_args(struct Cyc_Lexing_lexbuf*);int Cyc_token(struct Cyc_Lexing_lexbuf*);int
Cyc_string(struct Cyc_Lexing_lexbuf*);struct Cyc___cycFILE*Cyc_slurp_out=0;int Cyc_slurp(
struct Cyc_Lexing_lexbuf*);int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*);int Cyc_asm(
struct Cyc_Lexing_lexbuf*);int Cyc_asm_string(struct Cyc_Lexing_lexbuf*);int Cyc_asm_comment(
struct Cyc_Lexing_lexbuf*);struct _tuple13{struct _dynforward_ptr f1;struct
_dynforward_ptr*f2;};struct _tuple13*Cyc_suck_line(struct Cyc_Lexing_lexbuf*);int
Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*);int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*);
struct _dynforward_ptr Cyc_current_line=(struct _dynforward_ptr){(void*)0,(void*)(0
+ 0)};struct _tuple14{struct _dynforward_ptr f1;struct _dynforward_ptr f2;};struct
_tuple15{struct _dynforward_ptr f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;
struct Cyc_List_List*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct
_tuple15*Cyc_spec(struct Cyc_Lexing_lexbuf*);int Cyc_commands(struct Cyc_Lexing_lexbuf*);
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*);int Cyc_block(struct Cyc_Lexing_lexbuf*);
int Cyc_block_string(struct Cyc_Lexing_lexbuf*);int Cyc_block_comment(struct Cyc_Lexing_lexbuf*);
struct _dynforward_ptr Cyc_current_headerfile=(struct _dynforward_ptr){(void*)0,(
void*)(0 + 0)};struct Cyc_List_List*Cyc_snarfed_symbols=0;struct Cyc_List_List*Cyc_current_symbols=
0;struct Cyc_List_List*Cyc_current_cstubs=0;struct Cyc_List_List*Cyc_current_cycstubs=
0;struct Cyc_List_List*Cyc_current_hstubs=0;struct Cyc_List_List*Cyc_current_omit_symbols=
0;struct Cyc_List_List*Cyc_current_cpp=0;struct Cyc_Buffer_t*Cyc_specbuf=0;int Cyc_braces_to_match=
0;int Cyc_parens_to_match=0;int Cyc_numdef=0;const int Cyc_lex_base[406]=(const int[
406]){0,0,75,192,305,310,311,166,312,91,27,384,28,523,637,715,283,325,92,- 3,0,- 1,
- 2,- 8,- 3,1,- 2,323,- 4,2,166,- 5,605,797,312,- 6,- 7,- 4,16,- 3,29,11,835,- 3,910,13,- 14,
223,12,- 2,216,20,26,28,33,23,48,70,54,64,74,100,91,107,94,370,386,111,103,95,122,
122,375,414,111,111,153,377,1024,1139,525,176,210,228,214,216,217,242,498,1253,
1368,- 9,654,- 10,224,245,508,1482,1597,684,- 8,720,- 11,432,510,515,1674,1751,1828,
1909,434,465,550,1984,249,249,249,248,244,254,0,13,4,2065,5,628,2073,2138,660,49,
467,6,2099,7,705,2161,2199,820,- 21,1051,1056,261,314,242,251,258,250,271,281,274,
275,278,288,295,282,- 19,292,296,300,313,321,326,309,325,319,315,326,365,400,417,
418,404,400,400,427,431,- 16,422,421,415,436,433,449,427,449,453,441,445,436,436,
- 18,444,438,442,453,464,447,449,482,489,490,1,4,6,491,492,505,504,516,516,524,
554,2,21,556,557,654,19,20,21,592,555,554,585,592,594,23,650,651,- 13,600,603,658,
659,660,622,623,678,679,686,636,637,693,697,698,645,648,703,704,705,- 12,654,655,
1030,- 20,1165,654,664,661,671,670,665,668,698,700,698,712,1144,730,731,730,743,
1258,1170,747,758,748,749,747,760,1370,752,753,765,778,1375,- 7,- 8,8,1263,2231,9,
996,2255,2293,1341,1279,- 49,1150,- 2,817,- 4,818,997,1028,819,993,1023,1448,820,
2320,2363,824,866,868,907,2433,870,991,- 36,- 42,- 37,2508,- 28,909,- 40,- 25,911,- 27,
- 45,- 39,- 48,2583,2612,1467,889,979,1563,2622,2652,1582,2281,2685,2716,2754,1001,
1090,2824,2862,1082,1092,1084,1127,1148,1204,- 6,- 34,927,2794,- 47,- 30,- 32,- 46,- 29,
- 31,- 33,962,2902,963,964,2128,965,1002,1003,1014,1018,1020,1024,1032,1043,2975,
3059,- 23,- 17,- 15,- 22,2239,1077,- 24,- 41,- 38,- 35,- 26,1282,3141,4,3224,1076,15,1051,
1055,1056,1058,1054,1071,1141};const int Cyc_lex_backtrk[406]=(const int[406]){- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,- 1,3,
5,- 1,- 1,6,5,- 1,- 1,- 1,6,- 1,5,1,0,- 1,0,1,- 1,12,13,- 1,13,13,13,13,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,11,12,2,4,4,- 1,0,0,0,2,0,- 1,- 1,-
1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,6,5,6,5,2,8,3,5,- 1,6,5,- 1,20,20,20,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,14,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,16,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,6,1,9,2,4,- 1,5,4,- 1,- 1,2,- 1,48,- 1,48,48,48,48,48,48,48,48,5,7,48,
48,48,48,0,48,48,- 1,- 1,- 1,0,- 1,43,- 1,- 1,42,- 1,- 1,- 1,- 1,9,7,- 1,7,7,- 1,8,9,- 1,- 1,9,
5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,- 1,9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,2,1,1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1};const int Cyc_lex_default[406]=(const int[406]){- 1,- 1,- 1,297,286,138,
23,36,23,19,- 1,- 1,12,31,46,31,36,23,19,0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,0,- 1,- 1,- 1,0,
0,0,- 1,0,40,- 1,- 1,0,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,
- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,103,- 1,- 1,- 1,- 1,- 1,110,110,110,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,-
1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,0,- 1,- 1,366,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,- 1,0,0,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};const int Cyc_lex_trans[3481]=(const int[3481]){0,0,0,0,0,
0,0,0,0,0,22,19,28,398,19,28,19,28,100,19,45,45,45,45,45,22,45,0,0,0,0,0,21,202,
213,399,21,22,- 1,- 1,22,- 1,- 1,45,203,45,204,22,396,396,396,396,396,396,396,396,
396,396,31,103,22,214,114,40,227,396,396,396,396,396,396,396,396,396,396,396,396,
396,396,396,396,396,396,396,396,396,396,396,396,396,396,31,245,240,235,396,130,
396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,
396,396,396,396,396,396,394,394,394,394,394,394,394,394,394,394,121,20,74,67,54,
55,56,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,
394,394,394,394,394,394,394,394,57,58,59,60,394,61,394,394,394,394,394,394,394,
394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,62,
63,37,298,299,298,298,299,39,22,64,65,68,69,70,129,34,34,34,34,34,34,34,34,71,72,
298,300,301,75,76,302,303,304,104,104,305,306,104,307,308,309,310,311,311,311,
311,311,311,311,311,311,312,77,313,314,315,104,19,316,316,316,316,316,316,316,
316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,19,-
1,- 1,317,316,101,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,
316,316,316,316,316,316,316,316,316,316,287,318,37,288,139,139,24,24,139,131,122,
94,84,81,85,38,82,86,28,87,24,29,83,25,289,88,95,139,96,22,26,26,21,21,115,116,
117,140,118,119,120,142,191,192,26,35,35,35,35,35,35,35,35,177,171,162,30,30,30,
30,30,30,30,30,66,66,156,149,66,73,73,78,78,73,150,78,151,152,153,154,66,66,290,
155,66,157,158,66,132,123,141,39,73,22,78,159,143,144,145,160,146,161,27,66,147,
31,46,21,73,73,163,164,73,148,165,166,167,113,113,113,113,113,113,113,113,113,
113,- 1,32,- 1,- 1,73,- 1,22,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,- 1,168,- 1,- 1,113,- 1,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,89,89,28,169,89,170,100,172,173,174,97,97,104,104,97,175,104,112,112,
176,178,112,179,89,180,105,105,80,80,105,19,80,21,97,181,104,182,183,184,185,112,
186,187,188,189,190,251,193,105,194,80,195,112,112,91,196,112,197,198,21,21,21,
106,107,106,106,106,106,106,106,106,106,106,106,21,112,199,200,201,205,206,106,
106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,
106,106,106,106,106,207,208,209,210,106,211,106,106,106,106,106,106,106,106,106,
106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,47,47,35,212,
47,215,216,33,33,33,33,33,33,33,33,33,33,92,92,230,222,92,223,47,33,33,33,33,33,
33,128,128,128,128,128,128,128,128,48,224,92,225,- 1,226,- 1,228,229,99,99,231,49,
99,232,233,234,93,33,33,33,33,33,33,35,35,35,35,35,35,35,35,99,236,237,238,239,-
1,217,- 1,41,41,218,102,41,101,101,241,242,101,243,219,50,220,244,91,246,51,52,
247,248,249,250,41,53,252,253,268,101,137,137,137,137,137,137,137,137,263,259,42,
42,42,42,42,42,42,42,42,42,221,260,261,262,93,36,28,42,42,42,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,264,100,265,266,42,267,42,42,
42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,269,270,
43,271,272,102,281,33,33,33,33,33,33,33,33,33,33,276,277,278,279,280,282,283,33,
33,33,33,33,33,35,35,35,35,35,35,35,35,284,285,392,391,365,356,331,44,44,44,44,
44,44,44,44,44,44,21,33,33,33,33,33,33,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,327,328,329,326,44,321,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,323,324,325,28,330,359,100,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,388,254,155,138,44,100,44,44,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,78,78,389,363,78,- 1,253,253,
190,102,253,295,295,295,295,295,295,295,295,319,250,364,100,78,229,390,46,273,
273,253,384,273,255,255,- 1,360,255,254,176,79,79,79,79,79,79,79,79,79,79,383,273,
361,362,35,100,255,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,320,385,21,35,79,367,79,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,31,80,80,274,400,80,267,267,256,401,267,402,
393,275,393,393,403,404,257,35,31,31,36,258,80,405,21,255,255,267,0,255,273,273,
0,393,273,19,0,0,79,79,79,79,79,79,79,79,79,79,255,35,31,31,36,273,36,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,0,0,0,355,
79,36,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,89,89,256,355,89,272,272,274,0,272,296,296,257,0,296,0,0,275,355,0,0,0,- 1,89,
0,0,296,296,272,393,296,393,393,296,0,21,31,0,0,90,90,90,90,90,90,90,90,90,90,
296,355,21,393,0,0,0,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,
90,90,90,90,90,90,0,0,0,0,90,0,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,
90,90,90,90,90,90,90,90,90,90,91,92,92,280,280,92,0,280,285,285,0,0,285,31,31,31,
31,31,31,31,31,0,0,0,92,0,280,0,0,0,0,285,0,0,28,0,0,0,0,35,90,90,90,90,90,90,90,
90,90,90,0,0,0,0,0,0,0,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,
90,90,90,90,90,90,90,0,0,0,0,90,0,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,
90,90,90,90,90,90,90,90,90,90,90,97,97,0,357,97,358,358,358,358,358,358,358,358,
358,358,0,0,0,0,337,0,337,0,97,338,338,338,338,338,338,338,338,338,338,0,0,0,0,0,
98,98,98,98,98,98,98,98,98,98,0,0,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,98,98,98,98,98,0,0,0,0,98,0,98,98,98,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,36,99,99,0,0,99,338,338,
338,338,338,338,338,338,338,338,0,0,0,0,341,0,341,0,99,342,342,342,342,342,342,
342,342,342,342,0,0,0,0,0,98,98,98,98,98,98,98,98,98,98,0,0,0,0,0,0,0,98,98,98,
98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,0,0,0,0,98,
0,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,
108,108,108,108,108,108,108,108,108,108,108,108,22,0,0,0,0,0,0,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,0,0,0,0,108,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,109,108,108,108,108,108,108,
108,108,108,108,22,0,0,0,0,0,0,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,0,0,0,0,108,0,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,22,0,0,0,0,0,0,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,- 1,0,0,- 1,108,0,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,0,0,0,0,109,109,
109,109,109,109,109,109,109,109,109,109,111,0,0,0,0,0,0,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,
0,0,0,0,109,0,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,109,109,113,113,113,113,113,113,113,113,113,113,
0,0,0,0,0,0,0,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,28,0,0,124,113,0,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
0,0,28,0,0,133,125,125,125,125,125,125,125,125,127,127,127,127,127,127,127,127,
127,127,0,0,0,0,0,0,0,127,127,127,127,127,127,0,0,0,134,134,134,134,134,134,134,
134,0,0,0,0,0,0,0,31,0,0,- 1,0,385,0,0,127,127,127,127,127,127,386,386,386,386,
386,386,386,386,0,126,127,127,127,127,127,127,127,127,127,127,31,0,0,0,0,0,0,127,
127,127,127,127,127,136,136,136,136,136,136,136,136,136,136,135,0,0,0,0,0,0,136,
136,136,136,136,136,0,0,0,127,127,127,127,127,127,19,0,0,291,0,0,136,136,136,136,
136,136,136,136,136,136,0,136,136,136,136,136,136,136,136,136,136,136,136,0,0,0,
0,0,0,0,0,385,292,292,292,292,292,292,292,292,387,387,387,387,387,387,387,387,0,
136,136,136,136,136,136,0,294,294,294,294,294,294,294,294,294,294,0,0,0,0,0,0,0,
294,294,294,294,294,294,0,0,28,342,342,342,342,342,342,342,342,342,342,0,0,294,
294,294,294,294,294,294,294,294,294,293,294,294,294,294,294,294,294,294,294,294,
294,294,0,0,332,0,343,343,343,343,343,343,343,343,344,344,0,0,0,0,0,0,0,0,0,0,0,
334,294,294,294,294,294,294,345,0,0,0,0,0,0,0,0,346,0,0,347,332,0,333,333,333,
333,333,333,333,333,333,333,334,0,0,0,0,0,0,345,0,0,0,334,0,0,0,0,346,0,335,347,
0,0,0,0,0,0,0,336,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,334,0,0,0,0,0,0,335,0,0,0,0,0,0,
0,0,336,322,322,322,322,322,322,322,322,322,322,0,0,0,0,0,0,0,322,322,322,322,
322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,
322,322,0,0,0,0,322,0,322,322,322,322,322,322,322,322,322,322,322,322,322,322,
322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,
322,322,0,0,0,0,0,0,0,322,322,322,322,322,322,322,322,322,322,322,322,322,322,
322,322,322,322,322,322,322,322,322,322,322,322,0,0,0,0,322,0,322,322,322,322,
322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,
322,322,339,339,339,339,339,339,339,339,339,339,0,0,0,0,0,0,0,0,0,0,0,340,93,0,0,
0,0,332,93,333,333,333,333,333,333,333,333,333,333,338,338,338,338,338,338,338,
338,338,338,0,334,0,0,340,93,0,0,335,0,0,93,91,0,0,0,0,336,91,0,339,339,339,339,
339,339,339,339,339,339,0,0,0,334,0,0,0,0,0,0,335,340,93,0,91,0,0,0,93,336,91,0,
0,342,342,342,342,342,342,342,342,342,342,0,0,0,0,0,0,0,0,0,0,340,93,93,0,0,0,0,
93,93,332,0,343,343,343,343,343,343,343,343,344,344,0,0,0,0,0,0,0,0,0,0,0,334,0,
93,0,0,0,0,353,93,0,0,0,0,0,0,332,354,344,344,344,344,344,344,344,344,344,344,0,
0,0,0,0,334,0,0,0,0,0,334,353,0,0,0,0,0,351,0,0,354,0,0,0,0,0,352,0,0,358,358,
358,358,358,358,358,358,358,358,0,0,0,334,0,0,0,0,0,0,351,340,93,0,0,0,0,0,93,
352,348,348,348,348,348,348,348,348,348,348,0,0,0,0,0,0,0,348,348,348,348,348,
348,340,93,0,0,0,0,0,93,0,0,0,0,0,0,0,348,348,348,348,348,348,348,348,348,348,0,
348,348,348,348,348,348,348,348,348,348,348,348,0,0,0,368,0,349,0,0,369,0,0,0,0,
0,350,0,0,370,370,370,370,370,370,370,370,0,348,348,348,348,348,348,371,0,0,0,0,
349,0,0,0,0,0,0,0,0,350,0,0,0,0,0,0,0,0,0,0,0,0,0,0,372,0,0,0,0,373,374,0,0,0,
375,0,0,0,0,0,0,0,376,0,0,0,377,0,378,0,379,0,380,381,381,381,381,381,381,381,
381,381,381,0,0,0,0,0,0,0,381,381,381,381,381,381,381,381,381,381,381,381,381,
381,381,381,381,381,381,381,381,381,381,381,381,381,0,0,0,0,0,0,381,381,381,381,
381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,
381,381,382,0,0,0,0,0,0,0,0,381,381,381,381,381,381,381,381,381,381,0,0,0,0,0,0,
0,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,
381,381,381,381,381,381,381,0,0,0,0,0,0,381,381,381,381,381,381,381,381,381,381,
381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,22,0,0,395,0,0,0,
394,394,394,394,394,394,394,394,394,394,0,0,0,0,0,0,0,394,394,394,394,394,394,
394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,
0,0,0,0,394,0,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,
394,394,394,394,394,394,394,394,394,394,397,0,0,0,0,0,0,0,396,396,396,396,396,
396,396,396,396,396,0,0,0,0,0,0,0,396,396,396,396,396,396,396,396,396,396,396,
396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,0,0,0,0,396,0,396,
396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,
396,396,396,396,396,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};const int Cyc_lex_check[3481]=(
const int[3481]){- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,25,29,0,122,124,131,133,288,291,
41,41,45,45,41,398,45,- 1,- 1,- 1,- 1,- 1,120,201,212,0,395,10,12,40,10,12,40,41,202,
45,203,20,1,1,1,1,1,1,1,1,1,1,38,48,121,213,10,38,226,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,1,1,1,129,217,218,219,1,129,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,9,18,51,52,53,54,55,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,56,57,58,59,2,60,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,61,62,7,3,3,3,3,3,7,7,63,64,67,68,69,7,30,30,30,30,30,30,30,
30,70,71,3,3,3,74,75,3,3,3,47,47,3,3,47,3,3,3,3,3,3,3,3,3,3,3,3,3,3,76,3,3,3,47,
0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,10,12,40,3,3,81,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,3,16,4,5,5,6,8,5,6,8,82,83,50,84,16,
50,85,27,86,17,27,50,17,4,87,94,5,95,5,6,8,9,18,114,115,116,5,117,118,119,141,
143,143,17,34,34,34,34,34,34,34,34,144,145,146,27,27,27,27,27,27,27,27,65,65,147,
148,65,72,72,77,77,72,149,77,150,151,152,153,66,66,4,154,66,156,157,65,6,8,5,16,
72,16,77,158,142,142,142,159,142,160,17,66,142,27,161,7,73,73,162,163,73,142,164,
165,166,11,11,11,11,11,11,11,11,11,11,103,27,110,103,73,110,3,11,11,11,11,11,11,
11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,111,167,130,111,11,
130,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
11,88,88,66,168,88,169,170,171,172,173,96,96,104,104,96,174,104,105,105,175,177,
105,178,88,179,13,13,80,80,13,73,80,16,96,180,104,181,182,183,184,105,185,186,
187,188,189,191,192,13,193,80,194,112,112,4,195,112,196,197,5,6,8,13,13,13,13,13,
13,13,13,13,13,13,13,17,112,198,199,200,204,205,13,13,13,13,13,13,13,13,13,13,13,
13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,206,207,208,209,13,210,13,13,13,13,
13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,80,211,
14,214,215,32,32,32,32,32,32,32,32,32,32,92,92,220,221,92,222,14,32,32,32,32,32,
32,125,125,125,125,125,125,125,125,14,223,92,224,103,225,110,227,228,99,99,230,
14,99,231,232,233,234,32,32,32,32,32,32,128,128,128,128,128,128,128,128,99,235,
236,237,238,111,216,130,15,15,216,239,15,101,101,240,241,101,242,216,14,216,243,
244,245,14,14,246,247,248,249,15,14,251,252,256,101,134,134,134,134,134,134,134,
134,257,258,15,15,15,15,15,15,15,15,15,15,216,259,260,261,92,262,13,15,15,15,15,
15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,263,99,264,265,
15,266,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
15,15,268,269,15,270,271,101,274,33,33,33,33,33,33,33,33,33,33,275,276,277,278,
279,281,282,33,33,33,33,33,33,137,137,137,137,137,137,137,137,283,284,300,302,
305,309,312,42,42,42,42,42,42,42,42,42,42,14,33,33,33,33,33,33,42,42,42,42,42,42,
42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,313,313,313,314,42,
317,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,42,
42,44,44,44,44,44,44,44,44,44,44,315,315,324,15,327,357,335,44,44,44,44,44,44,44,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,366,368,369,371,44,335,
44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,78,
78,303,306,78,304,253,253,372,373,253,292,292,292,292,292,292,292,292,318,374,
306,336,78,375,303,376,139,139,253,377,139,140,140,304,307,140,253,378,78,78,78,
78,78,78,78,78,78,78,379,139,307,307,345,336,140,78,78,78,78,78,78,78,78,78,78,
78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,318,387,397,345,78,304,78,78,78,
78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,78,79,79,
139,399,79,267,267,140,400,267,401,298,139,298,298,402,403,140,346,349,350,351,
140,79,404,405,255,255,267,- 1,255,273,273,- 1,298,273,267,- 1,- 1,79,79,79,79,79,79,
79,79,79,79,255,346,349,350,351,273,352,79,79,79,79,79,79,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,- 1,- 1,- 1,353,79,352,79,79,79,79,79,79,79,
79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,89,89,255,353,89,272,
272,273,- 1,272,289,289,255,- 1,289,- 1,- 1,273,354,- 1,- 1,- 1,304,89,- 1,- 1,296,296,
272,393,296,393,393,289,- 1,289,272,- 1,- 1,89,89,89,89,89,89,89,89,89,89,296,354,
296,393,- 1,- 1,- 1,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,
89,89,89,89,89,- 1,- 1,- 1,- 1,89,- 1,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,
89,89,89,89,89,89,89,89,89,89,89,90,90,280,280,90,- 1,280,285,285,- 1,- 1,285,295,
295,295,295,295,295,295,295,- 1,- 1,- 1,90,- 1,280,- 1,- 1,- 1,- 1,285,- 1,- 1,280,- 1,- 1,-
1,- 1,285,90,90,90,90,90,90,90,90,90,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,90,90,90,90,90,90,
90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,- 1,- 1,- 1,- 1,90,- 1,90,90,
90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,97,97,- 1,
308,97,308,308,308,308,308,308,308,308,308,308,- 1,- 1,- 1,- 1,334,- 1,334,- 1,97,334,
334,334,334,334,334,334,334,334,334,- 1,- 1,- 1,- 1,- 1,97,97,97,97,97,97,97,97,97,97,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,
97,97,97,97,97,97,- 1,- 1,- 1,- 1,97,- 1,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,
97,97,97,97,97,97,97,97,97,97,97,97,98,98,- 1,- 1,98,337,337,337,337,337,337,337,
337,337,337,- 1,- 1,- 1,- 1,340,- 1,340,- 1,98,340,340,340,340,340,340,340,340,340,340,
- 1,- 1,- 1,- 1,- 1,98,98,98,98,98,98,98,98,98,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,98,98,98,98,
98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,- 1,- 1,- 1,- 1,98,- 1,
98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,98,
106,106,106,106,106,106,106,106,106,106,106,106,106,- 1,- 1,- 1,- 1,- 1,- 1,106,106,
106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,
106,106,106,106,- 1,- 1,- 1,- 1,106,- 1,106,106,106,106,106,106,106,106,106,106,106,
106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,107,107,107,107,107,
107,107,107,107,107,107,107,107,- 1,- 1,- 1,- 1,- 1,- 1,107,107,107,107,107,107,107,
107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,- 1,-
1,- 1,- 1,107,- 1,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,107,
107,107,107,107,107,107,107,107,107,107,108,108,108,108,108,108,108,108,108,108,
108,108,108,- 1,- 1,- 1,- 1,- 1,- 1,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,109,- 1,- 1,109,108,- 1,108,
108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,
108,108,108,108,108,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,109,109,109,109,
109,- 1,- 1,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,
109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,109,- 1,109,109,109,
109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,
109,109,109,113,113,113,113,113,113,113,113,113,113,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,123,- 1,- 1,123,113,- 1,113,113,113,113,113,113,113,113,113,113,113,
113,113,113,113,113,113,113,113,113,113,113,113,113,113,113,- 1,- 1,132,- 1,- 1,132,
123,123,123,123,123,123,123,123,126,126,126,126,126,126,126,126,126,126,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,126,126,126,126,126,126,- 1,- 1,- 1,132,132,132,132,132,132,132,132,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,123,- 1,- 1,109,- 1,370,- 1,- 1,126,126,126,126,126,126,370,370,370,
370,370,370,370,370,- 1,123,127,127,127,127,127,127,127,127,127,127,132,- 1,- 1,- 1,
- 1,- 1,- 1,127,127,127,127,127,127,135,135,135,135,135,135,135,135,135,135,132,- 1,
- 1,- 1,- 1,- 1,- 1,135,135,135,135,135,135,- 1,- 1,- 1,127,127,127,127,127,127,290,- 1,-
1,290,- 1,- 1,136,136,136,136,136,136,136,136,136,136,- 1,135,135,135,135,135,135,
136,136,136,136,136,136,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,386,290,290,290,290,290,290,290,
290,386,386,386,386,386,386,386,386,- 1,136,136,136,136,136,136,- 1,293,293,293,
293,293,293,293,293,293,293,- 1,- 1,- 1,- 1,- 1,- 1,- 1,293,293,293,293,293,293,- 1,- 1,
290,341,341,341,341,341,341,341,341,341,341,- 1,- 1,294,294,294,294,294,294,294,
294,294,294,290,293,293,293,293,293,293,294,294,294,294,294,294,- 1,- 1,310,- 1,310,
310,310,310,310,310,310,310,310,310,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,310,294,294,
294,294,294,294,310,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,310,- 1,- 1,310,311,- 1,311,311,311,311,
311,311,311,311,311,311,310,- 1,- 1,- 1,- 1,- 1,- 1,310,- 1,- 1,- 1,311,- 1,- 1,- 1,- 1,310,-
1,311,310,- 1,- 1,- 1,- 1,- 1,- 1,- 1,311,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
311,- 1,- 1,- 1,- 1,- 1,- 1,311,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,311,316,316,316,316,316,316,
316,316,316,316,- 1,- 1,- 1,- 1,- 1,- 1,- 1,316,316,316,316,316,316,316,316,316,316,316,
316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,- 1,- 1,- 1,- 1,316,- 1,
316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,316,
316,316,316,316,316,316,322,322,322,322,322,322,322,322,322,322,- 1,- 1,- 1,- 1,- 1,-
1,- 1,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,
322,322,322,322,322,322,322,- 1,- 1,- 1,- 1,322,- 1,322,322,322,322,322,322,322,322,
322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,322,332,332,
332,332,332,332,332,332,332,332,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,332,332,- 1,- 1,-
1,- 1,333,332,333,333,333,333,333,333,333,333,333,333,338,338,338,338,338,338,338,
338,338,338,- 1,333,- 1,- 1,332,332,- 1,- 1,333,- 1,- 1,332,338,- 1,- 1,- 1,- 1,333,338,- 1,
339,339,339,339,339,339,339,339,339,339,- 1,- 1,- 1,333,- 1,- 1,- 1,- 1,- 1,- 1,333,339,
339,- 1,338,- 1,- 1,- 1,339,333,338,- 1,- 1,342,342,342,342,342,342,342,342,342,342,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,339,339,342,- 1,- 1,- 1,- 1,339,342,343,- 1,343,343,343,
343,343,343,343,343,343,343,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,343,- 1,342,- 1,- 1,- 1,
- 1,343,342,- 1,- 1,- 1,- 1,- 1,- 1,344,343,344,344,344,344,344,344,344,344,344,344,- 1,
- 1,- 1,- 1,- 1,343,- 1,- 1,- 1,- 1,- 1,344,343,- 1,- 1,- 1,- 1,- 1,344,- 1,- 1,343,- 1,- 1,- 1,- 1,
- 1,344,- 1,- 1,358,358,358,358,358,358,358,358,358,358,- 1,- 1,- 1,344,- 1,- 1,- 1,- 1,- 1,
- 1,344,358,358,- 1,- 1,- 1,- 1,- 1,358,344,347,347,347,347,347,347,347,347,347,347,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,347,347,347,347,347,347,358,358,- 1,- 1,- 1,- 1,- 1,358,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,348,348,348,348,348,348,348,348,348,348,- 1,347,347,347,347,347,347,348,
348,348,348,348,348,- 1,- 1,- 1,367,- 1,348,- 1,- 1,367,- 1,- 1,- 1,- 1,- 1,348,- 1,- 1,367,
367,367,367,367,367,367,367,- 1,348,348,348,348,348,348,367,- 1,- 1,- 1,- 1,348,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,348,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,367,- 1,- 1,- 1,- 1,
367,367,- 1,- 1,- 1,367,- 1,- 1,- 1,- 1,- 1,- 1,- 1,367,- 1,- 1,- 1,367,- 1,367,- 1,367,- 1,367,
380,380,380,380,380,380,380,380,380,380,- 1,- 1,- 1,- 1,- 1,- 1,- 1,380,380,380,380,380,
380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,380,
380,- 1,- 1,- 1,- 1,- 1,- 1,380,380,380,380,380,380,380,380,380,380,380,380,380,380,
380,380,380,380,380,380,380,380,380,380,380,380,381,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,381,
381,381,381,381,381,381,381,381,381,- 1,- 1,- 1,- 1,- 1,- 1,- 1,381,381,381,381,381,381,
381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,
- 1,- 1,- 1,- 1,- 1,- 1,381,381,381,381,381,381,381,381,381,381,381,381,381,381,381,
381,381,381,381,381,381,381,381,381,381,381,394,- 1,- 1,394,- 1,- 1,- 1,394,394,394,
394,394,394,394,394,394,394,- 1,- 1,- 1,- 1,- 1,- 1,- 1,394,394,394,394,394,394,394,394,
394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,- 1,- 1,- 1,
- 1,394,- 1,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,394,
394,394,394,394,394,394,394,394,394,396,- 1,- 1,- 1,- 1,- 1,- 1,- 1,396,396,396,396,396,
396,396,396,396,396,- 1,- 1,- 1,- 1,- 1,- 1,- 1,396,396,396,396,396,396,396,396,396,396,
396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,- 1,- 1,- 1,- 1,396,
- 1,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,396,
396,396,396,396,396,396,396,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*
lbuf);int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){int state;
int base;int backtrk;int c;state=start_state;if(state >= 0){lbuf->lex_last_pos=(lbuf->lex_start_pos=
lbuf->lex_curr_pos);lbuf->lex_last_action=- 1;}else{state=(- state)- 1;}while(1){
base=Cyc_lex_base[_check_known_subscript_notnull(406,state)];if(base < 0)return(-
base)- 1;backtrk=Cyc_lex_backtrk[_check_known_subscript_notnull(406,state)];if(
backtrk >= 0){lbuf->lex_last_pos=lbuf->lex_curr_pos;lbuf->lex_last_action=backtrk;}
if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){if(!lbuf->lex_eof_reached)return(-
state)- 1;else{c=256;}}else{c=(int)*((char*)_check_dynforward_subscript(lbuf->lex_buffer,
sizeof(char),lbuf->lex_curr_pos ++));if(c == - 1)c=256;}if(Cyc_lex_check[
_check_known_subscript_notnull(3481,base + c)]== state)state=Cyc_lex_trans[
_check_known_subscript_notnull(3481,base + c)];else{state=Cyc_lex_default[
_check_known_subscript_notnull(406,state)];}if(state < 0){lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1){struct Cyc_Lexing_Error_struct _tmp32D;const char*
_tmp32C;struct Cyc_Lexing_Error_struct*_tmp32B;(int)_throw((void*)((_tmp32B=
_cycalloc(sizeof(*_tmp32B)),((_tmp32B[0]=((_tmp32D.tag=Cyc_Lexing_Error,((
_tmp32D.f1=((_tmp32C="empty token",_tag_dynforward(_tmp32C,sizeof(char),
_get_zero_arr_size_char(_tmp32C,12)))),_tmp32D)))),_tmp32B)))));}else{return lbuf->lex_last_action;}}
else{if(c == 256)lbuf->lex_eof_reached=0;}}}struct _tuple12*Cyc_line_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate);struct _tuple12*Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,
int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0:
_LL0: Cyc_macroname(lexbuf);for(0;Cyc_current_args != 0;Cyc_current_args=((struct
Cyc_List_List*)_check_null(Cyc_current_args))->tl){struct Cyc_Set_Set**_tmp32E;
Cyc_current_targets=((_tmp32E=_cycalloc(sizeof(*_tmp32E)),((_tmp32E[0]=((struct
Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dynforward_ptr*elt))Cyc_Set_delete)(*((
struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),(struct _dynforward_ptr*)((
struct Cyc_List_List*)_check_null(Cyc_current_args))->hd),_tmp32E))));}{struct
_tuple12*_tmp32F;return(_tmp32F=_cycalloc(sizeof(*_tmp32F)),((_tmp32F->f1=(
struct _dynforward_ptr*)_check_null(Cyc_current_source),((_tmp32F->f2=*((struct
Cyc_Set_Set**)_check_null(Cyc_current_targets)),_tmp32F)))));}case 1: _LL1: return
Cyc_line(lexbuf);case 2: _LL2: return 0;default: _LL3:(lexbuf->refill_buff)(lexbuf);
return Cyc_line_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp335;const
char*_tmp334;struct Cyc_Lexing_Error_struct*_tmp333;(int)_throw((void*)((_tmp333=
_cycalloc(sizeof(*_tmp333)),((_tmp333[0]=((_tmp335.tag=Cyc_Lexing_Error,((
_tmp335.f1=((_tmp334="some action didn't return!",_tag_dynforward(_tmp334,
sizeof(char),_get_zero_arr_size_char(_tmp334,27)))),_tmp335)))),_tmp333)))));}}
struct _tuple12*Cyc_line(struct Cyc_Lexing_lexbuf*lexbuf);struct _tuple12*Cyc_line(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_line_rec(lexbuf,0);}int Cyc_macroname_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL5:{struct _dynforward_ptr*_tmp336;Cyc_current_source=((_tmp336=_cycalloc(
sizeof(*_tmp336)),((_tmp336[0]=(struct _dynforward_ptr)Cyc_substring((struct
_dynforward_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2)),_tmp336))));}Cyc_current_args=0;{
struct Cyc_Set_Set**_tmp337;Cyc_current_targets=((_tmp337=_cycalloc(sizeof(*
_tmp337)),((_tmp337[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dynforward_ptr*,
struct _dynforward_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp337))));}Cyc_token(
lexbuf);return 0;case 1: _LL6:{struct _dynforward_ptr*_tmp338;Cyc_current_source=((
_tmp338=_cycalloc(sizeof(*_tmp338)),((_tmp338[0]=(struct _dynforward_ptr)Cyc_substring((
struct _dynforward_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1)),_tmp338))));}Cyc_current_args=0;{
struct Cyc_Set_Set**_tmp339;Cyc_current_targets=((_tmp339=_cycalloc(sizeof(*
_tmp339)),((_tmp339[0]=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dynforward_ptr*,
struct _dynforward_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp339))));}Cyc_args(
lexbuf);return 0;case 2: _LL7:{struct _dynforward_ptr*_tmp33A;Cyc_current_source=((
_tmp33A=_cycalloc(sizeof(*_tmp33A)),((_tmp33A[0]=(struct _dynforward_ptr)Cyc_Lexing_lexeme(
lexbuf),_tmp33A))));}Cyc_current_args=0;{struct Cyc_Set_Set**_tmp33B;Cyc_current_targets=((
_tmp33B=_cycalloc(sizeof(*_tmp33B)),((_tmp33B[0]=((struct Cyc_Set_Set*(*)(int(*
cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),
_tmp33B))));}Cyc_token(lexbuf);return 0;default: _LL8:(lexbuf->refill_buff)(lexbuf);
return Cyc_macroname_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp341;
const char*_tmp340;struct Cyc_Lexing_Error_struct*_tmp33F;(int)_throw((void*)((
_tmp33F=_cycalloc(sizeof(*_tmp33F)),((_tmp33F[0]=((_tmp341.tag=Cyc_Lexing_Error,((
_tmp341.f1=((_tmp340="some action didn't return!",_tag_dynforward(_tmp340,
sizeof(char),_get_zero_arr_size_char(_tmp340,27)))),_tmp341)))),_tmp33F)))));}}
int Cyc_macroname(struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_macroname(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_macroname_rec(lexbuf,1);}int Cyc_args_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate);int Cyc_args_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLA: {struct
_dynforward_ptr*_tmp342;struct _dynforward_ptr*_tmp15=(_tmp342=_cycalloc(sizeof(*
_tmp342)),((_tmp342[0]=(struct _dynforward_ptr)Cyc_substring((struct
_dynforward_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 2)),_tmp342)));{struct Cyc_List_List*
_tmp343;Cyc_current_args=((_tmp343=_cycalloc(sizeof(*_tmp343)),((_tmp343->hd=
_tmp15,((_tmp343->tl=Cyc_current_args,_tmp343))))));}return Cyc_args(lexbuf);}
case 1: _LLB: {struct _dynforward_ptr*_tmp344;struct _dynforward_ptr*_tmp18=(_tmp344=
_cycalloc(sizeof(*_tmp344)),((_tmp344[0]=(struct _dynforward_ptr)Cyc_substring((
struct _dynforward_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1)),_tmp344)));{struct Cyc_List_List*
_tmp345;Cyc_current_args=((_tmp345=_cycalloc(sizeof(*_tmp345)),((_tmp345->hd=
_tmp18,((_tmp345->tl=Cyc_current_args,_tmp345))))));}return Cyc_token(lexbuf);}
default: _LLC:(lexbuf->refill_buff)(lexbuf);return Cyc_args_rec(lexbuf,lexstate);}{
struct Cyc_Lexing_Error_struct _tmp34B;const char*_tmp34A;struct Cyc_Lexing_Error_struct*
_tmp349;(int)_throw((void*)((_tmp349=_cycalloc(sizeof(*_tmp349)),((_tmp349[0]=((
_tmp34B.tag=Cyc_Lexing_Error,((_tmp34B.f1=((_tmp34A="some action didn't return!",
_tag_dynforward(_tmp34A,sizeof(char),_get_zero_arr_size_char(_tmp34A,27)))),
_tmp34B)))),_tmp349)))));}}int Cyc_args(struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_args(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_args_rec(lexbuf,2);}int Cyc_token_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_token_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LLE:{struct _dynforward_ptr*_tmp34C;Cyc_add_target(((_tmp34C=_cycalloc(
sizeof(*_tmp34C)),((_tmp34C[0]=(struct _dynforward_ptr)Cyc_Lexing_lexeme(lexbuf),
_tmp34C)))));}return Cyc_token(lexbuf);case 1: _LLF: return 0;case 2: _LL10: return Cyc_token(
lexbuf);case 3: _LL11: Cyc_string(lexbuf);return Cyc_token(lexbuf);case 4: _LL12:
return Cyc_token(lexbuf);case 5: _LL13: return Cyc_token(lexbuf);case 6: _LL14: return
Cyc_token(lexbuf);case 7: _LL15: return Cyc_token(lexbuf);case 8: _LL16: return Cyc_token(
lexbuf);case 9: _LL17: return Cyc_token(lexbuf);case 10: _LL18: return Cyc_token(lexbuf);
case 11: _LL19: return Cyc_token(lexbuf);case 12: _LL1A: return Cyc_token(lexbuf);case 13:
_LL1B: return Cyc_token(lexbuf);case 14: _LL1C: return Cyc_token(lexbuf);case 15: _LL1D:
return Cyc_token(lexbuf);case 16: _LL1E: return Cyc_token(lexbuf);case 17: _LL1F: return
Cyc_token(lexbuf);case 18: _LL20: return Cyc_token(lexbuf);case 19: _LL21: return Cyc_token(
lexbuf);case 20: _LL22: return Cyc_token(lexbuf);case 21: _LL23: return Cyc_token(lexbuf);
case 22: _LL24: return Cyc_token(lexbuf);case 23: _LL25: return Cyc_token(lexbuf);case 24:
_LL26: return Cyc_token(lexbuf);case 25: _LL27: return Cyc_token(lexbuf);case 26: _LL28:
return Cyc_token(lexbuf);case 27: _LL29: return Cyc_token(lexbuf);case 28: _LL2A: return
Cyc_token(lexbuf);case 29: _LL2B: return Cyc_token(lexbuf);case 30: _LL2C: return Cyc_token(
lexbuf);case 31: _LL2D: return Cyc_token(lexbuf);case 32: _LL2E: return Cyc_token(lexbuf);
case 33: _LL2F: return Cyc_token(lexbuf);case 34: _LL30: return Cyc_token(lexbuf);case 35:
_LL31: return Cyc_token(lexbuf);case 36: _LL32: return Cyc_token(lexbuf);case 37: _LL33:
return Cyc_token(lexbuf);case 38: _LL34: return Cyc_token(lexbuf);case 39: _LL35: return
Cyc_token(lexbuf);case 40: _LL36: return Cyc_token(lexbuf);case 41: _LL37: return Cyc_token(
lexbuf);case 42: _LL38: return Cyc_token(lexbuf);case 43: _LL39: return Cyc_token(lexbuf);
case 44: _LL3A: return Cyc_token(lexbuf);case 45: _LL3B: return Cyc_token(lexbuf);case 46:
_LL3C: return Cyc_token(lexbuf);case 47: _LL3D: return Cyc_token(lexbuf);case 48: _LL3E:
return Cyc_token(lexbuf);default: _LL3F:(lexbuf->refill_buff)(lexbuf);return Cyc_token_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp352;const char*_tmp351;struct
Cyc_Lexing_Error_struct*_tmp350;(int)_throw((void*)((_tmp350=_cycalloc(sizeof(*
_tmp350)),((_tmp350[0]=((_tmp352.tag=Cyc_Lexing_Error,((_tmp352.f1=((_tmp351="some action didn't return!",
_tag_dynforward(_tmp351,sizeof(char),_get_zero_arr_size_char(_tmp351,27)))),
_tmp352)))),_tmp350)))));}}int Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_token(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_token_rec(lexbuf,3);}int Cyc_string_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL41: return Cyc_string(lexbuf);case 1: _LL42: return 0;case 2: _LL43: return Cyc_string(
lexbuf);case 3: _LL44: return Cyc_string(lexbuf);case 4: _LL45: return Cyc_string(lexbuf);
case 5: _LL46: return Cyc_string(lexbuf);case 6: _LL47: return Cyc_string(lexbuf);case 7:
_LL48: return 0;case 8: _LL49: return 0;case 9: _LL4A: return Cyc_string(lexbuf);default:
_LL4B:(lexbuf->refill_buff)(lexbuf);return Cyc_string_rec(lexbuf,lexstate);}{
struct Cyc_Lexing_Error_struct _tmp358;const char*_tmp357;struct Cyc_Lexing_Error_struct*
_tmp356;(int)_throw((void*)((_tmp356=_cycalloc(sizeof(*_tmp356)),((_tmp356[0]=((
_tmp358.tag=Cyc_Lexing_Error,((_tmp358.f1=((_tmp357="some action didn't return!",
_tag_dynforward(_tmp357,sizeof(char),_get_zero_arr_size_char(_tmp357,27)))),
_tmp358)))),_tmp356)))));}}int Cyc_string(struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_string(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_string_rec(lexbuf,4);}int Cyc_slurp_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_slurp_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL4D: return 0;case 1: _LL4E: Cyc_fputc((int)'"',(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));while(Cyc_slurp_string(lexbuf)){;}return 1;case 2:
_LL4F: Cyc_fputs((const char*)"*__IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));{const char*_tmp35B;void*_tmp35A;(_tmp35A=0,Cyc_log(((
_tmp35B="Warning: declaration of malloc sidestepped\n",_tag_dynforward(_tmp35B,
sizeof(char),_get_zero_arr_size_char(_tmp35B,44)))),_tag_dynforward(_tmp35A,
sizeof(void*),0)));}return 1;case 3: _LL50: Cyc_fputs((const char*)" __IGNORE_FOR_CYCLONE_MALLOC(",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*_tmp35E;void*_tmp35D;(
_tmp35D=0,Cyc_log(((_tmp35E="Warning: declaration of malloc sidestepped\n",
_tag_dynforward(_tmp35E,sizeof(char),_get_zero_arr_size_char(_tmp35E,44)))),
_tag_dynforward(_tmp35D,sizeof(void*),0)));}return 1;case 4: _LL51: Cyc_fputs((const
char*)"*__IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{
const char*_tmp361;void*_tmp360;(_tmp360=0,Cyc_log(((_tmp361="Warning: declaration of calloc sidestepped\n",
_tag_dynforward(_tmp361,sizeof(char),_get_zero_arr_size_char(_tmp361,44)))),
_tag_dynforward(_tmp360,sizeof(void*),0)));}return 1;case 5: _LL52: Cyc_fputs((const
char*)" __IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{
const char*_tmp364;void*_tmp363;(_tmp363=0,Cyc_log(((_tmp364="Warning: declaration of calloc sidestepped\n",
_tag_dynforward(_tmp364,sizeof(char),_get_zero_arr_size_char(_tmp364,44)))),
_tag_dynforward(_tmp363,sizeof(void*),0)));}return 1;case 6: _LL53: Cyc_fputs((const
char*)"__region",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));{const char*
_tmp367;void*_tmp366;(_tmp366=0,Cyc_log(((_tmp367="Warning: use of region sidestepped\n",
_tag_dynforward(_tmp367,sizeof(char),_get_zero_arr_size_char(_tmp367,36)))),
_tag_dynforward(_tmp366,sizeof(void*),0)));}return 1;case 7: _LL54: return 1;case 8:
_LL55: return 1;case 9: _LL56: return 1;case 10: _LL57: return 1;case 11: _LL58: return 1;case
12: _LL59: return 1;case 13: _LL5A: Cyc_fputs((const char*)"inline",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));return 1;case 14: _LL5B: Cyc_fputs((const char*)"inline",(
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 15: _LL5C: Cyc_fputs((
const char*)"const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;
case 16: _LL5D: Cyc_fputs((const char*)"const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
return 1;case 17: _LL5E: Cyc_fputs((const char*)"int",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));return 1;case 18: _LL5F: return 1;case 19: _LL60: Cyc_parens_to_match=
1;while(Cyc_asm(lexbuf)){;}Cyc_fputs((const char*)"0",(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));{const char*_tmp36A;void*_tmp369;(_tmp369=0,Cyc_log(((
_tmp36A="Warning: replacing use of __asm__ with 0\n",_tag_dynforward(_tmp36A,
sizeof(char),_get_zero_arr_size_char(_tmp36A,42)))),_tag_dynforward(_tmp369,
sizeof(void*),0)));}return 1;case 20: _LL61: Cyc_fputc((int)Cyc_Lexing_lexeme_char(
lexbuf,0),(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;default:
_LL62:(lexbuf->refill_buff)(lexbuf);return Cyc_slurp_rec(lexbuf,lexstate);}{
struct Cyc_Lexing_Error_struct _tmp370;const char*_tmp36F;struct Cyc_Lexing_Error_struct*
_tmp36E;(int)_throw((void*)((_tmp36E=_cycalloc(sizeof(*_tmp36E)),((_tmp36E[0]=((
_tmp370.tag=Cyc_Lexing_Error,((_tmp370.f1=((_tmp36F="some action didn't return!",
_tag_dynforward(_tmp36F,sizeof(char),_get_zero_arr_size_char(_tmp36F,27)))),
_tmp370)))),_tmp36E)))));}}int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_slurp(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(lexbuf,5);}int Cyc_slurp_string_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL64: return 0;case 1: _LL65: Cyc_fputc((int)'"',(struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));return 0;case 2: _LL66:{const char*_tmp373;void*_tmp372;(
_tmp372=0,Cyc_log(((_tmp373="Warning: unclosed string\n",_tag_dynforward(_tmp373,
sizeof(char),_get_zero_arr_size_char(_tmp373,26)))),_tag_dynforward(_tmp372,
sizeof(void*),0)));}{const char*_tmp377;void*_tmp376[1];struct Cyc_String_pa_struct
_tmp375;(_tmp375.tag=0,((_tmp375.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp376[0]=& _tmp375,Cyc_fprintf((
struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp377="%s",_tag_dynforward(
_tmp377,sizeof(char),_get_zero_arr_size_char(_tmp377,3)))),_tag_dynforward(
_tmp376,sizeof(void*),1)))))));}return 1;case 3: _LL67:{const char*_tmp37B;void*
_tmp37A[1];struct Cyc_String_pa_struct _tmp379;(_tmp379.tag=0,((_tmp379.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Lexing_lexeme(lexbuf)),((_tmp37A[0]=&
_tmp379,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((_tmp37B="%s",
_tag_dynforward(_tmp37B,sizeof(char),_get_zero_arr_size_char(_tmp37B,3)))),
_tag_dynforward(_tmp37A,sizeof(void*),1)))))));}return 1;case 4: _LL68:{const char*
_tmp37F;void*_tmp37E[1];struct Cyc_String_pa_struct _tmp37D;(_tmp37D.tag=0,((
_tmp37D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Lexing_lexeme(
lexbuf)),((_tmp37E[0]=& _tmp37D,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp37F="%s",_tag_dynforward(_tmp37F,sizeof(char),_get_zero_arr_size_char(
_tmp37F,3)))),_tag_dynforward(_tmp37E,sizeof(void*),1)))))));}return 1;case 5:
_LL69:{const char*_tmp383;void*_tmp382[1];struct Cyc_String_pa_struct _tmp381;(
_tmp381.tag=0,((_tmp381.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Lexing_lexeme(
lexbuf)),((_tmp382[0]=& _tmp381,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp383="%s",_tag_dynforward(_tmp383,sizeof(char),_get_zero_arr_size_char(
_tmp383,3)))),_tag_dynforward(_tmp382,sizeof(void*),1)))))));}return 1;case 6:
_LL6A:{const char*_tmp387;void*_tmp386[1];struct Cyc_String_pa_struct _tmp385;(
_tmp385.tag=0,((_tmp385.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Lexing_lexeme(
lexbuf)),((_tmp386[0]=& _tmp385,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp387="%s",_tag_dynforward(_tmp387,sizeof(char),_get_zero_arr_size_char(
_tmp387,3)))),_tag_dynforward(_tmp386,sizeof(void*),1)))))));}return 1;case 7:
_LL6B:{const char*_tmp38B;void*_tmp38A[1];struct Cyc_String_pa_struct _tmp389;(
_tmp389.tag=0,((_tmp389.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Lexing_lexeme(
lexbuf)),((_tmp38A[0]=& _tmp389,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp38B="%s",_tag_dynforward(_tmp38B,sizeof(char),_get_zero_arr_size_char(
_tmp38B,3)))),_tag_dynforward(_tmp38A,sizeof(void*),1)))))));}return 1;case 8:
_LL6C:{const char*_tmp38F;void*_tmp38E[1];struct Cyc_String_pa_struct _tmp38D;(
_tmp38D.tag=0,((_tmp38D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Lexing_lexeme(
lexbuf)),((_tmp38E[0]=& _tmp38D,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out),((
_tmp38F="%s",_tag_dynforward(_tmp38F,sizeof(char),_get_zero_arr_size_char(
_tmp38F,3)))),_tag_dynforward(_tmp38E,sizeof(void*),1)))))));}return 1;default:
_LL6D:(lexbuf->refill_buff)(lexbuf);return Cyc_slurp_string_rec(lexbuf,lexstate);}{
struct Cyc_Lexing_Error_struct _tmp395;const char*_tmp394;struct Cyc_Lexing_Error_struct*
_tmp393;(int)_throw((void*)((_tmp393=_cycalloc(sizeof(*_tmp393)),((_tmp393[0]=((
_tmp395.tag=Cyc_Lexing_Error,((_tmp395.f1=((_tmp394="some action didn't return!",
_tag_dynforward(_tmp394,sizeof(char),_get_zero_arr_size_char(_tmp394,27)))),
_tmp395)))),_tmp393)))));}}int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf);
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(
lexbuf,6);}int Cyc_asm_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_asm_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL6F: return 0;case 1: _LL70: if(Cyc_parens_to_match
== 1)return 0;-- Cyc_parens_to_match;return 1;case 2: _LL71: ++ Cyc_parens_to_match;
return 1;case 3: _LL72: while(Cyc_asm_string(lexbuf)){;}return 1;case 4: _LL73: while(
Cyc_asm_comment(lexbuf)){;}return 1;case 5: _LL74: return 1;case 6: _LL75: return 1;
default: _LL76:(lexbuf->refill_buff)(lexbuf);return Cyc_asm_rec(lexbuf,lexstate);}{
struct Cyc_Lexing_Error_struct _tmp39B;const char*_tmp39A;struct Cyc_Lexing_Error_struct*
_tmp399;(int)_throw((void*)((_tmp399=_cycalloc(sizeof(*_tmp399)),((_tmp399[0]=((
_tmp39B.tag=Cyc_Lexing_Error,((_tmp39B.f1=((_tmp39A="some action didn't return!",
_tag_dynforward(_tmp39A,sizeof(char),_get_zero_arr_size_char(_tmp39A,27)))),
_tmp39B)))),_tmp399)))));}}int Cyc_asm(struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_asm(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_rec(lexbuf,7);}int Cyc_asm_string_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL78:{const char*_tmp39E;void*_tmp39D;(_tmp39D=0,Cyc_log(((_tmp39E="Warning: unclosed string\n",
_tag_dynforward(_tmp39E,sizeof(char),_get_zero_arr_size_char(_tmp39E,26)))),
_tag_dynforward(_tmp39D,sizeof(void*),0)));}return 0;case 1: _LL79: return 0;case 2:
_LL7A:{const char*_tmp3A1;void*_tmp3A0;(_tmp3A0=0,Cyc_log(((_tmp3A1="Warning: unclosed string\n",
_tag_dynforward(_tmp3A1,sizeof(char),_get_zero_arr_size_char(_tmp3A1,26)))),
_tag_dynforward(_tmp3A0,sizeof(void*),0)));}return 1;case 3: _LL7B: return 1;case 4:
_LL7C: return 1;case 5: _LL7D: return 1;case 6: _LL7E: return 1;case 7: _LL7F: return 1;case 8:
_LL80: return 1;default: _LL81:(lexbuf->refill_buff)(lexbuf);return Cyc_asm_string_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp3A7;const char*_tmp3A6;struct
Cyc_Lexing_Error_struct*_tmp3A5;(int)_throw((void*)((_tmp3A5=_cycalloc(sizeof(*
_tmp3A5)),((_tmp3A5[0]=((_tmp3A7.tag=Cyc_Lexing_Error,((_tmp3A7.f1=((_tmp3A6="some action didn't return!",
_tag_dynforward(_tmp3A6,sizeof(char),_get_zero_arr_size_char(_tmp3A6,27)))),
_tmp3A7)))),_tmp3A5)))));}}int Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf);int
Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(lexbuf,8);}
int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_asm_comment_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL83:{const char*_tmp3AA;void*_tmp3A9;(_tmp3A9=0,
Cyc_log(((_tmp3AA="Warning: unclosed comment\n",_tag_dynforward(_tmp3AA,sizeof(
char),_get_zero_arr_size_char(_tmp3AA,27)))),_tag_dynforward(_tmp3A9,sizeof(void*),
0)));}return 0;case 1: _LL84: return 0;case 2: _LL85: return 1;default: _LL86:(lexbuf->refill_buff)(
lexbuf);return Cyc_asm_comment_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp3B0;const char*_tmp3AF;struct Cyc_Lexing_Error_struct*_tmp3AE;(int)_throw((
void*)((_tmp3AE=_cycalloc(sizeof(*_tmp3AE)),((_tmp3AE[0]=((_tmp3B0.tag=Cyc_Lexing_Error,((
_tmp3B0.f1=((_tmp3AF="some action didn't return!",_tag_dynforward(_tmp3AF,
sizeof(char),_get_zero_arr_size_char(_tmp3AF,27)))),_tmp3B0)))),_tmp3AE)))));}}
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_asm_comment_rec(lexbuf,9);}struct _tuple13*Cyc_suck_line_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);struct _tuple13*Cyc_suck_line_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,
lexbuf);switch(lexstate){case 0: _LL88:{const char*_tmp3B1;Cyc_current_line=((
_tmp3B1="#define ",_tag_dynforward(_tmp3B1,sizeof(char),_get_zero_arr_size_char(
_tmp3B1,9))));}Cyc_suck_macroname(lexbuf);{struct _tuple13*_tmp3B2;return(_tmp3B2=
_cycalloc(sizeof(*_tmp3B2)),((_tmp3B2->f1=Cyc_current_line,((_tmp3B2->f2=(struct
_dynforward_ptr*)_check_null(Cyc_current_source),_tmp3B2)))));}case 1: _LL89:
return Cyc_suck_line(lexbuf);case 2: _LL8A: return 0;default: _LL8B:(lexbuf->refill_buff)(
lexbuf);return Cyc_suck_line_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp3B8;const char*_tmp3B7;struct Cyc_Lexing_Error_struct*_tmp3B6;(int)_throw((
void*)((_tmp3B6=_cycalloc(sizeof(*_tmp3B6)),((_tmp3B6[0]=((_tmp3B8.tag=Cyc_Lexing_Error,((
_tmp3B8.f1=((_tmp3B7="some action didn't return!",_tag_dynforward(_tmp3B7,
sizeof(char),_get_zero_arr_size_char(_tmp3B7,27)))),_tmp3B8)))),_tmp3B6)))));}}
struct _tuple13*Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf);struct _tuple13*Cyc_suck_line(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(lexbuf,10);}int Cyc_suck_macroname_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL8D:{struct _dynforward_ptr*_tmp3B9;Cyc_current_source=((_tmp3B9=
_cycalloc(sizeof(*_tmp3B9)),((_tmp3B9[0]=(struct _dynforward_ptr)Cyc_Lexing_lexeme(
lexbuf),_tmp3B9))));}Cyc_current_line=(struct _dynforward_ptr)Cyc_strconcat((
struct _dynforward_ptr)Cyc_current_line,(struct _dynforward_ptr)*((struct
_dynforward_ptr*)_check_null(Cyc_current_source)));return Cyc_suck_restofline(
lexbuf);default: _LL8E:(lexbuf->refill_buff)(lexbuf);return Cyc_suck_macroname_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp3BF;const char*_tmp3BE;struct
Cyc_Lexing_Error_struct*_tmp3BD;(int)_throw((void*)((_tmp3BD=_cycalloc(sizeof(*
_tmp3BD)),((_tmp3BD[0]=((_tmp3BF.tag=Cyc_Lexing_Error,((_tmp3BF.f1=((_tmp3BE="some action didn't return!",
_tag_dynforward(_tmp3BE,sizeof(char),_get_zero_arr_size_char(_tmp3BE,27)))),
_tmp3BF)))),_tmp3BD)))));}}int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf);
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(
lexbuf,11);}int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate);int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LL90: Cyc_current_line=(
struct _dynforward_ptr)Cyc_strconcat((struct _dynforward_ptr)Cyc_current_line,(
struct _dynforward_ptr)Cyc_Lexing_lexeme(lexbuf));return 0;default: _LL91:(lexbuf->refill_buff)(
lexbuf);return Cyc_suck_restofline_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp3C5;const char*_tmp3C4;struct Cyc_Lexing_Error_struct*_tmp3C3;(int)_throw((
void*)((_tmp3C3=_cycalloc(sizeof(*_tmp3C3)),((_tmp3C3[0]=((_tmp3C5.tag=Cyc_Lexing_Error,((
_tmp3C5.f1=((_tmp3C4="some action didn't return!",_tag_dynforward(_tmp3C4,
sizeof(char),_get_zero_arr_size_char(_tmp3C4,27)))),_tmp3C5)))),_tmp3C3)))));}}
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_suck_restofline(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(lexbuf,12);}struct
_tuple15*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);struct _tuple15*
Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=Cyc_lex_engine(
lexstate,lexbuf);switch(lexstate){case 0: _LL93: return Cyc_spec(lexbuf);case 1: _LL94:
Cyc_current_headerfile=(struct _dynforward_ptr)Cyc_substring((struct
_dynforward_ptr)Cyc_Lexing_lexeme(lexbuf),0,(unsigned long)((Cyc_Lexing_lexeme_end(
lexbuf)- Cyc_Lexing_lexeme_start(lexbuf))- 1));Cyc_current_symbols=0;Cyc_current_omit_symbols=
0;Cyc_current_cstubs=0;Cyc_current_cycstubs=0;Cyc_current_hstubs=0;while(Cyc_commands(
lexbuf)){;}Cyc_current_hstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_current_hstubs);Cyc_current_cstubs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cstubs);Cyc_current_cycstubs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cycstubs);{
struct _tuple15*_tmp3C6;return(_tmp3C6=_cycalloc(sizeof(*_tmp3C6)),((_tmp3C6->f1=
Cyc_current_headerfile,((_tmp3C6->f2=Cyc_current_symbols,((_tmp3C6->f3=Cyc_current_omit_symbols,((
_tmp3C6->f4=Cyc_current_hstubs,((_tmp3C6->f5=Cyc_current_cstubs,((_tmp3C6->f6=
Cyc_current_cycstubs,_tmp3C6)))))))))))));}case 2: _LL95: return Cyc_spec(lexbuf);
case 3: _LL96: return 0;case 4: _LL97:{const char*_tmp3CA;void*_tmp3C9[1];struct Cyc_Int_pa_struct
_tmp3C8;(_tmp3C8.tag=1,((_tmp3C8.f1=(unsigned long)((int)Cyc_Lexing_lexeme_char(
lexbuf,0)),((_tmp3C9[0]=& _tmp3C8,Cyc_fprintf(Cyc_stderr,((_tmp3CA="Error in .cys file: expected header file name, found '%c' instead\n",
_tag_dynforward(_tmp3CA,sizeof(char),_get_zero_arr_size_char(_tmp3CA,67)))),
_tag_dynforward(_tmp3C9,sizeof(void*),1)))))));}return 0;default: _LL98:(lexbuf->refill_buff)(
lexbuf);return Cyc_spec_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp3D0;const char*_tmp3CF;struct Cyc_Lexing_Error_struct*_tmp3CE;(int)_throw((
void*)((_tmp3CE=_cycalloc(sizeof(*_tmp3CE)),((_tmp3CE[0]=((_tmp3D0.tag=Cyc_Lexing_Error,((
_tmp3D0.f1=((_tmp3CF="some action didn't return!",_tag_dynforward(_tmp3CF,
sizeof(char),_get_zero_arr_size_char(_tmp3CF,27)))),_tmp3D0)))),_tmp3CE)))));}}
struct _tuple15*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf);struct _tuple15*Cyc_spec(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(lexbuf,13);}int Cyc_commands_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LL9A: return 0;case 1: _LL9B: return 0;case 2: _LL9C: Cyc_snarfed_symbols=0;while(
Cyc_snarfsymbols(lexbuf)){;}Cyc_current_symbols=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_symbols);
return 1;case 3: _LL9D: Cyc_snarfed_symbols=0;while(Cyc_snarfsymbols(lexbuf)){;}Cyc_current_omit_symbols=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
Cyc_snarfed_symbols,Cyc_current_omit_symbols);return 1;case 4: _LL9E: Cyc_braces_to_match=
1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{
struct _tuple14*_tmp3D1;struct _tuple14*x=(_tmp3D1=_cycalloc(sizeof(*_tmp3D1)),((
_tmp3D1->f1=(struct _dynforward_ptr)_tag_dynforward(0,0,0),((_tmp3D1->f2=(struct
_dynforward_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),
_tmp3D1)))));{struct Cyc_List_List*_tmp3D2;Cyc_current_hstubs=((_tmp3D2=_cycalloc(
sizeof(*_tmp3D2)),((_tmp3D2->hd=x,((_tmp3D2->tl=Cyc_current_hstubs,_tmp3D2))))));}
return 1;}case 5: _LL9F: {struct _dynforward_ptr _tmp72=Cyc_Lexing_lexeme(lexbuf);{
const char*_tmp3D3;_dynforward_ptr_inplace_plus(& _tmp72,sizeof(char),(int)Cyc_strlen(((
_tmp3D3="hstub",_tag_dynforward(_tmp3D3,sizeof(char),_get_zero_arr_size_char(
_tmp3D3,6))))));}while(isspace((int)*((char*)_check_dynforward_subscript(_tmp72,
sizeof(char),0)))){_dynforward_ptr_inplace_plus(& _tmp72,sizeof(char),1);}{struct
_dynforward_ptr t=_tmp72;while(!isspace((int)*((char*)_check_dynforward_subscript(
t,sizeof(char),0)))){_dynforward_ptr_inplace_plus(& t,sizeof(char),1);}{struct
_dynforward_ptr _tmp74=Cyc_substring((struct _dynforward_ptr)_tmp72,0,(
unsigned long)((t.curr - _tmp72.curr)/ sizeof(char)));Cyc_braces_to_match=1;Cyc_specbuf=(
struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct
_tuple14*_tmp3D4;struct _tuple14*x=(_tmp3D4=_cycalloc(sizeof(*_tmp3D4)),((_tmp3D4->f1=(
struct _dynforward_ptr)_tmp74,((_tmp3D4->f2=(struct _dynforward_ptr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp3D4)))));{struct Cyc_List_List*
_tmp3D5;Cyc_current_hstubs=((_tmp3D5=_cycalloc(sizeof(*_tmp3D5)),((_tmp3D5->hd=x,((
_tmp3D5->tl=Cyc_current_hstubs,_tmp3D5))))));}return 1;}}}}case 6: _LLA0: Cyc_braces_to_match=
1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{
struct _tuple14*_tmp3D6;struct _tuple14*x=(_tmp3D6=_cycalloc(sizeof(*_tmp3D6)),((
_tmp3D6->f1=(struct _dynforward_ptr)_tag_dynforward(0,0,0),((_tmp3D6->f2=(struct
_dynforward_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),
_tmp3D6)))));{struct Cyc_List_List*_tmp3D7;Cyc_current_cstubs=((_tmp3D7=_cycalloc(
sizeof(*_tmp3D7)),((_tmp3D7->hd=x,((_tmp3D7->tl=Cyc_current_cstubs,_tmp3D7))))));}
return 1;}case 7: _LLA1: {struct _dynforward_ptr _tmp79=Cyc_Lexing_lexeme(lexbuf);{
const char*_tmp3D8;_dynforward_ptr_inplace_plus(& _tmp79,sizeof(char),(int)Cyc_strlen(((
_tmp3D8="cstub",_tag_dynforward(_tmp3D8,sizeof(char),_get_zero_arr_size_char(
_tmp3D8,6))))));}while(isspace((int)*((char*)_check_dynforward_subscript(_tmp79,
sizeof(char),0)))){_dynforward_ptr_inplace_plus(& _tmp79,sizeof(char),1);}{struct
_dynforward_ptr t=_tmp79;while(!isspace((int)*((char*)_check_dynforward_subscript(
t,sizeof(char),0)))){_dynforward_ptr_inplace_plus(& t,sizeof(char),1);}{struct
_dynforward_ptr _tmp7B=Cyc_substring((struct _dynforward_ptr)_tmp79,0,(
unsigned long)((t.curr - _tmp79.curr)/ sizeof(char)));Cyc_braces_to_match=1;Cyc_specbuf=(
struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct
_tuple14*_tmp3D9;struct _tuple14*x=(_tmp3D9=_cycalloc(sizeof(*_tmp3D9)),((_tmp3D9->f1=(
struct _dynforward_ptr)_tmp7B,((_tmp3D9->f2=(struct _dynforward_ptr)Cyc_Buffer_contents((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),_tmp3D9)))));{struct Cyc_List_List*
_tmp3DA;Cyc_current_cstubs=((_tmp3DA=_cycalloc(sizeof(*_tmp3DA)),((_tmp3DA->hd=x,((
_tmp3DA->tl=Cyc_current_cstubs,_tmp3DA))))));}return 1;}}}}case 8: _LLA2: Cyc_braces_to_match=
1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{
struct _tuple14*_tmp3DB;struct _tuple14*x=(_tmp3DB=_cycalloc(sizeof(*_tmp3DB)),((
_tmp3DB->f1=(struct _dynforward_ptr)_tag_dynforward(0,0,0),((_tmp3DB->f2=(struct
_dynforward_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf)),
_tmp3DB)))));{struct Cyc_List_List*_tmp3DC;Cyc_current_cycstubs=((_tmp3DC=
_cycalloc(sizeof(*_tmp3DC)),((_tmp3DC->hd=x,((_tmp3DC->tl=Cyc_current_cycstubs,
_tmp3DC))))));}return 1;}case 9: _LLA3: {struct _dynforward_ptr _tmp80=Cyc_Lexing_lexeme(
lexbuf);{const char*_tmp3DD;_dynforward_ptr_inplace_plus(& _tmp80,sizeof(char),(
int)Cyc_strlen(((_tmp3DD="cycstub",_tag_dynforward(_tmp3DD,sizeof(char),
_get_zero_arr_size_char(_tmp3DD,8))))));}while(isspace((int)*((char*)
_check_dynforward_subscript(_tmp80,sizeof(char),0)))){
_dynforward_ptr_inplace_plus(& _tmp80,sizeof(char),1);}{struct _dynforward_ptr t=
_tmp80;while(!isspace((int)*((char*)_check_dynforward_subscript(t,sizeof(char),0)))){
_dynforward_ptr_inplace_plus(& t,sizeof(char),1);}{struct _dynforward_ptr _tmp82=
Cyc_substring((struct _dynforward_ptr)_tmp80,0,(unsigned long)((t.curr - _tmp80.curr)
/ sizeof(char)));Cyc_braces_to_match=1;Cyc_specbuf=(struct Cyc_Buffer_t*)Cyc_Buffer_create(
255);while(Cyc_block(lexbuf)){;}{struct _tuple14*_tmp3DE;struct _tuple14*x=(
_tmp3DE=_cycalloc(sizeof(*_tmp3DE)),((_tmp3DE->f1=(struct _dynforward_ptr)_tmp82,((
_tmp3DE->f2=(struct _dynforward_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf)),_tmp3DE)))));{struct Cyc_List_List*_tmp3DF;Cyc_current_cycstubs=((
_tmp3DF=_cycalloc(sizeof(*_tmp3DF)),((_tmp3DF->hd=x,((_tmp3DF->tl=Cyc_current_cycstubs,
_tmp3DF))))));}return 1;}}}}case 10: _LLA4: Cyc_braces_to_match=1;Cyc_specbuf=(
struct Cyc_Buffer_t*)Cyc_Buffer_create(255);while(Cyc_block(lexbuf)){;}{struct
_dynforward_ptr*_tmp3E0;struct _dynforward_ptr*x=(_tmp3E0=_cycalloc(sizeof(*
_tmp3E0)),((_tmp3E0[0]=(struct _dynforward_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf)),_tmp3E0)));{struct Cyc_List_List*_tmp3E1;Cyc_current_cpp=((
_tmp3E1=_cycalloc(sizeof(*_tmp3E1)),((_tmp3E1->hd=x,((_tmp3E1->tl=Cyc_current_cpp,
_tmp3E1))))));}return 1;}case 11: _LLA5: return 1;case 12: _LLA6: return 1;case 13: _LLA7:{
const char*_tmp3E5;void*_tmp3E4[1];struct Cyc_Int_pa_struct _tmp3E3;(_tmp3E3.tag=1,((
_tmp3E3.f1=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0)),((_tmp3E4[0]=&
_tmp3E3,Cyc_fprintf(Cyc_stderr,((_tmp3E5="Error in .cys file: expected command, found '%c' instead\n",
_tag_dynforward(_tmp3E5,sizeof(char),_get_zero_arr_size_char(_tmp3E5,58)))),
_tag_dynforward(_tmp3E4,sizeof(void*),1)))))));}return 0;default: _LLA8:(lexbuf->refill_buff)(
lexbuf);return Cyc_commands_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp3EB;const char*_tmp3EA;struct Cyc_Lexing_Error_struct*_tmp3E9;(int)_throw((
void*)((_tmp3E9=_cycalloc(sizeof(*_tmp3E9)),((_tmp3E9[0]=((_tmp3EB.tag=Cyc_Lexing_Error,((
_tmp3EB.f1=((_tmp3EA="some action didn't return!",_tag_dynforward(_tmp3EA,
sizeof(char),_get_zero_arr_size_char(_tmp3EA,27)))),_tmp3EB)))),_tmp3E9)))));}}
int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_commands(struct Cyc_Lexing_lexbuf*
lexbuf){return Cyc_commands_rec(lexbuf,14);}int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate);int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int
lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLAA:{
struct _dynforward_ptr*_tmp3EE;struct Cyc_List_List*_tmp3ED;Cyc_snarfed_symbols=((
_tmp3ED=_cycalloc(sizeof(*_tmp3ED)),((_tmp3ED->hd=((_tmp3EE=_cycalloc(sizeof(*
_tmp3EE)),((_tmp3EE[0]=(struct _dynforward_ptr)Cyc_Lexing_lexeme(lexbuf),_tmp3EE)))),((
_tmp3ED->tl=Cyc_snarfed_symbols,_tmp3ED))))));}return 1;case 1: _LLAB: return 1;case 2:
_LLAC: return 0;case 3: _LLAD:{const char*_tmp3F1;void*_tmp3F0;(_tmp3F0=0,Cyc_fprintf(
Cyc_stderr,((_tmp3F1="Error in .cys file: unexpected end-of-file\n",
_tag_dynforward(_tmp3F1,sizeof(char),_get_zero_arr_size_char(_tmp3F1,44)))),
_tag_dynforward(_tmp3F0,sizeof(void*),0)));}return 0;case 4: _LLAE:{const char*
_tmp3F5;void*_tmp3F4[1];struct Cyc_Int_pa_struct _tmp3F3;(_tmp3F3.tag=1,((_tmp3F3.f1=(
unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0)),((_tmp3F4[0]=& _tmp3F3,Cyc_fprintf(
Cyc_stderr,((_tmp3F5="Error in .cys file: expected symbol, found '%c' instead\n",
_tag_dynforward(_tmp3F5,sizeof(char),_get_zero_arr_size_char(_tmp3F5,57)))),
_tag_dynforward(_tmp3F4,sizeof(void*),1)))))));}return 0;default: _LLAF:(lexbuf->refill_buff)(
lexbuf);return Cyc_snarfsymbols_rec(lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct
_tmp3FB;const char*_tmp3FA;struct Cyc_Lexing_Error_struct*_tmp3F9;(int)_throw((
void*)((_tmp3F9=_cycalloc(sizeof(*_tmp3F9)),((_tmp3F9[0]=((_tmp3FB.tag=Cyc_Lexing_Error,((
_tmp3FB.f1=((_tmp3FA="some action didn't return!",_tag_dynforward(_tmp3FA,
sizeof(char),_get_zero_arr_size_char(_tmp3FA,27)))),_tmp3FB)))),_tmp3F9)))));}}
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_snarfsymbols(struct
Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(lexbuf,15);}int Cyc_block_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_block_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LLB1:{const char*_tmp3FE;void*_tmp3FD;(_tmp3FD=0,Cyc_log(((_tmp3FE="Warning: unclosed brace\n",
_tag_dynforward(_tmp3FE,sizeof(char),_get_zero_arr_size_char(_tmp3FE,25)))),
_tag_dynforward(_tmp3FD,sizeof(void*),0)));}return 0;case 1: _LLB2: if(Cyc_braces_to_match
== 1)return 0;-- Cyc_braces_to_match;Cyc_Buffer_add_char((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),'}');return 1;case 2: _LLB3: ++ Cyc_braces_to_match;Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');return 1;case 3: _LLB4: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');while(Cyc_block_string(lexbuf)){;}
return 1;case 4: _LLB5:{const char*_tmp3FF;Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),((_tmp3FF="/*",_tag_dynforward(_tmp3FF,sizeof(char),
_get_zero_arr_size_char(_tmp3FF,3)))));}while(Cyc_block_comment(lexbuf)){;}
return 1;case 5: _LLB6: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _dynforward_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 6: _LLB7: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),Cyc_Lexing_lexeme_char(lexbuf,0));
return 1;default: _LLB8:(lexbuf->refill_buff)(lexbuf);return Cyc_block_rec(lexbuf,
lexstate);}{struct Cyc_Lexing_Error_struct _tmp405;const char*_tmp404;struct Cyc_Lexing_Error_struct*
_tmp403;(int)_throw((void*)((_tmp403=_cycalloc(sizeof(*_tmp403)),((_tmp403[0]=((
_tmp405.tag=Cyc_Lexing_Error,((_tmp405.f1=((_tmp404="some action didn't return!",
_tag_dynforward(_tmp404,sizeof(char),_get_zero_arr_size_char(_tmp404,27)))),
_tmp405)))),_tmp403)))));}}int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf);int Cyc_block(
struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_rec(lexbuf,16);}int Cyc_block_string_rec(
struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*
lexbuf,int lexstate){lexstate=Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){
case 0: _LLBA:{const char*_tmp408;void*_tmp407;(_tmp407=0,Cyc_log(((_tmp408="Warning: unclosed string\n",
_tag_dynforward(_tmp408,sizeof(char),_get_zero_arr_size_char(_tmp408,26)))),
_tag_dynforward(_tmp407,sizeof(void*),0)));}return 0;case 1: _LLBB: Cyc_Buffer_add_char((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');return 0;case 2: _LLBC:{const char*
_tmp40B;void*_tmp40A;(_tmp40A=0,Cyc_log(((_tmp40B="Warning: unclosed string\n",
_tag_dynforward(_tmp40B,sizeof(char),_get_zero_arr_size_char(_tmp40B,26)))),
_tag_dynforward(_tmp40A,sizeof(void*),0)));}Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _dynforward_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 3: _LLBD: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _dynforward_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 4: _LLBE: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dynforward_ptr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 5: _LLBF: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _dynforward_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 6: _LLC0: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(
struct _dynforward_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 7: _LLC1: Cyc_Buffer_add_string((
struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),(struct _dynforward_ptr)Cyc_Lexing_lexeme(
lexbuf));return 1;case 8: _LLC2: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _dynforward_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;default: _LLC3:(lexbuf->refill_buff)(lexbuf);return Cyc_block_string_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp411;const char*_tmp410;struct
Cyc_Lexing_Error_struct*_tmp40F;(int)_throw((void*)((_tmp40F=_cycalloc(sizeof(*
_tmp40F)),((_tmp40F[0]=((_tmp411.tag=Cyc_Lexing_Error,((_tmp411.f1=((_tmp410="some action didn't return!",
_tag_dynforward(_tmp410,sizeof(char),_get_zero_arr_size_char(_tmp410,27)))),
_tmp411)))),_tmp40F)))));}}int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf);
int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(
lexbuf,17);}int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate);
int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){lexstate=
Cyc_lex_engine(lexstate,lexbuf);switch(lexstate){case 0: _LLC5:{const char*_tmp414;
void*_tmp413;(_tmp413=0,Cyc_log(((_tmp414="Warning: unclosed comment\n",
_tag_dynforward(_tmp414,sizeof(char),_get_zero_arr_size_char(_tmp414,27)))),
_tag_dynforward(_tmp413,sizeof(void*),0)));}return 0;case 1: _LLC6:{const char*
_tmp415;Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),((
_tmp415="*/",_tag_dynforward(_tmp415,sizeof(char),_get_zero_arr_size_char(
_tmp415,3)))));}return 0;case 2: _LLC7: Cyc_Buffer_add_string((struct Cyc_Buffer_t*)
_check_null(Cyc_specbuf),(struct _dynforward_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;default: _LLC8:(lexbuf->refill_buff)(lexbuf);return Cyc_block_comment_rec(
lexbuf,lexstate);}{struct Cyc_Lexing_Error_struct _tmp41B;const char*_tmp41A;struct
Cyc_Lexing_Error_struct*_tmp419;(int)_throw((void*)((_tmp419=_cycalloc(sizeof(*
_tmp419)),((_tmp419[0]=((_tmp41B.tag=Cyc_Lexing_Error,((_tmp41B.f1=((_tmp41A="some action didn't return!",
_tag_dynforward(_tmp41A,sizeof(char),_get_zero_arr_size_char(_tmp41A,27)))),
_tmp41B)))),_tmp419)))));}}int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf);
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(
lexbuf,18);}void Cyc_scan_type(void*t);struct _tuple16{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};void Cyc_scan_exp(struct Cyc_Absyn_Exp*e);void Cyc_scan_exp(
struct Cyc_Absyn_Exp*e){void*_tmpAA=(void*)((struct Cyc_Absyn_Exp*)_check_null(e))->r;
struct _tuple0*_tmpAB;struct _tuple0 _tmpAC;struct _dynforward_ptr*_tmpAD;struct
_tuple0*_tmpAE;struct _tuple0 _tmpAF;struct _dynforward_ptr*_tmpB0;struct Cyc_List_List*
_tmpB1;struct Cyc_Absyn_Exp*_tmpB2;struct Cyc_Absyn_Exp*_tmpB3;struct Cyc_Absyn_Exp*
_tmpB4;struct Cyc_Absyn_Exp*_tmpB5;struct Cyc_Absyn_Exp*_tmpB6;struct Cyc_Absyn_Exp*
_tmpB7;struct Cyc_Absyn_Exp*_tmpB8;struct Cyc_Absyn_Exp*_tmpB9;struct Cyc_Absyn_Exp*
_tmpBA;struct Cyc_Absyn_Exp*_tmpBB;struct Cyc_Absyn_Exp*_tmpBC;struct Cyc_Absyn_Exp*
_tmpBD;struct Cyc_Absyn_Exp*_tmpBE;struct Cyc_Absyn_Exp*_tmpBF;struct Cyc_Absyn_Exp*
_tmpC0;struct Cyc_Absyn_Exp*_tmpC1;struct Cyc_Absyn_Exp*_tmpC2;struct Cyc_Absyn_Exp*
_tmpC3;struct Cyc_List_List*_tmpC4;struct Cyc_Absyn_Exp*_tmpC5;struct Cyc_List_List*
_tmpC6;void*_tmpC7;struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_Absyn_MallocInfo _tmpC9;
int _tmpCA;struct Cyc_Absyn_Exp*_tmpCB;void**_tmpCC;struct Cyc_Absyn_Exp*_tmpCD;
void*_tmpCE;struct Cyc_Absyn_Exp*_tmpCF;struct _dynforward_ptr*_tmpD0;struct Cyc_Absyn_Exp*
_tmpD1;struct _dynforward_ptr*_tmpD2;void*_tmpD3;void*_tmpD4;struct Cyc_List_List*
_tmpD5;_LLCB: if(*((int*)_tmpAA)!= 1)goto _LLCD;_tmpAB=((struct Cyc_Absyn_Var_e_struct*)
_tmpAA)->f1;_tmpAC=*_tmpAB;_tmpAD=_tmpAC.f2;_LLCC: _tmpB0=_tmpAD;goto _LLCE;_LLCD:
if(*((int*)_tmpAA)!= 2)goto _LLCF;_tmpAE=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmpAA)->f1;_tmpAF=*_tmpAE;_tmpB0=_tmpAF.f2;_LLCE: Cyc_add_target(_tmpB0);return;
_LLCF: if(*((int*)_tmpAA)!= 3)goto _LLD1;_tmpB1=((struct Cyc_Absyn_Primop_e_struct*)
_tmpAA)->f2;_LLD0: for(0;_tmpB1 != 0;_tmpB1=_tmpB1->tl){Cyc_scan_exp((struct Cyc_Absyn_Exp*)((
struct Cyc_Absyn_Exp*)_tmpB1->hd));}return;_LLD1: if(*((int*)_tmpAA)!= 25)goto
_LLD3;_tmpB2=((struct Cyc_Absyn_Subscript_e_struct*)_tmpAA)->f1;_tmpB3=((struct
Cyc_Absyn_Subscript_e_struct*)_tmpAA)->f2;_LLD2: _tmpB4=_tmpB2;_tmpB5=_tmpB3;goto
_LLD4;_LLD3: if(*((int*)_tmpAA)!= 9)goto _LLD5;_tmpB4=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpAA)->f1;_tmpB5=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpAA)->f2;_LLD4: _tmpB6=
_tmpB4;_tmpB7=_tmpB5;goto _LLD6;_LLD5: if(*((int*)_tmpAA)!= 4)goto _LLD7;_tmpB6=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmpAA)->f1;_tmpB7=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpAA)->f3;_LLD6: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpB6);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpB7);return;_LLD7: if(*((int*)_tmpAA)!= 22)goto _LLD9;
_tmpB8=((struct Cyc_Absyn_Deref_e_struct*)_tmpAA)->f1;_LLD8: _tmpB9=_tmpB8;goto
_LLDA;_LLD9: if(*((int*)_tmpAA)!= 19)goto _LLDB;_tmpB9=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmpAA)->f1;_LLDA: _tmpBA=_tmpB9;goto _LLDC;_LLDB: if(*((int*)_tmpAA)!= 16)goto
_LLDD;_tmpBA=((struct Cyc_Absyn_Address_e_struct*)_tmpAA)->f1;_LLDC: _tmpBB=_tmpBA;
goto _LLDE;_LLDD: if(*((int*)_tmpAA)!= 5)goto _LLDF;_tmpBB=((struct Cyc_Absyn_Increment_e_struct*)
_tmpAA)->f1;_LLDE: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpBB);return;_LLDF: if(*((
int*)_tmpAA)!= 6)goto _LLE1;_tmpBC=((struct Cyc_Absyn_Conditional_e_struct*)_tmpAA)->f1;
_tmpBD=((struct Cyc_Absyn_Conditional_e_struct*)_tmpAA)->f2;_tmpBE=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpAA)->f3;_LLE0: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpBC);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpBD);Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpBE);return;
_LLE1: if(*((int*)_tmpAA)!= 7)goto _LLE3;_tmpBF=((struct Cyc_Absyn_And_e_struct*)
_tmpAA)->f1;_tmpC0=((struct Cyc_Absyn_And_e_struct*)_tmpAA)->f2;_LLE2: _tmpC1=
_tmpBF;_tmpC2=_tmpC0;goto _LLE4;_LLE3: if(*((int*)_tmpAA)!= 8)goto _LLE5;_tmpC1=((
struct Cyc_Absyn_Or_e_struct*)_tmpAA)->f1;_tmpC2=((struct Cyc_Absyn_Or_e_struct*)
_tmpAA)->f2;_LLE4: Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpC1);Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpC2);return;_LLE5: if(*((int*)_tmpAA)!= 11)goto _LLE7;
_tmpC3=((struct Cyc_Absyn_FnCall_e_struct*)_tmpAA)->f1;_tmpC4=((struct Cyc_Absyn_FnCall_e_struct*)
_tmpAA)->f2;_LLE6: _tmpC5=_tmpC3;_tmpC6=_tmpC4;goto _LLE8;_LLE7: if(*((int*)_tmpAA)
!= 10)goto _LLE9;_tmpC5=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmpAA)->f1;
_tmpC6=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmpAA)->f2;_LLE8: Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpC5);for(0;_tmpC6 != 0;_tmpC6=_tmpC6->tl){Cyc_scan_exp((
struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Exp*)_tmpC6->hd));}return;_LLE9: if(*((int*)
_tmpAA)!= 15)goto _LLEB;_tmpC7=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpAA)->f1;
_tmpC8=((struct Cyc_Absyn_Cast_e_struct*)_tmpAA)->f2;_LLEA: Cyc_scan_type(_tmpC7);
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpC8);return;_LLEB: if(*((int*)_tmpAA)!= 35)
goto _LLED;_tmpC9=((struct Cyc_Absyn_Malloc_e_struct*)_tmpAA)->f1;_tmpCA=_tmpC9.is_calloc;
_tmpCB=_tmpC9.rgn;_tmpCC=_tmpC9.elt_type;_tmpCD=_tmpC9.num_elts;_LLEC: if(_tmpCB
!= 0)Cyc_scan_exp((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Exp*)_tmpCB));if(
_tmpCC != 0)Cyc_scan_type(*_tmpCC);Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpCD);
return;_LLED: if(*((int*)_tmpAA)!= 18)goto _LLEF;_tmpCE=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmpAA)->f1;_LLEE: Cyc_scan_type(_tmpCE);return;_LLEF: if(*((int*)_tmpAA)!= 23)
goto _LLF1;_tmpCF=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpAA)->f1;_tmpD0=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmpAA)->f2;_LLF0: _tmpD1=_tmpCF;_tmpD2=
_tmpD0;goto _LLF2;_LLF1: if(*((int*)_tmpAA)!= 24)goto _LLF3;_tmpD1=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpAA)->f1;_tmpD2=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpAA)->f2;_LLF2: Cyc_scan_exp((
struct Cyc_Absyn_Exp*)_tmpD1);Cyc_add_target(_tmpD2);return;_LLF3: if(*((int*)
_tmpAA)!= 20)goto _LLF5;_tmpD3=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpAA)->f1;
_tmpD4=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpAA)->f2;_LLF4: Cyc_scan_type(
_tmpD3);{void*_tmpD6=_tmpD4;struct _dynforward_ptr*_tmpD7;_LL11A: if(*((int*)
_tmpD6)!= 0)goto _LL11C;_tmpD7=((struct Cyc_Absyn_StructField_struct*)_tmpD6)->f1;
_LL11B: Cyc_add_target(_tmpD7);goto _LL119;_LL11C: if(*((int*)_tmpD6)!= 1)goto
_LL119;_LL11D: goto _LL119;_LL119:;}return;_LLF5: if(*((int*)_tmpAA)!= 0)goto _LLF7;
_LLF6: return;_LLF7: if(*((int*)_tmpAA)!= 37)goto _LLF9;_tmpD5=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmpAA)->f2;_LLF8: for(0;_tmpD5 != 0;_tmpD5=_tmpD5->tl){struct _tuple16 _tmpD9;
struct Cyc_Absyn_Exp*_tmpDA;struct _tuple16*_tmpD8=(struct _tuple16*)_tmpD5->hd;
_tmpD9=*_tmpD8;_tmpDA=_tmpD9.f2;Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmpDA);}
return;_LLF9: if(*((int*)_tmpAA)!= 36)goto _LLFB;_LLFA:{const char*_tmp41E;void*
_tmp41D;(_tmp41D=0,Cyc_fprintf(Cyc_stderr,((_tmp41E="Error: unexpected Swap_e\n",
_tag_dynforward(_tmp41E,sizeof(char),_get_zero_arr_size_char(_tmp41E,26)))),
_tag_dynforward(_tmp41D,sizeof(void*),0)));}exit(1);return;_LLFB: if(*((int*)
_tmpAA)!= 38)goto _LLFD;_LLFC:{const char*_tmp421;void*_tmp420;(_tmp420=0,Cyc_fprintf(
Cyc_stderr,((_tmp421="Error: unexpected Stmt_e\n",_tag_dynforward(_tmp421,
sizeof(char),_get_zero_arr_size_char(_tmp421,26)))),_tag_dynforward(_tmp420,
sizeof(void*),0)));}exit(1);return;_LLFD: if(*((int*)_tmpAA)!= 12)goto _LLFF;_LLFE:{
const char*_tmp424;void*_tmp423;(_tmp423=0,Cyc_fprintf(Cyc_stderr,((_tmp424="Error: unexpected Throw_e\n",
_tag_dynforward(_tmp424,sizeof(char),_get_zero_arr_size_char(_tmp424,27)))),
_tag_dynforward(_tmp423,sizeof(void*),0)));}exit(1);return;_LLFF: if(*((int*)
_tmpAA)!= 13)goto _LL101;_LL100:{const char*_tmp427;void*_tmp426;(_tmp426=0,Cyc_fprintf(
Cyc_stderr,((_tmp427="Error: unexpected NoInstantiate_e\n",_tag_dynforward(
_tmp427,sizeof(char),_get_zero_arr_size_char(_tmp427,35)))),_tag_dynforward(
_tmp426,sizeof(void*),0)));}exit(1);return;_LL101: if(*((int*)_tmpAA)!= 14)goto
_LL103;_LL102:{const char*_tmp42A;void*_tmp429;(_tmp429=0,Cyc_fprintf(Cyc_stderr,((
_tmp42A="Error: unexpected Instantiate_e\n",_tag_dynforward(_tmp42A,sizeof(char),
_get_zero_arr_size_char(_tmp42A,33)))),_tag_dynforward(_tmp429,sizeof(void*),0)));}
exit(1);return;_LL103: if(*((int*)_tmpAA)!= 17)goto _LL105;_LL104:{const char*
_tmp42D;void*_tmp42C;(_tmp42C=0,Cyc_fprintf(Cyc_stderr,((_tmp42D="Error: unexpected New_e\n",
_tag_dynforward(_tmp42D,sizeof(char),_get_zero_arr_size_char(_tmp42D,25)))),
_tag_dynforward(_tmp42C,sizeof(void*),0)));}exit(1);return;_LL105: if(*((int*)
_tmpAA)!= 21)goto _LL107;_LL106:{const char*_tmp430;void*_tmp42F;(_tmp42F=0,Cyc_fprintf(
Cyc_stderr,((_tmp430="Error: unexpected Gentyp_e\n",_tag_dynforward(_tmp430,
sizeof(char),_get_zero_arr_size_char(_tmp430,28)))),_tag_dynforward(_tmp42F,
sizeof(void*),0)));}exit(1);return;_LL107: if(*((int*)_tmpAA)!= 26)goto _LL109;
_LL108:{const char*_tmp433;void*_tmp432;(_tmp432=0,Cyc_fprintf(Cyc_stderr,((
_tmp433="Error: unexpected Tuple_e\n",_tag_dynforward(_tmp433,sizeof(char),
_get_zero_arr_size_char(_tmp433,27)))),_tag_dynforward(_tmp432,sizeof(void*),0)));}
exit(1);return;_LL109: if(*((int*)_tmpAA)!= 27)goto _LL10B;_LL10A:{const char*
_tmp436;void*_tmp435;(_tmp435=0,Cyc_fprintf(Cyc_stderr,((_tmp436="Error: unexpected CompoundLit_e\n",
_tag_dynforward(_tmp436,sizeof(char),_get_zero_arr_size_char(_tmp436,33)))),
_tag_dynforward(_tmp435,sizeof(void*),0)));}exit(1);return;_LL10B: if(*((int*)
_tmpAA)!= 28)goto _LL10D;_LL10C:{const char*_tmp439;void*_tmp438;(_tmp438=0,Cyc_fprintf(
Cyc_stderr,((_tmp439="Error: unexpected Array_e\n",_tag_dynforward(_tmp439,
sizeof(char),_get_zero_arr_size_char(_tmp439,27)))),_tag_dynforward(_tmp438,
sizeof(void*),0)));}exit(1);return;_LL10D: if(*((int*)_tmpAA)!= 29)goto _LL10F;
_LL10E:{const char*_tmp43C;void*_tmp43B;(_tmp43B=0,Cyc_fprintf(Cyc_stderr,((
_tmp43C="Error: unexpected Comprehension_e\n",_tag_dynforward(_tmp43C,sizeof(
char),_get_zero_arr_size_char(_tmp43C,35)))),_tag_dynforward(_tmp43B,sizeof(void*),
0)));}exit(1);return;_LL10F: if(*((int*)_tmpAA)!= 30)goto _LL111;_LL110:{const char*
_tmp43F;void*_tmp43E;(_tmp43E=0,Cyc_fprintf(Cyc_stderr,((_tmp43F="Error: unexpected Struct_e\n",
_tag_dynforward(_tmp43F,sizeof(char),_get_zero_arr_size_char(_tmp43F,28)))),
_tag_dynforward(_tmp43E,sizeof(void*),0)));}exit(1);return;_LL111: if(*((int*)
_tmpAA)!= 31)goto _LL113;_LL112:{const char*_tmp442;void*_tmp441;(_tmp441=0,Cyc_fprintf(
Cyc_stderr,((_tmp442="Error: unexpected AnonStruct_e\n",_tag_dynforward(_tmp442,
sizeof(char),_get_zero_arr_size_char(_tmp442,32)))),_tag_dynforward(_tmp441,
sizeof(void*),0)));}exit(1);return;_LL113: if(*((int*)_tmpAA)!= 32)goto _LL115;
_LL114:{const char*_tmp445;void*_tmp444;(_tmp444=0,Cyc_fprintf(Cyc_stderr,((
_tmp445="Error: unexpected Tunion_e\n",_tag_dynforward(_tmp445,sizeof(char),
_get_zero_arr_size_char(_tmp445,28)))),_tag_dynforward(_tmp444,sizeof(void*),0)));}
exit(1);return;_LL115: if(*((int*)_tmpAA)!= 33)goto _LL117;_LL116:{const char*
_tmp448;void*_tmp447;(_tmp447=0,Cyc_fprintf(Cyc_stderr,((_tmp448="Error: unexpected Enum_e\n",
_tag_dynforward(_tmp448,sizeof(char),_get_zero_arr_size_char(_tmp448,26)))),
_tag_dynforward(_tmp447,sizeof(void*),0)));}exit(1);return;_LL117: if(*((int*)
_tmpAA)!= 34)goto _LLCA;_LL118:{const char*_tmp44B;void*_tmp44A;(_tmp44A=0,Cyc_fprintf(
Cyc_stderr,((_tmp44B="Error: unexpected AnonEnum_e\n",_tag_dynforward(_tmp44B,
sizeof(char),_get_zero_arr_size_char(_tmp44B,30)))),_tag_dynforward(_tmp44A,
sizeof(void*),0)));}exit(1);return;_LLCA:;}void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*
eo);void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo){if((unsigned int)eo)Cyc_scan_exp(
eo);return;}void Cyc_scan_type(void*t);void Cyc_scan_type(void*t){void*_tmpFB=t;
struct Cyc_Absyn_PtrInfo _tmpFC;struct Cyc_Absyn_ArrayInfo _tmpFD;void*_tmpFE;struct
Cyc_Absyn_Exp*_tmpFF;struct Cyc_Absyn_Conref*_tmp100;struct Cyc_Absyn_FnInfo
_tmp101;struct Cyc_List_List*_tmp102;struct Cyc_Absyn_AggrInfo _tmp103;union Cyc_Absyn_AggrInfoU_union
_tmp104;struct _tuple0*_tmp105;struct _tuple0 _tmp106;struct _dynforward_ptr*_tmp107;
struct _tuple0*_tmp108;struct _tuple0 _tmp109;struct _dynforward_ptr*_tmp10A;_LL11F:
if((int)_tmpFB != 0)goto _LL121;_LL120: goto _LL122;_LL121: if(_tmpFB <= (void*)4)goto
_LL123;if(*((int*)_tmpFB)!= 5)goto _LL123;_LL122: goto _LL124;_LL123: if((int)_tmpFB
!= 1)goto _LL125;_LL124: goto _LL126;_LL125: if(_tmpFB <= (void*)4)goto _LL145;if(*((
int*)_tmpFB)!= 6)goto _LL127;_LL126: return;_LL127: if(*((int*)_tmpFB)!= 4)goto
_LL129;_tmpFC=((struct Cyc_Absyn_PointerType_struct*)_tmpFB)->f1;_LL128: Cyc_scan_type((
void*)_tmpFC.elt_typ);return;_LL129: if(*((int*)_tmpFB)!= 7)goto _LL12B;_tmpFD=((
struct Cyc_Absyn_ArrayType_struct*)_tmpFB)->f1;_tmpFE=(void*)_tmpFD.elt_type;
_tmpFF=_tmpFD.num_elts;_tmp100=_tmpFD.zero_term;_LL12A: Cyc_scan_type(_tmpFE);Cyc_scan_exp_opt(
_tmpFF);return;_LL12B: if(*((int*)_tmpFB)!= 8)goto _LL12D;_tmp101=((struct Cyc_Absyn_FnType_struct*)
_tmpFB)->f1;_LL12C: Cyc_scan_type((void*)_tmp101.ret_typ);{struct Cyc_List_List*
_tmp10B=_tmp101.args;for(0;_tmp10B != 0;_tmp10B=_tmp10B->tl){struct _tuple1 _tmp10D;
void*_tmp10E;struct _tuple1*_tmp10C=(struct _tuple1*)_tmp10B->hd;_tmp10D=*_tmp10C;
_tmp10E=_tmp10D.f3;Cyc_scan_type(_tmp10E);}}if(_tmp101.cyc_varargs != 0)Cyc_scan_type((
void*)(_tmp101.cyc_varargs)->type);return;_LL12D: if(*((int*)_tmpFB)!= 11)goto
_LL12F;_tmp102=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpFB)->f2;_LL12E: for(0;
_tmp102 != 0;_tmp102=_tmp102->tl){Cyc_scan_type((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp102->hd)->type);Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)_tmp102->hd)->width);}
return;_LL12F: if(*((int*)_tmpFB)!= 13)goto _LL131;_LL130: return;_LL131: if(*((int*)
_tmpFB)!= 10)goto _LL133;_tmp103=((struct Cyc_Absyn_AggrType_struct*)_tmpFB)->f1;
_tmp104=_tmp103.aggr_info;_LL132: {struct _tuple0*_tmp110;struct _tuple0 _tmp111;
struct _dynforward_ptr*_tmp112;struct _tuple3 _tmp10F=Cyc_Absyn_aggr_kinded_name(
_tmp104);_tmp110=_tmp10F.f2;_tmp111=*_tmp110;_tmp112=_tmp111.f2;_tmp107=_tmp112;
goto _LL134;}_LL133: if(*((int*)_tmpFB)!= 12)goto _LL135;_tmp105=((struct Cyc_Absyn_EnumType_struct*)
_tmpFB)->f1;_tmp106=*_tmp105;_tmp107=_tmp106.f2;_LL134: _tmp10A=_tmp107;goto
_LL136;_LL135: if(*((int*)_tmpFB)!= 17)goto _LL137;_tmp108=((struct Cyc_Absyn_TypedefType_struct*)
_tmpFB)->f1;_tmp109=*_tmp108;_tmp10A=_tmp109.f2;_LL136: Cyc_add_target(_tmp10A);
return;_LL137: if(*((int*)_tmpFB)!= 0)goto _LL139;_LL138:{const char*_tmp44E;void*
_tmp44D;(_tmp44D=0,Cyc_fprintf(Cyc_stderr,((_tmp44E="Error: unexpected Evar\n",
_tag_dynforward(_tmp44E,sizeof(char),_get_zero_arr_size_char(_tmp44E,24)))),
_tag_dynforward(_tmp44D,sizeof(void*),0)));}exit(1);return;_LL139: if(*((int*)
_tmpFB)!= 1)goto _LL13B;_LL13A:{const char*_tmp451;void*_tmp450;(_tmp450=0,Cyc_fprintf(
Cyc_stderr,((_tmp451="Error: unexpected VarType\n",_tag_dynforward(_tmp451,
sizeof(char),_get_zero_arr_size_char(_tmp451,27)))),_tag_dynforward(_tmp450,
sizeof(void*),0)));}exit(1);return;_LL13B: if(*((int*)_tmpFB)!= 2)goto _LL13D;
_LL13C:{const char*_tmp454;void*_tmp453;(_tmp453=0,Cyc_fprintf(Cyc_stderr,((
_tmp454="Error: unexpected TunionType\n",_tag_dynforward(_tmp454,sizeof(char),
_get_zero_arr_size_char(_tmp454,30)))),_tag_dynforward(_tmp453,sizeof(void*),0)));}
exit(1);return;_LL13D: if(*((int*)_tmpFB)!= 3)goto _LL13F;_LL13E:{const char*
_tmp457;void*_tmp456;(_tmp456=0,Cyc_fprintf(Cyc_stderr,((_tmp457="Error: unexpected TunionFieldType\n",
_tag_dynforward(_tmp457,sizeof(char),_get_zero_arr_size_char(_tmp457,35)))),
_tag_dynforward(_tmp456,sizeof(void*),0)));}exit(1);return;_LL13F: if(*((int*)
_tmpFB)!= 9)goto _LL141;_LL140:{const char*_tmp45A;void*_tmp459;(_tmp459=0,Cyc_fprintf(
Cyc_stderr,((_tmp45A="Error: unexpected TupleType\n",_tag_dynforward(_tmp45A,
sizeof(char),_get_zero_arr_size_char(_tmp45A,29)))),_tag_dynforward(_tmp459,
sizeof(void*),0)));}exit(1);return;_LL141: if(*((int*)_tmpFB)!= 15)goto _LL143;
_LL142:{const char*_tmp45D;void*_tmp45C;(_tmp45C=0,Cyc_fprintf(Cyc_stderr,((
_tmp45D="Error: unexpected RgnHandleType\n",_tag_dynforward(_tmp45D,sizeof(char),
_get_zero_arr_size_char(_tmp45D,33)))),_tag_dynforward(_tmp45C,sizeof(void*),0)));}
exit(1);return;_LL143: if(*((int*)_tmpFB)!= 16)goto _LL145;_LL144:{const char*
_tmp460;void*_tmp45F;(_tmp45F=0,Cyc_fprintf(Cyc_stderr,((_tmp460="Error: unexpected DynRgnType\n",
_tag_dynforward(_tmp460,sizeof(char),_get_zero_arr_size_char(_tmp460,30)))),
_tag_dynforward(_tmp45F,sizeof(void*),0)));}exit(1);return;_LL145: if((int)_tmpFB
!= 2)goto _LL147;_LL146:{const char*_tmp463;void*_tmp462;(_tmp462=0,Cyc_fprintf(
Cyc_stderr,((_tmp463="Error: unexpected HeapRgn\n",_tag_dynforward(_tmp463,
sizeof(char),_get_zero_arr_size_char(_tmp463,27)))),_tag_dynforward(_tmp462,
sizeof(void*),0)));}exit(1);return;_LL147: if((int)_tmpFB != 3)goto _LL149;_LL148:{
const char*_tmp466;void*_tmp465;(_tmp465=0,Cyc_fprintf(Cyc_stderr,((_tmp466="Error: unexpected UniqueRgn\n",
_tag_dynforward(_tmp466,sizeof(char),_get_zero_arr_size_char(_tmp466,29)))),
_tag_dynforward(_tmp465,sizeof(void*),0)));}exit(1);return;_LL149: if(_tmpFB <= (
void*)4)goto _LL14B;if(*((int*)_tmpFB)!= 20)goto _LL14B;_LL14A:{const char*_tmp469;
void*_tmp468;(_tmp468=0,Cyc_fprintf(Cyc_stderr,((_tmp469="Error: unexpected AccessEff\n",
_tag_dynforward(_tmp469,sizeof(char),_get_zero_arr_size_char(_tmp469,29)))),
_tag_dynforward(_tmp468,sizeof(void*),0)));}exit(1);return;_LL14B: if(_tmpFB <= (
void*)4)goto _LL14D;if(*((int*)_tmpFB)!= 21)goto _LL14D;_LL14C:{const char*_tmp46C;
void*_tmp46B;(_tmp46B=0,Cyc_fprintf(Cyc_stderr,((_tmp46C="Error: unexpected JoinEff\n",
_tag_dynforward(_tmp46C,sizeof(char),_get_zero_arr_size_char(_tmp46C,27)))),
_tag_dynforward(_tmp46B,sizeof(void*),0)));}exit(1);return;_LL14D: if(_tmpFB <= (
void*)4)goto _LL14F;if(*((int*)_tmpFB)!= 22)goto _LL14F;_LL14E:{const char*_tmp46F;
void*_tmp46E;(_tmp46E=0,Cyc_fprintf(Cyc_stderr,((_tmp46F="Error: unexpected RgnsEff\n",
_tag_dynforward(_tmp46F,sizeof(char),_get_zero_arr_size_char(_tmp46F,27)))),
_tag_dynforward(_tmp46E,sizeof(void*),0)));}exit(1);return;_LL14F: if(_tmpFB <= (
void*)4)goto _LL151;if(*((int*)_tmpFB)!= 14)goto _LL151;_LL150:{const char*_tmp472;
void*_tmp471;(_tmp471=0,Cyc_fprintf(Cyc_stderr,((_tmp472="Error: unexpected sizeof_t\n",
_tag_dynforward(_tmp472,sizeof(char),_get_zero_arr_size_char(_tmp472,28)))),
_tag_dynforward(_tmp471,sizeof(void*),0)));}exit(1);return;_LL151: if(_tmpFB <= (
void*)4)goto _LL153;if(*((int*)_tmpFB)!= 19)goto _LL153;_LL152:{const char*_tmp475;
void*_tmp474;(_tmp474=0,Cyc_fprintf(Cyc_stderr,((_tmp475="Error: unexpected type integer\n",
_tag_dynforward(_tmp475,sizeof(char),_get_zero_arr_size_char(_tmp475,32)))),
_tag_dynforward(_tmp474,sizeof(void*),0)));}exit(1);return;_LL153: if(_tmpFB <= (
void*)4)goto _LL11E;if(*((int*)_tmpFB)!= 18)goto _LL11E;_LL154:{const char*_tmp478;
void*_tmp477;(_tmp477=0,Cyc_fprintf(Cyc_stderr,((_tmp478="Error: unexpected tag_t\n",
_tag_dynforward(_tmp478,sizeof(char),_get_zero_arr_size_char(_tmp478,25)))),
_tag_dynforward(_tmp477,sizeof(void*),0)));}exit(1);return;_LL11E:;}struct
_tuple17{struct _dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct
_tuple12*Cyc_scan_decl(struct Cyc_Absyn_Decl*d);struct _tuple12*Cyc_scan_decl(
struct Cyc_Absyn_Decl*d){{struct Cyc_Set_Set**_tmp479;Cyc_current_targets=((
_tmp479=_cycalloc(sizeof(*_tmp479)),((_tmp479[0]=((struct Cyc_Set_Set*(*)(int(*
cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),
_tmp479))));}{void*_tmp132=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp133;struct Cyc_Absyn_Fndecl*
_tmp134;struct Cyc_Absyn_Aggrdecl*_tmp135;struct Cyc_Absyn_Enumdecl*_tmp136;struct
Cyc_Absyn_Typedefdecl*_tmp137;_LL156: if(_tmp132 <= (void*)2)goto _LL160;if(*((int*)
_tmp132)!= 0)goto _LL158;_tmp133=((struct Cyc_Absyn_Var_d_struct*)_tmp132)->f1;
_LL157: {struct _tuple0 _tmp139;struct _dynforward_ptr*_tmp13A;struct _tuple0*
_tmp138=_tmp133->name;_tmp139=*_tmp138;_tmp13A=_tmp139.f2;Cyc_current_source=(
struct _dynforward_ptr*)_tmp13A;Cyc_scan_type((void*)_tmp133->type);Cyc_scan_exp_opt(
_tmp133->initializer);goto _LL155;}_LL158: if(*((int*)_tmp132)!= 1)goto _LL15A;
_tmp134=((struct Cyc_Absyn_Fn_d_struct*)_tmp132)->f1;_LL159: {struct _tuple0
_tmp13C;struct _dynforward_ptr*_tmp13D;struct _tuple0*_tmp13B=_tmp134->name;
_tmp13C=*_tmp13B;_tmp13D=_tmp13C.f2;Cyc_current_source=(struct _dynforward_ptr*)
_tmp13D;Cyc_scan_type((void*)_tmp134->ret_type);{struct Cyc_List_List*_tmp13E=
_tmp134->args;for(0;_tmp13E != 0;_tmp13E=_tmp13E->tl){struct _tuple17 _tmp140;void*
_tmp141;struct _tuple17*_tmp13F=(struct _tuple17*)_tmp13E->hd;_tmp140=*_tmp13F;
_tmp141=_tmp140.f3;Cyc_scan_type(_tmp141);}}if(_tmp134->cyc_varargs != 0)Cyc_scan_type((
void*)((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp134->cyc_varargs))->type);
if(_tmp134->is_inline){const char*_tmp47C;void*_tmp47B;(_tmp47B=0,Cyc_fprintf(Cyc_stderr,((
_tmp47C="Warning: ignoring inline function\n",_tag_dynforward(_tmp47C,sizeof(
char),_get_zero_arr_size_char(_tmp47C,35)))),_tag_dynforward(_tmp47B,sizeof(void*),
0)));}goto _LL155;}_LL15A: if(*((int*)_tmp132)!= 4)goto _LL15C;_tmp135=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp132)->f1;_LL15B: {struct _tuple0 _tmp145;struct _dynforward_ptr*_tmp146;struct
_tuple0*_tmp144=_tmp135->name;_tmp145=*_tmp144;_tmp146=_tmp145.f2;Cyc_current_source=(
struct _dynforward_ptr*)_tmp146;if((unsigned int)_tmp135->impl){{struct Cyc_List_List*
_tmp147=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp135->impl))->fields;for(
0;_tmp147 != 0;_tmp147=_tmp147->tl){struct Cyc_Absyn_Aggrfield*_tmp148=(struct Cyc_Absyn_Aggrfield*)
_tmp147->hd;Cyc_scan_type((void*)_tmp148->type);Cyc_scan_exp_opt(_tmp148->width);}}{
struct Cyc_List_List*_tmp149=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp135->impl))->fields;
for(0;_tmp149 != 0;_tmp149=_tmp149->tl){;}}}goto _LL155;}_LL15C: if(*((int*)_tmp132)
!= 6)goto _LL15E;_tmp136=((struct Cyc_Absyn_Enum_d_struct*)_tmp132)->f1;_LL15D: {
struct _tuple0 _tmp14B;struct _dynforward_ptr*_tmp14C;struct _tuple0*_tmp14A=_tmp136->name;
_tmp14B=*_tmp14A;_tmp14C=_tmp14B.f2;Cyc_current_source=(struct _dynforward_ptr*)
_tmp14C;if((unsigned int)_tmp136->fields){{struct Cyc_List_List*_tmp14D=(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp136->fields))->v;for(0;
_tmp14D != 0;_tmp14D=_tmp14D->tl){struct Cyc_Absyn_Enumfield*_tmp14E=(struct Cyc_Absyn_Enumfield*)
_tmp14D->hd;Cyc_scan_exp_opt(_tmp14E->tag);}}{struct Cyc_List_List*_tmp14F=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp136->fields))->v;for(
0;_tmp14F != 0;_tmp14F=_tmp14F->tl){;}}}goto _LL155;}_LL15E: if(*((int*)_tmp132)!= 
7)goto _LL160;_tmp137=((struct Cyc_Absyn_Typedef_d_struct*)_tmp132)->f1;_LL15F: {
struct _tuple0 _tmp151;struct _dynforward_ptr*_tmp152;struct _tuple0*_tmp150=_tmp137->name;
_tmp151=*_tmp150;_tmp152=_tmp151.f2;Cyc_current_source=(struct _dynforward_ptr*)
_tmp152;if((unsigned int)_tmp137->defn)Cyc_scan_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp137->defn))->v);goto _LL155;}_LL160: if((int)_tmp132 != 0)goto
_LL162;_LL161:{const char*_tmp47F;void*_tmp47E;(_tmp47E=0,Cyc_fprintf(Cyc_stderr,((
_tmp47F="Error: unexpected __cyclone_port_on__",_tag_dynforward(_tmp47F,sizeof(
char),_get_zero_arr_size_char(_tmp47F,38)))),_tag_dynforward(_tmp47E,sizeof(void*),
0)));}exit(1);return 0;_LL162: if((int)_tmp132 != 1)goto _LL164;_LL163:{const char*
_tmp482;void*_tmp481;(_tmp481=0,Cyc_fprintf(Cyc_stderr,((_tmp482="Error: unexpected __cyclone_port_off__",
_tag_dynforward(_tmp482,sizeof(char),_get_zero_arr_size_char(_tmp482,39)))),
_tag_dynforward(_tmp481,sizeof(void*),0)));}exit(1);return 0;_LL164: if(_tmp132 <= (
void*)2)goto _LL166;if(*((int*)_tmp132)!= 2)goto _LL166;_LL165:{const char*_tmp485;
void*_tmp484;(_tmp484=0,Cyc_fprintf(Cyc_stderr,((_tmp485="Error: unexpected let declaration\n",
_tag_dynforward(_tmp485,sizeof(char),_get_zero_arr_size_char(_tmp485,35)))),
_tag_dynforward(_tmp484,sizeof(void*),0)));}exit(1);return 0;_LL166: if(_tmp132 <= (
void*)2)goto _LL168;if(*((int*)_tmp132)!= 5)goto _LL168;_LL167:{const char*_tmp488;
void*_tmp487;(_tmp487=0,Cyc_fprintf(Cyc_stderr,((_tmp488="Error: unexpected tunion declaration\n",
_tag_dynforward(_tmp488,sizeof(char),_get_zero_arr_size_char(_tmp488,38)))),
_tag_dynforward(_tmp487,sizeof(void*),0)));}exit(1);return 0;_LL168: if(_tmp132 <= (
void*)2)goto _LL16A;if(*((int*)_tmp132)!= 3)goto _LL16A;_LL169:{const char*_tmp48B;
void*_tmp48A;(_tmp48A=0,Cyc_fprintf(Cyc_stderr,((_tmp48B="Error: unexpected let declaration\n",
_tag_dynforward(_tmp48B,sizeof(char),_get_zero_arr_size_char(_tmp48B,35)))),
_tag_dynforward(_tmp48A,sizeof(void*),0)));}exit(1);return 0;_LL16A: if(_tmp132 <= (
void*)2)goto _LL16C;if(*((int*)_tmp132)!= 8)goto _LL16C;_LL16B:{const char*_tmp48E;
void*_tmp48D;(_tmp48D=0,Cyc_fprintf(Cyc_stderr,((_tmp48E="Error: unexpected namespace declaration\n",
_tag_dynforward(_tmp48E,sizeof(char),_get_zero_arr_size_char(_tmp48E,41)))),
_tag_dynforward(_tmp48D,sizeof(void*),0)));}exit(1);return 0;_LL16C: if(_tmp132 <= (
void*)2)goto _LL16E;if(*((int*)_tmp132)!= 9)goto _LL16E;_LL16D:{const char*_tmp491;
void*_tmp490;(_tmp490=0,Cyc_fprintf(Cyc_stderr,((_tmp491="Error: unexpected using declaration\n",
_tag_dynforward(_tmp491,sizeof(char),_get_zero_arr_size_char(_tmp491,37)))),
_tag_dynforward(_tmp490,sizeof(void*),0)));}exit(1);return 0;_LL16E: if(_tmp132 <= (
void*)2)goto _LL170;if(*((int*)_tmp132)!= 10)goto _LL170;_LL16F:{const char*_tmp494;
void*_tmp493;(_tmp493=0,Cyc_fprintf(Cyc_stderr,((_tmp494="Error: unexpected extern \"C\" declaration\n",
_tag_dynforward(_tmp494,sizeof(char),_get_zero_arr_size_char(_tmp494,42)))),
_tag_dynforward(_tmp493,sizeof(void*),0)));}exit(1);return 0;_LL170: if(_tmp132 <= (
void*)2)goto _LL155;if(*((int*)_tmp132)!= 11)goto _LL155;_LL171:{const char*_tmp497;
void*_tmp496;(_tmp496=0,Cyc_fprintf(Cyc_stderr,((_tmp497="Error: unexpected extern \"C include\" declaration\n",
_tag_dynforward(_tmp497,sizeof(char),_get_zero_arr_size_char(_tmp497,50)))),
_tag_dynforward(_tmp496,sizeof(void*),0)));}exit(1);return 0;_LL155:;}{struct
_tuple12*_tmp498;return(_tmp498=_cycalloc(sizeof(*_tmp498)),((_tmp498->f1=(
struct _dynforward_ptr*)_check_null(Cyc_current_source),((_tmp498->f2=*((struct
Cyc_Set_Set**)_check_null(Cyc_current_targets)),_tmp498)))));}}struct Cyc_Hashtable_Table*
Cyc_new_deps();struct Cyc_Hashtable_Table*Cyc_new_deps(){return((struct Cyc_Hashtable_Table*(*)(
int sz,int(*cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*),int(*hash)(struct
_dynforward_ptr*)))Cyc_Hashtable_create)(107,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}
struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*t,struct _dynforward_ptr*x);
struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*t,struct _dynforward_ptr*x){
struct _handler_cons _tmp166;_push_handler(& _tmp166);{int _tmp168=0;if(setjmp(
_tmp166.handler))_tmp168=1;if(!_tmp168){{struct Cyc_Set_Set*_tmp169=((struct Cyc_Set_Set*(*)(
struct Cyc_Hashtable_Table*t,struct _dynforward_ptr*key))Cyc_Hashtable_lookup)(t,x);
_npop_handler(0);return _tmp169;};_pop_handler();}else{void*_tmp167=(void*)
_exn_thrown;void*_tmp16B=_tmp167;_LL173: if(_tmp16B != Cyc_Core_Not_found)goto
_LL175;_LL174: return((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dynforward_ptr*,
struct _dynforward_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);_LL175:;_LL176:(void)
_throw(_tmp16B);_LL172:;}}}struct Cyc_Set_Set*Cyc_reachable(struct Cyc_List_List*
init,struct Cyc_Hashtable_Table*t);struct Cyc_Set_Set*Cyc_reachable(struct Cyc_List_List*
init,struct Cyc_Hashtable_Table*t){struct Cyc_Set_Set*emptyset=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
struct Cyc_Set_Set*curr;for(curr=emptyset;init != 0;init=init->tl){curr=((struct
Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dynforward_ptr*elt))Cyc_Set_insert)(
curr,(struct _dynforward_ptr*)init->hd);}{struct Cyc_Set_Set*_tmp16C=curr;struct
_dynforward_ptr*_tmp16D=_init_dynforward_ptr(_cycalloc(sizeof(struct
_dynforward_ptr)),"",sizeof(char),1);while(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(
_tmp16C)> 0){struct Cyc_Set_Set*_tmp16E=emptyset;struct Cyc_Iter_Iter _tmp16F=((
struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(
Cyc_Core_heap_region,_tmp16C);while(((int(*)(struct Cyc_Iter_Iter,struct
_dynforward_ptr**))Cyc_Iter_next)(_tmp16F,& _tmp16D)){_tmp16E=((struct Cyc_Set_Set*(*)(
struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmp16E,Cyc_find(t,
_tmp16D));}_tmp16C=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*
s2))Cyc_Set_diff)(_tmp16E,curr);curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*
s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(curr,_tmp16C);}return curr;}}static
void*Cyc_mode=(void*)0;static int Cyc_gathering();static int Cyc_gathering(){return
Cyc_mode == (void*)1  || Cyc_mode == (void*)2;}static struct Cyc___cycFILE*Cyc_script_file=
0;int Cyc_prscript(struct _dynforward_ptr fmt,struct _dynforward_ptr ap);int Cyc_prscript(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap){if(Cyc_script_file == 0){{const
char*_tmp49B;void*_tmp49A;(_tmp49A=0,Cyc_fprintf(Cyc_stderr,((_tmp49B="Internal error: script file is NULL\n",
_tag_dynforward(_tmp49B,sizeof(char),_get_zero_arr_size_char(_tmp49B,37)))),
_tag_dynforward(_tmp49A,sizeof(void*),0)));}exit(1);}return Cyc_vfprintf((struct
Cyc___cycFILE*)_check_null(Cyc_script_file),fmt,ap);}int Cyc_force_directory(
struct _dynforward_ptr d);int Cyc_force_directory(struct _dynforward_ptr d){if(Cyc_mode
== (void*)2){const char*_tmp4A0;void*_tmp49F[2];struct Cyc_String_pa_struct _tmp49E;
struct Cyc_String_pa_struct _tmp49D;(_tmp49D.tag=0,((_tmp49D.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)d),((_tmp49E.tag=0,((_tmp49E.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)d),((_tmp49F[0]=& _tmp49E,((_tmp49F[1]=&
_tmp49D,Cyc_prscript(((_tmp4A0="if ! test -e %s; then mkdir %s; fi\n",
_tag_dynforward(_tmp4A0,sizeof(char),_get_zero_arr_size_char(_tmp4A0,36)))),
_tag_dynforward(_tmp49F,sizeof(void*),2)))))))))))));}else{unsigned short _tmp4A1[
0];int _tmp177=Cyc_open((const char*)_check_null(_untag_dynforward_ptr(d,sizeof(
char),1)),0,_tag_dynforward(_tmp4A1,sizeof(unsigned short),0));if(_tmp177 == - 1){
if(mkdir((const char*)_untag_dynforward_ptr(d,sizeof(char),1),448)== - 1){{const
char*_tmp4A5;void*_tmp4A4[1];struct Cyc_String_pa_struct _tmp4A3;(_tmp4A3.tag=0,((
_tmp4A3.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)d),((_tmp4A4[0]=&
_tmp4A3,Cyc_fprintf(Cyc_stderr,((_tmp4A5="Error: could not create directory %s\n",
_tag_dynforward(_tmp4A5,sizeof(char),_get_zero_arr_size_char(_tmp4A5,38)))),
_tag_dynforward(_tmp4A4,sizeof(void*),1)))))));}return 1;}}else{close(_tmp177);}}
return 0;}int Cyc_force_directory_prefixes(struct _dynforward_ptr file);int Cyc_force_directory_prefixes(
struct _dynforward_ptr file){struct _dynforward_ptr _tmp17C=Cyc_strdup((struct
_dynforward_ptr)file);struct Cyc_List_List*_tmp17D=0;while(1){_tmp17C=Cyc_Filename_dirname((
struct _dynforward_ptr)_tmp17C);if(Cyc_strlen((struct _dynforward_ptr)_tmp17C)== 0)
break;{struct _dynforward_ptr*_tmp4A8;struct Cyc_List_List*_tmp4A7;_tmp17D=((
_tmp4A7=_cycalloc(sizeof(*_tmp4A7)),((_tmp4A7->hd=((_tmp4A8=_cycalloc(sizeof(*
_tmp4A8)),((_tmp4A8[0]=(struct _dynforward_ptr)_tmp17C,_tmp4A8)))),((_tmp4A7->tl=
_tmp17D,_tmp4A7))))));}}for(0;_tmp17D != 0;_tmp17D=_tmp17D->tl){if(Cyc_force_directory(*((
struct _dynforward_ptr*)_tmp17D->hd)))return 1;}return 0;}char Cyc_NO_SUPPORT[15]="\000\000\000\000NO_SUPPORT\000";
struct Cyc_NO_SUPPORT_struct{char*tag;struct _dynforward_ptr f1;};struct _tuple18{
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};int Cyc_process_file(const char*
filename,struct Cyc_List_List*start_symbols,struct Cyc_List_List*omit_symbols,
struct Cyc_List_List*hstubs,struct Cyc_List_List*cstubs,struct Cyc_List_List*
cycstubs);int Cyc_process_file(const char*filename,struct Cyc_List_List*
start_symbols,struct Cyc_List_List*omit_symbols,struct Cyc_List_List*hstubs,struct
Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs){struct Cyc___cycFILE*maybe;
struct Cyc___cycFILE*in_file;struct Cyc___cycFILE*out_file;int errorcode=0;{const
char*_tmp4AE;const char*_tmp4AD;void*_tmp4AC[1];struct Cyc_String_pa_struct _tmp4AB;(
_tmp4AB.tag=0,((_tmp4AB.f1=(struct _dynforward_ptr)((_tmp4AE=filename,
_tag_dynforward(_tmp4AE,sizeof(char),_get_zero_arr_size_char(_tmp4AE,1)))),((
_tmp4AC[0]=& _tmp4AB,Cyc_fprintf(Cyc_stderr,((_tmp4AD="********************************* %s...\n",
_tag_dynforward(_tmp4AD,sizeof(char),_get_zero_arr_size_char(_tmp4AD,41)))),
_tag_dynforward(_tmp4AC,sizeof(void*),1)))))));}if(!Cyc_gathering()){const char*
_tmp4B4;const char*_tmp4B3;void*_tmp4B2[1];struct Cyc_String_pa_struct _tmp4B1;(
_tmp4B1.tag=0,((_tmp4B1.f1=(struct _dynforward_ptr)((_tmp4B4=filename,
_tag_dynforward(_tmp4B4,sizeof(char),_get_zero_arr_size_char(_tmp4B4,1)))),((
_tmp4B2[0]=& _tmp4B1,Cyc_log(((_tmp4B3="\n%s:\n",_tag_dynforward(_tmp4B3,sizeof(
char),_get_zero_arr_size_char(_tmp4B3,6)))),_tag_dynforward(_tmp4B2,sizeof(void*),
1)))))));}{const char*_tmp4B5;struct _dynforward_ptr _tmp188=Cyc_Filename_basename(((
_tmp4B5=filename,_tag_dynforward(_tmp4B5,sizeof(char),_get_zero_arr_size_char(
_tmp4B5,1)))));const char*_tmp4B6;struct _dynforward_ptr _tmp189=Cyc_Filename_dirname(((
_tmp4B6=filename,_tag_dynforward(_tmp4B6,sizeof(char),_get_zero_arr_size_char(
_tmp4B6,1)))));struct _dynforward_ptr _tmp18A=Cyc_Filename_chop_extension((struct
_dynforward_ptr)_tmp188);const char*_tmp4B7;const char*_tmp18B=(const char*)
_untag_dynforward_ptr(Cyc_strconcat((struct _dynforward_ptr)_tmp18A,((_tmp4B7=".iA",
_tag_dynforward(_tmp4B7,sizeof(char),_get_zero_arr_size_char(_tmp4B7,4))))),
sizeof(char),1);const char*_tmp4BF;void*_tmp4BE[1];struct Cyc_String_pa_struct
_tmp4BD;const char*_tmp4BB;void*_tmp4BA[1];struct Cyc_String_pa_struct _tmp4B9;
const char*_tmp18C=(const char*)_untag_dynforward_ptr(_get_dynforward_size(_tmp189,
sizeof(char))== 0?(_tmp4BD.tag=0,((_tmp4BD.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)_tmp18A),((_tmp4BE[0]=& _tmp4BD,Cyc_aprintf(((_tmp4BF="%s.iB",
_tag_dynforward(_tmp4BF,sizeof(char),_get_zero_arr_size_char(_tmp4BF,6)))),
_tag_dynforward(_tmp4BE,sizeof(void*),1))))))): Cyc_Filename_concat((struct
_dynforward_ptr)_tmp189,(struct _dynforward_ptr)((_tmp4B9.tag=0,((_tmp4B9.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmp18A),((_tmp4BA[0]=& _tmp4B9,Cyc_aprintf(((
_tmp4BB="%s.iB",_tag_dynforward(_tmp4BB,sizeof(char),_get_zero_arr_size_char(
_tmp4BB,6)))),_tag_dynforward(_tmp4BA,sizeof(void*),1))))))))),sizeof(char),1);
const char*_tmp4C7;void*_tmp4C6[1];struct Cyc_String_pa_struct _tmp4C5;const char*
_tmp4C3;void*_tmp4C2[1];struct Cyc_String_pa_struct _tmp4C1;const char*_tmp18D=(
const char*)_untag_dynforward_ptr(_get_dynforward_size(_tmp189,sizeof(char))== 0?(
_tmp4C5.tag=0,((_tmp4C5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp18A),((_tmp4C6[0]=& _tmp4C5,Cyc_aprintf(((_tmp4C7="%s.iC",_tag_dynforward(
_tmp4C7,sizeof(char),_get_zero_arr_size_char(_tmp4C7,6)))),_tag_dynforward(
_tmp4C6,sizeof(void*),1))))))): Cyc_Filename_concat((struct _dynforward_ptr)
_tmp189,(struct _dynforward_ptr)((_tmp4C1.tag=0,((_tmp4C1.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp18A),((_tmp4C2[0]=& _tmp4C1,Cyc_aprintf(((
_tmp4C3="%s.iC",_tag_dynforward(_tmp4C3,sizeof(char),_get_zero_arr_size_char(
_tmp4C3,6)))),_tag_dynforward(_tmp4C2,sizeof(void*),1))))))))),sizeof(char),1);
const char*_tmp4CF;void*_tmp4CE[1];struct Cyc_String_pa_struct _tmp4CD;const char*
_tmp4CB;void*_tmp4CA[1];struct Cyc_String_pa_struct _tmp4C9;const char*_tmp18E=(
const char*)_untag_dynforward_ptr(_get_dynforward_size(_tmp189,sizeof(char))== 0?(
_tmp4CD.tag=0,((_tmp4CD.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp18A),((_tmp4CE[0]=& _tmp4CD,Cyc_aprintf(((_tmp4CF="%s.iD",_tag_dynforward(
_tmp4CF,sizeof(char),_get_zero_arr_size_char(_tmp4CF,6)))),_tag_dynforward(
_tmp4CE,sizeof(void*),1))))))): Cyc_Filename_concat((struct _dynforward_ptr)
_tmp189,(struct _dynforward_ptr)((_tmp4C9.tag=0,((_tmp4C9.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp18A),((_tmp4CA[0]=& _tmp4C9,Cyc_aprintf(((
_tmp4CB="%s.iD",_tag_dynforward(_tmp4CB,sizeof(char),_get_zero_arr_size_char(
_tmp4CB,6)))),_tag_dynforward(_tmp4CA,sizeof(void*),1))))))))),sizeof(char),1);
struct _handler_cons _tmp18F;_push_handler(& _tmp18F);{int _tmp191=0;if(setjmp(
_tmp18F.handler))_tmp191=1;if(!_tmp191){{const char*_tmp4D0;if(Cyc_force_directory_prefixes(((
_tmp4D0=filename,_tag_dynforward(_tmp4D0,sizeof(char),_get_zero_arr_size_char(
_tmp4D0,1)))))){int _tmp193=1;_npop_handler(0);return _tmp193;}}if(Cyc_mode != (
void*)3){Cyc_current_cpp=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_current_cpp);if(Cyc_mode == (void*)2){{const char*_tmp4D6;const char*_tmp4D5;
void*_tmp4D4[1];struct Cyc_String_pa_struct _tmp4D3;(_tmp4D3.tag=0,((_tmp4D3.f1=(
struct _dynforward_ptr)((_tmp4D6=_tmp18B,_tag_dynforward(_tmp4D6,sizeof(char),
_get_zero_arr_size_char(_tmp4D6,1)))),((_tmp4D4[0]=& _tmp4D3,Cyc_prscript(((
_tmp4D5="cat >%s <<XXX\n",_tag_dynforward(_tmp4D5,sizeof(char),
_get_zero_arr_size_char(_tmp4D5,15)))),_tag_dynforward(_tmp4D4,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp198=Cyc_current_cpp;for(0;_tmp198 != 0;_tmp198=_tmp198->tl){
const char*_tmp4DA;void*_tmp4D9[1];struct Cyc_String_pa_struct _tmp4D8;(_tmp4D8.tag=
0,((_tmp4D8.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((struct
_dynforward_ptr*)_tmp198->hd)),((_tmp4D9[0]=& _tmp4D8,Cyc_prscript(((_tmp4DA="%s",
_tag_dynforward(_tmp4DA,sizeof(char),_get_zero_arr_size_char(_tmp4DA,3)))),
_tag_dynforward(_tmp4D9,sizeof(void*),1)))))));}}{const char*_tmp4E0;const char*
_tmp4DF;void*_tmp4DE[1];struct Cyc_String_pa_struct _tmp4DD;(_tmp4DD.tag=0,((
_tmp4DD.f1=(struct _dynforward_ptr)((_tmp4E0=filename,_tag_dynforward(_tmp4E0,
sizeof(char),_get_zero_arr_size_char(_tmp4E0,1)))),((_tmp4DE[0]=& _tmp4DD,Cyc_prscript(((
_tmp4DF="#include <%s>\n",_tag_dynforward(_tmp4DF,sizeof(char),
_get_zero_arr_size_char(_tmp4DF,15)))),_tag_dynforward(_tmp4DE,sizeof(void*),1)))))));}{
const char*_tmp4E3;void*_tmp4E2;(_tmp4E2=0,Cyc_prscript(((_tmp4E3="XXX\n",
_tag_dynforward(_tmp4E3,sizeof(char),_get_zero_arr_size_char(_tmp4E3,5)))),
_tag_dynforward(_tmp4E2,sizeof(void*),0)));}{const char*_tmp4EC;const char*_tmp4EB;
const char*_tmp4EA;void*_tmp4E9[2];struct Cyc_String_pa_struct _tmp4E8;struct Cyc_String_pa_struct
_tmp4E7;(_tmp4E7.tag=0,((_tmp4E7.f1=(struct _dynforward_ptr)((_tmp4EC=_tmp18B,
_tag_dynforward(_tmp4EC,sizeof(char),_get_zero_arr_size_char(_tmp4EC,1)))),((
_tmp4E8.tag=0,((_tmp4E8.f1=(struct _dynforward_ptr)((_tmp4EB=_tmp18C,
_tag_dynforward(_tmp4EB,sizeof(char),_get_zero_arr_size_char(_tmp4EB,1)))),((
_tmp4E9[0]=& _tmp4E8,((_tmp4E9[1]=& _tmp4E7,Cyc_prscript(((_tmp4EA="$GCC -E -dM -o %s -x c %s && \\\n",
_tag_dynforward(_tmp4EA,sizeof(char),_get_zero_arr_size_char(_tmp4EA,32)))),
_tag_dynforward(_tmp4E9,sizeof(void*),2)))))))))))));}{const char*_tmp4F5;const
char*_tmp4F4;const char*_tmp4F3;void*_tmp4F2[2];struct Cyc_String_pa_struct _tmp4F1;
struct Cyc_String_pa_struct _tmp4F0;(_tmp4F0.tag=0,((_tmp4F0.f1=(struct
_dynforward_ptr)((_tmp4F5=_tmp18B,_tag_dynforward(_tmp4F5,sizeof(char),
_get_zero_arr_size_char(_tmp4F5,1)))),((_tmp4F1.tag=0,((_tmp4F1.f1=(struct
_dynforward_ptr)((_tmp4F4=_tmp18D,_tag_dynforward(_tmp4F4,sizeof(char),
_get_zero_arr_size_char(_tmp4F4,1)))),((_tmp4F2[0]=& _tmp4F1,((_tmp4F2[1]=&
_tmp4F0,Cyc_prscript(((_tmp4F3="$GCC -E     -o %s -x c %s;\n",_tag_dynforward(
_tmp4F3,sizeof(char),_get_zero_arr_size_char(_tmp4F3,28)))),_tag_dynforward(
_tmp4F2,sizeof(void*),2)))))))))))));}{const char*_tmp4FB;const char*_tmp4FA;void*
_tmp4F9[1];struct Cyc_String_pa_struct _tmp4F8;(_tmp4F8.tag=0,((_tmp4F8.f1=(struct
_dynforward_ptr)((_tmp4FB=_tmp18B,_tag_dynforward(_tmp4FB,sizeof(char),
_get_zero_arr_size_char(_tmp4FB,1)))),((_tmp4F9[0]=& _tmp4F8,Cyc_prscript(((
_tmp4FA="rm %s\n",_tag_dynforward(_tmp4FA,sizeof(char),_get_zero_arr_size_char(
_tmp4FA,7)))),_tag_dynforward(_tmp4F9,sizeof(void*),1)))))));}}else{maybe=Cyc_fopen(
_tmp18B,(const char*)"w");if(!((unsigned int)maybe)){{const char*_tmp501;const char*
_tmp500;void*_tmp4FF[1];struct Cyc_String_pa_struct _tmp4FE;(_tmp4FE.tag=0,((
_tmp4FE.f1=(struct _dynforward_ptr)((_tmp501=_tmp18B,_tag_dynforward(_tmp501,
sizeof(char),_get_zero_arr_size_char(_tmp501,1)))),((_tmp4FF[0]=& _tmp4FE,Cyc_fprintf(
Cyc_stderr,((_tmp500="Error: could not create file %s\n",_tag_dynforward(_tmp500,
sizeof(char),_get_zero_arr_size_char(_tmp500,33)))),_tag_dynforward(_tmp4FF,
sizeof(void*),1)))))));}{int _tmp1B6=1;_npop_handler(0);return _tmp1B6;}}out_file=(
struct Cyc___cycFILE*)maybe;{struct Cyc_List_List*_tmp1B7=Cyc_current_cpp;for(0;
_tmp1B7 != 0;_tmp1B7=_tmp1B7->tl){Cyc_fputs((const char*)_untag_dynforward_ptr(*((
struct _dynforward_ptr*)_tmp1B7->hd),sizeof(char),1),out_file);}}{const char*
_tmp507;const char*_tmp506;void*_tmp505[1];struct Cyc_String_pa_struct _tmp504;(
_tmp504.tag=0,((_tmp504.f1=(struct _dynforward_ptr)((_tmp507=filename,
_tag_dynforward(_tmp507,sizeof(char),_get_zero_arr_size_char(_tmp507,1)))),((
_tmp505[0]=& _tmp504,Cyc_fprintf(out_file,((_tmp506="#include <%s>\n",
_tag_dynforward(_tmp506,sizeof(char),_get_zero_arr_size_char(_tmp506,15)))),
_tag_dynforward(_tmp505,sizeof(void*),1)))))));}Cyc_fclose(out_file);{struct
_dynforward_ptr _tmp1BC=Cstring_to_string(Ccomp);const char*_tmp511;const char*
_tmp510;const char*_tmp50F;void*_tmp50E[3];struct Cyc_String_pa_struct _tmp50D;
struct Cyc_String_pa_struct _tmp50C;struct Cyc_String_pa_struct _tmp50B;char*cmd=(
char*)_untag_dynforward_ptr(((_tmp50B.tag=0,((_tmp50B.f1=(struct _dynforward_ptr)((
_tmp511=_tmp18B,_tag_dynforward(_tmp511,sizeof(char),_get_zero_arr_size_char(
_tmp511,1)))),((_tmp50C.tag=0,((_tmp50C.f1=(struct _dynforward_ptr)((_tmp510=
_tmp18C,_tag_dynforward(_tmp510,sizeof(char),_get_zero_arr_size_char(_tmp510,1)))),((
_tmp50D.tag=0,((_tmp50D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp1BC),((_tmp50E[0]=& _tmp50D,((_tmp50E[1]=& _tmp50C,((_tmp50E[2]=& _tmp50B,Cyc_aprintf(((
_tmp50F="%s -E -dM -o %s -x c %s",_tag_dynforward(_tmp50F,sizeof(char),
_get_zero_arr_size_char(_tmp50F,24)))),_tag_dynforward(_tmp50E,sizeof(void*),3)))))))))))))))))))),
sizeof(char),1);if(!system((const char*)cmd)){{const char*_tmp51B;const char*
_tmp51A;const char*_tmp519;void*_tmp518[3];struct Cyc_String_pa_struct _tmp517;
struct Cyc_String_pa_struct _tmp516;struct Cyc_String_pa_struct _tmp515;cmd=(char*)
_untag_dynforward_ptr(((_tmp515.tag=0,((_tmp515.f1=(struct _dynforward_ptr)((
_tmp51B=_tmp18B,_tag_dynforward(_tmp51B,sizeof(char),_get_zero_arr_size_char(
_tmp51B,1)))),((_tmp516.tag=0,((_tmp516.f1=(struct _dynforward_ptr)((_tmp51A=
_tmp18D,_tag_dynforward(_tmp51A,sizeof(char),_get_zero_arr_size_char(_tmp51A,1)))),((
_tmp517.tag=0,((_tmp517.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp1BC),((_tmp518[0]=& _tmp517,((_tmp518[1]=& _tmp516,((_tmp518[2]=& _tmp515,Cyc_aprintf(((
_tmp519="%s -E -o %s -x c %s",_tag_dynforward(_tmp519,sizeof(char),
_get_zero_arr_size_char(_tmp519,20)))),_tag_dynforward(_tmp518,sizeof(void*),3)))))))))))))))))))),
sizeof(char),1);}system((const char*)cmd);}remove(_tmp18B);}}}if(Cyc_gathering()){
int _tmp1CB=0;_npop_handler(0);return _tmp1CB;}{struct Cyc_Hashtable_Table*t=Cyc_new_deps();
maybe=Cyc_fopen(_tmp18C,(const char*)"r");if(!((unsigned int)maybe)){struct Cyc_NO_SUPPORT_struct
_tmp52C;const char*_tmp52B;const char*_tmp52A;void*_tmp529[1];struct Cyc_String_pa_struct
_tmp528;struct Cyc_NO_SUPPORT_struct*_tmp527;(int)_throw((void*)((_tmp527=
_cycalloc(sizeof(*_tmp527)),((_tmp527[0]=((_tmp52C.tag=Cyc_NO_SUPPORT,((_tmp52C.f1=(
struct _dynforward_ptr)((_tmp528.tag=0,((_tmp528.f1=(struct _dynforward_ptr)((
_tmp52B=_tmp18C,_tag_dynforward(_tmp52B,sizeof(char),_get_zero_arr_size_char(
_tmp52B,1)))),((_tmp529[0]=& _tmp528,Cyc_aprintf(((_tmp52A="can't open macrosfile %s",
_tag_dynforward(_tmp52A,sizeof(char),_get_zero_arr_size_char(_tmp52A,25)))),
_tag_dynforward(_tmp529,sizeof(void*),1)))))))),_tmp52C)))),_tmp527)))));}
in_file=(struct Cyc___cycFILE*)maybe;{struct Cyc_Lexing_lexbuf*_tmp1D2=Cyc_Lexing_from_file(
in_file);struct _tuple12*entry;while((entry=((struct _tuple12*(*)(struct Cyc_Lexing_lexbuf*
lexbuf))Cyc_line)(_tmp1D2))!= 0){struct _tuple12 _tmp1D4;struct _dynforward_ptr*
_tmp1D5;struct Cyc_Set_Set*_tmp1D6;struct _tuple12*_tmp1D3=(struct _tuple12*)
_check_null(entry);_tmp1D4=*_tmp1D3;_tmp1D5=_tmp1D4.f1;_tmp1D6=_tmp1D4.f2;((void(*)(
struct Cyc_Hashtable_Table*t,struct _dynforward_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(
t,_tmp1D5,_tmp1D6);}Cyc_fclose(in_file);maybe=Cyc_fopen(_tmp18D,(const char*)"r");
if(!((unsigned int)maybe)){struct Cyc_NO_SUPPORT_struct _tmp53D;const char*_tmp53C;
const char*_tmp53B;void*_tmp53A[1];struct Cyc_String_pa_struct _tmp539;struct Cyc_NO_SUPPORT_struct*
_tmp538;(int)_throw((void*)((_tmp538=_cycalloc(sizeof(*_tmp538)),((_tmp538[0]=((
_tmp53D.tag=Cyc_NO_SUPPORT,((_tmp53D.f1=(struct _dynforward_ptr)((_tmp539.tag=0,((
_tmp539.f1=(struct _dynforward_ptr)((_tmp53C=_tmp18D,_tag_dynforward(_tmp53C,
sizeof(char),_get_zero_arr_size_char(_tmp53C,1)))),((_tmp53A[0]=& _tmp539,Cyc_aprintf(((
_tmp53B="can't open declsfile %s",_tag_dynforward(_tmp53B,sizeof(char),
_get_zero_arr_size_char(_tmp53B,24)))),_tag_dynforward(_tmp53A,sizeof(void*),1)))))))),
_tmp53D)))),_tmp538)))));}in_file=(struct Cyc___cycFILE*)maybe;_tmp1D2=Cyc_Lexing_from_file(
in_file);Cyc_slurp_out=Cyc_fopen(_tmp18E,(const char*)"w");if(!((unsigned int)Cyc_slurp_out)){
int _tmp1DD=1;_npop_handler(0);return _tmp1DD;}while(((int(*)(struct Cyc_Lexing_lexbuf*
lexbuf))Cyc_slurp)(_tmp1D2)){;}Cyc_fclose(in_file);Cyc_fclose((struct Cyc___cycFILE*)
_check_null(Cyc_slurp_out));if(Cyc_mode != (void*)3)remove(_tmp18D);maybe=Cyc_fopen(
_tmp18E,(const char*)"r");if(!((unsigned int)maybe)){int _tmp1DE=1;_npop_handler(0);
return _tmp1DE;}in_file=(struct Cyc___cycFILE*)maybe;{const char*_tmp53E;Cyc_Position_reset_position(((
_tmp53E=_tmp18E,_tag_dynforward(_tmp53E,sizeof(char),_get_zero_arr_size_char(
_tmp53E,1)))));}Cyc_Lex_lex_init(0);{struct Cyc_List_List*_tmp1E0=Cyc_Parse_parse_file(
in_file);Cyc_Lex_lex_init(0);Cyc_fclose(in_file);remove(_tmp18E);{struct Cyc_List_List*
_tmp1E1=_tmp1E0;for(0;_tmp1E1 != 0;_tmp1E1=_tmp1E1->tl){struct _tuple12*_tmp1E2=
Cyc_scan_decl((struct Cyc_Absyn_Decl*)_tmp1E1->hd);if(_tmp1E2 == 0)continue;{
struct _tuple12 _tmp1E4;struct _dynforward_ptr*_tmp1E5;struct Cyc_Set_Set*_tmp1E6;
struct _tuple12*_tmp1E3=(struct _tuple12*)_tmp1E2;_tmp1E4=*_tmp1E3;_tmp1E5=_tmp1E4.f1;
_tmp1E6=_tmp1E4.f2;{struct Cyc_Set_Set*old;{struct _handler_cons _tmp1E7;
_push_handler(& _tmp1E7);{int _tmp1E9=0;if(setjmp(_tmp1E7.handler))_tmp1E9=1;if(!
_tmp1E9){old=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct
_dynforward_ptr*key))Cyc_Hashtable_lookup)(t,_tmp1E5);;_pop_handler();}else{void*
_tmp1E8=(void*)_exn_thrown;void*_tmp1EB=_tmp1E8;_LL178: if(_tmp1EB != Cyc_Core_Not_found)
goto _LL17A;_LL179: old=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dynforward_ptr*,
struct _dynforward_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);goto _LL177;_LL17A:;_LL17B:(
void)_throw(_tmp1EB);_LL177:;}}}((void(*)(struct Cyc_Hashtable_Table*t,struct
_dynforward_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp1E5,((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(
_tmp1E6,old));}}}}{struct Cyc_Set_Set*_tmp1EC=Cyc_reachable(start_symbols,t);
struct Cyc_List_List*_tmp1ED=0;struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(
int(*cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);{
struct Cyc_List_List*_tmp1EE=_tmp1E0;for(0;_tmp1EE != 0;_tmp1EE=_tmp1EE->tl){
struct Cyc_Absyn_Decl*_tmp1EF=(struct Cyc_Absyn_Decl*)_tmp1EE->hd;struct
_dynforward_ptr*name;{void*_tmp1F0=(void*)_tmp1EF->r;struct Cyc_Absyn_Vardecl*
_tmp1F1;struct Cyc_Absyn_Fndecl*_tmp1F2;struct Cyc_Absyn_Aggrdecl*_tmp1F3;struct
Cyc_Absyn_Enumdecl*_tmp1F4;struct Cyc_Absyn_Typedefdecl*_tmp1F5;_LL17D: if(_tmp1F0
<= (void*)2)goto _LL187;if(*((int*)_tmp1F0)!= 0)goto _LL17F;_tmp1F1=((struct Cyc_Absyn_Var_d_struct*)
_tmp1F0)->f1;_LL17E: {struct _tuple0 _tmp1F7;struct _dynforward_ptr*_tmp1F8;struct
_tuple0*_tmp1F6=_tmp1F1->name;_tmp1F7=*_tmp1F6;_tmp1F8=_tmp1F7.f2;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct
_dynforward_ptr*elt))Cyc_Set_insert)(defined_symbols,_tmp1F8);if(((int(*)(int(*
compare)(struct _dynforward_ptr*,struct _dynforward_ptr*),struct Cyc_List_List*l,
struct _dynforward_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp1F8))name=
0;else{name=(struct _dynforward_ptr*)_tmp1F8;}goto _LL17C;}_LL17F: if(*((int*)
_tmp1F0)!= 1)goto _LL181;_tmp1F2=((struct Cyc_Absyn_Fn_d_struct*)_tmp1F0)->f1;
_LL180: {struct _tuple0 _tmp1FA;struct _dynforward_ptr*_tmp1FB;struct _tuple0*
_tmp1F9=_tmp1F2->name;_tmp1FA=*_tmp1F9;_tmp1FB=_tmp1FA.f2;defined_symbols=((
struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dynforward_ptr*elt))Cyc_Set_insert)(
defined_symbols,_tmp1FB);if(((int(*)(int(*compare)(struct _dynforward_ptr*,struct
_dynforward_ptr*),struct Cyc_List_List*l,struct _dynforward_ptr*x))Cyc_List_mem)(
Cyc_strptrcmp,omit_symbols,_tmp1FB))name=0;else{name=(struct _dynforward_ptr*)
_tmp1FB;}goto _LL17C;}_LL181: if(*((int*)_tmp1F0)!= 4)goto _LL183;_tmp1F3=((struct
Cyc_Absyn_Aggr_d_struct*)_tmp1F0)->f1;_LL182: {struct _tuple0 _tmp1FD;struct
_dynforward_ptr*_tmp1FE;struct _tuple0*_tmp1FC=_tmp1F3->name;_tmp1FD=*_tmp1FC;
_tmp1FE=_tmp1FD.f2;name=(struct _dynforward_ptr*)_tmp1FE;goto _LL17C;}_LL183: if(*((
int*)_tmp1F0)!= 6)goto _LL185;_tmp1F4=((struct Cyc_Absyn_Enum_d_struct*)_tmp1F0)->f1;
_LL184: {struct _tuple0 _tmp200;struct _dynforward_ptr*_tmp201;struct _tuple0*
_tmp1FF=_tmp1F4->name;_tmp200=*_tmp1FF;_tmp201=_tmp200.f2;name=(struct
_dynforward_ptr*)_tmp201;if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct
_dynforward_ptr*elt))Cyc_Set_member)(_tmp1EC,(struct _dynforward_ptr*)name)){
struct Cyc_List_List*_tmp53F;_tmp1ED=((_tmp53F=_cycalloc(sizeof(*_tmp53F)),((
_tmp53F->hd=_tmp1EF,((_tmp53F->tl=_tmp1ED,_tmp53F))))));}else{if((unsigned int)
_tmp1F4->fields){struct Cyc_List_List*_tmp203=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp1F4->fields))->v;for(0;_tmp203 != 0;_tmp203=_tmp203->tl){struct
Cyc_Absyn_Enumfield*_tmp204=(struct Cyc_Absyn_Enumfield*)_tmp203->hd;struct
_tuple0 _tmp206;struct _dynforward_ptr*_tmp207;struct _tuple0*_tmp205=_tmp204->name;
_tmp206=*_tmp205;_tmp207=_tmp206.f2;if(((int(*)(struct Cyc_Set_Set*s,struct
_dynforward_ptr*elt))Cyc_Set_member)(_tmp1EC,_tmp207)){{struct Cyc_List_List*
_tmp540;_tmp1ED=((_tmp540=_cycalloc(sizeof(*_tmp540)),((_tmp540->hd=_tmp1EF,((
_tmp540->tl=_tmp1ED,_tmp540))))));}break;}}}}name=0;goto _LL17C;}_LL185: if(*((int*)
_tmp1F0)!= 7)goto _LL187;_tmp1F5=((struct Cyc_Absyn_Typedef_d_struct*)_tmp1F0)->f1;
_LL186: {struct _tuple0 _tmp20A;struct _dynforward_ptr*_tmp20B;struct _tuple0*
_tmp209=_tmp1F5->name;_tmp20A=*_tmp209;_tmp20B=_tmp20A.f2;name=(struct
_dynforward_ptr*)_tmp20B;goto _LL17C;}_LL187: if((int)_tmp1F0 != 0)goto _LL189;
_LL188: goto _LL18A;_LL189: if((int)_tmp1F0 != 1)goto _LL18B;_LL18A: goto _LL18C;_LL18B:
if(_tmp1F0 <= (void*)2)goto _LL18D;if(*((int*)_tmp1F0)!= 2)goto _LL18D;_LL18C: goto
_LL18E;_LL18D: if(_tmp1F0 <= (void*)2)goto _LL18F;if(*((int*)_tmp1F0)!= 5)goto
_LL18F;_LL18E: goto _LL190;_LL18F: if(_tmp1F0 <= (void*)2)goto _LL191;if(*((int*)
_tmp1F0)!= 3)goto _LL191;_LL190: goto _LL192;_LL191: if(_tmp1F0 <= (void*)2)goto
_LL193;if(*((int*)_tmp1F0)!= 8)goto _LL193;_LL192: goto _LL194;_LL193: if(_tmp1F0 <= (
void*)2)goto _LL195;if(*((int*)_tmp1F0)!= 9)goto _LL195;_LL194: goto _LL196;_LL195:
if(_tmp1F0 <= (void*)2)goto _LL197;if(*((int*)_tmp1F0)!= 10)goto _LL197;_LL196: goto
_LL198;_LL197: if(_tmp1F0 <= (void*)2)goto _LL17C;if(*((int*)_tmp1F0)!= 11)goto
_LL17C;_LL198: name=0;goto _LL17C;_LL17C:;}if(name != 0  && ((int(*)(struct Cyc_Set_Set*
s,struct _dynforward_ptr*elt))Cyc_Set_member)(_tmp1EC,(struct _dynforward_ptr*)
name)){struct Cyc_List_List*_tmp541;_tmp1ED=((_tmp541=_cycalloc(sizeof(*_tmp541)),((
_tmp541->hd=_tmp1EF,((_tmp541->tl=_tmp1ED,_tmp541))))));}}}if(!Cyc_do_setjmp){
maybe=Cyc_fopen(filename,(const char*)"w");if(!((unsigned int)maybe)){int _tmp20D=
1;_npop_handler(0);return _tmp20D;}out_file=(struct Cyc___cycFILE*)maybe;}else{
out_file=Cyc_stdout;}{const char*_tmp547;const char*_tmp546;void*_tmp545[1];struct
Cyc_String_pa_struct _tmp544;struct _dynforward_ptr ifdefmacro=(_tmp544.tag=0,((
_tmp544.f1=(struct _dynforward_ptr)((_tmp547=filename,_tag_dynforward(_tmp547,
sizeof(char),_get_zero_arr_size_char(_tmp547,1)))),((_tmp545[0]=& _tmp544,Cyc_aprintf(((
_tmp546="_%s_",_tag_dynforward(_tmp546,sizeof(char),_get_zero_arr_size_char(
_tmp546,5)))),_tag_dynforward(_tmp545,sizeof(void*),1)))))));{int _tmp20E=0;for(0;
_tmp20E < _get_dynforward_size(ifdefmacro,sizeof(char));++ _tmp20E){if(*((char*)
_check_dynforward_subscript(ifdefmacro,sizeof(char),_tmp20E))== '.'  || *((char*)
_check_dynforward_subscript(ifdefmacro,sizeof(char),_tmp20E))== '/'){char _tmp54A;
char _tmp549;struct _dynforward_ptr _tmp548;(_tmp548=_dynforward_ptr_plus(
ifdefmacro,sizeof(char),_tmp20E),((_tmp549=*((char*)_check_dynforward_subscript(
_tmp548,sizeof(char),0)),((_tmp54A='_',((_get_dynforward_size(_tmp548,sizeof(
char))== 1  && (_tmp549 == '\000'  && _tmp54A != '\000')?_throw_arraybounds(): 1,*((
char*)_tmp548.curr)=_tmp54A)))))));}else{if(*((char*)_check_dynforward_subscript(
ifdefmacro,sizeof(char),_tmp20E))!= '_'  && *((char*)_check_dynforward_subscript(
ifdefmacro,sizeof(char),_tmp20E))!= '/'){char _tmp54D;char _tmp54C;struct
_dynforward_ptr _tmp54B;(_tmp54B=_dynforward_ptr_plus(ifdefmacro,sizeof(char),
_tmp20E),((_tmp54C=*((char*)_check_dynforward_subscript(_tmp54B,sizeof(char),0)),((
_tmp54D=(char)toupper((int)*((char*)_check_dynforward_subscript(ifdefmacro,
sizeof(char),_tmp20E))),((_get_dynforward_size(_tmp54B,sizeof(char))== 1  && (
_tmp54C == '\000'  && _tmp54D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp54B.curr)=
_tmp54D)))))));}}}}{const char*_tmp552;void*_tmp551[2];struct Cyc_String_pa_struct
_tmp550;struct Cyc_String_pa_struct _tmp54F;(_tmp54F.tag=0,((_tmp54F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)ifdefmacro),((_tmp550.tag=0,((_tmp550.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)ifdefmacro),((_tmp551[0]=& _tmp550,((
_tmp551[1]=& _tmp54F,Cyc_fprintf(out_file,((_tmp552="#ifndef %s\n#define %s\n",
_tag_dynforward(_tmp552,sizeof(char),_get_zero_arr_size_char(_tmp552,23)))),
_tag_dynforward(_tmp551,sizeof(void*),2)))))))))))));}{struct Cyc_List_List*
_tmp219=0;struct Cyc_List_List*_tmp21A=0;{struct Cyc_List_List*_tmp21B=_tmp1ED;
for(0;_tmp21B != 0;_tmp21B=_tmp21B->tl){struct Cyc_Absyn_Decl*_tmp21C=(struct Cyc_Absyn_Decl*)
_tmp21B->hd;int _tmp21D=0;struct _dynforward_ptr*name;{void*_tmp21E=(void*)_tmp21C->r;
struct Cyc_Absyn_Vardecl*_tmp21F;struct Cyc_Absyn_Fndecl*_tmp220;struct Cyc_Absyn_Aggrdecl*
_tmp221;struct Cyc_Absyn_Enumdecl*_tmp222;struct Cyc_Absyn_Typedefdecl*_tmp223;
_LL19A: if(_tmp21E <= (void*)2)goto _LL1A4;if(*((int*)_tmp21E)!= 0)goto _LL19C;
_tmp21F=((struct Cyc_Absyn_Var_d_struct*)_tmp21E)->f1;_LL19B: {struct _tuple0
_tmp225;struct _dynforward_ptr*_tmp226;struct _tuple0*_tmp224=_tmp21F->name;
_tmp225=*_tmp224;_tmp226=_tmp225.f2;name=(struct _dynforward_ptr*)_tmp226;goto
_LL199;}_LL19C: if(*((int*)_tmp21E)!= 1)goto _LL19E;_tmp220=((struct Cyc_Absyn_Fn_d_struct*)
_tmp21E)->f1;_LL19D: {struct _tuple0 _tmp228;struct _dynforward_ptr*_tmp229;struct
_tuple0*_tmp227=_tmp220->name;_tmp228=*_tmp227;_tmp229=_tmp228.f2;name=(struct
_dynforward_ptr*)_tmp229;goto _LL199;}_LL19E: if(*((int*)_tmp21E)!= 4)goto _LL1A0;
_tmp221=((struct Cyc_Absyn_Aggr_d_struct*)_tmp21E)->f1;_LL19F: {struct _tuple0
_tmp22B;struct _dynforward_ptr*_tmp22C;struct _tuple0*_tmp22A=_tmp221->name;
_tmp22B=*_tmp22A;_tmp22C=_tmp22B.f2;name=(struct _dynforward_ptr*)_tmp22C;goto
_LL199;}_LL1A0: if(*((int*)_tmp21E)!= 6)goto _LL1A2;_tmp222=((struct Cyc_Absyn_Enum_d_struct*)
_tmp21E)->f1;_LL1A1: {struct _tuple0 _tmp22E;struct _dynforward_ptr*_tmp22F;struct
_tuple0*_tmp22D=_tmp222->name;_tmp22E=*_tmp22D;_tmp22F=_tmp22E.f2;name=(struct
_dynforward_ptr*)_tmp22F;goto _LL199;}_LL1A2: if(*((int*)_tmp21E)!= 7)goto _LL1A4;
_tmp223=((struct Cyc_Absyn_Typedef_d_struct*)_tmp21E)->f1;_LL1A3: {struct _tuple0
_tmp231;struct _dynforward_ptr*_tmp232;struct _tuple0*_tmp230=_tmp223->name;
_tmp231=*_tmp230;_tmp232=_tmp231.f2;name=(struct _dynforward_ptr*)_tmp232;goto
_LL199;}_LL1A4: if((int)_tmp21E != 0)goto _LL1A6;_LL1A5: goto _LL1A7;_LL1A6: if((int)
_tmp21E != 1)goto _LL1A8;_LL1A7: goto _LL1A9;_LL1A8: if(_tmp21E <= (void*)2)goto _LL1AA;
if(*((int*)_tmp21E)!= 2)goto _LL1AA;_LL1A9: goto _LL1AB;_LL1AA: if(_tmp21E <= (void*)
2)goto _LL1AC;if(*((int*)_tmp21E)!= 5)goto _LL1AC;_LL1AB: goto _LL1AD;_LL1AC: if(
_tmp21E <= (void*)2)goto _LL1AE;if(*((int*)_tmp21E)!= 3)goto _LL1AE;_LL1AD: goto
_LL1AF;_LL1AE: if(_tmp21E <= (void*)2)goto _LL1B0;if(*((int*)_tmp21E)!= 8)goto
_LL1B0;_LL1AF: goto _LL1B1;_LL1B0: if(_tmp21E <= (void*)2)goto _LL1B2;if(*((int*)
_tmp21E)!= 9)goto _LL1B2;_LL1B1: goto _LL1B3;_LL1B2: if(_tmp21E <= (void*)2)goto
_LL1B4;if(*((int*)_tmp21E)!= 10)goto _LL1B4;_LL1B3: goto _LL1B5;_LL1B4: if(_tmp21E <= (
void*)2)goto _LL199;if(*((int*)_tmp21E)!= 11)goto _LL199;_LL1B5: name=0;goto _LL199;
_LL199:;}if(!((unsigned int)name) && !_tmp21D)continue;{struct Cyc_List_List*
_tmp553;_tmp219=((_tmp553=_cycalloc(sizeof(*_tmp553)),((_tmp553->hd=_tmp21C,((
_tmp553->tl=_tmp219,_tmp553))))));}{struct Cyc_List_List*_tmp554;_tmp21A=((
_tmp554=_cycalloc(sizeof(*_tmp554)),((_tmp554->hd=name,((_tmp554->tl=_tmp21A,
_tmp554))))));}}}{struct _RegionHandle _tmp235=_new_region("tc_rgn");struct
_RegionHandle*tc_rgn=& _tmp235;_push_region(tc_rgn);{struct Cyc_Tcenv_Tenv*_tmp236=
Cyc_Tcenv_tc_init(tc_rgn);Cyc_Tc_tc(tc_rgn,_tmp236,1,_tmp219);};_pop_region(
tc_rgn);}{struct Cyc_List_List*_tmp239;struct Cyc_List_List*_tmp23A;struct _tuple18
_tmp555;struct _tuple18 _tmp238=(_tmp555.f1=_tmp219,((_tmp555.f2=_tmp21A,_tmp555)));
_tmp239=_tmp238.f1;_tmp23A=_tmp238.f2;for(0;_tmp239 != 0  && _tmp23A != 0;(_tmp239=
_tmp239->tl,_tmp23A=_tmp23A->tl)){struct Cyc_Absyn_Decl*_tmp23B=(struct Cyc_Absyn_Decl*)
_tmp239->hd;struct _dynforward_ptr*_tmp23C=(struct _dynforward_ptr*)_tmp23A->hd;
int _tmp23D=0;if(!((unsigned int)_tmp23C))_tmp23D=1;if((unsigned int)_tmp23C){{
const char*_tmp559;void*_tmp558[1];struct Cyc_String_pa_struct _tmp557;ifdefmacro=((
_tmp557.tag=0,((_tmp557.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
_tmp23C),((_tmp558[0]=& _tmp557,Cyc_aprintf(((_tmp559="_%s_def_",_tag_dynforward(
_tmp559,sizeof(char),_get_zero_arr_size_char(_tmp559,9)))),_tag_dynforward(
_tmp558,sizeof(void*),1))))))));}{const char*_tmp55D;void*_tmp55C[1];struct Cyc_String_pa_struct
_tmp55B;(_tmp55B.tag=0,((_tmp55B.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)ifdefmacro),((_tmp55C[0]=& _tmp55B,Cyc_fprintf(out_file,((_tmp55D="#ifndef %s\n",
_tag_dynforward(_tmp55D,sizeof(char),_get_zero_arr_size_char(_tmp55D,12)))),
_tag_dynforward(_tmp55C,sizeof(void*),1)))))));}{const char*_tmp561;void*_tmp560[
1];struct Cyc_String_pa_struct _tmp55F;(_tmp55F.tag=0,((_tmp55F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)ifdefmacro),((_tmp560[0]=& _tmp55F,Cyc_fprintf(
out_file,((_tmp561="#define %s\n",_tag_dynforward(_tmp561,sizeof(char),
_get_zero_arr_size_char(_tmp561,12)))),_tag_dynforward(_tmp560,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Decl*_tmp562[1];Cyc_Absynpp_decllist2file(((_tmp562[0]=_tmp23B,((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp562,sizeof(struct Cyc_Absyn_Decl*),1)))),out_file);}{const char*_tmp565;void*
_tmp564;(_tmp564=0,Cyc_fprintf(out_file,((_tmp565="#endif\n",_tag_dynforward(
_tmp565,sizeof(char),_get_zero_arr_size_char(_tmp565,8)))),_tag_dynforward(
_tmp564,sizeof(void*),0)));}}else{struct Cyc_Absyn_Decl*_tmp566[1];Cyc_Absynpp_decllist2file(((
_tmp566[0]=_tmp23B,((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp566,sizeof(struct Cyc_Absyn_Decl*),1)))),out_file);}}}maybe=
Cyc_fopen(_tmp18C,(const char*)"r");if(!((unsigned int)maybe)){struct Cyc_NO_SUPPORT_struct
_tmp577;const char*_tmp576;const char*_tmp575;void*_tmp574[1];struct Cyc_String_pa_struct
_tmp573;struct Cyc_NO_SUPPORT_struct*_tmp572;(int)_throw((void*)((_tmp572=
_cycalloc(sizeof(*_tmp572)),((_tmp572[0]=((_tmp577.tag=Cyc_NO_SUPPORT,((_tmp577.f1=(
struct _dynforward_ptr)((_tmp573.tag=0,((_tmp573.f1=(struct _dynforward_ptr)((
_tmp576=_tmp18C,_tag_dynforward(_tmp576,sizeof(char),_get_zero_arr_size_char(
_tmp576,1)))),((_tmp574[0]=& _tmp573,Cyc_aprintf(((_tmp575="can't open macrosfile %s",
_tag_dynforward(_tmp575,sizeof(char),_get_zero_arr_size_char(_tmp575,25)))),
_tag_dynforward(_tmp574,sizeof(void*),1)))))))),_tmp577)))),_tmp572)))));}
in_file=(struct Cyc___cycFILE*)maybe;_tmp1D2=Cyc_Lexing_from_file(in_file);{
struct _tuple13*entry2;while((entry2=((struct _tuple13*(*)(struct Cyc_Lexing_lexbuf*
lexbuf))Cyc_suck_line)(_tmp1D2))!= 0){struct _tuple13 _tmp252;struct
_dynforward_ptr _tmp253;struct _dynforward_ptr*_tmp254;struct _tuple13*_tmp251=(
struct _tuple13*)_check_null(entry2);_tmp252=*_tmp251;_tmp253=_tmp252.f1;_tmp254=
_tmp252.f2;if(((int(*)(struct Cyc_Set_Set*s,struct _dynforward_ptr*elt))Cyc_Set_member)(
_tmp1EC,_tmp254)){{const char*_tmp57B;void*_tmp57A[1];struct Cyc_String_pa_struct
_tmp579;(_tmp579.tag=0,((_tmp579.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)*_tmp254),((_tmp57A[0]=& _tmp579,Cyc_fprintf(out_file,((_tmp57B="#ifndef %s\n",
_tag_dynforward(_tmp57B,sizeof(char),_get_zero_arr_size_char(_tmp57B,12)))),
_tag_dynforward(_tmp57A,sizeof(void*),1)))))));}{const char*_tmp57F;void*_tmp57E[
1];struct Cyc_String_pa_struct _tmp57D;(_tmp57D.tag=0,((_tmp57D.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp253),((_tmp57E[0]=& _tmp57D,Cyc_fprintf(
out_file,((_tmp57F="%s\n",_tag_dynforward(_tmp57F,sizeof(char),
_get_zero_arr_size_char(_tmp57F,4)))),_tag_dynforward(_tmp57E,sizeof(void*),1)))))));}{
const char*_tmp582;void*_tmp581;(_tmp581=0,Cyc_fprintf(out_file,((_tmp582="#endif\n",
_tag_dynforward(_tmp582,sizeof(char),_get_zero_arr_size_char(_tmp582,8)))),
_tag_dynforward(_tmp581,sizeof(void*),0)));}}}Cyc_fclose(in_file);if(Cyc_mode != (
void*)3)remove(_tmp18C);if(hstubs != 0){struct Cyc_List_List*_tmp25D=hstubs;for(0;
_tmp25D != 0;_tmp25D=_tmp25D->tl){struct _tuple14 _tmp25F;struct _dynforward_ptr
_tmp260;struct _dynforward_ptr _tmp261;struct _tuple14*_tmp25E=(struct _tuple14*)
_tmp25D->hd;_tmp25F=*_tmp25E;_tmp260=_tmp25F.f1;_tmp261=_tmp25F.f2;{struct
_dynforward_ptr*_tmp583;if(_tmp261.curr != ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr  && (_tmp260.curr == ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,struct
_dynforward_ptr*elt))Cyc_Set_member)(defined_symbols,((_tmp583=_cycalloc(sizeof(*
_tmp583)),((_tmp583[0]=_tmp260,_tmp583)))))))Cyc_fputs((const char*)
_untag_dynforward_ptr(_tmp261,sizeof(char),1),out_file);else{const char*_tmp587;
void*_tmp586[1];struct Cyc_String_pa_struct _tmp585;(_tmp585.tag=0,((_tmp585.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmp260),((_tmp586[0]=& _tmp585,Cyc_log(((
_tmp587="%s is not supported on this platform\n",_tag_dynforward(_tmp587,sizeof(
char),_get_zero_arr_size_char(_tmp587,38)))),_tag_dynforward(_tmp586,sizeof(void*),
1)))))));}}}}{const char*_tmp58A;void*_tmp589;(_tmp589=0,Cyc_fprintf(out_file,((
_tmp58A="#endif\n",_tag_dynforward(_tmp58A,sizeof(char),_get_zero_arr_size_char(
_tmp58A,8)))),_tag_dynforward(_tmp589,sizeof(void*),0)));}if(Cyc_do_setjmp){int
_tmp268=0;_npop_handler(0);return _tmp268;}else{Cyc_fclose(out_file);}if(cstubs != 
0){out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file);{struct Cyc_List_List*
_tmp269=cstubs;for(0;_tmp269 != 0;_tmp269=_tmp269->tl){struct _tuple14 _tmp26B;
struct _dynforward_ptr _tmp26C;struct _dynforward_ptr _tmp26D;struct _tuple14*_tmp26A=(
struct _tuple14*)_tmp269->hd;_tmp26B=*_tmp26A;_tmp26C=_tmp26B.f1;_tmp26D=_tmp26B.f2;{
struct _dynforward_ptr*_tmp58B;if(_tmp26D.curr != ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr  && (_tmp26C.curr == ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,struct
_dynforward_ptr*elt))Cyc_Set_member)(defined_symbols,((_tmp58B=_cycalloc(sizeof(*
_tmp58B)),((_tmp58B[0]=_tmp26C,_tmp58B)))))))Cyc_fputs((const char*)
_untag_dynforward_ptr(_tmp26D,sizeof(char),1),out_file);}}}}out_file=(struct Cyc___cycFILE*)
_check_null(Cyc_cycstubs_file);{const char*_tmp591;const char*_tmp590;void*_tmp58F[
1];struct Cyc_String_pa_struct _tmp58E;(_tmp58E.tag=0,((_tmp58E.f1=(struct
_dynforward_ptr)((_tmp591=filename,_tag_dynforward(_tmp591,sizeof(char),
_get_zero_arr_size_char(_tmp591,1)))),((_tmp58F[0]=& _tmp58E,Cyc_fprintf(out_file,((
_tmp590="#include <%s>\n\n",_tag_dynforward(_tmp590,sizeof(char),
_get_zero_arr_size_char(_tmp590,16)))),_tag_dynforward(_tmp58F,sizeof(void*),1)))))));}
if(cycstubs != 0){out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);{
struct Cyc_List_List*_tmp273=cycstubs;for(0;_tmp273 != 0;_tmp273=_tmp273->tl){
struct _tuple14 _tmp275;struct _dynforward_ptr _tmp276;struct _dynforward_ptr _tmp277;
struct _tuple14*_tmp274=(struct _tuple14*)_tmp273->hd;_tmp275=*_tmp274;_tmp276=
_tmp275.f1;_tmp277=_tmp275.f2;{struct _dynforward_ptr*_tmp592;if(_tmp277.curr != ((
struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr  && (_tmp276.curr == ((struct
_dynforward_ptr)_tag_dynforward(0,0,0)).curr  || ((int(*)(struct Cyc_Set_Set*s,
struct _dynforward_ptr*elt))Cyc_Set_member)(defined_symbols,((_tmp592=_cycalloc(
sizeof(*_tmp592)),((_tmp592[0]=_tmp276,_tmp592)))))))Cyc_fputs((const char*)
_untag_dynforward_ptr(_tmp277,sizeof(char),1),out_file);}}}{const char*_tmp595;
void*_tmp594;(_tmp594=0,Cyc_fprintf(out_file,((_tmp595="\n",_tag_dynforward(
_tmp595,sizeof(char),_get_zero_arr_size_char(_tmp595,2)))),_tag_dynforward(
_tmp594,sizeof(void*),0)));}}{int _tmp27B=0;_npop_handler(0);return _tmp27B;}}}}}}}};
_pop_handler();}else{void*_tmp190=(void*)_exn_thrown;void*_tmp281=_tmp190;struct
_dynforward_ptr _tmp282;_LL1B7: if(*((void**)_tmp281)!= Cyc_NO_SUPPORT)goto _LL1B9;
_tmp282=((struct Cyc_NO_SUPPORT_struct*)_tmp281)->f1;_LL1B8:{const char*_tmp599;
void*_tmp598[1];struct Cyc_String_pa_struct _tmp597;(_tmp597.tag=0,((_tmp597.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmp282),((_tmp598[0]=& _tmp597,Cyc_fprintf(
Cyc_stderr,((_tmp599="No support because %s\n",_tag_dynforward(_tmp599,sizeof(
char),_get_zero_arr_size_char(_tmp599,23)))),_tag_dynforward(_tmp598,sizeof(void*),
1)))))));}goto _LL1BA;_LL1B9:;_LL1BA: maybe=Cyc_fopen(filename,(const char*)"w");
if(!((unsigned int)maybe)){{const char*_tmp59F;const char*_tmp59E;void*_tmp59D[1];
struct Cyc_String_pa_struct _tmp59C;(_tmp59C.tag=0,((_tmp59C.f1=(struct
_dynforward_ptr)((_tmp59F=filename,_tag_dynforward(_tmp59F,sizeof(char),
_get_zero_arr_size_char(_tmp59F,1)))),((_tmp59D[0]=& _tmp59C,Cyc_fprintf(Cyc_stderr,((
_tmp59E="Error: could not create file %s\n",_tag_dynforward(_tmp59E,sizeof(char),
_get_zero_arr_size_char(_tmp59E,33)))),_tag_dynforward(_tmp59D,sizeof(void*),1)))))));}
return 1;}out_file=(struct Cyc___cycFILE*)maybe;{const char*_tmp5A5;const char*
_tmp5A4;void*_tmp5A3[1];struct Cyc_String_pa_struct _tmp5A2;(_tmp5A2.tag=0,((
_tmp5A2.f1=(struct _dynforward_ptr)((_tmp5A5=filename,_tag_dynforward(_tmp5A5,
sizeof(char),_get_zero_arr_size_char(_tmp5A5,1)))),((_tmp5A3[0]=& _tmp5A2,Cyc_fprintf(
out_file,((_tmp5A4="#error -- %s is not supported on this platform\n",
_tag_dynforward(_tmp5A4,sizeof(char),_get_zero_arr_size_char(_tmp5A4,48)))),
_tag_dynforward(_tmp5A3,sizeof(void*),1)))))));}Cyc_fclose(out_file);{const char*
_tmp5AB;const char*_tmp5AA;void*_tmp5A9[1];struct Cyc_String_pa_struct _tmp5A8;(
_tmp5A8.tag=0,((_tmp5A8.f1=(struct _dynforward_ptr)((_tmp5AB=filename,
_tag_dynforward(_tmp5AB,sizeof(char),_get_zero_arr_size_char(_tmp5AB,1)))),((
_tmp5A9[0]=& _tmp5A8,Cyc_fprintf(Cyc_stderr,((_tmp5AA="Warning: %s will not be supported on this platform\n",
_tag_dynforward(_tmp5AA,sizeof(char),_get_zero_arr_size_char(_tmp5AA,52)))),
_tag_dynforward(_tmp5A9,sizeof(void*),1)))))));}{const char*_tmp5AE;void*_tmp5AD;(
_tmp5AD=0,Cyc_log(((_tmp5AE="Not supported on this platform\n",_tag_dynforward(
_tmp5AE,sizeof(char),_get_zero_arr_size_char(_tmp5AE,32)))),_tag_dynforward(
_tmp5AD,sizeof(void*),0)));}remove(_tmp18C);remove(_tmp18D);remove(_tmp18E);
return 0;_LL1BB:;_LL1BC:(void)_throw(_tmp281);_LL1B6:;}}}}int Cyc_process_specfile(
const char*file,const char*dir);static void _tmp5B9(unsigned int*_tmp5B8,
unsigned int*_tmp5B7,char**_tmp5B6){for(*_tmp5B8=0;*_tmp5B8 < *_tmp5B7;(*_tmp5B8)
++){(*_tmp5B6)[*_tmp5B8]='\000';}}int Cyc_process_specfile(const char*file,const
char*dir){struct Cyc___cycFILE*_tmp2A9=Cyc_fopen(file,(const char*)"r");if(!((
unsigned int)_tmp2A9)){{const char*_tmp5B4;const char*_tmp5B3;void*_tmp5B2[1];
struct Cyc_String_pa_struct _tmp5B1;(_tmp5B1.tag=0,((_tmp5B1.f1=(struct
_dynforward_ptr)((_tmp5B4=file,_tag_dynforward(_tmp5B4,sizeof(char),
_get_zero_arr_size_char(_tmp5B4,1)))),((_tmp5B2[0]=& _tmp5B1,Cyc_fprintf(Cyc_stderr,((
_tmp5B3="Error: could not open %s\n",_tag_dynforward(_tmp5B3,sizeof(char),
_get_zero_arr_size_char(_tmp5B3,26)))),_tag_dynforward(_tmp5B2,sizeof(void*),1)))))));}
return 1;}{struct Cyc___cycFILE*_tmp2AE=(struct Cyc___cycFILE*)_tmp2A9;char*_tmp5BE;
unsigned int _tmp5BD;char*_tmp5BC;unsigned int _tmp5BB;unsigned int _tmp5BA;struct
_dynforward_ptr buf=(_tmp5BE=((_tmp5BD=(unsigned int)1024,((_tmp5BC=(char*)
_cycalloc_atomic(_check_times(sizeof(char),_tmp5BD + 1)),((((_tmp5BB=_tmp5BD,((
_tmp5B9(& _tmp5BA,& _tmp5BB,& _tmp5BC),_tmp5BC[_tmp5BB]=(char)0)))),_tmp5BC)))))),
_tag_dynforward(_tmp5BE,sizeof(char),_get_zero_arr_size_char(_tmp5BE,(
unsigned int)1024 + 1)));struct _dynforward_ptr _tmp2AF=Cyc_getcwd(buf,
_get_dynforward_size(buf,sizeof(char)));if(Cyc_mode != (void*)2){if(chdir(dir)){{
const char*_tmp5C4;const char*_tmp5C3;void*_tmp5C2[1];struct Cyc_String_pa_struct
_tmp5C1;(_tmp5C1.tag=0,((_tmp5C1.f1=(struct _dynforward_ptr)((_tmp5C4=dir,
_tag_dynforward(_tmp5C4,sizeof(char),_get_zero_arr_size_char(_tmp5C4,1)))),((
_tmp5C2[0]=& _tmp5C1,Cyc_fprintf(Cyc_stderr,((_tmp5C3="Error: can't change directory to %s\n",
_tag_dynforward(_tmp5C3,sizeof(char),_get_zero_arr_size_char(_tmp5C3,37)))),
_tag_dynforward(_tmp5C2,sizeof(void*),1)))))));}return 1;}}if(Cyc_mode == (void*)1){
struct _dynforward_ptr _tmp2B4=Cstring_to_string(Ccomp);const char*_tmp5C8;void*
_tmp5C7[1];struct Cyc_String_pa_struct _tmp5C6;system((const char*)
_untag_dynforward_ptr(((_tmp5C6.tag=0,((_tmp5C6.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)_tmp2B4),((_tmp5C7[0]=& _tmp5C6,Cyc_aprintf(((_tmp5C8="echo | %s -E -dM - -o INITMACROS.h\n",
_tag_dynforward(_tmp5C8,sizeof(char),_get_zero_arr_size_char(_tmp5C8,36)))),
_tag_dynforward(_tmp5C7,sizeof(void*),1)))))))),sizeof(char),1));}{struct Cyc_Lexing_lexbuf*
_tmp2B8=Cyc_Lexing_from_file(_tmp2AE);struct _tuple15*entry;while((entry=((struct
_tuple15*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_spec)(_tmp2B8))!= 0){struct
_tuple15 _tmp2BA;struct _dynforward_ptr _tmp2BB;struct Cyc_List_List*_tmp2BC;struct
Cyc_List_List*_tmp2BD;struct Cyc_List_List*_tmp2BE;struct Cyc_List_List*_tmp2BF;
struct Cyc_List_List*_tmp2C0;struct _tuple15*_tmp2B9=(struct _tuple15*)_check_null(
entry);_tmp2BA=*_tmp2B9;_tmp2BB=_tmp2BA.f1;_tmp2BC=_tmp2BA.f2;_tmp2BD=_tmp2BA.f3;
_tmp2BE=_tmp2BA.f4;_tmp2BF=_tmp2BA.f5;_tmp2C0=_tmp2BA.f6;if(Cyc_process_file((
const char*)_untag_dynforward_ptr(_tmp2BB,sizeof(char),1),_tmp2BC,_tmp2BD,_tmp2BE,
_tmp2BF,_tmp2C0))return 1;}Cyc_fclose(_tmp2AE);if(Cyc_mode != (void*)2){if(chdir((
const char*)((char*)_untag_dynforward_ptr(_tmp2AF,sizeof(char),1)))){{const char*
_tmp5CC;void*_tmp5CB[1];struct Cyc_String_pa_struct _tmp5CA;(_tmp5CA.tag=0,((
_tmp5CA.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmp2AF),((_tmp5CB[0]=&
_tmp5CA,Cyc_fprintf(Cyc_stderr,((_tmp5CC="Error: could not change directory to %s\n",
_tag_dynforward(_tmp5CC,sizeof(char),_get_zero_arr_size_char(_tmp5CC,41)))),
_tag_dynforward(_tmp5CB,sizeof(void*),1)))))));}return 1;}}return 0;}}}int Cyc_process_setjmp(
const char*dir);static void _tmp5D1(unsigned int*_tmp5D0,unsigned int*_tmp5CF,char**
_tmp5CE){for(*_tmp5D0=0;*_tmp5D0 < *_tmp5CF;(*_tmp5D0)++){(*_tmp5CE)[*_tmp5D0]='\000';}}
int Cyc_process_setjmp(const char*dir){char*_tmp5D6;unsigned int _tmp5D5;char*
_tmp5D4;unsigned int _tmp5D3;unsigned int _tmp5D2;struct _dynforward_ptr buf=(
_tmp5D6=((_tmp5D5=(unsigned int)1024,((_tmp5D4=(char*)_cycalloc_atomic(
_check_times(sizeof(char),_tmp5D5 + 1)),((((_tmp5D3=_tmp5D5,((_tmp5D1(& _tmp5D2,&
_tmp5D3,& _tmp5D4),_tmp5D4[_tmp5D3]=(char)0)))),_tmp5D4)))))),_tag_dynforward(
_tmp5D6,sizeof(char),_get_zero_arr_size_char(_tmp5D6,(unsigned int)1024 + 1)));
struct _dynforward_ptr _tmp2C8=Cyc_getcwd(buf,_get_dynforward_size(buf,sizeof(char)));
if(chdir(dir)){{const char*_tmp5DC;const char*_tmp5DB;void*_tmp5DA[1];struct Cyc_String_pa_struct
_tmp5D9;(_tmp5D9.tag=0,((_tmp5D9.f1=(struct _dynforward_ptr)((_tmp5DC=dir,
_tag_dynforward(_tmp5DC,sizeof(char),_get_zero_arr_size_char(_tmp5DC,1)))),((
_tmp5DA[0]=& _tmp5D9,Cyc_fprintf(Cyc_stderr,((_tmp5DB="Error: can't change directory to %s\n",
_tag_dynforward(_tmp5DB,sizeof(char),_get_zero_arr_size_char(_tmp5DB,37)))),
_tag_dynforward(_tmp5DA,sizeof(void*),1)))))));}return 1;}{struct _tuple14*_tmp5E6;
const char*_tmp5E5;const char*_tmp5E4;struct _tuple14*_tmp5E3[1];struct
_dynforward_ptr*_tmp5DD[1];if(Cyc_process_file((const char*)"setjmp.h",((_tmp5DD[
0]=_init_dynforward_ptr(_cycalloc(sizeof(struct _dynforward_ptr)),"jmp_buf",
sizeof(char),8),((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp5DD,sizeof(struct _dynforward_ptr*),1)))),0,((_tmp5E3[0]=((
_tmp5E6=_cycalloc(sizeof(*_tmp5E6)),((_tmp5E6->f1=((_tmp5E4="setjmp",
_tag_dynforward(_tmp5E4,sizeof(char),_get_zero_arr_size_char(_tmp5E4,7)))),((
_tmp5E6->f2=((_tmp5E5="extern int setjmp(jmp_buf);\n",_tag_dynforward(_tmp5E5,
sizeof(char),_get_zero_arr_size_char(_tmp5E5,29)))),_tmp5E6)))))),((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp5E3,sizeof(struct
_tuple14*),1)))),0,0))return 1;}if(chdir((const char*)((char*)
_untag_dynforward_ptr(_tmp2C8,sizeof(char),1)))){{const char*_tmp5EA;void*_tmp5E9[
1];struct Cyc_String_pa_struct _tmp5E8;(_tmp5E8.tag=0,((_tmp5E8.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp2C8),((_tmp5E9[0]=& _tmp5E8,Cyc_fprintf(
Cyc_stderr,((_tmp5EA="Error: could not change directory to %s\n",_tag_dynforward(
_tmp5EA,sizeof(char),_get_zero_arr_size_char(_tmp5EA,41)))),_tag_dynforward(
_tmp5E9,sizeof(void*),1)))))));}return 1;}return 0;}static char _tmp2DA[13]="BUILDLIB.OUT";
static struct _dynforward_ptr Cyc_output_dir={_tmp2DA,_tmp2DA + 13};static void Cyc_set_output_dir(
struct _dynforward_ptr s);static void Cyc_set_output_dir(struct _dynforward_ptr s){Cyc_output_dir=
s;}static struct Cyc_List_List*Cyc_spec_files=0;static void Cyc_add_spec_file(struct
_dynforward_ptr s);static void Cyc_add_spec_file(struct _dynforward_ptr s){struct Cyc_List_List*
_tmp5EB;Cyc_spec_files=((_tmp5EB=_cycalloc(sizeof(*_tmp5EB)),((_tmp5EB->hd=(
const char*)_untag_dynforward_ptr(s,sizeof(char),1),((_tmp5EB->tl=Cyc_spec_files,
_tmp5EB))))));}static void Cyc_set_GATHER();static void Cyc_set_GATHER(){Cyc_mode=(
void*)1;}static void Cyc_set_GATHERSCRIPT();static void Cyc_set_GATHERSCRIPT(){Cyc_mode=(
void*)2;}static void Cyc_set_FINISH();static void Cyc_set_FINISH(){Cyc_mode=(void*)3;}
static int Cyc_badparse=0;static void Cyc_unsupported_option(struct _dynforward_ptr s);
static void Cyc_unsupported_option(struct _dynforward_ptr s){{const char*_tmp5EF;void*
_tmp5EE[1];struct Cyc_String_pa_struct _tmp5ED;(_tmp5ED.tag=0,((_tmp5ED.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)s),((_tmp5EE[0]=& _tmp5ED,Cyc_fprintf(Cyc_stderr,((
_tmp5EF="Unsupported option %s\n",_tag_dynforward(_tmp5EF,sizeof(char),
_get_zero_arr_size_char(_tmp5EF,23)))),_tag_dynforward(_tmp5EE,sizeof(void*),1)))))));}
Cyc_badparse=1;}void GC_blacklist_warn_clear();struct _tuple19{struct
_dynforward_ptr f1;int f2;struct _dynforward_ptr f3;void*f4;struct _dynforward_ptr f5;
};int Cyc_main(int argc,struct _dynforward_ptr argv);int Cyc_main(int argc,struct
_dynforward_ptr argv){GC_blacklist_warn_clear();{struct _tuple19*_tmp65C;const char*
_tmp65B;struct Cyc_Arg_Flag_spec_struct _tmp65A;struct Cyc_Arg_Flag_spec_struct*
_tmp659;const char*_tmp658;const char*_tmp657;struct _tuple19*_tmp656;const char*
_tmp655;struct Cyc_Arg_Set_spec_struct _tmp654;struct Cyc_Arg_Set_spec_struct*
_tmp653;const char*_tmp652;const char*_tmp651;struct _tuple19*_tmp650;const char*
_tmp64F;struct Cyc_Arg_Unit_spec_struct _tmp64E;struct Cyc_Arg_Unit_spec_struct*
_tmp64D;const char*_tmp64C;const char*_tmp64B;struct _tuple19*_tmp64A;const char*
_tmp649;struct Cyc_Arg_Unit_spec_struct _tmp648;struct Cyc_Arg_Unit_spec_struct*
_tmp647;const char*_tmp646;const char*_tmp645;struct _tuple19*_tmp644;const char*
_tmp643;struct Cyc_Arg_Unit_spec_struct _tmp642;struct Cyc_Arg_Unit_spec_struct*
_tmp641;const char*_tmp640;const char*_tmp63F;struct _tuple19*_tmp63E;const char*
_tmp63D;struct Cyc_Arg_String_spec_struct _tmp63C;struct Cyc_Arg_String_spec_struct*
_tmp63B;const char*_tmp63A;const char*_tmp639;struct _tuple19*_tmp638[6];struct Cyc_List_List*
options=(_tmp638[5]=((_tmp65C=_cycalloc(sizeof(*_tmp65C)),((_tmp65C->f1=((
_tmp657="-",_tag_dynforward(_tmp657,sizeof(char),_get_zero_arr_size_char(_tmp657,
2)))),((_tmp65C->f2=1,((_tmp65C->f3=((_tmp658="",_tag_dynforward(_tmp658,sizeof(
char),_get_zero_arr_size_char(_tmp658,1)))),((_tmp65C->f4=(void*)((_tmp659=
_cycalloc(sizeof(*_tmp659)),((_tmp659[0]=((_tmp65A.tag=1,((_tmp65A.f1=Cyc_unsupported_option,
_tmp65A)))),_tmp659)))),((_tmp65C->f5=((_tmp65B="",_tag_dynforward(_tmp65B,
sizeof(char),_get_zero_arr_size_char(_tmp65B,1)))),_tmp65C)))))))))))),((_tmp638[
4]=((_tmp656=_cycalloc(sizeof(*_tmp656)),((_tmp656->f1=((_tmp651="-setjmp",
_tag_dynforward(_tmp651,sizeof(char),_get_zero_arr_size_char(_tmp651,8)))),((
_tmp656->f2=0,((_tmp656->f3=((_tmp652="",_tag_dynforward(_tmp652,sizeof(char),
_get_zero_arr_size_char(_tmp652,1)))),((_tmp656->f4=(void*)((_tmp653=_cycalloc(
sizeof(*_tmp653)),((_tmp653[0]=((_tmp654.tag=3,((_tmp654.f1=& Cyc_do_setjmp,
_tmp654)))),_tmp653)))),((_tmp656->f5=((_tmp655="Produce the jmp_buf and setjmp declarations on the standard output, for use by the Cyclone compiler special file cyc_setjmp.h.  Cannot be used with -gather, -gatherscript, or specfiles.",
_tag_dynforward(_tmp655,sizeof(char),_get_zero_arr_size_char(_tmp655,186)))),
_tmp656)))))))))))),((_tmp638[3]=((_tmp650=_cycalloc(sizeof(*_tmp650)),((_tmp650->f1=((
_tmp64B="-finish",_tag_dynforward(_tmp64B,sizeof(char),_get_zero_arr_size_char(
_tmp64B,8)))),((_tmp650->f2=0,((_tmp650->f3=((_tmp64C="",_tag_dynforward(_tmp64C,
sizeof(char),_get_zero_arr_size_char(_tmp64C,1)))),((_tmp650->f4=(void*)((
_tmp64D=_cycalloc(sizeof(*_tmp64D)),((_tmp64D[0]=((_tmp64E.tag=0,((_tmp64E.f1=
Cyc_set_FINISH,_tmp64E)))),_tmp64D)))),((_tmp650->f5=((_tmp64F="Produce Cyclone headers from pre-gathered C library info",
_tag_dynforward(_tmp64F,sizeof(char),_get_zero_arr_size_char(_tmp64F,57)))),
_tmp650)))))))))))),((_tmp638[2]=((_tmp64A=_cycalloc(sizeof(*_tmp64A)),((_tmp64A->f1=((
_tmp645="-gatherscript",_tag_dynforward(_tmp645,sizeof(char),
_get_zero_arr_size_char(_tmp645,14)))),((_tmp64A->f2=0,((_tmp64A->f3=((_tmp646="",
_tag_dynforward(_tmp646,sizeof(char),_get_zero_arr_size_char(_tmp646,1)))),((
_tmp64A->f4=(void*)((_tmp647=_cycalloc(sizeof(*_tmp647)),((_tmp647[0]=((_tmp648.tag=
0,((_tmp648.f1=Cyc_set_GATHERSCRIPT,_tmp648)))),_tmp647)))),((_tmp64A->f5=((
_tmp649="Produce a script to gather C library info",_tag_dynforward(_tmp649,
sizeof(char),_get_zero_arr_size_char(_tmp649,42)))),_tmp64A)))))))))))),((
_tmp638[1]=((_tmp644=_cycalloc(sizeof(*_tmp644)),((_tmp644->f1=((_tmp63F="-gather",
_tag_dynforward(_tmp63F,sizeof(char),_get_zero_arr_size_char(_tmp63F,8)))),((
_tmp644->f2=0,((_tmp644->f3=((_tmp640="",_tag_dynforward(_tmp640,sizeof(char),
_get_zero_arr_size_char(_tmp640,1)))),((_tmp644->f4=(void*)((_tmp641=_cycalloc(
sizeof(*_tmp641)),((_tmp641[0]=((_tmp642.tag=0,((_tmp642.f1=Cyc_set_GATHER,
_tmp642)))),_tmp641)))),((_tmp644->f5=((_tmp643="Gather C library info but don't produce Cyclone headers",
_tag_dynforward(_tmp643,sizeof(char),_get_zero_arr_size_char(_tmp643,56)))),
_tmp644)))))))))))),((_tmp638[0]=((_tmp63E=_cycalloc(sizeof(*_tmp63E)),((_tmp63E->f1=((
_tmp639="-d",_tag_dynforward(_tmp639,sizeof(char),_get_zero_arr_size_char(
_tmp639,3)))),((_tmp63E->f2=0,((_tmp63E->f3=((_tmp63A=" <file>",_tag_dynforward(
_tmp63A,sizeof(char),_get_zero_arr_size_char(_tmp63A,8)))),((_tmp63E->f4=(void*)((
_tmp63B=_cycalloc(sizeof(*_tmp63B)),((_tmp63B[0]=((_tmp63C.tag=5,((_tmp63C.f1=
Cyc_set_output_dir,_tmp63C)))),_tmp63B)))),((_tmp63E->f5=((_tmp63D="Set the output directory to <file>",
_tag_dynforward(_tmp63D,sizeof(char),_get_zero_arr_size_char(_tmp63D,35)))),
_tmp63E)))))))))))),((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp638,sizeof(struct _tuple19*),6)))))))))))));{const char*
_tmp65D;Cyc_Arg_parse(options,Cyc_add_spec_file,((_tmp65D="Options:",
_tag_dynforward(_tmp65D,sizeof(char),_get_zero_arr_size_char(_tmp65D,9)))),argv);}
if((((Cyc_badparse  || !Cyc_do_setjmp  && Cyc_spec_files == 0) || Cyc_do_setjmp  && 
Cyc_spec_files != 0) || Cyc_do_setjmp  && Cyc_mode == (void*)1) || Cyc_do_setjmp
 && Cyc_mode == (void*)2){{const char*_tmp65E;Cyc_Arg_usage(options,((_tmp65E="Usage: buildlib [options] specfile1 specfile2 ...\nOptions:",
_tag_dynforward(_tmp65E,sizeof(char),_get_zero_arr_size_char(_tmp65E,59)))));}
return 1;}if(Cyc_mode == (void*)2){Cyc_script_file=Cyc_fopen((const char*)"BUILDLIB.sh",(
const char*)"w");if(!((unsigned int)Cyc_script_file)){{const char*_tmp661;void*
_tmp660;(_tmp660=0,Cyc_fprintf(Cyc_stderr,((_tmp661="Could not create file BUILDLIB.sh\n",
_tag_dynforward(_tmp661,sizeof(char),_get_zero_arr_size_char(_tmp661,35)))),
_tag_dynforward(_tmp660,sizeof(void*),0)));}exit(1);}{const char*_tmp664;void*
_tmp663;(_tmp663=0,Cyc_prscript(((_tmp664="#!/bin/sh\n",_tag_dynforward(_tmp664,
sizeof(char),_get_zero_arr_size_char(_tmp664,11)))),_tag_dynforward(_tmp663,
sizeof(void*),0)));}{const char*_tmp667;void*_tmp666;(_tmp666=0,Cyc_prscript(((
_tmp667="GCC=\"gcc\"\n",_tag_dynforward(_tmp667,sizeof(char),
_get_zero_arr_size_char(_tmp667,11)))),_tag_dynforward(_tmp666,sizeof(void*),0)));}}
if(Cyc_force_directory_prefixes(Cyc_output_dir) || Cyc_force_directory(Cyc_output_dir)){{
const char*_tmp66B;void*_tmp66A[1];struct Cyc_String_pa_struct _tmp669;(_tmp669.tag=
0,((_tmp669.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_output_dir),((
_tmp66A[0]=& _tmp669,Cyc_fprintf(Cyc_stderr,((_tmp66B="Error: could not create directory %s\n",
_tag_dynforward(_tmp66B,sizeof(char),_get_zero_arr_size_char(_tmp66B,38)))),
_tag_dynforward(_tmp66A,sizeof(void*),1)))))));}return 1;}if(Cyc_mode == (void*)2){{
const char*_tmp66F;void*_tmp66E[1];struct Cyc_String_pa_struct _tmp66D;(_tmp66D.tag=
0,((_tmp66D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_output_dir),((
_tmp66E[0]=& _tmp66D,Cyc_prscript(((_tmp66F="cd %s\n",_tag_dynforward(_tmp66F,
sizeof(char),_get_zero_arr_size_char(_tmp66F,7)))),_tag_dynforward(_tmp66E,
sizeof(void*),1)))))));}{const char*_tmp672;void*_tmp671;(_tmp671=0,Cyc_prscript(((
_tmp672="echo | $GCC -E -dM - -o INITMACROS.h\n",_tag_dynforward(_tmp672,sizeof(
char),_get_zero_arr_size_char(_tmp672,38)))),_tag_dynforward(_tmp671,sizeof(void*),
0)));}}if(!Cyc_gathering()){{const char*_tmp673;Cyc_log_file=Cyc_fopen((const char*)
_untag_dynforward_ptr(Cyc_Filename_concat(Cyc_output_dir,((_tmp673="BUILDLIB.LOG",
_tag_dynforward(_tmp673,sizeof(char),_get_zero_arr_size_char(_tmp673,13))))),
sizeof(char),1),(const char*)"w");}if(!((unsigned int)Cyc_log_file)){{const char*
_tmp677;void*_tmp676[1];struct Cyc_String_pa_struct _tmp675;(_tmp675.tag=0,((
_tmp675.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_output_dir),((
_tmp676[0]=& _tmp675,Cyc_fprintf(Cyc_stderr,((_tmp677="Error: could not create log file in directory %s\n",
_tag_dynforward(_tmp677,sizeof(char),_get_zero_arr_size_char(_tmp677,50)))),
_tag_dynforward(_tmp676,sizeof(void*),1)))))));}return 1;}if(!Cyc_do_setjmp){{
const char*_tmp678;Cyc_cstubs_file=Cyc_fopen((const char*)_untag_dynforward_ptr(
Cyc_Filename_concat(Cyc_output_dir,((_tmp678="cstubs.c",_tag_dynforward(_tmp678,
sizeof(char),_get_zero_arr_size_char(_tmp678,9))))),sizeof(char),1),(const char*)"w");}
if(!((unsigned int)Cyc_cstubs_file)){{const char*_tmp67C;void*_tmp67B[1];struct
Cyc_String_pa_struct _tmp67A;(_tmp67A.tag=0,((_tmp67A.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_output_dir),((_tmp67B[0]=& _tmp67A,Cyc_fprintf(Cyc_stderr,((
_tmp67C="Error: could not create cstubs.c in directory %s\n",_tag_dynforward(
_tmp67C,sizeof(char),_get_zero_arr_size_char(_tmp67C,50)))),_tag_dynforward(
_tmp67B,sizeof(void*),1)))))));}return 1;}{const char*_tmp67D;Cyc_cycstubs_file=
Cyc_fopen((const char*)_untag_dynforward_ptr(Cyc_Filename_concat(Cyc_output_dir,((
_tmp67D="cycstubs.cyc",_tag_dynforward(_tmp67D,sizeof(char),
_get_zero_arr_size_char(_tmp67D,13))))),sizeof(char),1),(const char*)"w");}if(!((
unsigned int)Cyc_cycstubs_file)){{const char*_tmp681;void*_tmp680[1];struct Cyc_String_pa_struct
_tmp67F;(_tmp67F.tag=0,((_tmp67F.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_output_dir),((_tmp680[0]=& _tmp67F,Cyc_fprintf(Cyc_stderr,((
_tmp681="Error: could not create cycstubs.c in directory %s\n",_tag_dynforward(
_tmp681,sizeof(char),_get_zero_arr_size_char(_tmp681,52)))),_tag_dynforward(
_tmp680,sizeof(void*),1)))))));}return 1;}{const char*_tmp684;void*_tmp683;(
_tmp683=0,Cyc_fprintf((struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file),((
_tmp684="#include <core.h>\nusing Core;\n\n",_tag_dynforward(_tmp684,sizeof(char),
_get_zero_arr_size_char(_tmp684,32)))),_tag_dynforward(_tmp683,sizeof(void*),0)));}}}{
const char*outdir=(const char*)_untag_dynforward_ptr(Cyc_output_dir,sizeof(char),1);
if(Cyc_do_setjmp  && Cyc_process_setjmp(outdir))return 1;else{for(0;Cyc_spec_files
!= 0;Cyc_spec_files=((struct Cyc_List_List*)_check_null(Cyc_spec_files))->tl){if(
Cyc_process_specfile((const char*)((struct Cyc_List_List*)_check_null(Cyc_spec_files))->hd,
outdir)){{const char*_tmp687;void*_tmp686;(_tmp686=0,Cyc_fprintf(Cyc_stderr,((
_tmp687="FATAL ERROR -- QUIT!\n",_tag_dynforward(_tmp687,sizeof(char),
_get_zero_arr_size_char(_tmp687,22)))),_tag_dynforward(_tmp686,sizeof(void*),0)));}
exit(1);}}}if(Cyc_mode == (void*)2)Cyc_fclose((struct Cyc___cycFILE*)_check_null(
Cyc_script_file));else{if(!Cyc_gathering()){Cyc_fclose((struct Cyc___cycFILE*)
_check_null(Cyc_log_file));if(!Cyc_do_setjmp){Cyc_fclose((struct Cyc___cycFILE*)
_check_null(Cyc_cstubs_file));Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file));}}}
return 0;}}}
