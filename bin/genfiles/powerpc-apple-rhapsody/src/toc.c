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
_dynforward_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_Core_NewRegion
Cyc_Core_new_dynregion();extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[
16];void Cyc_Core_free_dynregion(struct _DynRegionHandle*);struct Cyc___cycFILE;
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;
};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct
Cyc___cycFILE*,struct _dynforward_ptr,struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dynforward_ptr f1;}
;struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dynforward_ptr,struct _dynforward_ptr);
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_list(struct _dynforward_ptr);int Cyc_List_length(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(
void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap_c(struct
_RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char
Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*
x);void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rrev(
struct _RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];void*Cyc_List_nth(struct Cyc_List_List*
x,int n);int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);int Cyc_strcmp(
struct _dynforward_ptr s1,struct _dynforward_ptr s2);struct _dynforward_ptr Cyc_strconcat(
struct _dynforward_ptr,struct _dynforward_ptr);struct Cyc_Iter_Iter{void*env;int(*
next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct
Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(
void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*
s,void*elt);int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[
11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct
_RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern char
Cyc_Dict_Absent[11];struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*
cmp)(void*,void*));struct Cyc_Dict_Dict Cyc_Dict_rshare(struct _RegionHandle*,
struct Cyc_Dict_Dict);struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,
void*k,void*v);void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);void**Cyc_Dict_lookup_opt(
struct Cyc_Dict_Dict d,void*k);struct _tuple0{void*f1;void*f2;};struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_Lineno_Pos{struct
_dynforward_ptr logical_file;struct _dynforward_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct _dynforward_ptr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _dynforward_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dynforward_ptr desc;};
extern char Cyc_Position_Nocontext[14];struct Cyc_Xarray_Xarray{struct _RegionHandle*
r;struct _dynforward_ptr elmts;int num_elmts;};int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(
struct _RegionHandle*);int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple1*,struct _tuple1*);struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref;extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dynforward_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct
_dynforward_ptr*name);void*Cyc_Absyn_strctq(struct _tuple1*name);void*Cyc_Absyn_unionq_typ(
struct _tuple1*name);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual
tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*
ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*
Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst_union,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_int_exp(void*,int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(
int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(
unsigned int,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(
char c,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(
struct _dynforward_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dynforward_ptr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(
struct _tuple1*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(
struct _tuple1*,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(
void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,
void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(
void*t,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(
struct Cyc_Absyn_Exp*,struct _dynforward_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dynforward_ptr*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_match_exn_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(
struct Cyc_Core_Opt*,struct Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*
Cyc_Absyn_new_stmt(void*s,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_skip_stmt(struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(
struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,
struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct
Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*
init,struct Cyc_Absyn_Stmt*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*
Cyc_Absyn_label_stmt(struct _dynforward_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dynforward_ptr*lab,struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*
e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*
Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*
Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);int Cyc_Absyn_is_lvalue(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _dynforward_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dynforward_ptr*);struct _dynforward_ptr*Cyc_Absyn_fieldname(
int);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU_union
info);extern int Cyc_Absyn_no_regions;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct
Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dynforward_ptr Cyc_Absynpp_typ2string(void*);struct
_dynforward_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_dynforward_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);struct
_dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_RgnOrder_RgnPO;
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct
Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*
fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(
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
_dynforward_ptr fmt,struct _dynforward_ptr ap);void*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*
t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_is_pointer_type(
void*t);int Cyc_Tcutil_is_pointer_or_boxed(void*t,int*is_dynforward_ptr,int*
is_dyneither_ptr);int Cyc_Tcutil_unify(void*,void*);int Cyc_Tcutil_typecmp(void*,
void*);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(struct
_RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct
Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*,int*
is_forward_only);int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**
elt_typ_dest,int*is_forward_only);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,
void**elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**
ptr_type,int*is_dynforward,int*is_dyneither,void**elt_type);struct _tuple4{struct
Cyc_Absyn_Tqual f1;void*f2;};void*Cyc_Tcutil_snd_tqt(struct _tuple4*);struct
_tuple5{unsigned int f1;int f2;};struct _tuple5 Cyc_Evexp_eval_const_uint_exp(struct
Cyc_Absyn_Exp*e);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
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
f1;};struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);struct _tuple1*Cyc_Toc_temp_var();
extern struct _dynforward_ptr Cyc_Toc_globals;extern int Cyc_noexpand_r;int Cyc_Toc_warn_bounds_checks=
0;int Cyc_Toc_warn_all_null_deref=0;unsigned int Cyc_Toc_total_bounds_checks=0;
unsigned int Cyc_Toc_bounds_checks_eliminated=0;static struct Cyc_List_List*Cyc_Toc_result_decls=
0;struct Cyc_Toc_TocState{struct _DynRegionHandle*dyn;struct Cyc_List_List**
tuple_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_Set_Set**tunions_so_far;
struct Cyc_Dict_Dict*xtunions_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*
temp_labels;};static struct Cyc_Toc_TocState*Cyc_Toc_toc_state=0;struct _tuple6{
struct _tuple1*f1;struct _dynforward_ptr f2;};int Cyc_Toc_qvar_tag_cmp(struct _tuple6*
x,struct _tuple6*y);int Cyc_Toc_qvar_tag_cmp(struct _tuple6*x,struct _tuple6*y){
struct _tuple1*_tmp1;struct _dynforward_ptr _tmp2;struct _tuple6 _tmp0=*x;_tmp1=_tmp0.f1;
_tmp2=_tmp0.f2;{struct _tuple1*_tmp4;struct _dynforward_ptr _tmp5;struct _tuple6
_tmp3=*y;_tmp4=_tmp3.f1;_tmp5=_tmp3.f2;{int i=Cyc_Absyn_qvar_cmp(_tmp1,_tmp4);if(
i != 0)return i;return Cyc_strcmp((struct _dynforward_ptr)_tmp2,(struct
_dynforward_ptr)_tmp5);}}}struct _tuple7{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*));
void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _DynRegionHandle*_tmp7;struct Cyc_Dict_Dict*_tmp8;struct Cyc_Toc_TocState
_tmp6=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp7=_tmp6.dyn;
_tmp8=_tmp6.aggrs_so_far;{struct _DynRegionFrame _tmp9;struct _RegionHandle*d=
_open_dynregion(& _tmp9,_tmp7);{struct _tuple7**v=((struct _tuple7**(*)(struct Cyc_Dict_Dict
d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp8,q);if(v == 0){void*_tmpA=
type_maker(q);_npop_handler(0);return _tmpA;}else{struct _tuple7 _tmpC;void*_tmpD;
struct _tuple7*_tmpB=*v;_tmpC=*_tmpB;_tmpD=_tmpC.f2;{void*_tmpE=_tmpD;
_npop_handler(0);return _tmpE;}}};_pop_dynregion(d);}}static int Cyc_Toc_tuple_type_counter=
0;static int Cyc_Toc_temp_var_counter=0;static int Cyc_Toc_fresh_label_counter=0;
char Cyc_Toc_Toc_Unimplemented[22]="\000\000\000\000Toc_Unimplemented\000";char
Cyc_Toc_Toc_Impossible[19]="\000\000\000\000Toc_Impossible\000";static void*Cyc_Toc_unimp(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap);static void*Cyc_Toc_unimp(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);{
const char*_tmp87A;void*_tmp879;(_tmp879=0,Cyc_fprintf(Cyc_stderr,((_tmp87A="\n",
_tag_dynforward(_tmp87A,sizeof(char),_get_zero_arr_size_char(_tmp87A,2)))),
_tag_dynforward(_tmp879,sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(
int)_throw((void*)Cyc_Toc_Toc_Unimplemented);}static void*Cyc_Toc_toc_impos(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap);static void*Cyc_Toc_toc_impos(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);{
const char*_tmp87D;void*_tmp87C;(_tmp87C=0,Cyc_fprintf(Cyc_stderr,((_tmp87D="\n",
_tag_dynforward(_tmp87D,sizeof(char),_get_zero_arr_size_char(_tmp87D,2)))),
_tag_dynforward(_tmp87C,sizeof(void*),0)));}Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(
int)_throw((void*)Cyc_Toc_Toc_Impossible);}char Cyc_Toc_Match_error[16]="\000\000\000\000Match_error\000";
static char _tmp13[5]="curr";static struct _dynforward_ptr Cyc_Toc_curr_string={
_tmp13,_tmp13 + 5};static struct _dynforward_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;
static char _tmp14[4]="tag";static struct _dynforward_ptr Cyc_Toc_tag_string={_tmp14,
_tmp14 + 4};static struct _dynforward_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static
char _tmp15[4]="val";static struct _dynforward_ptr Cyc_Toc_val_string={_tmp15,_tmp15
+ 4};static struct _dynforward_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char
_tmp16[14]="_handler_cons";static struct _dynforward_ptr Cyc_Toc__handler_cons_string={
_tmp16,_tmp16 + 14};static struct _dynforward_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;
static char _tmp17[8]="handler";static struct _dynforward_ptr Cyc_Toc_handler_string={
_tmp17,_tmp17 + 8};static struct _dynforward_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;
static char _tmp18[14]="_RegionHandle";static struct _dynforward_ptr Cyc_Toc__RegionHandle_string={
_tmp18,_tmp18 + 14};static struct _dynforward_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;
static char _tmp19[17]="_DynRegionHandle";static struct _dynforward_ptr Cyc_Toc__DynRegionHandle_string={
_tmp19,_tmp19 + 17};static struct _dynforward_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;
static char _tmp1A[16]="_DynRegionFrame";static struct _dynforward_ptr Cyc_Toc__DynRegionFrame_string={
_tmp1A,_tmp1A + 16};static struct _dynforward_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;
struct _dynforward_ptr Cyc_Toc_globals={(void*)0,(void*)(0 + 0)};static char _tmp1B[7]="_throw";
static struct _dynforward_ptr Cyc_Toc__throw_str={_tmp1B,_tmp1B + 7};static struct
_tuple1 Cyc_Toc__throw_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={1,&
Cyc_Toc__throw_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={
0,(void*)((void*)& Cyc_Toc__throw_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp1E[7]="setjmp";
static struct _dynforward_ptr Cyc_Toc_setjmp_str={_tmp1E,_tmp1E + 7};static struct
_tuple1 Cyc_Toc_setjmp_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc_setjmp_re={1,&
Cyc_Toc_setjmp_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={
0,(void*)((void*)& Cyc_Toc_setjmp_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp21[
14]="_push_handler";static struct _dynforward_ptr Cyc_Toc__push_handler_str={_tmp21,
_tmp21 + 14};static struct _tuple1 Cyc_Toc__push_handler_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)((void*)& Cyc_Toc__push_handler_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=&
Cyc_Toc__push_handler_ev;static char _tmp24[13]="_pop_handler";static struct
_dynforward_ptr Cyc_Toc__pop_handler_str={_tmp24,_tmp24 + 13};static struct _tuple1
Cyc_Toc__pop_handler_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={
1,& Cyc_Toc__pop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={
0,(void*)((void*)& Cyc_Toc__pop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static
char _tmp27[12]="_exn_thrown";static struct _dynforward_ptr Cyc_Toc__exn_thrown_str={
_tmp27,_tmp27 + 12};static struct _tuple1 Cyc_Toc__exn_thrown_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__exn_thrown_re={1,& Cyc_Toc__exn_thrown_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)((void*)& Cyc_Toc__exn_thrown_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=&
Cyc_Toc__exn_thrown_ev;static char _tmp2A[14]="_npop_handler";static struct
_dynforward_ptr Cyc_Toc__npop_handler_str={_tmp2A,_tmp2A + 14};static struct _tuple1
Cyc_Toc__npop_handler_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={
1,& Cyc_Toc__npop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={
0,(void*)((void*)& Cyc_Toc__npop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;
static char _tmp2D[12]="_check_null";static struct _dynforward_ptr Cyc_Toc__check_null_str={
_tmp2D,_tmp2D + 12};static struct _tuple1 Cyc_Toc__check_null_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)((void*)& Cyc_Toc__check_null_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=&
Cyc_Toc__check_null_ev;static char _tmp30[28]="_check_known_subscript_null";static
struct _dynforward_ptr Cyc_Toc__check_known_subscript_null_str={_tmp30,_tmp30 + 28};
static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_known_subscript_null_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,(void*)((void*)& Cyc_Toc__check_known_subscript_null_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static char _tmp33[31]="_check_known_subscript_notnull";static struct
_dynforward_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp33,_tmp33 + 31};
static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_known_subscript_notnull_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={1,&
Cyc_Toc__check_known_subscript_notnull_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)((void*)& Cyc_Toc__check_known_subscript_notnull_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=&
Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp36[28]="_check_dynforward_subscript";
static struct _dynforward_ptr Cyc_Toc__check_dynforward_subscript_str={_tmp36,
_tmp36 + 28};static struct _tuple1 Cyc_Toc__check_dynforward_subscript_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_dynforward_subscript_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_dynforward_subscript_re={1,& Cyc_Toc__check_dynforward_subscript_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dynforward_subscript_ev={
0,(void*)((void*)& Cyc_Toc__check_dynforward_subscript_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dynforward_subscript_e=& Cyc_Toc__check_dynforward_subscript_ev;
static char _tmp39[27]="_check_dyneither_subscript";static struct _dynforward_ptr Cyc_Toc__check_dyneither_subscript_str={
_tmp39,_tmp39 + 27};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_dyneither_subscript_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_dyneither_subscript_re={1,& Cyc_Toc__check_dyneither_subscript_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={
0,(void*)((void*)& Cyc_Toc__check_dyneither_subscript_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;
static char _tmp3C[16]="_dynforward_ptr";static struct _dynforward_ptr Cyc_Toc__dynforward_ptr_str={
_tmp3C,_tmp3C + 16};static struct _tuple1 Cyc_Toc__dynforward_ptr_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__dynforward_ptr_re={1,& Cyc_Toc__dynforward_ptr_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_e=&
Cyc_Toc__dynforward_ptr_ev;static char _tmp3F[15]="_dyneither_ptr";static struct
_dynforward_ptr Cyc_Toc__dyneither_ptr_str={_tmp3F,_tmp3F + 15};static struct
_tuple1 Cyc_Toc__dyneither_ptr_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_re={
1,& Cyc_Toc__dyneither_ptr_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;
static char _tmp42[16]="_tag_dynforward";static struct _dynforward_ptr Cyc_Toc__tag_dynforward_str={
_tmp42,_tmp42 + 16};static struct _tuple1 Cyc_Toc__tag_dynforward_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__tag_dynforward_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__tag_dynforward_re={1,& Cyc_Toc__tag_dynforward_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__tag_dynforward_ev={0,(void*)((void*)& Cyc_Toc__tag_dynforward_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dynforward_e=&
Cyc_Toc__tag_dynforward_ev;static char _tmp45[15]="_tag_dyneither";static struct
_dynforward_ptr Cyc_Toc__tag_dyneither_str={_tmp45,_tmp45 + 15};static struct
_tuple1 Cyc_Toc__tag_dyneither_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_dyneither_re={
1,& Cyc_Toc__tag_dyneither_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={
0,(void*)((void*)& Cyc_Toc__tag_dyneither_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;
static char _tmp48[21]="_init_dynforward_ptr";static struct _dynforward_ptr Cyc_Toc__init_dynforward_ptr_str={
_tmp48,_tmp48 + 21};static struct _tuple1 Cyc_Toc__init_dynforward_ptr_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__init_dynforward_ptr_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__init_dynforward_ptr_re={1,& Cyc_Toc__init_dynforward_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__init_dynforward_ptr_ev={0,(
void*)((void*)& Cyc_Toc__init_dynforward_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__init_dynforward_ptr_e=& Cyc_Toc__init_dynforward_ptr_ev;
static char _tmp4B[20]="_init_dyneither_ptr";static struct _dynforward_ptr Cyc_Toc__init_dyneither_ptr_str={
_tmp4B,_tmp4B + 20};static struct _tuple1 Cyc_Toc__init_dyneither_ptr_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__init_dyneither_ptr_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__init_dyneither_ptr_re={1,& Cyc_Toc__init_dyneither_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(
void*)((void*)& Cyc_Toc__init_dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=& Cyc_Toc__init_dyneither_ptr_ev;
static char _tmp4E[22]="_untag_dynforward_ptr";static struct _dynforward_ptr Cyc_Toc__untag_dynforward_ptr_str={
_tmp4E,_tmp4E + 22};static struct _tuple1 Cyc_Toc__untag_dynforward_ptr_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__untag_dynforward_ptr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_dynforward_ptr_re={1,& Cyc_Toc__untag_dynforward_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dynforward_ptr_ev={0,(
void*)((void*)& Cyc_Toc__untag_dynforward_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dynforward_ptr_e=& Cyc_Toc__untag_dynforward_ptr_ev;
static char _tmp51[21]="_untag_dyneither_ptr";static struct _dynforward_ptr Cyc_Toc__untag_dyneither_ptr_str={
_tmp51,_tmp51 + 21};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__untag_dyneither_ptr_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_dyneither_ptr_re={1,& Cyc_Toc__untag_dyneither_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(
void*)((void*)& Cyc_Toc__untag_dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;
static char _tmp54[21]="_get_dynforward_size";static struct _dynforward_ptr Cyc_Toc__get_dynforward_size_str={
_tmp54,_tmp54 + 21};static struct _tuple1 Cyc_Toc__get_dynforward_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_dynforward_size_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_dynforward_size_re={1,& Cyc_Toc__get_dynforward_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_dynforward_size_ev={0,(
void*)((void*)& Cyc_Toc__get_dynforward_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_dynforward_size_e=& Cyc_Toc__get_dynforward_size_ev;
static char _tmp57[20]="_get_dyneither_size";static struct _dynforward_ptr Cyc_Toc__get_dyneither_size_str={
_tmp57,_tmp57 + 20};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_dyneither_size_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__get_dyneither_size_re={1,& Cyc_Toc__get_dyneither_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(
void*)((void*)& Cyc_Toc__get_dyneither_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;
static char _tmp5A[19]="_get_zero_arr_size";static struct _dynforward_ptr Cyc_Toc__get_zero_arr_size_str={
_tmp5A,_tmp5A + 19};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_re={1,& Cyc_Toc__get_zero_arr_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(
void*)((void*)& Cyc_Toc__get_zero_arr_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;
static char _tmp5D[24]="_get_zero_arr_size_char";static struct _dynforward_ptr Cyc_Toc__get_zero_arr_size_char_str={
_tmp5D,_tmp5D + 24};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_char_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_char_re={1,& Cyc_Toc__get_zero_arr_size_char_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(
void*)((void*)& Cyc_Toc__get_zero_arr_size_char_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;
static char _tmp60[25]="_get_zero_arr_size_short";static struct _dynforward_ptr Cyc_Toc__get_zero_arr_size_short_str={
_tmp60,_tmp60 + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_short_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_short_re={1,& Cyc_Toc__get_zero_arr_size_short_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={
0,(void*)((void*)& Cyc_Toc__get_zero_arr_size_short_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;
static char _tmp63[23]="_get_zero_arr_size_int";static struct _dynforward_ptr Cyc_Toc__get_zero_arr_size_int_str={
_tmp63,_tmp63 + 23};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_int_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_int_re={1,& Cyc_Toc__get_zero_arr_size_int_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(
void*)((void*)& Cyc_Toc__get_zero_arr_size_int_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;
static char _tmp66[25]="_get_zero_arr_size_float";static struct _dynforward_ptr Cyc_Toc__get_zero_arr_size_float_str={
_tmp66,_tmp66 + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_float_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_float_re={1,& Cyc_Toc__get_zero_arr_size_float_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={
0,(void*)((void*)& Cyc_Toc__get_zero_arr_size_float_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;
static char _tmp69[26]="_get_zero_arr_size_double";static struct _dynforward_ptr Cyc_Toc__get_zero_arr_size_double_str={
_tmp69,_tmp69 + 26};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_double_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_double_re={1,& Cyc_Toc__get_zero_arr_size_double_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={
0,(void*)((void*)& Cyc_Toc__get_zero_arr_size_double_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;
static char _tmp6C[30]="_get_zero_arr_size_longdouble";static struct _dynforward_ptr
Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp6C,_tmp6C + 30};static struct
_tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_longdouble_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1,&
Cyc_Toc__get_zero_arr_size_longdouble_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)((void*)& Cyc_Toc__get_zero_arr_size_longdouble_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=&
Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp6F[28]="_get_zero_arr_size_voidstar";
static struct _dynforward_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp6F,
_tmp6F + 28};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_voidstar_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1,& Cyc_Toc__get_zero_arr_size_voidstar_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={
0,(void*)((void*)& Cyc_Toc__get_zero_arr_size_voidstar_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;
static char _tmp72[21]="_dynforward_ptr_plus";static struct _dynforward_ptr Cyc_Toc__dynforward_ptr_plus_str={
_tmp72,_tmp72 + 21};static struct _tuple1 Cyc_Toc__dynforward_ptr_plus_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_plus_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_plus_re={1,& Cyc_Toc__dynforward_ptr_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_plus_ev={0,(
void*)((void*)& Cyc_Toc__dynforward_ptr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_plus_e=& Cyc_Toc__dynforward_ptr_plus_ev;
static char _tmp75[20]="_dyneither_ptr_plus";static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_plus_str={
_tmp75,_tmp75 + 20};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_plus_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_plus_re={1,& Cyc_Toc__dyneither_ptr_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(
void*)((void*)& Cyc_Toc__dyneither_ptr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;
static char _tmp78[15]="_zero_arr_plus";static struct _dynforward_ptr Cyc_Toc__zero_arr_plus_str={
_tmp78,_tmp78 + 15};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_plus_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=&
Cyc_Toc__zero_arr_plus_ev;static char _tmp7B[20]="_zero_arr_plus_char";static
struct _dynforward_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp7B,_tmp7B + 20};static
struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_char_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_char_re={1,& Cyc_Toc__zero_arr_plus_char_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_char_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;
static char _tmp7E[21]="_zero_arr_plus_short";static struct _dynforward_ptr Cyc_Toc__zero_arr_plus_short_str={
_tmp7E,_tmp7E + 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_short_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_short_re={1,& Cyc_Toc__zero_arr_plus_short_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_short_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;
static char _tmp81[19]="_zero_arr_plus_int";static struct _dynforward_ptr Cyc_Toc__zero_arr_plus_int_str={
_tmp81,_tmp81 + 19};static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_int_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_int_re={1,& Cyc_Toc__zero_arr_plus_int_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_int_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;
static char _tmp84[21]="_zero_arr_plus_float";static struct _dynforward_ptr Cyc_Toc__zero_arr_plus_float_str={
_tmp84,_tmp84 + 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_float_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_float_re={1,& Cyc_Toc__zero_arr_plus_float_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_float_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;
static char _tmp87[22]="_zero_arr_plus_double";static struct _dynforward_ptr Cyc_Toc__zero_arr_plus_double_str={
_tmp87,_tmp87 + 22};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_double_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_double_re={1,& Cyc_Toc__zero_arr_plus_double_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_double_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;
static char _tmp8A[26]="_zero_arr_plus_longdouble";static struct _dynforward_ptr Cyc_Toc__zero_arr_plus_longdouble_str={
_tmp8A,_tmp8A + 26};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_longdouble_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_longdouble_re={1,& Cyc_Toc__zero_arr_plus_longdouble_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_plus_longdouble_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;
static char _tmp8D[24]="_zero_arr_plus_voidstar";static struct _dynforward_ptr Cyc_Toc__zero_arr_plus_voidstar_str={
_tmp8D,_tmp8D + 24};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_voidstar_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_voidstar_re={1,& Cyc_Toc__zero_arr_plus_voidstar_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_voidstar_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;
static char _tmp90[29]="_dynforward_ptr_inplace_plus";static struct _dynforward_ptr
Cyc_Toc__dynforward_ptr_inplace_plus_str={_tmp90,_tmp90 + 29};static struct _tuple1
Cyc_Toc__dynforward_ptr_inplace_plus_pr={(union Cyc_Absyn_Nmspace_union)((struct
Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_inplace_plus_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_inplace_plus_re={1,& Cyc_Toc__dynforward_ptr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_inplace_plus_ev={
0,(void*)((void*)& Cyc_Toc__dynforward_ptr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_inplace_plus_e=& Cyc_Toc__dynforward_ptr_inplace_plus_ev;
static char _tmp93[28]="_dyneither_ptr_inplace_plus";static struct _dynforward_ptr
Cyc_Toc__dyneither_ptr_inplace_plus_str={_tmp93,_tmp93 + 28};static struct _tuple1
Cyc_Toc__dyneither_ptr_inplace_plus_pr={(union Cyc_Absyn_Nmspace_union)((struct
Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_inplace_plus_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;
static char _tmp96[23]="_zero_arr_inplace_plus";static struct _dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_str={
_tmp96,_tmp96 + 23};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_re={1,& Cyc_Toc__zero_arr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;
static char _tmp99[28]="_zero_arr_inplace_plus_char";static struct _dynforward_ptr
Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp99,_tmp99 + 28};static struct _tuple1
Cyc_Toc__zero_arr_inplace_plus_char_pr={(union Cyc_Absyn_Nmspace_union)((struct
Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_char_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_char_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_char_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;
static char _tmp9C[29]="_zero_arr_inplace_plus_short";static struct _dynforward_ptr
Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp9C,_tmp9C + 29};static struct _tuple1
Cyc_Toc__zero_arr_inplace_plus_short_pr={(union Cyc_Absyn_Nmspace_union)((struct
Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_short_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1,& Cyc_Toc__zero_arr_inplace_plus_short_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_short_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=& Cyc_Toc__zero_arr_inplace_plus_short_ev;
static char _tmp9F[27]="_zero_arr_inplace_plus_int";static struct _dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={
_tmp9F,_tmp9F + 27};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_int_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_int_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_int_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;
static char _tmpA2[29]="_zero_arr_inplace_plus_float";static struct _dynforward_ptr
Cyc_Toc__zero_arr_inplace_plus_float_str={_tmpA2,_tmpA2 + 29};static struct _tuple1
Cyc_Toc__zero_arr_inplace_plus_float_pr={(union Cyc_Absyn_Nmspace_union)((struct
Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_float_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1,& Cyc_Toc__zero_arr_inplace_plus_float_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_float_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=& Cyc_Toc__zero_arr_inplace_plus_float_ev;
static char _tmpA5[30]="_zero_arr_inplace_plus_double";static struct _dynforward_ptr
Cyc_Toc__zero_arr_inplace_plus_double_str={_tmpA5,_tmpA5 + 30};static struct
_tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_double_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1,&
Cyc_Toc__zero_arr_inplace_plus_double_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_double_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=&
Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmpA8[34]="_zero_arr_inplace_plus_longdouble";
static struct _dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmpA8,
_tmpA8 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={
1,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=&
Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmpAB[32]="_zero_arr_inplace_plus_voidstar";
static struct _dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmpAB,
_tmpAB + 32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1,&
Cyc_Toc__zero_arr_inplace_plus_voidstar_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=&
Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmpAE[34]="_dynforward_ptr_inplace_plus_post";
static struct _dynforward_ptr Cyc_Toc__dynforward_ptr_inplace_plus_post_str={_tmpAE,
_tmpAE + 34};static struct _tuple1 Cyc_Toc__dynforward_ptr_inplace_plus_post_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_inplace_plus_post_re={
1,& Cyc_Toc__dynforward_ptr_inplace_plus_post_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__dynforward_ptr_inplace_plus_post_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_inplace_plus_post_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_inplace_plus_post_e=&
Cyc_Toc__dynforward_ptr_inplace_plus_post_ev;static char _tmpB1[33]="_dyneither_ptr_inplace_plus_post";
static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmpB1,
_tmpB1 + 33};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,&
Cyc_Toc__dyneither_ptr_inplace_plus_post_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)((void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=&
Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmpB4[25]="_dynforward_to_dyneither";
static struct _dynforward_ptr Cyc_Toc__dynforward_to_dyneither_str={_tmpB4,_tmpB4 + 
25};static struct _tuple1 Cyc_Toc__dynforward_to_dyneither_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_to_dyneither_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_to_dyneither_re={1,& Cyc_Toc__dynforward_to_dyneither_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dynforward_to_dyneither_ev={
0,(void*)((void*)& Cyc_Toc__dynforward_to_dyneither_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_to_dyneither_e=& Cyc_Toc__dynforward_to_dyneither_ev;
static char _tmpB7[25]="_dyneither_to_dynforward";static struct _dynforward_ptr Cyc_Toc__dyneither_to_dynforward_str={
_tmpB7,_tmpB7 + 25};static struct _tuple1 Cyc_Toc__dyneither_to_dynforward_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_to_dynforward_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_to_dynforward_re={1,& Cyc_Toc__dyneither_to_dynforward_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_to_dynforward_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_to_dynforward_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_to_dynforward_e=& Cyc_Toc__dyneither_to_dynforward_ev;
static char _tmpBA[28]="_zero_arr_inplace_plus_post";static struct _dynforward_ptr
Cyc_Toc__zero_arr_inplace_plus_post_str={_tmpBA,_tmpBA + 28};static struct _tuple1
Cyc_Toc__zero_arr_inplace_plus_post_pr={(union Cyc_Absyn_Nmspace_union)((struct
Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_str};static
struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;
static char _tmpBD[33]="_zero_arr_inplace_plus_post_char";static struct
_dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmpBD,_tmpBD + 33};
static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_char_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,&
Cyc_Toc__zero_arr_inplace_plus_post_char_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=&
Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmpC0[34]="_zero_arr_inplace_plus_post_short";
static struct _dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmpC0,
_tmpC0 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_short_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={
1,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=&
Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmpC3[32]="_zero_arr_inplace_plus_post_int";
static struct _dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmpC3,
_tmpC3 + 32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_int_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1,&
Cyc_Toc__zero_arr_inplace_plus_post_int_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=&
Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmpC6[34]="_zero_arr_inplace_plus_post_float";
static struct _dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmpC6,
_tmpC6 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_float_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={
1,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=&
Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmpC9[35]="_zero_arr_inplace_plus_post_double";
static struct _dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={
_tmpC9,_tmpC9 + 35};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_double_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={
1,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)((void*)&
Cyc_Toc__zero_arr_inplace_plus_post_double_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;
static char _tmpCC[39]="_zero_arr_inplace_plus_post_longdouble";static struct
_dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmpCC,_tmpCC
+ 39};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={
1,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)((
void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;
static char _tmpCF[37]="_zero_arr_inplace_plus_post_voidstar";static struct
_dynforward_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmpCF,_tmpCF + 
37};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={
1,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)((
void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;
static char _tmpD2[10]="_cycalloc";static struct _dynforward_ptr Cyc_Toc__cycalloc_str={
_tmpD2,_tmpD2 + 10};static struct _tuple1 Cyc_Toc__cycalloc_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cycalloc_re={1,& Cyc_Toc__cycalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__cycalloc_ev={0,(void*)((void*)& Cyc_Toc__cycalloc_re),0,(void*)((void*)
Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;
static char _tmpD5[11]="_cyccalloc";static struct _dynforward_ptr Cyc_Toc__cyccalloc_str={
_tmpD5,_tmpD5 + 11};static struct _tuple1 Cyc_Toc__cyccalloc_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cyccalloc_re={1,& Cyc_Toc__cyccalloc_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)((void*)& Cyc_Toc__cyccalloc_re),0,(
void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=&
Cyc_Toc__cyccalloc_ev;static char _tmpD8[17]="_cycalloc_atomic";static struct
_dynforward_ptr Cyc_Toc__cycalloc_atomic_str={_tmpD8,_tmpD8 + 17};static struct
_tuple1 Cyc_Toc__cycalloc_atomic_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__cycalloc_atomic_re={
1,& Cyc_Toc__cycalloc_atomic_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={
0,(void*)((void*)& Cyc_Toc__cycalloc_atomic_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;
static char _tmpDB[18]="_cyccalloc_atomic";static struct _dynforward_ptr Cyc_Toc__cyccalloc_atomic_str={
_tmpDB,_tmpDB + 18};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cyccalloc_atomic_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_atomic_re={1,& Cyc_Toc__cyccalloc_atomic_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)((
void*)& Cyc_Toc__cyccalloc_atomic_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;
static char _tmpDE[15]="_region_malloc";static struct _dynforward_ptr Cyc_Toc__region_malloc_str={
_tmpDE,_tmpDE + 15};static struct _tuple1 Cyc_Toc__region_malloc_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)((void*)& Cyc_Toc__region_malloc_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=&
Cyc_Toc__region_malloc_ev;static char _tmpE1[15]="_region_calloc";static struct
_dynforward_ptr Cyc_Toc__region_calloc_str={_tmpE1,_tmpE1 + 15};static struct
_tuple1 Cyc_Toc__region_calloc_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_calloc_re={
1,& Cyc_Toc__region_calloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={
0,(void*)((void*)& Cyc_Toc__region_calloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;
static char _tmpE4[13]="_check_times";static struct _dynforward_ptr Cyc_Toc__check_times_str={
_tmpE4,_tmpE4 + 13};static struct _tuple1 Cyc_Toc__check_times_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)((void*)& Cyc_Toc__check_times_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=&
Cyc_Toc__check_times_ev;static char _tmpE7[12]="_new_region";static struct
_dynforward_ptr Cyc_Toc__new_region_str={_tmpE7,_tmpE7 + 12};static struct _tuple1
Cyc_Toc__new_region_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={
1,& Cyc_Toc__new_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={
0,(void*)((void*)& Cyc_Toc__new_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static
char _tmpEA[13]="_push_region";static struct _dynforward_ptr Cyc_Toc__push_region_str={
_tmpEA,_tmpEA + 13};static struct _tuple1 Cyc_Toc__push_region_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)((void*)& Cyc_Toc__push_region_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=&
Cyc_Toc__push_region_ev;static char _tmpED[12]="_pop_region";static struct
_dynforward_ptr Cyc_Toc__pop_region_str={_tmpED,_tmpED + 12};static struct _tuple1
Cyc_Toc__pop_region_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={
1,& Cyc_Toc__pop_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={
0,(void*)((void*)& Cyc_Toc__pop_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static
char _tmpF0[16]="_open_dynregion";static struct _dynforward_ptr Cyc_Toc__open_dynregion_str={
_tmpF0,_tmpF0 + 16};static struct _tuple1 Cyc_Toc__open_dynregion_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__open_dynregion_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__open_dynregion_re={1,& Cyc_Toc__open_dynregion_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={0,(void*)((void*)& Cyc_Toc__open_dynregion_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__open_dynregion_e=&
Cyc_Toc__open_dynregion_ev;static char _tmpF3[16]="_push_dynregion";static struct
_dynforward_ptr Cyc_Toc__push_dynregion_str={_tmpF3,_tmpF3 + 16};static struct
_tuple1 Cyc_Toc__push_dynregion_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__push_dynregion_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_dynregion_re={
1,& Cyc_Toc__push_dynregion_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={
0,(void*)((void*)& Cyc_Toc__push_dynregion_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__push_dynregion_e=& Cyc_Toc__push_dynregion_ev;
static char _tmpF6[15]="_pop_dynregion";static struct _dynforward_ptr Cyc_Toc__pop_dynregion_str={
_tmpF6,_tmpF6 + 15};static struct _tuple1 Cyc_Toc__pop_dynregion_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__pop_dynregion_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__pop_dynregion_re={1,& Cyc_Toc__pop_dynregion_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={0,(void*)((void*)& Cyc_Toc__pop_dynregion_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_dynregion_e=&
Cyc_Toc__pop_dynregion_ev;static char _tmpF9[14]="_reset_region";static struct
_dynforward_ptr Cyc_Toc__reset_region_str={_tmpF9,_tmpF9 + 14};static struct _tuple1
Cyc_Toc__reset_region_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__reset_region_re={
1,& Cyc_Toc__reset_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={
0,(void*)((void*)& Cyc_Toc__reset_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=& Cyc_Toc__reset_region_ev;
static char _tmpFC[19]="_throw_arraybounds";static struct _dynforward_ptr Cyc_Toc__throw_arraybounds_str={
_tmpFC,_tmpFC + 19};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__throw_arraybounds_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__throw_arraybounds_re={1,& Cyc_Toc__throw_arraybounds_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(
void*)((void*)& Cyc_Toc__throw_arraybounds_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;
static char _tmpFF[30]="_dynforward_ptr_decrease_size";static struct _dynforward_ptr
Cyc_Toc__dynforward_ptr_decrease_size_str={_tmpFF,_tmpFF + 30};static struct
_tuple1 Cyc_Toc__dynforward_ptr_decrease_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dynforward_ptr_decrease_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dynforward_ptr_decrease_size_re={1,&
Cyc_Toc__dynforward_ptr_decrease_size_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dynforward_ptr_decrease_size_ev={0,(void*)((void*)& Cyc_Toc__dynforward_ptr_decrease_size_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dynforward_ptr_decrease_size_e=&
Cyc_Toc__dynforward_ptr_decrease_size_ev;static char _tmp102[29]="_dyneither_ptr_decrease_size";
static struct _dynforward_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmp102,
_tmp102 + 29};static struct _tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_decrease_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,&
Cyc_Toc__dyneither_ptr_decrease_size_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)((void*)& Cyc_Toc__dyneither_ptr_decrease_size_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=&
Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmp105[11]="_swap_word";
static struct _dynforward_ptr Cyc_Toc__swap_word_str={_tmp105,_tmp105 + 11};static
struct _tuple1 Cyc_Toc__swap_word_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__swap_word_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_word_re={
1,& Cyc_Toc__swap_word_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={
0,(void*)((void*)& Cyc_Toc__swap_word_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char
_tmp108[17]="_swap_dynforward";static struct _dynforward_ptr Cyc_Toc__swap_dynforward_str={
_tmp108,_tmp108 + 17};static struct _tuple1 Cyc_Toc__swap_dynforward_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__swap_dynforward_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__swap_dynforward_re={1,& Cyc_Toc__swap_dynforward_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__swap_dynforward_ev={0,(void*)((void*)& Cyc_Toc__swap_dynforward_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_dynforward_e=&
Cyc_Toc__swap_dynforward_ev;static char _tmp10B[16]="_swap_dyneither";static struct
_dynforward_ptr Cyc_Toc__swap_dyneither_str={_tmp10B,_tmp10B + 16};static struct
_tuple1 Cyc_Toc__swap_dyneither_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__swap_dyneither_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_dyneither_re={
1,& Cyc_Toc__swap_dyneither_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_dyneither_ev={
0,(void*)((void*)& Cyc_Toc__swap_dyneither_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__swap_dyneither_e=& Cyc_Toc__swap_dyneither_ev;
static struct Cyc_Absyn_AggrType_struct Cyc_Toc_dynforward_ptr_typ_v={10,{(union Cyc_Absyn_AggrInfoU_union)((
struct Cyc_Absyn_UnknownAggr_struct){0,(void*)((void*)0),& Cyc_Toc__dynforward_ptr_pr}),
0}};static void*Cyc_Toc_dynforward_ptr_typ=(void*)& Cyc_Toc_dynforward_ptr_typ_v;
static struct Cyc_Absyn_AggrType_struct Cyc_Toc_dyneither_ptr_typ_v={10,{(union Cyc_Absyn_AggrInfoU_union)((
struct Cyc_Absyn_UnknownAggr_struct){0,(void*)((void*)0),& Cyc_Toc__dyneither_ptr_pr}),
0}};static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0};static struct Cyc_Absyn_Stmt*
Cyc_Toc_skip_stmt_dl();static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){static
struct Cyc_Absyn_Stmt**skip_stmt_opt=0;if(skip_stmt_opt == 0){struct Cyc_Absyn_Stmt**
_tmp87E;skip_stmt_opt=((_tmp87E=_cycalloc(sizeof(*_tmp87E)),((_tmp87E[0]=Cyc_Absyn_skip_stmt(
0),_tmp87E))));}return*skip_stmt_opt;}static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(
void*t,struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,
struct Cyc_Absyn_Exp*e){return Cyc_Absyn_cast_exp(t,e,0,(void*)1,0);}static void*
Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_cast_it_r(
void*t,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Cast_e_struct _tmp881;struct Cyc_Absyn_Cast_e_struct*
_tmp880;return(void*)((_tmp880=_cycalloc(sizeof(*_tmp880)),((_tmp880[0]=((
_tmp881.tag=15,((_tmp881.f1=(void*)t,((_tmp881.f2=e,((_tmp881.f3=0,((_tmp881.f4=(
void*)((void*)1),_tmp881)))))))))),_tmp880))));}static void*Cyc_Toc_deref_exp_r(
struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Deref_e_struct _tmp884;struct Cyc_Absyn_Deref_e_struct*_tmp883;
return(void*)((_tmp883=_cycalloc(sizeof(*_tmp883)),((_tmp883[0]=((_tmp884.tag=22,((
_tmp884.f1=e,_tmp884)))),_tmp883))));}static void*Cyc_Toc_subscript_exp_r(struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);static void*Cyc_Toc_subscript_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){struct Cyc_Absyn_Subscript_e_struct
_tmp887;struct Cyc_Absyn_Subscript_e_struct*_tmp886;return(void*)((_tmp886=
_cycalloc(sizeof(*_tmp886)),((_tmp886[0]=((_tmp887.tag=25,((_tmp887.f1=e1,((
_tmp887.f2=e2,_tmp887)))))),_tmp886))));}static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*
s);static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_StmtExp_e_struct
_tmp88A;struct Cyc_Absyn_StmtExp_e_struct*_tmp889;return(void*)((_tmp889=
_cycalloc(sizeof(*_tmp889)),((_tmp889[0]=((_tmp88A.tag=38,((_tmp88A.f1=s,_tmp88A)))),
_tmp889))));}static void*Cyc_Toc_sizeoftyp_exp_r(void*t);static void*Cyc_Toc_sizeoftyp_exp_r(
void*t){struct Cyc_Absyn_Sizeoftyp_e_struct _tmp88D;struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmp88C;return(void*)((_tmp88C=_cycalloc(sizeof(*_tmp88C)),((_tmp88C[0]=((
_tmp88D.tag=18,((_tmp88D.f1=(void*)t,_tmp88D)))),_tmp88C))));}static void*Cyc_Toc_fncall_exp_r(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es);static void*Cyc_Toc_fncall_exp_r(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){struct Cyc_Absyn_FnCall_e_struct
_tmp890;struct Cyc_Absyn_FnCall_e_struct*_tmp88F;return(void*)((_tmp88F=_cycalloc(
sizeof(*_tmp88F)),((_tmp88F[0]=((_tmp890.tag=11,((_tmp890.f1=e,((_tmp890.f2=es,((
_tmp890.f3=0,_tmp890)))))))),_tmp88F))));}static void*Cyc_Toc_exp_stmt_r(struct
Cyc_Absyn_Exp*e);static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_s_struct
_tmp893;struct Cyc_Absyn_Exp_s_struct*_tmp892;return(void*)((_tmp892=_cycalloc(
sizeof(*_tmp892)),((_tmp892[0]=((_tmp893.tag=0,((_tmp893.f1=e,_tmp893)))),
_tmp892))));}static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2);static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*
s2){struct Cyc_Absyn_Seq_s_struct _tmp896;struct Cyc_Absyn_Seq_s_struct*_tmp895;
return(void*)((_tmp895=_cycalloc(sizeof(*_tmp895)),((_tmp895[0]=((_tmp896.tag=1,((
_tmp896.f1=s1,((_tmp896.f2=s2,_tmp896)))))),_tmp895))));}static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3);static void*
Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct
Cyc_Absyn_Exp*e3){struct Cyc_Absyn_Conditional_e_struct _tmp899;struct Cyc_Absyn_Conditional_e_struct*
_tmp898;return(void*)((_tmp898=_cycalloc(sizeof(*_tmp898)),((_tmp898[0]=((
_tmp899.tag=6,((_tmp899.f1=e1,((_tmp899.f2=e2,((_tmp899.f3=e3,_tmp899)))))))),
_tmp898))));}static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct
_dynforward_ptr*n);static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,
struct _dynforward_ptr*n){struct Cyc_Absyn_AggrMember_e_struct _tmp89C;struct Cyc_Absyn_AggrMember_e_struct*
_tmp89B;return(void*)((_tmp89B=_cycalloc(sizeof(*_tmp89B)),((_tmp89B[0]=((
_tmp89C.tag=23,((_tmp89C.f1=e,((_tmp89C.f2=n,_tmp89C)))))),_tmp89B))));}static
void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dynforward_ptr*n);
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dynforward_ptr*n){
struct Cyc_Absyn_AggrArrow_e_struct _tmp89F;struct Cyc_Absyn_AggrArrow_e_struct*
_tmp89E;return(void*)((_tmp89E=_cycalloc(sizeof(*_tmp89E)),((_tmp89E[0]=((
_tmp89F.tag=24,((_tmp89F.f1=e,((_tmp89F.f2=n,_tmp89F)))))),_tmp89E))));}static
void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds);
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*
ds){struct Cyc_Absyn_UnresolvedMem_e_struct _tmp8A2;struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp8A1;return(void*)((_tmp8A1=_cycalloc(sizeof(*_tmp8A1)),((_tmp8A1[0]=((
_tmp8A2.tag=37,((_tmp8A2.f1=tdopt,((_tmp8A2.f2=ds,_tmp8A2)))))),_tmp8A1))));}
static void*Cyc_Toc_goto_stmt_r(struct _dynforward_ptr*v,struct Cyc_Absyn_Stmt*s);
static void*Cyc_Toc_goto_stmt_r(struct _dynforward_ptr*v,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Goto_s_struct _tmp8A5;struct Cyc_Absyn_Goto_s_struct*_tmp8A4;
return(void*)((_tmp8A4=_cycalloc(sizeof(*_tmp8A4)),((_tmp8A4[0]=((_tmp8A5.tag=7,((
_tmp8A5.f1=v,((_tmp8A5.f2=s,_tmp8A5)))))),_tmp8A4))));}static struct Cyc_Absyn_Const_e_struct
Cyc_Toc_zero_exp={0,(union Cyc_Absyn_Cnst_union)((struct Cyc_Absyn_Int_c_struct){2,(
void*)((void*)0),0})};struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct
Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct
Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*
fvoidstar;};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,&
Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,&
Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,&
Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,&
Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,&
Cyc_Toc__get_zero_arr_size_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={&
Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,&
Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,&
Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,&
Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={&
Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,&
Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(
struct Cyc_Toc_functionSet*fS,void*t);static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(
struct Cyc_Toc_functionSet*fS,void*t){struct Cyc_Absyn_Exp*function;{void*_tmp12F=
t;void*_tmp130;int _tmp131;_LL1: if(_tmp12F <= (void*)4)goto _LL3;if(*((int*)_tmp12F)
!= 5)goto _LL3;_tmp130=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp12F)->f2;_LL2:{
void*_tmp132=_tmp130;_LLC: if((int)_tmp132 != 0)goto _LLE;_LLD: function=fS->fchar;
goto _LLB;_LLE: if((int)_tmp132 != 1)goto _LL10;_LLF: function=fS->fshort;goto _LLB;
_LL10: if((int)_tmp132 != 2)goto _LL12;_LL11: function=fS->fint;goto _LLB;_LL12:;
_LL13: {struct Cyc_Core_Impossible_struct _tmp8AB;const char*_tmp8AA;struct Cyc_Core_Impossible_struct*
_tmp8A9;(int)_throw((void*)((_tmp8A9=_cycalloc(sizeof(*_tmp8A9)),((_tmp8A9[0]=((
_tmp8AB.tag=Cyc_Core_Impossible,((_tmp8AB.f1=((_tmp8AA="impossible IntType (not char, short or int)",
_tag_dynforward(_tmp8AA,sizeof(char),_get_zero_arr_size_char(_tmp8AA,44)))),
_tmp8AB)))),_tmp8A9)))));}_LLB:;}goto _LL0;_LL3: if((int)_tmp12F != 1)goto _LL5;_LL4:
function=fS->ffloat;goto _LL0;_LL5: if(_tmp12F <= (void*)4)goto _LL9;if(*((int*)
_tmp12F)!= 6)goto _LL7;_tmp131=((struct Cyc_Absyn_DoubleType_struct*)_tmp12F)->f1;
_LL6: switch(_tmp131){case 1: _LL14: function=fS->flongdouble;break;default: _LL15:
function=fS->fdouble;}goto _LL0;_LL7: if(*((int*)_tmp12F)!= 4)goto _LL9;_LL8:
function=fS->fvoidstar;goto _LL0;_LL9:;_LLA: {struct Cyc_Core_Impossible_struct
_tmp8B1;const char*_tmp8B0;struct Cyc_Core_Impossible_struct*_tmp8AF;(int)_throw((
void*)((_tmp8AF=_cycalloc(sizeof(*_tmp8AF)),((_tmp8AF[0]=((_tmp8B1.tag=Cyc_Core_Impossible,((
_tmp8B1.f1=((_tmp8B0="impossible expression type (not int, float, double, or pointer)",
_tag_dynforward(_tmp8B0,sizeof(char),_get_zero_arr_size_char(_tmp8B0,64)))),
_tmp8B1)))),_tmp8AF)))));}_LL0:;}return function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(
struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr);struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(
struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){return Cyc_Toc_getFunctionType(
fS,Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(arr->topt))->v));}
struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*
fS,struct Cyc_Absyn_Exp*arr);struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(
struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){void*_tmp139=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(arr->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp13A;void*_tmp13B;_LL18: if(_tmp139 <= (void*)4)goto _LL1A;if(*((int*)_tmp139)!= 
4)goto _LL1A;_tmp13A=((struct Cyc_Absyn_PointerType_struct*)_tmp139)->f1;_tmp13B=(
void*)_tmp13A.elt_typ;_LL19: return Cyc_Toc_getFunctionType(fS,_tmp13B);_LL1A:;
_LL1B: {struct Cyc_Core_Impossible_struct _tmp8B7;const char*_tmp8B6;struct Cyc_Core_Impossible_struct*
_tmp8B5;(int)_throw((void*)((_tmp8B5=_cycalloc(sizeof(*_tmp8B5)),((_tmp8B5[0]=((
_tmp8B7.tag=Cyc_Core_Impossible,((_tmp8B7.f1=((_tmp8B6="impossible type (not pointer)",
_tag_dynforward(_tmp8B6,sizeof(char),_get_zero_arr_size_char(_tmp8B6,30)))),
_tmp8B7)))),_tmp8B5)))));}_LL17:;}struct _tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e);static int Cyc_Toc_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp13F=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp140;
char _tmp141;union Cyc_Absyn_Cnst_union _tmp142;short _tmp143;union Cyc_Absyn_Cnst_union
_tmp144;int _tmp145;union Cyc_Absyn_Cnst_union _tmp146;long long _tmp147;union Cyc_Absyn_Cnst_union
_tmp148;struct Cyc_Absyn_Exp*_tmp149;struct Cyc_List_List*_tmp14A;struct Cyc_List_List*
_tmp14B;struct Cyc_List_List*_tmp14C;struct Cyc_List_List*_tmp14D;struct Cyc_List_List*
_tmp14E;_LL1D: if(*((int*)_tmp13F)!= 0)goto _LL1F;_tmp140=((struct Cyc_Absyn_Const_e_struct*)
_tmp13F)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp13F)->f1).Char_c).tag != 
0)goto _LL1F;_tmp141=(_tmp140.Char_c).f2;_LL1E: return _tmp141 == '\000';_LL1F: if(*((
int*)_tmp13F)!= 0)goto _LL21;_tmp142=((struct Cyc_Absyn_Const_e_struct*)_tmp13F)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp13F)->f1).Short_c).tag != 1)goto _LL21;
_tmp143=(_tmp142.Short_c).f2;_LL20: return _tmp143 == 0;_LL21: if(*((int*)_tmp13F)!= 
0)goto _LL23;_tmp144=((struct Cyc_Absyn_Const_e_struct*)_tmp13F)->f1;if(((((struct
Cyc_Absyn_Const_e_struct*)_tmp13F)->f1).Int_c).tag != 2)goto _LL23;_tmp145=(
_tmp144.Int_c).f2;_LL22: return _tmp145 == 0;_LL23: if(*((int*)_tmp13F)!= 0)goto
_LL25;_tmp146=((struct Cyc_Absyn_Const_e_struct*)_tmp13F)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp13F)->f1).LongLong_c).tag != 3)goto _LL25;_tmp147=(_tmp146.LongLong_c).f2;
_LL24: return _tmp147 == 0;_LL25: if(*((int*)_tmp13F)!= 0)goto _LL27;_tmp148=((struct
Cyc_Absyn_Const_e_struct*)_tmp13F)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp13F)->f1).Null_c).tag != 6)goto _LL27;_LL26: return 1;_LL27: if(*((int*)_tmp13F)
!= 15)goto _LL29;_tmp149=((struct Cyc_Absyn_Cast_e_struct*)_tmp13F)->f2;_LL28:
return Cyc_Toc_is_zero(_tmp149);_LL29: if(*((int*)_tmp13F)!= 26)goto _LL2B;_tmp14A=((
struct Cyc_Absyn_Tuple_e_struct*)_tmp13F)->f1;_LL2A: return((int(*)(int(*pred)(
struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,
_tmp14A);_LL2B: if(*((int*)_tmp13F)!= 28)goto _LL2D;_tmp14B=((struct Cyc_Absyn_Array_e_struct*)
_tmp13F)->f1;_LL2C: _tmp14C=_tmp14B;goto _LL2E;_LL2D: if(*((int*)_tmp13F)!= 30)goto
_LL2F;_tmp14C=((struct Cyc_Absyn_Struct_e_struct*)_tmp13F)->f3;_LL2E: _tmp14D=
_tmp14C;goto _LL30;_LL2F: if(*((int*)_tmp13F)!= 27)goto _LL31;_tmp14D=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp13F)->f2;_LL30: _tmp14E=_tmp14D;goto _LL32;_LL31: if(*((int*)_tmp13F)!= 37)goto
_LL33;_tmp14E=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp13F)->f2;_LL32: for(0;
_tmp14E != 0;_tmp14E=_tmp14E->tl){if(!Cyc_Toc_is_zero((*((struct _tuple8*)_tmp14E->hd)).f2))
return 0;}return 1;_LL33:;_LL34: return 0;_LL1C:;}static int Cyc_Toc_is_nullable(void*
t);static int Cyc_Toc_is_nullable(void*t){void*_tmp14F=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp150;struct Cyc_Absyn_PtrAtts _tmp151;struct Cyc_Absyn_Conref*
_tmp152;_LL36: if(_tmp14F <= (void*)4)goto _LL38;if(*((int*)_tmp14F)!= 4)goto _LL38;
_tmp150=((struct Cyc_Absyn_PointerType_struct*)_tmp14F)->f1;_tmp151=_tmp150.ptr_atts;
_tmp152=_tmp151.nullable;_LL37: return((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp152);_LL38:;_LL39: {const char*_tmp8BA;void*_tmp8B9;(_tmp8B9=0,((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmp8BA="is_nullable",
_tag_dynforward(_tmp8BA,sizeof(char),_get_zero_arr_size_char(_tmp8BA,12)))),
_tag_dynforward(_tmp8B9,sizeof(void*),0)));}_LL35:;}static char _tmp161[1]="";
static char _tmp163[8]="*bogus*";static struct _tuple1*Cyc_Toc_collapse_qvar_tag(
struct _tuple1*x,struct _dynforward_ptr tag);static struct _tuple1*Cyc_Toc_collapse_qvar_tag(
struct _tuple1*x,struct _dynforward_ptr tag){struct _DynRegionHandle*_tmp156;struct
Cyc_Dict_Dict*_tmp157;struct Cyc_Toc_TocState _tmp155=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmp156=_tmp155.dyn;_tmp157=_tmp155.qvar_tags;{
static struct _dynforward_ptr bogus_string={_tmp163,_tmp163 + 8};static struct _tuple1
bogus_qvar={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),&
bogus_string};static struct _tuple6 pair={& bogus_qvar,{_tmp161,_tmp161 + 1}};{struct
_tuple6 _tmp8BB;pair=((_tmp8BB.f1=x,((_tmp8BB.f2=tag,_tmp8BB))));}{struct
_DynRegionFrame _tmp159;struct _RegionHandle*d=_open_dynregion(& _tmp159,_tmp156);{
struct _tuple1**_tmp15A=((struct _tuple1**(*)(struct Cyc_Dict_Dict d,struct _tuple6*k))
Cyc_Dict_lookup_opt)(*_tmp157,(struct _tuple6*)& pair);if(_tmp15A != 0){struct
_tuple1*_tmp15B=*_tmp15A;_npop_handler(0);return _tmp15B;}{struct _tuple6*_tmp8BC;
struct _tuple6*_tmp15C=(_tmp8BC=_cycalloc(sizeof(*_tmp8BC)),((_tmp8BC->f1=x,((
_tmp8BC->f2=tag,_tmp8BC)))));struct _dynforward_ptr*_tmp8BF;struct _tuple1*_tmp8BE;
struct _tuple1*res=(_tmp8BE=_cycalloc(sizeof(*_tmp8BE)),((_tmp8BE->f1=(*x).f1,((
_tmp8BE->f2=((_tmp8BF=_cycalloc(sizeof(*_tmp8BF)),((_tmp8BF[0]=(struct
_dynforward_ptr)Cyc_strconcat((struct _dynforward_ptr)*(*x).f2,(struct
_dynforward_ptr)tag),_tmp8BF)))),_tmp8BE)))));*_tmp157=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple6*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmp157,(
struct _tuple6*)_tmp15C,res);{struct _tuple1*_tmp15D=res;_npop_handler(0);return
_tmp15D;}}};_pop_dynregion(d);}}}struct _tuple9{void*f1;struct Cyc_List_List*f2;};
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0);static void*Cyc_Toc_add_tuple_type(
struct Cyc_List_List*tqs0){struct _DynRegionHandle*_tmp165;struct Cyc_List_List**
_tmp166;struct Cyc_Toc_TocState _tmp164=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp165=_tmp164.dyn;_tmp166=_tmp164.tuple_types;{struct _DynRegionFrame _tmp167;
struct _RegionHandle*d=_open_dynregion(& _tmp167,_tmp165);{struct Cyc_List_List*
_tmp168=*_tmp166;for(0;_tmp168 != 0;_tmp168=_tmp168->tl){struct _tuple9 _tmp16A;
void*_tmp16B;struct Cyc_List_List*_tmp16C;struct _tuple9*_tmp169=(struct _tuple9*)
_tmp168->hd;_tmp16A=*_tmp169;_tmp16B=_tmp16A.f1;_tmp16C=_tmp16A.f2;{struct Cyc_List_List*
_tmp16D=tqs0;for(0;_tmp16D != 0  && _tmp16C != 0;(_tmp16D=_tmp16D->tl,_tmp16C=
_tmp16C->tl)){if(!Cyc_Tcutil_unify((*((struct _tuple4*)_tmp16D->hd)).f2,(void*)
_tmp16C->hd))break;}if(_tmp16D == 0  && _tmp16C == 0){void*_tmp16E=_tmp16B;
_npop_handler(0);return _tmp16E;}}}}{struct Cyc_Int_pa_struct _tmp8C7;void*_tmp8C6[
1];const char*_tmp8C5;struct _dynforward_ptr*_tmp8C4;struct _dynforward_ptr*xname=(
_tmp8C4=_cycalloc(sizeof(*_tmp8C4)),((_tmp8C4[0]=(struct _dynforward_ptr)((
_tmp8C7.tag=1,((_tmp8C7.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmp8C6[
0]=& _tmp8C7,Cyc_aprintf(((_tmp8C5="_tuple%d",_tag_dynforward(_tmp8C5,sizeof(char),
_get_zero_arr_size_char(_tmp8C5,9)))),_tag_dynforward(_tmp8C6,sizeof(void*),1)))))))),
_tmp8C4)));void*x=Cyc_Absyn_strct(xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(
struct _RegionHandle*,void*(*f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_rmap)(
d,Cyc_Tcutil_snd_tqt,tqs0);struct Cyc_List_List*_tmp16F=0;struct Cyc_List_List*ts2=
ts;{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){struct Cyc_Absyn_Aggrfield*_tmp8CA;
struct Cyc_List_List*_tmp8C9;_tmp16F=((_tmp8C9=_cycalloc(sizeof(*_tmp8C9)),((
_tmp8C9->hd=((_tmp8CA=_cycalloc(sizeof(*_tmp8CA)),((_tmp8CA->name=Cyc_Absyn_fieldname(
i),((_tmp8CA->tq=Cyc_Toc_mt_tq,((_tmp8CA->type=(void*)((void*)ts2->hd),((_tmp8CA->width=
0,((_tmp8CA->attributes=0,_tmp8CA)))))))))))),((_tmp8C9->tl=_tmp16F,_tmp8C9))))));}}
_tmp16F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp16F);{struct Cyc_Absyn_AggrdeclImpl*_tmp8D2;struct _tuple1*_tmp8D1;union Cyc_Absyn_Nmspace_union
_tmp8D0;struct Cyc_Absyn_Aggrdecl*_tmp8CF;struct Cyc_Absyn_Aggrdecl*_tmp172=(
_tmp8CF=_cycalloc(sizeof(*_tmp8CF)),((_tmp8CF->kind=(void*)((void*)0),((_tmp8CF->sc=(
void*)((void*)2),((_tmp8CF->name=((_tmp8D1=_cycalloc(sizeof(*_tmp8D1)),((_tmp8D1->f1=(
union Cyc_Absyn_Nmspace_union)(((_tmp8D0.Rel_n).tag=1,(((_tmp8D0.Rel_n).f1=0,
_tmp8D0)))),((_tmp8D1->f2=xname,_tmp8D1)))))),((_tmp8CF->tvs=0,((_tmp8CF->impl=((
_tmp8D2=_cycalloc(sizeof(*_tmp8D2)),((_tmp8D2->exist_vars=0,((_tmp8D2->rgn_po=0,((
_tmp8D2->fields=_tmp16F,_tmp8D2)))))))),((_tmp8CF->attributes=0,_tmp8CF)))))))))))));{
struct Cyc_Absyn_Aggr_d_struct*_tmp8D8;struct Cyc_Absyn_Aggr_d_struct _tmp8D7;
struct Cyc_List_List*_tmp8D6;Cyc_Toc_result_decls=((_tmp8D6=_cycalloc(sizeof(*
_tmp8D6)),((_tmp8D6->hd=Cyc_Absyn_new_decl((void*)((_tmp8D8=_cycalloc(sizeof(*
_tmp8D8)),((_tmp8D8[0]=((_tmp8D7.tag=4,((_tmp8D7.f1=_tmp172,_tmp8D7)))),_tmp8D8)))),
0),((_tmp8D6->tl=Cyc_Toc_result_decls,_tmp8D6))))));}{struct _tuple9*_tmp8DB;
struct Cyc_List_List*_tmp8DA;*_tmp166=((_tmp8DA=_region_malloc(d,sizeof(*_tmp8DA)),((
_tmp8DA->hd=((_tmp8DB=_region_malloc(d,sizeof(*_tmp8DB)),((_tmp8DB->f1=x,((
_tmp8DB->f2=ts,_tmp8DB)))))),((_tmp8DA->tl=*_tmp166,_tmp8DA))))));}{void*_tmp178=
x;_npop_handler(0);return _tmp178;}}};_pop_dynregion(d);}}struct _tuple1*Cyc_Toc_temp_var();
struct _tuple1*Cyc_Toc_temp_var(){int _tmp181=Cyc_Toc_temp_var_counter ++;struct
_dynforward_ptr*_tmp8EA;const char*_tmp8E9;void*_tmp8E8[1];struct Cyc_Int_pa_struct
_tmp8E7;union Cyc_Absyn_Nmspace_union _tmp8E6;struct _tuple1*_tmp8E5;struct _tuple1*
res=(_tmp8E5=_cycalloc(sizeof(*_tmp8E5)),((_tmp8E5->f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp8E6.Loc_n).tag=0,_tmp8E6)),((_tmp8E5->f2=((_tmp8EA=_cycalloc(sizeof(*_tmp8EA)),((
_tmp8EA[0]=(struct _dynforward_ptr)((_tmp8E7.tag=1,((_tmp8E7.f1=(unsigned int)
_tmp181,((_tmp8E8[0]=& _tmp8E7,Cyc_aprintf(((_tmp8E9="_tmp%X",_tag_dynforward(
_tmp8E9,sizeof(char),_get_zero_arr_size_char(_tmp8E9,7)))),_tag_dynforward(
_tmp8E8,sizeof(void*),1)))))))),_tmp8EA)))),_tmp8E5)))));return res;}static struct
_dynforward_ptr*Cyc_Toc_fresh_label();static struct _dynforward_ptr*Cyc_Toc_fresh_label(){
struct _DynRegionHandle*_tmp189;struct Cyc_Xarray_Xarray*_tmp18A;struct Cyc_Toc_TocState
_tmp188=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp189=
_tmp188.dyn;_tmp18A=_tmp188.temp_labels;{struct _DynRegionFrame _tmp18B;struct
_RegionHandle*d=_open_dynregion(& _tmp18B,_tmp189);{int _tmp18C=Cyc_Toc_fresh_label_counter
++;if(_tmp18C < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp18A)){
struct _dynforward_ptr*_tmp18D=((struct _dynforward_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp18A,_tmp18C);_npop_handler(0);return _tmp18D;}{struct Cyc_Int_pa_struct
_tmp8F2;void*_tmp8F1[1];const char*_tmp8F0;struct _dynforward_ptr*_tmp8EF;struct
_dynforward_ptr*res=(_tmp8EF=_cycalloc(sizeof(*_tmp8EF)),((_tmp8EF[0]=(struct
_dynforward_ptr)((_tmp8F2.tag=1,((_tmp8F2.f1=(unsigned int)_tmp18C,((_tmp8F1[0]=&
_tmp8F2,Cyc_aprintf(((_tmp8F0="_LL%X",_tag_dynforward(_tmp8F0,sizeof(char),
_get_zero_arr_size_char(_tmp8F0,6)))),_tag_dynforward(_tmp8F1,sizeof(void*),1)))))))),
_tmp8EF)));if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dynforward_ptr*))Cyc_Xarray_add_ind)(
_tmp18A,res)!= _tmp18C){const char*_tmp8F5;void*_tmp8F4;(_tmp8F4=0,((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmp8F5="fresh_label: add_ind returned bad index...",
_tag_dynforward(_tmp8F5,sizeof(char),_get_zero_arr_size_char(_tmp8F5,43)))),
_tag_dynforward(_tmp8F4,sizeof(void*),0)));}{struct _dynforward_ptr*_tmp190=res;
_npop_handler(0);return _tmp190;}}};_pop_dynregion(d);}}static struct Cyc_Absyn_Exp*
Cyc_Toc_tunion_tag(struct Cyc_Absyn_Tuniondecl*td,struct _tuple1*name,int
carries_value);static struct Cyc_Absyn_Exp*Cyc_Toc_tunion_tag(struct Cyc_Absyn_Tuniondecl*
td,struct _tuple1*name,int carries_value){int ans=0;struct Cyc_List_List*_tmp195=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(
name,((struct Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp195))->hd)->name)
!= 0){if((td->is_flat  || carries_value  && ((struct Cyc_Absyn_Tunionfield*)_tmp195->hd)->typs
!= 0) || !carries_value  && ((struct Cyc_Absyn_Tunionfield*)_tmp195->hd)->typs == 0)
++ ans;_tmp195=_tmp195->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}static
int Cyc_Toc_num_void_tags(struct Cyc_Absyn_Tuniondecl*td);static int Cyc_Toc_num_void_tags(
struct Cyc_Absyn_Tuniondecl*td){int ans=0;{struct Cyc_List_List*_tmp196=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(td->fields))->v;for(0;_tmp196 != 0;_tmp196=
_tmp196->tl){if(((struct Cyc_Absyn_Tunionfield*)_tmp196->hd)->typs == 0)++ ans;}}
return ans;}static void*Cyc_Toc_typ_to_c(void*t);static struct _tuple2*Cyc_Toc_arg_to_c(
struct _tuple2*a);static struct _tuple2*Cyc_Toc_arg_to_c(struct _tuple2*a){struct Cyc_Core_Opt*
_tmp198;struct Cyc_Absyn_Tqual _tmp199;void*_tmp19A;struct _tuple2 _tmp197=*a;
_tmp198=_tmp197.f1;_tmp199=_tmp197.f2;_tmp19A=_tmp197.f3;{struct _tuple2*_tmp8F6;
return(_tmp8F6=_cycalloc(sizeof(*_tmp8F6)),((_tmp8F6->f1=_tmp198,((_tmp8F6->f2=
_tmp199,((_tmp8F6->f3=Cyc_Toc_typ_to_c(_tmp19A),_tmp8F6)))))));}}static struct
_tuple4*Cyc_Toc_typ_to_c_f(struct _tuple4*x);static struct _tuple4*Cyc_Toc_typ_to_c_f(
struct _tuple4*x){struct Cyc_Absyn_Tqual _tmp19D;void*_tmp19E;struct _tuple4 _tmp19C=*
x;_tmp19D=_tmp19C.f1;_tmp19E=_tmp19C.f2;{struct _tuple4*_tmp8F7;return(_tmp8F7=
_cycalloc(sizeof(*_tmp8F7)),((_tmp8F7->f1=_tmp19D,((_tmp8F7->f2=Cyc_Toc_typ_to_c(
_tmp19E),_tmp8F7)))));}}static void*Cyc_Toc_typ_to_c_array(void*t);static void*Cyc_Toc_typ_to_c_array(
void*t){void*_tmp1A0=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp1A1;
void*_tmp1A2;struct Cyc_Absyn_Tqual _tmp1A3;struct Cyc_Absyn_Exp*_tmp1A4;struct Cyc_Absyn_Conref*
_tmp1A5;struct Cyc_Position_Segment*_tmp1A6;struct Cyc_Core_Opt*_tmp1A7;struct Cyc_Core_Opt
_tmp1A8;void*_tmp1A9;_LL3B: if(_tmp1A0 <= (void*)4)goto _LL3F;if(*((int*)_tmp1A0)!= 
7)goto _LL3D;_tmp1A1=((struct Cyc_Absyn_ArrayType_struct*)_tmp1A0)->f1;_tmp1A2=(
void*)_tmp1A1.elt_type;_tmp1A3=_tmp1A1.tq;_tmp1A4=_tmp1A1.num_elts;_tmp1A5=
_tmp1A1.zero_term;_tmp1A6=_tmp1A1.zt_loc;_LL3C: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(
_tmp1A2),_tmp1A3,_tmp1A4,Cyc_Absyn_false_conref,_tmp1A6);_LL3D: if(*((int*)
_tmp1A0)!= 0)goto _LL3F;_tmp1A7=((struct Cyc_Absyn_Evar_struct*)_tmp1A0)->f2;if(
_tmp1A7 == 0)goto _LL3F;_tmp1A8=*_tmp1A7;_tmp1A9=(void*)_tmp1A8.v;_LL3E: return Cyc_Toc_typ_to_c_array(
_tmp1A9);_LL3F:;_LL40: return Cyc_Toc_typ_to_c(t);_LL3A:;}static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f);static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){struct Cyc_Absyn_Aggrfield*
_tmp8F8;return(_tmp8F8=_cycalloc(sizeof(*_tmp8F8)),((_tmp8F8->name=f->name,((
_tmp8F8->tq=Cyc_Toc_mt_tq,((_tmp8F8->type=(void*)Cyc_Toc_typ_to_c((void*)f->type),((
_tmp8F8->width=f->width,((_tmp8F8->attributes=f->attributes,_tmp8F8)))))))))));}
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs);static void Cyc_Toc_enumfields_to_c(
struct Cyc_List_List*fs){return;}static void*Cyc_Toc_char_star_typ();static void*
Cyc_Toc_char_star_typ(){static void**cs=0;if(cs == 0){void**_tmp8F9;cs=((_tmp8F9=
_cycalloc(sizeof(*_tmp8F9)),((_tmp8F9[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(
void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmp8F9))));}return*cs;}static void*
Cyc_Toc_rgn_typ();static void*Cyc_Toc_rgn_typ(){static void**r=0;if(r == 0){void**
_tmp8FA;r=((_tmp8FA=_cycalloc(sizeof(*_tmp8FA)),((_tmp8FA[0]=Cyc_Absyn_cstar_typ(
Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),_tmp8FA))));}return*r;}
static void*Cyc_Toc_dyn_rgn_typ();static void*Cyc_Toc_dyn_rgn_typ(){static void**r=
0;if(r == 0){void**_tmp8FB;r=((_tmp8FB=_cycalloc(sizeof(*_tmp8FB)),((_tmp8FB[0]=
Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),
_tmp8FB))));}return*r;}static void*Cyc_Toc_typ_to_c(void*t);static void*Cyc_Toc_typ_to_c(
void*t){void*_tmp1AE=t;struct Cyc_Core_Opt*_tmp1AF;struct Cyc_Core_Opt*_tmp1B0;
struct Cyc_Core_Opt _tmp1B1;void*_tmp1B2;struct Cyc_Absyn_Tvar*_tmp1B3;struct Cyc_Absyn_TunionInfo
_tmp1B4;union Cyc_Absyn_TunionInfoU_union _tmp1B5;struct Cyc_Absyn_Tuniondecl**
_tmp1B6;struct Cyc_Absyn_Tuniondecl*_tmp1B7;struct Cyc_Absyn_TunionFieldInfo
_tmp1B8;union Cyc_Absyn_TunionFieldInfoU_union _tmp1B9;struct Cyc_Absyn_Tuniondecl*
_tmp1BA;struct Cyc_Absyn_Tunionfield*_tmp1BB;struct Cyc_Absyn_PtrInfo _tmp1BC;void*
_tmp1BD;struct Cyc_Absyn_Tqual _tmp1BE;struct Cyc_Absyn_PtrAtts _tmp1BF;struct Cyc_Absyn_Conref*
_tmp1C0;struct Cyc_Absyn_ArrayInfo _tmp1C1;void*_tmp1C2;struct Cyc_Absyn_Tqual
_tmp1C3;struct Cyc_Absyn_Exp*_tmp1C4;struct Cyc_Position_Segment*_tmp1C5;struct Cyc_Absyn_FnInfo
_tmp1C6;void*_tmp1C7;struct Cyc_List_List*_tmp1C8;int _tmp1C9;struct Cyc_Absyn_VarargInfo*
_tmp1CA;struct Cyc_List_List*_tmp1CB;struct Cyc_List_List*_tmp1CC;void*_tmp1CD;
struct Cyc_List_List*_tmp1CE;struct Cyc_Absyn_AggrInfo _tmp1CF;union Cyc_Absyn_AggrInfoU_union
_tmp1D0;struct Cyc_List_List*_tmp1D1;struct _tuple1*_tmp1D2;struct Cyc_List_List*
_tmp1D3;struct _tuple1*_tmp1D4;struct Cyc_List_List*_tmp1D5;struct Cyc_Absyn_Typedefdecl*
_tmp1D6;void**_tmp1D7;void*_tmp1D8;_LL42: if((int)_tmp1AE != 0)goto _LL44;_LL43:
return t;_LL44: if(_tmp1AE <= (void*)4)goto _LL56;if(*((int*)_tmp1AE)!= 0)goto _LL46;
_tmp1AF=((struct Cyc_Absyn_Evar_struct*)_tmp1AE)->f2;if(_tmp1AF != 0)goto _LL46;
_LL45: return Cyc_Absyn_sint_typ;_LL46: if(*((int*)_tmp1AE)!= 0)goto _LL48;_tmp1B0=((
struct Cyc_Absyn_Evar_struct*)_tmp1AE)->f2;if(_tmp1B0 == 0)goto _LL48;_tmp1B1=*
_tmp1B0;_tmp1B2=(void*)_tmp1B1.v;_LL47: return Cyc_Toc_typ_to_c(_tmp1B2);_LL48: if(*((
int*)_tmp1AE)!= 1)goto _LL4A;_tmp1B3=((struct Cyc_Absyn_VarType_struct*)_tmp1AE)->f1;
_LL49: if(Cyc_Tcutil_tvar_kind(_tmp1B3)== (void*)0)return(void*)0;else{return Cyc_Absyn_void_star_typ();}
_LL4A: if(*((int*)_tmp1AE)!= 2)goto _LL4C;_tmp1B4=((struct Cyc_Absyn_TunionType_struct*)
_tmp1AE)->f1;_tmp1B5=_tmp1B4.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp1AE)->f1).tunion_info).KnownTunion).tag != 1)goto _LL4C;_tmp1B6=(_tmp1B5.KnownTunion).f1;
_tmp1B7=*_tmp1B6;_LL4B: if(_tmp1B7->is_flat){const char*_tmp8FC;return Cyc_Absyn_unionq_typ(
Cyc_Toc_collapse_qvar_tag(_tmp1B7->name,((_tmp8FC="_union",_tag_dynforward(
_tmp8FC,sizeof(char),_get_zero_arr_size_char(_tmp8FC,7))))));}else{return Cyc_Absyn_void_star_typ();}
_LL4C: if(*((int*)_tmp1AE)!= 2)goto _LL4E;_LL4D: {const char*_tmp8FF;void*_tmp8FE;(
_tmp8FE=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((
_tmp8FF="unresolved TunionType",_tag_dynforward(_tmp8FF,sizeof(char),
_get_zero_arr_size_char(_tmp8FF,22)))),_tag_dynforward(_tmp8FE,sizeof(void*),0)));}
_LL4E: if(*((int*)_tmp1AE)!= 3)goto _LL50;_tmp1B8=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp1AE)->f1;_tmp1B9=_tmp1B8.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp1AE)->f1).field_info).KnownTunionfield).tag != 1)goto _LL50;_tmp1BA=(_tmp1B9.KnownTunionfield).f1;
_tmp1BB=(_tmp1B9.KnownTunionfield).f2;_LL4F: if(_tmp1BA->is_flat){const char*
_tmp900;return Cyc_Absyn_unionq_typ(Cyc_Toc_collapse_qvar_tag(_tmp1BA->name,((
_tmp900="_union",_tag_dynforward(_tmp900,sizeof(char),_get_zero_arr_size_char(
_tmp900,7))))));}if(_tmp1BB->typs == 0){if(_tmp1BA->is_xtunion)return Cyc_Toc_char_star_typ();
else{return Cyc_Absyn_uint_typ;}}else{const char*_tmp901;return Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp1BB->name,((_tmp901="_struct",_tag_dynforward(
_tmp901,sizeof(char),_get_zero_arr_size_char(_tmp901,8))))));}_LL50: if(*((int*)
_tmp1AE)!= 3)goto _LL52;_LL51: {const char*_tmp904;void*_tmp903;(_tmp903=0,((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmp904="unresolved TunionFieldType",
_tag_dynforward(_tmp904,sizeof(char),_get_zero_arr_size_char(_tmp904,27)))),
_tag_dynforward(_tmp903,sizeof(void*),0)));}_LL52: if(*((int*)_tmp1AE)!= 4)goto
_LL54;_tmp1BC=((struct Cyc_Absyn_PointerType_struct*)_tmp1AE)->f1;_tmp1BD=(void*)
_tmp1BC.elt_typ;_tmp1BE=_tmp1BC.elt_tq;_tmp1BF=_tmp1BC.ptr_atts;_tmp1C0=_tmp1BF.bounds;
_LL53: _tmp1BD=Cyc_Toc_typ_to_c_array(_tmp1BD);{union Cyc_Absyn_Constraint_union
_tmp1E0=(Cyc_Absyn_compress_conref(_tmp1C0))->v;void*_tmp1E1;void*_tmp1E2;_LL7F:
if((_tmp1E0.No_constr).tag != 2)goto _LL81;_LL80: goto _LL82;_LL81: if((_tmp1E0.Eq_constr).tag
!= 0)goto _LL83;_tmp1E1=(_tmp1E0.Eq_constr).f1;if((int)_tmp1E1 != 0)goto _LL83;
_LL82: return Cyc_Toc_dynforward_ptr_typ;_LL83: if((_tmp1E0.Eq_constr).tag != 0)goto
_LL85;_tmp1E2=(_tmp1E0.Eq_constr).f1;if((int)_tmp1E2 != 1)goto _LL85;_LL84: return
Cyc_Toc_dyneither_ptr_typ;_LL85:;_LL86: return Cyc_Absyn_star_typ(_tmp1BD,(void*)2,
_tmp1BE,Cyc_Absyn_false_conref);_LL7E:;}_LL54: if(*((int*)_tmp1AE)!= 5)goto _LL56;
_LL55: goto _LL57;_LL56: if((int)_tmp1AE != 1)goto _LL58;_LL57: goto _LL59;_LL58: if(
_tmp1AE <= (void*)4)goto _LL74;if(*((int*)_tmp1AE)!= 6)goto _LL5A;_LL59: return t;
_LL5A: if(*((int*)_tmp1AE)!= 7)goto _LL5C;_tmp1C1=((struct Cyc_Absyn_ArrayType_struct*)
_tmp1AE)->f1;_tmp1C2=(void*)_tmp1C1.elt_type;_tmp1C3=_tmp1C1.tq;_tmp1C4=_tmp1C1.num_elts;
_tmp1C5=_tmp1C1.zt_loc;_LL5B: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(
_tmp1C2),_tmp1C3,_tmp1C4,Cyc_Absyn_false_conref,_tmp1C5);_LL5C: if(*((int*)
_tmp1AE)!= 8)goto _LL5E;_tmp1C6=((struct Cyc_Absyn_FnType_struct*)_tmp1AE)->f1;
_tmp1C7=(void*)_tmp1C6.ret_typ;_tmp1C8=_tmp1C6.args;_tmp1C9=_tmp1C6.c_varargs;
_tmp1CA=_tmp1C6.cyc_varargs;_tmp1CB=_tmp1C6.attributes;_LL5D: {struct Cyc_List_List*
_tmp1E3=0;for(0;_tmp1CB != 0;_tmp1CB=_tmp1CB->tl){void*_tmp1E4=(void*)_tmp1CB->hd;
_LL88: if((int)_tmp1E4 != 3)goto _LL8A;_LL89: goto _LL8B;_LL8A: if((int)_tmp1E4 != 4)
goto _LL8C;_LL8B: goto _LL8D;_LL8C: if(_tmp1E4 <= (void*)17)goto _LL90;if(*((int*)
_tmp1E4)!= 3)goto _LL8E;_LL8D: continue;_LL8E: if(*((int*)_tmp1E4)!= 4)goto _LL90;
_LL8F: continue;_LL90:;_LL91:{struct Cyc_List_List*_tmp905;_tmp1E3=((_tmp905=
_cycalloc(sizeof(*_tmp905)),((_tmp905->hd=(void*)((void*)_tmp1CB->hd),((_tmp905->tl=
_tmp1E3,_tmp905))))));}goto _LL87;_LL87:;}{struct Cyc_List_List*_tmp1E6=((struct
Cyc_List_List*(*)(struct _tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_arg_to_c,_tmp1C8);if(_tmp1CA != 0){void*_tmp1E7=Cyc_Toc_typ_to_c(Cyc_Absyn_dynforward_typ((
void*)_tmp1CA->type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));struct
_tuple2*_tmp906;struct _tuple2*_tmp1E8=(_tmp906=_cycalloc(sizeof(*_tmp906)),((
_tmp906->f1=_tmp1CA->name,((_tmp906->f2=_tmp1CA->tq,((_tmp906->f3=_tmp1E7,
_tmp906)))))));struct Cyc_List_List*_tmp907;_tmp1E6=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp1E6,((
_tmp907=_cycalloc(sizeof(*_tmp907)),((_tmp907->hd=_tmp1E8,((_tmp907->tl=0,
_tmp907)))))));}{struct Cyc_Absyn_FnType_struct _tmp90D;struct Cyc_Absyn_FnInfo
_tmp90C;struct Cyc_Absyn_FnType_struct*_tmp90B;return(void*)((_tmp90B=_cycalloc(
sizeof(*_tmp90B)),((_tmp90B[0]=((_tmp90D.tag=8,((_tmp90D.f1=((_tmp90C.tvars=0,((
_tmp90C.effect=0,((_tmp90C.ret_typ=(void*)Cyc_Toc_typ_to_c(_tmp1C7),((_tmp90C.args=
_tmp1E6,((_tmp90C.c_varargs=_tmp1C9,((_tmp90C.cyc_varargs=0,((_tmp90C.rgn_po=0,((
_tmp90C.attributes=_tmp1E3,_tmp90C)))))))))))))))),_tmp90D)))),_tmp90B))));}}}
_LL5E: if(*((int*)_tmp1AE)!= 9)goto _LL60;_tmp1CC=((struct Cyc_Absyn_TupleType_struct*)
_tmp1AE)->f1;_LL5F: _tmp1CC=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct
_tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1CC);
return Cyc_Toc_add_tuple_type(_tmp1CC);_LL60: if(*((int*)_tmp1AE)!= 11)goto _LL62;
_tmp1CD=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp1AE)->f1;_tmp1CE=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp1AE)->f2;_LL61: {struct Cyc_Absyn_AnonAggrType_struct
_tmp910;struct Cyc_Absyn_AnonAggrType_struct*_tmp90F;return(void*)((_tmp90F=
_cycalloc(sizeof(*_tmp90F)),((_tmp90F[0]=((_tmp910.tag=11,((_tmp910.f1=(void*)
_tmp1CD,((_tmp910.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(
struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,
_tmp1CE),_tmp910)))))),_tmp90F))));}_LL62: if(*((int*)_tmp1AE)!= 10)goto _LL64;
_tmp1CF=((struct Cyc_Absyn_AggrType_struct*)_tmp1AE)->f1;_tmp1D0=_tmp1CF.aggr_info;
_tmp1D1=_tmp1CF.targs;_LL63: {struct Cyc_Absyn_Aggrdecl*_tmp1F0=Cyc_Absyn_get_known_aggrdecl(
_tmp1D0);if((void*)_tmp1F0->kind == (void*)1)return Cyc_Toc_aggrdecl_type(_tmp1F0->name,
Cyc_Absyn_unionq_typ);else{return Cyc_Toc_aggrdecl_type(_tmp1F0->name,Cyc_Absyn_strctq);}}
_LL64: if(*((int*)_tmp1AE)!= 12)goto _LL66;_tmp1D2=((struct Cyc_Absyn_EnumType_struct*)
_tmp1AE)->f1;_LL65: return t;_LL66: if(*((int*)_tmp1AE)!= 13)goto _LL68;_tmp1D3=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp1AE)->f1;_LL67: Cyc_Toc_enumfields_to_c(
_tmp1D3);return t;_LL68: if(*((int*)_tmp1AE)!= 17)goto _LL6A;_tmp1D4=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1AE)->f1;_tmp1D5=((struct Cyc_Absyn_TypedefType_struct*)_tmp1AE)->f2;_tmp1D6=((
struct Cyc_Absyn_TypedefType_struct*)_tmp1AE)->f3;_tmp1D7=((struct Cyc_Absyn_TypedefType_struct*)
_tmp1AE)->f4;_LL69: if(_tmp1D7 == 0  || Cyc_noexpand_r){if(_tmp1D5 != 0){struct Cyc_Absyn_TypedefType_struct
_tmp913;struct Cyc_Absyn_TypedefType_struct*_tmp912;return(void*)((_tmp912=
_cycalloc(sizeof(*_tmp912)),((_tmp912[0]=((_tmp913.tag=17,((_tmp913.f1=_tmp1D4,((
_tmp913.f2=0,((_tmp913.f3=_tmp1D6,((_tmp913.f4=0,_tmp913)))))))))),_tmp912))));}
else{return t;}}else{struct Cyc_Absyn_TypedefType_struct _tmp919;void**_tmp918;
struct Cyc_Absyn_TypedefType_struct*_tmp917;return(void*)((_tmp917=_cycalloc(
sizeof(*_tmp917)),((_tmp917[0]=((_tmp919.tag=17,((_tmp919.f1=_tmp1D4,((_tmp919.f2=
0,((_tmp919.f3=_tmp1D6,((_tmp919.f4=((_tmp918=_cycalloc(sizeof(*_tmp918)),((
_tmp918[0]=Cyc_Toc_typ_to_c_array(*_tmp1D7),_tmp918)))),_tmp919)))))))))),
_tmp917))));}_LL6A: if(*((int*)_tmp1AE)!= 14)goto _LL6C;_LL6B: goto _LL6D;_LL6C: if(*((
int*)_tmp1AE)!= 18)goto _LL6E;_LL6D: return Cyc_Absyn_uint_typ;_LL6E: if(*((int*)
_tmp1AE)!= 15)goto _LL70;_tmp1D8=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp1AE)->f1;_LL6F: return Cyc_Toc_rgn_typ();_LL70: if(*((int*)_tmp1AE)!= 16)goto
_LL72;_LL71: return Cyc_Toc_dyn_rgn_typ();_LL72: if(*((int*)_tmp1AE)!= 19)goto _LL74;
_LL73: {const char*_tmp91C;void*_tmp91B;(_tmp91B=0,((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmp91C="Toc::typ_to_c: type translation passed a type integer",
_tag_dynforward(_tmp91C,sizeof(char),_get_zero_arr_size_char(_tmp91C,54)))),
_tag_dynforward(_tmp91B,sizeof(void*),0)));}_LL74: if((int)_tmp1AE != 2)goto _LL76;
_LL75: {const char*_tmp91F;void*_tmp91E;(_tmp91E=0,((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmp91F="Toc::typ_to_c: type translation passed the heap region",
_tag_dynforward(_tmp91F,sizeof(char),_get_zero_arr_size_char(_tmp91F,55)))),
_tag_dynforward(_tmp91E,sizeof(void*),0)));}_LL76: if((int)_tmp1AE != 3)goto _LL78;
_LL77: {const char*_tmp922;void*_tmp921;(_tmp921=0,((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmp922="Toc::typ_to_c: type translation passed the unique region",
_tag_dynforward(_tmp922,sizeof(char),_get_zero_arr_size_char(_tmp922,57)))),
_tag_dynforward(_tmp921,sizeof(void*),0)));}_LL78: if(_tmp1AE <= (void*)4)goto
_LL7A;if(*((int*)_tmp1AE)!= 20)goto _LL7A;_LL79: goto _LL7B;_LL7A: if(_tmp1AE <= (
void*)4)goto _LL7C;if(*((int*)_tmp1AE)!= 21)goto _LL7C;_LL7B: goto _LL7D;_LL7C: if(
_tmp1AE <= (void*)4)goto _LL41;if(*((int*)_tmp1AE)!= 22)goto _LL41;_LL7D: {const
char*_tmp925;void*_tmp924;(_tmp924=0,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmp925="Toc::typ_to_c: type translation passed an effect",
_tag_dynforward(_tmp925,sizeof(char),_get_zero_arr_size_char(_tmp925,49)))),
_tag_dynforward(_tmp924,sizeof(void*),0)));}_LL41:;}static struct Cyc_Absyn_Exp*
Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
l);static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*l){void*_tmp1FE=t;struct Cyc_Absyn_ArrayInfo _tmp1FF;
void*_tmp200;struct Cyc_Absyn_Tqual _tmp201;_LL93: if(_tmp1FE <= (void*)4)goto _LL95;
if(*((int*)_tmp1FE)!= 7)goto _LL95;_tmp1FF=((struct Cyc_Absyn_ArrayType_struct*)
_tmp1FE)->f1;_tmp200=(void*)_tmp1FF.elt_type;_tmp201=_tmp1FF.tq;_LL94: return Cyc_Toc_cast_it(
Cyc_Absyn_star_typ(_tmp200,(void*)2,_tmp201,Cyc_Absyn_false_conref),e);_LL95:;
_LL96: return Cyc_Toc_cast_it(t,e);_LL92:;}static int Cyc_Toc_atomic_typ(void*t);
static int Cyc_Toc_atomic_typ(void*t){void*_tmp202=Cyc_Tcutil_compress(t);struct
Cyc_Absyn_ArrayInfo _tmp203;void*_tmp204;struct Cyc_Absyn_AggrInfo _tmp205;union Cyc_Absyn_AggrInfoU_union
_tmp206;struct Cyc_List_List*_tmp207;struct Cyc_Absyn_TunionFieldInfo _tmp208;union
Cyc_Absyn_TunionFieldInfoU_union _tmp209;struct Cyc_Absyn_Tuniondecl*_tmp20A;
struct Cyc_Absyn_Tunionfield*_tmp20B;struct Cyc_List_List*_tmp20C;_LL98: if((int)
_tmp202 != 0)goto _LL9A;_LL99: return 1;_LL9A: if(_tmp202 <= (void*)4)goto _LLA2;if(*((
int*)_tmp202)!= 1)goto _LL9C;_LL9B: return 0;_LL9C: if(*((int*)_tmp202)!= 5)goto
_LL9E;_LL9D: goto _LL9F;_LL9E: if(*((int*)_tmp202)!= 12)goto _LLA0;_LL9F: goto _LLA1;
_LLA0: if(*((int*)_tmp202)!= 13)goto _LLA2;_LLA1: goto _LLA3;_LLA2: if((int)_tmp202 != 
1)goto _LLA4;_LLA3: goto _LLA5;_LLA4: if(_tmp202 <= (void*)4)goto _LLBC;if(*((int*)
_tmp202)!= 6)goto _LLA6;_LLA5: goto _LLA7;_LLA6: if(*((int*)_tmp202)!= 8)goto _LLA8;
_LLA7: goto _LLA9;_LLA8: if(*((int*)_tmp202)!= 18)goto _LLAA;_LLA9: goto _LLAB;_LLAA:
if(*((int*)_tmp202)!= 14)goto _LLAC;_LLAB: return 1;_LLAC: if(*((int*)_tmp202)!= 7)
goto _LLAE;_tmp203=((struct Cyc_Absyn_ArrayType_struct*)_tmp202)->f1;_tmp204=(void*)
_tmp203.elt_type;_LLAD: return Cyc_Toc_atomic_typ(_tmp204);_LLAE: if(*((int*)
_tmp202)!= 10)goto _LLB0;_tmp205=((struct Cyc_Absyn_AggrType_struct*)_tmp202)->f1;
_tmp206=_tmp205.aggr_info;_LLAF:{union Cyc_Absyn_AggrInfoU_union _tmp20D=_tmp206;
_LLBF: if((_tmp20D.UnknownAggr).tag != 0)goto _LLC1;_LLC0: return 0;_LLC1:;_LLC2: goto
_LLBE;_LLBE:;}{struct Cyc_Absyn_Aggrdecl*_tmp20E=Cyc_Absyn_get_known_aggrdecl(
_tmp206);if(_tmp20E->impl == 0)return 0;{struct Cyc_List_List*_tmp20F=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp20E->impl))->fields;for(0;_tmp20F != 0;_tmp20F=_tmp20F->tl){if(!
Cyc_Toc_atomic_typ((void*)((struct Cyc_Absyn_Aggrfield*)_tmp20F->hd)->type))
return 0;}}return 1;}_LLB0: if(*((int*)_tmp202)!= 11)goto _LLB2;_tmp207=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp202)->f2;_LLB1: for(0;_tmp207 != 0;_tmp207=_tmp207->tl){if(!Cyc_Toc_atomic_typ((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp207->hd)->type))return 0;}return 1;_LLB2:
if(*((int*)_tmp202)!= 3)goto _LLB4;_tmp208=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp202)->f1;_tmp209=_tmp208.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp202)->f1).field_info).KnownTunionfield).tag != 1)goto _LLB4;_tmp20A=(_tmp209.KnownTunionfield).f1;
_tmp20B=(_tmp209.KnownTunionfield).f2;_LLB3: _tmp20C=_tmp20B->typs;goto _LLB5;
_LLB4: if(*((int*)_tmp202)!= 9)goto _LLB6;_tmp20C=((struct Cyc_Absyn_TupleType_struct*)
_tmp202)->f1;_LLB5: for(0;_tmp20C != 0;_tmp20C=_tmp20C->tl){if(!Cyc_Toc_atomic_typ((*((
struct _tuple4*)_tmp20C->hd)).f2))return 0;}return 1;_LLB6: if(*((int*)_tmp202)!= 2)
goto _LLB8;_LLB7: goto _LLB9;_LLB8: if(*((int*)_tmp202)!= 4)goto _LLBA;_LLB9: goto
_LLBB;_LLBA: if(*((int*)_tmp202)!= 15)goto _LLBC;_LLBB: return 0;_LLBC:;_LLBD: {
const char*_tmp929;void*_tmp928[1];struct Cyc_String_pa_struct _tmp927;(_tmp927.tag=
0,((_tmp927.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp928[0]=& _tmp927,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmp929="atomic_typ:  bad type %s",
_tag_dynforward(_tmp929,sizeof(char),_get_zero_arr_size_char(_tmp929,25)))),
_tag_dynforward(_tmp928,sizeof(void*),1)))))));}_LL97:;}static int Cyc_Toc_is_void_star(
void*t);static int Cyc_Toc_is_void_star(void*t){void*_tmp213=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_PtrInfo _tmp214;void*_tmp215;_LLC4: if(_tmp213 <= (void*)4)goto
_LLC6;if(*((int*)_tmp213)!= 4)goto _LLC6;_tmp214=((struct Cyc_Absyn_PointerType_struct*)
_tmp213)->f1;_tmp215=(void*)_tmp214.elt_typ;_LLC5: {void*_tmp216=Cyc_Tcutil_compress(
_tmp215);_LLC9: if((int)_tmp216 != 0)goto _LLCB;_LLCA: return 1;_LLCB:;_LLCC: return 0;
_LLC8:;}_LLC6:;_LLC7: return 0;_LLC3:;}static int Cyc_Toc_is_poly_field(void*t,
struct _dynforward_ptr*f);static int Cyc_Toc_is_poly_field(void*t,struct
_dynforward_ptr*f){void*_tmp217=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp218;union Cyc_Absyn_AggrInfoU_union _tmp219;struct Cyc_List_List*_tmp21A;_LLCE:
if(_tmp217 <= (void*)4)goto _LLD2;if(*((int*)_tmp217)!= 10)goto _LLD0;_tmp218=((
struct Cyc_Absyn_AggrType_struct*)_tmp217)->f1;_tmp219=_tmp218.aggr_info;_LLCF: {
struct Cyc_Absyn_Aggrdecl*_tmp21B=Cyc_Absyn_get_known_aggrdecl(_tmp219);if(
_tmp21B->impl == 0){const char*_tmp92C;void*_tmp92B;(_tmp92B=0,((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmp92C="is_poly_field: type missing fields",
_tag_dynforward(_tmp92C,sizeof(char),_get_zero_arr_size_char(_tmp92C,35)))),
_tag_dynforward(_tmp92B,sizeof(void*),0)));}_tmp21A=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp21B->impl))->fields;goto _LLD1;}_LLD0: if(*((int*)_tmp217)!= 11)
goto _LLD2;_tmp21A=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp217)->f2;_LLD1: {
struct Cyc_Absyn_Aggrfield*_tmp21E=Cyc_Absyn_lookup_field(_tmp21A,f);if(_tmp21E == 
0){const char*_tmp930;void*_tmp92F[1];struct Cyc_String_pa_struct _tmp92E;(_tmp92E.tag=
0,((_tmp92E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*f),((_tmp92F[0]=&
_tmp92E,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((
_tmp930="is_poly_field: bad field %s",_tag_dynforward(_tmp930,sizeof(char),
_get_zero_arr_size_char(_tmp930,28)))),_tag_dynforward(_tmp92F,sizeof(void*),1)))))));}
return Cyc_Toc_is_void_star((void*)_tmp21E->type);}_LLD2:;_LLD3: {const char*
_tmp934;void*_tmp933[1];struct Cyc_String_pa_struct _tmp932;(_tmp932.tag=0,((
_tmp932.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp933[0]=& _tmp932,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmp934="is_poly_field: bad type %s",
_tag_dynforward(_tmp934,sizeof(char),_get_zero_arr_size_char(_tmp934,27)))),
_tag_dynforward(_tmp933,sizeof(void*),1)))))));}_LLCD:;}static int Cyc_Toc_is_poly_project(
struct Cyc_Absyn_Exp*e);static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp225=(void*)e->r;struct Cyc_Absyn_Exp*_tmp226;struct _dynforward_ptr*
_tmp227;struct Cyc_Absyn_Exp*_tmp228;struct _dynforward_ptr*_tmp229;_LLD5: if(*((
int*)_tmp225)!= 23)goto _LLD7;_tmp226=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp225)->f1;_tmp227=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp225)->f2;_LLD6:
return Cyc_Toc_is_poly_field((void*)((struct Cyc_Core_Opt*)_check_null(_tmp226->topt))->v,
_tmp227);_LLD7: if(*((int*)_tmp225)!= 24)goto _LLD9;_tmp228=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp225)->f1;_tmp229=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp225)->f2;_LLD8: {
void*_tmp22A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp228->topt))->v);struct Cyc_Absyn_PtrInfo _tmp22B;void*_tmp22C;_LLDC: if(_tmp22A
<= (void*)4)goto _LLDE;if(*((int*)_tmp22A)!= 4)goto _LLDE;_tmp22B=((struct Cyc_Absyn_PointerType_struct*)
_tmp22A)->f1;_tmp22C=(void*)_tmp22B.elt_typ;_LLDD: return Cyc_Toc_is_poly_field(
_tmp22C,_tmp229);_LLDE:;_LLDF: {const char*_tmp938;void*_tmp937[1];struct Cyc_String_pa_struct
_tmp936;(_tmp936.tag=0,((_tmp936.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp228->topt))->v)),((_tmp937[0]=& _tmp936,((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmp938="is_poly_project: bad type %s",
_tag_dynforward(_tmp938,sizeof(char),_get_zero_arr_size_char(_tmp938,29)))),
_tag_dynforward(_tmp937,sizeof(void*),1)))))));}_LLDB:;}_LLD9:;_LLDA: return 0;
_LLD4:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s);
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){struct Cyc_List_List*
_tmp939;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmp939=_cycalloc(
sizeof(*_tmp939)),((_tmp939->hd=s,((_tmp939->tl=0,_tmp939)))))),0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){struct Cyc_List_List*_tmp93A;return
Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmp93A=_cycalloc(sizeof(*
_tmp93A)),((_tmp93A->hd=s,((_tmp93A->tl=0,_tmp93A)))))),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(
void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(t))return Cyc_Toc_malloc_atomic(
s);return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s);static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(
struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){struct Cyc_Absyn_Exp*_tmp93B[2];
return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmp93B[1]=s,((_tmp93B[0]=
rgn,((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp93B,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n);
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,
struct Cyc_Absyn_Exp*n){if(Cyc_Toc_atomic_typ(elt_type)){struct Cyc_Absyn_Exp*
_tmp93C[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmp93C[1]=n,((
_tmp93C[0]=s,((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp93C,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{struct Cyc_Absyn_Exp*
_tmp93D[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmp93D[1]=n,((
_tmp93D[0]=s,((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp93D,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}static struct
Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,
struct Cyc_Absyn_Exp*n);static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*
rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){struct Cyc_Absyn_Exp*_tmp93E[3];
return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((_tmp93E[2]=n,((_tmp93E[1]=
s,((_tmp93E[0]=rgn,((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp93E,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e);static struct Cyc_Absyn_Exp*
Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){struct Cyc_List_List*_tmp93F;return
Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((_tmp93F=_cycalloc(sizeof(*_tmp93F)),((
_tmp93F->hd=e,((_tmp93F->tl=0,_tmp93F)))))),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(
void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e,int forward_only);static
struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,
struct Cyc_Absyn_Exp*e,int forward_only){int is_string=0;{void*_tmp237=(void*)e->r;
union Cyc_Absyn_Cnst_union _tmp238;_LLE1: if(*((int*)_tmp237)!= 0)goto _LLE3;_tmp238=((
struct Cyc_Absyn_Const_e_struct*)_tmp237)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp237)->f1).String_c).tag != 5)goto _LLE3;_LLE2: is_string=1;goto _LLE0;_LLE3:;
_LLE4: goto _LLE0;_LLE0:;}{struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*xplussz;
if(is_string){struct _tuple1*x=Cyc_Toc_temp_var();void*vd_typ=Cyc_Absyn_array_typ(
Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)sz,Cyc_Absyn_false_conref,
0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,(struct Cyc_Absyn_Exp*)
e);{struct Cyc_Absyn_Var_d_struct*_tmp945;struct Cyc_Absyn_Var_d_struct _tmp944;
struct Cyc_List_List*_tmp943;Cyc_Toc_result_decls=((_tmp943=_cycalloc(sizeof(*
_tmp943)),((_tmp943->hd=Cyc_Absyn_new_decl((void*)((_tmp945=_cycalloc(sizeof(*
_tmp945)),((_tmp945[0]=((_tmp944.tag=0,((_tmp944.f1=vd,_tmp944)))),_tmp945)))),0),((
_tmp943->tl=Cyc_Toc_result_decls,_tmp943))))));}xexp=Cyc_Absyn_var_exp(x,0);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
struct Cyc_Absyn_Exp*urm_exp;if(!forward_only){struct _tuple8*_tmp94C;struct
_tuple8*_tmp94B;struct _tuple8*_tmp94A;struct _tuple8*_tmp949[3];urm_exp=Cyc_Absyn_unresolvedmem_exp(
0,((_tmp949[2]=((_tmp94A=_cycalloc(sizeof(*_tmp94A)),((_tmp94A->f1=0,((_tmp94A->f2=
xplussz,_tmp94A)))))),((_tmp949[1]=((_tmp94B=_cycalloc(sizeof(*_tmp94B)),((
_tmp94B->f1=0,((_tmp94B->f2=xexp,_tmp94B)))))),((_tmp949[0]=((_tmp94C=_cycalloc(
sizeof(*_tmp94C)),((_tmp94C->f1=0,((_tmp94C->f2=xexp,_tmp94C)))))),((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp949,sizeof(struct
_tuple8*),3)))))))),0);}else{struct _tuple8*_tmp951;struct _tuple8*_tmp950;struct
_tuple8*_tmp94F[2];urm_exp=Cyc_Absyn_unresolvedmem_exp(0,((_tmp94F[1]=((_tmp950=
_cycalloc(sizeof(*_tmp950)),((_tmp950->f1=0,((_tmp950->f2=xplussz,_tmp950)))))),((
_tmp94F[0]=((_tmp951=_cycalloc(sizeof(*_tmp951)),((_tmp951->f1=0,((_tmp951->f2=
xexp,_tmp951)))))),((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp94F,sizeof(struct _tuple8*),2)))))),0);}return urm_exp;}}}
struct Cyc_Toc_FallthruInfo{struct _dynforward_ptr*label;struct Cyc_List_List*
binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{struct
_dynforward_ptr**break_lab;struct _dynforward_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*
fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;};static int Cyc_Toc_is_toplevel(
struct Cyc_Toc_Env*nv);static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){struct
Cyc_Toc_Env _tmp244;int _tmp245;struct Cyc_Toc_Env*_tmp243=nv;_tmp244=*_tmp243;
_tmp245=_tmp244.toplevel;return _tmp245;}static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(
struct Cyc_Toc_Env*nv,struct _tuple1*x);static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(
struct Cyc_Toc_Env*nv,struct _tuple1*x){struct Cyc_Toc_Env _tmp247;struct Cyc_Dict_Dict
_tmp248;struct Cyc_Toc_Env*_tmp246=nv;_tmp247=*_tmp246;_tmp248=_tmp247.varmap;
return((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(
_tmp248,x);}static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r);
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){struct Cyc_Toc_Env*
_tmp952;return(_tmp952=_region_malloc(r,sizeof(*_tmp952)),((_tmp952->break_lab=(
struct _dynforward_ptr**)0,((_tmp952->continue_lab=(struct _dynforward_ptr**)0,((
_tmp952->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmp952->varmap=(struct
Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct
_tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((_tmp952->toplevel=(
int)1,_tmp952)))))))))));}static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct
_RegionHandle*r,struct Cyc_Toc_Env*e);static struct Cyc_Toc_Env*Cyc_Toc_share_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp24B;struct
_dynforward_ptr**_tmp24C;struct _dynforward_ptr**_tmp24D;struct Cyc_Toc_FallthruInfo*
_tmp24E;struct Cyc_Dict_Dict _tmp24F;int _tmp250;struct Cyc_Toc_Env*_tmp24A=e;
_tmp24B=*_tmp24A;_tmp24C=_tmp24B.break_lab;_tmp24D=_tmp24B.continue_lab;_tmp24E=
_tmp24B.fallthru_info;_tmp24F=_tmp24B.varmap;_tmp250=_tmp24B.toplevel;{struct Cyc_Toc_Env*
_tmp953;return(_tmp953=_region_malloc(r,sizeof(*_tmp953)),((_tmp953->break_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp24C),((_tmp953->continue_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp24D),((_tmp953->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp24E,((_tmp953->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp24F),((_tmp953->toplevel=(int)_tmp250,_tmp953)))))))))));}}static struct Cyc_Toc_Env*
Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e);static struct
Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env _tmp253;struct _dynforward_ptr**_tmp254;struct _dynforward_ptr**
_tmp255;struct Cyc_Toc_FallthruInfo*_tmp256;struct Cyc_Dict_Dict _tmp257;int _tmp258;
struct Cyc_Toc_Env*_tmp252=e;_tmp253=*_tmp252;_tmp254=_tmp253.break_lab;_tmp255=
_tmp253.continue_lab;_tmp256=_tmp253.fallthru_info;_tmp257=_tmp253.varmap;
_tmp258=_tmp253.toplevel;{struct Cyc_Toc_Env*_tmp954;return(_tmp954=
_region_malloc(r,sizeof(*_tmp954)),((_tmp954->break_lab=(struct _dynforward_ptr**)((
struct _dynforward_ptr**)_tmp254),((_tmp954->continue_lab=(struct _dynforward_ptr**)((
struct _dynforward_ptr**)_tmp255),((_tmp954->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp256,((_tmp954->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp257),((_tmp954->toplevel=(
int)0,_tmp954)))))))))));}}static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct
_RegionHandle*r,struct Cyc_Toc_Env*e);static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(
struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp25B;struct
_dynforward_ptr**_tmp25C;struct _dynforward_ptr**_tmp25D;struct Cyc_Toc_FallthruInfo*
_tmp25E;struct Cyc_Dict_Dict _tmp25F;int _tmp260;struct Cyc_Toc_Env*_tmp25A=e;
_tmp25B=*_tmp25A;_tmp25C=_tmp25B.break_lab;_tmp25D=_tmp25B.continue_lab;_tmp25E=
_tmp25B.fallthru_info;_tmp25F=_tmp25B.varmap;_tmp260=_tmp25B.toplevel;{struct Cyc_Toc_Env*
_tmp955;return(_tmp955=_region_malloc(r,sizeof(*_tmp955)),((_tmp955->break_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp25C),((_tmp955->continue_lab=(
struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp25D),((_tmp955->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp25E,((_tmp955->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp25F),((_tmp955->toplevel=(int)1,_tmp955)))))))))));}}static struct Cyc_Toc_Env*
Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,
struct Cyc_Absyn_Exp*y);static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct
_RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){{
union Cyc_Absyn_Nmspace_union _tmp262=(*x).f1;_LLE6: if((_tmp262.Rel_n).tag != 1)
goto _LLE8;_LLE7: {const char*_tmp959;void*_tmp958[1];struct Cyc_String_pa_struct
_tmp957;(_tmp957.tag=0,((_tmp957.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_qvar2string(x)),((_tmp958[0]=& _tmp957,((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmp959="Toc::add_varmap on Rel_n: %s\n",
_tag_dynforward(_tmp959,sizeof(char),_get_zero_arr_size_char(_tmp959,30)))),
_tag_dynforward(_tmp958,sizeof(void*),1)))))));}_LLE8:;_LLE9: goto _LLE5;_LLE5:;}{
struct Cyc_Toc_Env _tmp267;struct _dynforward_ptr**_tmp268;struct _dynforward_ptr**
_tmp269;struct Cyc_Toc_FallthruInfo*_tmp26A;struct Cyc_Dict_Dict _tmp26B;int _tmp26C;
struct Cyc_Toc_Env*_tmp266=e;_tmp267=*_tmp266;_tmp268=_tmp267.break_lab;_tmp269=
_tmp267.continue_lab;_tmp26A=_tmp267.fallthru_info;_tmp26B=_tmp267.varmap;
_tmp26C=_tmp267.toplevel;{struct Cyc_Dict_Dict _tmp26D=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp26B),x,y);struct Cyc_Toc_Env*_tmp95A;return(_tmp95A=_region_malloc(r,
sizeof(*_tmp95A)),((_tmp95A->break_lab=(struct _dynforward_ptr**)((struct
_dynforward_ptr**)_tmp268),((_tmp95A->continue_lab=(struct _dynforward_ptr**)((
struct _dynforward_ptr**)_tmp269),((_tmp95A->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp26A,((_tmp95A->varmap=(struct Cyc_Dict_Dict)_tmp26D,((_tmp95A->toplevel=(int)
_tmp26C,_tmp95A)))))))))));}}}static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct
_RegionHandle*r,struct Cyc_Toc_Env*e);static struct Cyc_Toc_Env*Cyc_Toc_loop_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp270;struct
_dynforward_ptr**_tmp271;struct _dynforward_ptr**_tmp272;struct Cyc_Toc_FallthruInfo*
_tmp273;struct Cyc_Dict_Dict _tmp274;int _tmp275;struct Cyc_Toc_Env*_tmp26F=e;
_tmp270=*_tmp26F;_tmp271=_tmp270.break_lab;_tmp272=_tmp270.continue_lab;_tmp273=
_tmp270.fallthru_info;_tmp274=_tmp270.varmap;_tmp275=_tmp270.toplevel;{struct Cyc_Toc_Env*
_tmp95B;return(_tmp95B=_region_malloc(r,sizeof(*_tmp95B)),((_tmp95B->break_lab=(
struct _dynforward_ptr**)0,((_tmp95B->continue_lab=(struct _dynforward_ptr**)0,((
_tmp95B->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp273,((_tmp95B->varmap=(
struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp274),((_tmp95B->toplevel=(int)_tmp275,_tmp95B)))))))))));}}
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,
struct Cyc_Toc_Env*e,struct _dynforward_ptr*break_l,struct _dynforward_ptr*
fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env);
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,
struct Cyc_Toc_Env*e,struct _dynforward_ptr*break_l,struct _dynforward_ptr*
fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){
struct Cyc_List_List*fallthru_vars=0;for(0;fallthru_binders != 0;fallthru_binders=
fallthru_binders->tl){struct Cyc_List_List*_tmp95C;fallthru_vars=((_tmp95C=
_region_malloc(r,sizeof(*_tmp95C)),((_tmp95C->hd=((struct Cyc_Absyn_Vardecl*)
fallthru_binders->hd)->name,((_tmp95C->tl=fallthru_vars,_tmp95C))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
fallthru_vars);{struct Cyc_Toc_Env _tmp279;struct _dynforward_ptr**_tmp27A;struct
_dynforward_ptr**_tmp27B;struct Cyc_Toc_FallthruInfo*_tmp27C;struct Cyc_Dict_Dict
_tmp27D;int _tmp27E;struct Cyc_Toc_Env*_tmp278=e;_tmp279=*_tmp278;_tmp27A=_tmp279.break_lab;
_tmp27B=_tmp279.continue_lab;_tmp27C=_tmp279.fallthru_info;_tmp27D=_tmp279.varmap;
_tmp27E=_tmp279.toplevel;{struct Cyc_Toc_Env _tmp280;struct Cyc_Dict_Dict _tmp281;
struct Cyc_Toc_Env*_tmp27F=next_case_env;_tmp280=*_tmp27F;_tmp281=_tmp280.varmap;{
struct Cyc_Toc_FallthruInfo*_tmp95D;struct Cyc_Toc_FallthruInfo*fi=(_tmp95D=
_region_malloc(r,sizeof(*_tmp95D)),((_tmp95D->label=fallthru_l,((_tmp95D->binders=
fallthru_vars,((_tmp95D->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp281),_tmp95D)))))));
struct _dynforward_ptr**_tmp960;struct Cyc_Toc_Env*_tmp95F;return(_tmp95F=
_region_malloc(r,sizeof(*_tmp95F)),((_tmp95F->break_lab=(struct _dynforward_ptr**)((
_tmp960=_region_malloc(r,sizeof(*_tmp960)),((_tmp960[0]=break_l,_tmp960)))),((
_tmp95F->continue_lab=(struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp27B),((
_tmp95F->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmp95F->varmap=(struct
Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp27D),((_tmp95F->toplevel=(int)_tmp27E,_tmp95F)))))))))));}}}}
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,
struct Cyc_Toc_Env*e,struct _dynforward_ptr*break_l);static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dynforward_ptr*break_l){struct
Cyc_Toc_Env _tmp286;struct _dynforward_ptr**_tmp287;struct _dynforward_ptr**_tmp288;
struct Cyc_Toc_FallthruInfo*_tmp289;struct Cyc_Dict_Dict _tmp28A;int _tmp28B;struct
Cyc_Toc_Env*_tmp285=e;_tmp286=*_tmp285;_tmp287=_tmp286.break_lab;_tmp288=_tmp286.continue_lab;
_tmp289=_tmp286.fallthru_info;_tmp28A=_tmp286.varmap;_tmp28B=_tmp286.toplevel;{
struct _dynforward_ptr**_tmp963;struct Cyc_Toc_Env*_tmp962;return(_tmp962=
_region_malloc(r,sizeof(*_tmp962)),((_tmp962->break_lab=(struct _dynforward_ptr**)((
_tmp963=_region_malloc(r,sizeof(*_tmp963)),((_tmp963[0]=break_l,_tmp963)))),((
_tmp962->continue_lab=(struct _dynforward_ptr**)((struct _dynforward_ptr**)_tmp288),((
_tmp962->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmp962->varmap=(struct
Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp28A),((_tmp962->toplevel=(int)_tmp28B,_tmp962)))))))))));}}
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,
struct Cyc_Toc_Env*e,struct _dynforward_ptr*next_l);static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dynforward_ptr*next_l){struct
Cyc_Toc_Env _tmp28F;struct _dynforward_ptr**_tmp290;struct _dynforward_ptr**_tmp291;
struct Cyc_Toc_FallthruInfo*_tmp292;struct Cyc_Dict_Dict _tmp293;int _tmp294;struct
Cyc_Toc_Env*_tmp28E=e;_tmp28F=*_tmp28E;_tmp290=_tmp28F.break_lab;_tmp291=_tmp28F.continue_lab;
_tmp292=_tmp28F.fallthru_info;_tmp293=_tmp28F.varmap;_tmp294=_tmp28F.toplevel;{
struct Cyc_Toc_FallthruInfo*_tmp966;struct Cyc_Toc_Env*_tmp965;return(_tmp965=
_region_malloc(r,sizeof(*_tmp965)),((_tmp965->break_lab=(struct _dynforward_ptr**)
0,((_tmp965->continue_lab=(struct _dynforward_ptr**)((struct _dynforward_ptr**)
_tmp291),((_tmp965->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((_tmp966=
_region_malloc(r,sizeof(*_tmp966)),((_tmp966->label=next_l,((_tmp966->binders=0,((
_tmp966->next_case_env=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(
struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),_tmp966)))))))),((
_tmp965->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp293),((_tmp965->toplevel=(
int)_tmp294,_tmp965)))))))))));}}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Stmt*s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e);static
int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){void*_tmp297=(void*)e->annot;
_LLEB: if(*((void**)_tmp297)!= Cyc_CfFlowInfo_UnknownZ)goto _LLED;_LLEC: return Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLED: if(*((void**)_tmp297)
!= Cyc_CfFlowInfo_NotZero)goto _LLEF;_LLEE: return 0;_LLEF: if(_tmp297 != Cyc_CfFlowInfo_IsZero)
goto _LLF1;_LLF0:{const char*_tmp969;void*_tmp968;(_tmp968=0,Cyc_Tcutil_terr(e->loc,((
_tmp969="dereference of NULL pointer",_tag_dynforward(_tmp969,sizeof(char),
_get_zero_arr_size_char(_tmp969,28)))),_tag_dynforward(_tmp968,sizeof(void*),0)));}
return 0;_LLF1: if(_tmp297 != Cyc_Absyn_EmptyAnnot)goto _LLF3;_LLF2: return 0;_LLF3:
if(*((void**)_tmp297)!= Cyc_CfFlowInfo_HasTagCmps)goto _LLF5;_LLF4:{const char*
_tmp96C;void*_tmp96B;(_tmp96B=0,Cyc_Tcutil_warn(e->loc,((_tmp96C="compiler oddity: pointer compared to tag type",
_tag_dynforward(_tmp96C,sizeof(char),_get_zero_arr_size_char(_tmp96C,46)))),
_tag_dynforward(_tmp96B,sizeof(void*),0)));}return Cyc_Toc_is_nullable((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLF5:;_LLF6: {const char*_tmp96F;
void*_tmp96E;(_tmp96E=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr
ap))Cyc_Toc_toc_impos)(((_tmp96F="need_null_check",_tag_dynforward(_tmp96F,
sizeof(char),_get_zero_arr_size_char(_tmp96F,16)))),_tag_dynforward(_tmp96E,
sizeof(void*),0)));}_LLEA:;}static struct Cyc_List_List*Cyc_Toc_get_relns(struct
Cyc_Absyn_Exp*e);static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*
e){void*_tmp29E=(void*)e->annot;struct Cyc_List_List*_tmp29F;struct Cyc_List_List*
_tmp2A0;_LLF8: if(*((void**)_tmp29E)!= Cyc_CfFlowInfo_UnknownZ)goto _LLFA;_tmp29F=((
struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp29E)->f1;_LLF9: return _tmp29F;_LLFA: if(*((
void**)_tmp29E)!= Cyc_CfFlowInfo_NotZero)goto _LLFC;_tmp2A0=((struct Cyc_CfFlowInfo_NotZero_struct*)
_tmp29E)->f1;_LLFB: return _tmp2A0;_LLFC: if(_tmp29E != Cyc_CfFlowInfo_IsZero)goto
_LLFE;_LLFD:{const char*_tmp972;void*_tmp971;(_tmp971=0,Cyc_Tcutil_terr(e->loc,((
_tmp972="dereference of NULL pointer",_tag_dynforward(_tmp972,sizeof(char),
_get_zero_arr_size_char(_tmp972,28)))),_tag_dynforward(_tmp971,sizeof(void*),0)));}
return 0;_LLFE: if(*((void**)_tmp29E)!= Cyc_CfFlowInfo_HasTagCmps)goto _LL100;_LLFF:
goto _LL101;_LL100: if(_tmp29E != Cyc_Absyn_EmptyAnnot)goto _LL102;_LL101: return 0;
_LL102:;_LL103: {const char*_tmp975;void*_tmp974;(_tmp974=0,((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmp975="get_relns",
_tag_dynforward(_tmp975,sizeof(char),_get_zero_arr_size_char(_tmp975,10)))),
_tag_dynforward(_tmp974,sizeof(void*),0)));}_LLF7:;}static int Cyc_Toc_check_const_array(
unsigned int i,void*t);static int Cyc_Toc_check_const_array(unsigned int i,void*t){
void*_tmp2A5=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp2A6;struct Cyc_Absyn_PtrAtts
_tmp2A7;struct Cyc_Absyn_Conref*_tmp2A8;struct Cyc_Absyn_Conref*_tmp2A9;struct Cyc_Absyn_ArrayInfo
_tmp2AA;struct Cyc_Absyn_Exp*_tmp2AB;_LL105: if(_tmp2A5 <= (void*)4)goto _LL109;if(*((
int*)_tmp2A5)!= 4)goto _LL107;_tmp2A6=((struct Cyc_Absyn_PointerType_struct*)
_tmp2A5)->f1;_tmp2A7=_tmp2A6.ptr_atts;_tmp2A8=_tmp2A7.bounds;_tmp2A9=_tmp2A7.zero_term;
_LL106: {void*_tmp2AC=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp2A8);struct
Cyc_Absyn_Exp*_tmp2AD;_LL10C: if((int)_tmp2AC != 0)goto _LL10E;_LL10D: return 0;
_LL10E: if((int)_tmp2AC != 1)goto _LL110;_LL10F: return 0;_LL110: if(_tmp2AC <= (void*)
2)goto _LL112;if(*((int*)_tmp2AC)!= 0)goto _LL112;_tmp2AD=((struct Cyc_Absyn_Upper_b_struct*)
_tmp2AC)->f1;_LL111: {unsigned int _tmp2AF;int _tmp2B0;struct _tuple5 _tmp2AE=Cyc_Evexp_eval_const_uint_exp(
_tmp2AD);_tmp2AF=_tmp2AE.f1;_tmp2B0=_tmp2AE.f2;return _tmp2B0  && i <= _tmp2AF;}
_LL112: if(_tmp2AC <= (void*)2)goto _LL10B;if(*((int*)_tmp2AC)!= 1)goto _LL10B;
_LL113: {const char*_tmp978;void*_tmp977;(_tmp977=0,((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(((_tmp978="check_const_array: AbsUpper_b",
_tag_dynforward(_tmp978,sizeof(char),_get_zero_arr_size_char(_tmp978,30)))),
_tag_dynforward(_tmp977,sizeof(void*),0)));}_LL10B:;}_LL107: if(*((int*)_tmp2A5)
!= 7)goto _LL109;_tmp2AA=((struct Cyc_Absyn_ArrayType_struct*)_tmp2A5)->f1;_tmp2AB=
_tmp2AA.num_elts;_LL108: if(_tmp2AB == 0)return 0;{unsigned int _tmp2B4;int _tmp2B5;
struct _tuple5 _tmp2B3=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_tmp2AB);
_tmp2B4=_tmp2B3.f1;_tmp2B5=_tmp2B3.f2;return _tmp2B5  && i <= _tmp2B4;}_LL109:;
_LL10A: return 0;_LL104:;}static int Cyc_Toc_check_leq_size_var(struct Cyc_List_List*
relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y);static int Cyc_Toc_check_leq_size_var(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){
for(0;relns != 0;relns=relns->tl){struct Cyc_CfFlowInfo_Reln*_tmp2B6=(struct Cyc_CfFlowInfo_Reln*)
relns->hd;if(_tmp2B6->vd != y)continue;{union Cyc_CfFlowInfo_RelnOp_union _tmp2B7=
_tmp2B6->rop;struct Cyc_Absyn_Vardecl*_tmp2B8;struct Cyc_Absyn_Vardecl*_tmp2B9;
_LL115: if((_tmp2B7.LessSize).tag != 2)goto _LL117;_tmp2B8=(_tmp2B7.LessSize).f1;
_LL116: _tmp2B9=_tmp2B8;goto _LL118;_LL117: if((_tmp2B7.LessEqSize).tag != 4)goto
_LL119;_tmp2B9=(_tmp2B7.LessEqSize).f1;_LL118: if(_tmp2B9 == v)return 1;else{goto
_LL114;}_LL119:;_LL11A: continue;_LL114:;}}return 0;}static int Cyc_Toc_check_leq_size(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e);
static int Cyc_Toc_check_leq_size(struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*
v,struct Cyc_Absyn_Exp*e){{void*_tmp2BA=(void*)e->r;void*_tmp2BB;struct Cyc_Absyn_Vardecl*
_tmp2BC;void*_tmp2BD;struct Cyc_Absyn_Vardecl*_tmp2BE;void*_tmp2BF;struct Cyc_Absyn_Vardecl*
_tmp2C0;void*_tmp2C1;struct Cyc_Absyn_Vardecl*_tmp2C2;void*_tmp2C3;struct Cyc_List_List*
_tmp2C4;struct Cyc_List_List _tmp2C5;struct Cyc_Absyn_Exp*_tmp2C6;_LL11C: if(*((int*)
_tmp2BA)!= 1)goto _LL11E;_tmp2BB=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2BA)->f2;
if(_tmp2BB <= (void*)1)goto _LL11E;if(*((int*)_tmp2BB)!= 4)goto _LL11E;_tmp2BC=((
struct Cyc_Absyn_Pat_b_struct*)_tmp2BB)->f1;_LL11D: _tmp2BE=_tmp2BC;goto _LL11F;
_LL11E: if(*((int*)_tmp2BA)!= 1)goto _LL120;_tmp2BD=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2BA)->f2;if(_tmp2BD <= (void*)1)goto _LL120;if(*((int*)_tmp2BD)!= 3)goto _LL120;
_tmp2BE=((struct Cyc_Absyn_Local_b_struct*)_tmp2BD)->f1;_LL11F: _tmp2C0=_tmp2BE;
goto _LL121;_LL120: if(*((int*)_tmp2BA)!= 1)goto _LL122;_tmp2BF=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2BA)->f2;if(_tmp2BF <= (void*)1)goto _LL122;if(*((int*)_tmp2BF)!= 0)goto _LL122;
_tmp2C0=((struct Cyc_Absyn_Global_b_struct*)_tmp2BF)->f1;_LL121: _tmp2C2=_tmp2C0;
goto _LL123;_LL122: if(*((int*)_tmp2BA)!= 1)goto _LL124;_tmp2C1=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2BA)->f2;if(_tmp2C1 <= (void*)1)goto _LL124;if(*((int*)_tmp2C1)!= 2)goto _LL124;
_tmp2C2=((struct Cyc_Absyn_Param_b_struct*)_tmp2C1)->f1;_LL123: if(_tmp2C2->escapes)
return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp2C2))return 1;goto _LL11B;_LL124:
if(*((int*)_tmp2BA)!= 3)goto _LL126;_tmp2C3=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp2BA)->f1;if((int)_tmp2C3 != 19)goto _LL126;_tmp2C4=((struct Cyc_Absyn_Primop_e_struct*)
_tmp2BA)->f2;if(_tmp2C4 == 0)goto _LL126;_tmp2C5=*_tmp2C4;_tmp2C6=(struct Cyc_Absyn_Exp*)
_tmp2C5.hd;_LL125:{void*_tmp2C7=(void*)_tmp2C6->r;void*_tmp2C8;struct Cyc_Absyn_Vardecl*
_tmp2C9;void*_tmp2CA;struct Cyc_Absyn_Vardecl*_tmp2CB;void*_tmp2CC;struct Cyc_Absyn_Vardecl*
_tmp2CD;void*_tmp2CE;struct Cyc_Absyn_Vardecl*_tmp2CF;_LL129: if(*((int*)_tmp2C7)
!= 1)goto _LL12B;_tmp2C8=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2C7)->f2;if(
_tmp2C8 <= (void*)1)goto _LL12B;if(*((int*)_tmp2C8)!= 4)goto _LL12B;_tmp2C9=((
struct Cyc_Absyn_Pat_b_struct*)_tmp2C8)->f1;_LL12A: _tmp2CB=_tmp2C9;goto _LL12C;
_LL12B: if(*((int*)_tmp2C7)!= 1)goto _LL12D;_tmp2CA=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2C7)->f2;if(_tmp2CA <= (void*)1)goto _LL12D;if(*((int*)_tmp2CA)!= 3)goto _LL12D;
_tmp2CB=((struct Cyc_Absyn_Local_b_struct*)_tmp2CA)->f1;_LL12C: _tmp2CD=_tmp2CB;
goto _LL12E;_LL12D: if(*((int*)_tmp2C7)!= 1)goto _LL12F;_tmp2CC=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2C7)->f2;if(_tmp2CC <= (void*)1)goto _LL12F;if(*((int*)_tmp2CC)!= 0)goto _LL12F;
_tmp2CD=((struct Cyc_Absyn_Global_b_struct*)_tmp2CC)->f1;_LL12E: _tmp2CF=_tmp2CD;
goto _LL130;_LL12F: if(*((int*)_tmp2C7)!= 1)goto _LL131;_tmp2CE=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2C7)->f2;if(_tmp2CE <= (void*)1)goto _LL131;if(*((int*)_tmp2CE)!= 2)goto _LL131;
_tmp2CF=((struct Cyc_Absyn_Param_b_struct*)_tmp2CE)->f1;_LL130: return _tmp2CF == v;
_LL131:;_LL132: goto _LL128;_LL128:;}goto _LL11B;_LL126:;_LL127: goto _LL11B;_LL11B:;}
return 0;}static int Cyc_Toc_check_bounds(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*
a,struct Cyc_Absyn_Exp*i);static int Cyc_Toc_check_bounds(struct Cyc_List_List*relns,
struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){{void*_tmp2D0=(void*)a->r;void*
_tmp2D1;struct Cyc_Absyn_Vardecl*_tmp2D2;void*_tmp2D3;struct Cyc_Absyn_Vardecl*
_tmp2D4;void*_tmp2D5;struct Cyc_Absyn_Vardecl*_tmp2D6;void*_tmp2D7;struct Cyc_Absyn_Vardecl*
_tmp2D8;_LL134: if(*((int*)_tmp2D0)!= 1)goto _LL136;_tmp2D1=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2D0)->f2;if(_tmp2D1 <= (void*)1)goto _LL136;if(*((int*)_tmp2D1)!= 4)goto _LL136;
_tmp2D2=((struct Cyc_Absyn_Pat_b_struct*)_tmp2D1)->f1;_LL135: _tmp2D4=_tmp2D2;goto
_LL137;_LL136: if(*((int*)_tmp2D0)!= 1)goto _LL138;_tmp2D3=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2D0)->f2;if(_tmp2D3 <= (void*)1)goto _LL138;if(*((int*)_tmp2D3)!= 3)goto _LL138;
_tmp2D4=((struct Cyc_Absyn_Local_b_struct*)_tmp2D3)->f1;_LL137: _tmp2D6=_tmp2D4;
goto _LL139;_LL138: if(*((int*)_tmp2D0)!= 1)goto _LL13A;_tmp2D5=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2D0)->f2;if(_tmp2D5 <= (void*)1)goto _LL13A;if(*((int*)_tmp2D5)!= 0)goto _LL13A;
_tmp2D6=((struct Cyc_Absyn_Global_b_struct*)_tmp2D5)->f1;_LL139: _tmp2D8=_tmp2D6;
goto _LL13B;_LL13A: if(*((int*)_tmp2D0)!= 1)goto _LL13C;_tmp2D7=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2D0)->f2;if(_tmp2D7 <= (void*)1)goto _LL13C;if(*((int*)_tmp2D7)!= 2)goto _LL13C;
_tmp2D8=((struct Cyc_Absyn_Param_b_struct*)_tmp2D7)->f1;_LL13B: if(_tmp2D8->escapes)
return 0;inner_loop: {void*_tmp2D9=(void*)i->r;void*_tmp2DA;struct Cyc_Absyn_Exp*
_tmp2DB;union Cyc_Absyn_Cnst_union _tmp2DC;void*_tmp2DD;int _tmp2DE;union Cyc_Absyn_Cnst_union
_tmp2DF;void*_tmp2E0;int _tmp2E1;union Cyc_Absyn_Cnst_union _tmp2E2;void*_tmp2E3;
int _tmp2E4;void*_tmp2E5;struct Cyc_List_List*_tmp2E6;struct Cyc_List_List _tmp2E7;
struct Cyc_Absyn_Exp*_tmp2E8;struct Cyc_List_List*_tmp2E9;struct Cyc_List_List
_tmp2EA;struct Cyc_Absyn_Exp*_tmp2EB;void*_tmp2EC;struct Cyc_Absyn_Vardecl*_tmp2ED;
void*_tmp2EE;struct Cyc_Absyn_Vardecl*_tmp2EF;void*_tmp2F0;struct Cyc_Absyn_Vardecl*
_tmp2F1;void*_tmp2F2;struct Cyc_Absyn_Vardecl*_tmp2F3;_LL13F: if(*((int*)_tmp2D9)
!= 15)goto _LL141;_tmp2DA=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp2D9)->f1;
_tmp2DB=((struct Cyc_Absyn_Cast_e_struct*)_tmp2D9)->f2;_LL140: i=_tmp2DB;goto
inner_loop;_LL141: if(*((int*)_tmp2D9)!= 0)goto _LL143;_tmp2DC=((struct Cyc_Absyn_Const_e_struct*)
_tmp2D9)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp2D9)->f1).Int_c).tag != 2)
goto _LL143;_tmp2DD=(_tmp2DC.Int_c).f1;if((int)_tmp2DD != 2)goto _LL143;_tmp2DE=(
_tmp2DC.Int_c).f2;_LL142: _tmp2E1=_tmp2DE;goto _LL144;_LL143: if(*((int*)_tmp2D9)!= 
0)goto _LL145;_tmp2DF=((struct Cyc_Absyn_Const_e_struct*)_tmp2D9)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmp2D9)->f1).Int_c).tag != 2)goto _LL145;_tmp2E0=(
_tmp2DF.Int_c).f1;if((int)_tmp2E0 != 0)goto _LL145;_tmp2E1=(_tmp2DF.Int_c).f2;
_LL144: return _tmp2E1 >= 0  && Cyc_Toc_check_const_array((unsigned int)(_tmp2E1 + 1),(
void*)_tmp2D8->type);_LL145: if(*((int*)_tmp2D9)!= 0)goto _LL147;_tmp2E2=((struct
Cyc_Absyn_Const_e_struct*)_tmp2D9)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp2D9)->f1).Int_c).tag != 2)goto _LL147;_tmp2E3=(_tmp2E2.Int_c).f1;if((int)
_tmp2E3 != 1)goto _LL147;_tmp2E4=(_tmp2E2.Int_c).f2;_LL146: return Cyc_Toc_check_const_array((
unsigned int)(_tmp2E4 + 1),(void*)_tmp2D8->type);_LL147: if(*((int*)_tmp2D9)!= 3)
goto _LL149;_tmp2E5=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp2D9)->f1;if((
int)_tmp2E5 != 4)goto _LL149;_tmp2E6=((struct Cyc_Absyn_Primop_e_struct*)_tmp2D9)->f2;
if(_tmp2E6 == 0)goto _LL149;_tmp2E7=*_tmp2E6;_tmp2E8=(struct Cyc_Absyn_Exp*)_tmp2E7.hd;
_tmp2E9=_tmp2E7.tl;if(_tmp2E9 == 0)goto _LL149;_tmp2EA=*_tmp2E9;_tmp2EB=(struct Cyc_Absyn_Exp*)
_tmp2EA.hd;_LL148: return Cyc_Toc_check_leq_size(relns,_tmp2D8,_tmp2EB);_LL149: if(*((
int*)_tmp2D9)!= 1)goto _LL14B;_tmp2EC=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2D9)->f2;if(_tmp2EC <= (void*)1)goto _LL14B;if(*((int*)_tmp2EC)!= 4)goto _LL14B;
_tmp2ED=((struct Cyc_Absyn_Pat_b_struct*)_tmp2EC)->f1;_LL14A: _tmp2EF=_tmp2ED;goto
_LL14C;_LL14B: if(*((int*)_tmp2D9)!= 1)goto _LL14D;_tmp2EE=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2D9)->f2;if(_tmp2EE <= (void*)1)goto _LL14D;if(*((int*)_tmp2EE)!= 3)goto _LL14D;
_tmp2EF=((struct Cyc_Absyn_Local_b_struct*)_tmp2EE)->f1;_LL14C: _tmp2F1=_tmp2EF;
goto _LL14E;_LL14D: if(*((int*)_tmp2D9)!= 1)goto _LL14F;_tmp2F0=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2D9)->f2;if(_tmp2F0 <= (void*)1)goto _LL14F;if(*((int*)_tmp2F0)!= 0)goto _LL14F;
_tmp2F1=((struct Cyc_Absyn_Global_b_struct*)_tmp2F0)->f1;_LL14E: _tmp2F3=_tmp2F1;
goto _LL150;_LL14F: if(*((int*)_tmp2D9)!= 1)goto _LL151;_tmp2F2=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2D9)->f2;if(_tmp2F2 <= (void*)1)goto _LL151;if(*((int*)_tmp2F2)!= 2)goto _LL151;
_tmp2F3=((struct Cyc_Absyn_Param_b_struct*)_tmp2F2)->f1;_LL150: if(_tmp2F3->escapes)
return 0;{struct Cyc_List_List*_tmp2F4=relns;for(0;_tmp2F4 != 0;_tmp2F4=_tmp2F4->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2F5=(struct Cyc_CfFlowInfo_Reln*)_tmp2F4->hd;if(
_tmp2F5->vd == _tmp2F3){union Cyc_CfFlowInfo_RelnOp_union _tmp2F6=_tmp2F5->rop;
struct Cyc_Absyn_Vardecl*_tmp2F7;struct Cyc_Absyn_Vardecl*_tmp2F8;unsigned int
_tmp2F9;_LL154: if((_tmp2F6.LessSize).tag != 2)goto _LL156;_tmp2F7=(_tmp2F6.LessSize).f1;
_LL155: if(_tmp2D8 == _tmp2F7)return 1;else{goto _LL153;}_LL156: if((_tmp2F6.LessVar).tag
!= 1)goto _LL158;_tmp2F8=(_tmp2F6.LessVar).f1;_LL157:{struct Cyc_List_List*_tmp2FA=
relns;for(0;_tmp2FA != 0;_tmp2FA=_tmp2FA->tl){struct Cyc_CfFlowInfo_Reln*_tmp2FB=(
struct Cyc_CfFlowInfo_Reln*)_tmp2FA->hd;if(_tmp2FB->vd == _tmp2F8){union Cyc_CfFlowInfo_RelnOp_union
_tmp2FC=_tmp2FB->rop;struct Cyc_Absyn_Vardecl*_tmp2FD;struct Cyc_Absyn_Vardecl*
_tmp2FE;unsigned int _tmp2FF;struct Cyc_Absyn_Vardecl*_tmp300;_LL15D: if((_tmp2FC.LessEqSize).tag
!= 4)goto _LL15F;_tmp2FD=(_tmp2FC.LessEqSize).f1;_LL15E: _tmp2FE=_tmp2FD;goto
_LL160;_LL15F: if((_tmp2FC.LessSize).tag != 2)goto _LL161;_tmp2FE=(_tmp2FC.LessSize).f1;
_LL160: if(_tmp2D8 == _tmp2FE)return 1;goto _LL15C;_LL161: if((_tmp2FC.EqualConst).tag
!= 0)goto _LL163;_tmp2FF=(_tmp2FC.EqualConst).f1;_LL162: return Cyc_Toc_check_const_array(
_tmp2FF,(void*)_tmp2D8->type);_LL163: if((_tmp2FC.LessVar).tag != 1)goto _LL165;
_tmp300=(_tmp2FC.LessVar).f1;_LL164: if(Cyc_Toc_check_leq_size_var(relns,_tmp2D8,
_tmp300))return 1;goto _LL15C;_LL165:;_LL166: goto _LL15C;_LL15C:;}}}goto _LL153;
_LL158: if((_tmp2F6.LessConst).tag != 3)goto _LL15A;_tmp2F9=(_tmp2F6.LessConst).f1;
_LL159: return Cyc_Toc_check_const_array(_tmp2F9,(void*)_tmp2D8->type);_LL15A:;
_LL15B: goto _LL153;_LL153:;}}}goto _LL13E;_LL151:;_LL152: goto _LL13E;_LL13E:;}goto
_LL133;_LL13C:;_LL13D: goto _LL133;_LL133:;}return 0;}static void*Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){if(e->topt
== 0){const char*_tmp97B;void*_tmp97A;(_tmp97A=0,((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmp97B="Missing type in primop ",
_tag_dynforward(_tmp97B,sizeof(char),_get_zero_arr_size_char(_tmp97B,24)))),
_tag_dynforward(_tmp97A,sizeof(void*),0)));}return Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);}static void*Cyc_Toc_get_cyc_typ(
struct Cyc_Absyn_Exp*e);static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){if(
e->topt == 0){const char*_tmp97E;void*_tmp97D;(_tmp97D=0,((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmp97E="Missing type in primop ",
_tag_dynforward(_tmp97E,sizeof(char),_get_zero_arr_size_char(_tmp97E,24)))),
_tag_dynforward(_tmp97D,sizeof(void*),0)));}return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;}static struct _tuple4*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*
e);static struct _tuple4*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){struct _tuple4*
_tmp97F;return(_tmp97F=_cycalloc(sizeof(*_tmp97F)),((_tmp97F->f1=Cyc_Toc_mt_tq,((
_tmp97F->f2=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),
_tmp97F)))));}static struct _tuple8*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*e);static struct _tuple8*Cyc_Toc_add_designator(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(nv,e);{struct _tuple8*_tmp980;return(
_tmp980=_cycalloc(sizeof(*_tmp980)),((_tmp980->f1=0,((_tmp980->f2=e,_tmp980)))));}}
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct _tuple1*
x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,
int is_atomic);static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,
struct _tuple1*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*
rgnopt,int is_atomic){struct Cyc_Absyn_Exp*eo;void*t;if(pointer){t=Cyc_Absyn_cstar_typ(
struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*_tmp307=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(_tmp307): Cyc_Toc_malloc_ptr(
_tmp307));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)rgnopt;Cyc_Toc_exp_to_c(
nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,_tmp307);}}}else{t=
struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,eo,s,0),0);}
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*
dles0,struct Cyc_Absyn_Stmt*s){int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
dles0)- 1;{struct Cyc_List_List*_tmp308=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(dles0);for(0;_tmp308 != 0;_tmp308=_tmp308->tl){struct _tuple8
_tmp30A;struct Cyc_List_List*_tmp30B;struct Cyc_Absyn_Exp*_tmp30C;struct _tuple8*
_tmp309=(struct _tuple8*)_tmp308->hd;_tmp30A=*_tmp309;_tmp30B=_tmp30A.f1;_tmp30C=
_tmp30A.f2;{struct Cyc_Absyn_Exp*e_index;if(_tmp30B == 0)e_index=Cyc_Absyn_signed_int_exp(
count --,0);else{if(_tmp30B->tl != 0){const char*_tmp983;void*_tmp982;(_tmp982=0,((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(((
_tmp983="multiple designators in array",_tag_dynforward(_tmp983,sizeof(char),
_get_zero_arr_size_char(_tmp983,30)))),_tag_dynforward(_tmp982,sizeof(void*),0)));}{
void*_tmp30F=(void*)_tmp30B->hd;void*_tmp310=_tmp30F;struct Cyc_Absyn_Exp*_tmp311;
_LL168: if(*((int*)_tmp310)!= 0)goto _LL16A;_tmp311=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp310)->f1;_LL169: Cyc_Toc_exp_to_c(nv,_tmp311);e_index=_tmp311;goto _LL167;
_LL16A: if(*((int*)_tmp310)!= 1)goto _LL167;_LL16B: {const char*_tmp986;void*
_tmp985;(_tmp985=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_unimp)(((_tmp986="field name designators in array",_tag_dynforward(
_tmp986,sizeof(char),_get_zero_arr_size_char(_tmp986,32)))),_tag_dynforward(
_tmp985,sizeof(void*),0)));}_LL167:;}}{struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(
lhs,e_index,0);void*_tmp314=(void*)_tmp30C->r;struct Cyc_List_List*_tmp315;struct
Cyc_Absyn_Vardecl*_tmp316;struct Cyc_Absyn_Exp*_tmp317;struct Cyc_Absyn_Exp*
_tmp318;int _tmp319;void*_tmp31A;struct Cyc_List_List*_tmp31B;_LL16D: if(*((int*)
_tmp314)!= 28)goto _LL16F;_tmp315=((struct Cyc_Absyn_Array_e_struct*)_tmp314)->f1;
_LL16E: s=Cyc_Toc_init_array(nv,lval,_tmp315,s);goto _LL16C;_LL16F: if(*((int*)
_tmp314)!= 29)goto _LL171;_tmp316=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp314)->f1;_tmp317=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp314)->f2;
_tmp318=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp314)->f3;_tmp319=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp314)->f4;_LL170: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp316,_tmp317,_tmp318,_tmp319,s,0);goto _LL16C;_LL171: if(*((int*)
_tmp314)!= 31)goto _LL173;_tmp31A=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp314)->f1;_tmp31B=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp314)->f2;_LL172:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp31A,_tmp31B,s);goto _LL16C;_LL173:;_LL174:
Cyc_Toc_exp_to_c(nv,_tmp30C);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
lhs,e_index,0),_tmp30C,0),s,0);goto _LL16C;_LL16C:;}}}}return s;}static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct
Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,
struct Cyc_Absyn_Stmt*s,int e1_already_translated);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct
Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int
e1_already_translated){struct _tuple1*_tmp31C=vd->name;void*_tmp31D=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);{struct _RegionHandle _tmp31E=_new_region("r2");struct
_RegionHandle*r2=& _tmp31E;_push_region(r2);{struct Cyc_Absyn_Local_b_struct
_tmp989;struct Cyc_Absyn_Local_b_struct*_tmp988;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(
r2,nv,_tmp31C,Cyc_Absyn_varb_exp(_tmp31C,(void*)((_tmp988=_cycalloc(sizeof(*
_tmp988)),((_tmp988[0]=((_tmp989.tag=3,((_tmp989.f1=vd,_tmp989)))),_tmp988)))),0));
struct _tuple1*max=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp(_tmp31C,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*
eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp31C,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp31C,0),0);
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp31C,0),
0);struct Cyc_Absyn_Stmt*body;{void*_tmp31F=(void*)e2->r;struct Cyc_List_List*
_tmp320;struct Cyc_Absyn_Vardecl*_tmp321;struct Cyc_Absyn_Exp*_tmp322;struct Cyc_Absyn_Exp*
_tmp323;int _tmp324;void*_tmp325;struct Cyc_List_List*_tmp326;_LL176: if(*((int*)
_tmp31F)!= 28)goto _LL178;_tmp320=((struct Cyc_Absyn_Array_e_struct*)_tmp31F)->f1;
_LL177: body=Cyc_Toc_init_array(nv2,lval,_tmp320,Cyc_Toc_skip_stmt_dl());goto
_LL175;_LL178: if(*((int*)_tmp31F)!= 29)goto _LL17A;_tmp321=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp31F)->f1;_tmp322=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp31F)->f2;
_tmp323=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp31F)->f3;_tmp324=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp31F)->f4;_LL179: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp321,_tmp322,_tmp323,_tmp324,Cyc_Toc_skip_stmt_dl(),0);goto _LL175;
_LL17A: if(*((int*)_tmp31F)!= 31)goto _LL17C;_tmp325=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp31F)->f1;_tmp326=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp31F)->f2;_LL17B:
body=Cyc_Toc_init_anon_struct(nv,lval,_tmp325,_tmp326,Cyc_Toc_skip_stmt_dl());
goto _LL175;_LL17C:;_LL17D: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL175;_LL175:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp((void*)lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(
_tmp31D,Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(
ex,0),0);}{struct Cyc_Absyn_Stmt*_tmp327=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(
max,Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp31C,
Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp327;}}};
_pop_region(r2);}}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp32A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp32A != 0;_tmp32A=_tmp32A->tl){struct _tuple8 _tmp32C;
struct Cyc_List_List*_tmp32D;struct Cyc_Absyn_Exp*_tmp32E;struct _tuple8*_tmp32B=(
struct _tuple8*)_tmp32A->hd;_tmp32C=*_tmp32B;_tmp32D=_tmp32C.f1;_tmp32E=_tmp32C.f2;
if(_tmp32D == 0){const char*_tmp98C;void*_tmp98B;(_tmp98B=0,((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmp98C="empty designator list",
_tag_dynforward(_tmp98C,sizeof(char),_get_zero_arr_size_char(_tmp98C,22)))),
_tag_dynforward(_tmp98B,sizeof(void*),0)));}if(_tmp32D->tl != 0){const char*
_tmp98F;void*_tmp98E;(_tmp98E=0,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmp98F="too many designators in anonymous struct",
_tag_dynforward(_tmp98F,sizeof(char),_get_zero_arr_size_char(_tmp98F,41)))),
_tag_dynforward(_tmp98E,sizeof(void*),0)));}{void*_tmp333=(void*)_tmp32D->hd;
struct _dynforward_ptr*_tmp334;_LL17F: if(*((int*)_tmp333)!= 1)goto _LL181;_tmp334=((
struct Cyc_Absyn_FieldName_struct*)_tmp333)->f1;_LL180: {struct Cyc_Absyn_Exp*lval=
Cyc_Absyn_aggrmember_exp(lhs,_tmp334,0);{void*_tmp335=(void*)_tmp32E->r;struct
Cyc_List_List*_tmp336;struct Cyc_Absyn_Vardecl*_tmp337;struct Cyc_Absyn_Exp*
_tmp338;struct Cyc_Absyn_Exp*_tmp339;int _tmp33A;void*_tmp33B;struct Cyc_List_List*
_tmp33C;_LL184: if(*((int*)_tmp335)!= 28)goto _LL186;_tmp336=((struct Cyc_Absyn_Array_e_struct*)
_tmp335)->f1;_LL185: s=Cyc_Toc_init_array(nv,lval,_tmp336,s);goto _LL183;_LL186:
if(*((int*)_tmp335)!= 29)goto _LL188;_tmp337=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp335)->f1;_tmp338=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp335)->f2;
_tmp339=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp335)->f3;_tmp33A=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp335)->f4;_LL187: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp337,_tmp338,_tmp339,_tmp33A,s,0);goto _LL183;_LL188: if(*((int*)
_tmp335)!= 31)goto _LL18A;_tmp33B=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp335)->f1;_tmp33C=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp335)->f2;_LL189:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp33B,_tmp33C,s);goto _LL183;_LL18A:;_LL18B:
Cyc_Toc_exp_to_c(nv,_tmp32E);if(Cyc_Toc_is_poly_field(struct_type,_tmp334))
_tmp32E=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp32E);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp32E,0),0),s,0);goto _LL183;
_LL183:;}goto _LL17E;}_LL181:;_LL182: {const char*_tmp992;void*_tmp991;(_tmp991=0,((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((
_tmp992="array designator in struct",_tag_dynforward(_tmp992,sizeof(char),
_get_zero_arr_size_char(_tmp992,27)))),_tag_dynforward(_tmp991,sizeof(void*),0)));}
_LL17E:;}}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*
nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es);static struct Cyc_Absyn_Exp*
Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,
struct Cyc_List_List*es){struct _RegionHandle _tmp33F=_new_region("r");struct
_RegionHandle*r=& _tmp33F;_push_region(r);{struct Cyc_List_List*_tmp340=((struct
Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);void*_tmp341=Cyc_Toc_add_tuple_type(
_tmp340);struct _tuple1*_tmp342=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp343=
Cyc_Absyn_var_exp(_tmp342,0);struct Cyc_Absyn_Stmt*_tmp344=Cyc_Absyn_exp_stmt(
_tmp343,0);struct Cyc_Absyn_Exp*(*_tmp345)(struct Cyc_Absyn_Exp*,struct
_dynforward_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;int is_atomic=1;struct Cyc_List_List*_tmp346=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);{int i=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp346);for(0;_tmp346 != 0;(_tmp346=
_tmp346->tl,-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp346->hd;struct
Cyc_Absyn_Exp*lval=_tmp345(_tmp343,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic
 && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{
void*_tmp347=(void*)e->r;struct Cyc_List_List*_tmp348;struct Cyc_Absyn_Vardecl*
_tmp349;struct Cyc_Absyn_Exp*_tmp34A;struct Cyc_Absyn_Exp*_tmp34B;int _tmp34C;
_LL18D: if(*((int*)_tmp347)!= 28)goto _LL18F;_tmp348=((struct Cyc_Absyn_Array_e_struct*)
_tmp347)->f1;_LL18E: _tmp344=Cyc_Toc_init_array(nv,lval,_tmp348,_tmp344);goto
_LL18C;_LL18F: if(*((int*)_tmp347)!= 29)goto _LL191;_tmp349=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp347)->f1;_tmp34A=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp347)->f2;
_tmp34B=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp347)->f3;_tmp34C=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp347)->f4;_LL190: _tmp344=Cyc_Toc_init_comprehension(
nv,lval,_tmp349,_tmp34A,_tmp34B,_tmp34C,_tmp344,0);goto _LL18C;_LL191:;_LL192: Cyc_Toc_exp_to_c(
nv,e);_tmp344=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp345(
_tmp343,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp344,0);goto _LL18C;_LL18C:;}}}{
struct Cyc_Absyn_Exp*_tmp34D=Cyc_Toc_make_struct(nv,_tmp342,_tmp341,_tmp344,
pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp34D;}};_pop_region(r);}
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*
struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*
dles,struct _tuple1*tdn);static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*
nv,void*struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct
Cyc_List_List*dles,struct _tuple1*tdn){struct _tuple1*_tmp34E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp34F=Cyc_Absyn_var_exp(_tmp34E,0);struct Cyc_Absyn_Stmt*
_tmp350=Cyc_Absyn_exp_stmt(_tmp34F,0);struct Cyc_Absyn_Exp*(*_tmp351)(struct Cyc_Absyn_Exp*,
struct _dynforward_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp:
Cyc_Absyn_aggrmember_exp;void*_tmp352=Cyc_Toc_aggrdecl_type(tdn,Cyc_Absyn_strctq);
int is_atomic=1;struct Cyc_Absyn_Aggrdecl*ad;{void*_tmp353=Cyc_Tcutil_compress(
struct_type);struct Cyc_Absyn_AggrInfo _tmp354;union Cyc_Absyn_AggrInfoU_union
_tmp355;_LL194: if(_tmp353 <= (void*)4)goto _LL196;if(*((int*)_tmp353)!= 10)goto
_LL196;_tmp354=((struct Cyc_Absyn_AggrType_struct*)_tmp353)->f1;_tmp355=_tmp354.aggr_info;
_LL195: ad=Cyc_Absyn_get_known_aggrdecl(_tmp355);goto _LL193;_LL196:;_LL197: {
const char*_tmp995;void*_tmp994;(_tmp994=0,((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmp995="init_struct: bad struct type",
_tag_dynforward(_tmp995,sizeof(char),_get_zero_arr_size_char(_tmp995,29)))),
_tag_dynforward(_tmp994,sizeof(void*),0)));}_LL193:;}{struct _RegionHandle _tmp358=
_new_region("r");struct _RegionHandle*r=& _tmp358;_push_region(r);{struct Cyc_List_List*
_tmp359=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))
Cyc_List_rrev)(r,dles);for(0;_tmp359 != 0;_tmp359=_tmp359->tl){struct _tuple8
_tmp35B;struct Cyc_List_List*_tmp35C;struct Cyc_Absyn_Exp*_tmp35D;struct _tuple8*
_tmp35A=(struct _tuple8*)_tmp359->hd;_tmp35B=*_tmp35A;_tmp35C=_tmp35B.f1;_tmp35D=
_tmp35B.f2;is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp35D->topt))->v);if(_tmp35C == 0){const char*_tmp998;void*_tmp997;(
_tmp997=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((
_tmp998="empty designator list",_tag_dynforward(_tmp998,sizeof(char),
_get_zero_arr_size_char(_tmp998,22)))),_tag_dynforward(_tmp997,sizeof(void*),0)));}
if(_tmp35C->tl != 0){struct _tuple1*_tmp360=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp361=Cyc_Absyn_var_exp(_tmp360,0);for(0;_tmp35C != 0;_tmp35C=_tmp35C->tl){void*
_tmp362=(void*)_tmp35C->hd;struct _dynforward_ptr*_tmp363;_LL199: if(*((int*)
_tmp362)!= 1)goto _LL19B;_tmp363=((struct Cyc_Absyn_FieldName_struct*)_tmp362)->f1;
_LL19A: if(Cyc_Toc_is_poly_field(struct_type,_tmp363))_tmp361=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp361);_tmp350=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp351(_tmp34F,_tmp363,0),_tmp361,0),0),_tmp350,0);goto _LL198;_LL19B:;_LL19C: {
const char*_tmp99B;void*_tmp99A;(_tmp99A=0,((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmp99B="array designator in struct",
_tag_dynforward(_tmp99B,sizeof(char),_get_zero_arr_size_char(_tmp99B,27)))),
_tag_dynforward(_tmp99A,sizeof(void*),0)));}_LL198:;}Cyc_Toc_exp_to_c(nv,_tmp35D);
_tmp350=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp361,
_tmp35D,0),0),_tmp350,0);}else{void*_tmp366=(void*)_tmp35C->hd;struct
_dynforward_ptr*_tmp367;_LL19E: if(*((int*)_tmp366)!= 1)goto _LL1A0;_tmp367=((
struct Cyc_Absyn_FieldName_struct*)_tmp366)->f1;_LL19F: {struct Cyc_Absyn_Exp*lval=
_tmp351(_tmp34F,_tmp367,0);{void*_tmp368=(void*)_tmp35D->r;struct Cyc_List_List*
_tmp369;struct Cyc_Absyn_Vardecl*_tmp36A;struct Cyc_Absyn_Exp*_tmp36B;struct Cyc_Absyn_Exp*
_tmp36C;int _tmp36D;void*_tmp36E;struct Cyc_List_List*_tmp36F;_LL1A3: if(*((int*)
_tmp368)!= 28)goto _LL1A5;_tmp369=((struct Cyc_Absyn_Array_e_struct*)_tmp368)->f1;
_LL1A4: _tmp350=Cyc_Toc_init_array(nv,lval,_tmp369,_tmp350);goto _LL1A2;_LL1A5: if(*((
int*)_tmp368)!= 29)goto _LL1A7;_tmp36A=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp368)->f1;_tmp36B=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp368)->f2;
_tmp36C=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp368)->f3;_tmp36D=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp368)->f4;_LL1A6: _tmp350=Cyc_Toc_init_comprehension(
nv,lval,_tmp36A,_tmp36B,_tmp36C,_tmp36D,_tmp350,0);goto _LL1A2;_LL1A7: if(*((int*)
_tmp368)!= 31)goto _LL1A9;_tmp36E=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp368)->f1;_tmp36F=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp368)->f2;_LL1A8:
_tmp350=Cyc_Toc_init_anon_struct(nv,lval,_tmp36E,_tmp36F,_tmp350);goto _LL1A2;
_LL1A9:;_LL1AA: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp35D->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp35D);{struct Cyc_Absyn_Aggrfield*
_tmp370=Cyc_Absyn_lookup_decl_field(ad,_tmp367);if(Cyc_Toc_is_poly_field(
struct_type,_tmp367) && !was_ptr_type)_tmp35D=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp35D);if(has_exists)_tmp35D=Cyc_Toc_cast_it((void*)((struct Cyc_Absyn_Aggrfield*)
_check_null(_tmp370))->type,_tmp35D);_tmp350=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(lval,_tmp35D,0),0),_tmp350,0);goto _LL1A2;}}_LL1A2:;}goto
_LL19D;}_LL1A0:;_LL1A1: {const char*_tmp99E;void*_tmp99D;(_tmp99D=0,((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmp99E="array designator in struct",
_tag_dynforward(_tmp99E,sizeof(char),_get_zero_arr_size_char(_tmp99E,27)))),
_tag_dynforward(_tmp99D,sizeof(void*),0)));}_LL19D:;}}}{struct Cyc_Absyn_Exp*
_tmp373=Cyc_Toc_make_struct(nv,_tmp34E,_tmp352,_tmp350,pointer,rgnopt,is_atomic);
_npop_handler(0);return _tmp373;};_pop_region(r);}}struct _tuple10{struct Cyc_Core_Opt*
f1;struct Cyc_Absyn_Exp*f2;};static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(
struct Cyc_Absyn_Exp*el,struct _tuple10*pr);static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(
struct Cyc_Absyn_Exp*el,struct _tuple10*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*
pr).f2,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*
e1,int ignore);static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*
e1,int ignore){return Cyc_Absyn_address_exp(e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp*e1,void*incr);static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(
struct Cyc_Absyn_Exp*e1,void*incr){struct Cyc_Absyn_Increment_e_struct _tmp9A1;
struct Cyc_Absyn_Increment_e_struct*_tmp9A0;return Cyc_Absyn_new_exp((void*)((
_tmp9A0=_cycalloc(sizeof(*_tmp9A0)),((_tmp9A0[0]=((_tmp9A1.tag=5,((_tmp9A1.f1=e1,((
_tmp9A1.f2=(void*)incr,_tmp9A1)))))),_tmp9A0)))),0);}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env);static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,
struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp376=(
void*)e1->r;struct Cyc_Absyn_Stmt*_tmp377;void*_tmp378;struct Cyc_Absyn_Exp*
_tmp379;struct Cyc_Absyn_Exp*_tmp37A;struct _dynforward_ptr*_tmp37B;_LL1AC: if(*((
int*)_tmp376)!= 38)goto _LL1AE;_tmp377=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp376)->f1;_LL1AD: Cyc_Toc_lvalue_assign_stmt(_tmp377,fs,f,f_env);goto _LL1AB;
_LL1AE: if(*((int*)_tmp376)!= 15)goto _LL1B0;_tmp378=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp376)->f1;_tmp379=((struct Cyc_Absyn_Cast_e_struct*)_tmp376)->f2;_LL1AF: Cyc_Toc_lvalue_assign(
_tmp379,fs,f,f_env);goto _LL1AB;_LL1B0: if(*((int*)_tmp376)!= 23)goto _LL1B2;
_tmp37A=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp376)->f1;_tmp37B=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp376)->f2;_LL1B1:(void*)(e1->r=(void*)((void*)_tmp37A->r));{struct Cyc_List_List*
_tmp9A2;Cyc_Toc_lvalue_assign(e1,(struct Cyc_List_List*)((_tmp9A2=_cycalloc(
sizeof(*_tmp9A2)),((_tmp9A2->hd=_tmp37B,((_tmp9A2->tl=fs,_tmp9A2)))))),f,f_env);}
goto _LL1AB;_LL1B2:;_LL1B3: {struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
for(0;fs != 0;fs=fs->tl){e1_copy=Cyc_Absyn_aggrmember_exp(e1_copy,(struct
_dynforward_ptr*)fs->hd,e1_copy->loc);}(void*)(e1->r=(void*)((void*)(f(e1_copy,
f_env))->r));goto _LL1AB;}_LL1AB:;}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*
s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),
void*f_env);static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct
Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
void*_tmp37D=(void*)s->r;struct Cyc_Absyn_Exp*_tmp37E;struct Cyc_Absyn_Decl*
_tmp37F;struct Cyc_Absyn_Stmt*_tmp380;struct Cyc_Absyn_Stmt*_tmp381;_LL1B5: if(
_tmp37D <= (void*)1)goto _LL1BB;if(*((int*)_tmp37D)!= 0)goto _LL1B7;_tmp37E=((
struct Cyc_Absyn_Exp_s_struct*)_tmp37D)->f1;_LL1B6: Cyc_Toc_lvalue_assign(_tmp37E,
fs,f,f_env);goto _LL1B4;_LL1B7: if(*((int*)_tmp37D)!= 11)goto _LL1B9;_tmp37F=((
struct Cyc_Absyn_Decl_s_struct*)_tmp37D)->f1;_tmp380=((struct Cyc_Absyn_Decl_s_struct*)
_tmp37D)->f2;_LL1B8: Cyc_Toc_lvalue_assign_stmt(_tmp380,fs,f,f_env);goto _LL1B4;
_LL1B9: if(*((int*)_tmp37D)!= 1)goto _LL1BB;_tmp381=((struct Cyc_Absyn_Seq_s_struct*)
_tmp37D)->f2;_LL1BA: Cyc_Toc_lvalue_assign_stmt(_tmp381,fs,f,f_env);goto _LL1B4;
_LL1BB:;_LL1BC: {const char*_tmp9A6;void*_tmp9A5[1];struct Cyc_String_pa_struct
_tmp9A4;(_tmp9A4.tag=0,((_tmp9A4.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_stmt2string(s)),((_tmp9A5[0]=& _tmp9A4,Cyc_Toc_toc_impos(((
_tmp9A6="lvalue_assign_stmt: %s",_tag_dynforward(_tmp9A6,sizeof(char),
_get_zero_arr_size_char(_tmp9A6,23)))),_tag_dynforward(_tmp9A5,sizeof(void*),1)))))));}
_LL1B4:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x);static struct Cyc_List_List*Cyc_Toc_rmap_2c(
struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
struct Cyc_List_List*result;struct Cyc_List_List*prev;if(x == 0)return 0;{struct Cyc_List_List*
_tmp9A7;result=((_tmp9A7=_region_malloc(r2,sizeof(*_tmp9A7)),((_tmp9A7->hd=(void*)
f((void*)x->hd,env),((_tmp9A7->tl=0,_tmp9A7))))));}prev=result;for(x=x->tl;x != 0;
x=x->tl){{struct Cyc_List_List*_tmp9A8;((struct Cyc_List_List*)_check_null(prev))->tl=((
_tmp9A8=_region_malloc(r2,sizeof(*_tmp9A8)),((_tmp9A8->hd=(void*)f((void*)x->hd,
env),((_tmp9A8->tl=0,_tmp9A8))))));}prev=((struct Cyc_List_List*)_check_null(prev))->tl;}
return result;}static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*
f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,
f,env,x);}static struct _tuple8*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e);static
struct _tuple8*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){struct _tuple8*_tmp9A9;
return(_tmp9A9=_cycalloc(sizeof(*_tmp9A9)),((_tmp9A9->f1=0,((_tmp9A9->f2=e,
_tmp9A9)))));}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t);static
struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){void*_tmp388=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp389;_LL1BE: if(_tmp388 <= (void*)4)goto _LL1C0;if(*((
int*)_tmp388)!= 4)goto _LL1C0;_tmp389=((struct Cyc_Absyn_PointerType_struct*)
_tmp388)->f1;_LL1BF: return _tmp389;_LL1C0:;_LL1C1: {const char*_tmp9AC;void*
_tmp9AB;(_tmp9AB=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(((_tmp9AC="get_ptr_typ: not a pointer!",_tag_dynforward(
_tmp9AC,sizeof(char),_get_zero_arr_size_char(_tmp9AC,28)))),_tag_dynforward(
_tmp9AB,sizeof(void*),0)));}_LL1BD:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(
void*t);static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){struct Cyc_Absyn_Exp*
res;{void*_tmp38C=Cyc_Tcutil_compress(t);void*_tmp38D;void*_tmp38E;void*_tmp38F;
void*_tmp390;void*_tmp391;void*_tmp392;void*_tmp393;void*_tmp394;void*_tmp395;
void*_tmp396;_LL1C3: if(_tmp38C <= (void*)4)goto _LL1D3;if(*((int*)_tmp38C)!= 4)
goto _LL1C5;_LL1C4: res=Cyc_Absyn_null_exp(0);goto _LL1C2;_LL1C5: if(*((int*)_tmp38C)
!= 5)goto _LL1C7;_tmp38D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp38C)->f1;
_tmp38E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp38C)->f2;if((int)_tmp38E != 
0)goto _LL1C7;_LL1C6:{union Cyc_Absyn_Cnst_union _tmp9AD;res=Cyc_Absyn_const_exp((
union Cyc_Absyn_Cnst_union)(((_tmp9AD.Char_c).tag=0,(((_tmp9AD.Char_c).f1=(void*)
_tmp38D,(((_tmp9AD.Char_c).f2='\000',_tmp9AD)))))),0);}goto _LL1C2;_LL1C7: if(*((
int*)_tmp38C)!= 5)goto _LL1C9;_tmp38F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp38C)->f1;_tmp390=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp38C)->f2;if((
int)_tmp390 != 1)goto _LL1C9;_LL1C8:{union Cyc_Absyn_Cnst_union _tmp9AE;res=Cyc_Absyn_const_exp((
union Cyc_Absyn_Cnst_union)(((_tmp9AE.Short_c).tag=1,(((_tmp9AE.Short_c).f1=(void*)
_tmp38F,(((_tmp9AE.Short_c).f2=0,_tmp9AE)))))),0);}goto _LL1C2;_LL1C9: if(*((int*)
_tmp38C)!= 12)goto _LL1CB;_LL1CA: goto _LL1CC;_LL1CB: if(*((int*)_tmp38C)!= 13)goto
_LL1CD;_LL1CC: _tmp391=(void*)1;goto _LL1CE;_LL1CD: if(*((int*)_tmp38C)!= 5)goto
_LL1CF;_tmp391=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp38C)->f1;_tmp392=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp38C)->f2;if((int)_tmp392 != 2)goto
_LL1CF;_LL1CE: _tmp393=_tmp391;goto _LL1D0;_LL1CF: if(*((int*)_tmp38C)!= 5)goto
_LL1D1;_tmp393=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp38C)->f1;_tmp394=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp38C)->f2;if((int)_tmp394 != 3)goto
_LL1D1;_LL1D0:{union Cyc_Absyn_Cnst_union _tmp9AF;res=Cyc_Absyn_const_exp((union
Cyc_Absyn_Cnst_union)(((_tmp9AF.Int_c).tag=2,(((_tmp9AF.Int_c).f1=(void*)_tmp393,(((
_tmp9AF.Int_c).f2=0,_tmp9AF)))))),0);}goto _LL1C2;_LL1D1: if(*((int*)_tmp38C)!= 5)
goto _LL1D3;_tmp395=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp38C)->f1;_tmp396=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp38C)->f2;if((int)_tmp396 != 4)goto
_LL1D3;_LL1D2:{union Cyc_Absyn_Cnst_union _tmp9B0;res=Cyc_Absyn_const_exp((union
Cyc_Absyn_Cnst_union)(((_tmp9B0.LongLong_c).tag=3,(((_tmp9B0.LongLong_c).f1=(
void*)_tmp395,(((_tmp9B0.LongLong_c).f2=(long long)0,_tmp9B0)))))),0);}goto
_LL1C2;_LL1D3: if((int)_tmp38C != 1)goto _LL1D5;_LL1D4: goto _LL1D6;_LL1D5: if(_tmp38C
<= (void*)4)goto _LL1D7;if(*((int*)_tmp38C)!= 6)goto _LL1D7;_LL1D6:{const char*
_tmp9B3;union Cyc_Absyn_Cnst_union _tmp9B2;res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)(((
_tmp9B2.Float_c).tag=4,(((_tmp9B2.Float_c).f1=((_tmp9B3="0.0",_tag_dynforward(
_tmp9B3,sizeof(char),_get_zero_arr_size_char(_tmp9B3,4)))),_tmp9B2)))),0);}goto
_LL1C2;_LL1D7:;_LL1D8: {const char*_tmp9B7;void*_tmp9B6[1];struct Cyc_String_pa_struct
_tmp9B5;(_tmp9B5.tag=0,((_tmp9B5.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_typ2string(t)),((_tmp9B6[0]=& _tmp9B5,((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmp9B7="found non-zero type %s in generate_zero",
_tag_dynforward(_tmp9B7,sizeof(char),_get_zero_arr_size_char(_tmp9B7,40)))),
_tag_dynforward(_tmp9B6,sizeof(void*),1)))))));}_LL1C2:;}{struct Cyc_Core_Opt*
_tmp9B8;res->topt=((_tmp9B8=_cycalloc(sizeof(*_tmp9B8)),((_tmp9B8->v=(void*)t,
_tmp9B8))));}return res;}static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct
Cyc_Absyn_Exp*e2,void*ptr_type,int is_dynforward,int is_dyneither,void*elt_type);
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*
ptr_type,int is_dynforward,int is_dyneither,void*elt_type){int is_fat=is_dynforward
 || is_dyneither;void*fat_ptr_type;if(is_dynforward)fat_ptr_type=Cyc_Absyn_dynforward_typ(
elt_type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);else{fat_ptr_type=Cyc_Absyn_dyneither_typ(
elt_type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);}{void*_tmp3A1=Cyc_Toc_typ_to_c(
elt_type);void*_tmp3A2=Cyc_Toc_typ_to_c(fat_ptr_type);void*_tmp3A3=Cyc_Absyn_cstar_typ(
_tmp3A1,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*_tmp9B9;struct Cyc_Core_Opt*_tmp3A4=(
_tmp9B9=_cycalloc(sizeof(*_tmp9B9)),((_tmp9B9->v=(void*)_tmp3A3,_tmp9B9)));
struct Cyc_Absyn_Exp*xinit;{void*_tmp3A5=(void*)e1->r;struct Cyc_Absyn_Exp*_tmp3A6;
struct Cyc_Absyn_Exp*_tmp3A7;struct Cyc_Absyn_Exp*_tmp3A8;_LL1DA: if(*((int*)
_tmp3A5)!= 22)goto _LL1DC;_tmp3A6=((struct Cyc_Absyn_Deref_e_struct*)_tmp3A5)->f1;
_LL1DB: if(!is_fat){_tmp3A6=Cyc_Toc_cast_it(fat_ptr_type,_tmp3A6);{struct Cyc_Core_Opt*
_tmp9BA;_tmp3A6->topt=((_tmp9BA=_cycalloc(sizeof(*_tmp9BA)),((_tmp9BA->v=(void*)
fat_ptr_type,_tmp9BA))));}}Cyc_Toc_exp_to_c(nv,_tmp3A6);xinit=_tmp3A6;goto _LL1D9;
_LL1DC: if(*((int*)_tmp3A5)!= 25)goto _LL1DE;_tmp3A7=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp3A5)->f1;_tmp3A8=((struct Cyc_Absyn_Subscript_e_struct*)_tmp3A5)->f2;_LL1DD:
if(!is_fat){_tmp3A7=Cyc_Toc_cast_it(fat_ptr_type,_tmp3A7);{struct Cyc_Core_Opt*
_tmp9BB;_tmp3A7->topt=((_tmp9BB=_cycalloc(sizeof(*_tmp9BB)),((_tmp9BB->v=(void*)
fat_ptr_type,_tmp9BB))));}}Cyc_Toc_exp_to_c(nv,_tmp3A7);Cyc_Toc_exp_to_c(nv,
_tmp3A8);if(is_dynforward){struct Cyc_Absyn_Exp*_tmp9BC[3];xinit=Cyc_Absyn_fncall_exp(
Cyc_Toc__dynforward_ptr_plus_e,((_tmp9BC[2]=_tmp3A8,((_tmp9BC[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0),((_tmp9BC[0]=_tmp3A7,((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp9BC,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);}else{struct Cyc_Absyn_Exp*_tmp9BD[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,((
_tmp9BD[2]=_tmp3A8,((_tmp9BD[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0),((_tmp9BD[0]=_tmp3A7,((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp9BD,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);}goto _LL1D9;_LL1DE:;_LL1DF: {const char*_tmp9C0;void*_tmp9BF;(
_tmp9BF=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((
_tmp9C0="found bad lhs for zero-terminated pointer assignment",_tag_dynforward(
_tmp9C0,sizeof(char),_get_zero_arr_size_char(_tmp9C0,53)))),_tag_dynforward(
_tmp9BF,sizeof(void*),0)));}_LL1D9:;}{struct _tuple1*_tmp3AF=Cyc_Toc_temp_var();
struct _RegionHandle _tmp3B0=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp3B0;
_push_region(rgn2);{struct Cyc_Toc_Env*_tmp3B1=Cyc_Toc_add_varmap(rgn2,nv,_tmp3AF,
Cyc_Absyn_var_exp(_tmp3AF,0));struct Cyc_Absyn_Vardecl*_tmp9C1;struct Cyc_Absyn_Vardecl*
_tmp3B2=(_tmp9C1=_cycalloc(sizeof(*_tmp9C1)),((_tmp9C1->sc=(void*)((void*)2),((
_tmp9C1->name=_tmp3AF,((_tmp9C1->tq=Cyc_Toc_mt_tq,((_tmp9C1->type=(void*)_tmp3A2,((
_tmp9C1->initializer=(struct Cyc_Absyn_Exp*)xinit,((_tmp9C1->rgn=0,((_tmp9C1->attributes=
0,((_tmp9C1->escapes=0,_tmp9C1)))))))))))))))));struct Cyc_Absyn_Local_b_struct
_tmp9C4;struct Cyc_Absyn_Local_b_struct*_tmp9C3;struct Cyc_Absyn_Local_b_struct*
_tmp3B3=(_tmp9C3=_cycalloc(sizeof(*_tmp9C3)),((_tmp9C3[0]=((_tmp9C4.tag=3,((
_tmp9C4.f1=_tmp3B2,_tmp9C4)))),_tmp9C3)));struct Cyc_Absyn_Exp*_tmp3B4=Cyc_Absyn_varb_exp(
_tmp3AF,(void*)_tmp3B3,0);{struct Cyc_Core_Opt*_tmp9C5;_tmp3B4->topt=((_tmp9C5=
_cycalloc(sizeof(*_tmp9C5)),((_tmp9C5->v=(void*)fat_ptr_type,_tmp9C5))));}{
struct Cyc_Absyn_Exp*_tmp3B6=Cyc_Absyn_deref_exp(_tmp3B4,0);{struct Cyc_Core_Opt*
_tmp9C6;_tmp3B6->topt=((_tmp9C6=_cycalloc(sizeof(*_tmp9C6)),((_tmp9C6->v=(void*)
elt_type,_tmp9C6))));}Cyc_Toc_exp_to_c(_tmp3B1,_tmp3B6);{struct _tuple1*_tmp3B8=
Cyc_Toc_temp_var();_tmp3B1=Cyc_Toc_add_varmap(rgn2,_tmp3B1,_tmp3B8,Cyc_Absyn_var_exp(
_tmp3B8,0));{struct Cyc_Absyn_Vardecl*_tmp9C7;struct Cyc_Absyn_Vardecl*_tmp3B9=(
_tmp9C7=_cycalloc(sizeof(*_tmp9C7)),((_tmp9C7->sc=(void*)((void*)2),((_tmp9C7->name=
_tmp3B8,((_tmp9C7->tq=Cyc_Toc_mt_tq,((_tmp9C7->type=(void*)_tmp3A1,((_tmp9C7->initializer=(
struct Cyc_Absyn_Exp*)_tmp3B6,((_tmp9C7->rgn=0,((_tmp9C7->attributes=0,((_tmp9C7->escapes=
0,_tmp9C7)))))))))))))))));struct Cyc_Absyn_Local_b_struct _tmp9CA;struct Cyc_Absyn_Local_b_struct*
_tmp9C9;struct Cyc_Absyn_Local_b_struct*_tmp3BA=(_tmp9C9=_cycalloc(sizeof(*
_tmp9C9)),((_tmp9C9[0]=((_tmp9CA.tag=3,((_tmp9CA.f1=_tmp3B9,_tmp9CA)))),_tmp9C9)));
struct Cyc_Absyn_Exp*z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp3BB=Cyc_Absyn_varb_exp(
_tmp3B8,(void*)_tmp3BA,0);_tmp3BB->topt=_tmp3B6->topt;z_init=Cyc_Absyn_prim2_exp((
void*)popt->v,_tmp3BB,e2,0);z_init->topt=_tmp3BB->topt;}Cyc_Toc_exp_to_c(_tmp3B1,
z_init);{struct _tuple1*_tmp3BC=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmp9CB;struct Cyc_Absyn_Vardecl*_tmp3BD=(_tmp9CB=_cycalloc(sizeof(*_tmp9CB)),((
_tmp9CB->sc=(void*)((void*)2),((_tmp9CB->name=_tmp3BC,((_tmp9CB->tq=Cyc_Toc_mt_tq,((
_tmp9CB->type=(void*)_tmp3A1,((_tmp9CB->initializer=(struct Cyc_Absyn_Exp*)z_init,((
_tmp9CB->rgn=0,((_tmp9CB->attributes=0,((_tmp9CB->escapes=0,_tmp9CB)))))))))))))))));
struct Cyc_Absyn_Local_b_struct _tmp9CE;struct Cyc_Absyn_Local_b_struct*_tmp9CD;
struct Cyc_Absyn_Local_b_struct*_tmp3BE=(_tmp9CD=_cycalloc(sizeof(*_tmp9CD)),((
_tmp9CD[0]=((_tmp9CE.tag=3,((_tmp9CE.f1=_tmp3BD,_tmp9CE)))),_tmp9CD)));_tmp3B1=
Cyc_Toc_add_varmap(rgn2,_tmp3B1,_tmp3BC,Cyc_Absyn_var_exp(_tmp3BC,0));{struct Cyc_Absyn_Exp*
_tmp3BF=Cyc_Absyn_varb_exp(_tmp3B8,(void*)_tmp3BA,0);_tmp3BF->topt=_tmp3B6->topt;{
struct Cyc_Absyn_Exp*_tmp3C0=Cyc_Toc_generate_zero(elt_type);struct Cyc_Absyn_Exp*
_tmp3C1=Cyc_Absyn_prim2_exp((void*)5,_tmp3BF,_tmp3C0,0);{struct Cyc_Core_Opt*
_tmp9CF;_tmp3C1->topt=((_tmp9CF=_cycalloc(sizeof(*_tmp9CF)),((_tmp9CF->v=(void*)
Cyc_Absyn_sint_typ,_tmp9CF))));}Cyc_Toc_exp_to_c(_tmp3B1,_tmp3C1);{struct Cyc_Absyn_Exp*
_tmp3C3=Cyc_Absyn_varb_exp(_tmp3BC,(void*)_tmp3BE,0);_tmp3C3->topt=_tmp3B6->topt;{
struct Cyc_Absyn_Exp*_tmp3C4=Cyc_Toc_generate_zero(elt_type);struct Cyc_Absyn_Exp*
_tmp3C5=Cyc_Absyn_prim2_exp((void*)6,_tmp3C3,_tmp3C4,0);{struct Cyc_Core_Opt*
_tmp9D0;_tmp3C5->topt=((_tmp9D0=_cycalloc(sizeof(*_tmp9D0)),((_tmp9D0->v=(void*)
Cyc_Absyn_sint_typ,_tmp9D0))));}Cyc_Toc_exp_to_c(_tmp3B1,_tmp3C5);{struct Cyc_Absyn_Exp*
_tmp9D1[2];struct Cyc_List_List*_tmp3C7=(_tmp9D1[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0),((_tmp9D1[0]=Cyc_Absyn_varb_exp(_tmp3AF,(void*)_tmp3B3,0),((struct
Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp9D1,
sizeof(struct Cyc_Absyn_Exp*),2)))));struct Cyc_Absyn_Exp*_tmp3C8=Cyc_Absyn_uint_exp(
1,0);struct Cyc_Absyn_Exp*xsize;if(is_dynforward)xsize=Cyc_Absyn_prim2_exp((void*)
5,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dynforward_size_e,_tmp3C7,0),_tmp3C8,0);
else{xsize=Cyc_Absyn_prim2_exp((void*)5,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,
_tmp3C7,0),_tmp3C8,0);}{struct Cyc_Absyn_Exp*_tmp3C9=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(
_tmp3C1,_tmp3C5,0),0);struct Cyc_Absyn_Stmt*_tmp3CA=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp3CB=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp3AF,(void*)_tmp3B3,0),Cyc_Toc_curr_sp,0);_tmp3CB=Cyc_Toc_cast_it(
_tmp3A3,_tmp3CB);{struct Cyc_Absyn_Exp*_tmp3CC=Cyc_Absyn_deref_exp(_tmp3CB,0);
struct Cyc_Absyn_Exp*_tmp3CD=Cyc_Absyn_assign_exp(_tmp3CC,Cyc_Absyn_var_exp(
_tmp3BC,0),0);struct Cyc_Absyn_Stmt*_tmp3CE=Cyc_Absyn_exp_stmt(_tmp3CD,0);_tmp3CE=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp3C9,_tmp3CA,Cyc_Absyn_skip_stmt(
0),0),_tmp3CE,0);{struct Cyc_Absyn_Var_d_struct*_tmp9D7;struct Cyc_Absyn_Var_d_struct
_tmp9D6;struct Cyc_Absyn_Decl*_tmp9D5;_tmp3CE=Cyc_Absyn_decl_stmt(((_tmp9D5=
_cycalloc(sizeof(*_tmp9D5)),((_tmp9D5->r=(void*)((void*)((_tmp9D7=_cycalloc(
sizeof(*_tmp9D7)),((_tmp9D7[0]=((_tmp9D6.tag=0,((_tmp9D6.f1=_tmp3BD,_tmp9D6)))),
_tmp9D7))))),((_tmp9D5->loc=0,_tmp9D5)))))),_tmp3CE,0);}{struct Cyc_Absyn_Var_d_struct*
_tmp9DD;struct Cyc_Absyn_Var_d_struct _tmp9DC;struct Cyc_Absyn_Decl*_tmp9DB;_tmp3CE=
Cyc_Absyn_decl_stmt(((_tmp9DB=_cycalloc(sizeof(*_tmp9DB)),((_tmp9DB->r=(void*)((
void*)((_tmp9DD=_cycalloc(sizeof(*_tmp9DD)),((_tmp9DD[0]=((_tmp9DC.tag=0,((
_tmp9DC.f1=_tmp3B9,_tmp9DC)))),_tmp9DD))))),((_tmp9DB->loc=0,_tmp9DB)))))),
_tmp3CE,0);}{struct Cyc_Absyn_Var_d_struct*_tmp9E3;struct Cyc_Absyn_Var_d_struct
_tmp9E2;struct Cyc_Absyn_Decl*_tmp9E1;_tmp3CE=Cyc_Absyn_decl_stmt(((_tmp9E1=
_cycalloc(sizeof(*_tmp9E1)),((_tmp9E1->r=(void*)((void*)((_tmp9E3=_cycalloc(
sizeof(*_tmp9E3)),((_tmp9E3[0]=((_tmp9E2.tag=0,((_tmp9E2.f1=_tmp3B2,_tmp9E2)))),
_tmp9E3))))),((_tmp9E1->loc=0,_tmp9E1)))))),_tmp3CE,0);}(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(
_tmp3CE));}}}}}}}}}}}};_pop_region(rgn2);}}}struct _tuple11{struct _tuple1*f1;void*
f2;struct Cyc_Absyn_Exp*f3;};struct _tuple12{struct Cyc_Absyn_Aggrfield*f1;struct
Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e);static void _tmpA3A(unsigned int*_tmpA39,unsigned int*_tmpA38,struct _tuple1***
_tmpA36){for(*_tmpA39=0;*_tmpA39 < *_tmpA38;(*_tmpA39)++){(*_tmpA36)[*_tmpA39]=
Cyc_Toc_temp_var();}}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
e){void*_tmp3E3=(void*)e->r;if(e->topt == 0){const char*_tmp9E7;void*_tmp9E6[1];
struct Cyc_String_pa_struct _tmp9E5;(_tmp9E5.tag=0,((_tmp9E5.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e)),((_tmp9E6[0]=&
_tmp9E5,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((
_tmp9E7="exp_to_c: no type for %s",_tag_dynforward(_tmp9E7,sizeof(char),
_get_zero_arr_size_char(_tmp9E7,25)))),_tag_dynforward(_tmp9E6,sizeof(void*),1)))))));}{
void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*_tmp3E7=
_tmp3E3;union Cyc_Absyn_Cnst_union _tmp3E8;struct _tuple1*_tmp3E9;void*_tmp3EA;
struct _tuple1*_tmp3EB;void*_tmp3EC;struct Cyc_List_List*_tmp3ED;struct Cyc_Absyn_Exp*
_tmp3EE;void*_tmp3EF;struct Cyc_Absyn_Exp*_tmp3F0;struct Cyc_Core_Opt*_tmp3F1;
struct Cyc_Absyn_Exp*_tmp3F2;struct Cyc_Absyn_Exp*_tmp3F3;struct Cyc_Absyn_Exp*
_tmp3F4;struct Cyc_Absyn_Exp*_tmp3F5;struct Cyc_Absyn_Exp*_tmp3F6;struct Cyc_Absyn_Exp*
_tmp3F7;struct Cyc_Absyn_Exp*_tmp3F8;struct Cyc_Absyn_Exp*_tmp3F9;struct Cyc_Absyn_Exp*
_tmp3FA;struct Cyc_Absyn_Exp*_tmp3FB;struct Cyc_Absyn_Exp*_tmp3FC;struct Cyc_List_List*
_tmp3FD;struct Cyc_Absyn_Exp*_tmp3FE;struct Cyc_List_List*_tmp3FF;struct Cyc_Absyn_VarargCallInfo*
_tmp400;struct Cyc_Absyn_Exp*_tmp401;struct Cyc_List_List*_tmp402;struct Cyc_Absyn_VarargCallInfo*
_tmp403;struct Cyc_Absyn_VarargCallInfo _tmp404;int _tmp405;struct Cyc_List_List*
_tmp406;struct Cyc_Absyn_VarargInfo*_tmp407;struct Cyc_Absyn_Exp*_tmp408;struct Cyc_Absyn_Exp*
_tmp409;struct Cyc_Absyn_Exp*_tmp40A;struct Cyc_List_List*_tmp40B;void*_tmp40C;
void**_tmp40D;struct Cyc_Absyn_Exp*_tmp40E;int _tmp40F;void*_tmp410;struct Cyc_Absyn_Exp*
_tmp411;struct Cyc_Absyn_Exp*_tmp412;struct Cyc_Absyn_Exp*_tmp413;struct Cyc_Absyn_Exp*
_tmp414;void*_tmp415;void*_tmp416;void*_tmp417;struct _dynforward_ptr*_tmp418;
void*_tmp419;void*_tmp41A;unsigned int _tmp41B;struct Cyc_Absyn_Exp*_tmp41C;struct
Cyc_Absyn_Exp*_tmp41D;struct _dynforward_ptr*_tmp41E;struct Cyc_Absyn_Exp*_tmp41F;
struct _dynforward_ptr*_tmp420;struct Cyc_Absyn_Exp*_tmp421;struct Cyc_Absyn_Exp*
_tmp422;struct Cyc_List_List*_tmp423;struct Cyc_List_List*_tmp424;struct Cyc_Absyn_Vardecl*
_tmp425;struct Cyc_Absyn_Exp*_tmp426;struct Cyc_Absyn_Exp*_tmp427;int _tmp428;
struct _tuple1*_tmp429;struct Cyc_List_List*_tmp42A;struct Cyc_List_List*_tmp42B;
struct Cyc_Absyn_Aggrdecl*_tmp42C;void*_tmp42D;struct Cyc_List_List*_tmp42E;struct
Cyc_List_List*_tmp42F;struct Cyc_Absyn_Tuniondecl*_tmp430;struct Cyc_Absyn_Tunionfield*
_tmp431;struct Cyc_List_List*_tmp432;struct Cyc_Absyn_Tuniondecl*_tmp433;struct Cyc_Absyn_Tunionfield*
_tmp434;struct Cyc_Absyn_MallocInfo _tmp435;int _tmp436;struct Cyc_Absyn_Exp*_tmp437;
void**_tmp438;struct Cyc_Absyn_Exp*_tmp439;int _tmp43A;struct Cyc_Absyn_Exp*_tmp43B;
struct Cyc_Absyn_Exp*_tmp43C;struct Cyc_Absyn_Stmt*_tmp43D;_LL1E1: if(*((int*)
_tmp3E7)!= 0)goto _LL1E3;_tmp3E8=((struct Cyc_Absyn_Const_e_struct*)_tmp3E7)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp3E7)->f1).Null_c).tag != 6)goto _LL1E3;
_LL1E2: {struct Cyc_Absyn_Exp*_tmp43E=Cyc_Absyn_uint_exp(0,0);int forward_only=0;
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ,& forward_only)){if(Cyc_Toc_is_toplevel(
nv))(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp43E,
_tmp43E,forward_only))->r));else{if(forward_only){struct Cyc_Absyn_Exp*_tmp9E8[3];(
void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dynforward_e,((_tmp9E8[2]=
_tmp43E,((_tmp9E8[1]=_tmp43E,((_tmp9E8[0]=_tmp43E,((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp9E8,sizeof(struct Cyc_Absyn_Exp*),
3))))))))));}else{struct Cyc_Absyn_Exp*_tmp9E9[3];(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__tag_dyneither_e,((_tmp9E9[2]=_tmp43E,((_tmp9E9[1]=_tmp43E,((_tmp9E9[0]=
_tmp43E,((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp9E9,sizeof(struct Cyc_Absyn_Exp*),3))))))))));}}}else{(void*)(
e->r=(void*)((void*)& Cyc_Toc_zero_exp));}goto _LL1E0;}_LL1E3: if(*((int*)_tmp3E7)
!= 0)goto _LL1E5;_LL1E4: goto _LL1E0;_LL1E5: if(*((int*)_tmp3E7)!= 1)goto _LL1E7;
_tmp3E9=((struct Cyc_Absyn_Var_e_struct*)_tmp3E7)->f1;_tmp3EA=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3E7)->f2;_LL1E6:{struct _handler_cons _tmp441;_push_handler(& _tmp441);{int
_tmp443=0;if(setjmp(_tmp441.handler))_tmp443=1;if(!_tmp443){(void*)(e->r=(void*)((
void*)(Cyc_Toc_lookup_varmap(nv,_tmp3E9))->r));;_pop_handler();}else{void*
_tmp442=(void*)_exn_thrown;void*_tmp445=_tmp442;_LL238: if(_tmp445 != Cyc_Dict_Absent)
goto _LL23A;_LL239: {const char*_tmp9ED;void*_tmp9EC[1];struct Cyc_String_pa_struct
_tmp9EB;(_tmp9EB.tag=0,((_tmp9EB.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp3E9)),((_tmp9EC[0]=& _tmp9EB,((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmp9ED="Can't find %s in exp_to_c, Var\n",
_tag_dynforward(_tmp9ED,sizeof(char),_get_zero_arr_size_char(_tmp9ED,32)))),
_tag_dynforward(_tmp9EC,sizeof(void*),1)))))));}_LL23A:;_LL23B:(void)_throw(
_tmp445);_LL237:;}}}goto _LL1E0;_LL1E7: if(*((int*)_tmp3E7)!= 2)goto _LL1E9;_tmp3EB=((
struct Cyc_Absyn_UnknownId_e_struct*)_tmp3E7)->f1;_LL1E8: {const char*_tmp9F0;void*
_tmp9EF;(_tmp9EF=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(((_tmp9F0="unknownid",_tag_dynforward(_tmp9F0,sizeof(char),
_get_zero_arr_size_char(_tmp9F0,10)))),_tag_dynforward(_tmp9EF,sizeof(void*),0)));}
_LL1E9: if(*((int*)_tmp3E7)!= 3)goto _LL1EB;_tmp3EC=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3E7)->f1;_tmp3ED=((struct Cyc_Absyn_Primop_e_struct*)_tmp3E7)->f2;_LL1EA: {
struct Cyc_List_List*_tmp44B=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp3ED);((void(*)(void(*
f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*
x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp3ED);{void*_tmp44C=_tmp3EC;_LL23D:
if((int)_tmp44C != 19)goto _LL23F;_LL23E: {struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3ED))->hd;{void*_tmp44D=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo
_tmp44E;struct Cyc_Absyn_Exp*_tmp44F;struct Cyc_Absyn_PtrInfo _tmp450;void*_tmp451;
struct Cyc_Absyn_PtrAtts _tmp452;struct Cyc_Absyn_Conref*_tmp453;struct Cyc_Absyn_Conref*
_tmp454;struct Cyc_Absyn_Conref*_tmp455;_LL252: if(_tmp44D <= (void*)4)goto _LL256;
if(*((int*)_tmp44D)!= 7)goto _LL254;_tmp44E=((struct Cyc_Absyn_ArrayType_struct*)
_tmp44D)->f1;_tmp44F=_tmp44E.num_elts;_LL253:(void*)(e->r=(void*)((void*)((
struct Cyc_Absyn_Exp*)_check_null(_tmp44F))->r));goto _LL251;_LL254: if(*((int*)
_tmp44D)!= 4)goto _LL256;_tmp450=((struct Cyc_Absyn_PointerType_struct*)_tmp44D)->f1;
_tmp451=(void*)_tmp450.elt_typ;_tmp452=_tmp450.ptr_atts;_tmp453=_tmp452.nullable;
_tmp454=_tmp452.bounds;_tmp455=_tmp452.zero_term;_LL255:{void*_tmp456=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp454);struct Cyc_Absyn_Exp*_tmp457;_LL259: if((int)_tmp456
!= 0)goto _LL25B;_LL25A:{struct Cyc_Absyn_Exp*_tmp9F1[2];(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__get_dynforward_size_e,((_tmp9F1[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
_tmp451),0),((_tmp9F1[0]=(struct Cyc_Absyn_Exp*)_tmp3ED->hd,((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp9F1,sizeof(struct Cyc_Absyn_Exp*),
2))))))));}goto _LL258;_LL25B: if((int)_tmp456 != 1)goto _LL25D;_LL25C:{struct Cyc_Absyn_Exp*
_tmp9F2[2];(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,((
_tmp9F2[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp451),0),((_tmp9F2[0]=(
struct Cyc_Absyn_Exp*)_tmp3ED->hd,((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmp9F2,sizeof(struct Cyc_Absyn_Exp*),2))))))));}
goto _LL258;_LL25D: if(_tmp456 <= (void*)2)goto _LL25F;if(*((int*)_tmp456)!= 0)goto
_LL25F;_tmp457=((struct Cyc_Absyn_Upper_b_struct*)_tmp456)->f1;_LL25E: if(((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp455)){struct Cyc_Absyn_Exp*
function_e=Cyc_Toc_getFunction(& Cyc_Toc__get_zero_arr_size_functionSet,(struct
Cyc_Absyn_Exp*)_tmp3ED->hd);struct Cyc_Absyn_Exp*_tmp9F3[2];(void*)(e->r=(void*)
Cyc_Toc_fncall_exp_r(function_e,((_tmp9F3[1]=_tmp457,((_tmp9F3[0]=(struct Cyc_Absyn_Exp*)
_tmp3ED->hd,((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp9F3,sizeof(struct Cyc_Absyn_Exp*),2))))))));}else{if(((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp453))(void*)(e->r=(
void*)Cyc_Toc_conditional_exp_r(arg,_tmp457,Cyc_Absyn_uint_exp(0,0)));else{(void*)(
e->r=(void*)((void*)_tmp457->r));goto _LL258;}}goto _LL258;_LL25F: if(_tmp456 <= (
void*)2)goto _LL258;if(*((int*)_tmp456)!= 1)goto _LL258;_LL260: {const char*_tmp9F6;
void*_tmp9F5;(_tmp9F5=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr
ap))Cyc_Toc_unimp)(((_tmp9F6="toc: size of AbsUpper_b",_tag_dynforward(_tmp9F6,
sizeof(char),_get_zero_arr_size_char(_tmp9F6,24)))),_tag_dynforward(_tmp9F5,
sizeof(void*),0)));}_LL258:;}goto _LL251;_LL256:;_LL257: {const char*_tmp9FB;void*
_tmp9FA[2];struct Cyc_String_pa_struct _tmp9F9;struct Cyc_String_pa_struct _tmp9F8;(
_tmp9F8.tag=0,((_tmp9F8.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((_tmp9F9.tag=0,((
_tmp9F9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v)),((_tmp9FA[0]=& _tmp9F9,((
_tmp9FA[1]=& _tmp9F8,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(((_tmp9FB="size primop applied to non-array %s (%s)",
_tag_dynforward(_tmp9FB,sizeof(char),_get_zero_arr_size_char(_tmp9FB,41)))),
_tag_dynforward(_tmp9FA,sizeof(void*),2)))))))))))));}_LL251:;}goto _LL23C;}
_LL23F: if((int)_tmp44C != 0)goto _LL241;_LL240:{void*_tmp461=Cyc_Tcutil_compress((
void*)((struct Cyc_List_List*)_check_null(_tmp44B))->hd);struct Cyc_Absyn_PtrInfo
_tmp462;void*_tmp463;struct Cyc_Absyn_PtrAtts _tmp464;struct Cyc_Absyn_Conref*
_tmp465;struct Cyc_Absyn_Conref*_tmp466;_LL262: if(_tmp461 <= (void*)4)goto _LL264;
if(*((int*)_tmp461)!= 4)goto _LL264;_tmp462=((struct Cyc_Absyn_PointerType_struct*)
_tmp461)->f1;_tmp463=(void*)_tmp462.elt_typ;_tmp464=_tmp462.ptr_atts;_tmp465=
_tmp464.bounds;_tmp466=_tmp464.zero_term;_LL263:{void*_tmp467=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp465);struct Cyc_Absyn_Exp*_tmp468;_LL267: if((int)_tmp467
!= 0)goto _LL269;_LL268: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp3ED))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3ED->tl))->hd;{struct Cyc_Absyn_Exp*_tmp9FC[
3];(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dynforward_ptr_plus_e,((
_tmp9FC[2]=e2,((_tmp9FC[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp463),0),((
_tmp9FC[0]=e1,((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp9FC,sizeof(struct Cyc_Absyn_Exp*),3))))))))));}goto _LL266;}
_LL269: if((int)_tmp467 != 1)goto _LL26B;_LL26A: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3ED))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3ED->tl))->hd;{struct Cyc_Absyn_Exp*_tmp9FD[
3];(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((
_tmp9FD[2]=e2,((_tmp9FD[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp463),0),((
_tmp9FD[0]=e1,((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmp9FD,sizeof(struct Cyc_Absyn_Exp*),3))))))))));}goto _LL266;}
_LL26B: if(_tmp467 <= (void*)2)goto _LL26D;if(*((int*)_tmp467)!= 0)goto _LL26D;
_tmp468=((struct Cyc_Absyn_Upper_b_struct*)_tmp467)->f1;_LL26C: if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp466)){struct Cyc_Absyn_Exp*
e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp3ED))->hd;struct
Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp3ED->tl))->hd;struct Cyc_Absyn_Exp*_tmp9FE[3];(void*)(e->r=(void*)((void*)(
Cyc_Absyn_fncall_exp(Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_plus_functionSet,e1),((
_tmp9FE[2]=e2,((_tmp9FE[1]=_tmp468,((_tmp9FE[0]=e1,((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmp9FE,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0))->r));}goto _LL266;_LL26D: if(_tmp467 <= (void*)2)goto _LL266;if(*((int*)
_tmp467)!= 1)goto _LL266;_LL26E: {const char*_tmpA01;void*_tmpA00;(_tmpA00=0,((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(((_tmpA01="toc: plus on AbsUpper_b",
_tag_dynforward(_tmpA01,sizeof(char),_get_zero_arr_size_char(_tmpA01,24)))),
_tag_dynforward(_tmpA00,sizeof(void*),0)));}_LL266:;}goto _LL261;_LL264:;_LL265:
goto _LL261;_LL261:;}goto _LL23C;_LL241: if((int)_tmp44C != 2)goto _LL243;_LL242: {
void*elt_typ=(void*)0;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt((
void*)((struct Cyc_List_List*)_check_null(_tmp44B))->hd,& elt_typ,& forward_only)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(
_tmp3ED))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp3ED->tl))->hd;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp44B->tl))->hd,& forward_only)){(void*)(
e1->r=(void*)Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp));(
void*)(e2->r=(void*)Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((void*)e2->r,0),
Cyc_Toc_curr_sp));(void*)(e->r=(void*)((void*)(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(
e),Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r));}else{if(
forward_only){const char*_tmpA04;void*_tmpA03;(_tmpA03=0,Cyc_Tcutil_terr(e->loc,((
_tmpA04="subtraction not allowed on forward-only ? pointer",_tag_dynforward(
_tmpA04,sizeof(char),_get_zero_arr_size_char(_tmpA04,50)))),_tag_dynforward(
_tmpA03,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*_tmpA05[3];(void*)(e->r=(void*)
Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,((_tmpA05[2]=Cyc_Absyn_prim1_exp((
void*)2,e2,0),((_tmpA05[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((
_tmpA05[0]=e1,((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmpA05,sizeof(struct Cyc_Absyn_Exp*),3))))))))));}}}goto _LL23C;}
_LL243: if((int)_tmp44C != 5)goto _LL245;_LL244: goto _LL246;_LL245: if((int)_tmp44C != 
6)goto _LL247;_LL246: goto _LL248;_LL247: if((int)_tmp44C != 7)goto _LL249;_LL248: goto
_LL24A;_LL249: if((int)_tmp44C != 9)goto _LL24B;_LL24A: goto _LL24C;_LL24B: if((int)
_tmp44C != 8)goto _LL24D;_LL24C: goto _LL24E;_LL24D: if((int)_tmp44C != 10)goto _LL24F;
_LL24E: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp3ED))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp3ED->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp44B))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp44B->tl))->hd;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ(t1,&
forward_only))(void*)(e1->r=(void*)Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((
void*)e1->r,0),Cyc_Toc_curr_sp));if(Cyc_Tcutil_is_tagged_pointer_typ(t2,&
forward_only))(void*)(e2->r=(void*)Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((
void*)e2->r,0),Cyc_Toc_curr_sp));goto _LL23C;}_LL24F:;_LL250: goto _LL23C;_LL23C:;}
goto _LL1E0;}_LL1EB: if(*((int*)_tmp3E7)!= 5)goto _LL1ED;_tmp3EE=((struct Cyc_Absyn_Increment_e_struct*)
_tmp3E7)->f1;_tmp3EF=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmp3E7)->f2;
_LL1EC: {void*e2_cyc_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3EE->topt))->v;
void*ptr_type=(void*)0;void*elt_type=(void*)0;int is_dynforward=0;int is_dyneither=
0;const char*_tmpA06;struct _dynforward_ptr incr_str=(_tmpA06="increment",
_tag_dynforward(_tmpA06,sizeof(char),_get_zero_arr_size_char(_tmpA06,10)));if(
_tmp3EF == (void*)2  || _tmp3EF == (void*)3){const char*_tmpA07;incr_str=((_tmpA07="decrement",
_tag_dynforward(_tmpA07,sizeof(char),_get_zero_arr_size_char(_tmpA07,10))));}if(
Cyc_Tcutil_is_zero_ptr_deref(_tmp3EE,& ptr_type,& is_dynforward,& is_dyneither,&
elt_type)){{const char*_tmpA0B;void*_tmpA0A[1];struct Cyc_String_pa_struct _tmpA09;(
_tmpA09.tag=0,((_tmpA09.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
incr_str),((_tmpA0A[0]=& _tmpA09,Cyc_Tcutil_terr(e->loc,((_tmpA0B="in-place %s is not supported when dereferencing a zero-terminated pointer",
_tag_dynforward(_tmpA0B,sizeof(char),_get_zero_arr_size_char(_tmpA0B,74)))),
_tag_dynforward(_tmpA0A,sizeof(void*),1)))))));}{const char*_tmpA0E;void*_tmpA0D;(
_tmpA0D=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((
_tmpA0E="in-place inc on zero-term",_tag_dynforward(_tmpA0E,sizeof(char),
_get_zero_arr_size_char(_tmpA0E,26)))),_tag_dynforward(_tmpA0D,sizeof(void*),0)));}}
Cyc_Toc_exp_to_c(nv,_tmp3EE);{void*elt_typ=(void*)0;int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ,& forward_only)){struct Cyc_Absyn_Exp*fn_e;int change=1;if(
forward_only){if(_tmp3EF == (void*)3  || _tmp3EF == (void*)2){const char*_tmpA11;
void*_tmpA10;(_tmpA10=0,Cyc_Tcutil_terr(e->loc,((_tmpA11="decrement not allowed on forward ? pointer",
_tag_dynforward(_tmpA11,sizeof(char),_get_zero_arr_size_char(_tmpA11,43)))),
_tag_dynforward(_tmpA10,sizeof(void*),0)));}fn_e=_tmp3EF == (void*)1?Cyc_Toc__dynforward_ptr_inplace_plus_post_e:
Cyc_Toc__dynforward_ptr_inplace_plus_e;}else{fn_e=(_tmp3EF == (void*)1  || _tmp3EF
== (void*)3)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp3EF == (void*)2  || _tmp3EF == (void*)3)change=- 1;}{struct Cyc_Absyn_Exp*
_tmpA12[3];(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(fn_e,((_tmpA12[2]=Cyc_Absyn_signed_int_exp(
change,0),((_tmpA12[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((
_tmpA12[0]=Cyc_Absyn_address_exp(_tmp3EE,0),((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmpA12,sizeof(struct Cyc_Absyn_Exp*),
3))))))))));}}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;{void*_tmp47A=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3EE->topt))->v);void*_tmp47B;int _tmp47C;_LL270: if(_tmp47A <= (
void*)4)goto _LL272;if(*((int*)_tmp47A)!= 5)goto _LL272;_tmp47B=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp47A)->f2;_LL271:{void*_tmp47D=_tmp47B;_LL27B: if((int)_tmp47D != 0)goto _LL27D;
_LL27C: fn_e=_tmp3EF == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_char_e: Cyc_Toc__zero_arr_inplace_plus_char_e;
goto _LL27A;_LL27D: if((int)_tmp47D != 1)goto _LL27F;_LL27E: fn_e=_tmp3EF == (void*)1?
Cyc_Toc__zero_arr_inplace_plus_post_short_e: Cyc_Toc__zero_arr_inplace_plus_short_e;
goto _LL27A;_LL27F: if((int)_tmp47D != 2)goto _LL281;_LL280: fn_e=_tmp3EF == (void*)1?
Cyc_Toc__zero_arr_inplace_plus_post_int_e: Cyc_Toc__zero_arr_inplace_plus_int_e;
goto _LL27A;_LL281:;_LL282: {struct Cyc_Core_Impossible_struct _tmpA18;const char*
_tmpA17;struct Cyc_Core_Impossible_struct*_tmpA16;(int)_throw((void*)((_tmpA16=
_cycalloc(sizeof(*_tmpA16)),((_tmpA16[0]=((_tmpA18.tag=Cyc_Core_Impossible,((
_tmpA18.f1=((_tmpA17="impossible IntType (not char, short or int)",
_tag_dynforward(_tmpA17,sizeof(char),_get_zero_arr_size_char(_tmpA17,44)))),
_tmpA18)))),_tmpA16)))));}_LL27A:;}goto _LL26F;_LL272: if((int)_tmp47A != 1)goto
_LL274;_LL273: fn_e=_tmp3EF == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_float_e:
Cyc_Toc__zero_arr_inplace_plus_float_e;goto _LL26F;_LL274: if(_tmp47A <= (void*)4)
goto _LL278;if(*((int*)_tmp47A)!= 6)goto _LL276;_tmp47C=((struct Cyc_Absyn_DoubleType_struct*)
_tmp47A)->f1;_LL275: switch(_tmp47C){case 1: _LL283: fn_e=_tmp3EF == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e:
Cyc_Toc__zero_arr_inplace_plus_longdouble_e;break;default: _LL284: fn_e=_tmp3EF == (
void*)1?Cyc_Toc__zero_arr_inplace_plus_post_double_e: Cyc_Toc__zero_arr_inplace_plus_double_e;}
goto _LL26F;_LL276: if(*((int*)_tmp47A)!= 4)goto _LL278;_LL277: fn_e=_tmp3EF == (void*)
1?Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e: Cyc_Toc__zero_arr_inplace_plus_voidstar_e;
goto _LL26F;_LL278:;_LL279: {struct Cyc_Core_Impossible_struct _tmpA1E;const char*
_tmpA1D;struct Cyc_Core_Impossible_struct*_tmpA1C;(int)_throw((void*)((_tmpA1C=
_cycalloc(sizeof(*_tmpA1C)),((_tmpA1C[0]=((_tmpA1E.tag=Cyc_Core_Impossible,((
_tmpA1E.f1=((_tmpA1D="impossible expression type (not int, float, double, or pointer)",
_tag_dynforward(_tmpA1D,sizeof(char),_get_zero_arr_size_char(_tmpA1D,64)))),
_tmpA1E)))),_tmpA1C)))));}_LL26F:;}{struct Cyc_Absyn_Exp*_tmpA1F[2];(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(fn_e,((_tmpA1F[1]=Cyc_Absyn_signed_int_exp(1,0),((
_tmpA1F[0]=_tmp3EE,((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmpA1F,sizeof(struct Cyc_Absyn_Exp*),2))))))));}}else{if(elt_typ
== (void*)0  && !Cyc_Absyn_is_lvalue(_tmp3EE)){Cyc_Toc_lvalue_assign(_tmp3EE,0,
Cyc_Toc_incr_lvalue,_tmp3EF);(void*)(e->r=(void*)((void*)_tmp3EE->r));}}}goto
_LL1E0;}}_LL1ED: if(*((int*)_tmp3E7)!= 4)goto _LL1EF;_tmp3F0=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp3E7)->f1;_tmp3F1=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp3E7)->f2;_tmp3F2=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp3E7)->f3;_LL1EE: {void*ptr_type=(void*)0;
void*elt_type=(void*)0;int is_dynforward=0;int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp3F0,& ptr_type,& is_dynforward,& is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(
nv,e,_tmp3F0,_tmp3F1,_tmp3F2,ptr_type,is_dynforward,is_dyneither,elt_type);
return;}{int e1_poly=Cyc_Toc_is_poly_project(_tmp3F0);void*e1_old_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp3F0->topt))->v;void*e2_old_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp3F2->topt))->v;Cyc_Toc_exp_to_c(nv,_tmp3F0);
Cyc_Toc_exp_to_c(nv,_tmp3F2);{int done=0;if(_tmp3F1 != 0){void*elt_typ=(void*)0;
int forward_only=0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ,&
forward_only)){struct Cyc_Absyn_Exp*change;{void*_tmp486=(void*)_tmp3F1->v;_LL287:
if((int)_tmp486 != 0)goto _LL289;_LL288: change=_tmp3F2;goto _LL286;_LL289: if((int)
_tmp486 != 2)goto _LL28B;_LL28A: if(forward_only){const char*_tmpA22;void*_tmpA21;(
_tmpA21=0,Cyc_Tcutil_terr(e->loc,((_tmpA22="subtraction not allowed on forward ? pointers",
_tag_dynforward(_tmpA22,sizeof(char),_get_zero_arr_size_char(_tmpA22,46)))),
_tag_dynforward(_tmpA21,sizeof(void*),0)));}change=Cyc_Absyn_prim1_exp((void*)2,
_tmp3F2,0);goto _LL286;_LL28B:;_LL28C: {const char*_tmpA25;void*_tmpA24;(_tmpA24=0,((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((
_tmpA25="bad t ? pointer arithmetic",_tag_dynforward(_tmpA25,sizeof(char),
_get_zero_arr_size_char(_tmpA25,27)))),_tag_dynforward(_tmpA24,sizeof(void*),0)));}
_LL286:;}done=1;{struct Cyc_Absyn_Exp*_tmp48B=forward_only?Cyc_Toc__dynforward_ptr_inplace_plus_e:
Cyc_Toc__dyneither_ptr_inplace_plus_e;struct Cyc_Absyn_Exp*_tmpA26[3];(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(_tmp48B,((_tmpA26[2]=change,((_tmpA26[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0),((_tmpA26[0]=Cyc_Absyn_address_exp(_tmp3F0,0),((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmpA26,sizeof(struct Cyc_Absyn_Exp*),3))))))))));}}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ)){void*_tmp48D=(void*)_tmp3F1->v;_LL28E: if((int)_tmp48D != 0)
goto _LL290;_LL28F: done=1;{struct Cyc_Absyn_Exp*_tmpA27[2];(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp3F0),((
_tmpA27[1]=_tmp3F2,((_tmpA27[0]=_tmp3F0,((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmpA27,sizeof(struct Cyc_Absyn_Exp*),
2))))))));}goto _LL28D;_LL290:;_LL291: {const char*_tmpA2A;void*_tmpA29;(_tmpA29=0,((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((
_tmpA2A="bad zero-terminated pointer arithmetic",_tag_dynforward(_tmpA2A,sizeof(
char),_get_zero_arr_size_char(_tmpA2A,39)))),_tag_dynforward(_tmpA29,sizeof(void*),
0)));}_LL28D:;}}}if(!done){if(e1_poly)(void*)(_tmp3F2->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp3F2->r,0)));if(!Cyc_Absyn_is_lvalue(
_tmp3F0)){{struct _tuple10 _tmpA2D;struct _tuple10*_tmpA2C;((void(*)(struct Cyc_Absyn_Exp*
e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct
_tuple10*),struct _tuple10*f_env))Cyc_Toc_lvalue_assign)(_tmp3F0,0,Cyc_Toc_assignop_lvalue,((
_tmpA2C=_cycalloc(sizeof(struct _tuple10)* 1),((_tmpA2C[0]=((_tmpA2D.f1=_tmp3F1,((
_tmpA2D.f2=_tmp3F2,_tmpA2D)))),_tmpA2C)))));}(void*)(e->r=(void*)((void*)_tmp3F0->r));}}
goto _LL1E0;}}}_LL1EF: if(*((int*)_tmp3E7)!= 6)goto _LL1F1;_tmp3F3=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp3E7)->f1;_tmp3F4=((struct Cyc_Absyn_Conditional_e_struct*)_tmp3E7)->f2;
_tmp3F5=((struct Cyc_Absyn_Conditional_e_struct*)_tmp3E7)->f3;_LL1F0: Cyc_Toc_exp_to_c(
nv,_tmp3F3);Cyc_Toc_exp_to_c(nv,_tmp3F4);Cyc_Toc_exp_to_c(nv,_tmp3F5);goto _LL1E0;
_LL1F1: if(*((int*)_tmp3E7)!= 7)goto _LL1F3;_tmp3F6=((struct Cyc_Absyn_And_e_struct*)
_tmp3E7)->f1;_tmp3F7=((struct Cyc_Absyn_And_e_struct*)_tmp3E7)->f2;_LL1F2: Cyc_Toc_exp_to_c(
nv,_tmp3F6);Cyc_Toc_exp_to_c(nv,_tmp3F7);goto _LL1E0;_LL1F3: if(*((int*)_tmp3E7)!= 
8)goto _LL1F5;_tmp3F8=((struct Cyc_Absyn_Or_e_struct*)_tmp3E7)->f1;_tmp3F9=((
struct Cyc_Absyn_Or_e_struct*)_tmp3E7)->f2;_LL1F4: Cyc_Toc_exp_to_c(nv,_tmp3F8);
Cyc_Toc_exp_to_c(nv,_tmp3F9);goto _LL1E0;_LL1F5: if(*((int*)_tmp3E7)!= 9)goto
_LL1F7;_tmp3FA=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp3E7)->f1;_tmp3FB=((struct
Cyc_Absyn_SeqExp_e_struct*)_tmp3E7)->f2;_LL1F6: Cyc_Toc_exp_to_c(nv,_tmp3FA);Cyc_Toc_exp_to_c(
nv,_tmp3FB);goto _LL1E0;_LL1F7: if(*((int*)_tmp3E7)!= 10)goto _LL1F9;_tmp3FC=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp3E7)->f1;_tmp3FD=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp3E7)->f2;_LL1F8: _tmp3FE=_tmp3FC;_tmp3FF=_tmp3FD;goto _LL1FA;_LL1F9: if(*((int*)
_tmp3E7)!= 11)goto _LL1FB;_tmp3FE=((struct Cyc_Absyn_FnCall_e_struct*)_tmp3E7)->f1;
_tmp3FF=((struct Cyc_Absyn_FnCall_e_struct*)_tmp3E7)->f2;_tmp400=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp3E7)->f3;if(_tmp400 != 0)goto _LL1FB;_LL1FA: Cyc_Toc_exp_to_c(nv,_tmp3FE);((
void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,
struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp3FF);goto _LL1E0;
_LL1FB: if(*((int*)_tmp3E7)!= 11)goto _LL1FD;_tmp401=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp3E7)->f1;_tmp402=((struct Cyc_Absyn_FnCall_e_struct*)_tmp3E7)->f2;_tmp403=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp3E7)->f3;if(_tmp403 == 0)goto _LL1FD;_tmp404=*
_tmp403;_tmp405=_tmp404.num_varargs;_tmp406=_tmp404.injectors;_tmp407=_tmp404.vai;
_LL1FC:{struct _RegionHandle _tmp493=_new_region("r");struct _RegionHandle*r=&
_tmp493;_push_region(r);{struct _tuple1*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
argvexp=Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp405,0);void*cva_type=Cyc_Toc_typ_to_c((void*)_tmp407->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp402);int num_normargs=num_args - _tmp405;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp402=_tmp402->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp402))->hd);{
struct Cyc_List_List*_tmpA2E;new_args=((_tmpA2E=_cycalloc(sizeof(*_tmpA2E)),((
_tmpA2E->hd=(struct Cyc_Absyn_Exp*)_tmp402->hd,((_tmpA2E->tl=new_args,_tmpA2E))))));}}}{
struct Cyc_Absyn_Exp*_tmpA31[3];struct Cyc_List_List*_tmpA30;new_args=((_tmpA30=
_cycalloc(sizeof(*_tmpA30)),((_tmpA30->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dynforward_e,((
_tmpA31[2]=num_varargs_exp,((_tmpA31[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0),((
_tmpA31[0]=argvexp,((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmpA31,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpA30->tl=
new_args,_tmpA30))))));}new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp401);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp401,new_args,0),0);if(_tmp407->inject){
struct Cyc_Absyn_Tuniondecl*tud;{void*_tmp497=Cyc_Tcutil_compress((void*)_tmp407->type);
struct Cyc_Absyn_TunionInfo _tmp498;union Cyc_Absyn_TunionInfoU_union _tmp499;struct
Cyc_Absyn_Tuniondecl**_tmp49A;struct Cyc_Absyn_Tuniondecl*_tmp49B;_LL293: if(
_tmp497 <= (void*)4)goto _LL295;if(*((int*)_tmp497)!= 2)goto _LL295;_tmp498=((
struct Cyc_Absyn_TunionType_struct*)_tmp497)->f1;_tmp499=_tmp498.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp497)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL295;_tmp49A=(_tmp499.KnownTunion).f1;_tmp49B=*_tmp49A;_LL294: tud=
_tmp49B;goto _LL292;_LL295:;_LL296: {const char*_tmpA34;void*_tmpA33;(_tmpA33=0,((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((
_tmpA34="toc: unknown tunion in vararg with inject",_tag_dynforward(_tmpA34,
sizeof(char),_get_zero_arr_size_char(_tmpA34,42)))),_tag_dynforward(_tmpA33,
sizeof(void*),0)));}_LL292:;}{unsigned int _tmpA39;unsigned int _tmpA38;struct
_dynforward_ptr _tmpA37;struct _tuple1**_tmpA36;unsigned int _tmpA35;struct
_dynforward_ptr vs=(_tmpA35=(unsigned int)_tmp405,((_tmpA36=(struct _tuple1**)
_region_malloc(r,_check_times(sizeof(struct _tuple1*),_tmpA35)),((_tmpA37=
_tag_dynforward(_tmpA36,sizeof(struct _tuple1*),_tmpA35),((((_tmpA38=_tmpA35,
_tmpA3A(& _tmpA39,& _tmpA38,& _tmpA36))),_tmpA37)))))));if(_tmp405 != 0){struct Cyc_List_List*
_tmp49E=0;{int i=_tmp405 - 1;for(0;i >= 0;-- i){struct Cyc_List_List*_tmpA3B;_tmp49E=((
_tmpA3B=_cycalloc(sizeof(*_tmpA3B)),((_tmpA3B->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(
Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dynforward_subscript(vs,sizeof(
struct _tuple1*),i)),0),0)),((_tmpA3B->tl=_tmp49E,_tmpA3B))))));}}s=Cyc_Absyn_declare_stmt(
argv,arr_type,(struct Cyc_Absyn_Exp*)Cyc_Absyn_unresolvedmem_exp(0,_tmp49E,0),s,0);{
int i=0;for(0;_tmp402 != 0;(((_tmp402=_tmp402->tl,_tmp406=_tmp406->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp402->hd;void*arg_type=(void*)((
struct Cyc_Core_Opt*)_check_null(arg->topt))->v;struct _tuple1*var=*((struct
_tuple1**)_check_dynforward_subscript(vs,sizeof(struct _tuple1*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Tunionfield _tmp4A1;struct _tuple1*
_tmp4A2;struct Cyc_List_List*_tmp4A3;struct Cyc_Absyn_Tunionfield*_tmp4A0=(struct
Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp406))->hd;_tmp4A1=*
_tmp4A0;_tmp4A2=_tmp4A1.name;_tmp4A3=_tmp4A1.typs;{void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp4A3))->hd)).f2);Cyc_Toc_exp_to_c(
nv,arg);if(Cyc_Toc_is_void_star(field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(
1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(
varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(tud,_tmp4A2,1),0),s,0);{const char*
_tmpA3C;s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp4A2,((_tmpA3C="_struct",_tag_dynforward(_tmpA3C,sizeof(char),
_get_zero_arr_size_char(_tmpA3C,8)))))),0,s,0);}}}}}else{struct _tuple8*_tmpA3D[3];
struct Cyc_List_List*_tmp4A5=(_tmpA3D[2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((
_tmpA3D[1]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpA3D[0]=Cyc_Toc_make_dle(
Cyc_Absyn_uint_exp(0,0)),((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmpA3D,sizeof(struct _tuple8*),3)))))));s=Cyc_Absyn_declare_stmt(
argv,Cyc_Absyn_void_star_typ(),(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}}}
else{{int i=0;for(0;_tmp402 != 0;(_tmp402=_tmp402->tl,++ i)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp402->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp402->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(
s));}};_pop_region(r);}goto _LL1E0;_LL1FD: if(*((int*)_tmp3E7)!= 12)goto _LL1FF;
_tmp408=((struct Cyc_Absyn_Throw_e_struct*)_tmp3E7)->f1;_LL1FE: Cyc_Toc_exp_to_c(
nv,_tmp408);(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(
old_typ),Cyc_Toc_newthrow_exp(_tmp408),0))->r));goto _LL1E0;_LL1FF: if(*((int*)
_tmp3E7)!= 13)goto _LL201;_tmp409=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp3E7)->f1;_LL200: Cyc_Toc_exp_to_c(nv,_tmp409);goto _LL1E0;_LL201: if(*((int*)
_tmp3E7)!= 14)goto _LL203;_tmp40A=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp3E7)->f1;
_tmp40B=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp3E7)->f2;_LL202: Cyc_Toc_exp_to_c(
nv,_tmp40A);for(0;_tmp40B != 0;_tmp40B=_tmp40B->tl){void*k=Cyc_Tcutil_typ_kind((
void*)_tmp40B->hd);if(((k != (void*)6  && k != (void*)3) && k != (void*)4) && k != (
void*)5){{void*_tmp4AB=Cyc_Tcutil_compress((void*)_tmp40B->hd);_LL298: if(_tmp4AB
<= (void*)4)goto _LL29C;if(*((int*)_tmp4AB)!= 1)goto _LL29A;_LL299: goto _LL29B;
_LL29A: if(*((int*)_tmp4AB)!= 2)goto _LL29C;_LL29B: continue;_LL29C:;_LL29D:(void*)(
e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp40A,0))->r));goto _LL297;_LL297:;}
break;}}goto _LL1E0;_LL203: if(*((int*)_tmp3E7)!= 15)goto _LL205;_tmp40C=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp3E7)->f1;_tmp40D=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp3E7)->f1);_tmp40E=((struct Cyc_Absyn_Cast_e_struct*)_tmp3E7)->f2;_tmp40F=((
struct Cyc_Absyn_Cast_e_struct*)_tmp3E7)->f3;_tmp410=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp3E7)->f4;_LL204: {void*old_t2=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp40E->topt))->v;void*new_typ=*_tmp40D;*_tmp40D=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(
nv,_tmp40E);{struct _tuple0 _tmpA3E;struct _tuple0 _tmp4AD=(_tmpA3E.f1=Cyc_Tcutil_compress(
old_t2),((_tmpA3E.f2=Cyc_Tcutil_compress(new_typ),_tmpA3E)));void*_tmp4AE;struct
Cyc_Absyn_PtrInfo _tmp4AF;void*_tmp4B0;struct Cyc_Absyn_PtrInfo _tmp4B1;void*
_tmp4B2;struct Cyc_Absyn_PtrInfo _tmp4B3;void*_tmp4B4;_LL29F: _tmp4AE=_tmp4AD.f1;
if(_tmp4AE <= (void*)4)goto _LL2A1;if(*((int*)_tmp4AE)!= 4)goto _LL2A1;_tmp4AF=((
struct Cyc_Absyn_PointerType_struct*)_tmp4AE)->f1;_tmp4B0=_tmp4AD.f2;if(_tmp4B0 <= (
void*)4)goto _LL2A1;if(*((int*)_tmp4B0)!= 4)goto _LL2A1;_tmp4B1=((struct Cyc_Absyn_PointerType_struct*)
_tmp4B0)->f1;_LL2A0: {int _tmp4B5=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,(_tmp4AF.ptr_atts).nullable);int _tmp4B6=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,(_tmp4B1.ptr_atts).nullable);void*_tmp4B7=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp4AF.ptr_atts).bounds);void*_tmp4B8=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp4B1.ptr_atts).bounds);int _tmp4B9=((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp4AF.ptr_atts).zero_term);int
_tmp4BA=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp4B1.ptr_atts).zero_term);
int p1_forward_only=0;int p2_forward_only=0;{struct _tuple0 _tmpA3F;struct _tuple0
_tmp4BC=(_tmpA3F.f1=_tmp4B7,((_tmpA3F.f2=_tmp4B8,_tmpA3F)));void*_tmp4BD;struct
Cyc_Absyn_Exp*_tmp4BE;void*_tmp4BF;struct Cyc_Absyn_Exp*_tmp4C0;void*_tmp4C1;
struct Cyc_Absyn_Exp*_tmp4C2;void*_tmp4C3;void*_tmp4C4;struct Cyc_Absyn_Exp*
_tmp4C5;void*_tmp4C6;void*_tmp4C7;void*_tmp4C8;struct Cyc_Absyn_Exp*_tmp4C9;void*
_tmp4CA;void*_tmp4CB;struct Cyc_Absyn_Exp*_tmp4CC;void*_tmp4CD;void*_tmp4CE;void*
_tmp4CF;void*_tmp4D0;void*_tmp4D1;void*_tmp4D2;void*_tmp4D3;void*_tmp4D4;void*
_tmp4D5;void*_tmp4D6;void*_tmp4D7;void*_tmp4D8;void*_tmp4D9;void*_tmp4DA;_LL2A6:
_tmp4BD=_tmp4BC.f1;if(_tmp4BD <= (void*)2)goto _LL2A8;if(*((int*)_tmp4BD)!= 0)goto
_LL2A8;_tmp4BE=((struct Cyc_Absyn_Upper_b_struct*)_tmp4BD)->f1;_tmp4BF=_tmp4BC.f2;
if(_tmp4BF <= (void*)2)goto _LL2A8;if(*((int*)_tmp4BF)!= 0)goto _LL2A8;_tmp4C0=((
struct Cyc_Absyn_Upper_b_struct*)_tmp4BF)->f1;_LL2A7: if(_tmp4B5  && !_tmp4B6){if(
Cyc_Toc_is_toplevel(nv)){const char*_tmpA42;void*_tmpA41;(_tmpA41=0,((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(((_tmpA42="can't do NULL-check conversion at top-level",
_tag_dynforward(_tmpA42,sizeof(char),_get_zero_arr_size_char(_tmpA42,44)))),
_tag_dynforward(_tmpA41,sizeof(void*),0)));}if(_tmp410 != (void*)2){const char*
_tmpA46;void*_tmpA45[1];struct Cyc_String_pa_struct _tmpA44;(_tmpA44.tag=0,((
_tmpA44.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpA45[0]=& _tmpA44,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmpA46="null-check conversion mis-classified: %s",
_tag_dynforward(_tmpA46,sizeof(char),_get_zero_arr_size_char(_tmpA46,41)))),
_tag_dynforward(_tmpA45,sizeof(void*),1)))))));}{int do_null_check=Cyc_Toc_need_null_check(
_tmp40E);if(do_null_check){if(!_tmp40F){const char*_tmpA49;void*_tmpA48;(_tmpA48=
0,Cyc_Tcutil_warn(e->loc,((_tmpA49="inserted null check due to implicit cast from * to @ type",
_tag_dynforward(_tmpA49,sizeof(char),_get_zero_arr_size_char(_tmpA49,58)))),
_tag_dynforward(_tmpA48,sizeof(void*),0)));}{struct Cyc_List_List*_tmpA4A;(void*)(
e->r=(void*)Cyc_Toc_cast_it_r(*_tmp40D,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((
_tmpA4A=_cycalloc(sizeof(*_tmpA4A)),((_tmpA4A->hd=_tmp40E,((_tmpA4A->tl=0,
_tmpA4A)))))),0)));}}}}goto _LL2A5;_LL2A8: _tmp4C1=_tmp4BC.f1;if(_tmp4C1 <= (void*)
2)goto _LL2AA;if(*((int*)_tmp4C1)!= 0)goto _LL2AA;_tmp4C2=((struct Cyc_Absyn_Upper_b_struct*)
_tmp4C1)->f1;_tmp4C3=_tmp4BC.f2;if((int)_tmp4C3 != 0)goto _LL2AA;_LL2A9:
p2_forward_only=1;_tmp4C5=_tmp4C2;goto _LL2AB;_LL2AA: _tmp4C4=_tmp4BC.f1;if(
_tmp4C4 <= (void*)2)goto _LL2AC;if(*((int*)_tmp4C4)!= 0)goto _LL2AC;_tmp4C5=((
struct Cyc_Absyn_Upper_b_struct*)_tmp4C4)->f1;_tmp4C6=_tmp4BC.f2;if((int)_tmp4C6
!= 1)goto _LL2AC;_LL2AB: if(_tmp410 == (void*)2){const char*_tmpA4E;void*_tmpA4D[1];
struct Cyc_String_pa_struct _tmpA4C;(_tmpA4C.tag=0,((_tmpA4C.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e)),((_tmpA4D[0]=&
_tmpA4C,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmpA4E="conversion mis-classified as null-check: %s",_tag_dynforward(_tmpA4E,
sizeof(char),_get_zero_arr_size_char(_tmpA4E,44)))),_tag_dynforward(_tmpA4D,
sizeof(void*),1)))))));}if(Cyc_Toc_is_toplevel(nv)){if((_tmp4B9  && !(_tmp4B1.elt_tq).real_const)
 && !_tmp4BA)_tmp4C5=Cyc_Absyn_prim2_exp((void*)2,_tmp4C5,Cyc_Absyn_uint_exp(1,0),
0);(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp4C5,
_tmp40E,p2_forward_only))->r));}else{struct Cyc_Absyn_Exp*_tmp4E6=p2_forward_only?
Cyc_Toc__tag_dynforward_e: Cyc_Toc__tag_dyneither_e;if(_tmp4B9){struct _tuple1*
_tmp4E7=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp4E8=Cyc_Absyn_var_exp(_tmp4E7,
0);struct Cyc_Absyn_Exp*_tmpA4F[2];struct Cyc_Absyn_Exp*_tmp4E9=Cyc_Absyn_fncall_exp(
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp40E),((
_tmpA4F[1]=_tmp4C5,((_tmpA4F[0]=_tmp4E8,((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmpA4F,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0);if(!_tmp4BA  && !(_tmp4B1.elt_tq).real_const)_tmp4E9=Cyc_Absyn_prim2_exp((
void*)2,_tmp4E9,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp4EA=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c((void*)_tmp4B1.elt_typ),0);struct Cyc_Absyn_Exp*_tmpA50[3];
struct Cyc_Absyn_Exp*_tmp4EB=Cyc_Absyn_fncall_exp(_tmp4E6,((_tmpA50[2]=_tmp4E9,((
_tmpA50[1]=_tmp4EA,((_tmpA50[0]=_tmp4E8,((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmpA50,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);struct Cyc_Absyn_Stmt*_tmp4EC=Cyc_Absyn_exp_stmt(_tmp4EB,0);_tmp4EC=
Cyc_Absyn_declare_stmt(_tmp4E7,Cyc_Toc_typ_to_c(old_t2),(struct Cyc_Absyn_Exp*)
_tmp40E,_tmp4EC,0);(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(_tmp4EC));}}else{
struct Cyc_Absyn_Exp*_tmpA51[3];(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp4E6,((
_tmpA51[2]=_tmp4C5,((_tmpA51[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((void*)
_tmp4B1.elt_typ),0),((_tmpA51[0]=_tmp40E,((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmpA51,sizeof(struct Cyc_Absyn_Exp*),
3))))))))));}}goto _LL2A5;_LL2AC: _tmp4C7=_tmp4BC.f1;if((int)_tmp4C7 != 0)goto
_LL2AE;_tmp4C8=_tmp4BC.f2;if(_tmp4C8 <= (void*)2)goto _LL2AE;if(*((int*)_tmp4C8)!= 
0)goto _LL2AE;_tmp4C9=((struct Cyc_Absyn_Upper_b_struct*)_tmp4C8)->f1;_LL2AD:
p1_forward_only=1;_tmp4CC=_tmp4C9;goto _LL2AF;_LL2AE: _tmp4CA=_tmp4BC.f1;if((int)
_tmp4CA != 1)goto _LL2B0;_tmp4CB=_tmp4BC.f2;if(_tmp4CB <= (void*)2)goto _LL2B0;if(*((
int*)_tmp4CB)!= 0)goto _LL2B0;_tmp4CC=((struct Cyc_Absyn_Upper_b_struct*)_tmp4CB)->f1;
_LL2AF: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpA54;void*_tmpA53;(_tmpA53=0,((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(((
_tmpA54="can't coerce t? to t* or t@ at the top-level",_tag_dynforward(_tmpA54,
sizeof(char),_get_zero_arr_size_char(_tmpA54,45)))),_tag_dynforward(_tmpA53,
sizeof(void*),0)));}{struct Cyc_Absyn_Exp*_tmp4F2=_tmp4CC;if(_tmp4B9  && !_tmp4BA)
_tmp4F2=Cyc_Absyn_add_exp(_tmp4CC,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*
_tmp4F3=p1_forward_only?Cyc_Toc__untag_dynforward_ptr_e: Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpA55[3];struct Cyc_Absyn_Exp*_tmp4F4=Cyc_Absyn_fncall_exp(
_tmp4F3,((_tmpA55[2]=_tmp4F2,((_tmpA55[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp4AF.elt_typ),0),((_tmpA55[0]=_tmp40E,((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmpA55,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);if(_tmp4B6){struct Cyc_List_List*_tmpA56;(void*)(_tmp4F4->r=(void*)
Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,((_tmpA56=_cycalloc(sizeof(*_tmpA56)),((
_tmpA56->hd=Cyc_Absyn_copy_exp(_tmp4F4),((_tmpA56->tl=0,_tmpA56))))))));}(void*)(
e->r=(void*)Cyc_Toc_cast_it_r(*_tmp40D,_tmp4F4));goto _LL2A5;}}_LL2B0: _tmp4CD=
_tmp4BC.f1;if((int)_tmp4CD != 0)goto _LL2B2;_tmp4CE=_tmp4BC.f2;if((int)_tmp4CE != 0)
goto _LL2B2;_LL2B1: p1_forward_only=1;p2_forward_only=1;goto DynCast;_LL2B2: _tmp4CF=
_tmp4BC.f1;if((int)_tmp4CF != 0)goto _LL2B4;_tmp4D0=_tmp4BC.f2;if((int)_tmp4D0 != 1)
goto _LL2B4;_LL2B3: p1_forward_only=1;goto DynCast;_LL2B4: _tmp4D1=_tmp4BC.f1;if((
int)_tmp4D1 != 1)goto _LL2B6;_tmp4D2=_tmp4BC.f2;if((int)_tmp4D2 != 0)goto _LL2B6;
_LL2B5: p2_forward_only=1;goto DynCast;_LL2B6: _tmp4D3=_tmp4BC.f1;if((int)_tmp4D3 != 
1)goto _LL2B8;_tmp4D4=_tmp4BC.f2;if((int)_tmp4D4 != 1)goto _LL2B8;_LL2B7: DynCast:
if((_tmp4B9  && !_tmp4BA) && !(_tmp4B1.elt_tq).real_const){if(Cyc_Toc_is_toplevel(
nv)){const char*_tmpA59;void*_tmpA58;(_tmpA58=0,((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_unimp)(((_tmpA59="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",
_tag_dynforward(_tmpA59,sizeof(char),_get_zero_arr_size_char(_tmpA59,70)))),
_tag_dynforward(_tmpA58,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*_tmp4F9=
p1_forward_only?Cyc_Toc__dynforward_ptr_decrease_size_e: Cyc_Toc__dyneither_ptr_decrease_size_e;{
struct Cyc_Absyn_Exp*_tmpA5A[3];(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp4F9,((
_tmpA5A[2]=Cyc_Absyn_uint_exp(1,0),((_tmpA5A[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp4AF.elt_typ),0),((_tmpA5A[0]=_tmp40E,((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmpA5A,sizeof(struct Cyc_Absyn_Exp*),
3))))))))));}if(p1_forward_only != p2_forward_only){if(p1_forward_only){struct Cyc_Absyn_Exp*
_tmpA5B[1];(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dynforward_to_dyneither_e,((
_tmpA5B[0]=Cyc_Absyn_copy_exp(e),((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmpA5B,sizeof(struct Cyc_Absyn_Exp*),1))))));}
else{struct Cyc_Absyn_Exp*_tmpA5C[1];(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_to_dynforward_e,((
_tmpA5C[0]=Cyc_Absyn_copy_exp(e),((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmpA5C,sizeof(struct Cyc_Absyn_Exp*),1))))));}}}}
else{if(p1_forward_only != p2_forward_only){if(Cyc_Toc_is_toplevel(nv)){const char*
_tmpA5F;void*_tmpA5E;(_tmpA5E=0,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(((_tmpA5F="can't coerce between ? and ?+- at toplevel",
_tag_dynforward(_tmpA5F,sizeof(char),_get_zero_arr_size_char(_tmpA5F,43)))),
_tag_dynforward(_tmpA5E,sizeof(void*),0)));}if(p1_forward_only){struct Cyc_Absyn_Exp*
_tmpA60[1];(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dynforward_to_dyneither_e,((
_tmpA60[0]=_tmp40E,((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmpA60,sizeof(struct Cyc_Absyn_Exp*),1))))));}else{struct Cyc_Absyn_Exp*
_tmpA61[1];(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_to_dynforward_e,((
_tmpA61[0]=_tmp40E,((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmpA61,sizeof(struct Cyc_Absyn_Exp*),1))))));}}}goto _LL2A5;
_LL2B8: _tmp4D5=_tmp4BC.f1;if(_tmp4D5 <= (void*)2)goto _LL2BA;if(*((int*)_tmp4D5)!= 
1)goto _LL2BA;_tmp4D6=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp4D5)->f1;
_tmp4D7=_tmp4BC.f2;if(_tmp4D7 <= (void*)2)goto _LL2BA;if(*((int*)_tmp4D7)!= 1)goto
_LL2BA;_tmp4D8=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)_tmp4D7)->f1;_LL2B9:
if(_tmp4B5  && !_tmp4B6){if(Cyc_Toc_is_toplevel(nv)){const char*_tmpA64;void*
_tmpA63;(_tmpA63=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_unimp)(((_tmpA64="can't do NULL-check conversion at top-level",
_tag_dynforward(_tmpA64,sizeof(char),_get_zero_arr_size_char(_tmpA64,44)))),
_tag_dynforward(_tmpA63,sizeof(void*),0)));}if(_tmp410 != (void*)2){const char*
_tmpA68;void*_tmpA67[1];struct Cyc_String_pa_struct _tmpA66;(_tmpA66.tag=0,((
_tmpA66.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e)),((_tmpA67[0]=& _tmpA66,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmpA68="null-check conversion mis-classified: %s",
_tag_dynforward(_tmpA68,sizeof(char),_get_zero_arr_size_char(_tmpA68,41)))),
_tag_dynforward(_tmpA67,sizeof(void*),1)))))));}{int do_null_check=Cyc_Toc_need_null_check(
_tmp40E);if(do_null_check){if(!_tmp40F){const char*_tmpA6B;void*_tmpA6A;(_tmpA6A=
0,Cyc_Tcutil_warn(e->loc,((_tmpA6B="inserted null check due to implicit cast from * to @ type",
_tag_dynforward(_tmpA6B,sizeof(char),_get_zero_arr_size_char(_tmpA6B,58)))),
_tag_dynforward(_tmpA6A,sizeof(void*),0)));}{struct Cyc_List_List*_tmpA6C;(void*)(
e->r=(void*)Cyc_Toc_cast_it_r(*_tmp40D,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((
_tmpA6C=_cycalloc(sizeof(*_tmpA6C)),((_tmpA6C->hd=_tmp40E,((_tmpA6C->tl=0,
_tmpA6C)))))),0)));}}}}goto _LL2A5;_LL2BA: _tmp4D9=_tmp4BC.f1;if(_tmp4D9 <= (void*)
2)goto _LL2BC;if(*((int*)_tmp4D9)!= 1)goto _LL2BC;_LL2BB: {const char*_tmpA70;void*
_tmpA6F[1];struct Cyc_String_pa_struct _tmpA6E;(_tmpA6E.tag=0,((_tmpA6E.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(_tmp40E->loc)),((
_tmpA6F[0]=& _tmpA6E,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_unimp)(((_tmpA70="%s: toc, cast from AbsUpper_b",_tag_dynforward(_tmpA70,
sizeof(char),_get_zero_arr_size_char(_tmpA70,30)))),_tag_dynforward(_tmpA6F,
sizeof(void*),1)))))));}_LL2BC: _tmp4DA=_tmp4BC.f2;if(_tmp4DA <= (void*)2)goto
_LL2A5;if(*((int*)_tmp4DA)!= 1)goto _LL2A5;_LL2BD: goto _LL2A5;_LL2A5:;}goto _LL29E;}
_LL2A1: _tmp4B2=_tmp4AD.f1;if(_tmp4B2 <= (void*)4)goto _LL2A3;if(*((int*)_tmp4B2)!= 
4)goto _LL2A3;_tmp4B3=((struct Cyc_Absyn_PointerType_struct*)_tmp4B2)->f1;_tmp4B4=
_tmp4AD.f2;if(_tmp4B4 <= (void*)4)goto _LL2A3;if(*((int*)_tmp4B4)!= 5)goto _LL2A3;
_LL2A2:{void*_tmp50C=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,(_tmp4B3.ptr_atts).bounds);
_LL2BF: if((int)_tmp50C != 0)goto _LL2C1;_LL2C0: goto _LL2C2;_LL2C1: if((int)_tmp50C != 
1)goto _LL2C3;_LL2C2:(void*)(_tmp40E->r=(void*)Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((
void*)_tmp40E->r,_tmp40E->loc),Cyc_Toc_curr_sp));goto _LL2BE;_LL2C3:;_LL2C4: goto
_LL2BE;_LL2BE:;}goto _LL29E;_LL2A3:;_LL2A4: goto _LL29E;_LL29E:;}goto _LL1E0;}_LL205:
if(*((int*)_tmp3E7)!= 16)goto _LL207;_tmp411=((struct Cyc_Absyn_Address_e_struct*)
_tmp3E7)->f1;_LL206:{void*_tmp50D=(void*)_tmp411->r;struct _tuple1*_tmp50E;struct
Cyc_List_List*_tmp50F;struct Cyc_List_List*_tmp510;struct Cyc_List_List*_tmp511;
_LL2C6: if(*((int*)_tmp50D)!= 30)goto _LL2C8;_tmp50E=((struct Cyc_Absyn_Struct_e_struct*)
_tmp50D)->f1;_tmp50F=((struct Cyc_Absyn_Struct_e_struct*)_tmp50D)->f2;_tmp510=((
struct Cyc_Absyn_Struct_e_struct*)_tmp50D)->f3;_LL2C7: if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpA74;void*_tmpA73[1];struct Cyc_String_pa_struct _tmpA72;(_tmpA72.tag=
0,((_tmpA72.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(
_tmp411->loc)),((_tmpA73[0]=& _tmpA72,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(((_tmpA74="%s: & on non-identifiers at the top-level",
_tag_dynforward(_tmpA74,sizeof(char),_get_zero_arr_size_char(_tmpA74,42)))),
_tag_dynforward(_tmpA73,sizeof(void*),1)))))));}(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(
nv,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp411->topt))->v,_tmp50F != 0,1,0,
_tmp510,_tmp50E))->r));goto _LL2C5;_LL2C8: if(*((int*)_tmp50D)!= 26)goto _LL2CA;
_tmp511=((struct Cyc_Absyn_Tuple_e_struct*)_tmp50D)->f1;_LL2C9: if(Cyc_Toc_is_toplevel(
nv)){const char*_tmpA78;void*_tmpA77[1];struct Cyc_String_pa_struct _tmpA76;(
_tmpA76.tag=0,((_tmpA76.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(
_tmp411->loc)),((_tmpA77[0]=& _tmpA76,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_unimp)(((_tmpA78="%s: & on non-identifiers at the top-level",
_tag_dynforward(_tmpA78,sizeof(char),_get_zero_arr_size_char(_tmpA78,42)))),
_tag_dynforward(_tmpA77,sizeof(void*),1)))))));}(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(
nv,1,0,_tmp511))->r));goto _LL2C5;_LL2CA:;_LL2CB: Cyc_Toc_exp_to_c(nv,_tmp411);if(
!Cyc_Absyn_is_lvalue(_tmp411)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(
_tmp411,0,Cyc_Toc_address_lvalue,1);(void*)(e->r=(void*)Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp411));}goto _LL2C5;
_LL2C5:;}goto _LL1E0;_LL207: if(*((int*)_tmp3E7)!= 17)goto _LL209;_tmp412=((struct
Cyc_Absyn_New_e_struct*)_tmp3E7)->f1;_tmp413=((struct Cyc_Absyn_New_e_struct*)
_tmp3E7)->f2;_LL208: if(Cyc_Toc_is_toplevel(nv)){const char*_tmpA7C;void*_tmpA7B[1];
struct Cyc_String_pa_struct _tmpA7A;(_tmpA7A.tag=0,((_tmpA7A.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(_tmp413->loc)),((
_tmpA7B[0]=& _tmpA7A,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_unimp)(((_tmpA7C="%s: new at top-level",_tag_dynforward(_tmpA7C,sizeof(
char),_get_zero_arr_size_char(_tmpA7C,21)))),_tag_dynforward(_tmpA7B,sizeof(void*),
1)))))));}{void*_tmp51B=(void*)_tmp413->r;struct Cyc_List_List*_tmp51C;struct Cyc_Absyn_Vardecl*
_tmp51D;struct Cyc_Absyn_Exp*_tmp51E;struct Cyc_Absyn_Exp*_tmp51F;int _tmp520;
struct _tuple1*_tmp521;struct Cyc_List_List*_tmp522;struct Cyc_List_List*_tmp523;
struct Cyc_List_List*_tmp524;_LL2CD: if(*((int*)_tmp51B)!= 28)goto _LL2CF;_tmp51C=((
struct Cyc_Absyn_Array_e_struct*)_tmp51B)->f1;_LL2CE: {struct _tuple1*_tmp525=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp526=Cyc_Absyn_var_exp(_tmp525,0);struct Cyc_Absyn_Stmt*
_tmp527=Cyc_Toc_init_array(nv,_tmp526,_tmp51C,Cyc_Absyn_exp_stmt(_tmp526,0));
void*old_elt_typ;{void*_tmp528=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp529;void*_tmp52A;struct Cyc_Absyn_Tqual _tmp52B;struct Cyc_Absyn_PtrAtts _tmp52C;
struct Cyc_Absyn_Conref*_tmp52D;_LL2D8: if(_tmp528 <= (void*)4)goto _LL2DA;if(*((int*)
_tmp528)!= 4)goto _LL2DA;_tmp529=((struct Cyc_Absyn_PointerType_struct*)_tmp528)->f1;
_tmp52A=(void*)_tmp529.elt_typ;_tmp52B=_tmp529.elt_tq;_tmp52C=_tmp529.ptr_atts;
_tmp52D=_tmp52C.zero_term;_LL2D9: old_elt_typ=_tmp52A;goto _LL2D7;_LL2DA:;_LL2DB: {
const char*_tmpA7F;void*_tmpA7E;old_elt_typ=((_tmpA7E=0,Cyc_Toc_toc_impos(((
_tmpA7F="exp_to_c:new array expression doesn't have ptr type",_tag_dynforward(
_tmpA7F,sizeof(char),_get_zero_arr_size_char(_tmpA7F,52)))),_tag_dynforward(
_tmpA7E,sizeof(void*),0))));}_LL2D7:;}{void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp530=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*
_tmp531=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),Cyc_Absyn_signed_int_exp(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp51C),0),0);struct Cyc_Absyn_Exp*
e1;if(_tmp412 == 0  || Cyc_Absyn_no_regions)e1=Cyc_Toc_malloc_exp(old_elt_typ,
_tmp531);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp412;Cyc_Toc_exp_to_c(
nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp531);}(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp525,_tmp530,(struct Cyc_Absyn_Exp*)e1,_tmp527,0)));
goto _LL2CC;}}_LL2CF: if(*((int*)_tmp51B)!= 29)goto _LL2D1;_tmp51D=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp51B)->f1;_tmp51E=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp51B)->f2;
_tmp51F=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp51B)->f3;_tmp520=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp51B)->f4;_LL2D0: {int is_dynforward_ptr=0;
int is_dyneither_ptr=0;{void*_tmp532=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp533;void*_tmp534;struct Cyc_Absyn_Tqual _tmp535;struct Cyc_Absyn_PtrAtts _tmp536;
struct Cyc_Absyn_Conref*_tmp537;struct Cyc_Absyn_Conref*_tmp538;_LL2DD: if(_tmp532
<= (void*)4)goto _LL2DF;if(*((int*)_tmp532)!= 4)goto _LL2DF;_tmp533=((struct Cyc_Absyn_PointerType_struct*)
_tmp532)->f1;_tmp534=(void*)_tmp533.elt_typ;_tmp535=_tmp533.elt_tq;_tmp536=
_tmp533.ptr_atts;_tmp537=_tmp536.bounds;_tmp538=_tmp536.zero_term;_LL2DE:
is_dynforward_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp537)== (void*)0;
is_dyneither_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp537)== (void*)1;
goto _LL2DC;_LL2DF:;_LL2E0: {const char*_tmpA82;void*_tmpA81;(_tmpA81=0,((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmpA82="exp_to_c: comprehension not an array type",
_tag_dynforward(_tmpA82,sizeof(char),_get_zero_arr_size_char(_tmpA82,42)))),
_tag_dynforward(_tmpA81,sizeof(void*),0)));}_LL2DC:;}{struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp51F->topt))->v;void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*
ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp51E);{
struct Cyc_Absyn_Exp*_tmp53B=Cyc_Absyn_var_exp(max,0);if(_tmp520)_tmp53B=Cyc_Absyn_add_exp(
_tmp53B,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(
nv,Cyc_Absyn_var_exp(a,0),_tmp51D,Cyc_Absyn_var_exp(max,0),_tmp51F,_tmp520,Cyc_Toc_skip_stmt_dl(),
1);{struct _RegionHandle _tmp53C=_new_region("r");struct _RegionHandle*r=& _tmp53C;
_push_region(r);{struct _tuple11*_tmpA85;struct Cyc_List_List*_tmpA84;struct Cyc_List_List*
decls=(_tmpA84=_region_malloc(r,sizeof(*_tmpA84)),((_tmpA84->hd=((_tmpA85=
_region_malloc(r,sizeof(*_tmpA85)),((_tmpA85->f1=max,((_tmpA85->f2=Cyc_Absyn_uint_typ,((
_tmpA85->f3=(struct Cyc_Absyn_Exp*)_tmp51E,_tmpA85)))))))),((_tmpA84->tl=0,
_tmpA84)))));struct Cyc_Absyn_Exp*ai;if(_tmp412 == 0  || Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*_tmpA86[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,((_tmpA86[1]=_tmp53B,((_tmpA86[0]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmpA86,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{struct Cyc_Absyn_Exp*
r=(struct Cyc_Absyn_Exp*)_tmp412;Cyc_Toc_exp_to_c(nv,r);{struct Cyc_Absyn_Exp*
_tmpA87[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,((
_tmpA87[1]=_tmp53B,((_tmpA87[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmpA87,sizeof(struct Cyc_Absyn_Exp*),
2)))))),0));}}{struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);{struct
_tuple11*_tmpA8A;struct Cyc_List_List*_tmpA89;decls=((_tmpA89=_region_malloc(r,
sizeof(*_tmpA89)),((_tmpA89->hd=((_tmpA8A=_region_malloc(r,sizeof(*_tmpA8A)),((
_tmpA8A->f1=a,((_tmpA8A->f2=ptr_typ,((_tmpA8A->f3=(struct Cyc_Absyn_Exp*)ainit,
_tmpA8A)))))))),((_tmpA89->tl=decls,_tmpA89))))));}if(is_dynforward_ptr  || 
is_dyneither_ptr){struct _tuple1*_tmp541=Cyc_Toc_temp_var();void*_tmp542=Cyc_Toc_typ_to_c(
old_typ);struct Cyc_Absyn_Exp*_tmp543=is_dynforward_ptr?Cyc_Toc__tag_dynforward_e:
Cyc_Toc__tag_dyneither_e;struct Cyc_Absyn_Exp*_tmpA8B[3];struct Cyc_Absyn_Exp*
_tmp544=Cyc_Absyn_fncall_exp(_tmp543,((_tmpA8B[2]=_tmp53B,((_tmpA8B[1]=Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),((_tmpA8B[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmpA8B,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);{struct _tuple11*_tmpA8E;struct Cyc_List_List*_tmpA8D;decls=((_tmpA8D=
_region_malloc(r,sizeof(*_tmpA8D)),((_tmpA8D->hd=((_tmpA8E=_region_malloc(r,
sizeof(*_tmpA8E)),((_tmpA8E->f1=_tmp541,((_tmpA8E->f2=_tmp542,((_tmpA8E->f3=(
struct Cyc_Absyn_Exp*)_tmp544,_tmpA8E)))))))),((_tmpA8D->tl=decls,_tmpA8D))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp541,0),0),0);}
else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}{
struct Cyc_List_List*_tmp548=decls;for(0;_tmp548 != 0;_tmp548=_tmp548->tl){struct
_tuple1*_tmp54A;void*_tmp54B;struct Cyc_Absyn_Exp*_tmp54C;struct _tuple11 _tmp549=*((
struct _tuple11*)_tmp548->hd);_tmp54A=_tmp549.f1;_tmp54B=_tmp549.f2;_tmp54C=
_tmp549.f3;s=Cyc_Absyn_declare_stmt(_tmp54A,_tmp54B,_tmp54C,s,0);}}(void*)(e->r=(
void*)Cyc_Toc_stmt_exp_r(s));}};_pop_region(r);}goto _LL2CC;}}}}_LL2D1: if(*((int*)
_tmp51B)!= 30)goto _LL2D3;_tmp521=((struct Cyc_Absyn_Struct_e_struct*)_tmp51B)->f1;
_tmp522=((struct Cyc_Absyn_Struct_e_struct*)_tmp51B)->f2;_tmp523=((struct Cyc_Absyn_Struct_e_struct*)
_tmp51B)->f3;_LL2D2:(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(nv,(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp413->topt))->v,_tmp522 != 0,1,_tmp412,_tmp523,
_tmp521))->r));goto _LL2CC;_LL2D3: if(*((int*)_tmp51B)!= 26)goto _LL2D5;_tmp524=((
struct Cyc_Absyn_Tuple_e_struct*)_tmp51B)->f1;_LL2D4:(void*)(e->r=(void*)((void*)(
Cyc_Toc_init_tuple(nv,1,_tmp412,_tmp524))->r));goto _LL2CC;_LL2D5:;_LL2D6: {void*
old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp413->topt))->v;void*
elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct _tuple1*_tmp54F=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp550=Cyc_Absyn_var_exp(_tmp54F,0);struct Cyc_Absyn_Exp*
mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp550,0),0);struct Cyc_Absyn_Exp*
inner_mexp=mexp;if(_tmp412 == 0  || Cyc_Absyn_no_regions)mexp=Cyc_Toc_malloc_exp(
old_elt_typ,mexp);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp412;Cyc_Toc_exp_to_c(
nv,r);mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;int forward_only=0;{void*
_tmp551=(void*)_tmp413->r;void*_tmp552;struct Cyc_Absyn_Exp*_tmp553;_LL2E2: if(*((
int*)_tmp551)!= 15)goto _LL2E4;_tmp552=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp551)->f1;_tmp553=((struct Cyc_Absyn_Cast_e_struct*)_tmp551)->f2;_LL2E3:{
struct _tuple0 _tmpA8F;struct _tuple0 _tmp555=(_tmpA8F.f1=Cyc_Tcutil_compress(
_tmp552),((_tmpA8F.f2=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp553->topt))->v),_tmpA8F)));void*_tmp556;struct Cyc_Absyn_PtrInfo
_tmp557;void*_tmp558;struct Cyc_Absyn_PtrAtts _tmp559;struct Cyc_Absyn_Conref*
_tmp55A;void*_tmp55B;struct Cyc_Absyn_PtrInfo _tmp55C;struct Cyc_Absyn_PtrAtts
_tmp55D;struct Cyc_Absyn_Conref*_tmp55E;_LL2E7: _tmp556=_tmp555.f1;if(_tmp556 <= (
void*)4)goto _LL2E9;if(*((int*)_tmp556)!= 4)goto _LL2E9;_tmp557=((struct Cyc_Absyn_PointerType_struct*)
_tmp556)->f1;_tmp558=(void*)_tmp557.elt_typ;_tmp559=_tmp557.ptr_atts;_tmp55A=
_tmp559.bounds;_tmp55B=_tmp555.f2;if(_tmp55B <= (void*)4)goto _LL2E9;if(*((int*)
_tmp55B)!= 4)goto _LL2E9;_tmp55C=((struct Cyc_Absyn_PointerType_struct*)_tmp55B)->f1;
_tmp55D=_tmp55C.ptr_atts;_tmp55E=_tmp55D.bounds;_LL2E8:{struct _tuple0 _tmpA90;
struct _tuple0 _tmp560=(_tmpA90.f1=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp55A),((_tmpA90.f2=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp55E),_tmpA90)));
void*_tmp561;void*_tmp562;struct Cyc_Absyn_Exp*_tmp563;void*_tmp564;void*_tmp565;
struct Cyc_Absyn_Exp*_tmp566;_LL2EC: _tmp561=_tmp560.f1;if((int)_tmp561 != 0)goto
_LL2EE;_tmp562=_tmp560.f2;if(_tmp562 <= (void*)2)goto _LL2EE;if(*((int*)_tmp562)!= 
0)goto _LL2EE;_tmp563=((struct Cyc_Absyn_Upper_b_struct*)_tmp562)->f1;_LL2ED:
forward_only=1;_tmp566=_tmp563;goto _LL2EF;_LL2EE: _tmp564=_tmp560.f1;if((int)
_tmp564 != 1)goto _LL2F0;_tmp565=_tmp560.f2;if(_tmp565 <= (void*)2)goto _LL2F0;if(*((
int*)_tmp565)!= 0)goto _LL2F0;_tmp566=((struct Cyc_Absyn_Upper_b_struct*)_tmp565)->f1;
_LL2EF: Cyc_Toc_exp_to_c(nv,_tmp553);(void*)(inner_mexp->r=(void*)Cyc_Toc_sizeoftyp_exp_r(
elt_typ));done=1;{struct Cyc_Absyn_Exp*_tmp567=forward_only?Cyc_Toc__init_dynforward_ptr_e:
Cyc_Toc__init_dyneither_ptr_e;{struct Cyc_Absyn_Exp*_tmpA91[4];(void*)(e->r=(void*)
Cyc_Toc_fncall_exp_r(_tmp567,((_tmpA91[3]=_tmp566,((_tmpA91[2]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp558),0),((_tmpA91[1]=_tmp553,((_tmpA91[0]=mexp,((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmpA91,sizeof(struct Cyc_Absyn_Exp*),
4))))))))))));}goto _LL2EB;}_LL2F0:;_LL2F1: goto _LL2EB;_LL2EB:;}goto _LL2E6;_LL2E9:;
_LL2EA: goto _LL2E6;_LL2E6:;}goto _LL2E1;_LL2E4:;_LL2E5: goto _LL2E1;_LL2E1:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp569=Cyc_Absyn_exp_stmt(_tmp550,0);struct Cyc_Absyn_Exp*
_tmp56A=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp413);_tmp569=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp550,_tmp56A,0),_tmp413,0),
_tmp569,0);{void*_tmp56B=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);(void*)(e->r=(
void*)Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp54F,_tmp56B,(struct Cyc_Absyn_Exp*)
mexp,_tmp569,0)));}}goto _LL2CC;}}_LL2CC:;}goto _LL1E0;_LL209: if(*((int*)_tmp3E7)
!= 19)goto _LL20B;_tmp414=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp3E7)->f1;
_LL20A: Cyc_Toc_exp_to_c(nv,_tmp414);goto _LL1E0;_LL20B: if(*((int*)_tmp3E7)!= 18)
goto _LL20D;_tmp415=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3E7)->f1;
_LL20C:{struct Cyc_Absyn_Sizeoftyp_e_struct _tmpA94;struct Cyc_Absyn_Sizeoftyp_e_struct*
_tmpA93;(void*)(e->r=(void*)((void*)((_tmpA93=_cycalloc(sizeof(*_tmpA93)),((
_tmpA93[0]=((_tmpA94.tag=18,((_tmpA94.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp415),
_tmpA94)))),_tmpA93))))));}goto _LL1E0;_LL20D: if(*((int*)_tmp3E7)!= 21)goto _LL20F;
_LL20E: {const char*_tmpA97;void*_tmpA96;(_tmpA96=0,((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmpA97="__gen() in code generator",
_tag_dynforward(_tmpA97,sizeof(char),_get_zero_arr_size_char(_tmpA97,26)))),
_tag_dynforward(_tmpA96,sizeof(void*),0)));}_LL20F: if(*((int*)_tmp3E7)!= 20)goto
_LL211;_tmp416=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp3E7)->f1;_tmp417=(
void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp3E7)->f2;if(*((int*)_tmp417)!= 0)
goto _LL211;_tmp418=((struct Cyc_Absyn_StructField_struct*)_tmp417)->f1;_LL210:{
struct Cyc_Absyn_Offsetof_e_struct _tmpAA1;struct Cyc_Absyn_StructField_struct
_tmpAA0;struct Cyc_Absyn_StructField_struct*_tmpA9F;struct Cyc_Absyn_Offsetof_e_struct*
_tmpA9E;(void*)(e->r=(void*)((void*)((_tmpA9E=_cycalloc(sizeof(*_tmpA9E)),((
_tmpA9E[0]=((_tmpAA1.tag=20,((_tmpAA1.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp416),((
_tmpAA1.f2=(void*)((void*)((_tmpA9F=_cycalloc(sizeof(*_tmpA9F)),((_tmpA9F[0]=((
_tmpAA0.tag=0,((_tmpAA0.f1=_tmp418,_tmpAA0)))),_tmpA9F))))),_tmpAA1)))))),
_tmpA9E))))));}goto _LL1E0;_LL211: if(*((int*)_tmp3E7)!= 20)goto _LL213;_tmp419=(
void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp3E7)->f1;_tmp41A=(void*)((struct
Cyc_Absyn_Offsetof_e_struct*)_tmp3E7)->f2;if(*((int*)_tmp41A)!= 1)goto _LL213;
_tmp41B=((struct Cyc_Absyn_TupleIndex_struct*)_tmp41A)->f1;_LL212:{void*_tmp574=
Cyc_Tcutil_compress(_tmp419);struct Cyc_Absyn_AggrInfo _tmp575;union Cyc_Absyn_AggrInfoU_union
_tmp576;struct Cyc_List_List*_tmp577;_LL2F3: if(_tmp574 <= (void*)4)goto _LL2FB;if(*((
int*)_tmp574)!= 10)goto _LL2F5;_tmp575=((struct Cyc_Absyn_AggrType_struct*)_tmp574)->f1;
_tmp576=_tmp575.aggr_info;_LL2F4: {struct Cyc_Absyn_Aggrdecl*_tmp578=Cyc_Absyn_get_known_aggrdecl(
_tmp576);if(_tmp578->impl == 0){const char*_tmpAA4;void*_tmpAA3;(_tmpAA3=0,((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmpAA4="struct fields must be known",
_tag_dynforward(_tmpAA4,sizeof(char),_get_zero_arr_size_char(_tmpAA4,28)))),
_tag_dynforward(_tmpAA3,sizeof(void*),0)));}_tmp577=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp578->impl))->fields;goto _LL2F6;}_LL2F5: if(*((int*)_tmp574)!= 11)
goto _LL2F7;_tmp577=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp574)->f2;_LL2F6: {
struct Cyc_Absyn_Aggrfield*_tmp57B=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*
x,int n))Cyc_List_nth)(_tmp577,(int)_tmp41B);{struct Cyc_Absyn_Offsetof_e_struct
_tmpAAE;struct Cyc_Absyn_StructField_struct _tmpAAD;struct Cyc_Absyn_StructField_struct*
_tmpAAC;struct Cyc_Absyn_Offsetof_e_struct*_tmpAAB;(void*)(e->r=(void*)((void*)((
_tmpAAB=_cycalloc(sizeof(*_tmpAAB)),((_tmpAAB[0]=((_tmpAAE.tag=20,((_tmpAAE.f1=(
void*)Cyc_Toc_typ_to_c_array(_tmp419),((_tmpAAE.f2=(void*)((void*)((_tmpAAC=
_cycalloc(sizeof(*_tmpAAC)),((_tmpAAC[0]=((_tmpAAD.tag=0,((_tmpAAD.f1=_tmp57B->name,
_tmpAAD)))),_tmpAAC))))),_tmpAAE)))))),_tmpAAB))))));}goto _LL2F2;}_LL2F7: if(*((
int*)_tmp574)!= 9)goto _LL2F9;_LL2F8:{struct Cyc_Absyn_Offsetof_e_struct _tmpAB8;
struct Cyc_Absyn_StructField_struct _tmpAB7;struct Cyc_Absyn_StructField_struct*
_tmpAB6;struct Cyc_Absyn_Offsetof_e_struct*_tmpAB5;(void*)(e->r=(void*)((void*)((
_tmpAB5=_cycalloc(sizeof(*_tmpAB5)),((_tmpAB5[0]=((_tmpAB8.tag=20,((_tmpAB8.f1=(
void*)Cyc_Toc_typ_to_c_array(_tmp419),((_tmpAB8.f2=(void*)((void*)((_tmpAB6=
_cycalloc(sizeof(*_tmpAB6)),((_tmpAB6[0]=((_tmpAB7.tag=0,((_tmpAB7.f1=Cyc_Absyn_fieldname((
int)(_tmp41B + 1)),_tmpAB7)))),_tmpAB6))))),_tmpAB8)))))),_tmpAB5))))));}goto
_LL2F2;_LL2F9: if(*((int*)_tmp574)!= 3)goto _LL2FB;_LL2FA: if(_tmp41B == 0){struct
Cyc_Absyn_Offsetof_e_struct _tmpAC2;struct Cyc_Absyn_StructField_struct _tmpAC1;
struct Cyc_Absyn_StructField_struct*_tmpAC0;struct Cyc_Absyn_Offsetof_e_struct*
_tmpABF;(void*)(e->r=(void*)((void*)((_tmpABF=_cycalloc(sizeof(*_tmpABF)),((
_tmpABF[0]=((_tmpAC2.tag=20,((_tmpAC2.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp419),((
_tmpAC2.f2=(void*)((void*)((_tmpAC0=_cycalloc(sizeof(*_tmpAC0)),((_tmpAC0[0]=((
_tmpAC1.tag=0,((_tmpAC1.f1=Cyc_Toc_tag_sp,_tmpAC1)))),_tmpAC0))))),_tmpAC2)))))),
_tmpABF))))));}else{struct Cyc_Absyn_Offsetof_e_struct _tmpACC;struct Cyc_Absyn_StructField_struct
_tmpACB;struct Cyc_Absyn_StructField_struct*_tmpACA;struct Cyc_Absyn_Offsetof_e_struct*
_tmpAC9;(void*)(e->r=(void*)((void*)((_tmpAC9=_cycalloc(sizeof(*_tmpAC9)),((
_tmpAC9[0]=((_tmpACC.tag=20,((_tmpACC.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp419),((
_tmpACC.f2=(void*)((void*)((_tmpACA=_cycalloc(sizeof(*_tmpACA)),((_tmpACA[0]=((
_tmpACB.tag=0,((_tmpACB.f1=Cyc_Absyn_fieldname((int)_tmp41B),_tmpACB)))),_tmpACA))))),
_tmpACC)))))),_tmpAC9))))));}goto _LL2F2;_LL2FB:;_LL2FC: {const char*_tmpACF;void*
_tmpACE;(_tmpACE=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(((_tmpACF="impossible type for offsetof tuple index",
_tag_dynforward(_tmpACF,sizeof(char),_get_zero_arr_size_char(_tmpACF,41)))),
_tag_dynforward(_tmpACE,sizeof(void*),0)));}_LL2F2:;}goto _LL1E0;_LL213: if(*((int*)
_tmp3E7)!= 22)goto _LL215;_tmp41C=((struct Cyc_Absyn_Deref_e_struct*)_tmp3E7)->f1;
_LL214: {void*_tmp58E=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp41C->topt))->v);{void*_tmp58F=_tmp58E;struct Cyc_Absyn_PtrInfo
_tmp590;void*_tmp591;struct Cyc_Absyn_Tqual _tmp592;struct Cyc_Absyn_PtrAtts _tmp593;
void*_tmp594;struct Cyc_Absyn_Conref*_tmp595;struct Cyc_Absyn_Conref*_tmp596;
struct Cyc_Absyn_Conref*_tmp597;_LL2FE: if(_tmp58F <= (void*)4)goto _LL300;if(*((int*)
_tmp58F)!= 4)goto _LL300;_tmp590=((struct Cyc_Absyn_PointerType_struct*)_tmp58F)->f1;
_tmp591=(void*)_tmp590.elt_typ;_tmp592=_tmp590.elt_tq;_tmp593=_tmp590.ptr_atts;
_tmp594=(void*)_tmp593.rgn;_tmp595=_tmp593.nullable;_tmp596=_tmp593.bounds;
_tmp597=_tmp593.zero_term;_LL2FF:{void*_tmp598=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp596);_LL303: if(_tmp598 <= (void*)2)goto _LL305;if(*((int*)_tmp598)!= 0)goto
_LL305;_LL304: {int do_null_check=Cyc_Toc_need_null_check(_tmp41C);Cyc_Toc_exp_to_c(
nv,_tmp41C);if(do_null_check){if(Cyc_Toc_warn_all_null_deref){const char*_tmpAD2;
void*_tmpAD1;(_tmpAD1=0,Cyc_Tcutil_warn(e->loc,((_tmpAD2="inserted null check due to dereference",
_tag_dynforward(_tmpAD2,sizeof(char),_get_zero_arr_size_char(_tmpAD2,39)))),
_tag_dynforward(_tmpAD1,sizeof(void*),0)));}{struct Cyc_List_List*_tmpAD3;(void*)(
_tmp41C->r=(void*)Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp58E),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,((_tmpAD3=_cycalloc(sizeof(*_tmpAD3)),((_tmpAD3->hd=Cyc_Absyn_copy_exp(
_tmp41C),((_tmpAD3->tl=0,_tmpAD3)))))),0)));}}goto _LL302;}_LL305: if((int)_tmp598
!= 0)goto _LL307;_LL306: goto _LL308;_LL307: if((int)_tmp598 != 1)goto _LL309;_LL308: {
struct Cyc_Absyn_Exp*_tmp59C=Cyc_Absyn_uint_exp(0,0);{struct Cyc_Core_Opt*_tmpAD4;
_tmp59C->topt=((_tmpAD4=_cycalloc(sizeof(*_tmpAD4)),((_tmpAD4->v=(void*)Cyc_Absyn_uint_typ,
_tmpAD4))));}(void*)(e->r=(void*)Cyc_Toc_subscript_exp_r(_tmp41C,_tmp59C));Cyc_Toc_exp_to_c(
nv,e);goto _LL302;}_LL309: if(_tmp598 <= (void*)2)goto _LL302;if(*((int*)_tmp598)!= 
1)goto _LL302;_LL30A: {const char*_tmpAD7;void*_tmpAD6;(_tmpAD6=0,((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(((_tmpAD7="exp_to_c: deref w/ AbsUpper_b",
_tag_dynforward(_tmpAD7,sizeof(char),_get_zero_arr_size_char(_tmpAD7,30)))),
_tag_dynforward(_tmpAD6,sizeof(void*),0)));}_LL302:;}goto _LL2FD;_LL300:;_LL301: {
const char*_tmpADA;void*_tmpAD9;(_tmpAD9=0,((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmpADA="exp_to_c: Deref: non-pointer",
_tag_dynforward(_tmpADA,sizeof(char),_get_zero_arr_size_char(_tmpADA,29)))),
_tag_dynforward(_tmpAD9,sizeof(void*),0)));}_LL2FD:;}goto _LL1E0;}_LL215: if(*((
int*)_tmp3E7)!= 23)goto _LL217;_tmp41D=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp3E7)->f1;_tmp41E=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp3E7)->f2;_LL216:
Cyc_Toc_exp_to_c(nv,_tmp41D);if(Cyc_Toc_is_poly_project(e))(void*)(e->r=(void*)((
void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL1E0;
_LL217: if(*((int*)_tmp3E7)!= 24)goto _LL219;_tmp41F=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp3E7)->f1;_tmp420=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp3E7)->f2;_LL218: {
void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp41F->topt))->v);
int do_null_check=Cyc_Toc_need_null_check(_tmp41F);Cyc_Toc_exp_to_c(nv,_tmp41F);{
int is_poly=Cyc_Toc_is_poly_project(e);int forward_only=0;void*_tmp5A3;struct Cyc_Absyn_Tqual
_tmp5A4;struct Cyc_Absyn_PtrAtts _tmp5A5;void*_tmp5A6;struct Cyc_Absyn_Conref*
_tmp5A7;struct Cyc_Absyn_Conref*_tmp5A8;struct Cyc_Absyn_Conref*_tmp5A9;struct Cyc_Absyn_PtrInfo
_tmp5A2=Cyc_Toc_get_ptr_type(e1typ);_tmp5A3=(void*)_tmp5A2.elt_typ;_tmp5A4=
_tmp5A2.elt_tq;_tmp5A5=_tmp5A2.ptr_atts;_tmp5A6=(void*)_tmp5A5.rgn;_tmp5A7=
_tmp5A5.nullable;_tmp5A8=_tmp5A5.bounds;_tmp5A9=_tmp5A5.zero_term;{void*_tmp5AA=
Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp5A8);struct Cyc_Absyn_Exp*_tmp5AB;
_LL30C: if(_tmp5AA <= (void*)2)goto _LL30E;if(*((int*)_tmp5AA)!= 0)goto _LL30E;
_tmp5AB=((struct Cyc_Absyn_Upper_b_struct*)_tmp5AA)->f1;_LL30D: {unsigned int
_tmp5AD;int _tmp5AE;struct _tuple5 _tmp5AC=Cyc_Evexp_eval_const_uint_exp(_tmp5AB);
_tmp5AD=_tmp5AC.f1;_tmp5AE=_tmp5AC.f2;if(_tmp5AE){if(_tmp5AD < 1){const char*
_tmpADD;void*_tmpADC;(_tmpADC=0,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmpADD="exp_to_c:  AggrArrow_e on pointer of size 0",
_tag_dynforward(_tmpADD,sizeof(char),_get_zero_arr_size_char(_tmpADD,44)))),
_tag_dynforward(_tmpADC,sizeof(void*),0)));}if(do_null_check){if(Cyc_Toc_warn_all_null_deref){
const char*_tmpAE0;void*_tmpADF;(_tmpADF=0,Cyc_Tcutil_warn(e->loc,((_tmpAE0="inserted null check due to dereference",
_tag_dynforward(_tmpAE0,sizeof(char),_get_zero_arr_size_char(_tmpAE0,39)))),
_tag_dynforward(_tmpADF,sizeof(void*),0)));}{struct Cyc_List_List*_tmpAE1;(void*)(
e->r=(void*)Cyc_Toc_aggrarrow_exp_r(Cyc_Toc_cast_it(Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp41F->topt))->v),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,((
_tmpAE1=_cycalloc(sizeof(*_tmpAE1)),((_tmpAE1->hd=_tmp41F,((_tmpAE1->tl=0,
_tmpAE1)))))),0)),_tmp420));}}}else{struct Cyc_Absyn_Exp*_tmpAE2[4];(void*)(e->r=(
void*)Cyc_Toc_aggrarrow_exp_r(Cyc_Toc_cast_it(Cyc_Toc_typ_to_c((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp41F->topt))->v),Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,((
_tmpAE2[3]=Cyc_Absyn_uint_exp(0,0),((_tmpAE2[2]=Cyc_Absyn_sizeoftyp_exp(_tmp5A3,
0),((_tmpAE2[1]=_tmp5AB,((_tmpAE2[0]=_tmp41F,((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmpAE2,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))),0)),_tmp420));}goto _LL30B;}_LL30E: if((int)_tmp5AA != 0)goto _LL310;
_LL30F: forward_only=1;goto _LL311;_LL310: if((int)_tmp5AA != 1)goto _LL312;_LL311: {
struct Cyc_Absyn_Exp*_tmp5B5=forward_only?Cyc_Toc__check_dynforward_subscript_e:
Cyc_Toc__check_dyneither_subscript_e;void*ta1=Cyc_Toc_typ_to_c_array(_tmp5A3);{
struct Cyc_Absyn_Exp*_tmpAE3[3];(void*)(_tmp41F->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(
ta1,_tmp5A4),Cyc_Absyn_fncall_exp(_tmp5B5,((_tmpAE3[2]=Cyc_Absyn_uint_exp(0,0),((
_tmpAE3[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpAE3[0]=Cyc_Absyn_copy_exp(
_tmp41F),((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmpAE3,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}goto _LL30B;}
_LL312: if(_tmp5AA <= (void*)2)goto _LL30B;if(*((int*)_tmp5AA)!= 1)goto _LL30B;
_LL313: {const char*_tmpAE6;void*_tmpAE5;(_tmpAE5=0,((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_unimp)(((_tmpAE6="exp_to_c: AggrArrow w/ AbsUpper_b",
_tag_dynforward(_tmpAE6,sizeof(char),_get_zero_arr_size_char(_tmpAE6,34)))),
_tag_dynforward(_tmpAE5,sizeof(void*),0)));}_LL30B:;}if(is_poly)(void*)(e->r=(
void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL1E0;}}
_LL219: if(*((int*)_tmp3E7)!= 25)goto _LL21B;_tmp421=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp3E7)->f1;_tmp422=((struct Cyc_Absyn_Subscript_e_struct*)_tmp3E7)->f2;_LL21A: {
void*_tmp5B9=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp421->topt))->v);{void*_tmp5BA=_tmp5B9;struct Cyc_List_List*_tmp5BB;struct Cyc_Absyn_PtrInfo
_tmp5BC;void*_tmp5BD;struct Cyc_Absyn_Tqual _tmp5BE;struct Cyc_Absyn_PtrAtts _tmp5BF;
void*_tmp5C0;struct Cyc_Absyn_Conref*_tmp5C1;struct Cyc_Absyn_Conref*_tmp5C2;
struct Cyc_Absyn_Conref*_tmp5C3;_LL315: if(_tmp5BA <= (void*)4)goto _LL319;if(*((int*)
_tmp5BA)!= 9)goto _LL317;_tmp5BB=((struct Cyc_Absyn_TupleType_struct*)_tmp5BA)->f1;
_LL316: Cyc_Toc_exp_to_c(nv,_tmp421);Cyc_Toc_exp_to_c(nv,_tmp422);{unsigned int
_tmp5C5;int _tmp5C6;struct _tuple5 _tmp5C4=Cyc_Evexp_eval_const_uint_exp(_tmp422);
_tmp5C5=_tmp5C4.f1;_tmp5C6=_tmp5C4.f2;if(!_tmp5C6){const char*_tmpAE9;void*
_tmpAE8;(_tmpAE8=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(((_tmpAE9="unknown tuple subscript in translation to C",
_tag_dynforward(_tmpAE9,sizeof(char),_get_zero_arr_size_char(_tmpAE9,44)))),
_tag_dynforward(_tmpAE8,sizeof(void*),0)));}(void*)(e->r=(void*)Cyc_Toc_aggrmember_exp_r(
_tmp421,Cyc_Absyn_fieldname((int)(_tmp5C5 + 1))));goto _LL314;}_LL317: if(*((int*)
_tmp5BA)!= 4)goto _LL319;_tmp5BC=((struct Cyc_Absyn_PointerType_struct*)_tmp5BA)->f1;
_tmp5BD=(void*)_tmp5BC.elt_typ;_tmp5BE=_tmp5BC.elt_tq;_tmp5BF=_tmp5BC.ptr_atts;
_tmp5C0=(void*)_tmp5BF.rgn;_tmp5C1=_tmp5BF.nullable;_tmp5C2=_tmp5BF.bounds;
_tmp5C3=_tmp5BF.zero_term;_LL318: {struct Cyc_List_List*_tmp5C9=Cyc_Toc_get_relns(
_tmp421);int in_bnds=0;{void*_tmp5CA=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp5C2);_LL31C: if(_tmp5CA <= (void*)2)goto _LL31E;if(*((int*)_tmp5CA)!= 1)goto
_LL31E;_LL31D: goto _LL31B;_LL31E:;_LL31F: in_bnds=Cyc_Toc_check_bounds(_tmp5C9,
_tmp421,_tmp422);if(Cyc_Toc_warn_bounds_checks  && !in_bnds){const char*_tmpAED;
void*_tmpAEC[1];struct Cyc_String_pa_struct _tmpAEB;(_tmpAEB.tag=0,((_tmpAEB.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(e)),((
_tmpAEC[0]=& _tmpAEB,Cyc_Tcutil_warn(e->loc,((_tmpAED="bounds check necessary for %s",
_tag_dynforward(_tmpAED,sizeof(char),_get_zero_arr_size_char(_tmpAED,30)))),
_tag_dynforward(_tmpAEC,sizeof(void*),1)))))));}_LL31B:;}Cyc_Toc_exp_to_c(nv,
_tmp421);Cyc_Toc_exp_to_c(nv,_tmp422);++ Cyc_Toc_total_bounds_checks;{int
forward_only=0;{void*_tmp5CE=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp5C2);
struct Cyc_Absyn_Exp*_tmp5CF;void*_tmp5D0;_LL321: if(_tmp5CE <= (void*)2)goto _LL323;
if(*((int*)_tmp5CE)!= 0)goto _LL323;_tmp5CF=((struct Cyc_Absyn_Upper_b_struct*)
_tmp5CE)->f1;_LL322: {int possibly_null=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp5C1);void*ta1=Cyc_Toc_typ_to_c(_tmp5BD);void*ta2=Cyc_Absyn_cstar_typ(
ta1,_tmp5BE);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;else{if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp5C3)){struct Cyc_Absyn_Exp*
function_e=Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_plus_functionSet,_tmp421);
struct Cyc_Absyn_Exp*_tmpAEE[3];(void*)(e->r=(void*)Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(
ta2,Cyc_Absyn_fncall_exp(function_e,((_tmpAEE[2]=_tmp422,((_tmpAEE[1]=_tmp5CF,((
_tmpAEE[0]=_tmp421,((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmpAEE,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0))));}else{if(
possibly_null){if(Cyc_Toc_warn_all_null_deref){const char*_tmpAF1;void*_tmpAF0;(
_tmpAF0=0,Cyc_Tcutil_warn(e->loc,((_tmpAF1="inserted null check due to dereference",
_tag_dynforward(_tmpAF1,sizeof(char),_get_zero_arr_size_char(_tmpAF1,39)))),
_tag_dynforward(_tmpAF0,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*_tmpAF2[4];(
void*)(e->r=(void*)Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,((_tmpAF2[3]=_tmp422,((_tmpAF2[2]=Cyc_Absyn_sizeoftyp_exp(
ta1,0),((_tmpAF2[1]=_tmp5CF,((_tmpAF2[0]=_tmp421,((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmpAF2,sizeof(struct Cyc_Absyn_Exp*),
4)))))))))),0))));}}else{struct Cyc_Absyn_Exp*_tmpAF3[2];(void*)(_tmp422->r=(void*)
Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,((_tmpAF3[1]=Cyc_Absyn_copy_exp(
_tmp422),((_tmpAF3[0]=_tmp5CF,((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmpAF3,sizeof(struct Cyc_Absyn_Exp*),2))))))));}}}
goto _LL320;}_LL323: if((int)_tmp5CE != 0)goto _LL325;_LL324: forward_only=1;goto
_LL326;_LL325: if((int)_tmp5CE != 1)goto _LL327;_LL326: {void*ta1=Cyc_Toc_typ_to_c_array(
_tmp5BD);if(in_bnds){++ Cyc_Toc_bounds_checks_eliminated;(void*)(e->r=(void*)Cyc_Toc_subscript_exp_r(
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp5BE),Cyc_Absyn_aggrmember_exp(
_tmp421,Cyc_Toc_curr_sp,0)),_tmp422));}else{struct Cyc_Absyn_Exp*_tmp5D6=
forward_only?Cyc_Toc__check_dynforward_subscript_e: Cyc_Toc__check_dyneither_subscript_e;
struct Cyc_Absyn_Exp*_tmpAF4[3];(void*)(e->r=(void*)Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(
Cyc_Absyn_cstar_typ(ta1,_tmp5BE),Cyc_Absyn_fncall_exp(_tmp5D6,((_tmpAF4[2]=
_tmp422,((_tmpAF4[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpAF4[0]=_tmp421,((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmpAF4,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0))));}goto _LL320;}_LL327: if(
_tmp5CE <= (void*)2)goto _LL320;if(*((int*)_tmp5CE)!= 1)goto _LL320;_tmp5D0=(void*)((
struct Cyc_Absyn_AbsUpper_b_struct*)_tmp5CE)->f1;_LL328:{void*_tmp5D8=(void*)
_tmp422->annot;struct Cyc_List_List*_tmp5D9;_LL32A: if(*((void**)_tmp5D8)!= Cyc_CfFlowInfo_HasTagCmps)
goto _LL32C;_tmp5D9=((struct Cyc_CfFlowInfo_HasTagCmps_struct*)_tmp5D8)->f1;_LL32B:
for(0;_tmp5D9 != 0;_tmp5D9=_tmp5D9->tl){if((void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp5D9->hd)->cmp == (void*)8  && Cyc_Tcutil_typecmp((void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp5D9->hd)->bd,_tmp5D0)== 0){in_bnds=1;break;}}goto _LL329;_LL32C:;_LL32D: goto
_LL329;_LL329:;}if(!in_bnds){const char*_tmpAF7;void*_tmpAF6;(_tmpAF6=0,Cyc_Tcutil_terr(
e->loc,((_tmpAF7="cannot determine that subscript is in bounds",_tag_dynforward(
_tmpAF7,sizeof(char),_get_zero_arr_size_char(_tmpAF7,45)))),_tag_dynforward(
_tmpAF6,sizeof(void*),0)));}goto _LL320;_LL320:;}goto _LL314;}}_LL319:;_LL31A: {
const char*_tmpAFA;void*_tmpAF9;(_tmpAF9=0,((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmpAFA="exp_to_c: Subscript on non-tuple/array/tuple ptr",
_tag_dynforward(_tmpAFA,sizeof(char),_get_zero_arr_size_char(_tmpAFA,49)))),
_tag_dynforward(_tmpAF9,sizeof(void*),0)));}_LL314:;}goto _LL1E0;}_LL21B: if(*((
int*)_tmp3E7)!= 26)goto _LL21D;_tmp423=((struct Cyc_Absyn_Tuple_e_struct*)_tmp3E7)->f1;
_LL21C: if(!Cyc_Toc_is_toplevel(nv))(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(
nv,0,0,_tmp423))->r));else{struct Cyc_List_List*_tmp5DE=((struct Cyc_List_List*(*)(
struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Toc_tup_to_c,_tmp423);void*_tmp5DF=Cyc_Toc_add_tuple_type(_tmp5DE);struct Cyc_List_List*
dles=0;{int i=1;for(0;_tmp423 != 0;(_tmp423=_tmp423->tl,i ++)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp423->hd);{struct _tuple8*_tmpAFD;struct Cyc_List_List*
_tmpAFC;dles=((_tmpAFC=_cycalloc(sizeof(*_tmpAFC)),((_tmpAFC->hd=((_tmpAFD=
_cycalloc(sizeof(*_tmpAFD)),((_tmpAFD->f1=0,((_tmpAFD->f2=(struct Cyc_Absyn_Exp*)
_tmp423->hd,_tmpAFD)))))),((_tmpAFC->tl=dles,_tmpAFC))))));}}}dles=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,dles));}goto _LL1E0;_LL21D: if(*((int*)_tmp3E7)!= 28)goto _LL21F;_tmp424=((struct
Cyc_Absyn_Array_e_struct*)_tmp3E7)->f1;_LL21E:(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,_tmp424));{struct Cyc_List_List*_tmp5E2=_tmp424;for(0;_tmp5E2 != 0;_tmp5E2=
_tmp5E2->tl){struct _tuple8 _tmp5E4;struct Cyc_Absyn_Exp*_tmp5E5;struct _tuple8*
_tmp5E3=(struct _tuple8*)_tmp5E2->hd;_tmp5E4=*_tmp5E3;_tmp5E5=_tmp5E4.f2;Cyc_Toc_exp_to_c(
nv,_tmp5E5);}}goto _LL1E0;_LL21F: if(*((int*)_tmp3E7)!= 29)goto _LL221;_tmp425=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp3E7)->f1;_tmp426=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3E7)->f2;_tmp427=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp3E7)->f3;
_tmp428=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp3E7)->f4;_LL220: {
unsigned int _tmp5E7;int _tmp5E8;struct _tuple5 _tmp5E6=Cyc_Evexp_eval_const_uint_exp(
_tmp426);_tmp5E7=_tmp5E6.f1;_tmp5E8=_tmp5E6.f2;{void*_tmp5E9=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp427->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp427);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp427)){if(!_tmp5E8){
const char*_tmpB00;void*_tmpAFF;(_tmpAFF=0,Cyc_Tcutil_terr(_tmp426->loc,((_tmpB00="cannot determine value of constant",
_tag_dynforward(_tmpB00,sizeof(char),_get_zero_arr_size_char(_tmpB00,35)))),
_tag_dynforward(_tmpAFF,sizeof(void*),0)));}{unsigned int i=0;for(0;i < _tmp5E7;++
i){struct _tuple8*_tmpB03;struct Cyc_List_List*_tmpB02;es=((_tmpB02=_cycalloc(
sizeof(*_tmpB02)),((_tmpB02->hd=((_tmpB03=_cycalloc(sizeof(*_tmpB03)),((_tmpB03->f1=
0,((_tmpB03->f2=_tmp427,_tmpB03)))))),((_tmpB02->tl=es,_tmpB02))))));}}if(
_tmp428){struct Cyc_Absyn_Exp*_tmp5EE=Cyc_Toc_cast_it(_tmp5E9,Cyc_Absyn_uint_exp(
0,0));struct _tuple8*_tmpB06;struct Cyc_List_List*_tmpB05;es=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,((_tmpB05=
_cycalloc(sizeof(*_tmpB05)),((_tmpB05->hd=((_tmpB06=_cycalloc(sizeof(*_tmpB06)),((
_tmpB06->f1=0,((_tmpB06->f2=_tmp5EE,_tmpB06)))))),((_tmpB05->tl=0,_tmpB05)))))));}}(
void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,es));goto _LL1E0;}}}_LL221: if(*((
int*)_tmp3E7)!= 30)goto _LL223;_tmp429=((struct Cyc_Absyn_Struct_e_struct*)_tmp3E7)->f1;
_tmp42A=((struct Cyc_Absyn_Struct_e_struct*)_tmp3E7)->f2;_tmp42B=((struct Cyc_Absyn_Struct_e_struct*)
_tmp3E7)->f3;_tmp42C=((struct Cyc_Absyn_Struct_e_struct*)_tmp3E7)->f4;_LL222: if(!
Cyc_Toc_is_toplevel(nv))(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(nv,
old_typ,_tmp42A != 0,0,0,_tmp42B,_tmp429))->r));else{if(_tmp42C == 0){const char*
_tmpB09;void*_tmpB08;(_tmpB08=0,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmpB09="Struct_e: missing aggrdecl pointer",
_tag_dynforward(_tmpB09,sizeof(char),_get_zero_arr_size_char(_tmpB09,35)))),
_tag_dynforward(_tmpB08,sizeof(void*),0)));}{struct Cyc_Absyn_Aggrdecl*sd2=(
struct Cyc_Absyn_Aggrdecl*)_tmp42C;struct _RegionHandle _tmp5F3=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp5F3;_push_region(rgn);{struct Cyc_List_List*_tmp5F4=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,e->loc,_tmp42B,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
struct Cyc_List_List*_tmp5F5=0;struct Cyc_List_List*_tmp5F6=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp5F6 != 0;_tmp5F6=_tmp5F6->tl){struct Cyc_List_List*
_tmp5F7=_tmp5F4;for(0;_tmp5F7 != 0;_tmp5F7=_tmp5F7->tl){if((*((struct _tuple12*)
_tmp5F7->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp5F6->hd){struct _tuple12
_tmp5F9;struct Cyc_Absyn_Aggrfield*_tmp5FA;struct Cyc_Absyn_Exp*_tmp5FB;struct
_tuple12*_tmp5F8=(struct _tuple12*)_tmp5F7->hd;_tmp5F9=*_tmp5F8;_tmp5FA=_tmp5F9.f1;
_tmp5FB=_tmp5F9.f2;{void*_tmp5FC=(void*)_tmp5FA->type;Cyc_Toc_exp_to_c(nv,
_tmp5FB);if(Cyc_Toc_is_void_star(_tmp5FC))(void*)(_tmp5FB->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp5FB->r,0)));{struct
_tuple8*_tmpB0C;struct Cyc_List_List*_tmpB0B;_tmp5F5=((_tmpB0B=_cycalloc(sizeof(*
_tmpB0B)),((_tmpB0B->hd=((_tmpB0C=_cycalloc(sizeof(*_tmpB0C)),((_tmpB0C->f1=0,((
_tmpB0C->f2=_tmp5FB,_tmpB0C)))))),((_tmpB0B->tl=_tmp5F5,_tmpB0B))))));}break;}}}}(
void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5F5)));};_pop_region(rgn);}}goto
_LL1E0;_LL223: if(*((int*)_tmp3E7)!= 31)goto _LL225;_tmp42D=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp3E7)->f1;_tmp42E=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3E7)->f2;_LL224: {
struct Cyc_List_List*fs;{void*_tmp5FF=Cyc_Tcutil_compress(_tmp42D);struct Cyc_List_List*
_tmp600;_LL32F: if(_tmp5FF <= (void*)4)goto _LL331;if(*((int*)_tmp5FF)!= 11)goto
_LL331;_tmp600=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp5FF)->f2;_LL330: fs=
_tmp600;goto _LL32E;_LL331:;_LL332: {const char*_tmpB10;void*_tmpB0F[1];struct Cyc_String_pa_struct
_tmpB0E;(_tmpB0E.tag=0,((_tmpB0E.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_typ2string(_tmp42D)),((_tmpB0F[0]=& _tmpB0E,((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmpB10="anon struct has type %s",
_tag_dynforward(_tmpB10,sizeof(char),_get_zero_arr_size_char(_tmpB10,24)))),
_tag_dynforward(_tmpB0F,sizeof(void*),1)))))));}_LL32E:;}{struct _RegionHandle
_tmp604=_new_region("rgn");struct _RegionHandle*rgn=& _tmp604;_push_region(rgn);{
struct Cyc_List_List*_tmp605=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_struct_designators)(rgn,e->loc,_tmp42E,fs);for(0;
_tmp605 != 0;_tmp605=_tmp605->tl){struct _tuple12 _tmp607;struct Cyc_Absyn_Aggrfield*
_tmp608;struct Cyc_Absyn_Exp*_tmp609;struct _tuple12*_tmp606=(struct _tuple12*)
_tmp605->hd;_tmp607=*_tmp606;_tmp608=_tmp607.f1;_tmp609=_tmp607.f2;{void*_tmp60A=(
void*)_tmp608->type;Cyc_Toc_exp_to_c(nv,_tmp609);if(Cyc_Toc_is_void_star(_tmp60A))(
void*)(_tmp609->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((
void*)_tmp609->r,0)));}}(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,
_tmp42E));};_pop_region(rgn);}goto _LL1E0;}_LL225: if(*((int*)_tmp3E7)!= 32)goto
_LL227;_tmp42F=((struct Cyc_Absyn_Tunion_e_struct*)_tmp3E7)->f1;if(_tmp42F != 0)
goto _LL227;_tmp430=((struct Cyc_Absyn_Tunion_e_struct*)_tmp3E7)->f2;_tmp431=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp3E7)->f3;if(!(!_tmp430->is_flat))goto _LL227;
_LL226: {struct _tuple1*qv=_tmp431->name;struct Cyc_Absyn_Exp*tag_exp=_tmp430->is_xtunion?
Cyc_Absyn_var_exp(qv,0): Cyc_Toc_tunion_tag(_tmp430,qv,0);(void*)(e->r=(void*)((
void*)tag_exp->r));goto _LL1E0;}_LL227: if(*((int*)_tmp3E7)!= 32)goto _LL229;
_tmp432=((struct Cyc_Absyn_Tunion_e_struct*)_tmp3E7)->f1;_tmp433=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp3E7)->f2;_tmp434=((struct Cyc_Absyn_Tunion_e_struct*)_tmp3E7)->f3;_LL228: {
void*tunion_ctype;struct Cyc_Absyn_Exp*tag_exp;struct _tuple1*_tmp60B=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp60C=Cyc_Absyn_var_exp(_tmp60B,0);struct Cyc_Absyn_Exp*
member_exp;if(_tmp433->is_flat){{const char*_tmpB11;tunion_ctype=Cyc_Absyn_unionq_typ(
Cyc_Toc_collapse_qvar_tag(_tmp433->name,((_tmpB11="_union",_tag_dynforward(
_tmpB11,sizeof(char),_get_zero_arr_size_char(_tmpB11,7))))));}tag_exp=Cyc_Toc_tunion_tag(
_tmp433,_tmp434->name,1);{struct _tuple1 _tmp60F;struct _dynforward_ptr*_tmp610;
struct _tuple1*_tmp60E=_tmp434->name;_tmp60F=*_tmp60E;_tmp610=_tmp60F.f2;
member_exp=Cyc_Absyn_aggrmember_exp(_tmp60C,_tmp610,0);}}else{{const char*_tmpB12;
tunion_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp434->name,((_tmpB12="_struct",
_tag_dynforward(_tmpB12,sizeof(char),_get_zero_arr_size_char(_tmpB12,8))))));}
tag_exp=_tmp433->is_xtunion?Cyc_Absyn_var_exp(_tmp434->name,0): Cyc_Toc_tunion_tag(
_tmp433,_tmp434->name,1);member_exp=_tmp60C;}{struct Cyc_List_List*_tmp612=
_tmp434->typs;if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*dles=0;for(0;
_tmp432 != 0;(_tmp432=_tmp432->tl,_tmp612=_tmp612->tl)){struct Cyc_Absyn_Exp*cur_e=(
struct Cyc_Absyn_Exp*)_tmp432->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct
_tuple4*)((struct Cyc_List_List*)_check_null(_tmp612))->hd)).f2);Cyc_Toc_exp_to_c(
nv,cur_e);if(Cyc_Toc_is_void_star(field_typ))cur_e=Cyc_Toc_cast_it(field_typ,
cur_e);{struct _tuple8*_tmpB15;struct Cyc_List_List*_tmpB14;dles=((_tmpB14=
_cycalloc(sizeof(*_tmpB14)),((_tmpB14->hd=((_tmpB15=_cycalloc(sizeof(*_tmpB15)),((
_tmpB15->f1=0,((_tmpB15->f2=cur_e,_tmpB15)))))),((_tmpB14->tl=dles,_tmpB14))))));}}{
struct _tuple8*_tmpB18;struct Cyc_List_List*_tmpB17;dles=((_tmpB17=_cycalloc(
sizeof(*_tmpB17)),((_tmpB17->hd=((_tmpB18=_cycalloc(sizeof(*_tmpB18)),((_tmpB18->f1=
0,((_tmpB18->f2=tag_exp,_tmpB18)))))),((_tmpB17->tl=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(dles),_tmpB17))))));}(void*)(e->r=(void*)
Cyc_Toc_unresolvedmem_exp_r(0,dles));if(_tmp433->is_flat){const char*_tmpB19;(
void*)(e->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp434->name,((_tmpB19="_struct",_tag_dynforward(_tmpB19,sizeof(char),
_get_zero_arr_size_char(_tmpB19,8)))))),Cyc_Absyn_copy_exp(e)));}}else{struct Cyc_List_List*
_tmpB1A;struct Cyc_List_List*_tmp618=(_tmpB1A=_cycalloc(sizeof(*_tmpB1A)),((
_tmpB1A->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Toc_tag_sp,
0),tag_exp,0),((_tmpB1A->tl=0,_tmpB1A)))));{int i=1;for(0;_tmp432 != 0;(((_tmp432=
_tmp432->tl,i ++)),_tmp612=_tmp612->tl)){struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)
_tmp432->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple4*)((struct Cyc_List_List*)
_check_null(_tmp612))->hd)).f2);Cyc_Toc_exp_to_c(nv,cur_e);if(Cyc_Toc_is_void_star(
field_typ))cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{struct Cyc_Absyn_Stmt*_tmp619=
Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(member_exp,Cyc_Absyn_fieldname(i),
0),cur_e,0);struct Cyc_List_List*_tmpB1B;_tmp618=((_tmpB1B=_cycalloc(sizeof(*
_tmpB1B)),((_tmpB1B->hd=_tmp619,((_tmpB1B->tl=_tmp618,_tmpB1B))))));}}}{struct
Cyc_Absyn_Stmt*_tmp61B=Cyc_Absyn_exp_stmt(_tmp60C,0);struct Cyc_List_List*_tmpB1C;
struct Cyc_Absyn_Stmt*_tmp61C=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpB1C=_cycalloc(sizeof(*_tmpB1C)),((
_tmpB1C->hd=_tmp61B,((_tmpB1C->tl=_tmp618,_tmpB1C))))))),0);(void*)(e->r=(void*)
Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp60B,tunion_ctype,0,_tmp61C,0)));}}
goto _LL1E0;}}_LL229: if(*((int*)_tmp3E7)!= 33)goto _LL22B;_LL22A: goto _LL22C;_LL22B:
if(*((int*)_tmp3E7)!= 34)goto _LL22D;_LL22C: goto _LL1E0;_LL22D: if(*((int*)_tmp3E7)
!= 35)goto _LL22F;_tmp435=((struct Cyc_Absyn_Malloc_e_struct*)_tmp3E7)->f1;_tmp436=
_tmp435.is_calloc;_tmp437=_tmp435.rgn;_tmp438=_tmp435.elt_type;_tmp439=_tmp435.num_elts;
_tmp43A=_tmp435.fat_result;_LL22E: {void*t_c=Cyc_Toc_typ_to_c(*((void**)
_check_null(_tmp438)));Cyc_Toc_exp_to_c(nv,_tmp439);if(_tmp43A){struct _tuple1*
_tmp61F=Cyc_Toc_temp_var();struct _tuple1*_tmp620=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;if(_tmp436){xexp=_tmp439;
if(_tmp437 != 0  && !Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)
_tmp437;Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp61F,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp438,Cyc_Absyn_sizeoftyp_exp(
t_c,0),Cyc_Absyn_var_exp(_tmp61F,0));}{struct Cyc_Absyn_Exp*_tmpB1D[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dynforward_e,((_tmpB1D[2]=Cyc_Absyn_var_exp(_tmp61F,0),((_tmpB1D[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpB1D[0]=Cyc_Absyn_var_exp(_tmp620,0),((
struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(
_tmpB1D,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}}else{xexp=Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp439,0);if(_tmp437 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp437;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp61F,0));}else{pexp=Cyc_Toc_malloc_exp(*
_tmp438,Cyc_Absyn_var_exp(_tmp61F,0));}{struct Cyc_Absyn_Exp*_tmpB1E[3];rexp=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dynforward_e,((_tmpB1E[2]=Cyc_Absyn_var_exp(_tmp61F,0),((_tmpB1E[1]=
Cyc_Absyn_uint_exp(1,0),((_tmpB1E[0]=Cyc_Absyn_var_exp(_tmp620,0),((struct Cyc_List_List*(*)(
struct _dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmpB1E,sizeof(struct Cyc_Absyn_Exp*),
3)))))))),0);}}{struct Cyc_Absyn_Stmt*_tmp623=Cyc_Absyn_declare_stmt(_tmp61F,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp620,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);(void*)(
e->r=(void*)Cyc_Toc_stmt_exp_r(_tmp623));}}else{if(_tmp437 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp437;Cyc_Toc_exp_to_c(nv,rgn);(
void*)(e->r=(void*)((void*)(Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,
0)))->r));}else{(void*)(e->r=(void*)((void*)(Cyc_Toc_malloc_exp(*_tmp438,Cyc_Absyn_sizeoftyp_exp(
t_c,0)))->r));}}goto _LL1E0;}_LL22F: if(*((int*)_tmp3E7)!= 36)goto _LL231;_tmp43B=((
struct Cyc_Absyn_Swap_e_struct*)_tmp3E7)->f1;_tmp43C=((struct Cyc_Absyn_Swap_e_struct*)
_tmp3E7)->f2;_LL230: {int is_dynforward_ptr=0;int is_dyneither_ptr=0;void*
e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp43B->topt))->v;void*
e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp43C->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(
e1_old_typ,& is_dynforward_ptr,& is_dyneither_ptr)){const char*_tmpB21;void*_tmpB20;(
_tmpB20=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB21="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",
_tag_dynforward(_tmpB21,sizeof(char),_get_zero_arr_size_char(_tmpB21,57)))),
_tag_dynforward(_tmpB20,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*swap_fn;if(
is_dynforward_ptr)swap_fn=Cyc_Toc__swap_dynforward_e;else{if(is_dyneither_ptr)
swap_fn=Cyc_Toc__swap_dyneither_e;else{swap_fn=Cyc_Toc__swap_word_e;}}if(!Cyc_Absyn_is_lvalue(
_tmp43B)){const char*_tmpB25;void*_tmpB24[1];struct Cyc_String_pa_struct _tmpB23;(
_tmpB23.tag=0,((_tmpB23.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
_tmp43B)),((_tmpB24[0]=& _tmpB23,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmpB25="Swap_e: %s is not an l-value\n",
_tag_dynforward(_tmpB25,sizeof(char),_get_zero_arr_size_char(_tmpB25,30)))),
_tag_dynforward(_tmpB24,sizeof(void*),1)))))));}if(!Cyc_Absyn_is_lvalue(_tmp43C)){
const char*_tmpB29;void*_tmpB28[1];struct Cyc_String_pa_struct _tmpB27;(_tmpB27.tag=
0,((_tmpB27.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
_tmp43C)),((_tmpB28[0]=& _tmpB27,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmpB29="Swap_e: %s is not an l-value\n",
_tag_dynforward(_tmpB29,sizeof(char),_get_zero_arr_size_char(_tmpB29,30)))),
_tag_dynforward(_tmpB28,sizeof(void*),1)))))));}Cyc_Toc_exp_to_c(nv,_tmp43B);Cyc_Toc_exp_to_c(
nv,_tmp43C);{struct Cyc_Absyn_Exp*_tmpB2A[2];(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
swap_fn,((_tmpB2A[1]=Cyc_Absyn_address_exp(_tmp43C,0),((_tmpB2A[0]=Cyc_Absyn_address_exp(
_tmp43B,0),((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmpB2A,sizeof(struct Cyc_Absyn_Exp*),2))))))));}goto _LL1E0;}}
_LL231: if(*((int*)_tmp3E7)!= 38)goto _LL233;_tmp43D=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp3E7)->f1;_LL232: Cyc_Toc_stmt_to_c(nv,_tmp43D);goto _LL1E0;_LL233: if(*((int*)
_tmp3E7)!= 37)goto _LL235;_LL234: {const char*_tmpB2D;void*_tmpB2C;(_tmpB2C=0,((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((
_tmpB2D="UnresolvedMem",_tag_dynforward(_tmpB2D,sizeof(char),
_get_zero_arr_size_char(_tmpB2D,14)))),_tag_dynforward(_tmpB2C,sizeof(void*),0)));}
_LL235: if(*((int*)_tmp3E7)!= 27)goto _LL1E0;_LL236: {const char*_tmpB30;void*
_tmpB2F;(_tmpB2F=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_unimp)(((_tmpB30="compoundlit",_tag_dynforward(_tmpB30,sizeof(char),
_get_zero_arr_size_char(_tmpB30,12)))),_tag_dynforward(_tmpB2F,sizeof(void*),0)));}
_LL1E0:;}}static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,
struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt);static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*
fail_stmt){return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
Cyc_Toc_skip_stmt_dl(),0);}struct _tuple13{struct Cyc_Toc_Env*f1;struct Cyc_List_List*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple14{struct _tuple1*f1;void*f2;};struct
_tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple13 Cyc_Toc_xlate_pat(
struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct
Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt**tag_fail_stmt,
struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls);static struct _tuple13
Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*
r,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt**
tag_fail_stmt,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){struct
Cyc_Absyn_Stmt*s;{void*_tmp631=(void*)p->r;struct Cyc_Absyn_Vardecl*_tmp632;
struct Cyc_Absyn_Vardecl _tmp633;struct _tuple1*_tmp634;struct Cyc_Absyn_Pat*_tmp635;
struct Cyc_Absyn_Vardecl*_tmp636;struct Cyc_Absyn_Vardecl _tmp637;struct _tuple1*
_tmp638;struct Cyc_Absyn_Vardecl*_tmp639;struct Cyc_Absyn_Pat*_tmp63A;void*_tmp63B;
int _tmp63C;char _tmp63D;struct _dynforward_ptr _tmp63E;struct Cyc_Absyn_Enumdecl*
_tmp63F;struct Cyc_Absyn_Enumfield*_tmp640;void*_tmp641;struct Cyc_Absyn_Enumfield*
_tmp642;struct Cyc_Absyn_Tuniondecl*_tmp643;struct Cyc_Absyn_Tunionfield*_tmp644;
struct Cyc_List_List*_tmp645;struct Cyc_Absyn_Pat*_tmp646;struct Cyc_Absyn_Pat
_tmp647;void*_tmp648;struct Cyc_Absyn_Tuniondecl*_tmp649;struct Cyc_Absyn_Tunionfield*
_tmp64A;struct Cyc_List_List*_tmp64B;struct Cyc_Absyn_Tuniondecl*_tmp64C;struct Cyc_Absyn_Tunionfield*
_tmp64D;struct Cyc_List_List*_tmp64E;struct Cyc_List_List*_tmp64F;struct Cyc_List_List*
_tmp650;struct Cyc_Absyn_AggrInfo _tmp651;union Cyc_Absyn_AggrInfoU_union _tmp652;
struct Cyc_List_List*_tmp653;struct Cyc_Absyn_Pat*_tmp654;_LL334: if(_tmp631 <= (
void*)2)goto _LL338;if(*((int*)_tmp631)!= 0)goto _LL336;_tmp632=((struct Cyc_Absyn_Var_p_struct*)
_tmp631)->f1;_tmp633=*_tmp632;_tmp634=_tmp633.name;_tmp635=((struct Cyc_Absyn_Var_p_struct*)
_tmp631)->f2;_LL335: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp634,r),
rgn,t,r,path,_tmp635,tag_fail_stmt,fail_stmt,decls);_LL336: if(*((int*)_tmp631)!= 
2)goto _LL338;_tmp636=((struct Cyc_Absyn_TagInt_p_struct*)_tmp631)->f2;_tmp637=*
_tmp636;_tmp638=_tmp637.name;_LL337: nv=Cyc_Toc_add_varmap(rgn,nv,_tmp638,r);goto
_LL339;_LL338: if((int)_tmp631 != 0)goto _LL33A;_LL339: s=Cyc_Toc_skip_stmt_dl();
goto _LL333;_LL33A: if(_tmp631 <= (void*)2)goto _LL33C;if(*((int*)_tmp631)!= 1)goto
_LL33C;_tmp639=((struct Cyc_Absyn_Reference_p_struct*)_tmp631)->f1;_tmp63A=((
struct Cyc_Absyn_Reference_p_struct*)_tmp631)->f2;_LL33B: {struct _tuple1*_tmp655=
Cyc_Toc_temp_var();{struct _tuple14*_tmpB33;struct Cyc_List_List*_tmpB32;decls=((
_tmpB32=_region_malloc(rgn,sizeof(*_tmpB32)),((_tmpB32->hd=((_tmpB33=
_region_malloc(rgn,sizeof(*_tmpB33)),((_tmpB33->f1=_tmp655,((_tmpB33->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq),_tmpB33)))))),((_tmpB32->tl=decls,_tmpB32))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp639->name,Cyc_Absyn_var_exp(_tmp655,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp655,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Absyn_address_exp(path,0)),0);{struct _tuple13 _tmp658=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp63A,tag_fail_stmt,fail_stmt,decls);_tmp658.f3=Cyc_Absyn_seq_stmt(
s,_tmp658.f3,0);return _tmp658;}}_LL33C: if((int)_tmp631 != 1)goto _LL33E;_LL33D: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL333;_LL33E:
if(_tmp631 <= (void*)2)goto _LL340;if(*((int*)_tmp631)!= 7)goto _LL340;_tmp63B=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp631)->f1;_tmp63C=((struct Cyc_Absyn_Int_p_struct*)
_tmp631)->f2;_LL33F: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp63B,_tmp63C,0),
fail_stmt);goto _LL333;_LL340: if(_tmp631 <= (void*)2)goto _LL342;if(*((int*)_tmp631)
!= 8)goto _LL342;_tmp63D=((struct Cyc_Absyn_Char_p_struct*)_tmp631)->f1;_LL341: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp63D,0),fail_stmt);goto _LL333;_LL342:
if(_tmp631 <= (void*)2)goto _LL344;if(*((int*)_tmp631)!= 9)goto _LL344;_tmp63E=((
struct Cyc_Absyn_Float_p_struct*)_tmp631)->f1;_LL343: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(
_tmp63E,0),fail_stmt);goto _LL333;_LL344: if(_tmp631 <= (void*)2)goto _LL346;if(*((
int*)_tmp631)!= 10)goto _LL346;_tmp63F=((struct Cyc_Absyn_Enum_p_struct*)_tmp631)->f1;
_tmp640=((struct Cyc_Absyn_Enum_p_struct*)_tmp631)->f2;_LL345:{struct Cyc_Absyn_Enum_e_struct
_tmpB36;struct Cyc_Absyn_Enum_e_struct*_tmpB35;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((
void*)((_tmpB35=_cycalloc(sizeof(*_tmpB35)),((_tmpB35[0]=((_tmpB36.tag=33,((
_tmpB36.f1=_tmp640->name,((_tmpB36.f2=(struct Cyc_Absyn_Enumdecl*)_tmp63F,((
_tmpB36.f3=(struct Cyc_Absyn_Enumfield*)_tmp640,_tmpB36)))))))),_tmpB35)))),0),
fail_stmt);}goto _LL333;_LL346: if(_tmp631 <= (void*)2)goto _LL348;if(*((int*)
_tmp631)!= 11)goto _LL348;_tmp641=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp631)->f1;_tmp642=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp631)->f2;_LL347:{
struct Cyc_Absyn_AnonEnum_e_struct _tmpB39;struct Cyc_Absyn_AnonEnum_e_struct*
_tmpB38;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpB38=_cycalloc(
sizeof(*_tmpB38)),((_tmpB38[0]=((_tmpB39.tag=34,((_tmpB39.f1=_tmp642->name,((
_tmpB39.f2=(void*)_tmp641,((_tmpB39.f3=(struct Cyc_Absyn_Enumfield*)_tmp642,
_tmpB39)))))))),_tmpB38)))),0),fail_stmt);}goto _LL333;_LL348: if(_tmp631 <= (void*)
2)goto _LL34A;if(*((int*)_tmp631)!= 6)goto _LL34A;_tmp643=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp631)->f1;_tmp644=((struct Cyc_Absyn_Tunion_p_struct*)_tmp631)->f2;_tmp645=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp631)->f3;if(_tmp645 != 0)goto _LL34A;if(!(!
_tmp643->is_flat))goto _LL34A;_LL349: {struct Cyc_Absyn_Exp*cmp_exp;if(_tmp643->is_xtunion)
cmp_exp=Cyc_Absyn_var_exp(_tmp644->name,0);else{cmp_exp=Cyc_Toc_tunion_tag(
_tmp643,_tmp644->name,0);r=Cyc_Toc_cast_it(Cyc_Absyn_sint_typ,r);}s=Cyc_Toc_if_neq_stmt(
r,cmp_exp,fail_stmt);goto _LL333;}_LL34A: if(_tmp631 <= (void*)2)goto _LL34C;if(*((
int*)_tmp631)!= 4)goto _LL34C;_tmp646=((struct Cyc_Absyn_Pointer_p_struct*)_tmp631)->f1;
_tmp647=*_tmp646;_tmp648=(void*)_tmp647.r;if(_tmp648 <= (void*)2)goto _LL34C;if(*((
int*)_tmp648)!= 6)goto _LL34C;_tmp649=((struct Cyc_Absyn_Tunion_p_struct*)_tmp648)->f1;
_tmp64A=((struct Cyc_Absyn_Tunion_p_struct*)_tmp648)->f2;_tmp64B=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp648)->f3;if(!(_tmp64B != 0  && !_tmp649->is_flat))goto _LL34C;_LL34B: s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;const char*_tmpB3A;struct _tuple1*tufstrct=Cyc_Toc_collapse_qvar_tag(
_tmp64A->name,((_tmpB3A="_struct",_tag_dynforward(_tmpB3A,sizeof(char),
_get_zero_arr_size_char(_tmpB3A,8)))));struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(
Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq),r);struct Cyc_List_List*
_tmp65D=_tmp64A->typs;for(0;_tmp64B != 0;(((_tmp64B=_tmp64B->tl,_tmp65D=((struct
Cyc_List_List*)_check_null(_tmp65D))->tl)),++ cnt)){struct Cyc_Absyn_Pat*_tmp65E=(
struct Cyc_Absyn_Pat*)_tmp64B->hd;if((void*)_tmp65E->r == (void*)0)continue;{void*
_tmp65F=(*((struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp65D))->hd)).f2;
struct _tuple1*_tmp660=Cyc_Toc_temp_var();void*_tmp661=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp65E->topt))->v;void*_tmp662=Cyc_Toc_typ_to_c(_tmp661);struct Cyc_Absyn_Exp*
_tmp663=Cyc_Absyn_aggrarrow_exp(rcast,Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star(
Cyc_Toc_typ_to_c(_tmp65F)))_tmp663=Cyc_Toc_cast_it(_tmp662,_tmp663);{struct
_tuple14*_tmpB3D;struct Cyc_List_List*_tmpB3C;decls=((_tmpB3C=_region_malloc(rgn,
sizeof(*_tmpB3C)),((_tmpB3C->hd=((_tmpB3D=_region_malloc(rgn,sizeof(*_tmpB3D)),((
_tmpB3D->f1=_tmp660,((_tmpB3D->f2=_tmp662,_tmpB3D)))))),((_tmpB3C->tl=decls,
_tmpB3C))))));}{struct _tuple13 _tmp666=Cyc_Toc_xlate_pat(nv,rgn,_tmp661,Cyc_Absyn_var_exp(
_tmp660,0),_tmp663,_tmp65E,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);
nv=_tmp666.f1;decls=_tmp666.f2;{struct Cyc_Absyn_Stmt*_tmp667=_tmp666.f3;struct
Cyc_Absyn_Stmt*_tmp668=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp660,0),
_tmp663,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp668,_tmp667,0),0);}}}}{
struct Cyc_Absyn_Exp*test_exp;if(_tmp649->is_xtunion){struct Cyc_Absyn_Exp*e2=Cyc_Toc_cast_it(
Cyc_Absyn_cstar_typ(Cyc_Absyn_void_star_typ(),Cyc_Toc_mt_tq),r);struct Cyc_Absyn_Exp*
e1=Cyc_Absyn_deref_exp(e2,0);struct Cyc_Absyn_Exp*e=Cyc_Absyn_var_exp(_tmp64A->name,
0);test_exp=Cyc_Absyn_neq_exp(e1,e,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
test_exp,fail_stmt,Cyc_Toc_skip_stmt_dl(),0),s,0);}else{struct Cyc_Absyn_Exp*e3=
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,Cyc_Toc_mt_tq),r);struct
Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e3,0);struct Cyc_Absyn_Exp*e=Cyc_Toc_tunion_tag(
_tmp649,_tmp64A->name,1);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(
e1,e,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),s,0);if(tag_fail_stmt != 0){int
max_tag=Cyc_Toc_num_void_tags(_tmp649);if(max_tag != 0){struct Cyc_Absyn_Exp*
max_tag_exp=Cyc_Absyn_uint_exp((unsigned int)max_tag,0);struct Cyc_Absyn_Exp*e5=
Cyc_Absyn_lte_exp(r,Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),max_tag_exp),0);s=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(e5,*tag_fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),s,0);}}}goto _LL333;}}_LL34C: if(_tmp631 <= (void*)2)goto _LL34E;if(*((int*)
_tmp631)!= 6)goto _LL34E;_tmp64C=((struct Cyc_Absyn_Tunion_p_struct*)_tmp631)->f1;
_tmp64D=((struct Cyc_Absyn_Tunion_p_struct*)_tmp631)->f2;_tmp64E=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp631)->f3;if(!_tmp64C->is_flat)goto _LL34E;_LL34D: {struct _tuple1 _tmp66B;
struct _dynforward_ptr*_tmp66C;struct _tuple1*_tmp66A=_tmp64D->name;_tmp66B=*
_tmp66A;_tmp66C=_tmp66B.f2;r=Cyc_Absyn_aggrmember_exp(r,_tmp66C,0);path=Cyc_Absyn_aggrmember_exp(
path,_tmp66C,0);s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(Cyc_Absyn_aggrmember_exp(
path,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(_tmp64C,_tmp64D->name,1),0),fail_stmt,
Cyc_Toc_skip_stmt_dl(),0);{int cnt=1;for(0;_tmp64E != 0;(_tmp64E=_tmp64E->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp66D=(struct Cyc_Absyn_Pat*)_tmp64E->hd;if((void*)_tmp66D->r
== (void*)0)continue;{struct _tuple1*_tmp66E=Cyc_Toc_temp_var();void*_tmp66F=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp66D->topt))->v;{struct _tuple14*
_tmpB40;struct Cyc_List_List*_tmpB3F;decls=((_tmpB3F=_region_malloc(rgn,sizeof(*
_tmpB3F)),((_tmpB3F->hd=((_tmpB40=_region_malloc(rgn,sizeof(*_tmpB40)),((_tmpB40->f1=
_tmp66E,((_tmpB40->f2=Cyc_Toc_typ_to_c(_tmp66F),_tmpB40)))))),((_tmpB3F->tl=
decls,_tmpB3F))))));}{struct _tuple13 _tmp672=Cyc_Toc_xlate_pat(nv,rgn,_tmp66F,Cyc_Absyn_var_exp(
_tmp66E,0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(cnt),0),_tmp66D,(
struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp672.f1;decls=_tmp672.f2;{
struct Cyc_Absyn_Stmt*_tmp673=_tmp672.f3;struct Cyc_Absyn_Stmt*_tmp674=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp66E,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),
0),0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp674,_tmp673,0),0);}}}}goto
_LL333;}}_LL34E: if(_tmp631 <= (void*)2)goto _LL350;if(*((int*)_tmp631)!= 6)goto
_LL350;_tmp64F=((struct Cyc_Absyn_Tunion_p_struct*)_tmp631)->f3;_LL34F: _tmp650=
_tmp64F;goto _LL351;_LL350: if(_tmp631 <= (void*)2)goto _LL352;if(*((int*)_tmp631)!= 
3)goto _LL352;_tmp650=((struct Cyc_Absyn_Tuple_p_struct*)_tmp631)->f1;_LL351: s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;for(0;_tmp650 != 0;(_tmp650=_tmp650->tl,++ cnt)){struct Cyc_Absyn_Pat*
_tmp675=(struct Cyc_Absyn_Pat*)_tmp650->hd;if((void*)_tmp675->r == (void*)0)
continue;{struct _tuple1*_tmp676=Cyc_Toc_temp_var();void*_tmp677=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp675->topt))->v;{struct _tuple14*_tmpB43;struct Cyc_List_List*
_tmpB42;decls=((_tmpB42=_region_malloc(rgn,sizeof(*_tmpB42)),((_tmpB42->hd=((
_tmpB43=_region_malloc(rgn,sizeof(*_tmpB43)),((_tmpB43->f1=_tmp676,((_tmpB43->f2=
Cyc_Toc_typ_to_c(_tmp677),_tmpB43)))))),((_tmpB42->tl=decls,_tmpB42))))));}{
struct _tuple13 _tmp67A=Cyc_Toc_xlate_pat(nv,rgn,_tmp677,Cyc_Absyn_var_exp(_tmp676,
0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(cnt),0),_tmp675,(struct Cyc_Absyn_Stmt**)&
fail_stmt,fail_stmt,decls);nv=_tmp67A.f1;decls=_tmp67A.f2;{struct Cyc_Absyn_Stmt*
_tmp67B=_tmp67A.f3;struct Cyc_Absyn_Stmt*_tmp67C=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp676,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp67C,_tmp67B,0),0);}}}}goto _LL333;}_LL352: if(_tmp631 <= (
void*)2)goto _LL354;if(*((int*)_tmp631)!= 5)goto _LL354;_tmp651=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp631)->f1;_tmp652=_tmp651.aggr_info;_tmp653=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp631)->f3;_LL353: {struct Cyc_Absyn_Aggrdecl*_tmp67D=Cyc_Absyn_get_known_aggrdecl(
_tmp652);s=Cyc_Toc_skip_stmt_dl();for(0;_tmp653 != 0;_tmp653=_tmp653->tl){struct
_tuple15*_tmp67E=(struct _tuple15*)_tmp653->hd;struct Cyc_Absyn_Pat*_tmp67F=(*
_tmp67E).f2;if((void*)_tmp67F->r == (void*)0)continue;{struct _dynforward_ptr*f;{
void*_tmp680=(void*)((struct Cyc_List_List*)_check_null((*_tmp67E).f1))->hd;
struct _dynforward_ptr*_tmp681;_LL35D: if(*((int*)_tmp680)!= 1)goto _LL35F;_tmp681=((
struct Cyc_Absyn_FieldName_struct*)_tmp680)->f1;_LL35E: f=_tmp681;goto _LL35C;
_LL35F:;_LL360:(int)_throw((void*)Cyc_Toc_Match_error);_LL35C:;}{struct _tuple1*
_tmp682=Cyc_Toc_temp_var();void*_tmp683=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp67F->topt))->v;void*_tmp684=Cyc_Toc_typ_to_c(_tmp683);{struct _tuple14*
_tmpB46;struct Cyc_List_List*_tmpB45;decls=((_tmpB45=_region_malloc(rgn,sizeof(*
_tmpB45)),((_tmpB45->hd=((_tmpB46=_region_malloc(rgn,sizeof(*_tmpB46)),((_tmpB46->f1=
_tmp682,((_tmpB46->f2=_tmp684,_tmpB46)))))),((_tmpB45->tl=decls,_tmpB45))))));}{
struct _tuple13 _tmp687=Cyc_Toc_xlate_pat(nv,rgn,_tmp683,Cyc_Absyn_var_exp(_tmp682,
0),Cyc_Absyn_aggrmember_exp(path,f,0),_tmp67F,(struct Cyc_Absyn_Stmt**)& fail_stmt,
fail_stmt,decls);nv=_tmp687.f1;decls=_tmp687.f2;{struct Cyc_Absyn_Exp*_tmp688=Cyc_Absyn_aggrmember_exp(
r,f,0);if(Cyc_Toc_is_void_star((void*)((struct Cyc_Absyn_Aggrfield*)_check_null(
Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp67D->impl))->fields,
f)))->type))_tmp688=Cyc_Toc_cast_it(_tmp684,_tmp688);{struct Cyc_Absyn_Stmt*
_tmp689=_tmp687.f3;struct Cyc_Absyn_Stmt*_tmp68A=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp682,0),_tmp688,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp68A,_tmp689,
0),0);}}}}}}goto _LL333;}_LL354: if(_tmp631 <= (void*)2)goto _LL356;if(*((int*)
_tmp631)!= 4)goto _LL356;_tmp654=((struct Cyc_Absyn_Pointer_p_struct*)_tmp631)->f1;
_LL355: {struct _tuple1*_tmp68B=Cyc_Toc_temp_var();void*_tmp68C=(void*)((struct
Cyc_Core_Opt*)_check_null(_tmp654->topt))->v;{struct _tuple14*_tmpB49;struct Cyc_List_List*
_tmpB48;decls=((_tmpB48=_region_malloc(rgn,sizeof(*_tmpB48)),((_tmpB48->hd=((
_tmpB49=_region_malloc(rgn,sizeof(*_tmpB49)),((_tmpB49->f1=_tmp68B,((_tmpB49->f2=
Cyc_Toc_typ_to_c(_tmp68C),_tmpB49)))))),((_tmpB48->tl=decls,_tmpB48))))));}{
struct _tuple13 _tmp68F=Cyc_Toc_xlate_pat(nv,rgn,_tmp68C,Cyc_Absyn_var_exp(_tmp68B,
0),Cyc_Absyn_deref_exp(path,0),_tmp654,(struct Cyc_Absyn_Stmt**)& fail_stmt,
fail_stmt,decls);nv=_tmp68F.f1;decls=_tmp68F.f2;{struct Cyc_Absyn_Stmt*_tmp690=
_tmp68F.f3;struct Cyc_Absyn_Stmt*_tmp691=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp68B,0),Cyc_Absyn_deref_exp(r,0),0),_tmp690,0);if(Cyc_Toc_is_nullable(
t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(
0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),_tmp691,0);else{s=_tmp691;}goto
_LL333;}}}_LL356: if(_tmp631 <= (void*)2)goto _LL358;if(*((int*)_tmp631)!= 12)goto
_LL358;_LL357: {const char*_tmpB4C;void*_tmpB4B;(_tmpB4B=0,((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmpB4C="unknownid pat",
_tag_dynforward(_tmpB4C,sizeof(char),_get_zero_arr_size_char(_tmpB4C,14)))),
_tag_dynforward(_tmpB4B,sizeof(void*),0)));}_LL358: if(_tmp631 <= (void*)2)goto
_LL35A;if(*((int*)_tmp631)!= 13)goto _LL35A;_LL359: {const char*_tmpB4F;void*
_tmpB4E;(_tmpB4E=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(((_tmpB4F="unknowncall pat",_tag_dynforward(_tmpB4F,sizeof(
char),_get_zero_arr_size_char(_tmpB4F,16)))),_tag_dynforward(_tmpB4E,sizeof(void*),
0)));}_LL35A: if(_tmp631 <= (void*)2)goto _LL333;if(*((int*)_tmp631)!= 14)goto
_LL333;_LL35B: {const char*_tmpB52;void*_tmpB51;(_tmpB51=0,((int(*)(struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmpB52="exp pat",
_tag_dynforward(_tmpB52,sizeof(char),_get_zero_arr_size_char(_tmpB52,8)))),
_tag_dynforward(_tmpB51,sizeof(void*),0)));}_LL333:;}{struct _tuple13 _tmpB53;
return(_tmpB53.f1=nv,((_tmpB53.f2=decls,((_tmpB53.f3=s,_tmpB53)))));}}struct
_tuple16{struct _dynforward_ptr*f1;struct _dynforward_ptr*f2;struct Cyc_Absyn_Switch_clause*
f3;};static struct _tuple16*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
sc);static struct _tuple16*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
sc){struct _tuple16*_tmpB54;return(_tmpB54=_region_malloc(r,sizeof(*_tmpB54)),((
_tmpB54->f1=Cyc_Toc_fresh_label(),((_tmpB54->f2=Cyc_Toc_fresh_label(),((_tmpB54->f3=
sc,_tmpB54)))))));}static int Cyc_Toc_is_mixed_tunion(void*t);static int Cyc_Toc_is_mixed_tunion(
void*t){{void*_tmp69A=Cyc_Tcutil_compress(t);struct Cyc_Absyn_TunionInfo _tmp69B;
union Cyc_Absyn_TunionInfoU_union _tmp69C;struct Cyc_Absyn_Tuniondecl**_tmp69D;
struct Cyc_Absyn_Tuniondecl*_tmp69E;_LL362: if(_tmp69A <= (void*)4)goto _LL364;if(*((
int*)_tmp69A)!= 2)goto _LL364;_tmp69B=((struct Cyc_Absyn_TunionType_struct*)
_tmp69A)->f1;_tmp69C=_tmp69B.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp69A)->f1).tunion_info).KnownTunion).tag != 1)goto _LL364;_tmp69D=(_tmp69C.KnownTunion).f1;
_tmp69E=*_tmp69D;if(!(!_tmp69E->is_flat))goto _LL364;_LL363: {int seen_novalue=0;
int seen_value=0;{struct Cyc_List_List*_tmp69F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp69E->fields))->v;for(0;(unsigned int)_tmp69F;_tmp69F=_tmp69F->tl){
if(((struct Cyc_Absyn_Tunionfield*)_tmp69F->hd)->typs == 0)seen_value=1;else{
seen_novalue=1;}if(seen_value  && seen_novalue)return 1;}}goto _LL361;}_LL364:;
_LL365: goto _LL361;_LL361:;}return 0;}static int Cyc_Toc_no_tag_test(struct Cyc_Absyn_Pat*
p);static int Cyc_Toc_no_tag_test(struct Cyc_Absyn_Pat*p){void*_tmp6A0=(void*)p->r;
struct Cyc_Absyn_Pat*_tmp6A1;struct Cyc_Absyn_Pat*_tmp6A2;struct Cyc_Absyn_Pat
_tmp6A3;void*_tmp6A4;struct Cyc_Absyn_Tuniondecl*_tmp6A5;struct Cyc_Absyn_Tunionfield*
_tmp6A6;struct Cyc_List_List*_tmp6A7;_LL367: if(_tmp6A0 <= (void*)2)goto _LL36B;if(*((
int*)_tmp6A0)!= 0)goto _LL369;_tmp6A1=((struct Cyc_Absyn_Var_p_struct*)_tmp6A0)->f2;
_LL368: return Cyc_Toc_no_tag_test(_tmp6A1);_LL369: if(*((int*)_tmp6A0)!= 4)goto
_LL36B;_tmp6A2=((struct Cyc_Absyn_Pointer_p_struct*)_tmp6A0)->f1;_tmp6A3=*_tmp6A2;
_tmp6A4=(void*)_tmp6A3.r;if(_tmp6A4 <= (void*)2)goto _LL36B;if(*((int*)_tmp6A4)!= 
6)goto _LL36B;_tmp6A5=((struct Cyc_Absyn_Tunion_p_struct*)_tmp6A4)->f1;_tmp6A6=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp6A4)->f2;_tmp6A7=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp6A4)->f3;if(!(_tmp6A7 != 0  && !_tmp6A5->is_flat))goto _LL36B;_LL36A: return 0;
_LL36B:;_LL36C: return 1;_LL366:;}static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs);
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){Cyc_Toc_exp_to_c(nv,e);{void*
_tmp6A8=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;int leave_as_switch;{
void*_tmp6A9=Cyc_Tcutil_compress(_tmp6A8);_LL36E: if(_tmp6A9 <= (void*)4)goto
_LL372;if(*((int*)_tmp6A9)!= 5)goto _LL370;_LL36F: goto _LL371;_LL370: if(*((int*)
_tmp6A9)!= 12)goto _LL372;_LL371: leave_as_switch=1;goto _LL36D;_LL372:;_LL373:
leave_as_switch=0;goto _LL36D;_LL36D:;}{struct Cyc_List_List*_tmp6AA=scs;for(0;
_tmp6AA != 0;_tmp6AA=_tmp6AA->tl){if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp6AA->hd)->pat_vars))->v != 0  || ((
struct Cyc_Absyn_Switch_clause*)_tmp6AA->hd)->where_clause != 0){leave_as_switch=0;
break;}}}if(leave_as_switch){struct _dynforward_ptr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp6AB=scs;for(0;_tmp6AB != 0;_tmp6AB=_tmp6AB->tl){struct Cyc_Absyn_Stmt*
_tmp6AC=((struct Cyc_Absyn_Switch_clause*)_tmp6AB->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp6AB->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp6AC,0);next_l=Cyc_Toc_fresh_label();{
struct _RegionHandle _tmp6AD=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6AD;
_push_region(rgn);Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),
_tmp6AC);;_pop_region(rgn);}}}return;}{struct _tuple1*v=Cyc_Toc_temp_var();struct
Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dynforward_ptr*end_l=Cyc_Toc_fresh_label();struct _RegionHandle
_tmp6AE=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6AE;_push_region(rgn);{
struct Cyc_Toc_Env*_tmp6AF=Cyc_Toc_share_env(rgn,nv);struct Cyc_List_List*lscs=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple16*(*f)(struct
_RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);struct Cyc_List_List*
test_stmts=0;struct Cyc_List_List*nvs=0;struct Cyc_List_List*decls=0;int is_tunion=
Cyc_Toc_is_mixed_tunion(_tmp6A8);int needs_tag_test=is_tunion;{struct Cyc_List_List*
_tmp6B0=lscs;for(0;_tmp6B0 != 0;_tmp6B0=_tmp6B0->tl){struct Cyc_Absyn_Switch_clause*
sc=(*((struct _tuple16*)_tmp6B0->hd)).f3;struct _dynforward_ptr*fail_lab=_tmp6B0->tl
== 0?end_l:(*((struct _tuple16*)((struct Cyc_List_List*)_check_null(_tmp6B0->tl))->hd)).f1;
struct Cyc_Absyn_Stmt**tag_fail_stmt=0;if(needs_tag_test  && !Cyc_Toc_no_tag_test(
sc->pattern)){{struct Cyc_List_List*_tmp6B1=_tmp6B0->tl;for(0;(unsigned int)
_tmp6B1;_tmp6B1=_tmp6B1->tl){if(Cyc_Toc_no_tag_test(((*((struct _tuple16*)_tmp6B1->hd)).f3)->pattern)){{
struct Cyc_Absyn_Stmt**_tmpB55;tag_fail_stmt=((_tmpB55=_region_malloc(rgn,sizeof(*
_tmpB55)),((_tmpB55[0]=Cyc_Absyn_goto_stmt((*((struct _tuple16*)_tmp6B1->hd)).f1,
0),_tmpB55))));}needs_tag_test=0;break;}}}if(tag_fail_stmt == 0){struct Cyc_Absyn_Stmt**
_tmpB56;tag_fail_stmt=((_tmpB56=_region_malloc(rgn,sizeof(*_tmpB56)),((_tmpB56[0]=
Cyc_Absyn_goto_stmt(fail_lab,0),_tmpB56))));}}{struct Cyc_Toc_Env*_tmp6B5;struct
Cyc_List_List*_tmp6B6;struct Cyc_Absyn_Stmt*_tmp6B7;struct _tuple13 _tmp6B4=Cyc_Toc_xlate_pat(
_tmp6AF,rgn,_tmp6A8,r,path,sc->pattern,tag_fail_stmt,Cyc_Absyn_goto_stmt(
fail_lab,0),decls);_tmp6B5=_tmp6B4.f1;_tmp6B6=_tmp6B4.f2;_tmp6B7=_tmp6B4.f3;if(
is_tunion  && Cyc_Toc_no_tag_test(sc->pattern))needs_tag_test=1;if(sc->where_clause
!= 0){struct Cyc_Absyn_Exp*_tmp6B8=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);
Cyc_Toc_exp_to_c(_tmp6B5,_tmp6B8);_tmp6B7=Cyc_Absyn_seq_stmt(_tmp6B7,Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_prim1_exp((void*)11,_tmp6B8,0),Cyc_Absyn_goto_stmt(fail_lab,0),Cyc_Toc_skip_stmt_dl(),
0),0);}decls=_tmp6B6;{struct Cyc_List_List*_tmpB57;nvs=((_tmpB57=_region_malloc(
rgn,sizeof(*_tmpB57)),((_tmpB57->hd=_tmp6B5,((_tmpB57->tl=nvs,_tmpB57))))));}{
struct Cyc_List_List*_tmpB58;test_stmts=((_tmpB58=_region_malloc(rgn,sizeof(*
_tmpB58)),((_tmpB58->hd=_tmp6B7,((_tmpB58->tl=test_stmts,_tmpB58))))));}}}}nvs=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{
struct Cyc_List_List*stmts=0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),
test_stmts=test_stmts->tl)){struct _tuple16 _tmp6BC;struct _dynforward_ptr*_tmp6BD;
struct _dynforward_ptr*_tmp6BE;struct Cyc_Absyn_Switch_clause*_tmp6BF;struct
_tuple16*_tmp6BB=(struct _tuple16*)lscs->hd;_tmp6BC=*_tmp6BB;_tmp6BD=_tmp6BC.f1;
_tmp6BE=_tmp6BC.f2;_tmp6BF=_tmp6BC.f3;{struct Cyc_Toc_Env*_tmp6C0=(struct Cyc_Toc_Env*)((
struct Cyc_List_List*)_check_null(nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp6BF->body;{
struct _RegionHandle _tmp6C1=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp6C1;
_push_region(rgn2);if(lscs->tl != 0){struct _tuple16 _tmp6C3;struct _dynforward_ptr*
_tmp6C4;struct Cyc_Absyn_Switch_clause*_tmp6C5;struct _tuple16*_tmp6C2=(struct
_tuple16*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp6C3=*_tmp6C2;
_tmp6C4=_tmp6C3.f2;_tmp6C5=_tmp6C3.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(
rgn2,_tmp6C0,end_l,_tmp6C4,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp6C5->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)
_check_null(nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(
rgn2,_tmp6C0,end_l),s);};_pop_region(rgn2);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(
_tmp6BD,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,
0),Cyc_Absyn_label_stmt(_tmp6BE,s,0),0);{struct Cyc_List_List*_tmpB59;stmts=((
_tmpB59=_region_malloc(rgn,sizeof(*_tmpB59)),((_tmpB59->hd=s,((_tmpB59->tl=stmts,
_tmpB59))))));}}}{struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(
end_l,Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct
_tuple14 _tmp6C8;struct _tuple1*_tmp6C9;void*_tmp6CA;struct _tuple14*_tmp6C7=(
struct _tuple14*)decls->hd;_tmp6C8=*_tmp6C7;_tmp6C9=_tmp6C8.f1;_tmp6CA=_tmp6C8.f2;
res=Cyc_Absyn_declare_stmt(_tmp6C9,_tmp6CA,0,res,0);}(void*)(whole_s->r=(void*)((
void*)(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),(struct Cyc_Absyn_Exp*)e,res,0))->r));}}};_pop_region(
rgn);}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*
init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler(int n);struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(
int n){struct Cyc_List_List*_tmpB5A;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__npop_handler_e,((_tmpB5A=_cycalloc(sizeof(*_tmpB5A)),((_tmpB5A->hd=Cyc_Absyn_uint_exp((
unsigned int)(n - 1),0),((_tmpB5A->tl=0,_tmpB5A)))))),0),0);}void Cyc_Toc_do_npop_before(
int n,struct Cyc_Absyn_Stmt*s);void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*
s){if(n > 0)(void*)(s->r=(void*)Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),
Cyc_Absyn_new_stmt((void*)s->r,0)));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Stmt*s){while(1){void*_tmp6CC=(void*)s->r;struct Cyc_Absyn_Exp*
_tmp6CD;struct Cyc_Absyn_Stmt*_tmp6CE;struct Cyc_Absyn_Stmt*_tmp6CF;struct Cyc_Absyn_Exp*
_tmp6D0;struct Cyc_Absyn_Exp*_tmp6D1;struct Cyc_Absyn_Stmt*_tmp6D2;struct Cyc_Absyn_Stmt*
_tmp6D3;struct _tuple3 _tmp6D4;struct Cyc_Absyn_Exp*_tmp6D5;struct Cyc_Absyn_Stmt*
_tmp6D6;struct Cyc_Absyn_Stmt*_tmp6D7;struct Cyc_Absyn_Stmt*_tmp6D8;struct Cyc_Absyn_Stmt*
_tmp6D9;struct Cyc_Absyn_Exp*_tmp6DA;struct _tuple3 _tmp6DB;struct Cyc_Absyn_Exp*
_tmp6DC;struct _tuple3 _tmp6DD;struct Cyc_Absyn_Exp*_tmp6DE;struct Cyc_Absyn_Stmt*
_tmp6DF;struct Cyc_Absyn_Exp*_tmp6E0;struct Cyc_List_List*_tmp6E1;struct Cyc_List_List*
_tmp6E2;struct Cyc_Absyn_Switch_clause**_tmp6E3;struct Cyc_Absyn_Decl*_tmp6E4;
struct Cyc_Absyn_Stmt*_tmp6E5;struct _dynforward_ptr*_tmp6E6;struct Cyc_Absyn_Stmt*
_tmp6E7;struct Cyc_Absyn_Stmt*_tmp6E8;struct _tuple3 _tmp6E9;struct Cyc_Absyn_Exp*
_tmp6EA;struct Cyc_Absyn_Stmt*_tmp6EB;struct Cyc_List_List*_tmp6EC;struct Cyc_Absyn_Tvar*
_tmp6ED;struct Cyc_Absyn_Vardecl*_tmp6EE;int _tmp6EF;struct Cyc_Absyn_Exp*_tmp6F0;
struct Cyc_Absyn_Stmt*_tmp6F1;struct Cyc_Absyn_Exp*_tmp6F2;struct Cyc_Absyn_Exp*
_tmp6F3;struct Cyc_Absyn_Tvar*_tmp6F4;struct Cyc_Absyn_Vardecl*_tmp6F5;struct Cyc_Absyn_Stmt*
_tmp6F6;_LL375: if((int)_tmp6CC != 0)goto _LL377;_LL376: return;_LL377: if(_tmp6CC <= (
void*)1)goto _LL379;if(*((int*)_tmp6CC)!= 0)goto _LL379;_tmp6CD=((struct Cyc_Absyn_Exp_s_struct*)
_tmp6CC)->f1;_LL378: Cyc_Toc_exp_to_c(nv,_tmp6CD);return;_LL379: if(_tmp6CC <= (
void*)1)goto _LL37B;if(*((int*)_tmp6CC)!= 1)goto _LL37B;_tmp6CE=((struct Cyc_Absyn_Seq_s_struct*)
_tmp6CC)->f1;_tmp6CF=((struct Cyc_Absyn_Seq_s_struct*)_tmp6CC)->f2;_LL37A: Cyc_Toc_stmt_to_c(
nv,_tmp6CE);s=_tmp6CF;continue;_LL37B: if(_tmp6CC <= (void*)1)goto _LL37D;if(*((int*)
_tmp6CC)!= 2)goto _LL37D;_tmp6D0=((struct Cyc_Absyn_Return_s_struct*)_tmp6CC)->f1;
_LL37C: {struct Cyc_Core_Opt*topt=0;if(_tmp6D0 != 0){{struct Cyc_Core_Opt*_tmpB5B;
topt=((_tmpB5B=_cycalloc(sizeof(*_tmpB5B)),((_tmpB5B->v=(void*)Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp6D0->topt))->v),_tmpB5B))));}Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_tmp6D0);}if(s->try_depth > 0){if(topt != 0){struct
_tuple1*_tmp6F8=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp6F9=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp6F8,0),0);(void*)(s->r=(void*)((void*)(
Cyc_Absyn_declare_stmt(_tmp6F8,(void*)topt->v,_tmp6D0,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
s->try_depth),_tmp6F9,0),0))->r));}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}
return;}_LL37D: if(_tmp6CC <= (void*)1)goto _LL37F;if(*((int*)_tmp6CC)!= 3)goto
_LL37F;_tmp6D1=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp6CC)->f1;_tmp6D2=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp6CC)->f2;_tmp6D3=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp6CC)->f3;_LL37E: Cyc_Toc_exp_to_c(nv,_tmp6D1);Cyc_Toc_stmt_to_c(nv,_tmp6D2);s=
_tmp6D3;continue;_LL37F: if(_tmp6CC <= (void*)1)goto _LL381;if(*((int*)_tmp6CC)!= 4)
goto _LL381;_tmp6D4=((struct Cyc_Absyn_While_s_struct*)_tmp6CC)->f1;_tmp6D5=
_tmp6D4.f1;_tmp6D6=((struct Cyc_Absyn_While_s_struct*)_tmp6CC)->f2;_LL380: Cyc_Toc_exp_to_c(
nv,_tmp6D5);{struct _RegionHandle _tmp6FA=_new_region("temp");struct _RegionHandle*
temp=& _tmp6FA;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),
_tmp6D6);;_pop_region(temp);}return;_LL381: if(_tmp6CC <= (void*)1)goto _LL383;if(*((
int*)_tmp6CC)!= 5)goto _LL383;_tmp6D7=((struct Cyc_Absyn_Break_s_struct*)_tmp6CC)->f1;
_LL382: {struct Cyc_Toc_Env _tmp6FC;struct _dynforward_ptr**_tmp6FD;struct Cyc_Toc_Env*
_tmp6FB=nv;_tmp6FC=*_tmp6FB;_tmp6FD=_tmp6FC.break_lab;if(_tmp6FD != 0)(void*)(s->r=(
void*)Cyc_Toc_goto_stmt_r(*_tmp6FD,0));{int dest_depth=_tmp6D7 == 0?0: _tmp6D7->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;}}_LL383: if(_tmp6CC <= (
void*)1)goto _LL385;if(*((int*)_tmp6CC)!= 6)goto _LL385;_tmp6D8=((struct Cyc_Absyn_Continue_s_struct*)
_tmp6CC)->f1;_LL384: {struct Cyc_Toc_Env _tmp6FF;struct _dynforward_ptr**_tmp700;
struct Cyc_Toc_Env*_tmp6FE=nv;_tmp6FF=*_tmp6FE;_tmp700=_tmp6FF.continue_lab;if(
_tmp700 != 0)(void*)(s->r=(void*)Cyc_Toc_goto_stmt_r(*_tmp700,0));_tmp6D9=_tmp6D8;
goto _LL386;}_LL385: if(_tmp6CC <= (void*)1)goto _LL387;if(*((int*)_tmp6CC)!= 7)goto
_LL387;_tmp6D9=((struct Cyc_Absyn_Goto_s_struct*)_tmp6CC)->f2;_LL386: Cyc_Toc_do_npop_before(
s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp6D9))->try_depth,s);
return;_LL387: if(_tmp6CC <= (void*)1)goto _LL389;if(*((int*)_tmp6CC)!= 8)goto
_LL389;_tmp6DA=((struct Cyc_Absyn_For_s_struct*)_tmp6CC)->f1;_tmp6DB=((struct Cyc_Absyn_For_s_struct*)
_tmp6CC)->f2;_tmp6DC=_tmp6DB.f1;_tmp6DD=((struct Cyc_Absyn_For_s_struct*)_tmp6CC)->f3;
_tmp6DE=_tmp6DD.f1;_tmp6DF=((struct Cyc_Absyn_For_s_struct*)_tmp6CC)->f4;_LL388:
Cyc_Toc_exp_to_c(nv,_tmp6DA);Cyc_Toc_exp_to_c(nv,_tmp6DC);Cyc_Toc_exp_to_c(nv,
_tmp6DE);{struct _RegionHandle _tmp701=_new_region("temp");struct _RegionHandle*
temp=& _tmp701;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),
_tmp6DF);;_pop_region(temp);}return;_LL389: if(_tmp6CC <= (void*)1)goto _LL38B;if(*((
int*)_tmp6CC)!= 9)goto _LL38B;_tmp6E0=((struct Cyc_Absyn_Switch_s_struct*)_tmp6CC)->f1;
_tmp6E1=((struct Cyc_Absyn_Switch_s_struct*)_tmp6CC)->f2;_LL38A: Cyc_Toc_xlate_switch(
nv,s,_tmp6E0,_tmp6E1);return;_LL38B: if(_tmp6CC <= (void*)1)goto _LL38D;if(*((int*)
_tmp6CC)!= 10)goto _LL38D;_tmp6E2=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp6CC)->f1;
_tmp6E3=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp6CC)->f2;_LL38C: {struct Cyc_Toc_Env
_tmp703;struct Cyc_Toc_FallthruInfo*_tmp704;struct Cyc_Toc_Env*_tmp702=nv;_tmp703=*
_tmp702;_tmp704=_tmp703.fallthru_info;if(_tmp704 == 0){const char*_tmpB5E;void*
_tmpB5D;(_tmpB5D=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(((_tmpB5E="fallthru in unexpected place",_tag_dynforward(
_tmpB5E,sizeof(char),_get_zero_arr_size_char(_tmpB5E,29)))),_tag_dynforward(
_tmpB5D,sizeof(void*),0)));}{struct _dynforward_ptr*_tmp708;struct Cyc_List_List*
_tmp709;struct Cyc_Dict_Dict _tmp70A;struct Cyc_Toc_FallthruInfo _tmp707=*_tmp704;
_tmp708=_tmp707.label;_tmp709=_tmp707.binders;_tmp70A=_tmp707.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp708,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp6E3)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp70B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp709);struct Cyc_List_List*_tmp70C=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp6E2);for(0;_tmp70B != 0;(_tmp70B=_tmp70B->tl,
_tmp70C=_tmp70C->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp70C))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp70A,(
struct _tuple1*)_tmp70B->hd),(struct Cyc_Absyn_Exp*)_tmp70C->hd,0),s2,0);}(void*)(
s->r=(void*)((void*)s2->r));return;}}}}_LL38D: if(_tmp6CC <= (void*)1)goto _LL38F;
if(*((int*)_tmp6CC)!= 11)goto _LL38F;_tmp6E4=((struct Cyc_Absyn_Decl_s_struct*)
_tmp6CC)->f1;_tmp6E5=((struct Cyc_Absyn_Decl_s_struct*)_tmp6CC)->f2;_LL38E:{void*
_tmp70D=(void*)_tmp6E4->r;struct Cyc_Absyn_Vardecl*_tmp70E;struct Cyc_Absyn_Pat*
_tmp70F;struct Cyc_Absyn_Exp*_tmp710;struct Cyc_List_List*_tmp711;struct Cyc_Absyn_Fndecl*
_tmp712;_LL39C: if(_tmp70D <= (void*)2)goto _LL3A4;if(*((int*)_tmp70D)!= 0)goto
_LL39E;_tmp70E=((struct Cyc_Absyn_Var_d_struct*)_tmp70D)->f1;_LL39D:{struct
_RegionHandle _tmp713=_new_region("temp");struct _RegionHandle*temp=& _tmp713;
_push_region(temp);{struct Cyc_Absyn_Local_b_struct _tmpB61;struct Cyc_Absyn_Local_b_struct*
_tmpB60;struct Cyc_Toc_Env*_tmp714=Cyc_Toc_add_varmap(temp,nv,_tmp70E->name,Cyc_Absyn_varb_exp(
_tmp70E->name,(void*)((_tmpB60=_cycalloc(sizeof(*_tmpB60)),((_tmpB60[0]=((
_tmpB61.tag=3,((_tmpB61.f1=_tmp70E,_tmpB61)))),_tmpB60)))),0));Cyc_Toc_local_decl_to_c(
_tmp714,_tmp714,_tmp70E,_tmp6E5);};_pop_region(temp);}goto _LL39B;_LL39E: if(*((
int*)_tmp70D)!= 2)goto _LL3A0;_tmp70F=((struct Cyc_Absyn_Let_d_struct*)_tmp70D)->f1;
_tmp710=((struct Cyc_Absyn_Let_d_struct*)_tmp70D)->f3;_LL39F:{void*_tmp717=(void*)
_tmp70F->r;struct Cyc_Absyn_Vardecl*_tmp718;struct Cyc_Absyn_Pat*_tmp719;struct Cyc_Absyn_Pat
_tmp71A;void*_tmp71B;_LL3A7: if(_tmp717 <= (void*)2)goto _LL3A9;if(*((int*)_tmp717)
!= 0)goto _LL3A9;_tmp718=((struct Cyc_Absyn_Var_p_struct*)_tmp717)->f1;_tmp719=((
struct Cyc_Absyn_Var_p_struct*)_tmp717)->f2;_tmp71A=*_tmp719;_tmp71B=(void*)
_tmp71A.r;if((int)_tmp71B != 0)goto _LL3A9;_LL3A8: {struct _tuple1*old_name=_tmp718->name;
struct _tuple1*new_name=Cyc_Toc_temp_var();_tmp718->name=new_name;_tmp718->initializer=(
struct Cyc_Absyn_Exp*)_tmp710;{struct Cyc_Absyn_Var_d_struct _tmpB64;struct Cyc_Absyn_Var_d_struct*
_tmpB63;(void*)(_tmp6E4->r=(void*)((void*)((_tmpB63=_cycalloc(sizeof(*_tmpB63)),((
_tmpB63[0]=((_tmpB64.tag=0,((_tmpB64.f1=_tmp718,_tmpB64)))),_tmpB63))))));}{
struct _RegionHandle _tmp71E=_new_region("temp");struct _RegionHandle*temp=& _tmp71E;
_push_region(temp);{struct Cyc_Absyn_Local_b_struct _tmpB67;struct Cyc_Absyn_Local_b_struct*
_tmpB66;struct Cyc_Toc_Env*_tmp71F=Cyc_Toc_add_varmap(temp,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpB66=_cycalloc(sizeof(*_tmpB66)),((_tmpB66[0]=((_tmpB67.tag=
3,((_tmpB67.f1=_tmp718,_tmpB67)))),_tmpB66)))),0));Cyc_Toc_local_decl_to_c(
_tmp71F,nv,_tmp718,_tmp6E5);};_pop_region(temp);}goto _LL3A6;}_LL3A9:;_LL3AA:(
void*)(s->r=(void*)((void*)(Cyc_Toc_letdecl_to_c(nv,_tmp70F,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp710->topt))->v,_tmp710,_tmp6E5))->r));goto _LL3A6;_LL3A6:;}goto
_LL39B;_LL3A0: if(*((int*)_tmp70D)!= 3)goto _LL3A2;_tmp711=((struct Cyc_Absyn_Letv_d_struct*)
_tmp70D)->f1;_LL3A1: {struct Cyc_List_List*_tmp722=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp711);if(_tmp722 == 0){const char*_tmpB6A;
void*_tmpB69;(_tmpB69=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr
ap))Cyc_Tcutil_impos)(((_tmpB6A="empty Letv_d",_tag_dynforward(_tmpB6A,sizeof(
char),_get_zero_arr_size_char(_tmpB6A,13)))),_tag_dynforward(_tmpB69,sizeof(void*),
0)));}{struct Cyc_Absyn_Var_d_struct _tmpB6D;struct Cyc_Absyn_Var_d_struct*_tmpB6C;(
void*)(_tmp6E4->r=(void*)((void*)((_tmpB6C=_cycalloc(sizeof(*_tmpB6C)),((_tmpB6C[
0]=((_tmpB6D.tag=0,((_tmpB6D.f1=(struct Cyc_Absyn_Vardecl*)_tmp722->hd,_tmpB6D)))),
_tmpB6C))))));}_tmp722=_tmp722->tl;for(0;_tmp722 != 0;_tmp722=_tmp722->tl){struct
Cyc_Absyn_Var_d_struct _tmpB70;struct Cyc_Absyn_Var_d_struct*_tmpB6F;struct Cyc_Absyn_Decl*
_tmp727=Cyc_Absyn_new_decl((void*)((_tmpB6F=_cycalloc(sizeof(*_tmpB6F)),((
_tmpB6F[0]=((_tmpB70.tag=0,((_tmpB70.f1=(struct Cyc_Absyn_Vardecl*)_tmp722->hd,
_tmpB70)))),_tmpB6F)))),0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_decl_stmt(
_tmp727,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));}Cyc_Toc_stmt_to_c(nv,s);goto
_LL39B;}_LL3A2: if(*((int*)_tmp70D)!= 1)goto _LL3A4;_tmp712=((struct Cyc_Absyn_Fn_d_struct*)
_tmp70D)->f1;_LL3A3: {struct _tuple1*_tmp72A=_tmp712->name;{struct _RegionHandle
_tmp72B=_new_region("temp");struct _RegionHandle*temp=& _tmp72B;_push_region(temp);{
struct Cyc_Toc_Env*_tmp72C=Cyc_Toc_add_varmap(temp,nv,_tmp712->name,Cyc_Absyn_var_exp(
_tmp72A,0));Cyc_Toc_fndecl_to_c(_tmp72C,_tmp712,0);Cyc_Toc_stmt_to_c(_tmp72C,
_tmp6E5);};_pop_region(temp);}goto _LL39B;}_LL3A4:;_LL3A5: {const char*_tmpB73;
void*_tmpB72;(_tmpB72=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr
ap))Cyc_Tcutil_impos)(((_tmpB73="bad nested declaration within function",
_tag_dynforward(_tmpB73,sizeof(char),_get_zero_arr_size_char(_tmpB73,39)))),
_tag_dynforward(_tmpB72,sizeof(void*),0)));}_LL39B:;}return;_LL38F: if(_tmp6CC <= (
void*)1)goto _LL391;if(*((int*)_tmp6CC)!= 12)goto _LL391;_tmp6E6=((struct Cyc_Absyn_Label_s_struct*)
_tmp6CC)->f1;_tmp6E7=((struct Cyc_Absyn_Label_s_struct*)_tmp6CC)->f2;_LL390: s=
_tmp6E7;continue;_LL391: if(_tmp6CC <= (void*)1)goto _LL393;if(*((int*)_tmp6CC)!= 
13)goto _LL393;_tmp6E8=((struct Cyc_Absyn_Do_s_struct*)_tmp6CC)->f1;_tmp6E9=((
struct Cyc_Absyn_Do_s_struct*)_tmp6CC)->f2;_tmp6EA=_tmp6E9.f1;_LL392:{struct
_RegionHandle _tmp72F=_new_region("temp");struct _RegionHandle*temp=& _tmp72F;
_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp6E8);Cyc_Toc_exp_to_c(
nv,_tmp6EA);;_pop_region(temp);}return;_LL393: if(_tmp6CC <= (void*)1)goto _LL395;
if(*((int*)_tmp6CC)!= 14)goto _LL395;_tmp6EB=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp6CC)->f1;_tmp6EC=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp6CC)->f2;_LL394: {
struct _tuple1*h_var=Cyc_Toc_temp_var();struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ);void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);{struct Cyc_Core_Opt*_tmpB74;
e_exp->topt=((_tmpB74=_cycalloc(sizeof(*_tmpB74)),((_tmpB74->v=(void*)e_typ,
_tmpB74))));}{struct _RegionHandle _tmp731=_new_region("temp");struct _RegionHandle*
temp=& _tmp731;_push_region(temp);{struct Cyc_Toc_Env*_tmp732=Cyc_Toc_add_varmap(
temp,nv,e_var,e_exp);Cyc_Toc_stmt_to_c(_tmp732,_tmp6EB);{struct Cyc_Absyn_Stmt*
_tmp733=Cyc_Absyn_seq_stmt(_tmp6EB,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,
0,0),0),0);struct _tuple1*_tmp734=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp735=
Cyc_Absyn_var_exp(_tmp734,0);struct Cyc_Absyn_Vardecl*_tmp736=Cyc_Absyn_new_vardecl(
_tmp734,Cyc_Absyn_exn_typ,0);{struct Cyc_Core_Opt*_tmpB75;_tmp735->topt=((_tmpB75=
_cycalloc(sizeof(*_tmpB75)),((_tmpB75->v=(void*)Cyc_Absyn_exn_typ,_tmpB75))));}{
struct Cyc_Core_Opt*_tmpB86;struct Cyc_Absyn_Var_p_struct*_tmpB85;struct Cyc_Absyn_Pat*
_tmpB84;struct Cyc_Core_Opt*_tmpB83;struct Cyc_Absyn_Var_p_struct _tmpB82;struct Cyc_Absyn_Pat*
_tmpB81;struct Cyc_Absyn_Pat*_tmp738=(_tmpB81=_cycalloc(sizeof(*_tmpB81)),((
_tmpB81->r=(void*)((void*)((_tmpB85=_cycalloc(sizeof(*_tmpB85)),((_tmpB85[0]=((
_tmpB82.tag=0,((_tmpB82.f1=_tmp736,((_tmpB82.f2=((_tmpB84=_cycalloc(sizeof(*
_tmpB84)),((_tmpB84->r=(void*)((void*)0),((_tmpB84->topt=((_tmpB83=_cycalloc(
sizeof(*_tmpB83)),((_tmpB83->v=(void*)Cyc_Absyn_exn_typ,_tmpB83)))),((_tmpB84->loc=
0,_tmpB84)))))))),_tmpB82)))))),_tmpB85))))),((_tmpB81->topt=((_tmpB86=_cycalloc(
sizeof(*_tmpB86)),((_tmpB86->v=(void*)Cyc_Absyn_exn_typ,_tmpB86)))),((_tmpB81->loc=
0,_tmpB81)))))));struct Cyc_Absyn_Exp*_tmp739=Cyc_Absyn_throw_exp(_tmp735,0);{
struct Cyc_Core_Opt*_tmpB87;_tmp739->topt=((_tmpB87=_cycalloc(sizeof(*_tmpB87)),((
_tmpB87->v=(void*)((void*)0),_tmpB87))));}{struct Cyc_Absyn_Stmt*_tmp73B=Cyc_Absyn_exp_stmt(
_tmp739,0);struct Cyc_Core_Opt*_tmpB8D;struct Cyc_List_List*_tmpB8C;struct Cyc_Absyn_Switch_clause*
_tmpB8B;struct Cyc_Absyn_Switch_clause*_tmp73C=(_tmpB8B=_cycalloc(sizeof(*_tmpB8B)),((
_tmpB8B->pattern=_tmp738,((_tmpB8B->pat_vars=((_tmpB8D=_cycalloc(sizeof(*_tmpB8D)),((
_tmpB8D->v=((_tmpB8C=_cycalloc(sizeof(*_tmpB8C)),((_tmpB8C->hd=_tmp736,((_tmpB8C->tl=
0,_tmpB8C)))))),_tmpB8D)))),((_tmpB8B->where_clause=0,((_tmpB8B->body=_tmp73B,((
_tmpB8B->loc=0,_tmpB8B)))))))))));struct Cyc_List_List*_tmpB8E;struct Cyc_Absyn_Stmt*
_tmp73D=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(_tmp6EC,((_tmpB8E=_cycalloc(sizeof(*
_tmpB8E)),((_tmpB8E->hd=_tmp73C,((_tmpB8E->tl=0,_tmpB8E))))))),0);Cyc_Toc_stmt_to_c(
_tmp732,_tmp73D);{struct Cyc_List_List*_tmpB8F;struct Cyc_Absyn_Exp*_tmp73E=Cyc_Absyn_fncall_exp(
Cyc_Toc_setjmp_e,((_tmpB8F=_cycalloc(sizeof(*_tmpB8F)),((_tmpB8F->hd=Cyc_Absyn_aggrmember_exp(
h_exp,Cyc_Toc_handler_sp,0),((_tmpB8F->tl=0,_tmpB8F)))))),0);struct Cyc_List_List*
_tmpB90;struct Cyc_Absyn_Stmt*_tmp73F=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,((
_tmpB90=_cycalloc(sizeof(*_tmpB90)),((_tmpB90->hd=Cyc_Absyn_address_exp(h_exp,0),((
_tmpB90->tl=0,_tmpB90)))))),0),0);struct Cyc_Absyn_Exp*_tmp740=Cyc_Absyn_int_exp((
void*)0,0,0);struct Cyc_Absyn_Exp*_tmp741=Cyc_Absyn_int_exp((void*)0,1,0);(void*)(
s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(
_tmp73F,Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)
_tmp740,Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp73E,Cyc_Absyn_assign_stmt(
was_thrown_exp,_tmp741,0),Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_prim1_exp((void*)11,was_thrown_exp,0),_tmp733,Cyc_Absyn_declare_stmt(
e_var,e_typ,(struct Cyc_Absyn_Exp*)Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),
_tmp73D,0),0),0),0),0),0))->r));}}}}};_pop_region(temp);}return;}_LL395: if(
_tmp6CC <= (void*)1)goto _LL397;if(*((int*)_tmp6CC)!= 15)goto _LL397;_tmp6ED=((
struct Cyc_Absyn_Region_s_struct*)_tmp6CC)->f1;_tmp6EE=((struct Cyc_Absyn_Region_s_struct*)
_tmp6CC)->f2;_tmp6EF=((struct Cyc_Absyn_Region_s_struct*)_tmp6CC)->f3;_tmp6F0=((
struct Cyc_Absyn_Region_s_struct*)_tmp6CC)->f4;_tmp6F1=((struct Cyc_Absyn_Region_s_struct*)
_tmp6CC)->f5;_LL396: {void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);struct
_tuple1*rh_var=Cyc_Toc_temp_var();struct _tuple1*x_var=_tmp6EE->name;struct Cyc_Absyn_Exp*
rh_exp=Cyc_Absyn_var_exp(rh_var,0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(
x_var,0);{struct _RegionHandle _tmp74E=_new_region("temp");struct _RegionHandle*
temp=& _tmp74E;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,
x_var,x_exp),_tmp6F1);;_pop_region(temp);}if(Cyc_Absyn_no_regions)(void*)(s->r=(
void*)((void*)(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(0,0),_tmp6F1,0))->r));else{if(_tmp6F0 == 0){struct Cyc_Absyn_Exp*
_tmpB93[1];struct Cyc_Absyn_Exp*_tmpB92[1];struct Cyc_List_List*_tmpB91;(void*)(s->r=(
void*)((void*)(Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,((_tmpB91=_cycalloc(sizeof(*_tmpB91)),((
_tmpB91->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0),((_tmpB91->tl=
0,_tmpB91)))))),0),Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_address_exp(rh_exp,0),Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__push_region_e,((_tmpB92[0]=x_exp,((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmpB92,sizeof(struct Cyc_Absyn_Exp*),
1)))),0),0),Cyc_Absyn_seq_stmt(_tmp6F1,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__pop_region_e,((_tmpB93[0]=x_exp,((struct Cyc_List_List*(*)(struct
_dynforward_ptr))Cyc_List_list)(_tag_dynforward(_tmpB93,sizeof(struct Cyc_Absyn_Exp*),
1)))),0),0),0),0),0),0))->r));}else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_tmp6F0);{struct Cyc_Absyn_Exp*_tmpB95[1];struct Cyc_Absyn_Exp*_tmpB94[2];(void*)(
s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(rh_var,Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),
0,Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(
Cyc_Toc__open_dynregion_e,((_tmpB94[1]=(struct Cyc_Absyn_Exp*)_tmp6F0,((_tmpB94[0]=
Cyc_Absyn_address_exp(rh_exp,0),((struct Cyc_List_List*(*)(struct _dynforward_ptr))
Cyc_List_list)(_tag_dynforward(_tmpB94,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
Cyc_Absyn_seq_stmt(_tmp6F1,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,((
_tmpB95[0]=x_exp,((struct Cyc_List_List*(*)(struct _dynforward_ptr))Cyc_List_list)(
_tag_dynforward(_tmpB95,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0))->r));}}}
return;}_LL397: if(_tmp6CC <= (void*)1)goto _LL399;if(*((int*)_tmp6CC)!= 16)goto
_LL399;_tmp6F2=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp6CC)->f1;_LL398: if(
Cyc_Absyn_no_regions)(void*)(s->r=(void*)((void*)0));else{Cyc_Toc_exp_to_c(nv,
_tmp6F2);{struct Cyc_List_List*_tmpB96;(void*)(s->r=(void*)Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(
Cyc_Toc__reset_region_e,((_tmpB96=_cycalloc(sizeof(*_tmpB96)),((_tmpB96->hd=
_tmp6F2,((_tmpB96->tl=0,_tmpB96)))))),0)));}}return;_LL399: if(_tmp6CC <= (void*)1)
goto _LL374;if(*((int*)_tmp6CC)!= 17)goto _LL374;_tmp6F3=((struct Cyc_Absyn_Alias_s_struct*)
_tmp6CC)->f1;_tmp6F4=((struct Cyc_Absyn_Alias_s_struct*)_tmp6CC)->f2;_tmp6F5=((
struct Cyc_Absyn_Alias_s_struct*)_tmp6CC)->f3;_tmp6F6=((struct Cyc_Absyn_Alias_s_struct*)
_tmp6CC)->f4;_LL39A: {struct _tuple1*old_name=_tmp6F5->name;struct _tuple1*
new_name=Cyc_Toc_temp_var();_tmp6F5->name=new_name;_tmp6F5->initializer=(struct
Cyc_Absyn_Exp*)_tmp6F3;{struct Cyc_Absyn_Decl_s_struct _tmpBA5;struct Cyc_Absyn_Var_d_struct*
_tmpBA4;struct Cyc_Absyn_Var_d_struct _tmpBA3;struct Cyc_Absyn_Decl*_tmpBA2;struct
Cyc_Absyn_Decl_s_struct*_tmpBA1;(void*)(s->r=(void*)((void*)((_tmpBA1=_cycalloc(
sizeof(*_tmpBA1)),((_tmpBA1[0]=((_tmpBA5.tag=11,((_tmpBA5.f1=((_tmpBA2=_cycalloc(
sizeof(*_tmpBA2)),((_tmpBA2->r=(void*)((void*)((_tmpBA4=_cycalloc(sizeof(*
_tmpBA4)),((_tmpBA4[0]=((_tmpBA3.tag=0,((_tmpBA3.f1=_tmp6F5,_tmpBA3)))),_tmpBA4))))),((
_tmpBA2->loc=0,_tmpBA2)))))),((_tmpBA5.f2=_tmp6F6,_tmpBA5)))))),_tmpBA1))))));}{
struct _RegionHandle _tmp75A=_new_region("temp");struct _RegionHandle*temp=& _tmp75A;
_push_region(temp);{struct Cyc_Absyn_Local_b_struct _tmpBA8;struct Cyc_Absyn_Local_b_struct*
_tmpBA7;struct Cyc_Toc_Env*_tmp75B=Cyc_Toc_add_varmap(temp,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)((_tmpBA7=_cycalloc(sizeof(*_tmpBA7)),((_tmpBA7[0]=((_tmpBA8.tag=
3,((_tmpBA8.f1=_tmp6F5,_tmpBA8)))),_tmpBA7)))),0));Cyc_Toc_local_decl_to_c(
_tmp75B,nv,_tmp6F5,_tmp6F6);};_pop_region(temp);}return;}_LL374:;}}static void Cyc_Toc_stmttypes_to_c(
struct Cyc_Absyn_Stmt*s);struct _tuple17{struct _dynforward_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*
f,int cinclude);static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*
f,int cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;(void*)(f->ret_type=(void*)Cyc_Toc_typ_to_c((
void*)f->ret_type));{struct _RegionHandle _tmp75E=_new_region("frgn");struct
_RegionHandle*frgn=& _tmp75E;_push_region(frgn);{struct Cyc_Toc_Env*_tmp75F=Cyc_Toc_share_env(
frgn,nv);{struct Cyc_List_List*_tmp760=f->args;for(0;_tmp760 != 0;_tmp760=_tmp760->tl){
union Cyc_Absyn_Nmspace_union _tmpBAB;struct _tuple1*_tmpBAA;struct _tuple1*_tmp761=(
_tmpBAA=_cycalloc(sizeof(*_tmpBAA)),((_tmpBAA->f1=(union Cyc_Absyn_Nmspace_union)((
union Cyc_Absyn_Nmspace_union)(((_tmpBAB.Loc_n).tag=0,_tmpBAB))),((_tmpBAA->f2=(*((
struct _tuple17*)_tmp760->hd)).f1,_tmpBAA)))));(*((struct _tuple17*)_tmp760->hd)).f3=
Cyc_Toc_typ_to_c((*((struct _tuple17*)_tmp760->hd)).f3);_tmp75F=Cyc_Toc_add_varmap(
frgn,_tmp75F,_tmp761,Cyc_Absyn_var_exp(_tmp761,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(
f->body);_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp765;struct Cyc_Absyn_Tqual
_tmp766;void*_tmp767;int _tmp768;struct Cyc_Absyn_VarargInfo _tmp764=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp765=_tmp764.name;_tmp766=_tmp764.tq;_tmp767=(
void*)_tmp764.type;_tmp768=_tmp764.inject;{void*_tmp769=Cyc_Toc_typ_to_c(Cyc_Absyn_dynforward_typ(
_tmp767,(void*)2,_tmp766,Cyc_Absyn_false_conref));union Cyc_Absyn_Nmspace_union
_tmpBAE;struct _tuple1*_tmpBAD;struct _tuple1*_tmp76A=(_tmpBAD=_cycalloc(sizeof(*
_tmpBAD)),((_tmpBAD->f1=(union Cyc_Absyn_Nmspace_union)((union Cyc_Absyn_Nmspace_union)(((
_tmpBAE.Loc_n).tag=0,_tmpBAE))),((_tmpBAD->f2=(struct _dynforward_ptr*)((struct
Cyc_Core_Opt*)_check_null(_tmp765))->v,_tmpBAD)))));{struct _tuple17*_tmpBB1;
struct Cyc_List_List*_tmpBB0;f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpBB0=_cycalloc(sizeof(*
_tmpBB0)),((_tmpBB0->hd=((_tmpBB1=_cycalloc(sizeof(*_tmpBB1)),((_tmpBB1->f1=(
struct _dynforward_ptr*)_tmp765->v,((_tmpBB1->f2=_tmp766,((_tmpBB1->f3=_tmp769,
_tmpBB1)))))))),((_tmpBB0->tl=0,_tmpBB0)))))));}_tmp75F=Cyc_Toc_add_varmap(frgn,
_tmp75F,_tmp76A,Cyc_Absyn_var_exp(_tmp76A,0));f->cyc_varargs=0;}}{struct Cyc_List_List*
_tmp76F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp76F != 0;_tmp76F=_tmp76F->tl){(void*)(((struct Cyc_Absyn_Vardecl*)
_tmp76F->hd)->type=(void*)Cyc_Toc_typ_to_c((void*)((struct Cyc_Absyn_Vardecl*)
_tmp76F->hd)->type));}}Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp75F),f->body);};
_pop_region(frgn);}}static void*Cyc_Toc_scope_to_c(void*s);static void*Cyc_Toc_scope_to_c(
void*s){void*_tmp770=s;_LL3AC: if((int)_tmp770 != 1)goto _LL3AE;_LL3AD: return(void*)
2;_LL3AE: if((int)_tmp770 != 4)goto _LL3B0;_LL3AF: return(void*)3;_LL3B0:;_LL3B1:
return s;_LL3AB:;}static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad){struct _tuple1*
_tmp771=ad->name;struct _DynRegionHandle*_tmp773;struct Cyc_Dict_Dict*_tmp774;
struct Cyc_Toc_TocState _tmp772=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp773=_tmp772.dyn;_tmp774=_tmp772.aggrs_so_far;{struct _DynRegionFrame _tmp775;
struct _RegionHandle*d=_open_dynregion(& _tmp775,_tmp773);{int seen_defn_before;
struct _tuple7**_tmp776=((struct _tuple7**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))
Cyc_Dict_lookup_opt)(*_tmp774,_tmp771);if(_tmp776 == 0){seen_defn_before=0;{
struct _tuple7*v;if((void*)ad->kind == (void*)0){struct _tuple7*_tmpBB2;v=((_tmpBB2=
_region_malloc(d,sizeof(*_tmpBB2)),((_tmpBB2->f1=ad,((_tmpBB2->f2=Cyc_Absyn_strctq(
ad->name),_tmpBB2))))));}else{struct _tuple7*_tmpBB3;v=((_tmpBB3=_region_malloc(d,
sizeof(*_tmpBB3)),((_tmpBB3->f1=ad,((_tmpBB3->f2=Cyc_Absyn_unionq_typ(ad->name),
_tmpBB3))))));}*_tmp774=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct
_tuple1*k,struct _tuple7*v))Cyc_Dict_insert)(*_tmp774,_tmp771,v);}}else{struct
_tuple7 _tmp77A;struct Cyc_Absyn_Aggrdecl*_tmp77B;void*_tmp77C;struct _tuple7*
_tmp779=*_tmp776;_tmp77A=*_tmp779;_tmp77B=_tmp77A.f1;_tmp77C=_tmp77A.f2;if(
_tmp77B->impl == 0){{struct _tuple7*_tmpBB4;*_tmp774=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple7*v))Cyc_Dict_insert)(*_tmp774,
_tmp771,((_tmpBB4=_region_malloc(d,sizeof(*_tmpBB4)),((_tmpBB4->f1=ad,((_tmpBB4->f2=
_tmp77C,_tmpBB4)))))));}seen_defn_before=0;}else{seen_defn_before=1;}}(void*)(ad->sc=(
void*)((void*)2));ad->tvs=0;if(ad->impl != 0){((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->exist_vars=0;((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
ad->impl))->rgn_po=0;if(seen_defn_before)ad->impl=0;else{struct Cyc_List_List*
_tmp77E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;
_tmp77E != 0;_tmp77E=_tmp77E->tl){((struct Cyc_Absyn_Aggrfield*)_tmp77E->hd)->tq=
Cyc_Toc_mt_tq;(void*)(((struct Cyc_Absyn_Aggrfield*)_tmp77E->hd)->type=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp77E->hd)->type));}}}};_pop_dynregion(d);}}
static void Cyc_Toc_tuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*tud);static void Cyc_Toc_tuniondecl_to_c(
struct Cyc_Absyn_Tuniondecl*tud){struct _DynRegionHandle*_tmp780;struct Cyc_Set_Set**
_tmp781;struct Cyc_Toc_TocState _tmp77F=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp780=_tmp77F.dyn;_tmp781=_tmp77F.tunions_so_far;{struct _DynRegionFrame _tmp782;
struct _RegionHandle*d=_open_dynregion(& _tmp782,_tmp780);{struct _tuple1*_tmp783=
tud->name;if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))
Cyc_Set_member)(*_tmp781,_tmp783)){_npop_handler(0);return;}*_tmp781=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*
_tmp781,_tmp783);};_pop_dynregion(d);}{struct Cyc_List_List*flat_structs=0;{
struct Cyc_List_List*_tmp784=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp784 != 0;_tmp784=_tmp784->tl){struct Cyc_Absyn_Tunionfield*
f=(struct Cyc_Absyn_Tunionfield*)_tmp784->hd;if(f->typs != 0  || tud->is_flat){
struct Cyc_List_List*_tmp785=0;int i=1;{struct Cyc_List_List*_tmp786=f->typs;for(0;
_tmp786 != 0;(_tmp786=_tmp786->tl,i ++)){struct _dynforward_ptr*_tmp787=Cyc_Absyn_fieldname(
i);struct Cyc_Absyn_Aggrfield*_tmpBB5;struct Cyc_Absyn_Aggrfield*_tmp788=(_tmpBB5=
_cycalloc(sizeof(*_tmpBB5)),((_tmpBB5->name=_tmp787,((_tmpBB5->tq=(*((struct
_tuple4*)_tmp786->hd)).f1,((_tmpBB5->type=(void*)Cyc_Toc_typ_to_c_array((*((
struct _tuple4*)_tmp786->hd)).f2),((_tmpBB5->width=0,((_tmpBB5->attributes=0,
_tmpBB5)))))))))));struct Cyc_List_List*_tmpBB6;_tmp785=((_tmpBB6=_cycalloc(
sizeof(*_tmpBB6)),((_tmpBB6->hd=_tmp788,((_tmpBB6->tl=_tmp785,_tmpBB6))))));}}{
struct Cyc_Absyn_Aggrfield*_tmpBB9;struct Cyc_List_List*_tmpBB8;_tmp785=((_tmpBB8=
_cycalloc(sizeof(*_tmpBB8)),((_tmpBB8->hd=((_tmpBB9=_cycalloc(sizeof(*_tmpBB9)),((
_tmpBB9->name=Cyc_Toc_tag_sp,((_tmpBB9->tq=Cyc_Toc_mt_tq,((_tmpBB9->type=(void*)
Cyc_Absyn_sint_typ,((_tmpBB9->width=0,((_tmpBB9->attributes=0,_tmpBB9)))))))))))),((
_tmpBB8->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp785),_tmpBB8))))));}{struct Cyc_Absyn_AggrdeclImpl*_tmpBBE;const char*_tmpBBD;
struct Cyc_Absyn_Aggrdecl*_tmpBBC;struct Cyc_Absyn_Aggrdecl*_tmp78D=(_tmpBBC=
_cycalloc(sizeof(*_tmpBBC)),((_tmpBBC->kind=(void*)((void*)0),((_tmpBBC->sc=(
void*)((void*)2),((_tmpBBC->name=Cyc_Toc_collapse_qvar_tag(f->name,((_tmpBBD="_struct",
_tag_dynforward(_tmpBBD,sizeof(char),_get_zero_arr_size_char(_tmpBBD,8))))),((
_tmpBBC->tvs=0,((_tmpBBC->impl=((_tmpBBE=_cycalloc(sizeof(*_tmpBBE)),((_tmpBBE->exist_vars=
0,((_tmpBBE->rgn_po=0,((_tmpBBE->fields=_tmp785,_tmpBBE)))))))),((_tmpBBC->attributes=
0,_tmpBBC)))))))))))));{struct Cyc_Absyn_Aggr_d_struct*_tmpBC4;struct Cyc_Absyn_Aggr_d_struct
_tmpBC3;struct Cyc_List_List*_tmpBC2;Cyc_Toc_result_decls=((_tmpBC2=_cycalloc(
sizeof(*_tmpBC2)),((_tmpBC2->hd=Cyc_Absyn_new_decl((void*)((_tmpBC4=_cycalloc(
sizeof(*_tmpBC4)),((_tmpBC4[0]=((_tmpBC3.tag=4,((_tmpBC3.f1=_tmp78D,_tmpBC3)))),
_tmpBC4)))),0),((_tmpBC2->tl=Cyc_Toc_result_decls,_tmpBC2))))));}if(tud->is_flat){
struct Cyc_Absyn_AggrType_struct*_tmpBD9;struct Cyc_Absyn_AggrInfo _tmpBD8;struct
Cyc_Absyn_Aggrdecl**_tmpBD7;union Cyc_Absyn_AggrInfoU_union _tmpBD6;struct Cyc_Absyn_AggrType_struct
_tmpBD5;struct Cyc_Absyn_Aggrfield*_tmpBD4;struct Cyc_Absyn_Aggrfield*_tmp791=(
_tmpBD4=_cycalloc(sizeof(*_tmpBD4)),((_tmpBD4->name=(*f->name).f2,((_tmpBD4->tq=
Cyc_Toc_mt_tq,((_tmpBD4->type=(void*)((void*)((_tmpBD9=_cycalloc(sizeof(*_tmpBD9)),((
_tmpBD9[0]=((_tmpBD5.tag=10,((_tmpBD5.f1=((_tmpBD8.aggr_info=(union Cyc_Absyn_AggrInfoU_union)(((
_tmpBD6.KnownAggr).tag=1,(((_tmpBD6.KnownAggr).f1=((_tmpBD7=_cycalloc(sizeof(*
_tmpBD7)),((_tmpBD7[0]=_tmp78D,_tmpBD7)))),_tmpBD6)))),((_tmpBD8.targs=0,_tmpBD8)))),
_tmpBD5)))),_tmpBD9))))),((_tmpBD4->width=0,((_tmpBD4->attributes=0,_tmpBD4)))))))))));
struct Cyc_List_List*_tmpBDA;flat_structs=((_tmpBDA=_cycalloc(sizeof(*_tmpBDA)),((
_tmpBDA->hd=_tmp791,((_tmpBDA->tl=flat_structs,_tmpBDA))))));}}}}}if(tud->is_flat){
flat_structs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
flat_structs);{struct Cyc_Absyn_AggrdeclImpl*_tmpBDF;const char*_tmpBDE;struct Cyc_Absyn_Aggrdecl*
_tmpBDD;struct Cyc_Absyn_Aggrdecl*_tmp79C=(_tmpBDD=_cycalloc(sizeof(*_tmpBDD)),((
_tmpBDD->kind=(void*)((void*)1),((_tmpBDD->sc=(void*)((void*)2),((_tmpBDD->name=
Cyc_Toc_collapse_qvar_tag(tud->name,((_tmpBDE="_union",_tag_dynforward(_tmpBDE,
sizeof(char),_get_zero_arr_size_char(_tmpBDE,7))))),((_tmpBDD->tvs=0,((_tmpBDD->impl=((
_tmpBDF=_cycalloc(sizeof(*_tmpBDF)),((_tmpBDF->exist_vars=0,((_tmpBDF->rgn_po=0,((
_tmpBDF->fields=flat_structs,_tmpBDF)))))))),((_tmpBDD->attributes=0,_tmpBDD)))))))))))));
struct Cyc_Absyn_Aggr_d_struct*_tmpBE5;struct Cyc_Absyn_Aggr_d_struct _tmpBE4;
struct Cyc_List_List*_tmpBE3;Cyc_Toc_result_decls=((_tmpBE3=_cycalloc(sizeof(*
_tmpBE3)),((_tmpBE3->hd=Cyc_Absyn_new_decl((void*)((_tmpBE5=_cycalloc(sizeof(*
_tmpBE5)),((_tmpBE5[0]=((_tmpBE4.tag=4,((_tmpBE4.f1=_tmp79C,_tmpBE4)))),_tmpBE5)))),
0),((_tmpBE3->tl=Cyc_Toc_result_decls,_tmpBE3))))));}}}}static void Cyc_Toc_xtuniondecl_to_c(
struct Cyc_Absyn_Tuniondecl*xd);static void Cyc_Toc_xtuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*
xd){if(xd->fields == 0)return;{struct _DynRegionHandle*_tmp7A4;struct Cyc_Dict_Dict*
_tmp7A5;struct Cyc_Toc_TocState _tmp7A3=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));
_tmp7A4=_tmp7A3.dyn;_tmp7A5=_tmp7A3.xtunions_so_far;{struct _DynRegionFrame
_tmp7A6;struct _RegionHandle*d=_open_dynregion(& _tmp7A6,_tmp7A4);{struct _tuple1*
_tmp7A7=xd->name;struct Cyc_List_List*_tmp7A8=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(xd->fields))->v;for(0;_tmp7A8 != 0;_tmp7A8=_tmp7A8->tl){struct Cyc_Absyn_Tunionfield*
f=(struct Cyc_Absyn_Tunionfield*)_tmp7A8->hd;struct _dynforward_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp7A9=Cyc_Absyn_uint_exp(_get_dynforward_size(*fn,sizeof(
char))+ 4,0);void*_tmp7AA=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)_tmp7A9,Cyc_Absyn_false_conref,0);int*_tmp7AB=((int*(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp7A5,f->name);int
_tmp7AC;_LL3B3: if(_tmp7AB != 0)goto _LL3B5;_LL3B4: {struct Cyc_Absyn_Exp*initopt=0;
if((void*)f->sc != (void*)3){char zero='\000';const char*_tmpBED;void*_tmpBEC[5];
struct Cyc_Int_pa_struct _tmpBEB;struct Cyc_Int_pa_struct _tmpBEA;struct Cyc_Int_pa_struct
_tmpBE9;struct Cyc_Int_pa_struct _tmpBE8;struct Cyc_String_pa_struct _tmpBE7;initopt=(
struct Cyc_Absyn_Exp*)Cyc_Absyn_string_exp((struct _dynforward_ptr)((_tmpBE7.tag=0,((
_tmpBE7.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*fn),((_tmpBE8.tag=1,((
_tmpBE8.f1=(unsigned long)((int)zero),((_tmpBE9.tag=1,((_tmpBE9.f1=(
unsigned long)((int)zero),((_tmpBEA.tag=1,((_tmpBEA.f1=(unsigned long)((int)zero),((
_tmpBEB.tag=1,((_tmpBEB.f1=(unsigned long)((int)zero),((_tmpBEC[0]=& _tmpBEB,((
_tmpBEC[1]=& _tmpBEA,((_tmpBEC[2]=& _tmpBE9,((_tmpBEC[3]=& _tmpBE8,((_tmpBEC[4]=&
_tmpBE7,Cyc_aprintf(((_tmpBED="%c%c%c%c%s",_tag_dynforward(_tmpBED,sizeof(char),
_get_zero_arr_size_char(_tmpBED,11)))),_tag_dynforward(_tmpBEC,sizeof(void*),5)))))))))))))))))))))))))))))))),
0);}{struct Cyc_Absyn_Vardecl*_tmp7B4=Cyc_Absyn_new_vardecl(f->name,_tmp7AA,
initopt);(void*)(_tmp7B4->sc=(void*)((void*)f->sc));{struct Cyc_Absyn_Var_d_struct*
_tmpBF3;struct Cyc_Absyn_Var_d_struct _tmpBF2;struct Cyc_List_List*_tmpBF1;Cyc_Toc_result_decls=((
_tmpBF1=_cycalloc(sizeof(*_tmpBF1)),((_tmpBF1->hd=Cyc_Absyn_new_decl((void*)((
_tmpBF3=_cycalloc(sizeof(*_tmpBF3)),((_tmpBF3[0]=((_tmpBF2.tag=0,((_tmpBF2.f1=
_tmp7B4,_tmpBF2)))),_tmpBF3)))),0),((_tmpBF1->tl=Cyc_Toc_result_decls,_tmpBF1))))));}*
_tmp7A5=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*
_tmp7A5,f->name,(void*)f->sc != (void*)3);if(f->typs != 0){struct Cyc_List_List*
fields=0;int i=1;{struct Cyc_List_List*_tmp7B8=f->typs;for(0;_tmp7B8 != 0;(_tmp7B8=
_tmp7B8->tl,i ++)){struct Cyc_Int_pa_struct _tmpBFB;void*_tmpBFA[1];const char*
_tmpBF9;struct _dynforward_ptr*_tmpBF8;struct _dynforward_ptr*_tmp7B9=(_tmpBF8=
_cycalloc(sizeof(*_tmpBF8)),((_tmpBF8[0]=(struct _dynforward_ptr)((_tmpBFB.tag=1,((
_tmpBFB.f1=(unsigned long)i,((_tmpBFA[0]=& _tmpBFB,Cyc_aprintf(((_tmpBF9="f%d",
_tag_dynforward(_tmpBF9,sizeof(char),_get_zero_arr_size_char(_tmpBF9,4)))),
_tag_dynforward(_tmpBFA,sizeof(void*),1)))))))),_tmpBF8)));struct Cyc_Absyn_Aggrfield*
_tmpBFC;struct Cyc_Absyn_Aggrfield*_tmp7BA=(_tmpBFC=_cycalloc(sizeof(*_tmpBFC)),((
_tmpBFC->name=_tmp7B9,((_tmpBFC->tq=(*((struct _tuple4*)_tmp7B8->hd)).f1,((
_tmpBFC->type=(void*)Cyc_Toc_typ_to_c_array((*((struct _tuple4*)_tmp7B8->hd)).f2),((
_tmpBFC->width=0,((_tmpBFC->attributes=0,_tmpBFC)))))))))));struct Cyc_List_List*
_tmpBFD;fields=((_tmpBFD=_cycalloc(sizeof(*_tmpBFD)),((_tmpBFD->hd=_tmp7BA,((
_tmpBFD->tl=fields,_tmpBFD))))));}}{struct Cyc_Absyn_Aggrfield*_tmpC00;struct Cyc_List_List*
_tmpBFF;fields=((_tmpBFF=_cycalloc(sizeof(*_tmpBFF)),((_tmpBFF->hd=((_tmpC00=
_cycalloc(sizeof(*_tmpC00)),((_tmpC00->name=Cyc_Toc_tag_sp,((_tmpC00->tq=Cyc_Toc_mt_tq,((
_tmpC00->type=(void*)Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq),((
_tmpC00->width=0,((_tmpC00->attributes=0,_tmpC00)))))))))))),((_tmpBFF->tl=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmpBFF))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpC05;const char*_tmpC04;struct Cyc_Absyn_Aggrdecl*
_tmpC03;struct Cyc_Absyn_Aggrdecl*_tmp7C3=(_tmpC03=_cycalloc(sizeof(*_tmpC03)),((
_tmpC03->kind=(void*)((void*)0),((_tmpC03->sc=(void*)((void*)2),((_tmpC03->name=
Cyc_Toc_collapse_qvar_tag(f->name,((_tmpC04="_struct",_tag_dynforward(_tmpC04,
sizeof(char),_get_zero_arr_size_char(_tmpC04,8))))),((_tmpC03->tvs=0,((_tmpC03->impl=((
_tmpC05=_cycalloc(sizeof(*_tmpC05)),((_tmpC05->exist_vars=0,((_tmpC05->rgn_po=0,((
_tmpC05->fields=fields,_tmpC05)))))))),((_tmpC03->attributes=0,_tmpC03)))))))))))));
struct Cyc_Absyn_Aggr_d_struct*_tmpC0B;struct Cyc_Absyn_Aggr_d_struct _tmpC0A;
struct Cyc_List_List*_tmpC09;Cyc_Toc_result_decls=((_tmpC09=_cycalloc(sizeof(*
_tmpC09)),((_tmpC09->hd=Cyc_Absyn_new_decl((void*)((_tmpC0B=_cycalloc(sizeof(*
_tmpC0B)),((_tmpC0B[0]=((_tmpC0A.tag=4,((_tmpC0A.f1=_tmp7C3,_tmpC0A)))),_tmpC0B)))),
0),((_tmpC09->tl=Cyc_Toc_result_decls,_tmpC09))))));}}goto _LL3B2;}}_LL3B5: if(
_tmp7AB == 0)goto _LL3B7;_tmp7AC=*_tmp7AB;if(_tmp7AC != 0)goto _LL3B7;_LL3B6: if((
void*)f->sc != (void*)3){char zero='\000';const char*_tmpC13;void*_tmpC12[5];struct
Cyc_Int_pa_struct _tmpC11;struct Cyc_Int_pa_struct _tmpC10;struct Cyc_Int_pa_struct
_tmpC0F;struct Cyc_Int_pa_struct _tmpC0E;struct Cyc_String_pa_struct _tmpC0D;struct
Cyc_Absyn_Exp*_tmp7CA=Cyc_Absyn_string_exp((struct _dynforward_ptr)((_tmpC0D.tag=
0,((_tmpC0D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*fn),((_tmpC0E.tag=
1,((_tmpC0E.f1=(unsigned long)((int)zero),((_tmpC0F.tag=1,((_tmpC0F.f1=(
unsigned long)((int)zero),((_tmpC10.tag=1,((_tmpC10.f1=(unsigned long)((int)zero),((
_tmpC11.tag=1,((_tmpC11.f1=(unsigned long)((int)zero),((_tmpC12[0]=& _tmpC11,((
_tmpC12[1]=& _tmpC10,((_tmpC12[2]=& _tmpC0F,((_tmpC12[3]=& _tmpC0E,((_tmpC12[4]=&
_tmpC0D,Cyc_aprintf(((_tmpC13="%c%c%c%c%s",_tag_dynforward(_tmpC13,sizeof(char),
_get_zero_arr_size_char(_tmpC13,11)))),_tag_dynforward(_tmpC12,sizeof(void*),5)))))))))))))))))))))))))))))))),
0);struct Cyc_Absyn_Vardecl*_tmp7CB=Cyc_Absyn_new_vardecl(f->name,_tmp7AA,(struct
Cyc_Absyn_Exp*)_tmp7CA);(void*)(_tmp7CB->sc=(void*)((void*)f->sc));{struct Cyc_Absyn_Var_d_struct*
_tmpC19;struct Cyc_Absyn_Var_d_struct _tmpC18;struct Cyc_List_List*_tmpC17;Cyc_Toc_result_decls=((
_tmpC17=_cycalloc(sizeof(*_tmpC17)),((_tmpC17->hd=Cyc_Absyn_new_decl((void*)((
_tmpC19=_cycalloc(sizeof(*_tmpC19)),((_tmpC19[0]=((_tmpC18.tag=0,((_tmpC18.f1=
_tmp7CB,_tmpC18)))),_tmpC19)))),0),((_tmpC17->tl=Cyc_Toc_result_decls,_tmpC17))))));}*
_tmp7A5=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*
_tmp7A5,f->name,1);}goto _LL3B2;_LL3B7:;_LL3B8: goto _LL3B2;_LL3B2:;}};
_pop_dynregion(d);}}}static void Cyc_Toc_enumdecl_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Enumdecl*ed);static void Cyc_Toc_enumdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Enumdecl*ed){(void*)(ed->sc=(void*)((void*)2));if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,
struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*
init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){void*old_typ=(void*)
vd->type;(void*)(vd->type=(void*)Cyc_Toc_typ_to_c_array(old_typ));{int
forward_only=0;if((void*)vd->sc == (void*)5  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ,& forward_only))(void*)(vd->sc=(void*)((void*)2));Cyc_Toc_stmt_to_c(
body_nv,s);if(vd->initializer != 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)
_check_null(vd->initializer);void*_tmp7D6=(void*)init->r;struct Cyc_Absyn_Vardecl*
_tmp7D7;struct Cyc_Absyn_Exp*_tmp7D8;struct Cyc_Absyn_Exp*_tmp7D9;int _tmp7DA;
_LL3BA: if(*((int*)_tmp7D6)!= 29)goto _LL3BC;_tmp7D7=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp7D6)->f1;_tmp7D8=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp7D6)->f2;
_tmp7D9=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp7D6)->f3;_tmp7DA=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp7D6)->f4;_LL3BB: vd->initializer=0;(void*)(
s->r=(void*)((void*)(Cyc_Toc_init_comprehension(init_nv,Cyc_Absyn_var_exp(vd->name,
0),_tmp7D7,_tmp7D8,_tmp7D9,_tmp7DA,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));
goto _LL3B9;_LL3BC:;_LL3BD: if((void*)vd->sc == (void*)0){struct _RegionHandle
_tmp7DB=_new_region("temp");struct _RegionHandle*temp=& _tmp7DB;_push_region(temp);{
struct Cyc_Toc_Env*_tmp7DC=Cyc_Toc_set_toplevel(temp,init_nv);Cyc_Toc_exp_to_c(
_tmp7DC,init);};_pop_region(temp);}else{Cyc_Toc_exp_to_c(init_nv,init);}goto
_LL3B9;_LL3B9:;}else{void*_tmp7DD=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_ArrayInfo
_tmp7DE;void*_tmp7DF;struct Cyc_Absyn_Exp*_tmp7E0;struct Cyc_Absyn_Conref*_tmp7E1;
_LL3BF: if(_tmp7DD <= (void*)4)goto _LL3C1;if(*((int*)_tmp7DD)!= 7)goto _LL3C1;
_tmp7DE=((struct Cyc_Absyn_ArrayType_struct*)_tmp7DD)->f1;_tmp7DF=(void*)_tmp7DE.elt_type;
_tmp7E0=_tmp7DE.num_elts;_tmp7E1=_tmp7DE.zero_term;if(!((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp7E1))goto _LL3C1;_LL3C0: if(_tmp7E0 == 0){const char*
_tmpC1C;void*_tmpC1B;(_tmpC1B=0,((int(*)(struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Toc_toc_impos)(((_tmpC1C="can't initialize zero-terminated array -- size unknown",
_tag_dynforward(_tmpC1C,sizeof(char),_get_zero_arr_size_char(_tmpC1C,55)))),
_tag_dynforward(_tmpC1B,sizeof(void*),0)));}{struct Cyc_Absyn_Exp*num_elts=(
struct Cyc_Absyn_Exp*)_tmp7E0;struct Cyc_Absyn_Exp*_tmp7E4=Cyc_Absyn_subscript_exp(
Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(
- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp7E5=Cyc_Absyn_signed_int_exp(0,0);(void*)(s->r=(
void*)Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp7E4,_tmp7E5,
0),0),Cyc_Absyn_new_stmt((void*)s->r,0)));goto _LL3BE;}_LL3C1:;_LL3C2: goto _LL3BE;
_LL3BE:;}}}static struct Cyc_Absyn_Stmt**Cyc_Toc_throw_match_stmt_opt=0;static
struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt();static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
if(Cyc_Toc_throw_match_stmt_opt == 0){struct Cyc_Absyn_Stmt**_tmpC1D;Cyc_Toc_throw_match_stmt_opt=((
_tmpC1D=_cycalloc(sizeof(*_tmpC1D)),((_tmpC1D[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(
Cyc_Absyn_match_exn_exp(0)),0),_tmpC1D))));}return*((struct Cyc_Absyn_Stmt**)
_check_null(Cyc_Toc_throw_match_stmt_opt));}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s);static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
Cyc_Toc_exp_to_c(nv,e);{struct _tuple1*x=Cyc_Toc_temp_var();{struct _RegionHandle
_tmp7E7=_new_region("prgn");struct _RegionHandle*prgn=& _tmp7E7;_push_region(prgn);{
struct Cyc_Absyn_Stmt*_tmp7E8=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*
_tmp7E9=Cyc_Toc_share_env(prgn,nv);struct Cyc_Toc_Env*_tmp7EB;struct Cyc_List_List*
_tmp7EC;struct Cyc_Absyn_Stmt*_tmp7ED;struct _tuple13 _tmp7EA=Cyc_Toc_xlate_pat(
_tmp7E9,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,(struct Cyc_Absyn_Stmt**)&
_tmp7E8,Cyc_Toc_throw_match_stmt(),0);_tmp7EB=_tmp7EA.f1;_tmp7EC=_tmp7EA.f2;
_tmp7ED=_tmp7EA.f3;Cyc_Toc_stmt_to_c(_tmp7EB,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(
t),(struct Cyc_Absyn_Exp*)e,Cyc_Absyn_seq_stmt(_tmp7ED,s,0),0);for(0;_tmp7EC != 0;
_tmp7EC=_tmp7EC->tl){struct _tuple14 _tmp7EF;struct _tuple1*_tmp7F0;void*_tmp7F1;
struct _tuple14*_tmp7EE=(struct _tuple14*)_tmp7EC->hd;_tmp7EF=*_tmp7EE;_tmp7F0=
_tmp7EF.f1;_tmp7F1=_tmp7EF.f2;s=Cyc_Absyn_declare_stmt(_tmp7F0,_tmp7F1,0,s,0);}};
_pop_region(prgn);}return s;}}static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*
e);static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){void*_tmp7F2=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp7F3;struct Cyc_Absyn_Exp*_tmp7F4;struct Cyc_Absyn_Exp*
_tmp7F5;struct Cyc_Absyn_Exp*_tmp7F6;struct Cyc_Absyn_Exp*_tmp7F7;struct Cyc_Absyn_Exp*
_tmp7F8;struct Cyc_Absyn_Exp*_tmp7F9;struct Cyc_Absyn_Exp*_tmp7FA;struct Cyc_List_List*
_tmp7FB;struct Cyc_Absyn_Exp*_tmp7FC;struct Cyc_Absyn_Exp*_tmp7FD;struct Cyc_Absyn_Exp*
_tmp7FE;struct Cyc_Absyn_Exp*_tmp7FF;struct Cyc_Absyn_Exp*_tmp800;struct Cyc_Absyn_Exp*
_tmp801;struct Cyc_Absyn_Exp*_tmp802;struct Cyc_Absyn_Exp*_tmp803;struct Cyc_Absyn_Exp*
_tmp804;struct Cyc_Absyn_Exp*_tmp805;struct Cyc_Absyn_Exp*_tmp806;struct Cyc_Absyn_Exp*
_tmp807;struct Cyc_Absyn_Exp*_tmp808;struct Cyc_Absyn_Exp*_tmp809;struct Cyc_Absyn_Exp*
_tmp80A;struct Cyc_Absyn_Exp*_tmp80B;struct Cyc_List_List*_tmp80C;struct Cyc_Absyn_Exp*
_tmp80D;struct Cyc_List_List*_tmp80E;void*_tmp80F;void**_tmp810;struct Cyc_Absyn_Exp*
_tmp811;struct _tuple2*_tmp812;struct _tuple2 _tmp813;void*_tmp814;void**_tmp815;
struct Cyc_List_List*_tmp816;struct Cyc_List_List*_tmp817;struct Cyc_List_List*
_tmp818;void*_tmp819;void**_tmp81A;void*_tmp81B;void**_tmp81C;struct Cyc_Absyn_Stmt*
_tmp81D;struct Cyc_Absyn_MallocInfo _tmp81E;struct Cyc_Absyn_MallocInfo*_tmp81F;
_LL3C4: if(*((int*)_tmp7F2)!= 22)goto _LL3C6;_tmp7F3=((struct Cyc_Absyn_Deref_e_struct*)
_tmp7F2)->f1;_LL3C5: _tmp7F4=_tmp7F3;goto _LL3C7;_LL3C6: if(*((int*)_tmp7F2)!= 23)
goto _LL3C8;_tmp7F4=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp7F2)->f1;_LL3C7:
_tmp7F5=_tmp7F4;goto _LL3C9;_LL3C8: if(*((int*)_tmp7F2)!= 24)goto _LL3CA;_tmp7F5=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp7F2)->f1;_LL3C9: _tmp7F6=_tmp7F5;goto
_LL3CB;_LL3CA: if(*((int*)_tmp7F2)!= 16)goto _LL3CC;_tmp7F6=((struct Cyc_Absyn_Address_e_struct*)
_tmp7F2)->f1;_LL3CB: _tmp7F7=_tmp7F6;goto _LL3CD;_LL3CC: if(*((int*)_tmp7F2)!= 12)
goto _LL3CE;_tmp7F7=((struct Cyc_Absyn_Throw_e_struct*)_tmp7F2)->f1;_LL3CD: _tmp7F8=
_tmp7F7;goto _LL3CF;_LL3CE: if(*((int*)_tmp7F2)!= 13)goto _LL3D0;_tmp7F8=((struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmp7F2)->f1;_LL3CF: _tmp7F9=_tmp7F8;goto _LL3D1;
_LL3D0: if(*((int*)_tmp7F2)!= 19)goto _LL3D2;_tmp7F9=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp7F2)->f1;_LL3D1: _tmp7FA=_tmp7F9;goto _LL3D3;_LL3D2: if(*((int*)_tmp7F2)!= 5)
goto _LL3D4;_tmp7FA=((struct Cyc_Absyn_Increment_e_struct*)_tmp7F2)->f1;_LL3D3: Cyc_Toc_exptypes_to_c(
_tmp7FA);goto _LL3C3;_LL3D4: if(*((int*)_tmp7F2)!= 3)goto _LL3D6;_tmp7FB=((struct
Cyc_Absyn_Primop_e_struct*)_tmp7F2)->f2;_LL3D5:((void(*)(void(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp7FB);goto _LL3C3;
_LL3D6: if(*((int*)_tmp7F2)!= 7)goto _LL3D8;_tmp7FC=((struct Cyc_Absyn_And_e_struct*)
_tmp7F2)->f1;_tmp7FD=((struct Cyc_Absyn_And_e_struct*)_tmp7F2)->f2;_LL3D7: _tmp7FE=
_tmp7FC;_tmp7FF=_tmp7FD;goto _LL3D9;_LL3D8: if(*((int*)_tmp7F2)!= 8)goto _LL3DA;
_tmp7FE=((struct Cyc_Absyn_Or_e_struct*)_tmp7F2)->f1;_tmp7FF=((struct Cyc_Absyn_Or_e_struct*)
_tmp7F2)->f2;_LL3D9: _tmp800=_tmp7FE;_tmp801=_tmp7FF;goto _LL3DB;_LL3DA: if(*((int*)
_tmp7F2)!= 9)goto _LL3DC;_tmp800=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp7F2)->f1;
_tmp801=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp7F2)->f2;_LL3DB: _tmp802=_tmp800;
_tmp803=_tmp801;goto _LL3DD;_LL3DC: if(*((int*)_tmp7F2)!= 25)goto _LL3DE;_tmp802=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp7F2)->f1;_tmp803=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp7F2)->f2;_LL3DD: _tmp804=_tmp802;_tmp805=_tmp803;goto _LL3DF;_LL3DE: if(*((int*)
_tmp7F2)!= 36)goto _LL3E0;_tmp804=((struct Cyc_Absyn_Swap_e_struct*)_tmp7F2)->f1;
_tmp805=((struct Cyc_Absyn_Swap_e_struct*)_tmp7F2)->f2;_LL3DF: _tmp806=_tmp804;
_tmp807=_tmp805;goto _LL3E1;_LL3E0: if(*((int*)_tmp7F2)!= 4)goto _LL3E2;_tmp806=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp7F2)->f1;_tmp807=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp7F2)->f3;_LL3E1: Cyc_Toc_exptypes_to_c(_tmp806);Cyc_Toc_exptypes_to_c(_tmp807);
goto _LL3C3;_LL3E2: if(*((int*)_tmp7F2)!= 6)goto _LL3E4;_tmp808=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp7F2)->f1;_tmp809=((struct Cyc_Absyn_Conditional_e_struct*)_tmp7F2)->f2;
_tmp80A=((struct Cyc_Absyn_Conditional_e_struct*)_tmp7F2)->f3;_LL3E3: Cyc_Toc_exptypes_to_c(
_tmp808);Cyc_Toc_exptypes_to_c(_tmp809);Cyc_Toc_exptypes_to_c(_tmp80A);goto
_LL3C3;_LL3E4: if(*((int*)_tmp7F2)!= 11)goto _LL3E6;_tmp80B=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp7F2)->f1;_tmp80C=((struct Cyc_Absyn_FnCall_e_struct*)_tmp7F2)->f2;_LL3E5:
_tmp80D=_tmp80B;_tmp80E=_tmp80C;goto _LL3E7;_LL3E6: if(*((int*)_tmp7F2)!= 10)goto
_LL3E8;_tmp80D=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp7F2)->f1;_tmp80E=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp7F2)->f2;_LL3E7: Cyc_Toc_exptypes_to_c(
_tmp80D);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp80E);goto _LL3C3;_LL3E8: if(*((int*)_tmp7F2)!= 15)goto
_LL3EA;_tmp80F=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp7F2)->f1;_tmp810=(
void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp7F2)->f1);_tmp811=((struct
Cyc_Absyn_Cast_e_struct*)_tmp7F2)->f2;_LL3E9:*_tmp810=Cyc_Toc_typ_to_c(*_tmp810);
Cyc_Toc_exptypes_to_c(_tmp811);goto _LL3C3;_LL3EA: if(*((int*)_tmp7F2)!= 27)goto
_LL3EC;_tmp812=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp7F2)->f1;_tmp813=*
_tmp812;_tmp814=_tmp813.f3;_tmp815=(void**)&(*((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp7F2)->f1).f3;_tmp816=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp7F2)->f2;
_LL3EB:*_tmp815=Cyc_Toc_typ_to_c(*_tmp815);_tmp817=_tmp816;goto _LL3ED;_LL3EC: if(*((
int*)_tmp7F2)!= 37)goto _LL3EE;_tmp817=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp7F2)->f2;_LL3ED: _tmp818=_tmp817;goto _LL3EF;_LL3EE: if(*((int*)_tmp7F2)!= 28)
goto _LL3F0;_tmp818=((struct Cyc_Absyn_Array_e_struct*)_tmp7F2)->f1;_LL3EF: for(0;
_tmp818 != 0;_tmp818=_tmp818->tl){struct Cyc_Absyn_Exp*_tmp821;struct _tuple8
_tmp820=*((struct _tuple8*)_tmp818->hd);_tmp821=_tmp820.f2;Cyc_Toc_exptypes_to_c(
_tmp821);}goto _LL3C3;_LL3F0: if(*((int*)_tmp7F2)!= 20)goto _LL3F2;_tmp819=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp7F2)->f1;_tmp81A=(void**)&((void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp7F2)->f1);_LL3F1: _tmp81C=_tmp81A;goto
_LL3F3;_LL3F2: if(*((int*)_tmp7F2)!= 18)goto _LL3F4;_tmp81B=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp7F2)->f1;_tmp81C=(void**)&((void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp7F2)->f1);_LL3F3:*_tmp81C=Cyc_Toc_typ_to_c(*_tmp81C);goto _LL3C3;_LL3F4: if(*((
int*)_tmp7F2)!= 38)goto _LL3F6;_tmp81D=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp7F2)->f1;_LL3F5: Cyc_Toc_stmttypes_to_c(_tmp81D);goto _LL3C3;_LL3F6: if(*((int*)
_tmp7F2)!= 35)goto _LL3F8;_tmp81E=((struct Cyc_Absyn_Malloc_e_struct*)_tmp7F2)->f1;
_tmp81F=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_struct*)_tmp7F2)->f1;
_LL3F7: if(_tmp81F->elt_type != 0){void**_tmpC1E;_tmp81F->elt_type=((_tmpC1E=
_cycalloc(sizeof(*_tmpC1E)),((_tmpC1E[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(
_tmp81F->elt_type))),_tmpC1E))));}Cyc_Toc_exptypes_to_c(_tmp81F->num_elts);goto
_LL3C3;_LL3F8: if(*((int*)_tmp7F2)!= 0)goto _LL3FA;_LL3F9: goto _LL3FB;_LL3FA: if(*((
int*)_tmp7F2)!= 1)goto _LL3FC;_LL3FB: goto _LL3FD;_LL3FC: if(*((int*)_tmp7F2)!= 2)
goto _LL3FE;_LL3FD: goto _LL3FF;_LL3FE: if(*((int*)_tmp7F2)!= 33)goto _LL400;_LL3FF:
goto _LL401;_LL400: if(*((int*)_tmp7F2)!= 34)goto _LL402;_LL401: goto _LL3C3;_LL402:
if(*((int*)_tmp7F2)!= 31)goto _LL404;_LL403: goto _LL405;_LL404: if(*((int*)_tmp7F2)
!= 32)goto _LL406;_LL405: goto _LL407;_LL406: if(*((int*)_tmp7F2)!= 30)goto _LL408;
_LL407: goto _LL409;_LL408: if(*((int*)_tmp7F2)!= 29)goto _LL40A;_LL409: goto _LL40B;
_LL40A: if(*((int*)_tmp7F2)!= 26)goto _LL40C;_LL40B: goto _LL40D;_LL40C: if(*((int*)
_tmp7F2)!= 14)goto _LL40E;_LL40D: goto _LL40F;_LL40E: if(*((int*)_tmp7F2)!= 17)goto
_LL410;_LL40F: goto _LL411;_LL410: if(*((int*)_tmp7F2)!= 21)goto _LL3C3;_LL411:{
const char*_tmpC21;void*_tmpC20;(_tmpC20=0,Cyc_Tcutil_terr(e->loc,((_tmpC21="Cyclone expression within C code",
_tag_dynforward(_tmpC21,sizeof(char),_get_zero_arr_size_char(_tmpC21,33)))),
_tag_dynforward(_tmpC20,sizeof(void*),0)));}goto _LL3C3;_LL3C3:;}static void Cyc_Toc_decltypes_to_c(
struct Cyc_Absyn_Decl*d);static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp825=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp826;struct Cyc_Absyn_Fndecl*
_tmp827;struct Cyc_Absyn_Aggrdecl*_tmp828;struct Cyc_Absyn_Enumdecl*_tmp829;struct
Cyc_Absyn_Typedefdecl*_tmp82A;_LL413: if(_tmp825 <= (void*)2)goto _LL42B;if(*((int*)
_tmp825)!= 0)goto _LL415;_tmp826=((struct Cyc_Absyn_Var_d_struct*)_tmp825)->f1;
_LL414:(void*)(_tmp826->type=(void*)Cyc_Toc_typ_to_c((void*)_tmp826->type));if(
_tmp826->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(
_tmp826->initializer));goto _LL412;_LL415: if(*((int*)_tmp825)!= 1)goto _LL417;
_tmp827=((struct Cyc_Absyn_Fn_d_struct*)_tmp825)->f1;_LL416:(void*)(_tmp827->ret_type=(
void*)Cyc_Toc_typ_to_c((void*)_tmp827->ret_type));{struct Cyc_List_List*_tmp82B=
_tmp827->args;for(0;_tmp82B != 0;_tmp82B=_tmp82B->tl){(*((struct _tuple17*)_tmp82B->hd)).f3=
Cyc_Toc_typ_to_c((*((struct _tuple17*)_tmp82B->hd)).f3);}}goto _LL412;_LL417: if(*((
int*)_tmp825)!= 4)goto _LL419;_tmp828=((struct Cyc_Absyn_Aggr_d_struct*)_tmp825)->f1;
_LL418: Cyc_Toc_aggrdecl_to_c(_tmp828);goto _LL412;_LL419: if(*((int*)_tmp825)!= 6)
goto _LL41B;_tmp829=((struct Cyc_Absyn_Enum_d_struct*)_tmp825)->f1;_LL41A: if(
_tmp829->fields != 0){struct Cyc_List_List*_tmp82C=(struct Cyc_List_List*)((struct
Cyc_Core_Opt*)_check_null(_tmp829->fields))->v;for(0;_tmp82C != 0;_tmp82C=_tmp82C->tl){
struct Cyc_Absyn_Enumfield*_tmp82D=(struct Cyc_Absyn_Enumfield*)_tmp82C->hd;if(
_tmp82D->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp82D->tag));}}
goto _LL412;_LL41B: if(*((int*)_tmp825)!= 7)goto _LL41D;_tmp82A=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp825)->f1;_LL41C:{struct Cyc_Core_Opt*_tmpC22;_tmp82A->defn=((_tmpC22=
_cycalloc(sizeof(*_tmpC22)),((_tmpC22->v=(void*)Cyc_Toc_typ_to_c_array((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp82A->defn))->v),_tmpC22))));}goto _LL412;
_LL41D: if(*((int*)_tmp825)!= 2)goto _LL41F;_LL41E: goto _LL420;_LL41F: if(*((int*)
_tmp825)!= 3)goto _LL421;_LL420: goto _LL422;_LL421: if(*((int*)_tmp825)!= 5)goto
_LL423;_LL422: goto _LL424;_LL423: if(*((int*)_tmp825)!= 8)goto _LL425;_LL424: goto
_LL426;_LL425: if(*((int*)_tmp825)!= 9)goto _LL427;_LL426: goto _LL428;_LL427: if(*((
int*)_tmp825)!= 10)goto _LL429;_LL428: goto _LL42A;_LL429: if(*((int*)_tmp825)!= 11)
goto _LL42B;_LL42A:{const char*_tmpC25;void*_tmpC24;(_tmpC24=0,Cyc_Tcutil_terr(d->loc,((
_tmpC25="Cyclone declaration within C code",_tag_dynforward(_tmpC25,sizeof(char),
_get_zero_arr_size_char(_tmpC25,34)))),_tag_dynforward(_tmpC24,sizeof(void*),0)));}
goto _LL412;_LL42B: if((int)_tmp825 != 0)goto _LL42D;_LL42C: goto _LL42E;_LL42D: if((
int)_tmp825 != 1)goto _LL412;_LL42E: goto _LL412;_LL412:;}static void Cyc_Toc_stmttypes_to_c(
struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp831=(void*)s->r;struct Cyc_Absyn_Exp*_tmp832;struct Cyc_Absyn_Stmt*
_tmp833;struct Cyc_Absyn_Stmt*_tmp834;struct Cyc_Absyn_Exp*_tmp835;struct Cyc_Absyn_Exp*
_tmp836;struct Cyc_Absyn_Stmt*_tmp837;struct Cyc_Absyn_Stmt*_tmp838;struct _tuple3
_tmp839;struct Cyc_Absyn_Exp*_tmp83A;struct Cyc_Absyn_Stmt*_tmp83B;struct Cyc_Absyn_Exp*
_tmp83C;struct _tuple3 _tmp83D;struct Cyc_Absyn_Exp*_tmp83E;struct _tuple3 _tmp83F;
struct Cyc_Absyn_Exp*_tmp840;struct Cyc_Absyn_Stmt*_tmp841;struct Cyc_Absyn_Exp*
_tmp842;struct Cyc_List_List*_tmp843;struct Cyc_Absyn_Decl*_tmp844;struct Cyc_Absyn_Stmt*
_tmp845;struct Cyc_Absyn_Stmt*_tmp846;struct _tuple3 _tmp847;struct Cyc_Absyn_Exp*
_tmp848;_LL430: if(_tmp831 <= (void*)1)goto _LL442;if(*((int*)_tmp831)!= 0)goto
_LL432;_tmp832=((struct Cyc_Absyn_Exp_s_struct*)_tmp831)->f1;_LL431: Cyc_Toc_exptypes_to_c(
_tmp832);goto _LL42F;_LL432: if(*((int*)_tmp831)!= 1)goto _LL434;_tmp833=((struct
Cyc_Absyn_Seq_s_struct*)_tmp831)->f1;_tmp834=((struct Cyc_Absyn_Seq_s_struct*)
_tmp831)->f2;_LL433: Cyc_Toc_stmttypes_to_c(_tmp833);Cyc_Toc_stmttypes_to_c(
_tmp834);goto _LL42F;_LL434: if(*((int*)_tmp831)!= 2)goto _LL436;_tmp835=((struct
Cyc_Absyn_Return_s_struct*)_tmp831)->f1;_LL435: if(_tmp835 != 0)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_tmp835);goto _LL42F;_LL436: if(*((int*)_tmp831)!= 3)goto
_LL438;_tmp836=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp831)->f1;_tmp837=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp831)->f2;_tmp838=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp831)->f3;_LL437: Cyc_Toc_exptypes_to_c(_tmp836);Cyc_Toc_stmttypes_to_c(
_tmp837);Cyc_Toc_stmttypes_to_c(_tmp838);goto _LL42F;_LL438: if(*((int*)_tmp831)!= 
4)goto _LL43A;_tmp839=((struct Cyc_Absyn_While_s_struct*)_tmp831)->f1;_tmp83A=
_tmp839.f1;_tmp83B=((struct Cyc_Absyn_While_s_struct*)_tmp831)->f2;_LL439: Cyc_Toc_exptypes_to_c(
_tmp83A);Cyc_Toc_stmttypes_to_c(_tmp83B);goto _LL42F;_LL43A: if(*((int*)_tmp831)!= 
8)goto _LL43C;_tmp83C=((struct Cyc_Absyn_For_s_struct*)_tmp831)->f1;_tmp83D=((
struct Cyc_Absyn_For_s_struct*)_tmp831)->f2;_tmp83E=_tmp83D.f1;_tmp83F=((struct
Cyc_Absyn_For_s_struct*)_tmp831)->f3;_tmp840=_tmp83F.f1;_tmp841=((struct Cyc_Absyn_For_s_struct*)
_tmp831)->f4;_LL43B: Cyc_Toc_exptypes_to_c(_tmp83C);Cyc_Toc_exptypes_to_c(_tmp83E);
Cyc_Toc_exptypes_to_c(_tmp840);Cyc_Toc_stmttypes_to_c(_tmp841);goto _LL42F;_LL43C:
if(*((int*)_tmp831)!= 9)goto _LL43E;_tmp842=((struct Cyc_Absyn_Switch_s_struct*)
_tmp831)->f1;_tmp843=((struct Cyc_Absyn_Switch_s_struct*)_tmp831)->f2;_LL43D: Cyc_Toc_exptypes_to_c(
_tmp842);for(0;_tmp843 != 0;_tmp843=_tmp843->tl){Cyc_Toc_stmttypes_to_c(((struct
Cyc_Absyn_Switch_clause*)_tmp843->hd)->body);}goto _LL42F;_LL43E: if(*((int*)
_tmp831)!= 11)goto _LL440;_tmp844=((struct Cyc_Absyn_Decl_s_struct*)_tmp831)->f1;
_tmp845=((struct Cyc_Absyn_Decl_s_struct*)_tmp831)->f2;_LL43F: Cyc_Toc_decltypes_to_c(
_tmp844);Cyc_Toc_stmttypes_to_c(_tmp845);goto _LL42F;_LL440: if(*((int*)_tmp831)!= 
13)goto _LL442;_tmp846=((struct Cyc_Absyn_Do_s_struct*)_tmp831)->f1;_tmp847=((
struct Cyc_Absyn_Do_s_struct*)_tmp831)->f2;_tmp848=_tmp847.f1;_LL441: Cyc_Toc_stmttypes_to_c(
_tmp846);Cyc_Toc_exptypes_to_c(_tmp848);goto _LL42F;_LL442: if((int)_tmp831 != 0)
goto _LL444;_LL443: goto _LL445;_LL444: if(_tmp831 <= (void*)1)goto _LL446;if(*((int*)
_tmp831)!= 5)goto _LL446;_LL445: goto _LL447;_LL446: if(_tmp831 <= (void*)1)goto
_LL448;if(*((int*)_tmp831)!= 6)goto _LL448;_LL447: goto _LL449;_LL448: if(_tmp831 <= (
void*)1)goto _LL44A;if(*((int*)_tmp831)!= 7)goto _LL44A;_LL449: goto _LL42F;_LL44A:
if(_tmp831 <= (void*)1)goto _LL44C;if(*((int*)_tmp831)!= 10)goto _LL44C;_LL44B: goto
_LL44D;_LL44C: if(_tmp831 <= (void*)1)goto _LL44E;if(*((int*)_tmp831)!= 12)goto
_LL44E;_LL44D: goto _LL44F;_LL44E: if(_tmp831 <= (void*)1)goto _LL450;if(*((int*)
_tmp831)!= 14)goto _LL450;_LL44F: goto _LL451;_LL450: if(_tmp831 <= (void*)1)goto
_LL452;if(*((int*)_tmp831)!= 15)goto _LL452;_LL451: goto _LL453;_LL452: if(_tmp831 <= (
void*)1)goto _LL454;if(*((int*)_tmp831)!= 17)goto _LL454;_LL453: goto _LL455;_LL454:
if(_tmp831 <= (void*)1)goto _LL42F;if(*((int*)_tmp831)!= 16)goto _LL42F;_LL455:{
const char*_tmpC28;void*_tmpC27;(_tmpC27=0,Cyc_Tcutil_terr(s->loc,((_tmpC28="Cyclone statement in C code",
_tag_dynforward(_tmpC28,sizeof(char),_get_zero_arr_size_char(_tmpC28,28)))),
_tag_dynforward(_tmpC27,sizeof(void*),0)));}goto _LL42F;_LL42F:;}static struct Cyc_Toc_Env*
Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*
ds,int top,int cinclude);static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct
_RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv)){const char*_tmpC2B;void*
_tmpC2A;(_tmpC2A=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Tcutil_impos)(((_tmpC2B="decls_to_c: not at toplevel!",_tag_dynforward(
_tmpC2B,sizeof(char),_get_zero_arr_size_char(_tmpC2B,29)))),_tag_dynforward(
_tmpC2A,sizeof(void*),0)));}{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp84D=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp84E;struct Cyc_Absyn_Fndecl*
_tmp84F;struct Cyc_Absyn_Aggrdecl*_tmp850;struct Cyc_Absyn_Tuniondecl*_tmp851;
struct Cyc_Absyn_Enumdecl*_tmp852;struct Cyc_Absyn_Typedefdecl*_tmp853;struct Cyc_List_List*
_tmp854;struct Cyc_List_List*_tmp855;struct Cyc_List_List*_tmp856;struct Cyc_List_List*
_tmp857;_LL457: if(_tmp84D <= (void*)2)goto _LL467;if(*((int*)_tmp84D)!= 0)goto
_LL459;_tmp84E=((struct Cyc_Absyn_Var_d_struct*)_tmp84D)->f1;_LL458: {struct
_tuple1*_tmp858=_tmp84E->name;if((void*)_tmp84E->sc == (void*)4){union Cyc_Absyn_Nmspace_union
_tmpC2E;struct _tuple1*_tmpC2D;_tmp858=((_tmpC2D=_cycalloc(sizeof(*_tmpC2D)),((
_tmpC2D->f1=(union Cyc_Absyn_Nmspace_union)(((_tmpC2E.Rel_n).tag=1,(((_tmpC2E.Rel_n).f1=
0,_tmpC2E)))),((_tmpC2D->f2=(*_tmp858).f2,_tmpC2D))))));}if(_tmp84E->initializer
!= 0){if(cinclude)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(
_tmp84E->initializer));else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)
_check_null(_tmp84E->initializer));}}{struct Cyc_Absyn_Global_b_struct _tmpC31;
struct Cyc_Absyn_Global_b_struct*_tmpC30;nv=Cyc_Toc_add_varmap(r,nv,_tmp84E->name,
Cyc_Absyn_varb_exp(_tmp858,(void*)((_tmpC30=_cycalloc(sizeof(*_tmpC30)),((
_tmpC30[0]=((_tmpC31.tag=0,((_tmpC31.f1=_tmp84E,_tmpC31)))),_tmpC30)))),0));}
_tmp84E->name=_tmp858;(void*)(_tmp84E->sc=(void*)Cyc_Toc_scope_to_c((void*)
_tmp84E->sc));(void*)(_tmp84E->type=(void*)Cyc_Toc_typ_to_c_array((void*)_tmp84E->type));{
struct Cyc_List_List*_tmpC32;Cyc_Toc_result_decls=((_tmpC32=_cycalloc(sizeof(*
_tmpC32)),((_tmpC32->hd=d,((_tmpC32->tl=Cyc_Toc_result_decls,_tmpC32))))));}goto
_LL456;}_LL459: if(*((int*)_tmp84D)!= 1)goto _LL45B;_tmp84F=((struct Cyc_Absyn_Fn_d_struct*)
_tmp84D)->f1;_LL45A: {struct _tuple1*_tmp85E=_tmp84F->name;if((void*)_tmp84F->sc
== (void*)4){{union Cyc_Absyn_Nmspace_union _tmpC35;struct _tuple1*_tmpC34;_tmp85E=((
_tmpC34=_cycalloc(sizeof(*_tmpC34)),((_tmpC34->f1=(union Cyc_Absyn_Nmspace_union)(((
_tmpC35.Rel_n).tag=1,(((_tmpC35.Rel_n).f1=0,_tmpC35)))),((_tmpC34->f2=(*_tmp85E).f2,
_tmpC34))))));}(void*)(_tmp84F->sc=(void*)((void*)2));}nv=Cyc_Toc_add_varmap(r,
nv,_tmp84F->name,Cyc_Absyn_var_exp(_tmp85E,0));_tmp84F->name=_tmp85E;Cyc_Toc_fndecl_to_c(
nv,_tmp84F,cinclude);{struct Cyc_List_List*_tmpC36;Cyc_Toc_result_decls=((_tmpC36=
_cycalloc(sizeof(*_tmpC36)),((_tmpC36->hd=d,((_tmpC36->tl=Cyc_Toc_result_decls,
_tmpC36))))));}goto _LL456;}_LL45B: if(*((int*)_tmp84D)!= 2)goto _LL45D;_LL45C: goto
_LL45E;_LL45D: if(*((int*)_tmp84D)!= 3)goto _LL45F;_LL45E: {const char*_tmpC39;void*
_tmpC38;(_tmpC38=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Toc_toc_impos)(((_tmpC39="letdecl at toplevel",_tag_dynforward(_tmpC39,
sizeof(char),_get_zero_arr_size_char(_tmpC39,20)))),_tag_dynforward(_tmpC38,
sizeof(void*),0)));}_LL45F: if(*((int*)_tmp84D)!= 4)goto _LL461;_tmp850=((struct
Cyc_Absyn_Aggr_d_struct*)_tmp84D)->f1;_LL460: Cyc_Toc_aggrdecl_to_c(_tmp850);{
struct Cyc_List_List*_tmpC3A;Cyc_Toc_result_decls=((_tmpC3A=_cycalloc(sizeof(*
_tmpC3A)),((_tmpC3A->hd=d,((_tmpC3A->tl=Cyc_Toc_result_decls,_tmpC3A))))));}goto
_LL456;_LL461: if(*((int*)_tmp84D)!= 5)goto _LL463;_tmp851=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp84D)->f1;_LL462: if(_tmp851->is_xtunion)Cyc_Toc_xtuniondecl_to_c(_tmp851);
else{Cyc_Toc_tuniondecl_to_c(_tmp851);}goto _LL456;_LL463: if(*((int*)_tmp84D)!= 6)
goto _LL465;_tmp852=((struct Cyc_Absyn_Enum_d_struct*)_tmp84D)->f1;_LL464: Cyc_Toc_enumdecl_to_c(
nv,_tmp852);{struct Cyc_List_List*_tmpC3B;Cyc_Toc_result_decls=((_tmpC3B=
_cycalloc(sizeof(*_tmpC3B)),((_tmpC3B->hd=d,((_tmpC3B->tl=Cyc_Toc_result_decls,
_tmpC3B))))));}goto _LL456;_LL465: if(*((int*)_tmp84D)!= 7)goto _LL467;_tmp853=((
struct Cyc_Absyn_Typedef_d_struct*)_tmp84D)->f1;_LL466: _tmp853->name=_tmp853->name;
_tmp853->tvs=0;if(_tmp853->defn != 0){struct Cyc_Core_Opt*_tmpC3C;_tmp853->defn=((
_tmpC3C=_cycalloc(sizeof(*_tmpC3C)),((_tmpC3C->v=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp853->defn))->v),_tmpC3C))));}else{
void*_tmp867=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp853->kind))->v;_LL474:
if((int)_tmp867 != 2)goto _LL476;_LL475:{struct Cyc_Core_Opt*_tmpC3D;_tmp853->defn=((
_tmpC3D=_cycalloc(sizeof(*_tmpC3D)),((_tmpC3D->v=(void*)Cyc_Absyn_void_star_typ(),
_tmpC3D))));}goto _LL473;_LL476:;_LL477:{struct Cyc_Core_Opt*_tmpC3E;_tmp853->defn=((
_tmpC3E=_cycalloc(sizeof(*_tmpC3E)),((_tmpC3E->v=(void*)((void*)0),_tmpC3E))));}
goto _LL473;_LL473:;}{struct Cyc_List_List*_tmpC3F;Cyc_Toc_result_decls=((_tmpC3F=
_cycalloc(sizeof(*_tmpC3F)),((_tmpC3F->hd=d,((_tmpC3F->tl=Cyc_Toc_result_decls,
_tmpC3F))))));}goto _LL456;_LL467: if((int)_tmp84D != 0)goto _LL469;_LL468: goto
_LL46A;_LL469: if((int)_tmp84D != 1)goto _LL46B;_LL46A: goto _LL456;_LL46B: if(_tmp84D
<= (void*)2)goto _LL46D;if(*((int*)_tmp84D)!= 8)goto _LL46D;_tmp854=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp84D)->f2;_LL46C: _tmp855=_tmp854;goto _LL46E;_LL46D: if(_tmp84D <= (void*)2)goto
_LL46F;if(*((int*)_tmp84D)!= 9)goto _LL46F;_tmp855=((struct Cyc_Absyn_Using_d_struct*)
_tmp84D)->f2;_LL46E: _tmp856=_tmp855;goto _LL470;_LL46F: if(_tmp84D <= (void*)2)goto
_LL471;if(*((int*)_tmp84D)!= 10)goto _LL471;_tmp856=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp84D)->f1;_LL470: nv=Cyc_Toc_decls_to_c(r,nv,_tmp856,top,cinclude);goto _LL456;
_LL471: if(_tmp84D <= (void*)2)goto _LL456;if(*((int*)_tmp84D)!= 11)goto _LL456;
_tmp857=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp84D)->f1;_LL472: nv=Cyc_Toc_decls_to_c(
r,nv,_tmp857,top,1);goto _LL456;_LL456:;}}return nv;}static void Cyc_Toc_init();
static void Cyc_Toc_init(){struct _DynRegionHandle*_tmp86C;struct Cyc_Core_NewRegion
_tmp86B=Cyc_Core_new_dynregion();_tmp86C=_tmp86B.dynregion;{struct
_DynRegionFrame _tmp86D;struct _RegionHandle*d=_open_dynregion(& _tmp86D,_tmp86C);{
struct Cyc_Dict_Dict*_tmpC4A;struct Cyc_Dict_Dict*_tmpC49;struct Cyc_Set_Set**
_tmpC48;struct Cyc_Dict_Dict*_tmpC47;struct Cyc_List_List**_tmpC46;struct Cyc_Toc_TocState*
_tmpC45;Cyc_Toc_toc_state=((_tmpC45=_cycalloc(sizeof(*_tmpC45)),((_tmpC45->dyn=(
struct _DynRegionHandle*)_tmp86C,((_tmpC45->tuple_types=(struct Cyc_List_List**)((
_tmpC46=_region_malloc(d,sizeof(*_tmpC46)),((_tmpC46[0]=0,_tmpC46)))),((_tmpC45->aggrs_so_far=(
struct Cyc_Dict_Dict*)((_tmpC47=_region_malloc(d,sizeof(*_tmpC47)),((_tmpC47[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct
_tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpC47)))),((_tmpC45->tunions_so_far=(
struct Cyc_Set_Set**)((_tmpC48=_region_malloc(d,sizeof(*_tmpC48)),((_tmpC48[0]=((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct
_tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpC48)))),((_tmpC45->xtunions_so_far=(
struct Cyc_Dict_Dict*)((_tmpC49=_region_malloc(d,sizeof(*_tmpC49)),((_tmpC49[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct
_tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpC49)))),((_tmpC45->qvar_tags=(
struct Cyc_Dict_Dict*)((_tmpC4A=_region_malloc(d,sizeof(*_tmpC4A)),((_tmpC4A[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple6*,struct
_tuple6*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpC4A)))),((_tmpC45->temp_labels=(
struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(
d),_tmpC45))))))))))))))));};_pop_dynregion(d);}Cyc_Toc_result_decls=0;Cyc_Toc_tuple_type_counter=
0;Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=0;Cyc_Toc_total_bounds_checks=
0;Cyc_Toc_bounds_checks_eliminated=0;{struct _dynforward_ptr**_tmpC4B;Cyc_Toc_globals=
_tag_dynforward(((_tmpC4B=_cycalloc(sizeof(struct _dynforward_ptr*)* 50),((
_tmpC4B[0]=& Cyc_Toc__throw_str,((_tmpC4B[1]=& Cyc_Toc_setjmp_str,((_tmpC4B[2]=&
Cyc_Toc__push_handler_str,((_tmpC4B[3]=& Cyc_Toc__pop_handler_str,((_tmpC4B[4]=&
Cyc_Toc__exn_thrown_str,((_tmpC4B[5]=& Cyc_Toc__npop_handler_str,((_tmpC4B[6]=&
Cyc_Toc__check_null_str,((_tmpC4B[7]=& Cyc_Toc__check_known_subscript_null_str,((
_tmpC4B[8]=& Cyc_Toc__check_known_subscript_notnull_str,((_tmpC4B[9]=& Cyc_Toc__check_dynforward_subscript_str,((
_tmpC4B[10]=& Cyc_Toc__check_dyneither_subscript_str,((_tmpC4B[11]=& Cyc_Toc__dynforward_ptr_str,((
_tmpC4B[12]=& Cyc_Toc__dyneither_ptr_str,((_tmpC4B[13]=& Cyc_Toc__tag_dynforward_str,((
_tmpC4B[14]=& Cyc_Toc__tag_dyneither_str,((_tmpC4B[15]=& Cyc_Toc__init_dynforward_ptr_str,((
_tmpC4B[16]=& Cyc_Toc__init_dyneither_ptr_str,((_tmpC4B[17]=& Cyc_Toc__untag_dynforward_ptr_str,((
_tmpC4B[18]=& Cyc_Toc__untag_dyneither_ptr_str,((_tmpC4B[19]=& Cyc_Toc__get_dynforward_size_str,((
_tmpC4B[20]=& Cyc_Toc__get_dyneither_size_str,((_tmpC4B[21]=& Cyc_Toc__get_zero_arr_size_str,((
_tmpC4B[22]=& Cyc_Toc__dynforward_ptr_plus_str,((_tmpC4B[23]=& Cyc_Toc__dyneither_ptr_plus_str,((
_tmpC4B[24]=& Cyc_Toc__zero_arr_plus_str,((_tmpC4B[25]=& Cyc_Toc__dynforward_ptr_inplace_plus_str,((
_tmpC4B[26]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpC4B[27]=& Cyc_Toc__zero_arr_inplace_plus_str,((
_tmpC4B[28]=& Cyc_Toc__dynforward_ptr_inplace_plus_post_str,((_tmpC4B[29]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((
_tmpC4B[30]=& Cyc_Toc__dynforward_to_dyneither_str,((_tmpC4B[31]=& Cyc_Toc__dyneither_to_dynforward_str,((
_tmpC4B[32]=& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpC4B[33]=& Cyc_Toc__cycalloc_str,((
_tmpC4B[34]=& Cyc_Toc__cyccalloc_str,((_tmpC4B[35]=& Cyc_Toc__cycalloc_atomic_str,((
_tmpC4B[36]=& Cyc_Toc__cyccalloc_atomic_str,((_tmpC4B[37]=& Cyc_Toc__region_malloc_str,((
_tmpC4B[38]=& Cyc_Toc__region_calloc_str,((_tmpC4B[39]=& Cyc_Toc__check_times_str,((
_tmpC4B[40]=& Cyc_Toc__new_region_str,((_tmpC4B[41]=& Cyc_Toc__push_region_str,((
_tmpC4B[42]=& Cyc_Toc__pop_region_str,((_tmpC4B[43]=& Cyc_Toc__open_dynregion_str,((
_tmpC4B[44]=& Cyc_Toc__push_dynregion_str,((_tmpC4B[45]=& Cyc_Toc__pop_dynregion_str,((
_tmpC4B[46]=& Cyc_Toc__reset_region_str,((_tmpC4B[47]=& Cyc_Toc__throw_arraybounds_str,((
_tmpC4B[48]=& Cyc_Toc__dynforward_ptr_decrease_size_str,((_tmpC4B[49]=& Cyc_Toc__dyneither_ptr_decrease_size_str,
_tmpC4B)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),
sizeof(struct _dynforward_ptr*),50);}}struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*
ds);struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){Cyc_Toc_init();{
struct _RegionHandle _tmp875=_new_region("start");struct _RegionHandle*start=&
_tmp875;_push_region(start);Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,
1,0);{struct _DynRegionHandle*_tmp877;struct Cyc_Toc_TocState _tmp876=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmp877=_tmp876.dyn;Cyc_Core_free_dynregion(
_tmp877);};_pop_region(start);}return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);}
