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
16];struct _dynforward_ptr Cstring_to_string(char*);struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct _dynforward_ptr);
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dynforward_ptr);
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern
char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
8];void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
struct _dynforward_ptr Cyc_List_to_array(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);extern char Cyc_Arg_Bad[8];
struct Cyc_Arg_Bad_struct{char*tag;struct _dynforward_ptr f1;};extern char Cyc_Arg_Error[
10];struct Cyc_Arg_Unit_spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_struct{
int tag;void(*f1)(struct _dynforward_ptr);};struct Cyc_Arg_FlagString_spec_struct{
int tag;void(*f1)(struct _dynforward_ptr,struct _dynforward_ptr);};struct Cyc_Arg_Set_spec_struct{
int tag;int*f1;};struct Cyc_Arg_Clear_spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_struct{
int tag;void(*f1)(struct _dynforward_ptr);};struct Cyc_Arg_Int_spec_struct{int tag;
void(*f1)(int);};struct Cyc_Arg_Rest_spec_struct{int tag;void(*f1)(struct
_dynforward_ptr);};extern int Cyc_Arg_current;void Cyc_Arg_parse(struct Cyc_List_List*
specs,void(*anonfun)(struct _dynforward_ptr),struct _dynforward_ptr errmsg,struct
_dynforward_ptr args);struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stdout;
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;
};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_fclose(struct Cyc___cycFILE*);int Cyc_feof(struct Cyc___cycFILE*);
int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fgetc(struct Cyc___cycFILE*);struct Cyc___cycFILE*
Cyc_fopen(const char*,const char*);int Cyc_fprintf(struct Cyc___cycFILE*,struct
_dynforward_ptr,struct _dynforward_ptr);unsigned long Cyc_fread(struct
_dynforward_ptr,unsigned long,unsigned long,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dynforward_ptr f1;}
;struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
unsigned long Cyc_fwrite(struct _dynforward_ptr,unsigned long,unsigned long,struct
Cyc___cycFILE*);int Cyc_printf(struct _dynforward_ptr,struct _dynforward_ptr);int
remove(const char*);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _dynforward_ptr f1;};struct Cyc___cycFILE*
Cyc_file_open(struct _dynforward_ptr,struct _dynforward_ptr);void Cyc_file_close(
struct Cyc___cycFILE*);typedef struct{int quot;int rem;}Cyc_div_t;typedef struct{long
quot;long rem;}Cyc_ldiv_t;void*abort();void exit(int);char*getenv(const char*);int
system(const char*);unsigned long Cyc_strlen(struct _dynforward_ptr s);int Cyc_strcmp(
struct _dynforward_ptr s1,struct _dynforward_ptr s2);int Cyc_strptrcmp(struct
_dynforward_ptr*s1,struct _dynforward_ptr*s2);int Cyc_strncmp(struct
_dynforward_ptr s1,struct _dynforward_ptr s2,unsigned long len);struct
_dynforward_ptr Cyc_strconcat(struct _dynforward_ptr,struct _dynforward_ptr);struct
_dynforward_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct _dynforward_ptr Cyc_str_sepstr(
struct Cyc_List_List*,struct _dynforward_ptr);struct _dynforward_ptr Cyc_strdup(
struct _dynforward_ptr src);struct _dynforward_ptr Cyc_substring(struct
_dynforward_ptr,int ofs,unsigned long n);struct _dynforward_ptr Cyc_strchr(struct
_dynforward_ptr s,char c);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple0{void*f1;void*f2;};struct _tuple0*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);struct _dynforward_ptr Cyc_Filename_concat(
struct _dynforward_ptr,struct _dynforward_ptr);struct _dynforward_ptr Cyc_Filename_chop_extension(
struct _dynforward_ptr);struct _dynforward_ptr Cyc_Filename_dirname(struct
_dynforward_ptr);struct _dynforward_ptr Cyc_Filename_basename(struct
_dynforward_ptr);int Cyc_Filename_check_suffix(struct _dynforward_ptr,struct
_dynforward_ptr);struct Cyc_Lineno_Pos{struct _dynforward_ptr logical_file;struct
_dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[9];void Cyc_Position_reset_position(
struct _dynforward_ptr);struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _dynforward_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];extern int Cyc_Position_use_gcc_style_location;
extern int Cyc_Position_max_errors;int Cyc_Position_error_p();int isspace(int);
struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};
union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};struct _tuple1{union Cyc_Absyn_Nmspace_union
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
_tuple1*name;int is_xtunion;int is_flat;};struct Cyc_Absyn_UnknownTunion_struct{int
tag;struct Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int
tag;struct Cyc_Absyn_Tuniondecl**f1;};union Cyc_Absyn_TunionInfoU_union{struct Cyc_Absyn_UnknownTunion_struct
UnknownTunion;struct Cyc_Absyn_KnownTunion_struct KnownTunion;};struct Cyc_Absyn_TunionInfo{
union Cyc_Absyn_TunionInfoU_union tunion_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple1*tunion_name;struct
_tuple1*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{int
tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};union Cyc_Absyn_TunionFieldInfoU_union{
struct Cyc_Absyn_UnknownTunionfield_struct UnknownTunionfield;struct Cyc_Absyn_KnownTunionfield_struct
KnownTunionfield;};struct Cyc_Absyn_TunionFieldInfo{union Cyc_Absyn_TunionFieldInfoU_union
field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{int tag;
void*f1;struct _tuple1*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct Cyc_Absyn_Aggrdecl**
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
int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;
};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple1*
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
_tuple1*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple1*f1;
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
struct Cyc_List_List*f1;};struct _tuple2{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple2*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
f2;struct Cyc_Absyn_Tunionfield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct
_tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{
int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;
void*r;struct Cyc_Position_Segment*loc;void*annot;};struct Cyc_Absyn_Exp_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple3{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple3 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dynforward_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple3 f2;struct _tuple3 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dynforward_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple3 f2;
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
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*
topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dynforward_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;};struct Cyc_Absyn_Aggrdecl{void*
kind;void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple1*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;int is_flat;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _dynforward_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];extern int Cyc_Absyn_porting_c_code;
extern int Cyc_Absyn_no_regions;extern char Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(
struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dynforward_ptr lex_buffer;int
lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;
int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{
int(*read_fun)(struct _dynforward_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dynforward_ptr lex_base;struct _dynforward_ptr lex_backtrk;struct
_dynforward_ptr lex_default;struct _dynforward_ptr lex_trans;struct _dynforward_ptr
lex_check;};struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);
extern int Cyc_Parse_no_register;struct Cyc_Declaration_spec;struct Cyc_Declarator;
struct Cyc_Abstractdeclarator;struct _tuple4{void*f1;int f2;};struct Cyc_Int_tok_struct{
int tag;struct _tuple4 f1;};struct Cyc_Char_tok_struct{int tag;char f1;};struct Cyc_String_tok_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Stringopt_tok_struct{int tag;struct Cyc_Core_Opt*
f1;};struct Cyc_QualId_tok_struct{int tag;struct _tuple1*f1;};struct _tuple5{struct
Cyc_Position_Segment*f1;struct Cyc_Absyn_Conref*f2;struct Cyc_Absyn_Conref*f3;};
struct Cyc_YY1_struct{int tag;struct _tuple5*f1;};struct Cyc_YY2_struct{int tag;
struct Cyc_Absyn_Conref*f1;};struct Cyc_YY3_struct{int tag;struct Cyc_Absyn_Exp*f1;}
;struct Cyc_YY4_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_YY5_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY6_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY7_struct{int tag;void*f1;};struct Cyc_YY8_struct{int tag;struct Cyc_Core_Opt*
f1;};struct Cyc_YY9_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_YY10_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY11_struct{int tag;struct Cyc_Absyn_Pat*
f1;};struct _tuple6{struct Cyc_List_List*f1;int f2;};struct Cyc_YY12_struct{int tag;
struct _tuple6*f1;};struct Cyc_YY13_struct{int tag;struct Cyc_List_List*f1;};struct
_tuple7{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_YY14_struct{
int tag;struct _tuple7*f1;};struct Cyc_YY15_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY16_struct{int tag;struct _tuple6*f1;};struct Cyc_YY17_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_YY18_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY19_struct{int tag;struct Cyc_Declaration_spec*f1;};struct _tuple8{
struct Cyc_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_YY20_struct{int tag;
struct _tuple8*f1;};struct Cyc_YY21_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_YY22_struct{int tag;void*f1;};struct Cyc_YY23_struct{int tag;void*f1;};struct
Cyc_YY24_struct{int tag;void*f1;};struct Cyc_YY25_struct{int tag;struct Cyc_Absyn_Tqual
f1;};struct Cyc_YY26_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY27_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY28_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_YY29_struct{int tag;struct Cyc_Declarator*f1;};struct Cyc_YY30_struct{
int tag;struct Cyc_Abstractdeclarator*f1;};struct Cyc_YY31_struct{int tag;int f1;};
struct Cyc_YY32_struct{int tag;void*f1;};struct Cyc_YY33_struct{int tag;struct Cyc_Absyn_Tunionfield*
f1;};struct Cyc_YY34_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct
Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_YY35_struct{
int tag;struct _tuple9*f1;};struct Cyc_YY36_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY37_struct{int tag;struct _tuple2*f1;};struct Cyc_YY38_struct{int tag;
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
Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int
expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;void Cyc_Absynpp_decllist2file(
struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);void Cyc_Absyndump_set_params(
struct Cyc_Absynpp_Params*fs);void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*
tdl,struct Cyc___cycFILE*f);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct
_RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct
Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,struct Cyc_Position_Segment*
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
le;};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);void Cyc_Tcutil_flush_warnings();
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,
struct Cyc_List_List*ds);struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*
te,struct Cyc_List_List*);struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);
extern int Cyc_Tovc_elim_array_initializers;struct Cyc_List_List*Cyc_Tovc_tovc(
struct Cyc_List_List*decls);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_EqualConst_struct{
int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_LessSize_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
struct Cyc_CfFlowInfo_LessConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};union Cyc_CfFlowInfo_RelnOp_union{struct Cyc_CfFlowInfo_EqualConst_struct
EqualConst;struct Cyc_CfFlowInfo_LessVar_struct LessVar;struct Cyc_CfFlowInfo_LessSize_struct
LessSize;struct Cyc_CfFlowInfo_LessConst_struct LessConst;struct Cyc_CfFlowInfo_LessEqSize_struct
LessEqSize;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp_union
rop;};struct Cyc_CfFlowInfo_TagCmp{void*cmp;void*bd;};extern char Cyc_CfFlowInfo_HasTagCmps[
15];struct Cyc_CfFlowInfo_HasTagCmps_struct{char*tag;struct Cyc_List_List*f1;};
extern char Cyc_CfFlowInfo_IsZero[11];extern char Cyc_CfFlowInfo_NotZero[12];struct
Cyc_CfFlowInfo_NotZero_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[
13];struct Cyc_CfFlowInfo_UnknownZ_struct{char*tag;struct Cyc_List_List*f1;};
struct Cyc_CfFlowInfo_PlaceL_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct
Cyc_CfFlowInfo_UnknownL_struct{int tag;};union Cyc_CfFlowInfo_AbsLVal_union{struct
Cyc_CfFlowInfo_PlaceL_struct PlaceL;struct Cyc_CfFlowInfo_UnknownL_struct UnknownL;
};struct Cyc_CfFlowInfo_UnknownR_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_CfFlowInfo_Aggregate_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_CfFlowInfo_ConsumeInfo{
struct Cyc_Dict_Dict consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_BottomFL_struct{
int tag;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};union Cyc_CfFlowInfo_FlowInfo_union{
struct Cyc_CfFlowInfo_BottomFL_struct BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*
unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*
esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);struct Cyc_Interface_I;
struct Cyc_Interface_I*Cyc_Interface_empty();struct Cyc_Interface_I*Cyc_Interface_final();
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict ae);struct
_tuple11{struct _dynforward_ptr f1;struct _dynforward_ptr f2;};int Cyc_Interface_is_subinterface(
struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple11*info);struct Cyc_Interface_I*
Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*
la,struct Cyc_List_List*linfo);struct Cyc_Interface_I*Cyc_Interface_parse(struct
Cyc___cycFILE*);void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*);extern int Cyc_Cyclone_tovc_r;
extern void*Cyc_Cyclone_c_compiler;void Cyc_Port_port(struct Cyc_List_List*);void
Cyc_Lex_lex_init(int use_cyclone_keywords);int Cyc_Toc_warn_bounds_checks;int Cyc_Toc_warn_all_null_deref;
static int Cyc_pp_r=0;int Cyc_noexpand_r=0;static int Cyc_noshake_r=0;static int Cyc_stop_after_cpp_r=
0;static int Cyc_parseonly_r=0;static int Cyc_tc_r=0;static int Cyc_cf_r=0;static int
Cyc_noprint_r=0;static int Cyc_ic_r=0;static int Cyc_toc_r=0;static int Cyc_stop_after_objectfile_r=
0;static int Cyc_stop_after_asmfile_r=0;static int Cyc_inline_functions_r=0;static
int Cyc_elim_se_r=0;static int Cyc_v_r=0;static int Cyc_save_temps_r=0;static int Cyc_save_c_r=
0;static int Cyc_nogc_r=0;static int Cyc_pa_r=0;static int Cyc_pg_r=0;static int Cyc_nocheck_r=
0;static int Cyc_add_cyc_namespace_r=1;static int Cyc_generate_line_directives_r=0;
static int Cyc_print_full_evars_r=0;static int Cyc_print_all_tvars_r=0;static int Cyc_print_all_kinds_r=
0;static int Cyc_print_all_effects_r=0;static int Cyc_nocyc_setjmp_r=0;static struct
Cyc_List_List*Cyc_ccargs=0;static void Cyc_add_ccarg(struct _dynforward_ptr s);
static void Cyc_add_ccarg(struct _dynforward_ptr s){struct _dynforward_ptr*_tmp358;
struct Cyc_List_List*_tmp357;Cyc_ccargs=((_tmp357=_cycalloc(sizeof(*_tmp357)),((
_tmp357->hd=((_tmp358=_cycalloc(sizeof(*_tmp358)),((_tmp358[0]=s,_tmp358)))),((
_tmp357->tl=Cyc_ccargs,_tmp357))))));}void Cyc_set_c_compiler(struct
_dynforward_ptr s);void Cyc_set_c_compiler(struct _dynforward_ptr s){const char*
_tmp359;if(Cyc_strcmp((struct _dynforward_ptr)s,((_tmp359="vc",_tag_dynforward(
_tmp359,sizeof(char),_get_zero_arr_size_char(_tmp359,3)))))== 0){Cyc_Cyclone_c_compiler=(
void*)1;{const char*_tmp35A;Cyc_add_ccarg(((_tmp35A="-DVC_C",_tag_dynforward(
_tmp35A,sizeof(char),_get_zero_arr_size_char(_tmp35A,7)))));}}else{const char*
_tmp35B;if(Cyc_strcmp((struct _dynforward_ptr)s,((_tmp35B="gcc",_tag_dynforward(
_tmp35B,sizeof(char),_get_zero_arr_size_char(_tmp35B,4)))))== 0){Cyc_Cyclone_c_compiler=(
void*)0;{const char*_tmp35C;Cyc_add_ccarg(((_tmp35C="-DGCC_C",_tag_dynforward(
_tmp35C,sizeof(char),_get_zero_arr_size_char(_tmp35C,8)))));}}}}void Cyc_set_port_c_code();
void Cyc_set_port_c_code(){Cyc_Absyn_porting_c_code=1;Cyc_Position_max_errors=
65535;Cyc_save_c_r=1;Cyc_parseonly_r=1;}static struct _dynforward_ptr*Cyc_output_file=
0;static void Cyc_set_output_file(struct _dynforward_ptr s);static void Cyc_set_output_file(
struct _dynforward_ptr s){struct _dynforward_ptr*_tmp35D;Cyc_output_file=((_tmp35D=
_cycalloc(sizeof(*_tmp35D)),((_tmp35D[0]=s,_tmp35D))));}extern char*Cdef_inc_path;
extern char*Cdef_lib_path;extern char*Carch;extern char*Ccomp;extern char*Cversion;
static char _tmp7[1]="";static struct _dynforward_ptr Cyc_cpp={_tmp7,_tmp7 + 1};static
void Cyc_set_cpp(struct _dynforward_ptr s);static void Cyc_set_cpp(struct
_dynforward_ptr s){Cyc_cpp=s;}static struct Cyc_List_List*Cyc_cppargs=0;static void
Cyc_add_cpparg(struct _dynforward_ptr s);static void Cyc_add_cpparg(struct
_dynforward_ptr s){struct _dynforward_ptr*_tmp360;struct Cyc_List_List*_tmp35F;Cyc_cppargs=((
_tmp35F=_cycalloc(sizeof(*_tmp35F)),((_tmp35F->hd=((_tmp360=_cycalloc(sizeof(*
_tmp360)),((_tmp360[0]=s,_tmp360)))),((_tmp35F->tl=Cyc_cppargs,_tmp35F))))));}
static struct _dynforward_ptr Cyc_target_arch=(struct _dynforward_ptr){(void*)0,(
void*)(0 + 0)};static void Cyc_set_target_arch(struct _dynforward_ptr s);static void
Cyc_set_target_arch(struct _dynforward_ptr s){Cyc_toc_r=1;Cyc_target_arch=s;}
static void Cyc_print_version();static void Cyc_print_version(){{const char*_tmp364;
void*_tmp363[1];struct Cyc_String_pa_struct _tmp362;(_tmp362.tag=0,((_tmp362.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cstring_to_string(Cversion)),((
_tmp363[0]=& _tmp362,Cyc_printf(((_tmp364="The Cyclone compiler, version %s\n",
_tag_dynforward(_tmp364,sizeof(char),_get_zero_arr_size_char(_tmp364,34)))),
_tag_dynforward(_tmp363,sizeof(void*),1)))))));}{const char*_tmp369;void*_tmp368[
2];struct Cyc_String_pa_struct _tmp367;struct Cyc_String_pa_struct _tmp366;(_tmp366.tag=
0,((_tmp366.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cstring_to_string(
Ccomp)),((_tmp367.tag=0,((_tmp367.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cstring_to_string(Carch)),((_tmp368[0]=& _tmp367,((_tmp368[1]=&
_tmp366,Cyc_printf(((_tmp369="Compiled for architecture: %s CC=\"%s\"\n",
_tag_dynforward(_tmp369,sizeof(char),_get_zero_arr_size_char(_tmp369,39)))),
_tag_dynforward(_tmp368,sizeof(void*),2)))))))))))));}{const char*_tmp36D;void*
_tmp36C[1];struct Cyc_String_pa_struct _tmp36B;(_tmp36B.tag=0,((_tmp36B.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cstring_to_string(Cdef_lib_path)),((
_tmp36C[0]=& _tmp36B,Cyc_printf(((_tmp36D="Standard library directory: %s\n",
_tag_dynforward(_tmp36D,sizeof(char),_get_zero_arr_size_char(_tmp36D,32)))),
_tag_dynforward(_tmp36C,sizeof(void*),1)))))));}{const char*_tmp371;void*_tmp370[
1];struct Cyc_String_pa_struct _tmp36F;(_tmp36F.tag=0,((_tmp36F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cstring_to_string(Cdef_inc_path)),((
_tmp370[0]=& _tmp36F,Cyc_printf(((_tmp371="Standard include directory: %s\n",
_tag_dynforward(_tmp371,sizeof(char),_get_zero_arr_size_char(_tmp371,32)))),
_tag_dynforward(_tmp370,sizeof(void*),1)))))));}exit(0);}static int Cyc_is_cyclone_sourcefile(
struct _dynforward_ptr s);static int Cyc_is_cyclone_sourcefile(struct _dynforward_ptr
s){unsigned long _tmp17=Cyc_strlen((struct _dynforward_ptr)s);if(_tmp17 <= 4)return
0;else{const char*_tmp372;return Cyc_strcmp((struct _dynforward_ptr)
_dynforward_ptr_plus(s,sizeof(char),(int)(_tmp17 - 4)),((_tmp372=".cyc",
_tag_dynforward(_tmp372,sizeof(char),_get_zero_arr_size_char(_tmp372,5)))))== 0;}}
static struct Cyc_List_List*Cyc_cyclone_exec_path=0;static void Cyc_add_cyclone_exec_path(
struct _dynforward_ptr s);static void Cyc_add_cyclone_exec_path(struct
_dynforward_ptr s){unsigned long _tmp19=Cyc_strlen((struct _dynforward_ptr)s);if(
_tmp19 <= 2)return;{struct _dynforward_ptr _tmp1A=(struct _dynforward_ptr)Cyc_substring((
struct _dynforward_ptr)s,2,_tmp19 - 2);struct _dynforward_ptr*_tmp375;struct Cyc_List_List*
_tmp374;Cyc_cyclone_exec_path=((_tmp374=_cycalloc(sizeof(*_tmp374)),((_tmp374->hd=((
_tmp375=_cycalloc(sizeof(*_tmp375)),((_tmp375[0]=_tmp1A,_tmp375)))),((_tmp374->tl=
Cyc_cyclone_exec_path,_tmp374))))));}}static struct Cyc_List_List*Cyc_libargs=0;
static void Cyc_add_libarg(struct _dynforward_ptr s);static void Cyc_add_libarg(struct
_dynforward_ptr s){const char*_tmp376;if(Cyc_strcmp((struct _dynforward_ptr)s,((
_tmp376="-lxml",_tag_dynforward(_tmp376,sizeof(char),_get_zero_arr_size_char(
_tmp376,6)))))== 0)Cyc_add_ccarg(s);else{struct _dynforward_ptr*_tmp379;struct Cyc_List_List*
_tmp378;Cyc_libargs=((_tmp378=_cycalloc(sizeof(*_tmp378)),((_tmp378->hd=((
_tmp379=_cycalloc(sizeof(*_tmp379)),((_tmp379[0]=s,_tmp379)))),((_tmp378->tl=Cyc_libargs,
_tmp378))))));}}static void Cyc_add_marg(struct _dynforward_ptr s);static void Cyc_add_marg(
struct _dynforward_ptr s){Cyc_add_ccarg(s);}static void Cyc_set_save_temps();static
void Cyc_set_save_temps(){Cyc_save_temps_r=1;{const char*_tmp37A;Cyc_add_ccarg(((
_tmp37A="-save-temps",_tag_dynforward(_tmp37A,sizeof(char),
_get_zero_arr_size_char(_tmp37A,12)))));}}static int Cyc_produce_dependencies=0;
static void Cyc_set_produce_dependencies();static void Cyc_set_produce_dependencies(){
Cyc_stop_after_cpp_r=1;Cyc_produce_dependencies=1;{const char*_tmp37B;Cyc_add_cpparg(((
_tmp37B="-M",_tag_dynforward(_tmp37B,sizeof(char),_get_zero_arr_size_char(
_tmp37B,3)))));}}static struct _dynforward_ptr*Cyc_dependencies_target=0;static
void Cyc_set_dependencies_target(struct _dynforward_ptr s);static void Cyc_set_dependencies_target(
struct _dynforward_ptr s){struct _dynforward_ptr*_tmp37C;Cyc_dependencies_target=((
_tmp37C=_cycalloc(sizeof(*_tmp37C)),((_tmp37C[0]=s,_tmp37C))));}static void Cyc_set_stop_after_objectfile();
static void Cyc_set_stop_after_objectfile(){Cyc_stop_after_objectfile_r=1;{const
char*_tmp37D;Cyc_add_ccarg(((_tmp37D="-c",_tag_dynforward(_tmp37D,sizeof(char),
_get_zero_arr_size_char(_tmp37D,3)))));}}static void Cyc_set_nocppprecomp();static
void Cyc_set_nocppprecomp(){{const char*_tmp37E;Cyc_add_cpparg(((_tmp37E="-no-cpp-precomp",
_tag_dynforward(_tmp37E,sizeof(char),_get_zero_arr_size_char(_tmp37E,16)))));}{
const char*_tmp37F;Cyc_add_ccarg(((_tmp37F="-no-cpp-precomp",_tag_dynforward(
_tmp37F,sizeof(char),_get_zero_arr_size_char(_tmp37F,16)))));}}static void Cyc_set_lineno();
static void Cyc_set_lineno(){Cyc_generate_line_directives_r=1;Cyc_set_save_temps();}
static void Cyc_set_inline_functions();static void Cyc_set_inline_functions(){Cyc_inline_functions_r=
1;}static void Cyc_set_elim_se();static void Cyc_set_elim_se(){Cyc_elim_se_r=1;Cyc_set_inline_functions();}
static void Cyc_set_tovc();static void Cyc_set_tovc(){Cyc_Cyclone_tovc_r=1;{const
char*_tmp380;Cyc_add_ccarg(((_tmp380="-DCYC_ANSI_OUTPUT",_tag_dynforward(_tmp380,
sizeof(char),_get_zero_arr_size_char(_tmp380,18)))));}Cyc_set_elim_se();}static
void Cyc_set_noboundschecks();static void Cyc_set_noboundschecks(){const char*
_tmp381;Cyc_add_ccarg(((_tmp381="-DNO_CYC_BOUNDS_CHECKS",_tag_dynforward(_tmp381,
sizeof(char),_get_zero_arr_size_char(_tmp381,23)))));}static void Cyc_set_nonullchecks();
static void Cyc_set_nonullchecks(){const char*_tmp382;Cyc_add_ccarg(((_tmp382="-DNO_CYC_NULL_CHECKS",
_tag_dynforward(_tmp382,sizeof(char),_get_zero_arr_size_char(_tmp382,21)))));}
static void Cyc_set_nochecks();static void Cyc_set_nochecks(){Cyc_set_noboundschecks();
Cyc_set_nonullchecks();Cyc_nocheck_r=1;}static void Cyc_set_nocyc();static void Cyc_set_nocyc(){
Cyc_add_cyc_namespace_r=0;{const char*_tmp383;Cyc_add_ccarg(((_tmp383="-DNO_CYC_PREFIX",
_tag_dynforward(_tmp383,sizeof(char),_get_zero_arr_size_char(_tmp383,16)))));}}
static void Cyc_set_pa();static void Cyc_set_pa(){Cyc_pa_r=1;{const char*_tmp384;Cyc_add_ccarg(((
_tmp384="-DCYC_REGION_PROFILE",_tag_dynforward(_tmp384,sizeof(char),
_get_zero_arr_size_char(_tmp384,21)))));}}static void Cyc_set_pg();static void Cyc_set_pg(){
Cyc_pg_r=1;{const char*_tmp385;Cyc_add_ccarg(((_tmp385="-pg",_tag_dynforward(
_tmp385,sizeof(char),_get_zero_arr_size_char(_tmp385,4)))));}}static void Cyc_set_stop_after_asmfile();
static void Cyc_set_stop_after_asmfile(){Cyc_stop_after_asmfile_r=1;{const char*
_tmp386;Cyc_add_ccarg(((_tmp386="-S",_tag_dynforward(_tmp386,sizeof(char),
_get_zero_arr_size_char(_tmp386,3)))));}}static void Cyc_set_all_warnings();static
void Cyc_set_all_warnings(){Cyc_Toc_warn_bounds_checks=1;Cyc_Toc_warn_all_null_deref=
1;}static void*Cyc_intype=(void*)0;static void Cyc_set_inputtype(struct
_dynforward_ptr s);static void Cyc_set_inputtype(struct _dynforward_ptr s){const char*
_tmp387;if(Cyc_strcmp((struct _dynforward_ptr)s,((_tmp387="cyc",_tag_dynforward(
_tmp387,sizeof(char),_get_zero_arr_size_char(_tmp387,4)))))== 0)Cyc_intype=(void*)
1;else{const char*_tmp388;if(Cyc_strcmp((struct _dynforward_ptr)s,((_tmp388="none",
_tag_dynforward(_tmp388,sizeof(char),_get_zero_arr_size_char(_tmp388,5)))))== 0)
Cyc_intype=(void*)0;else{const char*_tmp38C;void*_tmp38B[1];struct Cyc_String_pa_struct
_tmp38A;(_tmp38A.tag=0,((_tmp38A.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)s),((_tmp38B[0]=& _tmp38A,Cyc_fprintf(Cyc_stderr,((_tmp38C="Input type '%s' not supported\n",
_tag_dynforward(_tmp38C,sizeof(char),_get_zero_arr_size_char(_tmp38C,31)))),
_tag_dynforward(_tmp38B,sizeof(void*),1)))))));}}}struct _dynforward_ptr Cyc_make_base_filename(
struct _dynforward_ptr s,struct _dynforward_ptr*output_file);struct _dynforward_ptr
Cyc_make_base_filename(struct _dynforward_ptr s,struct _dynforward_ptr*output_file){
struct _dynforward_ptr _tmp32=Cyc_Filename_dirname(output_file == 0?s:*output_file);
struct _dynforward_ptr _tmp33=Cyc_Filename_chop_extension((struct _dynforward_ptr)
Cyc_Filename_basename(s));struct _dynforward_ptr _tmp34=Cyc_strlen((struct
_dynforward_ptr)_tmp32)> 0?Cyc_Filename_concat((struct _dynforward_ptr)_tmp32,(
struct _dynforward_ptr)_tmp33): _tmp33;return(struct _dynforward_ptr)_tmp34;}static
struct Cyc_List_List*Cyc_cyclone_files=0;static void Cyc_add_other(struct
_dynforward_ptr s);static void Cyc_add_other(struct _dynforward_ptr s){if(Cyc_intype
== (void*)1  || Cyc_is_cyclone_sourcefile(s)){{struct _dynforward_ptr*_tmp38F;
struct Cyc_List_List*_tmp38E;Cyc_cyclone_files=((_tmp38E=_cycalloc(sizeof(*
_tmp38E)),((_tmp38E->hd=((_tmp38F=_cycalloc(sizeof(*_tmp38F)),((_tmp38F[0]=s,
_tmp38F)))),((_tmp38E->tl=Cyc_cyclone_files,_tmp38E))))));}{struct
_dynforward_ptr _tmp37=Cyc_make_base_filename(s,Cyc_output_file);const char*
_tmp390;struct _dynforward_ptr _tmp38=Cyc_strconcat((struct _dynforward_ptr)_tmp37,((
_tmp390=".c",_tag_dynforward(_tmp390,sizeof(char),_get_zero_arr_size_char(
_tmp390,3)))));Cyc_add_ccarg((struct _dynforward_ptr)_tmp38);}}else{Cyc_add_ccarg(
s);}}static void Cyc_remove_file(struct _dynforward_ptr s);static void Cyc_remove_file(
struct _dynforward_ptr s){if(Cyc_save_temps_r)return;else{remove((const char*)
_untag_dynforward_ptr(s,sizeof(char),1));}}struct _tuple12{struct _dynforward_ptr*
f1;struct _dynforward_ptr*f2;};struct Cyc_List_List*Cyc_read_specs(struct
_dynforward_ptr file);struct Cyc_List_List*Cyc_read_specs(struct _dynforward_ptr
file){struct Cyc_List_List*_tmp3A=0;int c;int i;char strname[256];char strvalue[4096];
struct Cyc___cycFILE*spec_file=Cyc_fopen((const char*)_untag_dynforward_ptr(file,
sizeof(char),1),(const char*)"r");if(spec_file == 0){{const char*_tmp394;void*
_tmp393[1];struct Cyc_String_pa_struct _tmp392;(_tmp392.tag=0,((_tmp392.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)file),((_tmp393[0]=& _tmp392,Cyc_fprintf(
Cyc_stderr,((_tmp394="Error opening spec file %s\n",_tag_dynforward(_tmp394,
sizeof(char),_get_zero_arr_size_char(_tmp394,28)))),_tag_dynforward(_tmp393,
sizeof(void*),1)))))));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return 0;}
while(1){while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)spec_file);if(isspace(c))
continue;if(c == '*')break;if(c != - 1){{const char*_tmp399;void*_tmp398[2];struct
Cyc_String_pa_struct _tmp397;struct Cyc_Int_pa_struct _tmp396;(_tmp396.tag=1,((
_tmp396.f1=(unsigned long)c,((_tmp397.tag=0,((_tmp397.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)file),((_tmp398[0]=& _tmp397,((_tmp398[1]=& _tmp396,Cyc_fprintf(
Cyc_stderr,((_tmp399="Error reading spec file %s: unexpected character '%c'\n",
_tag_dynforward(_tmp399,sizeof(char),_get_zero_arr_size_char(_tmp399,55)))),
_tag_dynforward(_tmp398,sizeof(void*),2)))))))))))));}Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);}goto CLEANUP_AND_RETURN;}JUST_AFTER_STAR: i=0;while(1){c=Cyc_fgetc((
struct Cyc___cycFILE*)spec_file);if(c == - 1){{const char*_tmp39D;void*_tmp39C[1];
struct Cyc_String_pa_struct _tmp39B;(_tmp39B.tag=0,((_tmp39B.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)file),((_tmp39C[0]=& _tmp39B,Cyc_fprintf(
Cyc_stderr,((_tmp39D="Error reading spec file %s: unexpected EOF\n",
_tag_dynforward(_tmp39D,sizeof(char),_get_zero_arr_size_char(_tmp39D,44)))),
_tag_dynforward(_tmp39C,sizeof(void*),1)))))));}Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);goto CLEANUP_AND_RETURN;}if(c == ':'){strname[
_check_known_subscript_notnull(256,i)]='\000';break;}strname[
_check_known_subscript_notnull(256,i)]=(char)c;++ i;if(i >= 256){{const char*
_tmp3A1;void*_tmp3A0[1];struct Cyc_String_pa_struct _tmp39F;(_tmp39F.tag=0,((
_tmp39F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)file),((_tmp3A0[0]=&
_tmp39F,Cyc_fprintf(Cyc_stderr,((_tmp3A1="Error reading spec file %s: string name too long\n",
_tag_dynforward(_tmp3A1,sizeof(char),_get_zero_arr_size_char(_tmp3A1,50)))),
_tag_dynforward(_tmp3A0,sizeof(void*),1)))))));}Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);goto CLEANUP_AND_RETURN;}}while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)
spec_file);if(isspace(c))continue;break;}if(c == '*'){{struct _tuple12*_tmp3AE;
const char*_tmp3AD;struct _dynforward_ptr*_tmp3AC;struct _dynforward_ptr*_tmp3AB;
struct Cyc_List_List*_tmp3AA;_tmp3A=((_tmp3AA=_cycalloc(sizeof(*_tmp3AA)),((
_tmp3AA->hd=((_tmp3AE=_cycalloc(sizeof(*_tmp3AE)),((_tmp3AE->f1=((_tmp3AB=
_cycalloc(sizeof(*_tmp3AB)),((_tmp3AB[0]=(struct _dynforward_ptr)Cyc_strdup(
_tag_dynforward(strname,sizeof(char),256)),_tmp3AB)))),((_tmp3AE->f2=((_tmp3AC=
_cycalloc(sizeof(*_tmp3AC)),((_tmp3AC[0]=(struct _dynforward_ptr)Cyc_strdup(((
_tmp3AD="",_tag_dynforward(_tmp3AD,sizeof(char),_get_zero_arr_size_char(_tmp3AD,
1))))),_tmp3AC)))),_tmp3AE)))))),((_tmp3AA->tl=_tmp3A,_tmp3AA))))));}goto
JUST_AFTER_STAR;}strvalue[0]=(char)c;i=1;while(1){c=Cyc_fgetc((struct Cyc___cycFILE*)
spec_file);if((c == - 1  || c == '\n') || c == '\r'){strvalue[
_check_known_subscript_notnull(4096,i)]='\000';break;}strvalue[
_check_known_subscript_notnull(4096,i)]=(char)c;++ i;if(i >= 4096){{const char*
_tmp3B3;void*_tmp3B2[2];struct Cyc_String_pa_struct _tmp3B1;struct Cyc_String_pa_struct
_tmp3B0;(_tmp3B0.tag=0,((_tmp3B0.f1=(struct _dynforward_ptr)_tag_dynforward(
strname,sizeof(char),256),((_tmp3B1.tag=0,((_tmp3B1.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)file),((_tmp3B2[0]=& _tmp3B1,((_tmp3B2[1]=& _tmp3B0,Cyc_fprintf(
Cyc_stderr,((_tmp3B3="Error reading spec file %s: value of %s too long\n",
_tag_dynforward(_tmp3B3,sizeof(char),_get_zero_arr_size_char(_tmp3B3,50)))),
_tag_dynforward(_tmp3B2,sizeof(void*),2)))))))))))));}Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);goto CLEANUP_AND_RETURN;}}{struct _tuple12*_tmp3BC;struct
_dynforward_ptr*_tmp3BB;struct _dynforward_ptr*_tmp3BA;struct Cyc_List_List*
_tmp3B9;_tmp3A=((_tmp3B9=_cycalloc(sizeof(*_tmp3B9)),((_tmp3B9->hd=((_tmp3BC=
_cycalloc(sizeof(*_tmp3BC)),((_tmp3BC->f1=((_tmp3BA=_cycalloc(sizeof(*_tmp3BA)),((
_tmp3BA[0]=(struct _dynforward_ptr)Cyc_strdup(_tag_dynforward(strname,sizeof(char),
256)),_tmp3BA)))),((_tmp3BC->f2=((_tmp3BB=_cycalloc(sizeof(*_tmp3BB)),((_tmp3BB[
0]=(struct _dynforward_ptr)Cyc_strdup(_tag_dynforward(strvalue,sizeof(char),4096)),
_tmp3BB)))),_tmp3BC)))))),((_tmp3B9->tl=_tmp3A,_tmp3B9))))));}if(c == - 1)goto
CLEANUP_AND_RETURN;}CLEANUP_AND_RETURN: Cyc_fclose((struct Cyc___cycFILE*)
spec_file);return _tmp3A;}struct _dynforward_ptr Cyc_split_specs(struct
_dynforward_ptr cmdline);static void _tmp3C6(struct _dynforward_ptr*_tmp5B,
unsigned int*_tmp3C5,unsigned int*_tmp3C4,struct _dynforward_ptr**_tmp3C2){for(*
_tmp3C5=0;*_tmp3C5 < *_tmp3C4;(*_tmp3C5)++){(*_tmp3C2)[*_tmp3C5]=*(*((struct
_dynforward_ptr**)_check_dynforward_subscript(*_tmp5B,sizeof(struct
_dynforward_ptr*),(int)*_tmp3C5)));}}struct _dynforward_ptr Cyc_split_specs(struct
_dynforward_ptr cmdline){if(cmdline.curr == ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr)return _tag_dynforward(0,0,0);{unsigned long _tmp55=
Cyc_strlen((struct _dynforward_ptr)cmdline);struct Cyc_List_List*_tmp56=0;char buf[
4096];int i=0;int j=0;if(_tmp55 > 4096)goto DONE;while(1){while(1){if(i >= _tmp55)
goto DONE;if(*((const char*)_check_dynforward_subscript(cmdline,sizeof(char),i))== 
0)goto DONE;if(!isspace((int)*((const char*)_check_dynforward_subscript(cmdline,
sizeof(char),i))))break;++ i;}j=0;while(1){if(i >= _tmp55)break;if(*((const char*)
_check_dynforward_subscript(cmdline,sizeof(char),i))== 0)break;if(isspace((int)*((
const char*)_check_dynforward_subscript(cmdline,sizeof(char),i))))break;if(*((
const char*)_check_dynforward_subscript(cmdline,sizeof(char),i))== '\\'){++ i;if(i
>= _tmp55)break;if(*((const char*)_check_dynforward_subscript(cmdline,sizeof(char),
i))== 0)break;buf[_check_known_subscript_notnull(4096,j)]=*((const char*)
_check_dynforward_subscript(cmdline,sizeof(char),i));++ j;}else{buf[
_check_known_subscript_notnull(4096,j)]=*((const char*)
_check_dynforward_subscript(cmdline,sizeof(char),i));++ j;}++ i;}if(j < 4096)buf[
_check_known_subscript_notnull(4096,j)]='\000';{struct _dynforward_ptr*_tmp3BF;
struct Cyc_List_List*_tmp3BE;_tmp56=((_tmp3BE=_cycalloc(sizeof(*_tmp3BE)),((
_tmp3BE->hd=((_tmp3BF=_cycalloc(sizeof(*_tmp3BF)),((_tmp3BF[0]=(struct
_dynforward_ptr)Cyc_strdup(_tag_dynforward(buf,sizeof(char),4096)),_tmp3BF)))),((
_tmp3BE->tl=_tmp56,_tmp3BE))))));}}DONE: _tmp56=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(_tmp56);{struct Cyc_List_List*_tmp3C0;_tmp56=((
_tmp3C0=_cycalloc(sizeof(*_tmp3C0)),((_tmp3C0->hd=_init_dynforward_ptr(_cycalloc(
sizeof(struct _dynforward_ptr)),"",sizeof(char),1),((_tmp3C0->tl=_tmp56,_tmp3C0))))));}{
struct _dynforward_ptr _tmp5B=((struct _dynforward_ptr(*)(struct Cyc_List_List*x))
Cyc_List_to_array)(_tmp56);unsigned int _tmp3C5;unsigned int _tmp3C4;struct
_dynforward_ptr _tmp3C3;struct _dynforward_ptr*_tmp3C2;unsigned int _tmp3C1;struct
_dynforward_ptr _tmp5C=(_tmp3C1=_get_dynforward_size(_tmp5B,sizeof(struct
_dynforward_ptr*)),((_tmp3C2=(struct _dynforward_ptr*)_cycalloc(_check_times(
sizeof(struct _dynforward_ptr),_tmp3C1)),((_tmp3C3=_tag_dynforward(_tmp3C2,
sizeof(struct _dynforward_ptr),_tmp3C1),((((_tmp3C4=_tmp3C1,_tmp3C6(& _tmp5B,&
_tmp3C5,& _tmp3C4,& _tmp3C2))),_tmp3C3)))))));return _tmp5C;}}}int Cyc_compile_failure=
0;struct Cyc___cycFILE*Cyc_try_file_open(struct _dynforward_ptr filename,struct
_dynforward_ptr mode,struct _dynforward_ptr msg_part);struct Cyc___cycFILE*Cyc_try_file_open(
struct _dynforward_ptr filename,struct _dynforward_ptr mode,struct _dynforward_ptr
msg_part){struct _handler_cons _tmp61;_push_handler(& _tmp61);{int _tmp63=0;if(
setjmp(_tmp61.handler))_tmp63=1;if(!_tmp63){{struct Cyc___cycFILE*_tmp64=(struct
Cyc___cycFILE*)Cyc_file_open(filename,mode);_npop_handler(0);return _tmp64;};
_pop_handler();}else{void*_tmp62=(void*)_exn_thrown;void*_tmp66=_tmp62;_LL1:;
_LL2: Cyc_compile_failure=1;{const char*_tmp3CB;void*_tmp3CA[2];struct Cyc_String_pa_struct
_tmp3C9;struct Cyc_String_pa_struct _tmp3C8;(_tmp3C8.tag=0,((_tmp3C8.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)filename),((_tmp3C9.tag=0,((_tmp3C9.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)msg_part),((_tmp3CA[0]=& _tmp3C9,((
_tmp3CA[1]=& _tmp3C8,Cyc_fprintf(Cyc_stderr,((_tmp3CB="\nError: couldn't open %s %s\n",
_tag_dynforward(_tmp3CB,sizeof(char),_get_zero_arr_size_char(_tmp3CB,29)))),
_tag_dynforward(_tmp3CA,sizeof(void*),2)))))))))))));}Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);return 0;_LL3:;_LL4:(void)_throw(_tmp66);_LL0:;}}}struct Cyc_List_List*
Cyc_do_stage(struct _dynforward_ptr stage_name,struct Cyc_List_List*(*f)(void*,
struct Cyc_List_List*),void*env,struct Cyc_List_List*tds);struct Cyc_List_List*Cyc_do_stage(
struct _dynforward_ptr stage_name,struct Cyc_List_List*(*f)(void*,struct Cyc_List_List*),
void*env,struct Cyc_List_List*tds){const char*_tmp3CC;struct _dynforward_ptr
exn_string=(_tmp3CC="",_tag_dynforward(_tmp3CC,sizeof(char),
_get_zero_arr_size_char(_tmp3CC,1)));const char*_tmp3CD;struct _dynforward_ptr
explain_string=(_tmp3CD="",_tag_dynforward(_tmp3CD,sizeof(char),
_get_zero_arr_size_char(_tmp3CD,1)));int other_exn=0;struct Cyc_Core_Impossible_struct
_tmp3D3;const char*_tmp3D2;struct Cyc_Core_Impossible_struct*_tmp3D1;void*ex=(void*)((
_tmp3D1=_cycalloc(sizeof(*_tmp3D1)),((_tmp3D1[0]=((_tmp3D3.tag=Cyc_Core_Impossible,((
_tmp3D3.f1=((_tmp3D2="",_tag_dynforward(_tmp3D2,sizeof(char),
_get_zero_arr_size_char(_tmp3D2,1)))),_tmp3D3)))),_tmp3D1))));struct Cyc_List_List*
_tmp6B=0;{struct _handler_cons _tmp6C;_push_handler(& _tmp6C);{int _tmp6E=0;if(
setjmp(_tmp6C.handler))_tmp6E=1;if(!_tmp6E){_tmp6B=f(env,tds);;_pop_handler();}
else{void*_tmp6D=(void*)_exn_thrown;void*_tmp70=_tmp6D;struct _dynforward_ptr
_tmp71;struct _dynforward_ptr _tmp72;struct _dynforward_ptr _tmp73;_LL6: if(*((void**)
_tmp70)!= Cyc_Core_Impossible)goto _LL8;_tmp71=((struct Cyc_Core_Impossible_struct*)
_tmp70)->f1;_LL7:{const char*_tmp3D4;exn_string=((_tmp3D4="Exception Core::Impossible",
_tag_dynforward(_tmp3D4,sizeof(char),_get_zero_arr_size_char(_tmp3D4,27))));}
explain_string=_tmp71;goto _LL5;_LL8: if(_tmp70 != Cyc_Dict_Absent)goto _LLA;_LL9:{
const char*_tmp3D5;exn_string=((_tmp3D5="Exception Dict::Absent",_tag_dynforward(
_tmp3D5,sizeof(char),_get_zero_arr_size_char(_tmp3D5,23))));}goto _LL5;_LLA: if(*((
void**)_tmp70)!= Cyc_Core_Invalid_argument)goto _LLC;_tmp72=((struct Cyc_Core_Invalid_argument_struct*)
_tmp70)->f1;_LLB:{const char*_tmp3D6;exn_string=((_tmp3D6="Exception Core::Invalid_argument",
_tag_dynforward(_tmp3D6,sizeof(char),_get_zero_arr_size_char(_tmp3D6,33))));}
explain_string=_tmp72;goto _LL5;_LLC: if(*((void**)_tmp70)!= Cyc_Core_Failure)goto
_LLE;_tmp73=((struct Cyc_Core_Failure_struct*)_tmp70)->f1;_LLD:{const char*_tmp3D7;
exn_string=((_tmp3D7="Exception Core::Failure",_tag_dynforward(_tmp3D7,sizeof(
char),_get_zero_arr_size_char(_tmp3D7,24))));}explain_string=_tmp73;goto _LL5;
_LLE:;_LLF: ex=_tmp70;other_exn=1;{const char*_tmp3D8;exn_string=((_tmp3D8="Uncaught exception",
_tag_dynforward(_tmp3D8,sizeof(char),_get_zero_arr_size_char(_tmp3D8,19))));}
goto _LL5;_LL10:;_LL11:(void)_throw(_tmp70);_LL5:;}}}if(Cyc_Position_error_p())
Cyc_compile_failure=1;{const char*_tmp3D9;if(Cyc_strcmp((struct _dynforward_ptr)
exn_string,((_tmp3D9="",_tag_dynforward(_tmp3D9,sizeof(char),
_get_zero_arr_size_char(_tmp3D9,1)))))!= 0){Cyc_compile_failure=1;{const char*
_tmp3DF;void*_tmp3DE[3];struct Cyc_String_pa_struct _tmp3DD;struct Cyc_String_pa_struct
_tmp3DC;struct Cyc_String_pa_struct _tmp3DB;(_tmp3DB.tag=0,((_tmp3DB.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)explain_string),((_tmp3DC.tag=0,((
_tmp3DC.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)stage_name),((_tmp3DD.tag=
0,((_tmp3DD.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)exn_string),((
_tmp3DE[0]=& _tmp3DD,((_tmp3DE[1]=& _tmp3DC,((_tmp3DE[2]=& _tmp3DB,Cyc_fprintf(Cyc_stderr,((
_tmp3DF="\n%s thrown during %s: %s",_tag_dynforward(_tmp3DF,sizeof(char),
_get_zero_arr_size_char(_tmp3DF,25)))),_tag_dynforward(_tmp3DE,sizeof(void*),3)))))))))))))))))));}}}
if(Cyc_compile_failure){{const char*_tmp3E2;void*_tmp3E1;(_tmp3E1=0,Cyc_fprintf(
Cyc_stderr,((_tmp3E2="\nCOMPILATION FAILED!\n",_tag_dynforward(_tmp3E2,sizeof(
char),_get_zero_arr_size_char(_tmp3E2,22)))),_tag_dynforward(_tmp3E1,sizeof(void*),
0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);if(other_exn)(int)_throw(ex);
return _tmp6B;}else{if(Cyc_v_r){{const char*_tmp3E6;void*_tmp3E5[1];struct Cyc_String_pa_struct
_tmp3E4;(_tmp3E4.tag=0,((_tmp3E4.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)stage_name),((_tmp3E5[0]=& _tmp3E4,Cyc_fprintf(Cyc_stderr,((
_tmp3E6="%s completed.\n",_tag_dynforward(_tmp3E6,sizeof(char),
_get_zero_arr_size_char(_tmp3E6,15)))),_tag_dynforward(_tmp3E5,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);return _tmp6B;}}return _tmp6B;}struct
Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*ignore);
struct Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*
ignore){Cyc_Lex_lex_init(1);{struct Cyc_List_List*_tmp89=Cyc_Parse_parse_file(f);
Cyc_Lex_lex_init(1);return _tmp89;}}struct _tuple13{struct _RegionHandle*f1;struct
Cyc_Tcenv_Tenv*f2;};struct Cyc_List_List*Cyc_do_typecheck(struct _tuple13*tcenv,
struct Cyc_List_List*tds);struct Cyc_List_List*Cyc_do_typecheck(struct _tuple13*
tcenv,struct Cyc_List_List*tds){struct _RegionHandle*_tmp8B;struct Cyc_Tcenv_Tenv*
_tmp8C;struct _tuple13 _tmp8A=*tcenv;_tmp8B=_tmp8A.f1;_tmp8C=_tmp8A.f2;Cyc_Tc_tc(
_tmp8B,_tmp8C,1,tds);if(!Cyc_noshake_r)tds=Cyc_Tc_treeshake(_tmp8C,tds);return
tds;}struct Cyc_List_List*Cyc_do_cfcheck(int ignore,struct Cyc_List_List*tds);
struct Cyc_List_List*Cyc_do_cfcheck(int ignore,struct Cyc_List_List*tds){Cyc_NewControlFlow_cf_check(
tds);return tds;}struct _tuple14{struct Cyc_Tcenv_Tenv*f1;struct Cyc___cycFILE*f2;
struct Cyc___cycFILE*f3;};struct Cyc_List_List*Cyc_do_interface(struct _tuple14*
params,struct Cyc_List_List*tds);struct Cyc_List_List*Cyc_do_interface(struct
_tuple14*params,struct Cyc_List_List*tds){struct _tuple14 _tmp8E;struct Cyc_Tcenv_Tenv*
_tmp8F;struct Cyc___cycFILE*_tmp90;struct Cyc___cycFILE*_tmp91;struct _tuple14*
_tmp8D=params;_tmp8E=*_tmp8D;_tmp8F=_tmp8E.f1;_tmp90=_tmp8E.f2;_tmp91=_tmp8E.f3;{
struct Cyc_Interface_I*_tmp92=Cyc_Interface_extract(_tmp8F->ae);if(_tmp90 == 0)Cyc_Interface_save(
_tmp92,_tmp91);else{struct Cyc_Interface_I*_tmp93=Cyc_Interface_parse((struct Cyc___cycFILE*)
_tmp90);const char*_tmp3EB;const char*_tmp3EA;struct _tuple11*_tmp3E9;if(!Cyc_Interface_is_subinterface(
_tmp93,_tmp92,((_tmp3E9=_cycalloc(sizeof(*_tmp3E9)),((_tmp3E9->f1=((_tmp3EB="written interface",
_tag_dynforward(_tmp3EB,sizeof(char),_get_zero_arr_size_char(_tmp3EB,18)))),((
_tmp3E9->f2=((_tmp3EA="maximal interface",_tag_dynforward(_tmp3EA,sizeof(char),
_get_zero_arr_size_char(_tmp3EA,18)))),_tmp3E9))))))))Cyc_compile_failure=1;
else{Cyc_Interface_save(_tmp93,_tmp91);}}return tds;}}struct Cyc_List_List*Cyc_do_translate(
int ignore,struct Cyc_List_List*tds);struct Cyc_List_List*Cyc_do_translate(int
ignore,struct Cyc_List_List*tds){return Cyc_Toc_toc(tds);}struct Cyc_List_List*Cyc_do_tovc(
int ignore,struct Cyc_List_List*tds);struct Cyc_List_List*Cyc_do_tovc(int ignore,
struct Cyc_List_List*tds){Cyc_Tovc_elim_array_initializers=Cyc_Cyclone_tovc_r;
return Cyc_Tovc_tovc(tds);}static struct _dynforward_ptr Cyc_cyc_setjmp=(struct
_dynforward_ptr){(void*)0,(void*)(0 + 0)};static struct _dynforward_ptr Cyc_cyc_include=(
struct _dynforward_ptr){(void*)0,(void*)(0 + 0)};static void Cyc_set_cyc_include(
struct _dynforward_ptr f);static void Cyc_set_cyc_include(struct _dynforward_ptr f){
Cyc_cyc_include=f;}int Cyc_copy_internal_file(struct _dynforward_ptr file,struct Cyc___cycFILE*
out_file);int Cyc_copy_internal_file(struct _dynforward_ptr file,struct Cyc___cycFILE*
out_file){if(file.curr == ((struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr){{
const char*_tmp3EE;void*_tmp3ED;(_tmp3ED=0,Cyc_fprintf(Cyc_stderr,((_tmp3EE="Internal error: copy_internal_file called with NULL\n",
_tag_dynforward(_tmp3EE,sizeof(char),_get_zero_arr_size_char(_tmp3EE,53)))),
_tag_dynforward(_tmp3ED,sizeof(void*),0)));}return 1;}{const char*_tmp3F0;const
char*_tmp3EF;struct Cyc___cycFILE*_tmp99=Cyc_try_file_open(file,((_tmp3EF="r",
_tag_dynforward(_tmp3EF,sizeof(char),_get_zero_arr_size_char(_tmp3EF,2)))),((
_tmp3F0="internal compiler file",_tag_dynforward(_tmp3F0,sizeof(char),
_get_zero_arr_size_char(_tmp3F0,23)))));if(_tmp99 == 0)return 1;{unsigned long
n_read=1024;unsigned long n_written;char buf[1024];{unsigned int _tmpA0=1024;
unsigned int i;for(i=0;i < _tmpA0;i ++){buf[i]='\000';}}while(n_read == 1024){n_read=
Cyc_fread(_tag_dynforward(buf,sizeof(char),1024),1,1024,(struct Cyc___cycFILE*)
_tmp99);if(n_read != 1024  && !Cyc_feof((struct Cyc___cycFILE*)_tmp99)){Cyc_compile_failure=
1;{const char*_tmp3F4;void*_tmp3F3[1];struct Cyc_String_pa_struct _tmp3F2;(_tmp3F2.tag=
0,((_tmp3F2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)file),((_tmp3F3[0]=&
_tmp3F2,Cyc_fprintf(Cyc_stderr,((_tmp3F4="\nError: problem copying %s\n",
_tag_dynforward(_tmp3F4,sizeof(char),_get_zero_arr_size_char(_tmp3F4,28)))),
_tag_dynforward(_tmp3F3,sizeof(void*),1)))))));}Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);return 1;}n_written=Cyc_fwrite(_tag_dynforward(buf,sizeof(char),1024),
1,n_read,out_file);if(n_read != n_written){Cyc_compile_failure=1;{const char*
_tmp3F8;void*_tmp3F7[1];struct Cyc_String_pa_struct _tmp3F6;(_tmp3F6.tag=0,((
_tmp3F6.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)file),((_tmp3F7[0]=&
_tmp3F6,Cyc_fprintf(Cyc_stderr,((_tmp3F8="\nError: problem copying %s\n",
_tag_dynforward(_tmp3F8,sizeof(char),_get_zero_arr_size_char(_tmp3F8,28)))),
_tag_dynforward(_tmp3F7,sizeof(void*),1)))))));}Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);return 1;}}Cyc_fclose((struct Cyc___cycFILE*)_tmp99);return 0;}}}struct
Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*out_file,struct Cyc_List_List*tds);
struct Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*out_file,struct Cyc_List_List*
tds){struct Cyc_Absynpp_Params _tmpA3=Cyc_tc_r?Cyc_Absynpp_cyc_params_r: Cyc_Absynpp_c_params_r;
_tmpA3.expand_typedefs=!Cyc_noexpand_r;_tmpA3.to_VC=Cyc_Cyclone_tovc_r;_tmpA3.add_cyc_prefix=
Cyc_add_cyc_namespace_r;_tmpA3.generate_line_directives=Cyc_generate_line_directives_r;
_tmpA3.print_full_evars=Cyc_print_full_evars_r;_tmpA3.print_all_tvars=Cyc_print_all_tvars_r;
_tmpA3.print_all_kinds=Cyc_print_all_kinds_r;_tmpA3.print_all_effects=Cyc_print_all_effects_r;
if(Cyc_inline_functions_r){const char*_tmp3FB;void*_tmp3FA;(_tmp3FA=0,Cyc_fprintf(
out_file,((_tmp3FB="#define _INLINE_FUNCTIONS\n",_tag_dynforward(_tmp3FB,sizeof(
char),_get_zero_arr_size_char(_tmp3FB,27)))),_tag_dynforward(_tmp3FA,sizeof(void*),
0)));}if(!Cyc_nocyc_setjmp_r){if(Cyc_copy_internal_file(Cyc_cyc_setjmp,out_file))
return tds;}if(Cyc_copy_internal_file(Cyc_cyc_include,out_file))return tds;if(Cyc_pp_r){
Cyc_Absynpp_set_params(& _tmpA3);Cyc_Absynpp_decllist2file(tds,out_file);}else{
Cyc_Absyndump_set_params(& _tmpA3);Cyc_Absyndump_dumpdecllist2file(tds,out_file);}
Cyc_fflush((struct Cyc___cycFILE*)out_file);return tds;}static struct Cyc_List_List*
Cyc_cfiles=0;static void Cyc_remove_cfiles();static void Cyc_remove_cfiles(){if(!Cyc_save_c_r)
for(0;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)_check_null(Cyc_cfiles))->tl){
Cyc_remove_file(*((struct _dynforward_ptr*)((struct Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}
static struct Cyc_List_List*Cyc_split_by_char(struct _dynforward_ptr s,char c);static
struct Cyc_List_List*Cyc_split_by_char(struct _dynforward_ptr s,char c){if(s.curr == ((
struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr)return 0;{struct Cyc_List_List*
_tmpA6=0;unsigned long _tmpA7=Cyc_strlen((struct _dynforward_ptr)s);while(_tmpA7 > 
0){struct _dynforward_ptr _tmpA8=Cyc_strchr(s,c);if(_tmpA8.curr == ((struct
_dynforward_ptr)_tag_dynforward(0,0,0)).curr){{struct _dynforward_ptr*_tmp3FE;
struct Cyc_List_List*_tmp3FD;_tmpA6=((_tmp3FD=_cycalloc(sizeof(*_tmp3FD)),((
_tmp3FD->hd=((_tmp3FE=_cycalloc(sizeof(*_tmp3FE)),((_tmp3FE[0]=s,_tmp3FE)))),((
_tmp3FD->tl=_tmpA6,_tmp3FD))))));}break;}else{{struct _dynforward_ptr*_tmp401;
struct Cyc_List_List*_tmp400;_tmpA6=((_tmp400=_cycalloc(sizeof(*_tmp400)),((
_tmp400->hd=((_tmp401=_cycalloc(sizeof(*_tmp401)),((_tmp401[0]=(struct
_dynforward_ptr)Cyc_substring((struct _dynforward_ptr)s,0,(unsigned long)((((
struct _dynforward_ptr)_tmpA8).curr - s.curr)/ sizeof(char))),_tmp401)))),((
_tmp400->tl=_tmpA6,_tmp400))))));}_tmpA7 -=(((struct _dynforward_ptr)_tmpA8).curr - 
s.curr)/ sizeof(char);s=_dynforward_ptr_plus(_tmpA8,sizeof(char),1);}}return((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpA6);}}
static int Cyc_file_exists(struct _dynforward_ptr file);static int Cyc_file_exists(
struct _dynforward_ptr file){struct Cyc___cycFILE*f=0;{struct _handler_cons _tmpAD;
_push_handler(& _tmpAD);{int _tmpAF=0;if(setjmp(_tmpAD.handler))_tmpAF=1;if(!
_tmpAF){{const char*_tmp402;f=(struct Cyc___cycFILE*)Cyc_file_open(file,((_tmp402="r",
_tag_dynforward(_tmp402,sizeof(char),_get_zero_arr_size_char(_tmp402,2)))));};
_pop_handler();}else{void*_tmpAE=(void*)_exn_thrown;void*_tmpB2=_tmpAE;_LL13:;
_LL14: goto _LL12;_LL15:;_LL16:(void)_throw(_tmpB2);_LL12:;}}}if(f == 0)return 0;
else{Cyc_fclose((struct Cyc___cycFILE*)f);return 1;}}static struct _dynforward_ptr
Cyc_sprint_list(struct Cyc_List_List*dirs);static struct _dynforward_ptr Cyc_sprint_list(
struct Cyc_List_List*dirs){const char*_tmp403;struct _dynforward_ptr tmp=(_tmp403="",
_tag_dynforward(_tmp403,sizeof(char),_get_zero_arr_size_char(_tmp403,1)));for(0;
dirs != 0;dirs=dirs->tl){struct _dynforward_ptr _tmpB3=*((struct _dynforward_ptr*)
dirs->hd);if(_tmpB3.curr == ((struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr
 || Cyc_strlen((struct _dynforward_ptr)_tmpB3)== 0)continue;{const char*_tmp404;
_tmpB3=(struct _dynforward_ptr)Cyc_strconcat((struct _dynforward_ptr)_tmpB3,((
_tmp404=":",_tag_dynforward(_tmp404,sizeof(char),_get_zero_arr_size_char(_tmp404,
2)))));}tmp=(struct _dynforward_ptr)Cyc_strconcat((struct _dynforward_ptr)_tmpB3,(
struct _dynforward_ptr)tmp);}return tmp;}static struct _dynforward_ptr*Cyc_find(
struct Cyc_List_List*dirs,struct _dynforward_ptr file);static struct _dynforward_ptr*
Cyc_find(struct Cyc_List_List*dirs,struct _dynforward_ptr file){if(file.curr == ((
struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr)return 0;for(0;dirs != 0;dirs=
dirs->tl){struct _dynforward_ptr _tmpB6=*((struct _dynforward_ptr*)dirs->hd);if(
_tmpB6.curr == ((struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr  || Cyc_strlen((
struct _dynforward_ptr)_tmpB6)== 0)continue;{struct _dynforward_ptr s=(struct
_dynforward_ptr)Cyc_Filename_concat(_tmpB6,file);if(Cyc_file_exists(s)){struct
_dynforward_ptr*_tmp405;return(_tmp405=_cycalloc(sizeof(*_tmp405)),((_tmp405[0]=
s,_tmp405)));}}}return 0;}static struct _dynforward_ptr Cyc_do_find(struct Cyc_List_List*
dirs,struct _dynforward_ptr file);static struct _dynforward_ptr Cyc_do_find(struct Cyc_List_List*
dirs,struct _dynforward_ptr file){struct _dynforward_ptr*_tmpB8=Cyc_find(dirs,file);
if(_tmpB8 == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();{const char*_tmp40A;void*
_tmp409[2];struct Cyc_String_pa_struct _tmp408;struct Cyc_String_pa_struct _tmp407;(
_tmp407.tag=0,((_tmp407.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_sprint_list(
dirs)),((_tmp408.tag=0,((_tmp408.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)file),((_tmp409[0]=& _tmp408,((_tmp409[1]=& _tmp407,Cyc_fprintf(
Cyc_stderr,((_tmp40A="Error: can't find internal compiler file %s in path %s\n",
_tag_dynforward(_tmp40A,sizeof(char),_get_zero_arr_size_char(_tmp40A,56)))),
_tag_dynforward(_tmp409,sizeof(void*),2)))))))))))));}Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);{struct Cyc_Core_Failure_struct _tmp41A;const char*_tmp419;void*_tmp418[
2];struct Cyc_String_pa_struct _tmp417;struct Cyc_String_pa_struct _tmp416;struct Cyc_Core_Failure_struct*
_tmp415;(int)_throw((void*)((_tmp415=_cycalloc(sizeof(*_tmp415)),((_tmp415[0]=((
_tmp41A.tag=Cyc_Core_Failure,((_tmp41A.f1=(struct _dynforward_ptr)((_tmp416.tag=0,((
_tmp416.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_sprint_list(dirs)),((
_tmp417.tag=0,((_tmp417.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)file),((
_tmp418[0]=& _tmp417,((_tmp418[1]=& _tmp416,Cyc_aprintf(((_tmp419="Error: can't find internal compiler file %s in path %s\n",
_tag_dynforward(_tmp419,sizeof(char),_get_zero_arr_size_char(_tmp419,56)))),
_tag_dynforward(_tmp418,sizeof(void*),2)))))))))))))),_tmp41A)))),_tmp415)))));}}
return*_tmpB8;}static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,
struct _dynforward_ptr subdir);static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*
dirs,struct _dynforward_ptr subdir){struct Cyc_List_List*_tmpC3=0;for(0;dirs != 0;
dirs=dirs->tl){struct _dynforward_ptr*_tmp41D;struct Cyc_List_List*_tmp41C;_tmpC3=((
_tmp41C=_cycalloc(sizeof(*_tmp41C)),((_tmp41C->hd=((_tmp41D=_cycalloc(sizeof(*
_tmp41D)),((_tmp41D[0]=(struct _dynforward_ptr)Cyc_Filename_concat(*((struct
_dynforward_ptr*)dirs->hd),subdir),_tmp41D)))),((_tmp41C->tl=_tmpC3,_tmp41C))))));}
_tmpC3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpC3);
return _tmpC3;}static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*dirs,
struct _dynforward_ptr subdir);static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*
dirs,struct _dynforward_ptr subdir){struct Cyc_List_List*_tmpC6=0;for(0;dirs != 0;
dirs=dirs->tl){{struct Cyc_List_List*_tmp41E;_tmpC6=((_tmp41E=_cycalloc(sizeof(*
_tmp41E)),((_tmp41E->hd=(struct _dynforward_ptr*)dirs->hd,((_tmp41E->tl=_tmpC6,
_tmp41E))))));}{struct _dynforward_ptr*_tmp421;struct Cyc_List_List*_tmp420;_tmpC6=((
_tmp420=_cycalloc(sizeof(*_tmp420)),((_tmp420->hd=((_tmp421=_cycalloc(sizeof(*
_tmp421)),((_tmp421[0]=(struct _dynforward_ptr)Cyc_Filename_concat(*((struct
_dynforward_ptr*)dirs->hd),subdir),_tmp421)))),((_tmp420->tl=_tmpC6,_tmp420))))));}}
_tmpC6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpC6);
return _tmpC6;}static int Cyc_is_other_special(char c);static int Cyc_is_other_special(
char c){switch(c){case '\\': _LL17: goto _LL18;case '"': _LL18: goto _LL19;case ';': _LL19:
goto _LL1A;case '&': _LL1A: goto _LL1B;case '(': _LL1B: goto _LL1C;case ')': _LL1C: goto
_LL1D;case '|': _LL1D: goto _LL1E;case '^': _LL1E: goto _LL1F;case '<': _LL1F: goto _LL20;
case '>': _LL20: goto _LL21;case ' ': _LL21: goto _LL22;case '\n': _LL22: goto _LL23;case '\t':
_LL23: return 1;default: _LL24: return 0;}}static struct _dynforward_ptr Cyc_sh_escape_string(
struct _dynforward_ptr s);static void _tmp42A(unsigned int*_tmp429,unsigned int*
_tmp428,char**_tmp426){for(*_tmp429=0;*_tmp429 < *_tmp428;(*_tmp429)++){(*_tmp426)[*
_tmp429]='\000';}}static struct _dynforward_ptr Cyc_sh_escape_string(struct
_dynforward_ptr s){unsigned long _tmpCA=Cyc_strlen((struct _dynforward_ptr)s);int
_tmpCB=0;int _tmpCC=0;{int i=0;for(0;i < _tmpCA;++ i){char _tmpCD=*((const char*)
_check_dynforward_subscript(s,sizeof(char),i));if(_tmpCD == '\'')++ _tmpCB;else{
if(Cyc_is_other_special(_tmpCD))++ _tmpCC;}}}if(_tmpCB == 0  && _tmpCC == 0)return s;
if(_tmpCB == 0){struct _dynforward_ptr*_tmp424;struct _dynforward_ptr*_tmp423[3];
return(struct _dynforward_ptr)Cyc_strconcat_l(((_tmp423[2]=_init_dynforward_ptr(
_cycalloc(sizeof(struct _dynforward_ptr)),"'",sizeof(char),2),((_tmp423[1]=((
_tmp424=_cycalloc(sizeof(*_tmp424)),((_tmp424[0]=(struct _dynforward_ptr)s,
_tmp424)))),((_tmp423[0]=_init_dynforward_ptr(_cycalloc(sizeof(struct
_dynforward_ptr)),"'",sizeof(char),2),((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp423,sizeof(struct
_dynforward_ptr*),3)))))))));}{unsigned long _tmpD2=(_tmpCA + _tmpCB)+ _tmpCC;
unsigned int _tmp429;unsigned int _tmp428;struct _dynforward_ptr _tmp427;char*
_tmp426;unsigned int _tmp425;struct _dynforward_ptr s2=(_tmp425=_tmpD2 + 1,((_tmp426=(
char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp425 + 1)),((_tmp427=
_tag_dynforward(_tmp426,sizeof(char),_tmp425 + 1),((((_tmp428=_tmp425,((_tmp42A(&
_tmp429,& _tmp428,& _tmp426),_tmp426[_tmp428]=(char)0)))),_tmp427)))))));int _tmpD3=
0;int _tmpD4=0;for(0;_tmpD3 < _tmpCA;++ _tmpD3){char _tmpD5=*((const char*)
_check_dynforward_subscript(s,sizeof(char),_tmpD3));if(_tmpD5 == '\''  || Cyc_is_other_special(
_tmpD5)){char _tmp42D;char _tmp42C;struct _dynforward_ptr _tmp42B;(_tmp42B=
_dynforward_ptr_plus(s2,sizeof(char),_tmpD4 ++),((_tmp42C=*((char*)
_check_dynforward_subscript(_tmp42B,sizeof(char),0)),((_tmp42D='\\',((
_get_dynforward_size(_tmp42B,sizeof(char))== 1  && (_tmp42C == '\000'  && _tmp42D != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp42B.curr)=_tmp42D)))))));}{char _tmp430;char
_tmp42F;struct _dynforward_ptr _tmp42E;(_tmp42E=_dynforward_ptr_plus(s2,sizeof(
char),_tmpD4 ++),((_tmp42F=*((char*)_check_dynforward_subscript(_tmp42E,sizeof(
char),0)),((_tmp430=_tmpD5,((_get_dynforward_size(_tmp42E,sizeof(char))== 1  && (
_tmp42F == '\000'  && _tmp430 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp42E.curr)=
_tmp430)))))));}}return(struct _dynforward_ptr)s2;}}static struct _dynforward_ptr*
Cyc_sh_escape_stringptr(struct _dynforward_ptr*sp);static struct _dynforward_ptr*
Cyc_sh_escape_stringptr(struct _dynforward_ptr*sp){struct _dynforward_ptr*_tmp431;
return(_tmp431=_cycalloc(sizeof(*_tmp431)),((_tmp431[0]=Cyc_sh_escape_string(*sp),
_tmp431)));}static void Cyc_process_file(struct _dynforward_ptr filename);static void
Cyc_process_file(struct _dynforward_ptr filename){struct _dynforward_ptr _tmpE1=Cyc_make_base_filename(
filename,Cyc_output_file);const char*_tmp432;struct _dynforward_ptr _tmpE2=Cyc_strconcat((
struct _dynforward_ptr)_tmpE1,((_tmp432=".cyp",_tag_dynforward(_tmp432,sizeof(
char),_get_zero_arr_size_char(_tmp432,5)))));const char*_tmp433;struct
_dynforward_ptr _tmpE3=Cyc_strconcat((struct _dynforward_ptr)_tmpE1,((_tmp433=".cyci",
_tag_dynforward(_tmp433,sizeof(char),_get_zero_arr_size_char(_tmp433,6)))));
const char*_tmp434;struct _dynforward_ptr _tmpE4=Cyc_strconcat((struct
_dynforward_ptr)_tmpE1,((_tmp434=".cycio",_tag_dynforward(_tmp434,sizeof(char),
_get_zero_arr_size_char(_tmp434,7)))));const char*_tmp435;struct _dynforward_ptr
_tmpE5=Cyc_strconcat((struct _dynforward_ptr)_tmpE1,((_tmp435=".c",
_tag_dynforward(_tmp435,sizeof(char),_get_zero_arr_size_char(_tmp435,3)))));if(
Cyc_v_r){const char*_tmp439;void*_tmp438[1];struct Cyc_String_pa_struct _tmp437;(
_tmp437.tag=0,((_tmp437.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
filename),((_tmp438[0]=& _tmp437,Cyc_fprintf(Cyc_stderr,((_tmp439="Compiling %s\n",
_tag_dynforward(_tmp439,sizeof(char),_get_zero_arr_size_char(_tmp439,14)))),
_tag_dynforward(_tmp438,sizeof(void*),1)))))));}{const char*_tmp43B;const char*
_tmp43A;struct Cyc___cycFILE*f0=Cyc_try_file_open(filename,((_tmp43A="r",
_tag_dynforward(_tmp43A,sizeof(char),_get_zero_arr_size_char(_tmp43A,2)))),((
_tmp43B="input file",_tag_dynforward(_tmp43B,sizeof(char),
_get_zero_arr_size_char(_tmp43B,11)))));if(Cyc_compile_failure  || !((
unsigned int)f0))return;Cyc_fclose((struct Cyc___cycFILE*)f0);{const char*_tmp442;
struct _dynforward_ptr*_tmp441;const char*_tmp440;struct Cyc_List_List*_tmp43F;
struct _dynforward_ptr _tmpE9=Cyc_str_sepstr(((_tmp43F=_cycalloc(sizeof(*_tmp43F)),((
_tmp43F->hd=((_tmp441=_cycalloc(sizeof(*_tmp441)),((_tmp441[0]=(struct
_dynforward_ptr)((_tmp440="",_tag_dynforward(_tmp440,sizeof(char),
_get_zero_arr_size_char(_tmp440,1)))),_tmp441)))),((_tmp43F->tl=((struct Cyc_List_List*(*)(
struct _dynforward_ptr*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_cppargs)),_tmp43F)))))),((_tmp442=" ",_tag_dynforward(_tmp442,sizeof(char),
_get_zero_arr_size_char(_tmp442,2)))));struct _dynforward_ptr def_inc_path=(struct
_dynforward_ptr)Cstring_to_string(Cdef_inc_path);struct Cyc_List_List*_tmpEA=Cyc_add_subdir(
Cyc_cyclone_exec_path,Cyc_target_arch);{const char*_tmp443;_tmpEA=Cyc_add_subdir(
_tmpEA,((_tmp443="include",_tag_dynforward(_tmp443,sizeof(char),
_get_zero_arr_size_char(_tmp443,8)))));}if(Cyc_strlen((struct _dynforward_ptr)
def_inc_path)> 0){struct _dynforward_ptr*_tmp446;struct Cyc_List_List*_tmp445;
_tmpEA=((_tmp445=_cycalloc(sizeof(*_tmp445)),((_tmp445->hd=((_tmp446=_cycalloc(
sizeof(*_tmp446)),((_tmp446[0]=def_inc_path,_tmp446)))),((_tmp445->tl=_tmpEA,
_tmp445))))));}{char*_tmpEE=getenv((const char*)"CYCLONE_INCLUDE_PATH");if((char*)
_tmpEE != 0){char*_tmp447;_tmpEA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(Cyc_split_by_char(((_tmp447=_tmpEE,
_tag_dynforward(_tmp447,sizeof(char),_get_zero_arr_size_char(_tmp447,1)))),':'),
_tmpEA);}{const char*_tmp44E;struct _dynforward_ptr*_tmp44D;const char*_tmp44C;
struct Cyc_List_List*_tmp44B;struct _dynforward_ptr stdinc_string=(struct
_dynforward_ptr)Cyc_str_sepstr(((_tmp44B=_cycalloc(sizeof(*_tmp44B)),((_tmp44B->hd=((
_tmp44D=_cycalloc(sizeof(*_tmp44D)),((_tmp44D[0]=(struct _dynforward_ptr)((
_tmp44C="",_tag_dynforward(_tmp44C,sizeof(char),_get_zero_arr_size_char(_tmp44C,
1)))),_tmp44D)))),((_tmp44B->tl=((struct Cyc_List_List*(*)(struct _dynforward_ptr*(*
f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,
_tmpEA),_tmp44B)))))),((_tmp44E=" -I",_tag_dynforward(_tmp44E,sizeof(char),
_get_zero_arr_size_char(_tmp44E,4)))));struct _dynforward_ptr ofile_string;if(Cyc_stop_after_cpp_r){
if(Cyc_output_file != 0){const char*_tmp452;void*_tmp451[1];struct Cyc_String_pa_struct
_tmp450;ofile_string=(struct _dynforward_ptr)((_tmp450.tag=0,((_tmp450.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*((struct _dynforward_ptr*)_check_null(
Cyc_output_file))),((_tmp451[0]=& _tmp450,Cyc_aprintf(((_tmp452=" > %s",
_tag_dynforward(_tmp452,sizeof(char),_get_zero_arr_size_char(_tmp452,6)))),
_tag_dynforward(_tmp451,sizeof(void*),1))))))));}else{const char*_tmp453;
ofile_string=((_tmp453="",_tag_dynforward(_tmp453,sizeof(char),
_get_zero_arr_size_char(_tmp453,1))));}}else{const char*_tmp457;void*_tmp456[1];
struct Cyc_String_pa_struct _tmp455;ofile_string=(struct _dynforward_ptr)((_tmp455.tag=
0,((_tmp455.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_sh_escape_string((
struct _dynforward_ptr)_tmpE2)),((_tmp456[0]=& _tmp455,Cyc_aprintf(((_tmp457=" > %s",
_tag_dynforward(_tmp457,sizeof(char),_get_zero_arr_size_char(_tmp457,6)))),
_tag_dynforward(_tmp456,sizeof(void*),1))))))));}{struct _dynforward_ptr
fixup_string;if(Cyc_produce_dependencies){if(Cyc_dependencies_target == 0){const
char*_tmp458;fixup_string=((_tmp458=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",
_tag_dynforward(_tmp458,sizeof(char),_get_zero_arr_size_char(_tmp458,35))));}
else{const char*_tmp45C;void*_tmp45B[1];struct Cyc_String_pa_struct _tmp45A;
fixup_string=(struct _dynforward_ptr)((_tmp45A.tag=0,((_tmp45A.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*((struct _dynforward_ptr*)_check_null(
Cyc_dependencies_target))),((_tmp45B[0]=& _tmp45A,Cyc_aprintf(((_tmp45C=" | sed 's/^.*\\.cyc\\.o:/%s:/'",
_tag_dynforward(_tmp45C,sizeof(char),_get_zero_arr_size_char(_tmp45C,29)))),
_tag_dynforward(_tmp45B,sizeof(void*),1))))))));}}else{const char*_tmp45D;
fixup_string=((_tmp45D="",_tag_dynforward(_tmp45D,sizeof(char),
_get_zero_arr_size_char(_tmp45D,1))));}{const char*_tmp466;void*_tmp465[6];struct
Cyc_String_pa_struct _tmp464;struct Cyc_String_pa_struct _tmp463;struct Cyc_String_pa_struct
_tmp462;struct Cyc_String_pa_struct _tmp461;struct Cyc_String_pa_struct _tmp460;
struct Cyc_String_pa_struct _tmp45F;struct _dynforward_ptr _tmpFC=(_tmp45F.tag=0,((
_tmp45F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)ofile_string),((
_tmp460.tag=0,((_tmp460.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
fixup_string),((_tmp461.tag=0,((_tmp461.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_sh_escape_string(filename)),((_tmp462.tag=0,((_tmp462.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)stdinc_string),((_tmp463.tag=0,((
_tmp463.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmpE9),((_tmp464.tag=
0,((_tmp464.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_cpp),((_tmp465[
0]=& _tmp464,((_tmp465[1]=& _tmp463,((_tmp465[2]=& _tmp462,((_tmp465[3]=& _tmp461,((
_tmp465[4]=& _tmp460,((_tmp465[5]=& _tmp45F,Cyc_aprintf(((_tmp466="%s %s%s %s%s%s",
_tag_dynforward(_tmp466,sizeof(char),_get_zero_arr_size_char(_tmp466,15)))),
_tag_dynforward(_tmp465,sizeof(void*),6)))))))))))))))))))))))))))))))))))));if(
Cyc_v_r){const char*_tmp46A;void*_tmp469[1];struct Cyc_String_pa_struct _tmp468;(
_tmp468.tag=0,((_tmp468.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmpFC),((
_tmp469[0]=& _tmp468,Cyc_fprintf(Cyc_stderr,((_tmp46A="%s\n",_tag_dynforward(
_tmp46A,sizeof(char),_get_zero_arr_size_char(_tmp46A,4)))),_tag_dynforward(
_tmp469,sizeof(void*),1)))))));}if(system((const char*)_untag_dynforward_ptr(
_tmpFC,sizeof(char),1))!= 0){Cyc_compile_failure=1;{const char*_tmp46D;void*
_tmp46C;(_tmp46C=0,Cyc_fprintf(Cyc_stderr,((_tmp46D="\nError: preprocessing\n",
_tag_dynforward(_tmp46D,sizeof(char),_get_zero_arr_size_char(_tmp46D,23)))),
_tag_dynforward(_tmp46C,sizeof(void*),0)));}return;}if(Cyc_stop_after_cpp_r)
return;Cyc_Position_reset_position((struct _dynforward_ptr)_tmpE2);{const char*
_tmp46F;const char*_tmp46E;struct Cyc___cycFILE*in_file=Cyc_try_file_open((struct
_dynforward_ptr)_tmpE2,((_tmp46E="r",_tag_dynforward(_tmp46E,sizeof(char),
_get_zero_arr_size_char(_tmp46E,2)))),((_tmp46F="file",_tag_dynforward(_tmp46F,
sizeof(char),_get_zero_arr_size_char(_tmp46F,5)))));if(Cyc_compile_failure)
return;{struct Cyc_List_List*tds=0;{struct _handler_cons _tmp102;_push_handler(&
_tmp102);{int _tmp104=0;if(setjmp(_tmp102.handler))_tmp104=1;if(!_tmp104){{const
char*_tmp470;tds=((struct Cyc_List_List*(*)(struct _dynforward_ptr stage_name,
struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*
env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp470="parsing",_tag_dynforward(
_tmp470,sizeof(char),_get_zero_arr_size_char(_tmp470,8)))),Cyc_do_parse,(struct
Cyc___cycFILE*)_check_null(in_file),tds);};_pop_handler();}else{void*_tmp103=(
void*)_exn_thrown;void*_tmp107=_tmp103;_LL27:;_LL28: Cyc_file_close((struct Cyc___cycFILE*)
_check_null(in_file));Cyc_remove_file((struct _dynforward_ptr)_tmpE2);(int)_throw(
_tmp107);_LL29:;_LL2A:(void)_throw(_tmp107);_LL26:;}}}Cyc_file_close((struct Cyc___cycFILE*)
in_file);if(Cyc_compile_failure){Cyc_remove_file((struct _dynforward_ptr)_tmpE2);
return;}if(Cyc_Absyn_porting_c_code){Cyc_Port_port(tds);return;}{struct
_RegionHandle _tmp108=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp108;
_push_region(tc_rgn);{struct Cyc_Tcenv_Tenv*_tmp109=Cyc_Tcenv_tc_init(tc_rgn);if(
Cyc_parseonly_r){_npop_handler(0);goto PRINTC;}{struct _tuple13 _tmp471;struct
_tuple13 _tmp10A=(_tmp471.f1=tc_rgn,((_tmp471.f2=_tmp109,_tmp471)));{const char*
_tmp472;tds=((struct Cyc_List_List*(*)(struct _dynforward_ptr stage_name,struct Cyc_List_List*(*
f)(struct _tuple13*,struct Cyc_List_List*),struct _tuple13*env,struct Cyc_List_List*
tds))Cyc_do_stage)(((_tmp472="type checking",_tag_dynforward(_tmp472,sizeof(char),
_get_zero_arr_size_char(_tmp472,14)))),Cyc_do_typecheck,& _tmp10A,tds);}if(Cyc_compile_failure){
Cyc_remove_file((struct _dynforward_ptr)_tmpE2);_npop_handler(0);return;}if(Cyc_tc_r){
_npop_handler(0);goto PRINTC;}{const char*_tmp473;tds=((struct Cyc_List_List*(*)(
struct _dynforward_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),
int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp473="control-flow checking",
_tag_dynforward(_tmp473,sizeof(char),_get_zero_arr_size_char(_tmp473,22)))),Cyc_do_cfcheck,
1,tds);}if(Cyc_compile_failure){Cyc_remove_file((struct _dynforward_ptr)_tmpE2);
_npop_handler(0);return;}if(Cyc_ic_r){struct Cyc___cycFILE*inter_file=Cyc_fopen((
const char*)((char*)_untag_dynforward_ptr(_tmpE3,sizeof(char),1)),(const char*)"r");
const char*_tmp475;const char*_tmp474;struct Cyc___cycFILE*inter_objfile=Cyc_try_file_open((
struct _dynforward_ptr)_tmpE4,((_tmp474="w",_tag_dynforward(_tmp474,sizeof(char),
_get_zero_arr_size_char(_tmp474,2)))),((_tmp475="interface object file",
_tag_dynforward(_tmp475,sizeof(char),_get_zero_arr_size_char(_tmp475,22)))));if(
inter_objfile == 0){Cyc_compile_failure=1;_npop_handler(0);return;}Cyc_Position_reset_position((
struct _dynforward_ptr)_tmpE3);{struct _tuple14 _tmp476;struct _tuple14 _tmp10D=(
_tmp476.f1=_tmp109,((_tmp476.f2=inter_file,((_tmp476.f3=(struct Cyc___cycFILE*)
inter_objfile,_tmp476)))));{const char*_tmp477;tds=((struct Cyc_List_List*(*)(
struct _dynforward_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple14*,struct
Cyc_List_List*),struct _tuple14*env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp477="interface checking",_tag_dynforward(_tmp477,sizeof(char),
_get_zero_arr_size_char(_tmp477,19)))),Cyc_do_interface,& _tmp10D,tds);}if(
inter_file != 0)Cyc_file_close((struct Cyc___cycFILE*)inter_file);Cyc_file_close((
struct Cyc___cycFILE*)inter_objfile);}}}};_pop_region(tc_rgn);}if(Cyc_cf_r)goto
PRINTC;{const char*_tmp478;tds=((struct Cyc_List_List*(*)(struct _dynforward_ptr
stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*
tds))Cyc_do_stage)(((_tmp478="translation to C",_tag_dynforward(_tmp478,sizeof(
char),_get_zero_arr_size_char(_tmp478,17)))),Cyc_do_translate,1,tds);}if(Cyc_compile_failure){
Cyc_remove_file((struct _dynforward_ptr)_tmpE2);return;}if(!Cyc_compile_failure)
Cyc_Tcutil_flush_warnings();Cyc_remove_file((struct _dynforward_ptr)_tmpE2);if(
Cyc_compile_failure)return;if(Cyc_Cyclone_tovc_r  || Cyc_elim_se_r){const char*
_tmp479;tds=((struct Cyc_List_List*(*)(struct _dynforward_ptr stage_name,struct Cyc_List_List*(*
f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((
_tmp479="post-pass to VC",_tag_dynforward(_tmp479,sizeof(char),
_get_zero_arr_size_char(_tmp479,16)))),Cyc_do_tovc,1,tds);}if(Cyc_compile_failure)
return;PRINTC: {struct Cyc___cycFILE*out_file;if((Cyc_parseonly_r  || Cyc_tc_r)
 || Cyc_cf_r){if(Cyc_output_file != 0){const char*_tmp47B;const char*_tmp47A;
out_file=Cyc_try_file_open(*((struct _dynforward_ptr*)_check_null(Cyc_output_file)),((
_tmp47A="w",_tag_dynforward(_tmp47A,sizeof(char),_get_zero_arr_size_char(_tmp47A,
2)))),((_tmp47B="output file",_tag_dynforward(_tmp47B,sizeof(char),
_get_zero_arr_size_char(_tmp47B,12)))));}else{out_file=(struct Cyc___cycFILE*)Cyc_stdout;}}
else{if(Cyc_toc_r  && Cyc_output_file != 0){const char*_tmp47D;const char*_tmp47C;
out_file=Cyc_try_file_open(*((struct _dynforward_ptr*)_check_null(Cyc_output_file)),((
_tmp47C="w",_tag_dynforward(_tmp47C,sizeof(char),_get_zero_arr_size_char(_tmp47C,
2)))),((_tmp47D="output file",_tag_dynforward(_tmp47D,sizeof(char),
_get_zero_arr_size_char(_tmp47D,12)))));}else{const char*_tmp47F;const char*
_tmp47E;out_file=Cyc_try_file_open((struct _dynforward_ptr)_tmpE5,((_tmp47E="w",
_tag_dynforward(_tmp47E,sizeof(char),_get_zero_arr_size_char(_tmp47E,2)))),((
_tmp47F="output file",_tag_dynforward(_tmp47F,sizeof(char),
_get_zero_arr_size_char(_tmp47F,12)))));}}if(Cyc_compile_failure  || !((
unsigned int)out_file))return;{struct _handler_cons _tmp11B;_push_handler(& _tmp11B);{
int _tmp11D=0;if(setjmp(_tmp11B.handler))_tmp11D=1;if(!_tmp11D){if(!Cyc_noprint_r){
const char*_tmp480;tds=((struct Cyc_List_List*(*)(struct _dynforward_ptr stage_name,
struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*
env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp480="printing",_tag_dynforward(
_tmp480,sizeof(char),_get_zero_arr_size_char(_tmp480,9)))),Cyc_do_print,(struct
Cyc___cycFILE*)out_file,tds);};_pop_handler();}else{void*_tmp11C=(void*)
_exn_thrown;void*_tmp120=_tmp11C;_LL2C:;_LL2D: Cyc_compile_failure=1;Cyc_file_close((
struct Cyc___cycFILE*)out_file);{struct _dynforward_ptr*_tmp483;struct Cyc_List_List*
_tmp482;Cyc_cfiles=((_tmp482=_cycalloc(sizeof(*_tmp482)),((_tmp482->hd=((_tmp483=
_cycalloc(sizeof(*_tmp483)),((_tmp483[0]=(struct _dynforward_ptr)_tmpE5,_tmp483)))),((
_tmp482->tl=Cyc_cfiles,_tmp482))))));}(int)_throw(_tmp120);_LL2E:;_LL2F:(void)
_throw(_tmp120);_LL2B:;}}}Cyc_file_close((struct Cyc___cycFILE*)out_file);{struct
_dynforward_ptr*_tmp486;struct Cyc_List_List*_tmp485;Cyc_cfiles=((_tmp485=
_cycalloc(sizeof(*_tmp485)),((_tmp485->hd=((_tmp486=_cycalloc(sizeof(*_tmp486)),((
_tmp486[0]=(struct _dynforward_ptr)_tmpE5,_tmp486)))),((_tmp485->tl=Cyc_cfiles,
_tmp485))))));}}}}}}}}}}}static char _tmp13D[8]="<final>";static struct
_dynforward_ptr Cyc_final_str={_tmp13D,_tmp13D + 8};static struct _dynforward_ptr*
Cyc_final_strptr=& Cyc_final_str;static struct Cyc_Interface_I*Cyc_read_cycio(
struct _dynforward_ptr*n);static struct Cyc_Interface_I*Cyc_read_cycio(struct
_dynforward_ptr*n){if(n == (struct _dynforward_ptr*)Cyc_final_strptr)return Cyc_Interface_final();{
struct _dynforward_ptr basename;{struct _handler_cons _tmp13E;_push_handler(& _tmp13E);{
int _tmp140=0;if(setjmp(_tmp13E.handler))_tmp140=1;if(!_tmp140){basename=(struct
_dynforward_ptr)Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*
_tmp13F=(void*)_exn_thrown;void*_tmp142=_tmp13F;_LL31: if(*((void**)_tmp142)!= 
Cyc_Core_Invalid_argument)goto _LL33;_LL32: basename=*n;goto _LL30;_LL33:;_LL34:(
void)_throw(_tmp142);_LL30:;}}}{const char*_tmp487;struct _dynforward_ptr _tmp143=
Cyc_strconcat((struct _dynforward_ptr)basename,((_tmp487=".cycio",_tag_dynforward(
_tmp487,sizeof(char),_get_zero_arr_size_char(_tmp487,7)))));const char*_tmp489;
const char*_tmp488;struct Cyc___cycFILE*_tmp144=Cyc_try_file_open((struct
_dynforward_ptr)_tmp143,((_tmp488="rb",_tag_dynforward(_tmp488,sizeof(char),
_get_zero_arr_size_char(_tmp488,3)))),((_tmp489="interface object file",
_tag_dynforward(_tmp489,sizeof(char),_get_zero_arr_size_char(_tmp489,22)))));if(
_tmp144 == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();exit(1);}Cyc_Position_reset_position((
struct _dynforward_ptr)_tmp143);{struct Cyc_Interface_I*_tmp145=Cyc_Interface_load((
struct Cyc___cycFILE*)_tmp144);Cyc_file_close((struct Cyc___cycFILE*)_tmp144);
return _tmp145;}}}}static int Cyc_is_cfile(struct _dynforward_ptr*n);static int Cyc_is_cfile(
struct _dynforward_ptr*n){return*((const char*)_check_dynforward_subscript(*n,
sizeof(char),0))!= '-';}void GC_blacklist_warn_clear();struct _tuple15{struct
_dynforward_ptr f1;int f2;struct _dynforward_ptr f3;void*f4;struct _dynforward_ptr f5;
};int Cyc_main(int argc,struct _dynforward_ptr argv);static void _tmp48F(unsigned int*
_tmp48E,unsigned int*_tmp48D,int**_tmp48B){for(*_tmp48E=0;*_tmp48E < *_tmp48D;(*
_tmp48E)++){(*_tmp48B)[*_tmp48E]=0;}}static void _tmp497(unsigned int*_tmp496,
unsigned int*_tmp495,struct _dynforward_ptr**_tmp493){for(*_tmp496=0;*_tmp496 < *
_tmp495;(*_tmp496)++){(*_tmp493)[*_tmp496]=(struct _dynforward_ptr)
_tag_dynforward(0,0,0);}}static void _tmp49D(unsigned int*_tmp49C,unsigned int*
_tmp49B,struct _dynforward_ptr**_tmp499){for(*_tmp49C=0;*_tmp49C < *_tmp49B;(*
_tmp49C)++){(*_tmp499)[*_tmp49C]=(struct _dynforward_ptr)_tag_dynforward(0,0,0);}}
int Cyc_main(int argc,struct _dynforward_ptr argv){GC_blacklist_warn_clear();{struct
Cyc_List_List*cyclone_arch_path;struct _dynforward_ptr def_lib_path;struct
_dynforward_ptr comp=(struct _dynforward_ptr)Cstring_to_string(Ccomp);{struct
_RegionHandle _tmp149=_new_region("r");struct _RegionHandle*r=& _tmp149;
_push_region(r);{unsigned int _tmp48E;unsigned int _tmp48D;struct _dynforward_ptr
_tmp48C;int*_tmp48B;unsigned int _tmp48A;struct _dynforward_ptr _tmp14A=(_tmp48A=(
unsigned int)argc,((_tmp48B=(int*)_cycalloc_atomic(_check_times(sizeof(int),
_tmp48A)),((_tmp48C=_tag_dynforward(_tmp48B,sizeof(int),_tmp48A),((((_tmp48D=
_tmp48A,_tmp48F(& _tmp48E,& _tmp48D,& _tmp48B))),_tmp48C)))))));int _tmp14B=0;int i;
int j;int k;for(i=1;i < argc;++ i){const char*_tmp490;if(Cyc_strncmp(((_tmp490="-B",
_tag_dynforward(_tmp490,sizeof(char),_get_zero_arr_size_char(_tmp490,3)))),(
struct _dynforward_ptr)*((struct _dynforward_ptr*)_check_dynforward_subscript(argv,
sizeof(struct _dynforward_ptr),i)),2)== 0){*((int*)_check_dynforward_subscript(
_tmp14A,sizeof(int),i))=1;++ _tmp14B;}else{const char*_tmp491;if(Cyc_strcmp(((
_tmp491="-b",_tag_dynforward(_tmp491,sizeof(char),_get_zero_arr_size_char(
_tmp491,3)))),(struct _dynforward_ptr)*((struct _dynforward_ptr*)
_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),i)))== 0){*((int*)
_check_dynforward_subscript(_tmp14A,sizeof(int),i))=1;++ _tmp14B;++ i;if(i >= argc)
break;*((int*)_check_dynforward_subscript(_tmp14A,sizeof(int),i))=1;++ _tmp14B;}}}{
unsigned int _tmp496;unsigned int _tmp495;struct _dynforward_ptr _tmp494;struct
_dynforward_ptr*_tmp493;unsigned int _tmp492;struct _dynforward_ptr _tmp14E=(
_tmp492=(unsigned int)(_tmp14B + 1),((_tmp493=(struct _dynforward_ptr*)_cycalloc(
_check_times(sizeof(struct _dynforward_ptr),_tmp492)),((_tmp494=_tag_dynforward(
_tmp493,sizeof(struct _dynforward_ptr),_tmp492),((((_tmp495=_tmp492,_tmp497(&
_tmp496,& _tmp495,& _tmp493))),_tmp494)))))));unsigned int _tmp49C;unsigned int
_tmp49B;struct _dynforward_ptr _tmp49A;struct _dynforward_ptr*_tmp499;unsigned int
_tmp498;struct _dynforward_ptr _tmp14F=(_tmp498=(unsigned int)(argc - _tmp14B),((
_tmp499=(struct _dynforward_ptr*)_cycalloc(_check_times(sizeof(struct
_dynforward_ptr),_tmp498)),((_tmp49A=_tag_dynforward(_tmp499,sizeof(struct
_dynforward_ptr),_tmp498),((((_tmp49B=_tmp498,_tmp49D(& _tmp49C,& _tmp49B,& _tmp499))),
_tmp49A)))))));*((struct _dynforward_ptr*)_check_dynforward_subscript(_tmp14E,
sizeof(struct _dynforward_ptr),0))=(*((struct _dynforward_ptr*)
_check_dynforward_subscript(_tmp14F,sizeof(struct _dynforward_ptr),0))=*((struct
_dynforward_ptr*)_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),
0)));for(i=(j=(k=1));i < argc;++ i){if(*((int*)_check_dynforward_subscript(_tmp14A,
sizeof(int),i)))*((struct _dynforward_ptr*)_check_dynforward_subscript(_tmp14E,
sizeof(struct _dynforward_ptr),j ++))=*((struct _dynforward_ptr*)
_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),i));else{*((
struct _dynforward_ptr*)_check_dynforward_subscript(_tmp14F,sizeof(struct
_dynforward_ptr),k ++))=*((struct _dynforward_ptr*)_check_dynforward_subscript(
argv,sizeof(struct _dynforward_ptr),i));}}{struct _tuple15*_tmp91E;const char*
_tmp91D;struct Cyc_Arg_Set_spec_struct _tmp91C;struct Cyc_Arg_Set_spec_struct*
_tmp91B;const char*_tmp91A;const char*_tmp919;struct _tuple15*_tmp918;const char*
_tmp917;struct Cyc_Arg_Unit_spec_struct _tmp916;struct Cyc_Arg_Unit_spec_struct*
_tmp915;const char*_tmp914;const char*_tmp913;struct _tuple15*_tmp912;const char*
_tmp911;struct Cyc_Arg_Clear_spec_struct _tmp910;struct Cyc_Arg_Clear_spec_struct*
_tmp90F;const char*_tmp90E;const char*_tmp90D;struct _tuple15*_tmp90C;const char*
_tmp90B;struct Cyc_Arg_Set_spec_struct _tmp90A;struct Cyc_Arg_Set_spec_struct*
_tmp909;const char*_tmp908;const char*_tmp907;struct _tuple15*_tmp906;const char*
_tmp905;struct Cyc_Arg_Set_spec_struct _tmp904;struct Cyc_Arg_Set_spec_struct*
_tmp903;const char*_tmp902;const char*_tmp901;struct _tuple15*_tmp900;const char*
_tmp8FF;struct Cyc_Arg_Set_spec_struct _tmp8FE;struct Cyc_Arg_Set_spec_struct*
_tmp8FD;const char*_tmp8FC;const char*_tmp8FB;struct _tuple15*_tmp8FA;const char*
_tmp8F9;struct Cyc_Arg_String_spec_struct _tmp8F8;struct Cyc_Arg_String_spec_struct*
_tmp8F7;const char*_tmp8F6;const char*_tmp8F5;struct _tuple15*_tmp8F4;const char*
_tmp8F3;struct Cyc_Arg_Set_spec_struct _tmp8F2;struct Cyc_Arg_Set_spec_struct*
_tmp8F1;const char*_tmp8F0;const char*_tmp8EF;struct _tuple15*_tmp8EE;const char*
_tmp8ED;struct Cyc_Arg_Set_spec_struct _tmp8EC;struct Cyc_Arg_Set_spec_struct*
_tmp8EB;const char*_tmp8EA;const char*_tmp8E9;struct _tuple15*_tmp8E8;const char*
_tmp8E7;struct Cyc_Arg_Set_spec_struct _tmp8E6;struct Cyc_Arg_Set_spec_struct*
_tmp8E5;const char*_tmp8E4;const char*_tmp8E3;struct _tuple15*_tmp8E2;const char*
_tmp8E1;struct Cyc_Arg_Set_spec_struct _tmp8E0;struct Cyc_Arg_Set_spec_struct*
_tmp8DF;const char*_tmp8DE;const char*_tmp8DD;struct _tuple15*_tmp8DC;const char*
_tmp8DB;struct Cyc_Arg_Set_spec_struct _tmp8DA;struct Cyc_Arg_Set_spec_struct*
_tmp8D9;const char*_tmp8D8;const char*_tmp8D7;struct _tuple15*_tmp8D6;const char*
_tmp8D5;struct Cyc_Arg_Set_spec_struct _tmp8D4;struct Cyc_Arg_Set_spec_struct*
_tmp8D3;const char*_tmp8D2;const char*_tmp8D1;struct _tuple15*_tmp8D0;const char*
_tmp8CF;struct Cyc_Arg_Set_spec_struct _tmp8CE;struct Cyc_Arg_Set_spec_struct*
_tmp8CD;const char*_tmp8CC;const char*_tmp8CB;struct _tuple15*_tmp8CA;const char*
_tmp8C9;struct Cyc_Arg_Set_spec_struct _tmp8C8;struct Cyc_Arg_Set_spec_struct*
_tmp8C7;const char*_tmp8C6;const char*_tmp8C5;struct _tuple15*_tmp8C4;const char*
_tmp8C3;struct Cyc_Arg_Unit_spec_struct _tmp8C2;struct Cyc_Arg_Unit_spec_struct*
_tmp8C1;const char*_tmp8C0;const char*_tmp8BF;struct _tuple15*_tmp8BE;const char*
_tmp8BD;struct Cyc_Arg_Unit_spec_struct _tmp8BC;struct Cyc_Arg_Unit_spec_struct*
_tmp8BB;const char*_tmp8BA;const char*_tmp8B9;struct _tuple15*_tmp8B8;const char*
_tmp8B7;struct Cyc_Arg_String_spec_struct _tmp8B6;struct Cyc_Arg_String_spec_struct*
_tmp8B5;const char*_tmp8B4;const char*_tmp8B3;struct _tuple15*_tmp8B2;const char*
_tmp8B1;struct Cyc_Arg_Unit_spec_struct _tmp8B0;struct Cyc_Arg_Unit_spec_struct*
_tmp8AF;const char*_tmp8AE;const char*_tmp8AD;struct _tuple15*_tmp8AC;const char*
_tmp8AB;struct Cyc_Arg_Unit_spec_struct _tmp8AA;struct Cyc_Arg_Unit_spec_struct*
_tmp8A9;const char*_tmp8A8;const char*_tmp8A7;struct _tuple15*_tmp8A6;const char*
_tmp8A5;struct Cyc_Arg_Unit_spec_struct _tmp8A4;struct Cyc_Arg_Unit_spec_struct*
_tmp8A3;const char*_tmp8A2;const char*_tmp8A1;struct _tuple15*_tmp8A0;const char*
_tmp89F;struct Cyc_Arg_Unit_spec_struct _tmp89E;struct Cyc_Arg_Unit_spec_struct*
_tmp89D;const char*_tmp89C;const char*_tmp89B;struct _tuple15*_tmp89A;const char*
_tmp899;struct Cyc_Arg_Unit_spec_struct _tmp898;struct Cyc_Arg_Unit_spec_struct*
_tmp897;const char*_tmp896;const char*_tmp895;struct _tuple15*_tmp894;const char*
_tmp893;struct Cyc_Arg_Set_spec_struct _tmp892;struct Cyc_Arg_Set_spec_struct*
_tmp891;const char*_tmp890;const char*_tmp88F;struct _tuple15*_tmp88E;const char*
_tmp88D;struct Cyc_Arg_Unit_spec_struct _tmp88C;struct Cyc_Arg_Unit_spec_struct*
_tmp88B;const char*_tmp88A;const char*_tmp889;struct _tuple15*_tmp888;const char*
_tmp887;struct Cyc_Arg_String_spec_struct _tmp886;struct Cyc_Arg_String_spec_struct*
_tmp885;const char*_tmp884;const char*_tmp883;struct _tuple15*_tmp882;const char*
_tmp881;struct Cyc_Arg_Unit_spec_struct _tmp880;struct Cyc_Arg_Unit_spec_struct*
_tmp87F;const char*_tmp87E;const char*_tmp87D;struct _tuple15*_tmp87C;const char*
_tmp87B;struct Cyc_Arg_Set_spec_struct _tmp87A;struct Cyc_Arg_Set_spec_struct*
_tmp879;const char*_tmp878;const char*_tmp877;struct _tuple15*_tmp876;const char*
_tmp875;struct Cyc_Arg_Clear_spec_struct _tmp874;struct Cyc_Arg_Clear_spec_struct*
_tmp873;const char*_tmp872;const char*_tmp871;struct _tuple15*_tmp870;const char*
_tmp86F;struct Cyc_Arg_Set_spec_struct _tmp86E;struct Cyc_Arg_Set_spec_struct*
_tmp86D;const char*_tmp86C;const char*_tmp86B;struct _tuple15*_tmp86A;const char*
_tmp869;struct Cyc_Arg_Set_spec_struct _tmp868;struct Cyc_Arg_Set_spec_struct*
_tmp867;const char*_tmp866;const char*_tmp865;struct _tuple15*_tmp864;const char*
_tmp863;struct Cyc_Arg_Set_spec_struct _tmp862;struct Cyc_Arg_Set_spec_struct*
_tmp861;const char*_tmp860;const char*_tmp85F;struct _tuple15*_tmp85E;const char*
_tmp85D;struct Cyc_Arg_Set_spec_struct _tmp85C;struct Cyc_Arg_Set_spec_struct*
_tmp85B;const char*_tmp85A;const char*_tmp859;struct _tuple15*_tmp858;const char*
_tmp857;struct Cyc_Arg_Set_spec_struct _tmp856;struct Cyc_Arg_Set_spec_struct*
_tmp855;const char*_tmp854;const char*_tmp853;struct _tuple15*_tmp852;const char*
_tmp851;struct Cyc_Arg_Set_spec_struct _tmp850;struct Cyc_Arg_Set_spec_struct*
_tmp84F;const char*_tmp84E;const char*_tmp84D;struct _tuple15*_tmp84C;const char*
_tmp84B;struct Cyc_Arg_Set_spec_struct _tmp84A;struct Cyc_Arg_Set_spec_struct*
_tmp849;const char*_tmp848;const char*_tmp847;struct _tuple15*_tmp846;const char*
_tmp845;struct Cyc_Arg_Set_spec_struct _tmp844;struct Cyc_Arg_Set_spec_struct*
_tmp843;const char*_tmp842;const char*_tmp841;struct _tuple15*_tmp840;const char*
_tmp83F;struct Cyc_Arg_Unit_spec_struct _tmp83E;struct Cyc_Arg_Unit_spec_struct*
_tmp83D;const char*_tmp83C;const char*_tmp83B;struct _tuple15*_tmp83A;const char*
_tmp839;struct Cyc_Arg_String_spec_struct _tmp838;struct Cyc_Arg_String_spec_struct*
_tmp837;const char*_tmp836;const char*_tmp835;struct _tuple15*_tmp834;const char*
_tmp833;struct Cyc_Arg_String_spec_struct _tmp832;struct Cyc_Arg_String_spec_struct*
_tmp831;const char*_tmp830;const char*_tmp82F;struct _tuple15*_tmp82E;const char*
_tmp82D;struct Cyc_Arg_Flag_spec_struct _tmp82C;struct Cyc_Arg_Flag_spec_struct*
_tmp82B;const char*_tmp82A;const char*_tmp829;struct _tuple15*_tmp828;const char*
_tmp827;struct Cyc_Arg_Unit_spec_struct _tmp826;struct Cyc_Arg_Unit_spec_struct*
_tmp825;const char*_tmp824;const char*_tmp823;struct _tuple15*_tmp822;const char*
_tmp821;struct Cyc_Arg_Unit_spec_struct _tmp820;struct Cyc_Arg_Unit_spec_struct*
_tmp81F;const char*_tmp81E;const char*_tmp81D;struct _tuple15*_tmp81C;const char*
_tmp81B;struct Cyc_Arg_Unit_spec_struct _tmp81A;struct Cyc_Arg_Unit_spec_struct*
_tmp819;const char*_tmp818;const char*_tmp817;struct _tuple15*_tmp816;const char*
_tmp815;struct Cyc_Arg_Unit_spec_struct _tmp814;struct Cyc_Arg_Unit_spec_struct*
_tmp813;const char*_tmp812;const char*_tmp811;struct _tuple15*_tmp810;const char*
_tmp80F;struct Cyc_Arg_Flag_spec_struct _tmp80E;struct Cyc_Arg_Flag_spec_struct*
_tmp80D;const char*_tmp80C;const char*_tmp80B;struct _tuple15*_tmp80A;const char*
_tmp809;struct Cyc_Arg_Flag_spec_struct _tmp808;struct Cyc_Arg_Flag_spec_struct*
_tmp807;const char*_tmp806;const char*_tmp805;struct _tuple15*_tmp804;const char*
_tmp803;struct Cyc_Arg_Flag_spec_struct _tmp802;struct Cyc_Arg_Flag_spec_struct*
_tmp801;const char*_tmp800;const char*_tmp7FF;struct _tuple15*_tmp7FE;const char*
_tmp7FD;struct Cyc_Arg_Flag_spec_struct _tmp7FC;struct Cyc_Arg_Flag_spec_struct*
_tmp7FB;const char*_tmp7FA;const char*_tmp7F9;struct _tuple15*_tmp7F8;const char*
_tmp7F7;struct Cyc_Arg_Flag_spec_struct _tmp7F6;struct Cyc_Arg_Flag_spec_struct*
_tmp7F5;const char*_tmp7F4;const char*_tmp7F3;struct _tuple15*_tmp7F2;const char*
_tmp7F1;struct Cyc_Arg_Flag_spec_struct _tmp7F0;struct Cyc_Arg_Flag_spec_struct*
_tmp7EF;const char*_tmp7EE;const char*_tmp7ED;struct _tuple15*_tmp7EC;const char*
_tmp7EB;struct Cyc_Arg_Flag_spec_struct _tmp7EA;struct Cyc_Arg_Flag_spec_struct*
_tmp7E9;const char*_tmp7E8;const char*_tmp7E7;struct _tuple15*_tmp7E6;const char*
_tmp7E5;struct Cyc_Arg_Flag_spec_struct _tmp7E4;struct Cyc_Arg_Flag_spec_struct*
_tmp7E3;const char*_tmp7E2;const char*_tmp7E1;struct _tuple15*_tmp7E0;const char*
_tmp7DF;struct Cyc_Arg_String_spec_struct _tmp7DE;struct Cyc_Arg_String_spec_struct*
_tmp7DD;const char*_tmp7DC;const char*_tmp7DB;struct _tuple15*_tmp7DA;const char*
_tmp7D9;struct Cyc_Arg_Unit_spec_struct _tmp7D8;struct Cyc_Arg_Unit_spec_struct*
_tmp7D7;const char*_tmp7D6;const char*_tmp7D5;struct _tuple15*_tmp7D4;const char*
_tmp7D3;struct Cyc_Arg_Flag_spec_struct _tmp7D2;struct Cyc_Arg_Flag_spec_struct*
_tmp7D1;const char*_tmp7D0;const char*_tmp7CF;struct _tuple15*_tmp7CE;const char*
_tmp7CD;struct Cyc_Arg_Flag_spec_struct _tmp7CC;struct Cyc_Arg_Flag_spec_struct*
_tmp7CB;const char*_tmp7CA;const char*_tmp7C9;struct _tuple15*_tmp7C8;const char*
_tmp7C7;struct Cyc_Arg_Flag_spec_struct _tmp7C6;struct Cyc_Arg_Flag_spec_struct*
_tmp7C5;const char*_tmp7C4;const char*_tmp7C3;struct _tuple15*_tmp7C2;const char*
_tmp7C1;struct Cyc_Arg_Flag_spec_struct _tmp7C0;struct Cyc_Arg_Flag_spec_struct*
_tmp7BF;const char*_tmp7BE;const char*_tmp7BD;struct _tuple15*_tmp7BC;const char*
_tmp7BB;struct Cyc_Arg_Flag_spec_struct _tmp7BA;struct Cyc_Arg_Flag_spec_struct*
_tmp7B9;const char*_tmp7B8;const char*_tmp7B7;struct _tuple15*_tmp7B6;const char*
_tmp7B5;struct Cyc_Arg_Flag_spec_struct _tmp7B4;struct Cyc_Arg_Flag_spec_struct*
_tmp7B3;const char*_tmp7B2;const char*_tmp7B1;struct _tuple15*_tmp7B0;const char*
_tmp7AF;struct Cyc_Arg_String_spec_struct _tmp7AE;struct Cyc_Arg_String_spec_struct*
_tmp7AD;const char*_tmp7AC;const char*_tmp7AB;struct _tuple15*_tmp7AA;const char*
_tmp7A9;struct Cyc_Arg_Unit_spec_struct _tmp7A8;struct Cyc_Arg_Unit_spec_struct*
_tmp7A7;const char*_tmp7A6;const char*_tmp7A5;struct _tuple15*_tmp7A4;const char*
_tmp7A3;struct Cyc_Arg_Set_spec_struct _tmp7A2;struct Cyc_Arg_Set_spec_struct*
_tmp7A1;const char*_tmp7A0;const char*_tmp79F;struct _tuple15*_tmp79E[64];struct Cyc_List_List*
options=(_tmp79E[63]=((_tmp91E=_region_malloc(r,sizeof(*_tmp91E)),((_tmp91E->f1=((
_tmp919="-noregions",_tag_dynforward(_tmp919,sizeof(char),
_get_zero_arr_size_char(_tmp919,11)))),((_tmp91E->f2=0,((_tmp91E->f3=((_tmp91A="",
_tag_dynforward(_tmp91A,sizeof(char),_get_zero_arr_size_char(_tmp91A,1)))),((
_tmp91E->f4=(void*)((_tmp91B=_region_malloc(r,sizeof(*_tmp91B)),((_tmp91B[0]=((
_tmp91C.tag=3,((_tmp91C.f1=& Cyc_Absyn_no_regions,_tmp91C)))),_tmp91B)))),((
_tmp91E->f5=((_tmp91D="Generate code that doesn't use regions",_tag_dynforward(
_tmp91D,sizeof(char),_get_zero_arr_size_char(_tmp91D,39)))),_tmp91E)))))))))))),((
_tmp79E[62]=((_tmp918=_region_malloc(r,sizeof(*_tmp918)),((_tmp918->f1=((_tmp913="-port",
_tag_dynforward(_tmp913,sizeof(char),_get_zero_arr_size_char(_tmp913,6)))),((
_tmp918->f2=0,((_tmp918->f3=((_tmp914="",_tag_dynforward(_tmp914,sizeof(char),
_get_zero_arr_size_char(_tmp914,1)))),((_tmp918->f4=(void*)((_tmp915=
_region_malloc(r,sizeof(*_tmp915)),((_tmp915[0]=((_tmp916.tag=0,((_tmp916.f1=Cyc_set_port_c_code,
_tmp916)))),_tmp915)))),((_tmp918->f5=((_tmp917="Suggest how to port C code to Cyclone",
_tag_dynforward(_tmp917,sizeof(char),_get_zero_arr_size_char(_tmp917,38)))),
_tmp918)))))))))))),((_tmp79E[61]=((_tmp912=_region_malloc(r,sizeof(*_tmp912)),((
_tmp912->f1=((_tmp90D="-detailedlocation",_tag_dynforward(_tmp90D,sizeof(char),
_get_zero_arr_size_char(_tmp90D,18)))),((_tmp912->f2=0,((_tmp912->f3=((_tmp90E="",
_tag_dynforward(_tmp90E,sizeof(char),_get_zero_arr_size_char(_tmp90E,1)))),((
_tmp912->f4=(void*)((_tmp90F=_region_malloc(r,sizeof(*_tmp90F)),((_tmp90F[0]=((
_tmp910.tag=4,((_tmp910.f1=& Cyc_Position_use_gcc_style_location,_tmp910)))),
_tmp90F)))),((_tmp912->f5=((_tmp911="Try to give more detailed location information for errors",
_tag_dynforward(_tmp911,sizeof(char),_get_zero_arr_size_char(_tmp911,58)))),
_tmp912)))))))))))),((_tmp79E[60]=((_tmp90C=_region_malloc(r,sizeof(*_tmp90C)),((
_tmp90C->f1=((_tmp907="-noregister",_tag_dynforward(_tmp907,sizeof(char),
_get_zero_arr_size_char(_tmp907,12)))),((_tmp90C->f2=0,((_tmp90C->f3=((_tmp908="",
_tag_dynforward(_tmp908,sizeof(char),_get_zero_arr_size_char(_tmp908,1)))),((
_tmp90C->f4=(void*)((_tmp909=_region_malloc(r,sizeof(*_tmp909)),((_tmp909[0]=((
_tmp90A.tag=3,((_tmp90A.f1=& Cyc_Parse_no_register,_tmp90A)))),_tmp909)))),((
_tmp90C->f5=((_tmp90B="Treat register storage class as public",_tag_dynforward(
_tmp90B,sizeof(char),_get_zero_arr_size_char(_tmp90B,39)))),_tmp90C)))))))))))),((
_tmp79E[59]=((_tmp906=_region_malloc(r,sizeof(*_tmp906)),((_tmp906->f1=((_tmp901="-warnnullchecks",
_tag_dynforward(_tmp901,sizeof(char),_get_zero_arr_size_char(_tmp901,16)))),((
_tmp906->f2=0,((_tmp906->f3=((_tmp902="",_tag_dynforward(_tmp902,sizeof(char),
_get_zero_arr_size_char(_tmp902,1)))),((_tmp906->f4=(void*)((_tmp903=
_region_malloc(r,sizeof(*_tmp903)),((_tmp903[0]=((_tmp904.tag=3,((_tmp904.f1=&
Cyc_Toc_warn_all_null_deref,_tmp904)))),_tmp903)))),((_tmp906->f5=((_tmp905="Warn when any null check can't be eliminated",
_tag_dynforward(_tmp905,sizeof(char),_get_zero_arr_size_char(_tmp905,45)))),
_tmp906)))))))))))),((_tmp79E[58]=((_tmp900=_region_malloc(r,sizeof(*_tmp900)),((
_tmp900->f1=((_tmp8FB="-warnboundschecks",_tag_dynforward(_tmp8FB,sizeof(char),
_get_zero_arr_size_char(_tmp8FB,18)))),((_tmp900->f2=0,((_tmp900->f3=((_tmp8FC="",
_tag_dynforward(_tmp8FC,sizeof(char),_get_zero_arr_size_char(_tmp8FC,1)))),((
_tmp900->f4=(void*)((_tmp8FD=_region_malloc(r,sizeof(*_tmp8FD)),((_tmp8FD[0]=((
_tmp8FE.tag=3,((_tmp8FE.f1=& Cyc_Toc_warn_bounds_checks,_tmp8FE)))),_tmp8FD)))),((
_tmp900->f5=((_tmp8FF="Warn when bounds checks can't be eliminated",
_tag_dynforward(_tmp8FF,sizeof(char),_get_zero_arr_size_char(_tmp8FF,44)))),
_tmp900)))))))))))),((_tmp79E[57]=((_tmp8FA=_region_malloc(r,sizeof(*_tmp8FA)),((
_tmp8FA->f1=((_tmp8F5="-CI",_tag_dynforward(_tmp8F5,sizeof(char),
_get_zero_arr_size_char(_tmp8F5,4)))),((_tmp8FA->f2=0,((_tmp8FA->f3=((_tmp8F6=" <file>",
_tag_dynforward(_tmp8F6,sizeof(char),_get_zero_arr_size_char(_tmp8F6,8)))),((
_tmp8FA->f4=(void*)((_tmp8F7=_region_malloc(r,sizeof(*_tmp8F7)),((_tmp8F7[0]=((
_tmp8F8.tag=5,((_tmp8F8.f1=Cyc_set_cyc_include,_tmp8F8)))),_tmp8F7)))),((_tmp8FA->f5=((
_tmp8F9="Set cyc_include.h to be <file>",_tag_dynforward(_tmp8F9,sizeof(char),
_get_zero_arr_size_char(_tmp8F9,31)))),_tmp8FA)))))))))))),((_tmp79E[56]=((
_tmp8F4=_region_malloc(r,sizeof(*_tmp8F4)),((_tmp8F4->f1=((_tmp8EF="-nocyc_setjmp",
_tag_dynforward(_tmp8EF,sizeof(char),_get_zero_arr_size_char(_tmp8EF,14)))),((
_tmp8F4->f2=0,((_tmp8F4->f3=((_tmp8F0="",_tag_dynforward(_tmp8F0,sizeof(char),
_get_zero_arr_size_char(_tmp8F0,1)))),((_tmp8F4->f4=(void*)((_tmp8F1=
_region_malloc(r,sizeof(*_tmp8F1)),((_tmp8F1[0]=((_tmp8F2.tag=3,((_tmp8F2.f1=&
Cyc_nocyc_setjmp_r,_tmp8F2)))),_tmp8F1)))),((_tmp8F4->f5=((_tmp8F3="Do not use compiler special file cyc_setjmp.h",
_tag_dynforward(_tmp8F3,sizeof(char),_get_zero_arr_size_char(_tmp8F3,46)))),
_tmp8F4)))))))))))),((_tmp79E[55]=((_tmp8EE=_region_malloc(r,sizeof(*_tmp8EE)),((
_tmp8EE->f1=((_tmp8E9="-printalleffects",_tag_dynforward(_tmp8E9,sizeof(char),
_get_zero_arr_size_char(_tmp8E9,17)))),((_tmp8EE->f2=0,((_tmp8EE->f3=((_tmp8EA="",
_tag_dynforward(_tmp8EA,sizeof(char),_get_zero_arr_size_char(_tmp8EA,1)))),((
_tmp8EE->f4=(void*)((_tmp8EB=_region_malloc(r,sizeof(*_tmp8EB)),((_tmp8EB[0]=((
_tmp8EC.tag=3,((_tmp8EC.f1=& Cyc_print_all_effects_r,_tmp8EC)))),_tmp8EB)))),((
_tmp8EE->f5=((_tmp8ED="Print effects for functions (type debugging)",
_tag_dynforward(_tmp8ED,sizeof(char),_get_zero_arr_size_char(_tmp8ED,45)))),
_tmp8EE)))))))))))),((_tmp79E[54]=((_tmp8E8=_region_malloc(r,sizeof(*_tmp8E8)),((
_tmp8E8->f1=((_tmp8E3="-printfullevars",_tag_dynforward(_tmp8E3,sizeof(char),
_get_zero_arr_size_char(_tmp8E3,16)))),((_tmp8E8->f2=0,((_tmp8E8->f3=((_tmp8E4="",
_tag_dynforward(_tmp8E4,sizeof(char),_get_zero_arr_size_char(_tmp8E4,1)))),((
_tmp8E8->f4=(void*)((_tmp8E5=_region_malloc(r,sizeof(*_tmp8E5)),((_tmp8E5[0]=((
_tmp8E6.tag=3,((_tmp8E6.f1=& Cyc_print_full_evars_r,_tmp8E6)))),_tmp8E5)))),((
_tmp8E8->f5=((_tmp8E7="Print full information for evars (type debugging)",
_tag_dynforward(_tmp8E7,sizeof(char),_get_zero_arr_size_char(_tmp8E7,50)))),
_tmp8E8)))))))))))),((_tmp79E[53]=((_tmp8E2=_region_malloc(r,sizeof(*_tmp8E2)),((
_tmp8E2->f1=((_tmp8DD="-printallkinds",_tag_dynforward(_tmp8DD,sizeof(char),
_get_zero_arr_size_char(_tmp8DD,15)))),((_tmp8E2->f2=0,((_tmp8E2->f3=((_tmp8DE="",
_tag_dynforward(_tmp8DE,sizeof(char),_get_zero_arr_size_char(_tmp8DE,1)))),((
_tmp8E2->f4=(void*)((_tmp8DF=_region_malloc(r,sizeof(*_tmp8DF)),((_tmp8DF[0]=((
_tmp8E0.tag=3,((_tmp8E0.f1=& Cyc_print_all_kinds_r,_tmp8E0)))),_tmp8DF)))),((
_tmp8E2->f5=((_tmp8E1="Always print kinds of type variables",_tag_dynforward(
_tmp8E1,sizeof(char),_get_zero_arr_size_char(_tmp8E1,37)))),_tmp8E2)))))))))))),((
_tmp79E[52]=((_tmp8DC=_region_malloc(r,sizeof(*_tmp8DC)),((_tmp8DC->f1=((_tmp8D7="-printalltvars",
_tag_dynforward(_tmp8D7,sizeof(char),_get_zero_arr_size_char(_tmp8D7,15)))),((
_tmp8DC->f2=0,((_tmp8DC->f3=((_tmp8D8="",_tag_dynforward(_tmp8D8,sizeof(char),
_get_zero_arr_size_char(_tmp8D8,1)))),((_tmp8DC->f4=(void*)((_tmp8D9=
_region_malloc(r,sizeof(*_tmp8D9)),((_tmp8D9[0]=((_tmp8DA.tag=3,((_tmp8DA.f1=&
Cyc_print_all_tvars_r,_tmp8DA)))),_tmp8D9)))),((_tmp8DC->f5=((_tmp8DB="Print all type variables (even implicit default effects)",
_tag_dynforward(_tmp8DB,sizeof(char),_get_zero_arr_size_char(_tmp8DB,57)))),
_tmp8DC)))))))))))),((_tmp79E[51]=((_tmp8D6=_region_malloc(r,sizeof(*_tmp8D6)),((
_tmp8D6->f1=((_tmp8D1="-noexpandtypedefs",_tag_dynforward(_tmp8D1,sizeof(char),
_get_zero_arr_size_char(_tmp8D1,18)))),((_tmp8D6->f2=0,((_tmp8D6->f3=((_tmp8D2="",
_tag_dynforward(_tmp8D2,sizeof(char),_get_zero_arr_size_char(_tmp8D2,1)))),((
_tmp8D6->f4=(void*)((_tmp8D3=_region_malloc(r,sizeof(*_tmp8D3)),((_tmp8D3[0]=((
_tmp8D4.tag=3,((_tmp8D4.f1=& Cyc_noexpand_r,_tmp8D4)))),_tmp8D3)))),((_tmp8D6->f5=((
_tmp8D5="Don't expand typedefs in pretty printing",_tag_dynforward(_tmp8D5,
sizeof(char),_get_zero_arr_size_char(_tmp8D5,41)))),_tmp8D6)))))))))))),((
_tmp79E[50]=((_tmp8D0=_region_malloc(r,sizeof(*_tmp8D0)),((_tmp8D0->f1=((_tmp8CB="-noremoveunused",
_tag_dynforward(_tmp8CB,sizeof(char),_get_zero_arr_size_char(_tmp8CB,16)))),((
_tmp8D0->f2=0,((_tmp8D0->f3=((_tmp8CC="",_tag_dynforward(_tmp8CC,sizeof(char),
_get_zero_arr_size_char(_tmp8CC,1)))),((_tmp8D0->f4=(void*)((_tmp8CD=
_region_malloc(r,sizeof(*_tmp8CD)),((_tmp8CD[0]=((_tmp8CE.tag=3,((_tmp8CE.f1=&
Cyc_noshake_r,_tmp8CE)))),_tmp8CD)))),((_tmp8D0->f5=((_tmp8CF="Don't remove externed variables that aren't used",
_tag_dynforward(_tmp8CF,sizeof(char),_get_zero_arr_size_char(_tmp8CF,49)))),
_tmp8D0)))))))))))),((_tmp79E[49]=((_tmp8CA=_region_malloc(r,sizeof(*_tmp8CA)),((
_tmp8CA->f1=((_tmp8C5="-nogc",_tag_dynforward(_tmp8C5,sizeof(char),
_get_zero_arr_size_char(_tmp8C5,6)))),((_tmp8CA->f2=0,((_tmp8CA->f3=((_tmp8C6="",
_tag_dynforward(_tmp8C6,sizeof(char),_get_zero_arr_size_char(_tmp8C6,1)))),((
_tmp8CA->f4=(void*)((_tmp8C7=_region_malloc(r,sizeof(*_tmp8C7)),((_tmp8C7[0]=((
_tmp8C8.tag=3,((_tmp8C8.f1=& Cyc_nogc_r,_tmp8C8)))),_tmp8C7)))),((_tmp8CA->f5=((
_tmp8C9="Don't link in the garbage collector",_tag_dynforward(_tmp8C9,sizeof(
char),_get_zero_arr_size_char(_tmp8C9,36)))),_tmp8CA)))))))))))),((_tmp79E[48]=((
_tmp8C4=_region_malloc(r,sizeof(*_tmp8C4)),((_tmp8C4->f1=((_tmp8BF="-nocyc",
_tag_dynforward(_tmp8BF,sizeof(char),_get_zero_arr_size_char(_tmp8BF,7)))),((
_tmp8C4->f2=0,((_tmp8C4->f3=((_tmp8C0="",_tag_dynforward(_tmp8C0,sizeof(char),
_get_zero_arr_size_char(_tmp8C0,1)))),((_tmp8C4->f4=(void*)((_tmp8C1=
_region_malloc(r,sizeof(*_tmp8C1)),((_tmp8C1[0]=((_tmp8C2.tag=0,((_tmp8C2.f1=Cyc_set_nocyc,
_tmp8C2)))),_tmp8C1)))),((_tmp8C4->f5=((_tmp8C3="Don't add implicit namespace Cyc",
_tag_dynforward(_tmp8C3,sizeof(char),_get_zero_arr_size_char(_tmp8C3,33)))),
_tmp8C4)))))))))))),((_tmp79E[47]=((_tmp8BE=_region_malloc(r,sizeof(*_tmp8BE)),((
_tmp8BE->f1=((_tmp8B9="-no-cpp-precomp",_tag_dynforward(_tmp8B9,sizeof(char),
_get_zero_arr_size_char(_tmp8B9,16)))),((_tmp8BE->f2=0,((_tmp8BE->f3=((_tmp8BA="",
_tag_dynforward(_tmp8BA,sizeof(char),_get_zero_arr_size_char(_tmp8BA,1)))),((
_tmp8BE->f4=(void*)((_tmp8BB=_region_malloc(r,sizeof(*_tmp8BB)),((_tmp8BB[0]=((
_tmp8BC.tag=0,((_tmp8BC.f1=Cyc_set_nocppprecomp,_tmp8BC)))),_tmp8BB)))),((
_tmp8BE->f5=((_tmp8BD="Don't do smart preprocessing (mac only)",_tag_dynforward(
_tmp8BD,sizeof(char),_get_zero_arr_size_char(_tmp8BD,40)))),_tmp8BE)))))))))))),((
_tmp79E[46]=((_tmp8B8=_region_malloc(r,sizeof(*_tmp8B8)),((_tmp8B8->f1=((_tmp8B3="-use-cpp",
_tag_dynforward(_tmp8B3,sizeof(char),_get_zero_arr_size_char(_tmp8B3,9)))),((
_tmp8B8->f2=0,((_tmp8B8->f3=((_tmp8B4="<path>",_tag_dynforward(_tmp8B4,sizeof(
char),_get_zero_arr_size_char(_tmp8B4,7)))),((_tmp8B8->f4=(void*)((_tmp8B5=
_region_malloc(r,sizeof(*_tmp8B5)),((_tmp8B5[0]=((_tmp8B6.tag=5,((_tmp8B6.f1=Cyc_set_cpp,
_tmp8B6)))),_tmp8B5)))),((_tmp8B8->f5=((_tmp8B7="Indicate which preprocessor to use",
_tag_dynforward(_tmp8B7,sizeof(char),_get_zero_arr_size_char(_tmp8B7,35)))),
_tmp8B8)))))))))))),((_tmp79E[45]=((_tmp8B2=_region_malloc(r,sizeof(*_tmp8B2)),((
_tmp8B2->f1=((_tmp8AD="--inline-checks",_tag_dynforward(_tmp8AD,sizeof(char),
_get_zero_arr_size_char(_tmp8AD,16)))),((_tmp8B2->f2=0,((_tmp8B2->f3=((_tmp8AE="",
_tag_dynforward(_tmp8AE,sizeof(char),_get_zero_arr_size_char(_tmp8AE,1)))),((
_tmp8B2->f4=(void*)((_tmp8AF=_region_malloc(r,sizeof(*_tmp8AF)),((_tmp8AF[0]=((
_tmp8B0.tag=0,((_tmp8B0.f1=Cyc_set_inline_functions,_tmp8B0)))),_tmp8AF)))),((
_tmp8B2->f5=((_tmp8B1="Inline bounds checks instead of #define",_tag_dynforward(
_tmp8B1,sizeof(char),_get_zero_arr_size_char(_tmp8B1,40)))),_tmp8B2)))))))))))),((
_tmp79E[44]=((_tmp8AC=_region_malloc(r,sizeof(*_tmp8AC)),((_tmp8AC->f1=((_tmp8A7="--noboundschecks",
_tag_dynforward(_tmp8A7,sizeof(char),_get_zero_arr_size_char(_tmp8A7,17)))),((
_tmp8AC->f2=0,((_tmp8AC->f3=((_tmp8A8="",_tag_dynforward(_tmp8A8,sizeof(char),
_get_zero_arr_size_char(_tmp8A8,1)))),((_tmp8AC->f4=(void*)((_tmp8A9=
_region_malloc(r,sizeof(*_tmp8A9)),((_tmp8A9[0]=((_tmp8AA.tag=0,((_tmp8AA.f1=Cyc_set_noboundschecks,
_tmp8AA)))),_tmp8A9)))),((_tmp8AC->f5=((_tmp8AB="Disable bounds checks",
_tag_dynforward(_tmp8AB,sizeof(char),_get_zero_arr_size_char(_tmp8AB,22)))),
_tmp8AC)))))))))))),((_tmp79E[43]=((_tmp8A6=_region_malloc(r,sizeof(*_tmp8A6)),((
_tmp8A6->f1=((_tmp8A1="--nonullchecks",_tag_dynforward(_tmp8A1,sizeof(char),
_get_zero_arr_size_char(_tmp8A1,15)))),((_tmp8A6->f2=0,((_tmp8A6->f3=((_tmp8A2="",
_tag_dynforward(_tmp8A2,sizeof(char),_get_zero_arr_size_char(_tmp8A2,1)))),((
_tmp8A6->f4=(void*)((_tmp8A3=_region_malloc(r,sizeof(*_tmp8A3)),((_tmp8A3[0]=((
_tmp8A4.tag=0,((_tmp8A4.f1=Cyc_set_nonullchecks,_tmp8A4)))),_tmp8A3)))),((
_tmp8A6->f5=((_tmp8A5="Disable null checks",_tag_dynforward(_tmp8A5,sizeof(char),
_get_zero_arr_size_char(_tmp8A5,20)))),_tmp8A6)))))))))))),((_tmp79E[42]=((
_tmp8A0=_region_malloc(r,sizeof(*_tmp8A0)),((_tmp8A0->f1=((_tmp89B="--nochecks",
_tag_dynforward(_tmp89B,sizeof(char),_get_zero_arr_size_char(_tmp89B,11)))),((
_tmp8A0->f2=0,((_tmp8A0->f3=((_tmp89C="",_tag_dynforward(_tmp89C,sizeof(char),
_get_zero_arr_size_char(_tmp89C,1)))),((_tmp8A0->f4=(void*)((_tmp89D=
_region_malloc(r,sizeof(*_tmp89D)),((_tmp89D[0]=((_tmp89E.tag=0,((_tmp89E.f1=Cyc_set_nochecks,
_tmp89E)))),_tmp89D)))),((_tmp8A0->f5=((_tmp89F="Disable bounds/null checks",
_tag_dynforward(_tmp89F,sizeof(char),_get_zero_arr_size_char(_tmp89F,27)))),
_tmp8A0)))))))))))),((_tmp79E[41]=((_tmp89A=_region_malloc(r,sizeof(*_tmp89A)),((
_tmp89A->f1=((_tmp895="--lineno",_tag_dynforward(_tmp895,sizeof(char),
_get_zero_arr_size_char(_tmp895,9)))),((_tmp89A->f2=0,((_tmp89A->f3=((_tmp896="",
_tag_dynforward(_tmp896,sizeof(char),_get_zero_arr_size_char(_tmp896,1)))),((
_tmp89A->f4=(void*)((_tmp897=_region_malloc(r,sizeof(*_tmp897)),((_tmp897[0]=((
_tmp898.tag=0,((_tmp898.f1=Cyc_set_lineno,_tmp898)))),_tmp897)))),((_tmp89A->f5=((
_tmp899="Generate line numbers for debugging",_tag_dynforward(_tmp899,sizeof(
char),_get_zero_arr_size_char(_tmp899,36)))),_tmp89A)))))))))))),((_tmp79E[40]=((
_tmp894=_region_malloc(r,sizeof(*_tmp894)),((_tmp894->f1=((_tmp88F="-save-c",
_tag_dynforward(_tmp88F,sizeof(char),_get_zero_arr_size_char(_tmp88F,8)))),((
_tmp894->f2=0,((_tmp894->f3=((_tmp890="",_tag_dynforward(_tmp890,sizeof(char),
_get_zero_arr_size_char(_tmp890,1)))),((_tmp894->f4=(void*)((_tmp891=
_region_malloc(r,sizeof(*_tmp891)),((_tmp891[0]=((_tmp892.tag=3,((_tmp892.f1=&
Cyc_save_c_r,_tmp892)))),_tmp891)))),((_tmp894->f5=((_tmp893="Don't delete temporary C files",
_tag_dynforward(_tmp893,sizeof(char),_get_zero_arr_size_char(_tmp893,31)))),
_tmp894)))))))))))),((_tmp79E[39]=((_tmp88E=_region_malloc(r,sizeof(*_tmp88E)),((
_tmp88E->f1=((_tmp889="-save-temps",_tag_dynforward(_tmp889,sizeof(char),
_get_zero_arr_size_char(_tmp889,12)))),((_tmp88E->f2=0,((_tmp88E->f3=((_tmp88A="",
_tag_dynforward(_tmp88A,sizeof(char),_get_zero_arr_size_char(_tmp88A,1)))),((
_tmp88E->f4=(void*)((_tmp88B=_region_malloc(r,sizeof(*_tmp88B)),((_tmp88B[0]=((
_tmp88C.tag=0,((_tmp88C.f1=Cyc_set_save_temps,_tmp88C)))),_tmp88B)))),((_tmp88E->f5=((
_tmp88D="Don't delete temporary files",_tag_dynforward(_tmp88D,sizeof(char),
_get_zero_arr_size_char(_tmp88D,29)))),_tmp88E)))))))))))),((_tmp79E[38]=((
_tmp888=_region_malloc(r,sizeof(*_tmp888)),((_tmp888->f1=((_tmp883="-c-comp",
_tag_dynforward(_tmp883,sizeof(char),_get_zero_arr_size_char(_tmp883,8)))),((
_tmp888->f2=0,((_tmp888->f3=((_tmp884=" <compiler>",_tag_dynforward(_tmp884,
sizeof(char),_get_zero_arr_size_char(_tmp884,12)))),((_tmp888->f4=(void*)((
_tmp885=_region_malloc(r,sizeof(*_tmp885)),((_tmp885[0]=((_tmp886.tag=5,((
_tmp886.f1=Cyc_set_c_compiler,_tmp886)))),_tmp885)))),((_tmp888->f5=((_tmp887="Produce C output for the given compiler",
_tag_dynforward(_tmp887,sizeof(char),_get_zero_arr_size_char(_tmp887,40)))),
_tmp888)))))))))))),((_tmp79E[37]=((_tmp882=_region_malloc(r,sizeof(*_tmp882)),((
_tmp882->f1=((_tmp87D="-un-gcc",_tag_dynforward(_tmp87D,sizeof(char),
_get_zero_arr_size_char(_tmp87D,8)))),((_tmp882->f2=0,((_tmp882->f3=((_tmp87E="",
_tag_dynforward(_tmp87E,sizeof(char),_get_zero_arr_size_char(_tmp87E,1)))),((
_tmp882->f4=(void*)((_tmp87F=_region_malloc(r,sizeof(*_tmp87F)),((_tmp87F[0]=((
_tmp880.tag=0,((_tmp880.f1=Cyc_set_tovc,_tmp880)))),_tmp87F)))),((_tmp882->f5=((
_tmp881="Avoid gcc extensions in C output",_tag_dynforward(_tmp881,sizeof(char),
_get_zero_arr_size_char(_tmp881,33)))),_tmp882)))))))))))),((_tmp79E[36]=((
_tmp87C=_region_malloc(r,sizeof(*_tmp87C)),((_tmp87C->f1=((_tmp877="-elim-statement-expressions",
_tag_dynforward(_tmp877,sizeof(char),_get_zero_arr_size_char(_tmp877,28)))),((
_tmp87C->f2=0,((_tmp87C->f3=((_tmp878="",_tag_dynforward(_tmp878,sizeof(char),
_get_zero_arr_size_char(_tmp878,1)))),((_tmp87C->f4=(void*)((_tmp879=
_region_malloc(r,sizeof(*_tmp879)),((_tmp879[0]=((_tmp87A.tag=3,((_tmp87A.f1=&
Cyc_elim_se_r,_tmp87A)))),_tmp879)))),((_tmp87C->f5=((_tmp87B="Avoid statement expressions in C output (implies --inline-checks)",
_tag_dynforward(_tmp87B,sizeof(char),_get_zero_arr_size_char(_tmp87B,66)))),
_tmp87C)))))))))))),((_tmp79E[35]=((_tmp876=_region_malloc(r,sizeof(*_tmp876)),((
_tmp876->f1=((_tmp871="-up",_tag_dynforward(_tmp871,sizeof(char),
_get_zero_arr_size_char(_tmp871,4)))),((_tmp876->f2=0,((_tmp876->f3=((_tmp872="",
_tag_dynforward(_tmp872,sizeof(char),_get_zero_arr_size_char(_tmp872,1)))),((
_tmp876->f4=(void*)((_tmp873=_region_malloc(r,sizeof(*_tmp873)),((_tmp873[0]=((
_tmp874.tag=4,((_tmp874.f1=& Cyc_pp_r,_tmp874)))),_tmp873)))),((_tmp876->f5=((
_tmp875="Ugly print",_tag_dynforward(_tmp875,sizeof(char),
_get_zero_arr_size_char(_tmp875,11)))),_tmp876)))))))))))),((_tmp79E[34]=((
_tmp870=_region_malloc(r,sizeof(*_tmp870)),((_tmp870->f1=((_tmp86B="-pp",
_tag_dynforward(_tmp86B,sizeof(char),_get_zero_arr_size_char(_tmp86B,4)))),((
_tmp870->f2=0,((_tmp870->f3=((_tmp86C="",_tag_dynforward(_tmp86C,sizeof(char),
_get_zero_arr_size_char(_tmp86C,1)))),((_tmp870->f4=(void*)((_tmp86D=
_region_malloc(r,sizeof(*_tmp86D)),((_tmp86D[0]=((_tmp86E.tag=3,((_tmp86E.f1=&
Cyc_pp_r,_tmp86E)))),_tmp86D)))),((_tmp870->f5=((_tmp86F="Pretty print",
_tag_dynforward(_tmp86F,sizeof(char),_get_zero_arr_size_char(_tmp86F,13)))),
_tmp870)))))))))))),((_tmp79E[33]=((_tmp86A=_region_malloc(r,sizeof(*_tmp86A)),((
_tmp86A->f1=((_tmp865="-ic",_tag_dynforward(_tmp865,sizeof(char),
_get_zero_arr_size_char(_tmp865,4)))),((_tmp86A->f2=0,((_tmp86A->f3=((_tmp866="",
_tag_dynforward(_tmp866,sizeof(char),_get_zero_arr_size_char(_tmp866,1)))),((
_tmp86A->f4=(void*)((_tmp867=_region_malloc(r,sizeof(*_tmp867)),((_tmp867[0]=((
_tmp868.tag=3,((_tmp868.f1=& Cyc_ic_r,_tmp868)))),_tmp867)))),((_tmp86A->f5=((
_tmp869="Activate the link-checker",_tag_dynforward(_tmp869,sizeof(char),
_get_zero_arr_size_char(_tmp869,26)))),_tmp86A)))))))))))),((_tmp79E[32]=((
_tmp864=_region_malloc(r,sizeof(*_tmp864)),((_tmp864->f1=((_tmp85F="-stopafter-toc",
_tag_dynforward(_tmp85F,sizeof(char),_get_zero_arr_size_char(_tmp85F,15)))),((
_tmp864->f2=0,((_tmp864->f3=((_tmp860="",_tag_dynforward(_tmp860,sizeof(char),
_get_zero_arr_size_char(_tmp860,1)))),((_tmp864->f4=(void*)((_tmp861=
_region_malloc(r,sizeof(*_tmp861)),((_tmp861[0]=((_tmp862.tag=3,((_tmp862.f1=&
Cyc_toc_r,_tmp862)))),_tmp861)))),((_tmp864->f5=((_tmp863="Stop after translation to C",
_tag_dynforward(_tmp863,sizeof(char),_get_zero_arr_size_char(_tmp863,28)))),
_tmp864)))))))))))),((_tmp79E[31]=((_tmp85E=_region_malloc(r,sizeof(*_tmp85E)),((
_tmp85E->f1=((_tmp859="-stopafter-cf",_tag_dynforward(_tmp859,sizeof(char),
_get_zero_arr_size_char(_tmp859,14)))),((_tmp85E->f2=0,((_tmp85E->f3=((_tmp85A="",
_tag_dynforward(_tmp85A,sizeof(char),_get_zero_arr_size_char(_tmp85A,1)))),((
_tmp85E->f4=(void*)((_tmp85B=_region_malloc(r,sizeof(*_tmp85B)),((_tmp85B[0]=((
_tmp85C.tag=3,((_tmp85C.f1=& Cyc_cf_r,_tmp85C)))),_tmp85B)))),((_tmp85E->f5=((
_tmp85D="Stop after control-flow checking",_tag_dynforward(_tmp85D,sizeof(char),
_get_zero_arr_size_char(_tmp85D,33)))),_tmp85E)))))))))))),((_tmp79E[30]=((
_tmp858=_region_malloc(r,sizeof(*_tmp858)),((_tmp858->f1=((_tmp853="-noprint",
_tag_dynforward(_tmp853,sizeof(char),_get_zero_arr_size_char(_tmp853,9)))),((
_tmp858->f2=0,((_tmp858->f3=((_tmp854="",_tag_dynforward(_tmp854,sizeof(char),
_get_zero_arr_size_char(_tmp854,1)))),((_tmp858->f4=(void*)((_tmp855=
_region_malloc(r,sizeof(*_tmp855)),((_tmp855[0]=((_tmp856.tag=3,((_tmp856.f1=&
Cyc_noprint_r,_tmp856)))),_tmp855)))),((_tmp858->f5=((_tmp857="Do not print output (when stopping early)",
_tag_dynforward(_tmp857,sizeof(char),_get_zero_arr_size_char(_tmp857,42)))),
_tmp858)))))))))))),((_tmp79E[29]=((_tmp852=_region_malloc(r,sizeof(*_tmp852)),((
_tmp852->f1=((_tmp84D="-stopafter-tc",_tag_dynforward(_tmp84D,sizeof(char),
_get_zero_arr_size_char(_tmp84D,14)))),((_tmp852->f2=0,((_tmp852->f3=((_tmp84E="",
_tag_dynforward(_tmp84E,sizeof(char),_get_zero_arr_size_char(_tmp84E,1)))),((
_tmp852->f4=(void*)((_tmp84F=_region_malloc(r,sizeof(*_tmp84F)),((_tmp84F[0]=((
_tmp850.tag=3,((_tmp850.f1=& Cyc_tc_r,_tmp850)))),_tmp84F)))),((_tmp852->f5=((
_tmp851="Stop after type checking",_tag_dynforward(_tmp851,sizeof(char),
_get_zero_arr_size_char(_tmp851,25)))),_tmp852)))))))))))),((_tmp79E[28]=((
_tmp84C=_region_malloc(r,sizeof(*_tmp84C)),((_tmp84C->f1=((_tmp847="-stopafter-parse",
_tag_dynforward(_tmp847,sizeof(char),_get_zero_arr_size_char(_tmp847,17)))),((
_tmp84C->f2=0,((_tmp84C->f3=((_tmp848="",_tag_dynforward(_tmp848,sizeof(char),
_get_zero_arr_size_char(_tmp848,1)))),((_tmp84C->f4=(void*)((_tmp849=
_region_malloc(r,sizeof(*_tmp849)),((_tmp849[0]=((_tmp84A.tag=3,((_tmp84A.f1=&
Cyc_parseonly_r,_tmp84A)))),_tmp849)))),((_tmp84C->f5=((_tmp84B="Stop after parsing",
_tag_dynforward(_tmp84B,sizeof(char),_get_zero_arr_size_char(_tmp84B,19)))),
_tmp84C)))))))))))),((_tmp79E[27]=((_tmp846=_region_malloc(r,sizeof(*_tmp846)),((
_tmp846->f1=((_tmp841="-E",_tag_dynforward(_tmp841,sizeof(char),
_get_zero_arr_size_char(_tmp841,3)))),((_tmp846->f2=0,((_tmp846->f3=((_tmp842="",
_tag_dynforward(_tmp842,sizeof(char),_get_zero_arr_size_char(_tmp842,1)))),((
_tmp846->f4=(void*)((_tmp843=_region_malloc(r,sizeof(*_tmp843)),((_tmp843[0]=((
_tmp844.tag=3,((_tmp844.f1=& Cyc_stop_after_cpp_r,_tmp844)))),_tmp843)))),((
_tmp846->f5=((_tmp845="Stop after preprocessing",_tag_dynforward(_tmp845,sizeof(
char),_get_zero_arr_size_char(_tmp845,25)))),_tmp846)))))))))))),((_tmp79E[26]=((
_tmp840=_region_malloc(r,sizeof(*_tmp840)),((_tmp840->f1=((_tmp83B="-Wall",
_tag_dynforward(_tmp83B,sizeof(char),_get_zero_arr_size_char(_tmp83B,6)))),((
_tmp840->f2=0,((_tmp840->f3=((_tmp83C="",_tag_dynforward(_tmp83C,sizeof(char),
_get_zero_arr_size_char(_tmp83C,1)))),((_tmp840->f4=(void*)((_tmp83D=
_region_malloc(r,sizeof(*_tmp83D)),((_tmp83D[0]=((_tmp83E.tag=0,((_tmp83E.f1=Cyc_set_all_warnings,
_tmp83E)))),_tmp83D)))),((_tmp840->f5=((_tmp83F="Turn on all warnings",
_tag_dynforward(_tmp83F,sizeof(char),_get_zero_arr_size_char(_tmp83F,21)))),
_tmp840)))))))))))),((_tmp79E[25]=((_tmp83A=_region_malloc(r,sizeof(*_tmp83A)),((
_tmp83A->f1=((_tmp835="-b",_tag_dynforward(_tmp835,sizeof(char),
_get_zero_arr_size_char(_tmp835,3)))),((_tmp83A->f2=0,((_tmp83A->f3=((_tmp836="<arch>",
_tag_dynforward(_tmp836,sizeof(char),_get_zero_arr_size_char(_tmp836,7)))),((
_tmp83A->f4=(void*)((_tmp837=_region_malloc(r,sizeof(*_tmp837)),((_tmp837[0]=((
_tmp838.tag=5,((_tmp838.f1=Cyc_set_target_arch,_tmp838)))),_tmp837)))),((_tmp83A->f5=((
_tmp839="Set target architecture; implies -stopafter-toc",_tag_dynforward(
_tmp839,sizeof(char),_get_zero_arr_size_char(_tmp839,48)))),_tmp83A)))))))))))),((
_tmp79E[24]=((_tmp834=_region_malloc(r,sizeof(*_tmp834)),((_tmp834->f1=((_tmp82F="-MT",
_tag_dynforward(_tmp82F,sizeof(char),_get_zero_arr_size_char(_tmp82F,4)))),((
_tmp834->f2=0,((_tmp834->f3=((_tmp830=" <target>",_tag_dynforward(_tmp830,
sizeof(char),_get_zero_arr_size_char(_tmp830,10)))),((_tmp834->f4=(void*)((
_tmp831=_region_malloc(r,sizeof(*_tmp831)),((_tmp831[0]=((_tmp832.tag=5,((
_tmp832.f1=Cyc_set_dependencies_target,_tmp832)))),_tmp831)))),((_tmp834->f5=((
_tmp833="Give target for dependencies",_tag_dynforward(_tmp833,sizeof(char),
_get_zero_arr_size_char(_tmp833,29)))),_tmp834)))))))))))),((_tmp79E[23]=((
_tmp82E=_region_malloc(r,sizeof(*_tmp82E)),((_tmp82E->f1=((_tmp829="-MG",
_tag_dynforward(_tmp829,sizeof(char),_get_zero_arr_size_char(_tmp829,4)))),((
_tmp82E->f2=0,((_tmp82E->f3=((_tmp82A="",_tag_dynforward(_tmp82A,sizeof(char),
_get_zero_arr_size_char(_tmp82A,1)))),((_tmp82E->f4=(void*)((_tmp82B=
_region_malloc(r,sizeof(*_tmp82B)),((_tmp82B[0]=((_tmp82C.tag=1,((_tmp82C.f1=Cyc_add_cpparg,
_tmp82C)))),_tmp82B)))),((_tmp82E->f5=((_tmp82D="When producing dependencies assume that missing files are generated",
_tag_dynforward(_tmp82D,sizeof(char),_get_zero_arr_size_char(_tmp82D,68)))),
_tmp82E)))))))))))),((_tmp79E[22]=((_tmp828=_region_malloc(r,sizeof(*_tmp828)),((
_tmp828->f1=((_tmp823="-M",_tag_dynforward(_tmp823,sizeof(char),
_get_zero_arr_size_char(_tmp823,3)))),((_tmp828->f2=0,((_tmp828->f3=((_tmp824="",
_tag_dynforward(_tmp824,sizeof(char),_get_zero_arr_size_char(_tmp824,1)))),((
_tmp828->f4=(void*)((_tmp825=_region_malloc(r,sizeof(*_tmp825)),((_tmp825[0]=((
_tmp826.tag=0,((_tmp826.f1=Cyc_set_produce_dependencies,_tmp826)))),_tmp825)))),((
_tmp828->f5=((_tmp827="Produce dependencies",_tag_dynforward(_tmp827,sizeof(char),
_get_zero_arr_size_char(_tmp827,21)))),_tmp828)))))))))))),((_tmp79E[21]=((
_tmp822=_region_malloc(r,sizeof(*_tmp822)),((_tmp822->f1=((_tmp81D="-S",
_tag_dynforward(_tmp81D,sizeof(char),_get_zero_arr_size_char(_tmp81D,3)))),((
_tmp822->f2=0,((_tmp822->f3=((_tmp81E="",_tag_dynforward(_tmp81E,sizeof(char),
_get_zero_arr_size_char(_tmp81E,1)))),((_tmp822->f4=(void*)((_tmp81F=
_region_malloc(r,sizeof(*_tmp81F)),((_tmp81F[0]=((_tmp820.tag=0,((_tmp820.f1=Cyc_set_stop_after_asmfile,
_tmp820)))),_tmp81F)))),((_tmp822->f5=((_tmp821="Stop after producing assembly code",
_tag_dynforward(_tmp821,sizeof(char),_get_zero_arr_size_char(_tmp821,35)))),
_tmp822)))))))))))),((_tmp79E[20]=((_tmp81C=_region_malloc(r,sizeof(*_tmp81C)),((
_tmp81C->f1=((_tmp817="-pa",_tag_dynforward(_tmp817,sizeof(char),
_get_zero_arr_size_char(_tmp817,4)))),((_tmp81C->f2=0,((_tmp81C->f3=((_tmp818="",
_tag_dynforward(_tmp818,sizeof(char),_get_zero_arr_size_char(_tmp818,1)))),((
_tmp81C->f4=(void*)((_tmp819=_region_malloc(r,sizeof(*_tmp819)),((_tmp819[0]=((
_tmp81A.tag=0,((_tmp81A.f1=Cyc_set_pa,_tmp81A)))),_tmp819)))),((_tmp81C->f5=((
_tmp81B="Compile for profiling with aprof",_tag_dynforward(_tmp81B,sizeof(char),
_get_zero_arr_size_char(_tmp81B,33)))),_tmp81C)))))))))))),((_tmp79E[19]=((
_tmp816=_region_malloc(r,sizeof(*_tmp816)),((_tmp816->f1=((_tmp811="-pg",
_tag_dynforward(_tmp811,sizeof(char),_get_zero_arr_size_char(_tmp811,4)))),((
_tmp816->f2=0,((_tmp816->f3=((_tmp812="",_tag_dynforward(_tmp812,sizeof(char),
_get_zero_arr_size_char(_tmp812,1)))),((_tmp816->f4=(void*)((_tmp813=
_region_malloc(r,sizeof(*_tmp813)),((_tmp813[0]=((_tmp814.tag=0,((_tmp814.f1=Cyc_set_pg,
_tmp814)))),_tmp813)))),((_tmp816->f5=((_tmp815="Compile for profiling with gprof",
_tag_dynforward(_tmp815,sizeof(char),_get_zero_arr_size_char(_tmp815,33)))),
_tmp816)))))))))))),((_tmp79E[18]=((_tmp810=_region_malloc(r,sizeof(*_tmp810)),((
_tmp810->f1=((_tmp80B="-p",_tag_dynforward(_tmp80B,sizeof(char),
_get_zero_arr_size_char(_tmp80B,3)))),((_tmp810->f2=0,((_tmp810->f3=((_tmp80C="",
_tag_dynforward(_tmp80C,sizeof(char),_get_zero_arr_size_char(_tmp80C,1)))),((
_tmp810->f4=(void*)((_tmp80D=_region_malloc(r,sizeof(*_tmp80D)),((_tmp80D[0]=((
_tmp80E.tag=1,((_tmp80E.f1=Cyc_add_ccarg,_tmp80E)))),_tmp80D)))),((_tmp810->f5=((
_tmp80F="Compile for profiling with prof",_tag_dynforward(_tmp80F,sizeof(char),
_get_zero_arr_size_char(_tmp80F,32)))),_tmp810)))))))))))),((_tmp79E[17]=((
_tmp80A=_region_malloc(r,sizeof(*_tmp80A)),((_tmp80A->f1=((_tmp805="-g",
_tag_dynforward(_tmp805,sizeof(char),_get_zero_arr_size_char(_tmp805,3)))),((
_tmp80A->f2=0,((_tmp80A->f3=((_tmp806="",_tag_dynforward(_tmp806,sizeof(char),
_get_zero_arr_size_char(_tmp806,1)))),((_tmp80A->f4=(void*)((_tmp807=
_region_malloc(r,sizeof(*_tmp807)),((_tmp807[0]=((_tmp808.tag=1,((_tmp808.f1=Cyc_add_ccarg,
_tmp808)))),_tmp807)))),((_tmp80A->f5=((_tmp809="Compile for debugging",
_tag_dynforward(_tmp809,sizeof(char),_get_zero_arr_size_char(_tmp809,22)))),
_tmp80A)))))))))))),((_tmp79E[16]=((_tmp804=_region_malloc(r,sizeof(*_tmp804)),((
_tmp804->f1=((_tmp7FF="-fomit-frame-pointer",_tag_dynforward(_tmp7FF,sizeof(char),
_get_zero_arr_size_char(_tmp7FF,21)))),((_tmp804->f2=0,((_tmp804->f3=((_tmp800="",
_tag_dynforward(_tmp800,sizeof(char),_get_zero_arr_size_char(_tmp800,1)))),((
_tmp804->f4=(void*)((_tmp801=_region_malloc(r,sizeof(*_tmp801)),((_tmp801[0]=((
_tmp802.tag=1,((_tmp802.f1=Cyc_add_ccarg,_tmp802)))),_tmp801)))),((_tmp804->f5=((
_tmp803="Omit frame pointer",_tag_dynforward(_tmp803,sizeof(char),
_get_zero_arr_size_char(_tmp803,19)))),_tmp804)))))))))))),((_tmp79E[15]=((
_tmp7FE=_region_malloc(r,sizeof(*_tmp7FE)),((_tmp7FE->f1=((_tmp7F9="-O3",
_tag_dynforward(_tmp7F9,sizeof(char),_get_zero_arr_size_char(_tmp7F9,4)))),((
_tmp7FE->f2=0,((_tmp7FE->f3=((_tmp7FA="",_tag_dynforward(_tmp7FA,sizeof(char),
_get_zero_arr_size_char(_tmp7FA,1)))),((_tmp7FE->f4=(void*)((_tmp7FB=
_region_malloc(r,sizeof(*_tmp7FB)),((_tmp7FB[0]=((_tmp7FC.tag=1,((_tmp7FC.f1=Cyc_add_ccarg,
_tmp7FC)))),_tmp7FB)))),((_tmp7FE->f5=((_tmp7FD="Even more optimization",
_tag_dynforward(_tmp7FD,sizeof(char),_get_zero_arr_size_char(_tmp7FD,23)))),
_tmp7FE)))))))))))),((_tmp79E[14]=((_tmp7F8=_region_malloc(r,sizeof(*_tmp7F8)),((
_tmp7F8->f1=((_tmp7F3="-O2",_tag_dynforward(_tmp7F3,sizeof(char),
_get_zero_arr_size_char(_tmp7F3,4)))),((_tmp7F8->f2=0,((_tmp7F8->f3=((_tmp7F4="",
_tag_dynforward(_tmp7F4,sizeof(char),_get_zero_arr_size_char(_tmp7F4,1)))),((
_tmp7F8->f4=(void*)((_tmp7F5=_region_malloc(r,sizeof(*_tmp7F5)),((_tmp7F5[0]=((
_tmp7F6.tag=1,((_tmp7F6.f1=Cyc_add_ccarg,_tmp7F6)))),_tmp7F5)))),((_tmp7F8->f5=((
_tmp7F7="A higher level of optimization",_tag_dynforward(_tmp7F7,sizeof(char),
_get_zero_arr_size_char(_tmp7F7,31)))),_tmp7F8)))))))))))),((_tmp79E[13]=((
_tmp7F2=_region_malloc(r,sizeof(*_tmp7F2)),((_tmp7F2->f1=((_tmp7ED="-O",
_tag_dynforward(_tmp7ED,sizeof(char),_get_zero_arr_size_char(_tmp7ED,3)))),((
_tmp7F2->f2=0,((_tmp7F2->f3=((_tmp7EE="",_tag_dynforward(_tmp7EE,sizeof(char),
_get_zero_arr_size_char(_tmp7EE,1)))),((_tmp7F2->f4=(void*)((_tmp7EF=
_region_malloc(r,sizeof(*_tmp7EF)),((_tmp7EF[0]=((_tmp7F0.tag=1,((_tmp7F0.f1=Cyc_add_ccarg,
_tmp7F0)))),_tmp7EF)))),((_tmp7F2->f5=((_tmp7F1="Optimize",_tag_dynforward(
_tmp7F1,sizeof(char),_get_zero_arr_size_char(_tmp7F1,9)))),_tmp7F2)))))))))))),((
_tmp79E[12]=((_tmp7EC=_region_malloc(r,sizeof(*_tmp7EC)),((_tmp7EC->f1=((_tmp7E7="-O0",
_tag_dynforward(_tmp7E7,sizeof(char),_get_zero_arr_size_char(_tmp7E7,4)))),((
_tmp7EC->f2=0,((_tmp7EC->f3=((_tmp7E8="",_tag_dynforward(_tmp7E8,sizeof(char),
_get_zero_arr_size_char(_tmp7E8,1)))),((_tmp7EC->f4=(void*)((_tmp7E9=
_region_malloc(r,sizeof(*_tmp7E9)),((_tmp7E9[0]=((_tmp7EA.tag=1,((_tmp7EA.f1=Cyc_add_ccarg,
_tmp7EA)))),_tmp7E9)))),((_tmp7EC->f5=((_tmp7EB="Don't optimize",_tag_dynforward(
_tmp7EB,sizeof(char),_get_zero_arr_size_char(_tmp7EB,15)))),_tmp7EC)))))))))))),((
_tmp79E[11]=((_tmp7E6=_region_malloc(r,sizeof(*_tmp7E6)),((_tmp7E6->f1=((_tmp7E1="-s",
_tag_dynforward(_tmp7E1,sizeof(char),_get_zero_arr_size_char(_tmp7E1,3)))),((
_tmp7E6->f2=0,((_tmp7E6->f3=((_tmp7E2="",_tag_dynforward(_tmp7E2,sizeof(char),
_get_zero_arr_size_char(_tmp7E2,1)))),((_tmp7E6->f4=(void*)((_tmp7E3=
_region_malloc(r,sizeof(*_tmp7E3)),((_tmp7E3[0]=((_tmp7E4.tag=1,((_tmp7E4.f1=Cyc_add_ccarg,
_tmp7E4)))),_tmp7E3)))),((_tmp7E6->f5=((_tmp7E5="Remove all symbol table and relocation info from executable",
_tag_dynforward(_tmp7E5,sizeof(char),_get_zero_arr_size_char(_tmp7E5,60)))),
_tmp7E6)))))))))))),((_tmp79E[10]=((_tmp7E0=_region_malloc(r,sizeof(*_tmp7E0)),((
_tmp7E0->f1=((_tmp7DB="-x",_tag_dynforward(_tmp7DB,sizeof(char),
_get_zero_arr_size_char(_tmp7DB,3)))),((_tmp7E0->f2=0,((_tmp7E0->f3=((_tmp7DC=" <language>",
_tag_dynforward(_tmp7DC,sizeof(char),_get_zero_arr_size_char(_tmp7DC,12)))),((
_tmp7E0->f4=(void*)((_tmp7DD=_region_malloc(r,sizeof(*_tmp7DD)),((_tmp7DD[0]=((
_tmp7DE.tag=5,((_tmp7DE.f1=Cyc_set_inputtype,_tmp7DE)))),_tmp7DD)))),((_tmp7E0->f5=((
_tmp7DF="Specify <language> for the following input files",_tag_dynforward(
_tmp7DF,sizeof(char),_get_zero_arr_size_char(_tmp7DF,49)))),_tmp7E0)))))))))))),((
_tmp79E[9]=((_tmp7DA=_region_malloc(r,sizeof(*_tmp7DA)),((_tmp7DA->f1=((_tmp7D5="-c",
_tag_dynforward(_tmp7D5,sizeof(char),_get_zero_arr_size_char(_tmp7D5,3)))),((
_tmp7DA->f2=0,((_tmp7DA->f3=((_tmp7D6="",_tag_dynforward(_tmp7D6,sizeof(char),
_get_zero_arr_size_char(_tmp7D6,1)))),((_tmp7DA->f4=(void*)((_tmp7D7=
_region_malloc(r,sizeof(*_tmp7D7)),((_tmp7D7[0]=((_tmp7D8.tag=0,((_tmp7D8.f1=Cyc_set_stop_after_objectfile,
_tmp7D8)))),_tmp7D7)))),((_tmp7DA->f5=((_tmp7D9="Produce an object file instead of an executable; do not link",
_tag_dynforward(_tmp7D9,sizeof(char),_get_zero_arr_size_char(_tmp7D9,61)))),
_tmp7DA)))))))))))),((_tmp79E[8]=((_tmp7D4=_region_malloc(r,sizeof(*_tmp7D4)),((
_tmp7D4->f1=((_tmp7CF="-m",_tag_dynforward(_tmp7CF,sizeof(char),
_get_zero_arr_size_char(_tmp7CF,3)))),((_tmp7D4->f2=1,((_tmp7D4->f3=((_tmp7D0="<option>",
_tag_dynforward(_tmp7D0,sizeof(char),_get_zero_arr_size_char(_tmp7D0,9)))),((
_tmp7D4->f4=(void*)((_tmp7D1=_region_malloc(r,sizeof(*_tmp7D1)),((_tmp7D1[0]=((
_tmp7D2.tag=1,((_tmp7D2.f1=Cyc_add_marg,_tmp7D2)))),_tmp7D1)))),((_tmp7D4->f5=((
_tmp7D3="GCC specific: pass machine-dependent flag on to gcc",_tag_dynforward(
_tmp7D3,sizeof(char),_get_zero_arr_size_char(_tmp7D3,52)))),_tmp7D4)))))))))))),((
_tmp79E[7]=((_tmp7CE=_region_malloc(r,sizeof(*_tmp7CE)),((_tmp7CE->f1=((_tmp7C9="-l",
_tag_dynforward(_tmp7C9,sizeof(char),_get_zero_arr_size_char(_tmp7C9,3)))),((
_tmp7CE->f2=1,((_tmp7CE->f3=((_tmp7CA="<libname>",_tag_dynforward(_tmp7CA,
sizeof(char),_get_zero_arr_size_char(_tmp7CA,10)))),((_tmp7CE->f4=(void*)((
_tmp7CB=_region_malloc(r,sizeof(*_tmp7CB)),((_tmp7CB[0]=((_tmp7CC.tag=1,((
_tmp7CC.f1=Cyc_add_libarg,_tmp7CC)))),_tmp7CB)))),((_tmp7CE->f5=((_tmp7CD="Library file",
_tag_dynforward(_tmp7CD,sizeof(char),_get_zero_arr_size_char(_tmp7CD,13)))),
_tmp7CE)))))))))))),((_tmp79E[6]=((_tmp7C8=_region_malloc(r,sizeof(*_tmp7C8)),((
_tmp7C8->f1=((_tmp7C3="-L",_tag_dynforward(_tmp7C3,sizeof(char),
_get_zero_arr_size_char(_tmp7C3,3)))),((_tmp7C8->f2=1,((_tmp7C8->f3=((_tmp7C4="<dir>",
_tag_dynforward(_tmp7C4,sizeof(char),_get_zero_arr_size_char(_tmp7C4,6)))),((
_tmp7C8->f4=(void*)((_tmp7C5=_region_malloc(r,sizeof(*_tmp7C5)),((_tmp7C5[0]=((
_tmp7C6.tag=1,((_tmp7C6.f1=Cyc_add_ccarg,_tmp7C6)))),_tmp7C5)))),((_tmp7C8->f5=((
_tmp7C7="Add to the list of directories for -l",_tag_dynforward(_tmp7C7,sizeof(
char),_get_zero_arr_size_char(_tmp7C7,38)))),_tmp7C8)))))))))))),((_tmp79E[5]=((
_tmp7C2=_region_malloc(r,sizeof(*_tmp7C2)),((_tmp7C2->f1=((_tmp7BD="-I",
_tag_dynforward(_tmp7BD,sizeof(char),_get_zero_arr_size_char(_tmp7BD,3)))),((
_tmp7C2->f2=1,((_tmp7C2->f3=((_tmp7BE="<dir>",_tag_dynforward(_tmp7BE,sizeof(
char),_get_zero_arr_size_char(_tmp7BE,6)))),((_tmp7C2->f4=(void*)((_tmp7BF=
_region_malloc(r,sizeof(*_tmp7BF)),((_tmp7BF[0]=((_tmp7C0.tag=1,((_tmp7C0.f1=Cyc_add_cpparg,
_tmp7C0)))),_tmp7BF)))),((_tmp7C2->f5=((_tmp7C1="Add to the list of directories to search for include files",
_tag_dynforward(_tmp7C1,sizeof(char),_get_zero_arr_size_char(_tmp7C1,59)))),
_tmp7C2)))))))))))),((_tmp79E[4]=((_tmp7BC=_region_malloc(r,sizeof(*_tmp7BC)),((
_tmp7BC->f1=((_tmp7B7="-B",_tag_dynforward(_tmp7B7,sizeof(char),
_get_zero_arr_size_char(_tmp7B7,3)))),((_tmp7BC->f2=1,((_tmp7BC->f3=((_tmp7B8="<file>",
_tag_dynforward(_tmp7B8,sizeof(char),_get_zero_arr_size_char(_tmp7B8,7)))),((
_tmp7BC->f4=(void*)((_tmp7B9=_region_malloc(r,sizeof(*_tmp7B9)),((_tmp7B9[0]=((
_tmp7BA.tag=1,((_tmp7BA.f1=Cyc_add_cyclone_exec_path,_tmp7BA)))),_tmp7B9)))),((
_tmp7BC->f5=((_tmp7BB="Add to the list of directories to search for compiler files",
_tag_dynforward(_tmp7BB,sizeof(char),_get_zero_arr_size_char(_tmp7BB,60)))),
_tmp7BC)))))))))))),((_tmp79E[3]=((_tmp7B6=_region_malloc(r,sizeof(*_tmp7B6)),((
_tmp7B6->f1=((_tmp7B1="-D",_tag_dynforward(_tmp7B1,sizeof(char),
_get_zero_arr_size_char(_tmp7B1,3)))),((_tmp7B6->f2=1,((_tmp7B6->f3=((_tmp7B2="<name>[=<value>]",
_tag_dynforward(_tmp7B2,sizeof(char),_get_zero_arr_size_char(_tmp7B2,17)))),((
_tmp7B6->f4=(void*)((_tmp7B3=_region_malloc(r,sizeof(*_tmp7B3)),((_tmp7B3[0]=((
_tmp7B4.tag=1,((_tmp7B4.f1=Cyc_add_cpparg,_tmp7B4)))),_tmp7B3)))),((_tmp7B6->f5=((
_tmp7B5="Pass definition to preprocessor",_tag_dynforward(_tmp7B5,sizeof(char),
_get_zero_arr_size_char(_tmp7B5,32)))),_tmp7B6)))))))))))),((_tmp79E[2]=((
_tmp7B0=_region_malloc(r,sizeof(*_tmp7B0)),((_tmp7B0->f1=((_tmp7AB="-o",
_tag_dynforward(_tmp7AB,sizeof(char),_get_zero_arr_size_char(_tmp7AB,3)))),((
_tmp7B0->f2=0,((_tmp7B0->f3=((_tmp7AC=" <file>",_tag_dynforward(_tmp7AC,sizeof(
char),_get_zero_arr_size_char(_tmp7AC,8)))),((_tmp7B0->f4=(void*)((_tmp7AD=
_region_malloc(r,sizeof(*_tmp7AD)),((_tmp7AD[0]=((_tmp7AE.tag=5,((_tmp7AE.f1=Cyc_set_output_file,
_tmp7AE)))),_tmp7AD)))),((_tmp7B0->f5=((_tmp7AF="Set the output file name to <file>",
_tag_dynforward(_tmp7AF,sizeof(char),_get_zero_arr_size_char(_tmp7AF,35)))),
_tmp7B0)))))))))))),((_tmp79E[1]=((_tmp7AA=_region_malloc(r,sizeof(*_tmp7AA)),((
_tmp7AA->f1=((_tmp7A5="--version",_tag_dynforward(_tmp7A5,sizeof(char),
_get_zero_arr_size_char(_tmp7A5,10)))),((_tmp7AA->f2=0,((_tmp7AA->f3=((_tmp7A6="",
_tag_dynforward(_tmp7A6,sizeof(char),_get_zero_arr_size_char(_tmp7A6,1)))),((
_tmp7AA->f4=(void*)((_tmp7A7=_region_malloc(r,sizeof(*_tmp7A7)),((_tmp7A7[0]=((
_tmp7A8.tag=0,((_tmp7A8.f1=Cyc_print_version,_tmp7A8)))),_tmp7A7)))),((_tmp7AA->f5=((
_tmp7A9="Print version information and exit",_tag_dynforward(_tmp7A9,sizeof(char),
_get_zero_arr_size_char(_tmp7A9,35)))),_tmp7AA)))))))))))),((_tmp79E[0]=((
_tmp7A4=_region_malloc(r,sizeof(*_tmp7A4)),((_tmp7A4->f1=((_tmp79F="-v",
_tag_dynforward(_tmp79F,sizeof(char),_get_zero_arr_size_char(_tmp79F,3)))),((
_tmp7A4->f2=0,((_tmp7A4->f3=((_tmp7A0="",_tag_dynforward(_tmp7A0,sizeof(char),
_get_zero_arr_size_char(_tmp7A0,1)))),((_tmp7A4->f4=(void*)((_tmp7A1=
_region_malloc(r,sizeof(*_tmp7A1)),((_tmp7A1[0]=((_tmp7A2.tag=3,((_tmp7A2.f1=&
Cyc_v_r,_tmp7A2)))),_tmp7A1)))),((_tmp7A4->f5=((_tmp7A3="Print compilation stages verbosely",
_tag_dynforward(_tmp7A3,sizeof(char),_get_zero_arr_size_char(_tmp7A3,35)))),
_tmp7A4)))))))))))),((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_dynforward_ptr))Cyc_List_rlist)(r,_tag_dynforward(_tmp79E,sizeof(struct _tuple15*),
64)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))));
const char*_tmp91F;struct _dynforward_ptr optstring=(_tmp91F="Options:",
_tag_dynforward(_tmp91F,sizeof(char),_get_zero_arr_size_char(_tmp91F,9)));Cyc_Arg_current=
0;Cyc_Arg_parse(options,Cyc_add_other,optstring,_tmp14E);if(Cyc_target_arch.curr
== ((struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr)Cyc_target_arch=(struct
_dynforward_ptr)Cstring_to_string(Carch);{char*_tmp920;struct _dynforward_ptr
cyclone_exec_prefix=(_tmp920=getenv((const char*)"CYCLONE_EXEC_PREFIX"),
_tag_dynforward(_tmp920,sizeof(char),_get_zero_arr_size_char(_tmp920,1)));if(
cyclone_exec_prefix.curr != ((struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr){
struct _dynforward_ptr*_tmp923;struct Cyc_List_List*_tmp922;Cyc_cyclone_exec_path=((
_tmp922=_cycalloc(sizeof(*_tmp922)),((_tmp922->hd=((_tmp923=_cycalloc(sizeof(*
_tmp923)),((_tmp923[0]=cyclone_exec_prefix,_tmp923)))),((_tmp922->tl=Cyc_cyclone_exec_path,
_tmp922))))));}def_lib_path=(struct _dynforward_ptr)Cstring_to_string(
Cdef_lib_path);if(Cyc_strlen((struct _dynforward_ptr)def_lib_path)> 0){struct
_dynforward_ptr*_tmp929;const char*_tmp928;struct Cyc_List_List*_tmp927;Cyc_cyclone_exec_path=((
_tmp927=_cycalloc(sizeof(*_tmp927)),((_tmp927->hd=((_tmp929=_cycalloc(sizeof(*
_tmp929)),((_tmp929[0]=(struct _dynforward_ptr)Cyc_Filename_concat(def_lib_path,((
_tmp928="cyc-lib",_tag_dynforward(_tmp928,sizeof(char),_get_zero_arr_size_char(
_tmp928,8))))),_tmp929)))),((_tmp927->tl=Cyc_cyclone_exec_path,_tmp927))))));}
Cyc_cyclone_exec_path=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_cyclone_exec_path);cyclone_arch_path=Cyc_also_subdir(Cyc_cyclone_exec_path,
Cyc_target_arch);{const char*_tmp92A;struct _dynforward_ptr _tmp155=Cyc_do_find(
cyclone_arch_path,((_tmp92A="cycspecs",_tag_dynforward(_tmp92A,sizeof(char),
_get_zero_arr_size_char(_tmp92A,9)))));if(Cyc_v_r){const char*_tmp92E;void*
_tmp92D[1];struct Cyc_String_pa_struct _tmp92C;(_tmp92C.tag=0,((_tmp92C.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp155),((_tmp92D[0]=& _tmp92C,Cyc_fprintf(
Cyc_stderr,((_tmp92E="Reading from specs file %s\n",_tag_dynforward(_tmp92E,
sizeof(char),_get_zero_arr_size_char(_tmp92E,28)))),_tag_dynforward(_tmp92D,
sizeof(void*),1)))))));}{struct Cyc_List_List*_tmp159=Cyc_read_specs(_tmp155);
struct _dynforward_ptr _tmp15A=_tag_dynforward(0,0,0);{struct _handler_cons _tmp15B;
_push_handler(& _tmp15B);{int _tmp15D=0;if(setjmp(_tmp15B.handler))_tmp15D=1;if(!
_tmp15D){{struct _dynforward_ptr _tmp15E=*((struct _dynforward_ptr*(*)(int(*cmp)(
struct _dynforward_ptr*,struct _dynforward_ptr*),struct Cyc_List_List*l,struct
_dynforward_ptr*x))Cyc_List_assoc_cmp)(Cyc_strptrcmp,_tmp159,
_init_dynforward_ptr(_cycalloc(sizeof(struct _dynforward_ptr)),"cyclone",sizeof(
char),8));_tmp15A=Cyc_split_specs(_tmp15E);};_pop_handler();}else{void*_tmp15C=(
void*)_exn_thrown;void*_tmp161=_tmp15C;_LL36: if(_tmp161 != Cyc_Core_Not_found)
goto _LL38;_LL37: goto _LL35;_LL38:;_LL39:(void)_throw(_tmp161);_LL35:;}}}if(
_tmp15A.curr != (_tag_dynforward(0,0,0)).curr){Cyc_Arg_current=0;Cyc_Arg_parse(
options,Cyc_add_other,optstring,_tmp15A);}Cyc_Arg_current=0;Cyc_Arg_parse(
options,Cyc_add_other,optstring,_tmp14F);{const char*_tmp92F;if(Cyc_strcmp((
struct _dynforward_ptr)Cyc_cpp,((_tmp92F="",_tag_dynforward(_tmp92F,sizeof(char),
_get_zero_arr_size_char(_tmp92F,1)))))== 0){const char*_tmp934;void*_tmp933[2];
struct Cyc_String_pa_struct _tmp932;struct Cyc_String_pa_struct _tmp931;Cyc_set_cpp((
struct _dynforward_ptr)((_tmp931.tag=0,((_tmp931.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)_tmp155),((_tmp932.tag=0,((_tmp932.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)comp),((_tmp933[0]=& _tmp932,((_tmp933[1]=&
_tmp931,Cyc_aprintf(((_tmp934="%s -w -x c -E -specs %s",_tag_dynforward(_tmp934,
sizeof(char),_get_zero_arr_size_char(_tmp934,24)))),_tag_dynforward(_tmp933,
sizeof(void*),2)))))))))))))));}}}}}}}};_pop_region(r);}if(Cyc_cyclone_files == 0
 && Cyc_ccargs == 0){{const char*_tmp937;void*_tmp936;(_tmp936=0,Cyc_fprintf(Cyc_stderr,((
_tmp937="missing file\n",_tag_dynforward(_tmp937,sizeof(char),
_get_zero_arr_size_char(_tmp937,14)))),_tag_dynforward(_tmp936,sizeof(void*),0)));}
exit(1);}if(!Cyc_stop_after_cpp_r  && !Cyc_nocyc_setjmp_r){const char*_tmp938;Cyc_cyc_setjmp=
Cyc_do_find(cyclone_arch_path,((_tmp938="cyc_setjmp.h",_tag_dynforward(_tmp938,
sizeof(char),_get_zero_arr_size_char(_tmp938,13)))));}if(!Cyc_stop_after_cpp_r
 && Cyc_strlen((struct _dynforward_ptr)Cyc_cyc_include)== 0){const char*_tmp939;
Cyc_cyc_include=Cyc_do_find(Cyc_cyclone_exec_path,((_tmp939="cyc_include.h",
_tag_dynforward(_tmp939,sizeof(char),_get_zero_arr_size_char(_tmp939,14)))));}{
struct Cyc_List_List*_tmp2FB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(
Cyc_cyclone_files);for(0;_tmp2FB != 0;_tmp2FB=_tmp2FB->tl){Cyc_process_file(*((
struct _dynforward_ptr*)_tmp2FB->hd));if(Cyc_compile_failure)return 1;}}if(((Cyc_stop_after_cpp_r
 || Cyc_parseonly_r) || Cyc_tc_r) || Cyc_toc_r)return 0;if(Cyc_ccargs == 0)return 0;{
const char*_tmp93A;Cyc_add_ccarg((struct _dynforward_ptr)Cyc_strconcat(((_tmp93A="-L",
_tag_dynforward(_tmp93A,sizeof(char),_get_zero_arr_size_char(_tmp93A,3)))),(
struct _dynforward_ptr)def_lib_path));}Cyc_ccargs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{const char*_tmp93B;struct
_dynforward_ptr _tmp2FD=Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct
_dynforward_ptr*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,Cyc_ccargs),((_tmp93B=" ",_tag_dynforward(_tmp93B,
sizeof(char),_get_zero_arr_size_char(_tmp93B,2)))));Cyc_libargs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_libargs);{const char*_tmp93D;struct Cyc_List_List*
_tmp93C;struct _dynforward_ptr _tmp2FE=Cyc_str_sepstr(((_tmp93C=_cycalloc(sizeof(*
_tmp93C)),((_tmp93C->hd=_init_dynforward_ptr(_cycalloc(sizeof(struct
_dynforward_ptr)),"",sizeof(char),1),((_tmp93C->tl=((struct Cyc_List_List*(*)(
struct _dynforward_ptr*(*f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_sh_escape_stringptr,Cyc_libargs),_tmp93C)))))),((_tmp93D=" ",_tag_dynforward(
_tmp93D,sizeof(char),_get_zero_arr_size_char(_tmp93D,2)))));struct Cyc_List_List*
stdlib;struct _dynforward_ptr stdlib_string;const char*_tmp93F;const char*_tmp93E;
int _tmp2FF=((Cyc_stop_after_asmfile_r  || Cyc_stop_after_objectfile_r) || Cyc_output_file
!= 0  && Cyc_Filename_check_suffix(*((struct _dynforward_ptr*)_check_null(Cyc_output_file)),((
_tmp93F=".a",_tag_dynforward(_tmp93F,sizeof(char),_get_zero_arr_size_char(
_tmp93F,3)))))) || Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct
_dynforward_ptr*)_check_null(Cyc_output_file)),((_tmp93E=".lib",_tag_dynforward(
_tmp93E,sizeof(char),_get_zero_arr_size_char(_tmp93E,5)))));if(_tmp2FF){stdlib=0;{
const char*_tmp940;stdlib_string=(struct _dynforward_ptr)((_tmp940="",
_tag_dynforward(_tmp940,sizeof(char),_get_zero_arr_size_char(_tmp940,1))));}}
else{struct _dynforward_ptr libcyc_flag;struct _dynforward_ptr nogc_filename;struct
_dynforward_ptr runtime_filename;struct _dynforward_ptr gc_filename;if(Cyc_pa_r){{
const char*_tmp941;libcyc_flag=((_tmp941="-lcyc_a",_tag_dynforward(_tmp941,
sizeof(char),_get_zero_arr_size_char(_tmp941,8))));}{const char*_tmp942;
nogc_filename=((_tmp942="nogc_a.a",_tag_dynforward(_tmp942,sizeof(char),
_get_zero_arr_size_char(_tmp942,9))));}{const char*_tmp943;runtime_filename=((
_tmp943="runtime_cyc_a.o",_tag_dynforward(_tmp943,sizeof(char),
_get_zero_arr_size_char(_tmp943,16))));}}else{if(Cyc_nocheck_r){{const char*
_tmp944;libcyc_flag=((_tmp944="-lcyc_nocheck",_tag_dynforward(_tmp944,sizeof(
char),_get_zero_arr_size_char(_tmp944,14))));}{const char*_tmp945;nogc_filename=((
_tmp945="nogc_nocheck.a",_tag_dynforward(_tmp945,sizeof(char),
_get_zero_arr_size_char(_tmp945,15))));}{const char*_tmp946;runtime_filename=((
_tmp946="runtime_cyc.o",_tag_dynforward(_tmp946,sizeof(char),
_get_zero_arr_size_char(_tmp946,14))));}}else{if(Cyc_pg_r){{const char*_tmp947;
libcyc_flag=((_tmp947="-lcyc_pg",_tag_dynforward(_tmp947,sizeof(char),
_get_zero_arr_size_char(_tmp947,9))));}{const char*_tmp948;runtime_filename=((
_tmp948="runtime_cyc_pg.o",_tag_dynforward(_tmp948,sizeof(char),
_get_zero_arr_size_char(_tmp948,17))));}{const char*_tmp949;nogc_filename=((
_tmp949="nogc_pg.a",_tag_dynforward(_tmp949,sizeof(char),_get_zero_arr_size_char(
_tmp949,10))));}}else{{const char*_tmp94A;libcyc_flag=((_tmp94A="-lcyc",
_tag_dynforward(_tmp94A,sizeof(char),_get_zero_arr_size_char(_tmp94A,6))));}{
const char*_tmp94B;nogc_filename=((_tmp94B="nogc.a",_tag_dynforward(_tmp94B,
sizeof(char),_get_zero_arr_size_char(_tmp94B,7))));}{const char*_tmp94C;
runtime_filename=((_tmp94C="runtime_cyc.o",_tag_dynforward(_tmp94C,sizeof(char),
_get_zero_arr_size_char(_tmp94C,14))));}}}}{const char*_tmp94D;gc_filename=((
_tmp94D="gc.a",_tag_dynforward(_tmp94D,sizeof(char),_get_zero_arr_size_char(
_tmp94D,5))));}{struct _dynforward_ptr _tmp30E=Cyc_nogc_r?Cyc_do_find(
cyclone_arch_path,nogc_filename): Cyc_do_find(cyclone_arch_path,gc_filename);
struct _dynforward_ptr _tmp30F=Cyc_do_find(cyclone_arch_path,runtime_filename);
stdlib=0;{const char*_tmp953;void*_tmp952[3];struct Cyc_String_pa_struct _tmp951;
struct Cyc_String_pa_struct _tmp950;struct Cyc_String_pa_struct _tmp94F;
stdlib_string=(struct _dynforward_ptr)((_tmp94F.tag=0,((_tmp94F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp30E),((_tmp950.tag=0,((_tmp950.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)libcyc_flag),((_tmp951.tag=0,((
_tmp951.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmp30F),((_tmp952[0]=&
_tmp951,((_tmp952[1]=& _tmp950,((_tmp952[2]=& _tmp94F,Cyc_aprintf(((_tmp953=" %s %s %s",
_tag_dynforward(_tmp953,sizeof(char),_get_zero_arr_size_char(_tmp953,10)))),
_tag_dynforward(_tmp952,sizeof(void*),3))))))))))))))))))));}}}if(Cyc_ic_r){
struct _handler_cons _tmp315;_push_handler(& _tmp315);{int _tmp317=0;if(setjmp(
_tmp315.handler))_tmp317=1;if(!_tmp317){Cyc_ccargs=((struct Cyc_List_List*(*)(int(*
f)(struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,
Cyc_ccargs);Cyc_libargs=((struct Cyc_List_List*(*)(int(*f)(struct _dynforward_ptr*),
struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{struct Cyc_List_List*
_tmp318=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));if(!_tmp2FF){struct
Cyc_List_List*_tmp954;_tmp318=((_tmp954=_cycalloc(sizeof(*_tmp954)),((_tmp954->hd=
Cyc_final_strptr,((_tmp954->tl=_tmp318,_tmp954))))));}{struct Cyc_Interface_I*
_tmp31A=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct
_dynforward_ptr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(
Cyc_read_cycio,_tmp318,_tmp318);if(_tmp31A == 0){{const char*_tmp957;void*_tmp956;(
_tmp956=0,Cyc_fprintf(Cyc_stderr,((_tmp957="Error: interfaces incompatible\n",
_tag_dynforward(_tmp957,sizeof(char),_get_zero_arr_size_char(_tmp957,32)))),
_tag_dynforward(_tmp956,sizeof(void*),0)));}Cyc_compile_failure=1;Cyc_remove_cfiles();{
int _tmp31D=1;_npop_handler(0);return _tmp31D;}}if(_tmp2FF){if(Cyc_output_file != 0){
const char*_tmp95B;void*_tmp95A[1];struct Cyc_String_pa_struct _tmp959;struct
_dynforward_ptr _tmp31E=(_tmp959.tag=0,((_tmp959.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Filename_chop_extension(*((struct _dynforward_ptr*)
_check_null(Cyc_output_file)))),((_tmp95A[0]=& _tmp959,Cyc_aprintf(((_tmp95B="%s.cycio",
_tag_dynforward(_tmp95B,sizeof(char),_get_zero_arr_size_char(_tmp95B,9)))),
_tag_dynforward(_tmp95A,sizeof(void*),1)))))));const char*_tmp95D;const char*
_tmp95C;struct Cyc___cycFILE*_tmp31F=Cyc_try_file_open((struct _dynforward_ptr)
_tmp31E,((_tmp95C="wb",_tag_dynforward(_tmp95C,sizeof(char),
_get_zero_arr_size_char(_tmp95C,3)))),((_tmp95D="interface object file",
_tag_dynforward(_tmp95D,sizeof(char),_get_zero_arr_size_char(_tmp95D,22)))));if(
_tmp31F == 0){Cyc_compile_failure=1;Cyc_remove_cfiles();{int _tmp320=1;
_npop_handler(0);return _tmp320;}}Cyc_Interface_save((struct Cyc_Interface_I*)
_tmp31A,(struct Cyc___cycFILE*)_tmp31F);Cyc_file_close((struct Cyc___cycFILE*)
_tmp31F);}}else{const char*_tmp962;const char*_tmp961;struct _tuple11*_tmp960;if(!
Cyc_Interface_is_subinterface(Cyc_Interface_empty(),(struct Cyc_Interface_I*)
_tmp31A,((_tmp960=_cycalloc(sizeof(*_tmp960)),((_tmp960->f1=((_tmp962="empty interface",
_tag_dynforward(_tmp962,sizeof(char),_get_zero_arr_size_char(_tmp962,16)))),((
_tmp960->f2=((_tmp961="global interface",_tag_dynforward(_tmp961,sizeof(char),
_get_zero_arr_size_char(_tmp961,17)))),_tmp960)))))))){{const char*_tmp965;void*
_tmp964;(_tmp964=0,Cyc_fprintf(Cyc_stderr,((_tmp965="Error: some objects are still undefined\n",
_tag_dynforward(_tmp965,sizeof(char),_get_zero_arr_size_char(_tmp965,41)))),
_tag_dynforward(_tmp964,sizeof(void*),0)));}Cyc_compile_failure=1;Cyc_remove_cfiles();{
int _tmp32B=1;_npop_handler(0);return _tmp32B;}}}}};_pop_handler();}else{void*
_tmp316=(void*)_exn_thrown;void*_tmp32D=_tmp316;_LL3B:;_LL3C:{void*_tmp32E=
_tmp32D;struct _dynforward_ptr _tmp32F;struct _dynforward_ptr _tmp330;struct
_dynforward_ptr _tmp331;_LL40: if(*((void**)_tmp32E)!= Cyc_Core_Failure)goto _LL42;
_tmp32F=((struct Cyc_Core_Failure_struct*)_tmp32E)->f1;_LL41:{const char*_tmp969;
void*_tmp968[1];struct Cyc_String_pa_struct _tmp967;(_tmp967.tag=0,((_tmp967.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmp32F),((_tmp968[0]=& _tmp967,Cyc_fprintf(
Cyc_stderr,((_tmp969="Exception Core::Failure %s\n",_tag_dynforward(_tmp969,
sizeof(char),_get_zero_arr_size_char(_tmp969,28)))),_tag_dynforward(_tmp968,
sizeof(void*),1)))))));}goto _LL3F;_LL42: if(*((void**)_tmp32E)!= Cyc_Core_Impossible)
goto _LL44;_tmp330=((struct Cyc_Core_Impossible_struct*)_tmp32E)->f1;_LL43:{const
char*_tmp96D;void*_tmp96C[1];struct Cyc_String_pa_struct _tmp96B;(_tmp96B.tag=0,((
_tmp96B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmp330),((_tmp96C[0]=&
_tmp96B,Cyc_fprintf(Cyc_stderr,((_tmp96D="Exception Core::Impossible %s\n",
_tag_dynforward(_tmp96D,sizeof(char),_get_zero_arr_size_char(_tmp96D,31)))),
_tag_dynforward(_tmp96C,sizeof(void*),1)))))));}goto _LL3F;_LL44: if(_tmp32E != Cyc_Dict_Absent)
goto _LL46;_LL45:{const char*_tmp970;void*_tmp96F;(_tmp96F=0,Cyc_fprintf(Cyc_stderr,((
_tmp970="Exception Dict::Absent\n",_tag_dynforward(_tmp970,sizeof(char),
_get_zero_arr_size_char(_tmp970,24)))),_tag_dynforward(_tmp96F,sizeof(void*),0)));}
goto _LL3F;_LL46: if(*((void**)_tmp32E)!= Cyc_Core_Invalid_argument)goto _LL48;
_tmp331=((struct Cyc_Core_Invalid_argument_struct*)_tmp32E)->f1;_LL47:{const char*
_tmp974;void*_tmp973[1];struct Cyc_String_pa_struct _tmp972;(_tmp972.tag=0,((
_tmp972.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmp331),((_tmp973[0]=&
_tmp972,Cyc_fprintf(Cyc_stderr,((_tmp974="Exception Core::Invalid_argument %s\n",
_tag_dynforward(_tmp974,sizeof(char),_get_zero_arr_size_char(_tmp974,37)))),
_tag_dynforward(_tmp973,sizeof(void*),1)))))));}goto _LL3F;_LL48:;_LL49:{const
char*_tmp977;void*_tmp976;(_tmp976=0,Cyc_fprintf(Cyc_stderr,((_tmp977="Uncaught exception\n",
_tag_dynforward(_tmp977,sizeof(char),_get_zero_arr_size_char(_tmp977,20)))),
_tag_dynforward(_tmp976,sizeof(void*),0)));}goto _LL3F;_LL3F:;}Cyc_compile_failure=
1;Cyc_remove_cfiles();return 1;_LL3D:;_LL3E:(void)_throw(_tmp32D);_LL3A:;}}}{
const char*_tmp978;struct _dynforward_ptr outfile_str=(_tmp978="",_tag_dynforward(
_tmp978,sizeof(char),_get_zero_arr_size_char(_tmp978,1)));if(Cyc_output_file != 0){
const char*_tmp97C;void*_tmp97B[1];struct Cyc_String_pa_struct _tmp97A;outfile_str=(
struct _dynforward_ptr)((_tmp97A.tag=0,((_tmp97A.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_sh_escape_string(*((struct _dynforward_ptr*)_check_null(
Cyc_output_file)))),((_tmp97B[0]=& _tmp97A,Cyc_aprintf(((_tmp97C=" -o %s",
_tag_dynforward(_tmp97C,sizeof(char),_get_zero_arr_size_char(_tmp97C,7)))),
_tag_dynforward(_tmp97B,sizeof(void*),1))))))));}{const char*_tmp984;void*_tmp983[
5];struct Cyc_String_pa_struct _tmp982;struct Cyc_String_pa_struct _tmp981;struct Cyc_String_pa_struct
_tmp980;struct Cyc_String_pa_struct _tmp97F;struct Cyc_String_pa_struct _tmp97E;
struct _dynforward_ptr _tmp342=(_tmp97E.tag=0,((_tmp97E.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)_tmp2FE),((_tmp97F.tag=0,((_tmp97F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)stdlib_string),((_tmp980.tag=0,((_tmp980.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmp2FD),((_tmp981.tag=0,((_tmp981.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)outfile_str),((_tmp982.tag=0,((
_tmp982.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)comp),((_tmp983[0]=&
_tmp982,((_tmp983[1]=& _tmp981,((_tmp983[2]=& _tmp980,((_tmp983[3]=& _tmp97F,((
_tmp983[4]=& _tmp97E,Cyc_aprintf(((_tmp984="%s %s %s%s%s",_tag_dynforward(_tmp984,
sizeof(char),_get_zero_arr_size_char(_tmp984,13)))),_tag_dynforward(_tmp983,
sizeof(void*),5)))))))))))))))))))))))))))))));if(Cyc_v_r){{const char*_tmp988;
void*_tmp987[1];struct Cyc_String_pa_struct _tmp986;(_tmp986.tag=0,((_tmp986.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmp342),((_tmp987[0]=& _tmp986,Cyc_fprintf(
Cyc_stderr,((_tmp988="%s\n",_tag_dynforward(_tmp988,sizeof(char),
_get_zero_arr_size_char(_tmp988,4)))),_tag_dynforward(_tmp987,sizeof(void*),1)))))));}
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}if(system((const char*)
_untag_dynforward_ptr(_tmp342,sizeof(char),1))!= 0){{const char*_tmp98B;void*
_tmp98A;(_tmp98A=0,Cyc_fprintf(Cyc_stderr,((_tmp98B="Error: C compiler failed\n",
_tag_dynforward(_tmp98B,sizeof(char),_get_zero_arr_size_char(_tmp98B,26)))),
_tag_dynforward(_tmp98A,sizeof(void*),0)));}Cyc_compile_failure=1;Cyc_remove_cfiles();
return 1;}Cyc_remove_cfiles();return Cyc_compile_failure?1: 0;}}}}}}
