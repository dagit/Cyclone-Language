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
16];struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[
18];extern char Cyc_List_Nth[8];struct Cyc_Lineno_Pos{struct _dynforward_ptr
logical_file;struct _dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _dynforward_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dynforward_ptr desc;};
extern char Cyc_Position_Nocontext[14];struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};int Cyc_strptrcmp(struct _dynforward_ptr*s1,
struct _dynforward_ptr*s2);struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dynforward_ptr
Cyc_Absynpp_typ2string(void*);struct _dynforward_ptr Cyc_Absynpp_exp2string(struct
Cyc_Absyn_Exp*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[
11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern char
Cyc_Dict_Absent[11];struct _tuple3{void*f1;void*f2;};struct _tuple3*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple3*Cyc_Dict_rchoose(
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
le;};void*Cyc_Tcutil_impos(struct _dynforward_ptr fmt,struct _dynforward_ptr ap);
void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t);void*Cyc_Tcutil_compress(
void*t);int Cyc_Tcutil_unify(void*,void*);int Cyc_Tcutil_typecmp(void*,void*);
struct _tuple4{unsigned int f1;int f2;};struct _tuple4 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
int Cyc_Evexp_okay_szofarg(void*t);struct _tuple5{union Cyc_Absyn_Cnst_union f1;int
f2;};static struct _tuple5 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*);static
union Cyc_Absyn_Cnst_union Cyc_Evexp_promote_const(union Cyc_Absyn_Cnst_union cn);
static union Cyc_Absyn_Cnst_union Cyc_Evexp_promote_const(union Cyc_Absyn_Cnst_union
cn){union Cyc_Absyn_Cnst_union _tmp0=cn;void*_tmp1;char _tmp2;void*_tmp3;short _tmp4;
_LL1: if((_tmp0.Char_c).tag != 0)goto _LL3;_tmp1=(_tmp0.Char_c).f1;_tmp2=(_tmp0.Char_c).f2;
_LL2: {union Cyc_Absyn_Cnst_union _tmp16D;return(union Cyc_Absyn_Cnst_union)(((
_tmp16D.Int_c).tag=2,(((_tmp16D.Int_c).f1=(void*)_tmp1,(((_tmp16D.Int_c).f2=(int)
_tmp2,_tmp16D))))));}_LL3: if((_tmp0.Short_c).tag != 1)goto _LL5;_tmp3=(_tmp0.Short_c).f1;
_tmp4=(_tmp0.Short_c).f2;_LL4: {union Cyc_Absyn_Cnst_union _tmp16E;return(union Cyc_Absyn_Cnst_union)(((
_tmp16E.Int_c).tag=2,(((_tmp16E.Int_c).f1=(void*)_tmp3,(((_tmp16E.Int_c).f2=(int)
_tmp4,_tmp16E))))));}_LL5:;_LL6: return cn;_LL0:;}struct _tuple4 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct _tuple4 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*
e){union Cyc_Absyn_Cnst_union _tmp8;int _tmp9;struct _tuple5 _tmp7=Cyc_Evexp_eval_const_exp(
e);_tmp8=_tmp7.f1;_tmp9=_tmp7.f2;if(!_tmp9){struct _tuple4 _tmp16F;return(_tmp16F.f1=
0,((_tmp16F.f2=0,_tmp16F)));}{union Cyc_Absyn_Cnst_union _tmpB=Cyc_Evexp_promote_const(
_tmp8);int _tmpC;_LL8: if((_tmpB.Int_c).tag != 2)goto _LLA;_tmpC=(_tmpB.Int_c).f2;
_LL9: {struct _tuple4 _tmp170;return(_tmp170.f1=(unsigned int)_tmpC,((_tmp170.f2=1,
_tmp170)));}_LLA:;_LLB:{const char*_tmp173;void*_tmp172;(_tmp172=0,Cyc_Tcutil_terr(
e->loc,((_tmp173="expecting unsigned int",_tag_dynforward(_tmp173,sizeof(char),
_get_zero_arr_size_char(_tmp173,23)))),_tag_dynforward(_tmp172,sizeof(void*),0)));}{
struct _tuple4 _tmp174;return(_tmp174.f1=0,((_tmp174.f2=1,_tmp174)));}_LL7:;}}
struct _tuple6{int f1;int f2;};static struct _tuple6 Cyc_Evexp_eval_const_bool_exp(
struct Cyc_Absyn_Exp*e);static struct _tuple6 Cyc_Evexp_eval_const_bool_exp(struct
Cyc_Absyn_Exp*e){union Cyc_Absyn_Cnst_union _tmp12;int _tmp13;struct _tuple5 _tmp11=
Cyc_Evexp_eval_const_exp(e);_tmp12=_tmp11.f1;_tmp13=_tmp11.f2;if(!_tmp13){struct
_tuple6 _tmp175;return(_tmp175.f1=0,((_tmp175.f2=0,_tmp175)));}{union Cyc_Absyn_Cnst_union
_tmp15=Cyc_Evexp_promote_const(_tmp12);int _tmp16;long long _tmp17;_LLD: if((_tmp15.Int_c).tag
!= 2)goto _LLF;_tmp16=(_tmp15.Int_c).f2;_LLE: {struct _tuple6 _tmp176;return(
_tmp176.f1=_tmp16 != 0,((_tmp176.f2=1,_tmp176)));}_LLF: if((_tmp15.LongLong_c).tag
!= 3)goto _LL11;_tmp17=(_tmp15.LongLong_c).f2;_LL10: {struct _tuple6 _tmp177;return(
_tmp177.f1=_tmp17 != 0,((_tmp177.f2=1,_tmp177)));}_LL11: if((_tmp15.Null_c).tag != 
6)goto _LL13;_LL12: {struct _tuple6 _tmp178;return(_tmp178.f1=0,((_tmp178.f2=0,
_tmp178)));}_LL13:;_LL14:{const char*_tmp17B;void*_tmp17A;(_tmp17A=0,Cyc_Tcutil_terr(
e->loc,((_tmp17B="expecting bool",_tag_dynforward(_tmp17B,sizeof(char),
_get_zero_arr_size_char(_tmp17B,15)))),_tag_dynforward(_tmp17A,sizeof(void*),0)));}{
struct _tuple6 _tmp17C;return(_tmp17C.f1=0,((_tmp17C.f2=0,_tmp17C)));}_LLC:;}}
struct _tuple7{void*f1;union Cyc_Absyn_Cnst_union f2;};static struct _tuple5 Cyc_Evexp_eval_const_unprimop(
void*p,struct Cyc_Absyn_Exp*e);static struct _tuple5 Cyc_Evexp_eval_const_unprimop(
void*p,struct Cyc_Absyn_Exp*e){union Cyc_Absyn_Cnst_union _tmp1F;int _tmp20;struct
_tuple5 _tmp1E=Cyc_Evexp_eval_const_exp(e);_tmp1F=_tmp1E.f1;_tmp20=_tmp1E.f2;if(!
_tmp20){struct _tuple5 _tmp17D;return(_tmp17D.f1=_tmp1F,((_tmp17D.f2=0,_tmp17D)));}{
struct _tuple7 _tmp17E;struct _tuple7 _tmp23=(_tmp17E.f1=p,((_tmp17E.f2=_tmp1F,
_tmp17E)));void*_tmp24;void*_tmp25;union Cyc_Absyn_Cnst_union _tmp26;void*_tmp27;
int _tmp28;void*_tmp29;union Cyc_Absyn_Cnst_union _tmp2A;void*_tmp2B;int _tmp2C;void*
_tmp2D;union Cyc_Absyn_Cnst_union _tmp2E;int _tmp2F;void*_tmp30;union Cyc_Absyn_Cnst_union
_tmp31;_LL16: _tmp24=_tmp23.f1;if((int)_tmp24 != 0)goto _LL18;_LL17: goto _LL15;_LL18:
_tmp25=_tmp23.f1;if((int)_tmp25 != 2)goto _LL1A;_tmp26=_tmp23.f2;if(((_tmp23.f2).Int_c).tag
!= 2)goto _LL1A;_tmp27=(_tmp26.Int_c).f1;_tmp28=(_tmp26.Int_c).f2;_LL19:{union Cyc_Absyn_Cnst_union
_tmp17F;_tmp1F=(union Cyc_Absyn_Cnst_union)(((_tmp17F.Int_c).tag=2,(((_tmp17F.Int_c).f1=(
void*)((void*)0),(((_tmp17F.Int_c).f2=- _tmp28,_tmp17F))))));}goto _LL15;_LL1A:
_tmp29=_tmp23.f1;if((int)_tmp29 != 12)goto _LL1C;_tmp2A=_tmp23.f2;if(((_tmp23.f2).Int_c).tag
!= 2)goto _LL1C;_tmp2B=(_tmp2A.Int_c).f1;_tmp2C=(_tmp2A.Int_c).f2;_LL1B:{union Cyc_Absyn_Cnst_union
_tmp180;_tmp1F=(union Cyc_Absyn_Cnst_union)(((_tmp180.Int_c).tag=2,(((_tmp180.Int_c).f1=(
void*)((void*)1),(((_tmp180.Int_c).f2=~ _tmp2C,_tmp180))))));}goto _LL15;_LL1C:
_tmp2D=_tmp23.f1;if((int)_tmp2D != 11)goto _LL1E;_tmp2E=_tmp23.f2;if(((_tmp23.f2).Int_c).tag
!= 2)goto _LL1E;_tmp2F=(_tmp2E.Int_c).f2;_LL1D:{union Cyc_Absyn_Cnst_union _tmp181;
_tmp1F=(union Cyc_Absyn_Cnst_union)(((_tmp181.Int_c).tag=2,(((_tmp181.Int_c).f1=(
void*)((void*)0),(((_tmp181.Int_c).f2=_tmp2F == 0?1: 0,_tmp181))))));}goto _LL15;
_LL1E: _tmp30=_tmp23.f1;if((int)_tmp30 != 11)goto _LL20;_tmp31=_tmp23.f2;if(((
_tmp23.f2).Null_c).tag != 6)goto _LL20;_LL1F:{union Cyc_Absyn_Cnst_union _tmp182;
_tmp1F=(union Cyc_Absyn_Cnst_union)(((_tmp182.Int_c).tag=2,(((_tmp182.Int_c).f1=(
void*)((void*)0),(((_tmp182.Int_c).f2=1,_tmp182))))));}goto _LL15;_LL20:;_LL21:{
const char*_tmp185;void*_tmp184;(_tmp184=0,Cyc_Tcutil_terr(e->loc,((_tmp185="bad constant expression",
_tag_dynforward(_tmp185,sizeof(char),_get_zero_arr_size_char(_tmp185,24)))),
_tag_dynforward(_tmp184,sizeof(void*),0)));}goto _LL15;_LL15:;}{struct _tuple5
_tmp186;return(_tmp186.f1=_tmp1F,((_tmp186.f2=1,_tmp186)));}}struct _tuple8{void*
f1;int f2;};static struct _tuple5 Cyc_Evexp_eval_const_binprimop(void*p,struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);static struct _tuple5 Cyc_Evexp_eval_const_binprimop(
void*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){union Cyc_Absyn_Cnst_union
_tmp3A;int _tmp3B;struct _tuple5 _tmp39=Cyc_Evexp_eval_const_exp(e1);_tmp3A=_tmp39.f1;
_tmp3B=_tmp39.f2;{union Cyc_Absyn_Cnst_union _tmp3D;int _tmp3E;struct _tuple5 _tmp3C=
Cyc_Evexp_eval_const_exp(e2);_tmp3D=_tmp3C.f1;_tmp3E=_tmp3C.f2;if(!_tmp3B  || !
_tmp3E){struct _tuple5 _tmp187;return(_tmp187.f1=_tmp3A,((_tmp187.f2=0,_tmp187)));}
_tmp3A=Cyc_Evexp_promote_const(_tmp3A);_tmp3D=Cyc_Evexp_promote_const(_tmp3D);{
void*s1;void*s2;int i1;int i2;{union Cyc_Absyn_Cnst_union _tmp40=_tmp3A;void*_tmp41;
int _tmp42;_LL23: if((_tmp40.Int_c).tag != 2)goto _LL25;_tmp41=(_tmp40.Int_c).f1;
_tmp42=(_tmp40.Int_c).f2;_LL24: s1=_tmp41;i1=_tmp42;goto _LL22;_LL25:;_LL26:{const
char*_tmp18A;void*_tmp189;(_tmp189=0,Cyc_Tcutil_terr(e1->loc,((_tmp18A="bad constant expression",
_tag_dynforward(_tmp18A,sizeof(char),_get_zero_arr_size_char(_tmp18A,24)))),
_tag_dynforward(_tmp189,sizeof(void*),0)));}{struct _tuple5 _tmp18B;return(_tmp18B.f1=
_tmp3A,((_tmp18B.f2=1,_tmp18B)));}_LL22:;}{union Cyc_Absyn_Cnst_union _tmp46=
_tmp3D;void*_tmp47;int _tmp48;_LL28: if((_tmp46.Int_c).tag != 2)goto _LL2A;_tmp47=(
_tmp46.Int_c).f1;_tmp48=(_tmp46.Int_c).f2;_LL29: s2=_tmp47;i2=_tmp48;goto _LL27;
_LL2A:;_LL2B:{const char*_tmp18E;void*_tmp18D;(_tmp18D=0,Cyc_Tcutil_terr(e2->loc,((
_tmp18E="bad constant expression",_tag_dynforward(_tmp18E,sizeof(char),
_get_zero_arr_size_char(_tmp18E,24)))),_tag_dynforward(_tmp18D,sizeof(void*),0)));}{
struct _tuple5 _tmp18F;return(_tmp18F.f1=_tmp3A,((_tmp18F.f2=1,_tmp18F)));}_LL27:;}{
void*_tmp4C=p;_LL2D: if((int)_tmp4C != 3)goto _LL2F;_LL2E: goto _LL30;_LL2F: if((int)
_tmp4C != 4)goto _LL31;_LL30: if(i2 == 0){{const char*_tmp192;void*_tmp191;(_tmp191=0,
Cyc_Tcutil_terr(e2->loc,((_tmp192="division by zero in constant expression",
_tag_dynforward(_tmp192,sizeof(char),_get_zero_arr_size_char(_tmp192,40)))),
_tag_dynforward(_tmp191,sizeof(void*),0)));}{struct _tuple5 _tmp193;return(_tmp193.f1=
_tmp3A,((_tmp193.f2=1,_tmp193)));}}goto _LL2C;_LL31:;_LL32: goto _LL2C;_LL2C:;}{int
has_u_arg=s1 == (void*)1  || s2 == (void*)1;unsigned int u1=(unsigned int)i1;
unsigned int u2=(unsigned int)i2;int i3=0;unsigned int u3=0;int b3=1;int use_i3=0;int
use_u3=0;int use_b3=0;{struct _tuple8 _tmp194;struct _tuple8 _tmp51=(_tmp194.f1=p,((
_tmp194.f2=has_u_arg,_tmp194)));void*_tmp52;int _tmp53;void*_tmp54;int _tmp55;void*
_tmp56;int _tmp57;void*_tmp58;int _tmp59;void*_tmp5A;int _tmp5B;void*_tmp5C;int
_tmp5D;void*_tmp5E;int _tmp5F;void*_tmp60;int _tmp61;void*_tmp62;int _tmp63;void*
_tmp64;int _tmp65;void*_tmp66;void*_tmp67;void*_tmp68;int _tmp69;void*_tmp6A;int
_tmp6B;void*_tmp6C;int _tmp6D;void*_tmp6E;int _tmp6F;void*_tmp70;int _tmp71;void*
_tmp72;int _tmp73;void*_tmp74;int _tmp75;void*_tmp76;int _tmp77;void*_tmp78;void*
_tmp79;void*_tmp7A;void*_tmp7B;void*_tmp7C;void*_tmp7D;_LL34: _tmp52=_tmp51.f1;
if((int)_tmp52 != 0)goto _LL36;_tmp53=_tmp51.f2;if(_tmp53 != 0)goto _LL36;_LL35: i3=
i1 + i2;use_i3=1;goto _LL33;_LL36: _tmp54=_tmp51.f1;if((int)_tmp54 != 1)goto _LL38;
_tmp55=_tmp51.f2;if(_tmp55 != 0)goto _LL38;_LL37: i3=i1 * i2;use_i3=1;goto _LL33;
_LL38: _tmp56=_tmp51.f1;if((int)_tmp56 != 2)goto _LL3A;_tmp57=_tmp51.f2;if(_tmp57 != 
0)goto _LL3A;_LL39: i3=i1 - i2;use_i3=1;goto _LL33;_LL3A: _tmp58=_tmp51.f1;if((int)
_tmp58 != 3)goto _LL3C;_tmp59=_tmp51.f2;if(_tmp59 != 0)goto _LL3C;_LL3B: i3=i1 / i2;
use_i3=1;goto _LL33;_LL3C: _tmp5A=_tmp51.f1;if((int)_tmp5A != 4)goto _LL3E;_tmp5B=
_tmp51.f2;if(_tmp5B != 0)goto _LL3E;_LL3D: i3=i1 % i2;use_i3=1;goto _LL33;_LL3E:
_tmp5C=_tmp51.f1;if((int)_tmp5C != 0)goto _LL40;_tmp5D=_tmp51.f2;if(_tmp5D != 1)
goto _LL40;_LL3F: u3=u1 + u2;use_u3=1;goto _LL33;_LL40: _tmp5E=_tmp51.f1;if((int)
_tmp5E != 1)goto _LL42;_tmp5F=_tmp51.f2;if(_tmp5F != 1)goto _LL42;_LL41: u3=u1 * u2;
use_u3=1;goto _LL33;_LL42: _tmp60=_tmp51.f1;if((int)_tmp60 != 2)goto _LL44;_tmp61=
_tmp51.f2;if(_tmp61 != 1)goto _LL44;_LL43: u3=u1 - u2;use_u3=1;goto _LL33;_LL44:
_tmp62=_tmp51.f1;if((int)_tmp62 != 3)goto _LL46;_tmp63=_tmp51.f2;if(_tmp63 != 1)
goto _LL46;_LL45: u3=u1 / u2;use_u3=1;goto _LL33;_LL46: _tmp64=_tmp51.f1;if((int)
_tmp64 != 4)goto _LL48;_tmp65=_tmp51.f2;if(_tmp65 != 1)goto _LL48;_LL47: u3=u1 % u2;
use_u3=1;goto _LL33;_LL48: _tmp66=_tmp51.f1;if((int)_tmp66 != 5)goto _LL4A;_LL49: b3=
i1 == i2;use_b3=1;goto _LL33;_LL4A: _tmp67=_tmp51.f1;if((int)_tmp67 != 6)goto _LL4C;
_LL4B: b3=i1 != i2;use_b3=1;goto _LL33;_LL4C: _tmp68=_tmp51.f1;if((int)_tmp68 != 7)
goto _LL4E;_tmp69=_tmp51.f2;if(_tmp69 != 0)goto _LL4E;_LL4D: b3=i1 > i2;use_b3=1;goto
_LL33;_LL4E: _tmp6A=_tmp51.f1;if((int)_tmp6A != 8)goto _LL50;_tmp6B=_tmp51.f2;if(
_tmp6B != 0)goto _LL50;_LL4F: b3=i1 < i2;use_b3=1;goto _LL33;_LL50: _tmp6C=_tmp51.f1;
if((int)_tmp6C != 9)goto _LL52;_tmp6D=_tmp51.f2;if(_tmp6D != 0)goto _LL52;_LL51: b3=
i1 >= i2;use_b3=1;goto _LL33;_LL52: _tmp6E=_tmp51.f1;if((int)_tmp6E != 10)goto _LL54;
_tmp6F=_tmp51.f2;if(_tmp6F != 0)goto _LL54;_LL53: b3=i1 <= i2;use_b3=1;goto _LL33;
_LL54: _tmp70=_tmp51.f1;if((int)_tmp70 != 7)goto _LL56;_tmp71=_tmp51.f2;if(_tmp71 != 
1)goto _LL56;_LL55: b3=u1 > u2;use_b3=1;goto _LL33;_LL56: _tmp72=_tmp51.f1;if((int)
_tmp72 != 8)goto _LL58;_tmp73=_tmp51.f2;if(_tmp73 != 1)goto _LL58;_LL57: b3=u1 < u2;
use_b3=1;goto _LL33;_LL58: _tmp74=_tmp51.f1;if((int)_tmp74 != 9)goto _LL5A;_tmp75=
_tmp51.f2;if(_tmp75 != 1)goto _LL5A;_LL59: b3=u1 >= u2;use_b3=1;goto _LL33;_LL5A:
_tmp76=_tmp51.f1;if((int)_tmp76 != 10)goto _LL5C;_tmp77=_tmp51.f2;if(_tmp77 != 1)
goto _LL5C;_LL5B: b3=u1 <= u2;use_b3=1;goto _LL33;_LL5C: _tmp78=_tmp51.f1;if((int)
_tmp78 != 13)goto _LL5E;_LL5D: u3=u1 & u2;use_u3=1;goto _LL33;_LL5E: _tmp79=_tmp51.f1;
if((int)_tmp79 != 14)goto _LL60;_LL5F: u3=u1 | u2;use_u3=1;goto _LL33;_LL60: _tmp7A=
_tmp51.f1;if((int)_tmp7A != 15)goto _LL62;_LL61: u3=u1 ^ u2;use_u3=1;goto _LL33;_LL62:
_tmp7B=_tmp51.f1;if((int)_tmp7B != 16)goto _LL64;_LL63: u3=u1 << u2;use_u3=1;goto
_LL33;_LL64: _tmp7C=_tmp51.f1;if((int)_tmp7C != 17)goto _LL66;_LL65: u3=u1 >> u2;
use_u3=1;goto _LL33;_LL66: _tmp7D=_tmp51.f1;if((int)_tmp7D != 18)goto _LL68;_LL67:{
const char*_tmp197;void*_tmp196;(_tmp196=0,Cyc_Tcutil_terr(e1->loc,((_tmp197=">>> NOT IMPLEMENTED",
_tag_dynforward(_tmp197,sizeof(char),_get_zero_arr_size_char(_tmp197,20)))),
_tag_dynforward(_tmp196,sizeof(void*),0)));}goto _LL69;_LL68:;_LL69:{const char*
_tmp19A;void*_tmp199;(_tmp199=0,Cyc_Tcutil_terr(e1->loc,((_tmp19A="bad constant expression",
_tag_dynforward(_tmp19A,sizeof(char),_get_zero_arr_size_char(_tmp19A,24)))),
_tag_dynforward(_tmp199,sizeof(void*),0)));}{struct _tuple5 _tmp19B;return(_tmp19B.f1=
_tmp3A,((_tmp19B.f2=1,_tmp19B)));}_LL33:;}if(use_i3){union Cyc_Absyn_Cnst_union
_tmp19E;struct _tuple5 _tmp19D;return(_tmp19D.f1=(union Cyc_Absyn_Cnst_union)(((
_tmp19E.Int_c).tag=2,(((_tmp19E.Int_c).f1=(void*)((void*)0),(((_tmp19E.Int_c).f2=
i3,_tmp19E)))))),((_tmp19D.f2=1,_tmp19D)));}if(use_u3){union Cyc_Absyn_Cnst_union
_tmp1A1;struct _tuple5 _tmp1A0;return(_tmp1A0.f1=(union Cyc_Absyn_Cnst_union)(((
_tmp1A1.Int_c).tag=2,(((_tmp1A1.Int_c).f1=(void*)((void*)1),(((_tmp1A1.Int_c).f2=(
int)u3,_tmp1A1)))))),((_tmp1A0.f2=1,_tmp1A0)));}if(use_b3){union Cyc_Absyn_Cnst_union
_tmp1A4;struct _tuple5 _tmp1A3;return(_tmp1A3.f1=(union Cyc_Absyn_Cnst_union)(((
_tmp1A4.Int_c).tag=2,(((_tmp1A4.Int_c).f1=(void*)((void*)0),(((_tmp1A4.Int_c).f2=
b3?1: 0,_tmp1A4)))))),((_tmp1A3.f2=1,_tmp1A3)));}{const char*_tmp1A7;void*_tmp1A6;(
_tmp1A6=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmp1A7="Evexp::eval_const_binop",_tag_dynforward(_tmp1A7,sizeof(char),
_get_zero_arr_size_char(_tmp1A7,24)))),_tag_dynforward(_tmp1A6,sizeof(void*),0)));}}}}}
static struct _tuple5 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*e);static struct
_tuple5 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*e){struct _tuple5 ans;{void*
_tmp8B=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp8C;struct Cyc_Absyn_Exp*_tmp8D;
struct Cyc_Absyn_Exp*_tmp8E;struct Cyc_Absyn_Exp*_tmp8F;struct Cyc_Absyn_Exp*_tmp90;
struct Cyc_Absyn_Exp*_tmp91;struct Cyc_Absyn_Exp*_tmp92;struct Cyc_Absyn_Exp*_tmp93;
void*_tmp94;struct Cyc_List_List*_tmp95;void*_tmp96;struct Cyc_Absyn_Exp*_tmp97;
struct Cyc_Absyn_Enumfield*_tmp98;struct Cyc_Absyn_Enumfield*_tmp99;_LL6B: if(*((
int*)_tmp8B)!= 0)goto _LL6D;_tmp8C=((struct Cyc_Absyn_Const_e_struct*)_tmp8B)->f1;
_LL6C: {struct _tuple5 _tmp1A8;return(_tmp1A8.f1=_tmp8C,((_tmp1A8.f2=1,_tmp1A8)));}
_LL6D: if(*((int*)_tmp8B)!= 6)goto _LL6F;_tmp8D=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp8B)->f1;_tmp8E=((struct Cyc_Absyn_Conditional_e_struct*)_tmp8B)->f2;_tmp8F=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp8B)->f3;_LL6E: {int _tmp9C;int _tmp9D;
struct _tuple6 _tmp9B=Cyc_Evexp_eval_const_bool_exp(_tmp8D);_tmp9C=_tmp9B.f1;
_tmp9D=_tmp9B.f2;if(!_tmp9D){Cyc_Evexp_eval_const_exp(_tmp8E);Cyc_Evexp_eval_const_exp(
_tmp8F);{union Cyc_Absyn_Cnst_union _tmp1AB;struct _tuple5 _tmp1AA;return(_tmp1AA.f1=(
union Cyc_Absyn_Cnst_union)(((_tmp1AB.Int_c).tag=2,(((_tmp1AB.Int_c).f1=(void*)((
void*)0),(((_tmp1AB.Int_c).f2=0,_tmp1AB)))))),((_tmp1AA.f2=0,_tmp1AA)));}}ans=
_tmp9C?Cyc_Evexp_eval_const_exp(_tmp8E): Cyc_Evexp_eval_const_exp(_tmp8F);goto
_LL6A;}_LL6F: if(*((int*)_tmp8B)!= 7)goto _LL71;_tmp90=((struct Cyc_Absyn_And_e_struct*)
_tmp8B)->f1;_tmp91=((struct Cyc_Absyn_And_e_struct*)_tmp8B)->f2;_LL70: {int _tmpA1;
int _tmpA2;struct _tuple6 _tmpA0=Cyc_Evexp_eval_const_bool_exp(_tmp90);_tmpA1=
_tmpA0.f1;_tmpA2=_tmpA0.f2;if(!_tmpA2){Cyc_Evexp_eval_const_exp(_tmp91);{union
Cyc_Absyn_Cnst_union _tmp1AE;struct _tuple5 _tmp1AD;return(_tmp1AD.f1=(union Cyc_Absyn_Cnst_union)(((
_tmp1AE.Int_c).tag=2,(((_tmp1AE.Int_c).f1=(void*)((void*)0),(((_tmp1AE.Int_c).f2=
0,_tmp1AE)))))),((_tmp1AD.f2=0,_tmp1AD)));}}if(_tmpA1)ans=Cyc_Evexp_eval_const_exp(
_tmp91);else{union Cyc_Absyn_Cnst_union _tmp1B1;struct _tuple5 _tmp1B0;ans=((_tmp1B0.f1=(
union Cyc_Absyn_Cnst_union)(((_tmp1B1.Int_c).tag=2,(((_tmp1B1.Int_c).f1=(void*)((
void*)0),(((_tmp1B1.Int_c).f2=0,_tmp1B1)))))),((_tmp1B0.f2=1,_tmp1B0))));}goto
_LL6A;}_LL71: if(*((int*)_tmp8B)!= 8)goto _LL73;_tmp92=((struct Cyc_Absyn_Or_e_struct*)
_tmp8B)->f1;_tmp93=((struct Cyc_Absyn_Or_e_struct*)_tmp8B)->f2;_LL72: {int _tmpA8;
int _tmpA9;struct _tuple6 _tmpA7=Cyc_Evexp_eval_const_bool_exp(_tmp92);_tmpA8=
_tmpA7.f1;_tmpA9=_tmpA7.f2;if(!_tmpA9){Cyc_Evexp_eval_const_exp(_tmp93);{union
Cyc_Absyn_Cnst_union _tmp1B4;struct _tuple5 _tmp1B3;return(_tmp1B3.f1=(union Cyc_Absyn_Cnst_union)(((
_tmp1B4.Int_c).tag=2,(((_tmp1B4.Int_c).f1=(void*)((void*)0),(((_tmp1B4.Int_c).f2=
0,_tmp1B4)))))),((_tmp1B3.f2=0,_tmp1B3)));}}if(!_tmpA8)ans=Cyc_Evexp_eval_const_exp(
_tmp93);else{union Cyc_Absyn_Cnst_union _tmp1B7;struct _tuple5 _tmp1B6;ans=((_tmp1B6.f1=(
union Cyc_Absyn_Cnst_union)(((_tmp1B7.Int_c).tag=2,(((_tmp1B7.Int_c).f1=(void*)((
void*)0),(((_tmp1B7.Int_c).f2=1,_tmp1B7)))))),((_tmp1B6.f2=1,_tmp1B6))));}goto
_LL6A;}_LL73: if(*((int*)_tmp8B)!= 3)goto _LL75;_tmp94=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp8B)->f1;_tmp95=((struct Cyc_Absyn_Primop_e_struct*)_tmp8B)->f2;_LL74: if(
_tmp95 == 0){{const char*_tmp1BA;void*_tmp1B9;(_tmp1B9=0,Cyc_Tcutil_terr(e->loc,((
_tmp1BA="bad static expression (no args to primop)",_tag_dynforward(_tmp1BA,
sizeof(char),_get_zero_arr_size_char(_tmp1BA,42)))),_tag_dynforward(_tmp1B9,
sizeof(void*),0)));}{union Cyc_Absyn_Cnst_union _tmp1BD;struct _tuple5 _tmp1BC;
return(_tmp1BC.f1=(union Cyc_Absyn_Cnst_union)(((_tmp1BD.Int_c).tag=2,(((_tmp1BD.Int_c).f1=(
void*)((void*)0),(((_tmp1BD.Int_c).f2=0,_tmp1BD)))))),((_tmp1BC.f2=1,_tmp1BC)));}}
if(_tmp95->tl == 0){ans=Cyc_Evexp_eval_const_unprimop(_tmp94,(struct Cyc_Absyn_Exp*)
_tmp95->hd);goto _LL6A;}if(((struct Cyc_List_List*)_check_null(_tmp95->tl))->tl == 
0){ans=Cyc_Evexp_eval_const_binprimop(_tmp94,(struct Cyc_Absyn_Exp*)_tmp95->hd,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp95->tl))->hd);goto
_LL6A;}{const char*_tmp1C0;void*_tmp1BF;(_tmp1BF=0,Cyc_Tcutil_terr(e->loc,((
_tmp1C0="bad static expression (too many args to primop)",_tag_dynforward(
_tmp1C0,sizeof(char),_get_zero_arr_size_char(_tmp1C0,48)))),_tag_dynforward(
_tmp1BF,sizeof(void*),0)));}{union Cyc_Absyn_Cnst_union _tmp1C3;struct _tuple5
_tmp1C2;return(_tmp1C2.f1=(union Cyc_Absyn_Cnst_union)(((_tmp1C3.Int_c).tag=2,(((
_tmp1C3.Int_c).f1=(void*)((void*)0),(((_tmp1C3.Int_c).f2=0,_tmp1C3)))))),((
_tmp1C2.f2=1,_tmp1C2)));}_LL75: if(*((int*)_tmp8B)!= 18)goto _LL77;_LL76: goto _LL78;
_LL77: if(*((int*)_tmp8B)!= 19)goto _LL79;_LL78: goto _LL7A;_LL79: if(*((int*)_tmp8B)
!= 20)goto _LL7B;_LL7A:{union Cyc_Absyn_Cnst_union _tmp1C6;struct _tuple5 _tmp1C5;ans=((
_tmp1C5.f1=(union Cyc_Absyn_Cnst_union)(((_tmp1C6.Int_c).tag=2,(((_tmp1C6.Int_c).f1=(
void*)((void*)0),(((_tmp1C6.Int_c).f2=0,_tmp1C6)))))),((_tmp1C5.f2=0,_tmp1C5))));}
goto _LL6A;_LL7B: if(*((int*)_tmp8B)!= 15)goto _LL7D;_tmp96=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp8B)->f1;_tmp97=((struct Cyc_Absyn_Cast_e_struct*)_tmp8B)->f2;_LL7C: ans=Cyc_Evexp_eval_const_exp(
_tmp97);if(ans.f2){struct _tuple7 _tmp1C7;struct _tuple7 _tmpB9=(_tmp1C7.f1=Cyc_Tcutil_compress(
_tmp96),((_tmp1C7.f2=ans.f1,_tmp1C7)));void*_tmpBA;void*_tmpBB;void*_tmpBC;union
Cyc_Absyn_Cnst_union _tmpBD;void*_tmpBE;char _tmpBF;void*_tmpC0;void*_tmpC1;void*
_tmpC2;union Cyc_Absyn_Cnst_union _tmpC3;void*_tmpC4;short _tmpC5;void*_tmpC6;void*
_tmpC7;void*_tmpC8;union Cyc_Absyn_Cnst_union _tmpC9;void*_tmpCA;int _tmpCB;void*
_tmpCC;void*_tmpCD;union Cyc_Absyn_Cnst_union _tmpCE;void*_tmpCF;char _tmpD0;void*
_tmpD1;void*_tmpD2;union Cyc_Absyn_Cnst_union _tmpD3;void*_tmpD4;short _tmpD5;void*
_tmpD6;void*_tmpD7;union Cyc_Absyn_Cnst_union _tmpD8;void*_tmpD9;int _tmpDA;_LL84:
_tmpBA=_tmpB9.f1;if(_tmpBA <= (void*)4)goto _LL86;if(*((int*)_tmpBA)!= 5)goto _LL86;
_tmpBB=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpBA)->f1;_tmpBC=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmpBA)->f2;_tmpBD=_tmpB9.f2;if(((_tmpB9.f2).Char_c).tag
!= 0)goto _LL86;_tmpBE=(_tmpBD.Char_c).f1;_tmpBF=(_tmpBD.Char_c).f2;_LL85: _tmpC1=
_tmpBB;_tmpC2=_tmpBC;_tmpC4=_tmpBE;_tmpC5=(short)_tmpBF;goto _LL87;_LL86: _tmpC0=
_tmpB9.f1;if(_tmpC0 <= (void*)4)goto _LL88;if(*((int*)_tmpC0)!= 5)goto _LL88;_tmpC1=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmpC0)->f1;_tmpC2=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpC0)->f2;_tmpC3=_tmpB9.f2;if(((_tmpB9.f2).Short_c).tag != 1)goto _LL88;_tmpC4=(
_tmpC3.Short_c).f1;_tmpC5=(_tmpC3.Short_c).f2;_LL87: _tmpC7=_tmpC1;_tmpC8=_tmpC2;
_tmpCA=_tmpC4;_tmpCB=(int)_tmpC5;goto _LL89;_LL88: _tmpC6=_tmpB9.f1;if(_tmpC6 <= (
void*)4)goto _LL8A;if(*((int*)_tmpC6)!= 5)goto _LL8A;_tmpC7=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpC6)->f1;_tmpC8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpC6)->f2;_tmpC9=
_tmpB9.f2;if(((_tmpB9.f2).Int_c).tag != 2)goto _LL8A;_tmpCA=(_tmpC9.Int_c).f1;
_tmpCB=(_tmpC9.Int_c).f2;_LL89: if(_tmpC7 != _tmpCA){union Cyc_Absyn_Cnst_union
_tmp1C8;ans.f1=(union Cyc_Absyn_Cnst_union)(((_tmp1C8.Int_c).tag=2,(((_tmp1C8.Int_c).f1=(
void*)_tmpC7,(((_tmp1C8.Int_c).f2=_tmpCB,_tmp1C8))))));}goto _LL83;_LL8A: _tmpCC=
_tmpB9.f1;if(_tmpCC <= (void*)4)goto _LL8C;if(*((int*)_tmpCC)!= 18)goto _LL8C;
_tmpCD=(void*)((struct Cyc_Absyn_TagType_struct*)_tmpCC)->f1;_tmpCE=_tmpB9.f2;if(((
_tmpB9.f2).Char_c).tag != 0)goto _LL8C;_tmpCF=(_tmpCE.Char_c).f1;_tmpD0=(_tmpCE.Char_c).f2;
_LL8B: _tmpD2=_tmpCD;_tmpD4=_tmpCF;_tmpD5=(short)_tmpD0;goto _LL8D;_LL8C: _tmpD1=
_tmpB9.f1;if(_tmpD1 <= (void*)4)goto _LL8E;if(*((int*)_tmpD1)!= 18)goto _LL8E;
_tmpD2=(void*)((struct Cyc_Absyn_TagType_struct*)_tmpD1)->f1;_tmpD3=_tmpB9.f2;if(((
_tmpB9.f2).Short_c).tag != 1)goto _LL8E;_tmpD4=(_tmpD3.Short_c).f1;_tmpD5=(_tmpD3.Short_c).f2;
_LL8D: _tmpD7=_tmpD2;_tmpD9=_tmpD4;_tmpDA=(int)_tmpD5;goto _LL8F;_LL8E: _tmpD6=
_tmpB9.f1;if(_tmpD6 <= (void*)4)goto _LL90;if(*((int*)_tmpD6)!= 18)goto _LL90;
_tmpD7=(void*)((struct Cyc_Absyn_TagType_struct*)_tmpD6)->f1;_tmpD8=_tmpB9.f2;if(((
_tmpB9.f2).Int_c).tag != 2)goto _LL90;_tmpD9=(_tmpD8.Int_c).f1;_tmpDA=(_tmpD8.Int_c).f2;
_LL8F: if(_tmpDA < 0){const char*_tmp1CB;void*_tmp1CA;(_tmp1CA=0,Cyc_Tcutil_terr(e->loc,((
_tmp1CB="cannot cast negative number to a tag type",_tag_dynforward(_tmp1CB,
sizeof(char),_get_zero_arr_size_char(_tmp1CB,42)))),_tag_dynforward(_tmp1CA,
sizeof(void*),0)));}{struct Cyc_Absyn_TypeInt_struct _tmp1CE;struct Cyc_Absyn_TypeInt_struct*
_tmp1CD;Cyc_Tcutil_unify(_tmpD7,(void*)((_tmp1CD=_cycalloc_atomic(sizeof(*
_tmp1CD)),((_tmp1CD[0]=((_tmp1CE.tag=19,((_tmp1CE.f1=_tmpDA,_tmp1CE)))),_tmp1CD)))));}{
union Cyc_Absyn_Cnst_union _tmp1CF;ans.f1=(union Cyc_Absyn_Cnst_union)(((_tmp1CF.Int_c).tag=
2,(((_tmp1CF.Int_c).f1=(void*)((void*)1),(((_tmp1CF.Int_c).f2=_tmpDA,_tmp1CF))))));}
goto _LL83;_LL90:;_LL91:{const char*_tmp1D3;void*_tmp1D2[1];struct Cyc_String_pa_struct
_tmp1D1;(_tmp1D1.tag=0,((_tmp1D1.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_typ2string(_tmp96)),((_tmp1D2[0]=& _tmp1D1,Cyc_Tcutil_terr(
e->loc,((_tmp1D3="cannot cast to %s",_tag_dynforward(_tmp1D3,sizeof(char),
_get_zero_arr_size_char(_tmp1D3,18)))),_tag_dynforward(_tmp1D2,sizeof(void*),1)))))));}
goto _LL83;_LL83:;}goto _LL6A;_LL7D: if(*((int*)_tmp8B)!= 34)goto _LL7F;_tmp98=((
struct Cyc_Absyn_AnonEnum_e_struct*)_tmp8B)->f3;_LL7E: _tmp99=_tmp98;goto _LL80;
_LL7F: if(*((int*)_tmp8B)!= 33)goto _LL81;_tmp99=((struct Cyc_Absyn_Enum_e_struct*)
_tmp8B)->f3;_LL80: ans=Cyc_Evexp_eval_const_exp((struct Cyc_Absyn_Exp*)_check_null(((
struct Cyc_Absyn_Enumfield*)_check_null(_tmp99))->tag));goto _LL6A;_LL81:;_LL82:{
const char*_tmp1D7;void*_tmp1D6[1];struct Cyc_String_pa_struct _tmp1D5;(_tmp1D5.tag=
0,((_tmp1D5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e)),((_tmp1D6[0]=& _tmp1D5,Cyc_Tcutil_terr(e->loc,((_tmp1D7="bad static expression %s",
_tag_dynforward(_tmp1D7,sizeof(char),_get_zero_arr_size_char(_tmp1D7,25)))),
_tag_dynforward(_tmp1D6,sizeof(void*),1)))))));}{union Cyc_Absyn_Cnst_union
_tmp1DA;struct _tuple5 _tmp1D9;return(_tmp1D9.f1=(union Cyc_Absyn_Cnst_union)(((
_tmp1DA.Int_c).tag=2,(((_tmp1DA.Int_c).f1=(void*)((void*)0),(((_tmp1DA.Int_c).f2=
0,_tmp1DA)))))),((_tmp1D9.f2=1,_tmp1D9)));}_LL6A:;}if(ans.f2){struct Cyc_Absyn_Const_e_struct
_tmp1DD;struct Cyc_Absyn_Const_e_struct*_tmp1DC;(void*)(e->r=(void*)((void*)((
_tmp1DC=_cycalloc(sizeof(*_tmp1DC)),((_tmp1DC[0]=((_tmp1DD.tag=0,((_tmp1DD.f1=
ans.f1,_tmp1DD)))),_tmp1DC))))));}return ans;}static int Cyc_Evexp_const_exp_case_number(
struct Cyc_Absyn_Exp*e);static int Cyc_Evexp_const_exp_case_number(struct Cyc_Absyn_Exp*
e){void*_tmpEB=(void*)e->r;_LL93: if(*((int*)_tmpEB)!= 0)goto _LL95;_LL94: return 1;
_LL95: if(*((int*)_tmpEB)!= 6)goto _LL97;_LL96: return 2;_LL97: if(*((int*)_tmpEB)!= 
3)goto _LL99;_LL98: return 3;_LL99: if(*((int*)_tmpEB)!= 18)goto _LL9B;_LL9A: goto
_LL9C;_LL9B: if(*((int*)_tmpEB)!= 19)goto _LL9D;_LL9C: return 4;_LL9D: if(*((int*)
_tmpEB)!= 20)goto _LL9F;_LL9E: return 5;_LL9F: if(*((int*)_tmpEB)!= 15)goto _LLA1;
_LLA0: return 6;_LLA1: if(*((int*)_tmpEB)!= 7)goto _LLA3;_LLA2: return 7;_LLA3: if(*((
int*)_tmpEB)!= 8)goto _LLA5;_LLA4: return 8;_LLA5:;_LLA6:{const char*_tmp1E1;void*
_tmp1E0[1];struct Cyc_String_pa_struct _tmp1DF;(_tmp1DF.tag=0,((_tmp1DF.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e)),((_tmp1E0[0]=&
_tmp1DF,Cyc_Tcutil_terr(e->loc,((_tmp1E1="bad static expression %s",
_tag_dynforward(_tmp1E1,sizeof(char),_get_zero_arr_size_char(_tmp1E1,25)))),
_tag_dynforward(_tmp1E0,sizeof(void*),1)))))));}return 0;_LL92:;}int Cyc_Evexp_const_exp_cmp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_const_exp_cmp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){unsigned int _tmpF0;int _tmpF1;struct
_tuple4 _tmpEF=Cyc_Evexp_eval_const_uint_exp(e1);_tmpF0=_tmpEF.f1;_tmpF1=_tmpEF.f2;{
unsigned int _tmpF3;int _tmpF4;struct _tuple4 _tmpF2=Cyc_Evexp_eval_const_uint_exp(
e2);_tmpF3=_tmpF2.f1;_tmpF4=_tmpF2.f2;if(_tmpF1  && _tmpF4)return(int)(_tmpF0 - 
_tmpF3);{int _tmpF5=Cyc_Evexp_const_exp_case_number(e1);int _tmpF6=Cyc_Evexp_const_exp_case_number(
e2);if(_tmpF5 != _tmpF6)return _tmpF5 - _tmpF6;{struct _tuple3 _tmp1E2;struct _tuple3
_tmpF8=(_tmp1E2.f1=(void*)e1->r,((_tmp1E2.f2=(void*)e2->r,_tmp1E2)));void*_tmpF9;
struct Cyc_Absyn_Exp*_tmpFA;struct Cyc_Absyn_Exp*_tmpFB;struct Cyc_Absyn_Exp*_tmpFC;
void*_tmpFD;struct Cyc_Absyn_Exp*_tmpFE;struct Cyc_Absyn_Exp*_tmpFF;struct Cyc_Absyn_Exp*
_tmp100;void*_tmp101;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_Absyn_Exp*_tmp103;
void*_tmp104;struct Cyc_Absyn_Exp*_tmp105;struct Cyc_Absyn_Exp*_tmp106;void*
_tmp107;struct Cyc_Absyn_Exp*_tmp108;struct Cyc_Absyn_Exp*_tmp109;void*_tmp10A;
struct Cyc_Absyn_Exp*_tmp10B;struct Cyc_Absyn_Exp*_tmp10C;void*_tmp10D;void*
_tmp10E;struct Cyc_List_List*_tmp10F;void*_tmp110;void*_tmp111;struct Cyc_List_List*
_tmp112;void*_tmp113;void*_tmp114;void*_tmp115;void*_tmp116;void*_tmp117;void*
_tmp118;void*_tmp119;struct Cyc_Absyn_Exp*_tmp11A;void*_tmp11B;struct Cyc_Absyn_Exp*
_tmp11C;void*_tmp11D;void*_tmp11E;void*_tmp11F;struct Cyc_Absyn_Exp*_tmp120;void*
_tmp121;struct Cyc_Absyn_Exp*_tmp122;void*_tmp123;void*_tmp124;void*_tmp125;void*
_tmp126;void*_tmp127;void*_tmp128;void*_tmp129;void*_tmp12A;struct Cyc_Absyn_Exp*
_tmp12B;void*_tmp12C;void*_tmp12D;struct Cyc_Absyn_Exp*_tmp12E;_LLA8: _tmpF9=
_tmpF8.f1;if(*((int*)_tmpF9)!= 6)goto _LLAA;_tmpFA=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpF9)->f1;_tmpFB=((struct Cyc_Absyn_Conditional_e_struct*)_tmpF9)->f2;_tmpFC=((
struct Cyc_Absyn_Conditional_e_struct*)_tmpF9)->f3;_tmpFD=_tmpF8.f2;if(*((int*)
_tmpFD)!= 6)goto _LLAA;_tmpFE=((struct Cyc_Absyn_Conditional_e_struct*)_tmpFD)->f1;
_tmpFF=((struct Cyc_Absyn_Conditional_e_struct*)_tmpFD)->f2;_tmp100=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpFD)->f3;_LLA9: {int _tmp12F=Cyc_Evexp_const_exp_cmp(_tmpFC,_tmp100);if(
_tmp12F != 0)return _tmp12F;_tmp102=_tmpFA;_tmp103=_tmpFB;_tmp105=_tmpFE;_tmp106=
_tmpFF;goto _LLAB;}_LLAA: _tmp101=_tmpF8.f1;if(*((int*)_tmp101)!= 7)goto _LLAC;
_tmp102=((struct Cyc_Absyn_And_e_struct*)_tmp101)->f1;_tmp103=((struct Cyc_Absyn_And_e_struct*)
_tmp101)->f2;_tmp104=_tmpF8.f2;if(*((int*)_tmp104)!= 7)goto _LLAC;_tmp105=((
struct Cyc_Absyn_And_e_struct*)_tmp104)->f1;_tmp106=((struct Cyc_Absyn_And_e_struct*)
_tmp104)->f2;_LLAB: _tmp108=_tmp102;_tmp109=_tmp103;_tmp10B=_tmp105;_tmp10C=
_tmp106;goto _LLAD;_LLAC: _tmp107=_tmpF8.f1;if(*((int*)_tmp107)!= 8)goto _LLAE;
_tmp108=((struct Cyc_Absyn_Or_e_struct*)_tmp107)->f1;_tmp109=((struct Cyc_Absyn_Or_e_struct*)
_tmp107)->f2;_tmp10A=_tmpF8.f2;if(*((int*)_tmp10A)!= 8)goto _LLAE;_tmp10B=((
struct Cyc_Absyn_Or_e_struct*)_tmp10A)->f1;_tmp10C=((struct Cyc_Absyn_Or_e_struct*)
_tmp10A)->f2;_LLAD: {int _tmp130=Cyc_Evexp_const_exp_cmp(_tmp108,_tmp10B);if(
_tmp130 != 0)return _tmp130;return Cyc_Evexp_const_exp_cmp(_tmp109,_tmp10C);}_LLAE:
_tmp10D=_tmpF8.f1;if(*((int*)_tmp10D)!= 3)goto _LLB0;_tmp10E=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp10D)->f1;_tmp10F=((struct Cyc_Absyn_Primop_e_struct*)_tmp10D)->f2;_tmp110=
_tmpF8.f2;if(*((int*)_tmp110)!= 3)goto _LLB0;_tmp111=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp110)->f1;_tmp112=((struct Cyc_Absyn_Primop_e_struct*)_tmp110)->f2;_LLAF: {int
_tmp131=(int)_tmp10E - (int)_tmp111;if(_tmp131 != 0)return _tmp131;for(0;_tmp10F != 
0  && _tmp112 != 0;(_tmp10F=_tmp10F->tl,_tmp112=_tmp112->tl)){int _tmp132=Cyc_Evexp_const_exp_cmp((
struct Cyc_Absyn_Exp*)_tmp10F->hd,(struct Cyc_Absyn_Exp*)_tmp112->hd);if(_tmp132 != 
0)return _tmp132;}return 0;}_LLB0: _tmp113=_tmpF8.f1;if(*((int*)_tmp113)!= 18)goto
_LLB2;_tmp114=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp113)->f1;_tmp115=
_tmpF8.f2;if(*((int*)_tmp115)!= 18)goto _LLB2;_tmp116=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp115)->f1;_LLB1: return Cyc_Tcutil_typecmp(_tmp114,_tmp116);_LLB2: _tmp117=
_tmpF8.f1;if(*((int*)_tmp117)!= 18)goto _LLB4;_tmp118=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp117)->f1;_tmp119=_tmpF8.f2;if(*((int*)_tmp119)!= 19)goto _LLB4;_tmp11A=((
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp119)->f1;_LLB3: if(_tmp11A->topt == 0){
const char*_tmp1E5;void*_tmp1E4;(_tmp1E4=0,Cyc_Tcutil_terr(e2->loc,((_tmp1E5="cannot handle sizeof(exp) here -- use sizeof(type)",
_tag_dynforward(_tmp1E5,sizeof(char),_get_zero_arr_size_char(_tmp1E5,51)))),
_tag_dynforward(_tmp1E4,sizeof(void*),0)));}return Cyc_Tcutil_typecmp(_tmp118,(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp11A->topt))->v);_LLB4: _tmp11B=_tmpF8.f1;
if(*((int*)_tmp11B)!= 19)goto _LLB6;_tmp11C=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp11B)->f1;_tmp11D=_tmpF8.f2;if(*((int*)_tmp11D)!= 18)goto _LLB6;_tmp11E=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp11D)->f1;_LLB5: if(_tmp11C->topt == 0){
const char*_tmp1E8;void*_tmp1E7;(_tmp1E7=0,Cyc_Tcutil_terr(e1->loc,((_tmp1E8="cannot handle sizeof(exp) here -- use sizeof(type)",
_tag_dynforward(_tmp1E8,sizeof(char),_get_zero_arr_size_char(_tmp1E8,51)))),
_tag_dynforward(_tmp1E7,sizeof(void*),0)));}return Cyc_Tcutil_typecmp((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp11C->topt))->v,_tmp11E);_LLB6: _tmp11F=_tmpF8.f1;
if(*((int*)_tmp11F)!= 19)goto _LLB8;_tmp120=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp11F)->f1;_tmp121=_tmpF8.f2;if(*((int*)_tmp121)!= 19)goto _LLB8;_tmp122=((
struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp121)->f1;_LLB7: if(_tmp120->topt == 0){
const char*_tmp1EB;void*_tmp1EA;(_tmp1EA=0,Cyc_Tcutil_terr(e1->loc,((_tmp1EB="cannot handle sizeof(exp) here -- use sizeof(type)",
_tag_dynforward(_tmp1EB,sizeof(char),_get_zero_arr_size_char(_tmp1EB,51)))),
_tag_dynforward(_tmp1EA,sizeof(void*),0)));}if(_tmp122->topt == 0){const char*
_tmp1EE;void*_tmp1ED;(_tmp1ED=0,Cyc_Tcutil_terr(e2->loc,((_tmp1EE="cannot handle sizeof(exp) here -- use sizeof(type)",
_tag_dynforward(_tmp1EE,sizeof(char),_get_zero_arr_size_char(_tmp1EE,51)))),
_tag_dynforward(_tmp1ED,sizeof(void*),0)));}return Cyc_Tcutil_typecmp((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp120->topt))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp122->topt))->v);_LLB8: _tmp123=_tmpF8.f1;if(*((int*)_tmp123)!= 20)
goto _LLBA;_tmp124=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp123)->f1;
_tmp125=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp123)->f2;_tmp126=_tmpF8.f2;
if(*((int*)_tmp126)!= 20)goto _LLBA;_tmp127=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp126)->f1;_tmp128=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp126)->f2;
_LLB9: {int _tmp13B=Cyc_Tcutil_typecmp(_tmp124,_tmp127);if(_tmp13B != 0)return
_tmp13B;{struct _tuple3 _tmp1EF;struct _tuple3 _tmp13D=(_tmp1EF.f1=_tmp125,((_tmp1EF.f2=
_tmp128,_tmp1EF)));void*_tmp13E;void*_tmp13F;void*_tmp140;void*_tmp141;void*
_tmp142;struct _dynforward_ptr*_tmp143;void*_tmp144;struct _dynforward_ptr*_tmp145;
void*_tmp146;unsigned int _tmp147;void*_tmp148;unsigned int _tmp149;_LLBF: _tmp13E=
_tmp13D.f1;if(*((int*)_tmp13E)!= 0)goto _LLC1;_tmp13F=_tmp13D.f2;if(*((int*)
_tmp13F)!= 1)goto _LLC1;_LLC0: return - 1;_LLC1: _tmp140=_tmp13D.f1;if(*((int*)
_tmp140)!= 1)goto _LLC3;_tmp141=_tmp13D.f2;if(*((int*)_tmp141)!= 0)goto _LLC3;
_LLC2: return 1;_LLC3: _tmp142=_tmp13D.f1;if(*((int*)_tmp142)!= 0)goto _LLC5;_tmp143=((
struct Cyc_Absyn_StructField_struct*)_tmp142)->f1;_tmp144=_tmp13D.f2;if(*((int*)
_tmp144)!= 0)goto _LLC5;_tmp145=((struct Cyc_Absyn_StructField_struct*)_tmp144)->f1;
_LLC4: return Cyc_strptrcmp(_tmp143,_tmp145);_LLC5: _tmp146=_tmp13D.f1;if(*((int*)
_tmp146)!= 1)goto _LLBE;_tmp147=((struct Cyc_Absyn_TupleIndex_struct*)_tmp146)->f1;
_tmp148=_tmp13D.f2;if(*((int*)_tmp148)!= 1)goto _LLBE;_tmp149=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp148)->f1;_LLC6: return(int)(_tmp147 - _tmp149);_LLBE:;}}_LLBA: _tmp129=_tmpF8.f1;
if(*((int*)_tmp129)!= 15)goto _LLBC;_tmp12A=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp129)->f1;_tmp12B=((struct Cyc_Absyn_Cast_e_struct*)_tmp129)->f2;_tmp12C=
_tmpF8.f2;if(*((int*)_tmp12C)!= 15)goto _LLBC;_tmp12D=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp12C)->f1;_tmp12E=((struct Cyc_Absyn_Cast_e_struct*)_tmp12C)->f2;_LLBB: {int
_tmp14A=Cyc_Tcutil_typecmp(_tmp12A,_tmp12D);if(_tmp14A != 0)return _tmp14A;return
Cyc_Evexp_const_exp_cmp(_tmp12B,_tmp12E);}_LLBC:;_LLBD: {const char*_tmp1F2;void*
_tmp1F1;(_tmp1F1=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(((_tmp1F2="Evexp::const_exp_cmp, unexpected case",
_tag_dynforward(_tmp1F2,sizeof(char),_get_zero_arr_size_char(_tmp1F2,38)))),
_tag_dynforward(_tmp1F1,sizeof(void*),0)));}_LLA7:;}}}}int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){return Cyc_Evexp_const_exp_cmp(e1,
e2)== 0;}int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
unsigned int _tmp14E;int _tmp14F;struct _tuple4 _tmp14D=Cyc_Evexp_eval_const_uint_exp(
e1);_tmp14E=_tmp14D.f1;_tmp14F=_tmp14D.f2;{unsigned int _tmp151;int _tmp152;struct
_tuple4 _tmp150=Cyc_Evexp_eval_const_uint_exp(e2);_tmp151=_tmp150.f1;_tmp152=
_tmp150.f2;if(_tmp14F  && _tmp152)return _tmp14E <= _tmp151;return Cyc_Evexp_same_const_exp(
e1,e2);}}struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};int Cyc_Evexp_okay_szofarg(
void*t);int Cyc_Evexp_okay_szofarg(void*t){void*_tmp153=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_Tvar*_tmp154;struct Cyc_Core_Opt*_tmp155;struct Cyc_Core_Opt
_tmp156;void*_tmp157;struct Cyc_Absyn_TunionFieldInfo _tmp158;union Cyc_Absyn_TunionFieldInfoU_union
_tmp159;struct Cyc_Absyn_Tunionfield*_tmp15A;struct Cyc_List_List*_tmp15B;struct
Cyc_Absyn_ArrayInfo _tmp15C;void*_tmp15D;struct Cyc_Absyn_Exp*_tmp15E;struct Cyc_Absyn_AggrInfo
_tmp15F;union Cyc_Absyn_AggrInfoU_union _tmp160;struct Cyc_List_List*_tmp161;struct
Cyc_Absyn_Typedefdecl*_tmp162;_LLC8: if((int)_tmp153 != 0)goto _LLCA;_LLC9: return 0;
_LLCA: if(_tmp153 <= (void*)4)goto _LLE0;if(*((int*)_tmp153)!= 1)goto _LLCC;_tmp154=((
struct Cyc_Absyn_VarType_struct*)_tmp153)->f1;_LLCB: {void*_tmp163=Cyc_Tcutil_tvar_kind(
_tmp154);_LL103: if((int)_tmp163 != 2)goto _LL105;_LL104: return 1;_LL105:;_LL106:
return 0;_LL102:;}_LLCC: if(*((int*)_tmp153)!= 0)goto _LLCE;_tmp155=((struct Cyc_Absyn_Evar_struct*)
_tmp153)->f1;if(_tmp155 == 0)goto _LLCE;_tmp156=*_tmp155;_tmp157=(void*)_tmp156.v;
if((int)_tmp157 != 2)goto _LLCE;_LLCD: return 1;_LLCE: if(*((int*)_tmp153)!= 0)goto
_LLD0;_LLCF: return 0;_LLD0: if(*((int*)_tmp153)!= 2)goto _LLD2;_LLD1: return 1;_LLD2:
if(*((int*)_tmp153)!= 3)goto _LLD4;_tmp158=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp153)->f1;_tmp159=_tmp158.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp153)->f1).field_info).KnownTunionfield).tag != 1)goto _LLD4;_tmp15A=(_tmp159.KnownTunionfield).f2;
_LLD3: _tmp15B=_tmp15A->typs;goto _LLD5;_LLD4: if(*((int*)_tmp153)!= 9)goto _LLD6;
_tmp15B=((struct Cyc_Absyn_TupleType_struct*)_tmp153)->f1;_LLD5: for(0;_tmp15B != 0;
_tmp15B=_tmp15B->tl){if(!Cyc_Evexp_okay_szofarg((*((struct _tuple9*)_tmp15B->hd)).f2))
return 0;}return 1;_LLD6: if(*((int*)_tmp153)!= 3)goto _LLD8;_LLD7: return 0;_LLD8: if(*((
int*)_tmp153)!= 4)goto _LLDA;_LLD9: return 1;_LLDA: if(*((int*)_tmp153)!= 14)goto
_LLDC;_LLDB: goto _LLDD;_LLDC: if(*((int*)_tmp153)!= 18)goto _LLDE;_LLDD: goto _LLDF;
_LLDE: if(*((int*)_tmp153)!= 5)goto _LLE0;_LLDF: goto _LLE1;_LLE0: if((int)_tmp153 != 
1)goto _LLE2;_LLE1: goto _LLE3;_LLE2: if(_tmp153 <= (void*)4)goto _LLF6;if(*((int*)
_tmp153)!= 6)goto _LLE4;_LLE3: return 1;_LLE4: if(*((int*)_tmp153)!= 7)goto _LLE6;
_tmp15C=((struct Cyc_Absyn_ArrayType_struct*)_tmp153)->f1;_tmp15D=(void*)_tmp15C.elt_type;
_tmp15E=_tmp15C.num_elts;_LLE5: return _tmp15E != 0;_LLE6: if(*((int*)_tmp153)!= 8)
goto _LLE8;_LLE7: return 0;_LLE8: if(*((int*)_tmp153)!= 15)goto _LLEA;_LLE9: return 1;
_LLEA: if(*((int*)_tmp153)!= 16)goto _LLEC;_LLEB: return 1;_LLEC: if(*((int*)_tmp153)
!= 10)goto _LLEE;_tmp15F=((struct Cyc_Absyn_AggrType_struct*)_tmp153)->f1;_tmp160=
_tmp15F.aggr_info;_LLED: {struct Cyc_Absyn_Aggrdecl*ad;{union Cyc_Absyn_AggrInfoU_union
_tmp164=_tmp160;struct Cyc_Absyn_Aggrdecl**_tmp165;struct Cyc_Absyn_Aggrdecl*
_tmp166;_LL108: if((_tmp164.UnknownAggr).tag != 0)goto _LL10A;_LL109: {const char*
_tmp1F5;void*_tmp1F4;(_tmp1F4=0,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp1F5="szof on unchecked StructType",
_tag_dynforward(_tmp1F5,sizeof(char),_get_zero_arr_size_char(_tmp1F5,29)))),
_tag_dynforward(_tmp1F4,sizeof(void*),0)));}_LL10A: if((_tmp164.KnownAggr).tag != 
1)goto _LL107;_tmp165=(_tmp164.KnownAggr).f1;_tmp166=*_tmp165;_LL10B: ad=_tmp166;
_LL107:;}if(ad->impl == 0)return 0;_tmp161=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->fields;goto _LLEF;}_LLEE: if(*((int*)_tmp153)!= 11)goto
_LLF0;_tmp161=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp153)->f2;_LLEF: for(0;
_tmp161 != 0;_tmp161=_tmp161->tl){if(!Cyc_Evexp_okay_szofarg((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp161->hd)->type))return 0;}return 1;_LLF0: if(*((int*)_tmp153)!= 12)goto _LLF2;
_LLF1: goto _LLF3;_LLF2: if(*((int*)_tmp153)!= 13)goto _LLF4;_LLF3: return 1;_LLF4: if(*((
int*)_tmp153)!= 19)goto _LLF6;_LLF5: goto _LLF7;_LLF6: if((int)_tmp153 != 2)goto _LLF8;
_LLF7: goto _LLF9;_LLF8: if((int)_tmp153 != 3)goto _LLFA;_LLF9: goto _LLFB;_LLFA: if(
_tmp153 <= (void*)4)goto _LLFC;if(*((int*)_tmp153)!= 21)goto _LLFC;_LLFB: goto _LLFD;
_LLFC: if(_tmp153 <= (void*)4)goto _LLFE;if(*((int*)_tmp153)!= 20)goto _LLFE;_LLFD:
goto _LLFF;_LLFE: if(_tmp153 <= (void*)4)goto _LL100;if(*((int*)_tmp153)!= 22)goto
_LL100;_LLFF: return 0;_LL100: if(_tmp153 <= (void*)4)goto _LLC7;if(*((int*)_tmp153)
!= 17)goto _LLC7;_tmp162=((struct Cyc_Absyn_TypedefType_struct*)_tmp153)->f3;
_LL101: if(_tmp162 == 0  || _tmp162->kind == 0){struct Cyc_String_pa_struct _tmp1FD;
void*_tmp1FC[1];const char*_tmp1FB;void*_tmp1FA;(_tmp1FA=0,((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)((struct
_dynforward_ptr)((_tmp1FD.tag=0,((_tmp1FD.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1FC[0]=& _tmp1FD,Cyc_aprintf(((
_tmp1FB="szof typedeftype %s",_tag_dynforward(_tmp1FB,sizeof(char),
_get_zero_arr_size_char(_tmp1FB,20)))),_tag_dynforward(_tmp1FC,sizeof(void*),1)))))))),
_tag_dynforward(_tmp1FA,sizeof(void*),0)));}return(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp162->kind))->v == (void*)2;_LLC7:;}
