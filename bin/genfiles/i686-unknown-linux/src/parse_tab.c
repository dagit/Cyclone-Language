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
16];extern char Cyc_Core_Free_Region[16];typedef struct{int __count;union{
unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;typedef struct{long
__pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;
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
18];struct Cyc_FileOpenError_struct{char*tag;struct _dynforward_ptr f1;};extern char
Cyc_Lexing_Error[10];struct Cyc_Lexing_Error_struct{char*tag;struct
_dynforward_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);
void*refill_state;struct _dynforward_ptr lex_buffer;int lex_buffer_len;int
lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;
int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(
struct _dynforward_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{
struct _dynforward_ptr lex_base;struct _dynforward_ptr lex_backtrk;struct
_dynforward_ptr lex_default;struct _dynforward_ptr lex_trans;struct _dynforward_ptr
lex_check;};struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};int Cyc_List_length(struct
Cyc_List_List*x);struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*
x);struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];void Cyc_List_iter(void(*f)(void*),struct
Cyc_List_List*x);void*Cyc_List_fold_right(void*(*f)(void*,void*),struct Cyc_List_List*
x,void*accum);struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_flatten(
struct Cyc_List_List*x);extern char Cyc_List_Nth[8];struct Cyc_List_List*Cyc_List_rzip(
struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _tuple0
Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x);unsigned int Cyc_strlen(struct _dynforward_ptr s);int Cyc_strcmp(struct
_dynforward_ptr s1,struct _dynforward_ptr s2);int Cyc_zstrcmp(struct _dynforward_ptr,
struct _dynforward_ptr);int Cyc_zstrptrcmp(struct _dynforward_ptr*,struct
_dynforward_ptr*);struct _dynforward_ptr Cyc_strcat(struct _dynforward_ptr dest,
struct _dynforward_ptr src);struct _dynforward_ptr Cyc_strcpy(struct _dynforward_ptr
dest,struct _dynforward_ptr src);struct _dynforward_ptr Cyc_substring(struct
_dynforward_ptr,int ofs,unsigned int n);struct Cyc_Lineno_Pos{struct _dynforward_ptr
logical_file;struct _dynforward_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[
9];struct Cyc_Position_Segment;struct Cyc_Position_Segment*Cyc_Position_segment_of_abs(
int,int);struct _dynforward_ptr Cyc_Position_string_of_segment(struct Cyc_Position_Segment*);
struct Cyc_Position_Error{struct _dynforward_ptr source;struct Cyc_Position_Segment*
seg;void*kind;struct _dynforward_ptr desc;};struct Cyc_Position_Error*Cyc_Position_mk_err_parse(
struct Cyc_Position_Segment*,struct _dynforward_ptr);extern char Cyc_Position_Nocontext[
14];void Cyc_Position_post_error(struct Cyc_Position_Error*);struct Cyc_Absyn_Loc_n_struct{
int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct
Loc_n;struct Cyc_Absyn_Rel_n_struct Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};
struct _tuple1{union Cyc_Absyn_Nmspace_union f1;struct _dynforward_ptr*f2;};struct
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
_dynforward_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_is_qvar_qualified(
struct _tuple1*);struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual
x,struct Cyc_Absyn_Tqual y);struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Conref*Cyc_Absyn_new_conref(void*x);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_one_conref;
extern struct Cyc_Absyn_Conref*Cyc_Absyn_bounds_dynforward_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_bounds_dyneither_conref;void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(
struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(void*,void*);extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_float_typ;void*Cyc_Absyn_double_typ(
int);void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,
struct Cyc_Absyn_Conref*zero_term);void*Cyc_Absyn_array_typ(void*elt_type,struct
Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,struct Cyc_Absyn_Conref*zero_term,
struct Cyc_Position_Segment*ztloc);struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*
rgn_handle,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_null_exp(struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*,int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dynforward_ptr f,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dynforward_ptr s,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct
Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,
struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(
struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct
Cyc_Absyn_Exp*,int user_cast,void*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*
Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_gentyp_exp(struct Cyc_List_List*,void*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct
_dynforward_ptr*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(
struct Cyc_Absyn_Exp*,struct _dynforward_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct
Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,struct Cyc_Position_Segment*loc);struct
Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(struct
Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,
struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(
struct Cyc_List_List*el,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dynforward_ptr*lab,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct
Cyc_List_List*,struct Cyc_Position_Segment*);struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(
void*p,struct Cyc_Position_Segment*s);struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(
struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*
e,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(
struct Cyc_List_List*,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_AggrdeclImpl*
Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct
Cyc_List_List*fs);struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(void*k,void*s,struct
_tuple1*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*
atts,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_tunion_decl(
void*s,struct _tuple1*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int
is_xtunion,int is_flat,struct Cyc_Position_Segment*loc);void*Cyc_Absyn_function_typ(
struct Cyc_List_List*tvs,struct Cyc_Core_Opt*eff_typ,void*ret_typ,struct Cyc_List_List*
args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*
rgn_po,struct Cyc_List_List*atts);int Cyc_Absyn_fntype_att(void*a);extern int Cyc_Absyn_porting_c_code;
struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};int Cyc_Iter_next(
struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[11];struct
Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;
struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern char Cyc_Dict_Absent[11];
struct _tuple4{void*f1;void*f2;};struct _tuple4*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple4*Cyc_Dict_rchoose(struct
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
le;};void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,struct _dynforward_ptr fmt,
struct _dynforward_ptr ap);void*Cyc_Tcutil_copy_type(void*t);void*Cyc_Tcutil_compress(
void*t);extern struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_trk;
extern struct Cyc_Core_Opt Cyc_Tcutil_bk;extern struct Cyc_Core_Opt Cyc_Tcutil_ik;
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(void*k);void*Cyc_Tcutil_kind_to_bound(
void*k);struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);struct Cyc_PP_Ppstate;
struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;
int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int
rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;int
print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Lex_register_typedef(
struct _tuple1*s);void Cyc_Lex_enter_namespace(struct _dynforward_ptr*);void Cyc_Lex_leave_namespace();
void Cyc_Lex_enter_using(struct _tuple1*);void Cyc_Lex_leave_using();int Cyc_Parse_no_register=
0;struct Cyc_Parse_Signed_spec_struct{int tag;struct Cyc_Position_Segment*f1;};
struct Cyc_Parse_Unsigned_spec_struct{int tag;struct Cyc_Position_Segment*f1;};
struct Cyc_Parse_Short_spec_struct{int tag;struct Cyc_Position_Segment*f1;};struct
Cyc_Parse_Long_spec_struct{int tag;struct Cyc_Position_Segment*f1;};struct Cyc_Parse_Type_spec_struct{
int tag;void*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Parse_Decl_spec_struct{
int tag;struct Cyc_Absyn_Decl*f1;};struct Cyc_Parse_Declaration_spec{struct Cyc_Core_Opt*
sc;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*type_specs;int is_inline;struct Cyc_List_List*
attributes;};struct Cyc_Parse_Declarator{struct _tuple1*id;struct Cyc_List_List*tms;
};struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};struct _tuple5{
void*f1;struct Cyc_Core_Opt*f2;};static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc);struct _tuple6{struct Cyc_Absyn_Tqual
f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};static struct _tuple6
Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
static struct Cyc_List_List*Cyc_Parse_parse_result=0;static void Cyc_Parse_err(
struct _dynforward_ptr msg,struct Cyc_Position_Segment*sg){Cyc_Position_post_error(
Cyc_Position_mk_err_parse(sg,msg));}static void*Cyc_Parse_abort(struct Cyc_Position_Segment*
sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap){Cyc_Parse_err((struct
_dynforward_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap),sg);(int)_throw((void*)
Cyc_Position_Exit);}static void Cyc_Parse_unimp(struct _dynforward_ptr msg,struct Cyc_Position_Segment*
sg){({struct Cyc_String_pa_struct _tmp3;_tmp3.tag=0;_tmp3.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)msg);{struct Cyc_String_pa_struct _tmp2;
_tmp2.tag=0;_tmp2.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_Position_string_of_segment(
sg));{void*_tmp0[2]={& _tmp2,& _tmp3};Cyc_fprintf(Cyc_stderr,({const char*_tmp1="%s: Warning: Cyclone does not yet support %s\n";
_tag_dynforward(_tmp1,sizeof(char),_get_zero_arr_size_char(_tmp1,46));}),
_tag_dynforward(_tmp0,sizeof(void*),2));}}});return;}static int Cyc_Parse_enum_counter=
0;struct _tuple1*Cyc_Parse_gensym_enum(){return({struct _tuple1*_tmp4=_cycalloc(
sizeof(*_tmp4));_tmp4->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp9;(_tmp9.Rel_n).tag=1;(_tmp9.Rel_n).f1=0;_tmp9;});_tmp4->f2=({struct
_dynforward_ptr*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5[0]=(struct _dynforward_ptr)({
struct Cyc_Int_pa_struct _tmp8;_tmp8.tag=1;_tmp8.f1=(unsigned long)Cyc_Parse_enum_counter
++;{void*_tmp6[1]={& _tmp8};Cyc_aprintf(({const char*_tmp7="__anonymous_enum_%d__";
_tag_dynforward(_tmp7,sizeof(char),_get_zero_arr_size_char(_tmp7,22));}),
_tag_dynforward(_tmp6,sizeof(void*),1));}});_tmp5;});_tmp4;});}struct _tuple7{
struct _tuple1*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct
Cyc_List_List*f5;};struct _tuple8{struct _tuple7*f1;struct Cyc_Absyn_Exp*f2;};
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(struct Cyc_Position_Segment*
loc,struct _tuple8*field_info){struct _tuple8 _tmpB;struct _tuple7*_tmpC;struct
_tuple7 _tmpD;struct _tuple1*_tmpE;struct Cyc_Absyn_Tqual _tmpF;void*_tmp10;struct
Cyc_List_List*_tmp11;struct Cyc_List_List*_tmp12;struct Cyc_Absyn_Exp*_tmp13;
struct _tuple8*_tmpA=field_info;_tmpB=*_tmpA;_tmpC=_tmpB.f1;_tmpD=*_tmpC;_tmpE=
_tmpD.f1;_tmpF=_tmpD.f2;_tmp10=_tmpD.f3;_tmp11=_tmpD.f4;_tmp12=_tmpD.f5;_tmp13=
_tmpB.f2;if(_tmp11 != 0)Cyc_Parse_err(({const char*_tmp14="bad type params in struct field";
_tag_dynforward(_tmp14,sizeof(char),_get_zero_arr_size_char(_tmp14,32));}),loc);
if(Cyc_Absyn_is_qvar_qualified(_tmpE))Cyc_Parse_err(({const char*_tmp15="struct or union field cannot be qualified with a namespace";
_tag_dynforward(_tmp15,sizeof(char),_get_zero_arr_size_char(_tmp15,59));}),loc);
return({struct Cyc_Absyn_Aggrfield*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16->name=(*
_tmpE).f2;_tmp16->tq=_tmpF;_tmp16->type=(void*)_tmp10;_tmp16->width=_tmp13;
_tmp16->attributes=_tmp12;_tmp16;});}static void*Cyc_Parse_type_spec(void*t,
struct Cyc_Position_Segment*loc){return(void*)({struct Cyc_Parse_Type_spec_struct*
_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17[0]=({struct Cyc_Parse_Type_spec_struct
_tmp18;_tmp18.tag=4;_tmp18.f1=(void*)t;_tmp18.f2=loc;_tmp18;});_tmp17;});}static
void*Cyc_Parse_array2ptr(void*t,int argposn){void*_tmp19=t;struct Cyc_Absyn_ArrayInfo
_tmp1A;void*_tmp1B;struct Cyc_Absyn_Tqual _tmp1C;struct Cyc_Absyn_Exp*_tmp1D;struct
Cyc_Absyn_Conref*_tmp1E;struct Cyc_Position_Segment*_tmp1F;_LL1: if(_tmp19 <= (void*)
4)goto _LL3;if(*((int*)_tmp19)!= 7)goto _LL3;_tmp1A=((struct Cyc_Absyn_ArrayType_struct*)
_tmp19)->f1;_tmp1B=(void*)_tmp1A.elt_type;_tmp1C=_tmp1A.tq;_tmp1D=_tmp1A.num_elts;
_tmp1E=_tmp1A.zero_term;_tmp1F=_tmp1A.zt_loc;_LL2: return Cyc_Absyn_starb_typ(
_tmp1B,argposn?Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,0):(void*)
2,_tmp1C,_tmp1D == 0?(void*)0:(void*)({struct Cyc_Absyn_Upper_b_struct*_tmp20=
_cycalloc(sizeof(*_tmp20));_tmp20[0]=({struct Cyc_Absyn_Upper_b_struct _tmp21;
_tmp21.tag=0;_tmp21.f1=(struct Cyc_Absyn_Exp*)_tmp1D;_tmp21;});_tmp20;}),_tmp1E);
_LL3:;_LL4: return t;_LL0:;}static void Cyc_Parse_arg_array2ptr(struct _tuple2*x){(*x).f3=
Cyc_Parse_array2ptr((*x).f3,1);}struct _tuple9{struct Cyc_Absyn_Tqual f1;void*f2;};
static struct _tuple9*Cyc_Parse_get_tqual_typ(struct Cyc_Position_Segment*loc,
struct _tuple2*t){return({struct _tuple9*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->f1=(*
t).f2;_tmp22->f2=(*t).f3;_tmp22;});}static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(
struct Cyc_Absyn_Decl*d){void*_tmp23=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp24;
_LL6: if(_tmp23 <= (void*)2)goto _LL8;if(*((int*)_tmp23)!= 0)goto _LL8;_tmp24=((
struct Cyc_Absyn_Var_d_struct*)_tmp23)->f1;_LL7: return _tmp24;_LL8:;_LL9:({void*
_tmp25=0;((int(*)(struct Cyc_Position_Segment*sg,struct _dynforward_ptr fmt,struct
_dynforward_ptr ap))Cyc_Parse_abort)(d->loc,({const char*_tmp26="bad declaration in `forarray' statement";
_tag_dynforward(_tmp26,sizeof(char),_get_zero_arr_size_char(_tmp26,40));}),
_tag_dynforward(_tmp25,sizeof(void*),0));});_LL5:;}static int Cyc_Parse_is_typeparam(
void*tm){void*_tmp27=tm;_LLB: if(*((int*)_tmp27)!= 4)goto _LLD;_LLC: return 1;_LLD:;
_LLE: return 0;_LLA:;}static void*Cyc_Parse_id2type(struct _dynforward_ptr s,void*k){
if(Cyc_zstrcmp((struct _dynforward_ptr)s,({const char*_tmp28="`H";_tag_dynforward(
_tmp28,sizeof(char),_get_zero_arr_size_char(_tmp28,3));}))== 0)return(void*)2;
else{if(Cyc_zstrcmp((struct _dynforward_ptr)s,({const char*_tmp29="`U";
_tag_dynforward(_tmp29,sizeof(char),_get_zero_arr_size_char(_tmp29,3));}))== 0)
return(void*)3;else{return(void*)({struct Cyc_Absyn_VarType_struct*_tmp2A=
_cycalloc(sizeof(*_tmp2A));_tmp2A[0]=({struct Cyc_Absyn_VarType_struct _tmp2B;
_tmp2B.tag=1;_tmp2B.f1=({struct Cyc_Absyn_Tvar*_tmp2C=_cycalloc(sizeof(*_tmp2C));
_tmp2C->name=({struct _dynforward_ptr*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D[0]=
s;_tmp2D;});_tmp2C->identity=- 1;_tmp2C->kind=(void*)k;_tmp2C;});_tmp2B;});_tmp2A;});}}}
static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){void*k;{
void*_tmp2E=Cyc_Absyn_compress_kb((void*)t->kind);void*_tmp2F;_LL10: if(*((int*)
_tmp2E)!= 1)goto _LL12;_LL11: k=(void*)({struct Cyc_Absyn_Unknown_kb_struct*_tmp30=
_cycalloc(sizeof(*_tmp30));_tmp30[0]=({struct Cyc_Absyn_Unknown_kb_struct _tmp31;
_tmp31.tag=1;_tmp31.f1=0;_tmp31;});_tmp30;});goto _LLF;_LL12: if(*((int*)_tmp2E)!= 
2)goto _LL14;_tmp2F=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmp2E)->f2;_LL13: k=(
void*)({struct Cyc_Absyn_Less_kb_struct*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32[
0]=({struct Cyc_Absyn_Less_kb_struct _tmp33;_tmp33.tag=2;_tmp33.f1=0;_tmp33.f2=(
void*)_tmp2F;_tmp33;});_tmp32;});goto _LLF;_LL14:;_LL15: k=_tmp2E;goto _LLF;_LLF:;}
return({struct Cyc_Absyn_Tvar*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34->name=t->name;
_tmp34->identity=- 1;_tmp34->kind=(void*)k;_tmp34;});}static struct Cyc_Absyn_Tvar*
Cyc_Parse_typ2tvar(struct Cyc_Position_Segment*loc,void*t){void*_tmp35=t;struct
Cyc_Absyn_Tvar*_tmp36;_LL17: if(_tmp35 <= (void*)4)goto _LL19;if(*((int*)_tmp35)!= 
1)goto _LL19;_tmp36=((struct Cyc_Absyn_VarType_struct*)_tmp35)->f1;_LL18: return
_tmp36;_LL19:;_LL1A:({void*_tmp37=0;((int(*)(struct Cyc_Position_Segment*sg,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(loc,({const
char*_tmp38="expecting a list of type variables, not types";_tag_dynforward(
_tmp38,sizeof(char),_get_zero_arr_size_char(_tmp38,46));}),_tag_dynforward(
_tmp37,sizeof(void*),0));});_LL16:;}static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*
pr){return(void*)({struct Cyc_Absyn_VarType_struct*_tmp39=_cycalloc(sizeof(*
_tmp39));_tmp39[0]=({struct Cyc_Absyn_VarType_struct _tmp3A;_tmp3A.tag=1;_tmp3A.f1=
pr;_tmp3A;});_tmp39;});}static void Cyc_Parse_set_vartyp_kind(void*t,void*k,int leq){
void*_tmp3B=Cyc_Tcutil_compress(t);struct Cyc_Absyn_Tvar*_tmp3C;struct Cyc_Absyn_Tvar
_tmp3D;void*_tmp3E;void**_tmp3F;_LL1C: if(_tmp3B <= (void*)4)goto _LL1E;if(*((int*)
_tmp3B)!= 1)goto _LL1E;_tmp3C=((struct Cyc_Absyn_VarType_struct*)_tmp3B)->f1;
_tmp3D=*_tmp3C;_tmp3E=(void*)_tmp3D.kind;_tmp3F=(void**)&(*((struct Cyc_Absyn_VarType_struct*)
_tmp3B)->f1).kind;_LL1D: {void*_tmp40=Cyc_Absyn_compress_kb(*_tmp3F);_LL21: if(*((
int*)_tmp40)!= 1)goto _LL23;_LL22: if(!leq)*_tmp3F=Cyc_Tcutil_kind_to_bound(k);
else{*_tmp3F=(void*)({struct Cyc_Absyn_Less_kb_struct*_tmp41=_cycalloc(sizeof(*
_tmp41));_tmp41[0]=({struct Cyc_Absyn_Less_kb_struct _tmp42;_tmp42.tag=2;_tmp42.f1=
0;_tmp42.f2=(void*)k;_tmp42;});_tmp41;});}return;_LL23:;_LL24: return;_LL20:;}
_LL1E:;_LL1F: return;_LL1B:;}static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(
struct Cyc_List_List*tms,struct Cyc_List_List*tds,struct Cyc_Position_Segment*loc){
if(tds == 0)return tms;if(tms == 0)return 0;{void*_tmp43=(void*)tms->hd;void*_tmp44;
_LL26: if(*((int*)_tmp43)!= 3)goto _LL28;_tmp44=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp43)->f1;_LL27: if(tms->tl == 0  || Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 
0){void*_tmp45=_tmp44;struct Cyc_List_List*_tmp46;_LL2B: if(*((int*)_tmp45)!= 1)
goto _LL2D;_LL2C:({void*_tmp47=0;Cyc_Tcutil_warn(loc,({const char*_tmp48="function declaration with both new- and old-style parameter declarations; ignoring old-style";
_tag_dynforward(_tmp48,sizeof(char),_get_zero_arr_size_char(_tmp48,93));}),
_tag_dynforward(_tmp47,sizeof(void*),0));});return tms;_LL2D: if(*((int*)_tmp45)!= 
0)goto _LL2A;_tmp46=((struct Cyc_Absyn_NoTypes_struct*)_tmp45)->f1;_LL2E: if(((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmp46)!= ((int(*)(struct Cyc_List_List*x))
Cyc_List_length)(tds))({void*_tmp49=0;((int(*)(struct Cyc_Position_Segment*sg,
struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(loc,({const
char*_tmp4A="wrong number of parameter declarations in old-style function declaration";
_tag_dynforward(_tmp4A,sizeof(char),_get_zero_arr_size_char(_tmp4A,73));}),
_tag_dynforward(_tmp49,sizeof(void*),0));});{struct Cyc_List_List*_tmp4B=0;for(0;
_tmp46 != 0;_tmp46=_tmp46->tl){struct Cyc_List_List*_tmp4C=tds;for(0;_tmp4C != 0;
_tmp4C=_tmp4C->tl){struct Cyc_Absyn_Decl*_tmp4D=(struct Cyc_Absyn_Decl*)_tmp4C->hd;
void*_tmp4E=(void*)_tmp4D->r;struct Cyc_Absyn_Vardecl*_tmp4F;_LL30: if(_tmp4E <= (
void*)2)goto _LL32;if(*((int*)_tmp4E)!= 0)goto _LL32;_tmp4F=((struct Cyc_Absyn_Var_d_struct*)
_tmp4E)->f1;_LL31: if(Cyc_zstrptrcmp((*_tmp4F->name).f2,(struct _dynforward_ptr*)
_tmp46->hd)!= 0)continue;if(_tmp4F->initializer != 0)({void*_tmp50=0;((int(*)(
struct Cyc_Position_Segment*sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Parse_abort)(_tmp4D->loc,({const char*_tmp51="initializer found in parameter declaration";
_tag_dynforward(_tmp51,sizeof(char),_get_zero_arr_size_char(_tmp51,43));}),
_tag_dynforward(_tmp50,sizeof(void*),0));});if(Cyc_Absyn_is_qvar_qualified(
_tmp4F->name))({void*_tmp52=0;((int(*)(struct Cyc_Position_Segment*sg,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmp4D->loc,({const
char*_tmp53="namespaces forbidden in parameter declarations";_tag_dynforward(
_tmp53,sizeof(char),_get_zero_arr_size_char(_tmp53,47));}),_tag_dynforward(
_tmp52,sizeof(void*),0));});_tmp4B=({struct Cyc_List_List*_tmp54=_cycalloc(
sizeof(*_tmp54));_tmp54->hd=({struct _tuple2*_tmp55=_cycalloc(sizeof(*_tmp55));
_tmp55->f1=({struct Cyc_Core_Opt*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->v=(*
_tmp4F->name).f2;_tmp56;});_tmp55->f2=_tmp4F->tq;_tmp55->f3=(void*)_tmp4F->type;
_tmp55;});_tmp54->tl=_tmp4B;_tmp54;});goto L;_LL32:;_LL33:({void*_tmp57=0;((int(*)(
struct Cyc_Position_Segment*sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))
Cyc_Parse_abort)(_tmp4D->loc,({const char*_tmp58="nonvariable declaration in parameter type";
_tag_dynforward(_tmp58,sizeof(char),_get_zero_arr_size_char(_tmp58,42));}),
_tag_dynforward(_tmp57,sizeof(void*),0));});_LL2F:;}L: if(_tmp4C == 0)({struct Cyc_String_pa_struct
_tmp5B;_tmp5B.tag=0;_tmp5B.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((
struct _dynforward_ptr*)_tmp46->hd));{void*_tmp59[1]={& _tmp5B};((int(*)(struct Cyc_Position_Segment*
sg,struct _dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(loc,({
const char*_tmp5A="%s is not given a type";_tag_dynforward(_tmp5A,sizeof(char),
_get_zero_arr_size_char(_tmp5A,23));}),_tag_dynforward(_tmp59,sizeof(void*),1));}});}
return({struct Cyc_List_List*_tmp5C=_cycalloc(sizeof(*_tmp5C));_tmp5C->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp5D=_cycalloc(sizeof(*_tmp5D));
_tmp5D[0]=({struct Cyc_Absyn_Function_mod_struct _tmp5E;_tmp5E.tag=3;_tmp5E.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));
_tmp5F[0]=({struct Cyc_Absyn_WithTypes_struct _tmp60;_tmp60.tag=1;_tmp60.f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4B);_tmp60.f2=
0;_tmp60.f3=0;_tmp60.f4=0;_tmp60.f5=0;_tmp60;});_tmp5F;}));_tmp5E;});_tmp5D;}));
_tmp5C->tl=0;_tmp5C;});}_LL2A:;}goto _LL29;_LL28:;_LL29: return({struct Cyc_List_List*
_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->hd=(void*)((void*)tms->hd);_tmp61->tl=
Cyc_Parse_oldstyle2newstyle(tms->tl,tds,loc);_tmp61;});_LL25:;}}struct _tuple10{
struct _dynforward_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};static struct Cyc_Absyn_Fndecl*
Cyc_Parse_make_function(struct Cyc_Core_Opt*dso,struct Cyc_Parse_Declarator*d,
struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,struct Cyc_Position_Segment*loc){
if(tds != 0)d=({struct Cyc_Parse_Declarator*_tmp62=_cycalloc(sizeof(*_tmp62));
_tmp62->id=d->id;_tmp62->tms=Cyc_Parse_oldstyle2newstyle(d->tms,tds,loc);_tmp62;});{
void*sc=(void*)2;struct Cyc_List_List*tss=0;struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(
0);int is_inline=0;struct Cyc_List_List*atts=0;if(dso != 0){tss=((struct Cyc_Parse_Declaration_spec*)
dso->v)->type_specs;tq=((struct Cyc_Parse_Declaration_spec*)dso->v)->tq;is_inline=((
struct Cyc_Parse_Declaration_spec*)dso->v)->is_inline;atts=((struct Cyc_Parse_Declaration_spec*)
dso->v)->attributes;if(((struct Cyc_Parse_Declaration_spec*)dso->v)->sc != 0){void*
_tmp63=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Parse_Declaration_spec*)
dso->v)->sc))->v;_LL35: if((int)_tmp63 != 1)goto _LL37;_LL36: sc=(void*)3;goto _LL34;
_LL37: if((int)_tmp63 != 3)goto _LL39;_LL38: sc=(void*)0;goto _LL34;_LL39:;_LL3A: Cyc_Parse_err(({
const char*_tmp64="bad storage class on function";_tag_dynforward(_tmp64,sizeof(
char),_get_zero_arr_size_char(_tmp64,30));}),loc);goto _LL34;_LL34:;}}{void*
_tmp66;struct Cyc_Core_Opt*_tmp67;struct _tuple5 _tmp65=Cyc_Parse_collapse_type_specifiers(
tss,loc);_tmp66=_tmp65.f1;_tmp67=_tmp65.f2;{struct Cyc_Absyn_Tqual _tmp69;void*
_tmp6A;struct Cyc_List_List*_tmp6B;struct Cyc_List_List*_tmp6C;struct _tuple6 _tmp68=
Cyc_Parse_apply_tms(tq,_tmp66,atts,d->tms);_tmp69=_tmp68.f1;_tmp6A=_tmp68.f2;
_tmp6B=_tmp68.f3;_tmp6C=_tmp68.f4;if(_tmp67 != 0)({void*_tmp6D=0;Cyc_Tcutil_warn(
loc,({const char*_tmp6E="nested type declaration within function prototype";
_tag_dynforward(_tmp6E,sizeof(char),_get_zero_arr_size_char(_tmp6E,50));}),
_tag_dynforward(_tmp6D,sizeof(void*),0));});if(_tmp6B != 0)({void*_tmp6F=0;Cyc_Tcutil_warn(
loc,({const char*_tmp70="bad type params, ignoring";_tag_dynforward(_tmp70,
sizeof(char),_get_zero_arr_size_char(_tmp70,26));}),_tag_dynforward(_tmp6F,
sizeof(void*),0));});{void*_tmp71=_tmp6A;struct Cyc_Absyn_FnInfo _tmp72;struct Cyc_List_List*
_tmp73;struct Cyc_Core_Opt*_tmp74;void*_tmp75;struct Cyc_List_List*_tmp76;int
_tmp77;struct Cyc_Absyn_VarargInfo*_tmp78;struct Cyc_List_List*_tmp79;struct Cyc_List_List*
_tmp7A;_LL3C: if(_tmp71 <= (void*)4)goto _LL3E;if(*((int*)_tmp71)!= 8)goto _LL3E;
_tmp72=((struct Cyc_Absyn_FnType_struct*)_tmp71)->f1;_tmp73=_tmp72.tvars;_tmp74=
_tmp72.effect;_tmp75=(void*)_tmp72.ret_typ;_tmp76=_tmp72.args;_tmp77=_tmp72.c_varargs;
_tmp78=_tmp72.cyc_varargs;_tmp79=_tmp72.rgn_po;_tmp7A=_tmp72.attributes;_LL3D: {
struct Cyc_List_List*_tmp7B=0;{struct Cyc_List_List*_tmp7C=_tmp76;for(0;_tmp7C != 0;
_tmp7C=_tmp7C->tl){struct _tuple2 _tmp7E;struct Cyc_Core_Opt*_tmp7F;struct Cyc_Absyn_Tqual
_tmp80;void*_tmp81;struct _tuple2*_tmp7D=(struct _tuple2*)_tmp7C->hd;_tmp7E=*
_tmp7D;_tmp7F=_tmp7E.f1;_tmp80=_tmp7E.f2;_tmp81=_tmp7E.f3;if(_tmp7F == 0){Cyc_Parse_err(({
const char*_tmp82="missing argument variable in function prototype";
_tag_dynforward(_tmp82,sizeof(char),_get_zero_arr_size_char(_tmp82,48));}),loc);
_tmp7B=({struct Cyc_List_List*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->hd=({
struct _tuple10*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->f1=_init_dynforward_ptr(
_cycalloc(sizeof(struct _dynforward_ptr)),"?",sizeof(char),2);_tmp84->f2=_tmp80;
_tmp84->f3=_tmp81;_tmp84;});_tmp83->tl=_tmp7B;_tmp83;});}else{_tmp7B=({struct Cyc_List_List*
_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->hd=({struct _tuple10*_tmp87=_cycalloc(
sizeof(*_tmp87));_tmp87->f1=(struct _dynforward_ptr*)_tmp7F->v;_tmp87->f2=_tmp80;
_tmp87->f3=_tmp81;_tmp87;});_tmp86->tl=_tmp7B;_tmp86;});}}}return({struct Cyc_Absyn_Fndecl*
_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->sc=(void*)sc;_tmp88->name=d->id;_tmp88->tvs=
_tmp73;_tmp88->is_inline=is_inline;_tmp88->effect=_tmp74;_tmp88->ret_type=(void*)
_tmp75;_tmp88->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp7B);_tmp88->c_varargs=_tmp77;_tmp88->cyc_varargs=_tmp78;_tmp88->rgn_po=
_tmp79;_tmp88->body=body;_tmp88->cached_typ=0;_tmp88->param_vardecls=0;_tmp88->fn_vardecl=
0;_tmp88->attributes=Cyc_List_append(_tmp7A,_tmp6C);_tmp88;});}_LL3E:;_LL3F:({
void*_tmp89=0;((int(*)(struct Cyc_Position_Segment*sg,struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Parse_abort)(loc,({const char*_tmp8A="declarator is not a function prototype";
_tag_dynforward(_tmp8A,sizeof(char),_get_zero_arr_size_char(_tmp8A,39));}),
_tag_dynforward(_tmp89,sizeof(void*),0));});_LL3B:;}}}}}static char _tmp8B[52]="at most one type may appear within a type specifier";
static struct _dynforward_ptr Cyc_Parse_msg1={_tmp8B,_tmp8B + 52};static char _tmp8C[
63]="const or volatile may appear only once within a type specifier";static
struct _dynforward_ptr Cyc_Parse_msg2={_tmp8C,_tmp8C + 63};static char _tmp8D[50]="type specifier includes more than one declaration";
static struct _dynforward_ptr Cyc_Parse_msg3={_tmp8D,_tmp8D + 50};static char _tmp8E[
60]="sign specifier may appear only once within a type specifier";static struct
_dynforward_ptr Cyc_Parse_msg4={_tmp8E,_tmp8E + 60};static struct _tuple5 Cyc_Parse_collapse_type_specifiers(
struct Cyc_List_List*ts,struct Cyc_Position_Segment*loc){struct Cyc_Core_Opt*
declopt=0;int seen_type=0;int seen_sign=0;int seen_size=0;void*t=(void*)0;void*sz=(
void*)2;void*sgn=(void*)0;struct Cyc_Position_Segment*last_loc=loc;for(0;ts != 0;
ts=ts->tl){void*_tmp8F=(void*)ts->hd;void*_tmp90;struct Cyc_Position_Segment*
_tmp91;struct Cyc_Position_Segment*_tmp92;struct Cyc_Position_Segment*_tmp93;
struct Cyc_Position_Segment*_tmp94;struct Cyc_Position_Segment*_tmp95;struct Cyc_Absyn_Decl*
_tmp96;_LL41: if(*((int*)_tmp8F)!= 4)goto _LL43;_tmp90=(void*)((struct Cyc_Parse_Type_spec_struct*)
_tmp8F)->f1;_tmp91=((struct Cyc_Parse_Type_spec_struct*)_tmp8F)->f2;_LL42: if(
seen_type)Cyc_Parse_err(Cyc_Parse_msg1,_tmp91);last_loc=_tmp91;seen_type=1;t=
_tmp90;goto _LL40;_LL43: if(*((int*)_tmp8F)!= 0)goto _LL45;_tmp92=((struct Cyc_Parse_Signed_spec_struct*)
_tmp8F)->f1;_LL44: if(seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp92);if(seen_type)
Cyc_Parse_err(({const char*_tmp97="signed qualifier must come before type";
_tag_dynforward(_tmp97,sizeof(char),_get_zero_arr_size_char(_tmp97,39));}),
_tmp92);last_loc=_tmp92;seen_sign=1;sgn=(void*)0;goto _LL40;_LL45: if(*((int*)
_tmp8F)!= 1)goto _LL47;_tmp93=((struct Cyc_Parse_Unsigned_spec_struct*)_tmp8F)->f1;
_LL46: if(seen_sign)Cyc_Parse_err(Cyc_Parse_msg4,_tmp93);if(seen_type)Cyc_Parse_err(({
const char*_tmp98="signed qualifier must come before type";_tag_dynforward(_tmp98,
sizeof(char),_get_zero_arr_size_char(_tmp98,39));}),_tmp93);last_loc=_tmp93;
seen_sign=1;sgn=(void*)1;goto _LL40;_LL47: if(*((int*)_tmp8F)!= 2)goto _LL49;_tmp94=((
struct Cyc_Parse_Short_spec_struct*)_tmp8F)->f1;_LL48: if(seen_size)Cyc_Parse_err(({
const char*_tmp99="integral size may appear only once within a type specifier";
_tag_dynforward(_tmp99,sizeof(char),_get_zero_arr_size_char(_tmp99,59));}),
_tmp94);if(seen_type)Cyc_Parse_err(({const char*_tmp9A="short modifier must come before base type";
_tag_dynforward(_tmp9A,sizeof(char),_get_zero_arr_size_char(_tmp9A,42));}),
_tmp94);last_loc=_tmp94;seen_size=1;sz=(void*)1;goto _LL40;_LL49: if(*((int*)
_tmp8F)!= 3)goto _LL4B;_tmp95=((struct Cyc_Parse_Long_spec_struct*)_tmp8F)->f1;
_LL4A: if(seen_type)Cyc_Parse_err(({const char*_tmp9B="long modifier must come before base type";
_tag_dynforward(_tmp9B,sizeof(char),_get_zero_arr_size_char(_tmp9B,41));}),
_tmp95);if(seen_size){void*_tmp9C=sz;_LL4E: if((int)_tmp9C != 3)goto _LL50;_LL4F: sz=(
void*)4;goto _LL4D;_LL50:;_LL51: Cyc_Parse_err(({const char*_tmp9D="extra long in type specifier";
_tag_dynforward(_tmp9D,sizeof(char),_get_zero_arr_size_char(_tmp9D,29));}),
_tmp95);goto _LL4D;_LL4D:;}else{sz=(void*)3;}last_loc=_tmp95;seen_size=1;goto
_LL40;_LL4B: if(*((int*)_tmp8F)!= 5)goto _LL40;_tmp96=((struct Cyc_Parse_Decl_spec_struct*)
_tmp8F)->f1;_LL4C: last_loc=_tmp96->loc;if(declopt == 0  && !seen_type){seen_type=1;{
void*_tmp9E=(void*)_tmp96->r;struct Cyc_Absyn_Aggrdecl*_tmp9F;struct Cyc_Absyn_Tuniondecl*
_tmpA0;struct Cyc_Absyn_Enumdecl*_tmpA1;_LL53: if(_tmp9E <= (void*)2)goto _LL59;if(*((
int*)_tmp9E)!= 4)goto _LL55;_tmp9F=((struct Cyc_Absyn_Aggr_d_struct*)_tmp9E)->f1;
_LL54: {struct Cyc_List_List*_tmpA2=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(
Cyc_Parse_copy_tvar,_tmp9F->tvs));t=(void*)({struct Cyc_Absyn_AggrType_struct*
_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3[0]=({struct Cyc_Absyn_AggrType_struct
_tmpA4;_tmpA4.tag=10;_tmpA4.f1=({struct Cyc_Absyn_AggrInfo _tmpA5;_tmpA5.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmpA6;(_tmpA6.UnknownAggr).tag=
0;(_tmpA6.UnknownAggr).f1=(void*)((void*)_tmp9F->kind);(_tmpA6.UnknownAggr).f2=
_tmp9F->name;_tmpA6;});_tmpA5.targs=_tmpA2;_tmpA5;});_tmpA4;});_tmpA3;});if(
_tmp9F->impl != 0)declopt=({struct Cyc_Core_Opt*_tmpA7=_cycalloc(sizeof(*_tmpA7));
_tmpA7->v=_tmp96;_tmpA7;});goto _LL52;}_LL55: if(*((int*)_tmp9E)!= 5)goto _LL57;
_tmpA0=((struct Cyc_Absyn_Tunion_d_struct*)_tmp9E)->f1;_LL56: {struct Cyc_List_List*
_tmpA8=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
x))Cyc_List_map)(Cyc_Parse_tvar2typ,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*
f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_copy_tvar,
_tmpA0->tvs));struct Cyc_Core_Opt*ropt=_tmpA0->is_flat?0:({struct Cyc_Core_Opt*
_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->v=(void*)((void*)2);_tmpAF;});t=(void*)({
struct Cyc_Absyn_TunionType_struct*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9[0]=({
struct Cyc_Absyn_TunionType_struct _tmpAA;_tmpAA.tag=2;_tmpAA.f1=({struct Cyc_Absyn_TunionInfo
_tmpAB;_tmpAB.tunion_info=(union Cyc_Absyn_TunionInfoU_union)({union Cyc_Absyn_TunionInfoU_union
_tmpAC;(_tmpAC.KnownTunion).tag=1;(_tmpAC.KnownTunion).f1=({struct Cyc_Absyn_Tuniondecl**
_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD[0]=_tmpA0;_tmpAD;});_tmpAC;});_tmpAB.targs=
_tmpA8;_tmpAB.rgn=ropt;_tmpAB;});_tmpAA;});_tmpA9;});if(_tmpA0->fields != 0)
declopt=({struct Cyc_Core_Opt*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->v=_tmp96;
_tmpAE;});goto _LL52;}_LL57: if(*((int*)_tmp9E)!= 6)goto _LL59;_tmpA1=((struct Cyc_Absyn_Enum_d_struct*)
_tmp9E)->f1;_LL58: t=(void*)({struct Cyc_Absyn_EnumType_struct*_tmpB0=_cycalloc(
sizeof(*_tmpB0));_tmpB0[0]=({struct Cyc_Absyn_EnumType_struct _tmpB1;_tmpB1.tag=12;
_tmpB1.f1=_tmpA1->name;_tmpB1.f2=0;_tmpB1;});_tmpB0;});declopt=({struct Cyc_Core_Opt*
_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->v=_tmp96;_tmpB2;});goto _LL52;_LL59:;
_LL5A:({void*_tmpB3=0;((int(*)(struct Cyc_Position_Segment*sg,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmp96->loc,({const
char*_tmpB4="bad declaration within type specifier";_tag_dynforward(_tmpB4,
sizeof(char),_get_zero_arr_size_char(_tmpB4,38));}),_tag_dynforward(_tmpB3,
sizeof(void*),0));});_LL52:;}}else{Cyc_Parse_err(Cyc_Parse_msg3,_tmp96->loc);}
goto _LL40;_LL40:;}if(!seen_type){if(!seen_sign  && !seen_size)({void*_tmpB5=0;Cyc_Tcutil_warn(
last_loc,({const char*_tmpB6="missing type within specifier";_tag_dynforward(
_tmpB6,sizeof(char),_get_zero_arr_size_char(_tmpB6,30));}),_tag_dynforward(
_tmpB5,sizeof(void*),0));});t=Cyc_Absyn_int_typ(sgn,sz);}else{if(seen_sign){void*
_tmpB7=t;void*_tmpB8;void*_tmpB9;_LL5C: if(_tmpB7 <= (void*)4)goto _LL5E;if(*((int*)
_tmpB7)!= 5)goto _LL5E;_tmpB8=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpB7)->f1;
_tmpB9=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpB7)->f2;_LL5D: if(_tmpB8 != 
sgn)t=Cyc_Absyn_int_typ(sgn,_tmpB9);goto _LL5B;_LL5E:;_LL5F: Cyc_Parse_err(({const
char*_tmpBA="sign specification on non-integral type";_tag_dynforward(_tmpBA,
sizeof(char),_get_zero_arr_size_char(_tmpBA,40));}),last_loc);goto _LL5B;_LL5B:;}
if(seen_size){void*_tmpBB=t;void*_tmpBC;void*_tmpBD;_LL61: if(_tmpBB <= (void*)4)
goto _LL65;if(*((int*)_tmpBB)!= 5)goto _LL63;_tmpBC=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmpBB)->f1;_tmpBD=(void*)((struct Cyc_Absyn_IntType_struct*)_tmpBB)->f2;_LL62:
if(_tmpBD != sz)t=Cyc_Absyn_int_typ(_tmpBC,sz);goto _LL60;_LL63: if(*((int*)_tmpBB)
!= 6)goto _LL65;_LL64: t=Cyc_Absyn_double_typ(1);goto _LL60;_LL65:;_LL66: Cyc_Parse_err(({
const char*_tmpBE="size qualifier on non-integral type";_tag_dynforward(_tmpBE,
sizeof(char),_get_zero_arr_size_char(_tmpBE,36));}),last_loc);goto _LL60;_LL60:;}}
return({struct _tuple5 _tmpBF;_tmpBF.f1=t;_tmpBF.f2=declopt;_tmpBF;});}static
struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual
tq,void*t,struct Cyc_List_List*ds,struct Cyc_List_List*shared_atts){if(ds == 0)
return 0;{struct Cyc_Parse_Declarator*_tmpC0=(struct Cyc_Parse_Declarator*)ds->hd;
struct _tuple1*_tmpC1=_tmpC0->id;struct Cyc_Absyn_Tqual _tmpC3;void*_tmpC4;struct
Cyc_List_List*_tmpC5;struct Cyc_List_List*_tmpC6;struct _tuple6 _tmpC2=Cyc_Parse_apply_tms(
tq,t,shared_atts,_tmpC0->tms);_tmpC3=_tmpC2.f1;_tmpC4=_tmpC2.f2;_tmpC5=_tmpC2.f3;
_tmpC6=_tmpC2.f4;if(ds->tl == 0)return({struct Cyc_List_List*_tmpC7=_region_malloc(
r,sizeof(*_tmpC7));_tmpC7->hd=({struct _tuple7*_tmpC8=_region_malloc(r,sizeof(*
_tmpC8));_tmpC8->f1=_tmpC1;_tmpC8->f2=_tmpC3;_tmpC8->f3=_tmpC4;_tmpC8->f4=_tmpC5;
_tmpC8->f5=_tmpC6;_tmpC8;});_tmpC7->tl=0;_tmpC7;});else{return({struct Cyc_List_List*
_tmpC9=_region_malloc(r,sizeof(*_tmpC9));_tmpC9->hd=({struct _tuple7*_tmpCA=
_region_malloc(r,sizeof(*_tmpCA));_tmpCA->f1=_tmpC1;_tmpCA->f2=_tmpC3;_tmpCA->f3=
_tmpC4;_tmpCA->f4=_tmpC5;_tmpCA->f5=_tmpC6;_tmpCA;});_tmpC9->tl=Cyc_Parse_apply_tmss(
r,_tmpC3,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts);_tmpC9;});}}}static struct
_tuple6 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*
atts,struct Cyc_List_List*tms){if(tms == 0)return({struct _tuple6 _tmpCB;_tmpCB.f1=
tq;_tmpCB.f2=t;_tmpCB.f3=0;_tmpCB.f4=atts;_tmpCB;});{void*_tmpCC=(void*)tms->hd;
struct Cyc_Absyn_Conref*_tmpCD;struct Cyc_Position_Segment*_tmpCE;struct Cyc_Absyn_Exp*
_tmpCF;struct Cyc_Absyn_Conref*_tmpD0;struct Cyc_Position_Segment*_tmpD1;void*
_tmpD2;struct Cyc_List_List*_tmpD3;struct Cyc_Position_Segment*_tmpD4;struct Cyc_Absyn_PtrAtts
_tmpD5;struct Cyc_Absyn_Tqual _tmpD6;struct Cyc_Position_Segment*_tmpD7;struct Cyc_List_List*
_tmpD8;_LL68: if(*((int*)_tmpCC)!= 0)goto _LL6A;_tmpCD=((struct Cyc_Absyn_Carray_mod_struct*)
_tmpCC)->f1;_tmpCE=((struct Cyc_Absyn_Carray_mod_struct*)_tmpCC)->f2;_LL69: return
Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,0,_tmpCD,
_tmpCE),atts,tms->tl);_LL6A: if(*((int*)_tmpCC)!= 1)goto _LL6C;_tmpCF=((struct Cyc_Absyn_ConstArray_mod_struct*)
_tmpCC)->f1;_tmpD0=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmpCC)->f2;_tmpD1=((
struct Cyc_Absyn_ConstArray_mod_struct*)_tmpCC)->f3;_LL6B: return Cyc_Parse_apply_tms(
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_array_typ(t,tq,(struct Cyc_Absyn_Exp*)_tmpCF,
_tmpD0,_tmpD1),atts,tms->tl);_LL6C: if(*((int*)_tmpCC)!= 3)goto _LL6E;_tmpD2=(void*)((
struct Cyc_Absyn_Function_mod_struct*)_tmpCC)->f1;_LL6D: {void*_tmpD9=_tmpD2;
struct Cyc_List_List*_tmpDA;int _tmpDB;struct Cyc_Absyn_VarargInfo*_tmpDC;struct Cyc_Core_Opt*
_tmpDD;struct Cyc_List_List*_tmpDE;struct Cyc_Position_Segment*_tmpDF;_LL75: if(*((
int*)_tmpD9)!= 1)goto _LL77;_tmpDA=((struct Cyc_Absyn_WithTypes_struct*)_tmpD9)->f1;
_tmpDB=((struct Cyc_Absyn_WithTypes_struct*)_tmpD9)->f2;_tmpDC=((struct Cyc_Absyn_WithTypes_struct*)
_tmpD9)->f3;_tmpDD=((struct Cyc_Absyn_WithTypes_struct*)_tmpD9)->f4;_tmpDE=((
struct Cyc_Absyn_WithTypes_struct*)_tmpD9)->f5;_LL76: {struct Cyc_List_List*
typvars=0;struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;{struct
Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){if(Cyc_Absyn_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0->hd=(void*)((
void*)as->hd);_tmpE0->tl=fn_atts;_tmpE0;});else{new_atts=({struct Cyc_List_List*
_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->hd=(void*)((void*)as->hd);_tmpE1->tl=
new_atts;_tmpE1;});}}}if(tms->tl != 0){void*_tmpE2=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd;struct Cyc_List_List*_tmpE3;_LL7A: if(*((int*)_tmpE2)!= 
4)goto _LL7C;_tmpE3=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpE2)->f1;_LL7B:
typvars=_tmpE3;tms=tms->tl;goto _LL79;_LL7C:;_LL7D: goto _LL79;_LL79:;}if(((((!
_tmpDB  && _tmpDC == 0) && _tmpDA != 0) && _tmpDA->tl == 0) && (*((struct _tuple2*)
_tmpDA->hd)).f1 == 0) && (*((struct _tuple2*)_tmpDA->hd)).f3 == (void*)0)_tmpDA=0;t=
Cyc_Parse_array2ptr(t,0);((void(*)(void(*f)(struct _tuple2*),struct Cyc_List_List*
x))Cyc_List_iter)(Cyc_Parse_arg_array2ptr,_tmpDA);return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(
tq.loc),Cyc_Absyn_function_typ(typvars,_tmpDD,t,_tmpDA,_tmpDB,_tmpDC,_tmpDE,
fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);}_LL77: if(*((int*)
_tmpD9)!= 0)goto _LL74;_tmpDF=((struct Cyc_Absyn_NoTypes_struct*)_tmpD9)->f2;_LL78:({
void*_tmpE4=0;((int(*)(struct Cyc_Position_Segment*sg,struct _dynforward_ptr fmt,
struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmpDF,({const char*_tmpE5="function declaration without parameter types";
_tag_dynforward(_tmpE5,sizeof(char),_get_zero_arr_size_char(_tmpE5,45));}),
_tag_dynforward(_tmpE4,sizeof(void*),0));});_LL74:;}_LL6E: if(*((int*)_tmpCC)!= 4)
goto _LL70;_tmpD3=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCC)->f1;_tmpD4=((
struct Cyc_Absyn_TypeParams_mod_struct*)_tmpCC)->f2;_LL6F: if(tms->tl == 0)return({
struct _tuple6 _tmpE6;_tmpE6.f1=tq;_tmpE6.f2=t;_tmpE6.f3=_tmpD3;_tmpE6.f4=atts;
_tmpE6;});({void*_tmpE7=0;((int(*)(struct Cyc_Position_Segment*sg,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(_tmpD4,({const char*
_tmpE8="type parameters must appear before function arguments in declarator";
_tag_dynforward(_tmpE8,sizeof(char),_get_zero_arr_size_char(_tmpE8,68));}),
_tag_dynforward(_tmpE7,sizeof(void*),0));});_LL70: if(*((int*)_tmpCC)!= 2)goto
_LL72;_tmpD5=((struct Cyc_Absyn_Pointer_mod_struct*)_tmpCC)->f1;_tmpD6=((struct
Cyc_Absyn_Pointer_mod_struct*)_tmpCC)->f2;_LL71: return Cyc_Parse_apply_tms(_tmpD6,(
void*)({struct Cyc_Absyn_PointerType_struct*_tmpE9=_cycalloc(sizeof(*_tmpE9));
_tmpE9[0]=({struct Cyc_Absyn_PointerType_struct _tmpEA;_tmpEA.tag=4;_tmpEA.f1=({
struct Cyc_Absyn_PtrInfo _tmpEB;_tmpEB.elt_typ=(void*)t;_tmpEB.elt_tq=tq;_tmpEB.ptr_atts=
_tmpD5;_tmpEB;});_tmpEA;});_tmpE9;}),atts,tms->tl);_LL72: if(*((int*)_tmpCC)!= 5)
goto _LL67;_tmpD7=((struct Cyc_Absyn_Attributes_mod_struct*)_tmpCC)->f1;_tmpD8=((
struct Cyc_Absyn_Attributes_mod_struct*)_tmpCC)->f2;_LL73: return Cyc_Parse_apply_tms(
tq,t,Cyc_List_append(atts,_tmpD8),tms->tl);_LL67:;}}void*Cyc_Parse_speclist2typ(
struct Cyc_List_List*tss,struct Cyc_Position_Segment*loc){void*_tmpED;struct Cyc_Core_Opt*
_tmpEE;struct _tuple5 _tmpEC=Cyc_Parse_collapse_type_specifiers(tss,loc);_tmpED=
_tmpEC.f1;_tmpEE=_tmpEC.f2;if(_tmpEE != 0)({void*_tmpEF=0;Cyc_Tcutil_warn(loc,({
const char*_tmpF0="ignoring nested type declaration(s) in specifier list";
_tag_dynforward(_tmpF0,sizeof(char),_get_zero_arr_size_char(_tmpF0,54));}),
_tag_dynforward(_tmpEF,sizeof(void*),0));});return _tmpED;}static struct Cyc_Absyn_Decl*
Cyc_Parse_v_typ_to_typedef(struct Cyc_Position_Segment*loc,struct _tuple7*t){
struct _tuple7 _tmpF2;struct _tuple1*_tmpF3;struct Cyc_Absyn_Tqual _tmpF4;void*_tmpF5;
struct Cyc_List_List*_tmpF6;struct Cyc_List_List*_tmpF7;struct _tuple7*_tmpF1=t;
_tmpF2=*_tmpF1;_tmpF3=_tmpF2.f1;_tmpF4=_tmpF2.f2;_tmpF5=_tmpF2.f3;_tmpF6=_tmpF2.f4;
_tmpF7=_tmpF2.f5;Cyc_Lex_register_typedef(_tmpF3);{struct Cyc_Core_Opt*kind;
struct Cyc_Core_Opt*type;{void*_tmpF8=_tmpF5;struct Cyc_Core_Opt*_tmpF9;_LL7F: if(
_tmpF8 <= (void*)4)goto _LL81;if(*((int*)_tmpF8)!= 0)goto _LL81;_tmpF9=((struct Cyc_Absyn_Evar_struct*)
_tmpF8)->f1;_LL80: type=0;if(_tmpF9 == 0)kind=(struct Cyc_Core_Opt*)& Cyc_Tcutil_bk;
else{kind=_tmpF9;}goto _LL7E;_LL81:;_LL82: kind=0;type=({struct Cyc_Core_Opt*_tmpFA=
_cycalloc(sizeof(*_tmpFA));_tmpFA->v=(void*)_tmpF5;_tmpFA;});goto _LL7E;_LL7E:;}
return Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Typedef_d_struct*_tmpFB=
_cycalloc(sizeof(*_tmpFB));_tmpFB[0]=({struct Cyc_Absyn_Typedef_d_struct _tmpFC;
_tmpFC.tag=7;_tmpFC.f1=({struct Cyc_Absyn_Typedefdecl*_tmpFD=_cycalloc(sizeof(*
_tmpFD));_tmpFD->name=_tmpF3;_tmpFD->tvs=_tmpF6;_tmpFD->kind=kind;_tmpFD->defn=
type;_tmpFD->atts=_tmpF7;_tmpFD->tq=_tmpF4;_tmpFD;});_tmpFC;});_tmpFB;}),loc);}}
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct
Cyc_Absyn_Stmt*s){return Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE[0]=({struct Cyc_Absyn_Decl_s_struct
_tmpFF;_tmpFF.tag=11;_tmpFF.f1=d;_tmpFF.f2=s;_tmpFF;});_tmpFE;}),d->loc);}static
struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,
struct Cyc_Absyn_Stmt*s){return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*
f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*
accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}static struct Cyc_List_List*
Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec*ds,struct Cyc_List_List*
ids,struct Cyc_Position_Segment*tqual_loc,struct Cyc_Position_Segment*loc){struct
_RegionHandle _tmp100=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp100;
_push_region(mkrgn);{struct Cyc_Parse_Declaration_spec _tmp102;struct Cyc_Absyn_Tqual
_tmp103;struct Cyc_List_List*_tmp104;struct Cyc_List_List*_tmp105;struct Cyc_Parse_Declaration_spec*
_tmp101=ds;_tmp102=*_tmp101;_tmp103=_tmp102.tq;_tmp104=_tmp102.type_specs;
_tmp105=_tmp102.attributes;if(_tmp103.loc == 0)_tmp103.loc=tqual_loc;if(ds->is_inline)
Cyc_Parse_err(({const char*_tmp106="inline is allowed only on function definitions";
_tag_dynforward(_tmp106,sizeof(char),_get_zero_arr_size_char(_tmp106,47));}),loc);
if(_tmp104 == 0){Cyc_Parse_err(({const char*_tmp107="missing type specifiers in declaration";
_tag_dynforward(_tmp107,sizeof(char),_get_zero_arr_size_char(_tmp107,39));}),loc);{
struct Cyc_List_List*_tmp108=0;_npop_handler(0);return _tmp108;}}{void*s=(void*)2;
int istypedef=0;if(ds->sc != 0){void*_tmp109=(void*)((struct Cyc_Core_Opt*)
_check_null(ds->sc))->v;_LL84: if((int)_tmp109 != 0)goto _LL86;_LL85: istypedef=1;
goto _LL83;_LL86: if((int)_tmp109 != 1)goto _LL88;_LL87: s=(void*)3;goto _LL83;_LL88:
if((int)_tmp109 != 2)goto _LL8A;_LL89: s=(void*)4;goto _LL83;_LL8A: if((int)_tmp109 != 
3)goto _LL8C;_LL8B: s=(void*)0;goto _LL83;_LL8C: if((int)_tmp109 != 4)goto _LL8E;_LL8D:
s=(void*)2;goto _LL83;_LL8E: if((int)_tmp109 != 5)goto _LL90;_LL8F: if(Cyc_Parse_no_register)
s=(void*)2;else{s=(void*)5;}goto _LL83;_LL90: if((int)_tmp109 != 6)goto _LL83;_LL91:
s=(void*)1;goto _LL83;_LL83:;}{struct Cyc_List_List*_tmp10B;struct Cyc_List_List*
_tmp10C;struct _tuple0 _tmp10A=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(mkrgn,mkrgn,ids);
_tmp10B=_tmp10A.f1;_tmp10C=_tmp10A.f2;{int exps_empty=1;{struct Cyc_List_List*es=
_tmp10C;for(0;es != 0;es=es->tl){if((struct Cyc_Absyn_Exp*)es->hd != 0){exps_empty=
0;break;}}}{struct _tuple5 _tmp10D=Cyc_Parse_collapse_type_specifiers(_tmp104,loc);
if(_tmp10B == 0){void*_tmp10F;struct Cyc_Core_Opt*_tmp110;struct _tuple5 _tmp10E=
_tmp10D;_tmp10F=_tmp10E.f1;_tmp110=_tmp10E.f2;if(_tmp110 != 0){struct Cyc_Absyn_Decl*
d=(struct Cyc_Absyn_Decl*)_tmp110->v;{void*_tmp111=(void*)d->r;struct Cyc_Absyn_Enumdecl*
_tmp112;struct Cyc_Absyn_Aggrdecl*_tmp113;struct Cyc_Absyn_Tuniondecl*_tmp114;
_LL93: if(_tmp111 <= (void*)2)goto _LL99;if(*((int*)_tmp111)!= 6)goto _LL95;_tmp112=((
struct Cyc_Absyn_Enum_d_struct*)_tmp111)->f1;_LL94:(void*)(_tmp112->sc=(void*)s);
if(_tmp105 != 0)Cyc_Parse_err(({const char*_tmp115="bad attributes on enum";
_tag_dynforward(_tmp115,sizeof(char),_get_zero_arr_size_char(_tmp115,23));}),loc);
goto _LL92;_LL95: if(*((int*)_tmp111)!= 4)goto _LL97;_tmp113=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp111)->f1;_LL96:(void*)(_tmp113->sc=(void*)s);_tmp113->attributes=_tmp105;
goto _LL92;_LL97: if(*((int*)_tmp111)!= 5)goto _LL99;_tmp114=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp111)->f1;_LL98:(void*)(_tmp114->sc=(void*)s);if(_tmp105 != 0)Cyc_Parse_err(({
const char*_tmp116="bad attributes on tunion";_tag_dynforward(_tmp116,sizeof(char),
_get_zero_arr_size_char(_tmp116,25));}),loc);goto _LL92;_LL99:;_LL9A: Cyc_Parse_err(({
const char*_tmp117="bad declaration";_tag_dynforward(_tmp117,sizeof(char),
_get_zero_arr_size_char(_tmp117,16));}),loc);{struct Cyc_List_List*_tmp118=0;
_npop_handler(0);return _tmp118;}_LL92:;}{struct Cyc_List_List*_tmp11A=({struct Cyc_List_List*
_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->hd=d;_tmp119->tl=0;_tmp119;});
_npop_handler(0);return _tmp11A;}}else{void*_tmp11B=_tmp10F;struct Cyc_Absyn_AggrInfo
_tmp11C;union Cyc_Absyn_AggrInfoU_union _tmp11D;void*_tmp11E;struct _tuple1*_tmp11F;
struct Cyc_List_List*_tmp120;struct Cyc_Absyn_TunionInfo _tmp121;union Cyc_Absyn_TunionInfoU_union
_tmp122;struct Cyc_Absyn_Tuniondecl**_tmp123;struct Cyc_Absyn_TunionInfo _tmp124;
union Cyc_Absyn_TunionInfoU_union _tmp125;struct Cyc_Absyn_UnknownTunionInfo _tmp126;
struct _tuple1*_tmp127;int _tmp128;int _tmp129;struct Cyc_List_List*_tmp12A;struct
_tuple1*_tmp12B;struct Cyc_List_List*_tmp12C;_LL9C: if(_tmp11B <= (void*)4)goto
_LLA6;if(*((int*)_tmp11B)!= 10)goto _LL9E;_tmp11C=((struct Cyc_Absyn_AggrType_struct*)
_tmp11B)->f1;_tmp11D=_tmp11C.aggr_info;if((((((struct Cyc_Absyn_AggrType_struct*)
_tmp11B)->f1).aggr_info).UnknownAggr).tag != 0)goto _LL9E;_tmp11E=(_tmp11D.UnknownAggr).f1;
_tmp11F=(_tmp11D.UnknownAggr).f2;_tmp120=_tmp11C.targs;_LL9D: {struct Cyc_List_List*
_tmp12D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
loc,_tmp120);struct Cyc_Absyn_Aggrdecl*_tmp12E=({struct Cyc_Absyn_Aggrdecl*_tmp134=
_cycalloc(sizeof(*_tmp134));_tmp134->kind=(void*)_tmp11E;_tmp134->sc=(void*)s;
_tmp134->name=_tmp11F;_tmp134->tvs=_tmp12D;_tmp134->impl=0;_tmp134->attributes=0;
_tmp134;});if(_tmp105 != 0)Cyc_Parse_err(({const char*_tmp12F="bad attributes on type declaration";
_tag_dynforward(_tmp12F,sizeof(char),_get_zero_arr_size_char(_tmp12F,35));}),loc);{
struct Cyc_List_List*_tmp133=({struct Cyc_List_List*_tmp130=_cycalloc(sizeof(*
_tmp130));_tmp130->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_struct*
_tmp131=_cycalloc(sizeof(*_tmp131));_tmp131[0]=({struct Cyc_Absyn_Aggr_d_struct
_tmp132;_tmp132.tag=4;_tmp132.f1=_tmp12E;_tmp132;});_tmp131;}),loc);_tmp130->tl=
0;_tmp130;});_npop_handler(0);return _tmp133;}}_LL9E: if(*((int*)_tmp11B)!= 2)goto
_LLA0;_tmp121=((struct Cyc_Absyn_TunionType_struct*)_tmp11B)->f1;_tmp122=_tmp121.tunion_info;
if((((((struct Cyc_Absyn_TunionType_struct*)_tmp11B)->f1).tunion_info).KnownTunion).tag
!= 1)goto _LLA0;_tmp123=(_tmp122.KnownTunion).f1;_LL9F: if(_tmp105 != 0)Cyc_Parse_err(({
const char*_tmp135="bad attributes on tunion";_tag_dynforward(_tmp135,sizeof(char),
_get_zero_arr_size_char(_tmp135,25));}),loc);{struct Cyc_List_List*_tmp139=({
struct Cyc_List_List*_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Tunion_d_struct*_tmp137=_cycalloc(sizeof(*_tmp137));
_tmp137[0]=({struct Cyc_Absyn_Tunion_d_struct _tmp138;_tmp138.tag=5;_tmp138.f1=*
_tmp123;_tmp138;});_tmp137;}),loc);_tmp136->tl=0;_tmp136;});_npop_handler(0);
return _tmp139;}_LLA0: if(*((int*)_tmp11B)!= 2)goto _LLA2;_tmp124=((struct Cyc_Absyn_TunionType_struct*)
_tmp11B)->f1;_tmp125=_tmp124.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp11B)->f1).tunion_info).UnknownTunion).tag != 0)goto _LLA2;_tmp126=(_tmp125.UnknownTunion).f1;
_tmp127=_tmp126.name;_tmp128=_tmp126.is_xtunion;_tmp129=_tmp126.is_flat;_tmp12A=
_tmp124.targs;_LLA1: {struct Cyc_List_List*_tmp13A=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp12A);
struct Cyc_Absyn_Decl*_tmp13B=Cyc_Absyn_tunion_decl(s,_tmp127,_tmp13A,0,_tmp128,
_tmp129,loc);if(_tmp105 != 0)Cyc_Parse_err(({const char*_tmp13C="bad attributes on tunion";
_tag_dynforward(_tmp13C,sizeof(char),_get_zero_arr_size_char(_tmp13C,25));}),loc);{
struct Cyc_List_List*_tmp13E=({struct Cyc_List_List*_tmp13D=_cycalloc(sizeof(*
_tmp13D));_tmp13D->hd=_tmp13B;_tmp13D->tl=0;_tmp13D;});_npop_handler(0);return
_tmp13E;}}_LLA2: if(*((int*)_tmp11B)!= 12)goto _LLA4;_tmp12B=((struct Cyc_Absyn_EnumType_struct*)
_tmp11B)->f1;_LLA3: {struct Cyc_Absyn_Enumdecl*_tmp13F=({struct Cyc_Absyn_Enumdecl*
_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146->sc=(void*)s;_tmp146->name=_tmp12B;
_tmp146->fields=0;_tmp146;});if(_tmp105 != 0)Cyc_Parse_err(({const char*_tmp140="bad attributes on enum";
_tag_dynforward(_tmp140,sizeof(char),_get_zero_arr_size_char(_tmp140,23));}),loc);{
struct Cyc_List_List*_tmp145=({struct Cyc_List_List*_tmp141=_cycalloc(sizeof(*
_tmp141));_tmp141->hd=({struct Cyc_Absyn_Decl*_tmp142=_cycalloc(sizeof(*_tmp142));
_tmp142->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*_tmp143=_cycalloc(
sizeof(*_tmp143));_tmp143[0]=({struct Cyc_Absyn_Enum_d_struct _tmp144;_tmp144.tag=
6;_tmp144.f1=_tmp13F;_tmp144;});_tmp143;}));_tmp142->loc=loc;_tmp142;});_tmp141->tl=
0;_tmp141;});_npop_handler(0);return _tmp145;}}_LLA4: if(*((int*)_tmp11B)!= 13)
goto _LLA6;_tmp12C=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp11B)->f1;_LLA5: {
struct Cyc_Absyn_Enumdecl*_tmp147=({struct Cyc_Absyn_Enumdecl*_tmp14E=_cycalloc(
sizeof(*_tmp14E));_tmp14E->sc=(void*)s;_tmp14E->name=Cyc_Parse_gensym_enum();
_tmp14E->fields=({struct Cyc_Core_Opt*_tmp14F=_cycalloc(sizeof(*_tmp14F));_tmp14F->v=
_tmp12C;_tmp14F;});_tmp14E;});if(_tmp105 != 0)Cyc_Parse_err(({const char*_tmp148="bad attributes on enum";
_tag_dynforward(_tmp148,sizeof(char),_get_zero_arr_size_char(_tmp148,23));}),loc);{
struct Cyc_List_List*_tmp14D=({struct Cyc_List_List*_tmp149=_cycalloc(sizeof(*
_tmp149));_tmp149->hd=({struct Cyc_Absyn_Decl*_tmp14A=_cycalloc(sizeof(*_tmp14A));
_tmp14A->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*_tmp14B=_cycalloc(
sizeof(*_tmp14B));_tmp14B[0]=({struct Cyc_Absyn_Enum_d_struct _tmp14C;_tmp14C.tag=
6;_tmp14C.f1=_tmp147;_tmp14C;});_tmp14B;}));_tmp14A->loc=loc;_tmp14A;});_tmp149->tl=
0;_tmp149;});_npop_handler(0);return _tmp14D;}}_LLA6:;_LLA7: Cyc_Parse_err(({const
char*_tmp150="missing declarator";_tag_dynforward(_tmp150,sizeof(char),
_get_zero_arr_size_char(_tmp150,19));}),loc);{struct Cyc_List_List*_tmp151=0;
_npop_handler(0);return _tmp151;}_LL9B:;}}else{void*t=_tmp10D.f1;struct Cyc_List_List*
_tmp152=Cyc_Parse_apply_tmss(mkrgn,_tmp103,t,_tmp10B,_tmp105);if(istypedef){if(!
exps_empty)Cyc_Parse_err(({const char*_tmp153="initializer in typedef declaration";
_tag_dynforward(_tmp153,sizeof(char),_get_zero_arr_size_char(_tmp153,35));}),loc);{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(
struct Cyc_Position_Segment*,struct _tuple7*),struct Cyc_Position_Segment*env,
struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp152);
if(_tmp10D.f2 != 0){struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)(_tmp10D.f2)->v;{
void*_tmp154=(void*)d->r;struct Cyc_Absyn_Aggrdecl*_tmp155;struct Cyc_Absyn_Tuniondecl*
_tmp156;struct Cyc_Absyn_Enumdecl*_tmp157;_LLA9: if(_tmp154 <= (void*)2)goto _LLAF;
if(*((int*)_tmp154)!= 4)goto _LLAB;_tmp155=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp154)->f1;_LLAA:(void*)(_tmp155->sc=(void*)s);_tmp155->attributes=_tmp105;
_tmp105=0;goto _LLA8;_LLAB: if(*((int*)_tmp154)!= 5)goto _LLAD;_tmp156=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp154)->f1;_LLAC:(void*)(_tmp156->sc=(void*)s);goto _LLA8;_LLAD: if(*((int*)
_tmp154)!= 6)goto _LLAF;_tmp157=((struct Cyc_Absyn_Enum_d_struct*)_tmp154)->f1;
_LLAE:(void*)(_tmp157->sc=(void*)s);goto _LLA8;_LLAF:;_LLB0: Cyc_Parse_err(({const
char*_tmp158="declaration within typedef is not a struct, union, tunion, or xtunion";
_tag_dynforward(_tmp158,sizeof(char),_get_zero_arr_size_char(_tmp158,70));}),loc);
goto _LLA8;_LLA8:;}decls=({struct Cyc_List_List*_tmp159=_cycalloc(sizeof(*_tmp159));
_tmp159->hd=d;_tmp159->tl=decls;_tmp159;});}{struct Cyc_List_List*_tmp15A=decls;
_npop_handler(0);return _tmp15A;}}}else{if(_tmp10D.f2 != 0)Cyc_Parse_unimp(({const
char*_tmp15B="nested type declaration within declarator";_tag_dynforward(_tmp15B,
sizeof(char),_get_zero_arr_size_char(_tmp15B,42));}),loc);{struct Cyc_List_List*
decls=0;{struct Cyc_List_List*_tmp15C=_tmp152;for(0;_tmp15C != 0;(_tmp15C=_tmp15C->tl,
_tmp10C=_tmp10C->tl)){struct _tuple7 _tmp15E;struct _tuple1*_tmp15F;struct Cyc_Absyn_Tqual
_tmp160;void*_tmp161;struct Cyc_List_List*_tmp162;struct Cyc_List_List*_tmp163;
struct _tuple7*_tmp15D=(struct _tuple7*)_tmp15C->hd;_tmp15E=*_tmp15D;_tmp15F=
_tmp15E.f1;_tmp160=_tmp15E.f2;_tmp161=_tmp15E.f3;_tmp162=_tmp15E.f4;_tmp163=
_tmp15E.f5;if(_tmp162 != 0)({void*_tmp164=0;Cyc_Tcutil_warn(loc,({const char*
_tmp165="bad type params, ignoring";_tag_dynforward(_tmp165,sizeof(char),
_get_zero_arr_size_char(_tmp165,26));}),_tag_dynforward(_tmp164,sizeof(void*),0));});
if(_tmp10C == 0)({void*_tmp166=0;((int(*)(struct Cyc_Position_Segment*sg,struct
_dynforward_ptr fmt,struct _dynforward_ptr ap))Cyc_Parse_abort)(loc,({const char*
_tmp167="unexpected NULL in parse!";_tag_dynforward(_tmp167,sizeof(char),
_get_zero_arr_size_char(_tmp167,26));}),_tag_dynforward(_tmp166,sizeof(void*),0));});{
struct Cyc_Absyn_Exp*_tmp168=(struct Cyc_Absyn_Exp*)_tmp10C->hd;struct Cyc_Absyn_Vardecl*
_tmp169=Cyc_Absyn_new_vardecl(_tmp15F,_tmp161,_tmp168);_tmp169->tq=_tmp160;(void*)(
_tmp169->sc=(void*)s);_tmp169->attributes=_tmp163;{struct Cyc_Absyn_Decl*_tmp16A=({
struct Cyc_Absyn_Decl*_tmp16C=_cycalloc(sizeof(*_tmp16C));_tmp16C->r=(void*)((
void*)({struct Cyc_Absyn_Var_d_struct*_tmp16D=_cycalloc(sizeof(*_tmp16D));_tmp16D[
0]=({struct Cyc_Absyn_Var_d_struct _tmp16E;_tmp16E.tag=0;_tmp16E.f1=_tmp169;
_tmp16E;});_tmp16D;}));_tmp16C->loc=loc;_tmp16C;});decls=({struct Cyc_List_List*
_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B->hd=_tmp16A;_tmp16B->tl=decls;
_tmp16B;});}}}}{struct Cyc_List_List*_tmp16F=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(decls);_npop_handler(0);return _tmp16F;}}}}}}}}};_pop_region(
mkrgn);}static void*Cyc_Parse_id_to_kind(struct _dynforward_ptr s,struct Cyc_Position_Segment*
loc){if(Cyc_strlen((struct _dynforward_ptr)s)== 1  || Cyc_strlen((struct
_dynforward_ptr)s)== 2)switch(*((const char*)_check_dynforward_subscript(s,
sizeof(char),0))){case 'A': _LLB1: return(void*)0;case 'M': _LLB2: return(void*)1;case
'B': _LLB3: return(void*)2;case 'R': _LLB4: return(void*)3;case 'U': _LLB5: if(*((const
char*)_check_dynforward_subscript(s,sizeof(char),1))== 'R')return(void*)4;else{
break;}case 'T': _LLB6: if(*((const char*)_check_dynforward_subscript(s,sizeof(char),
1))== 'R')return(void*)5;else{break;}case 'E': _LLB7: return(void*)6;case 'I': _LLB8:
return(void*)7;default: _LLB9: break;}Cyc_Parse_err((struct _dynforward_ptr)({
struct Cyc_Int_pa_struct _tmp173;_tmp173.tag=1;_tmp173.f1=(unsigned long)((int)Cyc_strlen((
struct _dynforward_ptr)s));{struct Cyc_String_pa_struct _tmp172;_tmp172.tag=0;
_tmp172.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)s);{void*_tmp170[2]={&
_tmp172,& _tmp173};Cyc_aprintf(({const char*_tmp171="bad kind: %s; strlen=%d";
_tag_dynforward(_tmp171,sizeof(char),_get_zero_arr_size_char(_tmp171,24));}),
_tag_dynforward(_tmp170,sizeof(void*),2));}}}),loc);return(void*)2;}static struct
Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){void*_tmp174=(void*)p->r;
struct _tuple1*_tmp175;struct Cyc_Absyn_Vardecl*_tmp176;struct Cyc_Absyn_Pat*
_tmp177;struct Cyc_Absyn_Pat _tmp178;void*_tmp179;struct Cyc_Absyn_Pat*_tmp17A;void*
_tmp17B;int _tmp17C;char _tmp17D;struct _dynforward_ptr _tmp17E;struct _tuple1*
_tmp17F;struct Cyc_List_List*_tmp180;int _tmp181;struct Cyc_Absyn_Exp*_tmp182;_LLBC:
if(_tmp174 <= (void*)2)goto _LLC2;if(*((int*)_tmp174)!= 12)goto _LLBE;_tmp175=((
struct Cyc_Absyn_UnknownId_p_struct*)_tmp174)->f1;_LLBD: return Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_UnknownId_e_struct*_tmp183=_cycalloc(sizeof(*_tmp183));
_tmp183[0]=({struct Cyc_Absyn_UnknownId_e_struct _tmp184;_tmp184.tag=2;_tmp184.f1=
_tmp175;_tmp184;});_tmp183;}),p->loc);_LLBE: if(*((int*)_tmp174)!= 1)goto _LLC0;
_tmp176=((struct Cyc_Absyn_Reference_p_struct*)_tmp174)->f1;_tmp177=((struct Cyc_Absyn_Reference_p_struct*)
_tmp174)->f2;_tmp178=*_tmp177;_tmp179=(void*)_tmp178.r;if((int)_tmp179 != 0)goto
_LLC0;_LLBF: return Cyc_Absyn_deref_exp(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp185=_cycalloc(sizeof(*_tmp185));_tmp185[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp186;_tmp186.tag=2;_tmp186.f1=_tmp176->name;_tmp186;});_tmp185;}),p->loc),p->loc);
_LLC0: if(*((int*)_tmp174)!= 4)goto _LLC2;_tmp17A=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp174)->f1;_LLC1: return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(_tmp17A),p->loc);
_LLC2: if((int)_tmp174 != 1)goto _LLC4;_LLC3: return Cyc_Absyn_null_exp(p->loc);_LLC4:
if(_tmp174 <= (void*)2)goto _LLCE;if(*((int*)_tmp174)!= 7)goto _LLC6;_tmp17B=(void*)((
struct Cyc_Absyn_Int_p_struct*)_tmp174)->f1;_tmp17C=((struct Cyc_Absyn_Int_p_struct*)
_tmp174)->f2;_LLC5: return Cyc_Absyn_int_exp(_tmp17B,_tmp17C,p->loc);_LLC6: if(*((
int*)_tmp174)!= 8)goto _LLC8;_tmp17D=((struct Cyc_Absyn_Char_p_struct*)_tmp174)->f1;
_LLC7: return Cyc_Absyn_char_exp(_tmp17D,p->loc);_LLC8: if(*((int*)_tmp174)!= 9)
goto _LLCA;_tmp17E=((struct Cyc_Absyn_Float_p_struct*)_tmp174)->f1;_LLC9: return Cyc_Absyn_float_exp(
_tmp17E,p->loc);_LLCA: if(*((int*)_tmp174)!= 13)goto _LLCC;_tmp17F=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp174)->f1;_tmp180=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp174)->f2;
_tmp181=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp174)->f3;if(_tmp181 != 0)goto
_LLCC;_LLCB: {struct Cyc_Absyn_Exp*e1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnknownId_e_struct*
_tmp187=_cycalloc(sizeof(*_tmp187));_tmp187[0]=({struct Cyc_Absyn_UnknownId_e_struct
_tmp188;_tmp188.tag=2;_tmp188.f1=_tmp17F;_tmp188;});_tmp187;}),p->loc);struct Cyc_List_List*
es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp180);return Cyc_Absyn_unknowncall_exp(
e1,es,p->loc);}_LLCC: if(*((int*)_tmp174)!= 14)goto _LLCE;_tmp182=((struct Cyc_Absyn_Exp_p_struct*)
_tmp174)->f1;_LLCD: return _tmp182;_LLCE:;_LLCF: Cyc_Parse_err(({const char*_tmp189="cannot mix patterns and expressions in case";
_tag_dynforward(_tmp189,sizeof(char),_get_zero_arr_size_char(_tmp189,44));}),p->loc);
return Cyc_Absyn_null_exp(p->loc);_LLBB:;}struct _tuple11{void*f1;int f2;};struct
Cyc_Int_tok_struct{int tag;struct _tuple11 f1;};struct Cyc_Char_tok_struct{int tag;
char f1;};struct Cyc_String_tok_struct{int tag;struct _dynforward_ptr f1;};struct Cyc_Stringopt_tok_struct{
int tag;struct Cyc_Core_Opt*f1;};struct Cyc_QualId_tok_struct{int tag;struct _tuple1*
f1;};struct _tuple12{struct Cyc_Position_Segment*f1;struct Cyc_Absyn_Conref*f2;
struct Cyc_Absyn_Conref*f3;};struct Cyc_YY1_struct{int tag;struct _tuple12*f1;};
struct Cyc_YY2_struct{int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_YY3_struct{int
tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_YY4_struct{int tag;struct Cyc_Absyn_Exp*f1;
};struct Cyc_YY5_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY6_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY7_struct{int tag;void*f1;};struct Cyc_YY8_struct{
int tag;struct Cyc_Core_Opt*f1;};struct Cyc_YY9_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_YY10_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY11_struct{
int tag;struct Cyc_Absyn_Pat*f1;};struct _tuple13{struct Cyc_List_List*f1;int f2;};
struct Cyc_YY12_struct{int tag;struct _tuple13*f1;};struct Cyc_YY13_struct{int tag;
struct Cyc_List_List*f1;};struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};struct Cyc_YY14_struct{int tag;struct _tuple14*f1;};struct Cyc_YY15_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY16_struct{int tag;struct _tuple13*f1;};
struct Cyc_YY17_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_YY18_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY19_struct{int tag;struct Cyc_Parse_Declaration_spec*
f1;};struct _tuple15{struct Cyc_Parse_Declarator*f1;struct Cyc_Absyn_Exp*f2;};
struct Cyc_YY20_struct{int tag;struct _tuple15*f1;};struct Cyc_YY21_struct{int tag;
struct Cyc_List_List*f1;};struct Cyc_YY22_struct{int tag;void*f1;};struct Cyc_YY23_struct{
int tag;void*f1;};struct Cyc_YY24_struct{int tag;void*f1;};struct Cyc_YY25_struct{
int tag;struct Cyc_Absyn_Tqual f1;};struct Cyc_YY26_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_YY27_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YY28_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_YY29_struct{int tag;struct Cyc_Parse_Declarator*
f1;};struct Cyc_YY30_struct{int tag;struct Cyc_Parse_Abstractdeclarator*f1;};struct
Cyc_YY31_struct{int tag;int f1;};struct Cyc_YY32_struct{int tag;void*f1;};struct Cyc_YY33_struct{
int tag;struct Cyc_Absyn_Tunionfield*f1;};struct Cyc_YY34_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple16{struct Cyc_Absyn_Tqual f1;struct Cyc_List_List*f2;struct Cyc_List_List*
f3;};struct Cyc_YY35_struct{int tag;struct _tuple16*f1;};struct Cyc_YY36_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY37_struct{int tag;struct _tuple2*f1;};
struct Cyc_YY38_struct{int tag;struct Cyc_List_List*f1;};struct _tuple17{struct Cyc_List_List*
f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*
f5;};struct Cyc_YY39_struct{int tag;struct _tuple17*f1;};struct Cyc_YY40_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY41_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_YY42_struct{int tag;void*f1;};struct Cyc_YY43_struct{int tag;void*f1;};
struct Cyc_YY44_struct{int tag;void*f1;};struct Cyc_YY45_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_YY46_struct{int tag;void*f1;};struct Cyc_YY47_struct{int tag;struct
Cyc_Absyn_Enumfield*f1;};struct Cyc_YY48_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_YY49_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_YY50_struct{int
tag;struct Cyc_List_List*f1;};struct Cyc_YY51_struct{int tag;struct Cyc_Absyn_Conref*
f1;};struct Cyc_YY52_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_YYINITIALSVAL_struct{
int tag;};union Cyc_YYSTYPE_union{struct Cyc_Int_tok_struct Int_tok;struct Cyc_Char_tok_struct
Char_tok;struct Cyc_String_tok_struct String_tok;struct Cyc_Stringopt_tok_struct
Stringopt_tok;struct Cyc_QualId_tok_struct QualId_tok;struct Cyc_YY1_struct YY1;
struct Cyc_YY2_struct YY2;struct Cyc_YY3_struct YY3;struct Cyc_YY4_struct YY4;struct
Cyc_YY5_struct YY5;struct Cyc_YY6_struct YY6;struct Cyc_YY7_struct YY7;struct Cyc_YY8_struct
YY8;struct Cyc_YY9_struct YY9;struct Cyc_YY10_struct YY10;struct Cyc_YY11_struct YY11;
struct Cyc_YY12_struct YY12;struct Cyc_YY13_struct YY13;struct Cyc_YY14_struct YY14;
struct Cyc_YY15_struct YY15;struct Cyc_YY16_struct YY16;struct Cyc_YY17_struct YY17;
struct Cyc_YY18_struct YY18;struct Cyc_YY19_struct YY19;struct Cyc_YY20_struct YY20;
struct Cyc_YY21_struct YY21;struct Cyc_YY22_struct YY22;struct Cyc_YY23_struct YY23;
struct Cyc_YY24_struct YY24;struct Cyc_YY25_struct YY25;struct Cyc_YY26_struct YY26;
struct Cyc_YY27_struct YY27;struct Cyc_YY28_struct YY28;struct Cyc_YY29_struct YY29;
struct Cyc_YY30_struct YY30;struct Cyc_YY31_struct YY31;struct Cyc_YY32_struct YY32;
struct Cyc_YY33_struct YY33;struct Cyc_YY34_struct YY34;struct Cyc_YY35_struct YY35;
struct Cyc_YY36_struct YY36;struct Cyc_YY37_struct YY37;struct Cyc_YY38_struct YY38;
struct Cyc_YY39_struct YY39;struct Cyc_YY40_struct YY40;struct Cyc_YY41_struct YY41;
struct Cyc_YY42_struct YY42;struct Cyc_YY43_struct YY43;struct Cyc_YY44_struct YY44;
struct Cyc_YY45_struct YY45;struct Cyc_YY46_struct YY46;struct Cyc_YY47_struct YY47;
struct Cyc_YY48_struct YY48;struct Cyc_YY49_struct YY49;struct Cyc_YY50_struct YY50;
struct Cyc_YY51_struct YY51;struct Cyc_YY52_struct YY52;struct Cyc_YYINITIALSVAL_struct
YYINITIALSVAL;};static char _tmp18B[14]="$(sign_t,int)";static struct Cyc_Core_Failure_struct
Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp18B,_tmp18B + 14}};struct _tuple11 Cyc_yyget_Int_tok(
union Cyc_YYSTYPE_union yy1){struct _tuple11 yyzzz;{union Cyc_YYSTYPE_union _tmp18C=
yy1;struct _tuple11 _tmp18D;_LLD1: if((_tmp18C.Int_tok).tag != 0)goto _LLD3;_tmp18D=(
_tmp18C.Int_tok).f1;_LLD2: yyzzz=_tmp18D;goto _LLD0;_LLD3:;_LLD4:(int)_throw((void*)&
Cyc_yyfail_Int_tok);_LLD0:;}return yyzzz;}static char _tmp18F[5]="char";static
struct Cyc_Core_Failure_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp18F,
_tmp18F + 5}};char Cyc_yyget_Char_tok(union Cyc_YYSTYPE_union yy1){char yyzzz;{union
Cyc_YYSTYPE_union _tmp190=yy1;char _tmp191;_LLD6: if((_tmp190.Char_tok).tag != 1)
goto _LLD8;_tmp191=(_tmp190.Char_tok).f1;_LLD7: yyzzz=_tmp191;goto _LLD5;_LLD8:;
_LLD9:(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLD5:;}return yyzzz;}static char
_tmp193[9]="string_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_String_tok={
Cyc_Core_Failure,{_tmp193,_tmp193 + 9}};struct _dynforward_ptr Cyc_yyget_String_tok(
union Cyc_YYSTYPE_union yy1){struct _dynforward_ptr yyzzz;{union Cyc_YYSTYPE_union
_tmp194=yy1;struct _dynforward_ptr _tmp195;_LLDB: if((_tmp194.String_tok).tag != 2)
goto _LLDD;_tmp195=(_tmp194.String_tok).f1;_LLDC: yyzzz=_tmp195;goto _LLDA;_LLDD:;
_LLDE:(int)_throw((void*)& Cyc_yyfail_String_tok);_LLDA:;}return yyzzz;}static char
_tmp197[54]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp197,_tmp197 + 54}};
struct _tuple12*Cyc_yyget_YY1(union Cyc_YYSTYPE_union yy1){struct _tuple12*yyzzz;{
union Cyc_YYSTYPE_union _tmp198=yy1;struct _tuple12*_tmp199;_LLE0: if((_tmp198.YY1).tag
!= 5)goto _LLE2;_tmp199=(_tmp198.YY1).f1;_LLE1: yyzzz=_tmp199;goto _LLDF;_LLE2:;
_LLE3:(int)_throw((void*)& Cyc_yyfail_YY1);_LLDF:;}return yyzzz;}static char _tmp19B[
19]="conref_t<bounds_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{
_tmp19B,_tmp19B + 19}};struct Cyc_Absyn_Conref*Cyc_yyget_YY2(union Cyc_YYSTYPE_union
yy1){struct Cyc_Absyn_Conref*yyzzz;{union Cyc_YYSTYPE_union _tmp19C=yy1;struct Cyc_Absyn_Conref*
_tmp19D;_LLE5: if((_tmp19C.YY2).tag != 6)goto _LLE7;_tmp19D=(_tmp19C.YY2).f1;_LLE6:
yyzzz=_tmp19D;goto _LLE4;_LLE7:;_LLE8:(int)_throw((void*)& Cyc_yyfail_YY2);_LLE4:;}
return yyzzz;}static char _tmp19F[6]="exp_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY3={
Cyc_Core_Failure,{_tmp19F,_tmp19F + 6}};struct Cyc_Absyn_Exp*Cyc_yyget_YY3(union
Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Exp*yyzzz;{union Cyc_YYSTYPE_union _tmp1A0=
yy1;struct Cyc_Absyn_Exp*_tmp1A1;_LLEA: if((_tmp1A0.YY3).tag != 7)goto _LLEC;_tmp1A1=(
_tmp1A0.YY3).f1;_LLEB: yyzzz=_tmp1A1;goto _LLE9;_LLEC:;_LLED:(int)_throw((void*)&
Cyc_yyfail_YY3);_LLE9:;}return yyzzz;}static char _tmp1A3[10]="exp_opt_t";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1A3,_tmp1A3 + 
10}};struct Cyc_Absyn_Exp*Cyc_yyget_YY4(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Exp*
yyzzz;{union Cyc_YYSTYPE_union _tmp1A4=yy1;struct Cyc_Absyn_Exp*_tmp1A5;_LLEF: if((
_tmp1A4.YY4).tag != 8)goto _LLF1;_tmp1A5=(_tmp1A4.YY4).f1;_LLF0: yyzzz=_tmp1A5;goto
_LLEE;_LLF1:;_LLF2:(int)_throw((void*)& Cyc_yyfail_YY4);_LLEE:;}return yyzzz;}
static char _tmp1A7[14]="list_t<exp_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY5={
Cyc_Core_Failure,{_tmp1A7,_tmp1A7 + 14}};struct Cyc_List_List*Cyc_yyget_YY5(union
Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1A8=
yy1;struct Cyc_List_List*_tmp1A9;_LLF4: if((_tmp1A8.YY5).tag != 9)goto _LLF6;_tmp1A9=(
_tmp1A8.YY5).f1;_LLF5: yyzzz=_tmp1A9;goto _LLF3;_LLF6:;_LLF7:(int)_throw((void*)&
Cyc_yyfail_YY5);_LLF3:;}return yyzzz;}static char _tmp1AB[39]="list_t<$(list_t<designator_t>,exp_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp1AB,
_tmp1AB + 39}};struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1AC=yy1;struct Cyc_List_List*
_tmp1AD;_LLF9: if((_tmp1AC.YY6).tag != 10)goto _LLFB;_tmp1AD=(_tmp1AC.YY6).f1;_LLFA:
yyzzz=_tmp1AD;goto _LLF8;_LLFB:;_LLFC:(int)_throw((void*)& Cyc_yyfail_YY6);_LLF8:;}
return yyzzz;}static char _tmp1AF[9]="primop_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp1AF,_tmp1AF + 9}};void*Cyc_yyget_YY7(union
Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp1B0=yy1;void*_tmp1B1;
_LLFE: if((_tmp1B0.YY7).tag != 11)goto _LL100;_tmp1B1=(_tmp1B0.YY7).f1;_LLFF: yyzzz=
_tmp1B1;goto _LLFD;_LL100:;_LL101:(int)_throw((void*)& Cyc_yyfail_YY7);_LLFD:;}
return yyzzz;}static char _tmp1B3[16]="opt_t<primop_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp1B3,_tmp1B3 + 16}};struct Cyc_Core_Opt*Cyc_yyget_YY8(
union Cyc_YYSTYPE_union yy1){struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE_union
_tmp1B4=yy1;struct Cyc_Core_Opt*_tmp1B5;_LL103: if((_tmp1B4.YY8).tag != 12)goto
_LL105;_tmp1B5=(_tmp1B4.YY8).f1;_LL104: yyzzz=_tmp1B5;goto _LL102;_LL105:;_LL106:(
int)_throw((void*)& Cyc_yyfail_YY8);_LL102:;}return yyzzz;}static char _tmp1B7[7]="qvar_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{
_tmp1B7,_tmp1B7 + 7}};struct _tuple1*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE_union
yy1){struct _tuple1*yyzzz;{union Cyc_YYSTYPE_union _tmp1B8=yy1;struct _tuple1*
_tmp1B9;_LL108: if((_tmp1B8.QualId_tok).tag != 4)goto _LL10A;_tmp1B9=(_tmp1B8.QualId_tok).f1;
_LL109: yyzzz=_tmp1B9;goto _LL107;_LL10A:;_LL10B:(int)_throw((void*)& Cyc_yyfail_QualId_tok);
_LL107:;}return yyzzz;}static char _tmp1BB[7]="stmt_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp1BB,_tmp1BB + 7}};struct Cyc_Absyn_Stmt*Cyc_yyget_YY9(
union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Stmt*yyzzz;{union Cyc_YYSTYPE_union
_tmp1BC=yy1;struct Cyc_Absyn_Stmt*_tmp1BD;_LL10D: if((_tmp1BC.YY9).tag != 13)goto
_LL10F;_tmp1BD=(_tmp1BC.YY9).f1;_LL10E: yyzzz=_tmp1BD;goto _LL10C;_LL10F:;_LL110:(
int)_throw((void*)& Cyc_yyfail_YY9);_LL10C:;}return yyzzz;}static char _tmp1BF[24]="list_t<switch_clause_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp1BF,
_tmp1BF + 24}};struct Cyc_List_List*Cyc_yyget_YY10(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1C0=yy1;struct Cyc_List_List*
_tmp1C1;_LL112: if((_tmp1C0.YY10).tag != 14)goto _LL114;_tmp1C1=(_tmp1C0.YY10).f1;
_LL113: yyzzz=_tmp1C1;goto _LL111;_LL114:;_LL115:(int)_throw((void*)& Cyc_yyfail_YY10);
_LL111:;}return yyzzz;}static char _tmp1C3[6]="pat_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp1C3,_tmp1C3 + 6}};struct Cyc_Absyn_Pat*Cyc_yyget_YY11(
union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Pat*yyzzz;{union Cyc_YYSTYPE_union
_tmp1C4=yy1;struct Cyc_Absyn_Pat*_tmp1C5;_LL117: if((_tmp1C4.YY11).tag != 15)goto
_LL119;_tmp1C5=(_tmp1C4.YY11).f1;_LL118: yyzzz=_tmp1C5;goto _LL116;_LL119:;_LL11A:(
int)_throw((void*)& Cyc_yyfail_YY11);_LL116:;}return yyzzz;}static char _tmp1C7[23]="$(list_t<pat_t>,bool)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp1C7,
_tmp1C7 + 23}};struct _tuple13*Cyc_yyget_YY12(union Cyc_YYSTYPE_union yy1){struct
_tuple13*yyzzz;{union Cyc_YYSTYPE_union _tmp1C8=yy1;struct _tuple13*_tmp1C9;_LL11C:
if((_tmp1C8.YY12).tag != 16)goto _LL11E;_tmp1C9=(_tmp1C8.YY12).f1;_LL11D: yyzzz=
_tmp1C9;goto _LL11B;_LL11E:;_LL11F:(int)_throw((void*)& Cyc_yyfail_YY12);_LL11B:;}
return yyzzz;}static char _tmp1CB[14]="list_t<pat_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp1CB,_tmp1CB + 14}};struct Cyc_List_List*Cyc_yyget_YY13(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp1CC=yy1;struct Cyc_List_List*_tmp1CD;_LL121: if((_tmp1CC.YY13).tag != 17)goto
_LL123;_tmp1CD=(_tmp1CC.YY13).f1;_LL122: yyzzz=_tmp1CD;goto _LL120;_LL123:;_LL124:(
int)_throw((void*)& Cyc_yyfail_YY13);_LL120:;}return yyzzz;}static char _tmp1CF[31]="$(list_t<designator_t>,pat_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp1CF,
_tmp1CF + 31}};struct _tuple14*Cyc_yyget_YY14(union Cyc_YYSTYPE_union yy1){struct
_tuple14*yyzzz;{union Cyc_YYSTYPE_union _tmp1D0=yy1;struct _tuple14*_tmp1D1;_LL126:
if((_tmp1D0.YY14).tag != 18)goto _LL128;_tmp1D1=(_tmp1D0.YY14).f1;_LL127: yyzzz=
_tmp1D1;goto _LL125;_LL128:;_LL129:(int)_throw((void*)& Cyc_yyfail_YY14);_LL125:;}
return yyzzz;}static char _tmp1D3[39]="list_t<$(list_t<designator_t>,pat_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp1D3,
_tmp1D3 + 39}};struct Cyc_List_List*Cyc_yyget_YY15(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1D4=yy1;struct Cyc_List_List*
_tmp1D5;_LL12B: if((_tmp1D4.YY15).tag != 19)goto _LL12D;_tmp1D5=(_tmp1D4.YY15).f1;
_LL12C: yyzzz=_tmp1D5;goto _LL12A;_LL12D:;_LL12E:(int)_throw((void*)& Cyc_yyfail_YY15);
_LL12A:;}return yyzzz;}static char _tmp1D7[48]="$(list_t<$(list_t<designator_t>,pat_t)@>,bool)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp1D7,
_tmp1D7 + 48}};struct _tuple13*Cyc_yyget_YY16(union Cyc_YYSTYPE_union yy1){struct
_tuple13*yyzzz;{union Cyc_YYSTYPE_union _tmp1D8=yy1;struct _tuple13*_tmp1D9;_LL130:
if((_tmp1D8.YY16).tag != 20)goto _LL132;_tmp1D9=(_tmp1D8.YY16).f1;_LL131: yyzzz=
_tmp1D9;goto _LL12F;_LL132:;_LL133:(int)_throw((void*)& Cyc_yyfail_YY16);_LL12F:;}
return yyzzz;}static char _tmp1DB[9]="fndecl_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp1DB,_tmp1DB + 9}};struct Cyc_Absyn_Fndecl*
Cyc_yyget_YY17(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Fndecl*yyzzz;{union Cyc_YYSTYPE_union
_tmp1DC=yy1;struct Cyc_Absyn_Fndecl*_tmp1DD;_LL135: if((_tmp1DC.YY17).tag != 21)
goto _LL137;_tmp1DD=(_tmp1DC.YY17).f1;_LL136: yyzzz=_tmp1DD;goto _LL134;_LL137:;
_LL138:(int)_throw((void*)& Cyc_yyfail_YY17);_LL134:;}return yyzzz;}static char
_tmp1DF[15]="list_t<decl_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY18={
Cyc_Core_Failure,{_tmp1DF,_tmp1DF + 15}};struct Cyc_List_List*Cyc_yyget_YY18(union
Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp1E0=
yy1;struct Cyc_List_List*_tmp1E1;_LL13A: if((_tmp1E0.YY18).tag != 22)goto _LL13C;
_tmp1E1=(_tmp1E0.YY18).f1;_LL13B: yyzzz=_tmp1E1;goto _LL139;_LL13C:;_LL13D:(int)
_throw((void*)& Cyc_yyfail_YY18);_LL139:;}return yyzzz;}static char _tmp1E3[12]="decl_spec_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp1E3,
_tmp1E3 + 12}};struct Cyc_Parse_Declaration_spec*Cyc_yyget_YY19(union Cyc_YYSTYPE_union
yy1){struct Cyc_Parse_Declaration_spec*yyzzz;{union Cyc_YYSTYPE_union _tmp1E4=yy1;
struct Cyc_Parse_Declaration_spec*_tmp1E5;_LL13F: if((_tmp1E4.YY19).tag != 23)goto
_LL141;_tmp1E5=(_tmp1E4.YY19).f1;_LL140: yyzzz=_tmp1E5;goto _LL13E;_LL141:;_LL142:(
int)_throw((void*)& Cyc_yyfail_YY19);_LL13E:;}return yyzzz;}static char _tmp1E7[27]="$(declarator_t,exp_opt_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp1E7,
_tmp1E7 + 27}};struct _tuple15*Cyc_yyget_YY20(union Cyc_YYSTYPE_union yy1){struct
_tuple15*yyzzz;{union Cyc_YYSTYPE_union _tmp1E8=yy1;struct _tuple15*_tmp1E9;_LL144:
if((_tmp1E8.YY20).tag != 24)goto _LL146;_tmp1E9=(_tmp1E8.YY20).f1;_LL145: yyzzz=
_tmp1E9;goto _LL143;_LL146:;_LL147:(int)_throw((void*)& Cyc_yyfail_YY20);_LL143:;}
return yyzzz;}static char _tmp1EB[35]="list_t<$(declarator_t,exp_opt_t)@>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp1EB,_tmp1EB + 
35}};struct Cyc_List_List*Cyc_yyget_YY21(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp1EC=yy1;struct Cyc_List_List*_tmp1ED;_LL149: if((
_tmp1EC.YY21).tag != 25)goto _LL14B;_tmp1ED=(_tmp1EC.YY21).f1;_LL14A: yyzzz=_tmp1ED;
goto _LL148;_LL14B:;_LL14C:(int)_throw((void*)& Cyc_yyfail_YY21);_LL148:;}return
yyzzz;}static char _tmp1EF[16]="storage_class_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp1EF,_tmp1EF + 16}};void*Cyc_yyget_YY22(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp1F0=yy1;void*
_tmp1F1;_LL14E: if((_tmp1F0.YY22).tag != 26)goto _LL150;_tmp1F1=(_tmp1F0.YY22).f1;
_LL14F: yyzzz=_tmp1F1;goto _LL14D;_LL150:;_LL151:(int)_throw((void*)& Cyc_yyfail_YY22);
_LL14D:;}return yyzzz;}static char _tmp1F3[17]="type_specifier_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp1F3,_tmp1F3 + 17}};void*Cyc_yyget_YY23(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp1F4=yy1;void*
_tmp1F5;_LL153: if((_tmp1F4.YY23).tag != 27)goto _LL155;_tmp1F5=(_tmp1F4.YY23).f1;
_LL154: yyzzz=_tmp1F5;goto _LL152;_LL155:;_LL156:(int)_throw((void*)& Cyc_yyfail_YY23);
_LL152:;}return yyzzz;}static char _tmp1F7[12]="aggr_kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp1F7,_tmp1F7 + 12}};void*Cyc_yyget_YY24(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp1F8=yy1;void*
_tmp1F9;_LL158: if((_tmp1F8.YY24).tag != 28)goto _LL15A;_tmp1F9=(_tmp1F8.YY24).f1;
_LL159: yyzzz=_tmp1F9;goto _LL157;_LL15A:;_LL15B:(int)_throw((void*)& Cyc_yyfail_YY24);
_LL157:;}return yyzzz;}static char _tmp1FB[8]="tqual_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp1FB,_tmp1FB + 8}};struct Cyc_Absyn_Tqual Cyc_yyget_YY25(
union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Tqual yyzzz;{union Cyc_YYSTYPE_union
_tmp1FC=yy1;struct Cyc_Absyn_Tqual _tmp1FD;_LL15D: if((_tmp1FC.YY25).tag != 29)goto
_LL15F;_tmp1FD=(_tmp1FC.YY25).f1;_LL15E: yyzzz=_tmp1FD;goto _LL15C;_LL15F:;_LL160:(
int)_throw((void*)& Cyc_yyfail_YY25);_LL15C:;}return yyzzz;}static char _tmp1FF[20]="list_t<aggrfield_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp1FF,
_tmp1FF + 20}};struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp200=yy1;struct Cyc_List_List*
_tmp201;_LL162: if((_tmp200.YY26).tag != 30)goto _LL164;_tmp201=(_tmp200.YY26).f1;
_LL163: yyzzz=_tmp201;goto _LL161;_LL164:;_LL165:(int)_throw((void*)& Cyc_yyfail_YY26);
_LL161:;}return yyzzz;}static char _tmp203[28]="list_t<list_t<aggrfield_t>>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp203,_tmp203 + 
28}};struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp204=yy1;struct Cyc_List_List*_tmp205;_LL167: if((
_tmp204.YY27).tag != 31)goto _LL169;_tmp205=(_tmp204.YY27).f1;_LL168: yyzzz=_tmp205;
goto _LL166;_LL169:;_LL16A:(int)_throw((void*)& Cyc_yyfail_YY27);_LL166:;}return
yyzzz;}static char _tmp207[24]="list_t<type_modifier_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp207,_tmp207 + 24}};struct Cyc_List_List*Cyc_yyget_YY28(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp208=yy1;struct Cyc_List_List*_tmp209;_LL16C: if((_tmp208.YY28).tag != 32)goto
_LL16E;_tmp209=(_tmp208.YY28).f1;_LL16D: yyzzz=_tmp209;goto _LL16B;_LL16E:;_LL16F:(
int)_throw((void*)& Cyc_yyfail_YY28);_LL16B:;}return yyzzz;}static char _tmp20B[13]="declarator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp20B,
_tmp20B + 13}};struct Cyc_Parse_Declarator*Cyc_yyget_YY29(union Cyc_YYSTYPE_union
yy1){struct Cyc_Parse_Declarator*yyzzz;{union Cyc_YYSTYPE_union _tmp20C=yy1;struct
Cyc_Parse_Declarator*_tmp20D;_LL171: if((_tmp20C.YY29).tag != 33)goto _LL173;
_tmp20D=(_tmp20C.YY29).f1;_LL172: yyzzz=_tmp20D;goto _LL170;_LL173:;_LL174:(int)
_throw((void*)& Cyc_yyfail_YY29);_LL170:;}return yyzzz;}static char _tmp20F[21]="abstractdeclarator_t";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp20F,
_tmp20F + 21}};struct Cyc_Parse_Abstractdeclarator*Cyc_yyget_YY30(union Cyc_YYSTYPE_union
yy1){struct Cyc_Parse_Abstractdeclarator*yyzzz;{union Cyc_YYSTYPE_union _tmp210=yy1;
struct Cyc_Parse_Abstractdeclarator*_tmp211;_LL176: if((_tmp210.YY30).tag != 34)
goto _LL178;_tmp211=(_tmp210.YY30).f1;_LL177: yyzzz=_tmp211;goto _LL175;_LL178:;
_LL179:(int)_throw((void*)& Cyc_yyfail_YY30);_LL175:;}return yyzzz;}static char
_tmp213[5]="bool";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{
_tmp213,_tmp213 + 5}};int Cyc_yyget_YY31(union Cyc_YYSTYPE_union yy1){int yyzzz;{
union Cyc_YYSTYPE_union _tmp214=yy1;int _tmp215;_LL17B: if((_tmp214.YY31).tag != 35)
goto _LL17D;_tmp215=(_tmp214.YY31).f1;_LL17C: yyzzz=_tmp215;goto _LL17A;_LL17D:;
_LL17E:(int)_throw((void*)& Cyc_yyfail_YY31);_LL17A:;}return yyzzz;}static char
_tmp217[8]="scope_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{
_tmp217,_tmp217 + 8}};void*Cyc_yyget_YY32(union Cyc_YYSTYPE_union yy1){void*yyzzz;{
union Cyc_YYSTYPE_union _tmp218=yy1;void*_tmp219;_LL180: if((_tmp218.YY32).tag != 36)
goto _LL182;_tmp219=(_tmp218.YY32).f1;_LL181: yyzzz=_tmp219;goto _LL17F;_LL182:;
_LL183:(int)_throw((void*)& Cyc_yyfail_YY32);_LL17F:;}return yyzzz;}static char
_tmp21B[14]="tunionfield_t";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY33={
Cyc_Core_Failure,{_tmp21B,_tmp21B + 14}};struct Cyc_Absyn_Tunionfield*Cyc_yyget_YY33(
union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Tunionfield*yyzzz;{union Cyc_YYSTYPE_union
_tmp21C=yy1;struct Cyc_Absyn_Tunionfield*_tmp21D;_LL185: if((_tmp21C.YY33).tag != 
37)goto _LL187;_tmp21D=(_tmp21C.YY33).f1;_LL186: yyzzz=_tmp21D;goto _LL184;_LL187:;
_LL188:(int)_throw((void*)& Cyc_yyfail_YY33);_LL184:;}return yyzzz;}static char
_tmp21F[22]="list_t<tunionfield_t>";static struct Cyc_Core_Failure_struct Cyc_yyfail_YY34={
Cyc_Core_Failure,{_tmp21F,_tmp21F + 22}};struct Cyc_List_List*Cyc_yyget_YY34(union
Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp220=
yy1;struct Cyc_List_List*_tmp221;_LL18A: if((_tmp220.YY34).tag != 38)goto _LL18C;
_tmp221=(_tmp220.YY34).f1;_LL18B: yyzzz=_tmp221;goto _LL189;_LL18C:;_LL18D:(int)
_throw((void*)& Cyc_yyfail_YY34);_LL189:;}return yyzzz;}static char _tmp223[50]="$(tqual_t,list_t<type_specifier_t>,attributes_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp223,
_tmp223 + 50}};struct _tuple16*Cyc_yyget_YY35(union Cyc_YYSTYPE_union yy1){struct
_tuple16*yyzzz;{union Cyc_YYSTYPE_union _tmp224=yy1;struct _tuple16*_tmp225;_LL18F:
if((_tmp224.YY35).tag != 39)goto _LL191;_tmp225=(_tmp224.YY35).f1;_LL190: yyzzz=
_tmp225;goto _LL18E;_LL191:;_LL192:(int)_throw((void*)& Cyc_yyfail_YY35);_LL18E:;}
return yyzzz;}static char _tmp227[14]="list_t<var_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp227,_tmp227 + 14}};struct Cyc_List_List*Cyc_yyget_YY36(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp228=yy1;struct Cyc_List_List*_tmp229;_LL194: if((_tmp228.YY36).tag != 40)goto
_LL196;_tmp229=(_tmp228.YY36).f1;_LL195: yyzzz=_tmp229;goto _LL193;_LL196:;_LL197:(
int)_throw((void*)& Cyc_yyfail_YY36);_LL193:;}return yyzzz;}static char _tmp22B[32]="$(opt_t<var_t>,tqual_t,type_t)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp22B,
_tmp22B + 32}};struct _tuple2*Cyc_yyget_YY37(union Cyc_YYSTYPE_union yy1){struct
_tuple2*yyzzz;{union Cyc_YYSTYPE_union _tmp22C=yy1;struct _tuple2*_tmp22D;_LL199:
if((_tmp22C.YY37).tag != 41)goto _LL19B;_tmp22D=(_tmp22C.YY37).f1;_LL19A: yyzzz=
_tmp22D;goto _LL198;_LL19B:;_LL19C:(int)_throw((void*)& Cyc_yyfail_YY37);_LL198:;}
return yyzzz;}static char _tmp22F[40]="list_t<$(opt_t<var_t>,tqual_t,type_t)@>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp22F,
_tmp22F + 40}};struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp230=yy1;struct Cyc_List_List*
_tmp231;_LL19E: if((_tmp230.YY38).tag != 42)goto _LL1A0;_tmp231=(_tmp230.YY38).f1;
_LL19F: yyzzz=_tmp231;goto _LL19D;_LL1A0:;_LL1A1:(int)_throw((void*)& Cyc_yyfail_YY38);
_LL19D:;}return yyzzz;}static char _tmp233[107]="$(list_t<$(opt_t<var_t>,tqual_t,type_t)@>, bool,vararg_info_t *,opt_t<type_t>, list_t<$(type_t,type_t)@>)@";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp233,
_tmp233 + 107}};struct _tuple17*Cyc_yyget_YY39(union Cyc_YYSTYPE_union yy1){struct
_tuple17*yyzzz;{union Cyc_YYSTYPE_union _tmp234=yy1;struct _tuple17*_tmp235;_LL1A3:
if((_tmp234.YY39).tag != 43)goto _LL1A5;_tmp235=(_tmp234.YY39).f1;_LL1A4: yyzzz=
_tmp235;goto _LL1A2;_LL1A5:;_LL1A6:(int)_throw((void*)& Cyc_yyfail_YY39);_LL1A2:;}
return yyzzz;}static char _tmp237[15]="list_t<type_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp237,_tmp237 + 15}};struct Cyc_List_List*Cyc_yyget_YY40(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp238=yy1;struct Cyc_List_List*_tmp239;_LL1A8: if((_tmp238.YY40).tag != 44)goto
_LL1AA;_tmp239=(_tmp238.YY40).f1;_LL1A9: yyzzz=_tmp239;goto _LL1A7;_LL1AA:;_LL1AB:(
int)_throw((void*)& Cyc_yyfail_YY40);_LL1A7:;}return yyzzz;}static char _tmp23B[21]="list_t<designator_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp23B,
_tmp23B + 21}};struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE_union yy1){
struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union _tmp23C=yy1;struct Cyc_List_List*
_tmp23D;_LL1AD: if((_tmp23C.YY41).tag != 45)goto _LL1AF;_tmp23D=(_tmp23C.YY41).f1;
_LL1AE: yyzzz=_tmp23D;goto _LL1AC;_LL1AF:;_LL1B0:(int)_throw((void*)& Cyc_yyfail_YY41);
_LL1AC:;}return yyzzz;}static char _tmp23F[13]="designator_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp23F,_tmp23F + 13}};void*Cyc_yyget_YY42(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp240=yy1;void*
_tmp241;_LL1B2: if((_tmp240.YY42).tag != 46)goto _LL1B4;_tmp241=(_tmp240.YY42).f1;
_LL1B3: yyzzz=_tmp241;goto _LL1B1;_LL1B4:;_LL1B5:(int)_throw((void*)& Cyc_yyfail_YY42);
_LL1B1:;}return yyzzz;}static char _tmp243[7]="kind_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp243,_tmp243 + 7}};void*Cyc_yyget_YY43(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp244=yy1;void*
_tmp245;_LL1B7: if((_tmp244.YY43).tag != 47)goto _LL1B9;_tmp245=(_tmp244.YY43).f1;
_LL1B8: yyzzz=_tmp245;goto _LL1B6;_LL1B9:;_LL1BA:(int)_throw((void*)& Cyc_yyfail_YY43);
_LL1B6:;}return yyzzz;}static char _tmp247[7]="type_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp247,_tmp247 + 7}};void*Cyc_yyget_YY44(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp248=yy1;void*
_tmp249;_LL1BC: if((_tmp248.YY44).tag != 48)goto _LL1BE;_tmp249=(_tmp248.YY44).f1;
_LL1BD: yyzzz=_tmp249;goto _LL1BB;_LL1BE:;_LL1BF:(int)_throw((void*)& Cyc_yyfail_YY44);
_LL1BB:;}return yyzzz;}static char _tmp24B[20]="list_t<attribute_t>";static struct
Cyc_Core_Failure_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp24B,_tmp24B + 20}};
struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp24C=yy1;struct Cyc_List_List*_tmp24D;_LL1C1: if((
_tmp24C.YY45).tag != 49)goto _LL1C3;_tmp24D=(_tmp24C.YY45).f1;_LL1C2: yyzzz=_tmp24D;
goto _LL1C0;_LL1C3:;_LL1C4:(int)_throw((void*)& Cyc_yyfail_YY45);_LL1C0:;}return
yyzzz;}static char _tmp24F[12]="attribute_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp24F,_tmp24F + 12}};void*Cyc_yyget_YY46(
union Cyc_YYSTYPE_union yy1){void*yyzzz;{union Cyc_YYSTYPE_union _tmp250=yy1;void*
_tmp251;_LL1C6: if((_tmp250.YY46).tag != 50)goto _LL1C8;_tmp251=(_tmp250.YY46).f1;
_LL1C7: yyzzz=_tmp251;goto _LL1C5;_LL1C8:;_LL1C9:(int)_throw((void*)& Cyc_yyfail_YY46);
_LL1C5:;}return yyzzz;}static char _tmp253[12]="enumfield_t";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp253,_tmp253 + 12}};struct Cyc_Absyn_Enumfield*
Cyc_yyget_YY47(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Enumfield*yyzzz;{union
Cyc_YYSTYPE_union _tmp254=yy1;struct Cyc_Absyn_Enumfield*_tmp255;_LL1CB: if((
_tmp254.YY47).tag != 51)goto _LL1CD;_tmp255=(_tmp254.YY47).f1;_LL1CC: yyzzz=_tmp255;
goto _LL1CA;_LL1CD:;_LL1CE:(int)_throw((void*)& Cyc_yyfail_YY47);_LL1CA:;}return
yyzzz;}static char _tmp257[20]="list_t<enumfield_t>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp257,_tmp257 + 20}};struct Cyc_List_List*Cyc_yyget_YY48(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp258=yy1;struct Cyc_List_List*_tmp259;_LL1D0: if((_tmp258.YY48).tag != 52)goto
_LL1D2;_tmp259=(_tmp258.YY48).f1;_LL1D1: yyzzz=_tmp259;goto _LL1CF;_LL1D2:;_LL1D3:(
int)_throw((void*)& Cyc_yyfail_YY48);_LL1CF:;}return yyzzz;}static char _tmp25B[14]="opt_t<type_t>";
static struct Cyc_Core_Failure_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp25B,
_tmp25B + 14}};struct Cyc_Core_Opt*Cyc_yyget_YY49(union Cyc_YYSTYPE_union yy1){
struct Cyc_Core_Opt*yyzzz;{union Cyc_YYSTYPE_union _tmp25C=yy1;struct Cyc_Core_Opt*
_tmp25D;_LL1D5: if((_tmp25C.YY49).tag != 53)goto _LL1D7;_tmp25D=(_tmp25C.YY49).f1;
_LL1D6: yyzzz=_tmp25D;goto _LL1D4;_LL1D7:;_LL1D8:(int)_throw((void*)& Cyc_yyfail_YY49);
_LL1D4:;}return yyzzz;}static char _tmp25F[26]="list_t<$(type_t,type_t)@>";static
struct Cyc_Core_Failure_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp25F,_tmp25F + 
26}};struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*
yyzzz;{union Cyc_YYSTYPE_union _tmp260=yy1;struct Cyc_List_List*_tmp261;_LL1DA: if((
_tmp260.YY50).tag != 54)goto _LL1DC;_tmp261=(_tmp260.YY50).f1;_LL1DB: yyzzz=_tmp261;
goto _LL1D9;_LL1DC:;_LL1DD:(int)_throw((void*)& Cyc_yyfail_YY50);_LL1D9:;}return
yyzzz;}static char _tmp263[15]="conref_t<bool>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp263,_tmp263 + 15}};struct Cyc_Absyn_Conref*
Cyc_yyget_YY51(union Cyc_YYSTYPE_union yy1){struct Cyc_Absyn_Conref*yyzzz;{union Cyc_YYSTYPE_union
_tmp264=yy1;struct Cyc_Absyn_Conref*_tmp265;_LL1DF: if((_tmp264.YY51).tag != 55)
goto _LL1E1;_tmp265=(_tmp264.YY51).f1;_LL1E0: yyzzz=_tmp265;goto _LL1DE;_LL1E1:;
_LL1E2:(int)_throw((void*)& Cyc_yyfail_YY51);_LL1DE:;}return yyzzz;}static char
_tmp267[40]="list_t<$(Position::seg_t,qvar_t,bool)@>";static struct Cyc_Core_Failure_struct
Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp267,_tmp267 + 40}};struct Cyc_List_List*Cyc_yyget_YY52(
union Cyc_YYSTYPE_union yy1){struct Cyc_List_List*yyzzz;{union Cyc_YYSTYPE_union
_tmp268=yy1;struct Cyc_List_List*_tmp269;_LL1E4: if((_tmp268.YY52).tag != 56)goto
_LL1E6;_tmp269=(_tmp268.YY52).f1;_LL1E5: yyzzz=_tmp269;goto _LL1E3;_LL1E6:;_LL1E7:(
int)_throw((void*)& Cyc_yyfail_YY52);_LL1E3:;}return yyzzz;}struct Cyc_Yyltype{int
timestamp;int first_line;int first_column;int last_line;int last_column;};struct Cyc_Yyltype
Cyc_yynewloc(){return({struct Cyc_Yyltype _tmp26A;_tmp26A.timestamp=0;_tmp26A.first_line=
0;_tmp26A.first_column=0;_tmp26A.last_line=0;_tmp26A.last_column=0;_tmp26A;});}
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};static short Cyc_yytranslate[361]={0,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,132,2,2,114,130,128,2,
111,112,120,125,109,123,117,129,2,2,2,2,2,2,2,2,2,2,116,106,115,110,124,122,121,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,118,2,119,127,113,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,107,126,108,131,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,
2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,
58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,
85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105};static char
_tmp26B[2]="$";static char _tmp26C[6]="error";static char _tmp26D[12]="$undefined.";
static char _tmp26E[5]="AUTO";static char _tmp26F[9]="REGISTER";static char _tmp270[7]="STATIC";
static char _tmp271[7]="EXTERN";static char _tmp272[8]="TYPEDEF";static char _tmp273[5]="VOID";
static char _tmp274[5]="CHAR";static char _tmp275[6]="SHORT";static char _tmp276[4]="INT";
static char _tmp277[5]="LONG";static char _tmp278[6]="FLOAT";static char _tmp279[7]="DOUBLE";
static char _tmp27A[7]="SIGNED";static char _tmp27B[9]="UNSIGNED";static char _tmp27C[
6]="CONST";static char _tmp27D[9]="VOLATILE";static char _tmp27E[9]="RESTRICT";
static char _tmp27F[7]="STRUCT";static char _tmp280[6]="UNION";static char _tmp281[5]="CASE";
static char _tmp282[8]="DEFAULT";static char _tmp283[7]="INLINE";static char _tmp284[7]="SIZEOF";
static char _tmp285[9]="OFFSETOF";static char _tmp286[3]="IF";static char _tmp287[5]="ELSE";
static char _tmp288[7]="SWITCH";static char _tmp289[6]="WHILE";static char _tmp28A[3]="DO";
static char _tmp28B[4]="FOR";static char _tmp28C[5]="GOTO";static char _tmp28D[9]="CONTINUE";
static char _tmp28E[6]="BREAK";static char _tmp28F[7]="RETURN";static char _tmp290[5]="ENUM";
static char _tmp291[8]="NULL_kw";static char _tmp292[4]="LET";static char _tmp293[6]="THROW";
static char _tmp294[4]="TRY";static char _tmp295[6]="CATCH";static char _tmp296[7]="EXPORT";
static char _tmp297[4]="NEW";static char _tmp298[9]="ABSTRACT";static char _tmp299[9]="FALLTHRU";
static char _tmp29A[6]="USING";static char _tmp29B[10]="NAMESPACE";static char _tmp29C[
7]="TUNION";static char _tmp29D[8]="XTUNION";static char _tmp29E[7]="MALLOC";static
char _tmp29F[8]="RMALLOC";static char _tmp2A0[7]="CALLOC";static char _tmp2A1[8]="RCALLOC";
static char _tmp2A2[5]="SWAP";static char _tmp2A3[9]="REGION_T";static char _tmp2A4[9]="SIZEOF_T";
static char _tmp2A5[6]="TAG_T";static char _tmp2A6[7]="REGION";static char _tmp2A7[5]="RNEW";
static char _tmp2A8[8]="REGIONS";static char _tmp2A9[13]="RESET_REGION";static char
_tmp2AA[4]="GEN";static char _tmp2AB[14]="NOZEROTERM_kw";static char _tmp2AC[12]="ZEROTERM_kw";
static char _tmp2AD[7]="PORTON";static char _tmp2AE[8]="PORTOFF";static char _tmp2AF[8]="FLAT_kw";
static char _tmp2B0[12]="DYNREGION_T";static char _tmp2B1[6]="ALIAS";static char
_tmp2B2[7]="PTR_OP";static char _tmp2B3[7]="INC_OP";static char _tmp2B4[7]="DEC_OP";
static char _tmp2B5[8]="LEFT_OP";static char _tmp2B6[9]="RIGHT_OP";static char _tmp2B7[
6]="LE_OP";static char _tmp2B8[6]="GE_OP";static char _tmp2B9[6]="EQ_OP";static char
_tmp2BA[6]="NE_OP";static char _tmp2BB[7]="AND_OP";static char _tmp2BC[6]="OR_OP";
static char _tmp2BD[11]="MUL_ASSIGN";static char _tmp2BE[11]="DIV_ASSIGN";static char
_tmp2BF[11]="MOD_ASSIGN";static char _tmp2C0[11]="ADD_ASSIGN";static char _tmp2C1[11]="SUB_ASSIGN";
static char _tmp2C2[12]="LEFT_ASSIGN";static char _tmp2C3[13]="RIGHT_ASSIGN";static
char _tmp2C4[11]="AND_ASSIGN";static char _tmp2C5[11]="XOR_ASSIGN";static char
_tmp2C6[10]="OR_ASSIGN";static char _tmp2C7[9]="ELLIPSIS";static char _tmp2C8[11]="LEFT_RIGHT";
static char _tmp2C9[12]="COLON_COLON";static char _tmp2CA[11]="IDENTIFIER";static
char _tmp2CB[17]="INTEGER_CONSTANT";static char _tmp2CC[7]="STRING";static char
_tmp2CD[19]="CHARACTER_CONSTANT";static char _tmp2CE[18]="FLOATING_CONSTANT";
static char _tmp2CF[9]="TYPE_VAR";static char _tmp2D0[13]="TYPEDEF_NAME";static char
_tmp2D1[16]="QUAL_IDENTIFIER";static char _tmp2D2[18]="QUAL_TYPEDEF_NAME";static
char _tmp2D3[13]="TYPE_INTEGER";static char _tmp2D4[10]="ATTRIBUTE";static char
_tmp2D5[4]="';'";static char _tmp2D6[4]="'{'";static char _tmp2D7[4]="'}'";static
char _tmp2D8[4]="','";static char _tmp2D9[4]="'='";static char _tmp2DA[4]="'('";
static char _tmp2DB[4]="')'";static char _tmp2DC[4]="'_'";static char _tmp2DD[4]="'$'";
static char _tmp2DE[4]="'<'";static char _tmp2DF[4]="':'";static char _tmp2E0[4]="'.'";
static char _tmp2E1[4]="'['";static char _tmp2E2[4]="']'";static char _tmp2E3[4]="'*'";
static char _tmp2E4[4]="'@'";static char _tmp2E5[4]="'?'";static char _tmp2E6[4]="'-'";
static char _tmp2E7[4]="'>'";static char _tmp2E8[4]="'+'";static char _tmp2E9[4]="'|'";
static char _tmp2EA[4]="'^'";static char _tmp2EB[4]="'&'";static char _tmp2EC[4]="'/'";
static char _tmp2ED[4]="'%'";static char _tmp2EE[4]="'~'";static char _tmp2EF[4]="'!'";
static char _tmp2F0[5]="prog";static char _tmp2F1[17]="translation_unit";static char
_tmp2F2[12]="export_list";static char _tmp2F3[19]="export_list_values";static char
_tmp2F4[21]="external_declaration";static char _tmp2F5[20]="function_definition";
static char _tmp2F6[21]="function_definition2";static char _tmp2F7[13]="using_action";
static char _tmp2F8[15]="unusing_action";static char _tmp2F9[17]="namespace_action";
static char _tmp2FA[19]="unnamespace_action";static char _tmp2FB[12]="declaration";
static char _tmp2FC[17]="declaration_list";static char _tmp2FD[23]="declaration_specifiers";
static char _tmp2FE[24]="storage_class_specifier";static char _tmp2FF[15]="attributes_opt";
static char _tmp300[11]="attributes";static char _tmp301[15]="attribute_list";static
char _tmp302[10]="attribute";static char _tmp303[15]="type_specifier";static char
_tmp304[25]="type_specifier_notypedef";static char _tmp305[5]="kind";static char
_tmp306[15]="type_qualifier";static char _tmp307[15]="enum_specifier";static char
_tmp308[11]="enum_field";static char _tmp309[22]="enum_declaration_list";static
char _tmp30A[26]="struct_or_union_specifier";static char _tmp30B[16]="type_params_opt";
static char _tmp30C[16]="struct_or_union";static char _tmp30D[24]="struct_declaration_list";
static char _tmp30E[25]="struct_declaration_list0";static char _tmp30F[21]="init_declarator_list";
static char _tmp310[22]="init_declarator_list0";static char _tmp311[16]="init_declarator";
static char _tmp312[19]="struct_declaration";static char _tmp313[25]="specifier_qualifier_list";
static char _tmp314[35]="notypedef_specifier_qualifier_list";static char _tmp315[23]="struct_declarator_list";
static char _tmp316[24]="struct_declarator_list0";static char _tmp317[18]="struct_declarator";
static char _tmp318[17]="tunion_specifier";static char _tmp319[9]="flat_opt";static
char _tmp31A[18]="tunion_or_xtunion";static char _tmp31B[17]="tunionfield_list";
static char _tmp31C[18]="tunionfield_scope";static char _tmp31D[12]="tunionfield";
static char _tmp31E[11]="declarator";static char _tmp31F[23]="declarator_withtypedef";
static char _tmp320[18]="direct_declarator";static char _tmp321[30]="direct_declarator_withtypedef";
static char _tmp322[8]="pointer";static char _tmp323[12]="one_pointer";static char
_tmp324[23]="pointer_null_and_bound";static char _tmp325[14]="pointer_bound";
static char _tmp326[18]="zeroterm_qual_opt";static char _tmp327[12]="opt_rgn_opt";
static char _tmp328[8]="rgn_opt";static char _tmp329[11]="tqual_list";static char
_tmp32A[20]="parameter_type_list";static char _tmp32B[9]="type_var";static char
_tmp32C[16]="optional_effect";static char _tmp32D[19]="optional_rgn_order";static
char _tmp32E[10]="rgn_order";static char _tmp32F[16]="optional_inject";static char
_tmp330[11]="effect_set";static char _tmp331[14]="atomic_effect";static char _tmp332[
11]="region_set";static char _tmp333[15]="parameter_list";static char _tmp334[22]="parameter_declaration";
static char _tmp335[16]="identifier_list";static char _tmp336[17]="identifier_list0";
static char _tmp337[12]="initializer";static char _tmp338[18]="array_initializer";
static char _tmp339[17]="initializer_list";static char _tmp33A[12]="designation";
static char _tmp33B[16]="designator_list";static char _tmp33C[11]="designator";
static char _tmp33D[10]="type_name";static char _tmp33E[14]="any_type_name";static
char _tmp33F[15]="type_name_list";static char _tmp340[20]="abstract_declarator";
static char _tmp341[27]="direct_abstract_declarator";static char _tmp342[10]="statement";
static char _tmp343[13]="open_exp_opt";static char _tmp344[18]="labeled_statement";
static char _tmp345[21]="expression_statement";static char _tmp346[19]="compound_statement";
static char _tmp347[16]="block_item_list";static char _tmp348[20]="selection_statement";
static char _tmp349[15]="switch_clauses";static char _tmp34A[20]="iteration_statement";
static char _tmp34B[15]="jump_statement";static char _tmp34C[12]="exp_pattern";
static char _tmp34D[20]="conditional_pattern";static char _tmp34E[19]="logical_or_pattern";
static char _tmp34F[20]="logical_and_pattern";static char _tmp350[21]="inclusive_or_pattern";
static char _tmp351[21]="exclusive_or_pattern";static char _tmp352[12]="and_pattern";
static char _tmp353[17]="equality_pattern";static char _tmp354[19]="relational_pattern";
static char _tmp355[14]="shift_pattern";static char _tmp356[17]="additive_pattern";
static char _tmp357[23]="multiplicative_pattern";static char _tmp358[13]="cast_pattern";
static char _tmp359[14]="unary_pattern";static char _tmp35A[16]="postfix_pattern";
static char _tmp35B[16]="primary_pattern";static char _tmp35C[8]="pattern";static
char _tmp35D[19]="tuple_pattern_list";static char _tmp35E[20]="tuple_pattern_list0";
static char _tmp35F[14]="field_pattern";static char _tmp360[19]="field_pattern_list";
static char _tmp361[20]="field_pattern_list0";static char _tmp362[11]="expression";
static char _tmp363[22]="assignment_expression";static char _tmp364[20]="assignment_operator";
static char _tmp365[23]="conditional_expression";static char _tmp366[20]="constant_expression";
static char _tmp367[22]="logical_or_expression";static char _tmp368[23]="logical_and_expression";
static char _tmp369[24]="inclusive_or_expression";static char _tmp36A[24]="exclusive_or_expression";
static char _tmp36B[15]="and_expression";static char _tmp36C[20]="equality_expression";
static char _tmp36D[22]="relational_expression";static char _tmp36E[17]="shift_expression";
static char _tmp36F[20]="additive_expression";static char _tmp370[26]="multiplicative_expression";
static char _tmp371[16]="cast_expression";static char _tmp372[17]="unary_expression";
static char _tmp373[15]="unary_operator";static char _tmp374[19]="postfix_expression";
static char _tmp375[19]="primary_expression";static char _tmp376[25]="argument_expression_list";
static char _tmp377[26]="argument_expression_list0";static char _tmp378[9]="constant";
static char _tmp379[20]="qual_opt_identifier";static char _tmp37A[17]="qual_opt_typedef";
static char _tmp37B[18]="struct_union_name";static char _tmp37C[11]="field_name";
static char _tmp37D[12]="right_angle";static struct _dynforward_ptr Cyc_yytname[275]={{
_tmp26B,_tmp26B + 2},{_tmp26C,_tmp26C + 6},{_tmp26D,_tmp26D + 12},{_tmp26E,_tmp26E + 
5},{_tmp26F,_tmp26F + 9},{_tmp270,_tmp270 + 7},{_tmp271,_tmp271 + 7},{_tmp272,
_tmp272 + 8},{_tmp273,_tmp273 + 5},{_tmp274,_tmp274 + 5},{_tmp275,_tmp275 + 6},{
_tmp276,_tmp276 + 4},{_tmp277,_tmp277 + 5},{_tmp278,_tmp278 + 6},{_tmp279,_tmp279 + 
7},{_tmp27A,_tmp27A + 7},{_tmp27B,_tmp27B + 9},{_tmp27C,_tmp27C + 6},{_tmp27D,
_tmp27D + 9},{_tmp27E,_tmp27E + 9},{_tmp27F,_tmp27F + 7},{_tmp280,_tmp280 + 6},{
_tmp281,_tmp281 + 5},{_tmp282,_tmp282 + 8},{_tmp283,_tmp283 + 7},{_tmp284,_tmp284 + 
7},{_tmp285,_tmp285 + 9},{_tmp286,_tmp286 + 3},{_tmp287,_tmp287 + 5},{_tmp288,
_tmp288 + 7},{_tmp289,_tmp289 + 6},{_tmp28A,_tmp28A + 3},{_tmp28B,_tmp28B + 4},{
_tmp28C,_tmp28C + 5},{_tmp28D,_tmp28D + 9},{_tmp28E,_tmp28E + 6},{_tmp28F,_tmp28F + 
7},{_tmp290,_tmp290 + 5},{_tmp291,_tmp291 + 8},{_tmp292,_tmp292 + 4},{_tmp293,
_tmp293 + 6},{_tmp294,_tmp294 + 4},{_tmp295,_tmp295 + 6},{_tmp296,_tmp296 + 7},{
_tmp297,_tmp297 + 4},{_tmp298,_tmp298 + 9},{_tmp299,_tmp299 + 9},{_tmp29A,_tmp29A + 
6},{_tmp29B,_tmp29B + 10},{_tmp29C,_tmp29C + 7},{_tmp29D,_tmp29D + 8},{_tmp29E,
_tmp29E + 7},{_tmp29F,_tmp29F + 8},{_tmp2A0,_tmp2A0 + 7},{_tmp2A1,_tmp2A1 + 8},{
_tmp2A2,_tmp2A2 + 5},{_tmp2A3,_tmp2A3 + 9},{_tmp2A4,_tmp2A4 + 9},{_tmp2A5,_tmp2A5 + 
6},{_tmp2A6,_tmp2A6 + 7},{_tmp2A7,_tmp2A7 + 5},{_tmp2A8,_tmp2A8 + 8},{_tmp2A9,
_tmp2A9 + 13},{_tmp2AA,_tmp2AA + 4},{_tmp2AB,_tmp2AB + 14},{_tmp2AC,_tmp2AC + 12},{
_tmp2AD,_tmp2AD + 7},{_tmp2AE,_tmp2AE + 8},{_tmp2AF,_tmp2AF + 8},{_tmp2B0,_tmp2B0 + 
12},{_tmp2B1,_tmp2B1 + 6},{_tmp2B2,_tmp2B2 + 7},{_tmp2B3,_tmp2B3 + 7},{_tmp2B4,
_tmp2B4 + 7},{_tmp2B5,_tmp2B5 + 8},{_tmp2B6,_tmp2B6 + 9},{_tmp2B7,_tmp2B7 + 6},{
_tmp2B8,_tmp2B8 + 6},{_tmp2B9,_tmp2B9 + 6},{_tmp2BA,_tmp2BA + 6},{_tmp2BB,_tmp2BB + 
7},{_tmp2BC,_tmp2BC + 6},{_tmp2BD,_tmp2BD + 11},{_tmp2BE,_tmp2BE + 11},{_tmp2BF,
_tmp2BF + 11},{_tmp2C0,_tmp2C0 + 11},{_tmp2C1,_tmp2C1 + 11},{_tmp2C2,_tmp2C2 + 12},{
_tmp2C3,_tmp2C3 + 13},{_tmp2C4,_tmp2C4 + 11},{_tmp2C5,_tmp2C5 + 11},{_tmp2C6,
_tmp2C6 + 10},{_tmp2C7,_tmp2C7 + 9},{_tmp2C8,_tmp2C8 + 11},{_tmp2C9,_tmp2C9 + 12},{
_tmp2CA,_tmp2CA + 11},{_tmp2CB,_tmp2CB + 17},{_tmp2CC,_tmp2CC + 7},{_tmp2CD,_tmp2CD
+ 19},{_tmp2CE,_tmp2CE + 18},{_tmp2CF,_tmp2CF + 9},{_tmp2D0,_tmp2D0 + 13},{_tmp2D1,
_tmp2D1 + 16},{_tmp2D2,_tmp2D2 + 18},{_tmp2D3,_tmp2D3 + 13},{_tmp2D4,_tmp2D4 + 10},{
_tmp2D5,_tmp2D5 + 4},{_tmp2D6,_tmp2D6 + 4},{_tmp2D7,_tmp2D7 + 4},{_tmp2D8,_tmp2D8 + 
4},{_tmp2D9,_tmp2D9 + 4},{_tmp2DA,_tmp2DA + 4},{_tmp2DB,_tmp2DB + 4},{_tmp2DC,
_tmp2DC + 4},{_tmp2DD,_tmp2DD + 4},{_tmp2DE,_tmp2DE + 4},{_tmp2DF,_tmp2DF + 4},{
_tmp2E0,_tmp2E0 + 4},{_tmp2E1,_tmp2E1 + 4},{_tmp2E2,_tmp2E2 + 4},{_tmp2E3,_tmp2E3 + 
4},{_tmp2E4,_tmp2E4 + 4},{_tmp2E5,_tmp2E5 + 4},{_tmp2E6,_tmp2E6 + 4},{_tmp2E7,
_tmp2E7 + 4},{_tmp2E8,_tmp2E8 + 4},{_tmp2E9,_tmp2E9 + 4},{_tmp2EA,_tmp2EA + 4},{
_tmp2EB,_tmp2EB + 4},{_tmp2EC,_tmp2EC + 4},{_tmp2ED,_tmp2ED + 4},{_tmp2EE,_tmp2EE + 
4},{_tmp2EF,_tmp2EF + 4},{_tmp2F0,_tmp2F0 + 5},{_tmp2F1,_tmp2F1 + 17},{_tmp2F2,
_tmp2F2 + 12},{_tmp2F3,_tmp2F3 + 19},{_tmp2F4,_tmp2F4 + 21},{_tmp2F5,_tmp2F5 + 20},{
_tmp2F6,_tmp2F6 + 21},{_tmp2F7,_tmp2F7 + 13},{_tmp2F8,_tmp2F8 + 15},{_tmp2F9,
_tmp2F9 + 17},{_tmp2FA,_tmp2FA + 19},{_tmp2FB,_tmp2FB + 12},{_tmp2FC,_tmp2FC + 17},{
_tmp2FD,_tmp2FD + 23},{_tmp2FE,_tmp2FE + 24},{_tmp2FF,_tmp2FF + 15},{_tmp300,
_tmp300 + 11},{_tmp301,_tmp301 + 15},{_tmp302,_tmp302 + 10},{_tmp303,_tmp303 + 15},{
_tmp304,_tmp304 + 25},{_tmp305,_tmp305 + 5},{_tmp306,_tmp306 + 15},{_tmp307,_tmp307
+ 15},{_tmp308,_tmp308 + 11},{_tmp309,_tmp309 + 22},{_tmp30A,_tmp30A + 26},{_tmp30B,
_tmp30B + 16},{_tmp30C,_tmp30C + 16},{_tmp30D,_tmp30D + 24},{_tmp30E,_tmp30E + 25},{
_tmp30F,_tmp30F + 21},{_tmp310,_tmp310 + 22},{_tmp311,_tmp311 + 16},{_tmp312,
_tmp312 + 19},{_tmp313,_tmp313 + 25},{_tmp314,_tmp314 + 35},{_tmp315,_tmp315 + 23},{
_tmp316,_tmp316 + 24},{_tmp317,_tmp317 + 18},{_tmp318,_tmp318 + 17},{_tmp319,
_tmp319 + 9},{_tmp31A,_tmp31A + 18},{_tmp31B,_tmp31B + 17},{_tmp31C,_tmp31C + 18},{
_tmp31D,_tmp31D + 12},{_tmp31E,_tmp31E + 11},{_tmp31F,_tmp31F + 23},{_tmp320,
_tmp320 + 18},{_tmp321,_tmp321 + 30},{_tmp322,_tmp322 + 8},{_tmp323,_tmp323 + 12},{
_tmp324,_tmp324 + 23},{_tmp325,_tmp325 + 14},{_tmp326,_tmp326 + 18},{_tmp327,
_tmp327 + 12},{_tmp328,_tmp328 + 8},{_tmp329,_tmp329 + 11},{_tmp32A,_tmp32A + 20},{
_tmp32B,_tmp32B + 9},{_tmp32C,_tmp32C + 16},{_tmp32D,_tmp32D + 19},{_tmp32E,_tmp32E
+ 10},{_tmp32F,_tmp32F + 16},{_tmp330,_tmp330 + 11},{_tmp331,_tmp331 + 14},{_tmp332,
_tmp332 + 11},{_tmp333,_tmp333 + 15},{_tmp334,_tmp334 + 22},{_tmp335,_tmp335 + 16},{
_tmp336,_tmp336 + 17},{_tmp337,_tmp337 + 12},{_tmp338,_tmp338 + 18},{_tmp339,
_tmp339 + 17},{_tmp33A,_tmp33A + 12},{_tmp33B,_tmp33B + 16},{_tmp33C,_tmp33C + 11},{
_tmp33D,_tmp33D + 10},{_tmp33E,_tmp33E + 14},{_tmp33F,_tmp33F + 15},{_tmp340,
_tmp340 + 20},{_tmp341,_tmp341 + 27},{_tmp342,_tmp342 + 10},{_tmp343,_tmp343 + 13},{
_tmp344,_tmp344 + 18},{_tmp345,_tmp345 + 21},{_tmp346,_tmp346 + 19},{_tmp347,
_tmp347 + 16},{_tmp348,_tmp348 + 20},{_tmp349,_tmp349 + 15},{_tmp34A,_tmp34A + 20},{
_tmp34B,_tmp34B + 15},{_tmp34C,_tmp34C + 12},{_tmp34D,_tmp34D + 20},{_tmp34E,
_tmp34E + 19},{_tmp34F,_tmp34F + 20},{_tmp350,_tmp350 + 21},{_tmp351,_tmp351 + 21},{
_tmp352,_tmp352 + 12},{_tmp353,_tmp353 + 17},{_tmp354,_tmp354 + 19},{_tmp355,
_tmp355 + 14},{_tmp356,_tmp356 + 17},{_tmp357,_tmp357 + 23},{_tmp358,_tmp358 + 13},{
_tmp359,_tmp359 + 14},{_tmp35A,_tmp35A + 16},{_tmp35B,_tmp35B + 16},{_tmp35C,
_tmp35C + 8},{_tmp35D,_tmp35D + 19},{_tmp35E,_tmp35E + 20},{_tmp35F,_tmp35F + 14},{
_tmp360,_tmp360 + 19},{_tmp361,_tmp361 + 20},{_tmp362,_tmp362 + 11},{_tmp363,
_tmp363 + 22},{_tmp364,_tmp364 + 20},{_tmp365,_tmp365 + 23},{_tmp366,_tmp366 + 20},{
_tmp367,_tmp367 + 22},{_tmp368,_tmp368 + 23},{_tmp369,_tmp369 + 24},{_tmp36A,
_tmp36A + 24},{_tmp36B,_tmp36B + 15},{_tmp36C,_tmp36C + 20},{_tmp36D,_tmp36D + 22},{
_tmp36E,_tmp36E + 17},{_tmp36F,_tmp36F + 20},{_tmp370,_tmp370 + 26},{_tmp371,
_tmp371 + 16},{_tmp372,_tmp372 + 17},{_tmp373,_tmp373 + 15},{_tmp374,_tmp374 + 19},{
_tmp375,_tmp375 + 19},{_tmp376,_tmp376 + 25},{_tmp377,_tmp377 + 26},{_tmp378,
_tmp378 + 9},{_tmp379,_tmp379 + 20},{_tmp37A,_tmp37A + 17},{_tmp37B,_tmp37B + 18},{
_tmp37C,_tmp37C + 11},{_tmp37D,_tmp37D + 12}};static short Cyc_yyr1[501]={0,133,134,
134,134,134,134,134,134,134,134,134,135,135,136,136,137,137,137,138,138,138,138,
139,139,140,141,142,143,144,144,144,144,145,145,146,146,146,146,146,146,146,146,
146,146,147,147,147,147,147,147,147,148,148,149,150,150,151,151,151,151,151,151,
152,152,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,
153,153,153,153,153,154,155,155,155,156,156,156,157,157,158,158,158,159,159,159,
160,160,161,161,162,162,163,163,164,165,165,166,166,167,168,168,168,168,168,168,
169,169,169,169,169,169,170,171,171,172,172,172,173,173,173,174,174,175,175,176,
176,176,176,177,177,177,178,178,179,179,180,180,181,181,181,181,181,181,181,181,
181,181,182,182,182,182,182,182,182,182,182,182,182,183,183,184,185,185,185,185,
186,186,186,187,187,187,188,188,188,189,189,189,190,190,191,191,191,191,192,192,
193,193,194,194,195,195,196,196,197,197,198,198,198,198,199,199,200,200,201,201,
201,202,203,203,204,204,205,205,205,205,206,206,206,206,207,208,208,209,209,210,
210,211,211,211,211,211,212,212,213,213,213,214,214,214,214,214,214,214,214,214,
214,214,215,215,215,215,215,215,215,215,215,215,215,215,216,216,217,218,218,219,
219,220,220,220,220,220,220,221,221,221,221,221,221,222,222,222,222,222,222,223,
223,223,223,223,223,223,223,223,223,223,223,223,223,224,224,224,224,224,224,224,
224,225,226,226,227,227,228,228,229,229,230,230,231,231,232,232,232,233,233,233,
233,233,234,234,234,235,235,235,236,236,236,236,237,237,238,238,238,238,238,238,
238,239,240,241,241,241,241,241,241,241,241,241,241,241,241,241,241,242,242,242,
243,243,244,244,245,245,245,246,246,247,247,248,248,249,249,249,249,249,249,249,
249,249,249,249,250,250,250,250,250,250,250,251,252,252,253,253,254,254,255,255,
256,256,257,257,257,258,258,258,258,258,259,259,259,260,260,260,261,261,261,261,
262,262,263,263,263,263,263,263,263,263,263,263,263,263,263,263,263,263,263,264,
264,264,265,265,265,265,265,265,265,265,265,265,266,266,266,266,266,266,266,266,
266,267,268,268,269,269,269,269,270,270,271,271,272,272,273,273,274,274};static
short Cyc_yyr2[501]={0,1,2,3,5,3,5,6,7,3,3,0,4,3,2,3,1,1,1,2,3,3,4,3,4,2,1,2,1,2,
3,5,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,
1,1,1,1,1,1,1,1,1,1,4,4,4,6,4,4,1,1,1,1,1,1,5,2,4,1,3,1,2,3,4,8,3,0,3,1,1,0,1,1,
2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,1,2,3,8,5,7,1,0,1,1,1,2,3,3,0,1,1,2,
5,1,2,1,2,1,3,4,4,5,4,5,4,4,2,1,1,3,4,4,5,4,5,4,4,2,1,2,5,2,2,1,2,0,3,3,0,1,1,0,
1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,
3,4,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,
1,6,4,9,6,5,9,0,5,3,1,2,2,3,1,2,1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,
8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,
3,1,3,3,3,1,4,1,2,2,4,2,6,6,1,1,1,3,1,1,3,4,4,5,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,
1,3,1,3,1,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,
3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,6,5,4,6,9,11,6,1,1,1,1,4,3,4,3,
3,2,2,6,7,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1};static short Cyc_yydefact[
1007]={0,18,45,46,47,48,50,65,68,69,70,71,72,73,74,75,89,90,91,105,106,41,0,0,51,
0,0,0,0,86,0,0,138,0,491,201,493,492,494,87,0,0,66,0,183,183,181,1,0,16,0,0,17,0,
35,43,37,63,39,76,77,0,78,0,139,151,0,176,186,79,155,103,49,48,42,0,93,490,0,491,
487,488,489,0,366,0,0,0,0,224,0,368,369,25,27,139,139,139,0,0,139,0,0,0,0,0,139,
139,179,180,182,2,0,0,0,0,29,0,111,112,114,36,44,38,40,139,495,496,103,140,141,
189,139,33,139,0,19,139,139,0,164,152,177,188,187,192,139,64,0,49,97,0,95,0,491,
375,0,0,0,0,0,0,0,0,0,0,0,0,103,0,0,477,139,0,0,464,0,0,462,463,0,392,394,407,
415,417,419,421,423,425,428,433,436,439,443,0,445,465,476,475,0,376,374,32,0,0,
103,0,0,0,121,117,119,242,244,0,0,0,9,10,0,497,498,202,88,0,0,156,67,222,0,219,0,
0,0,3,0,5,0,30,0,0,139,20,0,139,109,0,102,191,0,190,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
491,280,282,288,284,0,286,265,266,267,0,268,269,270,0,34,21,114,209,225,0,0,205,
203,0,249,0,186,0,194,52,193,0,0,98,94,0,0,370,0,0,139,453,139,409,443,0,410,411,
0,0,0,0,0,0,0,139,446,447,139,0,0,0,449,450,448,0,367,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,397,398,399,400,401,402,403,404,405,406,396,0,451,0,471,472,0,0,0,
479,0,103,382,383,0,380,0,226,0,0,0,139,245,217,0,122,127,123,125,118,120,139,0,
251,243,252,500,499,0,81,84,85,139,82,58,57,0,55,157,139,221,153,251,223,165,166,
139,80,139,185,184,26,0,28,0,113,115,228,227,22,100,110,0,0,0,129,130,132,0,103,
103,0,0,0,0,0,0,139,0,317,318,319,0,0,321,0,277,0,0,0,0,0,289,285,114,287,283,
281,210,139,0,0,216,204,211,160,0,0,139,205,162,139,163,158,186,195,53,104,0,99,
414,96,92,378,379,0,0,0,229,0,0,233,0,0,0,238,0,0,0,0,0,0,139,0,0,0,478,485,0,
484,393,416,0,418,420,422,424,426,427,431,432,429,430,434,435,438,437,440,441,
442,395,470,467,0,469,0,139,0,371,0,377,31,389,0,385,390,0,387,372,0,0,246,128,
124,126,0,205,0,186,0,253,139,139,0,264,248,0,0,0,0,121,0,103,139,139,0,175,154,
220,4,6,0,133,116,0,0,205,0,136,0,0,0,296,0,0,0,0,0,316,320,0,0,0,0,0,0,0,0,0,
279,139,23,203,139,213,0,0,206,0,161,209,197,250,159,195,178,0,7,0,452,0,0,241,0,
230,0,234,237,239,457,0,0,0,0,0,0,0,0,0,444,481,0,0,468,466,0,0,381,384,386,373,
0,247,218,260,0,254,255,186,0,205,0,186,0,83,0,0,0,54,56,0,167,0,205,0,0,186,0,
131,134,139,103,146,0,0,0,0,0,0,0,0,0,0,0,0,0,296,322,0,0,272,0,0,0,0,24,205,0,
214,212,0,205,139,196,0,8,0,0,0,240,231,235,0,0,0,0,0,412,413,456,483,0,486,408,
480,482,0,388,391,259,257,262,0,263,256,186,0,61,59,60,168,171,0,173,174,169,186,
0,137,148,147,0,0,142,290,296,0,0,0,139,0,0,324,325,327,329,331,333,335,337,340,
345,348,351,355,357,364,365,0,139,293,302,0,0,0,0,0,0,0,0,0,0,323,0,0,0,0,275,0,
199,215,207,198,203,13,0,0,455,454,0,236,458,139,0,461,473,0,261,258,0,172,170,
101,135,149,146,146,0,0,296,139,361,139,0,358,139,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,359,297,0,304,0,0,0,312,0,0,0,0,0,0,295,0,271,274,0,0,0,205,12,14,0,0,
0,0,474,0,139,145,144,291,292,0,0,0,0,296,298,328,0,330,332,334,336,338,339,343,
344,341,342,346,347,350,349,352,353,354,0,303,305,306,0,314,313,0,308,0,0,0,278,
0,0,208,200,15,0,0,139,0,0,294,360,0,356,299,0,139,307,315,309,310,0,0,0,232,459,
0,62,150,0,0,326,296,300,311,273,276,0,363,362,301,460,0,0,0};static short Cyc_yydefgoto[
142]={1004,47,641,838,48,49,269,50,425,51,427,52,134,53,54,488,210,409,410,211,
57,223,212,59,150,151,60,147,61,244,245,117,118,119,246,213,392,437,438,439,62,
63,131,784,785,786,64,440,65,414,66,67,68,108,145,250,296,638,568,69,569,480,630,
472,476,477,387,289,231,88,89,504,430,505,506,507,508,214,291,292,570,398,272,
617,273,274,275,276,277,711,278,279,794,795,796,797,798,799,800,801,802,803,804,
805,806,807,808,809,376,377,378,558,559,560,280,181,364,182,494,183,184,185,186,
187,188,189,190,191,192,193,194,195,196,197,521,522,198,199,71,839,224,402};
static short Cyc_yypact[1007]={2652,- -32768,- -32768,- -32768,- -32768,- 53,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,3219,102,2878,- -32768,158,- 28,50,83,93,181,200,-
-32768,248,- -32768,164,- -32768,- -32768,- -32768,- -32768,240,496,319,288,317,317,
306,- -32768,2529,- -32768,128,402,- -32768,144,3219,3219,3219,- -32768,3219,- -32768,
- -32768,559,- -32768,475,3289,357,143,589,516,- -32768,- -32768,339,349,363,- -32768,
158,374,- -32768,2992,264,- -32768,- -32768,- -32768,5526,- -32768,382,392,2992,389,
406,407,- -32768,222,- -32768,- -32768,6033,6033,6033,2529,2529,6033,5,418,472,421,
5,6200,2023,- -32768,- -32768,- -32768,- -32768,2529,2774,2529,2774,- -32768,434,452,
- -32768,3148,- -32768,- -32768,- -32768,- -32768,6063,- -32768,- -32768,339,- -32768,-
-32768,168,1638,- -32768,3289,144,- -32768,3351,6033,3894,- -32768,357,- -32768,-
-32768,- -32768,186,6033,- -32768,2774,- -32768,457,492,503,158,105,- -32768,2992,
207,5746,511,5526,5580,517,522,524,526,533,538,339,5800,5800,- -32768,2148,554,
5854,- -32768,5854,5854,- -32768,- -32768,62,- -32768,- -32768,- 50,617,565,576,573,
530,- 14,611,252,194,- -32768,763,5854,238,- 4,- -32768,598,821,620,- -32768,- -32768,
634,5526,339,821,624,261,6048,6307,6048,326,- -32768,53,53,53,- -32768,- -32768,97,
- -32768,- -32768,- -32768,- -32768,42,625,- -32768,- -32768,509,463,- -32768,646,87,
637,- -32768,651,- -32768,655,- -32768,472,3990,3289,- -32768,662,6170,- -32768,605,
673,- -32768,158,- -32768,661,183,679,3505,683,712,703,709,4086,3505,119,- 49,732,
706,740,- -32768,- -32768,1508,1508,144,1508,- -32768,- -32768,- -32768,754,- -32768,-
-32768,- -32768,566,- -32768,- -32768,756,769,- -32768,126,760,778,568,792,781,- 39,
516,793,- -32768,813,- -32768,- 39,816,158,- -32768,5526,822,- -32768,807,809,2148,-
-32768,6200,- -32768,- -32768,3606,- -32768,855,5526,5526,5526,5526,5526,5526,829,
2148,- -32768,- -32768,1768,835,567,5526,- -32768,- -32768,- -32768,5526,- -32768,5854,
5526,5854,5854,5854,5854,5854,5854,5854,5854,5854,5854,5854,5854,5854,5854,5854,
5854,5854,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,5526,- -32768,5,- -32768,- -32768,4182,5,5526,- -32768,833,
339,- -32768,- -32768,838,842,2992,- -32768,574,1136,841,6033,- -32768,867,848,-
-32768,6307,6307,6307,- -32768,- -32768,2913,4278,145,- -32768,445,- -32768,- -32768,
126,- -32768,- -32768,- -32768,6033,- -32768,- -32768,866,868,872,- -32768,2843,-
-32768,541,236,- -32768,- -32768,- -32768,6200,- -32768,2273,- -32768,- -32768,- -32768,
2529,- -32768,2529,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,653,
5526,876,874,- -32768,869,283,339,160,5526,5526,873,879,5526,957,1898,882,- -32768,
- -32768,- -32768,593,947,- -32768,4374,881,893,899,5526,896,3505,- -32768,- -32768,
3148,- -32768,- -32768,- -32768,- -32768,6200,888,297,- -32768,- -32768,875,- -32768,
126,889,6185,778,- -32768,6033,- -32768,- -32768,516,86,- -32768,- -32768,2406,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,890,897,910,- -32768,5,5526,-
-32768,615,3990,366,- -32768,610,898,900,902,911,619,6200,907,915,5663,- -32768,-
-32768,912,916,- -32768,617,256,565,576,573,530,- 14,- 14,611,611,611,611,252,252,
194,194,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,922,- -32768,24,6033,3798,
- -32768,1097,- -32768,- -32768,- -32768,2992,- -32768,- -32768,919,926,- -32768,196,
908,- -32768,- -32768,- -32768,- -32768,924,778,925,516,909,445,3413,6033,4470,-
-32768,- -32768,53,785,927,42,3028,931,339,3351,6033,4566,- -32768,541,- -32768,-
-32768,- -32768,665,- -32768,- -32768,605,5526,778,158,937,629,630,5526,863,670,941,
4662,4758,682,- -32768,- -32768,946,942,943,959,3505,932,938,689,936,- -32768,3289,
- -32768,955,6033,- -32768,954,126,- -32768,939,- -32768,167,- -32768,781,- -32768,86,
- -32768,958,- -32768,2529,960,315,949,- -32768,950,- -32768,978,- -32768,- -32768,-
-32768,- -32768,5526,1041,5526,5526,5580,956,960,966,3798,- -32768,- -32768,5526,
5526,- -32768,- -32768,- 39,644,- -32768,- -32768,- -32768,- -32768,1330,- -32768,-
-32768,- -32768,967,- -32768,- -32768,516,969,778,- 39,516,951,- -32768,694,970,971,-
-32768,- -32768,975,- -32768,976,778,981,- 39,516,952,- -32768,- -32768,6063,339,887,
3505,965,982,1020,983,989,3505,5526,4854,699,4950,707,5046,863,- -32768,998,996,-
-32768,1013,112,1005,1017,- -32768,778,220,- -32768,- -32768,1023,778,6200,- -32768,
537,- -32768,1012,1015,5526,- -32768,- -32768,- -32768,3990,708,1018,1019,710,-
-32768,855,- -32768,- -32768,788,- -32768,- -32768,- -32768,- -32768,3798,- -32768,-
-32768,- -32768,- -32768,- -32768,1024,- -32768,- -32768,516,1034,- -32768,- -32768,-
-32768,- -32768,- -32768,1025,- -32768,- -32768,- -32768,516,1030,- -32768,- -32768,-
-32768,1031,158,717,1114,863,1037,5908,1036,2273,5854,1033,- -32768,- 44,- -32768,
1070,1027,782,836,99,847,318,360,- -32768,- -32768,- -32768,- -32768,1074,5854,1768,
- -32768,- -32768,716,3505,721,5142,3505,723,5238,5334,730,1047,- -32768,5526,3505,
3505,1056,- -32768,1048,- -32768,- -32768,1050,- -32768,955,- -32768,1049,731,- -32768,
- -32768,279,- -32768,- -32768,6200,1135,- -32768,- -32768,3702,- -32768,- -32768,1052,
- -32768,- -32768,- -32768,- -32768,1053,55,887,3505,1055,863,2148,- -32768,6200,1054,
- -32768,1292,5854,5526,5854,5854,5854,5854,5854,5854,5854,5854,5854,5854,5854,
5854,5854,5854,5854,5854,5854,5526,- -32768,- -32768,1059,- -32768,3505,3505,729,-
-32768,3505,3505,746,3505,751,5430,- -32768,765,- -32768,- -32768,1043,5526,126,778,
- -32768,- -32768,494,5526,1058,1057,- -32768,1075,6200,- -32768,- -32768,- -32768,-
-32768,1064,1061,1066,5854,863,- -32768,617,323,565,576,576,530,- 14,- 14,611,611,
611,611,252,252,194,194,- -32768,- -32768,- -32768,376,- -32768,- -32768,- -32768,3505,
- -32768,- -32768,3505,- -32768,3505,3505,766,- -32768,1083,101,- -32768,- -32768,-
-32768,818,1069,6200,1073,767,- -32768,960,562,- -32768,- -32768,5526,1292,- -32768,
- -32768,- -32768,- -32768,3505,3505,3505,- -32768,- -32768,1076,- -32768,- -32768,1078,
1079,- -32768,863,- -32768,- -32768,- -32768,- -32768,1082,- -32768,- -32768,- -32768,-
-32768,1182,1197,- -32768};static short Cyc_yypgoto[142]={- -32768,9,- -32768,285,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 41,- 117,19,- -32768,-
-32768,0,618,- -32768,71,- 173,1098,32,- -32768,- -32768,- 108,- -32768,- 12,- -32768,
501,- -32768,- -32768,- -32768,972,961,107,500,- -32768,- -32768,608,- -32768,- -32768,
- -32768,70,- -32768,- -32768,239,- 145,1143,- 339,301,- -32768,- -32768,1170,- 17,-
-32768,- -32768,582,- 123,- 98,- 122,- 81,311,590,595,- 382,- 454,- 105,- 257,- 113,-
-32768,- 206,- 151,- 511,- 246,- -32768,715,- 167,152,- 142,69,- 247,286,- -32768,- -32768,
- -32768,- 52,- 244,- -32768,- 462,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,-
-32768,- -32768,95,1022,- -32768,552,- -32768,- -32768,299,626,- -32768,- 143,- 409,-
150,- 332,- 327,- 303,895,- 323,- 333,- 293,- 317,- 306,- 16,623,518,- -32768,- -32768,- 363,
- -32768,115,305,- 48,1175,- 348,20};static short Cyc_yytable[6422]={55,230,524,242,
298,326,546,530,531,526,313,314,136,127,287,288,529,310,544,578,628,55,547,133,
290,465,466,595,468,536,537,334,58,251,527,429,399,869,390,669,74,103,538,539,72,
303,459,297,55,532,533,534,535,58,55,55,55,111,55,407,782,783,342,343,55,140,460,
94,243,461,484,56,335,121,122,123,590,124,870,133,58,517,282,135,413,400,58,58,
58,372,58,270,56,281,646,615,58,631,55,55,221,344,590,16,17,18,222,218,219,676,
345,386,55,55,55,55,248,373,90,56,55,235,236,237,238,56,56,56,399,56,58,58,55,
332,55,56,556,408,91,135,498,140,499,667,58,58,58,58,55,573,754,271,58,135,645,
516,321,299,329,493,330,331,591,- 143,58,95,58,482,573,56,56,332,399,155,333,877,
878,400,401,365,58,418,202,56,56,56,56,473,475,702,432,56,492,91,382,422,985,34,
96,418,156,281,91,56,37,56,405,828,97,75,332,389,401,229,879,625,390,390,390,56,
157,400,401,880,591,457,35,829,270,270,458,270,247,474,112,113,403,404,34,34,406,
708,55,391,229,37,37,215,216,217,116,304,220,34,41,41,394,824,101,233,37,135,471,
395,44,45,46,584,35,55,55,91,55,286,58,146,486,600,34,- 203,104,249,397,- 203,270,
37,35,98,271,271,584,271,120,493,445,740,489,446,416,295,649,58,58,475,58,70,99,
305,675,366,367,368,485,56,350,91,230,388,490,393,306,401,91,351,352,55,861,76,
92,206,93,34,833,207,540,541,542,36,37,38,56,56,226,56,271,401,70,412,658,369,
623,102,247,70,395,370,371,58,70,156,493,35,550,100,585,332,126,418,142,385,- 225,
70,665,- 225,283,348,386,349,34,157,152,475,180,92,36,37,38,418,332,389,389,389,
92,418,435,914,56,35,577,106,924,634,745,70,70,627,929,668,70,609,221,739,583,
105,589,624,222,70,70,70,70,391,391,391,107,55,133,55,556,110,599,601,332,684,
592,594,593,394,693,977,70,883,744,884,395,698,44,45,46,693,55,682,683,70,146,
493,148,58,152,58,149,92,40,695,696,386,976,55,137,135,636,327,138,697,553,139,
650,557,836,283,885,153,58,502,503,332,135,201,678,886,887,55,978,200,91,203,56,
91,56,58,640,229,662,729,381,92,750,751,114,115,467,734,92,745,396,204,995,205,
703,229,637,56,756,58,129,130,229,631,229,225,415,475,1002,227,417,585,562,930,
56,843,239,449,936,937,932,70,418,456,441,418,40,935,417,744,680,443,574,579,447,
455,575,240,56,576,942,943,300,34,890,933,934,728,419,577,37,420,70,944,945,229,
143,144,281,41,938,939,940,941,229,34,589,34,44,45,46,36,37,38,37,687,301,40,765,
745,34,152,327,41,340,341,36,37,38,302,509,775,44,45,46,514,412,327,309,55,928,
866,992,395,315,44,45,46,34,316,525,317,635,318,36,37,38,55,135,744,319,837,40,
671,832,320,738,672,586,835,34,58,587,221,991,588,36,37,38,222,763,328,125,91,
767,637,548,91,470,58,286,332,332,481,915,519,554,229,778,332,92,346,347,92,757,
127,388,337,781,229,56,396,925,336,926,612,34,339,332,338,766,374,36,37,38,44,45,
46,56,415,910,379,435,417,777,332,417,436,652,647,648,44,45,46,332,380,70,657,70,
234,994,384,441,411,332,332,417,706,707,602,603,423,417,606,34,610,622,851,758,
759,36,37,38,421,40,424,34,620,854,426,435,294,36,37,38,557,433,270,444,44,45,46,
435,867,730,332,442,308,712,311,311,44,45,46,718,91,448,332,323,324,450,889,70,
311,332,311,311,726,988,769,810,818,770,451,332,452,247,475,55,821,971,453,332,
332,311,332,844,463,847,858,91,332,859,270,891,965,332,271,332,893,993,897,902,
912,332,332,913,953,229,462,58,353,354,355,356,357,358,359,360,361,362,332,464,
92,956,77,332,92,469,958,471,127,241,431,55,946,947,948,478,363,332,332,419,961,
983,990,688,689,690,56,441,709,710,271,417,565,566,567,782,783,479,441,848,849,
441,417,58,78,417,723,483,704,401,715,717,873,874,975,487,375,875,876,154,80,40,
81,82,881,882,37,491,311,986,332,920,921,495,496,83,497,84,85,334,270,431,56,515,
86,510,511,512,513,70,518,549,87,551,552,746,561,520,749,564,311,523,311,311,311,
311,311,311,311,311,311,311,311,311,311,311,311,311,311,563,580,55,92,581,582,
596,597,604,598,605,607,611,613,543,616,787,618,619,520,621,271,814,626,629,632,
642,158,159,644,643,653,35,654,58,655,815,817,92,820,77,823,160,659,656,572,161,
660,663,664,229,673,681,162,163,164,165,166,666,674,677,679,167,691,842,168,126,
694,705,790,791,520,720,56,169,170,713,719,722,721,724,725,77,311,727,286,731,
733,741,737,747,661,752,742,768,779,788,34,80,171,81,82,753,762,37,764,771,772,
520,312,743,773,774,172,857,180,173,776,789,502,503,813,174,812,78,175,892,176,
825,896,177,826,827,178,179,830,831,905,906,154,80,895,81,82,899,901,37,834,840,
904,311,841,846,845,852,792,431,84,85,77,850,853,855,856,86,311,860,175,862,793,
922,865,87,868,871,178,179,872,888,903,907,911,908,909,916,918,327,923,919,950,
927,962,969,931,968,970,972,973,77,974,431,78,984,951,952,987,1005,954,955,989,
957,949,999,670,1000,1001,154,80,1003,81,82,1006,966,37,692,960,686,228,780,701,
434,963,83,141,84,85,428,967,700,109,78,86,126,736,964,311,651,735,732,87,761,
811,555,383,520,154,80,528,81,82,128,0,37,979,0,0,980,0,981,982,0,83,0,84,85,0,0,
502,503,0,86,0,0,0,0,0,0,0,87,0,0,0,0,996,997,998,0,0,431,0,0,0,0,0,311,748,0,0,
0,0,0,431,311,0,755,0,0,0,0,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
709,710,21,158,159,252,0,253,254,255,256,257,258,259,260,22,77,23,160,261,0,0,
161,24,262,0,0,0,0,162,163,164,165,166,27,28,29,263,167,0,264,168,0,0,0,0,32,33,
265,0,169,170,0,0,77,0,0,431,0,0,0,0,0,0,0,0,0,0,0,0,0,431,0,266,80,171,81,82,35,
36,37,38,39,40,267,132,- 296,0,0,172,0,42,232,0,0,0,78,0,174,864,0,175,311,176,0,
0,177,0,760,178,179,154,80,0,81,82,0,0,37,0,311,0,0,0,0,0,0,83,0,84,85,0,0,502,
503,0,86,0,0,0,0,0,0,0,87,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,431,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,311,0,311,311,311,311,311,311,311,311,311,311,311,311,311,311,311,
311,311,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,158,159,252,0,
253,254,255,256,257,258,259,260,22,77,23,160,261,311,0,161,24,262,0,0,- 139,- 139,
162,163,164,165,166,27,28,29,263,167,0,264,168,0,0,0,0,32,33,265,0,169,170,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,311,0,0,266,80,171,81,82,35,36,37,38,39,40,267,132,
0,0,0,172,0,42,232,0,0,0,0,0,174,0,0,175,0,176,0,0,177,0,0,178,179,2,3,4,73,6,7,
8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,158,159,252,0,253,254,255,256,257,
258,259,260,22,77,23,160,261,0,0,161,24,262,0,0,0,0,162,163,164,165,166,27,28,29,
263,167,0,264,168,0,0,0,0,32,33,265,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,266,80,171,81,82,35,36,37,38,39,40,267,132,268,0,0,172,0,42,232,0,0,0,0,
0,174,0,0,175,0,176,0,0,177,0,0,178,179,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,21,158,159,252,0,253,254,255,256,257,258,259,260,22,77,23,160,261,0,
0,161,24,262,0,0,0,0,162,163,164,165,166,27,28,29,263,167,0,264,168,0,0,0,0,32,
33,265,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,266,80,171,81,82,35,
36,37,38,39,40,267,132,0,0,0,172,0,42,232,0,0,0,0,0,174,0,0,175,0,176,0,0,177,0,
0,178,179,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,158,159,0,0,0,
0,0,0,0,0,0,0,22,77,23,160,0,0,0,161,24,0,0,0,0,0,162,163,164,165,166,27,28,29,0,
167,0,0,168,0,0,0,0,32,33,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
34,80,171,81,82,35,36,37,38,39,40,608,0,0,0,0,172,0,42,232,0,0,0,0,0,174,0,0,175,
0,176,0,0,177,0,0,178,179,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,158,159,0,
0,0,0,0,0,0,0,0,0,22,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,27,28,29,
0,167,208,0,168,0,0,0,0,32,33,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,34,80,171,81,82,35,36,37,38,39,40,0,209,0,0,0,172,0,42,232,0,0,0,0,0,174,0,0,
175,0,176,0,0,177,0,0,178,179,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,158,
159,0,0,0,0,0,0,0,0,0,0,22,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,27,
28,29,0,167,0,0,168,0,0,0,0,32,33,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,34,80,171,81,82,35,36,37,38,39,40,0,325,0,0,0,172,0,42,232,0,0,0,0,0,174,
0,0,175,0,176,0,0,177,0,0,178,179,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,
158,159,0,0,0,0,0,0,0,0,0,0,22,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,
166,27,28,29,0,167,0,0,168,0,0,0,0,32,33,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,34,80,171,81,82,35,36,37,38,39,40,0,0,0,0,0,172,0,42,232,0,0,0,0,0,
174,0,0,175,0,176,0,0,177,0,0,178,179,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,639,0,24,0,25,26,-
139,- 139,0,0,0,0,0,27,28,29,0,0,0,0,0,0,0,30,31,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,34,0,0,0,0,35,36,37,38,39,40,0,0,- 11,0,0,41,0,42,43,0,0,
0,0,0,44,45,46,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,
0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,- 139,- 139,0,0,0,0,0,27,28,29,
0,0,0,0,0,0,0,30,31,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,0,
0,0,0,35,36,37,38,39,40,0,0,- 11,0,0,41,0,42,43,0,0,0,0,0,44,45,46,- 11,1,0,2,3,4,
5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,
23,0,0,0,0,0,24,0,25,26,- 139,- 139,0,0,0,0,0,27,28,29,0,0,0,0,0,0,0,30,31,32,33,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,0,0,0,0,35,36,37,38,39,40,0,0,
0,0,0,41,0,42,43,0,0,0,0,0,44,45,46,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,- 139,- 139,0,
0,0,0,0,27,28,29,0,0,0,0,0,0,0,30,31,32,33,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,0,0,34,0,0,0,0,35,36,37,38,39,40,22,0,- 11,0,0,41,0,42,43,0,0,
0,0,0,44,45,46,0,0,27,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,0,77,0,0,0,0,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,284,0,0,34,0,0,0,0,35,36,37,38,39,40,286,22,0,0,0,412,
- 203,42,43,78,- 203,0,395,0,44,45,46,0,0,0,27,28,29,0,79,80,0,81,82,0,0,37,32,33,
0,0,0,0,0,0,83,0,84,85,0,0,0,0,0,86,0,0,0,0,0,0,284,87,0,0,0,0,0,0,35,36,0,38,39,
40,286,0,0,0,0,394,- 203,42,43,0,- 203,77,395,0,44,45,46,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,78,0,0,0,0,- 139,- 139,
0,0,0,0,0,27,28,29,154,80,0,81,82,0,0,37,0,32,33,0,0,0,0,0,83,0,84,85,0,0,0,0,0,
86,0,0,0,0,0,0,0,87,0,0,34,0,0,0,0,35,36,37,38,39,40,0,0,0,0,0,435,0,42,43,0,0,0,
0,0,44,45,46,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,
0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,0,0,- 139,- 139,0,0,0,0,0,27,28,29,0,0,0,0,0,
0,0,0,0,32,33,0,0,0,0,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,
0,0,0,35,36,0,38,39,40,0,132,22,0,241,0,0,42,43,0,24,0,0,0,- 139,- 139,0,0,0,0,0,
27,28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,0,2,3,4,73,6,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,0,0,21,0,0,0,0,0,35,36,0,38,39,40,0,22,0,23,0,0,0,42,43,24,0,0,0,0,0,0,
0,0,0,0,27,28,29,0,0,0,0,0,0,0,0,0,32,33,7,8,9,10,11,12,13,14,15,16,17,18,19,20,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,35,36,0,38,39,40,0,132,0,0,0,0,0,42,43,0,0,0,27,
28,29,0,0,0,0,0,0,0,0,0,32,33,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,
0,0,284,0,0,285,0,0,0,22,35,36,0,38,39,40,286,0,0,0,0,0,- 203,42,43,0,- 203,0,27,
28,29,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,284,0,
0,0,0,0,0,0,35,36,0,38,39,40,286,0,0,0,0,0,- 203,42,43,0,- 203,158,159,252,0,253,
254,255,256,257,258,259,260,0,77,0,160,261,0,0,161,0,262,0,0,0,0,162,163,164,165,
166,0,0,0,263,167,0,264,168,0,0,0,0,0,0,265,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,266,80,171,81,82,0,0,37,0,0,0,267,132,0,0,0,172,0,0,173,0,0,0,0,
0,174,0,0,175,0,176,158,159,177,0,0,178,179,500,0,0,0,0,0,77,0,160,0,0,0,161,0,0,
0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,312,501,0,0,172,
0,0,173,0,0,502,503,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,
161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,312,917,
0,0,172,0,0,173,0,0,502,503,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,
160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,
0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,
0,312,0,0,0,172,0,0,173,0,0,502,503,0,174,158,159,175,0,176,0,0,177,0,0,178,179,
0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,
0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,
0,0,0,0,0,0,0,0,172,0,0,173,0,0,0,0,293,174,158,159,175,0,176,0,0,177,0,0,178,
179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,
0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,
0,37,0,0,0,0,312,0,0,0,172,0,0,173,0,0,0,0,0,174,158,159,175,0,176,0,0,177,0,0,
178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,
0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,
82,0,0,37,0,0,0,454,0,0,0,0,172,0,0,173,0,0,0,0,0,174,158,159,175,0,176,0,0,177,
0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,
168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,
81,82,0,0,37,0,0,0,0,0,0,0,0,172,545,0,173,0,0,0,0,0,174,158,159,175,0,176,0,0,
177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,
0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,
171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,0,0,173,0,0,0,0,571,174,158,159,175,0,176,0,
0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,
167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,
80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,614,0,173,0,0,0,0,0,174,158,159,175,0,
176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,0,
0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,0,0,173,0,0,0,0,685,174,158,159,175,
0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,166,0,
0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,0,0,173,0,0,0,0,699,174,158,159,
175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,165,
166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,714,0,0,0,0,172,0,0,173,0,0,0,0,0,174,158,
159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,164,
165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,716,0,0,0,0,172,0,0,173,0,0,0,0,0,174,
158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,163,
164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,816,0,173,0,0,0,0,0,
174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,162,
163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,819,0,173,0,0,0,
0,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,0,
162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,822,0,0,0,0,172,0,0,173,0,
0,0,0,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,0,
0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,894,0,173,
0,0,0,0,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,0,0,
0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,898,0,
173,0,0,0,0,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,0,
0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,900,
0,173,0,0,0,0,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,161,0,
0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,172,
959,0,173,0,0,0,0,0,174,158,159,175,0,176,0,0,177,0,0,178,179,0,77,0,160,0,0,0,
161,0,0,0,0,0,0,162,163,164,165,166,0,0,0,0,167,0,0,168,0,0,0,0,0,0,0,0,169,170,
0,0,0,0,0,158,159,0,0,0,0,0,0,0,0,0,0,0,77,0,0,34,80,171,81,82,0,0,37,0,0,162,
163,164,165,166,0,172,0,0,173,0,0,168,0,0,174,0,0,175,0,176,169,170,177,0,0,178,
179,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,312,158,159,0,
172,0,0,173,0,0,0,0,0,174,77,0,175,0,176,0,0,177,0,0,178,179,0,162,163,164,165,
166,0,0,0,0,0,0,0,168,0,0,0,0,0,0,0,0,169,170,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,661,158,159,0,172,0,0,173,0,0,0,0,0,174,
77,0,175,0,176,0,0,177,0,0,178,179,0,162,163,164,165,166,0,0,0,0,0,0,0,168,0,0,0,
0,0,0,0,0,169,170,0,0,0,0,0,158,159,0,0,0,0,0,0,0,0,0,0,0,77,0,0,34,80,171,81,82,
0,0,37,0,0,162,163,164,165,166,0,307,0,0,173,0,0,168,0,0,174,0,0,175,0,176,169,
170,177,0,0,178,179,158,159,0,0,0,0,0,0,0,0,0,0,0,77,0,0,34,80,171,81,82,0,0,37,
0,0,162,163,164,165,166,0,322,0,0,173,0,0,168,0,0,174,0,0,175,0,176,169,170,177,
0,0,178,179,158,159,0,0,0,0,0,0,0,0,0,0,0,77,0,0,34,80,171,81,82,0,0,37,0,0,162,
163,164,165,166,0,172,0,0,173,0,0,168,0,0,174,0,0,175,0,176,169,170,177,0,0,178,
179,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,34,80,171,81,82,0,0,37,0,0,0,0,0,0,0,0,863,0,
0,173,0,0,0,0,0,174,0,0,175,0,176,0,0,177,0,0,178,179,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,7,8,9,10,11,12,13,14,15,
16,17,18,19,20,22,0,0,0,27,28,29,0,0,208,0,0,- 139,- 139,0,22,32,33,0,27,28,29,0,0,
0,0,0,0,0,0,0,32,33,0,27,28,29,0,0,0,0,0,0,0,0,0,32,33,35,36,0,38,39,40,0,209,0,
0,0,0,0,42,43,35,36,0,38,39,40,0,0,0,0,0,0,0,42,43,35,36,0,38,39,40,0,0,- 107,0,0,
0,0,42,43,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,7,8,9,10,11,12,13,14,15,16,17,
18,19,20,22,7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,0,0,0,27,28,29,0,0,0,0,0,0,
0,0,22,32,33,0,27,28,29,0,0,0,0,0,0,0,0,0,32,33,0,27,28,29,0,0,0,0,0,0,0,0,0,32,
33,35,36,0,38,39,40,0,633,- 108,0,0,0,0,42,43,35,36,0,38,39,40,0,0,0,0,0,0,0,42,
43,35,36,0,38,39,40,0,0,0,0,0,0,0,42,43,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,0,0,0,0,0,0,0,0,0,- 139,- 139,0,0,0,0,0,27,28,
29,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,35,0,0,0,39,40,0,0,0,0,0,0,0,42,43};static short Cyc_yycheck[6422]={0,106,
334,120,146,172,369,340,341,336,161,161,64,61,137,137,339,160,366,401,474,21,370,
64,137,269,270,436,272,346,347,81,0,131,337,241,75,81,211,550,21,41,348,349,97,
153,95,145,48,342,343,344,345,21,54,55,56,48,58,17,5,6,76,77,64,65,115,95,120,
118,109,0,122,54,55,56,415,58,122,120,48,325,134,64,229,124,54,55,56,93,58,132,
21,134,503,458,64,479,98,99,95,115,441,17,18,19,101,98,99,563,124,209,112,113,
114,115,128,121,23,48,120,112,113,114,115,54,55,56,75,58,98,99,132,109,134,64,
382,95,23,120,307,141,309,119,112,113,114,115,148,396,661,132,120,134,502,322,
168,148,174,302,176,177,419,108,132,115,134,289,415,98,99,109,75,78,112,76,77,
124,125,195,148,229,87,112,113,114,115,61,286,598,242,120,300,78,206,108,95,95,
115,247,95,242,87,132,102,134,109,95,115,107,109,211,125,106,115,472,389,390,391,
148,115,124,125,124,481,106,100,115,269,270,111,272,125,107,106,107,216,217,95,
95,220,604,242,211,137,102,102,95,96,97,106,156,100,95,111,111,111,719,94,107,
102,242,95,118,120,121,122,412,100,269,270,156,272,106,242,115,293,117,95,112,41,
113,213,116,325,102,100,106,269,270,435,272,53,436,111,643,296,114,229,113,506,
269,270,401,272,0,106,100,112,71,72,73,292,242,120,200,421,210,298,212,113,125,
207,129,130,325,788,22,23,107,25,95,112,111,350,351,352,101,102,103,269,270,103,
272,325,125,41,111,515,111,467,111,245,48,118,117,118,325,53,95,503,100,374,115,
412,109,61,415,67,108,106,66,116,109,135,123,474,125,95,115,75,479,83,78,101,102,
103,435,109,389,390,391,87,441,111,116,325,100,398,111,862,482,648,98,99,108,868,
549,103,450,95,96,412,94,414,467,101,112,113,114,115,389,390,391,107,425,467,427,
674,123,442,443,109,575,425,435,427,111,583,116,135,123,648,125,118,587,120,121,
122,594,450,574,574,148,115,598,107,425,153,427,97,156,105,586,586,563,928,467,
111,450,487,172,115,586,379,118,110,382,735,240,120,107,450,117,118,109,467,95,
569,129,130,491,116,111,379,106,425,382,427,467,491,394,518,625,205,200,657,657,
106,107,271,633,207,759,213,109,978,110,599,412,488,450,665,491,49,50,419,909,
421,111,229,629,994,112,229,583,384,869,467,745,106,255,875,876,871,240,594,261,
247,597,105,874,247,759,571,250,111,405,253,260,115,109,491,118,881,882,109,95,
812,872,873,623,109,573,102,112,271,883,884,472,64,65,623,111,877,878,879,880,
481,95,590,95,120,121,122,101,102,103,102,579,108,105,683,849,95,300,307,111,78,
79,101,102,103,110,315,696,120,121,122,320,111,322,111,623,868,792,974,118,111,
120,121,122,95,111,335,111,484,111,101,102,103,641,623,849,111,108,105,552,729,
111,641,556,111,734,95,623,115,95,96,118,101,102,103,101,681,111,107,552,685,637,
371,556,106,641,106,109,109,109,845,112,106,574,699,109,379,74,75,382,668,737,
583,126,704,586,623,394,863,80,865,106,95,128,109,127,684,107,101,102,103,120,
121,122,641,412,836,95,111,412,698,109,415,116,112,108,109,120,121,122,109,95,
425,112,427,107,978,111,435,112,109,109,435,112,112,444,445,108,441,448,95,450,
464,768,108,109,101,102,103,111,105,108,95,462,779,108,111,139,101,102,103,674,
108,812,111,120,121,122,111,793,626,109,107,158,112,160,161,120,121,122,106,674,
111,109,169,170,111,811,491,174,109,176,177,112,969,109,709,106,112,95,109,106,
703,909,812,106,919,106,109,109,195,109,112,115,112,106,709,109,109,868,112,910,
109,812,109,112,977,112,106,106,109,109,109,112,735,111,812,82,83,84,85,86,87,88,
89,90,91,109,116,552,112,38,109,556,108,112,95,913,110,241,868,885,886,887,112,
110,109,109,109,112,112,112,95,96,97,812,583,22,23,868,583,389,390,391,5,6,116,
594,108,109,597,594,868,80,597,617,112,600,125,608,609,127,128,927,119,92,78,79,
95,96,105,98,99,74,75,102,108,302,108,109,858,859,108,124,111,124,113,114,81,978,
312,868,111,120,316,317,318,319,641,112,115,128,112,109,653,112,328,656,108,334,
332,336,337,338,339,340,341,342,343,344,345,346,347,348,349,350,351,352,109,111,
978,674,112,109,106,109,111,116,107,30,106,42,364,110,706,100,95,369,100,978,712,
111,125,112,112,25,26,95,109,109,100,109,978,109,713,714,709,716,38,718,40,112,
109,395,44,108,112,109,919,108,119,51,52,53,54,55,112,109,112,112,60,112,741,63,
737,112,107,25,26,421,106,978,72,73,111,107,95,112,124,119,38,436,124,106,108,
124,115,107,25,107,112,119,119,119,107,95,96,97,98,99,112,112,102,112,112,112,
458,107,108,112,112,111,785,792,114,112,112,117,118,108,120,116,80,123,816,125,
106,819,128,111,95,131,132,106,95,827,828,95,96,818,98,99,821,822,102,100,112,
826,503,112,109,111,96,111,506,113,114,38,112,112,108,108,120,518,28,123,107,125,
860,111,128,116,80,131,132,126,80,108,100,108,110,109,25,109,863,108,111,106,112,
124,111,870,112,96,108,112,38,109,550,80,95,893,894,112,0,897,898,112,900,888,
112,92,112,112,95,96,112,98,99,0,913,102,582,902,576,105,703,597,245,908,111,66,
113,114,240,914,588,45,80,120,913,637,909,598,507,633,629,128,674,709,92,207,604,
95,96,338,98,99,61,- 1,102,953,- 1,- 1,956,- 1,958,959,- 1,111,- 1,113,114,- 1,- 1,117,
118,- 1,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,- 1,- 1,- 1,983,984,985,- 1,- 1,648,- 1,- 1,- 1,-
1,- 1,657,655,- 1,- 1,- 1,- 1,- 1,661,665,- 1,664,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,
14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,
41,- 1,- 1,44,45,46,- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,58,59,60,- 1,62,63,- 1,- 1,- 1,- 1,
68,69,70,- 1,72,73,- 1,- 1,38,- 1,- 1,745,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,759,
- 1,95,96,97,98,99,100,101,102,103,104,105,106,107,108,- 1,- 1,111,- 1,113,114,- 1,- 1,
- 1,80,- 1,120,790,- 1,123,793,125,- 1,- 1,128,- 1,92,131,132,95,96,- 1,98,99,- 1,- 1,102,
- 1,811,- 1,- 1,- 1,- 1,- 1,- 1,111,- 1,113,114,- 1,- 1,117,118,- 1,120,- 1,- 1,- 1,- 1,- 1,- 1,-
1,128,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,849,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,869,- 1,871,872,873,874,875,876,877,878,879,880,881,882,
883,884,885,886,887,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,
25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,927,- 1,44,45,46,- 1,- 1,49,50,
51,52,53,54,55,56,57,58,59,60,- 1,62,63,- 1,- 1,- 1,- 1,68,69,70,- 1,72,73,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,977,- 1,- 1,95,96,97,98,99,100,101,102,
103,104,105,106,107,- 1,- 1,- 1,111,- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,123,- 1,125,
- 1,- 1,128,- 1,- 1,131,132,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,- 1,- 1,
51,52,53,54,55,56,57,58,59,60,- 1,62,63,- 1,- 1,- 1,- 1,68,69,70,- 1,72,73,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,100,101,102,
103,104,105,106,107,108,- 1,- 1,111,- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,123,- 1,125,
- 1,- 1,128,- 1,- 1,131,132,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,- 1,- 1,
51,52,53,54,55,56,57,58,59,60,- 1,62,63,- 1,- 1,- 1,- 1,68,69,70,- 1,72,73,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,100,101,102,
103,104,105,106,107,- 1,- 1,- 1,111,- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,123,- 1,125,
- 1,- 1,128,- 1,- 1,131,132,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,
24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,- 1,- 1,
51,52,53,54,55,56,57,58,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,73,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,100,101,102,
103,104,105,106,- 1,- 1,- 1,- 1,111,- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,123,- 1,125,-
1,- 1,128,- 1,- 1,131,132,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,
55,56,57,58,- 1,60,61,- 1,63,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,100,101,102,103,104,105,- 1,
107,- 1,- 1,- 1,111,- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,123,- 1,125,- 1,- 1,128,- 1,- 1,
131,132,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,58,- 1,
60,- 1,- 1,63,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,100,101,102,103,104,105,- 1,107,- 1,- 1,- 1,
111,- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,56,57,58,- 1,60,- 1,- 1,63,
- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,95,96,97,98,99,100,101,102,103,104,105,- 1,- 1,- 1,- 1,- 1,111,- 1,113,114,
- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,0,1,- 1,3,4,5,6,7,8,9,
10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
37,- 1,39,- 1,- 1,- 1,43,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,66,67,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,95,- 1,- 1,- 1,- 1,100,101,102,103,104,105,- 1,- 1,108,- 1,- 1,111,- 1,113,
114,- 1,- 1,- 1,- 1,- 1,120,121,122,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,
19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,
- 1,47,48,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,- 1,- 1,- 1,- 1,
100,101,102,103,104,105,- 1,- 1,108,- 1,- 1,111,- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,121,
122,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,
- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,- 1,- 1,- 1,- 1,100,101,102,103,104,105,- 1,
- 1,- 1,- 1,- 1,111,- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,121,122,1,- 1,3,4,5,6,7,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,
39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
66,67,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,-
1,- 1,95,- 1,- 1,- 1,- 1,100,101,102,103,104,105,37,- 1,108,- 1,- 1,111,- 1,113,114,- 1,- 1,
- 1,- 1,- 1,120,121,122,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,38,
- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,92,- 1,- 1,95,- 1,- 1,- 1,- 1,100,
101,102,103,104,105,106,37,- 1,- 1,- 1,111,112,113,114,80,116,- 1,118,- 1,120,121,122,
- 1,- 1,- 1,56,57,58,- 1,95,96,- 1,98,99,- 1,- 1,102,68,69,- 1,- 1,- 1,- 1,- 1,- 1,111,- 1,113,
114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,- 1,92,128,- 1,- 1,- 1,- 1,- 1,- 1,100,101,- 1,103,
104,105,106,- 1,- 1,- 1,- 1,111,112,113,114,- 1,116,38,118,- 1,120,121,122,8,9,10,11,
12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,
- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,95,96,- 1,98,99,- 1,- 1,
102,- 1,68,69,- 1,- 1,- 1,- 1,- 1,111,- 1,113,114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,128,- 1,- 1,95,- 1,- 1,- 1,- 1,100,101,102,103,104,105,- 1,- 1,- 1,- 1,- 1,111,- 1,113,
114,- 1,- 1,- 1,- 1,- 1,120,121,122,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,
- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,- 1,- 1,
49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,3,4,5,
6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,100,101,- 1,103,
104,105,- 1,107,37,- 1,110,- 1,- 1,113,114,- 1,45,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,- 1,56,57,
58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,
17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,100,101,- 1,103,104,105,- 1,37,- 1,39,- 1,- 1,
- 1,113,114,45,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
68,69,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,37,100,101,- 1,103,104,105,- 1,107,- 1,- 1,- 1,- 1,- 1,113,114,- 1,- 1,- 1,56,
57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,8,9,10,11,12,13,14,15,16,17,18,19,20,21,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,- 1,- 1,95,- 1,- 1,- 1,37,100,101,- 1,103,104,105,106,- 1,- 1,
- 1,- 1,- 1,112,113,114,- 1,116,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,100,101,- 1,103,104,105,106,- 1,- 1,- 1,- 1,- 1,112,113,114,- 1,116,25,26,27,- 1,29,
30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,- 1,44,- 1,46,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,
- 1,- 1,59,60,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,70,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,106,107,- 1,- 1,
- 1,111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,123,- 1,125,25,26,128,- 1,- 1,131,132,32,
- 1,- 1,- 1,- 1,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,
60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,107,108,- 1,- 1,111,-
1,- 1,114,- 1,- 1,117,118,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,
40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,107,108,- 1,- 1,111,- 1,- 1,114,- 1,- 1,117,
118,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,-
1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,
73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,
- 1,- 1,102,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,111,- 1,- 1,114,- 1,- 1,117,118,- 1,120,25,26,123,
- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,
53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,111,- 1,- 1,114,- 1,- 1,- 1,- 1,119,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,
131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,
- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,111,- 1,- 1,
114,- 1,- 1,- 1,- 1,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,-
1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,
96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,120,
25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,
- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,114,- 1,- 1,- 1,- 1,- 1,120,25,26,123,- 1,125,- 1,- 1,
128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,
- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
111,- 1,- 1,114,- 1,- 1,- 1,- 1,119,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,
- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,114,- 1,- 1,- 1,-
1,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,
- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,
- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,- 1,- 1,114,- 1,- 1,- 1,- 1,119,120,25,26,123,- 1,
125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,
54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,111,- 1,- 1,114,- 1,- 1,- 1,- 1,119,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,
131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,
- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,111,- 1,- 1,
114,- 1,- 1,- 1,- 1,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,-
1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,
96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,120,
25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,
- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,114,- 1,- 1,- 1,- 1,- 1,120,25,26,123,- 1,125,- 1,- 1,
128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,
- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
111,112,- 1,114,- 1,- 1,- 1,- 1,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,
- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,111,- 1,- 1,114,- 1,- 1,- 1,-
1,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,
- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,
- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,114,- 1,- 1,- 1,- 1,- 1,120,25,26,123,- 1,
125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,
54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,-
1,- 1,- 1,- 1,111,112,- 1,114,- 1,- 1,- 1,- 1,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,
131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,
- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,
114,- 1,- 1,- 1,- 1,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,-
1,- 1,44,- 1,- 1,- 1,- 1,- 1,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,
96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,114,- 1,- 1,- 1,- 1,- 1,120,
25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,- 1,
- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,
- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,95,96,97,98,99,- 1,- 1,102,-
1,- 1,51,52,53,54,55,- 1,111,- 1,- 1,114,- 1,- 1,63,- 1,- 1,120,- 1,- 1,123,- 1,125,72,73,
128,- 1,- 1,131,132,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,
- 1,- 1,102,- 1,- 1,- 1,- 1,107,25,26,- 1,111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,120,38,- 1,123,- 1,
125,- 1,- 1,128,- 1,- 1,131,132,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,- 1,- 1,107,25,26,- 1,111,- 1,- 1,114,- 1,- 1,- 1,- 1,-
1,120,38,- 1,123,- 1,125,- 1,- 1,128,- 1,- 1,131,132,- 1,51,52,53,54,55,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,72,73,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,38,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,51,52,53,54,55,- 1,111,- 1,- 1,
114,- 1,- 1,63,- 1,- 1,120,- 1,- 1,123,- 1,125,72,73,128,- 1,- 1,131,132,25,26,- 1,- 1,- 1,-
1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,51,52,53,54,55,- 1,
111,- 1,- 1,114,- 1,- 1,63,- 1,- 1,120,- 1,- 1,123,- 1,125,72,73,128,- 1,- 1,131,132,25,26,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,- 1,95,96,97,98,99,- 1,- 1,102,- 1,- 1,51,52,
53,54,55,- 1,111,- 1,- 1,114,- 1,- 1,63,- 1,- 1,120,- 1,- 1,123,- 1,125,72,73,128,- 1,- 1,
131,132,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,97,98,99,- 1,- 1,102,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,123,- 1,125,- 1,- 1,
128,- 1,- 1,131,132,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,8,9,10,11,12,13,14,
15,16,17,18,19,20,21,37,8,9,10,11,12,13,14,15,16,17,18,19,20,21,37,- 1,- 1,- 1,56,
57,58,- 1,- 1,61,- 1,- 1,49,50,- 1,37,68,69,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,
69,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,100,101,- 1,103,104,105,- 1,107,- 1,
- 1,- 1,- 1,- 1,113,114,100,101,- 1,103,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,100,101,
- 1,103,104,105,- 1,- 1,108,- 1,- 1,- 1,- 1,113,114,8,9,10,11,12,13,14,15,16,17,18,19,
20,21,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,37,8,9,10,11,12,13,14,15,16,17,
18,19,20,21,37,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,68,69,- 1,56,57,58,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,100,
101,- 1,103,104,105,- 1,92,108,- 1,- 1,- 1,- 1,113,114,100,101,- 1,103,104,105,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,113,114,100,101,- 1,103,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,8,9,10,
11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,- 1,- 1,56,57,58,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,
- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,- 1,- 1,- 1,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114};char
Cyc_Yyimpossible[17]="\000\000\000\000Yyimpossible\000";char Cyc_Yystack_overflow[
21]="\000\000\000\000Yystack_overflow\000";void Cyc_yyerror(struct _dynforward_ptr);
int Cyc_yylex(struct Cyc_Lexing_lexbuf*);static int Cyc_yychar=(int)'\000';union Cyc_YYSTYPE_union
Cyc_yylval=(union Cyc_YYSTYPE_union)((struct Cyc_YYINITIALSVAL_struct){57});static
int Cyc_yynerrs=0;struct _tuple18{struct Cyc_Position_Segment*f1;struct _tuple1*f2;
int f3;};struct _tuple19{struct _dynforward_ptr f1;void*f2;};static char _tmp41F[8]="stdcall";
static char _tmp420[6]="cdecl";static char _tmp421[9]="fastcall";static char _tmp422[9]="noreturn";
static char _tmp423[6]="const";static char _tmp424[8]="aligned";static char _tmp425[7]="packed";
static char _tmp426[7]="shared";static char _tmp427[7]="unused";static char _tmp428[5]="weak";
static char _tmp429[10]="dllimport";static char _tmp42A[10]="dllexport";static char
_tmp42B[23]="no_instrument_function";static char _tmp42C[12]="constructor";static
char _tmp42D[11]="destructor";static char _tmp42E[22]="no_check_memory_usage";
static char _tmp42F[5]="pure";struct _tuple20{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*
f2;};int Cyc_yyparse(struct Cyc_Lexing_lexbuf*yylex_buf){struct _RegionHandle
_tmp37F=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp37F;
_push_region(yyregion);{int yystate;int yyn=0;int yyerrstatus;int yychar1=0;int
yyssp_offset;short*yyss=(short*)({unsigned int _tmp8CD=10000;short*_tmp8CE=(short*)
_region_malloc(yyregion,_check_times(sizeof(short),_tmp8CD));{unsigned int
_tmp8CF=_tmp8CD;unsigned int i;for(i=0;i < _tmp8CF;i ++){_tmp8CE[i]=(short)0;}}
_tmp8CE;});int yyvsp_offset;union Cyc_YYSTYPE_union*yyvs=(union Cyc_YYSTYPE_union*)({
unsigned int _tmp8CA=10000;union Cyc_YYSTYPE_union*_tmp8CB=(union Cyc_YYSTYPE_union*)
_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE_union),_tmp8CA));{
unsigned int _tmp8CC=_tmp8CA;unsigned int i;for(i=0;i < _tmp8CC;i ++){_tmp8CB[i]=Cyc_yylval;}}
_tmp8CB;});int yylsp_offset;struct Cyc_Yyltype*yyls=(struct Cyc_Yyltype*)({
unsigned int _tmp8C7=10000;struct Cyc_Yyltype*_tmp8C8=(struct Cyc_Yyltype*)
_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp8C7));{
unsigned int _tmp8C9=_tmp8C7;unsigned int i;for(i=0;i < _tmp8C9;i ++){_tmp8C8[i]=Cyc_yynewloc();}}
_tmp8C8;});int yystacksize=10000;union Cyc_YYSTYPE_union yyval=Cyc_yylval;int yylen;
yystate=0;yyerrstatus=0;Cyc_yynerrs=0;Cyc_yychar=- 2;yyssp_offset=- 1;yyvsp_offset=
0;yylsp_offset=0;yynewstate: yyss[_check_known_subscript_notnull(10000,++
yyssp_offset)]=(short)yystate;if(yyssp_offset >= yystacksize - 1){if(yystacksize >= 
10000){Cyc_yyerror(({const char*_tmp380="parser stack overflow";_tag_dynforward(
_tmp380,sizeof(char),_get_zero_arr_size_char(_tmp380,22));}));(int)_throw((void*)
Cyc_Yystack_overflow);}}goto yybackup;yybackup: yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(1007,yystate)];if(yyn == - 32768)goto yydefault;if(
Cyc_yychar == - 2)Cyc_yychar=Cyc_yylex(yylex_buf);if(Cyc_yychar <= 0){yychar1=0;Cyc_yychar=
0;}else{yychar1=Cyc_yychar > 0  && Cyc_yychar <= 360?(int)Cyc_yytranslate[
_check_known_subscript_notnull(361,Cyc_yychar)]: 275;}yyn +=yychar1;if((yyn < 0  || 
yyn > 6421) || Cyc_yycheck[_check_known_subscript_notnull(6422,yyn)]!= yychar1)
goto yydefault;yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6422,yyn)];if(
yyn < 0){if(yyn == - 32768)goto yyerrlab;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto
yyerrlab;}if(yyn == 1006){int _tmp381=0;_npop_handler(0);return _tmp381;}if(Cyc_yychar
!= 0)Cyc_yychar=- 2;yyvs[_check_known_subscript_notnull(10000,++ yyvsp_offset)]=
Cyc_yylval;yyls[_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;
if(yyerrstatus != 0)yyerrstatus --;yystate=yyn;goto yynewstate;yydefault: yyn=(int)
Cyc_yydefact[_check_known_subscript_notnull(1007,yystate)];if(yyn == 0)goto
yyerrlab;yyreduce: yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(501,yyn)];
if(yylen > 0)yyval=yyvs[_check_known_subscript_notnull(10000,(yyvsp_offset + 1)- 
yylen)];switch(yyn){case 1: _LL1E8: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];Cyc_Parse_parse_result=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);break;case 2: _LL1E9: {struct
Cyc_List_List*x;struct Cyc_List_List*y;{struct _handler_cons _tmp382;_push_handler(&
_tmp382);{int _tmp384=0;if(setjmp(_tmp382.handler))_tmp384=1;if(!_tmp384){x=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);;_pop_handler();}
else{void*_tmp383=(void*)_exn_thrown;void*_tmp386=_tmp383;_LL1EC: if(*((void**)
_tmp386)!= Cyc_Core_Failure)goto _LL1EE;_LL1ED: x=0;goto _LL1EB;_LL1EE:;_LL1EF:(
void)_throw(_tmp386);_LL1EB:;}}}{struct _handler_cons _tmp387;_push_handler(&
_tmp387);{int _tmp389=0;if(setjmp(_tmp387.handler))_tmp389=1;if(!_tmp389){y=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);;_pop_handler();}else{
void*_tmp388=(void*)_exn_thrown;void*_tmp38B=_tmp388;_LL1F1: if(*((void**)_tmp38B)
!= Cyc_Core_Failure)goto _LL1F3;_LL1F2: y=0;goto _LL1F0;_LL1F3:;_LL1F4:(void)_throw(
_tmp38B);_LL1F0:;}}}yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp38C;(_tmp38C.YY18).tag=22;(_tmp38C.YY18).f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y);_tmp38C;});
break;}case 3: _LL1EA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp38D;(_tmp38D.YY18).tag=22;(_tmp38D.YY18).f1=({struct Cyc_List_List*_tmp38E=
_cycalloc(sizeof(*_tmp38E));_tmp38E->hd=({struct Cyc_Absyn_Decl*_tmp38F=_cycalloc(
sizeof(*_tmp38F));_tmp38F->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*
_tmp390=_cycalloc(sizeof(*_tmp390));_tmp390[0]=({struct Cyc_Absyn_Using_d_struct
_tmp391;_tmp391.tag=9;_tmp391.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp391.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp391;});_tmp390;}));
_tmp38F->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp38F;});_tmp38E->tl=0;_tmp38E;});_tmp38D;});Cyc_Lex_leave_using();
break;case 4: _LL1F5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp392;(
_tmp392.YY18).tag=22;(_tmp392.YY18).f1=({struct Cyc_List_List*_tmp393=_cycalloc(
sizeof(*_tmp393));_tmp393->hd=({struct Cyc_Absyn_Decl*_tmp394=_cycalloc(sizeof(*
_tmp394));_tmp394->r=(void*)((void*)({struct Cyc_Absyn_Using_d_struct*_tmp395=
_cycalloc(sizeof(*_tmp395));_tmp395[0]=({struct Cyc_Absyn_Using_d_struct _tmp396;
_tmp396.tag=9;_tmp396.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp396.f2=Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp396;});_tmp395;}));
_tmp394->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);_tmp394;});_tmp393->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp393;});_tmp392;});
break;case 5: _LL1F6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp397;(
_tmp397.YY18).tag=22;(_tmp397.YY18).f1=({struct Cyc_List_List*_tmp398=_cycalloc(
sizeof(*_tmp398));_tmp398->hd=({struct Cyc_Absyn_Decl*_tmp399=_cycalloc(sizeof(*
_tmp399));_tmp399->r=(void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp39A=
_cycalloc(sizeof(*_tmp39A));_tmp39A[0]=({struct Cyc_Absyn_Namespace_d_struct
_tmp39B;_tmp39B.tag=8;_tmp39B.f1=({struct _dynforward_ptr*_tmp39C=_cycalloc(
sizeof(*_tmp39C));_tmp39C[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp39C;});_tmp39B.f2=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp39B;});
_tmp39A;}));_tmp399->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp399;});
_tmp398->tl=0;_tmp398;});_tmp397;});Cyc_Lex_leave_namespace();break;case 6: _LL1F7: {
struct _dynforward_ptr nspace;struct Cyc_List_List*x;struct Cyc_List_List*y;{struct
_handler_cons _tmp39D;_push_handler(& _tmp39D);{int _tmp39F=0;if(setjmp(_tmp39D.handler))
_tmp39F=1;if(!_tmp39F){nspace=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);;_pop_handler();}else{
void*_tmp39E=(void*)_exn_thrown;void*_tmp3A1=_tmp39E;_LL1FA: if(*((void**)_tmp3A1)
!= Cyc_Core_Failure)goto _LL1FC;_LL1FB: nspace=({const char*_tmp3A2="";
_tag_dynforward(_tmp3A2,sizeof(char),_get_zero_arr_size_char(_tmp3A2,1));});goto
_LL1F9;_LL1FC:;_LL1FD:(void)_throw(_tmp3A1);_LL1F9:;}}}{struct _handler_cons
_tmp3A3;_push_handler(& _tmp3A3);{int _tmp3A5=0;if(setjmp(_tmp3A3.handler))_tmp3A5=
1;if(!_tmp3A5){x=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);;_pop_handler();}else{void*_tmp3A4=(void*)_exn_thrown;void*
_tmp3A7=_tmp3A4;_LL1FF: if(*((void**)_tmp3A7)!= Cyc_Core_Failure)goto _LL201;
_LL200: x=0;goto _LL1FE;_LL201:;_LL202:(void)_throw(_tmp3A7);_LL1FE:;}}}{struct
_handler_cons _tmp3A8;_push_handler(& _tmp3A8);{int _tmp3AA=0;if(setjmp(_tmp3A8.handler))
_tmp3AA=1;if(!_tmp3AA){y=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);;_pop_handler();}else{void*_tmp3A9=(void*)_exn_thrown;void*
_tmp3AC=_tmp3A9;_LL204: if(*((void**)_tmp3AC)!= Cyc_Core_Failure)goto _LL206;
_LL205: y=0;goto _LL203;_LL206:;_LL207:(void)_throw(_tmp3AC);_LL203:;}}}yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3AD;(_tmp3AD.YY18).tag=22;(
_tmp3AD.YY18).f1=({struct Cyc_List_List*_tmp3AE=_cycalloc(sizeof(*_tmp3AE));
_tmp3AE->hd=({struct Cyc_Absyn_Decl*_tmp3AF=_cycalloc(sizeof(*_tmp3AF));_tmp3AF->r=(
void*)((void*)({struct Cyc_Absyn_Namespace_d_struct*_tmp3B0=_cycalloc(sizeof(*
_tmp3B0));_tmp3B0[0]=({struct Cyc_Absyn_Namespace_d_struct _tmp3B1;_tmp3B1.tag=8;
_tmp3B1.f1=({struct _dynforward_ptr*_tmp3B2=_cycalloc(sizeof(*_tmp3B2));_tmp3B2[0]=
nspace;_tmp3B2;});_tmp3B1.f2=x;_tmp3B1;});_tmp3B0;}));_tmp3AF->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3AF;});
_tmp3AE->tl=y;_tmp3AE;});_tmp3AD;});break;}case 7: _LL1F8: if(Cyc_strcmp((struct
_dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]),({const char*_tmp3B3="C";_tag_dynforward(_tmp3B3,sizeof(char),
_get_zero_arr_size_char(_tmp3B3,2));}))== 0)yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3B4;(_tmp3B4.YY18).tag=22;(_tmp3B4.YY18).f1=({struct
Cyc_List_List*_tmp3B5=_cycalloc(sizeof(*_tmp3B5));_tmp3B5->hd=({struct Cyc_Absyn_Decl*
_tmp3B6=_cycalloc(sizeof(*_tmp3B6));_tmp3B6->r=(void*)((void*)({struct Cyc_Absyn_ExternC_d_struct*
_tmp3B7=_cycalloc(sizeof(*_tmp3B7));_tmp3B7[0]=({struct Cyc_Absyn_ExternC_d_struct
_tmp3B8;_tmp3B8.tag=10;_tmp3B8.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3B8;});_tmp3B7;}));
_tmp3B6->loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp3B6;});_tmp3B5->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3B5;});_tmp3B4;});else{
if(Cyc_strcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),({const char*_tmp3B9="C include";
_tag_dynforward(_tmp3B9,sizeof(char),_get_zero_arr_size_char(_tmp3B9,10));}))!= 
0)Cyc_Parse_err(({const char*_tmp3BA="expecting \"C\" or \"C include\"";
_tag_dynforward(_tmp3BA,sizeof(char),_get_zero_arr_size_char(_tmp3BA,29));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3BB;(_tmp3BB.YY18).tag=22;(_tmp3BB.YY18).f1=({
struct Cyc_List_List*_tmp3BC=_cycalloc(sizeof(*_tmp3BC));_tmp3BC->hd=({struct Cyc_Absyn_Decl*
_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD->r=(void*)((void*)({struct Cyc_Absyn_ExternCinclude_d_struct*
_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE[0]=({struct Cyc_Absyn_ExternCinclude_d_struct
_tmp3BF;_tmp3BF.tag=11;_tmp3BF.f1=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3BF.f2=0;_tmp3BF;});
_tmp3BE;}));_tmp3BD->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3BD;});
_tmp3BC->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3BC;});_tmp3BB;});}break;case 8: _LL208: if(Cyc_strcmp((struct
_dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 5)]),({const char*_tmp3C0="C include";_tag_dynforward(_tmp3C0,
sizeof(char),_get_zero_arr_size_char(_tmp3C0,10));}))!= 0)Cyc_Parse_err(({const
char*_tmp3C1="expecting \"C include\"";_tag_dynforward(_tmp3C1,sizeof(char),
_get_zero_arr_size_char(_tmp3C1,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));{struct Cyc_List_List*
exs=Cyc_yyget_YY52(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3C2;(_tmp3C2.YY18).tag=
22;(_tmp3C2.YY18).f1=({struct Cyc_List_List*_tmp3C3=_cycalloc(sizeof(*_tmp3C3));
_tmp3C3->hd=({struct Cyc_Absyn_Decl*_tmp3C4=_cycalloc(sizeof(*_tmp3C4));_tmp3C4->r=(
void*)((void*)({struct Cyc_Absyn_ExternCinclude_d_struct*_tmp3C5=_cycalloc(
sizeof(*_tmp3C5));_tmp3C5[0]=({struct Cyc_Absyn_ExternCinclude_d_struct _tmp3C6;
_tmp3C6.tag=11;_tmp3C6.f1=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp3C6.f2=exs;_tmp3C6;});_tmp3C5;}));_tmp3C4->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3C4;});
_tmp3C3->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3C3;});_tmp3C2;});break;}case 9: _LL209: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3C7;(_tmp3C7.YY18).tag=22;(_tmp3C7.YY18).f1=({struct
Cyc_List_List*_tmp3C8=_cycalloc(sizeof(*_tmp3C8));_tmp3C8->hd=({struct Cyc_Absyn_Decl*
_tmp3C9=_cycalloc(sizeof(*_tmp3C9));_tmp3C9->r=(void*)((void*)0);_tmp3C9->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp3C9;});_tmp3C8->tl=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3C8;});_tmp3C7;});
break;case 10: _LL20A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3CA;(_tmp3CA.YY18).tag=22;(_tmp3CA.YY18).f1=({struct Cyc_List_List*_tmp3CB=
_cycalloc(sizeof(*_tmp3CB));_tmp3CB->hd=({struct Cyc_Absyn_Decl*_tmp3CC=_cycalloc(
sizeof(*_tmp3CC));_tmp3CC->r=(void*)((void*)1);_tmp3CC->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp3CC;});
_tmp3CB->tl=Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp3CB;});_tmp3CA;});break;case 11: _LL20B: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3CD;(_tmp3CD.YY18).tag=22;(_tmp3CD.YY18).f1=0;_tmp3CD;});
break;case 12: _LL20C: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)];break;case 13: _LL20D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3CE;(_tmp3CE.YY52).tag=56;(_tmp3CE.YY52).f1=0;_tmp3CE;});break;case 14: _LL20E:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3CF;(_tmp3CF.YY52).tag=
56;(_tmp3CF.YY52).f1=({struct Cyc_List_List*_tmp3D0=_cycalloc(sizeof(*_tmp3D0));
_tmp3D0->hd=({struct _tuple18*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));_tmp3D1->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp3D1->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp3D1->f3=0;_tmp3D1;});_tmp3D0->tl=0;_tmp3D0;});_tmp3CF;});break;case 15: _LL20F:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3D2;(_tmp3D2.YY52).tag=
56;(_tmp3D2.YY52).f1=({struct Cyc_List_List*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));
_tmp3D3->hd=({struct _tuple18*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));_tmp3D4->f1=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp3D4->f2=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp3D4->f3=0;_tmp3D4;});_tmp3D3->tl=Cyc_yyget_YY52(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3D3;});_tmp3D2;});
break;case 16: _LL210: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3D5;(_tmp3D5.YY18).tag=22;(_tmp3D5.YY18).f1=({struct Cyc_List_List*_tmp3D6=
_cycalloc(sizeof(*_tmp3D6));_tmp3D6->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp3D7=_cycalloc(sizeof(*_tmp3D7));_tmp3D7[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp3D8;_tmp3D8.tag=1;_tmp3D8.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3D8;});_tmp3D7;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3D6->tl=0;
_tmp3D6;});_tmp3D5;});break;case 17: _LL211: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 18: _LL212: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3D9;(_tmp3D9.YY18).tag=22;(
_tmp3D9.YY18).f1=0;_tmp3D9;});break;case 19: _LL213: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3DA;(_tmp3DA.YY17).tag=21;(_tmp3DA.YY17).f1=Cyc_Parse_make_function(
0,Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3DA;});
break;case 20: _LL214: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3DB;(_tmp3DB.YY17).tag=21;(_tmp3DB.YY17).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3DC=_cycalloc(sizeof(*_tmp3DC));_tmp3DC->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3DC;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3DB;});
break;case 21: _LL215: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3DD;(_tmp3DD.YY17).tag=21;(_tmp3DD.YY17).f1=Cyc_Parse_make_function(0,Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3DD;});
break;case 22: _LL216: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3DE;(_tmp3DE.YY17).tag=21;(_tmp3DE.YY17).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3DF=_cycalloc(sizeof(*_tmp3DF));_tmp3DF->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp3DF;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3DE;});
break;case 23: _LL217: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3E0;(_tmp3E0.YY17).tag=21;(_tmp3E0.YY17).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3E1=_cycalloc(sizeof(*_tmp3E1));_tmp3E1->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp3E1;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E0;});
break;case 24: _LL218: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3E2;(_tmp3E2.YY17).tag=21;(_tmp3E2.YY17).f1=Cyc_Parse_make_function(({struct
Cyc_Core_Opt*_tmp3E3=_cycalloc(sizeof(*_tmp3E3));_tmp3E3->v=Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp3E3;}),Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E2;});
break;case 25: _LL219: Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 26: _LL21A: Cyc_Lex_leave_using();
break;case 27: _LL21B: Cyc_Lex_enter_namespace(({struct _dynforward_ptr*_tmp3E4=
_cycalloc(sizeof(*_tmp3E4));_tmp3E4[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3E4;}));yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 28: _LL21C: Cyc_Lex_leave_namespace();
break;case 29: _LL21D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3E5;(_tmp3E5.YY18).tag=22;(_tmp3E5.YY18).f1=Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp3E5;});
break;case 30: _LL21E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3E6;(_tmp3E6.YY18).tag=22;(_tmp3E6.YY18).f1=Cyc_Parse_make_declarations(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E6;});
break;case 31: _LL21F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3E7;(_tmp3E7.YY18).tag=22;(_tmp3E7.YY18).f1=({struct Cyc_List_List*_tmp3E8=
_cycalloc(sizeof(*_tmp3E8));_tmp3E8->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3E8->tl=0;
_tmp3E8;});_tmp3E7;});break;case 32: _LL220: {struct Cyc_List_List*_tmp3E9=0;{
struct Cyc_List_List*_tmp3EA=Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);for(0;_tmp3EA != 0;_tmp3EA=_tmp3EA->tl){struct
_dynforward_ptr*_tmp3EB=(struct _dynforward_ptr*)_tmp3EA->hd;struct _tuple1*qv=({
struct _tuple1*_tmp3EE=_cycalloc(sizeof(*_tmp3EE));_tmp3EE->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp3EF;(_tmp3EF.Rel_n).tag=1;(_tmp3EF.Rel_n).f1=0;
_tmp3EF;});_tmp3EE->f2=_tmp3EB;_tmp3EE;});struct Cyc_Absyn_Vardecl*_tmp3EC=Cyc_Absyn_new_vardecl(
qv,Cyc_Absyn_wildtyp(0),0);_tmp3E9=({struct Cyc_List_List*_tmp3ED=_cycalloc(
sizeof(*_tmp3ED));_tmp3ED->hd=_tmp3EC;_tmp3ED->tl=_tmp3E9;_tmp3ED;});}}_tmp3E9=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3E9);yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3F0;(_tmp3F0.YY18).tag=22;(
_tmp3F0.YY18).f1=({struct Cyc_List_List*_tmp3F1=_cycalloc(sizeof(*_tmp3F1));
_tmp3F1->hd=Cyc_Absyn_letv_decl(_tmp3E9,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3F1->tl=0;
_tmp3F1;});_tmp3F0;});break;}case 33: _LL221: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 34: _LL222: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3F2;(_tmp3F2.YY18).tag=22;(
_tmp3F2.YY18).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_imp_append)(Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]),Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]));_tmp3F2;});break;case 35: _LL223: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp3F3;(_tmp3F3.YY19).tag=23;(_tmp3F3.YY19).f1=({struct
Cyc_Parse_Declaration_spec*_tmp3F4=_cycalloc(sizeof(*_tmp3F4));_tmp3F4->sc=({
struct Cyc_Core_Opt*_tmp3F5=_cycalloc(sizeof(*_tmp3F5));_tmp3F5->v=(void*)Cyc_yyget_YY22(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3F5;});_tmp3F4->tq=
Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp3F4->type_specs=
0;_tmp3F4->is_inline=0;_tmp3F4->attributes=0;_tmp3F4;});_tmp3F3;});break;case 36:
_LL224: if((Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc
!= 0)({void*_tmp3F6=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp3F7="Only one storage class is allowed in a declaration";_tag_dynforward(
_tmp3F7,sizeof(char),_get_zero_arr_size_char(_tmp3F7,51));}),_tag_dynforward(
_tmp3F6,sizeof(void*),0));});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp3F8;(_tmp3F8.YY19).tag=23;(_tmp3F8.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp3F9=_cycalloc(sizeof(*_tmp3F9));_tmp3F9->sc=({struct Cyc_Core_Opt*_tmp3FA=
_cycalloc(sizeof(*_tmp3FA));_tmp3FA->v=(void*)Cyc_yyget_YY22(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp3FA;});_tmp3F9->tq=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;
_tmp3F9->type_specs=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->type_specs;_tmp3F9->is_inline=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->is_inline;_tmp3F9->attributes=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;
_tmp3F9;});_tmp3F8;});break;case 37: _LL225: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp3FB;(_tmp3FB.YY19).tag=23;(_tmp3FB.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp3FC=_cycalloc(sizeof(*_tmp3FC));_tmp3FC->sc=0;_tmp3FC->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp3FC->type_specs=({struct Cyc_List_List*_tmp3FD=
_cycalloc(sizeof(*_tmp3FD));_tmp3FD->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp3FD->tl=0;_tmp3FD;});
_tmp3FC->is_inline=0;_tmp3FC->attributes=0;_tmp3FC;});_tmp3FB;});break;case 38:
_LL226: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp3FE;(_tmp3FE.YY19).tag=
23;(_tmp3FE.YY19).f1=({struct Cyc_Parse_Declaration_spec*_tmp3FF=_cycalloc(
sizeof(*_tmp3FF));_tmp3FF->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp3FF->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp3FF->type_specs=({
struct Cyc_List_List*_tmp400=_cycalloc(sizeof(*_tmp400));_tmp400->hd=(void*)Cyc_yyget_YY23(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp400->tl=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;_tmp400;});
_tmp3FF->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp3FF->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp3FF;});
_tmp3FE;});break;case 39: _LL227: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp401;(_tmp401.YY19).tag=23;(_tmp401.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp402=_cycalloc(sizeof(*_tmp402));_tmp402->sc=0;_tmp402->tq=Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp402->type_specs=0;
_tmp402->is_inline=0;_tmp402->attributes=0;_tmp402;});_tmp401;});break;case 40:
_LL228: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp403;(_tmp403.YY19).tag=
23;(_tmp403.YY19).f1=({struct Cyc_Parse_Declaration_spec*_tmp404=_cycalloc(
sizeof(*_tmp404));_tmp404->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp404->tq=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq);_tmp404->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp404->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp404->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp404;});
_tmp403;});break;case 41: _LL229: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp405;(_tmp405.YY19).tag=23;(_tmp405.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp406=_cycalloc(sizeof(*_tmp406));_tmp406->sc=0;_tmp406->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp406->type_specs=0;_tmp406->is_inline=1;_tmp406->attributes=
0;_tmp406;});_tmp405;});break;case 42: _LL22A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp407;(_tmp407.YY19).tag=23;(_tmp407.YY19).f1=({struct
Cyc_Parse_Declaration_spec*_tmp408=_cycalloc(sizeof(*_tmp408));_tmp408->sc=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp408->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp408->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp408->is_inline=1;_tmp408->attributes=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes;_tmp408;});
_tmp407;});break;case 43: _LL22B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp409;(_tmp409.YY19).tag=23;(_tmp409.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp40A=_cycalloc(sizeof(*_tmp40A));_tmp40A->sc=0;_tmp40A->tq=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp40A->type_specs=0;_tmp40A->is_inline=0;_tmp40A->attributes=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp40A;});
_tmp409;});break;case 44: _LL22C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp40B;(_tmp40B.YY19).tag=23;(_tmp40B.YY19).f1=({struct Cyc_Parse_Declaration_spec*
_tmp40C=_cycalloc(sizeof(*_tmp40C));_tmp40C->sc=(Cyc_yyget_YY19(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->sc;_tmp40C->tq=(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tq;_tmp40C->type_specs=(
Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->type_specs;
_tmp40C->is_inline=(Cyc_yyget_YY19(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->is_inline;_tmp40C->attributes=Cyc_List_imp_append(Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY19(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->attributes);_tmp40C;});
_tmp40B;});break;case 45: _LL22D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp40D;(_tmp40D.YY22).tag=26;(_tmp40D.YY22).f1=(void*)((void*)4);_tmp40D;});
break;case 46: _LL22E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp40E;(_tmp40E.YY22).tag=26;(_tmp40E.YY22).f1=(void*)((void*)5);_tmp40E;});
break;case 47: _LL22F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp40F;(_tmp40F.YY22).tag=26;(_tmp40F.YY22).f1=(void*)((void*)3);_tmp40F;});
break;case 48: _LL230: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp410;(_tmp410.YY22).tag=26;(_tmp410.YY22).f1=(void*)((void*)1);_tmp410;});
break;case 49: _LL231: if(Cyc_strcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp411="C";
_tag_dynforward(_tmp411,sizeof(char),_get_zero_arr_size_char(_tmp411,2));}))!= 0)
Cyc_Parse_err(({const char*_tmp412="only extern or extern \"C\" is allowed";
_tag_dynforward(_tmp412,sizeof(char),_get_zero_arr_size_char(_tmp412,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp413;(_tmp413.YY22).tag=26;(_tmp413.YY22).f1=(void*)((
void*)2);_tmp413;});break;case 50: _LL232: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp414;(_tmp414.YY22).tag=26;(_tmp414.YY22).f1=(void*)((void*)0);_tmp414;});
break;case 51: _LL233: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp415;(_tmp415.YY22).tag=26;(_tmp415.YY22).f1=(void*)((void*)6);_tmp415;});
break;case 52: _LL234: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp416;(_tmp416.YY45).tag=49;(_tmp416.YY45).f1=0;_tmp416;});break;case 53: _LL235:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 54:
_LL236: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)];break;
case 55: _LL237: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp417;(
_tmp417.YY45).tag=49;(_tmp417.YY45).f1=({struct Cyc_List_List*_tmp418=_cycalloc(
sizeof(*_tmp418));_tmp418->hd=(void*)Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp418->tl=0;_tmp418;});
_tmp417;});break;case 56: _LL238: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp419;(_tmp419.YY45).tag=49;(_tmp419.YY45).f1=({struct Cyc_List_List*_tmp41A=
_cycalloc(sizeof(*_tmp41A));_tmp41A->hd=(void*)Cyc_yyget_YY46(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp41A->tl=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp41A;});_tmp419;});
break;case 57: _LL239: {static struct Cyc_Absyn_Aligned_att_struct att_aligned={1,- 1};
static struct _tuple19 att_map[17]={{{_tmp41F,_tmp41F + 8},(void*)0},{{_tmp420,
_tmp420 + 6},(void*)1},{{_tmp421,_tmp421 + 9},(void*)2},{{_tmp422,_tmp422 + 9},(
void*)3},{{_tmp423,_tmp423 + 6},(void*)4},{{_tmp424,_tmp424 + 8},(void*)&
att_aligned},{{_tmp425,_tmp425 + 7},(void*)5},{{_tmp426,_tmp426 + 7},(void*)7},{{
_tmp427,_tmp427 + 7},(void*)8},{{_tmp428,_tmp428 + 5},(void*)9},{{_tmp429,_tmp429 + 
10},(void*)10},{{_tmp42A,_tmp42A + 10},(void*)11},{{_tmp42B,_tmp42B + 23},(void*)
12},{{_tmp42C,_tmp42C + 12},(void*)13},{{_tmp42D,_tmp42D + 11},(void*)14},{{
_tmp42E,_tmp42E + 22},(void*)15},{{_tmp42F,_tmp42F + 5},(void*)16}};struct
_dynforward_ptr _tmp41B=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);if((((_get_dynforward_size(_tmp41B,sizeof(char))> 4  && *((
const char*)_check_dynforward_subscript(_tmp41B,sizeof(char),0))== '_') && *((
const char*)_check_dynforward_subscript(_tmp41B,sizeof(char),1))== '_') && *((
const char*)_check_dynforward_subscript(_tmp41B,sizeof(char),(int)(
_get_dynforward_size(_tmp41B,sizeof(char))- 2)))== '_') && *((const char*)
_check_dynforward_subscript(_tmp41B,sizeof(char),(int)(_get_dynforward_size(
_tmp41B,sizeof(char))- 3)))== '_')_tmp41B=(struct _dynforward_ptr)Cyc_substring((
struct _dynforward_ptr)_tmp41B,2,_get_dynforward_size(_tmp41B,sizeof(char))- 5);{
int i=0;for(0;i < 17;++ i){if(Cyc_strcmp((struct _dynforward_ptr)_tmp41B,(struct
_dynforward_ptr)(att_map[_check_known_subscript_notnull(17,i)]).f1)== 0){yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp41C;(_tmp41C.YY46).tag=50;(
_tmp41C.YY46).f1=(void*)(att_map[_check_known_subscript_notnull(17,i)]).f2;
_tmp41C;});break;}}if(i == 17){Cyc_Parse_err(({const char*_tmp41D="unrecognized attribute";
_tag_dynforward(_tmp41D,sizeof(char),_get_zero_arr_size_char(_tmp41D,23));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp41E;(_tmp41E.YY46).tag=50;(_tmp41E.YY46).f1=(void*)((
void*)1);_tmp41E;});}break;}}case 58: _LL23A: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp431;(_tmp431.YY46).tag=50;(_tmp431.YY46).f1=(void*)((void*)
4);_tmp431;});break;case 59: _LL23B: {struct _dynforward_ptr _tmp432=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);int _tmp434;struct
_tuple11 _tmp433=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp434=_tmp433.f2;{void*a;if(Cyc_zstrcmp((struct
_dynforward_ptr)_tmp432,({const char*_tmp435="regparm";_tag_dynforward(_tmp435,
sizeof(char),_get_zero_arr_size_char(_tmp435,8));}))== 0  || Cyc_zstrcmp((struct
_dynforward_ptr)_tmp432,({const char*_tmp436="__regparm__";_tag_dynforward(
_tmp436,sizeof(char),_get_zero_arr_size_char(_tmp436,12));}))== 0){if(_tmp434 < 0
 || _tmp434 > 3)Cyc_Parse_err(({const char*_tmp437="regparm requires value between 0 and 3";
_tag_dynforward(_tmp437,sizeof(char),_get_zero_arr_size_char(_tmp437,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Regparm_att_struct*_tmp438=_cycalloc_atomic(sizeof(*_tmp438));
_tmp438[0]=({struct Cyc_Absyn_Regparm_att_struct _tmp439;_tmp439.tag=0;_tmp439.f1=
_tmp434;_tmp439;});_tmp438;});}else{if(Cyc_zstrcmp((struct _dynforward_ptr)
_tmp432,({const char*_tmp43A="aligned";_tag_dynforward(_tmp43A,sizeof(char),
_get_zero_arr_size_char(_tmp43A,8));}))== 0  || Cyc_zstrcmp((struct
_dynforward_ptr)_tmp432,({const char*_tmp43B="__aligned__";_tag_dynforward(
_tmp43B,sizeof(char),_get_zero_arr_size_char(_tmp43B,12));}))== 0){if(_tmp434 < 0)
Cyc_Parse_err(({const char*_tmp43C="aligned requires positive power of two";
_tag_dynforward(_tmp43C,sizeof(char),_get_zero_arr_size_char(_tmp43C,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{
unsigned int j=(unsigned int)_tmp434;for(0;(j & 1)== 0;j=j >> 1){;}j=j >> 1;if(j != 0)
Cyc_Parse_err(({const char*_tmp43D="aligned requires positive power of two";
_tag_dynforward(_tmp43D,sizeof(char),_get_zero_arr_size_char(_tmp43D,39));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));a=(void*)({
struct Cyc_Absyn_Aligned_att_struct*_tmp43E=_cycalloc_atomic(sizeof(*_tmp43E));
_tmp43E[0]=({struct Cyc_Absyn_Aligned_att_struct _tmp43F;_tmp43F.tag=1;_tmp43F.f1=
_tmp434;_tmp43F;});_tmp43E;});}}else{if(Cyc_zstrcmp((struct _dynforward_ptr)
_tmp432,({const char*_tmp440="initializes";_tag_dynforward(_tmp440,sizeof(char),
_get_zero_arr_size_char(_tmp440,12));}))== 0  || Cyc_zstrcmp((struct
_dynforward_ptr)_tmp432,({const char*_tmp441="__initializes__";_tag_dynforward(
_tmp441,sizeof(char),_get_zero_arr_size_char(_tmp441,16));}))== 0)a=(void*)({
struct Cyc_Absyn_Initializes_att_struct*_tmp442=_cycalloc_atomic(sizeof(*_tmp442));
_tmp442[0]=({struct Cyc_Absyn_Initializes_att_struct _tmp443;_tmp443.tag=4;_tmp443.f1=
_tmp434;_tmp443;});_tmp442;});else{Cyc_Parse_err(({const char*_tmp444="unrecognized attribute";
_tag_dynforward(_tmp444,sizeof(char),_get_zero_arr_size_char(_tmp444,23));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}}}
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp445;(_tmp445.YY46).tag=
50;(_tmp445.YY46).f1=(void*)a;_tmp445;});break;}}case 60: _LL23C: {struct
_dynforward_ptr _tmp446=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dynforward_ptr _tmp447=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((
struct _dynforward_ptr)_tmp446,({const char*_tmp448="section";_tag_dynforward(
_tmp448,sizeof(char),_get_zero_arr_size_char(_tmp448,8));}))== 0  || Cyc_zstrcmp((
struct _dynforward_ptr)_tmp446,({const char*_tmp449="__section__";_tag_dynforward(
_tmp449,sizeof(char),_get_zero_arr_size_char(_tmp449,12));}))== 0)a=(void*)({
struct Cyc_Absyn_Section_att_struct*_tmp44A=_cycalloc(sizeof(*_tmp44A));_tmp44A[0]=({
struct Cyc_Absyn_Section_att_struct _tmp44B;_tmp44B.tag=2;_tmp44B.f1=_tmp447;
_tmp44B;});_tmp44A;});else{Cyc_Parse_err(({const char*_tmp44C="unrecognized attribute";
_tag_dynforward(_tmp44C,sizeof(char),_get_zero_arr_size_char(_tmp44C,23));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp44D;(_tmp44D.YY46).tag=
50;(_tmp44D.YY46).f1=(void*)a;_tmp44D;});break;}case 61: _LL23D: {struct
_dynforward_ptr _tmp44E=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);struct _dynforward_ptr _tmp44F=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);void*a;if(Cyc_zstrcmp((
struct _dynforward_ptr)_tmp44E,({const char*_tmp450="__mode__";_tag_dynforward(
_tmp450,sizeof(char),_get_zero_arr_size_char(_tmp450,9));}))== 0)a=(void*)({
struct Cyc_Absyn_Mode_att_struct*_tmp451=_cycalloc(sizeof(*_tmp451));_tmp451[0]=({
struct Cyc_Absyn_Mode_att_struct _tmp452;_tmp452.tag=5;_tmp452.f1=_tmp44F;_tmp452;});
_tmp451;});else{Cyc_Parse_err(({const char*_tmp453="unrecognized attribute";
_tag_dynforward(_tmp453,sizeof(char),_get_zero_arr_size_char(_tmp453,23));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));a=(void*)1;}
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp454;(_tmp454.YY46).tag=
50;(_tmp454.YY46).f1=(void*)a;_tmp454;});break;}case 62: _LL23E: {struct
_dynforward_ptr _tmp455=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 7)]);struct _dynforward_ptr _tmp456=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);int _tmp458;struct
_tuple11 _tmp457=Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]);_tmp458=_tmp457.f2;{int _tmp45A;struct _tuple11 _tmp459=Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp45A=_tmp459.f2;{
void*a=(void*)1;if(Cyc_zstrcmp((struct _dynforward_ptr)_tmp455,({const char*
_tmp45B="format";_tag_dynforward(_tmp45B,sizeof(char),_get_zero_arr_size_char(
_tmp45B,7));}))== 0  || Cyc_zstrcmp((struct _dynforward_ptr)_tmp455,({const char*
_tmp45C="__format__";_tag_dynforward(_tmp45C,sizeof(char),
_get_zero_arr_size_char(_tmp45C,11));}))== 0){if(Cyc_zstrcmp((struct
_dynforward_ptr)_tmp456,({const char*_tmp45D="printf";_tag_dynforward(_tmp45D,
sizeof(char),_get_zero_arr_size_char(_tmp45D,7));}))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*
_tmp45E=_cycalloc(sizeof(*_tmp45E));_tmp45E[0]=({struct Cyc_Absyn_Format_att_struct
_tmp45F;_tmp45F.tag=3;_tmp45F.f1=(void*)((void*)0);_tmp45F.f2=_tmp458;_tmp45F.f3=
_tmp45A;_tmp45F;});_tmp45E;});else{if(Cyc_zstrcmp((struct _dynforward_ptr)_tmp456,({
const char*_tmp460="scanf";_tag_dynforward(_tmp460,sizeof(char),
_get_zero_arr_size_char(_tmp460,6));}))== 0)a=(void*)({struct Cyc_Absyn_Format_att_struct*
_tmp461=_cycalloc(sizeof(*_tmp461));_tmp461[0]=({struct Cyc_Absyn_Format_att_struct
_tmp462;_tmp462.tag=3;_tmp462.f1=(void*)((void*)1);_tmp462.f2=_tmp458;_tmp462.f3=
_tmp45A;_tmp462;});_tmp461;});else{Cyc_Parse_err(({const char*_tmp463="unrecognized format type";
_tag_dynforward(_tmp463,sizeof(char),_get_zero_arr_size_char(_tmp463,25));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line));}}}else{Cyc_Parse_err(({
const char*_tmp464="unrecognized attribute";_tag_dynforward(_tmp464,sizeof(char),
_get_zero_arr_size_char(_tmp464,23));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));}yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp465;(_tmp465.YY46).tag=50;(_tmp465.YY46).f1=(
void*)a;_tmp465;});break;}}}case 63: _LL23F: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 64: _LL240: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp466;(_tmp466.YY23).tag=27;(
_tmp466.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypedefType_struct*
_tmp467=_cycalloc(sizeof(*_tmp467));_tmp467[0]=({struct Cyc_Absyn_TypedefType_struct
_tmp468;_tmp468.tag=17;_tmp468.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp468.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp468.f3=0;_tmp468.f4=
0;_tmp468;});_tmp467;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp466;});
break;case 65: _LL241: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp469;(_tmp469.YY23).tag=27;(_tmp469.YY23).f1=(void*)Cyc_Parse_type_spec((void*)
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp469;});break;case 66: _LL242: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp46A;(_tmp46A.YY23).tag=27;(_tmp46A.YY23).f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_new_evar(0,0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp46A;});
break;case 67: _LL243: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp46B;(_tmp46B.YY23).tag=27;(_tmp46B.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_new_evar(
Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])),0),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp46B;});
break;case 68: _LL244: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp46C;(_tmp46C.YY23).tag=27;(_tmp46C.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_char_typ,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp46C;});break;case 69: _LL245: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp46D;(_tmp46D.YY23).tag=27;(_tmp46D.YY23).f1=(void*)((
void*)({struct Cyc_Parse_Short_spec_struct*_tmp46E=_cycalloc(sizeof(*_tmp46E));
_tmp46E[0]=({struct Cyc_Parse_Short_spec_struct _tmp46F;_tmp46F.tag=2;_tmp46F.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp46F;});_tmp46E;}));_tmp46D;});break;case 70: _LL246:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp470;(_tmp470.YY23).tag=
27;(_tmp470.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp470;});
break;case 71: _LL247: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp471;(_tmp471.YY23).tag=27;(_tmp471.YY23).f1=(void*)((void*)({struct Cyc_Parse_Long_spec_struct*
_tmp472=_cycalloc(sizeof(*_tmp472));_tmp472[0]=({struct Cyc_Parse_Long_spec_struct
_tmp473;_tmp473.tag=3;_tmp473.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp473;});
_tmp472;}));_tmp471;});break;case 72: _LL248: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp474;(_tmp474.YY23).tag=27;(_tmp474.YY23).f1=(void*)Cyc_Parse_type_spec(
Cyc_Absyn_float_typ,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp474;});
break;case 73: _LL249: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp475;(_tmp475.YY23).tag=27;(_tmp475.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_Absyn_double_typ(
0),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp475;});break;case 74: _LL24A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp476;(_tmp476.YY23).tag=27;(_tmp476.YY23).f1=(void*)((
void*)({struct Cyc_Parse_Signed_spec_struct*_tmp477=_cycalloc(sizeof(*_tmp477));
_tmp477[0]=({struct Cyc_Parse_Signed_spec_struct _tmp478;_tmp478.tag=0;_tmp478.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp478;});_tmp477;}));_tmp476;});break;case 75: _LL24B:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp479;(_tmp479.YY23).tag=
27;(_tmp479.YY23).f1=(void*)((void*)({struct Cyc_Parse_Unsigned_spec_struct*
_tmp47A=_cycalloc(sizeof(*_tmp47A));_tmp47A[0]=({struct Cyc_Parse_Unsigned_spec_struct
_tmp47B;_tmp47B.tag=1;_tmp47B.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp47B;});
_tmp47A;}));_tmp479;});break;case 76: _LL24C: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 77: _LL24D: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 78: _LL24E:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 79:
_LL24F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp47C;(_tmp47C.YY23).tag=
27;(_tmp47C.YY23).f1=(void*)Cyc_Parse_type_spec(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp47C;});
break;case 80: _LL250: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp47D;(_tmp47D.YY23).tag=27;(_tmp47D.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_TupleType_struct*_tmp47E=_cycalloc(sizeof(*_tmp47E));_tmp47E[0]=({
struct Cyc_Absyn_TupleType_struct _tmp47F;_tmp47F.tag=9;_tmp47F.f1=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));_tmp47F;});_tmp47E;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp47D;});break;case 81: _LL251: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp480;(_tmp480.YY23).tag=27;(_tmp480.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp481=_cycalloc(sizeof(*_tmp481));
_tmp481[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp482;_tmp482.tag=15;_tmp482.f1=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp482;});_tmp481;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp480;});
break;case 82: _LL252: {void*_tmp483=Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,
0);yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp484;(_tmp484.YY23).tag=
27;(_tmp484.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_DynRgnType_struct*
_tmp485=_cycalloc(sizeof(*_tmp485));_tmp485[0]=({struct Cyc_Absyn_DynRgnType_struct
_tmp486;_tmp486.tag=16;_tmp486.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp486.f2=(void*)
_tmp483;_tmp486;});_tmp485;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp484;});
break;}case 83: _LL253: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp487;(_tmp487.YY23).tag=27;(_tmp487.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_DynRgnType_struct*_tmp488=_cycalloc(sizeof(*_tmp488));_tmp488[0]=({
struct Cyc_Absyn_DynRgnType_struct _tmp489;_tmp489.tag=16;_tmp489.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp489.f2=(void*)
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp489;});_tmp488;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp487;});
break;case 84: _LL254: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp48A;(_tmp48A.YY23).tag=27;(_tmp48A.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_SizeofType_struct*_tmp48B=_cycalloc(sizeof(*_tmp48B));_tmp48B[0]=({
struct Cyc_Absyn_SizeofType_struct _tmp48C;_tmp48C.tag=14;_tmp48C.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp48C;});_tmp48B;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp48A;});break;case 85: _LL255: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp48D;(_tmp48D.YY23).tag=27;(_tmp48D.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TagType_struct*_tmp48E=_cycalloc(sizeof(*_tmp48E));
_tmp48E[0]=({struct Cyc_Absyn_TagType_struct _tmp48F;_tmp48F.tag=18;_tmp48F.f1=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp48F;});_tmp48E;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp48D;});
break;case 86: _LL256: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp490;(_tmp490.YY23).tag=27;(_tmp490.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_TagType_struct*_tmp491=_cycalloc(sizeof(*_tmp491));_tmp491[0]=({
struct Cyc_Absyn_TagType_struct _tmp492;_tmp492.tag=18;_tmp492.f1=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ik,0);_tmp492;});_tmp491;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp490;});
break;case 87: _LL257: {int _tmp494;struct _tuple11 _tmp493=Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp494=_tmp493.f2;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp495;(_tmp495.YY23).tag=27;(
_tmp495.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({struct Cyc_Absyn_TypeInt_struct*
_tmp496=_cycalloc_atomic(sizeof(*_tmp496));_tmp496[0]=({struct Cyc_Absyn_TypeInt_struct
_tmp497;_tmp497.tag=19;_tmp497.f1=_tmp494;_tmp497;});_tmp496;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp495;});
break;}case 88: _LL258: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp498;(_tmp498.YY43).tag=47;(_tmp498.YY43).f1=(void*)Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp498;});
break;case 89: _LL259: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line): 0;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp499;(_tmp499.YY25).tag=29;(_tmp499.YY25).f1=({struct Cyc_Absyn_Tqual _tmp49A;
_tmp49A.print_const=1;_tmp49A.q_volatile=0;_tmp49A.q_restrict=0;_tmp49A.real_const=
1;_tmp49A.loc=loc;_tmp49A;});_tmp499;});break;}case 90: _LL25A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp49B;(_tmp49B.YY25).tag=29;(_tmp49B.YY25).f1=({struct
Cyc_Absyn_Tqual _tmp49C;_tmp49C.print_const=0;_tmp49C.q_volatile=1;_tmp49C.q_restrict=
0;_tmp49C.real_const=0;_tmp49C.loc=0;_tmp49C;});_tmp49B;});break;case 91: _LL25B:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp49D;(_tmp49D.YY25).tag=
29;(_tmp49D.YY25).f1=({struct Cyc_Absyn_Tqual _tmp49E;_tmp49E.print_const=0;
_tmp49E.q_volatile=0;_tmp49E.q_restrict=1;_tmp49E.real_const=0;_tmp49E.loc=0;
_tmp49E;});_tmp49D;});break;case 92: _LL25C: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp49F;(_tmp49F.YY23).tag=27;(_tmp49F.YY23).f1=(void*)((void*)({
struct Cyc_Parse_Decl_spec_struct*_tmp4A0=_cycalloc(sizeof(*_tmp4A0));_tmp4A0[0]=({
struct Cyc_Parse_Decl_spec_struct _tmp4A1;_tmp4A1.tag=5;_tmp4A1.f1=({struct Cyc_Absyn_Decl*
_tmp4A2=_cycalloc(sizeof(*_tmp4A2));_tmp4A2->r=(void*)((void*)({struct Cyc_Absyn_Enum_d_struct*
_tmp4A3=_cycalloc(sizeof(*_tmp4A3));_tmp4A3[0]=({struct Cyc_Absyn_Enum_d_struct
_tmp4A4;_tmp4A4.tag=6;_tmp4A4.f1=({struct Cyc_Absyn_Enumdecl*_tmp4A5=_cycalloc(
sizeof(*_tmp4A5));_tmp4A5->sc=(void*)((void*)2);_tmp4A5->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp4A5->fields=({
struct Cyc_Core_Opt*_tmp4A6=_cycalloc(sizeof(*_tmp4A6));_tmp4A6->v=Cyc_yyget_YY48(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4A6;});_tmp4A5;});
_tmp4A4;});_tmp4A3;}));_tmp4A2->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4A2;});
_tmp4A1;});_tmp4A0;}));_tmp49F;});break;case 93: _LL25D: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4A7;(_tmp4A7.YY23).tag=27;(_tmp4A7.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_EnumType_struct*_tmp4A8=_cycalloc(sizeof(*_tmp4A8));
_tmp4A8[0]=({struct Cyc_Absyn_EnumType_struct _tmp4A9;_tmp4A9.tag=12;_tmp4A9.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp4A9.f2=0;_tmp4A9;});_tmp4A8;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4A7;});
break;case 94: _LL25E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4AA;(_tmp4AA.YY23).tag=27;(_tmp4AA.YY23).f1=(void*)((void*)({struct Cyc_Parse_Type_spec_struct*
_tmp4AB=_cycalloc(sizeof(*_tmp4AB));_tmp4AB[0]=({struct Cyc_Parse_Type_spec_struct
_tmp4AC;_tmp4AC.tag=4;_tmp4AC.f1=(void*)((void*)({struct Cyc_Absyn_AnonEnumType_struct*
_tmp4AD=_cycalloc(sizeof(*_tmp4AD));_tmp4AD[0]=({struct Cyc_Absyn_AnonEnumType_struct
_tmp4AE;_tmp4AE.tag=13;_tmp4AE.f1=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4AE;});_tmp4AD;}));
_tmp4AC.f2=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4AC;});_tmp4AB;}));_tmp4AA;});break;case 95: _LL25F:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4AF;(_tmp4AF.YY47).tag=
51;(_tmp4AF.YY47).f1=({struct Cyc_Absyn_Enumfield*_tmp4B0=_cycalloc(sizeof(*
_tmp4B0));_tmp4B0->name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp4B0->tag=0;_tmp4B0->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp4B0;});
_tmp4AF;});break;case 96: _LL260: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4B1;(_tmp4B1.YY47).tag=51;(_tmp4B1.YY47).f1=({struct Cyc_Absyn_Enumfield*
_tmp4B2=_cycalloc(sizeof(*_tmp4B2));_tmp4B2->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4B2->tag=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4B2->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp4B2;});_tmp4B1;});break;case 97: _LL261: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4B3;(_tmp4B3.YY48).tag=52;(
_tmp4B3.YY48).f1=({struct Cyc_List_List*_tmp4B4=_cycalloc(sizeof(*_tmp4B4));
_tmp4B4->hd=Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4B4->tl=0;_tmp4B4;});_tmp4B3;});break;case 98: _LL262: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4B5;(_tmp4B5.YY48).tag=52;(
_tmp4B5.YY48).f1=({struct Cyc_List_List*_tmp4B6=_cycalloc(sizeof(*_tmp4B6));
_tmp4B6->hd=Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp4B6->tl=0;_tmp4B6;});_tmp4B5;});break;case 99: _LL263: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4B7;(_tmp4B7.YY48).tag=52;(
_tmp4B7.YY48).f1=({struct Cyc_List_List*_tmp4B8=_cycalloc(sizeof(*_tmp4B8));
_tmp4B8->hd=Cyc_yyget_YY47(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp4B8->tl=Cyc_yyget_YY48(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4B8;});_tmp4B7;});
break;case 100: _LL264: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4B9;(_tmp4B9.YY23).tag=27;(_tmp4B9.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_AnonAggrType_struct*_tmp4BA=_cycalloc(sizeof(*_tmp4BA));_tmp4BA[
0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp4BB;_tmp4BB.tag=11;_tmp4BB.f1=(void*)
Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp4BB.f2=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset
- 1)]);_tmp4BB;});_tmp4BA;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4B9;});
break;case 101: _LL265: {struct Cyc_List_List*_tmp4BC=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]));struct Cyc_List_List*
_tmp4BD=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4BE;(_tmp4BE.YY23).tag=27;(_tmp4BE.YY23).f1=(void*)((void*)({struct Cyc_Parse_Decl_spec_struct*
_tmp4BF=_cycalloc(sizeof(*_tmp4BF));_tmp4BF[0]=({struct Cyc_Parse_Decl_spec_struct
_tmp4C0;_tmp4C0.tag=5;_tmp4C0.f1=Cyc_Absyn_aggr_decl(Cyc_yyget_YY24(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),(void*)2,Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),_tmp4BC,(struct Cyc_Absyn_AggrdeclImpl*)
Cyc_Absyn_aggrdecl_impl(_tmp4BD,Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY26(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4C0;});
_tmp4BF;}));_tmp4BE;});break;}case 102: _LL266: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4C1;(_tmp4C1.YY23).tag=27;(_tmp4C1.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_AggrType_struct*_tmp4C2=_cycalloc(sizeof(*_tmp4C2));
_tmp4C2[0]=({struct Cyc_Absyn_AggrType_struct _tmp4C3;_tmp4C3.tag=10;_tmp4C3.f1=({
struct Cyc_Absyn_AggrInfo _tmp4C4;_tmp4C4.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({
union Cyc_Absyn_AggrInfoU_union _tmp4C5;(_tmp4C5.UnknownAggr).tag=0;(_tmp4C5.UnknownAggr).f1=(
void*)Cyc_yyget_YY24(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);(
_tmp4C5.UnknownAggr).f2=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp4C5;});_tmp4C4.targs=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4C4;});_tmp4C3;});
_tmp4C2;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4C1;});break;case 103: _LL267: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4C6;(_tmp4C6.YY40).tag=44;(_tmp4C6.YY40).f1=0;_tmp4C6;});
break;case 104: _LL268: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4C7;(_tmp4C7.YY40).tag=44;(_tmp4C7.YY40).f1=Cyc_List_imp_rev(Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp4C7;});break;
case 105: _LL269: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4C8;(
_tmp4C8.YY24).tag=28;(_tmp4C8.YY24).f1=(void*)((void*)0);_tmp4C8;});break;case
106: _LL26A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4C9;(
_tmp4C9.YY24).tag=28;(_tmp4C9.YY24).f1=(void*)((void*)1);_tmp4C9;});break;case
107: _LL26B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4CA;(
_tmp4CA.YY26).tag=30;(_tmp4CA.YY26).f1=0;_tmp4CA;});break;case 108: _LL26C: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4CB;(_tmp4CB.YY26).tag=30;(
_tmp4CB.YY26).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_flatten)(((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY27(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));_tmp4CB;});break;
case 109: _LL26D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4CC;(
_tmp4CC.YY27).tag=31;(_tmp4CC.YY27).f1=({struct Cyc_List_List*_tmp4CD=_cycalloc(
sizeof(*_tmp4CD));_tmp4CD->hd=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp4CD->tl=0;_tmp4CD;});_tmp4CC;});break;case 110: _LL26E:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4CE;(_tmp4CE.YY27).tag=
31;(_tmp4CE.YY27).f1=({struct Cyc_List_List*_tmp4CF=_cycalloc(sizeof(*_tmp4CF));
_tmp4CF->hd=Cyc_yyget_YY26(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4CF->tl=Cyc_yyget_YY27(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp4CF;});_tmp4CE;});break;case 111: _LL26F: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4D0;(_tmp4D0.YY21).tag=25;(_tmp4D0.YY21).f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp4D0;});break;case
112: _LL270: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4D1;(
_tmp4D1.YY21).tag=25;(_tmp4D1.YY21).f1=({struct Cyc_List_List*_tmp4D2=_cycalloc(
sizeof(*_tmp4D2));_tmp4D2->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp4D2->tl=0;_tmp4D2;});_tmp4D1;});break;case 113: _LL271:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4D3;(_tmp4D3.YY21).tag=
25;(_tmp4D3.YY21).f1=({struct Cyc_List_List*_tmp4D4=_cycalloc(sizeof(*_tmp4D4));
_tmp4D4->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4D4->tl=Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp4D4;});_tmp4D3;});break;case 114: _LL272: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp4D5;(_tmp4D5.YY20).tag=24;(_tmp4D5.YY20).f1=({
struct _tuple15*_tmp4D6=_cycalloc(sizeof(*_tmp4D6));_tmp4D6->f1=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4D6->f2=0;_tmp4D6;});
_tmp4D5;});break;case 115: _LL273: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4D7;(_tmp4D7.YY20).tag=24;(_tmp4D7.YY20).f1=({struct _tuple15*_tmp4D8=
_cycalloc(sizeof(*_tmp4D8));_tmp4D8->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4D8->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4D8;});
_tmp4D7;});break;case 116: _LL274:{struct _RegionHandle _tmp4D9=_new_region("temp");
struct _RegionHandle*temp=& _tmp4D9;_push_region(temp);{struct _tuple16 _tmp4DB;
struct Cyc_Absyn_Tqual _tmp4DC;struct Cyc_List_List*_tmp4DD;struct Cyc_List_List*
_tmp4DE;struct _tuple16*_tmp4DA=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp4DB=*_tmp4DA;_tmp4DC=
_tmp4DB.f1;_tmp4DD=_tmp4DB.f2;_tmp4DE=_tmp4DB.f3;if(_tmp4DC.loc == 0)_tmp4DC.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);{struct Cyc_List_List*_tmp4E0;struct Cyc_List_List*
_tmp4E1;struct _tuple0 _tmp4DF=((struct _tuple0(*)(struct _RegionHandle*r1,struct
_RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(temp,temp,Cyc_yyget_YY21(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp4E0=_tmp4DF.f1;
_tmp4E1=_tmp4DF.f2;{void*_tmp4E2=Cyc_Parse_speclist2typ(_tmp4DD,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));struct Cyc_List_List*
_tmp4E3=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,Cyc_Parse_apply_tmss(
temp,_tmp4DC,_tmp4E2,_tmp4E0,_tmp4DE),_tmp4E1);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4E4;(_tmp4E4.YY26).tag=30;(_tmp4E4.YY26).f1=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Position_Segment*,
struct _tuple8*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(
Cyc_Parse_make_aggr_field,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp4E3);
_tmp4E4;});}}};_pop_region(temp);}break;case 117: _LL275: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4E5;(_tmp4E5.YY35).tag=39;(_tmp4E5.YY35).f1=({struct
_tuple16*_tmp4E6=_cycalloc(sizeof(*_tmp4E6));_tmp4E6->f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4E6->f2=({struct Cyc_List_List*_tmp4E7=_cycalloc(
sizeof(*_tmp4E7));_tmp4E7->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4E7->tl=0;_tmp4E7;});
_tmp4E6->f3=0;_tmp4E6;});_tmp4E5;});break;case 118: _LL276: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4E8;(_tmp4E8.YY35).tag=39;(_tmp4E8.YY35).f1=({struct
_tuple16*_tmp4E9=_cycalloc(sizeof(*_tmp4E9));_tmp4E9->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp4E9->f2=({struct Cyc_List_List*
_tmp4EA=_cycalloc(sizeof(*_tmp4EA));_tmp4EA->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4EA->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp4EA;});_tmp4E9->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;
_tmp4E9;});_tmp4E8;});break;case 119: _LL277: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp4EB;(_tmp4EB.YY35).tag=39;(_tmp4EB.YY35).f1=({struct
_tuple16*_tmp4EC=_cycalloc(sizeof(*_tmp4EC));_tmp4EC->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4EC->f2=0;_tmp4EC->f3=0;
_tmp4EC;});_tmp4EB;});break;case 120: _LL278: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp4ED;(_tmp4ED.YY35).tag=39;(_tmp4ED.YY35).f1=({struct
_tuple16*_tmp4EE=_cycalloc(sizeof(*_tmp4EE));_tmp4EE->f1=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);
_tmp4EE->f2=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f2;_tmp4EE->f3=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp4EE;});_tmp4ED;});
break;case 121: _LL279: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4EF;(_tmp4EF.YY35).tag=39;(_tmp4EF.YY35).f1=({struct _tuple16*_tmp4F0=
_cycalloc(sizeof(*_tmp4F0));_tmp4F0->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4F0->f2=0;
_tmp4F0->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4F0;});_tmp4EF;});break;case 122: _LL27A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4F1;(_tmp4F1.YY35).tag=39;(_tmp4F1.YY35).f1=({struct
_tuple16*_tmp4F2=_cycalloc(sizeof(*_tmp4F2));_tmp4F2->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp4F2->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp4F2->f3=Cyc_List_append(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);
_tmp4F2;});_tmp4F1;});break;case 123: _LL27B: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp4F3;(_tmp4F3.YY35).tag=39;(_tmp4F3.YY35).f1=({struct
_tuple16*_tmp4F4=_cycalloc(sizeof(*_tmp4F4));_tmp4F4->f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp4F4->f2=({struct Cyc_List_List*_tmp4F5=_cycalloc(
sizeof(*_tmp4F5));_tmp4F5->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4F5->tl=0;_tmp4F5;});
_tmp4F4->f3=0;_tmp4F4;});_tmp4F3;});break;case 124: _LL27C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4F6;(_tmp4F6.YY35).tag=39;(_tmp4F6.YY35).f1=({struct
_tuple16*_tmp4F7=_cycalloc(sizeof(*_tmp4F7));_tmp4F7->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp4F7->f2=({struct Cyc_List_List*
_tmp4F8=_cycalloc(sizeof(*_tmp4F8));_tmp4F8->hd=(void*)Cyc_yyget_YY23(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp4F8->tl=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp4F8;});_tmp4F7->f3=(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;
_tmp4F7;});_tmp4F6;});break;case 125: _LL27D: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp4F9;(_tmp4F9.YY35).tag=39;(_tmp4F9.YY35).f1=({struct
_tuple16*_tmp4FA=_cycalloc(sizeof(*_tmp4FA));_tmp4FA->f1=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp4FA->f2=0;_tmp4FA->f3=0;
_tmp4FA;});_tmp4F9;});break;case 126: _LL27E: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp4FB;(_tmp4FB.YY35).tag=39;(_tmp4FB.YY35).f1=({struct
_tuple16*_tmp4FC=_cycalloc(sizeof(*_tmp4FC));_tmp4FC->f1=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1);
_tmp4FC->f2=(*Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)])).f2;_tmp4FC->f3=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp4FC;});_tmp4FB;});
break;case 127: _LL27F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp4FD;(_tmp4FD.YY35).tag=39;(_tmp4FD.YY35).f1=({struct _tuple16*_tmp4FE=
_cycalloc(sizeof(*_tmp4FE));_tmp4FE->f1=Cyc_Absyn_empty_tqual(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp4FE->f2=0;
_tmp4FE->f3=Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp4FE;});_tmp4FD;});break;case 128: _LL280: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp4FF;(_tmp4FF.YY35).tag=39;(_tmp4FF.YY35).f1=({struct
_tuple16*_tmp500=_cycalloc(sizeof(*_tmp500));_tmp500->f1=(*Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f1;_tmp500->f2=(*Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2;_tmp500->f3=Cyc_List_append(
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(*
Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f3);
_tmp500;});_tmp4FF;});break;case 129: _LL281: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp501;(_tmp501.YY21).tag=25;(_tmp501.YY21).f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY21(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp501;});break;case 130:
_LL282: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp502;(_tmp502.YY21).tag=
25;(_tmp502.YY21).f1=({struct Cyc_List_List*_tmp503=_cycalloc(sizeof(*_tmp503));
_tmp503->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp503->tl=0;_tmp503;});_tmp502;});break;case 131: _LL283: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp504;(_tmp504.YY21).tag=25;(
_tmp504.YY21).f1=({struct Cyc_List_List*_tmp505=_cycalloc(sizeof(*_tmp505));
_tmp505->hd=Cyc_yyget_YY20(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp505->tl=Cyc_yyget_YY21(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp505;});_tmp504;});break;case 132: _LL284: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp506;(_tmp506.YY20).tag=24;(_tmp506.YY20).f1=({
struct _tuple15*_tmp507=_cycalloc(sizeof(*_tmp507));_tmp507->f1=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp507->f2=0;_tmp507;});
_tmp506;});break;case 133: _LL285: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp508;(_tmp508.YY20).tag=24;(_tmp508.YY20).f1=({struct _tuple15*_tmp509=
_cycalloc(sizeof(*_tmp509));_tmp509->f1=({struct Cyc_Parse_Declarator*_tmp50A=
_cycalloc(sizeof(*_tmp50A));_tmp50A->id=({struct _tuple1*_tmp50B=_cycalloc(
sizeof(*_tmp50B));_tmp50B->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp50D;(_tmp50D.Rel_n).tag=1;(_tmp50D.Rel_n).f1=0;_tmp50D;});_tmp50B->f2=
_init_dynforward_ptr(_cycalloc(sizeof(struct _dynforward_ptr)),"",sizeof(char),1);
_tmp50B;});_tmp50A->tms=0;_tmp50A;});_tmp509->f2=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp509;});_tmp508;});
break;case 134: _LL286: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp50E;(_tmp50E.YY20).tag=24;(_tmp50E.YY20).f1=({struct _tuple15*_tmp50F=
_cycalloc(sizeof(*_tmp50F));_tmp50F->f1=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp50F->f2=(struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp50F;});
_tmp50E;});break;case 135: _LL287: {struct Cyc_List_List*_tmp510=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line),Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp511;(_tmp511.YY23).tag=27;(_tmp511.YY23).f1=(void*)((
void*)({struct Cyc_Parse_Decl_spec_struct*_tmp512=_cycalloc(sizeof(*_tmp512));
_tmp512[0]=({struct Cyc_Parse_Decl_spec_struct _tmp513;_tmp513.tag=5;_tmp513.f1=
Cyc_Absyn_tunion_decl((void*)2,Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),_tmp510,({struct Cyc_Core_Opt*
_tmp514=_cycalloc(sizeof(*_tmp514));_tmp514->v=Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp514;}),Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY31(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 7)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp513;});
_tmp512;}));_tmp511;});break;}case 136: _LL288: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp515;(_tmp515.YY23).tag=27;(_tmp515.YY23).f1=(void*)Cyc_Parse_type_spec((
void*)({struct Cyc_Absyn_TunionType_struct*_tmp516=_cycalloc(sizeof(*_tmp516));
_tmp516[0]=({struct Cyc_Absyn_TunionType_struct _tmp517;_tmp517.tag=2;_tmp517.f1=({
struct Cyc_Absyn_TunionInfo _tmp518;_tmp518.tunion_info=(union Cyc_Absyn_TunionInfoU_union)({
union Cyc_Absyn_TunionInfoU_union _tmp519;(_tmp519.UnknownTunion).tag=0;(_tmp519.UnknownTunion).f1=({
struct Cyc_Absyn_UnknownTunionInfo _tmp51A;_tmp51A.name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp51A.is_xtunion=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp51A.is_flat=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp51A;});_tmp519;});
_tmp518.targs=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp518.rgn=Cyc_yyget_YY49(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp518;});_tmp517;});_tmp516;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp515;});
break;case 137: _LL289: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp51B;(_tmp51B.YY23).tag=27;(_tmp51B.YY23).f1=(void*)Cyc_Parse_type_spec((void*)({
struct Cyc_Absyn_TunionFieldType_struct*_tmp51C=_cycalloc(sizeof(*_tmp51C));
_tmp51C[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp51D;_tmp51D.tag=3;_tmp51D.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmp51E;_tmp51E.field_info=(union Cyc_Absyn_TunionFieldInfoU_union)({
union Cyc_Absyn_TunionFieldInfoU_union _tmp51F;(_tmp51F.UnknownTunionfield).tag=0;(
_tmp51F.UnknownTunionfield).f1=({struct Cyc_Absyn_UnknownTunionFieldInfo _tmp520;
_tmp520.tunion_name=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]);_tmp520.field_name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp520.is_xtunion=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp520;});_tmp51F;});
_tmp51E.targs=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp51E;});_tmp51D;});_tmp51C;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp51B;});
break;case 138: _LL28A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp521;(_tmp521.YY31).tag=35;(_tmp521.YY31).f1=1;_tmp521;});break;case 139:
_LL28B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp522;(_tmp522.YY31).tag=
35;(_tmp522.YY31).f1=0;_tmp522;});break;case 140: _LL28C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp523;(_tmp523.YY31).tag=35;(_tmp523.YY31).f1=0;_tmp523;});
break;case 141: _LL28D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp524;(_tmp524.YY31).tag=35;(_tmp524.YY31).f1=1;_tmp524;});break;case 142:
_LL28E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp525;(_tmp525.YY34).tag=
38;(_tmp525.YY34).f1=({struct Cyc_List_List*_tmp526=_cycalloc(sizeof(*_tmp526));
_tmp526->hd=Cyc_yyget_YY33(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp526->tl=0;_tmp526;});_tmp525;});break;case 143: _LL28F: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp527;(_tmp527.YY34).tag=38;(
_tmp527.YY34).f1=({struct Cyc_List_List*_tmp528=_cycalloc(sizeof(*_tmp528));
_tmp528->hd=Cyc_yyget_YY33(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp528->tl=0;_tmp528;});_tmp527;});break;case 144: _LL290:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp529;(_tmp529.YY34).tag=
38;(_tmp529.YY34).f1=({struct Cyc_List_List*_tmp52A=_cycalloc(sizeof(*_tmp52A));
_tmp52A->hd=Cyc_yyget_YY33(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]);_tmp52A->tl=Cyc_yyget_YY34(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp52A;});_tmp529;});
break;case 145: _LL291: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp52B;(_tmp52B.YY34).tag=38;(_tmp52B.YY34).f1=({struct Cyc_List_List*_tmp52C=
_cycalloc(sizeof(*_tmp52C));_tmp52C->hd=Cyc_yyget_YY33(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp52C->tl=Cyc_yyget_YY34(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp52C;});_tmp52B;});
break;case 146: _LL292: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp52D;(_tmp52D.YY32).tag=36;(_tmp52D.YY32).f1=(void*)((void*)2);_tmp52D;});
break;case 147: _LL293: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp52E;(_tmp52E.YY32).tag=36;(_tmp52E.YY32).f1=(void*)((void*)3);_tmp52E;});
break;case 148: _LL294: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp52F;(_tmp52F.YY32).tag=36;(_tmp52F.YY32).f1=(void*)((void*)0);_tmp52F;});
break;case 149: _LL295: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp530;(_tmp530.YY33).tag=37;(_tmp530.YY33).f1=({struct Cyc_Absyn_Tunionfield*
_tmp531=_cycalloc(sizeof(*_tmp531));_tmp531->name=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp531->typs=0;_tmp531->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp531->sc=(void*)Cyc_yyget_YY32(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp531;});_tmp530;});
break;case 150: _LL296: {struct Cyc_List_List*_tmp532=((struct Cyc_List_List*(*)(
struct _tuple9*(*f)(struct Cyc_Position_Segment*,struct _tuple2*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp533;(_tmp533.YY33).tag=37;(_tmp533.YY33).f1=({struct
Cyc_Absyn_Tunionfield*_tmp534=_cycalloc(sizeof(*_tmp534));_tmp534->name=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp534->typs=
_tmp532;_tmp534->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp534->sc=(
void*)Cyc_yyget_YY32(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
_tmp534;});_tmp533;});break;}case 151: _LL297: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 152: _LL298: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp535;(_tmp535.YY29).tag=33;(
_tmp535.YY29).f1=({struct Cyc_Parse_Declarator*_tmp536=_cycalloc(sizeof(*_tmp536));
_tmp536->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]))->id;_tmp536->tms=Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp536;});_tmp535;});
break;case 153: _LL299: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 154: _LL29A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp537;(_tmp537.YY29).tag=33;(_tmp537.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp538=_cycalloc(sizeof(*_tmp538));_tmp538->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->id;_tmp538->tms=Cyc_List_imp_append(
Cyc_yyget_YY28(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp538;});
_tmp537;});break;case 155: _LL29B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp539;(_tmp539.YY29).tag=33;(_tmp539.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp53A=_cycalloc(sizeof(*_tmp53A));_tmp53A->id=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp53A->tms=0;_tmp53A;});
_tmp539;});break;case 156: _LL29C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 157: _LL29D: {struct Cyc_Parse_Declarator*_tmp53B=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp53B->tms=({
struct Cyc_List_List*_tmp53C=_cycalloc(sizeof(*_tmp53C));_tmp53C->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp53D=_cycalloc(sizeof(*_tmp53D));
_tmp53D[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp53E;_tmp53E.tag=5;_tmp53E.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line);_tmp53E.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp53E;});_tmp53D;}));
_tmp53C->tl=_tmp53B->tms;_tmp53C;});yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)];break;}case 158: _LL29E: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp53F;(_tmp53F.YY29).tag=33;(_tmp53F.YY29).f1=({struct
Cyc_Parse_Declarator*_tmp540=_cycalloc(sizeof(*_tmp540));_tmp540->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp540->tms=({
struct Cyc_List_List*_tmp541=_cycalloc(sizeof(*_tmp541));_tmp541->hd=(void*)((
void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp542=_cycalloc(sizeof(*_tmp542));
_tmp542[0]=({struct Cyc_Absyn_Carray_mod_struct _tmp543;_tmp543.tag=0;_tmp543.f1=
Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp543.f2=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp543;});_tmp542;}));_tmp541->tl=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp541;});
_tmp540;});_tmp53F;});break;case 159: _LL29F: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp544;(_tmp544.YY29).tag=33;(_tmp544.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp545=_cycalloc(sizeof(*_tmp545));_tmp545->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp545->tms=({
struct Cyc_List_List*_tmp546=_cycalloc(sizeof(*_tmp546));_tmp546->hd=(void*)((
void*)({struct Cyc_Absyn_ConstArray_mod_struct*_tmp547=_cycalloc(sizeof(*_tmp547));
_tmp547[0]=({struct Cyc_Absyn_ConstArray_mod_struct _tmp548;_tmp548.tag=1;_tmp548.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp548.f2=Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp548.f3=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp548;});_tmp547;}));_tmp546->tl=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp546;});
_tmp545;});_tmp544;});break;case 160: _LL2A0: {struct _tuple17 _tmp54A;struct Cyc_List_List*
_tmp54B;int _tmp54C;struct Cyc_Absyn_VarargInfo*_tmp54D;struct Cyc_Core_Opt*_tmp54E;
struct Cyc_List_List*_tmp54F;struct _tuple17*_tmp549=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp54A=*_tmp549;_tmp54B=
_tmp54A.f1;_tmp54C=_tmp54A.f2;_tmp54D=_tmp54A.f3;_tmp54E=_tmp54A.f4;_tmp54F=
_tmp54A.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp550;(
_tmp550.YY29).tag=33;(_tmp550.YY29).f1=({struct Cyc_Parse_Declarator*_tmp551=
_cycalloc(sizeof(*_tmp551));_tmp551->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp551->tms=({
struct Cyc_List_List*_tmp552=_cycalloc(sizeof(*_tmp552));_tmp552->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp553=_cycalloc(sizeof(*_tmp553));
_tmp553[0]=({struct Cyc_Absyn_Function_mod_struct _tmp554;_tmp554.tag=3;_tmp554.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp555=_cycalloc(sizeof(*
_tmp555));_tmp555[0]=({struct Cyc_Absyn_WithTypes_struct _tmp556;_tmp556.tag=1;
_tmp556.f1=_tmp54B;_tmp556.f2=_tmp54C;_tmp556.f3=_tmp54D;_tmp556.f4=_tmp54E;
_tmp556.f5=_tmp54F;_tmp556;});_tmp555;}));_tmp554;});_tmp553;}));_tmp552->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;
_tmp552;});_tmp551;});_tmp550;});break;}case 161: _LL2A1: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp557;(_tmp557.YY29).tag=33;(_tmp557.YY29).f1=({struct
Cyc_Parse_Declarator*_tmp558=_cycalloc(sizeof(*_tmp558));_tmp558->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp558->tms=({
struct Cyc_List_List*_tmp559=_cycalloc(sizeof(*_tmp559));_tmp559->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp55A=_cycalloc(sizeof(*_tmp55A));
_tmp55A[0]=({struct Cyc_Absyn_Function_mod_struct _tmp55B;_tmp55B.tag=3;_tmp55B.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp55C=_cycalloc(sizeof(*
_tmp55C));_tmp55C[0]=({struct Cyc_Absyn_WithTypes_struct _tmp55D;_tmp55D.tag=1;
_tmp55D.f1=0;_tmp55D.f2=0;_tmp55D.f3=0;_tmp55D.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp55D.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp55D;});_tmp55C;}));
_tmp55B;});_tmp55A;}));_tmp559->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp559;});_tmp558;});
_tmp557;});break;case 162: _LL2A2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp55E;(_tmp55E.YY29).tag=33;(_tmp55E.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp55F=_cycalloc(sizeof(*_tmp55F));_tmp55F->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp55F->tms=({
struct Cyc_List_List*_tmp560=_cycalloc(sizeof(*_tmp560));_tmp560->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp561=_cycalloc(sizeof(*_tmp561));
_tmp561[0]=({struct Cyc_Absyn_Function_mod_struct _tmp562;_tmp562.tag=3;_tmp562.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp563=_cycalloc(sizeof(*_tmp563));
_tmp563[0]=({struct Cyc_Absyn_NoTypes_struct _tmp564;_tmp564.tag=0;_tmp564.f1=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp564.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp564;});
_tmp563;}));_tmp562;});_tmp561;}));_tmp560->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp560;});_tmp55F;});
_tmp55E;});break;case 163: _LL2A3: {struct Cyc_List_List*_tmp565=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp566;(_tmp566.YY29).tag=
33;(_tmp566.YY29).f1=({struct Cyc_Parse_Declarator*_tmp567=_cycalloc(sizeof(*
_tmp567));_tmp567->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp567->tms=({struct Cyc_List_List*_tmp568=_cycalloc(
sizeof(*_tmp568));_tmp568->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp569=_cycalloc(sizeof(*_tmp569));_tmp569[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp56A;_tmp56A.tag=4;_tmp56A.f1=_tmp565;_tmp56A.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp56A.f3=0;
_tmp56A;});_tmp569;}));_tmp568->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp568;});_tmp567;});
_tmp566;});break;}case 164: _LL2A4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp56B;(_tmp56B.YY29).tag=33;(_tmp56B.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp56C=_cycalloc(sizeof(*_tmp56C));_tmp56C->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp56C->tms=({
struct Cyc_List_List*_tmp56D=_cycalloc(sizeof(*_tmp56D));_tmp56D->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp56E=_cycalloc(sizeof(*_tmp56E));
_tmp56E[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp56F;_tmp56F.tag=5;_tmp56F.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp56F.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp56F;});_tmp56E;}));
_tmp56D->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp56D;});_tmp56C;});_tmp56B;});break;case 165: _LL2A5:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp570;(_tmp570.YY29).tag=
33;(_tmp570.YY29).f1=({struct Cyc_Parse_Declarator*_tmp571=_cycalloc(sizeof(*
_tmp571));_tmp571->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp571->tms=0;_tmp571;});_tmp570;});break;case 166: _LL2A6:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp572;(_tmp572.YY29).tag=
33;(_tmp572.YY29).f1=({struct Cyc_Parse_Declarator*_tmp573=_cycalloc(sizeof(*
_tmp573));_tmp573->id=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]);_tmp573->tms=0;_tmp573;});_tmp572;});break;case 167: _LL2A7:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 168:
_LL2A8: {struct Cyc_Parse_Declarator*_tmp574=Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp574->tms=({struct Cyc_List_List*
_tmp575=_cycalloc(sizeof(*_tmp575));_tmp575->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp576=_cycalloc(sizeof(*_tmp576));_tmp576[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp577;_tmp577.tag=5;_tmp577.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp577.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp577;});_tmp576;}));_tmp575->tl=_tmp574->tms;_tmp575;});yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;}case 169: _LL2A9:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp578;(_tmp578.YY29).tag=
33;(_tmp578.YY29).f1=({struct Cyc_Parse_Declarator*_tmp579=_cycalloc(sizeof(*
_tmp579));_tmp579->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp579->tms=({struct Cyc_List_List*_tmp57A=_cycalloc(
sizeof(*_tmp57A));_tmp57A->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp57C;_tmp57C.tag=0;_tmp57C.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp57C.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp57C;});
_tmp57B;}));_tmp57A->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms;_tmp57A;});_tmp579;});_tmp578;});break;case 170:
_LL2AA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp57D;(_tmp57D.YY29).tag=
33;(_tmp57D.YY29).f1=({struct Cyc_Parse_Declarator*_tmp57E=_cycalloc(sizeof(*
_tmp57E));_tmp57E->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]))->id;_tmp57E->tms=({struct Cyc_List_List*_tmp57F=_cycalloc(
sizeof(*_tmp57F));_tmp57F->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp580=_cycalloc(sizeof(*_tmp580));_tmp580[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp581;_tmp581.tag=1;_tmp581.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp581.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp581.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp581;});
_tmp580;}));_tmp57F->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms;_tmp57F;});_tmp57E;});_tmp57D;});break;case 171:
_LL2AB: {struct _tuple17 _tmp583;struct Cyc_List_List*_tmp584;int _tmp585;struct Cyc_Absyn_VarargInfo*
_tmp586;struct Cyc_Core_Opt*_tmp587;struct Cyc_List_List*_tmp588;struct _tuple17*
_tmp582=Cyc_yyget_YY39(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp583=*_tmp582;_tmp584=_tmp583.f1;_tmp585=_tmp583.f2;_tmp586=_tmp583.f3;
_tmp587=_tmp583.f4;_tmp588=_tmp583.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp589;(_tmp589.YY29).tag=33;(_tmp589.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp58A=_cycalloc(sizeof(*_tmp58A));_tmp58A->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp58A->tms=({
struct Cyc_List_List*_tmp58B=_cycalloc(sizeof(*_tmp58B));_tmp58B->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp58C=_cycalloc(sizeof(*_tmp58C));
_tmp58C[0]=({struct Cyc_Absyn_Function_mod_struct _tmp58D;_tmp58D.tag=3;_tmp58D.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp58E=_cycalloc(sizeof(*
_tmp58E));_tmp58E[0]=({struct Cyc_Absyn_WithTypes_struct _tmp58F;_tmp58F.tag=1;
_tmp58F.f1=_tmp584;_tmp58F.f2=_tmp585;_tmp58F.f3=_tmp586;_tmp58F.f4=_tmp587;
_tmp58F.f5=_tmp588;_tmp58F;});_tmp58E;}));_tmp58D;});_tmp58C;}));_tmp58B->tl=(
Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;
_tmp58B;});_tmp58A;});_tmp589;});break;}case 172: _LL2AC: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp590;(_tmp590.YY29).tag=33;(_tmp590.YY29).f1=({struct
Cyc_Parse_Declarator*_tmp591=_cycalloc(sizeof(*_tmp591));_tmp591->id=(Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->id;_tmp591->tms=({
struct Cyc_List_List*_tmp592=_cycalloc(sizeof(*_tmp592));_tmp592->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp593=_cycalloc(sizeof(*_tmp593));
_tmp593[0]=({struct Cyc_Absyn_Function_mod_struct _tmp594;_tmp594.tag=3;_tmp594.f1=(
void*)((void*)({struct Cyc_Absyn_WithTypes_struct*_tmp595=_cycalloc(sizeof(*
_tmp595));_tmp595[0]=({struct Cyc_Absyn_WithTypes_struct _tmp596;_tmp596.tag=1;
_tmp596.f1=0;_tmp596.f2=0;_tmp596.f3=0;_tmp596.f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp596.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp596;});_tmp595;}));
_tmp594;});_tmp593;}));_tmp592->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp592;});_tmp591;});
_tmp590;});break;case 173: _LL2AD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp597;(_tmp597.YY29).tag=33;(_tmp597.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp598=_cycalloc(sizeof(*_tmp598));_tmp598->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->id;_tmp598->tms=({
struct Cyc_List_List*_tmp599=_cycalloc(sizeof(*_tmp599));_tmp599->hd=(void*)((
void*)({struct Cyc_Absyn_Function_mod_struct*_tmp59A=_cycalloc(sizeof(*_tmp59A));
_tmp59A[0]=({struct Cyc_Absyn_Function_mod_struct _tmp59B;_tmp59B.tag=3;_tmp59B.f1=(
void*)((void*)({struct Cyc_Absyn_NoTypes_struct*_tmp59C=_cycalloc(sizeof(*_tmp59C));
_tmp59C[0]=({struct Cyc_Absyn_NoTypes_struct _tmp59D;_tmp59D.tag=0;_tmp59D.f1=Cyc_yyget_YY36(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp59D.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp59D;});
_tmp59C;}));_tmp59B;});_tmp59A;}));_tmp599->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp599;});_tmp598;});
_tmp597;});break;case 174: _LL2AE: {struct Cyc_List_List*_tmp59E=((struct Cyc_List_List*(*)(
struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*
env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp59F;(_tmp59F.YY29).tag=
33;(_tmp59F.YY29).f1=({struct Cyc_Parse_Declarator*_tmp5A0=_cycalloc(sizeof(*
_tmp5A0));_tmp5A0->id=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->id;_tmp5A0->tms=({struct Cyc_List_List*_tmp5A1=_cycalloc(
sizeof(*_tmp5A1));_tmp5A1->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp5A2=_cycalloc(sizeof(*_tmp5A2));_tmp5A2[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp5A3;_tmp5A3.tag=4;_tmp5A3.f1=_tmp59E;_tmp5A3.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp5A3.f3=0;
_tmp5A3;});_tmp5A2;}));_tmp5A1->tl=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp5A1;});_tmp5A0;});
_tmp59F;});break;}case 175: _LL2AF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5A4;(_tmp5A4.YY29).tag=33;(_tmp5A4.YY29).f1=({struct Cyc_Parse_Declarator*
_tmp5A5=_cycalloc(sizeof(*_tmp5A5));_tmp5A5->id=(Cyc_yyget_YY29(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]))->id;_tmp5A5->tms=({
struct Cyc_List_List*_tmp5A6=_cycalloc(sizeof(*_tmp5A6));_tmp5A6->hd=(void*)((
void*)({struct Cyc_Absyn_Attributes_mod_struct*_tmp5A7=_cycalloc(sizeof(*_tmp5A7));
_tmp5A7[0]=({struct Cyc_Absyn_Attributes_mod_struct _tmp5A8;_tmp5A8.tag=5;_tmp5A8.f1=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp5A8.f2=Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5A8;});_tmp5A7;}));
_tmp5A6->tl=(Cyc_yyget_YY29(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp5A6;});_tmp5A5;});_tmp5A4;});break;case 176: _LL2B0:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 177:
_LL2B1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5A9;(_tmp5A9.YY28).tag=
32;(_tmp5A9.YY28).f1=Cyc_List_imp_append(Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp5A9;});break;case 178:
_LL2B2: {struct Cyc_List_List*ans=0;if(Cyc_yyget_YY45(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])!= 0)ans=({struct Cyc_List_List*
_tmp5AA=_cycalloc(sizeof(*_tmp5AA));_tmp5AA->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp5AB=_cycalloc(sizeof(*_tmp5AB));_tmp5AB[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp5AC;_tmp5AC.tag=5;_tmp5AC.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp5AC.f2=
Cyc_yyget_YY45(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp5AC;});_tmp5AB;}));_tmp5AA->tl=ans;_tmp5AA;});{struct Cyc_Absyn_PtrLoc*ptrloc=
0;if(Cyc_Absyn_porting_c_code)ptrloc=({struct Cyc_Absyn_PtrLoc*_tmp5AD=_cycalloc(
sizeof(*_tmp5AD));_tmp5AD->ptr_loc=(*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f1;_tmp5AD->rgn_loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line);_tmp5AD->zt_loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line);_tmp5AD;});ans=({struct Cyc_List_List*_tmp5AE=
_cycalloc(sizeof(*_tmp5AE));_tmp5AE->hd=(void*)((void*)({struct Cyc_Absyn_Pointer_mod_struct*
_tmp5AF=_cycalloc(sizeof(*_tmp5AF));_tmp5AF[0]=({struct Cyc_Absyn_Pointer_mod_struct
_tmp5B0;_tmp5B0.tag=2;_tmp5B0.f1=({struct Cyc_Absyn_PtrAtts _tmp5B1;_tmp5B1.rgn=(
void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp5B1.nullable=(*Cyc_yyget_YY1(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)])).f2;_tmp5B1.bounds=(*Cyc_yyget_YY1(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])).f3;_tmp5B1.zero_term=
Cyc_yyget_YY51(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp5B1.ptrloc=ptrloc;_tmp5B1;});_tmp5B0.f2=Cyc_yyget_YY25(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5B0;});_tmp5AF;}));
_tmp5AE->tl=ans;_tmp5AE;});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5B2;(_tmp5B2.YY28).tag=32;(_tmp5B2.YY28).f1=ans;_tmp5B2;});break;}}case 179:
_LL2B3: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line): 0;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5B3;(_tmp5B3.YY1).tag=5;(
_tmp5B3.YY1).f1=({struct _tuple12*_tmp5B4=_cycalloc(sizeof(*_tmp5B4));_tmp5B4->f1=
loc;_tmp5B4->f2=Cyc_Absyn_true_conref;_tmp5B4->f3=Cyc_yyget_YY2(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5B4;});_tmp5B3;});
break;}case 180: _LL2B4: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line): 0;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5B5;(_tmp5B5.YY1).tag=5;(_tmp5B5.YY1).f1=({struct _tuple12*_tmp5B6=_cycalloc(
sizeof(*_tmp5B6));_tmp5B6->f1=loc;_tmp5B6->f2=Cyc_Absyn_false_conref;_tmp5B6->f3=
Cyc_yyget_YY2(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5B6;});
_tmp5B5;});break;}case 181: _LL2B5: {struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line): 0;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5B7;(_tmp5B7.YY1).tag=5;(_tmp5B7.YY1).f1=({struct _tuple12*_tmp5B8=_cycalloc(
sizeof(*_tmp5B8));_tmp5B8->f1=loc;_tmp5B8->f2=Cyc_Absyn_true_conref;_tmp5B8->f3=
Cyc_Absyn_bounds_dynforward_conref;_tmp5B8;});_tmp5B7;});break;}case 182: _LL2B6: {
struct Cyc_Position_Segment*loc=Cyc_Absyn_porting_c_code?Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line): 0;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5B9;(_tmp5B9.YY1).tag=5;(
_tmp5B9.YY1).f1=({struct _tuple12*_tmp5BA=_cycalloc(sizeof(*_tmp5BA));_tmp5BA->f1=
loc;_tmp5BA->f2=Cyc_Absyn_true_conref;_tmp5BA->f3=Cyc_Absyn_bounds_dyneither_conref;
_tmp5BA;});_tmp5B9;});break;}case 183: _LL2B7: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp5BB;(_tmp5BB.YY2).tag=6;(_tmp5BB.YY2).f1=Cyc_Absyn_bounds_one_conref;
_tmp5BB;});break;case 184: _LL2B8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5BC;(_tmp5BC.YY2).tag=6;(_tmp5BC.YY2).f1=Cyc_Absyn_new_conref((void*)({
struct Cyc_Absyn_Upper_b_struct*_tmp5BD=_cycalloc(sizeof(*_tmp5BD));_tmp5BD[0]=({
struct Cyc_Absyn_Upper_b_struct _tmp5BE;_tmp5BE.tag=0;_tmp5BE.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5BE;});_tmp5BD;}));
_tmp5BC;});break;case 185: _LL2B9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5BF;(_tmp5BF.YY2).tag=6;(_tmp5BF.YY2).f1=Cyc_Absyn_new_conref((void*)({
struct Cyc_Absyn_AbsUpper_b_struct*_tmp5C0=_cycalloc(sizeof(*_tmp5C0));_tmp5C0[0]=({
struct Cyc_Absyn_AbsUpper_b_struct _tmp5C1;_tmp5C1.tag=1;_tmp5C1.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5C1;});_tmp5C0;}));
_tmp5BF;});break;case 186: _LL2BA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5C2;(_tmp5C2.YY51).tag=55;(_tmp5C2.YY51).f1=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmp5C2;});break;case 187: _LL2BB: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp5C3;(_tmp5C3.YY51).tag=55;(_tmp5C3.YY51).f1=Cyc_Absyn_true_conref;
_tmp5C3;});break;case 188: _LL2BC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5C4;(_tmp5C4.YY51).tag=55;(_tmp5C4.YY51).f1=Cyc_Absyn_false_conref;_tmp5C4;});
break;case 189: _LL2BD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5C5;(_tmp5C5.YY49).tag=53;(_tmp5C5.YY49).f1=0;_tmp5C5;});break;case 190:
_LL2BE: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)3,1);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp5C6;(_tmp5C6.YY49).tag=53;(_tmp5C6.YY49).f1=({struct
Cyc_Core_Opt*_tmp5C7=_cycalloc(sizeof(*_tmp5C7));_tmp5C7->v=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5C7;});_tmp5C6;});
break;case 191: _LL2BF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5C8;(_tmp5C8.YY49).tag=53;(_tmp5C8.YY49).f1=({struct Cyc_Core_Opt*_tmp5C9=
_cycalloc(sizeof(*_tmp5C9));_tmp5C9->v=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,0);_tmp5C9;});_tmp5C8;});break;case 192: _LL2C0: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp5CA;(_tmp5CA.YY44).tag=48;(_tmp5CA.YY44).f1=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,0);_tmp5CA;});break;case 193: _LL2C1: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)
5,1);yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
194: _LL2C2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5CB;(
_tmp5CB.YY44).tag=48;(_tmp5CB.YY44).f1=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_trk,0);_tmp5CB;});break;case 195: _LL2C3: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp5CC;(_tmp5CC.YY25).tag=29;(_tmp5CC.YY25).f1=Cyc_Absyn_empty_tqual(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset + 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset + 1)]).last_line));_tmp5CC;});break;case 196: _LL2C4: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp5CD;(_tmp5CD.YY25).tag=29;(_tmp5CD.YY25).f1=Cyc_Absyn_combine_tqual(
Cyc_yyget_YY25(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY25(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp5CD;});break;case
197: _LL2C5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5CE;(
_tmp5CE.YY39).tag=43;(_tmp5CE.YY39).f1=({struct _tuple17*_tmp5CF=_cycalloc(
sizeof(*_tmp5CF));_tmp5CF->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp5CF->f2=0;_tmp5CF->f3=0;_tmp5CF->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5CF->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5CF;});_tmp5CE;});
break;case 198: _LL2C6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5D0;(_tmp5D0.YY39).tag=43;(_tmp5D0.YY39).f1=({struct _tuple17*_tmp5D1=
_cycalloc(sizeof(*_tmp5D1));_tmp5D1->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 4)]));_tmp5D1->f2=1;_tmp5D1->f3=0;_tmp5D1->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5D1->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5D1;});_tmp5D0;});
break;case 199: _LL2C7: {struct _tuple2 _tmp5D3;struct Cyc_Core_Opt*_tmp5D4;struct Cyc_Absyn_Tqual
_tmp5D5;void*_tmp5D6;struct _tuple2*_tmp5D2=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5D3=*_tmp5D2;_tmp5D4=
_tmp5D3.f1;_tmp5D5=_tmp5D3.f2;_tmp5D6=_tmp5D3.f3;{struct Cyc_Absyn_VarargInfo*
_tmp5D7=({struct Cyc_Absyn_VarargInfo*_tmp5DA=_cycalloc(sizeof(*_tmp5DA));_tmp5DA->name=
_tmp5D4;_tmp5DA->tq=_tmp5D5;_tmp5DA->type=(void*)_tmp5D6;_tmp5DA->inject=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5DA;});yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5D8;(_tmp5D8.YY39).tag=43;(
_tmp5D8.YY39).f1=({struct _tuple17*_tmp5D9=_cycalloc(sizeof(*_tmp5D9));_tmp5D9->f1=
0;_tmp5D9->f2=0;_tmp5D9->f3=_tmp5D7;_tmp5D9->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5D9->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5D9;});_tmp5D8;});
break;}}case 200: _LL2C8: {struct _tuple2 _tmp5DC;struct Cyc_Core_Opt*_tmp5DD;struct
Cyc_Absyn_Tqual _tmp5DE;void*_tmp5DF;struct _tuple2*_tmp5DB=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5DC=*_tmp5DB;_tmp5DD=
_tmp5DC.f1;_tmp5DE=_tmp5DC.f2;_tmp5DF=_tmp5DC.f3;{struct Cyc_Absyn_VarargInfo*
_tmp5E0=({struct Cyc_Absyn_VarargInfo*_tmp5E3=_cycalloc(sizeof(*_tmp5E3));_tmp5E3->name=
_tmp5DD;_tmp5E3->tq=_tmp5DE;_tmp5E3->type=(void*)_tmp5DF;_tmp5E3->inject=Cyc_yyget_YY31(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp5E3;});yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5E1;(_tmp5E1.YY39).tag=43;(
_tmp5E1.YY39).f1=({struct _tuple17*_tmp5E2=_cycalloc(sizeof(*_tmp5E2));_tmp5E2->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]));_tmp5E2->f2=0;
_tmp5E2->f3=_tmp5E0;_tmp5E2->f4=Cyc_yyget_YY49(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp5E2->f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5E2;});_tmp5E1;});
break;}}case 201: _LL2C9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5E4;(_tmp5E4.YY44).tag=48;(_tmp5E4.YY44).f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Unknown_kb_struct*
_tmp5E5=_cycalloc(sizeof(*_tmp5E5));_tmp5E5[0]=({struct Cyc_Absyn_Unknown_kb_struct
_tmp5E6;_tmp5E6.tag=1;_tmp5E6.f1=0;_tmp5E6;});_tmp5E5;}));_tmp5E4;});break;case
202: _LL2CA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5E7;(
_tmp5E7.YY44).tag=48;(_tmp5E7.YY44).f1=(void*)Cyc_Parse_id2type(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Tcutil_kind_to_bound(
Cyc_yyget_YY43(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])));
_tmp5E7;});break;case 203: _LL2CB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5E8;(_tmp5E8.YY49).tag=53;(_tmp5E8.YY49).f1=0;_tmp5E8;});break;case 204:
_LL2CC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5E9;(_tmp5E9.YY49).tag=
53;(_tmp5E9.YY49).f1=({struct Cyc_Core_Opt*_tmp5EA=_cycalloc(sizeof(*_tmp5EA));
_tmp5EA->v=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*_tmp5EB=_cycalloc(
sizeof(*_tmp5EB));_tmp5EB[0]=({struct Cyc_Absyn_JoinEff_struct _tmp5EC;_tmp5EC.tag=
21;_tmp5EC.f1=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp5EC;});_tmp5EB;}));_tmp5EA;});_tmp5E9;});break;case 205:
_LL2CD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5ED;(_tmp5ED.YY50).tag=
54;(_tmp5ED.YY50).f1=0;_tmp5ED;});break;case 206: _LL2CE: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 207: _LL2CF: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5EE;(_tmp5EE.YY50).tag=54;(
_tmp5EE.YY50).f1=({struct Cyc_List_List*_tmp5EF=_cycalloc(sizeof(*_tmp5EF));
_tmp5EF->hd=({struct _tuple4*_tmp5F0=_cycalloc(sizeof(*_tmp5F0));_tmp5F0->f1=(
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp5F3=_cycalloc(sizeof(*_tmp5F3));
_tmp5F3[0]=({struct Cyc_Absyn_JoinEff_struct _tmp5F4;_tmp5F4.tag=21;_tmp5F4.f1=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp5F4;});_tmp5F3;});
_tmp5F0->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp5F1=_cycalloc(sizeof(*_tmp5F1));_tmp5F1[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp5F2;_tmp5F2.tag=2;_tmp5F2.f1=0;_tmp5F2.f2=(void*)((void*)5);_tmp5F2;});
_tmp5F1;}));_tmp5F0;});_tmp5EF->tl=0;_tmp5EF;});_tmp5EE;});break;case 208: _LL2D0:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp5F5;(_tmp5F5.YY50).tag=
54;(_tmp5F5.YY50).f1=({struct Cyc_List_List*_tmp5F6=_cycalloc(sizeof(*_tmp5F6));
_tmp5F6->hd=({struct _tuple4*_tmp5F7=_cycalloc(sizeof(*_tmp5F7));_tmp5F7->f1=(
void*)({struct Cyc_Absyn_JoinEff_struct*_tmp5FA=_cycalloc(sizeof(*_tmp5FA));
_tmp5FA[0]=({struct Cyc_Absyn_JoinEff_struct _tmp5FB;_tmp5FB.tag=21;_tmp5FB.f1=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp5FB;});_tmp5FA;});
_tmp5F7->f2=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(void*)({struct Cyc_Absyn_Less_kb_struct*
_tmp5F8=_cycalloc(sizeof(*_tmp5F8));_tmp5F8[0]=({struct Cyc_Absyn_Less_kb_struct
_tmp5F9;_tmp5F9.tag=2;_tmp5F9.f1=0;_tmp5F9.f2=(void*)((void*)5);_tmp5F9;});
_tmp5F8;}));_tmp5F7;});_tmp5F6->tl=Cyc_yyget_YY50(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp5F6;});_tmp5F5;});
break;case 209: _LL2D1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp5FC;(_tmp5FC.YY31).tag=35;(_tmp5FC.YY31).f1=0;_tmp5FC;});break;case 210:
_LL2D2: if(Cyc_zstrcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),({const char*_tmp5FD="inject";
_tag_dynforward(_tmp5FD,sizeof(char),_get_zero_arr_size_char(_tmp5FD,7));}))!= 0)
Cyc_Parse_err(({const char*_tmp5FE="missing type in function declaration";
_tag_dynforward(_tmp5FE,sizeof(char),_get_zero_arr_size_char(_tmp5FE,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp5FF;(_tmp5FF.YY31).tag=35;(_tmp5FF.YY31).f1=1;_tmp5FF;});
break;case 211: _LL2D3: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 212: _LL2D4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp600;(_tmp600.YY40).tag=44;(_tmp600.YY40).f1=Cyc_List_imp_append(Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp600;});break;case 213:
_LL2D5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp601;(_tmp601.YY40).tag=
44;(_tmp601.YY40).f1=0;_tmp601;});break;case 214: _LL2D6: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)];break;case 215: _LL2D7:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp602;(_tmp602.YY40).tag=
44;(_tmp602.YY40).f1=({struct Cyc_List_List*_tmp603=_cycalloc(sizeof(*_tmp603));
_tmp603->hd=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*_tmp604=_cycalloc(
sizeof(*_tmp604));_tmp604[0]=({struct Cyc_Absyn_RgnsEff_struct _tmp605;_tmp605.tag=
22;_tmp605.f1=(void*)Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp605;});_tmp604;}));_tmp603->tl=0;_tmp603;});_tmp602;});
break;case 216: _LL2D8: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)6,0);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp606;(_tmp606.YY40).tag=44;(_tmp606.YY40).f1=({struct
Cyc_List_List*_tmp607=_cycalloc(sizeof(*_tmp607));_tmp607->hd=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp607->tl=0;_tmp607;});
_tmp606;});break;case 217: _LL2D9: Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),(void*)5,1);yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp608;(_tmp608.YY40).tag=44;(_tmp608.YY40).f1=({struct
Cyc_List_List*_tmp609=_cycalloc(sizeof(*_tmp609));_tmp609->hd=(void*)((void*)({
struct Cyc_Absyn_AccessEff_struct*_tmp60A=_cycalloc(sizeof(*_tmp60A));_tmp60A[0]=({
struct Cyc_Absyn_AccessEff_struct _tmp60B;_tmp60B.tag=20;_tmp60B.f1=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp60B;});_tmp60A;}));
_tmp609->tl=0;_tmp609;});_tmp608;});break;case 218: _LL2DA: Cyc_Parse_set_vartyp_kind(
Cyc_yyget_YY44(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),(
void*)5,1);yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp60C;(
_tmp60C.YY40).tag=44;(_tmp60C.YY40).f1=({struct Cyc_List_List*_tmp60D=_cycalloc(
sizeof(*_tmp60D));_tmp60D->hd=(void*)((void*)({struct Cyc_Absyn_AccessEff_struct*
_tmp60E=_cycalloc(sizeof(*_tmp60E));_tmp60E[0]=({struct Cyc_Absyn_AccessEff_struct
_tmp60F;_tmp60F.tag=20;_tmp60F.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp60F;});_tmp60E;}));
_tmp60D->tl=Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp60D;});_tmp60C;});break;case 219: _LL2DB: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp610;(_tmp610.YY38).tag=42;(_tmp610.YY38).f1=({struct
Cyc_List_List*_tmp611=_cycalloc(sizeof(*_tmp611));_tmp611->hd=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp611->tl=0;_tmp611;});
_tmp610;});break;case 220: _LL2DC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp612;(_tmp612.YY38).tag=42;(_tmp612.YY38).f1=({struct Cyc_List_List*_tmp613=
_cycalloc(sizeof(*_tmp613));_tmp613->hd=Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp613->tl=Cyc_yyget_YY38(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp613;});_tmp612;});
break;case 221: _LL2DD: {struct _tuple16 _tmp615;struct Cyc_Absyn_Tqual _tmp616;struct
Cyc_List_List*_tmp617;struct Cyc_List_List*_tmp618;struct _tuple16*_tmp614=Cyc_yyget_YY35(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp615=*_tmp614;
_tmp616=_tmp615.f1;_tmp617=_tmp615.f2;_tmp618=_tmp615.f3;if(_tmp616.loc == 0)
_tmp616.loc=Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line);{struct Cyc_Parse_Declarator _tmp61A;struct _tuple1*
_tmp61B;struct Cyc_List_List*_tmp61C;struct Cyc_Parse_Declarator*_tmp619=Cyc_yyget_YY29(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp61A=*_tmp619;
_tmp61B=_tmp61A.id;_tmp61C=_tmp61A.tms;{void*_tmp61D=Cyc_Parse_speclist2typ(
_tmp617,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_Absyn_Tqual _tmp61F;void*_tmp620;struct
Cyc_List_List*_tmp621;struct Cyc_List_List*_tmp622;struct _tuple6 _tmp61E=Cyc_Parse_apply_tms(
_tmp616,_tmp61D,_tmp618,_tmp61C);_tmp61F=_tmp61E.f1;_tmp620=_tmp61E.f2;_tmp621=
_tmp61E.f3;_tmp622=_tmp61E.f4;if(_tmp621 != 0)Cyc_Parse_err(({const char*_tmp623="parameter with bad type params";
_tag_dynforward(_tmp623,sizeof(char),_get_zero_arr_size_char(_tmp623,31));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(Cyc_Absyn_is_qvar_qualified(
_tmp61B))Cyc_Parse_err(({const char*_tmp624="parameter cannot be qualified with a namespace";
_tag_dynforward(_tmp624,sizeof(char),_get_zero_arr_size_char(_tmp624,47));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Core_Opt*
_tmp625=(struct Cyc_Core_Opt*)({struct Cyc_Core_Opt*_tmp62A=_cycalloc(sizeof(*
_tmp62A));_tmp62A->v=(*_tmp61B).f2;_tmp62A;});if(_tmp622 != 0)({void*_tmp626=0;
Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line),({const char*_tmp627="extra attributes on parameter, ignoring";
_tag_dynforward(_tmp627,sizeof(char),_get_zero_arr_size_char(_tmp627,40));}),
_tag_dynforward(_tmp626,sizeof(void*),0));});yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp628;(_tmp628.YY37).tag=41;(_tmp628.YY37).f1=({struct
_tuple2*_tmp629=_cycalloc(sizeof(*_tmp629));_tmp629->f1=_tmp625;_tmp629->f2=
_tmp61F;_tmp629->f3=_tmp620;_tmp629;});_tmp628;});break;}}}}case 222: _LL2DE: {
struct _tuple16 _tmp62C;struct Cyc_Absyn_Tqual _tmp62D;struct Cyc_List_List*_tmp62E;
struct Cyc_List_List*_tmp62F;struct _tuple16*_tmp62B=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp62C=*_tmp62B;_tmp62D=
_tmp62C.f1;_tmp62E=_tmp62C.f2;_tmp62F=_tmp62C.f3;if(_tmp62D.loc == 0)_tmp62D.loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);{void*_tmp630=Cyc_Parse_speclist2typ(_tmp62E,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp62F != 0)({
void*_tmp631=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp632="bad attributes on parameter, ignoring";_tag_dynforward(_tmp632,sizeof(
char),_get_zero_arr_size_char(_tmp632,38));}),_tag_dynforward(_tmp631,sizeof(
void*),0));});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp633;(
_tmp633.YY37).tag=41;(_tmp633.YY37).f1=({struct _tuple2*_tmp634=_cycalloc(sizeof(*
_tmp634));_tmp634->f1=0;_tmp634->f2=_tmp62D;_tmp634->f3=_tmp630;_tmp634;});
_tmp633;});break;}}case 223: _LL2DF: {struct _tuple16 _tmp636;struct Cyc_Absyn_Tqual
_tmp637;struct Cyc_List_List*_tmp638;struct Cyc_List_List*_tmp639;struct _tuple16*
_tmp635=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp636=*_tmp635;_tmp637=_tmp636.f1;_tmp638=_tmp636.f2;_tmp639=_tmp636.f3;if(
_tmp637.loc == 0)_tmp637.loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);{void*_tmp63A=
Cyc_Parse_speclist2typ(_tmp638,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));struct Cyc_List_List*
_tmp63B=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;
struct Cyc_Absyn_Tqual _tmp63D;void*_tmp63E;struct Cyc_List_List*_tmp63F;struct Cyc_List_List*
_tmp640;struct _tuple6 _tmp63C=Cyc_Parse_apply_tms(_tmp637,_tmp63A,_tmp639,_tmp63B);
_tmp63D=_tmp63C.f1;_tmp63E=_tmp63C.f2;_tmp63F=_tmp63C.f3;_tmp640=_tmp63C.f4;if(
_tmp63F != 0)({void*_tmp641=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp642="bad type parameters on formal argument, ignoring";_tag_dynforward(
_tmp642,sizeof(char),_get_zero_arr_size_char(_tmp642,49));}),_tag_dynforward(
_tmp641,sizeof(void*),0));});if(_tmp640 != 0)({void*_tmp643=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp644="bad attributes on parameter, ignoring";_tag_dynforward(_tmp644,sizeof(
char),_get_zero_arr_size_char(_tmp644,38));}),_tag_dynforward(_tmp643,sizeof(
void*),0));});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp645;(
_tmp645.YY37).tag=41;(_tmp645.YY37).f1=({struct _tuple2*_tmp646=_cycalloc(sizeof(*
_tmp646));_tmp646->f1=0;_tmp646->f2=_tmp63D;_tmp646->f3=_tmp63E;_tmp646;});
_tmp645;});break;}}case 224: _LL2E0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp647;(_tmp647.YY36).tag=40;(_tmp647.YY36).f1=((struct Cyc_List_List*(*)(struct
Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp647;});break;case 225:
_LL2E1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp648;(_tmp648.YY36).tag=
40;(_tmp648.YY36).f1=({struct Cyc_List_List*_tmp649=_cycalloc(sizeof(*_tmp649));
_tmp649->hd=({struct _dynforward_ptr*_tmp64A=_cycalloc(sizeof(*_tmp64A));_tmp64A[
0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp64A;});_tmp649->tl=0;_tmp649;});_tmp648;});break;case 226: _LL2E2: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp64B;(_tmp64B.YY36).tag=40;(_tmp64B.YY36).f1=({
struct Cyc_List_List*_tmp64C=_cycalloc(sizeof(*_tmp64C));_tmp64C->hd=({struct
_dynforward_ptr*_tmp64D=_cycalloc(sizeof(*_tmp64D));_tmp64D[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp64D;});_tmp64C->tl=
Cyc_yyget_YY36(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp64C;});_tmp64B;});break;case 227: _LL2E3: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 228: _LL2E4: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 229: _LL2E5:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp64E;(_tmp64E.YY3).tag=
7;(_tmp64E.YY3).f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp64F=_cycalloc(sizeof(*_tmp64F));_tmp64F[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp650;_tmp650.tag=37;_tmp650.f1=0;_tmp650.f2=0;_tmp650;});_tmp64F;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp64E;});
break;case 230: _LL2E6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp651;(_tmp651.YY3).tag=7;(_tmp651.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnresolvedMem_e_struct*_tmp652=_cycalloc(sizeof(*_tmp652));_tmp652[0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp653;_tmp653.tag=37;_tmp653.f1=0;
_tmp653.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));
_tmp653;});_tmp652;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp651;});
break;case 231: _LL2E7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp654;(_tmp654.YY3).tag=7;(_tmp654.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnresolvedMem_e_struct*_tmp655=_cycalloc(sizeof(*_tmp655));_tmp655[0]=({
struct Cyc_Absyn_UnresolvedMem_e_struct _tmp656;_tmp656.tag=37;_tmp656.f1=0;
_tmp656.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp656;});_tmp655;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp654;});
break;case 232: _LL2E8: {struct Cyc_Absyn_Vardecl*_tmp657=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp65B=_cycalloc(sizeof(*_tmp65B));_tmp65B->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp65D;(_tmp65D.Loc_n).tag=0;_tmp65D;});_tmp65B->f2=({
struct _dynforward_ptr*_tmp65C=_cycalloc(sizeof(*_tmp65C));_tmp65C[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp65C;});_tmp65B;}),
Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).last_line)));(_tmp657->tq).real_const=
1;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp658;(_tmp658.YY3).tag=
7;(_tmp658.YY3).f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_struct*
_tmp659=_cycalloc(sizeof(*_tmp659));_tmp659[0]=({struct Cyc_Absyn_Comprehension_e_struct
_tmp65A;_tmp65A.tag=29;_tmp65A.f1=_tmp657;_tmp65A.f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp65A.f3=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp65A.f4=0;
_tmp65A;});_tmp659;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp658;});
break;}case 233: _LL2E9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp65E;(_tmp65E.YY6).tag=10;(_tmp65E.YY6).f1=({struct Cyc_List_List*_tmp65F=
_cycalloc(sizeof(*_tmp65F));_tmp65F->hd=({struct _tuple20*_tmp660=_cycalloc(
sizeof(*_tmp660));_tmp660->f1=0;_tmp660->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp660;});_tmp65F->tl=0;
_tmp65F;});_tmp65E;});break;case 234: _LL2EA: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp661;(_tmp661.YY6).tag=10;(_tmp661.YY6).f1=({struct Cyc_List_List*
_tmp662=_cycalloc(sizeof(*_tmp662));_tmp662->hd=({struct _tuple20*_tmp663=
_cycalloc(sizeof(*_tmp663));_tmp663->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp663->f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp663;});_tmp662->tl=
0;_tmp662;});_tmp661;});break;case 235: _LL2EB: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp664;(_tmp664.YY6).tag=10;(_tmp664.YY6).f1=({struct Cyc_List_List*
_tmp665=_cycalloc(sizeof(*_tmp665));_tmp665->hd=({struct _tuple20*_tmp666=
_cycalloc(sizeof(*_tmp666));_tmp666->f1=0;_tmp666->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp666;});_tmp665->tl=Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp665;});_tmp664;});
break;case 236: _LL2EC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp667;(_tmp667.YY6).tag=10;(_tmp667.YY6).f1=({struct Cyc_List_List*_tmp668=
_cycalloc(sizeof(*_tmp668));_tmp668->hd=({struct _tuple20*_tmp669=_cycalloc(
sizeof(*_tmp669));_tmp669->f1=Cyc_yyget_YY41(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]);_tmp669->f2=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp669;});_tmp668->tl=Cyc_yyget_YY6(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp668;});_tmp667;});
break;case 237: _LL2ED: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp66A;(_tmp66A.YY41).tag=45;(_tmp66A.YY41).f1=Cyc_List_imp_rev(Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp66A;});break;
case 238: _LL2EE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp66B;(
_tmp66B.YY41).tag=45;(_tmp66B.YY41).f1=({struct Cyc_List_List*_tmp66C=_cycalloc(
sizeof(*_tmp66C));_tmp66C->hd=(void*)Cyc_yyget_YY42(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp66C->tl=0;_tmp66C;});
_tmp66B;});break;case 239: _LL2EF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp66D;(_tmp66D.YY41).tag=45;(_tmp66D.YY41).f1=({struct Cyc_List_List*_tmp66E=
_cycalloc(sizeof(*_tmp66E));_tmp66E->hd=(void*)Cyc_yyget_YY42(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp66E->tl=Cyc_yyget_YY41(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp66E;});_tmp66D;});
break;case 240: _LL2F0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp66F;(_tmp66F.YY42).tag=46;(_tmp66F.YY42).f1=(void*)((void*)({struct Cyc_Absyn_ArrayElement_struct*
_tmp670=_cycalloc(sizeof(*_tmp670));_tmp670[0]=({struct Cyc_Absyn_ArrayElement_struct
_tmp671;_tmp671.tag=0;_tmp671.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp671;});_tmp670;}));
_tmp66F;});break;case 241: _LL2F1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp672;(_tmp672.YY42).tag=46;(_tmp672.YY42).f1=(void*)((void*)({struct Cyc_Absyn_FieldName_struct*
_tmp673=_cycalloc(sizeof(*_tmp673));_tmp673[0]=({struct Cyc_Absyn_FieldName_struct
_tmp674;_tmp674.tag=1;_tmp674.f1=({struct _dynforward_ptr*_tmp675=_cycalloc(
sizeof(*_tmp675));_tmp675[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp675;});_tmp674;});
_tmp673;}));_tmp672;});break;case 242: _LL2F2: {struct _tuple16 _tmp677;struct Cyc_Absyn_Tqual
_tmp678;struct Cyc_List_List*_tmp679;struct Cyc_List_List*_tmp67A;struct _tuple16*
_tmp676=Cyc_yyget_YY35(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);
_tmp677=*_tmp676;_tmp678=_tmp677.f1;_tmp679=_tmp677.f2;_tmp67A=_tmp677.f3;{void*
_tmp67B=Cyc_Parse_speclist2typ(_tmp679,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));if(_tmp67A != 0)({
void*_tmp67C=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp67D="ignoring attributes in type";_tag_dynforward(_tmp67D,sizeof(char),
_get_zero_arr_size_char(_tmp67D,28));}),_tag_dynforward(_tmp67C,sizeof(void*),0));});
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp67E;(_tmp67E.YY37).tag=
41;(_tmp67E.YY37).f1=({struct _tuple2*_tmp67F=_cycalloc(sizeof(*_tmp67F));_tmp67F->f1=
0;_tmp67F->f2=_tmp678;_tmp67F->f3=_tmp67B;_tmp67F;});_tmp67E;});break;}}case 243:
_LL2F3: {struct _tuple16 _tmp681;struct Cyc_Absyn_Tqual _tmp682;struct Cyc_List_List*
_tmp683;struct Cyc_List_List*_tmp684;struct _tuple16*_tmp680=Cyc_yyget_YY35(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp681=*_tmp680;_tmp682=
_tmp681.f1;_tmp683=_tmp681.f2;_tmp684=_tmp681.f3;{void*_tmp685=Cyc_Parse_speclist2typ(
_tmp683,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));struct Cyc_List_List*_tmp686=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms;struct _tuple6 _tmp687=
Cyc_Parse_apply_tms(_tmp682,_tmp685,_tmp684,_tmp686);if(_tmp687.f3 != 0)({void*
_tmp688=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp689="bad type params, ignoring";_tag_dynforward(_tmp689,sizeof(char),
_get_zero_arr_size_char(_tmp689,26));}),_tag_dynforward(_tmp688,sizeof(void*),0));});
if(_tmp687.f4 != 0)({void*_tmp68A=0;Cyc_Tcutil_warn(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),({const char*
_tmp68B="bad specifiers, ignoring";_tag_dynforward(_tmp68B,sizeof(char),
_get_zero_arr_size_char(_tmp68B,25));}),_tag_dynforward(_tmp68A,sizeof(void*),0));});
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp68C;(_tmp68C.YY37).tag=
41;(_tmp68C.YY37).f1=({struct _tuple2*_tmp68D=_cycalloc(sizeof(*_tmp68D));_tmp68D->f1=
0;_tmp68D->f2=_tmp687.f1;_tmp68D->f3=_tmp687.f2;_tmp68D;});_tmp68C;});break;}}
case 244: _LL2F4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp68E;(
_tmp68E.YY44).tag=48;(_tmp68E.YY44).f1=(void*)(*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)])).f3;_tmp68E;});break;case
245: _LL2F5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp68F;(
_tmp68F.YY44).tag=48;(_tmp68F.YY44).f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp690=_cycalloc(sizeof(*_tmp690));_tmp690[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp691;_tmp691.tag=21;_tmp691.f1=0;_tmp691;});_tmp690;}));_tmp68F;});break;case
246: _LL2F6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp692;(
_tmp692.YY44).tag=48;(_tmp692.YY44).f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp693=_cycalloc(sizeof(*_tmp693));_tmp693[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp694;_tmp694.tag=21;_tmp694.f1=Cyc_yyget_YY40(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp694;});_tmp693;}));
_tmp692;});break;case 247: _LL2F7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp695;(_tmp695.YY44).tag=48;(_tmp695.YY44).f1=(void*)((void*)({struct Cyc_Absyn_RgnsEff_struct*
_tmp696=_cycalloc(sizeof(*_tmp696));_tmp696[0]=({struct Cyc_Absyn_RgnsEff_struct
_tmp697;_tmp697.tag=22;_tmp697.f1=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp697;});_tmp696;}));
_tmp695;});break;case 248: _LL2F8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp698;(_tmp698.YY44).tag=48;(_tmp698.YY44).f1=(void*)((void*)({struct Cyc_Absyn_JoinEff_struct*
_tmp699=_cycalloc(sizeof(*_tmp699));_tmp699[0]=({struct Cyc_Absyn_JoinEff_struct
_tmp69A;_tmp69A.tag=21;_tmp69A.f1=({struct Cyc_List_List*_tmp69B=_cycalloc(
sizeof(*_tmp69B));_tmp69B->hd=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp69B->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp69B;});_tmp69A;});
_tmp699;}));_tmp698;});break;case 249: _LL2F9: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp69C;(_tmp69C.YY40).tag=44;(_tmp69C.YY40).f1=({struct
Cyc_List_List*_tmp69D=_cycalloc(sizeof(*_tmp69D));_tmp69D->hd=(void*)Cyc_yyget_YY44(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp69D->tl=0;_tmp69D;});
_tmp69C;});break;case 250: _LL2FA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp69E;(_tmp69E.YY40).tag=44;(_tmp69E.YY40).f1=({struct Cyc_List_List*_tmp69F=
_cycalloc(sizeof(*_tmp69F));_tmp69F->hd=(void*)Cyc_yyget_YY44(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp69F->tl=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp69F;});_tmp69E;});
break;case 251: _LL2FB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6A0;(_tmp6A0.YY30).tag=34;(_tmp6A0.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6A1=_cycalloc(sizeof(*_tmp6A1));_tmp6A1->tms=Cyc_yyget_YY28(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6A1;});_tmp6A0;});
break;case 252: _LL2FC: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 253: _LL2FD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6A2;(_tmp6A2.YY30).tag=34;(_tmp6A2.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6A3=_cycalloc(sizeof(*_tmp6A3));_tmp6A3->tms=Cyc_List_imp_append(Cyc_yyget_YY28(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),(Cyc_yyget_YY30(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]))->tms);_tmp6A3;});
_tmp6A2;});break;case 254: _LL2FE: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 255: _LL2FF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6A4;(_tmp6A4.YY30).tag=34;(_tmp6A4.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6A5=_cycalloc(sizeof(*_tmp6A5));_tmp6A5->tms=({struct Cyc_List_List*_tmp6A6=
_cycalloc(sizeof(*_tmp6A6));_tmp6A6->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*
_tmp6A7=_cycalloc(sizeof(*_tmp6A7));_tmp6A7[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp6A8;_tmp6A8.tag=0;_tmp6A8.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6A8.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6A8;});
_tmp6A7;}));_tmp6A6->tl=0;_tmp6A6;});_tmp6A5;});_tmp6A4;});break;case 256: _LL300:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6A9;(_tmp6A9.YY30).tag=
34;(_tmp6A9.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6AA=_cycalloc(
sizeof(*_tmp6AA));_tmp6AA->tms=({struct Cyc_List_List*_tmp6AB=_cycalloc(sizeof(*
_tmp6AB));_tmp6AB->hd=(void*)((void*)({struct Cyc_Absyn_Carray_mod_struct*_tmp6AC=
_cycalloc(sizeof(*_tmp6AC));_tmp6AC[0]=({struct Cyc_Absyn_Carray_mod_struct
_tmp6AD;_tmp6AD.tag=0;_tmp6AD.f1=Cyc_yyget_YY51(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6AD.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6AD;});
_tmp6AC;}));_tmp6AB->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]))->tms;_tmp6AB;});_tmp6AA;});_tmp6A9;});break;case 257:
_LL301: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6AE;(_tmp6AE.YY30).tag=
34;(_tmp6AE.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6AF=_cycalloc(
sizeof(*_tmp6AF));_tmp6AF->tms=({struct Cyc_List_List*_tmp6B0=_cycalloc(sizeof(*
_tmp6B0));_tmp6B0->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp6B1=_cycalloc(sizeof(*_tmp6B1));_tmp6B1[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp6B2;_tmp6B2.tag=1;_tmp6B2.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6B2.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B2.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6B2;});
_tmp6B1;}));_tmp6B0->tl=0;_tmp6B0;});_tmp6AF;});_tmp6AE;});break;case 258: _LL302:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6B3;(_tmp6B3.YY30).tag=
34;(_tmp6B3.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6B4=_cycalloc(
sizeof(*_tmp6B4));_tmp6B4->tms=({struct Cyc_List_List*_tmp6B5=_cycalloc(sizeof(*
_tmp6B5));_tmp6B5->hd=(void*)((void*)({struct Cyc_Absyn_ConstArray_mod_struct*
_tmp6B6=_cycalloc(sizeof(*_tmp6B6));_tmp6B6[0]=({struct Cyc_Absyn_ConstArray_mod_struct
_tmp6B7;_tmp6B7.tag=1;_tmp6B7.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6B7.f2=Cyc_yyget_YY51(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6B7.f3=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6B7;});
_tmp6B6;}));_tmp6B5->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]))->tms;_tmp6B5;});_tmp6B4;});_tmp6B3;});break;case 259:
_LL303: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6B8;(_tmp6B8.YY30).tag=
34;(_tmp6B8.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6B9=_cycalloc(
sizeof(*_tmp6B9));_tmp6B9->tms=({struct Cyc_List_List*_tmp6BA=_cycalloc(sizeof(*
_tmp6BA));_tmp6BA->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6BB=_cycalloc(sizeof(*_tmp6BB));_tmp6BB[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6BC;_tmp6BC.tag=3;_tmp6BC.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6BD=_cycalloc(sizeof(*_tmp6BD));_tmp6BD[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6BE;_tmp6BE.tag=1;_tmp6BE.f1=0;_tmp6BE.f2=0;_tmp6BE.f3=0;_tmp6BE.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6BE.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6BE;});_tmp6BD;}));
_tmp6BC;});_tmp6BB;}));_tmp6BA->tl=0;_tmp6BA;});_tmp6B9;});_tmp6B8;});break;case
260: _LL304: {struct _tuple17 _tmp6C0;struct Cyc_List_List*_tmp6C1;int _tmp6C2;struct
Cyc_Absyn_VarargInfo*_tmp6C3;struct Cyc_Core_Opt*_tmp6C4;struct Cyc_List_List*
_tmp6C5;struct _tuple17*_tmp6BF=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6C0=*_tmp6BF;_tmp6C1=
_tmp6C0.f1;_tmp6C2=_tmp6C0.f2;_tmp6C3=_tmp6C0.f3;_tmp6C4=_tmp6C0.f4;_tmp6C5=
_tmp6C0.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6C6;(
_tmp6C6.YY30).tag=34;(_tmp6C6.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6C7=_cycalloc(sizeof(*_tmp6C7));_tmp6C7->tms=({struct Cyc_List_List*_tmp6C8=
_cycalloc(sizeof(*_tmp6C8));_tmp6C8->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6C9=_cycalloc(sizeof(*_tmp6C9));_tmp6C9[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6CA;_tmp6CA.tag=3;_tmp6CA.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6CB=_cycalloc(sizeof(*_tmp6CB));_tmp6CB[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6CC;_tmp6CC.tag=1;_tmp6CC.f1=_tmp6C1;_tmp6CC.f2=_tmp6C2;_tmp6CC.f3=_tmp6C3;
_tmp6CC.f4=_tmp6C4;_tmp6CC.f5=_tmp6C5;_tmp6CC;});_tmp6CB;}));_tmp6CA;});_tmp6C9;}));
_tmp6C8->tl=0;_tmp6C8;});_tmp6C7;});_tmp6C6;});break;}case 261: _LL305: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6CD;(_tmp6CD.YY30).tag=34;(
_tmp6CD.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6CE=_cycalloc(sizeof(*
_tmp6CE));_tmp6CE->tms=({struct Cyc_List_List*_tmp6CF=_cycalloc(sizeof(*_tmp6CF));
_tmp6CF->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*_tmp6D0=
_cycalloc(sizeof(*_tmp6D0));_tmp6D0[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6D1;_tmp6D1.tag=3;_tmp6D1.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6D2=_cycalloc(sizeof(*_tmp6D2));_tmp6D2[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6D3;_tmp6D3.tag=1;_tmp6D3.f1=0;_tmp6D3.f2=0;_tmp6D3.f3=0;_tmp6D3.f4=Cyc_yyget_YY49(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp6D3.f5=Cyc_yyget_YY50(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6D3;});_tmp6D2;}));
_tmp6D1;});_tmp6D0;}));_tmp6CF->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]))->tms;_tmp6CF;});_tmp6CE;});
_tmp6CD;});break;case 262: _LL306: {struct _tuple17 _tmp6D5;struct Cyc_List_List*
_tmp6D6;int _tmp6D7;struct Cyc_Absyn_VarargInfo*_tmp6D8;struct Cyc_Core_Opt*_tmp6D9;
struct Cyc_List_List*_tmp6DA;struct _tuple17*_tmp6D4=Cyc_yyget_YY39(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6D5=*_tmp6D4;_tmp6D6=
_tmp6D5.f1;_tmp6D7=_tmp6D5.f2;_tmp6D8=_tmp6D5.f3;_tmp6D9=_tmp6D5.f4;_tmp6DA=
_tmp6D5.f5;yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6DB;(
_tmp6DB.YY30).tag=34;(_tmp6DB.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6DC=_cycalloc(sizeof(*_tmp6DC));_tmp6DC->tms=({struct Cyc_List_List*_tmp6DD=
_cycalloc(sizeof(*_tmp6DD));_tmp6DD->hd=(void*)((void*)({struct Cyc_Absyn_Function_mod_struct*
_tmp6DE=_cycalloc(sizeof(*_tmp6DE));_tmp6DE[0]=({struct Cyc_Absyn_Function_mod_struct
_tmp6DF;_tmp6DF.tag=3;_tmp6DF.f1=(void*)((void*)({struct Cyc_Absyn_WithTypes_struct*
_tmp6E0=_cycalloc(sizeof(*_tmp6E0));_tmp6E0[0]=({struct Cyc_Absyn_WithTypes_struct
_tmp6E1;_tmp6E1.tag=1;_tmp6E1.f1=_tmp6D6;_tmp6E1.f2=_tmp6D7;_tmp6E1.f3=_tmp6D8;
_tmp6E1.f4=_tmp6D9;_tmp6E1.f5=_tmp6DA;_tmp6E1;});_tmp6E0;}));_tmp6DF;});_tmp6DE;}));
_tmp6DD->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]))->tms;_tmp6DD;});_tmp6DC;});_tmp6DB;});break;}case 263: _LL307: {
struct Cyc_List_List*_tmp6E2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(
struct Cyc_Position_Segment*,void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*
x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp6E3;(_tmp6E3.YY30).tag=
34;(_tmp6E3.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*_tmp6E4=_cycalloc(
sizeof(*_tmp6E4));_tmp6E4->tms=({struct Cyc_List_List*_tmp6E5=_cycalloc(sizeof(*
_tmp6E5));_tmp6E5->hd=(void*)((void*)({struct Cyc_Absyn_TypeParams_mod_struct*
_tmp6E6=_cycalloc(sizeof(*_tmp6E6));_tmp6E6[0]=({struct Cyc_Absyn_TypeParams_mod_struct
_tmp6E7;_tmp6E7.tag=4;_tmp6E7.f1=_tmp6E2;_tmp6E7.f2=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6E7.f3=0;
_tmp6E7;});_tmp6E6;}));_tmp6E5->tl=(Cyc_yyget_YY30(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]))->tms;_tmp6E5;});_tmp6E4;});
_tmp6E3;});break;}case 264: _LL308: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6E8;(_tmp6E8.YY30).tag=34;(_tmp6E8.YY30).f1=({struct Cyc_Parse_Abstractdeclarator*
_tmp6E9=_cycalloc(sizeof(*_tmp6E9));_tmp6E9->tms=({struct Cyc_List_List*_tmp6EA=
_cycalloc(sizeof(*_tmp6EA));_tmp6EA->hd=(void*)((void*)({struct Cyc_Absyn_Attributes_mod_struct*
_tmp6EB=_cycalloc(sizeof(*_tmp6EB));_tmp6EB[0]=({struct Cyc_Absyn_Attributes_mod_struct
_tmp6EC;_tmp6EC.tag=5;_tmp6EC.f1=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp6EC.f2=Cyc_yyget_YY45(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6EC;});_tmp6EB;}));
_tmp6EA->tl=(Cyc_yyget_YY30(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]))->tms;_tmp6EA;});_tmp6E9;});_tmp6E8;});break;case 265: _LL309:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 266:
_LL30A: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
267: _LL30B: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;
case 268: _LL30C: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];
break;case 269: _LL30D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 270: _LL30E: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 271: _LL30F: if(Cyc_zstrcmp((struct _dynforward_ptr)
Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({
const char*_tmp6ED="`H";_tag_dynforward(_tmp6ED,sizeof(char),
_get_zero_arr_size_char(_tmp6ED,3));}))== 0  || Cyc_zstrcmp((struct
_dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),({const char*_tmp6EE="`U";_tag_dynforward(_tmp6EE,sizeof(char),
_get_zero_arr_size_char(_tmp6EE,3));}))== 0)Cyc_Parse_err((struct _dynforward_ptr)({
struct Cyc_String_pa_struct _tmp6F1;_tmp6F1.tag=0;_tmp6F1.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));{void*_tmp6EF[1]={&
_tmp6F1};Cyc_aprintf(({const char*_tmp6F0="bad occurrence of heap region %s";
_tag_dynforward(_tmp6F0,sizeof(char),_get_zero_arr_size_char(_tmp6F0,33));}),
_tag_dynforward(_tmp6EF,sizeof(void*),1));}}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp6FC=_cycalloc(sizeof(*_tmp6FC));_tmp6FC->name=({
struct _dynforward_ptr*_tmp6FD=_cycalloc(sizeof(*_tmp6FD));_tmp6FD[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp6FD;});_tmp6FC->identity=
- 1;_tmp6FC->kind=(void*)Cyc_Tcutil_kind_to_bound((void*)3);_tmp6FC;});void*t=(
void*)({struct Cyc_Absyn_VarType_struct*_tmp6FA=_cycalloc(sizeof(*_tmp6FA));
_tmp6FA[0]=({struct Cyc_Absyn_VarType_struct _tmp6FB;_tmp6FB.tag=1;_tmp6FB.f1=tv;
_tmp6FB;});_tmp6FA;});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp6F2;(_tmp6F2.YY9).tag=13;(_tmp6F2.YY9).f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp6F3=_cycalloc(sizeof(*_tmp6F3));_tmp6F3[0]=({
struct Cyc_Absyn_Region_s_struct _tmp6F4;_tmp6F4.tag=15;_tmp6F4.f1=tv;_tmp6F4.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp6F5=_cycalloc(sizeof(*_tmp6F5));
_tmp6F5->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp6F7;(
_tmp6F7.Loc_n).tag=0;_tmp6F7;});_tmp6F5->f2=({struct _dynforward_ptr*_tmp6F6=
_cycalloc(sizeof(*_tmp6F6));_tmp6F6[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp6F6;});_tmp6F5;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp6F8=_cycalloc(sizeof(*_tmp6F8));
_tmp6F8[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp6F9;_tmp6F9.tag=15;_tmp6F9.f1=(
void*)t;_tmp6F9;});_tmp6F8;}),0);_tmp6F4.f3=0;_tmp6F4.f4=0;_tmp6F4.f5=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp6F4;});_tmp6F3;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp6F2;});break;}case 272: _LL310: if(Cyc_zstrcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]),({const char*_tmp6FE="H";_tag_dynforward(_tmp6FE,sizeof(
char),_get_zero_arr_size_char(_tmp6FE,2));}))== 0)Cyc_Parse_err(({const char*
_tmp6FF="bad occurrence of heap region `H";_tag_dynforward(_tmp6FF,sizeof(char),
_get_zero_arr_size_char(_tmp6FF,33));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp70A=_cycalloc(sizeof(*_tmp70A));_tmp70A->name=({
struct _dynforward_ptr*_tmp70B=_cycalloc(sizeof(*_tmp70B));_tmp70B[0]=(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp70E;_tmp70E.tag=0;_tmp70E.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));{void*_tmp70C[1]={&
_tmp70E};Cyc_aprintf(({const char*_tmp70D="`%s";_tag_dynforward(_tmp70D,sizeof(
char),_get_zero_arr_size_char(_tmp70D,4));}),_tag_dynforward(_tmp70C,sizeof(void*),
1));}});_tmp70B;});_tmp70A->identity=- 1;_tmp70A->kind=(void*)Cyc_Tcutil_kind_to_bound((
void*)3);_tmp70A;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp708=
_cycalloc(sizeof(*_tmp708));_tmp708[0]=({struct Cyc_Absyn_VarType_struct _tmp709;
_tmp709.tag=1;_tmp709.f1=tv;_tmp709;});_tmp708;});yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp700;(_tmp700.YY9).tag=13;(_tmp700.YY9).f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Region_s_struct*_tmp701=_cycalloc(sizeof(*_tmp701));
_tmp701[0]=({struct Cyc_Absyn_Region_s_struct _tmp702;_tmp702.tag=15;_tmp702.f1=tv;
_tmp702.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp703=_cycalloc(sizeof(*
_tmp703));_tmp703->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp705;(_tmp705.Loc_n).tag=0;_tmp705;});_tmp703->f2=({struct _dynforward_ptr*
_tmp704=_cycalloc(sizeof(*_tmp704));_tmp704[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp704;});_tmp703;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp706=_cycalloc(sizeof(*_tmp706));
_tmp706[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp707;_tmp707.tag=15;_tmp707.f1=(
void*)t;_tmp707;});_tmp706;}),0);_tmp702.f3=0;_tmp702.f4=Cyc_yyget_YY4(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp702.f5=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp702;});_tmp701;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp700;});break;}case 273: _LL311: if(Cyc_zstrcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 6)]),({const char*_tmp70F="resetable";_tag_dynforward(_tmp70F,
sizeof(char),_get_zero_arr_size_char(_tmp70F,10));}))!= 0)Cyc_Parse_err(({const
char*_tmp710="expecting [resetable]";_tag_dynforward(_tmp710,sizeof(char),
_get_zero_arr_size_char(_tmp710,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).last_line));if(Cyc_zstrcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),({const char*_tmp711="`H";_tag_dynforward(_tmp711,
sizeof(char),_get_zero_arr_size_char(_tmp711,3));}))== 0  || Cyc_zstrcmp((struct
_dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),({const char*_tmp712="`U";_tag_dynforward(_tmp712,sizeof(char),
_get_zero_arr_size_char(_tmp712,3));})))Cyc_Parse_err((struct _dynforward_ptr)({
struct Cyc_String_pa_struct _tmp715;_tmp715.tag=0;_tmp715.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));{void*_tmp713[1]={&
_tmp715};Cyc_aprintf(({const char*_tmp714="bad occurrence of heap region %s";
_tag_dynforward(_tmp714,sizeof(char),_get_zero_arr_size_char(_tmp714,33));}),
_tag_dynforward(_tmp713,sizeof(void*),1));}}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp720=_cycalloc(sizeof(*_tmp720));_tmp720->name=({
struct _dynforward_ptr*_tmp721=_cycalloc(sizeof(*_tmp721));_tmp721[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp721;});_tmp720->identity=
- 1;_tmp720->kind=(void*)Cyc_Tcutil_kind_to_bound((void*)3);_tmp720;});void*t=(
void*)({struct Cyc_Absyn_VarType_struct*_tmp71E=_cycalloc(sizeof(*_tmp71E));
_tmp71E[0]=({struct Cyc_Absyn_VarType_struct _tmp71F;_tmp71F.tag=1;_tmp71F.f1=tv;
_tmp71F;});_tmp71E;});yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp716;(_tmp716.YY9).tag=13;(_tmp716.YY9).f1=Cyc_Absyn_new_stmt((void*)({struct
Cyc_Absyn_Region_s_struct*_tmp717=_cycalloc(sizeof(*_tmp717));_tmp717[0]=({
struct Cyc_Absyn_Region_s_struct _tmp718;_tmp718.tag=15;_tmp718.f1=tv;_tmp718.f2=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp719=_cycalloc(sizeof(*_tmp719));
_tmp719->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp71B;(
_tmp71B.Loc_n).tag=0;_tmp71B;});_tmp719->f2=({struct _dynforward_ptr*_tmp71A=
_cycalloc(sizeof(*_tmp71A));_tmp71A[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp71A;});_tmp719;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp71C=_cycalloc(sizeof(*_tmp71C));
_tmp71C[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp71D;_tmp71D.tag=15;_tmp71D.f1=(
void*)t;_tmp71D;});_tmp71C;}),0);_tmp718.f3=1;_tmp718.f4=0;_tmp718.f5=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp718;});_tmp717;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 8)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp716;});break;}case 274: _LL312: if(Cyc_zstrcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)]),({const char*_tmp722="resetable";_tag_dynforward(_tmp722,
sizeof(char),_get_zero_arr_size_char(_tmp722,10));}))!= 0)Cyc_Parse_err(({const
char*_tmp723="expecting `resetable'";_tag_dynforward(_tmp723,sizeof(char),
_get_zero_arr_size_char(_tmp723,22));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));if(Cyc_zstrcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),({const char*_tmp724="H";_tag_dynforward(_tmp724,sizeof(
char),_get_zero_arr_size_char(_tmp724,2));}))== 0)Cyc_Parse_err(({const char*
_tmp725="bad occurrence of heap region `H";_tag_dynforward(_tmp725,sizeof(char),
_get_zero_arr_size_char(_tmp725,33));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp730=_cycalloc(sizeof(*_tmp730));_tmp730->name=({
struct _dynforward_ptr*_tmp731=_cycalloc(sizeof(*_tmp731));_tmp731[0]=(struct
_dynforward_ptr)({struct Cyc_String_pa_struct _tmp734;_tmp734.tag=0;_tmp734.f1=(
struct _dynforward_ptr)((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));{void*_tmp732[1]={&
_tmp734};Cyc_aprintf(({const char*_tmp733="`%s";_tag_dynforward(_tmp733,sizeof(
char),_get_zero_arr_size_char(_tmp733,4));}),_tag_dynforward(_tmp732,sizeof(void*),
1));}});_tmp731;});_tmp730->identity=- 1;_tmp730->kind=(void*)Cyc_Tcutil_kind_to_bound((
void*)3);_tmp730;});void*t=(void*)({struct Cyc_Absyn_VarType_struct*_tmp72E=
_cycalloc(sizeof(*_tmp72E));_tmp72E[0]=({struct Cyc_Absyn_VarType_struct _tmp72F;
_tmp72F.tag=1;_tmp72F.f1=tv;_tmp72F;});_tmp72E;});yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp726;(_tmp726.YY9).tag=13;(_tmp726.YY9).f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Region_s_struct*_tmp727=_cycalloc(sizeof(*_tmp727));
_tmp727[0]=({struct Cyc_Absyn_Region_s_struct _tmp728;_tmp728.tag=15;_tmp728.f1=tv;
_tmp728.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp729=_cycalloc(sizeof(*
_tmp729));_tmp729->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp72B;(_tmp72B.Loc_n).tag=0;_tmp72B;});_tmp729->f2=({struct _dynforward_ptr*
_tmp72A=_cycalloc(sizeof(*_tmp72A));_tmp72A[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp72A;});_tmp729;}),(
void*)({struct Cyc_Absyn_RgnHandleType_struct*_tmp72C=_cycalloc(sizeof(*_tmp72C));
_tmp72C[0]=({struct Cyc_Absyn_RgnHandleType_struct _tmp72D;_tmp72D.tag=15;_tmp72D.f1=(
void*)t;_tmp72D;});_tmp72C;}),0);_tmp728.f3=1;_tmp728.f4=0;_tmp728.f5=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp728;});_tmp727;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp726;});break;}case 275: _LL313: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp735;(_tmp735.YY9).tag=13;(_tmp735.YY9).f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_ResetRegion_s_struct*_tmp736=_cycalloc(sizeof(*_tmp736));
_tmp736[0]=({struct Cyc_Absyn_ResetRegion_s_struct _tmp737;_tmp737.tag=16;_tmp737.f1=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);
_tmp737;});_tmp736;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp735;});
break;case 276: _LL314: if(Cyc_zstrcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({const char*_tmp738="in";
_tag_dynforward(_tmp738,sizeof(char),_get_zero_arr_size_char(_tmp738,3));}))!= 0)
Cyc_Parse_err(({const char*_tmp739="expecting `in'";_tag_dynforward(_tmp739,
sizeof(char),_get_zero_arr_size_char(_tmp739,15));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));{struct Cyc_Absyn_Tvar*
tv=({struct Cyc_Absyn_Tvar*_tmp740=_cycalloc(sizeof(*_tmp740));_tmp740->name=({
struct _dynforward_ptr*_tmp743=_cycalloc(sizeof(*_tmp743));_tmp743[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp743;});_tmp740->identity=
- 1;_tmp740->kind=(void*)((void*)({struct Cyc_Absyn_Eq_kb_struct*_tmp741=_cycalloc(
sizeof(*_tmp741));_tmp741[0]=({struct Cyc_Absyn_Eq_kb_struct _tmp742;_tmp742.tag=0;
_tmp742.f1=(void*)((void*)3);_tmp742;});_tmp741;}));_tmp740;});yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp73A;(_tmp73A.YY9).tag=13;(_tmp73A.YY9).f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Alias_s_struct*_tmp73B=_cycalloc(sizeof(*_tmp73B));
_tmp73B[0]=({struct Cyc_Absyn_Alias_s_struct _tmp73C;_tmp73C.tag=17;_tmp73C.f1=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp73C.f2=tv;
_tmp73C.f3=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp73D=_cycalloc(sizeof(*
_tmp73D));_tmp73D->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp73F;(_tmp73F.Loc_n).tag=0;_tmp73F;});_tmp73D->f2=({struct _dynforward_ptr*
_tmp73E=_cycalloc(sizeof(*_tmp73E));_tmp73E[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp73E;});_tmp73D;}),(
void*)0,0);_tmp73C.f4=Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp73C;});_tmp73B;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp73A;});
break;}case 277: _LL315: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp744;(_tmp744.YY4).tag=8;(_tmp744.YY4).f1=0;_tmp744;});break;case 278: _LL316:
if(Cyc_zstrcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),({const char*_tmp745="open";
_tag_dynforward(_tmp745,sizeof(char),_get_zero_arr_size_char(_tmp745,5));}))!= 0)
Cyc_Parse_err(({const char*_tmp746="expecting `open'";_tag_dynforward(_tmp746,
sizeof(char),_get_zero_arr_size_char(_tmp746,17));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp747;(_tmp747.YY4).tag=8;(_tmp747.YY4).f1=(
struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp747;});break;case 279: _LL317: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp748;(_tmp748.YY9).tag=13;(_tmp748.YY9).f1=Cyc_Absyn_new_stmt((
void*)({struct Cyc_Absyn_Label_s_struct*_tmp749=_cycalloc(sizeof(*_tmp749));
_tmp749[0]=({struct Cyc_Absyn_Label_s_struct _tmp74A;_tmp74A.tag=12;_tmp74A.f1=({
struct _dynforward_ptr*_tmp74B=_cycalloc(sizeof(*_tmp74B));_tmp74B[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp74B;});_tmp74A.f2=
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp74A;});
_tmp749;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp748;});break;case 280: _LL318: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp74C;(_tmp74C.YY9).tag=13;(_tmp74C.YY9).f1=Cyc_Absyn_skip_stmt(
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp74C;});break;case 281: _LL319: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp74D;(_tmp74D.YY9).tag=13;(_tmp74D.YY9).f1=Cyc_Absyn_exp_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp74D;});
break;case 282: _LL31A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp74E;(_tmp74E.YY9).tag=13;(_tmp74E.YY9).f1=Cyc_Absyn_skip_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp74E;});
break;case 283: _LL31B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 284: _LL31C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp74F;(_tmp74F.YY9).tag=13;(_tmp74F.YY9).f1=Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0));
_tmp74F;});break;case 285: _LL31D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp750;(_tmp750.YY9).tag=13;(_tmp750.YY9).f1=Cyc_Parse_flatten_declarations(Cyc_yyget_YY18(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp750;});break;case 286:
_LL31E: yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case
287: _LL31F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp751;(
_tmp751.YY9).tag=13;(_tmp751.YY9).f1=Cyc_Absyn_seq_stmt(Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp751;});
break;case 288: _LL320: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp752;(_tmp752.YY9).tag=13;(_tmp752.YY9).f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Fn_d_struct*_tmp753=_cycalloc(sizeof(*_tmp753));_tmp753[
0]=({struct Cyc_Absyn_Fn_d_struct _tmp754;_tmp754.tag=1;_tmp754.f1=Cyc_yyget_YY17(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp754;});_tmp753;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)),Cyc_Absyn_skip_stmt(0));_tmp752;});break;case 289:
_LL321: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp755;(_tmp755.YY9).tag=
13;(_tmp755.YY9).f1=Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_struct*
_tmp756=_cycalloc(sizeof(*_tmp756));_tmp756[0]=({struct Cyc_Absyn_Fn_d_struct
_tmp757;_tmp757.tag=1;_tmp757.f1=Cyc_yyget_YY17(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp757;});_tmp756;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line)),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]));_tmp755;});break;case 290: _LL322: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp758;(_tmp758.YY9).tag=13;(_tmp758.YY9).f1=Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Absyn_skip_stmt(0),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp758;});break;case 291: _LL323: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp759;(_tmp759.YY9).tag=13;(_tmp759.YY9).f1=Cyc_Absyn_ifthenelse_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp759;});
break;case 292: _LL324: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp75A;(_tmp75A.YY9).tag=13;(_tmp75A.YY9).f1=Cyc_Absyn_switch_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp75A;});
break;case 293: _LL325: {struct Cyc_Absyn_Exp*_tmp75B=Cyc_Absyn_new_exp((void*)({
struct Cyc_Absyn_UnknownId_e_struct*_tmp75D=_cycalloc(sizeof(*_tmp75D));_tmp75D[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmp75E;_tmp75E.tag=2;_tmp75E.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp75E;});_tmp75D;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).last_line));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp75C;(_tmp75C.YY9).tag=13;(_tmp75C.YY9).f1=Cyc_Absyn_switch_stmt(_tmp75B,Cyc_yyget_YY10(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp75C;});
break;}case 294: _LL326: {struct Cyc_Absyn_Exp*_tmp75F=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp760;(_tmp760.YY9).tag=13;(_tmp760.YY9).f1=
Cyc_Absyn_switch_stmt(_tmp75F,Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp760;});
break;}case 295: _LL327: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp761;(_tmp761.YY9).tag=13;(_tmp761.YY9).f1=Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp761;});
break;case 296: _LL328: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp762;(_tmp762.YY10).tag=14;(_tmp762.YY10).f1=0;_tmp762;});break;case 297:
_LL329: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp763;(_tmp763.YY10).tag=
14;(_tmp763.YY10).f1=({struct Cyc_List_List*_tmp764=_cycalloc(sizeof(*_tmp764));
_tmp764->hd=({struct Cyc_Absyn_Switch_clause*_tmp765=_cycalloc(sizeof(*_tmp765));
_tmp765->pattern=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp765->pat_vars=
0;_tmp765->where_clause=0;_tmp765->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp765->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp765;});
_tmp764->tl=0;_tmp764;});_tmp763;});break;case 298: _LL32A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp766;(_tmp766.YY10).tag=14;(_tmp766.YY10).f1=({struct
Cyc_List_List*_tmp767=_cycalloc(sizeof(*_tmp767));_tmp767->hd=({struct Cyc_Absyn_Switch_clause*
_tmp768=_cycalloc(sizeof(*_tmp768));_tmp768->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp768->pat_vars=0;
_tmp768->where_clause=0;_tmp768->body=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp768->loc=
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line);_tmp768;});_tmp767->tl=Cyc_yyget_YY10(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp767;});_tmp766;});
break;case 299: _LL32B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp769;(_tmp769.YY10).tag=14;(_tmp769.YY10).f1=({struct Cyc_List_List*_tmp76A=
_cycalloc(sizeof(*_tmp76A));_tmp76A->hd=({struct Cyc_Absyn_Switch_clause*_tmp76B=
_cycalloc(sizeof(*_tmp76B));_tmp76B->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp76B->pat_vars=0;
_tmp76B->where_clause=0;_tmp76B->body=Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp76B->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line);_tmp76B;});
_tmp76A->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp76A;});_tmp769;});break;case 300: _LL32C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp76C;(_tmp76C.YY10).tag=14;(_tmp76C.YY10).f1=({struct
Cyc_List_List*_tmp76D=_cycalloc(sizeof(*_tmp76D));_tmp76D->hd=({struct Cyc_Absyn_Switch_clause*
_tmp76E=_cycalloc(sizeof(*_tmp76E));_tmp76E->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp76E->pat_vars=0;
_tmp76E->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp76E->body=Cyc_Absyn_fallthru_stmt(
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).last_line));_tmp76E->loc=Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp76E;});
_tmp76D->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp76D;});_tmp76C;});break;case 301: _LL32D: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp76F;(_tmp76F.YY10).tag=14;(_tmp76F.YY10).f1=({struct
Cyc_List_List*_tmp770=_cycalloc(sizeof(*_tmp770));_tmp770->hd=({struct Cyc_Absyn_Switch_clause*
_tmp771=_cycalloc(sizeof(*_tmp771));_tmp771->pattern=Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);_tmp771->pat_vars=0;
_tmp771->where_clause=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp771->body=Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp771->loc=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);_tmp771;});
_tmp770->tl=Cyc_yyget_YY10(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp770;});_tmp76F;});break;case 302: _LL32E: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp772;(_tmp772.YY9).tag=13;(_tmp772.YY9).f1=Cyc_Absyn_while_stmt(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp772;});
break;case 303: _LL32F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp773;(_tmp773.YY9).tag=13;(_tmp773.YY9).f1=Cyc_Absyn_do_stmt(Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp773;});
break;case 304: _LL330: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp774;(_tmp774.YY9).tag=13;(_tmp774.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp774;});
break;case 305: _LL331: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp775;(_tmp775.YY9).tag=13;(_tmp775.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 6)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp775;});break;case 306: _LL332: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp776;(_tmp776.YY9).tag=13;(_tmp776.YY9).f1=Cyc_Absyn_for_stmt(
Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp776;});
break;case 307: _LL333: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp777;(_tmp777.YY9).tag=13;(_tmp777.YY9).f1=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp777;});
break;case 308: _LL334: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp778;(_tmp778.YY9).tag=13;(_tmp778.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_Absyn_true_exp(
0),Cyc_Absyn_false_exp(0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp778;});
break;case 309: _LL335: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp779;(_tmp779.YY9).tag=13;(_tmp779.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp779;});
break;case 310: _LL336: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp77A;(_tmp77A.YY9).tag=13;(_tmp77A.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp77A;});
break;case 311: _LL337: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp77B;(_tmp77B.YY9).tag=13;(_tmp77B.YY9).f1=Cyc_Absyn_for_stmt(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp77B;});
break;case 312: _LL338: {struct Cyc_List_List*_tmp77C=Cyc_yyget_YY18(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);struct Cyc_Absyn_Stmt*
_tmp77D=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp77E;(_tmp77E.YY9).tag=13;(_tmp77E.YY9).f1=Cyc_Parse_flatten_declarations(
_tmp77C,_tmp77D);_tmp77E;});break;}case 313: _LL339: {struct Cyc_List_List*_tmp77F=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
struct Cyc_Absyn_Stmt*_tmp780=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_Absyn_false_exp(
0),Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp781;(_tmp781.YY9).tag=13;(_tmp781.YY9).f1=Cyc_Parse_flatten_declarations(
_tmp77F,_tmp780);_tmp781;});break;}case 314: _LL33A: {struct Cyc_List_List*_tmp782=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);
struct Cyc_Absyn_Stmt*_tmp783=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(
0),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp784;(_tmp784.YY9).tag=13;(_tmp784.YY9).f1=Cyc_Parse_flatten_declarations(
_tmp782,_tmp783);_tmp784;});break;}case 315: _LL33B: {struct Cyc_List_List*_tmp785=
Cyc_yyget_YY18(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);
struct Cyc_Absyn_Stmt*_tmp786=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY9(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 7)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp787;(_tmp787.YY9).tag=13;(_tmp787.YY9).f1=Cyc_Parse_flatten_declarations(
_tmp785,_tmp786);_tmp787;});break;}case 316: _LL33C: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp788;(_tmp788.YY9).tag=13;(_tmp788.YY9).f1=Cyc_Absyn_goto_stmt(({
struct _dynforward_ptr*_tmp789=_cycalloc(sizeof(*_tmp789));_tmp789[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp789;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp788;});
break;case 317: _LL33D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78A;(_tmp78A.YY9).tag=13;(_tmp78A.YY9).f1=Cyc_Absyn_continue_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp78A;});
break;case 318: _LL33E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78B;(_tmp78B.YY9).tag=13;(_tmp78B.YY9).f1=Cyc_Absyn_break_stmt(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp78B;});
break;case 319: _LL33F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78C;(_tmp78C.YY9).tag=13;(_tmp78C.YY9).f1=Cyc_Absyn_return_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp78C;});
break;case 320: _LL340: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78D;(_tmp78D.YY9).tag=13;(_tmp78D.YY9).f1=Cyc_Absyn_return_stmt((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp78D;});
break;case 321: _LL341: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78E;(_tmp78E.YY9).tag=13;(_tmp78E.YY9).f1=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));_tmp78E;});
break;case 322: _LL342: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp78F;(_tmp78F.YY9).tag=13;(_tmp78F.YY9).f1=Cyc_Absyn_fallthru_stmt(0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).last_line));_tmp78F;});
break;case 323: _LL343: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp790;(_tmp790.YY9).tag=13;(_tmp790.YY9).f1=Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 4)]).last_line));_tmp790;});
break;case 324: _LL344: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 325: _LL345: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 326: _LL346: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp791;(_tmp791.YY11).tag=15;(_tmp791.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 4)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp791;});
break;case 327: _LL347: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 328: _LL348: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp792;(_tmp792.YY11).tag=15;(_tmp792.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp792;});break;case 329: _LL349: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 330: _LL34A: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp793;(_tmp793.YY11).tag=15;(
_tmp793.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp793;});
break;case 331: _LL34B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 332: _LL34C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp794;(_tmp794.YY11).tag=15;(_tmp794.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)14,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp794;});
break;case 333: _LL34D: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 334: _LL34E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp795;(_tmp795.YY11).tag=15;(_tmp795.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)15,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp795;});
break;case 335: _LL34F: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 336: _LL350: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp796;(_tmp796.YY11).tag=15;(_tmp796.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)13,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp796;});
break;case 337: _LL351: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 338: _LL352: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp797;(_tmp797.YY11).tag=15;(_tmp797.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp797;});break;case 339: _LL353: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp798;(_tmp798.YY11).tag=15;(_tmp798.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp798;});
break;case 340: _LL354: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 341: _LL355: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp799;(_tmp799.YY11).tag=15;(_tmp799.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp799;});break;case 342: _LL356: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp79A;(_tmp79A.YY11).tag=15;(_tmp79A.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp79A;});
break;case 343: _LL357: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp79B;(_tmp79B.YY11).tag=15;(_tmp79B.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(
Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp79B;});break;case 344: _LL358: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp79C;(_tmp79C.YY11).tag=15;(_tmp79C.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp79C;});
break;case 345: _LL359: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 346: _LL35A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp79D;(_tmp79D.YY11).tag=15;(_tmp79D.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)16,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp79D;});
break;case 347: _LL35B: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp79E;(_tmp79E.YY11).tag=15;(_tmp79E.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)17,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp79E;});
break;case 348: _LL35C: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 349: _LL35D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp79F;(_tmp79F.YY11).tag=15;(_tmp79F.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)0,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp79F;});
break;case 350: _LL35E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7A0;(_tmp7A0.YY11).tag=15;(_tmp7A0.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)2,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7A0;});
break;case 351: _LL35F: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 352: _LL360: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7A1;(_tmp7A1.YY11).tag=15;(_tmp7A1.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)1,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7A1;});
break;case 353: _LL361: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7A2;(_tmp7A2.YY11).tag=15;(_tmp7A2.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)3,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7A2;});
break;case 354: _LL362: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7A3;(_tmp7A3.YY11).tag=15;(_tmp7A3.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp((
void*)4,Cyc_Parse_pat2exp(Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)])),Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7A3;});
break;case 355: _LL363: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 356: _LL364: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7A4;(_tmp7A4.YY11).tag=15;(_tmp7A4.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)
0,Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7A4;});break;case 357: _LL365: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 358: _LL366: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7A5;(_tmp7A5.YY11).tag=15;(
_tmp7A5.YY11).f1=Cyc_Absyn_exp_pat(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp7A5;});break;case 359:
_LL367: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7A6;(_tmp7A6.YY11).tag=
15;(_tmp7A6.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line)));_tmp7A6;});
break;case 360: _LL368: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7A7;(_tmp7A7.YY11).tag=15;(_tmp7A7.YY11).f1=Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7A7;});break;case 361: _LL369: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7A8;(_tmp7A8.YY11).tag=15;(_tmp7A8.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7A8;});break;case 362: _LL36A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7A9;(_tmp7A9.YY11).tag=15;(_tmp7A9.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_StructField_struct*_tmp7AA=
_cycalloc(sizeof(*_tmp7AA));_tmp7AA[0]=({struct Cyc_Absyn_StructField_struct
_tmp7AB;_tmp7AB.tag=0;_tmp7AB.f1=({struct _dynforward_ptr*_tmp7AC=_cycalloc(
sizeof(*_tmp7AC));_tmp7AC[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp7AC;});_tmp7AB;});
_tmp7AA;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7A9;});break;case 363: _LL36B: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7AD;(_tmp7AD.YY11).tag=15;(_tmp7AD.YY11).f1=Cyc_Absyn_exp_pat(
Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 3)])).f3,(void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp7AE=
_cycalloc_atomic(sizeof(*_tmp7AE));_tmp7AE[0]=({struct Cyc_Absyn_TupleIndex_struct
_tmp7AF;_tmp7AF.tag=1;_tmp7AF.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f2;_tmp7AF;});_tmp7AE;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line)));_tmp7AD;});break;case 364: _LL36C: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 365: _LL36D: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 366: _LL36E:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7B0;(_tmp7B0.YY11).tag=
15;(_tmp7B0.YY11).f1=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7B0;});
break;case 367: _LL36F: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7B1;(_tmp7B1.YY11).tag=15;(_tmp7B1.YY11).f1=Cyc_Absyn_exp_pat(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp7B1;});break;
case 368: _LL370: {struct Cyc_Absyn_Exp*e=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);{void*_tmp7B2=(void*)e->r;
union Cyc_Absyn_Cnst_union _tmp7B3;void*_tmp7B4;char _tmp7B5;union Cyc_Absyn_Cnst_union
_tmp7B6;void*_tmp7B7;short _tmp7B8;union Cyc_Absyn_Cnst_union _tmp7B9;void*_tmp7BA;
int _tmp7BB;union Cyc_Absyn_Cnst_union _tmp7BC;struct _dynforward_ptr _tmp7BD;union
Cyc_Absyn_Cnst_union _tmp7BE;union Cyc_Absyn_Cnst_union _tmp7BF;union Cyc_Absyn_Cnst_union
_tmp7C0;_LL373: if(*((int*)_tmp7B2)!= 0)goto _LL375;_tmp7B3=((struct Cyc_Absyn_Const_e_struct*)
_tmp7B2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7B2)->f1).Char_c).tag != 
0)goto _LL375;_tmp7B4=(_tmp7B3.Char_c).f1;_tmp7B5=(_tmp7B3.Char_c).f2;_LL374:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7C1;(_tmp7C1.YY11).tag=
15;(_tmp7C1.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Char_p_struct*
_tmp7C2=_cycalloc_atomic(sizeof(*_tmp7C2));_tmp7C2[0]=({struct Cyc_Absyn_Char_p_struct
_tmp7C3;_tmp7C3.tag=8;_tmp7C3.f1=_tmp7B5;_tmp7C3;});_tmp7C2;}),e->loc);_tmp7C1;});
goto _LL372;_LL375: if(*((int*)_tmp7B2)!= 0)goto _LL377;_tmp7B6=((struct Cyc_Absyn_Const_e_struct*)
_tmp7B2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7B2)->f1).Short_c).tag != 
1)goto _LL377;_tmp7B7=(_tmp7B6.Short_c).f1;_tmp7B8=(_tmp7B6.Short_c).f2;_LL376:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7C4;(_tmp7C4.YY11).tag=
15;(_tmp7C4.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Int_p_struct*
_tmp7C5=_cycalloc(sizeof(*_tmp7C5));_tmp7C5[0]=({struct Cyc_Absyn_Int_p_struct
_tmp7C6;_tmp7C6.tag=7;_tmp7C6.f1=(void*)_tmp7B7;_tmp7C6.f2=(int)_tmp7B8;_tmp7C6;});
_tmp7C5;}),e->loc);_tmp7C4;});goto _LL372;_LL377: if(*((int*)_tmp7B2)!= 0)goto
_LL379;_tmp7B9=((struct Cyc_Absyn_Const_e_struct*)_tmp7B2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp7B2)->f1).Int_c).tag != 2)goto _LL379;_tmp7BA=(_tmp7B9.Int_c).f1;_tmp7BB=(
_tmp7B9.Int_c).f2;_LL378: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7C7;(_tmp7C7.YY11).tag=15;(_tmp7C7.YY11).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Int_p_struct*_tmp7C8=_cycalloc(sizeof(*_tmp7C8));_tmp7C8[0]=({
struct Cyc_Absyn_Int_p_struct _tmp7C9;_tmp7C9.tag=7;_tmp7C9.f1=(void*)_tmp7BA;
_tmp7C9.f2=_tmp7BB;_tmp7C9;});_tmp7C8;}),e->loc);_tmp7C7;});goto _LL372;_LL379:
if(*((int*)_tmp7B2)!= 0)goto _LL37B;_tmp7BC=((struct Cyc_Absyn_Const_e_struct*)
_tmp7B2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7B2)->f1).Float_c).tag != 
4)goto _LL37B;_tmp7BD=(_tmp7BC.Float_c).f1;_LL37A: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7CA;(_tmp7CA.YY11).tag=15;(_tmp7CA.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Float_p_struct*_tmp7CB=_cycalloc(sizeof(*_tmp7CB));
_tmp7CB[0]=({struct Cyc_Absyn_Float_p_struct _tmp7CC;_tmp7CC.tag=9;_tmp7CC.f1=
_tmp7BD;_tmp7CC;});_tmp7CB;}),e->loc);_tmp7CA;});goto _LL372;_LL37B: if(*((int*)
_tmp7B2)!= 0)goto _LL37D;_tmp7BE=((struct Cyc_Absyn_Const_e_struct*)_tmp7B2)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7B2)->f1).Null_c).tag != 6)goto _LL37D;
_LL37C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7CD;(_tmp7CD.YY11).tag=
15;(_tmp7CD.YY11).f1=Cyc_Absyn_new_pat((void*)1,e->loc);_tmp7CD;});goto _LL372;
_LL37D: if(*((int*)_tmp7B2)!= 0)goto _LL37F;_tmp7BF=((struct Cyc_Absyn_Const_e_struct*)
_tmp7B2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7B2)->f1).String_c).tag
!= 5)goto _LL37F;_LL37E: Cyc_Parse_err(({const char*_tmp7CE="strings cannot occur within patterns";
_tag_dynforward(_tmp7CE,sizeof(char),_get_zero_arr_size_char(_tmp7CE,37));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL372;
_LL37F: if(*((int*)_tmp7B2)!= 0)goto _LL381;_tmp7C0=((struct Cyc_Absyn_Const_e_struct*)
_tmp7B2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp7B2)->f1).LongLong_c).tag
!= 3)goto _LL381;_LL380: Cyc_Parse_unimp(({const char*_tmp7CF="long long's in patterns";
_tag_dynforward(_tmp7CF,sizeof(char),_get_zero_arr_size_char(_tmp7CF,24));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));goto _LL372;
_LL381:;_LL382: Cyc_Parse_err(({const char*_tmp7D0="bad constant in case";
_tag_dynforward(_tmp7D0,sizeof(char),_get_zero_arr_size_char(_tmp7D0,21));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_LL372:;}break;}
case 369: _LL371: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7D1;(
_tmp7D1.YY11).tag=15;(_tmp7D1.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_UnknownId_p_struct*
_tmp7D2=_cycalloc(sizeof(*_tmp7D2));_tmp7D2[0]=({struct Cyc_Absyn_UnknownId_p_struct
_tmp7D3;_tmp7D3.tag=12;_tmp7D3.f1=Cyc_yyget_QualId_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7D3;});_tmp7D2;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7D1;});
break;case 370: _LL383: if(Cyc_strcmp((struct _dynforward_ptr)Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),({const char*_tmp7D4="as";
_tag_dynforward(_tmp7D4,sizeof(char),_get_zero_arr_size_char(_tmp7D4,3));}))!= 0)
Cyc_Parse_err(({const char*_tmp7D5="expecting `as'";_tag_dynforward(_tmp7D5,
sizeof(char),_get_zero_arr_size_char(_tmp7D5,15));}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp7D6;(_tmp7D6.YY11).tag=15;(_tmp7D6.YY11).f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_struct*_tmp7D7=_cycalloc(
sizeof(*_tmp7D7));_tmp7D7[0]=({struct Cyc_Absyn_Var_p_struct _tmp7D8;_tmp7D8.tag=0;
_tmp7D8.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7D9=_cycalloc(sizeof(*
_tmp7D9));_tmp7D9->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp7DB;(_tmp7DB.Loc_n).tag=0;_tmp7DB;});_tmp7D9->f2=({struct _dynforward_ptr*
_tmp7DA=_cycalloc(sizeof(*_tmp7DA));_tmp7DA[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp7DA;});_tmp7D9;}),(
void*)0,0);_tmp7D8.f2=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7D8;});_tmp7D7;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp7D6;});
break;case 371: _LL384: {struct Cyc_List_List*_tmp7DD;int _tmp7DE;struct _tuple13
_tmp7DC=*Cyc_yyget_YY12(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7DD=_tmp7DC.f1;_tmp7DE=_tmp7DC.f2;yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp7DF;(_tmp7DF.YY11).tag=15;(_tmp7DF.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Tuple_p_struct*_tmp7E0=_cycalloc(sizeof(*_tmp7E0));
_tmp7E0[0]=({struct Cyc_Absyn_Tuple_p_struct _tmp7E1;_tmp7E1.tag=3;_tmp7E1.f1=
_tmp7DD;_tmp7E1.f2=_tmp7DE;_tmp7E1;});_tmp7E0;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7DF;});
break;}case 372: _LL385: {struct Cyc_List_List*_tmp7E3;int _tmp7E4;struct _tuple13
_tmp7E2=*Cyc_yyget_YY12(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7E3=_tmp7E2.f1;_tmp7E4=_tmp7E2.f2;yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp7E5;(_tmp7E5.YY11).tag=15;(_tmp7E5.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_UnknownCall_p_struct*_tmp7E6=_cycalloc(sizeof(*_tmp7E6));
_tmp7E6[0]=({struct Cyc_Absyn_UnknownCall_p_struct _tmp7E7;_tmp7E7.tag=13;_tmp7E7.f1=
Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);
_tmp7E7.f2=_tmp7E3;_tmp7E7.f3=_tmp7E4;_tmp7E7;});_tmp7E6;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7E5;});
break;}case 373: _LL386: {struct Cyc_List_List*_tmp7E9;int _tmp7EA;struct _tuple13
_tmp7E8=*Cyc_yyget_YY16(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 
1)]);_tmp7E9=_tmp7E8.f1;_tmp7EA=_tmp7E8.f2;{struct Cyc_List_List*_tmp7EB=((struct
Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,void*),
struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 2)]).last_line),Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7EC;(_tmp7EC.YY11).tag=15;(_tmp7EC.YY11).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Aggr_p_struct*_tmp7ED=_cycalloc(sizeof(*_tmp7ED));_tmp7ED[0]=({
struct Cyc_Absyn_Aggr_p_struct _tmp7EE;_tmp7EE.tag=5;_tmp7EE.f1=({struct Cyc_Absyn_AggrInfo
_tmp7EF;_tmp7EF.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union
_tmp7F0;(_tmp7F0.UnknownAggr).tag=0;(_tmp7F0.UnknownAggr).f1=(void*)((void*)0);(
_tmp7F0.UnknownAggr).f2=Cyc_yyget_QualId_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 4)]);_tmp7F0;});_tmp7EF.targs=0;_tmp7EF;});_tmp7EE.f2=
_tmp7EB;_tmp7EE.f3=_tmp7E9;_tmp7EE.f4=_tmp7EA;_tmp7EE;});_tmp7ED;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7EC;});
break;}}case 374: _LL387: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp7F1;(_tmp7F1.YY11).tag=15;(_tmp7F1.YY11).f1=Cyc_Absyn_new_pat((void*)({
struct Cyc_Absyn_Pointer_p_struct*_tmp7F2=_cycalloc(sizeof(*_tmp7F2));_tmp7F2[0]=({
struct Cyc_Absyn_Pointer_p_struct _tmp7F3;_tmp7F3.tag=4;_tmp7F3.f1=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7F3;});_tmp7F2;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7F1;});break;case 375: _LL388: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7F4;(_tmp7F4.YY11).tag=15;(_tmp7F4.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp7F5=_cycalloc(sizeof(*_tmp7F5));
_tmp7F5[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp7F6;_tmp7F6.tag=4;_tmp7F6.f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Pointer_p_struct*_tmp7F7=_cycalloc(
sizeof(*_tmp7F7));_tmp7F7[0]=({struct Cyc_Absyn_Pointer_p_struct _tmp7F8;_tmp7F8.tag=
4;_tmp7F8.f1=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp7F8;});_tmp7F7;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7F6;});
_tmp7F5;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7F4;});break;case 376: _LL389: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp7F9;(_tmp7F9.YY11).tag=15;(_tmp7F9.YY11).f1=Cyc_Absyn_new_pat((
void*)({struct Cyc_Absyn_Reference_p_struct*_tmp7FA=_cycalloc(sizeof(*_tmp7FA));
_tmp7FA[0]=({struct Cyc_Absyn_Reference_p_struct _tmp7FB;_tmp7FB.tag=1;_tmp7FB.f1=
Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp7FC=_cycalloc(sizeof(*_tmp7FC));
_tmp7FC->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp7FE;(
_tmp7FE.Loc_n).tag=0;_tmp7FE;});_tmp7FC->f2=({struct _dynforward_ptr*_tmp7FD=
_cycalloc(sizeof(*_tmp7FD));_tmp7FD[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp7FD;});_tmp7FC;}),(void*)
0,0);_tmp7FB.f2=Cyc_Absyn_new_pat((void*)0,Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp7FB;});
_tmp7FA;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 1)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp7F9;});break;case 377: _LL38A: if(Cyc_strcmp((
struct _dynforward_ptr)Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)]),({const char*_tmp7FF="as";_tag_dynforward(_tmp7FF,
sizeof(char),_get_zero_arr_size_char(_tmp7FF,3));}))!= 0)Cyc_Parse_err(({const
char*_tmp800="expecting `as'";_tag_dynforward(_tmp800,sizeof(char),
_get_zero_arr_size_char(_tmp800,15));}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line));yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp801;(_tmp801.YY11).tag=15;(_tmp801.YY11).f1=
Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Reference_p_struct*_tmp802=_cycalloc(
sizeof(*_tmp802));_tmp802[0]=({struct Cyc_Absyn_Reference_p_struct _tmp803;_tmp803.tag=
1;_tmp803.f1=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp804=_cycalloc(sizeof(*
_tmp804));_tmp804->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp806;(_tmp806.Loc_n).tag=0;_tmp806;});_tmp804->f2=({struct _dynforward_ptr*
_tmp805=_cycalloc(sizeof(*_tmp805));_tmp805[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp805;});_tmp804;}),(
void*)0,0);_tmp803.f2=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp803;});_tmp802;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 2)]).last_line));_tmp801;});
break;case 378: _LL38B: {void*_tmp807=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Tcutil_kind_to_bound((
void*)7));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp808;(_tmp808.YY11).tag=
15;(_tmp808.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*
_tmp809=_cycalloc(sizeof(*_tmp809));_tmp809[0]=({struct Cyc_Absyn_TagInt_p_struct
_tmp80A;_tmp80A.tag=2;_tmp80A.f1=Cyc_Parse_typ2tvar(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line),_tmp807);
_tmp80A.f2=Cyc_Absyn_new_vardecl(({struct _tuple1*_tmp80B=_cycalloc(sizeof(*
_tmp80B));_tmp80B->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp80D;(_tmp80D.Loc_n).tag=0;_tmp80D;});_tmp80B->f2=({struct _dynforward_ptr*
_tmp80C=_cycalloc(sizeof(*_tmp80C));_tmp80C[0]=Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp80C;});_tmp80B;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp80E=_cycalloc(sizeof(*_tmp80E));
_tmp80E[0]=({struct Cyc_Absyn_TagType_struct _tmp80F;_tmp80F.tag=18;_tmp80F.f1=(
void*)_tmp807;_tmp80F;});_tmp80E;}),0);_tmp80A;});_tmp809;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp808;});
break;}case 379: _LL38C: {struct Cyc_Absyn_Tvar*_tmp810=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound((
void*)7));yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp811;(_tmp811.YY11).tag=
15;(_tmp811.YY11).f1=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_TagInt_p_struct*
_tmp812=_cycalloc(sizeof(*_tmp812));_tmp812[0]=({struct Cyc_Absyn_TagInt_p_struct
_tmp813;_tmp813.tag=2;_tmp813.f1=_tmp810;_tmp813.f2=Cyc_Absyn_new_vardecl(({
struct _tuple1*_tmp814=_cycalloc(sizeof(*_tmp814));_tmp814->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp816;(_tmp816.Loc_n).tag=0;_tmp816;});_tmp814->f2=({
struct _dynforward_ptr*_tmp815=_cycalloc(sizeof(*_tmp815));_tmp815[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp815;});_tmp814;}),(
void*)({struct Cyc_Absyn_TagType_struct*_tmp817=_cycalloc(sizeof(*_tmp817));
_tmp817[0]=({struct Cyc_Absyn_TagType_struct _tmp818;_tmp818.tag=18;_tmp818.f1=(
void*)((void*)({struct Cyc_Absyn_VarType_struct*_tmp819=_cycalloc(sizeof(*_tmp819));
_tmp819[0]=({struct Cyc_Absyn_VarType_struct _tmp81A;_tmp81A.tag=1;_tmp81A.f1=
_tmp810;_tmp81A;});_tmp819;}));_tmp818;});_tmp817;}),0);_tmp813;});_tmp812;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 3)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp811;});break;}case 380: _LL38D: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp81B;(_tmp81B.YY12).tag=16;(_tmp81B.YY12).f1=({struct
_tuple13*_tmp81C=_cycalloc(sizeof(*_tmp81C));_tmp81C->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]));_tmp81C->f2=0;_tmp81C;});
_tmp81B;});break;case 381: _LL38E: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp81D;(_tmp81D.YY12).tag=16;(_tmp81D.YY12).f1=({struct _tuple13*_tmp81E=
_cycalloc(sizeof(*_tmp81E));_tmp81E->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(Cyc_yyget_YY13(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 2)]));_tmp81E->f2=1;_tmp81E;});_tmp81D;});break;case 382: _LL38F:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp81F;(_tmp81F.YY12).tag=
16;(_tmp81F.YY12).f1=({struct _tuple13*_tmp820=_cycalloc(sizeof(*_tmp820));
_tmp820->f1=0;_tmp820->f2=1;_tmp820;});_tmp81F;});break;case 383: _LL390: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp821;(_tmp821.YY13).tag=17;(
_tmp821.YY13).f1=({struct Cyc_List_List*_tmp822=_cycalloc(sizeof(*_tmp822));
_tmp822->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp822->tl=0;_tmp822;});_tmp821;});break;case 384: _LL391: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp823;(_tmp823.YY13).tag=17;(
_tmp823.YY13).f1=({struct Cyc_List_List*_tmp824=_cycalloc(sizeof(*_tmp824));
_tmp824->hd=Cyc_yyget_YY11(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp824->tl=Cyc_yyget_YY13(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 2)]);_tmp824;});_tmp823;});break;case 385: _LL392: yyval=(union
Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp825;(_tmp825.YY14).tag=18;(_tmp825.YY14).f1=({
struct _tuple14*_tmp826=_cycalloc(sizeof(*_tmp826));_tmp826->f1=0;_tmp826->f2=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp826;});_tmp825;});
break;case 386: _LL393: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp827;(_tmp827.YY14).tag=18;(_tmp827.YY14).f1=({struct _tuple14*_tmp828=
_cycalloc(sizeof(*_tmp828));_tmp828->f1=Cyc_yyget_YY41(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp828->f2=Cyc_yyget_YY11(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp828;});_tmp827;});
break;case 387: _LL394: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp829;(_tmp829.YY16).tag=20;(_tmp829.YY16).f1=({struct _tuple13*_tmp82A=
_cycalloc(sizeof(*_tmp82A));_tmp82A->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]));_tmp82A->f2=0;_tmp82A;});_tmp829;});break;case 388: _LL395: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp82B;(_tmp82B.YY16).tag=20;(
_tmp82B.YY16).f1=({struct _tuple13*_tmp82C=_cycalloc(sizeof(*_tmp82C));_tmp82C->f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY15(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));_tmp82C->f2=1;_tmp82C;});
_tmp82B;});break;case 389: _LL396: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp82D;(_tmp82D.YY16).tag=20;(_tmp82D.YY16).f1=({struct _tuple13*_tmp82E=
_cycalloc(sizeof(*_tmp82E));_tmp82E->f1=0;_tmp82E->f2=1;_tmp82E;});_tmp82D;});
break;case 390: _LL397: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp82F;(_tmp82F.YY15).tag=19;(_tmp82F.YY15).f1=({struct Cyc_List_List*_tmp830=
_cycalloc(sizeof(*_tmp830));_tmp830->hd=Cyc_yyget_YY14(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp830->tl=0;_tmp830;});
_tmp82F;});break;case 391: _LL398: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp831;(_tmp831.YY15).tag=19;(_tmp831.YY15).f1=({struct Cyc_List_List*_tmp832=
_cycalloc(sizeof(*_tmp832));_tmp832->hd=Cyc_yyget_YY14(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp832->tl=Cyc_yyget_YY15(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp832;});_tmp831;});
break;case 392: _LL399: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 393: _LL39A: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp833;(_tmp833.YY3).tag=7;(_tmp833.YY3).f1=Cyc_Absyn_seq_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp833;});
break;case 394: _LL39B: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 395: _LL39C: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp834;(_tmp834.YY3).tag=7;(_tmp834.YY3).f1=Cyc_Absyn_assignop_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY8(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp834;});
break;case 396: _LL39D: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp835;(_tmp835.YY8).tag=12;(_tmp835.YY8).f1=0;_tmp835;});break;case 397: _LL39E:
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp836;(_tmp836.YY8).tag=
12;(_tmp836.YY8).f1=({struct Cyc_Core_Opt*_tmp837=_cycalloc(sizeof(*_tmp837));
_tmp837->v=(void*)((void*)1);_tmp837;});_tmp836;});break;case 398: _LL39F: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp838;(_tmp838.YY8).tag=12;(
_tmp838.YY8).f1=({struct Cyc_Core_Opt*_tmp839=_cycalloc(sizeof(*_tmp839));_tmp839->v=(
void*)((void*)3);_tmp839;});_tmp838;});break;case 399: _LL3A0: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp83A;(_tmp83A.YY8).tag=12;(_tmp83A.YY8).f1=({struct Cyc_Core_Opt*
_tmp83B=_cycalloc(sizeof(*_tmp83B));_tmp83B->v=(void*)((void*)4);_tmp83B;});
_tmp83A;});break;case 400: _LL3A1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp83C;(_tmp83C.YY8).tag=12;(_tmp83C.YY8).f1=({struct Cyc_Core_Opt*_tmp83D=
_cycalloc(sizeof(*_tmp83D));_tmp83D->v=(void*)((void*)0);_tmp83D;});_tmp83C;});
break;case 401: _LL3A2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp83E;(_tmp83E.YY8).tag=12;(_tmp83E.YY8).f1=({struct Cyc_Core_Opt*_tmp83F=
_cycalloc(sizeof(*_tmp83F));_tmp83F->v=(void*)((void*)2);_tmp83F;});_tmp83E;});
break;case 402: _LL3A3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp840;(_tmp840.YY8).tag=12;(_tmp840.YY8).f1=({struct Cyc_Core_Opt*_tmp841=
_cycalloc(sizeof(*_tmp841));_tmp841->v=(void*)((void*)16);_tmp841;});_tmp840;});
break;case 403: _LL3A4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp842;(_tmp842.YY8).tag=12;(_tmp842.YY8).f1=({struct Cyc_Core_Opt*_tmp843=
_cycalloc(sizeof(*_tmp843));_tmp843->v=(void*)((void*)17);_tmp843;});_tmp842;});
break;case 404: _LL3A5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp844;(_tmp844.YY8).tag=12;(_tmp844.YY8).f1=({struct Cyc_Core_Opt*_tmp845=
_cycalloc(sizeof(*_tmp845));_tmp845->v=(void*)((void*)13);_tmp845;});_tmp844;});
break;case 405: _LL3A6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp846;(_tmp846.YY8).tag=12;(_tmp846.YY8).f1=({struct Cyc_Core_Opt*_tmp847=
_cycalloc(sizeof(*_tmp847));_tmp847->v=(void*)((void*)15);_tmp847;});_tmp846;});
break;case 406: _LL3A7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp848;(_tmp848.YY8).tag=12;(_tmp848.YY8).f1=({struct Cyc_Core_Opt*_tmp849=
_cycalloc(sizeof(*_tmp849));_tmp849->v=(void*)((void*)14);_tmp849;});_tmp848;});
break;case 407: _LL3A8: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 408: _LL3A9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84A;(_tmp84A.YY3).tag=7;(_tmp84A.YY3).f1=Cyc_Absyn_conditional_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84A;});
break;case 409: _LL3AA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84B;(_tmp84B.YY3).tag=7;(_tmp84B.YY3).f1=Cyc_Absyn_throw_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84B;});
break;case 410: _LL3AB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84C;(_tmp84C.YY3).tag=7;(_tmp84C.YY3).f1=Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84C;});
break;case 411: _LL3AC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84D;(_tmp84D.YY3).tag=7;(_tmp84D.YY3).f1=Cyc_Absyn_New_exp(0,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84D;});
break;case 412: _LL3AD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84E;(_tmp84E.YY3).tag=7;(_tmp84E.YY3).f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84E;});
break;case 413: _LL3AE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp84F;(_tmp84F.YY3).tag=7;(_tmp84F.YY3).f1=Cyc_Absyn_New_exp((struct Cyc_Absyn_Exp*)
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp84F;});
break;case 414: _LL3AF: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 415: _LL3B0: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 416: _LL3B1: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp850;(_tmp850.YY3).tag=7;(_tmp850.YY3).f1=Cyc_Absyn_or_exp(
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp850;});
break;case 417: _LL3B2: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 418: _LL3B3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp851;(_tmp851.YY3).tag=7;(_tmp851.YY3).f1=Cyc_Absyn_and_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp851;});
break;case 419: _LL3B4: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 420: _LL3B5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp852;(_tmp852.YY3).tag=7;(_tmp852.YY3).f1=Cyc_Absyn_prim2_exp((void*)14,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp852;});
break;case 421: _LL3B6: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 422: _LL3B7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp853;(_tmp853.YY3).tag=7;(_tmp853.YY3).f1=Cyc_Absyn_prim2_exp((void*)15,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp853;});
break;case 423: _LL3B8: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 424: _LL3B9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp854;(_tmp854.YY3).tag=7;(_tmp854.YY3).f1=Cyc_Absyn_prim2_exp((void*)13,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp854;});
break;case 425: _LL3BA: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 426: _LL3BB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp855;(_tmp855.YY3).tag=7;(_tmp855.YY3).f1=Cyc_Absyn_eq_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp855;});
break;case 427: _LL3BC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp856;(_tmp856.YY3).tag=7;(_tmp856.YY3).f1=Cyc_Absyn_neq_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp856;});
break;case 428: _LL3BD: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 429: _LL3BE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp857;(_tmp857.YY3).tag=7;(_tmp857.YY3).f1=Cyc_Absyn_lt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp857;});
break;case 430: _LL3BF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp858;(_tmp858.YY3).tag=7;(_tmp858.YY3).f1=Cyc_Absyn_gt_exp(Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp858;});
break;case 431: _LL3C0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp859;(_tmp859.YY3).tag=7;(_tmp859.YY3).f1=Cyc_Absyn_lte_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp859;});
break;case 432: _LL3C1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp85A;(_tmp85A.YY3).tag=7;(_tmp85A.YY3).f1=Cyc_Absyn_gte_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85A;});
break;case 433: _LL3C2: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 434: _LL3C3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp85B;(_tmp85B.YY3).tag=7;(_tmp85B.YY3).f1=Cyc_Absyn_prim2_exp((void*)16,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85B;});
break;case 435: _LL3C4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp85C;(_tmp85C.YY3).tag=7;(_tmp85C.YY3).f1=Cyc_Absyn_prim2_exp((void*)17,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85C;});
break;case 436: _LL3C5: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 437: _LL3C6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp85D;(_tmp85D.YY3).tag=7;(_tmp85D.YY3).f1=Cyc_Absyn_prim2_exp((void*)0,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85D;});
break;case 438: _LL3C7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp85E;(_tmp85E.YY3).tag=7;(_tmp85E.YY3).f1=Cyc_Absyn_prim2_exp((void*)2,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85E;});
break;case 439: _LL3C8: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 440: _LL3C9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp85F;(_tmp85F.YY3).tag=7;(_tmp85F.YY3).f1=Cyc_Absyn_prim2_exp((void*)1,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp85F;});
break;case 441: _LL3CA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp860;(_tmp860.YY3).tag=7;(_tmp860.YY3).f1=Cyc_Absyn_prim2_exp((void*)3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp860;});
break;case 442: _LL3CB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp861;(_tmp861.YY3).tag=7;(_tmp861.YY3).f1=Cyc_Absyn_prim2_exp((void*)4,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp861;});
break;case 443: _LL3CC: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 444: _LL3CD: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp862;(_tmp862.YY3).tag=7;(_tmp862.YY3).f1=Cyc_Absyn_cast_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)])).f3,Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),1,(void*)0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp862;});
break;case 445: _LL3CE: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 446: _LL3CF: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp863;(_tmp863.YY3).tag=7;(_tmp863.YY3).f1=Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp863;});
break;case 447: _LL3D0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp864;(_tmp864.YY3).tag=7;(_tmp864.YY3).f1=Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp864;});
break;case 448: _LL3D1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp865;(_tmp865.YY3).tag=7;(_tmp865.YY3).f1=Cyc_Absyn_address_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp865;});
break;case 449: _LL3D2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp866;(_tmp866.YY3).tag=7;(_tmp866.YY3).f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp866;});
break;case 450: _LL3D3: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 451: _LL3D4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp867;(_tmp867.YY3).tag=7;(_tmp867.YY3).f1=Cyc_Absyn_prim1_exp(Cyc_yyget_YY7(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp867;});
break;case 452: _LL3D5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp868;(_tmp868.YY3).tag=7;(_tmp868.YY3).f1=Cyc_Absyn_sizeoftyp_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp868;});
break;case 453: _LL3D6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp869;(_tmp869.YY3).tag=7;(_tmp869.YY3).f1=Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp869;});
break;case 454: _LL3D7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp86A;(_tmp86A.YY3).tag=7;(_tmp86A.YY3).f1=Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(void*)({struct
Cyc_Absyn_StructField_struct*_tmp86B=_cycalloc(sizeof(*_tmp86B));_tmp86B[0]=({
struct Cyc_Absyn_StructField_struct _tmp86C;_tmp86C.tag=0;_tmp86C.f1=({struct
_dynforward_ptr*_tmp86D=_cycalloc(sizeof(*_tmp86D));_tmp86D[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp86D;});_tmp86C;});
_tmp86B;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp86A;});break;case 455: _LL3D8: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp86E;(_tmp86E.YY3).tag=7;(_tmp86E.YY3).f1=Cyc_Absyn_offsetof_exp((*
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)])).f3,(
void*)({struct Cyc_Absyn_TupleIndex_struct*_tmp86F=_cycalloc_atomic(sizeof(*
_tmp86F));_tmp86F[0]=({struct Cyc_Absyn_TupleIndex_struct _tmp870;_tmp870.tag=1;
_tmp870.f1=(unsigned int)(Cyc_yyget_Int_tok(yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset - 1)])).f2;_tmp870;});_tmp86F;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp86E;});
break;case 456: _LL3D9: {struct Cyc_Position_Segment*_tmp871=Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line);struct Cyc_List_List*
_tmp872=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,
_tmp871,Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]));
yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp873;(_tmp873.YY3).tag=
7;(_tmp873.YY3).f1=Cyc_Absyn_gentyp_exp(_tmp872,(*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)])).f3,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp873;});
break;}case 457: _LL3DA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp874;(_tmp874.YY3).tag=7;(_tmp874.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Malloc_e_struct*_tmp875=_cycalloc(sizeof(*_tmp875));_tmp875[0]=({
struct Cyc_Absyn_Malloc_e_struct _tmp876;_tmp876.tag=35;_tmp876.f1=({struct Cyc_Absyn_MallocInfo
_tmp877;_tmp877.is_calloc=0;_tmp877.rgn=0;_tmp877.elt_type=0;_tmp877.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);
_tmp877.fat_result=0;_tmp877;});_tmp876;});_tmp875;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp874;});
break;case 458: _LL3DB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp878;(_tmp878.YY3).tag=7;(_tmp878.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Malloc_e_struct*_tmp879=_cycalloc(sizeof(*_tmp879));_tmp879[0]=({
struct Cyc_Absyn_Malloc_e_struct _tmp87A;_tmp87A.tag=35;_tmp87A.f1=({struct Cyc_Absyn_MallocInfo
_tmp87B;_tmp87B.is_calloc=0;_tmp87B.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp87B.elt_type=0;
_tmp87B.num_elts=Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)]);_tmp87B.fat_result=0;_tmp87B;});_tmp87A;});_tmp879;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 5)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp878;});
break;case 459: _LL3DC: {void*_tmp87D;struct _tuple2 _tmp87C=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp87D=_tmp87C.f3;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp87E;(_tmp87E.YY3).tag=7;(
_tmp87E.YY3).f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp87F=_cycalloc(sizeof(*_tmp87F));_tmp87F[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp880;_tmp880.tag=35;_tmp880.f1=({struct Cyc_Absyn_MallocInfo _tmp881;_tmp881.is_calloc=
1;_tmp881.rgn=0;_tmp881.elt_type=({void**_tmp882=_cycalloc(sizeof(*_tmp882));
_tmp882[0]=_tmp87D;_tmp882;});_tmp881.num_elts=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);_tmp881.fat_result=0;
_tmp881;});_tmp880;});_tmp87F;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 8)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp87E;});
break;}case 460: _LL3DD: {void*_tmp884;struct _tuple2 _tmp883=*Cyc_yyget_YY37(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp884=_tmp883.f3;yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp885;(_tmp885.YY3).tag=7;(
_tmp885.YY3).f1=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Malloc_e_struct*
_tmp886=_cycalloc(sizeof(*_tmp886));_tmp886[0]=({struct Cyc_Absyn_Malloc_e_struct
_tmp887;_tmp887.tag=35;_tmp887.f1=({struct Cyc_Absyn_MallocInfo _tmp888;_tmp888.is_calloc=
1;_tmp888.rgn=(struct Cyc_Absyn_Exp*)Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 8)]);_tmp888.elt_type=({void**
_tmp889=_cycalloc(sizeof(*_tmp889));_tmp889[0]=_tmp884;_tmp889;});_tmp888.num_elts=
Cyc_yyget_YY3(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 6)]);
_tmp888.fat_result=0;_tmp888;});_tmp887;});_tmp886;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 10)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp885;});
break;}case 461: _LL3DE: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp88A;(_tmp88A.YY3).tag=7;(_tmp88A.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Swap_e_struct*_tmp88B=_cycalloc(sizeof(*_tmp88B));_tmp88B[0]=({struct
Cyc_Absyn_Swap_e_struct _tmp88C;_tmp88C.tag=36;_tmp88C.f1=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 3)]);_tmp88C.f2=Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]);_tmp88C;});_tmp88B;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp88A;});break;case 462: _LL3DF: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp88D;(_tmp88D.YY7).tag=11;(_tmp88D.YY7).f1=(void*)((
void*)12);_tmp88D;});break;case 463: _LL3E0: yyval=(union Cyc_YYSTYPE_union)({union
Cyc_YYSTYPE_union _tmp88E;(_tmp88E.YY7).tag=11;(_tmp88E.YY7).f1=(void*)((void*)11);
_tmp88E;});break;case 464: _LL3E1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp88F;(_tmp88F.YY7).tag=11;(_tmp88F.YY7).f1=(void*)((void*)2);_tmp88F;});
break;case 465: _LL3E2: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 466: _LL3E3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp890;(_tmp890.YY3).tag=7;(_tmp890.YY3).f1=Cyc_Absyn_subscript_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp890;});
break;case 467: _LL3E4: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp891;(_tmp891.YY3).tag=7;(_tmp891.YY3).f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),0,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp891;});
break;case 468: _LL3E5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp892;(_tmp892.YY3).tag=7;(_tmp892.YY3).f1=Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 3)]),Cyc_yyget_YY5(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp892;});
break;case 469: _LL3E6: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp893;(_tmp893.YY3).tag=7;(_tmp893.YY3).f1=Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct
_dynforward_ptr*_tmp894=_cycalloc(sizeof(*_tmp894));_tmp894[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp894;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp893;});
break;case 470: _LL3E7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp895;(_tmp895.YY3).tag=7;(_tmp895.YY3).f1=Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),({struct
_dynforward_ptr*_tmp896=_cycalloc(sizeof(*_tmp896));_tmp896[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp896;}),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 2)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp895;});
break;case 471: _LL3E8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp897;(_tmp897.YY3).tag=7;(_tmp897.YY3).f1=Cyc_Absyn_post_inc_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp897;});
break;case 472: _LL3E9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp898;(_tmp898.YY3).tag=7;(_tmp898.YY3).f1=Cyc_Absyn_post_dec_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp898;});
break;case 473: _LL3EA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp899;(_tmp899.YY3).tag=7;(_tmp899.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_CompoundLit_e_struct*_tmp89A=_cycalloc(sizeof(*_tmp89A));_tmp89A[0]=({
struct Cyc_Absyn_CompoundLit_e_struct _tmp89B;_tmp89B.tag=27;_tmp89B.f1=Cyc_yyget_YY37(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp89B.f2=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp89B;});_tmp89A;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset - 5)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp899;});break;case 474: _LL3EB: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp89C;(_tmp89C.YY3).tag=7;(_tmp89C.YY3).f1=Cyc_Absyn_new_exp((
void*)({struct Cyc_Absyn_CompoundLit_e_struct*_tmp89D=_cycalloc(sizeof(*_tmp89D));
_tmp89D[0]=({struct Cyc_Absyn_CompoundLit_e_struct _tmp89E;_tmp89E.tag=27;_tmp89E.f1=
Cyc_yyget_YY37(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 5)]);
_tmp89E.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
Cyc_yyget_YY6(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]));
_tmp89E;});_tmp89D;}),Cyc_Position_segment_of_abs((yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 6)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp89C;});
break;case 475: _LL3EC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp89F;(_tmp89F.YY3).tag=7;(_tmp89F.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_UnknownId_e_struct*_tmp8A0=_cycalloc(sizeof(*_tmp8A0));_tmp8A0[0]=({
struct Cyc_Absyn_UnknownId_e_struct _tmp8A1;_tmp8A1.tag=2;_tmp8A1.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8A1;});_tmp8A0;}),
Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp89F;});break;case 476: _LL3ED: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 477: _LL3EE: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp8A2;(_tmp8A2.YY3).tag=7;(
_tmp8A2.YY3).f1=Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A2;});
break;case 478: _LL3EF: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset - 1)];break;case 479: _LL3F0: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A3;(_tmp8A3.YY3).tag=7;(_tmp8A3.YY3).f1=Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A3;});
break;case 480: _LL3F1: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A4;(_tmp8A4.YY3).tag=7;(_tmp8A4.YY3).f1=Cyc_Absyn_instantiate_exp(Cyc_yyget_YY3(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]),Cyc_List_imp_rev(
Cyc_yyget_YY40(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)])),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A4;});
break;case 481: _LL3F2: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A5;(_tmp8A5.YY3).tag=7;(_tmp8A5.YY3).f1=Cyc_Absyn_tuple_exp(Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 1)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 3)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A5;});
break;case 482: _LL3F3: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8A6;(_tmp8A6.YY3).tag=7;(_tmp8A6.YY3).f1=Cyc_Absyn_new_exp((void*)({struct
Cyc_Absyn_Struct_e_struct*_tmp8A7=_cycalloc(sizeof(*_tmp8A7));_tmp8A7[0]=({
struct Cyc_Absyn_Struct_e_struct _tmp8A8;_tmp8A8.tag=30;_tmp8A8.f1=Cyc_yyget_QualId_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 4)]);_tmp8A8.f2=Cyc_yyget_YY40(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp8A8.f3=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset - 1)]));_tmp8A8.f4=0;_tmp8A8;});
_tmp8A7;}),Cyc_Position_segment_of_abs((yyls[_check_known_subscript_notnull(
10000,yylsp_offset - 4)]).first_line,(yyls[_check_known_subscript_notnull(10000,
yylsp_offset)]).last_line));_tmp8A6;});break;case 483: _LL3F4: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp8A9;(_tmp8A9.YY3).tag=7;(_tmp8A9.YY3).f1=Cyc_Absyn_stmt_exp(
Cyc_yyget_YY9(yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 4)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8A9;});
break;case 484: _LL3F5: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8AA;(_tmp8AA.YY5).tag=9;(_tmp8AA.YY5).f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]));_tmp8AA;});break;case 485: _LL3F6: yyval=(union Cyc_YYSTYPE_union)({
union Cyc_YYSTYPE_union _tmp8AB;(_tmp8AB.YY5).tag=9;(_tmp8AB.YY5).f1=({struct Cyc_List_List*
_tmp8AC=_cycalloc(sizeof(*_tmp8AC));_tmp8AC->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8AC->tl=0;_tmp8AC;});
_tmp8AB;});break;case 486: _LL3F7: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8AD;(_tmp8AD.YY5).tag=9;(_tmp8AD.YY5).f1=({struct Cyc_List_List*_tmp8AE=
_cycalloc(sizeof(*_tmp8AE));_tmp8AE->hd=Cyc_yyget_YY3(yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8AE->tl=Cyc_yyget_YY5(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset - 2)]);_tmp8AE;});_tmp8AD;});
break;case 487: _LL3F8: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8AF;(_tmp8AF.YY3).tag=7;(_tmp8AF.YY3).f1=Cyc_Absyn_int_exp((Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f1,(Cyc_yyget_Int_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)])).f2,Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8AF;});
break;case 488: _LL3F9: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8B0;(_tmp8B0.YY3).tag=7;(_tmp8B0.YY3).f1=Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B0;});
break;case 489: _LL3FA: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8B1;(_tmp8B1.YY3).tag=7;(_tmp8B1.YY3).f1=Cyc_Absyn_float_exp(Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]),Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B1;});
break;case 490: _LL3FB: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8B2;(_tmp8B2.YY3).tag=7;(_tmp8B2.YY3).f1=Cyc_Absyn_null_exp(Cyc_Position_segment_of_abs((
yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line,(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line));_tmp8B2;});
break;case 491: _LL3FC: yyval=(union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union
_tmp8B3;(_tmp8B3.QualId_tok).tag=4;(_tmp8B3.QualId_tok).f1=({struct _tuple1*
_tmp8B4=_cycalloc(sizeof(*_tmp8B4));_tmp8B4->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp8B6;(_tmp8B6.Rel_n).tag=1;(_tmp8B6.Rel_n).f1=0;
_tmp8B6;});_tmp8B4->f2=({struct _dynforward_ptr*_tmp8B5=_cycalloc(sizeof(*_tmp8B5));
_tmp8B5[0]=Cyc_yyget_String_tok(yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)]);_tmp8B5;});_tmp8B4;});_tmp8B3;});break;case 492: _LL3FD: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 493: _LL3FE: yyval=(
union Cyc_YYSTYPE_union)({union Cyc_YYSTYPE_union _tmp8B7;(_tmp8B7.QualId_tok).tag=
4;(_tmp8B7.QualId_tok).f1=({struct _tuple1*_tmp8B8=_cycalloc(sizeof(*_tmp8B8));
_tmp8B8->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp8BA;(
_tmp8BA.Rel_n).tag=1;(_tmp8BA.Rel_n).f1=0;_tmp8BA;});_tmp8B8->f2=({struct
_dynforward_ptr*_tmp8B9=_cycalloc(sizeof(*_tmp8B9));_tmp8B9[0]=Cyc_yyget_String_tok(
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)]);_tmp8B9;});_tmp8B8;});
_tmp8B7;});break;case 494: _LL3FF: yyval=yyvs[_check_known_subscript_notnull(10000,
yyvsp_offset)];break;case 495: _LL400: yyval=yyvs[_check_known_subscript_notnull(
10000,yyvsp_offset)];break;case 496: _LL401: yyval=yyvs[
_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 497: _LL402: yyval=
yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 498: _LL403:
yyval=yyvs[_check_known_subscript_notnull(10000,yyvsp_offset)];break;case 499:
_LL404: break;case 500: _LL405: yylex_buf->lex_curr_pos -=1;break;default: _LL406:
break;}yyvsp_offset -=yylen;yyssp_offset -=yylen;yylsp_offset -=yylen;yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=yyval;yylsp_offset ++;if(
yylen == 0){(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_line=
Cyc_yylloc.first_line;(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).first_column=
Cyc_yylloc.first_column;(yyls[_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(
yyls[_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_line;(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_column=(yyls[
_check_known_subscript_notnull(10000,yylsp_offset - 1)]).last_column;}else{(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_line=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_line;(yyls[
_check_known_subscript_notnull(10000,yylsp_offset)]).last_column=(yyls[
_check_known_subscript_notnull(10000,(yylsp_offset + yylen)- 1)]).last_column;}
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(501,yyn)];yystate=Cyc_yypgoto[
_check_known_subscript_notnull(142,yyn - 133)]+ yyss[
_check_known_subscript_notnull(10000,yyssp_offset)];if((yystate >= 0  && yystate <= 
6421) && Cyc_yycheck[_check_known_subscript_notnull(6422,yystate)]== yyss[
_check_known_subscript_notnull(10000,yyssp_offset)])yystate=(int)Cyc_yytable[
_check_known_subscript_notnull(6422,yystate)];else{yystate=(int)Cyc_yydefgoto[
_check_known_subscript_notnull(142,yyn - 133)];}goto yynewstate;yyerrlab: if(
yyerrstatus == 0){++ Cyc_yynerrs;yyn=(int)Cyc_yypact[
_check_known_subscript_notnull(1007,yystate)];if(yyn > - 32768  && yyn < 6421){int
sze=0;struct _dynforward_ptr msg;int x;int count;count=0;for(x=yyn < 0?- yyn: 0;x < 275 / 
sizeof(char*);x ++){if(Cyc_yycheck[_check_known_subscript_notnull(6422,x + yyn)]== 
x)(sze +=Cyc_strlen((struct _dynforward_ptr)Cyc_yytname[
_check_known_subscript_notnull(275,x)])+ 15,count ++);}msg=({unsigned int _tmp8BB=(
unsigned int)(sze + 15);char*_tmp8BC=(char*)_region_malloc(yyregion,_check_times(
sizeof(char),_tmp8BB + 1));struct _dynforward_ptr _tmp8BE=_tag_dynforward(_tmp8BC,
sizeof(char),_tmp8BB + 1);{unsigned int _tmp8BD=_tmp8BB;unsigned int i;for(i=0;i < 
_tmp8BD;i ++){_tmp8BC[i]='\000';}_tmp8BC[_tmp8BD]=(char)0;}_tmp8BE;});Cyc_strcpy(
msg,({const char*_tmp8BF="parse error";_tag_dynforward(_tmp8BF,sizeof(char),
_get_zero_arr_size_char(_tmp8BF,12));}));if(count < 5){count=0;for(x=yyn < 0?- yyn:
0;x < 275 / sizeof(char*);x ++){if(Cyc_yycheck[_check_known_subscript_notnull(6422,
x + yyn)]== x){Cyc_strcat(msg,(struct _dynforward_ptr)(count == 0?(struct
_dynforward_ptr)({const char*_tmp8C0=", expecting `";_tag_dynforward(_tmp8C0,
sizeof(char),_get_zero_arr_size_char(_tmp8C0,14));}):(struct _dynforward_ptr)({
const char*_tmp8C1=" or `";_tag_dynforward(_tmp8C1,sizeof(char),
_get_zero_arr_size_char(_tmp8C1,6));})));Cyc_strcat(msg,(struct _dynforward_ptr)
Cyc_yytname[_check_known_subscript_notnull(275,x)]);Cyc_strcat(msg,({const char*
_tmp8C2="'";_tag_dynforward(_tmp8C2,sizeof(char),_get_zero_arr_size_char(_tmp8C2,
2));}));count ++;}}}Cyc_yyerror((struct _dynforward_ptr)msg);}else{Cyc_yyerror(({
const char*_tmp8C3="parse error";_tag_dynforward(_tmp8C3,sizeof(char),
_get_zero_arr_size_char(_tmp8C3,12));}));}}goto yyerrlab1;yyerrlab1: if(
yyerrstatus == 3){if(Cyc_yychar == 0){int _tmp8C4=1;_npop_handler(0);return _tmp8C4;}
Cyc_yychar=- 2;}yyerrstatus=3;goto yyerrhandle;yyerrdefault: yyerrpop: if(
yyssp_offset == 0){int _tmp8C5=1;_npop_handler(0);return _tmp8C5;}yyvsp_offset --;
yystate=(int)yyss[_check_known_subscript_notnull(10000,-- yyssp_offset)];
yylsp_offset --;yyerrhandle: yyn=(int)Cyc_yypact[_check_known_subscript_notnull(
1007,yystate)];if(yyn == - 32768)goto yyerrdefault;yyn +=1;if((yyn < 0  || yyn > 6421)
 || Cyc_yycheck[_check_known_subscript_notnull(6422,yyn)]!= 1)goto yyerrdefault;
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6422,yyn)];if(yyn < 0){if(yyn
== - 32768)goto yyerrpop;yyn=- yyn;goto yyreduce;}else{if(yyn == 0)goto yyerrpop;}if(
yyn == 1006){int _tmp8C6=0;_npop_handler(0);return _tmp8C6;}yyvs[
_check_known_subscript_notnull(10000,++ yyvsp_offset)]=Cyc_yylval;yyls[
_check_known_subscript_notnull(10000,++ yylsp_offset)]=Cyc_yylloc;goto yynewstate;};
_pop_region(yyregion);}void Cyc_yyprint(int i,union Cyc_YYSTYPE_union v){union Cyc_YYSTYPE_union
_tmp8D0=v;struct _tuple11 _tmp8D1;int _tmp8D2;char _tmp8D3;struct _dynforward_ptr
_tmp8D4;struct _tuple1*_tmp8D5;struct _tuple1 _tmp8D6;union Cyc_Absyn_Nmspace_union
_tmp8D7;struct _dynforward_ptr*_tmp8D8;_LL409: if((_tmp8D0.Int_tok).tag != 0)goto
_LL40B;_tmp8D1=(_tmp8D0.Int_tok).f1;_tmp8D2=_tmp8D1.f2;_LL40A:({struct Cyc_Int_pa_struct
_tmp8DB;_tmp8DB.tag=1;_tmp8DB.f1=(unsigned long)_tmp8D2;{void*_tmp8D9[1]={&
_tmp8DB};Cyc_fprintf(Cyc_stderr,({const char*_tmp8DA="%d";_tag_dynforward(_tmp8DA,
sizeof(char),_get_zero_arr_size_char(_tmp8DA,3));}),_tag_dynforward(_tmp8D9,
sizeof(void*),1));}});goto _LL408;_LL40B: if((_tmp8D0.Char_tok).tag != 1)goto _LL40D;
_tmp8D3=(_tmp8D0.Char_tok).f1;_LL40C:({struct Cyc_Int_pa_struct _tmp8DE;_tmp8DE.tag=
1;_tmp8DE.f1=(unsigned long)((int)_tmp8D3);{void*_tmp8DC[1]={& _tmp8DE};Cyc_fprintf(
Cyc_stderr,({const char*_tmp8DD="%c";_tag_dynforward(_tmp8DD,sizeof(char),
_get_zero_arr_size_char(_tmp8DD,3));}),_tag_dynforward(_tmp8DC,sizeof(void*),1));}});
goto _LL408;_LL40D: if((_tmp8D0.String_tok).tag != 2)goto _LL40F;_tmp8D4=(_tmp8D0.String_tok).f1;
_LL40E:({struct Cyc_String_pa_struct _tmp8E1;_tmp8E1.tag=0;_tmp8E1.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)_tmp8D4);{void*_tmp8DF[1]={& _tmp8E1};Cyc_fprintf(
Cyc_stderr,({const char*_tmp8E0="\"%s\"";_tag_dynforward(_tmp8E0,sizeof(char),
_get_zero_arr_size_char(_tmp8E0,5));}),_tag_dynforward(_tmp8DF,sizeof(void*),1));}});
goto _LL408;_LL40F: if((_tmp8D0.QualId_tok).tag != 4)goto _LL411;_tmp8D5=(_tmp8D0.QualId_tok).f1;
_tmp8D6=*_tmp8D5;_tmp8D7=_tmp8D6.f1;_tmp8D8=_tmp8D6.f2;_LL410: {struct Cyc_List_List*
_tmp8E2=0;{union Cyc_Absyn_Nmspace_union _tmp8E3=_tmp8D7;struct Cyc_List_List*
_tmp8E4;struct Cyc_List_List*_tmp8E5;_LL414: if((_tmp8E3.Rel_n).tag != 1)goto _LL416;
_tmp8E4=(_tmp8E3.Rel_n).f1;_LL415: _tmp8E2=_tmp8E4;goto _LL413;_LL416: if((_tmp8E3.Abs_n).tag
!= 2)goto _LL418;_tmp8E5=(_tmp8E3.Abs_n).f1;_LL417: _tmp8E2=_tmp8E5;goto _LL413;
_LL418: if((_tmp8E3.Loc_n).tag != 0)goto _LL413;_LL419: goto _LL413;_LL413:;}for(0;
_tmp8E2 != 0;_tmp8E2=_tmp8E2->tl){({struct Cyc_String_pa_struct _tmp8E8;_tmp8E8.tag=
0;_tmp8E8.f1=(struct _dynforward_ptr)((struct _dynforward_ptr)*((struct
_dynforward_ptr*)_tmp8E2->hd));{void*_tmp8E6[1]={& _tmp8E8};Cyc_fprintf(Cyc_stderr,({
const char*_tmp8E7="%s::";_tag_dynforward(_tmp8E7,sizeof(char),
_get_zero_arr_size_char(_tmp8E7,5));}),_tag_dynforward(_tmp8E6,sizeof(void*),1));}});}({
struct Cyc_String_pa_struct _tmp8EB;_tmp8EB.tag=0;_tmp8EB.f1=(struct
_dynforward_ptr)((struct _dynforward_ptr)*_tmp8D8);{void*_tmp8E9[1]={& _tmp8EB};
Cyc_fprintf(Cyc_stderr,({const char*_tmp8EA="%s::";_tag_dynforward(_tmp8EA,
sizeof(char),_get_zero_arr_size_char(_tmp8EA,5));}),_tag_dynforward(_tmp8E9,
sizeof(void*),1));}});goto _LL408;}_LL411:;_LL412:({void*_tmp8EC=0;Cyc_fprintf(
Cyc_stderr,({const char*_tmp8ED="?";_tag_dynforward(_tmp8ED,sizeof(char),
_get_zero_arr_size_char(_tmp8ED,2));}),_tag_dynforward(_tmp8EC,sizeof(void*),0));});
goto _LL408;_LL408:;}struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*
f){Cyc_Parse_parse_result=0;Cyc_yyparse(Cyc_Lexing_from_file(f));return Cyc_Parse_parse_result;}
