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
_dynforward_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;extern char Cyc_Core_Open_Region[
16];extern char Cyc_Core_Free_Region[16];struct Cyc___cycFILE;extern struct Cyc___cycFILE*
Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{int tag;
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
struct _dynforward_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dynforward_ptr,
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
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct
Cyc_List_List*x);void*Cyc_List_fold_right(void*(*f)(void*,void*),struct Cyc_List_List*
x,void*accum);struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_flatten(
struct Cyc_List_List*x);extern char Cyc_List_Nth[8];struct Cyc_List_List*Cyc_List_rzip(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple0
Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x);unsigned long Cyc_strlen(struct _dynforward_ptr s);int Cyc_strcmp(struct
_dynforward_ptr s1,struct _dynforward_ptr s2);int Cyc_zstrcmp(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_zstrptrcmp(struct _dynforward_ptr*,struct
_dynforward_ptr*);struct _dynforward_ptr Cyc_strcat(struct _dynforward_ptr dest,
struct _dynforward_ptr src);struct _dynforward_ptr Cyc_strcpy(struct _dynforward_ptr
dest,struct _dynforward_ptr src);struct _dynforward_ptr Cyc_substring(struct
_dynforward_ptr,int ofs,unsigned long n);struct Cyc_Lineno_Pos{struct
_dynforward_ptr logical_file;struct _dynforward_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Segment*
Cyc_Position_segment_of_abs(int,int);struct _dynforward_ptr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _dynforward_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dynforward_ptr desc;};
struct Cyc_Position_Error*Cyc_Position_mk_err_parse(struct Cyc_Position_Segment*,
struct _dynforward_ptr);extern char Cyc_Position_Nocontext[14];void Cyc_Position_post_error(
struct Cyc_Position_Error*);struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_is_qvar_qualified(
struct _tuple1*);struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual
x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_dynforward_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(void*,void*);extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_float_typ;void*Cyc_Absyn_double_typ(
int);void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,
struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_array_typ(void*elt_type,struct
Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*zero_term,
struct Cyc_Position_Segment*ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*
rgn_handle,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*,int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dynforward_ptr f,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dynforward_ptr s,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct
Cyc_Absyn_Exp*,int user_cast,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(struct Cyc_List_List*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_dynforward_ptr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dynforward_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dynforward_ptr*lab,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct
Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(
void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(
struct Cyc_List_List*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_AggrdeclImpl*
Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct
Cyc_List_List*fs);struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*k,void*s,struct
_tuple1*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(
void*s,struct _tuple1*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int
is_xtunion,int is_flat,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts);int Cyc_Absyn_fntype_att(void*a);extern int Cyc_Absyn_porting_c_code;
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;
struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct _tuple4{void*f1;void*f2;};struct _tuple4*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple4*Cyc_Dict_rchoose(struct
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
le;};void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,
struct _dynforward_ptr ap);void*Cyc_Tcutil_copy_type(void*t);void*Cyc_Tcutil_compress(
void*t);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_trk;
extern struct Cyc_Core_Opt Cyc_Tcutil_bk;extern struct Cyc_Core_Opt Cyc_Tcutil_ik;
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(void*k);void*Cyc_Tcutil_kind_to_bound(
void*k);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Lex_register_typedef(
struct _tuple1*s);void Cyc_Lex_enter_namespace(struct _dynforward_ptr*);void Cyc_Lex_leave_namespace();
void Cyc_Lex_enter_using(struct _tuple1*);void Cyc_Lex_leave_using();int Cyc_Parse_no_register=
0;struct Cyc_Parse_Signed_spec_struct{int tag;struct Cyc_Position_Segment*f1;};
struct Cyc_Parse_Unsigned_spec_struct{int tag;struct Cyc_Position_Segment*f1;};
struct Cyc_Parse_Short_spec_struct{int tag;struct Cyc_Position_Segment*f1;};struct
Cyc_Parse_Long_spec_struct{int tag;struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Type_spec_struct{
int tag;void*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Parse_Decl_spec_struct{
int tag;struct Cyc_Absyn_Decl*f1;};struct Cyc_Parse_Declaration_spec{struct Cyc_Core_Opt*
sc;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*type_specs;int is_inline;struct Cyc_List_List*
attributes;};struct Cyc_Parse_Declarator{struct _tuple1*id;struct Cyc_List_List*tms;
};struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};struct _tuple5{
void*f1;struct Cyc_Core_Opt*f2;};static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc);struct _tuple6{struct Cyc_Absyn_Tqual
f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};static struct _tuple6
Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
static struct Cyc_List_List*Cyc_Parse_parse_result=0;static void Cyc_Parse_err(
struct _dynforward_ptr msg,struct Cyc_Position_Segment*sg);static void Cyc_Parse_err(
struct _dynforward_ptr msg,struct Cyc_Position_Segment*sg){Cyc_Position_post_error(
Cyc_Position_mk_err_parse(sg,msg));}static void*Cyc_Parse_abort(struct Cyc_Position_Segment*
sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap);static void*Cyc_Parse_abort(
struct Cyc_Position_Segment*sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap){
Cyc_Parse_err((struct _dynforward_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap),
sg);(int)_throw((void*)Cyc_Position_Exit);}static void Cyc_Parse_unimp(struct
_dynforward_ptr msg,struct Cyc_Position_Segment*sg);static void Cyc_Parse_unimp(
struct _dynforward_ptr msg,struct Cyc_Position_Segment*sg){{const char*_tmp8F2;void*
_tmp8F1[2];struct Cyc_String_pa_struct _tmp8F0;struct Cyc_String_pa_struct _tmp8EF;(
_tmp8EF.tag=0,((_tmp8EF.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)msg),((
_tmp8F0.tag=0,((_tmp8F0.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(
sg)),((_tmp8F1[0]=& _tmp8F0,((_tmp8F1[1]=& _tmp8EF,Cyc_fprintf(Cyc_stderr,((
_tmp8F2="%s: Warning: Cyclone does not yet support %s\n",_tag_dynforward(_tmp8F2,
sizeof(char),_get_zero_arr_size_char(_tmp8F2,46)))),_tag_dynforward(_tmp8F1,
sizeof(void*),2)))))))))))));}return;}static int Cyc_Parse_enum_counter=0;struct
_tuple1*Cyc_Parse_gensym_enum();struct _tuple1*Cyc_Parse_gensym_enum(){union Cyc_Absyn_Nmspace_union
_tmp901;struct _dynforward_ptr*_tmp900;const char*_tmp8FF;void*_tmp8FE[1];struct
Cyc_Int_pa_struct _tmp8FD;struct _tuple1*_tmp8FC;return(_tmp8FC=_cycalloc(sizeof(*
_tmp8FC)),((_tmp8FC->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp901.Rel_n).tag=1,(((
_tmp901.Rel_n).f1=0,_tmp901)))),((_tmp8FC->f2=((_tmp900=_cycalloc(sizeof(*
_tmp900)),((_tmp900[0]=(struct _dynforward_ptr)((_tmp8FD.tag=1,((_tmp8FD.f1=(
unsigned long)Cyc_Parse_enum_counter ++,((_tmp8FE[0]=& _tmp8FD,Cyc_aprintf(((
_tmp8FF="__anonymous_enum_%d__",_tag_dynforward(_tmp8FF,sizeof(char),
_get_zero_arr_size_char(_tmp8FF,22)))),_tag_dynforward(_tmp8FE,sizeof(void*),1)))))))),
_tmp900)))),_tmp8FC)))));}struct _tuple7{struct _tuple1*f1;struct Cyc_Absyn_Tqual f2;
void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple8{struct
_tuple7*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(
struct Cyc_Position_Segment*loc,struct _tuple8*field_info);static struct Cyc_Absyn_Aggrfield*
Cyc_Parse_make_aggr_field(struct Cyc_Position_Segment*loc,struct _tuple8*
field_info){struct _tuple8 _tmpB;struct _tuple7*_tmpC;struct _tuple7 _tmpD;struct
_tuple1*_tmpE;struct Cyc_Absyn_Tqual _tmpF;void*_tmp10;struct Cyc_List_List*_tmp11;
struct Cyc_List_List*_tmp12;struct Cyc_Absyn_Exp*_tmp13;struct _tuple8*_tmpA=
field_info;_tmpB=*_tmpA;_tmpC=_tmpB.f1;_tmpD=*_tmpC;_tmpE=_tmpD.f1;_tmpF=_tmpD.f2;
_tmp10=_tmpD.f3;_tmp11=_tmpD.f4;_tmp12=_tmpD.f5;_tmp13=_tmpB.f2;if(_tmp11 != 0){
const char*_tmp902;Cyc_Parse_err(((_tmp902="bad type params in struct field",
_tag_dynforward(_tmp902,sizeof(char),_get_zero_arr_size_char(_tmp902,32)))),loc);}
if(Cyc_Absyn_is_qvar_qualified(_tmpE)){const char*_tmp903;Cyc_Parse_err(((_tmp903="struct or union field cannot be qualified with a namespace",
_tag_dynforward(_tmp903,sizeof(char),_get_zero_arr_size_char(_tmp903,59)))),loc);}{
struct Cyc_Absyn_Aggrfield*_tmp904;return(_tmp904=_cycalloc(sizeof(*_tmp904)),((
_tmp904->name=(*_tmpE).f2,((_tmp904->tq=_tmpF,((_tmp904->type=(void*)_tmp10,((
_tmp904->width=_tmp13,((_tmp904->attributes=_tmp12,_tmp904)))))))))));}}static
void*Cyc_Parse_type_spec(void*t,struct Cyc_Position_Segment*loc);static void*Cyc_Parse_type_spec(
void*t,struct Cyc_Position_Segment*loc){struct Cyc_Parse_Type_spec_struct _tmp907;
struct Cyc_Parse_Type_spec_struct*_tmp906;return(void*)((_tmp906=_cycalloc(
sizeof(*_tmp906)),((_tmp906[0]=((_tmp907.tag=4,((_tmp907.f1=(void*)t,((_tmp907.f2=
loc,_tmp907)))))),_tmp906))));}static void*Cyc_Parse_array2ptr(void*t,int argposn);
static void*Cyc_Parse_array2ptr(void*t,int argposn){void*_tmp19=t;struct Cyc_Absyn_ArrayInfo
_tmp1A;void*_tmp1B;struct Cyc_Absyn_Tqual _tmp1C;struct Cyc_Absyn_Exp*_tmp1D;struct
Cyc_Absyn_Conref*_tmp1E;struct Cyc_Position_Segment*_tmp1F;_LL1: if(_tmp19 <= (void*)
4)goto _LL3;if(*((int*)_tmp19)!= 7)goto _LL3;_tmp1A=((struct Cyc_Absyn_ArrayType_struct*)
_tmp19)->f1;_tmp1B=(void*)_tmp1A.elt_type;_tmp1C=_tmp1A.tq;_tmp1D=_tmp1A.num_elts;
_tmp1E=_tmp1A.zero_term;_tmp1F=_tmp1A.zt_loc;_LL2: {struct Cyc_Absyn_Upper_b_struct
_tmp90A;struct Cyc_Absyn_Upper_b_struct*_tmp909;return Cyc_Absyn_starb_typ(_tmp1B,
argposn?Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,0):(void*)2,
_tmp1C,_tmp1D == 0?(void*)0:(void*)((_tmp909=_cycalloc(sizeof(*_tmp909)),((
_tmp909[0]=((_tmp90A.tag=0,((_tmp90A.f1=(struct Cyc_Absyn_Exp*)_tmp1D,_tmp90A)))),
_tmp909)))),_tmp1E);}_LL3:;_LL4: return t;_LL0:;}static void Cyc_Parse_arg_array2ptr(
struct _tuple2*x);static void Cyc_Parse_arg_array2ptr(struct _tuple2*x){(*x).f3=Cyc_Parse_array2ptr((*
x).f3,1);}struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};static struct _tuple9*
Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*loc,struct _tuple2*t);static
struct _tuple9*Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*loc,struct
_tuple2*t){struct _tuple9*_tmp90B;return(_tmp90B=_cycalloc(sizeof(*_tmp90B)),((
_tmp90B->f1=(*t).f2,((_tmp90B->f2=(*t).f3,_tmp90B)))));}static struct Cyc_Absyn_Vardecl*
Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d);static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(
struct Cyc_Absyn_Decl*d){void*_tmp23=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp24;
_LL6: if(_tmp23 <= (void*)2)goto _LL8;if(*((int*)_tmp23)!= 0)goto _LL8;_tmp24=((
struct Cyc_Absyn_Var_d_struct*)_tmp23)->f1;_LL7: return _tmp24;_LL8:;_LL9: {const
char*_tmp90E;void*_tmp90D;(_tmp90D=0,((int(*)(struct Cyc_Position_Segment*sg,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(d->loc,((
_tmp90E="bad declaration in `forarray' statement",_tag_dynforward(_tmp90E,
sizeof(char),_get_zero_arr_size_char(_tmp90E,40)))),_tag_dynforward(_tmp90D,
sizeof(void*),0)));}_LL5:;}static int Cyc_Parse_is_typeparam(void*tm);static int Cyc_Parse_is_typeparam(
void*tm){void*_tmp27=tm;_LLB: if(*((int*)_tmp27)!= 4)goto _LLD;_LLC: return 1;_LLD:;
_LLE: return 0;_LLA:;}static void*Cyc_Parse_id2type(struct _dynforward_ptr s,void*k);
static void*Cyc_Parse_id2type(struct _dynforward_ptr s,void*k){const char*_tmp90F;
if(Cyc_zstrcmp((struct _dynforward_ptr)s,((_tmp90F="`H",_tag_dynforward(_tmp90F,
sizeof(char),_get_zero_arr_size_char(_tmp90F,3)))))== 0)return(void*)2;else{
const char*_tmp910;if(Cyc_zstrcmp((struct _dynforward_ptr)s,((_tmp910="`U",
_tag_dynforward(_tmp910,sizeof(char),_get_zero_arr_size_char(_tmp910,3)))))== 0)
return(void*)3;else{struct Cyc_Absyn_VarType_struct _tmp91A;struct _dynforward_ptr*
_tmp919;struct Cyc_Absyn_Tvar*_tmp918;struct Cyc_Absyn_VarType_struct*_tmp917;
return(void*)((_tmp917=_cycalloc(sizeof(*_tmp917)),((_tmp917[0]=((_tmp91A.tag=1,((
_tmp91A.f1=((_tmp918=_cycalloc(sizeof(*_tmp918)),((_tmp918->name=((_tmp919=
_cycalloc(sizeof(*_tmp919)),((_tmp919[0]=s,_tmp919)))),((_tmp918->identity=- 1,((
_tmp918->kind=(void*)k,_tmp918)))))))),_tmp91A)))),_tmp917))));}}}static struct
Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t);static struct Cyc_Absyn_Tvar*
Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){void*k;{void*_tmp2E=Cyc_Absyn_compress_kb((
void*)t->kind);void*_tmp2F;_LL10: if(*((int*)_tmp2E)!= 1)goto _LL12;_LL11:{struct
Cyc_Absyn_Unknown_kb_struct _tmp91D;struct Cyc_Absyn_Unknown_kb_struct*_tmp91C;k=(
void*)((_tmp91C=_cycalloc(sizeof(*_tmp91C)),((_tmp91C[0]=((_tmp91D.tag=1,((
_tmp91D.f1=0,_tmp91D)))),_tmp91C))));}goto _LLF;_LL12: if(*((int*)_tmp2E)!= 2)goto
_LL14;_tmp2F=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp2E)->f2;_LL13:{struct
Cyc_Absyn_Less_kb_struct _tmp920;struct Cyc_Absyn_Less_kb_struct*_tmp91F;k=(void*)((
_tmp91F=_cycalloc(sizeof(*_tmp91F)),((_tmp91F[0]=((_tmp920.tag=2,((_tmp920.f1=0,((
_tmp920.f2=(void*)_tmp2F,_tmp920)))))),_tmp91F))));}goto _LLF;_LL14:;_LL15: k=
_tmp2E;goto _LLF;_LLF:;}{struct Cyc_Absyn_Tvar*_tmp921;return(_tmp921=_cycalloc(
sizeof(*_tmp921)),((_tmp921->name=t->name,((_tmp921->identity=- 1,((_tmp921->kind=(
void*)k,_tmp921)))))));}}static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*
loc,void*t);static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*
loc,void*t){void*_tmp35=t;struct Cyc_Absyn_Tvar*_tmp36;_LL17: if(_tmp35 <= (void*)4)
goto _LL19;if(*((int*)_tmp35)!= 1)goto _LL19;_tmp36=((struct Cyc_Absyn_VarType_struct*)
_tmp35)->f1;_LL18: return _tmp36;_LL19:;_LL1A: {const char*_tmp924;void*_tmp923;(
_tmp923=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Parse_abort)(loc,((_tmp924="expecting a list of type variables, not types",
_tag_dynforward(_tmp924,sizeof(char),_get_zero_arr_size_char(_tmp924,46)))),
_tag_dynforward(_tmp923,sizeof(void*),0)));}_LL16:;}static void*Cyc_Parse_tvar2typ(
struct Cyc_Absyn_Tvar*pr);static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){
struct Cyc_Absyn_VarType_struct _tmp927;struct Cyc_Absyn_VarType_struct*_tmp926;
return(void*)((_tmp926=_cycalloc(sizeof(*_tmp926)),((_tmp926[0]=((_tmp927.tag=1,((
_tmp927.f1=pr,_tmp927)))),_tmp926))));}static void Cyc_Parse_set_vartyp_kind(void*
t,void*k,int leq);static void Cyc_Parse_set_vartyp_kind(void*t,void*k,int leq){void*
_tmp3B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp3C;struct Cyc_Absyn_Tvar
_tmp3D;void*_tmp3E;void**_tmp3F;_LL1C: if(_tmp3B <= (void*)4)goto _LL1E;if(*((int*)
_tmp3B)!= 1)goto _LL1E;_tmp3C=((struct Cyc_Absyn_VarType_struct*)_tmp3B)->f1;
_tmp3D=*_tmp3C;_tmp3E=(void*)_tmp3D.kind;_tmp3F=(void**)&(*((struct Cyc_Absyn_VarType_struct*)
_tmp3B)->f1).kind;_LL1D: {void*_tmp40=Cyc_Absyn_compress_kb(*_tmp3F);_LL21: if(*((
int*)_tmp40)!= 1)goto _LL23;_LL22: if(!leq)*_tmp3F=Cyc_Tcutil_kind_to_bound(k);
else{struct Cyc_Absyn_Less_kb_struct _tmp92A;struct Cyc_Absyn_Less_kb_struct*
_tmp929;*_tmp3F=(void*)((_tmp929=_cycalloc(sizeof(*_tmp929)),((_tmp929[0]=((
_tmp92A.tag=2,((_tmp92A.f1=0,((_tmp92A.f2=(void*)k,_tmp92A)))))),_tmp929))));}
return;_LL23:;_LL24: return;_LL20:;}_LL1E:;_LL1F: return;_LL1B:;}static struct Cyc_List_List*
Cyc_Parse_oldstyle2newstyle(struct Cyc_List_List*tms,struct Cyc_List_List*tds,
struct Cyc_Position_Segment*loc);static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmp43=(void*)tms->hd;void*_tmp44;
_LL26: if(*((int*)_tmp43)!= 3)goto _LL28;_tmp44=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp43)->f1;_LL27: if(tms->tl == 0  || Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 
0){void*_tmp45=_tmp44;struct Cyc_List_List*_tmp46;_LL2B: if(*((int*)_tmp45)!= 1)
goto _LL2D;_LL2C:{const char*_tmp92D;void*_tmp92C;(_tmp92C=0,Cyc_Tcutil_warn(loc,((
_tmp92D="function declaration with both new- and old-style parameter declarations; ignoring old-style",
_tag_dynforward(_tmp92D,sizeof(char),_get_zero_arr_size_char(_tmp92D,93)))),
_tag_dynforward(_tmp92C,sizeof(void*),0)));}return tms;_LL2D: if(*((int*)_tmp45)!= 
0)goto _LL2A;_tmp46=((struct Cyc_Absyn_NoTypes_struct*)_tmp45)->f1;_LL2E: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp46)!= ((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(tds)){const char*_tmp930;void*_tmp92F;(_tmp92F=0,((int(*)(struct
Cyc_Position_Segment*sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(
loc,((_tmp930="wrong number of parameter declarations in old-style function declaration",
_tag_dynforward(_tmp930,sizeof(char),_get_zero_arr_size_char(_tmp930,73)))),
_tag_dynforward(_tmp92F,sizeof(void*),0)));}{struct Cyc_List_List*_tmp4B=0;for(0;
_tmp46 != 0;_tmp46=_tmp46->tl){struct Cyc_List_List*_tmp4C=tds;for(0;_tmp4C != 0;
_tmp4C=_tmp4C->tl){struct Cyc_Absyn_Decl*_tmp4D=(struct Cyc_Absyn_Decl*)_tmp4C->hd;
void*_tmp4E=(void*)_tmp4D->r;struct Cyc_Absyn_Vardecl*_tmp4F;_LL30: if(_tmp4E <= (
void*)2)goto _LL32;if(*((int*)_tmp4E)!= 0)goto _LL32;_tmp4F=((struct Cyc_Absyn_Var_d_struct*)
_tmp4E)->f1;_LL31: if(Cyc_zstrptrcmp((*_tmp4F->name).f2,(struct _dynforward_ptr*)
_tmp46->hd)!= 0)continue;if(_tmp4F->initializer != 0){const char*_tmp933;void*
_tmp932;(_tmp932=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmp4D->loc,((_tmp933="initializer found in parameter declaration",
_tag_dynforward(_tmp933,sizeof(char),_get_zero_arr_size_char(_tmp933,43)))),
_tag_dynforward(_tmp932,sizeof(void*),0)));}if(Cyc_Absyn_is_qvar_qualified(
_tmp4F->name)){const char*_tmp936;void*_tmp935;(_tmp935=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmp4D->loc,((
_tmp936="namespaces forbidden in parameter declarations",_tag_dynforward(_tmp936,
sizeof(char),_get_zero_arr_size_char(_tmp936,47)))),_tag_dynforward(_tmp935,
sizeof(void*),0)));}{struct _tuple2*_tmp93C;struct Cyc_Core_Opt*_tmp93B;struct Cyc_List_List*
_tmp93A;_tmp4B=((_tmp93A=_cycalloc(sizeof(*_tmp93A)),((_tmp93A->hd=((_tmp93C=
_cycalloc(sizeof(*_tmp93C)),((_tmp93C->f1=((_tmp93B=_cycalloc(sizeof(*_tmp93B)),((
_tmp93B->v=(*_tmp4F->name).f2,_tmp93B)))),((_tmp93C->f2=_tmp4F->tq,((_tmp93C->f3=(
void*)_tmp4F->type,_tmp93C)))))))),((_tmp93A->tl=_tmp4B,_tmp93A))))));}goto L;
_LL32:;_LL33: {const char*_tmp93F;void*_tmp93E;(_tmp93E=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmp4D->loc,((
_tmp93F="nonvariable declaration in parameter type",_tag_dynforward(_tmp93F,
sizeof(char),_get_zero_arr_size_char(_tmp93F,42)))),_tag_dynforward(_tmp93E,
sizeof(void*),0)));}_LL2F:;}L: if(_tmp4C == 0){const char*_tmp943;void*_tmp942[1];
struct Cyc_String_pa_struct _tmp941;(_tmp941.tag=0,((_tmp941.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*((struct _dynforward_ptr*)_tmp46->hd)),((
_tmp942[0]=& _tmp941,((int(*)(struct Cyc_Position_Segment*sg,struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(loc,((_tmp943="%s is not given a type",
_tag_dynforward(_tmp943,sizeof(char),_get_zero_arr_size_char(_tmp943,23)))),
_tag_dynforward(_tmp942,sizeof(void*),1)))))));}}{struct Cyc_Absyn_Function_mod_struct*
_tmp952;struct Cyc_Absyn_WithTypes_struct*_tmp951;struct Cyc_Absyn_WithTypes_struct
_tmp950;struct Cyc_Absyn_Function_mod_struct _tmp94F;struct Cyc_List_List*_tmp94E;
return(_tmp94E=_cycalloc(sizeof(*_tmp94E)),((_tmp94E->hd=(void*)((void*)((
_tmp952=_cycalloc(sizeof(*_tmp952)),((_tmp952[0]=((_tmp94F.tag=3,((_tmp94F.f1=(
void*)((void*)((_tmp951=_cycalloc(sizeof(*_tmp951)),((_tmp951[0]=((_tmp950.tag=1,((
_tmp950.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp4B),((_tmp950.f2=0,((_tmp950.f3=0,((_tmp950.f4=0,((_tmp950.f5=0,_tmp950)))))))))))),
_tmp951))))),_tmp94F)))),_tmp952))))),((_tmp94E->tl=0,_tmp94E)))));}}_LL2A:;}
goto _LL29;_LL28:;_LL29: {struct Cyc_List_List*_tmp953;return(_tmp953=_cycalloc(
sizeof(*_tmp953)),((_tmp953->hd=(void*)((void*)tms->hd),((_tmp953->tl=Cyc_Parse_oldstyle2newstyle(
tms->tl,tds,loc),_tmp953)))));}_LL25:;}}struct _tuple10{struct _dynforward_ptr*f1;
struct Cyc_Absyn_Tqual f2;void*f3;};static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(
struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,struct Cyc_List_List*tds,
struct Cyc_Absyn_Stmt*body,struct Cyc_Position_Segment*loc);static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function(struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,
struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,struct Cyc_Position_Segment*loc){
if(tds != 0){struct Cyc_Parse_Declarator*_tmp954;d=((_tmp954=_cycalloc(sizeof(*
_tmp954)),((_tmp954->id=d->id,((_tmp954->tms=Cyc_Parse_oldstyle2newstyle(d->tms,
tds,loc),_tmp954))))));}{void*sc=(void*)2;struct Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual
tq=Cyc_Absyn_empty_tqual(0);int is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){
tss=((struct Cyc_Parse_Declaration_spec*)dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)
dso->v)->tq;is_inline=((struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;
atts=((struct Cyc_Parse_Declaration_spec*)dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)
dso->v)->sc != 0){void*_tmp63=(void*)((struct Cyc_Core_Opt*)_check_null(((struct
Cyc_Parse_Declaration_spec*)dso->v)->sc))->v;_LL35: if((int)_tmp63 != 1)goto _LL37;
_LL36: sc=(void*)3;goto _LL34;_LL37: if((int)_tmp63 != 3)goto _LL39;_LL38: sc=(void*)0;
goto _LL34;_LL39:;_LL3A:{const char*_tmp955;Cyc_Parse_err(((_tmp955="bad storage class on function",
_tag_dynforward(_tmp955,sizeof(char),_get_zero_arr_size_char(_tmp955,30)))),loc);}
goto _LL34;_LL34:;}}{void*_tmp66;struct Cyc_Core_Opt*_tmp67;struct _tuple5 _tmp65=
Cyc_Parse_collapse_type_specifiers(tss,loc);_tmp66=_tmp65.f1;_tmp67=_tmp65.f2;{
struct Cyc_Absyn_Tqual _tmp69;void*_tmp6A;struct Cyc_List_List*_tmp6B;struct Cyc_List_List*
_tmp6C;struct _tuple6 _tmp68=Cyc_Parse_apply_tms(tq,_tmp66,atts,d->tms);_tmp69=
_tmp68.f1;_tmp6A=_tmp68.f2;_tmp6B=_tmp68.f3;_tmp6C=_tmp68.f4;if(_tmp67 != 0){
const char*_tmp958;void*_tmp957;(_tmp957=0,Cyc_Tcutil_warn(loc,((_tmp958="nested type declaration within function prototype",
_tag_dynforward(_tmp958,sizeof(char),_get_zero_arr_size_char(_tmp958,50)))),
_tag_dynforward(_tmp957,sizeof(void*),0)));}if(_tmp6B != 0){const char*_tmp95B;
void*_tmp95A;(_tmp95A=0,Cyc_Tcutil_warn(loc,((_tmp95B="bad type params, ignoring",
_tag_dynforward(_tmp95B,sizeof(char),_get_zero_arr_size_char(_tmp95B,26)))),
_tag_dynforward(_tmp95A,sizeof(void*),0)));}{void*_tmp71=_tmp6A;struct Cyc_Absyn_FnInfo
_tmp72;struct Cyc_List_List*_tmp73;struct Cyc_Core_Opt*_tmp74;void*_tmp75;struct
Cyc_List_List*_tmp76;int _tmp77;struct Cyc_Absyn_VarargInfo*_tmp78;struct Cyc_List_List*
_tmp79;struct Cyc_List_List*_tmp7A;_LL3C: if(_tmp71 <= (void*)4)goto _LL3E;if(*((int*)
_tmp71)!= 8)goto _LL3E;_tmp72=((struct Cyc_Absyn_FnType_struct*)_tmp71)->f1;_tmp73=
_tmp72.tvars;_tmp74=_tmp72.effect;_tmp75=(void*)_tmp72.ret_typ;_tmp76=_tmp72.args;
_tmp77=_tmp72.c_varargs;_tmp78=_tmp72.cyc_varargs;_tmp79=_tmp72.rgn_po;_tmp7A=
_tmp72.attributes;_LL3D: {struct Cyc_List_List*_tmp7B=0;{struct Cyc_List_List*
_tmp7C=_tmp76;for(0;_tmp7C != 0;_tmp7C=_tmp7C->tl){struct _tuple2 _tmp7E;struct Cyc_Core_Opt*
_tmp7F;struct Cyc_Absyn_Tqual _tmp80;void*_tmp81;struct _tuple2*_tmp7D=(struct
_tuple2*)_tmp7C->hd;_tmp7E=*_tmp7D;_tmp7F=_tmp7E.f1;_tmp80=_tmp7E.f2;_tmp81=
_tmp7E.f3;if(_tmp7F == 0){{const char*_tmp95C;Cyc_Parse_err(((_tmp95C="missing argument variable in function prototype",
_tag_dynforward(_tmp95C,sizeof(char),_get_zero_arr_size_char(_tmp95C,48)))),loc);}{
struct _tuple10*_tmp95F;struct Cyc_List_List*_tmp95E;_tmp7B=((_tmp95E=_cycalloc(
sizeof(*_tmp95E)),((_tmp95E->hd=((_tmp95F=_cycalloc(sizeof(*_tmp95F)),((_tmp95F->f1=
_init_dynforward_ptr(_cycalloc(sizeof(struct _dynforward_ptr)),"?",sizeof(char),2),((
_tmp95F->f2=_tmp80,((_tmp95F->f3=_tmp81,_tmp95F)))))))),((_tmp95E->tl=_tmp7B,
_tmp95E))))));}}else{struct _tuple10*_tmp962;struct Cyc_List_List*_tmp961;_tmp7B=((
_tmp961=_cycalloc(sizeof(*_tmp961)),((_tmp961->hd=((_tmp962=_cycalloc(sizeof(*
_tmp962)),((_tmp962->f1=(struct _dynforward_ptr*)_tmp7F->v,((_tmp962->f2=_tmp80,((
_tmp962->f3=_tmp81,_tmp962)))))))),((_tmp961->tl=_tmp7B,_tmp961))))));}}}{struct
Cyc_Absyn_Fndecl*_tmp963;return(_tmp963=_cycalloc(sizeof(*_tmp963)),((_tmp963->sc=(
void*)sc,((_tmp963->name=d->id,((_tmp963->tvs=_tmp73,((_tmp963->is_inline=
is_inline,((_tmp963->effect=_tmp74,((_tmp963->ret_type=(void*)_tmp75,((_tmp963->args=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp7B),((
_tmp963->c_varargs=_tmp77,((_tmp963->cyc_varargs=_tmp78,((_tmp963->rgn_po=_tmp79,((
_tmp963->body=body,((_tmp963->cached_typ=0,((_tmp963->param_vardecls=0,((_tmp963->fn_vardecl=
0,((_tmp963->attributes=Cyc_List_append(_tmp7A,_tmp6C),_tmp963)))))))))))))))))))))))))))))));}}
_LL3E:;_LL3F: {const char*_tmp966;void*_tmp965;(_tmp965=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(loc,((
_tmp966="declarator is not a function prototype",_tag_dynforward(_tmp966,sizeof(
char),_get_zero_arr_size_char(_tmp966,39)))),_tag_dynforward(_tmp965,sizeof(void*),
0)));}_LL3B:;}}}}}static char _tmp8B[52]="at most one type may appear within a type specifier";
static struct _dynforward_ptr Cyc_Parse_msg1={_tmp8B,_tmp8B + 52};static char _tmp8C[
63]="const or volatile may appear only once within a type specifier";static
struct _dynforward_ptr Cyc_Parse_msg2={_tmp8C,_tmp8C + 63};static char _tmp8D[50]="type specifier includes more than one declaration";
static struct _dynforward_ptr Cyc_Parse_msg3={_tmp8D,_tmp8D + 50};static char _tmp8E[
60]="sign specifier may appear only once within a type specifier";static struct
_dynforward_ptr Cyc_Parse_msg4={_tmp8E,_tmp8E + 60};static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc);static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc){struct Cyc_Core_Opt*
declopt=0;int seen_type=0;int seen_sign=0;int seen_size=0;void*t=(void*)0;void*sz=(
void*)2;void*sgn=(void*)0;struct Cyc_Position_Segment*last_loc=loc;for(0;ts != 0;
ts=ts->tl){void*_tmp8F=(void*)ts->hd;void*_tmp90;struct Cyc_Position_Segment*
_tmp91;struct Cyc_Position_Segment*_tmp92;struct Cyc_Position_Segment*_tmp93;
struct Cyc_Position_Segment*_tmp94;struct Cyc_Position_Segment*_tmp95;struct Cyc_Absyn_Decl*
_tmp96;_LL41: if(*((int*)_tmp8F)!= 4)goto _LL43;_tmp90=(void*)((struct Cyc_Parse_Type_spec_struct*)
_tmp8F)->f1;_tmp91=((struct Cyc_Parse_Type_spec_struct*)_tmp8F)->f2;_LL42: if(
seen_type)Cyc_Parse_err(Cyc_Parse_msg1,_tmp91);last_loc=_tmp91;seen_type=1;t=
_tmp90;goto _LL40;_LL43: if(*((int*)_tmp8F)!= 0)goto _LL45;_tmp92=((struct Cyc_Parse_Signed_spec_struct*)
_tmp8F)->f1;_LL44: if(seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp92);if(seen_type){
const char*_tmp967;Cyc_Parse_err(((_tmp967="signed qualifier must come before type",
_tag_dynforward(_tmp967,sizeof(char),_get_zero_arr_size_char(_tmp967,39)))),
_tmp92);}last_loc=_tmp92;seen_sign=1;sgn=(void*)0;goto _LL40;_LL45: if(*((int*)
_tmp8F)!= 1)goto _LL47;_tmp93=((struct Cyc_Parse_Unsigned_spec_struct*)_tmp8F)->f1;
_LL46: if(seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp93);if(seen_type){const char*
_tmp968;Cyc_Parse_err(((_tmp968="signed qualifier must come before type",
_tag_dynforward(_tmp968,sizeof(char),_get_zero_arr_size_char(_tmp968,39)))),
_tmp93);}last_loc=_tmp93;seen_sign=1;sgn=(void*)1;goto _LL40;_LL47: if(*((int*)
_tmp8F)!= 2)goto _LL49;_tmp94=((struct Cyc_Parse_Short_spec_struct*)_tmp8F)->f1;
_LL48: if(seen_size){const char*_tmp969;Cyc_Parse_err(((_tmp969="integral size may appear only once within a type specifier",
_tag_dynforward(_tmp969,sizeof(char),_get_zero_arr_size_char(_tmp969,59)))),
_tmp94);}if(seen_type){const char*_tmp96A;Cyc_Parse_err(((_tmp96A="short modifier must come before base type",
_tag_dynforward(_tmp96A,sizeof(char),_get_zero_arr_size_char(_tmp96A,42)))),
_tmp94);}last_loc=_tmp94;seen_size=1;sz=(void*)1;goto _LL40;_LL49: if(*((int*)
_tmp8F)!= 3)goto _LL4B;_tmp95=((struct Cyc_Parse_Long_spec_struct*)_tmp8F)->f1;
_LL4A: if(seen_type){const char*_tmp96B;Cyc_Parse_err(((_tmp96B="long modifier must come before base type",
_tag_dynforward(_tmp96B,sizeof(char),_get_zero_arr_size_char(_tmp96B,41)))),
_tmp95);}if(seen_size){void*_tmp9C=sz;_LL4E: if((int)_tmp9C != 3)goto _LL50;_LL4F:
sz=(void*)4;goto _LL4D;_LL50:;_LL51:{const char*_tmp96C;Cyc_Parse_err(((_tmp96C="extra long in type specifier",
_tag_dynforward(_tmp96C,sizeof(char),_get_zero_arr_size_char(_tmp96C,29)))),
_tmp95);}goto _LL4D;_LL4D:;}else{sz=(void*)3;}last_loc=_tmp95;seen_size=1;goto
_LL40;_LL4B: if(*((int*)_tmp8F)!= 5)goto _LL40;_tmp96=((struct Cyc_Parse_Decl_spec_struct*)
_tmp8F)->f1;_LL4C: last_loc=_tmp96->loc;if(declopt == 0  && !seen_type){seen_type=1;{
void*_tmp9E=(void*)_tmp96->r;struct Cyc_Absyn_Aggrdecl*_tmp9F;struct Cyc_Absyn_Tuniondecl*
_tmpA0;struct Cyc_Absyn_Enumdecl*_tmpA1;_LL53: if(_tmp9E <= (void*)2)goto _LL59;if(*((
int*)_tmp9E)!= 4)goto _LL55;_tmp9F=((struct Cyc_Absyn_Aggr_d_struct*)_tmp9E)->f1;
_LL54: {struct Cyc_List_List*_tmpA2=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Parse_copy_tvar,_tmp9F->tvs));{struct Cyc_Absyn_AggrType_struct _tmp976;union
Cyc_Absyn_AggrInfoU_union _tmp975;struct Cyc_Absyn_AggrInfo _tmp974;struct Cyc_Absyn_AggrType_struct*
_tmp973;t=(void*)((_tmp973=_cycalloc(sizeof(*_tmp973)),((_tmp973[0]=((_tmp976.tag=
10,((_tmp976.f1=((_tmp974.aggr_info=(union Cyc_Absyn_AggrInfoU_union)(((_tmp975.UnknownAggr).tag=
0,(((_tmp975.UnknownAggr).f1=(void*)((void*)_tmp9F->kind),(((_tmp975.UnknownAggr).f2=
_tmp9F->name,_tmp975)))))),((_tmp974.targs=_tmpA2,_tmp974)))),_tmp976)))),
_tmp973))));}if(_tmp9F->impl != 0){struct Cyc_Core_Opt*_tmp977;declopt=((_tmp977=
_cycalloc(sizeof(*_tmp977)),((_tmp977->v=_tmp96,_tmp977))));}goto _LL52;}_LL55:
if(*((int*)_tmp9E)!= 5)goto _LL57;_tmpA0=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp9E)->f1;_LL56: {struct Cyc_List_List*_tmpA8=((struct Cyc_List_List*(*)(void*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,_tmpA0->tvs));struct Cyc_Core_Opt*
_tmp978;struct Cyc_Core_Opt*ropt=_tmpA0->is_flat?0:((_tmp978=_cycalloc(sizeof(*
_tmp978)),((_tmp978->v=(void*)((void*)2),_tmp978))));{struct Cyc_Absyn_TunionType_struct
_tmp987;union Cyc_Absyn_TunionInfoU_union _tmp986;struct Cyc_Absyn_Tuniondecl**
_tmp985;struct Cyc_Absyn_TunionInfo _tmp984;struct Cyc_Absyn_TunionType_struct*
_tmp983;t=(void*)((_tmp983=_cycalloc(sizeof(*_tmp983)),((_tmp983[0]=((_tmp987.tag=
2,((_tmp987.f1=((_tmp984.tunion_info=(union Cyc_Absyn_TunionInfoU_union)(((
_tmp986.KnownTunion).tag=1,(((_tmp986.KnownTunion).f1=((_tmp985=_cycalloc(
sizeof(*_tmp985)),((_tmp985[0]=_tmpA0,_tmp985)))),_tmp986)))),((_tmp984.targs=
_tmpA8,((_tmp984.rgn=ropt,_tmp984)))))),_tmp987)))),_tmp983))));}if(_tmpA0->fields
!= 0){struct Cyc_Core_Opt*_tmp988;declopt=((_tmp988=_cycalloc(sizeof(*_tmp988)),((
_tmp988->v=_tmp96,_tmp988))));}goto _LL52;}_LL57: if(*((int*)_tmp9E)!= 6)goto _LL59;
_tmpA1=((struct Cyc_Absyn_Enum_d_struct*)_tmp9E)->f1;_LL58:{struct Cyc_Absyn_EnumType_struct
_tmp98B;struct Cyc_Absyn_EnumType_struct*_tmp98A;t=(void*)((_tmp98A=_cycalloc(
sizeof(*_tmp98A)),((_tmp98A[0]=((_tmp98B.tag=12,((_tmp98B.f1=_tmpA1->name,((
_tmp98B.f2=0,_tmp98B)))))),_tmp98A))));}{struct Cyc_Core_Opt*_tmp98C;declopt=((
_tmp98C=_cycalloc(sizeof(*_tmp98C)),((_tmp98C->v=_tmp96,_tmp98C))));}goto _LL52;
_LL59:;_LL5A: {const char*_tmp98F;void*_tmp98E;(_tmp98E=0,((int(*)(struct Cyc_Position_Segment*
sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmp96->loc,((
_tmp98F="bad declaration within type specifier",_tag_dynforward(_tmp98F,sizeof(
char),_get_zero_arr_size_char(_tmp98F,38)))),_tag_dynforward(_tmp98E,sizeof(void*),
0)));}_LL52:;}}else{Cyc_Parse_err(Cyc_Parse_msg3,_tmp96->loc);}goto _LL40;_LL40:;}
if(!seen_type){if(!seen_sign  && !seen_size){const char*_tmp992;void*_tmp991;(
_tmp991=0,Cyc_Tcutil_warn(last_loc,((_tmp992="missing type within specifier",
_tag_dynforward(_tmp992,sizeof(char),_get_zero_arr_size_char(_tmp992,30)))),
_tag_dynforward(_tmp991,sizeof(void*),0)));}t=Cyc_Absyn_int_typ(sgn,sz);}else{
if(seen_sign){void*_tmpB7=t;void*_tmpB8;void*_tmpB9;_LL5C: if(_tmpB7 <= (void*)4)
goto _LL5E;if(*((int*)_tmpB7)!= 5)goto _LL5E;_tmpB8=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpB7)->f1;_tmpB9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpB7)->f2;_LL5D:
if(_tmpB8 != sgn)t=Cyc_Absyn_int_typ(sgn,_tmpB9);goto _LL5B;_LL5E:;_LL5F:{const
char*_tmp993;Cyc_Parse_err(((_tmp993="sign specification on non-integral type",
_tag_dynforward(_tmp993,sizeof(char),_get_zero_arr_size_char(_tmp993,40)))),
last_loc);}goto _LL5B;_LL5B:;}if(seen_size){void*_tmpBB=t;void*_tmpBC;void*_tmpBD;
_LL61: if(_tmpBB <= (void*)4)goto _LL65;if(*((int*)_tmpBB)!= 5)goto _LL63;_tmpBC=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmpBB)->f1;_tmpBD=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpBB)->f2;_LL62: if(_tmpBD != sz)t=Cyc_Absyn_int_typ(_tmpBC,sz);goto _LL60;_LL63:
if(*((int*)_tmpBB)!= 6)goto _LL65;_LL64: t=Cyc_Absyn_double_typ(1);goto _LL60;_LL65:;
_LL66:{const char*_tmp994;Cyc_Parse_err(((_tmp994="size qualifier on non-integral type",
_tag_dynforward(_tmp994,sizeof(char),_get_zero_arr_size_char(_tmp994,36)))),
last_loc);}goto _LL60;_LL60:;}}{struct _tuple5 _tmp995;return(_tmp995.f1=t,((
_tmp995.f2=declopt,_tmp995)));}}static struct Cyc_List_List*Cyc_Parse_apply_tmss(
struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*ds,
struct Cyc_List_List*shared_atts);static struct Cyc_List_List*Cyc_Parse_apply_tmss(
struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*ds,
struct Cyc_List_List*shared_atts){if(ds == 0)return 0;{struct Cyc_Parse_Declarator*
_tmpC0=(struct Cyc_Parse_Declarator*)ds->hd;struct _tuple1*_tmpC1=_tmpC0->id;
struct Cyc_Absyn_Tqual _tmpC3;void*_tmpC4;struct Cyc_List_List*_tmpC5;struct Cyc_List_List*
_tmpC6;struct _tuple6 _tmpC2=Cyc_Parse_apply_tms(tq,t,shared_atts,_tmpC0->tms);
_tmpC3=_tmpC2.f1;_tmpC4=_tmpC2.f2;_tmpC5=_tmpC2.f3;_tmpC6=_tmpC2.f4;if(ds->tl == 
0){struct _tuple7*_tmp998;struct Cyc_List_List*_tmp997;return(_tmp997=
_region_malloc(r,sizeof(*_tmp997)),((_tmp997->hd=((_tmp998=_region_malloc(r,
sizeof(*_tmp998)),((_tmp998->f1=_tmpC1,((_tmp998->f2=_tmpC3,((_tmp998->f3=_tmpC4,((
_tmp998->f4=_tmpC5,((_tmp998->f5=_tmpC6,_tmp998)))))))))))),((_tmp997->tl=0,
_tmp997)))));}else{struct _tuple7*_tmp99B;struct Cyc_List_List*_tmp99A;return(
_tmp99A=_region_malloc(r,sizeof(*_tmp99A)),((_tmp99A->hd=((_tmp99B=
_region_malloc(r,sizeof(*_tmp99B)),((_tmp99B->f1=_tmpC1,((_tmp99B->f2=_tmpC3,((
_tmp99B->f3=_tmpC4,((_tmp99B->f4=_tmpC5,((_tmp99B->f5=_tmpC6,_tmp99B)))))))))))),((
_tmp99A->tl=Cyc_Parse_apply_tmss(r,_tmpC3,Cyc_Tcutil_copy_type(t),ds->tl,
shared_atts),_tmp99A)))));}}}static struct _tuple6 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms);static struct _tuple6
Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,
struct Cyc_List_List*tms){if(tms == 0){struct _tuple6 _tmp99C;return(_tmp99C.f1=tq,((
_tmp99C.f2=t,((_tmp99C.f3=0,((_tmp99C.f4=atts,_tmp99C)))))));}{void*_tmpCC=(void*)
tms->hd;struct Cyc_Absyn_Conref*_tmpCD;struct Cyc_Position_Segment*_tmpCE;struct
Cyc_Absyn_Exp*_tmpCF;struct Cyc_Absyn_Conref*_tmpD0;struct Cyc_Position_Segment*
_tmpD1;void*_tmpD2;struct Cyc_List_List*_tmpD3;struct Cyc_Position_Segment*_tmpD4;
struct Cyc_Absyn_PtrAtts _tmpD5;struct Cyc_Absyn_Tqual _tmpD6;struct Cyc_Position_Segment*
_tmpD7;struct Cyc_List_List*_tmpD8;_LL68: if(*((int*)_tmpCC)!= 0)goto _LL6A;_tmpCD=((
struct Cyc_Absyn_Carray_mod_struct*)_tmpCC)->f1;_tmpCE=((struct Cyc_Absyn_Carray_mod_struct*)
_tmpCC)->f2;_LL69: return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(
t,tq,0,_tmpCD,_tmpCE),atts,tms->tl);_LL6A: if(*((int*)_tmpCC)!= 1)goto _LL6C;
_tmpCF=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpCC)->f1;_tmpD0=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpCC)->f2;_tmpD1=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpCC)->f3;_LL6B:
return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,(
struct Cyc_Absyn_Exp*)_tmpCF,_tmpD0,_tmpD1),atts,tms->tl);_LL6C: if(*((int*)_tmpCC)
!= 3)goto _LL6E;_tmpD2=(void*)((struct Cyc_Absyn_Function_mod_struct*)_tmpCC)->f1;
_LL6D: {void*_tmpD9=_tmpD2;struct Cyc_List_List*_tmpDA;int _tmpDB;struct Cyc_Absyn_VarargInfo*
_tmpDC;struct Cyc_Core_Opt*_tmpDD;struct Cyc_List_List*_tmpDE;struct Cyc_Position_Segment*
_tmpDF;_LL75: if(*((int*)_tmpD9)!= 1)goto _LL77;_tmpDA=((struct Cyc_Absyn_WithTypes_struct*)
_tmpD9)->f1;_tmpDB=((struct Cyc_Absyn_WithTypes_struct*)_tmpD9)->f2;_tmpDC=((
struct Cyc_Absyn_WithTypes_struct*)_tmpD9)->f3;_tmpDD=((struct Cyc_Absyn_WithTypes_struct*)
_tmpD9)->f4;_tmpDE=((struct Cyc_Absyn_WithTypes_struct*)_tmpD9)->f5;_LL76: {
struct Cyc_List_List*typvars=0;struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*
new_atts=0;{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){if(Cyc_Absyn_fntype_att((
void*)as->hd)){struct Cyc_List_List*_tmp99D;fn_atts=((_tmp99D=_cycalloc(sizeof(*
_tmp99D)),((_tmp99D->hd=(void*)((void*)as->hd),((_tmp99D->tl=fn_atts,_tmp99D))))));}
else{struct Cyc_List_List*_tmp99E;new_atts=((_tmp99E=_cycalloc(sizeof(*_tmp99E)),((
_tmp99E->hd=(void*)((void*)as->hd),((_tmp99E->tl=new_atts,_tmp99E))))));}}}if(
tms->tl != 0){void*_tmpE2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;
struct Cyc_List_List*_tmpE3;_LL7A: if(*((int*)_tmpE2)!= 4)goto _LL7C;_tmpE3=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpE2)->f1;_LL7B: typvars=_tmpE3;tms=tms->tl;
goto _LL79;_LL7C:;_LL7D: goto _LL79;_LL79:;}if(((((!_tmpDB  && _tmpDC == 0) && _tmpDA
!= 0) && _tmpDA->tl == 0) && (*((struct _tuple2*)_tmpDA->hd)).f1 == 0) && (*((
struct _tuple2*)_tmpDA->hd)).f3 == (void*)0)_tmpDA=0;t=Cyc_Parse_array2ptr(t,0);((
void(*)(void(*f)(struct _tuple2*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Parse_arg_array2ptr,
_tmpDA);return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(tq.loc),Cyc_Absyn_function_typ(
typvars,_tmpDD,t,_tmpDA,_tmpDB,_tmpDC,_tmpDE,fn_atts),new_atts,((struct Cyc_List_List*)
_check_null(tms))->tl);}_LL77: if(*((int*)_tmpD9)!= 0)goto _LL74;_tmpDF=((struct
Cyc_Absyn_NoTypes_struct*)_tmpD9)->f2;_LL78: {const char*_tmp9A1;void*_tmp9A0;(
_tmp9A0=0,((int(*)(struct Cyc_Position_Segment*sg,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Parse_abort)(_tmpDF,((_tmp9A1="function declaration without parameter types",
_tag_dynforward(_tmp9A1,sizeof(char),_get_zero_arr_size_char(_tmp9A1,45)))),
_tag_dynforward(_tmp9A0,sizeof(void*),0)));}_LL74:;}_LL6E: if(*((int*)_tmpCC)!= 4)
goto _LL70;_tmpD3=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCC)->f1;_tmpD4=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCC)->f2;_LL6F: if(tms->tl == 0){struct
_tuple6 _tmp9A2;return(_tmp9A2.f1=tq,((_tmp9A2.f2=t,((_tmp9A2.f3=_tmpD3,((_tmp9A2.f4=
atts,_tmp9A2)))))));}{const char*_tmp9A5;void*_tmp9A4;(_tmp9A4=0,((int(*)(struct
Cyc_Position_Segment*sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(
_tmpD4,((_tmp9A5="type parameters must appear before function arguments in declarator",
_tag_dynforward(_tmp9A5,sizeof(char),_get_zero_arr_size_char(_tmp9A5,68)))),
_tag_dynforward(_tmp9A4,sizeof(void*),0)));}_LL70: if(*((int*)_tmpCC)!= 2)goto
_LL72;_tmpD5=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpCC)->f1;_tmpD6=((struct
Cyc_Absyn_Pointer_mod_struct*)_tmpCC)->f2;_LL71: {struct Cyc_Absyn_PointerType_struct
_tmp9AB;struct Cyc_Absyn_PtrInfo _tmp9AA;struct Cyc_Absyn_PointerType_struct*
_tmp9A9;return Cyc_Parse_apply_tms(_tmpD6,(void*)((_tmp9A9=_cycalloc(sizeof(*
_tmp9A9)),((_tmp9A9[0]=((_tmp9AB.tag=4,((_tmp9AB.f1=((_tmp9AA.elt_typ=(void*)t,((
_tmp9AA.elt_tq=tq,((_tmp9AA.ptr_atts=_tmpD5,_tmp9AA)))))),_tmp9AB)))),_tmp9A9)))),
atts,tms->tl);}_LL72: if(*((int*)_tmpCC)!= 5)goto _LL67;_tmpD7=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmpCC)->f1;_tmpD8=((struct Cyc_Absyn_Attributes_mod_struct*)_tmpCC)->f2;_LL73:
return Cyc_Parse_apply_tms(tq,t,Cyc_List_append(atts,_tmpD8),tms->tl);_LL67:;}}
void*Cyc_Parse_speclist2typ(struct Cyc_List_List*tss,struct Cyc_Position_Segment*
loc);void*Cyc_Parse_speclist2typ(struct Cyc_List_List*tss,struct Cyc_Position_Segment*
loc){void*_tmpED;struct Cyc_Core_Opt*_tmpEE;struct _tuple5 _tmpEC=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmpED=_tmpEC.f1;_tmpEE=_tmpEC.f2;if(_tmpEE != 0){const char*_tmp9AE;void*
_tmp9AD;(_tmp9AD=0,Cyc_Tcutil_warn(loc,((_tmp9AE="ignoring nested type declaration(s) in specifier list",
_tag_dynforward(_tmp9AE,sizeof(char),_get_zero_arr_size_char(_tmp9AE,54)))),
_tag_dynforward(_tmp9AD,sizeof(void*),0)));}return _tmpED;}static struct Cyc_Absyn_Decl*
Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*loc,struct _tuple7*t);
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*
loc,struct _tuple7*t){struct _tuple7 _tmpF2;struct _tuple1*_tmpF3;struct Cyc_Absyn_Tqual
_tmpF4;void*_tmpF5;struct Cyc_List_List*_tmpF6;struct Cyc_List_List*_tmpF7;struct
_tuple7*_tmpF1=t;_tmpF2=*_tmpF1;_tmpF3=_tmpF2.f1;_tmpF4=_tmpF2.f2;_tmpF5=_tmpF2.f3;
_tmpF6=_tmpF2.f4;_tmpF7=_tmpF2.f5;Cyc_Lex_register_typedef(_tmpF3);{struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*type;{void*_tmpF8=_tmpF5;struct Cyc_Core_Opt*_tmpF9;_LL7F:
if(_tmpF8 <= (void*)4)goto _LL81;if(*((int*)_tmpF8)!= 0)goto _LL81;_tmpF9=((struct
Cyc_Absyn_Evar_struct*)_tmpF8)->f1;_LL80: type=0;if(_tmpF9 == 0)kind=(struct Cyc_Core_Opt*)&
Cyc_Tcutil_bk;else{kind=_tmpF9;}goto _LL7E;_LL81:;_LL82: kind=0;{struct Cyc_Core_Opt*
_tmp9AF;type=((_tmp9AF=_cycalloc(sizeof(*_tmp9AF)),((_tmp9AF->v=(void*)_tmpF5,
_tmp9AF))));}goto _LL7E;_LL7E:;}{struct Cyc_Absyn_Typedef_d_struct _tmp9B5;struct
Cyc_Absyn_Typedefdecl*_tmp9B4;struct Cyc_Absyn_Typedef_d_struct*_tmp9B3;return Cyc_Absyn_new_decl((
void*)((_tmp9B3=_cycalloc(sizeof(*_tmp9B3)),((_tmp9B3[0]=((_tmp9B5.tag=7,((
_tmp9B5.f1=((_tmp9B4=_cycalloc(sizeof(*_tmp9B4)),((_tmp9B4->name=_tmpF3,((
_tmp9B4->tvs=_tmpF6,((_tmp9B4->kind=kind,((_tmp9B4->defn=type,((_tmp9B4->atts=
_tmpF7,((_tmp9B4->tq=_tmpF4,_tmp9B4)))))))))))))),_tmp9B5)))),_tmp9B3)))),loc);}}}
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct
Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*
d,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Decl_s_struct _tmp9B8;struct Cyc_Absyn_Decl_s_struct*
_tmp9B7;return Cyc_Absyn_new_stmt((void*)((_tmp9B7=_cycalloc(sizeof(*_tmp9B7)),((
_tmp9B7[0]=((_tmp9B8.tag=11,((_tmp9B8.f1=d,((_tmp9B8.f2=s,_tmp9B8)))))),_tmp9B7)))),
d->loc);}static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*
ds,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(
struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(
struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*
x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*
ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*
loc);static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*
ds,struct Cyc_List_List*ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*
loc){struct _RegionHandle _tmp100=_new_region("mkrgn");struct _RegionHandle*mkrgn=&
_tmp100;_push_region(mkrgn);{struct Cyc_Parse_Declaration_spec _tmp102;struct Cyc_Absyn_Tqual
_tmp103;struct Cyc_List_List*_tmp104;struct Cyc_List_List*_tmp105;struct Cyc_Parse_Declaration_spec*
_tmp101=ds;_tmp102=*_tmp101;_tmp103=_tmp102.tq;_tmp104=_tmp102.type_specs;
_tmp105=_tmp102.attributes;if(_tmp103.loc == 0)_tmp103.loc=tqual_loc;if(ds->is_inline){
const char*_tmp9B9;Cyc_Parse_err(((_tmp9B9="inline is allowed only on function definitions",
_tag_dynforward(_tmp9B9,sizeof(char),_get_zero_arr_size_char(_tmp9B9,47)))),loc);}
if(_tmp104 == 0){{const char*_tmp9BA;Cyc_Parse_err(((_tmp9BA="missing type specifiers in declaration",
_tag_dynforward(_tmp9BA,sizeof(char),_get_zero_arr_size_char(_tmp9BA,39)))),loc);}{
struct Cyc_List_List*_tmp108=0;_npop_handler(0);return _tmp108;}}{void*s=(void*)2;
int istypedef=0;if(ds->sc != 0){void*_tmp109=(void*)((struct Cyc_Core_Opt*)
_check_null(ds->sc))->v;_LL84: if((int)_tmp109 != 0)goto _LL86;_LL85: istypedef=1;
goto _LL83;_LL86: if((int)_tmp109 != 1)goto _LL88;_LL87: s=(void*)3;goto _LL83;_LL88:
if((int)_tmp109 != 2)goto _LL8A;_LL89: s=(void*)4;goto _LL83;_LL8A: if((int)_tmp109 != 
3)goto _LL8C;_LL8B: s=(void*)0;goto _LL83;_LL8C: if((int)_tmp109 != 4)goto _LL8E;_LL8D:
s=(void*)2;goto _LL83;_LL8E: if((int)_tmp109 != 5)goto _LL90;_LL8F: if(Cyc_Parse_no_register)
s=(void*)2;else{s=(void*)5;}goto _LL83;_LL90: if((int)_tmp109 != 6)goto _LL83;_LL91:
s=(void*)1;goto _LL83;_LL83:;}{struct Cyc_List_List*_tmp10B;struct Cyc_List_List*
_tmp10C;struct _tuple0 _tmp10A=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(mkrgn,mkrgn,ids);
_tmp10B=_tmp10A.f1;_tmp10C=_tmp10A.f2;{int exps_empty=1;{struct Cyc_List_List*es=
_tmp10C;for(0;es != 0;es=es->tl){if((struct Cyc_Absyn_Exp*)es->hd != 0){exps_empty=
0;break;}}}{struct _tuple5 _tmp10D=Cyc_Parse_collapse_type_specifiers(_tmp104,loc);
if(_tmp10B == 0){void*_tmp10F;struct Cyc_Core_Opt*_tmp110;struct _tuple5 _tmp10E=
_tmp10D;_tmp10F=_tmp10E.f1;_tmp110=_tmp10E.f2;if(_tmp110 != 0){struct Cyc_Absyn_Decl*
d=(struct Cyc_Absyn_Decl*)_tmp110->v;{void*_tmp111=(void*)d->r;struct Cyc_Absyn_Enumdecl*
_tmp112;struct Cyc_Absyn_Aggrdecl*_tmp113;struct Cyc_Absyn_Tuniondecl*_tmp114;
_LL93: if(_tmp111 <= (void*)2)goto _LL99;if(*((int*)_tmp111)!= 6)goto _LL95;_tmp112=((
struct Cyc_Absyn_Enum_d_struct*)_tmp111)->f1;_LL94:(void*)(_tmp112->sc=(void*)s);
if(_tmp105 != 0){const char*_tmp9BB;Cyc_Parse_err(((_tmp9BB="bad attributes on enum",
_tag_dynforward(_tmp9BB,sizeof(char),_get_zero_arr_size_char(_tmp9BB,23)))),loc);}
goto _LL92;_LL95: if(*((int*)_tmp111)!= 4)goto _LL97;_tmp113=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp111)->f1;_LL96:(void*)(_tmp113->sc=(void*)s);_tmp113->attributes=_tmp105;
goto _LL92;_LL97: if(*((int*)_tmp111)!= 5)goto _LL99;_tmp114=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp111)->f1;_LL98:(void*)(_tmp114->sc=(void*)s);if(_tmp105 != 0){const char*
_tmp9BC;Cyc_Parse_err(((_tmp9BC="bad attributes on tunion",_tag_dynforward(
_tmp9BC,sizeof(char),_get_zero_arr_size_char(_tmp9BC,25)))),loc);}goto _LL92;
_LL99:;_LL9A:{const char*_tmp9BD;Cyc_Parse_err(((_tmp9BD="bad declaration",
_tag_dynforward(_tmp9BD,sizeof(char),_get_zero_arr_size_char(_tmp9BD,16)))),loc);}{
struct Cyc_List_List*_tmp118=0;_npop_handler(0);return _tmp118;}_LL92:;}{struct Cyc_List_List*
_tmp9BE;struct Cyc_List_List*_tmp11A=(_tmp9BE=_cycalloc(sizeof(*_tmp9BE)),((
_tmp9BE->hd=d,((_tmp9BE->tl=0,_tmp9BE)))));_npop_handler(0);return _tmp11A;}}
else{void*_tmp11B=_tmp10F;struct Cyc_Absyn_AggrInfo _tmp11C;union Cyc_Absyn_AggrInfoU_union
_tmp11D;void*_tmp11E;struct _tuple1*_tmp11F;struct Cyc_List_List*_tmp120;struct Cyc_Absyn_TunionInfo
_tmp121;union Cyc_Absyn_TunionInfoU_union _tmp122;struct Cyc_Absyn_Tuniondecl**
_tmp123;struct Cyc_Absyn_TunionInfo _tmp124;union Cyc_Absyn_TunionInfoU_union
_tmp125;struct Cyc_Absyn_UnknownTunionInfo _tmp126;struct _tuple1*_tmp127;int
_tmp128;int _tmp129;struct Cyc_List_List*_tmp12A;struct _tuple1*_tmp12B;struct Cyc_List_List*
_tmp12C;_LL9C: if(_tmp11B <= (void*)4)goto _LLA6;if(*((int*)_tmp11B)!= 10)goto _LL9E;
_tmp11C=((struct Cyc_Absyn_AggrType_struct*)_tmp11B)->f1;_tmp11D=_tmp11C.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmp11B)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL9E;_tmp11E=(_tmp11D.UnknownAggr).f1;_tmp11F=(_tmp11D.UnknownAggr).f2;
_tmp120=_tmp11C.targs;_LL9D: {struct Cyc_List_List*_tmp12D=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp120);
struct Cyc_Absyn_Aggrdecl*_tmp9BF;struct Cyc_Absyn_Aggrdecl*_tmp12E=(_tmp9BF=
_cycalloc(sizeof(*_tmp9BF)),((_tmp9BF->kind=(void*)_tmp11E,((_tmp9BF->sc=(void*)
s,((_tmp9BF->name=_tmp11F,((_tmp9BF->tvs=_tmp12D,((_tmp9BF->impl=0,((_tmp9BF->attributes=
0,_tmp9BF)))))))))))));if(_tmp105 != 0){const char*_tmp9C0;Cyc_Parse_err(((_tmp9C0="bad attributes on type declaration",
_tag_dynforward(_tmp9C0,sizeof(char),_get_zero_arr_size_char(_tmp9C0,35)))),loc);}{
struct Cyc_Absyn_Aggr_d_struct*_tmp9C6;struct Cyc_Absyn_Aggr_d_struct _tmp9C5;
struct Cyc_List_List*_tmp9C4;struct Cyc_List_List*_tmp133=(_tmp9C4=_cycalloc(
sizeof(*_tmp9C4)),((_tmp9C4->hd=Cyc_Absyn_new_decl((void*)((_tmp9C6=_cycalloc(
sizeof(*_tmp9C6)),((_tmp9C6[0]=((_tmp9C5.tag=4,((_tmp9C5.f1=_tmp12E,_tmp9C5)))),
_tmp9C6)))),loc),((_tmp9C4->tl=0,_tmp9C4)))));_npop_handler(0);return _tmp133;}}
_LL9E: if(*((int*)_tmp11B)!= 2)goto _LLA0;_tmp121=((struct Cyc_Absyn_TunionType_struct*)
_tmp11B)->f1;_tmp122=_tmp121.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp11B)->f1).tunion_info).KnownTunion).tag != 1)goto _LLA0;_tmp123=(_tmp122.KnownTunion).f1;
_LL9F: if(_tmp105 != 0){const char*_tmp9C7;Cyc_Parse_err(((_tmp9C7="bad attributes on tunion",
_tag_dynforward(_tmp9C7,sizeof(char),_get_zero_arr_size_char(_tmp9C7,25)))),loc);}{
struct Cyc_Absyn_Tunion_d_struct*_tmp9CD;struct Cyc_Absyn_Tunion_d_struct _tmp9CC;
struct Cyc_List_List*_tmp9CB;struct Cyc_List_List*_tmp139=(_tmp9CB=_cycalloc(
sizeof(*_tmp9CB)),((_tmp9CB->hd=Cyc_Absyn_new_decl((void*)((_tmp9CD=_cycalloc(
sizeof(*_tmp9CD)),((_tmp9CD[0]=((_tmp9CC.tag=5,((_tmp9CC.f1=*_tmp123,_tmp9CC)))),
_tmp9CD)))),loc),((_tmp9CB->tl=0,_tmp9CB)))));_npop_handler(0);return _tmp139;}
_LLA0: if(*((int*)_tmp11B)!= 2)goto _LLA2;_tmp124=((struct Cyc_Absyn_TunionType_struct*)
_tmp11B)->f1;_tmp125=_tmp124.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp11B)->f1).tunion_info).UnknownTunion).tag != 0)goto _LLA2;_tmp126=(_tmp125.UnknownTunion).f1;
_tmp127=_tmp126.name;_tmp128=_tmp126.is_xtunion;_tmp129=_tmp126.is_flat;_tmp12A=
_tmp124.targs;_LLA1: {struct Cyc_List_List*_tmp13A=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp12A);
struct Cyc_Absyn_Decl*_tmp13B=Cyc_Absyn_tunion_decl(s,_tmp127,_tmp13A,0,_tmp128,
_tmp129,loc);if(_tmp105 != 0){const char*_tmp9CE;Cyc_Parse_err(((_tmp9CE="bad attributes on tunion",
_tag_dynforward(_tmp9CE,sizeof(char),_get_zero_arr_size_char(_tmp9CE,25)))),loc);}{
struct Cyc_List_List*_tmp9CF;struct Cyc_List_List*_tmp13E=(_tmp9CF=_cycalloc(
sizeof(*_tmp9CF)),((_tmp9CF->hd=_tmp13B,((_tmp9CF->tl=0,_tmp9CF)))));
_npop_handler(0);return _tmp13E;}}_LLA2: if(*((int*)_tmp11B)!= 12)goto _LLA4;
_tmp12B=((struct Cyc_Absyn_EnumType_struct*)_tmp11B)->f1;_LLA3: {struct Cyc_Absyn_Enumdecl*
_tmp9D0;struct Cyc_Absyn_Enumdecl*_tmp13F=(_tmp9D0=_cycalloc(sizeof(*_tmp9D0)),((
_tmp9D0->sc=(void*)s,((_tmp9D0->name=_tmp12B,((_tmp9D0->fields=0,_tmp9D0)))))));
if(_tmp105 != 0){const char*_tmp9D1;Cyc_Parse_err(((_tmp9D1="bad attributes on enum",
_tag_dynforward(_tmp9D1,sizeof(char),_get_zero_arr_size_char(_tmp9D1,23)))),loc);}{
struct Cyc_Absyn_Decl*_tmp9DB;struct Cyc_Absyn_Enum_d_struct _tmp9DA;struct Cyc_Absyn_Enum_d_struct*
_tmp9D9;struct Cyc_List_List*_tmp9D8;struct Cyc_List_List*_tmp145=(_tmp9D8=
_cycalloc(sizeof(*_tmp9D8)),((_tmp9D8->hd=((_tmp9DB=_cycalloc(sizeof(*_tmp9DB)),((
_tmp9DB->r=(void*)((void*)((_tmp9D9=_cycalloc(sizeof(*_tmp9D9)),((_tmp9D9[0]=((
_tmp9DA.tag=6,((_tmp9DA.f1=_tmp13F,_tmp9DA)))),_tmp9D9))))),((_tmp9DB->loc=loc,
_tmp9DB)))))),((_tmp9D8->tl=0,_tmp9D8)))));_npop_handler(0);return _tmp145;}}
_LLA4: if(*((int*)_tmp11B)!= 13)goto _LLA6;_tmp12C=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp11B)->f1;_LLA5: {struct Cyc_Core_Opt*_tmp9DE;struct Cyc_Absyn_Enumdecl*_tmp9DD;
struct Cyc_Absyn_Enumdecl*_tmp147=(_tmp9DD=_cycalloc(sizeof(*_tmp9DD)),((_tmp9DD->sc=(
void*)s,((_tmp9DD->name=Cyc_Parse_gensym_enum(),((_tmp9DD->fields=((_tmp9DE=
_cycalloc(sizeof(*_tmp9DE)),((_tmp9DE->v=_tmp12C,_tmp9DE)))),_tmp9DD)))))));if(
_tmp105 != 0){const char*_tmp9DF;Cyc_Parse_err(((_tmp9DF="bad attributes on enum",
_tag_dynforward(_tmp9DF,sizeof(char),_get_zero_arr_size_char(_tmp9DF,23)))),loc);}{
struct Cyc_Absyn_Decl*_tmp9E9;struct Cyc_Absyn_Enum_d_struct _tmp9E8;struct Cyc_Absyn_Enum_d_struct*
_tmp9E7;struct Cyc_List_List*_tmp9E6;struct Cyc_List_List*_tmp14D=(_tmp9E6=
_cycalloc(sizeof(*_tmp9E6)),((_tmp9E6->hd=((_tmp9E9=_cycalloc(sizeof(*_tmp9E9)),((
_tmp9E9->r=(void*)((void*)((_tmp9E7=_cycalloc(sizeof(*_tmp9E7)),((_tmp9E7[0]=((
_tmp9E8.tag=6,((_tmp9E8.f1=_tmp147,_tmp9E8)))),_tmp9E7))))),((_tmp9E9->loc=loc,
_tmp9E9)))))),((_tmp9E6->tl=0,_tmp9E6)))));_npop_handler(0);return _tmp14D;}}
_LLA6:;_LLA7:{const char*_tmp9EA;Cyc_Parse_err(((_tmp9EA="missing declarator",
_tag_dynforward(_tmp9EA,sizeof(char),_get_zero_arr_size_char(_tmp9EA,19)))),loc);}{
struct Cyc_List_List*_tmp151=0;_npop_handler(0);return _tmp151;}_LL9B:;}}else{void*
t=_tmp10D.f1;struct Cyc_List_List*_tmp152=Cyc_Parse_apply_tmss(mkrgn,_tmp103,t,
_tmp10B,_tmp105);if(istypedef){if(!exps_empty){const char*_tmp9EB;Cyc_Parse_err(((
_tmp9EB="initializer in typedef declaration",_tag_dynforward(_tmp9EB,sizeof(char),
_get_zero_arr_size_char(_tmp9EB,35)))),loc);}{struct Cyc_List_List*decls=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(struct Cyc_Position_Segment*,struct
_tuple7*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_v_typ_to_typedef,loc,_tmp152);if(_tmp10D.f2 != 0){struct Cyc_Absyn_Decl*
d=(struct Cyc_Absyn_Decl*)(_tmp10D.f2)->v;{void*_tmp154=(void*)d->r;struct Cyc_Absyn_Aggrdecl*
_tmp155;struct Cyc_Absyn_Tuniondecl*_tmp156;struct Cyc_Absyn_Enumdecl*_tmp157;
_LLA9: if(_tmp154 <= (void*)2)goto _LLAF;if(*((int*)_tmp154)!= 4)goto _LLAB;_tmp155=((
struct Cyc_Absyn_Aggr_d_struct*)_tmp154)->f1;_LLAA:(void*)(_tmp155->sc=(void*)s);
_tmp155->attributes=_tmp105;_tmp105=0;goto _LLA8;_LLAB: if(*((int*)_tmp154)!= 5)
goto _LLAD;_tmp156=((struct Cyc_Absyn_Tunion_d_struct*)_tmp154)->f1;_LLAC:(void*)(
_tmp156->sc=(void*)s);goto _LLA8;_LLAD: if(*((int*)_tmp154)!= 6)goto _LLAF;_tmp157=((
struct Cyc_Absyn_Enum_d_struct*)_tmp154)->f1;_LLAE:(void*)(_tmp157->sc=(void*)s);
goto _LLA8;_LLAF:;_LLB0:{const char*_tmp9EC;Cyc_Parse_err(((_tmp9EC="declaration within typedef is not a struct, union, tunion, or xtunion",
_tag_dynforward(_tmp9EC,sizeof(char),_get_zero_arr_size_char(_tmp9EC,70)))),loc);}
goto _LLA8;_LLA8:;}{struct Cyc_List_List*_tmp9ED;decls=((_tmp9ED=_cycalloc(sizeof(*
_tmp9ED)),((_tmp9ED->hd=d,((_tmp9ED->tl=decls,_tmp9ED))))));}}{struct Cyc_List_List*
_tmp15A=decls;_npop_handler(0);return _tmp15A;}}}else{if(_tmp10D.f2 != 0){const
char*_tmp9EE;Cyc_Parse_unimp(((_tmp9EE="nested type declaration within declarator",
_tag_dynforward(_tmp9EE,sizeof(char),_get_zero_arr_size_char(_tmp9EE,42)))),loc);}{
struct Cyc_List_List*decls=0;{struct Cyc_List_List*_tmp15C=_tmp152;for(0;_tmp15C != 
0;(_tmp15C=_tmp15C->tl,_tmp10C=_tmp10C->tl)){struct _tuple7 _tmp15E;struct _tuple1*
_tmp15F;struct Cyc_Absyn_Tqual _tmp160;void*_tmp161;struct Cyc_List_List*_tmp162;
struct Cyc_List_List*_tmp163;struct _tuple7*_tmp15D=(struct _tuple7*)_tmp15C->hd;
_tmp15E=*_tmp15D;_tmp15F=_tmp15E.f1;_tmp160=_tmp15E.f2;_tmp161=_tmp15E.f3;
_tmp162=_tmp15E.f4;_tmp163=_tmp15E.f5;if(_tmp162 != 0){const char*_tmp9F1;void*
_tmp9F0;(_tmp9F0=0,Cyc_Tcutil_warn(loc,((_tmp9F1="bad type params, ignoring",
_tag_dynforward(_tmp9F1,sizeof(char),_get_zero_arr_size_char(_tmp9F1,26)))),
_tag_dynforward(_tmp9F0,sizeof(void*),0)));}if(_tmp10C == 0){const char*_tmp9F4;
void*_tmp9F3;(_tmp9F3=0,((int(*)(struct Cyc_Position_Segment*sg,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(loc,((_tmp9F4="unexpected NULL in parse!",
_tag_dynforward(_tmp9F4,sizeof(char),_get_zero_arr_size_char(_tmp9F4,26)))),
_tag_dynforward(_tmp9F3,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*_tmp168=(struct
Cyc_Absyn_Exp*)_tmp10C->hd;struct Cyc_Absyn_Vardecl*_tmp169=Cyc_Absyn_new_vardecl(
_tmp15F,_tmp161,_tmp168);_tmp169->tq=_tmp160;(void*)(_tmp169->sc=(void*)s);
_tmp169->attributes=_tmp163;{struct Cyc_Absyn_Var_d_struct*_tmp9FA;struct Cyc_Absyn_Var_d_struct
_tmp9F9;struct Cyc_Absyn_Decl*_tmp9F8;struct Cyc_Absyn_Decl*_tmp16A=(_tmp9F8=
_cycalloc(sizeof(*_tmp9F8)),((_tmp9F8->r=(void*)((void*)((_tmp9FA=_cycalloc(
sizeof(*_tmp9FA)),((_tmp9FA[0]=((_tmp9F9.tag=0,((_tmp9F9.f1=_tmp169,_tmp9F9)))),
_tmp9FA))))),((_tmp9F8->loc=loc,_tmp9F8)))));struct Cyc_List_List*_tmp9FB;decls=((
_tmp9FB=_cycalloc(sizeof(*_tmp9FB)),((_tmp9FB->hd=_tmp16A,((_tmp9FB->tl=decls,
_tmp9FB))))));}}}}{struct Cyc_List_List*_tmp16F=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0);return _tmp16F;}}}}}}}}};
_pop_region(mkrgn);}static void*Cyc_Parse_id_to_kind(struct _dynforward_ptr s,
struct Cyc_Position_Segment*loc);static void*Cyc_Parse_id_to_kind(struct
_dynforward_ptr s,struct Cyc_Position_Segment*loc){if(Cyc_strlen((struct
_dynforward_ptr)s)== 1  || Cyc_strlen((struct _dynforward_ptr)s)== 2)switch(*((
const char*)_check_dynforward_subscript(s,sizeof(char),0))){case 'A': _LLB1: return(
void*)0;case 'M': _LLB2: return(void*)1;case 'B': _LLB3: return(void*)2;case 'R': _LLB4:
return(void*)3;case 'U': _LLB5: if(*((const char*)_check_dynforward_subscript(s,
sizeof(char),1))== 'R')return(void*)4;else{break;}case 'T': _LLB6: if(*((const char*)
_check_dynforward_subscript(s,sizeof(char),1))== 'R')return(void*)5;else{break;}
case 'E': _LLB7: return(void*)6;case 'I': _LLB8: return(void*)7;default: _LLB9: break;}{
const char*_tmpA00;void*_tmp9FF[2];struct Cyc_String_pa_struct _tmp9FE;struct Cyc_Int_pa_struct
_tmp9FD;Cyc_Parse_err((struct _dynforward_ptr)((_tmp9FD.tag=1,((_tmp9FD.f1=(
unsigned long)((int)Cyc_strlen((struct _dynforward_ptr)s)),((_tmp9FE.tag=0,((
_tmp9FE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)s),((_tmp9FF[0]=&
_tmp9FE,((_tmp9FF[1]=& _tmp9FD,Cyc_aprintf(((_tmpA00="bad kind: %s; strlen=%d",
_tag_dynforward(_tmpA00,sizeof(char),_get_zero_arr_size_char(_tmpA00,24)))),
_tag_dynforward(_tmp9FF,sizeof(void*),2)))))))))))))),loc);}return(void*)2;}
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p);static struct
Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){void*_tmp174=(void*)p->r;
struct _tuple1*_tmp175;struct Cyc_Absyn_Vardecl*_tmp176;struct Cyc_Absyn_Pat*
_tmp177;struct Cyc_Absyn_Pat _tmp178;void*_tmp179;struct Cyc_Absyn_Pat*_tmp17A;void*
_tmp17B;int _tmp17C;char _tmp17D;struct _dynforward_ptr _tmp17E;struct _tuple1*
_tmp17F;struct Cyc_List_List*_tmp180;int _tmp181;struct Cyc_Absyn_Exp*_tmp182;_LLBC:
if(_tmp174 <= (void*)2)goto _LLC2;if(*((int*)_tmp174)!= 12)goto _LLBE;_tmp175=((
struct Cyc_Absyn_UnknownId_p_struct*)_tmp174)->f1;_LLBD: {struct Cyc_Absyn_UnknownId_e_struct
_tmpA03;struct Cyc_Absyn_UnknownId_e_struct*_tmpA02;return Cyc_Absyn_new_exp((void*)((
_tmpA02=_cycalloc(sizeof(*_tmpA02)),((_tmpA02[0]=((_tmpA03.tag=2,((_tmpA03.f1=
_tmp175,_tmpA03)))),_tmpA02)))),p->loc);}_LLBE: if(*((int*)_tmp174)!= 1)goto _LLC0;
_tmp176=((struct Cyc_Absyn_Reference_p_struct*)_tmp174)->f1;_tmp177=((struct Cyc_Absyn_Reference_p_struct*)
_tmp174)->f2;_tmp178=*_tmp177;_tmp179=(void*)_tmp178.r;if((int)_tmp179 != 0)goto
_LLC0;_LLBF: {struct Cyc_Absyn_UnknownId_e_struct _tmpA06;struct Cyc_Absyn_UnknownId_e_struct*
_tmpA05;return Cyc_Absyn_deref_exp(Cyc_Absyn_new_exp((void*)((_tmpA05=_cycalloc(
sizeof(*_tmpA05)),((_tmpA05[0]=((_tmpA06.tag=2,((_tmpA06.f1=_tmp176->name,
_tmpA06)))),_tmpA05)))),p->loc),p->loc);}_LLC0: if(*((int*)_tmp174)!= 4)goto _LLC2;
_tmp17A=((struct Cyc_Absyn_Pointer_p_struct*)_tmp174)->f1;_LLC1: return Cyc_Absyn_address_exp(
Cyc_Parse_pat2exp(_tmp17A),p->loc);_LLC2: if((int)_tmp174 != 1)goto _LLC4;_LLC3:
return Cyc_Absyn_null_exp(p->loc);_LLC4: if(_tmp174 <= (void*)2)goto _LLCE;if(*((int*)
_tmp174)!= 7)goto _LLC6;_tmp17B=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp174)->f1;
_tmp17C=((struct Cyc_Absyn_Int_p_struct*)_tmp174)->f2;_LLC5: return Cyc_Absyn_int_exp(
_tmp17B,_tmp17C,p->loc);_LLC6: if(*((int*)_tmp174)!= 8)goto _LLC8;_tmp17D=((struct
Cyc_Absyn_Char_p_struct*)_tmp174)->f1;_LLC7: return Cyc_Absyn_char_exp(_tmp17D,p->loc);
_LLC8: if(*((int*)_tmp174)!= 9)goto _LLCA;_tmp17E=((struct Cyc_Absyn_Float_p_struct*)
_tmp174)->f1;_LLC9: return Cyc_Absyn_float_exp(_tmp17E,p->loc);_LLCA: if(*((int*)
_tmp174)!= 13)goto _LLCC;_tmp17F=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp174)->f1;
_tmp180=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp174)->f2;_tmp181=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp174)->f3;if(_tmp181 != 0)goto _LLCC;_LLCB: {struct Cyc_Absyn_UnknownId_e_struct
_tmpA09;struct Cyc_Absyn_UnknownId_e_struct*_tmpA08;struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((
void*)((_tmpA08=_cycalloc(sizeof(*_tmpA08)),((_tmpA08[0]=((_tmpA09.tag=2,((
_tmpA09.f1=_tmp17F,_tmpA09)))),_tmpA08)))),p->loc);struct Cyc_List_List*es=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp180);return Cyc_Absyn_unknowncall_exp(e1,
es,p->loc);}_LLCC: if(*((int*)_tmp174)!= 14)goto _LLCE;_tmp182=((struct Cyc_Absyn_Exp_p_struct*)
_tmp174)->f1;_LLCD: return _tmp182;_LLCE:;_LLCF:{const char*_tmpA0A;Cyc_Parse_err(((
_tmpA0A="cannot mix patterns and expressions in case",_tag_dynforward(_tmpA0A,
sizeof(char),_get_zero_arr_size_char(_tmpA0A,44)))),p->loc);}return Cyc_Absyn_null_exp(
p->loc);_LLBB:;}struct _tuple11{void*f1;int f2;};struct Cyc_Int_tok_struct{int tag;
struct _tuple11 f1;};struct Cyc_Char_tok_struct{int tag;char f1;};struct Cyc_String_tok_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Stringopt_tok_struct{int tag;struct Cyc_Core_Opt*
f1;};struct Cyc_QualId_tok_struct{int tag;struct _tuple1*f1;};struct _tuple12{struct
Cyc_Position_Segment*f1;struct Cyc_Absyn_Conref*f2;struct Cyc_Absyn_Conref*f3;};
struct Cyc_YY1_struct{int tag;struct _tuple12*f1;};struct Cyc_YY2_struct{int tag;
struct Cyc_Absyn_Conref*f1;};struct Cyc_YY3_struct{int tag;struct Cyc_Absyn_Exp*f1;}
;struct Cyc_YY4_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_YY5_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY6_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY7_struct{int tag;void*f1;};struct Cyc_YY8_struct{int tag;struct Cyc_Core_Opt*
f1;};struct Cyc_YY9_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_YY10_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY11_struct{int tag;struct Cyc_Absyn_Pat*
f1;};struct _tuple13{struct Cyc_List_List*f1;int f2;};struct Cyc_YY12_struct{int tag;
struct _tuple13*f1;};struct Cyc_YY13_struct{int tag;struct Cyc_List_List*f1;};struct
_tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_YY14_struct{
int tag;struct _tuple14*f1;};struct Cyc_YY15_struct{int tag;struct Cyc_List_List*f1;}
;struct Cyc_YY16_struct{int tag;struct _tuple13*f1;};struct Cyc_YY17_struct{int tag;
struct Cyc_Absyn_Fndecl*f1;};struct Cyc_YY18_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY19_struct{int tag;struct Cyc_Parse_Declaration_spec*f1;};struct
_tuple15{struct Cyc_Parse_Declarator*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_YY20_struct{
int tag;struct _tuple15*f1;};struct Cyc_YY21_struct{int tag;struct Cyc_List_List*f1;}
;struct Cyc_YY22_struct{int tag;void*f1;};struct Cyc_YY23_struct{int tag;void*f1;};
struct Cyc_YY24_struct{int tag;void*f1;};struct Cyc_YY25_struct{int tag;struct Cyc_Absyn_Tqual
f1;};struct Cyc_YY26_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY27_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY28_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_YY29_struct{int tag;struct Cyc_Parse_Declarator*f1;};struct Cyc_YY30_struct{
int tag;struct Cyc_Parse_Abstractdeclarator*f1;};struct Cyc_YY31_struct{int tag;int
f1;};struct Cyc_YY32_struct{int tag;void*f1;};struct Cyc_YY33_struct{int tag;struct
Cyc_Absyn_Tunionfield*f1;};struct Cyc_YY34_struct{int tag;struct Cyc_List_List*f1;}
;struct _tuple16{struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;};struct Cyc_YY35_struct{int tag;struct _tuple16*f1;};struct Cyc_YY36_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY37_struct{int tag;struct _tuple2*f1;};
struct Cyc_YY38_struct{int tag;struct Cyc_List_List*f1;};struct _tuple17{struct Cyc_List_List*
f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*
f5;};struct Cyc_YY39_struct{int tag;struct _tuple17*f1;};struct Cyc_YY40_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY41_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY42_struct{int tag;void*f1;};struct Cyc_YY43_struct{int tag;void*f1;};
struct Cyc_YY44_struct{int tag;void*f1;};struct Cyc_YY45_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_YY46_struct{int tag;void*f1;};struct Cyc_YY47_struct{int tag;struct
Cyc_Absyn_Enumfield*f1;};struct Cyc_YY48_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY49_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_YY50_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY51_struct{int tag;struct Cyc_Absyn_Conref*
f1;};struct Cyc_YY52_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YYINITIALSVAL_struct{
int tag;};union Cyc_YYSTYPE_union{struct Cyc_Int_tok_struct Int_tok;struct Cyc_Char_tok_struct
Char_tok;struct Cyc_String_tok_struct String_tok;struct Cyc_Stringopt_tok_struct
Stringopt_tok;struct Cyc_QualId_tok_struct QualId_tok;struct Cyc_YY1_struct YY1;
struct Cyc_YY2_struct YY2;struct Cyc_YY3_struct YY3;struct Cyc_YY4_struct YY4;struct
Cyc_YY5_struct YY5;struct Cyc_YY6_struct YY6;struct Cyc_YY7_struct YY7;struct Cyc_YY8_struct
YY8;struct Cyc_YY9_struct YY9;struct Cyc_YY10_struct YY10;struct Cyc_YY11_struct YY11;
struct Cyc_YY12_struct YY12;struct Cyc_YY13_struct YY13;struct Cyc_YY14_struct YY14;
struct Cyc_YY15_struct YY15;struct Cyc_YY16_struct YY16;struct Cyc_YY17_struct YY17;
struct Cyc_YY18_struct YY18;struct Cyc_YY19_struct YY19;struct Cyc_YY20_struct YY20;
struct Cyc_YY21_struct YY21;struct Cyc_YY22_struct YY22;struct Cyc_YY23_struct YY23;
struct Cyc_YY24_struct YY24;struct Cyc_YY25_struct YY25;struct Cyc_YY26_struct YY26;
struct Cyc_YY27_struct YY27;struct Cyc_YY28_struct YY28;struct Cyc_YY29_struct YY29;
struct Cyc_YY30_struct YY30;struct Cyc_YY31_struct YY31;struct Cyc_YY32_struct YY32;
struct Cyc_YY33_struct YY33;struct Cyc_YY34_struct YY34;struct Cyc_YY35_struct YY35;
struct Cyc_YY36_struct YY36;struct Cyc_YY37_struct YY37;struct Cyc_YY38_struct YY38;
struct Cyc_YY39_struct YY39;struct Cyc_YY40_struct YY40;struct Cyc_YY41_struct YY41;
struct Cyc_YY42_struct YY42;struct Cyc_YY43_struct YY43;struct Cyc_YY44_struct YY44;
struct Cyc_YY45_struct YY45;struct Cyc_YY46_struct YY46;struct Cyc_YY47_struct YY47;
struct Cyc_YY48_struct YY48;struct Cyc_YY49_struct YY49;struct Cyc_YY50_struct YY50;
struct Cyc_YY51_struct YY51;struct Cyc_YY52_struct YY52;struct Cyc_YYINITIALSVAL_struct
YYINITIALSVAL;};static char _tmp18B[14]="$(sign_t,int)";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp18B,_tmp18B + 14}};struct _tuple11 Cyc_yyget_Int_tok(
union Cyc_YYSTYPE_union yy1);struct _tuple11 Cyc_yyget_Int_tok(union Cyc_YYSTYPE_union
yy1){struct _tuple11 yyzzz;{union Cyc_YYSTYPE_union _tmp18C=yy1;struct _tuple11
_tmp18D;_LLD1: if((_tmp18C.Int_tok).tag != 0)goto _LLD3;_tmp18D=(_tmp18C.Int_tok).f1;
_LLD2: yyzzz=_tmp18D;goto _LLD0;_LLD3:;_LLD4:(int)_throw((void*)& Cyc_yyfail_Int_tok);
_LLD0:;}return yyzzz;}static char _tmp18F[5]="char";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp18F,_tmp18F + 5}};char Cyc_yyget_Char_tok(
union Cyc_YYSTYPE_union yy1);char Cyc_yyget_Char_tok(union Cyc_YYSTYPE_union yy1){
char yyzzz;{union Cyc_YYSTYPE_union _tmp190=yy1;char _tmp191;_LLD6: if((_tmp190.Char_tok).tag
!= 1)goto _LLD8;_tmp191=(_tmp190.Char_tok).f1;_LLD7: yyzzz=_tmp191;goto _LLD5;_LLD8:;
_LLD9:(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLD5:;}return yyzzz;}static char
_tmp193[9]="string_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={
Cyc_Core_Failure,{_tmp193,_tmp193 + 9}};struct _dynforward_ptr Cyc_yyget_String_tok(
union Cyc_YYSTYPE_union yy1);struct _dynforward_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE_union
yy1){struct _dynforward_ptr yyzzz;{union Cyc_YYSTYPE_union _tmp194=yy1;struct
_dynforward_ptr _tmp195;_LLDB: if((_tmp194.String_tok).tag != 2)goto _LLDD;_tmp195=(
_tmp194.String_tok).f1;_LLDC: yyzzz=_tmp195;goto _LLDA;_LLDD:;_LLDE:(int)_throw((
void*)& Cyc_yyfail_String_tok);_LLDA:;}return yyzzz;}static char _tmp197[54]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp197,
_tmp197 + 54}};struct _tuple12*Cyc_yyget_YY1(union Cyc_YYSTYPE_union yy1);struct
_tuple12*Cyc_yyget_YY1(union Cyc_YYSTYPE_union yy1){struct _tuple12*yyzzz;{union Cyc_YYSTYPE_union
_tmp198=yy1;struct _tuple12*_tmp199;_LLE0: if((_tmp198.YY1).tag != 5)goto _LLE2;
_tmp199=(_tmp198.YY1).f1;_LLE1: yyzzz=_tmp199;goto _LLDF;_LLE2:;_LLE3:(int)_throw((
void*)& Cyc_yyfail_YY1);_LLDF:;}return yyzzz;}static char _tmp19B[19]="conref_t<bounds_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp19B,
_tmp19B + 19}};struct Cyc_Absyn_Conref*Cyc_yyget_YY2(union Cyc_YYSTYPE_union yy1);
struct Cyc_Absyn_Conref*Cyc_yyget_YY2(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Conref*
yyzzz;{union Cyc_YYSTYPE_union _tmp19C=yy1;struct Cyc_Absyn_Conref*_tmp19D;_LLE5:
if((_tmp19C.YY2).tag != 6)goto _LLE7;_tmp19D=(_tmp19C.YY2).f1;_LLE6: yyzzz=_tmp19D;
goto _LLE4;_LLE7:;_LLE8:(int)_throw((void*)& Cyc_yyfail_YY2);_LLE4:;}return yyzzz;}
static char _tmp19F[6]="exp_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY3={
Cyc_Core_Failure,{_tmp19F,_tmp19F + 6}};struct Cyc_Absyn_Exp*Cyc_yyget_YY3(union
Cyc_YYSTYPE_union yy1);struct Cyc_Absyn_Exp*Cyc_yyget_YY3(union Cyc_YYSTYPE_union
yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE_union _tmp1A0=yy1;struct Cyc_Absyn_Exp*
_tmp1A1;_LLEA: if((_tmp1A0.YY3).tag != 7)goto _LLEC;_tmp1A1=(_tmp1A0.YY3).f1;_LLEB:
yyzzz=_tmp1A1;goto _LLE9;_LLEC:;_LLED:(int)_throw((void*)& Cyc_yyfail_YY3);_LLE9:;}
return yyzzz;}static char _tmp1A3[10]="exp_opt_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1A3,_tmp1A3 + 10}};struct Cyc_Absyn_Exp*Cyc_yyget_YY4(
union Cyc_YYSTYPE_union yy1);struct Cyc_Absyn_Exp*Cyc_yyget_YY4(union Cyc_YYSTYPE_union
yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE_union _tmp1A4=yy1;struct Cyc_Absyn_Exp*
_tmp1A5;_LLEF: if((_tmp1A4.YY4).tag != 8)goto _LLF1;_tmp1A5=(_tmp1A4.YY4).f1;_LLF0:
yyzzz=_tmp1A5;goto _LLEE;_LLF1:;_LLF2:(int)_throw((void*)& Cyc_yyfail_YY4);_LLEE:;}
return yyzzz;}static char _tmp1A7[14]="list_t<exp_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp1A7,_tmp1A7 + 14}};struct Cyc_List_List*Cyc_yyget_YY5(
union Cyc_YYSTYPE_union yy1);struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE_union
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1A8=yy1;struct Cyc_List_List*
_tmp1A9;_LLF4: if((_tmp1A8.YY5).tag != 9)goto _LLF6;_tmp1A9=(_tmp1A8.YY5).f1;_LLF5:
yyzzz=_tmp1A9;goto _LLF3;_LLF6:;_LLF7:(int)_throw((void*)& Cyc_yyfail_YY5);_LLF3:;}
return yyzzz;}static char _tmp1AB[39]="list_t<$(list_t<designator_t>,exp_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp1AB,
_tmp1AB + 39}};struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE_union yy1);
struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp1AC=yy1;struct Cyc_List_List*_tmp1AD;_LLF9: if((
_tmp1AC.YY6).tag != 10)goto _LLFB;_tmp1AD=(_tmp1AC.YY6).f1;_LLFA: yyzzz=_tmp1AD;
goto _LLF8;_LLFB:;_LLFC:(int)_throw((void*)& Cyc_yyfail_YY6);_LLF8:;}return yyzzz;}
static char _tmp1AF[9]="primop_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY7={
Cyc_Core_Failure,{_tmp1AF,_tmp1AF + 9}};void*Cyc_yyget_YY7(union Cyc_YYSTYPE_union
yy1);void*Cyc_yyget_YY7(union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union
_tmp1B0=yy1;void*_tmp1B1;_LLFE: if((_tmp1B0.YY7).tag != 11)goto _LL100;_tmp1B1=(
_tmp1B0.YY7).f1;_LLFF: yyzzz=_tmp1B1;goto _LLFD;_LL100:;_LL101:(int)_throw((void*)&
Cyc_yyfail_YY7);_LLFD:;}return yyzzz;}static char _tmp1B3[16]="opt_t<primop_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp1B3,
_tmp1B3 + 16}};struct Cyc_Core_Opt*Cyc_yyget_YY8(union Cyc_YYSTYPE_union yy1);struct
Cyc_Core_Opt*Cyc_yyget_YY8(union Cyc_YYSTYPE_union yy1){struct Cyc_Core_Opt*yyzzz;{
union Cyc_YYSTYPE_union _tmp1B4=yy1;struct Cyc_Core_Opt*_tmp1B5;_LL103: if((_tmp1B4.YY8).tag
!= 12)goto _LL105;_tmp1B5=(_tmp1B4.YY8).f1;_LL104: yyzzz=_tmp1B5;goto _LL102;_LL105:;
_LL106:(int)_throw((void*)& Cyc_yyfail_YY8);_LL102:;}return yyzzz;}static char
_tmp1B7[7]="qvar_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={
Cyc_Core_Failure,{_tmp1B7,_tmp1B7 + 7}};struct _tuple1*Cyc_yyget_QualId_tok(union
Cyc_YYSTYPE_union yy1);struct _tuple1*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE_union
yy1){struct _tuple1*yyzzz;{union Cyc_YYSTYPE_union _tmp1B8=yy1;struct _tuple1*
_tmp1B9;_LL108: if((_tmp1B8.QualId_tok).tag != 4)goto _LL10A;_tmp1B9=(_tmp1B8.QualId_tok).f1;
_LL109: yyzzz=_tmp1B9;goto _LL107;_LL10A:;_LL10B:(int)_throw((void*)& Cyc_yyfail_QualId_tok);
_LL107:;}return yyzzz;}static char _tmp1BB[7]="stmt_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp1BB,_tmp1BB + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_YY9(
union Cyc_YYSTYPE_union yy1);struct Cyc_Absyn_Stmt*Cyc_yyget_YY9(union Cyc_YYSTYPE_union
yy1){struct Cyc_Absyn_Stmt*yyzzz;{union Cyc_YYSTYPE_union _tmp1BC=yy1;struct Cyc_Absyn_Stmt*
_tmp1BD;_LL10D: if((_tmp1BC.YY9).tag != 13)goto _LL10F;_tmp1BD=(_tmp1BC.YY9).f1;
_LL10E: yyzzz=_tmp1BD;goto _LL10C;_LL10F:;_LL110:(int)_throw((void*)& Cyc_yyfail_YY9);
_LL10C:;}return yyzzz;}static char _tmp1BF[24]="list_t<switch_clause_t>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp1BF,_tmp1BF + 
24}};struct Cyc_List_List*Cyc_yyget_YY10(union Cyc_YYSTYPE_union yy1);struct Cyc_List_List*
Cyc_yyget_YY10(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp1C0=yy1;struct Cyc_List_List*_tmp1C1;_LL112: if((_tmp1C0.YY10).tag != 14)goto
_LL114;_tmp1C1=(_tmp1C0.YY10).f1;_LL113: yyzzz=_tmp1C1;goto _LL111;_LL114:;_LL115:(
int)_throw((void*)& Cyc_yyfail_YY10);_LL111:;}return yyzzz;}static char _tmp1C3[6]="pat_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp1C3,
_tmp1C3 + 6}};struct Cyc_Absyn_Pat*Cyc_yyget_YY11(union Cyc_YYSTYPE_union yy1);
struct Cyc_Absyn_Pat*Cyc_yyget_YY11(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Pat*
yyzzz;{union Cyc_YYSTYPE_union _tmp1C4=yy1;struct Cyc_Absyn_Pat*_tmp1C5;_LL117: if((
_tmp1C4.YY11).tag != 15)goto _LL119;_tmp1C5=(_tmp1C4.YY11).f1;_LL118: yyzzz=_tmp1C5;
goto _LL116;_LL119:;_LL11A:(int)_throw((void*)& Cyc_yyfail_YY11);_LL116:;}return
yyzzz;}static char _tmp1C7[23]="$(list_t<pat_t>,bool)@";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp1C7,_tmp1C7 + 23}};struct _tuple13*Cyc_yyget_YY12(
union Cyc_YYSTYPE_union yy1);struct _tuple13*Cyc_yyget_YY12(union Cyc_YYSTYPE_union
yy1){struct _tuple13*yyzzz;{union Cyc_YYSTYPE_union _tmp1C8=yy1;struct _tuple13*
_tmp1C9;_LL11C: if((_tmp1C8.YY12).tag != 16)goto _LL11E;_tmp1C9=(_tmp1C8.YY12).f1;
_LL11D: yyzzz=_tmp1C9;goto _LL11B;_LL11E:;_LL11F:(int)_throw((void*)& Cyc_yyfail_YY12);
_LL11B:;}return yyzzz;}static char _tmp1CB[14]="list_t<pat_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp1CB,_tmp1CB + 14}};struct Cyc_List_List*Cyc_yyget_YY13(
union Cyc_YYSTYPE_union yy1);struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE_union
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1CC=yy1;struct Cyc_List_List*
_tmp1CD;_LL121: if((_tmp1CC.YY13).tag != 17)goto _LL123;_tmp1CD=(_tmp1CC.YY13).f1;
_LL122: yyzzz=_tmp1CD;goto _LL120;_LL123:;_LL124:(int)_throw((void*)& Cyc_yyfail_YY13);
_LL120:;}return yyzzz;}static char _tmp1CF[31]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp1CF,
_tmp1CF + 31}};struct _tuple14*Cyc_yyget_YY14(union Cyc_YYSTYPE_union yy1);struct
_tuple14*Cyc_yyget_YY14(union Cyc_YYSTYPE_union yy1){struct _tuple14*yyzzz;{union
Cyc_YYSTYPE_union _tmp1D0=yy1;struct _tuple14*_tmp1D1;_LL126: if((_tmp1D0.YY14).tag
!= 18)goto _LL128;_tmp1D1=(_tmp1D0.YY14).f1;_LL127: yyzzz=_tmp1D1;goto _LL125;
_LL128:;_LL129:(int)_throw((void*)& Cyc_yyfail_YY14);_LL125:;}return yyzzz;}static
char _tmp1D3[39]="list_t<$(list_t<designator_t>,pat_t)@>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp1D3,_tmp1D3 + 39}};struct Cyc_List_List*Cyc_yyget_YY15(
union Cyc_YYSTYPE_union yy1);struct Cyc_List_List*Cyc_yyget_YY15(union Cyc_YYSTYPE_union
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1D4=yy1;struct Cyc_List_List*
_tmp1D5;_LL12B: if((_tmp1D4.YY15).tag != 19)goto _LL12D;_tmp1D5=(_tmp1D4.YY15).f1;
_LL12C: yyzzz=_tmp1D5;goto _LL12A;_LL12D:;_LL12E:(int)_throw((void*)& Cyc_yyfail_YY15);
_LL12A:;}return yyzzz;}static char _tmp1D7[48]="$(list_t<$(list_t<designator_t>,pat_t)@>,bool)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp1D7,
_tmp1D7 + 48}};struct _tuple13*Cyc_yyget_YY16(union Cyc_YYSTYPE_union yy1);struct
_tuple13*Cyc_yyget_YY16(union Cyc_YYSTYPE_union yy1){struct _tuple13*yyzzz;{union
Cyc_YYSTYPE_union _tmp1D8=yy1;struct _tuple13*_tmp1D9;_LL130: if((_tmp1D8.YY16).tag
!= 20)goto _LL132;_tmp1D9=(_tmp1D8.YY16).f1;_LL131: yyzzz=_tmp1D9;goto _LL12F;
_LL132:;_LL133:(int)_throw((void*)& Cyc_yyfail_YY16);_LL12F:;}return yyzzz;}static
char _tmp1DB[9]="fndecl_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY17={
Cyc_Core_Failure,{_tmp1DB,_tmp1DB + 9}};struct Cyc_Absyn_Fndecl*Cyc_yyget_YY17(
union Cyc_YYSTYPE_union yy1);struct Cyc_Absyn_Fndecl*Cyc_yyget_YY17(union Cyc_YYSTYPE_union
yy1){struct Cyc_Absyn_Fndecl*yyzzz;{union Cyc_YYSTYPE_union _tmp1DC=yy1;struct Cyc_Absyn_Fndecl*
_tmp1DD;_LL135: if((_tmp1DC.YY17).tag != 21)goto _LL137;_tmp1DD=(_tmp1DC.YY17).f1;
_LL136: yyzzz=_tmp1DD;goto _LL134;_LL137:;_LL138:(int)_throw((void*)& Cyc_yyfail_YY17);
_LL134:;}return yyzzz;}static char _tmp1DF[15]="list_t<decl_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp1DF,_tmp1DF + 15}};struct Cyc_List_List*Cyc_yyget_YY18(
union Cyc_YYSTYPE_union yy1);struct Cyc_List_List*Cyc_yyget_YY18(union Cyc_YYSTYPE_union
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1E0=yy1;struct Cyc_List_List*
_tmp1E1;_LL13A: if((_tmp1E0.YY18).tag != 22)goto _LL13C;_tmp1E1=(_tmp1E0.YY18).f1;
_LL13B: yyzzz=_tmp1E1;goto _LL139;_LL13C:;_LL13D:(int)_throw((void*)& Cyc_yyfail_YY18);
_LL139:;}return yyzzz;}static char _tmp1E3[12]="decl_spec_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp1E3,_tmp1E3 + 12}};struct Cyc_Parse_Declaration_spec*
Cyc_yyget_YY19(union Cyc_YYSTYPE_union yy1);struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY19(
union Cyc_YYSTYPE_union yy1){struct Cyc_Parse_Declaration_spec*yyzzz;{union Cyc_YYSTYPE_union
_tmp1E4=yy1;struct Cyc_Parse_Declaration_spec*_tmp1E5;_LL13F: if((_tmp1E4.YY19).tag
!= 23)goto _LL141;_tmp1E5=(_tmp1E4.YY19).f1;_LL140: yyzzz=_tmp1E5;goto _LL13E;
_LL141:;_LL142:(int)_throw((void*)& Cyc_yyfail_YY19);_LL13E:;}return yyzzz;}static
char _tmp1E7[27]="$(declarator_t,exp_opt_t)@";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp1E7,_tmp1E7 + 27}};struct _tuple15*Cyc_yyget_YY20(
union Cyc_YYSTYPE_union yy1);struct _tuple15*Cyc_yyget_YY20(union Cyc_YYSTYPE_union
yy1){struct _tuple15*yyzzz;{union Cyc_YYSTYPE_union _tmp1E8=yy1;struct _tuple15*
_tmp1E9;_LL144: if((_tmp1E8.YY20).tag != 24)goto _LL146;_tmp1E9=(_tmp1E8.YY20).f1;
_LL145: yyzzz=_tmp1E9;goto _LL143;_LL146:;_LL147:(int)_throw((void*)& Cyc_yyfail_YY20);
_LL143:;}return yyzzz;}static char _tmp1EB[35]="list_t<$(declarator_t,exp_opt_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp1EB,
_tmp1EB + 35}};struct Cyc_List_List*Cyc_yyget_YY21(union Cyc_YYSTYPE_union yy1);
struct Cyc_List_List*Cyc_yyget_YY21(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp1EC=yy1;struct Cyc_List_List*_tmp1ED;_LL149: if((
_tmp1EC.YY21).tag != 25)goto _LL14B;_tmp1ED=(_tmp1EC.YY21).f1;_LL14A: yyzzz=_tmp1ED;
goto _LL148;_LL14B:;_LL14C:(int)_throw((void*)& Cyc_yyfail_YY21);_LL148:;}return
yyzzz;}static char _tmp1EF[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp1EF,_tmp1EF + 16}};void*Cyc_yyget_YY22(
union Cyc_YYSTYPE_union yy1);void*Cyc_yyget_YY22(union Cyc_YYSTYPE_union yy1){void*
yyzzz;{union Cyc_YYSTYPE_union _tmp1F0=yy1;void*_tmp1F1;_LL14E: if((_tmp1F0.YY22).tag
!= 26)goto _LL150;_tmp1F1=(_tmp1F0.YY22).f1;_LL14F: yyzzz=_tmp1F1;goto _LL14D;
_LL150:;_LL151:(int)_throw((void*)& Cyc_yyfail_YY22);_LL14D:;}return yyzzz;}static
char _tmp1F3[17]="type_specifier_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY23={
Cyc_Core_Failure,{_tmp1F3,_tmp1F3 + 17}};void*Cyc_yyget_YY23(union Cyc_YYSTYPE_union
yy1);void*Cyc_yyget_YY23(union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union
_tmp1F4=yy1;void*_tmp1F5;_LL153: if((_tmp1F4.YY23).tag != 27)goto _LL155;_tmp1F5=(
_tmp1F4.YY23).f1;_LL154: yyzzz=_tmp1F5;goto _LL152;_LL155:;_LL156:(int)_throw((
void*)& Cyc_yyfail_YY23);_LL152:;}return yyzzz;}static char _tmp1F7[12]="aggr_kind_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp1F7,
_tmp1F7 + 12}};void*Cyc_yyget_YY24(union Cyc_YYSTYPE_union yy1);void*Cyc_yyget_YY24(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp1F8=yy1;void*
_tmp1F9;_LL158: if((_tmp1F8.YY24).tag != 28)goto _LL15A;_tmp1F9=(_tmp1F8.YY24).f1;
_LL159: yyzzz=_tmp1F9;goto _LL157;_LL15A:;_LL15B:(int)_throw((void*)& Cyc_yyfail_YY24);
_LL157:;}return yyzzz;}static char _tmp1FB[8]="tqual_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp1FB,_tmp1FB + 8}};struct Cyc_Absyn_Tqual Cyc_yyget_YY25(
union Cyc_YYSTYPE_union yy1);struct Cyc_Absyn_Tqual Cyc_yyget_YY25(union Cyc_YYSTYPE_union
yy1){struct Cyc_Absyn_Tqual yyzzz;{union Cyc_YYSTYPE_union _tmp1FC=yy1;struct Cyc_Absyn_Tqual
_tmp1FD;_LL15D: if((_tmp1FC.YY25).tag != 29)goto _LL15F;_tmp1FD=(_tmp1FC.YY25).f1;
_LL15E: yyzzz=_tmp1FD;goto _LL15C;_LL15F:;_LL160:(int)_throw((void*)& Cyc_yyfail_YY25);
_LL15C:;}return yyzzz;}static char _tmp1FF[20]="list_t<aggrfield_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp1FF,_tmp1FF + 20}};
struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE_union yy1);struct Cyc_List_List*
Cyc_yyget_YY26(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp200=yy1;struct Cyc_List_List*_tmp201;_LL162: if((_tmp200.YY26).tag != 30)goto
_LL164;_tmp201=(_tmp200.YY26).f1;_LL163: yyzzz=_tmp201;goto _LL161;_LL164:;_LL165:(
int)_throw((void*)& Cyc_yyfail_YY26);_LL161:;}return yyzzz;}static char _tmp203[28]="list_t<list_t<aggrfield_t>>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp203,
_tmp203 + 28}};struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE_union yy1);
struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp204=yy1;struct Cyc_List_List*_tmp205;_LL167: if((
_tmp204.YY27).tag != 31)goto _LL169;_tmp205=(_tmp204.YY27).f1;_LL168: yyzzz=_tmp205;
goto _LL166;_LL169:;_LL16A:(int)_throw((void*)& Cyc_yyfail_YY27);_LL166:;}return
yyzzz;}static char _tmp207[24]="list_t<type_modifier_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp207,_tmp207 + 24}};struct Cyc_List_List*Cyc_yyget_YY28(
union Cyc_YYSTYPE_union yy1);struct Cyc_List_List*Cyc_yyget_YY28(union Cyc_YYSTYPE_union
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp208=yy1;struct Cyc_List_List*
_tmp209;_LL16C: if((_tmp208.YY28).tag != 32)goto _LL16E;_tmp209=(_tmp208.YY28).f1;
_LL16D: yyzzz=_tmp209;goto _LL16B;_LL16E:;_LL16F:(int)_throw((void*)& Cyc_yyfail_YY28);
_LL16B:;}return yyzzz;}static char _tmp20B[13]="declarator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp20B,_tmp20B + 13}};struct Cyc_Parse_Declarator*
Cyc_yyget_YY29(union Cyc_YYSTYPE_union yy1);struct Cyc_Parse_Declarator*Cyc_yyget_YY29(
union Cyc_YYSTYPE_union yy1){struct Cyc_Parse_Declarator*yyzzz;{union Cyc_YYSTYPE_union
_tmp20C=yy1;struct Cyc_Parse_Declarator*_tmp20D;_LL171: if((_tmp20C.YY29).tag != 33)
goto _LL173;_tmp20D=(_tmp20C.YY29).f1;_LL172: yyzzz=_tmp20D;goto _LL170;_LL173:;
_LL174:(int)_throw((void*)& Cyc_yyfail_YY29);_LL170:;}return yyzzz;}static char
_tmp20F[21]="abstractdeclarator_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY30={
Cyc_Core_Failure,{_tmp20F,_tmp20F + 21}};struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_YY30(
union Cyc_YYSTYPE_union yy1);struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_YY30(
union Cyc_YYSTYPE_union yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{union Cyc_YYSTYPE_union
_tmp210=yy1;struct Cyc_Parse_Abstractdeclarator*_tmp211;_LL176: if((_tmp210.YY30).tag
!= 34)goto _LL178;_tmp211=(_tmp210.YY30).f1;_LL177: yyzzz=_tmp211;goto _LL175;
_LL178:;_LL179:(int)_throw((void*)& Cyc_yyfail_YY30);_LL175:;}return yyzzz;}static
char _tmp213[5]="bool";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{
_tmp213,_tmp213 + 5}};int Cyc_yyget_YY31(union Cyc_YYSTYPE_union yy1);int Cyc_yyget_YY31(
union Cyc_YYSTYPE_union yy1){int yyzzz;{union Cyc_YYSTYPE_union _tmp214=yy1;int
_tmp215;_LL17B: if((_tmp214.YY31).tag != 35)goto _LL17D;_tmp215=(_tmp214.YY31).f1;
_LL17C: yyzzz=_tmp215;goto _LL17A;_LL17D:;_LL17E:(int)_throw((void*)& Cyc_yyfail_YY31);
_LL17A:;}return yyzzz;}static char _tmp217[8]="scope_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp217,_tmp217 + 8}};void*Cyc_yyget_YY32(
union Cyc_YYSTYPE_union yy1);void*Cyc_yyget_YY32(union Cyc_YYSTYPE_union yy1){void*
yyzzz;{union Cyc_YYSTYPE_union _tmp218=yy1;void*_tmp219;_LL180: if((_tmp218.YY32).tag
!= 36)goto _LL182;_tmp219=(_tmp218.YY32).f1;_LL181: yyzzz=_tmp219;goto _LL17F;
_LL182:;_LL183:(int)_throw((void*)& Cyc_yyfail_YY32);_LL17F:;}return yyzzz;}static
char _tmp21B[14]="tunionfield_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY33={
Cyc_Core_Failure,{_tmp21B,_tmp21B + 14}};struct Cyc_Absyn_Tunionfield*Cyc_yyget_YY33(
union Cyc_YYSTYPE_union yy1);struct Cyc_Absyn_Tunionfield*Cyc_yyget_YY33(union Cyc_YYSTYPE_union
yy1){struct Cyc_Absyn_Tunionfield*yyzzz;{union Cyc_YYSTYPE_union _tmp21C=yy1;struct
Cyc_Absyn_Tunionfield*_tmp21D;_LL185: if((_tmp21C.YY33).tag != 37)goto _LL187;
_tmp21D=(_tmp21C.YY33).f1;_LL186: yyzzz=_tmp21D;goto _LL184;_LL187:;_LL188:(int)
_throw((void*)& Cyc_yyfail_YY33);_LL184:;}return yyzzz;}static char _tmp21F[22]="list_t<tunionfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp21F,
_tmp21F + 22}};struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE_union yy1);
struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp220=yy1;struct Cyc_List_List*_tmp221;_LL18A: if((
_tmp220.YY34).tag != 38)goto _LL18C;_tmp221=(_tmp220.YY34).f1;_LL18B: yyzzz=_tmp221;
goto _LL189;_LL18C:;_LL18D:(int)_throw((void*)& Cyc_yyfail_YY34);_LL189:;}return
yyzzz;}static char _tmp223[50]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp223,
_tmp223 + 50}};struct _tuple16*Cyc_yyget_YY35(union Cyc_YYSTYPE_union yy1);struct
_tuple16*Cyc_yyget_YY35(union Cyc_YYSTYPE_union yy1){struct _tuple16*yyzzz;{union
Cyc_YYSTYPE_union _tmp224=yy1;struct _tuple16*_tmp225;_LL18F: if((_tmp224.YY35).tag
!= 39)goto _LL191;_tmp225=(_tmp224.YY35).f1;_LL190: yyzzz=_tmp225;goto _LL18E;
_LL191:;_LL192:(int)_throw((void*)& Cyc_yyfail_YY35);_LL18E:;}return yyzzz;}static
char _tmp227[14]="list_t<var_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY36={
Cyc_Core_Failure,{_tmp227,_tmp227 + 14}};struct Cyc_List_List*Cyc_yyget_YY36(union
Cyc_YYSTYPE_union yy1);struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE_union
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp228=yy1;struct Cyc_List_List*
_tmp229;_LL194: if((_tmp228.YY36).tag != 40)goto _LL196;_tmp229=(_tmp228.YY36).f1;
_LL195: yyzzz=_tmp229;goto _LL193;_LL196:;_LL197:(int)_throw((void*)& Cyc_yyfail_YY36);
_LL193:;}return yyzzz;}static char _tmp22B[32]="$(opt_t<var_t>,tqual_t,type_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp22B,
_tmp22B + 32}};struct _tuple2*Cyc_yyget_YY37(union Cyc_YYSTYPE_union yy1);struct
_tuple2*Cyc_yyget_YY37(union Cyc_YYSTYPE_union yy1){struct _tuple2*yyzzz;{union Cyc_YYSTYPE_union
_tmp22C=yy1;struct _tuple2*_tmp22D;_LL199: if((_tmp22C.YY37).tag != 41)goto _LL19B;
_tmp22D=(_tmp22C.YY37).f1;_LL19A: yyzzz=_tmp22D;goto _LL198;_LL19B:;_LL19C:(int)
_throw((void*)& Cyc_yyfail_YY37);_LL198:;}return yyzzz;}static char _tmp22F[40]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp22F,
_tmp22F + 40}};struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE_union yy1);
struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp230=yy1;struct Cyc_List_List*_tmp231;_LL19E: if((
_tmp230.YY38).tag != 42)goto _LL1A0;_tmp231=(_tmp230.YY38).f1;_LL19F: yyzzz=_tmp231;
goto _LL19D;_LL1A0:;_LL1A1:(int)_throw((void*)& Cyc_yyfail_YY38);_LL19D:;}return
yyzzz;}static char _tmp233[107]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp233,
_tmp233 + 107}};struct _tuple17*Cyc_yyget_YY39(union Cyc_YYSTYPE_union yy1);struct
_tuple17*Cyc_yyget_YY39(union Cyc_YYSTYPE_union yy1){struct _tuple17*yyzzz;{union
Cyc_YYSTYPE_union _tmp234=yy1;struct _tuple17*_tmp235;_LL1A3: if((_tmp234.YY39).tag
!= 43)goto _LL1A5;_tmp235=(_tmp234.YY39).f1;_LL1A4: yyzzz=_tmp235;goto _LL1A2;
_LL1A5:;_LL1A6:(int)_throw((void*)& Cyc_yyfail_YY39);_LL1A2:;}return yyzzz;}static
char _tmp237[15]="list_t<type_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY40={
Cyc_Core_Failure,{_tmp237,_tmp237 + 15}};struct Cyc_List_List*Cyc_yyget_YY40(union
Cyc_YYSTYPE_union yy1);struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE_union
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp238=yy1;struct Cyc_List_List*
_tmp239;_LL1A8: if((_tmp238.YY40).tag != 44)goto _LL1AA;_tmp239=(_tmp238.YY40).f1;
_LL1A9: yyzzz=_tmp239;goto _LL1A7;_LL1AA:;_LL1AB:(int)_throw((void*)& Cyc_yyfail_YY40);
_LL1A7:;}return yyzzz;}static char _tmp23B[21]="list_t<designator_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp23B,_tmp23B + 21}};
struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE_union yy1);struct Cyc_List_List*
Cyc_yyget_YY41(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp23C=yy1;struct Cyc_List_List*_tmp23D;_LL1AD: if((_tmp23C.YY41).tag != 45)goto
_LL1AF;_tmp23D=(_tmp23C.YY41).f1;_LL1AE: yyzzz=_tmp23D;goto _LL1AC;_LL1AF:;_LL1B0:(
int)_throw((void*)& Cyc_yyfail_YY41);_LL1AC:;}return yyzzz;}static char _tmp23F[13]="designator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp23F,
_tmp23F + 13}};void*Cyc_yyget_YY42(union Cyc_YYSTYPE_union yy1);void*Cyc_yyget_YY42(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp240=yy1;void*
_tmp241;_LL1B2: if((_tmp240.YY42).tag != 46)goto _LL1B4;_tmp241=(_tmp240.YY42).f1;
_LL1B3: yyzzz=_tmp241;goto _LL1B1;_LL1B4:;_LL1B5:(int)_throw((void*)& Cyc_yyfail_YY42);
_LL1B1:;}return yyzzz;}static char _tmp243[7]="kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp243,_tmp243 + 7}};void*Cyc_yyget_YY43(
union Cyc_YYSTYPE_union yy1);void*Cyc_yyget_YY43(union Cyc_YYSTYPE_union yy1){void*
yyzzz;{union Cyc_YYSTYPE_union _tmp244=yy1;void*_tmp245;_LL1B7: if((_tmp244.YY43).tag
!= 47)goto _LL1B9;_tmp245=(_tmp244.YY43).f1;_LL1B8: yyzzz=_tmp245;goto _LL1B6;
_LL1B9:;_LL1BA:(int)_throw((void*)& Cyc_yyfail_YY43);_LL1B6:;}return yyzzz;}static
char _tmp247[7]="type_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY44={Cyc_Core_Failure,{
_tmp247,_tmp247 + 7}};void*Cyc_yyget_YY44(union Cyc_YYSTYPE_union yy1);void*Cyc_yyget_YY44(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp248=yy1;void*
_tmp249;_LL1BC: if((_tmp248.YY44).tag != 48)goto _LL1BE;_tmp249=(_tmp248.YY44).f1;
_LL1BD: yyzzz=_tmp249;goto _LL1BB;_LL1BE:;_LL1BF:(int)_throw((void*)& Cyc_yyfail_YY44);
_LL1BB:;}return yyzzz;}static char _tmp24B[20]="list_t<attribute_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp24B,_tmp24B + 20}};
struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE_union yy1);struct Cyc_List_List*
Cyc_yyget_YY45(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp24C=yy1;struct Cyc_List_List*_tmp24D;_LL1C1: if((_tmp24C.YY45).tag != 49)goto
_LL1C3;_tmp24D=(_tmp24C.YY45).f1;_LL1C2: yyzzz=_tmp24D;goto _LL1C0;_LL1C3:;_LL1C4:(
int)_throw((void*)& Cyc_yyfail_YY45);_LL1C0:;}return yyzzz;}static char _tmp24F[12]="attribute_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp24F,
_tmp24F + 12}};void*Cyc_yyget_YY46(union Cyc_YYSTYPE_union yy1);void*Cyc_yyget_YY46(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp250=yy1;void*
_tmp251;_LL1C6: if((_tmp250.YY46).tag != 50)goto _LL1C8;_tmp251=(_tmp250.YY46).f1;
_LL1C7: yyzzz=_tmp251;goto _LL1C5;_LL1C8:;_LL1C9:(int)_throw((void*)& Cyc_yyfail_YY46);
_LL1C5:;}return yyzzz;}static char _tmp253[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp253,_tmp253 + 12}};struct Cyc_Absyn_Enumfield*
Cyc_yyget_YY47(union Cyc_YYSTYPE_union yy1);struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(
union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Enumfield*yyzzz;{union Cyc_YYSTYPE_union
_tmp254=yy1;struct Cyc_Absyn_Enumfield*_tmp255;_LL1CB: if((_tmp254.YY47).tag != 51)
goto _LL1CD;_tmp255=(_tmp254.YY47).f1;_LL1CC: yyzzz=_tmp255;goto _LL1CA;_LL1CD:;
_LL1CE:(int)_throw((void*)& Cyc_yyfail_YY47);_LL1CA:;}return yyzzz;}static char
_tmp257[20]="list_t<enumfield_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY48={
Cyc_Core_Failure,{_tmp257,_tmp257 + 20}};struct Cyc_List_List*Cyc_yyget_YY48(union
Cyc_YYSTYPE_union yy1);struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE_union
yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp258=yy1;struct Cyc_List_List*
_tmp259;_LL1D0: if((_tmp258.YY48).tag != 52)goto _LL1D2;_tmp259=(_tmp258.YY48).f1;
_LL1D1: yyzzz=_tmp259;goto _LL1CF;_LL1D2:;_LL1D3:(int)_throw((void*)& Cyc_yyfail_YY48);
_LL1CF:;}return yyzzz;}static char _tmp25B[14]="opt_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp25B,_tmp25B + 14}};struct Cyc_Core_Opt*Cyc_yyget_YY49(
union Cyc_YYSTYPE_union yy1);struct Cyc_Core_Opt*Cyc_yyget_YY49(union Cyc_YYSTYPE_union
yy1){struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE_union _tmp25C=yy1;struct Cyc_Core_Opt*
_tmp25D;_LL1D5: if((_tmp25C.YY49).tag != 53)goto _LL1D7;_tmp25D=(_tmp25C.YY49).f1;
_LL1D6: yyzzz=_tmp25D;goto _LL1D4;_LL1D7:;_LL1D8:(int)_throw((void*)& Cyc_yyfail_YY49);
_LL1D4:;}return yyzzz;}static char _tmp25F[26]="list_t<$(type_t,type_t)@>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp25F,_tmp25F + 
26}};struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE_union yy1);struct Cyc_List_List*
Cyc_yyget_YY50(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp260=yy1;struct Cyc_List_List*_tmp261;_LL1DA: if((_tmp260.YY50).tag != 54)goto
_LL1DC;_tmp261=(_tmp260.YY50).f1;_LL1DB: yyzzz=_tmp261;goto _LL1D9;_LL1DC:;_LL1DD:(
int)_throw((void*)& Cyc_yyfail_YY50);_LL1D9:;}return yyzzz;}static char _tmp263[15]="conref_t<bool>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp263,
_tmp263 + 15}};struct Cyc_Absyn_Conref*Cyc_yyget_YY51(union Cyc_YYSTYPE_union yy1);
struct Cyc_Absyn_Conref*Cyc_yyget_YY51(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Conref*
yyzzz;{union Cyc_YYSTYPE_union _tmp264=yy1;struct Cyc_Absyn_Conref*_tmp265;_LL1DF:
if((_tmp264.YY51).tag != 55)goto _LL1E1;_tmp265=(_tmp264.YY51).f1;_LL1E0: yyzzz=
_tmp265;goto _LL1DE;_LL1E1:;_LL1E2:(int)_throw((void*)& Cyc_yyfail_YY51);_LL1DE:;}
return yyzzz;}static char _tmp267[40]="list_t<$(Position::seg_t,qvar_t,bool)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp267,
_tmp267 + 40}};struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE_union yy1);
struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp268=yy1;struct Cyc_List_List*_tmp269;_LL1E4: if((
_tmp268.YY52).tag != 56)goto _LL1E6;_tmp269=(_tmp268.YY52).f1;_LL1E5: yyzzz=_tmp269;
goto _LL1E3;_LL1E6:;_LL1E7:(int)_throw((void*)& Cyc_yyfail_YY52);_LL1E3:;}return
yyzzz;}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;
int last_column;};struct Cyc_Yyltype Cyc_yynewloc();struct Cyc_Yyltype Cyc_yynewloc(){
struct Cyc_Yyltype _tmpA0B;return(_tmpA0B.timestamp=0,((_tmpA0B.first_line=0,((
_tmpA0B.first_column=0,((_tmpA0B.last_line=0,((_tmpA0B.last_column=0,_tmpA0B)))))))));}
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};static short Cyc_yytranslate[361]={0,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,132,2,2,114,130,128,2,
111,112,120,125,109,123,117,129,2,2,2,2,2,2,2,2,2,2,116,106,115,110,124,122,121,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,118,2,119,127,113,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,107,126,108,131,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,
58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,
85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105};static char
_tmp26B[2]="$";static char _tmp26C[6]="error";static char _tmp26D[12]="$undefined.";
static char _tmp26E[5]="AUTO";static char _tmp26F[9]="REGISTER";static char _tmp270[7]="STATIC";
static char _tmp271[7]="EXTERN";static char _tmp272[8]="TYPEDEF";static char _tmp273[5]="VOID";
static char _tmp274[5]="CHAR";static char _tmp275[6]="SHORT";static char _tmp276[4]="INT";
static char _tmp277[5]="LONG";static char _tmp278[6]="FLOAT";static char _tmp279[7]="DOUBLE";
static char _tmp27A[7]="SIGNED";static char _tmp27B[9]="UNSIGNED";static char _tmp27C[
6]="CONST";static char _tmp27D[9]="VOLATILE";static char _tmp27E[9]="RESTRICT";
static char _tmp27F[7]="STRUCT";static char _tmp280[6]="UNION";static char _tmp281[5]="CASE";
static char _tmp282[8]="DEFAULT";static char _tmp283[7]="INLINE";static char _tmp284[7]="SIZEOF";
static char _tmp285[9]="OFFSETOF";static char _tmp286[3]="IF";static char _tmp287[5]="ELSE";
static char _tmp288[7]="SWITCH";static char _tmp289[6]="WHILE";static char _tmp28A[3]="DO";
static char _tmp28B[4]="FOR";static char _tmp28C[5]="GOTO";static char _tmp28D[9]="CONTINUE";
static char _tmp28E[6]="BREAK";static char _tmp28F[7]="RETURN";static char _tmp290[5]="ENUM";
static char _tmp291[8]="NULL_kw";static char _tmp292[4]="LET";static char _tmp293[6]="THROW";
static char _tmp294[4]="TRY";static char _tmp295[6]="CATCH";static char _tmp296[7]="EXPORT";
static char _tmp297[4]="NEW";static char _tmp298[9]="ABSTRACT";static char _tmp299[9]="FALLTHRU";
static char _tmp29A[6]="USING";static char _tmp29B[10]="NAMESPACE";static char _tmp29C[
7]="TUNION";static char _tmp29D[8]="XTUNION";static char _tmp29E[7]="MALLOC";static
char _tmp29F[8]="RMALLOC";static char _tmp2A0[7]="CALLOC";static char _tmp2A1[8]="RCALLOC";
static char _tmp2A2[5]="SWAP";static char _tmp2A3[9]="REGION_T";static char _tmp2A4[9]="SIZEOF_T";
static char _tmp2A5[6]="TAG_T";static char _tmp2A6[7]="REGION";static char _tmp2A7[5]="RNEW";
static char _tmp2A8[8]="REGIONS";static char _tmp2A9[13]="RESET_REGION";static char
_tmp2AA[4]="GEN";static char _tmp2AB[14]="NOZEROTERM_kw";static char _tmp2AC[12]="ZEROTERM_kw";
static char _tmp2AD[7]="PORTON";static char _tmp2AE[8]="PORTOFF";static char _tmp2AF[8]="FLAT_kw";
static char _tmp2B0[12]="DYNREGION_T";static char _tmp2B1[6]="ALIAS";static char
_tmp2B2[7]="PTR_OP";static char _tmp2B3[7]="INC_OP";static char _tmp2B4[7]="DEC_OP";
static char _tmp2B5[8]="LEFT_OP";static char _tmp2B6[9]="RIGHT_OP";static char _tmp2B7[
6]="LE_OP";static char _tmp2B8[6]="GE_OP";static char _tmp2B9[6]="EQ_OP";static char
_tmp2BA[6]="NE_OP";static char _tmp2BB[7]="AND_OP";static char _tmp2BC[6]="OR_OP";
static char _tmp2BD[11]="MUL_ASSIGN";static char _tmp2BE[11]="DIV_ASSIGN";static char
_tmp2BF[11]="MOD_ASSIGN";static char _tmp2C0[11]="ADD_ASSIGN";static char _tmp2C1[11]="SUB_ASSIGN";
static char _tmp2C2[12]="LEFT_ASSIGN";static char _tmp2C3[13]="RIGHT_ASSIGN";static
char _tmp2C4[11]="AND_ASSIGN";static char _tmp2C5[11]="XOR_ASSIGN";static char
_tmp2C6[10]="OR_ASSIGN";static char _tmp2C7[9]="ELLIPSIS";static char _tmp2C8[11]="LEFT_RIGHT";
static char _tmp2C9[12]="COLON_COLON";static char _tmp2CA[11]="IDENTIFIER";static
char _tmp2CB[17]="INTEGER_CONSTANT";static char _tmp2CC[7]="STRING";static char
_tmp2CD[19]="CHARACTER_CONSTANT";static char _tmp2CE[18]="FLOATING_CONSTANT";
static char _tmp2CF[9]="TYPE_VAR";static char _tmp2D0[13]="TYPEDEF_NAME";static char
_tmp2D1[16]="QUAL_IDENTIFIER";static char _tmp2D2[18]="QUAL_TYPEDEF_NAME";static
char _tmp2D3[13]="TYPE_INTEGER";static char _tmp2D4[10]="ATTRIBUTE";static char
_tmp2D5[4]="';'";static char _tmp2D6[4]="'{'";static char _tmp2D7[4]="'}'";static
char _tmp2D8[4]="','";static char _tmp2D9[4]="'='";static char _tmp2DA[4]="'('";
static char _tmp2DB[4]="')'";static char _tmp2DC[4]="'_'";static char _tmp2DD[4]="'$'";
static char _tmp2DE[4]="'<'";static char _tmp2DF[4]="':'";static char _tmp2E0[4]="'.'";
static char _tmp2E1[4]="'['";static char _tmp2E2[4]="']'";static char _tmp2E3[4]="'*'";
static char _tmp2E4[4]="'@'";static char _tmp2E5[4]="'?'";static char _tmp2E6[4]="'-'";
static char _tmp2E7[4]="'>'";static char _tmp2E8[4]="'+'";static char _tmp2E9[4]="'|'";
static char _tmp2EA[4]="'^'";static char _tmp2EB[4]="'&'";static char _tmp2EC[4]="'/'";
static char _tmp2ED[4]="'%'";static char _tmp2EE[4]="'~'";static char _tmp2EF[4]="'!'";
static char _tmp2F0[5]="prog";static char _tmp2F1[17]="translation_unit";static char
_tmp2F2[12]="export_list";static char _tmp2F3[19]="export_list_values";static char
_tmp2F4[21]="external_declaration";static char _tmp2F5[20]="function_definition";
static char _tmp2F6[21]="function_definition2";static char _tmp2F7[13]="using_action";
static char _tmp2F8[15]="unusing_action";static char _tmp2F9[17]="namespace_action";
static char _tmp2FA[19]="unnamespace_action";static char _tmp2FB[12]="declaration";
static char _tmp2FC[17]="declaration_list";static char _tmp2FD[23]="declaration_specifiers";
static char _tmp2FE[24]="storage_class_specifier";static char _tmp2FF[15]="attributes_opt";
static char _tmp300[11]="attributes";static char _tmp301[15]="attribute_list";static
char _tmp302[10]="attribute";static char _tmp303[15]="type_specifier";static char
_tmp304[25]="type_specifier_notypedef";static char _tmp305[5]="kind";static char
_tmp306[15]="type_qualifier";static char _tmp307[15]="enum_specifier";static char
_tmp308[11]="enum_field";static char _tmp309[22]="enum_declaration_list";static
char _tmp30A[26]="struct_or_union_specifier";static char _tmp30B[16]="type_params_opt";
static char _tmp30C[16]="struct_or_union";static char _tmp30D[24]="struct_declaration_list";
static char _tmp30E[25]="struct_declaration_list0";static char _tmp30F[21]="init_declarator_list";
static char _tmp310[22]="init_declarator_list0";static char _tmp311[16]="init_declarator";
static char _tmp312[19]="struct_declaration";static char _tmp313[25]="specifier_qualifier_list";
static char _tmp314[35]="notypedef_specifier_qualifier_list";static char _tmp315[23]="struct_declarator_list";
static char _tmp316[24]="struct_declarator_list0";static char _tmp317[18]="struct_declarator";
static char _tmp318[17]="tunion_specifier";static char _tmp319[9]="flat_opt";static
char _tmp31A[18]="tunion_or_xtunion";static char _tmp31B[17]="tunionfield_list";
static char _tmp31C[18]="tunionfield_scope";static char _tmp31D[12]="tunionfield";
static char _tmp31E[11]="declarator";static char _tmp31F[23]="declarator_withtypedef";
static char _tmp320[18]="direct_declarator";static char _tmp321[30]="direct_declarator_withtypedef";
static char _tmp322[8]="pointer";static char _tmp323[12]="one_pointer";static char
_tmp324[23]="pointer_null_and_bound";static char _tmp325[14]="pointer_bound";
static char _tmp326[18]="zeroterm_qual_opt";static char _tmp327[12]="opt_rgn_opt";
static char _tmp328[8]="rgn_opt";static char _tmp329[11]="tqual_list";static char
_tmp32A[20]="parameter_type_list";static char _tmp32B[9]="type_var";static char
_tmp32C[16]="optional_effect";static char _tmp32D[19]="optional_rgn_order";static
char _tmp32E[10]="rgn_order";static char _tmp32F[16]="optional_inject";static char
_tmp330[11]="effect_set";static char _tmp331[14]="atomic_effect";static char _tmp332[
11]="region_set";static char _tmp333[15]="parameter_list";static char _tmp334[22]="parameter_declaration";
static char _tmp335[16]="identifier_list";static char _tmp336[17]="identifier_list0";
static char _tmp337[12]="initializer";static char _tmp338[18]="array_initializer";
static char _tmp339[17]="initializer_list";static char _tmp33A[12]="designation";
static char _tmp33B[16]="designator_list";static char _tmp33C[11]="designator";
static char _tmp33D[10]="type_name";static char _tmp33E[14]="any_type_name";static
char _tmp33F[15]="type_name_list";static char _tmp340[20]="abstract_declarator";
static char _tmp341[27]="direct_abstract_declarator";static char _tmp342[10]="statement";
static char _tmp343[13]="open_exp_opt";static char _tmp344[18]="labeled_statement";
static char _tmp345[21]="expression_statement";static char _tmp346[19]="compound_statement";
static char _tmp347[16]="block_item_list";static char _tmp348[20]="selection_statement";
static char _tmp349[15]="switch_clauses";static char _tmp34A[20]="iteration_statement";
static char _tmp34B[15]="jump_statement";static char _tmp34C[12]="exp_pattern";
static char _tmp34D[20]="conditional_pattern";static char _tmp34E[19]="logical_or_pattern";
static char _tmp34F[20]="logical_and_pattern";static char _tmp350[21]="inclusive_or_pattern";
static char _tmp351[21]="exclusive_or_pattern";static char _tmp352[12]="and_pattern";
static char _tmp353[17]="equality_pattern";static char _tmp354[19]="relational_pattern";
static char _tmp355[14]="shift_pattern";static char _tmp356[17]="additive_pattern";
static char _tmp357[23]="multiplicative_pattern";static char _tmp358[13]="cast_pattern";
static char _tmp359[14]="unary_pattern";static char _tmp35A[16]="postfix_pattern";
static char _tmp35B[16]="primary_pattern";static char _tmp35C[8]="pattern";static
char _tmp35D[19]="tuple_pattern_list";static char _tmp35E[20]="tuple_pattern_list0";
static char _tmp35F[14]="field_pattern";static char _tmp360[19]="field_pattern_list";
static char _tmp361[20]="field_pattern_list0";static char _tmp362[11]="expression";
static char _tmp363[22]="assignment_expression";static char _tmp364[20]="assignment_operator";
static char _tmp365[23]="conditional_expression";static char _tmp366[20]="constant_expression";
static char _tmp367[22]="logical_or_expression";static char _tmp368[23]="logical_and_expression";
static char _tmp369[24]="inclusive_or_expression";static char _tmp36A[24]="exclusive_or_expression";
static char _tmp36B[15]="and_expression";static char _tmp36C[20]="equality_expression";
static char _tmp36D[22]="relational_expression";static char _tmp36E[17]="shift_expression";
static char _tmp36F[20]="additive_expression";static char _tmp370[26]="multiplicative_expression";
static char _tmp371[16]="cast_expression";static char _tmp372[17]="unary_expression";
static char _tmp373[15]="unary_operator";static char _tmp374[19]="postfix_expression";
static char _tmp375[19]="primary_expression";static char _tmp376[25]="argument_expression_list";
static char _tmp377[26]="argument_expression_list0";static char _tmp378[9]="constant";
static char _tmp379[20]="qual_opt_identifier";static char _tmp37A[17]="qual_opt_typedef";
static char _tmp37B[18]="struct_union_name";static char _tmp37C[11]="field_name";
static char _tmp37D[12]="right_angle";static struct _dynforward_ptr Cyc_yytname[275]={{
_tmp26B,_tmp26B + 2},{_tmp26C,_tmp26C + 6},{_tmp26D,_tmp26D + 12},{_tmp26E,_tmp26E + 
5},{_tmp26F,_tmp26F + 9},{_tmp270,_tmp270 + 7},{_tmp271,_tmp271 + 7},{_tmp272,
_tmp272 + 8},{_tmp273,_tmp273 + 5},{_tmp274,_tmp274 + 5},{_tmp275,_tmp275 + 6},{
_tmp276,_tmp276 + 4},{_tmp277,_tmp277 + 5},{_tmp278,_tmp278 + 6},{_tmp279,_tmp279 + 
7},{_tmp27A,_tmp27A + 7},{_tmp27B,_tmp27B + 9},{_tmp27C,_tmp27C + 6},{_tmp27D,
_tmp27D + 9},{_tmp27E,_tmp27E + 9},{_tmp27F,_tmp27F + 7},{_tmp280,_tmp280 + 6},{
_tmp281,_tmp281 + 5},{_tmp282,_tmp282 + 8},{_tmp283,_tmp283 + 7},{_tmp284,_tmp284 + 
7},{_tmp285,_tmp285 + 9},{_tmp286,_tmp286 + 3},{_tmp287,_tmp287 + 5},{_tmp288,
_tmp288 + 7},{_tmp289,_tmp289 + 6},{_tmp28A,_tmp28A + 3},{_tmp28B,_tmp28B + 4},{
_tmp28C,_tmp28C + 5},{_tmp28D,_tmp28D + 9},{_tmp28E,_tmp28E + 6},{_tmp28F,_tmp28F + 
7},{_tmp290,_tmp290 + 5},{_tmp291,_tmp291 + 8},{_tmp292,_tmp292 + 4},{_tmp293,
_tmp293 + 6},{_tmp294,_tmp294 + 4},{_tmp295,_tmp295 + 6},{_tmp296,_tmp296 + 7},{
_tmp297,_tmp297 + 4},{_tmp298,_tmp298 + 9},{_tmp299,_tmp299 + 9},{_tmp29A,_tmp29A + 
6},{_tmp29B,_tmp29B + 10},{_tmp29C,_tmp29C + 7},{_tmp29D,_tmp29D + 8},{_tmp29E,
_tmp29E + 7},{_tmp29F,_tmp29F + 8},{_tmp2A0,_tmp2A0 + 7},{_tmp2A1,_tmp2A1 + 8},{
_tmp2A2,_tmp2A2 + 5},{_tmp2A3,_tmp2A3 + 9},{_tmp2A4,_tmp2A4 + 9},{_tmp2A5,_tmp2A5 + 
6},{_tmp2A6,_tmp2A6 + 7},{_tmp2A7,_tmp2A7 + 5},{_tmp2A8,_tmp2A8 + 8},{_tmp2A9,
_tmp2A9 + 13},{_tmp2AA,_tmp2AA + 4},{_tmp2AB,_tmp2AB + 14},{_tmp2AC,_tmp2AC + 12},{
_tmp2AD,_tmp2AD + 7},{_tmp2AE,_tmp2AE + 8},{_tmp2AF,_tmp2AF + 8},{_tmp2B0,_tmp2B0 + 
12},{_tmp2B1,_tmp2B1 + 6},{_tmp2B2,_tmp2B2 + 7},{_tmp2B3,_tmp2B3 + 7},{_tmp2B4,
_tmp2B4 + 7},{_tmp2B5,_tmp2B5 + 8},{_tmp2B6,_tmp2B6 + 9},{_tmp2B7,_tmp2B7 + 6},{
_tmp2B8,_tmp2B8 + 6},{_tmp2B9,_tmp2B9 + 6},{_tmp2BA,_tmp2BA + 6},{_tmp2BB,_tmp2BB + 
7},{_tmp2BC,_tmp2BC + 6},{_tmp2BD,_tmp2BD + 11},{_tmp2BE,_tmp2BE + 11},{_tmp2BF,
_tmp2BF + 11},{_tmp2C0,_tmp2C0 + 11},{_tmp2C1,_tmp2C1 + 11},{_tmp2C2,_tmp2C2 + 12},{
_tmp2C3,_tmp2C3 + 13},{_tmp2C4,_tmp2C4 + 11},{_tmp2C5,_tmp2C5 + 11},{_tmp2C6,
_tmp2C6 + 10},{_tmp2C7,_tmp2C7 + 9},{_tmp2C8,_tmp2C8 + 11},{_tmp2C9,_tmp2C9 + 12},{
_tmp2CA,_tmp2CA + 11},{_tmp2CB,_tmp2CB + 17},{_tmp2CC,_tmp2CC + 7},{_tmp2CD,_tmp2CD
+ 19},{_tmp2CE,_tmp2CE + 18},{_tmp2CF,_tmp2CF + 9},{_tmp2D0,_tmp2D0 + 13},{_tmp2D1,
_tmp2D1 + 16},{_tmp2D2,_tmp2D2 + 18},{_tmp2D3,_tmp2D3 + 13},{_tmp2D4,_tmp2D4 + 10},{
_tmp2D5,_tmp2D5 + 4},{_tmp2D6,_tmp2D6 + 4},{_tmp2D7,_tmp2D7 + 4},{_tmp2D8,_tmp2D8 + 
4},{_tmp2D9,_tmp2D9 + 4},{_tmp2DA,_tmp2DA + 4},{_tmp2DB,_tmp2DB + 4},{_tmp2DC,
_tmp2DC + 4},{_tmp2DD,_tmp2DD + 4},{_tmp2DE,_tmp2DE + 4},{_tmp2DF,_tmp2DF + 4},{
_tmp2E0,_tmp2E0 + 4},{_tmp2E1,_tmp2E1 + 4},{_tmp2E2,_tmp2E2 + 4},{_tmp2E3,_tmp2E3 + 
4},{_tmp2E4,_tmp2E4 + 4},{_tmp2E5,_tmp2E5 + 4},{_tmp2E6,_tmp2E6 + 4},{_tmp2E7,
_tmp2E7 + 4},{_tmp2E8,_tmp2E8 + 4},{_tmp2E9,_tmp2E9 + 4},{_tmp2EA,_tmp2EA + 4},{
_tmp2EB,_tmp2EB + 4},{_tmp2EC,_tmp2EC + 4},{_tmp2ED,_tmp2ED + 4},{_tmp2EE,_tmp2EE + 
4},{_tmp2EF,_tmp2EF + 4},{_tmp2F0,_tmp2F0 + 5},{_tmp2F1,_tmp2F1 + 17},{_tmp2F2,
_tmp2F2 + 12},{_tmp2F3,_tmp2F3 + 19},{_tmp2F4,_tmp2F4 + 21},{_tmp2F5,_tmp2F5 + 20},{
_tmp2F6,_tmp2F6 + 21},{_tmp2F7,_tmp2F7 + 13},{_tmp2F8,_tmp2F8 + 15},{_tmp2F9,
_tmp2F9 + 17},{_tmp2FA,_tmp2FA + 19},{_tmp2FB,_tmp2FB + 12},{_tmp2FC,_tmp2FC + 17},{
_tmp2FD,_tmp2FD + 23},{_tmp2FE,_tmp2FE + 24},{_tmp2FF,_tmp2FF + 15},{_tmp300,
_tmp300 + 11},{_tmp301,_tmp301 + 15},{_tmp302,_tmp302 + 10},{_tmp303,_tmp303 + 15},{
_tmp304,_tmp304 + 25},{_tmp305,_tmp305 + 5},{_tmp306,_tmp306 + 15},{_tmp307,_tmp307
+ 15},{_tmp308,_tmp308 + 11},{_tmp309,_tmp309 + 22},{_tmp30A,_tmp30A + 26},{_tmp30B,
_tmp30B + 16},{_tmp30C,_tmp30C + 16},{_tmp30D,_tmp30D + 24},{_tmp30E,_tmp30E + 25},{
_tmp30F,_tmp30F + 21},{_tmp310,_tmp310 + 22},{_tmp311,_tmp311 + 16},{_tmp312,
_tmp312 + 19},{_tmp313,_tmp313 + 25},{_tmp314,_tmp314 + 35},{_tmp315,_tmp315 + 23},{
_tmp316,_tmp316 + 24},{_tmp317,_tmp317 + 18},{_tmp318,_tmp318 + 17},{_tmp319,
_tmp319 + 9},{_tmp31A,_tmp31A + 18},{_tmp31B,_tmp31B + 17},{_tmp31C,_tmp31C + 18},{
_tmp31D,_tmp31D + 12},{_tmp31E,_tmp31E + 11},{_tmp31F,_tmp31F + 23},{_tmp320,
_tmp320 + 18},{_tmp321,_tmp321 + 30},{_tmp322,_tmp322 + 8},{_tmp323,_tmp323 + 12},{
_tmp324,_tmp324 + 23},{_tmp325,_tmp325 + 14},{_tmp326,_tmp326 + 18},{_tmp327,
_tmp327 + 12},{_tmp328,_tmp328 + 8},{_tmp329,_tmp329 + 11},{_tmp32A,_tmp32A + 20},{
_tmp32B,_tmp32B + 9},{_tmp32C,_tmp32C + 16},{_tmp32D,_tmp32D + 19},{_tmp32E,_tmp32E
+ 10},{_tmp32F,_tmp32F + 16},{_tmp330,_tmp330 + 11},{_tmp331,_tmp331 + 14},{_tmp332,
_tmp332 + 11},{_tmp333,_tmp333 + 15},{_tmp334,_tmp334 + 22},{_tmp335,_tmp335 + 16},{
_tmp336,_tmp336 + 17},{_tmp337,_tmp337 + 12},{_tmp338,_tmp338 + 18},{_tmp339,
_tmp339 + 17},{_tmp33A,_tmp33A + 12},{_tmp33B,_tmp33B + 16},{_tmp33C,_tmp33C + 11},{
_tmp33D,_tmp33D + 10},{_tmp33E,_tmp33E + 14},{_tmp33F,_tmp33F + 15},{_tmp340,
_tmp340 + 20},{_tmp341,_tmp341 + 27},{_tmp342,_tmp342 + 10},{_tmp343,_tmp343 + 13},{
_tmp344,_tmp344 + 18},{_tmp345,_tmp345 + 21},{_tmp346,_tmp346 + 19},{_tmp347,
_tmp347 + 16},{_tmp348,_tmp348 + 20},{_tmp349,_tmp349 + 15},{_tmp34A,_tmp34A + 20},{
_tmp34B,_tmp34B + 15},{_tmp34C,_tmp34C + 12},{_tmp34D,_tmp34D + 20},{_tmp34E,
_tmp34E + 19},{_tmp34F,_tmp34F + 20},{_tmp350,_tmp350 + 21},{_tmp351,_tmp351 + 21},{
_tmp352,_tmp352 + 12},{_tmp353,_tmp353 + 17},{_tmp354,_tmp354 + 19},{_tmp355,
_tmp355 + 14},{_tmp356,_tmp356 + 17},{_tmp357,_tmp357 + 23},{_tmp358,_tmp358 + 13},{
_tmp359,_tmp359 + 14},{_tmp35A,_tmp35A + 16},{_tmp35B,_tmp35B + 16},{_tmp35C,
_tmp35C + 8},{_tmp35D,_tmp35D + 19},{_tmp35E,_tmp35E + 20},{_tmp35F,_tmp35F + 14},{
_tmp360,_tmp360 + 19},{_tmp361,_tmp361 + 20},{_tmp362,_tmp362 + 11},{_tmp363,
_tmp363 + 22},{_tmp364,_tmp364 + 20},{_tmp365,_tmp365 + 23},{_tmp366,_tmp366 + 20},{
_tmp367,_tmp367 + 22},{_tmp368,_tmp368 + 23},{_tmp369,_tmp369 + 24},{_tmp36A,
_tmp36A + 24},{_tmp36B,_tmp36B + 15},{_tmp36C,_tmp36C + 20},{_tmp36D,_tmp36D + 22},{
_tmp36E,_tmp36E + 17},{_tmp36F,_tmp36F + 20},{_tmp370,_tmp370 + 26},{_tmp371,
_tmp371 + 16},{_tmp372,_tmp372 + 17},{_tmp373,_tmp373 + 15},{_tmp374,_tmp374 + 19},{
_tmp375,_tmp375 + 19},{_tmp376,_tmp376 + 25},{_tmp377,_tmp377 + 26},{_tmp378,
_tmp378 + 9},{_tmp379,_tmp379 + 20},{_tmp37A,_tmp37A + 17},{_tmp37B,_tmp37B + 18},{
_tmp37C,_tmp37C + 11},{_tmp37D,_tmp37D + 12}};static short Cyc_yyr1[501]={0,133,134,
134,134,134,134,134,134,134,134,134,135,135,136,136,137,137,137,138,138,138,138,
139,139,140,141,142,143,144,144,144,144,145,145,146,146,146,146,146,146,146,146,
146,146,147,147,147,147,147,147,147,148,148,149,150,150,151,151,151,151,151,151,
152,152,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,
153,153,153,153,153,154,155,155,155,156,156,156,157,157,158,158,158,159,159,159,
160,160,161,161,162,162,163,163,164,165,165,166,166,167,168,168,168,168,168,168,
169,169,169,169,169,169,170,171,171,172,172,172,173,173,173,174,174,175,175,176,
176,176,176,177,177,177,178,178,179,179,180,180,181,181,181,181,181,181,181,181,
181,181,182,182,182,182,182,182,182,182,182,182,182,183,183,184,185,185,185,185,
186,186,186,187,187,187,188,188,188,189,189,189,190,190,191,191,191,191,192,192,
193,193,194,194,195,195,196,196,197,197,198,198,198,198,199,199,200,200,201,201,
201,202,203,203,204,204,205,205,205,205,206,206,206,206,207,208,208,209,209,210,
210,211,211,211,211,211,212,212,213,213,213,214,214,214,214,214,214,214,214,214,
214,214,215,215,215,215,215,215,215,215,215,215,215,215,216,216,217,218,218,219,
219,220,220,220,220,220,220,221,221,221,221,221,221,222,222,222,222,222,222,223,
223,223,223,223,223,223,223,223,223,223,223,223,223,224,224,224,224,224,224,224,
224,225,226,226,227,227,228,228,229,229,230,230,231,231,232,232,232,233,233,233,
233,233,234,234,234,235,235,235,236,236,236,236,237,237,238,238,238,238,238,238,
238,239,240,241,241,241,241,241,241,241,241,241,241,241,241,241,241,242,242,242,
243,243,244,244,245,245,245,246,246,247,247,248,248,249,249,249,249,249,249,249,
249,249,249,249,250,250,250,250,250,250,250,251,252,252,253,253,254,254,255,255,
256,256,257,257,257,258,258,258,258,258,259,259,259,260,260,260,261,261,261,261,
262,262,263,263,263,263,263,263,263,263,263,263,263,263,263,263,263,263,263,264,
264,264,265,265,265,265,265,265,265,265,265,265,266,266,266,266,266,266,266,266,
266,267,268,268,269,269,269,269,270,270,271,271,272,272,273,273,274,274};static
short Cyc_yyr2[501]={0,1,2,3,5,3,5,6,7,3,3,0,4,3,2,3,1,1,1,2,3,3,4,3,4,2,1,2,1,2,
3,5,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,
1,1,1,1,1,1,1,1,1,1,4,4,4,6,4,4,1,1,1,1,1,1,5,2,4,1,3,1,2,3,4,8,3,0,3,1,1,0,1,1,
2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,1,2,3,8,5,7,1,0,1,1,1,2,3,3,0,1,1,2,
5,1,2,1,2,1,3,4,4,5,4,5,4,4,2,1,1,3,4,4,5,4,5,4,4,2,1,2,5,2,2,1,2,0,3,3,0,1,1,0,
1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,
3,4,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,
1,6,4,9,6,5,9,0,5,3,1,2,2,3,1,2,1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,
8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,
3,1,3,3,3,1,4,1,2,2,4,2,6,6,1,1,1,3,1,1,3,4,4,5,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,
1,3,1,3,1,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,
3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,6,5,4,6,9,11,6,1,1,1,1,4,3,4,3,
3,2,2,6,7,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1};static short Cyc_yydefact[
1007]={0,18,45,46,47,48,50,65,68,69,70,71,72,73,74,75,89,90,91,105,106,41,0,0,51,
0,0,0,0,86,0,0,138,0,491,201,493,492,494,87,0,0,66,0,183,183,181,1,0,16,0,0,17,0,
35,43,37,63,39,76,77,0,78,0,139,151,0,176,186,79,155,103,49,48,42,0,93,490,0,491,
487,488,489,0,366,0,0,0,0,224,0,368,369,25,27,139,139,139,0,0,139,0,0,0,0,0,139,
139,179,180,182,2,0,0,0,0,29,0,111,112,114,36,44,38,40,139,495,496,103,140,141,
189,139,33,139,0,19,139,139,0,164,152,177,188,187,192,139,64,0,49,97,0,95,0,491,
375,0,0,0,0,0,0,0,0,0,0,0,0,103,0,0,477,139,0,0,464,0,0,462,463,0,392,394,407,
415,417,419,421,423,425,428,433,436,439,443,0,445,465,476,475,0,376,374,32,0,0,
103,0,0,0,121,117,119,242,244,0,0,0,9,10,0,497,498,202,88,0,0,156,67,222,0,219,0,
0,0,3,0,5,0,30,0,0,139,20,0,139,109,0,102,191,0,190,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
491,280,282,288,284,0,286,265,266,267,0,268,269,270,0,34,21,114,209,225,0,0,205,
203,0,249,0,186,0,194,52,193,0,0,98,94,0,0,370,0,0,139,453,139,409,443,0,410,411,
0,0,0,0,0,0,0,139,446,447,139,0,0,0,449,450,448,0,367,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,397,398,399,400,401,402,403,404,405,406,396,0,451,0,471,472,0,0,0,
479,0,103,382,383,0,380,0,226,0,0,0,139,245,217,0,122,127,123,125,118,120,139,0,
251,243,252,500,499,0,81,84,85,139,82,58,57,0,55,157,139,221,153,251,223,165,166,
139,80,139,185,184,26,0,28,0,113,115,228,227,22,100,110,0,0,0,129,130,132,0,103,
103,0,0,0,0,0,0,139,0,317,318,319,0,0,321,0,277,0,0,0,0,0,289,285,114,287,283,
281,210,139,0,0,216,204,211,160,0,0,139,205,162,139,163,158,186,195,53,104,0,99,
414,96,92,378,379,0,0,0,229,0,0,233,0,0,0,238,0,0,0,0,0,0,139,0,0,0,478,485,0,
484,393,416,0,418,420,422,424,426,427,431,432,429,430,434,435,438,437,440,441,
442,395,470,467,0,469,0,139,0,371,0,377,31,389,0,385,390,0,387,372,0,0,246,128,
124,126,0,205,0,186,0,253,139,139,0,264,248,0,0,0,0,121,0,103,139,139,0,175,154,
220,4,6,0,133,116,0,0,205,0,136,0,0,0,296,0,0,0,0,0,316,320,0,0,0,0,0,0,0,0,0,
279,139,23,203,139,213,0,0,206,0,161,209,197,250,159,195,178,0,7,0,452,0,0,241,0,
230,0,234,237,239,457,0,0,0,0,0,0,0,0,0,444,481,0,0,468,466,0,0,381,384,386,373,
0,247,218,260,0,254,255,186,0,205,0,186,0,83,0,0,0,54,56,0,167,0,205,0,0,186,0,
131,134,139,103,146,0,0,0,0,0,0,0,0,0,0,0,0,0,296,322,0,0,272,0,0,0,0,24,205,0,
214,212,0,205,139,196,0,8,0,0,0,240,231,235,0,0,0,0,0,412,413,456,483,0,486,408,
480,482,0,388,391,259,257,262,0,263,256,186,0,61,59,60,168,171,0,173,174,169,186,
0,137,148,147,0,0,142,290,296,0,0,0,139,0,0,324,325,327,329,331,333,335,337,340,
345,348,351,355,357,364,365,0,139,293,302,0,0,0,0,0,0,0,0,0,0,323,0,0,0,0,275,0,
199,215,207,198,203,13,0,0,455,454,0,236,458,139,0,461,473,0,261,258,0,172,170,
101,135,149,146,146,0,0,296,139,361,139,0,358,139,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,359,297,0,304,0,0,0,312,0,0,0,0,0,0,295,0,271,274,0,0,0,205,12,14,0,0,
0,0,474,0,139,145,144,291,292,0,0,0,0,296,298,328,0,330,332,334,336,338,339,343,
344,341,342,346,347,350,349,352,353,354,0,303,305,306,0,314,313,0,308,0,0,0,278,
0,0,208,200,15,0,0,139,0,0,294,360,0,356,299,0,139,307,315,309,310,0,0,0,232,459,
0,62,150,0,0,326,296,300,311,273,276,0,363,362,301,460,0,0,0};static short Cyc_yydefgoto[
142]={1004,47,641,838,48,49,269,50,425,51,427,52,134,53,54,488,210,409,410,211,
57,223,212,59,150,151,60,147,61,244,245,117,118,119,246,213,392,437,438,439,62,
63,131,784,785,786,64,440,65,414,66,67,68,108,145,250,296,638,568,69,569,480,630,
472,476,477,387,289,231,88,89,504,430,505,506,507,508,214,291,292,570,398,272,
617,273,274,275,276,277,711,278,279,794,795,796,797,798,799,800,801,802,803,804,
805,806,807,808,809,376,377,378,558,559,560,280,181,364,182,494,183,184,185,186,
187,188,189,190,191,192,193,194,195,196,197,521,522,198,199,71,839,224,402};
static short Cyc_yypact[1007]={2652,- -32768,- -32768,- -32768,- -32768,- 53,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,3219,102,2878,- -32768,158,- 28,50,83,93,181,200,-
-32768,248,- -32768,164,- -32768,- -32768,- -32768,- -32768,240,496,319,288,317,317,
306,- -32768,2529,- -32768,128,402,- -32768,144,3219,3219,3219,- -32768,3219,- -32768,
- -32768,559,- -32768,475,3289,357,143,589,516,- -32768,- -32768,339,349,363,- -32768,
158,374,- -32768,2992,264,- -32768,- -32768,- -32768,5526,- -32768,382,392,2992,389,
406,407,- -32768,222,- -32768,- -32768,6033,6033,6033,2529,2529,6033,5,418,472,421,
5,6200,2023,- -32768,- -32768,- -32768,- -32768,2529,2774,2529,2774,- -32768,434,452,
- -32768,3148,- -32768,- -32768,- -32768,- -32768,6063,- -32768,- -32768,339,- -32768,-
-32768,168,1638,- -32768,3289,144,- -32768,3351,6033,3894,- -32768,357,- -32768,-
-32768,- -32768,186,6033,- -32768,2774,- -32768,457,492,503,158,105,- -32768,2992,
207,5746,511,5526,5580,517,522,524,526,533,538,339,5800,5800,- -32768,2148,554,
5854,- -32768,5854,5854,- -32768,- -32768,62,- -32768,- -32768,- 50,617,565,576,573,
530,- 14,611,252,194,- -32768,763,5854,238,- 4,- -32768,598,821,620,- -32768,- -32768,
634,5526,339,821,624,261,6048,6307,6048,326,- -32768,53,53,53,- -32768,- -32768,97,
- -32768,- -32768,- -32768,- -32768,42,625,- -32768,- -32768,509,463,- -32768,646,87,
637,- -32768,651,- -32768,655,- -32768,472,3990,3289,- -32768,662,6170,- -32768,605,
673,- -32768,158,- -32768,661,183,679,3505,683,712,703,709,4086,3505,119,- 49,732,
706,740,- -32768,- -32768,1508,1508,144,1508,- -32768,- -32768,- -32768,754,- -32768,-
-32768,- -32768,566,- -32768,- -32768,756,769,- -32768,126,760,778,568,792,781,- 39,
516,793,- -32768,813,- -32768,- 39,816,158,- -32768,5526,822,- -32768,807,809,2148,-
-32768,6200,- -32768,- -32768,3606,- -32768,855,5526,5526,5526,5526,5526,5526,829,
2148,- -32768,- -32768,1768,835,567,5526,- -32768,- -32768,- -32768,5526,- -32768,5854,
5526,5854,5854,5854,5854,5854,5854,5854,5854,5854,5854,5854,5854,5854,5854,5854,
5854,5854,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,5526,- -32768,5,- -32768,- -32768,4182,5,5526,- -32768,833,
339,- -32768,- -32768,838,842,2992,- -32768,574,1136,841,6033,- -32768,867,848,-
-32768,6307,6307,6307,- -32768,- -32768,2913,4278,145,- -32768,445,- -32768,- -32768,
126,- -32768,- -32768,- -32768,6033,- -32768,- -32768,866,868,872,- -32768,2843,-
-32768,541,236,- -32768,- -32768,- -32768,6200,- -32768,2273,- -32768,- -32768,- -32768,
2529,- -32768,2529,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,653,
5526,876,874,- -32768,869,283,339,160,5526,5526,873,879,5526,957,1898,882,- -32768,
- -32768,- -32768,593,947,- -32768,4374,881,893,899,5526,896,3505,- -32768,- -32768,
3148,- -32768,- -32768,- -32768,- -32768,6200,888,297,- -32768,- -32768,875,- -32768,
126,889,6185,778,- -32768,6033,- -32768,- -32768,516,86,- -32768,- -32768,2406,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,890,897,910,- -32768,5,5526,-
-32768,615,3990,366,- -32768,610,898,900,902,911,619,6200,907,915,5663,- -32768,-
-32768,912,916,- -32768,617,256,565,576,573,530,- 14,- 14,611,611,611,611,252,252,
194,194,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,922,- -32768,24,6033,3798,
- -32768,1097,- -32768,- -32768,- -32768,2992,- -32768,- -32768,919,926,- -32768,196,
908,- -32768,- -32768,- -32768,- -32768,924,778,925,516,909,445,3413,6033,4470,-
-32768,- -32768,53,785,927,42,3028,931,339,3351,6033,4566,- -32768,541,- -32768,-
-32768,- -32768,665,- -32768,- -32768,605,5526,778,158,937,629,630,5526,863,670,941,
4662,4758,682,- -32768,- -32768,946,942,943,959,3505,932,938,689,936,- -32768,3289,
- -32768,955,6033,- -32768,954,126,- -32768,939,- -32768,167,- -32768,781,- -32768,86,
- -32768,958,- -32768,2529,960,315,949,- -32768,950,- -32768,978,- -32768,- -32768,-
-32768,- -32768,5526,1041,5526,5526,5580,956,960,966,3798,- -32768,- -32768,5526,
5526,- -32768,- -32768,- 39,644,- -32768,- -32768,- -32768,- -32768,1330,- -32768,-
-32768,- -32768,967,- -32768,- -32768,516,969,778,- 39,516,951,- -32768,694,970,971,-
-32768,- -32768,975,- -32768,976,778,981,- 39,516,952,- -32768,- -32768,6063,339,887,
3505,965,982,1020,983,989,3505,5526,4854,699,4950,707,5046,863,- -32768,998,996,-
-32768,1013,112,1005,1017,- -32768,778,220,- -32768,- -32768,1023,778,6200,- -32768,
537,- -32768,1012,1015,5526,- -32768,- -32768,- -32768,3990,708,1018,1019,710,-
-32768,855,- -32768,- -32768,788,- -32768,- -32768,- -32768,- -32768,3798,- -32768,-
-32768,- -32768,- -32768,- -32768,1024,- -32768,- -32768,516,1034,- -32768,- -32768,-
-32768,- -32768,- -32768,1025,- -32768,- -32768,- -32768,516,1030,- -32768,- -32768,-
-32768,1031,158,717,1114,863,1037,5908,1036,2273,5854,1033,- -32768,- 44,- -32768,
1070,1027,782,836,99,847,318,360,- -32768,- -32768,- -32768,- -32768,1074,5854,1768,
- -32768,- -32768,716,3505,721,5142,3505,723,5238,5334,730,1047,- -32768,5526,3505,
3505,1056,- -32768,1048,- -32768,- -32768,1050,- -32768,955,- -32768,1049,731,- -32768,
- -32768,279,- -32768,- -32768,6200,1135,- -32768,- -32768,3702,- -32768,- -32768,1052,
- -32768,- -32768,- -32768,- -32768,1053,55,887,3505,1055,863,2148,- -32768,6200,1054,
- -32768,1292,5854,5526,5854,5854,5854,5854,5854,5854,5854,5854,5854,5854,5854,
5854,5854,5854,5854,5854,5854,5526,- -32768,- -32768,1059,- -32768,3505,3505,729,-
-32768,3505,3505,746,3505,751,5430,- -32768,765,- -32768,- -32768,1043,5526,126,778,
- -32768,- -32768,494,5526,1058,1057,- -32768,1075,6200,- -32768,- -32768,- -32768,-
-32768,1064,1061,1066,5854,863,- -32768,617,323,565,576,576,530,- 14,- 14,611,611,
611,611,252,252,194,194,- -32768,- -32768,- -32768,376,- -32768,- -32768,- -32768,3505,
- -32768,- -32768,3505,- -32768,3505,3505,766,- -32768,1083,101,- -32768,- -32768,-
-32768,818,1069,6200,1073,767,- -32768,960,562,- -32768,- -32768,5526,1292,- -32768,
- -32768,- -32768,- -32768,3505,3505,3505,- -32768,- -32768,1076,- -32768,- -32768,1078,
1079,- -32768,863,- -32768,- -32768,- -32768,- -32768,1082,- -32768,- -32768,- -32768,-
-32768,1182,1197,- -32768};static short Cyc_yypgoto[142]={- -32768,9,- -32768,285,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 41,- 117,19,- -32768,-
-32768,0,618,- -32768,71,- 173,1098,32,- -32768,- -32768,- 108,- -32768,- 12,- -32768,
501,- -32768,- -32768,- -32768,972,961,107,500,- -32768,- -32768,608,- -32768,- -32768,
- -32768,70,- -32768,- -32768,239,- 145,1143,- 339,301,- -32768,- -32768,1170,- 17,-
-32768,- -32768,582,- 123,- 98,- 122,- 81,311,590,595,- 382,- 454,- 105,- 257,- 113,-
-32768,- 206,- 151,- 511,- 246,- -32768,715,- 167,152,- 142,69,- 247,286,- -32768,- -32768,
- -32768,- 52,- 244,- -32768,- 462,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,95,1022,- -32768,552,- -32768,- -32768,299,626,- -32768,- 143,- 409,-
150,- 332,- 327,- 303,895,- 323,- 333,- 293,- 317,- 306,- 16,623,518,- -32768,- -32768,- 363,
- -32768,115,305,- 48,1175,- 348,20};static short Cyc_yytable[6422]={55,230,524,242,
298,326,546,530,531,526,313,314,136,127,287,288,529,310,544,578,628,55,547,133,
290,465,466,595,468,536,537,334,58,251,527,429,399,869,390,669,74,103,538,539,72,
303,459,297,55,532,533,534,535,58,55,55,55,111,55,407,782,783,342,343,55,140,460,
94,243,461,484,56,335,121,122,123,590,124,870,133,58,517,282,135,413,400,58,58,
58,372,58,270,56,281,646,615,58,631,55,55,221,344,590,16,17,18,222,218,219,676,
345,386,55,55,55,55,248,373,90,56,55,235,236,237,238,56,56,56,399,56,58,58,55,
332,55,56,556,408,91,135,498,140,499,667,58,58,58,58,55,573,754,271,58,135,645,
516,321,299,329,493,330,331,591,- 143,58,95,58,482,573,56,56,332,399,155,333,877,
878,400,401,365,58,418,202,56,56,56,56,473,475,702,432,56,492,91,382,422,985,34,
96,418,156,281,91,56,37,56,405,828,97,75,332,389,401,229,879,625,390,390,390,56,
157,400,401,880,591,457,35,829,270,270,458,270,247,474,112,113,403,404,34,34,406,
708,55,391,229,37,37,215,216,217,116,304,220,34,41,41,394,824,101,233,37,135,471,
395,44,45,46,584,35,55,55,91,55,286,58,146,486,600,34,- 203,104,249,397,- 203,270,
37,35,98,271,271,584,271,120,493,445,740,489,446,416,295,649,58,58,475,58,70,99,
305,675,366,367,368,485,56,350,91,230,388,490,393,306,401,91,351,352,55,861,76,
92,206,93,34,833,207,540,541,542,36,37,38,56,56,226,56,271,401,70,412,658,369,
623,102,247,70,395,370,371,58,70,156,493,35,550,100,585,332,126,418,142,385,- 225,
70,665,- 225,283,348,386,349,34,157,152,475,180,92,36,37,38,418,332,389,389,389,
92,418,435,914,56,35,577,106,924,634,745,70,70,627,929,668,70,609,221,739,583,
105,589,624,222,70,70,70,70,391,391,391,107,55,133,55,556,110,599,601,332,684,
592,594,593,394,693,977,70,883,744,884,395,698,44,45,46,693,55,682,683,70,146,
493,148,58,152,58,149,92,40,695,696,386,976,55,137,135,636,327,138,697,553,139,
650,557,836,283,885,153,58,502,503,332,135,201,678,886,887,55,978,200,91,203,56,
91,56,58,640,229,662,729,381,92,750,751,114,115,467,734,92,745,396,204,995,205,
703,229,637,56,756,58,129,130,229,631,229,225,415,475,1002,227,417,585,562,930,
56,843,239,449,936,937,932,70,418,456,441,418,40,935,417,744,680,443,574,579,447,
455,575,240,56,576,942,943,300,34,890,933,934,728,419,577,37,420,70,944,945,229,
143,144,281,41,938,939,940,941,229,34,589,34,44,45,46,36,37,38,37,687,301,40,765,
745,34,152,327,41,340,341,36,37,38,302,509,775,44,45,46,514,412,327,309,55,928,
866,992,395,315,44,45,46,34,316,525,317,635,318,36,37,38,55,135,744,319,837,40,
671,832,320,738,672,586,835,34,58,587,221,991,588,36,37,38,222,763,328,125,91,
767,637,548,91,470,58,286,332,332,481,915,519,554,229,778,332,92,346,347,92,757,
127,388,337,781,229,56,396,925,336,926,612,34,339,332,338,766,374,36,37,38,44,45,
46,56,415,910,379,435,417,777,332,417,436,652,647,648,44,45,46,332,380,70,657,70,
234,994,384,441,411,332,332,417,706,707,602,603,423,417,606,34,610,622,851,758,
759,36,37,38,421,40,424,34,620,854,426,435,294,36,37,38,557,433,270,444,44,45,46,
435,867,730,332,442,308,712,311,311,44,45,46,718,91,448,332,323,324,450,889,70,
311,332,311,311,726,988,769,810,818,770,451,332,452,247,475,55,821,971,453,332,
332,311,332,844,463,847,858,91,332,859,270,891,965,332,271,332,893,993,897,902,
912,332,332,913,953,229,462,58,353,354,355,356,357,358,359,360,361,362,332,464,
92,956,77,332,92,469,958,471,127,241,431,55,946,947,948,478,363,332,332,419,961,
983,990,688,689,690,56,441,709,710,271,417,565,566,567,782,783,479,441,848,849,
441,417,58,78,417,723,483,704,401,715,717,873,874,975,487,375,875,876,154,80,40,
81,82,881,882,37,491,311,986,332,920,921,495,496,83,497,84,85,334,270,431,56,515,
86,510,511,512,513,70,518,549,87,551,552,746,561,520,749,564,311,523,311,311,311,
311,311,311,311,311,311,311,311,311,311,311,311,311,311,563,580,55,92,581,582,
596,597,604,598,605,607,611,613,543,616,787,618,619,520,621,271,814,626,629,632,
642,158,159,644,643,653,35,654,58,655,815,817,92,820,77,823,160,659,656,572,161,
660,663,664,229,673,681,162,163,164,165,166,666,674,677,679,167,691,842,168,126,
694,705,790,791,520,720,56,169,170,713,719,722,721,724,725,77,311,727,286,731,
733,741,737,747,661,752,742,768,779,788,34,80,171,81,82,753,762,37,764,771,772,
520,312,743,773,774,172,857,180,173,776,789,502,503,813,174,812,78,175,892,176,
825,896,177,826,827,178,179,830,831,905,906,154,80,895,81,82,899,901,37,834,840,
904,311,841,846,845,852,792,431,84,85,77,850,853,855,856,86,311,860,175,862,793,
922,865,87,868,871,178,179,872,888,903,907,911,908,909,916,918,327,923,919,950,
927,962,969,931,968,970,972,973,77,974,431,78,984,951,952,987,1005,954,955,989,
957,949,999,670,1000,1001,154,80,1003,81,82,1006,966,37,692,960,686,228,780,701,
434,963,83,141,84,85,428,967,700,109,78,86,126,736,964,311,651,735,732,87,761,
811,555,383,520,154,80,528,81,82,128,0,37,979,0,0,980,0,981,982,0,83,0,84,85,0,0,
502,503,0,86,0,0,0,0,0,0,0,87,0,0,0,0,996,997,998,0,0,431,0,0,0,0,0,311,748,0,0,
0,0,0,431,311,0,755,0,0,0,0,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
709,710,21,158,159,252,0,253,254,255,256,257,258,259,260,22,77,23,160,261,0,0,
161,24,262,0,0,0,0,162,163,164,165,166,27,28,29,263,167,0,264,168,0,0,0,0,32,33,
265,0,169,170,0,0,77,0,0,431,0,0,0,0,0,0,0,0,0,0,0,0,0,431,0,266,80,171,81,82,35,
36,37,38,39,40,267,132,- 296,0,0,172,0,42,232,0,0,0,78,0,174,864,0,175,311,176,0,
0,177,0,760,178,179,154,80,0,81,82,0,0,37,0,311,0,0,0,0,0,0,83,0,84,85,0,0,502,
503,0,86,0,0,0,0,0,0,0,87,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,431,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,311,0,311,311,311,311,311,311,311,311,311,311,311,311,311,311,311,
311,311,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,158,159,252,0,
253,254,255,256,257,258,259,260,22,77,23,160,261,311,0,161,24,262,0,0,- 139,- 139,
162,163,164,165,166,27,28,29,263,167,0,264,168,0,0,0,0,32,33,265,0,169,170,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,311,0,0,266,80,171,81,82,35,36,37,38,39,40,267,132,
0,0,0,172,0,42,232,0,0,0,0,0,174,0,0,175,0,176,0,0,177,0,0,178,179,2,3,4,73,6,7,
8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,158,159,252,0,253,254,255,256,257,
258,259,260,22,77,23,160,261,0,0,161,24,262,0,0,0,0,162,163,164,165,166,27,28,29,
263,167,0,264,168,0,0,0,0,32,33,265,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,266,80,171,81,82,35,36,37,38,39,40,267,132,268,0,0,172,0,42,232,0,0,0,0,
0,174,0,0,175,0,176,0,0,177,0,0,178,179,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,21,158,159,252,0,253,254,255,256,257,258,259,260,22,77,23,160,261,0,
0,161,24,262,0,0,0,0,162,163,164,165,166,27,28,29,263,167,0,264,168,0,0,0,0,32,
33,265,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,266,80,171,81,82,35,
36,37,38,39,40,267,132,0,0,0,172,0,42,232,0,0,0,0,0,174,0,0,175,0,176,0,0,177,0,
0,178,179,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,158,159,0,0,0,
0,0,0,0,0,0,0,22,77,23,160,0,0,0,161,24,0,0,0,0,0,162,163,164,165,166,27,28,29,0,
167,0,0,168,0,0,0,0,32,33,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
34,80,171,81,82,35,36,37,38,39,40,608,0,0,0,0,172,0,42,232,0,0,0,0,0,174,0,0,175,
0,176,0,0,177,0,0,178,179,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,158,159,0,
0,0,0,0,0,0,0,0,0,22,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,27,28,29,
0,167,208,0,168,0,0,0,0,32,33,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,34,80,171,81,82,35,36,37,38,39,40,0,209,0,0,0,172,0,42,232,0,0,0,0,0,174,0,0,
175,0,176,0,0,177,0,0,178,179,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,158,
159,0,0,0,0,0,0,0,0,0,0,22,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,27,
28,29,0,167,0,0,168,0,0,0,0,32,33,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,34,80,171,81,82,35,36,37,38,39,40,0,325,0,0,0,172,0,42,232,0,0,0,0,0,174,
0,0,175,0,176,0,0,177,0,0,178,179,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,
158,159,0,0,0,0,0,0,0,0,0,0,22,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,
166,27,28,29,0,167,0,0,168,0,0,0,0,32,33,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,34,80,171,81,82,35,36,37,38,39,40,0,0,0,0,0,172,0,42,232,0,0,0,0,0,
174,0,0,175,0,176,0,0,177,0,0,178,179,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,639,0,24,0,25,26,-
139,- 139,0,0,0,0,0,27,28,29,0,0,0,0,0,0,0,30,31,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,34,0,0,0,0,35,36,37,38,39,40,0,0,- 11,0,0,41,0,42,43,0,0,
0,0,0,44,45,46,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,
0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,- 139,- 139,0,0,0,0,0,27,28,29,
0,0,0,0,0,0,0,30,31,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,0,
0,0,0,35,36,37,38,39,40,0,0,- 11,0,0,41,0,42,43,0,0,0,0,0,44,45,46,- 11,1,0,2,3,4,
5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,
23,0,0,0,0,0,24,0,25,26,- 139,- 139,0,0,0,0,0,27,28,29,0,0,0,0,0,0,0,30,31,32,33,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,0,0,0,0,35,36,37,38,39,40,0,0,
0,0,0,41,0,42,43,0,0,0,0,0,44,45,46,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,- 139,- 139,0,
0,0,0,0,27,28,29,0,0,0,0,0,0,0,30,31,32,33,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,0,0,34,0,0,0,0,35,36,37,38,39,40,22,0,- 11,0,0,41,0,42,43,0,0,
0,0,0,44,45,46,0,0,27,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,0,77,0,0,0,0,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,284,0,0,34,0,0,0,0,35,36,37,38,39,40,286,22,0,0,0,412,
- 203,42,43,78,- 203,0,395,0,44,45,46,0,0,0,27,28,29,0,79,80,0,81,82,0,0,37,32,33,
0,0,0,0,0,0,83,0,84,85,0,0,0,0,0,86,0,0,0,0,0,0,284,87,0,0,0,0,0,0,35,36,0,38,39,
40,286,0,0,0,0,394,- 203,42,43,0,- 203,77,395,0,44,45,46,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,78,0,0,0,0,- 139,- 139,
0,0,0,0,0,27,28,29,154,80,0,81,82,0,0,37,0,32,33,0,0,0,0,0,83,0,84,85,0,0,0,0,0,
86,0,0,0,0,0,0,0,87,0,0,34,0,0,0,0,35,36,37,38,39,40,0,0,0,0,0,435,0,42,43,0,0,0,
0,0,44,45,46,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,
0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,0,0,- 139,- 139,0,0,0,0,0,27,28,29,0,0,0,0,0,
0,0,0,0,32,33,0,0,0,0,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,
0,0,0,35,36,0,38,39,40,0,132,22,0,241,0,0,42,43,0,24,0,0,0,- 139,- 139,0,0,0,0,0,
27,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,0,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,21,0,0,0,0,0,35,36,0,38,39,40,0,22,0,23,0,0,0,42,43,24,0,0,0,0,0,0,
0,0,0,0,27,28,29,0,0,0,0,0,0,0,0,0,32,33,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,35,36,0,38,39,40,0,132,0,0,0,0,0,42,43,0,0,0,27,
28,29,0,0,0,0,0,0,0,0,0,32,33,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,
0,0,284,0,0,285,0,0,0,22,35,36,0,38,39,40,286,0,0,0,0,0,- 203,42,43,0,- 203,0,27,
28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,284,0,
0,0,0,0,0,0,35,36,0,38,39,40,286,0,0,0,0,0,- 203,42,43,0,- 203,158,159,252,0,253,
254,255,256,257,258,259,260,0,77,0,160,261,0,0,161,0,262,0,0,0,0,162,163,164,165,
166,0,0,0,263,167,0,264,168,0,0,0,0,0,0,265,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,266,80,171,81,82,0,0,37,0,0,0,267,132,0,0,0,172,0,0,173,0,0,0,0,
0,174,0,0,175,0,176,158,159,177,0,0,178,179,500,0,0,0,0,0,77,0,160,0,0,0,161,0,0,
0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,312,501,0,0,172,
0,0,173,0,0,502,503,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,
161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,312,917,
0,0,172,0,0,173,0,0,502,503,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,
160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,
0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,
0,312,0,0,0,172,0,0,173,0,0,502,503,0,174,158,159,175,0,176,0,0,177,0,0,178,179,
0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,
0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,
0,0,0,0,0,0,0,0,172,0,0,173,0,0,0,0,293,174,158,159,175,0,176,0,0,177,0,0,178,
179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,
0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,
0,37,0,0,0,0,312,0,0,0,172,0,0,173,0,0,0,0,0,174,158,159,175,0,176,0,0,177,0,0,
178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,
0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,
82,0,0,37,0,0,0,454,0,0,0,0,172,0,0,173,0,0,0,0,0,174,158,159,175,0,176,0,0,177,
0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,
168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,
81,82,0,0,37,0,0,0,0,0,0,0,0,172,545,0,173,0,0,0,0,0,174,158,159,175,0,176,0,0,
177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,
0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,
171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,0,0,173,0,0,0,0,571,174,158,159,175,0,176,0,
0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,
167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,
80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,614,0,173,0,0,0,0,0,174,158,159,175,0,
176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,
0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,0,0,173,0,0,0,0,685,174,158,159,175,
0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,
0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,0,0,173,0,0,0,0,699,174,158,159,
175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,
166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,714,0,0,0,0,172,0,0,173,0,0,0,0,0,174,158,
159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,
165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,716,0,0,0,0,172,0,0,173,0,0,0,0,0,174,
158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,
164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,816,0,173,0,0,0,0,0,
174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,
163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,819,0,173,0,0,0,
0,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,
162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,822,0,0,0,0,172,0,0,173,0,
0,0,0,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,
0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,894,0,173,
0,0,0,0,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,
0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,898,0,
173,0,0,0,0,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,
0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,900,
0,173,0,0,0,0,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,
0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,
959,0,173,0,0,0,0,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,
161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,
0,0,0,0,0,158,159,0,0,0,0,0,0,0,0,0,0,0,77,0,0,34,80,171,81,82,0,0,37,0,0,162,
163,164,165,166,0,172,0,0,173,0,0,168,0,0,174,0,0,175,0,176,169,170,177,0,0,178,
179,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,312,158,159,0,
172,0,0,173,0,0,0,0,0,174,77,0,175,0,176,0,0,177,0,0,178,179,0,162,163,164,165,
166,0,0,0,0,0,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,661,158,159,0,172,0,0,173,0,0,0,0,0,174,
77,0,175,0,176,0,0,177,0,0,178,179,0,162,163,164,165,166,0,0,0,0,0,0,0,168,0,0,0,
0,0,0,0,0,169,170,0,0,0,0,0,158,159,0,0,0,0,0,0,0,0,0,0,0,77,0,0,34,80,171,81,82,
0,0,37,0,0,162,163,164,165,166,0,307,0,0,173,0,0,168,0,0,174,0,0,175,0,176,169,
170,177,0,0,178,179,158,159,0,0,0,0,0,0,0,0,0,0,0,77,0,0,34,80,171,81,82,0,0,37,
0,0,162,163,164,165,166,0,322,0,0,173,0,0,168,0,0,174,0,0,175,0,176,169,170,177,
0,0,178,179,158,159,0,0,0,0,0,0,0,0,0,0,0,77,0,0,34,80,171,81,82,0,0,37,0,0,162,
163,164,165,166,0,172,0,0,173,0,0,168,0,0,174,0,0,175,0,176,169,170,177,0,0,178,
179,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,863,0,
0,173,0,0,0,0,0,174,0,0,175,0,176,0,0,177,0,0,178,179,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,22,0,0,0,27,28,29,0,0,208,0,0,- 139,- 139,0,22,32,33,0,27,28,29,0,0,
0,0,0,0,0,0,0,32,33,0,27,28,29,0,0,0,0,0,0,0,0,0,32,33,35,36,0,38,39,40,0,209,0,
0,0,0,0,42,43,35,36,0,38,39,40,0,0,0,0,0,0,0,42,43,35,36,0,38,39,40,0,0,- 107,0,0,
0,0,42,43,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,22,7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,0,0,0,27,28,29,0,0,0,0,0,0,
0,0,22,32,33,0,27,28,29,0,0,0,0,0,0,0,0,0,32,33,0,27,28,29,0,0,0,0,0,0,0,0,0,32,
33,35,36,0,38,39,40,0,633,- 108,0,0,0,0,42,43,35,36,0,38,39,40,0,0,0,0,0,0,0,42,
43,35,36,0,38,39,40,0,0,0,0,0,0,0,42,43,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0,0,- 139,- 139,0,0,0,0,0,27,28,
29,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,35,0,0,0,39,40,0,0,0,0,0,0,0,42,43};static short Cyc_yycheck[6422]={0,106,
334,120,146,172,369,340,341,336,161,161,64,61,137,137,339,160,366,401,474,21,370,
64,137,269,270,436,272,346,347,81,0,131,337,241,75,81,211,550,21,41,348,349,97,
153,95,145,48,342,343,344,345,21,54,55,56,48,58,17,5,6,76,77,64,65,115,95,120,
118,109,0,122,54,55,56,415,58,122,120,48,325,134,64,229,124,54,55,56,93,58,132,
21,134,503,458,64,479,98,99,95,115,441,17,18,19,101,98,99,563,124,209,112,113,
114,115,128,121,23,48,120,112,113,114,115,54,55,56,75,58,98,99,132,109,134,64,
382,95,23,120,307,141,309,119,112,113,114,115,148,396,661,132,120,134,502,322,
168,148,174,302,176,177,419,108,132,115,134,289,415,98,99,109,75,78,112,76,77,
124,125,195,148,229,87,112,113,114,115,61,286,598,242,120,300,78,206,108,95,95,
115,247,95,242,87,132,102,134,109,95,115,107,109,211,125,106,115,472,389,390,391,
148,115,124,125,124,481,106,100,115,269,270,111,272,125,107,106,107,216,217,95,
95,220,604,242,211,137,102,102,95,96,97,106,156,100,95,111,111,111,719,94,107,
102,242,95,118,120,121,122,412,100,269,270,156,272,106,242,115,293,117,95,112,41,
113,213,116,325,102,100,106,269,270,435,272,53,436,111,643,296,114,229,113,506,
269,270,401,272,0,106,100,112,71,72,73,292,242,120,200,421,210,298,212,113,125,
207,129,130,325,788,22,23,107,25,95,112,111,350,351,352,101,102,103,269,270,103,
272,325,125,41,111,515,111,467,111,245,48,118,117,118,325,53,95,503,100,374,115,
412,109,61,415,67,108,106,66,116,109,135,123,474,125,95,115,75,479,83,78,101,102,
103,435,109,389,390,391,87,441,111,116,325,100,398,111,862,482,648,98,99,108,868,
549,103,450,95,96,412,94,414,467,101,112,113,114,115,389,390,391,107,425,467,427,
674,123,442,443,109,575,425,435,427,111,583,116,135,123,648,125,118,587,120,121,
122,594,450,574,574,148,115,598,107,425,153,427,97,156,105,586,586,563,928,467,
111,450,487,172,115,586,379,118,110,382,735,240,120,107,450,117,118,109,467,95,
569,129,130,491,116,111,379,106,425,382,427,467,491,394,518,625,205,200,657,657,
106,107,271,633,207,759,213,109,978,110,599,412,488,450,665,491,49,50,419,909,
421,111,229,629,994,112,229,583,384,869,467,745,106,255,875,876,871,240,594,261,
247,597,105,874,247,759,571,250,111,405,253,260,115,109,491,118,881,882,109,95,
812,872,873,623,109,573,102,112,271,883,884,472,64,65,623,111,877,878,879,880,
481,95,590,95,120,121,122,101,102,103,102,579,108,105,683,849,95,300,307,111,78,
79,101,102,103,110,315,696,120,121,122,320,111,322,111,623,868,792,974,118,111,
120,121,122,95,111,335,111,484,111,101,102,103,641,623,849,111,108,105,552,729,
111,641,556,111,734,95,623,115,95,96,118,101,102,103,101,681,111,107,552,685,637,
371,556,106,641,106,109,109,109,845,112,106,574,699,109,379,74,75,382,668,737,
583,126,704,586,623,394,863,80,865,106,95,128,109,127,684,107,101,102,103,120,
121,122,641,412,836,95,111,412,698,109,415,116,112,108,109,120,121,122,109,95,
425,112,427,107,978,111,435,112,109,109,435,112,112,444,445,108,441,448,95,450,
464,768,108,109,101,102,103,111,105,108,95,462,779,108,111,139,101,102,103,674,
108,812,111,120,121,122,111,793,626,109,107,158,112,160,161,120,121,122,106,674,
111,109,169,170,111,811,491,174,109,176,177,112,969,109,709,106,112,95,109,106,
703,909,812,106,919,106,109,109,195,109,112,115,112,106,709,109,109,868,112,910,
109,812,109,112,977,112,106,106,109,109,109,112,735,111,812,82,83,84,85,86,87,88,
89,90,91,109,116,552,112,38,109,556,108,112,95,913,110,241,868,885,886,887,112,
110,109,109,109,112,112,112,95,96,97,812,583,22,23,868,583,389,390,391,5,6,116,
594,108,109,597,594,868,80,597,617,112,600,125,608,609,127,128,927,119,92,78,79,
95,96,105,98,99,74,75,102,108,302,108,109,858,859,108,124,111,124,113,114,81,978,
312,868,111,120,316,317,318,319,641,112,115,128,112,109,653,112,328,656,108,334,
332,336,337,338,339,340,341,342,343,344,345,346,347,348,349,350,351,352,109,111,
978,674,112,109,106,109,111,116,107,30,106,42,364,110,706,100,95,369,100,978,712,
111,125,112,112,25,26,95,109,109,100,109,978,109,713,714,709,716,38,718,40,112,
109,395,44,108,112,109,919,108,119,51,52,53,54,55,112,109,112,112,60,112,741,63,
737,112,107,25,26,421,106,978,72,73,111,107,95,112,124,119,38,436,124,106,108,
124,115,107,25,107,112,119,119,119,107,95,96,97,98,99,112,112,102,112,112,112,
458,107,108,112,112,111,785,792,114,112,112,117,118,108,120,116,80,123,816,125,
106,819,128,111,95,131,132,106,95,827,828,95,96,818,98,99,821,822,102,100,112,
826,503,112,109,111,96,111,506,113,114,38,112,112,108,108,120,518,28,123,107,125,
860,111,128,116,80,131,132,126,80,108,100,108,110,109,25,109,863,108,111,106,112,
124,111,870,112,96,108,112,38,109,550,80,95,893,894,112,0,897,898,112,900,888,
112,92,112,112,95,96,112,98,99,0,913,102,582,902,576,105,703,597,245,908,111,66,
113,114,240,914,588,45,80,120,913,637,909,598,507,633,629,128,674,709,92,207,604,
95,96,338,98,99,61,- 1,102,953,- 1,- 1,956,- 1,958,959,- 1,111,- 1,113,114,- 1,- 1,117,
118,- 1,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,- 1,- 1,- 1,983,984,985,- 1,- 1,648,- 1,- 1,- 1,-
1,- 1,657,655,- 1,- 1,- 1,- 1,- 1,661,665,- 1,664,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,
41,- 1,- 1,44,45,46,- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,58,59,60,- 1,62,63,- 1,- 1,- 1,- 1,
68,69,70,- 1,72,73,- 1,- 1,38,- 1,- 1,745,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,759,
- 1,95,96,97,98,99,100,101,102,103,104,105,106,107,108,- 1,- 1,111,- 1,113,114,- 1,- 1,
- 1,80,- 1,120,790,- 1,123,793,125,- 1,- 1,128,- 1,92,131,132,95,96,- 1,98,99,- 1,- 1,102,
- 1,811,- 1,- 1,- 1,- 1,- 1,- 1,111,- 1,113,114,- 1,- 1,117,118,- 1,120,- 1,- 1,- 1,- 1,- 1,- 1,-
1,128,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,849,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,869,- 1,871,872,873,874,875,876,877,878,879,880,881,882,
883,884,885,886,887,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,
25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,927,- 1,44,45,46,- 1,- 1,49,50,
51,52,53,54,55,56,57,58,59,60,- 1,62,63,- 1,- 1,- 1,- 1,68,69,70,- 1,72,73,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,977,- 1,- 1,95,96,97,98,99,100,101,102,
103,104,105,106,107,- 1,- 1,- 1,111,- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,123,- 1,125,
- 1,- 1,128,- 1,- 1,131,132,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,- 1,- 1,
51,52,53,54,55,56,57,58,59,60,- 1,62,63,- 1,- 1,- 1,- 1,68,69,70,- 1,72,73,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,100,101,102,
103,104,105,106,107,108,- 1,- 1,111,- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,123,- 1,125,
- 1,- 1,128,- 1,- 1,131,132,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,- 1,- 1,
51,52,53,54,55,56,57,58,59,60,- 1,62,63,- 1,- 1,- 1,- 1,68,69,70,- 1,72,73,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,100,101,102,
103,104,105,106,107,- 1,- 1,- 1,111,- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,123,- 1,125,
- 1,- 1,128,- 1,- 1,131,132,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,- 1,- 1,
51,52,53,54,55,56,57,58,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,73,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,100,101,102,
103,104,105,106,- 1,- 1,- 1,- 1,111,- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,123,- 1,125,-
1,- 1,128,- 1,- 1,131,132,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,
55,56,57,58,- 1,60,61,- 1,63,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,100,101,102,103,104,105,- 1,
107,- 1,- 1,- 1,111,- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,123,- 1,125,- 1,- 1,128,- 1,- 1,
131,132,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,58,- 1,
60,- 1,- 1,63,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,100,101,102,103,104,105,- 1,107,- 1,- 1,- 1,
111,- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,58,- 1,60,- 1,- 1,63,
- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,95,96,97,98,99,100,101,102,103,104,105,- 1,- 1,- 1,- 1,- 1,111,- 1,113,114,
- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,0,1,- 1,3,4,5,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
37,- 1,39,- 1,- 1,- 1,43,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,66,67,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,95,- 1,- 1,- 1,- 1,100,101,102,103,104,105,- 1,- 1,108,- 1,- 1,111,- 1,113,
114,- 1,- 1,- 1,- 1,- 1,120,121,122,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,
- 1,47,48,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,- 1,- 1,- 1,- 1,
100,101,102,103,104,105,- 1,- 1,108,- 1,- 1,111,- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,121,
122,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,
- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,- 1,- 1,- 1,- 1,100,101,102,103,104,105,- 1,
- 1,- 1,- 1,- 1,111,- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,121,122,1,- 1,3,4,5,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,
39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
66,67,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,-
1,- 1,95,- 1,- 1,- 1,- 1,100,101,102,103,104,105,37,- 1,108,- 1,- 1,111,- 1,113,114,- 1,- 1,
- 1,- 1,- 1,120,121,122,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,38,
- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,92,- 1,- 1,95,- 1,- 1,- 1,- 1,100,
101,102,103,104,105,106,37,- 1,- 1,- 1,111,112,113,114,80,116,- 1,118,- 1,120,121,122,
- 1,- 1,- 1,56,57,58,- 1,95,96,- 1,98,99,- 1,- 1,102,68,69,- 1,- 1,- 1,- 1,- 1,- 1,111,- 1,113,
114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,- 1,92,128,- 1,- 1,- 1,- 1,- 1,- 1,100,101,- 1,103,
104,105,106,- 1,- 1,- 1,- 1,111,112,113,114,- 1,116,38,118,- 1,120,121,122,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,
- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,95,96,- 1,98,99,- 1,- 1,
102,- 1,68,69,- 1,- 1,- 1,- 1,- 1,111,- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,128,- 1,- 1,95,- 1,- 1,- 1,- 1,100,101,102,103,104,105,- 1,- 1,- 1,- 1,- 1,111,- 1,113,
114,- 1,- 1,- 1,- 1,- 1,120,121,122,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,- 1,- 1,
49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,3,4,5,
6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,100,101,- 1,103,
104,105,- 1,107,37,- 1,110,- 1,- 1,113,114,- 1,45,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,- 1,56,57,
58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,100,101,- 1,103,104,105,- 1,37,- 1,39,- 1,- 1,
- 1,113,114,45,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
68,69,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,37,100,101,- 1,103,104,105,- 1,107,- 1,- 1,- 1,- 1,- 1,113,114,- 1,- 1,- 1,56,
57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,8,9,10,11,12,13,14,15,16,17,18,19,20,21,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,- 1,- 1,95,- 1,- 1,- 1,37,100,101,- 1,103,104,105,106,- 1,- 1,
- 1,- 1,- 1,112,113,114,- 1,116,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,100,101,- 1,103,104,105,106,- 1,- 1,- 1,- 1,- 1,112,113,114,- 1,116,25,26,27,- 1,29,
30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,- 1,44,- 1,46,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,
- 1,- 1,59,60,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,70,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,106,107,- 1,- 1,
- 1,111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,123,- 1,125,25,26,128,- 1,- 1,131,132,32,
- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,
60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,107,108,- 1,- 1,111,-
1,- 1,114,- 1,- 1,117,118,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,
40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,107,108,- 1,- 1,111,- 1,- 1,114,- 1,- 1,117,
118,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,-
1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,
73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,
- 1,- 1,102,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,111,- 1,- 1,114,- 1,- 1,117,118,- 1,120,25,26,123,
- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,
53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,111,- 1,- 1,114,- 1,- 1,- 1,- 1,119,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,
131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,
- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,111,- 1,- 1,
114,- 1,- 1,- 1,- 1,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,-
1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,
96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,120,
25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,
- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,114,- 1,- 1,- 1,- 1,- 1,120,25,26,123,- 1,125,- 1,- 1,
128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,
- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
111,- 1,- 1,114,- 1,- 1,- 1,- 1,119,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,
- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,114,- 1,- 1,- 1,-
1,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,
- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,
- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,- 1,- 1,114,- 1,- 1,- 1,- 1,119,120,25,26,123,- 1,
125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,
54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,111,- 1,- 1,114,- 1,- 1,- 1,- 1,119,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,
131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,
- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,111,- 1,- 1,
114,- 1,- 1,- 1,- 1,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,-
1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,
96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,120,
25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,
- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,114,- 1,- 1,- 1,- 1,- 1,120,25,26,123,- 1,125,- 1,- 1,
128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,
- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
111,112,- 1,114,- 1,- 1,- 1,- 1,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,
- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,111,- 1,- 1,114,- 1,- 1,- 1,-
1,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,
- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,
- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,114,- 1,- 1,- 1,- 1,- 1,120,25,26,123,- 1,
125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,
54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,111,112,- 1,114,- 1,- 1,- 1,- 1,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,
131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,
- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,
114,- 1,- 1,- 1,- 1,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,-
1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,
96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,114,- 1,- 1,- 1,- 1,- 1,120,
25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,
- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,
- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,95,96,97,98,99,- 1,- 1,102,-
1,- 1,51,52,53,54,55,- 1,111,- 1,- 1,114,- 1,- 1,63,- 1,- 1,120,- 1,- 1,123,- 1,125,72,73,
128,- 1,- 1,131,132,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,
- 1,- 1,102,- 1,- 1,- 1,- 1,107,25,26,- 1,111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,120,38,- 1,123,- 1,
125,- 1,- 1,128,- 1,- 1,131,132,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,107,25,26,- 1,111,- 1,- 1,114,- 1,- 1,- 1,- 1,-
1,120,38,- 1,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,38,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,51,52,53,54,55,- 1,111,- 1,- 1,
114,- 1,- 1,63,- 1,- 1,120,- 1,- 1,123,- 1,125,72,73,128,- 1,- 1,131,132,25,26,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,51,52,53,54,55,- 1,
111,- 1,- 1,114,- 1,- 1,63,- 1,- 1,120,- 1,- 1,123,- 1,125,72,73,128,- 1,- 1,131,132,25,26,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,51,52,
53,54,55,- 1,111,- 1,- 1,114,- 1,- 1,63,- 1,- 1,120,- 1,- 1,123,- 1,125,72,73,128,- 1,- 1,
131,132,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,123,- 1,125,- 1,- 1,
128,- 1,- 1,131,132,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,37,8,9,10,11,12,13,14,15,16,17,18,19,20,21,37,- 1,- 1,- 1,56,
57,58,- 1,- 1,61,- 1,- 1,49,50,- 1,37,68,69,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,
69,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,100,101,- 1,103,104,105,- 1,107,- 1,
- 1,- 1,- 1,- 1,113,114,100,101,- 1,103,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,100,101,
- 1,103,104,105,- 1,- 1,108,- 1,- 1,- 1,- 1,113,114,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,37,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,37,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,68,69,- 1,56,57,58,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,100,
101,- 1,103,104,105,- 1,92,108,- 1,- 1,- 1,- 1,113,114,100,101,- 1,103,104,105,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,113,114,100,101,- 1,103,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,- 1,- 1,- 1,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114};char
Cyc_Yyimpossible[17]="\000\000\000\000Yyimpossible\000";char Cyc_Yystack_overflow[
21]="\000\000\000\000Yystack_overflow\000";void Cyc_yyerror(struct _dynforward_ptr);
int Cyc_yylex(struct Cyc_Lexing_lexbuf*);static int Cyc_yychar=(int)'\000';union Cyc_YYSTYPE_union
Cyc_yylval=(union Cyc_YYSTYPE_union)((struct Cyc_YYINITIALSVAL_struct){57});static
int Cyc_yynerrs=0;struct _tuple18{struct Cyc_Position_Segment*f1;struct _tuple1*f2;
int f3;};struct _tuple19{struct _dynforward_ptr f1;void*f2;};static char _tmp41F[8]="stdcall";
static char _tmp420[6]="cdecl";static char _tmp421[9]="fastcall";static char _tmp422[9]="noreturn";
static char _tmp423[6]="const";static char _tmp424[8]="aligned";static char _tmp425[7]="packed";
static char _tmp426[7]="shared";static char _tmp427[7]="unused";static char _tmp428[5]="weak";
static char _tmp429[10]="dllimport";static char _tmp42A[10]="dllexport";static char
_tmp42B[23]="no_instrument_function";static char _tmp42C[12]="constructor";static
char _tmp42D[11]="destructor";static char _tmp42E[22]="no_check_memory_usage";
static char _tmp42F[5]="pure";struct _tuple20{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf);static void _tmpA10(
unsigned int*_tmpA0F,unsigned int*_tmpA0E,short**_tmpA0D){for(*_tmpA0F=0;*
_tmpA0F < *_tmpA0E;(*_tmpA0F)++){(*_tmpA0D)[*_tmpA0F]=(short)0;}}static void
_tmpA15(unsigned int*_tmpA14,unsigned int*_tmpA13,union Cyc_YYSTYPE_union**
_tmpA12){for(*_tmpA14=0;*_tmpA14 < *_tmpA13;(*_tmpA14)++){(*_tmpA12)[*_tmpA14]=
Cyc_yylval;}}static void _tmpA1A(unsigned int*_tmpA19,unsigned int*_tmpA18,struct
Cyc_Yyltype**_tmpA17){for(*_tmpA19=0;*_tmpA19 < *_tmpA18;(*_tmpA19)++){(*_tmpA17)[*
_tmpA19]=Cyc_yynewloc();}}static void _tmp135A(unsigned int*_tmp1359,unsigned int*
_tmp1358,char**_tmp1356){for(*_tmp1359=0;*_tmp1359 < *_tmp1358;(*_tmp1359)++){(*
_tmp1356)[*_tmp1359]='\000';}}int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf){
struct _RegionHandle _tmp37F=_new_region("yyregion");struct _RegionHandle*yyregion=&
_tmp37F;_push_region(yyregion);{int yystate;int yyn=0;int yyerrstatus;int yychar1=0;
int yyssp_offset;unsigned int _tmpA0F;unsigned int _tmpA0E;short*_tmpA0D;
unsigned int _tmpA0C;short*yyss=(short*)((_tmpA0C=10000,((_tmpA0D=(short*)
_region_malloc(yyregion,_check_times(sizeof(short),_tmpA0C)),((((_tmpA0E=_tmpA0C,
_tmpA10(& _tmpA0F,& _tmpA0E,& _tmpA0D))),_tmpA0D))))));int yyvsp_offset;unsigned int
_tmpA14;unsigned int _tmpA13;union Cyc_YYSTYPE_union*_tmpA12;unsigned int _tmpA11;
union Cyc_YYSTYPE_union*yyvs=(union Cyc_YYSTYPE_union*)((_tmpA11=10000,((_tmpA12=(
union Cyc_YYSTYPE_union*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE_union),
_tmpA11)),((((_tmpA13=_tmpA11,_tmpA15(& _tmpA14,& _tmpA13,& _tmpA12))),_tmpA12))))));
int yylsp_offset;unsigned int _tmpA19;unsigned int _tmpA18;struct Cyc_Yyltype*
_tmpA17;unsigned int _tmpA16;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)((_tmpA16=
10000,((_tmpA17=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(
struct Cyc_Yyltype),_tmpA16)),((((_tmpA18=_tmpA16,_tmpA1A(& _tmpA19,& _tmpA18,&
_tmpA17))),_tmpA17))))));int yystacksize=10000;union Cyc_YYSTYPE_union yyval=Cyc_yylval;
int yylen;yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;
yyvsp_offset=0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(
10000,++ yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(
yystacksize >= 10000){{const char*_tmpA1B;Cyc_yyerror(((_tmpA1B="parser stack overflow",
_tag_dynforward(_tmpA1B,sizeof(char),_get_zero_arr_size_char(_tmpA1B,22)))));}(
int)_throw((void*)Cyc_Yystack_overflow);}}goto yybackup;yybackup: yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(1007,yystate)];if(yyn == - 32768)goto yydefault;if(
Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex(yylex_buf);if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=
0;}else{yychar1=Cyc_yychar > 0  && Cyc_yychar <= 360?(int)Cyc_yytranslate[
_check_known_subscript_notnull(361,Cyc_yychar)]: 275;}yyn +=yychar1;if((yyn < 0  || 
yyn > 6421) || Cyc_yycheck[_check_known_subscript_notnull(6422,yyn)]!= yychar1)
goto yydefault;yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6422,yyn)];if(
yyn < 0){if(yyn == - 32768)goto yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto
yyerrlab;}if(yyn == 1006){int _tmp381=0;_npop_handler(0);return _tmp381;}if(Cyc_yychar
!= 0)Cyc_yychar=- 2;yyvs[_check_known_subscript_notnull(10000,++ yyvsp_offset)]=
Cyc_yylval;yyls[_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;
if(yyerrstatus != 0)-- yyerrstatus;yystate=yyn;goto yynewstate;yydefault: yyn=(int)
Cyc_yydefact[_check_known_subscript_notnull(1007,yystate)];if(yyn == 0)goto
yyerrlab;yyreduce: yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(501,yyn)];
if(yylen > 0)yyval=yyvs[_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- 
yylen)];switch(yyn){case 1: _LL1E8: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];Cyc_Parse_parse_result=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);break;case 2: _LL1E9: {struct
Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons _tmp382;_push_handler(&
_tmp382);{int _tmp384=0;if(setjmp(_tmp382.handler))_tmp384=1;if(!_tmp384){x=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);;_pop_handler();}
else{void*_tmp383=(void*)_exn_thrown;void*_tmp386=_tmp383;_LL1EC: if(*((void**)
_tmp386)!= Cyc_Core_Failure)goto _LL1EE;_LL1ED: x=0;goto _LL1EB;_LL1EE:;_LL1EF:(
void)_throw(_tmp386);_LL1EB:;}}}{struct _handler_cons _tmp387;_push_handler(&
_tmp387);{int _tmp389=0;if(setjmp(_tmp387.handler))_tmp389=1;if(!_tmp389){y=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{
void*_tmp388=(void*)_exn_thrown;void*_tmp38B=_tmp388;_LL1F1: if(*((void**)_tmp38B)
!= Cyc_Core_Failure)goto _LL1F3;_LL1F2: y=0;goto _LL1F0;_LL1F3:;_LL1F4:(void)_throw(
_tmp38B);_LL1F0:;}}}{union Cyc_YYSTYPE_union _tmpA1C;yyval=(union Cyc_YYSTYPE_union)(((
_tmpA1C.YY18).tag=22,(((_tmpA1C.YY18).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y),_tmpA1C))));}break;}case 3:
_LL1EA:{struct Cyc_List_List*_tmpA2B;struct Cyc_Absyn_Using_d_struct*_tmpA2A;
struct Cyc_Absyn_Using_d_struct _tmpA29;struct Cyc_Absyn_Decl*_tmpA28;union Cyc_YYSTYPE_union
_tmpA27;yyval=(union Cyc_YYSTYPE_union)(((_tmpA27.YY18).tag=22,(((_tmpA27.YY18).f1=((
_tmpA2B=_cycalloc(sizeof(*_tmpA2B)),((_tmpA2B->hd=((_tmpA28=_cycalloc(sizeof(*
_tmpA28)),((_tmpA28->r=(void*)((void*)((_tmpA2A=_cycalloc(sizeof(*_tmpA2A)),((
_tmpA2A[0]=((_tmpA29.tag=9,((_tmpA29.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpA29.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA29)))))),_tmpA2A))))),((
_tmpA28->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpA28)))))),((_tmpA2B->tl=0,_tmpA2B)))))),_tmpA27))));}
Cyc_Lex_leave_using();break;case 4: _LL1F5:{struct Cyc_List_List*_tmpA3A;struct Cyc_Absyn_Using_d_struct*
_tmpA39;struct Cyc_Absyn_Using_d_struct _tmpA38;struct Cyc_Absyn_Decl*_tmpA37;union
Cyc_YYSTYPE_union _tmpA36;yyval=(union Cyc_YYSTYPE_union)(((_tmpA36.YY18).tag=22,(((
_tmpA36.YY18).f1=((_tmpA3A=_cycalloc(sizeof(*_tmpA3A)),((_tmpA3A->hd=((_tmpA37=
_cycalloc(sizeof(*_tmpA37)),((_tmpA37->r=(void*)((void*)((_tmpA39=_cycalloc(
sizeof(*_tmpA39)),((_tmpA39[0]=((_tmpA38.tag=9,((_tmpA38.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmpA38.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpA38)))))),
_tmpA39))))),((_tmpA37->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA37)))))),((
_tmpA3A->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA3A)))))),_tmpA36))));}break;case 5: _LL1F6:{struct Cyc_List_List*
_tmpA4F;struct Cyc_Absyn_Namespace_d_struct*_tmpA4E;struct _dynforward_ptr*_tmpA4D;
struct Cyc_Absyn_Namespace_d_struct _tmpA4C;struct Cyc_Absyn_Decl*_tmpA4B;union Cyc_YYSTYPE_union
_tmpA4A;yyval=(union Cyc_YYSTYPE_union)(((_tmpA4A.YY18).tag=22,(((_tmpA4A.YY18).f1=((
_tmpA4F=_cycalloc(sizeof(*_tmpA4F)),((_tmpA4F->hd=((_tmpA4B=_cycalloc(sizeof(*
_tmpA4B)),((_tmpA4B->r=(void*)((void*)((_tmpA4E=_cycalloc(sizeof(*_tmpA4E)),((
_tmpA4E[0]=((_tmpA4C.tag=8,((_tmpA4C.f1=((_tmpA4D=_cycalloc(sizeof(*_tmpA4D)),((
_tmpA4D[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),_tmpA4D)))),((_tmpA4C.f2=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA4C)))))),_tmpA4E))))),((
_tmpA4B->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpA4B)))))),((_tmpA4F->tl=0,_tmpA4F)))))),_tmpA4A))));}
Cyc_Lex_leave_namespace();break;case 6: _LL1F7: {struct _dynforward_ptr nspace;
struct Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons _tmp39D;
_push_handler(& _tmp39D);{int _tmp39F=0;if(setjmp(_tmp39D.handler))_tmp39F=1;if(!
_tmp39F){nspace=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]);;_pop_handler();}else{void*_tmp39E=(void*)_exn_thrown;void*
_tmp3A1=_tmp39E;_LL1FA: if(*((void**)_tmp3A1)!= Cyc_Core_Failure)goto _LL1FC;
_LL1FB:{const char*_tmpA50;nspace=((_tmpA50="",_tag_dynforward(_tmpA50,sizeof(
char),_get_zero_arr_size_char(_tmpA50,1))));}goto _LL1F9;_LL1FC:;_LL1FD:(void)
_throw(_tmp3A1);_LL1F9:;}}}{struct _handler_cons _tmp3A3;_push_handler(& _tmp3A3);{
int _tmp3A5=0;if(setjmp(_tmp3A3.handler))_tmp3A5=1;if(!_tmp3A5){x=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);;_pop_handler();}
else{void*_tmp3A4=(void*)_exn_thrown;void*_tmp3A7=_tmp3A4;_LL1FF: if(*((void**)
_tmp3A7)!= Cyc_Core_Failure)goto _LL201;_LL200: x=0;goto _LL1FE;_LL201:;_LL202:(
void)_throw(_tmp3A7);_LL1FE:;}}}{struct _handler_cons _tmp3A8;_push_handler(&
_tmp3A8);{int _tmp3AA=0;if(setjmp(_tmp3A8.handler))_tmp3AA=1;if(!_tmp3AA){y=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{
void*_tmp3A9=(void*)_exn_thrown;void*_tmp3AC=_tmp3A9;_LL204: if(*((void**)_tmp3AC)
!= Cyc_Core_Failure)goto _LL206;_LL205: y=0;goto _LL203;_LL206:;_LL207:(void)_throw(
_tmp3AC);_LL203:;}}}{struct Cyc_List_List*_tmpA65;struct Cyc_Absyn_Namespace_d_struct*
_tmpA64;struct _dynforward_ptr*_tmpA63;struct Cyc_Absyn_Namespace_d_struct _tmpA62;
struct Cyc_Absyn_Decl*_tmpA61;union Cyc_YYSTYPE_union _tmpA60;yyval=(union Cyc_YYSTYPE_union)(((
_tmpA60.YY18).tag=22,(((_tmpA60.YY18).f1=((_tmpA65=_cycalloc(sizeof(*_tmpA65)),((
_tmpA65->hd=((_tmpA61=_cycalloc(sizeof(*_tmpA61)),((_tmpA61->r=(void*)((void*)((
_tmpA64=_cycalloc(sizeof(*_tmpA64)),((_tmpA64[0]=((_tmpA62.tag=8,((_tmpA62.f1=((
_tmpA63=_cycalloc(sizeof(*_tmpA63)),((_tmpA63[0]=nspace,_tmpA63)))),((_tmpA62.f2=
x,_tmpA62)))))),_tmpA64))))),((_tmpA61->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA61)))))),((
_tmpA65->tl=y,_tmpA65)))))),_tmpA60))));}break;}case 7: _LL1F8:{const char*_tmpA66;
if(Cyc_strcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmpA66="C",
_tag_dynforward(_tmpA66,sizeof(char),_get_zero_arr_size_char(_tmpA66,2)))))== 0){
struct Cyc_List_List*_tmpA75;struct Cyc_Absyn_ExternC_d_struct*_tmpA74;struct Cyc_Absyn_ExternC_d_struct
_tmpA73;struct Cyc_Absyn_Decl*_tmpA72;union Cyc_YYSTYPE_union _tmpA71;yyval=(union
Cyc_YYSTYPE_union)(((_tmpA71.YY18).tag=22,(((_tmpA71.YY18).f1=((_tmpA75=
_cycalloc(sizeof(*_tmpA75)),((_tmpA75->hd=((_tmpA72=_cycalloc(sizeof(*_tmpA72)),((
_tmpA72->r=(void*)((void*)((_tmpA74=_cycalloc(sizeof(*_tmpA74)),((_tmpA74[0]=((
_tmpA73.tag=10,((_tmpA73.f1=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),_tmpA73)))),_tmpA74))))),((_tmpA72->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmpA72)))))),((
_tmpA75->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA75)))))),_tmpA71))));}else{{const char*_tmpA76;if(Cyc_strcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]),((_tmpA76="C include",_tag_dynforward(_tmpA76,sizeof(
char),_get_zero_arr_size_char(_tmpA76,10)))))!= 0){const char*_tmpA77;Cyc_Parse_err(((
_tmpA77="expecting \"C\" or \"C include\"",_tag_dynforward(_tmpA77,sizeof(char),
_get_zero_arr_size_char(_tmpA77,29)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));}}{struct Cyc_List_List*
_tmpA86;struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA85;struct Cyc_Absyn_ExternCinclude_d_struct
_tmpA84;struct Cyc_Absyn_Decl*_tmpA83;union Cyc_YYSTYPE_union _tmpA82;yyval=(union
Cyc_YYSTYPE_union)(((_tmpA82.YY18).tag=22,(((_tmpA82.YY18).f1=((_tmpA86=
_cycalloc(sizeof(*_tmpA86)),((_tmpA86->hd=((_tmpA83=_cycalloc(sizeof(*_tmpA83)),((
_tmpA83->r=(void*)((void*)((_tmpA85=_cycalloc(sizeof(*_tmpA85)),((_tmpA85[0]=((
_tmpA84.tag=11,((_tmpA84.f1=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),((_tmpA84.f2=0,_tmpA84)))))),_tmpA85))))),((_tmpA83->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line),_tmpA83)))))),((_tmpA86->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpA86)))))),_tmpA82))));}}}
break;case 8: _LL208:{const char*_tmpA87;if(Cyc_strcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),((_tmpA87="C include",
_tag_dynforward(_tmpA87,sizeof(char),_get_zero_arr_size_char(_tmpA87,10)))))!= 0){
const char*_tmpA88;Cyc_Parse_err(((_tmpA88="expecting \"C include\"",
_tag_dynforward(_tmpA88,sizeof(char),_get_zero_arr_size_char(_tmpA88,22)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}{struct Cyc_List_List*
exs=Cyc_yyget_YY52(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{
struct Cyc_List_List*_tmpA97;struct Cyc_Absyn_ExternCinclude_d_struct*_tmpA96;
struct Cyc_Absyn_ExternCinclude_d_struct _tmpA95;struct Cyc_Absyn_Decl*_tmpA94;
union Cyc_YYSTYPE_union _tmpA93;yyval=(union Cyc_YYSTYPE_union)(((_tmpA93.YY18).tag=
22,(((_tmpA93.YY18).f1=((_tmpA97=_cycalloc(sizeof(*_tmpA97)),((_tmpA97->hd=((
_tmpA94=_cycalloc(sizeof(*_tmpA94)),((_tmpA94->r=(void*)((void*)((_tmpA96=
_cycalloc(sizeof(*_tmpA96)),((_tmpA96[0]=((_tmpA95.tag=11,((_tmpA95.f1=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpA95.f2=exs,
_tmpA95)))))),_tmpA96))))),((_tmpA94->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmpA94)))))),((
_tmpA97->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA97)))))),_tmpA93))));}break;}case 9: _LL209:{struct Cyc_List_List*
_tmpA9D;struct Cyc_Absyn_Decl*_tmpA9C;union Cyc_YYSTYPE_union _tmpA9B;yyval=(union
Cyc_YYSTYPE_union)(((_tmpA9B.YY18).tag=22,(((_tmpA9B.YY18).f1=((_tmpA9D=
_cycalloc(sizeof(*_tmpA9D)),((_tmpA9D->hd=((_tmpA9C=_cycalloc(sizeof(*_tmpA9C)),((
_tmpA9C->r=(void*)((void*)0),((_tmpA9C->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmpA9C)))))),((
_tmpA9D->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpA9D)))))),_tmpA9B))));}break;case 10: _LL20A:{struct Cyc_List_List*
_tmpAA3;struct Cyc_Absyn_Decl*_tmpAA2;union Cyc_YYSTYPE_union _tmpAA1;yyval=(union
Cyc_YYSTYPE_union)(((_tmpAA1.YY18).tag=22,(((_tmpAA1.YY18).f1=((_tmpAA3=
_cycalloc(sizeof(*_tmpAA3)),((_tmpAA3->hd=((_tmpAA2=_cycalloc(sizeof(*_tmpAA2)),((
_tmpAA2->r=(void*)((void*)1),((_tmpAA2->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),_tmpAA2)))))),((
_tmpAA3->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpAA3)))))),_tmpAA1))));}break;case 11: _LL20B:{union Cyc_YYSTYPE_union
_tmpAA4;yyval=(union Cyc_YYSTYPE_union)(((_tmpAA4.YY18).tag=22,(((_tmpAA4.YY18).f1=
0,_tmpAA4))));}break;case 12: _LL20C: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;case 13: _LL20D:{union Cyc_YYSTYPE_union _tmpAA5;yyval=(
union Cyc_YYSTYPE_union)(((_tmpAA5.YY52).tag=56,(((_tmpAA5.YY52).f1=0,_tmpAA5))));}
break;case 14: _LL20E:{struct Cyc_List_List*_tmpAAB;struct _tuple18*_tmpAAA;union Cyc_YYSTYPE_union
_tmpAA9;yyval=(union Cyc_YYSTYPE_union)(((_tmpAA9.YY52).tag=56,(((_tmpAA9.YY52).f1=((
_tmpAAB=_cycalloc(sizeof(*_tmpAAB)),((_tmpAAB->hd=((_tmpAAA=_cycalloc(sizeof(*
_tmpAAA)),((_tmpAAA->f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((_tmpAAA->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpAAA->f3=0,_tmpAAA)))))))),((_tmpAAB->tl=0,_tmpAAB)))))),_tmpAA9))));}break;
case 15: _LL20F:{struct Cyc_List_List*_tmpAB1;struct _tuple18*_tmpAB0;union Cyc_YYSTYPE_union
_tmpAAF;yyval=(union Cyc_YYSTYPE_union)(((_tmpAAF.YY52).tag=56,(((_tmpAAF.YY52).f1=((
_tmpAB1=_cycalloc(sizeof(*_tmpAB1)),((_tmpAB1->hd=((_tmpAB0=_cycalloc(sizeof(*
_tmpAB0)),((_tmpAB0->f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),((_tmpAB0->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmpAB0->f3=0,_tmpAB0)))))))),((_tmpAB1->tl=Cyc_yyget_YY52(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpAB1)))))),_tmpAAF))));}
break;case 16: _LL210:{struct Cyc_List_List*_tmpABB;struct Cyc_Absyn_Fn_d_struct
_tmpABA;struct Cyc_Absyn_Fn_d_struct*_tmpAB9;union Cyc_YYSTYPE_union _tmpAB8;yyval=(
union Cyc_YYSTYPE_union)(((_tmpAB8.YY18).tag=22,(((_tmpAB8.YY18).f1=((_tmpABB=
_cycalloc(sizeof(*_tmpABB)),((_tmpABB->hd=Cyc_Absyn_new_decl((void*)((_tmpAB9=
_cycalloc(sizeof(*_tmpAB9)),((_tmpAB9[0]=((_tmpABA.tag=1,((_tmpABA.f1=Cyc_yyget_YY17(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpABA)))),_tmpAB9)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpABB->tl=0,_tmpABB)))))),_tmpAB8))));}break;
case 17: _LL211: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 18: _LL212:{union Cyc_YYSTYPE_union _tmpABC;yyval=(union Cyc_YYSTYPE_union)(((
_tmpABC.YY18).tag=22,(((_tmpABC.YY18).f1=0,_tmpABC))));}break;case 19: _LL213:{
union Cyc_YYSTYPE_union _tmpABD;yyval=(union Cyc_YYSTYPE_union)(((_tmpABD.YY17).tag=
21,(((_tmpABD.YY17).f1=Cyc_Parse_make_function(0,Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpABD))));}
break;case 20: _LL214:{struct Cyc_Core_Opt*_tmpAC0;union Cyc_YYSTYPE_union _tmpABF;
yyval=(union Cyc_YYSTYPE_union)(((_tmpABF.YY17).tag=21,(((_tmpABF.YY17).f1=Cyc_Parse_make_function(((
_tmpAC0=_cycalloc(sizeof(*_tmpAC0)),((_tmpAC0->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpAC0)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpABF))));}
break;case 21: _LL215:{union Cyc_YYSTYPE_union _tmpAC1;yyval=(union Cyc_YYSTYPE_union)(((
_tmpAC1.YY17).tag=21,(((_tmpAC1.YY17).f1=Cyc_Parse_make_function(0,Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpAC1))));}
break;case 22: _LL216:{struct Cyc_Core_Opt*_tmpAC4;union Cyc_YYSTYPE_union _tmpAC3;
yyval=(union Cyc_YYSTYPE_union)(((_tmpAC3.YY17).tag=21,(((_tmpAC3.YY17).f1=Cyc_Parse_make_function(((
_tmpAC4=_cycalloc(sizeof(*_tmpAC4)),((_tmpAC4->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpAC4)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpAC3))));}
break;case 23: _LL217:{struct Cyc_Core_Opt*_tmpAC7;union Cyc_YYSTYPE_union _tmpAC6;
yyval=(union Cyc_YYSTYPE_union)(((_tmpAC6.YY17).tag=21,(((_tmpAC6.YY17).f1=Cyc_Parse_make_function(((
_tmpAC7=_cycalloc(sizeof(*_tmpAC7)),((_tmpAC7->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpAC7)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpAC6))));}
break;case 24: _LL218:{struct Cyc_Core_Opt*_tmpACA;union Cyc_YYSTYPE_union _tmpAC9;
yyval=(union Cyc_YYSTYPE_union)(((_tmpAC9.YY17).tag=21,(((_tmpAC9.YY17).f1=Cyc_Parse_make_function(((
_tmpACA=_cycalloc(sizeof(*_tmpACA)),((_tmpACA->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmpACA)))),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpAC9))));}
break;case 25: _LL219: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 26: _LL21A: Cyc_Lex_leave_using();
break;case 27: _LL21B:{struct _dynforward_ptr*_tmpACB;Cyc_Lex_enter_namespace(((
_tmpACB=_cycalloc(sizeof(*_tmpACB)),((_tmpACB[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpACB)))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 28: _LL21C: Cyc_Lex_leave_namespace();
break;case 29: _LL21D:{union Cyc_YYSTYPE_union _tmpACC;yyval=(union Cyc_YYSTYPE_union)(((
_tmpACC.YY18).tag=22,(((_tmpACC.YY18).f1=Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmpACC))));}
break;case 30: _LL21E:{union Cyc_YYSTYPE_union _tmpACD;yyval=(union Cyc_YYSTYPE_union)(((
_tmpACD.YY18).tag=22,(((_tmpACD.YY18).f1=Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpACD))));}
break;case 31: _LL21F:{struct Cyc_List_List*_tmpAD0;union Cyc_YYSTYPE_union _tmpACF;
yyval=(union Cyc_YYSTYPE_union)(((_tmpACF.YY18).tag=22,(((_tmpACF.YY18).f1=((
_tmpAD0=_cycalloc(sizeof(*_tmpAD0)),((_tmpAD0->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAD0->tl=0,
_tmpAD0)))))),_tmpACF))));}break;case 32: _LL220: {struct Cyc_List_List*_tmp3E9=0;{
struct Cyc_List_List*_tmp3EA=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);for(0;_tmp3EA != 0;_tmp3EA=_tmp3EA->tl){struct
_dynforward_ptr*_tmp3EB=(struct _dynforward_ptr*)_tmp3EA->hd;union Cyc_Absyn_Nmspace_union
_tmpAD3;struct _tuple1*_tmpAD2;struct _tuple1*qv=(_tmpAD2=_cycalloc(sizeof(*
_tmpAD2)),((_tmpAD2->f1=(union Cyc_Absyn_Nmspace_union)(((_tmpAD3.Rel_n).tag=1,(((
_tmpAD3.Rel_n).f1=0,_tmpAD3)))),((_tmpAD2->f2=_tmp3EB,_tmpAD2)))));struct Cyc_Absyn_Vardecl*
_tmp3EC=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);struct Cyc_List_List*
_tmpAD4;_tmp3E9=((_tmpAD4=_cycalloc(sizeof(*_tmpAD4)),((_tmpAD4->hd=_tmp3EC,((
_tmpAD4->tl=_tmp3E9,_tmpAD4))))));}}_tmp3E9=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmp3E9);{struct Cyc_List_List*_tmpAD7;union Cyc_YYSTYPE_union
_tmpAD6;yyval=(union Cyc_YYSTYPE_union)(((_tmpAD6.YY18).tag=22,(((_tmpAD6.YY18).f1=((
_tmpAD7=_cycalloc(sizeof(*_tmpAD7)),((_tmpAD7->hd=Cyc_Absyn_letv_decl(_tmp3E9,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),((_tmpAD7->tl=0,_tmpAD7)))))),_tmpAD6))));}break;}
case 33: _LL221: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 34: _LL222:{union Cyc_YYSTYPE_union _tmpAD8;yyval=(union Cyc_YYSTYPE_union)(((
_tmpAD8.YY18).tag=22,(((_tmpAD8.YY18).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmpAD8))));}break;case 35:
_LL223:{struct Cyc_Parse_Declaration_spec*_tmpADE;struct Cyc_Core_Opt*_tmpADD;
union Cyc_YYSTYPE_union _tmpADC;yyval=(union Cyc_YYSTYPE_union)(((_tmpADC.YY19).tag=
23,(((_tmpADC.YY19).f1=((_tmpADE=_cycalloc(sizeof(*_tmpADE)),((_tmpADE->sc=((
_tmpADD=_cycalloc(sizeof(*_tmpADD)),((_tmpADD->v=(void*)Cyc_yyget_YY22(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpADD)))),((_tmpADE->tq=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpADE->type_specs=
0,((_tmpADE->is_inline=0,((_tmpADE->attributes=0,_tmpADE)))))))))))),_tmpADC))));}
break;case 36: _LL224: if((Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->sc != 0){const char*_tmpAE1;void*_tmpAE0;(_tmpAE0=0,Cyc_Tcutil_warn(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpAE1="Only one storage class is allowed in a declaration",
_tag_dynforward(_tmpAE1,sizeof(char),_get_zero_arr_size_char(_tmpAE1,51)))),
_tag_dynforward(_tmpAE0,sizeof(void*),0)));}{struct Cyc_Parse_Declaration_spec*
_tmpAE7;struct Cyc_Core_Opt*_tmpAE6;union Cyc_YYSTYPE_union _tmpAE5;yyval=(union Cyc_YYSTYPE_union)(((
_tmpAE5.YY19).tag=23,(((_tmpAE5.YY19).f1=((_tmpAE7=_cycalloc(sizeof(*_tmpAE7)),((
_tmpAE7->sc=((_tmpAE6=_cycalloc(sizeof(*_tmpAE6)),((_tmpAE6->v=(void*)Cyc_yyget_YY22(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpAE6)))),((
_tmpAE7->tq=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->tq,((_tmpAE7->type_specs=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,((_tmpAE7->is_inline=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline,((
_tmpAE7->attributes=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->attributes,_tmpAE7)))))))))))),_tmpAE5))));}break;case 37:
_LL225:{struct Cyc_Parse_Declaration_spec*_tmpAED;struct Cyc_List_List*_tmpAEC;
union Cyc_YYSTYPE_union _tmpAEB;yyval=(union Cyc_YYSTYPE_union)(((_tmpAEB.YY19).tag=
23,(((_tmpAEB.YY19).f1=((_tmpAED=_cycalloc(sizeof(*_tmpAED)),((_tmpAED->sc=0,((
_tmpAED->tq=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAED->type_specs=((
_tmpAEC=_cycalloc(sizeof(*_tmpAEC)),((_tmpAEC->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpAEC->tl=0,_tmpAEC)))))),((
_tmpAED->is_inline=0,((_tmpAED->attributes=0,_tmpAED)))))))))))),_tmpAEB))));}
break;case 38: _LL226:{struct Cyc_Parse_Declaration_spec*_tmpAF3;struct Cyc_List_List*
_tmpAF2;union Cyc_YYSTYPE_union _tmpAF1;yyval=(union Cyc_YYSTYPE_union)(((_tmpAF1.YY19).tag=
23,(((_tmpAF1.YY19).f1=((_tmpAF3=_cycalloc(sizeof(*_tmpAF3)),((_tmpAF3->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpAF3->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((
_tmpAF3->type_specs=((_tmpAF2=_cycalloc(sizeof(*_tmpAF2)),((_tmpAF2->hd=(void*)
Cyc_yyget_YY23(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmpAF2->tl=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs,_tmpAF2)))))),((_tmpAF3->is_inline=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline,((_tmpAF3->attributes=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,
_tmpAF3)))))))))))),_tmpAF1))));}break;case 39: _LL227:{struct Cyc_Parse_Declaration_spec*
_tmpAF6;union Cyc_YYSTYPE_union _tmpAF5;yyval=(union Cyc_YYSTYPE_union)(((_tmpAF5.YY19).tag=
23,(((_tmpAF5.YY19).f1=((_tmpAF6=_cycalloc(sizeof(*_tmpAF6)),((_tmpAF6->sc=0,((
_tmpAF6->tq=Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpAF6->type_specs=0,((_tmpAF6->is_inline=0,((_tmpAF6->attributes=
0,_tmpAF6)))))))))))),_tmpAF5))));}break;case 40: _LL228:{struct Cyc_Parse_Declaration_spec*
_tmpAF9;union Cyc_YYSTYPE_union _tmpAF8;yyval=(union Cyc_YYSTYPE_union)(((_tmpAF8.YY19).tag=
23,(((_tmpAF8.YY19).f1=((_tmpAF9=_cycalloc(sizeof(*_tmpAF9)),((_tmpAF9->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpAF9->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq),((_tmpAF9->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,((
_tmpAF9->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline,((_tmpAF9->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpAF9)))))))))))),
_tmpAF8))));}break;case 41: _LL229:{struct Cyc_Parse_Declaration_spec*_tmpAFC;union
Cyc_YYSTYPE_union _tmpAFB;yyval=(union Cyc_YYSTYPE_union)(((_tmpAFB.YY19).tag=23,(((
_tmpAFB.YY19).f1=((_tmpAFC=_cycalloc(sizeof(*_tmpAFC)),((_tmpAFC->sc=0,((_tmpAFC->tq=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpAFC->type_specs=
0,((_tmpAFC->is_inline=1,((_tmpAFC->attributes=0,_tmpAFC)))))))))))),_tmpAFB))));}
break;case 42: _LL22A:{struct Cyc_Parse_Declaration_spec*_tmpAFF;union Cyc_YYSTYPE_union
_tmpAFE;yyval=(union Cyc_YYSTYPE_union)(((_tmpAFE.YY19).tag=23,(((_tmpAFE.YY19).f1=((
_tmpAFF=_cycalloc(sizeof(*_tmpAFF)),((_tmpAFF->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpAFF->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((_tmpAFF->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,((
_tmpAFF->is_inline=1,((_tmpAFF->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes,_tmpAFF)))))))))))),
_tmpAFE))));}break;case 43: _LL22B:{struct Cyc_Parse_Declaration_spec*_tmpB02;union
Cyc_YYSTYPE_union _tmpB01;yyval=(union Cyc_YYSTYPE_union)(((_tmpB01.YY19).tag=23,(((
_tmpB01.YY19).f1=((_tmpB02=_cycalloc(sizeof(*_tmpB02)),((_tmpB02->sc=0,((_tmpB02->tq=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpB02->type_specs=
0,((_tmpB02->is_inline=0,((_tmpB02->attributes=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB02)))))))))))),_tmpB01))));}
break;case 44: _LL22C:{struct Cyc_Parse_Declaration_spec*_tmpB05;union Cyc_YYSTYPE_union
_tmpB04;yyval=(union Cyc_YYSTYPE_union)(((_tmpB04.YY19).tag=23,(((_tmpB04.YY19).f1=((
_tmpB05=_cycalloc(sizeof(*_tmpB05)),((_tmpB05->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc,((_tmpB05->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq,((_tmpB05->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs,((
_tmpB05->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline,((_tmpB05->attributes=Cyc_List_imp_append(Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes),_tmpB05)))))))))))),
_tmpB04))));}break;case 45: _LL22D:{union Cyc_YYSTYPE_union _tmpB06;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB06.YY22).tag=26,(((_tmpB06.YY22).f1=(void*)((void*)4),_tmpB06))));}break;
case 46: _LL22E:{union Cyc_YYSTYPE_union _tmpB07;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB07.YY22).tag=26,(((_tmpB07.YY22).f1=(void*)((void*)5),_tmpB07))));}break;
case 47: _LL22F:{union Cyc_YYSTYPE_union _tmpB08;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB08.YY22).tag=26,(((_tmpB08.YY22).f1=(void*)((void*)3),_tmpB08))));}break;
case 48: _LL230:{union Cyc_YYSTYPE_union _tmpB09;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB09.YY22).tag=26,(((_tmpB09.YY22).f1=(void*)((void*)1),_tmpB09))));}break;
case 49: _LL231:{const char*_tmpB0A;if(Cyc_strcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB0A="C",
_tag_dynforward(_tmpB0A,sizeof(char),_get_zero_arr_size_char(_tmpB0A,2)))))!= 0){
const char*_tmpB0B;Cyc_Parse_err(((_tmpB0B="only extern or extern \"C\" is allowed",
_tag_dynforward(_tmpB0B,sizeof(char),_get_zero_arr_size_char(_tmpB0B,37)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}}{union Cyc_YYSTYPE_union
_tmpB0C;yyval=(union Cyc_YYSTYPE_union)(((_tmpB0C.YY22).tag=26,(((_tmpB0C.YY22).f1=(
void*)((void*)2),_tmpB0C))));}break;case 50: _LL232:{union Cyc_YYSTYPE_union _tmpB0D;
yyval=(union Cyc_YYSTYPE_union)(((_tmpB0D.YY22).tag=26,(((_tmpB0D.YY22).f1=(void*)((
void*)0),_tmpB0D))));}break;case 51: _LL233:{union Cyc_YYSTYPE_union _tmpB0E;yyval=(
union Cyc_YYSTYPE_union)(((_tmpB0E.YY22).tag=26,(((_tmpB0E.YY22).f1=(void*)((void*)
6),_tmpB0E))));}break;case 52: _LL234:{union Cyc_YYSTYPE_union _tmpB0F;yyval=(union
Cyc_YYSTYPE_union)(((_tmpB0F.YY45).tag=49,(((_tmpB0F.YY45).f1=0,_tmpB0F))));}
break;case 53: _LL235: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 54: _LL236: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)];break;case 55: _LL237:{struct Cyc_List_List*_tmpB12;union Cyc_YYSTYPE_union
_tmpB11;yyval=(union Cyc_YYSTYPE_union)(((_tmpB11.YY45).tag=49,(((_tmpB11.YY45).f1=((
_tmpB12=_cycalloc(sizeof(*_tmpB12)),((_tmpB12->hd=(void*)Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB12->tl=0,_tmpB12)))))),
_tmpB11))));}break;case 56: _LL238:{struct Cyc_List_List*_tmpB15;union Cyc_YYSTYPE_union
_tmpB14;yyval=(union Cyc_YYSTYPE_union)(((_tmpB14.YY45).tag=49,(((_tmpB14.YY45).f1=((
_tmpB15=_cycalloc(sizeof(*_tmpB15)),((_tmpB15->hd=(void*)Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpB15->tl=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpB15)))))),_tmpB14))));}
break;case 57: _LL239: {static struct Cyc_Absyn_Aligned_att_struct att_aligned={1,- 1};
static struct _tuple19 att_map[17]={{{_tmp41F,_tmp41F + 8},(void*)0},{{_tmp420,
_tmp420 + 6},(void*)1},{{_tmp421,_tmp421 + 9},(void*)2},{{_tmp422,_tmp422 + 9},(
void*)3},{{_tmp423,_tmp423 + 6},(void*)4},{{_tmp424,_tmp424 + 8},(void*)&
att_aligned},{{_tmp425,_tmp425 + 7},(void*)5},{{_tmp426,_tmp426 + 7},(void*)7},{{
_tmp427,_tmp427 + 7},(void*)8},{{_tmp428,_tmp428 + 5},(void*)9},{{_tmp429,_tmp429 + 
10},(void*)10},{{_tmp42A,_tmp42A + 10},(void*)11},{{_tmp42B,_tmp42B + 23},(void*)
12},{{_tmp42C,_tmp42C + 12},(void*)13},{{_tmp42D,_tmp42D + 11},(void*)14},{{
_tmp42E,_tmp42E + 22},(void*)15},{{_tmp42F,_tmp42F + 5},(void*)16}};struct
_dynforward_ptr _tmp41B=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);if((((_get_dynforward_size(_tmp41B,sizeof(char))> 4  && *((
const char*)_check_dynforward_subscript(_tmp41B,sizeof(char),0))== '_') && *((
const char*)_check_dynforward_subscript(_tmp41B,sizeof(char),1))== '_') && *((
const char*)_check_dynforward_subscript(_tmp41B,sizeof(char),(int)(
_get_dynforward_size(_tmp41B,sizeof(char))- 2)))== '_') && *((const char*)
_check_dynforward_subscript(_tmp41B,sizeof(char),(int)(_get_dynforward_size(
_tmp41B,sizeof(char))- 3)))== '_')_tmp41B=(struct _dynforward_ptr)Cyc_substring((
struct _dynforward_ptr)_tmp41B,2,_get_dynforward_size(_tmp41B,sizeof(char))- 5);{
int i=0;for(0;i < 17;++ i){if(Cyc_strcmp((struct _dynforward_ptr)_tmp41B,(struct
_dynforward_ptr)(att_map[_check_known_subscript_notnull(17,i)]).f1)== 0){{union
Cyc_YYSTYPE_union _tmpB16;yyval=(union Cyc_YYSTYPE_union)(((_tmpB16.YY46).tag=50,(((
_tmpB16.YY46).f1=(void*)(att_map[_check_known_subscript_notnull(17,i)]).f2,
_tmpB16))));}break;}}if(i == 17){{const char*_tmpB17;Cyc_Parse_err(((_tmpB17="unrecognized attribute",
_tag_dynforward(_tmpB17,sizeof(char),_get_zero_arr_size_char(_tmpB17,23)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}{union Cyc_YYSTYPE_union
_tmpB18;yyval=(union Cyc_YYSTYPE_union)(((_tmpB18.YY46).tag=50,(((_tmpB18.YY46).f1=(
void*)((void*)1),_tmpB18))));}}break;}}case 58: _LL23A:{union Cyc_YYSTYPE_union
_tmpB19;yyval=(union Cyc_YYSTYPE_union)(((_tmpB19.YY46).tag=50,(((_tmpB19.YY46).f1=(
void*)((void*)4),_tmpB19))));}break;case 59: _LL23B: {struct _dynforward_ptr _tmp432=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
int _tmp434;struct _tuple11 _tmp433=Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp434=_tmp433.f2;{void*
a;{const char*_tmpB1B;const char*_tmpB1A;if(Cyc_zstrcmp((struct _dynforward_ptr)
_tmp432,((_tmpB1B="regparm",_tag_dynforward(_tmpB1B,sizeof(char),
_get_zero_arr_size_char(_tmpB1B,8)))))== 0  || Cyc_zstrcmp((struct _dynforward_ptr)
_tmp432,((_tmpB1A="__regparm__",_tag_dynforward(_tmpB1A,sizeof(char),
_get_zero_arr_size_char(_tmpB1A,12)))))== 0){if(_tmp434 < 0  || _tmp434 > 3){const
char*_tmpB1C;Cyc_Parse_err(((_tmpB1C="regparm requires value between 0 and 3",
_tag_dynforward(_tmpB1C,sizeof(char),_get_zero_arr_size_char(_tmpB1C,39)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Regparm_att_struct
_tmpB1F;struct Cyc_Absyn_Regparm_att_struct*_tmpB1E;a=(void*)((_tmpB1E=
_cycalloc_atomic(sizeof(*_tmpB1E)),((_tmpB1E[0]=((_tmpB1F.tag=0,((_tmpB1F.f1=
_tmp434,_tmpB1F)))),_tmpB1E))));}}else{const char*_tmpB21;const char*_tmpB20;if(
Cyc_zstrcmp((struct _dynforward_ptr)_tmp432,((_tmpB21="aligned",_tag_dynforward(
_tmpB21,sizeof(char),_get_zero_arr_size_char(_tmpB21,8)))))== 0  || Cyc_zstrcmp((
struct _dynforward_ptr)_tmp432,((_tmpB20="__aligned__",_tag_dynforward(_tmpB20,
sizeof(char),_get_zero_arr_size_char(_tmpB20,12)))))== 0){if(_tmp434 < 0){const
char*_tmpB22;Cyc_Parse_err(((_tmpB22="aligned requires positive power of two",
_tag_dynforward(_tmpB22,sizeof(char),_get_zero_arr_size_char(_tmpB22,39)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{
unsigned int j=(unsigned int)_tmp434;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0){
const char*_tmpB23;Cyc_Parse_err(((_tmpB23="aligned requires positive power of two",
_tag_dynforward(_tmpB23,sizeof(char),_get_zero_arr_size_char(_tmpB23,39)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Absyn_Aligned_att_struct
_tmpB26;struct Cyc_Absyn_Aligned_att_struct*_tmpB25;a=(void*)((_tmpB25=
_cycalloc_atomic(sizeof(*_tmpB25)),((_tmpB25[0]=((_tmpB26.tag=1,((_tmpB26.f1=
_tmp434,_tmpB26)))),_tmpB25))));}}}else{const char*_tmpB28;const char*_tmpB27;if(
Cyc_zstrcmp((struct _dynforward_ptr)_tmp432,((_tmpB28="initializes",
_tag_dynforward(_tmpB28,sizeof(char),_get_zero_arr_size_char(_tmpB28,12)))))== 0
 || Cyc_zstrcmp((struct _dynforward_ptr)_tmp432,((_tmpB27="__initializes__",
_tag_dynforward(_tmpB27,sizeof(char),_get_zero_arr_size_char(_tmpB27,16)))))== 0){
struct Cyc_Absyn_Initializes_att_struct _tmpB2B;struct Cyc_Absyn_Initializes_att_struct*
_tmpB2A;a=(void*)((_tmpB2A=_cycalloc_atomic(sizeof(*_tmpB2A)),((_tmpB2A[0]=((
_tmpB2B.tag=4,((_tmpB2B.f1=_tmp434,_tmpB2B)))),_tmpB2A))));}else{{const char*
_tmpB2C;Cyc_Parse_err(((_tmpB2C="unrecognized attribute",_tag_dynforward(_tmpB2C,
sizeof(char),_get_zero_arr_size_char(_tmpB2C,23)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)1;}}}}{
union Cyc_YYSTYPE_union _tmpB2D;yyval=(union Cyc_YYSTYPE_union)(((_tmpB2D.YY46).tag=
50,(((_tmpB2D.YY46).f1=(void*)a,_tmpB2D))));}break;}}case 60: _LL23C: {struct
_dynforward_ptr _tmp446=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dynforward_ptr _tmp447=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;{const char*
_tmpB2F;const char*_tmpB2E;if(Cyc_zstrcmp((struct _dynforward_ptr)_tmp446,((
_tmpB2F="section",_tag_dynforward(_tmpB2F,sizeof(char),_get_zero_arr_size_char(
_tmpB2F,8)))))== 0  || Cyc_zstrcmp((struct _dynforward_ptr)_tmp446,((_tmpB2E="__section__",
_tag_dynforward(_tmpB2E,sizeof(char),_get_zero_arr_size_char(_tmpB2E,12)))))== 0){
struct Cyc_Absyn_Section_att_struct _tmpB32;struct Cyc_Absyn_Section_att_struct*
_tmpB31;a=(void*)((_tmpB31=_cycalloc(sizeof(*_tmpB31)),((_tmpB31[0]=((_tmpB32.tag=
2,((_tmpB32.f1=_tmp447,_tmpB32)))),_tmpB31))));}else{{const char*_tmpB33;Cyc_Parse_err(((
_tmpB33="unrecognized attribute",_tag_dynforward(_tmpB33,sizeof(char),
_get_zero_arr_size_char(_tmpB33,23)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)1;}}{
union Cyc_YYSTYPE_union _tmpB34;yyval=(union Cyc_YYSTYPE_union)(((_tmpB34.YY46).tag=
50,(((_tmpB34.YY46).f1=(void*)a,_tmpB34))));}break;}case 61: _LL23D: {struct
_dynforward_ptr _tmp44E=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dynforward_ptr _tmp44F=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;{const char*
_tmpB35;if(Cyc_zstrcmp((struct _dynforward_ptr)_tmp44E,((_tmpB35="__mode__",
_tag_dynforward(_tmpB35,sizeof(char),_get_zero_arr_size_char(_tmpB35,9)))))== 0){
struct Cyc_Absyn_Mode_att_struct _tmpB38;struct Cyc_Absyn_Mode_att_struct*_tmpB37;a=(
void*)((_tmpB37=_cycalloc(sizeof(*_tmpB37)),((_tmpB37[0]=((_tmpB38.tag=5,((
_tmpB38.f1=_tmp44F,_tmpB38)))),_tmpB37))));}else{{const char*_tmpB39;Cyc_Parse_err(((
_tmpB39="unrecognized attribute",_tag_dynforward(_tmpB39,sizeof(char),
_get_zero_arr_size_char(_tmpB39,23)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}a=(void*)1;}}{
union Cyc_YYSTYPE_union _tmpB3A;yyval=(union Cyc_YYSTYPE_union)(((_tmpB3A.YY46).tag=
50,(((_tmpB3A.YY46).f1=(void*)a,_tmpB3A))));}break;}case 62: _LL23E: {struct
_dynforward_ptr _tmp455=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _dynforward_ptr _tmp456=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp458;struct
_tuple11 _tmp457=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp458=_tmp457.f2;{int _tmp45A;struct _tuple11 _tmp459=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp45A=_tmp459.f2;{
void*a=(void*)1;{const char*_tmpB3C;const char*_tmpB3B;if(Cyc_zstrcmp((struct
_dynforward_ptr)_tmp455,((_tmpB3C="format",_tag_dynforward(_tmpB3C,sizeof(char),
_get_zero_arr_size_char(_tmpB3C,7)))))== 0  || Cyc_zstrcmp((struct _dynforward_ptr)
_tmp455,((_tmpB3B="__format__",_tag_dynforward(_tmpB3B,sizeof(char),
_get_zero_arr_size_char(_tmpB3B,11)))))== 0){const char*_tmpB3D;if(Cyc_zstrcmp((
struct _dynforward_ptr)_tmp456,((_tmpB3D="printf",_tag_dynforward(_tmpB3D,sizeof(
char),_get_zero_arr_size_char(_tmpB3D,7)))))== 0){struct Cyc_Absyn_Format_att_struct
_tmpB40;struct Cyc_Absyn_Format_att_struct*_tmpB3F;a=(void*)((_tmpB3F=_cycalloc(
sizeof(*_tmpB3F)),((_tmpB3F[0]=((_tmpB40.tag=3,((_tmpB40.f1=(void*)((void*)0),((
_tmpB40.f2=_tmp458,((_tmpB40.f3=_tmp45A,_tmpB40)))))))),_tmpB3F))));}else{const
char*_tmpB41;if(Cyc_zstrcmp((struct _dynforward_ptr)_tmp456,((_tmpB41="scanf",
_tag_dynforward(_tmpB41,sizeof(char),_get_zero_arr_size_char(_tmpB41,6)))))== 0){
struct Cyc_Absyn_Format_att_struct _tmpB44;struct Cyc_Absyn_Format_att_struct*
_tmpB43;a=(void*)((_tmpB43=_cycalloc(sizeof(*_tmpB43)),((_tmpB43[0]=((_tmpB44.tag=
3,((_tmpB44.f1=(void*)((void*)1),((_tmpB44.f2=_tmp458,((_tmpB44.f3=_tmp45A,
_tmpB44)))))))),_tmpB43))));}else{const char*_tmpB45;Cyc_Parse_err(((_tmpB45="unrecognized format type",
_tag_dynforward(_tmpB45,sizeof(char),_get_zero_arr_size_char(_tmpB45,25)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{
const char*_tmpB46;Cyc_Parse_err(((_tmpB46="unrecognized attribute",
_tag_dynforward(_tmpB46,sizeof(char),_get_zero_arr_size_char(_tmpB46,23)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}}{union Cyc_YYSTYPE_union
_tmpB47;yyval=(union Cyc_YYSTYPE_union)(((_tmpB47.YY46).tag=50,(((_tmpB47.YY46).f1=(
void*)a,_tmpB47))));}break;}}}case 63: _LL23F: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 64: _LL240:{struct
Cyc_Absyn_TypedefType_struct*_tmpB4D;struct Cyc_Absyn_TypedefType_struct _tmpB4C;
union Cyc_YYSTYPE_union _tmpB4B;yyval=(union Cyc_YYSTYPE_union)(((_tmpB4B.YY23).tag=
27,(((_tmpB4B.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpB4D=_cycalloc(
sizeof(*_tmpB4D)),((_tmpB4D[0]=((_tmpB4C.tag=17,((_tmpB4C.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpB4C.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpB4C.f3=0,((
_tmpB4C.f4=0,_tmpB4C)))))))))),_tmpB4D)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB4B))));}
break;case 65: _LL241:{union Cyc_YYSTYPE_union _tmpB4E;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB4E.YY23).tag=27,(((_tmpB4E.YY23).f1=(void*)Cyc_Parse_type_spec((void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB4E))));}
break;case 66: _LL242:{union Cyc_YYSTYPE_union _tmpB4F;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB4F.YY23).tag=27,(((_tmpB4F.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(
0,0),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpB4F))));}break;case 67: _LL243:{union Cyc_YYSTYPE_union
_tmpB50;yyval=(union Cyc_YYSTYPE_union)(((_tmpB50.YY23).tag=27,(((_tmpB50.YY23).f1=(
void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB50))));}
break;case 68: _LL244:{union Cyc_YYSTYPE_union _tmpB51;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB51.YY23).tag=27,(((_tmpB51.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_char_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpB51))));}break;case 69: _LL245:{struct Cyc_Parse_Short_spec_struct*
_tmpB57;struct Cyc_Parse_Short_spec_struct _tmpB56;union Cyc_YYSTYPE_union _tmpB55;
yyval=(union Cyc_YYSTYPE_union)(((_tmpB55.YY23).tag=27,(((_tmpB55.YY23).f1=(void*)((
void*)((_tmpB57=_cycalloc(sizeof(*_tmpB57)),((_tmpB57[0]=((_tmpB56.tag=2,((
_tmpB56.f1=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpB56)))),_tmpB57))))),_tmpB55))));}break;case 70:
_LL246:{union Cyc_YYSTYPE_union _tmpB58;yyval=(union Cyc_YYSTYPE_union)(((_tmpB58.YY23).tag=
27,(((_tmpB58.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB58))));}
break;case 71: _LL247:{struct Cyc_Parse_Long_spec_struct*_tmpB5E;struct Cyc_Parse_Long_spec_struct
_tmpB5D;union Cyc_YYSTYPE_union _tmpB5C;yyval=(union Cyc_YYSTYPE_union)(((_tmpB5C.YY23).tag=
27,(((_tmpB5C.YY23).f1=(void*)((void*)((_tmpB5E=_cycalloc(sizeof(*_tmpB5E)),((
_tmpB5E[0]=((_tmpB5D.tag=3,((_tmpB5D.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB5D)))),
_tmpB5E))))),_tmpB5C))));}break;case 72: _LL248:{union Cyc_YYSTYPE_union _tmpB5F;
yyval=(union Cyc_YYSTYPE_union)(((_tmpB5F.YY23).tag=27,(((_tmpB5F.YY23).f1=(void*)
Cyc_Parse_type_spec(Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB5F))));}
break;case 73: _LL249:{union Cyc_YYSTYPE_union _tmpB60;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB60.YY23).tag=27,(((_tmpB60.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_double_typ(
0),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpB60))));}break;case 74: _LL24A:{struct Cyc_Parse_Signed_spec_struct*
_tmpB66;struct Cyc_Parse_Signed_spec_struct _tmpB65;union Cyc_YYSTYPE_union _tmpB64;
yyval=(union Cyc_YYSTYPE_union)(((_tmpB64.YY23).tag=27,(((_tmpB64.YY23).f1=(void*)((
void*)((_tmpB66=_cycalloc(sizeof(*_tmpB66)),((_tmpB66[0]=((_tmpB65.tag=0,((
_tmpB65.f1=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpB65)))),_tmpB66))))),_tmpB64))));}break;case 75:
_LL24B:{struct Cyc_Parse_Unsigned_spec_struct*_tmpB6C;struct Cyc_Parse_Unsigned_spec_struct
_tmpB6B;union Cyc_YYSTYPE_union _tmpB6A;yyval=(union Cyc_YYSTYPE_union)(((_tmpB6A.YY23).tag=
27,(((_tmpB6A.YY23).f1=(void*)((void*)((_tmpB6C=_cycalloc(sizeof(*_tmpB6C)),((
_tmpB6C[0]=((_tmpB6B.tag=1,((_tmpB6B.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpB6B)))),
_tmpB6C))))),_tmpB6A))));}break;case 76: _LL24C: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 77: _LL24D: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 78: _LL24E:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 79:
_LL24F:{union Cyc_YYSTYPE_union _tmpB6D;yyval=(union Cyc_YYSTYPE_union)(((_tmpB6D.YY23).tag=
27,(((_tmpB6D.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB6D))));}
break;case 80: _LL250:{struct Cyc_Absyn_TupleType_struct*_tmpB73;struct Cyc_Absyn_TupleType_struct
_tmpB72;union Cyc_YYSTYPE_union _tmpB71;yyval=(union Cyc_YYSTYPE_union)(((_tmpB71.YY23).tag=
27,(((_tmpB71.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpB73=_cycalloc(
sizeof(*_tmpB73)),((_tmpB73[0]=((_tmpB72.tag=9,((_tmpB72.f1=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))),_tmpB72)))),_tmpB73)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpB71))));}break;case 81: _LL251:{struct Cyc_Absyn_RgnHandleType_struct*
_tmpB79;struct Cyc_Absyn_RgnHandleType_struct _tmpB78;union Cyc_YYSTYPE_union
_tmpB77;yyval=(union Cyc_YYSTYPE_union)(((_tmpB77.YY23).tag=27,(((_tmpB77.YY23).f1=(
void*)Cyc_Parse_type_spec((void*)((_tmpB79=_cycalloc(sizeof(*_tmpB79)),((_tmpB79[
0]=((_tmpB78.tag=15,((_tmpB78.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB78)))),_tmpB79)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpB77))));}break;case 82: _LL252: {void*_tmp483=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,0);{struct Cyc_Absyn_DynRgnType_struct*_tmpB7F;
struct Cyc_Absyn_DynRgnType_struct _tmpB7E;union Cyc_YYSTYPE_union _tmpB7D;yyval=(
union Cyc_YYSTYPE_union)(((_tmpB7D.YY23).tag=27,(((_tmpB7D.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)((_tmpB7F=_cycalloc(sizeof(*_tmpB7F)),((_tmpB7F[0]=((_tmpB7E.tag=16,((
_tmpB7E.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpB7E.f2=(void*)_tmp483,_tmpB7E)))))),_tmpB7F)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB7D))));}
break;}case 83: _LL253:{struct Cyc_Absyn_DynRgnType_struct*_tmpB85;struct Cyc_Absyn_DynRgnType_struct
_tmpB84;union Cyc_YYSTYPE_union _tmpB83;yyval=(union Cyc_YYSTYPE_union)(((_tmpB83.YY23).tag=
27,(((_tmpB83.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpB85=_cycalloc(
sizeof(*_tmpB85)),((_tmpB85[0]=((_tmpB84.tag=16,((_tmpB84.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpB84.f2=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpB84)))))),_tmpB85)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB83))));}
break;case 84: _LL254:{struct Cyc_Absyn_SizeofType_struct*_tmpB8B;struct Cyc_Absyn_SizeofType_struct
_tmpB8A;union Cyc_YYSTYPE_union _tmpB89;yyval=(union Cyc_YYSTYPE_union)(((_tmpB89.YY23).tag=
27,(((_tmpB89.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpB8B=_cycalloc(
sizeof(*_tmpB8B)),((_tmpB8B[0]=((_tmpB8A.tag=14,((_tmpB8A.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB8A)))),_tmpB8B)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpB89))));}break;case 85: _LL255:{struct Cyc_Absyn_TagType_struct*
_tmpB91;struct Cyc_Absyn_TagType_struct _tmpB90;union Cyc_YYSTYPE_union _tmpB8F;
yyval=(union Cyc_YYSTYPE_union)(((_tmpB8F.YY23).tag=27,(((_tmpB8F.YY23).f1=(void*)
Cyc_Parse_type_spec((void*)((_tmpB91=_cycalloc(sizeof(*_tmpB91)),((_tmpB91[0]=((
_tmpB90.tag=18,((_tmpB90.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpB90)))),_tmpB91)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpB8F))));}break;case 86: _LL256:{struct Cyc_Absyn_TagType_struct*
_tmpB97;struct Cyc_Absyn_TagType_struct _tmpB96;union Cyc_YYSTYPE_union _tmpB95;
yyval=(union Cyc_YYSTYPE_union)(((_tmpB95.YY23).tag=27,(((_tmpB95.YY23).f1=(void*)
Cyc_Parse_type_spec((void*)((_tmpB97=_cycalloc(sizeof(*_tmpB97)),((_tmpB97[0]=((
_tmpB96.tag=18,((_tmpB96.f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ik,
0),_tmpB96)))),_tmpB97)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB95))));}
break;case 87: _LL257: {int _tmp494;struct _tuple11 _tmp493=Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp494=_tmp493.f2;{struct
Cyc_Absyn_TypeInt_struct*_tmpB9D;struct Cyc_Absyn_TypeInt_struct _tmpB9C;union Cyc_YYSTYPE_union
_tmpB9B;yyval=(union Cyc_YYSTYPE_union)(((_tmpB9B.YY23).tag=27,(((_tmpB9B.YY23).f1=(
void*)Cyc_Parse_type_spec((void*)((_tmpB9D=_cycalloc_atomic(sizeof(*_tmpB9D)),((
_tmpB9D[0]=((_tmpB9C.tag=19,((_tmpB9C.f1=_tmp494,_tmpB9C)))),_tmpB9D)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB9B))));}
break;}case 88: _LL258:{union Cyc_YYSTYPE_union _tmpB9E;yyval=(union Cyc_YYSTYPE_union)(((
_tmpB9E.YY43).tag=47,(((_tmpB9E.YY43).f1=(void*)Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpB9E))));}
break;case 89: _LL259: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line): 0;{struct Cyc_Absyn_Tqual _tmpBA1;union Cyc_YYSTYPE_union
_tmpBA0;yyval=(union Cyc_YYSTYPE_union)(((_tmpBA0.YY25).tag=29,(((_tmpBA0.YY25).f1=((
_tmpBA1.print_const=1,((_tmpBA1.q_volatile=0,((_tmpBA1.q_restrict=0,((_tmpBA1.real_const=
1,((_tmpBA1.loc=loc,_tmpBA1)))))))))),_tmpBA0))));}break;}case 90: _LL25A:{struct
Cyc_Absyn_Tqual _tmpBA4;union Cyc_YYSTYPE_union _tmpBA3;yyval=(union Cyc_YYSTYPE_union)(((
_tmpBA3.YY25).tag=29,(((_tmpBA3.YY25).f1=((_tmpBA4.print_const=0,((_tmpBA4.q_volatile=
1,((_tmpBA4.q_restrict=0,((_tmpBA4.real_const=0,((_tmpBA4.loc=0,_tmpBA4)))))))))),
_tmpBA3))));}break;case 91: _LL25B:{struct Cyc_Absyn_Tqual _tmpBA7;union Cyc_YYSTYPE_union
_tmpBA6;yyval=(union Cyc_YYSTYPE_union)(((_tmpBA6.YY25).tag=29,(((_tmpBA6.YY25).f1=((
_tmpBA7.print_const=0,((_tmpBA7.q_volatile=0,((_tmpBA7.q_restrict=1,((_tmpBA7.real_const=
0,((_tmpBA7.loc=0,_tmpBA7)))))))))),_tmpBA6))));}break;case 92: _LL25C:{struct Cyc_Parse_Decl_spec_struct*
_tmpBCB;struct Cyc_Absyn_Decl*_tmpBCA;struct Cyc_Absyn_Enum_d_struct _tmpBC9;struct
Cyc_Core_Opt*_tmpBC8;struct Cyc_Absyn_Enumdecl*_tmpBC7;struct Cyc_Absyn_Enum_d_struct*
_tmpBC6;struct Cyc_Parse_Decl_spec_struct _tmpBC5;union Cyc_YYSTYPE_union _tmpBC4;
yyval=(union Cyc_YYSTYPE_union)(((_tmpBC4.YY23).tag=27,(((_tmpBC4.YY23).f1=(void*)((
void*)((_tmpBCB=_cycalloc(sizeof(*_tmpBCB)),((_tmpBCB[0]=((_tmpBC5.tag=5,((
_tmpBC5.f1=((_tmpBCA=_cycalloc(sizeof(*_tmpBCA)),((_tmpBCA->r=(void*)((void*)((
_tmpBC6=_cycalloc(sizeof(*_tmpBC6)),((_tmpBC6[0]=((_tmpBC9.tag=6,((_tmpBC9.f1=((
_tmpBC7=_cycalloc(sizeof(*_tmpBC7)),((_tmpBC7->sc=(void*)((void*)2),((_tmpBC7->name=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpBC7->fields=((_tmpBC8=_cycalloc(sizeof(*_tmpBC8)),((_tmpBC8->v=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpBC8)))),_tmpBC7)))))))),
_tmpBC9)))),_tmpBC6))))),((_tmpBCA->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpBCA)))))),
_tmpBC5)))),_tmpBCB))))),_tmpBC4))));}break;case 93: _LL25D:{struct Cyc_Absyn_EnumType_struct*
_tmpBD1;struct Cyc_Absyn_EnumType_struct _tmpBD0;union Cyc_YYSTYPE_union _tmpBCF;
yyval=(union Cyc_YYSTYPE_union)(((_tmpBCF.YY23).tag=27,(((_tmpBCF.YY23).f1=(void*)
Cyc_Parse_type_spec((void*)((_tmpBD1=_cycalloc(sizeof(*_tmpBD1)),((_tmpBD1[0]=((
_tmpBD0.tag=12,((_tmpBD0.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBD0.f2=0,_tmpBD0)))))),
_tmpBD1)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpBCF))));}break;case 94: _LL25E:{struct Cyc_Parse_Type_spec_struct*
_tmpBE0;struct Cyc_Absyn_AnonEnumType_struct*_tmpBDF;struct Cyc_Absyn_AnonEnumType_struct
_tmpBDE;struct Cyc_Parse_Type_spec_struct _tmpBDD;union Cyc_YYSTYPE_union _tmpBDC;
yyval=(union Cyc_YYSTYPE_union)(((_tmpBDC.YY23).tag=27,(((_tmpBDC.YY23).f1=(void*)((
void*)((_tmpBE0=_cycalloc(sizeof(*_tmpBE0)),((_tmpBE0[0]=((_tmpBDD.tag=4,((
_tmpBDD.f1=(void*)((void*)((_tmpBDF=_cycalloc(sizeof(*_tmpBDF)),((_tmpBDF[0]=((
_tmpBDE.tag=13,((_tmpBDE.f1=Cyc_yyget_YY48(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmpBDE)))),_tmpBDF))))),((_tmpBDD.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpBDD)))))),
_tmpBE0))))),_tmpBDC))));}break;case 95: _LL25F:{struct Cyc_Absyn_Enumfield*_tmpBE3;
union Cyc_YYSTYPE_union _tmpBE2;yyval=(union Cyc_YYSTYPE_union)(((_tmpBE2.YY47).tag=
51,(((_tmpBE2.YY47).f1=((_tmpBE3=_cycalloc(sizeof(*_tmpBE3)),((_tmpBE3->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBE3->tag=0,((
_tmpBE3->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmpBE3)))))))),_tmpBE2))));}break;case 96: _LL260:{
struct Cyc_Absyn_Enumfield*_tmpBE6;union Cyc_YYSTYPE_union _tmpBE5;yyval=(union Cyc_YYSTYPE_union)(((
_tmpBE5.YY47).tag=51,(((_tmpBE5.YY47).f1=((_tmpBE6=_cycalloc(sizeof(*_tmpBE6)),((
_tmpBE6->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),((_tmpBE6->tag=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBE6->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpBE6)))))))),
_tmpBE5))));}break;case 97: _LL261:{struct Cyc_List_List*_tmpBE9;union Cyc_YYSTYPE_union
_tmpBE8;yyval=(union Cyc_YYSTYPE_union)(((_tmpBE8.YY48).tag=52,(((_tmpBE8.YY48).f1=((
_tmpBE9=_cycalloc(sizeof(*_tmpBE9)),((_tmpBE9->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpBE9->tl=0,_tmpBE9)))))),
_tmpBE8))));}break;case 98: _LL262:{struct Cyc_List_List*_tmpBEC;union Cyc_YYSTYPE_union
_tmpBEB;yyval=(union Cyc_YYSTYPE_union)(((_tmpBEB.YY48).tag=52,(((_tmpBEB.YY48).f1=((
_tmpBEC=_cycalloc(sizeof(*_tmpBEC)),((_tmpBEC->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpBEC->tl=0,_tmpBEC)))))),
_tmpBEB))));}break;case 99: _LL263:{struct Cyc_List_List*_tmpBEF;union Cyc_YYSTYPE_union
_tmpBEE;yyval=(union Cyc_YYSTYPE_union)(((_tmpBEE.YY48).tag=52,(((_tmpBEE.YY48).f1=((
_tmpBEF=_cycalloc(sizeof(*_tmpBEF)),((_tmpBEF->hd=Cyc_yyget_YY47(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpBEF->tl=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpBEF)))))),_tmpBEE))));}
break;case 100: _LL264:{struct Cyc_Absyn_AnonAggrType_struct*_tmpBF5;struct Cyc_Absyn_AnonAggrType_struct
_tmpBF4;union Cyc_YYSTYPE_union _tmpBF3;yyval=(union Cyc_YYSTYPE_union)(((_tmpBF3.YY23).tag=
27,(((_tmpBF3.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpBF5=_cycalloc(
sizeof(*_tmpBF5)),((_tmpBF5[0]=((_tmpBF4.tag=11,((_tmpBF4.f1=(void*)Cyc_yyget_YY24(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpBF4.f2=Cyc_yyget_YY26(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpBF4)))))),
_tmpBF5)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpBF3))));}break;case 101: _LL265: {struct Cyc_List_List*
_tmp4BC=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]));struct Cyc_List_List*_tmp4BD=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));{struct Cyc_Parse_Decl_spec_struct*
_tmpBFB;struct Cyc_Parse_Decl_spec_struct _tmpBFA;union Cyc_YYSTYPE_union _tmpBF9;
yyval=(union Cyc_YYSTYPE_union)(((_tmpBF9.YY23).tag=27,(((_tmpBF9.YY23).f1=(void*)((
void*)((_tmpBFB=_cycalloc(sizeof(*_tmpBFB)),((_tmpBFB[0]=((_tmpBFA.tag=5,((
_tmpBFA.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp4BC,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp4BD,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpBFA)))),
_tmpBFB))))),_tmpBF9))));}break;}case 102: _LL266:{struct Cyc_Absyn_AggrType_struct*
_tmpC0A;struct Cyc_Absyn_AggrInfo _tmpC09;union Cyc_Absyn_AggrInfoU_union _tmpC08;
struct Cyc_Absyn_AggrType_struct _tmpC07;union Cyc_YYSTYPE_union _tmpC06;yyval=(
union Cyc_YYSTYPE_union)(((_tmpC06.YY23).tag=27,(((_tmpC06.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)((_tmpC0A=_cycalloc(sizeof(*_tmpC0A)),((_tmpC0A[0]=((_tmpC07.tag=10,((
_tmpC07.f1=((_tmpC09.aggr_info=(union Cyc_Absyn_AggrInfoU_union)(((_tmpC08.UnknownAggr).tag=
0,(((_tmpC08.UnknownAggr).f1=(void*)Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(((_tmpC08.UnknownAggr).f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpC08)))))),((_tmpC09.targs=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),_tmpC09)))),_tmpC07)))),_tmpC0A)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpC06))));}
break;case 103: _LL267:{union Cyc_YYSTYPE_union _tmpC0B;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC0B.YY40).tag=44,(((_tmpC0B.YY40).f1=0,_tmpC0B))));}break;case 104: _LL268:{
union Cyc_YYSTYPE_union _tmpC0C;yyval=(union Cyc_YYSTYPE_union)(((_tmpC0C.YY40).tag=
44,(((_tmpC0C.YY40).f1=Cyc_List_imp_rev(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),_tmpC0C))));}break;case
105: _LL269:{union Cyc_YYSTYPE_union _tmpC0D;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC0D.YY24).tag=28,(((_tmpC0D.YY24).f1=(void*)((void*)0),_tmpC0D))));}break;
case 106: _LL26A:{union Cyc_YYSTYPE_union _tmpC0E;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC0E.YY24).tag=28,(((_tmpC0E.YY24).f1=(void*)((void*)1),_tmpC0E))));}break;
case 107: _LL26B:{union Cyc_YYSTYPE_union _tmpC0F;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC0F.YY26).tag=30,(((_tmpC0F.YY26).f1=0,_tmpC0F))));}break;case 108: _LL26C:{
union Cyc_YYSTYPE_union _tmpC10;yyval=(union Cyc_YYSTYPE_union)(((_tmpC10.YY26).tag=
30,(((_tmpC10.YY26).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_flatten)(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))),_tmpC10))));}break;
case 109: _LL26D:{struct Cyc_List_List*_tmpC13;union Cyc_YYSTYPE_union _tmpC12;yyval=(
union Cyc_YYSTYPE_union)(((_tmpC12.YY27).tag=31,(((_tmpC12.YY27).f1=((_tmpC13=
_cycalloc(sizeof(*_tmpC13)),((_tmpC13->hd=Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC13->tl=0,_tmpC13)))))),
_tmpC12))));}break;case 110: _LL26E:{struct Cyc_List_List*_tmpC16;union Cyc_YYSTYPE_union
_tmpC15;yyval=(union Cyc_YYSTYPE_union)(((_tmpC15.YY27).tag=31,(((_tmpC15.YY27).f1=((
_tmpC16=_cycalloc(sizeof(*_tmpC16)),((_tmpC16->hd=Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC16->tl=Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpC16)))))),
_tmpC15))));}break;case 111: _LL26F:{union Cyc_YYSTYPE_union _tmpC17;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC17.YY21).tag=25,(((_tmpC17.YY21).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])),_tmpC17))));}break;case 112: _LL270:{struct Cyc_List_List*_tmpC1A;
union Cyc_YYSTYPE_union _tmpC19;yyval=(union Cyc_YYSTYPE_union)(((_tmpC19.YY21).tag=
25,(((_tmpC19.YY21).f1=((_tmpC1A=_cycalloc(sizeof(*_tmpC1A)),((_tmpC1A->hd=Cyc_yyget_YY20(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC1A->tl=0,
_tmpC1A)))))),_tmpC19))));}break;case 113: _LL271:{struct Cyc_List_List*_tmpC1D;
union Cyc_YYSTYPE_union _tmpC1C;yyval=(union Cyc_YYSTYPE_union)(((_tmpC1C.YY21).tag=
25,(((_tmpC1C.YY21).f1=((_tmpC1D=_cycalloc(sizeof(*_tmpC1D)),((_tmpC1D->hd=Cyc_yyget_YY20(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC1D->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpC1D)))))),
_tmpC1C))));}break;case 114: _LL272:{struct _tuple15*_tmpC20;union Cyc_YYSTYPE_union
_tmpC1F;yyval=(union Cyc_YYSTYPE_union)(((_tmpC1F.YY20).tag=24,(((_tmpC1F.YY20).f1=((
_tmpC20=_cycalloc(sizeof(*_tmpC20)),((_tmpC20->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC20->f2=0,_tmpC20)))))),
_tmpC1F))));}break;case 115: _LL273:{struct _tuple15*_tmpC23;union Cyc_YYSTYPE_union
_tmpC22;yyval=(union Cyc_YYSTYPE_union)(((_tmpC22.YY20).tag=24,(((_tmpC22.YY20).f1=((
_tmpC23=_cycalloc(sizeof(*_tmpC23)),((_tmpC23->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpC23->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC23)))))),
_tmpC22))));}break;case 116: _LL274:{struct _RegionHandle _tmp4D9=_new_region("temp");
struct _RegionHandle*temp=& _tmp4D9;_push_region(temp);{struct _tuple16 _tmp4DB;
struct Cyc_Absyn_Tqual _tmp4DC;struct Cyc_List_List*_tmp4DD;struct Cyc_List_List*
_tmp4DE;struct _tuple16*_tmp4DA=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4DB=*_tmp4DA;_tmp4DC=
_tmp4DB.f1;_tmp4DD=_tmp4DB.f2;_tmp4DE=_tmp4DB.f3;if(_tmp4DC.loc == 0)_tmp4DC.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);{struct Cyc_List_List*_tmp4E0;struct Cyc_List_List*
_tmp4E1;struct _tuple0 _tmp4DF=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(temp,temp,Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp4E0=_tmp4DF.f1;
_tmp4E1=_tmp4DF.f2;{void*_tmp4E2=Cyc_Parse_speclist2typ(_tmp4DD,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));struct Cyc_List_List*
_tmp4E3=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,Cyc_Parse_apply_tmss(
temp,_tmp4DC,_tmp4E2,_tmp4E0,_tmp4DE),_tmp4E1);union Cyc_YYSTYPE_union _tmpC24;
yyval=(union Cyc_YYSTYPE_union)(((_tmpC24.YY26).tag=30,(((_tmpC24.YY26).f1=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Position_Segment*,
struct _tuple8*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_make_aggr_field,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp4E3),
_tmpC24))));}}};_pop_region(temp);}break;case 117: _LL275:{struct _tuple16*_tmpC2A;
struct Cyc_List_List*_tmpC29;union Cyc_YYSTYPE_union _tmpC28;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC28.YY35).tag=39,(((_tmpC28.YY35).f1=((_tmpC2A=_cycalloc(sizeof(*_tmpC2A)),((
_tmpC2A->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpC2A->f2=((
_tmpC29=_cycalloc(sizeof(*_tmpC29)),((_tmpC29->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC29->tl=0,_tmpC29)))))),((
_tmpC2A->f3=0,_tmpC2A)))))))),_tmpC28))));}break;case 118: _LL276:{struct _tuple16*
_tmpC30;struct Cyc_List_List*_tmpC2F;union Cyc_YYSTYPE_union _tmpC2E;yyval=(union
Cyc_YYSTYPE_union)(((_tmpC2E.YY35).tag=39,(((_tmpC2E.YY35).f1=((_tmpC30=
_cycalloc(sizeof(*_tmpC30)),((_tmpC30->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,((_tmpC30->f2=((_tmpC2F=
_cycalloc(sizeof(*_tmpC2F)),((_tmpC2F->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpC2F->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,_tmpC2F)))))),((
_tmpC30->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpC30)))))))),_tmpC2E))));}break;case 119: _LL277:{struct
_tuple16*_tmpC33;union Cyc_YYSTYPE_union _tmpC32;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC32.YY35).tag=39,(((_tmpC32.YY35).f1=((_tmpC33=_cycalloc(sizeof(*_tmpC33)),((
_tmpC33->f1=Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpC33->f2=0,((_tmpC33->f3=0,_tmpC33)))))))),_tmpC32))));}
break;case 120: _LL278:{struct _tuple16*_tmpC36;union Cyc_YYSTYPE_union _tmpC35;yyval=(
union Cyc_YYSTYPE_union)(((_tmpC35.YY35).tag=39,(((_tmpC35.YY35).f1=((_tmpC36=
_cycalloc(sizeof(*_tmpC36)),((_tmpC36->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1),((_tmpC36->f2=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((
_tmpC36->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpC36)))))))),_tmpC35))));}break;case 121: _LL279:{struct
_tuple16*_tmpC39;union Cyc_YYSTYPE_union _tmpC38;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC38.YY35).tag=39,(((_tmpC38.YY35).f1=((_tmpC39=_cycalloc(sizeof(*_tmpC39)),((
_tmpC39->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpC39->f2=0,((
_tmpC39->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpC39)))))))),_tmpC38))));}break;case 122: _LL27A:{struct
_tuple16*_tmpC3C;union Cyc_YYSTYPE_union _tmpC3B;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC3B.YY35).tag=39,(((_tmpC3B.YY35).f1=((_tmpC3C=_cycalloc(sizeof(*_tmpC3C)),((
_tmpC3C->f1=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1,((_tmpC3C->f2=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((_tmpC3C->f3=Cyc_List_append(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3),
_tmpC3C)))))))),_tmpC3B))));}break;case 123: _LL27B:{struct _tuple16*_tmpC42;struct
Cyc_List_List*_tmpC41;union Cyc_YYSTYPE_union _tmpC40;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC40.YY35).tag=39,(((_tmpC40.YY35).f1=((_tmpC42=_cycalloc(sizeof(*_tmpC42)),((
_tmpC42->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpC42->f2=((
_tmpC41=_cycalloc(sizeof(*_tmpC41)),((_tmpC41->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC41->tl=0,_tmpC41)))))),((
_tmpC42->f3=0,_tmpC42)))))))),_tmpC40))));}break;case 124: _LL27C:{struct _tuple16*
_tmpC48;struct Cyc_List_List*_tmpC47;union Cyc_YYSTYPE_union _tmpC46;yyval=(union
Cyc_YYSTYPE_union)(((_tmpC46.YY35).tag=39,(((_tmpC46.YY35).f1=((_tmpC48=
_cycalloc(sizeof(*_tmpC48)),((_tmpC48->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,((_tmpC48->f2=((_tmpC47=
_cycalloc(sizeof(*_tmpC47)),((_tmpC47->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpC47->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,_tmpC47)))))),((
_tmpC48->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpC48)))))))),_tmpC46))));}break;case 125: _LL27D:{struct
_tuple16*_tmpC4B;union Cyc_YYSTYPE_union _tmpC4A;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC4A.YY35).tag=39,(((_tmpC4A.YY35).f1=((_tmpC4B=_cycalloc(sizeof(*_tmpC4B)),((
_tmpC4B->f1=Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpC4B->f2=0,((_tmpC4B->f3=0,_tmpC4B)))))))),_tmpC4A))));}
break;case 126: _LL27E:{struct _tuple16*_tmpC4E;union Cyc_YYSTYPE_union _tmpC4D;yyval=(
union Cyc_YYSTYPE_union)(((_tmpC4D.YY35).tag=39,(((_tmpC4D.YY35).f1=((_tmpC4E=
_cycalloc(sizeof(*_tmpC4E)),((_tmpC4E->f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1),((_tmpC4E->f2=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((
_tmpC4E->f3=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f3,_tmpC4E)))))))),_tmpC4D))));}break;case 127: _LL27F:{struct
_tuple16*_tmpC51;union Cyc_YYSTYPE_union _tmpC50;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC50.YY35).tag=39,(((_tmpC50.YY35).f1=((_tmpC51=_cycalloc(sizeof(*_tmpC51)),((
_tmpC51->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),((_tmpC51->f2=0,((
_tmpC51->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpC51)))))))),_tmpC50))));}break;case 128: _LL280:{struct
_tuple16*_tmpC54;union Cyc_YYSTYPE_union _tmpC53;yyval=(union Cyc_YYSTYPE_union)(((
_tmpC53.YY35).tag=39,(((_tmpC53.YY35).f1=((_tmpC54=_cycalloc(sizeof(*_tmpC54)),((
_tmpC54->f1=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f1,((_tmpC54->f2=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,((_tmpC54->f3=Cyc_List_append(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3),
_tmpC54)))))))),_tmpC53))));}break;case 129: _LL281:{union Cyc_YYSTYPE_union _tmpC55;
yyval=(union Cyc_YYSTYPE_union)(((_tmpC55.YY21).tag=25,(((_tmpC55.YY21).f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmpC55))));}break;
case 130: _LL282:{struct Cyc_List_List*_tmpC58;union Cyc_YYSTYPE_union _tmpC57;yyval=(
union Cyc_YYSTYPE_union)(((_tmpC57.YY21).tag=25,(((_tmpC57.YY21).f1=((_tmpC58=
_cycalloc(sizeof(*_tmpC58)),((_tmpC58->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC58->tl=0,_tmpC58)))))),
_tmpC57))));}break;case 131: _LL283:{struct Cyc_List_List*_tmpC5B;union Cyc_YYSTYPE_union
_tmpC5A;yyval=(union Cyc_YYSTYPE_union)(((_tmpC5A.YY21).tag=25,(((_tmpC5A.YY21).f1=((
_tmpC5B=_cycalloc(sizeof(*_tmpC5B)),((_tmpC5B->hd=Cyc_yyget_YY20(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC5B->tl=Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpC5B)))))),
_tmpC5A))));}break;case 132: _LL284:{struct _tuple15*_tmpC5E;union Cyc_YYSTYPE_union
_tmpC5D;yyval=(union Cyc_YYSTYPE_union)(((_tmpC5D.YY20).tag=24,(((_tmpC5D.YY20).f1=((
_tmpC5E=_cycalloc(sizeof(*_tmpC5E)),((_tmpC5E->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpC5E->f2=0,_tmpC5E)))))),
_tmpC5D))));}break;case 133: _LL285:{struct _tuple15*_tmpC6D;struct _tuple1*_tmpC6C;
union Cyc_Absyn_Nmspace_union _tmpC6B;struct Cyc_Parse_Declarator*_tmpC6A;union Cyc_YYSTYPE_union
_tmpC69;yyval=(union Cyc_YYSTYPE_union)(((_tmpC69.YY20).tag=24,(((_tmpC69.YY20).f1=((
_tmpC6D=_cycalloc(sizeof(*_tmpC6D)),((_tmpC6D->f1=((_tmpC6A=_cycalloc(sizeof(*
_tmpC6A)),((_tmpC6A->id=((_tmpC6C=_cycalloc(sizeof(*_tmpC6C)),((_tmpC6C->f1=(
union Cyc_Absyn_Nmspace_union)(((_tmpC6B.Rel_n).tag=1,(((_tmpC6B.Rel_n).f1=0,
_tmpC6B)))),((_tmpC6C->f2=_init_dynforward_ptr(_cycalloc(sizeof(struct
_dynforward_ptr)),"",sizeof(char),1),_tmpC6C)))))),((_tmpC6A->tms=0,_tmpC6A)))))),((
_tmpC6D->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC6D)))))),_tmpC69))));}
break;case 134: _LL286:{struct _tuple15*_tmpC70;union Cyc_YYSTYPE_union _tmpC6F;yyval=(
union Cyc_YYSTYPE_union)(((_tmpC6F.YY20).tag=24,(((_tmpC6F.YY20).f1=((_tmpC70=
_cycalloc(sizeof(*_tmpC70)),((_tmpC70->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpC70->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpC70)))))),
_tmpC6F))));}break;case 135: _LL287: {struct Cyc_List_List*_tmp510=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));{struct Cyc_Parse_Decl_spec_struct*
_tmpC7A;struct Cyc_Core_Opt*_tmpC79;struct Cyc_Parse_Decl_spec_struct _tmpC78;union
Cyc_YYSTYPE_union _tmpC77;yyval=(union Cyc_YYSTYPE_union)(((_tmpC77.YY23).tag=27,(((
_tmpC77.YY23).f1=(void*)((void*)((_tmpC7A=_cycalloc(sizeof(*_tmpC7A)),((_tmpC7A[
0]=((_tmpC78.tag=5,((_tmpC78.f1=Cyc_Absyn_tunion_decl((void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp510,((_tmpC79=
_cycalloc(sizeof(*_tmpC79)),((_tmpC79->v=Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpC79)))),Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpC78)))),
_tmpC7A))))),_tmpC77))));}break;}case 136: _LL288:{struct Cyc_Absyn_TunionType_struct*
_tmpC8F;struct Cyc_Absyn_TunionInfo _tmpC8E;struct Cyc_Absyn_UnknownTunionInfo
_tmpC8D;union Cyc_Absyn_TunionInfoU_union _tmpC8C;struct Cyc_Absyn_TunionType_struct
_tmpC8B;union Cyc_YYSTYPE_union _tmpC8A;yyval=(union Cyc_YYSTYPE_union)(((_tmpC8A.YY23).tag=
27,(((_tmpC8A.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpC8F=_cycalloc(
sizeof(*_tmpC8F)),((_tmpC8F[0]=((_tmpC8B.tag=2,((_tmpC8B.f1=((_tmpC8E.tunion_info=(
union Cyc_Absyn_TunionInfoU_union)(((_tmpC8C.UnknownTunion).tag=0,(((_tmpC8C.UnknownTunion).f1=((
_tmpC8D.name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpC8D.is_xtunion=Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpC8D.is_flat=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmpC8D)))))),
_tmpC8C)))),((_tmpC8E.targs=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpC8E.rgn=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpC8E)))))),_tmpC8B)))),
_tmpC8F)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpC8A))));}break;case 137: _LL289:{struct Cyc_Absyn_TunionFieldType_struct*
_tmpCA4;struct Cyc_Absyn_TunionFieldInfo _tmpCA3;struct Cyc_Absyn_UnknownTunionFieldInfo
_tmpCA2;union Cyc_Absyn_TunionFieldInfoU_union _tmpCA1;struct Cyc_Absyn_TunionFieldType_struct
_tmpCA0;union Cyc_YYSTYPE_union _tmpC9F;yyval=(union Cyc_YYSTYPE_union)(((_tmpC9F.YY23).tag=
27,(((_tmpC9F.YY23).f1=(void*)Cyc_Parse_type_spec((void*)((_tmpCA4=_cycalloc(
sizeof(*_tmpCA4)),((_tmpCA4[0]=((_tmpCA0.tag=3,((_tmpCA0.f1=((_tmpCA3.field_info=(
union Cyc_Absyn_TunionFieldInfoU_union)(((_tmpCA1.UnknownTunionfield).tag=0,(((
_tmpCA1.UnknownTunionfield).f1=((_tmpCA2.tunion_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpCA2.field_name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCA2.is_xtunion=
Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),
_tmpCA2)))))),_tmpCA1)))),((_tmpCA3.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCA3)))),_tmpCA0)))),
_tmpCA4)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpC9F))));}break;case 138: _LL28A:{union Cyc_YYSTYPE_union
_tmpCA5;yyval=(union Cyc_YYSTYPE_union)(((_tmpCA5.YY31).tag=35,(((_tmpCA5.YY31).f1=
1,_tmpCA5))));}break;case 139: _LL28B:{union Cyc_YYSTYPE_union _tmpCA6;yyval=(union
Cyc_YYSTYPE_union)(((_tmpCA6.YY31).tag=35,(((_tmpCA6.YY31).f1=0,_tmpCA6))));}
break;case 140: _LL28C:{union Cyc_YYSTYPE_union _tmpCA7;yyval=(union Cyc_YYSTYPE_union)(((
_tmpCA7.YY31).tag=35,(((_tmpCA7.YY31).f1=0,_tmpCA7))));}break;case 141: _LL28D:{
union Cyc_YYSTYPE_union _tmpCA8;yyval=(union Cyc_YYSTYPE_union)(((_tmpCA8.YY31).tag=
35,(((_tmpCA8.YY31).f1=1,_tmpCA8))));}break;case 142: _LL28E:{struct Cyc_List_List*
_tmpCAB;union Cyc_YYSTYPE_union _tmpCAA;yyval=(union Cyc_YYSTYPE_union)(((_tmpCAA.YY34).tag=
38,(((_tmpCAA.YY34).f1=((_tmpCAB=_cycalloc(sizeof(*_tmpCAB)),((_tmpCAB->hd=Cyc_yyget_YY33(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpCAB->tl=0,
_tmpCAB)))))),_tmpCAA))));}break;case 143: _LL28F:{struct Cyc_List_List*_tmpCAE;
union Cyc_YYSTYPE_union _tmpCAD;yyval=(union Cyc_YYSTYPE_union)(((_tmpCAD.YY34).tag=
38,(((_tmpCAD.YY34).f1=((_tmpCAE=_cycalloc(sizeof(*_tmpCAE)),((_tmpCAE->hd=Cyc_yyget_YY33(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpCAE->tl=0,
_tmpCAE)))))),_tmpCAD))));}break;case 144: _LL290:{struct Cyc_List_List*_tmpCB1;
union Cyc_YYSTYPE_union _tmpCB0;yyval=(union Cyc_YYSTYPE_union)(((_tmpCB0.YY34).tag=
38,(((_tmpCB0.YY34).f1=((_tmpCB1=_cycalloc(sizeof(*_tmpCB1)),((_tmpCB1->hd=Cyc_yyget_YY33(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpCB1->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCB1)))))),_tmpCB0))));}
break;case 145: _LL291:{struct Cyc_List_List*_tmpCB4;union Cyc_YYSTYPE_union _tmpCB3;
yyval=(union Cyc_YYSTYPE_union)(((_tmpCB3.YY34).tag=38,(((_tmpCB3.YY34).f1=((
_tmpCB4=_cycalloc(sizeof(*_tmpCB4)),((_tmpCB4->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpCB4->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpCB4)))))),_tmpCB3))));}
break;case 146: _LL292:{union Cyc_YYSTYPE_union _tmpCB5;yyval=(union Cyc_YYSTYPE_union)(((
_tmpCB5.YY32).tag=36,(((_tmpCB5.YY32).f1=(void*)((void*)2),_tmpCB5))));}break;
case 147: _LL293:{union Cyc_YYSTYPE_union _tmpCB6;yyval=(union Cyc_YYSTYPE_union)(((
_tmpCB6.YY32).tag=36,(((_tmpCB6.YY32).f1=(void*)((void*)3),_tmpCB6))));}break;
case 148: _LL294:{union Cyc_YYSTYPE_union _tmpCB7;yyval=(union Cyc_YYSTYPE_union)(((
_tmpCB7.YY32).tag=36,(((_tmpCB7.YY32).f1=(void*)((void*)0),_tmpCB7))));}break;
case 149: _LL295:{struct Cyc_Absyn_Tunionfield*_tmpCBA;union Cyc_YYSTYPE_union
_tmpCB9;yyval=(union Cyc_YYSTYPE_union)(((_tmpCB9.YY33).tag=37,(((_tmpCB9.YY33).f1=((
_tmpCBA=_cycalloc(sizeof(*_tmpCBA)),((_tmpCBA->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpCBA->typs=0,((_tmpCBA->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpCBA->sc=(void*)Cyc_yyget_YY32(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpCBA)))))))))),
_tmpCB9))));}break;case 150: _LL296: {struct Cyc_List_List*_tmp532=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_Absyn_Tunionfield*
_tmpCBD;union Cyc_YYSTYPE_union _tmpCBC;yyval=(union Cyc_YYSTYPE_union)(((_tmpCBC.YY33).tag=
37,(((_tmpCBC.YY33).f1=((_tmpCBD=_cycalloc(sizeof(*_tmpCBD)),((_tmpCBD->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpCBD->typs=
_tmp532,((_tmpCBD->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpCBD->sc=(
void*)Cyc_yyget_YY32(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
_tmpCBD)))))))))),_tmpCBC))));}break;}case 151: _LL297: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 152: _LL298:{struct
Cyc_Parse_Declarator*_tmpCC0;union Cyc_YYSTYPE_union _tmpCBF;yyval=(union Cyc_YYSTYPE_union)(((
_tmpCBF.YY29).tag=33,(((_tmpCBF.YY29).f1=((_tmpCC0=_cycalloc(sizeof(*_tmpCC0)),((
_tmpCC0->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id,((_tmpCC0->tms=Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpCC0)))))),
_tmpCBF))));}break;case 153: _LL299: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 154: _LL29A:{struct Cyc_Parse_Declarator*_tmpCC3;
union Cyc_YYSTYPE_union _tmpCC2;yyval=(union Cyc_YYSTYPE_union)(((_tmpCC2.YY29).tag=
33,(((_tmpCC2.YY29).f1=((_tmpCC3=_cycalloc(sizeof(*_tmpCC3)),((_tmpCC3->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->id,((_tmpCC3->tms=
Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->tms),_tmpCC3)))))),_tmpCC2))));}break;case 155: _LL29B:{struct
Cyc_Parse_Declarator*_tmpCC6;union Cyc_YYSTYPE_union _tmpCC5;yyval=(union Cyc_YYSTYPE_union)(((
_tmpCC5.YY29).tag=33,(((_tmpCC5.YY29).f1=((_tmpCC6=_cycalloc(sizeof(*_tmpCC6)),((
_tmpCC6->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpCC6->tms=0,_tmpCC6)))))),_tmpCC5))));}break;case 156: _LL29C:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 157:
_LL29D: {struct Cyc_Parse_Declarator*_tmp53B=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{struct Cyc_Absyn_Attributes_mod_struct*
_tmpCCC;struct Cyc_Absyn_Attributes_mod_struct _tmpCCB;struct Cyc_List_List*_tmpCCA;
_tmp53B->tms=((_tmpCCA=_cycalloc(sizeof(*_tmpCCA)),((_tmpCCA->hd=(void*)((void*)((
_tmpCCC=_cycalloc(sizeof(*_tmpCCC)),((_tmpCCC[0]=((_tmpCCB.tag=5,((_tmpCCB.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),((_tmpCCB.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpCCB)))))),_tmpCCC))))),((
_tmpCCA->tl=_tmp53B->tms,_tmpCCA))))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 158: _LL29E:{
struct Cyc_Parse_Declarator*_tmpCDB;struct Cyc_Absyn_Carray_mod_struct*_tmpCDA;
struct Cyc_Absyn_Carray_mod_struct _tmpCD9;struct Cyc_List_List*_tmpCD8;union Cyc_YYSTYPE_union
_tmpCD7;yyval=(union Cyc_YYSTYPE_union)(((_tmpCD7.YY29).tag=33,(((_tmpCD7.YY29).f1=((
_tmpCDB=_cycalloc(sizeof(*_tmpCDB)),((_tmpCDB->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpCDB->tms=((
_tmpCD8=_cycalloc(sizeof(*_tmpCD8)),((_tmpCD8->hd=(void*)((void*)((_tmpCDA=
_cycalloc(sizeof(*_tmpCDA)),((_tmpCDA[0]=((_tmpCD9.tag=0,((_tmpCD9.f1=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpCD9.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpCD9)))))),
_tmpCDA))))),((_tmpCD8->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpCD8)))))),_tmpCDB)))))),_tmpCD7))));}break;
case 159: _LL29F:{struct Cyc_Parse_Declarator*_tmpCEA;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpCE9;struct Cyc_Absyn_ConstArray_mod_struct _tmpCE8;struct Cyc_List_List*_tmpCE7;
union Cyc_YYSTYPE_union _tmpCE6;yyval=(union Cyc_YYSTYPE_union)(((_tmpCE6.YY29).tag=
33,(((_tmpCE6.YY29).f1=((_tmpCEA=_cycalloc(sizeof(*_tmpCEA)),((_tmpCEA->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpCEA->tms=((
_tmpCE7=_cycalloc(sizeof(*_tmpCE7)),((_tmpCE7->hd=(void*)((void*)((_tmpCE9=
_cycalloc(sizeof(*_tmpCE9)),((_tmpCE9[0]=((_tmpCE8.tag=1,((_tmpCE8.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpCE8.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpCE8.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpCE8)))))))),
_tmpCE9))))),((_tmpCE7->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpCE7)))))),_tmpCEA)))))),_tmpCE6))));}break;
case 160: _LL2A0: {struct _tuple17 _tmp54A;struct Cyc_List_List*_tmp54B;int _tmp54C;
struct Cyc_Absyn_VarargInfo*_tmp54D;struct Cyc_Core_Opt*_tmp54E;struct Cyc_List_List*
_tmp54F;struct _tuple17*_tmp549=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp54A=*_tmp549;_tmp54B=
_tmp54A.f1;_tmp54C=_tmp54A.f2;_tmp54D=_tmp54A.f3;_tmp54E=_tmp54A.f4;_tmp54F=
_tmp54A.f5;{struct Cyc_Parse_Declarator*_tmpD06;struct Cyc_Absyn_Function_mod_struct*
_tmpD05;struct Cyc_Absyn_WithTypes_struct*_tmpD04;struct Cyc_Absyn_WithTypes_struct
_tmpD03;struct Cyc_Absyn_Function_mod_struct _tmpD02;struct Cyc_List_List*_tmpD01;
union Cyc_YYSTYPE_union _tmpD00;yyval=(union Cyc_YYSTYPE_union)(((_tmpD00.YY29).tag=
33,(((_tmpD00.YY29).f1=((_tmpD06=_cycalloc(sizeof(*_tmpD06)),((_tmpD06->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpD06->tms=((
_tmpD01=_cycalloc(sizeof(*_tmpD01)),((_tmpD01->hd=(void*)((void*)((_tmpD05=
_cycalloc(sizeof(*_tmpD05)),((_tmpD05[0]=((_tmpD02.tag=3,((_tmpD02.f1=(void*)((
void*)((_tmpD04=_cycalloc(sizeof(*_tmpD04)),((_tmpD04[0]=((_tmpD03.tag=1,((
_tmpD03.f1=_tmp54B,((_tmpD03.f2=_tmp54C,((_tmpD03.f3=_tmp54D,((_tmpD03.f4=
_tmp54E,((_tmpD03.f5=_tmp54F,_tmpD03)))))))))))),_tmpD04))))),_tmpD02)))),
_tmpD05))))),((_tmpD01->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpD01)))))),_tmpD06)))))),_tmpD00))));}break;}
case 161: _LL2A1:{struct Cyc_Parse_Declarator*_tmpD22;struct Cyc_Absyn_Function_mod_struct*
_tmpD21;struct Cyc_Absyn_WithTypes_struct*_tmpD20;struct Cyc_Absyn_WithTypes_struct
_tmpD1F;struct Cyc_Absyn_Function_mod_struct _tmpD1E;struct Cyc_List_List*_tmpD1D;
union Cyc_YYSTYPE_union _tmpD1C;yyval=(union Cyc_YYSTYPE_union)(((_tmpD1C.YY29).tag=
33,(((_tmpD1C.YY29).f1=((_tmpD22=_cycalloc(sizeof(*_tmpD22)),((_tmpD22->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpD22->tms=((
_tmpD1D=_cycalloc(sizeof(*_tmpD1D)),((_tmpD1D->hd=(void*)((void*)((_tmpD21=
_cycalloc(sizeof(*_tmpD21)),((_tmpD21[0]=((_tmpD1E.tag=3,((_tmpD1E.f1=(void*)((
void*)((_tmpD20=_cycalloc(sizeof(*_tmpD20)),((_tmpD20[0]=((_tmpD1F.tag=1,((
_tmpD1F.f1=0,((_tmpD1F.f2=0,((_tmpD1F.f3=0,((_tmpD1F.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD1F.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpD1F)))))))))))),
_tmpD20))))),_tmpD1E)))),_tmpD21))))),((_tmpD1D->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpD1D)))))),
_tmpD22)))))),_tmpD1C))));}break;case 162: _LL2A2:{struct Cyc_Parse_Declarator*
_tmpD3E;struct Cyc_Absyn_Function_mod_struct*_tmpD3D;struct Cyc_Absyn_NoTypes_struct*
_tmpD3C;struct Cyc_Absyn_NoTypes_struct _tmpD3B;struct Cyc_Absyn_Function_mod_struct
_tmpD3A;struct Cyc_List_List*_tmpD39;union Cyc_YYSTYPE_union _tmpD38;yyval=(union
Cyc_YYSTYPE_union)(((_tmpD38.YY29).tag=33,(((_tmpD38.YY29).f1=((_tmpD3E=
_cycalloc(sizeof(*_tmpD3E)),((_tmpD3E->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpD3E->tms=((
_tmpD39=_cycalloc(sizeof(*_tmpD39)),((_tmpD39->hd=(void*)((void*)((_tmpD3D=
_cycalloc(sizeof(*_tmpD3D)),((_tmpD3D[0]=((_tmpD3A.tag=3,((_tmpD3A.f1=(void*)((
void*)((_tmpD3C=_cycalloc(sizeof(*_tmpD3C)),((_tmpD3C[0]=((_tmpD3B.tag=0,((
_tmpD3B.f1=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]),((_tmpD3B.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD3B)))))),
_tmpD3C))))),_tmpD3A)))),_tmpD3D))))),((_tmpD39->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,_tmpD39)))))),
_tmpD3E)))))),_tmpD38))));}break;case 163: _LL2A3: {struct Cyc_List_List*_tmp565=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_Parse_Declarator*
_tmpD4D;struct Cyc_Absyn_TypeParams_mod_struct*_tmpD4C;struct Cyc_Absyn_TypeParams_mod_struct
_tmpD4B;struct Cyc_List_List*_tmpD4A;union Cyc_YYSTYPE_union _tmpD49;yyval=(union
Cyc_YYSTYPE_union)(((_tmpD49.YY29).tag=33,(((_tmpD49.YY29).f1=((_tmpD4D=
_cycalloc(sizeof(*_tmpD4D)),((_tmpD4D->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpD4D->tms=((
_tmpD4A=_cycalloc(sizeof(*_tmpD4A)),((_tmpD4A->hd=(void*)((void*)((_tmpD4C=
_cycalloc(sizeof(*_tmpD4C)),((_tmpD4C[0]=((_tmpD4B.tag=4,((_tmpD4B.f1=_tmp565,((
_tmpD4B.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpD4B.f3=0,_tmpD4B)))))))),_tmpD4C))))),((_tmpD4A->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpD4A)))))),_tmpD4D)))))),_tmpD49))));}break;}case 164: _LL2A4:{struct Cyc_Parse_Declarator*
_tmpD5C;struct Cyc_Absyn_Attributes_mod_struct*_tmpD5B;struct Cyc_Absyn_Attributes_mod_struct
_tmpD5A;struct Cyc_List_List*_tmpD59;union Cyc_YYSTYPE_union _tmpD58;yyval=(union
Cyc_YYSTYPE_union)(((_tmpD58.YY29).tag=33,(((_tmpD58.YY29).f1=((_tmpD5C=
_cycalloc(sizeof(*_tmpD5C)),((_tmpD5C->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id,((_tmpD5C->tms=((
_tmpD59=_cycalloc(sizeof(*_tmpD59)),((_tmpD59->hd=(void*)((void*)((_tmpD5B=
_cycalloc(sizeof(*_tmpD5B)),((_tmpD5B[0]=((_tmpD5A.tag=5,((_tmpD5A.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpD5A.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpD5A)))))),_tmpD5B))))),((
_tmpD59->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpD59)))))),_tmpD5C)))))),_tmpD58))));}break;case 165:
_LL2A5:{struct Cyc_Parse_Declarator*_tmpD5F;union Cyc_YYSTYPE_union _tmpD5E;yyval=(
union Cyc_YYSTYPE_union)(((_tmpD5E.YY29).tag=33,(((_tmpD5E.YY29).f1=((_tmpD5F=
_cycalloc(sizeof(*_tmpD5F)),((_tmpD5F->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD5F->tms=0,_tmpD5F)))))),
_tmpD5E))));}break;case 166: _LL2A6:{struct Cyc_Parse_Declarator*_tmpD62;union Cyc_YYSTYPE_union
_tmpD61;yyval=(union Cyc_YYSTYPE_union)(((_tmpD61.YY29).tag=33,(((_tmpD61.YY29).f1=((
_tmpD62=_cycalloc(sizeof(*_tmpD62)),((_tmpD62->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD62->tms=0,_tmpD62)))))),
_tmpD61))));}break;case 167: _LL2A7: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;case 168: _LL2A8: {struct Cyc_Parse_Declarator*
_tmp574=Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);{
struct Cyc_Absyn_Attributes_mod_struct*_tmpD68;struct Cyc_Absyn_Attributes_mod_struct
_tmpD67;struct Cyc_List_List*_tmpD66;_tmp574->tms=((_tmpD66=_cycalloc(sizeof(*
_tmpD66)),((_tmpD66->hd=(void*)((void*)((_tmpD68=_cycalloc(sizeof(*_tmpD68)),((
_tmpD68[0]=((_tmpD67.tag=5,((_tmpD67.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),((_tmpD67.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmpD67)))))),_tmpD68))))),((_tmpD66->tl=_tmp574->tms,_tmpD66))))));}yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 169: _LL2A9:{
struct Cyc_Parse_Declarator*_tmpD77;struct Cyc_Absyn_Carray_mod_struct*_tmpD76;
struct Cyc_Absyn_Carray_mod_struct _tmpD75;struct Cyc_List_List*_tmpD74;union Cyc_YYSTYPE_union
_tmpD73;yyval=(union Cyc_YYSTYPE_union)(((_tmpD73.YY29).tag=33,(((_tmpD73.YY29).f1=((
_tmpD77=_cycalloc(sizeof(*_tmpD77)),((_tmpD77->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpD77->tms=((
_tmpD74=_cycalloc(sizeof(*_tmpD74)),((_tmpD74->hd=(void*)((void*)((_tmpD76=
_cycalloc(sizeof(*_tmpD76)),((_tmpD76[0]=((_tmpD75.tag=0,((_tmpD75.f1=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD75.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD75)))))),
_tmpD76))))),((_tmpD74->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpD74)))))),_tmpD77)))))),_tmpD73))));}break;
case 170: _LL2AA:{struct Cyc_Parse_Declarator*_tmpD86;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpD85;struct Cyc_Absyn_ConstArray_mod_struct _tmpD84;struct Cyc_List_List*_tmpD83;
union Cyc_YYSTYPE_union _tmpD82;yyval=(union Cyc_YYSTYPE_union)(((_tmpD82.YY29).tag=
33,(((_tmpD82.YY29).f1=((_tmpD86=_cycalloc(sizeof(*_tmpD86)),((_tmpD86->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpD86->tms=((
_tmpD83=_cycalloc(sizeof(*_tmpD83)),((_tmpD83->hd=(void*)((void*)((_tmpD85=
_cycalloc(sizeof(*_tmpD85)),((_tmpD85[0]=((_tmpD84.tag=1,((_tmpD84.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpD84.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpD84.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpD84)))))))),
_tmpD85))))),((_tmpD83->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpD83)))))),_tmpD86)))))),_tmpD82))));}break;
case 171: _LL2AB: {struct _tuple17 _tmp583;struct Cyc_List_List*_tmp584;int _tmp585;
struct Cyc_Absyn_VarargInfo*_tmp586;struct Cyc_Core_Opt*_tmp587;struct Cyc_List_List*
_tmp588;struct _tuple17*_tmp582=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp583=*_tmp582;_tmp584=
_tmp583.f1;_tmp585=_tmp583.f2;_tmp586=_tmp583.f3;_tmp587=_tmp583.f4;_tmp588=
_tmp583.f5;{struct Cyc_Parse_Declarator*_tmpDA2;struct Cyc_Absyn_Function_mod_struct*
_tmpDA1;struct Cyc_Absyn_WithTypes_struct*_tmpDA0;struct Cyc_Absyn_WithTypes_struct
_tmpD9F;struct Cyc_Absyn_Function_mod_struct _tmpD9E;struct Cyc_List_List*_tmpD9D;
union Cyc_YYSTYPE_union _tmpD9C;yyval=(union Cyc_YYSTYPE_union)(((_tmpD9C.YY29).tag=
33,(((_tmpD9C.YY29).f1=((_tmpDA2=_cycalloc(sizeof(*_tmpDA2)),((_tmpDA2->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpDA2->tms=((
_tmpD9D=_cycalloc(sizeof(*_tmpD9D)),((_tmpD9D->hd=(void*)((void*)((_tmpDA1=
_cycalloc(sizeof(*_tmpDA1)),((_tmpDA1[0]=((_tmpD9E.tag=3,((_tmpD9E.f1=(void*)((
void*)((_tmpDA0=_cycalloc(sizeof(*_tmpDA0)),((_tmpDA0[0]=((_tmpD9F.tag=1,((
_tmpD9F.f1=_tmp584,((_tmpD9F.f2=_tmp585,((_tmpD9F.f3=_tmp586,((_tmpD9F.f4=
_tmp587,((_tmpD9F.f5=_tmp588,_tmpD9F)))))))))))),_tmpDA0))))),_tmpD9E)))),
_tmpDA1))))),((_tmpD9D->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpD9D)))))),_tmpDA2)))))),_tmpD9C))));}break;}
case 172: _LL2AC:{struct Cyc_Parse_Declarator*_tmpDBE;struct Cyc_Absyn_Function_mod_struct*
_tmpDBD;struct Cyc_Absyn_WithTypes_struct*_tmpDBC;struct Cyc_Absyn_WithTypes_struct
_tmpDBB;struct Cyc_Absyn_Function_mod_struct _tmpDBA;struct Cyc_List_List*_tmpDB9;
union Cyc_YYSTYPE_union _tmpDB8;yyval=(union Cyc_YYSTYPE_union)(((_tmpDB8.YY29).tag=
33,(((_tmpDB8.YY29).f1=((_tmpDBE=_cycalloc(sizeof(*_tmpDBE)),((_tmpDBE->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id,((_tmpDBE->tms=((
_tmpDB9=_cycalloc(sizeof(*_tmpDB9)),((_tmpDB9->hd=(void*)((void*)((_tmpDBD=
_cycalloc(sizeof(*_tmpDBD)),((_tmpDBD[0]=((_tmpDBA.tag=3,((_tmpDBA.f1=(void*)((
void*)((_tmpDBC=_cycalloc(sizeof(*_tmpDBC)),((_tmpDBC[0]=((_tmpDBB.tag=1,((
_tmpDBB.f1=0,((_tmpDBB.f2=0,((_tmpDBB.f3=0,((_tmpDBB.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpDBB.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpDBB)))))))))))),
_tmpDBC))))),_tmpDBA)))),_tmpDBD))))),((_tmpDB9->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpDB9)))))),
_tmpDBE)))))),_tmpDB8))));}break;case 173: _LL2AD:{struct Cyc_Parse_Declarator*
_tmpDDA;struct Cyc_Absyn_Function_mod_struct*_tmpDD9;struct Cyc_Absyn_NoTypes_struct*
_tmpDD8;struct Cyc_Absyn_NoTypes_struct _tmpDD7;struct Cyc_Absyn_Function_mod_struct
_tmpDD6;struct Cyc_List_List*_tmpDD5;union Cyc_YYSTYPE_union _tmpDD4;yyval=(union
Cyc_YYSTYPE_union)(((_tmpDD4.YY29).tag=33,(((_tmpDD4.YY29).f1=((_tmpDDA=
_cycalloc(sizeof(*_tmpDDA)),((_tmpDDA->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpDDA->tms=((
_tmpDD5=_cycalloc(sizeof(*_tmpDD5)),((_tmpDD5->hd=(void*)((void*)((_tmpDD9=
_cycalloc(sizeof(*_tmpDD9)),((_tmpDD9[0]=((_tmpDD6.tag=3,((_tmpDD6.f1=(void*)((
void*)((_tmpDD8=_cycalloc(sizeof(*_tmpDD8)),((_tmpDD8[0]=((_tmpDD7.tag=0,((
_tmpDD7.f1=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]),((_tmpDD7.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpDD7)))))),
_tmpDD8))))),_tmpDD6)))),_tmpDD9))))),((_tmpDD5->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,_tmpDD5)))))),
_tmpDDA)))))),_tmpDD4))));}break;case 174: _LL2AE: {struct Cyc_List_List*_tmp59E=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),Cyc_List_imp_rev(Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{struct Cyc_Parse_Declarator*
_tmpDE9;struct Cyc_Absyn_TypeParams_mod_struct*_tmpDE8;struct Cyc_Absyn_TypeParams_mod_struct
_tmpDE7;struct Cyc_List_List*_tmpDE6;union Cyc_YYSTYPE_union _tmpDE5;yyval=(union
Cyc_YYSTYPE_union)(((_tmpDE5.YY29).tag=33,(((_tmpDE5.YY29).f1=((_tmpDE9=
_cycalloc(sizeof(*_tmpDE9)),((_tmpDE9->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id,((_tmpDE9->tms=((
_tmpDE6=_cycalloc(sizeof(*_tmpDE6)),((_tmpDE6->hd=(void*)((void*)((_tmpDE8=
_cycalloc(sizeof(*_tmpDE8)),((_tmpDE8[0]=((_tmpDE7.tag=4,((_tmpDE7.f1=_tmp59E,((
_tmpDE7.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),((_tmpDE7.f3=0,_tmpDE7)))))))),_tmpDE8))))),((_tmpDE6->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,
_tmpDE6)))))),_tmpDE9)))))),_tmpDE5))));}break;}case 175: _LL2AF:{struct Cyc_Parse_Declarator*
_tmpDF8;struct Cyc_Absyn_Attributes_mod_struct*_tmpDF7;struct Cyc_Absyn_Attributes_mod_struct
_tmpDF6;struct Cyc_List_List*_tmpDF5;union Cyc_YYSTYPE_union _tmpDF4;yyval=(union
Cyc_YYSTYPE_union)(((_tmpDF4.YY29).tag=33,(((_tmpDF4.YY29).f1=((_tmpDF8=
_cycalloc(sizeof(*_tmpDF8)),((_tmpDF8->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id,((_tmpDF8->tms=((
_tmpDF5=_cycalloc(sizeof(*_tmpDF5)),((_tmpDF5->hd=(void*)((void*)((_tmpDF7=
_cycalloc(sizeof(*_tmpDF7)),((_tmpDF7[0]=((_tmpDF6.tag=5,((_tmpDF6.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpDF6.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpDF6)))))),_tmpDF7))))),((
_tmpDF5->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms,_tmpDF5)))))),_tmpDF8)))))),_tmpDF4))));}break;case 176:
_LL2B0: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
177: _LL2B1:{union Cyc_YYSTYPE_union _tmpDF9;yyval=(union Cyc_YYSTYPE_union)(((
_tmpDF9.YY28).tag=32,(((_tmpDF9.YY28).f1=Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmpDF9))));}break;case 178:
_LL2B2: {struct Cyc_List_List*ans=0;if(Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0){struct Cyc_Absyn_Attributes_mod_struct*
_tmpDFF;struct Cyc_Absyn_Attributes_mod_struct _tmpDFE;struct Cyc_List_List*_tmpDFD;
ans=((_tmpDFD=_cycalloc(sizeof(*_tmpDFD)),((_tmpDFD->hd=(void*)((void*)((_tmpDFF=
_cycalloc(sizeof(*_tmpDFF)),((_tmpDFF[0]=((_tmpDFE.tag=5,((_tmpDFE.f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((_tmpDFE.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmpDFE)))))),_tmpDFF))))),((_tmpDFD->tl=ans,_tmpDFD))))));}{struct Cyc_Absyn_PtrLoc*
ptrloc=0;if(Cyc_Absyn_porting_c_code){struct Cyc_Absyn_PtrLoc*_tmpE00;ptrloc=((
_tmpE00=_cycalloc(sizeof(*_tmpE00)),((_tmpE00->ptr_loc=(*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f1,((_tmpE00->rgn_loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),((_tmpE00->zt_loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),_tmpE00))))))));}{
struct Cyc_Absyn_Pointer_mod_struct*_tmpE0A;struct Cyc_Absyn_PtrAtts _tmpE09;struct
Cyc_Absyn_Pointer_mod_struct _tmpE08;struct Cyc_List_List*_tmpE07;ans=((_tmpE07=
_cycalloc(sizeof(*_tmpE07)),((_tmpE07->hd=(void*)((void*)((_tmpE0A=_cycalloc(
sizeof(*_tmpE0A)),((_tmpE0A[0]=((_tmpE08.tag=2,((_tmpE08.f1=((_tmpE09.rgn=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((
_tmpE09.nullable=(*Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])).f2,((_tmpE09.bounds=(*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f3,((_tmpE09.zero_term=
Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((
_tmpE09.ptrloc=ptrloc,_tmpE09)))))))))),((_tmpE08.f2=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE08)))))),_tmpE0A))))),((
_tmpE07->tl=ans,_tmpE07))))));}{union Cyc_YYSTYPE_union _tmpE0B;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE0B.YY28).tag=32,(((_tmpE0B.YY28).f1=ans,_tmpE0B))));}break;}}case 179: _LL2B3: {
struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line): 0;{struct
_tuple12*_tmpE0E;union Cyc_YYSTYPE_union _tmpE0D;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE0D.YY1).tag=5,(((_tmpE0D.YY1).f1=((_tmpE0E=_cycalloc(sizeof(*_tmpE0E)),((
_tmpE0E->f1=loc,((_tmpE0E->f2=Cyc_Absyn_true_conref,((_tmpE0E->f3=Cyc_yyget_YY2(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE0E)))))))),
_tmpE0D))));}break;}case 180: _LL2B4: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line): 0;{struct _tuple12*_tmpE11;union Cyc_YYSTYPE_union
_tmpE10;yyval=(union Cyc_YYSTYPE_union)(((_tmpE10.YY1).tag=5,(((_tmpE10.YY1).f1=((
_tmpE11=_cycalloc(sizeof(*_tmpE11)),((_tmpE11->f1=loc,((_tmpE11->f2=Cyc_Absyn_false_conref,((
_tmpE11->f3=Cyc_yyget_YY2(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpE11)))))))),_tmpE10))));}break;}case 181: _LL2B5: {struct Cyc_Position_Segment*
loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line): 0;{struct
_tuple12*_tmpE14;union Cyc_YYSTYPE_union _tmpE13;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE13.YY1).tag=5,(((_tmpE13.YY1).f1=((_tmpE14=_cycalloc(sizeof(*_tmpE14)),((
_tmpE14->f1=loc,((_tmpE14->f2=Cyc_Absyn_true_conref,((_tmpE14->f3=Cyc_Absyn_bounds_dynforward_conref,
_tmpE14)))))))),_tmpE13))));}break;}case 182: _LL2B6: {struct Cyc_Position_Segment*
loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line): 0;{struct
_tuple12*_tmpE17;union Cyc_YYSTYPE_union _tmpE16;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE16.YY1).tag=5,(((_tmpE16.YY1).f1=((_tmpE17=_cycalloc(sizeof(*_tmpE17)),((
_tmpE17->f1=loc,((_tmpE17->f2=Cyc_Absyn_true_conref,((_tmpE17->f3=Cyc_Absyn_bounds_dyneither_conref,
_tmpE17)))))))),_tmpE16))));}break;}case 183: _LL2B7:{union Cyc_YYSTYPE_union
_tmpE18;yyval=(union Cyc_YYSTYPE_union)(((_tmpE18.YY2).tag=6,(((_tmpE18.YY2).f1=
Cyc_Absyn_bounds_one_conref,_tmpE18))));}break;case 184: _LL2B8:{struct Cyc_Absyn_Upper_b_struct*
_tmpE1E;struct Cyc_Absyn_Upper_b_struct _tmpE1D;union Cyc_YYSTYPE_union _tmpE1C;
yyval=(union Cyc_YYSTYPE_union)(((_tmpE1C.YY2).tag=6,(((_tmpE1C.YY2).f1=Cyc_Absyn_new_conref((
void*)((_tmpE1E=_cycalloc(sizeof(*_tmpE1E)),((_tmpE1E[0]=((_tmpE1D.tag=0,((
_tmpE1D.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]),_tmpE1D)))),_tmpE1E))))),_tmpE1C))));}break;case 185: _LL2B9:{struct Cyc_Absyn_AbsUpper_b_struct*
_tmpE24;struct Cyc_Absyn_AbsUpper_b_struct _tmpE23;union Cyc_YYSTYPE_union _tmpE22;
yyval=(union Cyc_YYSTYPE_union)(((_tmpE22.YY2).tag=6,(((_tmpE22.YY2).f1=Cyc_Absyn_new_conref((
void*)((_tmpE24=_cycalloc(sizeof(*_tmpE24)),((_tmpE24[0]=((_tmpE23.tag=1,((
_tmpE23.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpE23)))),_tmpE24))))),_tmpE22))));}break;case 186: _LL2BA:{
union Cyc_YYSTYPE_union _tmpE25;yyval=(union Cyc_YYSTYPE_union)(((_tmpE25.YY51).tag=
55,(((_tmpE25.YY51).f1=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),
_tmpE25))));}break;case 187: _LL2BB:{union Cyc_YYSTYPE_union _tmpE26;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE26.YY51).tag=55,(((_tmpE26.YY51).f1=Cyc_Absyn_true_conref,_tmpE26))));}
break;case 188: _LL2BC:{union Cyc_YYSTYPE_union _tmpE27;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE27.YY51).tag=55,(((_tmpE27.YY51).f1=Cyc_Absyn_false_conref,_tmpE27))));}
break;case 189: _LL2BD:{union Cyc_YYSTYPE_union _tmpE28;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE28.YY49).tag=53,(((_tmpE28.YY49).f1=0,_tmpE28))));}break;case 190: _LL2BE: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)
3,1);{struct Cyc_Core_Opt*_tmpE2B;union Cyc_YYSTYPE_union _tmpE2A;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE2A.YY49).tag=53,(((_tmpE2A.YY49).f1=((_tmpE2B=_cycalloc(sizeof(*_tmpE2B)),((
_tmpE2B->v=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpE2B)))),_tmpE2A))));}break;case 191: _LL2BF:{struct Cyc_Core_Opt*
_tmpE2E;union Cyc_YYSTYPE_union _tmpE2D;yyval=(union Cyc_YYSTYPE_union)(((_tmpE2D.YY49).tag=
53,(((_tmpE2D.YY49).f1=((_tmpE2E=_cycalloc(sizeof(*_tmpE2E)),((_tmpE2E->v=(void*)
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,0),_tmpE2E)))),_tmpE2D))));}
break;case 192: _LL2C0:{union Cyc_YYSTYPE_union _tmpE2F;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE2F.YY44).tag=48,(((_tmpE2F.YY44).f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,0),_tmpE2F))));}break;case 193: _LL2C1: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)
5,1);yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
194: _LL2C2:{union Cyc_YYSTYPE_union _tmpE30;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE30.YY44).tag=48,(((_tmpE30.YY44).f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,0),_tmpE30))));}break;case 195: _LL2C3:{union Cyc_YYSTYPE_union
_tmpE31;yyval=(union Cyc_YYSTYPE_union)(((_tmpE31.YY25).tag=29,(((_tmpE31.YY25).f1=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset + 1)]).last_line)),_tmpE31))));}
break;case 196: _LL2C4:{union Cyc_YYSTYPE_union _tmpE32;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE32.YY25).tag=29,(((_tmpE32.YY25).f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmpE32))));}break;case 197:
_LL2C5:{struct _tuple17*_tmpE35;union Cyc_YYSTYPE_union _tmpE34;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE34.YY39).tag=43,(((_tmpE34.YY39).f1=((_tmpE35=_cycalloc(sizeof(*_tmpE35)),((
_tmpE35->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((
_tmpE35->f2=0,((_tmpE35->f3=0,((_tmpE35->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpE35->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE35)))))))))))),
_tmpE34))));}break;case 198: _LL2C6:{struct _tuple17*_tmpE38;union Cyc_YYSTYPE_union
_tmpE37;yyval=(union Cyc_YYSTYPE_union)(((_tmpE37.YY39).tag=43,(((_tmpE37.YY39).f1=((
_tmpE38=_cycalloc(sizeof(*_tmpE38)),((_tmpE38->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])),((_tmpE38->f2=1,((
_tmpE38->f3=0,((_tmpE38->f4=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),((_tmpE38->f5=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE38)))))))))))),_tmpE37))));}
break;case 199: _LL2C7: {struct _tuple2 _tmp5D3;struct Cyc_Core_Opt*_tmp5D4;struct Cyc_Absyn_Tqual
_tmp5D5;void*_tmp5D6;struct _tuple2*_tmp5D2=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5D3=*_tmp5D2;_tmp5D4=
_tmp5D3.f1;_tmp5D5=_tmp5D3.f2;_tmp5D6=_tmp5D3.f3;{struct Cyc_Absyn_VarargInfo*
_tmpE39;struct Cyc_Absyn_VarargInfo*_tmp5D7=(_tmpE39=_cycalloc(sizeof(*_tmpE39)),((
_tmpE39->name=_tmp5D4,((_tmpE39->tq=_tmp5D5,((_tmpE39->type=(void*)_tmp5D6,((
_tmpE39->inject=Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),_tmpE39)))))))));{struct _tuple17*_tmpE3C;union Cyc_YYSTYPE_union
_tmpE3B;yyval=(union Cyc_YYSTYPE_union)(((_tmpE3B.YY39).tag=43,(((_tmpE3B.YY39).f1=((
_tmpE3C=_cycalloc(sizeof(*_tmpE3C)),((_tmpE3C->f1=0,((_tmpE3C->f2=0,((_tmpE3C->f3=
_tmp5D7,((_tmpE3C->f4=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmpE3C->f5=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE3C)))))))))))),_tmpE3B))));}
break;}}case 200: _LL2C8: {struct _tuple2 _tmp5DC;struct Cyc_Core_Opt*_tmp5DD;struct
Cyc_Absyn_Tqual _tmp5DE;void*_tmp5DF;struct _tuple2*_tmp5DB=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5DC=*_tmp5DB;_tmp5DD=
_tmp5DC.f1;_tmp5DE=_tmp5DC.f2;_tmp5DF=_tmp5DC.f3;{struct Cyc_Absyn_VarargInfo*
_tmpE3D;struct Cyc_Absyn_VarargInfo*_tmp5E0=(_tmpE3D=_cycalloc(sizeof(*_tmpE3D)),((
_tmpE3D->name=_tmp5DD,((_tmpE3D->tq=_tmp5DE,((_tmpE3D->type=(void*)_tmp5DF,((
_tmpE3D->inject=Cyc_yyget_YY31(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),_tmpE3D)))))))));{struct _tuple17*_tmpE40;union Cyc_YYSTYPE_union
_tmpE3F;yyval=(union Cyc_YYSTYPE_union)(((_tmpE3F.YY39).tag=43,(((_tmpE3F.YY39).f1=((
_tmpE40=_cycalloc(sizeof(*_tmpE40)),((_tmpE40->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)])),((_tmpE40->f2=0,((
_tmpE40->f3=_tmp5E0,((_tmpE40->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpE40->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE40)))))))))))),
_tmpE3F))));}break;}}case 201: _LL2C9:{struct Cyc_Absyn_Unknown_kb_struct*_tmpE46;
struct Cyc_Absyn_Unknown_kb_struct _tmpE45;union Cyc_YYSTYPE_union _tmpE44;yyval=(
union Cyc_YYSTYPE_union)(((_tmpE44.YY44).tag=48,(((_tmpE44.YY44).f1=(void*)Cyc_Parse_id2type(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(
void*)((_tmpE46=_cycalloc(sizeof(*_tmpE46)),((_tmpE46[0]=((_tmpE45.tag=1,((
_tmpE45.f1=0,_tmpE45)))),_tmpE46))))),_tmpE44))));}break;case 202: _LL2CA:{union
Cyc_YYSTYPE_union _tmpE47;yyval=(union Cyc_YYSTYPE_union)(((_tmpE47.YY44).tag=48,(((
_tmpE47.YY44).f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Tcutil_kind_to_bound(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))),
_tmpE47))));}break;case 203: _LL2CB:{union Cyc_YYSTYPE_union _tmpE48;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE48.YY49).tag=53,(((_tmpE48.YY49).f1=0,_tmpE48))));}break;case 204: _LL2CC:{
struct Cyc_Core_Opt*_tmpE52;struct Cyc_Absyn_JoinEff_struct _tmpE51;struct Cyc_Absyn_JoinEff_struct*
_tmpE50;union Cyc_YYSTYPE_union _tmpE4F;yyval=(union Cyc_YYSTYPE_union)(((_tmpE4F.YY49).tag=
53,(((_tmpE4F.YY49).f1=((_tmpE52=_cycalloc(sizeof(*_tmpE52)),((_tmpE52->v=(void*)((
void*)((_tmpE50=_cycalloc(sizeof(*_tmpE50)),((_tmpE50[0]=((_tmpE51.tag=21,((
_tmpE51.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmpE51)))),_tmpE50))))),_tmpE52)))),_tmpE4F))));}break;case 205: _LL2CD:{union Cyc_YYSTYPE_union
_tmpE53;yyval=(union Cyc_YYSTYPE_union)(((_tmpE53.YY50).tag=54,(((_tmpE53.YY50).f1=
0,_tmpE53))));}break;case 206: _LL2CE: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 207: _LL2CF:{struct Cyc_List_List*_tmpE6B;struct Cyc_Absyn_JoinEff_struct*
_tmpE6A;struct Cyc_Absyn_JoinEff_struct _tmpE69;struct Cyc_Absyn_Less_kb_struct*
_tmpE68;struct Cyc_Absyn_Less_kb_struct _tmpE67;struct _tuple4*_tmpE66;union Cyc_YYSTYPE_union
_tmpE65;yyval=(union Cyc_YYSTYPE_union)(((_tmpE65.YY50).tag=54,(((_tmpE65.YY50).f1=((
_tmpE6B=_cycalloc(sizeof(*_tmpE6B)),((_tmpE6B->hd=((_tmpE66=_cycalloc(sizeof(*
_tmpE66)),((_tmpE66->f1=(void*)((_tmpE6A=_cycalloc(sizeof(*_tmpE6A)),((_tmpE6A[0]=((
_tmpE69.tag=21,((_tmpE69.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),_tmpE69)))),_tmpE6A)))),((_tmpE66->f2=Cyc_Parse_id2type(
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(
void*)((_tmpE68=_cycalloc(sizeof(*_tmpE68)),((_tmpE68[0]=((_tmpE67.tag=2,((
_tmpE67.f1=0,((_tmpE67.f2=(void*)((void*)5),_tmpE67)))))),_tmpE68))))),_tmpE66)))))),((
_tmpE6B->tl=0,_tmpE6B)))))),_tmpE65))));}break;case 208: _LL2D0:{struct Cyc_List_List*
_tmpE83;struct Cyc_Absyn_JoinEff_struct*_tmpE82;struct Cyc_Absyn_JoinEff_struct
_tmpE81;struct Cyc_Absyn_Less_kb_struct*_tmpE80;struct Cyc_Absyn_Less_kb_struct
_tmpE7F;struct _tuple4*_tmpE7E;union Cyc_YYSTYPE_union _tmpE7D;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE7D.YY50).tag=54,(((_tmpE7D.YY50).f1=((_tmpE83=_cycalloc(sizeof(*_tmpE83)),((
_tmpE83->hd=((_tmpE7E=_cycalloc(sizeof(*_tmpE7E)),((_tmpE7E->f1=(void*)((_tmpE82=
_cycalloc(sizeof(*_tmpE82)),((_tmpE82[0]=((_tmpE81.tag=21,((_tmpE81.f1=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmpE81)))),_tmpE82)))),((
_tmpE7E->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)((_tmpE80=
_cycalloc(sizeof(*_tmpE80)),((_tmpE80[0]=((_tmpE7F.tag=2,((_tmpE7F.f1=0,((
_tmpE7F.f2=(void*)((void*)5),_tmpE7F)))))),_tmpE80))))),_tmpE7E)))))),((_tmpE83->tl=
Cyc_yyget_YY50(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE83)))))),
_tmpE7D))));}break;case 209: _LL2D1:{union Cyc_YYSTYPE_union _tmpE84;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE84.YY31).tag=35,(((_tmpE84.YY31).f1=0,_tmpE84))));}break;case 210: _LL2D2:{
const char*_tmpE85;if(Cyc_zstrcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpE85="inject",
_tag_dynforward(_tmpE85,sizeof(char),_get_zero_arr_size_char(_tmpE85,7)))))!= 0){
const char*_tmpE86;Cyc_Parse_err(((_tmpE86="missing type in function declaration",
_tag_dynforward(_tmpE86,sizeof(char),_get_zero_arr_size_char(_tmpE86,37)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}}{union Cyc_YYSTYPE_union
_tmpE87;yyval=(union Cyc_YYSTYPE_union)(((_tmpE87.YY31).tag=35,(((_tmpE87.YY31).f1=
1,_tmpE87))));}break;case 211: _LL2D3: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 212: _LL2D4:{union Cyc_YYSTYPE_union _tmpE88;yyval=(
union Cyc_YYSTYPE_union)(((_tmpE88.YY40).tag=44,(((_tmpE88.YY40).f1=Cyc_List_imp_append(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmpE88))));}break;
case 213: _LL2D5:{union Cyc_YYSTYPE_union _tmpE89;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE89.YY40).tag=44,(((_tmpE89.YY40).f1=0,_tmpE89))));}break;case 214: _LL2D6:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 215:
_LL2D7:{struct Cyc_List_List*_tmpE93;struct Cyc_Absyn_RgnsEff_struct _tmpE92;struct
Cyc_Absyn_RgnsEff_struct*_tmpE91;union Cyc_YYSTYPE_union _tmpE90;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE90.YY40).tag=44,(((_tmpE90.YY40).f1=((_tmpE93=_cycalloc(sizeof(*_tmpE93)),((
_tmpE93->hd=(void*)((void*)((_tmpE91=_cycalloc(sizeof(*_tmpE91)),((_tmpE91[0]=((
_tmpE92.tag=22,((_tmpE92.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpE92)))),_tmpE91))))),((
_tmpE93->tl=0,_tmpE93)))))),_tmpE90))));}break;case 216: _LL2D8: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)
6,0);{struct Cyc_List_List*_tmpE96;union Cyc_YYSTYPE_union _tmpE95;yyval=(union Cyc_YYSTYPE_union)(((
_tmpE95.YY40).tag=44,(((_tmpE95.YY40).f1=((_tmpE96=_cycalloc(sizeof(*_tmpE96)),((
_tmpE96->hd=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmpE96->tl=0,_tmpE96)))))),_tmpE95))));}break;case 217: _LL2D9:
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),(void*)5,1);{struct Cyc_List_List*_tmpEA0;struct Cyc_Absyn_AccessEff_struct
_tmpE9F;struct Cyc_Absyn_AccessEff_struct*_tmpE9E;union Cyc_YYSTYPE_union _tmpE9D;
yyval=(union Cyc_YYSTYPE_union)(((_tmpE9D.YY40).tag=44,(((_tmpE9D.YY40).f1=((
_tmpEA0=_cycalloc(sizeof(*_tmpEA0)),((_tmpEA0->hd=(void*)((void*)((_tmpE9E=
_cycalloc(sizeof(*_tmpE9E)),((_tmpE9E[0]=((_tmpE9F.tag=20,((_tmpE9F.f1=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpE9F)))),
_tmpE9E))))),((_tmpEA0->tl=0,_tmpEA0)))))),_tmpE9D))));}break;case 218: _LL2DA: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(
void*)5,1);{struct Cyc_List_List*_tmpEAA;struct Cyc_Absyn_AccessEff_struct _tmpEA9;
struct Cyc_Absyn_AccessEff_struct*_tmpEA8;union Cyc_YYSTYPE_union _tmpEA7;yyval=(
union Cyc_YYSTYPE_union)(((_tmpEA7.YY40).tag=44,(((_tmpEA7.YY40).f1=((_tmpEAA=
_cycalloc(sizeof(*_tmpEAA)),((_tmpEAA->hd=(void*)((void*)((_tmpEA8=_cycalloc(
sizeof(*_tmpEA8)),((_tmpEA8[0]=((_tmpEA9.tag=20,((_tmpEA9.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpEA9)))),_tmpEA8))))),((
_tmpEAA->tl=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpEAA)))))),_tmpEA7))));}break;case 219: _LL2DB:{struct Cyc_List_List*
_tmpEAD;union Cyc_YYSTYPE_union _tmpEAC;yyval=(union Cyc_YYSTYPE_union)(((_tmpEAC.YY38).tag=
42,(((_tmpEAC.YY38).f1=((_tmpEAD=_cycalloc(sizeof(*_tmpEAD)),((_tmpEAD->hd=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpEAD->tl=0,
_tmpEAD)))))),_tmpEAC))));}break;case 220: _LL2DC:{struct Cyc_List_List*_tmpEB0;
union Cyc_YYSTYPE_union _tmpEAF;yyval=(union Cyc_YYSTYPE_union)(((_tmpEAF.YY38).tag=
42,(((_tmpEAF.YY38).f1=((_tmpEB0=_cycalloc(sizeof(*_tmpEB0)),((_tmpEB0->hd=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpEB0->tl=Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmpEB0)))))),
_tmpEAF))));}break;case 221: _LL2DD: {struct _tuple16 _tmp615;struct Cyc_Absyn_Tqual
_tmp616;struct Cyc_List_List*_tmp617;struct Cyc_List_List*_tmp618;struct _tuple16*
_tmp614=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp615=*_tmp614;_tmp616=_tmp615.f1;_tmp617=_tmp615.f2;_tmp618=_tmp615.f3;if(
_tmp616.loc == 0)_tmp616.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{struct Cyc_Parse_Declarator
_tmp61A;struct _tuple1*_tmp61B;struct Cyc_List_List*_tmp61C;struct Cyc_Parse_Declarator*
_tmp619=Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp61A=*_tmp619;_tmp61B=_tmp61A.id;_tmp61C=_tmp61A.tms;{void*_tmp61D=Cyc_Parse_speclist2typ(
_tmp617,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp61F;void*_tmp620;struct
Cyc_List_List*_tmp621;struct Cyc_List_List*_tmp622;struct _tuple6 _tmp61E=Cyc_Parse_apply_tms(
_tmp616,_tmp61D,_tmp618,_tmp61C);_tmp61F=_tmp61E.f1;_tmp620=_tmp61E.f2;_tmp621=
_tmp61E.f3;_tmp622=_tmp61E.f4;if(_tmp621 != 0){const char*_tmpEB1;Cyc_Parse_err(((
_tmpEB1="parameter with bad type params",_tag_dynforward(_tmpEB1,sizeof(char),
_get_zero_arr_size_char(_tmpEB1,31)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}if(Cyc_Absyn_is_qvar_qualified(
_tmp61B)){const char*_tmpEB2;Cyc_Parse_err(((_tmpEB2="parameter cannot be qualified with a namespace",
_tag_dynforward(_tmpEB2,sizeof(char),_get_zero_arr_size_char(_tmpEB2,47)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}{struct Cyc_Core_Opt*
_tmpEB3;struct Cyc_Core_Opt*_tmp625=(struct Cyc_Core_Opt*)((_tmpEB3=_cycalloc(
sizeof(*_tmpEB3)),((_tmpEB3->v=(*_tmp61B).f2,_tmpEB3))));if(_tmp622 != 0){const
char*_tmpEB6;void*_tmpEB5;(_tmpEB5=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpEB6="extra attributes on parameter, ignoring",
_tag_dynforward(_tmpEB6,sizeof(char),_get_zero_arr_size_char(_tmpEB6,40)))),
_tag_dynforward(_tmpEB5,sizeof(void*),0)));}{struct _tuple2*_tmpEB9;union Cyc_YYSTYPE_union
_tmpEB8;yyval=(union Cyc_YYSTYPE_union)(((_tmpEB8.YY37).tag=41,(((_tmpEB8.YY37).f1=((
_tmpEB9=_cycalloc(sizeof(*_tmpEB9)),((_tmpEB9->f1=_tmp625,((_tmpEB9->f2=_tmp61F,((
_tmpEB9->f3=_tmp620,_tmpEB9)))))))),_tmpEB8))));}break;}}}}case 222: _LL2DE: {
struct _tuple16 _tmp62C;struct Cyc_Absyn_Tqual _tmp62D;struct Cyc_List_List*_tmp62E;
struct Cyc_List_List*_tmp62F;struct _tuple16*_tmp62B=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp62C=*_tmp62B;_tmp62D=
_tmp62C.f1;_tmp62E=_tmp62C.f2;_tmp62F=_tmp62C.f3;if(_tmp62D.loc == 0)_tmp62D.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);{void*_tmp630=Cyc_Parse_speclist2typ(_tmp62E,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp62F != 0){
const char*_tmpEBC;void*_tmpEBB;(_tmpEBB=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpEBC="bad attributes on parameter, ignoring",
_tag_dynforward(_tmpEBC,sizeof(char),_get_zero_arr_size_char(_tmpEBC,38)))),
_tag_dynforward(_tmpEBB,sizeof(void*),0)));}{struct _tuple2*_tmpEBF;union Cyc_YYSTYPE_union
_tmpEBE;yyval=(union Cyc_YYSTYPE_union)(((_tmpEBE.YY37).tag=41,(((_tmpEBE.YY37).f1=((
_tmpEBF=_cycalloc(sizeof(*_tmpEBF)),((_tmpEBF->f1=0,((_tmpEBF->f2=_tmp62D,((
_tmpEBF->f3=_tmp630,_tmpEBF)))))))),_tmpEBE))));}break;}}case 223: _LL2DF: {struct
_tuple16 _tmp636;struct Cyc_Absyn_Tqual _tmp637;struct Cyc_List_List*_tmp638;struct
Cyc_List_List*_tmp639;struct _tuple16*_tmp635=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp636=*_tmp635;_tmp637=
_tmp636.f1;_tmp638=_tmp636.f2;_tmp639=_tmp636.f3;if(_tmp637.loc == 0)_tmp637.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{void*_tmp63A=Cyc_Parse_speclist2typ(_tmp638,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp63B=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct Cyc_Absyn_Tqual _tmp63D;void*_tmp63E;struct Cyc_List_List*_tmp63F;struct Cyc_List_List*
_tmp640;struct _tuple6 _tmp63C=Cyc_Parse_apply_tms(_tmp637,_tmp63A,_tmp639,_tmp63B);
_tmp63D=_tmp63C.f1;_tmp63E=_tmp63C.f2;_tmp63F=_tmp63C.f3;_tmp640=_tmp63C.f4;if(
_tmp63F != 0){const char*_tmpEC2;void*_tmpEC1;(_tmpEC1=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpEC2="bad type parameters on formal argument, ignoring",
_tag_dynforward(_tmpEC2,sizeof(char),_get_zero_arr_size_char(_tmpEC2,49)))),
_tag_dynforward(_tmpEC1,sizeof(void*),0)));}if(_tmp640 != 0){const char*_tmpEC5;
void*_tmpEC4;(_tmpEC4=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpEC5="bad attributes on parameter, ignoring",
_tag_dynforward(_tmpEC5,sizeof(char),_get_zero_arr_size_char(_tmpEC5,38)))),
_tag_dynforward(_tmpEC4,sizeof(void*),0)));}{struct _tuple2*_tmpEC8;union Cyc_YYSTYPE_union
_tmpEC7;yyval=(union Cyc_YYSTYPE_union)(((_tmpEC7.YY37).tag=41,(((_tmpEC7.YY37).f1=((
_tmpEC8=_cycalloc(sizeof(*_tmpEC8)),((_tmpEC8->f1=0,((_tmpEC8->f2=_tmp63D,((
_tmpEC8->f3=_tmp63E,_tmpEC8)))))))),_tmpEC7))));}break;}}case 224: _LL2E0:{union
Cyc_YYSTYPE_union _tmpEC9;yyval=(union Cyc_YYSTYPE_union)(((_tmpEC9.YY36).tag=40,(((
_tmpEC9.YY36).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),
_tmpEC9))));}break;case 225: _LL2E1:{struct Cyc_List_List*_tmpECF;struct
_dynforward_ptr*_tmpECE;union Cyc_YYSTYPE_union _tmpECD;yyval=(union Cyc_YYSTYPE_union)(((
_tmpECD.YY36).tag=40,(((_tmpECD.YY36).f1=((_tmpECF=_cycalloc(sizeof(*_tmpECF)),((
_tmpECF->hd=((_tmpECE=_cycalloc(sizeof(*_tmpECE)),((_tmpECE[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpECE)))),((_tmpECF->tl=
0,_tmpECF)))))),_tmpECD))));}break;case 226: _LL2E2:{struct Cyc_List_List*_tmpED5;
struct _dynforward_ptr*_tmpED4;union Cyc_YYSTYPE_union _tmpED3;yyval=(union Cyc_YYSTYPE_union)(((
_tmpED3.YY36).tag=40,(((_tmpED3.YY36).f1=((_tmpED5=_cycalloc(sizeof(*_tmpED5)),((
_tmpED5->hd=((_tmpED4=_cycalloc(sizeof(*_tmpED4)),((_tmpED4[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpED4)))),((_tmpED5->tl=
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmpED5)))))),_tmpED3))));}break;case 227: _LL2E3: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 228: _LL2E4: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 229: _LL2E5:{
struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpEDB;struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpEDA;union Cyc_YYSTYPE_union _tmpED9;yyval=(union Cyc_YYSTYPE_union)(((_tmpED9.YY3).tag=
7,(((_tmpED9.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmpEDB=_cycalloc(sizeof(*
_tmpEDB)),((_tmpEDB[0]=((_tmpEDA.tag=37,((_tmpEDA.f1=0,((_tmpEDA.f2=0,_tmpEDA)))))),
_tmpEDB)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpED9))));}break;case 230: _LL2E6:{struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmpEE1;struct Cyc_Absyn_UnresolvedMem_e_struct _tmpEE0;union Cyc_YYSTYPE_union
_tmpEDF;yyval=(union Cyc_YYSTYPE_union)(((_tmpEDF.YY3).tag=7,(((_tmpEDF.YY3).f1=
Cyc_Absyn_new_exp((void*)((_tmpEE1=_cycalloc(sizeof(*_tmpEE1)),((_tmpEE1[0]=((
_tmpEE0.tag=37,((_tmpEE0.f1=0,((_tmpEE0.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])),_tmpEE0)))))),_tmpEE1)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpEDF))));}
break;case 231: _LL2E7:{struct Cyc_Absyn_UnresolvedMem_e_struct*_tmpEE7;struct Cyc_Absyn_UnresolvedMem_e_struct
_tmpEE6;union Cyc_YYSTYPE_union _tmpEE5;yyval=(union Cyc_YYSTYPE_union)(((_tmpEE5.YY3).tag=
7,(((_tmpEE5.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmpEE7=_cycalloc(sizeof(*
_tmpEE7)),((_tmpEE7[0]=((_tmpEE6.tag=37,((_tmpEE6.f1=0,((_tmpEE6.f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),_tmpEE6)))))),_tmpEE7)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmpEE5))));}break;case 232: _LL2E8: {union Cyc_Absyn_Nmspace_union
_tmpEEC;struct _dynforward_ptr*_tmpEEB;struct _tuple1*_tmpEEA;struct Cyc_Absyn_Vardecl*
_tmp657=Cyc_Absyn_new_vardecl(((_tmpEEA=_cycalloc(sizeof(*_tmpEEA)),((_tmpEEA->f1=(
union Cyc_Absyn_Nmspace_union)(((_tmpEEC.Loc_n).tag=0,_tmpEEC)),((_tmpEEA->f2=((
_tmpEEB=_cycalloc(sizeof(*_tmpEEB)),((_tmpEEB[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),_tmpEEB)))),_tmpEEA)))))),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp657->tq).real_const=
1;{struct Cyc_Absyn_Comprehension_e_struct*_tmpEF2;struct Cyc_Absyn_Comprehension_e_struct
_tmpEF1;union Cyc_YYSTYPE_union _tmpEF0;yyval=(union Cyc_YYSTYPE_union)(((_tmpEF0.YY3).tag=
7,(((_tmpEF0.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmpEF2=_cycalloc(sizeof(*
_tmpEF2)),((_tmpEF2[0]=((_tmpEF1.tag=29,((_tmpEF1.f1=_tmp657,((_tmpEF1.f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmpEF1.f3=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpEF1.f4=0,
_tmpEF1)))))))))),_tmpEF2)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmpEF0))));}
break;}case 233: _LL2E9:{struct Cyc_List_List*_tmpEF8;struct _tuple20*_tmpEF7;union
Cyc_YYSTYPE_union _tmpEF6;yyval=(union Cyc_YYSTYPE_union)(((_tmpEF6.YY6).tag=10,(((
_tmpEF6.YY6).f1=((_tmpEF8=_cycalloc(sizeof(*_tmpEF8)),((_tmpEF8->hd=((_tmpEF7=
_cycalloc(sizeof(*_tmpEF7)),((_tmpEF7->f1=0,((_tmpEF7->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEF7)))))),((_tmpEF8->tl=
0,_tmpEF8)))))),_tmpEF6))));}break;case 234: _LL2EA:{struct Cyc_List_List*_tmpEFE;
struct _tuple20*_tmpEFD;union Cyc_YYSTYPE_union _tmpEFC;yyval=(union Cyc_YYSTYPE_union)(((
_tmpEFC.YY6).tag=10,(((_tmpEFC.YY6).f1=((_tmpEFE=_cycalloc(sizeof(*_tmpEFE)),((
_tmpEFE->hd=((_tmpEFD=_cycalloc(sizeof(*_tmpEFD)),((_tmpEFD->f1=Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpEFD->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpEFD)))))),((
_tmpEFE->tl=0,_tmpEFE)))))),_tmpEFC))));}break;case 235: _LL2EB:{struct Cyc_List_List*
_tmpF04;struct _tuple20*_tmpF03;union Cyc_YYSTYPE_union _tmpF02;yyval=(union Cyc_YYSTYPE_union)(((
_tmpF02.YY6).tag=10,(((_tmpF02.YY6).f1=((_tmpF04=_cycalloc(sizeof(*_tmpF04)),((
_tmpF04->hd=((_tmpF03=_cycalloc(sizeof(*_tmpF03)),((_tmpF03->f1=0,((_tmpF03->f2=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpF03)))))),((
_tmpF04->tl=Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]),_tmpF04)))))),_tmpF02))));}break;case 236: _LL2EC:{struct Cyc_List_List*
_tmpF0A;struct _tuple20*_tmpF09;union Cyc_YYSTYPE_union _tmpF08;yyval=(union Cyc_YYSTYPE_union)(((
_tmpF08.YY6).tag=10,(((_tmpF08.YY6).f1=((_tmpF0A=_cycalloc(sizeof(*_tmpF0A)),((
_tmpF0A->hd=((_tmpF09=_cycalloc(sizeof(*_tmpF09)),((_tmpF09->f1=Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmpF09->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpF09)))))),((
_tmpF0A->tl=Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 3)]),_tmpF0A)))))),_tmpF08))));}break;case 237: _LL2ED:{union Cyc_YYSTYPE_union
_tmpF0B;yyval=(union Cyc_YYSTYPE_union)(((_tmpF0B.YY41).tag=45,(((_tmpF0B.YY41).f1=
Cyc_List_imp_rev(Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])),_tmpF0B))));}break;case 238: _LL2EE:{struct Cyc_List_List*
_tmpF0E;union Cyc_YYSTYPE_union _tmpF0D;yyval=(union Cyc_YYSTYPE_union)(((_tmpF0D.YY41).tag=
45,(((_tmpF0D.YY41).f1=((_tmpF0E=_cycalloc(sizeof(*_tmpF0E)),((_tmpF0E->hd=(void*)
Cyc_yyget_YY42(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpF0E->tl=0,_tmpF0E)))))),_tmpF0D))));}break;case 239: _LL2EF:{struct Cyc_List_List*
_tmpF11;union Cyc_YYSTYPE_union _tmpF10;yyval=(union Cyc_YYSTYPE_union)(((_tmpF10.YY41).tag=
45,(((_tmpF10.YY41).f1=((_tmpF11=_cycalloc(sizeof(*_tmpF11)),((_tmpF11->hd=(void*)
Cyc_yyget_YY42(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpF11->tl=Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmpF11)))))),_tmpF10))));}break;case 240: _LL2F0:{struct Cyc_Absyn_ArrayElement_struct*
_tmpF17;struct Cyc_Absyn_ArrayElement_struct _tmpF16;union Cyc_YYSTYPE_union _tmpF15;
yyval=(union Cyc_YYSTYPE_union)(((_tmpF15.YY42).tag=46,(((_tmpF15.YY42).f1=(void*)((
void*)((_tmpF17=_cycalloc(sizeof(*_tmpF17)),((_tmpF17[0]=((_tmpF16.tag=0,((
_tmpF16.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]),_tmpF16)))),_tmpF17))))),_tmpF15))));}break;case 241: _LL2F1:{struct Cyc_Absyn_FieldName_struct*
_tmpF21;struct _dynforward_ptr*_tmpF20;struct Cyc_Absyn_FieldName_struct _tmpF1F;
union Cyc_YYSTYPE_union _tmpF1E;yyval=(union Cyc_YYSTYPE_union)(((_tmpF1E.YY42).tag=
46,(((_tmpF1E.YY42).f1=(void*)((void*)((_tmpF21=_cycalloc(sizeof(*_tmpF21)),((
_tmpF21[0]=((_tmpF1F.tag=1,((_tmpF1F.f1=((_tmpF20=_cycalloc(sizeof(*_tmpF20)),((
_tmpF20[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmpF20)))),_tmpF1F)))),_tmpF21))))),_tmpF1E))));}break;case 242:
_LL2F2: {struct _tuple16 _tmp677;struct Cyc_Absyn_Tqual _tmp678;struct Cyc_List_List*
_tmp679;struct Cyc_List_List*_tmp67A;struct _tuple16*_tmp676=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp677=*_tmp676;_tmp678=
_tmp677.f1;_tmp679=_tmp677.f2;_tmp67A=_tmp677.f3;{void*_tmp67B=Cyc_Parse_speclist2typ(
_tmp679,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));if(_tmp67A != 0){const char*_tmpF24;void*_tmpF23;(
_tmpF23=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpF24="ignoring attributes in type",
_tag_dynforward(_tmpF24,sizeof(char),_get_zero_arr_size_char(_tmpF24,28)))),
_tag_dynforward(_tmpF23,sizeof(void*),0)));}{struct _tuple2*_tmpF27;union Cyc_YYSTYPE_union
_tmpF26;yyval=(union Cyc_YYSTYPE_union)(((_tmpF26.YY37).tag=41,(((_tmpF26.YY37).f1=((
_tmpF27=_cycalloc(sizeof(*_tmpF27)),((_tmpF27->f1=0,((_tmpF27->f2=_tmp678,((
_tmpF27->f3=_tmp67B,_tmpF27)))))))),_tmpF26))));}break;}}case 243: _LL2F3: {struct
_tuple16 _tmp681;struct Cyc_Absyn_Tqual _tmp682;struct Cyc_List_List*_tmp683;struct
Cyc_List_List*_tmp684;struct _tuple16*_tmp680=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp681=*_tmp680;_tmp682=
_tmp681.f1;_tmp683=_tmp681.f2;_tmp684=_tmp681.f3;{void*_tmp685=Cyc_Parse_speclist2typ(
_tmp683,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp686=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;struct _tuple6 _tmp687=
Cyc_Parse_apply_tms(_tmp682,_tmp685,_tmp684,_tmp686);if(_tmp687.f3 != 0){const
char*_tmpF2A;void*_tmpF29;(_tmpF29=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpF2A="bad type params, ignoring",
_tag_dynforward(_tmpF2A,sizeof(char),_get_zero_arr_size_char(_tmpF2A,26)))),
_tag_dynforward(_tmpF29,sizeof(void*),0)));}if(_tmp687.f4 != 0){const char*_tmpF2D;
void*_tmpF2C;(_tmpF2C=0,Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmpF2D="bad specifiers, ignoring",
_tag_dynforward(_tmpF2D,sizeof(char),_get_zero_arr_size_char(_tmpF2D,25)))),
_tag_dynforward(_tmpF2C,sizeof(void*),0)));}{struct _tuple2*_tmpF30;union Cyc_YYSTYPE_union
_tmpF2F;yyval=(union Cyc_YYSTYPE_union)(((_tmpF2F.YY37).tag=41,(((_tmpF2F.YY37).f1=((
_tmpF30=_cycalloc(sizeof(*_tmpF30)),((_tmpF30->f1=0,((_tmpF30->f2=_tmp687.f1,((
_tmpF30->f3=_tmp687.f2,_tmpF30)))))))),_tmpF2F))));}break;}}case 244: _LL2F4:{
union Cyc_YYSTYPE_union _tmpF31;yyval=(union Cyc_YYSTYPE_union)(((_tmpF31.YY44).tag=
48,(((_tmpF31.YY44).f1=(void*)(*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3,_tmpF31))));}break;case
245: _LL2F5:{struct Cyc_Absyn_JoinEff_struct*_tmpF37;struct Cyc_Absyn_JoinEff_struct
_tmpF36;union Cyc_YYSTYPE_union _tmpF35;yyval=(union Cyc_YYSTYPE_union)(((_tmpF35.YY44).tag=
48,(((_tmpF35.YY44).f1=(void*)((void*)((_tmpF37=_cycalloc(sizeof(*_tmpF37)),((
_tmpF37[0]=((_tmpF36.tag=21,((_tmpF36.f1=0,_tmpF36)))),_tmpF37))))),_tmpF35))));}
break;case 246: _LL2F6:{struct Cyc_Absyn_JoinEff_struct*_tmpF3D;struct Cyc_Absyn_JoinEff_struct
_tmpF3C;union Cyc_YYSTYPE_union _tmpF3B;yyval=(union Cyc_YYSTYPE_union)(((_tmpF3B.YY44).tag=
48,(((_tmpF3B.YY44).f1=(void*)((void*)((_tmpF3D=_cycalloc(sizeof(*_tmpF3D)),((
_tmpF3D[0]=((_tmpF3C.tag=21,((_tmpF3C.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpF3C)))),_tmpF3D))))),
_tmpF3B))));}break;case 247: _LL2F7:{struct Cyc_Absyn_RgnsEff_struct*_tmpF43;struct
Cyc_Absyn_RgnsEff_struct _tmpF42;union Cyc_YYSTYPE_union _tmpF41;yyval=(union Cyc_YYSTYPE_union)(((
_tmpF41.YY44).tag=48,(((_tmpF41.YY44).f1=(void*)((void*)((_tmpF43=_cycalloc(
sizeof(*_tmpF43)),((_tmpF43[0]=((_tmpF42.tag=22,((_tmpF42.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpF42)))),_tmpF43))))),
_tmpF41))));}break;case 248: _LL2F8:{struct Cyc_Absyn_JoinEff_struct*_tmpF4D;struct
Cyc_List_List*_tmpF4C;struct Cyc_Absyn_JoinEff_struct _tmpF4B;union Cyc_YYSTYPE_union
_tmpF4A;yyval=(union Cyc_YYSTYPE_union)(((_tmpF4A.YY44).tag=48,(((_tmpF4A.YY44).f1=(
void*)((void*)((_tmpF4D=_cycalloc(sizeof(*_tmpF4D)),((_tmpF4D[0]=((_tmpF4B.tag=
21,((_tmpF4B.f1=((_tmpF4C=_cycalloc(sizeof(*_tmpF4C)),((_tmpF4C->hd=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpF4C->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpF4C)))))),_tmpF4B)))),
_tmpF4D))))),_tmpF4A))));}break;case 249: _LL2F9:{struct Cyc_List_List*_tmpF50;
union Cyc_YYSTYPE_union _tmpF4F;yyval=(union Cyc_YYSTYPE_union)(((_tmpF4F.YY40).tag=
44,(((_tmpF4F.YY40).f1=((_tmpF50=_cycalloc(sizeof(*_tmpF50)),((_tmpF50->hd=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpF50->tl=0,_tmpF50)))))),_tmpF4F))));}break;case 250: _LL2FA:{struct Cyc_List_List*
_tmpF53;union Cyc_YYSTYPE_union _tmpF52;yyval=(union Cyc_YYSTYPE_union)(((_tmpF52.YY40).tag=
44,(((_tmpF52.YY40).f1=((_tmpF53=_cycalloc(sizeof(*_tmpF53)),((_tmpF53->hd=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((
_tmpF53->tl=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),_tmpF53)))))),_tmpF52))));}break;case 251: _LL2FB:{struct Cyc_Parse_Abstractdeclarator*
_tmpF56;union Cyc_YYSTYPE_union _tmpF55;yyval=(union Cyc_YYSTYPE_union)(((_tmpF55.YY30).tag=
34,(((_tmpF55.YY30).f1=((_tmpF56=_cycalloc(sizeof(*_tmpF56)),((_tmpF56->tms=Cyc_yyget_YY28(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmpF56)))),_tmpF55))));}
break;case 252: _LL2FC: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 253: _LL2FD:{struct Cyc_Parse_Abstractdeclarator*_tmpF59;
union Cyc_YYSTYPE_union _tmpF58;yyval=(union Cyc_YYSTYPE_union)(((_tmpF58.YY30).tag=
34,(((_tmpF58.YY30).f1=((_tmpF59=_cycalloc(sizeof(*_tmpF59)),((_tmpF59->tms=Cyc_List_imp_append(
Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY30(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms),_tmpF59)))),
_tmpF58))));}break;case 254: _LL2FE: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;case 255: _LL2FF:{struct Cyc_Parse_Abstractdeclarator*
_tmpF68;struct Cyc_Absyn_Carray_mod_struct*_tmpF67;struct Cyc_Absyn_Carray_mod_struct
_tmpF66;struct Cyc_List_List*_tmpF65;union Cyc_YYSTYPE_union _tmpF64;yyval=(union
Cyc_YYSTYPE_union)(((_tmpF64.YY30).tag=34,(((_tmpF64.YY30).f1=((_tmpF68=
_cycalloc(sizeof(*_tmpF68)),((_tmpF68->tms=((_tmpF65=_cycalloc(sizeof(*_tmpF65)),((
_tmpF65->hd=(void*)((void*)((_tmpF67=_cycalloc(sizeof(*_tmpF67)),((_tmpF67[0]=((
_tmpF66.tag=0,((_tmpF66.f1=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),((_tmpF66.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpF66)))))),
_tmpF67))))),((_tmpF65->tl=0,_tmpF65)))))),_tmpF68)))),_tmpF64))));}break;case
256: _LL300:{struct Cyc_Parse_Abstractdeclarator*_tmpF77;struct Cyc_Absyn_Carray_mod_struct*
_tmpF76;struct Cyc_Absyn_Carray_mod_struct _tmpF75;struct Cyc_List_List*_tmpF74;
union Cyc_YYSTYPE_union _tmpF73;yyval=(union Cyc_YYSTYPE_union)(((_tmpF73.YY30).tag=
34,(((_tmpF73.YY30).f1=((_tmpF77=_cycalloc(sizeof(*_tmpF77)),((_tmpF77->tms=((
_tmpF74=_cycalloc(sizeof(*_tmpF74)),((_tmpF74->hd=(void*)((void*)((_tmpF76=
_cycalloc(sizeof(*_tmpF76)),((_tmpF76[0]=((_tmpF75.tag=0,((_tmpF75.f1=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpF75.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpF75)))))),
_tmpF76))))),((_tmpF74->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmpF74)))))),_tmpF77)))),_tmpF73))));}break;case
257: _LL301:{struct Cyc_Parse_Abstractdeclarator*_tmpF86;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpF85;struct Cyc_Absyn_ConstArray_mod_struct _tmpF84;struct Cyc_List_List*_tmpF83;
union Cyc_YYSTYPE_union _tmpF82;yyval=(union Cyc_YYSTYPE_union)(((_tmpF82.YY30).tag=
34,(((_tmpF82.YY30).f1=((_tmpF86=_cycalloc(sizeof(*_tmpF86)),((_tmpF86->tms=((
_tmpF83=_cycalloc(sizeof(*_tmpF83)),((_tmpF83->hd=(void*)((void*)((_tmpF85=
_cycalloc(sizeof(*_tmpF85)),((_tmpF85[0]=((_tmpF84.tag=1,((_tmpF84.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpF84.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpF84.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpF84)))))))),
_tmpF85))))),((_tmpF83->tl=0,_tmpF83)))))),_tmpF86)))),_tmpF82))));}break;case
258: _LL302:{struct Cyc_Parse_Abstractdeclarator*_tmpF95;struct Cyc_Absyn_ConstArray_mod_struct*
_tmpF94;struct Cyc_Absyn_ConstArray_mod_struct _tmpF93;struct Cyc_List_List*_tmpF92;
union Cyc_YYSTYPE_union _tmpF91;yyval=(union Cyc_YYSTYPE_union)(((_tmpF91.YY30).tag=
34,(((_tmpF91.YY30).f1=((_tmpF95=_cycalloc(sizeof(*_tmpF95)),((_tmpF95->tms=((
_tmpF92=_cycalloc(sizeof(*_tmpF92)),((_tmpF92->hd=(void*)((void*)((_tmpF94=
_cycalloc(sizeof(*_tmpF94)),((_tmpF94[0]=((_tmpF93.tag=1,((_tmpF93.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpF93.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmpF93.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmpF93)))))))),
_tmpF94))))),((_tmpF92->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms,_tmpF92)))))),_tmpF95)))),_tmpF91))));}break;case
259: _LL303:{struct Cyc_Parse_Abstractdeclarator*_tmpFB1;struct Cyc_Absyn_Function_mod_struct*
_tmpFB0;struct Cyc_Absyn_WithTypes_struct*_tmpFAF;struct Cyc_Absyn_WithTypes_struct
_tmpFAE;struct Cyc_Absyn_Function_mod_struct _tmpFAD;struct Cyc_List_List*_tmpFAC;
union Cyc_YYSTYPE_union _tmpFAB;yyval=(union Cyc_YYSTYPE_union)(((_tmpFAB.YY30).tag=
34,(((_tmpFAB.YY30).f1=((_tmpFB1=_cycalloc(sizeof(*_tmpFB1)),((_tmpFB1->tms=((
_tmpFAC=_cycalloc(sizeof(*_tmpFAC)),((_tmpFAC->hd=(void*)((void*)((_tmpFB0=
_cycalloc(sizeof(*_tmpFB0)),((_tmpFB0[0]=((_tmpFAD.tag=3,((_tmpFAD.f1=(void*)((
void*)((_tmpFAF=_cycalloc(sizeof(*_tmpFAF)),((_tmpFAF[0]=((_tmpFAE.tag=1,((
_tmpFAE.f1=0,((_tmpFAE.f2=0,((_tmpFAE.f3=0,((_tmpFAE.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpFAE.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpFAE)))))))))))),
_tmpFAF))))),_tmpFAD)))),_tmpFB0))))),((_tmpFAC->tl=0,_tmpFAC)))))),_tmpFB1)))),
_tmpFAB))));}break;case 260: _LL304: {struct _tuple17 _tmp6C0;struct Cyc_List_List*
_tmp6C1;int _tmp6C2;struct Cyc_Absyn_VarargInfo*_tmp6C3;struct Cyc_Core_Opt*_tmp6C4;
struct Cyc_List_List*_tmp6C5;struct _tuple17*_tmp6BF=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6C0=*_tmp6BF;_tmp6C1=
_tmp6C0.f1;_tmp6C2=_tmp6C0.f2;_tmp6C3=_tmp6C0.f3;_tmp6C4=_tmp6C0.f4;_tmp6C5=
_tmp6C0.f5;{struct Cyc_Parse_Abstractdeclarator*_tmpFCD;struct Cyc_Absyn_Function_mod_struct*
_tmpFCC;struct Cyc_Absyn_WithTypes_struct*_tmpFCB;struct Cyc_Absyn_WithTypes_struct
_tmpFCA;struct Cyc_Absyn_Function_mod_struct _tmpFC9;struct Cyc_List_List*_tmpFC8;
union Cyc_YYSTYPE_union _tmpFC7;yyval=(union Cyc_YYSTYPE_union)(((_tmpFC7.YY30).tag=
34,(((_tmpFC7.YY30).f1=((_tmpFCD=_cycalloc(sizeof(*_tmpFCD)),((_tmpFCD->tms=((
_tmpFC8=_cycalloc(sizeof(*_tmpFC8)),((_tmpFC8->hd=(void*)((void*)((_tmpFCC=
_cycalloc(sizeof(*_tmpFCC)),((_tmpFCC[0]=((_tmpFC9.tag=3,((_tmpFC9.f1=(void*)((
void*)((_tmpFCB=_cycalloc(sizeof(*_tmpFCB)),((_tmpFCB[0]=((_tmpFCA.tag=1,((
_tmpFCA.f1=_tmp6C1,((_tmpFCA.f2=_tmp6C2,((_tmpFCA.f3=_tmp6C3,((_tmpFCA.f4=
_tmp6C4,((_tmpFCA.f5=_tmp6C5,_tmpFCA)))))))))))),_tmpFCB))))),_tmpFC9)))),
_tmpFCC))))),((_tmpFC8->tl=0,_tmpFC8)))))),_tmpFCD)))),_tmpFC7))));}break;}case
261: _LL305:{struct Cyc_Parse_Abstractdeclarator*_tmpFE9;struct Cyc_Absyn_Function_mod_struct*
_tmpFE8;struct Cyc_Absyn_WithTypes_struct*_tmpFE7;struct Cyc_Absyn_WithTypes_struct
_tmpFE6;struct Cyc_Absyn_Function_mod_struct _tmpFE5;struct Cyc_List_List*_tmpFE4;
union Cyc_YYSTYPE_union _tmpFE3;yyval=(union Cyc_YYSTYPE_union)(((_tmpFE3.YY30).tag=
34,(((_tmpFE3.YY30).f1=((_tmpFE9=_cycalloc(sizeof(*_tmpFE9)),((_tmpFE9->tms=((
_tmpFE4=_cycalloc(sizeof(*_tmpFE4)),((_tmpFE4->hd=(void*)((void*)((_tmpFE8=
_cycalloc(sizeof(*_tmpFE8)),((_tmpFE8[0]=((_tmpFE5.tag=3,((_tmpFE5.f1=(void*)((
void*)((_tmpFE7=_cycalloc(sizeof(*_tmpFE7)),((_tmpFE7[0]=((_tmpFE6.tag=1,((
_tmpFE6.f1=0,((_tmpFE6.f2=0,((_tmpFE6.f3=0,((_tmpFE6.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmpFE6.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmpFE6)))))))))))),
_tmpFE7))))),_tmpFE5)))),_tmpFE8))))),((_tmpFE4->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms,_tmpFE4)))))),
_tmpFE9)))),_tmpFE3))));}break;case 262: _LL306: {struct _tuple17 _tmp6D5;struct Cyc_List_List*
_tmp6D6;int _tmp6D7;struct Cyc_Absyn_VarargInfo*_tmp6D8;struct Cyc_Core_Opt*_tmp6D9;
struct Cyc_List_List*_tmp6DA;struct _tuple17*_tmp6D4=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6D5=*_tmp6D4;_tmp6D6=
_tmp6D5.f1;_tmp6D7=_tmp6D5.f2;_tmp6D8=_tmp6D5.f3;_tmp6D9=_tmp6D5.f4;_tmp6DA=
_tmp6D5.f5;{struct Cyc_Parse_Abstractdeclarator*_tmp1005;struct Cyc_Absyn_Function_mod_struct*
_tmp1004;struct Cyc_Absyn_WithTypes_struct*_tmp1003;struct Cyc_Absyn_WithTypes_struct
_tmp1002;struct Cyc_Absyn_Function_mod_struct _tmp1001;struct Cyc_List_List*
_tmp1000;union Cyc_YYSTYPE_union _tmpFFF;yyval=(union Cyc_YYSTYPE_union)(((_tmpFFF.YY30).tag=
34,(((_tmpFFF.YY30).f1=((_tmp1005=_cycalloc(sizeof(*_tmp1005)),((_tmp1005->tms=((
_tmp1000=_cycalloc(sizeof(*_tmp1000)),((_tmp1000->hd=(void*)((void*)((_tmp1004=
_cycalloc(sizeof(*_tmp1004)),((_tmp1004[0]=((_tmp1001.tag=3,((_tmp1001.f1=(void*)((
void*)((_tmp1003=_cycalloc(sizeof(*_tmp1003)),((_tmp1003[0]=((_tmp1002.tag=1,((
_tmp1002.f1=_tmp6D6,((_tmp1002.f2=_tmp6D7,((_tmp1002.f3=_tmp6D8,((_tmp1002.f4=
_tmp6D9,((_tmp1002.f5=_tmp6DA,_tmp1002)))))))))))),_tmp1003))))),_tmp1001)))),
_tmp1004))))),((_tmp1000->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms,_tmp1000)))))),_tmp1005)))),_tmpFFF))));}break;}
case 263: _LL307: {struct Cyc_List_List*_tmp6E2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));{
struct Cyc_Parse_Abstractdeclarator*_tmp1014;struct Cyc_Absyn_TypeParams_mod_struct*
_tmp1013;struct Cyc_Absyn_TypeParams_mod_struct _tmp1012;struct Cyc_List_List*
_tmp1011;union Cyc_YYSTYPE_union _tmp1010;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1010.YY30).tag=34,(((_tmp1010.YY30).f1=((_tmp1014=_cycalloc(sizeof(*_tmp1014)),((
_tmp1014->tms=((_tmp1011=_cycalloc(sizeof(*_tmp1011)),((_tmp1011->hd=(void*)((
void*)((_tmp1013=_cycalloc(sizeof(*_tmp1013)),((_tmp1013[0]=((_tmp1012.tag=4,((
_tmp1012.f1=_tmp6E2,((_tmp1012.f2=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmp1012.f3=0,
_tmp1012)))))))),_tmp1013))))),((_tmp1011->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms,_tmp1011)))))),
_tmp1014)))),_tmp1010))));}break;}case 264: _LL308:{struct Cyc_Parse_Abstractdeclarator*
_tmp1023;struct Cyc_Absyn_Attributes_mod_struct*_tmp1022;struct Cyc_Absyn_Attributes_mod_struct
_tmp1021;struct Cyc_List_List*_tmp1020;union Cyc_YYSTYPE_union _tmp101F;yyval=(
union Cyc_YYSTYPE_union)(((_tmp101F.YY30).tag=34,(((_tmp101F.YY30).f1=((_tmp1023=
_cycalloc(sizeof(*_tmp1023)),((_tmp1023->tms=((_tmp1020=_cycalloc(sizeof(*
_tmp1020)),((_tmp1020->hd=(void*)((void*)((_tmp1022=_cycalloc(sizeof(*_tmp1022)),((
_tmp1022[0]=((_tmp1021.tag=5,((_tmp1021.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),((_tmp1021.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),
_tmp1021)))))),_tmp1022))))),((_tmp1020->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->tms,_tmp1020)))))),
_tmp1023)))),_tmp101F))));}break;case 265: _LL309: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 266: _LL30A: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 267: _LL30B:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 268:
_LL30C: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
269: _LL30D: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 270: _LL30E: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 271: _LL30F:{const char*_tmp1025;const char*_tmp1024;if(Cyc_zstrcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),((_tmp1025="`H",_tag_dynforward(_tmp1025,sizeof(char),
_get_zero_arr_size_char(_tmp1025,3)))))== 0  || Cyc_zstrcmp((struct
_dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),((_tmp1024="`U",_tag_dynforward(_tmp1024,sizeof(char),
_get_zero_arr_size_char(_tmp1024,3)))))== 0){const char*_tmp1029;void*_tmp1028[1];
struct Cyc_String_pa_struct _tmp1027;Cyc_Parse_err((struct _dynforward_ptr)((
_tmp1027.tag=0,((_tmp1027.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])),((_tmp1028[0]=&
_tmp1027,Cyc_aprintf(((_tmp1029="bad occurrence of heap region %s",
_tag_dynforward(_tmp1029,sizeof(char),_get_zero_arr_size_char(_tmp1029,33)))),
_tag_dynforward(_tmp1028,sizeof(void*),1)))))))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}}{struct
_dynforward_ptr*_tmp102C;struct Cyc_Absyn_Tvar*_tmp102B;struct Cyc_Absyn_Tvar*tv=(
_tmp102B=_cycalloc(sizeof(*_tmp102B)),((_tmp102B->name=((_tmp102C=_cycalloc(
sizeof(*_tmp102C)),((_tmp102C[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmp102C)))),((_tmp102B->identity=
- 1,((_tmp102B->kind=(void*)Cyc_Tcutil_kind_to_bound((void*)3),_tmp102B)))))));
struct Cyc_Absyn_VarType_struct _tmp102F;struct Cyc_Absyn_VarType_struct*_tmp102E;
void*t=(void*)((_tmp102E=_cycalloc(sizeof(*_tmp102E)),((_tmp102E[0]=((_tmp102F.tag=
1,((_tmp102F.f1=tv,_tmp102F)))),_tmp102E))));{struct Cyc_Absyn_Region_s_struct*
_tmp104C;struct _tuple1*_tmp104B;struct _dynforward_ptr*_tmp104A;union Cyc_Absyn_Nmspace_union
_tmp1049;struct Cyc_Absyn_RgnHandleType_struct*_tmp1048;struct Cyc_Absyn_RgnHandleType_struct
_tmp1047;struct Cyc_Absyn_Region_s_struct _tmp1046;union Cyc_YYSTYPE_union _tmp1045;
yyval=(union Cyc_YYSTYPE_union)(((_tmp1045.YY9).tag=13,(((_tmp1045.YY9).f1=Cyc_Absyn_new_stmt((
void*)((_tmp104C=_cycalloc(sizeof(*_tmp104C)),((_tmp104C[0]=((_tmp1046.tag=15,((
_tmp1046.f1=tv,((_tmp1046.f2=Cyc_Absyn_new_vardecl(((_tmp104B=_cycalloc(sizeof(*
_tmp104B)),((_tmp104B->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp1049.Loc_n).tag=0,
_tmp1049)),((_tmp104B->f2=((_tmp104A=_cycalloc(sizeof(*_tmp104A)),((_tmp104A[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmp104A)))),_tmp104B)))))),(void*)((_tmp1048=_cycalloc(sizeof(*_tmp1048)),((
_tmp1048[0]=((_tmp1047.tag=15,((_tmp1047.f1=(void*)t,_tmp1047)))),_tmp1048)))),0),((
_tmp1046.f3=0,((_tmp1046.f4=0,((_tmp1046.f5=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1046)))))))))))),
_tmp104C)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp1045))));}break;}case 272: _LL310:{const char*
_tmp104D;if(Cyc_zstrcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmp104D="H",
_tag_dynforward(_tmp104D,sizeof(char),_get_zero_arr_size_char(_tmp104D,2)))))== 
0){const char*_tmp104E;Cyc_Parse_err(((_tmp104E="bad occurrence of heap region `H",
_tag_dynforward(_tmp104E,sizeof(char),_get_zero_arr_size_char(_tmp104E,33)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line));}}{struct _dynforward_ptr*_tmp105B;const char*
_tmp105A;void*_tmp1059[1];struct Cyc_String_pa_struct _tmp1058;struct Cyc_Absyn_Tvar*
_tmp1057;struct Cyc_Absyn_Tvar*tv=(_tmp1057=_cycalloc(sizeof(*_tmp1057)),((
_tmp1057->name=((_tmp105B=_cycalloc(sizeof(*_tmp105B)),((_tmp105B[0]=(struct
_dynforward_ptr)((_tmp1058.tag=0,((_tmp1058.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),((_tmp1059[0]=& _tmp1058,Cyc_aprintf(((_tmp105A="`%s",
_tag_dynforward(_tmp105A,sizeof(char),_get_zero_arr_size_char(_tmp105A,4)))),
_tag_dynforward(_tmp1059,sizeof(void*),1)))))))),_tmp105B)))),((_tmp1057->identity=
- 1,((_tmp1057->kind=(void*)Cyc_Tcutil_kind_to_bound((void*)3),_tmp1057)))))));
struct Cyc_Absyn_VarType_struct _tmp105E;struct Cyc_Absyn_VarType_struct*_tmp105D;
void*t=(void*)((_tmp105D=_cycalloc(sizeof(*_tmp105D)),((_tmp105D[0]=((_tmp105E.tag=
1,((_tmp105E.f1=tv,_tmp105E)))),_tmp105D))));{struct Cyc_Absyn_Region_s_struct*
_tmp107B;struct _tuple1*_tmp107A;struct _dynforward_ptr*_tmp1079;union Cyc_Absyn_Nmspace_union
_tmp1078;struct Cyc_Absyn_RgnHandleType_struct*_tmp1077;struct Cyc_Absyn_RgnHandleType_struct
_tmp1076;struct Cyc_Absyn_Region_s_struct _tmp1075;union Cyc_YYSTYPE_union _tmp1074;
yyval=(union Cyc_YYSTYPE_union)(((_tmp1074.YY9).tag=13,(((_tmp1074.YY9).f1=Cyc_Absyn_new_stmt((
void*)((_tmp107B=_cycalloc(sizeof(*_tmp107B)),((_tmp107B[0]=((_tmp1075.tag=15,((
_tmp1075.f1=tv,((_tmp1075.f2=Cyc_Absyn_new_vardecl(((_tmp107A=_cycalloc(sizeof(*
_tmp107A)),((_tmp107A->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp1078.Loc_n).tag=0,
_tmp1078)),((_tmp107A->f2=((_tmp1079=_cycalloc(sizeof(*_tmp1079)),((_tmp1079[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmp1079)))),_tmp107A)))))),(void*)((_tmp1077=_cycalloc(sizeof(*_tmp1077)),((
_tmp1077[0]=((_tmp1076.tag=15,((_tmp1076.f1=(void*)t,_tmp1076)))),_tmp1077)))),0),((
_tmp1075.f3=0,((_tmp1075.f4=Cyc_yyget_YY4(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),((_tmp1075.f5=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1075)))))))))))),
_tmp107B)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),_tmp1074))));}break;}case 273: _LL311:{const char*
_tmp107C;if(Cyc_zstrcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),((_tmp107C="resetable",
_tag_dynforward(_tmp107C,sizeof(char),_get_zero_arr_size_char(_tmp107C,10)))))!= 
0){const char*_tmp107D;Cyc_Parse_err(((_tmp107D="expecting [resetable]",
_tag_dynforward(_tmp107D,sizeof(char),_get_zero_arr_size_char(_tmp107D,22)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 6)]).last_line));}}{const char*_tmp107F;const char*_tmp107E;if(Cyc_zstrcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),((_tmp107F="`H",_tag_dynforward(_tmp107F,sizeof(char),
_get_zero_arr_size_char(_tmp107F,3)))))== 0  || Cyc_zstrcmp((struct
_dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),((_tmp107E="`U",_tag_dynforward(_tmp107E,sizeof(char),
_get_zero_arr_size_char(_tmp107E,3)))))){const char*_tmp1083;void*_tmp1082[1];
struct Cyc_String_pa_struct _tmp1081;Cyc_Parse_err((struct _dynforward_ptr)((
_tmp1081.tag=0,((_tmp1081.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])),((_tmp1082[0]=&
_tmp1081,Cyc_aprintf(((_tmp1083="bad occurrence of heap region %s",
_tag_dynforward(_tmp1083,sizeof(char),_get_zero_arr_size_char(_tmp1083,33)))),
_tag_dynforward(_tmp1082,sizeof(void*),1)))))))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}}{struct
_dynforward_ptr*_tmp1086;struct Cyc_Absyn_Tvar*_tmp1085;struct Cyc_Absyn_Tvar*tv=(
_tmp1085=_cycalloc(sizeof(*_tmp1085)),((_tmp1085->name=((_tmp1086=_cycalloc(
sizeof(*_tmp1086)),((_tmp1086[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmp1086)))),((_tmp1085->identity=
- 1,((_tmp1085->kind=(void*)Cyc_Tcutil_kind_to_bound((void*)3),_tmp1085)))))));
struct Cyc_Absyn_VarType_struct _tmp1089;struct Cyc_Absyn_VarType_struct*_tmp1088;
void*t=(void*)((_tmp1088=_cycalloc(sizeof(*_tmp1088)),((_tmp1088[0]=((_tmp1089.tag=
1,((_tmp1089.f1=tv,_tmp1089)))),_tmp1088))));{struct Cyc_Absyn_Region_s_struct*
_tmp10A6;struct _tuple1*_tmp10A5;struct _dynforward_ptr*_tmp10A4;union Cyc_Absyn_Nmspace_union
_tmp10A3;struct Cyc_Absyn_RgnHandleType_struct*_tmp10A2;struct Cyc_Absyn_RgnHandleType_struct
_tmp10A1;struct Cyc_Absyn_Region_s_struct _tmp10A0;union Cyc_YYSTYPE_union _tmp109F;
yyval=(union Cyc_YYSTYPE_union)(((_tmp109F.YY9).tag=13,(((_tmp109F.YY9).f1=Cyc_Absyn_new_stmt((
void*)((_tmp10A6=_cycalloc(sizeof(*_tmp10A6)),((_tmp10A6[0]=((_tmp10A0.tag=15,((
_tmp10A0.f1=tv,((_tmp10A0.f2=Cyc_Absyn_new_vardecl(((_tmp10A5=_cycalloc(sizeof(*
_tmp10A5)),((_tmp10A5->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp10A3.Loc_n).tag=0,
_tmp10A3)),((_tmp10A5->f2=((_tmp10A4=_cycalloc(sizeof(*_tmp10A4)),((_tmp10A4[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmp10A4)))),_tmp10A5)))))),(void*)((_tmp10A2=_cycalloc(sizeof(*_tmp10A2)),((
_tmp10A2[0]=((_tmp10A1.tag=15,((_tmp10A1.f1=(void*)t,_tmp10A1)))),_tmp10A2)))),0),((
_tmp10A0.f3=1,((_tmp10A0.f4=0,((_tmp10A0.f5=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp10A0)))))))))))),
_tmp10A6)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 8)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp109F))));}break;}case 274: _LL312:{const char*
_tmp10A7;if(Cyc_zstrcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmp10A7="resetable",
_tag_dynforward(_tmp10A7,sizeof(char),_get_zero_arr_size_char(_tmp10A7,10)))))!= 
0){const char*_tmp10A8;Cyc_Parse_err(((_tmp10A8="expecting `resetable'",
_tag_dynforward(_tmp10A8,sizeof(char),_get_zero_arr_size_char(_tmp10A8,22)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));}}{const char*_tmp10A9;if(Cyc_zstrcmp((struct
_dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmp10A9="H",_tag_dynforward(_tmp10A9,sizeof(char),
_get_zero_arr_size_char(_tmp10A9,2)))))== 0){const char*_tmp10AA;Cyc_Parse_err(((
_tmp10AA="bad occurrence of heap region `H",_tag_dynforward(_tmp10AA,sizeof(char),
_get_zero_arr_size_char(_tmp10AA,33)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}}{struct
_dynforward_ptr*_tmp10B7;const char*_tmp10B6;void*_tmp10B5[1];struct Cyc_String_pa_struct
_tmp10B4;struct Cyc_Absyn_Tvar*_tmp10B3;struct Cyc_Absyn_Tvar*tv=(_tmp10B3=
_cycalloc(sizeof(*_tmp10B3)),((_tmp10B3->name=((_tmp10B7=_cycalloc(sizeof(*
_tmp10B7)),((_tmp10B7[0]=(struct _dynforward_ptr)((_tmp10B4.tag=0,((_tmp10B4.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),((_tmp10B5[0]=& _tmp10B4,
Cyc_aprintf(((_tmp10B6="`%s",_tag_dynforward(_tmp10B6,sizeof(char),
_get_zero_arr_size_char(_tmp10B6,4)))),_tag_dynforward(_tmp10B5,sizeof(void*),1)))))))),
_tmp10B7)))),((_tmp10B3->identity=- 1,((_tmp10B3->kind=(void*)Cyc_Tcutil_kind_to_bound((
void*)3),_tmp10B3)))))));struct Cyc_Absyn_VarType_struct _tmp10BA;struct Cyc_Absyn_VarType_struct*
_tmp10B9;void*t=(void*)((_tmp10B9=_cycalloc(sizeof(*_tmp10B9)),((_tmp10B9[0]=((
_tmp10BA.tag=1,((_tmp10BA.f1=tv,_tmp10BA)))),_tmp10B9))));{struct Cyc_Absyn_Region_s_struct*
_tmp10D7;struct _tuple1*_tmp10D6;struct _dynforward_ptr*_tmp10D5;union Cyc_Absyn_Nmspace_union
_tmp10D4;struct Cyc_Absyn_RgnHandleType_struct*_tmp10D3;struct Cyc_Absyn_RgnHandleType_struct
_tmp10D2;struct Cyc_Absyn_Region_s_struct _tmp10D1;union Cyc_YYSTYPE_union _tmp10D0;
yyval=(union Cyc_YYSTYPE_union)(((_tmp10D0.YY9).tag=13,(((_tmp10D0.YY9).f1=Cyc_Absyn_new_stmt((
void*)((_tmp10D7=_cycalloc(sizeof(*_tmp10D7)),((_tmp10D7[0]=((_tmp10D1.tag=15,((
_tmp10D1.f1=tv,((_tmp10D1.f2=Cyc_Absyn_new_vardecl(((_tmp10D6=_cycalloc(sizeof(*
_tmp10D6)),((_tmp10D6->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp10D4.Loc_n).tag=0,
_tmp10D4)),((_tmp10D6->f2=((_tmp10D5=_cycalloc(sizeof(*_tmp10D5)),((_tmp10D5[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),
_tmp10D5)))),_tmp10D6)))))),(void*)((_tmp10D3=_cycalloc(sizeof(*_tmp10D3)),((
_tmp10D3[0]=((_tmp10D2.tag=15,((_tmp10D2.f1=(void*)t,_tmp10D2)))),_tmp10D3)))),0),((
_tmp10D1.f3=1,((_tmp10D1.f4=0,((_tmp10D1.f5=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp10D1)))))))))))),
_tmp10D7)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp10D0))));}break;}case 275: _LL313:{struct Cyc_Absyn_ResetRegion_s_struct*
_tmp10DD;struct Cyc_Absyn_ResetRegion_s_struct _tmp10DC;union Cyc_YYSTYPE_union
_tmp10DB;yyval=(union Cyc_YYSTYPE_union)(((_tmp10DB.YY9).tag=13,(((_tmp10DB.YY9).f1=
Cyc_Absyn_new_stmt((void*)((_tmp10DD=_cycalloc(sizeof(*_tmp10DD)),((_tmp10DD[0]=((
_tmp10DC.tag=16,((_tmp10DC.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),_tmp10DC)))),_tmp10DD)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp10DB))));}
break;case 276: _LL314:{const char*_tmp10DE;if(Cyc_zstrcmp((struct _dynforward_ptr)
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((
_tmp10DE="in",_tag_dynforward(_tmp10DE,sizeof(char),_get_zero_arr_size_char(
_tmp10DE,3)))))!= 0){const char*_tmp10DF;Cyc_Parse_err(((_tmp10DF="expecting `in'",
_tag_dynforward(_tmp10DF,sizeof(char),_get_zero_arr_size_char(_tmp10DF,15)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));}}{struct _dynforward_ptr*_tmp10E7;struct Cyc_Absyn_Eq_kb_struct*
_tmp10E6;struct Cyc_Absyn_Eq_kb_struct _tmp10E5;struct Cyc_Absyn_Tvar*_tmp10E4;
struct Cyc_Absyn_Tvar*tv=(_tmp10E4=_cycalloc(sizeof(*_tmp10E4)),((_tmp10E4->name=((
_tmp10E7=_cycalloc(sizeof(*_tmp10E7)),((_tmp10E7[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp10E7)))),((_tmp10E4->identity=
- 1,((_tmp10E4->kind=(void*)((void*)((_tmp10E6=_cycalloc(sizeof(*_tmp10E6)),((
_tmp10E6[0]=((_tmp10E5.tag=0,((_tmp10E5.f1=(void*)((void*)3),_tmp10E5)))),
_tmp10E6))))),_tmp10E4)))))));{struct Cyc_Absyn_Alias_s_struct*_tmp10FB;struct
_tuple1*_tmp10FA;struct _dynforward_ptr*_tmp10F9;union Cyc_Absyn_Nmspace_union
_tmp10F8;struct Cyc_Absyn_Alias_s_struct _tmp10F7;union Cyc_YYSTYPE_union _tmp10F6;
yyval=(union Cyc_YYSTYPE_union)(((_tmp10F6.YY9).tag=13,(((_tmp10F6.YY9).f1=Cyc_Absyn_new_stmt((
void*)((_tmp10FB=_cycalloc(sizeof(*_tmp10FB)),((_tmp10FB[0]=((_tmp10F7.tag=17,((
_tmp10F7.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 2)]),((_tmp10F7.f2=tv,((_tmp10F7.f3=Cyc_Absyn_new_vardecl(((_tmp10FA=_cycalloc(
sizeof(*_tmp10FA)),((_tmp10FA->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp10F8.Loc_n).tag=
0,_tmp10F8)),((_tmp10FA->f2=((_tmp10F9=_cycalloc(sizeof(*_tmp10F9)),((_tmp10F9[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
_tmp10F9)))),_tmp10FA)))))),(void*)0,0),((_tmp10F7.f4=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp10F7)))))))))),_tmp10FB)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 8)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp10F6))));}break;}case 277: _LL315:{union Cyc_YYSTYPE_union
_tmp10FC;yyval=(union Cyc_YYSTYPE_union)(((_tmp10FC.YY4).tag=8,(((_tmp10FC.YY4).f1=
0,_tmp10FC))));}break;case 278: _LL316:{const char*_tmp10FD;if(Cyc_zstrcmp((struct
_dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),((_tmp10FD="open",_tag_dynforward(_tmp10FD,sizeof(char),
_get_zero_arr_size_char(_tmp10FD,5)))))!= 0){const char*_tmp10FE;Cyc_Parse_err(((
_tmp10FE="expecting `open'",_tag_dynforward(_tmp10FE,sizeof(char),
_get_zero_arr_size_char(_tmp10FE,17)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));}}{union Cyc_YYSTYPE_union
_tmp10FF;yyval=(union Cyc_YYSTYPE_union)(((_tmp10FF.YY4).tag=8,(((_tmp10FF.YY4).f1=(
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmp10FF))));}break;case 279: _LL317:{struct Cyc_Absyn_Label_s_struct*
_tmp1109;struct _dynforward_ptr*_tmp1108;struct Cyc_Absyn_Label_s_struct _tmp1107;
union Cyc_YYSTYPE_union _tmp1106;yyval=(union Cyc_YYSTYPE_union)(((_tmp1106.YY9).tag=
13,(((_tmp1106.YY9).f1=Cyc_Absyn_new_stmt((void*)((_tmp1109=_cycalloc(sizeof(*
_tmp1109)),((_tmp1109[0]=((_tmp1107.tag=12,((_tmp1107.f1=((_tmp1108=_cycalloc(
sizeof(*_tmp1108)),((_tmp1108[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp1108)))),((_tmp1107.f2=
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1107)))))),
_tmp1109)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp1106))));}break;case 280: _LL318:{union Cyc_YYSTYPE_union
_tmp110A;yyval=(union Cyc_YYSTYPE_union)(((_tmp110A.YY9).tag=13,(((_tmp110A.YY9).f1=
Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp110A))));}
break;case 281: _LL319:{union Cyc_YYSTYPE_union _tmp110B;yyval=(union Cyc_YYSTYPE_union)(((
_tmp110B.YY9).tag=13,(((_tmp110B.YY9).f1=Cyc_Absyn_exp_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp110B))));}
break;case 282: _LL31A:{union Cyc_YYSTYPE_union _tmp110C;yyval=(union Cyc_YYSTYPE_union)(((
_tmp110C.YY9).tag=13,(((_tmp110C.YY9).f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp110C))));}
break;case 283: _LL31B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 284: _LL31C:{union Cyc_YYSTYPE_union _tmp110D;yyval=(
union Cyc_YYSTYPE_union)(((_tmp110D.YY9).tag=13,(((_tmp110D.YY9).f1=Cyc_Parse_flatten_declarations(
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(
0)),_tmp110D))));}break;case 285: _LL31D:{union Cyc_YYSTYPE_union _tmp110E;yyval=(
union Cyc_YYSTYPE_union)(((_tmp110E.YY9).tag=13,(((_tmp110E.YY9).f1=Cyc_Parse_flatten_declarations(
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmp110E))));}break;
case 286: _LL31E: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 287: _LL31F:{union Cyc_YYSTYPE_union _tmp110F;yyval=(union Cyc_YYSTYPE_union)(((
_tmp110F.YY9).tag=13,(((_tmp110F.YY9).f1=Cyc_Absyn_seq_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp110F))));}
break;case 288: _LL320:{struct Cyc_Absyn_Fn_d_struct*_tmp1115;struct Cyc_Absyn_Fn_d_struct
_tmp1114;union Cyc_YYSTYPE_union _tmp1113;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1113.YY9).tag=13,(((_tmp1113.YY9).f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)((_tmp1115=_cycalloc(sizeof(*_tmp1115)),((_tmp1115[0]=((_tmp1114.tag=1,((
_tmp1114.f1=Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp1114)))),_tmp1115)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(
0)),_tmp1113))));}break;case 289: _LL321:{struct Cyc_Absyn_Fn_d_struct*_tmp111B;
struct Cyc_Absyn_Fn_d_struct _tmp111A;union Cyc_YYSTYPE_union _tmp1119;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1119.YY9).tag=13,(((_tmp1119.YY9).f1=Cyc_Parse_flatten_decl(
Cyc_Absyn_new_decl((void*)((_tmp111B=_cycalloc(sizeof(*_tmp111B)),((_tmp111B[0]=((
_tmp111A.tag=1,((_tmp111A.f1=Cyc_yyget_YY17(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),_tmp111A)))),_tmp111B)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),_tmp1119))));}break;
case 290: _LL322:{union Cyc_YYSTYPE_union _tmp111C;yyval=(union Cyc_YYSTYPE_union)(((
_tmp111C.YY9).tag=13,(((_tmp111C.YY9).f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp111C))));}
break;case 291: _LL323:{union Cyc_YYSTYPE_union _tmp111D;yyval=(union Cyc_YYSTYPE_union)(((
_tmp111D.YY9).tag=13,(((_tmp111D.YY9).f1=Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp111D))));}
break;case 292: _LL324:{union Cyc_YYSTYPE_union _tmp111E;yyval=(union Cyc_YYSTYPE_union)(((
_tmp111E.YY9).tag=13,(((_tmp111E.YY9).f1=Cyc_Absyn_switch_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp111E))));}
break;case 293: _LL325: {struct Cyc_Absyn_UnknownId_e_struct _tmp1121;struct Cyc_Absyn_UnknownId_e_struct*
_tmp1120;struct Cyc_Absyn_Exp*_tmp75B=Cyc_Absyn_new_exp((void*)((_tmp1120=
_cycalloc(sizeof(*_tmp1120)),((_tmp1120[0]=((_tmp1121.tag=2,((_tmp1121.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmp1121)))),
_tmp1120)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));{union Cyc_YYSTYPE_union _tmp1122;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1122.YY9).tag=13,(((_tmp1122.YY9).f1=Cyc_Absyn_switch_stmt(_tmp75B,Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1122))));}
break;}case 294: _LL326: {struct Cyc_Absyn_Exp*_tmp75F=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{union Cyc_YYSTYPE_union
_tmp1123;yyval=(union Cyc_YYSTYPE_union)(((_tmp1123.YY9).tag=13,(((_tmp1123.YY9).f1=
Cyc_Absyn_switch_stmt(_tmp75F,Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1123))));}
break;}case 295: _LL327:{union Cyc_YYSTYPE_union _tmp1124;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1124.YY9).tag=13,(((_tmp1124.YY9).f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1124))));}
break;case 296: _LL328:{union Cyc_YYSTYPE_union _tmp1125;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1125.YY10).tag=14,(((_tmp1125.YY10).f1=0,_tmp1125))));}break;case 297: _LL329:{
struct Cyc_List_List*_tmp112B;struct Cyc_Absyn_Switch_clause*_tmp112A;union Cyc_YYSTYPE_union
_tmp1129;yyval=(union Cyc_YYSTYPE_union)(((_tmp1129.YY10).tag=14,(((_tmp1129.YY10).f1=((
_tmp112B=_cycalloc(sizeof(*_tmp112B)),((_tmp112B->hd=((_tmp112A=_cycalloc(
sizeof(*_tmp112A)),((_tmp112A->pattern=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line)),((_tmp112A->pat_vars=
0,((_tmp112A->where_clause=0,((_tmp112A->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmp112A->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmp112A)))))))))))),((
_tmp112B->tl=0,_tmp112B)))))),_tmp1129))));}break;case 298: _LL32A:{struct Cyc_List_List*
_tmp1131;struct Cyc_Absyn_Switch_clause*_tmp1130;union Cyc_YYSTYPE_union _tmp112F;
yyval=(union Cyc_YYSTYPE_union)(((_tmp112F.YY10).tag=14,(((_tmp112F.YY10).f1=((
_tmp1131=_cycalloc(sizeof(*_tmp1131)),((_tmp1131->hd=((_tmp1130=_cycalloc(
sizeof(*_tmp1130)),((_tmp1130->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmp1130->pat_vars=0,((
_tmp1130->where_clause=0,((_tmp1130->body=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),((_tmp1130->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),_tmp1130)))))))))))),((_tmp1131->tl=Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1131)))))),
_tmp112F))));}break;case 299: _LL32B:{struct Cyc_List_List*_tmp1137;struct Cyc_Absyn_Switch_clause*
_tmp1136;union Cyc_YYSTYPE_union _tmp1135;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1135.YY10).tag=14,(((_tmp1135.YY10).f1=((_tmp1137=_cycalloc(sizeof(*_tmp1137)),((
_tmp1137->hd=((_tmp1136=_cycalloc(sizeof(*_tmp1136)),((_tmp1136->pattern=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmp1136->pat_vars=
0,((_tmp1136->where_clause=0,((_tmp1136->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmp1136->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp1136)))))))))))),((
_tmp1137->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp1137)))))),_tmp1135))));}break;case 300: _LL32C:{struct Cyc_List_List*
_tmp113D;struct Cyc_Absyn_Switch_clause*_tmp113C;union Cyc_YYSTYPE_union _tmp113B;
yyval=(union Cyc_YYSTYPE_union)(((_tmp113B.YY10).tag=14,(((_tmp113B.YY10).f1=((
_tmp113D=_cycalloc(sizeof(*_tmp113D)),((_tmp113D->hd=((_tmp113C=_cycalloc(
sizeof(*_tmp113C)),((_tmp113C->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmp113C->pat_vars=0,((
_tmp113C->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmp113C->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),((_tmp113C->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmp113C)))))))))))),((
_tmp113D->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp113D)))))),_tmp113B))));}break;case 301: _LL32D:{struct Cyc_List_List*
_tmp1143;struct Cyc_Absyn_Switch_clause*_tmp1142;union Cyc_YYSTYPE_union _tmp1141;
yyval=(union Cyc_YYSTYPE_union)(((_tmp1141.YY10).tag=14,(((_tmp1141.YY10).f1=((
_tmp1143=_cycalloc(sizeof(*_tmp1143)),((_tmp1143->hd=((_tmp1142=_cycalloc(
sizeof(*_tmp1142)),((_tmp1142->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),((_tmp1142->pat_vars=0,((
_tmp1142->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmp1142->body=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmp1142->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),_tmp1142)))))))))))),((
_tmp1143->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),_tmp1143)))))),_tmp1141))));}break;case 302: _LL32E:{union Cyc_YYSTYPE_union
_tmp1144;yyval=(union Cyc_YYSTYPE_union)(((_tmp1144.YY9).tag=13,(((_tmp1144.YY9).f1=
Cyc_Absyn_while_stmt(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp1144))));}break;case 303: _LL32F:{union Cyc_YYSTYPE_union
_tmp1145;yyval=(union Cyc_YYSTYPE_union)(((_tmp1145.YY9).tag=13,(((_tmp1145.YY9).f1=
Cyc_Absyn_do_stmt(Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1145))));}
break;case 304: _LL330:{union Cyc_YYSTYPE_union _tmp1146;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1146.YY9).tag=13,(((_tmp1146.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1146))));}
break;case 305: _LL331:{union Cyc_YYSTYPE_union _tmp1147;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1147.YY9).tag=13,(((_tmp1147.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp1147))));}break;case 306: _LL332:{union Cyc_YYSTYPE_union
_tmp1148;yyval=(union Cyc_YYSTYPE_union)(((_tmp1148.YY9).tag=13,(((_tmp1148.YY9).f1=
Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1148))));}
break;case 307: _LL333:{union Cyc_YYSTYPE_union _tmp1149;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1149.YY9).tag=13,(((_tmp1149.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmp1149))));}
break;case 308: _LL334:{union Cyc_YYSTYPE_union _tmp114A;yyval=(union Cyc_YYSTYPE_union)(((
_tmp114A.YY9).tag=13,(((_tmp114A.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(0),
Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp114A))));}break;case 309: _LL335:{union Cyc_YYSTYPE_union
_tmp114B;yyval=(union Cyc_YYSTYPE_union)(((_tmp114B.YY9).tag=13,(((_tmp114B.YY9).f1=
Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp114B))));}
break;case 310: _LL336:{union Cyc_YYSTYPE_union _tmp114C;yyval=(union Cyc_YYSTYPE_union)(((
_tmp114C.YY9).tag=13,(((_tmp114C.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp114C))));}
break;case 311: _LL337:{union Cyc_YYSTYPE_union _tmp114D;yyval=(union Cyc_YYSTYPE_union)(((
_tmp114D.YY9).tag=13,(((_tmp114D.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp114D))));}
break;case 312: _LL338: {struct Cyc_List_List*_tmp77C=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp77D=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{union Cyc_YYSTYPE_union
_tmp114E;yyval=(union Cyc_YYSTYPE_union)(((_tmp114E.YY9).tag=13,(((_tmp114E.YY9).f1=
Cyc_Parse_flatten_declarations(_tmp77C,_tmp77D),_tmp114E))));}break;}case 313:
_LL339: {struct Cyc_List_List*_tmp77F=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*
_tmp780=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{union Cyc_YYSTYPE_union
_tmp114F;yyval=(union Cyc_YYSTYPE_union)(((_tmp114F.YY9).tag=13,(((_tmp114F.YY9).f1=
Cyc_Parse_flatten_declarations(_tmp77F,_tmp780),_tmp114F))));}break;}case 314:
_LL33A: {struct Cyc_List_List*_tmp782=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);struct Cyc_Absyn_Stmt*
_tmp783=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{union Cyc_YYSTYPE_union
_tmp1150;yyval=(union Cyc_YYSTYPE_union)(((_tmp1150.YY9).tag=13,(((_tmp1150.YY9).f1=
Cyc_Parse_flatten_declarations(_tmp782,_tmp783),_tmp1150))));}break;}case 315:
_LL33B: {struct Cyc_List_List*_tmp785=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);struct Cyc_Absyn_Stmt*
_tmp786=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));{union Cyc_YYSTYPE_union
_tmp1151;yyval=(union Cyc_YYSTYPE_union)(((_tmp1151.YY9).tag=13,(((_tmp1151.YY9).f1=
Cyc_Parse_flatten_declarations(_tmp785,_tmp786),_tmp1151))));}break;}case 316:
_LL33C:{struct _dynforward_ptr*_tmp1154;union Cyc_YYSTYPE_union _tmp1153;yyval=(
union Cyc_YYSTYPE_union)(((_tmp1153.YY9).tag=13,(((_tmp1153.YY9).f1=Cyc_Absyn_goto_stmt(((
_tmp1154=_cycalloc(sizeof(*_tmp1154)),((_tmp1154[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmp1154)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmp1153))));}
break;case 317: _LL33D:{union Cyc_YYSTYPE_union _tmp1155;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1155.YY9).tag=13,(((_tmp1155.YY9).f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmp1155))));}
break;case 318: _LL33E:{union Cyc_YYSTYPE_union _tmp1156;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1156.YY9).tag=13,(((_tmp1156.YY9).f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmp1156))));}
break;case 319: _LL33F:{union Cyc_YYSTYPE_union _tmp1157;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1157.YY9).tag=13,(((_tmp1157.YY9).f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmp1157))));}
break;case 320: _LL340:{union Cyc_YYSTYPE_union _tmp1158;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1158.YY9).tag=13,(((_tmp1158.YY9).f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmp1158))));}
break;case 321: _LL341:{union Cyc_YYSTYPE_union _tmp1159;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1159.YY9).tag=13,(((_tmp1159.YY9).f1=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line)),_tmp1159))));}
break;case 322: _LL342:{union Cyc_YYSTYPE_union _tmp115A;yyval=(union Cyc_YYSTYPE_union)(((
_tmp115A.YY9).tag=13,(((_tmp115A.YY9).f1=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line)),_tmp115A))));}
break;case 323: _LL343:{union Cyc_YYSTYPE_union _tmp115B;yyval=(union Cyc_YYSTYPE_union)(((
_tmp115B.YY9).tag=13,(((_tmp115B.YY9).f1=Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line)),_tmp115B))));}
break;case 324: _LL344: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 325: _LL345: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 326: _LL346:{union Cyc_YYSTYPE_union _tmp115C;yyval=(
union Cyc_YYSTYPE_union)(((_tmp115C.YY11).tag=15,(((_tmp115C.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp115C))));}
break;case 327: _LL347: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 328: _LL348:{union Cyc_YYSTYPE_union _tmp115D;yyval=(union
Cyc_YYSTYPE_union)(((_tmp115D.YY11).tag=15,(((_tmp115D.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp115D))));}
break;case 329: _LL349: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 330: _LL34A:{union Cyc_YYSTYPE_union _tmp115E;yyval=(union
Cyc_YYSTYPE_union)(((_tmp115E.YY11).tag=15,(((_tmp115E.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp115E))));}
break;case 331: _LL34B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 332: _LL34C:{union Cyc_YYSTYPE_union _tmp115F;yyval=(union
Cyc_YYSTYPE_union)(((_tmp115F.YY11).tag=15,(((_tmp115F.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp((void*)14,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp115F))));}
break;case 333: _LL34D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 334: _LL34E:{union Cyc_YYSTYPE_union _tmp1160;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1160.YY11).tag=15,(((_tmp1160.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp((void*)15,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp1160))));}
break;case 335: _LL34F: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 336: _LL350:{union Cyc_YYSTYPE_union _tmp1161;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1161.YY11).tag=15,(((_tmp1161.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp((void*)13,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp1161))));}
break;case 337: _LL351: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 338: _LL352:{union Cyc_YYSTYPE_union _tmp1162;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1162.YY11).tag=15,(((_tmp1162.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp1162))));}
break;case 339: _LL353:{union Cyc_YYSTYPE_union _tmp1163;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1163.YY11).tag=15,(((_tmp1163.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))),_tmp1163))));}break;case 340: _LL354: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 341: _LL355:{union
Cyc_YYSTYPE_union _tmp1164;yyval=(union Cyc_YYSTYPE_union)(((_tmp1164.YY11).tag=15,(((
_tmp1164.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp1164))));}
break;case 342: _LL356:{union Cyc_YYSTYPE_union _tmp1165;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1165.YY11).tag=15,(((_tmp1165.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))),_tmp1165))));}break;case 343: _LL357:{union Cyc_YYSTYPE_union
_tmp1166;yyval=(union Cyc_YYSTYPE_union)(((_tmp1166.YY11).tag=15,(((_tmp1166.YY11).f1=
Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp1166))));}
break;case 344: _LL358:{union Cyc_YYSTYPE_union _tmp1167;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1167.YY11).tag=15,(((_tmp1167.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))),_tmp1167))));}break;case 345: _LL359: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 346: _LL35A:{union
Cyc_YYSTYPE_union _tmp1168;yyval=(union Cyc_YYSTYPE_union)(((_tmp1168.YY11).tag=15,(((
_tmp1168.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((void*)16,Cyc_Parse_pat2exp(
Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp1168))));}
break;case 347: _LL35B:{union Cyc_YYSTYPE_union _tmp1169;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1169.YY11).tag=15,(((_tmp1169.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)17,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp1169))));}
break;case 348: _LL35C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 349: _LL35D:{union Cyc_YYSTYPE_union _tmp116A;yyval=(union
Cyc_YYSTYPE_union)(((_tmp116A.YY11).tag=15,(((_tmp116A.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp((void*)0,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp116A))));}
break;case 350: _LL35E:{union Cyc_YYSTYPE_union _tmp116B;yyval=(union Cyc_YYSTYPE_union)(((
_tmp116B.YY11).tag=15,(((_tmp116B.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)2,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp116B))));}
break;case 351: _LL35F: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 352: _LL360:{union Cyc_YYSTYPE_union _tmp116C;yyval=(union
Cyc_YYSTYPE_union)(((_tmp116C.YY11).tag=15,(((_tmp116C.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_prim2_exp((void*)1,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp116C))));}
break;case 353: _LL361:{union Cyc_YYSTYPE_union _tmp116D;yyval=(union Cyc_YYSTYPE_union)(((
_tmp116D.YY11).tag=15,(((_tmp116D.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)3,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp116D))));}
break;case 354: _LL362:{union Cyc_YYSTYPE_union _tmp116E;yyval=(union Cyc_YYSTYPE_union)(((
_tmp116E.YY11).tag=15,(((_tmp116E.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)4,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp116E))));}
break;case 355: _LL363: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 356: _LL364:{union Cyc_YYSTYPE_union _tmp116F;yyval=(union
Cyc_YYSTYPE_union)(((_tmp116F.YY11).tag=15,(((_tmp116F.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),1,(void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp116F))));}
break;case 357: _LL365: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 358: _LL366:{union Cyc_YYSTYPE_union _tmp1170;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1170.YY11).tag=15,(((_tmp1170.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])),
_tmp1170))));}break;case 359: _LL367:{union Cyc_YYSTYPE_union _tmp1171;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1171.YY11).tag=15,(((_tmp1171.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))),_tmp1171))));}break;case 360: _LL368:{union Cyc_YYSTYPE_union
_tmp1172;yyval=(union Cyc_YYSTYPE_union)(((_tmp1172.YY11).tag=15,(((_tmp1172.YY11).f1=
Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp1172))));}
break;case 361: _LL369:{union Cyc_YYSTYPE_union _tmp1173;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1173.YY11).tag=15,(((_tmp1173.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp1173))));}
break;case 362: _LL36A:{struct Cyc_Absyn_StructField_struct*_tmp117D;struct
_dynforward_ptr*_tmp117C;struct Cyc_Absyn_StructField_struct _tmp117B;union Cyc_YYSTYPE_union
_tmp117A;yyval=(union Cyc_YYSTYPE_union)(((_tmp117A.YY11).tag=15,(((_tmp117A.YY11).f1=
Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)((_tmp117D=
_cycalloc(sizeof(*_tmp117D)),((_tmp117D[0]=((_tmp117B.tag=0,((_tmp117B.f1=((
_tmp117C=_cycalloc(sizeof(*_tmp117C)),((_tmp117C[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),_tmp117C)))),_tmp117B)))),
_tmp117D)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line))),_tmp117A))));}break;case 363: _LL36B:{struct Cyc_Absyn_TupleIndex_struct*
_tmp1183;struct Cyc_Absyn_TupleIndex_struct _tmp1182;union Cyc_YYSTYPE_union
_tmp1181;yyval=(union Cyc_YYSTYPE_union)(((_tmp1181.YY11).tag=15,(((_tmp1181.YY11).f1=
Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)((_tmp1183=
_cycalloc_atomic(sizeof(*_tmp1183)),((_tmp1183[0]=((_tmp1182.tag=1,((_tmp1182.f1=(
unsigned int)(Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])).f2,_tmp1182)))),_tmp1183)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line))),_tmp1181))));}
break;case 364: _LL36C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 365: _LL36D: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 366: _LL36E:{union Cyc_YYSTYPE_union _tmp1184;yyval=(
union Cyc_YYSTYPE_union)(((_tmp1184.YY11).tag=15,(((_tmp1184.YY11).f1=Cyc_Absyn_new_pat((
void*)0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp1184))));}break;case 367: _LL36F:{union Cyc_YYSTYPE_union
_tmp1185;yyval=(union Cyc_YYSTYPE_union)(((_tmp1185.YY11).tag=15,(((_tmp1185.YY11).f1=
Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)])),_tmp1185))));}break;case 368: _LL370: {struct Cyc_Absyn_Exp*e=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);{void*
_tmp7B2=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp7B3;void*_tmp7B4;char _tmp7B5;
union Cyc_Absyn_Cnst_union _tmp7B6;void*_tmp7B7;short _tmp7B8;union Cyc_Absyn_Cnst_union
_tmp7B9;void*_tmp7BA;int _tmp7BB;union Cyc_Absyn_Cnst_union _tmp7BC;struct
_dynforward_ptr _tmp7BD;union Cyc_Absyn_Cnst_union _tmp7BE;union Cyc_Absyn_Cnst_union
_tmp7BF;union Cyc_Absyn_Cnst_union _tmp7C0;_LL373: if(*((int*)_tmp7B2)!= 0)goto
_LL375;_tmp7B3=((struct Cyc_Absyn_Const_e_struct*)_tmp7B2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp7B2)->f1).Char_c).tag != 0)goto _LL375;_tmp7B4=(_tmp7B3.Char_c).f1;_tmp7B5=(
_tmp7B3.Char_c).f2;_LL374:{struct Cyc_Absyn_Char_p_struct*_tmp118B;struct Cyc_Absyn_Char_p_struct
_tmp118A;union Cyc_YYSTYPE_union _tmp1189;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1189.YY11).tag=15,(((_tmp1189.YY11).f1=Cyc_Absyn_new_pat((void*)((_tmp118B=
_cycalloc_atomic(sizeof(*_tmp118B)),((_tmp118B[0]=((_tmp118A.tag=8,((_tmp118A.f1=
_tmp7B5,_tmp118A)))),_tmp118B)))),e->loc),_tmp1189))));}goto _LL372;_LL375: if(*((
int*)_tmp7B2)!= 0)goto _LL377;_tmp7B6=((struct Cyc_Absyn_Const_e_struct*)_tmp7B2)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7B2)->f1).Short_c).tag != 1)goto _LL377;
_tmp7B7=(_tmp7B6.Short_c).f1;_tmp7B8=(_tmp7B6.Short_c).f2;_LL376:{struct Cyc_Absyn_Int_p_struct*
_tmp1191;struct Cyc_Absyn_Int_p_struct _tmp1190;union Cyc_YYSTYPE_union _tmp118F;
yyval=(union Cyc_YYSTYPE_union)(((_tmp118F.YY11).tag=15,(((_tmp118F.YY11).f1=Cyc_Absyn_new_pat((
void*)((_tmp1191=_cycalloc(sizeof(*_tmp1191)),((_tmp1191[0]=((_tmp1190.tag=7,((
_tmp1190.f1=(void*)_tmp7B7,((_tmp1190.f2=(int)_tmp7B8,_tmp1190)))))),_tmp1191)))),
e->loc),_tmp118F))));}goto _LL372;_LL377: if(*((int*)_tmp7B2)!= 0)goto _LL379;
_tmp7B9=((struct Cyc_Absyn_Const_e_struct*)_tmp7B2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp7B2)->f1).Int_c).tag != 2)goto _LL379;_tmp7BA=(_tmp7B9.Int_c).f1;_tmp7BB=(
_tmp7B9.Int_c).f2;_LL378:{struct Cyc_Absyn_Int_p_struct*_tmp1197;struct Cyc_Absyn_Int_p_struct
_tmp1196;union Cyc_YYSTYPE_union _tmp1195;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1195.YY11).tag=15,(((_tmp1195.YY11).f1=Cyc_Absyn_new_pat((void*)((_tmp1197=
_cycalloc(sizeof(*_tmp1197)),((_tmp1197[0]=((_tmp1196.tag=7,((_tmp1196.f1=(void*)
_tmp7BA,((_tmp1196.f2=_tmp7BB,_tmp1196)))))),_tmp1197)))),e->loc),_tmp1195))));}
goto _LL372;_LL379: if(*((int*)_tmp7B2)!= 0)goto _LL37B;_tmp7BC=((struct Cyc_Absyn_Const_e_struct*)
_tmp7B2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7B2)->f1).Float_c).tag != 
4)goto _LL37B;_tmp7BD=(_tmp7BC.Float_c).f1;_LL37A:{struct Cyc_Absyn_Float_p_struct*
_tmp119D;struct Cyc_Absyn_Float_p_struct _tmp119C;union Cyc_YYSTYPE_union _tmp119B;
yyval=(union Cyc_YYSTYPE_union)(((_tmp119B.YY11).tag=15,(((_tmp119B.YY11).f1=Cyc_Absyn_new_pat((
void*)((_tmp119D=_cycalloc(sizeof(*_tmp119D)),((_tmp119D[0]=((_tmp119C.tag=9,((
_tmp119C.f1=_tmp7BD,_tmp119C)))),_tmp119D)))),e->loc),_tmp119B))));}goto _LL372;
_LL37B: if(*((int*)_tmp7B2)!= 0)goto _LL37D;_tmp7BE=((struct Cyc_Absyn_Const_e_struct*)
_tmp7B2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7B2)->f1).Null_c).tag != 
6)goto _LL37D;_LL37C:{union Cyc_YYSTYPE_union _tmp119E;yyval=(union Cyc_YYSTYPE_union)(((
_tmp119E.YY11).tag=15,(((_tmp119E.YY11).f1=Cyc_Absyn_new_pat((void*)1,e->loc),
_tmp119E))));}goto _LL372;_LL37D: if(*((int*)_tmp7B2)!= 0)goto _LL37F;_tmp7BF=((
struct Cyc_Absyn_Const_e_struct*)_tmp7B2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp7B2)->f1).String_c).tag != 5)goto _LL37F;_LL37E:{const char*_tmp119F;Cyc_Parse_err(((
_tmp119F="strings cannot occur within patterns",_tag_dynforward(_tmp119F,sizeof(
char),_get_zero_arr_size_char(_tmp119F,37)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}goto _LL372;
_LL37F: if(*((int*)_tmp7B2)!= 0)goto _LL381;_tmp7C0=((struct Cyc_Absyn_Const_e_struct*)
_tmp7B2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7B2)->f1).LongLong_c).tag
!= 3)goto _LL381;_LL380:{const char*_tmp11A0;Cyc_Parse_unimp(((_tmp11A0="long long's in patterns",
_tag_dynforward(_tmp11A0,sizeof(char),_get_zero_arr_size_char(_tmp11A0,24)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));}goto _LL372;_LL381:;_LL382: {const char*_tmp11A1;Cyc_Parse_err(((
_tmp11A1="bad constant in case",_tag_dynforward(_tmp11A1,sizeof(char),
_get_zero_arr_size_char(_tmp11A1,21)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}_LL372:;}
break;}case 369: _LL371:{struct Cyc_Absyn_UnknownId_p_struct*_tmp11A7;struct Cyc_Absyn_UnknownId_p_struct
_tmp11A6;union Cyc_YYSTYPE_union _tmp11A5;yyval=(union Cyc_YYSTYPE_union)(((
_tmp11A5.YY11).tag=15,(((_tmp11A5.YY11).f1=Cyc_Absyn_new_pat((void*)((_tmp11A7=
_cycalloc(sizeof(*_tmp11A7)),((_tmp11A7[0]=((_tmp11A6.tag=12,((_tmp11A6.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp11A6)))),_tmp11A7)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp11A5))));}break;case 370: _LL383:{const char*
_tmp11A8;if(Cyc_strcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmp11A8="as",
_tag_dynforward(_tmp11A8,sizeof(char),_get_zero_arr_size_char(_tmp11A8,3)))))!= 
0){const char*_tmp11A9;Cyc_Parse_err(((_tmp11A9="expecting `as'",_tag_dynforward(
_tmp11A9,sizeof(char),_get_zero_arr_size_char(_tmp11A9,15)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}}{struct Cyc_Absyn_Var_p_struct*
_tmp11BD;struct _tuple1*_tmp11BC;struct _dynforward_ptr*_tmp11BB;union Cyc_Absyn_Nmspace_union
_tmp11BA;struct Cyc_Absyn_Var_p_struct _tmp11B9;union Cyc_YYSTYPE_union _tmp11B8;
yyval=(union Cyc_YYSTYPE_union)(((_tmp11B8.YY11).tag=15,(((_tmp11B8.YY11).f1=Cyc_Absyn_new_pat((
void*)((_tmp11BD=_cycalloc(sizeof(*_tmp11BD)),((_tmp11BD[0]=((_tmp11B9.tag=0,((
_tmp11B9.f1=Cyc_Absyn_new_vardecl(((_tmp11BC=_cycalloc(sizeof(*_tmp11BC)),((
_tmp11BC->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp11BA.Loc_n).tag=0,_tmp11BA)),((
_tmp11BC->f2=((_tmp11BB=_cycalloc(sizeof(*_tmp11BB)),((_tmp11BB[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp11BB)))),
_tmp11BC)))))),(void*)0,0),((_tmp11B9.f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp11B9)))))),_tmp11BD)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line)),_tmp11B8))));}break;case 371: _LL384: {struct Cyc_List_List*
_tmp7DD;int _tmp7DE;struct _tuple13 _tmp7DC=*Cyc_yyget_YY12(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7DD=_tmp7DC.f1;
_tmp7DE=_tmp7DC.f2;{struct Cyc_Absyn_Tuple_p_struct*_tmp11C3;struct Cyc_Absyn_Tuple_p_struct
_tmp11C2;union Cyc_YYSTYPE_union _tmp11C1;yyval=(union Cyc_YYSTYPE_union)(((
_tmp11C1.YY11).tag=15,(((_tmp11C1.YY11).f1=Cyc_Absyn_new_pat((void*)((_tmp11C3=
_cycalloc(sizeof(*_tmp11C3)),((_tmp11C3[0]=((_tmp11C2.tag=3,((_tmp11C2.f1=
_tmp7DD,((_tmp11C2.f2=_tmp7DE,_tmp11C2)))))),_tmp11C3)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp11C1))));}
break;}case 372: _LL385: {struct Cyc_List_List*_tmp7E3;int _tmp7E4;struct _tuple13
_tmp7E2=*Cyc_yyget_YY12(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7E3=_tmp7E2.f1;_tmp7E4=_tmp7E2.f2;{struct Cyc_Absyn_UnknownCall_p_struct*
_tmp11C9;struct Cyc_Absyn_UnknownCall_p_struct _tmp11C8;union Cyc_YYSTYPE_union
_tmp11C7;yyval=(union Cyc_YYSTYPE_union)(((_tmp11C7.YY11).tag=15,(((_tmp11C7.YY11).f1=
Cyc_Absyn_new_pat((void*)((_tmp11C9=_cycalloc(sizeof(*_tmp11C9)),((_tmp11C9[0]=((
_tmp11C8.tag=13,((_tmp11C8.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmp11C8.f2=_tmp7E3,((
_tmp11C8.f3=_tmp7E4,_tmp11C8)))))))),_tmp11C9)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp11C7))));}
break;}case 373: _LL386: {struct Cyc_List_List*_tmp7E9;int _tmp7EA;struct _tuple13
_tmp7E8=*Cyc_yyget_YY16(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7E9=_tmp7E8.f1;_tmp7EA=_tmp7E8.f2;{struct Cyc_List_List*_tmp7EB=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]));{struct Cyc_Absyn_Aggr_p_struct*_tmp11D8;struct Cyc_Absyn_AggrInfo
_tmp11D7;union Cyc_Absyn_AggrInfoU_union _tmp11D6;struct Cyc_Absyn_Aggr_p_struct
_tmp11D5;union Cyc_YYSTYPE_union _tmp11D4;yyval=(union Cyc_YYSTYPE_union)(((
_tmp11D4.YY11).tag=15,(((_tmp11D4.YY11).f1=Cyc_Absyn_new_pat((void*)((_tmp11D8=
_cycalloc(sizeof(*_tmp11D8)),((_tmp11D8[0]=((_tmp11D5.tag=5,((_tmp11D5.f1=((
_tmp11D7.aggr_info=(union Cyc_Absyn_AggrInfoU_union)(((_tmp11D6.UnknownAggr).tag=
0,(((_tmp11D6.UnknownAggr).f1=(void*)((void*)0),(((_tmp11D6.UnknownAggr).f2=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp11D6)))))),((
_tmp11D7.targs=0,_tmp11D7)))),((_tmp11D5.f2=_tmp7EB,((_tmp11D5.f3=_tmp7E9,((
_tmp11D5.f4=_tmp7EA,_tmp11D5)))))))))),_tmp11D8)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp11D4))));}
break;}}case 374: _LL387:{struct Cyc_Absyn_Pointer_p_struct*_tmp11DE;struct Cyc_Absyn_Pointer_p_struct
_tmp11DD;union Cyc_YYSTYPE_union _tmp11DC;yyval=(union Cyc_YYSTYPE_union)(((
_tmp11DC.YY11).tag=15,(((_tmp11DC.YY11).f1=Cyc_Absyn_new_pat((void*)((_tmp11DE=
_cycalloc(sizeof(*_tmp11DE)),((_tmp11DE[0]=((_tmp11DD.tag=4,((_tmp11DD.f1=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp11DD)))),_tmp11DE)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp11DC))));}break;case 375: _LL388:{struct Cyc_Absyn_Pointer_p_struct*
_tmp11ED;struct Cyc_Absyn_Pointer_p_struct*_tmp11EC;struct Cyc_Absyn_Pointer_p_struct
_tmp11EB;struct Cyc_Absyn_Pointer_p_struct _tmp11EA;union Cyc_YYSTYPE_union _tmp11E9;
yyval=(union Cyc_YYSTYPE_union)(((_tmp11E9.YY11).tag=15,(((_tmp11E9.YY11).f1=Cyc_Absyn_new_pat((
void*)((_tmp11ED=_cycalloc(sizeof(*_tmp11ED)),((_tmp11ED[0]=((_tmp11EA.tag=4,((
_tmp11EA.f1=Cyc_Absyn_new_pat((void*)((_tmp11EC=_cycalloc(sizeof(*_tmp11EC)),((
_tmp11EC[0]=((_tmp11EB.tag=4,((_tmp11EB.f1=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp11EB)))),_tmp11EC)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp11EA)))),_tmp11ED)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp11E9))));}
break;case 376: _LL389:{struct Cyc_Absyn_Reference_p_struct*_tmp1201;struct _tuple1*
_tmp1200;struct _dynforward_ptr*_tmp11FF;union Cyc_Absyn_Nmspace_union _tmp11FE;
struct Cyc_Absyn_Reference_p_struct _tmp11FD;union Cyc_YYSTYPE_union _tmp11FC;yyval=(
union Cyc_YYSTYPE_union)(((_tmp11FC.YY11).tag=15,(((_tmp11FC.YY11).f1=Cyc_Absyn_new_pat((
void*)((_tmp1201=_cycalloc(sizeof(*_tmp1201)),((_tmp1201[0]=((_tmp11FD.tag=1,((
_tmp11FD.f1=Cyc_Absyn_new_vardecl(((_tmp1200=_cycalloc(sizeof(*_tmp1200)),((
_tmp1200->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp11FE.Loc_n).tag=0,_tmp11FE)),((
_tmp1200->f2=((_tmp11FF=_cycalloc(sizeof(*_tmp11FF)),((_tmp11FF[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp11FF)))),_tmp1200)))))),(
void*)0,0),((_tmp11FD.f2=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp11FD)))))),
_tmp1201)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp11FC))));}break;case 377: _LL38A:{const char*
_tmp1202;if(Cyc_strcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmp1202="as",
_tag_dynforward(_tmp1202,sizeof(char),_get_zero_arr_size_char(_tmp1202,3)))))!= 
0){const char*_tmp1203;Cyc_Parse_err(((_tmp1203="expecting `as'",_tag_dynforward(
_tmp1203,sizeof(char),_get_zero_arr_size_char(_tmp1203,15)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));}}{struct Cyc_Absyn_Reference_p_struct*
_tmp1217;struct _tuple1*_tmp1216;struct _dynforward_ptr*_tmp1215;union Cyc_Absyn_Nmspace_union
_tmp1214;struct Cyc_Absyn_Reference_p_struct _tmp1213;union Cyc_YYSTYPE_union
_tmp1212;yyval=(union Cyc_YYSTYPE_union)(((_tmp1212.YY11).tag=15,(((_tmp1212.YY11).f1=
Cyc_Absyn_new_pat((void*)((_tmp1217=_cycalloc(sizeof(*_tmp1217)),((_tmp1217[0]=((
_tmp1213.tag=1,((_tmp1213.f1=Cyc_Absyn_new_vardecl(((_tmp1216=_cycalloc(sizeof(*
_tmp1216)),((_tmp1216->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp1214.Loc_n).tag=0,
_tmp1214)),((_tmp1216->f2=((_tmp1215=_cycalloc(sizeof(*_tmp1215)),((_tmp1215[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
_tmp1215)))),_tmp1216)))))),(void*)0,0),((_tmp1213.f2=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1213)))))),_tmp1217)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line)),_tmp1212))));}break;case 378: _LL38B: {void*_tmp807=
Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_Tcutil_kind_to_bound((void*)7));{struct Cyc_Absyn_TagInt_p_struct*
_tmp1234;struct _tuple1*_tmp1233;struct _dynforward_ptr*_tmp1232;union Cyc_Absyn_Nmspace_union
_tmp1231;struct Cyc_Absyn_TagType_struct*_tmp1230;struct Cyc_Absyn_TagType_struct
_tmp122F;struct Cyc_Absyn_TagInt_p_struct _tmp122E;union Cyc_YYSTYPE_union _tmp122D;
yyval=(union Cyc_YYSTYPE_union)(((_tmp122D.YY11).tag=15,(((_tmp122D.YY11).f1=Cyc_Absyn_new_pat((
void*)((_tmp1234=_cycalloc(sizeof(*_tmp1234)),((_tmp1234[0]=((_tmp122E.tag=2,((
_tmp122E.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp807),((
_tmp122E.f2=Cyc_Absyn_new_vardecl(((_tmp1233=_cycalloc(sizeof(*_tmp1233)),((
_tmp1233->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp1231.Loc_n).tag=0,_tmp1231)),((
_tmp1233->f2=((_tmp1232=_cycalloc(sizeof(*_tmp1232)),((_tmp1232[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),_tmp1232)))),
_tmp1233)))))),(void*)((_tmp1230=_cycalloc(sizeof(*_tmp1230)),((_tmp1230[0]=((
_tmp122F.tag=18,((_tmp122F.f1=(void*)_tmp807,_tmp122F)))),_tmp1230)))),0),
_tmp122E)))))),_tmp1234)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp122D))));}
break;}case 379: _LL38C: {struct Cyc_Absyn_Tvar*_tmp810=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound((
void*)7));{struct Cyc_Absyn_TagInt_p_struct*_tmp125E;struct _tuple1*_tmp125D;
struct _dynforward_ptr*_tmp125C;union Cyc_Absyn_Nmspace_union _tmp125B;struct Cyc_Absyn_TagType_struct*
_tmp125A;struct Cyc_Absyn_VarType_struct*_tmp1259;struct Cyc_Absyn_VarType_struct
_tmp1258;struct Cyc_Absyn_TagType_struct _tmp1257;struct Cyc_Absyn_TagInt_p_struct
_tmp1256;union Cyc_YYSTYPE_union _tmp1255;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1255.YY11).tag=15,(((_tmp1255.YY11).f1=Cyc_Absyn_new_pat((void*)((_tmp125E=
_cycalloc(sizeof(*_tmp125E)),((_tmp125E[0]=((_tmp1256.tag=2,((_tmp1256.f1=
_tmp810,((_tmp1256.f2=Cyc_Absyn_new_vardecl(((_tmp125D=_cycalloc(sizeof(*
_tmp125D)),((_tmp125D->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp125B.Loc_n).tag=0,
_tmp125B)),((_tmp125D->f2=((_tmp125C=_cycalloc(sizeof(*_tmp125C)),((_tmp125C[0]=
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),
_tmp125C)))),_tmp125D)))))),(void*)((_tmp125A=_cycalloc(sizeof(*_tmp125A)),((
_tmp125A[0]=((_tmp1257.tag=18,((_tmp1257.f1=(void*)((void*)((_tmp1259=_cycalloc(
sizeof(*_tmp1259)),((_tmp1259[0]=((_tmp1258.tag=1,((_tmp1258.f1=_tmp810,_tmp1258)))),
_tmp1259))))),_tmp1257)))),_tmp125A)))),0),_tmp1256)))))),_tmp125E)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1255))));}
break;}case 380: _LL38D:{struct _tuple13*_tmp1261;union Cyc_YYSTYPE_union _tmp1260;
yyval=(union Cyc_YYSTYPE_union)(((_tmp1260.YY12).tag=16,(((_tmp1260.YY12).f1=((
_tmp1261=_cycalloc(sizeof(*_tmp1261)),((_tmp1261->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])),((_tmp1261->f2=0,_tmp1261)))))),
_tmp1260))));}break;case 381: _LL38E:{struct _tuple13*_tmp1264;union Cyc_YYSTYPE_union
_tmp1263;yyval=(union Cyc_YYSTYPE_union)(((_tmp1263.YY12).tag=16,(((_tmp1263.YY12).f1=((
_tmp1264=_cycalloc(sizeof(*_tmp1264)),((_tmp1264->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmp1264->f2=1,
_tmp1264)))))),_tmp1263))));}break;case 382: _LL38F:{struct _tuple13*_tmp1267;union
Cyc_YYSTYPE_union _tmp1266;yyval=(union Cyc_YYSTYPE_union)(((_tmp1266.YY12).tag=16,(((
_tmp1266.YY12).f1=((_tmp1267=_cycalloc(sizeof(*_tmp1267)),((_tmp1267->f1=0,((
_tmp1267->f2=1,_tmp1267)))))),_tmp1266))));}break;case 383: _LL390:{struct Cyc_List_List*
_tmp126A;union Cyc_YYSTYPE_union _tmp1269;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1269.YY13).tag=17,(((_tmp1269.YY13).f1=((_tmp126A=_cycalloc(sizeof(*_tmp126A)),((
_tmp126A->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmp126A->tl=0,_tmp126A)))))),_tmp1269))));}break;case 384:
_LL391:{struct Cyc_List_List*_tmp126D;union Cyc_YYSTYPE_union _tmp126C;yyval=(union
Cyc_YYSTYPE_union)(((_tmp126C.YY13).tag=17,(((_tmp126C.YY13).f1=((_tmp126D=
_cycalloc(sizeof(*_tmp126D)),((_tmp126D->hd=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmp126D->tl=Cyc_yyget_YY13(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp126D)))))),
_tmp126C))));}break;case 385: _LL392:{struct _tuple14*_tmp1270;union Cyc_YYSTYPE_union
_tmp126F;yyval=(union Cyc_YYSTYPE_union)(((_tmp126F.YY14).tag=18,(((_tmp126F.YY14).f1=((
_tmp1270=_cycalloc(sizeof(*_tmp1270)),((_tmp1270->f1=0,((_tmp1270->f2=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1270)))))),
_tmp126F))));}break;case 386: _LL393:{struct _tuple14*_tmp1273;union Cyc_YYSTYPE_union
_tmp1272;yyval=(union Cyc_YYSTYPE_union)(((_tmp1272.YY14).tag=18,(((_tmp1272.YY14).f1=((
_tmp1273=_cycalloc(sizeof(*_tmp1273)),((_tmp1273->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),((_tmp1273->f2=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1273)))))),
_tmp1272))));}break;case 387: _LL394:{struct _tuple13*_tmp1276;union Cyc_YYSTYPE_union
_tmp1275;yyval=(union Cyc_YYSTYPE_union)(((_tmp1275.YY16).tag=20,(((_tmp1275.YY16).f1=((
_tmp1276=_cycalloc(sizeof(*_tmp1276)),((_tmp1276->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])),((_tmp1276->f2=0,_tmp1276)))))),
_tmp1275))));}break;case 388: _LL395:{struct _tuple13*_tmp1279;union Cyc_YYSTYPE_union
_tmp1278;yyval=(union Cyc_YYSTYPE_union)(((_tmp1278.YY16).tag=20,(((_tmp1278.YY16).f1=((
_tmp1279=_cycalloc(sizeof(*_tmp1279)),((_tmp1279->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),((_tmp1279->f2=1,
_tmp1279)))))),_tmp1278))));}break;case 389: _LL396:{struct _tuple13*_tmp127C;union
Cyc_YYSTYPE_union _tmp127B;yyval=(union Cyc_YYSTYPE_union)(((_tmp127B.YY16).tag=20,(((
_tmp127B.YY16).f1=((_tmp127C=_cycalloc(sizeof(*_tmp127C)),((_tmp127C->f1=0,((
_tmp127C->f2=1,_tmp127C)))))),_tmp127B))));}break;case 390: _LL397:{struct Cyc_List_List*
_tmp127F;union Cyc_YYSTYPE_union _tmp127E;yyval=(union Cyc_YYSTYPE_union)(((
_tmp127E.YY15).tag=19,(((_tmp127E.YY15).f1=((_tmp127F=_cycalloc(sizeof(*_tmp127F)),((
_tmp127F->hd=Cyc_yyget_YY14(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmp127F->tl=0,_tmp127F)))))),_tmp127E))));}break;case 391:
_LL398:{struct Cyc_List_List*_tmp1282;union Cyc_YYSTYPE_union _tmp1281;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1281.YY15).tag=19,(((_tmp1281.YY15).f1=((_tmp1282=
_cycalloc(sizeof(*_tmp1282)),((_tmp1282->hd=Cyc_yyget_YY14(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmp1282->tl=Cyc_yyget_YY15(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp1282)))))),
_tmp1281))));}break;case 392: _LL399: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 393: _LL39A:{union Cyc_YYSTYPE_union _tmp1283;yyval=(
union Cyc_YYSTYPE_union)(((_tmp1283.YY3).tag=7,(((_tmp1283.YY3).f1=Cyc_Absyn_seq_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1283))));}
break;case 394: _LL39B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 395: _LL39C:{union Cyc_YYSTYPE_union _tmp1284;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1284.YY3).tag=7,(((_tmp1284.YY3).f1=Cyc_Absyn_assignop_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY8(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1284))));}
break;case 396: _LL39D:{union Cyc_YYSTYPE_union _tmp1285;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1285.YY8).tag=12,(((_tmp1285.YY8).f1=0,_tmp1285))));}break;case 397: _LL39E:{
struct Cyc_Core_Opt*_tmp1288;union Cyc_YYSTYPE_union _tmp1287;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1287.YY8).tag=12,(((_tmp1287.YY8).f1=((_tmp1288=_cycalloc(sizeof(*_tmp1288)),((
_tmp1288->v=(void*)((void*)1),_tmp1288)))),_tmp1287))));}break;case 398: _LL39F:{
struct Cyc_Core_Opt*_tmp128B;union Cyc_YYSTYPE_union _tmp128A;yyval=(union Cyc_YYSTYPE_union)(((
_tmp128A.YY8).tag=12,(((_tmp128A.YY8).f1=((_tmp128B=_cycalloc(sizeof(*_tmp128B)),((
_tmp128B->v=(void*)((void*)3),_tmp128B)))),_tmp128A))));}break;case 399: _LL3A0:{
struct Cyc_Core_Opt*_tmp128E;union Cyc_YYSTYPE_union _tmp128D;yyval=(union Cyc_YYSTYPE_union)(((
_tmp128D.YY8).tag=12,(((_tmp128D.YY8).f1=((_tmp128E=_cycalloc(sizeof(*_tmp128E)),((
_tmp128E->v=(void*)((void*)4),_tmp128E)))),_tmp128D))));}break;case 400: _LL3A1:{
struct Cyc_Core_Opt*_tmp1291;union Cyc_YYSTYPE_union _tmp1290;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1290.YY8).tag=12,(((_tmp1290.YY8).f1=((_tmp1291=_cycalloc(sizeof(*_tmp1291)),((
_tmp1291->v=(void*)((void*)0),_tmp1291)))),_tmp1290))));}break;case 401: _LL3A2:{
struct Cyc_Core_Opt*_tmp1294;union Cyc_YYSTYPE_union _tmp1293;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1293.YY8).tag=12,(((_tmp1293.YY8).f1=((_tmp1294=_cycalloc(sizeof(*_tmp1294)),((
_tmp1294->v=(void*)((void*)2),_tmp1294)))),_tmp1293))));}break;case 402: _LL3A3:{
struct Cyc_Core_Opt*_tmp1297;union Cyc_YYSTYPE_union _tmp1296;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1296.YY8).tag=12,(((_tmp1296.YY8).f1=((_tmp1297=_cycalloc(sizeof(*_tmp1297)),((
_tmp1297->v=(void*)((void*)16),_tmp1297)))),_tmp1296))));}break;case 403: _LL3A4:{
struct Cyc_Core_Opt*_tmp129A;union Cyc_YYSTYPE_union _tmp1299;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1299.YY8).tag=12,(((_tmp1299.YY8).f1=((_tmp129A=_cycalloc(sizeof(*_tmp129A)),((
_tmp129A->v=(void*)((void*)17),_tmp129A)))),_tmp1299))));}break;case 404: _LL3A5:{
struct Cyc_Core_Opt*_tmp129D;union Cyc_YYSTYPE_union _tmp129C;yyval=(union Cyc_YYSTYPE_union)(((
_tmp129C.YY8).tag=12,(((_tmp129C.YY8).f1=((_tmp129D=_cycalloc(sizeof(*_tmp129D)),((
_tmp129D->v=(void*)((void*)13),_tmp129D)))),_tmp129C))));}break;case 405: _LL3A6:{
struct Cyc_Core_Opt*_tmp12A0;union Cyc_YYSTYPE_union _tmp129F;yyval=(union Cyc_YYSTYPE_union)(((
_tmp129F.YY8).tag=12,(((_tmp129F.YY8).f1=((_tmp12A0=_cycalloc(sizeof(*_tmp12A0)),((
_tmp12A0->v=(void*)((void*)15),_tmp12A0)))),_tmp129F))));}break;case 406: _LL3A7:{
struct Cyc_Core_Opt*_tmp12A3;union Cyc_YYSTYPE_union _tmp12A2;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12A2.YY8).tag=12,(((_tmp12A2.YY8).f1=((_tmp12A3=_cycalloc(sizeof(*_tmp12A3)),((
_tmp12A3->v=(void*)((void*)14),_tmp12A3)))),_tmp12A2))));}break;case 407: _LL3A8:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 408:
_LL3A9:{union Cyc_YYSTYPE_union _tmp12A4;yyval=(union Cyc_YYSTYPE_union)(((_tmp12A4.YY3).tag=
7,(((_tmp12A4.YY3).f1=Cyc_Absyn_conditional_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12A4))));}
break;case 409: _LL3AA:{union Cyc_YYSTYPE_union _tmp12A5;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12A5.YY3).tag=7,(((_tmp12A5.YY3).f1=Cyc_Absyn_throw_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12A5))));}
break;case 410: _LL3AB:{union Cyc_YYSTYPE_union _tmp12A6;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12A6.YY3).tag=7,(((_tmp12A6.YY3).f1=Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12A6))));}
break;case 411: _LL3AC:{union Cyc_YYSTYPE_union _tmp12A7;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12A7.YY3).tag=7,(((_tmp12A7.YY3).f1=Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12A7))));}
break;case 412: _LL3AD:{union Cyc_YYSTYPE_union _tmp12A8;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12A8.YY3).tag=7,(((_tmp12A8.YY3).f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12A8))));}
break;case 413: _LL3AE:{union Cyc_YYSTYPE_union _tmp12A9;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12A9.YY3).tag=7,(((_tmp12A9.YY3).f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12A9))));}
break;case 414: _LL3AF: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 415: _LL3B0: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 416: _LL3B1:{union Cyc_YYSTYPE_union _tmp12AA;yyval=(
union Cyc_YYSTYPE_union)(((_tmp12AA.YY3).tag=7,(((_tmp12AA.YY3).f1=Cyc_Absyn_or_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12AA))));}
break;case 417: _LL3B2: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 418: _LL3B3:{union Cyc_YYSTYPE_union _tmp12AB;yyval=(union
Cyc_YYSTYPE_union)(((_tmp12AB.YY3).tag=7,(((_tmp12AB.YY3).f1=Cyc_Absyn_and_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12AB))));}
break;case 419: _LL3B4: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 420: _LL3B5:{union Cyc_YYSTYPE_union _tmp12AC;yyval=(union
Cyc_YYSTYPE_union)(((_tmp12AC.YY3).tag=7,(((_tmp12AC.YY3).f1=Cyc_Absyn_prim2_exp((
void*)14,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12AC))));}
break;case 421: _LL3B6: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 422: _LL3B7:{union Cyc_YYSTYPE_union _tmp12AD;yyval=(union
Cyc_YYSTYPE_union)(((_tmp12AD.YY3).tag=7,(((_tmp12AD.YY3).f1=Cyc_Absyn_prim2_exp((
void*)15,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12AD))));}
break;case 423: _LL3B8: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 424: _LL3B9:{union Cyc_YYSTYPE_union _tmp12AE;yyval=(union
Cyc_YYSTYPE_union)(((_tmp12AE.YY3).tag=7,(((_tmp12AE.YY3).f1=Cyc_Absyn_prim2_exp((
void*)13,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12AE))));}
break;case 425: _LL3BA: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 426: _LL3BB:{union Cyc_YYSTYPE_union _tmp12AF;yyval=(union
Cyc_YYSTYPE_union)(((_tmp12AF.YY3).tag=7,(((_tmp12AF.YY3).f1=Cyc_Absyn_eq_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12AF))));}
break;case 427: _LL3BC:{union Cyc_YYSTYPE_union _tmp12B0;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12B0.YY3).tag=7,(((_tmp12B0.YY3).f1=Cyc_Absyn_neq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12B0))));}
break;case 428: _LL3BD: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 429: _LL3BE:{union Cyc_YYSTYPE_union _tmp12B1;yyval=(union
Cyc_YYSTYPE_union)(((_tmp12B1.YY3).tag=7,(((_tmp12B1.YY3).f1=Cyc_Absyn_lt_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12B1))));}
break;case 430: _LL3BF:{union Cyc_YYSTYPE_union _tmp12B2;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12B2.YY3).tag=7,(((_tmp12B2.YY3).f1=Cyc_Absyn_gt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12B2))));}
break;case 431: _LL3C0:{union Cyc_YYSTYPE_union _tmp12B3;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12B3.YY3).tag=7,(((_tmp12B3.YY3).f1=Cyc_Absyn_lte_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12B3))));}
break;case 432: _LL3C1:{union Cyc_YYSTYPE_union _tmp12B4;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12B4.YY3).tag=7,(((_tmp12B4.YY3).f1=Cyc_Absyn_gte_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12B4))));}
break;case 433: _LL3C2: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 434: _LL3C3:{union Cyc_YYSTYPE_union _tmp12B5;yyval=(union
Cyc_YYSTYPE_union)(((_tmp12B5.YY3).tag=7,(((_tmp12B5.YY3).f1=Cyc_Absyn_prim2_exp((
void*)16,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12B5))));}
break;case 435: _LL3C4:{union Cyc_YYSTYPE_union _tmp12B6;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12B6.YY3).tag=7,(((_tmp12B6.YY3).f1=Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12B6))));}
break;case 436: _LL3C5: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 437: _LL3C6:{union Cyc_YYSTYPE_union _tmp12B7;yyval=(union
Cyc_YYSTYPE_union)(((_tmp12B7.YY3).tag=7,(((_tmp12B7.YY3).f1=Cyc_Absyn_prim2_exp((
void*)0,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12B7))));}
break;case 438: _LL3C7:{union Cyc_YYSTYPE_union _tmp12B8;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12B8.YY3).tag=7,(((_tmp12B8.YY3).f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12B8))));}
break;case 439: _LL3C8: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 440: _LL3C9:{union Cyc_YYSTYPE_union _tmp12B9;yyval=(union
Cyc_YYSTYPE_union)(((_tmp12B9.YY3).tag=7,(((_tmp12B9.YY3).f1=Cyc_Absyn_prim2_exp((
void*)1,Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12B9))));}
break;case 441: _LL3CA:{union Cyc_YYSTYPE_union _tmp12BA;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12BA.YY3).tag=7,(((_tmp12BA.YY3).f1=Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12BA))));}
break;case 442: _LL3CB:{union Cyc_YYSTYPE_union _tmp12BB;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12BB.YY3).tag=7,(((_tmp12BB.YY3).f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12BB))));}
break;case 443: _LL3CC: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 444: _LL3CD:{union Cyc_YYSTYPE_union _tmp12BC;yyval=(union
Cyc_YYSTYPE_union)(((_tmp12BC.YY3).tag=7,(((_tmp12BC.YY3).f1=Cyc_Absyn_cast_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp12BC))));}break;case 445: _LL3CE: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 446: _LL3CF:{union
Cyc_YYSTYPE_union _tmp12BD;yyval=(union Cyc_YYSTYPE_union)(((_tmp12BD.YY3).tag=7,(((
_tmp12BD.YY3).f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12BD))));}
break;case 447: _LL3D0:{union Cyc_YYSTYPE_union _tmp12BE;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12BE.YY3).tag=7,(((_tmp12BE.YY3).f1=Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12BE))));}
break;case 448: _LL3D1:{union Cyc_YYSTYPE_union _tmp12BF;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12BF.YY3).tag=7,(((_tmp12BF.YY3).f1=Cyc_Absyn_address_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12BF))));}
break;case 449: _LL3D2:{union Cyc_YYSTYPE_union _tmp12C0;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12C0.YY3).tag=7,(((_tmp12C0.YY3).f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12C0))));}
break;case 450: _LL3D3: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 451: _LL3D4:{union Cyc_YYSTYPE_union _tmp12C1;yyval=(union
Cyc_YYSTYPE_union)(((_tmp12C1.YY3).tag=7,(((_tmp12C1.YY3).f1=Cyc_Absyn_prim1_exp(
Cyc_yyget_YY7(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12C1))));}
break;case 452: _LL3D5:{union Cyc_YYSTYPE_union _tmp12C2;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12C2.YY3).tag=7,(((_tmp12C2.YY3).f1=Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12C2))));}
break;case 453: _LL3D6:{union Cyc_YYSTYPE_union _tmp12C3;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12C3.YY3).tag=7,(((_tmp12C3.YY3).f1=Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12C3))));}
break;case 454: _LL3D7:{struct Cyc_Absyn_StructField_struct*_tmp12CD;struct
_dynforward_ptr*_tmp12CC;struct Cyc_Absyn_StructField_struct _tmp12CB;union Cyc_YYSTYPE_union
_tmp12CA;yyval=(union Cyc_YYSTYPE_union)(((_tmp12CA.YY3).tag=7,(((_tmp12CA.YY3).f1=
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)((_tmp12CD=_cycalloc(sizeof(*_tmp12CD)),((
_tmp12CD[0]=((_tmp12CB.tag=0,((_tmp12CB.f1=((_tmp12CC=_cycalloc(sizeof(*_tmp12CC)),((
_tmp12CC[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmp12CC)))),_tmp12CB)))),_tmp12CD)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12CA))));}
break;case 455: _LL3D8:{struct Cyc_Absyn_TupleIndex_struct*_tmp12D3;struct Cyc_Absyn_TupleIndex_struct
_tmp12D2;union Cyc_YYSTYPE_union _tmp12D1;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12D1.YY3).tag=7,(((_tmp12D1.YY3).f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)((
_tmp12D3=_cycalloc_atomic(sizeof(*_tmp12D3)),((_tmp12D3[0]=((_tmp12D2.tag=1,((
_tmp12D2.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f2,_tmp12D2)))),_tmp12D3)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12D1))));}
break;case 456: _LL3D9: {struct Cyc_Position_Segment*_tmp871=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);struct Cyc_List_List*
_tmp872=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
_tmp871,Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));{
union Cyc_YYSTYPE_union _tmp12D4;yyval=(union Cyc_YYSTYPE_union)(((_tmp12D4.YY3).tag=
7,(((_tmp12D4.YY3).f1=Cyc_Absyn_gentyp_exp(_tmp872,(*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12D4))));}
break;}case 457: _LL3DA:{struct Cyc_Absyn_Malloc_e_struct*_tmp12DE;struct Cyc_Absyn_MallocInfo
_tmp12DD;struct Cyc_Absyn_Malloc_e_struct _tmp12DC;union Cyc_YYSTYPE_union _tmp12DB;
yyval=(union Cyc_YYSTYPE_union)(((_tmp12DB.YY3).tag=7,(((_tmp12DB.YY3).f1=Cyc_Absyn_new_exp((
void*)((_tmp12DE=_cycalloc(sizeof(*_tmp12DE)),((_tmp12DE[0]=((_tmp12DC.tag=35,((
_tmp12DC.f1=((_tmp12DD.is_calloc=0,((_tmp12DD.rgn=0,((_tmp12DD.elt_type=0,((
_tmp12DD.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmp12DD.fat_result=0,_tmp12DD)))))))))),_tmp12DC)))),
_tmp12DE)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp12DB))));}break;case 458: _LL3DB:{struct Cyc_Absyn_Malloc_e_struct*
_tmp12E8;struct Cyc_Absyn_MallocInfo _tmp12E7;struct Cyc_Absyn_Malloc_e_struct
_tmp12E6;union Cyc_YYSTYPE_union _tmp12E5;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12E5.YY3).tag=7,(((_tmp12E5.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmp12E8=
_cycalloc(sizeof(*_tmp12E8)),((_tmp12E8[0]=((_tmp12E6.tag=35,((_tmp12E6.f1=((
_tmp12E7.is_calloc=0,((_tmp12E7.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),((_tmp12E7.elt_type=0,((
_tmp12E7.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),((_tmp12E7.fat_result=0,_tmp12E7)))))))))),_tmp12E6)))),
_tmp12E8)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp12E5))));}break;case 459: _LL3DC: {void*_tmp87D;
struct _tuple2 _tmp87C=*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp87D=_tmp87C.f3;{struct Cyc_Absyn_Malloc_e_struct*_tmp12F7;
struct Cyc_Absyn_MallocInfo _tmp12F6;void**_tmp12F5;struct Cyc_Absyn_Malloc_e_struct
_tmp12F4;union Cyc_YYSTYPE_union _tmp12F3;yyval=(union Cyc_YYSTYPE_union)(((
_tmp12F3.YY3).tag=7,(((_tmp12F3.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmp12F7=
_cycalloc(sizeof(*_tmp12F7)),((_tmp12F7[0]=((_tmp12F4.tag=35,((_tmp12F4.f1=((
_tmp12F6.is_calloc=1,((_tmp12F6.rgn=0,((_tmp12F6.elt_type=((_tmp12F5=_cycalloc(
sizeof(*_tmp12F5)),((_tmp12F5[0]=_tmp87D,_tmp12F5)))),((_tmp12F6.num_elts=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),((_tmp12F6.fat_result=
0,_tmp12F6)))))))))),_tmp12F4)))),_tmp12F7)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp12F3))));}
break;}case 460: _LL3DD: {void*_tmp884;struct _tuple2 _tmp883=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp884=_tmp883.f3;{
struct Cyc_Absyn_Malloc_e_struct*_tmp1306;struct Cyc_Absyn_MallocInfo _tmp1305;void**
_tmp1304;struct Cyc_Absyn_Malloc_e_struct _tmp1303;union Cyc_YYSTYPE_union _tmp1302;
yyval=(union Cyc_YYSTYPE_union)(((_tmp1302.YY3).tag=7,(((_tmp1302.YY3).f1=Cyc_Absyn_new_exp((
void*)((_tmp1306=_cycalloc(sizeof(*_tmp1306)),((_tmp1306[0]=((_tmp1303.tag=35,((
_tmp1303.f1=((_tmp1305.is_calloc=1,((_tmp1305.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 8)]),((_tmp1305.elt_type=((
_tmp1304=_cycalloc(sizeof(*_tmp1304)),((_tmp1304[0]=_tmp884,_tmp1304)))),((
_tmp1305.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 6)]),((_tmp1305.fat_result=0,_tmp1305)))))))))),_tmp1303)))),
_tmp1306)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 10)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp1302))));}break;}case 461: _LL3DE:{struct Cyc_Absyn_Swap_e_struct*
_tmp130C;struct Cyc_Absyn_Swap_e_struct _tmp130B;union Cyc_YYSTYPE_union _tmp130A;
yyval=(union Cyc_YYSTYPE_union)(((_tmp130A.YY3).tag=7,(((_tmp130A.YY3).f1=Cyc_Absyn_new_exp((
void*)((_tmp130C=_cycalloc(sizeof(*_tmp130C)),((_tmp130C[0]=((_tmp130B.tag=36,((
_tmp130B.f1=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 3)]),((_tmp130B.f2=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),_tmp130B)))))),_tmp130C)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp130A))));}
break;case 462: _LL3DF:{union Cyc_YYSTYPE_union _tmp130D;yyval=(union Cyc_YYSTYPE_union)(((
_tmp130D.YY7).tag=11,(((_tmp130D.YY7).f1=(void*)((void*)12),_tmp130D))));}break;
case 463: _LL3E0:{union Cyc_YYSTYPE_union _tmp130E;yyval=(union Cyc_YYSTYPE_union)(((
_tmp130E.YY7).tag=11,(((_tmp130E.YY7).f1=(void*)((void*)11),_tmp130E))));}break;
case 464: _LL3E1:{union Cyc_YYSTYPE_union _tmp130F;yyval=(union Cyc_YYSTYPE_union)(((
_tmp130F.YY7).tag=11,(((_tmp130F.YY7).f1=(void*)((void*)2),_tmp130F))));}break;
case 465: _LL3E2: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 466: _LL3E3:{union Cyc_YYSTYPE_union _tmp1310;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1310.YY3).tag=7,(((_tmp1310.YY3).f1=Cyc_Absyn_subscript_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1310))));}
break;case 467: _LL3E4:{union Cyc_YYSTYPE_union _tmp1311;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1311.YY3).tag=7,(((_tmp1311.YY3).f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1311))));}
break;case 468: _LL3E5:{union Cyc_YYSTYPE_union _tmp1312;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1312.YY3).tag=7,(((_tmp1312.YY3).f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1312))));}
break;case 469: _LL3E6:{struct _dynforward_ptr*_tmp1315;union Cyc_YYSTYPE_union
_tmp1314;yyval=(union Cyc_YYSTYPE_union)(((_tmp1314.YY3).tag=7,(((_tmp1314.YY3).f1=
Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),((_tmp1315=_cycalloc(sizeof(*_tmp1315)),((_tmp1315[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1315)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1314))));}
break;case 470: _LL3E7:{struct _dynforward_ptr*_tmp1318;union Cyc_YYSTYPE_union
_tmp1317;yyval=(union Cyc_YYSTYPE_union)(((_tmp1317.YY3).tag=7,(((_tmp1317.YY3).f1=
Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),((_tmp1318=_cycalloc(sizeof(*_tmp1318)),((_tmp1318[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1318)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1317))));}
break;case 471: _LL3E8:{union Cyc_YYSTYPE_union _tmp1319;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1319.YY3).tag=7,(((_tmp1319.YY3).f1=Cyc_Absyn_post_inc_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1319))));}
break;case 472: _LL3E9:{union Cyc_YYSTYPE_union _tmp131A;yyval=(union Cyc_YYSTYPE_union)(((
_tmp131A.YY3).tag=7,(((_tmp131A.YY3).f1=Cyc_Absyn_post_dec_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp131A))));}
break;case 473: _LL3EA:{struct Cyc_Absyn_CompoundLit_e_struct*_tmp1320;struct Cyc_Absyn_CompoundLit_e_struct
_tmp131F;union Cyc_YYSTYPE_union _tmp131E;yyval=(union Cyc_YYSTYPE_union)(((
_tmp131E.YY3).tag=7,(((_tmp131E.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmp1320=
_cycalloc(sizeof(*_tmp1320)),((_tmp1320[0]=((_tmp131F.tag=27,((_tmp131F.f1=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmp131F.f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),_tmp131F)))))),
_tmp1320)))),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp131E))));}break;case 474: _LL3EB:{struct Cyc_Absyn_CompoundLit_e_struct*
_tmp1326;struct Cyc_Absyn_CompoundLit_e_struct _tmp1325;union Cyc_YYSTYPE_union
_tmp1324;yyval=(union Cyc_YYSTYPE_union)(((_tmp1324.YY3).tag=7,(((_tmp1324.YY3).f1=
Cyc_Absyn_new_exp((void*)((_tmp1326=_cycalloc(sizeof(*_tmp1326)),((_tmp1326[0]=((
_tmp1325.tag=27,((_tmp1325.f1=Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 5)]),((_tmp1325.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),_tmp1325)))))),_tmp1326)))),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1324))));}
break;case 475: _LL3EC:{struct Cyc_Absyn_UnknownId_e_struct*_tmp132C;struct Cyc_Absyn_UnknownId_e_struct
_tmp132B;union Cyc_YYSTYPE_union _tmp132A;yyval=(union Cyc_YYSTYPE_union)(((
_tmp132A.YY3).tag=7,(((_tmp132A.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmp132C=
_cycalloc(sizeof(*_tmp132C)),((_tmp132C[0]=((_tmp132B.tag=2,((_tmp132B.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp132B)))),_tmp132C)))),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp132A))));}break;case 476: _LL3ED: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 477: _LL3EE:{union
Cyc_YYSTYPE_union _tmp132D;yyval=(union Cyc_YYSTYPE_union)(((_tmp132D.YY3).tag=7,(((
_tmp132D.YY3).f1=Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp132D))));}
break;case 478: _LL3EF: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 479: _LL3F0:{union Cyc_YYSTYPE_union _tmp132E;yyval=(
union Cyc_YYSTYPE_union)(((_tmp132E.YY3).tag=7,(((_tmp132E.YY3).f1=Cyc_Absyn_noinstantiate_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp132E))));}
break;case 480: _LL3F1:{union Cyc_YYSTYPE_union _tmp132F;yyval=(union Cyc_YYSTYPE_union)(((
_tmp132F.YY3).tag=7,(((_tmp132F.YY3).f1=Cyc_Absyn_instantiate_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp132F))));}
break;case 481: _LL3F2:{union Cyc_YYSTYPE_union _tmp1330;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1330.YY3).tag=7,(((_tmp1330.YY3).f1=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1330))));}
break;case 482: _LL3F3:{struct Cyc_Absyn_Struct_e_struct*_tmp1336;struct Cyc_Absyn_Struct_e_struct
_tmp1335;union Cyc_YYSTYPE_union _tmp1334;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1334.YY3).tag=7,(((_tmp1334.YY3).f1=Cyc_Absyn_new_exp((void*)((_tmp1336=
_cycalloc(sizeof(*_tmp1336)),((_tmp1336[0]=((_tmp1335.tag=30,((_tmp1335.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),((_tmp1335.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),((_tmp1335.f3=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),((_tmp1335.f4=0,
_tmp1335)))))))))),_tmp1336)))),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1334))));}
break;case 483: _LL3F4:{union Cyc_YYSTYPE_union _tmp1337;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1337.YY3).tag=7,(((_tmp1337.YY3).f1=Cyc_Absyn_stmt_exp(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1337))));}
break;case 484: _LL3F5:{union Cyc_YYSTYPE_union _tmp1338;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1338.YY5).tag=9,(((_tmp1338.YY5).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])),_tmp1338))));}break;case 485: _LL3F6:{struct Cyc_List_List*
_tmp133B;union Cyc_YYSTYPE_union _tmp133A;yyval=(union Cyc_YYSTYPE_union)(((
_tmp133A.YY5).tag=9,(((_tmp133A.YY5).f1=((_tmp133B=_cycalloc(sizeof(*_tmp133B)),((
_tmp133B->hd=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),((_tmp133B->tl=0,_tmp133B)))))),_tmp133A))));}break;case 486:
_LL3F7:{struct Cyc_List_List*_tmp133E;union Cyc_YYSTYPE_union _tmp133D;yyval=(union
Cyc_YYSTYPE_union)(((_tmp133D.YY5).tag=9,(((_tmp133D.YY5).f1=((_tmp133E=
_cycalloc(sizeof(*_tmp133E)),((_tmp133E->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),((_tmp133E->tl=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),_tmp133E)))))),
_tmp133D))));}break;case 487: _LL3F8:{union Cyc_YYSTYPE_union _tmp133F;yyval=(union
Cyc_YYSTYPE_union)(((_tmp133F.YY3).tag=7,(((_tmp133F.YY3).f1=Cyc_Absyn_int_exp((
Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(
Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp133F))));}break;case 488: _LL3F9:{union Cyc_YYSTYPE_union
_tmp1340;yyval=(union Cyc_YYSTYPE_union)(((_tmp1340.YY3).tag=7,(((_tmp1340.YY3).f1=
Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),_tmp1340))));}break;case 489: _LL3FA:{union Cyc_YYSTYPE_union
_tmp1341;yyval=(union Cyc_YYSTYPE_union)(((_tmp1341.YY3).tag=7,(((_tmp1341.YY3).f1=
Cyc_Absyn_float_exp(Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1341))));}
break;case 490: _LL3FB:{union Cyc_YYSTYPE_union _tmp1342;yyval=(union Cyc_YYSTYPE_union)(((
_tmp1342.YY3).tag=7,(((_tmp1342.YY3).f1=Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)),_tmp1342))));}
break;case 491: _LL3FC:{struct _tuple1*_tmp134B;struct _dynforward_ptr*_tmp134A;
union Cyc_Absyn_Nmspace_union _tmp1349;union Cyc_YYSTYPE_union _tmp1348;yyval=(union
Cyc_YYSTYPE_union)(((_tmp1348.QualId_tok).tag=4,(((_tmp1348.QualId_tok).f1=((
_tmp134B=_cycalloc(sizeof(*_tmp134B)),((_tmp134B->f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp1349.Rel_n).tag=1,(((_tmp1349.Rel_n).f1=0,_tmp1349)))),((_tmp134B->f2=((
_tmp134A=_cycalloc(sizeof(*_tmp134A)),((_tmp134A[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp134A)))),_tmp134B)))))),
_tmp1348))));}break;case 492: _LL3FD: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 493: _LL3FE:{struct _tuple1*_tmp1354;struct
_dynforward_ptr*_tmp1353;union Cyc_Absyn_Nmspace_union _tmp1352;union Cyc_YYSTYPE_union
_tmp1351;yyval=(union Cyc_YYSTYPE_union)(((_tmp1351.QualId_tok).tag=4,(((_tmp1351.QualId_tok).f1=((
_tmp1354=_cycalloc(sizeof(*_tmp1354)),((_tmp1354->f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp1352.Rel_n).tag=1,(((_tmp1352.Rel_n).f1=0,_tmp1352)))),((_tmp1354->f2=((
_tmp1353=_cycalloc(sizeof(*_tmp1353)),((_tmp1353[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),_tmp1353)))),_tmp1354)))))),
_tmp1351))));}break;case 494: _LL3FF: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 495: _LL400: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 496: _LL401: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 497: _LL402:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 498:
_LL403: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
499: _LL404: break;case 500: _LL405: yylex_buf->lex_curr_pos -=1;break;default: _LL406:
break;}yyvsp_offset -=yylen;yyssp_offset -=yylen;yylsp_offset -=yylen;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=yyval;++ yylsp_offset;if(
yylen == 0){(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line=
Cyc_yylloc.first_line;(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_column=
Cyc_yylloc.first_column;(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line;(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_column=(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_column;}else{(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_line;(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_column=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_column;}
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(501,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(142,yyn - 133)]+ yyss[
_check_known_subscript_notnull(10000,yyssp_offset)];if((yystate >= 0  && yystate <= 
6421) && Cyc_yycheck[_check_known_subscript_notnull(6422,yystate)]== yyss[
_check_known_subscript_notnull(10000,yyssp_offset)])yystate=(int)Cyc_yytable[
_check_known_subscript_notnull(6422,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(142,yyn - 133)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(1007,yystate)];if(yyn > - 32768  && yyn < 6421){int
sze=0;struct _dynforward_ptr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 275 / 
sizeof(char*);++ x){if(Cyc_yycheck[_check_known_subscript_notnull(6422,x + yyn)]== 
x)(sze +=Cyc_strlen((struct _dynforward_ptr)Cyc_yytname[
_check_known_subscript_notnull(275,x)])+ 15,count ++);}{unsigned int _tmp1359;
unsigned int _tmp1358;struct _dynforward_ptr _tmp1357;char*_tmp1356;unsigned int
_tmp1355;msg=((_tmp1355=(unsigned int)(sze + 15),((_tmp1356=(char*)_region_malloc(
yyregion,_check_times(sizeof(char),_tmp1355 + 1)),((_tmp1357=_tag_dynforward(
_tmp1356,sizeof(char),_tmp1355 + 1),((((_tmp1358=_tmp1355,((_tmp135A(& _tmp1359,&
_tmp1358,& _tmp1356),_tmp1356[_tmp1358]=(char)0)))),_tmp1357))))))));}{const char*
_tmp135B;Cyc_strcpy(msg,((_tmp135B="parse error",_tag_dynforward(_tmp135B,
sizeof(char),_get_zero_arr_size_char(_tmp135B,12)))));}if(count < 5){count=0;for(
x=yyn < 0?- yyn: 0;x < 275 / sizeof(char*);++ x){if(Cyc_yycheck[
_check_known_subscript_notnull(6422,x + yyn)]== x){{const char*_tmp135D;const char*
_tmp135C;Cyc_strcat(msg,(struct _dynforward_ptr)(count == 0?(struct _dynforward_ptr)((
_tmp135D=", expecting `",_tag_dynforward(_tmp135D,sizeof(char),
_get_zero_arr_size_char(_tmp135D,14)))):(struct _dynforward_ptr)((_tmp135C=" or `",
_tag_dynforward(_tmp135C,sizeof(char),_get_zero_arr_size_char(_tmp135C,6))))));}
Cyc_strcat(msg,(struct _dynforward_ptr)Cyc_yytname[_check_known_subscript_notnull(
275,x)]);{const char*_tmp135E;Cyc_strcat(msg,((_tmp135E="'",_tag_dynforward(
_tmp135E,sizeof(char),_get_zero_arr_size_char(_tmp135E,2)))));}++ count;}}}Cyc_yyerror((
struct _dynforward_ptr)msg);}else{const char*_tmp135F;Cyc_yyerror(((_tmp135F="parse error",
_tag_dynforward(_tmp135F,sizeof(char),_get_zero_arr_size_char(_tmp135F,12)))));}}
goto yyerrlab1;yyerrlab1: if(yyerrstatus == 3){if(Cyc_yychar == 0){int _tmp8C4=1;
_npop_handler(0);return _tmp8C4;}Cyc_yychar=- 2;}yyerrstatus=3;goto yyerrhandle;
yyerrdefault: yyerrpop: if(yyssp_offset == 0){int _tmp8C5=1;_npop_handler(0);return
_tmp8C5;}-- yyvsp_offset;yystate=(int)yyss[_check_known_subscript_notnull(10000,
-- yyssp_offset)];-- yylsp_offset;yyerrhandle: yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(1007,yystate)];if(yyn == - 32768)goto yyerrdefault;
yyn +=1;if((yyn < 0  || yyn > 6421) || Cyc_yycheck[_check_known_subscript_notnull(
6422,yyn)]!= 1)goto yyerrdefault;yyn=(int)Cyc_yytable[
_check_known_subscript_notnull(6422,yyn)];if(yyn < 0){if(yyn == - 32768)goto
yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(yyn == 1006){int
_tmp8C6=0;_npop_handler(0);return _tmp8C6;}yyvs[_check_known_subscript_notnull(
10000,++ yyvsp_offset)]=Cyc_yylval;yyls[_check_known_subscript_notnull(10000,++
yylsp_offset)]=Cyc_yylloc;goto yynewstate;};_pop_region(yyregion);}void Cyc_yyprint(
int i,union Cyc_YYSTYPE_union v);void Cyc_yyprint(int i,union Cyc_YYSTYPE_union v){
union Cyc_YYSTYPE_union _tmp8D0=v;struct _tuple11 _tmp8D1;int _tmp8D2;char _tmp8D3;
struct _dynforward_ptr _tmp8D4;struct _tuple1*_tmp8D5;struct _tuple1 _tmp8D6;union Cyc_Absyn_Nmspace_union
_tmp8D7;struct _dynforward_ptr*_tmp8D8;_LL409: if((_tmp8D0.Int_tok).tag != 0)goto
_LL40B;_tmp8D1=(_tmp8D0.Int_tok).f1;_tmp8D2=_tmp8D1.f2;_LL40A:{const char*
_tmp1363;void*_tmp1362[1];struct Cyc_Int_pa_struct _tmp1361;(_tmp1361.tag=1,((
_tmp1361.f1=(unsigned long)_tmp8D2,((_tmp1362[0]=& _tmp1361,Cyc_fprintf(Cyc_stderr,((
_tmp1363="%d",_tag_dynforward(_tmp1363,sizeof(char),_get_zero_arr_size_char(
_tmp1363,3)))),_tag_dynforward(_tmp1362,sizeof(void*),1)))))));}goto _LL408;
_LL40B: if((_tmp8D0.Char_tok).tag != 1)goto _LL40D;_tmp8D3=(_tmp8D0.Char_tok).f1;
_LL40C:{const char*_tmp1367;void*_tmp1366[1];struct Cyc_Int_pa_struct _tmp1365;(
_tmp1365.tag=1,((_tmp1365.f1=(unsigned long)((int)_tmp8D3),((_tmp1366[0]=&
_tmp1365,Cyc_fprintf(Cyc_stderr,((_tmp1367="%c",_tag_dynforward(_tmp1367,sizeof(
char),_get_zero_arr_size_char(_tmp1367,3)))),_tag_dynforward(_tmp1366,sizeof(
void*),1)))))));}goto _LL408;_LL40D: if((_tmp8D0.String_tok).tag != 2)goto _LL40F;
_tmp8D4=(_tmp8D0.String_tok).f1;_LL40E:{const char*_tmp136B;void*_tmp136A[1];
struct Cyc_String_pa_struct _tmp1369;(_tmp1369.tag=0,((_tmp1369.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp8D4),((_tmp136A[0]=& _tmp1369,Cyc_fprintf(
Cyc_stderr,((_tmp136B="\"%s\"",_tag_dynforward(_tmp136B,sizeof(char),
_get_zero_arr_size_char(_tmp136B,5)))),_tag_dynforward(_tmp136A,sizeof(void*),1)))))));}
goto _LL408;_LL40F: if((_tmp8D0.QualId_tok).tag != 4)goto _LL411;_tmp8D5=(_tmp8D0.QualId_tok).f1;
_tmp8D6=*_tmp8D5;_tmp8D7=_tmp8D6.f1;_tmp8D8=_tmp8D6.f2;_LL410: {struct Cyc_List_List*
_tmp8E2=0;{union Cyc_Absyn_Nmspace_union _tmp8E3=_tmp8D7;struct Cyc_List_List*
_tmp8E4;struct Cyc_List_List*_tmp8E5;_LL414: if((_tmp8E3.Rel_n).tag != 1)goto _LL416;
_tmp8E4=(_tmp8E3.Rel_n).f1;_LL415: _tmp8E2=_tmp8E4;goto _LL413;_LL416: if((_tmp8E3.Abs_n).tag
!= 2)goto _LL418;_tmp8E5=(_tmp8E3.Abs_n).f1;_LL417: _tmp8E2=_tmp8E5;goto _LL413;
_LL418: if((_tmp8E3.Loc_n).tag != 0)goto _LL413;_LL419: goto _LL413;_LL413:;}for(0;
_tmp8E2 != 0;_tmp8E2=_tmp8E2->tl){const char*_tmp136F;void*_tmp136E[1];struct Cyc_String_pa_struct
_tmp136D;(_tmp136D.tag=0,((_tmp136D.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)*((struct _dynforward_ptr*)_tmp8E2->hd)),((_tmp136E[0]=& _tmp136D,
Cyc_fprintf(Cyc_stderr,((_tmp136F="%s::",_tag_dynforward(_tmp136F,sizeof(char),
_get_zero_arr_size_char(_tmp136F,5)))),_tag_dynforward(_tmp136E,sizeof(void*),1)))))));}{
const char*_tmp1373;void*_tmp1372[1];struct Cyc_String_pa_struct _tmp1371;(_tmp1371.tag=
0,((_tmp1371.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp8D8),((
_tmp1372[0]=& _tmp1371,Cyc_fprintf(Cyc_stderr,((_tmp1373="%s::",_tag_dynforward(
_tmp1373,sizeof(char),_get_zero_arr_size_char(_tmp1373,5)))),_tag_dynforward(
_tmp1372,sizeof(void*),1)))))));}goto _LL408;}_LL411:;_LL412:{const char*_tmp1376;
void*_tmp1375;(_tmp1375=0,Cyc_fprintf(Cyc_stderr,((_tmp1376="?",_tag_dynforward(
_tmp1376,sizeof(char),_get_zero_arr_size_char(_tmp1376,2)))),_tag_dynforward(
_tmp1375,sizeof(void*),0)));}goto _LL408;_LL408:;}struct Cyc_List_List*Cyc_Parse_parse_file(
struct Cyc___cycFILE*f);struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*
f){Cyc_Parse_parse_result=0;Cyc_yyparse(Cyc_Lexing_from_file(f));return Cyc_Parse_parse_result;}
