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
consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_BottomFL_struct{
int tag;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};union Cyc_CfFlowInfo_FlowInfo_union{
struct Cyc_CfFlowInfo_BottomFL_struct BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*
unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*
esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct
_RegionHandle*r);int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct
Cyc_CfFlowInfo_Place*);struct _dynforward_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(
struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,void*);void*Cyc_CfFlowInfo_typ_to_absrval(
struct Cyc_CfFlowInfo_FlowEnv*,void*t,void*leafval);void*Cyc_CfFlowInfo_initlevel(
struct Cyc_Dict_Dict d,void*r);void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict
d,struct Cyc_CfFlowInfo_Place*place);int Cyc_CfFlowInfo_flow_lessthan_approx(union
Cyc_CfFlowInfo_FlowInfo_union f1,union Cyc_CfFlowInfo_FlowInfo_union f2);struct Cyc_List_List*
Cyc_CfFlowInfo_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct
Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_reln_assign_exp(
struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
struct Cyc_List_List*Cyc_CfFlowInfo_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,
struct Cyc_Absyn_Exp*);struct Cyc_List_List*Cyc_CfFlowInfo_copy_relns(struct
_RegionHandle*,struct Cyc_List_List*);int Cyc_CfFlowInfo_same_relns(struct Cyc_List_List*,
struct Cyc_List_List*);struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);struct Cyc_Dict_Dict
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
struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser){(void*)(enclosee->annot=(
void*)((void*)({struct Cyc_NewControlFlow_CFAnnot_struct*_tmp0=_cycalloc(sizeof(*
_tmp0));_tmp0[0]=({struct Cyc_NewControlFlow_CFAnnot_struct _tmp1;_tmp1.tag=Cyc_NewControlFlow_CFAnnot;
_tmp1.f1=({struct Cyc_NewControlFlow_CFStmtAnnot _tmp2;_tmp2.encloser=encloser;
_tmp2.visited=0;_tmp2;});_tmp1;});_tmp0;})));}struct Cyc_NewControlFlow_AnalEnv{
struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int
iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo_union tryflow;struct Cyc_Dict_Dict*
all_changed;int noreturn;struct Cyc_List_List*param_roots;struct Cyc_Hashtable_Table*
flow_table;};static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_stmt(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Stmt*);
static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Decl*);struct _tuple7{union Cyc_CfFlowInfo_FlowInfo_union
f1;union Cyc_CfFlowInfo_AbsLVal_union f2;};static struct _tuple7 Cyc_NewControlFlow_anal_Lexp(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);
static struct _tuple5 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,
union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);struct _tuple8{union Cyc_CfFlowInfo_FlowInfo_union
f1;union Cyc_CfFlowInfo_FlowInfo_union f2;};static struct _tuple8 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo_union,struct Cyc_Absyn_Exp*);
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_CfFlowInfo_Place*p,void*t);static struct Cyc_NewControlFlow_CFStmtAnnot*
Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){void*_tmp3=(void*)s->annot;
struct Cyc_NewControlFlow_CFStmtAnnot _tmp4;struct Cyc_NewControlFlow_CFStmtAnnot*
_tmp5;_LL1: if(*((void**)_tmp3)!= Cyc_NewControlFlow_CFAnnot)goto _LL3;_tmp4=((
struct Cyc_NewControlFlow_CFAnnot_struct*)_tmp3)->f1;_tmp5=(struct Cyc_NewControlFlow_CFStmtAnnot*)&((
struct Cyc_NewControlFlow_CFAnnot_struct*)_tmp3)->f1;_LL2: return _tmp5;_LL3:;_LL4:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp6=_cycalloc(sizeof(*
_tmp6));_tmp6[0]=({struct Cyc_Core_Impossible_struct _tmp7;_tmp7.tag=Cyc_Core_Impossible;
_tmp7.f1=({const char*_tmp8="ControlFlow -- wrong stmt annotation";
_tag_dynforward(_tmp8,sizeof(char),_get_zero_arr_size_char(_tmp8,37));});_tmp7;});
_tmp6;}));_LL0:;}static union Cyc_CfFlowInfo_FlowInfo_union*Cyc_NewControlFlow_get_stmt_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo_union**
sflow=((union Cyc_CfFlowInfo_FlowInfo_union**(*)(struct Cyc_Hashtable_Table*t,
struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);if(sflow
== 0){union Cyc_CfFlowInfo_FlowInfo_union*res=({union Cyc_CfFlowInfo_FlowInfo_union*
_tmp9=_region_malloc(env->r,sizeof(*_tmp9));_tmp9[0]=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmpA;(_tmpA.BottomFL).tag=0;_tmpA;});_tmp9;});((
void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo_union*
val))Cyc_Hashtable_insert)(env->flow_table,s,res);return res;}return*sflow;}
struct _tuple9{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo_union*
f2;};static struct _tuple9 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Stmt*s){struct Cyc_NewControlFlow_CFStmtAnnot*
_tmpB=Cyc_NewControlFlow_get_stmt_annot(s);union Cyc_CfFlowInfo_FlowInfo_union*
_tmpC=Cyc_NewControlFlow_get_stmt_flow(env,s);*_tmpC=Cyc_CfFlowInfo_join_flow(
env->fenv,env->all_changed,inflow,*_tmpC,1);_tmpB->visited=env->iteration_num;
return({struct _tuple9 _tmpD;_tmpD.f1=_tmpB;_tmpD.f2=_tmpC;_tmpD;});}static int Cyc_NewControlFlow_get_field_index_fs(
struct Cyc_List_List*fs,struct _dynforward_ptr*f){int n=0;for(0;(unsigned int)fs;fs=
fs->tl){struct _dynforward_ptr*_tmpE=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;
if(Cyc_strptrcmp(_tmpE,f)== 0)return n;n=n + 1;}(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF[0]=({struct Cyc_Core_Impossible_struct
_tmp10;_tmp10.tag=Cyc_Core_Impossible;_tmp10.f1=(struct _dynforward_ptr)({struct
Cyc_String_pa_struct _tmp13;_tmp13.tag=0;_tmp13.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*f);{void*_tmp11[1]={& _tmp13};Cyc_aprintf(({const char*
_tmp12="get_field_index_fs failed: %s";_tag_dynforward(_tmp12,sizeof(char),
_get_zero_arr_size_char(_tmp12,30));}),_tag_dynforward(_tmp11,sizeof(void*),1));}});
_tmp10;});_tmpF;}));}static int Cyc_NewControlFlow_get_field_index(void*t,struct
_dynforward_ptr*f){void*_tmp14=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp15;union Cyc_Absyn_AggrInfoU_union _tmp16;struct Cyc_List_List*_tmp17;_LL6: if(
_tmp14 <= (void*)4)goto _LLA;if(*((int*)_tmp14)!= 10)goto _LL8;_tmp15=((struct Cyc_Absyn_AggrType_struct*)
_tmp14)->f1;_tmp16=_tmp15.aggr_info;_LL7: {struct Cyc_List_List*_tmp18=((struct
Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp16))->impl))->fields;
_tmp17=_tmp18;goto _LL9;}_LL8: if(*((int*)_tmp14)!= 11)goto _LLA;_tmp17=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp14)->f2;_LL9: return Cyc_NewControlFlow_get_field_index_fs(_tmp17,f);_LLA:;
_LLB:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp19=_cycalloc(
sizeof(*_tmp19));_tmp19[0]=({struct Cyc_Core_Impossible_struct _tmp1A;_tmp1A.tag=
Cyc_Core_Impossible;_tmp1A.f1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp1D;_tmp1D.tag=0;_tmp1D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp1B[1]={& _tmp1D};Cyc_aprintf(({const char*_tmp1C="get_field_index failed: %s";
_tag_dynforward(_tmp1C,sizeof(char),_get_zero_arr_size_char(_tmp1C,27));}),
_tag_dynforward(_tmp1B,sizeof(void*),1));}});_tmp1A;});_tmp19;}));_LL5:;}static
void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo_union new_flow){if(env->in_try){env->tryflow=Cyc_CfFlowInfo_join_flow(
env->fenv,env->all_changed,new_flow,env->tryflow,1);{struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1F;struct Cyc_Dict_Dict _tmp20;struct Cyc_List_List*_tmp21;struct _tuple6 _tmp1E=
Cyc_CfFlowInfo_save_consume_info(env->fenv,env->tryflow,0);_tmp1F=_tmp1E.f1;
_tmp20=_tmp1F.consumed;_tmp21=_tmp1F.may_consume;env->tryflow=Cyc_CfFlowInfo_restore_consume_info(
env->tryflow,({struct Cyc_CfFlowInfo_ConsumeInfo _tmp22;_tmp22.consumed=_tmp20;
_tmp22.may_consume=0;_tmp22;}));}}}static void Cyc_NewControlFlow_update_flow(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo_union
flow){struct Cyc_NewControlFlow_CFStmtAnnot*_tmp23=Cyc_NewControlFlow_get_stmt_annot(
s);union Cyc_CfFlowInfo_FlowInfo_union*_tmp24=Cyc_NewControlFlow_get_stmt_flow(
env,s);union Cyc_CfFlowInfo_FlowInfo_union _tmp25=Cyc_CfFlowInfo_join_flow(env->fenv,
env->all_changed,flow,*_tmp24,1);if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp25,*
_tmp24)){*_tmp24=_tmp25;if(_tmp23->visited == env->iteration_num)env->iterate_again=
1;}}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_add_vars(struct
Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_List_List*
vds,void*leafval,struct Cyc_Position_Segment*loc){union Cyc_CfFlowInfo_FlowInfo_union
_tmp26=inflow;struct Cyc_Dict_Dict _tmp27;struct Cyc_List_List*_tmp28;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp29;_LLD: if((_tmp26.BottomFL).tag != 0)goto _LLF;_LLE: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp2A;(_tmp2A.BottomFL).tag=0;_tmp2A;});_LLF:
if((_tmp26.ReachableFL).tag != 1)goto _LLC;_tmp27=(_tmp26.ReachableFL).f1;_tmp28=(
_tmp26.ReachableFL).f2;_tmp29=(_tmp26.ReachableFL).f3;_LL10: for(0;vds != 0;vds=
vds->tl){struct Cyc_CfFlowInfo_VarRoot_struct*_tmp2B=({struct Cyc_CfFlowInfo_VarRoot_struct*
_tmp2C=_region_malloc(fenv->r,sizeof(*_tmp2C));_tmp2C[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp2D;_tmp2D.tag=0;_tmp2D.f1=(struct Cyc_Absyn_Vardecl*)vds->hd;_tmp2D;});_tmp2C;});
_tmp27=Cyc_Dict_insert(_tmp27,(void*)_tmp2B,Cyc_CfFlowInfo_typ_to_absrval(fenv,(
void*)((struct Cyc_Absyn_Vardecl*)vds->hd)->type,leafval));}return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp2E;(_tmp2E.ReachableFL).tag=1;(_tmp2E.ReachableFL).f1=
_tmp27;(_tmp2E.ReachableFL).f2=_tmp28;(_tmp2E.ReachableFL).f3=_tmp29;_tmp2E;});
_LLC:;}struct _tuple10{struct Cyc_CfFlowInfo_Place*f1;struct Cyc_Position_Segment*
f2;};static void Cyc_NewControlFlow_remove_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,
union Cyc_CfFlowInfo_FlowInfo_union outflow,struct Cyc_Dict_Dict*out_unique_places,
struct Cyc_Dict_Dict old_unique_places){struct Cyc_CfFlowInfo_ConsumeInfo _tmp30;
struct Cyc_Dict_Dict _tmp31;struct _tuple6 _tmp2F=Cyc_CfFlowInfo_save_consume_info(
fenv,outflow,0);_tmp30=_tmp2F.f1;_tmp31=_tmp30.consumed;{struct _RegionHandle
_tmp32=_new_region("tmp");struct _RegionHandle*tmp=& _tmp32;_push_region(tmp);{
struct Cyc_Iter_Iter _tmp33=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,
struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(tmp,*((struct Cyc_Dict_Dict*)
_check_null(out_unique_places)));struct _tuple10 _tmp34=*((struct _tuple10*(*)(
struct _RegionHandle*r,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(tmp,*
out_unique_places);while(((int(*)(struct Cyc_Iter_Iter,struct _tuple10*))Cyc_Iter_next)(
_tmp33,& _tmp34)){struct Cyc_CfFlowInfo_Place*_tmp35=_tmp34.f1;if(!((int(*)(struct
Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(_tmp31,_tmp35))({
struct Cyc_String_pa_struct _tmp38;_tmp38.tag=0;_tmp38.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)Cyc_CfFlowInfo_place_err_string(_tmp35));{void*_tmp36[1]={&
_tmp38};Cyc_Tcutil_terr(_tmp34.f2,({const char*_tmp37="Failed to consume unique variable %s";
_tag_dynforward(_tmp37,sizeof(char),_get_zero_arr_size_char(_tmp37,37));}),
_tag_dynforward(_tmp36,sizeof(void*),1));}});}};_pop_region(tmp);}*
out_unique_places=old_unique_places;}static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_use_Rval(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Position_Segment*loc,union Cyc_CfFlowInfo_FlowInfo_union
inflow,void*r){union Cyc_CfFlowInfo_FlowInfo_union _tmp39=inflow;struct Cyc_Dict_Dict
_tmp3A;struct Cyc_List_List*_tmp3B;struct Cyc_CfFlowInfo_ConsumeInfo _tmp3C;_LL12:
if((_tmp39.BottomFL).tag != 0)goto _LL14;_LL13: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3D;(_tmp3D.BottomFL).tag=0;_tmp3D;});_LL14:
if((_tmp39.ReachableFL).tag != 1)goto _LL11;_tmp3A=(_tmp39.ReachableFL).f1;_tmp3B=(
_tmp39.ReachableFL).f2;_tmp3C=(_tmp39.ReachableFL).f3;_LL15: if(Cyc_CfFlowInfo_initlevel(
_tmp3A,r)!= (void*)2)({void*_tmp3E=0;Cyc_Tcutil_terr(loc,({const char*_tmp3F="expression may not be fully initialized";
_tag_dynforward(_tmp3F,sizeof(char),_get_zero_arr_size_char(_tmp3F,40));}),
_tag_dynforward(_tmp3E,sizeof(void*),0));});{struct Cyc_Dict_Dict _tmp40=Cyc_CfFlowInfo_escape_deref(
env->fenv,_tmp3A,env->all_changed,r);if(_tmp3A.t == _tmp40.t)return inflow;{union
Cyc_CfFlowInfo_FlowInfo_union _tmp41=({union Cyc_CfFlowInfo_FlowInfo_union _tmp42;(
_tmp42.ReachableFL).tag=1;(_tmp42.ReachableFL).f1=_tmp40;(_tmp42.ReachableFL).f2=
_tmp3B;(_tmp42.ReachableFL).f3=_tmp3C;_tmp42;});Cyc_NewControlFlow_update_tryflow(
env,(union Cyc_CfFlowInfo_FlowInfo_union)_tmp41);return(union Cyc_CfFlowInfo_FlowInfo_union)
_tmp41;}}_LL11:;}struct _tuple11{union Cyc_CfFlowInfo_FlowInfo_union f1;struct Cyc_List_List*
f2;};static struct _tuple11 Cyc_NewControlFlow_anal_unordered_Rexps(struct
_RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_List_List*es,int arg1_unconsumed){if(es == 0)return({struct
_tuple11 _tmp43;_tmp43.f1=inflow;_tmp43.f2=0;_tmp43;});if(es->tl == 0){union Cyc_CfFlowInfo_FlowInfo_union
_tmp45;void*_tmp46;struct _tuple5 _tmp44=Cyc_NewControlFlow_anal_Rexp(env,inflow,(
struct Cyc_Absyn_Exp*)es->hd);_tmp45=_tmp44.f1;_tmp46=_tmp44.f2;return({struct
_tuple11 _tmp47;_tmp47.f1=_tmp45;_tmp47.f2=({struct Cyc_List_List*_tmp48=
_region_malloc(rgn,sizeof(*_tmp48));_tmp48->hd=(void*)_tmp46;_tmp48->tl=0;_tmp48;});
_tmp47;});}{struct Cyc_Dict_Dict*outer_all_changed=env->all_changed;struct Cyc_Dict_Dict
this_all_changed;union Cyc_CfFlowInfo_FlowInfo_union old_inflow;union Cyc_CfFlowInfo_FlowInfo_union
outflow;struct Cyc_List_List*rvals;struct Cyc_CfFlowInfo_ConsumeInfo _tmp4A;union
Cyc_CfFlowInfo_FlowInfo_union _tmp4B;struct _tuple6 _tmp49=Cyc_CfFlowInfo_save_consume_info(
env->fenv,inflow,1);_tmp4A=_tmp49.f1;_tmp4B=_tmp49.f2;{struct Cyc_CfFlowInfo_ConsumeInfo
outflow_consume=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp60;_tmp60.consumed=(env->fenv)->mt_place_set;
_tmp60.may_consume=0;_tmp60;});int init_consume=0;do{this_all_changed=(env->fenv)->mt_place_set;
_tmp4B=Cyc_CfFlowInfo_restore_consume_info(_tmp4B,_tmp4A);env->all_changed=({
struct Cyc_Dict_Dict*_tmp4C=_region_malloc(env->r,sizeof(*_tmp4C));_tmp4C[0]=(env->fenv)->mt_place_set;
_tmp4C;});{union Cyc_CfFlowInfo_FlowInfo_union _tmp4E;void*_tmp4F;struct _tuple5
_tmp4D=Cyc_NewControlFlow_anal_Rexp(env,_tmp4B,(struct Cyc_Absyn_Exp*)es->hd);
_tmp4E=_tmp4D.f1;_tmp4F=_tmp4D.f2;outflow=_tmp4E;rvals=({struct Cyc_List_List*
_tmp50=_region_malloc(rgn,sizeof(*_tmp50));_tmp50->hd=(void*)_tmp4F;_tmp50->tl=0;
_tmp50;});this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((
struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);if(arg1_unconsumed){union
Cyc_CfFlowInfo_FlowInfo_union _tmp51=outflow;struct Cyc_Dict_Dict _tmp52;struct Cyc_List_List*
_tmp53;struct Cyc_CfFlowInfo_ConsumeInfo _tmp54;_LL17: if((_tmp51.BottomFL).tag != 0)
goto _LL19;_LL18: goto _LL16;_LL19: if((_tmp51.ReachableFL).tag != 1)goto _LL16;_tmp52=(
_tmp51.ReachableFL).f1;_tmp53=(_tmp51.ReachableFL).f2;_tmp54=(_tmp51.ReachableFL).f3;
_LL1A: outflow=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp55;(_tmp55.ReachableFL).tag=1;(_tmp55.ReachableFL).f1=_tmp52;(_tmp55.ReachableFL).f2=
_tmp53;(_tmp55.ReachableFL).f3=({struct Cyc_CfFlowInfo_ConsumeInfo _tmp56;_tmp56.consumed=
_tmp54.consumed;_tmp56.may_consume=_tmp4A.may_consume;_tmp56;});_tmp55;});_LL16:;}{
struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){env->all_changed=({
struct Cyc_Dict_Dict*_tmp57=_region_malloc(env->r,sizeof(*_tmp57));_tmp57[0]=(env->fenv)->mt_place_set;
_tmp57;});{union Cyc_CfFlowInfo_FlowInfo_union _tmp59;void*_tmp5A;struct _tuple5
_tmp58=Cyc_NewControlFlow_anal_Rexp(env,_tmp4B,(struct Cyc_Absyn_Exp*)es2->hd);
_tmp59=_tmp58.f1;_tmp5A=_tmp58.f2;rvals=({struct Cyc_List_List*_tmp5B=
_region_malloc(rgn,sizeof(*_tmp5B));_tmp5B->hd=(void*)_tmp5A;_tmp5B->tl=rvals;
_tmp5B;});outflow=Cyc_CfFlowInfo_after_flow(env->fenv,(struct Cyc_Dict_Dict*)&
this_all_changed,outflow,_tmp59,this_all_changed,*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed)));this_all_changed=Cyc_CfFlowInfo_union_place_set(
this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);}}}{
struct Cyc_CfFlowInfo_ConsumeInfo _tmp5D;struct _tuple6 _tmp5C=Cyc_CfFlowInfo_save_consume_info(
env->fenv,outflow,0);_tmp5D=_tmp5C.f1;if(init_consume)old_inflow=Cyc_CfFlowInfo_restore_consume_info(
_tmp4B,outflow_consume);else{old_inflow=_tmp4B;}init_consume=1;outflow_consume=
_tmp5D;_tmp4B=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,_tmp4B,
outflow,1);}}}while(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp4B,old_inflow));if(
outer_all_changed == 0)env->all_changed=0;else{env->all_changed=({struct Cyc_Dict_Dict*
_tmp5E=_region_malloc(env->r,sizeof(*_tmp5E));_tmp5E[0]=Cyc_CfFlowInfo_union_place_set(*
outer_all_changed,this_all_changed,0);_tmp5E;});}Cyc_NewControlFlow_update_tryflow(
env,outflow);return({struct _tuple11 _tmp5F;_tmp5F.f1=outflow;_tmp5F.f2=Cyc_List_imp_rev(
rvals);_tmp5F;});}}}static struct _tuple5 Cyc_NewControlFlow_anal_use_ints(struct
Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct
Cyc_List_List*es,int arg1_unconsumed){struct _RegionHandle _tmp61=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp61;_push_region(rgn);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp63;struct Cyc_List_List*_tmp64;struct _tuple11 _tmp62=Cyc_NewControlFlow_anal_unordered_Rexps(
rgn,env,inflow,es,arg1_unconsumed);_tmp63=_tmp62.f1;_tmp64=_tmp62.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp65=_tmp63;struct Cyc_Dict_Dict _tmp66;_LL1C: if((_tmp65.ReachableFL).tag != 1)
goto _LL1E;_tmp66=(_tmp65.ReachableFL).f1;_LL1D: for(0;_tmp64 != 0;(_tmp64=_tmp64->tl,
es=((struct Cyc_List_List*)_check_null(es))->tl)){if(Cyc_CfFlowInfo_initlevel(
_tmp66,(void*)_tmp64->hd)== (void*)0)({void*_tmp67=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(es))->hd)->loc,({const char*_tmp68="expression may not be initialized";
_tag_dynforward(_tmp68,sizeof(char),_get_zero_arr_size_char(_tmp68,34));}),
_tag_dynforward(_tmp67,sizeof(void*),0));});}goto _LL1B;_LL1E: if((_tmp65.BottomFL).tag
!= 0)goto _LL1B;_LL1F: goto _LL1B;_LL1B:;}{struct _tuple5 _tmp6A=({struct _tuple5
_tmp69;_tmp69.f1=_tmp63;_tmp69.f2=(void*)(env->fenv)->unknown_all;_tmp69;});
_npop_handler(0);return _tmp6A;}};_pop_region(rgn);}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,union Cyc_CfFlowInfo_FlowInfo_union outflow,struct Cyc_Absyn_Exp*e,void*il){
union Cyc_CfFlowInfo_FlowInfo_union _tmp6B=outflow;struct Cyc_Dict_Dict _tmp6C;
struct Cyc_List_List*_tmp6D;struct Cyc_CfFlowInfo_ConsumeInfo _tmp6E;_LL21: if((
_tmp6B.BottomFL).tag != 0)goto _LL23;_LL22: return outflow;_LL23: if((_tmp6B.ReachableFL).tag
!= 1)goto _LL20;_tmp6C=(_tmp6B.ReachableFL).f1;_tmp6D=(_tmp6B.ReachableFL).f2;
_tmp6E=(_tmp6B.ReachableFL).f3;_LL24: {union Cyc_CfFlowInfo_AbsLVal_union _tmp6F=(
Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp70;
_LL26: if((_tmp6F.UnknownL).tag != 1)goto _LL28;_LL27: return outflow;_LL28: if((
_tmp6F.PlaceL).tag != 0)goto _LL25;_tmp70=(_tmp6F.PlaceL).f1;_LL29: {void*nzval=il
== (void*)2?(void*)1:(void*)2;union Cyc_CfFlowInfo_FlowInfo_union _tmp71=({union
Cyc_CfFlowInfo_FlowInfo_union _tmp72;(_tmp72.ReachableFL).tag=1;(_tmp72.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp6C,env->all_changed,_tmp70,
nzval);(_tmp72.ReachableFL).f2=_tmp6D;(_tmp72.ReachableFL).f3=_tmp6E;_tmp72;});
return(union Cyc_CfFlowInfo_FlowInfo_union)_tmp71;}_LL25:;}_LL20:;}static struct
_tuple8 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo_union inflow,union Cyc_CfFlowInfo_FlowInfo_union outflow,
struct Cyc_Absyn_Exp*e,void*il){union Cyc_CfFlowInfo_FlowInfo_union _tmp73=outflow;
struct Cyc_Dict_Dict _tmp74;struct Cyc_List_List*_tmp75;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp76;_LL2B: if((_tmp73.BottomFL).tag != 0)goto _LL2D;_LL2C: return({struct _tuple8
_tmp77;_tmp77.f1=outflow;_tmp77.f2=outflow;_tmp77;});_LL2D: if((_tmp73.ReachableFL).tag
!= 1)goto _LL2A;_tmp74=(_tmp73.ReachableFL).f1;_tmp75=(_tmp73.ReachableFL).f2;
_tmp76=(_tmp73.ReachableFL).f3;_LL2E: {union Cyc_CfFlowInfo_AbsLVal_union _tmp78=(
Cyc_NewControlFlow_anal_Lexp(env,inflow,e)).f2;struct Cyc_CfFlowInfo_Place*_tmp79;
_LL30: if((_tmp78.UnknownL).tag != 1)goto _LL32;_LL31: return({struct _tuple8 _tmp7A;
_tmp7A.f1=outflow;_tmp7A.f2=outflow;_tmp7A;});_LL32: if((_tmp78.PlaceL).tag != 0)
goto _LL2F;_tmp79=(_tmp78.PlaceL).f1;_LL33: {void*nzval=il == (void*)2?(void*)1:(
void*)2;return({struct _tuple8 _tmp7B;_tmp7B.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp7D;(_tmp7D.ReachableFL).tag=1;(_tmp7D.ReachableFL).f1=
Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmp74,env->all_changed,_tmp79,
nzval);(_tmp7D.ReachableFL).f2=_tmp75;(_tmp7D.ReachableFL).f3=_tmp76;_tmp7D;});
_tmp7B.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp7C;(_tmp7C.ReachableFL).tag=1;(_tmp7C.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
env->fenv,e->loc,_tmp74,env->all_changed,_tmp79,(void*)0);(_tmp7C.ReachableFL).f2=
_tmp75;(_tmp7C.ReachableFL).f3=_tmp76;_tmp7C;});_tmp7B;});}_LL2F:;}_LL2A:;}
static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_if_tagcmp(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union flow,struct Cyc_Absyn_Exp*e1,void*r1,void*p,
void*r2){union Cyc_CfFlowInfo_FlowInfo_union _tmp7E=flow;struct Cyc_Dict_Dict _tmp7F;
struct Cyc_List_List*_tmp80;struct Cyc_CfFlowInfo_ConsumeInfo _tmp81;_LL35: if((
_tmp7E.BottomFL).tag != 0)goto _LL37;_LL36: return flow;_LL37: if((_tmp7E.ReachableFL).tag
!= 1)goto _LL34;_tmp7F=(_tmp7E.ReachableFL).f1;_tmp80=(_tmp7E.ReachableFL).f2;
_tmp81=(_tmp7E.ReachableFL).f3;_LL38: {void*_tmp82=r2;struct Cyc_List_List*_tmp83;
_LL3A: if(_tmp82 <= (void*)3)goto _LL3C;if(*((int*)_tmp82)!= 3)goto _LL3C;_tmp83=((
struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp82)->f1;_LL3B: {union Cyc_CfFlowInfo_AbsLVal_union
_tmp84=(Cyc_NewControlFlow_anal_Lexp(env,flow,e1)).f2;struct Cyc_CfFlowInfo_Place*
_tmp85;_LL3F: if((_tmp84.UnknownL).tag != 1)goto _LL41;_LL40: return flow;_LL41: if((
_tmp84.PlaceL).tag != 0)goto _LL3E;_tmp85=(_tmp84.PlaceL).f1;_LL42: {struct Cyc_List_List*
new_cl;{void*_tmp86=r1;struct Cyc_List_List*_tmp87;void*_tmp88;_LL44: if(_tmp86 <= (
void*)3)goto _LL48;if(*((int*)_tmp86)!= 3)goto _LL46;_tmp87=((struct Cyc_CfFlowInfo_TagCmps_struct*)
_tmp86)->f1;_LL45: new_cl=_tmp87;goto _LL43;_LL46: if(*((int*)_tmp86)!= 0)goto _LL48;
_tmp88=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp86)->f1;if((int)_tmp88
!= 2)goto _LL48;_LL47: goto _LL49;_LL48: if((int)_tmp86 != 0)goto _LL4A;_LL49: goto
_LL4B;_LL4A: if((int)_tmp86 != 1)goto _LL4C;_LL4B: new_cl=0;goto _LL43;_LL4C:;_LL4D:
return flow;_LL43:;}for(0;_tmp83 != 0;_tmp83=_tmp83->tl){void*new_cmp;{struct
_tuple0 _tmp8A=({struct _tuple0 _tmp89;_tmp89.f1=p;_tmp89.f2=(void*)((struct Cyc_CfFlowInfo_TagCmp*)
_tmp83->hd)->cmp;_tmp89;});void*_tmp8B;void*_tmp8C;void*_tmp8D;void*_tmp8E;_LL4F:
_tmp8B=_tmp8A.f1;if((int)_tmp8B != 8)goto _LL51;_LL50: goto _LL52;_LL51: _tmp8C=
_tmp8A.f2;if((int)_tmp8C != 8)goto _LL53;_LL52: new_cmp=(void*)8;goto _LL4E;_LL53:
_tmp8D=_tmp8A.f1;if((int)_tmp8D != 5)goto _LL55;_tmp8E=_tmp8A.f2;if((int)_tmp8E != 
5)goto _LL55;_LL54: new_cmp=(void*)5;goto _LL4E;_LL55:;_LL56: new_cmp=(void*)10;goto
_LL4E;_LL4E:;}new_cl=({struct Cyc_List_List*_tmp8F=_region_malloc(env->r,sizeof(*
_tmp8F));_tmp8F->hd=({struct Cyc_CfFlowInfo_TagCmp*_tmp90=_region_malloc(env->r,
sizeof(*_tmp90));_tmp90->cmp=(void*)new_cmp;_tmp90->bd=(void*)((void*)((struct
Cyc_CfFlowInfo_TagCmp*)_tmp83->hd)->bd);_tmp90;});_tmp8F->tl=new_cl;_tmp8F;});}
return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp91;(_tmp91.ReachableFL).tag=1;(_tmp91.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
env->fenv,e1->loc,_tmp7F,env->all_changed,_tmp85,(void*)({struct Cyc_CfFlowInfo_TagCmps_struct*
_tmp92=_region_malloc(env->r,sizeof(*_tmp92));_tmp92[0]=({struct Cyc_CfFlowInfo_TagCmps_struct
_tmp93;_tmp93.tag=3;_tmp93.f1=new_cl;_tmp93;});_tmp92;}));(_tmp91.ReachableFL).f2=
_tmp80;(_tmp91.ReachableFL).f3=_tmp81;_tmp91;});}_LL3E:;}_LL3C:;_LL3D: return flow;
_LL39:;}_LL34:;}static struct Cyc_CfFlowInfo_NotZero_struct Cyc_NewControlFlow_mt_notzero_v={
Cyc_CfFlowInfo_NotZero,0};static struct Cyc_CfFlowInfo_UnknownZ_struct Cyc_NewControlFlow_mt_unknownz_v={
Cyc_CfFlowInfo_UnknownZ,0};static struct _tuple5 Cyc_NewControlFlow_anal_derefR(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
union Cyc_CfFlowInfo_FlowInfo_union f,struct Cyc_Absyn_Exp*e,void*r){void*_tmp96=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp97;void*_tmp98;struct Cyc_Absyn_PtrAtts _tmp99;struct
Cyc_Absyn_Conref*_tmp9A;struct Cyc_Absyn_Conref*_tmp9B;_LL58: if(_tmp96 <= (void*)4)
goto _LL5A;if(*((int*)_tmp96)!= 4)goto _LL5A;_tmp97=((struct Cyc_Absyn_PointerType_struct*)
_tmp96)->f1;_tmp98=(void*)_tmp97.elt_typ;_tmp99=_tmp97.ptr_atts;_tmp9A=_tmp99.bounds;
_tmp9B=_tmp99.zero_term;_LL59: {union Cyc_CfFlowInfo_FlowInfo_union _tmp9C=f;
struct Cyc_Dict_Dict _tmp9D;struct Cyc_List_List*_tmp9E;_LL5D: if((_tmp9C.BottomFL).tag
!= 0)goto _LL5F;_LL5E: return({struct _tuple5 _tmp9F;_tmp9F.f1=f;_tmp9F.f2=Cyc_CfFlowInfo_typ_to_absrval(
env->fenv,_tmp98,(void*)(env->fenv)->unknown_all);_tmp9F;});_LL5F: if((_tmp9C.ReachableFL).tag
!= 1)goto _LL5C;_tmp9D=(_tmp9C.ReachableFL).f1;_tmp9E=(_tmp9C.ReachableFL).f2;
_LL60: if(Cyc_Tcutil_is_bound_one(_tmp9A)){void*_tmpA0=r;struct Cyc_CfFlowInfo_Place*
_tmpA1;void*_tmpA2;_LL62: if((int)_tmpA0 != 1)goto _LL64;_LL63: goto _LL65;_LL64: if((
int)_tmpA0 != 2)goto _LL66;_LL65:{void*_tmpA3=(void*)e->annot;struct Cyc_List_List*
_tmpA4;_LL6F: if(*((void**)_tmpA3)!= Cyc_CfFlowInfo_NotZero)goto _LL71;_tmpA4=((
struct Cyc_CfFlowInfo_NotZero_struct*)_tmpA3)->f1;_LL70: if(!Cyc_CfFlowInfo_same_relns(
_tmp9E,_tmpA4))goto _LL72;goto _LL6E;_LL71:;_LL72:{void*_tmpA5=(void*)e->r;_LL74:
if(*((int*)_tmpA5)!= 25)goto _LL76;_LL75:(void*)(e->annot=(void*)((void*)({struct
Cyc_CfFlowInfo_NotZero_struct*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6[0]=({
struct Cyc_CfFlowInfo_NotZero_struct _tmpA7;_tmpA7.tag=Cyc_CfFlowInfo_NotZero;
_tmpA7.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp9E);_tmpA7;});
_tmpA6;})));goto _LL73;_LL76:;_LL77:(void*)(e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_notzero_v));
goto _LL73;_LL73:;}goto _LL6E;_LL6E:;}goto _LL61;_LL66: if(_tmpA0 <= (void*)3)goto
_LL68;if(*((int*)_tmpA0)!= 2)goto _LL68;_tmpA1=((struct Cyc_CfFlowInfo_AddressOf_struct*)
_tmpA0)->f1;_LL67:{void*_tmpA8=(void*)e->annot;struct Cyc_List_List*_tmpA9;_LL79:
if(*((void**)_tmpA8)!= Cyc_CfFlowInfo_NotZero)goto _LL7B;_tmpA9=((struct Cyc_CfFlowInfo_NotZero_struct*)
_tmpA8)->f1;_LL7A: if(!Cyc_CfFlowInfo_same_relns(_tmp9E,_tmpA9))goto _LL7C;goto
_LL78;_LL7B:;_LL7C:{void*_tmpAA=(void*)e->r;_LL7E: if(*((int*)_tmpAA)!= 25)goto
_LL80;_LL7F:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmpAC;_tmpAC.tag=Cyc_CfFlowInfo_NotZero;_tmpAC.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp9E);_tmpAC;});_tmpAB;})));goto _LL7D;_LL80:;_LL81:(void*)(e->annot=(void*)((
void*)& Cyc_NewControlFlow_mt_notzero_v));goto _LL7D;_LL7D:;}goto _LL78;_LL78:;}
return({struct _tuple5 _tmpAD;_tmpAD.f1=f;_tmpAD.f2=Cyc_CfFlowInfo_lookup_place(
_tmp9D,_tmpA1);_tmpAD;});_LL68: if((int)_tmpA0 != 0)goto _LL6A;_LL69:(void*)(e->annot=(
void*)((void*)Cyc_CfFlowInfo_IsZero));return({struct _tuple5 _tmpAE;_tmpAE.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmpAF;(
_tmpAF.BottomFL).tag=0;_tmpAF;});_tmpAE.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,
_tmp98,(void*)(env->fenv)->unknown_all);_tmpAE;});_LL6A: if(_tmpA0 <= (void*)3)
goto _LL6C;if(*((int*)_tmpA0)!= 0)goto _LL6C;_tmpA2=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmpA0)->f1;_LL6B: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpA2);goto _LL6D;
_LL6C:;_LL6D:{void*_tmpB0=(void*)e->r;_LL83: if(*((int*)_tmpB0)!= 25)goto _LL85;
_LL84:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmpB2;_tmpB2.tag=Cyc_CfFlowInfo_UnknownZ;_tmpB2.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp9E);_tmpB2;});_tmpB1;})));goto _LL82;_LL85:;_LL86:(void*)(
e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_unknownz_v));goto _LL82;_LL82:;}
goto _LL61;_LL61:;}else{void*_tmpB3=(void*)e->annot;struct Cyc_List_List*_tmpB4;
_LL88: if(*((void**)_tmpB3)!= Cyc_CfFlowInfo_UnknownZ)goto _LL8A;_tmpB4=((struct
Cyc_CfFlowInfo_UnknownZ_struct*)_tmpB3)->f1;_LL89: if(!Cyc_CfFlowInfo_same_relns(
_tmp9E,_tmpB4))goto _LL8B;goto _LL87;_LL8A:;_LL8B:{void*_tmpB5=(void*)e->r;_LL8D:
if(*((int*)_tmpB5)!= 25)goto _LL8F;_LL8E:(void*)(e->annot=(void*)((void*)({struct
Cyc_CfFlowInfo_UnknownZ_struct*_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6[0]=({
struct Cyc_CfFlowInfo_UnknownZ_struct _tmpB7;_tmpB7.tag=Cyc_CfFlowInfo_UnknownZ;
_tmpB7.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp9E);_tmpB7;});
_tmpB6;})));goto _LL8C;_LL8F:;_LL90:(void*)(e->annot=(void*)((void*)& Cyc_NewControlFlow_mt_unknownz_v));
goto _LL8C;_LL8C:;}goto _LL87;_LL87:;}{void*_tmpB8=Cyc_CfFlowInfo_initlevel(_tmp9D,
r);_LL92: if((int)_tmpB8 != 0)goto _LL94;_LL93:({void*_tmpB9=0;Cyc_Tcutil_terr(e->loc,({
const char*_tmpBA="dereference of possibly uninitialized pointer";_tag_dynforward(
_tmpBA,sizeof(char),_get_zero_arr_size_char(_tmpBA,46));}),_tag_dynforward(
_tmpB9,sizeof(void*),0));});goto _LL95;_LL94: if((int)_tmpB8 != 2)goto _LL96;_LL95:
return({struct _tuple5 _tmpBB;_tmpBB.f1=f;_tmpBB.f2=Cyc_CfFlowInfo_typ_to_absrval(
env->fenv,_tmp98,(void*)(env->fenv)->unknown_all);_tmpBB;});_LL96: if((int)_tmpB8
!= 1)goto _LL91;_LL97: return({struct _tuple5 _tmpBC;_tmpBC.f1=f;_tmpBC.f2=Cyc_CfFlowInfo_typ_to_absrval(
env->fenv,_tmp98,(void*)(env->fenv)->unknown_none);_tmpBC;});_LL91:;}_LL5C:;}
_LL5A:;_LL5B:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmpBD=
_cycalloc(sizeof(*_tmpBD));_tmpBD[0]=({struct Cyc_Core_Impossible_struct _tmpBE;
_tmpBE.tag=Cyc_Core_Impossible;_tmpBE.f1=({const char*_tmpBF="right deref of non-pointer-type";
_tag_dynforward(_tmpBF,sizeof(char),_get_zero_arr_size_char(_tmpBF,32));});
_tmpBE;});_tmpBD;}));_LL57:;}static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(
struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2){void*_tmpC0=(void*)e1->r;void*_tmpC1;struct Cyc_Absyn_Vardecl*
_tmpC2;void*_tmpC3;struct Cyc_Absyn_Vardecl*_tmpC4;void*_tmpC5;struct Cyc_Absyn_Vardecl*
_tmpC6;void*_tmpC7;struct Cyc_Absyn_Vardecl*_tmpC8;_LL99: if(*((int*)_tmpC0)!= 1)
goto _LL9B;_tmpC1=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpC0)->f2;if(_tmpC1 <= (
void*)1)goto _LL9B;if(*((int*)_tmpC1)!= 4)goto _LL9B;_tmpC2=((struct Cyc_Absyn_Pat_b_struct*)
_tmpC1)->f1;_LL9A: _tmpC4=_tmpC2;goto _LL9C;_LL9B: if(*((int*)_tmpC0)!= 1)goto _LL9D;
_tmpC3=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpC0)->f2;if(_tmpC3 <= (void*)1)
goto _LL9D;if(*((int*)_tmpC3)!= 3)goto _LL9D;_tmpC4=((struct Cyc_Absyn_Local_b_struct*)
_tmpC3)->f1;_LL9C: _tmpC6=_tmpC4;goto _LL9E;_LL9D: if(*((int*)_tmpC0)!= 1)goto _LL9F;
_tmpC5=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpC0)->f2;if(_tmpC5 <= (void*)1)
goto _LL9F;if(*((int*)_tmpC5)!= 2)goto _LL9F;_tmpC6=((struct Cyc_Absyn_Param_b_struct*)
_tmpC5)->f1;_LL9E: _tmpC8=_tmpC6;goto _LLA0;_LL9F: if(*((int*)_tmpC0)!= 1)goto _LLA1;
_tmpC7=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmpC0)->f2;if(_tmpC7 <= (void*)1)
goto _LLA1;if(*((int*)_tmpC7)!= 0)goto _LLA1;_tmpC8=((struct Cyc_Absyn_Global_b_struct*)
_tmpC7)->f1;_LLA0: if(!_tmpC8->escapes){void*_tmpC9=(void*)e2->r;void*_tmpCA;
struct Cyc_Absyn_Vardecl*_tmpCB;void*_tmpCC;struct Cyc_Absyn_Vardecl*_tmpCD;void*
_tmpCE;struct Cyc_Absyn_Vardecl*_tmpCF;void*_tmpD0;struct Cyc_Absyn_Vardecl*_tmpD1;
_LLA4: if(*((int*)_tmpC9)!= 1)goto _LLA6;_tmpCA=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC9)->f2;if(_tmpCA <= (void*)1)goto _LLA6;if(*((int*)_tmpCA)!= 4)goto _LLA6;
_tmpCB=((struct Cyc_Absyn_Pat_b_struct*)_tmpCA)->f1;_LLA5: _tmpCD=_tmpCB;goto _LLA7;
_LLA6: if(*((int*)_tmpC9)!= 1)goto _LLA8;_tmpCC=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC9)->f2;if(_tmpCC <= (void*)1)goto _LLA8;if(*((int*)_tmpCC)!= 3)goto _LLA8;
_tmpCD=((struct Cyc_Absyn_Local_b_struct*)_tmpCC)->f1;_LLA7: _tmpCF=_tmpCD;goto
_LLA9;_LLA8: if(*((int*)_tmpC9)!= 1)goto _LLAA;_tmpCE=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC9)->f2;if(_tmpCE <= (void*)1)goto _LLAA;if(*((int*)_tmpCE)!= 2)goto _LLAA;
_tmpCF=((struct Cyc_Absyn_Param_b_struct*)_tmpCE)->f1;_LLA9: _tmpD1=_tmpCF;goto
_LLAB;_LLAA: if(*((int*)_tmpC9)!= 1)goto _LLAC;_tmpD0=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmpC9)->f2;if(_tmpD0 <= (void*)1)goto _LLAC;if(*((int*)_tmpD0)!= 0)goto _LLAC;
_tmpD1=((struct Cyc_Absyn_Global_b_struct*)_tmpD0)->f1;_LLAB: if(!_tmpD1->escapes){
int found=0;{struct Cyc_List_List*_tmpD2=relns;for(0;_tmpD2 != 0;_tmpD2=_tmpD2->tl){
struct Cyc_CfFlowInfo_Reln*_tmpD3=(struct Cyc_CfFlowInfo_Reln*)_tmpD2->hd;if(
_tmpD3->vd == _tmpD1){union Cyc_CfFlowInfo_RelnOp_union _tmpD4=_tmpD3->rop;struct
Cyc_Absyn_Vardecl*_tmpD5;_LLAF: if((_tmpD4.LessSize).tag != 2)goto _LLB1;_tmpD5=(
_tmpD4.LessSize).f1;if(!(_tmpD5 == _tmpC8))goto _LLB1;_LLB0: return relns;_LLB1:;
_LLB2: continue;_LLAE:;}}}if(!found)return({struct Cyc_List_List*_tmpD6=
_region_malloc(rgn,sizeof(*_tmpD6));_tmpD6->hd=({struct Cyc_CfFlowInfo_Reln*
_tmpD7=_region_malloc(rgn,sizeof(*_tmpD7));_tmpD7->vd=_tmpD1;_tmpD7->rop=(union
Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmpD8;(_tmpD8.LessSize).tag=
2;(_tmpD8.LessSize).f1=_tmpC8;_tmpD8;});_tmpD7;});_tmpD6->tl=relns;_tmpD6;});}
return relns;_LLAC:;_LLAD: return relns;_LLA3:;}else{return relns;}_LLA1:;_LLA2:
return relns;_LL98:;}static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_NewControlFlow_may_consume_place(
struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,
struct Cyc_CfFlowInfo_Place*place,void*t,struct Cyc_List_List**ps){struct Cyc_CfFlowInfo_FlowEnv*
_tmpD9=env->fenv;struct Cyc_List_List*_tmpDA=Cyc_NewControlFlow_noalias_ptrs_rec(
env,place,t);if(ps != 0)*ps=_tmpDA;if(_tmpDA == 0)return cinfo;cinfo.may_consume=((
struct Cyc_List_List*(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(
_tmpD9->r,Cyc_CfFlowInfo_place_cmp,_tmpDA,cinfo.may_consume);return cinfo;}struct
_tuple12{union Cyc_CfFlowInfo_AbsLVal_union f1;union Cyc_CfFlowInfo_FlowInfo_union
f2;};static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_may_consume_exp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_noalias_path(e)){struct Cyc_CfFlowInfo_FlowEnv*
_tmpDB=env->fenv;union Cyc_CfFlowInfo_AbsLVal_union _tmpDD;struct _tuple7 _tmpDC=Cyc_NewControlFlow_anal_Lexp(
env,inflow,e);_tmpDD=_tmpDC.f2;{struct _tuple12 _tmpDF=({struct _tuple12 _tmpDE;
_tmpDE.f1=_tmpDD;_tmpDE.f2=inflow;_tmpDE;});union Cyc_CfFlowInfo_AbsLVal_union
_tmpE0;struct Cyc_CfFlowInfo_Place*_tmpE1;union Cyc_CfFlowInfo_FlowInfo_union
_tmpE2;struct Cyc_Dict_Dict _tmpE3;struct Cyc_List_List*_tmpE4;struct Cyc_CfFlowInfo_ConsumeInfo
_tmpE5;_LLB4: _tmpE0=_tmpDF.f1;if(((_tmpDF.f1).PlaceL).tag != 0)goto _LLB6;_tmpE1=(
_tmpE0.PlaceL).f1;_tmpE2=_tmpDF.f2;if(((_tmpDF.f2).ReachableFL).tag != 1)goto
_LLB6;_tmpE3=(_tmpE2.ReachableFL).f1;_tmpE4=(_tmpE2.ReachableFL).f2;_tmpE5=(
_tmpE2.ReachableFL).f3;_LLB5: {struct Cyc_CfFlowInfo_ConsumeInfo _tmpE6=Cyc_NewControlFlow_may_consume_place(
env,_tmpE5,_tmpE1,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,0);
return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmpE7;(_tmpE7.ReachableFL).tag=1;(_tmpE7.ReachableFL).f1=_tmpE3;(_tmpE7.ReachableFL).f2=
_tmpE4;(_tmpE7.ReachableFL).f3=_tmpE6;_tmpE7;});}_LLB6:;_LLB7:({struct Cyc_String_pa_struct
_tmpEA;_tmpEA.tag=0;_tmpEA.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmpE8[1]={& _tmpEA};Cyc_fprintf(Cyc_stderr,({const char*_tmpE9="Oops---no location for noalias_path |%s|\n";
_tag_dynforward(_tmpE9,sizeof(char),_get_zero_arr_size_char(_tmpE9,42));}),
_tag_dynforward(_tmpE8,sizeof(void*),1));}});return inflow;_LLB3:;}}return inflow;}
static struct Cyc_CfFlowInfo_ConsumeInfo Cyc_NewControlFlow_consume_place(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_CfFlowInfo_Place*p,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,struct Cyc_Dict_Dict
outdict,struct Cyc_Position_Segment*loc){if(!((int(*)(struct Cyc_Dict_Dict d,struct
Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(cinfo.consumed,p)){struct Cyc_CfFlowInfo_Place
_tmpEC;void*_tmpED;struct Cyc_List_List*_tmpEE;struct Cyc_CfFlowInfo_Place*_tmpEB=
p;_tmpEC=*_tmpEB;_tmpED=(void*)_tmpEC.root;_tmpEE=_tmpEC.fields;{void*rval=(void*)
0;if((Cyc_Dict_lookup_bool(outdict,_tmpED,& rval) && Cyc_CfFlowInfo_initlevel(
outdict,rval)!= (void*)0) && rval != (void*)0){void*_tmpEF=_tmpED;struct Cyc_Absyn_Vardecl*
_tmpF0;_LLB9: if(*((int*)_tmpEF)!= 0)goto _LLBB;_tmpF0=((struct Cyc_CfFlowInfo_VarRoot_struct*)
_tmpEF)->f1;_LLBA: {struct _dynforward_ptr _tmpF1=Cyc_Absynpp_qvar2string(_tmpF0->name);
if(_tmpEE == 0)({struct Cyc_String_pa_struct _tmpF4;_tmpF4.tag=0;_tmpF4.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmpF1);{void*_tmpF2[1]={& _tmpF4};Cyc_Tcutil_warn(
loc,({const char*_tmpF3="may clobber unique pointer %s";_tag_dynforward(_tmpF3,
sizeof(char),_get_zero_arr_size_char(_tmpF3,30));}),_tag_dynforward(_tmpF2,
sizeof(void*),1));}});else{({struct Cyc_String_pa_struct _tmpF7;_tmpF7.tag=0;
_tmpF7.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmpF1);{void*_tmpF5[1]={&
_tmpF7};Cyc_Tcutil_warn(loc,({const char*_tmpF6="may clobber unique pointer contained in %s";
_tag_dynforward(_tmpF6,sizeof(char),_get_zero_arr_size_char(_tmpF6,43));}),
_tag_dynforward(_tmpF5,sizeof(void*),1));}});}goto _LLB8;}_LLBB:;_LLBC:({void*
_tmpF8=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmpF9="warning locations not for VarRoots";_tag_dynforward(_tmpF9,
sizeof(char),_get_zero_arr_size_char(_tmpF9,35));}),_tag_dynforward(_tmpF8,
sizeof(void*),0));});_LLB8:;}}}if(env->all_changed == 0)cinfo.consumed=((struct
Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))
Cyc_Dict_rdelete)((env->fenv)->r,cinfo.consumed,p);return cinfo;}static struct Cyc_CfFlowInfo_ConsumeInfo
Cyc_NewControlFlow_consume_assignment(struct Cyc_NewControlFlow_AnalEnv*env,
struct Cyc_CfFlowInfo_Place*p,struct Cyc_CfFlowInfo_ConsumeInfo cinfo,struct Cyc_Dict_Dict
outdict,struct Cyc_Absyn_Exp*e){if(Cyc_Tcutil_is_noalias_path(e)){struct Cyc_CfFlowInfo_FlowEnv*
_tmpFA=env->fenv;struct Cyc_List_List*_tmpFB=0;cinfo=Cyc_NewControlFlow_may_consume_place(
env,cinfo,p,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(struct Cyc_List_List**)&
_tmpFB);while(_tmpFB != 0){cinfo=Cyc_NewControlFlow_consume_place(env,(struct Cyc_CfFlowInfo_Place*)((
struct Cyc_List_List*)_check_null(_tmpFB))->hd,cinfo,outdict,e->loc);_tmpFB=((
struct Cyc_List_List*)_check_null(_tmpFB))->tl;}}return cinfo;}static struct Cyc_List_List*
Cyc_NewControlFlow_noalias_ptrs_aux(struct Cyc_NewControlFlow_AnalEnv*env,struct
Cyc_CfFlowInfo_Place*p,struct Cyc_List_List*ts){struct Cyc_List_List*l=0;struct Cyc_CfFlowInfo_Place
_tmpFD;void*_tmpFE;struct Cyc_List_List*_tmpFF;struct Cyc_CfFlowInfo_Place*_tmpFC=
p;_tmpFD=*_tmpFC;_tmpFE=(void*)_tmpFD.root;_tmpFF=_tmpFD.fields;{int fld=0;for(0;
ts != 0;(fld ++,ts=ts->tl)){void*_tmp100=(void*)ts->hd;if(Cyc_Tcutil_is_noalias_pointer(
_tmp100)){struct Cyc_List_List*_tmp101=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
env->r,_tmpFF,({int _tmp104[1];_tmp104[0]=fld;((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(env->r,_tag_dynforward(
_tmp104,sizeof(int),1));}));l=((struct Cyc_List_List*(*)(struct _RegionHandle*,int(*
cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*
a,struct Cyc_List_List*b))Cyc_List_rmerge)(env->r,Cyc_CfFlowInfo_place_cmp,({
struct Cyc_List_List*_tmp102=_region_malloc(env->r,sizeof(*_tmp102));_tmp102->hd=({
struct Cyc_CfFlowInfo_Place*_tmp103=_region_malloc(env->r,sizeof(*_tmp103));
_tmp103->root=(void*)_tmpFE;_tmp103->fields=_tmp101;_tmp103;});_tmp102->tl=0;
_tmp102;}),l);}else{if(Cyc_Absyn_is_aggr_type(_tmp100)){struct Cyc_List_List*
_tmp105=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_rappend)(env->r,_tmpFF,({int _tmp108[1];_tmp108[0]=
fld;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(
env->r,_tag_dynforward(_tmp108,sizeof(int),1));}));struct Cyc_CfFlowInfo_Place*
_tmp106=({struct Cyc_CfFlowInfo_Place*_tmp107=_region_malloc(env->r,sizeof(*
_tmp107));_tmp107->root=(void*)_tmpFE;_tmp107->fields=_tmp105;_tmp107;});l=((
struct Cyc_List_List*(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,
struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*a,struct Cyc_List_List*b))Cyc_List_rmerge)(
env->r,Cyc_CfFlowInfo_place_cmp,l,Cyc_NewControlFlow_noalias_ptrs_rec(env,
_tmp106,_tmp100));}}}}return l;}struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;};
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*
env,struct Cyc_CfFlowInfo_Place*p,void*t){if(Cyc_Tcutil_is_noalias_pointer(t))
return({struct Cyc_List_List*_tmp109=_region_malloc(env->r,sizeof(*_tmp109));
_tmp109->hd=p;_tmp109->tl=0;_tmp109;});{void*_tmp10A=Cyc_Tcutil_compress(t);
struct Cyc_List_List*_tmp10B;struct Cyc_List_List*_tmp10C;struct Cyc_Absyn_AggrInfo
_tmp10D;union Cyc_Absyn_AggrInfoU_union _tmp10E;struct Cyc_Absyn_Aggrdecl**_tmp10F;
struct Cyc_List_List*_tmp110;struct Cyc_Absyn_AggrInfo _tmp111;union Cyc_Absyn_AggrInfoU_union
_tmp112;struct Cyc_Absyn_TunionFieldInfo _tmp113;union Cyc_Absyn_TunionFieldInfoU_union
_tmp114;struct Cyc_List_List*_tmp115;_LLBE: if(_tmp10A <= (void*)4)goto _LLCA;if(*((
int*)_tmp10A)!= 9)goto _LLC0;_tmp10B=((struct Cyc_Absyn_TupleType_struct*)_tmp10A)->f1;
_LLBF: {struct Cyc_List_List*_tmp116=0;while(_tmp10B != 0){_tmp116=({struct Cyc_List_List*
_tmp117=_region_malloc(env->r,sizeof(*_tmp117));_tmp117->hd=(void*)(*((struct
_tuple13*)_tmp10B->hd)).f2;_tmp117->tl=_tmp116;_tmp117;});_tmp10B=_tmp10B->tl;}
_tmp116=Cyc_List_imp_rev(_tmp116);return Cyc_NewControlFlow_noalias_ptrs_aux(env,
p,_tmp116);}_LLC0: if(*((int*)_tmp10A)!= 11)goto _LLC2;_tmp10C=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp10A)->f2;_LLC1: {struct Cyc_List_List*_tmp118=0;while(_tmp10C != 0){_tmp118=({
struct Cyc_List_List*_tmp119=_region_malloc(env->r,sizeof(*_tmp119));_tmp119->hd=(
void*)((void*)((struct Cyc_Absyn_Aggrfield*)_tmp10C->hd)->type);_tmp119->tl=
_tmp118;_tmp119;});_tmp10C=_tmp10C->tl;}_tmp118=Cyc_List_imp_rev(_tmp118);return
Cyc_NewControlFlow_noalias_ptrs_aux(env,p,_tmp118);}_LLC2: if(*((int*)_tmp10A)!= 
10)goto _LLC4;_tmp10D=((struct Cyc_Absyn_AggrType_struct*)_tmp10A)->f1;_tmp10E=
_tmp10D.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)_tmp10A)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LLC4;_tmp10F=(_tmp10E.KnownAggr).f1;_tmp110=_tmp10D.targs;_LLC3: if((*
_tmp10F)->impl == 0)return 0;else{struct Cyc_List_List*_tmp11A=0;{struct
_RegionHandle _tmp11B=_new_region("rgn");struct _RegionHandle*rgn=& _tmp11B;
_push_region(rgn);{struct Cyc_List_List*_tmp11C=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,(*_tmp10F)->tvs,_tmp110);struct Cyc_List_List*_tmp11D=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp10F)->impl))->fields;while(
_tmp11D != 0){_tmp11A=({struct Cyc_List_List*_tmp11E=_region_malloc(env->r,sizeof(*
_tmp11E));_tmp11E->hd=(void*)Cyc_Tcutil_rsubstitute(rgn,_tmp11C,(void*)((struct
Cyc_Absyn_Aggrfield*)_tmp11D->hd)->type);_tmp11E->tl=_tmp11A;_tmp11E;});_tmp11D=
_tmp11D->tl;}};_pop_region(rgn);}_tmp11A=Cyc_List_imp_rev(_tmp11A);return Cyc_NewControlFlow_noalias_ptrs_aux(
env,p,_tmp11A);}_LLC4: if(*((int*)_tmp10A)!= 10)goto _LLC6;_tmp111=((struct Cyc_Absyn_AggrType_struct*)
_tmp10A)->f1;_tmp112=_tmp111.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmp10A)->f1).aggr_info).UnknownAggr).tag != 0)goto _LLC6;_LLC5:({void*_tmp11F=0;((
int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp120="got unknown aggr in noalias_ptrs_rec";_tag_dynforward(_tmp120,
sizeof(char),_get_zero_arr_size_char(_tmp120,37));}),_tag_dynforward(_tmp11F,
sizeof(void*),0));});_LLC6: if(*((int*)_tmp10A)!= 2)goto _LLC8;_LLC7: if(Cyc_Tcutil_is_noalias_pointer_or_aggr(
t))return({struct Cyc_List_List*_tmp121=_region_malloc(env->r,sizeof(*_tmp121));
_tmp121->hd=p;_tmp121->tl=0;_tmp121;});else{return 0;}_LLC8: if(*((int*)_tmp10A)!= 
3)goto _LLCA;_tmp113=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp10A)->f1;
_tmp114=_tmp113.field_info;_tmp115=_tmp113.targs;_LLC9: {union Cyc_Absyn_TunionFieldInfoU_union
_tmp122=_tmp114;struct Cyc_Absyn_Tuniondecl*_tmp123;struct Cyc_Absyn_Tunionfield*
_tmp124;_LLCD: if((_tmp122.UnknownTunionfield).tag != 0)goto _LLCF;_LLCE:({void*
_tmp125=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp126="got unknown tunion field in noalias_ptrs_rec";_tag_dynforward(
_tmp126,sizeof(char),_get_zero_arr_size_char(_tmp126,45));}),_tag_dynforward(
_tmp125,sizeof(void*),0));});_LLCF: if((_tmp122.KnownTunionfield).tag != 1)goto
_LLCC;_tmp123=(_tmp122.KnownTunionfield).f1;_tmp124=(_tmp122.KnownTunionfield).f2;
_LLD0: {struct Cyc_List_List*_tmp127=0;{struct _RegionHandle _tmp128=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp128;_push_region(rgn);{struct Cyc_List_List*_tmp129=((
struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp123->tvs,_tmp115);struct Cyc_List_List*
_tmp12A=_tmp124->typs;while(_tmp12A != 0){_tmp127=({struct Cyc_List_List*_tmp12B=
_region_malloc(env->r,sizeof(*_tmp12B));_tmp12B->hd=(void*)Cyc_Tcutil_rsubstitute(
rgn,_tmp129,(*((struct _tuple13*)_tmp12A->hd)).f2);_tmp12B->tl=_tmp127;_tmp12B;});
_tmp12A=_tmp12A->tl;}};_pop_region(rgn);}_tmp127=Cyc_List_imp_rev(_tmp127);
return Cyc_NewControlFlow_noalias_ptrs_aux(env,p,_tmp127);}_LLCC:;}_LLCA:;_LLCB:
return 0;_LLBD:;}}static struct _tuple5 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*
fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal_union lval,struct Cyc_Absyn_Exp*
rexp,void*rval,struct Cyc_Position_Segment*loc){outflow=Cyc_CfFlowInfo_consume_unique_rvals(
loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp12C=outflow;struct Cyc_Dict_Dict
_tmp12D;struct Cyc_List_List*_tmp12E;struct Cyc_CfFlowInfo_ConsumeInfo _tmp12F;
_LLD2: if((_tmp12C.BottomFL).tag != 0)goto _LLD4;_LLD3: return({struct _tuple5 _tmp130;
_tmp130.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp131;(_tmp131.BottomFL).tag=0;_tmp131;});_tmp130.f2=rval;_tmp130;});_LLD4: if((
_tmp12C.ReachableFL).tag != 1)goto _LLD1;_tmp12D=(_tmp12C.ReachableFL).f1;_tmp12E=(
_tmp12C.ReachableFL).f2;_tmp12F=(_tmp12C.ReachableFL).f3;_LLD5: {union Cyc_CfFlowInfo_AbsLVal_union
_tmp132=lval;struct Cyc_CfFlowInfo_Place*_tmp133;_LLD7: if((_tmp132.PlaceL).tag != 
0)goto _LLD9;_tmp133=(_tmp132.PlaceL).f1;_LLD8: _tmp12F=Cyc_NewControlFlow_consume_assignment(
env,_tmp133,_tmp12F,_tmp12D,lexp);_tmp12D=Cyc_CfFlowInfo_assign_place(fenv,loc,
_tmp12D,env->all_changed,_tmp133,rval);_tmp12E=Cyc_CfFlowInfo_reln_assign_exp(
fenv->r,_tmp12E,lexp,rexp);outflow=(union Cyc_CfFlowInfo_FlowInfo_union)({union
Cyc_CfFlowInfo_FlowInfo_union _tmp134;(_tmp134.ReachableFL).tag=1;(_tmp134.ReachableFL).f1=
_tmp12D;(_tmp134.ReachableFL).f2=_tmp12E;(_tmp134.ReachableFL).f3=_tmp12F;
_tmp134;});Cyc_NewControlFlow_update_tryflow(env,outflow);return({struct _tuple5
_tmp135;_tmp135.f1=outflow;_tmp135.f2=rval;_tmp135;});_LLD9: if((_tmp132.UnknownL).tag
!= 1)goto _LLD6;_LLDA: return({struct _tuple5 _tmp136;_tmp136.f1=Cyc_NewControlFlow_use_Rval(
env,rexp->loc,outflow,rval);_tmp136.f2=rval;_tmp136;});_LLD6:;}_LLD1:;}}struct
_tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple5 Cyc_NewControlFlow_anal_Rexp(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*_tmp137=env->fenv;struct Cyc_Dict_Dict
d;struct Cyc_List_List*relns;struct Cyc_CfFlowInfo_ConsumeInfo cinfo;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp138=inflow;struct Cyc_Dict_Dict _tmp139;struct Cyc_List_List*_tmp13A;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp13B;_LLDC: if((_tmp138.BottomFL).tag != 0)goto _LLDE;_LLDD: return({struct
_tuple5 _tmp13C;_tmp13C.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp13D;(_tmp13D.BottomFL).tag=0;_tmp13D;});_tmp13C.f2=(void*)_tmp137->unknown_all;
_tmp13C;});_LLDE: if((_tmp138.ReachableFL).tag != 1)goto _LLDB;_tmp139=(_tmp138.ReachableFL).f1;
_tmp13A=(_tmp138.ReachableFL).f2;_tmp13B=(_tmp138.ReachableFL).f3;_LLDF: d=
_tmp139;relns=_tmp13A;cinfo=_tmp13B;_LLDB:;}{void*_tmp13E=(void*)e->r;struct Cyc_Absyn_Exp*
_tmp13F;void*_tmp140;struct Cyc_Absyn_Exp*_tmp141;struct Cyc_Absyn_Exp*_tmp142;
struct Cyc_Absyn_Exp*_tmp143;union Cyc_Absyn_Cnst_union _tmp144;union Cyc_Absyn_Cnst_union
_tmp145;int _tmp146;union Cyc_Absyn_Cnst_union _tmp147;void*_tmp148;struct Cyc_List_List*
_tmp149;void*_tmp14A;void*_tmp14B;struct Cyc_Absyn_Vardecl*_tmp14C;void*_tmp14D;
struct Cyc_Absyn_Vardecl*_tmp14E;void*_tmp14F;struct Cyc_Absyn_Vardecl*_tmp150;
void*_tmp151;struct Cyc_List_List*_tmp152;struct Cyc_Absyn_Exp*_tmp153;struct Cyc_Absyn_Exp*
_tmp154;struct Cyc_Core_Opt*_tmp155;struct Cyc_Core_Opt _tmp156;struct Cyc_Absyn_Exp*
_tmp157;struct Cyc_Absyn_Exp*_tmp158;struct Cyc_Core_Opt*_tmp159;struct Cyc_Absyn_Exp*
_tmp15A;struct Cyc_Absyn_Exp*_tmp15B;struct Cyc_Absyn_Exp*_tmp15C;struct Cyc_Absyn_Exp*
_tmp15D;struct Cyc_Absyn_Exp*_tmp15E;struct Cyc_List_List*_tmp15F;struct Cyc_Absyn_MallocInfo
_tmp160;int _tmp161;struct Cyc_Absyn_Exp*_tmp162;void**_tmp163;struct Cyc_Absyn_Exp*
_tmp164;int _tmp165;struct Cyc_Absyn_Exp*_tmp166;struct Cyc_Absyn_Exp*_tmp167;
struct Cyc_Absyn_Exp*_tmp168;struct Cyc_Absyn_Exp*_tmp169;struct Cyc_Absyn_Exp*
_tmp16A;struct Cyc_Absyn_Exp*_tmp16B;struct Cyc_Absyn_Exp*_tmp16C;struct
_dynforward_ptr*_tmp16D;struct Cyc_Absyn_Exp*_tmp16E;struct _dynforward_ptr*
_tmp16F;struct Cyc_Absyn_Exp*_tmp170;struct Cyc_Absyn_Exp*_tmp171;struct Cyc_Absyn_Exp*
_tmp172;struct Cyc_Absyn_Exp*_tmp173;struct Cyc_Absyn_Exp*_tmp174;struct Cyc_Absyn_Exp*
_tmp175;struct Cyc_Absyn_Exp*_tmp176;struct Cyc_Absyn_Exp*_tmp177;struct Cyc_Absyn_Exp*
_tmp178;struct Cyc_List_List*_tmp179;struct Cyc_Absyn_Tuniondecl*_tmp17A;struct Cyc_List_List*
_tmp17B;struct Cyc_List_List*_tmp17C;struct Cyc_List_List*_tmp17D;struct Cyc_Absyn_Aggrdecl*
_tmp17E;struct Cyc_Absyn_Aggrdecl _tmp17F;struct Cyc_Absyn_AggrdeclImpl*_tmp180;
struct Cyc_Absyn_AggrdeclImpl _tmp181;struct Cyc_List_List*_tmp182;struct Cyc_List_List*
_tmp183;struct Cyc_Absyn_Vardecl*_tmp184;struct Cyc_Absyn_Exp*_tmp185;struct Cyc_Absyn_Exp*
_tmp186;int _tmp187;struct Cyc_Absyn_Stmt*_tmp188;void*_tmp189;_LLE1: if(*((int*)
_tmp13E)!= 15)goto _LLE3;_tmp13F=((struct Cyc_Absyn_Cast_e_struct*)_tmp13E)->f2;
_tmp140=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp13E)->f4;if((int)_tmp140 != 2)
goto _LLE3;_LLE2: {union Cyc_CfFlowInfo_FlowInfo_union _tmp18B;void*_tmp18C;struct
_tuple5 _tmp18A=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp13F);_tmp18B=_tmp18A.f1;
_tmp18C=_tmp18A.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp18E;void*_tmp18F;
struct _tuple5 _tmp18D=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp18B,_tmp13F,
_tmp18C);_tmp18E=_tmp18D.f1;_tmp18F=_tmp18D.f2;return({struct _tuple5 _tmp190;
_tmp190.f1=_tmp18E;_tmp190.f2=_tmp18C;_tmp190;});}}_LLE3: if(*((int*)_tmp13E)!= 
15)goto _LLE5;_tmp141=((struct Cyc_Absyn_Cast_e_struct*)_tmp13E)->f2;_LLE4: _tmp142=
_tmp141;goto _LLE6;_LLE5: if(*((int*)_tmp13E)!= 13)goto _LLE7;_tmp142=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp13E)->f1;_LLE6: _tmp143=_tmp142;goto _LLE8;_LLE7: if(*((int*)_tmp13E)!= 14)goto
_LLE9;_tmp143=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp13E)->f1;_LLE8: return
Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp143);_LLE9: if(*((int*)_tmp13E)!= 0)
goto _LLEB;_tmp144=((struct Cyc_Absyn_Const_e_struct*)_tmp13E)->f1;if(((((struct
Cyc_Absyn_Const_e_struct*)_tmp13E)->f1).Null_c).tag != 6)goto _LLEB;_LLEA: goto
_LLEC;_LLEB: if(*((int*)_tmp13E)!= 0)goto _LLED;_tmp145=((struct Cyc_Absyn_Const_e_struct*)
_tmp13E)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp13E)->f1).Int_c).tag != 2)
goto _LLED;_tmp146=(_tmp145.Int_c).f2;if(_tmp146 != 0)goto _LLED;_LLEC: return({
struct _tuple5 _tmp191;_tmp191.f1=inflow;_tmp191.f2=(void*)0;_tmp191;});_LLED: if(*((
int*)_tmp13E)!= 0)goto _LLEF;_tmp147=((struct Cyc_Absyn_Const_e_struct*)_tmp13E)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp13E)->f1).Int_c).tag != 2)goto _LLEF;
_LLEE: goto _LLF0;_LLEF: if(*((int*)_tmp13E)!= 1)goto _LLF1;_tmp148=(void*)((struct
Cyc_Absyn_Var_e_struct*)_tmp13E)->f2;if(_tmp148 <= (void*)1)goto _LLF1;if(*((int*)
_tmp148)!= 1)goto _LLF1;_LLF0: return({struct _tuple5 _tmp192;_tmp192.f1=inflow;
_tmp192.f2=(void*)1;_tmp192;});_LLF1: if(*((int*)_tmp13E)!= 32)goto _LLF3;_tmp149=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp13E)->f1;if(_tmp149 != 0)goto _LLF3;_LLF2:
goto _LLF4;_LLF3: if(*((int*)_tmp13E)!= 0)goto _LLF5;_LLF4: goto _LLF6;_LLF5: if(*((
int*)_tmp13E)!= 19)goto _LLF7;_LLF6: goto _LLF8;_LLF7: if(*((int*)_tmp13E)!= 18)goto
_LLF9;_LLF8: goto _LLFA;_LLF9: if(*((int*)_tmp13E)!= 20)goto _LLFB;_LLFA: goto _LLFC;
_LLFB: if(*((int*)_tmp13E)!= 21)goto _LLFD;_LLFC: goto _LLFE;_LLFD: if(*((int*)
_tmp13E)!= 34)goto _LLFF;_LLFE: goto _LL100;_LLFF: if(*((int*)_tmp13E)!= 33)goto
_LL101;_LL100: return({struct _tuple5 _tmp193;_tmp193.f1=inflow;_tmp193.f2=(void*)
_tmp137->unknown_all;_tmp193;});_LL101: if(*((int*)_tmp13E)!= 1)goto _LL103;
_tmp14A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp13E)->f2;if(_tmp14A <= (void*)
1)goto _LL103;if(*((int*)_tmp14A)!= 0)goto _LL103;_LL102: return({struct _tuple5
_tmp194;_tmp194.f1=inflow;_tmp194.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp137,(void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp137->unknown_all);
_tmp194;});_LL103: if(*((int*)_tmp13E)!= 1)goto _LL105;_tmp14B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp13E)->f2;if(_tmp14B <= (void*)1)goto _LL105;if(*((int*)_tmp14B)!= 2)goto _LL105;
_tmp14C=((struct Cyc_Absyn_Param_b_struct*)_tmp14B)->f1;_LL104: _tmp14E=_tmp14C;
goto _LL106;_LL105: if(*((int*)_tmp13E)!= 1)goto _LL107;_tmp14D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp13E)->f2;if(_tmp14D <= (void*)1)goto _LL107;if(*((int*)_tmp14D)!= 3)goto _LL107;
_tmp14E=((struct Cyc_Absyn_Local_b_struct*)_tmp14D)->f1;_LL106: _tmp150=_tmp14E;
goto _LL108;_LL107: if(*((int*)_tmp13E)!= 1)goto _LL109;_tmp14F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp13E)->f2;if(_tmp14F <= (void*)1)goto _LL109;if(*((int*)_tmp14F)!= 4)goto _LL109;
_tmp150=((struct Cyc_Absyn_Pat_b_struct*)_tmp14F)->f1;_LL108: inflow=Cyc_NewControlFlow_may_consume_exp(
env,inflow,e);return({struct _tuple5 _tmp195;_tmp195.f1=inflow;_tmp195.f2=Cyc_Dict_lookup(
d,(void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp196=_region_malloc(env->r,
sizeof(*_tmp196));_tmp196[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp197;
_tmp197.tag=0;_tmp197.f1=_tmp150;_tmp197;});_tmp196;}));_tmp195;});_LL109: if(*((
int*)_tmp13E)!= 3)goto _LL10B;_tmp151=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp13E)->f1;_tmp152=((struct Cyc_Absyn_Primop_e_struct*)_tmp13E)->f2;_LL10A: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp199;void*_tmp19A;struct _tuple5 _tmp198=Cyc_NewControlFlow_anal_use_ints(
env,inflow,_tmp152,0);_tmp199=_tmp198.f1;_tmp19A=_tmp198.f2;{void*_tmp19B=
_tmp151;_LL148: if((int)_tmp19B != 0)goto _LL14A;_LL149: goto _LL14B;_LL14A: if((int)
_tmp19B != 2)goto _LL14C;_LL14B: Cyc_CfFlowInfo_check_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp152))->hd)->loc,_tmp199);goto _LL147;_LL14C:;
_LL14D: _tmp199=Cyc_CfFlowInfo_readthrough_unique_rvals(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp152))->hd)->loc,_tmp199);goto _LL147;_LL147:;}
return({struct _tuple5 _tmp19C;_tmp19C.f1=_tmp199;_tmp19C.f2=_tmp19A;_tmp19C;});}
_LL10B: if(*((int*)_tmp13E)!= 5)goto _LL10D;_tmp153=((struct Cyc_Absyn_Increment_e_struct*)
_tmp13E)->f1;_LL10C: {struct Cyc_List_List _tmp19D=({struct Cyc_List_List _tmp1AC;
_tmp1AC.hd=_tmp153;_tmp1AC.tl=0;_tmp1AC;});union Cyc_CfFlowInfo_FlowInfo_union
_tmp19F;struct _tuple5 _tmp19E=Cyc_NewControlFlow_anal_use_ints(env,inflow,(struct
Cyc_List_List*)& _tmp19D,0);_tmp19F=_tmp19E.f1;Cyc_CfFlowInfo_check_unique_rvals(
_tmp153->loc,_tmp19F);{union Cyc_CfFlowInfo_AbsLVal_union _tmp1A1;struct _tuple7
_tmp1A0=Cyc_NewControlFlow_anal_Lexp(env,_tmp19F,_tmp153);_tmp1A1=_tmp1A0.f2;{
struct _tuple12 _tmp1A3=({struct _tuple12 _tmp1A2;_tmp1A2.f1=_tmp1A1;_tmp1A2.f2=
_tmp19F;_tmp1A2;});union Cyc_CfFlowInfo_AbsLVal_union _tmp1A4;struct Cyc_CfFlowInfo_Place*
_tmp1A5;union Cyc_CfFlowInfo_FlowInfo_union _tmp1A6;struct Cyc_Dict_Dict _tmp1A7;
struct Cyc_List_List*_tmp1A8;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1A9;_LL14F:
_tmp1A4=_tmp1A3.f1;if(((_tmp1A3.f1).PlaceL).tag != 0)goto _LL151;_tmp1A5=(_tmp1A4.PlaceL).f1;
_tmp1A6=_tmp1A3.f2;if(((_tmp1A3.f2).ReachableFL).tag != 1)goto _LL151;_tmp1A7=(
_tmp1A6.ReachableFL).f1;_tmp1A8=(_tmp1A6.ReachableFL).f2;_tmp1A9=(_tmp1A6.ReachableFL).f3;
_LL150: _tmp1A8=Cyc_CfFlowInfo_reln_kill_exp(_tmp137->r,_tmp1A8,_tmp153);_tmp19F=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp1AA;(
_tmp1AA.ReachableFL).tag=1;(_tmp1AA.ReachableFL).f1=Cyc_CfFlowInfo_assign_place(
_tmp137,_tmp153->loc,_tmp1A7,env->all_changed,_tmp1A5,(void*)_tmp137->unknown_all);(
_tmp1AA.ReachableFL).f2=_tmp1A8;(_tmp1AA.ReachableFL).f3=_tmp1A9;_tmp1AA;});goto
_LL14E;_LL151:;_LL152: goto _LL14E;_LL14E:;}return({struct _tuple5 _tmp1AB;_tmp1AB.f1=
_tmp19F;_tmp1AB.f2=(void*)_tmp137->unknown_all;_tmp1AB;});}}_LL10D: if(*((int*)
_tmp13E)!= 4)goto _LL10F;_tmp154=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp13E)->f1;
_tmp155=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp13E)->f2;if(_tmp155 == 0)goto
_LL10F;_tmp156=*_tmp155;_tmp157=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp13E)->f3;
_LL10E: {struct Cyc_List_List _tmp1AD=({struct Cyc_List_List _tmp1BC;_tmp1BC.hd=
_tmp157;_tmp1BC.tl=0;_tmp1BC;});struct Cyc_List_List _tmp1AE=({struct Cyc_List_List
_tmp1BB;_tmp1BB.hd=_tmp154;_tmp1BB.tl=(struct Cyc_List_List*)& _tmp1AD;_tmp1BB;});
union Cyc_CfFlowInfo_FlowInfo_union _tmp1B0;struct _tuple5 _tmp1AF=Cyc_NewControlFlow_anal_use_ints(
env,inflow,(struct Cyc_List_List*)& _tmp1AE,1);_tmp1B0=_tmp1AF.f1;{union Cyc_CfFlowInfo_AbsLVal_union
_tmp1B2;struct _tuple7 _tmp1B1=Cyc_NewControlFlow_anal_Lexp(env,_tmp1B0,_tmp154);
_tmp1B2=_tmp1B1.f2;_tmp1B0=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,_tmp1B0);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp1B3=_tmp1B0;struct Cyc_Dict_Dict _tmp1B4;
struct Cyc_List_List*_tmp1B5;struct Cyc_CfFlowInfo_ConsumeInfo _tmp1B6;_LL154: if((
_tmp1B3.ReachableFL).tag != 1)goto _LL156;_tmp1B4=(_tmp1B3.ReachableFL).f1;_tmp1B5=(
_tmp1B3.ReachableFL).f2;_tmp1B6=(_tmp1B3.ReachableFL).f3;_LL155:{union Cyc_CfFlowInfo_AbsLVal_union
_tmp1B7=_tmp1B2;struct Cyc_CfFlowInfo_Place*_tmp1B8;_LL159: if((_tmp1B7.PlaceL).tag
!= 0)goto _LL15B;_tmp1B8=(_tmp1B7.PlaceL).f1;_LL15A: _tmp1B6=Cyc_NewControlFlow_consume_assignment(
env,_tmp1B8,_tmp1B6,_tmp1B4,_tmp154);_tmp1B5=Cyc_CfFlowInfo_reln_kill_exp(
_tmp137->r,_tmp1B5,_tmp154);_tmp1B4=Cyc_CfFlowInfo_assign_place(_tmp137,_tmp154->loc,
_tmp1B4,env->all_changed,_tmp1B8,(void*)_tmp137->unknown_all);_tmp1B0=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp1B9;(_tmp1B9.ReachableFL).tag=1;(_tmp1B9.ReachableFL).f1=
_tmp1B4;(_tmp1B9.ReachableFL).f2=_tmp1B5;(_tmp1B9.ReachableFL).f3=_tmp1B6;
_tmp1B9;});goto _LL158;_LL15B: if((_tmp1B7.UnknownL).tag != 1)goto _LL158;_LL15C:
goto _LL158;_LL158:;}goto _LL153;_LL156:;_LL157: goto _LL153;_LL153:;}return({struct
_tuple5 _tmp1BA;_tmp1BA.f1=_tmp1B0;_tmp1BA.f2=(void*)_tmp137->unknown_all;_tmp1BA;});}}
_LL10F: if(*((int*)_tmp13E)!= 4)goto _LL111;_tmp158=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp13E)->f1;_tmp159=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp13E)->f2;if(
_tmp159 != 0)goto _LL111;_tmp15A=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp13E)->f3;
_LL110: {struct Cyc_Dict_Dict*_tmp1BD=env->all_changed;union Cyc_CfFlowInfo_FlowInfo_union
_tmp1BF;union Cyc_CfFlowInfo_AbsLVal_union _tmp1C0;struct _tuple7 _tmp1BE=Cyc_NewControlFlow_anal_Lexp(
env,inflow,_tmp158);_tmp1BF=_tmp1BE.f1;_tmp1C0=_tmp1BE.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1C2;void*_tmp1C3;struct _tuple5 _tmp1C1=Cyc_NewControlFlow_anal_Rexp(env,
_tmp1BF,_tmp15A);_tmp1C2=_tmp1C1.f1;_tmp1C3=_tmp1C1.f2;{struct _tuple5 _tmp1C4=Cyc_NewControlFlow_do_assign(
_tmp137,env,_tmp1C2,_tmp158,_tmp1C0,_tmp15A,_tmp1C3,e->loc);struct Cyc_Dict_Dict*
_tmp1C5=env->all_changed;env->all_changed=_tmp1BD;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1C7;void*_tmp1C8;struct _tuple5 _tmp1C6=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp15A);_tmp1C7=_tmp1C6.f1;_tmp1C8=_tmp1C6.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1CA;union Cyc_CfFlowInfo_AbsLVal_union _tmp1CB;struct _tuple7 _tmp1C9=Cyc_NewControlFlow_anal_Lexp(
env,_tmp1C7,_tmp158);_tmp1CA=_tmp1C9.f1;_tmp1CB=_tmp1C9.f2;{struct _tuple5 _tmp1CC=
Cyc_NewControlFlow_do_assign(_tmp137,env,_tmp1CA,_tmp158,_tmp1CB,_tmp15A,_tmp1C8,
e->loc);if(_tmp1BD == 0)env->all_changed=0;else{*((struct Cyc_Dict_Dict*)
_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*((struct Cyc_Dict_Dict*)
_check_null(_tmp1C5)),*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);}{
struct _tuple5 _tmp1CD=Cyc_CfFlowInfo_join_flow_and_rval(_tmp137,_tmp1BD,_tmp1C4,
_tmp1CC,1);return _tmp1CD;}}}}}}}_LL111: if(*((int*)_tmp13E)!= 9)goto _LL113;
_tmp15B=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp13E)->f1;_tmp15C=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp13E)->f2;_LL112: {union Cyc_CfFlowInfo_FlowInfo_union _tmp1CF;void*_tmp1D0;
struct _tuple5 _tmp1CE=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp15B);_tmp1CF=
_tmp1CE.f1;_tmp1D0=_tmp1CE.f2;_tmp1CF=Cyc_CfFlowInfo_drop_unique_rvals(_tmp15B->loc,
_tmp1CF);return Cyc_NewControlFlow_anal_Rexp(env,_tmp1CF,_tmp15C);}_LL113: if(*((
int*)_tmp13E)!= 12)goto _LL115;_tmp15D=((struct Cyc_Absyn_Throw_e_struct*)_tmp13E)->f1;
_LL114: {union Cyc_CfFlowInfo_FlowInfo_union _tmp1D2;void*_tmp1D3;struct _tuple5
_tmp1D1=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp15D);_tmp1D2=_tmp1D1.f1;
_tmp1D3=_tmp1D1.f2;_tmp1D2=Cyc_CfFlowInfo_consume_unique_rvals(_tmp15D->loc,
_tmp1D2);Cyc_NewControlFlow_use_Rval(env,_tmp15D->loc,_tmp1D2,_tmp1D3);return({
struct _tuple5 _tmp1D4;_tmp1D4.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1D5;(_tmp1D5.BottomFL).tag=0;_tmp1D5;});_tmp1D4.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp137,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp137->unknown_all);
_tmp1D4;});}_LL115: if(*((int*)_tmp13E)!= 11)goto _LL117;_tmp15E=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp13E)->f1;_tmp15F=((struct Cyc_Absyn_FnCall_e_struct*)_tmp13E)->f2;_LL116: {
struct _RegionHandle _tmp1D6=_new_region("temp");struct _RegionHandle*temp=& _tmp1D6;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1D9;struct Cyc_List_List*
_tmp1DA;struct _tuple11 _tmp1D8=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,({struct Cyc_List_List*_tmp1D7=_region_malloc(temp,sizeof(*_tmp1D7));
_tmp1D7->hd=_tmp15E;_tmp1D7->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp15F);_tmp1D7;}),1);_tmp1D9=
_tmp1D8.f1;_tmp1DA=_tmp1D8.f2;_tmp1D9=Cyc_CfFlowInfo_consume_unique_rvals(e->loc,
_tmp1D9);{union Cyc_CfFlowInfo_FlowInfo_union _tmp1DB=Cyc_NewControlFlow_use_Rval(
env,_tmp15E->loc,_tmp1D9,(void*)((struct Cyc_List_List*)_check_null(_tmp1DA))->hd);
_tmp1DA=_tmp1DA->tl;{struct Cyc_List_List*init_params=0;{void*_tmp1DC=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp15E->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp1DD;void*_tmp1DE;_LL15E: if(_tmp1DC <= (void*)4)goto _LL160;if(*((int*)_tmp1DC)
!= 4)goto _LL160;_tmp1DD=((struct Cyc_Absyn_PointerType_struct*)_tmp1DC)->f1;
_tmp1DE=(void*)_tmp1DD.elt_typ;_LL15F:{void*_tmp1DF=Cyc_Tcutil_compress(_tmp1DE);
struct Cyc_Absyn_FnInfo _tmp1E0;struct Cyc_List_List*_tmp1E1;_LL163: if(_tmp1DF <= (
void*)4)goto _LL165;if(*((int*)_tmp1DF)!= 8)goto _LL165;_tmp1E0=((struct Cyc_Absyn_FnType_struct*)
_tmp1DF)->f1;_tmp1E1=_tmp1E0.attributes;_LL164: for(0;_tmp1E1 != 0;_tmp1E1=_tmp1E1->tl){
void*_tmp1E2=(void*)_tmp1E1->hd;int _tmp1E3;_LL168: if(_tmp1E2 <= (void*)17)goto
_LL16A;if(*((int*)_tmp1E2)!= 4)goto _LL16A;_tmp1E3=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp1E2)->f1;_LL169: init_params=({struct Cyc_List_List*_tmp1E4=_region_malloc(
temp,sizeof(*_tmp1E4));_tmp1E4->hd=(void*)_tmp1E3;_tmp1E4->tl=init_params;
_tmp1E4;});goto _LL167;_LL16A:;_LL16B: goto _LL167;_LL167:;}goto _LL162;_LL165:;
_LL166:({void*_tmp1E5=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr
ap))Cyc_Tcutil_impos)(({const char*_tmp1E6="anal_Rexp: bad function type";
_tag_dynforward(_tmp1E6,sizeof(char),_get_zero_arr_size_char(_tmp1E6,29));}),
_tag_dynforward(_tmp1E5,sizeof(void*),0));});_LL162:;}goto _LL15D;_LL160:;_LL161:({
void*_tmp1E7=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1E8="anal_Rexp: bad function type";_tag_dynforward(_tmp1E8,sizeof(
char),_get_zero_arr_size_char(_tmp1E8,29));}),_tag_dynforward(_tmp1E7,sizeof(
void*),0));});_LL15D:;}{int i=1;for(0;_tmp1DA != 0;(((_tmp1DA=_tmp1DA->tl,_tmp15F=((
struct Cyc_List_List*)_check_null(_tmp15F))->tl)),++ i)){if(!((int(*)(struct Cyc_List_List*
l,int x))Cyc_List_memq)(init_params,i)){_tmp1DB=Cyc_NewControlFlow_use_Rval(env,((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp15F))->hd)->loc,
_tmp1DB,(void*)_tmp1DA->hd);continue;}{union Cyc_CfFlowInfo_FlowInfo_union _tmp1E9=
_tmp1D9;struct Cyc_Dict_Dict _tmp1EA;_LL16D: if((_tmp1E9.BottomFL).tag != 0)goto
_LL16F;_LL16E: goto _LL16C;_LL16F: if((_tmp1E9.ReachableFL).tag != 1)goto _LL16C;
_tmp1EA=(_tmp1E9.ReachableFL).f1;_LL170: if(Cyc_CfFlowInfo_initlevel(_tmp1EA,(
void*)_tmp1DA->hd)== (void*)0)({void*_tmp1EB=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp15F))->hd)->loc,({const char*_tmp1EC="expression may not be initialized";
_tag_dynforward(_tmp1EC,sizeof(char),_get_zero_arr_size_char(_tmp1EC,34));}),
_tag_dynforward(_tmp1EB,sizeof(void*),0));});{union Cyc_CfFlowInfo_FlowInfo_union
_tmp1ED=_tmp1DB;struct Cyc_Dict_Dict _tmp1EE;struct Cyc_List_List*_tmp1EF;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp1F0;_LL172: if((_tmp1ED.BottomFL).tag != 0)goto _LL174;_LL173: goto _LL171;_LL174:
if((_tmp1ED.ReachableFL).tag != 1)goto _LL171;_tmp1EE=(_tmp1ED.ReachableFL).f1;
_tmp1EF=(_tmp1ED.ReachableFL).f2;_tmp1F0=(_tmp1ED.ReachableFL).f3;_LL175: {
struct Cyc_Dict_Dict _tmp1F1=Cyc_CfFlowInfo_escape_deref(_tmp137,_tmp1EE,env->all_changed,(
void*)_tmp1DA->hd);{void*_tmp1F2=(void*)_tmp1DA->hd;struct Cyc_CfFlowInfo_Place*
_tmp1F3;_LL177: if(_tmp1F2 <= (void*)3)goto _LL179;if(*((int*)_tmp1F2)!= 2)goto
_LL179;_tmp1F3=((struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp1F2)->f1;_LL178:{
void*_tmp1F4=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp15F))->hd)->topt))->v);
struct Cyc_Absyn_PtrInfo _tmp1F5;void*_tmp1F6;_LL17C: if(_tmp1F4 <= (void*)4)goto
_LL17E;if(*((int*)_tmp1F4)!= 4)goto _LL17E;_tmp1F5=((struct Cyc_Absyn_PointerType_struct*)
_tmp1F4)->f1;_tmp1F6=(void*)_tmp1F5.elt_typ;_LL17D: _tmp1F1=Cyc_CfFlowInfo_assign_place(
_tmp137,((struct Cyc_Absyn_Exp*)_tmp15F->hd)->loc,_tmp1F1,env->all_changed,
_tmp1F3,Cyc_CfFlowInfo_typ_to_absrval(_tmp137,_tmp1F6,(void*)_tmp137->esc_all));
goto _LL17B;_LL17E:;_LL17F:({void*_tmp1F7=0;((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1F8="anal_Rexp:bad type for initialized arg";
_tag_dynforward(_tmp1F8,sizeof(char),_get_zero_arr_size_char(_tmp1F8,39));}),
_tag_dynforward(_tmp1F7,sizeof(void*),0));});_LL17B:;}goto _LL176;_LL179:;_LL17A:
goto _LL176;_LL176:;}_tmp1DB=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1F9;(_tmp1F9.ReachableFL).tag=1;(_tmp1F9.ReachableFL).f1=_tmp1F1;(_tmp1F9.ReachableFL).f2=
_tmp1EF;(_tmp1F9.ReachableFL).f3=_tmp1F0;_tmp1F9;});goto _LL171;}_LL171:;}goto
_LL16C;_LL16C:;}}}if(Cyc_Tcutil_is_noreturn((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp15E->topt))->v)){struct _tuple5 _tmp1FC=({struct _tuple5 _tmp1FA;
_tmp1FA.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp1FB;(_tmp1FB.BottomFL).tag=0;_tmp1FB;});_tmp1FA.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp137,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp137->unknown_all);
_tmp1FA;});_npop_handler(0);return _tmp1FC;}else{struct _tuple5 _tmp1FE=({struct
_tuple5 _tmp1FD;_tmp1FD.f1=_tmp1DB;_tmp1FD.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp137,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp137->unknown_all);
_tmp1FD;});_npop_handler(0);return _tmp1FE;}}}};_pop_region(temp);}_LL117: if(*((
int*)_tmp13E)!= 35)goto _LL119;_tmp160=((struct Cyc_Absyn_Malloc_e_struct*)_tmp13E)->f1;
_tmp161=_tmp160.is_calloc;_tmp162=_tmp160.rgn;_tmp163=_tmp160.elt_type;_tmp164=
_tmp160.num_elts;_tmp165=_tmp160.fat_result;_LL118: {void*root=(void*)({struct
Cyc_CfFlowInfo_MallocPt_struct*_tmp210=_region_malloc(_tmp137->r,sizeof(*_tmp210));
_tmp210[0]=({struct Cyc_CfFlowInfo_MallocPt_struct _tmp211;_tmp211.tag=1;_tmp211.f1=
_tmp164;_tmp211.f2=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp211;});_tmp210;});struct Cyc_CfFlowInfo_Place*place=({struct Cyc_CfFlowInfo_Place*
_tmp20F=_region_malloc(_tmp137->r,sizeof(*_tmp20F));_tmp20F->root=(void*)root;
_tmp20F->fields=0;_tmp20F;});void*rval=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp20D=_region_malloc(_tmp137->r,sizeof(*_tmp20D));_tmp20D[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp20E;_tmp20E.tag=2;_tmp20E.f1=place;_tmp20E;});_tmp20D;});void*place_val=
_tmp165?(void*)1: Cyc_CfFlowInfo_typ_to_absrval(_tmp137,*((void**)_check_null(
_tmp163)),(void*)_tmp137->unknown_none);union Cyc_CfFlowInfo_FlowInfo_union
outflow;Cyc_CfFlowInfo_update_place_set(env->all_changed,place,0);if(_tmp162 != 0){
struct _RegionHandle _tmp1FF=_new_region("temp");struct _RegionHandle*temp=& _tmp1FF;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp202;struct Cyc_List_List*
_tmp203;struct _tuple11 _tmp201=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,({struct Cyc_Absyn_Exp*_tmp200[2];_tmp200[1]=_tmp164;_tmp200[0]=(struct Cyc_Absyn_Exp*)
_tmp162;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dynforward_ptr))
Cyc_List_rlist)(temp,_tag_dynforward(_tmp200,sizeof(struct Cyc_Absyn_Exp*),2));}),
0);_tmp202=_tmp201.f1;_tmp203=_tmp201.f2;outflow=_tmp202;};_pop_region(temp);}
else{union Cyc_CfFlowInfo_FlowInfo_union _tmp205;struct _tuple5 _tmp204=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp164);_tmp205=_tmp204.f1;outflow=_tmp205;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp164->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp206=outflow;struct
Cyc_Dict_Dict _tmp207;struct Cyc_List_List*_tmp208;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp209;_LL181: if((_tmp206.BottomFL).tag != 0)goto _LL183;_LL182: return({struct
_tuple5 _tmp20A;_tmp20A.f1=outflow;_tmp20A.f2=rval;_tmp20A;});_LL183: if((_tmp206.ReachableFL).tag
!= 1)goto _LL180;_tmp207=(_tmp206.ReachableFL).f1;_tmp208=(_tmp206.ReachableFL).f2;
_tmp209=(_tmp206.ReachableFL).f3;_LL184: return({struct _tuple5 _tmp20B;_tmp20B.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp20C;(
_tmp20C.ReachableFL).tag=1;(_tmp20C.ReachableFL).f1=Cyc_Dict_insert(_tmp207,root,
place_val);(_tmp20C.ReachableFL).f2=_tmp208;(_tmp20C.ReachableFL).f3=_tmp209;
_tmp20C;});_tmp20B.f2=rval;_tmp20B;});_LL180:;}}_LL119: if(*((int*)_tmp13E)!= 36)
goto _LL11B;_tmp166=((struct Cyc_Absyn_Swap_e_struct*)_tmp13E)->f1;_tmp167=((
struct Cyc_Absyn_Swap_e_struct*)_tmp13E)->f2;_LL11A: {void*left_rval;void*
right_rval;union Cyc_CfFlowInfo_FlowInfo_union outflow;{struct _RegionHandle _tmp212=
_new_region("temp");struct _RegionHandle*temp=& _tmp212;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp215;struct Cyc_List_List*_tmp216;struct _tuple11
_tmp214=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_Absyn_Exp*
_tmp213[2];_tmp213[1]=_tmp167;_tmp213[0]=_tmp166;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(temp,_tag_dynforward(
_tmp213,sizeof(struct Cyc_Absyn_Exp*),2));}),0);_tmp215=_tmp214.f1;_tmp216=
_tmp214.f2;left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp216))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp216->tl))->hd;outflow=
_tmp215;};_pop_region(temp);}Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
outflow);{union Cyc_CfFlowInfo_AbsLVal_union _tmp218;struct _tuple7 _tmp217=Cyc_NewControlFlow_anal_Lexp(
env,outflow,_tmp166);_tmp218=_tmp217.f2;{union Cyc_CfFlowInfo_AbsLVal_union
_tmp21A;struct _tuple7 _tmp219=Cyc_NewControlFlow_anal_Lexp(env,outflow,_tmp167);
_tmp21A=_tmp219.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp21B=outflow;struct Cyc_Dict_Dict
_tmp21C;struct Cyc_List_List*_tmp21D;struct Cyc_CfFlowInfo_ConsumeInfo _tmp21E;
_LL186: if((_tmp21B.ReachableFL).tag != 1)goto _LL188;_tmp21C=(_tmp21B.ReachableFL).f1;
_tmp21D=(_tmp21B.ReachableFL).f2;_tmp21E=(_tmp21B.ReachableFL).f3;_LL187:{union
Cyc_CfFlowInfo_AbsLVal_union _tmp21F=_tmp218;struct Cyc_CfFlowInfo_Place*_tmp220;
_LL18B: if((_tmp21F.PlaceL).tag != 0)goto _LL18D;_tmp220=(_tmp21F.PlaceL).f1;_LL18C:
_tmp21C=Cyc_CfFlowInfo_assign_place(_tmp137,_tmp166->loc,_tmp21C,env->all_changed,
_tmp220,right_rval);goto _LL18A;_LL18D: if((_tmp21F.UnknownL).tag != 1)goto _LL18A;
_LL18E: goto _LL18A;_LL18A:;}{union Cyc_CfFlowInfo_AbsLVal_union _tmp221=_tmp21A;
struct Cyc_CfFlowInfo_Place*_tmp222;_LL190: if((_tmp221.PlaceL).tag != 0)goto _LL192;
_tmp222=(_tmp221.PlaceL).f1;_LL191: _tmp21C=Cyc_CfFlowInfo_assign_place(_tmp137,
_tmp167->loc,_tmp21C,env->all_changed,_tmp222,left_rval);goto _LL18F;_LL192: if((
_tmp221.UnknownL).tag != 1)goto _LL18F;_LL193: goto _LL18F;_LL18F:;}_tmp21D=Cyc_CfFlowInfo_reln_kill_exp(
_tmp137->r,_tmp21D,_tmp166);_tmp21D=Cyc_CfFlowInfo_reln_kill_exp(_tmp137->r,
_tmp21D,_tmp167);outflow=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp223;(_tmp223.ReachableFL).tag=1;(_tmp223.ReachableFL).f1=_tmp21C;(_tmp223.ReachableFL).f2=
_tmp21D;(_tmp223.ReachableFL).f3=_tmp21E;_tmp223;});goto _LL185;_LL188:;_LL189:
goto _LL185;_LL185:;}return({struct _tuple5 _tmp224;_tmp224.f1=outflow;_tmp224.f2=(
void*)_tmp137->unknown_all;_tmp224;});}}}_LL11B: if(*((int*)_tmp13E)!= 17)goto
_LL11D;_tmp168=((struct Cyc_Absyn_New_e_struct*)_tmp13E)->f1;_tmp169=((struct Cyc_Absyn_New_e_struct*)
_tmp13E)->f2;_LL11C: {void*root=(void*)({struct Cyc_CfFlowInfo_MallocPt_struct*
_tmp237=_region_malloc(_tmp137->r,sizeof(*_tmp237));_tmp237[0]=({struct Cyc_CfFlowInfo_MallocPt_struct
_tmp238;_tmp238.tag=1;_tmp238.f1=_tmp169;_tmp238.f2=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v);_tmp238;});_tmp237;});struct Cyc_CfFlowInfo_Place*place=({
struct Cyc_CfFlowInfo_Place*_tmp236=_region_malloc(_tmp137->r,sizeof(*_tmp236));
_tmp236->root=(void*)root;_tmp236->fields=0;_tmp236;});void*rval=(void*)({struct
Cyc_CfFlowInfo_AddressOf_struct*_tmp234=_region_malloc(_tmp137->r,sizeof(*
_tmp234));_tmp234[0]=({struct Cyc_CfFlowInfo_AddressOf_struct _tmp235;_tmp235.tag=
2;_tmp235.f1=place;_tmp235;});_tmp234;});Cyc_CfFlowInfo_update_place_set(env->all_changed,
place,0);{union Cyc_CfFlowInfo_FlowInfo_union outflow;void*place_val;if(_tmp168 != 
0){struct _RegionHandle _tmp225=_new_region("temp");struct _RegionHandle*temp=&
_tmp225;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp228;struct Cyc_List_List*
_tmp229;struct _tuple11 _tmp227=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,({struct Cyc_Absyn_Exp*_tmp226[2];_tmp226[1]=_tmp169;_tmp226[0]=(struct Cyc_Absyn_Exp*)
_tmp168;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dynforward_ptr))
Cyc_List_rlist)(temp,_tag_dynforward(_tmp226,sizeof(struct Cyc_Absyn_Exp*),2));}),
0);_tmp228=_tmp227.f1;_tmp229=_tmp227.f2;outflow=_tmp228;place_val=(void*)((
struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp229))->tl))->hd;};
_pop_region(temp);}else{union Cyc_CfFlowInfo_FlowInfo_union _tmp22B;void*_tmp22C;
struct _tuple5 _tmp22A=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp169);_tmp22B=
_tmp22A.f1;_tmp22C=_tmp22A.f2;outflow=_tmp22B;place_val=_tmp22C;}outflow=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp169->loc,outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp22D=outflow;struct
Cyc_Dict_Dict _tmp22E;struct Cyc_List_List*_tmp22F;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp230;_LL195: if((_tmp22D.BottomFL).tag != 0)goto _LL197;_LL196: return({struct
_tuple5 _tmp231;_tmp231.f1=outflow;_tmp231.f2=rval;_tmp231;});_LL197: if((_tmp22D.ReachableFL).tag
!= 1)goto _LL194;_tmp22E=(_tmp22D.ReachableFL).f1;_tmp22F=(_tmp22D.ReachableFL).f2;
_tmp230=(_tmp22D.ReachableFL).f3;_LL198: return({struct _tuple5 _tmp232;_tmp232.f1=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp233;(
_tmp233.ReachableFL).tag=1;(_tmp233.ReachableFL).f1=Cyc_Dict_insert(_tmp22E,root,
place_val);(_tmp233.ReachableFL).f2=_tmp22F;(_tmp233.ReachableFL).f3=_tmp230;
_tmp233;});_tmp232.f2=rval;_tmp232;});_LL194:;}}}_LL11D: if(*((int*)_tmp13E)!= 16)
goto _LL11F;_tmp16A=((struct Cyc_Absyn_Address_e_struct*)_tmp13E)->f1;_LL11E: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp23A;struct _tuple5 _tmp239=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp16A);_tmp23A=_tmp239.f1;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp23C;
struct _tuple6 _tmp23B=Cyc_CfFlowInfo_save_consume_info(env->fenv,_tmp23A,0);
_tmp23C=_tmp23B.f1;{union Cyc_CfFlowInfo_FlowInfo_union _tmp23E;union Cyc_CfFlowInfo_AbsLVal_union
_tmp23F;struct _tuple7 _tmp23D=Cyc_NewControlFlow_anal_Lexp(env,inflow,_tmp16A);
_tmp23E=_tmp23D.f1;_tmp23F=_tmp23D.f2;_tmp23E=Cyc_CfFlowInfo_restore_consume_info(
_tmp23E,_tmp23C);{union Cyc_CfFlowInfo_AbsLVal_union _tmp240=_tmp23F;struct Cyc_CfFlowInfo_Place*
_tmp241;_LL19A: if((_tmp240.UnknownL).tag != 1)goto _LL19C;_LL19B: return({struct
_tuple5 _tmp242;_tmp242.f1=_tmp23E;_tmp242.f2=(void*)1;_tmp242;});_LL19C: if((
_tmp240.PlaceL).tag != 0)goto _LL199;_tmp241=(_tmp240.PlaceL).f1;_LL19D: return({
struct _tuple5 _tmp243;_tmp243.f1=_tmp23E;_tmp243.f2=(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp244=_region_malloc(env->r,sizeof(*_tmp244));_tmp244[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp245;_tmp245.tag=2;_tmp245.f1=_tmp241;_tmp245;});_tmp244;});_tmp243;});_LL199:;}}}}
_LL11F: if(*((int*)_tmp13E)!= 22)goto _LL121;_tmp16B=((struct Cyc_Absyn_Deref_e_struct*)
_tmp13E)->f1;_LL120: {union Cyc_CfFlowInfo_FlowInfo_union _tmp247;void*_tmp248;
struct _tuple5 _tmp246=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp16B);_tmp247=
_tmp246.f1;_tmp248=_tmp246.f2;_tmp247=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp247);return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp247,_tmp16B,_tmp248);}
_LL121: if(*((int*)_tmp13E)!= 23)goto _LL123;_tmp16C=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp13E)->f1;_tmp16D=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp13E)->f2;_LL122: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp24A;void*_tmp24B;struct _tuple5 _tmp249=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp16C);_tmp24A=_tmp249.f1;_tmp24B=_tmp249.f2;_tmp24A=Cyc_CfFlowInfo_drop_unique_rvals(
e->loc,_tmp24A);_tmp24A=Cyc_NewControlFlow_may_consume_exp(env,_tmp24A,e);if(Cyc_Absyn_is_union_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp16C->topt))->v))return({struct
_tuple5 _tmp24C;_tmp24C.f1=_tmp24A;_tmp24C.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp137,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp137->unknown_all);
_tmp24C;});{void*_tmp24D=_tmp24B;struct _dynforward_ptr _tmp24E;_LL19F: if(_tmp24D
<= (void*)3)goto _LL1A1;if(*((int*)_tmp24D)!= 4)goto _LL1A1;_tmp24E=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp24D)->f1;_LL1A0: {int _tmp24F=Cyc_NewControlFlow_get_field_index((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp16C->topt))->v,_tmp16D);return({struct
_tuple5 _tmp250;_tmp250.f1=_tmp24A;_tmp250.f2=*((void**)
_check_dynforward_subscript(_tmp24E,sizeof(void*),_tmp24F));_tmp250;});}_LL1A1:;
_LL1A2:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp251=_cycalloc(
sizeof(*_tmp251));_tmp251[0]=({struct Cyc_Core_Impossible_struct _tmp252;_tmp252.tag=
Cyc_Core_Impossible;_tmp252.f1=(struct _dynforward_ptr)({struct Cyc_String_pa_struct
_tmp255;_tmp255.tag=0;_tmp255.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_exp2string(e));{void*_tmp253[1]={& _tmp255};Cyc_aprintf(({const char*
_tmp254="anal_Rexp: AggrMember: %s";_tag_dynforward(_tmp254,sizeof(char),
_get_zero_arr_size_char(_tmp254,26));}),_tag_dynforward(_tmp253,sizeof(void*),1));}});
_tmp252;});_tmp251;}));_LL19E:;}}_LL123: if(*((int*)_tmp13E)!= 24)goto _LL125;
_tmp16E=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp13E)->f1;_tmp16F=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp13E)->f2;_LL124: {union Cyc_CfFlowInfo_FlowInfo_union _tmp257;void*_tmp258;
struct _tuple5 _tmp256=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp16E);_tmp257=
_tmp256.f1;_tmp258=_tmp256.f2;_tmp257=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp257);{union Cyc_CfFlowInfo_FlowInfo_union _tmp25A;void*_tmp25B;struct _tuple5
_tmp259=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp257,_tmp16E,_tmp258);
_tmp25A=_tmp259.f1;_tmp25B=_tmp259.f2;{void*_tmp25C=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp16E->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp25D;void*_tmp25E;_LL1A4: if(_tmp25C <= (void*)4)goto _LL1A6;if(*((int*)_tmp25C)
!= 4)goto _LL1A6;_tmp25D=((struct Cyc_Absyn_PointerType_struct*)_tmp25C)->f1;
_tmp25E=(void*)_tmp25D.elt_typ;_LL1A5: if(Cyc_Absyn_is_union_type(_tmp25E))return({
struct _tuple5 _tmp25F;_tmp25F.f1=_tmp25A;_tmp25F.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp137,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp137->unknown_all);
_tmp25F;});{void*_tmp260=_tmp25B;struct _dynforward_ptr _tmp261;_LL1A9: if(_tmp260
<= (void*)3)goto _LL1AB;if(*((int*)_tmp260)!= 4)goto _LL1AB;_tmp261=((struct Cyc_CfFlowInfo_Aggregate_struct*)
_tmp260)->f1;_LL1AA: {int _tmp262=Cyc_NewControlFlow_get_field_index(_tmp25E,
_tmp16F);return({struct _tuple5 _tmp263;_tmp263.f1=_tmp25A;_tmp263.f2=*((void**)
_check_dynforward_subscript(_tmp261,sizeof(void*),_tmp262));_tmp263;});}_LL1AB:;
_LL1AC:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp264=_cycalloc(
sizeof(*_tmp264));_tmp264[0]=({struct Cyc_Core_Impossible_struct _tmp265;_tmp265.tag=
Cyc_Core_Impossible;_tmp265.f1=({const char*_tmp266="anal_Rexp: AggrArrow";
_tag_dynforward(_tmp266,sizeof(char),_get_zero_arr_size_char(_tmp266,21));});
_tmp265;});_tmp264;}));_LL1A8:;}_LL1A6:;_LL1A7:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp267=_cycalloc(sizeof(*_tmp267));_tmp267[0]=({struct Cyc_Core_Impossible_struct
_tmp268;_tmp268.tag=Cyc_Core_Impossible;_tmp268.f1=({const char*_tmp269="anal_Rexp: AggrArrow ptr";
_tag_dynforward(_tmp269,sizeof(char),_get_zero_arr_size_char(_tmp269,25));});
_tmp268;});_tmp267;}));_LL1A3:;}}}_LL125: if(*((int*)_tmp13E)!= 6)goto _LL127;
_tmp170=((struct Cyc_Absyn_Conditional_e_struct*)_tmp13E)->f1;_tmp171=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp13E)->f2;_tmp172=((struct Cyc_Absyn_Conditional_e_struct*)_tmp13E)->f3;_LL126: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp26B;union Cyc_CfFlowInfo_FlowInfo_union
_tmp26C;struct _tuple8 _tmp26A=Cyc_NewControlFlow_anal_test(env,inflow,_tmp170);
_tmp26B=_tmp26A.f1;_tmp26C=_tmp26A.f2;_tmp26B=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp170->loc,_tmp26B);_tmp26C=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp170->loc,
_tmp26C);{struct _tuple5 _tmp26D=Cyc_NewControlFlow_anal_Rexp(env,_tmp26B,_tmp171);
struct _tuple5 _tmp26E=Cyc_NewControlFlow_anal_Rexp(env,_tmp26C,_tmp172);return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp137,env->all_changed,_tmp26D,_tmp26E,1);}}_LL127: if(*((int*)_tmp13E)!= 7)
goto _LL129;_tmp173=((struct Cyc_Absyn_And_e_struct*)_tmp13E)->f1;_tmp174=((struct
Cyc_Absyn_And_e_struct*)_tmp13E)->f2;_LL128: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp270;union Cyc_CfFlowInfo_FlowInfo_union _tmp271;struct _tuple8 _tmp26F=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp173);_tmp270=_tmp26F.f1;_tmp271=_tmp26F.f2;_tmp270=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp173->loc,_tmp270);_tmp271=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp173->loc,
_tmp271);{union Cyc_CfFlowInfo_FlowInfo_union _tmp273;void*_tmp274;struct _tuple5
_tmp272=Cyc_NewControlFlow_anal_Rexp(env,_tmp270,_tmp174);_tmp273=_tmp272.f1;
_tmp274=_tmp272.f2;_tmp273=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp174->loc,
_tmp273);{struct _tuple5 _tmp275=({struct _tuple5 _tmp278;_tmp278.f1=_tmp273;_tmp278.f2=
_tmp274;_tmp278;});struct _tuple5 _tmp276=({struct _tuple5 _tmp277;_tmp277.f1=
_tmp271;_tmp277.f2=(void*)((void*)0);_tmp277;});return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp137,env->all_changed,_tmp275,_tmp276,0);}}}_LL129: if(*((int*)_tmp13E)!= 8)
goto _LL12B;_tmp175=((struct Cyc_Absyn_Or_e_struct*)_tmp13E)->f1;_tmp176=((struct
Cyc_Absyn_Or_e_struct*)_tmp13E)->f2;_LL12A: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp27A;union Cyc_CfFlowInfo_FlowInfo_union _tmp27B;struct _tuple8 _tmp279=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp175);_tmp27A=_tmp279.f1;_tmp27B=_tmp279.f2;_tmp27A=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp175->loc,_tmp27A);_tmp27B=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp175->loc,
_tmp27B);{union Cyc_CfFlowInfo_FlowInfo_union _tmp27D;void*_tmp27E;struct _tuple5
_tmp27C=Cyc_NewControlFlow_anal_Rexp(env,_tmp27B,_tmp176);_tmp27D=_tmp27C.f1;
_tmp27E=_tmp27C.f2;_tmp27D=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp176->loc,
_tmp27D);{struct _tuple5 _tmp27F=({struct _tuple5 _tmp282;_tmp282.f1=_tmp27D;_tmp282.f2=
_tmp27E;_tmp282;});struct _tuple5 _tmp280=({struct _tuple5 _tmp281;_tmp281.f1=
_tmp27A;_tmp281.f2=(void*)((void*)1);_tmp281;});return Cyc_CfFlowInfo_join_flow_and_rval(
_tmp137,env->all_changed,_tmp27F,_tmp280,0);}}}_LL12B: if(*((int*)_tmp13E)!= 25)
goto _LL12D;_tmp177=((struct Cyc_Absyn_Subscript_e_struct*)_tmp13E)->f1;_tmp178=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp13E)->f2;_LL12C: {struct _RegionHandle
_tmp283=_new_region("temp");struct _RegionHandle*temp=& _tmp283;_push_region(temp);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp286;struct Cyc_List_List*_tmp287;struct
_tuple11 _tmp285=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct
Cyc_Absyn_Exp*_tmp284[2];_tmp284[1]=_tmp178;_tmp284[0]=_tmp177;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(temp,_tag_dynforward(
_tmp284,sizeof(struct Cyc_Absyn_Exp*),2));}),1);_tmp286=_tmp285.f1;_tmp287=
_tmp285.f2;_tmp286=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp178->loc,_tmp286);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp288=_tmp286;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp289=_tmp286;struct Cyc_Dict_Dict _tmp28A;struct Cyc_List_List*_tmp28B;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp28C;_LL1AE: if((_tmp289.ReachableFL).tag != 1)goto _LL1B0;_tmp28A=(_tmp289.ReachableFL).f1;
_tmp28B=(_tmp289.ReachableFL).f2;_tmp28C=(_tmp289.ReachableFL).f3;_LL1AF: if(Cyc_CfFlowInfo_initlevel(
_tmp28A,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp287))->tl))->hd)== (void*)0)({void*_tmp28D=0;Cyc_Tcutil_terr(
_tmp178->loc,({const char*_tmp28E="expression may not be initialized";
_tag_dynforward(_tmp28E,sizeof(char),_get_zero_arr_size_char(_tmp28E,34));}),
_tag_dynforward(_tmp28D,sizeof(void*),0));});{struct Cyc_List_List*_tmp28F=Cyc_NewControlFlow_add_subscript_reln(
_tmp137->r,_tmp28B,_tmp177,_tmp178);if(_tmp28B != _tmp28F)_tmp288=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp290;(_tmp290.ReachableFL).tag=1;(_tmp290.ReachableFL).f1=
_tmp28A;(_tmp290.ReachableFL).f2=_tmp28F;(_tmp290.ReachableFL).f3=_tmp28C;
_tmp290;});goto _LL1AD;}_LL1B0:;_LL1B1: goto _LL1AD;_LL1AD:;}{void*_tmp291=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp177->topt))->v);struct Cyc_List_List*
_tmp292;struct Cyc_Absyn_PtrInfo _tmp293;struct Cyc_Absyn_PtrAtts _tmp294;struct Cyc_Absyn_Conref*
_tmp295;_LL1B3: if(_tmp291 <= (void*)4)goto _LL1B7;if(*((int*)_tmp291)!= 9)goto
_LL1B5;_tmp292=((struct Cyc_Absyn_TupleType_struct*)_tmp291)->f1;_LL1B4: {void*
_tmp296=(void*)((struct Cyc_List_List*)_check_null(_tmp287))->hd;struct
_dynforward_ptr _tmp297;_LL1BA: if(_tmp296 <= (void*)3)goto _LL1BC;if(*((int*)
_tmp296)!= 4)goto _LL1BC;_tmp297=((struct Cyc_CfFlowInfo_Aggregate_struct*)_tmp296)->f1;
_LL1BB: _tmp288=Cyc_NewControlFlow_may_consume_exp(env,_tmp288,e);{unsigned int i=(
Cyc_Evexp_eval_const_uint_exp(_tmp178)).f1;struct _tuple5 _tmp299=({struct _tuple5
_tmp298;_tmp298.f1=_tmp288;_tmp298.f2=*((void**)_check_dynforward_subscript(
_tmp297,sizeof(void*),(int)i));_tmp298;});_npop_handler(0);return _tmp299;}_LL1BC:;
_LL1BD:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp29A=_cycalloc(
sizeof(*_tmp29A));_tmp29A[0]=({struct Cyc_Core_Impossible_struct _tmp29B;_tmp29B.tag=
Cyc_Core_Impossible;_tmp29B.f1=({const char*_tmp29C="anal_Rexp: Subscript";
_tag_dynforward(_tmp29C,sizeof(char),_get_zero_arr_size_char(_tmp29C,21));});
_tmp29B;});_tmp29A;}));_LL1B9:;}_LL1B5: if(*((int*)_tmp291)!= 4)goto _LL1B7;
_tmp293=((struct Cyc_Absyn_PointerType_struct*)_tmp291)->f1;_tmp294=_tmp293.ptr_atts;
_tmp295=_tmp294.bounds;_LL1B6:{union Cyc_Absyn_Constraint_union _tmp29D=(Cyc_Absyn_compress_conref(
_tmp295))->v;void*_tmp29E;_LL1BF: if((_tmp29D.Eq_constr).tag != 0)goto _LL1C1;
_tmp29E=(_tmp29D.Eq_constr).f1;if(_tmp29E <= (void*)2)goto _LL1C1;if(*((int*)
_tmp29E)!= 1)goto _LL1C1;_LL1C0:{void*_tmp29F=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp287))->tl))->hd;struct Cyc_List_List*
_tmp2A0;_LL1C4: if(_tmp29F <= (void*)3)goto _LL1C6;if(*((int*)_tmp29F)!= 3)goto
_LL1C6;_tmp2A0=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp29F)->f1;_LL1C5:(void*)(
_tmp178->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp2A1=
_cycalloc(sizeof(*_tmp2A1));_tmp2A1[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp2A2;_tmp2A2.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp2A2.f1=Cyc_CfFlowInfo_copy_tagcmps(
Cyc_Core_heap_region,_tmp2A0);_tmp2A2;});_tmp2A1;})));goto _LL1C3;_LL1C6:;_LL1C7:
goto _LL1C3;_LL1C3:;}goto _LL1BE;_LL1C1:;_LL1C2: goto _LL1BE;_LL1BE:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp2A4;void*_tmp2A5;struct _tuple5 _tmp2A3=Cyc_NewControlFlow_anal_derefR(env,
inflow,_tmp286,_tmp177,(void*)((struct Cyc_List_List*)_check_null(_tmp287))->hd);
_tmp2A4=_tmp2A3.f1;_tmp2A5=_tmp2A3.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp2A6=
_tmp2A4;_LL1C9: if((_tmp2A6.BottomFL).tag != 0)goto _LL1CB;_LL1CA: {struct _tuple5
_tmp2A8=({struct _tuple5 _tmp2A7;_tmp2A7.f1=_tmp2A4;_tmp2A7.f2=_tmp2A5;_tmp2A7;});
_npop_handler(0);return _tmp2A8;}_LL1CB:;_LL1CC: {struct _tuple5 _tmp2AA=({struct
_tuple5 _tmp2A9;_tmp2A9.f1=_tmp288;_tmp2A9.f2=_tmp2A5;_tmp2A9;});_npop_handler(0);
return _tmp2AA;}_LL1C8:;}}_LL1B7:;_LL1B8:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2AB=_cycalloc(sizeof(*_tmp2AB));_tmp2AB[0]=({struct Cyc_Core_Impossible_struct
_tmp2AC;_tmp2AC.tag=Cyc_Core_Impossible;_tmp2AC.f1=({const char*_tmp2AD="anal_Rexp: Subscript -- bad type";
_tag_dynforward(_tmp2AD,sizeof(char),_get_zero_arr_size_char(_tmp2AD,33));});
_tmp2AC;});_tmp2AB;}));_LL1B2:;}}};_pop_region(temp);}_LL12D: if(*((int*)_tmp13E)
!= 32)goto _LL12F;_tmp179=((struct Cyc_Absyn_Tunion_e_struct*)_tmp13E)->f1;_tmp17A=((
struct Cyc_Absyn_Tunion_e_struct*)_tmp13E)->f2;_LL12E: if(_tmp17A->is_flat){struct
_RegionHandle _tmp2AE=_new_region("temp");struct _RegionHandle*temp=& _tmp2AE;
_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union _tmp2B0;struct Cyc_List_List*
_tmp2B1;struct _tuple11 _tmp2AF=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,
inflow,_tmp179,0);_tmp2B0=_tmp2AF.f1;_tmp2B1=_tmp2AF.f2;_tmp2B0=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2B0);for(0;(unsigned int)_tmp179;(_tmp179=_tmp179->tl,_tmp2B1=_tmp2B1->tl)){
_tmp2B0=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)_tmp179->hd)->loc,
_tmp2B0,(void*)((struct Cyc_List_List*)_check_null(_tmp2B1))->hd);}{struct _tuple5
_tmp2B3=({struct _tuple5 _tmp2B2;_tmp2B2.f1=_tmp2B0;_tmp2B2.f2=(void*)_tmp137->unknown_all;
_tmp2B2;});_npop_handler(0);return _tmp2B3;}};_pop_region(temp);}_tmp17B=_tmp179;
goto _LL130;_LL12F: if(*((int*)_tmp13E)!= 26)goto _LL131;_tmp17B=((struct Cyc_Absyn_Tuple_e_struct*)
_tmp13E)->f1;_LL130: {struct _RegionHandle _tmp2B4=_new_region("temp");struct
_RegionHandle*temp=& _tmp2B4;_push_region(temp);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp2B6;struct Cyc_List_List*_tmp2B7;struct _tuple11 _tmp2B5=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp17B,0);_tmp2B6=_tmp2B5.f1;_tmp2B7=_tmp2B5.f2;_tmp2B6=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2B6);{struct _dynforward_ptr aggrdict=({unsigned int _tmp2BC=(
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp17B);void**
_tmp2BD=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp2BC));
struct _dynforward_ptr _tmp2C0=_tag_dynforward(_tmp2BD,sizeof(void*),_tmp2BC);{
unsigned int _tmp2BE=_tmp2BC;unsigned int i;for(i=0;i < _tmp2BE;i ++){_tmp2BD[i]=({
void*_tmp2BF=(void*)((struct Cyc_List_List*)_check_null(_tmp2B7))->hd;_tmp2B7=
_tmp2B7->tl;_tmp2BF;});}}_tmp2C0;});struct _tuple5 _tmp2BB=({struct _tuple5 _tmp2B8;
_tmp2B8.f1=_tmp2B6;_tmp2B8.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp2B9=_region_malloc(env->r,sizeof(*_tmp2B9));_tmp2B9[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp2BA;_tmp2BA.tag=4;_tmp2BA.f1=aggrdict;_tmp2BA;});_tmp2B9;});_tmp2B8;});
_npop_handler(0);return _tmp2BB;}};_pop_region(temp);}_LL131: if(*((int*)_tmp13E)
!= 31)goto _LL133;_tmp17C=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp13E)->f2;
_LL132: {struct Cyc_List_List*fs;{void*_tmp2C1=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_List_List*_tmp2C2;_LL1CE:
if(_tmp2C1 <= (void*)4)goto _LL1D0;if(*((int*)_tmp2C1)!= 11)goto _LL1D0;_tmp2C2=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp2C1)->f2;_LL1CF: fs=_tmp2C2;goto _LL1CD;
_LL1D0:;_LL1D1:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp2C3=
_cycalloc(sizeof(*_tmp2C3));_tmp2C3[0]=({struct Cyc_Core_Impossible_struct _tmp2C4;
_tmp2C4.tag=Cyc_Core_Impossible;_tmp2C4.f1=({const char*_tmp2C5="anal_Rexp:anon struct has bad type";
_tag_dynforward(_tmp2C5,sizeof(char),_get_zero_arr_size_char(_tmp2C5,35));});
_tmp2C4;});_tmp2C3;}));_LL1CD:;}_tmp17D=_tmp17C;_tmp182=fs;goto _LL134;}_LL133:
if(*((int*)_tmp13E)!= 30)goto _LL135;_tmp17D=((struct Cyc_Absyn_Struct_e_struct*)
_tmp13E)->f3;_tmp17E=((struct Cyc_Absyn_Struct_e_struct*)_tmp13E)->f4;if(_tmp17E
== 0)goto _LL135;_tmp17F=*_tmp17E;_tmp180=_tmp17F.impl;if(_tmp180 == 0)goto _LL135;
_tmp181=*_tmp180;_tmp182=_tmp181.fields;_LL134: {struct _RegionHandle _tmp2C6=
_new_region("temp");struct _RegionHandle*temp=& _tmp2C6;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp2C8;struct Cyc_List_List*_tmp2C9;struct _tuple11
_tmp2C7=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple14*),struct Cyc_List_List*
x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple14*))Cyc_Core_snd,
_tmp17D),0);_tmp2C8=_tmp2C7.f1;_tmp2C9=_tmp2C7.f2;_tmp2C8=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2C8);{struct _dynforward_ptr aggrdict=Cyc_CfFlowInfo_aggrfields_to_aggrdict(
_tmp137,_tmp182,(void*)_tmp137->unknown_all);{int i=0;for(0;_tmp2C9 != 0;(((
_tmp2C9=_tmp2C9->tl,_tmp17D=_tmp17D->tl)),++ i)){struct Cyc_List_List*ds=(*((
struct _tuple14*)((struct Cyc_List_List*)_check_null(_tmp17D))->hd)).f1;for(0;ds != 
0;ds=ds->tl){void*_tmp2CA=(void*)ds->hd;struct _dynforward_ptr*_tmp2CB;_LL1D3: if(*((
int*)_tmp2CA)!= 0)goto _LL1D5;_LL1D4:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2CC=_cycalloc(sizeof(*_tmp2CC));_tmp2CC[0]=({struct Cyc_Core_Impossible_struct
_tmp2CD;_tmp2CD.tag=Cyc_Core_Impossible;_tmp2CD.f1=({const char*_tmp2CE="anal_Rexp:Struct_e";
_tag_dynforward(_tmp2CE,sizeof(char),_get_zero_arr_size_char(_tmp2CE,19));});
_tmp2CD;});_tmp2CC;}));_LL1D5: if(*((int*)_tmp2CA)!= 1)goto _LL1D2;_tmp2CB=((
struct Cyc_Absyn_FieldName_struct*)_tmp2CA)->f1;_LL1D6: {int _tmp2CF=Cyc_NewControlFlow_get_field_index_fs(
_tmp182,_tmp2CB);*((void**)_check_dynforward_subscript(aggrdict,sizeof(void*),
_tmp2CF))=(void*)_tmp2C9->hd;}_LL1D2:;}}}{struct _tuple5 _tmp2D3=({struct _tuple5
_tmp2D0;_tmp2D0.f1=_tmp2C8;_tmp2D0.f2=(void*)({struct Cyc_CfFlowInfo_Aggregate_struct*
_tmp2D1=_region_malloc(env->r,sizeof(*_tmp2D1));_tmp2D1[0]=({struct Cyc_CfFlowInfo_Aggregate_struct
_tmp2D2;_tmp2D2.tag=4;_tmp2D2.f1=aggrdict;_tmp2D2;});_tmp2D1;});_tmp2D0;});
_npop_handler(0);return _tmp2D3;}}};_pop_region(temp);}_LL135: if(*((int*)_tmp13E)
!= 30)goto _LL137;_LL136:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp2D4=_cycalloc(sizeof(*_tmp2D4));_tmp2D4[0]=({struct Cyc_Core_Impossible_struct
_tmp2D5;_tmp2D5.tag=Cyc_Core_Impossible;_tmp2D5.f1=({const char*_tmp2D6="anal_Rexp:missing aggrdeclimpl";
_tag_dynforward(_tmp2D6,sizeof(char),_get_zero_arr_size_char(_tmp2D6,31));});
_tmp2D5;});_tmp2D4;}));_LL137: if(*((int*)_tmp13E)!= 28)goto _LL139;_tmp183=((
struct Cyc_Absyn_Array_e_struct*)_tmp13E)->f1;_LL138: {struct _RegionHandle _tmp2D7=
_new_region("temp");struct _RegionHandle*temp=& _tmp2D7;_push_region(temp);{struct
Cyc_List_List*_tmp2D8=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*
f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(
struct _tuple14*))Cyc_Core_snd,_tmp183);union Cyc_CfFlowInfo_FlowInfo_union _tmp2DA;
struct Cyc_List_List*_tmp2DB;struct _tuple11 _tmp2D9=Cyc_NewControlFlow_anal_unordered_Rexps(
temp,env,inflow,_tmp2D8,0);_tmp2DA=_tmp2D9.f1;_tmp2DB=_tmp2D9.f2;_tmp2DA=Cyc_CfFlowInfo_consume_unique_rvals(
e->loc,_tmp2DA);for(0;_tmp2DB != 0;(_tmp2DB=_tmp2DB->tl,_tmp2D8=_tmp2D8->tl)){
_tmp2DA=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp2D8))->hd)->loc,_tmp2DA,(void*)_tmp2DB->hd);}{struct _tuple5
_tmp2DD=({struct _tuple5 _tmp2DC;_tmp2DC.f1=_tmp2DA;_tmp2DC.f2=Cyc_CfFlowInfo_typ_to_absrval(
_tmp137,(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v,(void*)_tmp137->unknown_all);
_tmp2DC;});_npop_handler(0);return _tmp2DD;}};_pop_region(temp);}_LL139: if(*((int*)
_tmp13E)!= 29)goto _LL13B;_tmp184=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp13E)->f1;_tmp185=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp13E)->f2;
_tmp186=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp13E)->f3;_tmp187=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp13E)->f4;_LL13A: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp2DF;void*_tmp2E0;struct _tuple5 _tmp2DE=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp185);_tmp2DF=_tmp2DE.f1;_tmp2E0=_tmp2DE.f2;_tmp2DF=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp185->loc,_tmp2DF);{union Cyc_CfFlowInfo_FlowInfo_union _tmp2E1=_tmp2DF;struct
Cyc_Dict_Dict _tmp2E2;struct Cyc_List_List*_tmp2E3;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp2E4;_LL1D8: if((_tmp2E1.BottomFL).tag != 0)goto _LL1DA;_LL1D9: return({struct
_tuple5 _tmp2E5;_tmp2E5.f1=_tmp2DF;_tmp2E5.f2=(void*)_tmp137->unknown_all;_tmp2E5;});
_LL1DA: if((_tmp2E1.ReachableFL).tag != 1)goto _LL1D7;_tmp2E2=(_tmp2E1.ReachableFL).f1;
_tmp2E3=(_tmp2E1.ReachableFL).f2;_tmp2E4=(_tmp2E1.ReachableFL).f3;_LL1DB: if(Cyc_CfFlowInfo_initlevel(
_tmp2E2,_tmp2E0)== (void*)0)({void*_tmp2E6=0;Cyc_Tcutil_terr(_tmp185->loc,({
const char*_tmp2E7="expression may not be initialized";_tag_dynforward(_tmp2E7,
sizeof(char),_get_zero_arr_size_char(_tmp2E7,34));}),_tag_dynforward(_tmp2E6,
sizeof(void*),0));});{struct Cyc_List_List*new_relns=_tmp2E3;comp_loop: {void*
_tmp2E8=(void*)_tmp185->r;struct Cyc_Absyn_Exp*_tmp2E9;void*_tmp2EA;struct Cyc_Absyn_Vardecl*
_tmp2EB;void*_tmp2EC;struct Cyc_Absyn_Vardecl*_tmp2ED;void*_tmp2EE;struct Cyc_Absyn_Vardecl*
_tmp2EF;void*_tmp2F0;struct Cyc_Absyn_Vardecl*_tmp2F1;union Cyc_Absyn_Cnst_union
_tmp2F2;int _tmp2F3;void*_tmp2F4;struct Cyc_List_List*_tmp2F5;struct Cyc_List_List
_tmp2F6;struct Cyc_Absyn_Exp*_tmp2F7;_LL1DD: if(*((int*)_tmp2E8)!= 15)goto _LL1DF;
_tmp2E9=((struct Cyc_Absyn_Cast_e_struct*)_tmp2E8)->f2;_LL1DE: _tmp185=_tmp2E9;
goto comp_loop;_LL1DF: if(*((int*)_tmp2E8)!= 1)goto _LL1E1;_tmp2EA=(void*)((struct
Cyc_Absyn_Var_e_struct*)_tmp2E8)->f2;if(_tmp2EA <= (void*)1)goto _LL1E1;if(*((int*)
_tmp2EA)!= 0)goto _LL1E1;_tmp2EB=((struct Cyc_Absyn_Global_b_struct*)_tmp2EA)->f1;
if(!(!_tmp2EB->escapes))goto _LL1E1;_LL1E0: _tmp2ED=_tmp2EB;goto _LL1E2;_LL1E1: if(*((
int*)_tmp2E8)!= 1)goto _LL1E3;_tmp2EC=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2E8)->f2;if(_tmp2EC <= (void*)1)goto _LL1E3;if(*((int*)_tmp2EC)!= 3)goto _LL1E3;
_tmp2ED=((struct Cyc_Absyn_Local_b_struct*)_tmp2EC)->f1;if(!(!_tmp2ED->escapes))
goto _LL1E3;_LL1E2: _tmp2EF=_tmp2ED;goto _LL1E4;_LL1E3: if(*((int*)_tmp2E8)!= 1)goto
_LL1E5;_tmp2EE=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2E8)->f2;if(_tmp2EE <= (
void*)1)goto _LL1E5;if(*((int*)_tmp2EE)!= 4)goto _LL1E5;_tmp2EF=((struct Cyc_Absyn_Pat_b_struct*)
_tmp2EE)->f1;if(!(!_tmp2EF->escapes))goto _LL1E5;_LL1E4: _tmp2F1=_tmp2EF;goto
_LL1E6;_LL1E5: if(*((int*)_tmp2E8)!= 1)goto _LL1E7;_tmp2F0=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2E8)->f2;if(_tmp2F0 <= (void*)1)goto _LL1E7;if(*((int*)_tmp2F0)!= 2)goto _LL1E7;
_tmp2F1=((struct Cyc_Absyn_Param_b_struct*)_tmp2F0)->f1;if(!(!_tmp2F1->escapes))
goto _LL1E7;_LL1E6: new_relns=({struct Cyc_List_List*_tmp2F8=_region_malloc(env->r,
sizeof(*_tmp2F8));_tmp2F8->hd=({struct Cyc_CfFlowInfo_Reln*_tmp2F9=_region_malloc(
env->r,sizeof(*_tmp2F9));_tmp2F9->vd=_tmp184;_tmp2F9->rop=(union Cyc_CfFlowInfo_RelnOp_union)({
union Cyc_CfFlowInfo_RelnOp_union _tmp2FA;(_tmp2FA.LessVar).tag=1;(_tmp2FA.LessVar).f1=
_tmp2F1;_tmp2FA;});_tmp2F9;});_tmp2F8->tl=_tmp2E3;_tmp2F8;});goto _LL1DC;_LL1E7:
if(*((int*)_tmp2E8)!= 0)goto _LL1E9;_tmp2F2=((struct Cyc_Absyn_Const_e_struct*)
_tmp2E8)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp2E8)->f1).Int_c).tag != 2)
goto _LL1E9;_tmp2F3=(_tmp2F2.Int_c).f2;_LL1E8: new_relns=({struct Cyc_List_List*
_tmp2FB=_region_malloc(env->r,sizeof(*_tmp2FB));_tmp2FB->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp2FC=_region_malloc(env->r,sizeof(*_tmp2FC));_tmp2FC->vd=_tmp184;_tmp2FC->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp2FD;(
_tmp2FD.LessConst).tag=3;(_tmp2FD.LessConst).f1=(unsigned int)_tmp2F3;_tmp2FD;});
_tmp2FC;});_tmp2FB->tl=_tmp2E3;_tmp2FB;});goto _LL1DC;_LL1E9: if(*((int*)_tmp2E8)
!= 3)goto _LL1EB;_tmp2F4=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp2E8)->f1;
if((int)_tmp2F4 != 19)goto _LL1EB;_tmp2F5=((struct Cyc_Absyn_Primop_e_struct*)
_tmp2E8)->f2;if(_tmp2F5 == 0)goto _LL1EB;_tmp2F6=*_tmp2F5;_tmp2F7=(struct Cyc_Absyn_Exp*)
_tmp2F6.hd;_LL1EA:{void*_tmp2FE=(void*)_tmp2F7->r;void*_tmp2FF;struct Cyc_Absyn_Vardecl*
_tmp300;void*_tmp301;struct Cyc_Absyn_Vardecl*_tmp302;void*_tmp303;struct Cyc_Absyn_Vardecl*
_tmp304;void*_tmp305;struct Cyc_Absyn_Vardecl*_tmp306;_LL1EE: if(*((int*)_tmp2FE)
!= 1)goto _LL1F0;_tmp2FF=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2FE)->f2;if(
_tmp2FF <= (void*)1)goto _LL1F0;if(*((int*)_tmp2FF)!= 0)goto _LL1F0;_tmp300=((
struct Cyc_Absyn_Global_b_struct*)_tmp2FF)->f1;if(!(!_tmp300->escapes))goto _LL1F0;
_LL1EF: _tmp302=_tmp300;goto _LL1F1;_LL1F0: if(*((int*)_tmp2FE)!= 1)goto _LL1F2;
_tmp301=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2FE)->f2;if(_tmp301 <= (void*)
1)goto _LL1F2;if(*((int*)_tmp301)!= 3)goto _LL1F2;_tmp302=((struct Cyc_Absyn_Local_b_struct*)
_tmp301)->f1;if(!(!_tmp302->escapes))goto _LL1F2;_LL1F1: _tmp304=_tmp302;goto
_LL1F3;_LL1F2: if(*((int*)_tmp2FE)!= 1)goto _LL1F4;_tmp303=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2FE)->f2;if(_tmp303 <= (void*)1)goto _LL1F4;if(*((int*)_tmp303)!= 4)goto _LL1F4;
_tmp304=((struct Cyc_Absyn_Pat_b_struct*)_tmp303)->f1;if(!(!_tmp304->escapes))
goto _LL1F4;_LL1F3: _tmp306=_tmp304;goto _LL1F5;_LL1F4: if(*((int*)_tmp2FE)!= 1)goto
_LL1F6;_tmp305=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2FE)->f2;if(_tmp305 <= (
void*)1)goto _LL1F6;if(*((int*)_tmp305)!= 2)goto _LL1F6;_tmp306=((struct Cyc_Absyn_Param_b_struct*)
_tmp305)->f1;if(!(!_tmp306->escapes))goto _LL1F6;_LL1F5: new_relns=({struct Cyc_List_List*
_tmp307=_region_malloc(env->r,sizeof(*_tmp307));_tmp307->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp308=_region_malloc(env->r,sizeof(*_tmp308));_tmp308->vd=_tmp184;_tmp308->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp309;(
_tmp309.LessSize).tag=2;(_tmp309.LessSize).f1=_tmp306;_tmp309;});_tmp308;});
_tmp307->tl=_tmp2E3;_tmp307;});goto _LL1ED;_LL1F6:;_LL1F7: goto _LL1ED;_LL1ED:;}
goto _LL1DC;_LL1EB:;_LL1EC: goto _LL1DC;_LL1DC:;}if(_tmp2E3 != new_relns)_tmp2DF=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp30A;(
_tmp30A.ReachableFL).tag=1;(_tmp30A.ReachableFL).f1=_tmp2E2;(_tmp30A.ReachableFL).f2=
new_relns;(_tmp30A.ReachableFL).f3=_tmp2E4;_tmp30A;});{void*_tmp30B=_tmp2E0;
_LL1F9: if((int)_tmp30B != 0)goto _LL1FB;_LL1FA: return({struct _tuple5 _tmp30C;
_tmp30C.f1=_tmp2DF;_tmp30C.f2=(void*)_tmp137->unknown_all;_tmp30C;});_LL1FB: if((
int)_tmp30B != 2)goto _LL1FD;_LL1FC: goto _LL1FE;_LL1FD: if((int)_tmp30B != 1)goto
_LL1FF;_LL1FE: goto _LL200;_LL1FF: if(_tmp30B <= (void*)3)goto _LL201;if(*((int*)
_tmp30B)!= 2)goto _LL201;_LL200: {struct Cyc_List_List _tmp30D=({struct Cyc_List_List
_tmp319;_tmp319.hd=_tmp184;_tmp319.tl=0;_tmp319;});_tmp2DF=Cyc_NewControlFlow_add_vars(
_tmp137,_tmp2DF,(struct Cyc_List_List*)& _tmp30D,(void*)_tmp137->unknown_all,e->loc);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp30F;void*_tmp310;struct _tuple5 _tmp30E=Cyc_NewControlFlow_anal_Rexp(
env,_tmp2DF,_tmp186);_tmp30F=_tmp30E.f1;_tmp310=_tmp30E.f2;_tmp30F=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp186->loc,_tmp30F);{union Cyc_CfFlowInfo_FlowInfo_union _tmp311=_tmp30F;struct
Cyc_Dict_Dict _tmp312;struct Cyc_CfFlowInfo_ConsumeInfo _tmp313;_LL204: if((_tmp311.BottomFL).tag
!= 0)goto _LL206;_LL205: return({struct _tuple5 _tmp314;_tmp314.f1=_tmp30F;_tmp314.f2=(
void*)_tmp137->unknown_all;_tmp314;});_LL206: if((_tmp311.ReachableFL).tag != 1)
goto _LL203;_tmp312=(_tmp311.ReachableFL).f1;_tmp313=(_tmp311.ReachableFL).f3;
_LL207: if(Cyc_CfFlowInfo_initlevel(_tmp312,_tmp310)!= (void*)2){({void*_tmp315=0;
Cyc_Tcutil_terr(_tmp185->loc,({const char*_tmp316="expression may not be initialized";
_tag_dynforward(_tmp316,sizeof(char),_get_zero_arr_size_char(_tmp316,34));}),
_tag_dynforward(_tmp315,sizeof(void*),0));});return({struct _tuple5 _tmp317;
_tmp317.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp318;(_tmp318.BottomFL).tag=0;_tmp318;});_tmp317.f2=(void*)_tmp137->unknown_all;
_tmp317;});}_LL203:;}_tmp2DF=_tmp30F;goto _LL202;}}_LL201:;_LL202: while(1){struct
Cyc_List_List _tmp31A=({struct Cyc_List_List _tmp326;_tmp326.hd=_tmp184;_tmp326.tl=
0;_tmp326;});_tmp2DF=Cyc_NewControlFlow_add_vars(_tmp137,_tmp2DF,(struct Cyc_List_List*)&
_tmp31A,(void*)_tmp137->unknown_all,e->loc);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp31C;void*_tmp31D;struct _tuple5 _tmp31B=Cyc_NewControlFlow_anal_Rexp(env,
_tmp2DF,_tmp186);_tmp31C=_tmp31B.f1;_tmp31D=_tmp31B.f2;_tmp31C=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp186->loc,_tmp31C);{union Cyc_CfFlowInfo_FlowInfo_union _tmp31E=_tmp31C;struct
Cyc_Dict_Dict _tmp31F;struct Cyc_CfFlowInfo_ConsumeInfo _tmp320;_LL209: if((_tmp31E.BottomFL).tag
!= 0)goto _LL20B;_LL20A: goto _LL208;_LL20B: if((_tmp31E.ReachableFL).tag != 1)goto
_LL208;_tmp31F=(_tmp31E.ReachableFL).f1;_tmp320=(_tmp31E.ReachableFL).f3;_LL20C:
if(Cyc_CfFlowInfo_initlevel(_tmp31F,_tmp31D)!= (void*)2){({void*_tmp321=0;Cyc_Tcutil_terr(
_tmp185->loc,({const char*_tmp322="expression may not be initialized";
_tag_dynforward(_tmp322,sizeof(char),_get_zero_arr_size_char(_tmp322,34));}),
_tag_dynforward(_tmp321,sizeof(void*),0));});return({struct _tuple5 _tmp323;
_tmp323.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp324;(_tmp324.BottomFL).tag=0;_tmp324;});_tmp323.f2=(void*)_tmp137->unknown_all;
_tmp323;});}_LL208:;}{union Cyc_CfFlowInfo_FlowInfo_union _tmp325=Cyc_CfFlowInfo_join_flow(
_tmp137,env->all_changed,_tmp2DF,_tmp31C,1);if(Cyc_CfFlowInfo_flow_lessthan_approx(
_tmp325,_tmp2DF))break;_tmp2DF=_tmp325;}}}return({struct _tuple5 _tmp327;_tmp327.f1=
_tmp2DF;_tmp327.f2=(void*)_tmp137->unknown_all;_tmp327;});_LL1F8:;}}_LL1D7:;}}
_LL13B: if(*((int*)_tmp13E)!= 38)goto _LL13D;_tmp188=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp13E)->f1;_LL13C: while(1){union Cyc_CfFlowInfo_FlowInfo_union*_tmp329;struct
_tuple9 _tmp328=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp188);_tmp329=
_tmp328.f2;inflow=*_tmp329;{void*_tmp32A=(void*)_tmp188->r;struct Cyc_Absyn_Stmt*
_tmp32B;struct Cyc_Absyn_Stmt*_tmp32C;struct Cyc_Absyn_Decl*_tmp32D;struct Cyc_Absyn_Stmt*
_tmp32E;struct Cyc_Absyn_Exp*_tmp32F;_LL20E: if(_tmp32A <= (void*)1)goto _LL214;if(*((
int*)_tmp32A)!= 1)goto _LL210;_tmp32B=((struct Cyc_Absyn_Seq_s_struct*)_tmp32A)->f1;
_tmp32C=((struct Cyc_Absyn_Seq_s_struct*)_tmp32A)->f2;_LL20F: inflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp32B);_tmp188=_tmp32C;goto _LL20D;_LL210: if(*((int*)_tmp32A)!= 11)
goto _LL212;_tmp32D=((struct Cyc_Absyn_Decl_s_struct*)_tmp32A)->f1;_tmp32E=((
struct Cyc_Absyn_Decl_s_struct*)_tmp32A)->f2;_LL211: inflow=Cyc_NewControlFlow_anal_decl(
env,inflow,_tmp32D);_tmp188=_tmp32E;goto _LL20D;_LL212: if(*((int*)_tmp32A)!= 0)
goto _LL214;_tmp32F=((struct Cyc_Absyn_Exp_s_struct*)_tmp32A)->f1;_LL213: return Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp32F);_LL214:;_LL215:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp330=_cycalloc(sizeof(*_tmp330));_tmp330[0]=({struct Cyc_Core_Impossible_struct
_tmp331;_tmp331.tag=Cyc_Core_Impossible;_tmp331.f1=({const char*_tmp332="analyze_Rexp: ill-formed StmtExp";
_tag_dynforward(_tmp332,sizeof(char),_get_zero_arr_size_char(_tmp332,33));});
_tmp331;});_tmp330;}));_LL20D:;}}_LL13D: if(*((int*)_tmp13E)!= 1)goto _LL13F;
_tmp189=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp13E)->f2;if((int)_tmp189 != 0)
goto _LL13F;_LL13E: goto _LL140;_LL13F: if(*((int*)_tmp13E)!= 2)goto _LL141;_LL140:
goto _LL142;_LL141: if(*((int*)_tmp13E)!= 10)goto _LL143;_LL142: goto _LL144;_LL143:
if(*((int*)_tmp13E)!= 37)goto _LL145;_LL144: goto _LL146;_LL145: if(*((int*)_tmp13E)
!= 27)goto _LLE0;_LL146:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp333=_cycalloc(sizeof(*_tmp333));_tmp333[0]=({struct Cyc_Core_Impossible_struct
_tmp334;_tmp334.tag=Cyc_Core_Impossible;_tmp334.f1=({const char*_tmp335="anal_Rexp, unexpected exp form";
_tag_dynforward(_tmp335,sizeof(char),_get_zero_arr_size_char(_tmp335,31));});
_tmp334;});_tmp333;}));_LLE0:;}}static struct _tuple7 Cyc_NewControlFlow_anal_derefL(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo_union f,void*r,struct Cyc_List_List*
flds){struct Cyc_CfFlowInfo_FlowEnv*_tmp336=env->fenv;void*_tmp337=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp338;void*_tmp339;struct Cyc_Absyn_PtrAtts _tmp33A;struct Cyc_Absyn_Conref*
_tmp33B;struct Cyc_Absyn_Conref*_tmp33C;_LL217: if(_tmp337 <= (void*)4)goto _LL219;
if(*((int*)_tmp337)!= 4)goto _LL219;_tmp338=((struct Cyc_Absyn_PointerType_struct*)
_tmp337)->f1;_tmp339=(void*)_tmp338.elt_typ;_tmp33A=_tmp338.ptr_atts;_tmp33B=
_tmp33A.bounds;_tmp33C=_tmp33A.zero_term;_LL218: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp33D=f;struct Cyc_Dict_Dict _tmp33E;struct Cyc_List_List*_tmp33F;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp340;_LL21C: if((_tmp33D.BottomFL).tag != 0)goto _LL21E;_LL21D: return({struct
_tuple7 _tmp341;_tmp341.f1=f;_tmp341.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp342;(_tmp342.UnknownL).tag=1;_tmp342;});
_tmp341;});_LL21E: if((_tmp33D.ReachableFL).tag != 1)goto _LL21B;_tmp33E=(_tmp33D.ReachableFL).f1;
_tmp33F=(_tmp33D.ReachableFL).f2;_tmp340=(_tmp33D.ReachableFL).f3;_LL21F: if(Cyc_Tcutil_is_bound_one(
_tmp33B)){void*_tmp343=r;struct Cyc_CfFlowInfo_Place*_tmp344;struct Cyc_CfFlowInfo_Place
_tmp345;void*_tmp346;struct Cyc_List_List*_tmp347;void*_tmp348;_LL221: if((int)
_tmp343 != 1)goto _LL223;_LL222: goto _LL224;_LL223: if((int)_tmp343 != 2)goto _LL225;
_LL224:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_NotZero_struct*
_tmp349=_cycalloc(sizeof(*_tmp349));_tmp349[0]=({struct Cyc_CfFlowInfo_NotZero_struct
_tmp34A;_tmp34A.tag=Cyc_CfFlowInfo_NotZero;_tmp34A.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp33F);_tmp34A;});_tmp349;})));goto _LL220;_LL225: if(
_tmp343 <= (void*)3)goto _LL227;if(*((int*)_tmp343)!= 2)goto _LL227;_tmp344=((
struct Cyc_CfFlowInfo_AddressOf_struct*)_tmp343)->f1;_tmp345=*_tmp344;_tmp346=(
void*)_tmp345.root;_tmp347=_tmp345.fields;_LL226:(void*)(e->annot=(void*)((void*)({
struct Cyc_CfFlowInfo_NotZero_struct*_tmp34B=_cycalloc(sizeof(*_tmp34B));_tmp34B[
0]=({struct Cyc_CfFlowInfo_NotZero_struct _tmp34C;_tmp34C.tag=Cyc_CfFlowInfo_NotZero;
_tmp34C.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,_tmp33F);_tmp34C;});
_tmp34B;})));return({struct _tuple7 _tmp34D;_tmp34D.f1=f;_tmp34D.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp34E;(_tmp34E.PlaceL).tag=0;(_tmp34E.PlaceL).f1=({
struct Cyc_CfFlowInfo_Place*_tmp34F=_region_malloc(_tmp336->r,sizeof(*_tmp34F));
_tmp34F->root=(void*)_tmp346;_tmp34F->fields=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(
_tmp336->r,_tmp347,flds);_tmp34F;});_tmp34E;});_tmp34D;});_LL227: if((int)_tmp343
!= 0)goto _LL229;_LL228:(void*)(e->annot=(void*)((void*)Cyc_CfFlowInfo_IsZero));
return({struct _tuple7 _tmp350;_tmp350.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp352;(_tmp352.BottomFL).tag=0;_tmp352;});
_tmp350.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union
_tmp351;(_tmp351.UnknownL).tag=1;_tmp351;});_tmp350;});_LL229: if(_tmp343 <= (void*)
3)goto _LL22B;if(*((int*)_tmp343)!= 0)goto _LL22B;_tmp348=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp343)->f1;_LL22A: f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp348);goto
_LL22C;_LL22B:;_LL22C:(void*)(e->annot=(void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*
_tmp353=_cycalloc(sizeof(*_tmp353));_tmp353[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct
_tmp354;_tmp354.tag=Cyc_CfFlowInfo_UnknownZ;_tmp354.f1=Cyc_CfFlowInfo_copy_relns(
Cyc_Core_heap_region,_tmp33F);_tmp354;});_tmp353;})));_LL220:;}else{(void*)(e->annot=(
void*)((void*)({struct Cyc_CfFlowInfo_UnknownZ_struct*_tmp355=_cycalloc(sizeof(*
_tmp355));_tmp355[0]=({struct Cyc_CfFlowInfo_UnknownZ_struct _tmp356;_tmp356.tag=
Cyc_CfFlowInfo_UnknownZ;_tmp356.f1=Cyc_CfFlowInfo_copy_relns(Cyc_Core_heap_region,
_tmp33F);_tmp356;});_tmp355;})));}if(Cyc_CfFlowInfo_initlevel(_tmp33E,r)== (void*)
0)({void*_tmp357=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp358="dereference of possibly uninitialized pointer";
_tag_dynforward(_tmp358,sizeof(char),_get_zero_arr_size_char(_tmp358,46));}),
_tag_dynforward(_tmp357,sizeof(void*),0));});return({struct _tuple7 _tmp359;
_tmp359.f1=f;_tmp359.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union
_tmp35A;(_tmp35A.UnknownL).tag=1;_tmp35A;});_tmp359;});_LL21B:;}_LL219:;_LL21A:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp35B=_cycalloc(sizeof(*
_tmp35B));_tmp35B[0]=({struct Cyc_Core_Impossible_struct _tmp35C;_tmp35C.tag=Cyc_Core_Impossible;
_tmp35C.f1=({const char*_tmp35D="left deref of non-pointer-type";_tag_dynforward(
_tmp35D,sizeof(char),_get_zero_arr_size_char(_tmp35D,31));});_tmp35C;});_tmp35B;}));
_LL216:;}static struct _tuple7 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*
env,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*
flds){struct Cyc_Dict_Dict d;struct Cyc_CfFlowInfo_FlowEnv*_tmp35E=env->fenv;{union
Cyc_CfFlowInfo_FlowInfo_union _tmp35F=inflow;struct Cyc_Dict_Dict _tmp360;struct Cyc_List_List*
_tmp361;_LL22E: if((_tmp35F.BottomFL).tag != 0)goto _LL230;_LL22F: return({struct
_tuple7 _tmp362;_tmp362.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp364;(_tmp364.BottomFL).tag=0;_tmp364;});_tmp362.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp363;(_tmp363.UnknownL).tag=1;_tmp363;});
_tmp362;});_LL230: if((_tmp35F.ReachableFL).tag != 1)goto _LL22D;_tmp360=(_tmp35F.ReachableFL).f1;
_tmp361=(_tmp35F.ReachableFL).f2;_LL231: d=_tmp360;_LL22D:;}{void*_tmp365=(void*)
e->r;struct Cyc_Absyn_Exp*_tmp366;struct Cyc_Absyn_Exp*_tmp367;struct Cyc_Absyn_Exp*
_tmp368;void*_tmp369;struct Cyc_Absyn_Vardecl*_tmp36A;void*_tmp36B;struct Cyc_Absyn_Vardecl*
_tmp36C;void*_tmp36D;struct Cyc_Absyn_Vardecl*_tmp36E;void*_tmp36F;struct Cyc_Absyn_Vardecl*
_tmp370;struct Cyc_Absyn_Exp*_tmp371;struct _dynforward_ptr*_tmp372;struct Cyc_Absyn_Exp*
_tmp373;struct Cyc_Absyn_Exp*_tmp374;struct Cyc_Absyn_Exp*_tmp375;struct Cyc_Absyn_Exp*
_tmp376;struct _dynforward_ptr*_tmp377;_LL233: if(*((int*)_tmp365)!= 15)goto _LL235;
_tmp366=((struct Cyc_Absyn_Cast_e_struct*)_tmp365)->f2;_LL234: _tmp367=_tmp366;
goto _LL236;_LL235: if(*((int*)_tmp365)!= 13)goto _LL237;_tmp367=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp365)->f1;_LL236: _tmp368=_tmp367;goto _LL238;_LL237: if(*((int*)_tmp365)!= 14)
goto _LL239;_tmp368=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp365)->f1;_LL238:
return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp368,flds);_LL239: if(*((int*)
_tmp365)!= 1)goto _LL23B;_tmp369=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp365)->f2;
if(_tmp369 <= (void*)1)goto _LL23B;if(*((int*)_tmp369)!= 2)goto _LL23B;_tmp36A=((
struct Cyc_Absyn_Param_b_struct*)_tmp369)->f1;_LL23A: _tmp36C=_tmp36A;goto _LL23C;
_LL23B: if(*((int*)_tmp365)!= 1)goto _LL23D;_tmp36B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp365)->f2;if(_tmp36B <= (void*)1)goto _LL23D;if(*((int*)_tmp36B)!= 3)goto _LL23D;
_tmp36C=((struct Cyc_Absyn_Local_b_struct*)_tmp36B)->f1;_LL23C: _tmp36E=_tmp36C;
goto _LL23E;_LL23D: if(*((int*)_tmp365)!= 1)goto _LL23F;_tmp36D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp365)->f2;if(_tmp36D <= (void*)1)goto _LL23F;if(*((int*)_tmp36D)!= 4)goto _LL23F;
_tmp36E=((struct Cyc_Absyn_Pat_b_struct*)_tmp36D)->f1;_LL23E: return({struct
_tuple7 _tmp378;_tmp378.f1=inflow;_tmp378.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({
union Cyc_CfFlowInfo_AbsLVal_union _tmp379;(_tmp379.PlaceL).tag=0;(_tmp379.PlaceL).f1=({
struct Cyc_CfFlowInfo_Place*_tmp37A=_region_malloc(env->r,sizeof(*_tmp37A));
_tmp37A->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp37B=
_region_malloc(env->r,sizeof(*_tmp37B));_tmp37B[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp37C;_tmp37C.tag=0;_tmp37C.f1=_tmp36E;_tmp37C;});_tmp37B;}));_tmp37A->fields=
flds;_tmp37A;});_tmp379;});_tmp378;});_LL23F: if(*((int*)_tmp365)!= 1)goto _LL241;
_tmp36F=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp365)->f2;if(_tmp36F <= (void*)
1)goto _LL241;if(*((int*)_tmp36F)!= 0)goto _LL241;_tmp370=((struct Cyc_Absyn_Global_b_struct*)
_tmp36F)->f1;_LL240: return({struct _tuple7 _tmp37D;_tmp37D.f1=inflow;_tmp37D.f2=(
union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union _tmp37E;(
_tmp37E.UnknownL).tag=1;_tmp37E;});_tmp37D;});_LL241: if(*((int*)_tmp365)!= 24)
goto _LL243;_tmp371=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp365)->f1;_tmp372=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp365)->f2;_LL242:{void*_tmp37F=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp371->topt))->v);struct Cyc_Absyn_PtrInfo
_tmp380;void*_tmp381;_LL24C: if(_tmp37F <= (void*)4)goto _LL24E;if(*((int*)_tmp37F)
!= 4)goto _LL24E;_tmp380=((struct Cyc_Absyn_PointerType_struct*)_tmp37F)->f1;
_tmp381=(void*)_tmp380.elt_typ;_LL24D: if(!Cyc_Absyn_is_union_type(_tmp381))flds=({
struct Cyc_List_List*_tmp382=_region_malloc(env->r,sizeof(*_tmp382));_tmp382->hd=(
void*)Cyc_NewControlFlow_get_field_index(_tmp381,_tmp372);_tmp382->tl=flds;
_tmp382;});goto _LL24B;_LL24E:;_LL24F:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp383=_cycalloc(sizeof(*_tmp383));_tmp383[0]=({struct Cyc_Core_Impossible_struct
_tmp384;_tmp384.tag=Cyc_Core_Impossible;_tmp384.f1=({const char*_tmp385="anal_Rexp: AggrArrow ptr";
_tag_dynforward(_tmp385,sizeof(char),_get_zero_arr_size_char(_tmp385,25));});
_tmp384;});_tmp383;}));_LL24B:;}_tmp373=_tmp371;goto _LL244;_LL243: if(*((int*)
_tmp365)!= 22)goto _LL245;_tmp373=((struct Cyc_Absyn_Deref_e_struct*)_tmp365)->f1;
_LL244: {union Cyc_CfFlowInfo_FlowInfo_union _tmp387;void*_tmp388;struct _tuple5
_tmp386=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp373);_tmp387=_tmp386.f1;
_tmp388=_tmp386.f2;_tmp387=Cyc_CfFlowInfo_readthrough_unique_rvals(e->loc,
_tmp387);return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp373,_tmp387,_tmp388,
flds);}_LL245: if(*((int*)_tmp365)!= 25)goto _LL247;_tmp374=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp365)->f1;_tmp375=((struct Cyc_Absyn_Subscript_e_struct*)_tmp365)->f2;_LL246: {
void*_tmp389=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp374->topt))->v);struct Cyc_Absyn_PtrInfo _tmp38A;struct Cyc_Absyn_PtrAtts
_tmp38B;struct Cyc_Absyn_Conref*_tmp38C;_LL251: if(_tmp389 <= (void*)4)goto _LL255;
if(*((int*)_tmp389)!= 9)goto _LL253;_LL252: {unsigned int _tmp38D=(Cyc_Evexp_eval_const_uint_exp(
_tmp375)).f1;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,_tmp374,({struct
Cyc_List_List*_tmp38E=_region_malloc(env->r,sizeof(*_tmp38E));_tmp38E->hd=(void*)((
int)_tmp38D);_tmp38E->tl=flds;_tmp38E;}));}_LL253: if(*((int*)_tmp389)!= 4)goto
_LL255;_tmp38A=((struct Cyc_Absyn_PointerType_struct*)_tmp389)->f1;_tmp38B=
_tmp38A.ptr_atts;_tmp38C=_tmp38B.bounds;_LL254: {struct _RegionHandle _tmp38F=
_new_region("temp");struct _RegionHandle*temp=& _tmp38F;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp392;struct Cyc_List_List*_tmp393;struct _tuple11
_tmp391=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,({struct Cyc_Absyn_Exp*
_tmp390[2];_tmp390[1]=_tmp375;_tmp390[0]=_tmp374;((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _dynforward_ptr))Cyc_List_rlist)(temp,_tag_dynforward(
_tmp390,sizeof(struct Cyc_Absyn_Exp*),2));}),1);_tmp392=_tmp391.f1;_tmp393=
_tmp391.f2;_tmp392=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp375->loc,_tmp392);{
union Cyc_CfFlowInfo_FlowInfo_union _tmp394=_tmp392;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp395=_tmp392;struct Cyc_Dict_Dict _tmp396;struct Cyc_List_List*_tmp397;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp398;_LL258: if((_tmp395.ReachableFL).tag != 1)goto _LL25A;_tmp396=(_tmp395.ReachableFL).f1;
_tmp397=(_tmp395.ReachableFL).f2;_tmp398=(_tmp395.ReachableFL).f3;_LL259: if(Cyc_CfFlowInfo_initlevel(
_tmp396,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)
_check_null(_tmp393))->tl))->hd)== (void*)0)({void*_tmp399=0;Cyc_Tcutil_terr(
_tmp375->loc,({const char*_tmp39A="expression may not be initialized";
_tag_dynforward(_tmp39A,sizeof(char),_get_zero_arr_size_char(_tmp39A,34));}),
_tag_dynforward(_tmp399,sizeof(void*),0));});{struct Cyc_List_List*_tmp39B=Cyc_NewControlFlow_add_subscript_reln(
_tmp35E->r,_tmp397,_tmp374,_tmp375);if(_tmp397 != _tmp39B)_tmp394=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp39C;(_tmp39C.ReachableFL).tag=1;(_tmp39C.ReachableFL).f1=
_tmp396;(_tmp39C.ReachableFL).f2=_tmp39B;(_tmp39C.ReachableFL).f3=_tmp398;
_tmp39C;});goto _LL257;}_LL25A:;_LL25B: goto _LL257;_LL257:;}{union Cyc_Absyn_Constraint_union
_tmp39D=(Cyc_Absyn_compress_conref(_tmp38C))->v;void*_tmp39E;_LL25D: if((_tmp39D.Eq_constr).tag
!= 0)goto _LL25F;_tmp39E=(_tmp39D.Eq_constr).f1;if(_tmp39E <= (void*)2)goto _LL25F;
if(*((int*)_tmp39E)!= 1)goto _LL25F;_LL25E:{void*_tmp39F=(void*)((struct Cyc_List_List*)
_check_null(((struct Cyc_List_List*)_check_null(_tmp393))->tl))->hd;struct Cyc_List_List*
_tmp3A0;_LL262: if(_tmp39F <= (void*)3)goto _LL264;if(*((int*)_tmp39F)!= 3)goto
_LL264;_tmp3A0=((struct Cyc_CfFlowInfo_TagCmps_struct*)_tmp39F)->f1;_LL263:(void*)(
_tmp375->annot=(void*)((void*)({struct Cyc_CfFlowInfo_HasTagCmps_struct*_tmp3A1=
_cycalloc(sizeof(*_tmp3A1));_tmp3A1[0]=({struct Cyc_CfFlowInfo_HasTagCmps_struct
_tmp3A2;_tmp3A2.tag=Cyc_CfFlowInfo_HasTagCmps;_tmp3A2.f1=Cyc_CfFlowInfo_copy_tagcmps(
Cyc_Core_heap_region,_tmp3A0);_tmp3A2;});_tmp3A1;})));goto _LL261;_LL264:;_LL265:
goto _LL261;_LL261:;}goto _LL25C;_LL25F:;_LL260: goto _LL25C;_LL25C:;}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp3A4;union Cyc_CfFlowInfo_AbsLVal_union _tmp3A5;struct _tuple7 _tmp3A3=Cyc_NewControlFlow_anal_derefL(
env,inflow,_tmp374,_tmp392,(void*)((struct Cyc_List_List*)_check_null(_tmp393))->hd,
flds);_tmp3A4=_tmp3A3.f1;_tmp3A5=_tmp3A3.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp3A6=_tmp3A4;_LL267: if((_tmp3A6.BottomFL).tag != 0)goto _LL269;_LL268: {struct
_tuple7 _tmp3A8=({struct _tuple7 _tmp3A7;_tmp3A7.f1=_tmp3A4;_tmp3A7.f2=_tmp3A5;
_tmp3A7;});_npop_handler(0);return _tmp3A8;}_LL269:;_LL26A: {struct _tuple7 _tmp3AA=({
struct _tuple7 _tmp3A9;_tmp3A9.f1=_tmp394;_tmp3A9.f2=_tmp3A5;_tmp3A9;});
_npop_handler(0);return _tmp3AA;}_LL266:;}}}};_pop_region(temp);}_LL255:;_LL256:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp3AB=_cycalloc(sizeof(*
_tmp3AB));_tmp3AB[0]=({struct Cyc_Core_Impossible_struct _tmp3AC;_tmp3AC.tag=Cyc_Core_Impossible;
_tmp3AC.f1=({const char*_tmp3AD="anal_Lexp: Subscript -- bad type";
_tag_dynforward(_tmp3AD,sizeof(char),_get_zero_arr_size_char(_tmp3AD,33));});
_tmp3AC;});_tmp3AB;}));_LL250:;}_LL247: if(*((int*)_tmp365)!= 23)goto _LL249;
_tmp376=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp365)->f1;_tmp377=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp365)->f2;_LL248: if(Cyc_Absyn_is_union_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp376->topt))->v))return({struct _tuple7 _tmp3AE;_tmp3AE.f1=inflow;
_tmp3AE.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union
_tmp3AF;(_tmp3AF.UnknownL).tag=1;_tmp3AF;});_tmp3AE;});return Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,_tmp376,({struct Cyc_List_List*_tmp3B0=_region_malloc(env->r,sizeof(*
_tmp3B0));_tmp3B0->hd=(void*)Cyc_NewControlFlow_get_field_index((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp376->topt))->v,_tmp377);_tmp3B0->tl=flds;_tmp3B0;}));
_LL249:;_LL24A: return({struct _tuple7 _tmp3B1;_tmp3B1.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp3B3;(_tmp3B3.BottomFL).tag=0;_tmp3B3;});
_tmp3B1.f2=(union Cyc_CfFlowInfo_AbsLVal_union)({union Cyc_CfFlowInfo_AbsLVal_union
_tmp3B2;(_tmp3B2.UnknownL).tag=1;_tmp3B2;});_tmp3B1;});_LL232:;}}static struct
_tuple7 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union
Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Exp*e){union Cyc_CfFlowInfo_FlowInfo_union
_tmp3B5;union Cyc_CfFlowInfo_AbsLVal_union _tmp3B6;struct _tuple7 _tmp3B4=Cyc_NewControlFlow_anal_Lexp_rec(
env,inflow,e,0);_tmp3B5=_tmp3B4.f1;_tmp3B6=_tmp3B4.f2;return({struct _tuple7
_tmp3B7;_tmp3B7.f1=_tmp3B5;_tmp3B7.f2=_tmp3B6;_tmp3B7;});}static struct _tuple8 Cyc_NewControlFlow_anal_test(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Exp*e){struct Cyc_CfFlowInfo_FlowEnv*_tmp3B8=env->fenv;{void*
_tmp3B9=(void*)e->r;struct Cyc_Absyn_Exp*_tmp3BA;struct Cyc_Absyn_Exp*_tmp3BB;
struct Cyc_Absyn_Exp*_tmp3BC;struct Cyc_Absyn_Exp*_tmp3BD;struct Cyc_Absyn_Exp*
_tmp3BE;struct Cyc_Absyn_Exp*_tmp3BF;struct Cyc_Absyn_Exp*_tmp3C0;struct Cyc_Absyn_Exp*
_tmp3C1;struct Cyc_Absyn_Exp*_tmp3C2;void*_tmp3C3;struct Cyc_List_List*_tmp3C4;
struct Cyc_List_List _tmp3C5;struct Cyc_Absyn_Exp*_tmp3C6;struct Cyc_List_List*
_tmp3C7;void*_tmp3C8;struct Cyc_List_List*_tmp3C9;_LL26C: if(*((int*)_tmp3B9)!= 6)
goto _LL26E;_tmp3BA=((struct Cyc_Absyn_Conditional_e_struct*)_tmp3B9)->f1;_tmp3BB=((
struct Cyc_Absyn_Conditional_e_struct*)_tmp3B9)->f2;_tmp3BC=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp3B9)->f3;_LL26D: {union Cyc_CfFlowInfo_FlowInfo_union _tmp3CB;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3CC;struct _tuple8 _tmp3CA=Cyc_NewControlFlow_anal_test(env,inflow,_tmp3BA);
_tmp3CB=_tmp3CA.f1;_tmp3CC=_tmp3CA.f2;_tmp3CB=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3BA->loc,_tmp3CB);_tmp3CC=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3BA->loc,
_tmp3CC);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3CE;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3CF;struct _tuple8 _tmp3CD=Cyc_NewControlFlow_anal_test(env,_tmp3CB,_tmp3BB);
_tmp3CE=_tmp3CD.f1;_tmp3CF=_tmp3CD.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp3D1;
union Cyc_CfFlowInfo_FlowInfo_union _tmp3D2;struct _tuple8 _tmp3D0=Cyc_NewControlFlow_anal_test(
env,_tmp3CC,_tmp3BC);_tmp3D1=_tmp3D0.f1;_tmp3D2=_tmp3D0.f2;return({struct _tuple8
_tmp3D3;_tmp3D3.f1=Cyc_CfFlowInfo_join_flow(_tmp3B8,env->all_changed,_tmp3CE,
_tmp3D1,1);_tmp3D3.f2=Cyc_CfFlowInfo_join_flow(_tmp3B8,env->all_changed,_tmp3CF,
_tmp3D2,1);_tmp3D3;});}}}_LL26E: if(*((int*)_tmp3B9)!= 7)goto _LL270;_tmp3BD=((
struct Cyc_Absyn_And_e_struct*)_tmp3B9)->f1;_tmp3BE=((struct Cyc_Absyn_And_e_struct*)
_tmp3B9)->f2;_LL26F: {union Cyc_CfFlowInfo_FlowInfo_union _tmp3D5;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D6;struct _tuple8 _tmp3D4=Cyc_NewControlFlow_anal_test(env,inflow,_tmp3BD);
_tmp3D5=_tmp3D4.f1;_tmp3D6=_tmp3D4.f2;_tmp3D5=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3BD->loc,_tmp3D5);_tmp3D6=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3BD->loc,
_tmp3D6);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3D8;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3D9;struct _tuple8 _tmp3D7=Cyc_NewControlFlow_anal_test(env,_tmp3D5,_tmp3BE);
_tmp3D8=_tmp3D7.f1;_tmp3D9=_tmp3D7.f2;_tmp3D8=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3BE->loc,_tmp3D8);_tmp3D9=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3BE->loc,
_tmp3D9);return({struct _tuple8 _tmp3DA;_tmp3DA.f1=_tmp3D8;_tmp3DA.f2=Cyc_CfFlowInfo_join_flow(
_tmp3B8,env->all_changed,_tmp3D6,_tmp3D9,0);_tmp3DA;});}}_LL270: if(*((int*)
_tmp3B9)!= 8)goto _LL272;_tmp3BF=((struct Cyc_Absyn_Or_e_struct*)_tmp3B9)->f1;
_tmp3C0=((struct Cyc_Absyn_Or_e_struct*)_tmp3B9)->f2;_LL271: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3DC;union Cyc_CfFlowInfo_FlowInfo_union _tmp3DD;struct _tuple8 _tmp3DB=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp3BF);_tmp3DC=_tmp3DB.f1;_tmp3DD=_tmp3DB.f2;_tmp3DC=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3BF->loc,_tmp3DC);_tmp3DD=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3BF->loc,
_tmp3DD);{union Cyc_CfFlowInfo_FlowInfo_union _tmp3DF;union Cyc_CfFlowInfo_FlowInfo_union
_tmp3E0;struct _tuple8 _tmp3DE=Cyc_NewControlFlow_anal_test(env,_tmp3DD,_tmp3C0);
_tmp3DF=_tmp3DE.f1;_tmp3E0=_tmp3DE.f2;_tmp3DF=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp3C0->loc,_tmp3DF);_tmp3E0=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp3C0->loc,
_tmp3E0);return({struct _tuple8 _tmp3E1;_tmp3E1.f1=Cyc_CfFlowInfo_join_flow(
_tmp3B8,env->all_changed,_tmp3DC,_tmp3DF,0);_tmp3E1.f2=_tmp3E0;_tmp3E1;});}}
_LL272: if(*((int*)_tmp3B9)!= 9)goto _LL274;_tmp3C1=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmp3B9)->f1;_tmp3C2=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp3B9)->f2;_LL273: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp3E3;void*_tmp3E4;struct _tuple5 _tmp3E2=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp3C1);_tmp3E3=_tmp3E2.f1;_tmp3E4=_tmp3E2.f2;_tmp3E3=Cyc_CfFlowInfo_drop_unique_rvals(
_tmp3C1->loc,_tmp3E3);return Cyc_NewControlFlow_anal_test(env,_tmp3E3,_tmp3C2);}
_LL274: if(*((int*)_tmp3B9)!= 3)goto _LL276;_tmp3C3=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3B9)->f1;if((int)_tmp3C3 != 11)goto _LL276;_tmp3C4=((struct Cyc_Absyn_Primop_e_struct*)
_tmp3B9)->f2;if(_tmp3C4 == 0)goto _LL276;_tmp3C5=*_tmp3C4;_tmp3C6=(struct Cyc_Absyn_Exp*)
_tmp3C5.hd;_tmp3C7=_tmp3C5.tl;if(_tmp3C7 != 0)goto _LL276;_LL275: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp3E6;union Cyc_CfFlowInfo_FlowInfo_union _tmp3E7;struct _tuple8 _tmp3E5=Cyc_NewControlFlow_anal_test(
env,inflow,_tmp3C6);_tmp3E6=_tmp3E5.f1;_tmp3E7=_tmp3E5.f2;return({struct _tuple8
_tmp3E8;_tmp3E8.f1=_tmp3E7;_tmp3E8.f2=_tmp3E6;_tmp3E8;});}_LL276: if(*((int*)
_tmp3B9)!= 3)goto _LL278;_tmp3C8=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3B9)->f1;_tmp3C9=((struct Cyc_Absyn_Primop_e_struct*)_tmp3B9)->f2;_LL277: {
void*r1;void*r2;union Cyc_CfFlowInfo_FlowInfo_union f;{struct _RegionHandle _tmp3E9=
_new_region("temp");struct _RegionHandle*temp=& _tmp3E9;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp3EB;struct Cyc_List_List*_tmp3EC;struct _tuple11
_tmp3EA=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp3C9,0);
_tmp3EB=_tmp3EA.f1;_tmp3EC=_tmp3EA.f2;r1=(void*)((struct Cyc_List_List*)
_check_null(_tmp3EC))->hd;r2=(void*)((struct Cyc_List_List*)_check_null(_tmp3EC->tl))->hd;
f=_tmp3EB;};_pop_region(temp);}{union Cyc_CfFlowInfo_FlowInfo_union _tmp3ED=f;
struct Cyc_Dict_Dict _tmp3EE;struct Cyc_List_List*_tmp3EF;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp3F0;_LL27B: if((_tmp3ED.BottomFL).tag != 0)goto _LL27D;_LL27C: return({struct
_tuple8 _tmp3F1;_tmp3F1.f1=f;_tmp3F1.f2=f;_tmp3F1;});_LL27D: if((_tmp3ED.ReachableFL).tag
!= 1)goto _LL27A;_tmp3EE=(_tmp3ED.ReachableFL).f1;_tmp3EF=(_tmp3ED.ReachableFL).f2;
_tmp3F0=(_tmp3ED.ReachableFL).f3;_LL27E: {struct Cyc_Absyn_Exp*_tmp3F2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3C9))->hd;struct Cyc_Absyn_Exp*_tmp3F3=(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp3C9->tl))->hd;if(Cyc_CfFlowInfo_initlevel(
_tmp3EE,r1)== (void*)0)({void*_tmp3F4=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)
_tmp3C9->hd)->loc,({const char*_tmp3F5="expression may not be initialized";
_tag_dynforward(_tmp3F5,sizeof(char),_get_zero_arr_size_char(_tmp3F5,34));}),
_tag_dynforward(_tmp3F4,sizeof(void*),0));});if(Cyc_CfFlowInfo_initlevel(_tmp3EE,
r2)== (void*)0)({void*_tmp3F6=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp3C9->tl))->hd)->loc,({const char*_tmp3F7="expression may not be initialized";
_tag_dynforward(_tmp3F7,sizeof(char),_get_zero_arr_size_char(_tmp3F7,34));}),
_tag_dynforward(_tmp3F6,sizeof(void*),0));});if(_tmp3C8 == (void*)5  || _tmp3C8 == (
void*)6){struct _tuple0 _tmp3F9=({struct _tuple0 _tmp3F8;_tmp3F8.f1=r1;_tmp3F8.f2=r2;
_tmp3F8;});void*_tmp3FA;void*_tmp3FB;void*_tmp3FC;void*_tmp3FD;void*_tmp3FE;void*
_tmp3FF;void*_tmp400;void*_tmp401;void*_tmp402;void*_tmp403;void*_tmp404;void*
_tmp405;void*_tmp406;void*_tmp407;void*_tmp408;void*_tmp409;void*_tmp40A;void*
_tmp40B;void*_tmp40C;void*_tmp40D;_LL280: _tmp3FA=_tmp3F9.f1;if(_tmp3FA <= (void*)
3)goto _LL282;if(*((int*)_tmp3FA)!= 0)goto _LL282;_tmp3FB=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp3FA)->f1;_tmp3FC=_tmp3F9.f2;if((int)_tmp3FC != 0)goto _LL282;_LL281: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp40F;union Cyc_CfFlowInfo_FlowInfo_union _tmp410;struct _tuple8 _tmp40E=Cyc_NewControlFlow_splitzero(
env,f,f,_tmp3F2,_tmp3FB);_tmp40F=_tmp40E.f1;_tmp410=_tmp40E.f2;{void*_tmp411=
_tmp3C8;_LL295: if((int)_tmp411 != 5)goto _LL297;_LL296: return({struct _tuple8
_tmp412;_tmp412.f1=_tmp410;_tmp412.f2=_tmp40F;_tmp412;});_LL297: if((int)_tmp411
!= 6)goto _LL299;_LL298: return({struct _tuple8 _tmp413;_tmp413.f1=_tmp40F;_tmp413.f2=
_tmp410;_tmp413;});_LL299:;_LL29A:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp414=_cycalloc(sizeof(*_tmp414));_tmp414[0]=({struct Cyc_Core_Impossible_struct
_tmp415;_tmp415.tag=Cyc_Core_Impossible;_tmp415.f1=({const char*_tmp416="anal_test, zero-split";
_tag_dynforward(_tmp416,sizeof(char),_get_zero_arr_size_char(_tmp416,22));});
_tmp415;});_tmp414;}));_LL294:;}}_LL282: _tmp3FD=_tmp3F9.f1;if((int)_tmp3FD != 0)
goto _LL284;_tmp3FE=_tmp3F9.f2;if(_tmp3FE <= (void*)3)goto _LL284;if(*((int*)
_tmp3FE)!= 0)goto _LL284;_tmp3FF=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp3FE)->f1;_LL283: {union Cyc_CfFlowInfo_FlowInfo_union _tmp418;union Cyc_CfFlowInfo_FlowInfo_union
_tmp419;struct _tuple8 _tmp417=Cyc_NewControlFlow_splitzero(env,f,f,_tmp3F3,
_tmp3FF);_tmp418=_tmp417.f1;_tmp419=_tmp417.f2;{void*_tmp41A=_tmp3C8;_LL29C: if((
int)_tmp41A != 5)goto _LL29E;_LL29D: return({struct _tuple8 _tmp41B;_tmp41B.f1=
_tmp419;_tmp41B.f2=_tmp418;_tmp41B;});_LL29E: if((int)_tmp41A != 6)goto _LL2A0;
_LL29F: return({struct _tuple8 _tmp41C;_tmp41C.f1=_tmp418;_tmp41C.f2=_tmp419;
_tmp41C;});_LL2A0:;_LL2A1:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp41D=_cycalloc(sizeof(*_tmp41D));_tmp41D[0]=({struct Cyc_Core_Impossible_struct
_tmp41E;_tmp41E.tag=Cyc_Core_Impossible;_tmp41E.f1=({const char*_tmp41F="anal_test, zero-split";
_tag_dynforward(_tmp41F,sizeof(char),_get_zero_arr_size_char(_tmp41F,22));});
_tmp41E;});_tmp41D;}));_LL29B:;}}_LL284: _tmp400=_tmp3F9.f1;if((int)_tmp400 != 0)
goto _LL286;_tmp401=_tmp3F9.f2;if((int)_tmp401 != 0)goto _LL286;_LL285: if(_tmp3C8 == (
void*)5)return({struct _tuple8 _tmp420;_tmp420.f1=f;_tmp420.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp421;(_tmp421.BottomFL).tag=0;_tmp421;});
_tmp420;});else{return({struct _tuple8 _tmp422;_tmp422.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp423;(_tmp423.BottomFL).tag=0;_tmp423;});
_tmp422.f2=f;_tmp422;});}_LL286: _tmp402=_tmp3F9.f1;if((int)_tmp402 != 0)goto
_LL288;_tmp403=_tmp3F9.f2;if((int)_tmp403 != 1)goto _LL288;_LL287: goto _LL289;
_LL288: _tmp404=_tmp3F9.f1;if((int)_tmp404 != 0)goto _LL28A;_tmp405=_tmp3F9.f2;if((
int)_tmp405 != 2)goto _LL28A;_LL289: goto _LL28B;_LL28A: _tmp406=_tmp3F9.f1;if((int)
_tmp406 != 0)goto _LL28C;_tmp407=_tmp3F9.f2;if(_tmp407 <= (void*)3)goto _LL28C;if(*((
int*)_tmp407)!= 2)goto _LL28C;_LL28B: goto _LL28D;_LL28C: _tmp408=_tmp3F9.f1;if((int)
_tmp408 != 1)goto _LL28E;_tmp409=_tmp3F9.f2;if((int)_tmp409 != 0)goto _LL28E;_LL28D:
goto _LL28F;_LL28E: _tmp40A=_tmp3F9.f1;if((int)_tmp40A != 2)goto _LL290;_tmp40B=
_tmp3F9.f2;if((int)_tmp40B != 0)goto _LL290;_LL28F: goto _LL291;_LL290: _tmp40C=
_tmp3F9.f1;if(_tmp40C <= (void*)3)goto _LL292;if(*((int*)_tmp40C)!= 2)goto _LL292;
_tmp40D=_tmp3F9.f2;if((int)_tmp40D != 0)goto _LL292;_LL291: if(_tmp3C8 == (void*)6)
return({struct _tuple8 _tmp424;_tmp424.f1=f;_tmp424.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp425;(_tmp425.BottomFL).tag=0;_tmp425;});
_tmp424;});else{return({struct _tuple8 _tmp426;_tmp426.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp427;(_tmp427.BottomFL).tag=0;_tmp427;});
_tmp426.f2=f;_tmp426;});}_LL292:;_LL293: goto _LL27F;_LL27F:;}{struct _tuple0
_tmp429=({struct _tuple0 _tmp428;_tmp428.f1=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3F2->topt))->v);_tmp428.f2=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp3F3->topt))->v);_tmp428;});void*_tmp42A;void*
_tmp42B;void*_tmp42C;void*_tmp42D;void*_tmp42E;void*_tmp42F;_LL2A3: _tmp42A=
_tmp429.f1;if(_tmp42A <= (void*)4)goto _LL2A5;if(*((int*)_tmp42A)!= 5)goto _LL2A5;
_tmp42B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp42A)->f1;if((int)_tmp42B != 
1)goto _LL2A5;_LL2A4: goto _LL2A6;_LL2A5: _tmp42C=_tmp429.f2;if(_tmp42C <= (void*)4)
goto _LL2A7;if(*((int*)_tmp42C)!= 5)goto _LL2A7;_tmp42D=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp42C)->f1;if((int)_tmp42D != 1)goto _LL2A7;_LL2A6: goto _LL2A8;_LL2A7: _tmp42E=
_tmp429.f1;if(_tmp42E <= (void*)4)goto _LL2A9;if(*((int*)_tmp42E)!= 18)goto _LL2A9;
_LL2A8: goto _LL2AA;_LL2A9: _tmp42F=_tmp429.f2;if(_tmp42F <= (void*)4)goto _LL2AB;if(*((
int*)_tmp42F)!= 18)goto _LL2AB;_LL2AA: goto _LL2A2;_LL2AB:;_LL2AC: return({struct
_tuple8 _tmp430;_tmp430.f1=f;_tmp430.f2=f;_tmp430;});_LL2A2:;}{void*_tmp431=
_tmp3C8;_LL2AE: if((int)_tmp431 != 5)goto _LL2B0;_LL2AF: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp432=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3F2,r1,(void*)5,r2);_tmp432=Cyc_NewControlFlow_if_tagcmp(
env,_tmp432,_tmp3F3,r2,(void*)5,r1);return({struct _tuple8 _tmp433;_tmp433.f1=
_tmp432;_tmp433.f2=f;_tmp433;});}_LL2B0: if((int)_tmp431 != 6)goto _LL2B2;_LL2B1: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp434=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp3F2,r1,(void*)5,r2);_tmp434=Cyc_NewControlFlow_if_tagcmp(env,_tmp434,_tmp3F3,
r2,(void*)5,r1);return({struct _tuple8 _tmp435;_tmp435.f1=f;_tmp435.f2=_tmp434;
_tmp435;});}_LL2B2: if((int)_tmp431 != 7)goto _LL2B4;_LL2B3: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp436=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3F3,r2,(void*)8,r1);union Cyc_CfFlowInfo_FlowInfo_union
_tmp437=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3F2,r1,(void*)10,r2);return({
struct _tuple8 _tmp438;_tmp438.f1=_tmp436;_tmp438.f2=_tmp437;_tmp438;});}_LL2B4:
if((int)_tmp431 != 9)goto _LL2B6;_LL2B5: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp439=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3F3,r2,(void*)10,r1);union Cyc_CfFlowInfo_FlowInfo_union
_tmp43A=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3F2,r1,(void*)8,r2);return({
struct _tuple8 _tmp43B;_tmp43B.f1=_tmp439;_tmp43B.f2=_tmp43A;_tmp43B;});}_LL2B6:
if((int)_tmp431 != 8)goto _LL2B8;_LL2B7: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp43C=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3F2,r1,(void*)8,r2);union Cyc_CfFlowInfo_FlowInfo_union
_tmp43D=Cyc_NewControlFlow_if_tagcmp(env,f,_tmp3F3,r2,(void*)10,r1);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp43E=_tmp43C;struct Cyc_Dict_Dict _tmp43F;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp440;_LL2BD: if((_tmp43E.BottomFL).tag != 0)goto _LL2BF;_LL2BE:(int)_throw((void*)({
struct Cyc_Core_Impossible_struct*_tmp441=_cycalloc(sizeof(*_tmp441));_tmp441[0]=({
struct Cyc_Core_Impossible_struct _tmp442;_tmp442.tag=Cyc_Core_Impossible;_tmp442.f1=({
const char*_tmp443="anal_test, Lt";_tag_dynforward(_tmp443,sizeof(char),
_get_zero_arr_size_char(_tmp443,14));});_tmp442;});_tmp441;}));_LL2BF: if((
_tmp43E.ReachableFL).tag != 1)goto _LL2BC;_tmp43F=(_tmp43E.ReachableFL).f1;_tmp440=(
_tmp43E.ReachableFL).f3;_LL2C0: _tmp3EE=_tmp43F;_tmp3F0=_tmp440;_LL2BC:;}{void*
_tmp444=(void*)_tmp3F2->r;void*_tmp445;struct Cyc_Absyn_Vardecl*_tmp446;void*
_tmp447;struct Cyc_Absyn_Vardecl*_tmp448;void*_tmp449;struct Cyc_Absyn_Vardecl*
_tmp44A;void*_tmp44B;struct Cyc_Absyn_Vardecl*_tmp44C;_LL2C2: if(*((int*)_tmp444)
!= 1)goto _LL2C4;_tmp445=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp444)->f2;if(
_tmp445 <= (void*)1)goto _LL2C4;if(*((int*)_tmp445)!= 0)goto _LL2C4;_tmp446=((
struct Cyc_Absyn_Global_b_struct*)_tmp445)->f1;if(!(!_tmp446->escapes))goto _LL2C4;
_LL2C3: _tmp448=_tmp446;goto _LL2C5;_LL2C4: if(*((int*)_tmp444)!= 1)goto _LL2C6;
_tmp447=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp444)->f2;if(_tmp447 <= (void*)
1)goto _LL2C6;if(*((int*)_tmp447)!= 3)goto _LL2C6;_tmp448=((struct Cyc_Absyn_Local_b_struct*)
_tmp447)->f1;if(!(!_tmp448->escapes))goto _LL2C6;_LL2C5: _tmp44A=_tmp448;goto
_LL2C7;_LL2C6: if(*((int*)_tmp444)!= 1)goto _LL2C8;_tmp449=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp444)->f2;if(_tmp449 <= (void*)1)goto _LL2C8;if(*((int*)_tmp449)!= 4)goto _LL2C8;
_tmp44A=((struct Cyc_Absyn_Pat_b_struct*)_tmp449)->f1;if(!(!_tmp44A->escapes))
goto _LL2C8;_LL2C7: _tmp44C=_tmp44A;goto _LL2C9;_LL2C8: if(*((int*)_tmp444)!= 1)goto
_LL2CA;_tmp44B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp444)->f2;if(_tmp44B <= (
void*)1)goto _LL2CA;if(*((int*)_tmp44B)!= 2)goto _LL2CA;_tmp44C=((struct Cyc_Absyn_Param_b_struct*)
_tmp44B)->f1;if(!(!_tmp44C->escapes))goto _LL2CA;_LL2C9: {void*_tmp44D=(void*)
_tmp3F3->r;void*_tmp44E;struct Cyc_Absyn_Vardecl*_tmp44F;void*_tmp450;struct Cyc_Absyn_Vardecl*
_tmp451;void*_tmp452;struct Cyc_Absyn_Vardecl*_tmp453;void*_tmp454;struct Cyc_Absyn_Vardecl*
_tmp455;union Cyc_Absyn_Cnst_union _tmp456;int _tmp457;void*_tmp458;struct Cyc_List_List*
_tmp459;struct Cyc_List_List _tmp45A;struct Cyc_Absyn_Exp*_tmp45B;_LL2CD: if(*((int*)
_tmp44D)!= 1)goto _LL2CF;_tmp44E=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp44D)->f2;
if(_tmp44E <= (void*)1)goto _LL2CF;if(*((int*)_tmp44E)!= 0)goto _LL2CF;_tmp44F=((
struct Cyc_Absyn_Global_b_struct*)_tmp44E)->f1;if(!(!_tmp44F->escapes))goto _LL2CF;
_LL2CE: _tmp451=_tmp44F;goto _LL2D0;_LL2CF: if(*((int*)_tmp44D)!= 1)goto _LL2D1;
_tmp450=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp44D)->f2;if(_tmp450 <= (void*)
1)goto _LL2D1;if(*((int*)_tmp450)!= 3)goto _LL2D1;_tmp451=((struct Cyc_Absyn_Local_b_struct*)
_tmp450)->f1;if(!(!_tmp451->escapes))goto _LL2D1;_LL2D0: _tmp453=_tmp451;goto
_LL2D2;_LL2D1: if(*((int*)_tmp44D)!= 1)goto _LL2D3;_tmp452=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp44D)->f2;if(_tmp452 <= (void*)1)goto _LL2D3;if(*((int*)_tmp452)!= 4)goto _LL2D3;
_tmp453=((struct Cyc_Absyn_Pat_b_struct*)_tmp452)->f1;if(!(!_tmp453->escapes))
goto _LL2D3;_LL2D2: _tmp455=_tmp453;goto _LL2D4;_LL2D3: if(*((int*)_tmp44D)!= 1)goto
_LL2D5;_tmp454=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp44D)->f2;if(_tmp454 <= (
void*)1)goto _LL2D5;if(*((int*)_tmp454)!= 2)goto _LL2D5;_tmp455=((struct Cyc_Absyn_Param_b_struct*)
_tmp454)->f1;if(!(!_tmp455->escapes))goto _LL2D5;_LL2D4: {struct _RegionHandle*
_tmp45C=(env->fenv)->r;struct Cyc_List_List*_tmp45D=({struct Cyc_List_List*_tmp460=
_region_malloc(_tmp45C,sizeof(*_tmp460));_tmp460->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp461=_region_malloc(_tmp45C,sizeof(*_tmp461));_tmp461->vd=_tmp44C;_tmp461->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp462;(
_tmp462.LessVar).tag=1;(_tmp462.LessVar).f1=_tmp455;_tmp462;});_tmp461;});
_tmp460->tl=_tmp3EF;_tmp460;});return({struct _tuple8 _tmp45E;_tmp45E.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp45F;(_tmp45F.ReachableFL).tag=1;(_tmp45F.ReachableFL).f1=
_tmp3EE;(_tmp45F.ReachableFL).f2=_tmp45D;(_tmp45F.ReachableFL).f3=_tmp3F0;
_tmp45F;});_tmp45E.f2=_tmp43D;_tmp45E;});}_LL2D5: if(*((int*)_tmp44D)!= 0)goto
_LL2D7;_tmp456=((struct Cyc_Absyn_Const_e_struct*)_tmp44D)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp44D)->f1).Int_c).tag != 2)goto _LL2D7;_tmp457=(_tmp456.Int_c).f2;_LL2D6: {
struct _RegionHandle*_tmp463=(env->fenv)->r;struct Cyc_List_List*_tmp464=({struct
Cyc_List_List*_tmp467=_region_malloc(_tmp463,sizeof(*_tmp467));_tmp467->hd=({
struct Cyc_CfFlowInfo_Reln*_tmp468=_region_malloc(_tmp463,sizeof(*_tmp468));
_tmp468->vd=_tmp44C;_tmp468->rop=(union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union
_tmp469;(_tmp469.LessConst).tag=3;(_tmp469.LessConst).f1=(unsigned int)_tmp457;
_tmp469;});_tmp468;});_tmp467->tl=_tmp3EF;_tmp467;});return({struct _tuple8
_tmp465;_tmp465.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp466;(_tmp466.ReachableFL).tag=1;(_tmp466.ReachableFL).f1=_tmp3EE;(_tmp466.ReachableFL).f2=
_tmp464;(_tmp466.ReachableFL).f3=_tmp3F0;_tmp466;});_tmp465.f2=_tmp43D;_tmp465;});}
_LL2D7: if(*((int*)_tmp44D)!= 3)goto _LL2D9;_tmp458=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp44D)->f1;if((int)_tmp458 != 19)goto _LL2D9;_tmp459=((struct Cyc_Absyn_Primop_e_struct*)
_tmp44D)->f2;if(_tmp459 == 0)goto _LL2D9;_tmp45A=*_tmp459;_tmp45B=(struct Cyc_Absyn_Exp*)
_tmp45A.hd;_LL2D8: {void*_tmp46A=(void*)_tmp45B->r;void*_tmp46B;struct Cyc_Absyn_Vardecl*
_tmp46C;void*_tmp46D;struct Cyc_Absyn_Vardecl*_tmp46E;void*_tmp46F;struct Cyc_Absyn_Vardecl*
_tmp470;void*_tmp471;struct Cyc_Absyn_Vardecl*_tmp472;_LL2DC: if(*((int*)_tmp46A)
!= 1)goto _LL2DE;_tmp46B=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp46A)->f2;if(
_tmp46B <= (void*)1)goto _LL2DE;if(*((int*)_tmp46B)!= 0)goto _LL2DE;_tmp46C=((
struct Cyc_Absyn_Global_b_struct*)_tmp46B)->f1;if(!(!_tmp46C->escapes))goto _LL2DE;
_LL2DD: _tmp46E=_tmp46C;goto _LL2DF;_LL2DE: if(*((int*)_tmp46A)!= 1)goto _LL2E0;
_tmp46D=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp46A)->f2;if(_tmp46D <= (void*)
1)goto _LL2E0;if(*((int*)_tmp46D)!= 3)goto _LL2E0;_tmp46E=((struct Cyc_Absyn_Local_b_struct*)
_tmp46D)->f1;if(!(!_tmp46E->escapes))goto _LL2E0;_LL2DF: _tmp470=_tmp46E;goto
_LL2E1;_LL2E0: if(*((int*)_tmp46A)!= 1)goto _LL2E2;_tmp46F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp46A)->f2;if(_tmp46F <= (void*)1)goto _LL2E2;if(*((int*)_tmp46F)!= 4)goto _LL2E2;
_tmp470=((struct Cyc_Absyn_Pat_b_struct*)_tmp46F)->f1;if(!(!_tmp470->escapes))
goto _LL2E2;_LL2E1: _tmp472=_tmp470;goto _LL2E3;_LL2E2: if(*((int*)_tmp46A)!= 1)goto
_LL2E4;_tmp471=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp46A)->f2;if(_tmp471 <= (
void*)1)goto _LL2E4;if(*((int*)_tmp471)!= 2)goto _LL2E4;_tmp472=((struct Cyc_Absyn_Param_b_struct*)
_tmp471)->f1;if(!(!_tmp472->escapes))goto _LL2E4;_LL2E3: {struct _RegionHandle*
_tmp473=(env->fenv)->r;struct Cyc_List_List*_tmp474=({struct Cyc_List_List*_tmp477=
_region_malloc(_tmp473,sizeof(*_tmp477));_tmp477->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp478=_region_malloc(_tmp473,sizeof(*_tmp478));_tmp478->vd=_tmp44C;_tmp478->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp479;(
_tmp479.LessSize).tag=2;(_tmp479.LessSize).f1=_tmp472;_tmp479;});_tmp478;});
_tmp477->tl=_tmp3EF;_tmp477;});return({struct _tuple8 _tmp475;_tmp475.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp476;(_tmp476.ReachableFL).tag=1;(_tmp476.ReachableFL).f1=
_tmp3EE;(_tmp476.ReachableFL).f2=_tmp474;(_tmp476.ReachableFL).f3=_tmp3F0;
_tmp476;});_tmp475.f2=_tmp43D;_tmp475;});}_LL2E4:;_LL2E5: return({struct _tuple8
_tmp47A;_tmp47A.f1=_tmp43C;_tmp47A.f2=_tmp43D;_tmp47A;});_LL2DB:;}_LL2D9:;_LL2DA:
return({struct _tuple8 _tmp47B;_tmp47B.f1=_tmp43C;_tmp47B.f2=_tmp43D;_tmp47B;});
_LL2CC:;}_LL2CA:;_LL2CB: return({struct _tuple8 _tmp47C;_tmp47C.f1=_tmp43C;_tmp47C.f2=
_tmp43D;_tmp47C;});_LL2C1:;}}_LL2B8: if((int)_tmp431 != 10)goto _LL2BA;_LL2B9: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp47D=Cyc_NewControlFlow_if_tagcmp(env,f,
_tmp3F2,r1,(void*)10,r2);union Cyc_CfFlowInfo_FlowInfo_union _tmp47E=Cyc_NewControlFlow_if_tagcmp(
env,f,_tmp3F3,r1,(void*)8,r1);{union Cyc_CfFlowInfo_FlowInfo_union _tmp47F=_tmp47D;
struct Cyc_Dict_Dict _tmp480;struct Cyc_CfFlowInfo_ConsumeInfo _tmp481;_LL2E7: if((
_tmp47F.BottomFL).tag != 0)goto _LL2E9;_LL2E8:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp482=_cycalloc(sizeof(*_tmp482));_tmp482[0]=({struct Cyc_Core_Impossible_struct
_tmp483;_tmp483.tag=Cyc_Core_Impossible;_tmp483.f1=({const char*_tmp484="anal_test, Lte";
_tag_dynforward(_tmp484,sizeof(char),_get_zero_arr_size_char(_tmp484,15));});
_tmp483;});_tmp482;}));_LL2E9: if((_tmp47F.ReachableFL).tag != 1)goto _LL2E6;
_tmp480=(_tmp47F.ReachableFL).f1;_tmp481=(_tmp47F.ReachableFL).f3;_LL2EA: _tmp3EE=
_tmp480;_tmp3F0=_tmp481;_LL2E6:;}{void*_tmp485=(void*)_tmp3F2->r;void*_tmp486;
struct Cyc_Absyn_Vardecl*_tmp487;void*_tmp488;struct Cyc_Absyn_Vardecl*_tmp489;
void*_tmp48A;struct Cyc_Absyn_Vardecl*_tmp48B;void*_tmp48C;struct Cyc_Absyn_Vardecl*
_tmp48D;_LL2EC: if(*((int*)_tmp485)!= 1)goto _LL2EE;_tmp486=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp485)->f2;if(_tmp486 <= (void*)1)goto _LL2EE;if(*((int*)_tmp486)!= 0)goto _LL2EE;
_tmp487=((struct Cyc_Absyn_Global_b_struct*)_tmp486)->f1;if(!(!_tmp487->escapes))
goto _LL2EE;_LL2ED: _tmp489=_tmp487;goto _LL2EF;_LL2EE: if(*((int*)_tmp485)!= 1)goto
_LL2F0;_tmp488=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp485)->f2;if(_tmp488 <= (
void*)1)goto _LL2F0;if(*((int*)_tmp488)!= 3)goto _LL2F0;_tmp489=((struct Cyc_Absyn_Local_b_struct*)
_tmp488)->f1;if(!(!_tmp489->escapes))goto _LL2F0;_LL2EF: _tmp48B=_tmp489;goto
_LL2F1;_LL2F0: if(*((int*)_tmp485)!= 1)goto _LL2F2;_tmp48A=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp485)->f2;if(_tmp48A <= (void*)1)goto _LL2F2;if(*((int*)_tmp48A)!= 4)goto _LL2F2;
_tmp48B=((struct Cyc_Absyn_Pat_b_struct*)_tmp48A)->f1;if(!(!_tmp48B->escapes))
goto _LL2F2;_LL2F1: _tmp48D=_tmp48B;goto _LL2F3;_LL2F2: if(*((int*)_tmp485)!= 1)goto
_LL2F4;_tmp48C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp485)->f2;if(_tmp48C <= (
void*)1)goto _LL2F4;if(*((int*)_tmp48C)!= 2)goto _LL2F4;_tmp48D=((struct Cyc_Absyn_Param_b_struct*)
_tmp48C)->f1;if(!(!_tmp48D->escapes))goto _LL2F4;_LL2F3: {void*_tmp48E=(void*)
_tmp3F3->r;void*_tmp48F;struct Cyc_List_List*_tmp490;struct Cyc_List_List _tmp491;
struct Cyc_Absyn_Exp*_tmp492;_LL2F7: if(*((int*)_tmp48E)!= 3)goto _LL2F9;_tmp48F=(
void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp48E)->f1;if((int)_tmp48F != 19)goto
_LL2F9;_tmp490=((struct Cyc_Absyn_Primop_e_struct*)_tmp48E)->f2;if(_tmp490 == 0)
goto _LL2F9;_tmp491=*_tmp490;_tmp492=(struct Cyc_Absyn_Exp*)_tmp491.hd;_LL2F8: {
void*_tmp493=(void*)_tmp492->r;void*_tmp494;struct Cyc_Absyn_Vardecl*_tmp495;void*
_tmp496;struct Cyc_Absyn_Vardecl*_tmp497;void*_tmp498;struct Cyc_Absyn_Vardecl*
_tmp499;void*_tmp49A;struct Cyc_Absyn_Vardecl*_tmp49B;_LL2FC: if(*((int*)_tmp493)
!= 1)goto _LL2FE;_tmp494=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp493)->f2;if(
_tmp494 <= (void*)1)goto _LL2FE;if(*((int*)_tmp494)!= 0)goto _LL2FE;_tmp495=((
struct Cyc_Absyn_Global_b_struct*)_tmp494)->f1;if(!(!_tmp495->escapes))goto _LL2FE;
_LL2FD: _tmp497=_tmp495;goto _LL2FF;_LL2FE: if(*((int*)_tmp493)!= 1)goto _LL300;
_tmp496=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp493)->f2;if(_tmp496 <= (void*)
1)goto _LL300;if(*((int*)_tmp496)!= 3)goto _LL300;_tmp497=((struct Cyc_Absyn_Local_b_struct*)
_tmp496)->f1;if(!(!_tmp497->escapes))goto _LL300;_LL2FF: _tmp499=_tmp497;goto
_LL301;_LL300: if(*((int*)_tmp493)!= 1)goto _LL302;_tmp498=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp493)->f2;if(_tmp498 <= (void*)1)goto _LL302;if(*((int*)_tmp498)!= 4)goto _LL302;
_tmp499=((struct Cyc_Absyn_Pat_b_struct*)_tmp498)->f1;if(!(!_tmp499->escapes))
goto _LL302;_LL301: _tmp49B=_tmp499;goto _LL303;_LL302: if(*((int*)_tmp493)!= 1)goto
_LL304;_tmp49A=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp493)->f2;if(_tmp49A <= (
void*)1)goto _LL304;if(*((int*)_tmp49A)!= 2)goto _LL304;_tmp49B=((struct Cyc_Absyn_Param_b_struct*)
_tmp49A)->f1;if(!(!_tmp49B->escapes))goto _LL304;_LL303: {struct Cyc_CfFlowInfo_FlowEnv*
_tmp49C=env->fenv;struct Cyc_List_List*_tmp49D=({struct Cyc_List_List*_tmp4A0=
_region_malloc(_tmp49C->r,sizeof(*_tmp4A0));_tmp4A0->hd=({struct Cyc_CfFlowInfo_Reln*
_tmp4A1=_region_malloc(_tmp49C->r,sizeof(*_tmp4A1));_tmp4A1->vd=_tmp48D;_tmp4A1->rop=(
union Cyc_CfFlowInfo_RelnOp_union)({union Cyc_CfFlowInfo_RelnOp_union _tmp4A2;(
_tmp4A2.LessEqSize).tag=4;(_tmp4A2.LessEqSize).f1=_tmp49B;_tmp4A2;});_tmp4A1;});
_tmp4A0->tl=_tmp3EF;_tmp4A0;});return({struct _tuple8 _tmp49E;_tmp49E.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp49F;(_tmp49F.ReachableFL).tag=1;(_tmp49F.ReachableFL).f1=
_tmp3EE;(_tmp49F.ReachableFL).f2=_tmp49D;(_tmp49F.ReachableFL).f3=_tmp3F0;
_tmp49F;});_tmp49E.f2=_tmp47E;_tmp49E;});}_LL304:;_LL305: return({struct _tuple8
_tmp4A3;_tmp4A3.f1=_tmp47D;_tmp4A3.f2=_tmp47E;_tmp4A3;});_LL2FB:;}_LL2F9:;_LL2FA:
return({struct _tuple8 _tmp4A4;_tmp4A4.f1=_tmp47D;_tmp4A4.f2=_tmp47E;_tmp4A4;});
_LL2F6:;}_LL2F4:;_LL2F5: return({struct _tuple8 _tmp4A5;_tmp4A5.f1=_tmp47D;_tmp4A5.f2=
_tmp47E;_tmp4A5;});_LL2EB:;}}_LL2BA:;_LL2BB: return({struct _tuple8 _tmp4A6;_tmp4A6.f1=
f;_tmp4A6.f2=f;_tmp4A6;});_LL2AD:;}}_LL27A:;}}_LL278:;_LL279: goto _LL26B;_LL26B:;}{
union Cyc_CfFlowInfo_FlowInfo_union _tmp4A8;void*_tmp4A9;struct _tuple5 _tmp4A7=Cyc_NewControlFlow_anal_Rexp(
env,inflow,e);_tmp4A8=_tmp4A7.f1;_tmp4A9=_tmp4A7.f2;_tmp4A8=Cyc_CfFlowInfo_readthrough_unique_rvals(
e->loc,_tmp4A8);{union Cyc_CfFlowInfo_FlowInfo_union _tmp4AA=_tmp4A8;struct Cyc_Dict_Dict
_tmp4AB;_LL307: if((_tmp4AA.BottomFL).tag != 0)goto _LL309;_LL308: return({struct
_tuple8 _tmp4AC;_tmp4AC.f1=_tmp4A8;_tmp4AC.f2=_tmp4A8;_tmp4AC;});_LL309: if((
_tmp4AA.ReachableFL).tag != 1)goto _LL306;_tmp4AB=(_tmp4AA.ReachableFL).f1;_LL30A: {
void*_tmp4AD=_tmp4A9;void*_tmp4AE;void*_tmp4AF;void*_tmp4B0;_LL30C: if((int)
_tmp4AD != 0)goto _LL30E;_LL30D: return({struct _tuple8 _tmp4B1;_tmp4B1.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4B2;(_tmp4B2.BottomFL).tag=0;_tmp4B2;});
_tmp4B1.f2=_tmp4A8;_tmp4B1;});_LL30E: if((int)_tmp4AD != 2)goto _LL310;_LL30F: goto
_LL311;_LL310: if((int)_tmp4AD != 1)goto _LL312;_LL311: goto _LL313;_LL312: if(_tmp4AD
<= (void*)3)goto _LL314;if(*((int*)_tmp4AD)!= 2)goto _LL314;_LL313: return({struct
_tuple8 _tmp4B3;_tmp4B3.f1=_tmp4A8;_tmp4B3.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4B4;(_tmp4B4.BottomFL).tag=0;_tmp4B4;});
_tmp4B3;});_LL314: if(_tmp4AD <= (void*)3)goto _LL316;if(*((int*)_tmp4AD)!= 0)goto
_LL316;_tmp4AE=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)_tmp4AD)->f1;if((
int)_tmp4AE != 0)goto _LL316;_LL315: goto _LL317;_LL316: if(_tmp4AD <= (void*)3)goto
_LL318;if(*((int*)_tmp4AD)!= 1)goto _LL318;_tmp4AF=(void*)((struct Cyc_CfFlowInfo_Esc_struct*)
_tmp4AD)->f1;if((int)_tmp4AF != 0)goto _LL318;_LL317:({void*_tmp4B5=0;Cyc_Tcutil_terr(
e->loc,({const char*_tmp4B6="expression may not be initialized";_tag_dynforward(
_tmp4B6,sizeof(char),_get_zero_arr_size_char(_tmp4B6,34));}),_tag_dynforward(
_tmp4B5,sizeof(void*),0));});return({struct _tuple8 _tmp4B7;_tmp4B7.f1=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp4B9;(_tmp4B9.BottomFL).tag=0;_tmp4B9;});
_tmp4B7.f2=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp4B8;(_tmp4B8.BottomFL).tag=0;_tmp4B8;});_tmp4B7;});_LL318: if(_tmp4AD <= (void*)
3)goto _LL31A;if(*((int*)_tmp4AD)!= 0)goto _LL31A;_tmp4B0=(void*)((struct Cyc_CfFlowInfo_UnknownR_struct*)
_tmp4AD)->f1;_LL319: return Cyc_NewControlFlow_splitzero(env,inflow,_tmp4A8,e,
_tmp4B0);_LL31A: if(_tmp4AD <= (void*)3)goto _LL31C;if(*((int*)_tmp4AD)!= 1)goto
_LL31C;_LL31B: return({struct _tuple8 _tmp4BA;_tmp4BA.f1=_tmp4A8;_tmp4BA.f2=_tmp4A8;
_tmp4BA;});_LL31C: if(_tmp4AD <= (void*)3)goto _LL31E;if(*((int*)_tmp4AD)!= 3)goto
_LL31E;_LL31D: return({struct _tuple8 _tmp4BB;_tmp4BB.f1=_tmp4A8;_tmp4BB.f2=_tmp4A8;
_tmp4BB;});_LL31E: if(_tmp4AD <= (void*)3)goto _LL30B;if(*((int*)_tmp4AD)!= 4)goto
_LL30B;_LL31F:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp4BC=
_cycalloc(sizeof(*_tmp4BC));_tmp4BC[0]=({struct Cyc_Core_Impossible_struct _tmp4BD;
_tmp4BD.tag=Cyc_Core_Impossible;_tmp4BD.f1=({const char*_tmp4BE="anal_test";
_tag_dynforward(_tmp4BE,sizeof(char),_get_zero_arr_size_char(_tmp4BE,10));});
_tmp4BD;});_tmp4BC;}));_LL30B:;}_LL306:;}}}static void Cyc_NewControlFlow_check_init_params(
struct Cyc_Position_Segment*loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
flow){union Cyc_CfFlowInfo_FlowInfo_union _tmp4BF=flow;struct Cyc_Dict_Dict _tmp4C0;
_LL321: if((_tmp4BF.BottomFL).tag != 0)goto _LL323;_LL322: return;_LL323: if((_tmp4BF.ReachableFL).tag
!= 1)goto _LL320;_tmp4C0=(_tmp4BF.ReachableFL).f1;_LL324:{struct Cyc_List_List*
_tmp4C1=env->param_roots;for(0;_tmp4C1 != 0;_tmp4C1=_tmp4C1->tl){if(Cyc_CfFlowInfo_initlevel(
_tmp4C0,Cyc_CfFlowInfo_lookup_place(_tmp4C0,(struct Cyc_CfFlowInfo_Place*)_tmp4C1->hd))
!= (void*)2)({void*_tmp4C2=0;Cyc_Tcutil_terr(loc,({const char*_tmp4C3="function may not initialize all the parameters with attribute 'initializes'";
_tag_dynforward(_tmp4C3,sizeof(char),_get_zero_arr_size_char(_tmp4C3,76));}),
_tag_dynforward(_tmp4C2,sizeof(void*),0));});}}return;_LL320:;}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_List_List*scs){struct Cyc_CfFlowInfo_FlowEnv*_tmp4C4=env->fenv;
for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause _tmp4C6;struct Cyc_Core_Opt*
_tmp4C7;struct Cyc_Absyn_Exp*_tmp4C8;struct Cyc_Absyn_Stmt*_tmp4C9;struct Cyc_Position_Segment*
_tmp4CA;struct Cyc_Absyn_Switch_clause*_tmp4C5=(struct Cyc_Absyn_Switch_clause*)
scs->hd;_tmp4C6=*_tmp4C5;_tmp4C7=_tmp4C6.pat_vars;_tmp4C8=_tmp4C6.where_clause;
_tmp4C9=_tmp4C6.body;_tmp4CA=_tmp4C6.loc;{union Cyc_CfFlowInfo_FlowInfo_union
clause_inflow=Cyc_NewControlFlow_add_vars(_tmp4C4,inflow,(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp4C7))->v,(void*)_tmp4C4->unknown_all,_tmp4CA);
union Cyc_CfFlowInfo_FlowInfo_union clause_outflow;if(_tmp4C8 != 0){struct Cyc_Absyn_Exp*
wexp=(struct Cyc_Absyn_Exp*)_tmp4C8;union Cyc_CfFlowInfo_FlowInfo_union _tmp4CC;
union Cyc_CfFlowInfo_FlowInfo_union _tmp4CD;struct _tuple8 _tmp4CB=Cyc_NewControlFlow_anal_test(
env,clause_inflow,wexp);_tmp4CC=_tmp4CB.f1;_tmp4CD=_tmp4CB.f2;_tmp4CC=Cyc_CfFlowInfo_readthrough_unique_rvals(
wexp->loc,_tmp4CC);_tmp4CD=Cyc_CfFlowInfo_readthrough_unique_rvals(wexp->loc,
_tmp4CD);inflow=_tmp4CD;clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp4CC,
_tmp4C9);}else{clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,
_tmp4C9);}{union Cyc_CfFlowInfo_FlowInfo_union _tmp4CE=clause_outflow;_LL326: if((
_tmp4CE.BottomFL).tag != 0)goto _LL328;_LL327: goto _LL325;_LL328:;_LL329: if(scs->tl
== 0)return clause_outflow;else{if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(
scs->tl))->hd)->pat_vars))->v != 0)({void*_tmp4CF=0;Cyc_Tcutil_terr(_tmp4C9->loc,({
const char*_tmp4D0="switch clause may implicitly fallthru";_tag_dynforward(
_tmp4D0,sizeof(char),_get_zero_arr_size_char(_tmp4D0,38));}),_tag_dynforward(
_tmp4CF,sizeof(void*),0));});else{({void*_tmp4D1=0;Cyc_Tcutil_warn(_tmp4C9->loc,({
const char*_tmp4D2="switch clause may implicitly fallthru";_tag_dynforward(
_tmp4D2,sizeof(char),_get_zero_arr_size_char(_tmp4D2,38));}),_tag_dynforward(
_tmp4D1,sizeof(void*),0));});}Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((
struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}goto
_LL325;_LL325:;}}}return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp4D3;(_tmp4D3.BottomFL).tag=0;_tmp4D3;});}static union Cyc_CfFlowInfo_FlowInfo_union
Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_Absyn_Stmt*s){union Cyc_CfFlowInfo_FlowInfo_union outflow;struct
Cyc_NewControlFlow_CFStmtAnnot*_tmp4D5;union Cyc_CfFlowInfo_FlowInfo_union*
_tmp4D6;struct _tuple9 _tmp4D4=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);
_tmp4D5=_tmp4D4.f1;_tmp4D6=_tmp4D4.f2;inflow=*_tmp4D6;{struct Cyc_CfFlowInfo_FlowEnv*
_tmp4D7=env->fenv;{void*_tmp4D8=(void*)s->r;struct Cyc_Absyn_Exp*_tmp4D9;struct
Cyc_Absyn_Exp*_tmp4DA;struct Cyc_Absyn_Exp*_tmp4DB;struct Cyc_Absyn_Stmt*_tmp4DC;
struct Cyc_Absyn_Stmt*_tmp4DD;struct Cyc_Absyn_Exp*_tmp4DE;struct Cyc_Absyn_Stmt*
_tmp4DF;struct Cyc_Absyn_Stmt*_tmp4E0;struct _tuple3 _tmp4E1;struct Cyc_Absyn_Exp*
_tmp4E2;struct Cyc_Absyn_Stmt*_tmp4E3;struct Cyc_Absyn_Stmt*_tmp4E4;struct Cyc_Absyn_Stmt*
_tmp4E5;struct _tuple3 _tmp4E6;struct Cyc_Absyn_Exp*_tmp4E7;struct Cyc_Absyn_Stmt*
_tmp4E8;struct Cyc_Absyn_Exp*_tmp4E9;struct _tuple3 _tmp4EA;struct Cyc_Absyn_Exp*
_tmp4EB;struct Cyc_Absyn_Stmt*_tmp4EC;struct _tuple3 _tmp4ED;struct Cyc_Absyn_Exp*
_tmp4EE;struct Cyc_Absyn_Stmt*_tmp4EF;struct Cyc_Absyn_Stmt*_tmp4F0;struct Cyc_Absyn_Stmt*
_tmp4F1;struct Cyc_List_List*_tmp4F2;struct Cyc_Absyn_Switch_clause**_tmp4F3;
struct Cyc_Absyn_Switch_clause*_tmp4F4;struct Cyc_Absyn_Stmt*_tmp4F5;struct Cyc_Absyn_Stmt*
_tmp4F6;struct Cyc_Absyn_Stmt*_tmp4F7;struct Cyc_Absyn_Exp*_tmp4F8;struct Cyc_List_List*
_tmp4F9;struct Cyc_Absyn_Stmt*_tmp4FA;struct Cyc_List_List*_tmp4FB;struct Cyc_Absyn_Decl*
_tmp4FC;struct Cyc_Absyn_Stmt*_tmp4FD;struct Cyc_Absyn_Stmt*_tmp4FE;struct Cyc_Absyn_Tvar*
_tmp4FF;struct Cyc_Absyn_Vardecl*_tmp500;int _tmp501;struct Cyc_Absyn_Exp*_tmp502;
struct Cyc_Absyn_Stmt*_tmp503;struct Cyc_Absyn_Exp*_tmp504;struct Cyc_Absyn_Exp*
_tmp505;struct Cyc_Absyn_Tvar*_tmp506;struct Cyc_Absyn_Vardecl*_tmp507;struct Cyc_Absyn_Stmt*
_tmp508;_LL32B: if((int)_tmp4D8 != 0)goto _LL32D;_LL32C: return inflow;_LL32D: if(
_tmp4D8 <= (void*)1)goto _LL355;if(*((int*)_tmp4D8)!= 2)goto _LL32F;_tmp4D9=((
struct Cyc_Absyn_Return_s_struct*)_tmp4D8)->f1;if(_tmp4D9 != 0)goto _LL32F;_LL32E:
if(env->noreturn)({void*_tmp509=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp50A="`noreturn' function might return";
_tag_dynforward(_tmp50A,sizeof(char),_get_zero_arr_size_char(_tmp50A,33));}),
_tag_dynforward(_tmp509,sizeof(void*),0));});Cyc_NewControlFlow_check_init_params(
s->loc,env,inflow);return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp50B;(_tmp50B.BottomFL).tag=0;_tmp50B;});_LL32F: if(*((int*)_tmp4D8)!= 2)goto
_LL331;_tmp4DA=((struct Cyc_Absyn_Return_s_struct*)_tmp4D8)->f1;_LL330: if(env->noreturn)({
void*_tmp50C=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp50D="`noreturn' function might return";
_tag_dynforward(_tmp50D,sizeof(char),_get_zero_arr_size_char(_tmp50D,33));}),
_tag_dynforward(_tmp50C,sizeof(void*),0));});{union Cyc_CfFlowInfo_FlowInfo_union
_tmp50F;void*_tmp510;struct _tuple5 _tmp50E=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp4DA));_tmp50F=_tmp50E.f1;_tmp510=
_tmp50E.f2;_tmp50F=Cyc_CfFlowInfo_consume_unique_rvals(_tmp4DA->loc,_tmp50F);
_tmp50F=Cyc_NewControlFlow_use_Rval(env,_tmp4DA->loc,_tmp50F,_tmp510);Cyc_NewControlFlow_check_init_params(
s->loc,env,_tmp50F);return(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp511;(_tmp511.BottomFL).tag=0;_tmp511;});}_LL331: if(*((int*)_tmp4D8)!= 0)goto
_LL333;_tmp4DB=((struct Cyc_Absyn_Exp_s_struct*)_tmp4D8)->f1;_LL332: outflow=(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4DB)).f1;goto _LL32A;_LL333: if(*((int*)_tmp4D8)!= 1)goto _LL335;
_tmp4DC=((struct Cyc_Absyn_Seq_s_struct*)_tmp4D8)->f1;_tmp4DD=((struct Cyc_Absyn_Seq_s_struct*)
_tmp4D8)->f2;_LL334: outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp4DC),_tmp4DD);goto _LL32A;_LL335: if(*((int*)_tmp4D8)!= 3)goto
_LL337;_tmp4DE=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp4D8)->f1;_tmp4DF=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp4D8)->f2;_tmp4E0=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp4D8)->f3;_LL336: {union Cyc_CfFlowInfo_FlowInfo_union _tmp513;union Cyc_CfFlowInfo_FlowInfo_union
_tmp514;struct _tuple8 _tmp512=Cyc_NewControlFlow_anal_test(env,inflow,_tmp4DE);
_tmp513=_tmp512.f1;_tmp514=_tmp512.f2;_tmp513=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp4DE->loc,_tmp513);_tmp514=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4DE->loc,
_tmp514);outflow=Cyc_CfFlowInfo_join_flow(_tmp4D7,env->all_changed,Cyc_NewControlFlow_anal_stmt(
env,_tmp513,_tmp4DF),Cyc_NewControlFlow_anal_stmt(env,_tmp514,_tmp4E0),1);goto
_LL32A;}_LL337: if(*((int*)_tmp4D8)!= 4)goto _LL339;_tmp4E1=((struct Cyc_Absyn_While_s_struct*)
_tmp4D8)->f1;_tmp4E2=_tmp4E1.f1;_tmp4E3=_tmp4E1.f2;_tmp4E4=((struct Cyc_Absyn_While_s_struct*)
_tmp4D8)->f2;_LL338: {union Cyc_CfFlowInfo_FlowInfo_union*_tmp516;struct _tuple9
_tmp515=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp4E3);_tmp516=_tmp515.f2;{
union Cyc_CfFlowInfo_FlowInfo_union _tmp517=*_tmp516;union Cyc_CfFlowInfo_FlowInfo_union
_tmp519;union Cyc_CfFlowInfo_FlowInfo_union _tmp51A;struct _tuple8 _tmp518=Cyc_NewControlFlow_anal_test(
env,_tmp517,_tmp4E2);_tmp519=_tmp518.f1;_tmp51A=_tmp518.f2;_tmp519=Cyc_CfFlowInfo_readthrough_unique_rvals(
_tmp4E2->loc,_tmp519);_tmp51A=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp4E2->loc,
_tmp51A);{union Cyc_CfFlowInfo_FlowInfo_union _tmp51B=Cyc_NewControlFlow_anal_stmt(
env,_tmp519,_tmp4E4);Cyc_NewControlFlow_update_flow(env,_tmp4E3,_tmp51B);outflow=
_tmp51A;goto _LL32A;}}}_LL339: if(*((int*)_tmp4D8)!= 13)goto _LL33B;_tmp4E5=((
struct Cyc_Absyn_Do_s_struct*)_tmp4D8)->f1;_tmp4E6=((struct Cyc_Absyn_Do_s_struct*)
_tmp4D8)->f2;_tmp4E7=_tmp4E6.f1;_tmp4E8=_tmp4E6.f2;_LL33A: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp51C=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4E5);union Cyc_CfFlowInfo_FlowInfo_union*
_tmp51E;struct _tuple9 _tmp51D=Cyc_NewControlFlow_pre_stmt_check(env,_tmp51C,
_tmp4E8);_tmp51E=_tmp51D.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp51F=*_tmp51E;
union Cyc_CfFlowInfo_FlowInfo_union _tmp521;union Cyc_CfFlowInfo_FlowInfo_union
_tmp522;struct _tuple8 _tmp520=Cyc_NewControlFlow_anal_test(env,_tmp51F,_tmp4E7);
_tmp521=_tmp520.f1;_tmp522=_tmp520.f2;Cyc_NewControlFlow_update_flow(env,_tmp4E5,
_tmp521);outflow=_tmp522;goto _LL32A;}}_LL33B: if(*((int*)_tmp4D8)!= 8)goto _LL33D;
_tmp4E9=((struct Cyc_Absyn_For_s_struct*)_tmp4D8)->f1;_tmp4EA=((struct Cyc_Absyn_For_s_struct*)
_tmp4D8)->f2;_tmp4EB=_tmp4EA.f1;_tmp4EC=_tmp4EA.f2;_tmp4ED=((struct Cyc_Absyn_For_s_struct*)
_tmp4D8)->f3;_tmp4EE=_tmp4ED.f1;_tmp4EF=_tmp4ED.f2;_tmp4F0=((struct Cyc_Absyn_For_s_struct*)
_tmp4D8)->f4;_LL33C: {union Cyc_CfFlowInfo_FlowInfo_union _tmp523=(Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4E9)).f1;_tmp523=Cyc_CfFlowInfo_drop_unique_rvals(_tmp4E9->loc,
_tmp523);{union Cyc_CfFlowInfo_FlowInfo_union*_tmp525;struct _tuple9 _tmp524=Cyc_NewControlFlow_pre_stmt_check(
env,_tmp523,_tmp4EC);_tmp525=_tmp524.f2;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp526=*_tmp525;union Cyc_CfFlowInfo_FlowInfo_union _tmp528;union Cyc_CfFlowInfo_FlowInfo_union
_tmp529;struct _tuple8 _tmp527=Cyc_NewControlFlow_anal_test(env,_tmp526,_tmp4EB);
_tmp528=_tmp527.f1;_tmp529=_tmp527.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp52A=
Cyc_NewControlFlow_anal_stmt(env,_tmp528,_tmp4F0);union Cyc_CfFlowInfo_FlowInfo_union*
_tmp52C;struct _tuple9 _tmp52B=Cyc_NewControlFlow_pre_stmt_check(env,_tmp52A,
_tmp4EF);_tmp52C=_tmp52B.f2;{union Cyc_CfFlowInfo_FlowInfo_union _tmp52D=*_tmp52C;
union Cyc_CfFlowInfo_FlowInfo_union _tmp52E=(Cyc_NewControlFlow_anal_Rexp(env,
_tmp52D,_tmp4EE)).f1;_tmp52E=Cyc_CfFlowInfo_drop_unique_rvals(_tmp4EE->loc,
_tmp52E);Cyc_NewControlFlow_update_flow(env,_tmp4EC,_tmp52E);outflow=_tmp529;
goto _LL32A;}}}}}_LL33D: if(*((int*)_tmp4D8)!= 5)goto _LL33F;_tmp4F1=((struct Cyc_Absyn_Break_s_struct*)
_tmp4D8)->f1;if(_tmp4F1 != 0)goto _LL33F;_LL33E: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp52F;(_tmp52F.BottomFL).tag=0;_tmp52F;});
_LL33F: if(*((int*)_tmp4D8)!= 10)goto _LL341;_tmp4F2=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp4D8)->f1;_tmp4F3=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp4D8)->f2;if(
_tmp4F3 == 0)goto _LL341;_tmp4F4=*_tmp4F3;_LL340: {struct _RegionHandle _tmp530=
_new_region("temp");struct _RegionHandle*temp=& _tmp530;_push_region(temp);{union
Cyc_CfFlowInfo_FlowInfo_union _tmp532;struct Cyc_List_List*_tmp533;struct _tuple11
_tmp531=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp4F2,0);
_tmp532=_tmp531.f1;_tmp533=_tmp531.f2;for(0;_tmp533 != 0;(_tmp533=_tmp533->tl,
_tmp4F2=_tmp4F2->tl)){_tmp532=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp4F2))->hd)->loc,_tmp532,(void*)_tmp533->hd);}
_tmp532=Cyc_CfFlowInfo_consume_unique_rvals(s->loc,_tmp532);_tmp532=Cyc_NewControlFlow_add_vars(
_tmp4D7,_tmp532,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp4F4->pat_vars))->v,(
void*)_tmp4D7->unknown_all,s->loc);Cyc_NewControlFlow_update_flow(env,(struct Cyc_Absyn_Stmt*)
_tmp4F4->body,_tmp532);{union Cyc_CfFlowInfo_FlowInfo_union _tmp535=(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp534;(_tmp534.BottomFL).tag=0;_tmp534;});
_npop_handler(0);return _tmp535;}};_pop_region(temp);}_LL341: if(*((int*)_tmp4D8)
!= 5)goto _LL343;_tmp4F5=((struct Cyc_Absyn_Break_s_struct*)_tmp4D8)->f1;_LL342:
_tmp4F6=_tmp4F5;goto _LL344;_LL343: if(*((int*)_tmp4D8)!= 6)goto _LL345;_tmp4F6=((
struct Cyc_Absyn_Continue_s_struct*)_tmp4D8)->f1;_LL344: _tmp4F7=_tmp4F6;goto
_LL346;_LL345: if(*((int*)_tmp4D8)!= 7)goto _LL347;_tmp4F7=((struct Cyc_Absyn_Goto_s_struct*)
_tmp4D8)->f2;_LL346: if(env->iteration_num == 1){struct Cyc_Absyn_Stmt*_tmp536=
_tmp4D5->encloser;struct Cyc_Absyn_Stmt*_tmp537=(Cyc_NewControlFlow_get_stmt_annot((
struct Cyc_Absyn_Stmt*)_check_null(_tmp4F7)))->encloser;while(_tmp537 != _tmp536){
struct Cyc_Absyn_Stmt*_tmp538=(Cyc_NewControlFlow_get_stmt_annot(_tmp536))->encloser;
if(_tmp538 == _tmp536){({void*_tmp539=0;Cyc_Tcutil_terr(s->loc,({const char*
_tmp53A="goto enters local scope or exception handler";_tag_dynforward(_tmp53A,
sizeof(char),_get_zero_arr_size_char(_tmp53A,45));}),_tag_dynforward(_tmp539,
sizeof(void*),0));});break;}_tmp536=_tmp538;}}Cyc_NewControlFlow_update_flow(env,(
struct Cyc_Absyn_Stmt*)_check_null(_tmp4F7),inflow);return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp53B;(_tmp53B.BottomFL).tag=0;_tmp53B;});
_LL347: if(*((int*)_tmp4D8)!= 9)goto _LL349;_tmp4F8=((struct Cyc_Absyn_Switch_s_struct*)
_tmp4D8)->f1;_tmp4F9=((struct Cyc_Absyn_Switch_s_struct*)_tmp4D8)->f2;_LL348: {
union Cyc_CfFlowInfo_FlowInfo_union _tmp53D;void*_tmp53E;struct _tuple5 _tmp53C=Cyc_NewControlFlow_anal_Rexp(
env,inflow,_tmp4F8);_tmp53D=_tmp53C.f1;_tmp53E=_tmp53C.f2;_tmp53D=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp4F8->loc,_tmp53D);_tmp53D=Cyc_NewControlFlow_use_Rval(env,_tmp4F8->loc,
_tmp53D,_tmp53E);outflow=Cyc_NewControlFlow_anal_scs(env,_tmp53D,_tmp4F9);goto
_LL32A;}_LL349: if(*((int*)_tmp4D8)!= 14)goto _LL34B;_tmp4FA=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp4D8)->f1;_tmp4FB=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp4D8)->f2;_LL34A: {
int old_in_try=env->in_try;union Cyc_CfFlowInfo_FlowInfo_union old_tryflow=env->tryflow;
env->in_try=1;env->tryflow=inflow;{union Cyc_CfFlowInfo_FlowInfo_union s1_outflow=
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp4FA);union Cyc_CfFlowInfo_FlowInfo_union
scs_inflow=env->tryflow;env->in_try=old_in_try;env->tryflow=old_tryflow;Cyc_NewControlFlow_update_tryflow(
env,scs_inflow);{union Cyc_CfFlowInfo_FlowInfo_union scs_outflow=Cyc_NewControlFlow_anal_scs(
env,scs_inflow,_tmp4FB);{union Cyc_CfFlowInfo_FlowInfo_union _tmp53F=scs_outflow;
_LL358: if((_tmp53F.BottomFL).tag != 0)goto _LL35A;_LL359: goto _LL357;_LL35A:;_LL35B:({
void*_tmp540=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp541="last catch clause may implicitly fallthru";
_tag_dynforward(_tmp541,sizeof(char),_get_zero_arr_size_char(_tmp541,42));}),
_tag_dynforward(_tmp540,sizeof(void*),0));});_LL357:;}outflow=s1_outflow;goto
_LL32A;}}}_LL34B: if(*((int*)_tmp4D8)!= 11)goto _LL34D;_tmp4FC=((struct Cyc_Absyn_Decl_s_struct*)
_tmp4D8)->f1;_tmp4FD=((struct Cyc_Absyn_Decl_s_struct*)_tmp4D8)->f2;_LL34C:
outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,
_tmp4FC),_tmp4FD);goto _LL32A;_LL34D: if(*((int*)_tmp4D8)!= 12)goto _LL34F;_tmp4FE=((
struct Cyc_Absyn_Label_s_struct*)_tmp4D8)->f2;_LL34E: outflow=Cyc_NewControlFlow_anal_stmt(
env,inflow,_tmp4FE);goto _LL32A;_LL34F: if(*((int*)_tmp4D8)!= 15)goto _LL351;
_tmp4FF=((struct Cyc_Absyn_Region_s_struct*)_tmp4D8)->f1;_tmp500=((struct Cyc_Absyn_Region_s_struct*)
_tmp4D8)->f2;_tmp501=((struct Cyc_Absyn_Region_s_struct*)_tmp4D8)->f3;_tmp502=((
struct Cyc_Absyn_Region_s_struct*)_tmp4D8)->f4;_tmp503=((struct Cyc_Absyn_Region_s_struct*)
_tmp4D8)->f5;_LL350: if(_tmp502 != 0){struct Cyc_Absyn_Exp*_tmp542=(struct Cyc_Absyn_Exp*)
_tmp502;union Cyc_CfFlowInfo_FlowInfo_union _tmp544;void*_tmp545;struct _tuple5
_tmp543=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp542);_tmp544=_tmp543.f1;
_tmp545=_tmp543.f2;_tmp544=Cyc_CfFlowInfo_consume_unique_rvals(_tmp542->loc,
_tmp544);inflow=Cyc_NewControlFlow_use_Rval(env,_tmp542->loc,_tmp544,_tmp545);}{
struct Cyc_List_List _tmp546=({struct Cyc_List_List _tmp547;_tmp547.hd=_tmp500;
_tmp547.tl=0;_tmp547;});inflow=Cyc_NewControlFlow_add_vars(_tmp4D7,inflow,(
struct Cyc_List_List*)& _tmp546,(void*)_tmp4D7->unknown_all,_tmp503->loc);outflow=
Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp503);goto _LL32A;}_LL351: if(*((int*)
_tmp4D8)!= 16)goto _LL353;_tmp504=((struct Cyc_Absyn_ResetRegion_s_struct*)_tmp4D8)->f1;
_LL352: {union Cyc_CfFlowInfo_FlowInfo_union _tmp549;void*_tmp54A;struct _tuple5
_tmp548=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp504);_tmp549=_tmp548.f1;
_tmp54A=_tmp548.f2;_tmp549=Cyc_CfFlowInfo_readthrough_unique_rvals(_tmp504->loc,
_tmp549);{union Cyc_CfFlowInfo_FlowInfo_union _tmp54B=Cyc_NewControlFlow_use_Rval(
env,_tmp504->loc,_tmp549,_tmp54A);{void*_tmp54C=Cyc_Tcutil_compress((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp504->topt))->v);void*_tmp54D;_LL35D: if(
_tmp54C <= (void*)4)goto _LL35F;if(*((int*)_tmp54C)!= 15)goto _LL35F;_tmp54D=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp54C)->f1;_LL35E: outflow=Cyc_CfFlowInfo_kill_flow_region(
_tmp4D7,_tmp549,_tmp54D);goto _LL35C;_LL35F:;_LL360:(int)_throw((void*)({struct
Cyc_Core_Impossible_struct*_tmp54E=_cycalloc(sizeof(*_tmp54E));_tmp54E[0]=({
struct Cyc_Core_Impossible_struct _tmp54F;_tmp54F.tag=Cyc_Core_Impossible;_tmp54F.f1=({
const char*_tmp550="anal_stmt -- reset_region";_tag_dynforward(_tmp550,sizeof(
char),_get_zero_arr_size_char(_tmp550,26));});_tmp54F;});_tmp54E;}));_LL35C:;}
goto _LL32A;}}_LL353: if(*((int*)_tmp4D8)!= 17)goto _LL355;_tmp505=((struct Cyc_Absyn_Alias_s_struct*)
_tmp4D8)->f1;_tmp506=((struct Cyc_Absyn_Alias_s_struct*)_tmp4D8)->f2;_tmp507=((
struct Cyc_Absyn_Alias_s_struct*)_tmp4D8)->f3;_tmp508=((struct Cyc_Absyn_Alias_s_struct*)
_tmp4D8)->f4;_LL354: {union Cyc_CfFlowInfo_FlowInfo_union _tmp552;void*_tmp553;
struct _tuple5 _tmp551=Cyc_NewControlFlow_anal_Rexp(env,inflow,_tmp505);_tmp552=
_tmp551.f1;_tmp553=_tmp551.f2;{struct Cyc_CfFlowInfo_ConsumeInfo _tmp555;struct Cyc_List_List*
_tmp556;struct _tuple6 _tmp554=Cyc_CfFlowInfo_save_consume_info(_tmp4D7,_tmp552,0);
_tmp555=_tmp554.f1;_tmp556=_tmp555.may_consume;_tmp552=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp505->loc,_tmp552);_tmp552=Cyc_NewControlFlow_use_Rval(env,_tmp505->loc,
_tmp552,_tmp553);{struct Cyc_List_List _tmp557=({struct Cyc_List_List _tmp55E;
_tmp55E.hd=_tmp507;_tmp55E.tl=0;_tmp55E;});_tmp552=Cyc_NewControlFlow_add_vars(
_tmp4D7,_tmp552,(struct Cyc_List_List*)& _tmp557,(void*)_tmp4D7->unknown_all,s->loc);
outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp552,_tmp508);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp558=outflow;struct Cyc_Dict_Dict _tmp559;struct Cyc_List_List*_tmp55A;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp55B;_LL362: if((_tmp558.BottomFL).tag != 0)goto _LL364;_LL363: goto _LL361;_LL364:
if((_tmp558.ReachableFL).tag != 1)goto _LL361;_tmp559=(_tmp558.ReachableFL).f1;
_tmp55A=(_tmp558.ReachableFL).f2;_tmp55B=(_tmp558.ReachableFL).f3;_LL365: while(
_tmp556 != 0){struct Cyc_Dict_Dict _tmp55C=_tmp55B.consumed;_tmp55B.consumed=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*))
Cyc_Dict_rdelete)(_tmp4D7->r,_tmp55B.consumed,(struct Cyc_CfFlowInfo_Place*)
_tmp556->hd);if((_tmp55B.consumed).t != _tmp55C.t);_tmp556=_tmp556->tl;}outflow=(
union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union _tmp55D;(
_tmp55D.ReachableFL).tag=1;(_tmp55D.ReachableFL).f1=_tmp559;(_tmp55D.ReachableFL).f2=
_tmp55A;(_tmp55D.ReachableFL).f3=_tmp55B;_tmp55D;});goto _LL361;_LL361:;}goto
_LL32A;}}}_LL355:;_LL356:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp55F=_cycalloc(sizeof(*_tmp55F));_tmp55F[0]=({struct Cyc_Core_Impossible_struct
_tmp560;_tmp560.tag=Cyc_Core_Impossible;_tmp560.f1=({const char*_tmp561="anal_stmt -- bad stmt syntax or unimplemented stmt form";
_tag_dynforward(_tmp561,sizeof(char),_get_zero_arr_size_char(_tmp561,56));});
_tmp560;});_tmp55F;}));_LL32A:;}outflow=Cyc_CfFlowInfo_drop_unique_rvals(s->loc,
outflow);{union Cyc_CfFlowInfo_FlowInfo_union _tmp562=outflow;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp563;_LL367: if((_tmp562.ReachableFL).tag != 1)goto _LL369;_tmp563=(_tmp562.ReachableFL).f3;
_LL368: goto _LL366;_LL369:;_LL36A: goto _LL366;_LL366:;}return outflow;}}static void
Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo_union
inflow,struct Cyc_Absyn_Fndecl*fd);static union Cyc_CfFlowInfo_FlowInfo_union Cyc_NewControlFlow_anal_decl(
struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo_union inflow,
struct Cyc_Absyn_Decl*decl){void*_tmp564=(void*)decl->r;struct Cyc_Absyn_Vardecl*
_tmp565;struct Cyc_Core_Opt*_tmp566;struct Cyc_Core_Opt _tmp567;struct Cyc_List_List*
_tmp568;struct Cyc_Absyn_Exp*_tmp569;struct Cyc_List_List*_tmp56A;struct Cyc_Absyn_Fndecl*
_tmp56B;_LL36C: if(_tmp564 <= (void*)2)goto _LL374;if(*((int*)_tmp564)!= 0)goto
_LL36E;_tmp565=((struct Cyc_Absyn_Var_d_struct*)_tmp564)->f1;_LL36D: {struct Cyc_List_List
_tmp56C=({struct Cyc_List_List _tmp57B;_tmp57B.hd=_tmp565;_tmp57B.tl=0;_tmp57B;});
inflow=Cyc_NewControlFlow_add_vars(env->fenv,inflow,(struct Cyc_List_List*)&
_tmp56C,(void*)(env->fenv)->unknown_none,decl->loc);{struct Cyc_Absyn_Exp*_tmp56D=
_tmp565->initializer;if(_tmp56D == 0)return inflow;{union Cyc_CfFlowInfo_FlowInfo_union
_tmp56F;void*_tmp570;struct _tuple5 _tmp56E=Cyc_NewControlFlow_anal_Rexp(env,
inflow,(struct Cyc_Absyn_Exp*)_tmp56D);_tmp56F=_tmp56E.f1;_tmp570=_tmp56E.f2;
_tmp56F=Cyc_CfFlowInfo_consume_unique_rvals(_tmp56D->loc,_tmp56F);{union Cyc_CfFlowInfo_FlowInfo_union
_tmp571=_tmp56F;struct Cyc_Dict_Dict _tmp572;struct Cyc_List_List*_tmp573;struct Cyc_CfFlowInfo_ConsumeInfo
_tmp574;_LL377: if((_tmp571.BottomFL).tag != 0)goto _LL379;_LL378: return(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp575;(_tmp575.BottomFL).tag=0;_tmp575;});
_LL379: if((_tmp571.ReachableFL).tag != 1)goto _LL376;_tmp572=(_tmp571.ReachableFL).f1;
_tmp573=(_tmp571.ReachableFL).f2;_tmp574=(_tmp571.ReachableFL).f3;_LL37A: _tmp572=
Cyc_CfFlowInfo_assign_place(env->fenv,decl->loc,_tmp572,env->all_changed,({
struct Cyc_CfFlowInfo_Place*_tmp576=_region_malloc(env->r,sizeof(*_tmp576));
_tmp576->root=(void*)((void*)({struct Cyc_CfFlowInfo_VarRoot_struct*_tmp577=
_region_malloc(env->r,sizeof(*_tmp577));_tmp577[0]=({struct Cyc_CfFlowInfo_VarRoot_struct
_tmp578;_tmp578.tag=0;_tmp578.f1=_tmp565;_tmp578;});_tmp577;}));_tmp576->fields=
0;_tmp576;}),_tmp570);_tmp573=Cyc_CfFlowInfo_reln_assign_var(env->r,_tmp573,
_tmp565,(struct Cyc_Absyn_Exp*)_check_null(_tmp565->initializer));{union Cyc_CfFlowInfo_FlowInfo_union
_tmp579=({union Cyc_CfFlowInfo_FlowInfo_union _tmp57A;(_tmp57A.ReachableFL).tag=1;(
_tmp57A.ReachableFL).f1=_tmp572;(_tmp57A.ReachableFL).f2=_tmp573;(_tmp57A.ReachableFL).f3=
_tmp574;_tmp57A;});Cyc_NewControlFlow_update_tryflow(env,(union Cyc_CfFlowInfo_FlowInfo_union)
_tmp579);return(union Cyc_CfFlowInfo_FlowInfo_union)_tmp579;}_LL376:;}}}}_LL36E:
if(*((int*)_tmp564)!= 2)goto _LL370;_tmp566=((struct Cyc_Absyn_Let_d_struct*)
_tmp564)->f2;if(_tmp566 == 0)goto _LL370;_tmp567=*_tmp566;_tmp568=(struct Cyc_List_List*)
_tmp567.v;_tmp569=((struct Cyc_Absyn_Let_d_struct*)_tmp564)->f3;_LL36F: {union Cyc_CfFlowInfo_FlowInfo_union
_tmp57D;void*_tmp57E;struct _tuple5 _tmp57C=Cyc_NewControlFlow_anal_Rexp(env,
inflow,_tmp569);_tmp57D=_tmp57C.f1;_tmp57E=_tmp57C.f2;_tmp57D=Cyc_CfFlowInfo_consume_unique_rvals(
_tmp569->loc,_tmp57D);_tmp57D=Cyc_NewControlFlow_use_Rval(env,_tmp569->loc,
_tmp57D,_tmp57E);return Cyc_NewControlFlow_add_vars(env->fenv,_tmp57D,_tmp568,(
void*)(env->fenv)->unknown_all,decl->loc);}_LL370: if(*((int*)_tmp564)!= 3)goto
_LL372;_tmp56A=((struct Cyc_Absyn_Letv_d_struct*)_tmp564)->f1;_LL371: return Cyc_NewControlFlow_add_vars(
env->fenv,inflow,_tmp56A,(void*)(env->fenv)->unknown_none,decl->loc);_LL372: if(*((
int*)_tmp564)!= 1)goto _LL374;_tmp56B=((struct Cyc_Absyn_Fn_d_struct*)_tmp564)->f1;
_LL373: Cyc_NewControlFlow_check_nested_fun(env->fenv,inflow,_tmp56B);{void*
_tmp57F=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp56B->cached_typ))->v;struct
Cyc_Absyn_Vardecl*_tmp580=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp56B->fn_vardecl);
return Cyc_NewControlFlow_add_vars(env->fenv,inflow,({struct Cyc_List_List*_tmp581=
_region_malloc(env->r,sizeof(*_tmp581));_tmp581->hd=_tmp580;_tmp581->tl=0;
_tmp581;}),(void*)(env->fenv)->unknown_all,decl->loc);}_LL374:;_LL375:(int)
_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp582=_cycalloc(sizeof(*
_tmp582));_tmp582[0]=({struct Cyc_Core_Impossible_struct _tmp583;_tmp583.tag=Cyc_Core_Impossible;
_tmp583.f1=({const char*_tmp584="anal_decl: unexpected decl variant";
_tag_dynforward(_tmp584,sizeof(char),_get_zero_arr_size_char(_tmp584,35));});
_tmp583;});_tmp582;}));_LL36B:;}static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*
fd){struct _RegionHandle _tmp585=_new_region("frgn");struct _RegionHandle*frgn=&
_tmp585;_push_region(frgn);{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(
frgn);Cyc_NewControlFlow_check_nested_fun(fenv,(union Cyc_CfFlowInfo_FlowInfo_union)({
union Cyc_CfFlowInfo_FlowInfo_union _tmp586;(_tmp586.ReachableFL).tag=1;(_tmp586.ReachableFL).f1=
fenv->mt_flowdict;(_tmp586.ReachableFL).f2=0;(_tmp586.ReachableFL).f3=({struct
Cyc_CfFlowInfo_ConsumeInfo _tmp587;_tmp587.consumed=fenv->mt_place_set;_tmp587.may_consume=
0;_tmp587;});_tmp586;}),fd);};_pop_region(frgn);}static int Cyc_NewControlFlow_hash_ptr(
void*s){return(int)s;}static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*
fenv,union Cyc_CfFlowInfo_FlowInfo_union inflow,struct Cyc_Absyn_Fndecl*fd){struct
_RegionHandle*_tmp588=fenv->r;struct Cyc_Position_Segment*_tmp589=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v,(void*)fenv->unknown_all,_tmp589);{struct Cyc_List_List*
param_roots=0;{union Cyc_CfFlowInfo_FlowInfo_union _tmp58A=inflow;struct Cyc_Dict_Dict
_tmp58B;struct Cyc_List_List*_tmp58C;struct Cyc_CfFlowInfo_ConsumeInfo _tmp58D;
_LL37C: if((_tmp58A.BottomFL).tag != 0)goto _LL37E;_LL37D:({void*_tmp58E=0;((int(*)(
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp58F="check_fun";_tag_dynforward(_tmp58F,sizeof(char),_get_zero_arr_size_char(
_tmp58F,10));}),_tag_dynforward(_tmp58E,sizeof(void*),0));});_LL37E: if((_tmp58A.ReachableFL).tag
!= 1)goto _LL37B;_tmp58B=(_tmp58A.ReachableFL).f1;_tmp58C=(_tmp58A.ReachableFL).f2;
_tmp58D=(_tmp58A.ReachableFL).f3;_LL37F: {struct Cyc_List_List*atts;{void*_tmp590=
Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(fd->cached_typ))->v);
struct Cyc_Absyn_FnInfo _tmp591;struct Cyc_List_List*_tmp592;_LL381: if(_tmp590 <= (
void*)4)goto _LL383;if(*((int*)_tmp590)!= 8)goto _LL383;_tmp591=((struct Cyc_Absyn_FnType_struct*)
_tmp590)->f1;_tmp592=_tmp591.attributes;_LL382: atts=_tmp592;goto _LL380;_LL383:;
_LL384:({void*_tmp593=0;((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr
ap))Cyc_Tcutil_impos)(({const char*_tmp594="check_fun: non-function type cached with fndecl_t";
_tag_dynforward(_tmp594,sizeof(char),_get_zero_arr_size_char(_tmp594,50));}),
_tag_dynforward(_tmp593,sizeof(void*),0));});_LL380:;}for(0;atts != 0;atts=atts->tl){
void*_tmp595=(void*)atts->hd;int _tmp596;_LL386: if(_tmp595 <= (void*)17)goto _LL388;
if(*((int*)_tmp595)!= 4)goto _LL388;_tmp596=((struct Cyc_Absyn_Initializes_att_struct*)
_tmp595)->f1;_LL387: {unsigned int j=(unsigned int)_tmp596;if(j > ((int(*)(struct
Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(fd->param_vardecls))->v)){({void*_tmp597=0;Cyc_Tcutil_terr(_tmp589,({
const char*_tmp598="initializes attribute exceeds number of parameters";
_tag_dynforward(_tmp598,sizeof(char),_get_zero_arr_size_char(_tmp598,51));}),
_tag_dynforward(_tmp597,sizeof(void*),0));});continue;}{struct Cyc_Absyn_Vardecl*
_tmp599=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,(
int)(j - 1));{void*_tmp59A=Cyc_Tcutil_compress((void*)_tmp599->type);struct Cyc_Absyn_PtrInfo
_tmp59B;void*_tmp59C;struct Cyc_Absyn_PtrAtts _tmp59D;struct Cyc_Absyn_Conref*
_tmp59E;struct Cyc_Absyn_Conref*_tmp59F;struct Cyc_Absyn_Conref*_tmp5A0;_LL38B: if(
_tmp59A <= (void*)4)goto _LL38D;if(*((int*)_tmp59A)!= 4)goto _LL38D;_tmp59B=((
struct Cyc_Absyn_PointerType_struct*)_tmp59A)->f1;_tmp59C=(void*)_tmp59B.elt_typ;
_tmp59D=_tmp59B.ptr_atts;_tmp59E=_tmp59D.nullable;_tmp59F=_tmp59D.bounds;_tmp5A0=
_tmp59D.zero_term;_LL38C: if(((int(*)(struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_val)(
_tmp59E))({void*_tmp5A1=0;Cyc_Tcutil_terr(_tmp589,({const char*_tmp5A2="initializes attribute not allowed on nullable pointers";
_tag_dynforward(_tmp5A2,sizeof(char),_get_zero_arr_size_char(_tmp5A2,55));}),
_tag_dynforward(_tmp5A1,sizeof(void*),0));});if(!Cyc_Tcutil_is_bound_one(_tmp59F))({
void*_tmp5A3=0;Cyc_Tcutil_terr(_tmp589,({const char*_tmp5A4="initializes attribute allowed only on pointers of size 1";
_tag_dynforward(_tmp5A4,sizeof(char),_get_zero_arr_size_char(_tmp5A4,57));}),
_tag_dynforward(_tmp5A3,sizeof(void*),0));});if(((int(*)(int,struct Cyc_Absyn_Conref*
x))Cyc_Absyn_conref_def)(0,_tmp5A0))({void*_tmp5A5=0;Cyc_Tcutil_terr(_tmp589,({
const char*_tmp5A6="initializes attribute allowed only on pointers to non-zero-terminated arrays";
_tag_dynforward(_tmp5A6,sizeof(char),_get_zero_arr_size_char(_tmp5A6,77));}),
_tag_dynforward(_tmp5A5,sizeof(void*),0));});{struct Cyc_CfFlowInfo_InitParam_struct*
_tmp5A7=({struct Cyc_CfFlowInfo_InitParam_struct*_tmp5AF=_region_malloc(_tmp588,
sizeof(*_tmp5AF));_tmp5AF[0]=({struct Cyc_CfFlowInfo_InitParam_struct _tmp5B0;
_tmp5B0.tag=2;_tmp5B0.f1=(int)j;_tmp5B0.f2=(void*)_tmp59C;_tmp5B0;});_tmp5AF;});
struct Cyc_CfFlowInfo_Place*_tmp5A8=({struct Cyc_CfFlowInfo_Place*_tmp5AE=
_region_malloc(_tmp588,sizeof(*_tmp5AE));_tmp5AE->root=(void*)((void*)_tmp5A7);
_tmp5AE->fields=0;_tmp5AE;});_tmp58B=Cyc_Dict_insert(_tmp58B,(void*)_tmp5A7,Cyc_CfFlowInfo_typ_to_absrval(
fenv,_tmp59C,(void*)fenv->esc_none));_tmp58B=Cyc_Dict_insert(_tmp58B,(void*)({
struct Cyc_CfFlowInfo_VarRoot_struct*_tmp5A9=_region_malloc(_tmp588,sizeof(*
_tmp5A9));_tmp5A9[0]=({struct Cyc_CfFlowInfo_VarRoot_struct _tmp5AA;_tmp5AA.tag=0;
_tmp5AA.f1=_tmp599;_tmp5AA;});_tmp5A9;}),(void*)({struct Cyc_CfFlowInfo_AddressOf_struct*
_tmp5AB=_region_malloc(_tmp588,sizeof(*_tmp5AB));_tmp5AB[0]=({struct Cyc_CfFlowInfo_AddressOf_struct
_tmp5AC;_tmp5AC.tag=2;_tmp5AC.f1=_tmp5A8;_tmp5AC;});_tmp5AB;}));param_roots=({
struct Cyc_List_List*_tmp5AD=_region_malloc(_tmp588,sizeof(*_tmp5AD));_tmp5AD->hd=
_tmp5A8;_tmp5AD->tl=param_roots;_tmp5AD;});goto _LL38A;}_LL38D:;_LL38E:({void*
_tmp5B1=0;Cyc_Tcutil_terr(_tmp589,({const char*_tmp5B2="initializes attribute on non-pointer";
_tag_dynforward(_tmp5B2,sizeof(char),_get_zero_arr_size_char(_tmp5B2,37));}),
_tag_dynforward(_tmp5B1,sizeof(void*),0));});_LL38A:;}goto _LL385;}}_LL388:;
_LL389: goto _LL385;_LL385:;}inflow=(union Cyc_CfFlowInfo_FlowInfo_union)({union Cyc_CfFlowInfo_FlowInfo_union
_tmp5B3;(_tmp5B3.ReachableFL).tag=1;(_tmp5B3.ReachableFL).f1=_tmp58B;(_tmp5B3.ReachableFL).f2=
_tmp58C;(_tmp5B3.ReachableFL).f3=_tmp58D;_tmp5B3;});}_LL37B:;}{int noreturn=Cyc_Tcutil_is_noreturn(
Cyc_Tcutil_fndecl2typ(fd));struct Cyc_Hashtable_Table*flow_table=((struct Cyc_Hashtable_Table*(*)(
struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),
int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp588,33,(int(*)(
struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*
s))Cyc_NewControlFlow_hash_ptr);struct Cyc_NewControlFlow_AnalEnv*env=({struct Cyc_NewControlFlow_AnalEnv*
_tmp5BC=_region_malloc(_tmp588,sizeof(*_tmp5BC));_tmp5BC->r=_tmp588;_tmp5BC->fenv=
fenv;_tmp5BC->iterate_again=1;_tmp5BC->iteration_num=0;_tmp5BC->in_try=0;_tmp5BC->tryflow=
inflow;_tmp5BC->all_changed=0;_tmp5BC->noreturn=noreturn;_tmp5BC->param_roots=
param_roots;_tmp5BC->flow_table=flow_table;_tmp5BC;});union Cyc_CfFlowInfo_FlowInfo_union
outflow=inflow;while(env->iterate_again  && !Cyc_Position_error_p()){++ env->iteration_num;
env->iterate_again=0;outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body);
outflow=Cyc_CfFlowInfo_drop_unique_rvals((fd->body)->loc,outflow);}{union Cyc_CfFlowInfo_FlowInfo_union
_tmp5B4=outflow;_LL390: if((_tmp5B4.BottomFL).tag != 0)goto _LL392;_LL391: goto
_LL38F;_LL392:;_LL393: Cyc_NewControlFlow_check_init_params(_tmp589,env,outflow);
if(noreturn)({void*_tmp5B5=0;Cyc_Tcutil_terr(_tmp589,({const char*_tmp5B6="`noreturn' function might (implicitly) return";
_tag_dynforward(_tmp5B6,sizeof(char),_get_zero_arr_size_char(_tmp5B6,46));}),
_tag_dynforward(_tmp5B5,sizeof(void*),0));});else{void*_tmp5B7=Cyc_Tcutil_compress((
void*)fd->ret_type);_LL395: if((int)_tmp5B7 != 0)goto _LL397;_LL396: goto _LL394;
_LL397: if(_tmp5B7 <= (void*)4)goto _LL399;if(*((int*)_tmp5B7)!= 6)goto _LL399;
_LL398: goto _LL39A;_LL399: if((int)_tmp5B7 != 1)goto _LL39B;_LL39A: goto _LL39C;_LL39B:
if(_tmp5B7 <= (void*)4)goto _LL39D;if(*((int*)_tmp5B7)!= 5)goto _LL39D;_LL39C:({
void*_tmp5B8=0;Cyc_Tcutil_warn(_tmp589,({const char*_tmp5B9="function may not return a value";
_tag_dynforward(_tmp5B9,sizeof(char),_get_zero_arr_size_char(_tmp5B9,32));}),
_tag_dynforward(_tmp5B8,sizeof(void*),0));});goto _LL394;_LL39D:;_LL39E:({void*
_tmp5BA=0;Cyc_Tcutil_terr(_tmp589,({const char*_tmp5BB="function may not return a value";
_tag_dynforward(_tmp5BB,sizeof(char),_get_zero_arr_size_char(_tmp5BB,32));}),
_tag_dynforward(_tmp5BA,sizeof(void*),0));});goto _LL394;_LL394:;}goto _LL38F;
_LL38F:;}}}}void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){for(0;ds != 0;
ds=ds->tl){void*_tmp5BD=(void*)((struct Cyc_Absyn_Decl*)ds->hd)->r;struct Cyc_Absyn_Fndecl*
_tmp5BE;struct Cyc_List_List*_tmp5BF;struct Cyc_List_List*_tmp5C0;struct Cyc_List_List*
_tmp5C1;_LL3A0: if(_tmp5BD <= (void*)2)goto _LL3AA;if(*((int*)_tmp5BD)!= 1)goto
_LL3A2;_tmp5BE=((struct Cyc_Absyn_Fn_d_struct*)_tmp5BD)->f1;_LL3A1: Cyc_NewControlFlow_check_fun(
_tmp5BE);goto _LL39F;_LL3A2: if(*((int*)_tmp5BD)!= 10)goto _LL3A4;_tmp5BF=((struct
Cyc_Absyn_ExternC_d_struct*)_tmp5BD)->f1;_LL3A3: _tmp5C0=_tmp5BF;goto _LL3A5;
_LL3A4: if(*((int*)_tmp5BD)!= 9)goto _LL3A6;_tmp5C0=((struct Cyc_Absyn_Using_d_struct*)
_tmp5BD)->f2;_LL3A5: _tmp5C1=_tmp5C0;goto _LL3A7;_LL3A6: if(*((int*)_tmp5BD)!= 8)
goto _LL3A8;_tmp5C1=((struct Cyc_Absyn_Namespace_d_struct*)_tmp5BD)->f2;_LL3A7: Cyc_NewControlFlow_cf_check(
_tmp5C1);goto _LL39F;_LL3A8: if(*((int*)_tmp5BD)!= 11)goto _LL3AA;_LL3A9: goto _LL39F;
_LL3AA:;_LL3AB: goto _LL39F;_LL39F:;}}
