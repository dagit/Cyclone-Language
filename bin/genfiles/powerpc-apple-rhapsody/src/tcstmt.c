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
Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dynforward_ptr msg_part);struct
_tuple6{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_TcPatResult{
struct _tuple6*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt);void
Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p);void
Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*,struct Cyc_List_List*);
int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*,struct Cyc_Absyn_Pat*
p);void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*,struct Cyc_List_List*);
struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_struct{
int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*
fields;};struct Cyc_CfFlowInfo_EqualConst_struct{int tag;unsigned int f1;};struct
Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessSize_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_LessConst_struct{int tag;
unsigned int f1;};struct Cyc_CfFlowInfo_LessEqSize_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};union Cyc_CfFlowInfo_RelnOp_union{struct Cyc_CfFlowInfo_EqualConst_struct
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
int new_block);static void Cyc_Tcstmt_decorate_stmt(struct Cyc_Tcenv_Tenv*te,struct
Cyc_Absyn_Stmt*s);static void Cyc_Tcstmt_decorate_stmt(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Stmt*s){Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(
te));s->try_depth=Cyc_Tcenv_get_try_depth(te);}static void Cyc_Tcstmt_pattern_synth(
struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult
pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int new_block);
static void Cyc_Tcstmt_pattern_synth(struct Cyc_Position_Segment*loc,struct Cyc_Tcenv_Tenv*
te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*
where_opt,int new_block){struct _tuple6*_tmp1;struct Cyc_List_List*_tmp2;struct Cyc_Tcpat_TcPatResult
_tmp0=pat_res;_tmp1=_tmp0.tvars_and_bounds_opt;_tmp2=_tmp0.patvars;{struct Cyc_List_List*
_tmp3=_tmp1 == 0?0:(*_tmp1).f1;struct Cyc_List_List*_tmp4=_tmp1 == 0?0:(*_tmp1).f2;{
struct _RegionHandle _tmp5=_new_region("r");struct _RegionHandle*r=& _tmp5;
_push_region(r);{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r,loc,te,_tmp3);
te2=Cyc_Tcenv_new_outlives_constraints(r,te2,_tmp4,loc);if(new_block)te2=Cyc_Tcenv_new_block(
r,loc,te2);{struct Cyc_Core_Opt*_tmp141;struct Cyc_Core_Opt*_tmp6=(_tmp141=
_cycalloc(sizeof(*_tmp141)),((_tmp141->v=(void*)Cyc_Tcenv_curr_rgn(te2),_tmp141)));{
struct Cyc_List_List*_tmp7=_tmp2;for(0;_tmp7 != 0;_tmp7=_tmp7->tl){te2=Cyc_Tcenv_add_pat_var(
r,loc,te2,(struct Cyc_Absyn_Vardecl*)_tmp7->hd);((struct Cyc_Absyn_Vardecl*)_tmp7->hd)->rgn=
_tmp6;}}if(where_opt != 0){const char*_tmp142;Cyc_Tcexp_tcTest(te2,(struct Cyc_Absyn_Exp*)
where_opt,((_tmp142="switch clause guard",_tag_dynforward(_tmp142,sizeof(char),
_get_zero_arr_size_char(_tmp142,20)))));}if(_tmp2 != 0)te2=Cyc_Tcenv_set_encloser(
r,te2,s);Cyc_Tcstmt_tcStmt(te2,s,0);}};_pop_region(r);}if(_tmp2 != 0)Cyc_NewControlFlow_set_encloser(
s,Cyc_Tcenv_get_encloser(te));}}static struct Cyc_List_List*Cyc_Tcstmt_cmap_c(
struct _RegionHandle*r,void*(*f)(void*,void*),void*env,struct Cyc_Tcenv_CList*x);
static struct Cyc_List_List*Cyc_Tcstmt_cmap_c(struct _RegionHandle*r,void*(*f)(void*,
void*),void*env,struct Cyc_Tcenv_CList*x){if(x == 0)return 0;else{struct Cyc_List_List*
_tmp143;return(_tmp143=_region_malloc(r,sizeof(*_tmp143)),((_tmp143->hd=(void*)f(
env,(void*)x->hd),((_tmp143->tl=Cyc_Tcstmt_cmap_c(r,f,env,x->tl),_tmp143)))));}}
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int
new_block);void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,
int new_block){Cyc_Tcstmt_decorate_stmt(te,s0);{void*_tmpB=(void*)s0->r;struct Cyc_Absyn_Exp*
_tmpC;struct Cyc_Absyn_Stmt*_tmpD;struct Cyc_Absyn_Stmt*_tmpE;struct Cyc_Absyn_Exp*
_tmpF;struct Cyc_Absyn_Exp*_tmp10;struct Cyc_Absyn_Stmt*_tmp11;struct Cyc_Absyn_Stmt*
_tmp12;struct _tuple2 _tmp13;struct Cyc_Absyn_Exp*_tmp14;struct Cyc_Absyn_Stmt*
_tmp15;struct Cyc_Absyn_Stmt*_tmp16;struct Cyc_Absyn_Exp*_tmp17;struct _tuple2
_tmp18;struct Cyc_Absyn_Exp*_tmp19;struct Cyc_Absyn_Stmt*_tmp1A;struct _tuple2
_tmp1B;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_Absyn_Stmt*_tmp1D;struct Cyc_Absyn_Stmt*
_tmp1E;struct Cyc_Absyn_Stmt*_tmp1F;struct _tuple2 _tmp20;struct Cyc_Absyn_Exp*
_tmp21;struct Cyc_Absyn_Stmt*_tmp22;struct Cyc_Absyn_Stmt*_tmp23;struct Cyc_Absyn_Stmt**
_tmp24;struct Cyc_Absyn_Stmt*_tmp25;struct Cyc_Absyn_Stmt**_tmp26;struct
_dynforward_ptr*_tmp27;struct Cyc_Absyn_Stmt*_tmp28;struct Cyc_Absyn_Stmt**_tmp29;
struct Cyc_List_List*_tmp2A;struct Cyc_Absyn_Switch_clause**_tmp2B;struct Cyc_Absyn_Switch_clause***
_tmp2C;struct _dynforward_ptr*_tmp2D;struct Cyc_Absyn_Stmt*_tmp2E;struct Cyc_Absyn_Exp*
_tmp2F;struct Cyc_List_List*_tmp30;struct Cyc_Absyn_Stmt*_tmp31;struct Cyc_List_List*
_tmp32;struct Cyc_Absyn_Decl*_tmp33;struct Cyc_Absyn_Stmt*_tmp34;struct Cyc_Absyn_Tvar*
_tmp35;struct Cyc_Absyn_Vardecl*_tmp36;int _tmp37;struct Cyc_Absyn_Exp*_tmp38;
struct Cyc_Absyn_Stmt*_tmp39;struct Cyc_Absyn_Exp*_tmp3A;struct Cyc_Absyn_Exp*
_tmp3B;struct Cyc_Absyn_Tvar*_tmp3C;struct Cyc_Absyn_Vardecl*_tmp3D;struct Cyc_Absyn_Stmt*
_tmp3E;_LL1: if((int)_tmpB != 0)goto _LL3;_LL2: return;_LL3: if(_tmpB <= (void*)1)goto
_LL5;if(*((int*)_tmpB)!= 0)goto _LL5;_tmpC=((struct Cyc_Absyn_Exp_s_struct*)_tmpB)->f1;
_LL4: Cyc_Tcexp_tcExp(te,0,_tmpC);return;_LL5: if(_tmpB <= (void*)1)goto _LL7;if(*((
int*)_tmpB)!= 1)goto _LL7;_tmpD=((struct Cyc_Absyn_Seq_s_struct*)_tmpB)->f1;_tmpE=((
struct Cyc_Absyn_Seq_s_struct*)_tmpB)->f2;_LL6:{struct _RegionHandle _tmp3F=
_new_region("r");struct _RegionHandle*r=& _tmp3F;_push_region(r);{struct Cyc_Tcenv_Stmt_j_struct
_tmp146;struct Cyc_Tcenv_Stmt_j_struct*_tmp145;struct Cyc_Tcenv_Tenv*_tmp40=Cyc_Tcenv_set_next(
r,te,(void*)((_tmp145=_cycalloc(sizeof(*_tmp145)),((_tmp145[0]=((_tmp146.tag=0,((
_tmp146.f1=_tmpE,_tmp146)))),_tmp145)))));Cyc_Tcstmt_tcStmt(_tmp40,_tmpD,1);};
_pop_region(r);}Cyc_Tcstmt_tcStmt(te,_tmpE,1);return;_LL7: if(_tmpB <= (void*)1)
goto _LL9;if(*((int*)_tmpB)!= 2)goto _LL9;_tmpF=((struct Cyc_Absyn_Return_s_struct*)
_tmpB)->f1;_LL8: {void*t=Cyc_Tcenv_return_typ(te);if(_tmpF == 0){void*_tmp43=Cyc_Tcutil_compress(
t);_LL28: if((int)_tmp43 != 0)goto _LL2A;_LL29: goto _LL27;_LL2A:;_LL2B:{const char*
_tmp14A;void*_tmp149[1];struct Cyc_String_pa_struct _tmp148;(_tmp148.tag=0,((
_tmp148.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
t)),((_tmp149[0]=& _tmp148,Cyc_Tcutil_terr(s0->loc,((_tmp14A="must return a value of type %s",
_tag_dynforward(_tmp14A,sizeof(char),_get_zero_arr_size_char(_tmp14A,31)))),
_tag_dynforward(_tmp149,sizeof(void*),1)))))));}goto _LL27;_LL27:;}else{struct Cyc_Absyn_Exp*
e=(struct Cyc_Absyn_Exp*)_tmpF;Cyc_Tcexp_tcExp(te,(void**)& t,e);if(!Cyc_Tcutil_coerce_arg(
te,e,t)){{const char*_tmp14F;void*_tmp14E[2];struct Cyc_String_pa_struct _tmp14D;
struct Cyc_String_pa_struct _tmp14C;(_tmp14C.tag=0,((_tmp14C.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(t)),((_tmp14D.tag=
0,((_tmp14D.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v)),((_tmp14E[0]=& _tmp14D,((
_tmp14E[1]=& _tmp14C,Cyc_Tcutil_terr(s0->loc,((_tmp14F="returns value of type %s but requires %s",
_tag_dynforward(_tmp14F,sizeof(char),_get_zero_arr_size_char(_tmp14F,41)))),
_tag_dynforward(_tmp14E,sizeof(void*),2)))))))))))));}Cyc_Tcutil_explain_failure();}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !Cyc_Tcutil_is_noalias_path(e)){
const char*_tmp152;void*_tmp151;(_tmp151=0,Cyc_Tcutil_terr(e->loc,((_tmp152="Cannot consume paths; do swap instead",
_tag_dynforward(_tmp152,sizeof(char),_get_zero_arr_size_char(_tmp152,38)))),
_tag_dynforward(_tmp151,sizeof(void*),0)));}}return;}_LL9: if(_tmpB <= (void*)1)
goto _LLB;if(*((int*)_tmpB)!= 3)goto _LLB;_tmp10=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmpB)->f1;_tmp11=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmpB)->f2;_tmp12=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmpB)->f3;_LLA:{const char*_tmp153;Cyc_Tcexp_tcTest(
te,_tmp10,((_tmp153="if statement",_tag_dynforward(_tmp153,sizeof(char),
_get_zero_arr_size_char(_tmp153,13)))));}Cyc_Tcstmt_tcStmt(te,_tmp11,1);Cyc_Tcstmt_tcStmt(
te,_tmp12,1);return;_LLB: if(_tmpB <= (void*)1)goto _LLD;if(*((int*)_tmpB)!= 4)goto
_LLD;_tmp13=((struct Cyc_Absyn_While_s_struct*)_tmpB)->f1;_tmp14=_tmp13.f1;_tmp15=
_tmp13.f2;_tmp16=((struct Cyc_Absyn_While_s_struct*)_tmpB)->f2;_LLC: Cyc_Tcstmt_decorate_stmt(
te,_tmp15);{const char*_tmp154;Cyc_Tcexp_tcTest(te,_tmp14,((_tmp154="while loop",
_tag_dynforward(_tmp154,sizeof(char),_get_zero_arr_size_char(_tmp154,11)))));}{
struct _RegionHandle _tmp4F=_new_region("r");struct _RegionHandle*r=& _tmp4F;
_push_region(r);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(r,te,_tmp15),_tmp16,1);;
_pop_region(r);}return;_LLD: if(_tmpB <= (void*)1)goto _LLF;if(*((int*)_tmpB)!= 8)
goto _LLF;_tmp17=((struct Cyc_Absyn_For_s_struct*)_tmpB)->f1;_tmp18=((struct Cyc_Absyn_For_s_struct*)
_tmpB)->f2;_tmp19=_tmp18.f1;_tmp1A=_tmp18.f2;_tmp1B=((struct Cyc_Absyn_For_s_struct*)
_tmpB)->f3;_tmp1C=_tmp1B.f1;_tmp1D=_tmp1B.f2;_tmp1E=((struct Cyc_Absyn_For_s_struct*)
_tmpB)->f4;_LLE: Cyc_Tcstmt_decorate_stmt(te,_tmp1A);Cyc_Tcstmt_decorate_stmt(te,
_tmp1D);Cyc_Tcexp_tcExp(te,0,_tmp17);{const char*_tmp155;Cyc_Tcexp_tcTest(te,
_tmp19,((_tmp155="for loop",_tag_dynforward(_tmp155,sizeof(char),
_get_zero_arr_size_char(_tmp155,9)))));}{struct _RegionHandle _tmp51=_new_region("r");
struct _RegionHandle*r=& _tmp51;_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp52=Cyc_Tcenv_set_in_loop(
r,te,_tmp1D);Cyc_Tcstmt_tcStmt(_tmp52,_tmp1E,1);Cyc_Tcexp_tcExp(_tmp52,0,_tmp1C);};
_pop_region(r);}return;_LLF: if(_tmpB <= (void*)1)goto _LL11;if(*((int*)_tmpB)!= 13)
goto _LL11;_tmp1F=((struct Cyc_Absyn_Do_s_struct*)_tmpB)->f1;_tmp20=((struct Cyc_Absyn_Do_s_struct*)
_tmpB)->f2;_tmp21=_tmp20.f1;_tmp22=_tmp20.f2;_LL10: Cyc_Tcstmt_decorate_stmt(te,
_tmp22);{struct _RegionHandle _tmp53=_new_region("r");struct _RegionHandle*r=&
_tmp53;_push_region(r);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(r,te,_tmp22),
_tmp1F,1);;_pop_region(r);}{const char*_tmp156;Cyc_Tcexp_tcTest(te,_tmp21,((
_tmp156="do loop",_tag_dynforward(_tmp156,sizeof(char),_get_zero_arr_size_char(
_tmp156,8)))));}return;_LL11: if(_tmpB <= (void*)1)goto _LL13;if(*((int*)_tmpB)!= 5)
goto _LL13;_tmp23=((struct Cyc_Absyn_Break_s_struct*)_tmpB)->f1;_tmp24=(struct Cyc_Absyn_Stmt**)&((
struct Cyc_Absyn_Break_s_struct*)_tmpB)->f1;_LL12: Cyc_Tcenv_process_break(te,s0,
_tmp24);return;_LL13: if(_tmpB <= (void*)1)goto _LL15;if(*((int*)_tmpB)!= 6)goto
_LL15;_tmp25=((struct Cyc_Absyn_Continue_s_struct*)_tmpB)->f1;_tmp26=(struct Cyc_Absyn_Stmt**)&((
struct Cyc_Absyn_Continue_s_struct*)_tmpB)->f1;_LL14: Cyc_Tcenv_process_continue(
te,s0,_tmp26);return;_LL15: if(_tmpB <= (void*)1)goto _LL17;if(*((int*)_tmpB)!= 7)
goto _LL17;_tmp27=((struct Cyc_Absyn_Goto_s_struct*)_tmpB)->f1;_tmp28=((struct Cyc_Absyn_Goto_s_struct*)
_tmpB)->f2;_tmp29=(struct Cyc_Absyn_Stmt**)&((struct Cyc_Absyn_Goto_s_struct*)
_tmpB)->f2;_LL16: Cyc_Tcenv_process_goto(te,s0,_tmp27,_tmp29);return;_LL17: if(
_tmpB <= (void*)1)goto _LL19;if(*((int*)_tmpB)!= 10)goto _LL19;_tmp2A=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmpB)->f1;_tmp2B=((struct Cyc_Absyn_Fallthru_s_struct*)_tmpB)->f2;_tmp2C=(struct
Cyc_Absyn_Switch_clause***)&((struct Cyc_Absyn_Fallthru_s_struct*)_tmpB)->f2;
_LL18: {struct _tuple4*_tmp55=Cyc_Tcenv_process_fallthru(te,s0,_tmp2C);if(_tmp55
== 0){{const char*_tmp159;void*_tmp158;(_tmp158=0,Cyc_Tcutil_terr(s0->loc,((
_tmp159="fallthru not in a non-last case",_tag_dynforward(_tmp159,sizeof(char),
_get_zero_arr_size_char(_tmp159,32)))),_tag_dynforward(_tmp158,sizeof(void*),0)));}
return;}{struct Cyc_List_List*_tmp58=(*_tmp55).f2;struct Cyc_Tcenv_CList*_tmp59=(*
_tmp55).f3;struct Cyc_List_List*instantiation=((struct Cyc_List_List*(*)(struct
_tuple5*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_make_inst_var,Cyc_Tcenv_lookup_type_vars(
te),_tmp58);struct Cyc_List_List*_tmp5A=((struct Cyc_List_List*(*)(struct
_RegionHandle*r,void*(*f)(struct Cyc_List_List*,void*),struct Cyc_List_List*env,
struct Cyc_Tcenv_CList*x))Cyc_Tcstmt_cmap_c)(Cyc_Core_heap_region,Cyc_Tcutil_substitute,
instantiation,_tmp59);for(0;_tmp5A != 0  && _tmp2A != 0;(_tmp5A=_tmp5A->tl,_tmp2A=
_tmp2A->tl)){Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp2A->hd);if(!Cyc_Tcutil_coerce_arg(
te,(struct Cyc_Absyn_Exp*)_tmp2A->hd,(void*)_tmp5A->hd)){{const char*_tmp15E;void*
_tmp15D[2];struct Cyc_String_pa_struct _tmp15C;struct Cyc_String_pa_struct _tmp15B;(
_tmp15B.tag=0,((_tmp15B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)_tmp5A->hd)),((_tmp15C.tag=0,((_tmp15C.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(((
struct Cyc_Absyn_Exp*)_tmp2A->hd)->topt))->v)),((_tmp15D[0]=& _tmp15C,((_tmp15D[1]=&
_tmp15B,Cyc_Tcutil_terr(s0->loc,((_tmp15E="fallthru argument has type %s but pattern variable has type %s",
_tag_dynforward(_tmp15E,sizeof(char),_get_zero_arr_size_char(_tmp15E,63)))),
_tag_dynforward(_tmp15D,sizeof(void*),2)))))))))))));}Cyc_Tcutil_explain_failure();}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_tmp5A->hd) && !Cyc_Tcutil_is_noalias_path((
struct Cyc_Absyn_Exp*)_tmp2A->hd)){const char*_tmp161;void*_tmp160;(_tmp160=0,Cyc_Tcutil_terr(((
struct Cyc_Absyn_Exp*)_tmp2A->hd)->loc,((_tmp161="Cannot consume paths; do swap instead",
_tag_dynforward(_tmp161,sizeof(char),_get_zero_arr_size_char(_tmp161,38)))),
_tag_dynforward(_tmp160,sizeof(void*),0)));}}if(_tmp2A != 0){const char*_tmp164;
void*_tmp163;(_tmp163=0,Cyc_Tcutil_terr(s0->loc,((_tmp164="too many arguments to explicit fallthru",
_tag_dynforward(_tmp164,sizeof(char),_get_zero_arr_size_char(_tmp164,40)))),
_tag_dynforward(_tmp163,sizeof(void*),0)));}if(_tmp5A != 0){const char*_tmp167;
void*_tmp166;(_tmp166=0,Cyc_Tcutil_terr(s0->loc,((_tmp167="too few arguments to explicit fallthru",
_tag_dynforward(_tmp167,sizeof(char),_get_zero_arr_size_char(_tmp167,39)))),
_tag_dynforward(_tmp166,sizeof(void*),0)));}return;}}_LL19: if(_tmpB <= (void*)1)
goto _LL1B;if(*((int*)_tmpB)!= 12)goto _LL1B;_tmp2D=((struct Cyc_Absyn_Label_s_struct*)
_tmpB)->f1;_tmp2E=((struct Cyc_Absyn_Label_s_struct*)_tmpB)->f2;_LL1A:{struct
_RegionHandle _tmp65=_new_region("r");struct _RegionHandle*r=& _tmp65;_push_region(
r);{struct _dynforward_ptr*_tmp174;const char*_tmp173;void*_tmp172[1];struct Cyc_String_pa_struct
_tmp171;struct Cyc_Absyn_Tvar*_tmp170;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_new_named_block(
r,s0->loc,Cyc_Tcenv_add_label(te,_tmp2D,_tmp2E),((_tmp170=_cycalloc(sizeof(*
_tmp170)),((_tmp170->name=((_tmp174=_cycalloc(sizeof(*_tmp174)),((_tmp174[0]=(
struct _dynforward_ptr)((_tmp171.tag=0,((_tmp171.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*_tmp2D),((_tmp172[0]=& _tmp171,Cyc_aprintf(((_tmp173="`%s",
_tag_dynforward(_tmp173,sizeof(char),_get_zero_arr_size_char(_tmp173,4)))),
_tag_dynforward(_tmp172,sizeof(void*),1)))))))),_tmp174)))),((_tmp170->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp170->kind=(void*)Cyc_Tcutil_kind_to_bound((void*)
3),_tmp170))))))))),_tmp2E,0);};_pop_region(r);}return;_LL1B: if(_tmpB <= (void*)1)
goto _LL1D;if(*((int*)_tmpB)!= 9)goto _LL1D;_tmp2F=((struct Cyc_Absyn_Switch_s_struct*)
_tmpB)->f1;_tmp30=((struct Cyc_Absyn_Switch_s_struct*)_tmpB)->f2;_LL1C: Cyc_Tcexp_tcExp(
te,0,_tmp2F);{void*_tmp6B=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp2F->topt))->v;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp6B) && !Cyc_Tcutil_is_noalias_path(
_tmp2F)){const char*_tmp177;void*_tmp176;(_tmp176=0,Cyc_Tcutil_terr(_tmp2F->loc,((
_tmp177="Cannot consume paths; do swap instead",_tag_dynforward(_tmp177,sizeof(
char),_get_zero_arr_size_char(_tmp177,38)))),_tag_dynforward(_tmp176,sizeof(void*),
0)));}{struct _RegionHandle _tmp6E=_new_region("r");struct _RegionHandle*r=& _tmp6E;
_push_region(r);{struct Cyc_Tcenv_Tenv*_tmp6F=Cyc_Tcenv_set_in_switch(r,te);
_tmp6F=Cyc_Tcenv_clear_fallthru(r,_tmp6F);{struct Cyc_List_List*scs=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp30);for(0;scs != 0;scs=scs->tl){struct
Cyc_Absyn_Pat*_tmp70=((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;struct
Cyc_Tcpat_TcPatResult _tmp71=Cyc_Tcpat_tcPat(_tmp6F,_tmp70,& _tmp6B);struct Cyc_List_List*
_tmp72=_tmp71.tvars_and_bounds_opt == 0?0:(*_tmp71.tvars_and_bounds_opt).f1;if(!
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmp70->topt))->v,
_tmp6B)){{const char*_tmp17C;void*_tmp17B[2];struct Cyc_String_pa_struct _tmp17A;
struct Cyc_String_pa_struct _tmp179;(_tmp179.tag=0,((_tmp179.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp70->topt))->v)),((_tmp17A.tag=0,((_tmp17A.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(_tmp6B)),((
_tmp17B[0]=& _tmp17A,((_tmp17B[1]=& _tmp179,Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)
scs->hd)->loc,((_tmp17C="switch on type %s, but case expects type %s",
_tag_dynforward(_tmp17C,sizeof(char),_get_zero_arr_size_char(_tmp17C,44)))),
_tag_dynforward(_tmp17B,sizeof(void*),2)))))))))))));}Cyc_Tcutil_explain_failure();}
else{Cyc_Tcpat_check_pat_regions(_tmp6F,_tmp70);}{struct Cyc_Core_Opt*_tmp17D;((
struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=((_tmp17D=_cycalloc(sizeof(*
_tmp17D)),((_tmp17D->v=_tmp71.patvars,_tmp17D))));}Cyc_Tcstmt_pattern_synth(((
struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,_tmp6F,_tmp71,((struct Cyc_Absyn_Switch_clause*)
scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);if(
_tmp71.tvars_and_bounds_opt != 0  && (*_tmp71.tvars_and_bounds_opt).f2 != 0)_tmp6F=
Cyc_Tcenv_clear_fallthru(r,_tmp6F);else{_tmp6F=Cyc_Tcenv_set_fallthru(r,_tmp6F,
_tmp72,_tmp71.patvars,(struct Cyc_Absyn_Switch_clause*)scs->hd);}}}};_pop_region(
r);}Cyc_Tcpat_check_switch_exhaustive(s0->loc,_tmp30);return;}_LL1D: if(_tmpB <= (
void*)1)goto _LL1F;if(*((int*)_tmpB)!= 14)goto _LL1F;_tmp31=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmpB)->f1;_tmp32=((struct Cyc_Absyn_TryCatch_s_struct*)_tmpB)->f2;_LL1E:{struct
Cyc_Absyn_Seq_s_struct _tmp180;struct Cyc_Absyn_Seq_s_struct*_tmp17F;(void*)(
_tmp31->r=(void*)((void*)(Cyc_Absyn_new_stmt((void*)((_tmp17F=_cycalloc(sizeof(*
_tmp17F)),((_tmp17F[0]=((_tmp180.tag=1,((_tmp180.f1=Cyc_Absyn_new_stmt((void*)
_tmp31->r,_tmp31->loc),((_tmp180.f2=Cyc_Absyn_skip_stmt(_tmp31->loc),_tmp180)))))),
_tmp17F)))),_tmp31->loc))->r));}{struct _RegionHandle _tmp7A=_new_region("r");
struct _RegionHandle*r=& _tmp7A;_push_region(r);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
r,Cyc_Tcenv_enter_try(r,te),s0),_tmp31,1);;_pop_region(r);}{struct _RegionHandle
_tmp7B=_new_region("r2");struct _RegionHandle*r2=& _tmp7B;_push_region(r2);{struct
Cyc_Tcenv_Tenv*_tmp7C=Cyc_Tcenv_set_in_switch(r2,te);_tmp7C=Cyc_Tcenv_clear_fallthru(
r2,_tmp7C);{struct Cyc_List_List*_tmp7D=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(_tmp32);for(0;_tmp7D != 0;_tmp7D=_tmp7D->tl){struct Cyc_Absyn_Pat*
_tmp7E=((struct Cyc_Absyn_Switch_clause*)_tmp7D->hd)->pattern;struct Cyc_Tcpat_TcPatResult
_tmp7F=Cyc_Tcpat_tcPat(_tmp7C,_tmp7E,& Cyc_Absyn_exn_typ);struct Cyc_List_List*
_tmp80=_tmp7F.tvars_and_bounds_opt == 0?0:(*_tmp7F.tvars_and_bounds_opt).f1;if(!
Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)_check_null(_tmp7E->topt))->v,Cyc_Absyn_exn_typ)){
const char*_tmp184;void*_tmp183[1];struct Cyc_String_pa_struct _tmp182;(_tmp182.tag=
0,((_tmp182.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp7E->topt))->v)),((_tmp183[0]=&
_tmp182,Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)_tmp7D->hd)->loc,((
_tmp184="expected xtunion exn but found %s",_tag_dynforward(_tmp184,sizeof(char),
_get_zero_arr_size_char(_tmp184,34)))),_tag_dynforward(_tmp183,sizeof(void*),1)))))));}
else{Cyc_Tcpat_check_pat_regions(_tmp7C,_tmp7E);}{struct Cyc_Core_Opt*_tmp185;((
struct Cyc_Absyn_Switch_clause*)_tmp7D->hd)->pat_vars=((_tmp185=_cycalloc(sizeof(*
_tmp185)),((_tmp185->v=_tmp7F.patvars,_tmp185))));}Cyc_Tcstmt_pattern_synth(((
struct Cyc_Absyn_Switch_clause*)_tmp7D->hd)->loc,_tmp7C,_tmp7F,((struct Cyc_Absyn_Switch_clause*)
_tmp7D->hd)->body,((struct Cyc_Absyn_Switch_clause*)_tmp7D->hd)->where_clause,1);
if(_tmp7F.tvars_and_bounds_opt != 0  && (*_tmp7F.tvars_and_bounds_opt).f2 != 0)
_tmp7C=Cyc_Tcenv_clear_fallthru(r2,_tmp7C);else{_tmp7C=Cyc_Tcenv_set_fallthru(r2,
_tmp7C,_tmp80,_tmp7F.patvars,(struct Cyc_Absyn_Switch_clause*)_tmp7D->hd);}}}};
_pop_region(r2);}Cyc_Tcpat_check_catch_overlap(s0->loc,_tmp32);return;_LL1F: if(
_tmpB <= (void*)1)goto _LL21;if(*((int*)_tmpB)!= 11)goto _LL21;_tmp33=((struct Cyc_Absyn_Decl_s_struct*)
_tmpB)->f1;_tmp34=((struct Cyc_Absyn_Decl_s_struct*)_tmpB)->f2;_LL20: {struct
_dynforward_ptr unimp_msg_part;{struct _RegionHandle _tmp85=_new_region("decl_rgn");
struct _RegionHandle*decl_rgn=& _tmp85;_push_region(decl_rgn);{struct Cyc_Tcenv_Tenv*
te2;if(new_block)te2=Cyc_Tcenv_new_block(decl_rgn,s0->loc,te);else{te2=Cyc_Tcenv_copy_tenv(
decl_rgn,te);}{struct Cyc_Tcenv_Tenv*_tmp86=te2;void*_tmp87=(void*)_tmp33->r;
struct Cyc_Absyn_Vardecl*_tmp88;struct Cyc_Absyn_Pat*_tmp89;struct Cyc_Core_Opt*
_tmp8A;struct Cyc_Core_Opt**_tmp8B;struct Cyc_Absyn_Exp*_tmp8C;struct Cyc_List_List*
_tmp8D;struct Cyc_Absyn_Fndecl*_tmp8E;struct _dynforward_ptr*_tmp8F;struct Cyc_List_List*
_tmp90;struct _tuple0*_tmp91;struct Cyc_List_List*_tmp92;_LL2D: if(_tmp87 <= (void*)
2)goto _LL45;if(*((int*)_tmp87)!= 0)goto _LL2F;_tmp88=((struct Cyc_Absyn_Var_d_struct*)
_tmp87)->f1;_LL2E: {struct Cyc_Absyn_Vardecl _tmp94;void*_tmp95;struct _tuple0*
_tmp96;struct _tuple0 _tmp97;union Cyc_Absyn_Nmspace_union _tmp98;struct
_dynforward_ptr*_tmp99;struct Cyc_Absyn_Tqual _tmp9A;void*_tmp9B;struct Cyc_Absyn_Exp*
_tmp9C;struct Cyc_Core_Opt*_tmp9D;struct Cyc_Core_Opt**_tmp9E;struct Cyc_List_List*
_tmp9F;struct Cyc_Absyn_Vardecl*_tmp93=_tmp88;_tmp94=*_tmp93;_tmp95=(void*)_tmp94.sc;
_tmp96=_tmp94.name;_tmp97=*_tmp96;_tmp98=_tmp97.f1;_tmp99=_tmp97.f2;_tmp9A=
_tmp94.tq;_tmp9B=(void*)_tmp94.type;_tmp9C=_tmp94.initializer;_tmp9D=_tmp94.rgn;
_tmp9E=(struct Cyc_Core_Opt**)&(*_tmp93).rgn;_tmp9F=_tmp94.attributes;{void*
_tmpA0=Cyc_Tcenv_curr_rgn(_tmp86);int is_local;{void*_tmpA1=_tmp95;_LL4A: if((int)
_tmpA1 != 0)goto _LL4C;_LL4B: goto _LL4D;_LL4C: if((int)_tmpA1 != 3)goto _LL4E;_LL4D:
goto _LL4F;_LL4E: if((int)_tmpA1 != 4)goto _LL50;_LL4F: is_local=0;goto _LL49;_LL50:
if((int)_tmpA1 != 1)goto _LL52;_LL51:{const char*_tmp188;void*_tmp187;(_tmp187=0,
Cyc_Tcutil_terr(_tmp33->loc,((_tmp188="bad abstract scope for local variable",
_tag_dynforward(_tmp188,sizeof(char),_get_zero_arr_size_char(_tmp188,38)))),
_tag_dynforward(_tmp187,sizeof(void*),0)));}goto _LL53;_LL52: if((int)_tmpA1 != 5)
goto _LL54;_LL53: goto _LL55;_LL54: if((int)_tmpA1 != 2)goto _LL49;_LL55: is_local=1;
goto _LL49;_LL49:;}{struct Cyc_Core_Opt*_tmp18A;struct Cyc_Core_Opt*_tmp189;*_tmp9E=
is_local?(_tmp18A=_cycalloc(sizeof(*_tmp18A)),((_tmp18A->v=(void*)_tmpA0,_tmp18A))):((
_tmp189=_cycalloc(sizeof(*_tmp189)),((_tmp189->v=(void*)((void*)2),_tmp189))));}{
union Cyc_Absyn_Nmspace_union _tmpA6=_tmp98;struct Cyc_List_List*_tmpA7;struct Cyc_List_List*
_tmpA8;_LL57: if((_tmpA6.Loc_n).tag != 0)goto _LL59;_LL58: goto _LL56;_LL59: if((
_tmpA6.Rel_n).tag != 1)goto _LL5B;_tmpA7=(_tmpA6.Rel_n).f1;if(_tmpA7 != 0)goto _LL5B;
_LL5A:{union Cyc_Absyn_Nmspace_union _tmp18B;(*_tmp88->name).f1=(union Cyc_Absyn_Nmspace_union)(((
_tmp18B.Loc_n).tag=0,_tmp18B));}goto _LL56;_LL5B: if((_tmpA6.Abs_n).tag != 2)goto
_LL5D;_tmpA8=(_tmpA6.Abs_n).f1;_LL5C: {const char*_tmp18E;void*_tmp18D;(int)
_throw(((_tmp18D=0,Cyc_Tcutil_impos(((_tmp18E="tcstmt: Abs_n declaration",
_tag_dynforward(_tmp18E,sizeof(char),_get_zero_arr_size_char(_tmp18E,26)))),
_tag_dynforward(_tmp18D,sizeof(void*),0)))));}_LL5D:;_LL5E:{const char*_tmp191;
void*_tmp190;(_tmp190=0,Cyc_Tcutil_terr(_tmp33->loc,((_tmp191="cannot declare a qualified local variable",
_tag_dynforward(_tmp191,sizeof(char),_get_zero_arr_size_char(_tmp191,42)))),
_tag_dynforward(_tmp190,sizeof(void*),0)));}goto _LL56;_LL56:;}{void*_tmpAE=Cyc_Tcutil_compress(
_tmp9B);struct Cyc_Absyn_ArrayInfo _tmpAF;void*_tmpB0;struct Cyc_Absyn_Tqual _tmpB1;
struct Cyc_Absyn_Exp*_tmpB2;struct Cyc_Absyn_Conref*_tmpB3;struct Cyc_Position_Segment*
_tmpB4;_LL60: if(_tmpAE <= (void*)4)goto _LL62;if(*((int*)_tmpAE)!= 7)goto _LL62;
_tmpAF=((struct Cyc_Absyn_ArrayType_struct*)_tmpAE)->f1;_tmpB0=(void*)_tmpAF.elt_type;
_tmpB1=_tmpAF.tq;_tmpB2=_tmpAF.num_elts;if(_tmpB2 != 0)goto _LL62;_tmpB3=_tmpAF.zero_term;
_tmpB4=_tmpAF.zt_loc;if(!(_tmp88->initializer != 0))goto _LL62;_LL61:{void*_tmpB5=(
void*)((struct Cyc_Absyn_Exp*)_check_null(_tmp88->initializer))->r;union Cyc_Absyn_Cnst_union
_tmpB6;struct _dynforward_ptr _tmpB7;struct Cyc_Absyn_Exp*_tmpB8;struct Cyc_List_List*
_tmpB9;struct Cyc_List_List*_tmpBA;_LL65: if(*((int*)_tmpB5)!= 0)goto _LL67;_tmpB6=((
struct Cyc_Absyn_Const_e_struct*)_tmpB5)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmpB5)->f1).String_c).tag != 5)goto _LL67;_tmpB7=(_tmpB6.String_c).f1;_LL66:
_tmp9B=(void*)(_tmp88->type=(void*)Cyc_Absyn_array_typ(_tmpB0,_tmpB1,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_uint_exp(_get_dynforward_size(_tmpB7,sizeof(char)),0),_tmpB3,_tmpB4));
goto _LL64;_LL67: if(*((int*)_tmpB5)!= 29)goto _LL69;_tmpB8=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpB5)->f2;_LL68: _tmp9B=(void*)(_tmp88->type=(void*)Cyc_Absyn_array_typ(_tmpB0,
_tmpB1,(struct Cyc_Absyn_Exp*)_tmpB8,_tmpB3,_tmpB4));goto _LL64;_LL69: if(*((int*)
_tmpB5)!= 37)goto _LL6B;_tmpB9=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpB5)->f2;
_LL6A: _tmpBA=_tmpB9;goto _LL6C;_LL6B: if(*((int*)_tmpB5)!= 28)goto _LL6D;_tmpBA=((
struct Cyc_Absyn_Array_e_struct*)_tmpB5)->f1;_LL6C: _tmp9B=(void*)(_tmp88->type=(
void*)Cyc_Absyn_array_typ(_tmpB0,_tmpB1,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp((
unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpBA),0),_tmpB3,
_tmpB4));goto _LL64;_LL6D:;_LL6E: goto _LL64;_LL64:;}goto _LL5F;_LL62:;_LL63: goto
_LL5F;_LL5F:;}{struct Cyc_List_List*_tmpBB=!is_local?0: Cyc_Tcenv_lookup_type_vars(
_tmp86);int _tmpBC=!is_local?0: 1;Cyc_Tcutil_check_type(s0->loc,_tmp86,_tmpBB,(
void*)1,_tmpBC,_tmp9B);{struct Cyc_Tcenv_Tenv*_tmpBD=Cyc_Tcenv_add_local_var(
decl_rgn,_tmp34->loc,_tmp86,_tmp88);if(_tmp95 == (void*)3  || _tmp95 == (void*)4){
const char*_tmp194;void*_tmp193;(_tmp193=0,Cyc_Tcutil_terr(_tmp33->loc,((_tmp194="local extern declarations not yet supported",
_tag_dynforward(_tmp194,sizeof(char),_get_zero_arr_size_char(_tmp194,44)))),
_tag_dynforward(_tmp193,sizeof(void*),0)));}if(_tmp9C != 0){Cyc_Tcexp_tcExpInitializer(
_tmpBD,(void**)& _tmp9B,(struct Cyc_Absyn_Exp*)_tmp9C);if(!is_local  && !Cyc_Tcutil_is_const_exp(
_tmp86,(struct Cyc_Absyn_Exp*)_tmp9C)){const char*_tmp197;void*_tmp196;(_tmp196=0,
Cyc_Tcutil_terr(_tmp33->loc,((_tmp197="initializer needs to be a constant expression",
_tag_dynforward(_tmp197,sizeof(char),_get_zero_arr_size_char(_tmp197,46)))),
_tag_dynforward(_tmp196,sizeof(void*),0)));}if(!Cyc_Tcutil_coerce_assign(_tmpBD,(
struct Cyc_Absyn_Exp*)_tmp9C,_tmp9B)){struct _dynforward_ptr _tmpC2=*_tmp99;struct
_dynforward_ptr _tmpC3=Cyc_Absynpp_typ2string(_tmp9B);struct _dynforward_ptr _tmpC4=
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(_tmp9C->topt))->v);
if(((_get_dynforward_size(_tmpC2,sizeof(char))+ _get_dynforward_size(_tmpC3,
sizeof(char)))+ _get_dynforward_size(_tmpC4,sizeof(char)))+ 50 < 80){const char*
_tmp19D;void*_tmp19C[3];struct Cyc_String_pa_struct _tmp19B;struct Cyc_String_pa_struct
_tmp19A;struct Cyc_String_pa_struct _tmp199;(_tmp199.tag=0,((_tmp199.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmpC4),((_tmp19A.tag=0,((_tmp19A.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmpC3),((_tmp19B.tag=0,((_tmp19B.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmpC2),((_tmp19C[0]=& _tmp19B,((
_tmp19C[1]=& _tmp19A,((_tmp19C[2]=& _tmp199,Cyc_Tcutil_terr(_tmp33->loc,((_tmp19D="%s declared with type %s but initialized with type %s.",
_tag_dynforward(_tmp19D,sizeof(char),_get_zero_arr_size_char(_tmp19D,55)))),
_tag_dynforward(_tmp19C,sizeof(void*),3)))))))))))))))))));}else{if((
_get_dynforward_size(_tmpC2,sizeof(char))+ _get_dynforward_size(_tmpC3,sizeof(
char)))+ 25 < 80  && _get_dynforward_size(_tmpC4,sizeof(char))+ 25 < 80){const char*
_tmp1A3;void*_tmp1A2[3];struct Cyc_String_pa_struct _tmp1A1;struct Cyc_String_pa_struct
_tmp1A0;struct Cyc_String_pa_struct _tmp19F;(_tmp19F.tag=0,((_tmp19F.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmpC4),((_tmp1A0.tag=0,((_tmp1A0.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmpC3),((_tmp1A1.tag=0,((_tmp1A1.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmpC2),((_tmp1A2[0]=& _tmp1A1,((
_tmp1A2[1]=& _tmp1A0,((_tmp1A2[2]=& _tmp19F,Cyc_Tcutil_terr(_tmp33->loc,((_tmp1A3="%s declared with type %s\n but initialized with type %s.",
_tag_dynforward(_tmp1A3,sizeof(char),_get_zero_arr_size_char(_tmp1A3,56)))),
_tag_dynforward(_tmp1A2,sizeof(void*),3)))))))))))))))))));}else{const char*
_tmp1A9;void*_tmp1A8[3];struct Cyc_String_pa_struct _tmp1A7;struct Cyc_String_pa_struct
_tmp1A6;struct Cyc_String_pa_struct _tmp1A5;(_tmp1A5.tag=0,((_tmp1A5.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmpC4),((_tmp1A6.tag=0,((_tmp1A6.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmpC3),((_tmp1A7.tag=0,((_tmp1A7.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)_tmpC2),((_tmp1A8[0]=& _tmp1A7,((
_tmp1A8[1]=& _tmp1A6,((_tmp1A8[2]=& _tmp1A5,Cyc_Tcutil_terr(_tmp33->loc,((_tmp1A9="%s declared with type \n%s\n but initialized with type \n%s.",
_tag_dynforward(_tmp1A9,sizeof(char),_get_zero_arr_size_char(_tmp1A9,58)))),
_tag_dynforward(_tmp1A8,sizeof(void*),3)))))))))))))))))));}}Cyc_Tcutil_unify(
_tmp9B,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp9C->topt))->v);Cyc_Tcutil_explain_failure();}}
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,_tmpBD,s0),_tmp34,0);
_npop_handler(0);return;}}}}_LL2F: if(*((int*)_tmp87)!= 2)goto _LL31;_tmp89=((
struct Cyc_Absyn_Let_d_struct*)_tmp87)->f1;_tmp8A=((struct Cyc_Absyn_Let_d_struct*)
_tmp87)->f2;_tmp8B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Let_d_struct*)_tmp87)->f2;
_tmp8C=((struct Cyc_Absyn_Let_d_struct*)_tmp87)->f3;_LL30: Cyc_Tcexp_tcExpInitializer(
_tmp86,0,_tmp8C);{struct Cyc_Tcpat_TcPatResult _tmpD4=Cyc_Tcpat_tcPat(_tmp86,
_tmp89,(void**)((void*)&((struct Cyc_Core_Opt*)_check_null(_tmp8C->topt))->v));{
struct Cyc_Core_Opt*_tmp1AA;*_tmp8B=((_tmp1AA=_cycalloc(sizeof(*_tmp1AA)),((
_tmp1AA->v=_tmpD4.patvars,_tmp1AA))));}if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp89->topt))->v,(void*)((struct Cyc_Core_Opt*)_check_null(_tmp8C->topt))->v)
 && !Cyc_Tcutil_coerce_assign(_tmp86,_tmp8C,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp89->topt))->v)){{const char*_tmp1AF;void*_tmp1AE[2];struct Cyc_String_pa_struct
_tmp1AD;struct Cyc_String_pa_struct _tmp1AC;(_tmp1AC.tag=0,((_tmp1AC.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct
Cyc_Core_Opt*)_check_null(_tmp8C->topt))->v)),((_tmp1AD.tag=0,((_tmp1AD.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp89->topt))->v)),((_tmp1AE[0]=& _tmp1AD,((
_tmp1AE[1]=& _tmp1AC,Cyc_Tcutil_terr(_tmp33->loc,((_tmp1AF="pattern type %s does not match definition type %s",
_tag_dynforward(_tmp1AF,sizeof(char),_get_zero_arr_size_char(_tmp1AF,50)))),
_tag_dynforward(_tmp1AE,sizeof(void*),2)))))))))))));}Cyc_Tcutil_unify((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp89->topt))->v,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp8C->topt))->v);Cyc_Tcutil_explain_failure();}else{Cyc_Tcpat_check_pat_regions(
_tmp86,_tmp89);}Cyc_Tcpat_check_let_pat_exhaustive(_tmp89->loc,_tmp89);Cyc_Tcstmt_pattern_synth(
s0->loc,_tmp86,_tmpD4,_tmp34,0,0);_npop_handler(0);return;}_LL31: if(*((int*)
_tmp87)!= 3)goto _LL33;_tmp8D=((struct Cyc_Absyn_Letv_d_struct*)_tmp87)->f1;_LL32: {
void*_tmpDA=Cyc_Tcenv_curr_rgn(_tmp86);struct Cyc_Tcenv_Tenv*_tmpDB=_tmp86;for(0;
_tmp8D != 0;_tmp8D=_tmp8D->tl){struct Cyc_Absyn_Vardecl*_tmpDC=(struct Cyc_Absyn_Vardecl*)
_tmp8D->hd;struct Cyc_Absyn_Vardecl _tmpDE;struct _tuple0*_tmpDF;struct _tuple0
_tmpE0;union Cyc_Absyn_Nmspace_union _tmpE1;void*_tmpE2;struct Cyc_Core_Opt*_tmpE3;
struct Cyc_Core_Opt**_tmpE4;struct Cyc_Absyn_Vardecl*_tmpDD=_tmpDC;_tmpDE=*_tmpDD;
_tmpDF=_tmpDE.name;_tmpE0=*_tmpDF;_tmpE1=_tmpE0.f1;_tmpE2=(void*)_tmpDE.type;
_tmpE3=_tmpDE.rgn;_tmpE4=(struct Cyc_Core_Opt**)&(*_tmpDD).rgn;{struct Cyc_Core_Opt*
_tmp1B0;*_tmpE4=((_tmp1B0=_cycalloc(sizeof(*_tmp1B0)),((_tmp1B0->v=(void*)_tmpDA,
_tmp1B0))));}{union Cyc_Absyn_Nmspace_union _tmpE6=_tmpE1;struct Cyc_List_List*
_tmpE7;struct Cyc_List_List*_tmpE8;_LL70: if((_tmpE6.Loc_n).tag != 0)goto _LL72;
_LL71: goto _LL6F;_LL72: if((_tmpE6.Rel_n).tag != 1)goto _LL74;_tmpE7=(_tmpE6.Rel_n).f1;
if(_tmpE7 != 0)goto _LL74;_LL73:{union Cyc_Absyn_Nmspace_union _tmp1B1;(*_tmpDC->name).f1=(
union Cyc_Absyn_Nmspace_union)(((_tmp1B1.Loc_n).tag=0,_tmp1B1));}goto _LL6F;_LL74:
if((_tmpE6.Abs_n).tag != 2)goto _LL76;_tmpE8=(_tmpE6.Abs_n).f1;_LL75: {const char*
_tmp1B4;void*_tmp1B3;(int)_throw(((_tmp1B3=0,Cyc_Tcutil_impos(((_tmp1B4="tcstmt: Abs_n declaration",
_tag_dynforward(_tmp1B4,sizeof(char),_get_zero_arr_size_char(_tmp1B4,26)))),
_tag_dynforward(_tmp1B3,sizeof(void*),0)))));}_LL76:;_LL77:{const char*_tmp1B7;
void*_tmp1B6;(_tmp1B6=0,Cyc_Tcutil_terr(_tmp33->loc,((_tmp1B7="cannot declare a qualified local variable",
_tag_dynforward(_tmp1B7,sizeof(char),_get_zero_arr_size_char(_tmp1B7,42)))),
_tag_dynforward(_tmp1B6,sizeof(void*),0)));}goto _LL6F;_LL6F:;}Cyc_Tcutil_check_type(
s0->loc,_tmpDB,Cyc_Tcenv_lookup_type_vars(_tmpDB),(void*)1,1,_tmpE2);_tmpDB=Cyc_Tcenv_add_local_var(
decl_rgn,_tmp34->loc,_tmpDB,_tmpDC);}Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(
decl_rgn,_tmpDB,s0),_tmp34,0);_npop_handler(0);return;}_LL33: if(*((int*)_tmp87)
!= 1)goto _LL35;_tmp8E=((struct Cyc_Absyn_Fn_d_struct*)_tmp87)->f1;_LL34: {void*
_tmpEE=Cyc_Tcenv_curr_rgn(_tmp86);if((void*)_tmp8E->sc != (void*)2){const char*
_tmp1BA;void*_tmp1B9;(_tmp1B9=0,Cyc_Tcutil_terr(_tmp33->loc,((_tmp1BA="bad storage class for inner function",
_tag_dynforward(_tmp1BA,sizeof(char),_get_zero_arr_size_char(_tmp1BA,37)))),
_tag_dynforward(_tmp1B9,sizeof(void*),0)));}{union Cyc_Absyn_Nmspace_union _tmpF1=(*
_tmp8E->name).f1;struct Cyc_List_List*_tmpF2;struct Cyc_List_List*_tmpF3;_LL79: if((
_tmpF1.Rel_n).tag != 1)goto _LL7B;_tmpF2=(_tmpF1.Rel_n).f1;if(_tmpF2 != 0)goto _LL7B;
_LL7A: goto _LL78;_LL7B: if((_tmpF1.Abs_n).tag != 2)goto _LL7D;_tmpF3=(_tmpF1.Abs_n).f1;
_LL7C: {const char*_tmp1BD;void*_tmp1BC;(_tmp1BC=0,((int(*)(struct _dynforward_ptr
fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp1BD="tc: Abs_n in tcStmt var decl",
_tag_dynforward(_tmp1BD,sizeof(char),_get_zero_arr_size_char(_tmp1BD,29)))),
_tag_dynforward(_tmp1BC,sizeof(void*),0)));}_LL7D:;_LL7E: {const char*_tmp1C0;
void*_tmp1BF;(_tmp1BF=0,Cyc_Tcutil_terr(_tmp33->loc,((_tmp1C0="explicit namespace not allowed on inner function declaration",
_tag_dynforward(_tmp1C0,sizeof(char),_get_zero_arr_size_char(_tmp1C0,61)))),
_tag_dynforward(_tmp1BF,sizeof(void*),0)));}_LL78:;}{union Cyc_Absyn_Nmspace_union
_tmp1C1;(*_tmp8E->name).f1=(union Cyc_Absyn_Nmspace_union)(((_tmp1C1.Loc_n).tag=0,
_tmp1C1));}Cyc_Tcutil_check_fndecl_valid_type(_tmp33->loc,_tmp86,_tmp8E);{void*t=
Cyc_Tcutil_fndecl2typ(_tmp8E);_tmp8E->attributes=Cyc_Tcutil_transfer_fn_type_atts(
t,_tmp8E->attributes);{struct Cyc_List_List*atts=_tmp8E->attributes;for(0;(
unsigned int)atts;atts=atts->tl){void*_tmpF9=(void*)atts->hd;_LL80: if((int)
_tmpF9 != 5)goto _LL82;_LL81: goto _LL83;_LL82: if(_tmpF9 <= (void*)17)goto _LL84;if(*((
int*)_tmpF9)!= 1)goto _LL84;_LL83:{const char*_tmp1C5;void*_tmp1C4[1];struct Cyc_String_pa_struct
_tmp1C3;(_tmp1C3.tag=0,((_tmp1C3.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp1C4[0]=&
_tmp1C3,Cyc_Tcutil_terr(_tmp33->loc,((_tmp1C5="bad attribute %s for function",
_tag_dynforward(_tmp1C5,sizeof(char),_get_zero_arr_size_char(_tmp1C5,30)))),
_tag_dynforward(_tmp1C4,sizeof(void*),1)))))));}goto _LL7F;_LL84:;_LL85: goto _LL7F;
_LL7F:;}}{struct Cyc_Core_Opt*_tmp1C8;struct Cyc_Absyn_Vardecl*_tmp1C7;struct Cyc_Absyn_Vardecl*
vd=(_tmp1C7=_cycalloc(sizeof(*_tmp1C7)),((_tmp1C7->sc=(void*)((void*)_tmp8E->sc),((
_tmp1C7->name=_tmp8E->name,((_tmp1C7->tq=Cyc_Absyn_const_tqual(0),((_tmp1C7->type=(
void*)Cyc_Absyn_at_typ(t,_tmpEE,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),((
_tmp1C7->initializer=0,((_tmp1C7->rgn=((_tmp1C8=_cycalloc(sizeof(*_tmp1C8)),((
_tmp1C8->v=(void*)_tmpEE,_tmp1C8)))),((_tmp1C7->attributes=0,((_tmp1C7->escapes=
0,_tmp1C7)))))))))))))))));_tmp8E->fn_vardecl=(struct Cyc_Absyn_Vardecl*)vd;
_tmp86=Cyc_Tcenv_add_local_var(decl_rgn,_tmp33->loc,_tmp86,vd);{struct Cyc_Tcenv_Fenv*
old_fenv=(struct Cyc_Tcenv_Fenv*)_check_null(_tmp86->le);_tmp86->le=(struct Cyc_Tcenv_Fenv*)
Cyc_Tcenv_nested_fenv(_tmp33->loc,old_fenv,_tmp8E);Cyc_Tcstmt_tcStmt(_tmp86,
_tmp8E->body,0);Cyc_Tcenv_check_delayed_effects(_tmp86);Cyc_Tcenv_check_delayed_constraints(
_tmp86);if(!Cyc_Tcenv_all_labels_resolved(_tmp86)){const char*_tmp1CB;void*
_tmp1CA;(_tmp1CA=0,Cyc_Tcutil_terr(_tmp33->loc,((_tmp1CB="function has goto statements to undefined labels",
_tag_dynforward(_tmp1CB,sizeof(char),_get_zero_arr_size_char(_tmp1CB,49)))),
_tag_dynforward(_tmp1CA,sizeof(void*),0)));}_tmp86->le=(struct Cyc_Tcenv_Fenv*)
old_fenv;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,_tmp86,s0),_tmp34,0);
_npop_handler(0);return;}}}}_LL35: if(*((int*)_tmp87)!= 8)goto _LL37;_tmp8F=((
struct Cyc_Absyn_Namespace_d_struct*)_tmp87)->f1;_tmp90=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp87)->f2;_LL36:{const char*_tmp1CC;unimp_msg_part=((_tmp1CC="namespace",
_tag_dynforward(_tmp1CC,sizeof(char),_get_zero_arr_size_char(_tmp1CC,10))));}
goto _LL2C;_LL37: if(*((int*)_tmp87)!= 9)goto _LL39;_tmp91=((struct Cyc_Absyn_Using_d_struct*)
_tmp87)->f1;_tmp92=((struct Cyc_Absyn_Using_d_struct*)_tmp87)->f2;_LL38:{const
char*_tmp1CD;unimp_msg_part=((_tmp1CD="using",_tag_dynforward(_tmp1CD,sizeof(
char),_get_zero_arr_size_char(_tmp1CD,6))));}goto _LL2C;_LL39: if(*((int*)_tmp87)
!= 4)goto _LL3B;_LL3A:{const char*_tmp1CE;unimp_msg_part=((_tmp1CE="type",
_tag_dynforward(_tmp1CE,sizeof(char),_get_zero_arr_size_char(_tmp1CE,5))));}goto
_LL2C;_LL3B: if(*((int*)_tmp87)!= 5)goto _LL3D;_LL3C:{const char*_tmp1CF;
unimp_msg_part=((_tmp1CF="[x]tunion",_tag_dynforward(_tmp1CF,sizeof(char),
_get_zero_arr_size_char(_tmp1CF,10))));}goto _LL2C;_LL3D: if(*((int*)_tmp87)!= 6)
goto _LL3F;_LL3E:{const char*_tmp1D0;unimp_msg_part=((_tmp1D0="enum",
_tag_dynforward(_tmp1D0,sizeof(char),_get_zero_arr_size_char(_tmp1D0,5))));}goto
_LL2C;_LL3F: if(*((int*)_tmp87)!= 7)goto _LL41;_LL40:{const char*_tmp1D1;
unimp_msg_part=((_tmp1D1="typedef",_tag_dynforward(_tmp1D1,sizeof(char),
_get_zero_arr_size_char(_tmp1D1,8))));}goto _LL2C;_LL41: if(*((int*)_tmp87)!= 10)
goto _LL43;_LL42:{const char*_tmp1D2;unimp_msg_part=((_tmp1D2="extern \"C\"",
_tag_dynforward(_tmp1D2,sizeof(char),_get_zero_arr_size_char(_tmp1D2,11))));}
goto _LL2C;_LL43: if(*((int*)_tmp87)!= 11)goto _LL45;_LL44:{const char*_tmp1D3;
unimp_msg_part=((_tmp1D3="extern \"C include\"",_tag_dynforward(_tmp1D3,sizeof(
char),_get_zero_arr_size_char(_tmp1D3,19))));}goto _LL2C;_LL45: if((int)_tmp87 != 0)
goto _LL47;_LL46:{const char*_tmp1D4;unimp_msg_part=((_tmp1D4="__cyclone_port_on__",
_tag_dynforward(_tmp1D4,sizeof(char),_get_zero_arr_size_char(_tmp1D4,20))));}
goto _LL2C;_LL47: if((int)_tmp87 != 1)goto _LL2C;_LL48:{const char*_tmp1D5;
unimp_msg_part=((_tmp1D5="__cyclone_port_off__",_tag_dynforward(_tmp1D5,sizeof(
char),_get_zero_arr_size_char(_tmp1D5,21))));}goto _LL2C;_LL2C:;}};_pop_region(
decl_rgn);}{const char*_tmp1D9;void*_tmp1D8[1];struct Cyc_String_pa_struct _tmp1D7;(
int)_throw(((_tmp1D7.tag=0,((_tmp1D7.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)unimp_msg_part),((_tmp1D8[0]=& _tmp1D7,Cyc_Tcutil_impos(((_tmp1D9="tcStmt: nested %s declarations unimplemented",
_tag_dynforward(_tmp1D9,sizeof(char),_get_zero_arr_size_char(_tmp1D9,45)))),
_tag_dynforward(_tmp1D8,sizeof(void*),1)))))))));}}_LL21: if(_tmpB <= (void*)1)
goto _LL23;if(*((int*)_tmpB)!= 15)goto _LL23;_tmp35=((struct Cyc_Absyn_Region_s_struct*)
_tmpB)->f1;_tmp36=((struct Cyc_Absyn_Region_s_struct*)_tmpB)->f2;_tmp37=((struct
Cyc_Absyn_Region_s_struct*)_tmpB)->f3;_tmp38=((struct Cyc_Absyn_Region_s_struct*)
_tmpB)->f4;_tmp39=((struct Cyc_Absyn_Region_s_struct*)_tmpB)->f5;_LL22:{struct Cyc_Absyn_Seq_s_struct
_tmp1DC;struct Cyc_Absyn_Seq_s_struct*_tmp1DB;(void*)(_tmp39->r=(void*)((void*)(
Cyc_Absyn_new_stmt((void*)((_tmp1DB=_cycalloc(sizeof(*_tmp1DB)),((_tmp1DB[0]=((
_tmp1DC.tag=1,((_tmp1DC.f1=Cyc_Absyn_new_stmt((void*)_tmp39->r,_tmp39->loc),((
_tmp1DC.f2=Cyc_Absyn_skip_stmt(_tmp39->loc),_tmp1DC)))))),_tmp1DB)))),_tmp39->loc))->r));}{
struct _RegionHandle _tmp110=_new_region("r");struct _RegionHandle*r=& _tmp110;
_push_region(r);{struct Cyc_Tcenv_Tenv*te2;if(new_block)te2=Cyc_Tcenv_new_block(r,
s0->loc,te);else{te2=Cyc_Tcenv_copy_tenv(r,te);}{struct Cyc_Absyn_Vardecl _tmp112;
void*_tmp113;void**_tmp114;struct Cyc_Core_Opt*_tmp115;struct Cyc_Core_Opt**
_tmp116;struct Cyc_Absyn_Vardecl*_tmp111=_tmp36;_tmp112=*_tmp111;_tmp113=(void*)
_tmp112.type;_tmp114=(void**)&(*_tmp111).type;_tmp115=_tmp112.rgn;_tmp116=(
struct Cyc_Core_Opt**)&(*_tmp111).rgn;{void*_tmp117=Cyc_Tcenv_curr_rgn(te2);{
struct Cyc_Core_Opt*_tmp1DD;*_tmp116=((_tmp1DD=_cycalloc(sizeof(*_tmp1DD)),((
_tmp1DD->v=(void*)_tmp117,_tmp1DD))));}{void*rgn_typ;if(_tmp38 != 0){struct Cyc_Absyn_Exp*
open_exp=(struct Cyc_Absyn_Exp*)_tmp38;struct Cyc_List_List*_tmp119=Cyc_Tcenv_lookup_type_vars(
te);{struct Cyc_Core_Opt*_tmp1DE;rgn_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,((_tmp1DE=_cycalloc(sizeof(*_tmp1DE)),((_tmp1DE->v=_tmp119,_tmp1DE)))));}{
struct Cyc_Core_Opt*_tmp1DF;void*rgn2_typ=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,((_tmp1DF=_cycalloc(sizeof(*_tmp1DF)),((_tmp1DF->v=_tmp119,_tmp1DF)))));
struct Cyc_Absyn_DynRgnType_struct _tmp1E2;struct Cyc_Absyn_DynRgnType_struct*
_tmp1E1;void*expected=(void*)((_tmp1E1=_cycalloc(sizeof(*_tmp1E1)),((_tmp1E1[0]=((
_tmp1E2.tag=16,((_tmp1E2.f1=(void*)rgn_typ,((_tmp1E2.f2=(void*)rgn2_typ,_tmp1E2)))))),
_tmp1E1))));{struct Cyc_Absyn_RgnHandleType_struct _tmp1E5;struct Cyc_Absyn_RgnHandleType_struct*
_tmp1E4;*_tmp114=(void*)((_tmp1E4=_cycalloc(sizeof(*_tmp1E4)),((_tmp1E4[0]=((
_tmp1E5.tag=15,((_tmp1E5.f1=(void*)rgn_typ,_tmp1E5)))),_tmp1E4))));}if(!Cyc_Tcutil_unify(
Cyc_Tcexp_tcExp(te,(void**)& expected,open_exp),expected)){const char*_tmp1EA;void*
_tmp1E9[2];struct Cyc_String_pa_struct _tmp1E8;struct Cyc_String_pa_struct _tmp1E7;(
_tmp1E7.tag=0,((_tmp1E7.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(open_exp->topt))->v)),((_tmp1E8.tag=0,((
_tmp1E8.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Absynpp_typ2string(
expected)),((_tmp1E9[0]=& _tmp1E8,((_tmp1E9[1]=& _tmp1E7,Cyc_Tcutil_terr(open_exp->loc,((
_tmp1EA="expecting %s but found %s",_tag_dynforward(_tmp1EA,sizeof(char),
_get_zero_arr_size_char(_tmp1EA,26)))),_tag_dynforward(_tmp1E9,sizeof(void*),2)))))))))))));}
Cyc_Tcenv_check_rgn_accessible(te,open_exp->loc,rgn2_typ);te2=Cyc_Tcenv_add_region(
r,te2,rgn_typ,0,1);}}else{{struct Cyc_Absyn_VarType_struct _tmp1ED;struct Cyc_Absyn_VarType_struct*
_tmp1EC;rgn_typ=(void*)((_tmp1EC=_cycalloc(sizeof(*_tmp1EC)),((_tmp1EC[0]=((
_tmp1ED.tag=1,((_tmp1ED.f1=_tmp35,_tmp1ED)))),_tmp1EC))));}{struct Cyc_List_List*
_tmp1EE;te2=Cyc_Tcenv_add_type_vars(r,s0->loc,te2,((_tmp1EE=_cycalloc(sizeof(*
_tmp1EE)),((_tmp1EE->hd=_tmp35,((_tmp1EE->tl=0,_tmp1EE)))))));}te2=Cyc_Tcenv_add_region(
r,te2,rgn_typ,_tmp37,1);}Cyc_Tcutil_check_type(s0->loc,te,Cyc_Tcenv_lookup_type_vars(
te2),(void*)2,1,*_tmp114);{struct Cyc_Absyn_RgnHandleType_struct _tmp1F1;struct Cyc_Absyn_RgnHandleType_struct*
_tmp1F0;if(!Cyc_Tcutil_unify(*_tmp114,(void*)((_tmp1F0=_cycalloc(sizeof(*_tmp1F0)),((
_tmp1F0[0]=((_tmp1F1.tag=15,((_tmp1F1.f1=(void*)rgn_typ,_tmp1F1)))),_tmp1F0)))))){
const char*_tmp1F4;void*_tmp1F3;(_tmp1F3=0,((int(*)(struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((_tmp1F4="region stmt: type of region handle is wrong!",
_tag_dynforward(_tmp1F4,sizeof(char),_get_zero_arr_size_char(_tmp1F4,45)))),
_tag_dynforward(_tmp1F3,sizeof(void*),0)));}}if(!Cyc_Absyn_no_regions)te2=Cyc_Tcenv_enter_try(
r,te2);Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(r,Cyc_Tcenv_add_local_var(r,
_tmp39->loc,te2,_tmp36),s0),_tmp39,0);}}}};_pop_region(r);}return;_LL23: if(_tmpB
<= (void*)1)goto _LL25;if(*((int*)_tmpB)!= 16)goto _LL25;_tmp3A=((struct Cyc_Absyn_ResetRegion_s_struct*)
_tmpB)->f1;_LL24: {void*rgn_type=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,
Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_RgnHandleType_struct _tmp1F7;
struct Cyc_Absyn_RgnHandleType_struct*_tmp1F6;void*etype=(void*)((_tmp1F6=
_cycalloc(sizeof(*_tmp1F6)),((_tmp1F6[0]=((_tmp1F7.tag=15,((_tmp1F7.f1=(void*)
rgn_type,_tmp1F7)))),_tmp1F6))));if(!Cyc_Tcutil_unify(Cyc_Tcexp_tcExp(te,(void**)&
etype,_tmp3A),etype)){const char*_tmp1FB;void*_tmp1FA[1];struct Cyc_String_pa_struct
_tmp1F9;(_tmp1F9.tag=0,((_tmp1F9.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp3A->topt))->v)),((_tmp1FA[0]=& _tmp1F9,Cyc_Tcutil_terr(_tmp3A->loc,((_tmp1FB="expecting region_t but found %s",
_tag_dynforward(_tmp1FB,sizeof(char),_get_zero_arr_size_char(_tmp1FB,32)))),
_tag_dynforward(_tmp1FA,sizeof(void*),1)))))));}Cyc_Tcenv_check_rgn_resetable(te,
s0->loc,rgn_type);return;}_LL25: if(_tmpB <= (void*)1)goto _LL0;if(*((int*)_tmpB)!= 
17)goto _LL0;_tmp3B=((struct Cyc_Absyn_Alias_s_struct*)_tmpB)->f1;_tmp3C=((struct
Cyc_Absyn_Alias_s_struct*)_tmpB)->f2;_tmp3D=((struct Cyc_Absyn_Alias_s_struct*)
_tmpB)->f3;_tmp3E=((struct Cyc_Absyn_Alias_s_struct*)_tmpB)->f4;_LL26: Cyc_Tcexp_tcExpInitializer(
te,0,_tmp3B);if(!Cyc_Tcutil_is_pointer_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3B->topt))->v)){const char*_tmp1FF;void*_tmp1FE[1];struct Cyc_String_pa_struct
_tmp1FD;(_tmp1FD.tag=0,((_tmp1FD.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp3B->topt))->v)),((_tmp1FE[0]=& _tmp1FD,Cyc_Tcutil_terr(_tmp3B->loc,((_tmp1FF="%s is not a pointer type",
_tag_dynforward(_tmp1FF,sizeof(char),_get_zero_arr_size_char(_tmp1FF,25)))),
_tag_dynforward(_tmp1FE,sizeof(void*),1)))))));}(void*)(_tmp3D->type=(void*)Cyc_Tcutil_copy_type((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3B->topt))->v));{void*_tmp133=(void*)
_tmp3D->type;struct Cyc_Absyn_PtrInfo _tmp134;struct Cyc_Absyn_PtrAtts _tmp135;void*
_tmp136;void**_tmp137;_LL87: if(_tmp133 <= (void*)4)goto _LL89;if(*((int*)_tmp133)
!= 4)goto _LL89;_tmp134=((struct Cyc_Absyn_PointerType_struct*)_tmp133)->f1;
_tmp135=_tmp134.ptr_atts;_tmp136=(void*)_tmp135.rgn;_tmp137=(void**)&((((struct
Cyc_Absyn_PointerType_struct*)_tmp133)->f1).ptr_atts).rgn;_LL88:{struct Cyc_Absyn_VarType_struct
_tmp202;struct Cyc_Absyn_VarType_struct*_tmp201;*_tmp137=(void*)((_tmp201=
_cycalloc(sizeof(*_tmp201)),((_tmp201[0]=((_tmp202.tag=1,((_tmp202.f1=_tmp3C,
_tmp202)))),_tmp201))));}goto _LL86;_LL89:;_LL8A: {const char*_tmp205;void*_tmp204;(
_tmp204=0,((int(*)(struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Tcutil_impos)(((
_tmp205="Not a pointer type in tcstmt:Alias_s",_tag_dynforward(_tmp205,sizeof(
char),_get_zero_arr_size_char(_tmp205,37)))),_tag_dynforward(_tmp204,sizeof(void*),
0)));}_LL86:;}{struct Cyc_Core_Opt*_tmp206;_tmp3D->rgn=((_tmp206=_cycalloc(
sizeof(*_tmp206)),((_tmp206->v=(void*)Cyc_Tcenv_curr_rgn(te),_tmp206))));}{
struct _RegionHandle _tmp13D=_new_region("r");struct _RegionHandle*r=& _tmp13D;
_push_region(r);{struct Cyc_Absyn_Tvar*_tmp207[1];struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(
r,s0->loc,te,((_tmp207[0]=_tmp3C,((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dynforward_ptr))Cyc_List_rlist)(r,_tag_dynforward(_tmp207,sizeof(struct
Cyc_Absyn_Tvar*),1)))));{struct Cyc_Absyn_VarType_struct _tmp20A;struct Cyc_Absyn_VarType_struct*
_tmp209;te2=Cyc_Tcenv_add_region(r,te2,(void*)((_tmp209=_cycalloc(sizeof(*
_tmp209)),((_tmp209[0]=((_tmp20A.tag=1,((_tmp20A.f1=_tmp3C,_tmp20A)))),_tmp209)))),
0,1);}Cyc_Tcutil_check_type(s0->loc,te,Cyc_Tcenv_lookup_type_vars(te2),Cyc_Tcutil_typ_kind((
void*)_tmp3D->type),1,(void*)_tmp3D->type);te2=Cyc_Tcenv_add_local_var(r,s0->loc,
te2,_tmp3D);te2=Cyc_Tcenv_set_encloser(r,te2,_tmp3E);Cyc_Tcstmt_tcStmt(te2,
_tmp3E,0);};_pop_region(r);}Cyc_NewControlFlow_set_encloser(s0,Cyc_Tcenv_get_encloser(
te));return;_LL0:;}}
