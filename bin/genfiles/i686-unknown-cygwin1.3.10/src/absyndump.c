#ifndef _SETJMP_H_
#define _SETJMP_H_
#ifndef _jmp_buf_def_
#define _jmp_buf_def_
typedef int jmp_buf[52];
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
16];struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_fputc(int,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dynforward_ptr f1;}
;struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dynforward_ptr f1;};int Cyc_file_string_write(struct Cyc___cycFILE*,
struct _dynforward_ptr src,int src_offset,int max_count);struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
8];int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x);unsigned int Cyc_strlen(
struct _dynforward_ptr s);struct Cyc_Lineno_Pos{struct _dynforward_ptr logical_file;
struct _dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[9];
struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _dynforward_ptr source;
struct Cyc_Position_Segment*seg;void*kind;struct _dynforward_ptr desc;};extern char
Cyc_Position_Nocontext[14];struct _dynforward_ptr Cyc_Position_get_line_directive(
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*
k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_bounds_one;struct Cyc_Absyn_Exp*
Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct _dynforward_ptr Cyc_Absyn_attribute2string(void*);struct _tuple3{void*f1;
struct _tuple0*f2;};struct _tuple3 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU_union);
struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);int Cyc_Absynpp_is_anon_aggrtype(void*t);extern
struct _dynforward_ptr*Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(struct
Cyc_Absyn_Exp*);struct _dynforward_ptr Cyc_Absynpp_char_escape(char);struct
_dynforward_ptr Cyc_Absynpp_string_escape(struct _dynforward_ptr);struct
_dynforward_ptr Cyc_Absynpp_prim2str(void*p);int Cyc_Absynpp_is_declaration(struct
Cyc_Absyn_Stmt*s);struct _tuple4{struct _dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;
void*f3;};struct _tuple1*Cyc_Absynpp_arg_mk_opt(struct _tuple4*arg);struct _tuple5{
struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};struct _tuple5 Cyc_Absynpp_to_tms(
struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);struct _tuple6{unsigned int
f1;int f2;};struct _tuple6 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;
struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct _tuple7{void*f1;void*f2;};struct _tuple7*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple7*Cyc_Dict_rchoose(struct
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
le;};void*Cyc_Tcutil_impos(struct _dynforward_ptr fmt,struct _dynforward_ptr ap);
void*Cyc_Tcutil_compress(void*t);extern void*Cyc_Cyclone_c_compiler;static int Cyc_Absyndump_expand_typedefs;
static int Cyc_Absyndump_qvar_to_Cids;static int Cyc_Absyndump_add_cyc_prefix;static
int Cyc_Absyndump_generate_line_directives;static int Cyc_Absyndump_to_VC;void Cyc_Absyndump_set_params(
struct Cyc_Absynpp_Params*fs){Cyc_Absyndump_expand_typedefs=fs->expand_typedefs;
Cyc_Absyndump_qvar_to_Cids=fs->qvar_to_Cids;Cyc_Absyndump_add_cyc_prefix=fs->add_cyc_prefix;
Cyc_Absyndump_to_VC=fs->to_VC;Cyc_Absyndump_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_set_params(fs);}void Cyc_Absyndump_dumptyp(void*);void Cyc_Absyndump_dumpntyp(
void*t);void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*);void Cyc_Absyndump_dumpexp_prec(
int,struct Cyc_Absyn_Exp*);void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*);void
Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*);void Cyc_Absyndump_dumpvardecl(
struct Cyc_Absyn_Vardecl*,struct Cyc_Position_Segment*);void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl*);void Cyc_Absyndump_dumptms(struct Cyc_List_List*tms,void(*f)(
void*),void*a);void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual,void*,void(*f)(
void*),void*);void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields);void
Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields);void Cyc_Absyndump_dumploc(
struct Cyc_Position_Segment*);struct Cyc___cycFILE**Cyc_Absyndump_dump_file=& Cyc_stdout;
void Cyc_Absyndump_ignore(void*x){return;}static unsigned int Cyc_Absyndump_pos=0;
static char Cyc_Absyndump_prev_char='x';int Cyc_Absyndump_need_space_before(){
switch(Cyc_Absyndump_prev_char){case '{': _LL0: goto _LL1;case '}': _LL1: goto _LL2;case
'(': _LL2: goto _LL3;case ')': _LL3: goto _LL4;case '[': _LL4: goto _LL5;case ']': _LL5: goto
_LL6;case ';': _LL6: goto _LL7;case ',': _LL7: goto _LL8;case '=': _LL8: goto _LL9;case '?':
_LL9: goto _LLA;case '!': _LLA: goto _LLB;case ' ': _LLB: goto _LLC;case '\n': _LLC: goto _LLD;
case '*': _LLD: return 0;default: _LLE: return 1;}}void Cyc_Absyndump_dump(struct
_dynforward_ptr s){int sz=(int)Cyc_strlen((struct _dynforward_ptr)s);Cyc_Absyndump_pos
+=sz;if(Cyc_Absyndump_pos > 80){Cyc_Absyndump_pos=(unsigned int)sz;Cyc_fputc((int)'\n',*
Cyc_Absyndump_dump_file);}else{if(Cyc_Absyndump_need_space_before())Cyc_fputc((
int)' ',*Cyc_Absyndump_dump_file);}if(sz >= 1){Cyc_Absyndump_prev_char=*((const
char*)_check_dynforward_subscript(s,sizeof(char),sz - 1));Cyc_file_string_write(*
Cyc_Absyndump_dump_file,s,0,sz);}}void Cyc_Absyndump_dump_nospace(struct
_dynforward_ptr s){int sz=(int)Cyc_strlen((struct _dynforward_ptr)s);Cyc_Absyndump_pos
+=sz;if(sz >= 1){Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,sz);Cyc_Absyndump_prev_char=*((
const char*)_check_dynforward_subscript(s,sizeof(char),sz - 1));}}void Cyc_Absyndump_dump_char(
int c){++ Cyc_Absyndump_pos;Cyc_fputc(c,*Cyc_Absyndump_dump_file);Cyc_Absyndump_prev_char=(
char)c;}void Cyc_Absyndump_dumploc(struct Cyc_Position_Segment*loc){if(loc == 0)
return;if(!Cyc_Absyndump_generate_line_directives)return;{struct _dynforward_ptr
_tmp0=Cyc_Position_get_line_directive(loc);Cyc_Absyndump_dump(_tmp0);}}void Cyc_Absyndump_dump_str(
struct _dynforward_ptr*s){Cyc_Absyndump_dump(*s);}void Cyc_Absyndump_dump_semi(){
Cyc_Absyndump_dump_char((int)';');}void Cyc_Absyndump_dump_sep(void(*f)(void*),
struct Cyc_List_List*l,struct _dynforward_ptr sep){if(l == 0)return;for(0;l->tl != 0;
l=l->tl){f((void*)l->hd);Cyc_Absyndump_dump_nospace(sep);}f((void*)l->hd);}void
Cyc_Absyndump_dump_sep_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*l,
struct _dynforward_ptr sep){if(l == 0)return;for(0;l->tl != 0;l=l->tl){f(env,(void*)
l->hd);Cyc_Absyndump_dump_nospace(sep);}f(env,(void*)l->hd);}void Cyc_Absyndump_group(
void(*f)(void*),struct Cyc_List_List*l,struct _dynforward_ptr start,struct
_dynforward_ptr end,struct _dynforward_ptr sep){Cyc_Absyndump_dump_nospace(start);
Cyc_Absyndump_dump_sep(f,l,sep);Cyc_Absyndump_dump_nospace(end);}void Cyc_Absyndump_group_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _dynforward_ptr start,
struct _dynforward_ptr end,struct _dynforward_ptr sep){Cyc_Absyndump_dump_nospace(
start);Cyc_Absyndump_dump_sep_c(f,env,l,sep);Cyc_Absyndump_dump_nospace(end);}
void Cyc_Absyndump_egroup(void(*f)(void*),struct Cyc_List_List*l,struct
_dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep){if(l != 0)
Cyc_Absyndump_group(f,l,start,end,sep);}void Cyc_Absyndump_dumpqvar(struct _tuple0*
v){struct Cyc_List_List*_tmp1=0;struct _dynforward_ptr**prefix=0;{union Cyc_Absyn_Nmspace_union
_tmp2=(*v).f1;struct Cyc_List_List*_tmp3;struct Cyc_List_List*_tmp4;_LL11: if((
_tmp2.Loc_n).tag != 0)goto _LL13;_LL12: _tmp3=0;goto _LL14;_LL13: if((_tmp2.Rel_n).tag
!= 1)goto _LL15;_tmp3=(_tmp2.Rel_n).f1;_LL14: _tmp1=_tmp3;goto _LL10;_LL15: if((
_tmp2.Abs_n).tag != 2)goto _LL10;_tmp4=(_tmp2.Abs_n).f1;_LL16: if(Cyc_Absyndump_qvar_to_Cids
 && Cyc_Absyndump_add_cyc_prefix)prefix=(struct _dynforward_ptr**)& Cyc_Absynpp_cyc_stringptr;
_tmp1=_tmp4;goto _LL10;_LL10:;}if(prefix != 0){Cyc_Absyndump_dump_str(*prefix);if(
Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{Cyc_Absyndump_dump_nospace(({
const char*_tmp5="::";_tag_dynforward(_tmp5,sizeof(char),_get_zero_arr_size_char(
_tmp5,3));}));}}if(_tmp1 != 0){Cyc_Absyndump_dump_nospace(*((struct
_dynforward_ptr*)_tmp1->hd));for(_tmp1=_tmp1->tl;_tmp1 != 0;_tmp1=_tmp1->tl){if(
Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{Cyc_Absyndump_dump_nospace(({
const char*_tmp6="::";_tag_dynforward(_tmp6,sizeof(char),_get_zero_arr_size_char(
_tmp6,3));}));}Cyc_Absyndump_dump_nospace(*((struct _dynforward_ptr*)_tmp1->hd));}
if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_nospace(({const char*_tmp7="_";
_tag_dynforward(_tmp7,sizeof(char),_get_zero_arr_size_char(_tmp7,2));}));else{
Cyc_Absyndump_dump_nospace(({const char*_tmp8="::";_tag_dynforward(_tmp8,sizeof(
char),_get_zero_arr_size_char(_tmp8,3));}));}Cyc_Absyndump_dump_nospace(*(*v).f2);}
else{if(prefix != 0)Cyc_Absyndump_dump_nospace(*(*v).f2);else{Cyc_Absyndump_dump_str((*
v).f2);}}}void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){if(tq.q_restrict)Cyc_Absyndump_dump(({
const char*_tmp9="restrict";_tag_dynforward(_tmp9,sizeof(char),
_get_zero_arr_size_char(_tmp9,9));}));if(tq.q_volatile)Cyc_Absyndump_dump(({
const char*_tmpA="volatile";_tag_dynforward(_tmpA,sizeof(char),
_get_zero_arr_size_char(_tmpA,9));}));if(tq.print_const)Cyc_Absyndump_dump(({
const char*_tmpB="const";_tag_dynforward(_tmpB,sizeof(char),
_get_zero_arr_size_char(_tmpB,6));}));}void Cyc_Absyndump_dumpscope(void*sc){void*
_tmpC=sc;_LL18: if((int)_tmpC != 0)goto _LL1A;_LL19: Cyc_Absyndump_dump(({const char*
_tmpD="static";_tag_dynforward(_tmpD,sizeof(char),_get_zero_arr_size_char(_tmpD,
7));}));return;_LL1A: if((int)_tmpC != 2)goto _LL1C;_LL1B: return;_LL1C: if((int)
_tmpC != 3)goto _LL1E;_LL1D: Cyc_Absyndump_dump(({const char*_tmpE="extern";
_tag_dynforward(_tmpE,sizeof(char),_get_zero_arr_size_char(_tmpE,7));}));return;
_LL1E: if((int)_tmpC != 4)goto _LL20;_LL1F: Cyc_Absyndump_dump(({const char*_tmpF="extern \"C\"";
_tag_dynforward(_tmpF,sizeof(char),_get_zero_arr_size_char(_tmpF,11));}));
return;_LL20: if((int)_tmpC != 1)goto _LL22;_LL21: Cyc_Absyndump_dump(({const char*
_tmp10="abstract";_tag_dynforward(_tmp10,sizeof(char),_get_zero_arr_size_char(
_tmp10,9));}));return;_LL22: if((int)_tmpC != 5)goto _LL17;_LL23: Cyc_Absyndump_dump(({
const char*_tmp11="register";_tag_dynforward(_tmp11,sizeof(char),
_get_zero_arr_size_char(_tmp11,9));}));return;_LL17:;}void Cyc_Absyndump_dumpkind(
void*k){void*_tmp12=k;_LL25: if((int)_tmp12 != 0)goto _LL27;_LL26: Cyc_Absyndump_dump(({
const char*_tmp13="A";_tag_dynforward(_tmp13,sizeof(char),_get_zero_arr_size_char(
_tmp13,2));}));return;_LL27: if((int)_tmp12 != 1)goto _LL29;_LL28: Cyc_Absyndump_dump(({
const char*_tmp14="M";_tag_dynforward(_tmp14,sizeof(char),_get_zero_arr_size_char(
_tmp14,2));}));return;_LL29: if((int)_tmp12 != 2)goto _LL2B;_LL2A: Cyc_Absyndump_dump(({
const char*_tmp15="B";_tag_dynforward(_tmp15,sizeof(char),_get_zero_arr_size_char(
_tmp15,2));}));return;_LL2B: if((int)_tmp12 != 3)goto _LL2D;_LL2C: Cyc_Absyndump_dump(({
const char*_tmp16="R";_tag_dynforward(_tmp16,sizeof(char),_get_zero_arr_size_char(
_tmp16,2));}));return;_LL2D: if((int)_tmp12 != 4)goto _LL2F;_LL2E: Cyc_Absyndump_dump(({
const char*_tmp17="UR";_tag_dynforward(_tmp17,sizeof(char),
_get_zero_arr_size_char(_tmp17,3));}));return;_LL2F: if((int)_tmp12 != 5)goto _LL31;
_LL30: Cyc_Absyndump_dump(({const char*_tmp18="TR";_tag_dynforward(_tmp18,sizeof(
char),_get_zero_arr_size_char(_tmp18,3));}));return;_LL31: if((int)_tmp12 != 6)
goto _LL33;_LL32: Cyc_Absyndump_dump(({const char*_tmp19="E";_tag_dynforward(_tmp19,
sizeof(char),_get_zero_arr_size_char(_tmp19,2));}));return;_LL33: if((int)_tmp12
!= 7)goto _LL24;_LL34: Cyc_Absyndump_dump(({const char*_tmp1A="I";_tag_dynforward(
_tmp1A,sizeof(char),_get_zero_arr_size_char(_tmp1A,2));}));return;_LL24:;}void
Cyc_Absyndump_dumpaggr_kind(void*k){void*_tmp1B=k;_LL36: if((int)_tmp1B != 0)goto
_LL38;_LL37: Cyc_Absyndump_dump(({const char*_tmp1C="struct";_tag_dynforward(
_tmp1C,sizeof(char),_get_zero_arr_size_char(_tmp1C,7));}));return;_LL38: if((int)
_tmp1B != 1)goto _LL35;_LL39: Cyc_Absyndump_dump(({const char*_tmp1D="union";
_tag_dynforward(_tmp1D,sizeof(char),_get_zero_arr_size_char(_tmp1D,6));}));
return;_LL35:;}void Cyc_Absyndump_dumptps(struct Cyc_List_List*ts){Cyc_Absyndump_egroup(
Cyc_Absyndump_dumptyp,ts,({const char*_tmp1E="<";_tag_dynforward(_tmp1E,sizeof(
char),_get_zero_arr_size_char(_tmp1E,2));}),({const char*_tmp1F=">";
_tag_dynforward(_tmp1F,sizeof(char),_get_zero_arr_size_char(_tmp1F,2));}),({
const char*_tmp20=",";_tag_dynforward(_tmp20,sizeof(char),_get_zero_arr_size_char(
_tmp20,2));}));}void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(
tv->name);}void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv){Cyc_Absyndump_dump_str(
tv->name);{void*_tmp21=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmp22;void*
_tmp23;_LL3B: if(*((int*)_tmp21)!= 0)goto _LL3D;_tmp22=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmp21)->f1;if((int)_tmp22 != 2)goto _LL3D;_LL3C: goto _LL3E;_LL3D: if(*((int*)_tmp21)
!= 2)goto _LL3F;_LL3E: goto _LL40;_LL3F: if(*((int*)_tmp21)!= 1)goto _LL41;_LL40: Cyc_Absyndump_dump(({
const char*_tmp24="::?";_tag_dynforward(_tmp24,sizeof(char),
_get_zero_arr_size_char(_tmp24,4));}));goto _LL3A;_LL41: if(*((int*)_tmp21)!= 0)
goto _LL3A;_tmp23=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp21)->f1;_LL42: Cyc_Absyndump_dump(({
const char*_tmp25="::";_tag_dynforward(_tmp25,sizeof(char),
_get_zero_arr_size_char(_tmp25,3));}));Cyc_Absyndump_dumpkind(_tmp23);goto _LL3A;
_LL3A:;}}void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){((void(*)(void(*f)(
struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dynforward_ptr start,struct
_dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,
tvs,({const char*_tmp26="<";_tag_dynforward(_tmp26,sizeof(char),
_get_zero_arr_size_char(_tmp26,2));}),({const char*_tmp27=">";_tag_dynforward(
_tmp27,sizeof(char),_get_zero_arr_size_char(_tmp27,2));}),({const char*_tmp28=",";
_tag_dynforward(_tmp28,sizeof(char),_get_zero_arr_size_char(_tmp28,2));}));}void
Cyc_Absyndump_dumpkindedtvars(struct Cyc_List_List*tvs){((void(*)(void(*f)(struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dynforward_ptr start,struct
_dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,
tvs,({const char*_tmp29="<";_tag_dynforward(_tmp29,sizeof(char),
_get_zero_arr_size_char(_tmp29,2));}),({const char*_tmp2A=">";_tag_dynforward(
_tmp2A,sizeof(char),_get_zero_arr_size_char(_tmp2A,2));}),({const char*_tmp2B=",";
_tag_dynforward(_tmp2B,sizeof(char),_get_zero_arr_size_char(_tmp2B,2));}));}
struct _tuple8{struct Cyc_Absyn_Tqual f1;void*f2;};void Cyc_Absyndump_dumparg(struct
_tuple8*pr){((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*
pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpargs(
struct Cyc_List_List*ts){((void(*)(void(*f)(struct _tuple8*),struct Cyc_List_List*l,
struct _dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))
Cyc_Absyndump_group)(Cyc_Absyndump_dumparg,ts,({const char*_tmp2C="(";
_tag_dynforward(_tmp2C,sizeof(char),_get_zero_arr_size_char(_tmp2C,2));}),({
const char*_tmp2D=")";_tag_dynforward(_tmp2D,sizeof(char),_get_zero_arr_size_char(
_tmp2D,2));}),({const char*_tmp2E=",";_tag_dynforward(_tmp2E,sizeof(char),
_get_zero_arr_size_char(_tmp2E,2));}));}void Cyc_Absyndump_dump_callconv(struct
Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){void*_tmp2F=(void*)atts->hd;
_LL44: if((int)_tmp2F != 0)goto _LL46;_LL45: Cyc_Absyndump_dump(({const char*_tmp30="_stdcall";
_tag_dynforward(_tmp30,sizeof(char),_get_zero_arr_size_char(_tmp30,9));}));
return;_LL46: if((int)_tmp2F != 1)goto _LL48;_LL47: Cyc_Absyndump_dump(({const char*
_tmp31="_cdecl";_tag_dynforward(_tmp31,sizeof(char),_get_zero_arr_size_char(
_tmp31,7));}));return;_LL48: if((int)_tmp2F != 2)goto _LL4A;_LL49: Cyc_Absyndump_dump(({
const char*_tmp32="_fastcall";_tag_dynforward(_tmp32,sizeof(char),
_get_zero_arr_size_char(_tmp32,10));}));return;_LL4A:;_LL4B: goto _LL43;_LL43:;}}
void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*atts){int hasatt=0;{struct
Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){void*_tmp33=(void*)((
struct Cyc_List_List*)_check_null(atts))->hd;_LL4D: if((int)_tmp33 != 0)goto _LL4F;
_LL4E: goto _LL50;_LL4F: if((int)_tmp33 != 1)goto _LL51;_LL50: goto _LL52;_LL51: if((int)
_tmp33 != 2)goto _LL53;_LL52: goto _LL4C;_LL53:;_LL54: hasatt=1;goto _LL4C;_LL4C:;}}
if(!hasatt)return;Cyc_Absyndump_dump(({const char*_tmp34="__declspec(";
_tag_dynforward(_tmp34,sizeof(char),_get_zero_arr_size_char(_tmp34,12));}));for(
0;atts != 0;atts=atts->tl){void*_tmp35=(void*)atts->hd;_LL56: if((int)_tmp35 != 0)
goto _LL58;_LL57: goto _LL59;_LL58: if((int)_tmp35 != 1)goto _LL5A;_LL59: goto _LL5B;
_LL5A: if((int)_tmp35 != 2)goto _LL5C;_LL5B: goto _LL55;_LL5C:;_LL5D: Cyc_Absyndump_dump(
Cyc_Absyn_attribute2string((void*)atts->hd));goto _LL55;_LL55:;}Cyc_Absyndump_dump_char((
int)')');}void Cyc_Absyndump_dumpatts(struct Cyc_List_List*atts){if(atts == 0)
return;{void*_tmp36=Cyc_Cyclone_c_compiler;_LL5F: if((int)_tmp36 != 0)goto _LL61;
_LL60: Cyc_Absyndump_dump(({const char*_tmp37=" __attribute__((";_tag_dynforward(
_tmp37,sizeof(char),_get_zero_arr_size_char(_tmp37,17));}));for(0;atts != 0;atts=
atts->tl){Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));if(
atts->tl != 0)Cyc_Absyndump_dump(({const char*_tmp38=",";_tag_dynforward(_tmp38,
sizeof(char),_get_zero_arr_size_char(_tmp38,2));}));}Cyc_Absyndump_dump(({const
char*_tmp39=")) ";_tag_dynforward(_tmp39,sizeof(char),_get_zero_arr_size_char(
_tmp39,4));}));return;_LL61: if((int)_tmp36 != 1)goto _LL5E;_LL62: Cyc_Absyndump_dump_noncallconv(
atts);return;_LL5E:;}}int Cyc_Absyndump_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{void*_tmp3A=(void*)tms->hd;_LL64: if(*((int*)_tmp3A)!= 2)goto
_LL66;_LL65: return 1;_LL66:;_LL67: return 0;_LL63:;}}static void Cyc_Absyndump_dumprgn(
void*t){void*_tmp3B=Cyc_Tcutil_compress(t);_LL69: if((int)_tmp3B != 2)goto _LL6B;
_LL6A: Cyc_Absyndump_dump(({const char*_tmp3C="`H";_tag_dynforward(_tmp3C,sizeof(
char),_get_zero_arr_size_char(_tmp3C,3));}));goto _LL68;_LL6B:;_LL6C: Cyc_Absyndump_dumpntyp(
t);goto _LL68;_LL68:;}struct _tuple9{struct Cyc_List_List*f1;struct Cyc_List_List*f2;
};static struct _tuple9 Cyc_Absyndump_effects_split(void*t){struct Cyc_List_List*
rgions=0;struct Cyc_List_List*effects=0;{void*_tmp3D=Cyc_Tcutil_compress(t);void*
_tmp3E;struct Cyc_List_List*_tmp3F;_LL6E: if(_tmp3D <= (void*)4)goto _LL72;if(*((int*)
_tmp3D)!= 20)goto _LL70;_tmp3E=(void*)((struct Cyc_Absyn_AccessEff_struct*)_tmp3D)->f1;
_LL6F: rgions=({struct Cyc_List_List*_tmp40=_cycalloc(sizeof(*_tmp40));_tmp40->hd=(
void*)_tmp3E;_tmp40->tl=rgions;_tmp40;});goto _LL6D;_LL70: if(*((int*)_tmp3D)!= 21)
goto _LL72;_tmp3F=((struct Cyc_Absyn_JoinEff_struct*)_tmp3D)->f1;_LL71: for(0;
_tmp3F != 0;_tmp3F=_tmp3F->tl){struct Cyc_List_List*_tmp42;struct Cyc_List_List*
_tmp43;struct _tuple9 _tmp41=Cyc_Absyndump_effects_split((void*)_tmp3F->hd);_tmp42=
_tmp41.f1;_tmp43=_tmp41.f2;rgions=Cyc_List_imp_append(_tmp42,rgions);effects=Cyc_List_imp_append(
_tmp43,effects);}goto _LL6D;_LL72:;_LL73: effects=({struct Cyc_List_List*_tmp44=
_cycalloc(sizeof(*_tmp44));_tmp44->hd=(void*)t;_tmp44->tl=effects;_tmp44;});goto
_LL6D;_LL6D:;}return({struct _tuple9 _tmp45;_tmp45.f1=rgions;_tmp45.f2=effects;
_tmp45;});}static void Cyc_Absyndump_dumpeff(void*t){struct Cyc_List_List*_tmp47;
struct Cyc_List_List*_tmp48;struct _tuple9 _tmp46=Cyc_Absyndump_effects_split(t);
_tmp47=_tmp46.f1;_tmp48=_tmp46.f2;_tmp47=Cyc_List_imp_rev(_tmp47);_tmp48=Cyc_List_imp_rev(
_tmp48);for(0;_tmp48 != 0;_tmp48=_tmp48->tl){Cyc_Absyndump_dumpntyp((void*)_tmp48->hd);
Cyc_Absyndump_dump_char((int)'+');}Cyc_Absyndump_dump_char((int)'{');for(0;
_tmp47 != 0;_tmp47=_tmp47->tl){Cyc_Absyndump_dumprgn((void*)_tmp47->hd);if(_tmp47->tl
!= 0)Cyc_Absyndump_dump_char((int)',');}Cyc_Absyndump_dump_char((int)'}');}void
Cyc_Absyndump_dumpntyp(void*t){void*_tmp49=t;struct Cyc_Absyn_Tvar*_tmp4A;struct
Cyc_Core_Opt*_tmp4B;struct Cyc_Core_Opt*_tmp4C;int _tmp4D;struct Cyc_Core_Opt*
_tmp4E;struct Cyc_Core_Opt*_tmp4F;struct Cyc_Core_Opt _tmp50;void*_tmp51;int _tmp52;
struct Cyc_Absyn_TunionInfo _tmp53;union Cyc_Absyn_TunionInfoU_union _tmp54;struct
Cyc_List_List*_tmp55;struct Cyc_Core_Opt*_tmp56;struct Cyc_Absyn_TunionFieldInfo
_tmp57;union Cyc_Absyn_TunionFieldInfoU_union _tmp58;struct Cyc_List_List*_tmp59;
void*_tmp5A;void*_tmp5B;void*_tmp5C;void*_tmp5D;void*_tmp5E;void*_tmp5F;void*
_tmp60;void*_tmp61;void*_tmp62;void*_tmp63;void*_tmp64;void*_tmp65;void*_tmp66;
void*_tmp67;void*_tmp68;void*_tmp69;void*_tmp6A;void*_tmp6B;void*_tmp6C;void*
_tmp6D;void*_tmp6E;void*_tmp6F;void*_tmp70;void*_tmp71;void*_tmp72;void*_tmp73;
void*_tmp74;void*_tmp75;void*_tmp76;void*_tmp77;int _tmp78;struct Cyc_List_List*
_tmp79;struct Cyc_Absyn_AggrInfo _tmp7A;union Cyc_Absyn_AggrInfoU_union _tmp7B;
struct Cyc_List_List*_tmp7C;void*_tmp7D;struct Cyc_List_List*_tmp7E;struct _tuple0*
_tmp7F;struct Cyc_List_List*_tmp80;struct _tuple0*_tmp81;struct Cyc_List_List*
_tmp82;void*_tmp83;void*_tmp84;void*_tmp85;void*_tmp86;int _tmp87;void*_tmp88;
_LL75: if(_tmp49 <= (void*)4)goto _LL7B;if(*((int*)_tmp49)!= 7)goto _LL77;_LL76: goto
_LL78;_LL77: if(*((int*)_tmp49)!= 8)goto _LL79;_LL78: goto _LL7A;_LL79: if(*((int*)
_tmp49)!= 4)goto _LL7B;_LL7A: return;_LL7B: if((int)_tmp49 != 0)goto _LL7D;_LL7C: Cyc_Absyndump_dump(({
const char*_tmp89="void";_tag_dynforward(_tmp89,sizeof(char),
_get_zero_arr_size_char(_tmp89,5));}));return;_LL7D: if(_tmp49 <= (void*)4)goto
_LLA5;if(*((int*)_tmp49)!= 1)goto _LL7F;_tmp4A=((struct Cyc_Absyn_VarType_struct*)
_tmp49)->f1;_LL7E: Cyc_Absyndump_dump_str(_tmp4A->name);return;_LL7F: if(*((int*)
_tmp49)!= 0)goto _LL81;_tmp4B=((struct Cyc_Absyn_Evar_struct*)_tmp49)->f1;_tmp4C=((
struct Cyc_Absyn_Evar_struct*)_tmp49)->f2;if(_tmp4C != 0)goto _LL81;_tmp4D=((struct
Cyc_Absyn_Evar_struct*)_tmp49)->f3;_LL80: Cyc_Absyndump_dump(({const char*_tmp8A="%";
_tag_dynforward(_tmp8A,sizeof(char),_get_zero_arr_size_char(_tmp8A,2));}));if(
_tmp4B == 0)Cyc_Absyndump_dump(({const char*_tmp8B="?";_tag_dynforward(_tmp8B,
sizeof(char),_get_zero_arr_size_char(_tmp8B,2));}));else{Cyc_Absyndump_dumpkind((
void*)_tmp4B->v);}Cyc_Absyndump_dump((struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmp8E;_tmp8E.tag=1;_tmp8E.f1=(unsigned long)_tmp4D;{void*_tmp8C[1]={& _tmp8E};
Cyc_aprintf(({const char*_tmp8D="(%d)";_tag_dynforward(_tmp8D,sizeof(char),
_get_zero_arr_size_char(_tmp8D,5));}),_tag_dynforward(_tmp8C,sizeof(void*),1));}}));
return;_LL81: if(*((int*)_tmp49)!= 0)goto _LL83;_tmp4E=((struct Cyc_Absyn_Evar_struct*)
_tmp49)->f1;_tmp4F=((struct Cyc_Absyn_Evar_struct*)_tmp49)->f2;if(_tmp4F == 0)goto
_LL83;_tmp50=*_tmp4F;_tmp51=(void*)_tmp50.v;_tmp52=((struct Cyc_Absyn_Evar_struct*)
_tmp49)->f3;_LL82: Cyc_Absyndump_dumpntyp(_tmp51);return;_LL83: if(*((int*)_tmp49)
!= 2)goto _LL85;_tmp53=((struct Cyc_Absyn_TunionType_struct*)_tmp49)->f1;_tmp54=
_tmp53.tunion_info;_tmp55=_tmp53.targs;_tmp56=_tmp53.rgn;_LL84:{union Cyc_Absyn_TunionInfoU_union
_tmp8F=_tmp54;struct Cyc_Absyn_UnknownTunionInfo _tmp90;struct _tuple0*_tmp91;int
_tmp92;struct Cyc_Absyn_Tuniondecl**_tmp93;struct Cyc_Absyn_Tuniondecl*_tmp94;
struct Cyc_Absyn_Tuniondecl _tmp95;struct _tuple0*_tmp96;int _tmp97;_LLCA: if((_tmp8F.UnknownTunion).tag
!= 0)goto _LLCC;_tmp90=(_tmp8F.UnknownTunion).f1;_tmp91=_tmp90.name;_tmp92=_tmp90.is_xtunion;
_LLCB: _tmp96=_tmp91;_tmp97=_tmp92;goto _LLCD;_LLCC: if((_tmp8F.KnownTunion).tag != 
1)goto _LLC9;_tmp93=(_tmp8F.KnownTunion).f1;_tmp94=*_tmp93;_tmp95=*_tmp94;_tmp96=
_tmp95.name;_tmp97=_tmp95.is_xtunion;_LLCD: if(_tmp97)Cyc_Absyndump_dump(({const
char*_tmp98="xtunion ";_tag_dynforward(_tmp98,sizeof(char),
_get_zero_arr_size_char(_tmp98,9));}));else{Cyc_Absyndump_dump(({const char*
_tmp99="tunion ";_tag_dynforward(_tmp99,sizeof(char),_get_zero_arr_size_char(
_tmp99,8));}));}{void*r=(unsigned int)_tmp56?(void*)_tmp56->v:(void*)2;{void*
_tmp9A=Cyc_Tcutil_compress(r);_LLCF: if((int)_tmp9A != 2)goto _LLD1;_LLD0: goto _LLCE;
_LLD1:;_LLD2: Cyc_Absyndump_dumptyp(r);Cyc_Absyndump_dump(({const char*_tmp9B=" ";
_tag_dynforward(_tmp9B,sizeof(char),_get_zero_arr_size_char(_tmp9B,2));}));goto
_LLCE;_LLCE:;}Cyc_Absyndump_dumpqvar(_tmp96);Cyc_Absyndump_dumptps(_tmp55);goto
_LLC9;}_LLC9:;}goto _LL74;_LL85: if(*((int*)_tmp49)!= 3)goto _LL87;_tmp57=((struct
Cyc_Absyn_TunionFieldType_struct*)_tmp49)->f1;_tmp58=_tmp57.field_info;_tmp59=
_tmp57.targs;_LL86:{union Cyc_Absyn_TunionFieldInfoU_union _tmp9C=_tmp58;struct Cyc_Absyn_UnknownTunionFieldInfo
_tmp9D;struct _tuple0*_tmp9E;struct _tuple0*_tmp9F;int _tmpA0;struct Cyc_Absyn_Tuniondecl*
_tmpA1;struct Cyc_Absyn_Tuniondecl _tmpA2;struct _tuple0*_tmpA3;int _tmpA4;struct Cyc_Absyn_Tunionfield*
_tmpA5;struct Cyc_Absyn_Tunionfield _tmpA6;struct _tuple0*_tmpA7;_LLD4: if((_tmp9C.UnknownTunionfield).tag
!= 0)goto _LLD6;_tmp9D=(_tmp9C.UnknownTunionfield).f1;_tmp9E=_tmp9D.tunion_name;
_tmp9F=_tmp9D.field_name;_tmpA0=_tmp9D.is_xtunion;_LLD5: _tmpA3=_tmp9E;_tmpA4=
_tmpA0;_tmpA7=_tmp9F;goto _LLD7;_LLD6: if((_tmp9C.KnownTunionfield).tag != 1)goto
_LLD3;_tmpA1=(_tmp9C.KnownTunionfield).f1;_tmpA2=*_tmpA1;_tmpA3=_tmpA2.name;
_tmpA4=_tmpA2.is_xtunion;_tmpA5=(_tmp9C.KnownTunionfield).f2;_tmpA6=*_tmpA5;
_tmpA7=_tmpA6.name;_LLD7: if(_tmpA4)Cyc_Absyndump_dump(({const char*_tmpA8="xtunion ";
_tag_dynforward(_tmpA8,sizeof(char),_get_zero_arr_size_char(_tmpA8,9));}));else{
Cyc_Absyndump_dump(({const char*_tmpA9="tunion ";_tag_dynforward(_tmpA9,sizeof(
char),_get_zero_arr_size_char(_tmpA9,8));}));}Cyc_Absyndump_dumpqvar(_tmpA3);Cyc_Absyndump_dump(({
const char*_tmpAA=".";_tag_dynforward(_tmpAA,sizeof(char),_get_zero_arr_size_char(
_tmpAA,2));}));Cyc_Absyndump_dumpqvar(_tmpA7);Cyc_Absyndump_dumptps(_tmp59);goto
_LLD3;_LLD3:;}goto _LL74;_LL87: if(*((int*)_tmp49)!= 5)goto _LL89;_tmp5A=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp49)->f1;if((int)_tmp5A != 2)goto _LL89;_tmp5B=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp49)->f2;if((int)_tmp5B != 2)goto _LL89;
_LL88: goto _LL8A;_LL89: if(*((int*)_tmp49)!= 5)goto _LL8B;_tmp5C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp49)->f1;if((int)_tmp5C != 0)goto _LL8B;_tmp5D=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp49)->f2;if((int)_tmp5D != 2)goto _LL8B;_LL8A: Cyc_Absyndump_dump(({const char*
_tmpAB="int";_tag_dynforward(_tmpAB,sizeof(char),_get_zero_arr_size_char(_tmpAB,
4));}));return;_LL8B: if(*((int*)_tmp49)!= 5)goto _LL8D;_tmp5E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp49)->f1;if((int)_tmp5E != 2)goto _LL8D;_tmp5F=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp49)->f2;if((int)_tmp5F != 3)goto _LL8D;_LL8C: goto _LL8E;_LL8D: if(*((int*)_tmp49)
!= 5)goto _LL8F;_tmp60=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp49)->f1;if((
int)_tmp60 != 0)goto _LL8F;_tmp61=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp49)->f2;
if((int)_tmp61 != 3)goto _LL8F;_LL8E: Cyc_Absyndump_dump(({const char*_tmpAC="long";
_tag_dynforward(_tmpAC,sizeof(char),_get_zero_arr_size_char(_tmpAC,5));}));
return;_LL8F: if(*((int*)_tmp49)!= 5)goto _LL91;_tmp62=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp49)->f1;if((int)_tmp62 != 2)goto _LL91;_tmp63=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp49)->f2;if((int)_tmp63 != 0)goto _LL91;_LL90: Cyc_Absyndump_dump(({const char*
_tmpAD="char";_tag_dynforward(_tmpAD,sizeof(char),_get_zero_arr_size_char(_tmpAD,
5));}));return;_LL91: if(*((int*)_tmp49)!= 5)goto _LL93;_tmp64=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp49)->f1;if((int)_tmp64 != 0)goto _LL93;_tmp65=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp49)->f2;if((int)_tmp65 != 0)goto _LL93;_LL92: Cyc_Absyndump_dump(({const char*
_tmpAE="signed char";_tag_dynforward(_tmpAE,sizeof(char),_get_zero_arr_size_char(
_tmpAE,12));}));return;_LL93: if(*((int*)_tmp49)!= 5)goto _LL95;_tmp66=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp49)->f1;if((int)_tmp66 != 1)goto _LL95;_tmp67=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp49)->f2;if((int)_tmp67 != 0)goto _LL95;
_LL94: Cyc_Absyndump_dump(({const char*_tmpAF="unsigned char";_tag_dynforward(
_tmpAF,sizeof(char),_get_zero_arr_size_char(_tmpAF,14));}));return;_LL95: if(*((
int*)_tmp49)!= 5)goto _LL97;_tmp68=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp49)->f1;if((int)_tmp68 != 2)goto _LL97;_tmp69=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp49)->f2;if((int)_tmp69 != 1)goto _LL97;_LL96: goto _LL98;_LL97: if(*((int*)_tmp49)
!= 5)goto _LL99;_tmp6A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp49)->f1;if((
int)_tmp6A != 0)goto _LL99;_tmp6B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp49)->f2;
if((int)_tmp6B != 1)goto _LL99;_LL98: Cyc_Absyndump_dump(({const char*_tmpB0="short";
_tag_dynforward(_tmpB0,sizeof(char),_get_zero_arr_size_char(_tmpB0,6));}));
return;_LL99: if(*((int*)_tmp49)!= 5)goto _LL9B;_tmp6C=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp49)->f1;if((int)_tmp6C != 1)goto _LL9B;_tmp6D=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp49)->f2;if((int)_tmp6D != 1)goto _LL9B;_LL9A: Cyc_Absyndump_dump(({const char*
_tmpB1="unsigned short";_tag_dynforward(_tmpB1,sizeof(char),
_get_zero_arr_size_char(_tmpB1,15));}));return;_LL9B: if(*((int*)_tmp49)!= 5)goto
_LL9D;_tmp6E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp49)->f1;if((int)_tmp6E
!= 1)goto _LL9D;_tmp6F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp49)->f2;if((
int)_tmp6F != 2)goto _LL9D;_LL9C: Cyc_Absyndump_dump(({const char*_tmpB2="unsigned int";
_tag_dynforward(_tmpB2,sizeof(char),_get_zero_arr_size_char(_tmpB2,13));}));
return;_LL9D: if(*((int*)_tmp49)!= 5)goto _LL9F;_tmp70=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp49)->f1;if((int)_tmp70 != 1)goto _LL9F;_tmp71=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp49)->f2;if((int)_tmp71 != 3)goto _LL9F;_LL9E: Cyc_Absyndump_dump(({const char*
_tmpB3="unsigned long";_tag_dynforward(_tmpB3,sizeof(char),
_get_zero_arr_size_char(_tmpB3,14));}));return;_LL9F: if(*((int*)_tmp49)!= 5)goto
_LLA1;_tmp72=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp49)->f1;if((int)_tmp72
!= 2)goto _LLA1;_tmp73=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp49)->f2;if((
int)_tmp73 != 4)goto _LLA1;_LLA0: goto _LLA2;_LLA1: if(*((int*)_tmp49)!= 5)goto _LLA3;
_tmp74=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp49)->f1;if((int)_tmp74 != 0)
goto _LLA3;_tmp75=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp49)->f2;if((int)
_tmp75 != 4)goto _LLA3;_LLA2: {void*_tmpB4=Cyc_Cyclone_c_compiler;_LLD9: if((int)
_tmpB4 != 0)goto _LLDB;_LLDA: Cyc_Absyndump_dump(({const char*_tmpB5="long long";
_tag_dynforward(_tmpB5,sizeof(char),_get_zero_arr_size_char(_tmpB5,10));}));
return;_LLDB: if((int)_tmpB4 != 1)goto _LLD8;_LLDC: Cyc_Absyndump_dump(({const char*
_tmpB6="__int64";_tag_dynforward(_tmpB6,sizeof(char),_get_zero_arr_size_char(
_tmpB6,8));}));return;_LLD8:;}_LLA3: if(*((int*)_tmp49)!= 5)goto _LLA5;_tmp76=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp49)->f1;if((int)_tmp76 != 1)goto _LLA5;
_tmp77=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp49)->f2;if((int)_tmp77 != 4)
goto _LLA5;_LLA4: {void*_tmpB7=Cyc_Cyclone_c_compiler;_LLDE: if((int)_tmpB7 != 1)
goto _LLE0;_LLDF: Cyc_Absyndump_dump(({const char*_tmpB8="unsigned __int64";
_tag_dynforward(_tmpB8,sizeof(char),_get_zero_arr_size_char(_tmpB8,17));}));
return;_LLE0: if((int)_tmpB7 != 0)goto _LLDD;_LLE1: Cyc_Absyndump_dump(({const char*
_tmpB9="unsigned long long";_tag_dynforward(_tmpB9,sizeof(char),
_get_zero_arr_size_char(_tmpB9,19));}));return;_LLDD:;}_LLA5: if((int)_tmp49 != 1)
goto _LLA7;_LLA6: Cyc_Absyndump_dump(({const char*_tmpBA="float";_tag_dynforward(
_tmpBA,sizeof(char),_get_zero_arr_size_char(_tmpBA,6));}));return;_LLA7: if(
_tmp49 <= (void*)4)goto _LLBF;if(*((int*)_tmp49)!= 6)goto _LLA9;_tmp78=((struct Cyc_Absyn_DoubleType_struct*)
_tmp49)->f1;_LLA8: if(_tmp78)Cyc_Absyndump_dump(({const char*_tmpBB="long double";
_tag_dynforward(_tmpBB,sizeof(char),_get_zero_arr_size_char(_tmpBB,12));}));
else{Cyc_Absyndump_dump(({const char*_tmpBC="double";_tag_dynforward(_tmpBC,
sizeof(char),_get_zero_arr_size_char(_tmpBC,7));}));}return;_LLA9: if(*((int*)
_tmp49)!= 9)goto _LLAB;_tmp79=((struct Cyc_Absyn_TupleType_struct*)_tmp49)->f1;
_LLAA: Cyc_Absyndump_dump_char((int)'$');Cyc_Absyndump_dumpargs(_tmp79);return;
_LLAB: if(*((int*)_tmp49)!= 10)goto _LLAD;_tmp7A=((struct Cyc_Absyn_AggrType_struct*)
_tmp49)->f1;_tmp7B=_tmp7A.aggr_info;_tmp7C=_tmp7A.targs;_LLAC: {void*_tmpBE;
struct _tuple0*_tmpBF;struct _tuple3 _tmpBD=Cyc_Absyn_aggr_kinded_name(_tmp7B);
_tmpBE=_tmpBD.f1;_tmpBF=_tmpBD.f2;Cyc_Absyndump_dumpaggr_kind(_tmpBE);Cyc_Absyndump_dumpqvar(
_tmpBF);Cyc_Absyndump_dumptps(_tmp7C);return;}_LLAD: if(*((int*)_tmp49)!= 11)goto
_LLAF;_tmp7D=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp49)->f1;_tmp7E=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp49)->f2;_LLAE: Cyc_Absyndump_dumpaggr_kind(
_tmp7D);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpaggrfields(_tmp7E);
Cyc_Absyndump_dump_char((int)'}');return;_LLAF: if(*((int*)_tmp49)!= 12)goto _LLB1;
_tmp7F=((struct Cyc_Absyn_EnumType_struct*)_tmp49)->f1;_LLB0: Cyc_Absyndump_dump(({
const char*_tmpC0="enum ";_tag_dynforward(_tmpC0,sizeof(char),
_get_zero_arr_size_char(_tmpC0,6));}));Cyc_Absyndump_dumpqvar(_tmp7F);return;
_LLB1: if(*((int*)_tmp49)!= 13)goto _LLB3;_tmp80=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp49)->f1;_LLB2: Cyc_Absyndump_dump(({const char*_tmpC1="enum {";_tag_dynforward(
_tmpC1,sizeof(char),_get_zero_arr_size_char(_tmpC1,7));}));Cyc_Absyndump_dumpenumfields(
_tmp80);Cyc_Absyndump_dump(({const char*_tmpC2="}";_tag_dynforward(_tmpC2,sizeof(
char),_get_zero_arr_size_char(_tmpC2,2));}));return;_LLB3: if(*((int*)_tmp49)!= 
17)goto _LLB5;_tmp81=((struct Cyc_Absyn_TypedefType_struct*)_tmp49)->f1;_tmp82=((
struct Cyc_Absyn_TypedefType_struct*)_tmp49)->f2;_LLB4:(Cyc_Absyndump_dumpqvar(
_tmp81),Cyc_Absyndump_dumptps(_tmp82));return;_LLB5: if(*((int*)_tmp49)!= 14)goto
_LLB7;_tmp83=(void*)((struct Cyc_Absyn_SizeofType_struct*)_tmp49)->f1;_LLB6: Cyc_Absyndump_dump(({
const char*_tmpC3="sizeof_t<";_tag_dynforward(_tmpC3,sizeof(char),
_get_zero_arr_size_char(_tmpC3,10));}));Cyc_Absyndump_dumpntyp(_tmp83);Cyc_Absyndump_dump(({
const char*_tmpC4=">";_tag_dynforward(_tmpC4,sizeof(char),_get_zero_arr_size_char(
_tmpC4,2));}));return;_LLB7: if(*((int*)_tmp49)!= 15)goto _LLB9;_tmp84=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp49)->f1;_LLB8: Cyc_Absyndump_dump(({
const char*_tmpC5="region_t<";_tag_dynforward(_tmpC5,sizeof(char),
_get_zero_arr_size_char(_tmpC5,10));}));Cyc_Absyndump_dumprgn(_tmp84);Cyc_Absyndump_dump(({
const char*_tmpC6=">";_tag_dynforward(_tmpC6,sizeof(char),_get_zero_arr_size_char(
_tmpC6,2));}));return;_LLB9: if(*((int*)_tmp49)!= 16)goto _LLBB;_tmp85=(void*)((
struct Cyc_Absyn_DynRgnType_struct*)_tmp49)->f1;_tmp86=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp49)->f2;_LLBA: Cyc_Absyndump_dump(({const char*_tmpC7="dynregion_t<";
_tag_dynforward(_tmpC7,sizeof(char),_get_zero_arr_size_char(_tmpC7,13));}));Cyc_Absyndump_dumprgn(
_tmp85);Cyc_Absyndump_dump(({const char*_tmpC8=",";_tag_dynforward(_tmpC8,sizeof(
char),_get_zero_arr_size_char(_tmpC8,2));}));Cyc_Absyndump_dumprgn(_tmp86);Cyc_Absyndump_dump(({
const char*_tmpC9=">";_tag_dynforward(_tmpC9,sizeof(char),_get_zero_arr_size_char(
_tmpC9,2));}));return;_LLBB: if(*((int*)_tmp49)!= 19)goto _LLBD;_tmp87=((struct Cyc_Absyn_TypeInt_struct*)
_tmp49)->f1;_LLBC: Cyc_Absyndump_dump((struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmpCC;_tmpCC.tag=1;_tmpCC.f1=(unsigned long)_tmp87;{void*_tmpCA[1]={& _tmpCC};
Cyc_aprintf(({const char*_tmpCB="`%d";_tag_dynforward(_tmpCB,sizeof(char),
_get_zero_arr_size_char(_tmpCB,4));}),_tag_dynforward(_tmpCA,sizeof(void*),1));}}));
return;_LLBD: if(*((int*)_tmp49)!= 18)goto _LLBF;_tmp88=(void*)((struct Cyc_Absyn_TagType_struct*)
_tmp49)->f1;_LLBE: Cyc_Absyndump_dump(({const char*_tmpCD="tag_t<";_tag_dynforward(
_tmpCD,sizeof(char),_get_zero_arr_size_char(_tmpCD,7));}));Cyc_Absyndump_dumpntyp(
_tmp88);Cyc_Absyndump_dump(({const char*_tmpCE=">";_tag_dynforward(_tmpCE,sizeof(
char),_get_zero_arr_size_char(_tmpCE,2));}));return;_LLBF: if((int)_tmp49 != 3)
goto _LLC1;_LLC0: Cyc_Absyndump_dump(({const char*_tmpCF="`U";_tag_dynforward(
_tmpCF,sizeof(char),_get_zero_arr_size_char(_tmpCF,3));}));goto _LL74;_LLC1: if((
int)_tmp49 != 2)goto _LLC3;_LLC2: goto _LLC4;_LLC3: if(_tmp49 <= (void*)4)goto _LLC5;
if(*((int*)_tmp49)!= 20)goto _LLC5;_LLC4: goto _LLC6;_LLC5: if(_tmp49 <= (void*)4)
goto _LLC7;if(*((int*)_tmp49)!= 22)goto _LLC7;_LLC6: goto _LLC8;_LLC7: if(_tmp49 <= (
void*)4)goto _LL74;if(*((int*)_tmp49)!= 21)goto _LL74;_LLC8: return;_LL74:;}void Cyc_Absyndump_dumpvaropt(
struct Cyc_Core_Opt*vo){if(vo != 0)Cyc_Absyndump_dump_str((struct _dynforward_ptr*)
vo->v);}void Cyc_Absyndump_dumpfunarg(struct _tuple1*t){((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct Cyc_Core_Opt*),struct Cyc_Core_Opt*))Cyc_Absyndump_dumptqtd)((*
t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}void Cyc_Absyndump_dump_rgncmp(
struct _tuple7*cmp){struct _tuple7 _tmpD1;void*_tmpD2;void*_tmpD3;struct _tuple7*
_tmpD0=cmp;_tmpD1=*_tmpD0;_tmpD2=_tmpD1.f1;_tmpD3=_tmpD1.f2;Cyc_Absyndump_dumpeff(
_tmpD2);Cyc_Absyndump_dump_char((int)'>');Cyc_Absyndump_dumprgn(_tmpD3);}void Cyc_Absyndump_dump_rgnpo(
struct Cyc_List_List*rgn_po){((void(*)(void(*f)(struct _tuple7*),struct Cyc_List_List*
l,struct _dynforward_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_rgncmp,
rgn_po,({const char*_tmpD4=",";_tag_dynforward(_tmpD4,sizeof(char),
_get_zero_arr_size_char(_tmpD4,2));}));}void Cyc_Absyndump_dumpfunargs(struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_Core_Opt*
effopt,struct Cyc_List_List*rgn_po){Cyc_Absyndump_dump_char((int)'(');for(0;args
!= 0;args=args->tl){Cyc_Absyndump_dumpfunarg((struct _tuple1*)args->hd);if((args->tl
!= 0  || c_varargs) || cyc_varargs != 0)Cyc_Absyndump_dump_char((int)',');}if(
c_varargs)Cyc_Absyndump_dump(({const char*_tmpD5="...";_tag_dynforward(_tmpD5,
sizeof(char),_get_zero_arr_size_char(_tmpD5,4));}));else{if(cyc_varargs != 0){
struct _tuple1*_tmpD6=({struct _tuple1*_tmpD9=_cycalloc(sizeof(*_tmpD9));_tmpD9->f1=
cyc_varargs->name;_tmpD9->f2=cyc_varargs->tq;_tmpD9->f3=(void*)cyc_varargs->type;
_tmpD9;});Cyc_Absyndump_dump(({const char*_tmpD7="...";_tag_dynforward(_tmpD7,
sizeof(char),_get_zero_arr_size_char(_tmpD7,4));}));if(cyc_varargs->inject)Cyc_Absyndump_dump(({
const char*_tmpD8=" inject ";_tag_dynforward(_tmpD8,sizeof(char),
_get_zero_arr_size_char(_tmpD8,9));}));Cyc_Absyndump_dumpfunarg(_tmpD6);}}if(
effopt != 0){Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpeff((void*)effopt->v);}
if(rgn_po != 0){Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dump_rgnpo(rgn_po);}
Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumptyp(void*t){((void(*)(
struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(
0),t,(void(*)(int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpdesignator(
void*d){void*_tmpDA=d;struct Cyc_Absyn_Exp*_tmpDB;struct _dynforward_ptr*_tmpDC;
_LLE3: if(*((int*)_tmpDA)!= 0)goto _LLE5;_tmpDB=((struct Cyc_Absyn_ArrayElement_struct*)
_tmpDA)->f1;_LLE4: Cyc_Absyndump_dump(({const char*_tmpDD=".[";_tag_dynforward(
_tmpDD,sizeof(char),_get_zero_arr_size_char(_tmpDD,3));}));Cyc_Absyndump_dumpexp(
_tmpDB);Cyc_Absyndump_dump_char((int)']');goto _LLE2;_LLE5: if(*((int*)_tmpDA)!= 1)
goto _LLE2;_tmpDC=((struct Cyc_Absyn_FieldName_struct*)_tmpDA)->f1;_LLE6: Cyc_Absyndump_dump_char((
int)'.');Cyc_Absyndump_dump_nospace(*_tmpDC);goto _LLE2;_LLE2:;}struct _tuple10{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};void Cyc_Absyndump_dumpde(struct
_tuple10*de){Cyc_Absyndump_egroup(Cyc_Absyndump_dumpdesignator,(*de).f1,({const
char*_tmpDE="";_tag_dynforward(_tmpDE,sizeof(char),_get_zero_arr_size_char(
_tmpDE,1));}),({const char*_tmpDF="=";_tag_dynforward(_tmpDF,sizeof(char),
_get_zero_arr_size_char(_tmpDF,2));}),({const char*_tmpE0="=";_tag_dynforward(
_tmpE0,sizeof(char),_get_zero_arr_size_char(_tmpE0,2));}));Cyc_Absyndump_dumpexp((*
de).f2);}void Cyc_Absyndump_dumpexps_prec(int inprec,struct Cyc_List_List*es){((
void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct
_dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_group_c)(
Cyc_Absyndump_dumpexp_prec,inprec,es,({const char*_tmpE1="";_tag_dynforward(
_tmpE1,sizeof(char),_get_zero_arr_size_char(_tmpE1,1));}),({const char*_tmpE2="";
_tag_dynforward(_tmpE2,sizeof(char),_get_zero_arr_size_char(_tmpE2,1));}),({
const char*_tmpE3=",";_tag_dynforward(_tmpE3,sizeof(char),_get_zero_arr_size_char(
_tmpE3,2));}));}void Cyc_Absyndump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);if(inprec >= myprec)Cyc_Absyndump_dump_nospace(({
const char*_tmpE4="(";_tag_dynforward(_tmpE4,sizeof(char),_get_zero_arr_size_char(
_tmpE4,2));}));{void*_tmpE5=(void*)e->r;union Cyc_Absyn_Cnst_union _tmpE6;void*
_tmpE7;char _tmpE8;union Cyc_Absyn_Cnst_union _tmpE9;void*_tmpEA;short _tmpEB;union
Cyc_Absyn_Cnst_union _tmpEC;void*_tmpED;int _tmpEE;union Cyc_Absyn_Cnst_union _tmpEF;
void*_tmpF0;int _tmpF1;union Cyc_Absyn_Cnst_union _tmpF2;void*_tmpF3;int _tmpF4;
union Cyc_Absyn_Cnst_union _tmpF5;void*_tmpF6;long long _tmpF7;union Cyc_Absyn_Cnst_union
_tmpF8;struct _dynforward_ptr _tmpF9;union Cyc_Absyn_Cnst_union _tmpFA;union Cyc_Absyn_Cnst_union
_tmpFB;struct _dynforward_ptr _tmpFC;struct _tuple0*_tmpFD;struct _tuple0*_tmpFE;
void*_tmpFF;struct Cyc_List_List*_tmp100;struct Cyc_Absyn_Exp*_tmp101;struct Cyc_Core_Opt*
_tmp102;struct Cyc_Absyn_Exp*_tmp103;struct Cyc_Absyn_Exp*_tmp104;void*_tmp105;
struct Cyc_Absyn_Exp*_tmp106;void*_tmp107;struct Cyc_Absyn_Exp*_tmp108;void*
_tmp109;struct Cyc_Absyn_Exp*_tmp10A;void*_tmp10B;struct Cyc_Absyn_Exp*_tmp10C;
struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*
_tmp10F;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Absyn_Exp*
_tmp112;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*
_tmp115;struct Cyc_List_List*_tmp116;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_List_List*
_tmp118;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*
_tmp11B;void*_tmp11C;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11E;
struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp120;void*_tmp121;struct Cyc_Absyn_Exp*
_tmp122;void*_tmp123;void*_tmp124;struct _dynforward_ptr*_tmp125;void*_tmp126;
void*_tmp127;unsigned int _tmp128;struct Cyc_List_List*_tmp129;void*_tmp12A;struct
Cyc_Absyn_Exp*_tmp12B;struct Cyc_Absyn_Exp*_tmp12C;struct _dynforward_ptr*_tmp12D;
struct Cyc_Absyn_Exp*_tmp12E;struct _dynforward_ptr*_tmp12F;struct Cyc_Absyn_Exp*
_tmp130;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_List_List*_tmp132;struct _tuple1*
_tmp133;struct Cyc_List_List*_tmp134;struct Cyc_List_List*_tmp135;struct Cyc_Absyn_Vardecl*
_tmp136;struct Cyc_Absyn_Exp*_tmp137;struct Cyc_Absyn_Exp*_tmp138;struct _tuple0*
_tmp139;struct Cyc_List_List*_tmp13A;struct Cyc_List_List*_tmp13B;struct Cyc_List_List*
_tmp13C;struct Cyc_List_List*_tmp13D;struct Cyc_Absyn_Tunionfield*_tmp13E;struct
_tuple0*_tmp13F;struct _tuple0*_tmp140;struct Cyc_Absyn_MallocInfo _tmp141;int
_tmp142;struct Cyc_Absyn_Exp*_tmp143;void**_tmp144;struct Cyc_Absyn_Exp*_tmp145;
struct Cyc_Absyn_Exp*_tmp146;struct Cyc_Absyn_Exp*_tmp147;struct Cyc_Core_Opt*
_tmp148;struct Cyc_List_List*_tmp149;struct Cyc_Absyn_Stmt*_tmp14A;_LLE8: if(*((int*)
_tmpE5)!= 0)goto _LLEA;_tmpE6=((struct Cyc_Absyn_Const_e_struct*)_tmpE5)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmpE5)->f1).Char_c).tag != 0)goto _LLEA;_tmpE7=(
_tmpE6.Char_c).f1;_tmpE8=(_tmpE6.Char_c).f2;_LLE9: Cyc_Absyndump_dump_char((int)'\'');
Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmpE8));Cyc_Absyndump_dump_char((
int)'\'');goto _LLE7;_LLEA: if(*((int*)_tmpE5)!= 0)goto _LLEC;_tmpE9=((struct Cyc_Absyn_Const_e_struct*)
_tmpE5)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmpE5)->f1).Short_c).tag != 1)
goto _LLEC;_tmpEA=(_tmpE9.Short_c).f1;_tmpEB=(_tmpE9.Short_c).f2;_LLEB: Cyc_Absyndump_dump((
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp14D;_tmp14D.tag=1;_tmp14D.f1=(
unsigned long)((int)_tmpEB);{void*_tmp14B[1]={& _tmp14D};Cyc_aprintf(({const char*
_tmp14C="%d";_tag_dynforward(_tmp14C,sizeof(char),_get_zero_arr_size_char(
_tmp14C,3));}),_tag_dynforward(_tmp14B,sizeof(void*),1));}}));goto _LLE7;_LLEC:
if(*((int*)_tmpE5)!= 0)goto _LLEE;_tmpEC=((struct Cyc_Absyn_Const_e_struct*)_tmpE5)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmpE5)->f1).Int_c).tag != 2)goto _LLEE;
_tmpED=(_tmpEC.Int_c).f1;if((int)_tmpED != 2)goto _LLEE;_tmpEE=(_tmpEC.Int_c).f2;
_LLED: _tmpF1=_tmpEE;goto _LLEF;_LLEE: if(*((int*)_tmpE5)!= 0)goto _LLF0;_tmpEF=((
struct Cyc_Absyn_Const_e_struct*)_tmpE5)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpE5)->f1).Int_c).tag != 2)goto _LLF0;_tmpF0=(_tmpEF.Int_c).f1;if((int)_tmpF0 != 
0)goto _LLF0;_tmpF1=(_tmpEF.Int_c).f2;_LLEF: Cyc_Absyndump_dump((struct
_dynforward_ptr)({struct Cyc_Int_pa_struct _tmp150;_tmp150.tag=1;_tmp150.f1=(
unsigned long)_tmpF1;{void*_tmp14E[1]={& _tmp150};Cyc_aprintf(({const char*_tmp14F="%d";
_tag_dynforward(_tmp14F,sizeof(char),_get_zero_arr_size_char(_tmp14F,3));}),
_tag_dynforward(_tmp14E,sizeof(void*),1));}}));goto _LLE7;_LLF0: if(*((int*)_tmpE5)
!= 0)goto _LLF2;_tmpF2=((struct Cyc_Absyn_Const_e_struct*)_tmpE5)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmpE5)->f1).Int_c).tag != 2)goto _LLF2;_tmpF3=(
_tmpF2.Int_c).f1;if((int)_tmpF3 != 1)goto _LLF2;_tmpF4=(_tmpF2.Int_c).f2;_LLF1: Cyc_Absyndump_dump((
struct _dynforward_ptr)({struct Cyc_Int_pa_struct _tmp153;_tmp153.tag=1;_tmp153.f1=(
unsigned int)_tmpF4;{void*_tmp151[1]={& _tmp153};Cyc_aprintf(({const char*_tmp152="%u";
_tag_dynforward(_tmp152,sizeof(char),_get_zero_arr_size_char(_tmp152,3));}),
_tag_dynforward(_tmp151,sizeof(void*),1));}}));goto _LLE7;_LLF2: if(*((int*)_tmpE5)
!= 0)goto _LLF4;_tmpF5=((struct Cyc_Absyn_Const_e_struct*)_tmpE5)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmpE5)->f1).LongLong_c).tag != 3)goto _LLF4;
_tmpF6=(_tmpF5.LongLong_c).f1;_tmpF7=(_tmpF5.LongLong_c).f2;_LLF3: Cyc_Absyndump_dump(({
const char*_tmp154="<<FIX LONG LONG CONSTANT>>";_tag_dynforward(_tmp154,sizeof(
char),_get_zero_arr_size_char(_tmp154,27));}));goto _LLE7;_LLF4: if(*((int*)_tmpE5)
!= 0)goto _LLF6;_tmpF8=((struct Cyc_Absyn_Const_e_struct*)_tmpE5)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmpE5)->f1).Float_c).tag != 4)goto _LLF6;_tmpF9=(
_tmpF8.Float_c).f1;_LLF5: Cyc_Absyndump_dump(_tmpF9);goto _LLE7;_LLF6: if(*((int*)
_tmpE5)!= 0)goto _LLF8;_tmpFA=((struct Cyc_Absyn_Const_e_struct*)_tmpE5)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmpE5)->f1).Null_c).tag != 6)goto _LLF8;_LLF7: Cyc_Absyndump_dump(({
const char*_tmp155="NULL";_tag_dynforward(_tmp155,sizeof(char),
_get_zero_arr_size_char(_tmp155,5));}));goto _LLE7;_LLF8: if(*((int*)_tmpE5)!= 0)
goto _LLFA;_tmpFB=((struct Cyc_Absyn_Const_e_struct*)_tmpE5)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpE5)->f1).String_c).tag != 5)goto _LLFA;_tmpFC=(_tmpFB.String_c).f1;_LLF9: Cyc_Absyndump_dump_char((
int)'"');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_tmpFC));Cyc_Absyndump_dump_char((
int)'"');goto _LLE7;_LLFA: if(*((int*)_tmpE5)!= 2)goto _LLFC;_tmpFD=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmpE5)->f1;_LLFB: _tmpFE=_tmpFD;goto _LLFD;_LLFC: if(*((int*)_tmpE5)!= 1)goto _LLFE;
_tmpFE=((struct Cyc_Absyn_Var_e_struct*)_tmpE5)->f1;_LLFD: Cyc_Absyndump_dumpqvar(
_tmpFE);goto _LLE7;_LLFE: if(*((int*)_tmpE5)!= 3)goto _LL100;_tmpFF=(void*)((struct
Cyc_Absyn_Primop_e_struct*)_tmpE5)->f1;_tmp100=((struct Cyc_Absyn_Primop_e_struct*)
_tmpE5)->f2;_LLFF: {struct _dynforward_ptr _tmp156=Cyc_Absynpp_prim2str(_tmpFF);
switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp100)){case 1: _LL14E:
if(_tmpFF == (void*)((void*)19)){Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp100))->hd);Cyc_Absyndump_dump(({const char*
_tmp157=".size";_tag_dynforward(_tmp157,sizeof(char),_get_zero_arr_size_char(
_tmp157,6));}));}else{Cyc_Absyndump_dump(_tmp156);Cyc_Absyndump_dumpexp_prec(
myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp100))->hd);}
break;case 2: _LL14F: Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp100))->hd);Cyc_Absyndump_dump(_tmp156);Cyc_Absyndump_dump_char((
int)' ');Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp100->tl))->hd);break;default: _LL150:(int)_throw((void*)({struct
Cyc_Core_Failure_struct*_tmp158=_cycalloc(sizeof(*_tmp158));_tmp158[0]=({struct
Cyc_Core_Failure_struct _tmp159;_tmp159.tag=Cyc_Core_Failure;_tmp159.f1=({const
char*_tmp15A="Absyndump -- Bad number of arguments to primop";_tag_dynforward(
_tmp15A,sizeof(char),_get_zero_arr_size_char(_tmp15A,47));});_tmp159;});_tmp158;}));}
goto _LLE7;}_LL100: if(*((int*)_tmpE5)!= 4)goto _LL102;_tmp101=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpE5)->f1;_tmp102=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpE5)->f2;_tmp103=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmpE5)->f3;_LL101: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp101);if(_tmp102 != 0)Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((void*)
_tmp102->v));Cyc_Absyndump_dump_nospace(({const char*_tmp15B="=";_tag_dynforward(
_tmp15B,sizeof(char),_get_zero_arr_size_char(_tmp15B,2));}));Cyc_Absyndump_dumpexp_prec(
myprec,_tmp103);goto _LLE7;_LL102: if(*((int*)_tmpE5)!= 5)goto _LL104;_tmp104=((
struct Cyc_Absyn_Increment_e_struct*)_tmpE5)->f1;_tmp105=(void*)((struct Cyc_Absyn_Increment_e_struct*)
_tmpE5)->f2;if((int)_tmp105 != 0)goto _LL104;_LL103: Cyc_Absyndump_dump(({const char*
_tmp15C="++";_tag_dynforward(_tmp15C,sizeof(char),_get_zero_arr_size_char(
_tmp15C,3));}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp104);goto _LLE7;_LL104: if(*((
int*)_tmpE5)!= 5)goto _LL106;_tmp106=((struct Cyc_Absyn_Increment_e_struct*)_tmpE5)->f1;
_tmp107=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmpE5)->f2;if((int)_tmp107
!= 2)goto _LL106;_LL105: Cyc_Absyndump_dump(({const char*_tmp15D="--";
_tag_dynforward(_tmp15D,sizeof(char),_get_zero_arr_size_char(_tmp15D,3));}));Cyc_Absyndump_dumpexp_prec(
myprec,_tmp106);goto _LLE7;_LL106: if(*((int*)_tmpE5)!= 5)goto _LL108;_tmp108=((
struct Cyc_Absyn_Increment_e_struct*)_tmpE5)->f1;_tmp109=(void*)((struct Cyc_Absyn_Increment_e_struct*)
_tmpE5)->f2;if((int)_tmp109 != 1)goto _LL108;_LL107: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp108);Cyc_Absyndump_dump(({const char*_tmp15E="++";_tag_dynforward(
_tmp15E,sizeof(char),_get_zero_arr_size_char(_tmp15E,3));}));goto _LLE7;_LL108:
if(*((int*)_tmpE5)!= 5)goto _LL10A;_tmp10A=((struct Cyc_Absyn_Increment_e_struct*)
_tmpE5)->f1;_tmp10B=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmpE5)->f2;if((
int)_tmp10B != 3)goto _LL10A;_LL109: Cyc_Absyndump_dumpexp_prec(myprec,_tmp10A);Cyc_Absyndump_dump(({
const char*_tmp15F="--";_tag_dynforward(_tmp15F,sizeof(char),
_get_zero_arr_size_char(_tmp15F,3));}));goto _LLE7;_LL10A: if(*((int*)_tmpE5)!= 6)
goto _LL10C;_tmp10C=((struct Cyc_Absyn_Conditional_e_struct*)_tmpE5)->f1;_tmp10D=((
struct Cyc_Absyn_Conditional_e_struct*)_tmpE5)->f2;_tmp10E=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpE5)->f3;_LL10B: Cyc_Absyndump_dumpexp_prec(myprec,_tmp10C);Cyc_Absyndump_dump_char((
int)'?');Cyc_Absyndump_dumpexp_prec(0,_tmp10D);Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp10E);goto _LLE7;_LL10C: if(*((int*)_tmpE5)!= 
7)goto _LL10E;_tmp10F=((struct Cyc_Absyn_And_e_struct*)_tmpE5)->f1;_tmp110=((
struct Cyc_Absyn_And_e_struct*)_tmpE5)->f2;_LL10D: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp10F);Cyc_Absyndump_dump(({const char*_tmp160=" && ";_tag_dynforward(
_tmp160,sizeof(char),_get_zero_arr_size_char(_tmp160,5));}));Cyc_Absyndump_dumpexp_prec(
myprec,_tmp110);goto _LLE7;_LL10E: if(*((int*)_tmpE5)!= 8)goto _LL110;_tmp111=((
struct Cyc_Absyn_Or_e_struct*)_tmpE5)->f1;_tmp112=((struct Cyc_Absyn_Or_e_struct*)
_tmpE5)->f2;_LL10F: Cyc_Absyndump_dumpexp_prec(myprec,_tmp111);Cyc_Absyndump_dump(({
const char*_tmp161=" || ";_tag_dynforward(_tmp161,sizeof(char),
_get_zero_arr_size_char(_tmp161,5));}));Cyc_Absyndump_dumpexp_prec(myprec,
_tmp112);goto _LLE7;_LL110: if(*((int*)_tmpE5)!= 9)goto _LL112;_tmp113=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpE5)->f1;_tmp114=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpE5)->f2;_LL111: Cyc_Absyndump_dump_char((
int)'(');Cyc_Absyndump_dumpexp_prec(myprec,_tmp113);Cyc_Absyndump_dump_char((int)',');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp114);Cyc_Absyndump_dump_char((int)')');
goto _LLE7;_LL112: if(*((int*)_tmpE5)!= 10)goto _LL114;_tmp115=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmpE5)->f1;_tmp116=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmpE5)->f2;_LL113:
_tmp117=_tmp115;_tmp118=_tmp116;goto _LL115;_LL114: if(*((int*)_tmpE5)!= 11)goto
_LL116;_tmp117=((struct Cyc_Absyn_FnCall_e_struct*)_tmpE5)->f1;_tmp118=((struct
Cyc_Absyn_FnCall_e_struct*)_tmpE5)->f2;_LL115: Cyc_Absyndump_dumpexp_prec(myprec,
_tmp117);Cyc_Absyndump_dump_nospace(({const char*_tmp162="(";_tag_dynforward(
_tmp162,sizeof(char),_get_zero_arr_size_char(_tmp162,2));}));Cyc_Absyndump_dumpexps_prec(
20,_tmp118);Cyc_Absyndump_dump_nospace(({const char*_tmp163=")";_tag_dynforward(
_tmp163,sizeof(char),_get_zero_arr_size_char(_tmp163,2));}));goto _LLE7;_LL116:
if(*((int*)_tmpE5)!= 12)goto _LL118;_tmp119=((struct Cyc_Absyn_Throw_e_struct*)
_tmpE5)->f1;_LL117: Cyc_Absyndump_dump(({const char*_tmp164="throw";
_tag_dynforward(_tmp164,sizeof(char),_get_zero_arr_size_char(_tmp164,6));}));Cyc_Absyndump_dumpexp_prec(
myprec,_tmp119);goto _LLE7;_LL118: if(*((int*)_tmpE5)!= 13)goto _LL11A;_tmp11A=((
struct Cyc_Absyn_NoInstantiate_e_struct*)_tmpE5)->f1;_LL119: _tmp11B=_tmp11A;goto
_LL11B;_LL11A: if(*((int*)_tmpE5)!= 14)goto _LL11C;_tmp11B=((struct Cyc_Absyn_Instantiate_e_struct*)
_tmpE5)->f1;_LL11B: Cyc_Absyndump_dumpexp_prec(inprec,_tmp11B);goto _LLE7;_LL11C:
if(*((int*)_tmpE5)!= 15)goto _LL11E;_tmp11C=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmpE5)->f1;_tmp11D=((struct Cyc_Absyn_Cast_e_struct*)_tmpE5)->f2;_LL11D: Cyc_Absyndump_dump_char((
int)'(');Cyc_Absyndump_dumptyp(_tmp11C);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp11D);goto _LLE7;_LL11E: if(*((int*)_tmpE5)!= 16)goto _LL120;_tmp11E=((
struct Cyc_Absyn_Address_e_struct*)_tmpE5)->f1;_LL11F: Cyc_Absyndump_dump_char((
int)'&');Cyc_Absyndump_dumpexp_prec(myprec,_tmp11E);goto _LLE7;_LL120: if(*((int*)
_tmpE5)!= 17)goto _LL122;_tmp11F=((struct Cyc_Absyn_New_e_struct*)_tmpE5)->f1;
_tmp120=((struct Cyc_Absyn_New_e_struct*)_tmpE5)->f2;_LL121: Cyc_Absyndump_dump(({
const char*_tmp165="new ";_tag_dynforward(_tmp165,sizeof(char),
_get_zero_arr_size_char(_tmp165,5));}));Cyc_Absyndump_dumpexp_prec(myprec,
_tmp120);goto _LLE7;_LL122: if(*((int*)_tmpE5)!= 18)goto _LL124;_tmp121=(void*)((
struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpE5)->f1;_LL123: Cyc_Absyndump_dump(({
const char*_tmp166="sizeof(";_tag_dynforward(_tmp166,sizeof(char),
_get_zero_arr_size_char(_tmp166,8));}));Cyc_Absyndump_dumptyp(_tmp121);Cyc_Absyndump_dump_char((
int)')');goto _LLE7;_LL124: if(*((int*)_tmpE5)!= 19)goto _LL126;_tmp122=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmpE5)->f1;_LL125: Cyc_Absyndump_dump(({const char*_tmp167="sizeof(";
_tag_dynforward(_tmp167,sizeof(char),_get_zero_arr_size_char(_tmp167,8));}));Cyc_Absyndump_dumpexp(
_tmp122);Cyc_Absyndump_dump_char((int)')');goto _LLE7;_LL126: if(*((int*)_tmpE5)!= 
20)goto _LL128;_tmp123=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpE5)->f1;
_tmp124=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpE5)->f2;if(*((int*)
_tmp124)!= 0)goto _LL128;_tmp125=((struct Cyc_Absyn_StructField_struct*)_tmp124)->f1;
_LL127: Cyc_Absyndump_dump(({const char*_tmp168="offsetof(";_tag_dynforward(
_tmp168,sizeof(char),_get_zero_arr_size_char(_tmp168,10));}));Cyc_Absyndump_dumptyp(
_tmp123);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dump_nospace(*_tmp125);
Cyc_Absyndump_dump_char((int)')');goto _LLE7;_LL128: if(*((int*)_tmpE5)!= 20)goto
_LL12A;_tmp126=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpE5)->f1;_tmp127=(
void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpE5)->f2;if(*((int*)_tmp127)!= 1)
goto _LL12A;_tmp128=((struct Cyc_Absyn_TupleIndex_struct*)_tmp127)->f1;_LL129: Cyc_Absyndump_dump(({
const char*_tmp169="offsetof(";_tag_dynforward(_tmp169,sizeof(char),
_get_zero_arr_size_char(_tmp169,10));}));Cyc_Absyndump_dumptyp(_tmp126);Cyc_Absyndump_dump_char((
int)',');Cyc_Absyndump_dump((struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmp16C;_tmp16C.tag=1;_tmp16C.f1=(unsigned long)((int)_tmp128);{void*_tmp16A[1]={&
_tmp16C};Cyc_aprintf(({const char*_tmp16B="%d";_tag_dynforward(_tmp16B,sizeof(
char),_get_zero_arr_size_char(_tmp16B,3));}),_tag_dynforward(_tmp16A,sizeof(void*),
1));}}));Cyc_Absyndump_dump_char((int)')');goto _LLE7;_LL12A: if(*((int*)_tmpE5)!= 
21)goto _LL12C;_tmp129=((struct Cyc_Absyn_Gentyp_e_struct*)_tmpE5)->f1;_tmp12A=(
void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmpE5)->f2;_LL12B: Cyc_Absyndump_dump(({
const char*_tmp16D="__gen(";_tag_dynforward(_tmp16D,sizeof(char),
_get_zero_arr_size_char(_tmp16D,7));}));Cyc_Absyndump_dumptvars(_tmp129);Cyc_Absyndump_dumptyp(
_tmp12A);Cyc_Absyndump_dump_char((int)')');goto _LLE7;_LL12C: if(*((int*)_tmpE5)!= 
22)goto _LL12E;_tmp12B=((struct Cyc_Absyn_Deref_e_struct*)_tmpE5)->f1;_LL12D: Cyc_Absyndump_dump_char((
int)'*');Cyc_Absyndump_dumpexp_prec(myprec,_tmp12B);goto _LLE7;_LL12E: if(*((int*)
_tmpE5)!= 23)goto _LL130;_tmp12C=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpE5)->f1;
_tmp12D=((struct Cyc_Absyn_AggrMember_e_struct*)_tmpE5)->f2;_LL12F: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp12C);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*
_tmp12D);goto _LLE7;_LL130: if(*((int*)_tmpE5)!= 24)goto _LL132;_tmp12E=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmpE5)->f1;_tmp12F=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpE5)->f2;_LL131: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp12E);Cyc_Absyndump_dump_nospace(({const char*_tmp16E="->";
_tag_dynforward(_tmp16E,sizeof(char),_get_zero_arr_size_char(_tmp16E,3));}));Cyc_Absyndump_dump_nospace(*
_tmp12F);goto _LLE7;_LL132: if(*((int*)_tmpE5)!= 25)goto _LL134;_tmp130=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpE5)->f1;_tmp131=((struct Cyc_Absyn_Subscript_e_struct*)_tmpE5)->f2;_LL133: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp130);Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp131);
Cyc_Absyndump_dump_char((int)']');goto _LLE7;_LL134: if(*((int*)_tmpE5)!= 26)goto
_LL136;_tmp132=((struct Cyc_Absyn_Tuple_e_struct*)_tmpE5)->f1;_LL135: Cyc_Absyndump_dump(({
const char*_tmp16F="$(";_tag_dynforward(_tmp16F,sizeof(char),
_get_zero_arr_size_char(_tmp16F,3));}));Cyc_Absyndump_dumpexps_prec(20,_tmp132);
Cyc_Absyndump_dump_char((int)')');goto _LLE7;_LL136: if(*((int*)_tmpE5)!= 27)goto
_LL138;_tmp133=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmpE5)->f1;_tmp134=((
struct Cyc_Absyn_CompoundLit_e_struct*)_tmpE5)->f2;_LL137: Cyc_Absyndump_dump_char((
int)'(');Cyc_Absyndump_dumptyp((*_tmp133).f3);Cyc_Absyndump_dump_char((int)')');((
void(*)(void(*f)(struct _tuple10*),struct Cyc_List_List*l,struct _dynforward_ptr
start,struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp134,({const char*_tmp170="{";_tag_dynforward(_tmp170,
sizeof(char),_get_zero_arr_size_char(_tmp170,2));}),({const char*_tmp171="}";
_tag_dynforward(_tmp171,sizeof(char),_get_zero_arr_size_char(_tmp171,2));}),({
const char*_tmp172=",";_tag_dynforward(_tmp172,sizeof(char),
_get_zero_arr_size_char(_tmp172,2));}));goto _LLE7;_LL138: if(*((int*)_tmpE5)!= 28)
goto _LL13A;_tmp135=((struct Cyc_Absyn_Array_e_struct*)_tmpE5)->f1;_LL139:((void(*)(
void(*f)(struct _tuple10*),struct Cyc_List_List*l,struct _dynforward_ptr start,
struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,
_tmp135,({const char*_tmp173="{";_tag_dynforward(_tmp173,sizeof(char),
_get_zero_arr_size_char(_tmp173,2));}),({const char*_tmp174="}";_tag_dynforward(
_tmp174,sizeof(char),_get_zero_arr_size_char(_tmp174,2));}),({const char*_tmp175=",";
_tag_dynforward(_tmp175,sizeof(char),_get_zero_arr_size_char(_tmp175,2));}));
goto _LLE7;_LL13A: if(*((int*)_tmpE5)!= 29)goto _LL13C;_tmp136=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpE5)->f1;_tmp137=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpE5)->f2;
_tmp138=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpE5)->f3;_LL13B: Cyc_Absyndump_dump(({
const char*_tmp176="new {for";_tag_dynforward(_tmp176,sizeof(char),
_get_zero_arr_size_char(_tmp176,9));}));Cyc_Absyndump_dump_str((*_tmp136->name).f2);
Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmp137);Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dumpexp(_tmp138);Cyc_Absyndump_dump_char((int)'}');goto
_LLE7;_LL13C: if(*((int*)_tmpE5)!= 30)goto _LL13E;_tmp139=((struct Cyc_Absyn_Struct_e_struct*)
_tmpE5)->f1;_tmp13A=((struct Cyc_Absyn_Struct_e_struct*)_tmpE5)->f2;_tmp13B=((
struct Cyc_Absyn_Struct_e_struct*)_tmpE5)->f3;_LL13D: Cyc_Absyndump_dumpqvar(
_tmp139);Cyc_Absyndump_dump_char((int)'{');if(_tmp13A != 0)Cyc_Absyndump_dumptps(
_tmp13A);((void(*)(void(*f)(struct _tuple10*),struct Cyc_List_List*l,struct
_dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp13B,({const char*_tmp177="";_tag_dynforward(_tmp177,
sizeof(char),_get_zero_arr_size_char(_tmp177,1));}),({const char*_tmp178="}";
_tag_dynforward(_tmp178,sizeof(char),_get_zero_arr_size_char(_tmp178,2));}),({
const char*_tmp179=",";_tag_dynforward(_tmp179,sizeof(char),
_get_zero_arr_size_char(_tmp179,2));}));goto _LLE7;_LL13E: if(*((int*)_tmpE5)!= 31)
goto _LL140;_tmp13C=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmpE5)->f2;_LL13F:((
void(*)(void(*f)(struct _tuple10*),struct Cyc_List_List*l,struct _dynforward_ptr
start,struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp13C,({const char*_tmp17A="{";_tag_dynforward(_tmp17A,
sizeof(char),_get_zero_arr_size_char(_tmp17A,2));}),({const char*_tmp17B="}";
_tag_dynforward(_tmp17B,sizeof(char),_get_zero_arr_size_char(_tmp17B,2));}),({
const char*_tmp17C=",";_tag_dynforward(_tmp17C,sizeof(char),
_get_zero_arr_size_char(_tmp17C,2));}));goto _LLE7;_LL140: if(*((int*)_tmpE5)!= 32)
goto _LL142;_tmp13D=((struct Cyc_Absyn_Tunion_e_struct*)_tmpE5)->f1;_tmp13E=((
struct Cyc_Absyn_Tunion_e_struct*)_tmpE5)->f3;_LL141: Cyc_Absyndump_dumpqvar(
_tmp13E->name);if(_tmp13D != 0)((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
l,struct _dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))
Cyc_Absyndump_group)(Cyc_Absyndump_dumpexp,_tmp13D,({const char*_tmp17D="(";
_tag_dynforward(_tmp17D,sizeof(char),_get_zero_arr_size_char(_tmp17D,2));}),({
const char*_tmp17E=")";_tag_dynforward(_tmp17E,sizeof(char),
_get_zero_arr_size_char(_tmp17E,2));}),({const char*_tmp17F=",";_tag_dynforward(
_tmp17F,sizeof(char),_get_zero_arr_size_char(_tmp17F,2));}));goto _LLE7;_LL142:
if(*((int*)_tmpE5)!= 33)goto _LL144;_tmp13F=((struct Cyc_Absyn_Enum_e_struct*)
_tmpE5)->f1;_LL143: Cyc_Absyndump_dumpqvar(_tmp13F);goto _LLE7;_LL144: if(*((int*)
_tmpE5)!= 34)goto _LL146;_tmp140=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmpE5)->f1;
_LL145: Cyc_Absyndump_dumpqvar(_tmp140);goto _LLE7;_LL146: if(*((int*)_tmpE5)!= 35)
goto _LL148;_tmp141=((struct Cyc_Absyn_Malloc_e_struct*)_tmpE5)->f1;_tmp142=
_tmp141.is_calloc;_tmp143=_tmp141.rgn;_tmp144=_tmp141.elt_type;_tmp145=_tmp141.num_elts;
_LL147: if(_tmp142){if(_tmp143 != 0){Cyc_Absyndump_dump(({const char*_tmp180="rcalloc(";
_tag_dynforward(_tmp180,sizeof(char),_get_zero_arr_size_char(_tmp180,9));}));Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)_tmp143);Cyc_Absyndump_dump(({const char*_tmp181=",";
_tag_dynforward(_tmp181,sizeof(char),_get_zero_arr_size_char(_tmp181,2));}));}
else{Cyc_Absyndump_dump(({const char*_tmp182="calloc";_tag_dynforward(_tmp182,
sizeof(char),_get_zero_arr_size_char(_tmp182,7));}));}Cyc_Absyndump_dumpexp(
_tmp145);Cyc_Absyndump_dump(({const char*_tmp183=",";_tag_dynforward(_tmp183,
sizeof(char),_get_zero_arr_size_char(_tmp183,2));}));Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftyp_exp(*((
void**)_check_null(_tmp144)),0));Cyc_Absyndump_dump(({const char*_tmp184=")";
_tag_dynforward(_tmp184,sizeof(char),_get_zero_arr_size_char(_tmp184,2));}));}
else{if(_tmp143 != 0){Cyc_Absyndump_dump(({const char*_tmp185="rmalloc(";
_tag_dynforward(_tmp185,sizeof(char),_get_zero_arr_size_char(_tmp185,9));}));Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)_tmp143);Cyc_Absyndump_dump(({const char*_tmp186=",";
_tag_dynforward(_tmp186,sizeof(char),_get_zero_arr_size_char(_tmp186,2));}));}
else{Cyc_Absyndump_dump(({const char*_tmp187="malloc(";_tag_dynforward(_tmp187,
sizeof(char),_get_zero_arr_size_char(_tmp187,8));}));}if(_tmp144 != 0)Cyc_Absyndump_dumpexp(
Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmp144,0),_tmp145,0));else{Cyc_Absyndump_dumpexp(
_tmp145);}Cyc_Absyndump_dump(({const char*_tmp188=")";_tag_dynforward(_tmp188,
sizeof(char),_get_zero_arr_size_char(_tmp188,2));}));}goto _LLE7;_LL148: if(*((int*)
_tmpE5)!= 36)goto _LL14A;_tmp146=((struct Cyc_Absyn_Swap_e_struct*)_tmpE5)->f1;
_tmp147=((struct Cyc_Absyn_Swap_e_struct*)_tmpE5)->f2;_LL149: Cyc_Absyndump_dump(({
const char*_tmp189="cycswap(";_tag_dynforward(_tmp189,sizeof(char),
_get_zero_arr_size_char(_tmp189,9));}));Cyc_Absyndump_dumpexp_prec(myprec,
_tmp146);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(myprec,
_tmp147);Cyc_Absyndump_dump_char((int)')');goto _LLE7;_LL14A: if(*((int*)_tmpE5)!= 
37)goto _LL14C;_tmp148=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpE5)->f1;
_tmp149=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpE5)->f2;_LL14B:((void(*)(
void(*f)(struct _tuple10*),struct Cyc_List_List*l,struct _dynforward_ptr start,
struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,
_tmp149,({const char*_tmp18A="{";_tag_dynforward(_tmp18A,sizeof(char),
_get_zero_arr_size_char(_tmp18A,2));}),({const char*_tmp18B="}";_tag_dynforward(
_tmp18B,sizeof(char),_get_zero_arr_size_char(_tmp18B,2));}),({const char*_tmp18C=",";
_tag_dynforward(_tmp18C,sizeof(char),_get_zero_arr_size_char(_tmp18C,2));}));
goto _LLE7;_LL14C: if(*((int*)_tmpE5)!= 38)goto _LLE7;_tmp14A=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmpE5)->f1;_LL14D: Cyc_Absyndump_dump_nospace(({const char*_tmp18D="({";
_tag_dynforward(_tmp18D,sizeof(char),_get_zero_arr_size_char(_tmp18D,3));}));Cyc_Absyndump_dumpstmt(
_tmp14A);Cyc_Absyndump_dump_nospace(({const char*_tmp18E="})";_tag_dynforward(
_tmp18E,sizeof(char),_get_zero_arr_size_char(_tmp18E,3));}));goto _LLE7;_LLE7:;}
if(inprec >= myprec)Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumpexp(
struct Cyc_Absyn_Exp*e){Cyc_Absyndump_dumpexp_prec(0,e);}void Cyc_Absyndump_dumpswitchclauses(
struct Cyc_List_List*scs){for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause*
_tmp18F=(struct Cyc_Absyn_Switch_clause*)scs->hd;if(_tmp18F->where_clause == 0  && (
void*)(_tmp18F->pattern)->r == (void*)((void*)0))Cyc_Absyndump_dump(({const char*
_tmp190="default:";_tag_dynforward(_tmp190,sizeof(char),_get_zero_arr_size_char(
_tmp190,9));}));else{Cyc_Absyndump_dump(({const char*_tmp191="case";
_tag_dynforward(_tmp191,sizeof(char),_get_zero_arr_size_char(_tmp191,5));}));Cyc_Absyndump_dumppat(
_tmp18F->pattern);if(_tmp18F->where_clause != 0){Cyc_Absyndump_dump(({const char*
_tmp192="&&";_tag_dynforward(_tmp192,sizeof(char),_get_zero_arr_size_char(
_tmp192,3));}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp18F->where_clause));}
Cyc_Absyndump_dump_nospace(({const char*_tmp193=":";_tag_dynforward(_tmp193,
sizeof(char),_get_zero_arr_size_char(_tmp193,2));}));}Cyc_Absyndump_dumpstmt(
_tmp18F->body);}}void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s){void*_tmp194=(
void*)s->r;struct Cyc_Absyn_Exp*_tmp195;struct Cyc_Absyn_Stmt*_tmp196;struct Cyc_Absyn_Stmt*
_tmp197;struct Cyc_Absyn_Exp*_tmp198;struct Cyc_Absyn_Exp*_tmp199;struct Cyc_Absyn_Exp*
_tmp19A;struct Cyc_Absyn_Stmt*_tmp19B;struct Cyc_Absyn_Stmt*_tmp19C;struct _tuple2
_tmp19D;struct Cyc_Absyn_Exp*_tmp19E;struct Cyc_Absyn_Stmt*_tmp19F;struct
_dynforward_ptr*_tmp1A0;struct Cyc_Absyn_Exp*_tmp1A1;struct _tuple2 _tmp1A2;struct
Cyc_Absyn_Exp*_tmp1A3;struct _tuple2 _tmp1A4;struct Cyc_Absyn_Exp*_tmp1A5;struct Cyc_Absyn_Stmt*
_tmp1A6;struct Cyc_Absyn_Exp*_tmp1A7;struct Cyc_List_List*_tmp1A8;struct Cyc_Absyn_Decl*
_tmp1A9;struct Cyc_Absyn_Stmt*_tmp1AA;struct _dynforward_ptr*_tmp1AB;struct Cyc_Absyn_Stmt*
_tmp1AC;struct Cyc_Absyn_Stmt*_tmp1AD;struct _tuple2 _tmp1AE;struct Cyc_Absyn_Exp*
_tmp1AF;struct Cyc_List_List*_tmp1B0;struct Cyc_List_List*_tmp1B1;struct Cyc_Absyn_Stmt*
_tmp1B2;struct Cyc_List_List*_tmp1B3;struct Cyc_Absyn_Tvar*_tmp1B4;struct Cyc_Absyn_Vardecl*
_tmp1B5;int _tmp1B6;struct Cyc_Absyn_Exp*_tmp1B7;struct Cyc_Absyn_Stmt*_tmp1B8;
struct Cyc_Absyn_Exp*_tmp1B9;struct Cyc_Absyn_Exp*_tmp1BA;struct Cyc_Absyn_Tvar*
_tmp1BB;struct Cyc_Absyn_Vardecl*_tmp1BC;struct Cyc_Absyn_Stmt*_tmp1BD;_LL153: if((
int)_tmp194 != 0)goto _LL155;_LL154: Cyc_Absyndump_dump_semi();goto _LL152;_LL155:
if(_tmp194 <= (void*)1)goto _LL157;if(*((int*)_tmp194)!= 0)goto _LL157;_tmp195=((
struct Cyc_Absyn_Exp_s_struct*)_tmp194)->f1;_LL156: Cyc_Absyndump_dumploc(s->loc);
Cyc_Absyndump_dumpexp(_tmp195);Cyc_Absyndump_dump_semi();goto _LL152;_LL157: if(
_tmp194 <= (void*)1)goto _LL159;if(*((int*)_tmp194)!= 1)goto _LL159;_tmp196=((
struct Cyc_Absyn_Seq_s_struct*)_tmp194)->f1;_tmp197=((struct Cyc_Absyn_Seq_s_struct*)
_tmp194)->f2;_LL158: if(Cyc_Absynpp_is_declaration(_tmp196)){Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpstmt(_tmp196);Cyc_Absyndump_dump_char((int)'}');}
else{Cyc_Absyndump_dumpstmt(_tmp196);}if(Cyc_Absynpp_is_declaration(_tmp197)){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp197);Cyc_Absyndump_dump_char((
int)'}');}else{Cyc_Absyndump_dumpstmt(_tmp197);}goto _LL152;_LL159: if(_tmp194 <= (
void*)1)goto _LL15B;if(*((int*)_tmp194)!= 2)goto _LL15B;_tmp198=((struct Cyc_Absyn_Return_s_struct*)
_tmp194)->f1;if(_tmp198 != 0)goto _LL15B;_LL15A: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({
const char*_tmp1BE="return;";_tag_dynforward(_tmp1BE,sizeof(char),
_get_zero_arr_size_char(_tmp1BE,8));}));goto _LL152;_LL15B: if(_tmp194 <= (void*)1)
goto _LL15D;if(*((int*)_tmp194)!= 2)goto _LL15D;_tmp199=((struct Cyc_Absyn_Return_s_struct*)
_tmp194)->f1;_LL15C: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*
_tmp1BF="return";_tag_dynforward(_tmp1BF,sizeof(char),_get_zero_arr_size_char(
_tmp1BF,7));}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp199));
Cyc_Absyndump_dump_semi();goto _LL152;_LL15D: if(_tmp194 <= (void*)1)goto _LL15F;if(*((
int*)_tmp194)!= 3)goto _LL15F;_tmp19A=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp194)->f1;_tmp19B=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp194)->f2;_tmp19C=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp194)->f3;_LL15E: Cyc_Absyndump_dumploc(s->loc);
Cyc_Absyndump_dump(({const char*_tmp1C0="if(";_tag_dynforward(_tmp1C0,sizeof(char),
_get_zero_arr_size_char(_tmp1C0,4));}));Cyc_Absyndump_dumpexp(_tmp19A);{void*
_tmp1C1=(void*)_tmp19B->r;_LL17E: if(_tmp1C1 <= (void*)1)goto _LL186;if(*((int*)
_tmp1C1)!= 1)goto _LL180;_LL17F: goto _LL181;_LL180: if(*((int*)_tmp1C1)!= 11)goto
_LL182;_LL181: goto _LL183;_LL182: if(*((int*)_tmp1C1)!= 3)goto _LL184;_LL183: goto
_LL185;_LL184: if(*((int*)_tmp1C1)!= 12)goto _LL186;_LL185: Cyc_Absyndump_dump_nospace(({
const char*_tmp1C2="){";_tag_dynforward(_tmp1C2,sizeof(char),
_get_zero_arr_size_char(_tmp1C2,3));}));Cyc_Absyndump_dumpstmt(_tmp19B);Cyc_Absyndump_dump_char((
int)'}');goto _LL17D;_LL186:;_LL187: Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpstmt(
_tmp19B);_LL17D:;}{void*_tmp1C3=(void*)_tmp19C->r;_LL189: if((int)_tmp1C3 != 0)
goto _LL18B;_LL18A: goto _LL188;_LL18B:;_LL18C: Cyc_Absyndump_dump(({const char*
_tmp1C4="else{";_tag_dynforward(_tmp1C4,sizeof(char),_get_zero_arr_size_char(
_tmp1C4,6));}));Cyc_Absyndump_dumpstmt(_tmp19C);Cyc_Absyndump_dump_char((int)'}');
goto _LL188;_LL188:;}goto _LL152;_LL15F: if(_tmp194 <= (void*)1)goto _LL161;if(*((int*)
_tmp194)!= 4)goto _LL161;_tmp19D=((struct Cyc_Absyn_While_s_struct*)_tmp194)->f1;
_tmp19E=_tmp19D.f1;_tmp19F=((struct Cyc_Absyn_While_s_struct*)_tmp194)->f2;_LL160:
Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*_tmp1C5="while(";
_tag_dynforward(_tmp1C5,sizeof(char),_get_zero_arr_size_char(_tmp1C5,7));}));Cyc_Absyndump_dumpexp(
_tmp19E);Cyc_Absyndump_dump_nospace(({const char*_tmp1C6="){";_tag_dynforward(
_tmp1C6,sizeof(char),_get_zero_arr_size_char(_tmp1C6,3));}));Cyc_Absyndump_dumpstmt(
_tmp19F);Cyc_Absyndump_dump_char((int)'}');goto _LL152;_LL161: if(_tmp194 <= (void*)
1)goto _LL163;if(*((int*)_tmp194)!= 5)goto _LL163;_LL162: Cyc_Absyndump_dumploc(s->loc);
Cyc_Absyndump_dump(({const char*_tmp1C7="break;";_tag_dynforward(_tmp1C7,sizeof(
char),_get_zero_arr_size_char(_tmp1C7,7));}));goto _LL152;_LL163: if(_tmp194 <= (
void*)1)goto _LL165;if(*((int*)_tmp194)!= 6)goto _LL165;_LL164: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp1C8="continue;";_tag_dynforward(
_tmp1C8,sizeof(char),_get_zero_arr_size_char(_tmp1C8,10));}));goto _LL152;_LL165:
if(_tmp194 <= (void*)1)goto _LL167;if(*((int*)_tmp194)!= 7)goto _LL167;_tmp1A0=((
struct Cyc_Absyn_Goto_s_struct*)_tmp194)->f1;_LL166: Cyc_Absyndump_dumploc(s->loc);
Cyc_Absyndump_dump(({const char*_tmp1C9="goto";_tag_dynforward(_tmp1C9,sizeof(
char),_get_zero_arr_size_char(_tmp1C9,5));}));Cyc_Absyndump_dump_str(_tmp1A0);
Cyc_Absyndump_dump_semi();goto _LL152;_LL167: if(_tmp194 <= (void*)1)goto _LL169;if(*((
int*)_tmp194)!= 8)goto _LL169;_tmp1A1=((struct Cyc_Absyn_For_s_struct*)_tmp194)->f1;
_tmp1A2=((struct Cyc_Absyn_For_s_struct*)_tmp194)->f2;_tmp1A3=_tmp1A2.f1;_tmp1A4=((
struct Cyc_Absyn_For_s_struct*)_tmp194)->f3;_tmp1A5=_tmp1A4.f1;_tmp1A6=((struct
Cyc_Absyn_For_s_struct*)_tmp194)->f4;_LL168: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({
const char*_tmp1CA="for(";_tag_dynforward(_tmp1CA,sizeof(char),
_get_zero_arr_size_char(_tmp1CA,5));}));Cyc_Absyndump_dumpexp(_tmp1A1);Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpexp(_tmp1A3);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(
_tmp1A5);Cyc_Absyndump_dump_nospace(({const char*_tmp1CB="){";_tag_dynforward(
_tmp1CB,sizeof(char),_get_zero_arr_size_char(_tmp1CB,3));}));Cyc_Absyndump_dumpstmt(
_tmp1A6);Cyc_Absyndump_dump_char((int)'}');goto _LL152;_LL169: if(_tmp194 <= (void*)
1)goto _LL16B;if(*((int*)_tmp194)!= 9)goto _LL16B;_tmp1A7=((struct Cyc_Absyn_Switch_s_struct*)
_tmp194)->f1;_tmp1A8=((struct Cyc_Absyn_Switch_s_struct*)_tmp194)->f2;_LL16A: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp1CC="switch(";_tag_dynforward(_tmp1CC,
sizeof(char),_get_zero_arr_size_char(_tmp1CC,8));}));Cyc_Absyndump_dumpexp(
_tmp1A7);Cyc_Absyndump_dump_nospace(({const char*_tmp1CD="){";_tag_dynforward(
_tmp1CD,sizeof(char),_get_zero_arr_size_char(_tmp1CD,3));}));Cyc_Absyndump_dumpswitchclauses(
_tmp1A8);Cyc_Absyndump_dump_char((int)'}');goto _LL152;_LL16B: if(_tmp194 <= (void*)
1)goto _LL16D;if(*((int*)_tmp194)!= 11)goto _LL16D;_tmp1A9=((struct Cyc_Absyn_Decl_s_struct*)
_tmp194)->f1;_tmp1AA=((struct Cyc_Absyn_Decl_s_struct*)_tmp194)->f2;_LL16C: Cyc_Absyndump_dumpdecl(
_tmp1A9);Cyc_Absyndump_dumpstmt(_tmp1AA);goto _LL152;_LL16D: if(_tmp194 <= (void*)1)
goto _LL16F;if(*((int*)_tmp194)!= 12)goto _LL16F;_tmp1AB=((struct Cyc_Absyn_Label_s_struct*)
_tmp194)->f1;_tmp1AC=((struct Cyc_Absyn_Label_s_struct*)_tmp194)->f2;_LL16E: if(
Cyc_Absynpp_is_declaration(_tmp1AC)){Cyc_Absyndump_dump_str(_tmp1AB);Cyc_Absyndump_dump_nospace(({
const char*_tmp1CE=": {";_tag_dynforward(_tmp1CE,sizeof(char),
_get_zero_arr_size_char(_tmp1CE,4));}));Cyc_Absyndump_dumpstmt(_tmp1AC);Cyc_Absyndump_dump_char((
int)'}');}else{Cyc_Absyndump_dump_str(_tmp1AB);Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpstmt(_tmp1AC);}goto _LL152;_LL16F: if(_tmp194 <= (void*)1)goto
_LL171;if(*((int*)_tmp194)!= 13)goto _LL171;_tmp1AD=((struct Cyc_Absyn_Do_s_struct*)
_tmp194)->f1;_tmp1AE=((struct Cyc_Absyn_Do_s_struct*)_tmp194)->f2;_tmp1AF=_tmp1AE.f1;
_LL170: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*_tmp1CF="do{";
_tag_dynforward(_tmp1CF,sizeof(char),_get_zero_arr_size_char(_tmp1CF,4));}));Cyc_Absyndump_dumpstmt(
_tmp1AD);Cyc_Absyndump_dump_nospace(({const char*_tmp1D0="}while(";
_tag_dynforward(_tmp1D0,sizeof(char),_get_zero_arr_size_char(_tmp1D0,8));}));Cyc_Absyndump_dumpexp(
_tmp1AF);Cyc_Absyndump_dump_nospace(({const char*_tmp1D1=");";_tag_dynforward(
_tmp1D1,sizeof(char),_get_zero_arr_size_char(_tmp1D1,3));}));goto _LL152;_LL171:
if(_tmp194 <= (void*)1)goto _LL173;if(*((int*)_tmp194)!= 10)goto _LL173;_tmp1B0=((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp194)->f1;if(_tmp1B0 != 0)goto _LL173;_LL172:
Cyc_Absyndump_dump(({const char*_tmp1D2="fallthru;";_tag_dynforward(_tmp1D2,
sizeof(char),_get_zero_arr_size_char(_tmp1D2,10));}));goto _LL152;_LL173: if(
_tmp194 <= (void*)1)goto _LL175;if(*((int*)_tmp194)!= 10)goto _LL175;_tmp1B1=((
struct Cyc_Absyn_Fallthru_s_struct*)_tmp194)->f1;_LL174: Cyc_Absyndump_dump(({
const char*_tmp1D3="fallthru(";_tag_dynforward(_tmp1D3,sizeof(char),
_get_zero_arr_size_char(_tmp1D3,10));}));Cyc_Absyndump_dumpexps_prec(20,_tmp1B1);
Cyc_Absyndump_dump_nospace(({const char*_tmp1D4=");";_tag_dynforward(_tmp1D4,
sizeof(char),_get_zero_arr_size_char(_tmp1D4,3));}));goto _LL152;_LL175: if(
_tmp194 <= (void*)1)goto _LL177;if(*((int*)_tmp194)!= 14)goto _LL177;_tmp1B2=((
struct Cyc_Absyn_TryCatch_s_struct*)_tmp194)->f1;_tmp1B3=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp194)->f2;_LL176: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*
_tmp1D5="try";_tag_dynforward(_tmp1D5,sizeof(char),_get_zero_arr_size_char(
_tmp1D5,4));}));Cyc_Absyndump_dumpstmt(_tmp1B2);Cyc_Absyndump_dump(({const char*
_tmp1D6="catch{";_tag_dynforward(_tmp1D6,sizeof(char),_get_zero_arr_size_char(
_tmp1D6,7));}));Cyc_Absyndump_dumpswitchclauses(_tmp1B3);Cyc_Absyndump_dump_char((
int)'}');goto _LL152;_LL177: if(_tmp194 <= (void*)1)goto _LL179;if(*((int*)_tmp194)
!= 15)goto _LL179;_tmp1B4=((struct Cyc_Absyn_Region_s_struct*)_tmp194)->f1;_tmp1B5=((
struct Cyc_Absyn_Region_s_struct*)_tmp194)->f2;_tmp1B6=((struct Cyc_Absyn_Region_s_struct*)
_tmp194)->f3;_tmp1B7=((struct Cyc_Absyn_Region_s_struct*)_tmp194)->f4;_tmp1B8=((
struct Cyc_Absyn_Region_s_struct*)_tmp194)->f5;_LL178: Cyc_Absyndump_dumploc(s->loc);
Cyc_Absyndump_dump(({const char*_tmp1D7="region";_tag_dynforward(_tmp1D7,sizeof(
char),_get_zero_arr_size_char(_tmp1D7,7));}));if(_tmp1B6)Cyc_Absyndump_dump(({
const char*_tmp1D8="[resetable]";_tag_dynforward(_tmp1D8,sizeof(char),
_get_zero_arr_size_char(_tmp1D8,12));}));Cyc_Absyndump_dump(({const char*_tmp1D9="<";
_tag_dynforward(_tmp1D9,sizeof(char),_get_zero_arr_size_char(_tmp1D9,2));}));Cyc_Absyndump_dumptvar(
_tmp1B4);Cyc_Absyndump_dump(({const char*_tmp1DA="> ";_tag_dynforward(_tmp1DA,
sizeof(char),_get_zero_arr_size_char(_tmp1DA,3));}));Cyc_Absyndump_dumpqvar(
_tmp1B5->name);if(_tmp1B7 != 0){Cyc_Absyndump_dump(({const char*_tmp1DB=" = open(";
_tag_dynforward(_tmp1DB,sizeof(char),_get_zero_arr_size_char(_tmp1DB,9));}));Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)_tmp1B7);Cyc_Absyndump_dump(({const char*_tmp1DC=")";
_tag_dynforward(_tmp1DC,sizeof(char),_get_zero_arr_size_char(_tmp1DC,2));}));}
Cyc_Absyndump_dump(({const char*_tmp1DD="{";_tag_dynforward(_tmp1DD,sizeof(char),
_get_zero_arr_size_char(_tmp1DD,2));}));Cyc_Absyndump_dumpstmt(_tmp1B8);Cyc_Absyndump_dump(({
const char*_tmp1DE="}";_tag_dynforward(_tmp1DE,sizeof(char),
_get_zero_arr_size_char(_tmp1DE,2));}));goto _LL152;_LL179: if(_tmp194 <= (void*)1)
goto _LL17B;if(*((int*)_tmp194)!= 16)goto _LL17B;_tmp1B9=((struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp194)->f1;_LL17A: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*
_tmp1DF="reset_region(";_tag_dynforward(_tmp1DF,sizeof(char),
_get_zero_arr_size_char(_tmp1DF,14));}));Cyc_Absyndump_dumpexp(_tmp1B9);Cyc_Absyndump_dump(({
const char*_tmp1E0=");";_tag_dynforward(_tmp1E0,sizeof(char),
_get_zero_arr_size_char(_tmp1E0,3));}));goto _LL152;_LL17B: if(_tmp194 <= (void*)1)
goto _LL152;if(*((int*)_tmp194)!= 17)goto _LL152;_tmp1BA=((struct Cyc_Absyn_Alias_s_struct*)
_tmp194)->f1;_tmp1BB=((struct Cyc_Absyn_Alias_s_struct*)_tmp194)->f2;_tmp1BC=((
struct Cyc_Absyn_Alias_s_struct*)_tmp194)->f3;_tmp1BD=((struct Cyc_Absyn_Alias_s_struct*)
_tmp194)->f4;_LL17C: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*
_tmp1E1="alias(";_tag_dynforward(_tmp1E1,sizeof(char),_get_zero_arr_size_char(
_tmp1E1,7));}));Cyc_Absyndump_dumpexp(_tmp1BA);Cyc_Absyndump_dump(({const char*
_tmp1E2=") = ";_tag_dynforward(_tmp1E2,sizeof(char),_get_zero_arr_size_char(
_tmp1E2,5));}));Cyc_Absyndump_dump(({const char*_tmp1E3="<";_tag_dynforward(
_tmp1E3,sizeof(char),_get_zero_arr_size_char(_tmp1E3,2));}));Cyc_Absyndump_dumptvar(
_tmp1BB);Cyc_Absyndump_dump(({const char*_tmp1E4=">";_tag_dynforward(_tmp1E4,
sizeof(char),_get_zero_arr_size_char(_tmp1E4,2));}));Cyc_Absyndump_dumpqvar(
_tmp1BC->name);Cyc_Absyndump_dump(({const char*_tmp1E5="{";_tag_dynforward(
_tmp1E5,sizeof(char),_get_zero_arr_size_char(_tmp1E5,2));}));Cyc_Absyndump_dumpstmt(
_tmp1BD);Cyc_Absyndump_dump(({const char*_tmp1E6="}";_tag_dynforward(_tmp1E6,
sizeof(char),_get_zero_arr_size_char(_tmp1E6,2));}));goto _LL152;_LL152:;}struct
_tuple11{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};void Cyc_Absyndump_dumpdp(
struct _tuple11*dp){Cyc_Absyndump_egroup(Cyc_Absyndump_dumpdesignator,(*dp).f1,({
const char*_tmp1E7="";_tag_dynforward(_tmp1E7,sizeof(char),
_get_zero_arr_size_char(_tmp1E7,1));}),({const char*_tmp1E8="=";_tag_dynforward(
_tmp1E8,sizeof(char),_get_zero_arr_size_char(_tmp1E8,2));}),({const char*_tmp1E9="=";
_tag_dynforward(_tmp1E9,sizeof(char),_get_zero_arr_size_char(_tmp1E9,2));}));Cyc_Absyndump_dumppat((*
dp).f2);}void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){void*_tmp1EA=(void*)p->r;
void*_tmp1EB;int _tmp1EC;void*_tmp1ED;int _tmp1EE;void*_tmp1EF;int _tmp1F0;char
_tmp1F1;struct _dynforward_ptr _tmp1F2;struct Cyc_Absyn_Vardecl*_tmp1F3;struct Cyc_Absyn_Pat*
_tmp1F4;struct Cyc_Absyn_Pat _tmp1F5;void*_tmp1F6;struct Cyc_Absyn_Vardecl*_tmp1F7;
struct Cyc_Absyn_Pat*_tmp1F8;struct Cyc_List_List*_tmp1F9;int _tmp1FA;struct Cyc_Absyn_Pat*
_tmp1FB;struct Cyc_Absyn_Vardecl*_tmp1FC;struct Cyc_Absyn_Pat*_tmp1FD;struct Cyc_Absyn_Pat
_tmp1FE;void*_tmp1FF;struct Cyc_Absyn_Vardecl*_tmp200;struct Cyc_Absyn_Pat*_tmp201;
struct Cyc_Absyn_Tvar*_tmp202;struct Cyc_Absyn_Vardecl*_tmp203;struct _tuple0*
_tmp204;struct _tuple0*_tmp205;struct Cyc_List_List*_tmp206;int _tmp207;struct Cyc_Absyn_AggrInfo
_tmp208;union Cyc_Absyn_AggrInfoU_union _tmp209;struct Cyc_List_List*_tmp20A;struct
Cyc_List_List*_tmp20B;int _tmp20C;struct Cyc_Absyn_Tunionfield*_tmp20D;struct Cyc_List_List*
_tmp20E;int _tmp20F;struct Cyc_Absyn_Enumfield*_tmp210;struct Cyc_Absyn_Enumfield*
_tmp211;struct Cyc_Absyn_Exp*_tmp212;_LL18E: if((int)_tmp1EA != 0)goto _LL190;_LL18F:
Cyc_Absyndump_dump_char((int)'_');goto _LL18D;_LL190: if((int)_tmp1EA != 1)goto
_LL192;_LL191: Cyc_Absyndump_dump(({const char*_tmp213="NULL";_tag_dynforward(
_tmp213,sizeof(char),_get_zero_arr_size_char(_tmp213,5));}));goto _LL18D;_LL192:
if(_tmp1EA <= (void*)2)goto _LL194;if(*((int*)_tmp1EA)!= 7)goto _LL194;_tmp1EB=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp1EA)->f1;if((int)_tmp1EB != 2)goto
_LL194;_tmp1EC=((struct Cyc_Absyn_Int_p_struct*)_tmp1EA)->f2;_LL193: _tmp1EE=
_tmp1EC;goto _LL195;_LL194: if(_tmp1EA <= (void*)2)goto _LL196;if(*((int*)_tmp1EA)!= 
7)goto _LL196;_tmp1ED=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp1EA)->f1;if((int)
_tmp1ED != 0)goto _LL196;_tmp1EE=((struct Cyc_Absyn_Int_p_struct*)_tmp1EA)->f2;
_LL195: Cyc_Absyndump_dump((struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmp216;_tmp216.tag=1;_tmp216.f1=(unsigned long)_tmp1EE;{void*_tmp214[1]={&
_tmp216};Cyc_aprintf(({const char*_tmp215="%d";_tag_dynforward(_tmp215,sizeof(
char),_get_zero_arr_size_char(_tmp215,3));}),_tag_dynforward(_tmp214,sizeof(void*),
1));}}));goto _LL18D;_LL196: if(_tmp1EA <= (void*)2)goto _LL198;if(*((int*)_tmp1EA)
!= 7)goto _LL198;_tmp1EF=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp1EA)->f1;if((
int)_tmp1EF != 1)goto _LL198;_tmp1F0=((struct Cyc_Absyn_Int_p_struct*)_tmp1EA)->f2;
_LL197: Cyc_Absyndump_dump((struct _dynforward_ptr)({struct Cyc_Int_pa_struct
_tmp219;_tmp219.tag=1;_tmp219.f1=(unsigned int)_tmp1F0;{void*_tmp217[1]={&
_tmp219};Cyc_aprintf(({const char*_tmp218="%u";_tag_dynforward(_tmp218,sizeof(
char),_get_zero_arr_size_char(_tmp218,3));}),_tag_dynforward(_tmp217,sizeof(void*),
1));}}));goto _LL18D;_LL198: if(_tmp1EA <= (void*)2)goto _LL19A;if(*((int*)_tmp1EA)
!= 8)goto _LL19A;_tmp1F1=((struct Cyc_Absyn_Char_p_struct*)_tmp1EA)->f1;_LL199: Cyc_Absyndump_dump(({
const char*_tmp21A="'";_tag_dynforward(_tmp21A,sizeof(char),
_get_zero_arr_size_char(_tmp21A,2));}));Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(
_tmp1F1));Cyc_Absyndump_dump_nospace(({const char*_tmp21B="'";_tag_dynforward(
_tmp21B,sizeof(char),_get_zero_arr_size_char(_tmp21B,2));}));goto _LL18D;_LL19A:
if(_tmp1EA <= (void*)2)goto _LL19C;if(*((int*)_tmp1EA)!= 9)goto _LL19C;_tmp1F2=((
struct Cyc_Absyn_Float_p_struct*)_tmp1EA)->f1;_LL19B: Cyc_Absyndump_dump(_tmp1F2);
goto _LL18D;_LL19C: if(_tmp1EA <= (void*)2)goto _LL19E;if(*((int*)_tmp1EA)!= 0)goto
_LL19E;_tmp1F3=((struct Cyc_Absyn_Var_p_struct*)_tmp1EA)->f1;_tmp1F4=((struct Cyc_Absyn_Var_p_struct*)
_tmp1EA)->f2;_tmp1F5=*_tmp1F4;_tmp1F6=(void*)_tmp1F5.r;if((int)_tmp1F6 != 0)goto
_LL19E;_LL19D: Cyc_Absyndump_dumpqvar(_tmp1F3->name);goto _LL18D;_LL19E: if(_tmp1EA
<= (void*)2)goto _LL1A0;if(*((int*)_tmp1EA)!= 0)goto _LL1A0;_tmp1F7=((struct Cyc_Absyn_Var_p_struct*)
_tmp1EA)->f1;_tmp1F8=((struct Cyc_Absyn_Var_p_struct*)_tmp1EA)->f2;_LL19F: Cyc_Absyndump_dumpqvar(
_tmp1F7->name);Cyc_Absyndump_dump(({const char*_tmp21C=" as ";_tag_dynforward(
_tmp21C,sizeof(char),_get_zero_arr_size_char(_tmp21C,5));}));Cyc_Absyndump_dumppat(
_tmp1F8);goto _LL18D;_LL1A0: if(_tmp1EA <= (void*)2)goto _LL1A2;if(*((int*)_tmp1EA)
!= 3)goto _LL1A2;_tmp1F9=((struct Cyc_Absyn_Tuple_p_struct*)_tmp1EA)->f1;_tmp1FA=((
struct Cyc_Absyn_Tuple_p_struct*)_tmp1EA)->f2;_LL1A1: {struct _dynforward_ptr term=
_tmp1FA?({const char*_tmp21F=", ...)";_tag_dynforward(_tmp21F,sizeof(char),
_get_zero_arr_size_char(_tmp21F,7));}):({const char*_tmp220=")";_tag_dynforward(
_tmp220,sizeof(char),_get_zero_arr_size_char(_tmp220,2));});((void(*)(void(*f)(
struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dynforward_ptr start,struct
_dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,
_tmp1F9,({const char*_tmp21D="$(";_tag_dynforward(_tmp21D,sizeof(char),
_get_zero_arr_size_char(_tmp21D,3));}),term,({const char*_tmp21E=",";
_tag_dynforward(_tmp21E,sizeof(char),_get_zero_arr_size_char(_tmp21E,2));}));
goto _LL18D;}_LL1A2: if(_tmp1EA <= (void*)2)goto _LL1A4;if(*((int*)_tmp1EA)!= 4)goto
_LL1A4;_tmp1FB=((struct Cyc_Absyn_Pointer_p_struct*)_tmp1EA)->f1;_LL1A3: Cyc_Absyndump_dump(({
const char*_tmp221="&";_tag_dynforward(_tmp221,sizeof(char),
_get_zero_arr_size_char(_tmp221,2));}));Cyc_Absyndump_dumppat(_tmp1FB);goto
_LL18D;_LL1A4: if(_tmp1EA <= (void*)2)goto _LL1A6;if(*((int*)_tmp1EA)!= 1)goto
_LL1A6;_tmp1FC=((struct Cyc_Absyn_Reference_p_struct*)_tmp1EA)->f1;_tmp1FD=((
struct Cyc_Absyn_Reference_p_struct*)_tmp1EA)->f2;_tmp1FE=*_tmp1FD;_tmp1FF=(void*)
_tmp1FE.r;if((int)_tmp1FF != 0)goto _LL1A6;_LL1A5: Cyc_Absyndump_dump(({const char*
_tmp222="*";_tag_dynforward(_tmp222,sizeof(char),_get_zero_arr_size_char(_tmp222,
2));}));Cyc_Absyndump_dumpqvar(_tmp1FC->name);goto _LL18D;_LL1A6: if(_tmp1EA <= (
void*)2)goto _LL1A8;if(*((int*)_tmp1EA)!= 1)goto _LL1A8;_tmp200=((struct Cyc_Absyn_Reference_p_struct*)
_tmp1EA)->f1;_tmp201=((struct Cyc_Absyn_Reference_p_struct*)_tmp1EA)->f2;_LL1A7:
Cyc_Absyndump_dump(({const char*_tmp223="*";_tag_dynforward(_tmp223,sizeof(char),
_get_zero_arr_size_char(_tmp223,2));}));Cyc_Absyndump_dumpqvar(_tmp200->name);
Cyc_Absyndump_dump(({const char*_tmp224=" as ";_tag_dynforward(_tmp224,sizeof(
char),_get_zero_arr_size_char(_tmp224,5));}));Cyc_Absyndump_dumppat(_tmp201);
goto _LL18D;_LL1A8: if(_tmp1EA <= (void*)2)goto _LL1AA;if(*((int*)_tmp1EA)!= 2)goto
_LL1AA;_tmp202=((struct Cyc_Absyn_TagInt_p_struct*)_tmp1EA)->f1;_tmp203=((struct
Cyc_Absyn_TagInt_p_struct*)_tmp1EA)->f2;_LL1A9: Cyc_Absyndump_dumpqvar(_tmp203->name);
Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumptvar(_tmp202);Cyc_Absyndump_dump_char((
int)'>');goto _LL18D;_LL1AA: if(_tmp1EA <= (void*)2)goto _LL1AC;if(*((int*)_tmp1EA)
!= 12)goto _LL1AC;_tmp204=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp1EA)->f1;
_LL1AB: Cyc_Absyndump_dumpqvar(_tmp204);goto _LL18D;_LL1AC: if(_tmp1EA <= (void*)2)
goto _LL1AE;if(*((int*)_tmp1EA)!= 13)goto _LL1AE;_tmp205=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp1EA)->f1;_tmp206=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp1EA)->f2;
_tmp207=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp1EA)->f3;_LL1AD: {struct
_dynforward_ptr term=_tmp207?({const char*_tmp227=", ...)";_tag_dynforward(_tmp227,
sizeof(char),_get_zero_arr_size_char(_tmp227,7));}):({const char*_tmp228=")";
_tag_dynforward(_tmp228,sizeof(char),_get_zero_arr_size_char(_tmp228,2));});Cyc_Absyndump_dumpqvar(
_tmp205);((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct
_dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat,_tmp206,({const char*_tmp225="(";_tag_dynforward(_tmp225,
sizeof(char),_get_zero_arr_size_char(_tmp225,2));}),term,({const char*_tmp226=",";
_tag_dynforward(_tmp226,sizeof(char),_get_zero_arr_size_char(_tmp226,2));}));
goto _LL18D;}_LL1AE: if(_tmp1EA <= (void*)2)goto _LL1B0;if(*((int*)_tmp1EA)!= 5)goto
_LL1B0;_tmp208=((struct Cyc_Absyn_Aggr_p_struct*)_tmp1EA)->f1;_tmp209=_tmp208.aggr_info;
_tmp20A=((struct Cyc_Absyn_Aggr_p_struct*)_tmp1EA)->f2;_tmp20B=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp1EA)->f3;_tmp20C=((struct Cyc_Absyn_Aggr_p_struct*)_tmp1EA)->f4;_LL1AF: {
struct _tuple0*_tmp22A;struct _tuple3 _tmp229=Cyc_Absyn_aggr_kinded_name(_tmp209);
_tmp22A=_tmp229.f2;{struct _dynforward_ptr term=_tmp20C?({const char*_tmp230=", ...)";
_tag_dynforward(_tmp230,sizeof(char),_get_zero_arr_size_char(_tmp230,7));}):({
const char*_tmp231=")";_tag_dynforward(_tmp231,sizeof(char),
_get_zero_arr_size_char(_tmp231,2));});Cyc_Absyndump_dumpqvar(_tmp22A);Cyc_Absyndump_dump_char((
int)'{');((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct
_dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumptvar,_tmp20A,({const char*_tmp22B="[";_tag_dynforward(_tmp22B,
sizeof(char),_get_zero_arr_size_char(_tmp22B,2));}),({const char*_tmp22C="]";
_tag_dynforward(_tmp22C,sizeof(char),_get_zero_arr_size_char(_tmp22C,2));}),({
const char*_tmp22D=",";_tag_dynforward(_tmp22D,sizeof(char),
_get_zero_arr_size_char(_tmp22D,2));}));((void(*)(void(*f)(struct _tuple11*),
struct Cyc_List_List*l,struct _dynforward_ptr start,struct _dynforward_ptr end,struct
_dynforward_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp20B,({const
char*_tmp22E="";_tag_dynforward(_tmp22E,sizeof(char),_get_zero_arr_size_char(
_tmp22E,1));}),term,({const char*_tmp22F=",";_tag_dynforward(_tmp22F,sizeof(char),
_get_zero_arr_size_char(_tmp22F,2));}));goto _LL18D;}}_LL1B0: if(_tmp1EA <= (void*)
2)goto _LL1B2;if(*((int*)_tmp1EA)!= 6)goto _LL1B2;_tmp20D=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp1EA)->f2;_tmp20E=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1EA)->f3;_tmp20F=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp1EA)->f4;_LL1B1: {struct _dynforward_ptr term=
_tmp20F?({const char*_tmp234=", ...)";_tag_dynforward(_tmp234,sizeof(char),
_get_zero_arr_size_char(_tmp234,7));}):({const char*_tmp235=")";_tag_dynforward(
_tmp235,sizeof(char),_get_zero_arr_size_char(_tmp235,2));});Cyc_Absyndump_dumpqvar(
_tmp20D->name);if(_tmp20E != 0)((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
l,struct _dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))
Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp20E,({const char*_tmp232="(";
_tag_dynforward(_tmp232,sizeof(char),_get_zero_arr_size_char(_tmp232,2));}),term,({
const char*_tmp233=",";_tag_dynforward(_tmp233,sizeof(char),
_get_zero_arr_size_char(_tmp233,2));}));goto _LL18D;}_LL1B2: if(_tmp1EA <= (void*)2)
goto _LL1B4;if(*((int*)_tmp1EA)!= 10)goto _LL1B4;_tmp210=((struct Cyc_Absyn_Enum_p_struct*)
_tmp1EA)->f2;_LL1B3: _tmp211=_tmp210;goto _LL1B5;_LL1B4: if(_tmp1EA <= (void*)2)goto
_LL1B6;if(*((int*)_tmp1EA)!= 11)goto _LL1B6;_tmp211=((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp1EA)->f2;_LL1B5: Cyc_Absyndump_dumpqvar(_tmp211->name);goto _LL18D;_LL1B6: if(
_tmp1EA <= (void*)2)goto _LL18D;if(*((int*)_tmp1EA)!= 14)goto _LL18D;_tmp212=((
struct Cyc_Absyn_Exp_p_struct*)_tmp1EA)->f1;_LL1B7: Cyc_Absyndump_dumpexp(_tmp212);
goto _LL18D;_LL18D:;}void Cyc_Absyndump_dumptunionfield(struct Cyc_Absyn_Tunionfield*
ef){Cyc_Absyndump_dumpqvar(ef->name);if(ef->typs != 0)Cyc_Absyndump_dumpargs(ef->typs);}
void Cyc_Absyndump_dumptunionfields(struct Cyc_List_List*fields){((void(*)(void(*f)(
struct Cyc_Absyn_Tunionfield*),struct Cyc_List_List*l,struct _dynforward_ptr sep))
Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumptunionfield,fields,({const char*_tmp236=",";
_tag_dynforward(_tmp236,sizeof(char),_get_zero_arr_size_char(_tmp236,2));}));}
void Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield*ef){Cyc_Absyndump_dumpqvar(
ef->name);if(ef->tag != 0){Cyc_Absyndump_dump(({const char*_tmp237=" = ";
_tag_dynforward(_tmp237,sizeof(char),_get_zero_arr_size_char(_tmp237,4));}));Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}void Cyc_Absyndump_dumpenumfields(
struct Cyc_List_List*fields){((void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),
struct Cyc_List_List*l,struct _dynforward_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,
fields,({const char*_tmp238=",";_tag_dynforward(_tmp238,sizeof(char),
_get_zero_arr_size_char(_tmp238,2));}));}void Cyc_Absyndump_dumpaggrfields(struct
Cyc_List_List*fields){for(0;fields != 0;fields=fields->tl){struct Cyc_Absyn_Aggrfield
_tmp23A;struct _dynforward_ptr*_tmp23B;struct Cyc_Absyn_Tqual _tmp23C;void*_tmp23D;
struct Cyc_Absyn_Exp*_tmp23E;struct Cyc_List_List*_tmp23F;struct Cyc_Absyn_Aggrfield*
_tmp239=(struct Cyc_Absyn_Aggrfield*)fields->hd;_tmp23A=*_tmp239;_tmp23B=_tmp23A.name;
_tmp23C=_tmp23A.tq;_tmp23D=(void*)_tmp23A.type;_tmp23E=_tmp23A.width;_tmp23F=
_tmp23A.attributes;{void*_tmp240=Cyc_Cyclone_c_compiler;_LL1B9: if((int)_tmp240 != 
0)goto _LL1BB;_LL1BA:((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct
_dynforward_ptr*),struct _dynforward_ptr*))Cyc_Absyndump_dumptqtd)(_tmp23C,
_tmp23D,Cyc_Absyndump_dump_str,_tmp23B);Cyc_Absyndump_dumpatts(_tmp23F);goto
_LL1B8;_LL1BB: if((int)_tmp240 != 1)goto _LL1B8;_LL1BC: Cyc_Absyndump_dumpatts(
_tmp23F);((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dynforward_ptr*),
struct _dynforward_ptr*))Cyc_Absyndump_dumptqtd)(_tmp23C,_tmp23D,Cyc_Absyndump_dump_str,
_tmp23B);goto _LL1B8;_LL1B8:;}if(_tmp23E != 0){Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp23E);}Cyc_Absyndump_dump_semi();}}
void Cyc_Absyndump_dumptypedefname(struct Cyc_Absyn_Typedefdecl*td){Cyc_Absyndump_dumpqvar(
td->name);Cyc_Absyndump_dumptvars(td->tvs);}static void Cyc_Absyndump_dump_atts_qvar(
struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dumpatts(fd->attributes);Cyc_Absyndump_dumpqvar(
fd->name);}static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple3*pr){{void*
_tmp241=(*pr).f1;_LL1BE: if((int)_tmp241 != 8)goto _LL1C0;_LL1BF: goto _LL1BD;_LL1C0:
if((int)_tmp241 != 0)goto _LL1C2;_LL1C1: Cyc_Absyndump_dump(({const char*_tmp242="_stdcall";
_tag_dynforward(_tmp242,sizeof(char),_get_zero_arr_size_char(_tmp242,9));}));
goto _LL1BD;_LL1C2: if((int)_tmp241 != 1)goto _LL1C4;_LL1C3: Cyc_Absyndump_dump(({
const char*_tmp243="_cdecl";_tag_dynforward(_tmp243,sizeof(char),
_get_zero_arr_size_char(_tmp243,7));}));goto _LL1BD;_LL1C4: if((int)_tmp241 != 2)
goto _LL1C6;_LL1C5: Cyc_Absyndump_dump(({const char*_tmp244="_fastcall";
_tag_dynforward(_tmp244,sizeof(char),_get_zero_arr_size_char(_tmp244,10));}));
goto _LL1BD;_LL1C6:;_LL1C7: goto _LL1BD;_LL1BD:;}Cyc_Absyndump_dumpqvar((*pr).f2);}
static void Cyc_Absyndump_dump_callconv_fdqvar(struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dump_callconv(
fd->attributes);Cyc_Absyndump_dumpqvar(fd->name);}static void Cyc_Absyndump_dumpids(
struct Cyc_List_List*vds){for(0;vds != 0;vds=vds->tl){Cyc_Absyndump_dumpqvar(((
struct Cyc_Absyn_Vardecl*)vds->hd)->name);if(vds->tl != 0)Cyc_Absyndump_dump_char((
int)',');}}void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,struct Cyc_Position_Segment*
loc){struct Cyc_Absyn_Vardecl _tmp246;void*_tmp247;struct _tuple0*_tmp248;struct Cyc_Absyn_Tqual
_tmp249;void*_tmp24A;struct Cyc_Absyn_Exp*_tmp24B;struct Cyc_List_List*_tmp24C;
struct Cyc_Absyn_Vardecl*_tmp245=vd;_tmp246=*_tmp245;_tmp247=(void*)_tmp246.sc;
_tmp248=_tmp246.name;_tmp249=_tmp246.tq;_tmp24A=(void*)_tmp246.type;_tmp24B=
_tmp246.initializer;_tmp24C=_tmp246.attributes;Cyc_Absyndump_dumploc(loc);{void*
_tmp24D=Cyc_Cyclone_c_compiler;_LL1C9: if((int)_tmp24D != 0)goto _LL1CB;_LL1CA: if(
_tmp247 == (void*)3  && Cyc_Absyndump_qvar_to_Cids){void*_tmp24E=Cyc_Tcutil_compress(
_tmp24A);_LL1CE: if(_tmp24E <= (void*)4)goto _LL1D0;if(*((int*)_tmp24E)!= 8)goto
_LL1D0;_LL1CF: goto _LL1CD;_LL1D0:;_LL1D1: Cyc_Absyndump_dumpscope(_tmp247);_LL1CD:;}
else{Cyc_Absyndump_dumpscope(_tmp247);}((void(*)(struct Cyc_Absyn_Tqual,void*,
void(*f)(struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd)(_tmp249,_tmp24A,
Cyc_Absyndump_dumpqvar,_tmp248);Cyc_Absyndump_dumpatts(_tmp24C);goto _LL1C8;
_LL1CB: if((int)_tmp24D != 1)goto _LL1C8;_LL1CC: Cyc_Absyndump_dumpatts(_tmp24C);Cyc_Absyndump_dumpscope(
_tmp247);{struct _RegionHandle _tmp24F=_new_region("temp");struct _RegionHandle*
temp=& _tmp24F;_push_region(temp);{struct Cyc_Absyn_Tqual _tmp251;void*_tmp252;
struct Cyc_List_List*_tmp253;struct _tuple5 _tmp250=Cyc_Absynpp_to_tms(temp,_tmp249,
_tmp24A);_tmp251=_tmp250.f1;_tmp252=_tmp250.f2;_tmp253=_tmp250.f3;{void*
call_conv=(void*)8;{struct Cyc_List_List*tms2=_tmp253;for(0;tms2 != 0;tms2=tms2->tl){
void*_tmp254=(void*)tms2->hd;struct Cyc_List_List*_tmp255;_LL1D3: if(*((int*)
_tmp254)!= 5)goto _LL1D5;_tmp255=((struct Cyc_Absyn_Attributes_mod_struct*)_tmp254)->f2;
_LL1D4: for(0;_tmp255 != 0;_tmp255=_tmp255->tl){void*_tmp256=(void*)_tmp255->hd;
_LL1D8: if((int)_tmp256 != 0)goto _LL1DA;_LL1D9: call_conv=(void*)0;goto _LL1D7;
_LL1DA: if((int)_tmp256 != 1)goto _LL1DC;_LL1DB: call_conv=(void*)1;goto _LL1D7;
_LL1DC: if((int)_tmp256 != 2)goto _LL1DE;_LL1DD: call_conv=(void*)2;goto _LL1D7;
_LL1DE:;_LL1DF: goto _LL1D7;_LL1D7:;}goto _LL1D2;_LL1D5:;_LL1D6: goto _LL1D2;_LL1D2:;}}
Cyc_Absyndump_dumptq(_tmp251);Cyc_Absyndump_dumpntyp(_tmp252);{struct _tuple3
_tmp257=({struct _tuple3 _tmp258;_tmp258.f1=call_conv;_tmp258.f2=_tmp248;_tmp258;});((
void(*)(struct Cyc_List_List*tms,void(*f)(struct _tuple3*),struct _tuple3*a))Cyc_Absyndump_dumptms)(
Cyc_List_imp_rev(_tmp253),Cyc_Absyndump_dump_callconv_qvar,& _tmp257);}}};
_pop_region(temp);}goto _LL1C8;_LL1C8:;}if(_tmp24B != 0){Cyc_Absyndump_dump_char((
int)'=');Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp24B);}Cyc_Absyndump_dump_semi();}
struct _tuple12{struct Cyc_Position_Segment*f1;struct _tuple0*f2;int f3;};void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl*d){void*_tmp259=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp25A;
struct Cyc_Absyn_Fndecl*_tmp25B;struct Cyc_Absyn_Aggrdecl*_tmp25C;struct Cyc_Absyn_Tuniondecl*
_tmp25D;struct Cyc_Absyn_Tuniondecl _tmp25E;void*_tmp25F;struct _tuple0*_tmp260;
struct Cyc_List_List*_tmp261;struct Cyc_Core_Opt*_tmp262;int _tmp263;int _tmp264;
struct Cyc_Absyn_Enumdecl*_tmp265;struct Cyc_Absyn_Enumdecl _tmp266;void*_tmp267;
struct _tuple0*_tmp268;struct Cyc_Core_Opt*_tmp269;struct Cyc_Absyn_Pat*_tmp26A;
struct Cyc_Absyn_Exp*_tmp26B;struct Cyc_List_List*_tmp26C;struct Cyc_Absyn_Typedefdecl*
_tmp26D;struct _dynforward_ptr*_tmp26E;struct Cyc_List_List*_tmp26F;struct _tuple0*
_tmp270;struct Cyc_List_List*_tmp271;struct Cyc_List_List*_tmp272;struct Cyc_List_List*
_tmp273;struct Cyc_List_List*_tmp274;_LL1E1: if(_tmp259 <= (void*)2)goto _LL1F9;if(*((
int*)_tmp259)!= 0)goto _LL1E3;_tmp25A=((struct Cyc_Absyn_Var_d_struct*)_tmp259)->f1;
_LL1E2: Cyc_Absyndump_dumpvardecl(_tmp25A,d->loc);goto _LL1E0;_LL1E3: if(*((int*)
_tmp259)!= 1)goto _LL1E5;_tmp25B=((struct Cyc_Absyn_Fn_d_struct*)_tmp259)->f1;
_LL1E4: Cyc_Absyndump_dumploc(d->loc);{void*_tmp275=Cyc_Cyclone_c_compiler;_LL1FE:
if((int)_tmp275 != 1)goto _LL200;_LL1FF: Cyc_Absyndump_dumpatts(_tmp25B->attributes);
goto _LL1FD;_LL200: if((int)_tmp275 != 0)goto _LL1FD;_LL201: goto _LL1FD;_LL1FD:;}if(
_tmp25B->is_inline){void*_tmp276=Cyc_Cyclone_c_compiler;_LL203: if((int)_tmp276 != 
1)goto _LL205;_LL204: Cyc_Absyndump_dump(({const char*_tmp277="__inline";
_tag_dynforward(_tmp277,sizeof(char),_get_zero_arr_size_char(_tmp277,9));}));
goto _LL202;_LL205:;_LL206: Cyc_Absyndump_dump(({const char*_tmp278="inline";
_tag_dynforward(_tmp278,sizeof(char),_get_zero_arr_size_char(_tmp278,7));}));
goto _LL202;_LL202:;}Cyc_Absyndump_dumpscope((void*)_tmp25B->sc);{void*t=(void*)({
struct Cyc_Absyn_FnType_struct*_tmp27A=_cycalloc(sizeof(*_tmp27A));_tmp27A[0]=({
struct Cyc_Absyn_FnType_struct _tmp27B;_tmp27B.tag=8;_tmp27B.f1=({struct Cyc_Absyn_FnInfo
_tmp27C;_tmp27C.tvars=_tmp25B->tvs;_tmp27C.effect=_tmp25B->effect;_tmp27C.ret_typ=(
void*)((void*)_tmp25B->ret_type);_tmp27C.args=((struct Cyc_List_List*(*)(struct
_tuple1*(*f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,
_tmp25B->args);_tmp27C.c_varargs=_tmp25B->c_varargs;_tmp27C.cyc_varargs=_tmp25B->cyc_varargs;
_tmp27C.rgn_po=_tmp25B->rgn_po;_tmp27C.attributes=0;_tmp27C;});_tmp27B;});
_tmp27A;});{void*_tmp279=Cyc_Cyclone_c_compiler;_LL208: if((int)_tmp279 != 0)goto
_LL20A;_LL209:((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),
struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_atts_qvar,
_tmp25B);goto _LL207;_LL20A: if((int)_tmp279 != 1)goto _LL207;_LL20B:((void(*)(
struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))
Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_callconv_fdqvar,
_tmp25B);goto _LL207;_LL207:;}Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(
_tmp25B->body);Cyc_Absyndump_dump_char((int)'}');goto _LL1E0;}_LL1E5: if(*((int*)
_tmp259)!= 4)goto _LL1E7;_tmp25C=((struct Cyc_Absyn_Aggr_d_struct*)_tmp259)->f1;
_LL1E6: Cyc_Absyndump_dumpscope((void*)_tmp25C->sc);Cyc_Absyndump_dumpaggr_kind((
void*)_tmp25C->kind);Cyc_Absyndump_dumpqvar(_tmp25C->name);Cyc_Absyndump_dumptvars(
_tmp25C->tvs);if(_tmp25C->impl == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((
int)'{');if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp25C->impl))->exist_vars
!= 0)((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct
_dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpkindedtvar,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp25C->impl))->exist_vars,({
const char*_tmp27D="<";_tag_dynforward(_tmp27D,sizeof(char),
_get_zero_arr_size_char(_tmp27D,2));}),({const char*_tmp27E=">";_tag_dynforward(
_tmp27E,sizeof(char),_get_zero_arr_size_char(_tmp27E,2));}),({const char*_tmp27F=",";
_tag_dynforward(_tmp27F,sizeof(char),_get_zero_arr_size_char(_tmp27F,2));}));if(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp25C->impl))->rgn_po != 0){Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
_tmp25C->impl))->rgn_po);}Cyc_Absyndump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp25C->impl))->fields);Cyc_Absyndump_dump(({const char*_tmp280="}";
_tag_dynforward(_tmp280,sizeof(char),_get_zero_arr_size_char(_tmp280,2));}));Cyc_Absyndump_dumpatts(
_tmp25C->attributes);Cyc_Absyndump_dump(({const char*_tmp281=";";_tag_dynforward(
_tmp281,sizeof(char),_get_zero_arr_size_char(_tmp281,2));}));}goto _LL1E0;_LL1E7:
if(*((int*)_tmp259)!= 5)goto _LL1E9;_tmp25D=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp259)->f1;_tmp25E=*_tmp25D;_tmp25F=(void*)_tmp25E.sc;_tmp260=_tmp25E.name;
_tmp261=_tmp25E.tvs;_tmp262=_tmp25E.fields;_tmp263=_tmp25E.is_xtunion;_tmp264=
_tmp25E.is_flat;_LL1E8: Cyc_Absyndump_dumpscope(_tmp25F);Cyc_Absyndump_dump(
_tmp263?({const char*_tmp282="xtunion";_tag_dynforward(_tmp282,sizeof(char),
_get_zero_arr_size_char(_tmp282,8));}):({const char*_tmp283="tunion";
_tag_dynforward(_tmp283,sizeof(char),_get_zero_arr_size_char(_tmp283,7));}));if(
_tmp264)Cyc_Absyndump_dump_nospace(({const char*_tmp284=" __attribute__((flat))";
_tag_dynforward(_tmp284,sizeof(char),_get_zero_arr_size_char(_tmp284,23));}));
Cyc_Absyndump_dumpqvar(_tmp260);Cyc_Absyndump_dumptvars(_tmp261);if(_tmp262 == 0)
Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumptunionfields((
struct Cyc_List_List*)_tmp262->v);Cyc_Absyndump_dump_nospace(({const char*_tmp285="};";
_tag_dynforward(_tmp285,sizeof(char),_get_zero_arr_size_char(_tmp285,3));}));}
goto _LL1E0;_LL1E9: if(*((int*)_tmp259)!= 6)goto _LL1EB;_tmp265=((struct Cyc_Absyn_Enum_d_struct*)
_tmp259)->f1;_tmp266=*_tmp265;_tmp267=(void*)_tmp266.sc;_tmp268=_tmp266.name;
_tmp269=_tmp266.fields;_LL1EA: Cyc_Absyndump_dumpscope(_tmp267);Cyc_Absyndump_dump(({
const char*_tmp286="enum ";_tag_dynforward(_tmp286,sizeof(char),
_get_zero_arr_size_char(_tmp286,6));}));Cyc_Absyndump_dumpqvar(_tmp268);if(
_tmp269 == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpenumfields((
struct Cyc_List_List*)_tmp269->v);Cyc_Absyndump_dump_nospace(({const char*_tmp287="};";
_tag_dynforward(_tmp287,sizeof(char),_get_zero_arr_size_char(_tmp287,3));}));}
return;_LL1EB: if(*((int*)_tmp259)!= 2)goto _LL1ED;_tmp26A=((struct Cyc_Absyn_Let_d_struct*)
_tmp259)->f1;_tmp26B=((struct Cyc_Absyn_Let_d_struct*)_tmp259)->f3;_LL1EC: Cyc_Absyndump_dump(({
const char*_tmp288="let";_tag_dynforward(_tmp288,sizeof(char),
_get_zero_arr_size_char(_tmp288,4));}));Cyc_Absyndump_dumppat(_tmp26A);Cyc_Absyndump_dump_char((
int)'=');Cyc_Absyndump_dumpexp(_tmp26B);Cyc_Absyndump_dump_semi();goto _LL1E0;
_LL1ED: if(*((int*)_tmp259)!= 3)goto _LL1EF;_tmp26C=((struct Cyc_Absyn_Letv_d_struct*)
_tmp259)->f1;_LL1EE: Cyc_Absyndump_dump(({const char*_tmp289="let ";
_tag_dynforward(_tmp289,sizeof(char),_get_zero_arr_size_char(_tmp289,5));}));Cyc_Absyndump_dumpids(
_tmp26C);Cyc_Absyndump_dump_semi();goto _LL1E0;_LL1EF: if(*((int*)_tmp259)!= 7)
goto _LL1F1;_tmp26D=((struct Cyc_Absyn_Typedef_d_struct*)_tmp259)->f1;_LL1F0: if(!
Cyc_Absyndump_expand_typedefs  || _tmp26D->defn != 0  && Cyc_Absynpp_is_anon_aggrtype((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp26D->defn))->v)){Cyc_Absyndump_dump(({
const char*_tmp28A="typedef";_tag_dynforward(_tmp28A,sizeof(char),
_get_zero_arr_size_char(_tmp28A,8));}));{void*t;if(_tmp26D->defn == 0)t=Cyc_Absyn_new_evar(
_tmp26D->kind,0);else{t=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp26D->defn))->v;}((
void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),
struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(_tmp26D->tq,t,Cyc_Absyndump_dumptypedefname,
_tmp26D);Cyc_Absyndump_dumpatts(_tmp26D->atts);Cyc_Absyndump_dump_semi();}}goto
_LL1E0;_LL1F1: if(*((int*)_tmp259)!= 8)goto _LL1F3;_tmp26E=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp259)->f1;_tmp26F=((struct Cyc_Absyn_Namespace_d_struct*)_tmp259)->f2;_LL1F2:
Cyc_Absyndump_dump(({const char*_tmp28B="namespace";_tag_dynforward(_tmp28B,
sizeof(char),_get_zero_arr_size_char(_tmp28B,10));}));Cyc_Absyndump_dump_str(
_tmp26E);Cyc_Absyndump_dump_char((int)'{');for(0;_tmp26F != 0;_tmp26F=_tmp26F->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp26F->hd);}Cyc_Absyndump_dump_char((
int)'}');goto _LL1E0;_LL1F3: if(*((int*)_tmp259)!= 9)goto _LL1F5;_tmp270=((struct
Cyc_Absyn_Using_d_struct*)_tmp259)->f1;_tmp271=((struct Cyc_Absyn_Using_d_struct*)
_tmp259)->f2;_LL1F4: Cyc_Absyndump_dump(({const char*_tmp28C="using";
_tag_dynforward(_tmp28C,sizeof(char),_get_zero_arr_size_char(_tmp28C,6));}));Cyc_Absyndump_dumpqvar(
_tmp270);Cyc_Absyndump_dump_char((int)'{');for(0;_tmp271 != 0;_tmp271=_tmp271->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp271->hd);}Cyc_Absyndump_dump_char((
int)'}');goto _LL1E0;_LL1F5: if(*((int*)_tmp259)!= 10)goto _LL1F7;_tmp272=((struct
Cyc_Absyn_ExternC_d_struct*)_tmp259)->f1;_LL1F6: Cyc_Absyndump_dump(({const char*
_tmp28D="extern \"C\" {";_tag_dynforward(_tmp28D,sizeof(char),
_get_zero_arr_size_char(_tmp28D,13));}));for(0;_tmp272 != 0;_tmp272=_tmp272->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp272->hd);}Cyc_Absyndump_dump_char((
int)'}');goto _LL1E0;_LL1F7: if(*((int*)_tmp259)!= 11)goto _LL1F9;_tmp273=((struct
Cyc_Absyn_ExternCinclude_d_struct*)_tmp259)->f1;_tmp274=((struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp259)->f2;_LL1F8: Cyc_Absyndump_dump(({const char*_tmp28E="extern \"C include\" {";
_tag_dynforward(_tmp28E,sizeof(char),_get_zero_arr_size_char(_tmp28E,21));}));
for(0;_tmp273 != 0;_tmp273=_tmp273->tl){Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)
_tmp273->hd);}Cyc_Absyndump_dump_char((int)'}');if(_tmp274 != 0){Cyc_Absyndump_dump(({
const char*_tmp28F=" export {";_tag_dynforward(_tmp28F,sizeof(char),
_get_zero_arr_size_char(_tmp28F,10));}));for(0;_tmp274 != 0;_tmp274=_tmp274->tl){
struct _tuple0*_tmp291;struct _tuple12 _tmp290=*((struct _tuple12*)_tmp274->hd);
_tmp291=_tmp290.f2;Cyc_Absyndump_dumpqvar(_tmp291);if(_tmp274->tl != 0)Cyc_Absyndump_dump_char((
int)',');}Cyc_Absyndump_dump(({const char*_tmp292="}";_tag_dynforward(_tmp292,
sizeof(char),_get_zero_arr_size_char(_tmp292,2));}));}goto _LL1E0;_LL1F9: if((int)
_tmp259 != 0)goto _LL1FB;_LL1FA: Cyc_Absyndump_dump(({const char*_tmp293=" __cyclone_port_on__; ";
_tag_dynforward(_tmp293,sizeof(char),_get_zero_arr_size_char(_tmp293,23));}));
goto _LL1E0;_LL1FB: if((int)_tmp259 != 1)goto _LL1E0;_LL1FC: Cyc_Absyndump_dump(({
const char*_tmp294=" __cyclone_port_off__; ";_tag_dynforward(_tmp294,sizeof(char),
_get_zero_arr_size_char(_tmp294,24));}));goto _LL1E0;_LL1E0:;}static void Cyc_Absyndump_dump_upperbound(
struct Cyc_Absyn_Exp*e){struct _tuple6 pr=Cyc_Evexp_eval_const_uint_exp(e);if(pr.f1
!= 1  || !pr.f2){Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((
int)'}');}}void Cyc_Absyndump_dumptms(struct Cyc_List_List*tms,void(*f)(void*),
void*a){if(tms == 0){f(a);return;}{void*_tmp295=(void*)tms->hd;struct Cyc_Absyn_PtrAtts
_tmp296;void*_tmp297;struct Cyc_Absyn_Conref*_tmp298;struct Cyc_Absyn_Conref*
_tmp299;struct Cyc_Absyn_Conref*_tmp29A;struct Cyc_Absyn_Tqual _tmp29B;_LL20D: if(*((
int*)_tmp295)!= 2)goto _LL20F;_tmp296=((struct Cyc_Absyn_Pointer_mod_struct*)
_tmp295)->f1;_tmp297=(void*)_tmp296.rgn;_tmp298=_tmp296.nullable;_tmp299=_tmp296.bounds;
_tmp29A=_tmp296.zero_term;_tmp29B=((struct Cyc_Absyn_Pointer_mod_struct*)_tmp295)->f2;
_LL20E:{void*_tmp29C=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp299);struct
Cyc_Absyn_Exp*_tmp29D;void*_tmp29E;_LL212: if((int)_tmp29C != 0)goto _LL214;_LL213:
Cyc_Absyndump_dump_char((int)'?');goto _LL211;_LL214: if((int)_tmp29C != 1)goto
_LL216;_LL215: Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dump_char((int)'-');
goto _LL211;_LL216: if(_tmp29C <= (void*)2)goto _LL218;if(*((int*)_tmp29C)!= 0)goto
_LL218;_tmp29D=((struct Cyc_Absyn_Upper_b_struct*)_tmp29C)->f1;_LL217: Cyc_Absyndump_dump_char((
int)(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(1,_tmp298)?'*':'@'));
Cyc_Absyndump_dump_upperbound(_tmp29D);goto _LL211;_LL218: if(_tmp29C <= (void*)2)
goto _LL211;if(*((int*)_tmp29C)!= 1)goto _LL211;_tmp29E=(void*)((struct Cyc_Absyn_AbsUpper_b_struct*)
_tmp29C)->f1;_LL219: Cyc_Absyndump_dump_char((int)(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(1,_tmp298)?'*':'@'));Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumptyp(_tmp29E);Cyc_Absyndump_dump_char((int)'}');_LL211:;}if(((
int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp29A))Cyc_Absyndump_dump(({
const char*_tmp29F="ZEROTERM";_tag_dynforward(_tmp29F,sizeof(char),
_get_zero_arr_size_char(_tmp29F,9));}));{void*_tmp2A0=Cyc_Tcutil_compress(
_tmp297);struct Cyc_Absyn_Tvar*_tmp2A1;struct Cyc_Core_Opt*_tmp2A2;_LL21B: if((int)
_tmp2A0 != 2)goto _LL21D;_LL21C: goto _LL21A;_LL21D: if(_tmp2A0 <= (void*)4)goto _LL221;
if(*((int*)_tmp2A0)!= 1)goto _LL21F;_tmp2A1=((struct Cyc_Absyn_VarType_struct*)
_tmp2A0)->f1;_LL21E: Cyc_Absyndump_dump_str(_tmp2A1->name);goto _LL21A;_LL21F: if(*((
int*)_tmp2A0)!= 0)goto _LL221;_tmp2A2=((struct Cyc_Absyn_Evar_struct*)_tmp2A0)->f2;
if(_tmp2A2 != 0)goto _LL221;_LL220: Cyc_Absyndump_dumpntyp(Cyc_Tcutil_compress(
_tmp297));goto _LL21A;_LL221:;_LL222:({void*_tmp2A3=0;Cyc_Tcutil_impos(({const
char*_tmp2A4="dumptms: bad rgn type in Pointer_mod";_tag_dynforward(_tmp2A4,
sizeof(char),_get_zero_arr_size_char(_tmp2A4,37));}),_tag_dynforward(_tmp2A3,
sizeof(void*),0));});_LL21A:;}Cyc_Absyndump_dumptq(_tmp29B);Cyc_Absyndump_dumptms(
tms->tl,f,a);return;_LL20F:;_LL210: {int next_is_pointer=0;if(tms->tl != 0){void*
_tmp2A5=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_LL224: if(*((int*)
_tmp2A5)!= 2)goto _LL226;_LL225: next_is_pointer=1;goto _LL223;_LL226:;_LL227: goto
_LL223;_LL223:;}if(next_is_pointer)Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptms(
tms->tl,f,a);if(next_is_pointer)Cyc_Absyndump_dump_char((int)')');{void*_tmp2A6=(
void*)tms->hd;struct Cyc_Absyn_Conref*_tmp2A7;struct Cyc_Absyn_Exp*_tmp2A8;struct
Cyc_Absyn_Conref*_tmp2A9;void*_tmp2AA;struct Cyc_List_List*_tmp2AB;int _tmp2AC;
struct Cyc_Absyn_VarargInfo*_tmp2AD;struct Cyc_Core_Opt*_tmp2AE;struct Cyc_List_List*
_tmp2AF;void*_tmp2B0;struct Cyc_List_List*_tmp2B1;struct Cyc_Position_Segment*
_tmp2B2;struct Cyc_List_List*_tmp2B3;struct Cyc_Position_Segment*_tmp2B4;int
_tmp2B5;struct Cyc_List_List*_tmp2B6;_LL229: if(*((int*)_tmp2A6)!= 0)goto _LL22B;
_tmp2A7=((struct Cyc_Absyn_Carray_mod_struct*)_tmp2A6)->f1;_LL22A: Cyc_Absyndump_dump(({
const char*_tmp2B7="[]";_tag_dynforward(_tmp2B7,sizeof(char),
_get_zero_arr_size_char(_tmp2B7,3));}));if(((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,_tmp2A7))Cyc_Absyndump_dump(({const char*_tmp2B8="ZEROTERM";
_tag_dynforward(_tmp2B8,sizeof(char),_get_zero_arr_size_char(_tmp2B8,9));}));
goto _LL228;_LL22B: if(*((int*)_tmp2A6)!= 1)goto _LL22D;_tmp2A8=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmp2A6)->f1;_tmp2A9=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmp2A6)->f2;
_LL22C: Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp2A8);Cyc_Absyndump_dump_char((
int)']');if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp2A9))Cyc_Absyndump_dump(({const char*_tmp2B9="ZEROTERM";_tag_dynforward(
_tmp2B9,sizeof(char),_get_zero_arr_size_char(_tmp2B9,9));}));goto _LL228;_LL22D:
if(*((int*)_tmp2A6)!= 3)goto _LL22F;_tmp2AA=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp2A6)->f1;if(*((int*)_tmp2AA)!= 1)goto _LL22F;_tmp2AB=((struct Cyc_Absyn_WithTypes_struct*)
_tmp2AA)->f1;_tmp2AC=((struct Cyc_Absyn_WithTypes_struct*)_tmp2AA)->f2;_tmp2AD=((
struct Cyc_Absyn_WithTypes_struct*)_tmp2AA)->f3;_tmp2AE=((struct Cyc_Absyn_WithTypes_struct*)
_tmp2AA)->f4;_tmp2AF=((struct Cyc_Absyn_WithTypes_struct*)_tmp2AA)->f5;_LL22E: Cyc_Absyndump_dumpfunargs(
_tmp2AB,_tmp2AC,_tmp2AD,_tmp2AE,_tmp2AF);goto _LL228;_LL22F: if(*((int*)_tmp2A6)!= 
3)goto _LL231;_tmp2B0=(void*)((struct Cyc_Absyn_Function_mod_struct*)_tmp2A6)->f1;
if(*((int*)_tmp2B0)!= 0)goto _LL231;_tmp2B1=((struct Cyc_Absyn_NoTypes_struct*)
_tmp2B0)->f1;_tmp2B2=((struct Cyc_Absyn_NoTypes_struct*)_tmp2B0)->f2;_LL230:((
void(*)(void(*f)(struct _dynforward_ptr*),struct Cyc_List_List*l,struct
_dynforward_ptr start,struct _dynforward_ptr end,struct _dynforward_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dump_str,_tmp2B1,({const char*_tmp2BA="(";_tag_dynforward(_tmp2BA,
sizeof(char),_get_zero_arr_size_char(_tmp2BA,2));}),({const char*_tmp2BB=")";
_tag_dynforward(_tmp2BB,sizeof(char),_get_zero_arr_size_char(_tmp2BB,2));}),({
const char*_tmp2BC=",";_tag_dynforward(_tmp2BC,sizeof(char),
_get_zero_arr_size_char(_tmp2BC,2));}));goto _LL228;_LL231: if(*((int*)_tmp2A6)!= 
4)goto _LL233;_tmp2B3=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp2A6)->f1;
_tmp2B4=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp2A6)->f2;_tmp2B5=((struct
Cyc_Absyn_TypeParams_mod_struct*)_tmp2A6)->f3;_LL232: if(_tmp2B5)Cyc_Absyndump_dumpkindedtvars(
_tmp2B3);else{Cyc_Absyndump_dumptvars(_tmp2B3);}goto _LL228;_LL233: if(*((int*)
_tmp2A6)!= 5)goto _LL235;_tmp2B6=((struct Cyc_Absyn_Attributes_mod_struct*)_tmp2A6)->f2;
_LL234: Cyc_Absyndump_dumpatts(_tmp2B6);goto _LL228;_LL235: if(*((int*)_tmp2A6)!= 2)
goto _LL228;_LL236:({void*_tmp2BD=0;Cyc_Tcutil_impos(({const char*_tmp2BE="dumptms";
_tag_dynforward(_tmp2BE,sizeof(char),_get_zero_arr_size_char(_tmp2BE,8));}),
_tag_dynforward(_tmp2BD,sizeof(void*),0));});_LL228:;}return;}_LL20C:;}}void Cyc_Absyndump_dumptqtd(
struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){struct _RegionHandle
_tmp2BF=_new_region("temp");struct _RegionHandle*temp=& _tmp2BF;_push_region(temp);{
struct Cyc_Absyn_Tqual _tmp2C1;void*_tmp2C2;struct Cyc_List_List*_tmp2C3;struct
_tuple5 _tmp2C0=Cyc_Absynpp_to_tms(temp,tq,t);_tmp2C1=_tmp2C0.f1;_tmp2C2=_tmp2C0.f2;
_tmp2C3=_tmp2C0.f3;Cyc_Absyndump_dumptq(_tmp2C1);Cyc_Absyndump_dumpntyp(_tmp2C2);
Cyc_Absyndump_dumptms(Cyc_List_imp_rev(_tmp2C3),f,a);};_pop_region(temp);}void
Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
Cyc_Absyndump_pos=0;*Cyc_Absyndump_dump_file=f;for(0;tdl != 0;tdl=tdl->tl){Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)tdl->hd);}({void*_tmp2C4=0;Cyc_fprintf(f,({const char*
_tmp2C5="\n";_tag_dynforward(_tmp2C5,sizeof(char),_get_zero_arr_size_char(
_tmp2C5,2));}),_tag_dynforward(_tmp2C4,sizeof(void*),0));});}
