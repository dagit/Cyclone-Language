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
 struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};struct Cyc_Core_Opt{
void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dynforward_ptr f1;};extern char Cyc_Core_Not_found[14];extern char
Cyc_Core_Unreachable[16];struct Cyc_Core_Unreachable_struct{char*tag;struct
_dynforward_ptr f1;};extern struct _RegionHandle*Cyc_Core_heap_region;extern char Cyc_Core_Open_Region[
16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_list(struct _dynforward_ptr);struct Cyc_List_List*
Cyc_List_rlist(struct _RegionHandle*,struct _dynforward_ptr);int Cyc_List_length(
struct Cyc_List_List*x);void*Cyc_List_hd(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_tl(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct
Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*
Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
struct Cyc_List_List*Cyc_List_rmap2(struct _RegionHandle*,void*(*f)(void*,void*),
struct Cyc_List_List*x,struct Cyc_List_List*y);void Cyc_List_app(void*(*f)(void*),
struct Cyc_List_List*x);void Cyc_List_app_c(void*(*f)(void*,void*),void*,struct Cyc_List_List*
x);void Cyc_List_app2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y);void Cyc_List_app2_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_List_List*
x,struct Cyc_List_List*y);void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);void
Cyc_List_iter2(void(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
void Cyc_List_iter2_c(void(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,
struct Cyc_List_List*y);void*Cyc_List_fold_left(void*(*f)(void*,void*),void*accum,
struct Cyc_List_List*x);void*Cyc_List_fold_left_c(void*(*f)(void*,void*,void*),
void*,void*accum,struct Cyc_List_List*x);void*Cyc_List_fold_right(void*(*f)(void*,
void*),struct Cyc_List_List*x,void*accum);void*Cyc_List_fold_right_c(void*(*f)(
void*,void*,void*),void*,struct Cyc_List_List*x,void*accum);struct Cyc_List_List*
Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_rrevappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rrev(struct _RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rappend(
struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*
Cyc_List_flatten(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rflatten(
struct _RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_merge_sort(
int(*cmp)(void*,void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmerge_sort(
struct _RegionHandle*,int(*cmp)(void*,void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rimp_merge_sort(int(*cmp)(void*,void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_merge(int(*cmp)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*
y);struct Cyc_List_List*Cyc_List_rmerge(struct _RegionHandle*,int(*cmp)(void*,void*),
struct Cyc_List_List*a,struct Cyc_List_List*b);struct Cyc_List_List*Cyc_List_imp_merge(
int(*cmp)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*b);extern char
Cyc_List_Nth[8];void*Cyc_List_nth(struct Cyc_List_List*x,int n);struct Cyc_List_List*
Cyc_List_nth_tail(struct Cyc_List_List*x,int i);int Cyc_List_forall(int(*pred)(void*),
struct Cyc_List_List*x);int Cyc_List_forall_c(int(*pred)(void*,void*),void*env,
struct Cyc_List_List*x);int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*
x);int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_zip3(
struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z);struct Cyc_List_List*
Cyc_List_rzip3(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y,struct Cyc_List_List*z);struct _tuple0{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);
struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;};struct _tuple1 Cyc_List_split3(struct Cyc_List_List*x);struct _tuple0 Cyc_List_rsplit(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);struct
_tuple1 Cyc_List_rsplit3(struct _RegionHandle*r3,struct _RegionHandle*r4,struct
_RegionHandle*r5,struct Cyc_List_List*x);int Cyc_List_memq(struct Cyc_List_List*l,
void*x);int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
void*Cyc_List_assoc(struct Cyc_List_List*l,void*k);void*Cyc_List_assoc_cmp(int(*
cmp)(void*,void*),struct Cyc_List_List*l,void*x);int Cyc_List_mem_assoc(struct Cyc_List_List*
l,void*x);struct Cyc_List_List*Cyc_List_delete(struct Cyc_List_List*l,void*x);
struct Cyc_List_List*Cyc_List_delete_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*
l,void*x);struct Cyc_Core_Opt*Cyc_List_check_unique(int(*cmp)(void*,void*),struct
Cyc_List_List*x);struct _dynforward_ptr Cyc_List_to_array(struct Cyc_List_List*x);
struct _dynforward_ptr Cyc_List_rto_array(struct _RegionHandle*r,struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_from_array(struct _dynforward_ptr arr);struct Cyc_List_List*
Cyc_List_rfrom_array(struct _RegionHandle*r2,struct _dynforward_ptr arr);struct Cyc_List_List*
Cyc_List_tabulate(int n,void*(*f)(int));struct Cyc_List_List*Cyc_List_tabulate_c(
int n,void*(*f)(void*,int),void*env);struct Cyc_List_List*Cyc_List_rtabulate(
struct _RegionHandle*r,int n,void*(*f)(int));struct Cyc_List_List*Cyc_List_rtabulate_c(
struct _RegionHandle*r,int n,void*(*f)(void*,int),void*env);int Cyc_List_list_cmp(
int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);int Cyc_List_list_prefix(
int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_List_List*
Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_filter_c(
int(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rfilter(
struct _RegionHandle*r,int(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rfilter_c(struct _RegionHandle*r,int(*f)(void*,void*),void*env,struct Cyc_List_List*
x);int Cyc_List_length(struct Cyc_List_List*x){int i=0;while(x != 0){++ i;x=x->tl;}
return i;}void*Cyc_List_hd(struct Cyc_List_List*x){if((struct Cyc_List_List*)x == 0)(
int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp0=_cycalloc(sizeof(*_tmp0));
_tmp0[0]=({struct Cyc_Core_Failure_struct _tmp1;_tmp1.tag=Cyc_Core_Failure;_tmp1.f1=({
const char*_tmp2="hd";_tag_dynforward(_tmp2,sizeof(char),_get_zero_arr_size_char(
_tmp2,3));});_tmp1;});_tmp0;}));return(void*)x->hd;}struct Cyc_List_List*Cyc_List_tl(
struct Cyc_List_List*x){if((struct Cyc_List_List*)x == 0)(int)_throw((void*)({
struct Cyc_Core_Failure_struct*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3[0]=({struct
Cyc_Core_Failure_struct _tmp4;_tmp4.tag=Cyc_Core_Failure;_tmp4.f1=({const char*
_tmp5="tl";_tag_dynforward(_tmp5,sizeof(char),_get_zero_arr_size_char(_tmp5,3));});
_tmp4;});_tmp3;}));return x->tl;}struct Cyc_List_List*Cyc_List_rlist(struct
_RegionHandle*r,struct _dynforward_ptr argv){struct Cyc_List_List*result=0;{int i=(
int)(_get_dynforward_size(argv,sizeof(void*))- 1);for(0;i >= 0;i --){result=({
struct Cyc_List_List*_tmp6=_region_malloc(r,sizeof(*_tmp6));_tmp6->hd=(void*)*((
void**)_check_dynforward_subscript(argv,sizeof(void*),i));_tmp6->tl=result;_tmp6;});}}
return result;}struct Cyc_List_List*Cyc_List_list(struct _dynforward_ptr argv){
struct Cyc_List_List*result=0;{int i=(int)(_get_dynforward_size(argv,sizeof(void*))
- 1);for(0;i >= 0;i --){result=({struct Cyc_List_List*_tmp7=_cycalloc(sizeof(*_tmp7));
_tmp7->hd=(void*)*((void**)_check_dynforward_subscript(argv,sizeof(void*),i));
_tmp7->tl=result;_tmp7;});}}return result;}struct Cyc_List_List*Cyc_List_rcopy(
struct _RegionHandle*r2,struct Cyc_List_List*x){struct Cyc_List_List*result;struct
Cyc_List_List*prev;if(x == 0)return 0;result=({struct Cyc_List_List*_tmp8=
_region_malloc(r2,sizeof(*_tmp8));_tmp8->hd=(void*)((void*)x->hd);_tmp8->tl=0;
_tmp8;});prev=result;for(x=x->tl;x != 0;x=x->tl){struct Cyc_List_List*temp=({
struct Cyc_List_List*_tmp9=_region_malloc(r2,sizeof(*_tmp9));_tmp9->hd=(void*)((
void*)x->hd);_tmp9->tl=0;_tmp9;});prev->tl=(struct Cyc_List_List*)temp;prev=temp;}
return(struct Cyc_List_List*)result;}struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*
x){return Cyc_List_rcopy(Cyc_Core_heap_region,x);}struct Cyc_List_List*Cyc_List_rmap(
struct _RegionHandle*r2,void*(*f)(void*),struct Cyc_List_List*x){struct Cyc_List_List*
result;struct Cyc_List_List*prev;if(x == 0)return 0;result=({struct Cyc_List_List*
_tmpA=_region_malloc(r2,sizeof(*_tmpA));_tmpA->hd=(void*)f((void*)x->hd);_tmpA->tl=
0;_tmpA;});prev=result;for(x=x->tl;x != 0;x=x->tl){struct Cyc_List_List*temp=({
struct Cyc_List_List*_tmpB=_region_malloc(r2,sizeof(*_tmpB));_tmpB->hd=(void*)f((
void*)x->hd);_tmpB->tl=0;_tmpB;});prev->tl=(struct Cyc_List_List*)temp;prev=temp;}
return(struct Cyc_List_List*)result;}struct Cyc_List_List*Cyc_List_map(void*(*f)(
void*),struct Cyc_List_List*x){return Cyc_List_rmap(Cyc_Core_heap_region,f,x);}
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*r2,void*(*f)(void*,void*),
void*env,struct Cyc_List_List*x){struct Cyc_List_List*result;struct Cyc_List_List*
prev;if(x == 0)return 0;result=({struct Cyc_List_List*_tmpC=_region_malloc(r2,
sizeof(*_tmpC));_tmpC->hd=(void*)f(env,(void*)x->hd);_tmpC->tl=0;_tmpC;});prev=
result;for(x=x->tl;x != 0;x=x->tl){struct Cyc_List_List*e=({struct Cyc_List_List*
_tmpD=_region_malloc(r2,sizeof(*_tmpD));_tmpD->hd=(void*)f(env,(void*)x->hd);
_tmpD->tl=0;_tmpD;});prev->tl=(struct Cyc_List_List*)e;prev=e;}return(struct Cyc_List_List*)
result;}struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,
struct Cyc_List_List*x){return Cyc_List_rmap_c(Cyc_Core_heap_region,f,env,x);}char
Cyc_List_List_mismatch[18]="\000\000\000\000List_mismatch\000";struct Cyc_List_List*
Cyc_List_rmap2(struct _RegionHandle*r3,void*(*f)(void*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y){struct Cyc_List_List*result;struct Cyc_List_List*prev;if(
x == 0  && y == 0)return 0;if(x == 0  || y == 0)(int)_throw((void*)Cyc_List_List_mismatch);
result=({struct Cyc_List_List*_tmpE=_region_malloc(r3,sizeof(*_tmpE));_tmpE->hd=(
void*)f((void*)x->hd,(void*)y->hd);_tmpE->tl=0;_tmpE;});prev=result;x=x->tl;y=y->tl;
while(x != 0  && y != 0){struct Cyc_List_List*temp=({struct Cyc_List_List*_tmpF=
_region_malloc(r3,sizeof(*_tmpF));_tmpF->hd=(void*)f((void*)x->hd,(void*)y->hd);
_tmpF->tl=0;_tmpF;});prev->tl=(struct Cyc_List_List*)temp;prev=temp;x=x->tl;y=y->tl;}
if(x != 0  || y != 0)(int)_throw((void*)Cyc_List_List_mismatch);return(struct Cyc_List_List*)
result;}struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*
x,struct Cyc_List_List*y){return Cyc_List_rmap2(Cyc_Core_heap_region,f,x,y);}void
Cyc_List_app(void*(*f)(void*),struct Cyc_List_List*x){while(x != 0){f((void*)x->hd);
x=x->tl;}}void Cyc_List_app_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x){while(x != 0){f(env,(void*)x->hd);x=x->tl;}}void Cyc_List_iter(void(*f)(void*),
struct Cyc_List_List*x){while(x != 0){f((void*)x->hd);x=x->tl;}}void Cyc_List_iter_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*x){while(x != 0){f(env,(void*)
x->hd);x=x->tl;}}void Cyc_List_app2(void*(*f)(void*,void*),struct Cyc_List_List*x,
struct Cyc_List_List*y){while(x != 0  && y != 0){f((void*)x->hd,(void*)y->hd);x=x->tl;
y=y->tl;}if(x != 0  || y != 0)(int)_throw((void*)Cyc_List_List_mismatch);}void Cyc_List_app2_c(
void*(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,struct Cyc_List_List*
y){while(x != 0  && y != 0){f(env,(void*)x->hd,(void*)y->hd);x=x->tl;y=y->tl;}if(x
!= 0  || y != 0)(int)_throw((void*)Cyc_List_List_mismatch);}void Cyc_List_iter2(
void(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y){while(x != 0
 && y != 0){f((void*)x->hd,(void*)y->hd);x=x->tl;y=y->tl;}if(x != 0  || y != 0)(int)
_throw((void*)Cyc_List_List_mismatch);}void Cyc_List_iter2_c(void(*f)(void*,void*,
void*),void*env,struct Cyc_List_List*x,struct Cyc_List_List*y){while(x != 0  && y != 
0){f(env,(void*)x->hd,(void*)y->hd);x=x->tl;y=y->tl;}if(x != 0  || y != 0)(int)
_throw((void*)Cyc_List_List_mismatch);}void*Cyc_List_fold_left(void*(*f)(void*,
void*),void*accum,struct Cyc_List_List*x){while(x != 0){accum=f(accum,(void*)x->hd);
x=x->tl;}return accum;}void*Cyc_List_fold_left_c(void*(*f)(void*,void*,void*),
void*env,void*accum,struct Cyc_List_List*x){while(x != 0){accum=f(env,accum,(void*)
x->hd);x=x->tl;}return accum;}void*Cyc_List_fold_right(void*(*f)(void*,void*),
struct Cyc_List_List*x,void*accum){if(x == 0)return accum;else{return f((void*)x->hd,
Cyc_List_fold_right(f,x->tl,accum));}}void*Cyc_List_fold_right_c(void*(*f)(void*,
void*,void*),void*env,struct Cyc_List_List*x,void*accum){if(x == 0)return accum;
else{return f(env,(void*)x->hd,Cyc_List_fold_right_c(f,env,x->tl,accum));}}struct
Cyc_List_List*Cyc_List_rrevappend(struct _RegionHandle*r2,struct Cyc_List_List*x,
struct Cyc_List_List*y){while(x != 0){y=({struct Cyc_List_List*_tmp10=
_region_malloc(r2,sizeof(*_tmp10));_tmp10->hd=(void*)((void*)x->hd);_tmp10->tl=y;
_tmp10;});x=x->tl;}return y;}struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*
x,struct Cyc_List_List*y){return Cyc_List_rrevappend(Cyc_Core_heap_region,x,y);}
struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*r2,struct Cyc_List_List*x){
if(x == 0)return 0;return Cyc_List_rrevappend(r2,x,0);}struct Cyc_List_List*Cyc_List_rev(
struct Cyc_List_List*x){return Cyc_List_rrev(Cyc_Core_heap_region,x);}struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x){if(x == 0)return x;else{struct Cyc_List_List*
first=x;struct Cyc_List_List*second=x->tl;x->tl=0;while(second != 0){struct Cyc_List_List*
temp=second->tl;second->tl=first;first=second;second=temp;}return first;}}struct
Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*r2,struct Cyc_List_List*x,
struct Cyc_List_List*y){struct Cyc_List_List*result;struct Cyc_List_List*prev;if(x
== 0)return y;if(y == 0)return Cyc_List_rcopy(r2,x);result=({struct Cyc_List_List*
_tmp11=_region_malloc(r2,sizeof(*_tmp11));_tmp11->hd=(void*)((void*)x->hd);
_tmp11->tl=0;_tmp11;});prev=result;for(x=x->tl;x != 0;x=x->tl){struct Cyc_List_List*
temp=({struct Cyc_List_List*_tmp12=_region_malloc(r2,sizeof(*_tmp12));_tmp12->hd=(
void*)((void*)x->hd);_tmp12->tl=0;_tmp12;});prev->tl=(struct Cyc_List_List*)temp;
prev=temp;}prev->tl=y;return(struct Cyc_List_List*)result;}struct Cyc_List_List*
Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y){return Cyc_List_rappend(
Cyc_Core_heap_region,x,y);}struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*
x,struct Cyc_List_List*y){struct Cyc_List_List*z;if(x == 0)return y;if(y == 0)return x;
for(z=x;z->tl != 0;z=z->tl){;}z->tl=y;return x;}struct Cyc_List_List*Cyc_List_rflatten(
struct _RegionHandle*r3,struct Cyc_List_List*x){return((struct Cyc_List_List*(*)(
struct Cyc_List_List*(*f)(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*),
struct _RegionHandle*env,struct Cyc_List_List*x,struct Cyc_List_List*accum))Cyc_List_fold_right_c)(
Cyc_List_rappend,r3,x,0);}struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*
x){return Cyc_List_rflatten(Cyc_Core_heap_region,x);}struct Cyc_List_List*Cyc_List_imp_merge(
int(*less_eq)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*b){struct
Cyc_List_List*c;struct Cyc_List_List*d;if(a == 0)return b;if(b == 0)return a;if(
less_eq((void*)a->hd,(void*)b->hd)<= 0){c=a;a=a->tl;}else{c=b;b=b->tl;}d=c;
while(a != 0  && b != 0){if(less_eq((void*)a->hd,(void*)b->hd)<= 0){c->tl=a;c=a;a=a->tl;}
else{c->tl=b;c=b;b=b->tl;}}if(a == 0)c->tl=b;else{c->tl=a;}return d;}struct Cyc_List_List*
Cyc_List_rimp_merge_sort(int(*less_eq)(void*,void*),struct Cyc_List_List*x){if(x
== 0)return x;{struct Cyc_List_List*temp=x->tl;if(temp == 0)return x;{struct Cyc_List_List*
a=(struct Cyc_List_List*)x;struct Cyc_List_List*aptr=a;struct Cyc_List_List*b=(
struct Cyc_List_List*)temp;struct Cyc_List_List*bptr=b;x=b->tl;while(x != 0){aptr->tl=
x;aptr=(struct Cyc_List_List*)x;x=x->tl;if(x != 0){bptr->tl=x;bptr=(struct Cyc_List_List*)
x;x=x->tl;}}aptr->tl=0;bptr->tl=0;return Cyc_List_imp_merge(less_eq,Cyc_List_rimp_merge_sort(
less_eq,(struct Cyc_List_List*)a),Cyc_List_rimp_merge_sort(less_eq,(struct Cyc_List_List*)
b));}}}struct Cyc_List_List*Cyc_List_rmerge_sort(struct _RegionHandle*r2,int(*
less_eq)(void*,void*),struct Cyc_List_List*x){return Cyc_List_rimp_merge_sort(
less_eq,Cyc_List_rcopy(r2,x));}struct Cyc_List_List*Cyc_List_rmerge(struct
_RegionHandle*r3,int(*less_eq)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*
b){struct Cyc_List_List*c;struct Cyc_List_List*d;if(a == 0)return Cyc_List_rcopy(r3,
b);if(b == 0)return Cyc_List_rcopy(r3,a);if(less_eq((void*)a->hd,(void*)b->hd)<= 0){
c=({struct Cyc_List_List*_tmp13=_region_malloc(r3,sizeof(*_tmp13));_tmp13->hd=(
void*)((void*)a->hd);_tmp13->tl=0;_tmp13;});a=a->tl;}else{c=({struct Cyc_List_List*
_tmp14=_region_malloc(r3,sizeof(*_tmp14));_tmp14->hd=(void*)((void*)b->hd);
_tmp14->tl=0;_tmp14;});b=b->tl;}d=c;while(a != 0  && b != 0){struct Cyc_List_List*
temp;if(less_eq((void*)a->hd,(void*)b->hd)<= 0){temp=({struct Cyc_List_List*
_tmp15=_region_malloc(r3,sizeof(*_tmp15));_tmp15->hd=(void*)((void*)a->hd);
_tmp15->tl=0;_tmp15;});c->tl=(struct Cyc_List_List*)temp;c=temp;a=a->tl;}else{
temp=({struct Cyc_List_List*_tmp16=_region_malloc(r3,sizeof(*_tmp16));_tmp16->hd=(
void*)((void*)b->hd);_tmp16->tl=0;_tmp16;});c->tl=(struct Cyc_List_List*)temp;c=
temp;b=b->tl;}}if(a == 0)c->tl=Cyc_List_rcopy(r3,b);else{c->tl=Cyc_List_rcopy(r3,
a);}return(struct Cyc_List_List*)d;}struct Cyc_List_List*Cyc_List_merge_sort(int(*
less_eq)(void*,void*),struct Cyc_List_List*x){return Cyc_List_rmerge_sort(Cyc_Core_heap_region,
less_eq,x);}struct Cyc_List_List*Cyc_List_merge(int(*less_eq)(void*,void*),struct
Cyc_List_List*a,struct Cyc_List_List*b){return Cyc_List_rmerge(Cyc_Core_heap_region,
less_eq,a,b);}char Cyc_List_Nth[8]="\000\000\000\000Nth\000";void*Cyc_List_nth(
struct Cyc_List_List*x,int i){while(i > 0  && x != 0){-- i;x=x->tl;}if(i < 0  || x == 0)(
int)_throw((void*)Cyc_List_Nth);return(void*)x->hd;}struct Cyc_List_List*Cyc_List_nth_tail(
struct Cyc_List_List*x,int i){if(i < 0)(int)_throw((void*)Cyc_List_Nth);while(i != 0){
if(x == 0)(int)_throw((void*)Cyc_List_Nth);x=x->tl;-- i;}return x;}int Cyc_List_forall(
int(*pred)(void*),struct Cyc_List_List*x){while(x != 0  && pred((void*)x->hd)){x=x->tl;}
return x == 0;}int Cyc_List_forall_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*
x){while(x != 0  && pred(env,(void*)x->hd)){x=x->tl;}return x == 0;}int Cyc_List_exists(
int(*pred)(void*),struct Cyc_List_List*x){while(x != 0  && !pred((void*)x->hd)){x=x->tl;}
return x != 0;}int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*
x){while(x != 0  && !pred(env,(void*)x->hd)){x=x->tl;}return x != 0;}struct _tuple2{
void*f1;void*f2;};struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r3,
struct _RegionHandle*r4,struct Cyc_List_List*x,struct Cyc_List_List*y){struct Cyc_List_List*
result;struct Cyc_List_List*prev;struct Cyc_List_List*temp;if(x == 0  && y == 0)
return 0;if(x == 0  || y == 0)(int)_throw((void*)Cyc_List_List_mismatch);result=({
struct Cyc_List_List*_tmp17=_region_malloc(r3,sizeof(*_tmp17));_tmp17->hd=({
struct _tuple2*_tmp18=_region_malloc(r4,sizeof(*_tmp18));_tmp18->f1=(void*)x->hd;
_tmp18->f2=(void*)y->hd;_tmp18;});_tmp17->tl=0;_tmp17;});prev=result;x=x->tl;y=y->tl;
while(x != 0  && y != 0){temp=({struct Cyc_List_List*_tmp19=_region_malloc(r3,
sizeof(*_tmp19));_tmp19->hd=({struct _tuple2*_tmp1A=_region_malloc(r4,sizeof(*
_tmp1A));_tmp1A->f1=(void*)x->hd;_tmp1A->f2=(void*)y->hd;_tmp1A;});_tmp19->tl=0;
_tmp19;});prev->tl=(struct Cyc_List_List*)temp;prev=temp;x=x->tl;y=y->tl;}if(x != 
0  || y != 0)(int)_throw((void*)Cyc_List_List_mismatch);return(struct Cyc_List_List*)
result;}struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*
y){return Cyc_List_rzip(Cyc_Core_heap_region,Cyc_Core_heap_region,x,y);}struct
_tuple3{void*f1;void*f2;void*f3;};struct Cyc_List_List*Cyc_List_rzip3(struct
_RegionHandle*r3,struct _RegionHandle*r4,struct Cyc_List_List*x,struct Cyc_List_List*
y,struct Cyc_List_List*z){struct Cyc_List_List*result;struct Cyc_List_List*prev;
struct Cyc_List_List*temp;if((x == 0  && y == 0) && z == 0)return 0;if((x == 0  || y == 0)
 || z == 0)(int)_throw((void*)Cyc_List_List_mismatch);result=({struct Cyc_List_List*
_tmp1B=_region_malloc(r3,sizeof(*_tmp1B));_tmp1B->hd=({struct _tuple3*_tmp1C=
_region_malloc(r4,sizeof(*_tmp1C));_tmp1C->f1=(void*)x->hd;_tmp1C->f2=(void*)y->hd;
_tmp1C->f3=(void*)z->hd;_tmp1C;});_tmp1B->tl=0;_tmp1B;});prev=result;x=x->tl;y=y->tl;
z=z->tl;while((x != 0  && y != 0) && z != 0){temp=({struct Cyc_List_List*_tmp1D=
_region_malloc(r3,sizeof(*_tmp1D));_tmp1D->hd=({struct _tuple3*_tmp1E=
_region_malloc(r4,sizeof(*_tmp1E));_tmp1E->f1=(void*)x->hd;_tmp1E->f2=(void*)y->hd;
_tmp1E->f3=(void*)z->hd;_tmp1E;});_tmp1D->tl=0;_tmp1D;});prev->tl=(struct Cyc_List_List*)
temp;prev=temp;x=x->tl;y=y->tl;z=z->tl;}if((x != 0  || y != 0) || z != 0)(int)_throw((
void*)Cyc_List_List_mismatch);return(struct Cyc_List_List*)result;}struct Cyc_List_List*
Cyc_List_zip3(struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z){
return Cyc_List_rzip3(Cyc_Core_heap_region,Cyc_Core_heap_region,x,y,z);}struct
_tuple0 Cyc_List_rsplit(struct _RegionHandle*r3,struct _RegionHandle*r4,struct Cyc_List_List*
x){struct Cyc_List_List*result1;struct Cyc_List_List*prev1;struct Cyc_List_List*
temp1;struct Cyc_List_List*result2;struct Cyc_List_List*prev2;struct Cyc_List_List*
temp2;if(x == 0)return({struct _tuple0 _tmp1F;_tmp1F.f1=0;_tmp1F.f2=0;_tmp1F;});
prev1=(result1=({struct Cyc_List_List*_tmp20=_region_malloc(r3,sizeof(*_tmp20));
_tmp20->hd=(void*)(((struct _tuple2*)x->hd)[_check_known_subscript_notnull(1,0)]).f1;
_tmp20->tl=0;_tmp20;}));prev2=(result2=({struct Cyc_List_List*_tmp21=
_region_malloc(r4,sizeof(*_tmp21));_tmp21->hd=(void*)(((struct _tuple2*)x->hd)[
_check_known_subscript_notnull(1,0)]).f2;_tmp21->tl=0;_tmp21;}));for(x=x->tl;x != 
0;x=x->tl){temp1=({struct Cyc_List_List*_tmp22=_region_malloc(r3,sizeof(*_tmp22));
_tmp22->hd=(void*)(((struct _tuple2*)x->hd)[_check_known_subscript_notnull(1,0)]).f1;
_tmp22->tl=0;_tmp22;});temp2=({struct Cyc_List_List*_tmp23=_region_malloc(r4,
sizeof(*_tmp23));_tmp23->hd=(void*)(((struct _tuple2*)x->hd)[
_check_known_subscript_notnull(1,0)]).f2;_tmp23->tl=0;_tmp23;});prev1->tl=(
struct Cyc_List_List*)temp1;prev2->tl=(struct Cyc_List_List*)temp2;prev1=temp1;
prev2=temp2;}return({struct _tuple0 _tmp24;_tmp24.f1=(struct Cyc_List_List*)result1;
_tmp24.f2=(struct Cyc_List_List*)result2;_tmp24;});}struct _tuple0 Cyc_List_split(
struct Cyc_List_List*x){return Cyc_List_rsplit(Cyc_Core_heap_region,Cyc_Core_heap_region,
x);}struct _tuple1 Cyc_List_rsplit3(struct _RegionHandle*r3,struct _RegionHandle*r4,
struct _RegionHandle*r5,struct Cyc_List_List*x){struct Cyc_List_List*result1;struct
Cyc_List_List*prev1;struct Cyc_List_List*temp1;struct Cyc_List_List*result2;struct
Cyc_List_List*prev2;struct Cyc_List_List*temp2;struct Cyc_List_List*result3;struct
Cyc_List_List*prev3;struct Cyc_List_List*temp3;if(x == 0)return({struct _tuple1
_tmp25;_tmp25.f1=0;_tmp25.f2=0;_tmp25.f3=0;_tmp25;});prev1=(result1=({struct Cyc_List_List*
_tmp26=_region_malloc(r3,sizeof(*_tmp26));_tmp26->hd=(void*)(((struct _tuple3*)x->hd)[
_check_known_subscript_notnull(1,0)]).f1;_tmp26->tl=0;_tmp26;}));prev2=(result2=({
struct Cyc_List_List*_tmp27=_region_malloc(r4,sizeof(*_tmp27));_tmp27->hd=(void*)(((
struct _tuple3*)x->hd)[_check_known_subscript_notnull(1,0)]).f2;_tmp27->tl=0;
_tmp27;}));prev3=(result3=({struct Cyc_List_List*_tmp28=_region_malloc(r5,sizeof(*
_tmp28));_tmp28->hd=(void*)(((struct _tuple3*)x->hd)[
_check_known_subscript_notnull(1,0)]).f3;_tmp28->tl=0;_tmp28;}));for(x=x->tl;x != 
0;x=x->tl){temp1=({struct Cyc_List_List*_tmp29=_region_malloc(r3,sizeof(*_tmp29));
_tmp29->hd=(void*)(((struct _tuple3*)x->hd)[_check_known_subscript_notnull(1,0)]).f1;
_tmp29->tl=0;_tmp29;});temp2=({struct Cyc_List_List*_tmp2A=_region_malloc(r4,
sizeof(*_tmp2A));_tmp2A->hd=(void*)(((struct _tuple3*)x->hd)[
_check_known_subscript_notnull(1,0)]).f2;_tmp2A->tl=0;_tmp2A;});temp3=({struct
Cyc_List_List*_tmp2B=_region_malloc(r5,sizeof(*_tmp2B));_tmp2B->hd=(void*)(((
struct _tuple3*)x->hd)[_check_known_subscript_notnull(1,0)]).f3;_tmp2B->tl=0;
_tmp2B;});prev1->tl=(struct Cyc_List_List*)temp1;prev2->tl=(struct Cyc_List_List*)
temp2;prev3->tl=(struct Cyc_List_List*)temp3;prev1=temp1;prev2=temp2;prev3=temp3;}
return({struct _tuple1 _tmp2C;_tmp2C.f1=(struct Cyc_List_List*)result1;_tmp2C.f2=(
struct Cyc_List_List*)result2;_tmp2C.f3=(struct Cyc_List_List*)result3;_tmp2C;});}
struct _tuple1 Cyc_List_split3(struct Cyc_List_List*x){return Cyc_List_rsplit3(Cyc_Core_heap_region,
Cyc_Core_heap_region,Cyc_Core_heap_region,x);}int Cyc_List_memq(struct Cyc_List_List*
l,void*x){while(l != 0){if((void*)l->hd == x)return 1;l=l->tl;}return 0;}int Cyc_List_mem(
int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x){while(l != 0){if(cmp((void*)
l->hd,x)== 0)return 1;l=l->tl;}return 0;}void*Cyc_List_assoc(struct Cyc_List_List*l,
void*x){while(l != 0){if((((struct _tuple2*)l->hd)[_check_known_subscript_notnull(
1,0)]).f1 == x)return(((struct _tuple2*)l->hd)[_check_known_subscript_notnull(1,0)]).f2;
l=l->tl;}(int)_throw((void*)Cyc_Core_Not_found);}void*Cyc_List_assoc_cmp(int(*
cmp)(void*,void*),struct Cyc_List_List*l,void*x){while(l != 0){if(cmp((*((struct
_tuple2*)l->hd)).f1,x)== 0)return(*((struct _tuple2*)l->hd)).f2;l=l->tl;}(int)
_throw((void*)Cyc_Core_Not_found);}struct Cyc_List_List*Cyc_List_delete_cmp(int(*
cmp)(void*,void*),struct Cyc_List_List*l,void*x){struct Cyc_List_List*prev=0;
struct Cyc_List_List*_tmp2D=l;while(_tmp2D != 0){if(cmp((void*)_tmp2D->hd,x)== 0){
if(prev == 0)return((struct Cyc_List_List*)_check_null(l))->tl;else{prev->tl=
_tmp2D->tl;return l;}}prev=_tmp2D;_tmp2D=_tmp2D->tl;}(int)_throw((void*)Cyc_Core_Not_found);}
static int Cyc_List_ptrequal(void*x,void*y){return x == y;}struct Cyc_List_List*Cyc_List_delete(
struct Cyc_List_List*l,void*x){return Cyc_List_delete_cmp(Cyc_List_ptrequal,l,x);}
int Cyc_List_mem_assoc(struct Cyc_List_List*l,void*x){while(l != 0){if((((struct
_tuple2*)l->hd)[_check_known_subscript_notnull(1,0)]).f1 == x)return 1;l=l->tl;}
return 0;}struct Cyc_Core_Opt*Cyc_List_check_unique(int(*cmp)(void*,void*),struct
Cyc_List_List*x){while(x != 0){struct Cyc_List_List*_tmp2E=x->tl;if(_tmp2E != 0){
if(cmp((void*)x->hd,(void*)_tmp2E->hd)== 0)return({struct Cyc_Core_Opt*_tmp2F=
_cycalloc(sizeof(*_tmp2F));_tmp2F->v=(void*)((void*)x->hd);_tmp2F;});}x=_tmp2E;}
return 0;}struct _dynforward_ptr Cyc_List_rto_array(struct _RegionHandle*r2,struct
Cyc_List_List*x){int s;struct _dynforward_ptr arr;s=Cyc_List_length(x);arr=({
unsigned int _tmp30=(unsigned int)s;void**_tmp31=(void**)_region_malloc(r2,
_check_times(sizeof(void*),_tmp30));struct _dynforward_ptr _tmp34=_tag_dynforward(
_tmp31,sizeof(void*),_tmp30);{unsigned int _tmp32=_tmp30;unsigned int i;for(i=0;i < 
_tmp32;i ++){_tmp31[i]=({void*_tmp33=(void*)((struct Cyc_List_List*)_check_null(x))->hd;
x=x->tl;_tmp33;});}}_tmp34;});return arr;}struct _dynforward_ptr Cyc_List_to_array(
struct Cyc_List_List*x){return Cyc_List_rto_array(Cyc_Core_heap_region,x);}struct
Cyc_List_List*Cyc_List_rfrom_array(struct _RegionHandle*r2,struct _dynforward_ptr
arr){struct Cyc_List_List*ans=0;{int i=(int)(_get_dynforward_size(arr,sizeof(void*))
- 1);for(0;i >= 0;-- i){ans=({struct Cyc_List_List*_tmp35=_region_malloc(r2,sizeof(*
_tmp35));_tmp35->hd=(void*)*((void**)_check_dynforward_subscript(arr,sizeof(void*),
i));_tmp35->tl=ans;_tmp35;});}}return ans;}struct Cyc_List_List*Cyc_List_from_array(
struct _dynforward_ptr arr){return Cyc_List_rfrom_array(Cyc_Core_heap_region,arr);}
struct Cyc_List_List*Cyc_List_rtabulate(struct _RegionHandle*r,int n,void*(*f)(int)){
struct Cyc_List_List*res=0;{int i=0;for(0;i < n;++ i){res=({struct Cyc_List_List*
_tmp36=_region_malloc(r,sizeof(*_tmp36));_tmp36->hd=(void*)f(i);_tmp36->tl=res;
_tmp36;});}}return Cyc_List_imp_rev(res);}struct Cyc_List_List*Cyc_List_tabulate(
int n,void*(*f)(int)){return Cyc_List_rtabulate(Cyc_Core_heap_region,n,f);}struct
Cyc_List_List*Cyc_List_rtabulate_c(struct _RegionHandle*r,int n,void*(*f)(void*,
int),void*env){struct Cyc_List_List*res=0;{int i=0;for(0;i < n;++ i){res=({struct Cyc_List_List*
_tmp37=_region_malloc(r,sizeof(*_tmp37));_tmp37->hd=(void*)f(env,i);_tmp37->tl=
res;_tmp37;});}}return Cyc_List_imp_rev(res);}struct Cyc_List_List*Cyc_List_tabulate_c(
int n,void*(*f)(void*,int),void*env){return Cyc_List_rtabulate_c(Cyc_Core_heap_region,
n,f,env);}int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,
struct Cyc_List_List*l2){for(0;l1 != 0  && l2 != 0;(l1=l1->tl,l2=l2->tl)){if((
unsigned int)l1 == (unsigned int)l2)return 0;{int _tmp38=cmp((void*)l1->hd,(void*)
l2->hd);if(_tmp38 != 0)return _tmp38;}}if(l1 != 0)return 1;if(l2 != 0)return - 1;return
0;}int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct
Cyc_List_List*l2){for(0;l1 != 0  && l2 != 0;(l1=l1->tl,l2=l2->tl)){if((unsigned int)
l1 == (unsigned int)l2)return 1;{int _tmp39=cmp((void*)l1->hd,(void*)l2->hd);if(
_tmp39 != 0)return 0;}}return l1 == 0;}struct Cyc_List_List*Cyc_List_rfilter_c(struct
_RegionHandle*r,int(*f)(void*,void*),void*env,struct Cyc_List_List*l){if(l == 0)
return 0;{struct Cyc_List_List*result=({struct Cyc_List_List*_tmp3B=_region_malloc(
r,sizeof(*_tmp3B));_tmp3B->hd=(void*)((void*)l->hd);_tmp3B->tl=0;_tmp3B;});
struct Cyc_List_List*end=result;struct Cyc_List_List*temp;for(0;l != 0;l=l->tl){if(
f(env,(void*)l->hd)){temp=({struct Cyc_List_List*_tmp3A=_region_malloc(r,sizeof(*
_tmp3A));_tmp3A->hd=(void*)((void*)l->hd);_tmp3A->tl=0;_tmp3A;});end->tl=(struct
Cyc_List_List*)temp;end=temp;}}return result->tl;}}struct Cyc_List_List*Cyc_List_filter_c(
int(*f)(void*,void*),void*env,struct Cyc_List_List*l){return Cyc_List_rfilter_c(
Cyc_Core_heap_region,f,env,l);}struct Cyc_List_List*Cyc_List_rfilter(struct
_RegionHandle*r,int(*f)(void*),struct Cyc_List_List*l){if(l == 0)return 0;{struct
Cyc_List_List*result=({struct Cyc_List_List*_tmp3D=_region_malloc(r,sizeof(*
_tmp3D));_tmp3D->hd=(void*)((void*)l->hd);_tmp3D->tl=0;_tmp3D;});struct Cyc_List_List*
end=result;struct Cyc_List_List*temp;for(0;l != 0;l=l->tl){if(f((void*)l->hd)){
temp=({struct Cyc_List_List*_tmp3C=_region_malloc(r,sizeof(*_tmp3C));_tmp3C->hd=(
void*)((void*)l->hd);_tmp3C->tl=0;_tmp3C;});end->tl=(struct Cyc_List_List*)temp;
end=temp;}}return result->tl;}}struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),
struct Cyc_List_List*l){return Cyc_List_rfilter(Cyc_Core_heap_region,f,l);}
