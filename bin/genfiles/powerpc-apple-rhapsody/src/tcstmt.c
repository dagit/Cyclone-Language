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
16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct
_dynforward_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char
Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*
x);extern char Cyc_List_Nth[8];struct Cyc_Lineno_Pos{struct _dynforward_ptr
logical_file;struct _dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _dynforward_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dynforward_ptr desc;};
extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Loc_n_struct{int tag;};
struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct
Cyc_Position_Segment*);extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;void*
Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_exn_typ;
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct
Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);struct
_dynforward_ptr Cyc_Absyn_attribute2string(void*);extern int Cyc_Absyn_no_regions;
struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;
};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct
Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dynforward_ptr Cyc_Absynpp_typ2string(void*);struct Cyc_Iter_Iter{
void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,
void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple3{void*f1;void*f2;};struct _tuple3*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple3*Cyc_Dict_rchoose(
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
le;};struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*new_fn);void*Cyc_Tcenv_return_typ(
struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct
_RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(
struct _RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*,struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(
struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct
Cyc_Tcenv_Tenv*te);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct
_RegionHandle*,struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*continue_dest);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct
Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,void*);struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(
struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_get_try_depth(struct
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
void*rgn);void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);void Cyc_Tcenv_check_delayed_constraints(
struct Cyc_Tcenv_Tenv*te);void*Cyc_Tcutil_impos(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap);void*Cyc_Tcutil_copy_type(void*t);
void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_is_pointer_type(
void*t);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;void*Cyc_Tcutil_kind_to_bound(void*
k);void Cyc_Tcutil_explain_failure();int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_substitute(
struct Cyc_List_List*,void*);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
struct _tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple5*Cyc_Tcutil_make_inst_var(
struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void
Cyc_Tcutil_check_type(struct Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct
Cyc_List_List*bound_tvars,void*k,int allow_evars,void*);int Cyc_Tcutil_is_noalias_path(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);int Cyc_Tcutil_new_tvar_id();
int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);struct
Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);void*Cyc_Tcexp_tcExpInitializer(
struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);void Cyc_Tcexp_tcTest(struct
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dynforward_ptr msg_part);extern int
Cyc_Tcexp_in_stmt_exp;struct _tuple6{struct Cyc_List_List*f1;struct Cyc_List_List*
f2;};struct Cyc_Tcpat_TcPatResult{struct _tuple6*tvars_and_bounds_opt;struct Cyc_List_List*
patvars;};struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Pat*p,void**topt);void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_List_List*);int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*,
struct Cyc_List_List*);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
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
f1;};void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*
encloser);void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,
int new_block);static void Cyc_Tcstmt_simplify_unused_result_exp(struct Cyc_Absyn_Exp*
e);static void Cyc_Tcstmt_simplify_unused_result_exp(struct Cyc_Absyn_Exp*e){void*
_tmp0=(void*)e->r;struct Cyc_Absyn_Exp*_tmp1;void*_tmp2;struct Cyc_Absyn_Exp*_tmp3;
void*_tmp4;_LL1: if(*((int*)_tmp0)!= 5)goto _LL3;_tmp1=((struct Cyc_Absyn_Increment_e_struct*)
_tmp0)->f1;_tmp2=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp0)->f2;if((int)
_tmp2 != 1)goto _LL3;_LL2:{struct Cyc_Absyn_Increment_e_struct _tmp14C;struct Cyc_Absyn_Increment_e_struct*
_tmp14B;(void*)(e->r=(void*)((void*)((_tmp14B=_cycalloc(sizeof(*_tmp14B)),((
_tmp14B[0]=((_tmp14C.tag=5,((_tmp14C.f1=_tmp1,((_tmp14C.f2=(void*)((void*)0),
_tmp14C)))))),_tmp14B))))));}goto _LL0;_LL3: if(*((int*)_tmp0)!= 5)goto _LL5;_tmp3=((
struct Cyc_Absyn_Increment_e_struct*)_tmp0)->f1;_tmp4=(void*)((struct Cyc_Absyn_Increment_e_struct*)
_tmp0)->f2;if((int)_tmp4 != 3)goto _LL5;_LL4:{struct Cyc_Absyn_Increment_e_struct
_tmp14F;struct Cyc_Absyn_Increment_e_struct*_tmp14E;(void*)(e->r=(void*)((void*)((
_tmp14E=_cycalloc(sizeof(*_tmp14E)),((_tmp14E[0]=((_tmp14F.tag=5,((_tmp14F.f1=
_tmp3,((_tmp14F.f2=(void*)((void*)2),_tmp14F)))))),_tmp14E))))));}goto _LL0;_LL5:;
_LL6: goto _LL0;_LL0:;}static void Cyc_Tcstmt_decorate_stmt(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Stmt*s);static void Cyc_Tcstmt_decorate_stmt(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*s){Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(
te));s->try_depth=Cyc_Tcenv_get_try_depth(te);}static void Cyc_Tcstmt_pattern_synth(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult
pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int new_block);
static void Cyc_Tcstmt_pattern_synth(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*
where_opt,int new_block){struct _tuple6*_tmpA;struct Cyc_List_List*_tmpB;struct Cyc_Tcpat_TcPatResult
_tmp9=pat_res;_tmpA=_tmp9.tvars_and_bounds_opt;_tmpB=_tmp9.patvars;{struct Cyc_List_List*
_tmpC=_tmpA == 0?0:(*_tmpA).f1;struct Cyc_List_List*_tmpD=_tmpA == 0?0:(*_tmpA).f2;{
struct _RegionHandle _tmpE=_new_region("r");struct _RegionHandle*r=& _tmpE;
_push_region(r);{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r,loc,te,_tmpC);
te2=Cyc_Tcenv_new_outlives_constraints(r,te2,_tmpD,loc);if(new_block)te2=Cyc_Tcenv_new_block(
r,loc,te2);{struct Cyc_Core_Opt*_tmp150;struct Cyc_Core_Opt*_tmpF=(_tmp150=
_cycalloc(sizeof(*_tmp150)),((_tmp150->v=(void*)Cyc_Tcenv_curr_rgn(te2),_tmp150)));{
struct Cyc_List_List*_tmp10=_tmpB;for(0;_tmp10 != 0;_tmp10=_tmp10->tl){te2=Cyc_Tcenv_add_pat_var(
r,loc,te2,(struct Cyc_Absyn_Vardecl*)_tmp10->hd);((struct Cyc_Absyn_Vardecl*)
_tmp10->hd)->rgn=_tmpF;}}if(where_opt != 0){const char*_tmp151;Cyc_Tcexp_tcTest(
te2,(struct Cyc_Absyn_Exp*)where_opt,((_tmp151="switch clause guard",
_tag_dynforward(_tmp151,sizeof(char),_get_zero_arr_size_char(_tmp151,20)))));}
if(_tmpB != 0)te2=Cyc_Tcenv_set_encloser(r,te2,s);Cyc_Tcstmt_tcStmt(te2,s,0);}};
_pop_region(r);}if(_tmpB != 0)Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(
te));}}static struct Cyc_List_List*Cyc_Tcstmt_cmap_c(struct _RegionHandle*r,void*(*
f)(void*,void*),void*env,struct Cyc_Tcenv_CList*x);static struct Cyc_List_List*Cyc_Tcstmt_cmap_c(
struct _RegionHandle*r,void*(*f)(void*,void*),void*env,struct Cyc_Tcenv_CList*x){
if(x == 0)return 0;else{struct Cyc_List_List*_tmp152;return(_tmp152=_region_malloc(
r,sizeof(*_tmp152)),((_tmp152->hd=(void*)f(env,(void*)x->hd),((_tmp152->tl=Cyc_Tcstmt_cmap_c(
r,f,env,x->tl),_tmp152)))));}}void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Stmt*s0,int new_block);void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Stmt*s0,int new_block){Cyc_Tcstmt_decorate_stmt(te,s0);{void*
_tmp14=(void*)s0->r;struct Cyc_Absyn_Exp*_tmp15;struct Cyc_Absyn_Stmt*_tmp16;
struct Cyc_Absyn_Stmt*_tmp17;struct Cyc_Absyn_Exp*_tmp18;struct Cyc_Absyn_Exp*
_tmp19;struct Cyc_Absyn_Stmt*_tmp1A;struct Cyc_Absyn_Stmt*_tmp1B;struct _tuple2
_tmp1C;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_Absyn_Stmt*_tmp1E;struct Cyc_Absyn_Stmt*
_tmp1F;struct Cyc_Absyn_Exp*_tmp20;struct _tuple2 _tmp21;struct Cyc_Absyn_Exp*_tmp22;
struct Cyc_Absyn_Stmt*_tmp23;struct _tuple2 _tmp24;struct Cyc_Absyn_Exp*_tmp25;
struct Cyc_Absyn_Stmt*_tmp26;struct Cyc_Absyn_Stmt*_tmp27;struct Cyc_Absyn_Stmt*
_tmp28;struct _tuple2 _tmp29;struct Cyc_Absyn_Exp*_tmp2A;struct Cyc_Absyn_Stmt*
_tmp2B;struct Cyc_Absyn_Stmt*_tmp2C;struct Cyc_Absyn_Stmt**_tmp2D;struct Cyc_Absyn_Stmt*
_tmp2E;struct Cyc_Absyn_Stmt**_tmp2F;struct _dynforward_ptr*_tmp30;struct Cyc_Absyn_Stmt*
_tmp31;struct Cyc_Absyn_Stmt**_tmp32;struct Cyc_List_List*_tmp33;struct Cyc_Absyn_Switch_clause**
_tmp34;struct Cyc_Absyn_Switch_clause***_tmp35;struct _dynforward_ptr*_tmp36;
struct Cyc_Absyn_Stmt*_tmp37;struct Cyc_Absyn_Exp*_tmp38;struct Cyc_List_List*
_tmp39;struct Cyc_Absyn_Stmt*_tmp3A;struct Cyc_List_List*_tmp3B;struct Cyc_Absyn_Decl*
_tmp3C;struct Cyc_Absyn_Stmt*_tmp3D;struct Cyc_Absyn_Tvar*_tmp3E;struct Cyc_Absyn_Vardecl*
_tmp3F;int _tmp40;struct Cyc_Absyn_Exp*_tmp41;struct Cyc_Absyn_Stmt*_tmp42;struct
Cyc_Absyn_Exp*_tmp43;struct Cyc_Absyn_Exp*_tmp44;struct Cyc_Absyn_Tvar*_tmp45;
struct Cyc_Absyn_Vardecl*_tmp46;struct Cyc_Absyn_Stmt*_tmp47;_LL8: if((int)_tmp14 != 
0)goto _LLA;_LL9: return;_LLA: if(_tmp14 <= (void*)1)goto _LLC;if(*((int*)_tmp14)!= 0)
goto _LLC;_tmp15=((struct Cyc_Absyn_Exp_s_struct*)_tmp14)->f1;_LLB: Cyc_Tcexp_tcExp(
te,0,_tmp15);if(!Cyc_Tcexp_in_stmt_exp)Cyc_Tcstmt_simplify_unused_result_exp(
_tmp15);return;_LLC: if(_tmp14 <= (void*)1)goto _LLE;if(*((int*)_tmp14)!= 1)goto
_LLE;_tmp16=((struct Cyc_Absyn_Seq_s_struct*)_tmp14)->f1;_tmp17=((struct Cyc_Absyn_Seq_s_struct*)
_tmp14)->f2;_LLD:{struct _RegionHandle _tmp48=_new_region("r");struct _RegionHandle*
r=& _tmp48;_push_region(r);{struct Cyc_Tcenv_Stmt_j_struct _tmp155;struct Cyc_Tcenv_Stmt_j_struct*
_tmp154;struct Cyc_Tcenv_Tenv*_tmp49=Cyc_Tcenv_set_next(r,te,(void*)((_tmp154=
_cycalloc(sizeof(*_tmp154)),((_tmp154[0]=((_tmp155.tag=0,((_tmp155.f1=_tmp17,
_tmp155)))),_tmp154)))));Cyc_Tcstmt_tcStmt(_tmp49,_tmp16,1);};_pop_region(r);}
Cyc_Tcstmt_tcStmt(te,_tmp17,1);return;_LLE: if(_tmp14 <= (void*)1)goto _LL10;if(*((
int*)_tmp14)!= 2)goto _LL10;_tmp18=((struct Cyc_Absyn_Return_s_struct*)_tmp14)->f1;
_LLF: {void*t=Cyc_Tcenv_return_typ(te);if(_tmp18 == 0){void*_tmp4C=Cyc_Tcutil_compress(
t);_LL2F: if((int)_tmp4C != 0)goto _LL31;_LL30: goto _LL2E;_LL31:;_LL32:{const char*
_tmp159;void*_tmp158[1];struct Cyc_String_pa_struct _tmp157;(_tmp157.tag=0,((
_tmp157.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp158[0]=& _tmp157,Cyc_Tcutil_terr(s0->loc,((_tmp159="must return a value of type %s",
_tag_dynforward(_tmp159,sizeof(char),_get_zero_arr_size_char(_tmp159,31)))),
_tag_dynforward(_tmp158,sizeof(void*),1)))))));}goto _LL2E;_LL2E:;}else{struct Cyc_Absyn_Exp*
e=(struct Cyc_Absyn_Exp*)_tmp18;Cyc_Tcexp_tcExp(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(
te,e,t)){{const char*_tmp15E;void*_tmp15D[2];struct Cyc_String_pa_struct _tmp15C;
struct Cyc_String_pa_struct _tmp15B;(_tmp15B.tag=0,((_tmp15B.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t)),((_tmp15C.tag=
0,((_tmp15C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp15D[0]=& _tmp15C,((
_tmp15D[1]=& _tmp15B,Cyc_Tcutil_terr(s0->loc,((_tmp15E="returns value of type %s but requires %s",
_tag_dynforward(_tmp15E,sizeof(char),_get_zero_arr_size_char(_tmp15E,41)))),
_tag_dynforward(_tmp15D,sizeof(void*),2)))))))))))));}Cyc_Tcutil_explain_failure();}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !Cyc_Tcutil_is_noalias_path(e)){
const char*_tmp161;void*_tmp160;(_tmp160=0,Cyc_Tcutil_terr(e->loc,((_tmp161="Cannot consume paths; do swap instead",
_tag_dynforward(_tmp161,sizeof(char),_get_zero_arr_size_char(_tmp161,38)))),
_tag_dynforward(_tmp160,sizeof(void*),0)));}}return;}_LL10: if(_tmp14 <= (void*)1)
goto _LL12;if(*((int*)_tmp14)!= 3)goto _LL12;_tmp19=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp14)->f1;_tmp1A=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp14)->f2;_tmp1B=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp14)->f3;_LL11:{const char*_tmp162;Cyc_Tcexp_tcTest(
te,_tmp19,((_tmp162="if statement",_tag_dynforward(_tmp162,sizeof(char),
_get_zero_arr_size_char(_tmp162,13)))));}Cyc_Tcstmt_tcStmt(te,_tmp1A,1);Cyc_Tcstmt_tcStmt(
te,_tmp1B,1);return;_LL12: if(_tmp14 <= (void*)1)goto _LL14;if(*((int*)_tmp14)!= 4)
goto _LL14;_tmp1C=((struct Cyc_Absyn_While_s_struct*)_tmp14)->f1;_tmp1D=_tmp1C.f1;
_tmp1E=_tmp1C.f2;_tmp1F=((struct Cyc_Absyn_While_s_struct*)_tmp14)->f2;_LL13: Cyc_Tcstmt_decorate_stmt(
te,_tmp1E);{const char*_tmp163;Cyc_Tcexp_tcTest(te,_tmp1D,((_tmp163="while loop",
_tag_dynforward(_tmp163,sizeof(char),_get_zero_arr_size_char(_tmp163,11)))));}{
struct _RegionHandle _tmp58=_new_region("r");struct _RegionHandle*r=& _tmp58;
_push_region(r);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(r,te,_tmp1E),_tmp1F,1);;
_pop_region(r);}return;_LL14: if(_tmp14 <= (void*)1)goto _LL16;if(*((int*)_tmp14)!= 
8)goto _LL16;_tmp20=((struct Cyc_Absyn_For_s_struct*)_tmp14)->f1;_tmp21=((struct
Cyc_Absyn_For_s_struct*)_tmp14)->f2;_tmp22=_tmp21.f1;_tmp23=_tmp21.f2;_tmp24=((
struct Cyc_Absyn_For_s_struct*)_tmp14)->f3;_tmp25=_tmp24.f1;_tmp26=_tmp24.f2;
_tmp27=((struct Cyc_Absyn_For_s_struct*)_tmp14)->f4;_LL15: Cyc_Tcstmt_decorate_stmt(
te,_tmp23);Cyc_Tcstmt_decorate_stmt(te,_tmp26);Cyc_Tcexp_tcExp(te,0,_tmp20);{
const char*_tmp164;Cyc_Tcexp_tcTest(te,_tmp22,((_tmp164="for loop",
_tag_dynforward(_tmp164,sizeof(char),_get_zero_arr_size_char(_tmp164,9)))));}{
struct _RegionHandle _tmp5A=_new_region("r");struct _RegionHandle*r=& _tmp5A;
_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp5B=Cyc_Tcenv_set_in_loop(r,te,_tmp26);
Cyc_Tcstmt_tcStmt(_tmp5B,_tmp27,1);Cyc_Tcexp_tcExp(_tmp5B,0,_tmp25);Cyc_Tcstmt_simplify_unused_result_exp(
_tmp25);};_pop_region(r);}return;_LL16: if(_tmp14 <= (void*)1)goto _LL18;if(*((int*)
_tmp14)!= 13)goto _LL18;_tmp28=((struct Cyc_Absyn_Do_s_struct*)_tmp14)->f1;_tmp29=((
struct Cyc_Absyn_Do_s_struct*)_tmp14)->f2;_tmp2A=_tmp29.f1;_tmp2B=_tmp29.f2;_LL17:
Cyc_Tcstmt_decorate_stmt(te,_tmp2B);{struct _RegionHandle _tmp5C=_new_region("r");
struct _RegionHandle*r=& _tmp5C;_push_region(r);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(
r,te,_tmp2B),_tmp28,1);;_pop_region(r);}{const char*_tmp165;Cyc_Tcexp_tcTest(te,
_tmp2A,((_tmp165="do loop",_tag_dynforward(_tmp165,sizeof(char),
_get_zero_arr_size_char(_tmp165,8)))));}return;_LL18: if(_tmp14 <= (void*)1)goto
_LL1A;if(*((int*)_tmp14)!= 5)goto _LL1A;_tmp2C=((struct Cyc_Absyn_Break_s_struct*)
_tmp14)->f1;_tmp2D=(struct Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Break_s_struct*)
_tmp14)->f1;_LL19: Cyc_Tcenv_process_break(te,s0,_tmp2D);return;_LL1A: if(_tmp14 <= (
void*)1)goto _LL1C;if(*((int*)_tmp14)!= 6)goto _LL1C;_tmp2E=((struct Cyc_Absyn_Continue_s_struct*)
_tmp14)->f1;_tmp2F=(struct Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Continue_s_struct*)
_tmp14)->f1;_LL1B: Cyc_Tcenv_process_continue(te,s0,_tmp2F);return;_LL1C: if(
_tmp14 <= (void*)1)goto _LL1E;if(*((int*)_tmp14)!= 7)goto _LL1E;_tmp30=((struct Cyc_Absyn_Goto_s_struct*)
_tmp14)->f1;_tmp31=((struct Cyc_Absyn_Goto_s_struct*)_tmp14)->f2;_tmp32=(struct
Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Goto_s_struct*)_tmp14)->f2;_LL1D: Cyc_Tcenv_process_goto(
te,s0,_tmp30,_tmp32);return;_LL1E: if(_tmp14 <= (void*)1)goto _LL20;if(*((int*)
_tmp14)!= 10)goto _LL20;_tmp33=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp14)->f1;
_tmp34=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp14)->f2;_tmp35=(struct Cyc_Absyn_Switch_clause***)&((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp14)->f2;_LL1F: {struct _tuple4*_tmp5E=Cyc_Tcenv_process_fallthru(
te,s0,_tmp35);if(_tmp5E == 0){{const char*_tmp168;void*_tmp167;(_tmp167=0,Cyc_Tcutil_terr(
s0->loc,((_tmp168="fallthru not in a non-last case",_tag_dynforward(_tmp168,
sizeof(char),_get_zero_arr_size_char(_tmp168,32)))),_tag_dynforward(_tmp167,
sizeof(void*),0)));}return;}{struct Cyc_List_List*_tmp61=(*_tmp5E).f2;struct Cyc_Tcenv_CList*
_tmp62=(*_tmp5E).f3;struct Cyc_List_List*instantiation=((struct Cyc_List_List*(*)(
struct _tuple5*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_make_inst_var,Cyc_Tcenv_lookup_type_vars(
te),_tmp61);struct Cyc_List_List*_tmp63=((struct Cyc_List_List*(*)(struct
_RegionHandle*r,void*(*f)(struct Cyc_List_List*,void*),struct Cyc_List_List*env,
struct Cyc_Tcenv_CList*x))Cyc_Tcstmt_cmap_c)(Cyc_Core_heap_region,Cyc_Tcutil_substitute,
instantiation,_tmp62);for(0;_tmp63 != 0  && _tmp33 != 0;(_tmp63=_tmp63->tl,_tmp33=
_tmp33->tl)){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp33->hd);if(!Cyc_Tcutil_coerce_arg(
te,(struct Cyc_Absyn_Exp*)_tmp33->hd,(void*)_tmp63->hd)){{const char*_tmp16D;void*
_tmp16C[2];struct Cyc_String_pa_struct _tmp16B;struct Cyc_String_pa_struct _tmp16A;(
_tmp16A.tag=0,((_tmp16A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)_tmp63->hd)),((_tmp16B.tag=0,((_tmp16B.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)_tmp33->hd)->topt))->v)),((_tmp16C[0]=& _tmp16B,((_tmp16C[1]=&
_tmp16A,Cyc_Tcutil_terr(s0->loc,((_tmp16D="fallthru argument has type %s but pattern variable has type %s",
_tag_dynforward(_tmp16D,sizeof(char),_get_zero_arr_size_char(_tmp16D,63)))),
_tag_dynforward(_tmp16C,sizeof(void*),2)))))))))))));}Cyc_Tcutil_explain_failure();}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_tmp63->hd) && !Cyc_Tcutil_is_noalias_path((
struct Cyc_Absyn_Exp*)_tmp33->hd)){const char*_tmp170;void*_tmp16F;(_tmp16F=0,Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)_tmp33->hd)->loc,((_tmp170="Cannot consume paths; do swap instead",
_tag_dynforward(_tmp170,sizeof(char),_get_zero_arr_size_char(_tmp170,38)))),
_tag_dynforward(_tmp16F,sizeof(void*),0)));}}if(_tmp33 != 0){const char*_tmp173;
void*_tmp172;(_tmp172=0,Cyc_Tcutil_terr(s0->loc,((_tmp173="too many arguments to explicit fallthru",
_tag_dynforward(_tmp173,sizeof(char),_get_zero_arr_size_char(_tmp173,40)))),
_tag_dynforward(_tmp172,sizeof(void*),0)));}if(_tmp63 != 0){const char*_tmp176;
void*_tmp175;(_tmp175=0,Cyc_Tcutil_terr(s0->loc,((_tmp176="too few arguments to explicit fallthru",
_tag_dynforward(_tmp176,sizeof(char),_get_zero_arr_size_char(_tmp176,39)))),
_tag_dynforward(_tmp175,sizeof(void*),0)));}return;}}_LL20: if(_tmp14 <= (void*)1)
goto _LL22;if(*((int*)_tmp14)!= 12)goto _LL22;_tmp36=((struct Cyc_Absyn_Label_s_struct*)
_tmp14)->f1;_tmp37=((struct Cyc_Absyn_Label_s_struct*)_tmp14)->f2;_LL21:{struct
_RegionHandle _tmp6E=_new_region("r");struct _RegionHandle*r=& _tmp6E;_push_region(
r);{struct _dynforward_ptr*_tmp183;const char*_tmp182;void*_tmp181[1];struct Cyc_String_pa_struct
_tmp180;struct Cyc_Absyn_Tvar*_tmp17F;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_new_named_block(
r,s0->loc,Cyc_Tcenv_add_label(te,_tmp36,_tmp37),((_tmp17F=_cycalloc(sizeof(*
_tmp17F)),((_tmp17F->name=((_tmp183=_cycalloc(sizeof(*_tmp183)),((_tmp183[0]=(
struct _dynforward_ptr)((_tmp180.tag=0,((_tmp180.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*_tmp36),((_tmp181[0]=& _tmp180,Cyc_aprintf(((_tmp182="`%s",
_tag_dynforward(_tmp182,sizeof(char),_get_zero_arr_size_char(_tmp182,4)))),
_tag_dynforward(_tmp181,sizeof(void*),1)))))))),_tmp183)))),((_tmp17F->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp17F->kind=(void*)Cyc_Tcutil_kind_to_bound((void*)
3),_tmp17F))))))))),_tmp37,0);};_pop_region(r);}return;_LL22: if(_tmp14 <= (void*)
1)goto _LL24;if(*((int*)_tmp14)!= 9)goto _LL24;_tmp38=((struct Cyc_Absyn_Switch_s_struct*)
_tmp14)->f1;_tmp39=((struct Cyc_Absyn_Switch_s_struct*)_tmp14)->f2;_LL23: Cyc_Tcexp_tcExp(
te,0,_tmp38);{void*_tmp74=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp38->topt))->v;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp74) && !Cyc_Tcutil_is_noalias_path(
_tmp38)){const char*_tmp186;void*_tmp185;(_tmp185=0,Cyc_Tcutil_terr(_tmp38->loc,((
_tmp186="Cannot consume paths; do swap instead",_tag_dynforward(_tmp186,sizeof(
char),_get_zero_arr_size_char(_tmp186,38)))),_tag_dynforward(_tmp185,sizeof(void*),
0)));}{struct _RegionHandle _tmp77=_new_region("r");struct _RegionHandle*r=& _tmp77;
_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp78=Cyc_Tcenv_set_in_switch(r,te);
_tmp78=Cyc_Tcenv_clear_fallthru(r,_tmp78);{struct Cyc_List_List*scs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp39);for(0;scs != 0;scs=scs->tl){struct
Cyc_Absyn_Pat*_tmp79=((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;struct
Cyc_Tcpat_TcPatResult _tmp7A=Cyc_Tcpat_tcPat(_tmp78,_tmp79,& _tmp74);struct Cyc_List_List*
_tmp7B=_tmp7A.tvars_and_bounds_opt == 0?0:(*_tmp7A.tvars_and_bounds_opt).f1;if(!
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmp79->topt))->v,
_tmp74)){{const char*_tmp18B;void*_tmp18A[2];struct Cyc_String_pa_struct _tmp189;
struct Cyc_String_pa_struct _tmp188;(_tmp188.tag=0,((_tmp188.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp79->topt))->v)),((_tmp189.tag=0,((_tmp189.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp74)),((
_tmp18A[0]=& _tmp189,((_tmp18A[1]=& _tmp188,Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)
scs->hd)->loc,((_tmp18B="switch on type %s, but case expects type %s",
_tag_dynforward(_tmp18B,sizeof(char),_get_zero_arr_size_char(_tmp18B,44)))),
_tag_dynforward(_tmp18A,sizeof(void*),2)))))))))))));}Cyc_Tcutil_explain_failure();}
else{Cyc_Tcpat_check_pat_regions(_tmp78,_tmp79);}{struct Cyc_Core_Opt*_tmp18C;((
struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=((_tmp18C=_cycalloc(sizeof(*
_tmp18C)),((_tmp18C->v=_tmp7A.patvars,_tmp18C))));}Cyc_Tcstmt_pattern_synth(((
struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,_tmp78,_tmp7A,((struct Cyc_Absyn_Switch_clause*)
scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);if(
_tmp7A.tvars_and_bounds_opt != 0  && (*_tmp7A.tvars_and_bounds_opt).f2 != 0)_tmp78=
Cyc_Tcenv_clear_fallthru(r,_tmp78);else{_tmp78=Cyc_Tcenv_set_fallthru(r,_tmp78,
_tmp7B,_tmp7A.patvars,(struct Cyc_Absyn_Switch_clause*)scs->hd);}}}};_pop_region(
r);}Cyc_Tcpat_check_switch_exhaustive(s0->loc,_tmp39);return;}_LL24: if(_tmp14 <= (
void*)1)goto _LL26;if(*((int*)_tmp14)!= 14)goto _LL26;_tmp3A=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp14)->f1;_tmp3B=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp14)->f2;_LL25:{
struct Cyc_Absyn_Seq_s_struct _tmp18F;struct Cyc_Absyn_Seq_s_struct*_tmp18E;(void*)(
_tmp3A->r=(void*)((void*)(Cyc_Absyn_new_stmt((void*)((_tmp18E=_cycalloc(sizeof(*
_tmp18E)),((_tmp18E[0]=((_tmp18F.tag=1,((_tmp18F.f1=Cyc_Absyn_new_stmt((void*)
_tmp3A->r,_tmp3A->loc),((_tmp18F.f2=Cyc_Absyn_skip_stmt(_tmp3A->loc),_tmp18F)))))),
_tmp18E)))),_tmp3A->loc))->r));}{struct _RegionHandle _tmp83=_new_region("r");
struct _RegionHandle*r=& _tmp83;_push_region(r);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
r,Cyc_Tcenv_enter_try(r,te),s0),_tmp3A,1);;_pop_region(r);}{struct _RegionHandle
_tmp84=_new_region("r2");struct _RegionHandle*r2=& _tmp84;_push_region(r2);{struct
Cyc_Tcenv_Tenv*_tmp85=Cyc_Tcenv_set_in_switch(r2,te);_tmp85=Cyc_Tcenv_clear_fallthru(
r2,_tmp85);{struct Cyc_List_List*_tmp86=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(_tmp3B);for(0;_tmp86 != 0;_tmp86=_tmp86->tl){struct Cyc_Absyn_Pat*
_tmp87=((struct Cyc_Absyn_Switch_clause*)_tmp86->hd)->pattern;struct Cyc_Tcpat_TcPatResult
_tmp88=Cyc_Tcpat_tcPat(_tmp85,_tmp87,& Cyc_Absyn_exn_typ);struct Cyc_List_List*
_tmp89=_tmp88.tvars_and_bounds_opt == 0?0:(*_tmp88.tvars_and_bounds_opt).f1;if(!
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmp87->topt))->v,Cyc_Absyn_exn_typ)){
const char*_tmp193;void*_tmp192[1];struct Cyc_String_pa_struct _tmp191;(_tmp191.tag=
0,((_tmp191.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp87->topt))->v)),((_tmp192[0]=&
_tmp191,Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)_tmp86->hd)->loc,((
_tmp193="expected xtunion exn but found %s",_tag_dynforward(_tmp193,sizeof(char),
_get_zero_arr_size_char(_tmp193,34)))),_tag_dynforward(_tmp192,sizeof(void*),1)))))));}
else{Cyc_Tcpat_check_pat_regions(_tmp85,_tmp87);}{struct Cyc_Core_Opt*_tmp194;((
struct Cyc_Absyn_Switch_clause*)_tmp86->hd)->pat_vars=((_tmp194=_cycalloc(sizeof(*
_tmp194)),((_tmp194->v=_tmp88.patvars,_tmp194))));}Cyc_Tcstmt_pattern_synth(((
struct Cyc_Absyn_Switch_clause*)_tmp86->hd)->loc,_tmp85,_tmp88,((struct Cyc_Absyn_Switch_clause*)
_tmp86->hd)->body,((struct Cyc_Absyn_Switch_clause*)_tmp86->hd)->where_clause,1);
if(_tmp88.tvars_and_bounds_opt != 0  && (*_tmp88.tvars_and_bounds_opt).f2 != 0)
_tmp85=Cyc_Tcenv_clear_fallthru(r2,_tmp85);else{_tmp85=Cyc_Tcenv_set_fallthru(r2,
_tmp85,_tmp89,_tmp88.patvars,(struct Cyc_Absyn_Switch_clause*)_tmp86->hd);}}}};
_pop_region(r2);}Cyc_Tcpat_check_catch_overlap(s0->loc,_tmp3B);return;_LL26: if(
_tmp14 <= (void*)1)goto _LL28;if(*((int*)_tmp14)!= 11)goto _LL28;_tmp3C=((struct Cyc_Absyn_Decl_s_struct*)
_tmp14)->f1;_tmp3D=((struct Cyc_Absyn_Decl_s_struct*)_tmp14)->f2;_LL27: {struct
_dynforward_ptr unimp_msg_part;{struct _RegionHandle _tmp8E=_new_region("decl_rgn");
struct _RegionHandle*decl_rgn=& _tmp8E;_push_region(decl_rgn);{struct Cyc_Tcenv_Tenv*
te2;if(new_block)te2=Cyc_Tcenv_new_block(decl_rgn,s0->loc,te);else{te2=Cyc_Tcenv_copy_tenv(
decl_rgn,te);}{struct Cyc_Tcenv_Tenv*_tmp8F=te2;void*_tmp90=(void*)_tmp3C->r;
struct Cyc_Absyn_Vardecl*_tmp91;struct Cyc_Absyn_Pat*_tmp92;struct Cyc_Core_Opt*
_tmp93;struct Cyc_Core_Opt**_tmp94;struct Cyc_Absyn_Exp*_tmp95;struct Cyc_List_List*
_tmp96;struct Cyc_Absyn_Fndecl*_tmp97;struct _dynforward_ptr*_tmp98;struct Cyc_List_List*
_tmp99;struct _tuple0*_tmp9A;struct Cyc_List_List*_tmp9B;_LL34: if(_tmp90 <= (void*)
2)goto _LL4C;if(*((int*)_tmp90)!= 0)goto _LL36;_tmp91=((struct Cyc_Absyn_Var_d_struct*)
_tmp90)->f1;_LL35: {struct Cyc_Absyn_Vardecl _tmp9D;void*_tmp9E;struct _tuple0*
_tmp9F;struct _tuple0 _tmpA0;union Cyc_Absyn_Nmspace_union _tmpA1;struct
_dynforward_ptr*_tmpA2;struct Cyc_Absyn_Tqual _tmpA3;void*_tmpA4;struct Cyc_Absyn_Exp*
_tmpA5;struct Cyc_Core_Opt*_tmpA6;struct Cyc_Core_Opt**_tmpA7;struct Cyc_List_List*
_tmpA8;struct Cyc_Absyn_Vardecl*_tmp9C=_tmp91;_tmp9D=*_tmp9C;_tmp9E=(void*)_tmp9D.sc;
_tmp9F=_tmp9D.name;_tmpA0=*_tmp9F;_tmpA1=_tmpA0.f1;_tmpA2=_tmpA0.f2;_tmpA3=
_tmp9D.tq;_tmpA4=(void*)_tmp9D.type;_tmpA5=_tmp9D.initializer;_tmpA6=_tmp9D.rgn;
_tmpA7=(struct Cyc_Core_Opt**)&(*_tmp9C).rgn;_tmpA8=_tmp9D.attributes;{void*
_tmpA9=Cyc_Tcenv_curr_rgn(_tmp8F);int is_local;{void*_tmpAA=_tmp9E;_LL51: if((int)
_tmpAA != 0)goto _LL53;_LL52: goto _LL54;_LL53: if((int)_tmpAA != 3)goto _LL55;_LL54:
goto _LL56;_LL55: if((int)_tmpAA != 4)goto _LL57;_LL56: is_local=0;goto _LL50;_LL57:
if((int)_tmpAA != 1)goto _LL59;_LL58:{const char*_tmp197;void*_tmp196;(_tmp196=0,
Cyc_Tcutil_terr(_tmp3C->loc,((_tmp197="bad abstract scope for local variable",
_tag_dynforward(_tmp197,sizeof(char),_get_zero_arr_size_char(_tmp197,38)))),
_tag_dynforward(_tmp196,sizeof(void*),0)));}goto _LL5A;_LL59: if((int)_tmpAA != 5)
goto _LL5B;_LL5A: goto _LL5C;_LL5B: if((int)_tmpAA != 2)goto _LL50;_LL5C: is_local=1;
goto _LL50;_LL50:;}{struct Cyc_Core_Opt*_tmp199;struct Cyc_Core_Opt*_tmp198;*_tmpA7=
is_local?(_tmp199=_cycalloc(sizeof(*_tmp199)),((_tmp199->v=(void*)_tmpA9,_tmp199))):((
_tmp198=_cycalloc(sizeof(*_tmp198)),((_tmp198->v=(void*)((void*)2),_tmp198))));}{
union Cyc_Absyn_Nmspace_union _tmpAF=_tmpA1;struct Cyc_List_List*_tmpB0;struct Cyc_List_List*
_tmpB1;_LL5E: if((_tmpAF.Loc_n).tag != 0)goto _LL60;_LL5F: goto _LL5D;_LL60: if((
_tmpAF.Rel_n).tag != 1)goto _LL62;_tmpB0=(_tmpAF.Rel_n).f1;if(_tmpB0 != 0)goto _LL62;
_LL61:{union Cyc_Absyn_Nmspace_union _tmp19A;(*_tmp91->name).f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp19A.Loc_n).tag=0,_tmp19A));}goto _LL5D;_LL62: if((_tmpAF.Abs_n).tag != 2)goto
_LL64;_tmpB1=(_tmpAF.Abs_n).f1;_LL63: {const char*_tmp19D;void*_tmp19C;(int)
_throw(((_tmp19C=0,Cyc_Tcutil_impos(((_tmp19D="tcstmt: Abs_n declaration",
_tag_dynforward(_tmp19D,sizeof(char),_get_zero_arr_size_char(_tmp19D,26)))),
_tag_dynforward(_tmp19C,sizeof(void*),0)))));}_LL64:;_LL65:{const char*_tmp1A0;
void*_tmp19F;(_tmp19F=0,Cyc_Tcutil_terr(_tmp3C->loc,((_tmp1A0="cannot declare a qualified local variable",
_tag_dynforward(_tmp1A0,sizeof(char),_get_zero_arr_size_char(_tmp1A0,42)))),
_tag_dynforward(_tmp19F,sizeof(void*),0)));}goto _LL5D;_LL5D:;}{void*_tmpB7=Cyc_Tcutil_compress(
_tmpA4);struct Cyc_Absyn_ArrayInfo _tmpB8;void*_tmpB9;struct Cyc_Absyn_Tqual _tmpBA;
struct Cyc_Absyn_Exp*_tmpBB;struct Cyc_Absyn_Conref*_tmpBC;struct Cyc_Position_Segment*
_tmpBD;_LL67: if(_tmpB7 <= (void*)4)goto _LL69;if(*((int*)_tmpB7)!= 7)goto _LL69;
_tmpB8=((struct Cyc_Absyn_ArrayType_struct*)_tmpB7)->f1;_tmpB9=(void*)_tmpB8.elt_type;
_tmpBA=_tmpB8.tq;_tmpBB=_tmpB8.num_elts;if(_tmpBB != 0)goto _LL69;_tmpBC=_tmpB8.zero_term;
_tmpBD=_tmpB8.zt_loc;if(!(_tmp91->initializer != 0))goto _LL69;_LL68:{void*_tmpBE=(
void*)((struct Cyc_Absyn_Exp*)_check_null(_tmp91->initializer))->r;union Cyc_Absyn_Cnst_union
_tmpBF;struct _dynforward_ptr _tmpC0;struct Cyc_Absyn_Exp*_tmpC1;struct Cyc_List_List*
_tmpC2;struct Cyc_List_List*_tmpC3;_LL6C: if(*((int*)_tmpBE)!= 0)goto _LL6E;_tmpBF=((
struct Cyc_Absyn_Const_e_struct*)_tmpBE)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpBE)->f1).String_c).tag != 5)goto _LL6E;_tmpC0=(_tmpBF.String_c).f1;_LL6D:
_tmpA4=(void*)(_tmp91->type=(void*)Cyc_Absyn_array_typ(_tmpB9,_tmpBA,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(_get_dynforward_size(_tmpC0,sizeof(char)),0),_tmpBC,_tmpBD));
goto _LL6B;_LL6E: if(*((int*)_tmpBE)!= 29)goto _LL70;_tmpC1=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpBE)->f2;_LL6F: _tmpA4=(void*)(_tmp91->type=(void*)Cyc_Absyn_array_typ(_tmpB9,
_tmpBA,(struct Cyc_Absyn_Exp*)_tmpC1,_tmpBC,_tmpBD));goto _LL6B;_LL70: if(*((int*)
_tmpBE)!= 37)goto _LL72;_tmpC2=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpBE)->f2;
_LL71: _tmpC3=_tmpC2;goto _LL73;_LL72: if(*((int*)_tmpBE)!= 28)goto _LL74;_tmpC3=((
struct Cyc_Absyn_Array_e_struct*)_tmpBE)->f1;_LL73: _tmpA4=(void*)(_tmp91->type=(
void*)Cyc_Absyn_array_typ(_tmpB9,_tmpBA,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC3),0),_tmpBC,
_tmpBD));goto _LL6B;_LL74:;_LL75: goto _LL6B;_LL6B:;}goto _LL66;_LL69:;_LL6A: goto
_LL66;_LL66:;}{struct Cyc_List_List*_tmpC4=!is_local?0: Cyc_Tcenv_lookup_type_vars(
_tmp8F);int _tmpC5=!is_local?0: 1;Cyc_Tcutil_check_type(s0->loc,_tmp8F,_tmpC4,(
void*)1,_tmpC5,_tmpA4);{struct Cyc_Tcenv_Tenv*_tmpC6=Cyc_Tcenv_add_local_var(
decl_rgn,_tmp3D->loc,_tmp8F,_tmp91);if(_tmp9E == (void*)3  || _tmp9E == (void*)4){
const char*_tmp1A3;void*_tmp1A2;(_tmp1A2=0,Cyc_Tcutil_terr(_tmp3C->loc,((_tmp1A3="local extern declarations not yet supported",
_tag_dynforward(_tmp1A3,sizeof(char),_get_zero_arr_size_char(_tmp1A3,44)))),
_tag_dynforward(_tmp1A2,sizeof(void*),0)));}if(_tmpA5 != 0){Cyc_Tcexp_tcExpInitializer(
_tmpC6,(void**)& _tmpA4,(struct Cyc_Absyn_Exp*)_tmpA5);if(!is_local  && !Cyc_Tcutil_is_const_exp(
_tmp8F,(struct Cyc_Absyn_Exp*)_tmpA5)){const char*_tmp1A6;void*_tmp1A5;(_tmp1A5=0,
Cyc_Tcutil_terr(_tmp3C->loc,((_tmp1A6="initializer needs to be a constant expression",
_tag_dynforward(_tmp1A6,sizeof(char),_get_zero_arr_size_char(_tmp1A6,46)))),
_tag_dynforward(_tmp1A5,sizeof(void*),0)));}if(!Cyc_Tcutil_coerce_assign(_tmpC6,(
struct Cyc_Absyn_Exp*)_tmpA5,_tmpA4)){struct _dynforward_ptr _tmpCB=*_tmpA2;struct
_dynforward_ptr _tmpCC=Cyc_Absynpp_typ2string(_tmpA4);struct _dynforward_ptr _tmpCD=
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmpA5->topt))->v);
if(((_get_dynforward_size(_tmpCB,sizeof(char))+ _get_dynforward_size(_tmpCC,
sizeof(char)))+ _get_dynforward_size(_tmpCD,sizeof(char)))+ 50 < 80){const char*
_tmp1AC;void*_tmp1AB[3];struct Cyc_String_pa_struct _tmp1AA;struct Cyc_String_pa_struct
_tmp1A9;struct Cyc_String_pa_struct _tmp1A8;(_tmp1A8.tag=0,((_tmp1A8.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmpCD),((_tmp1A9.tag=0,((_tmp1A9.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmpCC),((_tmp1AA.tag=0,((_tmp1AA.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmpCB),((_tmp1AB[0]=& _tmp1AA,((
_tmp1AB[1]=& _tmp1A9,((_tmp1AB[2]=& _tmp1A8,Cyc_Tcutil_terr(_tmp3C->loc,((_tmp1AC="%s declared with type %s but initialized with type %s.",
_tag_dynforward(_tmp1AC,sizeof(char),_get_zero_arr_size_char(_tmp1AC,55)))),
_tag_dynforward(_tmp1AB,sizeof(void*),3)))))))))))))))))));}else{if((
_get_dynforward_size(_tmpCB,sizeof(char))+ _get_dynforward_size(_tmpCC,sizeof(
char)))+ 25 < 80  && _get_dynforward_size(_tmpCD,sizeof(char))+ 25 < 80){const char*
_tmp1B2;void*_tmp1B1[3];struct Cyc_String_pa_struct _tmp1B0;struct Cyc_String_pa_struct
_tmp1AF;struct Cyc_String_pa_struct _tmp1AE;(_tmp1AE.tag=0,((_tmp1AE.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmpCD),((_tmp1AF.tag=0,((_tmp1AF.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmpCC),((_tmp1B0.tag=0,((_tmp1B0.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmpCB),((_tmp1B1[0]=& _tmp1B0,((
_tmp1B1[1]=& _tmp1AF,((_tmp1B1[2]=& _tmp1AE,Cyc_Tcutil_terr(_tmp3C->loc,((_tmp1B2="%s declared with type %s\n but initialized with type %s.",
_tag_dynforward(_tmp1B2,sizeof(char),_get_zero_arr_size_char(_tmp1B2,56)))),
_tag_dynforward(_tmp1B1,sizeof(void*),3)))))))))))))))))));}else{const char*
_tmp1B8;void*_tmp1B7[3];struct Cyc_String_pa_struct _tmp1B6;struct Cyc_String_pa_struct
_tmp1B5;struct Cyc_String_pa_struct _tmp1B4;(_tmp1B4.tag=0,((_tmp1B4.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmpCD),((_tmp1B5.tag=0,((_tmp1B5.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmpCC),((_tmp1B6.tag=0,((_tmp1B6.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmpCB),((_tmp1B7[0]=& _tmp1B6,((
_tmp1B7[1]=& _tmp1B5,((_tmp1B7[2]=& _tmp1B4,Cyc_Tcutil_terr(_tmp3C->loc,((_tmp1B8="%s declared with type \n%s\n but initialized with type \n%s.",
_tag_dynforward(_tmp1B8,sizeof(char),_get_zero_arr_size_char(_tmp1B8,58)))),
_tag_dynforward(_tmp1B7,sizeof(void*),3)))))))))))))))))));}}Cyc_Tcutil_unify(
_tmpA4,(void*)((struct Cyc_Core_Opt*)_check_null(_tmpA5->topt))->v);Cyc_Tcutil_explain_failure();}}
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,_tmpC6,s0),_tmp3D,0);
_npop_handler(0);return;}}}}_LL36: if(*((int*)_tmp90)!= 2)goto _LL38;_tmp92=((
struct Cyc_Absyn_Let_d_struct*)_tmp90)->f1;_tmp93=((struct Cyc_Absyn_Let_d_struct*)
_tmp90)->f2;_tmp94=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Let_d_struct*)_tmp90)->f2;
_tmp95=((struct Cyc_Absyn_Let_d_struct*)_tmp90)->f3;_LL37: Cyc_Tcexp_tcExpInitializer(
_tmp8F,0,_tmp95);{struct Cyc_Tcpat_TcPatResult _tmpDD=Cyc_Tcpat_tcPat(_tmp8F,
_tmp92,(void**)((void*)&((struct Cyc_Core_Opt*)_check_null(_tmp95->topt))->v));{
struct Cyc_Core_Opt*_tmp1B9;*_tmp94=((_tmp1B9=_cycalloc(sizeof(*_tmp1B9)),((
_tmp1B9->v=_tmpDD.patvars,_tmp1B9))));}if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp92->topt))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp95->topt))->v)
 && !Cyc_Tcutil_coerce_assign(_tmp8F,_tmp95,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp92->topt))->v)){{const char*_tmp1BE;void*_tmp1BD[2];struct Cyc_String_pa_struct
_tmp1BC;struct Cyc_String_pa_struct _tmp1BB;(_tmp1BB.tag=0,((_tmp1BB.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp95->topt))->v)),((_tmp1BC.tag=0,((_tmp1BC.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp92->topt))->v)),((_tmp1BD[0]=& _tmp1BC,((
_tmp1BD[1]=& _tmp1BB,Cyc_Tcutil_terr(_tmp3C->loc,((_tmp1BE="pattern type %s does not match definition type %s",
_tag_dynforward(_tmp1BE,sizeof(char),_get_zero_arr_size_char(_tmp1BE,50)))),
_tag_dynforward(_tmp1BD,sizeof(void*),2)))))))))))));}Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp92->topt))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp95->topt))->v);Cyc_Tcutil_explain_failure();}else{Cyc_Tcpat_check_pat_regions(
_tmp8F,_tmp92);}Cyc_Tcpat_check_let_pat_exhaustive(_tmp92->loc,_tmp92);Cyc_Tcstmt_pattern_synth(
s0->loc,_tmp8F,_tmpDD,_tmp3D,0,0);_npop_handler(0);return;}_LL38: if(*((int*)
_tmp90)!= 3)goto _LL3A;_tmp96=((struct Cyc_Absyn_Letv_d_struct*)_tmp90)->f1;_LL39: {
void*_tmpE3=Cyc_Tcenv_curr_rgn(_tmp8F);struct Cyc_Tcenv_Tenv*_tmpE4=_tmp8F;for(0;
_tmp96 != 0;_tmp96=_tmp96->tl){struct Cyc_Absyn_Vardecl*_tmpE5=(struct Cyc_Absyn_Vardecl*)
_tmp96->hd;struct Cyc_Absyn_Vardecl _tmpE7;struct _tuple0*_tmpE8;struct _tuple0
_tmpE9;union Cyc_Absyn_Nmspace_union _tmpEA;void*_tmpEB;struct Cyc_Core_Opt*_tmpEC;
struct Cyc_Core_Opt**_tmpED;struct Cyc_Absyn_Vardecl*_tmpE6=_tmpE5;_tmpE7=*_tmpE6;
_tmpE8=_tmpE7.name;_tmpE9=*_tmpE8;_tmpEA=_tmpE9.f1;_tmpEB=(void*)_tmpE7.type;
_tmpEC=_tmpE7.rgn;_tmpED=(struct Cyc_Core_Opt**)&(*_tmpE6).rgn;{struct Cyc_Core_Opt*
_tmp1BF;*_tmpED=((_tmp1BF=_cycalloc(sizeof(*_tmp1BF)),((_tmp1BF->v=(void*)_tmpE3,
_tmp1BF))));}{union Cyc_Absyn_Nmspace_union _tmpEF=_tmpEA;struct Cyc_List_List*
_tmpF0;struct Cyc_List_List*_tmpF1;_LL77: if((_tmpEF.Loc_n).tag != 0)goto _LL79;
_LL78: goto _LL76;_LL79: if((_tmpEF.Rel_n).tag != 1)goto _LL7B;_tmpF0=(_tmpEF.Rel_n).f1;
if(_tmpF0 != 0)goto _LL7B;_LL7A:{union Cyc_Absyn_Nmspace_union _tmp1C0;(*_tmpE5->name).f1=(
union Cyc_Absyn_Nmspace_union)(((_tmp1C0.Loc_n).tag=0,_tmp1C0));}goto _LL76;_LL7B:
if((_tmpEF.Abs_n).tag != 2)goto _LL7D;_tmpF1=(_tmpEF.Abs_n).f1;_LL7C: {const char*
_tmp1C3;void*_tmp1C2;(int)_throw(((_tmp1C2=0,Cyc_Tcutil_impos(((_tmp1C3="tcstmt: Abs_n declaration",
_tag_dynforward(_tmp1C3,sizeof(char),_get_zero_arr_size_char(_tmp1C3,26)))),
_tag_dynforward(_tmp1C2,sizeof(void*),0)))));}_LL7D:;_LL7E:{const char*_tmp1C6;
void*_tmp1C5;(_tmp1C5=0,Cyc_Tcutil_terr(_tmp3C->loc,((_tmp1C6="cannot declare a qualified local variable",
_tag_dynforward(_tmp1C6,sizeof(char),_get_zero_arr_size_char(_tmp1C6,42)))),
_tag_dynforward(_tmp1C5,sizeof(void*),0)));}goto _LL76;_LL76:;}Cyc_Tcutil_check_type(
s0->loc,_tmpE4,Cyc_Tcenv_lookup_type_vars(_tmpE4),(void*)1,1,_tmpEB);_tmpE4=Cyc_Tcenv_add_local_var(
decl_rgn,_tmp3D->loc,_tmpE4,_tmpE5);}Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
decl_rgn,_tmpE4,s0),_tmp3D,0);_npop_handler(0);return;}_LL3A: if(*((int*)_tmp90)
!= 1)goto _LL3C;_tmp97=((struct Cyc_Absyn_Fn_d_struct*)_tmp90)->f1;_LL3B: {void*
_tmpF7=Cyc_Tcenv_curr_rgn(_tmp8F);if((void*)_tmp97->sc != (void*)2){const char*
_tmp1C9;void*_tmp1C8;(_tmp1C8=0,Cyc_Tcutil_terr(_tmp3C->loc,((_tmp1C9="bad storage class for inner function",
_tag_dynforward(_tmp1C9,sizeof(char),_get_zero_arr_size_char(_tmp1C9,37)))),
_tag_dynforward(_tmp1C8,sizeof(void*),0)));}{union Cyc_Absyn_Nmspace_union _tmpFA=(*
_tmp97->name).f1;struct Cyc_List_List*_tmpFB;struct Cyc_List_List*_tmpFC;_LL80: if((
_tmpFA.Rel_n).tag != 1)goto _LL82;_tmpFB=(_tmpFA.Rel_n).f1;if(_tmpFB != 0)goto _LL82;
_LL81: goto _LL7F;_LL82: if((_tmpFA.Abs_n).tag != 2)goto _LL84;_tmpFC=(_tmpFA.Abs_n).f1;
_LL83: {const char*_tmp1CC;void*_tmp1CB;(_tmp1CB=0,((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp1CC="tc: Abs_n in tcStmt var decl",
_tag_dynforward(_tmp1CC,sizeof(char),_get_zero_arr_size_char(_tmp1CC,29)))),
_tag_dynforward(_tmp1CB,sizeof(void*),0)));}_LL84:;_LL85: {const char*_tmp1CF;
void*_tmp1CE;(_tmp1CE=0,Cyc_Tcutil_terr(_tmp3C->loc,((_tmp1CF="explicit namespace not allowed on inner function declaration",
_tag_dynforward(_tmp1CF,sizeof(char),_get_zero_arr_size_char(_tmp1CF,61)))),
_tag_dynforward(_tmp1CE,sizeof(void*),0)));}_LL7F:;}{union Cyc_Absyn_Nmspace_union
_tmp1D0;(*_tmp97->name).f1=(union Cyc_Absyn_Nmspace_union)(((_tmp1D0.Loc_n).tag=0,
_tmp1D0));}Cyc_Tcutil_check_fndecl_valid_type(_tmp3C->loc,_tmp8F,_tmp97);{void*t=
Cyc_Tcutil_fndecl2typ(_tmp97);_tmp97->attributes=Cyc_Tcutil_transfer_fn_type_atts(
t,_tmp97->attributes);{struct Cyc_List_List*atts=_tmp97->attributes;for(0;(
unsigned int)atts;atts=atts->tl){void*_tmp102=(void*)atts->hd;_LL87: if((int)
_tmp102 != 5)goto _LL89;_LL88: goto _LL8A;_LL89: if(_tmp102 <= (void*)17)goto _LL8B;if(*((
int*)_tmp102)!= 1)goto _LL8B;_LL8A:{const char*_tmp1D4;void*_tmp1D3[1];struct Cyc_String_pa_struct
_tmp1D2;(_tmp1D2.tag=0,((_tmp1D2.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp1D3[0]=&
_tmp1D2,Cyc_Tcutil_terr(_tmp3C->loc,((_tmp1D4="bad attribute %s for function",
_tag_dynforward(_tmp1D4,sizeof(char),_get_zero_arr_size_char(_tmp1D4,30)))),
_tag_dynforward(_tmp1D3,sizeof(void*),1)))))));}goto _LL86;_LL8B:;_LL8C: goto _LL86;
_LL86:;}}{struct Cyc_Core_Opt*_tmp1D7;struct Cyc_Absyn_Vardecl*_tmp1D6;struct Cyc_Absyn_Vardecl*
vd=(_tmp1D6=_cycalloc(sizeof(*_tmp1D6)),((_tmp1D6->sc=(void*)((void*)_tmp97->sc),((
_tmp1D6->name=_tmp97->name,((_tmp1D6->tq=Cyc_Absyn_const_tqual(0),((_tmp1D6->type=(
void*)Cyc_Absyn_at_typ(t,_tmpF7,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),((
_tmp1D6->initializer=0,((_tmp1D6->rgn=((_tmp1D7=_cycalloc(sizeof(*_tmp1D7)),((
_tmp1D7->v=(void*)_tmpF7,_tmp1D7)))),((_tmp1D6->attributes=0,((_tmp1D6->escapes=
0,_tmp1D6)))))))))))))))));_tmp97->fn_vardecl=(struct Cyc_Absyn_Vardecl*)vd;
_tmp8F=Cyc_Tcenv_add_local_var(decl_rgn,_tmp3C->loc,_tmp8F,vd);{struct Cyc_Tcenv_Fenv*
old_fenv=(struct Cyc_Tcenv_Fenv*)_check_null(_tmp8F->le);_tmp8F->le=(struct Cyc_Tcenv_Fenv*)
Cyc_Tcenv_nested_fenv(_tmp3C->loc,old_fenv,_tmp97);Cyc_Tcstmt_tcStmt(_tmp8F,
_tmp97->body,0);Cyc_Tcenv_check_delayed_effects(_tmp8F);Cyc_Tcenv_check_delayed_constraints(
_tmp8F);if(!Cyc_Tcenv_all_labels_resolved(_tmp8F)){const char*_tmp1DA;void*
_tmp1D9;(_tmp1D9=0,Cyc_Tcutil_terr(_tmp3C->loc,((_tmp1DA="function has goto statements to undefined labels",
_tag_dynforward(_tmp1DA,sizeof(char),_get_zero_arr_size_char(_tmp1DA,49)))),
_tag_dynforward(_tmp1D9,sizeof(void*),0)));}_tmp8F->le=(struct Cyc_Tcenv_Fenv*)
old_fenv;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,_tmp8F,s0),_tmp3D,0);
_npop_handler(0);return;}}}}_LL3C: if(*((int*)_tmp90)!= 8)goto _LL3E;_tmp98=((
struct Cyc_Absyn_Namespace_d_struct*)_tmp90)->f1;_tmp99=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp90)->f2;_LL3D:{const char*_tmp1DB;unimp_msg_part=((_tmp1DB="namespace",
_tag_dynforward(_tmp1DB,sizeof(char),_get_zero_arr_size_char(_tmp1DB,10))));}
goto _LL33;_LL3E: if(*((int*)_tmp90)!= 9)goto _LL40;_tmp9A=((struct Cyc_Absyn_Using_d_struct*)
_tmp90)->f1;_tmp9B=((struct Cyc_Absyn_Using_d_struct*)_tmp90)->f2;_LL3F:{const
char*_tmp1DC;unimp_msg_part=((_tmp1DC="using",_tag_dynforward(_tmp1DC,sizeof(
char),_get_zero_arr_size_char(_tmp1DC,6))));}goto _LL33;_LL40: if(*((int*)_tmp90)
!= 4)goto _LL42;_LL41:{const char*_tmp1DD;unimp_msg_part=((_tmp1DD="type",
_tag_dynforward(_tmp1DD,sizeof(char),_get_zero_arr_size_char(_tmp1DD,5))));}goto
_LL33;_LL42: if(*((int*)_tmp90)!= 5)goto _LL44;_LL43:{const char*_tmp1DE;
unimp_msg_part=((_tmp1DE="[x]tunion",_tag_dynforward(_tmp1DE,sizeof(char),
_get_zero_arr_size_char(_tmp1DE,10))));}goto _LL33;_LL44: if(*((int*)_tmp90)!= 6)
goto _LL46;_LL45:{const char*_tmp1DF;unimp_msg_part=((_tmp1DF="enum",
_tag_dynforward(_tmp1DF,sizeof(char),_get_zero_arr_size_char(_tmp1DF,5))));}goto
_LL33;_LL46: if(*((int*)_tmp90)!= 7)goto _LL48;_LL47:{const char*_tmp1E0;
unimp_msg_part=((_tmp1E0="typedef",_tag_dynforward(_tmp1E0,sizeof(char),
_get_zero_arr_size_char(_tmp1E0,8))));}goto _LL33;_LL48: if(*((int*)_tmp90)!= 10)
goto _LL4A;_LL49:{const char*_tmp1E1;unimp_msg_part=((_tmp1E1="extern \"C\"",
_tag_dynforward(_tmp1E1,sizeof(char),_get_zero_arr_size_char(_tmp1E1,11))));}
goto _LL33;_LL4A: if(*((int*)_tmp90)!= 11)goto _LL4C;_LL4B:{const char*_tmp1E2;
unimp_msg_part=((_tmp1E2="extern \"C include\"",_tag_dynforward(_tmp1E2,sizeof(
char),_get_zero_arr_size_char(_tmp1E2,19))));}goto _LL33;_LL4C: if((int)_tmp90 != 0)
goto _LL4E;_LL4D:{const char*_tmp1E3;unimp_msg_part=((_tmp1E3="__cyclone_port_on__",
_tag_dynforward(_tmp1E3,sizeof(char),_get_zero_arr_size_char(_tmp1E3,20))));}
goto _LL33;_LL4E: if((int)_tmp90 != 1)goto _LL33;_LL4F:{const char*_tmp1E4;
unimp_msg_part=((_tmp1E4="__cyclone_port_off__",_tag_dynforward(_tmp1E4,sizeof(
char),_get_zero_arr_size_char(_tmp1E4,21))));}goto _LL33;_LL33:;}};_pop_region(
decl_rgn);}{const char*_tmp1E8;void*_tmp1E7[1];struct Cyc_String_pa_struct _tmp1E6;(
int)_throw(((_tmp1E6.tag=0,((_tmp1E6.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)unimp_msg_part),((_tmp1E7[0]=& _tmp1E6,Cyc_Tcutil_impos(((_tmp1E8="tcStmt: nested %s declarations unimplemented",
_tag_dynforward(_tmp1E8,sizeof(char),_get_zero_arr_size_char(_tmp1E8,45)))),
_tag_dynforward(_tmp1E7,sizeof(void*),1)))))))));}}_LL28: if(_tmp14 <= (void*)1)
goto _LL2A;if(*((int*)_tmp14)!= 15)goto _LL2A;_tmp3E=((struct Cyc_Absyn_Region_s_struct*)
_tmp14)->f1;_tmp3F=((struct Cyc_Absyn_Region_s_struct*)_tmp14)->f2;_tmp40=((
struct Cyc_Absyn_Region_s_struct*)_tmp14)->f3;_tmp41=((struct Cyc_Absyn_Region_s_struct*)
_tmp14)->f4;_tmp42=((struct Cyc_Absyn_Region_s_struct*)_tmp14)->f5;_LL29:{struct
Cyc_Absyn_Seq_s_struct _tmp1EB;struct Cyc_Absyn_Seq_s_struct*_tmp1EA;(void*)(
_tmp42->r=(void*)((void*)(Cyc_Absyn_new_stmt((void*)((_tmp1EA=_cycalloc(sizeof(*
_tmp1EA)),((_tmp1EA[0]=((_tmp1EB.tag=1,((_tmp1EB.f1=Cyc_Absyn_new_stmt((void*)
_tmp42->r,_tmp42->loc),((_tmp1EB.f2=Cyc_Absyn_skip_stmt(_tmp42->loc),_tmp1EB)))))),
_tmp1EA)))),_tmp42->loc))->r));}{struct _RegionHandle _tmp119=_new_region("r");
struct _RegionHandle*r=& _tmp119;_push_region(r);{struct Cyc_Tcenv_Tenv*te2;if(
new_block)te2=Cyc_Tcenv_new_block(r,s0->loc,te);else{te2=Cyc_Tcenv_copy_tenv(r,
te);}{struct Cyc_Absyn_Vardecl _tmp11B;void*_tmp11C;void**_tmp11D;struct Cyc_Core_Opt*
_tmp11E;struct Cyc_Core_Opt**_tmp11F;struct Cyc_Absyn_Vardecl*_tmp11A=_tmp3F;
_tmp11B=*_tmp11A;_tmp11C=(void*)_tmp11B.type;_tmp11D=(void**)&(*_tmp11A).type;
_tmp11E=_tmp11B.rgn;_tmp11F=(struct Cyc_Core_Opt**)&(*_tmp11A).rgn;{void*_tmp120=
Cyc_Tcenv_curr_rgn(te2);{struct Cyc_Core_Opt*_tmp1EC;*_tmp11F=((_tmp1EC=_cycalloc(
sizeof(*_tmp1EC)),((_tmp1EC->v=(void*)_tmp120,_tmp1EC))));}{void*rgn_typ;if(
_tmp41 != 0){struct Cyc_Absyn_Exp*open_exp=(struct Cyc_Absyn_Exp*)_tmp41;struct Cyc_List_List*
_tmp122=Cyc_Tcenv_lookup_type_vars(te);{struct Cyc_Core_Opt*_tmp1ED;rgn_typ=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,((_tmp1ED=_cycalloc(sizeof(*_tmp1ED)),((
_tmp1ED->v=_tmp122,_tmp1ED)))));}{struct Cyc_Core_Opt*_tmp1EE;void*rgn2_typ=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,((_tmp1EE=_cycalloc(sizeof(*_tmp1EE)),((
_tmp1EE->v=_tmp122,_tmp1EE)))));struct Cyc_Absyn_DynRgnType_struct _tmp1F1;struct
Cyc_Absyn_DynRgnType_struct*_tmp1F0;void*expected=(void*)((_tmp1F0=_cycalloc(
sizeof(*_tmp1F0)),((_tmp1F0[0]=((_tmp1F1.tag=16,((_tmp1F1.f1=(void*)rgn_typ,((
_tmp1F1.f2=(void*)rgn2_typ,_tmp1F1)))))),_tmp1F0))));{struct Cyc_Absyn_RgnHandleType_struct
_tmp1F4;struct Cyc_Absyn_RgnHandleType_struct*_tmp1F3;*_tmp11D=(void*)((_tmp1F3=
_cycalloc(sizeof(*_tmp1F3)),((_tmp1F3[0]=((_tmp1F4.tag=15,((_tmp1F4.f1=(void*)
rgn_typ,_tmp1F4)))),_tmp1F3))));}if(!Cyc_Tcutil_unify(Cyc_Tcexp_tcExp(te,(void**)&
expected,open_exp),expected)){const char*_tmp1F9;void*_tmp1F8[2];struct Cyc_String_pa_struct
_tmp1F7;struct Cyc_String_pa_struct _tmp1F6;(_tmp1F6.tag=0,((_tmp1F6.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(open_exp->topt))->v)),((_tmp1F7.tag=0,((_tmp1F7.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(expected)),((
_tmp1F8[0]=& _tmp1F7,((_tmp1F8[1]=& _tmp1F6,Cyc_Tcutil_terr(open_exp->loc,((
_tmp1F9="expecting %s but found %s",_tag_dynforward(_tmp1F9,sizeof(char),
_get_zero_arr_size_char(_tmp1F9,26)))),_tag_dynforward(_tmp1F8,sizeof(void*),2)))))))))))));}
Cyc_Tcenv_check_rgn_accessible(te,open_exp->loc,rgn2_typ);te2=Cyc_Tcenv_add_region(
r,te2,rgn_typ,0,1);}}else{{struct Cyc_Absyn_VarType_struct _tmp1FC;struct Cyc_Absyn_VarType_struct*
_tmp1FB;rgn_typ=(void*)((_tmp1FB=_cycalloc(sizeof(*_tmp1FB)),((_tmp1FB[0]=((
_tmp1FC.tag=1,((_tmp1FC.f1=_tmp3E,_tmp1FC)))),_tmp1FB))));}{struct Cyc_List_List*
_tmp1FD;te2=Cyc_Tcenv_add_type_vars(r,s0->loc,te2,((_tmp1FD=_cycalloc(sizeof(*
_tmp1FD)),((_tmp1FD->hd=_tmp3E,((_tmp1FD->tl=0,_tmp1FD)))))));}te2=Cyc_Tcenv_add_region(
r,te2,rgn_typ,_tmp40,1);}Cyc_Tcutil_check_type(s0->loc,te,Cyc_Tcenv_lookup_type_vars(
te2),(void*)2,1,*_tmp11D);{struct Cyc_Absyn_RgnHandleType_struct _tmp200;struct Cyc_Absyn_RgnHandleType_struct*
_tmp1FF;if(!Cyc_Tcutil_unify(*_tmp11D,(void*)((_tmp1FF=_cycalloc(sizeof(*_tmp1FF)),((
_tmp1FF[0]=((_tmp200.tag=15,((_tmp200.f1=(void*)rgn_typ,_tmp200)))),_tmp1FF)))))){
const char*_tmp203;void*_tmp202;(_tmp202=0,((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp203="region stmt: type of region handle is wrong!",
_tag_dynforward(_tmp203,sizeof(char),_get_zero_arr_size_char(_tmp203,45)))),
_tag_dynforward(_tmp202,sizeof(void*),0)));}}if(!Cyc_Absyn_no_regions)te2=Cyc_Tcenv_enter_try(
r,te2);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(r,Cyc_Tcenv_add_local_var(r,
_tmp42->loc,te2,_tmp3F),s0),_tmp42,0);}}}};_pop_region(r);}return;_LL2A: if(
_tmp14 <= (void*)1)goto _LL2C;if(*((int*)_tmp14)!= 16)goto _LL2C;_tmp43=((struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp14)->f1;_LL2B: {void*rgn_type=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,
Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_RgnHandleType_struct _tmp206;
struct Cyc_Absyn_RgnHandleType_struct*_tmp205;void*etype=(void*)((_tmp205=
_cycalloc(sizeof(*_tmp205)),((_tmp205[0]=((_tmp206.tag=15,((_tmp206.f1=(void*)
rgn_type,_tmp206)))),_tmp205))));if(!Cyc_Tcutil_unify(Cyc_Tcexp_tcExp(te,(void**)&
etype,_tmp43),etype)){const char*_tmp20A;void*_tmp209[1];struct Cyc_String_pa_struct
_tmp208;(_tmp208.tag=0,((_tmp208.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp43->topt))->v)),((_tmp209[0]=& _tmp208,Cyc_Tcutil_terr(_tmp43->loc,((_tmp20A="expecting region_t but found %s",
_tag_dynforward(_tmp20A,sizeof(char),_get_zero_arr_size_char(_tmp20A,32)))),
_tag_dynforward(_tmp209,sizeof(void*),1)))))));}Cyc_Tcenv_check_rgn_resetable(te,
s0->loc,rgn_type);return;}_LL2C: if(_tmp14 <= (void*)1)goto _LL7;if(*((int*)_tmp14)
!= 17)goto _LL7;_tmp44=((struct Cyc_Absyn_Alias_s_struct*)_tmp14)->f1;_tmp45=((
struct Cyc_Absyn_Alias_s_struct*)_tmp14)->f2;_tmp46=((struct Cyc_Absyn_Alias_s_struct*)
_tmp14)->f3;_tmp47=((struct Cyc_Absyn_Alias_s_struct*)_tmp14)->f4;_LL2D: Cyc_Tcexp_tcExpInitializer(
te,0,_tmp44);if(!Cyc_Tcutil_is_pointer_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp44->topt))->v)){const char*_tmp20E;void*_tmp20D[1];struct Cyc_String_pa_struct
_tmp20C;(_tmp20C.tag=0,((_tmp20C.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp44->topt))->v)),((_tmp20D[0]=& _tmp20C,Cyc_Tcutil_terr(_tmp44->loc,((_tmp20E="%s is not a pointer type",
_tag_dynforward(_tmp20E,sizeof(char),_get_zero_arr_size_char(_tmp20E,25)))),
_tag_dynforward(_tmp20D,sizeof(void*),1)))))));}(void*)(_tmp46->type=(void*)Cyc_Tcutil_copy_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp44->topt))->v));{void*_tmp13C=(void*)
_tmp46->type;struct Cyc_Absyn_PtrInfo _tmp13D;struct Cyc_Absyn_PtrAtts _tmp13E;void*
_tmp13F;void**_tmp140;_LL8E: if(_tmp13C <= (void*)4)goto _LL90;if(*((int*)_tmp13C)
!= 4)goto _LL90;_tmp13D=((struct Cyc_Absyn_PointerType_struct*)_tmp13C)->f1;
_tmp13E=_tmp13D.ptr_atts;_tmp13F=(void*)_tmp13E.rgn;_tmp140=(void**)&((((struct
Cyc_Absyn_PointerType_struct*)_tmp13C)->f1).ptr_atts).rgn;_LL8F:{struct Cyc_Absyn_VarType_struct
_tmp211;struct Cyc_Absyn_VarType_struct*_tmp210;*_tmp140=(void*)((_tmp210=
_cycalloc(sizeof(*_tmp210)),((_tmp210[0]=((_tmp211.tag=1,((_tmp211.f1=_tmp45,
_tmp211)))),_tmp210))));}goto _LL8D;_LL90:;_LL91: {const char*_tmp214;void*_tmp213;(
_tmp213=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmp214="Not a pointer type in tcstmt:Alias_s",_tag_dynforward(_tmp214,sizeof(
char),_get_zero_arr_size_char(_tmp214,37)))),_tag_dynforward(_tmp213,sizeof(void*),
0)));}_LL8D:;}{struct Cyc_Core_Opt*_tmp215;_tmp46->rgn=((_tmp215=_cycalloc(
sizeof(*_tmp215)),((_tmp215->v=(void*)Cyc_Tcenv_curr_rgn(te),_tmp215))));}{
struct _RegionHandle _tmp146=_new_region("r");struct _RegionHandle*r=& _tmp146;
_push_region(r);{struct Cyc_Absyn_Tvar*_tmp216[1];struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(
r,s0->loc,te,((_tmp216[0]=_tmp45,((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dynforward_ptr))Cyc_List_rlist)(r,_tag_dynforward(_tmp216,sizeof(struct
Cyc_Absyn_Tvar*),1)))));{struct Cyc_Absyn_VarType_struct _tmp219;struct Cyc_Absyn_VarType_struct*
_tmp218;te2=Cyc_Tcenv_add_region(r,te2,(void*)((_tmp218=_cycalloc(sizeof(*
_tmp218)),((_tmp218[0]=((_tmp219.tag=1,((_tmp219.f1=_tmp45,_tmp219)))),_tmp218)))),
0,1);}Cyc_Tcutil_check_type(s0->loc,te,Cyc_Tcenv_lookup_type_vars(te2),Cyc_Tcutil_typ_kind((
void*)_tmp46->type),1,(void*)_tmp46->type);te2=Cyc_Tcenv_add_local_var(r,s0->loc,
te2,_tmp46);te2=Cyc_Tcenv_set_encloser(r,te2,_tmp47);Cyc_Tcstmt_tcStmt(te2,
_tmp47,0);};_pop_region(r);}Cyc_NewControlFlow_set_encloser(s0,Cyc_Tcenv_get_encloser(
te));return;_LL7:;}}
