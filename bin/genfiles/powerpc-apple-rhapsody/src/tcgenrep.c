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
void*v;};void*Cyc_Core_identity(void*);extern char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dynforward_ptr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[
16];int isalnum(int);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct
Cyc_List_List*Cyc_List_list(struct _dynforward_ptr);int Cyc_List_length(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];void*Cyc_List_fold_left(void*(*f)(void*,
void*),void*accum,struct Cyc_List_List*x);void*Cyc_List_fold_right_c(void*(*f)(
void*,void*,void*),void*,struct Cyc_List_List*x,void*accum);struct Cyc_List_List*
Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_flatten(struct Cyc_List_List*x);extern char Cyc_List_Nth[8];int Cyc_List_forall(
int(*pred)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_zip(struct
Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_zip3(struct
Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z);struct _tuple0{
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple0 Cyc_List_split(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_tabulate(int n,void*(*f)(int));
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);struct
Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct
Cyc_Iter_Iter,void*);struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct
Cyc_String_pa_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
int Cyc_printf(struct _dynforward_ptr,struct _dynforward_ptr);extern char Cyc_FileCloseError[
19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{char*tag;
struct _dynforward_ptr f1;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,
void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];
extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,
void*));struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict d,void*k);void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict
d);struct _tuple1{void*f1;void*f2;};struct _tuple1*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple1*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_Dict_Dict Cyc_Dict_delete(struct
Cyc_Dict_Dict,void*);unsigned long Cyc_strlen(struct _dynforward_ptr s);int Cyc_strcmp(
struct _dynforward_ptr s1,struct _dynforward_ptr s2);struct _dynforward_ptr Cyc_strconcat(
struct _dynforward_ptr,struct _dynforward_ptr);struct _dynforward_ptr Cyc_strdup(
struct _dynforward_ptr src);struct Cyc_Hashtable_Table;struct Cyc_Hashtable_Table*
Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));void Cyc_Hashtable_insert(
struct Cyc_Hashtable_Table*t,void*key,void*val);void*Cyc_Hashtable_lookup(struct
Cyc_Hashtable_Table*t,void*key);struct Cyc_Lineno_Pos{struct _dynforward_ptr
logical_file;struct _dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(
int,int);struct Cyc_Position_Error{struct _dynforward_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dynforward_ptr desc;};extern char Cyc_Position_Nocontext[14];
struct Cyc_Typerep_Int_struct{int tag;int f1;unsigned int f2;};struct Cyc_Typerep_ThinPtr_struct{
int tag;unsigned int f1;void*f2;};struct Cyc_Typerep_FatPtr_struct{int tag;void*f1;}
;struct _tuple2{unsigned int f1;struct _dynforward_ptr f2;void*f3;};struct Cyc_Typerep_Struct_struct{
int tag;struct _dynforward_ptr*f1;unsigned int f2;struct _dynforward_ptr f3;};struct
_tuple3{unsigned int f1;void*f2;};struct Cyc_Typerep_Tuple_struct{int tag;
unsigned int f1;struct _dynforward_ptr f2;};struct _tuple4{unsigned int f1;struct
_dynforward_ptr f2;};struct Cyc_Typerep_TUnion_struct{int tag;struct _dynforward_ptr
f1;struct _dynforward_ptr f2;struct _dynforward_ptr f3;};struct Cyc_Typerep_TUnionField_struct{
int tag;struct _dynforward_ptr f1;struct _dynforward_ptr f2;unsigned int f3;struct
_dynforward_ptr f4;};struct _tuple5{struct _dynforward_ptr f1;void*f2;};struct Cyc_Typerep_XTUnion_struct{
int tag;struct _dynforward_ptr f1;struct _dynforward_ptr f2;};struct Cyc_Typerep_Union_struct{
int tag;struct _dynforward_ptr*f1;int f2;struct _dynforward_ptr f3;};struct Cyc_Typerep_Enum_struct{
int tag;struct _dynforward_ptr*f1;int f2;struct _dynforward_ptr f3;};unsigned int Cyc_Typerep_size_type(
void*rep);struct Cyc_timespec{long tv_sec;int tv_nsec;};struct Cyc_tm{int tm_sec;int
tm_min;int tm_hour;int tm_mday;int tm_mon;int tm_year;int tm_wday;int tm_yday;int
tm_isdst;long tm_gmtoff;char*tm_zone;};struct Cyc_Absyn_Loc_n_struct{int tag;};
struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct
Loc_n;struct Cyc_Absyn_Rel_n_struct Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};
struct _tuple6{union Cyc_Absyn_Nmspace_union f1;struct _dynforward_ptr*f2;};struct
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
_tuple6*name;int is_xtunion;int is_flat;};struct Cyc_Absyn_UnknownTunion_struct{int
tag;struct Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int
tag;struct Cyc_Absyn_Tuniondecl**f1;};union Cyc_Absyn_TunionInfoU_union{struct Cyc_Absyn_UnknownTunion_struct
UnknownTunion;struct Cyc_Absyn_KnownTunion_struct KnownTunion;};struct Cyc_Absyn_TunionInfo{
union Cyc_Absyn_TunionInfoU_union tunion_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple6*tunion_name;struct
_tuple6*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{int
tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};union Cyc_Absyn_TunionFieldInfoU_union{
struct Cyc_Absyn_UnknownTunionfield_struct UnknownTunionfield;struct Cyc_Absyn_KnownTunionfield_struct
KnownTunionfield;};struct Cyc_Absyn_TunionFieldInfo{union Cyc_Absyn_TunionFieldInfoU_union
field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{int tag;
void*f1;struct _tuple6*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct Cyc_Absyn_Aggrdecl**
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
int tag;struct _tuple6*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;
};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple6*
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
_tuple6*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple6*f1;
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
struct Cyc_List_List*f1;};struct _tuple7{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple7*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple6*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
f2;struct Cyc_Absyn_Tunionfield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct
_tuple6*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{
int tag;struct _tuple6*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;
void*r;struct Cyc_Position_Segment*loc;void*annot;};struct Cyc_Absyn_Exp_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple8{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple8 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dynforward_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple8 f2;struct _tuple8 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dynforward_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple8 f2;
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
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple6*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple6*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*
topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple6*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple6*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dynforward_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;};struct Cyc_Absyn_Aggrdecl{void*
kind;void*sc;struct _tuple6*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple6*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple6*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;int is_flat;};struct Cyc_Absyn_Enumfield{struct _tuple6*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple6*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple6*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _dynforward_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple6*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple6*,struct _tuple6*);void*Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*
x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref;extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
void*Cyc_Absyn_const_string_typ(void*rgn);void*Cyc_Absyn_star_typ(void*t,void*
rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_at_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_dynforward_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst_union,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple6*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct
Cyc_Absyn_Exp*,int user_cast,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple6*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU_union info);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct
Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dynforward_ptr Cyc_Absynpp_typ2string(void*);struct
_dynforward_ptr Cyc_Absynpp_typ2cstring(void*);struct _tuple9{unsigned int f1;int f2;
};struct _tuple9 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Set_Set;
extern char Cyc_Set_Absent[11];struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*
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
_dynforward_ptr ap);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_typecmp(void*,
void*);void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,void*);struct Cyc_Tcgenrep_RepInfo{
struct Cyc_List_List*decls;struct Cyc_Absyn_Exp*exp;struct Cyc_List_List*
dependencies;struct Cyc_Core_Opt*fwd_decl;int emitted;int is_extern;};struct Cyc_Dict_Dict
Cyc_Tcgenrep_empty_typerep_dict();struct Cyc_Dict_Dict Cyc_Tcgenrep_empty_typerep_dict(){
return((struct Cyc_Dict_Dict(*)(int(*cmp)(void*,void*)))Cyc_Dict_empty)(Cyc_Tcutil_typecmp);}
void Cyc_Tcgenrep_print_dict_entry(void*type,struct Cyc_Tcgenrep_RepInfo*info);
void Cyc_Tcgenrep_print_dict_entry(void*type,struct Cyc_Tcgenrep_RepInfo*info){{
const char*_tmp28C;void*_tmp28B[3];struct Cyc_String_pa_struct _tmp28A;struct Cyc_Int_pa_struct
_tmp289;struct Cyc_Int_pa_struct _tmp288;(_tmp288.tag=1,((_tmp288.f1=(
unsigned long)info->emitted,((_tmp289.tag=1,((_tmp289.f1=(unsigned int)info,((
_tmp28A.tag=0,((_tmp28A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
type)),((_tmp28B[0]=& _tmp28A,((_tmp28B[1]=& _tmp289,((_tmp28B[2]=& _tmp288,Cyc_printf(((
_tmp28C="(%s,%x:%d,",_tag_dynforward(_tmp28C,sizeof(char),
_get_zero_arr_size_char(_tmp28C,11)))),_tag_dynforward(_tmp28B,sizeof(void*),3)))))))))))))))))));}{
struct Cyc_List_List*p=info->dependencies;for(0;p != 0;p=p->tl){{const char*_tmp290;
void*_tmp28F[1];struct Cyc_Int_pa_struct _tmp28E;(_tmp28E.tag=1,((_tmp28E.f1=(
unsigned int)((struct Cyc_Tcgenrep_RepInfo*)p->hd),((_tmp28F[0]=& _tmp28E,Cyc_printf(((
_tmp290="%x",_tag_dynforward(_tmp290,sizeof(char),_get_zero_arr_size_char(
_tmp290,3)))),_tag_dynforward(_tmp28F,sizeof(void*),1)))))));}if(p->tl != 0){
const char*_tmp293;void*_tmp292;(_tmp292=0,Cyc_printf(((_tmp293=",",
_tag_dynforward(_tmp293,sizeof(char),_get_zero_arr_size_char(_tmp293,2)))),
_tag_dynforward(_tmp292,sizeof(void*),0)));}}}{const char*_tmp296;void*_tmp295;(
_tmp295=0,Cyc_printf(((_tmp296=")\n",_tag_dynforward(_tmp296,sizeof(char),
_get_zero_arr_size_char(_tmp296,3)))),_tag_dynforward(_tmp295,sizeof(void*),0)));}}
void Cyc_Tcgenrep_print_typerep_dict(struct Cyc_Dict_Dict dict);void Cyc_Tcgenrep_print_typerep_dict(
struct Cyc_Dict_Dict dict){((void(*)(void(*f)(void*,struct Cyc_Tcgenrep_RepInfo*),
struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_Tcgenrep_print_dict_entry,dict);}static
int Cyc_Tcgenrep_rephash(struct Cyc_Tcgenrep_RepInfo*ri);static int Cyc_Tcgenrep_rephash(
struct Cyc_Tcgenrep_RepInfo*ri){return(int)ri;}static int Cyc_Tcgenrep_repcmp(
struct Cyc_Tcgenrep_RepInfo*r1,struct Cyc_Tcgenrep_RepInfo*r2);static int Cyc_Tcgenrep_repcmp(
struct Cyc_Tcgenrep_RepInfo*r1,struct Cyc_Tcgenrep_RepInfo*r2){unsigned int r1p;
unsigned int r2p;r1p=(unsigned int)r1;r2p=(unsigned int)r2;if(r1 < r2)return - 1;
else{if(r1 == r2)return 0;else{return 1;}}}static struct Cyc_Absyn_Tunionfield*Cyc_Tcgenrep_getField(
struct Cyc_Absyn_Tuniondecl*td,struct _tuple6*fieldname);static struct Cyc_Absyn_Tunionfield*
Cyc_Tcgenrep_getField(struct Cyc_Absyn_Tuniondecl*td,struct _tuple6*fieldname){if(
td->fields == 0){const char*_tmp299;void*_tmp298;(_tmp298=0,((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp299="Could not find field in tuniondecl",
_tag_dynforward(_tmp299,sizeof(char),_get_zero_arr_size_char(_tmp299,35)))),
_tag_dynforward(_tmp298,sizeof(void*),0)));}else{struct Cyc_List_List*l=(struct
Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;for(0;l != 0;l=l->tl){
if(!Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Tunionfield*)l->hd)->name,fieldname))
return(struct Cyc_Absyn_Tunionfield*)l->hd;}}{const char*_tmp29C;void*_tmp29B;(
_tmp29B=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmp29C="Could not find field in tuniondecl",_tag_dynforward(_tmp29C,sizeof(char),
_get_zero_arr_size_char(_tmp29C,35)))),_tag_dynforward(_tmp29B,sizeof(void*),0)));}}
static char _tmp10[8]="Typerep";static struct _dynforward_ptr Cyc_Tcgenrep_typerep_nm={
_tmp10,_tmp10 + 8};static struct Cyc_List_List Cyc_Tcgenrep_l2={(void*)& Cyc_Tcgenrep_typerep_nm,
0};static struct Cyc_List_List*Cyc_Tcgenrep_dfsvisit(struct Cyc_Tcgenrep_RepInfo*ri,
struct Cyc_Hashtable_Table*visited);static struct Cyc_List_List*Cyc_Tcgenrep_dfsvisit(
struct Cyc_Tcgenrep_RepInfo*ri,struct Cyc_Hashtable_Table*visited){struct
_handler_cons _tmp11;_push_handler(& _tmp11);{int _tmp13=0;if(setjmp(_tmp11.handler))
_tmp13=1;if(!_tmp13){((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Tcgenrep_RepInfo*
key))Cyc_Hashtable_lookup)(visited,ri);{struct Cyc_List_List*_tmp14=0;
_npop_handler(0);return _tmp14;};_pop_handler();}else{void*_tmp12=(void*)
_exn_thrown;void*_tmp16=_tmp12;_LL1: if(_tmp16 != Cyc_Core_Not_found)goto _LL3;_LL2:((
void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Tcgenrep_RepInfo*key,int val))Cyc_Hashtable_insert)(
visited,ri,1);{struct Cyc_List_List*ds=0;{struct Cyc_List_List*l=ri->dependencies;
for(0;l != 0;l=l->tl){ds=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_imp_append)(ds,Cyc_Tcgenrep_dfsvisit((struct Cyc_Tcgenrep_RepInfo*)
l->hd,visited));}}if(ri->emitted == 0){if(ri->fwd_decl != 0){struct Cyc_Absyn_Decl*
_tmp29F[1];struct Cyc_List_List*_tmp29E;ds=((_tmp29E=_cycalloc(sizeof(*_tmp29E)),((
_tmp29E->hd=((_tmp29F[0]=(struct Cyc_Absyn_Decl*)((struct Cyc_Core_Opt*)
_check_null(ri->fwd_decl))->v,((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp29F,sizeof(struct Cyc_Absyn_Decl*),1)))),((
_tmp29E->tl=ds,_tmp29E))))));}{struct Cyc_List_List*_tmp2A0;struct Cyc_List_List*
_tmp19=(_tmp2A0=_cycalloc(sizeof(*_tmp2A0)),((_tmp2A0->hd=ri->decls,((_tmp2A0->tl=
0,_tmp2A0)))));ri->emitted=1;return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(ds,_tmp19);}}else{return ds;}}_LL3:;
_LL4:(void)_throw(_tmp16);_LL0:;}}}static struct Cyc_List_List*Cyc_Tcgenrep_dfs(
struct Cyc_Tcgenrep_RepInfo*ri);static struct Cyc_List_List*Cyc_Tcgenrep_dfs(struct
Cyc_Tcgenrep_RepInfo*ri){struct Cyc_Hashtable_Table*tab=((struct Cyc_Hashtable_Table*(*)(
int sz,int(*cmp)(struct Cyc_Tcgenrep_RepInfo*,struct Cyc_Tcgenrep_RepInfo*),int(*
hash)(struct Cyc_Tcgenrep_RepInfo*)))Cyc_Hashtable_create)(53,Cyc_Tcgenrep_repcmp,
Cyc_Tcgenrep_rephash);struct Cyc_List_List*ds=Cyc_Tcgenrep_dfsvisit(ri,tab);
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_flatten)(ds);}
static char _tmp1B[11]="Typestruct";static struct _dynforward_ptr Cyc_Tcgenrep_typestruct_str={
_tmp1B,_tmp1B + 11};static char _tmp1C[4]="Var";static struct _dynforward_ptr Cyc_Tcgenrep_var_str={
_tmp1C,_tmp1C + 4};static char _tmp1D[4]="Int";static struct _dynforward_ptr Cyc_Tcgenrep_int_str={
_tmp1D,_tmp1D + 4};static char _tmp1E[6]="Float";static struct _dynforward_ptr Cyc_Tcgenrep_float_str={
_tmp1E,_tmp1E + 6};static char _tmp1F[7]="Double";static struct _dynforward_ptr Cyc_Tcgenrep_double_str={
_tmp1F,_tmp1F + 7};static char _tmp20[8]="ThinPtr";static struct _dynforward_ptr Cyc_Tcgenrep_thinptr_str={
_tmp20,_tmp20 + 8};static char _tmp21[7]="FatPtr";static struct _dynforward_ptr Cyc_Tcgenrep_fatptr_str={
_tmp21,_tmp21 + 7};static char _tmp22[6]="Tuple";static struct _dynforward_ptr Cyc_Tcgenrep_tuple_str={
_tmp22,_tmp22 + 6};static char _tmp23[12]="TUnionField";static struct _dynforward_ptr
Cyc_Tcgenrep_tunionfield_str={_tmp23,_tmp23 + 12};static char _tmp24[7]="Struct";
static struct _dynforward_ptr Cyc_Tcgenrep_struct_str={_tmp24,_tmp24 + 7};static char
_tmp25[7]="TUnion";static struct _dynforward_ptr Cyc_Tcgenrep_tunion_str={_tmp25,
_tmp25 + 7};static char _tmp26[8]="XTUnion";static struct _dynforward_ptr Cyc_Tcgenrep_xtunion_str={
_tmp26,_tmp26 + 8};static char _tmp27[6]="Union";static struct _dynforward_ptr Cyc_Tcgenrep_union_str={
_tmp27,_tmp27 + 6};static char _tmp28[5]="Enum";static struct _dynforward_ptr Cyc_Tcgenrep_enum_str={
_tmp28,_tmp28 + 5};static char _tmp29[7]="name_t";static struct _dynforward_ptr Cyc_Tcgenrep_name_t_str={
_tmp29,_tmp29 + 7};static struct _tuple6*Cyc_Tcgenrep_typerep_name(struct
_dynforward_ptr*name);static struct _tuple6*Cyc_Tcgenrep_typerep_name(struct
_dynforward_ptr*name){union Cyc_Absyn_Nmspace_union _tmp2A3;struct _tuple6*_tmp2A2;
return(_tmp2A2=_cycalloc(sizeof(*_tmp2A2)),((_tmp2A2->f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp2A3.Abs_n).tag=2,(((_tmp2A3.Abs_n).f1=(struct Cyc_List_List*)& Cyc_Tcgenrep_l2,
_tmp2A3)))),((_tmp2A2->f2=name,_tmp2A2)))));}static int Cyc_Tcgenrep_gen_id_counter=
0;static struct _dynforward_ptr*Cyc_Tcgenrep_new_gen_id(struct _dynforward_ptr name);
static struct _dynforward_ptr*Cyc_Tcgenrep_new_gen_id(struct _dynforward_ptr name){
struct Cyc_Int_pa_struct _tmp2AD;struct Cyc_String_pa_struct _tmp2AC;void*_tmp2AB[2];
const char*_tmp2AA;struct _dynforward_ptr*_tmp2A9;return(_tmp2A9=_cycalloc(sizeof(*
_tmp2A9)),((_tmp2A9[0]=(struct _dynforward_ptr)((_tmp2AD.tag=1,((_tmp2AD.f1=(
unsigned long)Cyc_Tcgenrep_gen_id_counter ++,((_tmp2AC.tag=0,((_tmp2AC.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)name),((_tmp2AB[0]=& _tmp2AC,((_tmp2AB[1]=&
_tmp2AD,Cyc_aprintf(((_tmp2AA="_gen%s_%d",_tag_dynforward(_tmp2AA,sizeof(char),
_get_zero_arr_size_char(_tmp2AA,10)))),_tag_dynforward(_tmp2AB,sizeof(void*),2)))))))))))))),
_tmp2A9)));}static void Cyc_Tcgenrep_print_params(struct Cyc_List_List*l);static
void Cyc_Tcgenrep_print_params(struct Cyc_List_List*l){{const char*_tmp2B0;void*
_tmp2AF;(_tmp2AF=0,Cyc_printf(((_tmp2B0="<",_tag_dynforward(_tmp2B0,sizeof(char),
_get_zero_arr_size_char(_tmp2B0,2)))),_tag_dynforward(_tmp2AF,sizeof(void*),0)));}{
struct Cyc_List_List*p=l;for(0;p != 0;p=p->tl){const char*_tmp2B5;void*_tmp2B4[2];
struct Cyc_String_pa_struct _tmp2B3;struct Cyc_Int_pa_struct _tmp2B2;(_tmp2B2.tag=1,((
_tmp2B2.f1=(unsigned long)((int)(p->tl != 0?',':' ')),((_tmp2B3.tag=0,((_tmp2B3.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)p->hd)),((
_tmp2B4[0]=& _tmp2B3,((_tmp2B4[1]=& _tmp2B2,Cyc_printf(((_tmp2B5="%s%c",
_tag_dynforward(_tmp2B5,sizeof(char),_get_zero_arr_size_char(_tmp2B5,5)))),
_tag_dynforward(_tmp2B4,sizeof(void*),2)))))))))))));}}{const char*_tmp2B8;void*
_tmp2B7;(_tmp2B7=0,Cyc_printf(((_tmp2B8=">\n",_tag_dynforward(_tmp2B8,sizeof(
char),_get_zero_arr_size_char(_tmp2B8,3)))),_tag_dynforward(_tmp2B7,sizeof(void*),
0)));}}static void Cyc_Tcgenrep_print_tvars(struct Cyc_List_List*l);static void Cyc_Tcgenrep_print_tvars(
struct Cyc_List_List*l){{const char*_tmp2BB;void*_tmp2BA;(_tmp2BA=0,Cyc_printf(((
_tmp2BB="<",_tag_dynforward(_tmp2BB,sizeof(char),_get_zero_arr_size_char(_tmp2BB,
2)))),_tag_dynforward(_tmp2BA,sizeof(void*),0)));}{struct Cyc_List_List*p=l;for(0;
p != 0;p=p->tl){const char*_tmp2C0;void*_tmp2BF[2];struct Cyc_String_pa_struct
_tmp2BE;struct Cyc_Int_pa_struct _tmp2BD;(_tmp2BD.tag=1,((_tmp2BD.f1=(
unsigned long)((int)(p->tl != 0?',':' ')),((_tmp2BE.tag=0,((_tmp2BE.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*((struct Cyc_Absyn_Tvar*)p->hd)->name),((
_tmp2BF[0]=& _tmp2BE,((_tmp2BF[1]=& _tmp2BD,Cyc_printf(((_tmp2C0="%s%c",
_tag_dynforward(_tmp2C0,sizeof(char),_get_zero_arr_size_char(_tmp2C0,5)))),
_tag_dynforward(_tmp2BF,sizeof(void*),2)))))))))))));}}{const char*_tmp2C3;void*
_tmp2C2;(_tmp2C2=0,Cyc_printf(((_tmp2C3=">\n",_tag_dynforward(_tmp2C3,sizeof(
char),_get_zero_arr_size_char(_tmp2C3,3)))),_tag_dynforward(_tmp2C2,sizeof(void*),
0)));}}static struct _tuple6*Cyc_Tcgenrep_toplevel_name(struct _dynforward_ptr*name);
static struct _tuple6*Cyc_Tcgenrep_toplevel_name(struct _dynforward_ptr*name){union
Cyc_Absyn_Nmspace_union _tmp2C6;struct _tuple6*_tmp2C5;return(_tmp2C5=_cycalloc(
sizeof(*_tmp2C5)),((_tmp2C5->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp2C6.Rel_n).tag=
1,(((_tmp2C6.Rel_n).f1=0,_tmp2C6)))),((_tmp2C5->f2=(struct _dynforward_ptr*)name,
_tmp2C5)))));}static struct _tuple6*Cyc_Tcgenrep_relative_name(struct Cyc_List_List*
pathl,struct _dynforward_ptr*name);static struct _tuple6*Cyc_Tcgenrep_relative_name(
struct Cyc_List_List*pathl,struct _dynforward_ptr*name){union Cyc_Absyn_Nmspace_union
_tmp2C9;struct _tuple6*_tmp2C8;return(_tmp2C8=_cycalloc(sizeof(*_tmp2C8)),((
_tmp2C8->f1=(union Cyc_Absyn_Nmspace_union)(((_tmp2C9.Rel_n).tag=1,(((_tmp2C9.Rel_n).f1=
pathl,_tmp2C9)))),((_tmp2C8->f2=(struct _dynforward_ptr*)name,_tmp2C8)))));}
static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_gen_decl(struct _dynforward_ptr*name,void*
type,struct Cyc_Absyn_Exp*init,struct Cyc_Position_Segment*seg);static struct Cyc_Absyn_Decl*
Cyc_Tcgenrep_gen_decl(struct _dynforward_ptr*name,void*type,struct Cyc_Absyn_Exp*
init,struct Cyc_Position_Segment*seg){struct _tuple6*qvar=Cyc_Tcgenrep_toplevel_name(
name);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(qvar,type,init);(void*)(
vd->sc=(void*)((void*)0));{struct Cyc_Absyn_Var_d_struct _tmp2CC;struct Cyc_Absyn_Var_d_struct*
_tmp2CB;struct Cyc_Absyn_Var_d_struct*r1=(_tmp2CB=_cycalloc(sizeof(*_tmp2CB)),((
_tmp2CB[0]=((_tmp2CC.tag=0,((_tmp2CC.f1=vd,_tmp2CC)))),_tmp2CB)));void*r2=(void*)
r1;return Cyc_Absyn_new_decl(r2,seg);}}static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_gen_vardecl(
struct _tuple6*name,void*type,struct Cyc_Absyn_Exp*init,void*sc,struct Cyc_Position_Segment*
seg);static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_gen_vardecl(struct _tuple6*name,void*
type,struct Cyc_Absyn_Exp*init,void*sc,struct Cyc_Position_Segment*seg){struct
_tuple6*topname=Cyc_Tcgenrep_toplevel_name((*name).f2);struct Cyc_Absyn_Vardecl*
vd=Cyc_Absyn_new_vardecl(topname,type,init);(void*)(vd->sc=(void*)sc);{struct Cyc_Absyn_Var_d_struct
_tmp2CF;struct Cyc_Absyn_Var_d_struct*_tmp2CE;struct Cyc_Absyn_Var_d_struct*r1=(
_tmp2CE=_cycalloc(sizeof(*_tmp2CE)),((_tmp2CE[0]=((_tmp2CF.tag=0,((_tmp2CF.f1=vd,
_tmp2CF)))),_tmp2CE)));void*r2=(void*)r1;return Cyc_Absyn_new_decl(r2,seg);}}
static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_string(struct _dynforward_ptr s,struct
Cyc_Position_Segment*seg);static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_string(
struct _dynforward_ptr s,struct Cyc_Position_Segment*seg){union Cyc_Absyn_Cnst_union
_tmp2D0;return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)((union Cyc_Absyn_Cnst_union)(((
_tmp2D0.String_c).tag=5,(((_tmp2D0.String_c).f1=s,_tmp2D0))))),seg);}static
struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_string_cls(struct Cyc_Position_Segment*seg,
struct _dynforward_ptr*s);static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_string_cls(
struct Cyc_Position_Segment*seg,struct _dynforward_ptr*s){union Cyc_Absyn_Cnst_union
_tmp2D1;return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)((union Cyc_Absyn_Cnst_union)(((
_tmp2D1.String_c).tag=5,(((_tmp2D1.String_c).f1=*s,_tmp2D1))))),seg);}static
struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_qvar_string_cls(struct Cyc_Position_Segment*
seg,struct _tuple6*s);static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_qvar_string_cls(
struct Cyc_Position_Segment*seg,struct _tuple6*s){union Cyc_Absyn_Cnst_union _tmp2D2;
return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)((union Cyc_Absyn_Cnst_union)(((
_tmp2D2.String_c).tag=5,(((_tmp2D2.String_c).f1=*(*s).f2,_tmp2D2))))),seg);}
static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_int(int i,struct Cyc_Position_Segment*
seg);static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_cnst_int(int i,struct Cyc_Position_Segment*
seg){union Cyc_Absyn_Cnst_union _tmp2D3;return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)((
union Cyc_Absyn_Cnst_union)(((_tmp2D3.Int_c).tag=2,(((_tmp2D3.Int_c).f1=(void*)((
void*)0),(((_tmp2D3.Int_c).f2=i,_tmp2D3))))))),seg);}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_cnst_int_cls(struct Cyc_Position_Segment*seg,int i);static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_cnst_int_cls(struct Cyc_Position_Segment*seg,int i){union Cyc_Absyn_Cnst_union
_tmp2D4;return Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)((union Cyc_Absyn_Cnst_union)(((
_tmp2D4.Int_c).tag=2,(((_tmp2D4.Int_c).f1=(void*)((void*)0),(((_tmp2D4.Int_c).f2=
i,_tmp2D4))))))),seg);}static int Cyc_Tcgenrep_size_of2int(void*sz);static int Cyc_Tcgenrep_size_of2int(
void*sz){void*_tmp4E=sz;_LL6: if((int)_tmp4E != 0)goto _LL8;_LL7: return 8;_LL8: if((
int)_tmp4E != 1)goto _LLA;_LL9: return 16;_LLA: if((int)_tmp4E != 2)goto _LLC;_LLB:
return 32;_LLC: if((int)_tmp4E != 3)goto _LLE;_LLD: return 32;_LLE: if((int)_tmp4E != 4)
goto _LL5;_LLF: return 64;_LL5:;}static void*Cyc_Tcgenrep_tunion_typ(struct _tuple6*
name);static void*Cyc_Tcgenrep_tunion_typ(struct _tuple6*name){struct Cyc_Absyn_TunionType_struct
_tmp2E7;union Cyc_Absyn_TunionInfoU_union _tmp2E6;struct Cyc_Absyn_UnknownTunionInfo
_tmp2E5;struct Cyc_Core_Opt*_tmp2E4;struct Cyc_Absyn_TunionInfo _tmp2E3;struct Cyc_Absyn_TunionType_struct*
_tmp2E2;return(void*)((_tmp2E2=_cycalloc(sizeof(*_tmp2E2)),((_tmp2E2[0]=((
_tmp2E7.tag=2,((_tmp2E7.f1=((_tmp2E3.tunion_info=(union Cyc_Absyn_TunionInfoU_union)(((
_tmp2E6.UnknownTunion).tag=0,(((_tmp2E6.UnknownTunion).f1=((_tmp2E5.name=name,((
_tmp2E5.is_xtunion=0,((_tmp2E5.is_flat=0,_tmp2E5)))))),_tmp2E6)))),((_tmp2E3.targs=
0,((_tmp2E3.rgn=((_tmp2E4=_cycalloc(sizeof(*_tmp2E4)),((_tmp2E4->v=(void*)((void*)
2),_tmp2E4)))),_tmp2E3)))))),_tmp2E7)))),_tmp2E2))));}static void*Cyc_Tcgenrep_tunionfield_typ(
struct _tuple6*name,struct _tuple6*fieldname);static void*Cyc_Tcgenrep_tunionfield_typ(
struct _tuple6*name,struct _tuple6*fieldname){struct Cyc_Absyn_TunionFieldType_struct
_tmp2F6;union Cyc_Absyn_TunionFieldInfoU_union _tmp2F5;struct Cyc_Absyn_UnknownTunionFieldInfo
_tmp2F4;struct Cyc_Absyn_TunionFieldInfo _tmp2F3;struct Cyc_Absyn_TunionFieldType_struct*
_tmp2F2;return(void*)((_tmp2F2=_cycalloc(sizeof(*_tmp2F2)),((_tmp2F2[0]=((
_tmp2F6.tag=3,((_tmp2F6.f1=((_tmp2F3.field_info=(union Cyc_Absyn_TunionFieldInfoU_union)(((
_tmp2F5.UnknownTunionfield).tag=0,(((_tmp2F5.UnknownTunionfield).f1=((_tmp2F4.tunion_name=
name,((_tmp2F4.field_name=fieldname,((_tmp2F4.is_xtunion=0,_tmp2F4)))))),_tmp2F5)))),((
_tmp2F3.targs=0,_tmp2F3)))),_tmp2F6)))),_tmp2F2))));}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_call_exp(struct _tuple6*name,struct Cyc_List_List*args,struct Cyc_Position_Segment*
loc);static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_call_exp(struct _tuple6*name,struct
Cyc_List_List*args,struct Cyc_Position_Segment*loc){return Cyc_Absyn_unknowncall_exp(
Cyc_Absyn_unknownid_exp(name,loc),args,loc);}static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_tunion_constructor_decl(
struct _tuple6*tunionname,struct _tuple6*fieldname,struct _tuple6*varname,struct Cyc_List_List*
args,void*sc,struct Cyc_Position_Segment*loc);static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_tunion_constructor_decl(
struct _tuple6*tunionname,struct _tuple6*fieldname,struct _tuple6*varname,struct Cyc_List_List*
args,void*sc,struct Cyc_Position_Segment*loc){void*_tmp5A=Cyc_Tcgenrep_tunionfield_typ(
tunionname,fieldname);struct Cyc_Absyn_Exp*_tmp5B=Cyc_Tcgenrep_call_exp(fieldname,
args,loc);struct Cyc_Absyn_Decl*_tmp5C=Cyc_Tcgenrep_gen_vardecl(varname,_tmp5A,(
struct Cyc_Absyn_Exp*)_tmp5B,sc,loc);return _tmp5C;}struct _tuple10{struct Cyc_Absyn_Tqual
f1;void*f2;};static void*Cyc_Tcgenrep_get_second(struct _tuple10*pair);static void*
Cyc_Tcgenrep_get_second(struct _tuple10*pair){return(*pair).f2;}struct _tuple11{
struct _dynforward_ptr*f1;struct Cyc_Absyn_Exp*f2;};static struct _dynforward_ptr*
Cyc_Tcgenrep_get_first(struct _tuple11*pair);static struct _dynforward_ptr*Cyc_Tcgenrep_get_first(
struct _tuple11*pair){return(*pair).f1;}static char _tmp5D[5]="list";static struct
_dynforward_ptr Cyc_Tcgenrep_list_str={_tmp5D,_tmp5D + 5};static char _tmp5E[5]="List";
static struct _dynforward_ptr Cyc_Tcgenrep_List_str={_tmp5E,_tmp5E + 5};struct
_tuple12{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_tuple2_exp_cls(struct Cyc_Position_Segment*loc,struct _tuple12*es);
static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_tuple2_exp_cls(struct Cyc_Position_Segment*
loc,struct _tuple12*es){struct _tuple12 _tmp60;struct Cyc_Absyn_Exp*_tmp61;struct Cyc_Absyn_Exp*
_tmp62;struct _tuple12*_tmp5F=es;_tmp60=*_tmp5F;_tmp61=_tmp60.f1;_tmp62=_tmp60.f2;{
struct Cyc_Absyn_Exp*_tmp2F7[2];return Cyc_Absyn_tuple_exp(((_tmp2F7[1]=_tmp62,((
_tmp2F7[0]=_tmp61,((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp2F7,sizeof(struct Cyc_Absyn_Exp*),2)))))),loc);}}struct
_tuple13{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_tuple3_exp_cls(struct Cyc_Position_Segment*
loc,struct _tuple13*es);static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_tuple3_exp_cls(
struct Cyc_Position_Segment*loc,struct _tuple13*es){struct _tuple13 _tmp65;struct Cyc_Absyn_Exp*
_tmp66;struct Cyc_Absyn_Exp*_tmp67;struct Cyc_Absyn_Exp*_tmp68;struct _tuple13*
_tmp64=es;_tmp65=*_tmp64;_tmp66=_tmp65.f1;_tmp67=_tmp65.f2;_tmp68=_tmp65.f3;{
struct Cyc_Absyn_Exp*_tmp2F8[3];return Cyc_Absyn_tuple_exp(((_tmp2F8[2]=_tmp68,((
_tmp2F8[1]=_tmp67,((_tmp2F8[0]=_tmp66,((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp2F8,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),loc);}}static char _tmp6A[5]="NULL";static struct _dynforward_ptr Cyc_Tcgenrep_null_str={
_tmp6A,_tmp6A + 5};static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_list_exp(struct Cyc_List_List*
l,struct Cyc_Position_Segment*loc);static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_list_exp(
struct Cyc_List_List*l,struct Cyc_Position_Segment*loc){if(l == 0)return Cyc_Absyn_null_exp(
loc);return Cyc_Tcgenrep_call_exp(Cyc_Tcgenrep_toplevel_name(& Cyc_Tcgenrep_list_str),
l,loc);}struct _tuple14{void*f1;struct Cyc_Position_Segment*f2;};static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_make_offsetof_exp(struct _tuple14*typeloc,int index);static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_make_offsetof_exp(struct _tuple14*typeloc,int index){struct _tuple14
_tmp6C;void*_tmp6D;struct Cyc_Position_Segment*_tmp6E;struct _tuple14*_tmp6B=
typeloc;_tmp6C=*_tmp6B;_tmp6D=_tmp6C.f1;_tmp6E=_tmp6C.f2;{struct Cyc_Absyn_TupleIndex_struct
_tmp2FB;struct Cyc_Absyn_TupleIndex_struct*_tmp2FA;return Cyc_Absyn_offsetof_exp(
_tmp6D,(void*)((_tmp2FA=_cycalloc_atomic(sizeof(*_tmp2FA)),((_tmp2FA[0]=((
_tmp2FB.tag=1,((_tmp2FB.f1=(unsigned int)index,_tmp2FB)))),_tmp2FA)))),_tmp6E);}}
static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_get_and_cast_ri_exp(struct Cyc_Position_Segment*
loc,struct Cyc_Tcgenrep_RepInfo*info);static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_get_and_cast_ri_exp(
struct Cyc_Position_Segment*loc,struct Cyc_Tcgenrep_RepInfo*info){return Cyc_Absyn_cast_exp(
Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str)),(
struct Cyc_Absyn_Exp*)_check_null(info->exp),1,(void*)0,loc);}static struct
_tuple11*Cyc_Tcgenrep_gen_id_for_exp(struct Cyc_Absyn_Exp*e);static struct _tuple11*
Cyc_Tcgenrep_gen_id_for_exp(struct Cyc_Absyn_Exp*e){const char*_tmp2FE;struct
_tuple11*_tmp2FD;return(_tmp2FD=_cycalloc(sizeof(*_tmp2FD)),((_tmp2FD->f1=Cyc_Tcgenrep_new_gen_id(((
_tmp2FE="tuple",_tag_dynforward(_tmp2FE,sizeof(char),_get_zero_arr_size_char(
_tmp2FE,6))))),((_tmp2FD->f2=e,_tmp2FD)))));}static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_gen_decl_cls(
struct _tuple14*env,struct _tuple11*name_exp);static struct Cyc_Absyn_Decl*Cyc_Tcgenrep_gen_decl_cls(
struct _tuple14*env,struct _tuple11*name_exp){struct _tuple14 _tmp74;void*_tmp75;
struct Cyc_Position_Segment*_tmp76;struct _tuple14*_tmp73=env;_tmp74=*_tmp73;
_tmp75=_tmp74.f1;_tmp76=_tmp74.f2;{struct _tuple11 _tmp78;struct _dynforward_ptr*
_tmp79;struct Cyc_Absyn_Exp*_tmp7A;struct _tuple11*_tmp77=name_exp;_tmp78=*_tmp77;
_tmp79=_tmp78.f1;_tmp7A=_tmp78.f2;return Cyc_Tcgenrep_gen_decl(_tmp79,_tmp75,(
struct Cyc_Absyn_Exp*)_tmp7A,_tmp76);}}struct _tuple15{struct Cyc_List_List*f1;
struct Cyc_Absyn_Exp*f2;};static struct _tuple15*Cyc_Tcgenrep_null_designator_exp(
struct Cyc_Absyn_Exp*e);static struct _tuple15*Cyc_Tcgenrep_null_designator_exp(
struct Cyc_Absyn_Exp*e){struct _tuple15*_tmp2FF;return(_tmp2FF=_cycalloc(sizeof(*
_tmp2FF)),((_tmp2FF->f1=0,((_tmp2FF->f2=e,_tmp2FF)))));}static struct Cyc_Absyn_Exp*
Cyc_Tcgenrep_arr_init_exp(struct Cyc_List_List*l,struct Cyc_Position_Segment*loc);
static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_arr_init_exp(struct Cyc_List_List*l,struct
Cyc_Position_Segment*loc){struct Cyc_List_List*_tmp7C=((struct Cyc_List_List*(*)(
struct _tuple15*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_null_designator_exp,l);return Cyc_Absyn_unresolvedmem_exp(0,_tmp7C,
loc);}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_address_exp_cls(struct Cyc_Position_Segment*
loc,struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_address_exp_cls(
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_address_exp(
e,loc);}static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_unknownid_exp_cls(struct Cyc_Position_Segment*
loc,struct _tuple6*e);static struct Cyc_Absyn_Exp*Cyc_Tcgenrep_unknownid_exp_cls(
struct Cyc_Position_Segment*loc,struct _tuple6*e){return Cyc_Absyn_unknownid_exp(e,
loc);}static int Cyc_Tcgenrep_has_bitfield(struct Cyc_Absyn_Aggrfield*sf);static int
Cyc_Tcgenrep_has_bitfield(struct Cyc_Absyn_Aggrfield*sf){return sf->width != 0;}
static int Cyc_Tcgenrep_add_bitfield_sizes(int total,struct Cyc_Absyn_Aggrfield*sf);
static int Cyc_Tcgenrep_add_bitfield_sizes(int total,struct Cyc_Absyn_Aggrfield*sf){
unsigned int _tmp7E;int _tmp7F;struct _tuple9 _tmp7D=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(sf->width));_tmp7E=_tmp7D.f1;_tmp7F=_tmp7D.f2;
if(!_tmp7F){const char*_tmp302;void*_tmp301;(_tmp301=0,((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp302="add_bitfield_sizes: sizeof or offsetof in bitfield size",
_tag_dynforward(_tmp302,sizeof(char),_get_zero_arr_size_char(_tmp302,56)))),
_tag_dynforward(_tmp301,sizeof(void*),0)));}return(int)(_tmp7E + total);}static
void*Cyc_Tcgenrep_select_structfield_type(struct Cyc_Absyn_Aggrfield*sf);static
void*Cyc_Tcgenrep_select_structfield_type(struct Cyc_Absyn_Aggrfield*sf){{const
char*_tmp303;if(Cyc_strcmp((struct _dynforward_ptr)*sf->name,((_tmp303="",
_tag_dynforward(_tmp303,sizeof(char),_get_zero_arr_size_char(_tmp303,1)))))== 0){
const char*_tmp306;void*_tmp305;(_tmp305=0,((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp306="gen(): anonymous (padding) structfield not handled yet",
_tag_dynforward(_tmp306,sizeof(char),_get_zero_arr_size_char(_tmp306,55)))),
_tag_dynforward(_tmp305,sizeof(void*),0)));}}if(Cyc_Tcgenrep_has_bitfield(sf)){
const char*_tmp309;void*_tmp308;(_tmp308=0,((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp309="gen(): mixed struct bitfields and union bitfields not handled",
_tag_dynforward(_tmp309,sizeof(char),_get_zero_arr_size_char(_tmp309,62)))),
_tag_dynforward(_tmp308,sizeof(void*),0)));}return(void*)sf->type;}struct
_tuple16{struct _dynforward_ptr*f1;void*f2;};static struct _tuple16*Cyc_Tcgenrep_select_structfield_nmtype(
struct Cyc_Absyn_Aggrfield*sf);static struct _tuple16*Cyc_Tcgenrep_select_structfield_nmtype(
struct Cyc_Absyn_Aggrfield*sf){{const char*_tmp30A;if(Cyc_strcmp((struct
_dynforward_ptr)*sf->name,((_tmp30A="",_tag_dynforward(_tmp30A,sizeof(char),
_get_zero_arr_size_char(_tmp30A,1)))))== 0){const char*_tmp30D;void*_tmp30C;(
_tmp30C=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmp30D="gen(): anonymous (padding) structfield not handled yet",_tag_dynforward(
_tmp30D,sizeof(char),_get_zero_arr_size_char(_tmp30D,55)))),_tag_dynforward(
_tmp30C,sizeof(void*),0)));}}if(Cyc_Tcgenrep_has_bitfield(sf)){const char*_tmp310;
void*_tmp30F;(_tmp30F=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr
ap))Cyc_Tcutil_impos)(((_tmp310="gen(): mixed struct bitfields and union bitfields not handled",
_tag_dynforward(_tmp310,sizeof(char),_get_zero_arr_size_char(_tmp310,62)))),
_tag_dynforward(_tmp30F,sizeof(void*),0)));}{struct _tuple16*_tmp311;return(
_tmp311=_cycalloc(sizeof(*_tmp311)),((_tmp311->f1=sf->name,((_tmp311->f2=(void*)
sf->type,_tmp311)))));}}struct _tuple17{int f1;struct _tuple6*f2;};struct _tuple17*
Cyc_Tcgenrep_select_enumfield_tagnm(struct Cyc_Absyn_Enumfield*ef);struct _tuple17*
Cyc_Tcgenrep_select_enumfield_tagnm(struct Cyc_Absyn_Enumfield*ef){if(ef->tag == 0){
const char*_tmp314;void*_tmp313;(_tmp313=0,((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp314="Enum tag exp should be filled in by now",
_tag_dynforward(_tmp314,sizeof(char),_get_zero_arr_size_char(_tmp314,40)))),
_tag_dynforward(_tmp313,sizeof(void*),0)));}{struct _tuple17*_tmp315;return(
_tmp315=_cycalloc(sizeof(*_tmp315)),((_tmp315->f1=(int)(Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(ef->tag))).f1,((_tmp315->f2=ef->name,_tmp315)))));}}
static struct Cyc_Dict_Dict Cyc_Tcgenrep_update_info(struct Cyc_Dict_Dict dict,void*
type,struct Cyc_List_List*decls,struct Cyc_Absyn_Exp*exp,struct Cyc_List_List*
dependencies,struct Cyc_Core_Opt*fwd_decl);static struct Cyc_Dict_Dict Cyc_Tcgenrep_update_info(
struct Cyc_Dict_Dict dict,void*type,struct Cyc_List_List*decls,struct Cyc_Absyn_Exp*
exp,struct Cyc_List_List*dependencies,struct Cyc_Core_Opt*fwd_decl){struct Cyc_Tcgenrep_RepInfo**
_tmp90=((struct Cyc_Tcgenrep_RepInfo**(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup_opt)(
dict,type);if(_tmp90 != 0){if((*_tmp90)->decls != 0){Cyc_Tcgenrep_print_typerep_dict(
dict);{const char*_tmp318;void*_tmp317;(_tmp317=0,((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp318="Updating non-forward declaration",
_tag_dynforward(_tmp318,sizeof(char),_get_zero_arr_size_char(_tmp318,33)))),
_tag_dynforward(_tmp317,sizeof(void*),0)));}}(*_tmp90)->decls=decls;(*_tmp90)->exp=
exp;(*_tmp90)->dependencies=dependencies;return dict;}else{struct Cyc_Tcgenrep_RepInfo*
_tmp319;return((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Tcgenrep_RepInfo*
v))Cyc_Dict_insert)(dict,type,((_tmp319=_cycalloc(sizeof(*_tmp319)),((_tmp319->decls=
decls,((_tmp319->exp=exp,((_tmp319->dependencies=dependencies,((_tmp319->fwd_decl=
fwd_decl,((_tmp319->emitted=0,((_tmp319->is_extern=0,_tmp319)))))))))))))));}}
static struct Cyc_Dict_Dict Cyc_Tcgenrep_make_fwd_decl_info(struct Cyc_Dict_Dict dict,
void*type,struct Cyc_Absyn_Exp*exp,struct Cyc_Core_Opt*fwd_decl,int is_extern);
static struct Cyc_Dict_Dict Cyc_Tcgenrep_make_fwd_decl_info(struct Cyc_Dict_Dict dict,
void*type,struct Cyc_Absyn_Exp*exp,struct Cyc_Core_Opt*fwd_decl,int is_extern){
struct Cyc_Tcgenrep_RepInfo**_tmp94=((struct Cyc_Tcgenrep_RepInfo**(*)(struct Cyc_Dict_Dict
d,void*k))Cyc_Dict_lookup_opt)(dict,type);if(_tmp94 != 0){{const char*_tmp31D;void*
_tmp31C[1];struct Cyc_String_pa_struct _tmp31B;(_tmp31B.tag=0,((_tmp31B.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(type)),((_tmp31C[
0]=& _tmp31B,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmp31D="Repinfo for fwd declaration of %s already exists",_tag_dynforward(
_tmp31D,sizeof(char),_get_zero_arr_size_char(_tmp31D,49)))),_tag_dynforward(
_tmp31C,sizeof(void*),1)))))));}return dict;}else{struct Cyc_Tcgenrep_RepInfo*
_tmp31E;return((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Tcgenrep_RepInfo*
v))Cyc_Dict_insert)(dict,type,((_tmp31E=_cycalloc(sizeof(*_tmp31E)),((_tmp31E->decls=
0,((_tmp31E->exp=exp,((_tmp31E->dependencies=0,((_tmp31E->fwd_decl=fwd_decl,((
_tmp31E->emitted=0,((_tmp31E->is_extern=is_extern,_tmp31E)))))))))))))));}}
static struct Cyc_Absyn_Tqual Cyc_Tcgenrep_tq_none={0,0,0,0,0};static struct _tuple10*
Cyc_Tcgenrep_tqual_type(struct Cyc_Absyn_Tqual*tq,void*type);static struct _tuple10*
Cyc_Tcgenrep_tqual_type(struct Cyc_Absyn_Tqual*tq,void*type){struct _tuple10*
_tmp31F;return(_tmp31F=_cycalloc(sizeof(*_tmp31F)),((_tmp31F->f1=*((struct Cyc_Absyn_Tqual*)
_check_null(tq)),((_tmp31F->f2=type,_tmp31F)))));}static void*Cyc_Tcgenrep_tuple_typ(
struct Cyc_List_List*types);static void*Cyc_Tcgenrep_tuple_typ(struct Cyc_List_List*
types){struct Cyc_List_List*_tmp9A=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(
struct Cyc_Absyn_Tqual*,void*),struct Cyc_Absyn_Tqual*env,struct Cyc_List_List*x))
Cyc_List_map_c)(Cyc_Tcgenrep_tqual_type,(struct Cyc_Absyn_Tqual*)& Cyc_Tcgenrep_tq_none,
types);struct Cyc_Absyn_TupleType_struct _tmp322;struct Cyc_Absyn_TupleType_struct*
_tmp321;struct Cyc_Absyn_TupleType_struct*tuple_type_base=(_tmp321=_cycalloc(
sizeof(*_tmp321)),((_tmp321[0]=((_tmp322.tag=9,((_tmp322.f1=_tmp9A,_tmp322)))),
_tmp321)));void*tuple_type=(void*)tuple_type_base;return tuple_type;}static struct
_tuple15*Cyc_Tcgenrep_array_decls(void*type,struct Cyc_List_List*exps,struct Cyc_Position_Segment*
loc);static struct _tuple15*Cyc_Tcgenrep_array_decls(void*type,struct Cyc_List_List*
exps,struct Cyc_Position_Segment*loc){struct Cyc_List_List*_tmp9D=((struct Cyc_List_List*(*)(
struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_gen_id_for_exp,exps);struct _tuple14*_tmp323;struct Cyc_List_List*
_tmp9E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(struct _tuple14*,
struct _tuple11*),struct _tuple14*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_gen_decl_cls,((
_tmp323=_cycalloc(sizeof(*_tmp323)),((_tmp323->f1=type,((_tmp323->f2=loc,_tmp323)))))),
_tmp9D);struct Cyc_List_List*_tmp9F=((struct Cyc_List_List*(*)(struct
_dynforward_ptr*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_get_first,
_tmp9D);struct Cyc_List_List*_tmpA0=((struct Cyc_List_List*(*)(struct _tuple6*(*f)(
struct _dynforward_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_toplevel_name,
_tmp9F);struct Cyc_List_List*_tmpA1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple6*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_unknownid_exp_cls,loc,_tmpA0);
struct Cyc_List_List*_tmpA2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,struct Cyc_Absyn_Exp*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_address_exp_cls,loc,_tmpA1);
struct Cyc_Absyn_Exp*_tmpA3=Cyc_Tcgenrep_arr_init_exp(_tmpA2,loc);const char*
_tmp324;struct _dynforward_ptr*_tmpA4=Cyc_Tcgenrep_new_gen_id(((_tmp324="arr",
_tag_dynforward(_tmp324,sizeof(char),_get_zero_arr_size_char(_tmp324,4)))));void*
_tmpA5=Cyc_Absyn_at_typ(type,(void*)2,Cyc_Tcgenrep_tq_none,Cyc_Absyn_false_conref);
void*_tmpA6=Cyc_Absyn_array_typ(_tmpA5,Cyc_Tcgenrep_tq_none,0,Cyc_Absyn_false_conref,
0);struct Cyc_Absyn_Decl*_tmpA7=Cyc_Tcgenrep_gen_decl(_tmpA4,_tmpA6,(struct Cyc_Absyn_Exp*)
_tmpA3,loc);struct Cyc_Absyn_Exp*_tmpA8=Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_toplevel_name(
_tmpA4),loc);struct Cyc_Absyn_Decl*_tmp327[1];struct _tuple15*_tmp326;return(
_tmp326=_cycalloc(sizeof(*_tmp326)),((_tmp326->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9E,((
_tmp327[0]=_tmpA7,((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp327,sizeof(struct Cyc_Absyn_Decl*),1))))),((_tmp326->f2=
_tmpA8,_tmp326)))));}static void*Cyc_Tcgenrep_check_tunionfield_and_get_type(
struct Cyc_Absyn_Tunionfield*tuf);static void*Cyc_Tcgenrep_check_tunionfield_and_get_type(
struct Cyc_Absyn_Tunionfield*tuf){struct Cyc_Absyn_TupleType_struct _tmp331;struct
_tuple10*_tmp330;struct _tuple10*_tmp32F[1];struct Cyc_Absyn_TupleType_struct*
_tmp32E;return(void*)((_tmp32E=_cycalloc(sizeof(*_tmp32E)),((_tmp32E[0]=((
_tmp331.tag=9,((_tmp331.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(((_tmp32F[0]=((_tmp330=_cycalloc(
sizeof(*_tmp330)),((_tmp330->f1=Cyc_Tcgenrep_tq_none,((_tmp330->f2=Cyc_Absyn_uint_typ,
_tmp330)))))),((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp32F,sizeof(struct _tuple10*),1)))),tuf->typs),_tmp331)))),
_tmp32E))));}static struct _tuple6*Cyc_Tcgenrep_check_tunionfield_and_get_name(
struct Cyc_Absyn_Tunionfield*tuf);static struct _tuple6*Cyc_Tcgenrep_check_tunionfield_and_get_name(
struct Cyc_Absyn_Tunionfield*tuf){return tuf->name;}struct _tuple18{struct _tuple6*
f1;void*f2;};static struct _tuple18*Cyc_Tcgenrep_check_tunionfield_and_get_nmtype(
struct Cyc_Absyn_Tunionfield*tuf);static struct _tuple18*Cyc_Tcgenrep_check_tunionfield_and_get_nmtype(
struct Cyc_Absyn_Tunionfield*tuf){struct Cyc_Absyn_TupleType_struct*_tmp340;struct
_tuple10*_tmp33F[1];struct _tuple10*_tmp33E;struct Cyc_Absyn_TupleType_struct
_tmp33D;struct _tuple18*_tmp33C;return(_tmp33C=_cycalloc(sizeof(*_tmp33C)),((
_tmp33C->f1=tuf->name,((_tmp33C->f2=(void*)((_tmp340=_cycalloc(sizeof(*_tmp340)),((
_tmp340[0]=((_tmp33D.tag=9,((_tmp33D.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(((_tmp33F[0]=((_tmp33E=_cycalloc(
sizeof(*_tmp33E)),((_tmp33E->f1=Cyc_Tcgenrep_tq_none,((_tmp33E->f2=Cyc_Absyn_uint_typ,
_tmp33E)))))),((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp33F,sizeof(struct _tuple10*),1)))),tuf->typs),_tmp33D)))),
_tmp340)))),_tmp33C)))));}static struct _tuple16*Cyc_Tcgenrep_check_xtunionfield_and_get_name_type(
struct Cyc_Absyn_Tunionfield*tuf);static struct _tuple16*Cyc_Tcgenrep_check_xtunionfield_and_get_name_type(
struct Cyc_Absyn_Tunionfield*tuf){struct Cyc_Absyn_TupleType_struct*_tmp34F;struct
_tuple10*_tmp34E[1];struct _tuple10*_tmp34D;struct Cyc_Absyn_TupleType_struct
_tmp34C;struct _tuple16*_tmp34B;return(_tmp34B=_cycalloc(sizeof(*_tmp34B)),((
_tmp34B->f1=(*tuf->name).f2,((_tmp34B->f2=(void*)((_tmp34F=_cycalloc(sizeof(*
_tmp34F)),((_tmp34F[0]=((_tmp34C.tag=9,((_tmp34C.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(((_tmp34E[0]=((
_tmp34D=_cycalloc(sizeof(*_tmp34D)),((_tmp34D->f1=Cyc_Tcgenrep_tq_none,((_tmp34D->f2=
Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)2,Cyc_Tcgenrep_tq_none,Cyc_Absyn_true_conref),
_tmp34D)))))),((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp34E,sizeof(struct _tuple10*),1)))),tuf->typs),_tmp34C)))),
_tmp34F)))),_tmp34B)))));}static int Cyc_Tcgenrep_filter_empty_tunionfield(struct
Cyc_Absyn_Tunionfield*tuf);static int Cyc_Tcgenrep_filter_empty_tunionfield(struct
Cyc_Absyn_Tunionfield*tuf){return tuf->typs != 0;}static int Cyc_Tcgenrep_filter_nonempty_tunionfield(
struct Cyc_Absyn_Tunionfield*tuf);static int Cyc_Tcgenrep_filter_nonempty_tunionfield(
struct Cyc_Absyn_Tunionfield*tuf){return tuf->typs == 0;}static struct
_dynforward_ptr*Cyc_Tcgenrep_get_tunionfield_name(struct Cyc_Absyn_Tunionfield*
tuf);static struct _dynforward_ptr*Cyc_Tcgenrep_get_tunionfield_name(struct Cyc_Absyn_Tunionfield*
tuf){return(*tuf->name).f2;}struct Cyc_Absyn_Aggrfield*Cyc_Tcgenrep_substitute_structfield_type(
struct Cyc_List_List*subst,struct Cyc_Absyn_Aggrfield*sf);struct Cyc_Absyn_Aggrfield*
Cyc_Tcgenrep_substitute_structfield_type(struct Cyc_List_List*subst,struct Cyc_Absyn_Aggrfield*
sf){struct Cyc_Absyn_Aggrfield*_tmp350;return(_tmp350=_cycalloc(sizeof(*_tmp350)),((
_tmp350->name=sf->name,((_tmp350->tq=sf->tq,((_tmp350->type=(void*)Cyc_Tcutil_substitute(
subst,(void*)sf->type),((_tmp350->width=sf->width,((_tmp350->attributes=sf->attributes,
_tmp350)))))))))));}struct _tuple10*Cyc_Tcgenrep_substitute_tqual_type(struct Cyc_List_List*
subst,struct _tuple10*pair);struct _tuple10*Cyc_Tcgenrep_substitute_tqual_type(
struct Cyc_List_List*subst,struct _tuple10*pair){struct _tuple10 _tmpBD;struct Cyc_Absyn_Tqual
_tmpBE;void*_tmpBF;struct _tuple10*_tmpBC=pair;_tmpBD=*_tmpBC;_tmpBE=_tmpBD.f1;
_tmpBF=_tmpBD.f2;{struct _tuple10*_tmp351;return(_tmp351=_cycalloc(sizeof(*
_tmp351)),((_tmp351->f1=_tmpBE,((_tmp351->f2=Cyc_Tcutil_substitute(subst,_tmpBF),
_tmp351)))));}}struct Cyc_Absyn_Tunionfield*Cyc_Tcgenrep_substitute_tunionfield_type(
struct Cyc_List_List*subst,struct Cyc_Absyn_Tunionfield*tf);struct Cyc_Absyn_Tunionfield*
Cyc_Tcgenrep_substitute_tunionfield_type(struct Cyc_List_List*subst,struct Cyc_Absyn_Tunionfield*
tf){struct Cyc_Absyn_Tunionfield*_tmp352;return(_tmp352=_cycalloc(sizeof(*_tmp352)),((
_tmp352->name=tf->name,((_tmp352->typs=((struct Cyc_List_List*(*)(struct _tuple10*(*
f)(struct Cyc_List_List*,struct _tuple10*),struct Cyc_List_List*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_substitute_tqual_type,subst,tf->typs),((_tmp352->loc=
tf->loc,((_tmp352->sc=(void*)((void*)tf->sc),_tmp352)))))))));}void*Cyc_Tcgenrep_monomorphize_type(
void*type);void*Cyc_Tcgenrep_monomorphize_type(void*type){void*_tmpC2=Cyc_Tcutil_compress(
type);struct Cyc_Absyn_AggrInfo _tmpC3;union Cyc_Absyn_AggrInfoU_union _tmpC4;struct
Cyc_List_List*_tmpC5;struct Cyc_Absyn_TunionInfo _tmpC6;union Cyc_Absyn_TunionInfoU_union
_tmpC7;struct Cyc_Absyn_Tuniondecl**_tmpC8;struct Cyc_Absyn_Tuniondecl*_tmpC9;
struct Cyc_List_List*_tmpCA;struct Cyc_Core_Opt*_tmpCB;_LL11: if(_tmpC2 <= (void*)4)
goto _LL15;if(*((int*)_tmpC2)!= 10)goto _LL13;_tmpC3=((struct Cyc_Absyn_AggrType_struct*)
_tmpC2)->f1;_tmpC4=_tmpC3.aggr_info;_tmpC5=_tmpC3.targs;_LL12: {struct Cyc_Absyn_Aggrdecl*
_tmpCC=Cyc_Absyn_get_known_aggrdecl(_tmpC4);struct Cyc_List_List*_tmpCD=_tmpCC->tvs;
if(Cyc_List_length(_tmpC5)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpCD)){const char*_tmp357;void*_tmp356[2];struct Cyc_Int_pa_struct _tmp355;struct
Cyc_Int_pa_struct _tmp354;(_tmp354.tag=1,((_tmp354.f1=(unsigned long)((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmpCD),((_tmp355.tag=1,((_tmp355.f1=(
unsigned long)Cyc_List_length(_tmpC5),((_tmp356[0]=& _tmp355,((_tmp356[1]=&
_tmp354,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmp357="gen(): number of params %d differs from number of tyvars %d",
_tag_dynforward(_tmp357,sizeof(char),_get_zero_arr_size_char(_tmp357,60)))),
_tag_dynforward(_tmp356,sizeof(void*),2)))))))))))));}{struct Cyc_List_List*
_tmpD2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(_tmpCD,_tmpC5);struct Cyc_List_List*fields=0;if(_tmpCC->impl != 0)
fields=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_substitute_structfield_type,_tmpD2,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpCC->impl))->fields);{struct Cyc_Absyn_AggrdeclImpl*_tmp358;struct
Cyc_Absyn_AggrdeclImpl*_tmpD3=_tmpCC->impl == 0?0:((_tmp358=_cycalloc(sizeof(*
_tmp358)),((_tmp358->exist_vars=0,((_tmp358->rgn_po=0,((_tmp358->fields=fields,
_tmp358))))))));struct Cyc_Absyn_Aggrdecl*_tmp359;struct Cyc_Absyn_Aggrdecl*ad2=(
_tmp359=_cycalloc(sizeof(*_tmp359)),((_tmp359->kind=(void*)((void*)_tmpCC->kind),((
_tmp359->sc=(void*)((void*)_tmpCC->sc),((_tmp359->name=_tmpCC->name,((_tmp359->tvs=
0,((_tmp359->impl=_tmpD3,((_tmp359->attributes=_tmpCC->attributes,_tmp359)))))))))))));
struct Cyc_Absyn_AggrType_struct _tmp368;union Cyc_Absyn_AggrInfoU_union _tmp367;
struct Cyc_Absyn_Aggrdecl**_tmp366;struct Cyc_Absyn_AggrInfo _tmp365;struct Cyc_Absyn_AggrType_struct*
_tmp364;return(void*)((_tmp364=_cycalloc(sizeof(*_tmp364)),((_tmp364[0]=((
_tmp368.tag=10,((_tmp368.f1=((_tmp365.aggr_info=(union Cyc_Absyn_AggrInfoU_union)(((
_tmp367.KnownAggr).tag=1,(((_tmp367.KnownAggr).f1=((_tmp366=_cycalloc(sizeof(*
_tmp366)),((_tmp366[0]=ad2,_tmp366)))),_tmp367)))),((_tmp365.targs=0,_tmp365)))),
_tmp368)))),_tmp364))));}}}_LL13: if(*((int*)_tmpC2)!= 2)goto _LL15;_tmpC6=((
struct Cyc_Absyn_TunionType_struct*)_tmpC2)->f1;_tmpC7=_tmpC6.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmpC2)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL15;_tmpC8=(_tmpC7.KnownTunion).f1;_tmpC9=*_tmpC8;_tmpCA=_tmpC6.targs;
_tmpCB=_tmpC6.rgn;_LL14: {struct Cyc_List_List*_tmpDB=_tmpC9->tvs;if(Cyc_List_length(
_tmpCA)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpDB)){const char*
_tmp36D;void*_tmp36C[2];struct Cyc_Int_pa_struct _tmp36B;struct Cyc_Int_pa_struct
_tmp36A;(_tmp36A.tag=1,((_tmp36A.f1=(unsigned long)((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmpDB),((_tmp36B.tag=1,((_tmp36B.f1=(unsigned long)Cyc_List_length(
_tmpCA),((_tmp36C[0]=& _tmp36B,((_tmp36C[1]=& _tmp36A,((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp36D="gen(): number of params %d differs from number of tyvars %d",
_tag_dynforward(_tmp36D,sizeof(char),_get_zero_arr_size_char(_tmp36D,60)))),
_tag_dynforward(_tmp36C,sizeof(void*),2)))))))))))));}{struct Cyc_List_List*
_tmpE0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_zip)(_tmpDB,_tmpCA);struct Cyc_Core_Opt*fields=0;if(_tmpC9->fields != 0){
struct Cyc_Core_Opt*_tmp36E;fields=((_tmp36E=_cycalloc(sizeof(*_tmp36E)),((
_tmp36E->v=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tunionfield*(*f)(struct Cyc_List_List*,
struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_substitute_tunionfield_type,_tmpE0,(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmpC9->fields))->v),_tmp36E))));}{struct Cyc_Absyn_Tuniondecl*
_tmp36F;struct Cyc_Absyn_Tuniondecl*_tmpE2=(_tmp36F=_cycalloc(sizeof(*_tmp36F)),((
_tmp36F->sc=(void*)((void*)_tmpC9->sc),((_tmp36F->name=_tmpC9->name,((_tmp36F->tvs=
0,((_tmp36F->fields=fields,((_tmp36F->is_xtunion=_tmpC9->is_xtunion,((_tmp36F->is_flat=
_tmpC9->is_flat,_tmp36F)))))))))))));struct Cyc_Absyn_TunionType_struct _tmp37E;
union Cyc_Absyn_TunionInfoU_union _tmp37D;struct Cyc_Absyn_Tuniondecl**_tmp37C;
struct Cyc_Absyn_TunionInfo _tmp37B;struct Cyc_Absyn_TunionType_struct*_tmp37A;
struct Cyc_Absyn_TunionType_struct*_tmpE3=(_tmp37A=_cycalloc(sizeof(*_tmp37A)),((
_tmp37A[0]=((_tmp37E.tag=2,((_tmp37E.f1=((_tmp37B.tunion_info=(union Cyc_Absyn_TunionInfoU_union)(((
_tmp37D.KnownTunion).tag=1,(((_tmp37D.KnownTunion).f1=((_tmp37C=_cycalloc(
sizeof(*_tmp37C)),((_tmp37C[0]=_tmpE2,_tmp37C)))),_tmp37D)))),((_tmp37B.targs=
_tmpCA,((_tmp37B.rgn=_tmpCB,_tmp37B)))))),_tmp37E)))),_tmp37A)));struct Cyc_Position_Segment*
_tmpE4=Cyc_Position_segment_of_abs(0,0);return(void*)_tmpE3;}}}_LL15:;_LL16:
return type;_LL10:;}struct _dynforward_ptr Cyc_Tcgenrep_make_type_cstring(void*t);
struct _dynforward_ptr Cyc_Tcgenrep_make_type_cstring(void*t){struct
_dynforward_ptr s=Cyc_strdup((struct _dynforward_ptr)Cyc_Absynpp_typ2cstring(t));{
int i=0;for(0;i < Cyc_strlen((struct _dynforward_ptr)s);++ i){if(*((char*)
_check_dynforward_subscript(s,sizeof(char),i))== ' '){char _tmp381;char _tmp380;
struct _dynforward_ptr _tmp37F;(_tmp37F=_dynforward_ptr_plus(s,sizeof(char),i),((
_tmp380=*((char*)_check_dynforward_subscript(_tmp37F,sizeof(char),0)),((_tmp381='_',((
_get_dynforward_size(_tmp37F,sizeof(char))== 1  && (_tmp380 == '\000'  && _tmp381 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp37F.curr)=_tmp381)))))));}else{if(!isalnum((
int)*((char*)_check_dynforward_subscript(s,sizeof(char),i))) && *((char*)
_check_dynforward_subscript(s,sizeof(char),i))!= '_'){char _tmp384;char _tmp383;
struct _dynforward_ptr _tmp382;(_tmp382=_dynforward_ptr_plus(s,sizeof(char),i),((
_tmp383=*((char*)_check_dynforward_subscript(_tmp382,sizeof(char),0)),((_tmp384=(
char)('0' + *((char*)_check_dynforward_subscript(s,sizeof(char),i))% 10),((
_get_dynforward_size(_tmp382,sizeof(char))== 1  && (_tmp383 == '\000'  && _tmp384 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmp382.curr)=_tmp384)))))));}}}}{const char*
_tmp385;return(struct _dynforward_ptr)Cyc_strconcat((struct _dynforward_ptr)s,((
_tmp385="_rep",_tag_dynforward(_tmp385,sizeof(char),_get_zero_arr_size_char(
_tmp385,5)))));}}struct _tuple19{struct Cyc_Dict_Dict f1;struct Cyc_Tcgenrep_RepInfo*
f2;};static struct _tuple19*Cyc_Tcgenrep_lookupRep(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Dict_Dict dict,struct Cyc_Position_Segment*loc,void*type);struct _tuple20{
struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _tuple21{struct Cyc_Tcenv_Tenv*
f1;struct Cyc_Position_Segment*f2;};static struct _tuple20*Cyc_Tcgenrep_lookupRepsCls(
struct _tuple21*env,void*type,struct _tuple20*carry);static struct _tuple20*Cyc_Tcgenrep_lookupRepsCls(
struct _tuple21*env,void*type,struct _tuple20*carry){struct _tuple19 _tmpF3;struct
Cyc_Dict_Dict _tmpF4;struct Cyc_Tcgenrep_RepInfo*_tmpF5;struct _tuple19*_tmpF2=Cyc_Tcgenrep_lookupRep((*
env).f1,(*carry).f1,(*env).f2,type);_tmpF3=*_tmpF2;_tmpF4=_tmpF3.f1;_tmpF5=
_tmpF3.f2;{struct Cyc_List_List*_tmp388;struct _tuple20*_tmp387;return(_tmp387=
_cycalloc(sizeof(*_tmp387)),((_tmp387->f1=_tmpF4,((_tmp387->f2=((_tmp388=
_cycalloc(sizeof(*_tmp388)),((_tmp388->hd=_tmpF5,((_tmp388->tl=(*carry).f2,
_tmp388)))))),_tmp387)))));}}static struct Cyc_Dict_Dict Cyc_Tcgenrep_buildRepTuple(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict dict,struct Cyc_Position_Segment*loc,
struct _tuple6*varname,void*sc,void*type,struct Cyc_List_List*types);static struct
Cyc_Dict_Dict Cyc_Tcgenrep_buildRepTuple(struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict
dict,struct Cyc_Position_Segment*loc,struct _tuple6*varname,void*sc,void*type,
struct Cyc_List_List*types){struct Cyc_Absyn_Exp*_tmpF8=Cyc_Absyn_sizeoftyp_exp(
type,loc);struct _tuple21*_tmp389;struct _tuple21*_tmpF9=(_tmp389=_cycalloc(
sizeof(*_tmp389)),((_tmp389->f1=te,((_tmp389->f2=loc,_tmp389)))));struct _tuple20*
_tmp38A;struct _tuple20*_tmpFA=(_tmp38A=_cycalloc(sizeof(*_tmp38A)),((_tmp38A->f1=
dict,((_tmp38A->f2=0,_tmp38A)))));struct _tuple20 _tmpFC;struct Cyc_Dict_Dict _tmpFD;
struct Cyc_List_List*_tmpFE;struct _tuple20*_tmpFB=((struct _tuple20*(*)(struct
_tuple20*(*f)(struct _tuple21*,void*,struct _tuple20*),struct _tuple21*,struct Cyc_List_List*
x,struct _tuple20*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,_tmpF9,
types,_tmpFA);_tmpFC=*_tmpFB;_tmpFD=_tmpFC.f1;_tmpFE=_tmpFC.f2;dict=_tmpFD;{
struct Cyc_List_List*_tmpFF=((struct Cyc_List_List*(*)(int n,int(*f)(int)))Cyc_List_tabulate)(
Cyc_List_length(types),(int(*)(int))Cyc_Core_identity);struct _tuple14*_tmp38B;
struct Cyc_List_List*_tmp100=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct _tuple14*,int),struct _tuple14*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_make_offsetof_exp,((_tmp38B=_cycalloc(sizeof(*_tmp38B)),((_tmp38B->f1=
type,((_tmp38B->f2=loc,_tmp38B)))))),_tmpFF);struct Cyc_List_List*_tmp101=((
struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,_tmpFE);struct Cyc_List_List*
_tmp102=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple12*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_zip)(_tmp100,_tmp101));void*_tmp38C[2];void*
tuple_type=Cyc_Tcgenrep_tuple_typ(((_tmp38C[1]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str)),((_tmp38C[0]=Cyc_Absyn_uint_typ,Cyc_List_list(
_tag_dynforward(_tmp38C,sizeof(void*),2)))))));struct _tuple15 _tmp104;struct Cyc_List_List*
_tmp105;struct Cyc_Absyn_Exp*_tmp106;struct _tuple15*_tmp103=Cyc_Tcgenrep_array_decls(
tuple_type,_tmp102,loc);_tmp104=*_tmp103;_tmp105=_tmp104.f1;_tmp106=_tmp104.f2;{
struct Cyc_Absyn_Exp*_tmp38D[2];struct Cyc_Absyn_Decl*_tmp107=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_tuple_str),varname,((_tmp38D[1]=_tmp106,((_tmp38D[0]=_tmpF8,((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp38D,sizeof(struct Cyc_Absyn_Exp*),2)))))),sc,loc);struct Cyc_Absyn_Decl*
_tmp38E[1];struct Cyc_List_List*_tmp108=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp105,((_tmp38E[0]=_tmp107,((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp38E,sizeof(struct Cyc_Absyn_Decl*),1)))));return Cyc_Tcgenrep_update_info(dict,
type,_tmp108,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),_tmpFE,0);}}}static struct Cyc_Dict_Dict Cyc_Tcgenrep_buildRepTunionfield(
struct _tuple6*tname,struct _tuple6*fname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict
dict,struct Cyc_Position_Segment*loc,struct _tuple6*varname,void*sc,void*type,
struct Cyc_List_List*types);static struct Cyc_Dict_Dict Cyc_Tcgenrep_buildRepTunionfield(
struct _tuple6*tname,struct _tuple6*fname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict
dict,struct Cyc_Position_Segment*loc,struct _tuple6*varname,void*sc,void*type,
struct Cyc_List_List*types){struct Cyc_Absyn_Exp*_tmp10F=Cyc_Absyn_sizeoftyp_exp(
type,loc);struct _tuple21*_tmp38F;struct _tuple21*_tmp110=(_tmp38F=_cycalloc(
sizeof(*_tmp38F)),((_tmp38F->f1=te,((_tmp38F->f2=loc,_tmp38F)))));struct _tuple20*
_tmp390;struct _tuple20*_tmp111=(_tmp390=_cycalloc(sizeof(*_tmp390)),((_tmp390->f1=
dict,((_tmp390->f2=0,_tmp390)))));struct _tuple20 _tmp113;struct Cyc_Dict_Dict
_tmp114;struct Cyc_List_List*_tmp115;struct _tuple20*_tmp112=((struct _tuple20*(*)(
struct _tuple20*(*f)(struct _tuple21*,void*,struct _tuple20*),struct _tuple21*,
struct Cyc_List_List*x,struct _tuple20*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,
_tmp110,types,_tmp111);_tmp113=*_tmp112;_tmp114=_tmp113.f1;_tmp115=_tmp113.f2;
dict=_tmp114;{struct Cyc_List_List*_tmp116=((struct Cyc_List_List*(*)(int n,int(*f)(
int)))Cyc_List_tabulate)(Cyc_List_length(types),(int(*)(int))Cyc_Core_identity);
struct _tuple14*_tmp391;struct Cyc_List_List*_tmp117=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct _tuple14*,int),struct _tuple14*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_make_offsetof_exp,((_tmp391=_cycalloc(sizeof(*
_tmp391)),((_tmp391->f1=type,((_tmp391->f2=loc,_tmp391)))))),_tmp116);struct Cyc_List_List*
_tmp118=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,_tmp115);struct Cyc_List_List*
_tmp119=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple12*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_zip)(_tmp117,_tmp118));void*_tmp392[2];void*
tuple_type=Cyc_Tcgenrep_tuple_typ(((_tmp392[1]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str)),((_tmp392[0]=Cyc_Absyn_uint_typ,Cyc_List_list(
_tag_dynforward(_tmp392,sizeof(void*),2)))))));struct Cyc_Absyn_Exp*_tmp11A=Cyc_Tcgenrep_cnst_string(*(*
tname).f2,loc);struct Cyc_Absyn_Exp*_tmp11B=Cyc_Tcgenrep_cnst_string(*(*fname).f2,
loc);struct _tuple15 _tmp11D;struct Cyc_List_List*_tmp11E;struct Cyc_Absyn_Exp*
_tmp11F;struct _tuple15*_tmp11C=Cyc_Tcgenrep_array_decls(tuple_type,_tmp119,loc);
_tmp11D=*_tmp11C;_tmp11E=_tmp11D.f1;_tmp11F=_tmp11D.f2;{struct Cyc_Absyn_Exp*
_tmp393[4];struct Cyc_Absyn_Decl*_tmp120=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tunionfield_str),
varname,((_tmp393[3]=_tmp11F,((_tmp393[2]=_tmp10F,((_tmp393[1]=_tmp11B,((_tmp393[
0]=_tmp11A,((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp393,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),sc,loc);struct
Cyc_Absyn_Decl*_tmp394[1];struct Cyc_List_List*_tmp121=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp11E,((
_tmp394[0]=_tmp120,((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp394,sizeof(struct Cyc_Absyn_Decl*),1)))));return Cyc_Tcgenrep_update_info(
dict,type,_tmp121,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),_tmp115,0);}}}static struct Cyc_Dict_Dict Cyc_Tcgenrep_buildRepStruct(
struct Cyc_Core_Opt*sname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict dict,struct
Cyc_Position_Segment*loc,struct _tuple6*varname,void*sc,void*type,struct Cyc_List_List*
nmtypes);static struct Cyc_Dict_Dict Cyc_Tcgenrep_buildRepStruct(struct Cyc_Core_Opt*
sname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict dict,struct Cyc_Position_Segment*
loc,struct _tuple6*varname,void*sc,void*type,struct Cyc_List_List*nmtypes){struct
Cyc_Absyn_Exp*_tmp128=Cyc_Absyn_sizeoftyp_exp(type,loc);struct _tuple21*_tmp395;
struct _tuple21*_tmp129=(_tmp395=_cycalloc(sizeof(*_tmp395)),((_tmp395->f1=te,((
_tmp395->f2=loc,_tmp395)))));struct _tuple20*_tmp396;struct _tuple20*_tmp12A=(
_tmp396=_cycalloc(sizeof(*_tmp396)),((_tmp396->f1=dict,((_tmp396->f2=0,_tmp396)))));
struct Cyc_List_List*_tmp12C;struct Cyc_List_List*_tmp12D;struct _tuple0 _tmp12B=((
struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(nmtypes);_tmp12C=_tmp12B.f1;
_tmp12D=_tmp12B.f2;{struct _tuple20 _tmp12F;struct Cyc_Dict_Dict _tmp130;struct Cyc_List_List*
_tmp131;struct _tuple20*_tmp12E=((struct _tuple20*(*)(struct _tuple20*(*f)(struct
_tuple21*,void*,struct _tuple20*),struct _tuple21*,struct Cyc_List_List*x,struct
_tuple20*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,_tmp129,
_tmp12D,_tmp12A);_tmp12F=*_tmp12E;_tmp130=_tmp12F.f1;_tmp131=_tmp12F.f2;dict=
_tmp130;{struct Cyc_List_List*_tmp132=((struct Cyc_List_List*(*)(int n,int(*f)(int)))
Cyc_List_tabulate)(Cyc_List_length(_tmp12D),(int(*)(int))Cyc_Core_identity);
struct _tuple14*_tmp397;struct Cyc_List_List*_tmp133=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct _tuple14*,int),struct _tuple14*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_make_offsetof_exp,((_tmp397=_cycalloc(sizeof(*
_tmp397)),((_tmp397->f1=type,((_tmp397->f2=loc,_tmp397)))))),_tmp132);struct Cyc_List_List*
_tmp134=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _dynforward_ptr*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))
Cyc_List_map_c)(Cyc_Tcgenrep_cnst_string_cls,loc,_tmp12C);struct Cyc_List_List*
_tmp135=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,_tmp131);struct Cyc_List_List*
_tmp136=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple13*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tuple3_exp_cls,loc,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y,struct Cyc_List_List*z))Cyc_List_zip3)(_tmp133,_tmp134,
_tmp135));void*_tmp398[3];void*tuple_type=Cyc_Tcgenrep_tuple_typ(((_tmp398[2]=
Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str)),((
_tmp398[1]=Cyc_Absyn_const_string_typ((void*)2),((_tmp398[0]=Cyc_Absyn_uint_typ,
Cyc_List_list(_tag_dynforward(_tmp398,sizeof(void*),3)))))))));struct _tuple15
_tmp138;struct Cyc_List_List*_tmp139;struct Cyc_Absyn_Exp*_tmp13A;struct _tuple15*
_tmp137=Cyc_Tcgenrep_array_decls(tuple_type,_tmp136,loc);_tmp138=*_tmp137;
_tmp139=_tmp138.f1;_tmp13A=_tmp138.f2;{struct Cyc_Absyn_Exp*name;if(sname == 0)
name=Cyc_Absyn_null_exp(loc);else{const char*_tmp399;struct _dynforward_ptr*
_tmp13B=Cyc_Tcgenrep_new_gen_id(((_tmp399="name",_tag_dynforward(_tmp399,sizeof(
char),_get_zero_arr_size_char(_tmp399,5)))));struct Cyc_Absyn_Decl*_tmp13C=Cyc_Tcgenrep_gen_decl(
_tmp13B,Cyc_Absyn_const_string_typ((void*)2),(struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_string(*(*((
struct _tuple6*)sname->v)).f2,loc),loc);{struct Cyc_List_List*_tmp39A;_tmp139=((
_tmp39A=_cycalloc(sizeof(*_tmp39A)),((_tmp39A->hd=_tmp13C,((_tmp39A->tl=_tmp139,
_tmp39A))))));}name=Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_toplevel_name(
_tmp13B),loc),loc);}{struct Cyc_Absyn_Exp*_tmp39B[3];struct Cyc_Absyn_Decl*_tmp13F=
Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_struct_str),varname,((_tmp39B[2]=_tmp13A,((
_tmp39B[1]=_tmp128,((_tmp39B[0]=name,((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp39B,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),sc,loc);struct Cyc_Absyn_Decl*_tmp39C[1];struct Cyc_List_List*_tmp140=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmp139,((_tmp39C[0]=_tmp13F,((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp39C,sizeof(struct Cyc_Absyn_Decl*),1)))));
return Cyc_Tcgenrep_update_info(dict,type,_tmp140,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmp131,0);}}}}}static struct Cyc_Dict_Dict
Cyc_Tcgenrep_buildRepUnion(struct Cyc_Core_Opt*uname,struct Cyc_Tcenv_Tenv*te,
struct Cyc_Dict_Dict dict,struct Cyc_Position_Segment*loc,struct _tuple6*varname,
void*sc,void*type,struct Cyc_List_List*nmtypes);static struct Cyc_Dict_Dict Cyc_Tcgenrep_buildRepUnion(
struct Cyc_Core_Opt*uname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict dict,struct
Cyc_Position_Segment*loc,struct _tuple6*varname,void*sc,void*type,struct Cyc_List_List*
nmtypes){struct Cyc_Absyn_Exp*_tmp147=Cyc_Absyn_sizeoftyp_exp(type,loc);struct
_tuple21*_tmp39D;struct _tuple21*_tmp148=(_tmp39D=_cycalloc(sizeof(*_tmp39D)),((
_tmp39D->f1=te,((_tmp39D->f2=loc,_tmp39D)))));struct _tuple20*_tmp39E;struct
_tuple20*_tmp149=(_tmp39E=_cycalloc(sizeof(*_tmp39E)),((_tmp39E->f1=dict,((
_tmp39E->f2=0,_tmp39E)))));struct Cyc_List_List*_tmp14B;struct Cyc_List_List*
_tmp14C;struct _tuple0 _tmp14A=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(
nmtypes);_tmp14B=_tmp14A.f1;_tmp14C=_tmp14A.f2;{struct _tuple20 _tmp14E;struct Cyc_Dict_Dict
_tmp14F;struct Cyc_List_List*_tmp150;struct _tuple20*_tmp14D=((struct _tuple20*(*)(
struct _tuple20*(*f)(struct _tuple21*,void*,struct _tuple20*),struct _tuple21*,
struct Cyc_List_List*x,struct _tuple20*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,
_tmp148,_tmp14C,_tmp149);_tmp14E=*_tmp14D;_tmp14F=_tmp14E.f1;_tmp150=_tmp14E.f2;
dict=_tmp14F;{struct Cyc_List_List*_tmp151=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _dynforward_ptr*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_string_cls,loc,
_tmp14B);struct Cyc_List_List*_tmp152=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp150);struct Cyc_List_List*_tmp153=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple12*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct
Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(
_tmp151,_tmp152));void*_tmp39F[2];void*_tmp154=Cyc_Tcgenrep_tuple_typ(((_tmp39F[
1]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str)),((
_tmp39F[0]=Cyc_Absyn_const_string_typ((void*)2),Cyc_List_list(_tag_dynforward(
_tmp39F,sizeof(void*),2)))))));struct _tuple15 _tmp156;struct Cyc_List_List*_tmp157;
struct Cyc_Absyn_Exp*_tmp158;struct _tuple15*_tmp155=Cyc_Tcgenrep_array_decls(
_tmp154,_tmp153,loc);_tmp156=*_tmp155;_tmp157=_tmp156.f1;_tmp158=_tmp156.f2;{
struct Cyc_Absyn_Exp*name;if(uname == 0)name=Cyc_Absyn_null_exp(loc);else{const
char*_tmp3A0;struct _dynforward_ptr*_tmp159=Cyc_Tcgenrep_new_gen_id(((_tmp3A0="name",
_tag_dynforward(_tmp3A0,sizeof(char),_get_zero_arr_size_char(_tmp3A0,5)))));
struct Cyc_Absyn_Decl*_tmp15A=Cyc_Tcgenrep_gen_decl(_tmp159,Cyc_Absyn_const_string_typ((
void*)2),(struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_string(*(*((struct _tuple6*)
uname->v)).f2,loc),loc);{struct Cyc_List_List*_tmp3A1;_tmp157=((_tmp3A1=_cycalloc(
sizeof(*_tmp3A1)),((_tmp3A1->hd=_tmp15A,((_tmp3A1->tl=_tmp157,_tmp3A1))))));}
name=Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_toplevel_name(
_tmp159),loc),loc);}{struct Cyc_Absyn_Exp*_tmp3A2[3];struct Cyc_Absyn_Decl*_tmp15D=
Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_union_str),varname,((_tmp3A2[2]=_tmp158,((
_tmp3A2[1]=_tmp147,((_tmp3A2[0]=name,((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3A2,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),sc,loc);struct Cyc_Absyn_Decl*_tmp3A3[1];return Cyc_Tcgenrep_update_info(
dict,type,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp157,((_tmp3A3[0]=_tmp15D,((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3A3,sizeof(struct Cyc_Absyn_Decl*),
1))))),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),_tmp150,0);}}}}}static struct Cyc_Dict_Dict Cyc_Tcgenrep_buildRepEnum(
struct _tuple6**ename,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict dict,struct Cyc_Position_Segment*
loc,struct _tuple6*varname,void*sc,void*type,struct Cyc_List_List*tagnms);static
struct Cyc_Dict_Dict Cyc_Tcgenrep_buildRepEnum(struct _tuple6**ename,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Dict_Dict dict,struct Cyc_Position_Segment*loc,struct _tuple6*varname,
void*sc,void*type,struct Cyc_List_List*tagnms){struct Cyc_Absyn_Exp*_tmp163=Cyc_Absyn_sizeoftyp_exp(
type,loc);struct Cyc_List_List*_tmp165;struct Cyc_List_List*_tmp166;struct _tuple0
_tmp164=((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(tagnms);
_tmp165=_tmp164.f1;_tmp166=_tmp164.f2;{struct Cyc_List_List*_tmp167=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,struct _tuple6*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_qvar_string_cls,loc,
_tmp166);struct Cyc_List_List*_tmp168=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,int),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_int_cls,loc,_tmp165);struct Cyc_List_List*
_tmp169=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple12*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tuple2_exp_cls,loc,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_zip)(_tmp168,_tmp167));void*_tmp3A4[2];void*
_tmp16A=Cyc_Tcgenrep_tuple_typ(((_tmp3A4[1]=Cyc_Absyn_const_string_typ((void*)2),((
_tmp3A4[0]=Cyc_Absyn_uint_typ,Cyc_List_list(_tag_dynforward(_tmp3A4,sizeof(void*),
2)))))));struct _tuple15 _tmp16C;struct Cyc_List_List*_tmp16D;struct Cyc_Absyn_Exp*
_tmp16E;struct _tuple15*_tmp16B=Cyc_Tcgenrep_array_decls(_tmp16A,_tmp169,loc);
_tmp16C=*_tmp16B;_tmp16D=_tmp16C.f1;_tmp16E=_tmp16C.f2;{struct Cyc_Absyn_Exp*name;
if(ename == 0)name=Cyc_Absyn_null_exp(loc);else{const char*_tmp3A5;struct
_dynforward_ptr*_tmp16F=Cyc_Tcgenrep_new_gen_id(((_tmp3A5="name",_tag_dynforward(
_tmp3A5,sizeof(char),_get_zero_arr_size_char(_tmp3A5,5)))));struct Cyc_Absyn_Decl*
_tmp170=Cyc_Tcgenrep_gen_decl(_tmp16F,Cyc_Absyn_const_string_typ((void*)2),(
struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_string(*(*(*ename)).f2,loc),loc);{struct
Cyc_List_List*_tmp3A6;_tmp16D=((_tmp3A6=_cycalloc(sizeof(*_tmp3A6)),((_tmp3A6->hd=
_tmp170,((_tmp3A6->tl=_tmp16D,_tmp3A6))))));}name=Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
Cyc_Tcgenrep_toplevel_name(_tmp16F),loc),loc);}{struct Cyc_Absyn_Exp*_tmp3A7[3];
struct Cyc_Absyn_Decl*_tmp173=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_enum_str),
varname,((_tmp3A7[2]=_tmp16E,((_tmp3A7[1]=_tmp163,((_tmp3A7[0]=name,((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3A7,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),sc,loc);struct Cyc_Absyn_Decl*_tmp3A8[1];return Cyc_Tcgenrep_update_info(
dict,type,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(_tmp16D,((_tmp3A8[0]=_tmp173,((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3A8,sizeof(struct Cyc_Absyn_Decl*),
1))))),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
varname,loc),loc),0,0);}}}}static struct Cyc_Dict_Dict Cyc_Tcgenrep_buildRepTunion(
struct _tuple6*tname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict dict,struct Cyc_Position_Segment*
loc,struct _tuple6*varname,void*sc,void*type,struct Cyc_List_List*tonames,struct
Cyc_List_List*nmtypes);static struct Cyc_Dict_Dict Cyc_Tcgenrep_buildRepTunion(
struct _tuple6*tname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict dict,struct Cyc_Position_Segment*
loc,struct _tuple6*varname,void*sc,void*type,struct Cyc_List_List*tonames,struct
Cyc_List_List*nmtypes){struct Cyc_List_List*_tmp177=((struct Cyc_List_List*(*)(int
n,int(*f)(int)))Cyc_List_tabulate)(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
tonames),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*_tmp178=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,int),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_int_cls,loc,_tmp177);
struct Cyc_List_List*_tmp179=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(
struct Cyc_Position_Segment*,struct _tuple6*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_qvar_string_cls,loc,
tonames);struct Cyc_List_List*_tmp17A=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp178,_tmp179);struct Cyc_List_List*
_tmp17B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple12*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tuple2_exp_cls,loc,_tmp17A);void*_tmp3A9[2];void*_tmp17C=Cyc_Tcgenrep_tuple_typ(((
_tmp3A9[1]=Cyc_Absyn_const_string_typ((void*)2),((_tmp3A9[0]=Cyc_Absyn_uint_typ,
Cyc_List_list(_tag_dynforward(_tmp3A9,sizeof(void*),2)))))));struct _tuple15
_tmp17E;struct Cyc_List_List*_tmp17F;struct Cyc_Absyn_Exp*_tmp180;struct _tuple15*
_tmp17D=Cyc_Tcgenrep_array_decls(_tmp17C,_tmp17B,loc);_tmp17E=*_tmp17D;_tmp17F=
_tmp17E.f1;_tmp180=_tmp17E.f2;{struct _tuple21*_tmp3AA;struct _tuple21*_tmp181=(
_tmp3AA=_cycalloc(sizeof(*_tmp3AA)),((_tmp3AA->f1=te,((_tmp3AA->f2=loc,_tmp3AA)))));
struct _tuple20*_tmp3AB;struct _tuple20*_tmp182=(_tmp3AB=_cycalloc(sizeof(*_tmp3AB)),((
_tmp3AB->f1=dict,((_tmp3AB->f2=0,_tmp3AB)))));struct Cyc_List_List*_tmp184;struct
Cyc_List_List*_tmp185;struct _tuple0 _tmp183=((struct _tuple0(*)(struct Cyc_List_List*
x))Cyc_List_split)(nmtypes);_tmp184=_tmp183.f1;_tmp185=_tmp183.f2;{struct
_tuple20 _tmp187;struct Cyc_Dict_Dict _tmp188;struct Cyc_List_List*_tmp189;struct
_tuple20*_tmp186=((struct _tuple20*(*)(struct _tuple20*(*f)(struct _tuple21*,void*,
struct _tuple20*),struct _tuple21*,struct Cyc_List_List*x,struct _tuple20*accum))Cyc_List_fold_right_c)(
Cyc_Tcgenrep_lookupRepsCls,_tmp181,_tmp185,_tmp182);_tmp187=*_tmp186;_tmp188=
_tmp187.f1;_tmp189=_tmp187.f2;dict=_tmp188;{struct Cyc_List_List*_tmp18A=((struct
Cyc_List_List*(*)(int n,int(*f)(int)))Cyc_List_tabulate)(((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp189),(int(*)(int))Cyc_Core_identity);struct Cyc_List_List*
_tmp18B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
int),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_int_cls,
loc,_tmp18A);struct Cyc_List_List*_tmp18C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple6*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_qvar_string_cls,loc,
_tmp184);struct Cyc_List_List*_tmp18D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp189);struct Cyc_List_List*_tmp18E=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y,struct Cyc_List_List*z))Cyc_List_zip3)(_tmp18B,_tmp18C,
_tmp18D);struct Cyc_List_List*_tmp18F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _tuple13*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_tuple3_exp_cls,loc,_tmp18E);
void*_tmp3AC[3];void*tuple_type2=Cyc_Tcgenrep_tuple_typ(((_tmp3AC[2]=Cyc_Tcgenrep_tunion_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str)),((_tmp3AC[1]=Cyc_Absyn_const_string_typ((
void*)2),((_tmp3AC[0]=Cyc_Absyn_uint_typ,Cyc_List_list(_tag_dynforward(_tmp3AC,
sizeof(void*),3)))))))));struct _tuple15 _tmp191;struct Cyc_List_List*_tmp192;
struct Cyc_Absyn_Exp*_tmp193;struct _tuple15*_tmp190=Cyc_Tcgenrep_array_decls(
tuple_type2,_tmp18F,loc);_tmp191=*_tmp190;_tmp192=_tmp191.f1;_tmp193=_tmp191.f2;{
struct Cyc_Absyn_Exp*_tmp194=Cyc_Tcgenrep_cnst_string(*(*tname).f2,loc);struct Cyc_Absyn_Exp*
_tmp3AD[3];struct Cyc_Absyn_Decl*_tmp195=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tunion_str),
varname,((_tmp3AD[2]=_tmp193,((_tmp3AD[1]=_tmp180,((_tmp3AD[0]=_tmp194,((struct
Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3AD,
sizeof(struct Cyc_Absyn_Exp*),3)))))))),sc,loc);{struct Cyc_Absyn_Decl*_tmp3AE[1];
return Cyc_Tcgenrep_update_info(dict,type,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp17F,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp192,((
_tmp3AE[0]=_tmp195,((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp3AE,sizeof(struct Cyc_Absyn_Decl*),1)))))),(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmp189,0);}
return dict;}}}}}static struct Cyc_Dict_Dict Cyc_Tcgenrep_buildRepXTunion(struct
_tuple6*xname,struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict dict,struct Cyc_Position_Segment*
loc,struct _tuple6*varname,void*sc,void*type,struct Cyc_List_List*nametypes);
static struct Cyc_Dict_Dict Cyc_Tcgenrep_buildRepXTunion(struct _tuple6*xname,struct
Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict dict,struct Cyc_Position_Segment*loc,struct
_tuple6*varname,void*sc,void*type,struct Cyc_List_List*nametypes){struct Cyc_List_List*
_tmp19D;struct Cyc_List_List*_tmp19E;struct _tuple0 _tmp19C=((struct _tuple0(*)(
struct Cyc_List_List*x))Cyc_List_split)(nametypes);_tmp19D=_tmp19C.f1;_tmp19E=
_tmp19C.f2;{struct _tuple21*_tmp3AF;struct _tuple21*_tmp19F=(_tmp3AF=_cycalloc(
sizeof(*_tmp3AF)),((_tmp3AF->f1=te,((_tmp3AF->f2=loc,_tmp3AF)))));struct _tuple20*
_tmp3B0;struct _tuple20*_tmp1A0=(_tmp3B0=_cycalloc(sizeof(*_tmp3B0)),((_tmp3B0->f1=
dict,((_tmp3B0->f2=0,_tmp3B0)))));struct _tuple20 _tmp1A2;struct Cyc_Dict_Dict
_tmp1A3;struct Cyc_List_List*_tmp1A4;struct _tuple20*_tmp1A1=((struct _tuple20*(*)(
struct _tuple20*(*f)(struct _tuple21*,void*,struct _tuple20*),struct _tuple21*,
struct Cyc_List_List*x,struct _tuple20*accum))Cyc_List_fold_right_c)(Cyc_Tcgenrep_lookupRepsCls,
_tmp19F,_tmp19E,_tmp1A0);_tmp1A2=*_tmp1A1;_tmp1A3=_tmp1A2.f1;_tmp1A4=_tmp1A2.f2;
dict=_tmp1A3;{struct Cyc_List_List*_tmp1A5=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct _dynforward_ptr*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_cnst_string_cls,loc,
_tmp19D);struct Cyc_List_List*_tmp1A6=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Position_Segment*,struct Cyc_Tcgenrep_RepInfo*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcgenrep_get_and_cast_ri_exp,loc,
_tmp1A4);struct Cyc_List_List*_tmp1A7=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp1A5,_tmp1A6);struct Cyc_List_List*
_tmp1A8=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Position_Segment*,
struct _tuple12*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Tcgenrep_tuple2_exp_cls,loc,_tmp1A7);struct Cyc_Absyn_Tqual _tmp3B1;void*
name_type=Cyc_Absyn_dynforward_typ(Cyc_Absyn_char_typ,(void*)2,((_tmp3B1.print_const=
1,((_tmp3B1.q_volatile=0,((_tmp3B1.q_restrict=0,((_tmp3B1.real_const=1,((_tmp3B1.loc=
0,_tmp3B1)))))))))),Cyc_Absyn_true_conref);void*_tmp3B2[2];void*tuple_type=Cyc_Tcgenrep_tuple_typ(((
_tmp3B2[1]=Cyc_Tcgenrep_tunion_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str)),((
_tmp3B2[0]=name_type,Cyc_List_list(_tag_dynforward(_tmp3B2,sizeof(void*),2)))))));
struct _tuple15 _tmp1AA;struct Cyc_List_List*_tmp1AB;struct Cyc_Absyn_Exp*_tmp1AC;
struct _tuple15*_tmp1A9=Cyc_Tcgenrep_array_decls(tuple_type,_tmp1A8,loc);_tmp1AA=*
_tmp1A9;_tmp1AB=_tmp1AA.f1;_tmp1AC=_tmp1AA.f2;{struct Cyc_Absyn_Exp*_tmp1AD=Cyc_Tcgenrep_cnst_string(*(*
xname).f2,loc);struct Cyc_Absyn_Exp*_tmp3B3[2];struct Cyc_Absyn_Decl*_tmp1AE=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_xtunion_str),varname,((_tmp3B3[1]=_tmp1AC,((_tmp3B3[0]=_tmp1AD,((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp3B3,sizeof(struct Cyc_Absyn_Exp*),2)))))),sc,loc);{struct Cyc_Absyn_Decl*
_tmp3B4[1];return Cyc_Tcgenrep_update_info(dict,type,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1AB,((
_tmp3B4[0]=_tmp1AE,((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp3B4,sizeof(struct Cyc_Absyn_Decl*),1))))),(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(varname,loc),loc),_tmp1A4,0);}
return dict;}}}}static struct _tuple19*Cyc_Tcgenrep_lookupRep(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Dict_Dict dict,struct Cyc_Position_Segment*loc,void*type);static
struct _tuple19*Cyc_Tcgenrep_lookupRep(struct Cyc_Tcenv_Tenv*te,struct Cyc_Dict_Dict
dict,struct Cyc_Position_Segment*loc,void*type){struct Cyc_Tcgenrep_RepInfo**info=((
struct Cyc_Tcgenrep_RepInfo**(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup_opt)(
dict,type);if(info != 0){if((*info)->is_extern)dict=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict,void*))Cyc_Dict_delete)(dict,type);else{struct _tuple19*
_tmp3B5;struct _tuple19*_tmp1B5=(_tmp3B5=_cycalloc(sizeof(*_tmp3B5)),((_tmp3B5->f1=
dict,((_tmp3B5->f2=*info,_tmp3B5)))));return _tmp1B5;}}{void*_tmp1B7=Cyc_Tcutil_compress(
Cyc_Tcgenrep_monomorphize_type(type));void*_tmp1B8;void*_tmp1B9;int _tmp1BA;
struct Cyc_Absyn_PtrInfo _tmp1BB;struct Cyc_Absyn_ArrayInfo _tmp1BC;void*_tmp1BD;
struct Cyc_Absyn_Tqual _tmp1BE;struct Cyc_Absyn_Exp*_tmp1BF;struct Cyc_Absyn_Conref*
_tmp1C0;struct Cyc_List_List*_tmp1C1;struct _tuple6*_tmp1C2;struct Cyc_List_List*
_tmp1C3;struct Cyc_Absyn_Typedefdecl*_tmp1C4;void**_tmp1C5;struct Cyc_Absyn_Tvar*
_tmp1C6;struct Cyc_Absyn_FnInfo _tmp1C7;struct Cyc_Absyn_TunionInfo _tmp1C8;union Cyc_Absyn_TunionInfoU_union
_tmp1C9;struct Cyc_Absyn_Tuniondecl**_tmp1CA;struct Cyc_Absyn_Tuniondecl*_tmp1CB;
struct Cyc_List_List*_tmp1CC;struct Cyc_Core_Opt*_tmp1CD;struct Cyc_Absyn_TunionFieldInfo
_tmp1CE;struct Cyc_Absyn_AggrInfo _tmp1CF;union Cyc_Absyn_AggrInfoU_union _tmp1D0;
struct Cyc_List_List*_tmp1D1;void*_tmp1D2;struct Cyc_List_List*_tmp1D3;void*
_tmp1D4;struct Cyc_List_List*_tmp1D5;struct Cyc_Absyn_Enumdecl*_tmp1D6;struct Cyc_List_List*
_tmp1D7;_LL18: if(_tmp1B7 <= (void*)4)goto _LL1A;if(*((int*)_tmp1B7)!= 5)goto _LL1A;
_tmp1B8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp1B7)->f1;_tmp1B9=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp1B7)->f2;_LL19: {const char*_tmp3B6;struct
_tuple6*_tmp1D8=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(((_tmp3B6="rep",
_tag_dynforward(_tmp3B6,sizeof(char),_get_zero_arr_size_char(_tmp3B6,4))))));
struct Cyc_Absyn_Exp*_tmp3B7[2];struct Cyc_Absyn_Decl*_tmp1D9=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_int_str),_tmp1D8,((_tmp3B7[1]=Cyc_Tcgenrep_cnst_int(Cyc_Tcgenrep_size_of2int(
_tmp1B9),loc),((_tmp3B7[0]=Cyc_Tcgenrep_cnst_int(_tmp1B8 == (void*)0?1: 0,loc),((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmp3B7,sizeof(struct Cyc_Absyn_Exp*),2)))))),(void*)0,loc);{struct Cyc_Absyn_Decl*
_tmp3B8[1];dict=Cyc_Tcgenrep_update_info(dict,type,((_tmp3B8[0]=_tmp1D9,((struct
Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3B8,
sizeof(struct Cyc_Absyn_Decl*),1)))),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp1D8,loc),loc),0,0);}goto _LL17;}_LL1A: if((int)_tmp1B7
!= 1)goto _LL1C;_LL1B: dict=Cyc_Tcgenrep_update_info(dict,type,0,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_unknownid_exp(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_float_str),loc),
0,0);goto _LL17;_LL1C: if(_tmp1B7 <= (void*)4)goto _LL26;if(*((int*)_tmp1B7)!= 6)
goto _LL1E;_tmp1BA=((struct Cyc_Absyn_DoubleType_struct*)_tmp1B7)->f1;_LL1D: dict=
Cyc_Tcgenrep_update_info(dict,type,0,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unknownid_exp(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_double_str),loc),0,0);goto _LL17;_LL1E:
if(*((int*)_tmp1B7)!= 4)goto _LL20;_tmp1BB=((struct Cyc_Absyn_PointerType_struct*)
_tmp1B7)->f1;_LL1F:{void*_tmp1DD=Cyc_Absyn_conref_val((_tmp1BB.ptr_atts).bounds);
struct Cyc_Absyn_Exp*_tmp1DE;_LL53: if(_tmp1DD <= (void*)2)goto _LL55;if(*((int*)
_tmp1DD)!= 0)goto _LL55;_tmp1DE=((struct Cyc_Absyn_Upper_b_struct*)_tmp1DD)->f1;
_LL54: {const char*_tmp3B9;struct _tuple6*_tmp1DF=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(((
_tmp3B9="rep",_tag_dynforward(_tmp3B9,sizeof(char),_get_zero_arr_size_char(
_tmp3B9,4))))));struct Cyc_Core_Opt*_tmp3BA;struct Cyc_Core_Opt*_tmp1E0=(_tmp3BA=
_cycalloc(sizeof(*_tmp3BA)),((_tmp3BA->v=Cyc_Tcgenrep_gen_vardecl(_tmp1DF,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_thinptr_str)),0,(void*)3,loc),_tmp3BA)));dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp1DF,loc),loc),_tmp1E0,0);{struct _tuple19 _tmp1E2;struct Cyc_Dict_Dict _tmp1E3;
struct Cyc_Tcgenrep_RepInfo*_tmp1E4;struct _tuple19*_tmp1E1=Cyc_Tcgenrep_lookupRep(
te,dict,loc,(void*)_tmp1BB.elt_typ);_tmp1E2=*_tmp1E1;_tmp1E3=_tmp1E2.f1;_tmp1E4=
_tmp1E2.f2;dict=_tmp1E3;{struct Cyc_Absyn_Exp*_tmp3BB[2];struct Cyc_Absyn_Decl*
_tmp1E5=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_thinptr_str),_tmp1DF,((_tmp3BB[1]=(
struct Cyc_Absyn_Exp*)_check_null(_tmp1E4->exp),((_tmp3BB[0]=_tmp1DE,((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3BB,sizeof(struct Cyc_Absyn_Exp*),
2)))))),(void*)0,loc);{struct Cyc_Tcgenrep_RepInfo*_tmp3BD[1];struct Cyc_Absyn_Decl*
_tmp3BC[1];dict=Cyc_Tcgenrep_update_info(dict,type,((_tmp3BC[0]=_tmp1E5,((struct
Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3BC,
sizeof(struct Cyc_Absyn_Decl*),1)))),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp1DF,loc),loc),((_tmp3BD[0]=_tmp1E4,((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3BD,sizeof(struct Cyc_Tcgenrep_RepInfo*),
1)))),0);}goto _LL52;}}}_LL55: if((int)_tmp1DD != 0)goto _LL57;_LL56: goto _LL58;_LL57:
if((int)_tmp1DD != 1)goto _LL59;_LL58: {const char*_tmp3BE;struct _tuple6*_tmp1EB=
Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(((_tmp3BE="rep",
_tag_dynforward(_tmp3BE,sizeof(char),_get_zero_arr_size_char(_tmp3BE,4))))));
struct Cyc_Core_Opt*_tmp3BF;struct Cyc_Core_Opt*_tmp1EC=(_tmp3BF=_cycalloc(sizeof(*
_tmp3BF)),((_tmp3BF->v=Cyc_Tcgenrep_gen_vardecl(_tmp1EB,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_fatptr_str)),0,(void*)3,loc),_tmp3BF)));dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp1EB,loc),loc),_tmp1EC,0);{struct _tuple19 _tmp1EE;struct Cyc_Dict_Dict _tmp1EF;
struct Cyc_Tcgenrep_RepInfo*_tmp1F0;struct _tuple19*_tmp1ED=Cyc_Tcgenrep_lookupRep(
te,dict,loc,(void*)_tmp1BB.elt_typ);_tmp1EE=*_tmp1ED;_tmp1EF=_tmp1EE.f1;_tmp1F0=
_tmp1EE.f2;dict=_tmp1EF;{struct Cyc_Absyn_Exp*_tmp3C0[1];struct Cyc_Absyn_Decl*
_tmp1F1=Cyc_Tcgenrep_tunion_constructor_decl(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_fatptr_str),_tmp1EB,((_tmp3C0[0]=(struct
Cyc_Absyn_Exp*)_check_null(_tmp1F0->exp),((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3C0,sizeof(struct Cyc_Absyn_Exp*),
1)))),(void*)0,loc);{struct Cyc_Tcgenrep_RepInfo*_tmp3C2[1];struct Cyc_Absyn_Decl*
_tmp3C1[1];dict=Cyc_Tcgenrep_update_info(dict,type,((_tmp3C1[0]=_tmp1F1,((struct
Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3C1,
sizeof(struct Cyc_Absyn_Decl*),1)))),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp1EB,loc),loc),((_tmp3C2[0]=_tmp1F0,((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3C2,sizeof(struct Cyc_Tcgenrep_RepInfo*),
1)))),0);}goto _LL52;}}}_LL59: if(_tmp1DD <= (void*)2)goto _LL52;if(*((int*)_tmp1DD)
!= 1)goto _LL52;_LL5A: {const char*_tmp3C5;void*_tmp3C4;(_tmp3C4=0,((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp3C5="tcgenregp: no support for abstract array bounds",
_tag_dynforward(_tmp3C5,sizeof(char),_get_zero_arr_size_char(_tmp3C5,48)))),
_tag_dynforward(_tmp3C4,sizeof(void*),0)));}_LL52:;}goto _LL17;_LL20: if(*((int*)
_tmp1B7)!= 7)goto _LL22;_tmp1BC=((struct Cyc_Absyn_ArrayType_struct*)_tmp1B7)->f1;
_tmp1BD=(void*)_tmp1BC.elt_type;_tmp1BE=_tmp1BC.tq;_tmp1BF=_tmp1BC.num_elts;
_tmp1C0=_tmp1BC.zero_term;_LL21: if(_tmp1BF == 0){const char*_tmp3C8;void*_tmp3C7;(
_tmp3C7=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmp3C8="tcgenrep:At this point, array bounds must be constants",_tag_dynforward(
_tmp3C8,sizeof(char),_get_zero_arr_size_char(_tmp3C8,55)))),_tag_dynforward(
_tmp3C7,sizeof(void*),0)));}{struct _tuple19 _tmp1FC;struct Cyc_Dict_Dict _tmp1FD;
struct Cyc_Tcgenrep_RepInfo*_tmp1FE;struct _tuple19*_tmp1FB=Cyc_Tcgenrep_lookupRep(
te,dict,loc,_tmp1BD);_tmp1FC=*_tmp1FB;_tmp1FD=_tmp1FC.f1;_tmp1FE=_tmp1FC.f2;dict=
_tmp1FD;{const char*_tmp3C9;struct _tuple6*_tmp1FF=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(((
_tmp3C9="rep",_tag_dynforward(_tmp3C9,sizeof(char),_get_zero_arr_size_char(
_tmp3C9,4))))));struct Cyc_Absyn_Exp*_tmp3CA[2];struct Cyc_Absyn_Decl*_tmp200=Cyc_Tcgenrep_tunion_constructor_decl(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_thinptr_str),_tmp1FF,((_tmp3CA[1]=(struct Cyc_Absyn_Exp*)_check_null(
_tmp1FE->exp),((_tmp3CA[0]=(struct Cyc_Absyn_Exp*)_tmp1BF,((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3CA,sizeof(struct Cyc_Absyn_Exp*),
2)))))),(void*)0,loc);{struct Cyc_Tcgenrep_RepInfo*_tmp3CC[1];struct Cyc_Absyn_Decl*
_tmp3CB[1];dict=Cyc_Tcgenrep_update_info(dict,type,((_tmp3CB[0]=_tmp200,((struct
Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3CB,
sizeof(struct Cyc_Absyn_Decl*),1)))),(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp1FF,loc),loc),((_tmp3CC[0]=_tmp1FE,((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp3CC,sizeof(struct Cyc_Tcgenrep_RepInfo*),
1)))),0);}goto _LL17;}}_LL22: if(*((int*)_tmp1B7)!= 9)goto _LL24;_tmp1C1=((struct
Cyc_Absyn_TupleType_struct*)_tmp1B7)->f1;_LL23: {const char*_tmp3CD;struct _tuple6*
_tmp205=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(((_tmp3CD="rep",
_tag_dynforward(_tmp3CD,sizeof(char),_get_zero_arr_size_char(_tmp3CD,4))))));
struct Cyc_Core_Opt*_tmp3CE;struct Cyc_Core_Opt*_tmp206=(_tmp3CE=_cycalloc(sizeof(*
_tmp3CE)),((_tmp3CE->v=Cyc_Tcgenrep_gen_vardecl(_tmp205,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_tuple_str)),0,(void*)3,loc),_tmp3CE)));dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp205,loc),loc),_tmp206,0);{struct Cyc_List_List*_tmp207=((struct Cyc_List_List*(*)(
void*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_get_second,
_tmp1C1);dict=Cyc_Tcgenrep_buildRepTuple(te,dict,loc,_tmp205,(void*)0,type,
_tmp207);goto _LL17;}}_LL24: if(*((int*)_tmp1B7)!= 17)goto _LL26;_tmp1C2=((struct
Cyc_Absyn_TypedefType_struct*)_tmp1B7)->f1;_tmp1C3=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1B7)->f2;_tmp1C4=((struct Cyc_Absyn_TypedefType_struct*)_tmp1B7)->f3;_tmp1C5=((
struct Cyc_Absyn_TypedefType_struct*)_tmp1B7)->f4;_LL25: if(_tmp1C5 == 0){const char*
_tmp3D1;void*_tmp3D0;(_tmp3D0=0,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp3D1="gen(): can't gen for abstract typedef",
_tag_dynforward(_tmp3D1,sizeof(char),_get_zero_arr_size_char(_tmp3D1,38)))),
_tag_dynforward(_tmp3D0,sizeof(void*),0)));}{struct _tuple19 _tmp20D;struct Cyc_Dict_Dict
_tmp20E;struct Cyc_Tcgenrep_RepInfo*_tmp20F;struct _tuple19*_tmp20C=Cyc_Tcgenrep_lookupRep(
te,dict,loc,*_tmp1C5);_tmp20D=*_tmp20C;_tmp20E=_tmp20D.f1;_tmp20F=_tmp20D.f2;
dict=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Tcgenrep_RepInfo*
v))Cyc_Dict_insert)(_tmp20E,type,_tmp20F);goto _LL17;}_LL26: if((int)_tmp1B7 != 0)
goto _LL28;_LL27:{const char*_tmp3D4;void*_tmp3D3;(_tmp3D3=0,Cyc_Tcutil_terr(loc,((
_tmp3D4="found void in gen() expression",_tag_dynforward(_tmp3D4,sizeof(char),
_get_zero_arr_size_char(_tmp3D4,31)))),_tag_dynforward(_tmp3D3,sizeof(void*),0)));}
goto _LL17;_LL28: if(_tmp1B7 <= (void*)4)goto _LL32;if(*((int*)_tmp1B7)!= 0)goto
_LL2A;_LL29:{const char*_tmp3D7;void*_tmp3D6;(_tmp3D6=0,Cyc_Tcutil_terr(loc,((
_tmp3D7="found evar in gen() expression",_tag_dynforward(_tmp3D7,sizeof(char),
_get_zero_arr_size_char(_tmp3D7,31)))),_tag_dynforward(_tmp3D6,sizeof(void*),0)));}
goto _LL17;_LL2A: if(*((int*)_tmp1B7)!= 1)goto _LL2C;_tmp1C6=((struct Cyc_Absyn_VarType_struct*)
_tmp1B7)->f1;_LL2B:{const char*_tmp3DA;void*_tmp3D9;(_tmp3D9=0,Cyc_Tcutil_terr(
loc,((_tmp3DA="found tyvar in gen() expression",_tag_dynforward(_tmp3DA,sizeof(
char),_get_zero_arr_size_char(_tmp3DA,32)))),_tag_dynforward(_tmp3D9,sizeof(void*),
0)));}goto _LL17;_LL2C: if(*((int*)_tmp1B7)!= 8)goto _LL2E;_tmp1C7=((struct Cyc_Absyn_FnType_struct*)
_tmp1B7)->f1;_LL2D:{const char*_tmp3DD;void*_tmp3DC;(_tmp3DC=0,Cyc_Tcutil_terr(
loc,((_tmp3DD="found function type in gen() expression",_tag_dynforward(_tmp3DD,
sizeof(char),_get_zero_arr_size_char(_tmp3DD,40)))),_tag_dynforward(_tmp3DC,
sizeof(void*),0)));}goto _LL17;_LL2E: if(*((int*)_tmp1B7)!= 15)goto _LL30;_LL2F:
goto _LL31;_LL30: if(*((int*)_tmp1B7)!= 16)goto _LL32;_LL31: goto _LL33;_LL32: if((int)
_tmp1B7 != 3)goto _LL34;_LL33: goto _LL35;_LL34: if((int)_tmp1B7 != 2)goto _LL36;_LL35:
goto _LL37;_LL36: if(_tmp1B7 <= (void*)4)goto _LL38;if(*((int*)_tmp1B7)!= 20)goto
_LL38;_LL37: goto _LL39;_LL38: if(_tmp1B7 <= (void*)4)goto _LL3A;if(*((int*)_tmp1B7)
!= 21)goto _LL3A;_LL39: goto _LL3B;_LL3A: if(_tmp1B7 <= (void*)4)goto _LL3C;if(*((int*)
_tmp1B7)!= 22)goto _LL3C;_LL3B:{const char*_tmp3E0;void*_tmp3DF;(_tmp3DF=0,Cyc_Tcutil_terr(
loc,((_tmp3E0="gen(): unhandled region, handle or effect type",_tag_dynforward(
_tmp3E0,sizeof(char),_get_zero_arr_size_char(_tmp3E0,47)))),_tag_dynforward(
_tmp3DF,sizeof(void*),0)));}goto _LL17;_LL3C: if(_tmp1B7 <= (void*)4)goto _LL3E;if(*((
int*)_tmp1B7)!= 2)goto _LL3E;_tmp1C8=((struct Cyc_Absyn_TunionType_struct*)_tmp1B7)->f1;
_tmp1C9=_tmp1C8.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)_tmp1B7)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LL3E;_tmp1CA=(_tmp1C9.KnownTunion).f1;_tmp1CB=*_tmp1CA;_tmp1CC=_tmp1C8.targs;
_tmp1CD=_tmp1C8.rgn;_LL3D: if(_tmp1CB->tvs != 0){const char*_tmp3E3;void*_tmp3E2;(
_tmp3E2=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmp3E3="gen(): tunion type variables not handled yet",_tag_dynforward(_tmp3E3,
sizeof(char),_get_zero_arr_size_char(_tmp3E3,45)))),_tag_dynforward(_tmp3E2,
sizeof(void*),0)));}if(_tmp1CB->fields == 0){struct _dynforward_ptr*_tmp3E4;struct
_tuple6*_tmp21C=Cyc_Tcgenrep_toplevel_name(((_tmp3E4=_cycalloc(sizeof(*_tmp3E4)),((
_tmp3E4[0]=Cyc_Tcgenrep_make_type_cstring(type),_tmp3E4)))));struct Cyc_Core_Opt*
_tmp3E5;struct Cyc_Core_Opt*_tmp21D=(_tmp3E5=_cycalloc(sizeof(*_tmp3E5)),((
_tmp3E5->v=Cyc_Tcgenrep_gen_vardecl(_tmp21C,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_tunion_str)),
0,(void*)3,loc),_tmp3E5)));dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(_tmp21C,loc),
loc),_tmp21D,1);goto _LL17;}if(!_tmp1CB->is_xtunion){struct _dynforward_ptr*
_tmp3E6;struct _tuple6*_tmp220=Cyc_Tcgenrep_toplevel_name(((_tmp3E6=_cycalloc(
sizeof(*_tmp3E6)),((_tmp3E6[0]=Cyc_Tcgenrep_make_type_cstring(type),_tmp3E6)))));
struct Cyc_Core_Opt*_tmp3E7;struct Cyc_Core_Opt*_tmp221=(_tmp3E7=_cycalloc(sizeof(*
_tmp3E7)),((_tmp3E7->v=Cyc_Tcgenrep_gen_vardecl(_tmp220,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_tunion_str)),0,(void*)3,loc),_tmp3E7)));dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp220,loc),loc),_tmp221,0);{struct Cyc_List_List*_tmp222=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp1CB->fields))->v;struct Cyc_List_List*_tmp223=((
struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*
x))Cyc_List_filter)(Cyc_Tcgenrep_filter_empty_tunionfield,_tmp222);struct Cyc_List_List*
_tmp224=((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Absyn_Tunionfield*),struct
Cyc_List_List*x))Cyc_List_filter)(Cyc_Tcgenrep_filter_nonempty_tunionfield,
_tmp222);struct Cyc_List_List*_tmp225=((struct Cyc_List_List*(*)(struct _tuple18*(*
f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_check_tunionfield_and_get_nmtype,
_tmp223);struct Cyc_List_List*_tmp226=((struct Cyc_List_List*(*)(struct _tuple6*(*f)(
struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_check_tunionfield_and_get_name,
_tmp224);dict=Cyc_Tcgenrep_buildRepTunion(_tmp1CB->name,te,dict,loc,_tmp220,(
void*)2,type,_tmp226,_tmp225);}}else{struct _dynforward_ptr*_tmp3E8;struct _tuple6*
_tmp229=Cyc_Tcgenrep_toplevel_name(((_tmp3E8=_cycalloc(sizeof(*_tmp3E8)),((
_tmp3E8[0]=Cyc_Tcgenrep_make_type_cstring(type),_tmp3E8)))));struct Cyc_Core_Opt*
_tmp3E9;struct Cyc_Core_Opt*_tmp22A=(_tmp3E9=_cycalloc(sizeof(*_tmp3E9)),((
_tmp3E9->v=Cyc_Tcgenrep_gen_vardecl(_tmp229,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_xtunion_str)),
0,(void*)3,loc),_tmp3E9)));dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(_tmp229,loc),
loc),_tmp22A,0);{struct Cyc_List_List*_tmp22B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp1CB->fields))->v;struct Cyc_List_List*_tmp22C=((struct Cyc_List_List*(*)(
int(*f)(struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*x))Cyc_List_filter)(
Cyc_Tcgenrep_filter_empty_tunionfield,_tmp22B);struct Cyc_List_List*_tmp22D=((
struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tunionfield*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_check_xtunionfield_and_get_name_type,
_tmp22C);dict=Cyc_Tcgenrep_buildRepXTunion(_tmp1CB->name,te,dict,loc,_tmp229,(
void*)2,type,_tmp22D);}}goto _LL17;_LL3E: if(_tmp1B7 <= (void*)4)goto _LL40;if(*((
int*)_tmp1B7)!= 2)goto _LL40;_LL3F: {const char*_tmp3EC;void*_tmp3EB;(_tmp3EB=0,((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmp3EC="gen(): tunion must be resolved by now",_tag_dynforward(_tmp3EC,sizeof(
char),_get_zero_arr_size_char(_tmp3EC,38)))),_tag_dynforward(_tmp3EB,sizeof(void*),
0)));}_LL40: if(_tmp1B7 <= (void*)4)goto _LL42;if(*((int*)_tmp1B7)!= 3)goto _LL42;
_tmp1CE=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp1B7)->f1;_LL41: if(_tmp1CE.targs
!= 0){const char*_tmp3EF;void*_tmp3EE;(_tmp3EE=0,((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp3EF="gen(): tunionfield type parameters not handled yet",
_tag_dynforward(_tmp3EF,sizeof(char),_get_zero_arr_size_char(_tmp3EF,51)))),
_tag_dynforward(_tmp3EE,sizeof(void*),0)));}{union Cyc_Absyn_TunionFieldInfoU_union
_tmp234=_tmp1CE.field_info;struct Cyc_Absyn_Tuniondecl*_tmp235;struct Cyc_Absyn_Tunionfield*
_tmp236;_LL5C: if((_tmp234.KnownTunionfield).tag != 1)goto _LL5E;_tmp235=(_tmp234.KnownTunionfield).f1;
_tmp236=(_tmp234.KnownTunionfield).f2;_LL5D: {struct Cyc_List_List*_tmp237=((
struct Cyc_List_List*(*)(void*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_get_second,_tmp236->typs);struct Cyc_List_List*_tmp3F0;struct Cyc_List_List*
_tmp238=(_tmp3F0=_cycalloc(sizeof(*_tmp3F0)),((_tmp3F0->hd=(void*)Cyc_Absyn_uint_typ,((
_tmp3F0->tl=_tmp237,_tmp3F0)))));const char*_tmp3F1;struct _tuple6*_tmp239=Cyc_Tcgenrep_toplevel_name(
Cyc_Tcgenrep_new_gen_id(((_tmp3F1="rep",_tag_dynforward(_tmp3F1,sizeof(char),
_get_zero_arr_size_char(_tmp3F1,4))))));dict=Cyc_Tcgenrep_buildRepTunionfield(
_tmp235->name,_tmp236->name,te,dict,loc,_tmp239,(void*)0,type,_tmp238);goto _LL5B;}
_LL5E:;_LL5F: {const char*_tmp3F4;void*_tmp3F3;(_tmp3F3=0,((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp3F4="gen(): tunionfield must be known at this point",
_tag_dynforward(_tmp3F4,sizeof(char),_get_zero_arr_size_char(_tmp3F4,47)))),
_tag_dynforward(_tmp3F3,sizeof(void*),0)));}_LL5B:;}goto _LL17;_LL42: if(_tmp1B7 <= (
void*)4)goto _LL44;if(*((int*)_tmp1B7)!= 10)goto _LL44;_tmp1CF=((struct Cyc_Absyn_AggrType_struct*)
_tmp1B7)->f1;_tmp1D0=_tmp1CF.aggr_info;_tmp1D1=_tmp1CF.targs;_LL43: {struct Cyc_Absyn_Aggrdecl*
_tmp23E=Cyc_Absyn_get_known_aggrdecl(_tmp1D0);if(_tmp23E->impl != 0  && ((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp23E->impl))->exist_vars != 0){const char*
_tmp3F7;void*_tmp3F6;(_tmp3F6=0,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp3F7="gen(): existential type variables not yet supported",
_tag_dynforward(_tmp3F7,sizeof(char),_get_zero_arr_size_char(_tmp3F7,52)))),
_tag_dynforward(_tmp3F6,sizeof(void*),0)));}if((void*)_tmp23E->kind == (void*)0){
struct Cyc_Absyn_Aggrdecl*_tmp241=_tmp23E;if(_tmp241->impl == 0){struct
_dynforward_ptr*_tmp3F8;struct _tuple6*_tmp242=Cyc_Tcgenrep_toplevel_name(((
_tmp3F8=_cycalloc(sizeof(*_tmp3F8)),((_tmp3F8[0]=Cyc_Tcgenrep_make_type_cstring(
type),_tmp3F8)))));struct Cyc_Core_Opt*_tmp3F9;struct Cyc_Core_Opt*_tmp243=(
_tmp3F9=_cycalloc(sizeof(*_tmp3F9)),((_tmp3F9->v=Cyc_Tcgenrep_gen_vardecl(
_tmp242,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_struct_str)),0,(void*)3,loc),_tmp3F9)));
dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp242,loc),loc),_tmp243,1);goto _LL17;}if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp241->impl))->fields != 0  && ((int(*)(int(*pred)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Tcgenrep_has_bitfield,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp241->impl))->fields)){int _tmp246=((int(*)(int(*f)(int,struct Cyc_Absyn_Aggrfield*),
int accum,struct Cyc_List_List*x))Cyc_List_fold_left)(Cyc_Tcgenrep_add_bitfield_sizes,
0,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp241->impl))->fields);int
_tmp247=_tmp246 / 8 + (_tmp246 % 8 == 0?0: 1);struct Cyc_List_List*chars=0;{int i=0;
for(0;i < _tmp247;++ i){struct _tuple10*_tmp3FC;struct Cyc_List_List*_tmp3FB;chars=((
_tmp3FB=_cycalloc(sizeof(*_tmp3FB)),((_tmp3FB->hd=((_tmp3FC=_cycalloc(sizeof(*
_tmp3FC)),((_tmp3FC->f1=Cyc_Tcgenrep_tq_none,((_tmp3FC->f2=Cyc_Absyn_char_typ,
_tmp3FC)))))),((_tmp3FB->tl=chars,_tmp3FB))))));}}{struct Cyc_Absyn_TupleType_struct
_tmp3FF;struct Cyc_Absyn_TupleType_struct*_tmp3FE;void*base_type=(void*)((_tmp3FE=
_cycalloc(sizeof(*_tmp3FE)),((_tmp3FE[0]=((_tmp3FF.tag=9,((_tmp3FF.f1=chars,
_tmp3FF)))),_tmp3FE))));struct _tuple19 _tmp24B;struct Cyc_Dict_Dict _tmp24C;struct
Cyc_Tcgenrep_RepInfo*_tmp24D;struct _tuple19*_tmp24A=Cyc_Tcgenrep_lookupRep(te,
dict,loc,base_type);_tmp24B=*_tmp24A;_tmp24C=_tmp24B.f1;_tmp24D=_tmp24B.f2;dict=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Tcgenrep_RepInfo*v))
Cyc_Dict_insert)(_tmp24C,type,_tmp24D);}}else{struct _dynforward_ptr*_tmp400;
struct _tuple6*_tmp250=Cyc_Tcgenrep_toplevel_name(((_tmp400=_cycalloc(sizeof(*
_tmp400)),((_tmp400[0]=Cyc_Tcgenrep_make_type_cstring(type),_tmp400)))));struct
Cyc_Core_Opt*_tmp401;struct Cyc_Core_Opt*_tmp251=(_tmp401=_cycalloc(sizeof(*
_tmp401)),((_tmp401->v=Cyc_Tcgenrep_gen_vardecl(_tmp250,Cyc_Tcgenrep_tunionfield_typ(
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_struct_str)),0,(void*)3,loc),_tmp401)));dict=Cyc_Tcgenrep_make_fwd_decl_info(
dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(
_tmp250,loc),loc),_tmp251,0);{struct Cyc_List_List*_tmp252=((struct Cyc_List_List*(*)(
struct _tuple16*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_structfield_nmtype,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp241->impl))->fields);struct Cyc_Core_Opt*_tmp402;dict=Cyc_Tcgenrep_buildRepStruct(((
_tmp402=_cycalloc(sizeof(*_tmp402)),((_tmp402->v=_tmp241->name,_tmp402)))),te,
dict,loc,_tmp250,(void*)2,type,_tmp252);}}}else{struct Cyc_Absyn_Aggrdecl*_tmp256=
_tmp23E;if(_tmp256->tvs != 0){const char*_tmp405;void*_tmp404;(_tmp404=0,((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp405="gen: unions with parameters not yet supported",
_tag_dynforward(_tmp405,sizeof(char),_get_zero_arr_size_char(_tmp405,46)))),
_tag_dynforward(_tmp404,sizeof(void*),0)));}if(_tmp256->impl == 0){struct
_dynforward_ptr*_tmp406;struct _tuple6*_tmp259=Cyc_Tcgenrep_toplevel_name(((
_tmp406=_cycalloc(sizeof(*_tmp406)),((_tmp406[0]=Cyc_Tcgenrep_make_type_cstring(
type),_tmp406)))));struct Cyc_Core_Opt*_tmp407;struct Cyc_Core_Opt*_tmp25A=(
_tmp407=_cycalloc(sizeof(*_tmp407)),((_tmp407->v=Cyc_Tcgenrep_gen_vardecl(
_tmp259,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_typestruct_str),
Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_union_str)),0,(void*)3,loc),_tmp407)));
dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(
Cyc_Absyn_unknownid_exp(_tmp259,loc),loc),_tmp25A,1);goto _LL17;}{struct Cyc_List_List*
_tmp25D=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_select_structfield_nmtype,((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp256->impl))->fields);struct
_dynforward_ptr*_tmp408;struct _tuple6*_tmp25E=Cyc_Tcgenrep_toplevel_name(((
_tmp408=_cycalloc(sizeof(*_tmp408)),((_tmp408[0]=Cyc_Tcgenrep_make_type_cstring(
type),_tmp408)))));struct Cyc_Core_Opt*_tmp409;dict=Cyc_Tcgenrep_buildRepUnion(((
_tmp409=_cycalloc(sizeof(*_tmp409)),((_tmp409->v=_tmp256->name,_tmp409)))),te,
dict,loc,_tmp25E,(void*)2,type,_tmp25D);}}goto _LL17;}_LL44: if(_tmp1B7 <= (void*)4)
goto _LL46;if(*((int*)_tmp1B7)!= 11)goto _LL46;_tmp1D2=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1B7)->f1;if((int)_tmp1D2 != 0)goto _LL46;_tmp1D3=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1B7)->f2;_LL45: if(_tmp1D3 != 0  && ((int(*)(int(*pred)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Tcgenrep_has_bitfield,_tmp1D3)){int
_tmp261=((int(*)(int(*f)(int,struct Cyc_Absyn_Aggrfield*),int accum,struct Cyc_List_List*
x))Cyc_List_fold_left)(Cyc_Tcgenrep_add_bitfield_sizes,0,_tmp1D3);int _tmp262=
_tmp261 / 8 + (_tmp261 % 8 == 0?0: 1);void*base_type=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,
Cyc_Tcgenrep_tq_none,(struct Cyc_Absyn_Exp*)Cyc_Tcgenrep_cnst_int(_tmp262,loc),
Cyc_Absyn_true_conref,0);struct _tuple19 _tmp264;struct Cyc_Dict_Dict _tmp265;struct
Cyc_Tcgenrep_RepInfo*_tmp266;struct _tuple19*_tmp263=Cyc_Tcgenrep_lookupRep(te,
dict,loc,base_type);_tmp264=*_tmp263;_tmp265=_tmp264.f1;_tmp266=_tmp264.f2;dict=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,struct Cyc_Tcgenrep_RepInfo*v))
Cyc_Dict_insert)(_tmp265,type,_tmp266);}else{struct Cyc_List_List*_tmp267=((
struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Aggrfield*),struct
Cyc_List_List*x))Cyc_List_map)(Cyc_Tcgenrep_select_structfield_nmtype,_tmp1D3);
const char*_tmp40A;struct _tuple6*_tmp268=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(((
_tmp40A="rep",_tag_dynforward(_tmp40A,sizeof(char),_get_zero_arr_size_char(
_tmp40A,4))))));dict=Cyc_Tcgenrep_buildRepStruct(0,te,dict,loc,_tmp268,(void*)0,
type,_tmp267);}goto _LL17;_LL46: if(_tmp1B7 <= (void*)4)goto _LL48;if(*((int*)
_tmp1B7)!= 11)goto _LL48;_tmp1D4=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1B7)->f1;if((int)_tmp1D4 != 1)goto _LL48;_tmp1D5=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1B7)->f2;_LL47: {struct Cyc_List_List*_tmp26A=((struct Cyc_List_List*(*)(
struct _tuple16*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_structfield_nmtype,_tmp1D5);const char*_tmp40B;struct _tuple6*
_tmp26B=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(((_tmp40B="rep",
_tag_dynforward(_tmp40B,sizeof(char),_get_zero_arr_size_char(_tmp40B,4))))));
dict=Cyc_Tcgenrep_buildRepUnion(0,te,dict,loc,_tmp26B,(void*)0,type,_tmp26A);
goto _LL17;}_LL48: if(_tmp1B7 <= (void*)4)goto _LL4A;if(*((int*)_tmp1B7)!= 12)goto
_LL4A;_tmp1D6=((struct Cyc_Absyn_EnumType_struct*)_tmp1B7)->f2;_LL49: if(_tmp1D6 == 
0){const char*_tmp40E;void*_tmp40D;(_tmp40D=0,((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp40E="gen(): enum declaration must be present by now",
_tag_dynforward(_tmp40E,sizeof(char),_get_zero_arr_size_char(_tmp40E,47)))),
_tag_dynforward(_tmp40D,sizeof(void*),0)));}{struct Cyc_Absyn_Enumdecl _tmp26F=*
_tmp1D6;if(_tmp26F.fields == 0){struct _dynforward_ptr*_tmp40F;struct _tuple6*
_tmp270=Cyc_Tcgenrep_toplevel_name(((_tmp40F=_cycalloc(sizeof(*_tmp40F)),((
_tmp40F[0]=Cyc_Tcgenrep_make_type_cstring(type),_tmp40F)))));struct Cyc_Core_Opt*
_tmp410;struct Cyc_Core_Opt*_tmp271=(_tmp410=_cycalloc(sizeof(*_tmp410)),((
_tmp410->v=Cyc_Tcgenrep_gen_vardecl(_tmp270,Cyc_Tcgenrep_tunionfield_typ(Cyc_Tcgenrep_typerep_name(&
Cyc_Tcgenrep_typestruct_str),Cyc_Tcgenrep_typerep_name(& Cyc_Tcgenrep_enum_str)),
0,(void*)3,loc),_tmp410)));dict=Cyc_Tcgenrep_make_fwd_decl_info(dict,type,(
struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(Cyc_Absyn_unknownid_exp(_tmp270,loc),
loc),_tmp271,1);goto _LL17;}{struct Cyc_List_List*_tmp274=((struct Cyc_List_List*(*)(
struct _tuple17*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_enumfield_tagnm,(struct Cyc_List_List*)(_tmp26F.fields)->v);
struct _dynforward_ptr*_tmp411;struct _tuple6*_tmp275=Cyc_Tcgenrep_toplevel_name(((
_tmp411=_cycalloc(sizeof(*_tmp411)),((_tmp411[0]=Cyc_Tcgenrep_make_type_cstring(
type),_tmp411)))));dict=Cyc_Tcgenrep_buildRepEnum((struct _tuple6**)& _tmp26F.name,
te,dict,loc,_tmp275,(void*)2,type,_tmp274);goto _LL17;}}_LL4A: if(_tmp1B7 <= (void*)
4)goto _LL4C;if(*((int*)_tmp1B7)!= 13)goto _LL4C;_tmp1D7=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp1B7)->f1;_LL4B: {struct Cyc_List_List*_tmp277=((struct Cyc_List_List*(*)(
struct _tuple17*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Tcgenrep_select_enumfield_tagnm,_tmp1D7);const char*_tmp412;struct _tuple6*
_tmp278=Cyc_Tcgenrep_toplevel_name(Cyc_Tcgenrep_new_gen_id(((_tmp412="rep",
_tag_dynforward(_tmp412,sizeof(char),_get_zero_arr_size_char(_tmp412,4))))));
dict=Cyc_Tcgenrep_buildRepEnum(0,te,dict,loc,_tmp278,(void*)0,type,_tmp277);goto
_LL17;}_LL4C: if(_tmp1B7 <= (void*)4)goto _LL4E;if(*((int*)_tmp1B7)!= 14)goto _LL4E;
_LL4D: {const char*_tmp415;void*_tmp414;(_tmp414=0,((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp415="gen() for sizeof_t<-> not yet supported",
_tag_dynforward(_tmp415,sizeof(char),_get_zero_arr_size_char(_tmp415,40)))),
_tag_dynforward(_tmp414,sizeof(void*),0)));}_LL4E: if(_tmp1B7 <= (void*)4)goto
_LL50;if(*((int*)_tmp1B7)!= 19)goto _LL50;_LL4F: {const char*_tmp418;void*_tmp417;(
_tmp417=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmp418="gen() for `i not yet supported",_tag_dynforward(_tmp418,sizeof(char),
_get_zero_arr_size_char(_tmp418,31)))),_tag_dynforward(_tmp417,sizeof(void*),0)));}
_LL50: if(_tmp1B7 <= (void*)4)goto _LL17;if(*((int*)_tmp1B7)!= 18)goto _LL17;_LL51: {
const char*_tmp41B;void*_tmp41A;(_tmp41A=0,((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp41B="gen() for tag_t<-> not yet supported",
_tag_dynforward(_tmp41B,sizeof(char),_get_zero_arr_size_char(_tmp41B,37)))),
_tag_dynforward(_tmp41A,sizeof(void*),0)));}_LL17:;}{struct _tuple19*_tmp41C;
return(_tmp41C=_cycalloc(sizeof(*_tmp41C)),((_tmp41C->f1=dict,((_tmp41C->f2=((
struct Cyc_Tcgenrep_RepInfo*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(
dict,type),_tmp41C)))));}}static int Cyc_Tcgenrep_not_emitted_filter(struct Cyc_Tcgenrep_RepInfo*
ri);static int Cyc_Tcgenrep_not_emitted_filter(struct Cyc_Tcgenrep_RepInfo*ri){
return ri->emitted == 0;}static void Cyc_Tcgenrep_mark_emitted(struct Cyc_Tcgenrep_RepInfo*
ri);static void Cyc_Tcgenrep_mark_emitted(struct Cyc_Tcgenrep_RepInfo*ri){ri->emitted=
1;}struct _tuple22{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Exp*
f3;};struct _tuple22 Cyc_Tcgenrep_tcGenrep(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*
ge,struct Cyc_Position_Segment*loc,void*type,struct Cyc_Dict_Dict dict);struct
_tuple22 Cyc_Tcgenrep_tcGenrep(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,
struct Cyc_Position_Segment*loc,void*type,struct Cyc_Dict_Dict dict){Cyc_Tcutil_check_valid_toplevel_type(
loc,te,type);Cyc_Tcutil_check_valid_toplevel_type(loc,te,type);{struct _tuple19
_tmp282;struct Cyc_Dict_Dict _tmp283;struct Cyc_Tcgenrep_RepInfo*_tmp284;struct
_tuple19*_tmp281=Cyc_Tcgenrep_lookupRep(te,dict,loc,type);_tmp282=*_tmp281;
_tmp283=_tmp282.f1;_tmp284=_tmp282.f2;{struct Cyc_List_List*_tmp285=Cyc_Tcgenrep_dfs(
_tmp284);struct _tuple22 _tmp41D;return(_tmp41D.f1=_tmp283,((_tmp41D.f2=_tmp285,((
_tmp41D.f3=(struct Cyc_Absyn_Exp*)_check_null(_tmp284->exp),_tmp41D)))));}}}
