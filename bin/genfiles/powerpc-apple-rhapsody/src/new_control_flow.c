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
void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(struct _tuple0*);int
Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;extern char Cyc_Core_Open_Region[
16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct
_dynforward_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_rmerge(struct _RegionHandle*,int(*cmp)(void*,void*),
struct Cyc_List_List*a,struct Cyc_List_List*b);extern char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);struct Cyc_List_List*Cyc_List_rzip(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);int Cyc_List_memq(struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc___cycFILE;extern struct
Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;
};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dynforward_ptr,
struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(
struct Cyc_Dict_Dict d,void*k);int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,
void**ans);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict
d);struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);
struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*,struct Cyc_Dict_Dict,
void*);struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict
d);int Cyc_strptrcmp(struct _dynforward_ptr*s1,struct _dynforward_ptr*s2);struct Cyc_Hashtable_Table;
struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(struct _RegionHandle*r,int sz,int(*
cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*
t,void*key,void*val);void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,
void*key);struct Cyc_Lineno_Pos{struct _dynforward_ptr logical_file;struct
_dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;
struct Cyc_Position_Error{struct _dynforward_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];
int Cyc_Position_error_p();struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Conref*
Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_val(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU_union
info);int Cyc_Absyn_is_union_type(void*);int Cyc_Absyn_is_aggr_type(void*t);struct
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
le;};void*Cyc_Tcutil_impos(struct _dynforward_ptr fmt,struct _dynforward_ptr ap);
void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap);void*Cyc_Tcutil_compress(void*t);
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b);int Cyc_Tcutil_is_noalias_pointer(void*t);int Cyc_Tcutil_is_noalias_path(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);int Cyc_Tcutil_is_noreturn(
void*);struct _tuple4{unsigned int f1;int f2;};struct _tuple4 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
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
struct Cyc_List_List*Cyc_CfFlowInfo_copy_tagcmps(struct _RegionHandle*,struct Cyc_List_List*);
struct Cyc_CfFlowInfo_PlaceL_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct
Cyc_CfFlowInfo_UnknownL_struct{int tag;};union Cyc_CfFlowInfo_AbsLVal_union{struct
Cyc_CfFlowInfo_PlaceL_struct PlaceL;struct Cyc_CfFlowInfo_UnknownL_struct UnknownL;
};struct Cyc_CfFlowInfo_UnknownR_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Esc_struct{
int tag;void*f1;};struct Cyc_CfFlowInfo_AddressOf_struct{int tag;struct Cyc_CfFlowInfo_Place*
f1;};struct Cyc_CfFlowInfo_TagCmps_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_CfFlowInfo_Aggregate_struct{int tag;struct _dynforward_ptr f1;};int Cyc_CfFlowInfo_update_place_set(
struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,struct Cyc_Position_Segment*
loc);struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,
struct Cyc_Dict_Dict s2,int disjoint);struct Cyc_CfFlowInfo_ConsumeInfo{struct Cyc_Dict_Dict
consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_ConsumeInfo Cyc_CfFlowInfo_and_consume(
struct _RegionHandle*,struct Cyc_CfFlowInfo_ConsumeInfo c1,struct Cyc_CfFlowInfo_ConsumeInfo
c2);struct Cyc_CfFlowInfo_BottomFL_struct{int tag;};struct Cyc_CfFlowInfo_ReachableFL_struct{
int tag;struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo
f3;};union Cyc_CfFlowInfo_FlowInfo_union{struct Cyc_CfFlowInfo_BottomFL_struct
BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{
struct _RegionHandle*r;void*unknown_none;void*unknown_this;void*unknown_all;void*
esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict
mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_CfFlowInfo_FlowEnv*
Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);int Cyc_CfFlowInfo_place_cmp(
struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);struct _dynforward_ptr
Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,
void*);void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,
void*leafval);void*Cyc_CfFlowInfo_initlevel(struct Cyc_Dict_Dict d,void*r);void*
Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*
place);int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo_union
f1,union Cyc_CfFlowInfo_FlowInfo_union f2);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_var(
struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(struct _RegionHandle*,struct
Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,struct Cyc_List_List*);struct
Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,
struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);struct Cyc_Dict_Dict
Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Position_Segment*
loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*
place,void*r);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_join_flow(struct
Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo_union,
union Cyc_CfFlowInfo_FlowInfo_union,int);struct _tuple5{union Cyc_CfFlowInfo_FlowInfo_union
f1;void*f2;};struct _tuple5 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,
struct Cyc_Dict_Dict*all_changed,struct _tuple5 pr1,struct _tuple5 pr2,int);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,
union Cyc_CfFlowInfo_FlowInfo_union,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Dict_Dict,
struct Cyc_Dict_Dict);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_kill_flow_region(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union f,void*rgn);
struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*f1;};union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_consume_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);void Cyc_CfFlowInfo_check_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);union Cyc_CfFlowInfo_FlowInfo_union Cyc_CfFlowInfo_readthrough_unique_rvals(
struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union f);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_drop_unique_rvals(struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
f);struct _tuple6{struct Cyc_CfFlowInfo_ConsumeInfo f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;};struct _tuple6 Cyc_CfFlowInfo_save_consume_info(struct Cyc_CfFlowInfo_FlowEnv*,
union Cyc_CfFlowInfo_FlowInfo_union f,int clear);union Cyc_CfFlowInfo_FlowInfo_union
Cyc_CfFlowInfo_restore_consume_info(union Cyc_CfFlowInfo_FlowInfo_union f,struct
Cyc_CfFlowInfo_ConsumeInfo c);struct _dynforward_ptr Cyc_CfFlowInfo_place_err_string(
struct Cyc_CfFlowInfo_Place*place);void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*
enclosee,struct Cyc_Absyn_Stmt*encloser);void Cyc_NewControlFlow_cf_check(struct
Cyc_List_List*ds);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct
Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;
int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int
print_all_kinds: 1;int print_all_effects: 1;int print_using_stmts: 1;int
print_externC_stmts: 1;int print_full_evars: 1;int print_zeroterm: 1;int
generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dynforward_ptr Cyc_Absynpp_typ2string(void*);struct
_dynforward_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_NewControlFlow_CFStmtAnnot{
struct Cyc_Absyn_Stmt*encloser;int visited;};static char Cyc_NewControlFlow_CFAnnot[
12]="\000\000\000\000CFAnnot\000";struct Cyc_NewControlFlow_CFAnnot_struct{char*
tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);void Cyc_NewControlFlow_set_encloser(
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser){struct Cyc_NewControlFlow_CFAnnot_struct
_tmp5C4;struct Cyc_NewControlFlow_CFStmtAnnot _tmp5C3;struct Cyc_NewControlFlow_CFAnnot_struct*
_tmp5C2;(void*)(enclosee->annot=(void*)((void*)((_tmp5C2=_cycalloc(sizeof(*
_tmp5C2)),((_tmp5C2[0]=((_tmp5C4.tag=Cyc_NewControlFlow_CFAnnot,((_tmp5C4.f1=((
_tmp5C3.encloser=encloser,((_tmp5C3.visited=0,_tmp5C3)))),_tmp5C4)))),_tmp5C2))))));}
struct Cyc_NewControlFlow_AnalEnv{struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*
fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo_union
tryflow;struct Cyc_Dict_Dict*all_changed;int noreturn;struct Cyc_List_List*
param_roots;struct Cyc_Hashtable_Table*flow_table;};static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,
struct Cyc_Absyn_Stmt*);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Decl*);
struct _tuple7{union Cyc_CfFlowInfo_FlowInfo_union f1;union Cyc_CfFlowInfo_AbsLVal_union
f2;};static struct _tuple7 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);static struct _tuple5 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);
struct _tuple8{union Cyc_CfFlowInfo_FlowInfo_union f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;};static struct _tuple8 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);static struct Cyc_List_List*
Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*env,struct
Cyc_CfFlowInfo_Place*p,void*t);static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(
struct Cyc_Absyn_Stmt*s);static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(
struct Cyc_Absyn_Stmt*s){void*_tmp3=(void*)s->annot;struct Cyc_NewControlFlow_CFStmtAnnot
_tmp4;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp5;_LL1: if(*((void**)_tmp3)!= Cyc_NewControlFlow_CFAnnot)
goto _LL3;_tmp4=((struct Cyc_NewControlFlow_CFAnnot_struct*)_tmp3)->f1;_tmp5=(
struct Cyc_NewControlFlow_CFStmtAnnot*)&((struct Cyc_NewControlFlow_CFAnnot_struct*)
_tmp3)->f1;_LL2: return _tmp5;_LL3:;_LL4: {struct Cyc_Core_Impossible_struct _tmp5CA;
const char*_tmp5C9;struct Cyc_Core_Impossible_struct*_tmp5C8;(int)_throw((void*)((
_tmp5C8=_cycalloc(sizeof(*_tmp5C8)),((_tmp5C8[0]=((_tmp5CA.tag=Cyc_Core_Impossible,((
_tmp5CA.f1=((_tmp5C9="ControlFlow -- wrong stmt annotation",_tag_dynforward(
_tmp5C9,sizeof(char),_get_zero_arr_size_char(_tmp5C9,37)))),_tmp5CA)))),_tmp5C8)))));}
_LL0:;}static union Cyc_CfFlowInfo_FlowInfo_union*Cyc_NewControlFlow_get_stmt_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s);static union Cyc_CfFlowInfo_FlowInfo_union*
Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*
s){union Cyc_CfFlowInfo_FlowInfo_union**sflow=((union Cyc_CfFlowInfo_FlowInfo_union**(*)(
struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(
env->flow_table,s);if(sflow == 0){union Cyc_CfFlowInfo_FlowInfo_union _tmp5CD;union
Cyc_CfFlowInfo_FlowInfo_union*_tmp5CC;union Cyc_CfFlowInfo_FlowInfo_union*res=(
_tmp5CC=_region_malloc(env->r,sizeof(*_tmp5CC)),((_tmp5CC[0]=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp5CD.BottomFL).tag=0,_tmp5CD)),_tmp5CC)));((void(*)(struct Cyc_Hashtable_Table*
t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo_union*val))Cyc_Hashtable_insert)(
env->flow_table,s,res);return res;}return*sflow;}struct _tuple9{struct Cyc_NewControlFlow_CFStmtAnnot*
f1;union Cyc_CfFlowInfo_FlowInfo_union*f2;};static struct _tuple9 Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Stmt*s);static struct _tuple9 Cyc_NewControlFlow_pre_stmt_check(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Stmt*s){struct Cyc_NewControlFlow_CFStmtAnnot*_tmpB=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo_union*_tmpC=Cyc_NewControlFlow_get_stmt_flow(env,
s);*_tmpC=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpC,1);
_tmpB->visited=env->iteration_num;{struct _tuple9 _tmp5CE;return(_tmp5CE.f1=_tmpB,((
_tmp5CE.f2=_tmpC,_tmp5CE)));}}static int Cyc_NewControlFlow_get_field_index_fs(
struct Cyc_List_List*fs,struct _dynforward_ptr*f);static int Cyc_NewControlFlow_get_field_index_fs(
struct Cyc_List_List*fs,struct _dynforward_ptr*f){int n=0;for(0;(unsigned int)fs;fs=
fs->tl){struct _dynforward_ptr*_tmpE=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;
if(Cyc_strptrcmp(_tmpE,f)== 0)return n;n=n + 1;}{struct Cyc_Core_Impossible_struct
_tmp5DB;const char*_tmp5DA;void*_tmp5D9[1];struct Cyc_String_pa_struct _tmp5D8;
struct Cyc_Core_Impossible_struct*_tmp5D7;(int)_throw((void*)((_tmp5D7=_cycalloc(
sizeof(*_tmp5D7)),((_tmp5D7[0]=((_tmp5DB.tag=Cyc_Core_Impossible,((_tmp5DB.f1=(
struct _dynforward_ptr)((_tmp5D8.tag=0,((_tmp5D8.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*f),((_tmp5D9[0]=& _tmp5D8,Cyc_aprintf(((_tmp5DA="get_field_index_fs failed: %s",
_tag_dynforward(_tmp5DA,sizeof(char),_get_zero_arr_size_char(_tmp5DA,30)))),
_tag_dynforward(_tmp5D9,sizeof(void*),1)))))))),_tmp5DB)))),_tmp5D7)))));}}
static int Cyc_NewControlFlow_get_field_index(void*t,struct _dynforward_ptr*f);
static int Cyc_NewControlFlow_get_field_index(void*t,struct _dynforward_ptr*f){void*
_tmp14=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo _tmp15;union Cyc_Absyn_AggrInfoU_union
_tmp16;struct Cyc_List_List*_tmp17;_LL6: if(_tmp14 <= (void*)4)goto _LLA;if(*((int*)
_tmp14)!= 10)goto _LL8;_tmp15=((struct Cyc_Absyn_AggrType_struct*)_tmp14)->f1;
_tmp16=_tmp15.aggr_info;_LL7: {struct Cyc_List_List*_tmp18=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp16))->impl))->fields;_tmp17=_tmp18;
goto _LL9;}_LL8: if(*((int*)_tmp14)!= 11)goto _LLA;_tmp17=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp14)->f2;_LL9: return Cyc_NewControlFlow_get_field_index_fs(_tmp17,f);_LLA:;
_LLB: {struct Cyc_Core_Impossible_struct _tmp5E8;const char*_tmp5E7;void*_tmp5E6[1];
struct Cyc_String_pa_struct _tmp5E5;struct Cyc_Core_Impossible_struct*_tmp5E4;(int)
_throw((void*)((_tmp5E4=_cycalloc(sizeof(*_tmp5E4)),((_tmp5E4[0]=((_tmp5E8.tag=
Cyc_Core_Impossible,((_tmp5E8.f1=(struct _dynforward_ptr)((_tmp5E5.tag=0,((
_tmp5E5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp5E6[0]=& _tmp5E5,Cyc_aprintf(((_tmp5E7="get_field_index failed: %s",
_tag_dynforward(_tmp5E7,sizeof(char),_get_zero_arr_size_char(_tmp5E7,27)))),
_tag_dynforward(_tmp5E6,sizeof(void*),1)))))))),_tmp5E8)))),_tmp5E4)))));}_LL5:;}
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,
union Cyc_CfFlowInfo_FlowInfo_union new_flow);static void Cyc_NewControlFlow_update_tryflow(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union new_flow){
if(env->in_try)env->tryflow=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,
new_flow,env->tryflow,1);}static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo_union flow);static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo_union
flow){struct Cyc_NewControlFlow_CFStmtAnnot*_tmp1E=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo_union*_tmp1F=Cyc_NewControlFlow_get_stmt_flow(
env,s);union Cyc_CfFlowInfo_FlowInfo_union _tmp20=Cyc_CfFlowInfo_join_flow(env->fenv,
env->all_changed,flow,*_tmp1F,1);if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp20,*
_tmp1F)){*_tmp1F=_tmp20;if(_tmp1E->visited == env->iteration_num)env->iterate_again=
1;}}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_add_vars(struct
Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_List_List*
vds,void*leafval,struct Cyc_Position_Segment*loc);static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_List_List*vds,void*leafval,struct Cyc_Position_Segment*loc){
union Cyc_CfFlowInfo_FlowInfo_union _tmp21=inflow;struct Cyc_Dict_Dict _tmp22;struct
Cyc_List_List*_tmp23;struct Cyc_CfFlowInfo_ConsumeInfo _tmp24;_LLD: if((_tmp21.BottomFL).tag
!= 0)goto _LLF;_LLE: {union Cyc_CfFlowInfo_FlowInfo_union _tmp5E9;return(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp5E9.BottomFL).tag=0,_tmp5E9));}_LLF: if((_tmp21.ReachableFL).tag != 1)goto _LLC;
_tmp22=(_tmp21.ReachableFL).f1;_tmp23=(_tmp21.ReachableFL).f2;_tmp24=(_tmp21.ReachableFL).f3;
_LL10: for(0;vds != 0;vds=vds->tl){struct Cyc_CfFlowInfo_VarRoot_struct _tmp5EC;
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp5EB;struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp26=(_tmp5EB=_region_malloc(fenv->r,sizeof(*_tmp5EB)),((_tmp5EB[0]=((_tmp5EC.tag=
0,((_tmp5EC.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp5EC)))),_tmp5EB)));_tmp22=
Cyc_Dict_insert(_tmp22,(void*)_tmp26,Cyc_CfFlowInfo_typ_to_absrval(fenv,(void*)((
struct Cyc_Absyn_Vardecl*)vds->hd)->type,leafval));}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp5ED;return(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp5ED.ReachableFL).tag=1,(((
_tmp5ED.ReachableFL).f1=_tmp22,(((_tmp5ED.ReachableFL).f2=_tmp23,(((_tmp5ED.ReachableFL).f3=
_tmp24,_tmp5ED))))))));}_LLC:;}struct _tuple10{struct Cyc_CfFlowInfo_Place*f1;
struct Cyc_Position_Segment*f2;};static void Cyc_NewControlFlow_remove_vars(struct
Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union outflow,struct Cyc_Dict_Dict*
out_unique_places,struct Cyc_Dict_Dict old_unique_places);static void Cyc_NewControlFlow_remove_vars(
struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union outflow,
struct Cyc_Dict_Dict*out_unique_places,struct Cyc_Dict_Dict old_unique_places){
struct Cyc_CfFlowInfo_ConsumeInfo _tmp2B;struct Cyc_Dict_Dict _tmp2C;struct _tuple6
_tmp2A=Cyc_CfFlowInfo_save_consume_info(fenv,outflow,0);_tmp2B=_tmp2A.f1;_tmp2C=
_tmp2B.consumed;{struct _RegionHandle _tmp2D=_new_region("tmp");struct
_RegionHandle*tmp=& _tmp2D;_push_region(tmp);{struct Cyc_Iter_Iter _tmp2E=((struct
Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(
tmp,*((struct Cyc_Dict_Dict*)_check_null(out_unique_places)));struct _tuple10
_tmp2F=*((struct _tuple10*(*)(struct _RegionHandle*r,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(
tmp,*out_unique_places);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple10*))Cyc_Iter_next)(
_tmp2E,& _tmp2F)){struct Cyc_CfFlowInfo_Place*_tmp30=_tmp2F.f1;if(!((int(*)(struct
Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp2C,_tmp30)){
const char*_tmp5F1;void*_tmp5F0[1];struct Cyc_String_pa_struct _tmp5EF;(_tmp5EF.tag=
0,((_tmp5EF.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_CfFlowInfo_place_err_string(
_tmp30)),((_tmp5F0[0]=& _tmp5EF,Cyc_Tcutil_terr(_tmp2F.f2,((_tmp5F1="Failed to consume unique variable %s",
_tag_dynforward(_tmp5F1,sizeof(char),_get_zero_arr_size_char(_tmp5F1,37)))),
_tag_dynforward(_tmp5F0,sizeof(void*),1)))))));}}};_pop_region(tmp);}*
out_unique_places=old_unique_places;}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_use_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
inflow,void*r);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_use_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
inflow,void*r){union Cyc_CfFlowInfo_FlowInfo_union _tmp34=inflow;struct Cyc_Dict_Dict
_tmp35;struct Cyc_List_List*_tmp36;struct Cyc_CfFlowInfo_ConsumeInfo _tmp37;_LL12:
if((_tmp34.BottomFL).tag != 0)goto _LL14;_LL13: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp5F2;return(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp5F2.BottomFL).tag=0,
_tmp5F2));}_LL14: if((_tmp34.ReachableFL).tag != 1)goto _LL11;_tmp35=(_tmp34.ReachableFL).f1;
_tmp36=(_tmp34.ReachableFL).f2;_tmp37=(_tmp34.ReachableFL).f3;_LL15: if(Cyc_CfFlowInfo_initlevel(
_tmp35,r)!= (void*)2){const char*_tmp5F5;void*_tmp5F4;(_tmp5F4=0,Cyc_Tcutil_terr(
loc,((_tmp5F5="expression may not be fully initialized",_tag_dynforward(_tmp5F5,
sizeof(char),_get_zero_arr_size_char(_tmp5F5,40)))),_tag_dynforward(_tmp5F4,
sizeof(void*),0)));}{struct Cyc_Dict_Dict _tmp3B=Cyc_CfFlowInfo_escape_deref(env->fenv,
_tmp35,env->all_changed,r);if(_tmp35.t == _tmp3B.t)return inflow;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp5F6;union Cyc_CfFlowInfo_FlowInfo_union _tmp3C=((_tmp5F6.ReachableFL).tag=1,(((
_tmp5F6.ReachableFL).f1=_tmp3B,(((_tmp5F6.ReachableFL).f2=_tmp36,(((_tmp5F6.ReachableFL).f3=
_tmp37,_tmp5F6)))))));Cyc_NewControlFlow_update_tryflow(env,(union Cyc_CfFlowInfo_FlowInfo_union)
_tmp3C);return(union Cyc_CfFlowInfo_FlowInfo_union)_tmp3C;}}_LL11:;}struct
_tuple11{union Cyc_CfFlowInfo_FlowInfo_union f1;struct Cyc_List_List*f2;};static
struct _tuple11 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*rgn,
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_List_List*es,int arg1_unconsumed);static struct _tuple11 Cyc_NewControlFlow_anal_unordered_Rexps(
struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_List_List*es,int arg1_unconsumed){if(es == 0){struct _tuple11
_tmp5F7;return(_tmp5F7.f1=inflow,((_tmp5F7.f2=0,_tmp5F7)));}if(es->tl == 0){union
Cyc_CfFlowInfo_FlowInfo_union _tmp40;void*_tmp41;struct _tuple5 _tmp3F=Cyc_NewControlFlow_anal_Rexp(
env,inflow,(struct Cyc_Absyn_Exp*)es->hd);_tmp40=_tmp3F.f1;_tmp41=_tmp3F.f2;{
struct Cyc_List_List*_tmp5FA;struct _tuple11 _tmp5F9;return(_tmp5F9.f1=_tmp40,((
_tmp5F9.f2=((_tmp5FA=_region_malloc(rgn,sizeof(*_tmp5FA)),((_tmp5FA->hd=(void*)
_tmp41,((_tmp5FA->tl=0,_tmp5FA)))))),_tmp5F9)));}}{struct Cyc_Dict_Dict*
outer_all_changed=env->all_changed;struct Cyc_Dict_Dict this_all_changed;union Cyc_CfFlowInfo_FlowInfo_union
old_inflow;union Cyc_CfFlowInfo_FlowInfo_union outflow;struct Cyc_List_List*rvals;
struct Cyc_CfFlowInfo_ConsumeInfo _tmp45;union Cyc_CfFlowInfo_FlowInfo_union _tmp46;
struct _tuple6 _tmp44=Cyc_CfFlowInfo_save_consume_info(env->fenv,inflow,1);_tmp45=
_tmp44.f1;_tmp46=_tmp44.f2;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp5FB;struct Cyc_CfFlowInfo_ConsumeInfo
outflow_consume=(_tmp5FB.consumed=(env->fenv)->mt_place_set,((_tmp5FB.may_consume=
0,_tmp5FB)));int init_consume=0;do{this_all_changed=(env->fenv)->mt_place_set;
_tmp46=Cyc_CfFlowInfo_restore_consume_info(_tmp46,_tmp45);{struct Cyc_Dict_Dict*
_tmp5FC;env->all_changed=((_tmp5FC=_region_malloc(env->r,sizeof(*_tmp5FC)),((
_tmp5FC[0]=(env->fenv)->mt_place_set,_tmp5FC))));}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp49;void*_tmp4A;struct _tuple5 _tmp48=Cyc_NewControlFlow_anal_Rexp(env,_tmp46,(
struct Cyc_Absyn_Exp*)es->hd);_tmp49=_tmp48.f1;_tmp4A=_tmp48.f2;outflow=_tmp49;{
struct Cyc_List_List*_tmp5FD;rvals=((_tmp5FD=_region_malloc(rgn,sizeof(*_tmp5FD)),((
_tmp5FD->hd=(void*)_tmp4A,((_tmp5FD->tl=0,_tmp5FD))))));}this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);if(
arg1_unconsumed){union Cyc_CfFlowInfo_FlowInfo_union _tmp4C=outflow;struct Cyc_Dict_Dict
_tmp4D;struct Cyc_List_List*_tmp4E;struct Cyc_CfFlowInfo_ConsumeInfo _tmp4F;_LL17:
if((_tmp4C.BottomFL).tag != 0)goto _LL19;_LL18: goto _LL16;_LL19: if((_tmp4C.ReachableFL).tag
!= 1)goto _LL16;_tmp4D=(_tmp4C.ReachableFL).f1;_tmp4E=(_tmp4C.ReachableFL).f2;
_tmp4F=(_tmp4C.ReachableFL).f3;_LL1A: {struct Cyc_CfFlowInfo_ConsumeInfo _tmp600;
union Cyc_CfFlowInfo_FlowInfo_union _tmp5FF;outflow=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp5FF.ReachableFL).tag=1,(((_tmp5FF.ReachableFL).f1=_tmp4D,(((_tmp5FF.ReachableFL).f2=
_tmp4E,(((_tmp5FF.ReachableFL).f3=((_tmp600.consumed=_tmp4F.consumed,((_tmp600.may_consume=
_tmp45.may_consume,_tmp600)))),_tmp5FF))))))));}_LL16:;}{struct Cyc_List_List*es2=
es->tl;for(0;es2 != 0;es2=es2->tl){{struct Cyc_Dict_Dict*_tmp601;env->all_changed=((
_tmp601=_region_malloc(env->r,sizeof(*_tmp601)),((_tmp601[0]=(env->fenv)->mt_place_set,
_tmp601))));}{union Cyc_CfFlowInfo_FlowInfo_union _tmp54;void*_tmp55;struct _tuple5
_tmp53=Cyc_NewControlFlow_anal_Rexp(env,_tmp46,(struct Cyc_Absyn_Exp*)es2->hd);
_tmp54=_tmp53.f1;_tmp55=_tmp53.f2;{struct Cyc_List_List*_tmp602;rvals=((_tmp602=
_region_malloc(rgn,sizeof(*_tmp602)),((_tmp602->hd=(void*)_tmp55,((_tmp602->tl=
rvals,_tmp602))))));}outflow=Cyc_CfFlowInfo_after_flow(env->fenv,(struct Cyc_Dict_Dict*)&
this_all_changed,outflow,_tmp54,this_all_changed,*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed)));this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);}}}{
struct Cyc_CfFlowInfo_ConsumeInfo _tmp58;struct _tuple6 _tmp57=Cyc_CfFlowInfo_save_consume_info(
env->fenv,outflow,0);_tmp58=_tmp57.f1;if(init_consume)old_inflow=Cyc_CfFlowInfo_restore_consume_info(
_tmp46,outflow_consume);else{old_inflow=_tmp46;}init_consume=1;outflow_consume=
_tmp58;_tmp46=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,_tmp46,
outflow,1);}}}while(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp46,old_inflow));if(
outer_all_changed == 0)env->all_changed=0;else{struct Cyc_Dict_Dict*_tmp603;env->all_changed=((
_tmp603=_region_malloc(env->r,sizeof(*_tmp603)),((_tmp603[0]=Cyc_CfFlowInfo_union_place_set(*
outer_all_changed,this_all_changed,0),_tmp603))));}Cyc_NewControlFlow_update_tryflow(
env,outflow);{struct _tuple11 _tmp604;return(_tmp604.f1=outflow,((_tmp604.f2=Cyc_List_imp_rev(
rvals),_tmp604)));}}}}static struct _tuple5 Cyc_NewControlFlow_anal_use_ints(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct
Cyc_List_List*es,int arg1_unconsumed);static struct _tuple5 Cyc_NewControlFlow_anal_use_ints(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_List_List*es,int arg1_unconsumed){struct _RegionHandle _tmp5C=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp5C;_push_region(rgn);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp5E;struct Cyc_List_List*_tmp5F;struct _tuple11 _tmp5D=Cyc_NewControlFlow_anal_unordered_Rexps(
rgn,env,inflow,es,arg1_unconsumed);_tmp5E=_tmp5D.f1;_tmp5F=_tmp5D.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp60=_tmp5E;struct Cyc_Dict_Dict _tmp61;_LL1C: if((_tmp60.ReachableFL).tag != 1)
goto _LL1E;_tmp61=(_tmp60.ReachableFL).f1;_LL1D: for(0;_tmp5F != 0;(_tmp5F=_tmp5F->tl,
es=((struct Cyc_List_List*)_check_null(es))->tl)){if(Cyc_CfFlowInfo_initlevel(
_tmp61,(void*)_tmp5F->hd)== (void*)0){const char*_tmp607;void*_tmp606;(_tmp606=0,
Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((
_tmp607="expression may not be initialized",_tag_dynforward(_tmp607,sizeof(char),
_get_zero_arr_size_char(_tmp607,34)))),_tag_dynforward(_tmp606,sizeof(void*),0)));}}
goto _LL1B;_LL1E: if((_tmp60.BottomFL).tag != 0)goto _LL1B;_LL1F: goto _LL1B;_LL1B:;}{
struct _tuple5 _tmp608;struct _tuple5 _tmp65=(_tmp608.f1=_tmp5E,((_tmp608.f2=(void*)(
env->fenv)->unknown_all,_tmp608)));_npop_handler(0);return _tmp65;}};_pop_region(
rgn);}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_notzero(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,union Cyc_CfFlowInfo_FlowInfo_union
outflow,struct Cyc_Absyn_Exp*e,void*il);static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,union Cyc_CfFlowInfo_FlowInfo_union outflow,struct Cyc_Absyn_Exp*e,void*il){
union Cyc_CfFlowInfo_FlowInfo_union _tmp66=outflow;struct Cyc_Dict_Dict _tmp67;
struct Cyc_List_List*_tmp68;struct Cyc_CfFlowInfo_ConsumeInfo _tmp69;_LL21: if((
_tmp66.BottomFL).tag != 0)goto _LL23;_LL22: return outflow;_LL23: if((_tmp66.ReachableFL).tag
!= 1)goto _LL20;_tmp67=(_tmp66.ReachableFL).f1;_tmp68=(_tmp66.ReachableFL).f2;
_tmp69=(_tmp66.ReachableFL).f3;_LL24: {union Cyc_CfFlowInfo_AbsLVal_union _tmp6A=(
Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp6B;
_LL26: if((_tmp6A.UnknownL).tag != 1)goto _LL28;_LL27: return outflow;_LL28: if((
_tmp6A.PlaceL).tag != 0)goto _LL25;_tmp6B=(_tmp6A.PlaceL).f1;_LL29: {void*nzval=il
== (void*)2?(void*)1:(void*)2;union Cyc_CfFlowInfo_FlowInfo_union _tmp609;union Cyc_CfFlowInfo_FlowInfo_union
_tmp6C=((_tmp609.ReachableFL).tag=1,(((_tmp609.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmp67,env->all_changed,_tmp6B,nzval),(((_tmp609.ReachableFL).f2=
_tmp68,(((_tmp609.ReachableFL).f3=_tmp69,_tmp609)))))));return(union Cyc_CfFlowInfo_FlowInfo_union)
_tmp6C;}_LL25:;}_LL20:;}static struct _tuple8 Cyc_NewControlFlow_splitzero(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,union Cyc_CfFlowInfo_FlowInfo_union
outflow,struct Cyc_Absyn_Exp*e,void*il);static struct _tuple8 Cyc_NewControlFlow_splitzero(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
union Cyc_CfFlowInfo_FlowInfo_union outflow,struct Cyc_Absyn_Exp*e,void*il){union
Cyc_CfFlowInfo_FlowInfo_union _tmp6E=outflow;struct Cyc_Dict_Dict _tmp6F;struct Cyc_List_List*
_tmp70;struct Cyc_CfFlowInfo_ConsumeInfo _tmp71;_LL2B: if((_tmp6E.BottomFL).tag != 0)
goto _LL2D;_LL2C: {struct _tuple8 _tmp60A;return(_tmp60A.f1=outflow,((_tmp60A.f2=
outflow,_tmp60A)));}_LL2D: if((_tmp6E.ReachableFL).tag != 1)goto _LL2A;_tmp6F=(
_tmp6E.ReachableFL).f1;_tmp70=(_tmp6E.ReachableFL).f2;_tmp71=(_tmp6E.ReachableFL).f3;
_LL2E: {union Cyc_CfFlowInfo_AbsLVal_union _tmp73=(Cyc_NewControlFlow_anal_Lexp(
env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp74;_LL30: if((_tmp73.UnknownL).tag
!= 1)goto _LL32;_LL31: {struct _tuple8 _tmp60B;return(_tmp60B.f1=outflow,((_tmp60B.f2=
outflow,_tmp60B)));}_LL32: if((_tmp73.PlaceL).tag != 0)goto _LL2F;_tmp74=(_tmp73.PlaceL).f1;
_LL33: {void*nzval=il == (void*)2?(void*)1:(void*)2;union Cyc_CfFlowInfo_FlowInfo_union
_tmp610;union Cyc_CfFlowInfo_FlowInfo_union _tmp60F;struct _tuple8 _tmp60E;return(
_tmp60E.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp60F.ReachableFL).tag=1,(((
_tmp60F.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp6F,env->all_changed,
_tmp74,nzval),(((_tmp60F.ReachableFL).f2=_tmp70,(((_tmp60F.ReachableFL).f3=
_tmp71,_tmp60F)))))))),((_tmp60E.f2=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp610.ReachableFL).tag=1,(((_tmp610.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmp6F,env->all_changed,_tmp74,(void*)0),(((_tmp610.ReachableFL).f2=
_tmp70,(((_tmp610.ReachableFL).f3=_tmp71,_tmp610)))))))),_tmp60E)));}_LL2F:;}
_LL2A:;}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_if_tagcmp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union flow,
struct Cyc_Absyn_Exp*e1,void*r1,void*p,void*r2);static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_if_tagcmp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
flow,struct Cyc_Absyn_Exp*e1,void*r1,void*p,void*r2){union Cyc_CfFlowInfo_FlowInfo_union
_tmp79=flow;struct Cyc_Dict_Dict _tmp7A;struct Cyc_List_List*_tmp7B;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp7C;_LL35: if((_tmp79.BottomFL).tag != 0)goto _LL37;_LL36: return flow;_LL37: if((
_tmp79.ReachableFL).tag != 1)goto _LL34;_tmp7A=(_tmp79.ReachableFL).f1;_tmp7B=(
_tmp79.ReachableFL).f2;_tmp7C=(_tmp79.ReachableFL).f3;_LL38: {void*_tmp7D=r2;
struct Cyc_List_List*_tmp7E;_LL3A: if(_tmp7D <= (void*)3)goto _LL3C;if(*((int*)
_tmp7D)!= 3)goto _LL3C;_tmp7E=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp7D)->f1;
_LL3B: {union Cyc_CfFlowInfo_AbsLVal_union _tmp7F=(Cyc_NewControlFlow_anal_Lexp(
env,flow,e1)).f2;struct Cyc_CfFlowInfo_Place*_tmp80;_LL3F: if((_tmp7F.UnknownL).tag
!= 1)goto _LL41;_LL40: return flow;_LL41: if((_tmp7F.PlaceL).tag != 0)goto _LL3E;
_tmp80=(_tmp7F.PlaceL).f1;_LL42: {struct Cyc_List_List*new_cl;{void*_tmp81=r1;
struct Cyc_List_List*_tmp82;void*_tmp83;_LL44: if(_tmp81 <= (void*)3)goto _LL48;if(*((
int*)_tmp81)!= 3)goto _LL46;_tmp82=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp81)->f1;
_LL45: new_cl=_tmp82;goto _LL43;_LL46: if(*((int*)_tmp81)!= 0)goto _LL48;_tmp83=(
void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp81)->f1;if((int)_tmp83 != 2)
goto _LL48;_LL47: goto _LL49;_LL48: if((int)_tmp81 != 0)goto _LL4A;_LL49: goto _LL4B;
_LL4A: if((int)_tmp81 != 1)goto _LL4C;_LL4B: new_cl=0;goto _LL43;_LL4C:;_LL4D: return
flow;_LL43:;}for(0;_tmp7E != 0;_tmp7E=_tmp7E->tl){void*new_cmp;{struct _tuple0
_tmp611;struct _tuple0 _tmp85=(_tmp611.f1=p,((_tmp611.f2=(void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp7E->hd)->cmp,_tmp611)));void*_tmp86;void*_tmp87;void*_tmp88;void*_tmp89;
_LL4F: _tmp86=_tmp85.f1;if((int)_tmp86 != 8)goto _LL51;_LL50: goto _LL52;_LL51: _tmp87=
_tmp85.f2;if((int)_tmp87 != 8)goto _LL53;_LL52: new_cmp=(void*)8;goto _LL4E;_LL53:
_tmp88=_tmp85.f1;if((int)_tmp88 != 5)goto _LL55;_tmp89=_tmp85.f2;if((int)_tmp89 != 
5)goto _LL55;_LL54: new_cmp=(void*)5;goto _LL4E;_LL55:;_LL56: new_cmp=(void*)10;goto
_LL4E;_LL4E:;}{struct Cyc_CfFlowInfo_TagCmp*_tmp614;struct Cyc_List_List*_tmp613;
new_cl=((_tmp613=_region_malloc(env->r,sizeof(*_tmp613)),((_tmp613->hd=((_tmp614=
_region_malloc(env->r,sizeof(*_tmp614)),((_tmp614->cmp=(void*)new_cmp,((_tmp614->bd=(
void*)((void*)((struct Cyc_CfFlowInfo_TagCmp*)_tmp7E->hd)->bd),_tmp614)))))),((
_tmp613->tl=new_cl,_tmp613))))));}}{struct Cyc_CfFlowInfo_TagCmps_struct*_tmp61A;
struct Cyc_CfFlowInfo_TagCmps_struct _tmp619;union Cyc_CfFlowInfo_FlowInfo_union
_tmp618;return(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp618.ReachableFL).tag=1,(((
_tmp618.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(env->fenv,e1->loc,_tmp7A,env->all_changed,
_tmp80,(void*)((_tmp61A=_region_malloc(env->r,sizeof(*_tmp61A)),((_tmp61A[0]=((
_tmp619.tag=3,((_tmp619.f1=new_cl,_tmp619)))),_tmp61A))))),(((_tmp618.ReachableFL).f2=
_tmp7B,(((_tmp618.ReachableFL).f3=_tmp7C,_tmp618))))))));}}_LL3E:;}_LL3C:;_LL3D:
return flow;_LL39:;}_LL34:;}static struct Cyc_CfFlowInfo_NotZero_struct Cyc_NewControlFlow_mt_notzero_v={
Cyc_CfFlowInfo_NotZero,0};static struct Cyc_CfFlowInfo_UnknownZ_struct Cyc_NewControlFlow_mt_unknownz_v={
Cyc_CfFlowInfo_UnknownZ,0};static struct _tuple5 Cyc_NewControlFlow_anal_derefR(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
union Cyc_CfFlowInfo_FlowInfo_union f,struct Cyc_Absyn_Exp*e,void*r);static struct
_tuple5 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo_union inflow,union Cyc_CfFlowInfo_FlowInfo_union f,struct
Cyc_Absyn_Exp*e,void*r){void*_tmp91=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo _tmp92;void*_tmp93;struct Cyc_Absyn_PtrAtts
_tmp94;struct Cyc_Absyn_Conref*_tmp95;struct Cyc_Absyn_Conref*_tmp96;_LL58: if(
_tmp91 <= (void*)4)goto _LL5A;if(*((int*)_tmp91)!= 4)goto _LL5A;_tmp92=((struct Cyc_Absyn_PointerType_struct*)
_tmp91)->f1;_tmp93=(void*)_tmp92.elt_typ;_tmp94=_tmp92.ptr_atts;_tmp95=_tmp94.bounds;
_tmp96=_tmp94.zero_term;_LL59: {union Cyc_CfFlowInfo_FlowInfo_union _tmp97=f;
struct Cyc_Dict_Dict _tmp98;struct Cyc_List_List*_tmp99;_LL5D: if((_tmp97.BottomFL).tag
!= 0)goto _LL5F;_LL5E: {struct _tuple5 _tmp61B;return(_tmp61B.f1=f,((_tmp61B.f2=Cyc_CfFlowInfo_typ_to_absrval(
env->fenv,_tmp93,(void*)(env->fenv)->unknown_all),_tmp61B)));}_LL5F: if((_tmp97.ReachableFL).tag
!= 1)goto _LL5C;_tmp98=(_tmp97.ReachableFL).f1;_tmp99=(_tmp97.ReachableFL).f2;
_LL60: if(Cyc_Tcutil_is_bound_one(_tmp95)){void*_tmp9B=r;struct Cyc_CfFlowInfo_Place*
_tmp9C;void*_tmp9D;_LL62: if((int)_tmp9B != 1)goto _LL64;_LL63: goto _LL65;_LL64: if((
int)_tmp9B != 2)goto _LL66;_LL65:{void*_tmp9E=(void*)e->annot;struct Cyc_List_List*
_tmp9F;_LL6F: if(*((void**)_tmp9E)!= Cyc_CfFlowInfo_NotZero)goto _LL71;_tmp9F=((
struct Cyc_CfFlowInfo_NotZero_struct*)_tmp9E)->f1;_LL70: if(!Cyc_CfFlowInfo_same_relns(
_tmp99,_tmp9F))goto _LL72;goto _LL6E;_LL71:;_LL72:{void*_tmpA0=(void*)e->r;_LL74:
if(*((int*)_tmpA0)!= 25)goto _LL76;_LL75:{struct Cyc_CfFlowInfo_NotZero_struct
_tmp61E;struct Cyc_CfFlowInfo_NotZero_struct*_tmp61D;(void*)(e->annot=(void*)((
void*)((_tmp61D=_cycalloc(sizeof(*_tmp61D)),((_tmp61D[0]=((_tmp61E.tag=Cyc_CfFlowInfo_NotZero,((
_tmp61E.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp99),_tmp61E)))),
_tmp61D))))));}goto _LL73;_LL76:;_LL77:(void*)(e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_notzero_v));
goto _LL73;_LL73:;}goto _LL6E;_LL6E:;}goto _LL61;_LL66: if(_tmp9B <= (void*)3)goto
_LL68;if(*((int*)_tmp9B)!= 2)goto _LL68;_tmp9C=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp9B)->f1;_LL67:{void*_tmpA3=(void*)e->annot;struct Cyc_List_List*_tmpA4;_LL79:
if(*((void**)_tmpA3)!= Cyc_CfFlowInfo_NotZero)goto _LL7B;_tmpA4=((struct Cyc_CfFlowInfo_NotZero_struct*)
_tmpA3)->f1;_LL7A: if(!Cyc_CfFlowInfo_same_relns(_tmp99,_tmpA4))goto _LL7C;goto
_LL78;_LL7B:;_LL7C:{void*_tmpA5=(void*)e->r;_LL7E: if(*((int*)_tmpA5)!= 25)goto
_LL80;_LL7F:{struct Cyc_CfFlowInfo_NotZero_struct _tmp621;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp620;(void*)(e->annot=(void*)((void*)((_tmp620=_cycalloc(sizeof(*_tmp620)),((
_tmp620[0]=((_tmp621.tag=Cyc_CfFlowInfo_NotZero,((_tmp621.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp99),_tmp621)))),_tmp620))))));}goto _LL7D;_LL80:;_LL81:(
void*)(e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_notzero_v));goto _LL7D;
_LL7D:;}goto _LL78;_LL78:;}{struct _tuple5 _tmp622;return(_tmp622.f1=f,((_tmp622.f2=
Cyc_CfFlowInfo_lookup_place(_tmp98,_tmp9C),_tmp622)));}_LL68: if((int)_tmp9B != 0)
goto _LL6A;_LL69:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));{union
Cyc_CfFlowInfo_FlowInfo_union _tmp625;struct _tuple5 _tmp624;return(_tmp624.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp625.BottomFL).tag=0,_tmp625)),((
_tmp624.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp93,(void*)(env->fenv)->unknown_all),
_tmp624)));}_LL6A: if(_tmp9B <= (void*)3)goto _LL6C;if(*((int*)_tmp9B)!= 0)goto
_LL6C;_tmp9D=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp9B)->f1;_LL6B: f=
Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp9D);goto _LL6D;_LL6C:;_LL6D:{void*
_tmpAB=(void*)e->r;_LL83: if(*((int*)_tmpAB)!= 25)goto _LL85;_LL84:{struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp628;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp627;(void*)(e->annot=(void*)((
void*)((_tmp627=_cycalloc(sizeof(*_tmp627)),((_tmp627[0]=((_tmp628.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp628.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp99),_tmp628)))),
_tmp627))))));}goto _LL82;_LL85:;_LL86:(void*)(e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_unknownz_v));
goto _LL82;_LL82:;}goto _LL61;_LL61:;}else{void*_tmpAE=(void*)e->annot;struct Cyc_List_List*
_tmpAF;_LL88: if(*((void**)_tmpAE)!= Cyc_CfFlowInfo_UnknownZ)goto _LL8A;_tmpAF=((
struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmpAE)->f1;_LL89: if(!Cyc_CfFlowInfo_same_relns(
_tmp99,_tmpAF))goto _LL8B;goto _LL87;_LL8A:;_LL8B:{void*_tmpB0=(void*)e->r;_LL8D:
if(*((int*)_tmpB0)!= 25)goto _LL8F;_LL8E:{struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp62B;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp62A;(void*)(e->annot=(void*)((
void*)((_tmp62A=_cycalloc(sizeof(*_tmp62A)),((_tmp62A[0]=((_tmp62B.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp62B.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp99),_tmp62B)))),
_tmp62A))))));}goto _LL8C;_LL8F:;_LL90:(void*)(e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_unknownz_v));
goto _LL8C;_LL8C:;}goto _LL87;_LL87:;}{void*_tmpB3=Cyc_CfFlowInfo_initlevel(_tmp98,
r);_LL92: if((int)_tmpB3 != 0)goto _LL94;_LL93:{const char*_tmp62E;void*_tmp62D;(
_tmp62D=0,Cyc_Tcutil_terr(e->loc,((_tmp62E="dereference of possibly uninitialized pointer",
_tag_dynforward(_tmp62E,sizeof(char),_get_zero_arr_size_char(_tmp62E,46)))),
_tag_dynforward(_tmp62D,sizeof(void*),0)));}goto _LL95;_LL94: if((int)_tmpB3 != 2)
goto _LL96;_LL95: {struct _tuple5 _tmp62F;return(_tmp62F.f1=f,((_tmp62F.f2=Cyc_CfFlowInfo_typ_to_absrval(
env->fenv,_tmp93,(void*)(env->fenv)->unknown_all),_tmp62F)));}_LL96: if((int)
_tmpB3 != 1)goto _LL91;_LL97: {struct _tuple5 _tmp630;return(_tmp630.f1=f,((_tmp630.f2=
Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmp93,(void*)(env->fenv)->unknown_none),
_tmp630)));}_LL91:;}_LL5C:;}_LL5A:;_LL5B: {struct Cyc_Core_Impossible_struct
_tmp636;const char*_tmp635;struct Cyc_Core_Impossible_struct*_tmp634;(int)_throw((
void*)((_tmp634=_cycalloc(sizeof(*_tmp634)),((_tmp634[0]=((_tmp636.tag=Cyc_Core_Impossible,((
_tmp636.f1=((_tmp635="right deref of non-pointer-type",_tag_dynforward(_tmp635,
sizeof(char),_get_zero_arr_size_char(_tmp635,32)))),_tmp636)))),_tmp634)))));}
_LL57:;}static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct
_RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2);static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct
_RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2){void*_tmpBB=(void*)e1->r;void*_tmpBC;struct Cyc_Absyn_Vardecl*_tmpBD;void*
_tmpBE;struct Cyc_Absyn_Vardecl*_tmpBF;void*_tmpC0;struct Cyc_Absyn_Vardecl*_tmpC1;
void*_tmpC2;struct Cyc_Absyn_Vardecl*_tmpC3;_LL99: if(*((int*)_tmpBB)!= 1)goto
_LL9B;_tmpBC=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBB)->f2;if(_tmpBC <= (
void*)1)goto _LL9B;if(*((int*)_tmpBC)!= 4)goto _LL9B;_tmpBD=((struct Cyc_Absyn_Pat_b_struct*)
_tmpBC)->f1;_LL9A: _tmpBF=_tmpBD;goto _LL9C;_LL9B: if(*((int*)_tmpBB)!= 1)goto _LL9D;
_tmpBE=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBB)->f2;if(_tmpBE <= (void*)1)
goto _LL9D;if(*((int*)_tmpBE)!= 3)goto _LL9D;_tmpBF=((struct Cyc_Absyn_Local_b_struct*)
_tmpBE)->f1;_LL9C: _tmpC1=_tmpBF;goto _LL9E;_LL9D: if(*((int*)_tmpBB)!= 1)goto _LL9F;
_tmpC0=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBB)->f2;if(_tmpC0 <= (void*)1)
goto _LL9F;if(*((int*)_tmpC0)!= 2)goto _LL9F;_tmpC1=((struct Cyc_Absyn_Param_b_struct*)
_tmpC0)->f1;_LL9E: _tmpC3=_tmpC1;goto _LLA0;_LL9F: if(*((int*)_tmpBB)!= 1)goto _LLA1;
_tmpC2=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpBB)->f2;if(_tmpC2 <= (void*)1)
goto _LLA1;if(*((int*)_tmpC2)!= 0)goto _LLA1;_tmpC3=((struct Cyc_Absyn_Global_b_struct*)
_tmpC2)->f1;_LLA0: if(!_tmpC3->escapes){void*_tmpC4=(void*)e2->r;void*_tmpC5;
struct Cyc_Absyn_Vardecl*_tmpC6;void*_tmpC7;struct Cyc_Absyn_Vardecl*_tmpC8;void*
_tmpC9;struct Cyc_Absyn_Vardecl*_tmpCA;void*_tmpCB;struct Cyc_Absyn_Vardecl*_tmpCC;
_LLA4: if(*((int*)_tmpC4)!= 1)goto _LLA6;_tmpC5=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC4)->f2;if(_tmpC5 <= (void*)1)goto _LLA6;if(*((int*)_tmpC5)!= 4)goto _LLA6;
_tmpC6=((struct Cyc_Absyn_Pat_b_struct*)_tmpC5)->f1;_LLA5: _tmpC8=_tmpC6;goto _LLA7;
_LLA6: if(*((int*)_tmpC4)!= 1)goto _LLA8;_tmpC7=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC4)->f2;if(_tmpC7 <= (void*)1)goto _LLA8;if(*((int*)_tmpC7)!= 3)goto _LLA8;
_tmpC8=((struct Cyc_Absyn_Local_b_struct*)_tmpC7)->f1;_LLA7: _tmpCA=_tmpC8;goto
_LLA9;_LLA8: if(*((int*)_tmpC4)!= 1)goto _LLAA;_tmpC9=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC4)->f2;if(_tmpC9 <= (void*)1)goto _LLAA;if(*((int*)_tmpC9)!= 2)goto _LLAA;
_tmpCA=((struct Cyc_Absyn_Param_b_struct*)_tmpC9)->f1;_LLA9: _tmpCC=_tmpCA;goto
_LLAB;_LLAA: if(*((int*)_tmpC4)!= 1)goto _LLAC;_tmpCB=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC4)->f2;if(_tmpCB <= (void*)1)goto _LLAC;if(*((int*)_tmpCB)!= 0)goto _LLAC;
_tmpCC=((struct Cyc_Absyn_Global_b_struct*)_tmpCB)->f1;_LLAB: if(!_tmpCC->escapes){
int found=0;{struct Cyc_List_List*_tmpCD=relns;for(0;_tmpCD != 0;_tmpCD=_tmpCD->tl){
struct Cyc_CfFlowInfo_Reln*_tmpCE=(struct Cyc_CfFlowInfo_Reln*)_tmpCD->hd;if(
_tmpCE->vd == _tmpCC){union Cyc_CfFlowInfo_RelnOp_union _tmpCF=_tmpCE->rop;struct
Cyc_Absyn_Vardecl*_tmpD0;_LLAF: if((_tmpCF.LessSize).tag != 2)goto _LLB1;_tmpD0=(
_tmpCF.LessSize).f1;if(!(_tmpD0 == _tmpC3))goto _LLB1;_LLB0: return relns;_LLB1:;
_LLB2: continue;_LLAE:;}}}if(!found){struct Cyc_CfFlowInfo_Reln*_tmp63C;union Cyc_CfFlowInfo_RelnOp_union
_tmp63B;struct Cyc_List_List*_tmp63A;return(_tmp63A=_region_malloc(rgn,sizeof(*
_tmp63A)),((_tmp63A->hd=((_tmp63C=_region_malloc(rgn,sizeof(*_tmp63C)),((_tmp63C->vd=
_tmpCC,((_tmp63C->rop=(union Cyc_CfFlowInfo_RelnOp_union)(((_tmp63B.LessSize).tag=
2,(((_tmp63B.LessSize).f1=_tmpC3,_tmp63B)))),_tmp63C)))))),((_tmp63A->tl=relns,
_tmp63A)))));}}return relns;_LLAC:;_LLAD: return relns;_LLA3:;}else{return relns;}
_LLA1:;_LLA2: return relns;_LL98:;}static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_NewControlFlow_may_consume_place(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,
struct Cyc_CfFlowInfo_Place*place,void*t,struct Cyc_List_List**ps);static struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_NewControlFlow_may_consume_place(struct Cyc_NewControlFlow_AnalEnv*env,struct
Cyc_CfFlowInfo_ConsumeInfo cinfo,struct Cyc_CfFlowInfo_Place*place,void*t,struct
Cyc_List_List**ps){struct Cyc_CfFlowInfo_FlowEnv*_tmpD4=env->fenv;struct Cyc_List_List*
_tmpD5=Cyc_NewControlFlow_noalias_ptrs_rec(env,place,t);if(ps != 0)*ps=_tmpD5;if(
_tmpD5 == 0)return cinfo;cinfo.may_consume=((struct Cyc_List_List*(*)(struct
_RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(_tmpD4->r,Cyc_CfFlowInfo_place_cmp,
_tmpD5,cinfo.may_consume);return cinfo;}struct _tuple12{union Cyc_CfFlowInfo_AbsLVal_union
f1;union Cyc_CfFlowInfo_FlowInfo_union f2;};static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_may_consume_exp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_Absyn_Exp*e);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_may_consume_exp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_noalias_path(e)){struct Cyc_CfFlowInfo_FlowEnv*
_tmpD6=env->fenv;union Cyc_CfFlowInfo_AbsLVal_union _tmpD8;struct _tuple7 _tmpD7=Cyc_NewControlFlow_anal_Lexp(
env,inflow,e);_tmpD8=_tmpD7.f2;{struct _tuple12 _tmp63D;struct _tuple12 _tmpDA=(
_tmp63D.f1=_tmpD8,((_tmp63D.f2=inflow,_tmp63D)));union Cyc_CfFlowInfo_AbsLVal_union
_tmpDB;struct Cyc_CfFlowInfo_Place*_tmpDC;union Cyc_CfFlowInfo_FlowInfo_union
_tmpDD;struct Cyc_Dict_Dict _tmpDE;struct Cyc_List_List*_tmpDF;struct Cyc_CfFlowInfo_ConsumeInfo
_tmpE0;_LLB4: _tmpDB=_tmpDA.f1;if(((_tmpDA.f1).PlaceL).tag != 0)goto _LLB6;_tmpDC=(
_tmpDB.PlaceL).f1;_tmpDD=_tmpDA.f2;if(((_tmpDA.f2).ReachableFL).tag != 1)goto
_LLB6;_tmpDE=(_tmpDD.ReachableFL).f1;_tmpDF=(_tmpDD.ReachableFL).f2;_tmpE0=(
_tmpDD.ReachableFL).f3;_LLB5: {struct Cyc_CfFlowInfo_ConsumeInfo _tmpE1=Cyc_NewControlFlow_may_consume_place(
env,_tmpE0,_tmpDC,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0);union
Cyc_CfFlowInfo_FlowInfo_union _tmp63E;return(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp63E.ReachableFL).tag=1,(((_tmp63E.ReachableFL).f1=_tmpDE,(((_tmp63E.ReachableFL).f2=
_tmpDF,(((_tmp63E.ReachableFL).f3=_tmpE1,_tmp63E))))))));}_LLB6:;_LLB7:{const
char*_tmp642;void*_tmp641[1];struct Cyc_String_pa_struct _tmp640;(_tmp640.tag=0,((
_tmp640.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e)),((_tmp641[0]=& _tmp640,Cyc_fprintf(Cyc_stderr,((_tmp642="Oops---no location for noalias_path |%s|\n",
_tag_dynforward(_tmp642,sizeof(char),_get_zero_arr_size_char(_tmp642,42)))),
_tag_dynforward(_tmp641,sizeof(void*),1)))))));}return inflow;_LLB3:;}}return
inflow;}static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_NewControlFlow_consume_place(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,struct Cyc_CfFlowInfo_ConsumeInfo
cinfo,struct Cyc_Dict_Dict outdict,struct Cyc_Position_Segment*loc);static struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_NewControlFlow_consume_place(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*
p,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,struct Cyc_Dict_Dict outdict,struct Cyc_Position_Segment*
loc){if(!((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(
cinfo.consumed,p)){struct Cyc_CfFlowInfo_Place _tmpE7;void*_tmpE8;struct Cyc_List_List*
_tmpE9;struct Cyc_CfFlowInfo_Place*_tmpE6=p;_tmpE7=*_tmpE6;_tmpE8=(void*)_tmpE7.root;
_tmpE9=_tmpE7.fields;{void*rval=(void*)0;if((Cyc_Dict_lookup_bool(outdict,_tmpE8,&
rval) && Cyc_CfFlowInfo_initlevel(outdict,rval)!= (void*)0) && rval != (void*)0){
void*_tmpEA=_tmpE8;struct Cyc_Absyn_Vardecl*_tmpEB;_LLB9: if(*((int*)_tmpEA)!= 0)
goto _LLBB;_tmpEB=((struct Cyc_CfFlowInfo_VarRoot_struct*)_tmpEA)->f1;_LLBA: {
struct _dynforward_ptr _tmpEC=Cyc_Absynpp_qvar2string(_tmpEB->name);if(_tmpE9 == 0){
const char*_tmp646;void*_tmp645[1];struct Cyc_String_pa_struct _tmp644;(_tmp644.tag=
0,((_tmp644.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmpEC),((_tmp645[
0]=& _tmp644,Cyc_Tcutil_warn(loc,((_tmp646="may clobber unique pointer %s",
_tag_dynforward(_tmp646,sizeof(char),_get_zero_arr_size_char(_tmp646,30)))),
_tag_dynforward(_tmp645,sizeof(void*),1)))))));}else{const char*_tmp64A;void*
_tmp649[1];struct Cyc_String_pa_struct _tmp648;(_tmp648.tag=0,((_tmp648.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmpEC),((_tmp649[0]=& _tmp648,Cyc_Tcutil_warn(
loc,((_tmp64A="may clobber unique pointer contained in %s",_tag_dynforward(
_tmp64A,sizeof(char),_get_zero_arr_size_char(_tmp64A,43)))),_tag_dynforward(
_tmp649,sizeof(void*),1)))))));}goto _LLB8;}_LLBB:;_LLBC: {const char*_tmp64D;void*
_tmp64C;(_tmp64C=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(((_tmp64D="warning locations not for VarRoots",_tag_dynforward(
_tmp64D,sizeof(char),_get_zero_arr_size_char(_tmp64D,35)))),_tag_dynforward(
_tmp64C,sizeof(void*),0)));}_LLB8:;}}}if(env->all_changed == 0)cinfo.consumed=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))
Cyc_Dict_rdelete)((env->fenv)->r,cinfo.consumed,p);return cinfo;}static struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_NewControlFlow_consume_assignment(struct Cyc_NewControlFlow_AnalEnv*env,
struct Cyc_CfFlowInfo_Place*p,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,struct Cyc_Dict_Dict
outdict,struct Cyc_Absyn_Exp*e);static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_NewControlFlow_consume_assignment(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,struct Cyc_CfFlowInfo_ConsumeInfo
cinfo,struct Cyc_Dict_Dict outdict,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_noalias_path(
e)){struct Cyc_CfFlowInfo_FlowEnv*_tmpF5=env->fenv;struct Cyc_List_List*_tmpF6=0;
cinfo=Cyc_NewControlFlow_may_consume_place(env,cinfo,p,(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,(struct Cyc_List_List**)& _tmpF6);while(_tmpF6 != 0){cinfo=
Cyc_NewControlFlow_consume_place(env,(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)
_check_null(_tmpF6))->hd,cinfo,outdict,e->loc);_tmpF6=((struct Cyc_List_List*)
_check_null(_tmpF6))->tl;}}return cinfo;}static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_aux(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*
ts);static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_aux(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*ts){struct Cyc_List_List*l=0;
struct Cyc_CfFlowInfo_Place _tmpF8;void*_tmpF9;struct Cyc_List_List*_tmpFA;struct
Cyc_CfFlowInfo_Place*_tmpF7=p;_tmpF8=*_tmpF7;_tmpF9=(void*)_tmpF8.root;_tmpFA=
_tmpF8.fields;{int fld=0;for(0;ts != 0;(fld ++,ts=ts->tl)){void*_tmpFB=(void*)ts->hd;
if(Cyc_Tcutil_is_noalias_pointer(_tmpFB)){int _tmp64E[1];struct Cyc_List_List*
_tmpFC=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_rappend)(env->r,_tmpFA,((_tmp64E[0]=fld,((struct
Cyc_List_List*(*)(struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(
env->r,_tag_dynforward(_tmp64E,sizeof(int),1)))));struct Cyc_CfFlowInfo_Place*
_tmp651;struct Cyc_List_List*_tmp650;l=((struct Cyc_List_List*(*)(struct
_RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(env->r,Cyc_CfFlowInfo_place_cmp,((
_tmp650=_region_malloc(env->r,sizeof(*_tmp650)),((_tmp650->hd=((_tmp651=
_region_malloc(env->r,sizeof(*_tmp651)),((_tmp651->root=(void*)_tmpF9,((_tmp651->fields=
_tmpFC,_tmp651)))))),((_tmp650->tl=0,_tmp650)))))),l);}else{if(Cyc_Absyn_is_aggr_type(
_tmpFB)){int _tmp652[1];struct Cyc_List_List*_tmp100=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
env->r,_tmpFA,((_tmp652[0]=fld,((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dynforward_ptr))Cyc_List_rlist)(env->r,_tag_dynforward(_tmp652,sizeof(int),
1)))));struct Cyc_CfFlowInfo_Place*_tmp653;struct Cyc_CfFlowInfo_Place*_tmp101=(
_tmp653=_region_malloc(env->r,sizeof(*_tmp653)),((_tmp653->root=(void*)_tmpF9,((
_tmp653->fields=_tmp100,_tmp653)))));l=((struct Cyc_List_List*(*)(struct
_RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),
struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(env->r,Cyc_CfFlowInfo_place_cmp,
l,Cyc_NewControlFlow_noalias_ptrs_rec(env,_tmp101,_tmpFB));}}}}return l;}struct
_tuple13{struct Cyc_Absyn_Tqual f1;void*f2;};static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,void*t);static
struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_CfFlowInfo_Place*p,void*t){if(Cyc_Tcutil_is_noalias_pointer(t)){
struct Cyc_List_List*_tmp654;return(_tmp654=_region_malloc(env->r,sizeof(*_tmp654)),((
_tmp654->hd=p,((_tmp654->tl=0,_tmp654)))));}{void*_tmp105=Cyc_Tcutil_compress(t);
struct Cyc_List_List*_tmp106;struct Cyc_List_List*_tmp107;struct Cyc_Absyn_AggrInfo
_tmp108;union Cyc_Absyn_AggrInfoU_union _tmp109;struct Cyc_Absyn_Aggrdecl**_tmp10A;
struct Cyc_List_List*_tmp10B;struct Cyc_Absyn_AggrInfo _tmp10C;union Cyc_Absyn_AggrInfoU_union
_tmp10D;struct Cyc_Absyn_TunionFieldInfo _tmp10E;union Cyc_Absyn_TunionFieldInfoU_union
_tmp10F;struct Cyc_List_List*_tmp110;_LLBE: if(_tmp105 <= (void*)4)goto _LLCA;if(*((
int*)_tmp105)!= 9)goto _LLC0;_tmp106=((struct Cyc_Absyn_TupleType_struct*)_tmp105)->f1;
_LLBF: {struct Cyc_List_List*_tmp111=0;while(_tmp106 != 0){{struct Cyc_List_List*
_tmp655;_tmp111=((_tmp655=_region_malloc(env->r,sizeof(*_tmp655)),((_tmp655->hd=(
void*)(*((struct _tuple13*)_tmp106->hd)).f2,((_tmp655->tl=_tmp111,_tmp655))))));}
_tmp106=_tmp106->tl;}_tmp111=Cyc_List_imp_rev(_tmp111);return Cyc_NewControlFlow_noalias_ptrs_aux(
env,p,_tmp111);}_LLC0: if(*((int*)_tmp105)!= 11)goto _LLC2;_tmp107=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp105)->f2;_LLC1: {struct Cyc_List_List*_tmp113=0;while(_tmp107 != 0){{struct Cyc_List_List*
_tmp656;_tmp113=((_tmp656=_region_malloc(env->r,sizeof(*_tmp656)),((_tmp656->hd=(
void*)((void*)((struct Cyc_Absyn_Aggrfield*)_tmp107->hd)->type),((_tmp656->tl=
_tmp113,_tmp656))))));}_tmp107=_tmp107->tl;}_tmp113=Cyc_List_imp_rev(_tmp113);
return Cyc_NewControlFlow_noalias_ptrs_aux(env,p,_tmp113);}_LLC2: if(*((int*)
_tmp105)!= 10)goto _LLC4;_tmp108=((struct Cyc_Absyn_AggrType_struct*)_tmp105)->f1;
_tmp109=_tmp108.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp105)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LLC4;_tmp10A=(_tmp109.KnownAggr).f1;_tmp10B=_tmp108.targs;_LLC3: if((*
_tmp10A)->impl == 0)return 0;else{struct Cyc_List_List*_tmp115=0;{struct
_RegionHandle _tmp116=_new_region("rgn");struct _RegionHandle*rgn=& _tmp116;
_push_region(rgn);{struct Cyc_List_List*_tmp117=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmp10A)->tvs,_tmp10B);struct Cyc_List_List*_tmp118=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp10A)->impl))->fields;while(
_tmp118 != 0){{struct Cyc_List_List*_tmp657;_tmp115=((_tmp657=_region_malloc(env->r,
sizeof(*_tmp657)),((_tmp657->hd=(void*)Cyc_Tcutil_rsubstitute(rgn,_tmp117,(void*)((
struct Cyc_Absyn_Aggrfield*)_tmp118->hd)->type),((_tmp657->tl=_tmp115,_tmp657))))));}
_tmp118=_tmp118->tl;}};_pop_region(rgn);}_tmp115=Cyc_List_imp_rev(_tmp115);
return Cyc_NewControlFlow_noalias_ptrs_aux(env,p,_tmp115);}_LLC4: if(*((int*)
_tmp105)!= 10)goto _LLC6;_tmp10C=((struct Cyc_Absyn_AggrType_struct*)_tmp105)->f1;
_tmp10D=_tmp10C.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp105)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LLC6;_LLC5: {const char*_tmp65A;void*_tmp659;(_tmp659=0,((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp65A="got unknown aggr in noalias_ptrs_rec",
_tag_dynforward(_tmp65A,sizeof(char),_get_zero_arr_size_char(_tmp65A,37)))),
_tag_dynforward(_tmp659,sizeof(void*),0)));}_LLC6: if(*((int*)_tmp105)!= 2)goto
_LLC8;_LLC7: if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t)){struct Cyc_List_List*
_tmp65B;return(_tmp65B=_region_malloc(env->r,sizeof(*_tmp65B)),((_tmp65B->hd=p,((
_tmp65B->tl=0,_tmp65B)))));}else{return 0;}_LLC8: if(*((int*)_tmp105)!= 3)goto
_LLCA;_tmp10E=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp105)->f1;_tmp10F=
_tmp10E.field_info;_tmp110=_tmp10E.targs;_LLC9: {union Cyc_Absyn_TunionFieldInfoU_union
_tmp11D=_tmp10F;struct Cyc_Absyn_Tuniondecl*_tmp11E;struct Cyc_Absyn_Tunionfield*
_tmp11F;_LLCD: if((_tmp11D.UnknownTunionfield).tag != 0)goto _LLCF;_LLCE: {const
char*_tmp65E;void*_tmp65D;(_tmp65D=0,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp65E="got unknown tunion field in noalias_ptrs_rec",
_tag_dynforward(_tmp65E,sizeof(char),_get_zero_arr_size_char(_tmp65E,45)))),
_tag_dynforward(_tmp65D,sizeof(void*),0)));}_LLCF: if((_tmp11D.KnownTunionfield).tag
!= 1)goto _LLCC;_tmp11E=(_tmp11D.KnownTunionfield).f1;_tmp11F=(_tmp11D.KnownTunionfield).f2;
_LLD0: {struct Cyc_List_List*_tmp122=0;{struct _RegionHandle _tmp123=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp123;_push_region(rgn);{struct Cyc_List_List*_tmp124=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp11E->tvs,_tmp110);struct Cyc_List_List*
_tmp125=_tmp11F->typs;while(_tmp125 != 0){{struct Cyc_List_List*_tmp65F;_tmp122=((
_tmp65F=_region_malloc(env->r,sizeof(*_tmp65F)),((_tmp65F->hd=(void*)Cyc_Tcutil_rsubstitute(
rgn,_tmp124,(*((struct _tuple13*)_tmp125->hd)).f2),((_tmp65F->tl=_tmp122,_tmp65F))))));}
_tmp125=_tmp125->tl;}};_pop_region(rgn);}_tmp122=Cyc_List_imp_rev(_tmp122);
return Cyc_NewControlFlow_noalias_ptrs_aux(env,p,_tmp122);}_LLCC:;}_LLCA:;_LLCB:
return 0;_LLBD:;}}struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
static struct _tuple5 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e);static void
_tmp6F7(unsigned int*_tmp6F6,unsigned int*_tmp6F5,void***_tmp6F3,struct Cyc_List_List**
_tmp2B4){for(*_tmp6F6=0;*_tmp6F6 < *_tmp6F5;(*_tmp6F6)++){void*_tmp6F1;(*_tmp6F3)[*
_tmp6F6]=((_tmp6F1=(void*)((struct Cyc_List_List*)_check_null(*_tmp2B4))->hd,((*
_tmp2B4=(*_tmp2B4)->tl,_tmp6F1))));}}static struct _tuple5 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*_tmp127=env->fenv;struct Cyc_Dict_Dict
d;struct Cyc_List_List*relns;struct Cyc_CfFlowInfo_ConsumeInfo cinfo;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp128=inflow;struct Cyc_Dict_Dict _tmp129;struct Cyc_List_List*_tmp12A;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp12B;_LLD2: if((_tmp128.BottomFL).tag != 0)goto _LLD4;_LLD3: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp662;struct _tuple5 _tmp661;return(_tmp661.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp662.BottomFL).tag=0,_tmp662)),((_tmp661.f2=(void*)_tmp127->unknown_all,
_tmp661)));}_LLD4: if((_tmp128.ReachableFL).tag != 1)goto _LLD1;_tmp129=(_tmp128.ReachableFL).f1;
_tmp12A=(_tmp128.ReachableFL).f2;_tmp12B=(_tmp128.ReachableFL).f3;_LLD5: d=
_tmp129;relns=_tmp12A;cinfo=_tmp12B;_LLD1:;}{void*_tmp12E=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp12F;void*_tmp130;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Absyn_Exp*_tmp132;
struct Cyc_Absyn_Exp*_tmp133;union Cyc_Absyn_Cnst_union _tmp134;union Cyc_Absyn_Cnst_union
_tmp135;int _tmp136;union Cyc_Absyn_Cnst_union _tmp137;void*_tmp138;struct Cyc_List_List*
_tmp139;void*_tmp13A;void*_tmp13B;struct Cyc_Absyn_Vardecl*_tmp13C;void*_tmp13D;
struct Cyc_Absyn_Vardecl*_tmp13E;void*_tmp13F;struct Cyc_Absyn_Vardecl*_tmp140;
void*_tmp141;struct Cyc_List_List*_tmp142;struct Cyc_Absyn_Exp*_tmp143;struct Cyc_Absyn_Exp*
_tmp144;struct Cyc_Core_Opt*_tmp145;struct Cyc_Core_Opt _tmp146;struct Cyc_Absyn_Exp*
_tmp147;struct Cyc_Absyn_Exp*_tmp148;struct Cyc_Core_Opt*_tmp149;struct Cyc_Absyn_Exp*
_tmp14A;struct Cyc_Absyn_Exp*_tmp14B;struct Cyc_Absyn_Exp*_tmp14C;struct Cyc_Absyn_Exp*
_tmp14D;struct Cyc_Absyn_Exp*_tmp14E;struct Cyc_List_List*_tmp14F;struct Cyc_Absyn_MallocInfo
_tmp150;int _tmp151;struct Cyc_Absyn_Exp*_tmp152;void**_tmp153;struct Cyc_Absyn_Exp*
_tmp154;int _tmp155;struct Cyc_Absyn_Exp*_tmp156;struct Cyc_Absyn_Exp*_tmp157;
struct Cyc_Absyn_Exp*_tmp158;struct Cyc_Absyn_Exp*_tmp159;struct Cyc_Absyn_Exp*
_tmp15A;struct Cyc_Absyn_Exp*_tmp15B;struct Cyc_Absyn_Exp*_tmp15C;struct
_dynforward_ptr*_tmp15D;struct Cyc_Absyn_Exp*_tmp15E;struct _dynforward_ptr*
_tmp15F;struct Cyc_Absyn_Exp*_tmp160;struct Cyc_Absyn_Exp*_tmp161;struct Cyc_Absyn_Exp*
_tmp162;struct Cyc_Absyn_Exp*_tmp163;struct Cyc_Absyn_Exp*_tmp164;struct Cyc_Absyn_Exp*
_tmp165;struct Cyc_Absyn_Exp*_tmp166;struct Cyc_Absyn_Exp*_tmp167;struct Cyc_Absyn_Exp*
_tmp168;struct Cyc_List_List*_tmp169;struct Cyc_Absyn_Tuniondecl*_tmp16A;struct Cyc_List_List*
_tmp16B;struct Cyc_List_List*_tmp16C;struct Cyc_List_List*_tmp16D;struct Cyc_Absyn_Aggrdecl*
_tmp16E;struct Cyc_Absyn_Aggrdecl _tmp16F;struct Cyc_Absyn_AggrdeclImpl*_tmp170;
struct Cyc_Absyn_AggrdeclImpl _tmp171;struct Cyc_List_List*_tmp172;struct Cyc_List_List*
_tmp173;struct Cyc_Absyn_Vardecl*_tmp174;struct Cyc_Absyn_Exp*_tmp175;struct Cyc_Absyn_Exp*
_tmp176;int _tmp177;struct Cyc_Absyn_Stmt*_tmp178;void*_tmp179;_LLD7: if(*((int*)
_tmp12E)!= 15)goto _LLD9;_tmp12F=((struct Cyc_Absyn_Cast_e_struct*)_tmp12E)->f2;
_tmp130=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp12E)->f4;if((int)_tmp130 != 2)
goto _LLD9;_LLD8: {union Cyc_CfFlowInfo_FlowInfo_union _tmp17B;void*_tmp17C;struct
_tuple5 _tmp17A=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp12F);_tmp17B=_tmp17A.f1;
_tmp17C=_tmp17A.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp17E;void*_tmp17F;
struct _tuple5 _tmp17D=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp17B,_tmp12F,
_tmp17C);_tmp17E=_tmp17D.f1;_tmp17F=_tmp17D.f2;{struct _tuple5 _tmp663;return(
_tmp663.f1=_tmp17E,((_tmp663.f2=_tmp17C,_tmp663)));}}}_LLD9: if(*((int*)_tmp12E)
!= 15)goto _LLDB;_tmp131=((struct Cyc_Absyn_Cast_e_struct*)_tmp12E)->f2;_LLDA:
_tmp132=_tmp131;goto _LLDC;_LLDB: if(*((int*)_tmp12E)!= 13)goto _LLDD;_tmp132=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmp12E)->f1;_LLDC: _tmp133=_tmp132;goto
_LLDE;_LLDD: if(*((int*)_tmp12E)!= 14)goto _LLDF;_tmp133=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmp12E)->f1;_LLDE: return Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp133);_LLDF:
if(*((int*)_tmp12E)!= 0)goto _LLE1;_tmp134=((struct Cyc_Absyn_Const_e_struct*)
_tmp12E)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp12E)->f1).Null_c).tag != 
6)goto _LLE1;_LLE0: goto _LLE2;_LLE1: if(*((int*)_tmp12E)!= 0)goto _LLE3;_tmp135=((
struct Cyc_Absyn_Const_e_struct*)_tmp12E)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp12E)->f1).Int_c).tag != 2)goto _LLE3;_tmp136=(_tmp135.Int_c).f2;if(_tmp136 != 0)
goto _LLE3;_LLE2: {struct _tuple5 _tmp664;return(_tmp664.f1=inflow,((_tmp664.f2=(
void*)0,_tmp664)));}_LLE3: if(*((int*)_tmp12E)!= 0)goto _LLE5;_tmp137=((struct Cyc_Absyn_Const_e_struct*)
_tmp12E)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp12E)->f1).Int_c).tag != 2)
goto _LLE5;_LLE4: goto _LLE6;_LLE5: if(*((int*)_tmp12E)!= 1)goto _LLE7;_tmp138=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmp12E)->f2;if(_tmp138 <= (void*)1)goto _LLE7;if(*((
int*)_tmp138)!= 1)goto _LLE7;_LLE6: {struct _tuple5 _tmp665;return(_tmp665.f1=
inflow,((_tmp665.f2=(void*)1,_tmp665)));}_LLE7: if(*((int*)_tmp12E)!= 32)goto
_LLE9;_tmp139=((struct Cyc_Absyn_Tunion_e_struct*)_tmp12E)->f1;if(_tmp139 != 0)
goto _LLE9;_LLE8: goto _LLEA;_LLE9: if(*((int*)_tmp12E)!= 0)goto _LLEB;_LLEA: goto
_LLEC;_LLEB: if(*((int*)_tmp12E)!= 19)goto _LLED;_LLEC: goto _LLEE;_LLED: if(*((int*)
_tmp12E)!= 18)goto _LLEF;_LLEE: goto _LLF0;_LLEF: if(*((int*)_tmp12E)!= 20)goto _LLF1;
_LLF0: goto _LLF2;_LLF1: if(*((int*)_tmp12E)!= 21)goto _LLF3;_LLF2: goto _LLF4;_LLF3:
if(*((int*)_tmp12E)!= 34)goto _LLF5;_LLF4: goto _LLF6;_LLF5: if(*((int*)_tmp12E)!= 
33)goto _LLF7;_LLF6: {struct _tuple5 _tmp666;return(_tmp666.f1=inflow,((_tmp666.f2=(
void*)_tmp127->unknown_all,_tmp666)));}_LLF7: if(*((int*)_tmp12E)!= 1)goto _LLF9;
_tmp13A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp12E)->f2;if(_tmp13A <= (void*)
1)goto _LLF9;if(*((int*)_tmp13A)!= 0)goto _LLF9;_LLF8: {struct _tuple5 _tmp667;
return(_tmp667.f1=inflow,((_tmp667.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp127,(
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp127->unknown_all),
_tmp667)));}_LLF9: if(*((int*)_tmp12E)!= 1)goto _LLFB;_tmp13B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp12E)->f2;if(_tmp13B <= (void*)1)goto _LLFB;if(*((int*)_tmp13B)!= 2)goto _LLFB;
_tmp13C=((struct Cyc_Absyn_Param_b_struct*)_tmp13B)->f1;_LLFA: _tmp13E=_tmp13C;
goto _LLFC;_LLFB: if(*((int*)_tmp12E)!= 1)goto _LLFD;_tmp13D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp12E)->f2;if(_tmp13D <= (void*)1)goto _LLFD;if(*((int*)_tmp13D)!= 3)goto _LLFD;
_tmp13E=((struct Cyc_Absyn_Local_b_struct*)_tmp13D)->f1;_LLFC: _tmp140=_tmp13E;
goto _LLFE;_LLFD: if(*((int*)_tmp12E)!= 1)goto _LLFF;_tmp13F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp12E)->f2;if(_tmp13F <= (void*)1)goto _LLFF;if(*((int*)_tmp13F)!= 4)goto _LLFF;
_tmp140=((struct Cyc_Absyn_Pat_b_struct*)_tmp13F)->f1;_LLFE: inflow=Cyc_NewControlFlow_may_consume_exp(
env,inflow,e);{struct Cyc_CfFlowInfo_VarRoot_struct*_tmp66D;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp66C;struct _tuple5 _tmp66B;return(_tmp66B.f1=inflow,((_tmp66B.f2=Cyc_Dict_lookup(
d,(void*)((_tmp66D=_region_malloc(env->r,sizeof(*_tmp66D)),((_tmp66D[0]=((
_tmp66C.tag=0,((_tmp66C.f1=_tmp140,_tmp66C)))),_tmp66D))))),_tmp66B)));}_LLFF:
if(*((int*)_tmp12E)!= 3)goto _LL101;_tmp141=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp12E)->f1;_tmp142=((struct Cyc_Absyn_Primop_e_struct*)_tmp12E)->f2;_LL100: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp189;void*_tmp18A;struct _tuple5 _tmp188=Cyc_NewControlFlow_anal_use_ints(
env,inflow,_tmp142,0);_tmp189=_tmp188.f1;_tmp18A=_tmp188.f2;{void*_tmp18B=
_tmp141;_LL13E: if((int)_tmp18B != 0)goto _LL140;_LL13F: goto _LL141;_LL140: if((int)
_tmp18B != 2)goto _LL142;_LL141: Cyc_CfFlowInfo_check_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp142))->hd)->loc,_tmp189);goto _LL13D;_LL142:;
_LL143: _tmp189=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp142))->hd)->loc,_tmp189);goto _LL13D;_LL13D:;}{
struct _tuple5 _tmp66E;return(_tmp66E.f1=_tmp189,((_tmp66E.f2=_tmp18A,_tmp66E)));}}
_LL101: if(*((int*)_tmp12E)!= 5)goto _LL103;_tmp143=((struct Cyc_Absyn_Increment_e_struct*)
_tmp12E)->f1;_LL102: {struct Cyc_List_List _tmp66F;struct Cyc_List_List _tmp18D=(
_tmp66F.hd=_tmp143,((_tmp66F.tl=0,_tmp66F)));union Cyc_CfFlowInfo_FlowInfo_union
_tmp18F;struct _tuple5 _tmp18E=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct
Cyc_List_List*)& _tmp18D,0);_tmp18F=_tmp18E.f1;Cyc_CfFlowInfo_check_unique_rvals(
_tmp143->loc,_tmp18F);{union Cyc_CfFlowInfo_AbsLVal_union _tmp191;struct _tuple7
_tmp190=Cyc_NewControlFlow_anal_Lexp(env,_tmp18F,_tmp143);_tmp191=_tmp190.f2;{
struct _tuple12 _tmp670;struct _tuple12 _tmp193=(_tmp670.f1=_tmp191,((_tmp670.f2=
_tmp18F,_tmp670)));union Cyc_CfFlowInfo_AbsLVal_union _tmp194;struct Cyc_CfFlowInfo_Place*
_tmp195;union Cyc_CfFlowInfo_FlowInfo_union _tmp196;struct Cyc_Dict_Dict _tmp197;
struct Cyc_List_List*_tmp198;struct Cyc_CfFlowInfo_ConsumeInfo _tmp199;_LL145:
_tmp194=_tmp193.f1;if(((_tmp193.f1).PlaceL).tag != 0)goto _LL147;_tmp195=(_tmp194.PlaceL).f1;
_tmp196=_tmp193.f2;if(((_tmp193.f2).ReachableFL).tag != 1)goto _LL147;_tmp197=(
_tmp196.ReachableFL).f1;_tmp198=(_tmp196.ReachableFL).f2;_tmp199=(_tmp196.ReachableFL).f3;
_LL146: _tmp198=Cyc_CfFlowInfo_reln_kill_exp(_tmp127->r,_tmp198,_tmp143);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp671;_tmp18F=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp671.ReachableFL).tag=1,(((_tmp671.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
_tmp127,_tmp143->loc,_tmp197,env->all_changed,_tmp195,(void*)_tmp127->unknown_all),(((
_tmp671.ReachableFL).f2=_tmp198,(((_tmp671.ReachableFL).f3=_tmp199,_tmp671))))))));}
goto _LL144;_LL147:;_LL148: goto _LL144;_LL144:;}{struct _tuple5 _tmp672;return(
_tmp672.f1=_tmp18F,((_tmp672.f2=(void*)_tmp127->unknown_all,_tmp672)));}}}_LL103:
if(*((int*)_tmp12E)!= 4)goto _LL105;_tmp144=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp12E)->f1;_tmp145=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp12E)->f2;if(
_tmp145 == 0)goto _LL105;_tmp146=*_tmp145;_tmp147=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp12E)->f3;_LL104: {struct Cyc_List_List _tmp673;struct Cyc_List_List _tmp19D=(
_tmp673.hd=_tmp147,((_tmp673.tl=0,_tmp673)));struct Cyc_List_List _tmp674;struct
Cyc_List_List _tmp19E=(_tmp674.hd=_tmp144,((_tmp674.tl=(struct Cyc_List_List*)&
_tmp19D,_tmp674)));union Cyc_CfFlowInfo_FlowInfo_union _tmp1A0;struct _tuple5
_tmp19F=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct Cyc_List_List*)&
_tmp19E,1);_tmp1A0=_tmp19F.f1;{union Cyc_CfFlowInfo_AbsLVal_union _tmp1A2;struct
_tuple7 _tmp1A1=Cyc_NewControlFlow_anal_Lexp(env,_tmp1A0,_tmp144);_tmp1A2=_tmp1A1.f2;
_tmp1A0=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp1A0);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1A3=_tmp1A0;struct Cyc_Dict_Dict _tmp1A4;struct Cyc_List_List*_tmp1A5;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1A6;_LL14A: if((_tmp1A3.ReachableFL).tag != 1)goto _LL14C;_tmp1A4=(_tmp1A3.ReachableFL).f1;
_tmp1A5=(_tmp1A3.ReachableFL).f2;_tmp1A6=(_tmp1A3.ReachableFL).f3;_LL14B:{union
Cyc_CfFlowInfo_AbsLVal_union _tmp1A7=_tmp1A2;struct Cyc_CfFlowInfo_Place*_tmp1A8;
_LL14F: if((_tmp1A7.PlaceL).tag != 0)goto _LL151;_tmp1A8=(_tmp1A7.PlaceL).f1;_LL150:
_tmp1A6=Cyc_NewControlFlow_consume_assignment(env,_tmp1A8,_tmp1A6,_tmp1A4,
_tmp144);_tmp1A5=Cyc_CfFlowInfo_reln_kill_exp(_tmp127->r,_tmp1A5,_tmp144);
_tmp1A4=Cyc_CfFlowInfo_assign_place(_tmp127,_tmp144->loc,_tmp1A4,env->all_changed,
_tmp1A8,(void*)_tmp127->unknown_all);{union Cyc_CfFlowInfo_FlowInfo_union _tmp675;
_tmp1A0=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp675.ReachableFL).tag=1,(((
_tmp675.ReachableFL).f1=_tmp1A4,(((_tmp675.ReachableFL).f2=_tmp1A5,(((_tmp675.ReachableFL).f3=
_tmp1A6,_tmp675))))))));}goto _LL14E;_LL151: if((_tmp1A7.UnknownL).tag != 1)goto
_LL14E;_LL152: goto _LL14E;_LL14E:;}goto _LL149;_LL14C:;_LL14D: goto _LL149;_LL149:;}{
struct _tuple5 _tmp676;return(_tmp676.f1=_tmp1A0,((_tmp676.f2=(void*)_tmp127->unknown_all,
_tmp676)));}}}_LL105: if(*((int*)_tmp12E)!= 4)goto _LL107;_tmp148=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp12E)->f1;_tmp149=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp12E)->f2;if(
_tmp149 != 0)goto _LL107;_tmp14A=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp12E)->f3;
_LL106: {struct Cyc_Dict_Dict*_tmp1AD=env->all_changed;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1AF;union Cyc_CfFlowInfo_FlowInfo_union _tmp1B0;struct _tuple6 _tmp1AE=Cyc_CfFlowInfo_save_consume_info(
_tmp127,inflow,1);_tmp1AF=_tmp1AE.f1;_tmp1B0=_tmp1AE.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
_tmp677;struct Cyc_CfFlowInfo_ConsumeInfo empty_consume=(_tmp677.consumed=_tmp127->mt_place_set,((
_tmp677.may_consume=0,_tmp677)));struct Cyc_CfFlowInfo_ConsumeInfo outflow_consume=
empty_consume;int init_consume=0;while(1){{struct Cyc_Dict_Dict*_tmp678;env->all_changed=((
_tmp678=_region_malloc(env->r,sizeof(*_tmp678)),((_tmp678[0]=_tmp127->mt_place_set,
_tmp678))));}{union Cyc_CfFlowInfo_FlowInfo_union _tmp1B3;union Cyc_CfFlowInfo_AbsLVal_union
_tmp1B4;struct _tuple7 _tmp1B2=Cyc_NewControlFlow_anal_Lexp(env,_tmp1B0,_tmp148);
_tmp1B3=_tmp1B2.f1;_tmp1B4=_tmp1B2.f2;{struct Cyc_Dict_Dict _tmp1B5=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));{struct Cyc_Dict_Dict*_tmp679;env->all_changed=((
_tmp679=_region_malloc(env->r,sizeof(*_tmp679)),((_tmp679[0]=_tmp127->mt_place_set,
_tmp679))));}{union Cyc_CfFlowInfo_FlowInfo_union _tmp1B8;void*_tmp1B9;struct
_tuple5 _tmp1B7=Cyc_NewControlFlow_anal_Rexp(env,_tmp1B0,_tmp14A);_tmp1B8=_tmp1B7.f1;
_tmp1B9=_tmp1B7.f2;{struct Cyc_Dict_Dict _tmp1BA=*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed));union Cyc_CfFlowInfo_FlowInfo_union _tmp1BB=Cyc_CfFlowInfo_after_flow(
_tmp127,(struct Cyc_Dict_Dict*)& _tmp1B5,_tmp1B3,_tmp1B8,_tmp1B5,_tmp1BA);union Cyc_CfFlowInfo_FlowInfo_union
_tmp1BC=Cyc_CfFlowInfo_join_flow(_tmp127,_tmp1AD,_tmp1B0,_tmp1BB,1);struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1BE;struct _tuple6 _tmp1BD=Cyc_CfFlowInfo_save_consume_info(_tmp127,_tmp1BC,0);
_tmp1BE=_tmp1BD.f1;if(init_consume)_tmp1B0=Cyc_CfFlowInfo_restore_consume_info(
_tmp1B0,outflow_consume);outflow_consume=_tmp1BE;init_consume=1;if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp1BC,_tmp1B0)){if(_tmp1AD == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp1AD,Cyc_CfFlowInfo_union_place_set(
_tmp1B5,_tmp1BA,0),0);}_tmp1BB=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,
_tmp1BB);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1BF=_tmp1BB;struct Cyc_Dict_Dict
_tmp1C0;struct Cyc_List_List*_tmp1C1;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1C2;
_LL154: if((_tmp1BF.BottomFL).tag != 0)goto _LL156;_LL155: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp67C;struct _tuple5 _tmp67B;return(_tmp67B.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp67C.BottomFL).tag=0,_tmp67C)),((_tmp67B.f2=_tmp1B9,_tmp67B)));}_LL156: if((
_tmp1BF.ReachableFL).tag != 1)goto _LL153;_tmp1C0=(_tmp1BF.ReachableFL).f1;_tmp1C1=(
_tmp1BF.ReachableFL).f2;_tmp1C2=(_tmp1BF.ReachableFL).f3;_LL157: _tmp1C2=Cyc_CfFlowInfo_and_consume(
_tmp127->r,_tmp1AF,_tmp1C2);{union Cyc_CfFlowInfo_AbsLVal_union _tmp1C5=_tmp1B4;
struct Cyc_CfFlowInfo_Place*_tmp1C6;_LL159: if((_tmp1C5.PlaceL).tag != 0)goto _LL15B;
_tmp1C6=(_tmp1C5.PlaceL).f1;_LL15A: _tmp1C2=Cyc_NewControlFlow_consume_assignment(
env,_tmp1C6,_tmp1C2,_tmp1C0,_tmp148);_tmp1C0=Cyc_CfFlowInfo_assign_place(_tmp127,
e->loc,_tmp1C0,env->all_changed,_tmp1C6,_tmp1B9);_tmp1C1=Cyc_CfFlowInfo_reln_assign_exp(
_tmp127->r,_tmp1C1,_tmp148,_tmp14A);{union Cyc_CfFlowInfo_FlowInfo_union _tmp67D;
_tmp1BB=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp67D.ReachableFL).tag=1,(((
_tmp67D.ReachableFL).f1=_tmp1C0,(((_tmp67D.ReachableFL).f2=_tmp1C1,(((_tmp67D.ReachableFL).f3=
_tmp1C2,_tmp67D))))))));}Cyc_NewControlFlow_update_tryflow(env,_tmp1BB);{struct
_tuple5 _tmp67E;return(_tmp67E.f1=_tmp1BB,((_tmp67E.f2=_tmp1B9,_tmp67E)));}_LL15B:
if((_tmp1C5.UnknownL).tag != 1)goto _LL158;_LL15C: {struct _tuple5 _tmp67F;return(
_tmp67F.f1=Cyc_NewControlFlow_use_Rval(env,_tmp14A->loc,_tmp1BB,_tmp1B9),((
_tmp67F.f2=_tmp1B9,_tmp67F)));}_LL158:;}_LL153:;}}_tmp1B0=Cyc_CfFlowInfo_restore_consume_info(
_tmp1BC,empty_consume);}}}}}}}_LL107: if(*((int*)_tmp12E)!= 9)goto _LL109;_tmp14B=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmp12E)->f1;_tmp14C=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp12E)->f2;_LL108: {union Cyc_CfFlowInfo_FlowInfo_union _tmp1CC;void*_tmp1CD;
struct _tuple5 _tmp1CB=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp14B);_tmp1CC=
_tmp1CB.f1;_tmp1CD=_tmp1CB.f2;_tmp1CC=Cyc_CfFlowInfo_drop_unique_rvals(_tmp14B->loc,
_tmp1CC);return Cyc_NewControlFlow_anal_Rexp(env,_tmp1CC,_tmp14C);}_LL109: if(*((
int*)_tmp12E)!= 12)goto _LL10B;_tmp14D=((struct Cyc_Absyn_Throw_e_struct*)_tmp12E)->f1;
_LL10A: {union Cyc_CfFlowInfo_FlowInfo_union _tmp1CF;void*_tmp1D0;struct _tuple5
_tmp1CE=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp14D);_tmp1CF=_tmp1CE.f1;
_tmp1D0=_tmp1CE.f2;_tmp1CF=Cyc_CfFlowInfo_consume_unique_rvals(_tmp14D->loc,
_tmp1CF);Cyc_NewControlFlow_use_Rval(env,_tmp14D->loc,_tmp1CF,_tmp1D0);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp682;struct _tuple5 _tmp681;return(_tmp681.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp682.BottomFL).tag=0,_tmp682)),((_tmp681.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp127,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp127->unknown_all),
_tmp681)));}}_LL10B: if(*((int*)_tmp12E)!= 11)goto _LL10D;_tmp14E=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp12E)->f1;_tmp14F=((struct Cyc_Absyn_FnCall_e_struct*)_tmp12E)->f2;_LL10C: {
struct _RegionHandle _tmp1D3=_new_region("temp");struct _RegionHandle*temp=& _tmp1D3;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1D6;struct Cyc_List_List*
_tmp1D7;struct Cyc_List_List*_tmp683;struct _tuple11 _tmp1D5=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,((_tmp683=_region_malloc(temp,sizeof(*_tmp683)),((_tmp683->hd=
_tmp14E,((_tmp683->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*
x))Cyc_List_rcopy)(temp,_tmp14F),_tmp683)))))),1);_tmp1D6=_tmp1D5.f1;_tmp1D7=
_tmp1D5.f2;_tmp1D6=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp1D6);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1D8=Cyc_NewControlFlow_use_Rval(env,_tmp14E->loc,_tmp1D6,(void*)((struct Cyc_List_List*)
_check_null(_tmp1D7))->hd);_tmp1D7=_tmp1D7->tl;{struct Cyc_List_List*init_params=
0;{void*_tmp1D9=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp14E->topt))->v);struct Cyc_Absyn_PtrInfo _tmp1DA;void*_tmp1DB;_LL15E: if(
_tmp1D9 <= (void*)4)goto _LL160;if(*((int*)_tmp1D9)!= 4)goto _LL160;_tmp1DA=((
struct Cyc_Absyn_PointerType_struct*)_tmp1D9)->f1;_tmp1DB=(void*)_tmp1DA.elt_typ;
_LL15F:{void*_tmp1DC=Cyc_Tcutil_compress(_tmp1DB);struct Cyc_Absyn_FnInfo _tmp1DD;
struct Cyc_List_List*_tmp1DE;_LL163: if(_tmp1DC <= (void*)4)goto _LL165;if(*((int*)
_tmp1DC)!= 8)goto _LL165;_tmp1DD=((struct Cyc_Absyn_FnType_struct*)_tmp1DC)->f1;
_tmp1DE=_tmp1DD.attributes;_LL164: for(0;_tmp1DE != 0;_tmp1DE=_tmp1DE->tl){void*
_tmp1DF=(void*)_tmp1DE->hd;int _tmp1E0;_LL168: if(_tmp1DF <= (void*)17)goto _LL16A;
if(*((int*)_tmp1DF)!= 4)goto _LL16A;_tmp1E0=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp1DF)->f1;_LL169:{struct Cyc_List_List*_tmp684;init_params=((_tmp684=
_region_malloc(temp,sizeof(*_tmp684)),((_tmp684->hd=(void*)_tmp1E0,((_tmp684->tl=
init_params,_tmp684))))));}goto _LL167;_LL16A:;_LL16B: goto _LL167;_LL167:;}goto
_LL162;_LL165:;_LL166: {const char*_tmp687;void*_tmp686;(_tmp686=0,((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp687="anal_Rexp: bad function type",
_tag_dynforward(_tmp687,sizeof(char),_get_zero_arr_size_char(_tmp687,29)))),
_tag_dynforward(_tmp686,sizeof(void*),0)));}_LL162:;}goto _LL15D;_LL160:;_LL161: {
const char*_tmp68A;void*_tmp689;(_tmp689=0,((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp68A="anal_Rexp: bad function type",
_tag_dynforward(_tmp68A,sizeof(char),_get_zero_arr_size_char(_tmp68A,29)))),
_tag_dynforward(_tmp689,sizeof(void*),0)));}_LL15D:;}{int i=1;for(0;_tmp1D7 != 0;(((
_tmp1D7=_tmp1D7->tl,_tmp14F=((struct Cyc_List_List*)_check_null(_tmp14F))->tl)),
++ i)){if(!((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
_tmp1D8=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp14F))->hd)->loc,_tmp1D8,(void*)_tmp1D7->hd);continue;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1E6=_tmp1D6;struct Cyc_Dict_Dict _tmp1E7;_LL16D: if((_tmp1E6.BottomFL).tag != 0)
goto _LL16F;_LL16E: goto _LL16C;_LL16F: if((_tmp1E6.ReachableFL).tag != 1)goto _LL16C;
_tmp1E7=(_tmp1E6.ReachableFL).f1;_LL170: if(Cyc_CfFlowInfo_initlevel(_tmp1E7,(
void*)_tmp1D7->hd)== (void*)0){const char*_tmp68D;void*_tmp68C;(_tmp68C=0,Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp14F))->hd)->loc,((
_tmp68D="expression may not be initialized",_tag_dynforward(_tmp68D,sizeof(char),
_get_zero_arr_size_char(_tmp68D,34)))),_tag_dynforward(_tmp68C,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo_union _tmp1EA=_tmp1D8;struct Cyc_Dict_Dict _tmp1EB;
struct Cyc_List_List*_tmp1EC;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1ED;_LL172: if((
_tmp1EA.BottomFL).tag != 0)goto _LL174;_LL173: goto _LL171;_LL174: if((_tmp1EA.ReachableFL).tag
!= 1)goto _LL171;_tmp1EB=(_tmp1EA.ReachableFL).f1;_tmp1EC=(_tmp1EA.ReachableFL).f2;
_tmp1ED=(_tmp1EA.ReachableFL).f3;_LL175: {struct Cyc_Dict_Dict _tmp1EE=Cyc_CfFlowInfo_escape_deref(
_tmp127,_tmp1EB,env->all_changed,(void*)_tmp1D7->hd);{void*_tmp1EF=(void*)
_tmp1D7->hd;struct Cyc_CfFlowInfo_Place*_tmp1F0;_LL177: if(_tmp1EF <= (void*)3)goto
_LL179;if(*((int*)_tmp1EF)!= 2)goto _LL179;_tmp1F0=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmp1EF)->f1;_LL178:{void*_tmp1F1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp14F))->hd)->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp1F2;void*_tmp1F3;_LL17C: if(_tmp1F1 <= (void*)4)goto
_LL17E;if(*((int*)_tmp1F1)!= 4)goto _LL17E;_tmp1F2=((struct Cyc_Absyn_PointerType_struct*)
_tmp1F1)->f1;_tmp1F3=(void*)_tmp1F2.elt_typ;_LL17D: _tmp1EE=Cyc_CfFlowInfo_assign_place(
_tmp127,((struct Cyc_Absyn_Exp*)_tmp14F->hd)->loc,_tmp1EE,env->all_changed,
_tmp1F0,Cyc_CfFlowInfo_typ_to_absrval(_tmp127,_tmp1F3,(void*)_tmp127->esc_all));
goto _LL17B;_LL17E:;_LL17F: {const char*_tmp690;void*_tmp68F;(_tmp68F=0,((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp690="anal_Rexp:bad type for initialized arg",
_tag_dynforward(_tmp690,sizeof(char),_get_zero_arr_size_char(_tmp690,39)))),
_tag_dynforward(_tmp68F,sizeof(void*),0)));}_LL17B:;}goto _LL176;_LL179:;_LL17A:
goto _LL176;_LL176:;}{union Cyc_CfFlowInfo_FlowInfo_union _tmp691;_tmp1D8=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp691.ReachableFL).tag=1,(((_tmp691.ReachableFL).f1=_tmp1EE,(((_tmp691.ReachableFL).f2=
_tmp1EC,(((_tmp691.ReachableFL).f3=_tmp1ED,_tmp691))))))));}goto _LL171;}_LL171:;}
goto _LL16C;_LL16C:;}}}if(Cyc_Tcutil_is_noreturn((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp14E->topt))->v)){union Cyc_CfFlowInfo_FlowInfo_union _tmp694;
struct _tuple5 _tmp693;struct _tuple5 _tmp1F9=(_tmp693.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp694.BottomFL).tag=0,_tmp694)),((_tmp693.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp127,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp127->unknown_all),
_tmp693)));_npop_handler(0);return _tmp1F9;}else{struct _tuple5 _tmp695;struct
_tuple5 _tmp1FB=(_tmp695.f1=_tmp1D8,((_tmp695.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp127,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp127->unknown_all),
_tmp695)));_npop_handler(0);return _tmp1FB;}}}};_pop_region(temp);}_LL10D: if(*((
int*)_tmp12E)!= 35)goto _LL10F;_tmp150=((struct Cyc_Absyn_Malloc_e_struct*)_tmp12E)->f1;
_tmp151=_tmp150.is_calloc;_tmp152=_tmp150.rgn;_tmp153=_tmp150.elt_type;_tmp154=
_tmp150.num_elts;_tmp155=_tmp150.fat_result;_LL10E: {struct Cyc_CfFlowInfo_MallocPt_struct
_tmp698;struct Cyc_CfFlowInfo_MallocPt_struct*_tmp697;void*root=(void*)((_tmp697=
_region_malloc(_tmp127->r,sizeof(*_tmp697)),((_tmp697[0]=((_tmp698.tag=1,((
_tmp698.f1=_tmp154,((_tmp698.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v),_tmp698)))))),_tmp697))));struct Cyc_CfFlowInfo_Place*_tmp699;
struct Cyc_CfFlowInfo_Place*place=(_tmp699=_region_malloc(_tmp127->r,sizeof(*
_tmp699)),((_tmp699->root=(void*)root,((_tmp699->fields=0,_tmp699)))));struct Cyc_CfFlowInfo_AddressOf_struct
_tmp69C;struct Cyc_CfFlowInfo_AddressOf_struct*_tmp69B;void*rval=(void*)((_tmp69B=
_region_malloc(_tmp127->r,sizeof(*_tmp69B)),((_tmp69B[0]=((_tmp69C.tag=2,((
_tmp69C.f1=place,_tmp69C)))),_tmp69B))));void*place_val=_tmp155?(void*)1: Cyc_CfFlowInfo_typ_to_absrval(
_tmp127,*((void**)_check_null(_tmp153)),(void*)_tmp127->unknown_none);union Cyc_CfFlowInfo_FlowInfo_union
outflow;Cyc_CfFlowInfo_update_place_set(env->all_changed,place,0);if(_tmp152 != 0){
struct _RegionHandle _tmp1FC=_new_region("temp");struct _RegionHandle*temp=& _tmp1FC;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1FF;struct Cyc_List_List*
_tmp200;struct Cyc_Absyn_Exp*_tmp69D[2];struct _tuple11 _tmp1FE=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,((_tmp69D[1]=_tmp154,((_tmp69D[0]=(struct Cyc_Absyn_Exp*)_tmp152,((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(
temp,_tag_dynforward(_tmp69D,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);_tmp1FF=
_tmp1FE.f1;_tmp200=_tmp1FE.f2;outflow=_tmp1FF;};_pop_region(temp);}else{union Cyc_CfFlowInfo_FlowInfo_union
_tmp202;struct _tuple5 _tmp201=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp154);
_tmp202=_tmp201.f1;outflow=_tmp202;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp154->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp203=outflow;struct
Cyc_Dict_Dict _tmp204;struct Cyc_List_List*_tmp205;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp206;_LL181: if((_tmp203.BottomFL).tag != 0)goto _LL183;_LL182: {struct _tuple5
_tmp69E;return(_tmp69E.f1=outflow,((_tmp69E.f2=rval,_tmp69E)));}_LL183: if((
_tmp203.ReachableFL).tag != 1)goto _LL180;_tmp204=(_tmp203.ReachableFL).f1;_tmp205=(
_tmp203.ReachableFL).f2;_tmp206=(_tmp203.ReachableFL).f3;_LL184: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp6A1;struct _tuple5 _tmp6A0;return(_tmp6A0.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp6A1.ReachableFL).tag=1,(((_tmp6A1.ReachableFL).f1=Cyc_Dict_insert(_tmp204,
root,place_val),(((_tmp6A1.ReachableFL).f2=_tmp205,(((_tmp6A1.ReachableFL).f3=
_tmp206,_tmp6A1)))))))),((_tmp6A0.f2=rval,_tmp6A0)));}_LL180:;}}_LL10F: if(*((int*)
_tmp12E)!= 36)goto _LL111;_tmp156=((struct Cyc_Absyn_Swap_e_struct*)_tmp12E)->f1;
_tmp157=((struct Cyc_Absyn_Swap_e_struct*)_tmp12E)->f2;_LL110: {void*left_rval;
void*right_rval;union Cyc_CfFlowInfo_FlowInfo_union outflow;{struct _RegionHandle
_tmp20F=_new_region("temp");struct _RegionHandle*temp=& _tmp20F;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp212;struct Cyc_List_List*_tmp213;struct Cyc_Absyn_Exp*
_tmp6A2[2];struct _tuple11 _tmp211=Cyc_NewControlFlow_anal_unordered_Rexps(temp,
env,inflow,((_tmp6A2[1]=_tmp157,((_tmp6A2[0]=_tmp156,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(temp,_tag_dynforward(
_tmp6A2,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);_tmp212=_tmp211.f1;_tmp213=
_tmp211.f2;left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp213))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp213->tl))->hd;outflow=
_tmp212;};_pop_region(temp);}Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
outflow);{union Cyc_CfFlowInfo_AbsLVal_union _tmp215;struct _tuple7 _tmp214=Cyc_NewControlFlow_anal_Lexp(
env,outflow,_tmp156);_tmp215=_tmp214.f2;{union Cyc_CfFlowInfo_AbsLVal_union
_tmp217;struct _tuple7 _tmp216=Cyc_NewControlFlow_anal_Lexp(env,outflow,_tmp157);
_tmp217=_tmp216.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp218=outflow;struct Cyc_Dict_Dict
_tmp219;struct Cyc_List_List*_tmp21A;struct Cyc_CfFlowInfo_ConsumeInfo _tmp21B;
_LL186: if((_tmp218.ReachableFL).tag != 1)goto _LL188;_tmp219=(_tmp218.ReachableFL).f1;
_tmp21A=(_tmp218.ReachableFL).f2;_tmp21B=(_tmp218.ReachableFL).f3;_LL187:{union
Cyc_CfFlowInfo_AbsLVal_union _tmp21C=_tmp215;struct Cyc_CfFlowInfo_Place*_tmp21D;
_LL18B: if((_tmp21C.PlaceL).tag != 0)goto _LL18D;_tmp21D=(_tmp21C.PlaceL).f1;_LL18C:
_tmp219=Cyc_CfFlowInfo_assign_place(_tmp127,_tmp156->loc,_tmp219,env->all_changed,
_tmp21D,right_rval);goto _LL18A;_LL18D: if((_tmp21C.UnknownL).tag != 1)goto _LL18A;
_LL18E: goto _LL18A;_LL18A:;}{union Cyc_CfFlowInfo_AbsLVal_union _tmp21E=_tmp217;
struct Cyc_CfFlowInfo_Place*_tmp21F;_LL190: if((_tmp21E.PlaceL).tag != 0)goto _LL192;
_tmp21F=(_tmp21E.PlaceL).f1;_LL191: _tmp219=Cyc_CfFlowInfo_assign_place(_tmp127,
_tmp157->loc,_tmp219,env->all_changed,_tmp21F,left_rval);goto _LL18F;_LL192: if((
_tmp21E.UnknownL).tag != 1)goto _LL18F;_LL193: goto _LL18F;_LL18F:;}_tmp21A=Cyc_CfFlowInfo_reln_kill_exp(
_tmp127->r,_tmp21A,_tmp156);_tmp21A=Cyc_CfFlowInfo_reln_kill_exp(_tmp127->r,
_tmp21A,_tmp157);{union Cyc_CfFlowInfo_FlowInfo_union _tmp6A3;outflow=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp6A3.ReachableFL).tag=1,(((_tmp6A3.ReachableFL).f1=_tmp219,(((_tmp6A3.ReachableFL).f2=
_tmp21A,(((_tmp6A3.ReachableFL).f3=_tmp21B,_tmp6A3))))))));}goto _LL185;_LL188:;
_LL189: goto _LL185;_LL185:;}{struct _tuple5 _tmp6A4;return(_tmp6A4.f1=outflow,((
_tmp6A4.f2=(void*)_tmp127->unknown_all,_tmp6A4)));}}}}_LL111: if(*((int*)_tmp12E)
!= 17)goto _LL113;_tmp158=((struct Cyc_Absyn_New_e_struct*)_tmp12E)->f1;_tmp159=((
struct Cyc_Absyn_New_e_struct*)_tmp12E)->f2;_LL112: {struct Cyc_CfFlowInfo_MallocPt_struct
_tmp6A7;struct Cyc_CfFlowInfo_MallocPt_struct*_tmp6A6;void*root=(void*)((_tmp6A6=
_region_malloc(_tmp127->r,sizeof(*_tmp6A6)),((_tmp6A6[0]=((_tmp6A7.tag=1,((
_tmp6A7.f1=_tmp159,((_tmp6A7.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(
e->topt))->v),_tmp6A7)))))),_tmp6A6))));struct Cyc_CfFlowInfo_Place*_tmp6A8;
struct Cyc_CfFlowInfo_Place*place=(_tmp6A8=_region_malloc(_tmp127->r,sizeof(*
_tmp6A8)),((_tmp6A8->root=(void*)root,((_tmp6A8->fields=0,_tmp6A8)))));struct Cyc_CfFlowInfo_AddressOf_struct
_tmp6AB;struct Cyc_CfFlowInfo_AddressOf_struct*_tmp6AA;void*rval=(void*)((_tmp6AA=
_region_malloc(_tmp127->r,sizeof(*_tmp6AA)),((_tmp6AA[0]=((_tmp6AB.tag=2,((
_tmp6AB.f1=place,_tmp6AB)))),_tmp6AA))));Cyc_CfFlowInfo_update_place_set(env->all_changed,
place,0);{union Cyc_CfFlowInfo_FlowInfo_union outflow;void*place_val;if(_tmp158 != 
0){struct _RegionHandle _tmp222=_new_region("temp");struct _RegionHandle*temp=&
_tmp222;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp225;struct Cyc_List_List*
_tmp226;struct Cyc_Absyn_Exp*_tmp6AC[2];struct _tuple11 _tmp224=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,((_tmp6AC[1]=_tmp159,((_tmp6AC[0]=(struct Cyc_Absyn_Exp*)_tmp158,((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(
temp,_tag_dynforward(_tmp6AC,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);_tmp225=
_tmp224.f1;_tmp226=_tmp224.f2;outflow=_tmp225;place_val=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp226))->tl))->hd;};_pop_region(
temp);}else{union Cyc_CfFlowInfo_FlowInfo_union _tmp228;void*_tmp229;struct _tuple5
_tmp227=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp159);_tmp228=_tmp227.f1;
_tmp229=_tmp227.f2;outflow=_tmp228;place_val=_tmp229;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp159->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp22A=outflow;struct
Cyc_Dict_Dict _tmp22B;struct Cyc_List_List*_tmp22C;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp22D;_LL195: if((_tmp22A.BottomFL).tag != 0)goto _LL197;_LL196: {struct _tuple5
_tmp6AD;return(_tmp6AD.f1=outflow,((_tmp6AD.f2=rval,_tmp6AD)));}_LL197: if((
_tmp22A.ReachableFL).tag != 1)goto _LL194;_tmp22B=(_tmp22A.ReachableFL).f1;_tmp22C=(
_tmp22A.ReachableFL).f2;_tmp22D=(_tmp22A.ReachableFL).f3;_LL198: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp6B0;struct _tuple5 _tmp6AF;return(_tmp6AF.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp6B0.ReachableFL).tag=1,(((_tmp6B0.ReachableFL).f1=Cyc_Dict_insert(_tmp22B,
root,place_val),(((_tmp6B0.ReachableFL).f2=_tmp22C,(((_tmp6B0.ReachableFL).f3=
_tmp22D,_tmp6B0)))))))),((_tmp6AF.f2=rval,_tmp6AF)));}_LL194:;}}}_LL113: if(*((
int*)_tmp12E)!= 16)goto _LL115;_tmp15A=((struct Cyc_Absyn_Address_e_struct*)
_tmp12E)->f1;_LL114: {union Cyc_CfFlowInfo_FlowInfo_union _tmp237;struct _tuple5
_tmp236=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp15A);_tmp237=_tmp236.f1;{
struct Cyc_CfFlowInfo_ConsumeInfo _tmp239;struct _tuple6 _tmp238=Cyc_CfFlowInfo_save_consume_info(
env->fenv,_tmp237,0);_tmp239=_tmp238.f1;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp23B;union Cyc_CfFlowInfo_AbsLVal_union _tmp23C;struct _tuple7 _tmp23A=Cyc_NewControlFlow_anal_Lexp(
env,inflow,_tmp15A);_tmp23B=_tmp23A.f1;_tmp23C=_tmp23A.f2;_tmp23B=Cyc_CfFlowInfo_restore_consume_info(
_tmp23B,_tmp239);{union Cyc_CfFlowInfo_AbsLVal_union _tmp23D=_tmp23C;struct Cyc_CfFlowInfo_Place*
_tmp23E;_LL19A: if((_tmp23D.UnknownL).tag != 1)goto _LL19C;_LL19B: {struct _tuple5
_tmp6B1;return(_tmp6B1.f1=_tmp23B,((_tmp6B1.f2=(void*)1,_tmp6B1)));}_LL19C: if((
_tmp23D.PlaceL).tag != 0)goto _LL199;_tmp23E=(_tmp23D.PlaceL).f1;_LL19D: {struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp6B7;struct Cyc_CfFlowInfo_AddressOf_struct
_tmp6B6;struct _tuple5 _tmp6B5;return(_tmp6B5.f1=_tmp23B,((_tmp6B5.f2=(void*)((
_tmp6B7=_region_malloc(env->r,sizeof(*_tmp6B7)),((_tmp6B7[0]=((_tmp6B6.tag=2,((
_tmp6B6.f1=_tmp23E,_tmp6B6)))),_tmp6B7)))),_tmp6B5)));}_LL199:;}}}}_LL115: if(*((
int*)_tmp12E)!= 22)goto _LL117;_tmp15B=((struct Cyc_Absyn_Deref_e_struct*)_tmp12E)->f1;
_LL116: {union Cyc_CfFlowInfo_FlowInfo_union _tmp244;void*_tmp245;struct _tuple5
_tmp243=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp15B);_tmp244=_tmp243.f1;
_tmp245=_tmp243.f2;_tmp244=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp244);return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp244,_tmp15B,_tmp245);}
_LL117: if(*((int*)_tmp12E)!= 23)goto _LL119;_tmp15C=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp12E)->f1;_tmp15D=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp12E)->f2;_LL118: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp247;void*_tmp248;struct _tuple5 _tmp246=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp15C);_tmp247=_tmp246.f1;_tmp248=_tmp246.f2;_tmp247=Cyc_CfFlowInfo_drop_unique_rvals(
e->loc,_tmp247);_tmp247=Cyc_NewControlFlow_may_consume_exp(env,_tmp247,e);if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp15C->topt))->v)){struct _tuple5
_tmp6B8;return(_tmp6B8.f1=_tmp247,((_tmp6B8.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp127,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp127->unknown_all),
_tmp6B8)));}{void*_tmp24A=_tmp248;struct _dynforward_ptr _tmp24B;_LL19F: if(_tmp24A
<= (void*)3)goto _LL1A1;if(*((int*)_tmp24A)!= 4)goto _LL1A1;_tmp24B=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp24A)->f1;_LL1A0: {int _tmp24C=Cyc_NewControlFlow_get_field_index((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp15C->topt))->v,_tmp15D);struct _tuple5 _tmp6B9;
return(_tmp6B9.f1=_tmp247,((_tmp6B9.f2=*((void**)_check_dynforward_subscript(
_tmp24B,sizeof(void*),_tmp24C)),_tmp6B9)));}_LL1A1:;_LL1A2: {struct Cyc_Core_Impossible_struct
_tmp6C6;const char*_tmp6C5;void*_tmp6C4[1];struct Cyc_String_pa_struct _tmp6C3;
struct Cyc_Core_Impossible_struct*_tmp6C2;(int)_throw((void*)((_tmp6C2=_cycalloc(
sizeof(*_tmp6C2)),((_tmp6C2[0]=((_tmp6C6.tag=Cyc_Core_Impossible,((_tmp6C6.f1=(
struct _dynforward_ptr)((_tmp6C3.tag=0,((_tmp6C3.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_exp2string(e)),((_tmp6C4[0]=& _tmp6C3,Cyc_aprintf(((
_tmp6C5="anal_Rexp: AggrMember: %s",_tag_dynforward(_tmp6C5,sizeof(char),
_get_zero_arr_size_char(_tmp6C5,26)))),_tag_dynforward(_tmp6C4,sizeof(void*),1)))))))),
_tmp6C6)))),_tmp6C2)))));}_LL19E:;}}_LL119: if(*((int*)_tmp12E)!= 24)goto _LL11B;
_tmp15E=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp12E)->f1;_tmp15F=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp12E)->f2;_LL11A: {union Cyc_CfFlowInfo_FlowInfo_union _tmp254;void*_tmp255;
struct _tuple5 _tmp253=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp15E);_tmp254=
_tmp253.f1;_tmp255=_tmp253.f2;_tmp254=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp254);{union Cyc_CfFlowInfo_FlowInfo_union _tmp257;void*_tmp258;struct _tuple5
_tmp256=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp254,_tmp15E,_tmp255);
_tmp257=_tmp256.f1;_tmp258=_tmp256.f2;{void*_tmp259=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp15E->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp25A;void*_tmp25B;_LL1A4: if(_tmp259 <= (void*)4)goto _LL1A6;if(*((int*)_tmp259)
!= 4)goto _LL1A6;_tmp25A=((struct Cyc_Absyn_PointerType_struct*)_tmp259)->f1;
_tmp25B=(void*)_tmp25A.elt_typ;_LL1A5: if(Cyc_Absyn_is_union_type(_tmp25B)){
struct _tuple5 _tmp6C7;return(_tmp6C7.f1=_tmp257,((_tmp6C7.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp127,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp127->unknown_all),
_tmp6C7)));}{void*_tmp25D=_tmp258;struct _dynforward_ptr _tmp25E;_LL1A9: if(_tmp25D
<= (void*)3)goto _LL1AB;if(*((int*)_tmp25D)!= 4)goto _LL1AB;_tmp25E=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp25D)->f1;_LL1AA: {int _tmp25F=Cyc_NewControlFlow_get_field_index(_tmp25B,
_tmp15F);struct _tuple5 _tmp6C8;return(_tmp6C8.f1=_tmp257,((_tmp6C8.f2=*((void**)
_check_dynforward_subscript(_tmp25E,sizeof(void*),_tmp25F)),_tmp6C8)));}_LL1AB:;
_LL1AC: {struct Cyc_Core_Impossible_struct _tmp6CE;const char*_tmp6CD;struct Cyc_Core_Impossible_struct*
_tmp6CC;(int)_throw((void*)((_tmp6CC=_cycalloc(sizeof(*_tmp6CC)),((_tmp6CC[0]=((
_tmp6CE.tag=Cyc_Core_Impossible,((_tmp6CE.f1=((_tmp6CD="anal_Rexp: AggrArrow",
_tag_dynforward(_tmp6CD,sizeof(char),_get_zero_arr_size_char(_tmp6CD,21)))),
_tmp6CE)))),_tmp6CC)))));}_LL1A8:;}_LL1A6:;_LL1A7: {struct Cyc_Core_Impossible_struct
_tmp6D4;const char*_tmp6D3;struct Cyc_Core_Impossible_struct*_tmp6D2;(int)_throw((
void*)((_tmp6D2=_cycalloc(sizeof(*_tmp6D2)),((_tmp6D2[0]=((_tmp6D4.tag=Cyc_Core_Impossible,((
_tmp6D4.f1=((_tmp6D3="anal_Rexp: AggrArrow ptr",_tag_dynforward(_tmp6D3,sizeof(
char),_get_zero_arr_size_char(_tmp6D3,25)))),_tmp6D4)))),_tmp6D2)))));}_LL1A3:;}}}
_LL11B: if(*((int*)_tmp12E)!= 6)goto _LL11D;_tmp160=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp12E)->f1;_tmp161=((struct Cyc_Absyn_Conditional_e_struct*)_tmp12E)->f2;
_tmp162=((struct Cyc_Absyn_Conditional_e_struct*)_tmp12E)->f3;_LL11C: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp268;union Cyc_CfFlowInfo_FlowInfo_union _tmp269;struct _tuple8 _tmp267=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp160);_tmp268=_tmp267.f1;_tmp269=_tmp267.f2;_tmp268=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp160->loc,_tmp268);_tmp269=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp160->loc,
_tmp269);{struct _tuple5 _tmp26A=Cyc_NewControlFlow_anal_Rexp(env,_tmp268,_tmp161);
struct _tuple5 _tmp26B=Cyc_NewControlFlow_anal_Rexp(env,_tmp269,_tmp162);return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp127,env->all_changed,_tmp26A,_tmp26B,1);}}_LL11D: if(*((int*)_tmp12E)!= 7)
goto _LL11F;_tmp163=((struct Cyc_Absyn_And_e_struct*)_tmp12E)->f1;_tmp164=((struct
Cyc_Absyn_And_e_struct*)_tmp12E)->f2;_LL11E: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp26D;union Cyc_CfFlowInfo_FlowInfo_union _tmp26E;struct _tuple8 _tmp26C=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp163);_tmp26D=_tmp26C.f1;_tmp26E=_tmp26C.f2;_tmp26D=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp163->loc,_tmp26D);_tmp26E=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp163->loc,
_tmp26E);{union Cyc_CfFlowInfo_FlowInfo_union _tmp270;void*_tmp271;struct _tuple5
_tmp26F=Cyc_NewControlFlow_anal_Rexp(env,_tmp26D,_tmp164);_tmp270=_tmp26F.f1;
_tmp271=_tmp26F.f2;_tmp270=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp164->loc,
_tmp270);{struct _tuple5 _tmp6D5;struct _tuple5 _tmp272=(_tmp6D5.f1=_tmp270,((
_tmp6D5.f2=_tmp271,_tmp6D5)));struct _tuple5 _tmp6D6;struct _tuple5 _tmp273=(_tmp6D6.f1=
_tmp26E,((_tmp6D6.f2=(void*)((void*)0),_tmp6D6)));return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp127,env->all_changed,_tmp272,_tmp273,0);}}}_LL11F: if(*((int*)_tmp12E)!= 8)
goto _LL121;_tmp165=((struct Cyc_Absyn_Or_e_struct*)_tmp12E)->f1;_tmp166=((struct
Cyc_Absyn_Or_e_struct*)_tmp12E)->f2;_LL120: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp277;union Cyc_CfFlowInfo_FlowInfo_union _tmp278;struct _tuple8 _tmp276=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp165);_tmp277=_tmp276.f1;_tmp278=_tmp276.f2;_tmp277=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp165->loc,_tmp277);_tmp278=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp165->loc,
_tmp278);{union Cyc_CfFlowInfo_FlowInfo_union _tmp27A;void*_tmp27B;struct _tuple5
_tmp279=Cyc_NewControlFlow_anal_Rexp(env,_tmp278,_tmp166);_tmp27A=_tmp279.f1;
_tmp27B=_tmp279.f2;_tmp27A=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp166->loc,
_tmp27A);{struct _tuple5 _tmp6D7;struct _tuple5 _tmp27C=(_tmp6D7.f1=_tmp27A,((
_tmp6D7.f2=_tmp27B,_tmp6D7)));struct _tuple5 _tmp6D8;struct _tuple5 _tmp27D=(_tmp6D8.f1=
_tmp277,((_tmp6D8.f2=(void*)((void*)1),_tmp6D8)));return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp127,env->all_changed,_tmp27C,_tmp27D,0);}}}_LL121: if(*((int*)_tmp12E)!= 25)
goto _LL123;_tmp167=((struct Cyc_Absyn_Subscript_e_struct*)_tmp12E)->f1;_tmp168=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp12E)->f2;_LL122: {struct _RegionHandle
_tmp280=_new_region("temp");struct _RegionHandle*temp=& _tmp280;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp283;struct Cyc_List_List*_tmp284;struct Cyc_Absyn_Exp*
_tmp6D9[2];struct _tuple11 _tmp282=Cyc_NewControlFlow_anal_unordered_Rexps(temp,
env,inflow,((_tmp6D9[1]=_tmp168,((_tmp6D9[0]=_tmp167,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(temp,_tag_dynforward(
_tmp6D9,sizeof(struct Cyc_Absyn_Exp*),2)))))),1);_tmp283=_tmp282.f1;_tmp284=
_tmp282.f2;_tmp283=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp168->loc,_tmp283);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp285=_tmp283;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp286=_tmp283;struct Cyc_Dict_Dict _tmp287;struct Cyc_List_List*_tmp288;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp289;_LL1AE: if((_tmp286.ReachableFL).tag != 1)goto _LL1B0;_tmp287=(_tmp286.ReachableFL).f1;
_tmp288=(_tmp286.ReachableFL).f2;_tmp289=(_tmp286.ReachableFL).f3;_LL1AF: if(Cyc_CfFlowInfo_initlevel(
_tmp287,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp284))->tl))->hd)== (void*)0){const char*_tmp6DC;void*_tmp6DB;(
_tmp6DB=0,Cyc_Tcutil_terr(_tmp168->loc,((_tmp6DC="expression may not be initialized",
_tag_dynforward(_tmp6DC,sizeof(char),_get_zero_arr_size_char(_tmp6DC,34)))),
_tag_dynforward(_tmp6DB,sizeof(void*),0)));}{struct Cyc_List_List*_tmp28C=Cyc_NewControlFlow_add_subscript_reln(
_tmp127->r,_tmp288,_tmp167,_tmp168);if(_tmp288 != _tmp28C){union Cyc_CfFlowInfo_FlowInfo_union
_tmp6DD;_tmp285=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp6DD.ReachableFL).tag=
1,(((_tmp6DD.ReachableFL).f1=_tmp287,(((_tmp6DD.ReachableFL).f2=_tmp28C,(((
_tmp6DD.ReachableFL).f3=_tmp289,_tmp6DD))))))));}goto _LL1AD;}_LL1B0:;_LL1B1: goto
_LL1AD;_LL1AD:;}{void*_tmp28E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp167->topt))->v);struct Cyc_List_List*_tmp28F;struct Cyc_Absyn_PtrInfo
_tmp290;struct Cyc_Absyn_PtrAtts _tmp291;struct Cyc_Absyn_Conref*_tmp292;_LL1B3: if(
_tmp28E <= (void*)4)goto _LL1B7;if(*((int*)_tmp28E)!= 9)goto _LL1B5;_tmp28F=((
struct Cyc_Absyn_TupleType_struct*)_tmp28E)->f1;_LL1B4: {void*_tmp293=(void*)((
struct Cyc_List_List*)_check_null(_tmp284))->hd;struct _dynforward_ptr _tmp294;
_LL1BA: if(_tmp293 <= (void*)3)goto _LL1BC;if(*((int*)_tmp293)!= 4)goto _LL1BC;
_tmp294=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp293)->f1;_LL1BB: _tmp285=Cyc_NewControlFlow_may_consume_exp(
env,_tmp285,e);{unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp168)).f1;struct
_tuple5 _tmp6DE;struct _tuple5 _tmp296=(_tmp6DE.f1=_tmp285,((_tmp6DE.f2=*((void**)
_check_dynforward_subscript(_tmp294,sizeof(void*),(int)i)),_tmp6DE)));
_npop_handler(0);return _tmp296;}_LL1BC:;_LL1BD: {struct Cyc_Core_Impossible_struct
_tmp6E4;const char*_tmp6E3;struct Cyc_Core_Impossible_struct*_tmp6E2;(int)_throw((
void*)((_tmp6E2=_cycalloc(sizeof(*_tmp6E2)),((_tmp6E2[0]=((_tmp6E4.tag=Cyc_Core_Impossible,((
_tmp6E4.f1=((_tmp6E3="anal_Rexp: Subscript",_tag_dynforward(_tmp6E3,sizeof(char),
_get_zero_arr_size_char(_tmp6E3,21)))),_tmp6E4)))),_tmp6E2)))));}_LL1B9:;}_LL1B5:
if(*((int*)_tmp28E)!= 4)goto _LL1B7;_tmp290=((struct Cyc_Absyn_PointerType_struct*)
_tmp28E)->f1;_tmp291=_tmp290.ptr_atts;_tmp292=_tmp291.bounds;_LL1B6:{union Cyc_Absyn_Constraint_union
_tmp29A=(Cyc_Absyn_compress_conref(_tmp292))->v;void*_tmp29B;_LL1BF: if((_tmp29A.Eq_constr).tag
!= 0)goto _LL1C1;_tmp29B=(_tmp29A.Eq_constr).f1;if(_tmp29B <= (void*)2)goto _LL1C1;
if(*((int*)_tmp29B)!= 1)goto _LL1C1;_LL1C0:{void*_tmp29C=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp284))->tl))->hd;struct Cyc_List_List*
_tmp29D;_LL1C4: if(_tmp29C <= (void*)3)goto _LL1C6;if(*((int*)_tmp29C)!= 3)goto
_LL1C6;_tmp29D=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp29C)->f1;_LL1C5:{
struct Cyc_CfFlowInfo_HasTagCmps_struct _tmp6E7;struct Cyc_CfFlowInfo_HasTagCmps_struct*
_tmp6E6;(void*)(_tmp168->annot=(void*)((void*)((_tmp6E6=_cycalloc(sizeof(*
_tmp6E6)),((_tmp6E6[0]=((_tmp6E7.tag=Cyc_CfFlowInfo_HasTagCmps,((_tmp6E7.f1=Cyc_CfFlowInfo_copy_tagcmps(
Cyc_Core_heap_region,_tmp29D),_tmp6E7)))),_tmp6E6))))));}goto _LL1C3;_LL1C6:;
_LL1C7: goto _LL1C3;_LL1C3:;}goto _LL1BE;_LL1C1:;_LL1C2: goto _LL1BE;_LL1BE:;}{union
Cyc_CfFlowInfo_FlowInfo_union _tmp2A1;void*_tmp2A2;struct _tuple5 _tmp2A0=Cyc_NewControlFlow_anal_derefR(
env,inflow,_tmp283,_tmp167,(void*)((struct Cyc_List_List*)_check_null(_tmp284))->hd);
_tmp2A1=_tmp2A0.f1;_tmp2A2=_tmp2A0.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp2A3=
_tmp2A1;_LL1C9: if((_tmp2A3.BottomFL).tag != 0)goto _LL1CB;_LL1CA: {struct _tuple5
_tmp6E8;struct _tuple5 _tmp2A5=(_tmp6E8.f1=_tmp2A1,((_tmp6E8.f2=_tmp2A2,_tmp6E8)));
_npop_handler(0);return _tmp2A5;}_LL1CB:;_LL1CC: {struct _tuple5 _tmp6E9;struct
_tuple5 _tmp2A7=(_tmp6E9.f1=_tmp285,((_tmp6E9.f2=_tmp2A2,_tmp6E9)));_npop_handler(
0);return _tmp2A7;}_LL1C8:;}}_LL1B7:;_LL1B8: {struct Cyc_Core_Impossible_struct
_tmp6EF;const char*_tmp6EE;struct Cyc_Core_Impossible_struct*_tmp6ED;(int)_throw((
void*)((_tmp6ED=_cycalloc(sizeof(*_tmp6ED)),((_tmp6ED[0]=((_tmp6EF.tag=Cyc_Core_Impossible,((
_tmp6EF.f1=((_tmp6EE="anal_Rexp: Subscript -- bad type",_tag_dynforward(_tmp6EE,
sizeof(char),_get_zero_arr_size_char(_tmp6EE,33)))),_tmp6EF)))),_tmp6ED)))));}
_LL1B2:;}}};_pop_region(temp);}_LL123: if(*((int*)_tmp12E)!= 32)goto _LL125;
_tmp169=((struct Cyc_Absyn_Tunion_e_struct*)_tmp12E)->f1;_tmp16A=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp12E)->f2;_LL124: if(_tmp16A->is_flat){struct _RegionHandle _tmp2AB=_new_region("temp");
struct _RegionHandle*temp=& _tmp2AB;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp2AD;struct Cyc_List_List*_tmp2AE;struct _tuple11 _tmp2AC=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp169,0);_tmp2AD=_tmp2AC.f1;_tmp2AE=_tmp2AC.f2;_tmp2AD=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2AD);for(0;(unsigned int)_tmp169;(_tmp169=_tmp169->tl,_tmp2AE=_tmp2AE->tl)){
_tmp2AD=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)_tmp169->hd)->loc,
_tmp2AD,(void*)((struct Cyc_List_List*)_check_null(_tmp2AE))->hd);}{struct _tuple5
_tmp6F0;struct _tuple5 _tmp2B0=(_tmp6F0.f1=_tmp2AD,((_tmp6F0.f2=(void*)_tmp127->unknown_all,
_tmp6F0)));_npop_handler(0);return _tmp2B0;}};_pop_region(temp);}_tmp16B=_tmp169;
goto _LL126;_LL125: if(*((int*)_tmp12E)!= 26)goto _LL127;_tmp16B=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp12E)->f1;_LL126: {struct _RegionHandle _tmp2B1=_new_region("temp");struct
_RegionHandle*temp=& _tmp2B1;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp2B3;struct Cyc_List_List*_tmp2B4;struct _tuple11 _tmp2B2=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp16B,0);_tmp2B3=_tmp2B2.f1;_tmp2B4=_tmp2B2.f2;_tmp2B3=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2B3);{unsigned int _tmp6F6;unsigned int _tmp6F5;struct _dynforward_ptr
_tmp6F4;void**_tmp6F3;unsigned int _tmp6F2;struct _dynforward_ptr aggrdict=(_tmp6F2=(
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp16B),((_tmp6F3=(
void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp6F2)),((_tmp6F4=
_tag_dynforward(_tmp6F3,sizeof(void*),_tmp6F2),((((_tmp6F5=_tmp6F2,_tmp6F7(&
_tmp6F6,& _tmp6F5,& _tmp6F3,& _tmp2B4))),_tmp6F4)))))));struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp6FD;struct Cyc_CfFlowInfo_Aggregate_struct _tmp6FC;struct _tuple5 _tmp6FB;struct
_tuple5 _tmp2B8=(_tmp6FB.f1=_tmp2B3,((_tmp6FB.f2=(void*)((_tmp6FD=_region_malloc(
env->r,sizeof(*_tmp6FD)),((_tmp6FD[0]=((_tmp6FC.tag=4,((_tmp6FC.f1=aggrdict,
_tmp6FC)))),_tmp6FD)))),_tmp6FB)));_npop_handler(0);return _tmp2B8;}};_pop_region(
temp);}_LL127: if(*((int*)_tmp12E)!= 31)goto _LL129;_tmp16C=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp12E)->f2;_LL128: {struct Cyc_List_List*fs;{void*_tmp2BE=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_List_List*
_tmp2BF;_LL1CE: if(_tmp2BE <= (void*)4)goto _LL1D0;if(*((int*)_tmp2BE)!= 11)goto
_LL1D0;_tmp2BF=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp2BE)->f2;_LL1CF: fs=
_tmp2BF;goto _LL1CD;_LL1D0:;_LL1D1: {struct Cyc_Core_Impossible_struct _tmp703;
const char*_tmp702;struct Cyc_Core_Impossible_struct*_tmp701;(int)_throw((void*)((
_tmp701=_cycalloc(sizeof(*_tmp701)),((_tmp701[0]=((_tmp703.tag=Cyc_Core_Impossible,((
_tmp703.f1=((_tmp702="anal_Rexp:anon struct has bad type",_tag_dynforward(
_tmp702,sizeof(char),_get_zero_arr_size_char(_tmp702,35)))),_tmp703)))),_tmp701)))));}
_LL1CD:;}_tmp16D=_tmp16C;_tmp172=fs;goto _LL12A;}_LL129: if(*((int*)_tmp12E)!= 30)
goto _LL12B;_tmp16D=((struct Cyc_Absyn_Struct_e_struct*)_tmp12E)->f3;_tmp16E=((
struct Cyc_Absyn_Struct_e_struct*)_tmp12E)->f4;if(_tmp16E == 0)goto _LL12B;_tmp16F=*
_tmp16E;_tmp170=_tmp16F.impl;if(_tmp170 == 0)goto _LL12B;_tmp171=*_tmp170;_tmp172=
_tmp171.fields;_LL12A: {struct _RegionHandle _tmp2C3=_new_region("temp");struct
_RegionHandle*temp=& _tmp2C3;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp2C5;struct Cyc_List_List*_tmp2C6;struct _tuple11 _tmp2C4=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(
struct _tuple14*))Cyc_Core_snd,_tmp16D),0);_tmp2C5=_tmp2C4.f1;_tmp2C6=_tmp2C4.f2;
_tmp2C5=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp2C5);{struct
_dynforward_ptr aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp127,_tmp172,(
void*)_tmp127->unknown_all);{int i=0;for(0;_tmp2C6 != 0;(((_tmp2C6=_tmp2C6->tl,
_tmp16D=_tmp16D->tl)),++ i)){struct Cyc_List_List*ds=(*((struct _tuple14*)((struct
Cyc_List_List*)_check_null(_tmp16D))->hd)).f1;for(0;ds != 0;ds=ds->tl){void*
_tmp2C7=(void*)ds->hd;struct _dynforward_ptr*_tmp2C8;_LL1D3: if(*((int*)_tmp2C7)!= 
0)goto _LL1D5;_LL1D4: {struct Cyc_Core_Impossible_struct _tmp709;const char*_tmp708;
struct Cyc_Core_Impossible_struct*_tmp707;(int)_throw((void*)((_tmp707=_cycalloc(
sizeof(*_tmp707)),((_tmp707[0]=((_tmp709.tag=Cyc_Core_Impossible,((_tmp709.f1=((
_tmp708="anal_Rexp:Struct_e",_tag_dynforward(_tmp708,sizeof(char),
_get_zero_arr_size_char(_tmp708,19)))),_tmp709)))),_tmp707)))));}_LL1D5: if(*((
int*)_tmp2C7)!= 1)goto _LL1D2;_tmp2C8=((struct Cyc_Absyn_FieldName_struct*)_tmp2C7)->f1;
_LL1D6: {int _tmp2CC=Cyc_NewControlFlow_get_field_index_fs(_tmp172,_tmp2C8);*((
void**)_check_dynforward_subscript(aggrdict,sizeof(void*),_tmp2CC))=(void*)
_tmp2C6->hd;}_LL1D2:;}}}{struct Cyc_CfFlowInfo_Aggregate_struct*_tmp70F;struct Cyc_CfFlowInfo_Aggregate_struct
_tmp70E;struct _tuple5 _tmp70D;struct _tuple5 _tmp2D0=(_tmp70D.f1=_tmp2C5,((_tmp70D.f2=(
void*)((_tmp70F=_region_malloc(env->r,sizeof(*_tmp70F)),((_tmp70F[0]=((_tmp70E.tag=
4,((_tmp70E.f1=aggrdict,_tmp70E)))),_tmp70F)))),_tmp70D)));_npop_handler(0);
return _tmp2D0;}}};_pop_region(temp);}_LL12B: if(*((int*)_tmp12E)!= 30)goto _LL12D;
_LL12C: {struct Cyc_Core_Impossible_struct _tmp715;const char*_tmp714;struct Cyc_Core_Impossible_struct*
_tmp713;(int)_throw((void*)((_tmp713=_cycalloc(sizeof(*_tmp713)),((_tmp713[0]=((
_tmp715.tag=Cyc_Core_Impossible,((_tmp715.f1=((_tmp714="anal_Rexp:missing aggrdeclimpl",
_tag_dynforward(_tmp714,sizeof(char),_get_zero_arr_size_char(_tmp714,31)))),
_tmp715)))),_tmp713)))));}_LL12D: if(*((int*)_tmp12E)!= 28)goto _LL12F;_tmp173=((
struct Cyc_Absyn_Array_e_struct*)_tmp12E)->f1;_LL12E: {struct _RegionHandle _tmp2D4=
_new_region("temp");struct _RegionHandle*temp=& _tmp2D4;_push_region(temp);{struct
Cyc_List_List*_tmp2D5=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(
struct _tuple14*))Cyc_Core_snd,_tmp173);union Cyc_CfFlowInfo_FlowInfo_union _tmp2D7;
struct Cyc_List_List*_tmp2D8;struct _tuple11 _tmp2D6=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp2D5,0);_tmp2D7=_tmp2D6.f1;_tmp2D8=_tmp2D6.f2;_tmp2D7=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2D7);for(0;_tmp2D8 != 0;(_tmp2D8=_tmp2D8->tl,_tmp2D5=_tmp2D5->tl)){
_tmp2D7=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp2D5))->hd)->loc,_tmp2D7,(void*)_tmp2D8->hd);}{struct _tuple5
_tmp716;struct _tuple5 _tmp2DA=(_tmp716.f1=_tmp2D7,((_tmp716.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp127,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp127->unknown_all),
_tmp716)));_npop_handler(0);return _tmp2DA;}};_pop_region(temp);}_LL12F: if(*((int*)
_tmp12E)!= 29)goto _LL131;_tmp174=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp12E)->f1;_tmp175=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp12E)->f2;
_tmp176=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp12E)->f3;_tmp177=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp12E)->f4;_LL130: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp2DC;void*_tmp2DD;struct _tuple5 _tmp2DB=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp175);_tmp2DC=_tmp2DB.f1;_tmp2DD=_tmp2DB.f2;_tmp2DC=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp175->loc,_tmp2DC);{union Cyc_CfFlowInfo_FlowInfo_union _tmp2DE=_tmp2DC;struct
Cyc_Dict_Dict _tmp2DF;struct Cyc_List_List*_tmp2E0;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp2E1;_LL1D8: if((_tmp2DE.BottomFL).tag != 0)goto _LL1DA;_LL1D9: {struct _tuple5
_tmp717;return(_tmp717.f1=_tmp2DC,((_tmp717.f2=(void*)_tmp127->unknown_all,
_tmp717)));}_LL1DA: if((_tmp2DE.ReachableFL).tag != 1)goto _LL1D7;_tmp2DF=(_tmp2DE.ReachableFL).f1;
_tmp2E0=(_tmp2DE.ReachableFL).f2;_tmp2E1=(_tmp2DE.ReachableFL).f3;_LL1DB: if(Cyc_CfFlowInfo_initlevel(
_tmp2DF,_tmp2DD)== (void*)0){const char*_tmp71A;void*_tmp719;(_tmp719=0,Cyc_Tcutil_terr(
_tmp175->loc,((_tmp71A="expression may not be initialized",_tag_dynforward(
_tmp71A,sizeof(char),_get_zero_arr_size_char(_tmp71A,34)))),_tag_dynforward(
_tmp719,sizeof(void*),0)));}{struct Cyc_List_List*new_relns=_tmp2E0;comp_loop: {
void*_tmp2E5=(void*)_tmp175->r;struct Cyc_Absyn_Exp*_tmp2E6;void*_tmp2E7;struct
Cyc_Absyn_Vardecl*_tmp2E8;void*_tmp2E9;struct Cyc_Absyn_Vardecl*_tmp2EA;void*
_tmp2EB;struct Cyc_Absyn_Vardecl*_tmp2EC;void*_tmp2ED;struct Cyc_Absyn_Vardecl*
_tmp2EE;union Cyc_Absyn_Cnst_union _tmp2EF;int _tmp2F0;void*_tmp2F1;struct Cyc_List_List*
_tmp2F2;struct Cyc_List_List _tmp2F3;struct Cyc_Absyn_Exp*_tmp2F4;_LL1DD: if(*((int*)
_tmp2E5)!= 15)goto _LL1DF;_tmp2E6=((struct Cyc_Absyn_Cast_e_struct*)_tmp2E5)->f2;
_LL1DE: _tmp175=_tmp2E6;goto comp_loop;_LL1DF: if(*((int*)_tmp2E5)!= 1)goto _LL1E1;
_tmp2E7=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2E5)->f2;if(_tmp2E7 <= (void*)
1)goto _LL1E1;if(*((int*)_tmp2E7)!= 0)goto _LL1E1;_tmp2E8=((struct Cyc_Absyn_Global_b_struct*)
_tmp2E7)->f1;if(!(!_tmp2E8->escapes))goto _LL1E1;_LL1E0: _tmp2EA=_tmp2E8;goto
_LL1E2;_LL1E1: if(*((int*)_tmp2E5)!= 1)goto _LL1E3;_tmp2E9=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2E5)->f2;if(_tmp2E9 <= (void*)1)goto _LL1E3;if(*((int*)_tmp2E9)!= 3)goto _LL1E3;
_tmp2EA=((struct Cyc_Absyn_Local_b_struct*)_tmp2E9)->f1;if(!(!_tmp2EA->escapes))
goto _LL1E3;_LL1E2: _tmp2EC=_tmp2EA;goto _LL1E4;_LL1E3: if(*((int*)_tmp2E5)!= 1)goto
_LL1E5;_tmp2EB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2E5)->f2;if(_tmp2EB <= (
void*)1)goto _LL1E5;if(*((int*)_tmp2EB)!= 4)goto _LL1E5;_tmp2EC=((struct Cyc_Absyn_Pat_b_struct*)
_tmp2EB)->f1;if(!(!_tmp2EC->escapes))goto _LL1E5;_LL1E4: _tmp2EE=_tmp2EC;goto
_LL1E6;_LL1E5: if(*((int*)_tmp2E5)!= 1)goto _LL1E7;_tmp2ED=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2E5)->f2;if(_tmp2ED <= (void*)1)goto _LL1E7;if(*((int*)_tmp2ED)!= 2)goto _LL1E7;
_tmp2EE=((struct Cyc_Absyn_Param_b_struct*)_tmp2ED)->f1;if(!(!_tmp2EE->escapes))
goto _LL1E7;_LL1E6:{struct Cyc_CfFlowInfo_Reln*_tmp720;union Cyc_CfFlowInfo_RelnOp_union
_tmp71F;struct Cyc_List_List*_tmp71E;new_relns=((_tmp71E=_region_malloc(env->r,
sizeof(*_tmp71E)),((_tmp71E->hd=((_tmp720=_region_malloc(env->r,sizeof(*_tmp720)),((
_tmp720->vd=_tmp174,((_tmp720->rop=(union Cyc_CfFlowInfo_RelnOp_union)(((_tmp71F.LessVar).tag=
1,(((_tmp71F.LessVar).f1=_tmp2EE,_tmp71F)))),_tmp720)))))),((_tmp71E->tl=_tmp2E0,
_tmp71E))))));}goto _LL1DC;_LL1E7: if(*((int*)_tmp2E5)!= 0)goto _LL1E9;_tmp2EF=((
struct Cyc_Absyn_Const_e_struct*)_tmp2E5)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp2E5)->f1).Int_c).tag != 2)goto _LL1E9;_tmp2F0=(_tmp2EF.Int_c).f2;_LL1E8:{
struct Cyc_CfFlowInfo_Reln*_tmp726;union Cyc_CfFlowInfo_RelnOp_union _tmp725;struct
Cyc_List_List*_tmp724;new_relns=((_tmp724=_region_malloc(env->r,sizeof(*_tmp724)),((
_tmp724->hd=((_tmp726=_region_malloc(env->r,sizeof(*_tmp726)),((_tmp726->vd=
_tmp174,((_tmp726->rop=(union Cyc_CfFlowInfo_RelnOp_union)(((_tmp725.LessConst).tag=
3,(((_tmp725.LessConst).f1=(unsigned int)_tmp2F0,_tmp725)))),_tmp726)))))),((
_tmp724->tl=_tmp2E0,_tmp724))))));}goto _LL1DC;_LL1E9: if(*((int*)_tmp2E5)!= 3)
goto _LL1EB;_tmp2F1=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp2E5)->f1;if((
int)_tmp2F1 != 19)goto _LL1EB;_tmp2F2=((struct Cyc_Absyn_Primop_e_struct*)_tmp2E5)->f2;
if(_tmp2F2 == 0)goto _LL1EB;_tmp2F3=*_tmp2F2;_tmp2F4=(struct Cyc_Absyn_Exp*)_tmp2F3.hd;
_LL1EA:{void*_tmp2FB=(void*)_tmp2F4->r;void*_tmp2FC;struct Cyc_Absyn_Vardecl*
_tmp2FD;void*_tmp2FE;struct Cyc_Absyn_Vardecl*_tmp2FF;void*_tmp300;struct Cyc_Absyn_Vardecl*
_tmp301;void*_tmp302;struct Cyc_Absyn_Vardecl*_tmp303;_LL1EE: if(*((int*)_tmp2FB)
!= 1)goto _LL1F0;_tmp2FC=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2FB)->f2;if(
_tmp2FC <= (void*)1)goto _LL1F0;if(*((int*)_tmp2FC)!= 0)goto _LL1F0;_tmp2FD=((
struct Cyc_Absyn_Global_b_struct*)_tmp2FC)->f1;if(!(!_tmp2FD->escapes))goto _LL1F0;
_LL1EF: _tmp2FF=_tmp2FD;goto _LL1F1;_LL1F0: if(*((int*)_tmp2FB)!= 1)goto _LL1F2;
_tmp2FE=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2FB)->f2;if(_tmp2FE <= (void*)
1)goto _LL1F2;if(*((int*)_tmp2FE)!= 3)goto _LL1F2;_tmp2FF=((struct Cyc_Absyn_Local_b_struct*)
_tmp2FE)->f1;if(!(!_tmp2FF->escapes))goto _LL1F2;_LL1F1: _tmp301=_tmp2FF;goto
_LL1F3;_LL1F2: if(*((int*)_tmp2FB)!= 1)goto _LL1F4;_tmp300=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2FB)->f2;if(_tmp300 <= (void*)1)goto _LL1F4;if(*((int*)_tmp300)!= 4)goto _LL1F4;
_tmp301=((struct Cyc_Absyn_Pat_b_struct*)_tmp300)->f1;if(!(!_tmp301->escapes))
goto _LL1F4;_LL1F3: _tmp303=_tmp301;goto _LL1F5;_LL1F4: if(*((int*)_tmp2FB)!= 1)goto
_LL1F6;_tmp302=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2FB)->f2;if(_tmp302 <= (
void*)1)goto _LL1F6;if(*((int*)_tmp302)!= 2)goto _LL1F6;_tmp303=((struct Cyc_Absyn_Param_b_struct*)
_tmp302)->f1;if(!(!_tmp303->escapes))goto _LL1F6;_LL1F5:{struct Cyc_CfFlowInfo_Reln*
_tmp72C;union Cyc_CfFlowInfo_RelnOp_union _tmp72B;struct Cyc_List_List*_tmp72A;
new_relns=((_tmp72A=_region_malloc(env->r,sizeof(*_tmp72A)),((_tmp72A->hd=((
_tmp72C=_region_malloc(env->r,sizeof(*_tmp72C)),((_tmp72C->vd=_tmp174,((_tmp72C->rop=(
union Cyc_CfFlowInfo_RelnOp_union)(((_tmp72B.LessSize).tag=2,(((_tmp72B.LessSize).f1=
_tmp303,_tmp72B)))),_tmp72C)))))),((_tmp72A->tl=_tmp2E0,_tmp72A))))));}goto
_LL1ED;_LL1F6:;_LL1F7: goto _LL1ED;_LL1ED:;}goto _LL1DC;_LL1EB:;_LL1EC: goto _LL1DC;
_LL1DC:;}if(_tmp2E0 != new_relns){union Cyc_CfFlowInfo_FlowInfo_union _tmp72D;
_tmp2DC=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp72D.ReachableFL).tag=1,(((
_tmp72D.ReachableFL).f1=_tmp2DF,(((_tmp72D.ReachableFL).f2=new_relns,(((_tmp72D.ReachableFL).f3=
_tmp2E1,_tmp72D))))))));}{void*_tmp308=_tmp2DD;_LL1F9: if((int)_tmp308 != 0)goto
_LL1FB;_LL1FA: {struct _tuple5 _tmp72E;return(_tmp72E.f1=_tmp2DC,((_tmp72E.f2=(
void*)_tmp127->unknown_all,_tmp72E)));}_LL1FB: if((int)_tmp308 != 2)goto _LL1FD;
_LL1FC: goto _LL1FE;_LL1FD: if((int)_tmp308 != 1)goto _LL1FF;_LL1FE: goto _LL200;_LL1FF:
if(_tmp308 <= (void*)3)goto _LL201;if(*((int*)_tmp308)!= 2)goto _LL201;_LL200: {
struct Cyc_List_List _tmp72F;struct Cyc_List_List _tmp30A=(_tmp72F.hd=_tmp174,((
_tmp72F.tl=0,_tmp72F)));_tmp2DC=Cyc_NewControlFlow_add_vars(_tmp127,_tmp2DC,(
struct Cyc_List_List*)& _tmp30A,(void*)_tmp127->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp30C;void*_tmp30D;struct _tuple5 _tmp30B=Cyc_NewControlFlow_anal_Rexp(env,
_tmp2DC,_tmp176);_tmp30C=_tmp30B.f1;_tmp30D=_tmp30B.f2;_tmp30C=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp176->loc,_tmp30C);{union Cyc_CfFlowInfo_FlowInfo_union _tmp30E=_tmp30C;struct
Cyc_Dict_Dict _tmp30F;struct Cyc_CfFlowInfo_ConsumeInfo _tmp310;_LL204: if((_tmp30E.BottomFL).tag
!= 0)goto _LL206;_LL205: {struct _tuple5 _tmp730;return(_tmp730.f1=_tmp30C,((
_tmp730.f2=(void*)_tmp127->unknown_all,_tmp730)));}_LL206: if((_tmp30E.ReachableFL).tag
!= 1)goto _LL203;_tmp30F=(_tmp30E.ReachableFL).f1;_tmp310=(_tmp30E.ReachableFL).f3;
_LL207: if(Cyc_CfFlowInfo_initlevel(_tmp30F,_tmp30D)!= (void*)2){{const char*
_tmp733;void*_tmp732;(_tmp732=0,Cyc_Tcutil_terr(_tmp175->loc,((_tmp733="expression may not be initialized",
_tag_dynforward(_tmp733,sizeof(char),_get_zero_arr_size_char(_tmp733,34)))),
_tag_dynforward(_tmp732,sizeof(void*),0)));}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp736;struct _tuple5 _tmp735;return(_tmp735.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp736.BottomFL).tag=0,_tmp736)),((_tmp735.f2=(void*)_tmp127->unknown_all,
_tmp735)));}}_LL203:;}_tmp2DC=_tmp30C;goto _LL202;}}_LL201:;_LL202: while(1){
struct Cyc_List_List _tmp737;struct Cyc_List_List _tmp317=(_tmp737.hd=_tmp174,((
_tmp737.tl=0,_tmp737)));_tmp2DC=Cyc_NewControlFlow_add_vars(_tmp127,_tmp2DC,(
struct Cyc_List_List*)& _tmp317,(void*)_tmp127->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp319;void*_tmp31A;struct _tuple5 _tmp318=Cyc_NewControlFlow_anal_Rexp(env,
_tmp2DC,_tmp176);_tmp319=_tmp318.f1;_tmp31A=_tmp318.f2;_tmp319=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp176->loc,_tmp319);{union Cyc_CfFlowInfo_FlowInfo_union _tmp31B=_tmp319;struct
Cyc_Dict_Dict _tmp31C;struct Cyc_CfFlowInfo_ConsumeInfo _tmp31D;_LL209: if((_tmp31B.BottomFL).tag
!= 0)goto _LL20B;_LL20A: goto _LL208;_LL20B: if((_tmp31B.ReachableFL).tag != 1)goto
_LL208;_tmp31C=(_tmp31B.ReachableFL).f1;_tmp31D=(_tmp31B.ReachableFL).f3;_LL20C:
if(Cyc_CfFlowInfo_initlevel(_tmp31C,_tmp31A)!= (void*)2){{const char*_tmp73A;void*
_tmp739;(_tmp739=0,Cyc_Tcutil_terr(_tmp175->loc,((_tmp73A="expression may not be initialized",
_tag_dynforward(_tmp73A,sizeof(char),_get_zero_arr_size_char(_tmp73A,34)))),
_tag_dynforward(_tmp739,sizeof(void*),0)));}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp73D;struct _tuple5 _tmp73C;return(_tmp73C.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp73D.BottomFL).tag=0,_tmp73D)),((_tmp73C.f2=(void*)_tmp127->unknown_all,
_tmp73C)));}}_LL208:;}{union Cyc_CfFlowInfo_FlowInfo_union _tmp322=Cyc_CfFlowInfo_join_flow(
_tmp127,env->all_changed,_tmp2DC,_tmp319,1);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp322,_tmp2DC))break;_tmp2DC=_tmp322;}}}{struct _tuple5 _tmp73E;return(_tmp73E.f1=
_tmp2DC,((_tmp73E.f2=(void*)_tmp127->unknown_all,_tmp73E)));}_LL1F8:;}}_LL1D7:;}}
_LL131: if(*((int*)_tmp12E)!= 38)goto _LL133;_tmp178=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp12E)->f1;_LL132: while(1){union Cyc_CfFlowInfo_FlowInfo_union*_tmp326;struct
_tuple9 _tmp325=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp178);_tmp326=
_tmp325.f2;inflow=*_tmp326;{void*_tmp327=(void*)_tmp178->r;struct Cyc_Absyn_Stmt*
_tmp328;struct Cyc_Absyn_Stmt*_tmp329;struct Cyc_Absyn_Decl*_tmp32A;struct Cyc_Absyn_Stmt*
_tmp32B;struct Cyc_Absyn_Exp*_tmp32C;_LL20E: if(_tmp327 <= (void*)1)goto _LL214;if(*((
int*)_tmp327)!= 1)goto _LL210;_tmp328=((struct Cyc_Absyn_Seq_s_struct*)_tmp327)->f1;
_tmp329=((struct Cyc_Absyn_Seq_s_struct*)_tmp327)->f2;_LL20F: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp328);_tmp178=_tmp329;goto _LL20D;_LL210: if(*((int*)_tmp327)!= 11)
goto _LL212;_tmp32A=((struct Cyc_Absyn_Decl_s_struct*)_tmp327)->f1;_tmp32B=((
struct Cyc_Absyn_Decl_s_struct*)_tmp327)->f2;_LL211: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp32A);_tmp178=_tmp32B;goto _LL20D;_LL212: if(*((int*)_tmp327)!= 0)
goto _LL214;_tmp32C=((struct Cyc_Absyn_Exp_s_struct*)_tmp327)->f1;_LL213: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp32C);_LL214:;_LL215: {struct Cyc_Core_Impossible_struct _tmp744;
const char*_tmp743;struct Cyc_Core_Impossible_struct*_tmp742;(int)_throw((void*)((
_tmp742=_cycalloc(sizeof(*_tmp742)),((_tmp742[0]=((_tmp744.tag=Cyc_Core_Impossible,((
_tmp744.f1=((_tmp743="analyze_Rexp: ill-formed StmtExp",_tag_dynforward(_tmp743,
sizeof(char),_get_zero_arr_size_char(_tmp743,33)))),_tmp744)))),_tmp742)))));}
_LL20D:;}}_LL133: if(*((int*)_tmp12E)!= 1)goto _LL135;_tmp179=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp12E)->f2;if((int)_tmp179 != 0)goto _LL135;_LL134: goto _LL136;_LL135: if(*((int*)
_tmp12E)!= 2)goto _LL137;_LL136: goto _LL138;_LL137: if(*((int*)_tmp12E)!= 10)goto
_LL139;_LL138: goto _LL13A;_LL139: if(*((int*)_tmp12E)!= 37)goto _LL13B;_LL13A: goto
_LL13C;_LL13B: if(*((int*)_tmp12E)!= 27)goto _LLD6;_LL13C: {struct Cyc_Core_Impossible_struct
_tmp74A;const char*_tmp749;struct Cyc_Core_Impossible_struct*_tmp748;(int)_throw((
void*)((_tmp748=_cycalloc(sizeof(*_tmp748)),((_tmp748[0]=((_tmp74A.tag=Cyc_Core_Impossible,((
_tmp74A.f1=((_tmp749="anal_Rexp, unexpected exp form",_tag_dynforward(_tmp749,
sizeof(char),_get_zero_arr_size_char(_tmp749,31)))),_tmp74A)))),_tmp748)))));}
_LLD6:;}}static struct _tuple7 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo_union
f,void*r,struct Cyc_List_List*flds);static struct _tuple7 Cyc_NewControlFlow_anal_derefL(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo_union f,void*r,struct Cyc_List_List*
flds){struct Cyc_CfFlowInfo_FlowEnv*_tmp333=env->fenv;void*_tmp334=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp335;void*_tmp336;struct Cyc_Absyn_PtrAtts _tmp337;struct Cyc_Absyn_Conref*
_tmp338;struct Cyc_Absyn_Conref*_tmp339;_LL217: if(_tmp334 <= (void*)4)goto _LL219;
if(*((int*)_tmp334)!= 4)goto _LL219;_tmp335=((struct Cyc_Absyn_PointerType_struct*)
_tmp334)->f1;_tmp336=(void*)_tmp335.elt_typ;_tmp337=_tmp335.ptr_atts;_tmp338=
_tmp337.bounds;_tmp339=_tmp337.zero_term;_LL218: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp33A=f;struct Cyc_Dict_Dict _tmp33B;struct Cyc_List_List*_tmp33C;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp33D;_LL21C: if((_tmp33A.BottomFL).tag != 0)goto _LL21E;_LL21D: {union Cyc_CfFlowInfo_AbsLVal_union
_tmp74D;struct _tuple7 _tmp74C;return(_tmp74C.f1=f,((_tmp74C.f2=(union Cyc_CfFlowInfo_AbsLVal_union)(((
_tmp74D.UnknownL).tag=1,_tmp74D)),_tmp74C)));}_LL21E: if((_tmp33A.ReachableFL).tag
!= 1)goto _LL21B;_tmp33B=(_tmp33A.ReachableFL).f1;_tmp33C=(_tmp33A.ReachableFL).f2;
_tmp33D=(_tmp33A.ReachableFL).f3;_LL21F: if(Cyc_Tcutil_is_bound_one(_tmp338)){
void*_tmp340=r;struct Cyc_CfFlowInfo_Place*_tmp341;struct Cyc_CfFlowInfo_Place
_tmp342;void*_tmp343;struct Cyc_List_List*_tmp344;void*_tmp345;_LL221: if((int)
_tmp340 != 1)goto _LL223;_LL222: goto _LL224;_LL223: if((int)_tmp340 != 2)goto _LL225;
_LL224:{struct Cyc_CfFlowInfo_NotZero_struct _tmp750;struct Cyc_CfFlowInfo_NotZero_struct*
_tmp74F;(void*)(e->annot=(void*)((void*)((_tmp74F=_cycalloc(sizeof(*_tmp74F)),((
_tmp74F[0]=((_tmp750.tag=Cyc_CfFlowInfo_NotZero,((_tmp750.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp33C),_tmp750)))),_tmp74F))))));}goto _LL220;_LL225: if(
_tmp340 <= (void*)3)goto _LL227;if(*((int*)_tmp340)!= 2)goto _LL227;_tmp341=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp340)->f1;_tmp342=*_tmp341;_tmp343=(
void*)_tmp342.root;_tmp344=_tmp342.fields;_LL226:{struct Cyc_CfFlowInfo_NotZero_struct
_tmp753;struct Cyc_CfFlowInfo_NotZero_struct*_tmp752;(void*)(e->annot=(void*)((
void*)((_tmp752=_cycalloc(sizeof(*_tmp752)),((_tmp752[0]=((_tmp753.tag=Cyc_CfFlowInfo_NotZero,((
_tmp753.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp33C),_tmp753)))),
_tmp752))))));}{union Cyc_CfFlowInfo_AbsLVal_union _tmp759;struct Cyc_CfFlowInfo_Place*
_tmp758;struct _tuple7 _tmp757;return(_tmp757.f1=f,((_tmp757.f2=(union Cyc_CfFlowInfo_AbsLVal_union)(((
_tmp759.PlaceL).tag=0,(((_tmp759.PlaceL).f1=((_tmp758=_region_malloc(_tmp333->r,
sizeof(*_tmp758)),((_tmp758->root=(void*)_tmp343,((_tmp758->fields=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp333->r,_tmp344,flds),_tmp758)))))),_tmp759)))),_tmp757)));}_LL227: if((int)
_tmp340 != 0)goto _LL229;_LL228:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));{
union Cyc_CfFlowInfo_AbsLVal_union _tmp75E;union Cyc_CfFlowInfo_FlowInfo_union
_tmp75D;struct _tuple7 _tmp75C;return(_tmp75C.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp75D.BottomFL).tag=0,_tmp75D)),((_tmp75C.f2=(union Cyc_CfFlowInfo_AbsLVal_union)(((
_tmp75E.UnknownL).tag=1,_tmp75E)),_tmp75C)));}_LL229: if(_tmp340 <= (void*)3)goto
_LL22B;if(*((int*)_tmp340)!= 0)goto _LL22B;_tmp345=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp340)->f1;_LL22A: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp345);goto
_LL22C;_LL22B:;_LL22C: {struct Cyc_CfFlowInfo_UnknownZ_struct _tmp761;struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp760;(void*)(e->annot=(void*)((void*)((_tmp760=_cycalloc(sizeof(*_tmp760)),((
_tmp760[0]=((_tmp761.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp761.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp33C),_tmp761)))),_tmp760))))));}_LL220:;}else{struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp764;struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp763;(void*)(e->annot=(void*)((
void*)((_tmp763=_cycalloc(sizeof(*_tmp763)),((_tmp763[0]=((_tmp764.tag=Cyc_CfFlowInfo_UnknownZ,((
_tmp764.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp33C),_tmp764)))),
_tmp763))))));}if(Cyc_CfFlowInfo_initlevel(_tmp33B,r)== (void*)0){const char*
_tmp767;void*_tmp766;(_tmp766=0,Cyc_Tcutil_terr(e->loc,((_tmp767="dereference of possibly uninitialized pointer",
_tag_dynforward(_tmp767,sizeof(char),_get_zero_arr_size_char(_tmp767,46)))),
_tag_dynforward(_tmp766,sizeof(void*),0)));}{union Cyc_CfFlowInfo_AbsLVal_union
_tmp76A;struct _tuple7 _tmp769;return(_tmp769.f1=f,((_tmp769.f2=(union Cyc_CfFlowInfo_AbsLVal_union)(((
_tmp76A.UnknownL).tag=1,_tmp76A)),_tmp769)));}_LL21B:;}_LL219:;_LL21A: {struct
Cyc_Core_Impossible_struct _tmp770;const char*_tmp76F;struct Cyc_Core_Impossible_struct*
_tmp76E;(int)_throw((void*)((_tmp76E=_cycalloc(sizeof(*_tmp76E)),((_tmp76E[0]=((
_tmp770.tag=Cyc_Core_Impossible,((_tmp770.f1=((_tmp76F="left deref of non-pointer-type",
_tag_dynforward(_tmp76F,sizeof(char),_get_zero_arr_size_char(_tmp76F,31)))),
_tmp770)))),_tmp76E)))));}_LL216:;}static struct _tuple7 Cyc_NewControlFlow_anal_Lexp_rec(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds);static struct _tuple7 Cyc_NewControlFlow_anal_Lexp_rec(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){struct Cyc_Dict_Dict d;struct Cyc_CfFlowInfo_FlowEnv*
_tmp35B=env->fenv;{union Cyc_CfFlowInfo_FlowInfo_union _tmp35C=inflow;struct Cyc_Dict_Dict
_tmp35D;struct Cyc_List_List*_tmp35E;_LL22E: if((_tmp35C.BottomFL).tag != 0)goto
_LL230;_LL22F: {union Cyc_CfFlowInfo_AbsLVal_union _tmp775;union Cyc_CfFlowInfo_FlowInfo_union
_tmp774;struct _tuple7 _tmp773;return(_tmp773.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp774.BottomFL).tag=0,_tmp774)),((_tmp773.f2=(union Cyc_CfFlowInfo_AbsLVal_union)(((
_tmp775.UnknownL).tag=1,_tmp775)),_tmp773)));}_LL230: if((_tmp35C.ReachableFL).tag
!= 1)goto _LL22D;_tmp35D=(_tmp35C.ReachableFL).f1;_tmp35E=(_tmp35C.ReachableFL).f2;
_LL231: d=_tmp35D;_LL22D:;}{void*_tmp362=(void*)e->r;struct Cyc_Absyn_Exp*_tmp363;
struct Cyc_Absyn_Exp*_tmp364;struct Cyc_Absyn_Exp*_tmp365;void*_tmp366;struct Cyc_Absyn_Vardecl*
_tmp367;void*_tmp368;struct Cyc_Absyn_Vardecl*_tmp369;void*_tmp36A;struct Cyc_Absyn_Vardecl*
_tmp36B;void*_tmp36C;struct Cyc_Absyn_Vardecl*_tmp36D;struct Cyc_Absyn_Exp*_tmp36E;
struct _dynforward_ptr*_tmp36F;struct Cyc_Absyn_Exp*_tmp370;struct Cyc_Absyn_Exp*
_tmp371;struct Cyc_Absyn_Exp*_tmp372;struct Cyc_Absyn_Exp*_tmp373;struct
_dynforward_ptr*_tmp374;_LL233: if(*((int*)_tmp362)!= 15)goto _LL235;_tmp363=((
struct Cyc_Absyn_Cast_e_struct*)_tmp362)->f2;_LL234: _tmp364=_tmp363;goto _LL236;
_LL235: if(*((int*)_tmp362)!= 13)goto _LL237;_tmp364=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp362)->f1;_LL236: _tmp365=_tmp364;goto _LL238;_LL237: if(*((int*)_tmp362)!= 14)
goto _LL239;_tmp365=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp362)->f1;_LL238:
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp365,flds);_LL239: if(*((int*)
_tmp362)!= 1)goto _LL23B;_tmp366=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp362)->f2;
if(_tmp366 <= (void*)1)goto _LL23B;if(*((int*)_tmp366)!= 2)goto _LL23B;_tmp367=((
struct Cyc_Absyn_Param_b_struct*)_tmp366)->f1;_LL23A: _tmp369=_tmp367;goto _LL23C;
_LL23B: if(*((int*)_tmp362)!= 1)goto _LL23D;_tmp368=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp362)->f2;if(_tmp368 <= (void*)1)goto _LL23D;if(*((int*)_tmp368)!= 3)goto _LL23D;
_tmp369=((struct Cyc_Absyn_Local_b_struct*)_tmp368)->f1;_LL23C: _tmp36B=_tmp369;
goto _LL23E;_LL23D: if(*((int*)_tmp362)!= 1)goto _LL23F;_tmp36A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp362)->f2;if(_tmp36A <= (void*)1)goto _LL23F;if(*((int*)_tmp36A)!= 4)goto _LL23F;
_tmp36B=((struct Cyc_Absyn_Pat_b_struct*)_tmp36A)->f1;_LL23E: {union Cyc_CfFlowInfo_AbsLVal_union
_tmp784;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp783;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp782;struct Cyc_CfFlowInfo_Place*_tmp781;struct _tuple7 _tmp780;return(_tmp780.f1=
inflow,((_tmp780.f2=(union Cyc_CfFlowInfo_AbsLVal_union)(((_tmp784.PlaceL).tag=0,(((
_tmp784.PlaceL).f1=((_tmp781=_region_malloc(env->r,sizeof(*_tmp781)),((_tmp781->root=(
void*)((void*)((_tmp783=_region_malloc(env->r,sizeof(*_tmp783)),((_tmp783[0]=((
_tmp782.tag=0,((_tmp782.f1=_tmp36B,_tmp782)))),_tmp783))))),((_tmp781->fields=
flds,_tmp781)))))),_tmp784)))),_tmp780)));}_LL23F: if(*((int*)_tmp362)!= 1)goto
_LL241;_tmp36C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp362)->f2;if(_tmp36C <= (
void*)1)goto _LL241;if(*((int*)_tmp36C)!= 0)goto _LL241;_tmp36D=((struct Cyc_Absyn_Global_b_struct*)
_tmp36C)->f1;_LL240: {union Cyc_CfFlowInfo_AbsLVal_union _tmp787;struct _tuple7
_tmp786;return(_tmp786.f1=inflow,((_tmp786.f2=(union Cyc_CfFlowInfo_AbsLVal_union)(((
_tmp787.UnknownL).tag=1,_tmp787)),_tmp786)));}_LL241: if(*((int*)_tmp362)!= 24)
goto _LL243;_tmp36E=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp362)->f1;_tmp36F=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp362)->f2;_LL242:{void*_tmp37C=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp36E->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp37D;void*_tmp37E;_LL24C: if(_tmp37C <= (void*)4)goto _LL24E;if(*((int*)_tmp37C)
!= 4)goto _LL24E;_tmp37D=((struct Cyc_Absyn_PointerType_struct*)_tmp37C)->f1;
_tmp37E=(void*)_tmp37D.elt_typ;_LL24D: if(!Cyc_Absyn_is_union_type(_tmp37E)){
struct Cyc_List_List*_tmp788;flds=((_tmp788=_region_malloc(env->r,sizeof(*_tmp788)),((
_tmp788->hd=(void*)Cyc_NewControlFlow_get_field_index(_tmp37E,_tmp36F),((_tmp788->tl=
flds,_tmp788))))));}goto _LL24B;_LL24E:;_LL24F: {struct Cyc_Core_Impossible_struct
_tmp78E;const char*_tmp78D;struct Cyc_Core_Impossible_struct*_tmp78C;(int)_throw((
void*)((_tmp78C=_cycalloc(sizeof(*_tmp78C)),((_tmp78C[0]=((_tmp78E.tag=Cyc_Core_Impossible,((
_tmp78E.f1=((_tmp78D="anal_Rexp: AggrArrow ptr",_tag_dynforward(_tmp78D,sizeof(
char),_get_zero_arr_size_char(_tmp78D,25)))),_tmp78E)))),_tmp78C)))));}_LL24B:;}
_tmp370=_tmp36E;goto _LL244;_LL243: if(*((int*)_tmp362)!= 22)goto _LL245;_tmp370=((
struct Cyc_Absyn_Deref_e_struct*)_tmp362)->f1;_LL244: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp384;void*_tmp385;struct _tuple5 _tmp383=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp370);_tmp384=_tmp383.f1;_tmp385=_tmp383.f2;_tmp384=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp384);return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp370,_tmp384,
_tmp385,flds);}_LL245: if(*((int*)_tmp362)!= 25)goto _LL247;_tmp371=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp362)->f1;_tmp372=((struct Cyc_Absyn_Subscript_e_struct*)_tmp362)->f2;_LL246: {
void*_tmp386=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp371->topt))->v);struct Cyc_Absyn_PtrInfo _tmp387;struct Cyc_Absyn_PtrAtts
_tmp388;struct Cyc_Absyn_Conref*_tmp389;_LL251: if(_tmp386 <= (void*)4)goto _LL255;
if(*((int*)_tmp386)!= 9)goto _LL253;_LL252: {unsigned int _tmp38A=(Cyc_Evexp_eval_const_uint_exp(
_tmp372)).f1;struct Cyc_List_List*_tmp78F;return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,_tmp371,((_tmp78F=_region_malloc(env->r,sizeof(*_tmp78F)),((_tmp78F->hd=(
void*)((int)_tmp38A),((_tmp78F->tl=flds,_tmp78F)))))));}_LL253: if(*((int*)
_tmp386)!= 4)goto _LL255;_tmp387=((struct Cyc_Absyn_PointerType_struct*)_tmp386)->f1;
_tmp388=_tmp387.ptr_atts;_tmp389=_tmp388.bounds;_LL254: {struct _RegionHandle
_tmp38C=_new_region("temp");struct _RegionHandle*temp=& _tmp38C;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp38F;struct Cyc_List_List*_tmp390;struct Cyc_Absyn_Exp*
_tmp790[2];struct _tuple11 _tmp38E=Cyc_NewControlFlow_anal_unordered_Rexps(temp,
env,inflow,((_tmp790[1]=_tmp372,((_tmp790[0]=_tmp371,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(temp,_tag_dynforward(
_tmp790,sizeof(struct Cyc_Absyn_Exp*),2)))))),1);_tmp38F=_tmp38E.f1;_tmp390=
_tmp38E.f2;_tmp38F=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp372->loc,_tmp38F);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp391=_tmp38F;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp392=_tmp38F;struct Cyc_Dict_Dict _tmp393;struct Cyc_List_List*_tmp394;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp395;_LL258: if((_tmp392.ReachableFL).tag != 1)goto _LL25A;_tmp393=(_tmp392.ReachableFL).f1;
_tmp394=(_tmp392.ReachableFL).f2;_tmp395=(_tmp392.ReachableFL).f3;_LL259: if(Cyc_CfFlowInfo_initlevel(
_tmp393,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp390))->tl))->hd)== (void*)0){const char*_tmp793;void*_tmp792;(
_tmp792=0,Cyc_Tcutil_terr(_tmp372->loc,((_tmp793="expression may not be initialized",
_tag_dynforward(_tmp793,sizeof(char),_get_zero_arr_size_char(_tmp793,34)))),
_tag_dynforward(_tmp792,sizeof(void*),0)));}{struct Cyc_List_List*_tmp398=Cyc_NewControlFlow_add_subscript_reln(
_tmp35B->r,_tmp394,_tmp371,_tmp372);if(_tmp394 != _tmp398){union Cyc_CfFlowInfo_FlowInfo_union
_tmp794;_tmp391=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp794.ReachableFL).tag=
1,(((_tmp794.ReachableFL).f1=_tmp393,(((_tmp794.ReachableFL).f2=_tmp398,(((
_tmp794.ReachableFL).f3=_tmp395,_tmp794))))))));}goto _LL257;}_LL25A:;_LL25B: goto
_LL257;_LL257:;}{union Cyc_Absyn_Constraint_union _tmp39A=(Cyc_Absyn_compress_conref(
_tmp389))->v;void*_tmp39B;_LL25D: if((_tmp39A.Eq_constr).tag != 0)goto _LL25F;
_tmp39B=(_tmp39A.Eq_constr).f1;if(_tmp39B <= (void*)2)goto _LL25F;if(*((int*)
_tmp39B)!= 1)goto _LL25F;_LL25E:{void*_tmp39C=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp390))->tl))->hd;struct Cyc_List_List*
_tmp39D;_LL262: if(_tmp39C <= (void*)3)goto _LL264;if(*((int*)_tmp39C)!= 3)goto
_LL264;_tmp39D=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp39C)->f1;_LL263:{
struct Cyc_CfFlowInfo_HasTagCmps_struct _tmp797;struct Cyc_CfFlowInfo_HasTagCmps_struct*
_tmp796;(void*)(_tmp372->annot=(void*)((void*)((_tmp796=_cycalloc(sizeof(*
_tmp796)),((_tmp796[0]=((_tmp797.tag=Cyc_CfFlowInfo_HasTagCmps,((_tmp797.f1=Cyc_CfFlowInfo_copy_tagcmps(
Cyc_Core_heap_region,_tmp39D),_tmp797)))),_tmp796))))));}goto _LL261;_LL264:;
_LL265: goto _LL261;_LL261:;}goto _LL25C;_LL25F:;_LL260: goto _LL25C;_LL25C:;}{union
Cyc_CfFlowInfo_FlowInfo_union _tmp3A1;union Cyc_CfFlowInfo_AbsLVal_union _tmp3A2;
struct _tuple7 _tmp3A0=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp371,_tmp38F,(
void*)((struct Cyc_List_List*)_check_null(_tmp390))->hd,flds);_tmp3A1=_tmp3A0.f1;
_tmp3A2=_tmp3A0.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp3A3=_tmp3A1;_LL267: if((
_tmp3A3.BottomFL).tag != 0)goto _LL269;_LL268: {struct _tuple7 _tmp798;struct _tuple7
_tmp3A5=(_tmp798.f1=_tmp3A1,((_tmp798.f2=_tmp3A2,_tmp798)));_npop_handler(0);
return _tmp3A5;}_LL269:;_LL26A: {struct _tuple7 _tmp799;struct _tuple7 _tmp3A7=(
_tmp799.f1=_tmp391,((_tmp799.f2=_tmp3A2,_tmp799)));_npop_handler(0);return
_tmp3A7;}_LL266:;}}}};_pop_region(temp);}_LL255:;_LL256: {struct Cyc_Core_Impossible_struct
_tmp79F;const char*_tmp79E;struct Cyc_Core_Impossible_struct*_tmp79D;(int)_throw((
void*)((_tmp79D=_cycalloc(sizeof(*_tmp79D)),((_tmp79D[0]=((_tmp79F.tag=Cyc_Core_Impossible,((
_tmp79F.f1=((_tmp79E="anal_Lexp: Subscript -- bad type",_tag_dynforward(_tmp79E,
sizeof(char),_get_zero_arr_size_char(_tmp79E,33)))),_tmp79F)))),_tmp79D)))));}
_LL250:;}_LL247: if(*((int*)_tmp362)!= 23)goto _LL249;_tmp373=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp362)->f1;_tmp374=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp362)->f2;_LL248:
if(Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)_check_null(_tmp373->topt))->v)){
union Cyc_CfFlowInfo_AbsLVal_union _tmp7A2;struct _tuple7 _tmp7A1;return(_tmp7A1.f1=
inflow,((_tmp7A1.f2=(union Cyc_CfFlowInfo_AbsLVal_union)(((_tmp7A2.UnknownL).tag=
1,_tmp7A2)),_tmp7A1)));}{struct Cyc_List_List*_tmp7A3;return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,_tmp373,((_tmp7A3=_region_malloc(env->r,sizeof(*_tmp7A3)),((_tmp7A3->hd=(
void*)Cyc_NewControlFlow_get_field_index((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp373->topt))->v,_tmp374),((_tmp7A3->tl=flds,_tmp7A3)))))));}
_LL249:;_LL24A: {union Cyc_CfFlowInfo_AbsLVal_union _tmp7A8;union Cyc_CfFlowInfo_FlowInfo_union
_tmp7A7;struct _tuple7 _tmp7A6;return(_tmp7A6.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp7A7.BottomFL).tag=0,_tmp7A7)),((_tmp7A6.f2=(union Cyc_CfFlowInfo_AbsLVal_union)(((
_tmp7A8.UnknownL).tag=1,_tmp7A8)),_tmp7A6)));}_LL232:;}}static struct _tuple7 Cyc_NewControlFlow_anal_Lexp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e);static struct _tuple7 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e){union Cyc_CfFlowInfo_FlowInfo_union
_tmp3B2;union Cyc_CfFlowInfo_AbsLVal_union _tmp3B3;struct _tuple7 _tmp3B1=Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,e,0);_tmp3B2=_tmp3B1.f1;_tmp3B3=_tmp3B1.f2;{struct _tuple7 _tmp7A9;
return(_tmp7A9.f1=_tmp3B2,((_tmp7A9.f2=_tmp3B3,_tmp7A9)));}}static struct _tuple8
Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_Absyn_Exp*e);static struct _tuple8 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*_tmp3B5=env->fenv;{void*
_tmp3B6=(void*)e->r;struct Cyc_Absyn_Exp*_tmp3B7;struct Cyc_Absyn_Exp*_tmp3B8;
struct Cyc_Absyn_Exp*_tmp3B9;struct Cyc_Absyn_Exp*_tmp3BA;struct Cyc_Absyn_Exp*
_tmp3BB;struct Cyc_Absyn_Exp*_tmp3BC;struct Cyc_Absyn_Exp*_tmp3BD;struct Cyc_Absyn_Exp*
_tmp3BE;struct Cyc_Absyn_Exp*_tmp3BF;void*_tmp3C0;struct Cyc_List_List*_tmp3C1;
struct Cyc_List_List _tmp3C2;struct Cyc_Absyn_Exp*_tmp3C3;struct Cyc_List_List*
_tmp3C4;void*_tmp3C5;struct Cyc_List_List*_tmp3C6;_LL26C: if(*((int*)_tmp3B6)!= 6)
goto _LL26E;_tmp3B7=((struct Cyc_Absyn_Conditional_e_struct*)_tmp3B6)->f1;_tmp3B8=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp3B6)->f2;_tmp3B9=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp3B6)->f3;_LL26D: {union Cyc_CfFlowInfo_FlowInfo_union _tmp3C8;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3C9;struct _tuple8 _tmp3C7=Cyc_NewControlFlow_anal_test(env,inflow,_tmp3B7);
_tmp3C8=_tmp3C7.f1;_tmp3C9=_tmp3C7.f2;_tmp3C8=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3B7->loc,_tmp3C8);_tmp3C9=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3B7->loc,
_tmp3C9);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3CB;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3CC;struct _tuple8 _tmp3CA=Cyc_NewControlFlow_anal_test(env,_tmp3C8,_tmp3B8);
_tmp3CB=_tmp3CA.f1;_tmp3CC=_tmp3CA.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp3CE;
union Cyc_CfFlowInfo_FlowInfo_union _tmp3CF;struct _tuple8 _tmp3CD=Cyc_NewControlFlow_anal_test(
env,_tmp3C9,_tmp3B9);_tmp3CE=_tmp3CD.f1;_tmp3CF=_tmp3CD.f2;{struct _tuple8 _tmp7AA;
return(_tmp7AA.f1=Cyc_CfFlowInfo_join_flow(_tmp3B5,env->all_changed,_tmp3CB,
_tmp3CE,1),((_tmp7AA.f2=Cyc_CfFlowInfo_join_flow(_tmp3B5,env->all_changed,
_tmp3CC,_tmp3CF,1),_tmp7AA)));}}}}_LL26E: if(*((int*)_tmp3B6)!= 7)goto _LL270;
_tmp3BA=((struct Cyc_Absyn_And_e_struct*)_tmp3B6)->f1;_tmp3BB=((struct Cyc_Absyn_And_e_struct*)
_tmp3B6)->f2;_LL26F: {union Cyc_CfFlowInfo_FlowInfo_union _tmp3D2;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D3;struct _tuple8 _tmp3D1=Cyc_NewControlFlow_anal_test(env,inflow,_tmp3BA);
_tmp3D2=_tmp3D1.f1;_tmp3D3=_tmp3D1.f2;_tmp3D2=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3BA->loc,_tmp3D2);_tmp3D3=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3BA->loc,
_tmp3D3);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3D5;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D6;struct _tuple8 _tmp3D4=Cyc_NewControlFlow_anal_test(env,_tmp3D2,_tmp3BB);
_tmp3D5=_tmp3D4.f1;_tmp3D6=_tmp3D4.f2;_tmp3D5=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3BB->loc,_tmp3D5);_tmp3D6=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3BB->loc,
_tmp3D6);{struct _tuple8 _tmp7AB;return(_tmp7AB.f1=_tmp3D5,((_tmp7AB.f2=Cyc_CfFlowInfo_join_flow(
_tmp3B5,env->all_changed,_tmp3D3,_tmp3D6,0),_tmp7AB)));}}}_LL270: if(*((int*)
_tmp3B6)!= 8)goto _LL272;_tmp3BC=((struct Cyc_Absyn_Or_e_struct*)_tmp3B6)->f1;
_tmp3BD=((struct Cyc_Absyn_Or_e_struct*)_tmp3B6)->f2;_LL271: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D9;union Cyc_CfFlowInfo_FlowInfo_union _tmp3DA;struct _tuple8 _tmp3D8=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp3BC);_tmp3D9=_tmp3D8.f1;_tmp3DA=_tmp3D8.f2;_tmp3D9=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3BC->loc,_tmp3D9);_tmp3DA=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3BC->loc,
_tmp3DA);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3DC;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3DD;struct _tuple8 _tmp3DB=Cyc_NewControlFlow_anal_test(env,_tmp3DA,_tmp3BD);
_tmp3DC=_tmp3DB.f1;_tmp3DD=_tmp3DB.f2;_tmp3DC=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3BD->loc,_tmp3DC);_tmp3DD=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3BD->loc,
_tmp3DD);{struct _tuple8 _tmp7AC;return(_tmp7AC.f1=Cyc_CfFlowInfo_join_flow(
_tmp3B5,env->all_changed,_tmp3D9,_tmp3DC,0),((_tmp7AC.f2=_tmp3DD,_tmp7AC)));}}}
_LL272: if(*((int*)_tmp3B6)!= 9)goto _LL274;_tmp3BE=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp3B6)->f1;_tmp3BF=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp3B6)->f2;_LL273: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp3E0;void*_tmp3E1;struct _tuple5 _tmp3DF=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp3BE);_tmp3E0=_tmp3DF.f1;_tmp3E1=_tmp3DF.f2;_tmp3E0=Cyc_CfFlowInfo_drop_unique_rvals(
_tmp3BE->loc,_tmp3E0);return Cyc_NewControlFlow_anal_test(env,_tmp3E0,_tmp3BF);}
_LL274: if(*((int*)_tmp3B6)!= 3)goto _LL276;_tmp3C0=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3B6)->f1;if((int)_tmp3C0 != 11)goto _LL276;_tmp3C1=((struct Cyc_Absyn_Primop_e_struct*)
_tmp3B6)->f2;if(_tmp3C1 == 0)goto _LL276;_tmp3C2=*_tmp3C1;_tmp3C3=(struct Cyc_Absyn_Exp*)
_tmp3C2.hd;_tmp3C4=_tmp3C2.tl;if(_tmp3C4 != 0)goto _LL276;_LL275: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3E3;union Cyc_CfFlowInfo_FlowInfo_union _tmp3E4;struct _tuple8 _tmp3E2=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp3C3);_tmp3E3=_tmp3E2.f1;_tmp3E4=_tmp3E2.f2;{struct _tuple8 _tmp7AD;
return(_tmp7AD.f1=_tmp3E4,((_tmp7AD.f2=_tmp3E3,_tmp7AD)));}}_LL276: if(*((int*)
_tmp3B6)!= 3)goto _LL278;_tmp3C5=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3B6)->f1;_tmp3C6=((struct Cyc_Absyn_Primop_e_struct*)_tmp3B6)->f2;_LL277: {
void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo_union f;{struct _RegionHandle _tmp3E6=
_new_region("temp");struct _RegionHandle*temp=& _tmp3E6;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp3E8;struct Cyc_List_List*_tmp3E9;struct _tuple11
_tmp3E7=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp3C6,0);
_tmp3E8=_tmp3E7.f1;_tmp3E9=_tmp3E7.f2;r1=(void*)((struct Cyc_List_List*)
_check_null(_tmp3E9))->hd;r2=(void*)((struct Cyc_List_List*)_check_null(_tmp3E9->tl))->hd;
f=_tmp3E8;};_pop_region(temp);}{union Cyc_CfFlowInfo_FlowInfo_union _tmp3EA=f;
struct Cyc_Dict_Dict _tmp3EB;struct Cyc_List_List*_tmp3EC;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3ED;_LL27B: if((_tmp3EA.BottomFL).tag != 0)goto _LL27D;_LL27C: {struct _tuple8
_tmp7AE;return(_tmp7AE.f1=f,((_tmp7AE.f2=f,_tmp7AE)));}_LL27D: if((_tmp3EA.ReachableFL).tag
!= 1)goto _LL27A;_tmp3EB=(_tmp3EA.ReachableFL).f1;_tmp3EC=(_tmp3EA.ReachableFL).f2;
_tmp3ED=(_tmp3EA.ReachableFL).f3;_LL27E: {struct Cyc_Absyn_Exp*_tmp3EF=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3C6))->hd;struct Cyc_Absyn_Exp*_tmp3F0=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp3C6->tl))->hd;if(Cyc_CfFlowInfo_initlevel(
_tmp3EB,r1)== (void*)0){const char*_tmp7B1;void*_tmp7B0;(_tmp7B0=0,Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)_tmp3C6->hd)->loc,((_tmp7B1="expression may not be initialized",
_tag_dynforward(_tmp7B1,sizeof(char),_get_zero_arr_size_char(_tmp7B1,34)))),
_tag_dynforward(_tmp7B0,sizeof(void*),0)));}if(Cyc_CfFlowInfo_initlevel(_tmp3EB,
r2)== (void*)0){const char*_tmp7B4;void*_tmp7B3;(_tmp7B3=0,Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp3C6->tl))->hd)->loc,((
_tmp7B4="expression may not be initialized",_tag_dynforward(_tmp7B4,sizeof(char),
_get_zero_arr_size_char(_tmp7B4,34)))),_tag_dynforward(_tmp7B3,sizeof(void*),0)));}
if(_tmp3C5 == (void*)5  || _tmp3C5 == (void*)6){struct _tuple0 _tmp7B5;struct _tuple0
_tmp3F6=(_tmp7B5.f1=r1,((_tmp7B5.f2=r2,_tmp7B5)));void*_tmp3F7;void*_tmp3F8;void*
_tmp3F9;void*_tmp3FA;void*_tmp3FB;void*_tmp3FC;void*_tmp3FD;void*_tmp3FE;void*
_tmp3FF;void*_tmp400;void*_tmp401;void*_tmp402;void*_tmp403;void*_tmp404;void*
_tmp405;void*_tmp406;void*_tmp407;void*_tmp408;void*_tmp409;void*_tmp40A;_LL280:
_tmp3F7=_tmp3F6.f1;if(_tmp3F7 <= (void*)3)goto _LL282;if(*((int*)_tmp3F7)!= 0)goto
_LL282;_tmp3F8=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3F7)->f1;
_tmp3F9=_tmp3F6.f2;if((int)_tmp3F9 != 0)goto _LL282;_LL281: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp40C;union Cyc_CfFlowInfo_FlowInfo_union _tmp40D;struct _tuple8 _tmp40B=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp3EF,_tmp3F8);_tmp40C=_tmp40B.f1;_tmp40D=_tmp40B.f2;{void*_tmp40E=
_tmp3C5;_LL295: if((int)_tmp40E != 5)goto _LL297;_LL296: {struct _tuple8 _tmp7B6;
return(_tmp7B6.f1=_tmp40D,((_tmp7B6.f2=_tmp40C,_tmp7B6)));}_LL297: if((int)
_tmp40E != 6)goto _LL299;_LL298: {struct _tuple8 _tmp7B7;return(_tmp7B7.f1=_tmp40C,((
_tmp7B7.f2=_tmp40D,_tmp7B7)));}_LL299:;_LL29A: {struct Cyc_Core_Impossible_struct
_tmp7BD;const char*_tmp7BC;struct Cyc_Core_Impossible_struct*_tmp7BB;(int)_throw((
void*)((_tmp7BB=_cycalloc(sizeof(*_tmp7BB)),((_tmp7BB[0]=((_tmp7BD.tag=Cyc_Core_Impossible,((
_tmp7BD.f1=((_tmp7BC="anal_test, zero-split",_tag_dynforward(_tmp7BC,sizeof(char),
_get_zero_arr_size_char(_tmp7BC,22)))),_tmp7BD)))),_tmp7BB)))));}_LL294:;}}
_LL282: _tmp3FA=_tmp3F6.f1;if((int)_tmp3FA != 0)goto _LL284;_tmp3FB=_tmp3F6.f2;if(
_tmp3FB <= (void*)3)goto _LL284;if(*((int*)_tmp3FB)!= 0)goto _LL284;_tmp3FC=(void*)((
struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp3FB)->f1;_LL283: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp415;union Cyc_CfFlowInfo_FlowInfo_union _tmp416;struct _tuple8 _tmp414=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp3F0,_tmp3FC);_tmp415=_tmp414.f1;_tmp416=_tmp414.f2;{void*_tmp417=
_tmp3C5;_LL29C: if((int)_tmp417 != 5)goto _LL29E;_LL29D: {struct _tuple8 _tmp7BE;
return(_tmp7BE.f1=_tmp416,((_tmp7BE.f2=_tmp415,_tmp7BE)));}_LL29E: if((int)
_tmp417 != 6)goto _LL2A0;_LL29F: {struct _tuple8 _tmp7BF;return(_tmp7BF.f1=_tmp415,((
_tmp7BF.f2=_tmp416,_tmp7BF)));}_LL2A0:;_LL2A1: {struct Cyc_Core_Impossible_struct
_tmp7C5;const char*_tmp7C4;struct Cyc_Core_Impossible_struct*_tmp7C3;(int)_throw((
void*)((_tmp7C3=_cycalloc(sizeof(*_tmp7C3)),((_tmp7C3[0]=((_tmp7C5.tag=Cyc_Core_Impossible,((
_tmp7C5.f1=((_tmp7C4="anal_test, zero-split",_tag_dynforward(_tmp7C4,sizeof(char),
_get_zero_arr_size_char(_tmp7C4,22)))),_tmp7C5)))),_tmp7C3)))));}_LL29B:;}}
_LL284: _tmp3FD=_tmp3F6.f1;if((int)_tmp3FD != 0)goto _LL286;_tmp3FE=_tmp3F6.f2;if((
int)_tmp3FE != 0)goto _LL286;_LL285: if(_tmp3C5 == (void*)5){union Cyc_CfFlowInfo_FlowInfo_union
_tmp7C8;struct _tuple8 _tmp7C7;return(_tmp7C7.f1=f,((_tmp7C7.f2=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp7C8.BottomFL).tag=0,_tmp7C8)),_tmp7C7)));}else{union Cyc_CfFlowInfo_FlowInfo_union
_tmp7CB;struct _tuple8 _tmp7CA;return(_tmp7CA.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp7CB.BottomFL).tag=0,_tmp7CB)),((_tmp7CA.f2=f,_tmp7CA)));}_LL286: _tmp3FF=
_tmp3F6.f1;if((int)_tmp3FF != 0)goto _LL288;_tmp400=_tmp3F6.f2;if((int)_tmp400 != 1)
goto _LL288;_LL287: goto _LL289;_LL288: _tmp401=_tmp3F6.f1;if((int)_tmp401 != 0)goto
_LL28A;_tmp402=_tmp3F6.f2;if((int)_tmp402 != 2)goto _LL28A;_LL289: goto _LL28B;
_LL28A: _tmp403=_tmp3F6.f1;if((int)_tmp403 != 0)goto _LL28C;_tmp404=_tmp3F6.f2;if(
_tmp404 <= (void*)3)goto _LL28C;if(*((int*)_tmp404)!= 2)goto _LL28C;_LL28B: goto
_LL28D;_LL28C: _tmp405=_tmp3F6.f1;if((int)_tmp405 != 1)goto _LL28E;_tmp406=_tmp3F6.f2;
if((int)_tmp406 != 0)goto _LL28E;_LL28D: goto _LL28F;_LL28E: _tmp407=_tmp3F6.f1;if((
int)_tmp407 != 2)goto _LL290;_tmp408=_tmp3F6.f2;if((int)_tmp408 != 0)goto _LL290;
_LL28F: goto _LL291;_LL290: _tmp409=_tmp3F6.f1;if(_tmp409 <= (void*)3)goto _LL292;if(*((
int*)_tmp409)!= 2)goto _LL292;_tmp40A=_tmp3F6.f2;if((int)_tmp40A != 0)goto _LL292;
_LL291: if(_tmp3C5 == (void*)6){union Cyc_CfFlowInfo_FlowInfo_union _tmp7CE;struct
_tuple8 _tmp7CD;return(_tmp7CD.f1=f,((_tmp7CD.f2=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp7CE.BottomFL).tag=0,_tmp7CE)),_tmp7CD)));}else{union Cyc_CfFlowInfo_FlowInfo_union
_tmp7D1;struct _tuple8 _tmp7D0;return(_tmp7D0.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp7D1.BottomFL).tag=0,_tmp7D1)),((_tmp7D0.f2=f,_tmp7D0)));}_LL292:;_LL293: goto
_LL27F;_LL27F:;}{struct _tuple0 _tmp7D2;struct _tuple0 _tmp426=(_tmp7D2.f1=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3EF->topt))->v),((_tmp7D2.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3F0->topt))->v),_tmp7D2)));void*
_tmp427;void*_tmp428;void*_tmp429;void*_tmp42A;void*_tmp42B;void*_tmp42C;_LL2A3:
_tmp427=_tmp426.f1;if(_tmp427 <= (void*)4)goto _LL2A5;if(*((int*)_tmp427)!= 5)goto
_LL2A5;_tmp428=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp427)->f1;if((int)
_tmp428 != 1)goto _LL2A5;_LL2A4: goto _LL2A6;_LL2A5: _tmp429=_tmp426.f2;if(_tmp429 <= (
void*)4)goto _LL2A7;if(*((int*)_tmp429)!= 5)goto _LL2A7;_tmp42A=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp429)->f1;if((int)_tmp42A != 1)goto _LL2A7;_LL2A6: goto _LL2A8;_LL2A7: _tmp42B=
_tmp426.f1;if(_tmp42B <= (void*)4)goto _LL2A9;if(*((int*)_tmp42B)!= 18)goto _LL2A9;
_LL2A8: goto _LL2AA;_LL2A9: _tmp42C=_tmp426.f2;if(_tmp42C <= (void*)4)goto _LL2AB;if(*((
int*)_tmp42C)!= 18)goto _LL2AB;_LL2AA: goto _LL2A2;_LL2AB:;_LL2AC: {struct _tuple8
_tmp7D3;return(_tmp7D3.f1=f,((_tmp7D3.f2=f,_tmp7D3)));}_LL2A2:;}{void*_tmp42E=
_tmp3C5;_LL2AE: if((int)_tmp42E != 5)goto _LL2B0;_LL2AF: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp42F=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3EF,r1,(void*)5,r2);_tmp42F=Cyc_NewControlFlow_if_tagcmp(
env,_tmp42F,_tmp3F0,r2,(void*)5,r1);{struct _tuple8 _tmp7D4;return(_tmp7D4.f1=
_tmp42F,((_tmp7D4.f2=f,_tmp7D4)));}}_LL2B0: if((int)_tmp42E != 6)goto _LL2B2;_LL2B1: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp431=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp3EF,r1,(void*)5,r2);_tmp431=Cyc_NewControlFlow_if_tagcmp(env,_tmp431,_tmp3F0,
r2,(void*)5,r1);{struct _tuple8 _tmp7D5;return(_tmp7D5.f1=f,((_tmp7D5.f2=_tmp431,
_tmp7D5)));}}_LL2B2: if((int)_tmp42E != 7)goto _LL2B4;_LL2B3: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp433=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3F0,r2,(void*)8,r1);union Cyc_CfFlowInfo_FlowInfo_union
_tmp434=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3EF,r1,(void*)10,r2);struct
_tuple8 _tmp7D6;return(_tmp7D6.f1=_tmp433,((_tmp7D6.f2=_tmp434,_tmp7D6)));}_LL2B4:
if((int)_tmp42E != 9)goto _LL2B6;_LL2B5: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp436=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3F0,r2,(void*)10,r1);union Cyc_CfFlowInfo_FlowInfo_union
_tmp437=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3EF,r1,(void*)8,r2);struct _tuple8
_tmp7D7;return(_tmp7D7.f1=_tmp436,((_tmp7D7.f2=_tmp437,_tmp7D7)));}_LL2B6: if((
int)_tmp42E != 8)goto _LL2B8;_LL2B7: {union Cyc_CfFlowInfo_FlowInfo_union _tmp439=
Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3EF,r1,(void*)8,r2);union Cyc_CfFlowInfo_FlowInfo_union
_tmp43A=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3F0,r2,(void*)10,r1);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp43B=_tmp439;struct Cyc_Dict_Dict _tmp43C;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp43D;_LL2BD: if((_tmp43B.BottomFL).tag != 0)goto _LL2BF;_LL2BE: {struct Cyc_Core_Impossible_struct
_tmp7DD;const char*_tmp7DC;struct Cyc_Core_Impossible_struct*_tmp7DB;(int)_throw((
void*)((_tmp7DB=_cycalloc(sizeof(*_tmp7DB)),((_tmp7DB[0]=((_tmp7DD.tag=Cyc_Core_Impossible,((
_tmp7DD.f1=((_tmp7DC="anal_test, Lt",_tag_dynforward(_tmp7DC,sizeof(char),
_get_zero_arr_size_char(_tmp7DC,14)))),_tmp7DD)))),_tmp7DB)))));}_LL2BF: if((
_tmp43B.ReachableFL).tag != 1)goto _LL2BC;_tmp43C=(_tmp43B.ReachableFL).f1;_tmp43D=(
_tmp43B.ReachableFL).f3;_LL2C0: _tmp3EB=_tmp43C;_tmp3ED=_tmp43D;_LL2BC:;}{void*
_tmp441=(void*)_tmp3EF->r;void*_tmp442;struct Cyc_Absyn_Vardecl*_tmp443;void*
_tmp444;struct Cyc_Absyn_Vardecl*_tmp445;void*_tmp446;struct Cyc_Absyn_Vardecl*
_tmp447;void*_tmp448;struct Cyc_Absyn_Vardecl*_tmp449;_LL2C2: if(*((int*)_tmp441)
!= 1)goto _LL2C4;_tmp442=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp441)->f2;if(
_tmp442 <= (void*)1)goto _LL2C4;if(*((int*)_tmp442)!= 0)goto _LL2C4;_tmp443=((
struct Cyc_Absyn_Global_b_struct*)_tmp442)->f1;if(!(!_tmp443->escapes))goto _LL2C4;
_LL2C3: _tmp445=_tmp443;goto _LL2C5;_LL2C4: if(*((int*)_tmp441)!= 1)goto _LL2C6;
_tmp444=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp441)->f2;if(_tmp444 <= (void*)
1)goto _LL2C6;if(*((int*)_tmp444)!= 3)goto _LL2C6;_tmp445=((struct Cyc_Absyn_Local_b_struct*)
_tmp444)->f1;if(!(!_tmp445->escapes))goto _LL2C6;_LL2C5: _tmp447=_tmp445;goto
_LL2C7;_LL2C6: if(*((int*)_tmp441)!= 1)goto _LL2C8;_tmp446=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp441)->f2;if(_tmp446 <= (void*)1)goto _LL2C8;if(*((int*)_tmp446)!= 4)goto _LL2C8;
_tmp447=((struct Cyc_Absyn_Pat_b_struct*)_tmp446)->f1;if(!(!_tmp447->escapes))
goto _LL2C8;_LL2C7: _tmp449=_tmp447;goto _LL2C9;_LL2C8: if(*((int*)_tmp441)!= 1)goto
_LL2CA;_tmp448=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp441)->f2;if(_tmp448 <= (
void*)1)goto _LL2CA;if(*((int*)_tmp448)!= 2)goto _LL2CA;_tmp449=((struct Cyc_Absyn_Param_b_struct*)
_tmp448)->f1;if(!(!_tmp449->escapes))goto _LL2CA;_LL2C9: {void*_tmp44A=(void*)
_tmp3F0->r;void*_tmp44B;struct Cyc_Absyn_Vardecl*_tmp44C;void*_tmp44D;struct Cyc_Absyn_Vardecl*
_tmp44E;void*_tmp44F;struct Cyc_Absyn_Vardecl*_tmp450;void*_tmp451;struct Cyc_Absyn_Vardecl*
_tmp452;union Cyc_Absyn_Cnst_union _tmp453;int _tmp454;void*_tmp455;struct Cyc_List_List*
_tmp456;struct Cyc_List_List _tmp457;struct Cyc_Absyn_Exp*_tmp458;_LL2CD: if(*((int*)
_tmp44A)!= 1)goto _LL2CF;_tmp44B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp44A)->f2;
if(_tmp44B <= (void*)1)goto _LL2CF;if(*((int*)_tmp44B)!= 0)goto _LL2CF;_tmp44C=((
struct Cyc_Absyn_Global_b_struct*)_tmp44B)->f1;if(!(!_tmp44C->escapes))goto _LL2CF;
_LL2CE: _tmp44E=_tmp44C;goto _LL2D0;_LL2CF: if(*((int*)_tmp44A)!= 1)goto _LL2D1;
_tmp44D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp44A)->f2;if(_tmp44D <= (void*)
1)goto _LL2D1;if(*((int*)_tmp44D)!= 3)goto _LL2D1;_tmp44E=((struct Cyc_Absyn_Local_b_struct*)
_tmp44D)->f1;if(!(!_tmp44E->escapes))goto _LL2D1;_LL2D0: _tmp450=_tmp44E;goto
_LL2D2;_LL2D1: if(*((int*)_tmp44A)!= 1)goto _LL2D3;_tmp44F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp44A)->f2;if(_tmp44F <= (void*)1)goto _LL2D3;if(*((int*)_tmp44F)!= 4)goto _LL2D3;
_tmp450=((struct Cyc_Absyn_Pat_b_struct*)_tmp44F)->f1;if(!(!_tmp450->escapes))
goto _LL2D3;_LL2D2: _tmp452=_tmp450;goto _LL2D4;_LL2D3: if(*((int*)_tmp44A)!= 1)goto
_LL2D5;_tmp451=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp44A)->f2;if(_tmp451 <= (
void*)1)goto _LL2D5;if(*((int*)_tmp451)!= 2)goto _LL2D5;_tmp452=((struct Cyc_Absyn_Param_b_struct*)
_tmp451)->f1;if(!(!_tmp452->escapes))goto _LL2D5;_LL2D4: {struct _RegionHandle*
_tmp459=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp7E3;union Cyc_CfFlowInfo_RelnOp_union
_tmp7E2;struct Cyc_List_List*_tmp7E1;struct Cyc_List_List*_tmp45A=(_tmp7E1=
_region_malloc(_tmp459,sizeof(*_tmp7E1)),((_tmp7E1->hd=((_tmp7E3=_region_malloc(
_tmp459,sizeof(*_tmp7E3)),((_tmp7E3->vd=_tmp449,((_tmp7E3->rop=(union Cyc_CfFlowInfo_RelnOp_union)(((
_tmp7E2.LessVar).tag=1,(((_tmp7E2.LessVar).f1=_tmp452,_tmp7E2)))),_tmp7E3)))))),((
_tmp7E1->tl=_tmp3EC,_tmp7E1)))));union Cyc_CfFlowInfo_FlowInfo_union _tmp7E6;
struct _tuple8 _tmp7E5;return(_tmp7E5.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp7E6.ReachableFL).tag=1,(((_tmp7E6.ReachableFL).f1=_tmp3EB,(((_tmp7E6.ReachableFL).f2=
_tmp45A,(((_tmp7E6.ReachableFL).f3=_tmp3ED,_tmp7E6)))))))),((_tmp7E5.f2=_tmp43A,
_tmp7E5)));}_LL2D5: if(*((int*)_tmp44A)!= 0)goto _LL2D7;_tmp453=((struct Cyc_Absyn_Const_e_struct*)
_tmp44A)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp44A)->f1).Int_c).tag != 2)
goto _LL2D7;_tmp454=(_tmp453.Int_c).f2;_LL2D6: {struct _RegionHandle*_tmp460=(env->fenv)->r;
struct Cyc_CfFlowInfo_Reln*_tmp7EC;union Cyc_CfFlowInfo_RelnOp_union _tmp7EB;struct
Cyc_List_List*_tmp7EA;struct Cyc_List_List*_tmp461=(_tmp7EA=_region_malloc(
_tmp460,sizeof(*_tmp7EA)),((_tmp7EA->hd=((_tmp7EC=_region_malloc(_tmp460,sizeof(*
_tmp7EC)),((_tmp7EC->vd=_tmp449,((_tmp7EC->rop=(union Cyc_CfFlowInfo_RelnOp_union)(((
_tmp7EB.LessConst).tag=3,(((_tmp7EB.LessConst).f1=(unsigned int)_tmp454,_tmp7EB)))),
_tmp7EC)))))),((_tmp7EA->tl=_tmp3EC,_tmp7EA)))));union Cyc_CfFlowInfo_FlowInfo_union
_tmp7EF;struct _tuple8 _tmp7EE;return(_tmp7EE.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp7EF.ReachableFL).tag=1,(((_tmp7EF.ReachableFL).f1=_tmp3EB,(((_tmp7EF.ReachableFL).f2=
_tmp461,(((_tmp7EF.ReachableFL).f3=_tmp3ED,_tmp7EF)))))))),((_tmp7EE.f2=_tmp43A,
_tmp7EE)));}_LL2D7: if(*((int*)_tmp44A)!= 3)goto _LL2D9;_tmp455=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp44A)->f1;if((int)_tmp455 != 19)goto _LL2D9;_tmp456=((struct Cyc_Absyn_Primop_e_struct*)
_tmp44A)->f2;if(_tmp456 == 0)goto _LL2D9;_tmp457=*_tmp456;_tmp458=(struct Cyc_Absyn_Exp*)
_tmp457.hd;_LL2D8: {void*_tmp467=(void*)_tmp458->r;void*_tmp468;struct Cyc_Absyn_Vardecl*
_tmp469;void*_tmp46A;struct Cyc_Absyn_Vardecl*_tmp46B;void*_tmp46C;struct Cyc_Absyn_Vardecl*
_tmp46D;void*_tmp46E;struct Cyc_Absyn_Vardecl*_tmp46F;_LL2DC: if(*((int*)_tmp467)
!= 1)goto _LL2DE;_tmp468=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp467)->f2;if(
_tmp468 <= (void*)1)goto _LL2DE;if(*((int*)_tmp468)!= 0)goto _LL2DE;_tmp469=((
struct Cyc_Absyn_Global_b_struct*)_tmp468)->f1;if(!(!_tmp469->escapes))goto _LL2DE;
_LL2DD: _tmp46B=_tmp469;goto _LL2DF;_LL2DE: if(*((int*)_tmp467)!= 1)goto _LL2E0;
_tmp46A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp467)->f2;if(_tmp46A <= (void*)
1)goto _LL2E0;if(*((int*)_tmp46A)!= 3)goto _LL2E0;_tmp46B=((struct Cyc_Absyn_Local_b_struct*)
_tmp46A)->f1;if(!(!_tmp46B->escapes))goto _LL2E0;_LL2DF: _tmp46D=_tmp46B;goto
_LL2E1;_LL2E0: if(*((int*)_tmp467)!= 1)goto _LL2E2;_tmp46C=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp467)->f2;if(_tmp46C <= (void*)1)goto _LL2E2;if(*((int*)_tmp46C)!= 4)goto _LL2E2;
_tmp46D=((struct Cyc_Absyn_Pat_b_struct*)_tmp46C)->f1;if(!(!_tmp46D->escapes))
goto _LL2E2;_LL2E1: _tmp46F=_tmp46D;goto _LL2E3;_LL2E2: if(*((int*)_tmp467)!= 1)goto
_LL2E4;_tmp46E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp467)->f2;if(_tmp46E <= (
void*)1)goto _LL2E4;if(*((int*)_tmp46E)!= 2)goto _LL2E4;_tmp46F=((struct Cyc_Absyn_Param_b_struct*)
_tmp46E)->f1;if(!(!_tmp46F->escapes))goto _LL2E4;_LL2E3: {struct _RegionHandle*
_tmp470=(env->fenv)->r;struct Cyc_CfFlowInfo_Reln*_tmp7F5;union Cyc_CfFlowInfo_RelnOp_union
_tmp7F4;struct Cyc_List_List*_tmp7F3;struct Cyc_List_List*_tmp471=(_tmp7F3=
_region_malloc(_tmp470,sizeof(*_tmp7F3)),((_tmp7F3->hd=((_tmp7F5=_region_malloc(
_tmp470,sizeof(*_tmp7F5)),((_tmp7F5->vd=_tmp449,((_tmp7F5->rop=(union Cyc_CfFlowInfo_RelnOp_union)(((
_tmp7F4.LessSize).tag=2,(((_tmp7F4.LessSize).f1=_tmp46F,_tmp7F4)))),_tmp7F5)))))),((
_tmp7F3->tl=_tmp3EC,_tmp7F3)))));union Cyc_CfFlowInfo_FlowInfo_union _tmp7F8;
struct _tuple8 _tmp7F7;return(_tmp7F7.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp7F8.ReachableFL).tag=1,(((_tmp7F8.ReachableFL).f1=_tmp3EB,(((_tmp7F8.ReachableFL).f2=
_tmp471,(((_tmp7F8.ReachableFL).f3=_tmp3ED,_tmp7F8)))))))),((_tmp7F7.f2=_tmp43A,
_tmp7F7)));}_LL2E4:;_LL2E5: {struct _tuple8 _tmp7F9;return(_tmp7F9.f1=_tmp439,((
_tmp7F9.f2=_tmp43A,_tmp7F9)));}_LL2DB:;}_LL2D9:;_LL2DA: {struct _tuple8 _tmp7FA;
return(_tmp7FA.f1=_tmp439,((_tmp7FA.f2=_tmp43A,_tmp7FA)));}_LL2CC:;}_LL2CA:;
_LL2CB: {struct _tuple8 _tmp7FB;return(_tmp7FB.f1=_tmp439,((_tmp7FB.f2=_tmp43A,
_tmp7FB)));}_LL2C1:;}}_LL2B8: if((int)_tmp42E != 10)goto _LL2BA;_LL2B9: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp47A=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3EF,r1,(void*)10,r2);union Cyc_CfFlowInfo_FlowInfo_union
_tmp47B=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3F0,r1,(void*)8,r1);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp47C=_tmp47A;struct Cyc_Dict_Dict _tmp47D;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp47E;_LL2E7: if((_tmp47C.BottomFL).tag != 0)goto _LL2E9;_LL2E8: {struct Cyc_Core_Impossible_struct
_tmp801;const char*_tmp800;struct Cyc_Core_Impossible_struct*_tmp7FF;(int)_throw((
void*)((_tmp7FF=_cycalloc(sizeof(*_tmp7FF)),((_tmp7FF[0]=((_tmp801.tag=Cyc_Core_Impossible,((
_tmp801.f1=((_tmp800="anal_test, Lte",_tag_dynforward(_tmp800,sizeof(char),
_get_zero_arr_size_char(_tmp800,15)))),_tmp801)))),_tmp7FF)))));}_LL2E9: if((
_tmp47C.ReachableFL).tag != 1)goto _LL2E6;_tmp47D=(_tmp47C.ReachableFL).f1;_tmp47E=(
_tmp47C.ReachableFL).f3;_LL2EA: _tmp3EB=_tmp47D;_tmp3ED=_tmp47E;_LL2E6:;}{void*
_tmp482=(void*)_tmp3EF->r;void*_tmp483;struct Cyc_Absyn_Vardecl*_tmp484;void*
_tmp485;struct Cyc_Absyn_Vardecl*_tmp486;void*_tmp487;struct Cyc_Absyn_Vardecl*
_tmp488;void*_tmp489;struct Cyc_Absyn_Vardecl*_tmp48A;_LL2EC: if(*((int*)_tmp482)
!= 1)goto _LL2EE;_tmp483=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp482)->f2;if(
_tmp483 <= (void*)1)goto _LL2EE;if(*((int*)_tmp483)!= 0)goto _LL2EE;_tmp484=((
struct Cyc_Absyn_Global_b_struct*)_tmp483)->f1;if(!(!_tmp484->escapes))goto _LL2EE;
_LL2ED: _tmp486=_tmp484;goto _LL2EF;_LL2EE: if(*((int*)_tmp482)!= 1)goto _LL2F0;
_tmp485=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp482)->f2;if(_tmp485 <= (void*)
1)goto _LL2F0;if(*((int*)_tmp485)!= 3)goto _LL2F0;_tmp486=((struct Cyc_Absyn_Local_b_struct*)
_tmp485)->f1;if(!(!_tmp486->escapes))goto _LL2F0;_LL2EF: _tmp488=_tmp486;goto
_LL2F1;_LL2F0: if(*((int*)_tmp482)!= 1)goto _LL2F2;_tmp487=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp482)->f2;if(_tmp487 <= (void*)1)goto _LL2F2;if(*((int*)_tmp487)!= 4)goto _LL2F2;
_tmp488=((struct Cyc_Absyn_Pat_b_struct*)_tmp487)->f1;if(!(!_tmp488->escapes))
goto _LL2F2;_LL2F1: _tmp48A=_tmp488;goto _LL2F3;_LL2F2: if(*((int*)_tmp482)!= 1)goto
_LL2F4;_tmp489=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp482)->f2;if(_tmp489 <= (
void*)1)goto _LL2F4;if(*((int*)_tmp489)!= 2)goto _LL2F4;_tmp48A=((struct Cyc_Absyn_Param_b_struct*)
_tmp489)->f1;if(!(!_tmp48A->escapes))goto _LL2F4;_LL2F3: {void*_tmp48B=(void*)
_tmp3F0->r;void*_tmp48C;struct Cyc_List_List*_tmp48D;struct Cyc_List_List _tmp48E;
struct Cyc_Absyn_Exp*_tmp48F;_LL2F7: if(*((int*)_tmp48B)!= 3)goto _LL2F9;_tmp48C=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp48B)->f1;if((int)_tmp48C != 19)goto
_LL2F9;_tmp48D=((struct Cyc_Absyn_Primop_e_struct*)_tmp48B)->f2;if(_tmp48D == 0)
goto _LL2F9;_tmp48E=*_tmp48D;_tmp48F=(struct Cyc_Absyn_Exp*)_tmp48E.hd;_LL2F8: {
void*_tmp490=(void*)_tmp48F->r;void*_tmp491;struct Cyc_Absyn_Vardecl*_tmp492;void*
_tmp493;struct Cyc_Absyn_Vardecl*_tmp494;void*_tmp495;struct Cyc_Absyn_Vardecl*
_tmp496;void*_tmp497;struct Cyc_Absyn_Vardecl*_tmp498;_LL2FC: if(*((int*)_tmp490)
!= 1)goto _LL2FE;_tmp491=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp490)->f2;if(
_tmp491 <= (void*)1)goto _LL2FE;if(*((int*)_tmp491)!= 0)goto _LL2FE;_tmp492=((
struct Cyc_Absyn_Global_b_struct*)_tmp491)->f1;if(!(!_tmp492->escapes))goto _LL2FE;
_LL2FD: _tmp494=_tmp492;goto _LL2FF;_LL2FE: if(*((int*)_tmp490)!= 1)goto _LL300;
_tmp493=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp490)->f2;if(_tmp493 <= (void*)
1)goto _LL300;if(*((int*)_tmp493)!= 3)goto _LL300;_tmp494=((struct Cyc_Absyn_Local_b_struct*)
_tmp493)->f1;if(!(!_tmp494->escapes))goto _LL300;_LL2FF: _tmp496=_tmp494;goto
_LL301;_LL300: if(*((int*)_tmp490)!= 1)goto _LL302;_tmp495=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp490)->f2;if(_tmp495 <= (void*)1)goto _LL302;if(*((int*)_tmp495)!= 4)goto _LL302;
_tmp496=((struct Cyc_Absyn_Pat_b_struct*)_tmp495)->f1;if(!(!_tmp496->escapes))
goto _LL302;_LL301: _tmp498=_tmp496;goto _LL303;_LL302: if(*((int*)_tmp490)!= 1)goto
_LL304;_tmp497=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp490)->f2;if(_tmp497 <= (
void*)1)goto _LL304;if(*((int*)_tmp497)!= 2)goto _LL304;_tmp498=((struct Cyc_Absyn_Param_b_struct*)
_tmp497)->f1;if(!(!_tmp498->escapes))goto _LL304;_LL303: {struct Cyc_CfFlowInfo_FlowEnv*
_tmp499=env->fenv;struct Cyc_CfFlowInfo_Reln*_tmp807;union Cyc_CfFlowInfo_RelnOp_union
_tmp806;struct Cyc_List_List*_tmp805;struct Cyc_List_List*_tmp49A=(_tmp805=
_region_malloc(_tmp499->r,sizeof(*_tmp805)),((_tmp805->hd=((_tmp807=
_region_malloc(_tmp499->r,sizeof(*_tmp807)),((_tmp807->vd=_tmp48A,((_tmp807->rop=(
union Cyc_CfFlowInfo_RelnOp_union)(((_tmp806.LessEqSize).tag=4,(((_tmp806.LessEqSize).f1=
_tmp498,_tmp806)))),_tmp807)))))),((_tmp805->tl=_tmp3EC,_tmp805)))));union Cyc_CfFlowInfo_FlowInfo_union
_tmp80A;struct _tuple8 _tmp809;return(_tmp809.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp80A.ReachableFL).tag=1,(((_tmp80A.ReachableFL).f1=_tmp3EB,(((_tmp80A.ReachableFL).f2=
_tmp49A,(((_tmp80A.ReachableFL).f3=_tmp3ED,_tmp80A)))))))),((_tmp809.f2=_tmp47B,
_tmp809)));}_LL304:;_LL305: {struct _tuple8 _tmp80B;return(_tmp80B.f1=_tmp47A,((
_tmp80B.f2=_tmp47B,_tmp80B)));}_LL2FB:;}_LL2F9:;_LL2FA: {struct _tuple8 _tmp80C;
return(_tmp80C.f1=_tmp47A,((_tmp80C.f2=_tmp47B,_tmp80C)));}_LL2F6:;}_LL2F4:;
_LL2F5: {struct _tuple8 _tmp80D;return(_tmp80D.f1=_tmp47A,((_tmp80D.f2=_tmp47B,
_tmp80D)));}_LL2EB:;}}_LL2BA:;_LL2BB: {struct _tuple8 _tmp80E;return(_tmp80E.f1=f,((
_tmp80E.f2=f,_tmp80E)));}_LL2AD:;}}_LL27A:;}}_LL278:;_LL279: goto _LL26B;_LL26B:;}{
union Cyc_CfFlowInfo_FlowInfo_union _tmp4A5;void*_tmp4A6;struct _tuple5 _tmp4A4=Cyc_NewControlFlow_anal_Rexp(
env,inflow,e);_tmp4A5=_tmp4A4.f1;_tmp4A6=_tmp4A4.f2;_tmp4A5=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp4A5);{union Cyc_CfFlowInfo_FlowInfo_union _tmp4A7=_tmp4A5;struct Cyc_Dict_Dict
_tmp4A8;_LL307: if((_tmp4A7.BottomFL).tag != 0)goto _LL309;_LL308: {struct _tuple8
_tmp80F;return(_tmp80F.f1=_tmp4A5,((_tmp80F.f2=_tmp4A5,_tmp80F)));}_LL309: if((
_tmp4A7.ReachableFL).tag != 1)goto _LL306;_tmp4A8=(_tmp4A7.ReachableFL).f1;_LL30A: {
void*_tmp4AA=_tmp4A6;void*_tmp4AB;void*_tmp4AC;void*_tmp4AD;_LL30C: if((int)
_tmp4AA != 0)goto _LL30E;_LL30D: {union Cyc_CfFlowInfo_FlowInfo_union _tmp812;struct
_tuple8 _tmp811;return(_tmp811.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp812.BottomFL).tag=
0,_tmp812)),((_tmp811.f2=_tmp4A5,_tmp811)));}_LL30E: if((int)_tmp4AA != 2)goto
_LL310;_LL30F: goto _LL311;_LL310: if((int)_tmp4AA != 1)goto _LL312;_LL311: goto _LL313;
_LL312: if(_tmp4AA <= (void*)3)goto _LL314;if(*((int*)_tmp4AA)!= 2)goto _LL314;
_LL313: {union Cyc_CfFlowInfo_FlowInfo_union _tmp815;struct _tuple8 _tmp814;return(
_tmp814.f1=_tmp4A5,((_tmp814.f2=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp815.BottomFL).tag=
0,_tmp815)),_tmp814)));}_LL314: if(_tmp4AA <= (void*)3)goto _LL316;if(*((int*)
_tmp4AA)!= 0)goto _LL316;_tmp4AB=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp4AA)->f1;if((int)_tmp4AB != 0)goto _LL316;_LL315: goto _LL317;_LL316: if(_tmp4AA
<= (void*)3)goto _LL318;if(*((int*)_tmp4AA)!= 1)goto _LL318;_tmp4AC=(void*)((
struct Cyc_CfFlowInfo_Esc_struct*)_tmp4AA)->f1;if((int)_tmp4AC != 0)goto _LL318;
_LL317:{const char*_tmp818;void*_tmp817;(_tmp817=0,Cyc_Tcutil_terr(e->loc,((
_tmp818="expression may not be initialized",_tag_dynforward(_tmp818,sizeof(char),
_get_zero_arr_size_char(_tmp818,34)))),_tag_dynforward(_tmp817,sizeof(void*),0)));}{
union Cyc_CfFlowInfo_FlowInfo_union _tmp81D;union Cyc_CfFlowInfo_FlowInfo_union
_tmp81C;struct _tuple8 _tmp81B;return(_tmp81B.f1=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp81C.BottomFL).tag=0,_tmp81C)),((_tmp81B.f2=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp81D.BottomFL).tag=0,_tmp81D)),_tmp81B)));}_LL318: if(_tmp4AA <= (void*)3)goto
_LL31A;if(*((int*)_tmp4AA)!= 0)goto _LL31A;_tmp4AD=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp4AA)->f1;_LL319: return Cyc_NewControlFlow_splitzero(env,inflow,_tmp4A5,e,
_tmp4AD);_LL31A: if(_tmp4AA <= (void*)3)goto _LL31C;if(*((int*)_tmp4AA)!= 1)goto
_LL31C;_LL31B: {struct _tuple8 _tmp81E;return(_tmp81E.f1=_tmp4A5,((_tmp81E.f2=
_tmp4A5,_tmp81E)));}_LL31C: if(_tmp4AA <= (void*)3)goto _LL31E;if(*((int*)_tmp4AA)
!= 3)goto _LL31E;_LL31D: {struct _tuple8 _tmp81F;return(_tmp81F.f1=_tmp4A5,((
_tmp81F.f2=_tmp4A5,_tmp81F)));}_LL31E: if(_tmp4AA <= (void*)3)goto _LL30B;if(*((int*)
_tmp4AA)!= 4)goto _LL30B;_LL31F: {struct Cyc_Core_Impossible_struct _tmp825;const
char*_tmp824;struct Cyc_Core_Impossible_struct*_tmp823;(int)_throw((void*)((
_tmp823=_cycalloc(sizeof(*_tmp823)),((_tmp823[0]=((_tmp825.tag=Cyc_Core_Impossible,((
_tmp825.f1=((_tmp824="anal_test",_tag_dynforward(_tmp824,sizeof(char),
_get_zero_arr_size_char(_tmp824,10)))),_tmp825)))),_tmp823)))));}_LL30B:;}_LL306:;}}}
static void Cyc_NewControlFlow_check_init_params(struct Cyc_Position_Segment*loc,
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union flow);
static void Cyc_NewControlFlow_check_init_params(struct Cyc_Position_Segment*loc,
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union flow){
union Cyc_CfFlowInfo_FlowInfo_union _tmp4BC=flow;struct Cyc_Dict_Dict _tmp4BD;_LL321:
if((_tmp4BC.BottomFL).tag != 0)goto _LL323;_LL322: return;_LL323: if((_tmp4BC.ReachableFL).tag
!= 1)goto _LL320;_tmp4BD=(_tmp4BC.ReachableFL).f1;_LL324:{struct Cyc_List_List*
_tmp4BE=env->param_roots;for(0;_tmp4BE != 0;_tmp4BE=_tmp4BE->tl){if(Cyc_CfFlowInfo_initlevel(
_tmp4BD,Cyc_CfFlowInfo_lookup_place(_tmp4BD,(struct Cyc_CfFlowInfo_Place*)_tmp4BE->hd))
!= (void*)2){const char*_tmp828;void*_tmp827;(_tmp827=0,Cyc_Tcutil_terr(loc,((
_tmp828="function may not initialize all the parameters with attribute 'initializes'",
_tag_dynforward(_tmp828,sizeof(char),_get_zero_arr_size_char(_tmp828,76)))),
_tag_dynforward(_tmp827,sizeof(void*),0)));}}}return;_LL320:;}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_List_List*scs);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_scs(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_List_List*scs){struct Cyc_CfFlowInfo_FlowEnv*_tmp4C1=env->fenv;for(0;
scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp4C3;struct Cyc_Core_Opt*
_tmp4C4;struct Cyc_Absyn_Exp*_tmp4C5;struct Cyc_Absyn_Stmt*_tmp4C6;struct Cyc_Position_Segment*
_tmp4C7;struct Cyc_Absyn_Switch_clause*_tmp4C2=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp4C3=*_tmp4C2;_tmp4C4=_tmp4C3.pat_vars;_tmp4C5=_tmp4C3.where_clause;
_tmp4C6=_tmp4C3.body;_tmp4C7=_tmp4C3.loc;{union Cyc_CfFlowInfo_FlowInfo_union
clause_inflow=Cyc_NewControlFlow_add_vars(_tmp4C1,inflow,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp4C4))->v,(void*)_tmp4C1->unknown_all,_tmp4C7);
union Cyc_CfFlowInfo_FlowInfo_union clause_outflow;if(_tmp4C5 != 0){struct Cyc_Absyn_Exp*
wexp=(struct Cyc_Absyn_Exp*)_tmp4C5;union Cyc_CfFlowInfo_FlowInfo_union _tmp4C9;
union Cyc_CfFlowInfo_FlowInfo_union _tmp4CA;struct _tuple8 _tmp4C8=Cyc_NewControlFlow_anal_test(
env,clause_inflow,wexp);_tmp4C9=_tmp4C8.f1;_tmp4CA=_tmp4C8.f2;_tmp4C9=Cyc_CfFlowInfo_readthrough_unique_rvals(
wexp->loc,_tmp4C9);_tmp4CA=Cyc_CfFlowInfo_readthrough_unique_rvals(wexp->loc,
_tmp4CA);inflow=_tmp4CA;clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp4C9,
_tmp4C6);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,
_tmp4C6);}{union Cyc_CfFlowInfo_FlowInfo_union _tmp4CB=clause_outflow;_LL326: if((
_tmp4CB.BottomFL).tag != 0)goto _LL328;_LL327: goto _LL325;_LL328:;_LL329: if(scs->tl
== 0)return clause_outflow;else{if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(
scs->tl))->hd)->pat_vars))->v != 0){const char*_tmp82B;void*_tmp82A;(_tmp82A=0,Cyc_Tcutil_terr(
_tmp4C6->loc,((_tmp82B="switch clause may implicitly fallthru",_tag_dynforward(
_tmp82B,sizeof(char),_get_zero_arr_size_char(_tmp82B,38)))),_tag_dynforward(
_tmp82A,sizeof(void*),0)));}else{const char*_tmp82E;void*_tmp82D;(_tmp82D=0,Cyc_Tcutil_warn(
_tmp4C6->loc,((_tmp82E="switch clause may implicitly fallthru",_tag_dynforward(
_tmp82E,sizeof(char),_get_zero_arr_size_char(_tmp82E,38)))),_tag_dynforward(
_tmp82D,sizeof(void*),0)));}Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}goto
_LL325;_LL325:;}}}{union Cyc_CfFlowInfo_FlowInfo_union _tmp82F;return(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp82F.BottomFL).tag=0,_tmp82F));}}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Stmt*s);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo_union outflow;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp4D2;union Cyc_CfFlowInfo_FlowInfo_union*_tmp4D3;struct _tuple9 _tmp4D1=Cyc_NewControlFlow_pre_stmt_check(
env,inflow,s);_tmp4D2=_tmp4D1.f1;_tmp4D3=_tmp4D1.f2;inflow=*_tmp4D3;{struct Cyc_CfFlowInfo_FlowEnv*
_tmp4D4=env->fenv;{void*_tmp4D5=(void*)s->r;struct Cyc_Absyn_Exp*_tmp4D6;struct
Cyc_Absyn_Exp*_tmp4D7;struct Cyc_Absyn_Exp*_tmp4D8;struct Cyc_Absyn_Stmt*_tmp4D9;
struct Cyc_Absyn_Stmt*_tmp4DA;struct Cyc_Absyn_Exp*_tmp4DB;struct Cyc_Absyn_Stmt*
_tmp4DC;struct Cyc_Absyn_Stmt*_tmp4DD;struct _tuple3 _tmp4DE;struct Cyc_Absyn_Exp*
_tmp4DF;struct Cyc_Absyn_Stmt*_tmp4E0;struct Cyc_Absyn_Stmt*_tmp4E1;struct Cyc_Absyn_Stmt*
_tmp4E2;struct _tuple3 _tmp4E3;struct Cyc_Absyn_Exp*_tmp4E4;struct Cyc_Absyn_Stmt*
_tmp4E5;struct Cyc_Absyn_Exp*_tmp4E6;struct _tuple3 _tmp4E7;struct Cyc_Absyn_Exp*
_tmp4E8;struct Cyc_Absyn_Stmt*_tmp4E9;struct _tuple3 _tmp4EA;struct Cyc_Absyn_Exp*
_tmp4EB;struct Cyc_Absyn_Stmt*_tmp4EC;struct Cyc_Absyn_Stmt*_tmp4ED;struct Cyc_Absyn_Stmt*
_tmp4EE;struct Cyc_List_List*_tmp4EF;struct Cyc_Absyn_Switch_clause**_tmp4F0;
struct Cyc_Absyn_Switch_clause*_tmp4F1;struct Cyc_Absyn_Stmt*_tmp4F2;struct Cyc_Absyn_Stmt*
_tmp4F3;struct Cyc_Absyn_Stmt*_tmp4F4;struct Cyc_Absyn_Exp*_tmp4F5;struct Cyc_List_List*
_tmp4F6;struct Cyc_Absyn_Stmt*_tmp4F7;struct Cyc_List_List*_tmp4F8;struct Cyc_Absyn_Decl*
_tmp4F9;struct Cyc_Absyn_Stmt*_tmp4FA;struct Cyc_Absyn_Stmt*_tmp4FB;struct Cyc_Absyn_Tvar*
_tmp4FC;struct Cyc_Absyn_Vardecl*_tmp4FD;int _tmp4FE;struct Cyc_Absyn_Exp*_tmp4FF;
struct Cyc_Absyn_Stmt*_tmp500;struct Cyc_Absyn_Exp*_tmp501;struct Cyc_Absyn_Exp*
_tmp502;struct Cyc_Absyn_Tvar*_tmp503;struct Cyc_Absyn_Vardecl*_tmp504;struct Cyc_Absyn_Stmt*
_tmp505;_LL32B: if((int)_tmp4D5 != 0)goto _LL32D;_LL32C: return inflow;_LL32D: if(
_tmp4D5 <= (void*)1)goto _LL355;if(*((int*)_tmp4D5)!= 2)goto _LL32F;_tmp4D6=((
struct Cyc_Absyn_Return_s_struct*)_tmp4D5)->f1;if(_tmp4D6 != 0)goto _LL32F;_LL32E:
if(env->noreturn){const char*_tmp832;void*_tmp831;(_tmp831=0,Cyc_Tcutil_terr(s->loc,((
_tmp832="`noreturn' function might return",_tag_dynforward(_tmp832,sizeof(char),
_get_zero_arr_size_char(_tmp832,33)))),_tag_dynforward(_tmp831,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp833;return(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp833.BottomFL).tag=0,
_tmp833));}_LL32F: if(*((int*)_tmp4D5)!= 2)goto _LL331;_tmp4D7=((struct Cyc_Absyn_Return_s_struct*)
_tmp4D5)->f1;_LL330: if(env->noreturn){const char*_tmp836;void*_tmp835;(_tmp835=0,
Cyc_Tcutil_terr(s->loc,((_tmp836="`noreturn' function might return",
_tag_dynforward(_tmp836,sizeof(char),_get_zero_arr_size_char(_tmp836,33)))),
_tag_dynforward(_tmp835,sizeof(void*),0)));}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp50C;void*_tmp50D;struct _tuple5 _tmp50B=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp4D7));_tmp50C=_tmp50B.f1;_tmp50D=
_tmp50B.f2;_tmp50C=Cyc_CfFlowInfo_consume_unique_rvals(_tmp4D7->loc,_tmp50C);
_tmp50C=Cyc_NewControlFlow_use_Rval(env,_tmp4D7->loc,_tmp50C,_tmp50D);Cyc_NewControlFlow_check_init_params(
s->loc,env,_tmp50C);{union Cyc_CfFlowInfo_FlowInfo_union _tmp837;return(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp837.BottomFL).tag=0,_tmp837));}}_LL331: if(*((int*)_tmp4D5)!= 0)goto _LL333;
_tmp4D8=((struct Cyc_Absyn_Exp_s_struct*)_tmp4D5)->f1;_LL332: outflow=(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4D8)).f1;goto _LL32A;_LL333: if(*((int*)_tmp4D5)!= 1)goto _LL335;
_tmp4D9=((struct Cyc_Absyn_Seq_s_struct*)_tmp4D5)->f1;_tmp4DA=((struct Cyc_Absyn_Seq_s_struct*)
_tmp4D5)->f2;_LL334: outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp4D9),_tmp4DA);goto _LL32A;_LL335: if(*((int*)_tmp4D5)!= 3)goto
_LL337;_tmp4DB=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp4D5)->f1;_tmp4DC=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp4D5)->f2;_tmp4DD=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp4D5)->f3;_LL336: {union Cyc_CfFlowInfo_FlowInfo_union _tmp510;union Cyc_CfFlowInfo_FlowInfo_union
_tmp511;struct _tuple8 _tmp50F=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4DB);
_tmp510=_tmp50F.f1;_tmp511=_tmp50F.f2;_tmp510=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp4DB->loc,_tmp510);_tmp511=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4DB->loc,
_tmp511);outflow=Cyc_CfFlowInfo_join_flow(_tmp4D4,env->all_changed,Cyc_NewControlFlow_anal_stmt(
env,_tmp510,_tmp4DC),Cyc_NewControlFlow_anal_stmt(env,_tmp511,_tmp4DD),1);goto
_LL32A;}_LL337: if(*((int*)_tmp4D5)!= 4)goto _LL339;_tmp4DE=((struct Cyc_Absyn_While_s_struct*)
_tmp4D5)->f1;_tmp4DF=_tmp4DE.f1;_tmp4E0=_tmp4DE.f2;_tmp4E1=((struct Cyc_Absyn_While_s_struct*)
_tmp4D5)->f2;_LL338: {union Cyc_CfFlowInfo_FlowInfo_union*_tmp513;struct _tuple9
_tmp512=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp4E0);_tmp513=_tmp512.f2;{
union Cyc_CfFlowInfo_FlowInfo_union _tmp514=*_tmp513;union Cyc_CfFlowInfo_FlowInfo_union
_tmp516;union Cyc_CfFlowInfo_FlowInfo_union _tmp517;struct _tuple8 _tmp515=Cyc_NewControlFlow_anal_test(
env,_tmp514,_tmp4DF);_tmp516=_tmp515.f1;_tmp517=_tmp515.f2;_tmp516=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp4DF->loc,_tmp516);_tmp517=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4DF->loc,
_tmp517);{union Cyc_CfFlowInfo_FlowInfo_union _tmp518=Cyc_NewControlFlow_anal_stmt(
env,_tmp516,_tmp4E1);Cyc_NewControlFlow_update_flow(env,_tmp4E0,_tmp518);outflow=
_tmp517;goto _LL32A;}}}_LL339: if(*((int*)_tmp4D5)!= 13)goto _LL33B;_tmp4E2=((
struct Cyc_Absyn_Do_s_struct*)_tmp4D5)->f1;_tmp4E3=((struct Cyc_Absyn_Do_s_struct*)
_tmp4D5)->f2;_tmp4E4=_tmp4E3.f1;_tmp4E5=_tmp4E3.f2;_LL33A: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp519=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4E2);union Cyc_CfFlowInfo_FlowInfo_union*
_tmp51B;struct _tuple9 _tmp51A=Cyc_NewControlFlow_pre_stmt_check(env,_tmp519,
_tmp4E5);_tmp51B=_tmp51A.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp51C=*_tmp51B;
union Cyc_CfFlowInfo_FlowInfo_union _tmp51E;union Cyc_CfFlowInfo_FlowInfo_union
_tmp51F;struct _tuple8 _tmp51D=Cyc_NewControlFlow_anal_test(env,_tmp51C,_tmp4E4);
_tmp51E=_tmp51D.f1;_tmp51F=_tmp51D.f2;Cyc_NewControlFlow_update_flow(env,_tmp4E2,
_tmp51E);outflow=_tmp51F;goto _LL32A;}}_LL33B: if(*((int*)_tmp4D5)!= 8)goto _LL33D;
_tmp4E6=((struct Cyc_Absyn_For_s_struct*)_tmp4D5)->f1;_tmp4E7=((struct Cyc_Absyn_For_s_struct*)
_tmp4D5)->f2;_tmp4E8=_tmp4E7.f1;_tmp4E9=_tmp4E7.f2;_tmp4EA=((struct Cyc_Absyn_For_s_struct*)
_tmp4D5)->f3;_tmp4EB=_tmp4EA.f1;_tmp4EC=_tmp4EA.f2;_tmp4ED=((struct Cyc_Absyn_For_s_struct*)
_tmp4D5)->f4;_LL33C: {union Cyc_CfFlowInfo_FlowInfo_union _tmp520=(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4E6)).f1;_tmp520=Cyc_CfFlowInfo_drop_unique_rvals(_tmp4E6->loc,
_tmp520);{union Cyc_CfFlowInfo_FlowInfo_union*_tmp522;struct _tuple9 _tmp521=Cyc_NewControlFlow_pre_stmt_check(
env,_tmp520,_tmp4E9);_tmp522=_tmp521.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp523=*_tmp522;union Cyc_CfFlowInfo_FlowInfo_union _tmp525;union Cyc_CfFlowInfo_FlowInfo_union
_tmp526;struct _tuple8 _tmp524=Cyc_NewControlFlow_anal_test(env,_tmp523,_tmp4E8);
_tmp525=_tmp524.f1;_tmp526=_tmp524.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp527=
Cyc_NewControlFlow_anal_stmt(env,_tmp525,_tmp4ED);union Cyc_CfFlowInfo_FlowInfo_union*
_tmp529;struct _tuple9 _tmp528=Cyc_NewControlFlow_pre_stmt_check(env,_tmp527,
_tmp4EC);_tmp529=_tmp528.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp52A=*_tmp529;
union Cyc_CfFlowInfo_FlowInfo_union _tmp52B=(Cyc_NewControlFlow_anal_Rexp(env,
_tmp52A,_tmp4EB)).f1;_tmp52B=Cyc_CfFlowInfo_drop_unique_rvals(_tmp4EB->loc,
_tmp52B);Cyc_NewControlFlow_update_flow(env,_tmp4E9,_tmp52B);outflow=_tmp526;
goto _LL32A;}}}}}_LL33D: if(*((int*)_tmp4D5)!= 5)goto _LL33F;_tmp4EE=((struct Cyc_Absyn_Break_s_struct*)
_tmp4D5)->f1;if(_tmp4EE != 0)goto _LL33F;_LL33E: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp838;return(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp838.BottomFL).tag=0,
_tmp838));}_LL33F: if(*((int*)_tmp4D5)!= 10)goto _LL341;_tmp4EF=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp4D5)->f1;_tmp4F0=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp4D5)->f2;if(
_tmp4F0 == 0)goto _LL341;_tmp4F1=*_tmp4F0;_LL340: {struct _RegionHandle _tmp52D=
_new_region("temp");struct _RegionHandle*temp=& _tmp52D;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp52F;struct Cyc_List_List*_tmp530;struct _tuple11
_tmp52E=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp4EF,0);
_tmp52F=_tmp52E.f1;_tmp530=_tmp52E.f2;for(0;_tmp530 != 0;(_tmp530=_tmp530->tl,
_tmp4EF=_tmp4EF->tl)){_tmp52F=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4EF))->hd)->loc,_tmp52F,(void*)_tmp530->hd);}
_tmp52F=Cyc_CfFlowInfo_consume_unique_rvals(s->loc,_tmp52F);_tmp52F=Cyc_NewControlFlow_add_vars(
_tmp4D4,_tmp52F,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp4F1->pat_vars))->v,(
void*)_tmp4D4->unknown_all,s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_tmp4F1->body,_tmp52F);{union Cyc_CfFlowInfo_FlowInfo_union _tmp839;union Cyc_CfFlowInfo_FlowInfo_union
_tmp532=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp839.BottomFL).tag=0,_tmp839));
_npop_handler(0);return _tmp532;}};_pop_region(temp);}_LL341: if(*((int*)_tmp4D5)
!= 5)goto _LL343;_tmp4F2=((struct Cyc_Absyn_Break_s_struct*)_tmp4D5)->f1;_LL342:
_tmp4F3=_tmp4F2;goto _LL344;_LL343: if(*((int*)_tmp4D5)!= 6)goto _LL345;_tmp4F3=((
struct Cyc_Absyn_Continue_s_struct*)_tmp4D5)->f1;_LL344: _tmp4F4=_tmp4F3;goto
_LL346;_LL345: if(*((int*)_tmp4D5)!= 7)goto _LL347;_tmp4F4=((struct Cyc_Absyn_Goto_s_struct*)
_tmp4D5)->f2;_LL346: if(env->iteration_num == 1){struct Cyc_Absyn_Stmt*_tmp533=
_tmp4D2->encloser;struct Cyc_Absyn_Stmt*_tmp534=(Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*)_check_null(_tmp4F4)))->encloser;while(_tmp534 != _tmp533){
struct Cyc_Absyn_Stmt*_tmp535=(Cyc_NewControlFlow_get_stmt_annot(_tmp533))->encloser;
if(_tmp535 == _tmp533){{const char*_tmp83C;void*_tmp83B;(_tmp83B=0,Cyc_Tcutil_terr(
s->loc,((_tmp83C="goto enters local scope or exception handler",_tag_dynforward(
_tmp83C,sizeof(char),_get_zero_arr_size_char(_tmp83C,45)))),_tag_dynforward(
_tmp83B,sizeof(void*),0)));}break;}_tmp533=_tmp535;}}Cyc_NewControlFlow_update_flow(
env,(struct Cyc_Absyn_Stmt*)_check_null(_tmp4F4),inflow);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp83D;return(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp83D.BottomFL).tag=0,
_tmp83D));}_LL347: if(*((int*)_tmp4D5)!= 9)goto _LL349;_tmp4F5=((struct Cyc_Absyn_Switch_s_struct*)
_tmp4D5)->f1;_tmp4F6=((struct Cyc_Absyn_Switch_s_struct*)_tmp4D5)->f2;_LL348: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp53A;void*_tmp53B;struct _tuple5 _tmp539=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4F5);_tmp53A=_tmp539.f1;_tmp53B=_tmp539.f2;_tmp53A=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp4F5->loc,_tmp53A);_tmp53A=Cyc_NewControlFlow_use_Rval(env,_tmp4F5->loc,
_tmp53A,_tmp53B);outflow=Cyc_NewControlFlow_anal_scs(env,_tmp53A,_tmp4F6);goto
_LL32A;}_LL349: if(*((int*)_tmp4D5)!= 14)goto _LL34B;_tmp4F7=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp4D5)->f1;_tmp4F8=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp4D5)->f2;_LL34A: {
int old_in_try=env->in_try;union Cyc_CfFlowInfo_FlowInfo_union old_tryflow=env->tryflow;
env->in_try=1;env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo_union s1_outflow=
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4F7);union Cyc_CfFlowInfo_FlowInfo_union
scs_inflow=env->tryflow;env->in_try=old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(
env,scs_inflow);{union Cyc_CfFlowInfo_FlowInfo_union scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp4F8);{union Cyc_CfFlowInfo_FlowInfo_union _tmp53C=scs_outflow;
_LL358: if((_tmp53C.BottomFL).tag != 0)goto _LL35A;_LL359: goto _LL357;_LL35A:;_LL35B: {
const char*_tmp840;void*_tmp83F;(_tmp83F=0,Cyc_Tcutil_terr(s->loc,((_tmp840="last catch clause may implicitly fallthru",
_tag_dynforward(_tmp840,sizeof(char),_get_zero_arr_size_char(_tmp840,42)))),
_tag_dynforward(_tmp83F,sizeof(void*),0)));}_LL357:;}outflow=s1_outflow;goto
_LL32A;}}}_LL34B: if(*((int*)_tmp4D5)!= 11)goto _LL34D;_tmp4F9=((struct Cyc_Absyn_Decl_s_struct*)
_tmp4D5)->f1;_tmp4FA=((struct Cyc_Absyn_Decl_s_struct*)_tmp4D5)->f2;_LL34C:
outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,
_tmp4F9),_tmp4FA);goto _LL32A;_LL34D: if(*((int*)_tmp4D5)!= 12)goto _LL34F;_tmp4FB=((
struct Cyc_Absyn_Label_s_struct*)_tmp4D5)->f2;_LL34E: outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp4FB);goto _LL32A;_LL34F: if(*((int*)_tmp4D5)!= 15)goto _LL351;
_tmp4FC=((struct Cyc_Absyn_Region_s_struct*)_tmp4D5)->f1;_tmp4FD=((struct Cyc_Absyn_Region_s_struct*)
_tmp4D5)->f2;_tmp4FE=((struct Cyc_Absyn_Region_s_struct*)_tmp4D5)->f3;_tmp4FF=((
struct Cyc_Absyn_Region_s_struct*)_tmp4D5)->f4;_tmp500=((struct Cyc_Absyn_Region_s_struct*)
_tmp4D5)->f5;_LL350: if(_tmp4FF != 0){struct Cyc_Absyn_Exp*_tmp53F=(struct Cyc_Absyn_Exp*)
_tmp4FF;union Cyc_CfFlowInfo_FlowInfo_union _tmp541;void*_tmp542;struct _tuple5
_tmp540=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp53F);_tmp541=_tmp540.f1;
_tmp542=_tmp540.f2;_tmp541=Cyc_CfFlowInfo_consume_unique_rvals(_tmp53F->loc,
_tmp541);inflow=Cyc_NewControlFlow_use_Rval(env,_tmp53F->loc,_tmp541,_tmp542);}{
struct Cyc_List_List _tmp841;struct Cyc_List_List _tmp543=(_tmp841.hd=_tmp4FD,((
_tmp841.tl=0,_tmp841)));inflow=Cyc_NewControlFlow_add_vars(_tmp4D4,inflow,(
struct Cyc_List_List*)& _tmp543,(void*)_tmp4D4->unknown_all,_tmp500->loc);outflow=
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp500);goto _LL32A;}_LL351: if(*((int*)
_tmp4D5)!= 16)goto _LL353;_tmp501=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp4D5)->f1;
_LL352: {union Cyc_CfFlowInfo_FlowInfo_union _tmp546;void*_tmp547;struct _tuple5
_tmp545=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp501);_tmp546=_tmp545.f1;
_tmp547=_tmp545.f2;_tmp546=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp501->loc,
_tmp546);{union Cyc_CfFlowInfo_FlowInfo_union _tmp548=Cyc_NewControlFlow_use_Rval(
env,_tmp501->loc,_tmp546,_tmp547);{void*_tmp549=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp501->topt))->v);void*_tmp54A;_LL35D: if(
_tmp549 <= (void*)4)goto _LL35F;if(*((int*)_tmp549)!= 15)goto _LL35F;_tmp54A=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp549)->f1;_LL35E: outflow=Cyc_CfFlowInfo_kill_flow_region(
_tmp4D4,_tmp546,_tmp54A);goto _LL35C;_LL35F:;_LL360: {struct Cyc_Core_Impossible_struct
_tmp847;const char*_tmp846;struct Cyc_Core_Impossible_struct*_tmp845;(int)_throw((
void*)((_tmp845=_cycalloc(sizeof(*_tmp845)),((_tmp845[0]=((_tmp847.tag=Cyc_Core_Impossible,((
_tmp847.f1=((_tmp846="anal_stmt -- reset_region",_tag_dynforward(_tmp846,sizeof(
char),_get_zero_arr_size_char(_tmp846,26)))),_tmp847)))),_tmp845)))));}_LL35C:;}
goto _LL32A;}}_LL353: if(*((int*)_tmp4D5)!= 17)goto _LL355;_tmp502=((struct Cyc_Absyn_Alias_s_struct*)
_tmp4D5)->f1;_tmp503=((struct Cyc_Absyn_Alias_s_struct*)_tmp4D5)->f2;_tmp504=((
struct Cyc_Absyn_Alias_s_struct*)_tmp4D5)->f3;_tmp505=((struct Cyc_Absyn_Alias_s_struct*)
_tmp4D5)->f4;_LL354: {union Cyc_CfFlowInfo_FlowInfo_union _tmp54F;void*_tmp550;
struct _tuple5 _tmp54E=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp502);_tmp54F=
_tmp54E.f1;_tmp550=_tmp54E.f2;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp552;struct Cyc_List_List*
_tmp553;struct _tuple6 _tmp551=Cyc_CfFlowInfo_save_consume_info(_tmp4D4,_tmp54F,0);
_tmp552=_tmp551.f1;_tmp553=_tmp552.may_consume;_tmp54F=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp502->loc,_tmp54F);_tmp54F=Cyc_NewControlFlow_use_Rval(env,_tmp502->loc,
_tmp54F,_tmp550);{struct Cyc_List_List _tmp848;struct Cyc_List_List _tmp554=(_tmp848.hd=
_tmp504,((_tmp848.tl=0,_tmp848)));_tmp54F=Cyc_NewControlFlow_add_vars(_tmp4D4,
_tmp54F,(struct Cyc_List_List*)& _tmp554,(void*)_tmp4D4->unknown_all,s->loc);
outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp54F,_tmp505);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp555=outflow;struct Cyc_Dict_Dict _tmp556;struct Cyc_List_List*_tmp557;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp558;_LL362: if((_tmp555.BottomFL).tag != 0)goto _LL364;_LL363: goto _LL361;_LL364:
if((_tmp555.ReachableFL).tag != 1)goto _LL361;_tmp556=(_tmp555.ReachableFL).f1;
_tmp557=(_tmp555.ReachableFL).f2;_tmp558=(_tmp555.ReachableFL).f3;_LL365: while(
_tmp553 != 0){struct Cyc_Dict_Dict _tmp559=_tmp558.consumed;_tmp558.consumed=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))
Cyc_Dict_rdelete)(_tmp4D4->r,_tmp558.consumed,(struct Cyc_CfFlowInfo_Place*)
_tmp553->hd);if((_tmp558.consumed).t != _tmp559.t);_tmp553=_tmp553->tl;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp849;outflow=(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp849.ReachableFL).tag=
1,(((_tmp849.ReachableFL).f1=_tmp556,(((_tmp849.ReachableFL).f2=_tmp557,(((
_tmp849.ReachableFL).f3=_tmp558,_tmp849))))))));}goto _LL361;_LL361:;}goto _LL32A;}}}
_LL355:;_LL356: {struct Cyc_Core_Impossible_struct _tmp84F;const char*_tmp84E;
struct Cyc_Core_Impossible_struct*_tmp84D;(int)_throw((void*)((_tmp84D=_cycalloc(
sizeof(*_tmp84D)),((_tmp84D[0]=((_tmp84F.tag=Cyc_Core_Impossible,((_tmp84F.f1=((
_tmp84E="anal_stmt -- bad stmt syntax or unimplemented stmt form",
_tag_dynforward(_tmp84E,sizeof(char),_get_zero_arr_size_char(_tmp84E,56)))),
_tmp84F)))),_tmp84D)))));}_LL32A:;}outflow=Cyc_CfFlowInfo_drop_unique_rvals(s->loc,
outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp55F=outflow;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp560;_LL367: if((_tmp55F.ReachableFL).tag != 1)goto _LL369;_tmp560=(_tmp55F.ReachableFL).f3;
_LL368: goto _LL366;_LL369:;_LL36A: goto _LL366;_LL366:;}return outflow;}}static void
Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_Absyn_Fndecl*fd);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Decl*decl);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Decl*decl){void*_tmp561=(void*)decl->r;struct Cyc_Absyn_Vardecl*
_tmp562;struct Cyc_Core_Opt*_tmp563;struct Cyc_Core_Opt _tmp564;struct Cyc_List_List*
_tmp565;struct Cyc_Absyn_Exp*_tmp566;struct Cyc_List_List*_tmp567;struct Cyc_Absyn_Fndecl*
_tmp568;_LL36C: if(_tmp561 <= (void*)2)goto _LL374;if(*((int*)_tmp561)!= 0)goto
_LL36E;_tmp562=((struct Cyc_Absyn_Var_d_struct*)_tmp561)->f1;_LL36D: {struct Cyc_List_List
_tmp850;struct Cyc_List_List _tmp569=(_tmp850.hd=_tmp562,((_tmp850.tl=0,_tmp850)));
inflow=Cyc_NewControlFlow_add_vars(env->fenv,inflow,(struct Cyc_List_List*)&
_tmp569,(void*)(env->fenv)->unknown_none,decl->loc);{struct Cyc_Absyn_Exp*_tmp56A=
_tmp562->initializer;if(_tmp56A == 0)return inflow;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp56C;void*_tmp56D;struct _tuple5 _tmp56B=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_tmp56A);_tmp56C=_tmp56B.f1;_tmp56D=_tmp56B.f2;
_tmp56C=Cyc_CfFlowInfo_consume_unique_rvals(_tmp56A->loc,_tmp56C);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp56E=_tmp56C;struct Cyc_Dict_Dict _tmp56F;struct Cyc_List_List*_tmp570;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp571;_LL377: if((_tmp56E.BottomFL).tag != 0)goto _LL379;_LL378: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp851;return(union Cyc_CfFlowInfo_FlowInfo_union)(((_tmp851.BottomFL).tag=0,
_tmp851));}_LL379: if((_tmp56E.ReachableFL).tag != 1)goto _LL376;_tmp56F=(_tmp56E.ReachableFL).f1;
_tmp570=(_tmp56E.ReachableFL).f2;_tmp571=(_tmp56E.ReachableFL).f3;_LL37A:{struct
Cyc_CfFlowInfo_VarRoot_struct*_tmp857;struct Cyc_CfFlowInfo_VarRoot_struct _tmp856;
struct Cyc_CfFlowInfo_Place*_tmp855;_tmp56F=Cyc_CfFlowInfo_assign_place(env->fenv,
decl->loc,_tmp56F,env->all_changed,((_tmp855=_region_malloc(env->r,sizeof(*
_tmp855)),((_tmp855->root=(void*)((void*)((_tmp857=_region_malloc(env->r,sizeof(*
_tmp857)),((_tmp857[0]=((_tmp856.tag=0,((_tmp856.f1=_tmp562,_tmp856)))),_tmp857))))),((
_tmp855->fields=0,_tmp855)))))),_tmp56D);}_tmp570=Cyc_CfFlowInfo_reln_assign_var(
env->r,_tmp570,_tmp562,(struct Cyc_Absyn_Exp*)_check_null(_tmp562->initializer));{
union Cyc_CfFlowInfo_FlowInfo_union _tmp858;union Cyc_CfFlowInfo_FlowInfo_union
_tmp576=((_tmp858.ReachableFL).tag=1,(((_tmp858.ReachableFL).f1=_tmp56F,(((
_tmp858.ReachableFL).f2=_tmp570,(((_tmp858.ReachableFL).f3=_tmp571,_tmp858)))))));
Cyc_NewControlFlow_update_tryflow(env,(union Cyc_CfFlowInfo_FlowInfo_union)
_tmp576);return(union Cyc_CfFlowInfo_FlowInfo_union)_tmp576;}_LL376:;}}}}_LL36E:
if(*((int*)_tmp561)!= 2)goto _LL370;_tmp563=((struct Cyc_Absyn_Let_d_struct*)
_tmp561)->f2;if(_tmp563 == 0)goto _LL370;_tmp564=*_tmp563;_tmp565=(struct Cyc_List_List*)
_tmp564.v;_tmp566=((struct Cyc_Absyn_Let_d_struct*)_tmp561)->f3;_LL36F: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp57A;void*_tmp57B;struct _tuple5 _tmp579=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp566);_tmp57A=_tmp579.f1;_tmp57B=_tmp579.f2;_tmp57A=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp566->loc,_tmp57A);_tmp57A=Cyc_NewControlFlow_use_Rval(env,_tmp566->loc,
_tmp57A,_tmp57B);return Cyc_NewControlFlow_add_vars(env->fenv,_tmp57A,_tmp565,(
void*)(env->fenv)->unknown_all,decl->loc);}_LL370: if(*((int*)_tmp561)!= 3)goto
_LL372;_tmp567=((struct Cyc_Absyn_Letv_d_struct*)_tmp561)->f1;_LL371: return Cyc_NewControlFlow_add_vars(
env->fenv,inflow,_tmp567,(void*)(env->fenv)->unknown_none,decl->loc);_LL372: if(*((
int*)_tmp561)!= 1)goto _LL374;_tmp568=((struct Cyc_Absyn_Fn_d_struct*)_tmp561)->f1;
_LL373: Cyc_NewControlFlow_check_nested_fun(env->fenv,inflow,_tmp568);{void*
_tmp57C=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp568->cached_typ))->v;struct
Cyc_Absyn_Vardecl*_tmp57D=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp568->fn_vardecl);
struct Cyc_List_List*_tmp859;return Cyc_NewControlFlow_add_vars(env->fenv,inflow,((
_tmp859=_region_malloc(env->r,sizeof(*_tmp859)),((_tmp859->hd=_tmp57D,((_tmp859->tl=
0,_tmp859)))))),(void*)(env->fenv)->unknown_all,decl->loc);}_LL374:;_LL375: {
struct Cyc_Core_Impossible_struct _tmp85F;const char*_tmp85E;struct Cyc_Core_Impossible_struct*
_tmp85D;(int)_throw((void*)((_tmp85D=_cycalloc(sizeof(*_tmp85D)),((_tmp85D[0]=((
_tmp85F.tag=Cyc_Core_Impossible,((_tmp85F.f1=((_tmp85E="anal_decl: unexpected decl variant",
_tag_dynforward(_tmp85E,sizeof(char),_get_zero_arr_size_char(_tmp85E,35)))),
_tmp85F)))),_tmp85D)))));}_LL36B:;}static void Cyc_NewControlFlow_check_fun(struct
Cyc_Absyn_Fndecl*fd);static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*
fd){struct _RegionHandle _tmp582=_new_region("frgn");struct _RegionHandle*frgn=&
_tmp582;_push_region(frgn);{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(
frgn);struct Cyc_CfFlowInfo_ConsumeInfo _tmp862;union Cyc_CfFlowInfo_FlowInfo_union
_tmp861;Cyc_NewControlFlow_check_nested_fun(fenv,(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp861.ReachableFL).tag=1,(((_tmp861.ReachableFL).f1=fenv->mt_flowdict,(((
_tmp861.ReachableFL).f2=0,(((_tmp861.ReachableFL).f3=((_tmp862.consumed=fenv->mt_place_set,((
_tmp862.may_consume=0,_tmp862)))),_tmp861)))))))),fd);};_pop_region(frgn);}
static int Cyc_NewControlFlow_hash_ptr(void*s);static int Cyc_NewControlFlow_hash_ptr(
void*s){return(int)s;}static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Fndecl*fd);static
void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*fenv,union
Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Fndecl*fd){struct
_RegionHandle*_tmp585=fenv->r;struct Cyc_Position_Segment*_tmp586=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(void*)fenv->unknown_all,_tmp586);{struct Cyc_List_List*
param_roots=0;{union Cyc_CfFlowInfo_FlowInfo_union _tmp587=inflow;struct Cyc_Dict_Dict
_tmp588;struct Cyc_List_List*_tmp589;struct Cyc_CfFlowInfo_ConsumeInfo _tmp58A;
_LL37C: if((_tmp587.BottomFL).tag != 0)goto _LL37E;_LL37D: {const char*_tmp865;void*
_tmp864;(_tmp864=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(((_tmp865="check_fun",_tag_dynforward(_tmp865,sizeof(char),
_get_zero_arr_size_char(_tmp865,10)))),_tag_dynforward(_tmp864,sizeof(void*),0)));}
_LL37E: if((_tmp587.ReachableFL).tag != 1)goto _LL37B;_tmp588=(_tmp587.ReachableFL).f1;
_tmp589=(_tmp587.ReachableFL).f2;_tmp58A=(_tmp587.ReachableFL).f3;_LL37F: {
struct Cyc_List_List*atts;{void*_tmp58D=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(fd->cached_typ))->v);struct Cyc_Absyn_FnInfo _tmp58E;struct Cyc_List_List*
_tmp58F;_LL381: if(_tmp58D <= (void*)4)goto _LL383;if(*((int*)_tmp58D)!= 8)goto
_LL383;_tmp58E=((struct Cyc_Absyn_FnType_struct*)_tmp58D)->f1;_tmp58F=_tmp58E.attributes;
_LL382: atts=_tmp58F;goto _LL380;_LL383:;_LL384: {const char*_tmp868;void*_tmp867;(
_tmp867=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmp868="check_fun: non-function type cached with fndecl_t",_tag_dynforward(
_tmp868,sizeof(char),_get_zero_arr_size_char(_tmp868,50)))),_tag_dynforward(
_tmp867,sizeof(void*),0)));}_LL380:;}for(0;atts != 0;atts=atts->tl){void*_tmp592=(
void*)atts->hd;int _tmp593;_LL386: if(_tmp592 <= (void*)17)goto _LL388;if(*((int*)
_tmp592)!= 4)goto _LL388;_tmp593=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp592)->f1;_LL387: {unsigned int j=(unsigned int)_tmp593;if(j > ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v)){{const char*_tmp86B;void*_tmp86A;(_tmp86A=0,
Cyc_Tcutil_terr(_tmp586,((_tmp86B="initializes attribute exceeds number of parameters",
_tag_dynforward(_tmp86B,sizeof(char),_get_zero_arr_size_char(_tmp86B,51)))),
_tag_dynforward(_tmp86A,sizeof(void*),0)));}continue;}{struct Cyc_Absyn_Vardecl*
_tmp596=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(
int)(j - 1));{void*_tmp597=Cyc_Tcutil_compress((void*)_tmp596->type);struct Cyc_Absyn_PtrInfo
_tmp598;void*_tmp599;struct Cyc_Absyn_PtrAtts _tmp59A;struct Cyc_Absyn_Conref*
_tmp59B;struct Cyc_Absyn_Conref*_tmp59C;struct Cyc_Absyn_Conref*_tmp59D;_LL38B: if(
_tmp597 <= (void*)4)goto _LL38D;if(*((int*)_tmp597)!= 4)goto _LL38D;_tmp598=((
struct Cyc_Absyn_PointerType_struct*)_tmp597)->f1;_tmp599=(void*)_tmp598.elt_typ;
_tmp59A=_tmp598.ptr_atts;_tmp59B=_tmp59A.nullable;_tmp59C=_tmp59A.bounds;_tmp59D=
_tmp59A.zero_term;_LL38C: if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp59B)){const char*_tmp86E;void*_tmp86D;(_tmp86D=0,Cyc_Tcutil_terr(_tmp586,((
_tmp86E="initializes attribute not allowed on nullable pointers",_tag_dynforward(
_tmp86E,sizeof(char),_get_zero_arr_size_char(_tmp86E,55)))),_tag_dynforward(
_tmp86D,sizeof(void*),0)));}if(!Cyc_Tcutil_is_bound_one(_tmp59C)){const char*
_tmp871;void*_tmp870;(_tmp870=0,Cyc_Tcutil_terr(_tmp586,((_tmp871="initializes attribute allowed only on pointers of size 1",
_tag_dynforward(_tmp871,sizeof(char),_get_zero_arr_size_char(_tmp871,57)))),
_tag_dynforward(_tmp870,sizeof(void*),0)));}if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp59D)){const char*_tmp874;void*_tmp873;(_tmp873=0,
Cyc_Tcutil_terr(_tmp586,((_tmp874="initializes attribute allowed only on pointers to non-zero-terminated arrays",
_tag_dynforward(_tmp874,sizeof(char),_get_zero_arr_size_char(_tmp874,77)))),
_tag_dynforward(_tmp873,sizeof(void*),0)));}{struct Cyc_CfFlowInfo_InitParam_struct
_tmp877;struct Cyc_CfFlowInfo_InitParam_struct*_tmp876;struct Cyc_CfFlowInfo_InitParam_struct*
_tmp5A4=(_tmp876=_region_malloc(_tmp585,sizeof(*_tmp876)),((_tmp876[0]=((_tmp877.tag=
2,((_tmp877.f1=(int)j,((_tmp877.f2=(void*)_tmp599,_tmp877)))))),_tmp876)));
struct Cyc_CfFlowInfo_Place*_tmp878;struct Cyc_CfFlowInfo_Place*_tmp5A5=(_tmp878=
_region_malloc(_tmp585,sizeof(*_tmp878)),((_tmp878->root=(void*)((void*)_tmp5A4),((
_tmp878->fields=0,_tmp878)))));_tmp588=Cyc_Dict_insert(_tmp588,(void*)_tmp5A4,
Cyc_CfFlowInfo_typ_to_absrval(fenv,_tmp599,(void*)fenv->esc_none));{struct Cyc_CfFlowInfo_AddressOf_struct
_tmp87E;struct Cyc_CfFlowInfo_AddressOf_struct*_tmp87D;struct Cyc_CfFlowInfo_VarRoot_struct
_tmp87B;struct Cyc_CfFlowInfo_VarRoot_struct*_tmp87A;_tmp588=Cyc_Dict_insert(
_tmp588,(void*)((_tmp87A=_region_malloc(_tmp585,sizeof(*_tmp87A)),((_tmp87A[0]=((
_tmp87B.tag=0,((_tmp87B.f1=_tmp596,_tmp87B)))),_tmp87A)))),(void*)((_tmp87D=
_region_malloc(_tmp585,sizeof(*_tmp87D)),((_tmp87D[0]=((_tmp87E.tag=2,((_tmp87E.f1=
_tmp5A5,_tmp87E)))),_tmp87D)))));}{struct Cyc_List_List*_tmp87F;param_roots=((
_tmp87F=_region_malloc(_tmp585,sizeof(*_tmp87F)),((_tmp87F->hd=_tmp5A5,((_tmp87F->tl=
param_roots,_tmp87F))))));}goto _LL38A;}_LL38D:;_LL38E: {const char*_tmp882;void*
_tmp881;(_tmp881=0,Cyc_Tcutil_terr(_tmp586,((_tmp882="initializes attribute on non-pointer",
_tag_dynforward(_tmp882,sizeof(char),_get_zero_arr_size_char(_tmp882,37)))),
_tag_dynforward(_tmp881,sizeof(void*),0)));}_LL38A:;}goto _LL385;}}_LL388:;_LL389:
goto _LL385;_LL385:;}{union Cyc_CfFlowInfo_FlowInfo_union _tmp883;inflow=(union Cyc_CfFlowInfo_FlowInfo_union)(((
_tmp883.ReachableFL).tag=1,(((_tmp883.ReachableFL).f1=_tmp588,(((_tmp883.ReachableFL).f2=
_tmp589,(((_tmp883.ReachableFL).f3=_tmp58A,_tmp883))))))));}}_LL37B:;}{int
noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));struct Cyc_Hashtable_Table*
flow_table=((struct Cyc_Hashtable_Table*(*)(struct _RegionHandle*r,int sz,int(*cmp)(
struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))
Cyc_Hashtable_rcreate)(_tmp585,33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))
Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*_tmp884;struct Cyc_NewControlFlow_AnalEnv*env=(
_tmp884=_region_malloc(_tmp585,sizeof(*_tmp884)),((_tmp884->r=_tmp585,((_tmp884->fenv=
fenv,((_tmp884->iterate_again=1,((_tmp884->iteration_num=0,((_tmp884->in_try=0,((
_tmp884->tryflow=inflow,((_tmp884->all_changed=0,((_tmp884->noreturn=noreturn,((
_tmp884->param_roots=param_roots,((_tmp884->flow_table=flow_table,_tmp884)))))))))))))))))))));
union Cyc_CfFlowInfo_FlowInfo_union outflow=inflow;while(env->iterate_again  && !
Cyc_Position_error_p()){++ env->iteration_num;env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,fd->body);outflow=Cyc_CfFlowInfo_drop_unique_rvals((fd->body)->loc,
outflow);}{union Cyc_CfFlowInfo_FlowInfo_union _tmp5B1=outflow;_LL390: if((_tmp5B1.BottomFL).tag
!= 0)goto _LL392;_LL391: goto _LL38F;_LL392:;_LL393: Cyc_NewControlFlow_check_init_params(
_tmp586,env,outflow);if(noreturn){const char*_tmp887;void*_tmp886;(_tmp886=0,Cyc_Tcutil_terr(
_tmp586,((_tmp887="`noreturn' function might (implicitly) return",
_tag_dynforward(_tmp887,sizeof(char),_get_zero_arr_size_char(_tmp887,46)))),
_tag_dynforward(_tmp886,sizeof(void*),0)));}else{void*_tmp5B4=Cyc_Tcutil_compress((
void*)fd->ret_type);_LL395: if((int)_tmp5B4 != 0)goto _LL397;_LL396: goto _LL394;
_LL397: if(_tmp5B4 <= (void*)4)goto _LL399;if(*((int*)_tmp5B4)!= 6)goto _LL399;
_LL398: goto _LL39A;_LL399: if((int)_tmp5B4 != 1)goto _LL39B;_LL39A: goto _LL39C;_LL39B:
if(_tmp5B4 <= (void*)4)goto _LL39D;if(*((int*)_tmp5B4)!= 5)goto _LL39D;_LL39C:{
const char*_tmp88A;void*_tmp889;(_tmp889=0,Cyc_Tcutil_warn(_tmp586,((_tmp88A="function may not return a value",
_tag_dynforward(_tmp88A,sizeof(char),_get_zero_arr_size_char(_tmp88A,32)))),
_tag_dynforward(_tmp889,sizeof(void*),0)));}goto _LL394;_LL39D:;_LL39E:{const char*
_tmp88D;void*_tmp88C;(_tmp88C=0,Cyc_Tcutil_terr(_tmp586,((_tmp88D="function may not return a value",
_tag_dynforward(_tmp88D,sizeof(char),_get_zero_arr_size_char(_tmp88D,32)))),
_tag_dynforward(_tmp88C,sizeof(void*),0)));}goto _LL394;_LL394:;}goto _LL38F;
_LL38F:;}}}}void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);void Cyc_NewControlFlow_cf_check(
struct Cyc_List_List*ds){for(0;ds != 0;ds=ds->tl){void*_tmp5BA=(void*)((struct Cyc_Absyn_Decl*)
ds->hd)->r;struct Cyc_Absyn_Fndecl*_tmp5BB;struct Cyc_List_List*_tmp5BC;struct Cyc_List_List*
_tmp5BD;struct Cyc_List_List*_tmp5BE;_LL3A0: if(_tmp5BA <= (void*)2)goto _LL3AA;if(*((
int*)_tmp5BA)!= 1)goto _LL3A2;_tmp5BB=((struct Cyc_Absyn_Fn_d_struct*)_tmp5BA)->f1;
_LL3A1: Cyc_NewControlFlow_check_fun(_tmp5BB);goto _LL39F;_LL3A2: if(*((int*)
_tmp5BA)!= 10)goto _LL3A4;_tmp5BC=((struct Cyc_Absyn_ExternC_d_struct*)_tmp5BA)->f1;
_LL3A3: _tmp5BD=_tmp5BC;goto _LL3A5;_LL3A4: if(*((int*)_tmp5BA)!= 9)goto _LL3A6;
_tmp5BD=((struct Cyc_Absyn_Using_d_struct*)_tmp5BA)->f2;_LL3A5: _tmp5BE=_tmp5BD;
goto _LL3A7;_LL3A6: if(*((int*)_tmp5BA)!= 8)goto _LL3A8;_tmp5BE=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp5BA)->f2;_LL3A7: Cyc_NewControlFlow_cf_check(_tmp5BE);goto _LL39F;_LL3A8: if(*((
int*)_tmp5BA)!= 11)goto _LL3AA;_LL3A9: goto _LL39F;_LL3AA:;_LL3AB: goto _LL39F;_LL39F:;}}
