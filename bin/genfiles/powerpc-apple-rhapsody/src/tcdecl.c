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
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  if (!_cus_arr.last_plus_one) _throw_null();
  if (_cus_ans >= _cus_arr.last_plus_one)
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
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  if (!_cus_arr.last_plus_one) _throw_null(); \
  if (_cus_ans >= _cus_arr.last_plus_one) \
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
 struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dynforward_ptr Cyc_aprintf(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dynforward_ptr,
struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dynforward_ptr f1;};
struct _dynforward_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dynforward_ptr,
struct _dynforward_ptr);extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _dynforward_ptr f1;};struct Cyc_Core_NewRegion{
struct _DynRegionHandle*dynregion;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[
21];struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dynforward_ptr f1;};
extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;extern char Cyc_Core_Open_Region[
16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map_c(
void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*
y);extern char Cyc_List_Nth[8];struct Cyc_Lineno_Pos{struct _dynforward_ptr
logical_file;struct _dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _dynforward_ptr
source;struct Cyc_Position_Segment*seg;void*kind;struct _dynforward_ptr desc;};
struct Cyc_Position_Error*Cyc_Position_mk_err_elab(struct Cyc_Position_Segment*,
struct _dynforward_ptr);extern char Cyc_Position_Nocontext[14];void Cyc_Position_post_error(
struct Cyc_Position_Error*);struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple0*,struct _tuple0*);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct
Cyc_Position_Segment*);void*Cyc_Absyn_force_kb(void*kb);struct _dynforward_ptr Cyc_Absyn_attribute2string(
void*);extern char Cyc_Tcdecl_Incompatible[17];struct Cyc_Tcdecl_Xtunionfielddecl{
struct Cyc_Absyn_Tuniondecl*base;struct Cyc_Absyn_Tunionfield*field;};void Cyc_Tcdecl_merr(
struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg1,struct _dynforward_ptr
fmt,struct _dynforward_ptr ap);struct _tuple3{void*f1;int f2;};struct _tuple3 Cyc_Tcdecl_merge_scope(
void*s0,void*s1,struct _dynforward_ptr t,struct _dynforward_ptr v,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr*msg,int allow_externC_extern_merge);struct Cyc_Absyn_Aggrdecl*
Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*
d1,struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg);struct Cyc_Absyn_Tuniondecl*
Cyc_Tcdecl_merge_tuniondecl(struct Cyc_Absyn_Tuniondecl*d0,struct Cyc_Absyn_Tuniondecl*
d1,struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg);struct Cyc_Absyn_Enumdecl*
Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*
d1,struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg);struct Cyc_Absyn_Vardecl*
Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,
struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg);struct Cyc_Absyn_Typedefdecl*
Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*
d1,struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg);void*Cyc_Tcdecl_merge_binding(
void*d0,void*d1,struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg);struct
Cyc_Tcdecl_Xtunionfielddecl*Cyc_Tcdecl_merge_xtunionfielddecl(struct Cyc_Tcdecl_Xtunionfielddecl*
d0,struct Cyc_Tcdecl_Xtunionfielddecl*d1,struct Cyc_Position_Segment*loc,struct
_dynforward_ptr*msg);struct Cyc_List_List*Cyc_Tcdecl_sort_xtunion_fields(struct
Cyc_List_List*f,int*res,struct _dynforward_ptr*v,struct Cyc_Position_Segment*loc,
struct _dynforward_ptr*msg);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{
int(*rel)(void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple4{void*f1;void*f2;};struct _tuple4*
Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple4*Cyc_Dict_rchoose(
struct _RegionHandle*,struct Cyc_Dict_Dict d);int Cyc_strptrcmp(struct
_dynforward_ptr*s1,struct _dynforward_ptr*s2);struct _dynforward_ptr Cyc_strconcat(
struct _dynforward_ptr,struct _dynforward_ptr);struct Cyc_Set_Set;extern char Cyc_Set_Absent[
11];struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
struct _RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,
struct Cyc_Absyn_Tvar*fst_rgn,struct Cyc_Position_Segment*);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*
po,void*eff,void*rgn,struct Cyc_Position_Segment*loc);struct Cyc_RgnOrder_RgnPO*
Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,
struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);int Cyc_RgnOrder_is_region_resetable(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);int Cyc_RgnOrder_effect_outlives(
struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);int Cyc_RgnOrder_satisfies_constraints(
struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,
int do_pin);int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*
eff1,void*eff2);void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);
struct Cyc_Tcenv_CList{void*hd;struct Cyc_Tcenv_CList*tl;};struct Cyc_Tcenv_VarRes_struct{
int tag;void*f1;};struct Cyc_Tcenv_AggrRes_struct{int tag;struct Cyc_Absyn_Aggrdecl*
f1;};struct Cyc_Tcenv_TunionRes_struct{int tag;struct Cyc_Absyn_Tuniondecl*f1;
struct Cyc_Absyn_Tunionfield*f2;};struct Cyc_Tcenv_EnumRes_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{int tag;void*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;
struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict
tuniondecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict
ordinaries;struct Cyc_List_List*availables;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;};void Cyc_Tcutil_explain_failure();
int Cyc_Tcutil_unify(void*,void*);void*Cyc_Tcutil_substitute(struct Cyc_List_List*,
void*);int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual
tq2);int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);struct
Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int
expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};struct _dynforward_ptr
Cyc_Absynpp_typ2string(void*);struct _dynforward_ptr Cyc_Absynpp_kind2string(void*);
struct _dynforward_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct
_dynforward_ptr Cyc_Absynpp_scope2string(void*sc);char Cyc_Tcdecl_Incompatible[17]="\000\000\000\000Incompatible\000";
void Cyc_Tcdecl_merr(struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg1,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap);static void _tmp18E(struct
_dynforward_ptr**msg1,struct _dynforward_ptr*ap,unsigned int*_tmp18D,unsigned int*
_tmp18C,void***_tmp18A){for(*_tmp18D=0;*_tmp18D < *_tmp18C;(*_tmp18D)++){struct
Cyc_String_pa_struct _tmp188;struct Cyc_String_pa_struct*_tmp187;(*_tmp18A)[*
_tmp18D]=*_tmp18D == 0?(void*)((_tmp187=_cycalloc(sizeof(*_tmp187)),((_tmp187[0]=((
_tmp188.tag=0,((_tmp188.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*(*
msg1)),_tmp188)))),_tmp187)))):*((void**)_check_dynforward_subscript(*ap,sizeof(
void*),(int)(*_tmp18D - 1)));}}void Cyc_Tcdecl_merr(struct Cyc_Position_Segment*loc,
struct _dynforward_ptr*msg1,struct _dynforward_ptr fmt,struct _dynforward_ptr ap){if(
msg1 == 0)(int)_throw((void*)Cyc_Tcdecl_Incompatible);{const char*_tmp185;struct
_dynforward_ptr fmt2=(struct _dynforward_ptr)Cyc_strconcat(((_tmp185="%s ",
_tag_dynforward(_tmp185,sizeof(char),_get_zero_arr_size_char(_tmp185,4)))),(
struct _dynforward_ptr)fmt);unsigned int _tmp18D;unsigned int _tmp18C;struct
_dynforward_ptr _tmp18B;void**_tmp18A;unsigned int _tmp189;struct _dynforward_ptr
ap2=(_tmp189=_get_dynforward_size(ap,sizeof(void*))+ 1,((_tmp18A=(void**)
_cycalloc(_check_times(sizeof(void*),_tmp189)),((_tmp18B=_tag_dynforward(_tmp18A,
sizeof(void*),_tmp189),((((_tmp18C=_tmp189,_tmp18E(& msg1,& ap,& _tmp18D,& _tmp18C,&
_tmp18A))),_tmp18B)))))));Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(
struct _dynforward_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt2,ap2)));}}static void
Cyc_Tcdecl_merge_scope_err(void*s0,void*s1,struct _dynforward_ptr t,struct
_dynforward_ptr v,struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg);
static void Cyc_Tcdecl_merge_scope_err(void*s0,void*s1,struct _dynforward_ptr t,
struct _dynforward_ptr v,struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg){
const char*_tmp195;void*_tmp194[4];struct Cyc_String_pa_struct _tmp193;struct Cyc_String_pa_struct
_tmp192;struct Cyc_String_pa_struct _tmp191;struct Cyc_String_pa_struct _tmp190;(
_tmp190.tag=0,((_tmp190.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_scope2string(
s0)),((_tmp191.tag=0,((_tmp191.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
Cyc_Absynpp_scope2string(s1)),((_tmp192.tag=0,((_tmp192.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)v),((_tmp193.tag=0,((_tmp193.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)t),((_tmp194[0]=& _tmp193,((_tmp194[1]=&
_tmp192,((_tmp194[2]=& _tmp191,((_tmp194[3]=& _tmp190,Cyc_Tcdecl_merr(loc,msg,((
_tmp195="%s %s is %s whereas expected scope is %s",_tag_dynforward(_tmp195,
sizeof(char),_get_zero_arr_size_char(_tmp195,41)))),_tag_dynforward(_tmp194,
sizeof(void*),4)))))))))))))))))))))))));}struct _tuple3 Cyc_Tcdecl_merge_scope(
void*s0,void*s1,struct _dynforward_ptr t,struct _dynforward_ptr v,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr*msg,int externCmerge);struct _tuple3 Cyc_Tcdecl_merge_scope(
void*s0,void*s1,struct _dynforward_ptr t,struct _dynforward_ptr v,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr*msg,int externCmerge){{struct _tuple4 _tmp196;struct
_tuple4 _tmpE=(_tmp196.f1=s0,((_tmp196.f2=s1,_tmp196)));void*_tmpF;void*_tmp10;
void*_tmp11;void*_tmp12;void*_tmp13;void*_tmp14;void*_tmp15;void*_tmp16;void*
_tmp17;void*_tmp18;void*_tmp19;void*_tmp1A;void*_tmp1B;void*_tmp1C;void*_tmp1D;
void*_tmp1E;void*_tmp1F;void*_tmp20;_LL1: _tmpF=_tmpE.f1;if((int)_tmpF != 4)goto
_LL3;_tmp10=_tmpE.f2;if((int)_tmp10 != 4)goto _LL3;_LL2: goto _LL0;_LL3: _tmp11=_tmpE.f1;
if((int)_tmp11 != 4)goto _LL5;_tmp12=_tmpE.f2;if((int)_tmp12 != 3)goto _LL5;_LL4:
goto _LL6;_LL5: _tmp13=_tmpE.f1;if((int)_tmp13 != 3)goto _LL7;_tmp14=_tmpE.f2;if((
int)_tmp14 != 4)goto _LL7;_LL6: if(externCmerge)goto _LL0;goto _LL8;_LL7: _tmp15=_tmpE.f1;
if((int)_tmp15 != 4)goto _LL9;_LL8: goto _LLA;_LL9: _tmp16=_tmpE.f2;if((int)_tmp16 != 
4)goto _LLB;_LLA: Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);{struct _tuple3
_tmp197;return(_tmp197.f1=s1,((_tmp197.f2=0,_tmp197)));}_LLB: _tmp17=_tmpE.f2;if((
int)_tmp17 != 3)goto _LLD;_LLC: s1=s0;goto _LL0;_LLD: _tmp18=_tmpE.f1;if((int)_tmp18
!= 3)goto _LLF;_LLE: goto _LL0;_LLF: _tmp19=_tmpE.f1;if((int)_tmp19 != 0)goto _LL11;
_tmp1A=_tmpE.f2;if((int)_tmp1A != 0)goto _LL11;_LL10: goto _LL12;_LL11: _tmp1B=_tmpE.f1;
if((int)_tmp1B != 2)goto _LL13;_tmp1C=_tmpE.f2;if((int)_tmp1C != 2)goto _LL13;_LL12:
goto _LL14;_LL13: _tmp1D=_tmpE.f1;if((int)_tmp1D != 1)goto _LL15;_tmp1E=_tmpE.f2;if((
int)_tmp1E != 1)goto _LL15;_LL14: goto _LL0;_LL15: _tmp1F=_tmpE.f1;if((int)_tmp1F != 5)
goto _LL17;_tmp20=_tmpE.f2;if((int)_tmp20 != 5)goto _LL17;_LL16: goto _LL0;_LL17:;
_LL18: Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);{struct _tuple3 _tmp198;return(
_tmp198.f1=s1,((_tmp198.f2=0,_tmp198)));}_LL0:;}{struct _tuple3 _tmp199;return(
_tmp199.f1=s1,((_tmp199.f2=1,_tmp199)));}}static int Cyc_Tcdecl_check_type(void*t0,
void*t1);static int Cyc_Tcdecl_check_type(void*t0,void*t1){return Cyc_Tcutil_unify(
t0,t1);}static unsigned int Cyc_Tcdecl_get_uint_const_value(struct Cyc_Absyn_Exp*e);
static unsigned int Cyc_Tcdecl_get_uint_const_value(struct Cyc_Absyn_Exp*e){void*
_tmp24=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp25;int _tmp26;_LL1A: if(*((int*)
_tmp24)!= 0)goto _LL1C;_tmp25=((struct Cyc_Absyn_Const_e_struct*)_tmp24)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmp24)->f1).Int_c).tag != 2)goto _LL1C;_tmp26=(
_tmp25.Int_c).f2;_LL1B: return(unsigned int)_tmp26;_LL1C:;_LL1D: {struct Cyc_Core_Invalid_argument_struct
_tmp19F;const char*_tmp19E;struct Cyc_Core_Invalid_argument_struct*_tmp19D;(int)
_throw((void*)((_tmp19D=_cycalloc(sizeof(*_tmp19D)),((_tmp19D[0]=((_tmp19F.tag=
Cyc_Core_Invalid_argument,((_tmp19F.f1=((_tmp19E="Tcdecl::get_uint_const_value",
_tag_dynforward(_tmp19E,sizeof(char),_get_zero_arr_size_char(_tmp19E,29)))),
_tmp19F)))),_tmp19D)))));}_LL19:;}static int Cyc_Tcdecl_check_tvs(struct Cyc_List_List*
tvs0,struct Cyc_List_List*tvs1,struct _dynforward_ptr t,struct _dynforward_ptr v,
struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg);inline static int Cyc_Tcdecl_check_tvs(
struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,struct _dynforward_ptr t,struct
_dynforward_ptr v,struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg){if(((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs0)!= ((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(tvs1)){{const char*_tmp1A4;void*_tmp1A3[2];struct Cyc_String_pa_struct
_tmp1A2;struct Cyc_String_pa_struct _tmp1A1;(_tmp1A1.tag=0,((_tmp1A1.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)v),((_tmp1A2.tag=0,((_tmp1A2.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)t),((_tmp1A3[0]=& _tmp1A2,((_tmp1A3[1]=&
_tmp1A1,Cyc_Tcdecl_merr(loc,msg,((_tmp1A4="%s %s has a different number of type parameters",
_tag_dynforward(_tmp1A4,sizeof(char),_get_zero_arr_size_char(_tmp1A4,48)))),
_tag_dynforward(_tmp1A3,sizeof(void*),2)))))))))))));}return 0;}{struct Cyc_List_List*
_tmp2E=tvs0;struct Cyc_List_List*_tmp2F=tvs1;for(0;_tmp2E != 0;(_tmp2E=_tmp2E->tl,
_tmp2F=_tmp2F->tl)){void*_tmp30=Cyc_Absyn_force_kb((void*)((struct Cyc_Absyn_Tvar*)
_tmp2E->hd)->kind);void*_tmp31=Cyc_Absyn_force_kb((void*)((struct Cyc_Absyn_Tvar*)((
struct Cyc_List_List*)_check_null(_tmp2F))->hd)->kind);if(_tmp30 == _tmp31)
continue;{const char*_tmp1AC;void*_tmp1AB[5];struct Cyc_String_pa_struct _tmp1AA;
struct Cyc_String_pa_struct _tmp1A9;struct Cyc_String_pa_struct _tmp1A8;struct Cyc_String_pa_struct
_tmp1A7;struct Cyc_String_pa_struct _tmp1A6;(_tmp1A6.tag=0,((_tmp1A6.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(_tmp31)),((
_tmp1A7.tag=0,((_tmp1A7.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct Cyc_Absyn_Tvar*)_tmp2E->hd)->name),((_tmp1A8.tag=0,((_tmp1A8.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_kind2string(_tmp30)),((
_tmp1A9.tag=0,((_tmp1A9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)v),((
_tmp1AA.tag=0,((_tmp1AA.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)t),((
_tmp1AB[0]=& _tmp1AA,((_tmp1AB[1]=& _tmp1A9,((_tmp1AB[2]=& _tmp1A8,((_tmp1AB[3]=&
_tmp1A7,((_tmp1AB[4]=& _tmp1A6,Cyc_Tcdecl_merr(loc,msg,((_tmp1AC="%s %s has a different kind (%s) for type parameter %s (%s)",
_tag_dynforward(_tmp1AC,sizeof(char),_get_zero_arr_size_char(_tmp1AC,59)))),
_tag_dynforward(_tmp1AB,sizeof(void*),5)))))))))))))))))))))))))))))));}return 0;}
return 1;}}static int Cyc_Tcdecl_check_atts(struct Cyc_List_List*atts0,struct Cyc_List_List*
atts1,struct _dynforward_ptr t,struct _dynforward_ptr v,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr*msg);static int Cyc_Tcdecl_check_atts(struct Cyc_List_List*
atts0,struct Cyc_List_List*atts1,struct _dynforward_ptr t,struct _dynforward_ptr v,
struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg){if(!Cyc_Tcutil_same_atts(
atts0,atts1)){{const char*_tmp1B1;void*_tmp1B0[2];struct Cyc_String_pa_struct
_tmp1AF;struct Cyc_String_pa_struct _tmp1AE;(_tmp1AE.tag=0,((_tmp1AE.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)v),((_tmp1AF.tag=0,((_tmp1AF.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)t),((_tmp1B0[0]=& _tmp1AF,((_tmp1B0[1]=&
_tmp1AE,Cyc_Tcdecl_merr(loc,msg,((_tmp1B1="%s %s has different attributes",
_tag_dynforward(_tmp1B1,sizeof(char),_get_zero_arr_size_char(_tmp1B1,31)))),
_tag_dynforward(_tmp1B0,sizeof(void*),2)))))))))))));}return 0;}return 1;}struct
_tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};static struct Cyc_List_List*Cyc_Tcdecl_build_tvs_map(
struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1);static struct Cyc_List_List*Cyc_Tcdecl_build_tvs_map(
struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1){struct Cyc_List_List*inst=0;
for(0;tvs0 != 0;(tvs0=tvs0->tl,tvs1=tvs1->tl)){struct _tuple5*_tmp1BB;struct Cyc_Absyn_VarType_struct
_tmp1BA;struct Cyc_Absyn_VarType_struct*_tmp1B9;struct Cyc_List_List*_tmp1B8;inst=((
_tmp1B8=_cycalloc(sizeof(*_tmp1B8)),((_tmp1B8->hd=((_tmp1BB=_cycalloc(sizeof(*
_tmp1BB)),((_tmp1BB->f1=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)
_check_null(tvs1))->hd,((_tmp1BB->f2=(void*)((_tmp1B9=_cycalloc(sizeof(*_tmp1B9)),((
_tmp1B9[0]=((_tmp1BA.tag=1,((_tmp1BA.f1=(struct Cyc_Absyn_Tvar*)tvs0->hd,_tmp1BA)))),
_tmp1B9)))),_tmp1BB)))))),((_tmp1B8->tl=inst,_tmp1B8))))));}return inst;}struct
_tuple6{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_AggrdeclImpl*f2;};struct
Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct
Cyc_Absyn_Aggrdecl*d1,struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg);
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,
struct Cyc_Absyn_Aggrdecl*d1,struct Cyc_Position_Segment*loc,struct _dynforward_ptr*
msg){struct _dynforward_ptr _tmp41=Cyc_Absynpp_qvar2string(d0->name);int _tmp42=1;
if(!((void*)d0->kind == (void*)d1->kind))return 0;{const char*_tmp1BC;if(!Cyc_Tcdecl_check_tvs(
d0->tvs,d1->tvs,((_tmp1BC="type",_tag_dynforward(_tmp1BC,sizeof(char),
_get_zero_arr_size_char(_tmp1BC,5)))),_tmp41,loc,msg))return 0;}{void*_tmp46;int
_tmp47;const char*_tmp1BD;struct _tuple3 _tmp45=Cyc_Tcdecl_merge_scope((void*)d0->sc,(
void*)d1->sc,((_tmp1BD="type",_tag_dynforward(_tmp1BD,sizeof(char),
_get_zero_arr_size_char(_tmp1BD,5)))),_tmp41,loc,msg,1);_tmp46=_tmp45.f1;_tmp47=
_tmp45.f2;if(!_tmp47)_tmp42=0;{const char*_tmp1BE;if(!Cyc_Tcdecl_check_atts(d0->attributes,
d1->attributes,((_tmp1BE="type",_tag_dynforward(_tmp1BE,sizeof(char),
_get_zero_arr_size_char(_tmp1BE,5)))),_tmp41,loc,msg))_tmp42=0;}{struct Cyc_Absyn_Aggrdecl*
d2;{struct _tuple6 _tmp1BF;struct _tuple6 _tmp4A=(_tmp1BF.f1=d0->impl,((_tmp1BF.f2=
d1->impl,_tmp1BF)));struct Cyc_Absyn_AggrdeclImpl*_tmp4B;struct Cyc_Absyn_AggrdeclImpl*
_tmp4C;struct Cyc_Absyn_AggrdeclImpl*_tmp4D;struct Cyc_Absyn_AggrdeclImpl _tmp4E;
struct Cyc_List_List*_tmp4F;struct Cyc_List_List*_tmp50;struct Cyc_List_List*_tmp51;
struct Cyc_Absyn_AggrdeclImpl*_tmp52;struct Cyc_Absyn_AggrdeclImpl _tmp53;struct Cyc_List_List*
_tmp54;struct Cyc_List_List*_tmp55;struct Cyc_List_List*_tmp56;_LL1F: _tmp4B=_tmp4A.f2;
if(_tmp4B != 0)goto _LL21;_LL20: d2=d0;goto _LL1E;_LL21: _tmp4C=_tmp4A.f1;if(_tmp4C != 
0)goto _LL23;_LL22: d2=d1;goto _LL1E;_LL23: _tmp4D=_tmp4A.f1;if(_tmp4D == 0)goto _LL1E;
_tmp4E=*_tmp4D;_tmp4F=_tmp4E.exist_vars;_tmp50=_tmp4E.rgn_po;_tmp51=_tmp4E.fields;
_tmp52=_tmp4A.f2;if(_tmp52 == 0)goto _LL1E;_tmp53=*_tmp52;_tmp54=_tmp53.exist_vars;
_tmp55=_tmp53.rgn_po;_tmp56=_tmp53.fields;_LL24:{const char*_tmp1C0;if(!Cyc_Tcdecl_check_tvs(
_tmp4F,_tmp54,((_tmp1C0="type",_tag_dynforward(_tmp1C0,sizeof(char),
_get_zero_arr_size_char(_tmp1C0,5)))),_tmp41,loc,msg))return 0;}{struct Cyc_List_List*
_tmp58=Cyc_Tcdecl_build_tvs_map(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_append)(d0->tvs,_tmp4F),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(d1->tvs,_tmp54));
for(0;_tmp50 != 0  && _tmp55 != 0;(_tmp50=_tmp50->tl,_tmp55=_tmp55->tl)){Cyc_Tcdecl_check_type((*((
struct _tuple4*)_tmp50->hd)).f1,(*((struct _tuple4*)_tmp55->hd)).f1);Cyc_Tcdecl_check_type((*((
struct _tuple4*)_tmp50->hd)).f2,(*((struct _tuple4*)_tmp55->hd)).f2);}for(0;_tmp51
!= 0  && _tmp56 != 0;(_tmp51=_tmp51->tl,_tmp56=_tmp56->tl)){struct Cyc_Absyn_Aggrfield
_tmp5A;struct _dynforward_ptr*_tmp5B;struct Cyc_Absyn_Tqual _tmp5C;void*_tmp5D;
struct Cyc_Absyn_Exp*_tmp5E;struct Cyc_List_List*_tmp5F;struct Cyc_Absyn_Aggrfield*
_tmp59=(struct Cyc_Absyn_Aggrfield*)_tmp51->hd;_tmp5A=*_tmp59;_tmp5B=_tmp5A.name;
_tmp5C=_tmp5A.tq;_tmp5D=(void*)_tmp5A.type;_tmp5E=_tmp5A.width;_tmp5F=_tmp5A.attributes;{
struct Cyc_Absyn_Aggrfield _tmp61;struct _dynforward_ptr*_tmp62;struct Cyc_Absyn_Tqual
_tmp63;void*_tmp64;struct Cyc_Absyn_Exp*_tmp65;struct Cyc_List_List*_tmp66;struct
Cyc_Absyn_Aggrfield*_tmp60=(struct Cyc_Absyn_Aggrfield*)_tmp56->hd;_tmp61=*_tmp60;
_tmp62=_tmp61.name;_tmp63=_tmp61.tq;_tmp64=(void*)_tmp61.type;_tmp65=_tmp61.width;
_tmp66=_tmp61.attributes;if(Cyc_strptrcmp(_tmp5B,_tmp62)!= 0){{const char*_tmp1C9;
void*_tmp1C8[4];const char*_tmp1C7;struct Cyc_String_pa_struct _tmp1C6;struct Cyc_String_pa_struct
_tmp1C5;struct Cyc_String_pa_struct _tmp1C4;struct Cyc_String_pa_struct _tmp1C3;(
_tmp1C3.tag=0,((_tmp1C3.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
_tmp62),((_tmp1C4.tag=0,((_tmp1C4.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)*_tmp5B),((_tmp1C5.tag=0,((_tmp1C5.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)_tmp41),((_tmp1C6.tag=0,((_tmp1C6.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)((_tmp1C7="type",_tag_dynforward(_tmp1C7,
sizeof(char),_get_zero_arr_size_char(_tmp1C7,5))))),((_tmp1C8[0]=& _tmp1C6,((
_tmp1C8[1]=& _tmp1C5,((_tmp1C8[2]=& _tmp1C4,((_tmp1C8[3]=& _tmp1C3,Cyc_Tcdecl_merr(
loc,msg,((_tmp1C9="%s %s : field name mismatch %s != %s",_tag_dynforward(_tmp1C9,
sizeof(char),_get_zero_arr_size_char(_tmp1C9,37)))),_tag_dynforward(_tmp1C8,
sizeof(void*),4)))))))))))))))))))))))));}return 0;}if(!Cyc_Tcutil_same_atts(
_tmp5F,_tmp66)){{const char*_tmp1D1;void*_tmp1D0[3];const char*_tmp1CF;struct Cyc_String_pa_struct
_tmp1CE;struct Cyc_String_pa_struct _tmp1CD;struct Cyc_String_pa_struct _tmp1CC;(
_tmp1CC.tag=0,((_tmp1CC.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
_tmp5B),((_tmp1CD.tag=0,((_tmp1CD.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)_tmp41),((_tmp1CE.tag=0,((_tmp1CE.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)((_tmp1CF="type",_tag_dynforward(_tmp1CF,sizeof(char),
_get_zero_arr_size_char(_tmp1CF,5))))),((_tmp1D0[0]=& _tmp1CE,((_tmp1D0[1]=&
_tmp1CD,((_tmp1D0[2]=& _tmp1CC,Cyc_Tcdecl_merr(loc,msg,((_tmp1D1="%s %s : attribute mismatch on field %s",
_tag_dynforward(_tmp1D1,sizeof(char),_get_zero_arr_size_char(_tmp1D1,39)))),
_tag_dynforward(_tmp1D0,sizeof(void*),3)))))))))))))))))));}_tmp42=0;}if(!Cyc_Tcutil_equal_tqual(
_tmp5C,_tmp63)){{const char*_tmp1D9;void*_tmp1D8[3];const char*_tmp1D7;struct Cyc_String_pa_struct
_tmp1D6;struct Cyc_String_pa_struct _tmp1D5;struct Cyc_String_pa_struct _tmp1D4;(
_tmp1D4.tag=0,((_tmp1D4.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*
_tmp5B),((_tmp1D5.tag=0,((_tmp1D5.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)_tmp41),((_tmp1D6.tag=0,((_tmp1D6.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)((_tmp1D7="type",_tag_dynforward(_tmp1D7,sizeof(char),
_get_zero_arr_size_char(_tmp1D7,5))))),((_tmp1D8[0]=& _tmp1D6,((_tmp1D8[1]=&
_tmp1D5,((_tmp1D8[2]=& _tmp1D4,Cyc_Tcdecl_merr(loc,msg,((_tmp1D9="%s %s : qualifier mismatch on field %s",
_tag_dynforward(_tmp1D9,sizeof(char),_get_zero_arr_size_char(_tmp1D9,39)))),
_tag_dynforward(_tmp1D8,sizeof(void*),3)))))))))))))))))));}_tmp42=0;}if(((
_tmp5E != 0  && _tmp65 != 0) && Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)
_tmp5E)!= Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_tmp65) || 
_tmp5E == 0  && _tmp65 != 0) || _tmp5E != 0  && _tmp65 == 0){{const char*_tmp1E1;void*
_tmp1E0[3];const char*_tmp1DF;struct Cyc_String_pa_struct _tmp1DE;struct Cyc_String_pa_struct
_tmp1DD;struct Cyc_String_pa_struct _tmp1DC;(_tmp1DC.tag=0,((_tmp1DC.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp5B),((_tmp1DD.tag=0,((_tmp1DD.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmp41),((_tmp1DE.tag=0,((_tmp1DE.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)((_tmp1DF="type",_tag_dynforward(
_tmp1DF,sizeof(char),_get_zero_arr_size_char(_tmp1DF,5))))),((_tmp1E0[0]=&
_tmp1DE,((_tmp1E0[1]=& _tmp1DD,((_tmp1E0[2]=& _tmp1DC,Cyc_Tcdecl_merr(loc,msg,((
_tmp1E1="%s %s : bitfield mismatch on field %s",_tag_dynforward(_tmp1E1,sizeof(
char),_get_zero_arr_size_char(_tmp1E1,38)))),_tag_dynforward(_tmp1E0,sizeof(void*),
3)))))))))))))))))));}_tmp42=0;}{void*subst_t1=Cyc_Tcutil_substitute(_tmp58,
_tmp64);if(!Cyc_Tcdecl_check_type(_tmp5D,subst_t1)){{const char*_tmp1E8;void*
_tmp1E7[4];struct Cyc_String_pa_struct _tmp1E6;struct Cyc_String_pa_struct _tmp1E5;
struct Cyc_String_pa_struct _tmp1E4;struct Cyc_String_pa_struct _tmp1E3;(_tmp1E3.tag=
0,((_tmp1E3.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
subst_t1)),((_tmp1E4.tag=0,((_tmp1E4.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_typ2string(_tmp5D)),((_tmp1E5.tag=0,((_tmp1E5.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp5B),((_tmp1E6.tag=0,((_tmp1E6.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmp41),((_tmp1E7[0]=& _tmp1E6,((
_tmp1E7[1]=& _tmp1E5,((_tmp1E7[2]=& _tmp1E4,((_tmp1E7[3]=& _tmp1E3,Cyc_Tcdecl_merr(
loc,msg,((_tmp1E8="type %s : type mismatch on field %s: %s != %s",
_tag_dynforward(_tmp1E8,sizeof(char),_get_zero_arr_size_char(_tmp1E8,46)))),
_tag_dynforward(_tmp1E7,sizeof(void*),4)))))))))))))))))))))))));}Cyc_Tcutil_explain_failure();
_tmp42=0;}}}}if(_tmp51 != 0){{const char*_tmp1ED;void*_tmp1EC[2];struct Cyc_String_pa_struct
_tmp1EB;struct Cyc_String_pa_struct _tmp1EA;(_tmp1EA.tag=0,((_tmp1EA.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp51->hd)->name),((
_tmp1EB.tag=0,((_tmp1EB.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmp41),((
_tmp1EC[0]=& _tmp1EB,((_tmp1EC[1]=& _tmp1EA,Cyc_Tcdecl_merr(loc,msg,((_tmp1ED="type %s is missing field %s",
_tag_dynforward(_tmp1ED,sizeof(char),_get_zero_arr_size_char(_tmp1ED,28)))),
_tag_dynforward(_tmp1EC,sizeof(void*),2)))))))))))));}_tmp42=0;}if(_tmp56 != 0){{
const char*_tmp1F2;void*_tmp1F1[2];struct Cyc_String_pa_struct _tmp1F0;struct Cyc_String_pa_struct
_tmp1EF;(_tmp1EF.tag=0,((_tmp1EF.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp56->hd)->name),((_tmp1F0.tag=0,((
_tmp1F0.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmp41),((_tmp1F1[0]=&
_tmp1F0,((_tmp1F1[1]=& _tmp1EF,Cyc_Tcdecl_merr(loc,msg,((_tmp1F2="type %s has extra field %s",
_tag_dynforward(_tmp1F2,sizeof(char),_get_zero_arr_size_char(_tmp1F2,27)))),
_tag_dynforward(_tmp1F1,sizeof(void*),2)))))))))))));}_tmp42=0;}d2=d0;goto _LL1E;}
_LL1E:;}if(!_tmp42)return 0;if(_tmp46 == (void*)d2->sc)return(struct Cyc_Absyn_Aggrdecl*)
d2;else{{struct Cyc_Absyn_Aggrdecl*_tmp1F3;d2=((_tmp1F3=_cycalloc(sizeof(*_tmp1F3)),((
_tmp1F3[0]=*d2,_tmp1F3))));}(void*)(d2->sc=(void*)_tmp46);return(struct Cyc_Absyn_Aggrdecl*)
d2;}}}}static struct _dynforward_ptr Cyc_Tcdecl_is_x2string(int is_x);inline static
struct _dynforward_ptr Cyc_Tcdecl_is_x2string(int is_x){const char*_tmp1F5;const char*
_tmp1F4;return is_x?(_tmp1F5="xtunion",_tag_dynforward(_tmp1F5,sizeof(char),
_get_zero_arr_size_char(_tmp1F5,8))):((_tmp1F4="tunion",_tag_dynforward(_tmp1F4,
sizeof(char),_get_zero_arr_size_char(_tmp1F4,7))));}struct _tuple7{struct Cyc_Absyn_Tqual
f1;void*f2;};static struct Cyc_Absyn_Tunionfield*Cyc_Tcdecl_merge_tunionfield(
struct Cyc_Absyn_Tunionfield*f0,struct Cyc_Absyn_Tunionfield*f1,struct Cyc_List_List*
inst,struct _dynforward_ptr t,struct _dynforward_ptr v,struct _dynforward_ptr*msg);
static struct Cyc_Absyn_Tunionfield*Cyc_Tcdecl_merge_tunionfield(struct Cyc_Absyn_Tunionfield*
f0,struct Cyc_Absyn_Tunionfield*f1,struct Cyc_List_List*inst,struct _dynforward_ptr
t,struct _dynforward_ptr v,struct _dynforward_ptr*msg){struct Cyc_Position_Segment*
loc=f1->loc;if(Cyc_strptrcmp((*f0->name).f2,(*f1->name).f2)!= 0){{const char*
_tmp1FC;void*_tmp1FB[4];struct Cyc_String_pa_struct _tmp1FA;struct Cyc_String_pa_struct
_tmp1F9;struct Cyc_String_pa_struct _tmp1F8;struct Cyc_String_pa_struct _tmp1F7;(
_tmp1F7.tag=0,((_tmp1F7.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*(*f0->name).f2),((
_tmp1F8.tag=0,((_tmp1F8.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*(*f1->name).f2),((
_tmp1F9.tag=0,((_tmp1F9.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)v),((
_tmp1FA.tag=0,((_tmp1FA.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)t),((
_tmp1FB[0]=& _tmp1FA,((_tmp1FB[1]=& _tmp1F9,((_tmp1FB[2]=& _tmp1F8,((_tmp1FB[3]=&
_tmp1F7,Cyc_Tcdecl_merr(loc,msg,((_tmp1FC="%s %s: field name mismatch %s != %s",
_tag_dynforward(_tmp1FC,sizeof(char),_get_zero_arr_size_char(_tmp1FC,36)))),
_tag_dynforward(_tmp1FB,sizeof(void*),4)))))))))))))))))))))))));}return 0;}{
struct _dynforward_ptr _tmp97=*(*f0->name).f2;void*_tmp9D;int _tmp9E;const char*
_tmp201;void*_tmp200[2];struct Cyc_String_pa_struct _tmp1FF;struct Cyc_String_pa_struct
_tmp1FE;struct _tuple3 _tmp9C=Cyc_Tcdecl_merge_scope((void*)f0->sc,(void*)f1->sc,(
struct _dynforward_ptr)((_tmp1FE.tag=0,((_tmp1FE.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)v),((_tmp1FF.tag=0,((_tmp1FF.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)t),((_tmp200[0]=& _tmp1FF,((_tmp200[1]=& _tmp1FE,Cyc_aprintf(((
_tmp201="in %s %s, field",_tag_dynforward(_tmp201,sizeof(char),
_get_zero_arr_size_char(_tmp201,16)))),_tag_dynforward(_tmp200,sizeof(void*),2)))))))))))))),
_tmp97,loc,msg,0);_tmp9D=_tmp9C.f1;_tmp9E=_tmp9C.f2;{struct Cyc_List_List*_tmp9F=
f0->typs;struct Cyc_List_List*_tmpA0=f1->typs;if(((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(_tmp9F)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpA0)){{const char*_tmp207;void*_tmp206[3];struct Cyc_String_pa_struct _tmp205;
struct Cyc_String_pa_struct _tmp204;struct Cyc_String_pa_struct _tmp203;(_tmp203.tag=
0,((_tmp203.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmp97),((_tmp204.tag=
0,((_tmp204.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)v),((_tmp205.tag=0,((
_tmp205.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)t),((_tmp206[0]=&
_tmp205,((_tmp206[1]=& _tmp204,((_tmp206[2]=& _tmp203,Cyc_Tcdecl_merr(loc,msg,((
_tmp207="%s %s, field %s: parameter number mismatch",_tag_dynforward(_tmp207,
sizeof(char),_get_zero_arr_size_char(_tmp207,43)))),_tag_dynforward(_tmp206,
sizeof(void*),3)))))))))))))))))));}_tmp9E=0;}for(0;_tmp9F != 0;(_tmp9F=_tmp9F->tl,
_tmpA0=_tmpA0->tl)){if(!Cyc_Tcutil_equal_tqual((*((struct _tuple7*)_tmp9F->hd)).f1,(*((
struct _tuple7*)((struct Cyc_List_List*)_check_null(_tmpA0))->hd)).f1)){{const char*
_tmp20D;void*_tmp20C[3];struct Cyc_String_pa_struct _tmp20B;struct Cyc_String_pa_struct
_tmp20A;struct Cyc_String_pa_struct _tmp209;(_tmp209.tag=0,((_tmp209.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp97),((_tmp20A.tag=0,((_tmp20A.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)v),((_tmp20B.tag=0,((_tmp20B.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)t),((_tmp20C[0]=& _tmp20B,((_tmp20C[
1]=& _tmp20A,((_tmp20C[2]=& _tmp209,Cyc_Tcdecl_merr(loc,msg,((_tmp20D="%s %s, field %s: parameter qualifier",
_tag_dynforward(_tmp20D,sizeof(char),_get_zero_arr_size_char(_tmp20D,37)))),
_tag_dynforward(_tmp20C,sizeof(void*),3)))))))))))))))))));}_tmp9E=0;}{void*
subst_t1=Cyc_Tcutil_substitute(inst,(*((struct _tuple7*)_tmpA0->hd)).f2);if(!Cyc_Tcdecl_check_type((*((
struct _tuple7*)_tmp9F->hd)).f2,subst_t1)){{const char*_tmp215;void*_tmp214[5];
struct Cyc_String_pa_struct _tmp213;struct Cyc_String_pa_struct _tmp212;struct Cyc_String_pa_struct
_tmp211;struct Cyc_String_pa_struct _tmp210;struct Cyc_String_pa_struct _tmp20F;(
_tmp20F.tag=0,((_tmp20F.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
subst_t1)),((_tmp210.tag=0,((_tmp210.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_typ2string((*((struct _tuple7*)_tmp9F->hd)).f2)),((
_tmp211.tag=0,((_tmp211.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmp97),((
_tmp212.tag=0,((_tmp212.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)v),((
_tmp213.tag=0,((_tmp213.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)t),((
_tmp214[0]=& _tmp213,((_tmp214[1]=& _tmp212,((_tmp214[2]=& _tmp211,((_tmp214[3]=&
_tmp210,((_tmp214[4]=& _tmp20F,Cyc_Tcdecl_merr(loc,msg,((_tmp215="%s %s, field %s: parameter type mismatch %s != %s",
_tag_dynforward(_tmp215,sizeof(char),_get_zero_arr_size_char(_tmp215,50)))),
_tag_dynforward(_tmp214,sizeof(void*),5)))))))))))))))))))))))))))))));}Cyc_Tcutil_explain_failure();
_tmp9E=0;}}}if(!_tmp9E)return 0;if((void*)f0->sc != _tmp9D){struct Cyc_Absyn_Tunionfield*
_tmp216;struct Cyc_Absyn_Tunionfield*_tmpB2=(_tmp216=_cycalloc(sizeof(*_tmp216)),((
_tmp216[0]=*f0,_tmp216)));(void*)(((struct Cyc_Absyn_Tunionfield*)_check_null(
_tmpB2))->sc=(void*)_tmp9D);return _tmpB2;}else{return(struct Cyc_Absyn_Tunionfield*)
f0;}}}}static struct _tuple7*Cyc_Tcdecl_substitute_tunionfield_f2(struct Cyc_List_List*
inst,struct _tuple7*x);static struct _tuple7*Cyc_Tcdecl_substitute_tunionfield_f2(
struct Cyc_List_List*inst,struct _tuple7*x){struct _tuple7 _tmpB5;struct Cyc_Absyn_Tqual
_tmpB6;void*_tmpB7;struct _tuple7*_tmpB4=x;_tmpB5=*_tmpB4;_tmpB6=_tmpB5.f1;_tmpB7=
_tmpB5.f2;{struct _tuple7*_tmp217;return(_tmp217=_cycalloc(sizeof(*_tmp217)),((
_tmp217->f1=_tmpB6,((_tmp217->f2=Cyc_Tcutil_substitute(inst,_tmpB7),_tmp217)))));}}
static struct Cyc_Absyn_Tunionfield*Cyc_Tcdecl_substitute_tunionfield(struct Cyc_List_List*
inst1,struct Cyc_Absyn_Tunionfield*f1);static struct Cyc_Absyn_Tunionfield*Cyc_Tcdecl_substitute_tunionfield(
struct Cyc_List_List*inst1,struct Cyc_Absyn_Tunionfield*f1){struct Cyc_Absyn_Tunionfield*
_tmp218;struct Cyc_Absyn_Tunionfield*_tmpB9=(_tmp218=_cycalloc(sizeof(*_tmp218)),((
_tmp218[0]=*f1,_tmp218)));_tmpB9->typs=((struct Cyc_List_List*(*)(struct _tuple7*(*
f)(struct Cyc_List_List*,struct _tuple7*),struct Cyc_List_List*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Tcdecl_substitute_tunionfield_f2,inst1,f1->typs);return
_tmpB9;}static struct Cyc_List_List*Cyc_Tcdecl_merge_xtunion_fields(struct Cyc_List_List*
f0s,struct Cyc_List_List*f1s,struct Cyc_List_List*inst,struct Cyc_List_List*tvs0,
struct Cyc_List_List*tvs1,int*res,int*incl,struct _dynforward_ptr t,struct
_dynforward_ptr v,struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg);
static struct Cyc_List_List*Cyc_Tcdecl_merge_xtunion_fields(struct Cyc_List_List*
f0s,struct Cyc_List_List*f1s,struct Cyc_List_List*inst,struct Cyc_List_List*tvs0,
struct Cyc_List_List*tvs1,int*res,int*incl,struct _dynforward_ptr t,struct
_dynforward_ptr v,struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg){
struct Cyc_List_List**_tmp219;struct Cyc_List_List**f2sp=(_tmp219=_cycalloc(
sizeof(*_tmp219)),((_tmp219[0]=0,_tmp219)));struct Cyc_List_List**_tmpBB=f2sp;int
cmp=- 1;for(0;f0s != 0  && f1s != 0;f1s=f1s->tl){while(f0s != 0  && (cmp=Cyc_Absyn_qvar_cmp(((
struct Cyc_Absyn_Tunionfield*)f0s->hd)->name,((struct Cyc_Absyn_Tunionfield*)f1s->hd)->name))
< 0){struct Cyc_List_List*_tmp21A;struct Cyc_List_List*_tmpBC=(_tmp21A=_cycalloc(
sizeof(*_tmp21A)),((_tmp21A->hd=(struct Cyc_Absyn_Tunionfield*)f0s->hd,((_tmp21A->tl=
0,_tmp21A)))));*_tmpBB=_tmpBC;_tmpBB=&((struct Cyc_List_List*)_check_null(_tmpBC))->tl;
f0s=f0s->tl;}if(f0s == 0  || cmp > 0){*incl=0;{struct Cyc_List_List*_tmp21B;struct
Cyc_List_List*_tmpBE=(_tmp21B=_cycalloc(sizeof(*_tmp21B)),((_tmp21B->hd=Cyc_Tcdecl_substitute_tunionfield(
inst,(struct Cyc_Absyn_Tunionfield*)f1s->hd),((_tmp21B->tl=0,_tmp21B)))));*_tmpBB=
_tmpBE;_tmpBB=&((struct Cyc_List_List*)_check_null(_tmpBE))->tl;}}else{struct Cyc_Absyn_Tunionfield*
_tmpC0=Cyc_Tcdecl_merge_tunionfield((struct Cyc_Absyn_Tunionfield*)f0s->hd,(
struct Cyc_Absyn_Tunionfield*)f1s->hd,inst,t,v,msg);if(_tmpC0 != 0){if(_tmpC0 != (
struct Cyc_Absyn_Tunionfield*)((struct Cyc_Absyn_Tunionfield*)f0s->hd))*incl=0;{
struct Cyc_List_List*_tmp21C;struct Cyc_List_List*_tmpC1=(_tmp21C=_cycalloc(
sizeof(*_tmp21C)),((_tmp21C->hd=(struct Cyc_Absyn_Tunionfield*)_tmpC0,((_tmp21C->tl=
0,_tmp21C)))));*_tmpBB=_tmpC1;_tmpBB=&((struct Cyc_List_List*)_check_null(_tmpC1))->tl;}}
else{*res=0;}f0s=f0s->tl;}}if(f1s != 0){*incl=0;*_tmpBB=f1s;}else{*_tmpBB=f0s;}
return*f2sp;}struct _tuple8{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
static struct _tuple8 Cyc_Tcdecl_split(struct Cyc_List_List*f);static struct _tuple8
Cyc_Tcdecl_split(struct Cyc_List_List*f){if(f == 0){struct _tuple8 _tmp21D;return(
_tmp21D.f1=0,((_tmp21D.f2=0,_tmp21D)));}if(f->tl == 0){struct _tuple8 _tmp21E;
return(_tmp21E.f1=f,((_tmp21E.f2=0,_tmp21E)));}{struct Cyc_List_List*_tmpC7;
struct Cyc_List_List*_tmpC8;struct _tuple8 _tmpC6=Cyc_Tcdecl_split(((struct Cyc_List_List*)
_check_null(f->tl))->tl);_tmpC7=_tmpC6.f1;_tmpC8=_tmpC6.f2;{struct Cyc_List_List*
_tmp223;struct Cyc_List_List*_tmp222;struct _tuple8 _tmp221;return(_tmp221.f1=((
_tmp222=_cycalloc(sizeof(*_tmp222)),((_tmp222->hd=(void*)((void*)f->hd),((
_tmp222->tl=_tmpC7,_tmp222)))))),((_tmp221.f2=((_tmp223=_cycalloc(sizeof(*
_tmp223)),((_tmp223->hd=(void*)((void*)((struct Cyc_List_List*)_check_null(f->tl))->hd),((
_tmp223->tl=_tmpC8,_tmp223)))))),_tmp221)));}}}struct Cyc_List_List*Cyc_Tcdecl_sort_xtunion_fields(
struct Cyc_List_List*f,int*res,struct _dynforward_ptr*v,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr*msg);struct Cyc_List_List*Cyc_Tcdecl_sort_xtunion_fields(
struct Cyc_List_List*f,int*res,struct _dynforward_ptr*v,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr*msg){struct Cyc_List_List*_tmpCD;struct Cyc_List_List*
_tmpCE;struct _tuple8 _tmpCC=((struct _tuple8(*)(struct Cyc_List_List*f))Cyc_Tcdecl_split)(
f);_tmpCD=_tmpCC.f1;_tmpCE=_tmpCC.f2;if(_tmpCD != 0  && _tmpCD->tl != 0)_tmpCD=Cyc_Tcdecl_sort_xtunion_fields(
_tmpCD,res,v,loc,msg);if(_tmpCE != 0  && _tmpCE->tl != 0)_tmpCE=Cyc_Tcdecl_sort_xtunion_fields(
_tmpCE,res,v,loc,msg);{const char*_tmp225;int*_tmp224;return Cyc_Tcdecl_merge_xtunion_fields(
_tmpCD,_tmpCE,0,0,0,res,((_tmp224=_cycalloc_atomic(sizeof(*_tmp224)),((_tmp224[0]=
1,_tmp224)))),((_tmp225="xtunion",_tag_dynforward(_tmp225,sizeof(char),
_get_zero_arr_size_char(_tmp225,8)))),*v,loc,msg);}}struct _tuple9{struct Cyc_Core_Opt*
f1;struct Cyc_Core_Opt*f2;};struct Cyc_Absyn_Tuniondecl*Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl*d0,struct Cyc_Absyn_Tuniondecl*d1,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr*msg);struct Cyc_Absyn_Tuniondecl*Cyc_Tcdecl_merge_tuniondecl(
struct Cyc_Absyn_Tuniondecl*d0,struct Cyc_Absyn_Tuniondecl*d1,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr*msg){struct _dynforward_ptr _tmpD1=Cyc_Absynpp_qvar2string(
d0->name);const char*_tmp226;struct _dynforward_ptr t=(_tmp226="[x]tunion",
_tag_dynforward(_tmp226,sizeof(char),_get_zero_arr_size_char(_tmp226,10)));int
_tmpD2=1;if(d0->is_xtunion != d1->is_xtunion){{const char*_tmp22C;void*_tmp22B[3];
struct Cyc_String_pa_struct _tmp22A;struct Cyc_String_pa_struct _tmp229;struct Cyc_String_pa_struct
_tmp228;(_tmp228.tag=0,((_tmp228.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Tcdecl_is_x2string(d1->is_xtunion)),((_tmp229.tag=0,((
_tmp229.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Tcdecl_is_x2string(
d0->is_xtunion)),((_tmp22A.tag=0,((_tmp22A.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)_tmpD1),((_tmp22B[0]=& _tmp22A,((_tmp22B[1]=& _tmp229,((_tmp22B[2]=&
_tmp228,Cyc_Tcdecl_merr(loc,msg,((_tmp22C="expected %s to be a %s instead of a %s",
_tag_dynforward(_tmp22C,sizeof(char),_get_zero_arr_size_char(_tmp22C,39)))),
_tag_dynforward(_tmp22B,sizeof(void*),3)))))))))))))))))));}_tmpD2=0;}else{t=Cyc_Tcdecl_is_x2string(
d0->is_xtunion);}if(d0->is_flat != d1->is_flat){if(d0->is_flat){const char*_tmp22F;
void*_tmp22E;(_tmp22E=0,Cyc_Tcdecl_merr(loc,msg,((_tmp22F="expected __attribute__((flat))",
_tag_dynforward(_tmp22F,sizeof(char),_get_zero_arr_size_char(_tmp22F,31)))),
_tag_dynforward(_tmp22E,sizeof(void*),0)));}else{const char*_tmp232;void*_tmp231;(
_tmp231=0,Cyc_Tcdecl_merr(loc,msg,((_tmp232="did not expect __attribute__((flat))",
_tag_dynforward(_tmp232,sizeof(char),_get_zero_arr_size_char(_tmp232,37)))),
_tag_dynforward(_tmp231,sizeof(void*),0)));}}if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,
t,_tmpD1,loc,msg))return 0;{void*_tmpDD;int _tmpDE;struct _tuple3 _tmpDC=Cyc_Tcdecl_merge_scope((
void*)d0->sc,(void*)d1->sc,t,_tmpD1,loc,msg,0);_tmpDD=_tmpDC.f1;_tmpDE=_tmpDC.f2;
if(!_tmpDE)_tmpD2=0;{struct Cyc_Absyn_Tuniondecl*d2;{struct _tuple9 _tmp233;struct
_tuple9 _tmpE0=(_tmp233.f1=d0->fields,((_tmp233.f2=d1->fields,_tmp233)));struct
Cyc_Core_Opt*_tmpE1;struct Cyc_Core_Opt*_tmpE2;struct Cyc_Core_Opt*_tmpE3;struct
Cyc_Core_Opt _tmpE4;struct Cyc_List_List*_tmpE5;struct Cyc_Core_Opt*_tmpE6;struct
Cyc_Core_Opt _tmpE7;struct Cyc_List_List*_tmpE8;_LL26: _tmpE1=_tmpE0.f2;if(_tmpE1 != 
0)goto _LL28;_LL27: d2=d0;goto _LL25;_LL28: _tmpE2=_tmpE0.f1;if(_tmpE2 != 0)goto _LL2A;
_LL29: d2=d1;goto _LL25;_LL2A: _tmpE3=_tmpE0.f1;if(_tmpE3 == 0)goto _LL25;_tmpE4=*
_tmpE3;_tmpE5=(struct Cyc_List_List*)_tmpE4.v;_tmpE6=_tmpE0.f2;if(_tmpE6 == 0)goto
_LL25;_tmpE7=*_tmpE6;_tmpE8=(struct Cyc_List_List*)_tmpE7.v;_LL2B: {struct Cyc_List_List*
_tmpE9=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);if(d0->is_xtunion){int _tmpEA=1;
struct Cyc_List_List*_tmpEB=Cyc_Tcdecl_merge_xtunion_fields(_tmpE5,_tmpE8,_tmpE9,
d0->tvs,d1->tvs,& _tmpD2,& _tmpEA,t,_tmpD1,loc,msg);if(_tmpEA)d2=d0;else{{struct
Cyc_Absyn_Tuniondecl*_tmp234;d2=((_tmp234=_cycalloc(sizeof(*_tmp234)),((_tmp234[
0]=*d0,_tmp234))));}(void*)(d2->sc=(void*)_tmpDD);{struct Cyc_Core_Opt*_tmp235;d2->fields=((
_tmp235=_cycalloc(sizeof(*_tmp235)),((_tmp235->v=_tmpEB,_tmp235))));}}}else{for(
0;_tmpE5 != 0  && _tmpE8 != 0;(_tmpE5=_tmpE5->tl,_tmpE8=_tmpE8->tl)){Cyc_Tcdecl_merge_tunionfield((
struct Cyc_Absyn_Tunionfield*)_tmpE5->hd,(struct Cyc_Absyn_Tunionfield*)_tmpE8->hd,
_tmpE9,t,_tmpD1,msg);}if(_tmpE5 != 0){{const char*_tmp23B;void*_tmp23A[3];struct
Cyc_String_pa_struct _tmp239;struct Cyc_String_pa_struct _tmp238;struct Cyc_String_pa_struct
_tmp237;(_tmp237.tag=0,((_tmp237.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)*(*((struct Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)
_check_null(_tmpE8))->hd)->name).f2),((_tmp238.tag=0,((_tmp238.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmpD1),((_tmp239.tag=0,((_tmp239.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)t),((_tmp23A[0]=& _tmp239,((_tmp23A[
1]=& _tmp238,((_tmp23A[2]=& _tmp237,Cyc_Tcdecl_merr(loc,msg,((_tmp23B="%s %s has extra field %s",
_tag_dynforward(_tmp23B,sizeof(char),_get_zero_arr_size_char(_tmp23B,25)))),
_tag_dynforward(_tmp23A,sizeof(void*),3)))))))))))))))))));}_tmpD2=0;}if(_tmpE8
!= 0){{const char*_tmp241;void*_tmp240[3];struct Cyc_String_pa_struct _tmp23F;
struct Cyc_String_pa_struct _tmp23E;struct Cyc_String_pa_struct _tmp23D;(_tmp23D.tag=
0,((_tmp23D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*(*((struct Cyc_Absyn_Tunionfield*)
_tmpE8->hd)->name).f2),((_tmp23E.tag=0,((_tmp23E.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)_tmpD1),((_tmp23F.tag=0,((_tmp23F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)t),((_tmp240[0]=& _tmp23F,((_tmp240[1]=&
_tmp23E,((_tmp240[2]=& _tmp23D,Cyc_Tcdecl_merr(loc,msg,((_tmp241="%s %s is missing field %s",
_tag_dynforward(_tmp241,sizeof(char),_get_zero_arr_size_char(_tmp241,26)))),
_tag_dynforward(_tmp240,sizeof(void*),3)))))))))))))))))));}_tmpD2=0;}d2=d0;}
goto _LL25;}_LL25:;}if(!_tmpD2)return 0;if(_tmpDD == (void*)d2->sc)return(struct Cyc_Absyn_Tuniondecl*)
d2;else{{struct Cyc_Absyn_Tuniondecl*_tmp242;d2=((_tmp242=_cycalloc(sizeof(*
_tmp242)),((_tmp242[0]=*d2,_tmp242))));}(void*)(d2->sc=(void*)_tmpDD);return(
struct Cyc_Absyn_Tuniondecl*)d2;}}}}struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr*msg);struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(
struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr*msg){struct _dynforward_ptr _tmpFA=Cyc_Absynpp_qvar2string(
d0->name);int _tmpFB=1;void*_tmpFE;int _tmpFF;const char*_tmp243;struct _tuple3
_tmpFD=Cyc_Tcdecl_merge_scope((void*)d0->sc,(void*)d1->sc,((_tmp243="enum",
_tag_dynforward(_tmp243,sizeof(char),_get_zero_arr_size_char(_tmp243,5)))),
_tmpFA,loc,msg,1);_tmpFE=_tmpFD.f1;_tmpFF=_tmpFD.f2;if(!_tmpFF)_tmpFB=0;{struct
Cyc_Absyn_Enumdecl*d2;{struct _tuple9 _tmp244;struct _tuple9 _tmp101=(_tmp244.f1=d0->fields,((
_tmp244.f2=d1->fields,_tmp244)));struct Cyc_Core_Opt*_tmp102;struct Cyc_Core_Opt*
_tmp103;struct Cyc_Core_Opt*_tmp104;struct Cyc_Core_Opt _tmp105;struct Cyc_List_List*
_tmp106;struct Cyc_Core_Opt*_tmp107;struct Cyc_Core_Opt _tmp108;struct Cyc_List_List*
_tmp109;_LL2D: _tmp102=_tmp101.f2;if(_tmp102 != 0)goto _LL2F;_LL2E: d2=d0;goto _LL2C;
_LL2F: _tmp103=_tmp101.f1;if(_tmp103 != 0)goto _LL31;_LL30: d2=d1;goto _LL2C;_LL31:
_tmp104=_tmp101.f1;if(_tmp104 == 0)goto _LL2C;_tmp105=*_tmp104;_tmp106=(struct Cyc_List_List*)
_tmp105.v;_tmp107=_tmp101.f2;if(_tmp107 == 0)goto _LL2C;_tmp108=*_tmp107;_tmp109=(
struct Cyc_List_List*)_tmp108.v;_LL32: for(0;_tmp106 != 0  && _tmp109 != 0;(_tmp106=
_tmp106->tl,_tmp109=_tmp109->tl)){struct Cyc_Absyn_Enumfield _tmp10B;struct _tuple0*
_tmp10C;struct _tuple0 _tmp10D;struct _dynforward_ptr*_tmp10E;struct Cyc_Absyn_Exp*
_tmp10F;struct Cyc_Position_Segment*_tmp110;struct Cyc_Absyn_Enumfield*_tmp10A=(
struct Cyc_Absyn_Enumfield*)_tmp106->hd;_tmp10B=*_tmp10A;_tmp10C=_tmp10B.name;
_tmp10D=*_tmp10C;_tmp10E=_tmp10D.f2;_tmp10F=_tmp10B.tag;_tmp110=_tmp10B.loc;{
struct Cyc_Absyn_Enumfield _tmp112;struct _tuple0*_tmp113;struct _tuple0 _tmp114;
struct _dynforward_ptr*_tmp115;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Position_Segment*
_tmp117;struct Cyc_Absyn_Enumfield*_tmp111=(struct Cyc_Absyn_Enumfield*)_tmp109->hd;
_tmp112=*_tmp111;_tmp113=_tmp112.name;_tmp114=*_tmp113;_tmp115=_tmp114.f2;
_tmp116=_tmp112.tag;_tmp117=_tmp112.loc;if(Cyc_strptrcmp(_tmp10E,_tmp115)!= 0){{
const char*_tmp24A;void*_tmp249[3];struct Cyc_String_pa_struct _tmp248;struct Cyc_String_pa_struct
_tmp247;struct Cyc_String_pa_struct _tmp246;(_tmp246.tag=0,((_tmp246.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp115),((_tmp247.tag=0,((_tmp247.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*_tmp10E),((_tmp248.tag=0,((
_tmp248.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)_tmpFA),((_tmp249[0]=&
_tmp248,((_tmp249[1]=& _tmp247,((_tmp249[2]=& _tmp246,Cyc_Tcdecl_merr(loc,msg,((
_tmp24A="enum %s: field name mismatch %s != %s",_tag_dynforward(_tmp24A,sizeof(
char),_get_zero_arr_size_char(_tmp24A,38)))),_tag_dynforward(_tmp249,sizeof(void*),
3)))))))))))))))))));}_tmpFB=0;}if(Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)
_check_null(_tmp10F))!= Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)
_check_null(_tmp116))){{const char*_tmp24F;void*_tmp24E[2];struct Cyc_String_pa_struct
_tmp24D;struct Cyc_String_pa_struct _tmp24C;(_tmp24C.tag=0,((_tmp24C.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp115),((_tmp24D.tag=0,((_tmp24D.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmpFA),((_tmp24E[0]=& _tmp24D,((
_tmp24E[1]=& _tmp24C,Cyc_Tcdecl_merr(loc,msg,((_tmp24F="enum %s, field %s, value mismatch",
_tag_dynforward(_tmp24F,sizeof(char),_get_zero_arr_size_char(_tmp24F,34)))),
_tag_dynforward(_tmp24E,sizeof(void*),2)))))))))))));}_tmpFB=0;}}}d2=d0;goto
_LL2C;_LL2C:;}if(!_tmpFB)return 0;if((void*)d2->sc == _tmpFE)return(struct Cyc_Absyn_Enumdecl*)
d2;else{{struct Cyc_Absyn_Enumdecl*_tmp250;d2=((_tmp250=_cycalloc(sizeof(*_tmp250)),((
_tmp250[0]=*d2,_tmp250))));}(void*)(d2->sc=(void*)_tmpFE);return(struct Cyc_Absyn_Enumdecl*)
d2;}}}static struct _tuple3 Cyc_Tcdecl_check_var_or_fn_decl(void*sc0,void*t0,struct
Cyc_Absyn_Tqual tq0,struct Cyc_List_List*atts0,void*sc1,void*t1,struct Cyc_Absyn_Tqual
tq1,struct Cyc_List_List*atts1,struct _dynforward_ptr t,struct _dynforward_ptr v,
struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg);static struct _tuple3 Cyc_Tcdecl_check_var_or_fn_decl(
void*sc0,void*t0,struct Cyc_Absyn_Tqual tq0,struct Cyc_List_List*atts0,void*sc1,
void*t1,struct Cyc_Absyn_Tqual tq1,struct Cyc_List_List*atts1,struct _dynforward_ptr
t,struct _dynforward_ptr v,struct Cyc_Position_Segment*loc,struct _dynforward_ptr*
msg){int _tmp122=1;void*_tmp124;int _tmp125;struct _tuple3 _tmp123=Cyc_Tcdecl_merge_scope(
sc0,sc1,t,v,loc,msg,0);_tmp124=_tmp123.f1;_tmp125=_tmp123.f2;if(!_tmp125)_tmp122=
0;if(!Cyc_Tcdecl_check_type(t0,t1)){{const char*_tmp257;void*_tmp256[4];struct Cyc_String_pa_struct
_tmp255;struct Cyc_String_pa_struct _tmp254;struct Cyc_String_pa_struct _tmp253;
struct Cyc_String_pa_struct _tmp252;(_tmp252.tag=0,((_tmp252.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t0)),((_tmp253.tag=
0,((_tmp253.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t1)),((_tmp254.tag=0,((_tmp254.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
v),((_tmp255.tag=0,((_tmp255.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)t),((
_tmp256[0]=& _tmp255,((_tmp256[1]=& _tmp254,((_tmp256[2]=& _tmp253,((_tmp256[3]=&
_tmp252,Cyc_Tcdecl_merr(loc,msg,((_tmp257="%s %s has type \n%s\n instead of \n%s\n",
_tag_dynforward(_tmp257,sizeof(char),_get_zero_arr_size_char(_tmp257,36)))),
_tag_dynforward(_tmp256,sizeof(void*),4)))))))))))))))))))))))));}Cyc_Tcutil_explain_failure();
_tmp122=0;}if(!Cyc_Tcutil_equal_tqual(tq0,tq1)){{const char*_tmp25C;void*_tmp25B[
2];struct Cyc_String_pa_struct _tmp25A;struct Cyc_String_pa_struct _tmp259;(_tmp259.tag=
0,((_tmp259.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)v),((_tmp25A.tag=0,((
_tmp25A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)t),((_tmp25B[0]=&
_tmp25A,((_tmp25B[1]=& _tmp259,Cyc_Tcdecl_merr(loc,msg,((_tmp25C="%s %s has different type qualifiers",
_tag_dynforward(_tmp25C,sizeof(char),_get_zero_arr_size_char(_tmp25C,36)))),
_tag_dynforward(_tmp25B,sizeof(void*),2)))))))))))));}_tmp122=0;}if(!Cyc_Tcutil_same_atts(
atts0,atts1)){{const char*_tmp261;void*_tmp260[2];struct Cyc_String_pa_struct
_tmp25F;struct Cyc_String_pa_struct _tmp25E;(_tmp25E.tag=0,((_tmp25E.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)v),((_tmp25F.tag=0,((_tmp25F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)t),((_tmp260[0]=& _tmp25F,((_tmp260[1]=&
_tmp25E,Cyc_Tcdecl_merr(loc,msg,((_tmp261="%s %s has different attributes",
_tag_dynforward(_tmp261,sizeof(char),_get_zero_arr_size_char(_tmp261,31)))),
_tag_dynforward(_tmp260,sizeof(void*),2)))))))))))));}{const char*_tmp264;void*
_tmp263;(_tmp263=0,Cyc_fprintf(Cyc_stderr,((_tmp264="previous attributes: ",
_tag_dynforward(_tmp264,sizeof(char),_get_zero_arr_size_char(_tmp264,22)))),
_tag_dynforward(_tmp263,sizeof(void*),0)));}for(0;atts0 != 0;atts0=atts0->tl){
const char*_tmp268;void*_tmp267[1];struct Cyc_String_pa_struct _tmp266;(_tmp266.tag=
0,((_tmp266.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absyn_attribute2string((
void*)atts0->hd)),((_tmp267[0]=& _tmp266,Cyc_fprintf(Cyc_stderr,((_tmp268="%s ",
_tag_dynforward(_tmp268,sizeof(char),_get_zero_arr_size_char(_tmp268,4)))),
_tag_dynforward(_tmp267,sizeof(void*),1)))))));}{const char*_tmp26B;void*_tmp26A;(
_tmp26A=0,Cyc_fprintf(Cyc_stderr,((_tmp26B="\ncurrent attributes: ",
_tag_dynforward(_tmp26B,sizeof(char),_get_zero_arr_size_char(_tmp26B,22)))),
_tag_dynforward(_tmp26A,sizeof(void*),0)));}for(0;atts1 != 0;atts1=atts1->tl){
const char*_tmp26F;void*_tmp26E[1];struct Cyc_String_pa_struct _tmp26D;(_tmp26D.tag=
0,((_tmp26D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absyn_attribute2string((
void*)atts1->hd)),((_tmp26E[0]=& _tmp26D,Cyc_fprintf(Cyc_stderr,((_tmp26F="%s ",
_tag_dynforward(_tmp26F,sizeof(char),_get_zero_arr_size_char(_tmp26F,4)))),
_tag_dynforward(_tmp26E,sizeof(void*),1)))))));}{const char*_tmp272;void*_tmp271;(
_tmp271=0,Cyc_fprintf(Cyc_stderr,((_tmp272="\n",_tag_dynforward(_tmp272,sizeof(
char),_get_zero_arr_size_char(_tmp272,2)))),_tag_dynforward(_tmp271,sizeof(void*),
0)));}_tmp122=0;}{struct _tuple3 _tmp273;return(_tmp273.f1=_tmp124,((_tmp273.f2=
_tmp122,_tmp273)));}}struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*
d0,struct Cyc_Absyn_Vardecl*d1,struct Cyc_Position_Segment*loc,struct
_dynforward_ptr*msg);struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*
d0,struct Cyc_Absyn_Vardecl*d1,struct Cyc_Position_Segment*loc,struct
_dynforward_ptr*msg){struct _dynforward_ptr _tmp141=Cyc_Absynpp_qvar2string(d0->name);
void*_tmp144;int _tmp145;const char*_tmp274;struct _tuple3 _tmp143=Cyc_Tcdecl_check_var_or_fn_decl((
void*)d0->sc,(void*)d0->type,d0->tq,d0->attributes,(void*)d1->sc,(void*)d1->type,
d1->tq,d1->attributes,((_tmp274="variable",_tag_dynforward(_tmp274,sizeof(char),
_get_zero_arr_size_char(_tmp274,9)))),_tmp141,loc,msg);_tmp144=_tmp143.f1;
_tmp145=_tmp143.f2;if(!_tmp145)return 0;if((void*)d0->sc == _tmp144)return(struct
Cyc_Absyn_Vardecl*)d0;else{struct Cyc_Absyn_Vardecl*_tmp275;struct Cyc_Absyn_Vardecl*
_tmp146=(_tmp275=_cycalloc(sizeof(*_tmp275)),((_tmp275[0]=*d0,_tmp275)));(void*)(((
struct Cyc_Absyn_Vardecl*)_check_null(_tmp146))->sc=(void*)_tmp144);return _tmp146;}}
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*
d0,struct Cyc_Absyn_Typedefdecl*d1,struct Cyc_Position_Segment*loc,struct
_dynforward_ptr*msg);struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(
struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,struct Cyc_Position_Segment*
loc,struct _dynforward_ptr*msg){struct _dynforward_ptr _tmp148=Cyc_Absynpp_qvar2string(
d0->name);{const char*_tmp276;if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,((_tmp276="typedef",
_tag_dynforward(_tmp276,sizeof(char),_get_zero_arr_size_char(_tmp276,8)))),
_tmp148,loc,msg))return 0;}{struct Cyc_List_List*_tmp14A=Cyc_Tcdecl_build_tvs_map(
d0->tvs,d1->tvs);if(d0->defn != 0  && d1->defn != 0){void*subst_defn1=Cyc_Tcutil_substitute(
_tmp14A,(void*)((struct Cyc_Core_Opt*)_check_null(d1->defn))->v);if(!Cyc_Tcdecl_check_type((
void*)((struct Cyc_Core_Opt*)_check_null(d0->defn))->v,subst_defn1)){{const char*
_tmp27C;void*_tmp27B[3];struct Cyc_String_pa_struct _tmp27A;struct Cyc_String_pa_struct
_tmp279;struct Cyc_String_pa_struct _tmp278;(_tmp278.tag=0,((_tmp278.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(d0->defn))->v)),((_tmp279.tag=0,((_tmp279.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(subst_defn1)),((
_tmp27A.tag=0,((_tmp27A.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp148),((_tmp27B[0]=& _tmp27A,((_tmp27B[1]=& _tmp279,((_tmp27B[2]=& _tmp278,Cyc_Tcdecl_merr(
loc,msg,((_tmp27C="typedef %s does not refer to the same type: %s != %s",
_tag_dynforward(_tmp27C,sizeof(char),_get_zero_arr_size_char(_tmp27C,53)))),
_tag_dynforward(_tmp27B,sizeof(void*),3)))))))))))))))))));}return 0;}}return(
struct Cyc_Absyn_Typedefdecl*)d0;}}void*Cyc_Tcdecl_merge_binding(void*b0,void*b1,
struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg);void*Cyc_Tcdecl_merge_binding(
void*b0,void*b1,struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg){struct
_tuple4 _tmp27D;struct _tuple4 _tmp151=(_tmp27D.f1=b0,((_tmp27D.f2=b1,_tmp27D)));
void*_tmp152;void*_tmp153;void*_tmp154;struct Cyc_Absyn_Vardecl*_tmp155;void*
_tmp156;struct Cyc_Absyn_Vardecl*_tmp157;void*_tmp158;struct Cyc_Absyn_Vardecl*
_tmp159;void*_tmp15A;struct Cyc_Absyn_Fndecl*_tmp15B;void*_tmp15C;void*_tmp15D;
struct Cyc_Absyn_Fndecl*_tmp15E;void*_tmp15F;struct Cyc_Absyn_Fndecl*_tmp160;void*
_tmp161;struct Cyc_Absyn_Vardecl*_tmp162;_LL34: _tmp152=_tmp151.f1;if((int)_tmp152
!= 0)goto _LL36;_tmp153=_tmp151.f2;if((int)_tmp153 != 0)goto _LL36;_LL35: return(
void*)0;_LL36: _tmp154=_tmp151.f1;if(_tmp154 <= (void*)1)goto _LL38;if(*((int*)
_tmp154)!= 0)goto _LL38;_tmp155=((struct Cyc_Absyn_Global_b_struct*)_tmp154)->f1;
_tmp156=_tmp151.f2;if(_tmp156 <= (void*)1)goto _LL38;if(*((int*)_tmp156)!= 0)goto
_LL38;_tmp157=((struct Cyc_Absyn_Global_b_struct*)_tmp156)->f1;_LL37: {struct Cyc_Absyn_Vardecl*
_tmp163=Cyc_Tcdecl_merge_vardecl(_tmp155,_tmp157,loc,msg);if(_tmp163 == 0)return(
void*)0;if(_tmp163 == (struct Cyc_Absyn_Vardecl*)_tmp155)return b0;if(_tmp163 == (
struct Cyc_Absyn_Vardecl*)_tmp157)return b1;{struct Cyc_Absyn_Global_b_struct
_tmp280;struct Cyc_Absyn_Global_b_struct*_tmp27F;return(void*)((_tmp27F=_cycalloc(
sizeof(*_tmp27F)),((_tmp27F[0]=((_tmp280.tag=0,((_tmp280.f1=(struct Cyc_Absyn_Vardecl*)
_tmp163,_tmp280)))),_tmp27F))));}}_LL38: _tmp158=_tmp151.f1;if(_tmp158 <= (void*)1)
goto _LL3A;if(*((int*)_tmp158)!= 0)goto _LL3A;_tmp159=((struct Cyc_Absyn_Global_b_struct*)
_tmp158)->f1;_tmp15A=_tmp151.f2;if(_tmp15A <= (void*)1)goto _LL3A;if(*((int*)
_tmp15A)!= 1)goto _LL3A;_tmp15B=((struct Cyc_Absyn_Funname_b_struct*)_tmp15A)->f1;
_LL39: {int _tmp168;const char*_tmp281;struct _tuple3 _tmp167=Cyc_Tcdecl_check_var_or_fn_decl((
void*)_tmp159->sc,(void*)_tmp159->type,_tmp159->tq,_tmp159->attributes,(void*)
_tmp15B->sc,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp15B->cached_typ))->v,
Cyc_Absyn_empty_tqual(0),_tmp15B->attributes,((_tmp281="function",
_tag_dynforward(_tmp281,sizeof(char),_get_zero_arr_size_char(_tmp281,9)))),Cyc_Absynpp_qvar2string(
_tmp159->name),loc,msg);_tmp168=_tmp167.f2;return _tmp168?b1:(void*)0;}_LL3A:
_tmp15C=_tmp151.f1;if(_tmp15C <= (void*)1)goto _LL3C;if(*((int*)_tmp15C)!= 1)goto
_LL3C;_tmp15D=_tmp151.f2;if(_tmp15D <= (void*)1)goto _LL3C;if(*((int*)_tmp15D)!= 1)
goto _LL3C;_tmp15E=((struct Cyc_Absyn_Funname_b_struct*)_tmp15D)->f1;_LL3B:{const
char*_tmp285;void*_tmp284[1];struct Cyc_String_pa_struct _tmp283;(_tmp283.tag=0,((
_tmp283.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp15E->name)),((_tmp284[0]=& _tmp283,Cyc_Tcdecl_merr(loc,msg,((_tmp285="redefinition of function %s",
_tag_dynforward(_tmp285,sizeof(char),_get_zero_arr_size_char(_tmp285,28)))),
_tag_dynforward(_tmp284,sizeof(void*),1)))))));}return(void*)0;_LL3C: _tmp15F=
_tmp151.f1;if(_tmp15F <= (void*)1)goto _LL3E;if(*((int*)_tmp15F)!= 1)goto _LL3E;
_tmp160=((struct Cyc_Absyn_Funname_b_struct*)_tmp15F)->f1;_tmp161=_tmp151.f2;if(
_tmp161 <= (void*)1)goto _LL3E;if(*((int*)_tmp161)!= 0)goto _LL3E;_tmp162=((struct
Cyc_Absyn_Global_b_struct*)_tmp161)->f1;_LL3D: {int _tmp16E;const char*_tmp286;
struct _tuple3 _tmp16D=Cyc_Tcdecl_check_var_or_fn_decl((void*)_tmp160->sc,(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp160->cached_typ))->v,Cyc_Absyn_empty_tqual(0),
_tmp160->attributes,(void*)_tmp162->sc,(void*)_tmp162->type,_tmp162->tq,_tmp162->attributes,((
_tmp286="variable",_tag_dynforward(_tmp286,sizeof(char),_get_zero_arr_size_char(
_tmp286,9)))),Cyc_Absynpp_qvar2string(_tmp160->name),loc,msg);_tmp16E=_tmp16D.f2;
return _tmp16E?b0:(void*)0;}_LL3E:;_LL3F: {struct Cyc_Core_Invalid_argument_struct
_tmp28C;const char*_tmp28B;struct Cyc_Core_Invalid_argument_struct*_tmp28A;(int)
_throw((void*)((_tmp28A=_cycalloc(sizeof(*_tmp28A)),((_tmp28A[0]=((_tmp28C.tag=
Cyc_Core_Invalid_argument,((_tmp28C.f1=((_tmp28B="Tcdecl::merge_binding",
_tag_dynforward(_tmp28B,sizeof(char),_get_zero_arr_size_char(_tmp28B,22)))),
_tmp28C)))),_tmp28A)))));}_LL33:;}struct Cyc_Tcdecl_Xtunionfielddecl*Cyc_Tcdecl_merge_xtunionfielddecl(
struct Cyc_Tcdecl_Xtunionfielddecl*d0,struct Cyc_Tcdecl_Xtunionfielddecl*d1,struct
Cyc_Position_Segment*loc,struct _dynforward_ptr*msg);struct Cyc_Tcdecl_Xtunionfielddecl*
Cyc_Tcdecl_merge_xtunionfielddecl(struct Cyc_Tcdecl_Xtunionfielddecl*d0,struct Cyc_Tcdecl_Xtunionfielddecl*
d1,struct Cyc_Position_Segment*loc,struct _dynforward_ptr*msg){struct Cyc_Tcdecl_Xtunionfielddecl
_tmp173;struct Cyc_Absyn_Tuniondecl*_tmp174;struct Cyc_Absyn_Tunionfield*_tmp175;
struct Cyc_Tcdecl_Xtunionfielddecl*_tmp172=d0;_tmp173=*_tmp172;_tmp174=_tmp173.base;
_tmp175=_tmp173.field;{struct Cyc_Tcdecl_Xtunionfielddecl _tmp177;struct Cyc_Absyn_Tuniondecl*
_tmp178;struct Cyc_Absyn_Tunionfield*_tmp179;struct Cyc_Tcdecl_Xtunionfielddecl*
_tmp176=d1;_tmp177=*_tmp176;_tmp178=_tmp177.base;_tmp179=_tmp177.field;{struct
_dynforward_ptr _tmp17A=Cyc_Absynpp_qvar2string(_tmp175->name);if(Cyc_Absyn_qvar_cmp(
_tmp174->name,_tmp178->name)!= 0){{const char*_tmp292;void*_tmp291[3];struct Cyc_String_pa_struct
_tmp290;struct Cyc_String_pa_struct _tmp28F;struct Cyc_String_pa_struct _tmp28E;(
_tmp28E.tag=0,((_tmp28E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)
_tmp17A),((_tmp28F.tag=0,((_tmp28F.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_qvar2string(_tmp178->name)),((_tmp290.tag=0,((
_tmp290.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_qvar2string(
_tmp174->name)),((_tmp291[0]=& _tmp290,((_tmp291[1]=& _tmp28F,((_tmp291[2]=&
_tmp28E,Cyc_Tcdecl_merr(loc,msg,((_tmp292="xtunions %s and %s have both a field named %s",
_tag_dynforward(_tmp292,sizeof(char),_get_zero_arr_size_char(_tmp292,46)))),
_tag_dynforward(_tmp291,sizeof(void*),3)))))))))))))))))));}return 0;}{const char*
_tmp293;if(!Cyc_Tcdecl_check_tvs(_tmp174->tvs,_tmp178->tvs,((_tmp293="xtunion",
_tag_dynforward(_tmp293,sizeof(char),_get_zero_arr_size_char(_tmp293,8)))),Cyc_Absynpp_qvar2string(
_tmp174->name),loc,msg))return 0;}{struct Cyc_List_List*_tmp181=Cyc_Tcdecl_build_tvs_map(
_tmp174->tvs,_tmp178->tvs);const char*_tmp294;struct Cyc_Absyn_Tunionfield*_tmp182=
Cyc_Tcdecl_merge_tunionfield(_tmp175,_tmp179,_tmp181,((_tmp294="xtunionfield",
_tag_dynforward(_tmp294,sizeof(char),_get_zero_arr_size_char(_tmp294,13)))),
_tmp17A,msg);if(_tmp182 == 0)return 0;if(_tmp182 == (struct Cyc_Absyn_Tunionfield*)
_tmp175)return(struct Cyc_Tcdecl_Xtunionfielddecl*)d0;{struct Cyc_Tcdecl_Xtunionfielddecl*
_tmp295;return(_tmp295=_cycalloc(sizeof(*_tmp295)),((_tmp295->base=_tmp174,((
_tmp295->field=(struct Cyc_Absyn_Tunionfield*)_tmp182,_tmp295)))));}}}}}
