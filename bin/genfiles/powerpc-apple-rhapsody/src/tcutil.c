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
void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_fst(struct _tuple0*);int
Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
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
struct _dynforward_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct
Cyc___cycFILE*,struct _dynforward_ptr,struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dynforward_ptr f1;}
;struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
int Cyc_printf(struct _dynforward_ptr,struct _dynforward_ptr);struct _dynforward_ptr
Cyc_vrprintf(struct _RegionHandle*,struct _dynforward_ptr,struct _dynforward_ptr);
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_list(struct _dynforward_ptr);int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*
Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_revappend(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(
struct Cyc_List_List*x,int n);int Cyc_List_exists_c(int(*pred)(void*,void*),void*
env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*
r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct
_tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple1 Cyc_List_rsplit(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);int Cyc_List_mem(
int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);void*Cyc_List_assoc_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);int Cyc_List_list_cmp(int(*
cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{
struct _dynforward_ptr logical_file;struct _dynforward_ptr line;int line_no;int col;};
extern char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_List_List*
Cyc_Position_strings_of_segments(struct Cyc_List_List*);struct Cyc_Position_Error{
struct _dynforward_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dynforward_ptr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_elab(struct
Cyc_Position_Segment*,struct _dynforward_ptr);extern char Cyc_Position_Nocontext[14];
extern int Cyc_Position_num_errors;extern int Cyc_Position_max_errors;void Cyc_Position_post_error(
struct Cyc_Position_Error*);struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*
f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};struct _tuple2{union Cyc_Absyn_Nmspace_union
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
_tuple2*name;int is_xtunion;int is_flat;};struct Cyc_Absyn_UnknownTunion_struct{int
tag;struct Cyc_Absyn_UnknownTunionInfo f1;};struct Cyc_Absyn_KnownTunion_struct{int
tag;struct Cyc_Absyn_Tuniondecl**f1;};union Cyc_Absyn_TunionInfoU_union{struct Cyc_Absyn_UnknownTunion_struct
UnknownTunion;struct Cyc_Absyn_KnownTunion_struct KnownTunion;};struct Cyc_Absyn_TunionInfo{
union Cyc_Absyn_TunionInfoU_union tunion_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
rgn;};struct Cyc_Absyn_UnknownTunionFieldInfo{struct _tuple2*tunion_name;struct
_tuple2*field_name;int is_xtunion;};struct Cyc_Absyn_UnknownTunionfield_struct{int
tag;struct Cyc_Absyn_UnknownTunionFieldInfo f1;};struct Cyc_Absyn_KnownTunionfield_struct{
int tag;struct Cyc_Absyn_Tuniondecl*f1;struct Cyc_Absyn_Tunionfield*f2;};union Cyc_Absyn_TunionFieldInfoU_union{
struct Cyc_Absyn_UnknownTunionfield_struct UnknownTunionfield;struct Cyc_Absyn_KnownTunionfield_struct
KnownTunionfield;};struct Cyc_Absyn_TunionFieldInfo{union Cyc_Absyn_TunionFieldInfoU_union
field_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownAggr_struct{int tag;
void*f1;struct _tuple2*f2;};struct Cyc_Absyn_KnownAggr_struct{int tag;struct Cyc_Absyn_Aggrdecl**
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
int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_SizeofType_struct{int tag;void*f1;
};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple2*
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
_tuple2*f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple2*f1;
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
struct Cyc_List_List*f1;};struct _tuple3{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple3*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Struct_e_struct{
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Tunion_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*
f2;struct Cyc_Absyn_Tunionfield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct
_tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{
int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;
void*r;struct Cyc_Position_Segment*loc;void*annot;};struct Cyc_Absyn_Exp_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*
f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple4{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_While_s_struct{int tag;struct _tuple4 f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Break_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct
_dynforward_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct _tuple4 f2;struct _tuple4 f3;struct Cyc_Absyn_Stmt*f4;}
;struct Cyc_Absyn_Switch_s_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Fallthru_s_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**
f2;};struct Cyc_Absyn_Decl_s_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Label_s_struct{int tag;struct _dynforward_ptr*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Do_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple4 f2;
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
f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_struct{
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*
topt;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*
pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*
body;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Global_b_struct{int tag;
struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Param_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Local_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{void*sc;struct
_tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;
struct Cyc_Core_Opt*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{
void*sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
effect;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dynforward_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;};struct Cyc_Absyn_Aggrdecl{void*
kind;void*sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Tunionfield{struct _tuple2*
name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};struct Cyc_Absyn_Tuniondecl{
void*sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int
is_xtunion;int is_flat;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*
tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{void*sc;struct
_tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct
_tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Aggr_d_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct
Cyc_Absyn_Tunion_d_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;};struct Cyc_Absyn_Enum_d_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;
struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_struct{int tag;
struct _dynforward_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple2*,struct _tuple2*);int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*);struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);struct
Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(struct Cyc_Absyn_Conref*x);void*
Cyc_Absyn_conref_val(struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,
struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_one_conref;void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;extern void*Cyc_Absyn_sint_typ;
extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;extern void*Cyc_Absyn_empty_effect;
extern struct _tuple2*Cyc_Absyn_tunion_print_arg_qvar;extern struct _tuple2*Cyc_Absyn_tunion_scanf_arg_qvar;
extern void*Cyc_Absyn_bounds_one;void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual
tq,void*b,struct Cyc_Absyn_Conref*zero_term);struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct
Cyc_List_List*,struct _dynforward_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dynforward_ptr*);struct _tuple5{struct Cyc_Absyn_Tqual
f1;void*f2;};struct _tuple5*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
struct _dynforward_ptr Cyc_Absyn_attribute2string(void*);int Cyc_Absyn_fntype_att(
void*a);struct _tuple6{void*f1;struct _tuple2*f2;};struct _tuple6 Cyc_Absyn_aggr_kinded_name(
union Cyc_Absyn_AggrInfoU_union);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(
union Cyc_Absyn_AggrInfoU_union info);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct
Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dynforward_ptr Cyc_Absynpp_typ2string(void*);struct
_dynforward_ptr Cyc_Absynpp_kind2string(void*);struct _dynforward_ptr Cyc_Absynpp_kindbound2string(
void*);struct _dynforward_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);struct Cyc_Iter_Iter{void*
env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict
d,void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct _tuple0*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(
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
le;};void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Tuniondecl**
Cyc_Tcenv_lookup_tuniondecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple2*);struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,
struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple2*);struct Cyc_List_List*
Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(
struct Cyc_Tcenv_Tenv*te);int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*
r1,void*r2);unsigned long Cyc_strlen(struct _dynforward_ptr s);int Cyc_strcmp(struct
_dynforward_ptr s1,struct _dynforward_ptr s2);int Cyc_strptrcmp(struct
_dynforward_ptr*s1,struct _dynforward_ptr*s2);struct _dynforward_ptr Cyc_strconcat(
struct _dynforward_ptr,struct _dynforward_ptr);struct _tuple7{unsigned int f1;int f2;
};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_lte_const_exp(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2);void*Cyc_Tcutil_impos(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap);void Cyc_Tcutil_flush_warnings();
extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;void*Cyc_Tcutil_copy_type(void*
t);int Cyc_Tcutil_kind_leq(void*k1,void*k2);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(void*t);void Cyc_Tcutil_unchecked_cast(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,void*);int Cyc_Tcutil_coerce_arg(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_assign(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,
void*,struct Cyc_List_List*);int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct
Cyc_Absyn_Exp*);int Cyc_Tcutil_coerceable(void*);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);void*Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*,void*);int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);int Cyc_Tcutil_is_function_type(
void*t);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*
e);int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dynforward_ptr,int*
is_dyneither_ptr);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt
Cyc_Tcutil_trk;extern struct Cyc_Core_Opt Cyc_Tcutil_urk;extern struct Cyc_Core_Opt
Cyc_Tcutil_ak;extern struct Cyc_Core_Opt Cyc_Tcutil_bk;extern struct Cyc_Core_Opt Cyc_Tcutil_mk;
extern struct Cyc_Core_Opt Cyc_Tcutil_ek;extern struct Cyc_Core_Opt Cyc_Tcutil_ik;
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(void*k);void*Cyc_Tcutil_kind_to_bound(
void*k);int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*
e);void*Cyc_Tcutil_max_arithmetic_type(void*,void*);void Cyc_Tcutil_explain_failure();
int Cyc_Tcutil_unify(void*,void*);int Cyc_Tcutil_typecmp(void*,void*);void*Cyc_Tcutil_substitute(
struct Cyc_List_List*,void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,
struct Cyc_List_List*,void*);int Cyc_Tcutil_subset_effect(int may_constrain_evars,
void*e1,void*e2);int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);void*
Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple8{struct Cyc_Absyn_Tvar*
f1;void*f2;};struct _tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct
Cyc_Absyn_Tvar*);struct _tuple9{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
struct _tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*,struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dynforward_ptr*fn);void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*);void Cyc_Tcutil_check_valid_toplevel_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,void*);void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);void Cyc_Tcutil_check_type(struct
Cyc_Position_Segment*,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,void*
k,int allow_evars,void*);void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,
struct Cyc_Position_Segment*loc,struct _dynforward_ptr err_msg);void Cyc_Tcutil_check_unique_tvars(
struct Cyc_Position_Segment*,struct Cyc_List_List*);void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*,struct Cyc_Absyn_Conref*);void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*,unsigned int i,struct Cyc_Absyn_Conref*);int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,
struct Cyc_Absyn_Tqual tq2);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*,int*
is_forward_only);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**
elt_typ_dest,int*is_forward_only);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,
void**elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*
is_dynforward,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_deref(
struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dynforward,int*is_dyneither,void**
elt_type);int Cyc_Tcutil_is_noalias_pointer(void*t);int Cyc_Tcutil_is_noalias_path(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);struct _tuple10{int f1;void*f2;
};struct _tuple10 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e);void*Cyc_Tcutil_normalize_effect(void*e);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);int Cyc_Tcutil_new_tvar_id();void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);int Cyc_Tcutil_bits_only(
void*t);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
void*Cyc_Tcutil_snd_tqt(struct _tuple5*);int Cyc_Tcutil_supports_default(void*);
int Cyc_Tcutil_admits_zero(void*t);int Cyc_Tcutil_is_noreturn(void*);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*,int declared_const,void*);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(
void*t,struct Cyc_List_List*atts);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,
struct Cyc_Absyn_Exp*);void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,
struct Cyc_Position_Segment*,struct Cyc_Absyn_Aggrdecl*);void Cyc_Tc_tcTuniondecl(
struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,struct Cyc_Position_Segment*,struct
Cyc_Absyn_Tuniondecl*);void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,
struct Cyc_Position_Segment*,struct Cyc_Absyn_Enumdecl*);extern int Cyc_Cyclone_tovc_r;
char Cyc_Tcutil_Unify[10]="\000\000\000\000Unify\000";void Cyc_Tcutil_unify_it(
void*t1,void*t2);void*Cyc_Tcutil_t1_failure=(void*)0;int Cyc_Tcutil_tq1_const=0;
void*Cyc_Tcutil_t2_failure=(void*)0;int Cyc_Tcutil_tq2_const=0;struct
_dynforward_ptr Cyc_Tcutil_failure_reason=(struct _dynforward_ptr){(void*)0,(void*)(
0 + 0)};void Cyc_Tcutil_explain_failure();void Cyc_Tcutil_explain_failure(){if(Cyc_Position_num_errors
>= Cyc_Position_max_errors)return;Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
const char*_tmpB7B;const char*_tmpB7A;const char*_tmpB79;void*_tmpB78[2];struct Cyc_String_pa_struct
_tmpB77;struct Cyc_String_pa_struct _tmpB76;struct _dynforward_ptr s1=(struct
_dynforward_ptr)((_tmpB76.tag=0,((_tmpB76.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure)),((_tmpB77.tag=0,((
_tmpB77.f1=(struct _dynforward_ptr)(Cyc_Tcutil_tq1_const?(_tmpB7A="const ",
_tag_dynforward(_tmpB7A,sizeof(char),_get_zero_arr_size_char(_tmpB7A,7))):((
_tmpB7B="",_tag_dynforward(_tmpB7B,sizeof(char),_get_zero_arr_size_char(_tmpB7B,
1))))),((_tmpB78[0]=& _tmpB77,((_tmpB78[1]=& _tmpB76,Cyc_aprintf(((_tmpB79="%s%s",
_tag_dynforward(_tmpB79,sizeof(char),_get_zero_arr_size_char(_tmpB79,5)))),
_tag_dynforward(_tmpB78,sizeof(void*),2))))))))))))));const char*_tmpB84;const
char*_tmpB83;const char*_tmpB82;void*_tmpB81[2];struct Cyc_String_pa_struct _tmpB80;
struct Cyc_String_pa_struct _tmpB7F;struct _dynforward_ptr s2=(struct _dynforward_ptr)((
_tmpB7F.tag=0,((_tmpB7F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
Cyc_Tcutil_t2_failure)),((_tmpB80.tag=0,((_tmpB80.f1=(struct _dynforward_ptr)(Cyc_Tcutil_tq2_const?(
_tmpB83="const ",_tag_dynforward(_tmpB83,sizeof(char),_get_zero_arr_size_char(
_tmpB83,7))):((_tmpB84="",_tag_dynforward(_tmpB84,sizeof(char),
_get_zero_arr_size_char(_tmpB84,1))))),((_tmpB81[0]=& _tmpB80,((_tmpB81[1]=&
_tmpB7F,Cyc_aprintf(((_tmpB82="%s%s",_tag_dynforward(_tmpB82,sizeof(char),
_get_zero_arr_size_char(_tmpB82,5)))),_tag_dynforward(_tmpB81,sizeof(void*),2))))))))))))));
int pos=2;{const char*_tmpB88;void*_tmpB87[1];struct Cyc_String_pa_struct _tmpB86;(
_tmpB86.tag=0,((_tmpB86.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)s1),((
_tmpB87[0]=& _tmpB86,Cyc_fprintf(Cyc_stderr,((_tmpB88="  %s",_tag_dynforward(
_tmpB88,sizeof(char),_get_zero_arr_size_char(_tmpB88,5)))),_tag_dynforward(
_tmpB87,sizeof(void*),1)))))));}pos +=_get_dynforward_size(s1,sizeof(char));if(
pos + 5 >= 80){{const char*_tmpB8B;void*_tmpB8A;(_tmpB8A=0,Cyc_fprintf(Cyc_stderr,((
_tmpB8B="\n\t",_tag_dynforward(_tmpB8B,sizeof(char),_get_zero_arr_size_char(
_tmpB8B,3)))),_tag_dynforward(_tmpB8A,sizeof(void*),0)));}pos=8;}else{{const char*
_tmpB8E;void*_tmpB8D;(_tmpB8D=0,Cyc_fprintf(Cyc_stderr,((_tmpB8E=" ",
_tag_dynforward(_tmpB8E,sizeof(char),_get_zero_arr_size_char(_tmpB8E,2)))),
_tag_dynforward(_tmpB8D,sizeof(void*),0)));}++ pos;}{const char*_tmpB91;void*
_tmpB90;(_tmpB90=0,Cyc_fprintf(Cyc_stderr,((_tmpB91="and ",_tag_dynforward(
_tmpB91,sizeof(char),_get_zero_arr_size_char(_tmpB91,5)))),_tag_dynforward(
_tmpB90,sizeof(void*),0)));}pos +=4;if(pos + _get_dynforward_size(s2,sizeof(char))
>= 80){{const char*_tmpB94;void*_tmpB93;(_tmpB93=0,Cyc_fprintf(Cyc_stderr,((
_tmpB94="\n\t",_tag_dynforward(_tmpB94,sizeof(char),_get_zero_arr_size_char(
_tmpB94,3)))),_tag_dynforward(_tmpB93,sizeof(void*),0)));}pos=8;}{const char*
_tmpB98;void*_tmpB97[1];struct Cyc_String_pa_struct _tmpB96;(_tmpB96.tag=0,((
_tmpB96.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)s2),((_tmpB97[0]=&
_tmpB96,Cyc_fprintf(Cyc_stderr,((_tmpB98="%s ",_tag_dynforward(_tmpB98,sizeof(
char),_get_zero_arr_size_char(_tmpB98,4)))),_tag_dynforward(_tmpB97,sizeof(void*),
1)))))));}pos +=_get_dynforward_size(s2,sizeof(char))+ 1;if(pos + 17 >= 80){{const
char*_tmpB9B;void*_tmpB9A;(_tmpB9A=0,Cyc_fprintf(Cyc_stderr,((_tmpB9B="\n\t",
_tag_dynforward(_tmpB9B,sizeof(char),_get_zero_arr_size_char(_tmpB9B,3)))),
_tag_dynforward(_tmpB9A,sizeof(void*),0)));}pos=8;}{const char*_tmpB9E;void*
_tmpB9D;(_tmpB9D=0,Cyc_fprintf(Cyc_stderr,((_tmpB9E="are not compatible. ",
_tag_dynforward(_tmpB9E,sizeof(char),_get_zero_arr_size_char(_tmpB9E,21)))),
_tag_dynforward(_tmpB9D,sizeof(void*),0)));}pos +=17;if(Cyc_Tcutil_failure_reason.curr
!= ((struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr){if(pos + Cyc_strlen((
struct _dynforward_ptr)Cyc_Tcutil_failure_reason)>= 80){const char*_tmpBA1;void*
_tmpBA0;(_tmpBA0=0,Cyc_fprintf(Cyc_stderr,((_tmpBA1="\n\t",_tag_dynforward(
_tmpBA1,sizeof(char),_get_zero_arr_size_char(_tmpBA1,3)))),_tag_dynforward(
_tmpBA0,sizeof(void*),0)));}{const char*_tmpBA5;void*_tmpBA4[1];struct Cyc_String_pa_struct
_tmpBA3;(_tmpBA3.tag=0,((_tmpBA3.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Tcutil_failure_reason),((_tmpBA4[0]=& _tmpBA3,Cyc_fprintf(Cyc_stderr,((
_tmpBA5="%s",_tag_dynforward(_tmpBA5,sizeof(char),_get_zero_arr_size_char(
_tmpBA5,3)))),_tag_dynforward(_tmpBA4,sizeof(void*),1)))))));}}{const char*
_tmpBA8;void*_tmpBA7;(_tmpBA7=0,Cyc_fprintf(Cyc_stderr,((_tmpBA8="\n",
_tag_dynforward(_tmpBA8,sizeof(char),_get_zero_arr_size_char(_tmpBA8,2)))),
_tag_dynforward(_tmpBA7,sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}}
void Cyc_Tcutil_terr(struct Cyc_Position_Segment*loc,struct _dynforward_ptr fmt,
struct _dynforward_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*loc,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap){Cyc_Position_post_error(Cyc_Position_mk_err_elab(
loc,(struct _dynforward_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}void*Cyc_Tcutil_impos(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap);void*Cyc_Tcutil_impos(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap){struct _dynforward_ptr msg=(struct
_dynforward_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{const char*_tmpBAC;
void*_tmpBAB[1];struct Cyc_String_pa_struct _tmpBAA;(_tmpBAA.tag=0,((_tmpBAA.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)msg),((_tmpBAB[0]=& _tmpBAA,Cyc_fprintf(
Cyc_stderr,((_tmpBAC="Compiler Error (Tcutil::impos): %s\n",_tag_dynforward(
_tmpBAC,sizeof(char),_get_zero_arr_size_char(_tmpBAC,36)))),_tag_dynforward(
_tmpBAB,sizeof(void*),1)))))));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);{
struct Cyc_Core_Impossible_struct _tmpBAF;struct Cyc_Core_Impossible_struct*_tmpBAE;(
int)_throw((void*)((_tmpBAE=_cycalloc(sizeof(*_tmpBAE)),((_tmpBAE[0]=((_tmpBAF.tag=
Cyc_Core_Impossible,((_tmpBAF.f1=msg,_tmpBAF)))),_tmpBAE)))));}}static struct
_dynforward_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv);static struct
_dynforward_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){return*tv->name;}
void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs);void Cyc_Tcutil_print_tvars(
struct Cyc_List_List*tvs){for(0;tvs != 0;tvs=tvs->tl){const char*_tmpBB4;void*
_tmpBB3[2];struct Cyc_String_pa_struct _tmpBB2;struct Cyc_String_pa_struct _tmpBB1;(
_tmpBB1.tag=0,((_tmpBB1.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kindbound2string((
void*)((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmpBB2.tag=0,((_tmpBB2.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
tvs->hd)),((_tmpBB3[0]=& _tmpBB2,((_tmpBB3[1]=& _tmpBB1,Cyc_fprintf(Cyc_stderr,((
_tmpBB4="%s::%s ",_tag_dynforward(_tmpBB4,sizeof(char),_get_zero_arr_size_char(
_tmpBB4,8)))),_tag_dynforward(_tmpBB3,sizeof(void*),2)))))))))))));}{const char*
_tmpBB7;void*_tmpBB6;(_tmpBB6=0,Cyc_fprintf(Cyc_stderr,((_tmpBB7="\n",
_tag_dynforward(_tmpBB7,sizeof(char),_get_zero_arr_size_char(_tmpBB7,2)))),
_tag_dynforward(_tmpBB6,sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}
static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;static struct Cyc_List_List*
Cyc_Tcutil_warning_msgs=0;void Cyc_Tcutil_warn(struct Cyc_Position_Segment*sg,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*
sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap){struct _dynforward_ptr msg=(
struct _dynforward_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{struct Cyc_List_List*
_tmpBB8;Cyc_Tcutil_warning_segs=((_tmpBB8=_cycalloc(sizeof(*_tmpBB8)),((_tmpBB8->hd=
sg,((_tmpBB8->tl=Cyc_Tcutil_warning_segs,_tmpBB8))))));}{struct _dynforward_ptr*
_tmpBBB;struct Cyc_List_List*_tmpBBA;Cyc_Tcutil_warning_msgs=((_tmpBBA=_cycalloc(
sizeof(*_tmpBBA)),((_tmpBBA->hd=((_tmpBBB=_cycalloc(sizeof(*_tmpBBB)),((_tmpBBB[
0]=msg,_tmpBBB)))),((_tmpBBA->tl=Cyc_Tcutil_warning_msgs,_tmpBBA))))));}}void Cyc_Tcutil_flush_warnings();
void Cyc_Tcutil_flush_warnings(){if(Cyc_Tcutil_warning_segs == 0)return;{const char*
_tmpBBE;void*_tmpBBD;(_tmpBBD=0,Cyc_fprintf(Cyc_stderr,((_tmpBBE="***Warnings***\n",
_tag_dynforward(_tmpBBE,sizeof(char),_get_zero_arr_size_char(_tmpBBE,16)))),
_tag_dynforward(_tmpBBD,sizeof(void*),0)));}{struct Cyc_List_List*_tmp35=Cyc_Position_strings_of_segments(
Cyc_Tcutil_warning_segs);Cyc_Tcutil_warning_segs=0;Cyc_Tcutil_warning_msgs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){{const char*_tmpBC3;void*_tmpBC2[2];struct Cyc_String_pa_struct
_tmpBC1;struct Cyc_String_pa_struct _tmpBC0;(_tmpBC0.tag=0,((_tmpBC0.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*((struct _dynforward_ptr*)((struct Cyc_List_List*)
_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmpBC1.tag=0,((_tmpBC1.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*((struct _dynforward_ptr*)((struct Cyc_List_List*)
_check_null(_tmp35))->hd)),((_tmpBC2[0]=& _tmpBC1,((_tmpBC2[1]=& _tmpBC0,Cyc_fprintf(
Cyc_stderr,((_tmpBC3="%s: %s\n",_tag_dynforward(_tmpBC3,sizeof(char),
_get_zero_arr_size_char(_tmpBC3,8)))),_tag_dynforward(_tmpBC2,sizeof(void*),2)))))))))))));}
_tmp35=_tmp35->tl;Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}{
const char*_tmpBC6;void*_tmpBC5;(_tmpBC5=0,Cyc_fprintf(Cyc_stderr,((_tmpBC6="**************\n",
_tag_dynforward(_tmpBC6,sizeof(char),_get_zero_arr_size_char(_tmpBC6,16)))),
_tag_dynforward(_tmpBC5,sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);}}
struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=0;static int Cyc_Tcutil_fast_tvar_cmp(
struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2);static int Cyc_Tcutil_fast_tvar_cmp(
struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){return tv1->identity - tv2->identity;}
void*Cyc_Tcutil_compress(void*t);void*Cyc_Tcutil_compress(void*t){void*_tmp3C=t;
struct Cyc_Core_Opt*_tmp3D;void**_tmp3E;void**_tmp3F;void***_tmp40;struct Cyc_Core_Opt*
_tmp41;struct Cyc_Core_Opt**_tmp42;_LL1: if(_tmp3C <= (void*)4)goto _LL9;if(*((int*)
_tmp3C)!= 0)goto _LL3;_tmp3D=((struct Cyc_Absyn_Evar_struct*)_tmp3C)->f2;if(_tmp3D
!= 0)goto _LL3;_LL2: goto _LL4;_LL3: if(*((int*)_tmp3C)!= 17)goto _LL5;_tmp3E=((
struct Cyc_Absyn_TypedefType_struct*)_tmp3C)->f4;if(_tmp3E != 0)goto _LL5;_LL4:
return t;_LL5: if(*((int*)_tmp3C)!= 17)goto _LL7;_tmp3F=((struct Cyc_Absyn_TypedefType_struct*)
_tmp3C)->f4;_tmp40=(void***)&((struct Cyc_Absyn_TypedefType_struct*)_tmp3C)->f4;
_LL6: {void*t2=Cyc_Tcutil_compress(*((void**)_check_null(*_tmp40)));if(t2 != *((
void**)_check_null(*_tmp40))){void**_tmpBC7;*_tmp40=((_tmpBC7=_cycalloc(sizeof(*
_tmpBC7)),((_tmpBC7[0]=t2,_tmpBC7))));}return t2;}_LL7: if(*((int*)_tmp3C)!= 0)
goto _LL9;_tmp41=((struct Cyc_Absyn_Evar_struct*)_tmp3C)->f2;_tmp42=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp3C)->f2;_LL8: {void*t2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(*_tmp42))->v);if(t2 != (void*)((struct Cyc_Core_Opt*)
_check_null(*_tmp42))->v){struct Cyc_Core_Opt*_tmpBC8;*_tmp42=((_tmpBC8=_cycalloc(
sizeof(*_tmpBC8)),((_tmpBC8->v=(void*)t2,_tmpBC8))));}return t2;}_LL9:;_LLA:
return t;_LL0:;}void*Cyc_Tcutil_copy_type(void*t);static struct Cyc_List_List*Cyc_Tcutil_copy_types(
struct Cyc_List_List*ts);static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct
Cyc_List_List*ts){return Cyc_List_map(Cyc_Tcutil_copy_type,ts);}static struct Cyc_Absyn_Conref*
Cyc_Tcutil_copy_conref(struct Cyc_Absyn_Conref*c);static struct Cyc_Absyn_Conref*
Cyc_Tcutil_copy_conref(struct Cyc_Absyn_Conref*c){union Cyc_Absyn_Constraint_union
_tmp45=c->v;void*_tmp46;struct Cyc_Absyn_Conref*_tmp47;_LLC: if((_tmp45.No_constr).tag
!= 2)goto _LLE;_LLD: return Cyc_Absyn_empty_conref();_LLE: if((_tmp45.Eq_constr).tag
!= 0)goto _LL10;_tmp46=(_tmp45.Eq_constr).f1;_LLF: return Cyc_Absyn_new_conref(
_tmp46);_LL10: if((_tmp45.Forward_constr).tag != 1)goto _LLB;_tmp47=(_tmp45.Forward_constr).f1;
_LL11: return Cyc_Tcutil_copy_conref(_tmp47);_LLB:;}static void*Cyc_Tcutil_copy_kindbound(
void*kb);static void*Cyc_Tcutil_copy_kindbound(void*kb){void*_tmp48=Cyc_Absyn_compress_kb(
kb);void*_tmp49;_LL13: if(*((int*)_tmp48)!= 1)goto _LL15;_LL14: {struct Cyc_Absyn_Unknown_kb_struct
_tmpBCB;struct Cyc_Absyn_Unknown_kb_struct*_tmpBCA;return(void*)((_tmpBCA=
_cycalloc(sizeof(*_tmpBCA)),((_tmpBCA[0]=((_tmpBCB.tag=1,((_tmpBCB.f1=0,_tmpBCB)))),
_tmpBCA))));}_LL15: if(*((int*)_tmp48)!= 2)goto _LL17;_tmp49=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp48)->f2;_LL16: {struct Cyc_Absyn_Less_kb_struct _tmpBCE;struct Cyc_Absyn_Less_kb_struct*
_tmpBCD;return(void*)((_tmpBCD=_cycalloc(sizeof(*_tmpBCD)),((_tmpBCD[0]=((
_tmpBCE.tag=2,((_tmpBCE.f1=0,((_tmpBCE.f2=(void*)_tmp49,_tmpBCE)))))),_tmpBCD))));}
_LL17:;_LL18: return kb;_LL12:;}static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(
struct Cyc_Absyn_Tvar*tv);static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct
Cyc_Absyn_Tvar*tv){struct Cyc_Absyn_Tvar*_tmpBCF;return(_tmpBCF=_cycalloc(sizeof(*
_tmpBCF)),((_tmpBCF->name=tv->name,((_tmpBCF->identity=- 1,((_tmpBCF->kind=(void*)
Cyc_Tcutil_copy_kindbound((void*)tv->kind),_tmpBCF)))))));}static struct _tuple3*
Cyc_Tcutil_copy_arg(struct _tuple3*arg);static struct _tuple3*Cyc_Tcutil_copy_arg(
struct _tuple3*arg){struct _tuple3 _tmp50;struct Cyc_Core_Opt*_tmp51;struct Cyc_Absyn_Tqual
_tmp52;void*_tmp53;struct _tuple3*_tmp4F=arg;_tmp50=*_tmp4F;_tmp51=_tmp50.f1;
_tmp52=_tmp50.f2;_tmp53=_tmp50.f3;{struct _tuple3*_tmpBD0;return(_tmpBD0=
_cycalloc(sizeof(*_tmpBD0)),((_tmpBD0->f1=_tmp51,((_tmpBD0->f2=_tmp52,((_tmpBD0->f3=
Cyc_Tcutil_copy_type(_tmp53),_tmpBD0)))))));}}static struct _tuple5*Cyc_Tcutil_copy_tqt(
struct _tuple5*arg);static struct _tuple5*Cyc_Tcutil_copy_tqt(struct _tuple5*arg){
struct _tuple5 _tmp56;struct Cyc_Absyn_Tqual _tmp57;void*_tmp58;struct _tuple5*_tmp55=
arg;_tmp56=*_tmp55;_tmp57=_tmp56.f1;_tmp58=_tmp56.f2;{struct _tuple5*_tmpBD1;
return(_tmpBD1=_cycalloc(sizeof(*_tmpBD1)),((_tmpBD1->f1=_tmp57,((_tmpBD1->f2=
Cyc_Tcutil_copy_type(_tmp58),_tmpBD1)))));}}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(
struct Cyc_Absyn_Aggrfield*f);static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(
struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*_tmpBD2;return(_tmpBD2=
_cycalloc(sizeof(*_tmpBD2)),((_tmpBD2->name=f->name,((_tmpBD2->tq=f->tq,((
_tmpBD2->type=(void*)Cyc_Tcutil_copy_type((void*)f->type),((_tmpBD2->width=f->width,((
_tmpBD2->attributes=f->attributes,_tmpBD2)))))))))));}static struct _tuple0*Cyc_Tcutil_copy_rgncmp(
struct _tuple0*x);static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
struct _tuple0 _tmp5C;void*_tmp5D;void*_tmp5E;struct _tuple0*_tmp5B=x;_tmp5C=*
_tmp5B;_tmp5D=_tmp5C.f1;_tmp5E=_tmp5C.f2;{struct _tuple0*_tmpBD3;return(_tmpBD3=
_cycalloc(sizeof(*_tmpBD3)),((_tmpBD3->f1=Cyc_Tcutil_copy_type(_tmp5D),((_tmpBD3->f2=
Cyc_Tcutil_copy_type(_tmp5E),_tmpBD3)))));}}static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(
struct Cyc_Absyn_Enumfield*f);static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(
struct Cyc_Absyn_Enumfield*f){struct Cyc_Absyn_Enumfield*_tmpBD4;return(_tmpBD4=
_cycalloc(sizeof(*_tmpBD4)),((_tmpBD4->name=f->name,((_tmpBD4->tag=f->tag,((
_tmpBD4->loc=f->loc,_tmpBD4)))))));}void*Cyc_Tcutil_copy_type(void*t);void*Cyc_Tcutil_copy_type(
void*t){void*_tmp61=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp62;struct Cyc_Absyn_TunionInfo
_tmp63;union Cyc_Absyn_TunionInfoU_union _tmp64;struct Cyc_List_List*_tmp65;struct
Cyc_Core_Opt*_tmp66;struct Cyc_Absyn_TunionFieldInfo _tmp67;union Cyc_Absyn_TunionFieldInfoU_union
_tmp68;struct Cyc_List_List*_tmp69;struct Cyc_Absyn_PtrInfo _tmp6A;void*_tmp6B;
struct Cyc_Absyn_Tqual _tmp6C;struct Cyc_Absyn_PtrAtts _tmp6D;void*_tmp6E;struct Cyc_Absyn_Conref*
_tmp6F;struct Cyc_Absyn_Conref*_tmp70;struct Cyc_Absyn_Conref*_tmp71;struct Cyc_Absyn_PtrLoc*
_tmp72;struct Cyc_Absyn_ArrayInfo _tmp73;void*_tmp74;struct Cyc_Absyn_Tqual _tmp75;
struct Cyc_Absyn_Exp*_tmp76;struct Cyc_Absyn_Conref*_tmp77;struct Cyc_Position_Segment*
_tmp78;struct Cyc_Absyn_FnInfo _tmp79;struct Cyc_List_List*_tmp7A;struct Cyc_Core_Opt*
_tmp7B;void*_tmp7C;struct Cyc_List_List*_tmp7D;int _tmp7E;struct Cyc_Absyn_VarargInfo*
_tmp7F;struct Cyc_List_List*_tmp80;struct Cyc_List_List*_tmp81;struct Cyc_List_List*
_tmp82;struct Cyc_Absyn_AggrInfo _tmp83;union Cyc_Absyn_AggrInfoU_union _tmp84;void*
_tmp85;struct _tuple2*_tmp86;struct Cyc_List_List*_tmp87;struct Cyc_Absyn_AggrInfo
_tmp88;union Cyc_Absyn_AggrInfoU_union _tmp89;struct Cyc_Absyn_Aggrdecl**_tmp8A;
struct Cyc_List_List*_tmp8B;void*_tmp8C;struct Cyc_List_List*_tmp8D;struct _tuple2*
_tmp8E;struct Cyc_Absyn_Enumdecl*_tmp8F;struct Cyc_List_List*_tmp90;void*_tmp91;
int _tmp92;void*_tmp93;void*_tmp94;void*_tmp95;void*_tmp96;struct _tuple2*_tmp97;
struct Cyc_List_List*_tmp98;struct Cyc_Absyn_Typedefdecl*_tmp99;void*_tmp9A;struct
Cyc_List_List*_tmp9B;void*_tmp9C;_LL1A: if((int)_tmp61 != 0)goto _LL1C;_LL1B: goto
_LL1D;_LL1C: if(_tmp61 <= (void*)4)goto _LL28;if(*((int*)_tmp61)!= 0)goto _LL1E;
_LL1D: return t;_LL1E: if(*((int*)_tmp61)!= 1)goto _LL20;_tmp62=((struct Cyc_Absyn_VarType_struct*)
_tmp61)->f1;_LL1F: {struct Cyc_Absyn_VarType_struct _tmpBD7;struct Cyc_Absyn_VarType_struct*
_tmpBD6;return(void*)((_tmpBD6=_cycalloc(sizeof(*_tmpBD6)),((_tmpBD6[0]=((
_tmpBD7.tag=1,((_tmpBD7.f1=Cyc_Tcutil_copy_tvar(_tmp62),_tmpBD7)))),_tmpBD6))));}
_LL20: if(*((int*)_tmp61)!= 2)goto _LL22;_tmp63=((struct Cyc_Absyn_TunionType_struct*)
_tmp61)->f1;_tmp64=_tmp63.tunion_info;_tmp65=_tmp63.targs;_tmp66=_tmp63.rgn;
_LL21: {struct Cyc_Core_Opt*_tmpBD8;struct Cyc_Core_Opt*_tmp9F=(unsigned int)
_tmp66?(_tmpBD8=_cycalloc(sizeof(*_tmpBD8)),((_tmpBD8->v=(void*)Cyc_Tcutil_copy_type((
void*)_tmp66->v),_tmpBD8))): 0;struct Cyc_Absyn_TunionType_struct _tmpBDE;struct Cyc_Absyn_TunionInfo
_tmpBDD;struct Cyc_Absyn_TunionType_struct*_tmpBDC;return(void*)((_tmpBDC=
_cycalloc(sizeof(*_tmpBDC)),((_tmpBDC[0]=((_tmpBDE.tag=2,((_tmpBDE.f1=((_tmpBDD.tunion_info=
_tmp64,((_tmpBDD.targs=Cyc_Tcutil_copy_types(_tmp65),((_tmpBDD.rgn=_tmp9F,
_tmpBDD)))))),_tmpBDE)))),_tmpBDC))));}_LL22: if(*((int*)_tmp61)!= 3)goto _LL24;
_tmp67=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp61)->f1;_tmp68=_tmp67.field_info;
_tmp69=_tmp67.targs;_LL23: {struct Cyc_Absyn_TunionFieldType_struct _tmpBE4;struct
Cyc_Absyn_TunionFieldInfo _tmpBE3;struct Cyc_Absyn_TunionFieldType_struct*_tmpBE2;
return(void*)((_tmpBE2=_cycalloc(sizeof(*_tmpBE2)),((_tmpBE2[0]=((_tmpBE4.tag=3,((
_tmpBE4.f1=((_tmpBE3.field_info=_tmp68,((_tmpBE3.targs=Cyc_Tcutil_copy_types(
_tmp69),_tmpBE3)))),_tmpBE4)))),_tmpBE2))));}_LL24: if(*((int*)_tmp61)!= 4)goto
_LL26;_tmp6A=((struct Cyc_Absyn_PointerType_struct*)_tmp61)->f1;_tmp6B=(void*)
_tmp6A.elt_typ;_tmp6C=_tmp6A.elt_tq;_tmp6D=_tmp6A.ptr_atts;_tmp6E=(void*)_tmp6D.rgn;
_tmp6F=_tmp6D.nullable;_tmp70=_tmp6D.bounds;_tmp71=_tmp6D.zero_term;_tmp72=
_tmp6D.ptrloc;_LL25: {void*_tmpA7=Cyc_Tcutil_copy_type(_tmp6B);void*_tmpA8=Cyc_Tcutil_copy_type(
_tmp6E);struct Cyc_Absyn_Conref*_tmpA9=((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*
c))Cyc_Tcutil_copy_conref)(_tmp6F);struct Cyc_Absyn_Tqual _tmpAA=_tmp6C;struct Cyc_Absyn_Conref*
_tmpAB=Cyc_Tcutil_copy_conref(_tmp70);struct Cyc_Absyn_Conref*_tmpAC=((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(_tmp71);struct Cyc_Absyn_PointerType_struct
_tmpBEE;struct Cyc_Absyn_PtrAtts _tmpBED;struct Cyc_Absyn_PtrInfo _tmpBEC;struct Cyc_Absyn_PointerType_struct*
_tmpBEB;return(void*)((_tmpBEB=_cycalloc(sizeof(*_tmpBEB)),((_tmpBEB[0]=((
_tmpBEE.tag=4,((_tmpBEE.f1=((_tmpBEC.elt_typ=(void*)_tmpA7,((_tmpBEC.elt_tq=
_tmpAA,((_tmpBEC.ptr_atts=((_tmpBED.rgn=(void*)_tmpA8,((_tmpBED.nullable=_tmpA9,((
_tmpBED.bounds=_tmpAB,((_tmpBED.zero_term=_tmpAC,((_tmpBED.ptrloc=_tmp72,_tmpBED)))))))))),
_tmpBEC)))))),_tmpBEE)))),_tmpBEB))));}_LL26: if(*((int*)_tmp61)!= 5)goto _LL28;
_LL27: goto _LL29;_LL28: if((int)_tmp61 != 1)goto _LL2A;_LL29: goto _LL2B;_LL2A: if(
_tmp61 <= (void*)4)goto _LL48;if(*((int*)_tmp61)!= 6)goto _LL2C;_LL2B: return t;_LL2C:
if(*((int*)_tmp61)!= 7)goto _LL2E;_tmp73=((struct Cyc_Absyn_ArrayType_struct*)
_tmp61)->f1;_tmp74=(void*)_tmp73.elt_type;_tmp75=_tmp73.tq;_tmp76=_tmp73.num_elts;
_tmp77=_tmp73.zero_term;_tmp78=_tmp73.zt_loc;_LL2D: {struct Cyc_Absyn_ArrayType_struct
_tmpBF4;struct Cyc_Absyn_ArrayInfo _tmpBF3;struct Cyc_Absyn_ArrayType_struct*
_tmpBF2;return(void*)((_tmpBF2=_cycalloc(sizeof(*_tmpBF2)),((_tmpBF2[0]=((
_tmpBF4.tag=7,((_tmpBF4.f1=((_tmpBF3.elt_type=(void*)Cyc_Tcutil_copy_type(_tmp74),((
_tmpBF3.tq=_tmp75,((_tmpBF3.num_elts=_tmp76,((_tmpBF3.zero_term=((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*c))Cyc_Tcutil_copy_conref)(_tmp77),((_tmpBF3.zt_loc=
_tmp78,_tmpBF3)))))))))),_tmpBF4)))),_tmpBF2))));}_LL2E: if(*((int*)_tmp61)!= 8)
goto _LL30;_tmp79=((struct Cyc_Absyn_FnType_struct*)_tmp61)->f1;_tmp7A=_tmp79.tvars;
_tmp7B=_tmp79.effect;_tmp7C=(void*)_tmp79.ret_typ;_tmp7D=_tmp79.args;_tmp7E=
_tmp79.c_varargs;_tmp7F=_tmp79.cyc_varargs;_tmp80=_tmp79.rgn_po;_tmp81=_tmp79.attributes;
_LL2F: {struct Cyc_List_List*_tmpB4=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,
_tmp7A);struct Cyc_Core_Opt*_tmpBF5;struct Cyc_Core_Opt*_tmpB5=_tmp7B == 0?0:((
_tmpBF5=_cycalloc(sizeof(*_tmpBF5)),((_tmpBF5->v=(void*)Cyc_Tcutil_copy_type((
void*)_tmp7B->v),_tmpBF5))));void*_tmpB6=Cyc_Tcutil_copy_type(_tmp7C);struct Cyc_List_List*
_tmpB7=((struct Cyc_List_List*(*)(struct _tuple3*(*f)(struct _tuple3*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp7D);int _tmpB8=_tmp7E;struct Cyc_Absyn_VarargInfo*
cyc_varargs2=0;if(_tmp7F != 0){struct Cyc_Absyn_VarargInfo*cv=(struct Cyc_Absyn_VarargInfo*)
_tmp7F;struct Cyc_Absyn_VarargInfo*_tmpBF6;cyc_varargs2=((_tmpBF6=_cycalloc(
sizeof(*_tmpBF6)),((_tmpBF6->name=cv->name,((_tmpBF6->tq=cv->tq,((_tmpBF6->type=(
void*)Cyc_Tcutil_copy_type((void*)cv->type),((_tmpBF6->inject=cv->inject,_tmpBF6))))))))));}{
struct Cyc_List_List*_tmpBA=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct
_tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp80);
struct Cyc_List_List*_tmpBB=_tmp81;struct Cyc_Absyn_FnType_struct _tmpBFC;struct Cyc_Absyn_FnInfo
_tmpBFB;struct Cyc_Absyn_FnType_struct*_tmpBFA;return(void*)((_tmpBFA=_cycalloc(
sizeof(*_tmpBFA)),((_tmpBFA[0]=((_tmpBFC.tag=8,((_tmpBFC.f1=((_tmpBFB.tvars=
_tmpB4,((_tmpBFB.effect=_tmpB5,((_tmpBFB.ret_typ=(void*)_tmpB6,((_tmpBFB.args=
_tmpB7,((_tmpBFB.c_varargs=_tmpB8,((_tmpBFB.cyc_varargs=cyc_varargs2,((_tmpBFB.rgn_po=
_tmpBA,((_tmpBFB.attributes=_tmpBB,_tmpBFB)))))))))))))))),_tmpBFC)))),_tmpBFA))));}}
_LL30: if(*((int*)_tmp61)!= 9)goto _LL32;_tmp82=((struct Cyc_Absyn_TupleType_struct*)
_tmp61)->f1;_LL31: {struct Cyc_Absyn_TupleType_struct _tmpBFF;struct Cyc_Absyn_TupleType_struct*
_tmpBFE;return(void*)((_tmpBFE=_cycalloc(sizeof(*_tmpBFE)),((_tmpBFE[0]=((
_tmpBFF.tag=9,((_tmpBFF.f1=((struct Cyc_List_List*(*)(struct _tuple5*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp82),
_tmpBFF)))),_tmpBFE))));}_LL32: if(*((int*)_tmp61)!= 10)goto _LL34;_tmp83=((struct
Cyc_Absyn_AggrType_struct*)_tmp61)->f1;_tmp84=_tmp83.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmp61)->f1).aggr_info).UnknownAggr).tag != 0)goto _LL34;_tmp85=(_tmp84.UnknownAggr).f1;
_tmp86=(_tmp84.UnknownAggr).f2;_tmp87=_tmp83.targs;_LL33: {struct Cyc_Absyn_AggrType_struct
_tmpC09;union Cyc_Absyn_AggrInfoU_union _tmpC08;struct Cyc_Absyn_AggrInfo _tmpC07;
struct Cyc_Absyn_AggrType_struct*_tmpC06;return(void*)((_tmpC06=_cycalloc(sizeof(*
_tmpC06)),((_tmpC06[0]=((_tmpC09.tag=10,((_tmpC09.f1=((_tmpC07.aggr_info=(union
Cyc_Absyn_AggrInfoU_union)(((_tmpC08.UnknownAggr).tag=0,(((_tmpC08.UnknownAggr).f1=(
void*)_tmp85,(((_tmpC08.UnknownAggr).f2=_tmp86,_tmpC08)))))),((_tmpC07.targs=Cyc_Tcutil_copy_types(
_tmp87),_tmpC07)))),_tmpC09)))),_tmpC06))));}_LL34: if(*((int*)_tmp61)!= 10)goto
_LL36;_tmp88=((struct Cyc_Absyn_AggrType_struct*)_tmp61)->f1;_tmp89=_tmp88.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmp61)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL36;_tmp8A=(_tmp89.KnownAggr).f1;_tmp8B=_tmp88.targs;_LL35: {struct
Cyc_Absyn_AggrType_struct _tmpC13;union Cyc_Absyn_AggrInfoU_union _tmpC12;struct Cyc_Absyn_AggrInfo
_tmpC11;struct Cyc_Absyn_AggrType_struct*_tmpC10;return(void*)((_tmpC10=_cycalloc(
sizeof(*_tmpC10)),((_tmpC10[0]=((_tmpC13.tag=10,((_tmpC13.f1=((_tmpC11.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)(((_tmpC12.KnownAggr).tag=1,(((_tmpC12.KnownAggr).f1=
_tmp8A,_tmpC12)))),((_tmpC11.targs=Cyc_Tcutil_copy_types(_tmp8B),_tmpC11)))),
_tmpC13)))),_tmpC10))));}_LL36: if(*((int*)_tmp61)!= 11)goto _LL38;_tmp8C=(void*)((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp61)->f1;_tmp8D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp61)->f2;_LL37: {struct Cyc_Absyn_AnonAggrType_struct _tmpC16;struct Cyc_Absyn_AnonAggrType_struct*
_tmpC15;return(void*)((_tmpC15=_cycalloc(sizeof(*_tmpC15)),((_tmpC15[0]=((
_tmpC16.tag=11,((_tmpC16.f1=(void*)_tmp8C,((_tmpC16.f2=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp8D),_tmpC16)))))),_tmpC15))));}_LL38: if(*((
int*)_tmp61)!= 12)goto _LL3A;_tmp8E=((struct Cyc_Absyn_EnumType_struct*)_tmp61)->f1;
_tmp8F=((struct Cyc_Absyn_EnumType_struct*)_tmp61)->f2;_LL39: {struct Cyc_Absyn_EnumType_struct
_tmpC19;struct Cyc_Absyn_EnumType_struct*_tmpC18;return(void*)((_tmpC18=_cycalloc(
sizeof(*_tmpC18)),((_tmpC18[0]=((_tmpC19.tag=12,((_tmpC19.f1=_tmp8E,((_tmpC19.f2=
_tmp8F,_tmpC19)))))),_tmpC18))));}_LL3A: if(*((int*)_tmp61)!= 13)goto _LL3C;_tmp90=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp61)->f1;_LL3B: {struct Cyc_Absyn_AnonEnumType_struct
_tmpC1C;struct Cyc_Absyn_AnonEnumType_struct*_tmpC1B;return(void*)((_tmpC1B=
_cycalloc(sizeof(*_tmpC1B)),((_tmpC1B[0]=((_tmpC1C.tag=13,((_tmpC1C.f1=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmp90),_tmpC1C)))),
_tmpC1B))));}_LL3C: if(*((int*)_tmp61)!= 14)goto _LL3E;_tmp91=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp61)->f1;_LL3D: {struct Cyc_Absyn_SizeofType_struct _tmpC1F;struct Cyc_Absyn_SizeofType_struct*
_tmpC1E;return(void*)((_tmpC1E=_cycalloc(sizeof(*_tmpC1E)),((_tmpC1E[0]=((
_tmpC1F.tag=14,((_tmpC1F.f1=(void*)Cyc_Tcutil_copy_type(_tmp91),_tmpC1F)))),
_tmpC1E))));}_LL3E: if(*((int*)_tmp61)!= 19)goto _LL40;_tmp92=((struct Cyc_Absyn_TypeInt_struct*)
_tmp61)->f1;_LL3F: {struct Cyc_Absyn_TypeInt_struct _tmpC22;struct Cyc_Absyn_TypeInt_struct*
_tmpC21;return(void*)((_tmpC21=_cycalloc_atomic(sizeof(*_tmpC21)),((_tmpC21[0]=((
_tmpC22.tag=19,((_tmpC22.f1=_tmp92,_tmpC22)))),_tmpC21))));}_LL40: if(*((int*)
_tmp61)!= 18)goto _LL42;_tmp93=(void*)((struct Cyc_Absyn_TagType_struct*)_tmp61)->f1;
_LL41: {struct Cyc_Absyn_TagType_struct _tmpC25;struct Cyc_Absyn_TagType_struct*
_tmpC24;return(void*)((_tmpC24=_cycalloc(sizeof(*_tmpC24)),((_tmpC24[0]=((
_tmpC25.tag=18,((_tmpC25.f1=(void*)Cyc_Tcutil_copy_type(_tmp93),_tmpC25)))),
_tmpC24))));}_LL42: if(*((int*)_tmp61)!= 15)goto _LL44;_tmp94=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp61)->f1;_LL43: {struct Cyc_Absyn_RgnHandleType_struct _tmpC28;struct Cyc_Absyn_RgnHandleType_struct*
_tmpC27;return(void*)((_tmpC27=_cycalloc(sizeof(*_tmpC27)),((_tmpC27[0]=((
_tmpC28.tag=15,((_tmpC28.f1=(void*)Cyc_Tcutil_copy_type(_tmp94),_tmpC28)))),
_tmpC27))));}_LL44: if(*((int*)_tmp61)!= 16)goto _LL46;_tmp95=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp61)->f1;_tmp96=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp61)->f2;_LL45: {
struct Cyc_Absyn_DynRgnType_struct _tmpC2B;struct Cyc_Absyn_DynRgnType_struct*
_tmpC2A;return(void*)((_tmpC2A=_cycalloc(sizeof(*_tmpC2A)),((_tmpC2A[0]=((
_tmpC2B.tag=16,((_tmpC2B.f1=(void*)Cyc_Tcutil_copy_type(_tmp95),((_tmpC2B.f2=(
void*)Cyc_Tcutil_copy_type(_tmp96),_tmpC2B)))))),_tmpC2A))));}_LL46: if(*((int*)
_tmp61)!= 17)goto _LL48;_tmp97=((struct Cyc_Absyn_TypedefType_struct*)_tmp61)->f1;
_tmp98=((struct Cyc_Absyn_TypedefType_struct*)_tmp61)->f2;_tmp99=((struct Cyc_Absyn_TypedefType_struct*)
_tmp61)->f3;_LL47: {struct Cyc_Absyn_TypedefType_struct _tmpC2E;struct Cyc_Absyn_TypedefType_struct*
_tmpC2D;return(void*)((_tmpC2D=_cycalloc(sizeof(*_tmpC2D)),((_tmpC2D[0]=((
_tmpC2E.tag=17,((_tmpC2E.f1=_tmp97,((_tmpC2E.f2=Cyc_Tcutil_copy_types(_tmp98),((
_tmpC2E.f3=_tmp99,((_tmpC2E.f4=0,_tmpC2E)))))))))),_tmpC2D))));}_LL48: if((int)
_tmp61 != 3)goto _LL4A;_LL49: goto _LL4B;_LL4A: if((int)_tmp61 != 2)goto _LL4C;_LL4B:
return t;_LL4C: if(_tmp61 <= (void*)4)goto _LL4E;if(*((int*)_tmp61)!= 20)goto _LL4E;
_tmp9A=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp61)->f1;_LL4D: {struct Cyc_Absyn_AccessEff_struct
_tmpC31;struct Cyc_Absyn_AccessEff_struct*_tmpC30;return(void*)((_tmpC30=
_cycalloc(sizeof(*_tmpC30)),((_tmpC30[0]=((_tmpC31.tag=20,((_tmpC31.f1=(void*)
Cyc_Tcutil_copy_type(_tmp9A),_tmpC31)))),_tmpC30))));}_LL4E: if(_tmp61 <= (void*)4)
goto _LL50;if(*((int*)_tmp61)!= 21)goto _LL50;_tmp9B=((struct Cyc_Absyn_JoinEff_struct*)
_tmp61)->f1;_LL4F: {struct Cyc_Absyn_JoinEff_struct _tmpC34;struct Cyc_Absyn_JoinEff_struct*
_tmpC33;return(void*)((_tmpC33=_cycalloc(sizeof(*_tmpC33)),((_tmpC33[0]=((
_tmpC34.tag=21,((_tmpC34.f1=Cyc_Tcutil_copy_types(_tmp9B),_tmpC34)))),_tmpC33))));}
_LL50: if(_tmp61 <= (void*)4)goto _LL19;if(*((int*)_tmp61)!= 22)goto _LL19;_tmp9C=(
void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp61)->f1;_LL51: {struct Cyc_Absyn_RgnsEff_struct
_tmpC37;struct Cyc_Absyn_RgnsEff_struct*_tmpC36;return(void*)((_tmpC36=_cycalloc(
sizeof(*_tmpC36)),((_tmpC36[0]=((_tmpC37.tag=22,((_tmpC37.f1=(void*)Cyc_Tcutil_copy_type(
_tmp9C),_tmpC37)))),_tmpC36))));}_LL19:;}int Cyc_Tcutil_kind_leq(void*k1,void*k2);
int Cyc_Tcutil_kind_leq(void*k1,void*k2){if(k1 == k2)return 1;{struct _tuple0 _tmpC38;
struct _tuple0 _tmpE3=(_tmpC38.f1=k1,((_tmpC38.f2=k2,_tmpC38)));void*_tmpE4;void*
_tmpE5;void*_tmpE6;void*_tmpE7;void*_tmpE8;void*_tmpE9;void*_tmpEA;void*_tmpEB;
void*_tmpEC;void*_tmpED;_LL53: _tmpE4=_tmpE3.f1;if((int)_tmpE4 != 2)goto _LL55;
_tmpE5=_tmpE3.f2;if((int)_tmpE5 != 1)goto _LL55;_LL54: goto _LL56;_LL55: _tmpE6=
_tmpE3.f1;if((int)_tmpE6 != 2)goto _LL57;_tmpE7=_tmpE3.f2;if((int)_tmpE7 != 0)goto
_LL57;_LL56: goto _LL58;_LL57: _tmpE8=_tmpE3.f1;if((int)_tmpE8 != 1)goto _LL59;_tmpE9=
_tmpE3.f2;if((int)_tmpE9 != 0)goto _LL59;_LL58: goto _LL5A;_LL59: _tmpEA=_tmpE3.f1;
if((int)_tmpEA != 3)goto _LL5B;_tmpEB=_tmpE3.f2;if((int)_tmpEB != 5)goto _LL5B;_LL5A:
goto _LL5C;_LL5B: _tmpEC=_tmpE3.f1;if((int)_tmpEC != 4)goto _LL5D;_tmpED=_tmpE3.f2;
if((int)_tmpED != 5)goto _LL5D;_LL5C: return 1;_LL5D:;_LL5E: return 0;_LL52:;}}static
int Cyc_Tcutil_is_region_kind(void*k);static int Cyc_Tcutil_is_region_kind(void*k){
void*_tmpEE=k;_LL60: if((int)_tmpEE != 3)goto _LL62;_LL61: goto _LL63;_LL62: if((int)
_tmpEE != 5)goto _LL64;_LL63: goto _LL65;_LL64: if((int)_tmpEE != 4)goto _LL66;_LL65:
return 1;_LL66:;_LL67: return 0;_LL5F:;}void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
tv);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv){void*_tmpEF=Cyc_Absyn_compress_kb((
void*)tv->kind);void*_tmpF0;void*_tmpF1;_LL69: if(*((int*)_tmpEF)!= 0)goto _LL6B;
_tmpF0=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpEF)->f1;_LL6A: return _tmpF0;
_LL6B: if(*((int*)_tmpEF)!= 2)goto _LL6D;_tmpF1=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpEF)->f2;_LL6C: return _tmpF1;_LL6D:;_LL6E: {const char*_tmpC3B;void*_tmpC3A;(
_tmpC3A=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmpC3B="kind not suitably constrained!",_tag_dynforward(_tmpC3B,sizeof(char),
_get_zero_arr_size_char(_tmpC3B,31)))),_tag_dynforward(_tmpC3A,sizeof(void*),0)));}
_LL68:;}void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_typ_kind(void*t){void*
_tmpF4=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*_tmpF5;struct Cyc_Absyn_Tvar*
_tmpF6;void*_tmpF7;struct Cyc_Absyn_TunionInfo _tmpF8;union Cyc_Absyn_TunionInfoU_union
_tmpF9;struct Cyc_Absyn_UnknownTunionInfo _tmpFA;struct Cyc_Absyn_TunionInfo _tmpFB;
union Cyc_Absyn_TunionInfoU_union _tmpFC;struct Cyc_Absyn_Tuniondecl**_tmpFD;struct
Cyc_Absyn_Tuniondecl*_tmpFE;struct Cyc_Absyn_TunionFieldInfo _tmpFF;union Cyc_Absyn_TunionFieldInfoU_union
_tmp100;struct Cyc_Absyn_Tuniondecl*_tmp101;struct Cyc_Absyn_Tunionfield*_tmp102;
struct Cyc_Absyn_TunionFieldInfo _tmp103;union Cyc_Absyn_TunionFieldInfoU_union
_tmp104;struct Cyc_Absyn_Enumdecl*_tmp105;struct Cyc_Absyn_AggrInfo _tmp106;union
Cyc_Absyn_AggrInfoU_union _tmp107;struct Cyc_Absyn_AggrInfo _tmp108;union Cyc_Absyn_AggrInfoU_union
_tmp109;struct Cyc_Absyn_Aggrdecl**_tmp10A;struct Cyc_Absyn_Aggrdecl*_tmp10B;
struct Cyc_Absyn_Aggrdecl _tmp10C;struct Cyc_Absyn_AggrdeclImpl*_tmp10D;struct Cyc_Absyn_Enumdecl*
_tmp10E;struct Cyc_Absyn_PtrInfo _tmp10F;struct Cyc_Absyn_Typedefdecl*_tmp110;_LL70:
if(_tmpF4 <= (void*)4)goto _LL74;if(*((int*)_tmpF4)!= 0)goto _LL72;_tmpF5=((struct
Cyc_Absyn_Evar_struct*)_tmpF4)->f1;_LL71: return(void*)((struct Cyc_Core_Opt*)
_check_null(_tmpF5))->v;_LL72: if(*((int*)_tmpF4)!= 1)goto _LL74;_tmpF6=((struct
Cyc_Absyn_VarType_struct*)_tmpF4)->f1;_LL73: return Cyc_Tcutil_tvar_kind(_tmpF6);
_LL74: if((int)_tmpF4 != 0)goto _LL76;_LL75: return(void*)1;_LL76: if(_tmpF4 <= (void*)
4)goto _LL78;if(*((int*)_tmpF4)!= 5)goto _LL78;_tmpF7=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpF4)->f2;_LL77: return(_tmpF7 == (void*)((void*)2) || _tmpF7 == (void*)((void*)3))?(
void*)2:(void*)1;_LL78: if((int)_tmpF4 != 1)goto _LL7A;_LL79: goto _LL7B;_LL7A: if(
_tmpF4 <= (void*)4)goto _LL82;if(*((int*)_tmpF4)!= 6)goto _LL7C;_LL7B: goto _LL7D;
_LL7C: if(*((int*)_tmpF4)!= 8)goto _LL7E;_LL7D: return(void*)1;_LL7E: if(*((int*)
_tmpF4)!= 16)goto _LL80;_LL7F: goto _LL81;_LL80: if(*((int*)_tmpF4)!= 15)goto _LL82;
_LL81: return(void*)2;_LL82: if((int)_tmpF4 != 3)goto _LL84;_LL83: return(void*)4;
_LL84: if((int)_tmpF4 != 2)goto _LL86;_LL85: return(void*)3;_LL86: if(_tmpF4 <= (void*)
4)goto _LL88;if(*((int*)_tmpF4)!= 2)goto _LL88;_tmpF8=((struct Cyc_Absyn_TunionType_struct*)
_tmpF4)->f1;_tmpF9=_tmpF8.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmpF4)->f1).tunion_info).UnknownTunion).tag != 0)goto _LL88;_tmpFA=(_tmpF9.UnknownTunion).f1;
_LL87: if(_tmpFA.is_flat)return(void*)1;else{return(void*)2;}_LL88: if(_tmpF4 <= (
void*)4)goto _LL8A;if(*((int*)_tmpF4)!= 2)goto _LL8A;_tmpFB=((struct Cyc_Absyn_TunionType_struct*)
_tmpF4)->f1;_tmpFC=_tmpFB.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmpF4)->f1).tunion_info).KnownTunion).tag != 1)goto _LL8A;_tmpFD=(_tmpFC.KnownTunion).f1;
_tmpFE=*_tmpFD;_LL89: if(_tmpFE->is_flat)return(void*)1;else{return(void*)2;}
_LL8A: if(_tmpF4 <= (void*)4)goto _LL8C;if(*((int*)_tmpF4)!= 3)goto _LL8C;_tmpFF=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmpF4)->f1;_tmp100=_tmpFF.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmpF4)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL8C;_tmp101=(_tmp100.KnownTunionfield).f1;_tmp102=(_tmp100.KnownTunionfield).f2;
_LL8B: if(_tmp101->is_flat)return(void*)1;else{if(_tmp102->typs == 0)return(void*)
2;else{return(void*)1;}}_LL8C: if(_tmpF4 <= (void*)4)goto _LL8E;if(*((int*)_tmpF4)
!= 3)goto _LL8E;_tmp103=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpF4)->f1;
_tmp104=_tmp103.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpF4)->f1).field_info).UnknownTunionfield).tag != 0)goto _LL8E;_LL8D: {const char*
_tmpC3E;void*_tmpC3D;(_tmpC3D=0,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmpC3E="typ_kind: Unresolved TunionFieldType",
_tag_dynforward(_tmpC3E,sizeof(char),_get_zero_arr_size_char(_tmpC3E,37)))),
_tag_dynforward(_tmpC3D,sizeof(void*),0)));}_LL8E: if(_tmpF4 <= (void*)4)goto _LL90;
if(*((int*)_tmpF4)!= 12)goto _LL90;_tmp105=((struct Cyc_Absyn_EnumType_struct*)
_tmpF4)->f2;if(_tmp105 != 0)goto _LL90;_LL8F: goto _LL91;_LL90: if(_tmpF4 <= (void*)4)
goto _LL92;if(*((int*)_tmpF4)!= 10)goto _LL92;_tmp106=((struct Cyc_Absyn_AggrType_struct*)
_tmpF4)->f1;_tmp107=_tmp106.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmpF4)->f1).aggr_info).UnknownAggr).tag != 0)goto _LL92;_LL91: return(void*)0;
_LL92: if(_tmpF4 <= (void*)4)goto _LL94;if(*((int*)_tmpF4)!= 10)goto _LL94;_tmp108=((
struct Cyc_Absyn_AggrType_struct*)_tmpF4)->f1;_tmp109=_tmp108.aggr_info;if((((((
struct Cyc_Absyn_AggrType_struct*)_tmpF4)->f1).aggr_info).KnownAggr).tag != 1)goto
_LL94;_tmp10A=(_tmp109.KnownAggr).f1;_tmp10B=*_tmp10A;_tmp10C=*_tmp10B;_tmp10D=
_tmp10C.impl;_LL93: return _tmp10D == 0?(void*)0:(void*)1;_LL94: if(_tmpF4 <= (void*)
4)goto _LL96;if(*((int*)_tmpF4)!= 11)goto _LL96;_LL95: goto _LL97;_LL96: if(_tmpF4 <= (
void*)4)goto _LL98;if(*((int*)_tmpF4)!= 13)goto _LL98;_LL97: return(void*)1;_LL98:
if(_tmpF4 <= (void*)4)goto _LL9A;if(*((int*)_tmpF4)!= 12)goto _LL9A;_tmp10E=((
struct Cyc_Absyn_EnumType_struct*)_tmpF4)->f2;_LL99: if(_tmp10E->fields == 0)return(
void*)0;else{return(void*)1;}_LL9A: if(_tmpF4 <= (void*)4)goto _LL9C;if(*((int*)
_tmpF4)!= 4)goto _LL9C;_tmp10F=((struct Cyc_Absyn_PointerType_struct*)_tmpF4)->f1;
_LL9B: {union Cyc_Absyn_Constraint_union _tmp113=(Cyc_Absyn_compress_conref((
_tmp10F.ptr_atts).bounds))->v;void*_tmp114;void*_tmp115;void*_tmp116;void*
_tmp117;_LLAF: if((_tmp113.No_constr).tag != 2)goto _LLB1;_LLB0: goto _LLB2;_LLB1: if((
_tmp113.Eq_constr).tag != 0)goto _LLB3;_tmp114=(_tmp113.Eq_constr).f1;if((int)
_tmp114 != 0)goto _LLB3;_LLB2: return(void*)1;_LLB3: if((_tmp113.Eq_constr).tag != 0)
goto _LLB5;_tmp115=(_tmp113.Eq_constr).f1;if((int)_tmp115 != 1)goto _LLB5;_LLB4:
return(void*)1;_LLB5: if((_tmp113.Eq_constr).tag != 0)goto _LLB7;_tmp116=(_tmp113.Eq_constr).f1;
if(_tmp116 <= (void*)2)goto _LLB7;if(*((int*)_tmp116)!= 0)goto _LLB7;_LLB6: goto
_LLB8;_LLB7: if((_tmp113.Eq_constr).tag != 0)goto _LLB9;_tmp117=(_tmp113.Eq_constr).f1;
if(_tmp117 <= (void*)2)goto _LLB9;if(*((int*)_tmp117)!= 1)goto _LLB9;_LLB8: return(
void*)2;_LLB9: if((_tmp113.Forward_constr).tag != 1)goto _LLAE;_LLBA: {const char*
_tmpC41;void*_tmpC40;(_tmpC40=0,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmpC41="typ_kind: forward constr in ptr bounds",
_tag_dynforward(_tmpC41,sizeof(char),_get_zero_arr_size_char(_tmpC41,39)))),
_tag_dynforward(_tmpC40,sizeof(void*),0)));}_LLAE:;}_LL9C: if(_tmpF4 <= (void*)4)
goto _LL9E;if(*((int*)_tmpF4)!= 14)goto _LL9E;_LL9D: return(void*)2;_LL9E: if(_tmpF4
<= (void*)4)goto _LLA0;if(*((int*)_tmpF4)!= 19)goto _LLA0;_LL9F: return(void*)7;
_LLA0: if(_tmpF4 <= (void*)4)goto _LLA2;if(*((int*)_tmpF4)!= 18)goto _LLA2;_LLA1:
return(void*)2;_LLA2: if(_tmpF4 <= (void*)4)goto _LLA4;if(*((int*)_tmpF4)!= 7)goto
_LLA4;_LLA3: goto _LLA5;_LLA4: if(_tmpF4 <= (void*)4)goto _LLA6;if(*((int*)_tmpF4)!= 
9)goto _LLA6;_LLA5: return(void*)1;_LLA6: if(_tmpF4 <= (void*)4)goto _LLA8;if(*((int*)
_tmpF4)!= 17)goto _LLA8;_tmp110=((struct Cyc_Absyn_TypedefType_struct*)_tmpF4)->f3;
_LLA7: if(_tmp110 == 0  || _tmp110->kind == 0){const char*_tmpC45;void*_tmpC44[1];
struct Cyc_String_pa_struct _tmpC43;(_tmpC43.tag=0,((_tmpC43.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t)),((_tmpC44[0]=&
_tmpC43,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmpC45="typ_kind: typedef found: %s",_tag_dynforward(_tmpC45,sizeof(char),
_get_zero_arr_size_char(_tmpC45,28)))),_tag_dynforward(_tmpC44,sizeof(void*),1)))))));}
return(void*)((struct Cyc_Core_Opt*)_check_null(_tmp110->kind))->v;_LLA8: if(
_tmpF4 <= (void*)4)goto _LLAA;if(*((int*)_tmpF4)!= 20)goto _LLAA;_LLA9: goto _LLAB;
_LLAA: if(_tmpF4 <= (void*)4)goto _LLAC;if(*((int*)_tmpF4)!= 21)goto _LLAC;_LLAB:
goto _LLAD;_LLAC: if(_tmpF4 <= (void*)4)goto _LL6F;if(*((int*)_tmpF4)!= 22)goto _LL6F;
_LLAD: return(void*)6;_LL6F:;}int Cyc_Tcutil_unify(void*t1,void*t2);int Cyc_Tcutil_unify(
void*t1,void*t2){struct _handler_cons _tmp11D;_push_handler(& _tmp11D);{int _tmp11F=
0;if(setjmp(_tmp11D.handler))_tmp11F=1;if(!_tmp11F){Cyc_Tcutil_unify_it(t1,t2);{
int _tmp120=1;_npop_handler(0);return _tmp120;};_pop_handler();}else{void*_tmp11E=(
void*)_exn_thrown;void*_tmp122=_tmp11E;_LLBC: if(_tmp122 != Cyc_Tcutil_Unify)goto
_LLBE;_LLBD: return 0;_LLBE:;_LLBF:(void)_throw(_tmp122);_LLBB:;}}}static void Cyc_Tcutil_occurslist(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*
env,void*t);static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct
Cyc_List_List*env,void*t){t=Cyc_Tcutil_compress(t);{void*_tmp123=t;struct Cyc_Absyn_Tvar*
_tmp124;struct Cyc_Core_Opt*_tmp125;struct Cyc_Core_Opt*_tmp126;struct Cyc_Core_Opt**
_tmp127;struct Cyc_Absyn_PtrInfo _tmp128;struct Cyc_Absyn_ArrayInfo _tmp129;void*
_tmp12A;struct Cyc_Absyn_FnInfo _tmp12B;struct Cyc_List_List*_tmp12C;struct Cyc_Core_Opt*
_tmp12D;void*_tmp12E;struct Cyc_List_List*_tmp12F;int _tmp130;struct Cyc_Absyn_VarargInfo*
_tmp131;struct Cyc_List_List*_tmp132;struct Cyc_List_List*_tmp133;struct Cyc_List_List*
_tmp134;struct Cyc_Absyn_TunionInfo _tmp135;struct Cyc_List_List*_tmp136;struct Cyc_Core_Opt*
_tmp137;struct Cyc_List_List*_tmp138;struct Cyc_Absyn_TunionFieldInfo _tmp139;
struct Cyc_List_List*_tmp13A;struct Cyc_Absyn_AggrInfo _tmp13B;struct Cyc_List_List*
_tmp13C;struct Cyc_List_List*_tmp13D;void*_tmp13E;void*_tmp13F;void*_tmp140;void*
_tmp141;void*_tmp142;struct Cyc_List_List*_tmp143;_LLC1: if(_tmp123 <= (void*)4)
goto _LLE3;if(*((int*)_tmp123)!= 1)goto _LLC3;_tmp124=((struct Cyc_Absyn_VarType_struct*)
_tmp123)->f1;_LLC2: if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,_tmp124)){{const char*_tmpC46;Cyc_Tcutil_failure_reason=((_tmpC46="(type variable would escape scope)",
_tag_dynforward(_tmpC46,sizeof(char),_get_zero_arr_size_char(_tmpC46,35))));}(
int)_throw((void*)Cyc_Tcutil_Unify);}goto _LLC0;_LLC3: if(*((int*)_tmp123)!= 0)
goto _LLC5;_tmp125=((struct Cyc_Absyn_Evar_struct*)_tmp123)->f2;_tmp126=((struct
Cyc_Absyn_Evar_struct*)_tmp123)->f4;_tmp127=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp123)->f4;_LLC4: if(t == evar){{const char*_tmpC47;Cyc_Tcutil_failure_reason=((
_tmpC47="(occurs check)",_tag_dynforward(_tmpC47,sizeof(char),
_get_zero_arr_size_char(_tmpC47,15))));}(int)_throw((void*)Cyc_Tcutil_Unify);}
else{if(_tmp125 != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp125->v);else{int
problem=0;{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmp127))->v;for(0;s != 0;s=s->tl){if(!((int(*)(int(*compare)(struct
Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}if(problem){struct Cyc_List_List*_tmp146=0;{struct Cyc_List_List*
s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp127))->v;for(0;s
!= 0;s=s->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
env,(struct Cyc_Absyn_Tvar*)s->hd)){struct Cyc_List_List*_tmpC48;_tmp146=((_tmpC48=
_cycalloc(sizeof(*_tmpC48)),((_tmpC48->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmpC48->tl=
_tmp146,_tmpC48))))));}}}{struct Cyc_Core_Opt*_tmpC49;*_tmp127=((_tmpC49=
_cycalloc(sizeof(*_tmpC49)),((_tmpC49->v=_tmp146,_tmpC49))));}}}}goto _LLC0;_LLC5:
if(*((int*)_tmp123)!= 4)goto _LLC7;_tmp128=((struct Cyc_Absyn_PointerType_struct*)
_tmp123)->f1;_LLC6: Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp128.elt_typ);Cyc_Tcutil_occurs(
evar,r,env,(void*)(_tmp128.ptr_atts).rgn);{union Cyc_Absyn_Constraint_union
_tmp149=(Cyc_Absyn_compress_conref((_tmp128.ptr_atts).bounds))->v;void*_tmp14A;
void*_tmp14B;_LLE6: if((_tmp149.Eq_constr).tag != 0)goto _LLE8;_tmp14A=(_tmp149.Eq_constr).f1;
if(_tmp14A <= (void*)2)goto _LLE8;if(*((int*)_tmp14A)!= 1)goto _LLE8;_tmp14B=(void*)((
struct Cyc_Absyn_AbsUpper_b_struct*)_tmp14A)->f1;_LLE7: Cyc_Tcutil_occurs(evar,r,
env,_tmp14B);goto _LLE5;_LLE8:;_LLE9: goto _LLE5;_LLE5:;}goto _LLC0;_LLC7: if(*((int*)
_tmp123)!= 7)goto _LLC9;_tmp129=((struct Cyc_Absyn_ArrayType_struct*)_tmp123)->f1;
_tmp12A=(void*)_tmp129.elt_type;_LLC8: Cyc_Tcutil_occurs(evar,r,env,_tmp12A);goto
_LLC0;_LLC9: if(*((int*)_tmp123)!= 8)goto _LLCB;_tmp12B=((struct Cyc_Absyn_FnType_struct*)
_tmp123)->f1;_tmp12C=_tmp12B.tvars;_tmp12D=_tmp12B.effect;_tmp12E=(void*)_tmp12B.ret_typ;
_tmp12F=_tmp12B.args;_tmp130=_tmp12B.c_varargs;_tmp131=_tmp12B.cyc_varargs;
_tmp132=_tmp12B.rgn_po;_tmp133=_tmp12B.attributes;_LLCA: env=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
r,_tmp12C,env);if(_tmp12D != 0)Cyc_Tcutil_occurs(evar,r,env,(void*)_tmp12D->v);
Cyc_Tcutil_occurs(evar,r,env,_tmp12E);for(0;_tmp12F != 0;_tmp12F=_tmp12F->tl){Cyc_Tcutil_occurs(
evar,r,env,(*((struct _tuple3*)_tmp12F->hd)).f3);}if(_tmp131 != 0)Cyc_Tcutil_occurs(
evar,r,env,(void*)_tmp131->type);for(0;_tmp132 != 0;_tmp132=_tmp132->tl){struct
_tuple0 _tmp14D;void*_tmp14E;void*_tmp14F;struct _tuple0*_tmp14C=(struct _tuple0*)
_tmp132->hd;_tmp14D=*_tmp14C;_tmp14E=_tmp14D.f1;_tmp14F=_tmp14D.f2;Cyc_Tcutil_occurs(
evar,r,env,_tmp14E);Cyc_Tcutil_occurs(evar,r,env,_tmp14F);}goto _LLC0;_LLCB: if(*((
int*)_tmp123)!= 9)goto _LLCD;_tmp134=((struct Cyc_Absyn_TupleType_struct*)_tmp123)->f1;
_LLCC: for(0;_tmp134 != 0;_tmp134=_tmp134->tl){Cyc_Tcutil_occurs(evar,r,env,(*((
struct _tuple5*)_tmp134->hd)).f2);}goto _LLC0;_LLCD: if(*((int*)_tmp123)!= 2)goto
_LLCF;_tmp135=((struct Cyc_Absyn_TunionType_struct*)_tmp123)->f1;_tmp136=_tmp135.targs;
_tmp137=_tmp135.rgn;_LLCE: if((unsigned int)_tmp137)Cyc_Tcutil_occurs(evar,r,env,(
void*)_tmp137->v);Cyc_Tcutil_occurslist(evar,r,env,_tmp136);goto _LLC0;_LLCF: if(*((
int*)_tmp123)!= 17)goto _LLD1;_tmp138=((struct Cyc_Absyn_TypedefType_struct*)
_tmp123)->f2;_LLD0: _tmp13A=_tmp138;goto _LLD2;_LLD1: if(*((int*)_tmp123)!= 3)goto
_LLD3;_tmp139=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp123)->f1;_tmp13A=
_tmp139.targs;_LLD2: _tmp13C=_tmp13A;goto _LLD4;_LLD3: if(*((int*)_tmp123)!= 10)
goto _LLD5;_tmp13B=((struct Cyc_Absyn_AggrType_struct*)_tmp123)->f1;_tmp13C=
_tmp13B.targs;_LLD4: Cyc_Tcutil_occurslist(evar,r,env,_tmp13C);goto _LLC0;_LLD5:
if(*((int*)_tmp123)!= 11)goto _LLD7;_tmp13D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp123)->f2;_LLD6: for(0;_tmp13D != 0;_tmp13D=_tmp13D->tl){Cyc_Tcutil_occurs(evar,
r,env,(void*)((struct Cyc_Absyn_Aggrfield*)_tmp13D->hd)->type);}goto _LLC0;_LLD7:
if(*((int*)_tmp123)!= 18)goto _LLD9;_tmp13E=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp123)->f1;_LLD8: _tmp13F=_tmp13E;goto _LLDA;_LLD9: if(*((int*)_tmp123)!= 14)goto
_LLDB;_tmp13F=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp123)->f1;_LLDA:
_tmp140=_tmp13F;goto _LLDC;_LLDB: if(*((int*)_tmp123)!= 20)goto _LLDD;_tmp140=(void*)((
struct Cyc_Absyn_AccessEff_struct*)_tmp123)->f1;_LLDC: _tmp141=_tmp140;goto _LLDE;
_LLDD: if(*((int*)_tmp123)!= 15)goto _LLDF;_tmp141=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp123)->f1;_LLDE: _tmp142=_tmp141;goto _LLE0;_LLDF: if(*((int*)_tmp123)!= 22)goto
_LLE1;_tmp142=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp123)->f1;_LLE0: Cyc_Tcutil_occurs(
evar,r,env,_tmp142);goto _LLC0;_LLE1: if(*((int*)_tmp123)!= 21)goto _LLE3;_tmp143=((
struct Cyc_Absyn_JoinEff_struct*)_tmp123)->f1;_LLE2: Cyc_Tcutil_occurslist(evar,r,
env,_tmp143);goto _LLC0;_LLE3:;_LLE4: goto _LLC0;_LLC0:;}}static void Cyc_Tcutil_occurslist(
void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*
env,struct Cyc_List_List*ts){for(0;ts != 0;ts=ts->tl){Cyc_Tcutil_occurs(evar,r,env,(
void*)ts->hd);}}static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*
t2);static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*
t2){for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){Cyc_Tcutil_unify_it((void*)t1->hd,(
void*)t2->hd);}if(t1 != 0  || t2 != 0)(int)_throw((void*)Cyc_Tcutil_Unify);}static
void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual
tq2,void*t2);static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,
struct Cyc_Absyn_Tqual tq2,void*t2){if(tq1.print_const  && !tq1.real_const){const
char*_tmpC4C;void*_tmpC4B;(_tmpC4B=0,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmpC4C="tq1 real_const not set.",
_tag_dynforward(_tmpC4C,sizeof(char),_get_zero_arr_size_char(_tmpC4C,24)))),
_tag_dynforward(_tmpC4B,sizeof(void*),0)));}if(tq2.print_const  && !tq2.real_const){
const char*_tmpC4F;void*_tmpC4E;(_tmpC4E=0,((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmpC4F="tq2 real_const not set.",
_tag_dynforward(_tmpC4F,sizeof(char),_get_zero_arr_size_char(_tmpC4F,24)))),
_tag_dynforward(_tmpC4E,sizeof(void*),0)));}if((tq1.real_const != tq2.real_const
 || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_tq1_const=tq1.real_const;Cyc_Tcutil_tq2_const=
tq2.real_const;{const char*_tmpC50;Cyc_Tcutil_failure_reason=((_tmpC50="(qualifiers don't match)",
_tag_dynforward(_tmpC50,sizeof(char),_get_zero_arr_size_char(_tmpC50,25))));}(
int)_throw((void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_tq1_const=0;Cyc_Tcutil_tq2_const=
0;}int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const  && tq1.q_volatile == tq2.q_volatile) && 
tq1.q_restrict == tq2.q_restrict;}static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(
void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct
_dynforward_ptr reason);static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,
void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y,struct _dynforward_ptr
reason){x=Cyc_Absyn_compress_conref(x);y=Cyc_Absyn_compress_conref(y);if(x == y)
return;{union Cyc_Absyn_Constraint_union _tmp155=x->v;void*_tmp156;_LLEB: if((
_tmp155.No_constr).tag != 2)goto _LLED;_LLEC:{union Cyc_Absyn_Constraint_union
_tmpC51;x->v=(union Cyc_Absyn_Constraint_union)(((_tmpC51.Forward_constr).tag=1,(((
_tmpC51.Forward_constr).f1=y,_tmpC51))));}return;_LLED: if((_tmp155.Eq_constr).tag
!= 0)goto _LLEF;_tmp156=(_tmp155.Eq_constr).f1;_LLEE: {union Cyc_Absyn_Constraint_union
_tmp158=y->v;void*_tmp159;_LLF2: if((_tmp158.No_constr).tag != 2)goto _LLF4;_LLF3: y->v=
x->v;return;_LLF4: if((_tmp158.Eq_constr).tag != 0)goto _LLF6;_tmp159=(_tmp158.Eq_constr).f1;
_LLF5: if(cmp(_tmp156,_tmp159)!= 0){Cyc_Tcutil_failure_reason=reason;(int)_throw((
void*)Cyc_Tcutil_Unify);}return;_LLF6: if((_tmp158.Forward_constr).tag != 1)goto
_LLF1;_LLF7: {const char*_tmpC54;void*_tmpC53;(_tmpC53=0,Cyc_Tcutil_impos(((
_tmpC54="unify_conref: forward after compress(2)",_tag_dynforward(_tmpC54,
sizeof(char),_get_zero_arr_size_char(_tmpC54,40)))),_tag_dynforward(_tmpC53,
sizeof(void*),0)));}_LLF1:;}_LLEF: if((_tmp155.Forward_constr).tag != 1)goto _LLEA;
_LLF0: {const char*_tmpC57;void*_tmpC56;(_tmpC56=0,Cyc_Tcutil_impos(((_tmpC57="unify_conref: forward after compress",
_tag_dynforward(_tmpC57,sizeof(char),_get_zero_arr_size_char(_tmpC57,37)))),
_tag_dynforward(_tmpC56,sizeof(void*),0)));}_LLEA:;}}static int Cyc_Tcutil_unify_conrefs(
int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y);static
int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y){struct _handler_cons _tmp15E;_push_handler(& _tmp15E);{
int _tmp160=0;if(setjmp(_tmp15E.handler))_tmp160=1;if(!_tmp160){Cyc_Tcutil_unify_it_conrefs(
cmp,x,y,(struct _dynforward_ptr)_tag_dynforward(0,0,0));{int _tmp161=1;
_npop_handler(0);return _tmp161;};_pop_handler();}else{void*_tmp15F=(void*)
_exn_thrown;void*_tmp163=_tmp15F;_LLF9: if(_tmp163 != Cyc_Tcutil_Unify)goto _LLFB;
_LLFA: return 0;_LLFB:;_LLFC:(void)_throw(_tmp163);_LLF8:;}}}static int Cyc_Tcutil_boundscmp(
void*b1,void*b2);static int Cyc_Tcutil_boundscmp(void*b1,void*b2){struct _tuple0
_tmpC58;struct _tuple0 _tmp165=(_tmpC58.f1=b1,((_tmpC58.f2=b2,_tmpC58)));void*
_tmp166;void*_tmp167;void*_tmp168;void*_tmp169;void*_tmp16A;void*_tmp16B;void*
_tmp16C;void*_tmp16D;void*_tmp16E;struct Cyc_Absyn_Exp*_tmp16F;void*_tmp170;
struct Cyc_Absyn_Exp*_tmp171;void*_tmp172;void*_tmp173;void*_tmp174;void*_tmp175;
void*_tmp176;void*_tmp177;void*_tmp178;void*_tmp179;_LLFE: _tmp166=_tmp165.f1;if((
int)_tmp166 != 0)goto _LL100;_tmp167=_tmp165.f2;if((int)_tmp167 != 0)goto _LL100;
_LLFF: return 0;_LL100: _tmp168=_tmp165.f1;if((int)_tmp168 != 0)goto _LL102;_LL101:
return - 1;_LL102: _tmp169=_tmp165.f2;if((int)_tmp169 != 0)goto _LL104;_LL103: return 1;
_LL104: _tmp16A=_tmp165.f1;if((int)_tmp16A != 1)goto _LL106;_tmp16B=_tmp165.f2;if((
int)_tmp16B != 1)goto _LL106;_LL105: return 0;_LL106: _tmp16C=_tmp165.f1;if((int)
_tmp16C != 1)goto _LL108;_LL107: return - 1;_LL108: _tmp16D=_tmp165.f2;if((int)_tmp16D
!= 1)goto _LL10A;_LL109: return 1;_LL10A: _tmp16E=_tmp165.f1;if(_tmp16E <= (void*)2)
goto _LL10C;if(*((int*)_tmp16E)!= 0)goto _LL10C;_tmp16F=((struct Cyc_Absyn_Upper_b_struct*)
_tmp16E)->f1;_tmp170=_tmp165.f2;if(_tmp170 <= (void*)2)goto _LL10C;if(*((int*)
_tmp170)!= 0)goto _LL10C;_tmp171=((struct Cyc_Absyn_Upper_b_struct*)_tmp170)->f1;
_LL10B: return Cyc_Evexp_const_exp_cmp(_tmp16F,_tmp171);_LL10C: _tmp172=_tmp165.f1;
if(_tmp172 <= (void*)2)goto _LL10E;if(*((int*)_tmp172)!= 0)goto _LL10E;_tmp173=
_tmp165.f2;if(_tmp173 <= (void*)2)goto _LL10E;if(*((int*)_tmp173)!= 1)goto _LL10E;
_LL10D: return - 1;_LL10E: _tmp174=_tmp165.f1;if(_tmp174 <= (void*)2)goto _LL110;if(*((
int*)_tmp174)!= 1)goto _LL110;_tmp175=_tmp165.f2;if(_tmp175 <= (void*)2)goto _LL110;
if(*((int*)_tmp175)!= 0)goto _LL110;_LL10F: return 1;_LL110: _tmp176=_tmp165.f1;if(
_tmp176 <= (void*)2)goto _LLFD;if(*((int*)_tmp176)!= 1)goto _LLFD;_tmp177=(void*)((
struct Cyc_Absyn_AbsUpper_b_struct*)_tmp176)->f1;_tmp178=_tmp165.f2;if(_tmp178 <= (
void*)2)goto _LLFD;if(*((int*)_tmp178)!= 1)goto _LLFD;_tmp179=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp178)->f1;_LL111: return Cyc_Tcutil_typecmp(_tmp177,_tmp179);_LLFD:;}static int
Cyc_Tcutil_unify_it_bounds(void*b1,void*b2);static int Cyc_Tcutil_unify_it_bounds(
void*b1,void*b2){struct _tuple0 _tmpC59;struct _tuple0 _tmp17B=(_tmpC59.f1=b1,((
_tmpC59.f2=b2,_tmpC59)));void*_tmp17C;void*_tmp17D;void*_tmp17E;void*_tmp17F;
void*_tmp180;void*_tmp181;void*_tmp182;void*_tmp183;void*_tmp184;struct Cyc_Absyn_Exp*
_tmp185;void*_tmp186;struct Cyc_Absyn_Exp*_tmp187;void*_tmp188;struct Cyc_Absyn_Exp*
_tmp189;void*_tmp18A;void*_tmp18B;void*_tmp18C;void*_tmp18D;void*_tmp18E;struct
Cyc_Absyn_Exp*_tmp18F;void*_tmp190;void*_tmp191;void*_tmp192;void*_tmp193;_LL113:
_tmp17C=_tmp17B.f1;if((int)_tmp17C != 0)goto _LL115;_tmp17D=_tmp17B.f2;if((int)
_tmp17D != 0)goto _LL115;_LL114: return 0;_LL115: _tmp17E=_tmp17B.f1;if((int)_tmp17E
!= 0)goto _LL117;_LL116: return - 1;_LL117: _tmp17F=_tmp17B.f2;if((int)_tmp17F != 0)
goto _LL119;_LL118: return 1;_LL119: _tmp180=_tmp17B.f1;if((int)_tmp180 != 1)goto
_LL11B;_tmp181=_tmp17B.f2;if((int)_tmp181 != 1)goto _LL11B;_LL11A: return 0;_LL11B:
_tmp182=_tmp17B.f1;if((int)_tmp182 != 1)goto _LL11D;_LL11C: return - 1;_LL11D: _tmp183=
_tmp17B.f2;if((int)_tmp183 != 1)goto _LL11F;_LL11E: return 1;_LL11F: _tmp184=_tmp17B.f1;
if(_tmp184 <= (void*)2)goto _LL121;if(*((int*)_tmp184)!= 0)goto _LL121;_tmp185=((
struct Cyc_Absyn_Upper_b_struct*)_tmp184)->f1;_tmp186=_tmp17B.f2;if(_tmp186 <= (
void*)2)goto _LL121;if(*((int*)_tmp186)!= 0)goto _LL121;_tmp187=((struct Cyc_Absyn_Upper_b_struct*)
_tmp186)->f1;_LL120: return Cyc_Evexp_const_exp_cmp(_tmp185,_tmp187);_LL121:
_tmp188=_tmp17B.f1;if(_tmp188 <= (void*)2)goto _LL123;if(*((int*)_tmp188)!= 0)goto
_LL123;_tmp189=((struct Cyc_Absyn_Upper_b_struct*)_tmp188)->f1;_tmp18A=_tmp17B.f2;
if(_tmp18A <= (void*)2)goto _LL123;if(*((int*)_tmp18A)!= 1)goto _LL123;_tmp18B=(
void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp18A)->f1;_LL122: _tmp18D=_tmp18B;
_tmp18F=_tmp189;goto _LL124;_LL123: _tmp18C=_tmp17B.f1;if(_tmp18C <= (void*)2)goto
_LL125;if(*((int*)_tmp18C)!= 1)goto _LL125;_tmp18D=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp18C)->f1;_tmp18E=_tmp17B.f2;if(_tmp18E <= (void*)2)goto _LL125;if(*((int*)
_tmp18E)!= 0)goto _LL125;_tmp18F=((struct Cyc_Absyn_Upper_b_struct*)_tmp18E)->f1;
_LL124: {unsigned int _tmp195;int _tmp196;struct _tuple7 _tmp194=Cyc_Evexp_eval_const_uint_exp(
_tmp18F);_tmp195=_tmp194.f1;_tmp196=_tmp194.f2;if(!_tmp196)return 1;_tmp191=
_tmp18D;{struct Cyc_Absyn_TypeInt_struct _tmpC5C;struct Cyc_Absyn_TypeInt_struct*
_tmpC5B;_tmp193=(void*)((_tmpC5B=_cycalloc_atomic(sizeof(*_tmpC5B)),((_tmpC5B[0]=((
_tmpC5C.tag=19,((_tmpC5C.f1=(int)_tmp195,_tmpC5C)))),_tmpC5B))));}goto _LL126;}
_LL125: _tmp190=_tmp17B.f1;if(_tmp190 <= (void*)2)goto _LL112;if(*((int*)_tmp190)!= 
1)goto _LL112;_tmp191=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp190)->f1;
_tmp192=_tmp17B.f2;if(_tmp192 <= (void*)2)goto _LL112;if(*((int*)_tmp192)!= 1)goto
_LL112;_tmp193=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp192)->f1;_LL126:
Cyc_Tcutil_unify_it(_tmp191,_tmp193);return 0;_LL112:;}static int Cyc_Tcutil_attribute_case_number(
void*att);static int Cyc_Tcutil_attribute_case_number(void*att){void*_tmp199=att;
_LL128: if(_tmp199 <= (void*)17)goto _LL12A;if(*((int*)_tmp199)!= 0)goto _LL12A;
_LL129: return 0;_LL12A: if((int)_tmp199 != 0)goto _LL12C;_LL12B: return 1;_LL12C: if((
int)_tmp199 != 1)goto _LL12E;_LL12D: return 2;_LL12E: if((int)_tmp199 != 2)goto _LL130;
_LL12F: return 3;_LL130: if((int)_tmp199 != 3)goto _LL132;_LL131: return 4;_LL132: if((
int)_tmp199 != 4)goto _LL134;_LL133: return 5;_LL134: if(_tmp199 <= (void*)17)goto
_LL136;if(*((int*)_tmp199)!= 1)goto _LL136;_LL135: return 6;_LL136: if((int)_tmp199
!= 5)goto _LL138;_LL137: return 7;_LL138: if(_tmp199 <= (void*)17)goto _LL13A;if(*((
int*)_tmp199)!= 2)goto _LL13A;_LL139: return 8;_LL13A: if((int)_tmp199 != 6)goto
_LL13C;_LL13B: return 9;_LL13C: if((int)_tmp199 != 7)goto _LL13E;_LL13D: return 10;
_LL13E: if((int)_tmp199 != 8)goto _LL140;_LL13F: return 11;_LL140: if((int)_tmp199 != 9)
goto _LL142;_LL141: return 12;_LL142: if((int)_tmp199 != 10)goto _LL144;_LL143: return
13;_LL144: if((int)_tmp199 != 11)goto _LL146;_LL145: return 14;_LL146: if((int)_tmp199
!= 12)goto _LL148;_LL147: return 15;_LL148: if((int)_tmp199 != 13)goto _LL14A;_LL149:
return 16;_LL14A: if((int)_tmp199 != 14)goto _LL14C;_LL14B: return 17;_LL14C: if((int)
_tmp199 != 15)goto _LL14E;_LL14D: return 18;_LL14E: if(_tmp199 <= (void*)17)goto _LL152;
if(*((int*)_tmp199)!= 3)goto _LL150;_LL14F: return 19;_LL150: if(*((int*)_tmp199)!= 
4)goto _LL152;_LL151: return 20;_LL152:;_LL153: return 21;_LL127:;}static int Cyc_Tcutil_attribute_cmp(
void*att1,void*att2);static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmpC5D;struct _tuple0 _tmp19B=(_tmpC5D.f1=att1,((_tmpC5D.f2=att2,
_tmpC5D)));void*_tmp19C;int _tmp19D;void*_tmp19E;int _tmp19F;void*_tmp1A0;int
_tmp1A1;void*_tmp1A2;int _tmp1A3;void*_tmp1A4;int _tmp1A5;void*_tmp1A6;int _tmp1A7;
void*_tmp1A8;struct _dynforward_ptr _tmp1A9;void*_tmp1AA;struct _dynforward_ptr
_tmp1AB;void*_tmp1AC;void*_tmp1AD;int _tmp1AE;int _tmp1AF;void*_tmp1B0;void*
_tmp1B1;int _tmp1B2;int _tmp1B3;_LL155: _tmp19C=_tmp19B.f1;if(_tmp19C <= (void*)17)
goto _LL157;if(*((int*)_tmp19C)!= 0)goto _LL157;_tmp19D=((struct Cyc_Absyn_Regparm_att_struct*)
_tmp19C)->f1;_tmp19E=_tmp19B.f2;if(_tmp19E <= (void*)17)goto _LL157;if(*((int*)
_tmp19E)!= 0)goto _LL157;_tmp19F=((struct Cyc_Absyn_Regparm_att_struct*)_tmp19E)->f1;
_LL156: _tmp1A1=_tmp19D;_tmp1A3=_tmp19F;goto _LL158;_LL157: _tmp1A0=_tmp19B.f1;if(
_tmp1A0 <= (void*)17)goto _LL159;if(*((int*)_tmp1A0)!= 4)goto _LL159;_tmp1A1=((
struct Cyc_Absyn_Initializes_att_struct*)_tmp1A0)->f1;_tmp1A2=_tmp19B.f2;if(
_tmp1A2 <= (void*)17)goto _LL159;if(*((int*)_tmp1A2)!= 4)goto _LL159;_tmp1A3=((
struct Cyc_Absyn_Initializes_att_struct*)_tmp1A2)->f1;_LL158: _tmp1A5=_tmp1A1;
_tmp1A7=_tmp1A3;goto _LL15A;_LL159: _tmp1A4=_tmp19B.f1;if(_tmp1A4 <= (void*)17)goto
_LL15B;if(*((int*)_tmp1A4)!= 1)goto _LL15B;_tmp1A5=((struct Cyc_Absyn_Aligned_att_struct*)
_tmp1A4)->f1;_tmp1A6=_tmp19B.f2;if(_tmp1A6 <= (void*)17)goto _LL15B;if(*((int*)
_tmp1A6)!= 1)goto _LL15B;_tmp1A7=((struct Cyc_Absyn_Aligned_att_struct*)_tmp1A6)->f1;
_LL15A: return Cyc_Core_intcmp(_tmp1A5,_tmp1A7);_LL15B: _tmp1A8=_tmp19B.f1;if(
_tmp1A8 <= (void*)17)goto _LL15D;if(*((int*)_tmp1A8)!= 2)goto _LL15D;_tmp1A9=((
struct Cyc_Absyn_Section_att_struct*)_tmp1A8)->f1;_tmp1AA=_tmp19B.f2;if(_tmp1AA <= (
void*)17)goto _LL15D;if(*((int*)_tmp1AA)!= 2)goto _LL15D;_tmp1AB=((struct Cyc_Absyn_Section_att_struct*)
_tmp1AA)->f1;_LL15C: return Cyc_strcmp((struct _dynforward_ptr)_tmp1A9,(struct
_dynforward_ptr)_tmp1AB);_LL15D: _tmp1AC=_tmp19B.f1;if(_tmp1AC <= (void*)17)goto
_LL15F;if(*((int*)_tmp1AC)!= 3)goto _LL15F;_tmp1AD=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp1AC)->f1;_tmp1AE=((struct Cyc_Absyn_Format_att_struct*)_tmp1AC)->f2;_tmp1AF=((
struct Cyc_Absyn_Format_att_struct*)_tmp1AC)->f3;_tmp1B0=_tmp19B.f2;if(_tmp1B0 <= (
void*)17)goto _LL15F;if(*((int*)_tmp1B0)!= 3)goto _LL15F;_tmp1B1=(void*)((struct
Cyc_Absyn_Format_att_struct*)_tmp1B0)->f1;_tmp1B2=((struct Cyc_Absyn_Format_att_struct*)
_tmp1B0)->f2;_tmp1B3=((struct Cyc_Absyn_Format_att_struct*)_tmp1B0)->f3;_LL15E: {
int _tmp1B4=Cyc_Core_intcmp((int)((unsigned int)_tmp1AD),(int)((unsigned int)
_tmp1B1));if(_tmp1B4 != 0)return _tmp1B4;{int _tmp1B5=Cyc_Core_intcmp(_tmp1AE,
_tmp1B2);if(_tmp1B5 != 0)return _tmp1B5;return Cyc_Core_intcmp(_tmp1AF,_tmp1B3);}}
_LL15F:;_LL160: return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(
att2));_LL154:;}static int Cyc_Tcutil_equal_att(void*a1,void*a2);static int Cyc_Tcutil_equal_att(
void*a1,void*a2){return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}int Cyc_Tcutil_same_atts(
struct Cyc_List_List*a1,struct Cyc_List_List*a2);int Cyc_Tcutil_same_atts(struct Cyc_List_List*
a1,struct Cyc_List_List*a2){{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){if(!
Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}{struct Cyc_List_List*
a=a2;for(0;a != 0;a=a->tl){if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)a->hd,
a1))return 0;}}return 1;}static void*Cyc_Tcutil_rgns_of(void*t);static void*Cyc_Tcutil_rgns_of_field(
struct Cyc_Absyn_Aggrfield*af);static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*
af){return Cyc_Tcutil_rgns_of((void*)af->type);}static struct Cyc_Absyn_TypeInt_struct
Cyc_Tcutil_tizero={19,0};static struct _tuple8*Cyc_Tcutil_region_free_subst(struct
Cyc_Absyn_Tvar*tv);static struct _tuple8*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*
tv){void*t;{void*_tmp1B7=Cyc_Tcutil_tvar_kind(tv);_LL162: if((int)_tmp1B7 != 4)
goto _LL164;_LL163: t=(void*)3;goto _LL161;_LL164: if((int)_tmp1B7 != 3)goto _LL166;
_LL165: t=(void*)2;goto _LL161;_LL166: if((int)_tmp1B7 != 6)goto _LL168;_LL167: t=Cyc_Absyn_empty_effect;
goto _LL161;_LL168: if((int)_tmp1B7 != 7)goto _LL16A;_LL169: t=(void*)& Cyc_Tcutil_tizero;
goto _LL161;_LL16A:;_LL16B: t=Cyc_Absyn_sint_typ;goto _LL161;_LL161:;}{struct
_tuple8*_tmpC5E;return(_tmpC5E=_cycalloc(sizeof(*_tmpC5E)),((_tmpC5E->f1=tv,((
_tmpC5E->f2=t,_tmpC5E)))));}}static void*Cyc_Tcutil_rgns_of(void*t);static void*
Cyc_Tcutil_rgns_of(void*t){void*_tmp1B9=Cyc_Tcutil_compress(t);void*_tmp1BA;void*
_tmp1BB;void*_tmp1BC;struct Cyc_Absyn_TunionInfo _tmp1BD;struct Cyc_List_List*
_tmp1BE;struct Cyc_Core_Opt*_tmp1BF;struct Cyc_Absyn_PtrInfo _tmp1C0;void*_tmp1C1;
struct Cyc_Absyn_PtrAtts _tmp1C2;void*_tmp1C3;struct Cyc_Absyn_ArrayInfo _tmp1C4;
void*_tmp1C5;struct Cyc_List_List*_tmp1C6;struct Cyc_Absyn_TunionFieldInfo _tmp1C7;
struct Cyc_List_List*_tmp1C8;struct Cyc_Absyn_AggrInfo _tmp1C9;struct Cyc_List_List*
_tmp1CA;struct Cyc_List_List*_tmp1CB;void*_tmp1CC;struct Cyc_Absyn_FnInfo _tmp1CD;
struct Cyc_List_List*_tmp1CE;struct Cyc_Core_Opt*_tmp1CF;void*_tmp1D0;struct Cyc_List_List*
_tmp1D1;struct Cyc_Absyn_VarargInfo*_tmp1D2;struct Cyc_List_List*_tmp1D3;void*
_tmp1D4;struct Cyc_List_List*_tmp1D5;_LL16D: if((int)_tmp1B9 != 0)goto _LL16F;_LL16E:
goto _LL170;_LL16F: if((int)_tmp1B9 != 1)goto _LL171;_LL170: goto _LL172;_LL171: if(
_tmp1B9 <= (void*)4)goto _LL197;if(*((int*)_tmp1B9)!= 6)goto _LL173;_LL172: goto
_LL174;_LL173: if(*((int*)_tmp1B9)!= 12)goto _LL175;_LL174: goto _LL176;_LL175: if(*((
int*)_tmp1B9)!= 13)goto _LL177;_LL176: goto _LL178;_LL177: if(*((int*)_tmp1B9)!= 19)
goto _LL179;_LL178: goto _LL17A;_LL179: if(*((int*)_tmp1B9)!= 5)goto _LL17B;_LL17A:
return Cyc_Absyn_empty_effect;_LL17B: if(*((int*)_tmp1B9)!= 0)goto _LL17D;_LL17C:
goto _LL17E;_LL17D: if(*((int*)_tmp1B9)!= 1)goto _LL17F;_LL17E: {void*_tmp1D6=Cyc_Tcutil_typ_kind(
t);_LL1A4: if((int)_tmp1D6 != 3)goto _LL1A6;_LL1A5: goto _LL1A7;_LL1A6: if((int)
_tmp1D6 != 4)goto _LL1A8;_LL1A7: goto _LL1A9;_LL1A8: if((int)_tmp1D6 != 5)goto _LL1AA;
_LL1A9: {struct Cyc_Absyn_AccessEff_struct _tmpC61;struct Cyc_Absyn_AccessEff_struct*
_tmpC60;return(void*)((_tmpC60=_cycalloc(sizeof(*_tmpC60)),((_tmpC60[0]=((
_tmpC61.tag=20,((_tmpC61.f1=(void*)t,_tmpC61)))),_tmpC60))));}_LL1AA: if((int)
_tmp1D6 != 6)goto _LL1AC;_LL1AB: return t;_LL1AC: if((int)_tmp1D6 != 7)goto _LL1AE;
_LL1AD: return Cyc_Absyn_empty_effect;_LL1AE:;_LL1AF: {struct Cyc_Absyn_RgnsEff_struct
_tmpC64;struct Cyc_Absyn_RgnsEff_struct*_tmpC63;return(void*)((_tmpC63=_cycalloc(
sizeof(*_tmpC63)),((_tmpC63[0]=((_tmpC64.tag=22,((_tmpC64.f1=(void*)t,_tmpC64)))),
_tmpC63))));}_LL1A3:;}_LL17F: if(*((int*)_tmp1B9)!= 15)goto _LL181;_tmp1BA=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp1B9)->f1;_LL180: {struct Cyc_Absyn_AccessEff_struct
_tmpC67;struct Cyc_Absyn_AccessEff_struct*_tmpC66;return(void*)((_tmpC66=
_cycalloc(sizeof(*_tmpC66)),((_tmpC66[0]=((_tmpC67.tag=20,((_tmpC67.f1=(void*)
_tmp1BA,_tmpC67)))),_tmpC66))));}_LL181: if(*((int*)_tmp1B9)!= 16)goto _LL183;
_tmp1BB=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp1B9)->f1;_tmp1BC=(void*)((
struct Cyc_Absyn_DynRgnType_struct*)_tmp1B9)->f2;_LL182: {struct Cyc_Absyn_AccessEff_struct
_tmpC6A;struct Cyc_Absyn_AccessEff_struct*_tmpC69;return(void*)((_tmpC69=
_cycalloc(sizeof(*_tmpC69)),((_tmpC69[0]=((_tmpC6A.tag=20,((_tmpC6A.f1=(void*)
_tmp1BC,_tmpC6A)))),_tmpC69))));}_LL183: if(*((int*)_tmp1B9)!= 2)goto _LL185;
_tmp1BD=((struct Cyc_Absyn_TunionType_struct*)_tmp1B9)->f1;_tmp1BE=_tmp1BD.targs;
_tmp1BF=_tmp1BD.rgn;_LL184: {struct Cyc_List_List*ts=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp1BE);if((unsigned int)_tmp1BF){struct Cyc_Absyn_AccessEff_struct*_tmpC70;
struct Cyc_Absyn_AccessEff_struct _tmpC6F;struct Cyc_List_List*_tmpC6E;ts=((_tmpC6E=
_cycalloc(sizeof(*_tmpC6E)),((_tmpC6E->hd=(void*)((void*)((_tmpC70=_cycalloc(
sizeof(*_tmpC70)),((_tmpC70[0]=((_tmpC6F.tag=20,((_tmpC6F.f1=(void*)((void*)
_tmp1BF->v),_tmpC6F)))),_tmpC70))))),((_tmpC6E->tl=ts,_tmpC6E))))));}{struct Cyc_Absyn_JoinEff_struct
_tmpC73;struct Cyc_Absyn_JoinEff_struct*_tmpC72;return Cyc_Tcutil_normalize_effect((
void*)((_tmpC72=_cycalloc(sizeof(*_tmpC72)),((_tmpC72[0]=((_tmpC73.tag=21,((
_tmpC73.f1=ts,_tmpC73)))),_tmpC72)))));}}_LL185: if(*((int*)_tmp1B9)!= 4)goto
_LL187;_tmp1C0=((struct Cyc_Absyn_PointerType_struct*)_tmp1B9)->f1;_tmp1C1=(void*)
_tmp1C0.elt_typ;_tmp1C2=_tmp1C0.ptr_atts;_tmp1C3=(void*)_tmp1C2.rgn;_LL186: {
struct Cyc_Absyn_JoinEff_struct _tmpC82;struct Cyc_Absyn_AccessEff_struct*_tmpC81;
struct Cyc_Absyn_AccessEff_struct _tmpC80;void*_tmpC7F[2];struct Cyc_Absyn_JoinEff_struct*
_tmpC7E;return Cyc_Tcutil_normalize_effect((void*)((_tmpC7E=_cycalloc(sizeof(*
_tmpC7E)),((_tmpC7E[0]=((_tmpC82.tag=21,((_tmpC82.f1=((_tmpC7F[1]=Cyc_Tcutil_rgns_of(
_tmp1C1),((_tmpC7F[0]=(void*)((_tmpC81=_cycalloc(sizeof(*_tmpC81)),((_tmpC81[0]=((
_tmpC80.tag=20,((_tmpC80.f1=(void*)_tmp1C3,_tmpC80)))),_tmpC81)))),Cyc_List_list(
_tag_dynforward(_tmpC7F,sizeof(void*),2)))))),_tmpC82)))),_tmpC7E)))));}_LL187:
if(*((int*)_tmp1B9)!= 7)goto _LL189;_tmp1C4=((struct Cyc_Absyn_ArrayType_struct*)
_tmp1B9)->f1;_tmp1C5=(void*)_tmp1C4.elt_type;_LL188: return Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_rgns_of(_tmp1C5));_LL189: if(*((int*)_tmp1B9)!= 9)goto _LL18B;_tmp1C6=((
struct Cyc_Absyn_TupleType_struct*)_tmp1B9)->f1;_LL18A: {struct Cyc_List_List*
_tmp1E9=0;for(0;_tmp1C6 != 0;_tmp1C6=_tmp1C6->tl){struct Cyc_List_List*_tmpC83;
_tmp1E9=((_tmpC83=_cycalloc(sizeof(*_tmpC83)),((_tmpC83->hd=(void*)(*((struct
_tuple5*)_tmp1C6->hd)).f2,((_tmpC83->tl=_tmp1E9,_tmpC83))))));}_tmp1C8=_tmp1E9;
goto _LL18C;}_LL18B: if(*((int*)_tmp1B9)!= 3)goto _LL18D;_tmp1C7=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp1B9)->f1;_tmp1C8=_tmp1C7.targs;_LL18C: _tmp1CA=_tmp1C8;goto _LL18E;_LL18D: if(*((
int*)_tmp1B9)!= 10)goto _LL18F;_tmp1C9=((struct Cyc_Absyn_AggrType_struct*)_tmp1B9)->f1;
_tmp1CA=_tmp1C9.targs;_LL18E: {struct Cyc_Absyn_JoinEff_struct _tmpC86;struct Cyc_Absyn_JoinEff_struct*
_tmpC85;return Cyc_Tcutil_normalize_effect((void*)((_tmpC85=_cycalloc(sizeof(*
_tmpC85)),((_tmpC85[0]=((_tmpC86.tag=21,((_tmpC86.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp1CA),_tmpC86)))),_tmpC85)))));}_LL18F: if(*((int*)_tmp1B9)!= 11)goto _LL191;
_tmp1CB=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp1B9)->f2;_LL190: {struct Cyc_Absyn_JoinEff_struct
_tmpC89;struct Cyc_Absyn_JoinEff_struct*_tmpC88;return Cyc_Tcutil_normalize_effect((
void*)((_tmpC88=_cycalloc(sizeof(*_tmpC88)),((_tmpC88[0]=((_tmpC89.tag=21,((
_tmpC89.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp1CB),_tmpC89)))),
_tmpC88)))));}_LL191: if(*((int*)_tmp1B9)!= 14)goto _LL193;_tmp1CC=(void*)((struct
Cyc_Absyn_SizeofType_struct*)_tmp1B9)->f1;_LL192: return Cyc_Tcutil_rgns_of(
_tmp1CC);_LL193: if(*((int*)_tmp1B9)!= 18)goto _LL195;_LL194: return Cyc_Absyn_empty_effect;
_LL195: if(*((int*)_tmp1B9)!= 8)goto _LL197;_tmp1CD=((struct Cyc_Absyn_FnType_struct*)
_tmp1B9)->f1;_tmp1CE=_tmp1CD.tvars;_tmp1CF=_tmp1CD.effect;_tmp1D0=(void*)_tmp1CD.ret_typ;
_tmp1D1=_tmp1CD.args;_tmp1D2=_tmp1CD.cyc_varargs;_tmp1D3=_tmp1CD.rgn_po;_LL196: {
void*_tmp1EF=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple8*(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,
_tmp1CE),(void*)((struct Cyc_Core_Opt*)_check_null(_tmp1CF))->v);return Cyc_Tcutil_normalize_effect(
_tmp1EF);}_LL197: if((int)_tmp1B9 != 3)goto _LL199;_LL198: goto _LL19A;_LL199: if((int)
_tmp1B9 != 2)goto _LL19B;_LL19A: return Cyc_Absyn_empty_effect;_LL19B: if(_tmp1B9 <= (
void*)4)goto _LL19D;if(*((int*)_tmp1B9)!= 20)goto _LL19D;_LL19C: goto _LL19E;_LL19D:
if(_tmp1B9 <= (void*)4)goto _LL19F;if(*((int*)_tmp1B9)!= 21)goto _LL19F;_LL19E:
return t;_LL19F: if(_tmp1B9 <= (void*)4)goto _LL1A1;if(*((int*)_tmp1B9)!= 22)goto
_LL1A1;_tmp1D4=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1B9)->f1;_LL1A0:
return Cyc_Tcutil_rgns_of(_tmp1D4);_LL1A1: if(_tmp1B9 <= (void*)4)goto _LL16C;if(*((
int*)_tmp1B9)!= 17)goto _LL16C;_tmp1D5=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1B9)->f2;_LL1A2: {struct Cyc_Absyn_JoinEff_struct _tmpC8C;struct Cyc_Absyn_JoinEff_struct*
_tmpC8B;return Cyc_Tcutil_normalize_effect((void*)((_tmpC8B=_cycalloc(sizeof(*
_tmpC8B)),((_tmpC8B[0]=((_tmpC8C.tag=21,((_tmpC8C.f1=Cyc_List_map(Cyc_Tcutil_rgns_of,
_tmp1D5),_tmpC8C)))),_tmpC8B)))));}_LL16C:;}void*Cyc_Tcutil_normalize_effect(
void*e);void*Cyc_Tcutil_normalize_effect(void*e){e=Cyc_Tcutil_compress(e);{void*
_tmp1F2=e;struct Cyc_List_List*_tmp1F3;struct Cyc_List_List**_tmp1F4;void*_tmp1F5;
_LL1B1: if(_tmp1F2 <= (void*)4)goto _LL1B5;if(*((int*)_tmp1F2)!= 21)goto _LL1B3;
_tmp1F3=((struct Cyc_Absyn_JoinEff_struct*)_tmp1F2)->f1;_tmp1F4=(struct Cyc_List_List**)&((
struct Cyc_Absyn_JoinEff_struct*)_tmp1F2)->f1;_LL1B2: {int redo_join=0;{struct Cyc_List_List*
effs=*_tmp1F4;for(0;effs != 0;effs=effs->tl){void*_tmp1F6=(void*)effs->hd;(void*)(
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp1F6)));{void*
_tmp1F7=(void*)effs->hd;void*_tmp1F8;_LL1B8: if(_tmp1F7 <= (void*)4)goto _LL1BC;if(*((
int*)_tmp1F7)!= 21)goto _LL1BA;_LL1B9: goto _LL1BB;_LL1BA: if(*((int*)_tmp1F7)!= 20)
goto _LL1BC;_tmp1F8=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp1F7)->f1;if((
int)_tmp1F8 != 2)goto _LL1BC;_LL1BB: redo_join=1;goto _LL1B7;_LL1BC:;_LL1BD: goto
_LL1B7;_LL1B7:;}}}if(!redo_join)return e;{struct Cyc_List_List*effects=0;{struct
Cyc_List_List*effs=*_tmp1F4;for(0;effs != 0;effs=effs->tl){void*_tmp1F9=Cyc_Tcutil_compress((
void*)effs->hd);struct Cyc_List_List*_tmp1FA;void*_tmp1FB;_LL1BF: if(_tmp1F9 <= (
void*)4)goto _LL1C3;if(*((int*)_tmp1F9)!= 21)goto _LL1C1;_tmp1FA=((struct Cyc_Absyn_JoinEff_struct*)
_tmp1F9)->f1;_LL1C0: effects=Cyc_List_revappend(_tmp1FA,effects);goto _LL1BE;
_LL1C1: if(*((int*)_tmp1F9)!= 20)goto _LL1C3;_tmp1FB=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp1F9)->f1;if((int)_tmp1FB != 2)goto _LL1C3;_LL1C2: goto _LL1BE;_LL1C3:;_LL1C4:{
struct Cyc_List_List*_tmpC8D;effects=((_tmpC8D=_cycalloc(sizeof(*_tmpC8D)),((
_tmpC8D->hd=(void*)_tmp1F9,((_tmpC8D->tl=effects,_tmpC8D))))));}goto _LL1BE;
_LL1BE:;}}*_tmp1F4=Cyc_List_imp_rev(effects);return e;}}_LL1B3: if(*((int*)_tmp1F2)
!= 22)goto _LL1B5;_tmp1F5=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp1F2)->f1;
_LL1B4: {void*_tmp1FD=Cyc_Tcutil_compress(_tmp1F5);_LL1C6: if(_tmp1FD <= (void*)4)
goto _LL1CA;if(*((int*)_tmp1FD)!= 0)goto _LL1C8;_LL1C7: goto _LL1C9;_LL1C8: if(*((int*)
_tmp1FD)!= 1)goto _LL1CA;_LL1C9: return e;_LL1CA:;_LL1CB: return Cyc_Tcutil_rgns_of(
_tmp1F5);_LL1C5:;}_LL1B5:;_LL1B6: return e;_LL1B0:;}}static void*Cyc_Tcutil_dummy_fntype(
void*eff);static void*Cyc_Tcutil_dummy_fntype(void*eff){struct Cyc_Absyn_FnType_struct
_tmpC97;struct Cyc_Core_Opt*_tmpC96;struct Cyc_Absyn_FnInfo _tmpC95;struct Cyc_Absyn_FnType_struct*
_tmpC94;struct Cyc_Absyn_FnType_struct*_tmp1FE=(_tmpC94=_cycalloc(sizeof(*_tmpC94)),((
_tmpC94[0]=((_tmpC97.tag=8,((_tmpC97.f1=((_tmpC95.tvars=0,((_tmpC95.effect=((
_tmpC96=_cycalloc(sizeof(*_tmpC96)),((_tmpC96->v=(void*)eff,_tmpC96)))),((
_tmpC95.ret_typ=(void*)((void*)0),((_tmpC95.args=0,((_tmpC95.c_varargs=0,((
_tmpC95.cyc_varargs=0,((_tmpC95.rgn_po=0,((_tmpC95.attributes=0,_tmpC95)))))))))))))))),
_tmpC97)))),_tmpC94)));return Cyc_Absyn_atb_typ((void*)_tmp1FE,(void*)2,Cyc_Absyn_empty_tqual(
0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}int Cyc_Tcutil_region_in_effect(
int constrain,void*r,void*e);int Cyc_Tcutil_region_in_effect(int constrain,void*r,
void*e){r=Cyc_Tcutil_compress(r);if(r == (void*)2  || r == (void*)3)return 1;{void*
_tmp203=Cyc_Tcutil_compress(e);void*_tmp204;struct Cyc_List_List*_tmp205;void*
_tmp206;struct Cyc_Core_Opt*_tmp207;struct Cyc_Core_Opt*_tmp208;struct Cyc_Core_Opt**
_tmp209;struct Cyc_Core_Opt*_tmp20A;_LL1CD: if(_tmp203 <= (void*)4)goto _LL1D5;if(*((
int*)_tmp203)!= 20)goto _LL1CF;_tmp204=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp203)->f1;_LL1CE: if(constrain)return Cyc_Tcutil_unify(r,_tmp204);_tmp204=Cyc_Tcutil_compress(
_tmp204);if(r == _tmp204)return 1;{struct _tuple0 _tmpC98;struct _tuple0 _tmp20C=(
_tmpC98.f1=r,((_tmpC98.f2=_tmp204,_tmpC98)));void*_tmp20D;struct Cyc_Absyn_Tvar*
_tmp20E;void*_tmp20F;struct Cyc_Absyn_Tvar*_tmp210;_LL1D8: _tmp20D=_tmp20C.f1;if(
_tmp20D <= (void*)4)goto _LL1DA;if(*((int*)_tmp20D)!= 1)goto _LL1DA;_tmp20E=((
struct Cyc_Absyn_VarType_struct*)_tmp20D)->f1;_tmp20F=_tmp20C.f2;if(_tmp20F <= (
void*)4)goto _LL1DA;if(*((int*)_tmp20F)!= 1)goto _LL1DA;_tmp210=((struct Cyc_Absyn_VarType_struct*)
_tmp20F)->f1;_LL1D9: return Cyc_Absyn_tvar_cmp(_tmp20E,_tmp210)== 0;_LL1DA:;_LL1DB:
return 0;_LL1D7:;}_LL1CF: if(*((int*)_tmp203)!= 21)goto _LL1D1;_tmp205=((struct Cyc_Absyn_JoinEff_struct*)
_tmp203)->f1;_LL1D0: for(0;_tmp205 != 0;_tmp205=_tmp205->tl){if(Cyc_Tcutil_region_in_effect(
constrain,r,(void*)_tmp205->hd))return 1;}return 0;_LL1D1: if(*((int*)_tmp203)!= 22)
goto _LL1D3;_tmp206=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp203)->f1;_LL1D2: {
void*_tmp211=Cyc_Tcutil_rgns_of(_tmp206);void*_tmp212;_LL1DD: if(_tmp211 <= (void*)
4)goto _LL1DF;if(*((int*)_tmp211)!= 22)goto _LL1DF;_tmp212=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp211)->f1;_LL1DE: if(!constrain)return 0;{void*_tmp213=Cyc_Tcutil_compress(
_tmp212);struct Cyc_Core_Opt*_tmp214;struct Cyc_Core_Opt*_tmp215;struct Cyc_Core_Opt**
_tmp216;struct Cyc_Core_Opt*_tmp217;_LL1E2: if(_tmp213 <= (void*)4)goto _LL1E4;if(*((
int*)_tmp213)!= 0)goto _LL1E4;_tmp214=((struct Cyc_Absyn_Evar_struct*)_tmp213)->f1;
_tmp215=((struct Cyc_Absyn_Evar_struct*)_tmp213)->f2;_tmp216=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp213)->f2;_tmp217=((struct Cyc_Absyn_Evar_struct*)
_tmp213)->f4;_LL1E3: {void*_tmp218=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,
_tmp217);Cyc_Tcutil_occurs(_tmp218,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp217))->v,r);{struct Cyc_Absyn_JoinEff_struct
_tmpCA7;struct Cyc_Absyn_AccessEff_struct*_tmpCA6;struct Cyc_Absyn_AccessEff_struct
_tmpCA5;void*_tmpCA4[2];struct Cyc_Absyn_JoinEff_struct*_tmpCA3;void*_tmp219=Cyc_Tcutil_dummy_fntype((
void*)((_tmpCA3=_cycalloc(sizeof(*_tmpCA3)),((_tmpCA3[0]=((_tmpCA7.tag=21,((
_tmpCA7.f1=((_tmpCA4[1]=(void*)((_tmpCA6=_cycalloc(sizeof(*_tmpCA6)),((_tmpCA6[0]=((
_tmpCA5.tag=20,((_tmpCA5.f1=(void*)r,_tmpCA5)))),_tmpCA6)))),((_tmpCA4[0]=
_tmp218,Cyc_List_list(_tag_dynforward(_tmpCA4,sizeof(void*),2)))))),_tmpCA7)))),
_tmpCA3)))));{struct Cyc_Core_Opt*_tmpCA8;*_tmp216=((_tmpCA8=_cycalloc(sizeof(*
_tmpCA8)),((_tmpCA8->v=(void*)_tmp219,_tmpCA8))));}return 1;}}_LL1E4:;_LL1E5:
return 0;_LL1E1:;}_LL1DF:;_LL1E0: return Cyc_Tcutil_region_in_effect(constrain,r,
_tmp211);_LL1DC:;}_LL1D3: if(*((int*)_tmp203)!= 0)goto _LL1D5;_tmp207=((struct Cyc_Absyn_Evar_struct*)
_tmp203)->f1;_tmp208=((struct Cyc_Absyn_Evar_struct*)_tmp203)->f2;_tmp209=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp203)->f2;_tmp20A=((struct Cyc_Absyn_Evar_struct*)
_tmp203)->f4;_LL1D4: if(_tmp207 == 0  || (void*)_tmp207->v != (void*)6){const char*
_tmpCAB;void*_tmpCAA;(_tmpCAA=0,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmpCAB="effect evar has wrong kind",
_tag_dynforward(_tmpCAB,sizeof(char),_get_zero_arr_size_char(_tmpCAB,27)))),
_tag_dynforward(_tmpCAA,sizeof(void*),0)));}if(!constrain)return 0;{void*_tmp222=
Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp20A);Cyc_Tcutil_occurs(
_tmp222,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp20A))->v,r);{struct Cyc_Absyn_JoinEff_struct _tmpCC0;struct Cyc_List_List*
_tmpCBF;struct Cyc_Absyn_AccessEff_struct _tmpCBE;struct Cyc_Absyn_AccessEff_struct*
_tmpCBD;struct Cyc_List_List*_tmpCBC;struct Cyc_Absyn_JoinEff_struct*_tmpCBB;
struct Cyc_Absyn_JoinEff_struct*_tmp223=(_tmpCBB=_cycalloc(sizeof(*_tmpCBB)),((
_tmpCBB[0]=((_tmpCC0.tag=21,((_tmpCC0.f1=((_tmpCBC=_cycalloc(sizeof(*_tmpCBC)),((
_tmpCBC->hd=(void*)_tmp222,((_tmpCBC->tl=((_tmpCBF=_cycalloc(sizeof(*_tmpCBF)),((
_tmpCBF->hd=(void*)((void*)((_tmpCBD=_cycalloc(sizeof(*_tmpCBD)),((_tmpCBD[0]=((
_tmpCBE.tag=20,((_tmpCBE.f1=(void*)r,_tmpCBE)))),_tmpCBD))))),((_tmpCBF->tl=0,
_tmpCBF)))))),_tmpCBC)))))),_tmpCC0)))),_tmpCBB)));{struct Cyc_Core_Opt*_tmpCC1;*
_tmp209=((_tmpCC1=_cycalloc(sizeof(*_tmpCC1)),((_tmpCC1->v=(void*)((void*)
_tmp223),_tmpCC1))));}return 1;}}_LL1D5:;_LL1D6: return 0;_LL1CC:;}}static int Cyc_Tcutil_type_in_effect(
int may_constrain_evars,void*t,void*e);static int Cyc_Tcutil_type_in_effect(int
may_constrain_evars,void*t,void*e){t=Cyc_Tcutil_compress(t);{void*_tmp22B=Cyc_Tcutil_normalize_effect(
Cyc_Tcutil_compress(e));struct Cyc_List_List*_tmp22C;void*_tmp22D;struct Cyc_Core_Opt*
_tmp22E;struct Cyc_Core_Opt*_tmp22F;struct Cyc_Core_Opt**_tmp230;struct Cyc_Core_Opt*
_tmp231;_LL1E7: if(_tmp22B <= (void*)4)goto _LL1EF;if(*((int*)_tmp22B)!= 20)goto
_LL1E9;_LL1E8: return 0;_LL1E9: if(*((int*)_tmp22B)!= 21)goto _LL1EB;_tmp22C=((
struct Cyc_Absyn_JoinEff_struct*)_tmp22B)->f1;_LL1EA: for(0;_tmp22C != 0;_tmp22C=
_tmp22C->tl){if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp22C->hd))
return 1;}return 0;_LL1EB: if(*((int*)_tmp22B)!= 22)goto _LL1ED;_tmp22D=(void*)((
struct Cyc_Absyn_RgnsEff_struct*)_tmp22B)->f1;_LL1EC: _tmp22D=Cyc_Tcutil_compress(
_tmp22D);if(t == _tmp22D)return 1;if(may_constrain_evars)return Cyc_Tcutil_unify(t,
_tmp22D);{void*_tmp232=Cyc_Tcutil_rgns_of(t);void*_tmp233;_LL1F2: if(_tmp232 <= (
void*)4)goto _LL1F4;if(*((int*)_tmp232)!= 22)goto _LL1F4;_tmp233=(void*)((struct
Cyc_Absyn_RgnsEff_struct*)_tmp232)->f1;_LL1F3: {struct _tuple0 _tmpCC2;struct
_tuple0 _tmp235=(_tmpCC2.f1=Cyc_Tcutil_compress(_tmp233),((_tmpCC2.f2=_tmp22D,
_tmpCC2)));void*_tmp236;struct Cyc_Absyn_Tvar*_tmp237;void*_tmp238;struct Cyc_Absyn_Tvar*
_tmp239;_LL1F7: _tmp236=_tmp235.f1;if(_tmp236 <= (void*)4)goto _LL1F9;if(*((int*)
_tmp236)!= 1)goto _LL1F9;_tmp237=((struct Cyc_Absyn_VarType_struct*)_tmp236)->f1;
_tmp238=_tmp235.f2;if(_tmp238 <= (void*)4)goto _LL1F9;if(*((int*)_tmp238)!= 1)goto
_LL1F9;_tmp239=((struct Cyc_Absyn_VarType_struct*)_tmp238)->f1;_LL1F8: return Cyc_Tcutil_unify(
t,_tmp22D);_LL1F9:;_LL1FA: return _tmp233 == _tmp22D;_LL1F6:;}_LL1F4:;_LL1F5: return
Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp232);_LL1F1:;}_LL1ED: if(*((
int*)_tmp22B)!= 0)goto _LL1EF;_tmp22E=((struct Cyc_Absyn_Evar_struct*)_tmp22B)->f1;
_tmp22F=((struct Cyc_Absyn_Evar_struct*)_tmp22B)->f2;_tmp230=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp22B)->f2;_tmp231=((struct Cyc_Absyn_Evar_struct*)
_tmp22B)->f4;_LL1EE: if(_tmp22E == 0  || (void*)_tmp22E->v != (void*)6){const char*
_tmpCC5;void*_tmpCC4;(_tmpCC4=0,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmpCC5="effect evar has wrong kind",
_tag_dynforward(_tmpCC5,sizeof(char),_get_zero_arr_size_char(_tmpCC5,27)))),
_tag_dynforward(_tmpCC4,sizeof(void*),0)));}if(!may_constrain_evars)return 0;{
void*_tmp23C=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp231);Cyc_Tcutil_occurs(
_tmp23C,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp231))->v,t);{struct Cyc_Absyn_JoinEff_struct _tmpCDA;struct Cyc_List_List*
_tmpCD9;struct Cyc_Absyn_RgnsEff_struct _tmpCD8;struct Cyc_Absyn_RgnsEff_struct*
_tmpCD7;struct Cyc_List_List*_tmpCD6;struct Cyc_Absyn_JoinEff_struct*_tmpCD5;
struct Cyc_Absyn_JoinEff_struct*_tmp23D=(_tmpCD5=_cycalloc(sizeof(*_tmpCD5)),((
_tmpCD5[0]=((_tmpCDA.tag=21,((_tmpCDA.f1=((_tmpCD6=_cycalloc(sizeof(*_tmpCD6)),((
_tmpCD6->hd=(void*)_tmp23C,((_tmpCD6->tl=((_tmpCD9=_cycalloc(sizeof(*_tmpCD9)),((
_tmpCD9->hd=(void*)((void*)((_tmpCD7=_cycalloc(sizeof(*_tmpCD7)),((_tmpCD7[0]=((
_tmpCD8.tag=22,((_tmpCD8.f1=(void*)t,_tmpCD8)))),_tmpCD7))))),((_tmpCD9->tl=0,
_tmpCD9)))))),_tmpCD6)))))),_tmpCDA)))),_tmpCD5)));{struct Cyc_Core_Opt*_tmpCDB;*
_tmp230=((_tmpCDB=_cycalloc(sizeof(*_tmpCDB)),((_tmpCDB->v=(void*)((void*)
_tmp23D),_tmpCDB))));}return 1;}}_LL1EF:;_LL1F0: return 0;_LL1E6:;}}static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e);static int Cyc_Tcutil_variable_in_effect(
int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){e=Cyc_Tcutil_compress(e);{
void*_tmp245=e;struct Cyc_Absyn_Tvar*_tmp246;struct Cyc_List_List*_tmp247;void*
_tmp248;struct Cyc_Core_Opt*_tmp249;struct Cyc_Core_Opt*_tmp24A;struct Cyc_Core_Opt**
_tmp24B;struct Cyc_Core_Opt*_tmp24C;_LL1FC: if(_tmp245 <= (void*)4)goto _LL204;if(*((
int*)_tmp245)!= 1)goto _LL1FE;_tmp246=((struct Cyc_Absyn_VarType_struct*)_tmp245)->f1;
_LL1FD: return Cyc_Absyn_tvar_cmp(v,_tmp246)== 0;_LL1FE: if(*((int*)_tmp245)!= 21)
goto _LL200;_tmp247=((struct Cyc_Absyn_JoinEff_struct*)_tmp245)->f1;_LL1FF: for(0;
_tmp247 != 0;_tmp247=_tmp247->tl){if(Cyc_Tcutil_variable_in_effect(
may_constrain_evars,v,(void*)_tmp247->hd))return 1;}return 0;_LL200: if(*((int*)
_tmp245)!= 22)goto _LL202;_tmp248=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp245)->f1;_LL201: {void*_tmp24D=Cyc_Tcutil_rgns_of(_tmp248);void*_tmp24E;
_LL207: if(_tmp24D <= (void*)4)goto _LL209;if(*((int*)_tmp24D)!= 22)goto _LL209;
_tmp24E=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp24D)->f1;_LL208: if(!
may_constrain_evars)return 0;{void*_tmp24F=Cyc_Tcutil_compress(_tmp24E);struct Cyc_Core_Opt*
_tmp250;struct Cyc_Core_Opt*_tmp251;struct Cyc_Core_Opt**_tmp252;struct Cyc_Core_Opt*
_tmp253;_LL20C: if(_tmp24F <= (void*)4)goto _LL20E;if(*((int*)_tmp24F)!= 0)goto
_LL20E;_tmp250=((struct Cyc_Absyn_Evar_struct*)_tmp24F)->f1;_tmp251=((struct Cyc_Absyn_Evar_struct*)
_tmp24F)->f2;_tmp252=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp24F)->f2;_tmp253=((struct Cyc_Absyn_Evar_struct*)_tmp24F)->f4;_LL20D: {void*
_tmp254=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp253);if(!((
int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp253))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct
_tmpCEA;struct Cyc_Absyn_VarType_struct*_tmpCE9;struct Cyc_Absyn_VarType_struct
_tmpCE8;void*_tmpCE7[2];struct Cyc_Absyn_JoinEff_struct*_tmpCE6;void*_tmp255=Cyc_Tcutil_dummy_fntype((
void*)((_tmpCE6=_cycalloc(sizeof(*_tmpCE6)),((_tmpCE6[0]=((_tmpCEA.tag=21,((
_tmpCEA.f1=((_tmpCE7[1]=(void*)((_tmpCE9=_cycalloc(sizeof(*_tmpCE9)),((_tmpCE9[0]=((
_tmpCE8.tag=1,((_tmpCE8.f1=v,_tmpCE8)))),_tmpCE9)))),((_tmpCE7[0]=_tmp254,Cyc_List_list(
_tag_dynforward(_tmpCE7,sizeof(void*),2)))))),_tmpCEA)))),_tmpCE6)))));{struct
Cyc_Core_Opt*_tmpCEB;*_tmp252=((_tmpCEB=_cycalloc(sizeof(*_tmpCEB)),((_tmpCEB->v=(
void*)_tmp255,_tmpCEB))));}return 1;}}_LL20E:;_LL20F: return 0;_LL20B:;}_LL209:;
_LL20A: return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp24D);_LL206:;}
_LL202: if(*((int*)_tmp245)!= 0)goto _LL204;_tmp249=((struct Cyc_Absyn_Evar_struct*)
_tmp245)->f1;_tmp24A=((struct Cyc_Absyn_Evar_struct*)_tmp245)->f2;_tmp24B=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp245)->f2;_tmp24C=((struct Cyc_Absyn_Evar_struct*)
_tmp245)->f4;_LL203: if(_tmp249 == 0  || (void*)_tmp249->v != (void*)6){const char*
_tmpCEE;void*_tmpCED;(_tmpCED=0,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmpCEE="effect evar has wrong kind",
_tag_dynforward(_tmpCEE,sizeof(char),_get_zero_arr_size_char(_tmpCEE,27)))),
_tag_dynforward(_tmpCED,sizeof(void*),0)));}{void*_tmp25E=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ek,_tmp24C);if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(
Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp24C))->v,v))return 0;{struct Cyc_Absyn_JoinEff_struct _tmpD03;struct Cyc_List_List*
_tmpD02;struct Cyc_Absyn_VarType_struct _tmpD01;struct Cyc_Absyn_VarType_struct*
_tmpD00;struct Cyc_List_List*_tmpCFF;struct Cyc_Absyn_JoinEff_struct*_tmpCFE;
struct Cyc_Absyn_JoinEff_struct*_tmp25F=(_tmpCFE=_cycalloc(sizeof(*_tmpCFE)),((
_tmpCFE[0]=((_tmpD03.tag=21,((_tmpD03.f1=((_tmpCFF=_cycalloc(sizeof(*_tmpCFF)),((
_tmpCFF->hd=(void*)_tmp25E,((_tmpCFF->tl=((_tmpD02=_cycalloc(sizeof(*_tmpD02)),((
_tmpD02->hd=(void*)((void*)((_tmpD00=_cycalloc(sizeof(*_tmpD00)),((_tmpD00[0]=((
_tmpD01.tag=1,((_tmpD01.f1=v,_tmpD01)))),_tmpD00))))),((_tmpD02->tl=0,_tmpD02)))))),
_tmpCFF)))))),_tmpD03)))),_tmpCFE)));{struct Cyc_Core_Opt*_tmpD04;*_tmp24B=((
_tmpD04=_cycalloc(sizeof(*_tmpD04)),((_tmpD04->v=(void*)((void*)_tmp25F),_tmpD04))));}
return 1;}}_LL204:;_LL205: return 0;_LL1FB:;}}static int Cyc_Tcutil_evar_in_effect(
void*evar,void*e);static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){e=Cyc_Tcutil_compress(
e);{void*_tmp267=e;struct Cyc_List_List*_tmp268;void*_tmp269;_LL211: if(_tmp267 <= (
void*)4)goto _LL217;if(*((int*)_tmp267)!= 21)goto _LL213;_tmp268=((struct Cyc_Absyn_JoinEff_struct*)
_tmp267)->f1;_LL212: for(0;_tmp268 != 0;_tmp268=_tmp268->tl){if(Cyc_Tcutil_evar_in_effect(
evar,(void*)_tmp268->hd))return 1;}return 0;_LL213: if(*((int*)_tmp267)!= 22)goto
_LL215;_tmp269=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp267)->f1;_LL214: {
void*_tmp26A=Cyc_Tcutil_rgns_of(_tmp269);void*_tmp26B;_LL21A: if(_tmp26A <= (void*)
4)goto _LL21C;if(*((int*)_tmp26A)!= 22)goto _LL21C;_tmp26B=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp26A)->f1;_LL21B: return 0;_LL21C:;_LL21D: return Cyc_Tcutil_evar_in_effect(evar,
_tmp26A);_LL219:;}_LL215: if(*((int*)_tmp267)!= 0)goto _LL217;_LL216: return evar == 
e;_LL217:;_LL218: return 0;_LL210:;}}int Cyc_Tcutil_subset_effect(int
may_constrain_evars,void*e1,void*e2);int Cyc_Tcutil_subset_effect(int
may_constrain_evars,void*e1,void*e2){void*_tmp26C=Cyc_Tcutil_compress(e1);struct
Cyc_List_List*_tmp26D;void*_tmp26E;struct Cyc_Absyn_Tvar*_tmp26F;void*_tmp270;
struct Cyc_Core_Opt*_tmp271;struct Cyc_Core_Opt**_tmp272;struct Cyc_Core_Opt*
_tmp273;_LL21F: if(_tmp26C <= (void*)4)goto _LL229;if(*((int*)_tmp26C)!= 21)goto
_LL221;_tmp26D=((struct Cyc_Absyn_JoinEff_struct*)_tmp26C)->f1;_LL220: for(0;
_tmp26D != 0;_tmp26D=_tmp26D->tl){if(!Cyc_Tcutil_subset_effect(
may_constrain_evars,(void*)_tmp26D->hd,e2))return 0;}return 1;_LL221: if(*((int*)
_tmp26C)!= 20)goto _LL223;_tmp26E=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp26C)->f1;_LL222: return Cyc_Tcutil_region_in_effect(0,_tmp26E,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp26E,(void*)2);_LL223: if(*((int*)
_tmp26C)!= 1)goto _LL225;_tmp26F=((struct Cyc_Absyn_VarType_struct*)_tmp26C)->f1;
_LL224: return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp26F,e2);
_LL225: if(*((int*)_tmp26C)!= 22)goto _LL227;_tmp270=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp26C)->f1;_LL226: {void*_tmp274=Cyc_Tcutil_rgns_of(_tmp270);void*_tmp275;
_LL22C: if(_tmp274 <= (void*)4)goto _LL22E;if(*((int*)_tmp274)!= 22)goto _LL22E;
_tmp275=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp274)->f1;_LL22D: return Cyc_Tcutil_type_in_effect(
may_constrain_evars,_tmp275,e2) || may_constrain_evars  && Cyc_Tcutil_unify(
_tmp275,Cyc_Absyn_sint_typ);_LL22E:;_LL22F: return Cyc_Tcutil_subset_effect(
may_constrain_evars,_tmp274,e2);_LL22B:;}_LL227: if(*((int*)_tmp26C)!= 0)goto
_LL229;_tmp271=((struct Cyc_Absyn_Evar_struct*)_tmp26C)->f2;_tmp272=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp26C)->f2;_tmp273=((struct Cyc_Absyn_Evar_struct*)
_tmp26C)->f4;_LL228: if(!Cyc_Tcutil_evar_in_effect(e1,e2)){struct Cyc_Core_Opt*
_tmpD05;*_tmp272=((_tmpD05=_cycalloc(sizeof(*_tmpD05)),((_tmpD05->v=(void*)Cyc_Absyn_empty_effect,
_tmpD05))));}return 1;_LL229:;_LL22A: {const char*_tmpD09;void*_tmpD08[1];struct
Cyc_String_pa_struct _tmpD07;(_tmpD07.tag=0,((_tmpD07.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(e1)),((_tmpD08[0]=& _tmpD07,((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmpD09="subset_effect: bad effect: %s",
_tag_dynforward(_tmpD09,sizeof(char),_get_zero_arr_size_char(_tmpD09,30)))),
_tag_dynforward(_tmpD08,sizeof(void*),1)))))));}_LL21E:;}static int Cyc_Tcutil_unify_effect(
void*e1,void*e2);static int Cyc_Tcutil_unify_effect(void*e1,void*e2){e1=Cyc_Tcutil_normalize_effect(
e1);e2=Cyc_Tcutil_normalize_effect(e2);if(Cyc_Tcutil_subset_effect(0,e1,e2) && 
Cyc_Tcutil_subset_effect(0,e2,e1))return 1;if(Cyc_Tcutil_subset_effect(1,e1,e2)
 && Cyc_Tcutil_subset_effect(1,e2,e1))return 1;return 0;}static int Cyc_Tcutil_sub_rgnpo(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2);static int Cyc_Tcutil_sub_rgnpo(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){{struct Cyc_List_List*r1=rpo1;
for(0;r1 != 0;r1=r1->tl){struct _tuple0 _tmp27B;void*_tmp27C;void*_tmp27D;struct
_tuple0*_tmp27A=(struct _tuple0*)r1->hd;_tmp27B=*_tmp27A;_tmp27C=_tmp27B.f1;
_tmp27D=_tmp27B.f2;{int found=_tmp27C == (void*)2;{struct Cyc_List_List*r2=rpo2;
for(0;r2 != 0  && !found;r2=r2->tl){struct _tuple0 _tmp27F;void*_tmp280;void*_tmp281;
struct _tuple0*_tmp27E=(struct _tuple0*)r2->hd;_tmp27F=*_tmp27E;_tmp280=_tmp27F.f1;
_tmp281=_tmp27F.f2;if(Cyc_Tcutil_unify(_tmp27C,_tmp280) && Cyc_Tcutil_unify(
_tmp27D,_tmp281)){found=1;break;}}}if(!found)return 0;}}}return 1;}static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2);static int Cyc_Tcutil_same_rgn_po(
struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){return Cyc_Tcutil_sub_rgnpo(
rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple11{struct Cyc_Absyn_VarargInfo*
f1;struct Cyc_Absyn_VarargInfo*f2;};struct _tuple12{struct Cyc_Core_Opt*f1;struct
Cyc_Core_Opt*f2;};void Cyc_Tcutil_unify_it(void*t1,void*t2);void Cyc_Tcutil_unify_it(
void*t1,void*t2){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;Cyc_Tcutil_failure_reason=(
struct _dynforward_ptr)_tag_dynforward(0,0,0);t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(
t2);if(t1 == t2)return;{void*_tmp282=t1;struct Cyc_Core_Opt*_tmp283;struct Cyc_Core_Opt*
_tmp284;struct Cyc_Core_Opt**_tmp285;struct Cyc_Core_Opt*_tmp286;_LL231: if(_tmp282
<= (void*)4)goto _LL233;if(*((int*)_tmp282)!= 0)goto _LL233;_tmp283=((struct Cyc_Absyn_Evar_struct*)
_tmp282)->f1;_tmp284=((struct Cyc_Absyn_Evar_struct*)_tmp282)->f2;_tmp285=(struct
Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)_tmp282)->f2;_tmp286=((struct Cyc_Absyn_Evar_struct*)
_tmp282)->f4;_LL232: Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp286))->v,t2);{void*_tmp287=Cyc_Tcutil_typ_kind(
t2);if(Cyc_Tcutil_kind_leq(_tmp287,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp283))->v)){{struct Cyc_Core_Opt*_tmpD0A;*_tmp285=((_tmpD0A=_cycalloc(sizeof(*
_tmpD0A)),((_tmpD0A->v=(void*)t2,_tmpD0A))));}return;}else{{void*_tmp289=t2;
struct Cyc_Core_Opt*_tmp28A;struct Cyc_Core_Opt**_tmp28B;struct Cyc_Core_Opt*
_tmp28C;struct Cyc_Absyn_PtrInfo _tmp28D;_LL236: if(_tmp289 <= (void*)4)goto _LL23A;
if(*((int*)_tmp289)!= 0)goto _LL238;_tmp28A=((struct Cyc_Absyn_Evar_struct*)
_tmp289)->f2;_tmp28B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp289)->f2;_tmp28C=((struct Cyc_Absyn_Evar_struct*)_tmp289)->f4;_LL237: {struct
Cyc_List_List*_tmp28E=(struct Cyc_List_List*)_tmp286->v;{struct Cyc_List_List*s2=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp28C))->v;for(0;s2 != 0;
s2=s2->tl){if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmp28E,(struct Cyc_Absyn_Tvar*)s2->hd)){{const char*_tmpD0B;Cyc_Tcutil_failure_reason=((
_tmpD0B="(type variable would escape scope)",_tag_dynforward(_tmpD0B,sizeof(char),
_get_zero_arr_size_char(_tmpD0B,35))));}(int)_throw((void*)Cyc_Tcutil_Unify);}}}
if(Cyc_Tcutil_kind_leq((void*)_tmp283->v,_tmp287)){{struct Cyc_Core_Opt*_tmpD0C;*
_tmp28B=((_tmpD0C=_cycalloc(sizeof(*_tmpD0C)),((_tmpD0C->v=(void*)t1,_tmpD0C))));}
return;}{const char*_tmpD0D;Cyc_Tcutil_failure_reason=((_tmpD0D="(kinds are incompatible)",
_tag_dynforward(_tmpD0D,sizeof(char),_get_zero_arr_size_char(_tmpD0D,25))));}
goto _LL235;}_LL238: if(*((int*)_tmp289)!= 4)goto _LL23A;_tmp28D=((struct Cyc_Absyn_PointerType_struct*)
_tmp289)->f1;if(!((void*)_tmp283->v == (void*)2))goto _LL23A;_LL239: {struct Cyc_Absyn_Conref*
_tmp292=Cyc_Absyn_compress_conref((_tmp28D.ptr_atts).bounds);{union Cyc_Absyn_Constraint_union
_tmp293=_tmp292->v;_LL23D: if((_tmp293.No_constr).tag != 2)goto _LL23F;_LL23E:{
union Cyc_Absyn_Constraint_union _tmpD0E;_tmp292->v=(union Cyc_Absyn_Constraint_union)(((
_tmpD0E.Eq_constr).tag=0,(((_tmpD0E.Eq_constr).f1=(void*)Cyc_Absyn_bounds_one,
_tmpD0E))));}{struct Cyc_Core_Opt*_tmpD0F;*_tmp285=((_tmpD0F=_cycalloc(sizeof(*
_tmpD0F)),((_tmpD0F->v=(void*)t2,_tmpD0F))));}return;_LL23F:;_LL240: goto _LL23C;
_LL23C:;}goto _LL235;}_LL23A:;_LL23B: goto _LL235;_LL235:;}{const char*_tmpD10;Cyc_Tcutil_failure_reason=((
_tmpD10="(kinds are incompatible)",_tag_dynforward(_tmpD10,sizeof(char),
_get_zero_arr_size_char(_tmpD10,25))));}(int)_throw((void*)Cyc_Tcutil_Unify);}}
_LL233:;_LL234: goto _LL230;_LL230:;}{struct _tuple0 _tmpD11;struct _tuple0 _tmp298=(
_tmpD11.f1=t2,((_tmpD11.f2=t1,_tmpD11)));void*_tmp299;void*_tmp29A;void*_tmp29B;
void*_tmp29C;struct Cyc_Absyn_Tvar*_tmp29D;void*_tmp29E;struct Cyc_Absyn_Tvar*
_tmp29F;void*_tmp2A0;struct Cyc_Absyn_AggrInfo _tmp2A1;union Cyc_Absyn_AggrInfoU_union
_tmp2A2;struct Cyc_List_List*_tmp2A3;void*_tmp2A4;struct Cyc_Absyn_AggrInfo _tmp2A5;
union Cyc_Absyn_AggrInfoU_union _tmp2A6;struct Cyc_List_List*_tmp2A7;void*_tmp2A8;
struct _tuple2*_tmp2A9;void*_tmp2AA;struct _tuple2*_tmp2AB;void*_tmp2AC;struct Cyc_List_List*
_tmp2AD;void*_tmp2AE;struct Cyc_List_List*_tmp2AF;void*_tmp2B0;struct Cyc_Absyn_TunionInfo
_tmp2B1;union Cyc_Absyn_TunionInfoU_union _tmp2B2;struct Cyc_Absyn_Tuniondecl**
_tmp2B3;struct Cyc_Absyn_Tuniondecl*_tmp2B4;struct Cyc_List_List*_tmp2B5;struct Cyc_Core_Opt*
_tmp2B6;void*_tmp2B7;struct Cyc_Absyn_TunionInfo _tmp2B8;union Cyc_Absyn_TunionInfoU_union
_tmp2B9;struct Cyc_Absyn_Tuniondecl**_tmp2BA;struct Cyc_Absyn_Tuniondecl*_tmp2BB;
struct Cyc_List_List*_tmp2BC;struct Cyc_Core_Opt*_tmp2BD;void*_tmp2BE;struct Cyc_Absyn_TunionFieldInfo
_tmp2BF;union Cyc_Absyn_TunionFieldInfoU_union _tmp2C0;struct Cyc_Absyn_Tuniondecl*
_tmp2C1;struct Cyc_Absyn_Tunionfield*_tmp2C2;struct Cyc_List_List*_tmp2C3;void*
_tmp2C4;struct Cyc_Absyn_TunionFieldInfo _tmp2C5;union Cyc_Absyn_TunionFieldInfoU_union
_tmp2C6;struct Cyc_Absyn_Tuniondecl*_tmp2C7;struct Cyc_Absyn_Tunionfield*_tmp2C8;
struct Cyc_List_List*_tmp2C9;void*_tmp2CA;struct Cyc_Absyn_PtrInfo _tmp2CB;void*
_tmp2CC;struct Cyc_Absyn_Tqual _tmp2CD;struct Cyc_Absyn_PtrAtts _tmp2CE;void*_tmp2CF;
struct Cyc_Absyn_Conref*_tmp2D0;struct Cyc_Absyn_Conref*_tmp2D1;struct Cyc_Absyn_Conref*
_tmp2D2;void*_tmp2D3;struct Cyc_Absyn_PtrInfo _tmp2D4;void*_tmp2D5;struct Cyc_Absyn_Tqual
_tmp2D6;struct Cyc_Absyn_PtrAtts _tmp2D7;void*_tmp2D8;struct Cyc_Absyn_Conref*
_tmp2D9;struct Cyc_Absyn_Conref*_tmp2DA;struct Cyc_Absyn_Conref*_tmp2DB;void*
_tmp2DC;void*_tmp2DD;void*_tmp2DE;void*_tmp2DF;void*_tmp2E0;void*_tmp2E1;void*
_tmp2E2;void*_tmp2E3;void*_tmp2E4;int _tmp2E5;void*_tmp2E6;int _tmp2E7;void*
_tmp2E8;void*_tmp2E9;void*_tmp2EA;void*_tmp2EB;void*_tmp2EC;int _tmp2ED;void*
_tmp2EE;int _tmp2EF;void*_tmp2F0;void*_tmp2F1;void*_tmp2F2;void*_tmp2F3;void*
_tmp2F4;struct Cyc_Absyn_ArrayInfo _tmp2F5;void*_tmp2F6;struct Cyc_Absyn_Tqual
_tmp2F7;struct Cyc_Absyn_Exp*_tmp2F8;struct Cyc_Absyn_Conref*_tmp2F9;void*_tmp2FA;
struct Cyc_Absyn_ArrayInfo _tmp2FB;void*_tmp2FC;struct Cyc_Absyn_Tqual _tmp2FD;
struct Cyc_Absyn_Exp*_tmp2FE;struct Cyc_Absyn_Conref*_tmp2FF;void*_tmp300;struct
Cyc_Absyn_FnInfo _tmp301;struct Cyc_List_List*_tmp302;struct Cyc_Core_Opt*_tmp303;
void*_tmp304;struct Cyc_List_List*_tmp305;int _tmp306;struct Cyc_Absyn_VarargInfo*
_tmp307;struct Cyc_List_List*_tmp308;struct Cyc_List_List*_tmp309;void*_tmp30A;
struct Cyc_Absyn_FnInfo _tmp30B;struct Cyc_List_List*_tmp30C;struct Cyc_Core_Opt*
_tmp30D;void*_tmp30E;struct Cyc_List_List*_tmp30F;int _tmp310;struct Cyc_Absyn_VarargInfo*
_tmp311;struct Cyc_List_List*_tmp312;struct Cyc_List_List*_tmp313;void*_tmp314;
struct Cyc_List_List*_tmp315;void*_tmp316;struct Cyc_List_List*_tmp317;void*
_tmp318;void*_tmp319;struct Cyc_List_List*_tmp31A;void*_tmp31B;void*_tmp31C;
struct Cyc_List_List*_tmp31D;void*_tmp31E;void*_tmp31F;void*_tmp320;void*_tmp321;
void*_tmp322;void*_tmp323;void*_tmp324;void*_tmp325;void*_tmp326;void*_tmp327;
void*_tmp328;void*_tmp329;void*_tmp32A;void*_tmp32B;void*_tmp32C;void*_tmp32D;
void*_tmp32E;void*_tmp32F;void*_tmp330;void*_tmp331;_LL242: _tmp299=_tmp298.f1;
if(_tmp299 <= (void*)4)goto _LL244;if(*((int*)_tmp299)!= 0)goto _LL244;_LL243: Cyc_Tcutil_unify_it(
t2,t1);return;_LL244: _tmp29A=_tmp298.f1;if((int)_tmp29A != 0)goto _LL246;_tmp29B=
_tmp298.f2;if((int)_tmp29B != 0)goto _LL246;_LL245: return;_LL246: _tmp29C=_tmp298.f1;
if(_tmp29C <= (void*)4)goto _LL248;if(*((int*)_tmp29C)!= 1)goto _LL248;_tmp29D=((
struct Cyc_Absyn_VarType_struct*)_tmp29C)->f1;_tmp29E=_tmp298.f2;if(_tmp29E <= (
void*)4)goto _LL248;if(*((int*)_tmp29E)!= 1)goto _LL248;_tmp29F=((struct Cyc_Absyn_VarType_struct*)
_tmp29E)->f1;_LL247: {struct _dynforward_ptr*_tmp332=_tmp29D->name;struct
_dynforward_ptr*_tmp333=_tmp29F->name;int _tmp334=_tmp29D->identity;int _tmp335=
_tmp29F->identity;void*_tmp336=Cyc_Tcutil_tvar_kind(_tmp29D);void*_tmp337=Cyc_Tcutil_tvar_kind(
_tmp29F);if(_tmp335 == _tmp334  && Cyc_strptrcmp(_tmp332,_tmp333)== 0){if(_tmp336
!= _tmp337){const char*_tmpD17;void*_tmpD16[3];struct Cyc_String_pa_struct _tmpD15;
struct Cyc_String_pa_struct _tmpD14;struct Cyc_String_pa_struct _tmpD13;(_tmpD13.tag=
0,((_tmpD13.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(
_tmp337)),((_tmpD14.tag=0,((_tmpD14.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_kind2string(_tmp336)),((_tmpD15.tag=0,((_tmpD15.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp332),((_tmpD16[0]=& _tmpD15,((
_tmpD16[1]=& _tmpD14,((_tmpD16[2]=& _tmpD13,((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmpD17="same type variable %s has kinds %s and %s",
_tag_dynforward(_tmpD17,sizeof(char),_get_zero_arr_size_char(_tmpD17,42)))),
_tag_dynforward(_tmpD16,sizeof(void*),3)))))))))))))))))));}return;}{const char*
_tmpD18;Cyc_Tcutil_failure_reason=((_tmpD18="(variable types are not the same)",
_tag_dynforward(_tmpD18,sizeof(char),_get_zero_arr_size_char(_tmpD18,34))));}
goto _LL241;}_LL248: _tmp2A0=_tmp298.f1;if(_tmp2A0 <= (void*)4)goto _LL24A;if(*((int*)
_tmp2A0)!= 10)goto _LL24A;_tmp2A1=((struct Cyc_Absyn_AggrType_struct*)_tmp2A0)->f1;
_tmp2A2=_tmp2A1.aggr_info;_tmp2A3=_tmp2A1.targs;_tmp2A4=_tmp298.f2;if(_tmp2A4 <= (
void*)4)goto _LL24A;if(*((int*)_tmp2A4)!= 10)goto _LL24A;_tmp2A5=((struct Cyc_Absyn_AggrType_struct*)
_tmp2A4)->f1;_tmp2A6=_tmp2A5.aggr_info;_tmp2A7=_tmp2A5.targs;_LL249: {void*
_tmp33F;struct _tuple2*_tmp340;struct _tuple6 _tmp33E=Cyc_Absyn_aggr_kinded_name(
_tmp2A6);_tmp33F=_tmp33E.f1;_tmp340=_tmp33E.f2;{void*_tmp342;struct _tuple2*
_tmp343;struct _tuple6 _tmp341=Cyc_Absyn_aggr_kinded_name(_tmp2A2);_tmp342=_tmp341.f1;
_tmp343=_tmp341.f2;if(_tmp33F != _tmp342){{const char*_tmpD19;Cyc_Tcutil_failure_reason=((
_tmpD19="(struct and union type)",_tag_dynforward(_tmpD19,sizeof(char),
_get_zero_arr_size_char(_tmpD19,24))));}goto _LL241;}if(Cyc_Absyn_qvar_cmp(
_tmp340,_tmp343)!= 0){{const char*_tmpD1A;Cyc_Tcutil_failure_reason=((_tmpD1A="(different type name)",
_tag_dynforward(_tmpD1A,sizeof(char),_get_zero_arr_size_char(_tmpD1A,22))));}
goto _LL241;}Cyc_Tcutil_unify_list(_tmp2A7,_tmp2A3);return;}}_LL24A: _tmp2A8=
_tmp298.f1;if(_tmp2A8 <= (void*)4)goto _LL24C;if(*((int*)_tmp2A8)!= 12)goto _LL24C;
_tmp2A9=((struct Cyc_Absyn_EnumType_struct*)_tmp2A8)->f1;_tmp2AA=_tmp298.f2;if(
_tmp2AA <= (void*)4)goto _LL24C;if(*((int*)_tmp2AA)!= 12)goto _LL24C;_tmp2AB=((
struct Cyc_Absyn_EnumType_struct*)_tmp2AA)->f1;_LL24B: if(Cyc_Absyn_qvar_cmp(
_tmp2A9,_tmp2AB)== 0)return;{const char*_tmpD1B;Cyc_Tcutil_failure_reason=((
_tmpD1B="(different enum types)",_tag_dynforward(_tmpD1B,sizeof(char),
_get_zero_arr_size_char(_tmpD1B,23))));}goto _LL241;_LL24C: _tmp2AC=_tmp298.f1;if(
_tmp2AC <= (void*)4)goto _LL24E;if(*((int*)_tmp2AC)!= 13)goto _LL24E;_tmp2AD=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp2AC)->f1;_tmp2AE=_tmp298.f2;if(_tmp2AE
<= (void*)4)goto _LL24E;if(*((int*)_tmp2AE)!= 13)goto _LL24E;_tmp2AF=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp2AE)->f1;_LL24D: {int bad=0;for(0;_tmp2AD != 0  && _tmp2AF != 0;(_tmp2AD=_tmp2AD->tl,
_tmp2AF=_tmp2AF->tl)){struct Cyc_Absyn_Enumfield*_tmp347=(struct Cyc_Absyn_Enumfield*)
_tmp2AD->hd;struct Cyc_Absyn_Enumfield*_tmp348=(struct Cyc_Absyn_Enumfield*)
_tmp2AF->hd;if(Cyc_Absyn_qvar_cmp(_tmp347->name,_tmp348->name)!= 0){{const char*
_tmpD1C;Cyc_Tcutil_failure_reason=((_tmpD1C="(different names for enum fields)",
_tag_dynforward(_tmpD1C,sizeof(char),_get_zero_arr_size_char(_tmpD1C,34))));}bad=
1;break;}if(_tmp347->tag == _tmp348->tag)continue;if(_tmp347->tag == 0  || _tmp348->tag
== 0){{const char*_tmpD1D;Cyc_Tcutil_failure_reason=((_tmpD1D="(different tag values for enum fields)",
_tag_dynforward(_tmpD1D,sizeof(char),_get_zero_arr_size_char(_tmpD1D,39))));}bad=
1;break;}if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp347->tag),(
struct Cyc_Absyn_Exp*)_check_null(_tmp348->tag))){{const char*_tmpD1E;Cyc_Tcutil_failure_reason=((
_tmpD1E="(different tag values for enum fields)",_tag_dynforward(_tmpD1E,sizeof(
char),_get_zero_arr_size_char(_tmpD1E,39))));}bad=1;break;}}if(bad)goto _LL241;
if(_tmp2AD == 0  && _tmp2AF == 0)return;{const char*_tmpD1F;Cyc_Tcutil_failure_reason=((
_tmpD1F="(different number of fields for enums)",_tag_dynforward(_tmpD1F,sizeof(
char),_get_zero_arr_size_char(_tmpD1F,39))));}goto _LL241;}_LL24E: _tmp2B0=_tmp298.f1;
if(_tmp2B0 <= (void*)4)goto _LL250;if(*((int*)_tmp2B0)!= 2)goto _LL250;_tmp2B1=((
struct Cyc_Absyn_TunionType_struct*)_tmp2B0)->f1;_tmp2B2=_tmp2B1.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp2B0)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL250;_tmp2B3=(_tmp2B2.KnownTunion).f1;_tmp2B4=*_tmp2B3;_tmp2B5=_tmp2B1.targs;
_tmp2B6=_tmp2B1.rgn;_tmp2B7=_tmp298.f2;if(_tmp2B7 <= (void*)4)goto _LL250;if(*((
int*)_tmp2B7)!= 2)goto _LL250;_tmp2B8=((struct Cyc_Absyn_TunionType_struct*)
_tmp2B7)->f1;_tmp2B9=_tmp2B8.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp2B7)->f1).tunion_info).KnownTunion).tag != 1)goto _LL250;_tmp2BA=(_tmp2B9.KnownTunion).f1;
_tmp2BB=*_tmp2BA;_tmp2BC=_tmp2B8.targs;_tmp2BD=_tmp2B8.rgn;_LL24F: if(_tmp2B4 == 
_tmp2BB  || Cyc_Absyn_qvar_cmp(_tmp2B4->name,_tmp2BB->name)== 0){if(_tmp2BD != 0
 && _tmp2B6 != 0)Cyc_Tcutil_unify_it((void*)_tmp2BD->v,(void*)_tmp2B6->v);else{
if(_tmp2BD != 0  || _tmp2B6 != 0)goto Tunion_fail;}Cyc_Tcutil_unify_list(_tmp2BC,
_tmp2B5);return;}Tunion_fail: {const char*_tmpD20;Cyc_Tcutil_failure_reason=((
_tmpD20="(different tunion types)",_tag_dynforward(_tmpD20,sizeof(char),
_get_zero_arr_size_char(_tmpD20,25))));}goto _LL241;_LL250: _tmp2BE=_tmp298.f1;if(
_tmp2BE <= (void*)4)goto _LL252;if(*((int*)_tmp2BE)!= 3)goto _LL252;_tmp2BF=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp2BE)->f1;_tmp2C0=_tmp2BF.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp2BE)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL252;_tmp2C1=(_tmp2C0.KnownTunionfield).f1;_tmp2C2=(_tmp2C0.KnownTunionfield).f2;
_tmp2C3=_tmp2BF.targs;_tmp2C4=_tmp298.f2;if(_tmp2C4 <= (void*)4)goto _LL252;if(*((
int*)_tmp2C4)!= 3)goto _LL252;_tmp2C5=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp2C4)->f1;_tmp2C6=_tmp2C5.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp2C4)->f1).field_info).KnownTunionfield).tag != 1)goto _LL252;_tmp2C7=(_tmp2C6.KnownTunionfield).f1;
_tmp2C8=(_tmp2C6.KnownTunionfield).f2;_tmp2C9=_tmp2C5.targs;_LL251: if((_tmp2C1 == 
_tmp2C7  || Cyc_Absyn_qvar_cmp(_tmp2C1->name,_tmp2C7->name)== 0) && (_tmp2C2 == 
_tmp2C8  || Cyc_Absyn_qvar_cmp(_tmp2C2->name,_tmp2C8->name)== 0)){Cyc_Tcutil_unify_list(
_tmp2C9,_tmp2C3);return;}{const char*_tmpD21;Cyc_Tcutil_failure_reason=((_tmpD21="(different tunion field types)",
_tag_dynforward(_tmpD21,sizeof(char),_get_zero_arr_size_char(_tmpD21,31))));}
goto _LL241;_LL252: _tmp2CA=_tmp298.f1;if(_tmp2CA <= (void*)4)goto _LL254;if(*((int*)
_tmp2CA)!= 4)goto _LL254;_tmp2CB=((struct Cyc_Absyn_PointerType_struct*)_tmp2CA)->f1;
_tmp2CC=(void*)_tmp2CB.elt_typ;_tmp2CD=_tmp2CB.elt_tq;_tmp2CE=_tmp2CB.ptr_atts;
_tmp2CF=(void*)_tmp2CE.rgn;_tmp2D0=_tmp2CE.nullable;_tmp2D1=_tmp2CE.bounds;
_tmp2D2=_tmp2CE.zero_term;_tmp2D3=_tmp298.f2;if(_tmp2D3 <= (void*)4)goto _LL254;
if(*((int*)_tmp2D3)!= 4)goto _LL254;_tmp2D4=((struct Cyc_Absyn_PointerType_struct*)
_tmp2D3)->f1;_tmp2D5=(void*)_tmp2D4.elt_typ;_tmp2D6=_tmp2D4.elt_tq;_tmp2D7=
_tmp2D4.ptr_atts;_tmp2D8=(void*)_tmp2D7.rgn;_tmp2D9=_tmp2D7.nullable;_tmp2DA=
_tmp2D7.bounds;_tmp2DB=_tmp2D7.zero_term;_LL253: Cyc_Tcutil_unify_it(_tmp2D5,
_tmp2CC);Cyc_Tcutil_unify_it(_tmp2CF,_tmp2D8);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmpD22;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y,struct _dynforward_ptr reason))Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp,_tmp2DB,_tmp2D2,((_tmpD22="(not both zero terminated)",
_tag_dynforward(_tmpD22,sizeof(char),_get_zero_arr_size_char(_tmpD22,27)))));}
Cyc_Tcutil_unify_tqual(_tmp2D6,_tmp2D5,_tmp2CD,_tmp2CC);{const char*_tmpD23;Cyc_Tcutil_unify_it_conrefs(
Cyc_Tcutil_unify_it_bounds,_tmp2DA,_tmp2D1,((_tmpD23="(different pointer bounds)",
_tag_dynforward(_tmpD23,sizeof(char),_get_zero_arr_size_char(_tmpD23,27)))));}{
union Cyc_Absyn_Constraint_union _tmp351=(Cyc_Absyn_compress_conref(_tmp2DA))->v;
void*_tmp352;void*_tmp353;_LL27F: if((_tmp351.Eq_constr).tag != 0)goto _LL281;
_tmp352=(_tmp351.Eq_constr).f1;if((int)_tmp352 != 0)goto _LL281;_LL280: return;
_LL281: if((_tmp351.Eq_constr).tag != 0)goto _LL283;_tmp353=(_tmp351.Eq_constr).f1;
if((int)_tmp353 != 1)goto _LL283;_LL282: return;_LL283:;_LL284: goto _LL27E;_LL27E:;}{
const char*_tmpD24;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct
Cyc_Absyn_Conref*y,struct _dynforward_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,
_tmp2D9,_tmp2D0,((_tmpD24="(incompatible pointer types)",_tag_dynforward(_tmpD24,
sizeof(char),_get_zero_arr_size_char(_tmpD24,29)))));}return;_LL254: _tmp2DC=
_tmp298.f1;if(_tmp2DC <= (void*)4)goto _LL256;if(*((int*)_tmp2DC)!= 5)goto _LL256;
_tmp2DD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2DC)->f1;_tmp2DE=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp2DC)->f2;_tmp2DF=_tmp298.f2;if(_tmp2DF <= (
void*)4)goto _LL256;if(*((int*)_tmp2DF)!= 5)goto _LL256;_tmp2E0=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp2DF)->f1;_tmp2E1=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp2DF)->f2;
_LL255: if(_tmp2E0 == _tmp2DD  && ((_tmp2E1 == _tmp2DE  || _tmp2E1 == (void*)2  && 
_tmp2DE == (void*)3) || _tmp2E1 == (void*)3  && _tmp2DE == (void*)2))return;{const
char*_tmpD25;Cyc_Tcutil_failure_reason=((_tmpD25="(different integral types)",
_tag_dynforward(_tmpD25,sizeof(char),_get_zero_arr_size_char(_tmpD25,27))));}
goto _LL241;_LL256: _tmp2E2=_tmp298.f1;if((int)_tmp2E2 != 1)goto _LL258;_tmp2E3=
_tmp298.f2;if((int)_tmp2E3 != 1)goto _LL258;_LL257: return;_LL258: _tmp2E4=_tmp298.f1;
if(_tmp2E4 <= (void*)4)goto _LL25A;if(*((int*)_tmp2E4)!= 6)goto _LL25A;_tmp2E5=((
struct Cyc_Absyn_DoubleType_struct*)_tmp2E4)->f1;_tmp2E6=_tmp298.f2;if(_tmp2E6 <= (
void*)4)goto _LL25A;if(*((int*)_tmp2E6)!= 6)goto _LL25A;_tmp2E7=((struct Cyc_Absyn_DoubleType_struct*)
_tmp2E6)->f1;_LL259: if(_tmp2E5 == _tmp2E7)return;goto _LL241;_LL25A: _tmp2E8=
_tmp298.f1;if(_tmp2E8 <= (void*)4)goto _LL25C;if(*((int*)_tmp2E8)!= 14)goto _LL25C;
_tmp2E9=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp2E8)->f1;_tmp2EA=_tmp298.f2;
if(_tmp2EA <= (void*)4)goto _LL25C;if(*((int*)_tmp2EA)!= 14)goto _LL25C;_tmp2EB=(
void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp2EA)->f1;_LL25B: Cyc_Tcutil_unify_it(
_tmp2E9,_tmp2EB);return;_LL25C: _tmp2EC=_tmp298.f1;if(_tmp2EC <= (void*)4)goto
_LL25E;if(*((int*)_tmp2EC)!= 19)goto _LL25E;_tmp2ED=((struct Cyc_Absyn_TypeInt_struct*)
_tmp2EC)->f1;_tmp2EE=_tmp298.f2;if(_tmp2EE <= (void*)4)goto _LL25E;if(*((int*)
_tmp2EE)!= 19)goto _LL25E;_tmp2EF=((struct Cyc_Absyn_TypeInt_struct*)_tmp2EE)->f1;
_LL25D: if(_tmp2ED == _tmp2EF)return;{const char*_tmpD26;Cyc_Tcutil_failure_reason=((
_tmpD26="(different type integers)",_tag_dynforward(_tmpD26,sizeof(char),
_get_zero_arr_size_char(_tmpD26,26))));}goto _LL241;_LL25E: _tmp2F0=_tmp298.f1;if(
_tmp2F0 <= (void*)4)goto _LL260;if(*((int*)_tmp2F0)!= 18)goto _LL260;_tmp2F1=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp2F0)->f1;_tmp2F2=_tmp298.f2;if(_tmp2F2 <= (
void*)4)goto _LL260;if(*((int*)_tmp2F2)!= 18)goto _LL260;_tmp2F3=(void*)((struct
Cyc_Absyn_TagType_struct*)_tmp2F2)->f1;_LL25F: Cyc_Tcutil_unify_it(_tmp2F1,
_tmp2F3);return;_LL260: _tmp2F4=_tmp298.f1;if(_tmp2F4 <= (void*)4)goto _LL262;if(*((
int*)_tmp2F4)!= 7)goto _LL262;_tmp2F5=((struct Cyc_Absyn_ArrayType_struct*)_tmp2F4)->f1;
_tmp2F6=(void*)_tmp2F5.elt_type;_tmp2F7=_tmp2F5.tq;_tmp2F8=_tmp2F5.num_elts;
_tmp2F9=_tmp2F5.zero_term;_tmp2FA=_tmp298.f2;if(_tmp2FA <= (void*)4)goto _LL262;
if(*((int*)_tmp2FA)!= 7)goto _LL262;_tmp2FB=((struct Cyc_Absyn_ArrayType_struct*)
_tmp2FA)->f1;_tmp2FC=(void*)_tmp2FB.elt_type;_tmp2FD=_tmp2FB.tq;_tmp2FE=_tmp2FB.num_elts;
_tmp2FF=_tmp2FB.zero_term;_LL261: Cyc_Tcutil_unify_it(_tmp2FC,_tmp2F6);Cyc_Tcutil_unify_tqual(
_tmp2FD,_tmp2FC,_tmp2F7,_tmp2F6);Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmpD27;((void(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y,struct _dynforward_ptr reason))Cyc_Tcutil_unify_it_conrefs)(
Cyc_Core_intcmp,_tmp2F9,_tmp2FF,((_tmpD27="(not both zero terminated)",
_tag_dynforward(_tmpD27,sizeof(char),_get_zero_arr_size_char(_tmpD27,27)))));}
if(_tmp2F8 == _tmp2FE)return;if(_tmp2F8 == 0  || _tmp2FE == 0)goto _LL241;if(Cyc_Evexp_same_const_exp((
struct Cyc_Absyn_Exp*)_tmp2F8,(struct Cyc_Absyn_Exp*)_tmp2FE))return;{const char*
_tmpD28;Cyc_Tcutil_failure_reason=((_tmpD28="(different array sizes)",
_tag_dynforward(_tmpD28,sizeof(char),_get_zero_arr_size_char(_tmpD28,24))));}
goto _LL241;_LL262: _tmp300=_tmp298.f1;if(_tmp300 <= (void*)4)goto _LL264;if(*((int*)
_tmp300)!= 8)goto _LL264;_tmp301=((struct Cyc_Absyn_FnType_struct*)_tmp300)->f1;
_tmp302=_tmp301.tvars;_tmp303=_tmp301.effect;_tmp304=(void*)_tmp301.ret_typ;
_tmp305=_tmp301.args;_tmp306=_tmp301.c_varargs;_tmp307=_tmp301.cyc_varargs;
_tmp308=_tmp301.rgn_po;_tmp309=_tmp301.attributes;_tmp30A=_tmp298.f2;if(_tmp30A
<= (void*)4)goto _LL264;if(*((int*)_tmp30A)!= 8)goto _LL264;_tmp30B=((struct Cyc_Absyn_FnType_struct*)
_tmp30A)->f1;_tmp30C=_tmp30B.tvars;_tmp30D=_tmp30B.effect;_tmp30E=(void*)_tmp30B.ret_typ;
_tmp30F=_tmp30B.args;_tmp310=_tmp30B.c_varargs;_tmp311=_tmp30B.cyc_varargs;
_tmp312=_tmp30B.rgn_po;_tmp313=_tmp30B.attributes;_LL263: {int done=0;{struct
_RegionHandle _tmp359=_new_region("rgn");struct _RegionHandle*rgn=& _tmp359;
_push_region(rgn);{struct Cyc_List_List*inst=0;while(_tmp30C != 0){if(_tmp302 == 0){{
const char*_tmpD29;Cyc_Tcutil_failure_reason=((_tmpD29="(second function type has too few type variables)",
_tag_dynforward(_tmpD29,sizeof(char),_get_zero_arr_size_char(_tmpD29,50))));}(
int)_throw((void*)Cyc_Tcutil_Unify);}{void*_tmp35B=Cyc_Tcutil_tvar_kind((struct
Cyc_Absyn_Tvar*)_tmp30C->hd);void*_tmp35C=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
_tmp302->hd);if(_tmp35B != _tmp35C){{const char*_tmpD2F;void*_tmpD2E[3];struct Cyc_String_pa_struct
_tmpD2D;struct Cyc_String_pa_struct _tmpD2C;struct Cyc_String_pa_struct _tmpD2B;Cyc_Tcutil_failure_reason=(
struct _dynforward_ptr)((_tmpD2B.tag=0,((_tmpD2B.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_kind2string(_tmp35C)),((_tmpD2C.tag=0,((
_tmpD2C.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(
_tmp35B)),((_tmpD2D.tag=0,((_tmpD2D.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp30C->hd)),((
_tmpD2E[0]=& _tmpD2D,((_tmpD2E[1]=& _tmpD2C,((_tmpD2E[2]=& _tmpD2B,Cyc_aprintf(((
_tmpD2F="(type var %s has different kinds %s and %s)",_tag_dynforward(_tmpD2F,
sizeof(char),_get_zero_arr_size_char(_tmpD2F,44)))),_tag_dynforward(_tmpD2E,
sizeof(void*),3))))))))))))))))))));}(int)_throw((void*)Cyc_Tcutil_Unify);}{
struct _tuple8*_tmpD39;struct Cyc_Absyn_VarType_struct _tmpD38;struct Cyc_Absyn_VarType_struct*
_tmpD37;struct Cyc_List_List*_tmpD36;inst=((_tmpD36=_region_malloc(rgn,sizeof(*
_tmpD36)),((_tmpD36->hd=((_tmpD39=_region_malloc(rgn,sizeof(*_tmpD39)),((_tmpD39->f1=(
struct Cyc_Absyn_Tvar*)_tmp302->hd,((_tmpD39->f2=(void*)((_tmpD37=_cycalloc(
sizeof(*_tmpD37)),((_tmpD37[0]=((_tmpD38.tag=1,((_tmpD38.f1=(struct Cyc_Absyn_Tvar*)
_tmp30C->hd,_tmpD38)))),_tmpD37)))),_tmpD39)))))),((_tmpD36->tl=inst,_tmpD36))))));}
_tmp30C=_tmp30C->tl;_tmp302=_tmp302->tl;}}if(_tmp302 != 0){{const char*_tmpD3A;Cyc_Tcutil_failure_reason=((
_tmpD3A="(second function type has too many type variables)",_tag_dynforward(
_tmpD3A,sizeof(char),_get_zero_arr_size_char(_tmpD3A,51))));}_npop_handler(0);
goto _LL241;}if(inst != 0){{struct Cyc_Absyn_FnType_struct _tmpD46;struct Cyc_Absyn_FnInfo
_tmpD45;struct Cyc_Absyn_FnType_struct*_tmpD44;struct Cyc_Absyn_FnType_struct
_tmpD40;struct Cyc_Absyn_FnInfo _tmpD3F;struct Cyc_Absyn_FnType_struct*_tmpD3E;Cyc_Tcutil_unify_it((
void*)((_tmpD3E=_cycalloc(sizeof(*_tmpD3E)),((_tmpD3E[0]=((_tmpD40.tag=8,((
_tmpD40.f1=((_tmpD3F.tvars=0,((_tmpD3F.effect=_tmp30D,((_tmpD3F.ret_typ=(void*)
_tmp30E,((_tmpD3F.args=_tmp30F,((_tmpD3F.c_varargs=_tmp310,((_tmpD3F.cyc_varargs=
_tmp311,((_tmpD3F.rgn_po=_tmp312,((_tmpD3F.attributes=_tmp313,_tmpD3F)))))))))))))))),
_tmpD40)))),_tmpD3E)))),Cyc_Tcutil_rsubstitute(rgn,inst,(void*)((_tmpD44=
_cycalloc(sizeof(*_tmpD44)),((_tmpD44[0]=((_tmpD46.tag=8,((_tmpD46.f1=((_tmpD45.tvars=
0,((_tmpD45.effect=_tmp303,((_tmpD45.ret_typ=(void*)_tmp304,((_tmpD45.args=
_tmp305,((_tmpD45.c_varargs=_tmp306,((_tmpD45.cyc_varargs=_tmp307,((_tmpD45.rgn_po=
_tmp308,((_tmpD45.attributes=_tmp309,_tmpD45)))))))))))))))),_tmpD46)))),_tmpD44))))));}
done=1;}};_pop_region(rgn);}if(done)return;Cyc_Tcutil_unify_it(_tmp30E,_tmp304);
for(0;_tmp30F != 0  && _tmp305 != 0;(_tmp30F=_tmp30F->tl,_tmp305=_tmp305->tl)){
struct Cyc_Absyn_Tqual _tmp36E;void*_tmp36F;struct _tuple3 _tmp36D=*((struct _tuple3*)
_tmp30F->hd);_tmp36E=_tmp36D.f2;_tmp36F=_tmp36D.f3;{struct Cyc_Absyn_Tqual _tmp371;
void*_tmp372;struct _tuple3 _tmp370=*((struct _tuple3*)_tmp305->hd);_tmp371=_tmp370.f2;
_tmp372=_tmp370.f3;Cyc_Tcutil_unify_it(_tmp36F,_tmp372);Cyc_Tcutil_unify_tqual(
_tmp36E,_tmp36F,_tmp371,_tmp372);}}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;if(_tmp30F != 0  || _tmp305 != 0){{const char*_tmpD47;Cyc_Tcutil_failure_reason=((
_tmpD47="(function types have different number of arguments)",_tag_dynforward(
_tmpD47,sizeof(char),_get_zero_arr_size_char(_tmpD47,52))));}goto _LL241;}if(
_tmp310 != _tmp306){{const char*_tmpD48;Cyc_Tcutil_failure_reason=((_tmpD48="(only one function type takes C varargs)",
_tag_dynforward(_tmpD48,sizeof(char),_get_zero_arr_size_char(_tmpD48,41))));}
goto _LL241;}{int bad_cyc_vararg=0;{struct _tuple11 _tmpD49;struct _tuple11 _tmp376=(
_tmpD49.f1=_tmp311,((_tmpD49.f2=_tmp307,_tmpD49)));struct Cyc_Absyn_VarargInfo*
_tmp377;struct Cyc_Absyn_VarargInfo*_tmp378;struct Cyc_Absyn_VarargInfo*_tmp379;
struct Cyc_Absyn_VarargInfo*_tmp37A;struct Cyc_Absyn_VarargInfo*_tmp37B;struct Cyc_Absyn_VarargInfo
_tmp37C;struct Cyc_Core_Opt*_tmp37D;struct Cyc_Absyn_Tqual _tmp37E;void*_tmp37F;int
_tmp380;struct Cyc_Absyn_VarargInfo*_tmp381;struct Cyc_Absyn_VarargInfo _tmp382;
struct Cyc_Core_Opt*_tmp383;struct Cyc_Absyn_Tqual _tmp384;void*_tmp385;int _tmp386;
_LL286: _tmp377=_tmp376.f1;if(_tmp377 != 0)goto _LL288;_tmp378=_tmp376.f2;if(
_tmp378 != 0)goto _LL288;_LL287: goto _LL285;_LL288: _tmp379=_tmp376.f1;if(_tmp379 != 
0)goto _LL28A;_LL289: goto _LL28B;_LL28A: _tmp37A=_tmp376.f2;if(_tmp37A != 0)goto
_LL28C;_LL28B: bad_cyc_vararg=1;{const char*_tmpD4A;Cyc_Tcutil_failure_reason=((
_tmpD4A="(only one function type takes varargs)",_tag_dynforward(_tmpD4A,sizeof(
char),_get_zero_arr_size_char(_tmpD4A,39))));}goto _LL285;_LL28C: _tmp37B=_tmp376.f1;
if(_tmp37B == 0)goto _LL285;_tmp37C=*_tmp37B;_tmp37D=_tmp37C.name;_tmp37E=_tmp37C.tq;
_tmp37F=(void*)_tmp37C.type;_tmp380=_tmp37C.inject;_tmp381=_tmp376.f2;if(_tmp381
== 0)goto _LL285;_tmp382=*_tmp381;_tmp383=_tmp382.name;_tmp384=_tmp382.tq;_tmp385=(
void*)_tmp382.type;_tmp386=_tmp382.inject;_LL28D: Cyc_Tcutil_unify_it(_tmp37F,
_tmp385);Cyc_Tcutil_unify_tqual(_tmp37E,_tmp37F,_tmp384,_tmp385);if(_tmp380 != 
_tmp386){bad_cyc_vararg=1;{const char*_tmpD4B;Cyc_Tcutil_failure_reason=((_tmpD4B="(only one function type injects varargs)",
_tag_dynforward(_tmpD4B,sizeof(char),_get_zero_arr_size_char(_tmpD4B,41))));}}
goto _LL285;_LL285:;}if(bad_cyc_vararg)goto _LL241;{int bad_effect=0;{struct
_tuple12 _tmpD4C;struct _tuple12 _tmp38A=(_tmpD4C.f1=_tmp30D,((_tmpD4C.f2=_tmp303,
_tmpD4C)));struct Cyc_Core_Opt*_tmp38B;struct Cyc_Core_Opt*_tmp38C;struct Cyc_Core_Opt*
_tmp38D;struct Cyc_Core_Opt*_tmp38E;_LL28F: _tmp38B=_tmp38A.f1;if(_tmp38B != 0)goto
_LL291;_tmp38C=_tmp38A.f2;if(_tmp38C != 0)goto _LL291;_LL290: goto _LL28E;_LL291:
_tmp38D=_tmp38A.f1;if(_tmp38D != 0)goto _LL293;_LL292: goto _LL294;_LL293: _tmp38E=
_tmp38A.f2;if(_tmp38E != 0)goto _LL295;_LL294: bad_effect=1;goto _LL28E;_LL295:;
_LL296: bad_effect=!Cyc_Tcutil_unify_effect((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp30D))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp303))->v);
goto _LL28E;_LL28E:;}Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;if(
bad_effect){{const char*_tmpD4D;Cyc_Tcutil_failure_reason=((_tmpD4D="(function type effects do not unify)",
_tag_dynforward(_tmpD4D,sizeof(char),_get_zero_arr_size_char(_tmpD4D,37))));}
goto _LL241;}if(!Cyc_Tcutil_same_atts(_tmp309,_tmp313)){{const char*_tmpD4E;Cyc_Tcutil_failure_reason=((
_tmpD4E="(function types have different attributes)",_tag_dynforward(_tmpD4E,
sizeof(char),_get_zero_arr_size_char(_tmpD4E,43))));}goto _LL241;}if(!Cyc_Tcutil_same_rgn_po(
_tmp308,_tmp312)){{const char*_tmpD4F;Cyc_Tcutil_failure_reason=((_tmpD4F="(function types have different region lifetime orderings)",
_tag_dynforward(_tmpD4F,sizeof(char),_get_zero_arr_size_char(_tmpD4F,58))));}
goto _LL241;}return;}}}_LL264: _tmp314=_tmp298.f1;if(_tmp314 <= (void*)4)goto _LL266;
if(*((int*)_tmp314)!= 9)goto _LL266;_tmp315=((struct Cyc_Absyn_TupleType_struct*)
_tmp314)->f1;_tmp316=_tmp298.f2;if(_tmp316 <= (void*)4)goto _LL266;if(*((int*)
_tmp316)!= 9)goto _LL266;_tmp317=((struct Cyc_Absyn_TupleType_struct*)_tmp316)->f1;
_LL265: for(0;_tmp317 != 0  && _tmp315 != 0;(_tmp317=_tmp317->tl,_tmp315=_tmp315->tl)){
struct Cyc_Absyn_Tqual _tmp393;void*_tmp394;struct _tuple5 _tmp392=*((struct _tuple5*)
_tmp317->hd);_tmp393=_tmp392.f1;_tmp394=_tmp392.f2;{struct Cyc_Absyn_Tqual _tmp396;
void*_tmp397;struct _tuple5 _tmp395=*((struct _tuple5*)_tmp315->hd);_tmp396=_tmp395.f1;
_tmp397=_tmp395.f2;Cyc_Tcutil_unify_it(_tmp394,_tmp397);Cyc_Tcutil_unify_tqual(
_tmp393,_tmp394,_tmp396,_tmp397);}}if(_tmp317 == 0  && _tmp315 == 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{const char*_tmpD50;Cyc_Tcutil_failure_reason=((
_tmpD50="(tuple types have different numbers of components)",_tag_dynforward(
_tmpD50,sizeof(char),_get_zero_arr_size_char(_tmpD50,51))));}goto _LL241;_LL266:
_tmp318=_tmp298.f1;if(_tmp318 <= (void*)4)goto _LL268;if(*((int*)_tmp318)!= 11)
goto _LL268;_tmp319=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp318)->f1;
_tmp31A=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp318)->f2;_tmp31B=_tmp298.f2;
if(_tmp31B <= (void*)4)goto _LL268;if(*((int*)_tmp31B)!= 11)goto _LL268;_tmp31C=(
void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp31B)->f1;_tmp31D=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp31B)->f2;_LL267: if(_tmp31C != _tmp319){{const char*_tmpD51;Cyc_Tcutil_failure_reason=((
_tmpD51="(struct and union type)",_tag_dynforward(_tmpD51,sizeof(char),
_get_zero_arr_size_char(_tmpD51,24))));}goto _LL241;}for(0;_tmp31D != 0  && _tmp31A
!= 0;(_tmp31D=_tmp31D->tl,_tmp31A=_tmp31A->tl)){struct Cyc_Absyn_Aggrfield*
_tmp39A=(struct Cyc_Absyn_Aggrfield*)_tmp31D->hd;struct Cyc_Absyn_Aggrfield*
_tmp39B=(struct Cyc_Absyn_Aggrfield*)_tmp31A->hd;if(Cyc_strptrcmp(_tmp39A->name,
_tmp39B->name)!= 0){{const char*_tmpD52;Cyc_Tcutil_failure_reason=((_tmpD52="(different member names)",
_tag_dynforward(_tmpD52,sizeof(char),_get_zero_arr_size_char(_tmpD52,25))));}(
int)_throw((void*)Cyc_Tcutil_Unify);}Cyc_Tcutil_unify_it((void*)_tmp39A->type,(
void*)_tmp39B->type);Cyc_Tcutil_unify_tqual(_tmp39A->tq,(void*)_tmp39A->type,
_tmp39B->tq,(void*)_tmp39B->type);if(!Cyc_Tcutil_same_atts(_tmp39A->attributes,
_tmp39B->attributes)){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;{const
char*_tmpD53;Cyc_Tcutil_failure_reason=((_tmpD53="(different attributes on member)",
_tag_dynforward(_tmpD53,sizeof(char),_get_zero_arr_size_char(_tmpD53,33))));}(
int)_throw((void*)Cyc_Tcutil_Unify);}if((_tmp39A->width != 0  && _tmp39B->width == 
0  || _tmp39B->width != 0  && _tmp39A->width == 0) || (_tmp39A->width != 0  && _tmp39B->width
!= 0) && !Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp39A->width),(
struct Cyc_Absyn_Exp*)_check_null(_tmp39B->width))){Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=
t2;{const char*_tmpD54;Cyc_Tcutil_failure_reason=((_tmpD54="(different bitfield widths on member)",
_tag_dynforward(_tmpD54,sizeof(char),_get_zero_arr_size_char(_tmpD54,38))));}(
int)_throw((void*)Cyc_Tcutil_Unify);}}if(_tmp31D == 0  && _tmp31A == 0)return;Cyc_Tcutil_t1_failure=
t1;Cyc_Tcutil_t2_failure=t2;{const char*_tmpD55;Cyc_Tcutil_failure_reason=((
_tmpD55="(different number of members)",_tag_dynforward(_tmpD55,sizeof(char),
_get_zero_arr_size_char(_tmpD55,30))));}goto _LL241;_LL268: _tmp31E=_tmp298.f1;if((
int)_tmp31E != 2)goto _LL26A;_tmp31F=_tmp298.f2;if((int)_tmp31F != 2)goto _LL26A;
_LL269: return;_LL26A: _tmp320=_tmp298.f1;if((int)_tmp320 != 3)goto _LL26C;_tmp321=
_tmp298.f2;if((int)_tmp321 != 3)goto _LL26C;_LL26B: return;_LL26C: _tmp322=_tmp298.f1;
if(_tmp322 <= (void*)4)goto _LL26E;if(*((int*)_tmp322)!= 15)goto _LL26E;_tmp323=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp322)->f1;_tmp324=_tmp298.f2;
if(_tmp324 <= (void*)4)goto _LL26E;if(*((int*)_tmp324)!= 15)goto _LL26E;_tmp325=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp324)->f1;_LL26D: Cyc_Tcutil_unify_it(
_tmp323,_tmp325);return;_LL26E: _tmp326=_tmp298.f1;if(_tmp326 <= (void*)4)goto
_LL270;if(*((int*)_tmp326)!= 16)goto _LL270;_tmp327=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp326)->f1;_tmp328=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp326)->f2;
_tmp329=_tmp298.f2;if(_tmp329 <= (void*)4)goto _LL270;if(*((int*)_tmp329)!= 16)
goto _LL270;_tmp32A=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp329)->f1;
_tmp32B=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp329)->f2;_LL26F: Cyc_Tcutil_unify_it(
_tmp327,_tmp32A);Cyc_Tcutil_unify_it(_tmp328,_tmp32B);return;_LL270: _tmp32C=
_tmp298.f1;if(_tmp32C <= (void*)4)goto _LL272;if(*((int*)_tmp32C)!= 21)goto _LL272;
_LL271: goto _LL273;_LL272: _tmp32D=_tmp298.f2;if(_tmp32D <= (void*)4)goto _LL274;if(*((
int*)_tmp32D)!= 21)goto _LL274;_LL273: goto _LL275;_LL274: _tmp32E=_tmp298.f1;if(
_tmp32E <= (void*)4)goto _LL276;if(*((int*)_tmp32E)!= 20)goto _LL276;_LL275: goto
_LL277;_LL276: _tmp32F=_tmp298.f1;if(_tmp32F <= (void*)4)goto _LL278;if(*((int*)
_tmp32F)!= 22)goto _LL278;_LL277: goto _LL279;_LL278: _tmp330=_tmp298.f2;if(_tmp330
<= (void*)4)goto _LL27A;if(*((int*)_tmp330)!= 22)goto _LL27A;_LL279: goto _LL27B;
_LL27A: _tmp331=_tmp298.f2;if(_tmp331 <= (void*)4)goto _LL27C;if(*((int*)_tmp331)!= 
20)goto _LL27C;_LL27B: if(Cyc_Tcutil_unify_effect(t1,t2))return;{const char*_tmpD56;
Cyc_Tcutil_failure_reason=((_tmpD56="(effects don't unify)",_tag_dynforward(
_tmpD56,sizeof(char),_get_zero_arr_size_char(_tmpD56,22))));}goto _LL241;_LL27C:;
_LL27D: goto _LL241;_LL241:;}(int)_throw((void*)Cyc_Tcutil_Unify);}int Cyc_Tcutil_star_cmp(
int(*cmp)(void*,void*),void*a1,void*a2);int Cyc_Tcutil_star_cmp(int(*cmp)(void*,
void*),void*a1,void*a2){if(a1 == a2)return 0;if(a1 == 0  && a2 != 0)return - 1;if(a1 != 
0  && a2 == 0)return 1;return cmp((void*)_check_null(a1),(void*)_check_null(a2));}
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp3A1=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int
_tmp3A2=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);return Cyc_Core_intcmp(
_tmp3A1,_tmp3A2);}static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y);static int Cyc_Tcutil_conrefs_cmp(int(*
cmp)(void*,void*),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y){x=Cyc_Absyn_compress_conref(
x);y=Cyc_Absyn_compress_conref(y);if(x == y)return 0;{union Cyc_Absyn_Constraint_union
_tmp3A3=x->v;void*_tmp3A4;_LL298: if((_tmp3A3.No_constr).tag != 2)goto _LL29A;
_LL299: return - 1;_LL29A: if((_tmp3A3.Eq_constr).tag != 0)goto _LL29C;_tmp3A4=(
_tmp3A3.Eq_constr).f1;_LL29B: {union Cyc_Absyn_Constraint_union _tmp3A5=y->v;void*
_tmp3A6;_LL29F: if((_tmp3A5.No_constr).tag != 2)goto _LL2A1;_LL2A0: return 1;_LL2A1:
if((_tmp3A5.Eq_constr).tag != 0)goto _LL2A3;_tmp3A6=(_tmp3A5.Eq_constr).f1;_LL2A2:
return cmp(_tmp3A4,_tmp3A6);_LL2A3: if((_tmp3A5.Forward_constr).tag != 1)goto _LL29E;
_LL2A4: {const char*_tmpD59;void*_tmpD58;(_tmpD58=0,Cyc_Tcutil_impos(((_tmpD59="unify_conref: forward after compress(2)",
_tag_dynforward(_tmpD59,sizeof(char),_get_zero_arr_size_char(_tmpD59,40)))),
_tag_dynforward(_tmpD58,sizeof(void*),0)));}_LL29E:;}_LL29C: if((_tmp3A3.Forward_constr).tag
!= 1)goto _LL297;_LL29D: {const char*_tmpD5C;void*_tmpD5B;(_tmpD5B=0,Cyc_Tcutil_impos(((
_tmpD5C="unify_conref: forward after compress",_tag_dynforward(_tmpD5C,sizeof(
char),_get_zero_arr_size_char(_tmpD5C,37)))),_tag_dynforward(_tmpD5B,sizeof(void*),
0)));}_LL297:;}}static int Cyc_Tcutil_tqual_type_cmp(struct _tuple5*tqt1,struct
_tuple5*tqt2);static int Cyc_Tcutil_tqual_type_cmp(struct _tuple5*tqt1,struct
_tuple5*tqt2){struct _tuple5 _tmp3AC;struct Cyc_Absyn_Tqual _tmp3AD;void*_tmp3AE;
struct _tuple5*_tmp3AB=tqt1;_tmp3AC=*_tmp3AB;_tmp3AD=_tmp3AC.f1;_tmp3AE=_tmp3AC.f2;{
struct _tuple5 _tmp3B0;struct Cyc_Absyn_Tqual _tmp3B1;void*_tmp3B2;struct _tuple5*
_tmp3AF=tqt2;_tmp3B0=*_tmp3AF;_tmp3B1=_tmp3B0.f1;_tmp3B2=_tmp3B0.f2;{int _tmp3B3=
Cyc_Tcutil_tqual_cmp(_tmp3AD,_tmp3B1);if(_tmp3B3 != 0)return _tmp3B3;return Cyc_Tcutil_typecmp(
_tmp3AE,_tmp3B2);}}}static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*
f1,struct Cyc_Absyn_Aggrfield*f2);static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*
f1,struct Cyc_Absyn_Aggrfield*f2){int _tmp3B4=Cyc_strptrcmp(f1->name,f2->name);if(
_tmp3B4 != 0)return _tmp3B4;{int _tmp3B5=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);if(
_tmp3B5 != 0)return _tmp3B5;{int _tmp3B6=Cyc_Tcutil_typecmp((void*)f1->type,(void*)
f2->type);if(_tmp3B6 != 0)return _tmp3B6;{int _tmp3B7=Cyc_List_list_cmp(Cyc_Tcutil_attribute_cmp,
f1->attributes,f2->attributes);if(_tmp3B7 != 0)return _tmp3B7;return((int(*)(int(*
cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*
a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);}}}}static
int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*
e2);static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*
e2){int _tmp3B8=Cyc_Absyn_qvar_cmp(e1->name,e2->name);if(_tmp3B8 != 0)return
_tmp3B8;return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),
struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,
e1->tag,e2->tag);}static int Cyc_Tcutil_type_case_number(void*t);static int Cyc_Tcutil_type_case_number(
void*t){void*_tmp3B9=t;_LL2A6: if((int)_tmp3B9 != 0)goto _LL2A8;_LL2A7: return 0;
_LL2A8: if(_tmp3B9 <= (void*)4)goto _LL2B4;if(*((int*)_tmp3B9)!= 0)goto _LL2AA;
_LL2A9: return 1;_LL2AA: if(*((int*)_tmp3B9)!= 1)goto _LL2AC;_LL2AB: return 2;_LL2AC:
if(*((int*)_tmp3B9)!= 2)goto _LL2AE;_LL2AD: return 3;_LL2AE: if(*((int*)_tmp3B9)!= 3)
goto _LL2B0;_LL2AF: return 4;_LL2B0: if(*((int*)_tmp3B9)!= 4)goto _LL2B2;_LL2B1:
return 5;_LL2B2: if(*((int*)_tmp3B9)!= 5)goto _LL2B4;_LL2B3: return 6;_LL2B4: if((int)
_tmp3B9 != 1)goto _LL2B6;_LL2B5: return 7;_LL2B6: if(_tmp3B9 <= (void*)4)goto _LL2CA;
if(*((int*)_tmp3B9)!= 6)goto _LL2B8;_LL2B7: return 8;_LL2B8: if(*((int*)_tmp3B9)!= 7)
goto _LL2BA;_LL2B9: return 9;_LL2BA: if(*((int*)_tmp3B9)!= 8)goto _LL2BC;_LL2BB:
return 10;_LL2BC: if(*((int*)_tmp3B9)!= 9)goto _LL2BE;_LL2BD: return 11;_LL2BE: if(*((
int*)_tmp3B9)!= 10)goto _LL2C0;_LL2BF: return 12;_LL2C0: if(*((int*)_tmp3B9)!= 11)
goto _LL2C2;_LL2C1: return 14;_LL2C2: if(*((int*)_tmp3B9)!= 12)goto _LL2C4;_LL2C3:
return 16;_LL2C4: if(*((int*)_tmp3B9)!= 13)goto _LL2C6;_LL2C5: return 17;_LL2C6: if(*((
int*)_tmp3B9)!= 15)goto _LL2C8;_LL2C7: return 18;_LL2C8: if(*((int*)_tmp3B9)!= 17)
goto _LL2CA;_LL2C9: return 19;_LL2CA: if((int)_tmp3B9 != 3)goto _LL2CC;_LL2CB: return 20;
_LL2CC: if((int)_tmp3B9 != 2)goto _LL2CE;_LL2CD: return 21;_LL2CE: if(_tmp3B9 <= (void*)
4)goto _LL2D0;if(*((int*)_tmp3B9)!= 20)goto _LL2D0;_LL2CF: return 22;_LL2D0: if(
_tmp3B9 <= (void*)4)goto _LL2D2;if(*((int*)_tmp3B9)!= 21)goto _LL2D2;_LL2D1: return
23;_LL2D2: if(_tmp3B9 <= (void*)4)goto _LL2D4;if(*((int*)_tmp3B9)!= 22)goto _LL2D4;
_LL2D3: return 24;_LL2D4: if(_tmp3B9 <= (void*)4)goto _LL2D6;if(*((int*)_tmp3B9)!= 14)
goto _LL2D6;_LL2D5: return 25;_LL2D6: if(_tmp3B9 <= (void*)4)goto _LL2D8;if(*((int*)
_tmp3B9)!= 19)goto _LL2D8;_LL2D7: return 26;_LL2D8: if(_tmp3B9 <= (void*)4)goto _LL2DA;
if(*((int*)_tmp3B9)!= 18)goto _LL2DA;_LL2D9: return 27;_LL2DA: if(_tmp3B9 <= (void*)4)
goto _LL2A5;if(*((int*)_tmp3B9)!= 16)goto _LL2A5;_LL2DB: return 28;_LL2A5:;}int Cyc_Tcutil_typecmp(
void*t1,void*t2);int Cyc_Tcutil_typecmp(void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);if(t1 == t2)return 0;{int _tmp3BA=Cyc_Core_intcmp(
Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));if(_tmp3BA != 0)
return _tmp3BA;{struct _tuple0 _tmpD5D;struct _tuple0 _tmp3BC=(_tmpD5D.f1=t2,((
_tmpD5D.f2=t1,_tmpD5D)));void*_tmp3BD;void*_tmp3BE;void*_tmp3BF;struct Cyc_Absyn_Tvar*
_tmp3C0;void*_tmp3C1;struct Cyc_Absyn_Tvar*_tmp3C2;void*_tmp3C3;struct Cyc_Absyn_AggrInfo
_tmp3C4;union Cyc_Absyn_AggrInfoU_union _tmp3C5;struct Cyc_List_List*_tmp3C6;void*
_tmp3C7;struct Cyc_Absyn_AggrInfo _tmp3C8;union Cyc_Absyn_AggrInfoU_union _tmp3C9;
struct Cyc_List_List*_tmp3CA;void*_tmp3CB;struct _tuple2*_tmp3CC;void*_tmp3CD;
struct _tuple2*_tmp3CE;void*_tmp3CF;struct Cyc_List_List*_tmp3D0;void*_tmp3D1;
struct Cyc_List_List*_tmp3D2;void*_tmp3D3;struct Cyc_Absyn_TunionInfo _tmp3D4;union
Cyc_Absyn_TunionInfoU_union _tmp3D5;struct Cyc_Absyn_Tuniondecl**_tmp3D6;struct Cyc_Absyn_Tuniondecl*
_tmp3D7;struct Cyc_List_List*_tmp3D8;struct Cyc_Core_Opt*_tmp3D9;void*_tmp3DA;
struct Cyc_Absyn_TunionInfo _tmp3DB;union Cyc_Absyn_TunionInfoU_union _tmp3DC;struct
Cyc_Absyn_Tuniondecl**_tmp3DD;struct Cyc_Absyn_Tuniondecl*_tmp3DE;struct Cyc_List_List*
_tmp3DF;struct Cyc_Core_Opt*_tmp3E0;void*_tmp3E1;struct Cyc_Absyn_TunionFieldInfo
_tmp3E2;union Cyc_Absyn_TunionFieldInfoU_union _tmp3E3;struct Cyc_Absyn_Tuniondecl*
_tmp3E4;struct Cyc_Absyn_Tunionfield*_tmp3E5;struct Cyc_List_List*_tmp3E6;void*
_tmp3E7;struct Cyc_Absyn_TunionFieldInfo _tmp3E8;union Cyc_Absyn_TunionFieldInfoU_union
_tmp3E9;struct Cyc_Absyn_Tuniondecl*_tmp3EA;struct Cyc_Absyn_Tunionfield*_tmp3EB;
struct Cyc_List_List*_tmp3EC;void*_tmp3ED;struct Cyc_Absyn_PtrInfo _tmp3EE;void*
_tmp3EF;struct Cyc_Absyn_Tqual _tmp3F0;struct Cyc_Absyn_PtrAtts _tmp3F1;void*_tmp3F2;
struct Cyc_Absyn_Conref*_tmp3F3;struct Cyc_Absyn_Conref*_tmp3F4;struct Cyc_Absyn_Conref*
_tmp3F5;void*_tmp3F6;struct Cyc_Absyn_PtrInfo _tmp3F7;void*_tmp3F8;struct Cyc_Absyn_Tqual
_tmp3F9;struct Cyc_Absyn_PtrAtts _tmp3FA;void*_tmp3FB;struct Cyc_Absyn_Conref*
_tmp3FC;struct Cyc_Absyn_Conref*_tmp3FD;struct Cyc_Absyn_Conref*_tmp3FE;void*
_tmp3FF;void*_tmp400;void*_tmp401;void*_tmp402;void*_tmp403;void*_tmp404;void*
_tmp405;void*_tmp406;void*_tmp407;int _tmp408;void*_tmp409;int _tmp40A;void*
_tmp40B;struct Cyc_Absyn_ArrayInfo _tmp40C;void*_tmp40D;struct Cyc_Absyn_Tqual
_tmp40E;struct Cyc_Absyn_Exp*_tmp40F;struct Cyc_Absyn_Conref*_tmp410;void*_tmp411;
struct Cyc_Absyn_ArrayInfo _tmp412;void*_tmp413;struct Cyc_Absyn_Tqual _tmp414;
struct Cyc_Absyn_Exp*_tmp415;struct Cyc_Absyn_Conref*_tmp416;void*_tmp417;struct
Cyc_Absyn_FnInfo _tmp418;struct Cyc_List_List*_tmp419;struct Cyc_Core_Opt*_tmp41A;
void*_tmp41B;struct Cyc_List_List*_tmp41C;int _tmp41D;struct Cyc_Absyn_VarargInfo*
_tmp41E;struct Cyc_List_List*_tmp41F;struct Cyc_List_List*_tmp420;void*_tmp421;
struct Cyc_Absyn_FnInfo _tmp422;struct Cyc_List_List*_tmp423;struct Cyc_Core_Opt*
_tmp424;void*_tmp425;struct Cyc_List_List*_tmp426;int _tmp427;struct Cyc_Absyn_VarargInfo*
_tmp428;struct Cyc_List_List*_tmp429;struct Cyc_List_List*_tmp42A;void*_tmp42B;
struct Cyc_List_List*_tmp42C;void*_tmp42D;struct Cyc_List_List*_tmp42E;void*
_tmp42F;void*_tmp430;struct Cyc_List_List*_tmp431;void*_tmp432;void*_tmp433;
struct Cyc_List_List*_tmp434;void*_tmp435;void*_tmp436;void*_tmp437;void*_tmp438;
void*_tmp439;void*_tmp43A;void*_tmp43B;void*_tmp43C;void*_tmp43D;void*_tmp43E;
void*_tmp43F;void*_tmp440;void*_tmp441;void*_tmp442;void*_tmp443;void*_tmp444;
void*_tmp445;void*_tmp446;void*_tmp447;int _tmp448;void*_tmp449;int _tmp44A;void*
_tmp44B;void*_tmp44C;void*_tmp44D;void*_tmp44E;void*_tmp44F;void*_tmp450;_LL2DD:
_tmp3BD=_tmp3BC.f1;if(_tmp3BD <= (void*)4)goto _LL2DF;if(*((int*)_tmp3BD)!= 0)goto
_LL2DF;_tmp3BE=_tmp3BC.f2;if(_tmp3BE <= (void*)4)goto _LL2DF;if(*((int*)_tmp3BE)!= 
0)goto _LL2DF;_LL2DE: {const char*_tmpD60;void*_tmpD5F;(_tmpD5F=0,((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmpD60="typecmp: can only compare closed types",
_tag_dynforward(_tmpD60,sizeof(char),_get_zero_arr_size_char(_tmpD60,39)))),
_tag_dynforward(_tmpD5F,sizeof(void*),0)));}_LL2DF: _tmp3BF=_tmp3BC.f1;if(_tmp3BF
<= (void*)4)goto _LL2E1;if(*((int*)_tmp3BF)!= 1)goto _LL2E1;_tmp3C0=((struct Cyc_Absyn_VarType_struct*)
_tmp3BF)->f1;_tmp3C1=_tmp3BC.f2;if(_tmp3C1 <= (void*)4)goto _LL2E1;if(*((int*)
_tmp3C1)!= 1)goto _LL2E1;_tmp3C2=((struct Cyc_Absyn_VarType_struct*)_tmp3C1)->f1;
_LL2E0: return Cyc_Core_intcmp(_tmp3C2->identity,_tmp3C0->identity);_LL2E1: _tmp3C3=
_tmp3BC.f1;if(_tmp3C3 <= (void*)4)goto _LL2E3;if(*((int*)_tmp3C3)!= 10)goto _LL2E3;
_tmp3C4=((struct Cyc_Absyn_AggrType_struct*)_tmp3C3)->f1;_tmp3C5=_tmp3C4.aggr_info;
_tmp3C6=_tmp3C4.targs;_tmp3C7=_tmp3BC.f2;if(_tmp3C7 <= (void*)4)goto _LL2E3;if(*((
int*)_tmp3C7)!= 10)goto _LL2E3;_tmp3C8=((struct Cyc_Absyn_AggrType_struct*)_tmp3C7)->f1;
_tmp3C9=_tmp3C8.aggr_info;_tmp3CA=_tmp3C8.targs;_LL2E2: {struct _tuple2*_tmp454;
struct _tuple6 _tmp453=Cyc_Absyn_aggr_kinded_name(_tmp3C5);_tmp454=_tmp453.f2;{
struct _tuple2*_tmp456;struct _tuple6 _tmp455=Cyc_Absyn_aggr_kinded_name(_tmp3C9);
_tmp456=_tmp455.f2;{int _tmp457=Cyc_Absyn_qvar_cmp(_tmp454,_tmp456);if(_tmp457 != 
0)return _tmp457;else{return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,_tmp3C6,_tmp3CA);}}}}
_LL2E3: _tmp3CB=_tmp3BC.f1;if(_tmp3CB <= (void*)4)goto _LL2E5;if(*((int*)_tmp3CB)!= 
12)goto _LL2E5;_tmp3CC=((struct Cyc_Absyn_EnumType_struct*)_tmp3CB)->f1;_tmp3CD=
_tmp3BC.f2;if(_tmp3CD <= (void*)4)goto _LL2E5;if(*((int*)_tmp3CD)!= 12)goto _LL2E5;
_tmp3CE=((struct Cyc_Absyn_EnumType_struct*)_tmp3CD)->f1;_LL2E4: return Cyc_Absyn_qvar_cmp(
_tmp3CC,_tmp3CE);_LL2E5: _tmp3CF=_tmp3BC.f1;if(_tmp3CF <= (void*)4)goto _LL2E7;if(*((
int*)_tmp3CF)!= 13)goto _LL2E7;_tmp3D0=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp3CF)->f1;_tmp3D1=_tmp3BC.f2;if(_tmp3D1 <= (void*)4)goto _LL2E7;if(*((int*)
_tmp3D1)!= 13)goto _LL2E7;_tmp3D2=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp3D1)->f1;
_LL2E6: return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,
_tmp3D0,_tmp3D2);_LL2E7: _tmp3D3=_tmp3BC.f1;if(_tmp3D3 <= (void*)4)goto _LL2E9;if(*((
int*)_tmp3D3)!= 2)goto _LL2E9;_tmp3D4=((struct Cyc_Absyn_TunionType_struct*)
_tmp3D3)->f1;_tmp3D5=_tmp3D4.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp3D3)->f1).tunion_info).KnownTunion).tag != 1)goto _LL2E9;_tmp3D6=(_tmp3D5.KnownTunion).f1;
_tmp3D7=*_tmp3D6;_tmp3D8=_tmp3D4.targs;_tmp3D9=_tmp3D4.rgn;_tmp3DA=_tmp3BC.f2;
if(_tmp3DA <= (void*)4)goto _LL2E9;if(*((int*)_tmp3DA)!= 2)goto _LL2E9;_tmp3DB=((
struct Cyc_Absyn_TunionType_struct*)_tmp3DA)->f1;_tmp3DC=_tmp3DB.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp3DA)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL2E9;_tmp3DD=(_tmp3DC.KnownTunion).f1;_tmp3DE=*_tmp3DD;_tmp3DF=_tmp3DB.targs;
_tmp3E0=_tmp3DB.rgn;_LL2E8: if(_tmp3DE == _tmp3D7)return 0;{int _tmp458=Cyc_Absyn_qvar_cmp(
_tmp3DE->name,_tmp3D7->name);if(_tmp458 != 0)return _tmp458;if((unsigned int)
_tmp3E0  && (unsigned int)_tmp3D9){int _tmp459=Cyc_Tcutil_typecmp((void*)_tmp3E0->v,(
void*)_tmp3D9->v);if(_tmp459 != 0)return _tmp459;}else{if((unsigned int)_tmp3E0)
return - 1;else{return 1;}}return Cyc_List_list_cmp(Cyc_Tcutil_typecmp,_tmp3DF,
_tmp3D8);}_LL2E9: _tmp3E1=_tmp3BC.f1;if(_tmp3E1 <= (void*)4)goto _LL2EB;if(*((int*)
_tmp3E1)!= 3)goto _LL2EB;_tmp3E2=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3E1)->f1;_tmp3E3=_tmp3E2.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp3E1)->f1).field_info).KnownTunionfield).tag != 1)goto _LL2EB;_tmp3E4=(_tmp3E3.KnownTunionfield).f1;
_tmp3E5=(_tmp3E3.KnownTunionfield).f2;_tmp3E6=_tmp3E2.targs;_tmp3E7=_tmp3BC.f2;
if(_tmp3E7 <= (void*)4)goto _LL2EB;if(*((int*)_tmp3E7)!= 3)goto _LL2EB;_tmp3E8=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp3E7)->f1;_tmp3E9=_tmp3E8.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp3E7)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL2EB;_tmp3EA=(_tmp3E9.KnownTunionfield).f1;_tmp3EB=(_tmp3E9.KnownTunionfield).f2;
_tmp3EC=_tmp3E8.targs;_LL2EA: if(_tmp3EA == _tmp3E4)return 0;{int _tmp45A=Cyc_Absyn_qvar_cmp(
_tmp3E4->name,_tmp3EA->name);if(_tmp45A != 0)return _tmp45A;{int _tmp45B=Cyc_Absyn_qvar_cmp(
_tmp3E5->name,_tmp3EB->name);if(_tmp45B != 0)return _tmp45B;return Cyc_List_list_cmp(
Cyc_Tcutil_typecmp,_tmp3EC,_tmp3E6);}}_LL2EB: _tmp3ED=_tmp3BC.f1;if(_tmp3ED <= (
void*)4)goto _LL2ED;if(*((int*)_tmp3ED)!= 4)goto _LL2ED;_tmp3EE=((struct Cyc_Absyn_PointerType_struct*)
_tmp3ED)->f1;_tmp3EF=(void*)_tmp3EE.elt_typ;_tmp3F0=_tmp3EE.elt_tq;_tmp3F1=
_tmp3EE.ptr_atts;_tmp3F2=(void*)_tmp3F1.rgn;_tmp3F3=_tmp3F1.nullable;_tmp3F4=
_tmp3F1.bounds;_tmp3F5=_tmp3F1.zero_term;_tmp3F6=_tmp3BC.f2;if(_tmp3F6 <= (void*)
4)goto _LL2ED;if(*((int*)_tmp3F6)!= 4)goto _LL2ED;_tmp3F7=((struct Cyc_Absyn_PointerType_struct*)
_tmp3F6)->f1;_tmp3F8=(void*)_tmp3F7.elt_typ;_tmp3F9=_tmp3F7.elt_tq;_tmp3FA=
_tmp3F7.ptr_atts;_tmp3FB=(void*)_tmp3FA.rgn;_tmp3FC=_tmp3FA.nullable;_tmp3FD=
_tmp3FA.bounds;_tmp3FE=_tmp3FA.zero_term;_LL2EC: {int _tmp45C=Cyc_Tcutil_typecmp(
_tmp3F8,_tmp3EF);if(_tmp45C != 0)return _tmp45C;{int _tmp45D=Cyc_Tcutil_typecmp(
_tmp3FB,_tmp3F2);if(_tmp45D != 0)return _tmp45D;{int _tmp45E=Cyc_Tcutil_tqual_cmp(
_tmp3F9,_tmp3F0);if(_tmp45E != 0)return _tmp45E;{int _tmp45F=Cyc_Tcutil_conrefs_cmp(
Cyc_Tcutil_boundscmp,_tmp3FD,_tmp3F4);if(_tmp45F != 0)return _tmp45F;{int _tmp460=((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(
Cyc_Core_intcmp,_tmp3FE,_tmp3F5);if(_tmp460 != 0)return _tmp460;{union Cyc_Absyn_Constraint_union
_tmp461=(Cyc_Absyn_compress_conref(_tmp3FD))->v;void*_tmp462;void*_tmp463;_LL314:
if((_tmp461.Eq_constr).tag != 0)goto _LL316;_tmp462=(_tmp461.Eq_constr).f1;if((int)
_tmp462 != 0)goto _LL316;_LL315: return 0;_LL316: if((_tmp461.Eq_constr).tag != 0)goto
_LL318;_tmp463=(_tmp461.Eq_constr).f1;if((int)_tmp463 != 1)goto _LL318;_LL317:
return 0;_LL318:;_LL319: goto _LL313;_LL313:;}return((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,
_tmp3FC,_tmp3F3);}}}}}_LL2ED: _tmp3FF=_tmp3BC.f1;if(_tmp3FF <= (void*)4)goto _LL2EF;
if(*((int*)_tmp3FF)!= 5)goto _LL2EF;_tmp400=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp3FF)->f1;_tmp401=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp3FF)->f2;
_tmp402=_tmp3BC.f2;if(_tmp402 <= (void*)4)goto _LL2EF;if(*((int*)_tmp402)!= 5)goto
_LL2EF;_tmp403=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp402)->f1;_tmp404=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp402)->f2;_LL2EE: if(_tmp403 != _tmp400)
return Cyc_Core_intcmp((int)((unsigned int)_tmp403),(int)((unsigned int)_tmp400));
if(_tmp404 != _tmp401)return Cyc_Core_intcmp((int)((unsigned int)_tmp404),(int)((
unsigned int)_tmp401));return 0;_LL2EF: _tmp405=_tmp3BC.f1;if((int)_tmp405 != 1)
goto _LL2F1;_tmp406=_tmp3BC.f2;if((int)_tmp406 != 1)goto _LL2F1;_LL2F0: return 0;
_LL2F1: _tmp407=_tmp3BC.f1;if(_tmp407 <= (void*)4)goto _LL2F3;if(*((int*)_tmp407)!= 
6)goto _LL2F3;_tmp408=((struct Cyc_Absyn_DoubleType_struct*)_tmp407)->f1;_tmp409=
_tmp3BC.f2;if(_tmp409 <= (void*)4)goto _LL2F3;if(*((int*)_tmp409)!= 6)goto _LL2F3;
_tmp40A=((struct Cyc_Absyn_DoubleType_struct*)_tmp409)->f1;_LL2F2: if(_tmp408 == 
_tmp40A)return 0;else{if(_tmp408)return - 1;else{return 1;}}_LL2F3: _tmp40B=_tmp3BC.f1;
if(_tmp40B <= (void*)4)goto _LL2F5;if(*((int*)_tmp40B)!= 7)goto _LL2F5;_tmp40C=((
struct Cyc_Absyn_ArrayType_struct*)_tmp40B)->f1;_tmp40D=(void*)_tmp40C.elt_type;
_tmp40E=_tmp40C.tq;_tmp40F=_tmp40C.num_elts;_tmp410=_tmp40C.zero_term;_tmp411=
_tmp3BC.f2;if(_tmp411 <= (void*)4)goto _LL2F5;if(*((int*)_tmp411)!= 7)goto _LL2F5;
_tmp412=((struct Cyc_Absyn_ArrayType_struct*)_tmp411)->f1;_tmp413=(void*)_tmp412.elt_type;
_tmp414=_tmp412.tq;_tmp415=_tmp412.num_elts;_tmp416=_tmp412.zero_term;_LL2F4: {
int _tmp464=Cyc_Tcutil_tqual_cmp(_tmp414,_tmp40E);if(_tmp464 != 0)return _tmp464;{
int _tmp465=Cyc_Tcutil_typecmp(_tmp413,_tmp40D);if(_tmp465 != 0)return _tmp465;{int
_tmp466=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp410,_tmp416);if(_tmp466 != 0)
return _tmp466;if(_tmp40F == _tmp415)return 0;if(_tmp40F == 0  || _tmp415 == 0){const
char*_tmpD63;void*_tmpD62;(_tmpD62=0,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmpD63="missing expression in array index",
_tag_dynforward(_tmpD63,sizeof(char),_get_zero_arr_size_char(_tmpD63,34)))),
_tag_dynforward(_tmpD62,sizeof(void*),0)));}return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(
Cyc_Evexp_const_exp_cmp,_tmp40F,_tmp415);}}}_LL2F5: _tmp417=_tmp3BC.f1;if(_tmp417
<= (void*)4)goto _LL2F7;if(*((int*)_tmp417)!= 8)goto _LL2F7;_tmp418=((struct Cyc_Absyn_FnType_struct*)
_tmp417)->f1;_tmp419=_tmp418.tvars;_tmp41A=_tmp418.effect;_tmp41B=(void*)_tmp418.ret_typ;
_tmp41C=_tmp418.args;_tmp41D=_tmp418.c_varargs;_tmp41E=_tmp418.cyc_varargs;
_tmp41F=_tmp418.rgn_po;_tmp420=_tmp418.attributes;_tmp421=_tmp3BC.f2;if(_tmp421
<= (void*)4)goto _LL2F7;if(*((int*)_tmp421)!= 8)goto _LL2F7;_tmp422=((struct Cyc_Absyn_FnType_struct*)
_tmp421)->f1;_tmp423=_tmp422.tvars;_tmp424=_tmp422.effect;_tmp425=(void*)_tmp422.ret_typ;
_tmp426=_tmp422.args;_tmp427=_tmp422.c_varargs;_tmp428=_tmp422.cyc_varargs;
_tmp429=_tmp422.rgn_po;_tmp42A=_tmp422.attributes;_LL2F6: {const char*_tmpD66;
void*_tmpD65;(_tmpD65=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr
ap))Cyc_Tcutil_impos)(((_tmpD66="typecmp: function types not handled",
_tag_dynforward(_tmpD66,sizeof(char),_get_zero_arr_size_char(_tmpD66,36)))),
_tag_dynforward(_tmpD65,sizeof(void*),0)));}_LL2F7: _tmp42B=_tmp3BC.f1;if(_tmp42B
<= (void*)4)goto _LL2F9;if(*((int*)_tmp42B)!= 9)goto _LL2F9;_tmp42C=((struct Cyc_Absyn_TupleType_struct*)
_tmp42B)->f1;_tmp42D=_tmp3BC.f2;if(_tmp42D <= (void*)4)goto _LL2F9;if(*((int*)
_tmp42D)!= 9)goto _LL2F9;_tmp42E=((struct Cyc_Absyn_TupleType_struct*)_tmp42D)->f1;
_LL2F8: return((int(*)(int(*cmp)(struct _tuple5*,struct _tuple5*),struct Cyc_List_List*
l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp42E,
_tmp42C);_LL2F9: _tmp42F=_tmp3BC.f1;if(_tmp42F <= (void*)4)goto _LL2FB;if(*((int*)
_tmp42F)!= 11)goto _LL2FB;_tmp430=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp42F)->f1;_tmp431=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp42F)->f2;_tmp432=
_tmp3BC.f2;if(_tmp432 <= (void*)4)goto _LL2FB;if(*((int*)_tmp432)!= 11)goto _LL2FB;
_tmp433=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp432)->f1;_tmp434=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp432)->f2;_LL2FA: if(_tmp433 != _tmp430){
if(_tmp433 == (void*)0)return - 1;else{return 1;}}return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(
Cyc_Tcutil_aggrfield_cmp,_tmp434,_tmp431);_LL2FB: _tmp435=_tmp3BC.f1;if(_tmp435 <= (
void*)4)goto _LL2FD;if(*((int*)_tmp435)!= 15)goto _LL2FD;_tmp436=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp435)->f1;_tmp437=_tmp3BC.f2;if(_tmp437 <= (
void*)4)goto _LL2FD;if(*((int*)_tmp437)!= 15)goto _LL2FD;_tmp438=(void*)((struct
Cyc_Absyn_RgnHandleType_struct*)_tmp437)->f1;_LL2FC: return Cyc_Tcutil_typecmp(
_tmp436,_tmp438);_LL2FD: _tmp439=_tmp3BC.f1;if(_tmp439 <= (void*)4)goto _LL2FF;if(*((
int*)_tmp439)!= 16)goto _LL2FF;_tmp43A=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp439)->f1;_tmp43B=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp439)->f2;
_tmp43C=_tmp3BC.f2;if(_tmp43C <= (void*)4)goto _LL2FF;if(*((int*)_tmp43C)!= 16)
goto _LL2FF;_tmp43D=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp43C)->f1;
_tmp43E=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp43C)->f2;_LL2FE: {int
_tmp46B=Cyc_Tcutil_typecmp(_tmp43A,_tmp43D);if(_tmp46B != 0)return _tmp46B;else{
return Cyc_Tcutil_typecmp(_tmp43B,_tmp43E);}}_LL2FF: _tmp43F=_tmp3BC.f1;if(_tmp43F
<= (void*)4)goto _LL301;if(*((int*)_tmp43F)!= 14)goto _LL301;_tmp440=(void*)((
struct Cyc_Absyn_SizeofType_struct*)_tmp43F)->f1;_tmp441=_tmp3BC.f2;if(_tmp441 <= (
void*)4)goto _LL301;if(*((int*)_tmp441)!= 14)goto _LL301;_tmp442=(void*)((struct
Cyc_Absyn_SizeofType_struct*)_tmp441)->f1;_LL300: return Cyc_Tcutil_typecmp(
_tmp440,_tmp442);_LL301: _tmp443=_tmp3BC.f1;if(_tmp443 <= (void*)4)goto _LL303;if(*((
int*)_tmp443)!= 18)goto _LL303;_tmp444=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp443)->f1;_tmp445=_tmp3BC.f2;if(_tmp445 <= (void*)4)goto _LL303;if(*((int*)
_tmp445)!= 18)goto _LL303;_tmp446=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp445)->f1;_LL302: return Cyc_Tcutil_typecmp(_tmp444,_tmp446);_LL303: _tmp447=
_tmp3BC.f1;if(_tmp447 <= (void*)4)goto _LL305;if(*((int*)_tmp447)!= 19)goto _LL305;
_tmp448=((struct Cyc_Absyn_TypeInt_struct*)_tmp447)->f1;_tmp449=_tmp3BC.f2;if(
_tmp449 <= (void*)4)goto _LL305;if(*((int*)_tmp449)!= 19)goto _LL305;_tmp44A=((
struct Cyc_Absyn_TypeInt_struct*)_tmp449)->f1;_LL304: return Cyc_Core_intcmp(
_tmp448,_tmp44A);_LL305: _tmp44B=_tmp3BC.f1;if(_tmp44B <= (void*)4)goto _LL307;if(*((
int*)_tmp44B)!= 21)goto _LL307;_LL306: goto _LL308;_LL307: _tmp44C=_tmp3BC.f2;if(
_tmp44C <= (void*)4)goto _LL309;if(*((int*)_tmp44C)!= 21)goto _LL309;_LL308: goto
_LL30A;_LL309: _tmp44D=_tmp3BC.f1;if(_tmp44D <= (void*)4)goto _LL30B;if(*((int*)
_tmp44D)!= 20)goto _LL30B;_LL30A: goto _LL30C;_LL30B: _tmp44E=_tmp3BC.f1;if(_tmp44E
<= (void*)4)goto _LL30D;if(*((int*)_tmp44E)!= 22)goto _LL30D;_LL30C: goto _LL30E;
_LL30D: _tmp44F=_tmp3BC.f2;if(_tmp44F <= (void*)4)goto _LL30F;if(*((int*)_tmp44F)!= 
22)goto _LL30F;_LL30E: goto _LL310;_LL30F: _tmp450=_tmp3BC.f2;if(_tmp450 <= (void*)4)
goto _LL311;if(*((int*)_tmp450)!= 20)goto _LL311;_LL310: {const char*_tmpD69;void*
_tmpD68;(_tmpD68=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(((_tmpD69="typecmp: effects not handled",_tag_dynforward(
_tmpD69,sizeof(char),_get_zero_arr_size_char(_tmpD69,29)))),_tag_dynforward(
_tmpD68,sizeof(void*),0)));}_LL311:;_LL312: {const char*_tmpD6C;void*_tmpD6B;(
_tmpD6B=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmpD6C="Unmatched case in typecmp",_tag_dynforward(_tmpD6C,sizeof(char),
_get_zero_arr_size_char(_tmpD6C,26)))),_tag_dynforward(_tmpD6B,sizeof(void*),0)));}
_LL2DC:;}}}int Cyc_Tcutil_is_arithmetic_type(void*t);int Cyc_Tcutil_is_arithmetic_type(
void*t){void*_tmp470=Cyc_Tcutil_compress(t);_LL31B: if(_tmp470 <= (void*)4)goto
_LL31D;if(*((int*)_tmp470)!= 5)goto _LL31D;_LL31C: goto _LL31E;_LL31D: if((int)
_tmp470 != 1)goto _LL31F;_LL31E: goto _LL320;_LL31F: if(_tmp470 <= (void*)4)goto _LL325;
if(*((int*)_tmp470)!= 6)goto _LL321;_LL320: goto _LL322;_LL321: if(*((int*)_tmp470)
!= 12)goto _LL323;_LL322: goto _LL324;_LL323: if(*((int*)_tmp470)!= 13)goto _LL325;
_LL324: return 1;_LL325:;_LL326: return 0;_LL31A:;}int Cyc_Tcutil_will_lose_precision(
void*t1,void*t2);int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmpD6D;struct _tuple0 _tmp472=(
_tmpD6D.f1=t1,((_tmpD6D.f2=t2,_tmpD6D)));void*_tmp473;int _tmp474;void*_tmp475;
int _tmp476;void*_tmp477;void*_tmp478;void*_tmp479;void*_tmp47A;void*_tmp47B;void*
_tmp47C;void*_tmp47D;void*_tmp47E;void*_tmp47F;void*_tmp480;void*_tmp481;void*
_tmp482;void*_tmp483;void*_tmp484;void*_tmp485;void*_tmp486;void*_tmp487;void*
_tmp488;void*_tmp489;void*_tmp48A;void*_tmp48B;void*_tmp48C;void*_tmp48D;void*
_tmp48E;void*_tmp48F;void*_tmp490;void*_tmp491;void*_tmp492;void*_tmp493;void*
_tmp494;void*_tmp495;void*_tmp496;void*_tmp497;void*_tmp498;void*_tmp499;void*
_tmp49A;void*_tmp49B;void*_tmp49C;void*_tmp49D;void*_tmp49E;void*_tmp49F;void*
_tmp4A0;void*_tmp4A1;void*_tmp4A2;void*_tmp4A3;void*_tmp4A4;void*_tmp4A5;void*
_tmp4A6;void*_tmp4A7;void*_tmp4A8;void*_tmp4A9;void*_tmp4AA;void*_tmp4AB;void*
_tmp4AC;void*_tmp4AD;void*_tmp4AE;void*_tmp4AF;void*_tmp4B0;void*_tmp4B1;void*
_tmp4B2;void*_tmp4B3;void*_tmp4B4;void*_tmp4B5;void*_tmp4B6;void*_tmp4B7;void*
_tmp4B8;_LL328: _tmp473=_tmp472.f1;if(_tmp473 <= (void*)4)goto _LL32A;if(*((int*)
_tmp473)!= 6)goto _LL32A;_tmp474=((struct Cyc_Absyn_DoubleType_struct*)_tmp473)->f1;
_tmp475=_tmp472.f2;if(_tmp475 <= (void*)4)goto _LL32A;if(*((int*)_tmp475)!= 6)goto
_LL32A;_tmp476=((struct Cyc_Absyn_DoubleType_struct*)_tmp475)->f1;_LL329: return !
_tmp476  && _tmp474;_LL32A: _tmp477=_tmp472.f1;if(_tmp477 <= (void*)4)goto _LL32C;
if(*((int*)_tmp477)!= 6)goto _LL32C;_tmp478=_tmp472.f2;if((int)_tmp478 != 1)goto
_LL32C;_LL32B: goto _LL32D;_LL32C: _tmp479=_tmp472.f1;if(_tmp479 <= (void*)4)goto
_LL32E;if(*((int*)_tmp479)!= 6)goto _LL32E;_tmp47A=_tmp472.f2;if(_tmp47A <= (void*)
4)goto _LL32E;if(*((int*)_tmp47A)!= 5)goto _LL32E;_LL32D: goto _LL32F;_LL32E: _tmp47B=
_tmp472.f1;if(_tmp47B <= (void*)4)goto _LL330;if(*((int*)_tmp47B)!= 6)goto _LL330;
_tmp47C=_tmp472.f2;if(_tmp47C <= (void*)4)goto _LL330;if(*((int*)_tmp47C)!= 14)
goto _LL330;_LL32F: goto _LL331;_LL330: _tmp47D=_tmp472.f1;if((int)_tmp47D != 1)goto
_LL332;_tmp47E=_tmp472.f2;if(_tmp47E <= (void*)4)goto _LL332;if(*((int*)_tmp47E)!= 
14)goto _LL332;_LL331: goto _LL333;_LL332: _tmp47F=_tmp472.f1;if(_tmp47F <= (void*)4)
goto _LL334;if(*((int*)_tmp47F)!= 6)goto _LL334;_tmp480=_tmp472.f2;if(_tmp480 <= (
void*)4)goto _LL334;if(*((int*)_tmp480)!= 18)goto _LL334;_LL333: goto _LL335;_LL334:
_tmp481=_tmp472.f1;if((int)_tmp481 != 1)goto _LL336;_tmp482=_tmp472.f2;if(_tmp482
<= (void*)4)goto _LL336;if(*((int*)_tmp482)!= 18)goto _LL336;_LL335: goto _LL337;
_LL336: _tmp483=_tmp472.f1;if((int)_tmp483 != 1)goto _LL338;_tmp484=_tmp472.f2;if(
_tmp484 <= (void*)4)goto _LL338;if(*((int*)_tmp484)!= 5)goto _LL338;_LL337: return 1;
_LL338: _tmp485=_tmp472.f1;if(_tmp485 <= (void*)4)goto _LL33A;if(*((int*)_tmp485)!= 
5)goto _LL33A;_tmp486=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp485)->f2;if((
int)_tmp486 != 4)goto _LL33A;_tmp487=_tmp472.f2;if(_tmp487 <= (void*)4)goto _LL33A;
if(*((int*)_tmp487)!= 5)goto _LL33A;_tmp488=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp487)->f2;if((int)_tmp488 != 4)goto _LL33A;_LL339: return 0;_LL33A: _tmp489=
_tmp472.f1;if(_tmp489 <= (void*)4)goto _LL33C;if(*((int*)_tmp489)!= 5)goto _LL33C;
_tmp48A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp489)->f2;if((int)_tmp48A != 
4)goto _LL33C;_LL33B: return 1;_LL33C: _tmp48B=_tmp472.f1;if(_tmp48B <= (void*)4)goto
_LL33E;if(*((int*)_tmp48B)!= 5)goto _LL33E;_tmp48C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp48B)->f2;if((int)_tmp48C != 3)goto _LL33E;_tmp48D=_tmp472.f2;if(_tmp48D <= (
void*)4)goto _LL33E;if(*((int*)_tmp48D)!= 5)goto _LL33E;_tmp48E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp48D)->f2;if((int)_tmp48E != 2)goto _LL33E;_LL33D: goto _LL33F;_LL33E: _tmp48F=
_tmp472.f1;if(_tmp48F <= (void*)4)goto _LL340;if(*((int*)_tmp48F)!= 5)goto _LL340;
_tmp490=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp48F)->f2;if((int)_tmp490 != 
2)goto _LL340;_tmp491=_tmp472.f2;if(_tmp491 <= (void*)4)goto _LL340;if(*((int*)
_tmp491)!= 5)goto _LL340;_tmp492=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp491)->f2;
if((int)_tmp492 != 3)goto _LL340;_LL33F: return 0;_LL340: _tmp493=_tmp472.f1;if(
_tmp493 <= (void*)4)goto _LL342;if(*((int*)_tmp493)!= 5)goto _LL342;_tmp494=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp493)->f2;if((int)_tmp494 != 3)goto _LL342;
_tmp495=_tmp472.f2;if((int)_tmp495 != 1)goto _LL342;_LL341: goto _LL343;_LL342:
_tmp496=_tmp472.f1;if(_tmp496 <= (void*)4)goto _LL344;if(*((int*)_tmp496)!= 5)goto
_LL344;_tmp497=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp496)->f2;if((int)
_tmp497 != 2)goto _LL344;_tmp498=_tmp472.f2;if((int)_tmp498 != 1)goto _LL344;_LL343:
goto _LL345;_LL344: _tmp499=_tmp472.f1;if(_tmp499 <= (void*)4)goto _LL346;if(*((int*)
_tmp499)!= 5)goto _LL346;_tmp49A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp499)->f2;
if((int)_tmp49A != 3)goto _LL346;_tmp49B=_tmp472.f2;if(_tmp49B <= (void*)4)goto
_LL346;if(*((int*)_tmp49B)!= 5)goto _LL346;_tmp49C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp49B)->f2;if((int)_tmp49C != 1)goto _LL346;_LL345: goto _LL347;_LL346: _tmp49D=
_tmp472.f1;if(_tmp49D <= (void*)4)goto _LL348;if(*((int*)_tmp49D)!= 5)goto _LL348;
_tmp49E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp49D)->f2;if((int)_tmp49E != 
2)goto _LL348;_tmp49F=_tmp472.f2;if(_tmp49F <= (void*)4)goto _LL348;if(*((int*)
_tmp49F)!= 5)goto _LL348;_tmp4A0=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp49F)->f2;
if((int)_tmp4A0 != 1)goto _LL348;_LL347: goto _LL349;_LL348: _tmp4A1=_tmp472.f1;if(
_tmp4A1 <= (void*)4)goto _LL34A;if(*((int*)_tmp4A1)!= 18)goto _LL34A;_tmp4A2=
_tmp472.f2;if(_tmp4A2 <= (void*)4)goto _LL34A;if(*((int*)_tmp4A2)!= 5)goto _LL34A;
_tmp4A3=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A2)->f2;if((int)_tmp4A3 != 
1)goto _LL34A;_LL349: goto _LL34B;_LL34A: _tmp4A4=_tmp472.f1;if(_tmp4A4 <= (void*)4)
goto _LL34C;if(*((int*)_tmp4A4)!= 14)goto _LL34C;_tmp4A5=_tmp472.f2;if(_tmp4A5 <= (
void*)4)goto _LL34C;if(*((int*)_tmp4A5)!= 5)goto _LL34C;_tmp4A6=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4A5)->f2;if((int)_tmp4A6 != 1)goto _LL34C;_LL34B: goto _LL34D;_LL34C: _tmp4A7=
_tmp472.f1;if(_tmp4A7 <= (void*)4)goto _LL34E;if(*((int*)_tmp4A7)!= 5)goto _LL34E;
_tmp4A8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A7)->f2;if((int)_tmp4A8 != 
3)goto _LL34E;_tmp4A9=_tmp472.f2;if(_tmp4A9 <= (void*)4)goto _LL34E;if(*((int*)
_tmp4A9)!= 5)goto _LL34E;_tmp4AA=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A9)->f2;
if((int)_tmp4AA != 0)goto _LL34E;_LL34D: goto _LL34F;_LL34E: _tmp4AB=_tmp472.f1;if(
_tmp4AB <= (void*)4)goto _LL350;if(*((int*)_tmp4AB)!= 5)goto _LL350;_tmp4AC=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp4AB)->f2;if((int)_tmp4AC != 2)goto _LL350;
_tmp4AD=_tmp472.f2;if(_tmp4AD <= (void*)4)goto _LL350;if(*((int*)_tmp4AD)!= 5)goto
_LL350;_tmp4AE=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4AD)->f2;if((int)
_tmp4AE != 0)goto _LL350;_LL34F: goto _LL351;_LL350: _tmp4AF=_tmp472.f1;if(_tmp4AF <= (
void*)4)goto _LL352;if(*((int*)_tmp4AF)!= 5)goto _LL352;_tmp4B0=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4AF)->f2;if((int)_tmp4B0 != 1)goto _LL352;_tmp4B1=_tmp472.f2;if(_tmp4B1 <= (
void*)4)goto _LL352;if(*((int*)_tmp4B1)!= 5)goto _LL352;_tmp4B2=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4B1)->f2;if((int)_tmp4B2 != 0)goto _LL352;_LL351: goto _LL353;_LL352: _tmp4B3=
_tmp472.f1;if(_tmp4B3 <= (void*)4)goto _LL354;if(*((int*)_tmp4B3)!= 18)goto _LL354;
_tmp4B4=_tmp472.f2;if(_tmp4B4 <= (void*)4)goto _LL354;if(*((int*)_tmp4B4)!= 5)goto
_LL354;_tmp4B5=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4B4)->f2;if((int)
_tmp4B5 != 0)goto _LL354;_LL353: goto _LL355;_LL354: _tmp4B6=_tmp472.f1;if(_tmp4B6 <= (
void*)4)goto _LL356;if(*((int*)_tmp4B6)!= 14)goto _LL356;_tmp4B7=_tmp472.f2;if(
_tmp4B7 <= (void*)4)goto _LL356;if(*((int*)_tmp4B7)!= 5)goto _LL356;_tmp4B8=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp4B7)->f2;if((int)_tmp4B8 != 0)goto _LL356;
_LL355: return 1;_LL356:;_LL357: return 0;_LL327:;}}int Cyc_Tcutil_coerce_list(struct
Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es);int Cyc_Tcutil_coerce_list(
struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){{struct _RegionHandle
_tmp4B9=_new_region("r");struct _RegionHandle*r=& _tmp4B9;_push_region(r);{struct
Cyc_Core_Opt*max_arith_type=0;{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)
el->hd)->topt))->v);if(Cyc_Tcutil_is_arithmetic_type(t1)){if(max_arith_type == 0
 || Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v)){struct Cyc_Core_Opt*
_tmpD6E;max_arith_type=((_tmpD6E=_region_malloc(r,sizeof(*_tmpD6E)),((_tmpD6E->v=(
void*)t1,_tmpD6E))));}}}}if(max_arith_type != 0){if(!Cyc_Tcutil_unify(t,(void*)
max_arith_type->v)){int _tmp4BB=0;_npop_handler(0);return _tmp4BB;}}};_pop_region(
r);}{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){if(!Cyc_Tcutil_coerce_assign(
te,(struct Cyc_Absyn_Exp*)el->hd,t)){{const char*_tmpD73;void*_tmpD72[2];struct Cyc_String_pa_struct
_tmpD71;struct Cyc_String_pa_struct _tmpD70;(_tmpD70.tag=0,((_tmpD70.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))->v)),((_tmpD71.tag=
0,((_tmpD71.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t)),((_tmpD72[0]=& _tmpD71,((_tmpD72[1]=& _tmpD70,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
el->hd)->loc,((_tmpD73="type mismatch: expecting %s but found %s",
_tag_dynforward(_tmpD73,sizeof(char),_get_zero_arr_size_char(_tmpD73,41)))),
_tag_dynforward(_tmpD72,sizeof(void*),2)))))))))))));}return 0;}}}return 1;}int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_to_bool(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(!Cyc_Tcutil_coerce_sint_typ(te,
e)){void*_tmp4C0=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL359: if(_tmp4C0 <= (void*)4)goto _LL35B;if(*((int*)_tmp4C0)!= 4)goto _LL35B;
_LL35A: Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,(void*)3);goto _LL358;
_LL35B:;_LL35C: return 0;_LL358:;}return 1;}int Cyc_Tcutil_is_integral_type(void*t);
int Cyc_Tcutil_is_integral_type(void*t){void*_tmp4C1=Cyc_Tcutil_compress(t);
_LL35E: if(_tmp4C1 <= (void*)4)goto _LL368;if(*((int*)_tmp4C1)!= 5)goto _LL360;
_LL35F: goto _LL361;_LL360: if(*((int*)_tmp4C1)!= 14)goto _LL362;_LL361: goto _LL363;
_LL362: if(*((int*)_tmp4C1)!= 18)goto _LL364;_LL363: goto _LL365;_LL364: if(*((int*)
_tmp4C1)!= 12)goto _LL366;_LL365: goto _LL367;_LL366: if(*((int*)_tmp4C1)!= 13)goto
_LL368;_LL367: return 1;_LL368:;_LL369: return 0;_LL35D:;}int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_uint_typ(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_uint_typ)){const
char*_tmpD76;void*_tmpD75;(_tmpD75=0,Cyc_Tcutil_warn(e->loc,((_tmpD76="integral size mismatch; conversion supplied",
_tag_dynforward(_tmpD76,sizeof(char),_get_zero_arr_size_char(_tmpD76,44)))),
_tag_dynforward(_tmpD75,sizeof(void*),0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,(
void*)1);return 1;}return 0;}int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,Cyc_Absyn_sint_typ))return 1;if(Cyc_Tcutil_is_integral_type((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)){if(Cyc_Tcutil_will_lose_precision((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ)){const
char*_tmpD79;void*_tmpD78;(_tmpD78=0,Cyc_Tcutil_warn(e->loc,((_tmpD79="integral size mismatch; conversion supplied",
_tag_dynforward(_tmpD79,sizeof(char),_get_zero_arr_size_char(_tmpD79,44)))),
_tag_dynforward(_tmpD78,sizeof(void*),0)));}Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,(
void*)1);return 1;}return 0;}struct _tuple13{union Cyc_Absyn_Constraint_union f1;
union Cyc_Absyn_Constraint_union f2;};int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Position_Segment*loc,void*t1,void*t2);int Cyc_Tcutil_silent_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){t1=Cyc_Tcutil_compress(
t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0 _tmpD7A;struct _tuple0 _tmp4C7=(
_tmpD7A.f1=t1,((_tmpD7A.f2=t2,_tmpD7A)));void*_tmp4C8;struct Cyc_Absyn_PtrInfo
_tmp4C9;void*_tmp4CA;struct Cyc_Absyn_PtrInfo _tmp4CB;void*_tmp4CC;struct Cyc_Absyn_TunionInfo
_tmp4CD;union Cyc_Absyn_TunionInfoU_union _tmp4CE;struct Cyc_Absyn_Tuniondecl**
_tmp4CF;struct Cyc_Absyn_Tuniondecl*_tmp4D0;struct Cyc_List_List*_tmp4D1;struct Cyc_Core_Opt*
_tmp4D2;struct Cyc_Core_Opt _tmp4D3;void*_tmp4D4;void*_tmp4D5;struct Cyc_Absyn_TunionInfo
_tmp4D6;union Cyc_Absyn_TunionInfoU_union _tmp4D7;struct Cyc_Absyn_Tuniondecl**
_tmp4D8;struct Cyc_Absyn_Tuniondecl*_tmp4D9;struct Cyc_List_List*_tmp4DA;struct Cyc_Core_Opt*
_tmp4DB;struct Cyc_Core_Opt _tmp4DC;void*_tmp4DD;void*_tmp4DE;struct Cyc_Absyn_ArrayInfo
_tmp4DF;void*_tmp4E0;struct Cyc_Absyn_Tqual _tmp4E1;struct Cyc_Absyn_Exp*_tmp4E2;
struct Cyc_Absyn_Conref*_tmp4E3;void*_tmp4E4;struct Cyc_Absyn_ArrayInfo _tmp4E5;
void*_tmp4E6;struct Cyc_Absyn_Tqual _tmp4E7;struct Cyc_Absyn_Exp*_tmp4E8;struct Cyc_Absyn_Conref*
_tmp4E9;void*_tmp4EA;struct Cyc_Absyn_TunionFieldInfo _tmp4EB;union Cyc_Absyn_TunionFieldInfoU_union
_tmp4EC;struct Cyc_Absyn_Tuniondecl*_tmp4ED;struct Cyc_Absyn_Tunionfield*_tmp4EE;
struct Cyc_List_List*_tmp4EF;void*_tmp4F0;struct Cyc_Absyn_TunionInfo _tmp4F1;union
Cyc_Absyn_TunionInfoU_union _tmp4F2;struct Cyc_Absyn_Tuniondecl**_tmp4F3;struct Cyc_Absyn_Tuniondecl*
_tmp4F4;struct Cyc_List_List*_tmp4F5;void*_tmp4F6;struct Cyc_Absyn_PtrInfo _tmp4F7;
void*_tmp4F8;struct Cyc_Absyn_Tqual _tmp4F9;struct Cyc_Absyn_PtrAtts _tmp4FA;void*
_tmp4FB;struct Cyc_Absyn_Conref*_tmp4FC;struct Cyc_Absyn_Conref*_tmp4FD;struct Cyc_Absyn_Conref*
_tmp4FE;void*_tmp4FF;struct Cyc_Absyn_TunionInfo _tmp500;union Cyc_Absyn_TunionInfoU_union
_tmp501;struct Cyc_Absyn_Tuniondecl**_tmp502;struct Cyc_Absyn_Tuniondecl*_tmp503;
struct Cyc_List_List*_tmp504;struct Cyc_Core_Opt*_tmp505;void*_tmp506;void*_tmp507;
void*_tmp508;void*_tmp509;void*_tmp50A;void*_tmp50B;void*_tmp50C;void*_tmp50D;
_LL36B: _tmp4C8=_tmp4C7.f1;if(_tmp4C8 <= (void*)4)goto _LL36D;if(*((int*)_tmp4C8)!= 
4)goto _LL36D;_tmp4C9=((struct Cyc_Absyn_PointerType_struct*)_tmp4C8)->f1;_tmp4CA=
_tmp4C7.f2;if(_tmp4CA <= (void*)4)goto _LL36D;if(*((int*)_tmp4CA)!= 4)goto _LL36D;
_tmp4CB=((struct Cyc_Absyn_PointerType_struct*)_tmp4CA)->f1;_LL36C: {int okay=1;
if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp4C9.ptr_atts).nullable,(
_tmp4CB.ptr_atts).nullable)){union Cyc_Absyn_Constraint_union _tmp50E=(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)((_tmp4C9.ptr_atts).nullable))->v;
int _tmp50F;_LL37E: if((_tmp50E.Eq_constr).tag != 0)goto _LL380;_tmp50F=(_tmp50E.Eq_constr).f1;
_LL37F: okay=!_tmp50F;goto _LL37D;_LL380:;_LL381: {const char*_tmpD7D;void*_tmpD7C;(
_tmpD7C=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmpD7D="silent_castable conref not eq",_tag_dynforward(_tmpD7D,sizeof(char),
_get_zero_arr_size_char(_tmpD7D,30)))),_tag_dynforward(_tmpD7C,sizeof(void*),0)));}
_LL37D:;}if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,(_tmp4C9.ptr_atts).bounds,(
_tmp4CB.ptr_atts).bounds)){struct _tuple13 _tmpD7E;struct _tuple13 _tmp513=(_tmpD7E.f1=(
Cyc_Absyn_compress_conref((_tmp4C9.ptr_atts).bounds))->v,((_tmpD7E.f2=(Cyc_Absyn_compress_conref((
_tmp4CB.ptr_atts).bounds))->v,_tmpD7E)));union Cyc_Absyn_Constraint_union _tmp514;
void*_tmp515;union Cyc_Absyn_Constraint_union _tmp516;void*_tmp517;union Cyc_Absyn_Constraint_union
_tmp518;_LL383: _tmp514=_tmp513.f1;if(((_tmp513.f1).Eq_constr).tag != 0)goto _LL385;
_tmp515=(_tmp514.Eq_constr).f1;_tmp516=_tmp513.f2;if(((_tmp513.f2).Eq_constr).tag
!= 0)goto _LL385;_tmp517=(_tmp516.Eq_constr).f1;_LL384:{struct _tuple0 _tmpD7F;
struct _tuple0 _tmp51A=(_tmpD7F.f1=_tmp515,((_tmpD7F.f2=_tmp517,_tmpD7F)));void*
_tmp51B;void*_tmp51C;void*_tmp51D;void*_tmp51E;void*_tmp51F;void*_tmp520;void*
_tmp521;void*_tmp522;void*_tmp523;void*_tmp524;void*_tmp525;void*_tmp526;void*
_tmp527;struct Cyc_Absyn_Exp*_tmp528;void*_tmp529;struct Cyc_Absyn_Exp*_tmp52A;
void*_tmp52B;void*_tmp52C;struct Cyc_Absyn_Exp*_tmp52D;void*_tmp52E;void*_tmp52F;
struct Cyc_Absyn_Exp*_tmp530;void*_tmp531;void*_tmp532;void*_tmp533;void*_tmp534;
void*_tmp535;struct Cyc_Absyn_Exp*_tmp536;void*_tmp537;void*_tmp538;void*_tmp539;
void*_tmp53A;_LL38A: _tmp51B=_tmp51A.f1;if(_tmp51B <= (void*)2)goto _LL38C;if(*((
int*)_tmp51B)!= 0)goto _LL38C;_tmp51C=_tmp51A.f2;if((int)_tmp51C != 0)goto _LL38C;
_LL38B: goto _LL38D;_LL38C: _tmp51D=_tmp51A.f1;if(_tmp51D <= (void*)2)goto _LL38E;if(*((
int*)_tmp51D)!= 0)goto _LL38E;_tmp51E=_tmp51A.f2;if((int)_tmp51E != 1)goto _LL38E;
_LL38D: goto _LL38F;_LL38E: _tmp51F=_tmp51A.f1;if((int)_tmp51F != 1)goto _LL390;
_tmp520=_tmp51A.f2;if((int)_tmp520 != 1)goto _LL390;_LL38F: goto _LL391;_LL390:
_tmp521=_tmp51A.f1;if((int)_tmp521 != 0)goto _LL392;_tmp522=_tmp51A.f2;if((int)
_tmp522 != 1)goto _LL392;_LL391: goto _LL393;_LL392: _tmp523=_tmp51A.f1;if((int)
_tmp523 != 0)goto _LL394;_tmp524=_tmp51A.f2;if((int)_tmp524 != 0)goto _LL394;_LL393:
okay=1;goto _LL389;_LL394: _tmp525=_tmp51A.f1;if((int)_tmp525 != 1)goto _LL396;
_tmp526=_tmp51A.f2;if((int)_tmp526 != 0)goto _LL396;_LL395: okay=0;goto _LL389;
_LL396: _tmp527=_tmp51A.f1;if(_tmp527 <= (void*)2)goto _LL398;if(*((int*)_tmp527)!= 
0)goto _LL398;_tmp528=((struct Cyc_Absyn_Upper_b_struct*)_tmp527)->f1;_tmp529=
_tmp51A.f2;if(_tmp529 <= (void*)2)goto _LL398;if(*((int*)_tmp529)!= 0)goto _LL398;
_tmp52A=((struct Cyc_Absyn_Upper_b_struct*)_tmp529)->f1;_LL397: okay=okay  && Cyc_Evexp_lte_const_exp(
_tmp52A,_tmp528);if(!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,(_tmp4CB.ptr_atts).zero_term)){const char*_tmpD82;void*_tmpD81;(_tmpD81=0,Cyc_Tcutil_warn(
loc,((_tmpD82="implicit cast to shorter array",_tag_dynforward(_tmpD82,sizeof(
char),_get_zero_arr_size_char(_tmpD82,31)))),_tag_dynforward(_tmpD81,sizeof(void*),
0)));}goto _LL389;_LL398: _tmp52B=_tmp51A.f1;if((int)_tmp52B != 0)goto _LL39A;
_tmp52C=_tmp51A.f2;if(_tmp52C <= (void*)2)goto _LL39A;if(*((int*)_tmp52C)!= 0)goto
_LL39A;_tmp52D=((struct Cyc_Absyn_Upper_b_struct*)_tmp52C)->f1;_LL399: _tmp530=
_tmp52D;goto _LL39B;_LL39A: _tmp52E=_tmp51A.f1;if((int)_tmp52E != 1)goto _LL39C;
_tmp52F=_tmp51A.f2;if(_tmp52F <= (void*)2)goto _LL39C;if(*((int*)_tmp52F)!= 0)goto
_LL39C;_tmp530=((struct Cyc_Absyn_Upper_b_struct*)_tmp52F)->f1;_LL39B: if(((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp4C9.ptr_atts).zero_term)
 && Cyc_Tcutil_is_bound_one((_tmp4CB.ptr_atts).bounds))goto _LL389;okay=0;goto
_LL389;_LL39C: _tmp531=_tmp51A.f1;if(_tmp531 <= (void*)2)goto _LL39E;if(*((int*)
_tmp531)!= 1)goto _LL39E;_tmp532=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp531)->f1;_tmp533=_tmp51A.f2;if(_tmp533 <= (void*)2)goto _LL39E;if(*((int*)
_tmp533)!= 1)goto _LL39E;_tmp534=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp533)->f1;_LL39D: if(!Cyc_Tcutil_unify(_tmp532,_tmp534)){struct _tuple0 _tmpD83;
struct _tuple0 _tmp53E=(_tmpD83.f1=Cyc_Tcutil_compress(_tmp532),((_tmpD83.f2=Cyc_Tcutil_compress(
_tmp534),_tmpD83)));void*_tmp53F;int _tmp540;void*_tmp541;int _tmp542;_LL3A5:
_tmp53F=_tmp53E.f1;if(_tmp53F <= (void*)4)goto _LL3A7;if(*((int*)_tmp53F)!= 19)
goto _LL3A7;_tmp540=((struct Cyc_Absyn_TypeInt_struct*)_tmp53F)->f1;_tmp541=
_tmp53E.f2;if(_tmp541 <= (void*)4)goto _LL3A7;if(*((int*)_tmp541)!= 19)goto _LL3A7;
_tmp542=((struct Cyc_Absyn_TypeInt_struct*)_tmp541)->f1;_LL3A6: if(_tmp540 < 
_tmp542)okay=0;goto _LL3A4;_LL3A7:;_LL3A8: okay=0;goto _LL3A4;_LL3A4:;}goto _LL389;
_LL39E: _tmp535=_tmp51A.f1;if(_tmp535 <= (void*)2)goto _LL3A0;if(*((int*)_tmp535)!= 
0)goto _LL3A0;_tmp536=((struct Cyc_Absyn_Upper_b_struct*)_tmp535)->f1;_tmp537=
_tmp51A.f2;if(_tmp537 <= (void*)2)goto _LL3A0;if(*((int*)_tmp537)!= 1)goto _LL3A0;
_tmp538=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp537)->f1;_LL39F: {
unsigned int _tmp544;int _tmp545;struct _tuple7 _tmp543=Cyc_Evexp_eval_const_uint_exp(
_tmp536);_tmp544=_tmp543.f1;_tmp545=_tmp543.f2;if(!_tmp545){okay=0;goto _LL389;}{
void*_tmp546=Cyc_Tcutil_compress(_tmp538);int _tmp547;_LL3AA: if(_tmp546 <= (void*)
4)goto _LL3AC;if(*((int*)_tmp546)!= 19)goto _LL3AC;_tmp547=((struct Cyc_Absyn_TypeInt_struct*)
_tmp546)->f1;_LL3AB: if(_tmp544 < _tmp547)okay=0;goto _LL3A9;_LL3AC:;_LL3AD: okay=0;
goto _LL3A9;_LL3A9:;}goto _LL389;}_LL3A0: _tmp539=_tmp51A.f1;if(_tmp539 <= (void*)2)
goto _LL3A2;if(*((int*)_tmp539)!= 1)goto _LL3A2;_LL3A1: goto _LL3A3;_LL3A2: _tmp53A=
_tmp51A.f2;if(_tmp53A <= (void*)2)goto _LL389;if(*((int*)_tmp53A)!= 1)goto _LL389;
_LL3A3: okay=0;goto _LL389;_LL389:;}goto _LL382;_LL385: _tmp518=_tmp513.f1;if(((
_tmp513.f1).No_constr).tag != 2)goto _LL387;_LL386: okay=0;goto _LL382;_LL387:;
_LL388: {const char*_tmpD86;void*_tmpD85;(_tmpD85=0,((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmpD86="silent_castable conrefs didn't unify",
_tag_dynforward(_tmpD86,sizeof(char),_get_zero_arr_size_char(_tmpD86,37)))),
_tag_dynforward(_tmpD85,sizeof(void*),0)));}_LL382:;}okay=okay  && Cyc_Tcutil_unify((
void*)_tmp4C9.elt_typ,(void*)_tmp4CB.elt_typ);okay=okay  && (Cyc_Tcutil_unify((
void*)(_tmp4C9.ptr_atts).rgn,(void*)(_tmp4CB.ptr_atts).rgn) || Cyc_Tcenv_region_outlives(
te,(void*)(_tmp4C9.ptr_atts).rgn,(void*)(_tmp4CB.ptr_atts).rgn));okay=okay  && (!(
_tmp4C9.elt_tq).real_const  || (_tmp4CB.elt_tq).real_const);okay=okay  && (((int(*)(
int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,(_tmp4C9.ptr_atts).zero_term,(_tmp4CB.ptr_atts).zero_term) || ((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,(_tmp4C9.ptr_atts).zero_term)
 && (_tmp4CB.elt_tq).real_const);return okay;}_LL36D: _tmp4CC=_tmp4C7.f1;if(
_tmp4CC <= (void*)4)goto _LL36F;if(*((int*)_tmp4CC)!= 2)goto _LL36F;_tmp4CD=((
struct Cyc_Absyn_TunionType_struct*)_tmp4CC)->f1;_tmp4CE=_tmp4CD.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp4CC)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL36F;_tmp4CF=(_tmp4CE.KnownTunion).f1;_tmp4D0=*_tmp4CF;_tmp4D1=_tmp4CD.targs;
_tmp4D2=_tmp4CD.rgn;if(_tmp4D2 == 0)goto _LL36F;_tmp4D3=*_tmp4D2;_tmp4D4=(void*)
_tmp4D3.v;_tmp4D5=_tmp4C7.f2;if(_tmp4D5 <= (void*)4)goto _LL36F;if(*((int*)_tmp4D5)
!= 2)goto _LL36F;_tmp4D6=((struct Cyc_Absyn_TunionType_struct*)_tmp4D5)->f1;
_tmp4D7=_tmp4D6.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)_tmp4D5)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LL36F;_tmp4D8=(_tmp4D7.KnownTunion).f1;_tmp4D9=*_tmp4D8;_tmp4DA=
_tmp4D6.targs;_tmp4DB=_tmp4D6.rgn;if(_tmp4DB == 0)goto _LL36F;_tmp4DC=*_tmp4DB;
_tmp4DD=(void*)_tmp4DC.v;_LL36E: if(_tmp4D0 != _tmp4D9  || !Cyc_Tcenv_region_outlives(
te,_tmp4D4,_tmp4DD))return 0;for(0;_tmp4D1 != 0  && _tmp4DA != 0;(_tmp4D1=_tmp4D1->tl,
_tmp4DA=_tmp4DA->tl)){if(!Cyc_Tcutil_unify((void*)_tmp4D1->hd,(void*)_tmp4DA->hd))
return 0;}if(_tmp4D1 != 0  || _tmp4DA != 0)return 0;return 1;_LL36F: _tmp4DE=_tmp4C7.f1;
if(_tmp4DE <= (void*)4)goto _LL371;if(*((int*)_tmp4DE)!= 7)goto _LL371;_tmp4DF=((
struct Cyc_Absyn_ArrayType_struct*)_tmp4DE)->f1;_tmp4E0=(void*)_tmp4DF.elt_type;
_tmp4E1=_tmp4DF.tq;_tmp4E2=_tmp4DF.num_elts;_tmp4E3=_tmp4DF.zero_term;_tmp4E4=
_tmp4C7.f2;if(_tmp4E4 <= (void*)4)goto _LL371;if(*((int*)_tmp4E4)!= 7)goto _LL371;
_tmp4E5=((struct Cyc_Absyn_ArrayType_struct*)_tmp4E4)->f1;_tmp4E6=(void*)_tmp4E5.elt_type;
_tmp4E7=_tmp4E5.tq;_tmp4E8=_tmp4E5.num_elts;_tmp4E9=_tmp4E5.zero_term;_LL370: {
int okay;okay=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4E3,_tmp4E9) && ((_tmp4E2 != 0
 && _tmp4E8 != 0) && Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_tmp4E2,(
struct Cyc_Absyn_Exp*)_tmp4E8));return(okay  && Cyc_Tcutil_unify(_tmp4E0,_tmp4E6))
 && (!_tmp4E1.real_const  || _tmp4E7.real_const);}_LL371: _tmp4EA=_tmp4C7.f1;if(
_tmp4EA <= (void*)4)goto _LL373;if(*((int*)_tmp4EA)!= 3)goto _LL373;_tmp4EB=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp4EA)->f1;_tmp4EC=_tmp4EB.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp4EA)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL373;_tmp4ED=(_tmp4EC.KnownTunionfield).f1;_tmp4EE=(_tmp4EC.KnownTunionfield).f2;
_tmp4EF=_tmp4EB.targs;_tmp4F0=_tmp4C7.f2;if(_tmp4F0 <= (void*)4)goto _LL373;if(*((
int*)_tmp4F0)!= 2)goto _LL373;_tmp4F1=((struct Cyc_Absyn_TunionType_struct*)
_tmp4F0)->f1;_tmp4F2=_tmp4F1.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp4F0)->f1).tunion_info).KnownTunion).tag != 1)goto _LL373;_tmp4F3=(_tmp4F2.KnownTunion).f1;
_tmp4F4=*_tmp4F3;_tmp4F5=_tmp4F1.targs;_LL372: if((_tmp4ED == _tmp4F4  || Cyc_Absyn_qvar_cmp(
_tmp4ED->name,_tmp4F4->name)== 0) && (_tmp4EE->typs == 0  || _tmp4ED->is_flat)){
for(0;_tmp4EF != 0  && _tmp4F5 != 0;(_tmp4EF=_tmp4EF->tl,_tmp4F5=_tmp4F5->tl)){if(!
Cyc_Tcutil_unify((void*)_tmp4EF->hd,(void*)_tmp4F5->hd))break;}if(_tmp4EF == 0
 && _tmp4F5 == 0)return 1;}return 0;_LL373: _tmp4F6=_tmp4C7.f1;if(_tmp4F6 <= (void*)4)
goto _LL375;if(*((int*)_tmp4F6)!= 4)goto _LL375;_tmp4F7=((struct Cyc_Absyn_PointerType_struct*)
_tmp4F6)->f1;_tmp4F8=(void*)_tmp4F7.elt_typ;_tmp4F9=_tmp4F7.elt_tq;_tmp4FA=
_tmp4F7.ptr_atts;_tmp4FB=(void*)_tmp4FA.rgn;_tmp4FC=_tmp4FA.nullable;_tmp4FD=
_tmp4FA.bounds;_tmp4FE=_tmp4FA.zero_term;_tmp4FF=_tmp4C7.f2;if(_tmp4FF <= (void*)
4)goto _LL375;if(*((int*)_tmp4FF)!= 2)goto _LL375;_tmp500=((struct Cyc_Absyn_TunionType_struct*)
_tmp4FF)->f1;_tmp501=_tmp500.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp4FF)->f1).tunion_info).KnownTunion).tag != 1)goto _LL375;_tmp502=(_tmp501.KnownTunion).f1;
_tmp503=*_tmp502;_tmp504=_tmp500.targs;_tmp505=_tmp500.rgn;if(!(!_tmp503->is_flat))
goto _LL375;_LL374:{void*_tmp54A=Cyc_Tcutil_compress(_tmp4F8);struct Cyc_Absyn_TunionFieldInfo
_tmp54B;union Cyc_Absyn_TunionFieldInfoU_union _tmp54C;struct Cyc_Absyn_Tuniondecl*
_tmp54D;struct Cyc_Absyn_Tunionfield*_tmp54E;struct Cyc_List_List*_tmp54F;_LL3AF:
if(_tmp54A <= (void*)4)goto _LL3B1;if(*((int*)_tmp54A)!= 3)goto _LL3B1;_tmp54B=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp54A)->f1;_tmp54C=_tmp54B.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp54A)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL3B1;_tmp54D=(_tmp54C.KnownTunionfield).f1;_tmp54E=(_tmp54C.KnownTunionfield).f2;
_tmp54F=_tmp54B.targs;_LL3B0: if(!Cyc_Tcutil_unify(_tmp4FB,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp505))->v) && !Cyc_Tcenv_region_outlives(te,_tmp4FB,(void*)
_tmp505->v))return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4FC,Cyc_Absyn_false_conref))
return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,_tmp4FD,Cyc_Absyn_bounds_one_conref))
return 0;if(!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*
y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp4FE,Cyc_Absyn_false_conref))
return 0;if(Cyc_Absyn_qvar_cmp(_tmp503->name,_tmp54D->name)== 0  && _tmp54E->typs
!= 0){int okay=1;for(0;_tmp54F != 0  && _tmp504 != 0;(_tmp54F=_tmp54F->tl,_tmp504=
_tmp504->tl)){if(!Cyc_Tcutil_unify((void*)_tmp54F->hd,(void*)_tmp504->hd)){okay=
0;break;}}if((!okay  || _tmp54F != 0) || _tmp504 != 0)return 0;return 1;}goto _LL3AE;
_LL3B1:;_LL3B2: goto _LL3AE;_LL3AE:;}return 0;_LL375: _tmp506=_tmp4C7.f1;if(_tmp506
<= (void*)4)goto _LL377;if(*((int*)_tmp506)!= 14)goto _LL377;_tmp507=_tmp4C7.f2;
if(_tmp507 <= (void*)4)goto _LL377;if(*((int*)_tmp507)!= 5)goto _LL377;_tmp508=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp507)->f2;if((int)_tmp508 != 2)goto
_LL377;_LL376: goto _LL378;_LL377: _tmp509=_tmp4C7.f1;if(_tmp509 <= (void*)4)goto
_LL379;if(*((int*)_tmp509)!= 14)goto _LL379;_tmp50A=_tmp4C7.f2;if(_tmp50A <= (void*)
4)goto _LL379;if(*((int*)_tmp50A)!= 5)goto _LL379;_tmp50B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp50A)->f2;if((int)_tmp50B != 3)goto _LL379;_LL378: return 1;_LL379: _tmp50C=
_tmp4C7.f1;if(_tmp50C <= (void*)4)goto _LL37B;if(*((int*)_tmp50C)!= 18)goto _LL37B;
_tmp50D=_tmp4C7.f2;if(_tmp50D <= (void*)4)goto _LL37B;if(*((int*)_tmp50D)!= 5)goto
_LL37B;_LL37A: return 0;_LL37B:;_LL37C: return Cyc_Tcutil_unify(t1,t2);_LL36A:;}}int
Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_pointer_type(void*t){void*
_tmp550=Cyc_Tcutil_compress(t);_LL3B4: if(_tmp550 <= (void*)4)goto _LL3B6;if(*((int*)
_tmp550)!= 4)goto _LL3B6;_LL3B5: return 1;_LL3B6:;_LL3B7: return 0;_LL3B3:;}int Cyc_Tcutil_is_pointer_or_boxed(
void*t,int*is_dynforward_ptr,int*is_dyneither_ptr);int Cyc_Tcutil_is_pointer_or_boxed(
void*t,int*is_dynforward_ptr,int*is_dyneither_ptr){void*_tmp551=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp552;struct Cyc_Absyn_PtrAtts _tmp553;struct Cyc_Absyn_Conref*
_tmp554;_LL3B9: if(_tmp551 <= (void*)4)goto _LL3BB;if(*((int*)_tmp551)!= 4)goto
_LL3BB;_tmp552=((struct Cyc_Absyn_PointerType_struct*)_tmp551)->f1;_tmp553=
_tmp552.ptr_atts;_tmp554=_tmp553.bounds;_LL3BA:*is_dynforward_ptr=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp554)== (void*)0;*is_dyneither_ptr=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp554)== (void*)1;return 1;_LL3BB:;_LL3BC: return Cyc_Tcutil_typ_kind(
t)== (void*)2;_LL3B8:;}int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp555=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp556;
int _tmp557;union Cyc_Absyn_Cnst_union _tmp558;char _tmp559;union Cyc_Absyn_Cnst_union
_tmp55A;short _tmp55B;union Cyc_Absyn_Cnst_union _tmp55C;long long _tmp55D;void*
_tmp55E;struct Cyc_Absyn_Exp*_tmp55F;_LL3BE: if(*((int*)_tmp555)!= 0)goto _LL3C0;
_tmp556=((struct Cyc_Absyn_Const_e_struct*)_tmp555)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp555)->f1).Int_c).tag != 2)goto _LL3C0;_tmp557=(_tmp556.Int_c).f2;if(_tmp557 != 
0)goto _LL3C0;_LL3BF: goto _LL3C1;_LL3C0: if(*((int*)_tmp555)!= 0)goto _LL3C2;_tmp558=((
struct Cyc_Absyn_Const_e_struct*)_tmp555)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp555)->f1).Char_c).tag != 0)goto _LL3C2;_tmp559=(_tmp558.Char_c).f2;if(_tmp559
!= 0)goto _LL3C2;_LL3C1: goto _LL3C3;_LL3C2: if(*((int*)_tmp555)!= 0)goto _LL3C4;
_tmp55A=((struct Cyc_Absyn_Const_e_struct*)_tmp555)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp555)->f1).Short_c).tag != 1)goto _LL3C4;_tmp55B=(_tmp55A.Short_c).f2;if(
_tmp55B != 0)goto _LL3C4;_LL3C3: goto _LL3C5;_LL3C4: if(*((int*)_tmp555)!= 0)goto
_LL3C6;_tmp55C=((struct Cyc_Absyn_Const_e_struct*)_tmp555)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp555)->f1).LongLong_c).tag != 3)goto _LL3C6;_tmp55D=(_tmp55C.LongLong_c).f2;if(
_tmp55D != 0)goto _LL3C6;_LL3C5: return 1;_LL3C6: if(*((int*)_tmp555)!= 15)goto _LL3C8;
_tmp55E=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp555)->f1;_tmp55F=((struct Cyc_Absyn_Cast_e_struct*)
_tmp555)->f2;_LL3C7: return Cyc_Tcutil_is_zero(_tmp55F) && Cyc_Tcutil_admits_zero(
_tmp55E);_LL3C8:;_LL3C9: return 0;_LL3BD:;}struct Cyc_Core_Opt Cyc_Tcutil_trk={(void*)((
void*)5)};struct Cyc_Core_Opt Cyc_Tcutil_urk={(void*)((void*)4)};struct Cyc_Core_Opt
Cyc_Tcutil_rk={(void*)((void*)3)};struct Cyc_Core_Opt Cyc_Tcutil_ak={(void*)((void*)
0)};struct Cyc_Core_Opt Cyc_Tcutil_bk={(void*)((void*)2)};struct Cyc_Core_Opt Cyc_Tcutil_mk={(
void*)((void*)1)};struct Cyc_Core_Opt Cyc_Tcutil_ik={(void*)((void*)7)};struct Cyc_Core_Opt
Cyc_Tcutil_ek={(void*)((void*)6)};struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(void*
k);struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(void*k){void*_tmp560=k;_LL3CB: if((
int)_tmp560 != 0)goto _LL3CD;_LL3CC: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ak;
_LL3CD: if((int)_tmp560 != 1)goto _LL3CF;_LL3CE: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_mk;
_LL3CF: if((int)_tmp560 != 2)goto _LL3D1;_LL3D0: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_bk;
_LL3D1: if((int)_tmp560 != 3)goto _LL3D3;_LL3D2: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_rk;
_LL3D3: if((int)_tmp560 != 4)goto _LL3D5;_LL3D4: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_urk;
_LL3D5: if((int)_tmp560 != 5)goto _LL3D7;_LL3D6: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_trk;
_LL3D7: if((int)_tmp560 != 6)goto _LL3D9;_LL3D8: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ek;
_LL3D9: if((int)_tmp560 != 7)goto _LL3CA;_LL3DA: return(struct Cyc_Core_Opt*)& Cyc_Tcutil_ik;
_LL3CA:;}static void**Cyc_Tcutil_kind_to_b(void*k);static void**Cyc_Tcutil_kind_to_b(
void*k){static struct Cyc_Absyn_Eq_kb_struct ab_v={0,(void*)((void*)0)};static
struct Cyc_Absyn_Eq_kb_struct mb_v={0,(void*)((void*)1)};static struct Cyc_Absyn_Eq_kb_struct
bb_v={0,(void*)((void*)2)};static struct Cyc_Absyn_Eq_kb_struct rb_v={0,(void*)((
void*)3)};static struct Cyc_Absyn_Eq_kb_struct ub_v={0,(void*)((void*)4)};static
struct Cyc_Absyn_Eq_kb_struct tb_v={0,(void*)((void*)5)};static struct Cyc_Absyn_Eq_kb_struct
eb_v={0,(void*)((void*)6)};static struct Cyc_Absyn_Eq_kb_struct ib_v={0,(void*)((
void*)7)};static void*ab=(void*)& ab_v;static void*mb=(void*)& mb_v;static void*bb=(
void*)& bb_v;static void*rb=(void*)& rb_v;static void*ub=(void*)& ub_v;static void*tb=(
void*)& tb_v;static void*eb=(void*)& eb_v;static void*ib=(void*)& ib_v;void*_tmp561=k;
_LL3DC: if((int)_tmp561 != 0)goto _LL3DE;_LL3DD: return& ab;_LL3DE: if((int)_tmp561 != 
1)goto _LL3E0;_LL3DF: return& mb;_LL3E0: if((int)_tmp561 != 2)goto _LL3E2;_LL3E1:
return& bb;_LL3E2: if((int)_tmp561 != 3)goto _LL3E4;_LL3E3: return& rb;_LL3E4: if((int)
_tmp561 != 4)goto _LL3E6;_LL3E5: return& ub;_LL3E6: if((int)_tmp561 != 5)goto _LL3E8;
_LL3E7: return& tb;_LL3E8: if((int)_tmp561 != 6)goto _LL3EA;_LL3E9: return& eb;_LL3EA:
if((int)_tmp561 != 7)goto _LL3DB;_LL3EB: return& ib;_LL3DB:;}void*Cyc_Tcutil_kind_to_bound(
void*k);void*Cyc_Tcutil_kind_to_bound(void*k){return*Cyc_Tcutil_kind_to_b(k);}
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(void*k);struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(
void*k){return(struct Cyc_Core_Opt*)Cyc_Tcutil_kind_to_b(k);}int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1);int Cyc_Tcutil_zero_to_null(
struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){if(Cyc_Tcutil_is_pointer_type(
t2) && Cyc_Tcutil_is_zero(e1)){{struct Cyc_Absyn_Const_e_struct _tmpD8C;union Cyc_Absyn_Cnst_union
_tmpD8B;struct Cyc_Absyn_Const_e_struct*_tmpD8A;(void*)(e1->r=(void*)((void*)((
_tmpD8A=_cycalloc(sizeof(*_tmpD8A)),((_tmpD8A[0]=((_tmpD8C.tag=0,((_tmpD8C.f1=(
union Cyc_Absyn_Cnst_union)(((_tmpD8B.Null_c).tag=6,_tmpD8B)),_tmpD8C)))),_tmpD8A))))));}{
struct Cyc_Core_Opt*_tmp56D=Cyc_Tcenv_lookup_opt_type_vars(te);struct Cyc_Absyn_PointerType_struct
_tmpD96;struct Cyc_Absyn_PtrAtts _tmpD95;struct Cyc_Absyn_PtrInfo _tmpD94;struct Cyc_Absyn_PointerType_struct*
_tmpD93;struct Cyc_Absyn_PointerType_struct*_tmp56E=(_tmpD93=_cycalloc(sizeof(*
_tmpD93)),((_tmpD93[0]=((_tmpD96.tag=4,((_tmpD96.f1=((_tmpD94.elt_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,_tmp56D),((_tmpD94.elt_tq=Cyc_Absyn_empty_tqual(
0),((_tmpD94.ptr_atts=((_tmpD95.rgn=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,_tmp56D),((_tmpD95.nullable=Cyc_Absyn_true_conref,((_tmpD95.bounds=
Cyc_Absyn_empty_conref(),((_tmpD95.zero_term=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)(),((
_tmpD95.ptrloc=0,_tmpD95)))))))))),_tmpD94)))))),_tmpD96)))),_tmpD93)));(void*)(((
struct Cyc_Core_Opt*)_check_null(e1->topt))->v=(void*)((void*)_tmp56E));return Cyc_Tcutil_coerce_arg(
te,e1,t2);}}return 0;}struct _dynforward_ptr Cyc_Tcutil_coercion2string(void*c);
struct _dynforward_ptr Cyc_Tcutil_coercion2string(void*c){void*_tmp573=c;_LL3ED:
if((int)_tmp573 != 0)goto _LL3EF;_LL3EE: {const char*_tmpD97;return(_tmpD97="unknown",
_tag_dynforward(_tmpD97,sizeof(char),_get_zero_arr_size_char(_tmpD97,8)));}
_LL3EF: if((int)_tmp573 != 1)goto _LL3F1;_LL3F0: {const char*_tmpD98;return(_tmpD98="no coercion",
_tag_dynforward(_tmpD98,sizeof(char),_get_zero_arr_size_char(_tmpD98,12)));}
_LL3F1: if((int)_tmp573 != 2)goto _LL3F3;_LL3F2: {const char*_tmpD99;return(_tmpD99="null check",
_tag_dynforward(_tmpD99,sizeof(char),_get_zero_arr_size_char(_tmpD99,11)));}
_LL3F3: if((int)_tmp573 != 3)goto _LL3EC;_LL3F4: {const char*_tmpD9A;return(_tmpD9A="other coercion",
_tag_dynforward(_tmpD9A,sizeof(char),_get_zero_arr_size_char(_tmpD9A,15)));}
_LL3EC:;}int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,
void*t2);int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,
void*t2){void*t1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
void*c;if(Cyc_Tcutil_unify(t1,t2))return 1;if(Cyc_Tcutil_is_arithmetic_type(t2)
 && Cyc_Tcutil_is_arithmetic_type(t1)){if(Cyc_Tcutil_will_lose_precision(t1,t2)){
const char*_tmpD9F;void*_tmpD9E[2];struct Cyc_String_pa_struct _tmpD9D;struct Cyc_String_pa_struct
_tmpD9C;(_tmpD9C.tag=0,((_tmpD9C.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_typ2string(t2)),((_tmpD9D.tag=0,((_tmpD9D.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t1)),((_tmpD9E[0]=&
_tmpD9D,((_tmpD9E[1]=& _tmpD9C,Cyc_Tcutil_warn(e->loc,((_tmpD9F="integral size mismatch; %s -> %s conversion supplied",
_tag_dynforward(_tmpD9F,sizeof(char),_get_zero_arr_size_char(_tmpD9F,53)))),
_tag_dynforward(_tmpD9E,sizeof(void*),2)))))))))))));}Cyc_Tcutil_unchecked_cast(
te,e,t2,(void*)1);return 1;}else{if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,(void*)3);return 1;}else{if(Cyc_Tcutil_zero_to_null(
te,t2,e))return 1;else{if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= (void*)0){
Cyc_Tcutil_unchecked_cast(te,e,t2,c);if(c != (void*)2){const char*_tmpDA4;void*
_tmpDA3[2];struct Cyc_String_pa_struct _tmpDA2;struct Cyc_String_pa_struct _tmpDA1;(
_tmpDA1.tag=0,((_tmpDA1.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t2)),((_tmpDA2.tag=0,((_tmpDA2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmpDA3[0]=& _tmpDA2,((_tmpDA3[1]=& _tmpDA1,Cyc_Tcutil_warn(
e->loc,((_tmpDA4="implicit cast from %s to %s",_tag_dynforward(_tmpDA4,sizeof(
char),_get_zero_arr_size_char(_tmpDA4,28)))),_tag_dynforward(_tmpDA3,sizeof(void*),
2)))))))))))));}return 1;}else{return 0;}}}}}int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t);int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t){return Cyc_Tcutil_coerce_arg(te,e,t);}int Cyc_Tcutil_coerceable(
void*t);int Cyc_Tcutil_coerceable(void*t){void*_tmp580=Cyc_Tcutil_compress(t);
_LL3F6: if(_tmp580 <= (void*)4)goto _LL3F8;if(*((int*)_tmp580)!= 5)goto _LL3F8;
_LL3F7: goto _LL3F9;_LL3F8: if((int)_tmp580 != 1)goto _LL3FA;_LL3F9: goto _LL3FB;_LL3FA:
if(_tmp580 <= (void*)4)goto _LL3FC;if(*((int*)_tmp580)!= 6)goto _LL3FC;_LL3FB:
return 1;_LL3FC:;_LL3FD: return 0;_LL3F5:;}static struct _tuple5*Cyc_Tcutil_flatten_typ_f(
struct _tuple9*env,struct Cyc_Absyn_Aggrfield*x);static struct _tuple5*Cyc_Tcutil_flatten_typ_f(
struct _tuple9*env,struct Cyc_Absyn_Aggrfield*x){struct Cyc_List_List*_tmp582;
struct _RegionHandle*_tmp583;struct _tuple9 _tmp581=*env;_tmp582=_tmp581.f1;_tmp583=
_tmp581.f2;{struct _tuple5*_tmpDA5;return(_tmpDA5=_region_malloc(_tmp583,sizeof(*
_tmpDA5)),((_tmpDA5->f1=x->tq,((_tmpDA5->f2=Cyc_Tcutil_rsubstitute(_tmp583,
_tmp582,(void*)x->type),_tmpDA5)))));}}static struct _tuple5*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple5*x);static struct _tuple5*Cyc_Tcutil_rcopy_tqt(
struct _RegionHandle*r,struct _tuple5*x){struct Cyc_Absyn_Tqual _tmp586;void*_tmp587;
struct _tuple5 _tmp585=*x;_tmp586=_tmp585.f1;_tmp587=_tmp585.f2;{struct _tuple5*
_tmpDA6;return(_tmpDA6=_region_malloc(r,sizeof(*_tmpDA6)),((_tmpDA6->f1=_tmp586,((
_tmpDA6->f2=_tmp587,_tmpDA6)))));}}static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(
struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*t1);static struct Cyc_List_List*
Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*t1){t1=
Cyc_Tcutil_compress(t1);{void*_tmp589=t1;struct Cyc_List_List*_tmp58A;struct Cyc_Absyn_AggrInfo
_tmp58B;union Cyc_Absyn_AggrInfoU_union _tmp58C;struct Cyc_Absyn_Aggrdecl**_tmp58D;
struct Cyc_Absyn_Aggrdecl*_tmp58E;struct Cyc_List_List*_tmp58F;void*_tmp590;struct
Cyc_List_List*_tmp591;struct Cyc_Absyn_FnInfo _tmp592;_LL3FF: if((int)_tmp589 != 0)
goto _LL401;_LL400: return 0;_LL401: if(_tmp589 <= (void*)4)goto _LL409;if(*((int*)
_tmp589)!= 9)goto _LL403;_tmp58A=((struct Cyc_Absyn_TupleType_struct*)_tmp589)->f1;
_LL402: return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(
struct _RegionHandle*,struct _tuple5*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,r,_tmp58A);_LL403: if(*((int*)_tmp589)
!= 10)goto _LL405;_tmp58B=((struct Cyc_Absyn_AggrType_struct*)_tmp589)->f1;_tmp58C=
_tmp58B.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp589)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL405;_tmp58D=(_tmp58C.KnownAggr).f1;_tmp58E=*_tmp58D;_tmp58F=_tmp58B.targs;
_LL404: if((((void*)_tmp58E->kind == (void*)1  || _tmp58E->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp58E->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp58E->impl))->rgn_po != 0){struct _tuple5*_tmpDA9;struct Cyc_List_List*
_tmpDA8;return(_tmpDA8=_region_malloc(r,sizeof(*_tmpDA8)),((_tmpDA8->hd=((
_tmpDA9=_region_malloc(r,sizeof(*_tmpDA9)),((_tmpDA9->f1=Cyc_Absyn_empty_tqual(0),((
_tmpDA9->f2=t1,_tmpDA9)))))),((_tmpDA8->tl=0,_tmpDA8)))));}{struct Cyc_List_List*
_tmp595=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp58E->tvs,
_tmp58F);struct _tuple9 _tmpDAA;struct _tuple9 env=(_tmpDAA.f1=_tmp595,((_tmpDAA.f2=
r,_tmpDAA)));return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*
f)(struct _tuple9*,struct Cyc_Absyn_Aggrfield*),struct _tuple9*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp58E->impl))->fields);}_LL405: if(*((int*)_tmp589)!= 11)goto _LL407;
_tmp590=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp589)->f1;if((int)
_tmp590 != 0)goto _LL407;_tmp591=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp589)->f2;
_LL406: {struct _tuple9 _tmpDAB;struct _tuple9 env=(_tmpDAB.f1=0,((_tmpDAB.f2=r,
_tmpDAB)));return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*
f)(struct _tuple9*,struct Cyc_Absyn_Aggrfield*),struct _tuple9*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp591);}_LL407: if(*((int*)
_tmp589)!= 8)goto _LL409;_tmp592=((struct Cyc_Absyn_FnType_struct*)_tmp589)->f1;
_LL408: {struct _tuple5*_tmpDAE;struct Cyc_List_List*_tmpDAD;return(_tmpDAD=
_region_malloc(r,sizeof(*_tmpDAD)),((_tmpDAD->hd=((_tmpDAE=_region_malloc(r,
sizeof(*_tmpDAE)),((_tmpDAE->f1=Cyc_Absyn_const_tqual(0),((_tmpDAE->f2=t1,
_tmpDAE)))))),((_tmpDAD->tl=0,_tmpDAD)))));}_LL409:;_LL40A: {struct _tuple5*
_tmpDB1;struct Cyc_List_List*_tmpDB0;return(_tmpDB0=_region_malloc(r,sizeof(*
_tmpDB0)),((_tmpDB0->hd=((_tmpDB1=_region_malloc(r,sizeof(*_tmpDB1)),((_tmpDB1->f1=
Cyc_Absyn_empty_tqual(0),((_tmpDB1->f2=t1,_tmpDB1)))))),((_tmpDB0->tl=0,_tmpDB0)))));}
_LL3FE:;}}static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*
a2);static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*
a2){{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){void*_tmp59C=(void*)t->hd;
_LL40C: if((int)_tmp59C != 16)goto _LL40E;_LL40D: goto _LL40F;_LL40E: if((int)_tmp59C
!= 3)goto _LL410;_LL40F: goto _LL411;_LL410: if(_tmp59C <= (void*)17)goto _LL412;if(*((
int*)_tmp59C)!= 4)goto _LL412;_LL411: continue;_LL412:;_LL413: if(!Cyc_List_exists_c(
Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;_LL40B:;}}for(0;a2 != 0;a2=a2->tl){
if(!Cyc_List_exists_c(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}return 1;}
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,
struct Cyc_List_List*assume,void*t1,void*t2);static int Cyc_Tcutil_subtype(struct
Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return 1;{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){if(Cyc_Tcutil_unify(
t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);{struct _tuple0
_tmpDB2;struct _tuple0 _tmp59E=(_tmpDB2.f1=t1,((_tmpDB2.f2=t2,_tmpDB2)));void*
_tmp59F;struct Cyc_Absyn_PtrInfo _tmp5A0;void*_tmp5A1;struct Cyc_Absyn_Tqual _tmp5A2;
struct Cyc_Absyn_PtrAtts _tmp5A3;void*_tmp5A4;struct Cyc_Absyn_Conref*_tmp5A5;
struct Cyc_Absyn_Conref*_tmp5A6;struct Cyc_Absyn_Conref*_tmp5A7;void*_tmp5A8;
struct Cyc_Absyn_PtrInfo _tmp5A9;void*_tmp5AA;struct Cyc_Absyn_Tqual _tmp5AB;struct
Cyc_Absyn_PtrAtts _tmp5AC;void*_tmp5AD;struct Cyc_Absyn_Conref*_tmp5AE;struct Cyc_Absyn_Conref*
_tmp5AF;struct Cyc_Absyn_Conref*_tmp5B0;void*_tmp5B1;struct Cyc_Absyn_TunionInfo
_tmp5B2;union Cyc_Absyn_TunionInfoU_union _tmp5B3;struct Cyc_Absyn_Tuniondecl**
_tmp5B4;struct Cyc_Absyn_Tuniondecl*_tmp5B5;struct Cyc_List_List*_tmp5B6;struct Cyc_Core_Opt*
_tmp5B7;struct Cyc_Core_Opt _tmp5B8;void*_tmp5B9;void*_tmp5BA;struct Cyc_Absyn_TunionInfo
_tmp5BB;union Cyc_Absyn_TunionInfoU_union _tmp5BC;struct Cyc_Absyn_Tuniondecl**
_tmp5BD;struct Cyc_Absyn_Tuniondecl*_tmp5BE;struct Cyc_List_List*_tmp5BF;struct Cyc_Core_Opt*
_tmp5C0;struct Cyc_Core_Opt _tmp5C1;void*_tmp5C2;void*_tmp5C3;struct Cyc_Absyn_FnInfo
_tmp5C4;void*_tmp5C5;struct Cyc_Absyn_FnInfo _tmp5C6;_LL415: _tmp59F=_tmp59E.f1;if(
_tmp59F <= (void*)4)goto _LL417;if(*((int*)_tmp59F)!= 4)goto _LL417;_tmp5A0=((
struct Cyc_Absyn_PointerType_struct*)_tmp59F)->f1;_tmp5A1=(void*)_tmp5A0.elt_typ;
_tmp5A2=_tmp5A0.elt_tq;_tmp5A3=_tmp5A0.ptr_atts;_tmp5A4=(void*)_tmp5A3.rgn;
_tmp5A5=_tmp5A3.nullable;_tmp5A6=_tmp5A3.bounds;_tmp5A7=_tmp5A3.zero_term;
_tmp5A8=_tmp59E.f2;if(_tmp5A8 <= (void*)4)goto _LL417;if(*((int*)_tmp5A8)!= 4)goto
_LL417;_tmp5A9=((struct Cyc_Absyn_PointerType_struct*)_tmp5A8)->f1;_tmp5AA=(void*)
_tmp5A9.elt_typ;_tmp5AB=_tmp5A9.elt_tq;_tmp5AC=_tmp5A9.ptr_atts;_tmp5AD=(void*)
_tmp5AC.rgn;_tmp5AE=_tmp5AC.nullable;_tmp5AF=_tmp5AC.bounds;_tmp5B0=_tmp5AC.zero_term;
_LL416: if(_tmp5A2.real_const  && !_tmp5AB.real_const)return 0;if((!((int(*)(int(*
cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp5A5,_tmp5AE) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp5A5)) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp5AE))return 0;if((!((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,
struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5A7,
_tmp5B0) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp5A7)) && ((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp5B0))return 0;if(!Cyc_Tcutil_unify(_tmp5A4,_tmp5AD) && !Cyc_Tcenv_region_outlives(
te,_tmp5A4,_tmp5AD))return 0;if(!Cyc_Tcutil_unify_conrefs(Cyc_Tcutil_unify_it_bounds,
_tmp5A6,_tmp5AF)){struct _tuple0 _tmpDB3;struct _tuple0 _tmp5C8=(_tmpDB3.f1=Cyc_Absyn_conref_val(
_tmp5A6),((_tmpDB3.f2=Cyc_Absyn_conref_val(_tmp5AF),_tmpDB3)));void*_tmp5C9;void*
_tmp5CA;void*_tmp5CB;void*_tmp5CC;void*_tmp5CD;struct Cyc_Absyn_Exp*_tmp5CE;void*
_tmp5CF;struct Cyc_Absyn_Exp*_tmp5D0;_LL41E: _tmp5C9=_tmp5C8.f1;if(_tmp5C9 <= (void*)
2)goto _LL420;if(*((int*)_tmp5C9)!= 0)goto _LL420;_tmp5CA=_tmp5C8.f2;if((int)
_tmp5CA != 0)goto _LL420;_LL41F: goto _LL41D;_LL420: _tmp5CB=_tmp5C8.f1;if(_tmp5CB <= (
void*)2)goto _LL422;if(*((int*)_tmp5CB)!= 0)goto _LL422;_tmp5CC=_tmp5C8.f2;if((int)
_tmp5CC != 1)goto _LL422;_LL421: goto _LL41D;_LL422: _tmp5CD=_tmp5C8.f1;if(_tmp5CD <= (
void*)2)goto _LL424;if(*((int*)_tmp5CD)!= 0)goto _LL424;_tmp5CE=((struct Cyc_Absyn_Upper_b_struct*)
_tmp5CD)->f1;_tmp5CF=_tmp5C8.f2;if(_tmp5CF <= (void*)2)goto _LL424;if(*((int*)
_tmp5CF)!= 0)goto _LL424;_tmp5D0=((struct Cyc_Absyn_Upper_b_struct*)_tmp5CF)->f1;
_LL423: if(!Cyc_Evexp_lte_const_exp(_tmp5D0,_tmp5CE))return 0;goto _LL41D;_LL424:;
_LL425: return 0;_LL41D:;}{struct _tuple0*_tmpDB6;struct Cyc_List_List*_tmpDB5;
return Cyc_Tcutil_ptrsubtype(te,((_tmpDB5=_cycalloc(sizeof(*_tmpDB5)),((_tmpDB5->hd=((
_tmpDB6=_cycalloc(sizeof(*_tmpDB6)),((_tmpDB6->f1=t1,((_tmpDB6->f2=t2,_tmpDB6)))))),((
_tmpDB5->tl=assume,_tmpDB5)))))),_tmp5A1,_tmp5AA);}_LL417: _tmp5B1=_tmp59E.f1;if(
_tmp5B1 <= (void*)4)goto _LL419;if(*((int*)_tmp5B1)!= 2)goto _LL419;_tmp5B2=((
struct Cyc_Absyn_TunionType_struct*)_tmp5B1)->f1;_tmp5B3=_tmp5B2.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp5B1)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL419;_tmp5B4=(_tmp5B3.KnownTunion).f1;_tmp5B5=*_tmp5B4;_tmp5B6=_tmp5B2.targs;
_tmp5B7=_tmp5B2.rgn;if(_tmp5B7 == 0)goto _LL419;_tmp5B8=*_tmp5B7;_tmp5B9=(void*)
_tmp5B8.v;_tmp5BA=_tmp59E.f2;if(_tmp5BA <= (void*)4)goto _LL419;if(*((int*)_tmp5BA)
!= 2)goto _LL419;_tmp5BB=((struct Cyc_Absyn_TunionType_struct*)_tmp5BA)->f1;
_tmp5BC=_tmp5BB.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)_tmp5BA)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LL419;_tmp5BD=(_tmp5BC.KnownTunion).f1;_tmp5BE=*_tmp5BD;_tmp5BF=
_tmp5BB.targs;_tmp5C0=_tmp5BB.rgn;if(_tmp5C0 == 0)goto _LL419;_tmp5C1=*_tmp5C0;
_tmp5C2=(void*)_tmp5C1.v;_LL418: if(_tmp5B5 != _tmp5BE  || !Cyc_Tcenv_region_outlives(
te,_tmp5B9,_tmp5C2))return 0;for(0;_tmp5B6 != 0  && _tmp5BF != 0;(_tmp5B6=_tmp5B6->tl,
_tmp5BF=_tmp5BF->tl)){if(!Cyc_Tcutil_unify((void*)_tmp5B6->hd,(void*)_tmp5BF->hd))
return 0;}if(_tmp5B6 != 0  || _tmp5BF != 0)return 0;return 1;_LL419: _tmp5C3=_tmp59E.f1;
if(_tmp5C3 <= (void*)4)goto _LL41B;if(*((int*)_tmp5C3)!= 8)goto _LL41B;_tmp5C4=((
struct Cyc_Absyn_FnType_struct*)_tmp5C3)->f1;_tmp5C5=_tmp59E.f2;if(_tmp5C5 <= (
void*)4)goto _LL41B;if(*((int*)_tmp5C5)!= 8)goto _LL41B;_tmp5C6=((struct Cyc_Absyn_FnType_struct*)
_tmp5C5)->f1;_LL41A: if(_tmp5C4.tvars != 0  || _tmp5C6.tvars != 0){struct Cyc_List_List*
_tmp5D3=_tmp5C4.tvars;struct Cyc_List_List*_tmp5D4=_tmp5C6.tvars;if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp5D3)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp5D4))return 0;{struct _RegionHandle _tmp5D5=_new_region("r");
struct _RegionHandle*r=& _tmp5D5;_push_region(r);{struct Cyc_List_List*inst=0;
while(_tmp5D3 != 0){if(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp5D3->hd)!= 
Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(
_tmp5D4))->hd)){int _tmp5D6=0;_npop_handler(0);return _tmp5D6;}{struct _tuple8*
_tmpDC0;struct Cyc_Absyn_VarType_struct _tmpDBF;struct Cyc_Absyn_VarType_struct*
_tmpDBE;struct Cyc_List_List*_tmpDBD;inst=((_tmpDBD=_region_malloc(r,sizeof(*
_tmpDBD)),((_tmpDBD->hd=((_tmpDC0=_region_malloc(r,sizeof(*_tmpDC0)),((_tmpDC0->f1=(
struct Cyc_Absyn_Tvar*)_tmp5D4->hd,((_tmpDC0->f2=(void*)((_tmpDBE=_cycalloc(
sizeof(*_tmpDBE)),((_tmpDBE[0]=((_tmpDBF.tag=1,((_tmpDBF.f1=(struct Cyc_Absyn_Tvar*)
_tmp5D3->hd,_tmpDBF)))),_tmpDBE)))),_tmpDC0)))))),((_tmpDBD->tl=inst,_tmpDBD))))));}
_tmp5D3=_tmp5D3->tl;_tmp5D4=_tmp5D4->tl;}if(inst != 0){_tmp5C4.tvars=0;_tmp5C6.tvars=
0;{struct Cyc_Absyn_FnType_struct _tmpDC6;struct Cyc_Absyn_FnType_struct*_tmpDC5;
struct Cyc_Absyn_FnType_struct _tmpDC3;struct Cyc_Absyn_FnType_struct*_tmpDC2;int
_tmp5DF=Cyc_Tcutil_subtype(te,assume,(void*)((_tmpDC2=_cycalloc(sizeof(*_tmpDC2)),((
_tmpDC2[0]=((_tmpDC3.tag=8,((_tmpDC3.f1=_tmp5C4,_tmpDC3)))),_tmpDC2)))),(void*)((
_tmpDC5=_cycalloc(sizeof(*_tmpDC5)),((_tmpDC5[0]=((_tmpDC6.tag=8,((_tmpDC6.f1=
_tmp5C6,_tmpDC6)))),_tmpDC5)))));_npop_handler(0);return _tmp5DF;}}};_pop_region(
r);}}if(!Cyc_Tcutil_subtype(te,assume,(void*)_tmp5C4.ret_typ,(void*)_tmp5C6.ret_typ))
return 0;{struct Cyc_List_List*_tmp5E0=_tmp5C4.args;struct Cyc_List_List*_tmp5E1=
_tmp5C6.args;if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5E0)!= ((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5E1))return 0;for(0;_tmp5E0 != 
0;(_tmp5E0=_tmp5E0->tl,_tmp5E1=_tmp5E1->tl)){struct Cyc_Absyn_Tqual _tmp5E3;void*
_tmp5E4;struct _tuple3 _tmp5E2=*((struct _tuple3*)_tmp5E0->hd);_tmp5E3=_tmp5E2.f2;
_tmp5E4=_tmp5E2.f3;{struct Cyc_Absyn_Tqual _tmp5E6;void*_tmp5E7;struct _tuple3
_tmp5E5=*((struct _tuple3*)((struct Cyc_List_List*)_check_null(_tmp5E1))->hd);
_tmp5E6=_tmp5E5.f2;_tmp5E7=_tmp5E5.f3;if(_tmp5E6.real_const  && !_tmp5E3.real_const
 || !Cyc_Tcutil_subtype(te,assume,_tmp5E7,_tmp5E4))return 0;}}if(_tmp5C4.c_varargs
!= _tmp5C6.c_varargs)return 0;if(_tmp5C4.cyc_varargs != 0  && _tmp5C6.cyc_varargs != 
0){struct Cyc_Absyn_VarargInfo _tmp5E8=*_tmp5C4.cyc_varargs;struct Cyc_Absyn_VarargInfo
_tmp5E9=*_tmp5C6.cyc_varargs;if((_tmp5E9.tq).real_const  && !(_tmp5E8.tq).real_const
 || !Cyc_Tcutil_subtype(te,assume,(void*)_tmp5E9.type,(void*)_tmp5E8.type))
return 0;}else{if(_tmp5C4.cyc_varargs != 0  || _tmp5C6.cyc_varargs != 0)return 0;}if(
!Cyc_Tcutil_subset_effect(1,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp5C4.effect))->v,(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp5C6.effect))->v))return 0;if(!Cyc_Tcutil_sub_rgnpo(
_tmp5C4.rgn_po,_tmp5C6.rgn_po))return 0;if(!Cyc_Tcutil_sub_attributes(_tmp5C4.attributes,
_tmp5C6.attributes))return 0;return 1;}_LL41B:;_LL41C: return 0;_LL414:;}}static int
Cyc_Tcutil_isomorphic(void*t1,void*t2);static int Cyc_Tcutil_isomorphic(void*t1,
void*t2){struct _tuple0 _tmpDC7;struct _tuple0 _tmp5EB=(_tmpDC7.f1=Cyc_Tcutil_compress(
t1),((_tmpDC7.f2=Cyc_Tcutil_compress(t2),_tmpDC7)));void*_tmp5EC;void*_tmp5ED;
void*_tmp5EE;void*_tmp5EF;_LL427: _tmp5EC=_tmp5EB.f1;if(_tmp5EC <= (void*)4)goto
_LL429;if(*((int*)_tmp5EC)!= 5)goto _LL429;_tmp5ED=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp5EC)->f2;_tmp5EE=_tmp5EB.f2;if(_tmp5EE <= (void*)4)goto _LL429;if(*((int*)
_tmp5EE)!= 5)goto _LL429;_tmp5EF=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5EE)->f2;
_LL428: return(_tmp5ED == _tmp5EF  || _tmp5ED == (void*)2  && _tmp5EF == (void*)3) || 
_tmp5ED == (void*)3  && _tmp5EF == (void*)2;_LL429:;_LL42A: return 0;_LL426:;}static
int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*
t1,void*t2);static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
assume,void*t1,void*t2){struct _RegionHandle _tmp5F0=_new_region("temp");struct
_RegionHandle*temp=& _tmp5F0;_push_region(temp);{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(
temp,te,t1);struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,te,t2);for(0;
tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){if(tqs1 == 0){int _tmp5F1=0;_npop_handler(
0);return _tmp5F1;}{struct _tuple5 _tmp5F3;struct Cyc_Absyn_Tqual _tmp5F4;void*
_tmp5F5;struct _tuple5*_tmp5F2=(struct _tuple5*)tqs1->hd;_tmp5F3=*_tmp5F2;_tmp5F4=
_tmp5F3.f1;_tmp5F5=_tmp5F3.f2;{struct _tuple5 _tmp5F7;struct Cyc_Absyn_Tqual _tmp5F8;
void*_tmp5F9;struct _tuple5*_tmp5F6=(struct _tuple5*)tqs2->hd;_tmp5F7=*_tmp5F6;
_tmp5F8=_tmp5F7.f1;_tmp5F9=_tmp5F7.f2;if(_tmp5F8.real_const  && Cyc_Tcutil_subtype(
te,assume,_tmp5F5,_tmp5F9))continue;else{if(Cyc_Tcutil_unify(_tmp5F5,_tmp5F9))
continue;else{if(Cyc_Tcutil_isomorphic(_tmp5F5,_tmp5F9))continue;else{int _tmp5FA=
0;_npop_handler(0);return _tmp5FA;}}}}}}{int _tmp5FB=1;_npop_handler(0);return
_tmp5FB;}};_pop_region(temp);}static int Cyc_Tcutil_is_char_type(void*t);static int
Cyc_Tcutil_is_char_type(void*t){void*_tmp5FC=Cyc_Tcutil_compress(t);void*_tmp5FD;
_LL42C: if(_tmp5FC <= (void*)4)goto _LL42E;if(*((int*)_tmp5FC)!= 5)goto _LL42E;
_tmp5FD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp5FC)->f2;if((int)_tmp5FD != 
0)goto _LL42E;_LL42D: return 1;_LL42E:;_LL42F: return 0;_LL42B:;}void*Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2);void*Cyc_Tcutil_castable(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Position_Segment*loc,void*t1,void*t2){if(Cyc_Tcutil_unify(
t1,t2))return(void*)1;t1=Cyc_Tcutil_compress(t1);t2=Cyc_Tcutil_compress(t2);if(
t2 == (void*)0)return(void*)1;{void*_tmp5FE=t2;void*_tmp5FF;void*_tmp600;_LL431:
if(_tmp5FE <= (void*)4)goto _LL435;if(*((int*)_tmp5FE)!= 5)goto _LL433;_tmp5FF=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp5FE)->f2;if((int)_tmp5FF != 2)goto
_LL433;_LL432: goto _LL434;_LL433: if(*((int*)_tmp5FE)!= 5)goto _LL435;_tmp600=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp5FE)->f2;if((int)_tmp600 != 3)goto _LL435;
_LL434: if(Cyc_Tcutil_typ_kind(t1)== (void*)2)return(void*)1;goto _LL430;_LL435:;
_LL436: goto _LL430;_LL430:;}{void*_tmp601=t1;struct Cyc_Absyn_PtrInfo _tmp602;void*
_tmp603;struct Cyc_Absyn_Tqual _tmp604;struct Cyc_Absyn_PtrAtts _tmp605;void*_tmp606;
struct Cyc_Absyn_Conref*_tmp607;struct Cyc_Absyn_Conref*_tmp608;struct Cyc_Absyn_Conref*
_tmp609;struct Cyc_Absyn_ArrayInfo _tmp60A;void*_tmp60B;struct Cyc_Absyn_Tqual
_tmp60C;struct Cyc_Absyn_Exp*_tmp60D;struct Cyc_Absyn_Conref*_tmp60E;struct Cyc_Absyn_Enumdecl*
_tmp60F;_LL438: if(_tmp601 <= (void*)4)goto _LL440;if(*((int*)_tmp601)!= 4)goto
_LL43A;_tmp602=((struct Cyc_Absyn_PointerType_struct*)_tmp601)->f1;_tmp603=(void*)
_tmp602.elt_typ;_tmp604=_tmp602.elt_tq;_tmp605=_tmp602.ptr_atts;_tmp606=(void*)
_tmp605.rgn;_tmp607=_tmp605.nullable;_tmp608=_tmp605.bounds;_tmp609=_tmp605.zero_term;
_LL439:{void*_tmp610=t2;struct Cyc_Absyn_PtrInfo _tmp611;void*_tmp612;struct Cyc_Absyn_Tqual
_tmp613;struct Cyc_Absyn_PtrAtts _tmp614;void*_tmp615;struct Cyc_Absyn_Conref*
_tmp616;struct Cyc_Absyn_Conref*_tmp617;struct Cyc_Absyn_Conref*_tmp618;_LL447: if(
_tmp610 <= (void*)4)goto _LL449;if(*((int*)_tmp610)!= 4)goto _LL449;_tmp611=((
struct Cyc_Absyn_PointerType_struct*)_tmp610)->f1;_tmp612=(void*)_tmp611.elt_typ;
_tmp613=_tmp611.elt_tq;_tmp614=_tmp611.ptr_atts;_tmp615=(void*)_tmp614.rgn;
_tmp616=_tmp614.nullable;_tmp617=_tmp614.bounds;_tmp618=_tmp614.zero_term;_LL448: {
void*coercion=(void*)3;struct _tuple0*_tmpDCA;struct Cyc_List_List*_tmpDC9;struct
Cyc_List_List*_tmp619=(_tmpDC9=_cycalloc(sizeof(*_tmpDC9)),((_tmpDC9->hd=((
_tmpDCA=_cycalloc(sizeof(*_tmpDCA)),((_tmpDCA->f1=t1,((_tmpDCA->f2=t2,_tmpDCA)))))),((
_tmpDC9->tl=0,_tmpDC9)))));int _tmp61A=Cyc_Tcutil_ptrsubtype(te,_tmp619,_tmp603,
_tmp612) && (!_tmp604.real_const  || _tmp613.real_const);Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{int zeroterm_ok=((int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*
x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp609,
_tmp618) || !((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(_tmp618);
int _tmp61B=_tmp61A?0:((Cyc_Tcutil_bits_only(_tmp603) && Cyc_Tcutil_is_char_type(
_tmp612)) && !((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp618)) && (_tmp613.real_const  || !_tmp604.real_const);int bounds_ok=Cyc_Tcutil_unify_conrefs(
Cyc_Tcutil_unify_it_bounds,_tmp608,_tmp617);if(!bounds_ok  && !_tmp61B){struct
_tuple0 _tmpDCB;struct _tuple0 _tmp61D=(_tmpDCB.f1=Cyc_Absyn_conref_val(_tmp608),((
_tmpDCB.f2=Cyc_Absyn_conref_val(_tmp617),_tmpDCB)));void*_tmp61E;struct Cyc_Absyn_Exp*
_tmp61F;void*_tmp620;struct Cyc_Absyn_Exp*_tmp621;void*_tmp622;void*_tmp623;void*
_tmp624;void*_tmp625;void*_tmp626;void*_tmp627;_LL44C: _tmp61E=_tmp61D.f1;if(
_tmp61E <= (void*)2)goto _LL44E;if(*((int*)_tmp61E)!= 0)goto _LL44E;_tmp61F=((
struct Cyc_Absyn_Upper_b_struct*)_tmp61E)->f1;_tmp620=_tmp61D.f2;if(_tmp620 <= (
void*)2)goto _LL44E;if(*((int*)_tmp620)!= 0)goto _LL44E;_tmp621=((struct Cyc_Absyn_Upper_b_struct*)
_tmp620)->f1;_LL44D: if(Cyc_Evexp_lte_const_exp(_tmp621,_tmp61F))bounds_ok=1;goto
_LL44B;_LL44E: _tmp622=_tmp61D.f1;if(_tmp622 <= (void*)2)goto _LL450;if(*((int*)
_tmp622)!= 1)goto _LL450;_tmp623=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp622)->f1;_tmp624=_tmp61D.f2;if(_tmp624 <= (void*)2)goto _LL450;if(*((int*)
_tmp624)!= 1)goto _LL450;_tmp625=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp624)->f1;_LL44F: bounds_ok=Cyc_Tcutil_unify(_tmp623,_tmp625);goto _LL44B;
_LL450: _tmp626=_tmp61D.f1;if(_tmp626 <= (void*)2)goto _LL452;if(*((int*)_tmp626)!= 
1)goto _LL452;_LL451: goto _LL453;_LL452: _tmp627=_tmp61D.f2;if(_tmp627 <= (void*)2)
goto _LL454;if(*((int*)_tmp627)!= 1)goto _LL454;_LL453: bounds_ok=0;goto _LL44B;
_LL454:;_LL455: bounds_ok=1;goto _LL44B;_LL44B:;}if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp607) && !((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp616))coercion=(void*)2;if(((bounds_ok  && zeroterm_ok)
 && (_tmp61A  || _tmp61B)) && (Cyc_Tcutil_unify(_tmp606,_tmp615) || Cyc_Tcenv_region_outlives(
te,_tmp606,_tmp615)))return coercion;else{return(void*)0;}}}_LL449:;_LL44A: goto
_LL446;_LL446:;}return(void*)0;_LL43A: if(*((int*)_tmp601)!= 7)goto _LL43C;_tmp60A=((
struct Cyc_Absyn_ArrayType_struct*)_tmp601)->f1;_tmp60B=(void*)_tmp60A.elt_type;
_tmp60C=_tmp60A.tq;_tmp60D=_tmp60A.num_elts;_tmp60E=_tmp60A.zero_term;_LL43B:{
void*_tmp62A=t2;struct Cyc_Absyn_ArrayInfo _tmp62B;void*_tmp62C;struct Cyc_Absyn_Tqual
_tmp62D;struct Cyc_Absyn_Exp*_tmp62E;struct Cyc_Absyn_Conref*_tmp62F;_LL457: if(
_tmp62A <= (void*)4)goto _LL459;if(*((int*)_tmp62A)!= 7)goto _LL459;_tmp62B=((
struct Cyc_Absyn_ArrayType_struct*)_tmp62A)->f1;_tmp62C=(void*)_tmp62B.elt_type;
_tmp62D=_tmp62B.tq;_tmp62E=_tmp62B.num_elts;_tmp62F=_tmp62B.zero_term;_LL458: {
int okay;okay=((_tmp60D != 0  && _tmp62E != 0) && ((int(*)(int(*cmp)(int,int),struct
Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp60E,_tmp62F)) && Cyc_Evexp_lte_const_exp((struct Cyc_Absyn_Exp*)_tmp62E,(
struct Cyc_Absyn_Exp*)_tmp60D);return(okay  && Cyc_Tcutil_unify(_tmp60B,_tmp62C))
 && (!_tmp60C.real_const  || _tmp62D.real_const)?(void*)3:(void*)0;}_LL459:;
_LL45A: return(void*)0;_LL456:;}return(void*)0;_LL43C: if(*((int*)_tmp601)!= 12)
goto _LL43E;_tmp60F=((struct Cyc_Absyn_EnumType_struct*)_tmp601)->f2;_LL43D:{void*
_tmp630=t2;struct Cyc_Absyn_Enumdecl*_tmp631;_LL45C: if(_tmp630 <= (void*)4)goto
_LL45E;if(*((int*)_tmp630)!= 12)goto _LL45E;_tmp631=((struct Cyc_Absyn_EnumType_struct*)
_tmp630)->f2;_LL45D: if((_tmp60F->fields != 0  && _tmp631->fields != 0) && ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp60F->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp631->fields))->v))
return(void*)1;goto _LL45B;_LL45E:;_LL45F: goto _LL45B;_LL45B:;}goto _LL43F;_LL43E:
if(*((int*)_tmp601)!= 5)goto _LL440;_LL43F: goto _LL441;_LL440: if((int)_tmp601 != 1)
goto _LL442;_LL441: goto _LL443;_LL442: if(_tmp601 <= (void*)4)goto _LL444;if(*((int*)
_tmp601)!= 6)goto _LL444;_LL443: return Cyc_Tcutil_coerceable(t2)?(void*)1:(void*)0;
_LL444:;_LL445: return(void*)0;_LL437:;}}void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t,void*c);void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e,void*t,void*c){if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v,t)){struct Cyc_Absyn_Exp*_tmp632=Cyc_Absyn_copy_exp(e);{
struct Cyc_Absyn_Cast_e_struct _tmpDCE;struct Cyc_Absyn_Cast_e_struct*_tmpDCD;(void*)(
e->r=(void*)((void*)((_tmpDCD=_cycalloc(sizeof(*_tmpDCD)),((_tmpDCD[0]=((_tmpDCE.tag=
15,((_tmpDCE.f1=(void*)t,((_tmpDCE.f2=_tmp632,((_tmpDCE.f3=0,((_tmpDCE.f4=(void*)
c,_tmpDCE)))))))))),_tmpDCD))))));}{struct Cyc_Core_Opt*_tmpDCF;e->topt=((_tmpDCF=
_cycalloc(sizeof(*_tmpDCF)),((_tmpDCF->v=(void*)t,_tmpDCF))));}}}int Cyc_Tcutil_is_integral(
struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){void*
_tmp636=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL461: if(_tmp636 <= (void*)4)goto _LL46D;if(*((int*)_tmp636)!= 5)goto _LL463;
_LL462: goto _LL464;_LL463: if(*((int*)_tmp636)!= 12)goto _LL465;_LL464: goto _LL466;
_LL465: if(*((int*)_tmp636)!= 13)goto _LL467;_LL466: goto _LL468;_LL467: if(*((int*)
_tmp636)!= 18)goto _LL469;_LL468: goto _LL46A;_LL469: if(*((int*)_tmp636)!= 14)goto
_LL46B;_LL46A: return 1;_LL46B: if(*((int*)_tmp636)!= 0)goto _LL46D;_LL46C: return Cyc_Tcutil_unify((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,Cyc_Absyn_sint_typ);_LL46D:;
_LL46E: return 0;_LL460:;}int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_numeric(
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_integral(e))return 1;{void*_tmp637=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LL470: if((int)_tmp637 != 1)
goto _LL472;_LL471: goto _LL473;_LL472: if(_tmp637 <= (void*)4)goto _LL474;if(*((int*)
_tmp637)!= 6)goto _LL474;_LL473: return 1;_LL474:;_LL475: return 0;_LL46F:;}}int Cyc_Tcutil_is_function_type(
void*t);int Cyc_Tcutil_is_function_type(void*t){void*_tmp638=Cyc_Tcutil_compress(
t);_LL477: if(_tmp638 <= (void*)4)goto _LL479;if(*((int*)_tmp638)!= 8)goto _LL479;
_LL478: return 1;_LL479:;_LL47A: return 0;_LL476:;}void*Cyc_Tcutil_max_arithmetic_type(
void*t1,void*t2);void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){struct
_tuple0 _tmpDD0;struct _tuple0 _tmp63A=(_tmpDD0.f1=t1,((_tmpDD0.f2=t2,_tmpDD0)));
void*_tmp63B;int _tmp63C;void*_tmp63D;int _tmp63E;void*_tmp63F;void*_tmp640;void*
_tmp641;void*_tmp642;void*_tmp643;void*_tmp644;void*_tmp645;void*_tmp646;void*
_tmp647;void*_tmp648;void*_tmp649;void*_tmp64A;void*_tmp64B;void*_tmp64C;void*
_tmp64D;void*_tmp64E;void*_tmp64F;void*_tmp650;void*_tmp651;void*_tmp652;void*
_tmp653;void*_tmp654;void*_tmp655;void*_tmp656;void*_tmp657;void*_tmp658;void*
_tmp659;void*_tmp65A;void*_tmp65B;void*_tmp65C;_LL47C: _tmp63B=_tmp63A.f1;if(
_tmp63B <= (void*)4)goto _LL47E;if(*((int*)_tmp63B)!= 6)goto _LL47E;_tmp63C=((
struct Cyc_Absyn_DoubleType_struct*)_tmp63B)->f1;_tmp63D=_tmp63A.f2;if(_tmp63D <= (
void*)4)goto _LL47E;if(*((int*)_tmp63D)!= 6)goto _LL47E;_tmp63E=((struct Cyc_Absyn_DoubleType_struct*)
_tmp63D)->f1;_LL47D: if(_tmp63C)return t1;else{return t2;}_LL47E: _tmp63F=_tmp63A.f1;
if(_tmp63F <= (void*)4)goto _LL480;if(*((int*)_tmp63F)!= 6)goto _LL480;_LL47F:
return t1;_LL480: _tmp640=_tmp63A.f2;if(_tmp640 <= (void*)4)goto _LL482;if(*((int*)
_tmp640)!= 6)goto _LL482;_LL481: return t2;_LL482: _tmp641=_tmp63A.f1;if((int)
_tmp641 != 1)goto _LL484;_LL483: goto _LL485;_LL484: _tmp642=_tmp63A.f2;if((int)
_tmp642 != 1)goto _LL486;_LL485: return(void*)1;_LL486: _tmp643=_tmp63A.f1;if(
_tmp643 <= (void*)4)goto _LL488;if(*((int*)_tmp643)!= 5)goto _LL488;_tmp644=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp643)->f1;if((int)_tmp644 != 1)goto _LL488;
_tmp645=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp643)->f2;if((int)_tmp645 != 
4)goto _LL488;_LL487: goto _LL489;_LL488: _tmp646=_tmp63A.f2;if(_tmp646 <= (void*)4)
goto _LL48A;if(*((int*)_tmp646)!= 5)goto _LL48A;_tmp647=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp646)->f1;if((int)_tmp647 != 1)goto _LL48A;_tmp648=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp646)->f2;if((int)_tmp648 != 4)goto _LL48A;_LL489: return Cyc_Absyn_ulonglong_typ;
_LL48A: _tmp649=_tmp63A.f1;if(_tmp649 <= (void*)4)goto _LL48C;if(*((int*)_tmp649)!= 
5)goto _LL48C;_tmp64A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp649)->f2;if((
int)_tmp64A != 4)goto _LL48C;_LL48B: goto _LL48D;_LL48C: _tmp64B=_tmp63A.f2;if(
_tmp64B <= (void*)4)goto _LL48E;if(*((int*)_tmp64B)!= 5)goto _LL48E;_tmp64C=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp64B)->f2;if((int)_tmp64C != 4)goto _LL48E;
_LL48D: return Cyc_Absyn_slonglong_typ;_LL48E: _tmp64D=_tmp63A.f1;if(_tmp64D <= (
void*)4)goto _LL490;if(*((int*)_tmp64D)!= 5)goto _LL490;_tmp64E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp64D)->f1;if((int)_tmp64E != 1)goto _LL490;_tmp64F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp64D)->f2;if((int)_tmp64F != 3)goto _LL490;_LL48F: goto _LL491;_LL490: _tmp650=
_tmp63A.f2;if(_tmp650 <= (void*)4)goto _LL492;if(*((int*)_tmp650)!= 5)goto _LL492;
_tmp651=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp650)->f1;if((int)_tmp651 != 
1)goto _LL492;_tmp652=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp650)->f2;if((
int)_tmp652 != 3)goto _LL492;_LL491: return Cyc_Absyn_ulong_typ;_LL492: _tmp653=
_tmp63A.f1;if(_tmp653 <= (void*)4)goto _LL494;if(*((int*)_tmp653)!= 5)goto _LL494;
_tmp654=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp653)->f1;if((int)_tmp654 != 
1)goto _LL494;_tmp655=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp653)->f2;if((
int)_tmp655 != 2)goto _LL494;_LL493: goto _LL495;_LL494: _tmp656=_tmp63A.f2;if(
_tmp656 <= (void*)4)goto _LL496;if(*((int*)_tmp656)!= 5)goto _LL496;_tmp657=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp656)->f1;if((int)_tmp657 != 1)goto _LL496;
_tmp658=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp656)->f2;if((int)_tmp658 != 
2)goto _LL496;_LL495: return Cyc_Absyn_uint_typ;_LL496: _tmp659=_tmp63A.f1;if(
_tmp659 <= (void*)4)goto _LL498;if(*((int*)_tmp659)!= 5)goto _LL498;_tmp65A=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp659)->f2;if((int)_tmp65A != 3)goto _LL498;
_LL497: goto _LL499;_LL498: _tmp65B=_tmp63A.f2;if(_tmp65B <= (void*)4)goto _LL49A;if(*((
int*)_tmp65B)!= 5)goto _LL49A;_tmp65C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp65B)->f2;if((int)_tmp65C != 3)goto _LL49A;_LL499: return Cyc_Absyn_slong_typ;
_LL49A:;_LL49B: return Cyc_Absyn_sint_typ;_LL47B:;}void Cyc_Tcutil_check_contains_assign(
struct Cyc_Absyn_Exp*e);void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*
e){void*_tmp65D=(void*)e->r;struct Cyc_Core_Opt*_tmp65E;_LL49D: if(*((int*)_tmp65D)
!= 4)goto _LL49F;_tmp65E=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp65D)->f2;if(
_tmp65E != 0)goto _LL49F;_LL49E:{const char*_tmpDD3;void*_tmpDD2;(_tmpDD2=0,Cyc_Tcutil_warn(
e->loc,((_tmpDD3="assignment in test",_tag_dynforward(_tmpDD3,sizeof(char),
_get_zero_arr_size_char(_tmpDD3,19)))),_tag_dynforward(_tmpDD2,sizeof(void*),0)));}
goto _LL49C;_LL49F:;_LL4A0: goto _LL49C;_LL49C:;}static int Cyc_Tcutil_constrain_kinds(
void*c1,void*c2);static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){c1=Cyc_Absyn_compress_kb(
c1);c2=Cyc_Absyn_compress_kb(c2);{struct _tuple0 _tmpDD4;struct _tuple0 _tmp662=(
_tmpDD4.f1=c1,((_tmpDD4.f2=c2,_tmpDD4)));void*_tmp663;void*_tmp664;void*_tmp665;
void*_tmp666;void*_tmp667;struct Cyc_Core_Opt*_tmp668;struct Cyc_Core_Opt**_tmp669;
void*_tmp66A;struct Cyc_Core_Opt*_tmp66B;struct Cyc_Core_Opt**_tmp66C;void*_tmp66D;
struct Cyc_Core_Opt*_tmp66E;struct Cyc_Core_Opt**_tmp66F;void*_tmp670;void*_tmp671;
void*_tmp672;void*_tmp673;void*_tmp674;void*_tmp675;struct Cyc_Core_Opt*_tmp676;
struct Cyc_Core_Opt**_tmp677;void*_tmp678;void*_tmp679;struct Cyc_Core_Opt*_tmp67A;
struct Cyc_Core_Opt**_tmp67B;void*_tmp67C;void*_tmp67D;struct Cyc_Core_Opt*_tmp67E;
struct Cyc_Core_Opt**_tmp67F;void*_tmp680;_LL4A2: _tmp663=_tmp662.f1;if(*((int*)
_tmp663)!= 0)goto _LL4A4;_tmp664=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp663)->f1;
_tmp665=_tmp662.f2;if(*((int*)_tmp665)!= 0)goto _LL4A4;_tmp666=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp665)->f1;_LL4A3: return _tmp664 == _tmp666;_LL4A4: _tmp667=_tmp662.f2;if(*((int*)
_tmp667)!= 1)goto _LL4A6;_tmp668=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp667)->f1;
_tmp669=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp667)->f1;
_LL4A5:{struct Cyc_Core_Opt*_tmpDD5;*_tmp669=((_tmpDD5=_cycalloc(sizeof(*_tmpDD5)),((
_tmpDD5->v=(void*)c1,_tmpDD5))));}return 1;_LL4A6: _tmp66A=_tmp662.f1;if(*((int*)
_tmp66A)!= 1)goto _LL4A8;_tmp66B=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp66A)->f1;
_tmp66C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp66A)->f1;
_LL4A7:{struct Cyc_Core_Opt*_tmpDD6;*_tmp66C=((_tmpDD6=_cycalloc(sizeof(*_tmpDD6)),((
_tmpDD6->v=(void*)c2,_tmpDD6))));}return 1;_LL4A8: _tmp66D=_tmp662.f1;if(*((int*)
_tmp66D)!= 2)goto _LL4AA;_tmp66E=((struct Cyc_Absyn_Less_kb_struct*)_tmp66D)->f1;
_tmp66F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp66D)->f1;
_tmp670=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp66D)->f2;_tmp671=_tmp662.f2;
if(*((int*)_tmp671)!= 0)goto _LL4AA;_tmp672=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp671)->f1;_LL4A9: if(Cyc_Tcutil_kind_leq(_tmp672,_tmp670)){{struct Cyc_Core_Opt*
_tmpDD7;*_tmp66F=((_tmpDD7=_cycalloc(sizeof(*_tmpDD7)),((_tmpDD7->v=(void*)c2,
_tmpDD7))));}return 1;}else{return 0;}_LL4AA: _tmp673=_tmp662.f1;if(*((int*)_tmp673)
!= 0)goto _LL4AC;_tmp674=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp673)->f1;
_tmp675=_tmp662.f2;if(*((int*)_tmp675)!= 2)goto _LL4AC;_tmp676=((struct Cyc_Absyn_Less_kb_struct*)
_tmp675)->f1;_tmp677=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp675)->f1;_tmp678=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp675)->f2;
_LL4AB: if(Cyc_Tcutil_kind_leq(_tmp674,_tmp678)){{struct Cyc_Core_Opt*_tmpDD8;*
_tmp677=((_tmpDD8=_cycalloc(sizeof(*_tmpDD8)),((_tmpDD8->v=(void*)c1,_tmpDD8))));}
return 1;}else{return 0;}_LL4AC: _tmp679=_tmp662.f1;if(*((int*)_tmp679)!= 2)goto
_LL4A1;_tmp67A=((struct Cyc_Absyn_Less_kb_struct*)_tmp679)->f1;_tmp67B=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp679)->f1;_tmp67C=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp679)->f2;_tmp67D=_tmp662.f2;if(*((int*)_tmp67D)!= 2)goto _LL4A1;_tmp67E=((
struct Cyc_Absyn_Less_kb_struct*)_tmp67D)->f1;_tmp67F=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp67D)->f1;_tmp680=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp67D)->f2;_LL4AD: if(Cyc_Tcutil_kind_leq(_tmp67C,_tmp680)){{struct Cyc_Core_Opt*
_tmpDD9;*_tmp67F=((_tmpDD9=_cycalloc(sizeof(*_tmpDD9)),((_tmpDD9->v=(void*)c1,
_tmpDD9))));}return 1;}else{if(Cyc_Tcutil_kind_leq(_tmp680,_tmp67C)){{struct Cyc_Core_Opt*
_tmpDDA;*_tmp67B=((_tmpDDA=_cycalloc(sizeof(*_tmpDDA)),((_tmpDDA->v=(void*)c2,
_tmpDDA))));}return 1;}else{return 0;}}_LL4A1:;}}static int Cyc_Tcutil_tvar_id_counter=
0;int Cyc_Tcutil_new_tvar_id();int Cyc_Tcutil_new_tvar_id(){return Cyc_Tcutil_tvar_id_counter
++;}static int Cyc_Tcutil_tvar_counter=0;struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(
void*k);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){int i=Cyc_Tcutil_tvar_counter
++;const char*_tmpDDE;void*_tmpDDD[1];struct Cyc_Int_pa_struct _tmpDDC;struct
_dynforward_ptr s=(struct _dynforward_ptr)((_tmpDDC.tag=1,((_tmpDDC.f1=(
unsigned long)i,((_tmpDDD[0]=& _tmpDDC,Cyc_aprintf(((_tmpDDE="#%d",
_tag_dynforward(_tmpDDE,sizeof(char),_get_zero_arr_size_char(_tmpDDE,4)))),
_tag_dynforward(_tmpDDD,sizeof(void*),1))))))));struct _dynforward_ptr*_tmpDE1;
struct Cyc_Absyn_Tvar*_tmpDE0;return(_tmpDE0=_cycalloc(sizeof(*_tmpDE0)),((
_tmpDE0->name=((_tmpDE1=_cycalloc(sizeof(struct _dynforward_ptr)* 1),((_tmpDE1[0]=
s,_tmpDE1)))),((_tmpDE0->identity=- 1,((_tmpDE0->kind=(void*)k,_tmpDE0)))))));}
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t);int Cyc_Tcutil_is_temp_tvar(
struct Cyc_Absyn_Tvar*t){struct _dynforward_ptr _tmp68C=*t->name;return*((const char*)
_check_dynforward_subscript(_tmp68C,sizeof(char),0))== '#';}void Cyc_Tcutil_rewrite_temp_tvar(
struct Cyc_Absyn_Tvar*t);void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){{
const char*_tmpDE5;void*_tmpDE4[1];struct Cyc_String_pa_struct _tmpDE3;(_tmpDE3.tag=
0,((_tmpDE3.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*t->name),((
_tmpDE4[0]=& _tmpDE3,Cyc_printf(((_tmpDE5="%s",_tag_dynforward(_tmpDE5,sizeof(
char),_get_zero_arr_size_char(_tmpDE5,3)))),_tag_dynforward(_tmpDE4,sizeof(void*),
1)))))));}if(!Cyc_Tcutil_is_temp_tvar(t))return;{const char*_tmpDE6;struct
_dynforward_ptr _tmp690=Cyc_strconcat(((_tmpDE6="`",_tag_dynforward(_tmpDE6,
sizeof(char),_get_zero_arr_size_char(_tmpDE6,2)))),(struct _dynforward_ptr)*t->name);{
char _tmpDE9;char _tmpDE8;struct _dynforward_ptr _tmpDE7;(_tmpDE7=
_dynforward_ptr_plus(_tmp690,sizeof(char),1),((_tmpDE8=*((char*)
_check_dynforward_subscript(_tmpDE7,sizeof(char),0)),((_tmpDE9='t',((
_get_dynforward_size(_tmpDE7,sizeof(char))== 1  && (_tmpDE8 == '\000'  && _tmpDE9 != '\000')?
_throw_arraybounds(): 1,*((char*)_tmpDE7.curr)=_tmpDE9)))))));}{struct
_dynforward_ptr*_tmpDEA;t->name=((_tmpDEA=_cycalloc(sizeof(struct _dynforward_ptr)
* 1),((_tmpDEA[0]=(struct _dynforward_ptr)_tmp690,_tmpDEA))));}}}struct _tuple14{
struct _dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static struct _tuple3*
Cyc_Tcutil_fndecl2typ_f(struct _tuple14*x);static struct _tuple3*Cyc_Tcutil_fndecl2typ_f(
struct _tuple14*x){struct Cyc_Core_Opt*_tmpDED;struct _tuple3*_tmpDEC;return(
_tmpDEC=_cycalloc(sizeof(*_tmpDEC)),((_tmpDEC->f1=(struct Cyc_Core_Opt*)((_tmpDED=
_cycalloc(sizeof(*_tmpDED)),((_tmpDED->v=(*x).f1,_tmpDED)))),((_tmpDEC->f2=(*x).f2,((
_tmpDEC->f3=(*x).f3,_tmpDEC)))))));}void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*
fd);void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){if(fd->cached_typ == 0){
struct Cyc_List_List*_tmp698=0;{struct Cyc_List_List*atts=fd->attributes;for(0;
atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){struct Cyc_List_List*
_tmpDEE;_tmp698=((_tmpDEE=_cycalloc(sizeof(*_tmpDEE)),((_tmpDEE->hd=(void*)((
void*)atts->hd),((_tmpDEE->tl=_tmp698,_tmpDEE))))));}}}{struct Cyc_Absyn_FnType_struct
_tmpDF4;struct Cyc_Absyn_FnInfo _tmpDF3;struct Cyc_Absyn_FnType_struct*_tmpDF2;
return(void*)((_tmpDF2=_cycalloc(sizeof(*_tmpDF2)),((_tmpDF2[0]=((_tmpDF4.tag=8,((
_tmpDF4.f1=((_tmpDF3.tvars=fd->tvs,((_tmpDF3.effect=fd->effect,((_tmpDF3.ret_typ=(
void*)((void*)fd->ret_type),((_tmpDF3.args=((struct Cyc_List_List*(*)(struct
_tuple3*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,
fd->args),((_tmpDF3.c_varargs=fd->c_varargs,((_tmpDF3.cyc_varargs=fd->cyc_varargs,((
_tmpDF3.rgn_po=fd->rgn_po,((_tmpDF3.attributes=_tmp698,_tmpDF3)))))))))))))))),
_tmpDF4)))),_tmpDF2))));}}return(void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v;}
struct _tuple15{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static void*Cyc_Tcutil_fst_fdarg(
struct _tuple15*t);static void*Cyc_Tcutil_fst_fdarg(struct _tuple15*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple5*t);void*Cyc_Tcutil_snd_tqt(struct _tuple5*t){
return(*t).f2;}static struct _tuple5*Cyc_Tcutil_map2_tq(struct _tuple5*pr,void*t);
static struct _tuple5*Cyc_Tcutil_map2_tq(struct _tuple5*pr,void*t){struct _tuple5*
_tmpDF5;return(_tmpDF5=_cycalloc(sizeof(*_tmpDF5)),((_tmpDF5->f1=(*pr).f1,((
_tmpDF5->f2=t,_tmpDF5)))));}struct _tuple16{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;};struct _tuple17{struct _tuple16*f1;void*f2;};static struct _tuple17*Cyc_Tcutil_substitute_f1(
struct _RegionHandle*rgn,struct _tuple3*y);static struct _tuple17*Cyc_Tcutil_substitute_f1(
struct _RegionHandle*rgn,struct _tuple3*y){struct _tuple16*_tmpDF8;struct _tuple17*
_tmpDF7;return(_tmpDF7=_region_malloc(rgn,sizeof(*_tmpDF7)),((_tmpDF7->f1=((
_tmpDF8=_region_malloc(rgn,sizeof(*_tmpDF8)),((_tmpDF8->f1=(*y).f1,((_tmpDF8->f2=(*
y).f2,_tmpDF8)))))),((_tmpDF7->f2=(*y).f3,_tmpDF7)))));}static struct _tuple3*Cyc_Tcutil_substitute_f2(
struct _tuple17*w);static struct _tuple3*Cyc_Tcutil_substitute_f2(struct _tuple17*w){
struct _tuple16*_tmp6A1;void*_tmp6A2;struct _tuple17 _tmp6A0=*w;_tmp6A1=_tmp6A0.f1;
_tmp6A2=_tmp6A0.f2;{struct Cyc_Core_Opt*_tmp6A4;struct Cyc_Absyn_Tqual _tmp6A5;
struct _tuple16 _tmp6A3=*_tmp6A1;_tmp6A4=_tmp6A3.f1;_tmp6A5=_tmp6A3.f2;{struct
_tuple3*_tmpDF9;return(_tmpDF9=_cycalloc(sizeof(*_tmpDF9)),((_tmpDF9->f1=_tmp6A4,((
_tmpDF9->f2=_tmp6A5,((_tmpDF9->f3=_tmp6A2,_tmpDF9)))))));}}}static void*Cyc_Tcutil_field_type(
struct Cyc_Absyn_Aggrfield*f);static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*
f){return(void*)f->type;}static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t);static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_zip_field_type(
struct Cyc_Absyn_Aggrfield*f,void*t){struct Cyc_Absyn_Aggrfield*_tmpDFA;return(
_tmpDFA=_cycalloc(sizeof(*_tmpDFA)),((_tmpDFA->name=f->name,((_tmpDFA->tq=f->tq,((
_tmpDFA->type=(void*)t,((_tmpDFA->width=f->width,((_tmpDFA->attributes=f->attributes,
_tmpDFA)))))))))));}static struct Cyc_List_List*Cyc_Tcutil_substs(struct
_RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t);void*Cyc_Tcutil_rsubstitute(
struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){void*_tmp6A8=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_Tvar*_tmp6A9;struct Cyc_Absyn_AggrInfo _tmp6AA;union Cyc_Absyn_AggrInfoU_union
_tmp6AB;struct Cyc_List_List*_tmp6AC;struct Cyc_Absyn_TunionInfo _tmp6AD;union Cyc_Absyn_TunionInfoU_union
_tmp6AE;struct Cyc_List_List*_tmp6AF;struct Cyc_Core_Opt*_tmp6B0;struct Cyc_Absyn_TunionFieldInfo
_tmp6B1;union Cyc_Absyn_TunionFieldInfoU_union _tmp6B2;struct Cyc_List_List*_tmp6B3;
struct _tuple2*_tmp6B4;struct Cyc_List_List*_tmp6B5;struct Cyc_Absyn_Typedefdecl*
_tmp6B6;void**_tmp6B7;struct Cyc_Absyn_ArrayInfo _tmp6B8;void*_tmp6B9;struct Cyc_Absyn_Tqual
_tmp6BA;struct Cyc_Absyn_Exp*_tmp6BB;struct Cyc_Absyn_Conref*_tmp6BC;struct Cyc_Position_Segment*
_tmp6BD;struct Cyc_Absyn_PtrInfo _tmp6BE;void*_tmp6BF;struct Cyc_Absyn_Tqual _tmp6C0;
struct Cyc_Absyn_PtrAtts _tmp6C1;void*_tmp6C2;struct Cyc_Absyn_Conref*_tmp6C3;
struct Cyc_Absyn_Conref*_tmp6C4;struct Cyc_Absyn_Conref*_tmp6C5;struct Cyc_Absyn_FnInfo
_tmp6C6;struct Cyc_List_List*_tmp6C7;struct Cyc_Core_Opt*_tmp6C8;void*_tmp6C9;
struct Cyc_List_List*_tmp6CA;int _tmp6CB;struct Cyc_Absyn_VarargInfo*_tmp6CC;struct
Cyc_List_List*_tmp6CD;struct Cyc_List_List*_tmp6CE;struct Cyc_List_List*_tmp6CF;
void*_tmp6D0;struct Cyc_List_List*_tmp6D1;struct Cyc_Core_Opt*_tmp6D2;void*_tmp6D3;
void*_tmp6D4;void*_tmp6D5;void*_tmp6D6;void*_tmp6D7;void*_tmp6D8;void*_tmp6D9;
struct Cyc_List_List*_tmp6DA;_LL4AF: if(_tmp6A8 <= (void*)4)goto _LL4D3;if(*((int*)
_tmp6A8)!= 1)goto _LL4B1;_tmp6A9=((struct Cyc_Absyn_VarType_struct*)_tmp6A8)->f1;
_LL4B0: {struct _handler_cons _tmp6DB;_push_handler(& _tmp6DB);{int _tmp6DD=0;if(
setjmp(_tmp6DB.handler))_tmp6DD=1;if(!_tmp6DD){{void*_tmp6DE=((void*(*)(int(*cmp)(
struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*
x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp6A9);_npop_handler(0);return
_tmp6DE;};_pop_handler();}else{void*_tmp6DC=(void*)_exn_thrown;void*_tmp6E0=
_tmp6DC;_LL4E6: if(_tmp6E0 != Cyc_Core_Not_found)goto _LL4E8;_LL4E7: return t;_LL4E8:;
_LL4E9:(void)_throw(_tmp6E0);_LL4E5:;}}}_LL4B1: if(*((int*)_tmp6A8)!= 10)goto
_LL4B3;_tmp6AA=((struct Cyc_Absyn_AggrType_struct*)_tmp6A8)->f1;_tmp6AB=_tmp6AA.aggr_info;
_tmp6AC=_tmp6AA.targs;_LL4B2: {struct Cyc_List_List*_tmp6E1=Cyc_Tcutil_substs(rgn,
inst,_tmp6AC);struct Cyc_Absyn_AggrType_struct _tmpE00;struct Cyc_Absyn_AggrInfo
_tmpDFF;struct Cyc_Absyn_AggrType_struct*_tmpDFE;return _tmp6E1 == _tmp6AC?t:(void*)((
_tmpDFE=_cycalloc(sizeof(*_tmpDFE)),((_tmpDFE[0]=((_tmpE00.tag=10,((_tmpE00.f1=((
_tmpDFF.aggr_info=_tmp6AB,((_tmpDFF.targs=_tmp6E1,_tmpDFF)))),_tmpE00)))),
_tmpDFE))));}_LL4B3: if(*((int*)_tmp6A8)!= 2)goto _LL4B5;_tmp6AD=((struct Cyc_Absyn_TunionType_struct*)
_tmp6A8)->f1;_tmp6AE=_tmp6AD.tunion_info;_tmp6AF=_tmp6AD.targs;_tmp6B0=_tmp6AD.rgn;
_LL4B4: {struct Cyc_List_List*_tmp6E5=Cyc_Tcutil_substs(rgn,inst,_tmp6AF);struct
Cyc_Core_Opt*new_r;if((unsigned int)_tmp6B0){void*_tmp6E6=Cyc_Tcutil_rsubstitute(
rgn,inst,(void*)_tmp6B0->v);if(_tmp6E6 == (void*)_tmp6B0->v)new_r=_tmp6B0;else{
struct Cyc_Core_Opt*_tmpE01;new_r=((_tmpE01=_cycalloc(sizeof(*_tmpE01)),((_tmpE01->v=(
void*)_tmp6E6,_tmpE01))));}}else{new_r=_tmp6B0;}{struct Cyc_Absyn_TunionType_struct
_tmpE07;struct Cyc_Absyn_TunionInfo _tmpE06;struct Cyc_Absyn_TunionType_struct*
_tmpE05;return _tmp6E5 == _tmp6AF  && new_r == _tmp6B0?t:(void*)((_tmpE05=_cycalloc(
sizeof(*_tmpE05)),((_tmpE05[0]=((_tmpE07.tag=2,((_tmpE07.f1=((_tmpE06.tunion_info=
_tmp6AE,((_tmpE06.targs=_tmp6E5,((_tmpE06.rgn=new_r,_tmpE06)))))),_tmpE07)))),
_tmpE05))));}}_LL4B5: if(*((int*)_tmp6A8)!= 3)goto _LL4B7;_tmp6B1=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp6A8)->f1;_tmp6B2=_tmp6B1.field_info;_tmp6B3=_tmp6B1.targs;_LL4B6: {struct Cyc_List_List*
_tmp6EB=Cyc_Tcutil_substs(rgn,inst,_tmp6B3);struct Cyc_Absyn_TunionFieldType_struct
_tmpE0D;struct Cyc_Absyn_TunionFieldInfo _tmpE0C;struct Cyc_Absyn_TunionFieldType_struct*
_tmpE0B;return _tmp6EB == _tmp6B3?t:(void*)((_tmpE0B=_cycalloc(sizeof(*_tmpE0B)),((
_tmpE0B[0]=((_tmpE0D.tag=3,((_tmpE0D.f1=((_tmpE0C.field_info=_tmp6B2,((_tmpE0C.targs=
_tmp6EB,_tmpE0C)))),_tmpE0D)))),_tmpE0B))));}_LL4B7: if(*((int*)_tmp6A8)!= 17)
goto _LL4B9;_tmp6B4=((struct Cyc_Absyn_TypedefType_struct*)_tmp6A8)->f1;_tmp6B5=((
struct Cyc_Absyn_TypedefType_struct*)_tmp6A8)->f2;_tmp6B6=((struct Cyc_Absyn_TypedefType_struct*)
_tmp6A8)->f3;_tmp6B7=((struct Cyc_Absyn_TypedefType_struct*)_tmp6A8)->f4;_LL4B8: {
struct Cyc_List_List*_tmp6EF=Cyc_Tcutil_substs(rgn,inst,_tmp6B5);struct Cyc_Absyn_TypedefType_struct
_tmpE10;struct Cyc_Absyn_TypedefType_struct*_tmpE0F;return _tmp6EF == _tmp6B5?t:(
void*)((_tmpE0F=_cycalloc(sizeof(*_tmpE0F)),((_tmpE0F[0]=((_tmpE10.tag=17,((
_tmpE10.f1=_tmp6B4,((_tmpE10.f2=_tmp6EF,((_tmpE10.f3=_tmp6B6,((_tmpE10.f4=
_tmp6B7,_tmpE10)))))))))),_tmpE0F))));}_LL4B9: if(*((int*)_tmp6A8)!= 7)goto _LL4BB;
_tmp6B8=((struct Cyc_Absyn_ArrayType_struct*)_tmp6A8)->f1;_tmp6B9=(void*)_tmp6B8.elt_type;
_tmp6BA=_tmp6B8.tq;_tmp6BB=_tmp6B8.num_elts;_tmp6BC=_tmp6B8.zero_term;_tmp6BD=
_tmp6B8.zt_loc;_LL4BA: {void*_tmp6F2=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6B9);
struct Cyc_Absyn_ArrayType_struct _tmpE16;struct Cyc_Absyn_ArrayInfo _tmpE15;struct
Cyc_Absyn_ArrayType_struct*_tmpE14;return _tmp6F2 == _tmp6B9?t:(void*)((_tmpE14=
_cycalloc(sizeof(*_tmpE14)),((_tmpE14[0]=((_tmpE16.tag=7,((_tmpE16.f1=((_tmpE15.elt_type=(
void*)_tmp6F2,((_tmpE15.tq=_tmp6BA,((_tmpE15.num_elts=_tmp6BB,((_tmpE15.zero_term=
_tmp6BC,((_tmpE15.zt_loc=_tmp6BD,_tmpE15)))))))))),_tmpE16)))),_tmpE14))));}
_LL4BB: if(*((int*)_tmp6A8)!= 4)goto _LL4BD;_tmp6BE=((struct Cyc_Absyn_PointerType_struct*)
_tmp6A8)->f1;_tmp6BF=(void*)_tmp6BE.elt_typ;_tmp6C0=_tmp6BE.elt_tq;_tmp6C1=
_tmp6BE.ptr_atts;_tmp6C2=(void*)_tmp6C1.rgn;_tmp6C3=_tmp6C1.nullable;_tmp6C4=
_tmp6C1.bounds;_tmp6C5=_tmp6C1.zero_term;_LL4BC: {void*_tmp6F6=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6BF);void*_tmp6F7=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6C2);struct
Cyc_Absyn_Conref*_tmp6F8=_tmp6C4;{union Cyc_Absyn_Constraint_union _tmp6F9=(Cyc_Absyn_compress_conref(
_tmp6C4))->v;void*_tmp6FA;void*_tmp6FB;_LL4EB: if((_tmp6F9.Eq_constr).tag != 0)
goto _LL4ED;_tmp6FA=(_tmp6F9.Eq_constr).f1;if(_tmp6FA <= (void*)2)goto _LL4ED;if(*((
int*)_tmp6FA)!= 1)goto _LL4ED;_tmp6FB=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp6FA)->f1;_LL4EC: {void*_tmp6FC=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6FB);if(
_tmp6FB != _tmp6FC){struct Cyc_Absyn_AbsUpper_b_struct _tmpE19;struct Cyc_Absyn_AbsUpper_b_struct*
_tmpE18;_tmp6F8=Cyc_Absyn_new_conref((void*)((_tmpE18=_cycalloc(sizeof(*_tmpE18)),((
_tmpE18[0]=((_tmpE19.tag=1,((_tmpE19.f1=(void*)_tmp6FC,_tmpE19)))),_tmpE18)))));}
goto _LL4EA;}_LL4ED:;_LL4EE: goto _LL4EA;_LL4EA:;}if((_tmp6F6 == _tmp6BF  && _tmp6F7
== _tmp6C2) && _tmp6F8 == _tmp6C4)return t;{struct Cyc_Absyn_PointerType_struct
_tmpE23;struct Cyc_Absyn_PtrAtts _tmpE22;struct Cyc_Absyn_PtrInfo _tmpE21;struct Cyc_Absyn_PointerType_struct*
_tmpE20;return(void*)((_tmpE20=_cycalloc(sizeof(*_tmpE20)),((_tmpE20[0]=((
_tmpE23.tag=4,((_tmpE23.f1=((_tmpE21.elt_typ=(void*)_tmp6F6,((_tmpE21.elt_tq=
_tmp6C0,((_tmpE21.ptr_atts=((_tmpE22.rgn=(void*)_tmp6F7,((_tmpE22.nullable=
_tmp6C3,((_tmpE22.bounds=_tmp6F8,((_tmpE22.zero_term=_tmp6C5,((_tmpE22.ptrloc=0,
_tmpE22)))))))))),_tmpE21)))))),_tmpE23)))),_tmpE20))));}}_LL4BD: if(*((int*)
_tmp6A8)!= 8)goto _LL4BF;_tmp6C6=((struct Cyc_Absyn_FnType_struct*)_tmp6A8)->f1;
_tmp6C7=_tmp6C6.tvars;_tmp6C8=_tmp6C6.effect;_tmp6C9=(void*)_tmp6C6.ret_typ;
_tmp6CA=_tmp6C6.args;_tmp6CB=_tmp6C6.c_varargs;_tmp6CC=_tmp6C6.cyc_varargs;
_tmp6CD=_tmp6C6.rgn_po;_tmp6CE=_tmp6C6.attributes;_LL4BE:{struct Cyc_List_List*
_tmp703=_tmp6C7;for(0;_tmp703 != 0;_tmp703=_tmp703->tl){struct _tuple8*_tmpE2D;
struct Cyc_Absyn_VarType_struct _tmpE2C;struct Cyc_Absyn_VarType_struct*_tmpE2B;
struct Cyc_List_List*_tmpE2A;inst=((_tmpE2A=_region_malloc(rgn,sizeof(*_tmpE2A)),((
_tmpE2A->hd=((_tmpE2D=_region_malloc(rgn,sizeof(*_tmpE2D)),((_tmpE2D->f1=(struct
Cyc_Absyn_Tvar*)_tmp703->hd,((_tmpE2D->f2=(void*)((_tmpE2B=_cycalloc(sizeof(*
_tmpE2B)),((_tmpE2B[0]=((_tmpE2C.tag=1,((_tmpE2C.f1=(struct Cyc_Absyn_Tvar*)
_tmp703->hd,_tmpE2C)))),_tmpE2B)))),_tmpE2D)))))),((_tmpE2A->tl=inst,_tmpE2A))))));}}{
struct Cyc_List_List*_tmp709;struct Cyc_List_List*_tmp70A;struct _tuple1 _tmp708=((
struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x))Cyc_List_rsplit)(rgn,rgn,((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _tuple17*(*f)(struct _RegionHandle*,struct _tuple3*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp6CA));
_tmp709=_tmp708.f1;_tmp70A=_tmp708.f2;{struct Cyc_List_List*_tmp70B=Cyc_Tcutil_substs(
rgn,inst,_tmp70A);struct Cyc_List_List*_tmp70C=((struct Cyc_List_List*(*)(struct
_tuple3*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_substitute_f2,((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp709,_tmp70B));struct Cyc_Core_Opt*
eff2;if(_tmp6C8 == 0)eff2=0;else{void*_tmp70D=Cyc_Tcutil_rsubstitute(rgn,inst,(
void*)_tmp6C8->v);if(_tmp70D == (void*)_tmp6C8->v)eff2=_tmp6C8;else{struct Cyc_Core_Opt*
_tmpE2E;eff2=((_tmpE2E=_cycalloc(sizeof(*_tmpE2E)),((_tmpE2E->v=(void*)_tmp70D,
_tmpE2E))));}}{struct Cyc_Absyn_VarargInfo*cyc_varargs2;if(_tmp6CC == 0)
cyc_varargs2=0;else{struct Cyc_Core_Opt*_tmp710;struct Cyc_Absyn_Tqual _tmp711;void*
_tmp712;int _tmp713;struct Cyc_Absyn_VarargInfo _tmp70F=*_tmp6CC;_tmp710=_tmp70F.name;
_tmp711=_tmp70F.tq;_tmp712=(void*)_tmp70F.type;_tmp713=_tmp70F.inject;{void*
_tmp714=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp712);if(_tmp714 == _tmp712)
cyc_varargs2=_tmp6CC;else{struct Cyc_Absyn_VarargInfo*_tmpE2F;cyc_varargs2=((
_tmpE2F=_cycalloc(sizeof(*_tmpE2F)),((_tmpE2F->name=_tmp710,((_tmpE2F->tq=
_tmp711,((_tmpE2F->type=(void*)_tmp714,((_tmpE2F->inject=_tmp713,_tmpE2F))))))))));}}}{
struct Cyc_List_List*rgn_po2;struct Cyc_List_List*_tmp717;struct Cyc_List_List*
_tmp718;struct _tuple1 _tmp716=Cyc_List_rsplit(rgn,rgn,_tmp6CD);_tmp717=_tmp716.f1;
_tmp718=_tmp716.f2;{struct Cyc_List_List*_tmp719=Cyc_Tcutil_substs(rgn,inst,
_tmp717);struct Cyc_List_List*_tmp71A=Cyc_Tcutil_substs(rgn,inst,_tmp718);if(
_tmp719 == _tmp717  && _tmp71A == _tmp718)rgn_po2=_tmp6CD;else{rgn_po2=Cyc_List_zip(
_tmp719,_tmp71A);}{struct Cyc_Absyn_FnType_struct _tmpE35;struct Cyc_Absyn_FnInfo
_tmpE34;struct Cyc_Absyn_FnType_struct*_tmpE33;return(void*)((_tmpE33=_cycalloc(
sizeof(*_tmpE33)),((_tmpE33[0]=((_tmpE35.tag=8,((_tmpE35.f1=((_tmpE34.tvars=
_tmp6C7,((_tmpE34.effect=eff2,((_tmpE34.ret_typ=(void*)Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6C9),((_tmpE34.args=_tmp70C,((_tmpE34.c_varargs=_tmp6CB,((_tmpE34.cyc_varargs=
cyc_varargs2,((_tmpE34.rgn_po=rgn_po2,((_tmpE34.attributes=_tmp6CE,_tmpE34)))))))))))))))),
_tmpE35)))),_tmpE33))));}}}}}}_LL4BF: if(*((int*)_tmp6A8)!= 9)goto _LL4C1;_tmp6CF=((
struct Cyc_Absyn_TupleType_struct*)_tmp6A8)->f1;_LL4C0: {struct Cyc_List_List*
_tmp71E=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple5*),
struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_snd_tqt,_tmp6CF);struct Cyc_List_List*
_tmp71F=Cyc_Tcutil_substs(rgn,inst,_tmp71E);if(_tmp71F == _tmp71E)return t;{struct
Cyc_List_List*_tmp720=((struct Cyc_List_List*(*)(struct _tuple5*(*f)(struct _tuple5*,
void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,
_tmp6CF,_tmp71F);struct Cyc_Absyn_TupleType_struct _tmpE38;struct Cyc_Absyn_TupleType_struct*
_tmpE37;return(void*)((_tmpE37=_cycalloc(sizeof(*_tmpE37)),((_tmpE37[0]=((
_tmpE38.tag=9,((_tmpE38.f1=_tmp720,_tmpE38)))),_tmpE37))));}}_LL4C1: if(*((int*)
_tmp6A8)!= 11)goto _LL4C3;_tmp6D0=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp6A8)->f1;_tmp6D1=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp6A8)->f2;_LL4C2: {
struct Cyc_List_List*_tmp723=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_rmap)(rgn,Cyc_Tcutil_field_type,
_tmp6D1);struct Cyc_List_List*_tmp724=Cyc_Tcutil_substs(rgn,inst,_tmp723);if(
_tmp724 == _tmp723)return t;{struct Cyc_List_List*_tmp725=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_zip_field_type,_tmp6D1,
_tmp724);struct Cyc_Absyn_AnonAggrType_struct _tmpE3B;struct Cyc_Absyn_AnonAggrType_struct*
_tmpE3A;return(void*)((_tmpE3A=_cycalloc(sizeof(*_tmpE3A)),((_tmpE3A[0]=((
_tmpE3B.tag=11,((_tmpE3B.f1=(void*)_tmp6D0,((_tmpE3B.f2=_tmp725,_tmpE3B)))))),
_tmpE3A))));}}_LL4C3: if(*((int*)_tmp6A8)!= 0)goto _LL4C5;_tmp6D2=((struct Cyc_Absyn_Evar_struct*)
_tmp6A8)->f2;_LL4C4: if(_tmp6D2 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,(void*)
_tmp6D2->v);else{return t;}_LL4C5: if(*((int*)_tmp6A8)!= 15)goto _LL4C7;_tmp6D3=(
void*)((struct Cyc_Absyn_RgnHandleType_struct*)_tmp6A8)->f1;_LL4C6: {void*_tmp728=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6D3);struct Cyc_Absyn_RgnHandleType_struct
_tmpE3E;struct Cyc_Absyn_RgnHandleType_struct*_tmpE3D;return _tmp728 == _tmp6D3?t:(
void*)((_tmpE3D=_cycalloc(sizeof(*_tmpE3D)),((_tmpE3D[0]=((_tmpE3E.tag=15,((
_tmpE3E.f1=(void*)_tmp728,_tmpE3E)))),_tmpE3D))));}_LL4C7: if(*((int*)_tmp6A8)!= 
16)goto _LL4C9;_tmp6D4=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp6A8)->f1;
_tmp6D5=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp6A8)->f2;_LL4C8: {void*
_tmp72B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6D4);void*_tmp72C=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6D5);struct Cyc_Absyn_DynRgnType_struct _tmpE41;struct Cyc_Absyn_DynRgnType_struct*
_tmpE40;return _tmp72B == _tmp6D4  && _tmp72C == _tmp6D5?t:(void*)((_tmpE40=
_cycalloc(sizeof(*_tmpE40)),((_tmpE40[0]=((_tmpE41.tag=16,((_tmpE41.f1=(void*)
_tmp72B,((_tmpE41.f2=(void*)_tmp72C,_tmpE41)))))),_tmpE40))));}_LL4C9: if(*((int*)
_tmp6A8)!= 14)goto _LL4CB;_tmp6D6=(void*)((struct Cyc_Absyn_SizeofType_struct*)
_tmp6A8)->f1;_LL4CA: {void*_tmp72F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6D6);
struct Cyc_Absyn_SizeofType_struct _tmpE44;struct Cyc_Absyn_SizeofType_struct*
_tmpE43;return _tmp72F == _tmp6D6?t:(void*)((_tmpE43=_cycalloc(sizeof(*_tmpE43)),((
_tmpE43[0]=((_tmpE44.tag=14,((_tmpE44.f1=(void*)_tmp72F,_tmpE44)))),_tmpE43))));}
_LL4CB: if(*((int*)_tmp6A8)!= 18)goto _LL4CD;_tmp6D7=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp6A8)->f1;_LL4CC: {void*_tmp732=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6D7);
struct Cyc_Absyn_TagType_struct _tmpE47;struct Cyc_Absyn_TagType_struct*_tmpE46;
return _tmp732 == _tmp6D7?t:(void*)((_tmpE46=_cycalloc(sizeof(*_tmpE46)),((_tmpE46[
0]=((_tmpE47.tag=18,((_tmpE47.f1=(void*)_tmp732,_tmpE47)))),_tmpE46))));}_LL4CD:
if(*((int*)_tmp6A8)!= 19)goto _LL4CF;_LL4CE: goto _LL4D0;_LL4CF: if(*((int*)_tmp6A8)
!= 12)goto _LL4D1;_LL4D0: goto _LL4D2;_LL4D1: if(*((int*)_tmp6A8)!= 13)goto _LL4D3;
_LL4D2: goto _LL4D4;_LL4D3: if((int)_tmp6A8 != 0)goto _LL4D5;_LL4D4: goto _LL4D6;_LL4D5:
if(_tmp6A8 <= (void*)4)goto _LL4D7;if(*((int*)_tmp6A8)!= 5)goto _LL4D7;_LL4D6: goto
_LL4D8;_LL4D7: if((int)_tmp6A8 != 1)goto _LL4D9;_LL4D8: goto _LL4DA;_LL4D9: if(_tmp6A8
<= (void*)4)goto _LL4DB;if(*((int*)_tmp6A8)!= 6)goto _LL4DB;_LL4DA: goto _LL4DC;
_LL4DB: if((int)_tmp6A8 != 3)goto _LL4DD;_LL4DC: goto _LL4DE;_LL4DD: if((int)_tmp6A8 != 
2)goto _LL4DF;_LL4DE: return t;_LL4DF: if(_tmp6A8 <= (void*)4)goto _LL4E1;if(*((int*)
_tmp6A8)!= 22)goto _LL4E1;_tmp6D8=(void*)((struct Cyc_Absyn_RgnsEff_struct*)
_tmp6A8)->f1;_LL4E0: {void*_tmp735=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6D8);
struct Cyc_Absyn_RgnsEff_struct _tmpE4A;struct Cyc_Absyn_RgnsEff_struct*_tmpE49;
return _tmp735 == _tmp6D8?t:(void*)((_tmpE49=_cycalloc(sizeof(*_tmpE49)),((_tmpE49[
0]=((_tmpE4A.tag=22,((_tmpE4A.f1=(void*)_tmp735,_tmpE4A)))),_tmpE49))));}_LL4E1:
if(_tmp6A8 <= (void*)4)goto _LL4E3;if(*((int*)_tmp6A8)!= 20)goto _LL4E3;_tmp6D9=(
void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp6A8)->f1;_LL4E2: {void*_tmp738=Cyc_Tcutil_rsubstitute(
rgn,inst,_tmp6D9);struct Cyc_Absyn_AccessEff_struct _tmpE4D;struct Cyc_Absyn_AccessEff_struct*
_tmpE4C;return _tmp738 == _tmp6D9?t:(void*)((_tmpE4C=_cycalloc(sizeof(*_tmpE4C)),((
_tmpE4C[0]=((_tmpE4D.tag=20,((_tmpE4D.f1=(void*)_tmp738,_tmpE4D)))),_tmpE4C))));}
_LL4E3: if(_tmp6A8 <= (void*)4)goto _LL4AE;if(*((int*)_tmp6A8)!= 21)goto _LL4AE;
_tmp6DA=((struct Cyc_Absyn_JoinEff_struct*)_tmp6A8)->f1;_LL4E4: {struct Cyc_List_List*
_tmp73B=Cyc_Tcutil_substs(rgn,inst,_tmp6DA);struct Cyc_Absyn_JoinEff_struct
_tmpE50;struct Cyc_Absyn_JoinEff_struct*_tmpE4F;return _tmp73B == _tmp6DA?t:(void*)((
_tmpE4F=_cycalloc(sizeof(*_tmpE4F)),((_tmpE4F[0]=((_tmpE50.tag=21,((_tmpE50.f1=
_tmp73B,_tmpE50)))),_tmpE4F))));}_LL4AE:;}static struct Cyc_List_List*Cyc_Tcutil_substs(
struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);static
struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*
inst,struct Cyc_List_List*ts){if(ts == 0)return 0;{void*_tmp73E=(void*)ts->hd;
struct Cyc_List_List*_tmp73F=ts->tl;void*_tmp740=Cyc_Tcutil_rsubstitute(rgn,inst,
_tmp73E);struct Cyc_List_List*_tmp741=Cyc_Tcutil_substs(rgn,inst,_tmp73F);if(
_tmp73E == _tmp740  && _tmp73F == _tmp741)return ts;{struct Cyc_List_List*_tmpE51;
return(struct Cyc_List_List*)((struct Cyc_List_List*)((_tmpE51=_cycalloc(sizeof(*
_tmpE51)),((_tmpE51->hd=(void*)_tmp740,((_tmpE51->tl=_tmp741,_tmpE51)))))));}}}
void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t);extern void*Cyc_Tcutil_substitute(
struct Cyc_List_List*inst,void*t){return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,
inst,t);}struct _tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv);struct _tuple8*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*
tv){struct Cyc_Core_Opt*_tmp743=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv));
struct Cyc_Core_Opt*_tmpE54;struct _tuple8*_tmpE53;return(_tmpE53=_cycalloc(
sizeof(*_tmpE53)),((_tmpE53->f1=tv,((_tmpE53->f2=Cyc_Absyn_new_evar(_tmp743,((
_tmpE54=_cycalloc(sizeof(*_tmpE54)),((_tmpE54->v=s,_tmpE54))))),_tmpE53)))));}
struct _tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*env,struct Cyc_Absyn_Tvar*
tv);struct _tuple8*Cyc_Tcutil_r_make_inst_var(struct _tuple9*env,struct Cyc_Absyn_Tvar*
tv){struct _tuple9 _tmp747;struct Cyc_List_List*_tmp748;struct _RegionHandle*_tmp749;
struct _tuple9*_tmp746=env;_tmp747=*_tmp746;_tmp748=_tmp747.f1;_tmp749=_tmp747.f2;{
struct Cyc_Core_Opt*_tmp74A=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv));
struct Cyc_Core_Opt*_tmpE57;struct _tuple8*_tmpE56;return(_tmpE56=_region_malloc(
_tmp749,sizeof(*_tmpE56)),((_tmpE56->f1=tv,((_tmpE56->f2=Cyc_Absyn_new_evar(
_tmp74A,((_tmpE57=_cycalloc(sizeof(*_tmpE57)),((_tmpE57->v=_tmp748,_tmpE57))))),
_tmpE56)))));}}static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*
loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*
Cyc_Tcutil_add_free_tvar(struct Cyc_Position_Segment*loc,struct Cyc_List_List*tvs,
struct Cyc_Absyn_Tvar*tv){{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){void*k1=(
void*)((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;void*k2=(void*)tv->kind;if(!Cyc_Tcutil_constrain_kinds(
k1,k2)){const char*_tmpE5D;void*_tmpE5C[3];struct Cyc_String_pa_struct _tmpE5B;
struct Cyc_String_pa_struct _tmpE5A;struct Cyc_String_pa_struct _tmpE59;(_tmpE59.tag=
0,((_tmpE59.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kindbound2string(
k2)),((_tmpE5A.tag=0,((_tmpE5A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_kindbound2string(k1)),((_tmpE5B.tag=0,((_tmpE5B.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*tv->name),((_tmpE5C[0]=& _tmpE5B,((
_tmpE5C[1]=& _tmpE5A,((_tmpE5C[2]=& _tmpE59,Cyc_Tcutil_terr(loc,((_tmpE5D="type variable %s is used with inconsistent kinds %s and %s",
_tag_dynforward(_tmpE5D,sizeof(char),_get_zero_arr_size_char(_tmpE5D,59)))),
_tag_dynforward(_tmpE5C,sizeof(void*),3)))))))))))))))))));}if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{if(tv->identity != ((
struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){const char*_tmpE60;void*_tmpE5F;(
_tmpE5F=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmpE60="same type variable has different identity!",_tag_dynforward(_tmpE60,
sizeof(char),_get_zero_arr_size_char(_tmpE60,43)))),_tag_dynforward(_tmpE5F,
sizeof(void*),0)));}}return tvs;}}}tv->identity=Cyc_Tcutil_new_tvar_id();{struct
Cyc_List_List*_tmpE61;return(_tmpE61=_cycalloc(sizeof(*_tmpE61)),((_tmpE61->hd=
tv,((_tmpE61->tl=tvs,_tmpE61)))));}}static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmpE64;void*_tmpE63;(_tmpE63=0,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmpE64="fast_add_free_tvar: bad identity in tv",
_tag_dynforward(_tmpE64,sizeof(char),_get_zero_arr_size_char(_tmpE64,39)))),
_tag_dynforward(_tmpE63,sizeof(void*),0)));}{struct Cyc_List_List*tvs2=tvs;for(0;
tvs2 != 0;tvs2=tvs2->tl){struct Cyc_Absyn_Tvar*_tmp757=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp757->identity == - 1){const char*_tmpE67;void*_tmpE66;(_tmpE66=0,((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmpE67="fast_add_free_tvar: bad identity in tvs2",
_tag_dynforward(_tmpE67,sizeof(char),_get_zero_arr_size_char(_tmpE67,41)))),
_tag_dynforward(_tmpE66,sizeof(void*),0)));}if(_tmp757->identity == tv->identity)
return tvs;}}{struct Cyc_List_List*_tmpE68;return(_tmpE68=_cycalloc(sizeof(*
_tmpE68)),((_tmpE68->hd=tv,((_tmpE68->tl=tvs,_tmpE68)))));}}struct _tuple18{
struct Cyc_Absyn_Tvar*f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(
struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b);
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*
r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){if(tv->identity == - 1){
const char*_tmpE6B;void*_tmpE6A;(_tmpE6A=0,((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmpE6B="fast_add_free_tvar_bool: bad identity in tv",
_tag_dynforward(_tmpE6B,sizeof(char),_get_zero_arr_size_char(_tmpE6B,44)))),
_tag_dynforward(_tmpE6A,sizeof(void*),0)));}{struct Cyc_List_List*tvs2=tvs;for(0;
tvs2 != 0;tvs2=tvs2->tl){struct _tuple18 _tmp75E;struct Cyc_Absyn_Tvar*_tmp75F;int
_tmp760;int*_tmp761;struct _tuple18*_tmp75D=(struct _tuple18*)tvs2->hd;_tmp75E=*
_tmp75D;_tmp75F=_tmp75E.f1;_tmp760=_tmp75E.f2;_tmp761=(int*)&(*_tmp75D).f2;if(
_tmp75F->identity == - 1){const char*_tmpE6E;void*_tmpE6D;(_tmpE6D=0,((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmpE6E="fast_add_free_tvar_bool: bad identity in tvs2",
_tag_dynforward(_tmpE6E,sizeof(char),_get_zero_arr_size_char(_tmpE6E,46)))),
_tag_dynforward(_tmpE6D,sizeof(void*),0)));}if(_tmp75F->identity == tv->identity){*
_tmp761=*_tmp761  || b;return tvs;}}}{struct _tuple18*_tmpE71;struct Cyc_List_List*
_tmpE70;return(_tmpE70=_region_malloc(r,sizeof(*_tmpE70)),((_tmpE70->hd=((
_tmpE71=_region_malloc(r,sizeof(*_tmpE71)),((_tmpE71->f1=tv,((_tmpE71->f2=b,
_tmpE71)))))),((_tmpE70->tl=tvs,_tmpE70)))));}}static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv);static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(
struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1){const char*
_tmpE75;void*_tmpE74[1];struct Cyc_String_pa_struct _tmpE73;(_tmpE73.tag=0,((
_tmpE73.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcutil_tvar2string(
tv)),((_tmpE74[0]=& _tmpE73,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmpE75="bound tvar id for %s is NULL",
_tag_dynforward(_tmpE75,sizeof(char),_get_zero_arr_size_char(_tmpE75,29)))),
_tag_dynforward(_tmpE74,sizeof(void*),1)))))));}{struct Cyc_List_List*_tmpE76;
return(_tmpE76=_cycalloc(sizeof(*_tmpE76)),((_tmpE76->hd=tv,((_tmpE76->tl=tvs,
_tmpE76)))));}}struct _tuple19{void*f1;int f2;};static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(
struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b);static struct Cyc_List_List*
Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int
b){void*_tmp76A=Cyc_Tcutil_compress(e);int _tmp76B;_LL4F0: if(_tmp76A <= (void*)4)
goto _LL4F2;if(*((int*)_tmp76A)!= 0)goto _LL4F2;_tmp76B=((struct Cyc_Absyn_Evar_struct*)
_tmp76A)->f3;_LL4F1:{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple19 _tmp76D;void*_tmp76E;int _tmp76F;int*_tmp770;struct _tuple19*_tmp76C=(
struct _tuple19*)es2->hd;_tmp76D=*_tmp76C;_tmp76E=_tmp76D.f1;_tmp76F=_tmp76D.f2;
_tmp770=(int*)&(*_tmp76C).f2;{void*_tmp771=Cyc_Tcutil_compress(_tmp76E);int
_tmp772;_LL4F5: if(_tmp771 <= (void*)4)goto _LL4F7;if(*((int*)_tmp771)!= 0)goto
_LL4F7;_tmp772=((struct Cyc_Absyn_Evar_struct*)_tmp771)->f3;_LL4F6: if(_tmp76B == 
_tmp772){if(b != *_tmp770)*_tmp770=1;return es;}goto _LL4F4;_LL4F7:;_LL4F8: goto
_LL4F4;_LL4F4:;}}}{struct _tuple19*_tmpE79;struct Cyc_List_List*_tmpE78;return(
_tmpE78=_region_malloc(r,sizeof(*_tmpE78)),((_tmpE78->hd=((_tmpE79=
_region_malloc(r,sizeof(*_tmpE79)),((_tmpE79->f1=e,((_tmpE79->f2=b,_tmpE79)))))),((
_tmpE78->tl=es,_tmpE78)))));}_LL4F2:;_LL4F3: return es;_LL4EF:;}static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,
struct Cyc_List_List*btvs);static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(
struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct
Cyc_List_List*r=0;for(0;tvs != 0;tvs=tvs->tl){int present=0;{struct Cyc_List_List*b=
btvs;for(0;b != 0;b=b->tl){if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((
struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;break;}}}if(!present){struct
Cyc_List_List*_tmpE7A;r=((_tmpE7A=_region_malloc(rgn,sizeof(*_tmpE7A)),((_tmpE7A->hd=(
struct Cyc_Absyn_Tvar*)tvs->hd,((_tmpE7A->tl=r,_tmpE7A))))));}}r=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(r);return r;}static struct Cyc_List_List*
Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,
struct Cyc_List_List*btvs);static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(
struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){struct
Cyc_List_List*res=0;for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp777;int
_tmp778;struct _tuple18 _tmp776=*((struct _tuple18*)tvs->hd);_tmp777=_tmp776.f1;
_tmp778=_tmp776.f2;{int present=0;{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp777->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){present=1;
break;}}}if(!present){struct Cyc_List_List*_tmpE7B;res=((_tmpE7B=_region_malloc(r,
sizeof(*_tmpE7B)),((_tmpE7B->hd=(struct _tuple18*)tvs->hd,((_tmpE7B->tl=res,
_tmpE7B))))));}}}res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
res);return res;}void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dynforward_ptr*fn);void Cyc_Tcutil_check_bitfield(struct Cyc_Position_Segment*loc,
struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct
_dynforward_ptr*fn){if(width != 0){unsigned int w=0;if(!Cyc_Tcutil_is_const_exp(te,(
struct Cyc_Absyn_Exp*)width)){const char*_tmpE7F;void*_tmpE7E[1];struct Cyc_String_pa_struct
_tmpE7D;(_tmpE7D.tag=0,((_tmpE7D.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)*fn),((_tmpE7E[0]=& _tmpE7D,Cyc_Tcutil_terr(loc,((_tmpE7F="bitfield %s does not have constant width",
_tag_dynforward(_tmpE7F,sizeof(char),_get_zero_arr_size_char(_tmpE7F,41)))),
_tag_dynforward(_tmpE7E,sizeof(void*),1)))))));}else{unsigned int _tmp77E;int
_tmp77F;struct _tuple7 _tmp77D=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)
width);_tmp77E=_tmp77D.f1;_tmp77F=_tmp77D.f2;if(!_tmp77F){const char*_tmpE82;void*
_tmpE81;(_tmpE81=0,Cyc_Tcutil_terr(loc,((_tmpE82="bitfield width cannot use sizeof or offsetof",
_tag_dynforward(_tmpE82,sizeof(char),_get_zero_arr_size_char(_tmpE82,45)))),
_tag_dynforward(_tmpE81,sizeof(void*),0)));}w=_tmp77E;}{void*_tmp782=Cyc_Tcutil_compress(
field_typ);void*_tmp783;_LL4FA: if(_tmp782 <= (void*)4)goto _LL4FC;if(*((int*)
_tmp782)!= 5)goto _LL4FC;_tmp783=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp782)->f2;
_LL4FB:{void*_tmp784=_tmp783;_LL4FF: if((int)_tmp784 != 0)goto _LL501;_LL500: if(w > 
8){const char*_tmpE85;void*_tmpE84;(_tmpE84=0,Cyc_Tcutil_terr(loc,((_tmpE85="bitfield larger than type",
_tag_dynforward(_tmpE85,sizeof(char),_get_zero_arr_size_char(_tmpE85,26)))),
_tag_dynforward(_tmpE84,sizeof(void*),0)));}goto _LL4FE;_LL501: if((int)_tmp784 != 
1)goto _LL503;_LL502: if(w > 16){const char*_tmpE88;void*_tmpE87;(_tmpE87=0,Cyc_Tcutil_terr(
loc,((_tmpE88="bitfield larger than type",_tag_dynforward(_tmpE88,sizeof(char),
_get_zero_arr_size_char(_tmpE88,26)))),_tag_dynforward(_tmpE87,sizeof(void*),0)));}
goto _LL4FE;_LL503: if((int)_tmp784 != 3)goto _LL505;_LL504: goto _LL506;_LL505: if((
int)_tmp784 != 2)goto _LL507;_LL506: if(w > 32){const char*_tmpE8B;void*_tmpE8A;(
_tmpE8A=0,Cyc_Tcutil_terr(loc,((_tmpE8B="bitfield larger than type",
_tag_dynforward(_tmpE8B,sizeof(char),_get_zero_arr_size_char(_tmpE8B,26)))),
_tag_dynforward(_tmpE8A,sizeof(void*),0)));}goto _LL4FE;_LL507: if((int)_tmp784 != 
4)goto _LL4FE;_LL508: if(w > 64){const char*_tmpE8E;void*_tmpE8D;(_tmpE8D=0,Cyc_Tcutil_terr(
loc,((_tmpE8E="bitfield larger than type",_tag_dynforward(_tmpE8E,sizeof(char),
_get_zero_arr_size_char(_tmpE8E,26)))),_tag_dynforward(_tmpE8D,sizeof(void*),0)));}
goto _LL4FE;_LL4FE:;}goto _LL4F9;_LL4FC:;_LL4FD:{const char*_tmpE93;void*_tmpE92[2];
struct Cyc_String_pa_struct _tmpE91;struct Cyc_String_pa_struct _tmpE90;(_tmpE90.tag=
0,((_tmpE90.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
field_typ)),((_tmpE91.tag=0,((_tmpE91.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)*fn),((_tmpE92[0]=& _tmpE91,((_tmpE92[1]=& _tmpE90,Cyc_Tcutil_terr(
loc,((_tmpE93="bitfield %s must have integral type but has type %s",
_tag_dynforward(_tmpE93,sizeof(char),_get_zero_arr_size_char(_tmpE93,52)))),
_tag_dynforward(_tmpE92,sizeof(void*),2)))))))))))));}goto _LL4F9;_LL4F9:;}}}
static void Cyc_Tcutil_check_field_atts(struct Cyc_Position_Segment*loc,struct
_dynforward_ptr*fn,struct Cyc_List_List*atts);static void Cyc_Tcutil_check_field_atts(
struct Cyc_Position_Segment*loc,struct _dynforward_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){void*_tmp791=(void*)atts->hd;_LL50A: if((int)
_tmp791 != 5)goto _LL50C;_LL50B: continue;_LL50C: if(_tmp791 <= (void*)17)goto _LL50E;
if(*((int*)_tmp791)!= 1)goto _LL50E;_LL50D: continue;_LL50E:;_LL50F: {const char*
_tmpE98;void*_tmpE97[2];struct Cyc_String_pa_struct _tmpE96;struct Cyc_String_pa_struct
_tmpE95;(_tmpE95.tag=0,((_tmpE95.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)*fn),((_tmpE96.tag=0,((_tmpE96.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmpE97[0]=&
_tmpE96,((_tmpE97[1]=& _tmpE95,Cyc_Tcutil_terr(loc,((_tmpE98="bad attribute %s on member %s",
_tag_dynforward(_tmpE98,sizeof(char),_get_zero_arr_size_char(_tmpE98,30)))),
_tag_dynforward(_tmpE97,sizeof(void*),2)))))))))))));}_LL509:;}}struct Cyc_Tcutil_CVTEnv{
struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;
struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t);int Cyc_Tcutil_extract_const_from_typedef(
struct Cyc_Position_Segment*loc,int declared_const,void*t){void*_tmp796=t;struct
Cyc_Absyn_Typedefdecl*_tmp797;void**_tmp798;_LL511: if(_tmp796 <= (void*)4)goto
_LL513;if(*((int*)_tmp796)!= 17)goto _LL513;_tmp797=((struct Cyc_Absyn_TypedefType_struct*)
_tmp796)->f3;_tmp798=((struct Cyc_Absyn_TypedefType_struct*)_tmp796)->f4;_LL512:
if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp797))->tq).real_const  || (
_tmp797->tq).print_const){if(declared_const){const char*_tmpE9B;void*_tmpE9A;(
_tmpE9A=0,Cyc_Tcutil_warn(loc,((_tmpE9B="extra const",_tag_dynforward(_tmpE9B,
sizeof(char),_get_zero_arr_size_char(_tmpE9B,12)))),_tag_dynforward(_tmpE9A,
sizeof(void*),0)));}return 1;}if((unsigned int)_tmp798)return Cyc_Tcutil_extract_const_from_typedef(
loc,declared_const,*_tmp798);else{return declared_const;}_LL513:;_LL514: return
declared_const;_LL510:;}static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv
cvtenv,void*expected_kind,void*t,int put_in_effect);static struct Cyc_Tcutil_CVTEnv
Cyc_Tcutil_i_check_valid_type(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcutil_CVTEnv cvtenv,void*expected_kind,void*t,int put_in_effect){
static struct Cyc_Core_Opt urgn={(void*)((void*)3)};static struct Cyc_Core_Opt hrgn={(
void*)((void*)2)};{void*_tmp79B=Cyc_Tcutil_compress(t);struct Cyc_Core_Opt*
_tmp79C;struct Cyc_Core_Opt**_tmp79D;struct Cyc_Core_Opt*_tmp79E;struct Cyc_Core_Opt**
_tmp79F;struct Cyc_Absyn_Tvar*_tmp7A0;struct Cyc_List_List*_tmp7A1;struct _tuple2*
_tmp7A2;struct Cyc_Absyn_Enumdecl*_tmp7A3;struct Cyc_Absyn_Enumdecl**_tmp7A4;
struct Cyc_Absyn_TunionInfo _tmp7A5;union Cyc_Absyn_TunionInfoU_union _tmp7A6;union
Cyc_Absyn_TunionInfoU_union*_tmp7A7;struct Cyc_List_List*_tmp7A8;struct Cyc_List_List**
_tmp7A9;struct Cyc_Core_Opt*_tmp7AA;struct Cyc_Core_Opt**_tmp7AB;struct Cyc_Absyn_TunionFieldInfo
_tmp7AC;union Cyc_Absyn_TunionFieldInfoU_union _tmp7AD;union Cyc_Absyn_TunionFieldInfoU_union*
_tmp7AE;struct Cyc_List_List*_tmp7AF;struct Cyc_Absyn_PtrInfo _tmp7B0;void*_tmp7B1;
struct Cyc_Absyn_Tqual _tmp7B2;struct Cyc_Absyn_Tqual*_tmp7B3;struct Cyc_Absyn_PtrAtts
_tmp7B4;void*_tmp7B5;struct Cyc_Absyn_Conref*_tmp7B6;struct Cyc_Absyn_Conref*
_tmp7B7;struct Cyc_Absyn_Conref*_tmp7B8;void*_tmp7B9;void*_tmp7BA;struct Cyc_Absyn_ArrayInfo
_tmp7BB;void*_tmp7BC;struct Cyc_Absyn_Tqual _tmp7BD;struct Cyc_Absyn_Tqual*_tmp7BE;
struct Cyc_Absyn_Exp*_tmp7BF;struct Cyc_Absyn_Exp**_tmp7C0;struct Cyc_Absyn_Conref*
_tmp7C1;struct Cyc_Position_Segment*_tmp7C2;struct Cyc_Absyn_FnInfo _tmp7C3;struct
Cyc_List_List*_tmp7C4;struct Cyc_List_List**_tmp7C5;struct Cyc_Core_Opt*_tmp7C6;
struct Cyc_Core_Opt**_tmp7C7;void*_tmp7C8;struct Cyc_List_List*_tmp7C9;int _tmp7CA;
struct Cyc_Absyn_VarargInfo*_tmp7CB;struct Cyc_List_List*_tmp7CC;struct Cyc_List_List*
_tmp7CD;struct Cyc_List_List*_tmp7CE;void*_tmp7CF;struct Cyc_List_List*_tmp7D0;
struct Cyc_Absyn_AggrInfo _tmp7D1;union Cyc_Absyn_AggrInfoU_union _tmp7D2;union Cyc_Absyn_AggrInfoU_union*
_tmp7D3;struct Cyc_List_List*_tmp7D4;struct Cyc_List_List**_tmp7D5;struct _tuple2*
_tmp7D6;struct Cyc_List_List*_tmp7D7;struct Cyc_List_List**_tmp7D8;struct Cyc_Absyn_Typedefdecl*
_tmp7D9;struct Cyc_Absyn_Typedefdecl**_tmp7DA;void**_tmp7DB;void***_tmp7DC;void*
_tmp7DD;void*_tmp7DE;void*_tmp7DF;void*_tmp7E0;void*_tmp7E1;struct Cyc_List_List*
_tmp7E2;_LL516: if((int)_tmp79B != 0)goto _LL518;_LL517: goto _LL515;_LL518: if(
_tmp79B <= (void*)4)goto _LL52C;if(*((int*)_tmp79B)!= 0)goto _LL51A;_tmp79C=((
struct Cyc_Absyn_Evar_struct*)_tmp79B)->f1;_tmp79D=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp79B)->f1;_tmp79E=((struct Cyc_Absyn_Evar_struct*)
_tmp79B)->f2;_tmp79F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_struct*)
_tmp79B)->f2;_LL519: if(*_tmp79D == 0)*_tmp79D=Cyc_Tcutil_kind_to_opt(
expected_kind);if((cvtenv.fn_result  && cvtenv.generalize_evars) && Cyc_Tcutil_is_region_kind(
expected_kind)){if(expected_kind == (void*)4)*_tmp79F=(struct Cyc_Core_Opt*)& urgn;
else{*_tmp79F=(struct Cyc_Core_Opt*)& hrgn;}}else{if(cvtenv.generalize_evars){
struct Cyc_Absyn_Less_kb_struct _tmpE9E;struct Cyc_Absyn_Less_kb_struct*_tmpE9D;
struct Cyc_Absyn_Tvar*_tmp7E3=Cyc_Tcutil_new_tvar((void*)((_tmpE9D=_cycalloc(
sizeof(*_tmpE9D)),((_tmpE9D[0]=((_tmpE9E.tag=2,((_tmpE9E.f1=0,((_tmpE9E.f2=(void*)
expected_kind,_tmpE9E)))))),_tmpE9D)))));{struct Cyc_Absyn_VarType_struct*_tmpEA4;
struct Cyc_Absyn_VarType_struct _tmpEA3;struct Cyc_Core_Opt*_tmpEA2;*_tmp79F=((
_tmpEA2=_cycalloc(sizeof(*_tmpEA2)),((_tmpEA2->v=(void*)((void*)((_tmpEA4=
_cycalloc(sizeof(*_tmpEA4)),((_tmpEA4[0]=((_tmpEA3.tag=1,((_tmpEA3.f1=_tmp7E3,
_tmpEA3)))),_tmpEA4))))),_tmpEA2))));}_tmp7A0=_tmp7E3;goto _LL51B;}else{cvtenv.free_evars=
Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}goto
_LL515;_LL51A: if(*((int*)_tmp79B)!= 1)goto _LL51C;_tmp7A0=((struct Cyc_Absyn_VarType_struct*)
_tmp79B)->f1;_LL51B:{void*_tmp7E9=Cyc_Absyn_compress_kb((void*)_tmp7A0->kind);
struct Cyc_Core_Opt*_tmp7EA;struct Cyc_Core_Opt**_tmp7EB;_LL54D: if(*((int*)_tmp7E9)
!= 1)goto _LL54F;_tmp7EA=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp7E9)->f1;
_tmp7EB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp7E9)->f1;
_LL54E:{struct Cyc_Absyn_Less_kb_struct*_tmpEAA;struct Cyc_Absyn_Less_kb_struct
_tmpEA9;struct Cyc_Core_Opt*_tmpEA8;*_tmp7EB=((_tmpEA8=_cycalloc(sizeof(*_tmpEA8)),((
_tmpEA8->v=(void*)((void*)((_tmpEAA=_cycalloc(sizeof(*_tmpEAA)),((_tmpEAA[0]=((
_tmpEA9.tag=2,((_tmpEA9.f1=0,((_tmpEA9.f2=(void*)expected_kind,_tmpEA9)))))),
_tmpEAA))))),_tmpEA8))));}goto _LL54C;_LL54F:;_LL550: goto _LL54C;_LL54C:;}cvtenv.kind_env=
Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp7A0);cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(
cvtenv.r,cvtenv.free_vars,_tmp7A0,put_in_effect);{void*_tmp7EF=Cyc_Absyn_compress_kb((
void*)_tmp7A0->kind);struct Cyc_Core_Opt*_tmp7F0;struct Cyc_Core_Opt**_tmp7F1;void*
_tmp7F2;_LL552: if(*((int*)_tmp7EF)!= 2)goto _LL554;_tmp7F0=((struct Cyc_Absyn_Less_kb_struct*)
_tmp7EF)->f1;_tmp7F1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp7EF)->f1;_tmp7F2=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp7EF)->f2;
_LL553: if(Cyc_Tcutil_kind_leq(expected_kind,_tmp7F2)){struct Cyc_Absyn_Less_kb_struct*
_tmpEB0;struct Cyc_Absyn_Less_kb_struct _tmpEAF;struct Cyc_Core_Opt*_tmpEAE;*
_tmp7F1=((_tmpEAE=_cycalloc(sizeof(*_tmpEAE)),((_tmpEAE->v=(void*)((void*)((
_tmpEB0=_cycalloc(sizeof(*_tmpEB0)),((_tmpEB0[0]=((_tmpEAF.tag=2,((_tmpEAF.f1=0,((
_tmpEAF.f2=(void*)expected_kind,_tmpEAF)))))),_tmpEB0))))),_tmpEAE))));}goto
_LL551;_LL554:;_LL555: goto _LL551;_LL551:;}goto _LL515;_LL51C: if(*((int*)_tmp79B)
!= 13)goto _LL51E;_tmp7A1=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp79B)->f1;
_LL51D: {struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,
struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);{struct _RegionHandle
_tmp7F6=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp7F6;
_push_region(uprev_rgn);{struct Cyc_List_List*prev_fields=0;unsigned int tag_count=
0;for(0;_tmp7A1 != 0;_tmp7A1=_tmp7A1->tl){struct Cyc_Absyn_Enumfield*_tmp7F7=(
struct Cyc_Absyn_Enumfield*)_tmp7A1->hd;if(((int(*)(int(*compare)(struct
_dynforward_ptr*,struct _dynforward_ptr*),struct Cyc_List_List*l,struct
_dynforward_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp7F7->name).f2)){
const char*_tmpEB4;void*_tmpEB3[1];struct Cyc_String_pa_struct _tmpEB2;(_tmpEB2.tag=
0,((_tmpEB2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*(*_tmp7F7->name).f2),((
_tmpEB3[0]=& _tmpEB2,Cyc_Tcutil_terr(_tmp7F7->loc,((_tmpEB4="duplicate enum field name %s",
_tag_dynforward(_tmpEB4,sizeof(char),_get_zero_arr_size_char(_tmpEB4,29)))),
_tag_dynforward(_tmpEB3,sizeof(void*),1)))))));}else{struct Cyc_List_List*_tmpEB5;
prev_fields=((_tmpEB5=_region_malloc(uprev_rgn,sizeof(*_tmpEB5)),((_tmpEB5->hd=(*
_tmp7F7->name).f2,((_tmpEB5->tl=prev_fields,_tmpEB5))))));}if(_tmp7F7->tag == 0)
_tmp7F7->tag=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(tag_count,_tmp7F7->loc);
else{if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)_check_null(_tmp7F7->tag))){
const char*_tmpEB9;void*_tmpEB8[1];struct Cyc_String_pa_struct _tmpEB7;(_tmpEB7.tag=
0,((_tmpEB7.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*(*_tmp7F7->name).f2),((
_tmpEB8[0]=& _tmpEB7,Cyc_Tcutil_terr(loc,((_tmpEB9="enum field %s: expression is not constant",
_tag_dynforward(_tmpEB9,sizeof(char),_get_zero_arr_size_char(_tmpEB9,42)))),
_tag_dynforward(_tmpEB8,sizeof(void*),1)))))));}}{unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_check_null(_tmp7F7->tag))).f1;tag_count=t1 + 1;{union Cyc_Absyn_Nmspace_union
_tmpEBA;(*_tmp7F7->name).f1=(union Cyc_Absyn_Nmspace_union)(((_tmpEBA.Abs_n).tag=
2,(((_tmpEBA.Abs_n).f1=te->ns,_tmpEBA))));}{struct Cyc_Tcenv_AnonEnumRes_struct*
_tmpEC0;struct Cyc_Tcenv_AnonEnumRes_struct _tmpEBF;struct _tuple19*_tmpEBE;ge->ordinaries=((
struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dynforward_ptr*k,struct
_tuple19*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp7F7->name).f2,(struct _tuple19*)((
_tmpEBE=_cycalloc(sizeof(*_tmpEBE)),((_tmpEBE->f1=(void*)((_tmpEC0=_cycalloc(
sizeof(*_tmpEC0)),((_tmpEC0[0]=((_tmpEBF.tag=4,((_tmpEBF.f1=(void*)t,((_tmpEBF.f2=
_tmp7F7,_tmpEBF)))))),_tmpEC0)))),((_tmpEBE->f2=1,_tmpEBE)))))));}}}};
_pop_region(uprev_rgn);}goto _LL515;}_LL51E: if(*((int*)_tmp79B)!= 12)goto _LL520;
_tmp7A2=((struct Cyc_Absyn_EnumType_struct*)_tmp79B)->f1;_tmp7A3=((struct Cyc_Absyn_EnumType_struct*)
_tmp79B)->f2;_tmp7A4=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_struct*)
_tmp79B)->f2;_LL51F: if(*_tmp7A4 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*
_tmp7A4))->fields == 0){struct _handler_cons _tmp803;_push_handler(& _tmp803);{int
_tmp805=0;if(setjmp(_tmp803.handler))_tmp805=1;if(!_tmp805){{struct Cyc_Absyn_Enumdecl**
ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp7A2);*_tmp7A4=(struct Cyc_Absyn_Enumdecl*)*
ed;};_pop_handler();}else{void*_tmp804=(void*)_exn_thrown;void*_tmp807=_tmp804;
_LL557: if(_tmp807 != Cyc_Dict_Absent)goto _LL559;_LL558: {struct Cyc_Tcenv_Genv*
_tmp808=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))
Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Enumdecl*_tmpEC1;struct Cyc_Absyn_Enumdecl*
_tmp809=(_tmpEC1=_cycalloc(sizeof(*_tmpEC1)),((_tmpEC1->sc=(void*)((void*)3),((
_tmpEC1->name=_tmp7A2,((_tmpEC1->fields=0,_tmpEC1)))))));Cyc_Tc_tcEnumdecl(te,
_tmp808,loc,_tmp809);{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,
loc,_tmp7A2);*_tmp7A4=(struct Cyc_Absyn_Enumdecl*)*ed;goto _LL556;}}_LL559:;_LL55A:(
void)_throw(_tmp807);_LL556:;}}}{struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)
_check_null(*_tmp7A4);*_tmp7A2=(ed->name)[_check_known_subscript_notnull(1,0)];
goto _LL515;}_LL520: if(*((int*)_tmp79B)!= 2)goto _LL522;_tmp7A5=((struct Cyc_Absyn_TunionType_struct*)
_tmp79B)->f1;_tmp7A6=_tmp7A5.tunion_info;_tmp7A7=(union Cyc_Absyn_TunionInfoU_union*)&(((
struct Cyc_Absyn_TunionType_struct*)_tmp79B)->f1).tunion_info;_tmp7A8=_tmp7A5.targs;
_tmp7A9=(struct Cyc_List_List**)&(((struct Cyc_Absyn_TunionType_struct*)_tmp79B)->f1).targs;
_tmp7AA=_tmp7A5.rgn;_tmp7AB=(struct Cyc_Core_Opt**)&(((struct Cyc_Absyn_TunionType_struct*)
_tmp79B)->f1).rgn;_LL521: {struct Cyc_List_List*_tmp80B=*_tmp7A9;{union Cyc_Absyn_TunionInfoU_union
_tmp80C=*_tmp7A7;struct Cyc_Absyn_UnknownTunionInfo _tmp80D;struct _tuple2*_tmp80E;
int _tmp80F;int _tmp810;struct Cyc_Absyn_Tuniondecl**_tmp811;struct Cyc_Absyn_Tuniondecl*
_tmp812;_LL55C: if((_tmp80C.UnknownTunion).tag != 0)goto _LL55E;_tmp80D=(_tmp80C.UnknownTunion).f1;
_tmp80E=_tmp80D.name;_tmp80F=_tmp80D.is_xtunion;_tmp810=_tmp80D.is_flat;_LL55D: {
struct Cyc_Absyn_Tuniondecl**tudp;{struct _handler_cons _tmp813;_push_handler(&
_tmp813);{int _tmp815=0;if(setjmp(_tmp813.handler))_tmp815=1;if(!_tmp815){tudp=
Cyc_Tcenv_lookup_tuniondecl(te,loc,_tmp80E);;_pop_handler();}else{void*_tmp814=(
void*)_exn_thrown;void*_tmp817=_tmp814;_LL561: if(_tmp817 != Cyc_Dict_Absent)goto
_LL563;_LL562: {struct Cyc_Tcenv_Genv*_tmp818=((struct Cyc_Tcenv_Genv*(*)(struct
Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);struct Cyc_Absyn_Tuniondecl*
_tmpEC2;struct Cyc_Absyn_Tuniondecl*_tmp819=(_tmpEC2=_cycalloc(sizeof(*_tmpEC2)),((
_tmpEC2->sc=(void*)((void*)3),((_tmpEC2->name=_tmp80E,((_tmpEC2->tvs=0,((_tmpEC2->fields=
0,((_tmpEC2->is_xtunion=_tmp80F,((_tmpEC2->is_flat=_tmp810,_tmpEC2)))))))))))));
Cyc_Tc_tcTuniondecl(te,_tmp818,loc,_tmp819);tudp=Cyc_Tcenv_lookup_tuniondecl(te,
loc,_tmp80E);if(_tmp80B != 0){{const char*_tmpECB;const char*_tmpECA;const char*
_tmpEC9;void*_tmpEC8[2];struct Cyc_String_pa_struct _tmpEC7;struct Cyc_String_pa_struct
_tmpEC6;(_tmpEC6.tag=0,((_tmpEC6.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp80E)),((_tmpEC7.tag=0,((_tmpEC7.f1=(
struct _dynforward_ptr)(_tmp80F?(struct _dynforward_ptr)((struct _dynforward_ptr)((
_tmpECA="xtunion",_tag_dynforward(_tmpECA,sizeof(char),_get_zero_arr_size_char(
_tmpECA,8))))):(struct _dynforward_ptr)((_tmpECB="tunion",_tag_dynforward(_tmpECB,
sizeof(char),_get_zero_arr_size_char(_tmpECB,7))))),((_tmpEC8[0]=& _tmpEC7,((
_tmpEC8[1]=& _tmpEC6,Cyc_Tcutil_terr(loc,((_tmpEC9="declare parameterized %s %s before using",
_tag_dynforward(_tmpEC9,sizeof(char),_get_zero_arr_size_char(_tmpEC9,41)))),
_tag_dynforward(_tmpEC8,sizeof(void*),2)))))))))))));}return cvtenv;}goto _LL560;}
_LL563:;_LL564:(void)_throw(_tmp817);_LL560:;}}}if((*tudp)->is_xtunion != _tmp80F){
const char*_tmpECF;void*_tmpECE[1];struct Cyc_String_pa_struct _tmpECD;(_tmpECD.tag=
0,((_tmpECD.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp80E)),((_tmpECE[0]=& _tmpECD,Cyc_Tcutil_terr(loc,((_tmpECF="[x]tunion is different from type declaration %s",
_tag_dynforward(_tmpECF,sizeof(char),_get_zero_arr_size_char(_tmpECF,48)))),
_tag_dynforward(_tmpECE,sizeof(void*),1)))))));}{union Cyc_Absyn_TunionInfoU_union
_tmpED0;*_tmp7A7=(union Cyc_Absyn_TunionInfoU_union)(((_tmpED0.KnownTunion).tag=1,(((
_tmpED0.KnownTunion).f1=tudp,_tmpED0))));}_tmp812=*tudp;goto _LL55F;}_LL55E: if((
_tmp80C.KnownTunion).tag != 1)goto _LL55B;_tmp811=(_tmp80C.KnownTunion).f1;_tmp812=*
_tmp811;_LL55F: if(_tmp812->is_flat  && (unsigned int)*_tmp7AB){const char*_tmpED4;
void*_tmpED3[1];struct Cyc_String_pa_struct _tmpED2;(_tmpED2.tag=0,((_tmpED2.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(*_tmp7AB))->v)),((_tmpED3[0]=& _tmpED2,Cyc_Tcutil_terr(
loc,((_tmpED4="flat tunion has region %s",_tag_dynforward(_tmpED4,sizeof(char),
_get_zero_arr_size_char(_tmpED4,26)))),_tag_dynforward(_tmpED3,sizeof(void*),1)))))));}
if(!_tmp812->is_flat  && !((unsigned int)*_tmp7AB)){struct Cyc_Core_Opt*_tmpED5;*
_tmp7AB=((_tmpED5=_cycalloc(sizeof(*_tmpED5)),((_tmpED5->v=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,0),_tmpED5))));}if((unsigned int)*_tmp7AB){
void*_tmp829=(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp7AB))->v;cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)3,_tmp829,1);}{struct Cyc_List_List*tvs=_tmp812->tvs;for(0;
_tmp80B != 0  && tvs != 0;(_tmp80B=_tmp80B->tl,tvs=tvs->tl)){void*t1=(void*)_tmp80B->hd;
void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,k1,t1,1);}if(_tmp80B != 0){const char*_tmpED9;void*_tmpED8[1];struct
Cyc_String_pa_struct _tmpED7;(_tmpED7.tag=0,((_tmpED7.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp812->name)),((_tmpED8[0]=&
_tmpED7,Cyc_Tcutil_terr(loc,((_tmpED9="too many type arguments for tunion %s",
_tag_dynforward(_tmpED9,sizeof(char),_get_zero_arr_size_char(_tmpED9,38)))),
_tag_dynforward(_tmpED8,sizeof(void*),1)))))));}if(tvs != 0){struct Cyc_List_List*
hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k1=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*_tmpEDA;hidden_ts=((
_tmpEDA=_cycalloc(sizeof(*_tmpEDA)),((_tmpEDA->hd=(void*)e,((_tmpEDA->tl=
hidden_ts,_tmpEDA))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,
1);}*_tmp7A9=Cyc_List_imp_append(*_tmp7A9,Cyc_List_imp_rev(hidden_ts));}goto
_LL55B;}_LL55B:;}goto _LL515;}_LL522: if(*((int*)_tmp79B)!= 3)goto _LL524;_tmp7AC=((
struct Cyc_Absyn_TunionFieldType_struct*)_tmp79B)->f1;_tmp7AD=_tmp7AC.field_info;
_tmp7AE=(union Cyc_Absyn_TunionFieldInfoU_union*)&(((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp79B)->f1).field_info;_tmp7AF=_tmp7AC.targs;_LL523:{union Cyc_Absyn_TunionFieldInfoU_union
_tmp82E=*_tmp7AE;struct Cyc_Absyn_UnknownTunionFieldInfo _tmp82F;struct _tuple2*
_tmp830;struct _tuple2*_tmp831;int _tmp832;struct Cyc_Absyn_Tuniondecl*_tmp833;
struct Cyc_Absyn_Tunionfield*_tmp834;_LL566: if((_tmp82E.UnknownTunionfield).tag != 
0)goto _LL568;_tmp82F=(_tmp82E.UnknownTunionfield).f1;_tmp830=_tmp82F.tunion_name;
_tmp831=_tmp82F.field_name;_tmp832=_tmp82F.is_xtunion;_LL567: {struct Cyc_Absyn_Tuniondecl*
tud;struct Cyc_Absyn_Tunionfield*tuf;{struct _handler_cons _tmp835;_push_handler(&
_tmp835);{int _tmp837=0;if(setjmp(_tmp835.handler))_tmp837=1;if(!_tmp837){*Cyc_Tcenv_lookup_tuniondecl(
te,loc,_tmp830);;_pop_handler();}else{void*_tmp836=(void*)_exn_thrown;void*
_tmp839=_tmp836;_LL56B: if(_tmp839 != Cyc_Dict_Absent)goto _LL56D;_LL56C:{const char*
_tmpEDE;void*_tmpEDD[1];struct Cyc_String_pa_struct _tmpEDC;(_tmpEDC.tag=0,((
_tmpEDC.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp830)),((_tmpEDD[0]=& _tmpEDC,Cyc_Tcutil_terr(loc,((_tmpEDE="unbound type tunion %s",
_tag_dynforward(_tmpEDE,sizeof(char),_get_zero_arr_size_char(_tmpEDE,23)))),
_tag_dynforward(_tmpEDD,sizeof(void*),1)))))));}return cvtenv;_LL56D:;_LL56E:(
void)_throw(_tmp839);_LL56A:;}}}{struct _handler_cons _tmp83D;_push_handler(&
_tmp83D);{int _tmp83F=0;if(setjmp(_tmp83D.handler))_tmp83F=1;if(!_tmp83F){{struct
_RegionHandle _tmp840=_new_region("r");struct _RegionHandle*r=& _tmp840;
_push_region(r);{void*_tmp841=Cyc_Tcenv_lookup_ordinary(r,te,loc,_tmp831);struct
Cyc_Absyn_Tuniondecl*_tmp842;struct Cyc_Absyn_Tunionfield*_tmp843;_LL570: if(*((
int*)_tmp841)!= 2)goto _LL572;_tmp842=((struct Cyc_Tcenv_TunionRes_struct*)_tmp841)->f1;
_tmp843=((struct Cyc_Tcenv_TunionRes_struct*)_tmp841)->f2;_LL571: tuf=_tmp843;tud=
_tmp842;if(tud->is_xtunion != _tmp832){const char*_tmpEE2;void*_tmpEE1[1];struct
Cyc_String_pa_struct _tmpEE0;(_tmpEE0.tag=0,((_tmpEE0.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp830)),((_tmpEE1[0]=& _tmpEE0,
Cyc_Tcutil_terr(loc,((_tmpEE2="[x]tunion is different from type declaration %s",
_tag_dynforward(_tmpEE2,sizeof(char),_get_zero_arr_size_char(_tmpEE2,48)))),
_tag_dynforward(_tmpEE1,sizeof(void*),1)))))));}goto _LL56F;_LL572:;_LL573:{const
char*_tmpEE7;void*_tmpEE6[2];struct Cyc_String_pa_struct _tmpEE5;struct Cyc_String_pa_struct
_tmpEE4;(_tmpEE4.tag=0,((_tmpEE4.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp830)),((_tmpEE5.tag=0,((_tmpEE5.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp831)),((
_tmpEE6[0]=& _tmpEE5,((_tmpEE6[1]=& _tmpEE4,Cyc_Tcutil_terr(loc,((_tmpEE7="unbound field %s in type tunion %s",
_tag_dynforward(_tmpEE7,sizeof(char),_get_zero_arr_size_char(_tmpEE7,35)))),
_tag_dynforward(_tmpEE6,sizeof(void*),2)))))))))))));}{struct Cyc_Tcutil_CVTEnv
_tmp84B=cvtenv;_npop_handler(1);return _tmp84B;}_LL56F:;};_pop_region(r);};
_pop_handler();}else{void*_tmp83E=(void*)_exn_thrown;void*_tmp84D=_tmp83E;_LL575:
if(_tmp84D != Cyc_Dict_Absent)goto _LL577;_LL576:{const char*_tmpEEC;void*_tmpEEB[2];
struct Cyc_String_pa_struct _tmpEEA;struct Cyc_String_pa_struct _tmpEE9;(_tmpEE9.tag=
0,((_tmpEE9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp830)),((_tmpEEA.tag=0,((_tmpEEA.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp831)),((_tmpEEB[0]=& _tmpEEA,((
_tmpEEB[1]=& _tmpEE9,Cyc_Tcutil_terr(loc,((_tmpEEC="unbound field %s in type tunion %s",
_tag_dynforward(_tmpEEC,sizeof(char),_get_zero_arr_size_char(_tmpEEC,35)))),
_tag_dynforward(_tmpEEB,sizeof(void*),2)))))))))))));}return cvtenv;_LL577:;
_LL578:(void)_throw(_tmp84D);_LL574:;}}}{union Cyc_Absyn_TunionFieldInfoU_union
_tmpEED;*_tmp7AE=(union Cyc_Absyn_TunionFieldInfoU_union)(((_tmpEED.KnownTunionfield).tag=
1,(((_tmpEED.KnownTunionfield).f1=tud,(((_tmpEED.KnownTunionfield).f2=tuf,
_tmpEED))))));}_tmp833=tud;_tmp834=tuf;goto _LL569;}_LL568: if((_tmp82E.KnownTunionfield).tag
!= 1)goto _LL565;_tmp833=(_tmp82E.KnownTunionfield).f1;_tmp834=(_tmp82E.KnownTunionfield).f2;
_LL569: {struct Cyc_List_List*tvs=_tmp833->tvs;for(0;_tmp7AF != 0  && tvs != 0;(
_tmp7AF=_tmp7AF->tl,tvs=tvs->tl)){void*t1=(void*)_tmp7AF->hd;void*k1=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k1,t1,1);}if(_tmp7AF != 0){const char*_tmpEF2;void*_tmpEF1[2];struct Cyc_String_pa_struct
_tmpEF0;struct Cyc_String_pa_struct _tmpEEF;(_tmpEEF.tag=0,((_tmpEEF.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp834->name)),((
_tmpEF0.tag=0,((_tmpEF0.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp833->name)),((_tmpEF1[0]=& _tmpEF0,((_tmpEF1[1]=& _tmpEEF,Cyc_Tcutil_terr(loc,((
_tmpEF2="too many type arguments for tunion %s.%s",_tag_dynforward(_tmpEF2,
sizeof(char),_get_zero_arr_size_char(_tmpEF2,41)))),_tag_dynforward(_tmpEF1,
sizeof(void*),2)))))))))))));}if(tvs != 0){const char*_tmpEF7;void*_tmpEF6[2];
struct Cyc_String_pa_struct _tmpEF5;struct Cyc_String_pa_struct _tmpEF4;(_tmpEF4.tag=
0,((_tmpEF4.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp834->name)),((_tmpEF5.tag=0,((_tmpEF5.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp833->name)),((_tmpEF6[0]=& _tmpEF5,((
_tmpEF6[1]=& _tmpEF4,Cyc_Tcutil_terr(loc,((_tmpEF7="too few type arguments for tunion %s.%s",
_tag_dynforward(_tmpEF7,sizeof(char),_get_zero_arr_size_char(_tmpEF7,40)))),
_tag_dynforward(_tmpEF6,sizeof(void*),2)))))))))))));}goto _LL565;}_LL565:;}goto
_LL515;_LL524: if(*((int*)_tmp79B)!= 4)goto _LL526;_tmp7B0=((struct Cyc_Absyn_PointerType_struct*)
_tmp79B)->f1;_tmp7B1=(void*)_tmp7B0.elt_typ;_tmp7B2=_tmp7B0.elt_tq;_tmp7B3=(
struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_struct*)_tmp79B)->f1).elt_tq;
_tmp7B4=_tmp7B0.ptr_atts;_tmp7B5=(void*)_tmp7B4.rgn;_tmp7B6=_tmp7B4.nullable;
_tmp7B7=_tmp7B4.bounds;_tmp7B8=_tmp7B4.zero_term;_LL525: {int is_zero_terminated;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp7B1,1);_tmp7B3->real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7B3->print_const,_tmp7B1);cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,_tmp7B5,1);{union Cyc_Absyn_Constraint_union
_tmp85B=(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp7B8))->v;int _tmp85C;_LL57A: if((_tmp85B.No_constr).tag != 2)goto _LL57C;_LL57B:{
void*_tmp85D=Cyc_Tcutil_compress(_tmp7B1);void*_tmp85E;_LL581: if(_tmp85D <= (void*)
4)goto _LL583;if(*((int*)_tmp85D)!= 5)goto _LL583;_tmp85E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp85D)->f2;if((int)_tmp85E != 0)goto _LL583;_LL582:((int(*)(int(*cmp)(int,int),
struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,
_tmp7B8,Cyc_Absyn_true_conref);is_zero_terminated=1;goto _LL580;_LL583:;_LL584:((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp7B8,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL580;
_LL580:;}goto _LL579;_LL57C: if((_tmp85B.Eq_constr).tag != 0)goto _LL57E;_tmp85C=(
_tmp85B.Eq_constr).f1;if(_tmp85C != 1)goto _LL57E;_LL57D: if(!Cyc_Tcutil_admits_zero(
_tmp7B1)){const char*_tmpEFB;void*_tmpEFA[1];struct Cyc_String_pa_struct _tmpEF9;(
_tmpEF9.tag=0,((_tmpEF9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
_tmp7B1)),((_tmpEFA[0]=& _tmpEF9,Cyc_Tcutil_terr(loc,((_tmpEFB="cannot have a pointer to zero-terminated %s elements",
_tag_dynforward(_tmpEFB,sizeof(char),_get_zero_arr_size_char(_tmpEFB,53)))),
_tag_dynforward(_tmpEFA,sizeof(void*),1)))))));}is_zero_terminated=1;goto _LL579;
_LL57E:;_LL57F: is_zero_terminated=0;goto _LL579;_LL579:;}{union Cyc_Absyn_Constraint_union
_tmp862=(Cyc_Absyn_compress_conref(_tmp7B7))->v;void*_tmp863;void*_tmp864;void*
_tmp865;struct Cyc_Absyn_Exp*_tmp866;void*_tmp867;void*_tmp868;_LL586: if((_tmp862.No_constr).tag
!= 2)goto _LL588;_LL587: goto _LL589;_LL588: if((_tmp862.Eq_constr).tag != 0)goto
_LL58A;_tmp863=(_tmp862.Eq_constr).f1;if((int)_tmp863 != 0)goto _LL58A;_LL589: goto
_LL585;_LL58A: if((_tmp862.Eq_constr).tag != 0)goto _LL58C;_tmp864=(_tmp862.Eq_constr).f1;
if((int)_tmp864 != 1)goto _LL58C;_LL58B: goto _LL585;_LL58C: if((_tmp862.Eq_constr).tag
!= 0)goto _LL58E;_tmp865=(_tmp862.Eq_constr).f1;if(_tmp865 <= (void*)2)goto _LL58E;
if(*((int*)_tmp865)!= 0)goto _LL58E;_tmp866=((struct Cyc_Absyn_Upper_b_struct*)
_tmp865)->f1;_LL58D: Cyc_Tcexp_tcExp(te,0,_tmp866);if(!Cyc_Tcutil_coerce_uint_typ(
te,_tmp866)){const char*_tmpEFE;void*_tmpEFD;(_tmpEFD=0,Cyc_Tcutil_terr(loc,((
_tmpEFE="pointer bounds expression is not an unsigned int",_tag_dynforward(
_tmpEFE,sizeof(char),_get_zero_arr_size_char(_tmpEFE,49)))),_tag_dynforward(
_tmpEFD,sizeof(void*),0)));}{unsigned int _tmp86C;int _tmp86D;struct _tuple7 _tmp86B=
Cyc_Evexp_eval_const_uint_exp(_tmp866);_tmp86C=_tmp86B.f1;_tmp86D=_tmp86B.f2;if(
is_zero_terminated  && (!_tmp86D  || _tmp86C < 1)){const char*_tmpF01;void*_tmpF00;(
_tmpF00=0,Cyc_Tcutil_terr(loc,((_tmpF01="zero-terminated pointer cannot point to empty sequence",
_tag_dynforward(_tmpF01,sizeof(char),_get_zero_arr_size_char(_tmpF01,55)))),
_tag_dynforward(_tmpF00,sizeof(void*),0)));}goto _LL585;}_LL58E: if((_tmp862.Eq_constr).tag
!= 0)goto _LL590;_tmp867=(_tmp862.Eq_constr).f1;if(_tmp867 <= (void*)2)goto _LL590;
if(*((int*)_tmp867)!= 1)goto _LL590;_tmp868=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp867)->f1;_LL58F: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)7,
_tmp868,1);goto _LL585;_LL590: if((_tmp862.Forward_constr).tag != 1)goto _LL585;
_LL591: {const char*_tmpF04;void*_tmpF03;(_tmpF03=0,((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmpF04="forward constraint",
_tag_dynforward(_tmpF04,sizeof(char),_get_zero_arr_size_char(_tmpF04,19)))),
_tag_dynforward(_tmpF03,sizeof(void*),0)));}_LL585:;}goto _LL515;}_LL526: if(*((
int*)_tmp79B)!= 18)goto _LL528;_tmp7B9=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp79B)->f1;_LL527: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)7,
_tmp7B9,1);goto _LL515;_LL528: if(*((int*)_tmp79B)!= 14)goto _LL52A;_tmp7BA=(void*)((
struct Cyc_Absyn_SizeofType_struct*)_tmp79B)->f1;_LL529: cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)0,_tmp7BA,1);goto _LL515;_LL52A: if(*((int*)_tmp79B)!= 5)goto
_LL52C;_LL52B: goto _LL52D;_LL52C: if((int)_tmp79B != 1)goto _LL52E;_LL52D: goto _LL52F;
_LL52E: if(_tmp79B <= (void*)4)goto _LL53E;if(*((int*)_tmp79B)!= 6)goto _LL530;
_LL52F: goto _LL515;_LL530: if(*((int*)_tmp79B)!= 7)goto _LL532;_tmp7BB=((struct Cyc_Absyn_ArrayType_struct*)
_tmp79B)->f1;_tmp7BC=(void*)_tmp7BB.elt_type;_tmp7BD=_tmp7BB.tq;_tmp7BE=(struct
Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_struct*)_tmp79B)->f1).tq;_tmp7BF=
_tmp7BB.num_elts;_tmp7C0=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_struct*)
_tmp79B)->f1).num_elts;_tmp7C1=_tmp7BB.zero_term;_tmp7C2=_tmp7BB.zt_loc;_LL531: {
struct Cyc_Absyn_Exp*_tmp872=*_tmp7C0;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)1,_tmp7BC,1);_tmp7BE->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp7BE->print_const,_tmp7BC);{int is_zero_terminated;{union Cyc_Absyn_Constraint_union
_tmp873=(((struct Cyc_Absyn_Conref*(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(
_tmp7C1))->v;int _tmp874;_LL593: if((_tmp873.No_constr).tag != 2)goto _LL595;_LL594:((
int(*)(int(*cmp)(int,int),struct Cyc_Absyn_Conref*x,struct Cyc_Absyn_Conref*y))Cyc_Tcutil_unify_conrefs)(
Cyc_Core_intcmp,_tmp7C1,Cyc_Absyn_false_conref);is_zero_terminated=0;goto _LL592;
_LL595: if((_tmp873.Eq_constr).tag != 0)goto _LL597;_tmp874=(_tmp873.Eq_constr).f1;
if(_tmp874 != 1)goto _LL597;_LL596: if(!Cyc_Tcutil_admits_zero(_tmp7BC)){const char*
_tmpF08;void*_tmpF07[1];struct Cyc_String_pa_struct _tmpF06;(_tmpF06.tag=0,((
_tmpF06.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
_tmp7BC)),((_tmpF07[0]=& _tmpF06,Cyc_Tcutil_terr(loc,((_tmpF08="cannot have a zero-terminated array of %s elements",
_tag_dynforward(_tmpF08,sizeof(char),_get_zero_arr_size_char(_tmpF08,51)))),
_tag_dynforward(_tmpF07,sizeof(void*),1)))))));}is_zero_terminated=1;goto _LL592;
_LL597:;_LL598: is_zero_terminated=0;goto _LL592;_LL592:;}if(_tmp872 == 0){if(
is_zero_terminated)*_tmp7C0=(_tmp872=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,
0));else{{const char*_tmpF0B;void*_tmpF0A;(_tmpF0A=0,Cyc_Tcutil_warn(loc,((
_tmpF0B="array bound defaults to 1 here",_tag_dynforward(_tmpF0B,sizeof(char),
_get_zero_arr_size_char(_tmpF0B,31)))),_tag_dynforward(_tmpF0A,sizeof(void*),0)));}*
_tmp7C0=(_tmp872=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(1,0));}}Cyc_Tcexp_tcExp(
te,0,(struct Cyc_Absyn_Exp*)_tmp872);if(!Cyc_Tcutil_is_const_exp(te,(struct Cyc_Absyn_Exp*)
_tmp872)){const char*_tmpF0E;void*_tmpF0D;(_tmpF0D=0,Cyc_Tcutil_terr(loc,((
_tmpF0E="array bounds expression is not constant",_tag_dynforward(_tmpF0E,
sizeof(char),_get_zero_arr_size_char(_tmpF0E,40)))),_tag_dynforward(_tmpF0D,
sizeof(void*),0)));}if(!Cyc_Tcutil_coerce_uint_typ(te,(struct Cyc_Absyn_Exp*)
_tmp872)){const char*_tmpF11;void*_tmpF10;(_tmpF10=0,Cyc_Tcutil_terr(loc,((
_tmpF11="array bounds expression is not an unsigned int",_tag_dynforward(_tmpF11,
sizeof(char),_get_zero_arr_size_char(_tmpF11,47)))),_tag_dynforward(_tmpF10,
sizeof(void*),0)));}{unsigned int _tmp87F;int _tmp880;struct _tuple7 _tmp87E=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp872);_tmp87F=_tmp87E.f1;_tmp880=_tmp87E.f2;if((
is_zero_terminated  && _tmp880) && _tmp87F < 1){const char*_tmpF14;void*_tmpF13;(
_tmpF13=0,Cyc_Tcutil_warn(loc,((_tmpF14="zero terminated array cannot have zero elements",
_tag_dynforward(_tmpF14,sizeof(char),_get_zero_arr_size_char(_tmpF14,48)))),
_tag_dynforward(_tmpF13,sizeof(void*),0)));}if((_tmp880  && _tmp87F < 1) && Cyc_Cyclone_tovc_r){{
const char*_tmpF17;void*_tmpF16;(_tmpF16=0,Cyc_Tcutil_warn(loc,((_tmpF17="arrays with 0 elements are not supported except with gcc -- changing to 1.",
_tag_dynforward(_tmpF17,sizeof(char),_get_zero_arr_size_char(_tmpF17,75)))),
_tag_dynforward(_tmpF16,sizeof(void*),0)));}*_tmp7C0=(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(
1,0);}goto _LL515;}}}_LL532: if(*((int*)_tmp79B)!= 8)goto _LL534;_tmp7C3=((struct
Cyc_Absyn_FnType_struct*)_tmp79B)->f1;_tmp7C4=_tmp7C3.tvars;_tmp7C5=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_FnType_struct*)_tmp79B)->f1).tvars;_tmp7C6=_tmp7C3.effect;
_tmp7C7=(struct Cyc_Core_Opt**)&(((struct Cyc_Absyn_FnType_struct*)_tmp79B)->f1).effect;
_tmp7C8=(void*)_tmp7C3.ret_typ;_tmp7C9=_tmp7C3.args;_tmp7CA=_tmp7C3.c_varargs;
_tmp7CB=_tmp7C3.cyc_varargs;_tmp7CC=_tmp7C3.rgn_po;_tmp7CD=_tmp7C3.attributes;
_LL533: {int num_convs=0;int seen_cdecl=0;int seen_stdcall=0;int seen_fastcall=0;int
seen_format=0;void*ft=(void*)0;int fmt_desc_arg=- 1;int fmt_arg_start=- 1;for(0;
_tmp7CD != 0;_tmp7CD=_tmp7CD->tl){if(!Cyc_Absyn_fntype_att((void*)_tmp7CD->hd)){
const char*_tmpF1B;void*_tmpF1A[1];struct Cyc_String_pa_struct _tmpF19;(_tmpF19.tag=
0,((_tmpF19.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absyn_attribute2string((
void*)_tmp7CD->hd)),((_tmpF1A[0]=& _tmpF19,Cyc_Tcutil_terr(loc,((_tmpF1B="bad function type attribute %s",
_tag_dynforward(_tmpF1B,sizeof(char),_get_zero_arr_size_char(_tmpF1B,31)))),
_tag_dynforward(_tmpF1A,sizeof(void*),1)))))));}{void*_tmp888=(void*)_tmp7CD->hd;
void*_tmp889;int _tmp88A;int _tmp88B;_LL59A: if((int)_tmp888 != 0)goto _LL59C;_LL59B:
if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL599;_LL59C: if((int)_tmp888
!= 1)goto _LL59E;_LL59D: if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL599;
_LL59E: if((int)_tmp888 != 2)goto _LL5A0;_LL59F: if(!seen_fastcall){seen_fastcall=1;
++ num_convs;}goto _LL599;_LL5A0: if(_tmp888 <= (void*)17)goto _LL5A2;if(*((int*)
_tmp888)!= 3)goto _LL5A2;_tmp889=(void*)((struct Cyc_Absyn_Format_att_struct*)
_tmp888)->f1;_tmp88A=((struct Cyc_Absyn_Format_att_struct*)_tmp888)->f2;_tmp88B=((
struct Cyc_Absyn_Format_att_struct*)_tmp888)->f3;_LL5A1: if(!seen_format){
seen_format=1;ft=_tmp889;fmt_desc_arg=_tmp88A;fmt_arg_start=_tmp88B;}else{const
char*_tmpF1E;void*_tmpF1D;(_tmpF1D=0,Cyc_Tcutil_terr(loc,((_tmpF1E="function can't have multiple format attributes",
_tag_dynforward(_tmpF1E,sizeof(char),_get_zero_arr_size_char(_tmpF1E,47)))),
_tag_dynforward(_tmpF1D,sizeof(void*),0)));}goto _LL599;_LL5A2:;_LL5A3: goto _LL599;
_LL599:;}}if(num_convs > 1){const char*_tmpF21;void*_tmpF20;(_tmpF20=0,Cyc_Tcutil_terr(
loc,((_tmpF21="function can't have multiple calling conventions",_tag_dynforward(
_tmpF21,sizeof(char),_get_zero_arr_size_char(_tmpF21,49)))),_tag_dynforward(
_tmpF20,sizeof(void*),0)));}Cyc_Tcutil_check_unique_tvars(loc,*_tmp7C5);{struct
Cyc_List_List*b=*_tmp7C5;for(0;b != 0;b=b->tl){((struct Cyc_Absyn_Tvar*)b->hd)->identity=
Cyc_Tcutil_new_tvar_id();cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(
struct Cyc_Absyn_Tvar*)b->hd);{void*_tmp890=Cyc_Absyn_compress_kb((void*)((struct
Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp891;_LL5A5: if(*((int*)_tmp890)!= 0)goto
_LL5A7;_tmp891=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp890)->f1;if((int)
_tmp891 != 1)goto _LL5A7;_LL5A6:{const char*_tmpF25;void*_tmpF24[1];struct Cyc_String_pa_struct
_tmpF23;(_tmpF23.tag=0,((_tmpF23.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name),((_tmpF24[0]=& _tmpF23,Cyc_Tcutil_terr(
loc,((_tmpF25="function attempts to abstract Mem type variable %s",
_tag_dynforward(_tmpF25,sizeof(char),_get_zero_arr_size_char(_tmpF25,51)))),
_tag_dynforward(_tmpF24,sizeof(void*),1)))))));}goto _LL5A4;_LL5A7:;_LL5A8: goto
_LL5A4;_LL5A4:;}}}{struct _RegionHandle _tmp895=_new_region("newr");struct
_RegionHandle*newr=& _tmp895;_push_region(newr);{struct Cyc_Tcutil_CVTEnv _tmpF26;
struct Cyc_Tcutil_CVTEnv _tmp896=(_tmpF26.r=newr,((_tmpF26.kind_env=cvtenv.kind_env,((
_tmpF26.free_vars=0,((_tmpF26.free_evars=0,((_tmpF26.generalize_evars=cvtenv.generalize_evars,((
_tmpF26.fn_result=1,_tmpF26)))))))))));_tmp896=Cyc_Tcutil_i_check_valid_type(loc,
te,_tmp896,(void*)1,_tmp7C8,1);_tmp896.fn_result=0;{struct Cyc_List_List*a=
_tmp7C9;for(0;a != 0;a=a->tl){struct _tuple3*_tmp897=(struct _tuple3*)a->hd;void*
_tmp898=(*_tmp897).f3;_tmp896=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp896,(void*)
1,_tmp898,1);((*_tmp897).f2).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmp897).f2).print_const,_tmp898);}}if(_tmp7CB != 0){if(_tmp7CA){const char*
_tmpF29;void*_tmpF28;(_tmpF28=0,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmpF29="both c_vararg and cyc_vararg",
_tag_dynforward(_tmpF29,sizeof(char),_get_zero_arr_size_char(_tmpF29,29)))),
_tag_dynforward(_tmpF28,sizeof(void*),0)));}{void*_tmp89C;int _tmp89D;struct Cyc_Absyn_VarargInfo
_tmp89B=*_tmp7CB;_tmp89C=(void*)_tmp89B.type;_tmp89D=_tmp89B.inject;_tmp896=Cyc_Tcutil_i_check_valid_type(
loc,te,_tmp896,(void*)1,_tmp89C,1);(_tmp7CB->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,(_tmp7CB->tq).print_const,_tmp89C);if(_tmp89D){void*_tmp89E=Cyc_Tcutil_compress(
_tmp89C);struct Cyc_Absyn_TunionInfo _tmp89F;union Cyc_Absyn_TunionInfoU_union
_tmp8A0;struct Cyc_Absyn_Tuniondecl**_tmp8A1;_LL5AA: if(_tmp89E <= (void*)4)goto
_LL5AC;if(*((int*)_tmp89E)!= 2)goto _LL5AC;_tmp89F=((struct Cyc_Absyn_TunionType_struct*)
_tmp89E)->f1;_tmp8A0=_tmp89F.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp89E)->f1).tunion_info).KnownTunion).tag != 1)goto _LL5AC;_tmp8A1=(_tmp8A0.KnownTunion).f1;
_LL5AB: if((*_tmp8A1)->is_flat){const char*_tmpF2C;void*_tmpF2B;(_tmpF2B=0,Cyc_Tcutil_terr(
loc,((_tmpF2C="cant inject into a flat tunion",_tag_dynforward(_tmpF2C,sizeof(
char),_get_zero_arr_size_char(_tmpF2C,31)))),_tag_dynforward(_tmpF2B,sizeof(void*),
0)));}goto _LL5A9;_LL5AC:;_LL5AD:{const char*_tmpF2F;void*_tmpF2E;(_tmpF2E=0,Cyc_Tcutil_terr(
loc,((_tmpF2F="can't inject a non-[x]tunion type",_tag_dynforward(_tmpF2F,
sizeof(char),_get_zero_arr_size_char(_tmpF2F,34)))),_tag_dynforward(_tmpF2E,
sizeof(void*),0)));}goto _LL5A9;_LL5A9:;}}}if(seen_format){int _tmp8A6=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp7C9);if((((fmt_desc_arg < 0  || 
fmt_desc_arg > _tmp8A6) || fmt_arg_start < 0) || _tmp7CB == 0  && fmt_arg_start != 0)
 || _tmp7CB != 0  && fmt_arg_start != _tmp8A6 + 1){const char*_tmpF32;void*_tmpF31;(
_tmpF31=0,Cyc_Tcutil_terr(loc,((_tmpF32="bad format descriptor",_tag_dynforward(
_tmpF32,sizeof(char),_get_zero_arr_size_char(_tmpF32,22)))),_tag_dynforward(
_tmpF31,sizeof(void*),0)));}else{void*_tmp8AA;struct _tuple3 _tmp8A9=*((struct
_tuple3*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp7C9,fmt_desc_arg - 1);
_tmp8AA=_tmp8A9.f3;{void*_tmp8AB=Cyc_Tcutil_compress(_tmp8AA);struct Cyc_Absyn_PtrInfo
_tmp8AC;void*_tmp8AD;struct Cyc_Absyn_PtrAtts _tmp8AE;struct Cyc_Absyn_Conref*
_tmp8AF;struct Cyc_Absyn_Conref*_tmp8B0;_LL5AF: if(_tmp8AB <= (void*)4)goto _LL5B1;
if(*((int*)_tmp8AB)!= 4)goto _LL5B1;_tmp8AC=((struct Cyc_Absyn_PointerType_struct*)
_tmp8AB)->f1;_tmp8AD=(void*)_tmp8AC.elt_typ;_tmp8AE=_tmp8AC.ptr_atts;_tmp8AF=
_tmp8AE.bounds;_tmp8B0=_tmp8AE.zero_term;_LL5B0:{struct _tuple0 _tmpF33;struct
_tuple0 _tmp8B2=(_tmpF33.f1=Cyc_Tcutil_compress(_tmp8AD),((_tmpF33.f2=Cyc_Absyn_conref_def((
void*)((void*)0),_tmp8AF),_tmpF33)));void*_tmp8B3;void*_tmp8B4;void*_tmp8B5;void*
_tmp8B6;_LL5B4: _tmp8B3=_tmp8B2.f1;if(_tmp8B3 <= (void*)4)goto _LL5B6;if(*((int*)
_tmp8B3)!= 5)goto _LL5B6;_tmp8B4=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp8B3)->f1;
if((int)_tmp8B4 != 2)goto _LL5B6;_tmp8B5=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp8B3)->f2;if((int)_tmp8B5 != 0)goto _LL5B6;_tmp8B6=_tmp8B2.f2;if((int)_tmp8B6 != 
0)goto _LL5B6;_LL5B5: goto _LL5B3;_LL5B6:;_LL5B7:{const char*_tmpF36;void*_tmpF35;(
_tmpF35=0,Cyc_Tcutil_terr(loc,((_tmpF36="format descriptor is not a char ? type",
_tag_dynforward(_tmpF36,sizeof(char),_get_zero_arr_size_char(_tmpF36,39)))),
_tag_dynforward(_tmpF35,sizeof(void*),0)));}goto _LL5B3;_LL5B3:;}goto _LL5AE;
_LL5B1:;_LL5B2:{const char*_tmpF39;void*_tmpF38;(_tmpF38=0,Cyc_Tcutil_terr(loc,((
_tmpF39="format descriptor is not a char ? type",_tag_dynforward(_tmpF39,sizeof(
char),_get_zero_arr_size_char(_tmpF39,39)))),_tag_dynforward(_tmpF38,sizeof(void*),
0)));}goto _LL5AE;_LL5AE:;}if(fmt_arg_start != 0){int problem;{struct _tuple0 _tmpF3A;
struct _tuple0 _tmp8BC=(_tmpF3A.f1=ft,((_tmpF3A.f2=Cyc_Tcutil_compress((void*)((
struct Cyc_Absyn_VarargInfo*)_check_null(_tmp7CB))->type),_tmpF3A)));void*_tmp8BD;
void*_tmp8BE;struct Cyc_Absyn_TunionInfo _tmp8BF;union Cyc_Absyn_TunionInfoU_union
_tmp8C0;struct Cyc_Absyn_Tuniondecl**_tmp8C1;struct Cyc_Absyn_Tuniondecl*_tmp8C2;
void*_tmp8C3;void*_tmp8C4;struct Cyc_Absyn_TunionInfo _tmp8C5;union Cyc_Absyn_TunionInfoU_union
_tmp8C6;struct Cyc_Absyn_Tuniondecl**_tmp8C7;struct Cyc_Absyn_Tuniondecl*_tmp8C8;
_LL5B9: _tmp8BD=_tmp8BC.f1;if((int)_tmp8BD != 0)goto _LL5BB;_tmp8BE=_tmp8BC.f2;if(
_tmp8BE <= (void*)4)goto _LL5BB;if(*((int*)_tmp8BE)!= 2)goto _LL5BB;_tmp8BF=((
struct Cyc_Absyn_TunionType_struct*)_tmp8BE)->f1;_tmp8C0=_tmp8BF.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp8BE)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL5BB;_tmp8C1=(_tmp8C0.KnownTunion).f1;_tmp8C2=*_tmp8C1;_LL5BA: problem=
Cyc_Absyn_qvar_cmp(_tmp8C2->name,Cyc_Absyn_tunion_print_arg_qvar)!= 0;goto _LL5B8;
_LL5BB: _tmp8C3=_tmp8BC.f1;if((int)_tmp8C3 != 1)goto _LL5BD;_tmp8C4=_tmp8BC.f2;if(
_tmp8C4 <= (void*)4)goto _LL5BD;if(*((int*)_tmp8C4)!= 2)goto _LL5BD;_tmp8C5=((
struct Cyc_Absyn_TunionType_struct*)_tmp8C4)->f1;_tmp8C6=_tmp8C5.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp8C4)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL5BD;_tmp8C7=(_tmp8C6.KnownTunion).f1;_tmp8C8=*_tmp8C7;_LL5BC: problem=
Cyc_Absyn_qvar_cmp(_tmp8C8->name,Cyc_Absyn_tunion_scanf_arg_qvar)!= 0;goto _LL5B8;
_LL5BD:;_LL5BE: problem=1;goto _LL5B8;_LL5B8:;}if(problem){const char*_tmpF3D;void*
_tmpF3C;(_tmpF3C=0,Cyc_Tcutil_terr(loc,((_tmpF3D="format attribute and vararg types don't match",
_tag_dynforward(_tmpF3D,sizeof(char),_get_zero_arr_size_char(_tmpF3D,46)))),
_tag_dynforward(_tmpF3C,sizeof(void*),0)));}}}}{struct Cyc_List_List*rpo=_tmp7CC;
for(0;rpo != 0;rpo=rpo->tl){struct _tuple0 _tmp8CC;void*_tmp8CD;void*_tmp8CE;struct
_tuple0*_tmp8CB=(struct _tuple0*)rpo->hd;_tmp8CC=*_tmp8CB;_tmp8CD=_tmp8CC.f1;
_tmp8CE=_tmp8CC.f2;_tmp896=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp896,(void*)6,
_tmp8CD,1);_tmp896=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp896,(void*)5,_tmp8CE,
1);}}if(*_tmp7C7 != 0)_tmp896=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp896,(void*)
6,(void*)((struct Cyc_Core_Opt*)_check_null(*_tmp7C7))->v,1);else{struct Cyc_List_List*
effect=0;{struct Cyc_List_List*tvs=_tmp896.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Tvar*_tmp8D0;int _tmp8D1;struct _tuple18 _tmp8CF=*((struct _tuple18*)
tvs->hd);_tmp8D0=_tmp8CF.f1;_tmp8D1=_tmp8CF.f2;if(!_tmp8D1)continue;{void*
_tmp8D2=Cyc_Absyn_compress_kb((void*)_tmp8D0->kind);struct Cyc_Core_Opt*_tmp8D3;
struct Cyc_Core_Opt**_tmp8D4;void*_tmp8D5;struct Cyc_Core_Opt*_tmp8D6;struct Cyc_Core_Opt**
_tmp8D7;void*_tmp8D8;void*_tmp8D9;void*_tmp8DA;void*_tmp8DB;struct Cyc_Core_Opt*
_tmp8DC;struct Cyc_Core_Opt**_tmp8DD;void*_tmp8DE;void*_tmp8DF;struct Cyc_Core_Opt*
_tmp8E0;struct Cyc_Core_Opt**_tmp8E1;_LL5C0: if(*((int*)_tmp8D2)!= 2)goto _LL5C2;
_tmp8D3=((struct Cyc_Absyn_Less_kb_struct*)_tmp8D2)->f1;_tmp8D4=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp8D2)->f1;_tmp8D5=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp8D2)->f2;if((int)_tmp8D5 != 5)goto _LL5C2;_LL5C1: _tmp8D7=_tmp8D4;_tmp8D8=(void*)
3;goto _LL5C3;_LL5C2: if(*((int*)_tmp8D2)!= 2)goto _LL5C4;_tmp8D6=((struct Cyc_Absyn_Less_kb_struct*)
_tmp8D2)->f1;_tmp8D7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp8D2)->f1;_tmp8D8=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8D2)->f2;if(!(
_tmp8D8 == (void*)3  || _tmp8D8 == (void*)4))goto _LL5C4;_LL5C3:*_tmp8D7=Cyc_Tcutil_kind_to_bound_opt(
_tmp8D8);_tmp8D9=_tmp8D8;goto _LL5C5;_LL5C4: if(*((int*)_tmp8D2)!= 0)goto _LL5C6;
_tmp8D9=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8D2)->f1;if(!((_tmp8D9 == (
void*)3  || _tmp8D9 == (void*)4) || _tmp8D9 == (void*)5))goto _LL5C6;_LL5C5:{struct
Cyc_Absyn_AccessEff_struct*_tmpF4C;struct Cyc_Absyn_VarType_struct*_tmpF4B;struct
Cyc_Absyn_VarType_struct _tmpF4A;struct Cyc_Absyn_AccessEff_struct _tmpF49;struct
Cyc_List_List*_tmpF48;effect=((_tmpF48=_cycalloc(sizeof(*_tmpF48)),((_tmpF48->hd=(
void*)((void*)((_tmpF4C=_cycalloc(sizeof(*_tmpF4C)),((_tmpF4C[0]=((_tmpF49.tag=
20,((_tmpF49.f1=(void*)((void*)((_tmpF4B=_cycalloc(sizeof(*_tmpF4B)),((_tmpF4B[0]=((
_tmpF4A.tag=1,((_tmpF4A.f1=_tmp8D0,_tmpF4A)))),_tmpF4B))))),_tmpF49)))),_tmpF4C))))),((
_tmpF48->tl=effect,_tmpF48))))));}goto _LL5BF;_LL5C6: if(*((int*)_tmp8D2)!= 2)goto
_LL5C8;_tmp8DA=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp8D2)->f2;if((int)
_tmp8DA != 7)goto _LL5C8;_LL5C7: goto _LL5C9;_LL5C8: if(*((int*)_tmp8D2)!= 0)goto
_LL5CA;_tmp8DB=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp8D2)->f1;if((int)
_tmp8DB != 7)goto _LL5CA;_LL5C9: goto _LL5BF;_LL5CA: if(*((int*)_tmp8D2)!= 2)goto
_LL5CC;_tmp8DC=((struct Cyc_Absyn_Less_kb_struct*)_tmp8D2)->f1;_tmp8DD=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp8D2)->f1;_tmp8DE=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp8D2)->f2;if((int)_tmp8DE != 6)goto _LL5CC;_LL5CB:*_tmp8DD=Cyc_Tcutil_kind_to_bound_opt((
void*)6);goto _LL5CD;_LL5CC: if(*((int*)_tmp8D2)!= 0)goto _LL5CE;_tmp8DF=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp8D2)->f1;if((int)_tmp8DF != 6)goto _LL5CE;_LL5CD:{
struct Cyc_Absyn_VarType_struct*_tmpF52;struct Cyc_Absyn_VarType_struct _tmpF51;
struct Cyc_List_List*_tmpF50;effect=((_tmpF50=_cycalloc(sizeof(*_tmpF50)),((
_tmpF50->hd=(void*)((void*)((_tmpF52=_cycalloc(sizeof(*_tmpF52)),((_tmpF52[0]=((
_tmpF51.tag=1,((_tmpF51.f1=_tmp8D0,_tmpF51)))),_tmpF52))))),((_tmpF50->tl=effect,
_tmpF50))))));}goto _LL5BF;_LL5CE: if(*((int*)_tmp8D2)!= 1)goto _LL5D0;_tmp8E0=((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp8D2)->f1;_tmp8E1=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp8D2)->f1;_LL5CF:{struct Cyc_Absyn_Less_kb_struct*
_tmpF58;struct Cyc_Absyn_Less_kb_struct _tmpF57;struct Cyc_Core_Opt*_tmpF56;*
_tmp8E1=((_tmpF56=_cycalloc(sizeof(*_tmpF56)),((_tmpF56->v=(void*)((void*)((
_tmpF58=_cycalloc(sizeof(*_tmpF58)),((_tmpF58[0]=((_tmpF57.tag=2,((_tmpF57.f1=0,((
_tmpF57.f2=(void*)((void*)0),_tmpF57)))))),_tmpF58))))),_tmpF56))));}goto _LL5D1;
_LL5D0:;_LL5D1:{struct Cyc_Absyn_RgnsEff_struct*_tmpF67;struct Cyc_Absyn_VarType_struct*
_tmpF66;struct Cyc_Absyn_VarType_struct _tmpF65;struct Cyc_Absyn_RgnsEff_struct
_tmpF64;struct Cyc_List_List*_tmpF63;effect=((_tmpF63=_cycalloc(sizeof(*_tmpF63)),((
_tmpF63->hd=(void*)((void*)((_tmpF67=_cycalloc(sizeof(*_tmpF67)),((_tmpF67[0]=((
_tmpF64.tag=22,((_tmpF64.f1=(void*)((void*)((_tmpF66=_cycalloc(sizeof(*_tmpF66)),((
_tmpF66[0]=((_tmpF65.tag=1,((_tmpF65.f1=_tmp8D0,_tmpF65)))),_tmpF66))))),_tmpF64)))),
_tmpF67))))),((_tmpF63->tl=effect,_tmpF63))))));}goto _LL5BF;_LL5BF:;}}}{struct
Cyc_List_List*ts=_tmp896.free_evars;for(0;ts != 0;ts=ts->tl){void*_tmp8F3;int
_tmp8F4;struct _tuple19 _tmp8F2=*((struct _tuple19*)ts->hd);_tmp8F3=_tmp8F2.f1;
_tmp8F4=_tmp8F2.f2;if(!_tmp8F4)continue;{void*_tmp8F5=Cyc_Tcutil_typ_kind(
_tmp8F3);_LL5D3: if((int)_tmp8F5 != 5)goto _LL5D5;_LL5D4: goto _LL5D6;_LL5D5: if((int)
_tmp8F5 != 4)goto _LL5D7;_LL5D6: goto _LL5D8;_LL5D7: if((int)_tmp8F5 != 3)goto _LL5D9;
_LL5D8:{struct Cyc_Absyn_AccessEff_struct*_tmpF6D;struct Cyc_Absyn_AccessEff_struct
_tmpF6C;struct Cyc_List_List*_tmpF6B;effect=((_tmpF6B=_cycalloc(sizeof(*_tmpF6B)),((
_tmpF6B->hd=(void*)((void*)((_tmpF6D=_cycalloc(sizeof(*_tmpF6D)),((_tmpF6D[0]=((
_tmpF6C.tag=20,((_tmpF6C.f1=(void*)_tmp8F3,_tmpF6C)))),_tmpF6D))))),((_tmpF6B->tl=
effect,_tmpF6B))))));}goto _LL5D2;_LL5D9: if((int)_tmp8F5 != 6)goto _LL5DB;_LL5DA:{
struct Cyc_List_List*_tmpF6E;effect=((_tmpF6E=_cycalloc(sizeof(*_tmpF6E)),((
_tmpF6E->hd=(void*)_tmp8F3,((_tmpF6E->tl=effect,_tmpF6E))))));}goto _LL5D2;_LL5DB:
if((int)_tmp8F5 != 7)goto _LL5DD;_LL5DC: goto _LL5D2;_LL5DD:;_LL5DE:{struct Cyc_Absyn_RgnsEff_struct*
_tmpF74;struct Cyc_Absyn_RgnsEff_struct _tmpF73;struct Cyc_List_List*_tmpF72;effect=((
_tmpF72=_cycalloc(sizeof(*_tmpF72)),((_tmpF72->hd=(void*)((void*)((_tmpF74=
_cycalloc(sizeof(*_tmpF74)),((_tmpF74[0]=((_tmpF73.tag=22,((_tmpF73.f1=(void*)
_tmp8F3,_tmpF73)))),_tmpF74))))),((_tmpF72->tl=effect,_tmpF72))))));}goto _LL5D2;
_LL5D2:;}}}{struct Cyc_Absyn_JoinEff_struct*_tmpF7A;struct Cyc_Absyn_JoinEff_struct
_tmpF79;struct Cyc_Core_Opt*_tmpF78;*_tmp7C7=((_tmpF78=_cycalloc(sizeof(*_tmpF78)),((
_tmpF78->v=(void*)((void*)((_tmpF7A=_cycalloc(sizeof(*_tmpF7A)),((_tmpF7A[0]=((
_tmpF79.tag=21,((_tmpF79.f1=effect,_tmpF79)))),_tmpF7A))))),_tmpF78))));}}if(*
_tmp7C5 != 0){struct Cyc_List_List*bs=*_tmp7C5;for(0;bs != 0;bs=bs->tl){void*
_tmp900=Cyc_Absyn_compress_kb((void*)((struct Cyc_Absyn_Tvar*)bs->hd)->kind);
struct Cyc_Core_Opt*_tmp901;struct Cyc_Core_Opt**_tmp902;struct Cyc_Core_Opt*
_tmp903;struct Cyc_Core_Opt**_tmp904;void*_tmp905;struct Cyc_Core_Opt*_tmp906;
struct Cyc_Core_Opt**_tmp907;void*_tmp908;struct Cyc_Core_Opt*_tmp909;struct Cyc_Core_Opt**
_tmp90A;void*_tmp90B;struct Cyc_Core_Opt*_tmp90C;struct Cyc_Core_Opt**_tmp90D;void*
_tmp90E;void*_tmp90F;_LL5E0: if(*((int*)_tmp900)!= 1)goto _LL5E2;_tmp901=((struct
Cyc_Absyn_Unknown_kb_struct*)_tmp900)->f1;_tmp902=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Unknown_kb_struct*)_tmp900)->f1;_LL5E1:{const char*_tmpF7E;void*
_tmpF7D[1];struct Cyc_String_pa_struct _tmpF7C;(_tmpF7C.tag=0,((_tmpF7C.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((
_tmpF7D[0]=& _tmpF7C,Cyc_Tcutil_warn(loc,((_tmpF7E="Type variable %s unconstrained, assuming boxed",
_tag_dynforward(_tmpF7E,sizeof(char),_get_zero_arr_size_char(_tmpF7E,47)))),
_tag_dynforward(_tmpF7D,sizeof(void*),1)))))));}_tmp904=_tmp902;goto _LL5E3;
_LL5E2: if(*((int*)_tmp900)!= 2)goto _LL5E4;_tmp903=((struct Cyc_Absyn_Less_kb_struct*)
_tmp900)->f1;_tmp904=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp900)->f1;_tmp905=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp900)->f2;if((
int)_tmp905 != 1)goto _LL5E4;_LL5E3: _tmp907=_tmp904;goto _LL5E5;_LL5E4: if(*((int*)
_tmp900)!= 2)goto _LL5E6;_tmp906=((struct Cyc_Absyn_Less_kb_struct*)_tmp900)->f1;
_tmp907=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmp900)->f1;
_tmp908=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp900)->f2;if((int)_tmp908 != 
0)goto _LL5E6;_LL5E5:*_tmp907=Cyc_Tcutil_kind_to_bound_opt((void*)2);goto _LL5DF;
_LL5E6: if(*((int*)_tmp900)!= 2)goto _LL5E8;_tmp909=((struct Cyc_Absyn_Less_kb_struct*)
_tmp900)->f1;_tmp90A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp900)->f1;_tmp90B=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp900)->f2;if((
int)_tmp90B != 5)goto _LL5E8;_LL5E7:*_tmp90A=Cyc_Tcutil_kind_to_bound_opt((void*)3);
goto _LL5DF;_LL5E8: if(*((int*)_tmp900)!= 2)goto _LL5EA;_tmp90C=((struct Cyc_Absyn_Less_kb_struct*)
_tmp900)->f1;_tmp90D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp900)->f1;_tmp90E=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp900)->f2;
_LL5E9:*_tmp90D=Cyc_Tcutil_kind_to_bound_opt(_tmp90E);goto _LL5DF;_LL5EA: if(*((
int*)_tmp900)!= 0)goto _LL5EC;_tmp90F=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp900)->f1;if((int)_tmp90F != 1)goto _LL5EC;_LL5EB:{const char*_tmpF81;void*
_tmpF80;(_tmpF80=0,Cyc_Tcutil_terr(loc,((_tmpF81="functions can't abstract M types",
_tag_dynforward(_tmpF81,sizeof(char),_get_zero_arr_size_char(_tmpF81,33)))),
_tag_dynforward(_tmpF80,sizeof(void*),0)));}goto _LL5DF;_LL5EC:;_LL5ED: goto _LL5DF;
_LL5DF:;}}cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,
_tmp896.kind_env,*_tmp7C5);_tmp896.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(
_tmp896.r,_tmp896.free_vars,*_tmp7C5);{struct Cyc_List_List*tvs=_tmp896.free_vars;
for(0;tvs != 0;tvs=tvs->tl){struct Cyc_Absyn_Tvar*_tmp916;int _tmp917;struct
_tuple18 _tmp915=*((struct _tuple18*)tvs->hd);_tmp916=_tmp915.f1;_tmp917=_tmp915.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,
_tmp916,_tmp917);}}{struct Cyc_List_List*evs=_tmp896.free_evars;for(0;evs != 0;evs=
evs->tl){void*_tmp919;int _tmp91A;struct _tuple19 _tmp918=*((struct _tuple19*)evs->hd);
_tmp919=_tmp918.f1;_tmp91A=_tmp918.f2;cvtenv.free_evars=Cyc_Tcutil_add_free_evar(
cvtenv.r,cvtenv.free_evars,_tmp919,_tmp91A);}}};_pop_region(newr);}goto _LL515;}
_LL534: if(*((int*)_tmp79B)!= 9)goto _LL536;_tmp7CE=((struct Cyc_Absyn_TupleType_struct*)
_tmp79B)->f1;_LL535: for(0;_tmp7CE != 0;_tmp7CE=_tmp7CE->tl){struct _tuple5*_tmp91C=(
struct _tuple5*)_tmp7CE->hd;cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(
void*)1,(*_tmp91C).f2,1);((*_tmp91C).f1).real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,((*_tmp91C).f1).print_const,(*_tmp91C).f2);}goto _LL515;_LL536: if(*((int*)
_tmp79B)!= 11)goto _LL538;_tmp7CF=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp79B)->f1;_tmp7D0=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp79B)->f2;_LL537:{
struct _RegionHandle _tmp91D=_new_region("aprev_rgn");struct _RegionHandle*
aprev_rgn=& _tmp91D;_push_region(aprev_rgn);{struct Cyc_List_List*prev_fields=0;
for(0;_tmp7D0 != 0;_tmp7D0=_tmp7D0->tl){struct Cyc_Absyn_Aggrfield _tmp91F;struct
_dynforward_ptr*_tmp920;struct Cyc_Absyn_Tqual _tmp921;struct Cyc_Absyn_Tqual*
_tmp922;void*_tmp923;struct Cyc_Absyn_Exp*_tmp924;struct Cyc_List_List*_tmp925;
struct Cyc_Absyn_Aggrfield*_tmp91E=(struct Cyc_Absyn_Aggrfield*)_tmp7D0->hd;
_tmp91F=*_tmp91E;_tmp920=_tmp91F.name;_tmp921=_tmp91F.tq;_tmp922=(struct Cyc_Absyn_Tqual*)&(*
_tmp91E).tq;_tmp923=(void*)_tmp91F.type;_tmp924=_tmp91F.width;_tmp925=_tmp91F.attributes;
if(((int(*)(int(*compare)(struct _dynforward_ptr*,struct _dynforward_ptr*),struct
Cyc_List_List*l,struct _dynforward_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,
_tmp920)){const char*_tmpF85;void*_tmpF84[1];struct Cyc_String_pa_struct _tmpF83;(
_tmpF83.tag=0,((_tmpF83.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
_tmp920),((_tmpF84[0]=& _tmpF83,Cyc_Tcutil_terr(loc,((_tmpF85="duplicate field %s",
_tag_dynforward(_tmpF85,sizeof(char),_get_zero_arr_size_char(_tmpF85,19)))),
_tag_dynforward(_tmpF84,sizeof(void*),1)))))));}{const char*_tmpF86;if(Cyc_strcmp((
struct _dynforward_ptr)*_tmp920,((_tmpF86="",_tag_dynforward(_tmpF86,sizeof(char),
_get_zero_arr_size_char(_tmpF86,1)))))!= 0){struct Cyc_List_List*_tmpF87;
prev_fields=((_tmpF87=_region_malloc(aprev_rgn,sizeof(*_tmpF87)),((_tmpF87->hd=
_tmp920,((_tmpF87->tl=prev_fields,_tmpF87))))));}}cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)1,_tmp923,1);_tmp922->real_const=Cyc_Tcutil_extract_const_from_typedef(
loc,_tmp922->print_const,_tmp923);if(_tmp7CF == (void*)1  && !Cyc_Tcutil_bits_only(
_tmp923)){const char*_tmpF8B;void*_tmpF8A[1];struct Cyc_String_pa_struct _tmpF89;(
_tmpF89.tag=0,((_tmpF89.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
_tmp920),((_tmpF8A[0]=& _tmpF89,Cyc_Tcutil_warn(loc,((_tmpF8B="union member %s is not `bits-only' so it can only be written and not read",
_tag_dynforward(_tmpF8B,sizeof(char),_get_zero_arr_size_char(_tmpF8B,74)))),
_tag_dynforward(_tmpF8A,sizeof(void*),1)))))));}Cyc_Tcutil_check_bitfield(loc,te,
_tmp923,_tmp924,_tmp920);Cyc_Tcutil_check_field_atts(loc,_tmp920,_tmp925);}};
_pop_region(aprev_rgn);}goto _LL515;_LL538: if(*((int*)_tmp79B)!= 10)goto _LL53A;
_tmp7D1=((struct Cyc_Absyn_AggrType_struct*)_tmp79B)->f1;_tmp7D2=_tmp7D1.aggr_info;
_tmp7D3=(union Cyc_Absyn_AggrInfoU_union*)&(((struct Cyc_Absyn_AggrType_struct*)
_tmp79B)->f1).aggr_info;_tmp7D4=_tmp7D1.targs;_tmp7D5=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_AggrType_struct*)_tmp79B)->f1).targs;_LL539:{union Cyc_Absyn_AggrInfoU_union
_tmp92E=*_tmp7D3;void*_tmp92F;struct _tuple2*_tmp930;struct Cyc_Absyn_Aggrdecl**
_tmp931;struct Cyc_Absyn_Aggrdecl*_tmp932;_LL5EF: if((_tmp92E.UnknownAggr).tag != 0)
goto _LL5F1;_tmp92F=(_tmp92E.UnknownAggr).f1;_tmp930=(_tmp92E.UnknownAggr).f2;
_LL5F0: {struct Cyc_Absyn_Aggrdecl**adp;{struct _handler_cons _tmp933;_push_handler(&
_tmp933);{int _tmp935=0;if(setjmp(_tmp933.handler))_tmp935=1;if(!_tmp935){adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp930);{union Cyc_Absyn_AggrInfoU_union _tmpF8C;*_tmp7D3=(union Cyc_Absyn_AggrInfoU_union)(((
_tmpF8C.KnownAggr).tag=1,(((_tmpF8C.KnownAggr).f1=adp,_tmpF8C))));};_pop_handler();}
else{void*_tmp934=(void*)_exn_thrown;void*_tmp938=_tmp934;_LL5F4: if(_tmp938 != 
Cyc_Dict_Absent)goto _LL5F6;_LL5F5: {struct Cyc_Tcenv_Genv*_tmp939=((struct Cyc_Tcenv_Genv*(*)(
struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmpF8D;struct Cyc_Absyn_Aggrdecl*_tmp93A=(_tmpF8D=
_cycalloc(sizeof(*_tmpF8D)),((_tmpF8D->kind=(void*)_tmp92F,((_tmpF8D->sc=(void*)((
void*)3),((_tmpF8D->name=_tmp930,((_tmpF8D->tvs=0,((_tmpF8D->impl=0,((_tmpF8D->attributes=
0,_tmpF8D)))))))))))));Cyc_Tc_tcAggrdecl(te,_tmp939,loc,_tmp93A);adp=Cyc_Tcenv_lookup_aggrdecl(
te,loc,_tmp930);{union Cyc_Absyn_AggrInfoU_union _tmpF8E;*_tmp7D3=(union Cyc_Absyn_AggrInfoU_union)(((
_tmpF8E.KnownAggr).tag=1,(((_tmpF8E.KnownAggr).f1=adp,_tmpF8E))));}if(*_tmp7D5 != 
0){{const char*_tmpF92;void*_tmpF91[1];struct Cyc_String_pa_struct _tmpF90;(_tmpF90.tag=
0,((_tmpF90.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp930)),((_tmpF91[0]=& _tmpF90,Cyc_Tcutil_terr(loc,((_tmpF92="declare parameterized type %s before using",
_tag_dynforward(_tmpF92,sizeof(char),_get_zero_arr_size_char(_tmpF92,43)))),
_tag_dynforward(_tmpF91,sizeof(void*),1)))))));}return cvtenv;}goto _LL5F3;}_LL5F6:;
_LL5F7:(void)_throw(_tmp938);_LL5F3:;}}}_tmp932=*adp;goto _LL5F2;}_LL5F1: if((
_tmp92E.KnownAggr).tag != 1)goto _LL5EE;_tmp931=(_tmp92E.KnownAggr).f1;_tmp932=*
_tmp931;_LL5F2: {struct Cyc_List_List*tvs=_tmp932->tvs;struct Cyc_List_List*ts=*
_tmp7D5;for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k,(void*)ts->hd,1);}if(ts != 0){const char*_tmpF96;void*_tmpF95[1];struct
Cyc_String_pa_struct _tmpF94;(_tmpF94.tag=0,((_tmpF94.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp932->name)),((_tmpF95[0]=&
_tmpF94,Cyc_Tcutil_terr(loc,((_tmpF96="too many parameters for type %s",
_tag_dynforward(_tmpF96,sizeof(char),_get_zero_arr_size_char(_tmpF96,32)))),
_tag_dynforward(_tmpF95,sizeof(void*),1)))))));}if(tvs != 0){struct Cyc_List_List*
hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*_tmpF97;hidden_ts=((
_tmpF97=_cycalloc(sizeof(*_tmpF97)),((_tmpF97->hd=(void*)e,((_tmpF97->tl=
hidden_ts,_tmpF97))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,
1);}*_tmp7D5=Cyc_List_imp_append(*_tmp7D5,Cyc_List_imp_rev(hidden_ts));}}_LL5EE:;}
goto _LL515;_LL53A: if(*((int*)_tmp79B)!= 17)goto _LL53C;_tmp7D6=((struct Cyc_Absyn_TypedefType_struct*)
_tmp79B)->f1;_tmp7D7=((struct Cyc_Absyn_TypedefType_struct*)_tmp79B)->f2;_tmp7D8=(
struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_struct*)_tmp79B)->f2;
_tmp7D9=((struct Cyc_Absyn_TypedefType_struct*)_tmp79B)->f3;_tmp7DA=(struct Cyc_Absyn_Typedefdecl**)&((
struct Cyc_Absyn_TypedefType_struct*)_tmp79B)->f3;_tmp7DB=((struct Cyc_Absyn_TypedefType_struct*)
_tmp79B)->f4;_tmp7DC=(void***)&((struct Cyc_Absyn_TypedefType_struct*)_tmp79B)->f4;
_LL53B: {struct Cyc_List_List*targs=*_tmp7D8;struct Cyc_Absyn_Typedefdecl*td;{
struct _handler_cons _tmp944;_push_handler(& _tmp944);{int _tmp946=0;if(setjmp(
_tmp944.handler))_tmp946=1;if(!_tmp946){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,
_tmp7D6);;_pop_handler();}else{void*_tmp945=(void*)_exn_thrown;void*_tmp948=
_tmp945;_LL5F9: if(_tmp948 != Cyc_Dict_Absent)goto _LL5FB;_LL5FA:{const char*_tmpF9B;
void*_tmpF9A[1];struct Cyc_String_pa_struct _tmpF99;(_tmpF99.tag=0,((_tmpF99.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp7D6)),((
_tmpF9A[0]=& _tmpF99,Cyc_Tcutil_terr(loc,((_tmpF9B="unbound typedef name %s",
_tag_dynforward(_tmpF9B,sizeof(char),_get_zero_arr_size_char(_tmpF9B,24)))),
_tag_dynforward(_tmpF9A,sizeof(void*),1)))))));}return cvtenv;_LL5FB:;_LL5FC:(
void)_throw(_tmp948);_LL5F8:;}}}*_tmp7DA=(struct Cyc_Absyn_Typedefdecl*)td;
_tmp7D6[0]=(td->name)[_check_known_subscript_notnull(1,0)];{struct Cyc_List_List*
tvs=td->tvs;struct Cyc_List_List*ts=targs;{struct _RegionHandle _tmp94C=_new_region("temp");
struct _RegionHandle*temp=& _tmp94C;_push_region(temp);{struct Cyc_List_List*inst=0;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){void*k=Cyc_Tcutil_tvar_kind((
struct Cyc_Absyn_Tvar*)tvs->hd);cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,k,(void*)ts->hd,1);{struct _tuple8*_tmpF9E;struct Cyc_List_List*_tmpF9D;
inst=((_tmpF9D=_region_malloc(temp,sizeof(*_tmpF9D)),((_tmpF9D->hd=((_tmpF9E=
_region_malloc(temp,sizeof(*_tmpF9E)),((_tmpF9E->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((
_tmpF9E->f2=(void*)ts->hd,_tmpF9E)))))),((_tmpF9D->tl=inst,_tmpF9D))))));}}if(ts
!= 0){const char*_tmpFA2;void*_tmpFA1[1];struct Cyc_String_pa_struct _tmpFA0;(
_tmpFA0.tag=0,((_tmpFA0.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp7D6)),((_tmpFA1[0]=& _tmpFA0,Cyc_Tcutil_terr(loc,((_tmpFA2="too many parameters for typedef %s",
_tag_dynforward(_tmpFA2,sizeof(char),_get_zero_arr_size_char(_tmpFA2,35)))),
_tag_dynforward(_tmpFA1,sizeof(void*),1)))))));}if(tvs != 0){struct Cyc_List_List*
hidden_ts=0;for(0;tvs != 0;tvs=tvs->tl){void*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)
tvs->hd);void*e=Cyc_Absyn_new_evar(0,0);{struct Cyc_List_List*_tmpFA3;hidden_ts=((
_tmpFA3=_cycalloc(sizeof(*_tmpFA3)),((_tmpFA3->hd=(void*)e,((_tmpFA3->tl=
hidden_ts,_tmpFA3))))));}cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,
1);{struct _tuple8*_tmpFA6;struct Cyc_List_List*_tmpFA5;inst=(struct Cyc_List_List*)((
_tmpFA5=_cycalloc(sizeof(*_tmpFA5)),((_tmpFA5->hd=(struct _tuple8*)((_tmpFA6=
_cycalloc(sizeof(*_tmpFA6)),((_tmpFA6->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((
_tmpFA6->f2=e,_tmpFA6)))))),((_tmpFA5->tl=inst,_tmpFA5))))));}}*_tmp7D8=Cyc_List_imp_append(
targs,Cyc_List_imp_rev(hidden_ts));}if(td->defn != 0){void*new_typ=Cyc_Tcutil_rsubstitute(
temp,inst,(void*)((struct Cyc_Core_Opt*)_check_null(td->defn))->v);void**_tmpFA7;*
_tmp7DC=((_tmpFA7=_cycalloc(sizeof(*_tmpFA7)),((_tmpFA7[0]=new_typ,_tmpFA7))));}};
_pop_region(temp);}goto _LL515;}}_LL53C: if(*((int*)_tmp79B)!= 19)goto _LL53E;
_LL53D: goto _LL53F;_LL53E: if((int)_tmp79B != 3)goto _LL540;_LL53F: goto _LL541;_LL540:
if((int)_tmp79B != 2)goto _LL542;_LL541: goto _LL515;_LL542: if(_tmp79B <= (void*)4)
goto _LL544;if(*((int*)_tmp79B)!= 15)goto _LL544;_tmp7DD=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp79B)->f1;_LL543: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,
_tmp7DD,1);goto _LL515;_LL544: if(_tmp79B <= (void*)4)goto _LL546;if(*((int*)_tmp79B)
!= 16)goto _LL546;_tmp7DE=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp79B)->f1;
_tmp7DF=(void*)((struct Cyc_Absyn_DynRgnType_struct*)_tmp79B)->f2;_LL545: cvtenv=
Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)3,_tmp7DE,0);cvtenv=Cyc_Tcutil_i_check_valid_type(
loc,te,cvtenv,(void*)3,_tmp7DF,1);goto _LL515;_LL546: if(_tmp79B <= (void*)4)goto
_LL548;if(*((int*)_tmp79B)!= 20)goto _LL548;_tmp7E0=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp79B)->f1;_LL547: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)5,
_tmp7E0,1);goto _LL515;_LL548: if(_tmp79B <= (void*)4)goto _LL54A;if(*((int*)_tmp79B)
!= 22)goto _LL54A;_tmp7E1=(void*)((struct Cyc_Absyn_RgnsEff_struct*)_tmp79B)->f1;
_LL549: cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,(void*)0,_tmp7E1,1);
goto _LL515;_LL54A: if(_tmp79B <= (void*)4)goto _LL515;if(*((int*)_tmp79B)!= 21)goto
_LL515;_tmp7E2=((struct Cyc_Absyn_JoinEff_struct*)_tmp79B)->f1;_LL54B: for(0;
_tmp7E2 != 0;_tmp7E2=_tmp7E2->tl){cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,
cvtenv,(void*)6,(void*)_tmp7E2->hd,1);}goto _LL515;_LL515:;}if(!Cyc_Tcutil_kind_leq(
Cyc_Tcutil_typ_kind(t),expected_kind)){{void*_tmp956=t;struct Cyc_Core_Opt*
_tmp957;struct Cyc_Core_Opt*_tmp958;_LL5FE: if(_tmp956 <= (void*)4)goto _LL600;if(*((
int*)_tmp956)!= 0)goto _LL600;_tmp957=((struct Cyc_Absyn_Evar_struct*)_tmp956)->f1;
_tmp958=((struct Cyc_Absyn_Evar_struct*)_tmp956)->f2;_LL5FF: {struct
_dynforward_ptr s;{struct Cyc_Core_Opt*_tmp959=_tmp957;struct Cyc_Core_Opt _tmp95A;
void*_tmp95B;_LL603: if(_tmp959 != 0)goto _LL605;_LL604:{const char*_tmpFA8;s=((
_tmpFA8="kind=NULL ",_tag_dynforward(_tmpFA8,sizeof(char),
_get_zero_arr_size_char(_tmpFA8,11))));}goto _LL602;_LL605: if(_tmp959 == 0)goto
_LL602;_tmp95A=*_tmp959;_tmp95B=(void*)_tmp95A.v;_LL606:{const char*_tmpFAC;void*
_tmpFAB[1];struct Cyc_String_pa_struct _tmpFAA;s=(struct _dynforward_ptr)((_tmpFAA.tag=
0,((_tmpFAA.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(
_tmp95B)),((_tmpFAB[0]=& _tmpFAA,Cyc_aprintf(((_tmpFAC="kind=%s ",_tag_dynforward(
_tmpFAC,sizeof(char),_get_zero_arr_size_char(_tmpFAC,9)))),_tag_dynforward(
_tmpFAB,sizeof(void*),1))))))));}goto _LL602;_LL602:;}{struct Cyc_Core_Opt*_tmp960=
_tmp958;struct Cyc_Core_Opt _tmp961;void*_tmp962;_LL608: if(_tmp960 != 0)goto _LL60A;
_LL609:{const char*_tmpFB0;void*_tmpFAF[1];struct Cyc_String_pa_struct _tmpFAE;s=(
struct _dynforward_ptr)((_tmpFAE.tag=0,((_tmpFAE.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)s),((_tmpFAF[0]=& _tmpFAE,Cyc_aprintf(((_tmpFB0="%s ref=NULL",
_tag_dynforward(_tmpFB0,sizeof(char),_get_zero_arr_size_char(_tmpFB0,12)))),
_tag_dynforward(_tmpFAF,sizeof(void*),1))))))));}goto _LL607;_LL60A: if(_tmp960 == 
0)goto _LL607;_tmp961=*_tmp960;_tmp962=(void*)_tmp961.v;_LL60B:{const char*_tmpFB5;
void*_tmpFB4[2];struct Cyc_String_pa_struct _tmpFB3;struct Cyc_String_pa_struct
_tmpFB2;s=(struct _dynforward_ptr)((_tmpFB2.tag=0,((_tmpFB2.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp962)),((
_tmpFB3.tag=0,((_tmpFB3.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)s),((
_tmpFB4[0]=& _tmpFB3,((_tmpFB4[1]=& _tmpFB2,Cyc_aprintf(((_tmpFB5="%s ref=%s",
_tag_dynforward(_tmpFB5,sizeof(char),_get_zero_arr_size_char(_tmpFB5,10)))),
_tag_dynforward(_tmpFB4,sizeof(void*),2))))))))))))));}goto _LL607;_LL607:;}{
const char*_tmpFB9;void*_tmpFB8[1];struct Cyc_String_pa_struct _tmpFB7;(_tmpFB7.tag=
0,((_tmpFB7.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)s),((_tmpFB8[0]=&
_tmpFB7,Cyc_fprintf(Cyc_stderr,((_tmpFB9="evar info: %s\n",_tag_dynforward(
_tmpFB9,sizeof(char),_get_zero_arr_size_char(_tmpFB9,15)))),_tag_dynforward(
_tmpFB8,sizeof(void*),1)))))));}goto _LL5FD;}_LL600:;_LL601: goto _LL5FD;_LL5FD:;}{
const char*_tmpFBF;void*_tmpFBE[3];struct Cyc_String_pa_struct _tmpFBD;struct Cyc_String_pa_struct
_tmpFBC;struct Cyc_String_pa_struct _tmpFBB;(_tmpFBB.tag=0,((_tmpFBB.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(expected_kind)),((
_tmpFBC.tag=0,((_tmpFBC.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(
Cyc_Tcutil_typ_kind(t))),((_tmpFBD.tag=0,((_tmpFBD.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_Absynpp_typ2string(t)),((_tmpFBE[0]=& _tmpFBD,((_tmpFBE[
1]=& _tmpFBC,((_tmpFBE[2]=& _tmpFBB,Cyc_Tcutil_terr(loc,((_tmpFBF="type %s has kind %s but as used here needs kind %s",
_tag_dynforward(_tmpFBF,sizeof(char),_get_zero_arr_size_char(_tmpFBF,51)))),
_tag_dynforward(_tmpFBE,sizeof(void*),3)))))))))))))))))));}}return cvtenv;}
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,void*expected_kind,void*t);
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,void*expected_kind,void*t){
struct Cyc_List_List*_tmp972=cvt.kind_env;cvt=Cyc_Tcutil_i_check_valid_type(loc,
te,cvt,expected_kind,t,1);{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=
vs->tl){struct Cyc_Absyn_Tvar*_tmp974;struct _tuple18 _tmp973=*((struct _tuple18*)vs->hd);
_tmp974=_tmp973.f1;cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmp972,_tmp974);}}{
struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){void*_tmp976;
struct _tuple19 _tmp975=*((struct _tuple19*)evs->hd);_tmp976=_tmp975.f1;{void*
_tmp977=Cyc_Tcutil_compress(_tmp976);struct Cyc_Core_Opt*_tmp978;struct Cyc_Core_Opt**
_tmp979;_LL60D: if(_tmp977 <= (void*)4)goto _LL60F;if(*((int*)_tmp977)!= 0)goto
_LL60F;_tmp978=((struct Cyc_Absyn_Evar_struct*)_tmp977)->f4;_tmp979=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Evar_struct*)_tmp977)->f4;_LL60E: if(*_tmp979 == 0){struct Cyc_Core_Opt*
_tmpFC0;*_tmp979=((_tmpFC0=_cycalloc(sizeof(*_tmpFC0)),((_tmpFC0->v=_tmp972,
_tmpFC0))));}else{struct Cyc_List_List*_tmp97B=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(*_tmp979))->v;struct Cyc_List_List*_tmp97C=0;for(0;_tmp97B != 0;
_tmp97B=_tmp97B->tl){if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,
_tmp972,(struct Cyc_Absyn_Tvar*)_tmp97B->hd)){struct Cyc_List_List*_tmpFC1;_tmp97C=((
_tmpFC1=_cycalloc(sizeof(*_tmpFC1)),((_tmpFC1->hd=(struct Cyc_Absyn_Tvar*)_tmp97B->hd,((
_tmpFC1->tl=_tmp97C,_tmpFC1))))));}}{struct Cyc_Core_Opt*_tmpFC2;*_tmp979=((
_tmpFC2=_cycalloc(sizeof(*_tmpFC2)),((_tmpFC2->v=_tmp97C,_tmpFC2))));}}goto
_LL60C;_LL60F:;_LL610: goto _LL60C;_LL60C:;}}}return cvt;}void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t);void Cyc_Tcutil_check_valid_toplevel_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,void*t){int
generalize_evars=Cyc_Tcutil_is_function_type(t);struct Cyc_List_List*_tmp97F=Cyc_Tcenv_lookup_type_vars(
te);struct _RegionHandle _tmp980=_new_region("temp");struct _RegionHandle*temp=&
_tmp980;_push_region(temp);{struct Cyc_Tcutil_CVTEnv _tmpFC3;struct Cyc_Tcutil_CVTEnv
_tmp981=Cyc_Tcutil_check_valid_type(loc,te,((_tmpFC3.r=temp,((_tmpFC3.kind_env=
_tmp97F,((_tmpFC3.free_vars=0,((_tmpFC3.free_evars=0,((_tmpFC3.generalize_evars=
generalize_evars,((_tmpFC3.fn_result=0,_tmpFC3)))))))))))),(void*)1,t);struct Cyc_List_List*
_tmp982=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(
struct _tuple18*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple18*))Cyc_Core_fst,_tmp981.free_vars);struct Cyc_List_List*_tmp983=((
struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple19*),struct
Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct _tuple19*))Cyc_Core_fst,
_tmp981.free_evars);if(_tmp97F != 0){struct Cyc_List_List*_tmp984=0;{struct Cyc_List_List*
_tmp985=_tmp982;for(0;(unsigned int)_tmp985;_tmp985=_tmp985->tl){struct Cyc_Absyn_Tvar*
_tmp986=(struct Cyc_Absyn_Tvar*)_tmp985->hd;int found=0;{struct Cyc_List_List*
_tmp987=_tmp97F;for(0;(unsigned int)_tmp987;_tmp987=_tmp987->tl){if(Cyc_Absyn_tvar_cmp(
_tmp986,(struct Cyc_Absyn_Tvar*)_tmp987->hd)== 0){found=1;break;}}}if(!found){
struct Cyc_List_List*_tmpFC4;_tmp984=((_tmpFC4=_region_malloc(temp,sizeof(*
_tmpFC4)),((_tmpFC4->hd=(struct Cyc_Absyn_Tvar*)_tmp985->hd,((_tmpFC4->tl=_tmp984,
_tmpFC4))))));}}}_tmp982=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp984);}{struct Cyc_List_List*x=_tmp982;for(0;x != 0;x=x->tl){void*_tmp989=Cyc_Absyn_compress_kb((
void*)((struct Cyc_Absyn_Tvar*)x->hd)->kind);struct Cyc_Core_Opt*_tmp98A;struct Cyc_Core_Opt**
_tmp98B;struct Cyc_Core_Opt*_tmp98C;struct Cyc_Core_Opt**_tmp98D;void*_tmp98E;
struct Cyc_Core_Opt*_tmp98F;struct Cyc_Core_Opt**_tmp990;void*_tmp991;struct Cyc_Core_Opt*
_tmp992;struct Cyc_Core_Opt**_tmp993;void*_tmp994;struct Cyc_Core_Opt*_tmp995;
struct Cyc_Core_Opt**_tmp996;void*_tmp997;void*_tmp998;_LL612: if(*((int*)_tmp989)
!= 1)goto _LL614;_tmp98A=((struct Cyc_Absyn_Unknown_kb_struct*)_tmp989)->f1;
_tmp98B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)_tmp989)->f1;
_LL613: _tmp98D=_tmp98B;goto _LL615;_LL614: if(*((int*)_tmp989)!= 2)goto _LL616;
_tmp98C=((struct Cyc_Absyn_Less_kb_struct*)_tmp989)->f1;_tmp98D=(struct Cyc_Core_Opt**)&((
struct Cyc_Absyn_Less_kb_struct*)_tmp989)->f1;_tmp98E=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmp989)->f2;if((int)_tmp98E != 1)goto _LL616;_LL615: _tmp990=_tmp98D;goto _LL617;
_LL616: if(*((int*)_tmp989)!= 2)goto _LL618;_tmp98F=((struct Cyc_Absyn_Less_kb_struct*)
_tmp989)->f1;_tmp990=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp989)->f1;_tmp991=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp989)->f2;if((
int)_tmp991 != 0)goto _LL618;_LL617:*_tmp990=Cyc_Tcutil_kind_to_bound_opt((void*)2);
goto _LL611;_LL618: if(*((int*)_tmp989)!= 2)goto _LL61A;_tmp992=((struct Cyc_Absyn_Less_kb_struct*)
_tmp989)->f1;_tmp993=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp989)->f1;_tmp994=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp989)->f2;if((
int)_tmp994 != 5)goto _LL61A;_LL619:*_tmp993=Cyc_Tcutil_kind_to_bound_opt((void*)3);
goto _LL611;_LL61A: if(*((int*)_tmp989)!= 2)goto _LL61C;_tmp995=((struct Cyc_Absyn_Less_kb_struct*)
_tmp989)->f1;_tmp996=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)
_tmp989)->f1;_tmp997=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp989)->f2;
_LL61B:*_tmp996=Cyc_Tcutil_kind_to_bound_opt(_tmp997);goto _LL611;_LL61C: if(*((
int*)_tmp989)!= 0)goto _LL61E;_tmp998=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp989)->f1;if((int)_tmp998 != 1)goto _LL61E;_LL61D:{const char*_tmpFC8;void*
_tmpFC7[1];struct Cyc_String_pa_struct _tmpFC6;(_tmpFC6.tag=0,((_tmpFC6.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)
x->hd)),((_tmpFC7[0]=& _tmpFC6,Cyc_Tcutil_terr(loc,((_tmpFC8="type variable %s cannot have kind M",
_tag_dynforward(_tmpFC8,sizeof(char),_get_zero_arr_size_char(_tmpFC8,36)))),
_tag_dynforward(_tmpFC7,sizeof(void*),1)))))));}goto _LL611;_LL61E:;_LL61F: goto
_LL611;_LL611:;}}if(_tmp982 != 0  || _tmp983 != 0){{void*_tmp99C=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_FnInfo _tmp99D;struct Cyc_List_List*_tmp99E;struct Cyc_List_List**
_tmp99F;struct Cyc_Core_Opt*_tmp9A0;void*_tmp9A1;struct Cyc_List_List*_tmp9A2;int
_tmp9A3;struct Cyc_Absyn_VarargInfo*_tmp9A4;struct Cyc_List_List*_tmp9A5;struct Cyc_List_List*
_tmp9A6;_LL621: if(_tmp99C <= (void*)4)goto _LL623;if(*((int*)_tmp99C)!= 8)goto
_LL623;_tmp99D=((struct Cyc_Absyn_FnType_struct*)_tmp99C)->f1;_tmp99E=_tmp99D.tvars;
_tmp99F=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_struct*)_tmp99C)->f1).tvars;
_tmp9A0=_tmp99D.effect;_tmp9A1=(void*)_tmp99D.ret_typ;_tmp9A2=_tmp99D.args;
_tmp9A3=_tmp99D.c_varargs;_tmp9A4=_tmp99D.cyc_varargs;_tmp9A5=_tmp99D.rgn_po;
_tmp9A6=_tmp99D.attributes;_LL622: if(*_tmp99F == 0){*_tmp99F=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_copy)(_tmp982);_tmp982=0;}goto _LL620;_LL623:;
_LL624: goto _LL620;_LL620:;}if(_tmp982 != 0){const char*_tmpFCC;void*_tmpFCB[1];
struct Cyc_String_pa_struct _tmpFCA;(_tmpFCA.tag=0,((_tmpFCA.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*((struct Cyc_Absyn_Tvar*)_tmp982->hd)->name),((
_tmpFCB[0]=& _tmpFCA,Cyc_Tcutil_terr(loc,((_tmpFCC="unbound type variable %s",
_tag_dynforward(_tmpFCC,sizeof(char),_get_zero_arr_size_char(_tmpFCC,25)))),
_tag_dynforward(_tmpFCB,sizeof(void*),1)))))));}if(_tmp983 != 0)for(0;_tmp983 != 0;
_tmp983=_tmp983->tl){void*e=(void*)_tmp983->hd;void*_tmp9AA=Cyc_Tcutil_typ_kind(
e);_LL626: if((int)_tmp9AA != 4)goto _LL628;_LL627: if(!Cyc_Tcutil_unify(e,(void*)3)){
const char*_tmpFCF;void*_tmpFCE;(_tmpFCE=0,((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmpFCF="can't unify evar with unique region!",
_tag_dynforward(_tmpFCF,sizeof(char),_get_zero_arr_size_char(_tmpFCF,37)))),
_tag_dynforward(_tmpFCE,sizeof(void*),0)));}goto _LL625;_LL628: if((int)_tmp9AA != 
5)goto _LL62A;_LL629: goto _LL62B;_LL62A: if((int)_tmp9AA != 3)goto _LL62C;_LL62B: if(!
Cyc_Tcutil_unify(e,(void*)2)){const char*_tmpFD2;void*_tmpFD1;(_tmpFD1=0,((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmpFD2="can't unify evar with heap!",
_tag_dynforward(_tmpFD2,sizeof(char),_get_zero_arr_size_char(_tmpFD2,28)))),
_tag_dynforward(_tmpFD1,sizeof(void*),0)));}goto _LL625;_LL62C: if((int)_tmp9AA != 
6)goto _LL62E;_LL62D: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmpFD5;void*_tmpFD4;(_tmpFD4=0,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmpFD5="can't unify evar with {}!",
_tag_dynforward(_tmpFD5,sizeof(char),_get_zero_arr_size_char(_tmpFD5,26)))),
_tag_dynforward(_tmpFD4,sizeof(void*),0)));}goto _LL625;_LL62E:;_LL62F:{const char*
_tmpFDA;void*_tmpFD9[2];struct Cyc_String_pa_struct _tmpFD8;struct Cyc_String_pa_struct
_tmpFD7;(_tmpFD7.tag=0,((_tmpFD7.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_typ2string(t)),((_tmpFD8.tag=0,((_tmpFD8.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(e)),((_tmpFD9[0]=&
_tmpFD8,((_tmpFD9[1]=& _tmpFD7,Cyc_Tcutil_terr(loc,((_tmpFDA="hidden type variable %s isn't abstracted in type %s",
_tag_dynforward(_tmpFDA,sizeof(char),_get_zero_arr_size_char(_tmpFDA,52)))),
_tag_dynforward(_tmpFD9,sizeof(void*),2)))))))))))));}goto _LL625;_LL625:;}}};
_pop_region(temp);}void Cyc_Tcutil_check_fndecl_valid_type(struct Cyc_Position_Segment*
loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd);void Cyc_Tcutil_check_fndecl_valid_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);{
void*_tmp9B6=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmp9B7;struct Cyc_List_List*
_tmp9B8;struct Cyc_Core_Opt*_tmp9B9;void*_tmp9BA;_LL631: if(_tmp9B6 <= (void*)4)
goto _LL633;if(*((int*)_tmp9B6)!= 8)goto _LL633;_tmp9B7=((struct Cyc_Absyn_FnType_struct*)
_tmp9B6)->f1;_tmp9B8=_tmp9B7.tvars;_tmp9B9=_tmp9B7.effect;_tmp9BA=(void*)_tmp9B7.ret_typ;
_LL632: fd->tvs=_tmp9B8;fd->effect=_tmp9B9;goto _LL630;_LL633:;_LL634: {const char*
_tmpFDD;void*_tmpFDC;(_tmpFDC=0,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmpFDD="check_fndecl_valid_type: not a FnType",
_tag_dynforward(_tmpFDD,sizeof(char),_get_zero_arr_size_char(_tmpFDD,38)))),
_tag_dynforward(_tmpFDC,sizeof(void*),0)));}_LL630:;}{struct _RegionHandle _tmp9BD=
_new_region("r");struct _RegionHandle*r=& _tmp9BD;_push_region(r);{const char*
_tmpFDE;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dynforward_ptr*(*f)(struct _tuple14*),struct Cyc_List_List*x))
Cyc_List_rmap)(r,(struct _dynforward_ptr*(*)(struct _tuple14*t))Cyc_Tcutil_fst_fdarg,
fd->args),loc,((_tmpFDE="function declaration has repeated parameter",
_tag_dynforward(_tmpFDE,sizeof(char),_get_zero_arr_size_char(_tmpFDE,44)))));};
_pop_region(r);}{struct Cyc_Core_Opt*_tmpFDF;fd->cached_typ=((_tmpFDF=_cycalloc(
sizeof(*_tmpFDF)),((_tmpFDF->v=(void*)t,_tmpFDF))));}}void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,void*expected_kind,int allow_evars,void*t);void Cyc_Tcutil_check_type(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*
bound_tvars,void*expected_kind,int allow_evars,void*t){struct _RegionHandle _tmp9C0=
_new_region("r");struct _RegionHandle*r=& _tmp9C0;_push_region(r);{struct Cyc_Tcutil_CVTEnv
_tmpFE0;struct Cyc_Tcutil_CVTEnv _tmp9C1=Cyc_Tcutil_check_valid_type(loc,te,((
_tmpFE0.r=r,((_tmpFE0.kind_env=bound_tvars,((_tmpFE0.free_vars=0,((_tmpFE0.free_evars=
0,((_tmpFE0.generalize_evars=0,((_tmpFE0.fn_result=0,_tmpFE0)))))))))))),
expected_kind,t);struct Cyc_List_List*_tmp9C2=Cyc_Tcutil_remove_bound_tvars(r,((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct
_tuple18*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(
struct _tuple18*))Cyc_Core_fst,_tmp9C1.free_vars),bound_tvars);struct Cyc_List_List*
_tmp9C3=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple19*),
struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple19*))Cyc_Core_fst,
_tmp9C1.free_evars);{struct Cyc_List_List*fs=_tmp9C2;for(0;fs != 0;fs=fs->tl){
struct _dynforward_ptr*_tmp9C4=((struct Cyc_Absyn_Tvar*)fs->hd)->name;const char*
_tmpFE5;void*_tmpFE4[2];struct Cyc_String_pa_struct _tmpFE3;struct Cyc_String_pa_struct
_tmpFE2;(_tmpFE2.tag=0,((_tmpFE2.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_typ2string(t)),((_tmpFE3.tag=0,((_tmpFE3.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp9C4),((_tmpFE4[0]=& _tmpFE3,((
_tmpFE4[1]=& _tmpFE2,Cyc_Tcutil_terr(loc,((_tmpFE5="unbound type variable %s in type %s",
_tag_dynforward(_tmpFE5,sizeof(char),_get_zero_arr_size_char(_tmpFE5,36)))),
_tag_dynforward(_tmpFE4,sizeof(void*),2)))))))))))));}}if(!allow_evars  && 
_tmp9C3 != 0)for(0;_tmp9C3 != 0;_tmp9C3=_tmp9C3->tl){void*e=(void*)_tmp9C3->hd;
void*_tmp9C9=Cyc_Tcutil_typ_kind(e);_LL636: if((int)_tmp9C9 != 4)goto _LL638;_LL637:
if(!Cyc_Tcutil_unify(e,(void*)3)){const char*_tmpFE8;void*_tmpFE7;(_tmpFE7=0,((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmpFE8="can't unify evar with unique region!",_tag_dynforward(_tmpFE8,sizeof(
char),_get_zero_arr_size_char(_tmpFE8,37)))),_tag_dynforward(_tmpFE7,sizeof(void*),
0)));}goto _LL635;_LL638: if((int)_tmp9C9 != 5)goto _LL63A;_LL639: goto _LL63B;_LL63A:
if((int)_tmp9C9 != 3)goto _LL63C;_LL63B: if(!Cyc_Tcutil_unify(e,(void*)2)){const
char*_tmpFEB;void*_tmpFEA;(_tmpFEA=0,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmpFEB="can't unify evar with heap!",
_tag_dynforward(_tmpFEB,sizeof(char),_get_zero_arr_size_char(_tmpFEB,28)))),
_tag_dynforward(_tmpFEA,sizeof(void*),0)));}goto _LL635;_LL63C: if((int)_tmp9C9 != 
6)goto _LL63E;_LL63D: if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*
_tmpFEE;void*_tmpFED;(_tmpFED=0,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmpFEE="can't unify evar with {}!",
_tag_dynforward(_tmpFEE,sizeof(char),_get_zero_arr_size_char(_tmpFEE,26)))),
_tag_dynforward(_tmpFED,sizeof(void*),0)));}goto _LL635;_LL63E:;_LL63F:{const char*
_tmpFF3;void*_tmpFF2[2];struct Cyc_String_pa_struct _tmpFF1;struct Cyc_String_pa_struct
_tmpFF0;(_tmpFF0.tag=0,((_tmpFF0.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_typ2string(t)),((_tmpFF1.tag=0,((_tmpFF1.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(e)),((_tmpFF2[0]=&
_tmpFF1,((_tmpFF2[1]=& _tmpFF0,Cyc_Tcutil_terr(loc,((_tmpFF3="hidden type variable %s isn't abstracted in type %s",
_tag_dynforward(_tmpFF3,sizeof(char),_get_zero_arr_size_char(_tmpFF3,52)))),
_tag_dynforward(_tmpFF2,sizeof(void*),2)))))))))))));}goto _LL635;_LL635:;}};
_pop_region(r);}void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv);void
Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){if(tv->identity == - 1)tv->identity=
Cyc_Tcutil_new_tvar_id();}void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*
tvs);void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){((void(*)(void(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,
tvs);}static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct
Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _dynforward_ptr(*a2string)(
void*),struct _dynforward_ptr msg);static void Cyc_Tcutil_check_unique_unsorted(int(*
cmp)(void*,void*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct
_dynforward_ptr(*a2string)(void*),struct _dynforward_ptr msg){for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){if(cmp((void*)vs->hd,(
void*)vs2->hd)== 0){const char*_tmpFF8;void*_tmpFF7[2];struct Cyc_String_pa_struct
_tmpFF6;struct Cyc_String_pa_struct _tmpFF5;(_tmpFF5.tag=0,((_tmpFF5.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)a2string((void*)vs->hd)),((_tmpFF6.tag=0,((
_tmpFF6.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)msg),((_tmpFF7[0]=&
_tmpFF6,((_tmpFF7[1]=& _tmpFF5,Cyc_Tcutil_terr(loc,((_tmpFF8="%s: %s",
_tag_dynforward(_tmpFF8,sizeof(char),_get_zero_arr_size_char(_tmpFF8,7)))),
_tag_dynforward(_tmpFF7,sizeof(void*),2)))))))))))));}}}}static struct
_dynforward_ptr Cyc_Tcutil_strptr2string(struct _dynforward_ptr*s);static struct
_dynforward_ptr Cyc_Tcutil_strptr2string(struct _dynforward_ptr*s){return*s;}void
Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr msg);void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dynforward_ptr msg){((void(*)(int(*cmp)(
struct _dynforward_ptr*,struct _dynforward_ptr*),struct Cyc_List_List*vs,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr(*a2string)(struct _dynforward_ptr*),struct
_dynforward_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,
msg);}void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*loc,struct Cyc_List_List*
tvs);void Cyc_Tcutil_check_unique_tvars(struct Cyc_Position_Segment*loc,struct Cyc_List_List*
tvs){const char*_tmpFF9;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*vs,struct Cyc_Position_Segment*loc,struct _dynforward_ptr(*
a2string)(struct Cyc_Absyn_Tvar*),struct _dynforward_ptr msg))Cyc_Tcutil_check_unique_unsorted)(
Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmpFF9="duplicate type variable",
_tag_dynforward(_tmpFF9,sizeof(char),_get_zero_arr_size_char(_tmpFF9,24)))));}
struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple21{struct Cyc_List_List*
f1;void*f2;};struct _tuple22{struct Cyc_Absyn_Aggrfield*f1;void*f2;};struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*sdfields);struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*sdfields){struct Cyc_List_List*
fields=0;{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=
sd_fields->tl){const char*_tmpFFA;if(Cyc_strcmp((struct _dynforward_ptr)*((struct
Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmpFFA="",_tag_dynforward(_tmpFFA,
sizeof(char),_get_zero_arr_size_char(_tmpFFA,1)))))!= 0){struct _tuple20*_tmpFFD;
struct Cyc_List_List*_tmpFFC;fields=((_tmpFFC=_cycalloc(sizeof(*_tmpFFC)),((
_tmpFFC->hd=((_tmpFFD=_cycalloc(sizeof(*_tmpFFD)),((_tmpFFD->f1=(struct Cyc_Absyn_Aggrfield*)
sd_fields->hd,((_tmpFFD->f2=0,_tmpFFD)))))),((_tmpFFC->tl=fields,_tmpFFC))))));}}}
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
struct Cyc_List_List*ans=0;for(0;des != 0;des=des->tl){struct _tuple21 _tmp9DE;
struct Cyc_List_List*_tmp9DF;void*_tmp9E0;struct _tuple21*_tmp9DD=(struct _tuple21*)
des->hd;_tmp9DE=*_tmp9DD;_tmp9DF=_tmp9DE.f1;_tmp9E0=_tmp9DE.f2;if(_tmp9DF == 0){
struct Cyc_List_List*_tmp9E1=fields;for(0;_tmp9E1 != 0;_tmp9E1=_tmp9E1->tl){if(!(*((
struct _tuple20*)_tmp9E1->hd)).f2){(*((struct _tuple20*)_tmp9E1->hd)).f2=1;{struct
Cyc_Absyn_FieldName_struct*_tmp1003;struct Cyc_Absyn_FieldName_struct _tmp1002;
struct Cyc_List_List*_tmp1001;(*((struct _tuple21*)des->hd)).f1=(struct Cyc_List_List*)((
_tmp1001=_cycalloc(sizeof(*_tmp1001)),((_tmp1001->hd=(void*)((void*)((_tmp1003=
_cycalloc(sizeof(*_tmp1003)),((_tmp1003[0]=((_tmp1002.tag=1,((_tmp1002.f1=((*((
struct _tuple20*)_tmp9E1->hd)).f1)->name,_tmp1002)))),_tmp1003))))),((_tmp1001->tl=
0,_tmp1001))))));}{struct _tuple22*_tmp1006;struct Cyc_List_List*_tmp1005;ans=((
_tmp1005=_region_malloc(rgn,sizeof(*_tmp1005)),((_tmp1005->hd=((_tmp1006=
_region_malloc(rgn,sizeof(*_tmp1006)),((_tmp1006->f1=(*((struct _tuple20*)_tmp9E1->hd)).f1,((
_tmp1006->f2=_tmp9E0,_tmp1006)))))),((_tmp1005->tl=ans,_tmp1005))))));}break;}}
if(_tmp9E1 == 0){const char*_tmp1009;void*_tmp1008;(_tmp1008=0,Cyc_Tcutil_terr(loc,((
_tmp1009="too many arguments to struct",_tag_dynforward(_tmp1009,sizeof(char),
_get_zero_arr_size_char(_tmp1009,29)))),_tag_dynforward(_tmp1008,sizeof(void*),0)));}}
else{if(_tmp9DF->tl != 0){const char*_tmp100C;void*_tmp100B;(_tmp100B=0,Cyc_Tcutil_terr(
loc,((_tmp100C="multiple designators are not supported",_tag_dynforward(_tmp100C,
sizeof(char),_get_zero_arr_size_char(_tmp100C,39)))),_tag_dynforward(_tmp100B,
sizeof(void*),0)));}else{void*_tmp9EB=(void*)_tmp9DF->hd;struct _dynforward_ptr*
_tmp9EC;_LL641: if(*((int*)_tmp9EB)!= 0)goto _LL643;_LL642:{const char*_tmp100F;
void*_tmp100E;(_tmp100E=0,Cyc_Tcutil_terr(loc,((_tmp100F="array designator used in argument to struct",
_tag_dynforward(_tmp100F,sizeof(char),_get_zero_arr_size_char(_tmp100F,44)))),
_tag_dynforward(_tmp100E,sizeof(void*),0)));}goto _LL640;_LL643: if(*((int*)
_tmp9EB)!= 1)goto _LL640;_tmp9EC=((struct Cyc_Absyn_FieldName_struct*)_tmp9EB)->f1;
_LL644: {struct Cyc_List_List*_tmp9EF=fields;for(0;_tmp9EF != 0;_tmp9EF=_tmp9EF->tl){
if(Cyc_strptrcmp(_tmp9EC,((*((struct _tuple20*)_tmp9EF->hd)).f1)->name)== 0){if((*((
struct _tuple20*)_tmp9EF->hd)).f2){const char*_tmp1013;void*_tmp1012[1];struct Cyc_String_pa_struct
_tmp1011;(_tmp1011.tag=0,((_tmp1011.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)*_tmp9EC),((_tmp1012[0]=& _tmp1011,Cyc_Tcutil_terr(loc,((_tmp1013="field %s has already been used as an argument",
_tag_dynforward(_tmp1013,sizeof(char),_get_zero_arr_size_char(_tmp1013,46)))),
_tag_dynforward(_tmp1012,sizeof(void*),1)))))));}(*((struct _tuple20*)_tmp9EF->hd)).f2=
1;{struct _tuple22*_tmp1016;struct Cyc_List_List*_tmp1015;ans=((_tmp1015=
_region_malloc(rgn,sizeof(*_tmp1015)),((_tmp1015->hd=((_tmp1016=_region_malloc(
rgn,sizeof(*_tmp1016)),((_tmp1016->f1=(*((struct _tuple20*)_tmp9EF->hd)).f1,((
_tmp1016->f2=_tmp9E0,_tmp1016)))))),((_tmp1015->tl=ans,_tmp1015))))));}break;}}
if(_tmp9EF == 0){const char*_tmp101A;void*_tmp1019[1];struct Cyc_String_pa_struct
_tmp1018;(_tmp1018.tag=0,((_tmp1018.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)*_tmp9EC),((_tmp1019[0]=& _tmp1018,Cyc_Tcutil_terr(loc,((_tmp101A="bad field designator %s",
_tag_dynforward(_tmp101A,sizeof(char),_get_zero_arr_size_char(_tmp101A,24)))),
_tag_dynforward(_tmp1019,sizeof(void*),1)))))));}goto _LL640;}_LL640:;}}}for(0;
fields != 0;fields=fields->tl){if(!(*((struct _tuple20*)fields->hd)).f2){{const
char*_tmp101D;void*_tmp101C;(_tmp101C=0,Cyc_Tcutil_terr(loc,((_tmp101D="too few arguments to struct",
_tag_dynforward(_tmp101D,sizeof(char),_get_zero_arr_size_char(_tmp101D,28)))),
_tag_dynforward(_tmp101C,sizeof(void*),0)));}break;}}return((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);}}int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest,int*forward_only);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest,int*forward_only){*forward_only=0;{void*_tmp9FA=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp9FB;void*_tmp9FC;struct Cyc_Absyn_PtrAtts _tmp9FD;
struct Cyc_Absyn_Conref*_tmp9FE;_LL646: if(_tmp9FA <= (void*)4)goto _LL648;if(*((int*)
_tmp9FA)!= 4)goto _LL648;_tmp9FB=((struct Cyc_Absyn_PointerType_struct*)_tmp9FA)->f1;
_tmp9FC=(void*)_tmp9FB.elt_typ;_tmp9FD=_tmp9FB.ptr_atts;_tmp9FE=_tmp9FD.bounds;
_LL647: {struct Cyc_Absyn_Conref*_tmp9FF=Cyc_Absyn_compress_conref(_tmp9FE);union
Cyc_Absyn_Constraint_union _tmpA00=(Cyc_Absyn_compress_conref(_tmp9FF))->v;void*
_tmpA01;void*_tmpA02;_LL64B: if((_tmpA00.Eq_constr).tag != 0)goto _LL64D;_tmpA01=(
_tmpA00.Eq_constr).f1;if((int)_tmpA01 != 0)goto _LL64D;_LL64C:*forward_only=1;*
elt_typ_dest=_tmp9FC;return 1;_LL64D: if((_tmpA00.Eq_constr).tag != 0)goto _LL64F;
_tmpA02=(_tmpA00.Eq_constr).f1;if((int)_tmpA02 != 1)goto _LL64F;_LL64E:*
elt_typ_dest=_tmp9FC;return 1;_LL64F: if((_tmpA00.No_constr).tag != 2)goto _LL651;
_LL650:{union Cyc_Absyn_Constraint_union _tmp101E;_tmp9FF->v=(union Cyc_Absyn_Constraint_union)(((
_tmp101E.Eq_constr).tag=0,(((_tmp101E.Eq_constr).f1=(void*)((void*)0),_tmp101E))));}*
forward_only=1;*elt_typ_dest=_tmp9FC;return 1;_LL651:;_LL652: return 0;_LL64A:;}
_LL648:;_LL649: return 0;_LL645:;}}int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,
void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest){void*_tmpA04=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpA05;
void*_tmpA06;struct Cyc_Absyn_PtrAtts _tmpA07;struct Cyc_Absyn_Conref*_tmpA08;
_LL654: if(_tmpA04 <= (void*)4)goto _LL656;if(*((int*)_tmpA04)!= 4)goto _LL656;
_tmpA05=((struct Cyc_Absyn_PointerType_struct*)_tmpA04)->f1;_tmpA06=(void*)
_tmpA05.elt_typ;_tmpA07=_tmpA05.ptr_atts;_tmpA08=_tmpA07.zero_term;_LL655:*
elt_typ_dest=_tmpA06;return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpA08);_LL656:;_LL657: return 0;_LL653:;}int Cyc_Tcutil_is_zero_ptr_type(void*t,
void**ptr_type,int*is_dynforward,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_type(
void*t,void**ptr_type,int*is_dynforward,int*is_dyneither,void**elt_type){void*
_tmpA09=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmpA0A;void*_tmpA0B;
struct Cyc_Absyn_PtrAtts _tmpA0C;struct Cyc_Absyn_Conref*_tmpA0D;struct Cyc_Absyn_Conref*
_tmpA0E;struct Cyc_Absyn_ArrayInfo _tmpA0F;void*_tmpA10;struct Cyc_Absyn_Tqual
_tmpA11;struct Cyc_Absyn_Exp*_tmpA12;struct Cyc_Absyn_Conref*_tmpA13;_LL659: if(
_tmpA09 <= (void*)4)goto _LL65D;if(*((int*)_tmpA09)!= 4)goto _LL65B;_tmpA0A=((
struct Cyc_Absyn_PointerType_struct*)_tmpA09)->f1;_tmpA0B=(void*)_tmpA0A.elt_typ;
_tmpA0C=_tmpA0A.ptr_atts;_tmpA0D=_tmpA0C.bounds;_tmpA0E=_tmpA0C.zero_term;_LL65A:
if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmpA0E)){*
ptr_type=t;*elt_type=_tmpA0B;{void*_tmpA14=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmpA0D);_LL660: if((int)_tmpA14 != 0)goto _LL662;_LL661:*is_dyneither=0;*
is_dynforward=1;goto _LL65F;_LL662: if((int)_tmpA14 != 1)goto _LL664;_LL663:*
is_dynforward=0;*is_dyneither=1;goto _LL65F;_LL664:;_LL665:*is_dynforward=(*
is_dyneither=0);goto _LL65F;_LL65F:;}return 1;}else{return 0;}_LL65B: if(*((int*)
_tmpA09)!= 7)goto _LL65D;_tmpA0F=((struct Cyc_Absyn_ArrayType_struct*)_tmpA09)->f1;
_tmpA10=(void*)_tmpA0F.elt_type;_tmpA11=_tmpA0F.tq;_tmpA12=_tmpA0F.num_elts;
_tmpA13=_tmpA0F.zero_term;_LL65C: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmpA13)){*elt_type=_tmpA10;*is_dynforward=(*is_dyneither=0);{struct Cyc_Absyn_PointerType_struct
_tmp1033;struct Cyc_Absyn_PtrAtts _tmp1032;struct Cyc_Absyn_Upper_b_struct _tmp1031;
struct Cyc_Absyn_Upper_b_struct*_tmp1030;struct Cyc_Absyn_PtrInfo _tmp102F;struct
Cyc_Absyn_PointerType_struct*_tmp102E;*ptr_type=(void*)((_tmp102E=_cycalloc(
sizeof(*_tmp102E)),((_tmp102E[0]=((_tmp1033.tag=4,((_tmp1033.f1=((_tmp102F.elt_typ=(
void*)_tmpA10,((_tmp102F.elt_tq=_tmpA11,((_tmp102F.ptr_atts=((_tmp1032.rgn=(void*)((
void*)2),((_tmp1032.nullable=Cyc_Absyn_false_conref,((_tmp1032.bounds=Cyc_Absyn_new_conref((
void*)((_tmp1030=_cycalloc(sizeof(*_tmp1030)),((_tmp1030[0]=((_tmp1031.tag=0,((
_tmp1031.f1=(struct Cyc_Absyn_Exp*)_check_null(_tmpA12),_tmp1031)))),_tmp1030))))),((
_tmp1032.zero_term=_tmpA13,((_tmp1032.ptrloc=0,_tmp1032)))))))))),_tmp102F)))))),
_tmp1033)))),_tmp102E))));}return 1;}else{return 0;}_LL65D:;_LL65E: return 0;_LL658:;}
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*
is_dynforward,int*is_dyneither,void**elt_type);int Cyc_Tcutil_is_zero_ptr_deref(
struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dynforward,int*is_dyneither,void**
elt_type){void*_tmpA1B=(void*)e1->r;struct Cyc_Absyn_Exp*_tmpA1C;struct Cyc_Absyn_Exp*
_tmpA1D;struct Cyc_Absyn_Exp*_tmpA1E;struct Cyc_Absyn_Exp*_tmpA1F;struct Cyc_Absyn_Exp*
_tmpA20;struct Cyc_Absyn_Exp*_tmpA21;_LL667: if(*((int*)_tmpA1B)!= 15)goto _LL669;
_LL668: {const char*_tmp1037;void*_tmp1036[1];struct Cyc_String_pa_struct _tmp1035;(
_tmp1035.tag=0,((_tmp1035.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e1)),((_tmp1036[0]=& _tmp1035,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp1037="we have a cast in a lhs:  %s",
_tag_dynforward(_tmp1037,sizeof(char),_get_zero_arr_size_char(_tmp1037,29)))),
_tag_dynforward(_tmp1036,sizeof(void*),1)))))));}_LL669: if(*((int*)_tmpA1B)!= 22)
goto _LL66B;_tmpA1C=((struct Cyc_Absyn_Deref_e_struct*)_tmpA1B)->f1;_LL66A: _tmpA1D=
_tmpA1C;goto _LL66C;_LL66B: if(*((int*)_tmpA1B)!= 25)goto _LL66D;_tmpA1D=((struct
Cyc_Absyn_Subscript_e_struct*)_tmpA1B)->f1;_LL66C: return Cyc_Tcutil_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA1D->topt))->v,ptr_type,
is_dynforward,is_dyneither,elt_type);_LL66D: if(*((int*)_tmpA1B)!= 24)goto _LL66F;
_tmpA1E=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpA1B)->f1;_LL66E: _tmpA1F=
_tmpA1E;goto _LL670;_LL66F: if(*((int*)_tmpA1B)!= 23)goto _LL671;_tmpA1F=((struct
Cyc_Absyn_AggrMember_e_struct*)_tmpA1B)->f1;_LL670: if(Cyc_Tcutil_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA1F->topt))->v,ptr_type,
is_dynforward,is_dyneither,elt_type)){const char*_tmp103B;void*_tmp103A[1];struct
Cyc_String_pa_struct _tmp1039;(_tmp1039.tag=0,((_tmp1039.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp103A[0]=&
_tmp1039,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmp103B="found zero pointer aggregate member assignment: %s",_tag_dynforward(
_tmp103B,sizeof(char),_get_zero_arr_size_char(_tmp103B,51)))),_tag_dynforward(
_tmp103A,sizeof(void*),1)))))));}return 0;_LL671: if(*((int*)_tmpA1B)!= 14)goto
_LL673;_tmpA20=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpA1B)->f1;_LL672:
_tmpA21=_tmpA20;goto _LL674;_LL673: if(*((int*)_tmpA1B)!= 13)goto _LL675;_tmpA21=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpA1B)->f1;_LL674: if(Cyc_Tcutil_is_zero_ptr_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA21->topt))->v,ptr_type,
is_dynforward,is_dyneither,elt_type)){const char*_tmp103F;void*_tmp103E[1];struct
Cyc_String_pa_struct _tmp103D;(_tmp103D.tag=0,((_tmp103D.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp103E[0]=&
_tmp103D,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmp103F="found zero pointer instantiate/noinstantiate: %s",_tag_dynforward(
_tmp103F,sizeof(char),_get_zero_arr_size_char(_tmp103F,49)))),_tag_dynforward(
_tmp103E,sizeof(void*),1)))))));}return 0;_LL675: if(*((int*)_tmpA1B)!= 1)goto
_LL677;_LL676: return 0;_LL677:;_LL678: {const char*_tmp1043;void*_tmp1042[1];
struct Cyc_String_pa_struct _tmp1041;(_tmp1041.tag=0,((_tmp1041.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1042[0]=&
_tmp1041,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmp1043="found bad lhs in is_zero_ptr_deref: %s",_tag_dynforward(_tmp1043,
sizeof(char),_get_zero_arr_size_char(_tmp1043,39)))),_tag_dynforward(_tmp1042,
sizeof(void*),1)))))));}_LL666:;}int Cyc_Tcutil_is_tagged_pointer_typ(void*t,int*
is_forward_only);int Cyc_Tcutil_is_tagged_pointer_typ(void*t,int*is_forward_only){
void*ignore=(void*)0;return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore,
is_forward_only);}static int Cyc_Tcutil_is_noalias_region(void*r);static int Cyc_Tcutil_is_noalias_region(
void*r){void*_tmpA2E=Cyc_Tcutil_compress(r);struct Cyc_Absyn_Tvar*_tmpA2F;_LL67A:
if((int)_tmpA2E != 3)goto _LL67C;_LL67B: return 1;_LL67C: if(_tmpA2E <= (void*)4)goto
_LL67E;if(*((int*)_tmpA2E)!= 1)goto _LL67E;_tmpA2F=((struct Cyc_Absyn_VarType_struct*)
_tmpA2E)->f1;_LL67D: return Cyc_Tcutil_tvar_kind(_tmpA2F)== (void*)4  || Cyc_Tcutil_tvar_kind(
_tmpA2F)== (void*)5;_LL67E:;_LL67F: return 0;_LL679:;}int Cyc_Tcutil_is_noalias_pointer(
void*t);int Cyc_Tcutil_is_noalias_pointer(void*t){void*_tmpA30=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpA31;struct Cyc_Absyn_PtrAtts _tmpA32;void*_tmpA33;
_LL681: if(_tmpA30 <= (void*)4)goto _LL683;if(*((int*)_tmpA30)!= 4)goto _LL683;
_tmpA31=((struct Cyc_Absyn_PointerType_struct*)_tmpA30)->f1;_tmpA32=_tmpA31.ptr_atts;
_tmpA33=(void*)_tmpA32.rgn;_LL682: return Cyc_Tcutil_is_noalias_region(_tmpA33);
_LL683:;_LL684: return 0;_LL680:;}int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){void*_tmpA34=Cyc_Tcutil_compress(
t);if(Cyc_Tcutil_is_noalias_pointer(_tmpA34))return 1;{void*_tmpA35=_tmpA34;
struct Cyc_List_List*_tmpA36;struct Cyc_Absyn_AggrInfo _tmpA37;union Cyc_Absyn_AggrInfoU_union
_tmpA38;struct Cyc_Absyn_Aggrdecl**_tmpA39;struct Cyc_List_List*_tmpA3A;struct Cyc_List_List*
_tmpA3B;struct Cyc_Absyn_AggrInfo _tmpA3C;union Cyc_Absyn_AggrInfoU_union _tmpA3D;
struct Cyc_Absyn_TunionInfo _tmpA3E;union Cyc_Absyn_TunionInfoU_union _tmpA3F;struct
Cyc_List_List*_tmpA40;struct Cyc_Core_Opt*_tmpA41;struct Cyc_Absyn_TunionFieldInfo
_tmpA42;union Cyc_Absyn_TunionFieldInfoU_union _tmpA43;struct Cyc_List_List*_tmpA44;
_LL686: if(_tmpA35 <= (void*)4)goto _LL692;if(*((int*)_tmpA35)!= 9)goto _LL688;
_tmpA36=((struct Cyc_Absyn_TupleType_struct*)_tmpA35)->f1;_LL687: while(_tmpA36 != 
0){if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple5*)_tmpA36->hd)).f2))
return 1;_tmpA36=_tmpA36->tl;}return 0;_LL688: if(*((int*)_tmpA35)!= 10)goto _LL68A;
_tmpA37=((struct Cyc_Absyn_AggrType_struct*)_tmpA35)->f1;_tmpA38=_tmpA37.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmpA35)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL68A;_tmpA39=(_tmpA38.KnownAggr).f1;_tmpA3A=_tmpA37.targs;_LL689: if((*
_tmpA39)->impl == 0)return 0;else{{struct _RegionHandle _tmpA45=_new_region("rgn");
struct _RegionHandle*rgn=& _tmpA45;_push_region(rgn);{struct Cyc_List_List*_tmpA46=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,(*_tmpA39)->tvs,_tmpA3A);struct
Cyc_List_List*_tmpA47=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpA39)->impl))->fields;
void*t;while(_tmpA47 != 0){t=Cyc_Tcutil_rsubstitute(rgn,_tmpA46,(void*)((struct
Cyc_Absyn_Aggrfield*)_tmpA47->hd)->type);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
t)){int _tmpA48=1;_npop_handler(0);return _tmpA48;}_tmpA47=_tmpA47->tl;}};
_pop_region(rgn);}return 0;}_LL68A: if(*((int*)_tmpA35)!= 11)goto _LL68C;_tmpA3B=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmpA35)->f2;_LL68B: while(_tmpA3B != 0){if(
Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)((struct Cyc_Absyn_Aggrfield*)
_tmpA3B->hd)->type))return 1;_tmpA3B=_tmpA3B->tl;}return 0;_LL68C: if(*((int*)
_tmpA35)!= 10)goto _LL68E;_tmpA3C=((struct Cyc_Absyn_AggrType_struct*)_tmpA35)->f1;
_tmpA3D=_tmpA3C.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpA35)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL68E;_LL68D: {const char*_tmp1046;void*_tmp1045;(_tmp1045=0,((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp1046="got unknown aggr in is_noalias_aggr",
_tag_dynforward(_tmp1046,sizeof(char),_get_zero_arr_size_char(_tmp1046,36)))),
_tag_dynforward(_tmp1045,sizeof(void*),0)));}_LL68E: if(*((int*)_tmpA35)!= 2)goto
_LL690;_tmpA3E=((struct Cyc_Absyn_TunionType_struct*)_tmpA35)->f1;_tmpA3F=_tmpA3E.tunion_info;
_tmpA40=_tmpA3E.targs;_tmpA41=_tmpA3E.rgn;_LL68F: {union Cyc_Absyn_TunionInfoU_union
_tmpA4B=_tmpA3F;struct Cyc_Absyn_UnknownTunionInfo _tmpA4C;struct _tuple2*_tmpA4D;
int _tmpA4E;int _tmpA4F;struct Cyc_Absyn_Tuniondecl**_tmpA50;struct Cyc_Absyn_Tuniondecl*
_tmpA51;struct Cyc_Absyn_Tuniondecl _tmpA52;struct Cyc_List_List*_tmpA53;struct Cyc_Core_Opt*
_tmpA54;int _tmpA55;_LL695: if((_tmpA4B.UnknownTunion).tag != 0)goto _LL697;_tmpA4C=(
_tmpA4B.UnknownTunion).f1;_tmpA4D=_tmpA4C.name;_tmpA4E=_tmpA4C.is_xtunion;
_tmpA4F=_tmpA4C.is_flat;_LL696: {const char*_tmp1049;void*_tmp1048;(_tmp1048=0,((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmp1049="got unknown tunion in is_noalias_aggr",_tag_dynforward(_tmp1049,
sizeof(char),_get_zero_arr_size_char(_tmp1049,38)))),_tag_dynforward(_tmp1048,
sizeof(void*),0)));}_LL697: if((_tmpA4B.KnownTunion).tag != 1)goto _LL694;_tmpA50=(
_tmpA4B.KnownTunion).f1;_tmpA51=*_tmpA50;_tmpA52=*_tmpA51;_tmpA53=_tmpA52.tvs;
_tmpA54=_tmpA52.fields;_tmpA55=_tmpA52.is_flat;_LL698: if(!_tmpA55){if(_tmpA41 == 
0){const char*_tmp104C;void*_tmp104B;(_tmp104B=0,((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp104C="got tunion with no region",
_tag_dynforward(_tmp104C,sizeof(char),_get_zero_arr_size_char(_tmp104C,26)))),
_tag_dynforward(_tmp104B,sizeof(void*),0)));}return Cyc_Tcutil_is_noalias_region((
void*)_tmpA41->v);}else{if(_tmpA54 == 0){const char*_tmp104F;void*_tmp104E;(
_tmp104E=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmp104F="got tunion with no fields",_tag_dynforward(_tmp104F,sizeof(char),
_get_zero_arr_size_char(_tmp104F,26)))),_tag_dynforward(_tmp104E,sizeof(void*),0)));}
else{{struct _RegionHandle _tmpA5C=_new_region("rgn");struct _RegionHandle*rgn=&
_tmpA5C;_push_region(rgn);{struct Cyc_List_List*_tmpA5D=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpA53,_tmpA40);struct Cyc_List_List*_tmpA5E=(struct
Cyc_List_List*)_tmpA54->v;while(_tmpA5E != 0){struct Cyc_List_List*_tmpA5F=((
struct Cyc_Absyn_Tunionfield*)_tmpA5E->hd)->typs;while(_tmpA5F != 0){_tmpA34=Cyc_Tcutil_rsubstitute(
rgn,_tmpA5D,(*((struct _tuple5*)_tmpA5F->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmpA34)){int _tmpA60=1;_npop_handler(0);return _tmpA60;}_tmpA5F=_tmpA5F->tl;}
_tmpA5E=_tmpA5E->tl;}};_pop_region(rgn);}return 0;}}_LL694:;}_LL690: if(*((int*)
_tmpA35)!= 3)goto _LL692;_tmpA42=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmpA35)->f1;_tmpA43=_tmpA42.field_info;_tmpA44=_tmpA42.targs;_LL691: {union Cyc_Absyn_TunionFieldInfoU_union
_tmpA61=_tmpA43;struct Cyc_Absyn_Tuniondecl*_tmpA62;struct Cyc_Absyn_Tunionfield*
_tmpA63;_LL69A: if((_tmpA61.UnknownTunionfield).tag != 0)goto _LL69C;_LL69B: {const
char*_tmp1052;void*_tmp1051;(_tmp1051=0,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp1052="got unknown tunion field in is_noalias_aggr",
_tag_dynforward(_tmp1052,sizeof(char),_get_zero_arr_size_char(_tmp1052,44)))),
_tag_dynforward(_tmp1051,sizeof(void*),0)));}_LL69C: if((_tmpA61.KnownTunionfield).tag
!= 1)goto _LL699;_tmpA62=(_tmpA61.KnownTunionfield).f1;_tmpA63=(_tmpA61.KnownTunionfield).f2;
_LL69D:{struct _RegionHandle _tmpA66=_new_region("rgn");struct _RegionHandle*rgn=&
_tmpA66;_push_region(rgn);{struct Cyc_List_List*_tmpA67=((struct Cyc_List_List*(*)(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,_tmpA62->tvs,_tmpA44);struct Cyc_List_List*_tmpA68=
_tmpA63->typs;while(_tmpA68 != 0){_tmpA34=Cyc_Tcutil_rsubstitute(rgn,_tmpA67,(*((
struct _tuple5*)_tmpA68->hd)).f2);if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmpA34)){int _tmpA69=1;_npop_handler(0);return _tmpA69;}_tmpA68=_tmpA68->tl;}};
_pop_region(rgn);}return 0;_LL699:;}_LL692:;_LL693: return 0;_LL685:;}}static int Cyc_Tcutil_is_noalias_field(
void*t,struct _dynforward_ptr*f);static int Cyc_Tcutil_is_noalias_field(void*t,
struct _dynforward_ptr*f){void*_tmpA6A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmpA6B;union Cyc_Absyn_AggrInfoU_union _tmpA6C;struct Cyc_Absyn_Aggrdecl**_tmpA6D;
struct Cyc_Absyn_Aggrdecl*_tmpA6E;struct Cyc_List_List*_tmpA6F;struct Cyc_List_List*
_tmpA70;_LL69F: if(_tmpA6A <= (void*)4)goto _LL6A3;if(*((int*)_tmpA6A)!= 10)goto
_LL6A1;_tmpA6B=((struct Cyc_Absyn_AggrType_struct*)_tmpA6A)->f1;_tmpA6C=_tmpA6B.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmpA6A)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL6A1;_tmpA6D=(_tmpA6C.KnownAggr).f1;_tmpA6E=*_tmpA6D;_tmpA6F=_tmpA6B.targs;
_LL6A0: _tmpA70=_tmpA6E->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmpA6E->impl))->fields;goto _LL6A2;_LL6A1: if(*((int*)_tmpA6A)!= 11)goto _LL6A3;
_tmpA70=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpA6A)->f2;_LL6A2: {struct Cyc_Absyn_Aggrfield*
_tmpA71=Cyc_Absyn_lookup_field(_tmpA70,f);{struct Cyc_Absyn_Aggrfield*_tmpA72=
_tmpA71;struct Cyc_Absyn_Aggrfield _tmpA73;void*_tmpA74;_LL6A6: if(_tmpA72 != 0)goto
_LL6A8;_LL6A7: {const char*_tmp1055;void*_tmp1054;(_tmp1054=0,((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp1055="is_noalias_field: missing field",
_tag_dynforward(_tmp1055,sizeof(char),_get_zero_arr_size_char(_tmp1055,32)))),
_tag_dynforward(_tmp1054,sizeof(void*),0)));}_LL6A8: if(_tmpA72 == 0)goto _LL6A5;
_tmpA73=*_tmpA72;_tmpA74=(void*)_tmpA73.type;_LL6A9: return Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmpA74);_LL6A5:;}return 0;}_LL6A3:;_LL6A4: {const char*_tmp1059;void*_tmp1058[1];
struct Cyc_String_pa_struct _tmp1057;(_tmp1057.tag=0,((_tmp1057.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1058[0]=&
_tmp1057,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmp1059="is_noalias_field: invalid type |%s|",_tag_dynforward(_tmp1059,sizeof(
char),_get_zero_arr_size_char(_tmp1059,36)))),_tag_dynforward(_tmp1058,sizeof(
void*),1)))))));}_LL69E:;}static int Cyc_Tcutil_is_noalias_path_aux(struct Cyc_Absyn_Exp*
e,int ignore_leaf);static int Cyc_Tcutil_is_noalias_path_aux(struct Cyc_Absyn_Exp*e,
int ignore_leaf){void*_tmpA7A=(void*)e->r;void*_tmpA7B;struct Cyc_Absyn_Exp*
_tmpA7C;struct _dynforward_ptr*_tmpA7D;struct Cyc_Absyn_Exp*_tmpA7E;struct Cyc_Absyn_Exp*
_tmpA7F;void*_tmpA80;void*_tmpA81;void*_tmpA82;struct Cyc_Absyn_Exp*_tmpA83;
struct Cyc_Absyn_Exp*_tmpA84;struct Cyc_Absyn_Exp*_tmpA85;struct Cyc_Absyn_Exp*
_tmpA86;void*_tmpA87;struct Cyc_Absyn_Exp*_tmpA88;struct Cyc_Absyn_Stmt*_tmpA89;
_LL6AB: if(*((int*)_tmpA7A)!= 5)goto _LL6AD;_LL6AC: goto _LL6AE;_LL6AD: if(*((int*)
_tmpA7A)!= 7)goto _LL6AF;_LL6AE: goto _LL6B0;_LL6AF: if(*((int*)_tmpA7A)!= 8)goto
_LL6B1;_LL6B0: goto _LL6B2;_LL6B1: if(*((int*)_tmpA7A)!= 12)goto _LL6B3;_LL6B2: goto
_LL6B4;_LL6B3: if(*((int*)_tmpA7A)!= 16)goto _LL6B5;_LL6B4: goto _LL6B6;_LL6B5: if(*((
int*)_tmpA7A)!= 18)goto _LL6B7;_LL6B6: goto _LL6B8;_LL6B7: if(*((int*)_tmpA7A)!= 19)
goto _LL6B9;_LL6B8: goto _LL6BA;_LL6B9: if(*((int*)_tmpA7A)!= 20)goto _LL6BB;_LL6BA:
goto _LL6BC;_LL6BB: if(*((int*)_tmpA7A)!= 21)goto _LL6BD;_LL6BC: goto _LL6BE;_LL6BD:
if(*((int*)_tmpA7A)!= 27)goto _LL6BF;_LL6BE: goto _LL6C0;_LL6BF: if(*((int*)_tmpA7A)
!= 29)goto _LL6C1;_LL6C0: goto _LL6C2;_LL6C1: if(*((int*)_tmpA7A)!= 28)goto _LL6C3;
_LL6C2: goto _LL6C4;_LL6C3: if(*((int*)_tmpA7A)!= 33)goto _LL6C5;_LL6C4: goto _LL6C6;
_LL6C5: if(*((int*)_tmpA7A)!= 34)goto _LL6C7;_LL6C6: goto _LL6C8;_LL6C7: if(*((int*)
_tmpA7A)!= 36)goto _LL6C9;_LL6C8: goto _LL6CA;_LL6C9: if(*((int*)_tmpA7A)!= 1)goto
_LL6CB;_tmpA7B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpA7A)->f2;if(_tmpA7B <= (
void*)1)goto _LL6CB;if(*((int*)_tmpA7B)!= 0)goto _LL6CB;_LL6CA: goto _LL6CC;_LL6CB:
if(*((int*)_tmpA7A)!= 3)goto _LL6CD;_LL6CC: return 0;_LL6CD: if(*((int*)_tmpA7A)!= 
22)goto _LL6CF;_LL6CE: goto _LL6D0;_LL6CF: if(*((int*)_tmpA7A)!= 24)goto _LL6D1;
_LL6D0: return 0;_LL6D1: if(*((int*)_tmpA7A)!= 23)goto _LL6D3;_tmpA7C=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmpA7A)->f1;_tmpA7D=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpA7A)->f2;_LL6D2:
return(ignore_leaf  || Cyc_Tcutil_is_noalias_field((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA7C->topt))->v,_tmpA7D)) && Cyc_Tcutil_is_noalias_path_aux(
_tmpA7C,0);_LL6D3: if(*((int*)_tmpA7A)!= 25)goto _LL6D5;_tmpA7E=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpA7A)->f1;_tmpA7F=((struct Cyc_Absyn_Subscript_e_struct*)_tmpA7A)->f2;_LL6D4: {
void*_tmpA8A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpA7E->topt))->v);struct Cyc_Absyn_PtrInfo _tmpA8B;void*_tmpA8C;struct Cyc_List_List*
_tmpA8D;_LL6F8: if(_tmpA8A <= (void*)4)goto _LL6FC;if(*((int*)_tmpA8A)!= 4)goto
_LL6FA;_tmpA8B=((struct Cyc_Absyn_PointerType_struct*)_tmpA8A)->f1;_tmpA8C=(void*)
_tmpA8B.elt_typ;_LL6F9: return 0;_LL6FA: if(*((int*)_tmpA8A)!= 9)goto _LL6FC;_tmpA8D=((
struct Cyc_Absyn_TupleType_struct*)_tmpA8A)->f1;_LL6FB: {unsigned int _tmpA8F;int
_tmpA90;struct _tuple7 _tmpA8E=Cyc_Evexp_eval_const_uint_exp(_tmpA7F);_tmpA8F=
_tmpA8E.f1;_tmpA90=_tmpA8E.f2;if(!_tmpA90){const char*_tmp105C;void*_tmp105B;(
_tmp105B=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmp105C="is_noalias_path_aux: non-constant subscript",_tag_dynforward(_tmp105C,
sizeof(char),_get_zero_arr_size_char(_tmp105C,44)))),_tag_dynforward(_tmp105B,
sizeof(void*),0)));}{struct _handler_cons _tmpA93;_push_handler(& _tmpA93);{int
_tmpA95=0;if(setjmp(_tmpA93.handler))_tmpA95=1;if(!_tmpA95){{void*_tmpA96=(*((
struct _tuple5*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpA8D,(int)_tmpA8F)).f2;
int _tmpA97=(ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmpA96)) && 
Cyc_Tcutil_is_noalias_path_aux(_tmpA7E,0);_npop_handler(0);return _tmpA97;};
_pop_handler();}else{void*_tmpA94=(void*)_exn_thrown;void*_tmpA99=_tmpA94;_LL6FF:
if(_tmpA99 != Cyc_List_Nth)goto _LL701;_LL700: {const char*_tmp105F;void*_tmp105E;
return(_tmp105E=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmp105F="is_noalias_path_aux: out-of-bounds subscript",_tag_dynforward(_tmp105F,
sizeof(char),_get_zero_arr_size_char(_tmp105F,45)))),_tag_dynforward(_tmp105E,
sizeof(void*),0)));}_LL701:;_LL702:(void)_throw(_tmpA99);_LL6FE:;}}}}_LL6FC:;
_LL6FD: {const char*_tmp1062;void*_tmp1061;(_tmp1061=0,((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp1062="is_noalias_path_aux: subscript on non-pointer/tuple",
_tag_dynforward(_tmp1062,sizeof(char),_get_zero_arr_size_char(_tmp1062,52)))),
_tag_dynforward(_tmp1061,sizeof(void*),0)));}_LL6F7:;}_LL6D5: if(*((int*)_tmpA7A)
!= 32)goto _LL6D7;_LL6D6: goto _LL6D8;_LL6D7: if(*((int*)_tmpA7A)!= 26)goto _LL6D9;
_LL6D8: goto _LL6DA;_LL6D9: if(*((int*)_tmpA7A)!= 30)goto _LL6DB;_LL6DA: goto _LL6DC;
_LL6DB: if(*((int*)_tmpA7A)!= 31)goto _LL6DD;_LL6DC: goto _LL6DE;_LL6DD: if(*((int*)
_tmpA7A)!= 0)goto _LL6DF;_LL6DE: goto _LL6E0;_LL6DF: if(*((int*)_tmpA7A)!= 35)goto
_LL6E1;_LL6E0: goto _LL6E2;_LL6E1: if(*((int*)_tmpA7A)!= 17)goto _LL6E3;_LL6E2: goto
_LL6E4;_LL6E3: if(*((int*)_tmpA7A)!= 1)goto _LL6E5;_tmpA80=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpA7A)->f2;if(_tmpA80 <= (void*)1)goto _LL6E5;if(*((int*)_tmpA80)!= 3)goto _LL6E5;
_LL6E4: goto _LL6E6;_LL6E5: if(*((int*)_tmpA7A)!= 1)goto _LL6E7;_tmpA81=(void*)((
struct Cyc_Absyn_Var_e_struct*)_tmpA7A)->f2;if(_tmpA81 <= (void*)1)goto _LL6E7;if(*((
int*)_tmpA81)!= 4)goto _LL6E7;_LL6E6: goto _LL6E8;_LL6E7: if(*((int*)_tmpA7A)!= 1)
goto _LL6E9;_tmpA82=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpA7A)->f2;if(
_tmpA82 <= (void*)1)goto _LL6E9;if(*((int*)_tmpA82)!= 2)goto _LL6E9;_LL6E8: {int
_tmpA9E=ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);return _tmpA9E;}_LL6E9: if(*((int*)_tmpA7A)!= 6)goto
_LL6EB;_tmpA83=((struct Cyc_Absyn_Conditional_e_struct*)_tmpA7A)->f2;_LL6EA:
_tmpA84=_tmpA83;goto _LL6EC;_LL6EB: if(*((int*)_tmpA7A)!= 9)goto _LL6ED;_tmpA84=((
struct Cyc_Absyn_SeqExp_e_struct*)_tmpA7A)->f2;_LL6EC: _tmpA85=_tmpA84;goto _LL6EE;
_LL6ED: if(*((int*)_tmpA7A)!= 4)goto _LL6EF;_tmpA85=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpA7A)->f1;_LL6EE: return Cyc_Tcutil_is_noalias_path_aux(_tmpA85,ignore_leaf);
_LL6EF: if(*((int*)_tmpA7A)!= 11)goto _LL6F1;_tmpA86=((struct Cyc_Absyn_FnCall_e_struct*)
_tmpA7A)->f1;_LL6F0: {void*_tmpA9F=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpA86->topt))->v);struct Cyc_Absyn_FnInfo _tmpAA0;void*_tmpAA1;
struct Cyc_Absyn_PtrInfo _tmpAA2;void*_tmpAA3;_LL704: if(_tmpA9F <= (void*)4)goto
_LL708;if(*((int*)_tmpA9F)!= 8)goto _LL706;_tmpAA0=((struct Cyc_Absyn_FnType_struct*)
_tmpA9F)->f1;_tmpAA1=(void*)_tmpAA0.ret_typ;_LL705: return ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmpAA1);_LL706: if(*((int*)_tmpA9F)!= 4)goto _LL708;_tmpAA2=((struct Cyc_Absyn_PointerType_struct*)
_tmpA9F)->f1;_tmpAA3=(void*)_tmpAA2.elt_typ;_LL707:{void*_tmpAA4=Cyc_Tcutil_compress(
_tmpAA3);struct Cyc_Absyn_FnInfo _tmpAA5;void*_tmpAA6;_LL70B: if(_tmpAA4 <= (void*)4)
goto _LL70D;if(*((int*)_tmpAA4)!= 8)goto _LL70D;_tmpAA5=((struct Cyc_Absyn_FnType_struct*)
_tmpAA4)->f1;_tmpAA6=(void*)_tmpAA5.ret_typ;_LL70C: return ignore_leaf  || Cyc_Tcutil_is_noalias_pointer_or_aggr(
_tmpAA6);_LL70D:;_LL70E: goto _LL70A;_LL70A:;}goto _LL709;_LL708:;_LL709: {const
char*_tmp1066;void*_tmp1065[1];struct Cyc_String_pa_struct _tmp1064;(_tmp1064.tag=
0,((_tmp1064.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpA86->topt))->v)),((_tmp1065[0]=&
_tmp1064,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmp1066="Fncall has non function type %s",_tag_dynforward(_tmp1066,sizeof(char),
_get_zero_arr_size_char(_tmp1066,32)))),_tag_dynforward(_tmp1065,sizeof(void*),1)))))));}
_LL703:;}_LL6F1: if(*((int*)_tmpA7A)!= 15)goto _LL6F3;_tmpA87=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmpA7A)->f1;_tmpA88=((struct Cyc_Absyn_Cast_e_struct*)_tmpA7A)->f2;_LL6F2: return
Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmpA87) && Cyc_Tcutil_is_noalias_path_aux(
_tmpA88,1);_LL6F3: if(*((int*)_tmpA7A)!= 38)goto _LL6F5;_tmpA89=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmpA7A)->f1;_LL6F4: while(1){void*_tmpAAA=(void*)_tmpA89->r;struct Cyc_Absyn_Stmt*
_tmpAAB;struct Cyc_Absyn_Stmt*_tmpAAC;struct Cyc_Absyn_Decl*_tmpAAD;struct Cyc_Absyn_Stmt*
_tmpAAE;struct Cyc_Absyn_Exp*_tmpAAF;_LL710: if(_tmpAAA <= (void*)1)goto _LL716;if(*((
int*)_tmpAAA)!= 1)goto _LL712;_tmpAAB=((struct Cyc_Absyn_Seq_s_struct*)_tmpAAA)->f1;
_tmpAAC=((struct Cyc_Absyn_Seq_s_struct*)_tmpAAA)->f2;_LL711: _tmpA89=_tmpAAC;goto
_LL70F;_LL712: if(*((int*)_tmpAAA)!= 11)goto _LL714;_tmpAAD=((struct Cyc_Absyn_Decl_s_struct*)
_tmpAAA)->f1;_tmpAAE=((struct Cyc_Absyn_Decl_s_struct*)_tmpAAA)->f2;_LL713:
_tmpA89=_tmpAAE;goto _LL70F;_LL714: if(*((int*)_tmpAAA)!= 0)goto _LL716;_tmpAAF=((
struct Cyc_Absyn_Exp_s_struct*)_tmpAAA)->f1;_LL715: return Cyc_Tcutil_is_noalias_path_aux(
_tmpAAF,ignore_leaf);_LL716:;_LL717: {const char*_tmp1069;void*_tmp1068;(_tmp1068=
0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmp1069="is_noalias_stmt_exp: ill-formed StmtExp",_tag_dynforward(_tmp1069,
sizeof(char),_get_zero_arr_size_char(_tmp1069,40)))),_tag_dynforward(_tmp1068,
sizeof(void*),0)));}_LL70F:;}_LL6F5:;_LL6F6: {const char*_tmp106D;void*_tmp106C[1];
struct Cyc_String_pa_struct _tmp106B;(_tmp106B.tag=0,((_tmp106B.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e)),((_tmp106C[0]=&
_tmp106B,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmp106D="Called is_noalias_path_aux with bogus exp |%s|\n",_tag_dynforward(
_tmp106D,sizeof(char),_get_zero_arr_size_char(_tmp106D,48)))),_tag_dynforward(
_tmp106C,sizeof(void*),1)))))));}_LL6AA:;}int Cyc_Tcutil_is_noalias_path(struct
Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_is_noalias_path_aux(
e,0);}struct _tuple10 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e);struct _tuple10 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*
e){struct _tuple10 _tmp106E;struct _tuple10 bogus_ans=(_tmp106E.f1=0,((_tmp106E.f2=(
void*)2,_tmp106E)));void*_tmpAB5=(void*)e->r;struct _tuple2*_tmpAB6;void*_tmpAB7;
struct Cyc_Absyn_Exp*_tmpAB8;struct _dynforward_ptr*_tmpAB9;struct Cyc_Absyn_Exp*
_tmpABA;struct _dynforward_ptr*_tmpABB;struct Cyc_Absyn_Exp*_tmpABC;struct Cyc_Absyn_Exp*
_tmpABD;struct Cyc_Absyn_Exp*_tmpABE;_LL719: if(*((int*)_tmpAB5)!= 1)goto _LL71B;
_tmpAB6=((struct Cyc_Absyn_Var_e_struct*)_tmpAB5)->f1;_tmpAB7=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpAB5)->f2;_LL71A: {void*_tmpABF=_tmpAB7;struct Cyc_Absyn_Vardecl*_tmpAC0;
struct Cyc_Absyn_Vardecl*_tmpAC1;struct Cyc_Absyn_Vardecl*_tmpAC2;struct Cyc_Absyn_Vardecl*
_tmpAC3;_LL726: if((int)_tmpABF != 0)goto _LL728;_LL727: goto _LL729;_LL728: if(
_tmpABF <= (void*)1)goto _LL72A;if(*((int*)_tmpABF)!= 1)goto _LL72A;_LL729: return
bogus_ans;_LL72A: if(_tmpABF <= (void*)1)goto _LL72C;if(*((int*)_tmpABF)!= 0)goto
_LL72C;_tmpAC0=((struct Cyc_Absyn_Global_b_struct*)_tmpABF)->f1;_LL72B: {void*
_tmpAC4=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LL733: if(_tmpAC4 <= (void*)4)goto _LL735;if(*((int*)_tmpAC4)!= 7)goto _LL735;
_LL734: {struct _tuple10 _tmp106F;return(_tmp106F.f1=1,((_tmp106F.f2=(void*)2,
_tmp106F)));}_LL735:;_LL736: {struct _tuple10 _tmp1070;return(_tmp1070.f1=(_tmpAC0->tq).real_const,((
_tmp1070.f2=(void*)2,_tmp1070)));}_LL732:;}_LL72C: if(_tmpABF <= (void*)1)goto
_LL72E;if(*((int*)_tmpABF)!= 3)goto _LL72E;_tmpAC1=((struct Cyc_Absyn_Local_b_struct*)
_tmpABF)->f1;_LL72D: {void*_tmpAC7=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_LL738: if(_tmpAC7 <= (void*)4)goto _LL73A;if(*((int*)
_tmpAC7)!= 7)goto _LL73A;_LL739: {struct _tuple10 _tmp1071;return(_tmp1071.f1=1,((
_tmp1071.f2=(void*)((struct Cyc_Core_Opt*)_check_null(_tmpAC1->rgn))->v,_tmp1071)));}
_LL73A:;_LL73B: _tmpAC1->escapes=1;{struct _tuple10 _tmp1072;return(_tmp1072.f1=(
_tmpAC1->tq).real_const,((_tmp1072.f2=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmpAC1->rgn))->v,_tmp1072)));}_LL737:;}_LL72E: if(_tmpABF <= (void*)1)goto _LL730;
if(*((int*)_tmpABF)!= 4)goto _LL730;_tmpAC2=((struct Cyc_Absyn_Pat_b_struct*)
_tmpABF)->f1;_LL72F: _tmpAC3=_tmpAC2;goto _LL731;_LL730: if(_tmpABF <= (void*)1)goto
_LL725;if(*((int*)_tmpABF)!= 2)goto _LL725;_tmpAC3=((struct Cyc_Absyn_Param_b_struct*)
_tmpABF)->f1;_LL731: _tmpAC3->escapes=1;{struct _tuple10 _tmp1073;return(_tmp1073.f1=(
_tmpAC3->tq).real_const,((_tmp1073.f2=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmpAC3->rgn))->v,_tmp1073)));}_LL725:;}_LL71B: if(*((int*)_tmpAB5)!= 23)goto
_LL71D;_tmpAB8=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpAB5)->f1;_tmpAB9=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmpAB5)->f2;_LL71C: {void*_tmpACB=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmpAB8->topt))->v);struct Cyc_List_List*
_tmpACC;struct Cyc_Absyn_AggrInfo _tmpACD;union Cyc_Absyn_AggrInfoU_union _tmpACE;
struct Cyc_Absyn_Aggrdecl**_tmpACF;struct Cyc_Absyn_Aggrdecl*_tmpAD0;_LL73D: if(
_tmpACB <= (void*)4)goto _LL741;if(*((int*)_tmpACB)!= 11)goto _LL73F;_tmpACC=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmpACB)->f2;_LL73E: {struct Cyc_Absyn_Aggrfield*
_tmpAD1=Cyc_Absyn_lookup_field(_tmpACC,_tmpAB9);if(_tmpAD1 != 0  && _tmpAD1->width
!= 0){struct _tuple10 _tmp1074;return(_tmp1074.f1=(_tmpAD1->tq).real_const,((
_tmp1074.f2=(Cyc_Tcutil_addressof_props(te,_tmpAB8)).f2,_tmp1074)));}return
bogus_ans;}_LL73F: if(*((int*)_tmpACB)!= 10)goto _LL741;_tmpACD=((struct Cyc_Absyn_AggrType_struct*)
_tmpACB)->f1;_tmpACE=_tmpACD.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmpACB)->f1).aggr_info).KnownAggr).tag != 1)goto _LL741;_tmpACF=(_tmpACE.KnownAggr).f1;
_tmpAD0=*_tmpACF;_LL740: {struct Cyc_Absyn_Aggrfield*_tmpAD3=Cyc_Absyn_lookup_decl_field(
_tmpAD0,_tmpAB9);if(_tmpAD3 != 0  && _tmpAD3->width != 0){struct _tuple10 _tmp1075;
return(_tmp1075.f1=(_tmpAD3->tq).real_const,((_tmp1075.f2=(Cyc_Tcutil_addressof_props(
te,_tmpAB8)).f2,_tmp1075)));}return bogus_ans;}_LL741:;_LL742: return bogus_ans;
_LL73C:;}_LL71D: if(*((int*)_tmpAB5)!= 24)goto _LL71F;_tmpABA=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpAB5)->f1;_tmpABB=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpAB5)->f2;_LL71E: {
void*_tmpAD5=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmpABA->topt))->v);struct Cyc_Absyn_PtrInfo _tmpAD6;void*_tmpAD7;struct Cyc_Absyn_PtrAtts
_tmpAD8;void*_tmpAD9;_LL744: if(_tmpAD5 <= (void*)4)goto _LL746;if(*((int*)_tmpAD5)
!= 4)goto _LL746;_tmpAD6=((struct Cyc_Absyn_PointerType_struct*)_tmpAD5)->f1;
_tmpAD7=(void*)_tmpAD6.elt_typ;_tmpAD8=_tmpAD6.ptr_atts;_tmpAD9=(void*)_tmpAD8.rgn;
_LL745: {struct Cyc_Absyn_Aggrfield*finfo;{void*_tmpADA=Cyc_Tcutil_compress(
_tmpAD7);struct Cyc_List_List*_tmpADB;struct Cyc_Absyn_AggrInfo _tmpADC;union Cyc_Absyn_AggrInfoU_union
_tmpADD;struct Cyc_Absyn_Aggrdecl**_tmpADE;struct Cyc_Absyn_Aggrdecl*_tmpADF;
_LL749: if(_tmpADA <= (void*)4)goto _LL74D;if(*((int*)_tmpADA)!= 11)goto _LL74B;
_tmpADB=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpADA)->f2;_LL74A: finfo=Cyc_Absyn_lookup_field(
_tmpADB,_tmpABB);goto _LL748;_LL74B: if(*((int*)_tmpADA)!= 10)goto _LL74D;_tmpADC=((
struct Cyc_Absyn_AggrType_struct*)_tmpADA)->f1;_tmpADD=_tmpADC.aggr_info;if((((((
struct Cyc_Absyn_AggrType_struct*)_tmpADA)->f1).aggr_info).KnownAggr).tag != 1)
goto _LL74D;_tmpADE=(_tmpADD.KnownAggr).f1;_tmpADF=*_tmpADE;_LL74C: finfo=Cyc_Absyn_lookup_decl_field(
_tmpADF,_tmpABB);goto _LL748;_LL74D:;_LL74E: return bogus_ans;_LL748:;}if(finfo != 0
 && finfo->width != 0){struct _tuple10 _tmp1076;return(_tmp1076.f1=(finfo->tq).real_const,((
_tmp1076.f2=_tmpAD9,_tmp1076)));}return bogus_ans;}_LL746:;_LL747: return bogus_ans;
_LL743:;}_LL71F: if(*((int*)_tmpAB5)!= 22)goto _LL721;_tmpABC=((struct Cyc_Absyn_Deref_e_struct*)
_tmpAB5)->f1;_LL720: {void*_tmpAE1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmpABC->topt))->v);struct Cyc_Absyn_PtrInfo _tmpAE2;struct Cyc_Absyn_Tqual
_tmpAE3;struct Cyc_Absyn_PtrAtts _tmpAE4;void*_tmpAE5;_LL750: if(_tmpAE1 <= (void*)4)
goto _LL752;if(*((int*)_tmpAE1)!= 4)goto _LL752;_tmpAE2=((struct Cyc_Absyn_PointerType_struct*)
_tmpAE1)->f1;_tmpAE3=_tmpAE2.elt_tq;_tmpAE4=_tmpAE2.ptr_atts;_tmpAE5=(void*)
_tmpAE4.rgn;_LL751: {struct _tuple10 _tmp1077;return(_tmp1077.f1=_tmpAE3.real_const,((
_tmp1077.f2=_tmpAE5,_tmp1077)));}_LL752:;_LL753: return bogus_ans;_LL74F:;}_LL721:
if(*((int*)_tmpAB5)!= 25)goto _LL723;_tmpABD=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpAB5)->f1;_tmpABE=((struct Cyc_Absyn_Subscript_e_struct*)_tmpAB5)->f2;_LL722: {
void*t=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmpABD->topt))->v);
void*_tmpAE7=t;struct Cyc_List_List*_tmpAE8;struct Cyc_Absyn_PtrInfo _tmpAE9;struct
Cyc_Absyn_Tqual _tmpAEA;struct Cyc_Absyn_PtrAtts _tmpAEB;void*_tmpAEC;struct Cyc_Absyn_ArrayInfo
_tmpAED;struct Cyc_Absyn_Tqual _tmpAEE;_LL755: if(_tmpAE7 <= (void*)4)goto _LL75B;if(*((
int*)_tmpAE7)!= 9)goto _LL757;_tmpAE8=((struct Cyc_Absyn_TupleType_struct*)_tmpAE7)->f1;
_LL756: {unsigned int _tmpAF0;int _tmpAF1;struct _tuple7 _tmpAEF=Cyc_Evexp_eval_const_uint_exp(
_tmpABE);_tmpAF0=_tmpAEF.f1;_tmpAF1=_tmpAEF.f2;if(!_tmpAF1)return bogus_ans;{
struct _tuple5*_tmpAF2=Cyc_Absyn_lookup_tuple_field(_tmpAE8,(int)_tmpAF0);if(
_tmpAF2 != 0){struct _tuple10 _tmp1078;return(_tmp1078.f1=((*_tmpAF2).f1).real_const,((
_tmp1078.f2=(Cyc_Tcutil_addressof_props(te,_tmpABD)).f2,_tmp1078)));}return
bogus_ans;}}_LL757: if(*((int*)_tmpAE7)!= 4)goto _LL759;_tmpAE9=((struct Cyc_Absyn_PointerType_struct*)
_tmpAE7)->f1;_tmpAEA=_tmpAE9.elt_tq;_tmpAEB=_tmpAE9.ptr_atts;_tmpAEC=(void*)
_tmpAEB.rgn;_LL758: {struct _tuple10 _tmp1079;return(_tmp1079.f1=_tmpAEA.real_const,((
_tmp1079.f2=_tmpAEC,_tmp1079)));}_LL759: if(*((int*)_tmpAE7)!= 7)goto _LL75B;
_tmpAED=((struct Cyc_Absyn_ArrayType_struct*)_tmpAE7)->f1;_tmpAEE=_tmpAED.tq;
_LL75A: {struct _tuple10 _tmp107A;return(_tmp107A.f1=_tmpAEE.real_const,((_tmp107A.f2=(
Cyc_Tcutil_addressof_props(te,_tmpABD)).f2,_tmp107A)));}_LL75B:;_LL75C: return
bogus_ans;_LL754:;}_LL723:;_LL724:{const char*_tmp107D;void*_tmp107C;(_tmp107C=0,
Cyc_Tcutil_terr(e->loc,((_tmp107D="unary & applied to non-lvalue",
_tag_dynforward(_tmp107D,sizeof(char),_get_zero_arr_size_char(_tmp107D,30)))),
_tag_dynforward(_tmp107C,sizeof(void*),0)));}return bogus_ans;_LL718:;}void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e);void*Cyc_Tcutil_array_to_ptr(
struct Cyc_Tcenv_Tenv*te,void*e_typ,struct Cyc_Absyn_Exp*e){void*_tmpAF9=Cyc_Tcutil_compress(
e_typ);struct Cyc_Absyn_ArrayInfo _tmpAFA;void*_tmpAFB;struct Cyc_Absyn_Tqual
_tmpAFC;struct Cyc_Absyn_Conref*_tmpAFD;_LL75E: if(_tmpAF9 <= (void*)4)goto _LL760;
if(*((int*)_tmpAF9)!= 7)goto _LL760;_tmpAFA=((struct Cyc_Absyn_ArrayType_struct*)
_tmpAF9)->f1;_tmpAFB=(void*)_tmpAFA.elt_type;_tmpAFC=_tmpAFA.tq;_tmpAFD=_tmpAFA.zero_term;
_LL75F: {void*_tmpAFF;struct _tuple10 _tmpAFE=Cyc_Tcutil_addressof_props(te,e);
_tmpAFF=_tmpAFE.f2;{struct Cyc_Absyn_Upper_b_struct _tmp1080;struct Cyc_Absyn_Upper_b_struct*
_tmp107F;return Cyc_Absyn_atb_typ(_tmpAFB,_tmpAFF,_tmpAFC,(void*)((_tmp107F=
_cycalloc(sizeof(*_tmp107F)),((_tmp107F[0]=((_tmp1080.tag=0,((_tmp1080.f1=e,
_tmp1080)))),_tmp107F)))),_tmpAFD);}}_LL760:;_LL761: return e_typ;_LL75D:;}void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*loc,unsigned int i,struct Cyc_Absyn_Conref*b);void Cyc_Tcutil_check_bound(
struct Cyc_Position_Segment*loc,unsigned int i,struct Cyc_Absyn_Conref*b){b=Cyc_Absyn_compress_conref(
b);{union Cyc_Absyn_Constraint_union _tmpB02=b->v;void*_tmpB03;void*_tmpB04;void*
_tmpB05;struct Cyc_Absyn_Exp*_tmpB06;void*_tmpB07;_LL763: if((_tmpB02.Eq_constr).tag
!= 0)goto _LL765;_tmpB03=(_tmpB02.Eq_constr).f1;if((int)_tmpB03 != 1)goto _LL765;
_LL764: return;_LL765: if((_tmpB02.Eq_constr).tag != 0)goto _LL767;_tmpB04=(_tmpB02.Eq_constr).f1;
if((int)_tmpB04 != 0)goto _LL767;_LL766: if(i < 0){const char*_tmp1083;void*_tmp1082;(
_tmp1082=0,Cyc_Tcutil_terr(loc,((_tmp1083="dereference is out of bounds",
_tag_dynforward(_tmp1083,sizeof(char),_get_zero_arr_size_char(_tmp1083,29)))),
_tag_dynforward(_tmp1082,sizeof(void*),0)));}return;_LL767: if((_tmpB02.Eq_constr).tag
!= 0)goto _LL769;_tmpB05=(_tmpB02.Eq_constr).f1;if(_tmpB05 <= (void*)2)goto _LL769;
if(*((int*)_tmpB05)!= 0)goto _LL769;_tmpB06=((struct Cyc_Absyn_Upper_b_struct*)
_tmpB05)->f1;_LL768: {unsigned int _tmpB0B;int _tmpB0C;struct _tuple7 _tmpB0A=Cyc_Evexp_eval_const_uint_exp(
_tmpB06);_tmpB0B=_tmpB0A.f1;_tmpB0C=_tmpB0A.f2;if(_tmpB0C  && _tmpB0B <= i){const
char*_tmp1088;void*_tmp1087[2];struct Cyc_Int_pa_struct _tmp1086;struct Cyc_Int_pa_struct
_tmp1085;(_tmp1085.tag=1,((_tmp1085.f1=(unsigned long)((int)i),((_tmp1086.tag=1,((
_tmp1086.f1=(unsigned long)((int)_tmpB0B),((_tmp1087[0]=& _tmp1086,((_tmp1087[1]=&
_tmp1085,Cyc_Tcutil_terr(loc,((_tmp1088="dereference is out of bounds: %d <= %d",
_tag_dynforward(_tmp1088,sizeof(char),_get_zero_arr_size_char(_tmp1088,39)))),
_tag_dynforward(_tmp1087,sizeof(void*),2)))))))))))));}return;}_LL769: if((
_tmpB02.Eq_constr).tag != 0)goto _LL76B;_tmpB07=(_tmpB02.Eq_constr).f1;if(_tmpB07
<= (void*)2)goto _LL76B;if(*((int*)_tmpB07)!= 1)goto _LL76B;_LL76A: return;_LL76B:
if((_tmpB02.No_constr).tag != 2)goto _LL76D;_LL76C:{struct Cyc_Absyn_Upper_b_struct*
_tmp108E;struct Cyc_Absyn_Upper_b_struct _tmp108D;union Cyc_Absyn_Constraint_union
_tmp108C;b->v=(union Cyc_Absyn_Constraint_union)(((_tmp108C.Eq_constr).tag=0,(((
_tmp108C.Eq_constr).f1=(void*)((void*)((void*)((_tmp108E=_cycalloc(sizeof(*
_tmp108E)),((_tmp108E[0]=((_tmp108D.tag=0,((_tmp108D.f1=Cyc_Absyn_uint_exp(i + 1,
0),_tmp108D)))),_tmp108E)))))),_tmp108C))));}return;_LL76D:;_LL76E: {const char*
_tmp1091;void*_tmp1090;(_tmp1090=0,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp1091="check_bound -- bad compressed conref",
_tag_dynforward(_tmp1091,sizeof(char),_get_zero_arr_size_char(_tmp1091,37)))),
_tag_dynforward(_tmp1090,sizeof(void*),0)));}_LL762:;}}void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Conref*b);void Cyc_Tcutil_check_nonzero_bound(
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Conref*b){Cyc_Tcutil_check_bound(
loc,0,b);}int Cyc_Tcutil_is_bound_one(struct Cyc_Absyn_Conref*b);int Cyc_Tcutil_is_bound_one(
struct Cyc_Absyn_Conref*b){union Cyc_Absyn_Constraint_union _tmpB16=(Cyc_Absyn_compress_conref(
b))->v;void*_tmpB17;struct Cyc_Absyn_Exp*_tmpB18;_LL770: if((_tmpB16.Eq_constr).tag
!= 0)goto _LL772;_tmpB17=(_tmpB16.Eq_constr).f1;if(_tmpB17 <= (void*)2)goto _LL772;
if(*((int*)_tmpB17)!= 0)goto _LL772;_tmpB18=((struct Cyc_Absyn_Upper_b_struct*)
_tmpB17)->f1;_LL771: {unsigned int _tmpB1A;int _tmpB1B;struct _tuple7 _tmpB19=Cyc_Evexp_eval_const_uint_exp(
_tmpB18);_tmpB1A=_tmpB19.f1;_tmpB1B=_tmpB19.f2;return _tmpB1B  && _tmpB1A == 1;}
_LL772:;_LL773: return 0;_LL76F:;}int Cyc_Tcutil_bits_only(void*t);int Cyc_Tcutil_bits_only(
void*t){void*_tmpB1C=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmpB1D;
void*_tmpB1E;struct Cyc_Absyn_Conref*_tmpB1F;struct Cyc_List_List*_tmpB20;struct
Cyc_Absyn_AggrInfo _tmpB21;union Cyc_Absyn_AggrInfoU_union _tmpB22;struct Cyc_Absyn_AggrInfo
_tmpB23;union Cyc_Absyn_AggrInfoU_union _tmpB24;struct Cyc_Absyn_Aggrdecl**_tmpB25;
struct Cyc_Absyn_Aggrdecl*_tmpB26;struct Cyc_List_List*_tmpB27;struct Cyc_List_List*
_tmpB28;_LL775: if((int)_tmpB1C != 0)goto _LL777;_LL776: goto _LL778;_LL777: if(
_tmpB1C <= (void*)4)goto _LL779;if(*((int*)_tmpB1C)!= 5)goto _LL779;_LL778: goto
_LL77A;_LL779: if((int)_tmpB1C != 1)goto _LL77B;_LL77A: goto _LL77C;_LL77B: if(_tmpB1C
<= (void*)4)goto _LL78B;if(*((int*)_tmpB1C)!= 6)goto _LL77D;_LL77C: return 1;_LL77D:
if(*((int*)_tmpB1C)!= 12)goto _LL77F;_LL77E: goto _LL780;_LL77F: if(*((int*)_tmpB1C)
!= 13)goto _LL781;_LL780: return 0;_LL781: if(*((int*)_tmpB1C)!= 7)goto _LL783;
_tmpB1D=((struct Cyc_Absyn_ArrayType_struct*)_tmpB1C)->f1;_tmpB1E=(void*)_tmpB1D.elt_type;
_tmpB1F=_tmpB1D.zero_term;_LL782: return !((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmpB1F) && Cyc_Tcutil_bits_only(_tmpB1E);_LL783: if(*((
int*)_tmpB1C)!= 9)goto _LL785;_tmpB20=((struct Cyc_Absyn_TupleType_struct*)_tmpB1C)->f1;
_LL784: for(0;_tmpB20 != 0;_tmpB20=_tmpB20->tl){if(!Cyc_Tcutil_bits_only((*((
struct _tuple5*)_tmpB20->hd)).f2))return 0;}return 1;_LL785: if(*((int*)_tmpB1C)!= 
10)goto _LL787;_tmpB21=((struct Cyc_Absyn_AggrType_struct*)_tmpB1C)->f1;_tmpB22=
_tmpB21.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmpB1C)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL787;_LL786: return 0;_LL787: if(*((int*)_tmpB1C)!= 10)goto _LL789;
_tmpB23=((struct Cyc_Absyn_AggrType_struct*)_tmpB1C)->f1;_tmpB24=_tmpB23.aggr_info;
if((((((struct Cyc_Absyn_AggrType_struct*)_tmpB1C)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL789;_tmpB25=(_tmpB24.KnownAggr).f1;_tmpB26=*_tmpB25;_tmpB27=_tmpB23.targs;
_LL788: if(_tmpB26->impl == 0)return 0;{struct _RegionHandle _tmpB29=_new_region("rgn");
struct _RegionHandle*rgn=& _tmpB29;_push_region(rgn);{struct Cyc_List_List*_tmpB2A=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpB26->tvs,_tmpB27);{struct Cyc_List_List*
fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB26->impl))->fields;for(0;fs
!= 0;fs=fs->tl){if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpB2A,(
void*)((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpB2B=0;_npop_handler(0);
return _tmpB2B;}}}{int _tmpB2C=1;_npop_handler(0);return _tmpB2C;}};_pop_region(rgn);}
_LL789: if(*((int*)_tmpB1C)!= 11)goto _LL78B;_tmpB28=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmpB1C)->f2;_LL78A: for(0;_tmpB28 != 0;_tmpB28=_tmpB28->tl){if(!Cyc_Tcutil_bits_only((
void*)((struct Cyc_Absyn_Aggrfield*)_tmpB28->hd)->type))return 0;}return 1;_LL78B:;
_LL78C: return 0;_LL774:;}struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static int Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*
e);static int Cyc_Tcutil_cnst_exp(struct Cyc_Tcenv_Tenv*te,int var_okay,struct Cyc_Absyn_Exp*
e){void*_tmpB2D=(void*)e->r;struct _tuple2*_tmpB2E;void*_tmpB2F;struct Cyc_Absyn_Exp*
_tmpB30;struct Cyc_Absyn_Exp*_tmpB31;struct Cyc_Absyn_Exp*_tmpB32;struct Cyc_Absyn_Exp*
_tmpB33;struct Cyc_Absyn_Exp*_tmpB34;struct Cyc_Absyn_Exp*_tmpB35;struct Cyc_Absyn_Exp*
_tmpB36;void*_tmpB37;struct Cyc_Absyn_Exp*_tmpB38;void*_tmpB39;void*_tmpB3A;
struct Cyc_Absyn_Exp*_tmpB3B;struct Cyc_Absyn_Exp*_tmpB3C;struct Cyc_Absyn_Exp*
_tmpB3D;struct Cyc_Absyn_Exp*_tmpB3E;struct Cyc_List_List*_tmpB3F;struct Cyc_List_List*
_tmpB40;struct Cyc_List_List*_tmpB41;void*_tmpB42;struct Cyc_List_List*_tmpB43;
struct Cyc_List_List*_tmpB44;struct Cyc_List_List*_tmpB45;_LL78E: if(*((int*)
_tmpB2D)!= 0)goto _LL790;_LL78F: goto _LL791;_LL790: if(*((int*)_tmpB2D)!= 18)goto
_LL792;_LL791: goto _LL793;_LL792: if(*((int*)_tmpB2D)!= 19)goto _LL794;_LL793: goto
_LL795;_LL794: if(*((int*)_tmpB2D)!= 20)goto _LL796;_LL795: goto _LL797;_LL796: if(*((
int*)_tmpB2D)!= 21)goto _LL798;_LL797: goto _LL799;_LL798: if(*((int*)_tmpB2D)!= 33)
goto _LL79A;_LL799: goto _LL79B;_LL79A: if(*((int*)_tmpB2D)!= 34)goto _LL79C;_LL79B:
return 1;_LL79C: if(*((int*)_tmpB2D)!= 1)goto _LL79E;_tmpB2E=((struct Cyc_Absyn_Var_e_struct*)
_tmpB2D)->f1;_tmpB2F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpB2D)->f2;_LL79D: {
void*_tmpB46=_tmpB2F;struct Cyc_Absyn_Vardecl*_tmpB47;_LL7BD: if(_tmpB46 <= (void*)
1)goto _LL7C1;if(*((int*)_tmpB46)!= 1)goto _LL7BF;_LL7BE: return 1;_LL7BF: if(*((int*)
_tmpB46)!= 0)goto _LL7C1;_tmpB47=((struct Cyc_Absyn_Global_b_struct*)_tmpB46)->f1;
_LL7C0: {void*_tmpB48=Cyc_Tcutil_compress((void*)_tmpB47->type);_LL7C6: if(
_tmpB48 <= (void*)4)goto _LL7CA;if(*((int*)_tmpB48)!= 7)goto _LL7C8;_LL7C7: goto
_LL7C9;_LL7C8: if(*((int*)_tmpB48)!= 8)goto _LL7CA;_LL7C9: return 1;_LL7CA:;_LL7CB:
return var_okay;_LL7C5:;}_LL7C1: if((int)_tmpB46 != 0)goto _LL7C3;_LL7C2: return 0;
_LL7C3:;_LL7C4: return var_okay;_LL7BC:;}_LL79E: if(*((int*)_tmpB2D)!= 6)goto _LL7A0;
_tmpB30=((struct Cyc_Absyn_Conditional_e_struct*)_tmpB2D)->f1;_tmpB31=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpB2D)->f2;_tmpB32=((struct Cyc_Absyn_Conditional_e_struct*)_tmpB2D)->f3;_LL79F:
return(Cyc_Tcutil_cnst_exp(te,0,_tmpB30) && Cyc_Tcutil_cnst_exp(te,0,_tmpB31))
 && Cyc_Tcutil_cnst_exp(te,0,_tmpB32);_LL7A0: if(*((int*)_tmpB2D)!= 9)goto _LL7A2;
_tmpB33=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpB2D)->f1;_tmpB34=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpB2D)->f2;_LL7A1: return Cyc_Tcutil_cnst_exp(te,0,_tmpB33) && Cyc_Tcutil_cnst_exp(
te,0,_tmpB34);_LL7A2: if(*((int*)_tmpB2D)!= 13)goto _LL7A4;_tmpB35=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpB2D)->f1;_LL7A3: _tmpB36=_tmpB35;goto _LL7A5;_LL7A4: if(*((int*)_tmpB2D)!= 14)
goto _LL7A6;_tmpB36=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpB2D)->f1;_LL7A5:
return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpB36);_LL7A6: if(*((int*)_tmpB2D)!= 15)
goto _LL7A8;_tmpB37=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpB2D)->f1;_tmpB38=((
struct Cyc_Absyn_Cast_e_struct*)_tmpB2D)->f2;_tmpB39=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmpB2D)->f4;if((int)_tmpB39 != 1)goto _LL7A8;_LL7A7: return Cyc_Tcutil_cnst_exp(te,
var_okay,_tmpB38);_LL7A8: if(*((int*)_tmpB2D)!= 15)goto _LL7AA;_tmpB3A=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmpB2D)->f1;_tmpB3B=((struct Cyc_Absyn_Cast_e_struct*)
_tmpB2D)->f2;_LL7A9: return Cyc_Tcutil_cnst_exp(te,var_okay,_tmpB3B);_LL7AA: if(*((
int*)_tmpB2D)!= 16)goto _LL7AC;_tmpB3C=((struct Cyc_Absyn_Address_e_struct*)
_tmpB2D)->f1;_LL7AB: return Cyc_Tcutil_cnst_exp(te,1,_tmpB3C);_LL7AC: if(*((int*)
_tmpB2D)!= 29)goto _LL7AE;_tmpB3D=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpB2D)->f2;_tmpB3E=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpB2D)->f3;
_LL7AD: return Cyc_Tcutil_cnst_exp(te,0,_tmpB3D) && Cyc_Tcutil_cnst_exp(te,0,
_tmpB3E);_LL7AE: if(*((int*)_tmpB2D)!= 28)goto _LL7B0;_tmpB3F=((struct Cyc_Absyn_Array_e_struct*)
_tmpB2D)->f1;_LL7AF: _tmpB40=_tmpB3F;goto _LL7B1;_LL7B0: if(*((int*)_tmpB2D)!= 31)
goto _LL7B2;_tmpB40=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmpB2D)->f2;_LL7B1:
_tmpB41=_tmpB40;goto _LL7B3;_LL7B2: if(*((int*)_tmpB2D)!= 30)goto _LL7B4;_tmpB41=((
struct Cyc_Absyn_Struct_e_struct*)_tmpB2D)->f3;_LL7B3: for(0;_tmpB41 != 0;_tmpB41=
_tmpB41->tl){if(!Cyc_Tcutil_cnst_exp(te,0,(*((struct _tuple23*)_tmpB41->hd)).f2))
return 0;}return 1;_LL7B4: if(*((int*)_tmpB2D)!= 3)goto _LL7B6;_tmpB42=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmpB2D)->f1;_tmpB43=((struct Cyc_Absyn_Primop_e_struct*)
_tmpB2D)->f2;_LL7B5: _tmpB44=_tmpB43;goto _LL7B7;_LL7B6: if(*((int*)_tmpB2D)!= 26)
goto _LL7B8;_tmpB44=((struct Cyc_Absyn_Tuple_e_struct*)_tmpB2D)->f1;_LL7B7: _tmpB45=
_tmpB44;goto _LL7B9;_LL7B8: if(*((int*)_tmpB2D)!= 32)goto _LL7BA;_tmpB45=((struct
Cyc_Absyn_Tunion_e_struct*)_tmpB2D)->f1;_LL7B9: for(0;_tmpB45 != 0;_tmpB45=_tmpB45->tl){
if(!Cyc_Tcutil_cnst_exp(te,0,(struct Cyc_Absyn_Exp*)_tmpB45->hd))return 0;}return 1;
_LL7BA:;_LL7BB: return 0;_LL78D:;}int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Exp*e);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e){return Cyc_Tcutil_cnst_exp(te,0,e);}static int Cyc_Tcutil_fields_support_default(
struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);int Cyc_Tcutil_supports_default(
void*t);int Cyc_Tcutil_supports_default(void*t){void*_tmpB49=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmpB4A;struct Cyc_Absyn_PtrAtts _tmpB4B;struct Cyc_Absyn_Conref*
_tmpB4C;struct Cyc_Absyn_Conref*_tmpB4D;struct Cyc_Absyn_ArrayInfo _tmpB4E;void*
_tmpB4F;struct Cyc_List_List*_tmpB50;struct Cyc_Absyn_AggrInfo _tmpB51;union Cyc_Absyn_AggrInfoU_union
_tmpB52;struct Cyc_List_List*_tmpB53;struct Cyc_List_List*_tmpB54;_LL7CD: if((int)
_tmpB49 != 0)goto _LL7CF;_LL7CE: goto _LL7D0;_LL7CF: if(_tmpB49 <= (void*)4)goto _LL7D1;
if(*((int*)_tmpB49)!= 5)goto _LL7D1;_LL7D0: goto _LL7D2;_LL7D1: if((int)_tmpB49 != 1)
goto _LL7D3;_LL7D2: goto _LL7D4;_LL7D3: if(_tmpB49 <= (void*)4)goto _LL7E3;if(*((int*)
_tmpB49)!= 6)goto _LL7D5;_LL7D4: return 1;_LL7D5: if(*((int*)_tmpB49)!= 4)goto _LL7D7;
_tmpB4A=((struct Cyc_Absyn_PointerType_struct*)_tmpB49)->f1;_tmpB4B=_tmpB4A.ptr_atts;
_tmpB4C=_tmpB4B.nullable;_tmpB4D=_tmpB4B.bounds;_LL7D6: {void*_tmpB55=Cyc_Absyn_conref_def((
void*)((void*)0),_tmpB4D);_LL7E6: if((int)_tmpB55 != 0)goto _LL7E8;_LL7E7: return 1;
_LL7E8: if((int)_tmpB55 != 1)goto _LL7EA;_LL7E9: return 1;_LL7EA:;_LL7EB: return((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmpB4C);_LL7E5:;}_LL7D7:
if(*((int*)_tmpB49)!= 7)goto _LL7D9;_tmpB4E=((struct Cyc_Absyn_ArrayType_struct*)
_tmpB49)->f1;_tmpB4F=(void*)_tmpB4E.elt_type;_LL7D8: return Cyc_Tcutil_supports_default(
_tmpB4F);_LL7D9: if(*((int*)_tmpB49)!= 9)goto _LL7DB;_tmpB50=((struct Cyc_Absyn_TupleType_struct*)
_tmpB49)->f1;_LL7DA: for(0;_tmpB50 != 0;_tmpB50=_tmpB50->tl){if(!Cyc_Tcutil_supports_default((*((
struct _tuple5*)_tmpB50->hd)).f2))return 0;}return 1;_LL7DB: if(*((int*)_tmpB49)!= 
10)goto _LL7DD;_tmpB51=((struct Cyc_Absyn_AggrType_struct*)_tmpB49)->f1;_tmpB52=
_tmpB51.aggr_info;_tmpB53=_tmpB51.targs;_LL7DC: {struct Cyc_Absyn_Aggrdecl*
_tmpB56=Cyc_Absyn_get_known_aggrdecl(_tmpB52);if(_tmpB56->impl == 0)return 0;if(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB56->impl))->exist_vars != 0)return
0;return Cyc_Tcutil_fields_support_default(_tmpB56->tvs,_tmpB53,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmpB56->impl))->fields);}_LL7DD: if(*((int*)_tmpB49)!= 11)goto _LL7DF;
_tmpB54=((struct Cyc_Absyn_AnonAggrType_struct*)_tmpB49)->f2;_LL7DE: return Cyc_Tcutil_fields_support_default(
0,0,_tmpB54);_LL7DF: if(*((int*)_tmpB49)!= 13)goto _LL7E1;_LL7E0: goto _LL7E2;_LL7E1:
if(*((int*)_tmpB49)!= 12)goto _LL7E3;_LL7E2: return 1;_LL7E3:;_LL7E4: return 0;_LL7CC:;}
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*
ts,struct Cyc_List_List*fs);static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*
tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){{struct _RegionHandle _tmpB57=
_new_region("rgn");struct _RegionHandle*rgn=& _tmpB57;_push_region(rgn);{struct Cyc_List_List*
_tmpB58=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);for(
0;fs != 0;fs=fs->tl){void*t=Cyc_Tcutil_rsubstitute(rgn,_tmpB58,(void*)((struct Cyc_Absyn_Aggrfield*)
fs->hd)->type);if(!Cyc_Tcutil_supports_default(t)){int _tmpB59=0;_npop_handler(0);
return _tmpB59;}}};_pop_region(rgn);}return 1;}int Cyc_Tcutil_admits_zero(void*t);
int Cyc_Tcutil_admits_zero(void*t){void*_tmpB5A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpB5B;struct Cyc_Absyn_PtrAtts _tmpB5C;struct Cyc_Absyn_Conref*_tmpB5D;struct Cyc_Absyn_Conref*
_tmpB5E;_LL7ED: if(_tmpB5A <= (void*)4)goto _LL7EF;if(*((int*)_tmpB5A)!= 5)goto
_LL7EF;_LL7EE: goto _LL7F0;_LL7EF: if((int)_tmpB5A != 1)goto _LL7F1;_LL7F0: goto _LL7F2;
_LL7F1: if(_tmpB5A <= (void*)4)goto _LL7F5;if(*((int*)_tmpB5A)!= 6)goto _LL7F3;
_LL7F2: return 1;_LL7F3: if(*((int*)_tmpB5A)!= 4)goto _LL7F5;_tmpB5B=((struct Cyc_Absyn_PointerType_struct*)
_tmpB5A)->f1;_tmpB5C=_tmpB5B.ptr_atts;_tmpB5D=_tmpB5C.nullable;_tmpB5E=_tmpB5C.bounds;
_LL7F4: {union Cyc_Absyn_Constraint_union _tmpB5F=(Cyc_Absyn_compress_conref(
_tmpB5E))->v;void*_tmpB60;void*_tmpB61;_LL7F8: if((_tmpB5F.Eq_constr).tag != 0)
goto _LL7FA;_tmpB60=(_tmpB5F.Eq_constr).f1;if((int)_tmpB60 != 0)goto _LL7FA;_LL7F9:
return 0;_LL7FA: if((_tmpB5F.Eq_constr).tag != 0)goto _LL7FC;_tmpB61=(_tmpB5F.Eq_constr).f1;
if((int)_tmpB61 != 1)goto _LL7FC;_LL7FB: return 0;_LL7FC: if((_tmpB5F.Eq_constr).tag
!= 0)goto _LL7FE;_LL7FD: {union Cyc_Absyn_Constraint_union _tmpB62=(((struct Cyc_Absyn_Conref*(*)(
struct Cyc_Absyn_Conref*x))Cyc_Absyn_compress_conref)(_tmpB5D))->v;int _tmpB63;
_LL801: if((_tmpB62.Eq_constr).tag != 0)goto _LL803;_tmpB63=(_tmpB62.Eq_constr).f1;
_LL802: return _tmpB63;_LL803:;_LL804: return 0;_LL800:;}_LL7FE:;_LL7FF: return 0;
_LL7F7:;}_LL7F5:;_LL7F6: return 0;_LL7EC:;}int Cyc_Tcutil_is_noreturn(void*t);int
Cyc_Tcutil_is_noreturn(void*t){{void*_tmpB64=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo
_tmpB65;void*_tmpB66;struct Cyc_Absyn_FnInfo _tmpB67;struct Cyc_List_List*_tmpB68;
_LL806: if(_tmpB64 <= (void*)4)goto _LL80A;if(*((int*)_tmpB64)!= 4)goto _LL808;
_tmpB65=((struct Cyc_Absyn_PointerType_struct*)_tmpB64)->f1;_tmpB66=(void*)
_tmpB65.elt_typ;_LL807: return Cyc_Tcutil_is_noreturn(_tmpB66);_LL808: if(*((int*)
_tmpB64)!= 8)goto _LL80A;_tmpB67=((struct Cyc_Absyn_FnType_struct*)_tmpB64)->f1;
_tmpB68=_tmpB67.attributes;_LL809: for(0;_tmpB68 != 0;_tmpB68=_tmpB68->tl){void*
_tmpB69=(void*)_tmpB68->hd;_LL80D: if((int)_tmpB69 != 3)goto _LL80F;_LL80E: return 1;
_LL80F:;_LL810: continue;_LL80C:;}goto _LL805;_LL80A:;_LL80B: goto _LL805;_LL805:;}
return 0;}struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*
atts);struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*
atts){void*_tmpB6A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_FnInfo _tmpB6B;struct
Cyc_List_List*_tmpB6C;struct Cyc_List_List**_tmpB6D;_LL812: if(_tmpB6A <= (void*)4)
goto _LL814;if(*((int*)_tmpB6A)!= 8)goto _LL814;_tmpB6B=((struct Cyc_Absyn_FnType_struct*)
_tmpB6A)->f1;_tmpB6C=_tmpB6B.attributes;_tmpB6D=(struct Cyc_List_List**)&(((
struct Cyc_Absyn_FnType_struct*)_tmpB6A)->f1).attributes;_LL813: {struct Cyc_List_List*
_tmpB6E=0;for(0;atts != 0;atts=atts->tl){if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp1092;*_tmpB6D=((_tmp1092=_cycalloc(sizeof(*_tmp1092)),((
_tmp1092->hd=(void*)((void*)atts->hd),((_tmp1092->tl=*_tmpB6D,_tmp1092))))));}
else{struct Cyc_List_List*_tmp1093;_tmpB6E=((_tmp1093=_cycalloc(sizeof(*_tmp1093)),((
_tmp1093->hd=(void*)((void*)atts->hd),((_tmp1093->tl=_tmpB6E,_tmp1093))))));}}
return _tmpB6E;}_LL814:;_LL815: {const char*_tmp1096;void*_tmp1095;(_tmp1095=0,((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmp1096="transfer_fn_type_atts",_tag_dynforward(_tmp1096,sizeof(char),
_get_zero_arr_size_char(_tmp1096,22)))),_tag_dynforward(_tmp1095,sizeof(void*),0)));}
_LL811:;}
