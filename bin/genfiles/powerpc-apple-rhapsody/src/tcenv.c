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
16];struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct
Cyc___cycFILE*,struct _dynforward_ptr,struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dynforward_ptr f1;}
;struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[8];struct Cyc_Iter_Iter{void*env;int(*next)(void*env,
void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct
Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};
extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict
Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));int Cyc_Dict_is_empty(
struct Cyc_Dict_Dict d);int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);struct Cyc_Dict_Dict
Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);void*Cyc_Dict_lookup(struct
Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);
struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct
_RegionHandle*,struct Cyc_Dict_Dict,void*);struct Cyc_Set_Set;struct Cyc_Set_Set*
Cyc_Set_empty(int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rempty(struct
_RegionHandle*r,int(*cmp)(void*,void*));int Cyc_Set_member(struct Cyc_Set_Set*s,
void*elt);extern char Cyc_Set_Absent[11];struct Cyc_SlowDict_Dict;extern char Cyc_SlowDict_Present[
12];extern char Cyc_SlowDict_Absent[11];struct Cyc_Lineno_Pos{struct _dynforward_ptr
logical_file;struct _dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _dynforward_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dynforward_ptr desc;};
extern char Cyc_Position_Nocontext[14];int Cyc_strptrcmp(struct _dynforward_ptr*s1,
struct _dynforward_ptr*s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_varlist_cmp(
struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;
extern void*Cyc_Absyn_empty_effect;extern struct _tuple1*Cyc_Absyn_exn_name;extern
struct Cyc_Absyn_Tuniondecl*Cyc_Absyn_exn_tud;void*Cyc_Absyn_dynforward_typ(void*
t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dynforward_ptr
Cyc_Absynpp_typ2string(void*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
le;};void*Cyc_Tcenv_env_err(struct _dynforward_ptr msg);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(
struct _RegionHandle*);struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct
_RegionHandle*);struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*,
struct Cyc_Position_Segment*,struct Cyc_Absyn_Fndecl*);struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*
new_fn);struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _dynforward_ptr*,struct Cyc_List_List*);void*
Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Tuniondecl***
Cyc_Tcenv_lookup_xtuniondecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct
Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Typedefdecl*
Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_local_var(struct _RegionHandle*,struct Cyc_Position_Segment*,struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*
Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*continue_dest);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct
Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,void*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_get_try_depth(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_in_notreadctxt(struct
Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);void Cyc_Tcenv_process_goto(struct
Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct _dynforward_ptr*,struct Cyc_Absyn_Stmt**);
struct _tuple4{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;struct Cyc_Tcenv_CList*
f3;};struct _tuple4*Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,
struct Cyc_Absyn_Switch_clause***);struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,struct _dynforward_ptr*,struct Cyc_Absyn_Stmt*);
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*);struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*,struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,struct Cyc_Position_Segment*
loc);void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int resetable,int opened);void
Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*rgn);void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
void*rgn);void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,void*eff);int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,
void*r1,void*r2);void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);void Cyc_Tcenv_check_delayed_effects(
struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*
te);void*Cyc_Tcutil_impos(struct _dynforward_ptr fmt,struct _dynforward_ptr ap);void
Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;void*Cyc_Tcutil_tvar_kind(
struct Cyc_Absyn_Tvar*t);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_subset_effect(
int may_constrain_evars,void*e1,void*e2);int Cyc_Tcutil_region_in_effect(int
constrain,void*r,void*e);void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*,
struct Cyc_List_List*);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);int Cyc_Tcutil_new_tvar_id();
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_add_tvar_identities(
struct Cyc_List_List*);char Cyc_Tcenv_Env_error[14]="\000\000\000\000Env_error\000";
void*Cyc_Tcenv_env_err(struct _dynforward_ptr msg);void*Cyc_Tcenv_env_err(struct
_dynforward_ptr msg){{const char*_tmp2C3;void*_tmp2C2[1];struct Cyc_String_pa_struct
_tmp2C1;(_tmp2C1.tag=0,((_tmp2C1.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)msg),((_tmp2C2[0]=& _tmp2C1,Cyc_fprintf(Cyc_stderr,((_tmp2C3="Internal error in tcenv: %s\n",
_tag_dynforward(_tmp2C3,sizeof(char),_get_zero_arr_size_char(_tmp2C3,29)))),
_tag_dynforward(_tmp2C2,sizeof(void*),1)))))));}Cyc_fflush((struct Cyc___cycFILE*)
Cyc_stderr);(int)_throw((void*)Cyc_Tcenv_Env_error);}struct Cyc_Tcenv_Tenv;struct
Cyc_Tcenv_Genv;struct Cyc_Tcenv_CtrlEnv{struct _RegionHandle*ctrl_rgn;void*
continue_stmt;void*break_stmt;struct _tuple4*fallthru_clause;void*next_stmt;int
try_depth;};struct Cyc_Tcenv_SharedFenv{struct _RegionHandle*frgn;void*return_typ;
struct Cyc_Dict_Dict seen_labels;struct Cyc_Dict_Dict needed_labels;struct Cyc_List_List*
delayed_effect_checks;struct Cyc_List_List*delayed_constraint_checks;};struct Cyc_Tcenv_Bindings{
struct _dynforward_ptr*v;void*b;struct Cyc_Tcenv_Bindings*tl;};struct Cyc_Tcenv_Fenv{
struct Cyc_Tcenv_SharedFenv*shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*
region_order;struct Cyc_Tcenv_Bindings*locals;struct Cyc_Absyn_Stmt*encloser;
struct Cyc_Tcenv_CtrlEnv*ctrl_env;void*capability;void*curr_rgn;int in_notreadctxt;
};char Cyc_Tcenv_NoBinding[14]="\000\000\000\000NoBinding\000";void*Cyc_Tcenv_lookup_binding(
struct Cyc_Tcenv_Bindings*bs,struct _dynforward_ptr*v);void*Cyc_Tcenv_lookup_binding(
struct Cyc_Tcenv_Bindings*bs,struct _dynforward_ptr*v){for(0;(unsigned int)bs;bs=
bs->tl){if(Cyc_strptrcmp(v,bs->v)== 0)return(void*)bs->b;}(int)_throw((void*)Cyc_Tcenv_NoBinding);}
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*r);struct Cyc_Tcenv_Genv*
Cyc_Tcenv_empty_genv(struct _RegionHandle*r){struct Cyc_Tcenv_Genv*_tmp2C4;return(
_tmp2C4=_region_malloc(r,sizeof(*_tmp2C4)),((_tmp2C4->grgn=r,((_tmp2C4->namespaces=((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dynforward_ptr*,
struct _dynforward_ptr*)))Cyc_Set_rempty)(r,Cyc_strptrcmp),((_tmp2C4->aggrdecls=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dynforward_ptr*,
struct _dynforward_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp2C4->tuniondecls=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dynforward_ptr*,
struct _dynforward_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp2C4->enumdecls=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dynforward_ptr*,
struct _dynforward_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp2C4->typedefs=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dynforward_ptr*,
struct _dynforward_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp2C4->ordinaries=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dynforward_ptr*,
struct _dynforward_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp2C4->availables=
0,_tmp2C4)))))))))))))))));}struct _tuple5{void*f1;int f2;};struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_tc_init(struct _RegionHandle*r);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(
struct _RegionHandle*r){{struct Cyc_Core_Opt*_tmp2C5;Cyc_Tcutil_empty_var_set=((
_tmp2C5=_cycalloc(sizeof(*_tmp2C5)),((_tmp2C5->v=((struct Cyc_Set_Set*(*)(int(*
cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),
_tmp2C5))));}{struct Cyc_Tcenv_Genv*_tmp5=Cyc_Tcenv_empty_genv(r);{struct Cyc_Absyn_Tuniondecl**
_tmp2C6;_tmp5->tuniondecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dynforward_ptr*k,struct Cyc_Absyn_Tuniondecl**v))Cyc_Dict_insert)(_tmp5->tuniondecls,(*
Cyc_Absyn_exn_name).f2,((_tmp2C6=_cycalloc(sizeof(*_tmp2C6)),((_tmp2C6[0]=Cyc_Absyn_exn_tud,
_tmp2C6)))));}{struct Cyc_List_List*_tmp7=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmp7 != 0;_tmp7=_tmp7->tl){
struct Cyc_Tcenv_TunionRes_struct*_tmp2CC;struct Cyc_Tcenv_TunionRes_struct _tmp2CB;
struct _tuple5*_tmp2CA;_tmp5->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct _dynforward_ptr*k,struct _tuple5*v))Cyc_Dict_insert)(_tmp5->ordinaries,(*((
struct Cyc_Absyn_Tunionfield*)_tmp7->hd)->name).f2,((_tmp2CA=_region_malloc(r,
sizeof(*_tmp2CA)),((_tmp2CA->f1=(void*)((_tmp2CC=_cycalloc(sizeof(*_tmp2CC)),((
_tmp2CC[0]=((_tmp2CB.tag=2,((_tmp2CB.f1=Cyc_Absyn_exn_tud,((_tmp2CB.f2=(struct
Cyc_Absyn_Tunionfield*)_tmp7->hd,_tmp2CB)))))),_tmp2CC)))),((_tmp2CA->f2=1,
_tmp2CA)))))));}}{struct Cyc_Dict_Dict ae=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_varlist_cmp),0,_tmp5);struct Cyc_Tcenv_Tenv*_tmp2CD;return(_tmp2CD=
_region_malloc(r,sizeof(*_tmp2CD)),((_tmp2CD->ns=0,((_tmp2CD->ae=ae,((_tmp2CD->le=
0,_tmp2CD)))))));}}}static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _dynforward_ptr*n,struct
Cyc_List_List*ns);static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _dynforward_ptr*n,struct
Cyc_List_List*ns){return((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct
Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,Cyc_Tcenv_resolve_namespace(te,loc,n,ns));}
static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*ns);
static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*ns){if(
ns == 0){const char*_tmp2CE;return((struct Cyc_List_List*(*)(struct _dynforward_ptr
msg))Cyc_Tcenv_env_err)(((_tmp2CE="outer_namespace",_tag_dynforward(_tmp2CE,
sizeof(char),_get_zero_arr_size_char(_tmp2CE,16)))));}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);}static int Cyc_Tcenv_same_namespace(
struct Cyc_List_List*n1,struct Cyc_List_List*n2);static int Cyc_Tcenv_same_namespace(
struct Cyc_List_List*n1,struct Cyc_List_List*n2){if(n1 == n2)return 1;for(0;n1 != 0;
n1=n1->tl){if(n2 == 0)return 0;if(Cyc_strptrcmp((struct _dynforward_ptr*)n1->hd,(
struct _dynforward_ptr*)n2->hd)!= 0)return 0;n2=n2->tl;}if(n2 != 0)return 0;return 1;}
static void Cyc_Tcenv_check_repeat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dynforward_ptr*),struct
_dynforward_ptr*v,struct Cyc_List_List*cns,struct Cyc_List_List*nss);static void Cyc_Tcenv_check_repeat(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,
struct _dynforward_ptr*),struct _dynforward_ptr*v,struct Cyc_List_List*cns,struct
Cyc_List_List*nss){for(0;nss != 0;nss=nss->tl){if(!Cyc_Tcenv_same_namespace(cns,(
struct Cyc_List_List*)nss->hd)){struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)
nss->hd);struct _handler_cons _tmpD;_push_handler(& _tmpD);{int _tmpF=0;if(setjmp(
_tmpD.handler))_tmpF=1;if(!_tmpF){lookup(ge2,v);{const char*_tmp2D2;void*_tmp2D1[
1];struct Cyc_String_pa_struct _tmp2D0;(_tmp2D0.tag=0,((_tmp2D0.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*v),((_tmp2D1[0]=& _tmp2D0,Cyc_Tcutil_terr(
loc,((_tmp2D2="%s is ambiguous",_tag_dynforward(_tmp2D2,sizeof(char),
_get_zero_arr_size_char(_tmp2D2,16)))),_tag_dynforward(_tmp2D1,sizeof(void*),1)))))));};
_pop_handler();}else{void*_tmpE=(void*)_exn_thrown;void*_tmp14=_tmpE;_LL1: if(
_tmp14 != Cyc_Dict_Absent)goto _LL3;_LL2: goto _LL0;_LL3:;_LL4:(void)_throw(_tmp14);
_LL0:;}}}}return;}static void*Cyc_Tcenv_scoped_lookup(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct
_dynforward_ptr*),struct _dynforward_ptr*v);static void*Cyc_Tcenv_scoped_lookup(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,
struct _dynforward_ptr*),struct _dynforward_ptr*v){struct Cyc_List_List*cns=te->ns;
while(1){struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,cns);{struct _handler_cons _tmp15;
_push_handler(& _tmp15);{int _tmp17=0;if(setjmp(_tmp15.handler))_tmp17=1;if(!
_tmp17){{void*result=lookup(ge,v);Cyc_Tcenv_check_repeat(te,loc,lookup,v,cns,ge->availables);{
void*_tmp18=result;_npop_handler(0);return _tmp18;}};_pop_handler();}else{void*
_tmp16=(void*)_exn_thrown;void*_tmp1A=_tmp16;_LL6: if(_tmp1A != Cyc_Dict_Absent)
goto _LL8;_LL7: goto _LL5;_LL8:;_LL9:(void)_throw(_tmp1A);_LL5:;}}}{struct Cyc_List_List*
_tmp1B=ge->availables;for(0;_tmp1B != 0;_tmp1B=_tmp1B->tl){struct Cyc_Tcenv_Genv*
ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,(struct Cyc_List_List*)_tmp1B->hd);struct _handler_cons _tmp1C;_push_handler(&
_tmp1C);{int _tmp1E=0;if(setjmp(_tmp1C.handler))_tmp1E=1;if(!_tmp1E){{void*result=
lookup(ge2,v);Cyc_Tcenv_check_repeat(te,loc,lookup,v,(struct Cyc_List_List*)
_tmp1B->hd,_tmp1B->tl);{void*_tmp1F=result;_npop_handler(0);return _tmp1F;}};
_pop_handler();}else{void*_tmp1D=(void*)_exn_thrown;void*_tmp21=_tmp1D;_LLB: if(
_tmp21 != Cyc_Dict_Absent)goto _LLD;_LLC: goto _LLA;_LLD:;_LLE:(void)_throw(_tmp21);
_LLA:;}}}}if(cns == 0)(int)_throw((void*)Cyc_Dict_Absent);cns=Cyc_Tcenv_outer_namespace(
cns);}}static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*ge,
struct _dynforward_ptr*v);static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*
ge,struct _dynforward_ptr*v){struct Cyc_Dict_Dict _tmp22=ge->ordinaries;struct
_tuple5*ans=((struct _tuple5*(*)(struct Cyc_Dict_Dict d,struct _dynforward_ptr*k))
Cyc_Dict_lookup)(_tmp22,v);(*ans).f2=1;return(*ans).f1;}static void*Cyc_Tcenv_lookup_ordinary_global(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q);static
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple1*q){struct _tuple1 _tmp24;union Cyc_Absyn_Nmspace_union _tmp25;
struct _dynforward_ptr*_tmp26;struct _tuple1*_tmp23=q;_tmp24=*_tmp23;_tmp25=_tmp24.f1;
_tmp26=_tmp24.f2;{union Cyc_Absyn_Nmspace_union _tmp27=_tmp25;struct Cyc_List_List*
_tmp28;struct Cyc_List_List*_tmp29;struct Cyc_List_List _tmp2A;struct
_dynforward_ptr*_tmp2B;struct Cyc_List_List*_tmp2C;struct Cyc_List_List*_tmp2D;
_LL10: if((_tmp27.Loc_n).tag != 0)goto _LL12;_LL11: goto _LL13;_LL12: if((_tmp27.Rel_n).tag
!= 1)goto _LL14;_tmp28=(_tmp27.Rel_n).f1;if(_tmp28 != 0)goto _LL14;_LL13: return Cyc_Tcenv_scoped_lookup(
te,loc,Cyc_Tcenv_lookup_ordinary_global_f,_tmp26);_LL14: if((_tmp27.Rel_n).tag != 
1)goto _LL16;_tmp29=(_tmp27.Rel_n).f1;if(_tmp29 == 0)goto _LL16;_tmp2A=*_tmp29;
_tmp2B=(struct _dynforward_ptr*)_tmp2A.hd;_tmp2C=_tmp2A.tl;_LL15: {struct Cyc_Tcenv_Genv*
_tmp2E=Cyc_Tcenv_lookup_namespace(te,loc,_tmp2B,_tmp2C);return Cyc_Tcenv_lookup_ordinary_global_f(
_tmp2E,_tmp26);}_LL16: if((_tmp27.Abs_n).tag != 2)goto _LLF;_tmp2D=(_tmp27.Abs_n).f1;
_LL17: return Cyc_Tcenv_lookup_ordinary_global_f(((struct Cyc_Tcenv_Genv*(*)(struct
Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp2D),_tmp26);
_LLF:;}}struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _dynforward_ptr*n,struct Cyc_List_List*ns);
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,struct _dynforward_ptr*n,struct Cyc_List_List*ns){struct
Cyc_List_List*_tmp2F=te->ns;struct _RegionHandle _tmp30=_new_region("temp");struct
_RegionHandle*temp=& _tmp30;_push_region(temp);{struct Cyc_List_List*_tmp31=0;
while(1){struct Cyc_Tcenv_Genv*_tmp32=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict
d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp2F);struct Cyc_List_List*
_tmp33=_tmp32->availables;struct Cyc_Set_Set*_tmp34=_tmp32->namespaces;{struct Cyc_List_List*
_tmp35=_tmp33;for(0;_tmp35 != 0;_tmp35=_tmp35->tl){struct Cyc_Set_Set*_tmp36=(((
struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(
te->ae,(struct Cyc_List_List*)_tmp35->hd))->namespaces;if(((int(*)(struct Cyc_Set_Set*
s,struct _dynforward_ptr*elt))Cyc_Set_member)(_tmp36,n)){struct Cyc_List_List*
_tmp2D5;struct Cyc_List_List*_tmp2D4;_tmp31=((_tmp2D4=_region_malloc(temp,sizeof(*
_tmp2D4)),((_tmp2D4->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(_tmp2F,((_tmp2D5=_cycalloc(sizeof(*_tmp2D5)),((
_tmp2D5->hd=n,((_tmp2D5->tl=ns,_tmp2D5))))))),((_tmp2D4->tl=_tmp31,_tmp2D4))))));}}}
if(((int(*)(struct Cyc_Set_Set*s,struct _dynforward_ptr*elt))Cyc_Set_member)(
_tmp34,n)){struct Cyc_List_List*_tmp2D8;struct Cyc_List_List*_tmp2D7;_tmp31=((
_tmp2D7=_region_malloc(temp,sizeof(*_tmp2D7)),((_tmp2D7->hd=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2F,((_tmp2D8=
_cycalloc(sizeof(*_tmp2D8)),((_tmp2D8->hd=n,((_tmp2D8->tl=ns,_tmp2D8))))))),((
_tmp2D7->tl=_tmp31,_tmp2D7))))));}if(_tmp31 != 0){if(_tmp31->tl != 0){const char*
_tmp2DC;void*_tmp2DB[1];struct Cyc_String_pa_struct _tmp2DA;(_tmp2DA.tag=0,((
_tmp2DA.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*n),((_tmp2DB[0]=&
_tmp2DA,Cyc_Tcutil_terr(loc,((_tmp2DC="%s is ambiguous",_tag_dynforward(_tmp2DC,
sizeof(char),_get_zero_arr_size_char(_tmp2DC,16)))),_tag_dynforward(_tmp2DB,
sizeof(void*),1)))))));}{struct Cyc_List_List*_tmp3E=(struct Cyc_List_List*)_tmp31->hd;
_npop_handler(0);return _tmp3E;}}if(_tmp2F == 0)(int)_throw((void*)Cyc_Dict_Absent);
_tmp2F=Cyc_Tcenv_outer_namespace(_tmp2F);}};_pop_region(temp);}static struct Cyc_Absyn_Aggrdecl**
Cyc_Tcenv_lookup_aggrdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dynforward_ptr*v);
static struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl_f(struct Cyc_Tcenv_Genv*
ge,struct _dynforward_ptr*v){struct Cyc_Dict_Dict _tmp3F=ge->aggrdecls;return((
struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(
_tmp3F,v);}struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct _tuple1*q);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct
_tuple1 _tmp41;union Cyc_Absyn_Nmspace_union _tmp42;struct _dynforward_ptr*_tmp43;
struct _tuple1*_tmp40=q;_tmp41=*_tmp40;_tmp42=_tmp41.f1;_tmp43=_tmp41.f2;{union
Cyc_Absyn_Nmspace_union _tmp44=_tmp42;struct Cyc_List_List*_tmp45;struct Cyc_List_List*
_tmp46;struct Cyc_List_List*_tmp47;struct Cyc_List_List _tmp48;struct
_dynforward_ptr*_tmp49;struct Cyc_List_List*_tmp4A;_LL19: if((_tmp44.Loc_n).tag != 
0)goto _LL1B;_LL1A: goto _LL1C;_LL1B: if((_tmp44.Rel_n).tag != 1)goto _LL1D;_tmp45=(
_tmp44.Rel_n).f1;if(_tmp45 != 0)goto _LL1D;_LL1C: return((struct Cyc_Absyn_Aggrdecl**(*)(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Aggrdecl**(*
lookup)(struct Cyc_Tcenv_Genv*,struct _dynforward_ptr*),struct _dynforward_ptr*v))
Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_aggrdecl_f,_tmp43);_LL1D: if((
_tmp44.Abs_n).tag != 2)goto _LL1F;_tmp46=(_tmp44.Abs_n).f1;_LL1E: {struct Cyc_Dict_Dict
_tmp4B=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,_tmp46))->aggrdecls;return((struct Cyc_Absyn_Aggrdecl**(*)(
struct Cyc_Dict_Dict d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp4B,_tmp43);}
_LL1F: if((_tmp44.Rel_n).tag != 1)goto _LL18;_tmp47=(_tmp44.Rel_n).f1;if(_tmp47 == 0)
goto _LL18;_tmp48=*_tmp47;_tmp49=(struct _dynforward_ptr*)_tmp48.hd;_tmp4A=_tmp48.tl;
_LL20: {struct Cyc_Dict_Dict _tmp4C=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp49,
_tmp4A))->aggrdecls;return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,
struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp4C,_tmp43);}_LL18:;}}static struct
Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl_f(struct Cyc_Tcenv_Genv*ge,
struct _dynforward_ptr*v);static struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dynforward_ptr*v){struct Cyc_Dict_Dict _tmp4D=ge->tuniondecls;
return((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict d,struct
_dynforward_ptr*k))Cyc_Dict_lookup)(_tmp4D,v);}struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q);struct
Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_tuniondecl(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Position_Segment*loc,struct _tuple1*q){struct _tuple1 _tmp4F;union Cyc_Absyn_Nmspace_union
_tmp50;struct _dynforward_ptr*_tmp51;struct _tuple1*_tmp4E=q;_tmp4F=*_tmp4E;_tmp50=
_tmp4F.f1;_tmp51=_tmp4F.f2;{union Cyc_Absyn_Nmspace_union _tmp52=_tmp50;struct Cyc_List_List*
_tmp53;struct Cyc_List_List*_tmp54;struct Cyc_List_List _tmp55;struct
_dynforward_ptr*_tmp56;struct Cyc_List_List*_tmp57;struct Cyc_List_List*_tmp58;
_LL22: if((_tmp52.Loc_n).tag != 0)goto _LL24;_LL23: goto _LL25;_LL24: if((_tmp52.Rel_n).tag
!= 1)goto _LL26;_tmp53=(_tmp52.Rel_n).f1;if(_tmp53 != 0)goto _LL26;_LL25: return((
struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Tuniondecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct
_dynforward_ptr*),struct _dynforward_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_tuniondecl_f,
_tmp51);_LL26: if((_tmp52.Rel_n).tag != 1)goto _LL28;_tmp54=(_tmp52.Rel_n).f1;if(
_tmp54 == 0)goto _LL28;_tmp55=*_tmp54;_tmp56=(struct _dynforward_ptr*)_tmp55.hd;
_tmp57=_tmp55.tl;_LL27: {struct Cyc_Dict_Dict _tmp59=(Cyc_Tcenv_lookup_namespace(
te,loc,_tmp56,_tmp57))->tuniondecls;return((struct Cyc_Absyn_Tuniondecl**(*)(
struct Cyc_Dict_Dict d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp59,_tmp51);}
_LL28: if((_tmp52.Abs_n).tag != 2)goto _LL21;_tmp58=(_tmp52.Abs_n).f1;_LL29: {
struct Cyc_Dict_Dict _tmp5A=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp58))->tuniondecls;return((
struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict d,struct _dynforward_ptr*k))
Cyc_Dict_lookup)(_tmp5A,_tmp51);}_LL21:;}}static struct Cyc_Absyn_Tuniondecl**Cyc_Tcenv_lookup_xtuniondecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dynforward_ptr*v);static struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_xtuniondecl_f(struct Cyc_Tcenv_Genv*ge,struct _dynforward_ptr*v){
return((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict d,struct
_dynforward_ptr*k))Cyc_Dict_lookup)(ge->tuniondecls,v);}struct Cyc_Absyn_Tuniondecl***
Cyc_Tcenv_lookup_xtuniondecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple1*q);struct Cyc_Absyn_Tuniondecl***Cyc_Tcenv_lookup_xtuniondecl(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
struct _tuple1*q){struct _tuple1 _tmp5C;union Cyc_Absyn_Nmspace_union _tmp5D;struct
_dynforward_ptr*_tmp5E;struct _tuple1*_tmp5B=q;_tmp5C=*_tmp5B;_tmp5D=_tmp5C.f1;
_tmp5E=_tmp5C.f2;{union Cyc_Absyn_Nmspace_union _tmp5F=_tmp5D;struct Cyc_List_List*
_tmp60;struct Cyc_List_List*_tmp61;struct Cyc_List_List _tmp62;struct
_dynforward_ptr*_tmp63;struct Cyc_List_List*_tmp64;struct Cyc_List_List*_tmp65;
_LL2B: if((_tmp5F.Rel_n).tag != 1)goto _LL2D;_tmp60=(_tmp5F.Rel_n).f1;if(_tmp60 != 0)
goto _LL2D;_LL2C: {struct _handler_cons _tmp66;_push_handler(& _tmp66);{int _tmp68=0;
if(setjmp(_tmp66.handler))_tmp68=1;if(!_tmp68){{struct Cyc_Absyn_Tuniondecl***
_tmp2DD;struct Cyc_Absyn_Tuniondecl***_tmp6A=(_tmp2DD=_region_malloc(r,sizeof(*
_tmp2DD)),((_tmp2DD[0]=((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Tuniondecl**(*lookup)(struct Cyc_Tcenv_Genv*,
struct _dynforward_ptr*),struct _dynforward_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,
Cyc_Tcenv_lookup_xtuniondecl_f,_tmp5E),_tmp2DD)));_npop_handler(0);return _tmp6A;};
_pop_handler();}else{void*_tmp67=(void*)_exn_thrown;void*_tmp6C=_tmp67;_LL34: if(
_tmp6C != Cyc_Dict_Absent)goto _LL36;_LL35: return 0;_LL36:;_LL37:(void)_throw(
_tmp6C);_LL33:;}}}_LL2D: if((_tmp5F.Loc_n).tag != 0)goto _LL2F;_LL2E:{const char*
_tmp2E0;void*_tmp2DF;(_tmp2DF=0,Cyc_Tcutil_terr(loc,((_tmp2E0="lookup_xtuniondecl: impossible",
_tag_dynforward(_tmp2E0,sizeof(char),_get_zero_arr_size_char(_tmp2E0,31)))),
_tag_dynforward(_tmp2DF,sizeof(void*),0)));}return 0;_LL2F: if((_tmp5F.Rel_n).tag
!= 1)goto _LL31;_tmp61=(_tmp5F.Rel_n).f1;if(_tmp61 == 0)goto _LL31;_tmp62=*_tmp61;
_tmp63=(struct _dynforward_ptr*)_tmp62.hd;_tmp64=_tmp62.tl;_LL30: {struct Cyc_Tcenv_Genv*
ge;{struct _handler_cons _tmp6F;_push_handler(& _tmp6F);{int _tmp71=0;if(setjmp(
_tmp6F.handler))_tmp71=1;if(!_tmp71){ge=Cyc_Tcenv_lookup_namespace(te,loc,_tmp63,
_tmp64);;_pop_handler();}else{void*_tmp70=(void*)_exn_thrown;void*_tmp73=_tmp70;
_LL39: if(_tmp73 != Cyc_Dict_Absent)goto _LL3B;_LL3A:{const char*_tmp2E3;void*
_tmp2E2;(_tmp2E2=0,Cyc_Tcutil_terr(loc,((_tmp2E3="bad qualified name for xtunion",
_tag_dynforward(_tmp2E3,sizeof(char),_get_zero_arr_size_char(_tmp2E3,31)))),
_tag_dynforward(_tmp2E2,sizeof(void*),0)));}(int)_throw((void*)Cyc_Dict_Absent);
_LL3B:;_LL3C:(void)_throw(_tmp73);_LL38:;}}}{struct Cyc_Dict_Dict _tmp76=ge->tuniondecls;
struct Cyc_Absyn_Tuniondecl***_tmp2E4;return(_tmp2E4=_region_malloc(r,sizeof(*
_tmp2E4)),((_tmp2E4[0]=((struct Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict d,
struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp76,_tmp5E),_tmp2E4)));}}_LL31: if((
_tmp5F.Abs_n).tag != 2)goto _LL2A;_tmp65=(_tmp5F.Abs_n).f1;_LL32: {struct Cyc_Dict_Dict
_tmp78=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,_tmp65))->tuniondecls;struct Cyc_Absyn_Tuniondecl***
_tmp2E5;return(_tmp2E5=_region_malloc(r,sizeof(*_tmp2E5)),((_tmp2E5[0]=((struct
Cyc_Absyn_Tuniondecl**(*)(struct Cyc_Dict_Dict d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(
_tmp78,_tmp5E),_tmp2E5)));}_LL2A:;}}static struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dynforward_ptr*v);static struct Cyc_Absyn_Enumdecl**
Cyc_Tcenv_lookup_enumdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dynforward_ptr*v){
struct Cyc_Dict_Dict _tmp7A=ge->enumdecls;return((struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp7A,v);}struct
Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct _tuple1*q);struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct _tuple1
_tmp7C;union Cyc_Absyn_Nmspace_union _tmp7D;struct _dynforward_ptr*_tmp7E;struct
_tuple1*_tmp7B=q;_tmp7C=*_tmp7B;_tmp7D=_tmp7C.f1;_tmp7E=_tmp7C.f2;{union Cyc_Absyn_Nmspace_union
_tmp7F=_tmp7D;struct Cyc_List_List*_tmp80;struct Cyc_List_List*_tmp81;struct Cyc_List_List
_tmp82;struct _dynforward_ptr*_tmp83;struct Cyc_List_List*_tmp84;struct Cyc_List_List*
_tmp85;_LL3E: if((_tmp7F.Loc_n).tag != 0)goto _LL40;_LL3F: goto _LL41;_LL40: if((
_tmp7F.Rel_n).tag != 1)goto _LL42;_tmp80=(_tmp7F.Rel_n).f1;if(_tmp80 != 0)goto _LL42;
_LL41: return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Enumdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct
_dynforward_ptr*),struct _dynforward_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_enumdecl_f,
_tmp7E);_LL42: if((_tmp7F.Rel_n).tag != 1)goto _LL44;_tmp81=(_tmp7F.Rel_n).f1;if(
_tmp81 == 0)goto _LL44;_tmp82=*_tmp81;_tmp83=(struct _dynforward_ptr*)_tmp82.hd;
_tmp84=_tmp82.tl;_LL43: {struct Cyc_Dict_Dict _tmp86=(Cyc_Tcenv_lookup_namespace(
te,loc,_tmp83,_tmp84))->enumdecls;return((struct Cyc_Absyn_Enumdecl**(*)(struct
Cyc_Dict_Dict d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp86,_tmp7E);}_LL44:
if((_tmp7F.Abs_n).tag != 2)goto _LL3D;_tmp85=(_tmp7F.Abs_n).f1;_LL45: {struct Cyc_Dict_Dict
_tmp87=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,_tmp85))->enumdecls;return((struct Cyc_Absyn_Enumdecl**(*)(
struct Cyc_Dict_Dict d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp87,_tmp7E);}
_LL3D:;}}static struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(struct
Cyc_Tcenv_Genv*ge,struct _dynforward_ptr*v);static struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(
struct Cyc_Tcenv_Genv*ge,struct _dynforward_ptr*v){struct Cyc_Dict_Dict _tmp88=ge->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct
_dynforward_ptr*k))Cyc_Dict_lookup)(_tmp88,v);}struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct _tuple1*q);struct
Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Position_Segment*loc,struct _tuple1*q){struct _tuple1 _tmp8A;union Cyc_Absyn_Nmspace_union
_tmp8B;struct _dynforward_ptr*_tmp8C;struct _tuple1*_tmp89=q;_tmp8A=*_tmp89;_tmp8B=
_tmp8A.f1;_tmp8C=_tmp8A.f2;{union Cyc_Absyn_Nmspace_union _tmp8D=_tmp8B;struct Cyc_List_List*
_tmp8E;struct Cyc_List_List*_tmp8F;struct Cyc_List_List _tmp90;struct
_dynforward_ptr*_tmp91;struct Cyc_List_List*_tmp92;struct Cyc_List_List*_tmp93;
_LL47: if((_tmp8D.Loc_n).tag != 0)goto _LL49;_LL48: goto _LL4A;_LL49: if((_tmp8D.Rel_n).tag
!= 1)goto _LL4B;_tmp8E=(_tmp8D.Rel_n).f1;if(_tmp8E != 0)goto _LL4B;_LL4A: return((
struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Tcenv_Genv*,struct
_dynforward_ptr*),struct _dynforward_ptr*v))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_typedefdecl_f,
_tmp8C);_LL4B: if((_tmp8D.Rel_n).tag != 1)goto _LL4D;_tmp8F=(_tmp8D.Rel_n).f1;if(
_tmp8F == 0)goto _LL4D;_tmp90=*_tmp8F;_tmp91=(struct _dynforward_ptr*)_tmp90.hd;
_tmp92=_tmp90.tl;_LL4C: {struct Cyc_Dict_Dict _tmp94=(Cyc_Tcenv_lookup_namespace(
te,loc,_tmp91,_tmp92))->typedefs;return((struct Cyc_Absyn_Typedefdecl*(*)(struct
Cyc_Dict_Dict d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp94,_tmp8C);}_LL4D:
if((_tmp8D.Abs_n).tag != 2)goto _LL46;_tmp93=(_tmp8D.Abs_n).f1;_LL4E: {struct Cyc_Dict_Dict
_tmp95=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,_tmp93))->typedefs;return((struct Cyc_Absyn_Typedefdecl*(*)(
struct Cyc_Dict_Dict d,struct _dynforward_ptr*k))Cyc_Dict_lookup)(_tmp95,_tmp8C);}
_LL46:;}}static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te,
struct _dynforward_ptr err_msg);static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(
struct Cyc_Tcenv_Tenv*te,struct _dynforward_ptr err_msg){struct Cyc_Tcenv_Fenv*
_tmp96=te->le;if((struct Cyc_Tcenv_Tenv*)te == 0)((int(*)(struct _dynforward_ptr msg))
Cyc_Tcenv_env_err)(err_msg);return(struct Cyc_Tcenv_Fenv*)_check_null(_tmp96);}
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcenv_Fenv*fe);static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct
_RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){if(te->le == 0){
const char*_tmp2E6;((int(*)(struct _dynforward_ptr msg))Cyc_Tcenv_env_err)(((
_tmp2E6="put_fenv",_tag_dynforward(_tmp2E6,sizeof(char),_get_zero_arr_size_char(
_tmp2E6,9)))));}{struct Cyc_Tcenv_Tenv*_tmp2E7;return(_tmp2E7=_region_malloc(l,
sizeof(*_tmp2E7)),((_tmp2E7->ns=te->ns,((_tmp2E7->ae=te->ae,((_tmp2E7->le=(
struct Cyc_Tcenv_Fenv*)fe,_tmp2E7)))))));}}static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(
struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te);static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(
struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Tenv*_tmp2E8;
return(_tmp2E8=_region_malloc(l,sizeof(*_tmp2E8)),((_tmp2E8->ns=te->ns,((_tmp2E8->ae=
te->ae,((_tmp2E8->le=0,_tmp2E8)))))));}static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(
struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f);static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(
struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){struct Cyc_Tcenv_SharedFenv*_tmp9B;
struct Cyc_List_List*_tmp9C;struct Cyc_RgnOrder_RgnPO*_tmp9D;struct Cyc_Tcenv_Bindings*
_tmp9E;struct Cyc_Absyn_Stmt*_tmp9F;struct Cyc_Tcenv_CtrlEnv*_tmpA0;void*_tmpA1;
void*_tmpA2;int _tmpA3;struct Cyc_Tcenv_Fenv _tmp9A=*f;_tmp9B=_tmp9A.shared;_tmp9C=
_tmp9A.type_vars;_tmp9D=_tmp9A.region_order;_tmp9E=_tmp9A.locals;_tmp9F=_tmp9A.encloser;
_tmpA0=_tmp9A.ctrl_env;_tmpA1=(void*)_tmp9A.capability;_tmpA2=(void*)_tmp9A.curr_rgn;
_tmpA3=_tmp9A.in_notreadctxt;{struct Cyc_Tcenv_Fenv*_tmp2E9;return(_tmp2E9=
_region_malloc(l,sizeof(*_tmp2E9)),((_tmp2E9->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp9B,((_tmp2E9->type_vars=(struct Cyc_List_List*)_tmp9C,((_tmp2E9->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp9D,((_tmp2E9->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp9E),((_tmp2E9->encloser=(struct Cyc_Absyn_Stmt*)
_tmp9F,((_tmp2E9->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmpA0),((_tmp2E9->capability=(void*)((void*)_tmpA1),((_tmp2E9->curr_rgn=(void*)((
void*)_tmpA2),((_tmp2E9->in_notreadctxt=(int)_tmpA3,_tmp2E9)))))))))))))))))));}}
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(struct _RegionHandle*l,
struct Cyc_Tcenv_Fenv*f);static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(
struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){struct Cyc_Tcenv_SharedFenv*_tmpA6;
struct Cyc_List_List*_tmpA7;struct Cyc_RgnOrder_RgnPO*_tmpA8;struct Cyc_Tcenv_Bindings*
_tmpA9;struct Cyc_Absyn_Stmt*_tmpAA;struct Cyc_Tcenv_CtrlEnv*_tmpAB;void*_tmpAC;
void*_tmpAD;int _tmpAE;struct Cyc_Tcenv_Fenv _tmpA5=*f;_tmpA6=_tmpA5.shared;_tmpA7=
_tmpA5.type_vars;_tmpA8=_tmpA5.region_order;_tmpA9=_tmpA5.locals;_tmpAA=_tmpA5.encloser;
_tmpAB=_tmpA5.ctrl_env;_tmpAC=(void*)_tmpA5.capability;_tmpAD=(void*)_tmpA5.curr_rgn;
_tmpAE=_tmpA5.in_notreadctxt;{struct _RegionHandle*_tmpB0;void*_tmpB1;void*_tmpB2;
struct _tuple4*_tmpB3;void*_tmpB4;int _tmpB5;struct Cyc_Tcenv_CtrlEnv _tmpAF=*_tmpAB;
_tmpB0=_tmpAF.ctrl_rgn;_tmpB1=(void*)_tmpAF.continue_stmt;_tmpB2=(void*)_tmpAF.break_stmt;
_tmpB3=_tmpAF.fallthru_clause;_tmpB4=(void*)_tmpAF.next_stmt;_tmpB5=_tmpAF.try_depth;{
struct Cyc_Tcenv_CtrlEnv*_tmp2EA;struct Cyc_Tcenv_CtrlEnv*_tmpB6=(_tmp2EA=
_region_malloc(l,sizeof(*_tmp2EA)),((_tmp2EA->ctrl_rgn=_tmpB0,((_tmp2EA->continue_stmt=(
void*)_tmpB1,((_tmp2EA->break_stmt=(void*)_tmpB2,((_tmp2EA->fallthru_clause=
_tmpB3,((_tmp2EA->next_stmt=(void*)_tmpB4,((_tmp2EA->try_depth=_tmpB5,_tmp2EA)))))))))))));
struct Cyc_Tcenv_Fenv*_tmp2EB;return(_tmp2EB=_region_malloc(l,sizeof(*_tmp2EB)),((
_tmp2EB->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpA6,((_tmp2EB->type_vars=(struct
Cyc_List_List*)_tmpA7,((_tmp2EB->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpA8,((
_tmp2EB->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmpA9),((
_tmp2EB->encloser=(struct Cyc_Absyn_Stmt*)_tmpAA,((_tmp2EB->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)
_tmpB6,((_tmp2EB->capability=(void*)((void*)_tmpAC),((_tmp2EB->curr_rgn=(void*)((
void*)_tmpAD),((_tmp2EB->in_notreadctxt=(int)_tmpAE,_tmp2EB)))))))))))))))))));}}}
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te);void*Cyc_Tcenv_return_typ(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmpBB;struct Cyc_Tcenv_SharedFenv*
_tmpBC;const char*_tmp2EC;struct Cyc_Tcenv_Fenv*_tmpBA=Cyc_Tcenv_get_fenv(te,((
_tmp2EC="return_typ",_tag_dynforward(_tmp2EC,sizeof(char),
_get_zero_arr_size_char(_tmp2EC,11)))));_tmpBB=*_tmpBA;_tmpBC=_tmpBB.shared;{
void*_tmpBE;struct Cyc_Tcenv_SharedFenv _tmpBD=*_tmpBC;_tmpBE=(void*)_tmpBD.return_typ;
return _tmpBE;}}struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*
te);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpBF=te->le;if(te->le == 0)return 0;{struct Cyc_List_List*
_tmpC1;struct Cyc_Tcenv_Fenv _tmpC0=*((struct Cyc_Tcenv_Fenv*)_check_null(_tmpBF));
_tmpC1=_tmpC0.type_vars;return _tmpC1;}}struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(
struct Cyc_Tcenv_Tenv*te);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Core_Opt*_tmp2ED;return(_tmp2ED=_cycalloc(
sizeof(*_tmp2ED)),((_tmp2ED->v=Cyc_Tcenv_lookup_type_vars(te),_tmp2ED)));}struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*tvs){struct Cyc_Tcenv_Fenv _tmpC5;struct Cyc_Tcenv_SharedFenv*
_tmpC6;struct Cyc_List_List*_tmpC7;struct Cyc_RgnOrder_RgnPO*_tmpC8;struct Cyc_Tcenv_Bindings*
_tmpC9;struct Cyc_Absyn_Stmt*_tmpCA;struct Cyc_Tcenv_CtrlEnv*_tmpCB;void*_tmpCC;
void*_tmpCD;int _tmpCE;const char*_tmp2EE;struct Cyc_Tcenv_Fenv*_tmpC4=Cyc_Tcenv_get_fenv(
te,((_tmp2EE="add_type_vars",_tag_dynforward(_tmp2EE,sizeof(char),
_get_zero_arr_size_char(_tmp2EE,14)))));_tmpC5=*_tmpC4;_tmpC6=_tmpC5.shared;
_tmpC7=_tmpC5.type_vars;_tmpC8=_tmpC5.region_order;_tmpC9=_tmpC5.locals;_tmpCA=
_tmpC5.encloser;_tmpCB=_tmpC5.ctrl_env;_tmpCC=(void*)_tmpC5.capability;_tmpCD=(
void*)_tmpC5.curr_rgn;_tmpCE=_tmpC5.in_notreadctxt;Cyc_Tcutil_add_tvar_identities(
tvs);{struct Cyc_List_List*_tmpCF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpC7);Cyc_Tcutil_check_unique_tvars(
loc,_tmpCF);{struct Cyc_Tcenv_Fenv*_tmp2EF;struct Cyc_Tcenv_Fenv*_tmpD0=(_tmp2EF=
_region_malloc(r,sizeof(*_tmp2EF)),((_tmp2EF->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmpC6,((_tmp2EF->type_vars=(struct Cyc_List_List*)_tmpCF,((_tmp2EF->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmpC8,((_tmp2EF->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmpC9),((_tmp2EF->encloser=(struct Cyc_Absyn_Stmt*)
_tmpCA,((_tmp2EF->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmpCB),((_tmp2EF->capability=(void*)((void*)_tmpCC),((_tmp2EF->curr_rgn=(void*)((
void*)_tmpCD),((_tmp2EF->in_notreadctxt=(int)_tmpCE,_tmp2EF)))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmpD0);}}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmpD2=te->le;
if(_tmpD2 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);else{struct Cyc_Tcenv_Fenv*fe=
Cyc_Tcenv_copy_fenv_old_ctrl(r,(struct Cyc_Tcenv_Fenv*)_tmpD2);return Cyc_Tcenv_put_fenv(
r,te,fe);}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*r,
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*
vd);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){struct _dynforward_ptr*
_tmpD3=(*vd->name).f2;{union Cyc_Absyn_Nmspace_union _tmpD4=(*vd->name).f1;_LL50:
if((_tmpD4.Loc_n).tag != 0)goto _LL52;_LL51: goto _LL4F;_LL52:;_LL53: {struct Cyc_Core_Impossible_struct
_tmp2F5;const char*_tmp2F4;struct Cyc_Core_Impossible_struct*_tmp2F3;(int)_throw((
void*)((_tmp2F3=_cycalloc(sizeof(*_tmp2F3)),((_tmp2F3[0]=((_tmp2F5.tag=Cyc_Core_Impossible,((
_tmp2F5.f1=((_tmp2F4="add_local_var: called with Rel_n",_tag_dynforward(_tmp2F4,
sizeof(char),_get_zero_arr_size_char(_tmp2F4,33)))),_tmp2F5)))),_tmp2F3)))));}
_LL4F:;}{struct Cyc_Tcenv_Fenv _tmpDA;struct Cyc_Tcenv_SharedFenv*_tmpDB;struct Cyc_List_List*
_tmpDC;struct Cyc_RgnOrder_RgnPO*_tmpDD;struct Cyc_Tcenv_Bindings*_tmpDE;struct Cyc_Absyn_Stmt*
_tmpDF;struct Cyc_Tcenv_CtrlEnv*_tmpE0;void*_tmpE1;void*_tmpE2;int _tmpE3;const
char*_tmp2F6;struct Cyc_Tcenv_Fenv*_tmpD9=Cyc_Tcenv_get_fenv(te,((_tmp2F6="add_local_var",
_tag_dynforward(_tmp2F6,sizeof(char),_get_zero_arr_size_char(_tmp2F6,14)))));
_tmpDA=*_tmpD9;_tmpDB=_tmpDA.shared;_tmpDC=_tmpDA.type_vars;_tmpDD=_tmpDA.region_order;
_tmpDE=_tmpDA.locals;_tmpDF=_tmpDA.encloser;_tmpE0=_tmpDA.ctrl_env;_tmpE1=(void*)
_tmpDA.capability;_tmpE2=(void*)_tmpDA.curr_rgn;_tmpE3=_tmpDA.in_notreadctxt;{
struct Cyc_Absyn_Local_b_struct*_tmp2FC;struct Cyc_Absyn_Local_b_struct _tmp2FB;
struct Cyc_Tcenv_Bindings*_tmp2FA;struct Cyc_Tcenv_Bindings*_tmpE4=(_tmp2FA=
_region_malloc(r,sizeof(*_tmp2FA)),((_tmp2FA->v=_tmpD3,((_tmp2FA->b=(void*)((
void*)((_tmp2FC=_cycalloc(sizeof(*_tmp2FC)),((_tmp2FC[0]=((_tmp2FB.tag=3,((
_tmp2FB.f1=vd,_tmp2FB)))),_tmp2FC))))),((_tmp2FA->tl=(struct Cyc_Tcenv_Bindings*)
_tmpDE,_tmp2FA)))))));struct Cyc_Tcenv_Fenv*_tmp2FD;struct Cyc_Tcenv_Fenv*_tmpE5=(
_tmp2FD=_region_malloc(r,sizeof(*_tmp2FD)),((_tmp2FD->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmpDB,((_tmp2FD->type_vars=(struct Cyc_List_List*)_tmpDC,((_tmp2FD->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmpDD,((_tmp2FD->locals=(struct Cyc_Tcenv_Bindings*)
_tmpE4,((_tmp2FD->encloser=(struct Cyc_Absyn_Stmt*)_tmpDF,((_tmp2FD->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmpE0),((_tmp2FD->capability=(
void*)((void*)_tmpE1),((_tmp2FD->curr_rgn=(void*)((void*)_tmpE2),((_tmp2FD->in_notreadctxt=(
int)_tmpE3,_tmp2FD)))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,_tmpE5);}}}
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*r,
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmpEA=te->le;if(_tmpEA == 0)return
Cyc_Tcenv_put_emptyfenv(r,te);{struct Cyc_Tcenv_SharedFenv*_tmpEC;struct Cyc_List_List*
_tmpED;struct Cyc_RgnOrder_RgnPO*_tmpEE;struct Cyc_Tcenv_Bindings*_tmpEF;struct Cyc_Absyn_Stmt*
_tmpF0;struct Cyc_Tcenv_CtrlEnv*_tmpF1;void*_tmpF2;void*_tmpF3;int _tmpF4;struct
Cyc_Tcenv_Fenv _tmpEB=*_tmpEA;_tmpEC=_tmpEB.shared;_tmpED=_tmpEB.type_vars;_tmpEE=
_tmpEB.region_order;_tmpEF=_tmpEB.locals;_tmpF0=_tmpEB.encloser;_tmpF1=_tmpEB.ctrl_env;
_tmpF2=(void*)_tmpEB.capability;_tmpF3=(void*)_tmpEB.curr_rgn;_tmpF4=_tmpEB.in_notreadctxt;{
struct Cyc_Tcenv_Fenv*_tmp2FE;struct Cyc_Tcenv_Fenv*_tmpF5=(_tmp2FE=_region_malloc(
r,sizeof(*_tmp2FE)),((_tmp2FE->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpEC,((
_tmp2FE->type_vars=(struct Cyc_List_List*)_tmpED,((_tmp2FE->region_order=(struct
Cyc_RgnOrder_RgnPO*)_tmpEE,((_tmp2FE->locals=(struct Cyc_Tcenv_Bindings*)((struct
Cyc_Tcenv_Bindings*)_tmpEF),((_tmp2FE->encloser=(struct Cyc_Absyn_Stmt*)_tmpF0,((
_tmp2FE->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmpF1),((
_tmp2FE->capability=(void*)((void*)_tmpF2),((_tmp2FE->curr_rgn=(void*)((void*)
_tmpF3),((_tmp2FE->in_notreadctxt=(int)1,_tmp2FE)))))))))))))))))));return Cyc_Tcenv_put_fenv(
r,te,_tmpF5);}}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*_tmpF7=te->le;
if(_tmpF7 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{struct Cyc_Tcenv_SharedFenv*
_tmpF9;struct Cyc_List_List*_tmpFA;struct Cyc_RgnOrder_RgnPO*_tmpFB;struct Cyc_Tcenv_Bindings*
_tmpFC;struct Cyc_Absyn_Stmt*_tmpFD;struct Cyc_Tcenv_CtrlEnv*_tmpFE;void*_tmpFF;
void*_tmp100;int _tmp101;struct Cyc_Tcenv_Fenv _tmpF8=*_tmpF7;_tmpF9=_tmpF8.shared;
_tmpFA=_tmpF8.type_vars;_tmpFB=_tmpF8.region_order;_tmpFC=_tmpF8.locals;_tmpFD=
_tmpF8.encloser;_tmpFE=_tmpF8.ctrl_env;_tmpFF=(void*)_tmpF8.capability;_tmp100=(
void*)_tmpF8.curr_rgn;_tmp101=_tmpF8.in_notreadctxt;{struct Cyc_Tcenv_Fenv*
_tmp2FF;struct Cyc_Tcenv_Fenv*_tmp102=(_tmp2FF=_region_malloc(r,sizeof(*_tmp2FF)),((
_tmp2FF->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpF9,((_tmp2FF->type_vars=(struct
Cyc_List_List*)_tmpFA,((_tmp2FF->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpFB,((
_tmp2FF->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)_tmpFC),((
_tmp2FF->encloser=(struct Cyc_Absyn_Stmt*)_tmpFD,((_tmp2FF->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
struct Cyc_Tcenv_CtrlEnv*)_tmpFE),((_tmp2FF->capability=(void*)((void*)_tmpFF),((
_tmp2FF->curr_rgn=(void*)((void*)_tmp100),((_tmp2FF->in_notreadctxt=(int)0,
_tmp2FF)))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,_tmp102);}}}int Cyc_Tcenv_in_notreadctxt(
struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp104=te->le;if(_tmp104 == 0)return 0;{struct Cyc_Tcenv_Fenv
_tmp106;int _tmp107;struct Cyc_Tcenv_Fenv*_tmp105=(struct Cyc_Tcenv_Fenv*)_tmp104;
_tmp106=*_tmp105;_tmp107=_tmp106.in_notreadctxt;return _tmp107;}}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_pat_var(struct _RegionHandle*r,struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Vardecl*vd){struct _dynforward_ptr*_tmp108=(*vd->name).f2;struct
Cyc_Tcenv_Fenv _tmp10B;struct Cyc_Tcenv_SharedFenv*_tmp10C;struct Cyc_List_List*
_tmp10D;struct Cyc_RgnOrder_RgnPO*_tmp10E;struct Cyc_Tcenv_Bindings*_tmp10F;struct
Cyc_Absyn_Stmt*_tmp110;struct Cyc_Tcenv_CtrlEnv*_tmp111;void*_tmp112;void*_tmp113;
int _tmp114;const char*_tmp300;struct Cyc_Tcenv_Fenv*_tmp10A=Cyc_Tcenv_get_fenv(te,((
_tmp300="add_pat_var",_tag_dynforward(_tmp300,sizeof(char),
_get_zero_arr_size_char(_tmp300,12)))));_tmp10B=*_tmp10A;_tmp10C=_tmp10B.shared;
_tmp10D=_tmp10B.type_vars;_tmp10E=_tmp10B.region_order;_tmp10F=_tmp10B.locals;
_tmp110=_tmp10B.encloser;_tmp111=_tmp10B.ctrl_env;_tmp112=(void*)_tmp10B.capability;
_tmp113=(void*)_tmp10B.curr_rgn;_tmp114=_tmp10B.in_notreadctxt;{struct Cyc_Absyn_Pat_b_struct*
_tmp306;struct Cyc_Absyn_Pat_b_struct _tmp305;struct Cyc_Tcenv_Bindings*_tmp304;
struct Cyc_Tcenv_Bindings*_tmp115=(_tmp304=_region_malloc(r,sizeof(*_tmp304)),((
_tmp304->v=_tmp108,((_tmp304->b=(void*)((void*)((_tmp306=_cycalloc(sizeof(*
_tmp306)),((_tmp306[0]=((_tmp305.tag=4,((_tmp305.f1=vd,_tmp305)))),_tmp306))))),((
_tmp304->tl=(struct Cyc_Tcenv_Bindings*)_tmp10F,_tmp304)))))));struct Cyc_Tcenv_Fenv*
_tmp307;struct Cyc_Tcenv_Fenv*_tmp116=(_tmp307=_region_malloc(r,sizeof(*_tmp307)),((
_tmp307->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp10C,((_tmp307->type_vars=(
struct Cyc_List_List*)_tmp10D,((_tmp307->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp10E,((_tmp307->locals=(struct Cyc_Tcenv_Bindings*)_tmp115,((_tmp307->encloser=(
struct Cyc_Absyn_Stmt*)_tmp110,((_tmp307->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
struct Cyc_Tcenv_CtrlEnv*)_tmp111),((_tmp307->capability=(void*)((void*)_tmp112),((
_tmp307->curr_rgn=(void*)((void*)_tmp113),((_tmp307->in_notreadctxt=(int)_tmp114,
_tmp307)))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,_tmp116);}}void*Cyc_Tcenv_lookup_ordinary(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,
struct _tuple1*q);void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,struct _tuple1*q){struct Cyc_Tcenv_Fenv*_tmp11B=
te->le;struct _tuple1 _tmp11D;union Cyc_Absyn_Nmspace_union _tmp11E;struct
_dynforward_ptr*_tmp11F;struct _tuple1*_tmp11C=q;_tmp11D=*_tmp11C;_tmp11E=_tmp11D.f1;
_tmp11F=_tmp11D.f2;{union Cyc_Absyn_Nmspace_union _tmp120=_tmp11E;struct Cyc_List_List*
_tmp121;_LL55: if((_tmp120.Loc_n).tag != 0)goto _LL57;_LL56: if(_tmp11B == 0)(int)
_throw((void*)Cyc_Dict_Absent);goto _LL58;_LL57: if((_tmp120.Rel_n).tag != 1)goto
_LL59;_tmp121=(_tmp120.Rel_n).f1;if(_tmp121 != 0)goto _LL59;if(!(_tmp11B != 0))goto
_LL59;_LL58: {struct Cyc_Tcenv_Fenv _tmp123;struct Cyc_Tcenv_Bindings*_tmp124;
struct Cyc_Tcenv_Fenv*_tmp122=(struct Cyc_Tcenv_Fenv*)_tmp11B;_tmp123=*_tmp122;
_tmp124=_tmp123.locals;{struct _handler_cons _tmp125;_push_handler(& _tmp125);{int
_tmp127=0;if(setjmp(_tmp125.handler))_tmp127=1;if(!_tmp127){{struct Cyc_Tcenv_VarRes_struct
_tmp30A;struct Cyc_Tcenv_VarRes_struct*_tmp309;void*_tmp12A=(void*)((_tmp309=
_region_malloc(r,sizeof(*_tmp309)),((_tmp309[0]=((_tmp30A.tag=0,((_tmp30A.f1=(
void*)Cyc_Tcenv_lookup_binding(_tmp124,_tmp11F),_tmp30A)))),_tmp309))));
_npop_handler(0);return _tmp12A;};_pop_handler();}else{void*_tmp126=(void*)
_exn_thrown;void*_tmp12C=_tmp126;_LL5C:;_LL5D: return(void*)Cyc_Tcenv_lookup_ordinary_global(
te,loc,q);_LL5E:;_LL5F:(void)_throw(_tmp12C);_LL5B:;}}}}_LL59:;_LL5A: {struct
_handler_cons _tmp12D;_push_handler(& _tmp12D);{int _tmp12F=0;if(setjmp(_tmp12D.handler))
_tmp12F=1;if(!_tmp12F){{void*_tmp130=(void*)Cyc_Tcenv_lookup_ordinary_global(te,
loc,q);_npop_handler(0);return _tmp130;};_pop_handler();}else{void*_tmp12E=(void*)
_exn_thrown;void*_tmp132=_tmp12E;_LL61: if(_tmp132 != Cyc_Dict_Absent)goto _LL63;
_LL62: {struct Cyc_Tcenv_VarRes_struct _tmp30D;struct Cyc_Tcenv_VarRes_struct*
_tmp30C;return(void*)((_tmp30C=_region_malloc(r,sizeof(*_tmp30C)),((_tmp30C[0]=((
_tmp30D.tag=0,((_tmp30D.f1=(void*)((void*)0),_tmp30D)))),_tmp30C))));}_LL63:;
_LL64:(void)_throw(_tmp132);_LL60:;}}}_LL54:;}}void Cyc_Tcenv_process_continue(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt);void
Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct
Cyc_Absyn_Stmt**sopt){struct Cyc_Tcenv_Fenv _tmp137;struct Cyc_Tcenv_CtrlEnv*
_tmp138;const char*_tmp30E;struct Cyc_Tcenv_Fenv*_tmp136=Cyc_Tcenv_get_fenv(te,((
_tmp30E="process_continue",_tag_dynforward(_tmp30E,sizeof(char),
_get_zero_arr_size_char(_tmp30E,17)))));_tmp137=*_tmp136;_tmp138=_tmp137.ctrl_env;{
void*_tmp139=(void*)_tmp138->continue_stmt;struct Cyc_Absyn_Stmt*_tmp13A;_LL66:
if(_tmp139 <= (void*)3)goto _LL68;if(*((int*)_tmp139)!= 0)goto _LL68;_tmp13A=((
struct Cyc_Tcenv_Stmt_j_struct*)_tmp139)->f1;_LL67:{struct Cyc_List_List*_tmp30F;
_tmp13A->non_local_preds=((_tmp30F=_cycalloc(sizeof(*_tmp30F)),((_tmp30F->hd=s,((
_tmp30F->tl=_tmp13A->non_local_preds,_tmp30F))))));}*sopt=(struct Cyc_Absyn_Stmt*)
_tmp13A;return;_LL68: if((int)_tmp139 != 0)goto _LL6A;_LL69:{const char*_tmp312;void*
_tmp311;(_tmp311=0,Cyc_Tcutil_terr(s->loc,((_tmp312="continue not in a loop",
_tag_dynforward(_tmp312,sizeof(char),_get_zero_arr_size_char(_tmp312,23)))),
_tag_dynforward(_tmp311,sizeof(void*),0)));}return;_LL6A: if((int)_tmp139 != 1)
goto _LL6C;_LL6B: goto _LL6D;_LL6C: if((int)_tmp139 != 2)goto _LL65;_LL6D: {const char*
_tmp313;((int(*)(struct _dynforward_ptr msg))Cyc_Tcenv_env_err)(((_tmp313="bad continue destination",
_tag_dynforward(_tmp313,sizeof(char),_get_zero_arr_size_char(_tmp313,25)))));}
_LL65:;}}void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s,struct Cyc_Absyn_Stmt**sopt);void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){struct Cyc_Tcenv_Fenv
_tmp141;struct Cyc_Tcenv_CtrlEnv*_tmp142;struct Cyc_Tcenv_SharedFenv*_tmp143;const
char*_tmp314;struct Cyc_Tcenv_Fenv*_tmp140=Cyc_Tcenv_get_fenv(te,((_tmp314="process_break",
_tag_dynforward(_tmp314,sizeof(char),_get_zero_arr_size_char(_tmp314,14)))));
_tmp141=*_tmp140;_tmp142=_tmp141.ctrl_env;_tmp143=_tmp141.shared;{void*_tmp144=(
void*)_tmp142->break_stmt;struct Cyc_Absyn_Stmt*_tmp145;_LL6F: if(_tmp144 <= (void*)
3)goto _LL71;if(*((int*)_tmp144)!= 0)goto _LL71;_tmp145=((struct Cyc_Tcenv_Stmt_j_struct*)
_tmp144)->f1;_LL70:{struct Cyc_List_List*_tmp315;_tmp145->non_local_preds=((
_tmp315=_cycalloc(sizeof(*_tmp315)),((_tmp315->hd=s,((_tmp315->tl=_tmp145->non_local_preds,
_tmp315))))));}*sopt=(struct Cyc_Absyn_Stmt*)_tmp145;return;_LL71: if((int)_tmp144
!= 0)goto _LL73;_LL72:{const char*_tmp318;void*_tmp317;(_tmp317=0,Cyc_Tcutil_terr(
s->loc,((_tmp318="break not in a loop or switch",_tag_dynforward(_tmp318,sizeof(
char),_get_zero_arr_size_char(_tmp318,30)))),_tag_dynforward(_tmp317,sizeof(void*),
0)));}return;_LL73: if((int)_tmp144 != 2)goto _LL75;_LL74: if((void*)_tmp143->return_typ
!= (void*)((void*)0)){const char*_tmp31B;void*_tmp31A;(_tmp31A=0,Cyc_Tcutil_terr(
s->loc,((_tmp31B="break causes function not to return a value",_tag_dynforward(
_tmp31B,sizeof(char),_get_zero_arr_size_char(_tmp31B,44)))),_tag_dynforward(
_tmp31A,sizeof(void*),0)));}return;_LL75: if((int)_tmp144 != 1)goto _LL6E;_LL76:{
const char*_tmp31E;void*_tmp31D;(_tmp31D=0,Cyc_Tcutil_terr(s->loc,((_tmp31E="break causes outer switch clause to implicitly fallthru",
_tag_dynforward(_tmp31E,sizeof(char),_get_zero_arr_size_char(_tmp31E,56)))),
_tag_dynforward(_tmp31D,sizeof(void*),0)));}return;_LL6E:;}}void Cyc_Tcenv_process_goto(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct _dynforward_ptr*l,struct Cyc_Absyn_Stmt**
sopt);void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,
struct _dynforward_ptr*l,struct Cyc_Absyn_Stmt**sopt){struct Cyc_Tcenv_Fenv _tmp14F;
struct Cyc_Tcenv_SharedFenv*_tmp150;const char*_tmp31F;struct Cyc_Tcenv_Fenv*
_tmp14E=Cyc_Tcenv_get_fenv(te,((_tmp31F="process_goto",_tag_dynforward(_tmp31F,
sizeof(char),_get_zero_arr_size_char(_tmp31F,13)))));_tmp14F=*_tmp14E;_tmp150=
_tmp14F.shared;{struct Cyc_Absyn_Stmt**sopt2=((struct Cyc_Absyn_Stmt**(*)(struct
Cyc_Dict_Dict d,struct _dynforward_ptr*k))Cyc_Dict_lookup_opt)(_tmp150->seen_labels,
l);if(sopt2 == 0){struct Cyc_Dict_Dict _tmp151=_tmp150->needed_labels;struct Cyc_List_List**
slopt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dynforward_ptr*k))
Cyc_Dict_lookup_opt)(_tmp151,l);struct _RegionHandle*frgn=_tmp150->frgn;if(slopt
== 0){struct Cyc_List_List**_tmp320;slopt=((_tmp320=_region_malloc(frgn,sizeof(*
_tmp320)),((_tmp320[0]=0,_tmp320))));}{struct Cyc_List_List*_tmp321;struct Cyc_List_List*
new_needed=(_tmp321=_cycalloc(sizeof(*_tmp321)),((_tmp321->hd=s,((_tmp321->tl=*((
struct Cyc_List_List**)_check_null(slopt)),_tmp321)))));_tmp150->needed_labels=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dynforward_ptr*k,struct Cyc_List_List*
v))Cyc_Dict_insert)(_tmp151,l,new_needed);}}else{struct Cyc_Absyn_Stmt*s=*sopt2;{
struct Cyc_List_List*_tmp322;s->non_local_preds=((_tmp322=_cycalloc(sizeof(*
_tmp322)),((_tmp322->hd=s,((_tmp322->tl=s->non_local_preds,_tmp322))))));}*sopt=(
struct Cyc_Absyn_Stmt*)s;}}}struct _tuple4*Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt);struct
_tuple4*Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
s,struct Cyc_Absyn_Switch_clause***clauseopt){struct Cyc_Tcenv_Fenv _tmp157;struct
Cyc_Tcenv_CtrlEnv*_tmp158;const char*_tmp323;struct Cyc_Tcenv_Fenv*_tmp156=Cyc_Tcenv_get_fenv(
te,((_tmp323="process_fallthru",_tag_dynforward(_tmp323,sizeof(char),
_get_zero_arr_size_char(_tmp323,17)))));_tmp157=*_tmp156;_tmp158=_tmp157.ctrl_env;{
struct _tuple4*_tmp159=(struct _tuple4*)_tmp158->fallthru_clause;if(_tmp159 != 0){{
struct Cyc_List_List*_tmp324;(((*_tmp159).f1)->body)->non_local_preds=((_tmp324=
_cycalloc(sizeof(*_tmp324)),((_tmp324->hd=s,((_tmp324->tl=(((*_tmp159).f1)->body)->non_local_preds,
_tmp324))))));}{struct Cyc_Absyn_Switch_clause**_tmp325;*clauseopt=(struct Cyc_Absyn_Switch_clause**)((
_tmp325=_cycalloc(sizeof(*_tmp325)),((_tmp325[0]=(*_tmp159).f1,_tmp325))));}}
return _tmp159;}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*
r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,
struct Cyc_Absyn_Switch_clause*clause);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct
Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){struct Cyc_Tcenv_Fenv
_tmp15E;struct Cyc_Tcenv_SharedFenv*_tmp15F;struct Cyc_List_List*_tmp160;struct Cyc_RgnOrder_RgnPO*
_tmp161;struct Cyc_Tcenv_Bindings*_tmp162;struct Cyc_Absyn_Stmt*_tmp163;struct Cyc_Tcenv_CtrlEnv*
_tmp164;void*_tmp165;void*_tmp166;int _tmp167;const char*_tmp326;struct Cyc_Tcenv_Fenv*
_tmp15D=Cyc_Tcenv_get_fenv(te,((_tmp326="set_fallthru",_tag_dynforward(_tmp326,
sizeof(char),_get_zero_arr_size_char(_tmp326,13)))));_tmp15E=*_tmp15D;_tmp15F=
_tmp15E.shared;_tmp160=_tmp15E.type_vars;_tmp161=_tmp15E.region_order;_tmp162=
_tmp15E.locals;_tmp163=_tmp15E.encloser;_tmp164=_tmp15E.ctrl_env;_tmp165=(void*)
_tmp15E.capability;_tmp166=(void*)_tmp15E.curr_rgn;_tmp167=_tmp15E.in_notreadctxt;{
struct Cyc_Tcenv_CtrlEnv _tmp169;struct _RegionHandle*_tmp16A;void*_tmp16B;void*
_tmp16C;struct _tuple4*_tmp16D;void*_tmp16E;int _tmp16F;struct Cyc_Tcenv_CtrlEnv*
_tmp168=_tmp164;_tmp169=*_tmp168;_tmp16A=_tmp169.ctrl_rgn;_tmp16B=(void*)_tmp169.continue_stmt;
_tmp16C=(void*)_tmp169.break_stmt;_tmp16D=_tmp169.fallthru_clause;_tmp16E=(void*)
_tmp169.next_stmt;_tmp16F=_tmp169.try_depth;{struct Cyc_List_List*ft_typ=0;for(0;
vds != 0;vds=vds->tl){struct Cyc_List_List*_tmp327;ft_typ=((_tmp327=_region_malloc(
_tmp16A,sizeof(*_tmp327)),((_tmp327->hd=(void*)((void*)((struct Cyc_Absyn_Vardecl*)
vds->hd)->type),((_tmp327->tl=ft_typ,_tmp327))))));}{struct Cyc_Tcenv_CList*
_tmp171=(struct Cyc_Tcenv_CList*)Cyc_List_imp_rev(ft_typ);struct _tuple4*_tmp32A;
struct Cyc_Tcenv_CtrlEnv*_tmp329;struct Cyc_Tcenv_CtrlEnv*_tmp172=(_tmp329=
_region_malloc(r,sizeof(*_tmp329)),((_tmp329->ctrl_rgn=_tmp16A,((_tmp329->continue_stmt=(
void*)_tmp16B,((_tmp329->break_stmt=(void*)_tmp16C,((_tmp329->fallthru_clause=((
_tmp32A=_region_malloc(_tmp16A,sizeof(*_tmp32A)),((_tmp32A->f1=clause,((_tmp32A->f2=
new_tvs,((_tmp32A->f3=_tmp171,_tmp32A)))))))),((_tmp329->next_stmt=(void*)
_tmp16E,((_tmp329->try_depth=_tmp16F,_tmp329)))))))))))));struct Cyc_Tcenv_Fenv*
_tmp32B;struct Cyc_Tcenv_Fenv*_tmp173=(_tmp32B=_region_malloc(r,sizeof(*_tmp32B)),((
_tmp32B->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp15F,((_tmp32B->type_vars=(
struct Cyc_List_List*)_tmp160,((_tmp32B->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp161,((_tmp32B->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmp162),((_tmp32B->encloser=(struct Cyc_Absyn_Stmt*)_tmp163,((_tmp32B->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)_tmp172,((_tmp32B->capability=(void*)((void*)_tmp165),((
_tmp32B->curr_rgn=(void*)((void*)_tmp166),((_tmp32B->in_notreadctxt=(int)_tmp167,
_tmp32B)))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,_tmp173);}}}}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_clear_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te){const char*_tmp32C;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(
te,((_tmp32C="clear_fallthru",_tag_dynforward(_tmp32C,sizeof(char),
_get_zero_arr_size_char(_tmp32C,15))))));struct Cyc_Tcenv_Fenv _tmp178;struct Cyc_Tcenv_CtrlEnv*
_tmp179;struct Cyc_Tcenv_Fenv*_tmp177=fe;_tmp178=*_tmp177;_tmp179=_tmp178.ctrl_env;
_tmp179->fallthru_clause=0;return Cyc_Tcenv_put_fenv(r,te,fe);}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_in_loop(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*
continue_dest);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*r,
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest){struct Cyc_Tcenv_Fenv
_tmp17D;struct Cyc_Tcenv_SharedFenv*_tmp17E;struct Cyc_List_List*_tmp17F;struct Cyc_RgnOrder_RgnPO*
_tmp180;struct Cyc_Tcenv_Bindings*_tmp181;struct Cyc_Absyn_Stmt*_tmp182;struct Cyc_Tcenv_CtrlEnv*
_tmp183;void*_tmp184;void*_tmp185;int _tmp186;const char*_tmp32D;struct Cyc_Tcenv_Fenv*
_tmp17C=Cyc_Tcenv_get_fenv(te,((_tmp32D="set_in_loop",_tag_dynforward(_tmp32D,
sizeof(char),_get_zero_arr_size_char(_tmp32D,12)))));_tmp17D=*_tmp17C;_tmp17E=
_tmp17D.shared;_tmp17F=_tmp17D.type_vars;_tmp180=_tmp17D.region_order;_tmp181=
_tmp17D.locals;_tmp182=_tmp17D.encloser;_tmp183=_tmp17D.ctrl_env;_tmp184=(void*)
_tmp17D.capability;_tmp185=(void*)_tmp17D.curr_rgn;_tmp186=_tmp17D.in_notreadctxt;{
struct Cyc_Tcenv_Stmt_j_struct*_tmp338;struct Cyc_Tcenv_Stmt_j_struct _tmp337;
struct Cyc_Tcenv_Stmt_j_struct*_tmp336;struct Cyc_Tcenv_Stmt_j_struct _tmp335;
struct Cyc_Tcenv_CtrlEnv*_tmp334;struct Cyc_Tcenv_CtrlEnv*new_cenv=(_tmp334=
_region_malloc(r,sizeof(*_tmp334)),((_tmp334->ctrl_rgn=r,((_tmp334->continue_stmt=(
void*)((void*)((_tmp338=_region_malloc(r,sizeof(*_tmp338)),((_tmp338[0]=((
_tmp337.tag=0,((_tmp337.f1=continue_dest,_tmp337)))),_tmp338))))),((_tmp334->break_stmt=(
void*)((void*)((void*)_tmp183->next_stmt)),((_tmp334->next_stmt=(void*)((void*)((
_tmp336=_region_malloc(r,sizeof(*_tmp336)),((_tmp336[0]=((_tmp335.tag=0,((
_tmp335.f1=continue_dest,_tmp335)))),_tmp336))))),((_tmp334->fallthru_clause=(
struct _tuple4*)_tmp183->fallthru_clause,((_tmp334->try_depth=_tmp183->try_depth,
_tmp334)))))))))))));struct Cyc_Tcenv_Fenv*_tmp339;struct Cyc_Tcenv_Fenv*new_fenv=(
_tmp339=_region_malloc(r,sizeof(*_tmp339)),((_tmp339->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp17E,((_tmp339->type_vars=(struct Cyc_List_List*)_tmp17F,((_tmp339->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp180,((_tmp339->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp181),((_tmp339->encloser=(struct Cyc_Absyn_Stmt*)
_tmp182,((_tmp339->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp339->capability=(
void*)((void*)_tmp184),((_tmp339->curr_rgn=(void*)((void*)_tmp185),((_tmp339->in_notreadctxt=(
int)_tmp186,_tmp339)))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,new_fenv);}}
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te){const char*_tmp33A;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(
te,((_tmp33A="enter_try",_tag_dynforward(_tmp33A,sizeof(char),
_get_zero_arr_size_char(_tmp33A,10))))));struct Cyc_Tcenv_Fenv _tmp18E;struct Cyc_Tcenv_CtrlEnv*
_tmp18F;struct Cyc_Tcenv_Fenv*_tmp18D=fe;_tmp18E=*_tmp18D;_tmp18F=_tmp18E.ctrl_env;
++ _tmp18F->try_depth;return Cyc_Tcenv_put_fenv(r,te,fe);}int Cyc_Tcenv_get_try_depth(
struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv _tmp193;struct Cyc_Tcenv_CtrlEnv*_tmp194;const char*_tmp33B;
struct Cyc_Tcenv_Fenv*_tmp192=Cyc_Tcenv_get_fenv(te,((_tmp33B="get_try_depth",
_tag_dynforward(_tmp33B,sizeof(char),_get_zero_arr_size_char(_tmp33B,14)))));
_tmp193=*_tmp192;_tmp194=_tmp193.ctrl_env;return _tmp194->try_depth;}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_in_switch(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_set_in_switch(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){const char*
_tmp33C;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(
te,((_tmp33C="set_in_switch",_tag_dynforward(_tmp33C,sizeof(char),
_get_zero_arr_size_char(_tmp33C,14))))));struct Cyc_Tcenv_Fenv _tmp196;struct Cyc_Tcenv_CtrlEnv*
_tmp197;struct Cyc_Tcenv_Fenv*_tmp195=fe;_tmp196=*_tmp195;_tmp197=_tmp196.ctrl_env;(
void*)(_tmp197->break_stmt=(void*)((void*)_tmp197->next_stmt));(void*)(_tmp197->next_stmt=(
void*)((void*)1));return Cyc_Tcenv_put_fenv(r,te,fe);}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*j);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*j){struct Cyc_Tcenv_SharedFenv*
_tmp19B;struct Cyc_List_List*_tmp19C;struct Cyc_RgnOrder_RgnPO*_tmp19D;struct Cyc_Tcenv_Bindings*
_tmp19E;struct Cyc_Absyn_Stmt*_tmp19F;struct Cyc_Tcenv_CtrlEnv*_tmp1A0;void*
_tmp1A1;void*_tmp1A2;int _tmp1A3;const char*_tmp33D;struct Cyc_Tcenv_Fenv _tmp19A=*
Cyc_Tcenv_get_fenv(te,((_tmp33D="set_next",_tag_dynforward(_tmp33D,sizeof(char),
_get_zero_arr_size_char(_tmp33D,9)))));_tmp19B=_tmp19A.shared;_tmp19C=_tmp19A.type_vars;
_tmp19D=_tmp19A.region_order;_tmp19E=_tmp19A.locals;_tmp19F=_tmp19A.encloser;
_tmp1A0=_tmp19A.ctrl_env;_tmp1A1=(void*)_tmp19A.capability;_tmp1A2=(void*)
_tmp19A.curr_rgn;_tmp1A3=_tmp19A.in_notreadctxt;{struct Cyc_Tcenv_CtrlEnv*_tmp33E;
struct Cyc_Tcenv_CtrlEnv*new_cenv=(_tmp33E=_region_malloc(r,sizeof(*_tmp33E)),((
_tmp33E->ctrl_rgn=r,((_tmp33E->continue_stmt=(void*)((void*)((void*)_tmp1A0->continue_stmt)),((
_tmp33E->break_stmt=(void*)((void*)((void*)_tmp1A0->break_stmt)),((_tmp33E->next_stmt=(
void*)j,((_tmp33E->fallthru_clause=(struct _tuple4*)_tmp1A0->fallthru_clause,((
_tmp33E->try_depth=_tmp1A0->try_depth,_tmp33E)))))))))))));struct Cyc_Tcenv_Fenv*
_tmp33F;struct Cyc_Tcenv_Fenv*new_fenv=(_tmp33F=_region_malloc(r,sizeof(*_tmp33F)),((
_tmp33F->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp19B,((_tmp33F->type_vars=(
struct Cyc_List_List*)_tmp19C,((_tmp33F->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp19D,((_tmp33F->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmp19E),((_tmp33F->encloser=(struct Cyc_Absyn_Stmt*)_tmp19F,((_tmp33F->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp33F->capability=(void*)((void*)_tmp1A1),((
_tmp33F->curr_rgn=(void*)((void*)_tmp1A2),((_tmp33F->in_notreadctxt=(int)_tmp1A3,
_tmp33F)))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,new_fenv);}}struct Cyc_Tcenv_Tenv*
Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*te,struct _dynforward_ptr*v,struct Cyc_Absyn_Stmt*
s);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*te,struct
_dynforward_ptr*v,struct Cyc_Absyn_Stmt*s){struct Cyc_Tcenv_Fenv _tmp1A8;struct Cyc_Tcenv_SharedFenv*
_tmp1A9;const char*_tmp340;struct Cyc_Tcenv_Fenv*_tmp1A7=Cyc_Tcenv_get_fenv(te,((
_tmp340="add_label",_tag_dynforward(_tmp340,sizeof(char),_get_zero_arr_size_char(
_tmp340,10)))));_tmp1A8=*_tmp1A7;_tmp1A9=_tmp1A8.shared;{struct Cyc_Dict_Dict
needed=_tmp1A9->needed_labels;struct Cyc_List_List**sl_opt=((struct Cyc_List_List**(*)(
struct Cyc_Dict_Dict d,struct _dynforward_ptr*k))Cyc_Dict_lookup_opt)(needed,v);
struct _RegionHandle*frgn=_tmp1A9->frgn;if(sl_opt != 0){_tmp1A9->needed_labels=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct
_dynforward_ptr*))Cyc_Dict_rdelete)(frgn,needed,v);{struct Cyc_List_List*_tmp1AA=*
sl_opt;s->non_local_preds=_tmp1AA;for(0;_tmp1AA != 0;_tmp1AA=_tmp1AA->tl){void*
_tmp1AB=(void*)((struct Cyc_Absyn_Stmt*)_tmp1AA->hd)->r;struct Cyc_Absyn_Stmt*
_tmp1AC;struct Cyc_Absyn_Stmt**_tmp1AD;_LL78: if(_tmp1AB <= (void*)1)goto _LL7A;if(*((
int*)_tmp1AB)!= 7)goto _LL7A;_tmp1AC=((struct Cyc_Absyn_Goto_s_struct*)_tmp1AB)->f2;
_tmp1AD=(struct Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Goto_s_struct*)_tmp1AB)->f2;
_LL79:*_tmp1AD=(struct Cyc_Absyn_Stmt*)s;goto _LL77;_LL7A:;_LL7B:{const char*
_tmp341;((int(*)(struct _dynforward_ptr msg))Cyc_Tcenv_env_err)(((_tmp341="Tcenv: add_label backpatching of non-goto",
_tag_dynforward(_tmp341,sizeof(char),_get_zero_arr_size_char(_tmp341,42)))));}
goto _LL77;_LL77:;}}}if(((int(*)(struct Cyc_Dict_Dict d,struct _dynforward_ptr*k))
Cyc_Dict_member)(_tmp1A9->seen_labels,v)){const char*_tmp345;void*_tmp344[1];
struct Cyc_String_pa_struct _tmp343;(_tmp343.tag=0,((_tmp343.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*v),((_tmp344[0]=& _tmp343,Cyc_Tcutil_terr(
s->loc,((_tmp345="Repeated label: %s",_tag_dynforward(_tmp345,sizeof(char),
_get_zero_arr_size_char(_tmp345,19)))),_tag_dynforward(_tmp344,sizeof(void*),1)))))));}
_tmp1A9->seen_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_dynforward_ptr*k,struct Cyc_Absyn_Stmt*v))Cyc_Dict_insert)(_tmp1A9->seen_labels,
v,s);return te;}}int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_all_labels_resolved(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmp1B4;struct Cyc_Tcenv_SharedFenv*
_tmp1B5;const char*_tmp346;struct Cyc_Tcenv_Fenv*_tmp1B3=Cyc_Tcenv_get_fenv(te,((
_tmp346="all_labels_resolved",_tag_dynforward(_tmp346,sizeof(char),
_get_zero_arr_size_char(_tmp346,20)))));_tmp1B4=*_tmp1B3;_tmp1B5=_tmp1B4.shared;
return((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp1B5->needed_labels);}
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*te);struct Cyc_Absyn_Stmt*
Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmp1B8;
struct Cyc_Absyn_Stmt*_tmp1B9;const char*_tmp347;struct Cyc_Tcenv_Fenv*_tmp1B7=Cyc_Tcenv_get_fenv(
te,((_tmp347="get_encloser",_tag_dynforward(_tmp347,sizeof(char),
_get_zero_arr_size_char(_tmp347,13)))));_tmp1B8=*_tmp1B7;_tmp1B9=_tmp1B8.encloser;
return _tmp1B9;}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*r,
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){struct Cyc_Tcenv_SharedFenv*
_tmp1BC;struct Cyc_List_List*_tmp1BD;struct Cyc_RgnOrder_RgnPO*_tmp1BE;struct Cyc_Tcenv_Bindings*
_tmp1BF;struct Cyc_Absyn_Stmt*_tmp1C0;struct Cyc_Tcenv_CtrlEnv*_tmp1C1;void*
_tmp1C2;void*_tmp1C3;int _tmp1C4;const char*_tmp348;struct Cyc_Tcenv_Fenv _tmp1BB=*
Cyc_Tcenv_get_fenv(te,((_tmp348="set_encloser",_tag_dynforward(_tmp348,sizeof(
char),_get_zero_arr_size_char(_tmp348,13)))));_tmp1BC=_tmp1BB.shared;_tmp1BD=
_tmp1BB.type_vars;_tmp1BE=_tmp1BB.region_order;_tmp1BF=_tmp1BB.locals;_tmp1C0=
_tmp1BB.encloser;_tmp1C1=_tmp1BB.ctrl_env;_tmp1C2=(void*)_tmp1BB.capability;
_tmp1C3=(void*)_tmp1BB.curr_rgn;_tmp1C4=_tmp1BB.in_notreadctxt;{struct Cyc_Tcenv_Fenv*
_tmp349;struct Cyc_Tcenv_Fenv*_tmp1C5=(_tmp349=_region_malloc(r,sizeof(*_tmp349)),((
_tmp349->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1BC,((_tmp349->type_vars=(
struct Cyc_List_List*)_tmp1BD,((_tmp349->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp1BE,((_tmp349->locals=(struct Cyc_Tcenv_Bindings*)((struct Cyc_Tcenv_Bindings*)
_tmp1BF),((_tmp349->encloser=(struct Cyc_Absyn_Stmt*)s,((_tmp349->ctrl_env=(
struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)_tmp1C1),((_tmp349->capability=(
void*)((void*)_tmp1C2),((_tmp349->curr_rgn=(void*)((void*)_tmp1C3),((_tmp349->in_notreadctxt=(
int)_tmp1C4,_tmp349)))))))))))))))))));return Cyc_Tcenv_put_fenv(r,te,_tmp1C5);}}
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*
te,void*rgn,int resetable,int opened);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened){
struct Cyc_Absyn_Tvar*tv;{void*_tmp1C7=Cyc_Tcutil_compress(rgn);struct Cyc_Absyn_Tvar*
_tmp1C8;_LL7D: if(_tmp1C7 <= (void*)4)goto _LL7F;if(*((int*)_tmp1C7)!= 1)goto _LL7F;
_tmp1C8=((struct Cyc_Absyn_VarType_struct*)_tmp1C7)->f1;_LL7E: tv=_tmp1C8;goto
_LL7C;_LL7F:;_LL80:{const char*_tmp34A;tv=((struct Cyc_Absyn_Tvar*(*)(struct
_dynforward_ptr msg))Cyc_Tcenv_env_err)(((_tmp34A="bad add region",
_tag_dynforward(_tmp34A,sizeof(char),_get_zero_arr_size_char(_tmp34A,15)))));}
goto _LL7C;_LL7C:;}{struct Cyc_Tcenv_SharedFenv*_tmp1CC;struct Cyc_List_List*
_tmp1CD;struct Cyc_RgnOrder_RgnPO*_tmp1CE;struct Cyc_Tcenv_Bindings*_tmp1CF;struct
Cyc_Absyn_Stmt*_tmp1D0;struct Cyc_Tcenv_CtrlEnv*_tmp1D1;void*_tmp1D2;void*_tmp1D3;
int _tmp1D4;const char*_tmp34B;struct Cyc_Tcenv_Fenv _tmp1CB=*Cyc_Tcenv_get_fenv(te,((
_tmp34B="add_region",_tag_dynforward(_tmp34B,sizeof(char),
_get_zero_arr_size_char(_tmp34B,11)))));_tmp1CC=_tmp1CB.shared;_tmp1CD=_tmp1CB.type_vars;
_tmp1CE=_tmp1CB.region_order;_tmp1CF=_tmp1CB.locals;_tmp1D0=_tmp1CB.encloser;
_tmp1D1=_tmp1CB.ctrl_env;_tmp1D2=(void*)_tmp1CB.capability;_tmp1D3=(void*)
_tmp1CB.curr_rgn;_tmp1D4=_tmp1CB.in_notreadctxt;_tmp1CE=Cyc_RgnOrder_add_youngest(
_tmp1CC->frgn,_tmp1CE,tv,resetable,opened);{struct Cyc_Absyn_JoinEff_struct
_tmp35E;struct Cyc_Absyn_AccessEff_struct*_tmp35D;struct Cyc_Absyn_AccessEff_struct
_tmp35C;struct Cyc_List_List*_tmp35B;struct Cyc_List_List*_tmp35A;struct Cyc_Absyn_JoinEff_struct*
_tmp359;_tmp1D2=(void*)((_tmp359=_cycalloc(sizeof(*_tmp359)),((_tmp359[0]=((
_tmp35E.tag=21,((_tmp35E.f1=((_tmp35A=_cycalloc(sizeof(*_tmp35A)),((_tmp35A->hd=(
void*)((void*)((_tmp35D=_cycalloc(sizeof(*_tmp35D)),((_tmp35D[0]=((_tmp35C.tag=
20,((_tmp35C.f1=(void*)rgn,_tmp35C)))),_tmp35D))))),((_tmp35A->tl=((_tmp35B=
_cycalloc(sizeof(*_tmp35B)),((_tmp35B->hd=(void*)_tmp1D2,((_tmp35B->tl=0,_tmp35B)))))),
_tmp35A)))))),_tmp35E)))),_tmp359))));}{struct Cyc_Tcenv_Fenv*_tmp35F;struct Cyc_Tcenv_Fenv*
_tmp1DB=(_tmp35F=_region_malloc(r,sizeof(*_tmp35F)),((_tmp35F->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp1CC,((_tmp35F->type_vars=(struct Cyc_List_List*)_tmp1CD,((_tmp35F->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp1CE,((_tmp35F->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp1CF),((_tmp35F->encloser=(struct Cyc_Absyn_Stmt*)
_tmp1D0,((_tmp35F->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmp1D1),((_tmp35F->capability=(void*)((void*)_tmp1D2),((_tmp35F->curr_rgn=(void*)((
void*)_tmp1D3),((_tmp35F->in_notreadctxt=(int)_tmp1D4,_tmp35F)))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp1DB);}}}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Tvar*block_rgn);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Tvar*block_rgn){struct Cyc_Tcenv_SharedFenv*_tmp1DF;struct Cyc_List_List*
_tmp1E0;struct Cyc_RgnOrder_RgnPO*_tmp1E1;struct Cyc_Tcenv_Bindings*_tmp1E2;struct
Cyc_Absyn_Stmt*_tmp1E3;struct Cyc_Tcenv_CtrlEnv*_tmp1E4;void*_tmp1E5;void*_tmp1E6;
int _tmp1E7;const char*_tmp360;struct Cyc_Tcenv_Fenv _tmp1DE=*Cyc_Tcenv_get_fenv(te,((
_tmp360="new_named_block",_tag_dynforward(_tmp360,sizeof(char),
_get_zero_arr_size_char(_tmp360,16)))));_tmp1DF=_tmp1DE.shared;_tmp1E0=_tmp1DE.type_vars;
_tmp1E1=_tmp1DE.region_order;_tmp1E2=_tmp1DE.locals;_tmp1E3=_tmp1DE.encloser;
_tmp1E4=_tmp1DE.ctrl_env;_tmp1E5=(void*)_tmp1DE.capability;_tmp1E6=(void*)
_tmp1DE.curr_rgn;_tmp1E7=_tmp1DE.in_notreadctxt;{const char*_tmp361;struct Cyc_Tcenv_Fenv*
fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp361="new_block",
_tag_dynforward(_tmp361,sizeof(char),_get_zero_arr_size_char(_tmp361,10))))));
struct Cyc_Absyn_VarType_struct _tmp364;struct Cyc_Absyn_VarType_struct*_tmp363;
void*block_typ=(void*)((_tmp363=_cycalloc(sizeof(*_tmp363)),((_tmp363[0]=((
_tmp364.tag=1,((_tmp364.f1=block_rgn,_tmp364)))),_tmp363))));{struct Cyc_List_List*
_tmp365;_tmp1E0=((_tmp365=_cycalloc(sizeof(*_tmp365)),((_tmp365->hd=block_rgn,((
_tmp365->tl=_tmp1E0,_tmp365))))));}Cyc_Tcutil_check_unique_tvars(loc,_tmp1E0);
_tmp1E1=Cyc_RgnOrder_add_youngest(_tmp1DF->frgn,_tmp1E1,block_rgn,0,0);{struct
Cyc_Absyn_JoinEff_struct _tmp378;struct Cyc_Absyn_AccessEff_struct*_tmp377;struct
Cyc_Absyn_AccessEff_struct _tmp376;struct Cyc_List_List*_tmp375;struct Cyc_List_List*
_tmp374;struct Cyc_Absyn_JoinEff_struct*_tmp373;_tmp1E5=(void*)((_tmp373=
_cycalloc(sizeof(*_tmp373)),((_tmp373[0]=((_tmp378.tag=21,((_tmp378.f1=((_tmp374=
_cycalloc(sizeof(*_tmp374)),((_tmp374->hd=(void*)((void*)((_tmp377=_cycalloc(
sizeof(*_tmp377)),((_tmp377[0]=((_tmp376.tag=20,((_tmp376.f1=(void*)block_typ,
_tmp376)))),_tmp377))))),((_tmp374->tl=((_tmp375=_cycalloc(sizeof(*_tmp375)),((
_tmp375->hd=(void*)_tmp1E5,((_tmp375->tl=0,_tmp375)))))),_tmp374)))))),_tmp378)))),
_tmp373))));}_tmp1E6=block_typ;{struct Cyc_Tcenv_Fenv*_tmp379;struct Cyc_Tcenv_Fenv*
_tmp1EF=(_tmp379=_region_malloc(r,sizeof(*_tmp379)),((_tmp379->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp1DF,((_tmp379->type_vars=(struct Cyc_List_List*)_tmp1E0,((_tmp379->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp1E1,((_tmp379->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp1E2),((_tmp379->encloser=(struct Cyc_Absyn_Stmt*)
_tmp1E3,((_tmp379->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmp1E4),((_tmp379->capability=(void*)((void*)_tmp1E5),((_tmp379->curr_rgn=(void*)((
void*)_tmp1E6),((_tmp379->in_notreadctxt=(int)_tmp1E7,_tmp379)))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp1EF);}}}static struct Cyc_Absyn_Eq_kb_struct Cyc_Tcenv_rgn_kb={
0,(void*)((void*)3)};struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct
_RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te);struct
Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te){struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)&
Cyc_Tcenv_rgn_kb);Cyc_Tcutil_add_tvar_identity(t);return Cyc_Tcenv_new_named_block(
r,loc,te,t);}struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct
_RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,struct Cyc_Position_Segment*
loc);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*
r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,struct Cyc_Position_Segment*loc){
struct Cyc_Tcenv_SharedFenv*_tmp1F7;struct Cyc_List_List*_tmp1F8;struct Cyc_RgnOrder_RgnPO*
_tmp1F9;struct Cyc_Tcenv_Bindings*_tmp1FA;struct Cyc_Absyn_Stmt*_tmp1FB;struct Cyc_Tcenv_CtrlEnv*
_tmp1FC;void*_tmp1FD;void*_tmp1FE;int _tmp1FF;const char*_tmp37A;struct Cyc_Tcenv_Fenv
_tmp1F6=*Cyc_Tcenv_get_fenv(te,((_tmp37A="new_outlives_constraints",
_tag_dynforward(_tmp37A,sizeof(char),_get_zero_arr_size_char(_tmp37A,25)))));
_tmp1F7=_tmp1F6.shared;_tmp1F8=_tmp1F6.type_vars;_tmp1F9=_tmp1F6.region_order;
_tmp1FA=_tmp1F6.locals;_tmp1FB=_tmp1F6.encloser;_tmp1FC=_tmp1F6.ctrl_env;_tmp1FD=(
void*)_tmp1F6.capability;_tmp1FE=(void*)_tmp1F6.curr_rgn;_tmp1FF=_tmp1F6.in_notreadctxt;
for(0;cs != 0;cs=cs->tl){_tmp1F9=Cyc_RgnOrder_add_outlives_constraint(_tmp1F7->frgn,
_tmp1F9,(*((struct _tuple0*)cs->hd)).f1,(*((struct _tuple0*)cs->hd)).f2,loc);}{
struct Cyc_Tcenv_Fenv*_tmp37B;struct Cyc_Tcenv_Fenv*_tmp200=(_tmp37B=
_region_malloc(r,sizeof(*_tmp37B)),((_tmp37B->shared=(struct Cyc_Tcenv_SharedFenv*)
_tmp1F7,((_tmp37B->type_vars=(struct Cyc_List_List*)_tmp1F8,((_tmp37B->region_order=(
struct Cyc_RgnOrder_RgnPO*)_tmp1F9,((_tmp37B->locals=(struct Cyc_Tcenv_Bindings*)((
struct Cyc_Tcenv_Bindings*)_tmp1FA),((_tmp37B->encloser=(struct Cyc_Absyn_Stmt*)
_tmp1FB,((_tmp37B->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((struct Cyc_Tcenv_CtrlEnv*)
_tmp1FC),((_tmp37B->capability=(void*)((void*)_tmp1FD),((_tmp37B->curr_rgn=(void*)((
void*)_tmp1FE),((_tmp37B->in_notreadctxt=(int)_tmp1FF,_tmp37B)))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp200);}}void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*
te);void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv*
_tmp202=te->le;if(_tmp202 == 0)return(void*)2;{struct Cyc_Tcenv_Fenv*fe=(struct Cyc_Tcenv_Fenv*)
_tmp202;struct Cyc_Tcenv_Fenv _tmp204;void*_tmp205;struct Cyc_Tcenv_Fenv*_tmp203=fe;
_tmp204=*_tmp203;_tmp205=(void*)_tmp204.curr_rgn;return _tmp205;}}void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*rgn);void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*rgn){const char*
_tmp37C;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te,((_tmp37C="check_rgn_accessible",
_tag_dynforward(_tmp37C,sizeof(char),_get_zero_arr_size_char(_tmp37C,21)))));
struct Cyc_Tcenv_Fenv _tmp207;void*_tmp208;struct Cyc_RgnOrder_RgnPO*_tmp209;struct
Cyc_Tcenv_Fenv*_tmp206=fe;_tmp207=*_tmp206;_tmp208=(void*)_tmp207.capability;
_tmp209=_tmp207.region_order;if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp208) || 
Cyc_Tcutil_region_in_effect(1,rgn,_tmp208))return;{struct Cyc_Absyn_AccessEff_struct
_tmp37F;struct Cyc_Absyn_AccessEff_struct*_tmp37E;if(Cyc_RgnOrder_eff_outlives_eff(
_tmp209,(void*)((_tmp37E=_cycalloc(sizeof(*_tmp37E)),((_tmp37E[0]=((_tmp37F.tag=
20,((_tmp37F.f1=(void*)rgn,_tmp37F)))),_tmp37E)))),_tmp208))return;}{const char*
_tmp383;void*_tmp382[1];struct Cyc_String_pa_struct _tmp381;(_tmp381.tag=0,((
_tmp381.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
rgn)),((_tmp382[0]=& _tmp381,Cyc_Tcutil_terr(loc,((_tmp383="Expression accesses unavailable region %s",
_tag_dynforward(_tmp383,sizeof(char),_get_zero_arr_size_char(_tmp383,42)))),
_tag_dynforward(_tmp382,sizeof(void*),1)))))));}}void Cyc_Tcenv_check_rgn_resetable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*rgn);void Cyc_Tcenv_check_rgn_resetable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*rgn){Cyc_Tcenv_check_rgn_accessible(
te,loc,rgn);{struct Cyc_Tcenv_Fenv _tmp212;struct Cyc_RgnOrder_RgnPO*_tmp213;const
char*_tmp384;struct Cyc_Tcenv_Fenv*_tmp211=Cyc_Tcenv_get_fenv(te,((_tmp384="check_rgn_resetable",
_tag_dynforward(_tmp384,sizeof(char),_get_zero_arr_size_char(_tmp384,20)))));
_tmp212=*_tmp211;_tmp213=_tmp212.region_order;{void*_tmp214=Cyc_Tcutil_compress(
rgn);struct Cyc_Absyn_Tvar*_tmp215;_LL82: if(_tmp214 <= (void*)4)goto _LL84;if(*((
int*)_tmp214)!= 1)goto _LL84;_tmp215=((struct Cyc_Absyn_VarType_struct*)_tmp214)->f1;
_LL83: if(!Cyc_RgnOrder_is_region_resetable(_tmp213,_tmp215)){const char*_tmp388;
void*_tmp387[1];struct Cyc_String_pa_struct _tmp386;(_tmp386.tag=0,((_tmp386.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(rgn)),((
_tmp387[0]=& _tmp386,Cyc_Tcutil_terr(loc,((_tmp388="Region %s is not resetable",
_tag_dynforward(_tmp388,sizeof(char),_get_zero_arr_size_char(_tmp388,27)))),
_tag_dynforward(_tmp387,sizeof(void*),1)))))));}return;_LL84:;_LL85: {const char*
_tmp38B;void*_tmp38A;(_tmp38A=0,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp38B="check_rgn_resetable",
_tag_dynforward(_tmp38B,sizeof(char),_get_zero_arr_size_char(_tmp38B,20)))),
_tag_dynforward(_tmp38A,sizeof(void*),0)));}_LL81:;}}}int Cyc_Tcenv_region_outlives(
struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b);int Cyc_Tcenv_region_outlives(struct
Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){struct Cyc_Tcenv_Fenv*_tmp21B=te->le;rt_a=
Cyc_Tcutil_compress(rt_a);rt_b=Cyc_Tcutil_compress(rt_b);if(_tmp21B == 0)return
rt_a == (void*)3  || rt_a == (void*)2  && rt_b != (void*)3;{struct Cyc_Tcenv_Fenv*fe=(
struct Cyc_Tcenv_Fenv*)_tmp21B;struct Cyc_Tcenv_Fenv _tmp21D;struct Cyc_RgnOrder_RgnPO*
_tmp21E;struct Cyc_Tcenv_Fenv*_tmp21C=fe;_tmp21D=*_tmp21C;_tmp21E=_tmp21D.region_order;{
struct Cyc_Absyn_AccessEff_struct _tmp38E;struct Cyc_Absyn_AccessEff_struct*_tmp38D;
int _tmp21F=Cyc_RgnOrder_effect_outlives(_tmp21E,(void*)((_tmp38D=_cycalloc(
sizeof(*_tmp38D)),((_tmp38D[0]=((_tmp38E.tag=20,((_tmp38E.f1=(void*)rt_a,_tmp38E)))),
_tmp38D)))),rt_b);return _tmp21F;}}}struct _tuple6{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*
f3;struct Cyc_Position_Segment*f4;};void Cyc_Tcenv_check_effect_accessible(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff);void Cyc_Tcenv_check_effect_accessible(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*eff){struct Cyc_Tcenv_Fenv
_tmp224;void*_tmp225;struct Cyc_RgnOrder_RgnPO*_tmp226;struct Cyc_Tcenv_SharedFenv*
_tmp227;const char*_tmp38F;struct Cyc_Tcenv_Fenv*_tmp223=Cyc_Tcenv_get_fenv(te,((
_tmp38F="check_effect_accessible",_tag_dynforward(_tmp38F,sizeof(char),
_get_zero_arr_size_char(_tmp38F,24)))));_tmp224=*_tmp223;_tmp225=(void*)_tmp224.capability;
_tmp226=_tmp224.region_order;_tmp227=_tmp224.shared;if(Cyc_Tcutil_subset_effect(
0,eff,_tmp225))return;if(Cyc_RgnOrder_eff_outlives_eff(_tmp226,eff,_tmp225))
return;{struct _RegionHandle*frgn=_tmp227->frgn;struct _tuple6*_tmp392;struct Cyc_List_List*
_tmp391;_tmp227->delayed_effect_checks=((_tmp391=_region_malloc(frgn,sizeof(*
_tmp391)),((_tmp391->hd=((_tmp392=_region_malloc(frgn,sizeof(*_tmp392)),((
_tmp392->f1=_tmp225,((_tmp392->f2=eff,((_tmp392->f3=_tmp226,((_tmp392->f4=loc,
_tmp392)))))))))),((_tmp391->tl=_tmp227->delayed_effect_checks,_tmp391))))));}}
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_effects(
struct Cyc_Tcenv_Tenv*te){struct Cyc_Tcenv_Fenv _tmp22C;struct Cyc_Tcenv_SharedFenv*
_tmp22D;const char*_tmp393;struct Cyc_Tcenv_Fenv*_tmp22B=Cyc_Tcenv_get_fenv(te,((
_tmp393="check_delayed_effects",_tag_dynforward(_tmp393,sizeof(char),
_get_zero_arr_size_char(_tmp393,22)))));_tmp22C=*_tmp22B;_tmp22D=_tmp22C.shared;{
struct Cyc_List_List*_tmp22E=_tmp22D->delayed_effect_checks;for(0;_tmp22E != 0;
_tmp22E=_tmp22E->tl){struct _tuple6 _tmp230;void*_tmp231;void*_tmp232;struct Cyc_RgnOrder_RgnPO*
_tmp233;struct Cyc_Position_Segment*_tmp234;struct _tuple6*_tmp22F=(struct _tuple6*)
_tmp22E->hd;_tmp230=*_tmp22F;_tmp231=_tmp230.f1;_tmp232=_tmp230.f2;_tmp233=
_tmp230.f3;_tmp234=_tmp230.f4;if(Cyc_Tcutil_subset_effect(1,_tmp232,_tmp231))
continue;if(Cyc_RgnOrder_eff_outlives_eff(_tmp233,_tmp232,_tmp231))continue;{
const char*_tmp398;void*_tmp397[2];struct Cyc_String_pa_struct _tmp396;struct Cyc_String_pa_struct
_tmp395;(_tmp395.tag=0,((_tmp395.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_typ2string(_tmp232)),((_tmp396.tag=0,((_tmp396.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp231)),((
_tmp397[0]=& _tmp396,((_tmp397[1]=& _tmp395,Cyc_Tcutil_terr(_tmp234,((_tmp398="Capability \n%s\ndoes not cover function's effect\n%s",
_tag_dynforward(_tmp398,sizeof(char),_get_zero_arr_size_char(_tmp398,51)))),
_tag_dynforward(_tmp397,sizeof(void*),2)))))))))))));}}}}struct _tuple7{struct Cyc_RgnOrder_RgnPO*
f1;struct Cyc_List_List*f2;struct Cyc_Position_Segment*f3;};void Cyc_Tcenv_check_rgn_partial_order(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,struct Cyc_List_List*po);
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*po){struct Cyc_Tcenv_Fenv*_tmp239=te->le;if(_tmp239 == 0){
for(0;po != 0;po=po->tl){struct Cyc_Absyn_AccessEff_struct _tmp39B;struct Cyc_Absyn_AccessEff_struct*
_tmp39A;if(!Cyc_Tcutil_subset_effect(1,(*((struct _tuple0*)po->hd)).f1,Cyc_Absyn_empty_effect)
 || !Cyc_Tcutil_subset_effect(1,(void*)((_tmp39A=_cycalloc(sizeof(*_tmp39A)),((
_tmp39A[0]=((_tmp39B.tag=20,((_tmp39B.f1=(void*)(*((struct _tuple0*)po->hd)).f2,
_tmp39B)))),_tmp39A)))),Cyc_Absyn_empty_effect)){const char*_tmp39E;void*_tmp39D;(
_tmp39D=0,Cyc_Tcutil_terr(loc,((_tmp39E="the required region ordering is not satisfied here",
_tag_dynforward(_tmp39E,sizeof(char),_get_zero_arr_size_char(_tmp39E,51)))),
_tag_dynforward(_tmp39D,sizeof(void*),0)));}}return;}{struct Cyc_Tcenv_Fenv
_tmp23F;struct Cyc_RgnOrder_RgnPO*_tmp240;struct Cyc_Tcenv_SharedFenv*_tmp241;
struct Cyc_Tcenv_Fenv*_tmp23E=(struct Cyc_Tcenv_Fenv*)_tmp239;_tmp23F=*_tmp23E;
_tmp240=_tmp23F.region_order;_tmp241=_tmp23F.shared;if(!Cyc_RgnOrder_satisfies_constraints(
_tmp240,po,(void*)2,0)){struct _tuple7*_tmp3A1;struct Cyc_List_List*_tmp3A0;
_tmp241->delayed_constraint_checks=((_tmp3A0=_region_malloc(_tmp241->frgn,
sizeof(*_tmp3A0)),((_tmp3A0->hd=((_tmp3A1=_region_malloc(_tmp241->frgn,sizeof(*
_tmp3A1)),((_tmp3A1->f1=_tmp240,((_tmp3A1->f2=po,((_tmp3A1->f3=loc,_tmp3A1)))))))),((
_tmp3A0->tl=_tmp241->delayed_constraint_checks,_tmp3A0))))));}}}void Cyc_Tcenv_check_delayed_constraints(
struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*
te){struct Cyc_Tcenv_Fenv _tmp246;struct Cyc_Tcenv_SharedFenv*_tmp247;const char*
_tmp3A2;struct Cyc_Tcenv_Fenv*_tmp245=Cyc_Tcenv_get_fenv(te,((_tmp3A2="check_delayed_constraints",
_tag_dynforward(_tmp3A2,sizeof(char),_get_zero_arr_size_char(_tmp3A2,26)))));
_tmp246=*_tmp245;_tmp247=_tmp246.shared;{struct Cyc_List_List*_tmp248=_tmp247->delayed_constraint_checks;
for(0;_tmp248 != 0;_tmp248=_tmp248->tl){struct _tuple7 _tmp24A;struct Cyc_RgnOrder_RgnPO*
_tmp24B;struct Cyc_List_List*_tmp24C;struct Cyc_Position_Segment*_tmp24D;struct
_tuple7*_tmp249=(struct _tuple7*)_tmp248->hd;_tmp24A=*_tmp249;_tmp24B=_tmp24A.f1;
_tmp24C=_tmp24A.f2;_tmp24D=_tmp24A.f3;if(!Cyc_RgnOrder_satisfies_constraints(
_tmp24B,_tmp24C,(void*)2,1)){const char*_tmp3A5;void*_tmp3A4;(_tmp3A4=0,Cyc_Tcutil_terr(
_tmp24D,((_tmp3A5="the required region ordering is not satisfied here",
_tag_dynforward(_tmp3A5,sizeof(char),_get_zero_arr_size_char(_tmp3A5,51)))),
_tag_dynforward(_tmp3A4,sizeof(void*),0)));}}}}struct _tuple8{struct
_dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Fndecl*fd);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Fndecl*fd){struct Cyc_Tcenv_Bindings*locals=0;struct
_dynforward_ptr*_tmp3B2;const char*_tmp3B1;void*_tmp3B0[1];struct Cyc_String_pa_struct
_tmp3AF;struct Cyc_Absyn_Tvar*_tmp3AE;struct Cyc_Absyn_Tvar*rgn0=(_tmp3AE=
_cycalloc(sizeof(*_tmp3AE)),((_tmp3AE->name=((_tmp3B2=_cycalloc(sizeof(*_tmp3B2)),((
_tmp3B2[0]=(struct _dynforward_ptr)((_tmp3AF.tag=0,((_tmp3AF.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*(*fd->name).f2),((_tmp3B0[0]=& _tmp3AF,
Cyc_aprintf(((_tmp3B1="`%s",_tag_dynforward(_tmp3B1,sizeof(char),
_get_zero_arr_size_char(_tmp3B1,4)))),_tag_dynforward(_tmp3B0,sizeof(void*),1)))))))),
_tmp3B2)))),((_tmp3AE->identity=Cyc_Tcutil_new_tvar_id(),((_tmp3AE->kind=(void*)((
void*)& Cyc_Tcenv_rgn_kb),_tmp3AE)))))));struct Cyc_List_List*_tmp3B3;struct Cyc_List_List*
_tmp250=(_tmp3B3=_cycalloc(sizeof(*_tmp3B3)),((_tmp3B3->hd=rgn0,((_tmp3B3->tl=fd->tvs,
_tmp3B3)))));Cyc_Tcutil_check_unique_tvars(loc,_tmp250);{struct Cyc_RgnOrder_RgnPO*
_tmp251=Cyc_RgnOrder_initial_fn_po(r,fd->tvs,fd->rgn_po,(void*)((struct Cyc_Core_Opt*)
_check_null(fd->effect))->v,rgn0,loc);struct Cyc_Absyn_VarType_struct _tmp3B6;
struct Cyc_Absyn_VarType_struct*_tmp3B5;void*param_rgn=(void*)((_tmp3B5=_cycalloc(
sizeof(*_tmp3B5)),((_tmp3B5[0]=((_tmp3B6.tag=1,((_tmp3B6.f1=rgn0,_tmp3B6)))),
_tmp3B5))));struct Cyc_Core_Opt*_tmp3B7;struct Cyc_Core_Opt*param_rgn_opt=(_tmp3B7=
_cycalloc(sizeof(*_tmp3B7)),((_tmp3B7->v=(void*)param_rgn,_tmp3B7)));struct Cyc_List_List*
_tmp252=0;{struct Cyc_List_List*_tmp253=fd->args;for(0;_tmp253 != 0;_tmp253=
_tmp253->tl){struct Cyc_Absyn_Vardecl _tmp3C1;union Cyc_Absyn_Nmspace_union _tmp3C0;
struct _tuple1*_tmp3BF;struct Cyc_Absyn_Vardecl*_tmp3BE;struct Cyc_Absyn_Vardecl*
_tmp254=(_tmp3BE=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp3BE[0]=((
_tmp3C1.sc=(void*)((void*)2),((_tmp3C1.name=((_tmp3BF=_cycalloc(sizeof(*_tmp3BF)),((
_tmp3BF->f1=(union Cyc_Absyn_Nmspace_union)((union Cyc_Absyn_Nmspace_union)(((
_tmp3C0.Loc_n).tag=0,_tmp3C0))),((_tmp3BF->f2=(*((struct _tuple8*)_tmp253->hd)).f1,
_tmp3BF)))))),((_tmp3C1.tq=(*((struct _tuple8*)_tmp253->hd)).f2,((_tmp3C1.type=(
void*)(*((struct _tuple8*)_tmp253->hd)).f3,((_tmp3C1.initializer=0,((_tmp3C1.rgn=
param_rgn_opt,((_tmp3C1.attributes=0,((_tmp3C1.escapes=0,_tmp3C1)))))))))))))))),
_tmp3BE)));{struct Cyc_List_List _tmp3C4;struct Cyc_List_List*_tmp3C3;_tmp252=((
_tmp3C3=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp3C3[0]=((_tmp3C4.hd=
_tmp254,((_tmp3C4.tl=_tmp252,_tmp3C4)))),_tmp3C3))));}{struct Cyc_Absyn_Param_b_struct*
_tmp3CA;struct Cyc_Absyn_Param_b_struct _tmp3C9;struct Cyc_Tcenv_Bindings*_tmp3C8;
locals=((_tmp3C8=_region_malloc(r,sizeof(*_tmp3C8)),((_tmp3C8->v=(*((struct
_tuple8*)_tmp253->hd)).f1,((_tmp3C8->b=(void*)((void*)((_tmp3CA=_cycalloc(
sizeof(*_tmp3CA)),((_tmp3CA[0]=((_tmp3C9.tag=2,((_tmp3C9.f1=_tmp254,_tmp3C9)))),
_tmp3CA))))),((_tmp3C8->tl=locals,_tmp3C8))))))));}}}if(fd->cyc_varargs != 0){
struct Cyc_Core_Opt*_tmp25F;struct Cyc_Absyn_Tqual _tmp260;void*_tmp261;int _tmp262;
struct Cyc_Absyn_VarargInfo _tmp25E=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));
_tmp25F=_tmp25E.name;_tmp260=_tmp25E.tq;_tmp261=(void*)_tmp25E.type;_tmp262=
_tmp25E.inject;if(_tmp25F != 0){void*_tmp263=Cyc_Absyn_dynforward_typ(_tmp261,
param_rgn,_tmp260,Cyc_Absyn_false_conref);struct Cyc_Absyn_Vardecl _tmp3D4;union
Cyc_Absyn_Nmspace_union _tmp3D3;struct _tuple1*_tmp3D2;struct Cyc_Absyn_Vardecl*
_tmp3D1;struct Cyc_Absyn_Vardecl*_tmp264=(_tmp3D1=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)
* 1),((_tmp3D1[0]=((_tmp3D4.sc=(void*)((void*)2),((_tmp3D4.name=((_tmp3D2=
_cycalloc(sizeof(*_tmp3D2)),((_tmp3D2->f1=(union Cyc_Absyn_Nmspace_union)((union
Cyc_Absyn_Nmspace_union)(((_tmp3D3.Loc_n).tag=0,_tmp3D3))),((_tmp3D2->f2=(struct
_dynforward_ptr*)_tmp25F->v,_tmp3D2)))))),((_tmp3D4.tq=Cyc_Absyn_empty_tqual(0),((
_tmp3D4.type=(void*)_tmp263,((_tmp3D4.initializer=0,((_tmp3D4.rgn=param_rgn_opt,((
_tmp3D4.attributes=0,((_tmp3D4.escapes=0,_tmp3D4)))))))))))))))),_tmp3D1)));{
struct Cyc_List_List*_tmp3D5;_tmp252=((_tmp3D5=_cycalloc(sizeof(*_tmp3D5)),((
_tmp3D5->hd=_tmp264,((_tmp3D5->tl=_tmp252,_tmp3D5))))));}{struct Cyc_Absyn_Param_b_struct*
_tmp3DB;struct Cyc_Absyn_Param_b_struct _tmp3DA;struct Cyc_Tcenv_Bindings*_tmp3D9;
locals=((_tmp3D9=_region_malloc(r,sizeof(*_tmp3D9)),((_tmp3D9->v=(struct
_dynforward_ptr*)_tmp25F->v,((_tmp3D9->b=(void*)((void*)((_tmp3DB=_cycalloc(
sizeof(*_tmp3DB)),((_tmp3DB[0]=((_tmp3DA.tag=2,((_tmp3DA.f1=_tmp264,_tmp3DA)))),
_tmp3DB))))),((_tmp3D9->tl=locals,_tmp3D9))))))));}}else{const char*_tmp3DE;void*
_tmp3DD;(_tmp3DD=0,Cyc_Tcutil_terr(loc,((_tmp3DE="missing name for varargs",
_tag_dynforward(_tmp3DE,sizeof(char),_get_zero_arr_size_char(_tmp3DE,25)))),
_tag_dynforward(_tmp3DD,sizeof(void*),0)));}}{struct Cyc_Core_Opt _tmp3E1;struct
Cyc_Core_Opt*_tmp3E0;fd->param_vardecls=((_tmp3E0=_cycalloc(sizeof(struct Cyc_Core_Opt)
* 1),((_tmp3E0[0]=((_tmp3E1.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp252),_tmp3E1)),_tmp3E0))));}{struct Cyc_Tcenv_SharedFenv*
_tmp3FF;struct Cyc_Tcenv_CtrlEnv*_tmp3FE;struct Cyc_Absyn_JoinEff_struct*_tmp3FD;
struct Cyc_List_List*_tmp3FC;struct Cyc_List_List*_tmp3FB;struct Cyc_Absyn_AccessEff_struct
_tmp3FA;struct Cyc_Absyn_AccessEff_struct*_tmp3F9;struct Cyc_Absyn_JoinEff_struct
_tmp3F8;struct Cyc_Tcenv_Fenv*_tmp3F7;return(_tmp3F7=_region_malloc(r,sizeof(*
_tmp3F7)),((_tmp3F7->shared=(struct Cyc_Tcenv_SharedFenv*)((_tmp3FF=
_region_malloc(r,sizeof(*_tmp3FF)),((_tmp3FF->frgn=r,((_tmp3FF->return_typ=(void*)((
void*)fd->ret_type),((_tmp3FF->seen_labels=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*)))Cyc_Dict_rempty)(
r,Cyc_strptrcmp),((_tmp3FF->needed_labels=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _dynforward_ptr*,struct _dynforward_ptr*)))Cyc_Dict_rempty)(
r,Cyc_strptrcmp),((_tmp3FF->delayed_effect_checks=0,((_tmp3FF->delayed_constraint_checks=
0,_tmp3FF)))))))))))))),((_tmp3F7->type_vars=(struct Cyc_List_List*)_tmp250,((
_tmp3F7->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp251,((_tmp3F7->locals=(
struct Cyc_Tcenv_Bindings*)locals,((_tmp3F7->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((
_tmp3F7->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((_tmp3FE=_region_malloc(r,sizeof(*
_tmp3FE)),((_tmp3FE->ctrl_rgn=r,((_tmp3FE->continue_stmt=(void*)((void*)0),((
_tmp3FE->break_stmt=(void*)((void*)0),((_tmp3FE->fallthru_clause=0,((_tmp3FE->next_stmt=(
void*)((void*)2),((_tmp3FE->try_depth=0,_tmp3FE)))))))))))))),((_tmp3F7->capability=(
void*)((void*)((void*)((_tmp3FD=_cycalloc(sizeof(*_tmp3FD)),((_tmp3FD[0]=((
_tmp3F8.tag=21,((_tmp3F8.f1=((_tmp3FC=_cycalloc(sizeof(*_tmp3FC)),((_tmp3FC->hd=(
void*)((void*)((_tmp3F9=_cycalloc(sizeof(*_tmp3F9)),((_tmp3F9[0]=((_tmp3FA.tag=
20,((_tmp3FA.f1=(void*)param_rgn,_tmp3FA)))),_tmp3F9))))),((_tmp3FC->tl=((
_tmp3FB=_cycalloc(sizeof(*_tmp3FB)),((_tmp3FB->hd=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(fd->effect))->v),((_tmp3FB->tl=0,_tmp3FB)))))),_tmp3FC)))))),_tmp3F8)))),
_tmp3FD)))))),((_tmp3F7->curr_rgn=(void*)((void*)param_rgn),((_tmp3F7->in_notreadctxt=(
int)0,_tmp3F7)))))))))))))))))));}}}struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*
fd);struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){struct Cyc_Tcenv_Fenv
_tmp284;struct Cyc_Tcenv_Bindings*_tmp285;struct Cyc_RgnOrder_RgnPO*_tmp286;struct
Cyc_List_List*_tmp287;struct Cyc_Tcenv_SharedFenv*_tmp288;struct Cyc_Tcenv_Fenv*
_tmp283=old_fenv;_tmp284=*_tmp283;_tmp285=_tmp284.locals;_tmp286=_tmp284.region_order;
_tmp287=_tmp284.type_vars;_tmp288=_tmp284.shared;{struct _RegionHandle*_tmp289=
_tmp288->frgn;struct Cyc_Tcenv_Bindings*_tmp28A=(struct Cyc_Tcenv_Bindings*)
_tmp285;struct _dynforward_ptr*_tmp40C;const char*_tmp40B;void*_tmp40A[1];struct
Cyc_String_pa_struct _tmp409;struct Cyc_Absyn_Tvar*_tmp408;struct Cyc_Absyn_Tvar*
rgn0=(_tmp408=_cycalloc(sizeof(*_tmp408)),((_tmp408->name=((_tmp40C=_cycalloc(
sizeof(*_tmp40C)),((_tmp40C[0]=(struct _dynforward_ptr)((_tmp409.tag=0,((_tmp409.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*(*fd->name).f2),((_tmp40A[0]=&
_tmp409,Cyc_aprintf(((_tmp40B="`%s",_tag_dynforward(_tmp40B,sizeof(char),
_get_zero_arr_size_char(_tmp40B,4)))),_tag_dynforward(_tmp40A,sizeof(void*),1)))))))),
_tmp40C)))),((_tmp408->identity=Cyc_Tcutil_new_tvar_id(),((_tmp408->kind=(void*)((
void*)& Cyc_Tcenv_rgn_kb),_tmp408)))))));{struct Cyc_List_List*_tmp28B=fd->tvs;
for(0;_tmp28B != 0;_tmp28B=_tmp28B->tl){if(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp28B->hd)== (void*)3)_tmp286=Cyc_RgnOrder_add_youngest(_tmp289,_tmp286,(
struct Cyc_Absyn_Tvar*)_tmp28B->hd,0,0);else{if(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp28B->hd)== (void*)4  || Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp28B->hd)
== (void*)5){const char*_tmp40F;void*_tmp40E;(_tmp40E=0,((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp40F="non-intuitionistic tvar in nested_fenv",
_tag_dynforward(_tmp40F,sizeof(char),_get_zero_arr_size_char(_tmp40F,39)))),
_tag_dynforward(_tmp40E,sizeof(void*),0)));}}}}_tmp286=Cyc_RgnOrder_add_youngest(
_tmp289,_tmp286,rgn0,0,0);{struct Cyc_List_List*_tmp410;struct Cyc_List_List*
_tmp28E=(_tmp410=_cycalloc(sizeof(*_tmp410)),((_tmp410->hd=rgn0,((_tmp410->tl=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(
fd->tvs,_tmp287),_tmp410)))));Cyc_Tcutil_check_unique_tvars(loc,_tmp28E);{struct
Cyc_Absyn_VarType_struct _tmp413;struct Cyc_Absyn_VarType_struct*_tmp412;void*
param_rgn=(void*)((_tmp412=_cycalloc(sizeof(*_tmp412)),((_tmp412[0]=((_tmp413.tag=
1,((_tmp413.f1=rgn0,_tmp413)))),_tmp412))));struct Cyc_Core_Opt*_tmp414;struct Cyc_Core_Opt*
param_rgn_opt=(_tmp414=_cycalloc(sizeof(*_tmp414)),((_tmp414->v=(void*)param_rgn,
_tmp414)));struct Cyc_List_List*_tmp28F=0;{struct Cyc_List_List*_tmp290=fd->args;
for(0;_tmp290 != 0;_tmp290=_tmp290->tl){struct Cyc_Absyn_Vardecl _tmp41E;union Cyc_Absyn_Nmspace_union
_tmp41D;struct _tuple1*_tmp41C;struct Cyc_Absyn_Vardecl*_tmp41B;struct Cyc_Absyn_Vardecl*
_tmp291=(_tmp41B=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp41B[0]=((
_tmp41E.sc=(void*)((void*)2),((_tmp41E.name=((_tmp41C=_cycalloc(sizeof(*_tmp41C)),((
_tmp41C->f1=(union Cyc_Absyn_Nmspace_union)((union Cyc_Absyn_Nmspace_union)(((
_tmp41D.Loc_n).tag=0,_tmp41D))),((_tmp41C->f2=(*((struct _tuple8*)_tmp290->hd)).f1,
_tmp41C)))))),((_tmp41E.tq=(*((struct _tuple8*)_tmp290->hd)).f2,((_tmp41E.type=(
void*)(*((struct _tuple8*)_tmp290->hd)).f3,((_tmp41E.initializer=0,((_tmp41E.rgn=
param_rgn_opt,((_tmp41E.attributes=0,((_tmp41E.escapes=0,_tmp41E)))))))))))))))),
_tmp41B)));{struct Cyc_List_List _tmp421;struct Cyc_List_List*_tmp420;_tmp28F=((
_tmp420=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp420[0]=((_tmp421.hd=
_tmp291,((_tmp421.tl=_tmp28F,_tmp421)))),_tmp420))));}{struct Cyc_Absyn_Param_b_struct*
_tmp427;struct Cyc_Absyn_Param_b_struct _tmp426;struct Cyc_Tcenv_Bindings*_tmp425;
_tmp28A=(struct Cyc_Tcenv_Bindings*)((_tmp425=_region_malloc(_tmp289,sizeof(*
_tmp425)),((_tmp425->v=(*((struct _tuple8*)_tmp290->hd)).f1,((_tmp425->b=(void*)((
void*)((_tmp427=_cycalloc(sizeof(*_tmp427)),((_tmp427[0]=((_tmp426.tag=2,((
_tmp426.f1=_tmp291,_tmp426)))),_tmp427))))),((_tmp425->tl=_tmp28A,_tmp425))))))));}}}
if(fd->cyc_varargs != 0){struct Cyc_Core_Opt*_tmp29C;struct Cyc_Absyn_Tqual _tmp29D;
void*_tmp29E;int _tmp29F;struct Cyc_Absyn_VarargInfo _tmp29B=*((struct Cyc_Absyn_VarargInfo*)
_check_null(fd->cyc_varargs));_tmp29C=_tmp29B.name;_tmp29D=_tmp29B.tq;_tmp29E=(
void*)_tmp29B.type;_tmp29F=_tmp29B.inject;if(_tmp29C != 0){void*_tmp2A0=Cyc_Absyn_dynforward_typ(
_tmp29E,param_rgn,_tmp29D,Cyc_Absyn_false_conref);struct Cyc_Absyn_Vardecl _tmp431;
union Cyc_Absyn_Nmspace_union _tmp430;struct _tuple1*_tmp42F;struct Cyc_Absyn_Vardecl*
_tmp42E;struct Cyc_Absyn_Vardecl*_tmp2A1=(_tmp42E=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)
* 1),((_tmp42E[0]=((_tmp431.sc=(void*)((void*)2),((_tmp431.name=((_tmp42F=
_cycalloc(sizeof(*_tmp42F)),((_tmp42F->f1=(union Cyc_Absyn_Nmspace_union)((union
Cyc_Absyn_Nmspace_union)(((_tmp430.Loc_n).tag=0,_tmp430))),((_tmp42F->f2=(struct
_dynforward_ptr*)_tmp29C->v,_tmp42F)))))),((_tmp431.tq=Cyc_Absyn_empty_tqual(0),((
_tmp431.type=(void*)_tmp2A0,((_tmp431.initializer=0,((_tmp431.rgn=param_rgn_opt,((
_tmp431.attributes=0,((_tmp431.escapes=0,_tmp431)))))))))))))))),_tmp42E)));{
struct Cyc_List_List*_tmp432;_tmp28F=((_tmp432=_cycalloc(sizeof(*_tmp432)),((
_tmp432->hd=_tmp2A1,((_tmp432->tl=_tmp28F,_tmp432))))));}{struct Cyc_Absyn_Param_b_struct*
_tmp438;struct Cyc_Absyn_Param_b_struct _tmp437;struct Cyc_Tcenv_Bindings*_tmp436;
_tmp28A=(struct Cyc_Tcenv_Bindings*)((_tmp436=_region_malloc(_tmp289,sizeof(*
_tmp436)),((_tmp436->v=(struct _dynforward_ptr*)_tmp29C->v,((_tmp436->b=(void*)((
void*)((_tmp438=_cycalloc(sizeof(*_tmp438)),((_tmp438[0]=((_tmp437.tag=2,((
_tmp437.f1=_tmp2A1,_tmp437)))),_tmp438))))),((_tmp436->tl=_tmp28A,_tmp436))))))));}}
else{const char*_tmp43B;void*_tmp43A;(_tmp43A=0,Cyc_Tcutil_terr(loc,((_tmp43B="missing name for varargs",
_tag_dynforward(_tmp43B,sizeof(char),_get_zero_arr_size_char(_tmp43B,25)))),
_tag_dynforward(_tmp43A,sizeof(void*),0)));}}{struct Cyc_Core_Opt _tmp43E;struct
Cyc_Core_Opt*_tmp43D;fd->param_vardecls=((_tmp43D=_cycalloc(sizeof(struct Cyc_Core_Opt)
* 1),((_tmp43D[0]=((_tmp43E.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp28F),_tmp43E)),_tmp43D))));}{struct Cyc_Tcenv_SharedFenv*
_tmp45C;struct Cyc_Tcenv_CtrlEnv*_tmp45B;struct Cyc_Absyn_JoinEff_struct*_tmp45A;
struct Cyc_List_List*_tmp459;struct Cyc_List_List*_tmp458;struct Cyc_Absyn_AccessEff_struct
_tmp457;struct Cyc_Absyn_AccessEff_struct*_tmp456;struct Cyc_Absyn_JoinEff_struct
_tmp455;struct Cyc_Tcenv_Fenv*_tmp454;return(struct Cyc_Tcenv_Fenv*)((_tmp454=
_region_malloc(_tmp289,sizeof(*_tmp454)),((_tmp454->shared=(struct Cyc_Tcenv_SharedFenv*)((
_tmp45C=_region_malloc(_tmp289,sizeof(*_tmp45C)),((_tmp45C->frgn=_tmp289,((
_tmp45C->return_typ=(void*)((void*)fd->ret_type),((_tmp45C->seen_labels=((struct
Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dynforward_ptr*,struct
_dynforward_ptr*)))Cyc_Dict_rempty)(_tmp289,Cyc_strptrcmp),((_tmp45C->needed_labels=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dynforward_ptr*,
struct _dynforward_ptr*)))Cyc_Dict_rempty)(_tmp289,Cyc_strptrcmp),((_tmp45C->delayed_effect_checks=
0,((_tmp45C->delayed_constraint_checks=0,_tmp45C)))))))))))))),((_tmp454->type_vars=(
struct Cyc_List_List*)_tmp28E,((_tmp454->region_order=(struct Cyc_RgnOrder_RgnPO*)
_tmp286,((_tmp454->locals=(struct Cyc_Tcenv_Bindings*)_tmp28A,((_tmp454->encloser=(
struct Cyc_Absyn_Stmt*)fd->body,((_tmp454->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)((
struct Cyc_Tcenv_CtrlEnv*)((_tmp45B=_region_malloc(_tmp289,sizeof(*_tmp45B)),((
_tmp45B->ctrl_rgn=_tmp289,((_tmp45B->continue_stmt=(void*)((void*)0),((_tmp45B->break_stmt=(
void*)((void*)0),((_tmp45B->fallthru_clause=0,((_tmp45B->next_stmt=(void*)((void*)
2),((_tmp45B->try_depth=0,_tmp45B))))))))))))))),((_tmp454->capability=(void*)((
void*)((void*)((_tmp45A=_cycalloc(sizeof(*_tmp45A)),((_tmp45A[0]=((_tmp455.tag=
21,((_tmp455.f1=((_tmp459=_cycalloc(sizeof(*_tmp459)),((_tmp459->hd=(void*)((
void*)((_tmp456=_cycalloc(sizeof(*_tmp456)),((_tmp456[0]=((_tmp457.tag=20,((
_tmp457.f1=(void*)param_rgn,_tmp457)))),_tmp456))))),((_tmp459->tl=((_tmp458=
_cycalloc(sizeof(*_tmp458)),((_tmp458->hd=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(fd->effect))->v),((_tmp458->tl=0,_tmp458)))))),_tmp459)))))),_tmp455)))),
_tmp45A)))))),((_tmp454->curr_rgn=(void*)((void*)param_rgn),((_tmp454->in_notreadctxt=(
int)0,_tmp454))))))))))))))))))));}}}}}
