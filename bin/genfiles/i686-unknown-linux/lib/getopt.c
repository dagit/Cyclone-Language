#ifndef _SETJMP_H_
#define _SETJMP_H_
#ifndef ___sigset_t_def_
#define ___sigset_t_def_
typedef struct {unsigned long __val[1024 / (8 * sizeof(unsigned long))];} __sigset_t;
#endif
#ifndef ___jmp_buf_def_
#define ___jmp_buf_def_
typedef int __jmp_buf[6];
#endif
#ifndef ___jmp_buf_tag_def_
#define ___jmp_buf_tag_def_
struct __jmp_buf_tag{
  __jmp_buf __jmpbuf;
  int __mask_was_saved;
  __sigset_t __saved_mask;
};
#endif
#ifndef _jmp_buf_def_
#define _jmp_buf_def_
typedef struct __jmp_buf_tag jmp_buf[1];
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
 void exit(int);void*abort();struct Cyc_Core_NewRegion{struct _DynRegionHandle*
dynregion;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[21];
struct Cyc_Core_Invalid_argument_struct{char*tag;struct _dynforward_ptr f1;};extern
char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[
16];typedef struct{int __count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;
typedef struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t
Cyc_fpos_t;struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};int Cyc_fprintf(struct Cyc___cycFILE*,struct
_dynforward_ptr,struct _dynforward_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*
f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{
int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{
int tag;struct _dynforward_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;
};struct Cyc_FloatPtr_sa_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int
tag;struct _dynforward_ptr f1;};extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[
18];struct Cyc_FileOpenError_struct{char*tag;struct _dynforward_ptr f1;};typedef
struct{int quot;int rem;}Cyc_div_t;typedef struct{long quot;long rem;}Cyc_ldiv_t;void*
abort();void exit(int);char*getenv(const char*);enum Cyc___anonymous_enum_368__{
Cyc__PC_LINK_MAX  = 0,Cyc__PC_MAX_CANON  = 1,Cyc__PC_MAX_INPUT  = 2,Cyc__PC_NAME_MAX
 = 3,Cyc__PC_PATH_MAX  = 4,Cyc__PC_PIPE_BUF  = 5,Cyc__PC_CHOWN_RESTRICTED  = 6,Cyc__PC_NO_TRUNC
 = 7,Cyc__PC_VDISABLE  = 8,Cyc__PC_SYNC_IO  = 9,Cyc__PC_ASYNC_IO  = 10,Cyc__PC_PRIO_IO
 = 11,Cyc__PC_SOCK_MAXBUF  = 12,Cyc__PC_FILESIZEBITS  = 13,Cyc__PC_REC_INCR_XFER_SIZE
 = 14,Cyc__PC_REC_MAX_XFER_SIZE  = 15,Cyc__PC_REC_MIN_XFER_SIZE  = 16,Cyc__PC_REC_XFER_ALIGN
 = 17,Cyc__PC_ALLOC_SIZE_MIN  = 18,Cyc__PC_SYMLINK_MAX  = 19};enum Cyc___anonymous_enum_370__{
Cyc__CS_PATH  = 0,Cyc__CS_V6_WIDTH_RESTRICTED_ENVS  = 1};extern struct
_dynforward_ptr Cyc_optarg;extern int Cyc_optind;extern int Cyc_opterr;extern int Cyc_optopt;
struct Cyc_option{struct _dynforward_ptr name;int has_arg;int*flag;int val;};int Cyc_getopt(
int __argc,struct _dynforward_ptr __argv,struct _dynforward_ptr __shortopts);int Cyc__getopt_internal(
int __argc,struct _dynforward_ptr __argv,struct _dynforward_ptr __shortopts,struct
_dynforward_ptr __longopts,int*__longind,int __long_only);struct Cyc_List_List{void*
hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[18];extern char Cyc_List_Nth[
8];unsigned int Cyc_strlen(struct _dynforward_ptr s);int Cyc_strcmp(struct
_dynforward_ptr s1,struct _dynforward_ptr s2);int Cyc_strncmp(struct _dynforward_ptr
s1,struct _dynforward_ptr s2,unsigned int len);struct _dynforward_ptr Cyc_strchr(
struct _dynforward_ptr s,char c);struct _dynforward_ptr Cyc_optarg;int Cyc_optind=1;
int Cyc___getopt_initialized;static struct _dynforward_ptr Cyc_nextchar;int Cyc_opterr=
1;int Cyc_optopt=(int)'?';enum Cyc_ordering_tag{Cyc_REQUIRE_ORDER  = 0,Cyc_PERMUTE
 = 1,Cyc_RETURN_IN_ORDER  = 2};static enum Cyc_ordering_tag Cyc_ordering;static
struct _dynforward_ptr Cyc_posixly_correct;static int Cyc_first_nonopt;static int Cyc_last_nonopt;
static int Cyc_nonoption_flags_max_len;static int Cyc_nonoption_flags_len;static int
Cyc_original_argc;static int Cyc_original_argv;static void  __attribute__((unused )) 
Cyc_store_args_and_env(int argc,struct _dynforward_ptr argv){Cyc_original_argc=argc;
Cyc_original_argv=(int)((struct _dynforward_ptr*)_check_null(
_untag_dynforward_ptr(argv,sizeof(struct _dynforward_ptr),1)));}static void Cyc_exchange(
struct _dynforward_ptr argv){int bottom=Cyc_first_nonopt;int middle=Cyc_last_nonopt;
int top=Cyc_optind;struct _dynforward_ptr tem;while(top > middle  && middle > bottom){
if(top - middle > middle - bottom){int len=middle - bottom;register int i;for(i=0;i < 
len;i ++){tem=*((struct _dynforward_ptr*)_check_dynforward_subscript(argv,sizeof(
struct _dynforward_ptr),bottom + i));*((struct _dynforward_ptr*)
_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),bottom + i))=*((
struct _dynforward_ptr*)_check_dynforward_subscript(argv,sizeof(struct
_dynforward_ptr),(top - (middle - bottom))+ i));*((struct _dynforward_ptr*)
_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),(top - (middle - 
bottom))+ i))=tem;}top -=len;}else{int len=top - middle;register int i;for(i=0;i < len;
i ++){tem=*((struct _dynforward_ptr*)_check_dynforward_subscript(argv,sizeof(
struct _dynforward_ptr),bottom + i));*((struct _dynforward_ptr*)
_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),bottom + i))=*((
struct _dynforward_ptr*)_check_dynforward_subscript(argv,sizeof(struct
_dynforward_ptr),middle + i));*((struct _dynforward_ptr*)
_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),middle + i))=tem;}
bottom +=len;}}Cyc_first_nonopt +=Cyc_optind - Cyc_last_nonopt;Cyc_last_nonopt=Cyc_optind;}
static struct _dynforward_ptr Cyc__getopt_initialize(int argc,struct _dynforward_ptr
argv,struct _dynforward_ptr optstring){Cyc_first_nonopt=(Cyc_last_nonopt=Cyc_optind);
Cyc_nextchar=_tag_dynforward(0,0,0);Cyc_posixly_correct=({char*_tmp0=getenv((
const char*)"POSIXLY_CORRECT");_tag_dynforward(_tmp0,sizeof(char),
_get_zero_arr_size_char(_tmp0,1));});if(*((const char*)
_check_dynforward_subscript(optstring,sizeof(char),0))== '-'){Cyc_ordering=Cyc_RETURN_IN_ORDER;
_dynforward_ptr_inplace_plus(& optstring,sizeof(char),1);}else{if(*((const char*)
_check_dynforward_subscript(optstring,sizeof(char),0))== '+'){Cyc_ordering=Cyc_REQUIRE_ORDER;
_dynforward_ptr_inplace_plus(& optstring,sizeof(char),1);}else{if(Cyc_posixly_correct.curr
!= (_tag_dynforward(0,0,0)).curr)Cyc_ordering=Cyc_REQUIRE_ORDER;else{Cyc_ordering=
Cyc_PERMUTE;}}}return optstring;}int Cyc__getopt_internal(int argc,struct
_dynforward_ptr argv,struct _dynforward_ptr optstring,struct _dynforward_ptr longopts,
int*longind,int long_only){int print_errors=Cyc_opterr;if(*((const char*)
_check_dynforward_subscript(optstring,sizeof(char),0))== ':')print_errors=0;if(
argc < 1)return - 1;Cyc_optarg=_tag_dynforward(0,0,0);if(Cyc_optind == 0  || !Cyc___getopt_initialized){
if(Cyc_optind == 0)Cyc_optind=1;optstring=Cyc__getopt_initialize(argc,argv,
optstring);Cyc___getopt_initialized=1;}if(Cyc_nextchar.curr == (_tag_dynforward(0,
0,0)).curr  || *((char*)_check_dynforward_subscript(Cyc_nextchar,sizeof(char),0))
== '\000'){if(Cyc_last_nonopt > Cyc_optind)Cyc_last_nonopt=Cyc_optind;if(Cyc_first_nonopt
> Cyc_optind)Cyc_first_nonopt=Cyc_optind;if(Cyc_ordering == Cyc_PERMUTE){if(Cyc_first_nonopt
!= Cyc_last_nonopt  && Cyc_last_nonopt != Cyc_optind)Cyc_exchange(argv);else{if(
Cyc_last_nonopt != Cyc_optind)Cyc_first_nonopt=Cyc_optind;}while(Cyc_optind < argc
 && (*((char*)_check_dynforward_subscript(*((struct _dynforward_ptr*)
_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),Cyc_optind)),
sizeof(char),0))!= '-'  || *((char*)_check_dynforward_subscript(*((struct
_dynforward_ptr*)_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),
Cyc_optind)),sizeof(char),1))== '\000')){Cyc_optind ++;}Cyc_last_nonopt=Cyc_optind;}
if(Cyc_optind != argc  && !Cyc_strcmp((struct _dynforward_ptr)*((struct
_dynforward_ptr*)_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),
Cyc_optind)),({const char*_tmp1="--";_tag_dynforward(_tmp1,sizeof(char),
_get_zero_arr_size_char(_tmp1,3));}))){Cyc_optind ++;if(Cyc_first_nonopt != Cyc_last_nonopt
 && Cyc_last_nonopt != Cyc_optind)Cyc_exchange(argv);else{if(Cyc_first_nonopt == 
Cyc_last_nonopt)Cyc_first_nonopt=Cyc_optind;}Cyc_last_nonopt=argc;Cyc_optind=
argc;}if(Cyc_optind == argc){if(Cyc_first_nonopt != Cyc_last_nonopt)Cyc_optind=Cyc_first_nonopt;
return - 1;}if(*((char*)_check_dynforward_subscript(*((struct _dynforward_ptr*)
_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),Cyc_optind)),
sizeof(char),0))!= '-'  || *((char*)_check_dynforward_subscript(*((struct
_dynforward_ptr*)_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),
Cyc_optind)),sizeof(char),1))== '\000'){if(Cyc_ordering == Cyc_REQUIRE_ORDER)
return - 1;Cyc_optarg=*((struct _dynforward_ptr*)_check_dynforward_subscript(argv,
sizeof(struct _dynforward_ptr),Cyc_optind ++));return 1;}Cyc_nextchar=
_dynforward_ptr_plus(_dynforward_ptr_plus(*((struct _dynforward_ptr*)
_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),Cyc_optind)),
sizeof(char),1),sizeof(char),longopts.curr != ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr  && *((char*)_check_dynforward_subscript(*((struct
_dynforward_ptr*)_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),
Cyc_optind)),sizeof(char),1))== '-');}if(longopts.curr != ((struct _dynforward_ptr)
_tag_dynforward(0,0,0)).curr  && (*((char*)_check_dynforward_subscript(*((struct
_dynforward_ptr*)_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),
Cyc_optind)),sizeof(char),1))== '-'  || long_only  && ((int)*((char*)
_check_dynforward_subscript(*((struct _dynforward_ptr*)
_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),Cyc_optind)),
sizeof(char),2)) || !((unsigned int)(Cyc_strchr(optstring,*((char*)
_check_dynforward_subscript(*((struct _dynforward_ptr*)
_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),Cyc_optind)),
sizeof(char),1)))).curr)))){struct _dynforward_ptr nameend;struct _dynforward_ptr p;
struct _dynforward_ptr pfound=(struct _dynforward_ptr)_tag_dynforward(0,0,0);int
exact=0;int ambig=0;int indfound=- 1;int option_index;for(nameend=Cyc_nextchar;(int)*((
char*)_check_dynforward_subscript(nameend,sizeof(char),0)) && *((char*)
_check_dynforward_subscript(nameend,sizeof(char),0))!= '=';
_dynforward_ptr_inplace_plus_post(& nameend,sizeof(char),1)){;}for((p=longopts,
option_index=0);(unsigned int)(((const struct Cyc_option*)
_check_dynforward_subscript(p,sizeof(struct Cyc_option),0))->name).curr;(
_dynforward_ptr_inplace_plus_post(& p,sizeof(struct Cyc_option),1),option_index ++)){
if(!Cyc_strncmp((struct _dynforward_ptr)((const struct Cyc_option*)
_check_dynforward_subscript(p,sizeof(struct Cyc_option),0))->name,(struct
_dynforward_ptr)Cyc_nextchar,(unsigned int)((nameend.curr - Cyc_nextchar.curr)/ 
sizeof(char)))){if((unsigned int)((nameend.curr - Cyc_nextchar.curr)/ sizeof(char))
== (unsigned int)Cyc_strlen((struct _dynforward_ptr)((const struct Cyc_option*)
_check_dynforward_subscript(p,sizeof(struct Cyc_option),0))->name)){pfound=p;
indfound=option_index;exact=1;break;}else{if(pfound.curr == ((struct
_dynforward_ptr)_tag_dynforward(0,0,0)).curr){pfound=p;indfound=option_index;}
else{if(((long_only  || ((const struct Cyc_option*)_check_dynforward_subscript(
pfound,sizeof(struct Cyc_option),0))->has_arg != ((const struct Cyc_option*)
_check_dynforward_subscript(p,sizeof(struct Cyc_option),0))->has_arg) || ((const
struct Cyc_option*)_check_dynforward_subscript(pfound,sizeof(struct Cyc_option),0))->flag
!= ((const struct Cyc_option*)_check_dynforward_subscript(p,sizeof(struct Cyc_option),
0))->flag) || ((const struct Cyc_option*)_check_dynforward_subscript(pfound,
sizeof(struct Cyc_option),0))->val != ((const struct Cyc_option*)
_check_dynforward_subscript(p,sizeof(struct Cyc_option),0))->val)ambig=1;}}}}if(
ambig  && !exact){if(print_errors)({struct Cyc_String_pa_struct _tmp5;_tmp5.tag=0;
_tmp5.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((struct _dynforward_ptr*)
_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),Cyc_optind)));{
struct Cyc_String_pa_struct _tmp4;_tmp4.tag=0;_tmp4.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*((struct _dynforward_ptr*)_check_dynforward_subscript(argv,
sizeof(struct _dynforward_ptr),0)));{void*_tmp2[2]={& _tmp4,& _tmp5};Cyc_fprintf(
Cyc_stderr,({const char*_tmp3="%s: option `%s' is ambiguous\n";_tag_dynforward(
_tmp3,sizeof(char),_get_zero_arr_size_char(_tmp3,30));}),_tag_dynforward(_tmp2,
sizeof(void*),2));}}});_dynforward_ptr_inplace_plus(& Cyc_nextchar,sizeof(char),(
int)Cyc_strlen((struct _dynforward_ptr)Cyc_nextchar));Cyc_optind ++;Cyc_optopt=0;
return(int)'?';}if(pfound.curr != ((struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr){
option_index=indfound;Cyc_optind ++;if((int)*((char*)_check_dynforward_subscript(
nameend,sizeof(char),0))){if(((const struct Cyc_option*)
_check_dynforward_subscript(pfound,sizeof(struct Cyc_option),0))->has_arg)Cyc_optarg=
_dynforward_ptr_plus(nameend,sizeof(char),1);else{if(print_errors){if(*((char*)
_check_dynforward_subscript(*((struct _dynforward_ptr*)
_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),Cyc_optind - 1)),
sizeof(char),1))== '-')({struct Cyc_String_pa_struct _tmp9;_tmp9.tag=0;_tmp9.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)((const struct Cyc_option*)
_check_dynforward_subscript(pfound,sizeof(struct Cyc_option),0))->name);{struct
Cyc_String_pa_struct _tmp8;_tmp8.tag=0;_tmp8.f1=(struct _dynforward_ptr)((struct
_dynforward_ptr)*((struct _dynforward_ptr*)_check_dynforward_subscript(argv,
sizeof(struct _dynforward_ptr),0)));{void*_tmp6[2]={& _tmp8,& _tmp9};Cyc_fprintf(
Cyc_stderr,({const char*_tmp7="%s: option `--%s' doesn't allow an argument\n";
_tag_dynforward(_tmp7,sizeof(char),_get_zero_arr_size_char(_tmp7,45));}),
_tag_dynforward(_tmp6,sizeof(void*),2));}}});else{({struct Cyc_String_pa_struct
_tmpE;_tmpE.tag=0;_tmpE.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)((
const struct Cyc_option*)_check_dynforward_subscript(pfound,sizeof(struct Cyc_option),
0))->name);{struct Cyc_Int_pa_struct _tmpD;_tmpD.tag=1;_tmpD.f1=(unsigned long)((
int)*((char*)_check_dynforward_subscript(*((struct _dynforward_ptr*)
_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),Cyc_optind - 1)),
sizeof(char),0)));{struct Cyc_String_pa_struct _tmpC;_tmpC.tag=0;_tmpC.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*((struct _dynforward_ptr*)
_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),0)));{void*_tmpA[
3]={& _tmpC,& _tmpD,& _tmpE};Cyc_fprintf(Cyc_stderr,({const char*_tmpB="%s: option `%c%s' doesn't allow an argument\n";
_tag_dynforward(_tmpB,sizeof(char),_get_zero_arr_size_char(_tmpB,45));}),
_tag_dynforward(_tmpA,sizeof(void*),3));}}}});}}_dynforward_ptr_inplace_plus(&
Cyc_nextchar,sizeof(char),(int)Cyc_strlen((struct _dynforward_ptr)Cyc_nextchar));
Cyc_optopt=((const struct Cyc_option*)_check_dynforward_subscript(pfound,sizeof(
struct Cyc_option),0))->val;return(int)'?';}}else{if(((const struct Cyc_option*)
_check_dynforward_subscript(pfound,sizeof(struct Cyc_option),0))->has_arg == 1){
if(Cyc_optind < argc)Cyc_optarg=*((struct _dynforward_ptr*)
_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),Cyc_optind ++));
else{if(print_errors)({struct Cyc_String_pa_struct _tmp12;_tmp12.tag=0;_tmp12.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*((struct _dynforward_ptr*)
_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),Cyc_optind - 1)));{
struct Cyc_String_pa_struct _tmp11;_tmp11.tag=0;_tmp11.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*((struct _dynforward_ptr*)_check_dynforward_subscript(argv,
sizeof(struct _dynforward_ptr),0)));{void*_tmpF[2]={& _tmp11,& _tmp12};Cyc_fprintf(
Cyc_stderr,({const char*_tmp10="%s: option `%s' requires an argument\n";
_tag_dynforward(_tmp10,sizeof(char),_get_zero_arr_size_char(_tmp10,38));}),
_tag_dynforward(_tmpF,sizeof(void*),2));}}});_dynforward_ptr_inplace_plus(& Cyc_nextchar,
sizeof(char),(int)Cyc_strlen((struct _dynforward_ptr)Cyc_nextchar));Cyc_optopt=((
const struct Cyc_option*)_check_dynforward_subscript(pfound,sizeof(struct Cyc_option),
0))->val;return(int)(*((const char*)_check_dynforward_subscript(optstring,sizeof(
char),0))== ':'?':':'?');}}}_dynforward_ptr_inplace_plus(& Cyc_nextchar,sizeof(
char),(int)Cyc_strlen((struct _dynforward_ptr)Cyc_nextchar));if(longind != 0)*
longind=option_index;if((unsigned int)((const struct Cyc_option*)
_check_dynforward_subscript(pfound,sizeof(struct Cyc_option),0))->flag){*((int*)
_check_null(((const struct Cyc_option*)_check_dynforward_subscript(pfound,sizeof(
struct Cyc_option),0))->flag))=((const struct Cyc_option*)
_check_dynforward_subscript(pfound,sizeof(struct Cyc_option),0))->val;return 0;}
return((const struct Cyc_option*)_check_dynforward_subscript(pfound,sizeof(struct
Cyc_option),0))->val;}if((!long_only  || *((char*)_check_dynforward_subscript(*((
struct _dynforward_ptr*)_check_dynforward_subscript(argv,sizeof(struct
_dynforward_ptr),Cyc_optind)),sizeof(char),1))== '-') || (Cyc_strchr(optstring,*((
char*)_check_dynforward_subscript(Cyc_nextchar,sizeof(char),0)))).curr == ((
struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr){if(print_errors){if(*((char*)
_check_dynforward_subscript(*((struct _dynforward_ptr*)
_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),Cyc_optind)),
sizeof(char),1))== '-')({struct Cyc_String_pa_struct _tmp16;_tmp16.tag=0;_tmp16.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_nextchar);{struct Cyc_String_pa_struct
_tmp15;_tmp15.tag=0;_tmp15.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct _dynforward_ptr*)_check_dynforward_subscript(argv,sizeof(struct
_dynforward_ptr),0)));{void*_tmp13[2]={& _tmp15,& _tmp16};Cyc_fprintf(Cyc_stderr,({
const char*_tmp14="%s: unrecognized option `--%s'\n";_tag_dynforward(_tmp14,
sizeof(char),_get_zero_arr_size_char(_tmp14,32));}),_tag_dynforward(_tmp13,
sizeof(void*),2));}}});else{({struct Cyc_String_pa_struct _tmp1B;_tmp1B.tag=0;
_tmp1B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_nextchar);{struct
Cyc_Int_pa_struct _tmp1A;_tmp1A.tag=1;_tmp1A.f1=(unsigned long)((int)*((char*)
_check_dynforward_subscript(*((struct _dynforward_ptr*)
_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),Cyc_optind)),
sizeof(char),0)));{struct Cyc_String_pa_struct _tmp19;_tmp19.tag=0;_tmp19.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*((struct _dynforward_ptr*)
_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),0)));{void*_tmp17[
3]={& _tmp19,& _tmp1A,& _tmp1B};Cyc_fprintf(Cyc_stderr,({const char*_tmp18="%s: unrecognized option `%c%s'\n";
_tag_dynforward(_tmp18,sizeof(char),_get_zero_arr_size_char(_tmp18,32));}),
_tag_dynforward(_tmp17,sizeof(void*),3));}}}});}}Cyc_nextchar=({char*_tmp1F=({
unsigned int _tmp1C=(unsigned int)1;char*_tmp1D=(char*)_cycalloc_atomic(
_check_times(sizeof(char),_tmp1C + 1));{unsigned int _tmp1E=_tmp1C;unsigned int i;
for(i=0;i < _tmp1E;i ++){_tmp1D[i]='\000';}_tmp1D[_tmp1E]=(char)0;}_tmp1D;});
_tag_dynforward(_tmp1F,sizeof(char),_get_zero_arr_size_char(_tmp1F,(unsigned int)
1 + 1));});Cyc_optind ++;Cyc_optopt=0;return(int)'?';}}{char c=*((char*)
_check_dynforward_subscript(_dynforward_ptr_inplace_plus_post(& Cyc_nextchar,
sizeof(char),1),sizeof(char),0));struct _dynforward_ptr temp=Cyc_strchr(optstring,
c);if(*((char*)_check_dynforward_subscript(Cyc_nextchar,sizeof(char),0))== '\000')
++ Cyc_optind;if(temp.curr == ((struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr
 || c == ':'){if(print_errors){if((unsigned int)Cyc_posixly_correct.curr)({struct
Cyc_Int_pa_struct _tmp23;_tmp23.tag=1;_tmp23.f1=(unsigned long)((int)c);{struct
Cyc_String_pa_struct _tmp22;_tmp22.tag=0;_tmp22.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*((struct _dynforward_ptr*)_check_dynforward_subscript(argv,
sizeof(struct _dynforward_ptr),0)));{void*_tmp20[2]={& _tmp22,& _tmp23};Cyc_fprintf(
Cyc_stderr,({const char*_tmp21="%s: illegal option -- %c\n";_tag_dynforward(
_tmp21,sizeof(char),_get_zero_arr_size_char(_tmp21,26));}),_tag_dynforward(
_tmp20,sizeof(void*),2));}}});else{({struct Cyc_Int_pa_struct _tmp27;_tmp27.tag=1;
_tmp27.f1=(unsigned long)((int)c);{struct Cyc_String_pa_struct _tmp26;_tmp26.tag=0;
_tmp26.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((struct
_dynforward_ptr*)_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),
0)));{void*_tmp24[2]={& _tmp26,& _tmp27};Cyc_fprintf(Cyc_stderr,({const char*_tmp25="%s: invalid option -- %c\n";
_tag_dynforward(_tmp25,sizeof(char),_get_zero_arr_size_char(_tmp25,26));}),
_tag_dynforward(_tmp24,sizeof(void*),2));}}});}}Cyc_optopt=(int)c;return(int)'?';}
if(*((const char*)_check_dynforward_subscript(temp,sizeof(char),0))== 'W'  && *((
const char*)_check_dynforward_subscript(temp,sizeof(char),1))== ';'){struct
_dynforward_ptr nameend;struct _dynforward_ptr p;struct _dynforward_ptr pfound=(
struct _dynforward_ptr)_tag_dynforward(0,0,0);int exact=0;int ambig=0;int indfound=0;
int option_index;if(*((char*)_check_dynforward_subscript(Cyc_nextchar,sizeof(char),
0))!= '\000'){Cyc_optarg=Cyc_nextchar;Cyc_optind ++;}else{if(Cyc_optind == argc){
if(print_errors)({struct Cyc_Int_pa_struct _tmp2B;_tmp2B.tag=1;_tmp2B.f1=(
unsigned long)((int)c);{struct Cyc_String_pa_struct _tmp2A;_tmp2A.tag=0;_tmp2A.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*((struct _dynforward_ptr*)
_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),0)));{void*_tmp28[
2]={& _tmp2A,& _tmp2B};Cyc_fprintf(Cyc_stderr,({const char*_tmp29="%s: option requires an argument -- %c\n";
_tag_dynforward(_tmp29,sizeof(char),_get_zero_arr_size_char(_tmp29,39));}),
_tag_dynforward(_tmp28,sizeof(void*),2));}}});Cyc_optopt=(int)c;if(*((const char*)
_check_dynforward_subscript(optstring,sizeof(char),0))== ':')c=':';else{c='?';}
return(int)c;}else{Cyc_optarg=*((struct _dynforward_ptr*)
_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),Cyc_optind ++));}}
for(Cyc_nextchar=(nameend=Cyc_optarg);(int)*((char*)_check_dynforward_subscript(
nameend,sizeof(char),0)) && *((char*)_check_dynforward_subscript(nameend,sizeof(
char),0))!= '=';_dynforward_ptr_inplace_plus_post(& nameend,sizeof(char),1)){;}
for((p=longopts,option_index=0);(unsigned int)(((const struct Cyc_option*)
_check_dynforward_subscript(p,sizeof(struct Cyc_option),0))->name).curr;(
_dynforward_ptr_inplace_plus_post(& p,sizeof(struct Cyc_option),1),option_index ++)){
if(!Cyc_strncmp((struct _dynforward_ptr)((const struct Cyc_option*)
_check_dynforward_subscript(p,sizeof(struct Cyc_option),0))->name,(struct
_dynforward_ptr)Cyc_nextchar,(unsigned int)((nameend.curr - Cyc_nextchar.curr)/ 
sizeof(char)))){if((unsigned int)((nameend.curr - Cyc_nextchar.curr)/ sizeof(char))
== Cyc_strlen((struct _dynforward_ptr)((const struct Cyc_option*)
_check_dynforward_subscript(p,sizeof(struct Cyc_option),0))->name)){pfound=
_tag_dynforward((const struct Cyc_option*)_check_null(_untag_dynforward_ptr(p,
sizeof(struct Cyc_option),1)),sizeof(struct Cyc_option),1);indfound=option_index;
exact=1;break;}else{if(pfound.curr == ((struct _dynforward_ptr)_tag_dynforward(0,0,
0)).curr){pfound=_tag_dynforward((const struct Cyc_option*)_check_null(
_untag_dynforward_ptr(p,sizeof(struct Cyc_option),1)),sizeof(struct Cyc_option),1);
indfound=option_index;}else{ambig=1;}}}}if(ambig  && !exact){if(print_errors)({
struct Cyc_String_pa_struct _tmp2F;_tmp2F.tag=0;_tmp2F.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*((struct _dynforward_ptr*)_check_dynforward_subscript(argv,
sizeof(struct _dynforward_ptr),Cyc_optind)));{struct Cyc_String_pa_struct _tmp2E;
_tmp2E.tag=0;_tmp2E.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((struct
_dynforward_ptr*)_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),
0)));{void*_tmp2C[2]={& _tmp2E,& _tmp2F};Cyc_fprintf(Cyc_stderr,({const char*_tmp2D="%s: option `-W %s' is ambiguous\n";
_tag_dynforward(_tmp2D,sizeof(char),_get_zero_arr_size_char(_tmp2D,33));}),
_tag_dynforward(_tmp2C,sizeof(void*),2));}}});_dynforward_ptr_inplace_plus(& Cyc_nextchar,
sizeof(char),(int)Cyc_strlen((struct _dynforward_ptr)Cyc_nextchar));Cyc_optind ++;
return(int)'?';}if(pfound.curr != ((struct _dynforward_ptr)_tag_dynforward(0,0,0)).curr){
option_index=indfound;if((int)*((char*)_check_dynforward_subscript(nameend,
sizeof(char),0))){if(((const struct Cyc_option*)_check_dynforward_subscript(pfound,
sizeof(struct Cyc_option),0))->has_arg)Cyc_optarg=_dynforward_ptr_plus(nameend,
sizeof(char),1);else{if(print_errors)({struct Cyc_String_pa_struct _tmp33;_tmp33.tag=
0;_tmp33.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)((const struct Cyc_option*)
_check_dynforward_subscript(pfound,sizeof(struct Cyc_option),0))->name);{struct
Cyc_String_pa_struct _tmp32;_tmp32.tag=0;_tmp32.f1=(struct _dynforward_ptr)((
struct _dynforward_ptr)*((struct _dynforward_ptr*)_check_dynforward_subscript(argv,
sizeof(struct _dynforward_ptr),0)));{void*_tmp30[2]={& _tmp32,& _tmp33};Cyc_fprintf(
Cyc_stderr,({const char*_tmp31="%s: option `-W %s' doesn't allow an argument\n";
_tag_dynforward(_tmp31,sizeof(char),_get_zero_arr_size_char(_tmp31,46));}),
_tag_dynforward(_tmp30,sizeof(void*),2));}}});_dynforward_ptr_inplace_plus(& Cyc_nextchar,
sizeof(char),(int)Cyc_strlen((struct _dynforward_ptr)Cyc_nextchar));return(int)'?';}}
else{if(((const struct Cyc_option*)_check_dynforward_subscript(pfound,sizeof(
struct Cyc_option),0))->has_arg == 1){if(Cyc_optind < argc)Cyc_optarg=*((struct
_dynforward_ptr*)_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),
Cyc_optind ++));else{if(print_errors)({struct Cyc_String_pa_struct _tmp37;_tmp37.tag=
0;_tmp37.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((struct
_dynforward_ptr*)_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),
Cyc_optind - 1)));{struct Cyc_String_pa_struct _tmp36;_tmp36.tag=0;_tmp36.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*((struct _dynforward_ptr*)
_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),0)));{void*_tmp34[
2]={& _tmp36,& _tmp37};Cyc_fprintf(Cyc_stderr,({const char*_tmp35="%s: option `%s' requires an argument\n";
_tag_dynforward(_tmp35,sizeof(char),_get_zero_arr_size_char(_tmp35,38));}),
_tag_dynforward(_tmp34,sizeof(void*),2));}}});_dynforward_ptr_inplace_plus(& Cyc_nextchar,
sizeof(char),(int)Cyc_strlen((struct _dynforward_ptr)Cyc_nextchar));return(int)(*((
const char*)_check_dynforward_subscript(optstring,sizeof(char),0))== ':'?':':'?');}}}
_dynforward_ptr_inplace_plus(& Cyc_nextchar,sizeof(char),(int)Cyc_strlen((struct
_dynforward_ptr)Cyc_nextchar));if(longind != 0)*longind=option_index;if((
unsigned int)((const struct Cyc_option*)_check_dynforward_subscript(pfound,sizeof(
struct Cyc_option),0))->flag){*((int*)_check_null(((const struct Cyc_option*)
_check_dynforward_subscript(pfound,sizeof(struct Cyc_option),0))->flag))=((const
struct Cyc_option*)_check_dynforward_subscript(pfound,sizeof(struct Cyc_option),0))->val;
return 0;}return((const struct Cyc_option*)_check_dynforward_subscript(pfound,
sizeof(struct Cyc_option),0))->val;}Cyc_nextchar=_tag_dynforward(0,0,0);return(
int)'W';}if(*((const char*)_check_dynforward_subscript(temp,sizeof(char),1))== ':'){
if(*((const char*)_check_dynforward_subscript(temp,sizeof(char),2))== ':'){if(*((
char*)_check_dynforward_subscript(Cyc_nextchar,sizeof(char),0))!= '\000'){Cyc_optarg=
Cyc_nextchar;Cyc_optind ++;}else{Cyc_optarg=_tag_dynforward(0,0,0);}Cyc_nextchar=
_tag_dynforward(0,0,0);}else{if(*((char*)_check_dynforward_subscript(Cyc_nextchar,
sizeof(char),0))!= '\000'){Cyc_optarg=Cyc_nextchar;Cyc_optind ++;}else{if(Cyc_optind
== argc){if(print_errors)({struct Cyc_Int_pa_struct _tmp3B;_tmp3B.tag=1;_tmp3B.f1=(
unsigned long)((int)c);{struct Cyc_String_pa_struct _tmp3A;_tmp3A.tag=0;_tmp3A.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)*((struct _dynforward_ptr*)
_check_dynforward_subscript(argv,sizeof(struct _dynforward_ptr),0)));{void*_tmp38[
2]={& _tmp3A,& _tmp3B};Cyc_fprintf(Cyc_stderr,({const char*_tmp39="%s: option requires an argument -- %c\n";
_tag_dynforward(_tmp39,sizeof(char),_get_zero_arr_size_char(_tmp39,39));}),
_tag_dynforward(_tmp38,sizeof(void*),2));}}});Cyc_optopt=(int)c;if(*((const char*)
_check_dynforward_subscript(optstring,sizeof(char),0))== ':')c=':';else{c='?';}}
else{Cyc_optarg=*((struct _dynforward_ptr*)_check_dynforward_subscript(argv,
sizeof(struct _dynforward_ptr),Cyc_optind ++));}}Cyc_nextchar=_tag_dynforward(0,0,
0);}}return(int)c;}}int Cyc_getopt(int argc,struct _dynforward_ptr argv,struct
_dynforward_ptr optstring){return Cyc__getopt_internal(argc,argv,optstring,(struct
_dynforward_ptr)_tag_dynforward(0,0,0),(int*)0,0);}
