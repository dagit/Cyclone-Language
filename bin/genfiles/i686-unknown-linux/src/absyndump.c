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
#else
#define _check_dyneither_subscript(arr,elt_sz,index) ({ \
  struct _dyneither_ptr _cus_arr = (arr); \
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
  /* JGM: not needed! if (!_cus_arr.base) _throw_null(); */ 
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one)
    _throw_arraybounds();
  return _cus_ans;
}
#else
#define _check_dyneither_subscript(arr,elt_sz,index) ({ \
  struct _dyneither_ptr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  /* JGM: not needed! if (!_cus_arr.base) _throw_null();*/ \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
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
#else
#define _tag_dyneither(tcurr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr _tag_arr_ans; \
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
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
#else
#define _init_dyneither_ptr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr *_itarr_ptr = (arr_ptr); \
  void* _itarr = (arr); \
  _itarr_ptr->base = _itarr_ptr->curr = _itarr; \
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts); \
  _itarr_ptr; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
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
#else
#define _untag_dyneither_ptr(arr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if (_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one)\
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
#else
#define _get_dyneither_size(arr,elt_sz) \
  ({struct _dyneither_ptr _get_arr_size_temp = (arr); \
    unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr; \
    unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one; \
    (_get_arr_size_curr < _get_arr_size_temp.base || \
     _get_arr_size_curr >= _get_arr_size_last) ? 0 : \
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));})
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_dyneither_ptr_plus(struct _dyneither_ptr arr,unsigned elt_sz,int change) {
  struct _dyneither_ptr _ans = (arr);
  _ans.curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _dyneither_ptr_plus(arr,elt_sz,change) ({ \
  struct _dyneither_ptr _ans = (arr); \
  _ans.curr += ((int)(elt_sz))*(change); \
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
#else
#define _dyneither_ptr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _dyneither_ptr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
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
#else
#define _dyneither_ptr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _dyneither_ptr * _arr_ptr = (arr_ptr); \
  struct _dyneither_ptr _ans = *_arr_ptr; \
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

/* the next two routines swap [x] and [y]; not thread safe! */
static _INLINE void _swap_word(void *x, void *y) {
  unsigned long *lx = (unsigned long *)x, *ly = (unsigned long *)y, tmp;
  tmp = *lx;
  *lx = *ly;
  *ly = tmp;
}
static _INLINE void _swap_dyneither(struct _dyneither_ptr *x, 
				   struct _dyneither_ptr *y) {
  struct _dyneither_ptr tmp = *x;
  *x = *y;
  *y = tmp;
}
 struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};struct Cyc_Core_Opt{
void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];typedef struct{
int __count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;
typedef struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t
Cyc_fpos_t;struct Cyc___cycFILE;extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_Cstdio___abstractFILE;
struct Cyc_String_pa_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{
int tag;unsigned long f1;};struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{
int tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fputc(int,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dyneither_ptr f1;};int Cyc_file_string_write(struct Cyc___cycFILE*,
struct _dyneither_ptr src,int src_offset,int max_count);struct Cyc_List_List{void*hd;
struct Cyc_List_List*tl;};int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[
18];struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[
8];int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x);unsigned int Cyc_strlen(
struct _dyneither_ptr s);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;
struct _dyneither_ptr line;int line_no;int col;};extern char Cyc_Position_Exit[9];
struct Cyc_Position_Segment;struct Cyc_Position_Error{struct _dyneither_ptr source;
struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};extern char
Cyc_Position_Nocontext[14];struct _dyneither_ptr Cyc_Position_get_line_directive(
struct Cyc_Position_Segment*loc);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*
val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct
_union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct
_union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct
_union_Nmspace_Loc_n Loc_n;};union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;union Cyc_Absyn_Nmspace
Cyc_Absyn_Rel_n(struct Cyc_List_List*);union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(
struct Cyc_List_List*);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct
_dyneither_ptr*f2;};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int
q_restrict;int real_const;struct Cyc_Position_Segment*loc;};struct
_union_Constraint_Eq_constr{int tag;void*val;};struct
_union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct
_union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct
_union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr
Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_struct{
int tag;void*f1;};struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;
};struct Cyc_Absyn_Less_kb_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;};struct
Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*
ptr_loc;struct Cyc_Position_Segment*rgn_loc;struct Cyc_Position_Segment*zt_loc;};
struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*
bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};
struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts
ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_struct{int tag;struct Cyc_Absyn_Exp*f1;
};struct Cyc_Absyn_Region_ptrqual_struct{int tag;void*f1;};struct Cyc_Absyn_VarargInfo{
struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{
struct Cyc_List_List*tvars;struct Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*
args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*
rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{
struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{
int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct
_union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};
union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype
UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct
Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*
targs;struct Cyc_Core_Opt*rgn;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct
_tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct
_union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo
val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*
f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;
};union Cyc_Absyn_DatatypeFieldInfoU{struct
_union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct
_union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{
union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct
_tuple2{void*f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct
_union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct
_union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{
struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr
KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*
targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct
Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Position_Segment*
zt_loc;};struct Cyc_Absyn_Evar_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*
f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_Absyn_DatatypeType_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;}
;struct Cyc_Absyn_DatatypeFieldType_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo
f1;};struct Cyc_Absyn_PointerType_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};
struct Cyc_Absyn_IntType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_DoubleType_struct{
int tag;int f1;};struct Cyc_Absyn_ArrayType_struct{int tag;struct Cyc_Absyn_ArrayInfo
f1;};struct Cyc_Absyn_FnType_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_struct{int tag;struct Cyc_Absyn_AggrInfo
f1;};struct Cyc_Absyn_AnonAggrType_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_EnumType_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*
f2;};struct Cyc_Absyn_AnonEnumType_struct{int tag;struct Cyc_List_List*f1;};struct
Cyc_Absyn_RgnHandleType_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{int tag;struct _tuple0*
f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void**f4;};struct Cyc_Absyn_ValueofType_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_struct{int tag;void*f1;};
struct Cyc_Absyn_AccessEff_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};
struct Cyc_Absyn_NoTypes_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;};struct Cyc_Absyn_WithTypes_struct{int tag;struct Cyc_List_List*f1;int f2;struct
Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Regparm_att_struct{
int tag;int f1;};struct Cyc_Absyn_Aligned_att_struct{int tag;int f1;};struct Cyc_Absyn_Section_att_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Format_att_struct{int tag;void*f1;
int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{int tag;int f1;};struct Cyc_Absyn_Mode_att_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_struct{int tag;union
Cyc_Absyn_Constraint*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;struct Cyc_Position_Segment*
f3;};struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct
Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int tag;struct Cyc_Position_Segment*
f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct
_tuple3{void*f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};
struct _tuple4{void*f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4
val;};struct _tuple5{void*f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5
val;};struct _tuple6{void*f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;
struct _tuple6 val;};struct _union_Cnst_Float_c{int tag;struct _dyneither_ptr val;};
struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{
struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct
_union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct
_union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct
_union_Cnst_String_c String_c;};struct Cyc_Absyn_VarargCallInfo{int num_varargs;
struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_struct{
int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{int tag;
unsigned int f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;
void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_struct{
int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_struct{int tag;struct _tuple0*
f1;void*f2;};struct Cyc_Absyn_UnknownId_e_struct{int tag;struct _tuple0*f1;};struct
Cyc_Absyn_Primop_e_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};
struct Cyc_Absyn_Increment_e_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};
struct Cyc_Absyn_Conditional_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnknownCall_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_FnCall_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;
};struct Cyc_Absyn_Throw_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_struct{
int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;void*f4;};struct Cyc_Absyn_Address_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_struct{int tag;void*f1;};
struct Cyc_Absyn_Sizeofexp_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_struct{
int tag;void*f1;void*f2;};struct Cyc_Absyn_Gentyp_e_struct{int tag;struct Cyc_List_List*
f1;void*f2;};struct Cyc_Absyn_Deref_e_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_AggrMember_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{
int tag;struct Cyc_List_List*f1;};struct _tuple7{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple7*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;
};struct Cyc_Absyn_Comprehension_e_struct{int tag;struct Cyc_Absyn_Vardecl*f1;
struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_struct{
int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*
f4;};struct Cyc_Absyn_AnonStruct_e_struct{int tag;void*f1;struct Cyc_List_List*f2;}
;struct Cyc_Absyn_Datatype_e_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*
f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_struct{int tag;
struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};
struct Cyc_Absyn_AnonEnum_e_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*
f3;};struct Cyc_Absyn_Malloc_e_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};
struct Cyc_Absyn_Swap_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*
f2;};struct Cyc_Absyn_UnresolvedMem_e_struct{int tag;struct Cyc_Core_Opt*f1;struct
Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{int tag;struct Cyc_Absyn_Stmt*
f1;};struct Cyc_Absyn_Tagcheck_e_struct{int tag;struct Cyc_Absyn_Exp*f1;struct
_dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_struct{int tag;void*f1;};struct Cyc_Absyn_Exp{
struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*loc;void*annot;};
struct Cyc_Absyn_Exp_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct
_tuple8{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
int tag;struct _tuple8 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct _dyneither_ptr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tuple8 f2;struct _tuple8 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple8 f2;};struct Cyc_Absyn_TryCatch_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;struct Cyc_Position_Segment*
loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Var_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_struct{
int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_struct{int tag;
struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_struct{int tag;struct Cyc_Absyn_AggrInfo*
f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_struct{
int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct
Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Int_p_struct{int tag;void*f1;int f2;};
struct Cyc_Absyn_Char_p_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_struct{int
tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Enum_p_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_struct{int tag;void*
f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_struct{int tag;
struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_struct{int tag;struct _tuple0*f1;
struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_Pat{void*r;struct Cyc_Core_Opt*topt;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*
pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Global_b_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct
Cyc_Absyn_Funname_b_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_struct{int tag;struct
Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_Absyn_Vardecl{void*sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;
void*type;struct Cyc_Absyn_Exp*initializer;struct Cyc_Core_Opt*rgn;struct Cyc_List_List*
attributes;int escapes;};struct Cyc_Absyn_Fndecl{void*sc;int is_inline;struct
_tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*effect;void*ret_type;
struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;
struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;struct Cyc_Core_Opt*
cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*
attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct
Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{
void*kind;void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*
impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct
_tuple0*name;struct Cyc_List_List*typs;struct Cyc_Position_Segment*loc;void*sc;};
struct Cyc_Absyn_Datatypedecl{void*sc;struct _tuple0*name;struct Cyc_List_List*tvs;
struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct
_tuple0*name;struct Cyc_Absyn_Exp*tag;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Enumdecl{
void*sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{
struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*
kind;struct Cyc_Core_Opt*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_struct{int tag;struct Cyc_Absyn_Fndecl*
f1;};struct Cyc_Absyn_Let_d_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*
f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_struct{int tag;struct Cyc_List_List*
f1;};struct Cyc_Absyn_Region_d_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*
f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_struct{int tag;struct
Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_struct{
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_struct{int tag;struct Cyc_Absyn_Enumdecl*
f1;};struct Cyc_Absyn_Typedef_d_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};
struct Cyc_Absyn_Namespace_d_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Using_d_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_ExternC_d_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*
r;struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*
x);void*Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*
k,struct Cyc_Core_Opt*tenv);extern void*Cyc_Absyn_bounds_one;struct Cyc_Absyn_Exp*
Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Position_Segment*);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,struct Cyc_Position_Segment*);
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct _tuple9{void*f1;
struct _tuple0*f2;};struct _tuple9 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{
int expand_typedefs: 1;int qvar_to_Cids: 1;int add_cyc_prefix: 1;int to_VC: 1;int
decls_first: 1;int rewrite_temp_tvars: 1;int print_all_tvars: 1;int print_all_kinds: 1;
int print_all_effects: 1;int print_using_stmts: 1;int print_externC_stmts: 1;int
print_full_evars: 1;int print_zeroterm: 1;int generate_line_directives: 1;int
use_curr_namespace: 1;struct Cyc_List_List*curr_namespace;};void Cyc_Absynpp_set_params(
struct Cyc_Absynpp_Params*fs);int Cyc_Absynpp_is_anon_aggrtype(void*t);extern
struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr;int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_char_escape(char);struct _dyneither_ptr Cyc_Absynpp_string_escape(
struct _dyneither_ptr);struct _dyneither_ptr Cyc_Absynpp_prim2str(void*p);int Cyc_Absynpp_is_declaration(
struct Cyc_Absyn_Stmt*s);struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};struct _tuple7*Cyc_Absynpp_arg_mk_opt(struct _tuple10*arg);struct
_tuple11{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};struct _tuple11
Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);struct
_tuple12{unsigned int f1;int f2;};struct _tuple12 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*
dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char
Cyc_Set_Absent[11];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);
struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[12];extern
char Cyc_Dict_Absent[11];struct _tuple13{void*f1;void*f2;};struct _tuple13*Cyc_Dict_rchoose(
struct _RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple13*Cyc_Dict_rchoose(
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
int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_struct{int tag;
struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_struct{
int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_struct{
int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_Genv{struct
_RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;
struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict
typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};struct
Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Stmt_j_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*
le;int allow_valueof;};void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap);void*Cyc_Tcutil_compress(void*t);extern void*Cyc_Cyclone_c_compiler;
static int Cyc_Absyndump_expand_typedefs;static int Cyc_Absyndump_qvar_to_Cids;
static int Cyc_Absyndump_add_cyc_prefix;static int Cyc_Absyndump_generate_line_directives;
static int Cyc_Absyndump_to_VC;void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*
fs){Cyc_Absyndump_expand_typedefs=fs->expand_typedefs;Cyc_Absyndump_qvar_to_Cids=
fs->qvar_to_Cids;Cyc_Absyndump_add_cyc_prefix=fs->add_cyc_prefix;Cyc_Absyndump_to_VC=
fs->to_VC;Cyc_Absyndump_generate_line_directives=fs->generate_line_directives;
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
_dyneither_ptr s){int sz=(int)Cyc_strlen((struct _dyneither_ptr)s);Cyc_Absyndump_pos
+=sz;if(Cyc_Absyndump_pos > 80){Cyc_Absyndump_pos=(unsigned int)sz;Cyc_fputc((int)'\n',*
Cyc_Absyndump_dump_file);}else{if(Cyc_Absyndump_need_space_before())Cyc_fputc((
int)' ',*Cyc_Absyndump_dump_file);}if(sz >= 1){Cyc_Absyndump_prev_char=*((const
char*)_check_dyneither_subscript(s,sizeof(char),sz - 1));Cyc_file_string_write(*
Cyc_Absyndump_dump_file,s,0,sz);}}void Cyc_Absyndump_dump_nospace(struct
_dyneither_ptr s){int sz=(int)Cyc_strlen((struct _dyneither_ptr)s);Cyc_Absyndump_pos
+=sz;if(sz >= 1){Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,sz);Cyc_Absyndump_prev_char=*((
const char*)_check_dyneither_subscript(s,sizeof(char),sz - 1));}}void Cyc_Absyndump_dump_char(
int c){++ Cyc_Absyndump_pos;Cyc_fputc(c,*Cyc_Absyndump_dump_file);Cyc_Absyndump_prev_char=(
char)c;}void Cyc_Absyndump_dumploc(struct Cyc_Position_Segment*loc){if(loc == 0)
return;if(!Cyc_Absyndump_generate_line_directives)return;{struct _dyneither_ptr
_tmp0=Cyc_Position_get_line_directive(loc);Cyc_Absyndump_dump(_tmp0);}}void Cyc_Absyndump_dump_str(
struct _dyneither_ptr*s){Cyc_Absyndump_dump(*s);}void Cyc_Absyndump_dump_semi(){
Cyc_Absyndump_dump_char((int)';');}void Cyc_Absyndump_dump_sep(void(*f)(void*),
struct Cyc_List_List*l,struct _dyneither_ptr sep){if(l == 0)return;for(0;l->tl != 0;l=
l->tl){f((void*)l->hd);Cyc_Absyndump_dump_nospace(sep);}f((void*)l->hd);}void Cyc_Absyndump_dump_sep_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr sep){
if(l == 0)return;for(0;l->tl != 0;l=l->tl){f(env,(void*)l->hd);Cyc_Absyndump_dump_nospace(
sep);}f(env,(void*)l->hd);}void Cyc_Absyndump_group(void(*f)(void*),struct Cyc_List_List*
l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){Cyc_Absyndump_dump_nospace(
start);Cyc_Absyndump_dump_sep(f,l,sep);Cyc_Absyndump_dump_nospace(end);}void Cyc_Absyndump_group_c(
void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr start,
struct _dyneither_ptr end,struct _dyneither_ptr sep){Cyc_Absyndump_dump_nospace(
start);Cyc_Absyndump_dump_sep_c(f,env,l,sep);Cyc_Absyndump_dump_nospace(end);}
void Cyc_Absyndump_egroup(void(*f)(void*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){if(l != 0)
Cyc_Absyndump_group(f,l,start,end,sep);}void Cyc_Absyndump_dumpqvar(struct _tuple0*
v){struct Cyc_List_List*_tmp1=0;struct _dyneither_ptr**prefix=0;{union Cyc_Absyn_Nmspace
_tmp2=(*v).f1;int _tmp3;struct Cyc_List_List*_tmp4;struct Cyc_List_List*_tmp5;_LL11:
if((_tmp2.Loc_n).tag != 3)goto _LL13;_tmp3=(int)(_tmp2.Loc_n).val;_LL12: _tmp4=0;
goto _LL14;_LL13: if((_tmp2.Rel_n).tag != 1)goto _LL15;_tmp4=(struct Cyc_List_List*)(
_tmp2.Rel_n).val;_LL14: _tmp1=_tmp4;goto _LL10;_LL15: if((_tmp2.Abs_n).tag != 2)goto
_LL10;_tmp5=(struct Cyc_List_List*)(_tmp2.Abs_n).val;_LL16: if(Cyc_Absyndump_qvar_to_Cids
 && Cyc_Absyndump_add_cyc_prefix)prefix=(struct _dyneither_ptr**)& Cyc_Absynpp_cyc_stringptr;
_tmp1=_tmp5;goto _LL10;_LL10:;}if(prefix != 0){Cyc_Absyndump_dump_str(*prefix);if(
Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{Cyc_Absyndump_dump_nospace(({
const char*_tmp6="::";_tag_dyneither(_tmp6,sizeof(char),3);}));}}if(_tmp1 != 0){
Cyc_Absyndump_dump_nospace(*((struct _dyneither_ptr*)_tmp1->hd));for(_tmp1=_tmp1->tl;
_tmp1 != 0;_tmp1=_tmp1->tl){if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((
int)'_');else{Cyc_Absyndump_dump_nospace(({const char*_tmp7="::";_tag_dyneither(
_tmp7,sizeof(char),3);}));}Cyc_Absyndump_dump_nospace(*((struct _dyneither_ptr*)
_tmp1->hd));}if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_nospace(({const
char*_tmp8="_";_tag_dyneither(_tmp8,sizeof(char),2);}));else{Cyc_Absyndump_dump_nospace(({
const char*_tmp9="::";_tag_dyneither(_tmp9,sizeof(char),3);}));}Cyc_Absyndump_dump_nospace(*(*
v).f2);}else{if(prefix != 0)Cyc_Absyndump_dump_nospace(*(*v).f2);else{Cyc_Absyndump_dump_str((*
v).f2);}}}void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){if(tq.q_restrict)Cyc_Absyndump_dump(({
const char*_tmpA="restrict";_tag_dyneither(_tmpA,sizeof(char),9);}));if(tq.q_volatile)
Cyc_Absyndump_dump(({const char*_tmpB="volatile";_tag_dyneither(_tmpB,sizeof(char),
9);}));if(tq.print_const)Cyc_Absyndump_dump(({const char*_tmpC="const";
_tag_dyneither(_tmpC,sizeof(char),6);}));}void Cyc_Absyndump_dumpscope(void*sc){
void*_tmpD=sc;_LL18: if((int)_tmpD != 0)goto _LL1A;_LL19: Cyc_Absyndump_dump(({const
char*_tmpE="static";_tag_dyneither(_tmpE,sizeof(char),7);}));return;_LL1A: if((
int)_tmpD != 2)goto _LL1C;_LL1B: return;_LL1C: if((int)_tmpD != 3)goto _LL1E;_LL1D: Cyc_Absyndump_dump(({
const char*_tmpF="extern";_tag_dyneither(_tmpF,sizeof(char),7);}));return;_LL1E:
if((int)_tmpD != 4)goto _LL20;_LL1F: Cyc_Absyndump_dump(({const char*_tmp10="extern \"C\"";
_tag_dyneither(_tmp10,sizeof(char),11);}));return;_LL20: if((int)_tmpD != 1)goto
_LL22;_LL21: Cyc_Absyndump_dump(({const char*_tmp11="abstract";_tag_dyneither(
_tmp11,sizeof(char),9);}));return;_LL22: if((int)_tmpD != 5)goto _LL17;_LL23: Cyc_Absyndump_dump(({
const char*_tmp12="register";_tag_dyneither(_tmp12,sizeof(char),9);}));return;
_LL17:;}void Cyc_Absyndump_dumpkind(void*k){void*_tmp13=k;_LL25: if((int)_tmp13 != 
0)goto _LL27;_LL26: Cyc_Absyndump_dump(({const char*_tmp14="A";_tag_dyneither(
_tmp14,sizeof(char),2);}));return;_LL27: if((int)_tmp13 != 1)goto _LL29;_LL28: Cyc_Absyndump_dump(({
const char*_tmp15="M";_tag_dyneither(_tmp15,sizeof(char),2);}));return;_LL29: if((
int)_tmp13 != 2)goto _LL2B;_LL2A: Cyc_Absyndump_dump(({const char*_tmp16="B";
_tag_dyneither(_tmp16,sizeof(char),2);}));return;_LL2B: if((int)_tmp13 != 3)goto
_LL2D;_LL2C: Cyc_Absyndump_dump(({const char*_tmp17="R";_tag_dyneither(_tmp17,
sizeof(char),2);}));return;_LL2D: if((int)_tmp13 != 4)goto _LL2F;_LL2E: Cyc_Absyndump_dump(({
const char*_tmp18="UR";_tag_dyneither(_tmp18,sizeof(char),3);}));return;_LL2F: if((
int)_tmp13 != 5)goto _LL31;_LL30: Cyc_Absyndump_dump(({const char*_tmp19="TR";
_tag_dyneither(_tmp19,sizeof(char),3);}));return;_LL31: if((int)_tmp13 != 6)goto
_LL33;_LL32: Cyc_Absyndump_dump(({const char*_tmp1A="E";_tag_dyneither(_tmp1A,
sizeof(char),2);}));return;_LL33: if((int)_tmp13 != 7)goto _LL24;_LL34: Cyc_Absyndump_dump(({
const char*_tmp1B="I";_tag_dyneither(_tmp1B,sizeof(char),2);}));return;_LL24:;}
void Cyc_Absyndump_dumpaggr_kind(void*k){void*_tmp1C=k;_LL36: if((int)_tmp1C != 0)
goto _LL38;_LL37: Cyc_Absyndump_dump(({const char*_tmp1D="struct";_tag_dyneither(
_tmp1D,sizeof(char),7);}));return;_LL38: if((int)_tmp1C != 1)goto _LL35;_LL39: Cyc_Absyndump_dump(({
const char*_tmp1E="union";_tag_dyneither(_tmp1E,sizeof(char),6);}));return;_LL35:;}
void Cyc_Absyndump_dumptps(struct Cyc_List_List*ts){Cyc_Absyndump_egroup(Cyc_Absyndump_dumptyp,
ts,({const char*_tmp1F="<";_tag_dyneither(_tmp1F,sizeof(char),2);}),({const char*
_tmp20=">";_tag_dyneither(_tmp20,sizeof(char),2);}),({const char*_tmp21=",";
_tag_dyneither(_tmp21,sizeof(char),2);}));}void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*
tv){Cyc_Absyndump_dump_str(tv->name);}void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*
tv){Cyc_Absyndump_dump_str(tv->name);{void*_tmp22=Cyc_Absyn_compress_kb(tv->kind);
void*_tmp23;void*_tmp24;_LL3B: if(*((int*)_tmp22)!= 0)goto _LL3D;_tmp23=(void*)((
struct Cyc_Absyn_Eq_kb_struct*)_tmp22)->f1;if((int)_tmp23 != 2)goto _LL3D;_LL3C:
goto _LL3E;_LL3D: if(*((int*)_tmp22)!= 2)goto _LL3F;_LL3E: goto _LL40;_LL3F: if(*((int*)
_tmp22)!= 1)goto _LL41;_LL40: Cyc_Absyndump_dump(({const char*_tmp25="::?";
_tag_dyneither(_tmp25,sizeof(char),4);}));goto _LL3A;_LL41: if(*((int*)_tmp22)!= 0)
goto _LL3A;_tmp24=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmp22)->f1;_LL42: Cyc_Absyndump_dump(({
const char*_tmp26="::";_tag_dyneither(_tmp26,sizeof(char),3);}));Cyc_Absyndump_dumpkind(
_tmp24);goto _LL3A;_LL3A:;}}void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){((
void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumptvar,tvs,({const char*_tmp27="<";_tag_dyneither(_tmp27,sizeof(
char),2);}),({const char*_tmp28=">";_tag_dyneither(_tmp28,sizeof(char),2);}),({
const char*_tmp29=",";_tag_dyneither(_tmp29,sizeof(char),2);}));}void Cyc_Absyndump_dumpkindedtvars(
struct Cyc_List_List*tvs){((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*
l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpkindedtvar,tvs,({const char*_tmp2A="<";_tag_dyneither(_tmp2A,
sizeof(char),2);}),({const char*_tmp2B=">";_tag_dyneither(_tmp2B,sizeof(char),2);}),({
const char*_tmp2C=",";_tag_dyneither(_tmp2C,sizeof(char),2);}));}struct _tuple14{
struct Cyc_Absyn_Tqual f1;void*f2;};void Cyc_Absyndump_dumparg(struct _tuple14*pr){((
void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*
pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}void Cyc_Absyndump_dumpargs(
struct Cyc_List_List*ts){((void(*)(void(*f)(struct _tuple14*),struct Cyc_List_List*
l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumparg,ts,({const char*_tmp2D="(";_tag_dyneither(_tmp2D,sizeof(
char),2);}),({const char*_tmp2E=")";_tag_dyneither(_tmp2E,sizeof(char),2);}),({
const char*_tmp2F=",";_tag_dyneither(_tmp2F,sizeof(char),2);}));}void Cyc_Absyndump_dump_callconv(
struct Cyc_List_List*atts){for(0;atts != 0;atts=atts->tl){void*_tmp30=(void*)atts->hd;
_LL44: if((int)_tmp30 != 0)goto _LL46;_LL45: Cyc_Absyndump_dump(({const char*_tmp31="_stdcall";
_tag_dyneither(_tmp31,sizeof(char),9);}));return;_LL46: if((int)_tmp30 != 1)goto
_LL48;_LL47: Cyc_Absyndump_dump(({const char*_tmp32="_cdecl";_tag_dyneither(_tmp32,
sizeof(char),7);}));return;_LL48: if((int)_tmp30 != 2)goto _LL4A;_LL49: Cyc_Absyndump_dump(({
const char*_tmp33="_fastcall";_tag_dyneither(_tmp33,sizeof(char),10);}));return;
_LL4A:;_LL4B: goto _LL43;_LL43:;}}void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*
atts){int hasatt=0;{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmp34=(void*)((struct Cyc_List_List*)_check_null(atts))->hd;_LL4D: if((int)
_tmp34 != 0)goto _LL4F;_LL4E: goto _LL50;_LL4F: if((int)_tmp34 != 1)goto _LL51;_LL50:
goto _LL52;_LL51: if((int)_tmp34 != 2)goto _LL53;_LL52: goto _LL4C;_LL53:;_LL54: hasatt=
1;goto _LL4C;_LL4C:;}}if(!hasatt)return;Cyc_Absyndump_dump(({const char*_tmp35="__declspec(";
_tag_dyneither(_tmp35,sizeof(char),12);}));for(0;atts != 0;atts=atts->tl){void*
_tmp36=(void*)atts->hd;_LL56: if((int)_tmp36 != 0)goto _LL58;_LL57: goto _LL59;_LL58:
if((int)_tmp36 != 1)goto _LL5A;_LL59: goto _LL5B;_LL5A: if((int)_tmp36 != 2)goto _LL5C;
_LL5B: goto _LL55;_LL5C:;_LL5D: Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)
atts->hd));goto _LL55;_LL55:;}Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumpatts(
struct Cyc_List_List*atts){if(atts == 0)return;{void*_tmp37=Cyc_Cyclone_c_compiler;
_LL5F: if((int)_tmp37 != 0)goto _LL61;_LL60: Cyc_Absyndump_dump(({const char*_tmp38=" __attribute__((";
_tag_dyneither(_tmp38,sizeof(char),17);}));for(0;atts != 0;atts=atts->tl){Cyc_Absyndump_dump(
Cyc_Absyn_attribute2string((void*)atts->hd));if(atts->tl != 0)Cyc_Absyndump_dump(({
const char*_tmp39=",";_tag_dyneither(_tmp39,sizeof(char),2);}));}Cyc_Absyndump_dump(({
const char*_tmp3A=")) ";_tag_dyneither(_tmp3A,sizeof(char),4);}));return;_LL61:
if((int)_tmp37 != 1)goto _LL5E;_LL62: Cyc_Absyndump_dump_noncallconv(atts);return;
_LL5E:;}}int Cyc_Absyndump_next_is_pointer(struct Cyc_List_List*tms){if(tms == 0)
return 0;{void*_tmp3B=(void*)tms->hd;_LL64: if(*((int*)_tmp3B)!= 2)goto _LL66;_LL65:
return 1;_LL66:;_LL67: return 0;_LL63:;}}static void Cyc_Absyndump_dumprgn(void*t){
void*_tmp3C=Cyc_Tcutil_compress(t);_LL69: if((int)_tmp3C != 2)goto _LL6B;_LL6A: Cyc_Absyndump_dump(({
const char*_tmp3D="`H";_tag_dyneither(_tmp3D,sizeof(char),3);}));goto _LL68;_LL6B:;
_LL6C: Cyc_Absyndump_dumpntyp(t);goto _LL68;_LL68:;}struct _tuple15{struct Cyc_List_List*
f1;struct Cyc_List_List*f2;};static struct _tuple15 Cyc_Absyndump_effects_split(void*
t){struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;{void*_tmp3E=Cyc_Tcutil_compress(
t);void*_tmp3F;struct Cyc_List_List*_tmp40;_LL6E: if(_tmp3E <= (void*)4)goto _LL72;
if(*((int*)_tmp3E)!= 19)goto _LL70;_tmp3F=(void*)((struct Cyc_Absyn_AccessEff_struct*)
_tmp3E)->f1;_LL6F: rgions=({struct Cyc_List_List*_tmp41=_cycalloc(sizeof(*_tmp41));
_tmp41->hd=(void*)_tmp3F;_tmp41->tl=rgions;_tmp41;});goto _LL6D;_LL70: if(*((int*)
_tmp3E)!= 20)goto _LL72;_tmp40=((struct Cyc_Absyn_JoinEff_struct*)_tmp3E)->f1;
_LL71: for(0;_tmp40 != 0;_tmp40=_tmp40->tl){struct Cyc_List_List*_tmp43;struct Cyc_List_List*
_tmp44;struct _tuple15 _tmp42=Cyc_Absyndump_effects_split((void*)_tmp40->hd);
_tmp43=_tmp42.f1;_tmp44=_tmp42.f2;rgions=Cyc_List_imp_append(_tmp43,rgions);
effects=Cyc_List_imp_append(_tmp44,effects);}goto _LL6D;_LL72:;_LL73: effects=({
struct Cyc_List_List*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45->hd=(void*)t;_tmp45->tl=
effects;_tmp45;});goto _LL6D;_LL6D:;}return({struct _tuple15 _tmp46;_tmp46.f1=
rgions;_tmp46.f2=effects;_tmp46;});}static void Cyc_Absyndump_dumpeff(void*t){
struct Cyc_List_List*_tmp48;struct Cyc_List_List*_tmp49;struct _tuple15 _tmp47=Cyc_Absyndump_effects_split(
t);_tmp48=_tmp47.f1;_tmp49=_tmp47.f2;_tmp48=Cyc_List_imp_rev(_tmp48);_tmp49=Cyc_List_imp_rev(
_tmp49);for(0;_tmp49 != 0;_tmp49=_tmp49->tl){Cyc_Absyndump_dumpntyp((void*)_tmp49->hd);
Cyc_Absyndump_dump_char((int)'+');}Cyc_Absyndump_dump_char((int)'{');for(0;
_tmp48 != 0;_tmp48=_tmp48->tl){Cyc_Absyndump_dumprgn((void*)_tmp48->hd);if(_tmp48->tl
!= 0)Cyc_Absyndump_dump_char((int)',');}Cyc_Absyndump_dump_char((int)'}');}void
Cyc_Absyndump_dumpntyp(void*t){void*_tmp4A=t;struct Cyc_Absyn_Tvar*_tmp4B;struct
Cyc_Core_Opt*_tmp4C;struct Cyc_Core_Opt*_tmp4D;int _tmp4E;struct Cyc_Core_Opt*
_tmp4F;struct Cyc_Core_Opt*_tmp50;struct Cyc_Core_Opt _tmp51;void*_tmp52;int _tmp53;
struct Cyc_Absyn_DatatypeInfo _tmp54;union Cyc_Absyn_DatatypeInfoU _tmp55;struct Cyc_List_List*
_tmp56;struct Cyc_Core_Opt*_tmp57;struct Cyc_Absyn_DatatypeFieldInfo _tmp58;union
Cyc_Absyn_DatatypeFieldInfoU _tmp59;struct Cyc_List_List*_tmp5A;void*_tmp5B;void*
_tmp5C;void*_tmp5D;void*_tmp5E;void*_tmp5F;void*_tmp60;void*_tmp61;void*_tmp62;
void*_tmp63;void*_tmp64;void*_tmp65;void*_tmp66;void*_tmp67;void*_tmp68;void*
_tmp69;void*_tmp6A;void*_tmp6B;void*_tmp6C;void*_tmp6D;void*_tmp6E;void*_tmp6F;
void*_tmp70;void*_tmp71;void*_tmp72;void*_tmp73;void*_tmp74;void*_tmp75;void*
_tmp76;void*_tmp77;void*_tmp78;int _tmp79;struct Cyc_List_List*_tmp7A;struct Cyc_Absyn_AggrInfo
_tmp7B;union Cyc_Absyn_AggrInfoU _tmp7C;struct Cyc_List_List*_tmp7D;void*_tmp7E;
struct Cyc_List_List*_tmp7F;struct _tuple0*_tmp80;struct Cyc_List_List*_tmp81;
struct _tuple0*_tmp82;struct Cyc_List_List*_tmp83;struct Cyc_Absyn_Exp*_tmp84;void*
_tmp85;void*_tmp86;void*_tmp87;void*_tmp88;_LL75: if(_tmp4A <= (void*)4)goto _LL7B;
if(*((int*)_tmp4A)!= 7)goto _LL77;_LL76: goto _LL78;_LL77: if(*((int*)_tmp4A)!= 8)
goto _LL79;_LL78: goto _LL7A;_LL79: if(*((int*)_tmp4A)!= 4)goto _LL7B;_LL7A: return;
_LL7B: if((int)_tmp4A != 0)goto _LL7D;_LL7C: Cyc_Absyndump_dump(({const char*_tmp89="void";
_tag_dyneither(_tmp89,sizeof(char),5);}));return;_LL7D: if(_tmp4A <= (void*)4)goto
_LLA5;if(*((int*)_tmp4A)!= 1)goto _LL7F;_tmp4B=((struct Cyc_Absyn_VarType_struct*)
_tmp4A)->f1;_LL7E: Cyc_Absyndump_dump_str(_tmp4B->name);return;_LL7F: if(*((int*)
_tmp4A)!= 0)goto _LL81;_tmp4C=((struct Cyc_Absyn_Evar_struct*)_tmp4A)->f1;_tmp4D=((
struct Cyc_Absyn_Evar_struct*)_tmp4A)->f2;if(_tmp4D != 0)goto _LL81;_tmp4E=((struct
Cyc_Absyn_Evar_struct*)_tmp4A)->f3;_LL80: Cyc_Absyndump_dump(({const char*_tmp8A="%";
_tag_dyneither(_tmp8A,sizeof(char),2);}));if(_tmp4C == 0)Cyc_Absyndump_dump(({
const char*_tmp8B="?";_tag_dyneither(_tmp8B,sizeof(char),2);}));else{Cyc_Absyndump_dumpkind((
void*)_tmp4C->v);}Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_struct
_tmp8E;_tmp8E.tag=1;_tmp8E.f1=(unsigned long)_tmp4E;{void*_tmp8C[1]={& _tmp8E};
Cyc_aprintf(({const char*_tmp8D="(%d)";_tag_dyneither(_tmp8D,sizeof(char),5);}),
_tag_dyneither(_tmp8C,sizeof(void*),1));}}));return;_LL81: if(*((int*)_tmp4A)!= 0)
goto _LL83;_tmp4F=((struct Cyc_Absyn_Evar_struct*)_tmp4A)->f1;_tmp50=((struct Cyc_Absyn_Evar_struct*)
_tmp4A)->f2;if(_tmp50 == 0)goto _LL83;_tmp51=*_tmp50;_tmp52=(void*)_tmp51.v;_tmp53=((
struct Cyc_Absyn_Evar_struct*)_tmp4A)->f3;_LL82: Cyc_Absyndump_dumpntyp(_tmp52);
return;_LL83: if(*((int*)_tmp4A)!= 2)goto _LL85;_tmp54=((struct Cyc_Absyn_DatatypeType_struct*)
_tmp4A)->f1;_tmp55=_tmp54.datatype_info;_tmp56=_tmp54.targs;_tmp57=_tmp54.rgn;
_LL84:{union Cyc_Absyn_DatatypeInfoU _tmp8F=_tmp55;struct Cyc_Absyn_UnknownDatatypeInfo
_tmp90;struct _tuple0*_tmp91;int _tmp92;struct Cyc_Absyn_Datatypedecl**_tmp93;
struct Cyc_Absyn_Datatypedecl*_tmp94;struct Cyc_Absyn_Datatypedecl _tmp95;struct
_tuple0*_tmp96;int _tmp97;_LLC8: if((_tmp8F.UnknownDatatype).tag != 1)goto _LLCA;
_tmp90=(struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp8F.UnknownDatatype).val;_tmp91=
_tmp90.name;_tmp92=_tmp90.is_extensible;_LLC9: _tmp96=_tmp91;_tmp97=_tmp92;goto
_LLCB;_LLCA: if((_tmp8F.KnownDatatype).tag != 2)goto _LLC7;_tmp93=(struct Cyc_Absyn_Datatypedecl**)(
_tmp8F.KnownDatatype).val;_tmp94=*_tmp93;_tmp95=*_tmp94;_tmp96=_tmp95.name;
_tmp97=_tmp95.is_extensible;_LLCB: if(_tmp97)Cyc_Absyndump_dump(({const char*
_tmp98="@extensible ";_tag_dyneither(_tmp98,sizeof(char),13);}));Cyc_Absyndump_dump(({
const char*_tmp99="datatype ";_tag_dyneither(_tmp99,sizeof(char),10);}));{void*r=(
unsigned int)_tmp57?(void*)_tmp57->v:(void*)2;{void*_tmp9A=Cyc_Tcutil_compress(r);
_LLCD: if((int)_tmp9A != 2)goto _LLCF;_LLCE: goto _LLCC;_LLCF:;_LLD0: Cyc_Absyndump_dumptyp(
r);Cyc_Absyndump_dump(({const char*_tmp9B=" ";_tag_dyneither(_tmp9B,sizeof(char),
2);}));goto _LLCC;_LLCC:;}Cyc_Absyndump_dumpqvar(_tmp96);Cyc_Absyndump_dumptps(
_tmp56);goto _LLC7;}_LLC7:;}goto _LL74;_LL85: if(*((int*)_tmp4A)!= 3)goto _LL87;
_tmp58=((struct Cyc_Absyn_DatatypeFieldType_struct*)_tmp4A)->f1;_tmp59=_tmp58.field_info;
_tmp5A=_tmp58.targs;_LL86:{union Cyc_Absyn_DatatypeFieldInfoU _tmp9C=_tmp59;struct
Cyc_Absyn_UnknownDatatypeFieldInfo _tmp9D;struct _tuple0*_tmp9E;struct _tuple0*
_tmp9F;int _tmpA0;struct _tuple1 _tmpA1;struct Cyc_Absyn_Datatypedecl*_tmpA2;struct
Cyc_Absyn_Datatypedecl _tmpA3;struct _tuple0*_tmpA4;int _tmpA5;struct Cyc_Absyn_Datatypefield*
_tmpA6;struct Cyc_Absyn_Datatypefield _tmpA7;struct _tuple0*_tmpA8;_LLD2: if((_tmp9C.UnknownDatatypefield).tag
!= 1)goto _LLD4;_tmp9D=(struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp9C.UnknownDatatypefield).val;
_tmp9E=_tmp9D.datatype_name;_tmp9F=_tmp9D.field_name;_tmpA0=_tmp9D.is_extensible;
_LLD3: _tmpA4=_tmp9E;_tmpA5=_tmpA0;_tmpA8=_tmp9F;goto _LLD5;_LLD4: if((_tmp9C.KnownDatatypefield).tag
!= 2)goto _LLD1;_tmpA1=(struct _tuple1)(_tmp9C.KnownDatatypefield).val;_tmpA2=
_tmpA1.f1;_tmpA3=*_tmpA2;_tmpA4=_tmpA3.name;_tmpA5=_tmpA3.is_extensible;_tmpA6=
_tmpA1.f2;_tmpA7=*_tmpA6;_tmpA8=_tmpA7.name;_LLD5: if(_tmpA5)Cyc_Absyndump_dump(({
const char*_tmpA9="@extensible ";_tag_dyneither(_tmpA9,sizeof(char),13);}));Cyc_Absyndump_dump(({
const char*_tmpAA="datatype ";_tag_dyneither(_tmpAA,sizeof(char),10);}));Cyc_Absyndump_dumpqvar(
_tmpA4);Cyc_Absyndump_dump(({const char*_tmpAB=".";_tag_dyneither(_tmpAB,sizeof(
char),2);}));Cyc_Absyndump_dumpqvar(_tmpA8);Cyc_Absyndump_dumptps(_tmp5A);goto
_LLD1;_LLD1:;}goto _LL74;_LL87: if(*((int*)_tmp4A)!= 5)goto _LL89;_tmp5B=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp4A)->f1;if((int)_tmp5B != 2)goto _LL89;_tmp5C=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A)->f2;if((int)_tmp5C != 2)goto _LL89;
_LL88: goto _LL8A;_LL89: if(*((int*)_tmp4A)!= 5)goto _LL8B;_tmp5D=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4A)->f1;if((int)_tmp5D != 0)goto _LL8B;_tmp5E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4A)->f2;if((int)_tmp5E != 2)goto _LL8B;_LL8A: Cyc_Absyndump_dump(({const char*
_tmpAC="int";_tag_dyneither(_tmpAC,sizeof(char),4);}));return;_LL8B: if(*((int*)
_tmp4A)!= 5)goto _LL8D;_tmp5F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A)->f1;
if((int)_tmp5F != 2)goto _LL8D;_tmp60=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp4A)->f2;if((int)_tmp60 != 3)goto _LL8D;_LL8C: goto _LL8E;_LL8D: if(*((int*)_tmp4A)
!= 5)goto _LL8F;_tmp61=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A)->f1;if((
int)_tmp61 != 0)goto _LL8F;_tmp62=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A)->f2;
if((int)_tmp62 != 3)goto _LL8F;_LL8E: Cyc_Absyndump_dump(({const char*_tmpAD="long";
_tag_dyneither(_tmpAD,sizeof(char),5);}));return;_LL8F: if(*((int*)_tmp4A)!= 5)
goto _LL91;_tmp63=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A)->f1;if((int)
_tmp63 != 2)goto _LL91;_tmp64=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A)->f2;
if((int)_tmp64 != 0)goto _LL91;_LL90: Cyc_Absyndump_dump(({const char*_tmpAE="char";
_tag_dyneither(_tmpAE,sizeof(char),5);}));return;_LL91: if(*((int*)_tmp4A)!= 5)
goto _LL93;_tmp65=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A)->f1;if((int)
_tmp65 != 0)goto _LL93;_tmp66=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A)->f2;
if((int)_tmp66 != 0)goto _LL93;_LL92: Cyc_Absyndump_dump(({const char*_tmpAF="signed char";
_tag_dyneither(_tmpAF,sizeof(char),12);}));return;_LL93: if(*((int*)_tmp4A)!= 5)
goto _LL95;_tmp67=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A)->f1;if((int)
_tmp67 != 1)goto _LL95;_tmp68=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A)->f2;
if((int)_tmp68 != 0)goto _LL95;_LL94: Cyc_Absyndump_dump(({const char*_tmpB0="unsigned char";
_tag_dyneither(_tmpB0,sizeof(char),14);}));return;_LL95: if(*((int*)_tmp4A)!= 5)
goto _LL97;_tmp69=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A)->f1;if((int)
_tmp69 != 2)goto _LL97;_tmp6A=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A)->f2;
if((int)_tmp6A != 1)goto _LL97;_LL96: goto _LL98;_LL97: if(*((int*)_tmp4A)!= 5)goto
_LL99;_tmp6B=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A)->f1;if((int)_tmp6B
!= 0)goto _LL99;_tmp6C=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A)->f2;if((
int)_tmp6C != 1)goto _LL99;_LL98: Cyc_Absyndump_dump(({const char*_tmpB1="short";
_tag_dyneither(_tmpB1,sizeof(char),6);}));return;_LL99: if(*((int*)_tmp4A)!= 5)
goto _LL9B;_tmp6D=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A)->f1;if((int)
_tmp6D != 1)goto _LL9B;_tmp6E=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A)->f2;
if((int)_tmp6E != 1)goto _LL9B;_LL9A: Cyc_Absyndump_dump(({const char*_tmpB2="unsigned short";
_tag_dyneither(_tmpB2,sizeof(char),15);}));return;_LL9B: if(*((int*)_tmp4A)!= 5)
goto _LL9D;_tmp6F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A)->f1;if((int)
_tmp6F != 1)goto _LL9D;_tmp70=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A)->f2;
if((int)_tmp70 != 2)goto _LL9D;_LL9C: Cyc_Absyndump_dump(({const char*_tmpB3="unsigned int";
_tag_dyneither(_tmpB3,sizeof(char),13);}));return;_LL9D: if(*((int*)_tmp4A)!= 5)
goto _LL9F;_tmp71=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A)->f1;if((int)
_tmp71 != 1)goto _LL9F;_tmp72=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A)->f2;
if((int)_tmp72 != 3)goto _LL9F;_LL9E: Cyc_Absyndump_dump(({const char*_tmpB4="unsigned long";
_tag_dyneither(_tmpB4,sizeof(char),14);}));return;_LL9F: if(*((int*)_tmp4A)!= 5)
goto _LLA1;_tmp73=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A)->f1;if((int)
_tmp73 != 2)goto _LLA1;_tmp74=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A)->f2;
if((int)_tmp74 != 4)goto _LLA1;_LLA0: goto _LLA2;_LLA1: if(*((int*)_tmp4A)!= 5)goto
_LLA3;_tmp75=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A)->f1;if((int)_tmp75
!= 0)goto _LLA3;_tmp76=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A)->f2;if((
int)_tmp76 != 4)goto _LLA3;_LLA2: {void*_tmpB5=Cyc_Cyclone_c_compiler;_LLD7: if((
int)_tmpB5 != 0)goto _LLD9;_LLD8: Cyc_Absyndump_dump(({const char*_tmpB6="long long";
_tag_dyneither(_tmpB6,sizeof(char),10);}));return;_LLD9: if((int)_tmpB5 != 1)goto
_LLD6;_LLDA: Cyc_Absyndump_dump(({const char*_tmpB7="__int64";_tag_dyneither(
_tmpB7,sizeof(char),8);}));return;_LLD6:;}_LLA3: if(*((int*)_tmp4A)!= 5)goto _LLA5;
_tmp77=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A)->f1;if((int)_tmp77 != 1)
goto _LLA5;_tmp78=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp4A)->f2;if((int)
_tmp78 != 4)goto _LLA5;_LLA4: {void*_tmpB8=Cyc_Cyclone_c_compiler;_LLDC: if((int)
_tmpB8 != 1)goto _LLDE;_LLDD: Cyc_Absyndump_dump(({const char*_tmpB9="unsigned __int64";
_tag_dyneither(_tmpB9,sizeof(char),17);}));return;_LLDE: if((int)_tmpB8 != 0)goto
_LLDB;_LLDF: Cyc_Absyndump_dump(({const char*_tmpBA="unsigned long long";
_tag_dyneither(_tmpBA,sizeof(char),19);}));return;_LLDB:;}_LLA5: if((int)_tmp4A != 
1)goto _LLA7;_LLA6: Cyc_Absyndump_dump(({const char*_tmpBB="float";_tag_dyneither(
_tmpBB,sizeof(char),6);}));return;_LLA7: if(_tmp4A <= (void*)4)goto _LLBD;if(*((int*)
_tmp4A)!= 6)goto _LLA9;_tmp79=((struct Cyc_Absyn_DoubleType_struct*)_tmp4A)->f1;
_LLA8: if(_tmp79)Cyc_Absyndump_dump(({const char*_tmpBC="long double";
_tag_dyneither(_tmpBC,sizeof(char),12);}));else{Cyc_Absyndump_dump(({const char*
_tmpBD="double";_tag_dyneither(_tmpBD,sizeof(char),7);}));}return;_LLA9: if(*((
int*)_tmp4A)!= 9)goto _LLAB;_tmp7A=((struct Cyc_Absyn_TupleType_struct*)_tmp4A)->f1;
_LLAA: Cyc_Absyndump_dump_char((int)'$');Cyc_Absyndump_dumpargs(_tmp7A);return;
_LLAB: if(*((int*)_tmp4A)!= 10)goto _LLAD;_tmp7B=((struct Cyc_Absyn_AggrType_struct*)
_tmp4A)->f1;_tmp7C=_tmp7B.aggr_info;_tmp7D=_tmp7B.targs;_LLAC: {void*_tmpBF;
struct _tuple0*_tmpC0;struct _tuple9 _tmpBE=Cyc_Absyn_aggr_kinded_name(_tmp7C);
_tmpBF=_tmpBE.f1;_tmpC0=_tmpBE.f2;Cyc_Absyndump_dumpaggr_kind(_tmpBF);Cyc_Absyndump_dumpqvar(
_tmpC0);Cyc_Absyndump_dumptps(_tmp7D);return;}_LLAD: if(*((int*)_tmp4A)!= 11)goto
_LLAF;_tmp7E=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)_tmp4A)->f1;_tmp7F=((
struct Cyc_Absyn_AnonAggrType_struct*)_tmp4A)->f2;_LLAE: Cyc_Absyndump_dumpaggr_kind(
_tmp7E);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpaggrfields(_tmp7F);
Cyc_Absyndump_dump_char((int)'}');return;_LLAF: if(*((int*)_tmp4A)!= 12)goto _LLB1;
_tmp80=((struct Cyc_Absyn_EnumType_struct*)_tmp4A)->f1;_LLB0: Cyc_Absyndump_dump(({
const char*_tmpC1="enum ";_tag_dyneither(_tmpC1,sizeof(char),6);}));Cyc_Absyndump_dumpqvar(
_tmp80);return;_LLB1: if(*((int*)_tmp4A)!= 13)goto _LLB3;_tmp81=((struct Cyc_Absyn_AnonEnumType_struct*)
_tmp4A)->f1;_LLB2: Cyc_Absyndump_dump(({const char*_tmpC2="enum {";_tag_dyneither(
_tmpC2,sizeof(char),7);}));Cyc_Absyndump_dumpenumfields(_tmp81);Cyc_Absyndump_dump(({
const char*_tmpC3="}";_tag_dyneither(_tmpC3,sizeof(char),2);}));return;_LLB3: if(*((
int*)_tmp4A)!= 16)goto _LLB5;_tmp82=((struct Cyc_Absyn_TypedefType_struct*)_tmp4A)->f1;
_tmp83=((struct Cyc_Absyn_TypedefType_struct*)_tmp4A)->f2;_LLB4:(Cyc_Absyndump_dumpqvar(
_tmp82),Cyc_Absyndump_dumptps(_tmp83));return;_LLB5: if(*((int*)_tmp4A)!= 17)goto
_LLB7;_tmp84=((struct Cyc_Absyn_ValueofType_struct*)_tmp4A)->f1;_LLB6: Cyc_Absyndump_dump(({
const char*_tmpC4="valueof_t(";_tag_dyneither(_tmpC4,sizeof(char),11);}));Cyc_Absyndump_dumpexp(
_tmp84);Cyc_Absyndump_dump(({const char*_tmpC5=")";_tag_dyneither(_tmpC5,sizeof(
char),2);}));return;_LLB7: if(*((int*)_tmp4A)!= 14)goto _LLB9;_tmp85=(void*)((
struct Cyc_Absyn_RgnHandleType_struct*)_tmp4A)->f1;_LLB8: Cyc_Absyndump_dump(({
const char*_tmpC6="region_t<";_tag_dyneither(_tmpC6,sizeof(char),10);}));Cyc_Absyndump_dumprgn(
_tmp85);Cyc_Absyndump_dump(({const char*_tmpC7=">";_tag_dyneither(_tmpC7,sizeof(
char),2);}));return;_LLB9: if(*((int*)_tmp4A)!= 15)goto _LLBB;_tmp86=(void*)((
struct Cyc_Absyn_DynRgnType_struct*)_tmp4A)->f1;_tmp87=(void*)((struct Cyc_Absyn_DynRgnType_struct*)
_tmp4A)->f2;_LLBA: Cyc_Absyndump_dump(({const char*_tmpC8="dynregion_t<";
_tag_dyneither(_tmpC8,sizeof(char),13);}));Cyc_Absyndump_dumprgn(_tmp86);Cyc_Absyndump_dump(({
const char*_tmpC9=",";_tag_dyneither(_tmpC9,sizeof(char),2);}));Cyc_Absyndump_dumprgn(
_tmp87);Cyc_Absyndump_dump(({const char*_tmpCA=">";_tag_dyneither(_tmpCA,sizeof(
char),2);}));return;_LLBB: if(*((int*)_tmp4A)!= 18)goto _LLBD;_tmp88=(void*)((
struct Cyc_Absyn_TagType_struct*)_tmp4A)->f1;_LLBC: Cyc_Absyndump_dump(({const char*
_tmpCB="tag_t<";_tag_dyneither(_tmpCB,sizeof(char),7);}));Cyc_Absyndump_dumpntyp(
_tmp88);Cyc_Absyndump_dump(({const char*_tmpCC=">";_tag_dyneither(_tmpCC,sizeof(
char),2);}));return;_LLBD: if((int)_tmp4A != 3)goto _LLBF;_LLBE: Cyc_Absyndump_dump(({
const char*_tmpCD="`U";_tag_dyneither(_tmpCD,sizeof(char),3);}));goto _LL74;_LLBF:
if((int)_tmp4A != 2)goto _LLC1;_LLC0: goto _LLC2;_LLC1: if(_tmp4A <= (void*)4)goto
_LLC3;if(*((int*)_tmp4A)!= 19)goto _LLC3;_LLC2: goto _LLC4;_LLC3: if(_tmp4A <= (void*)
4)goto _LLC5;if(*((int*)_tmp4A)!= 21)goto _LLC5;_LLC4: goto _LLC6;_LLC5: if(_tmp4A <= (
void*)4)goto _LL74;if(*((int*)_tmp4A)!= 20)goto _LL74;_LLC6: return;_LL74:;}void Cyc_Absyndump_dumpvaropt(
struct Cyc_Core_Opt*vo){if(vo != 0)Cyc_Absyndump_dump_str((struct _dyneither_ptr*)
vo->v);}void Cyc_Absyndump_dumpfunarg(struct _tuple7*t){((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct Cyc_Core_Opt*),struct Cyc_Core_Opt*))Cyc_Absyndump_dumptqtd)((*
t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}void Cyc_Absyndump_dump_rgncmp(
struct _tuple13*cmp){struct _tuple13 _tmpCF;void*_tmpD0;void*_tmpD1;struct _tuple13*
_tmpCE=cmp;_tmpCF=*_tmpCE;_tmpD0=_tmpCF.f1;_tmpD1=_tmpCF.f2;Cyc_Absyndump_dumpeff(
_tmpD0);Cyc_Absyndump_dump_char((int)'>');Cyc_Absyndump_dumprgn(_tmpD1);}void Cyc_Absyndump_dump_rgnpo(
struct Cyc_List_List*rgn_po){((void(*)(void(*f)(struct _tuple13*),struct Cyc_List_List*
l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_rgncmp,
rgn_po,({const char*_tmpD2=",";_tag_dyneither(_tmpD2,sizeof(char),2);}));}void Cyc_Absyndump_dumpfunargs(
struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,
struct Cyc_Core_Opt*effopt,struct Cyc_List_List*rgn_po){Cyc_Absyndump_dump_char((
int)'(');for(0;args != 0;args=args->tl){Cyc_Absyndump_dumpfunarg((struct _tuple7*)
args->hd);if((args->tl != 0  || c_varargs) || cyc_varargs != 0)Cyc_Absyndump_dump_char((
int)',');}if(c_varargs)Cyc_Absyndump_dump(({const char*_tmpD3="...";
_tag_dyneither(_tmpD3,sizeof(char),4);}));else{if(cyc_varargs != 0){struct _tuple7*
_tmpD4=({struct _tuple7*_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7->f1=cyc_varargs->name;
_tmpD7->f2=cyc_varargs->tq;_tmpD7->f3=cyc_varargs->type;_tmpD7;});Cyc_Absyndump_dump(({
const char*_tmpD5="...";_tag_dyneither(_tmpD5,sizeof(char),4);}));if(cyc_varargs->inject)
Cyc_Absyndump_dump(({const char*_tmpD6=" inject ";_tag_dyneither(_tmpD6,sizeof(
char),9);}));Cyc_Absyndump_dumpfunarg(_tmpD4);}}if(effopt != 0){Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpeff((void*)effopt->v);}if(rgn_po != 0){Cyc_Absyndump_dump_char((
int)':');Cyc_Absyndump_dump_rgnpo(rgn_po);}Cyc_Absyndump_dump_char((int)')');}
void Cyc_Absyndump_dumptyp(void*t){((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(
int),int))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,(void(*)(int x))Cyc_Absyndump_ignore,
0);}void Cyc_Absyndump_dumpdesignator(void*d){void*_tmpD8=d;struct Cyc_Absyn_Exp*
_tmpD9;struct _dyneither_ptr*_tmpDA;_LLE1: if(*((int*)_tmpD8)!= 0)goto _LLE3;_tmpD9=((
struct Cyc_Absyn_ArrayElement_struct*)_tmpD8)->f1;_LLE2: Cyc_Absyndump_dump(({
const char*_tmpDB=".[";_tag_dyneither(_tmpDB,sizeof(char),3);}));Cyc_Absyndump_dumpexp(
_tmpD9);Cyc_Absyndump_dump_char((int)']');goto _LLE0;_LLE3: if(*((int*)_tmpD8)!= 1)
goto _LLE0;_tmpDA=((struct Cyc_Absyn_FieldName_struct*)_tmpD8)->f1;_LLE4: Cyc_Absyndump_dump_char((
int)'.');Cyc_Absyndump_dump_nospace(*_tmpDA);goto _LLE0;_LLE0:;}struct _tuple16{
struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};void Cyc_Absyndump_dumpde(struct
_tuple16*de){Cyc_Absyndump_egroup(Cyc_Absyndump_dumpdesignator,(*de).f1,({const
char*_tmpDC="";_tag_dyneither(_tmpDC,sizeof(char),1);}),({const char*_tmpDD="=";
_tag_dyneither(_tmpDD,sizeof(char),2);}),({const char*_tmpDE="=";_tag_dyneither(
_tmpDE,sizeof(char),2);}));Cyc_Absyndump_dumpexp((*de).f2);}void Cyc_Absyndump_dumpexps_prec(
int inprec,struct Cyc_List_List*es){((void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),
int env,struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,
struct _dyneither_ptr sep))Cyc_Absyndump_group_c)(Cyc_Absyndump_dumpexp_prec,
inprec,es,({const char*_tmpDF="";_tag_dyneither(_tmpDF,sizeof(char),1);}),({const
char*_tmpE0="";_tag_dyneither(_tmpE0,sizeof(char),1);}),({const char*_tmpE1=",";
_tag_dyneither(_tmpE1,sizeof(char),2);}));}void Cyc_Absyndump_dumpexp_prec(int
inprec,struct Cyc_Absyn_Exp*e){int myprec=Cyc_Absynpp_exp_prec(e);if(inprec >= 
myprec)Cyc_Absyndump_dump_nospace(({const char*_tmpE2="(";_tag_dyneither(_tmpE2,
sizeof(char),2);}));{void*_tmpE3=e->r;union Cyc_Absyn_Cnst _tmpE4;struct _tuple3
_tmpE5;void*_tmpE6;char _tmpE7;union Cyc_Absyn_Cnst _tmpE8;struct _tuple4 _tmpE9;void*
_tmpEA;short _tmpEB;union Cyc_Absyn_Cnst _tmpEC;struct _tuple5 _tmpED;void*_tmpEE;int
_tmpEF;union Cyc_Absyn_Cnst _tmpF0;struct _tuple5 _tmpF1;void*_tmpF2;int _tmpF3;union
Cyc_Absyn_Cnst _tmpF4;struct _tuple5 _tmpF5;void*_tmpF6;int _tmpF7;union Cyc_Absyn_Cnst
_tmpF8;struct _tuple6 _tmpF9;void*_tmpFA;long long _tmpFB;union Cyc_Absyn_Cnst _tmpFC;
struct _dyneither_ptr _tmpFD;union Cyc_Absyn_Cnst _tmpFE;int _tmpFF;union Cyc_Absyn_Cnst
_tmp100;struct _dyneither_ptr _tmp101;struct _tuple0*_tmp102;struct _tuple0*_tmp103;
void*_tmp104;struct Cyc_List_List*_tmp105;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_Core_Opt*
_tmp107;struct Cyc_Absyn_Exp*_tmp108;struct Cyc_Absyn_Exp*_tmp109;void*_tmp10A;
struct Cyc_Absyn_Exp*_tmp10B;void*_tmp10C;struct Cyc_Absyn_Exp*_tmp10D;void*
_tmp10E;struct Cyc_Absyn_Exp*_tmp10F;void*_tmp110;struct Cyc_Absyn_Exp*_tmp111;
struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*
_tmp114;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*
_tmp117;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*
_tmp11A;struct Cyc_List_List*_tmp11B;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_List_List*
_tmp11D;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*
_tmp120;void*_tmp121;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*_tmp123;
struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*_tmp125;void*_tmp126;struct Cyc_Absyn_Exp*
_tmp127;void*_tmp128;struct Cyc_Absyn_Exp*_tmp129;struct _dyneither_ptr*_tmp12A;
void*_tmp12B;void*_tmp12C;struct _dyneither_ptr*_tmp12D;void*_tmp12E;void*_tmp12F;
unsigned int _tmp130;struct Cyc_List_List*_tmp131;void*_tmp132;struct Cyc_Absyn_Exp*
_tmp133;struct Cyc_Absyn_Exp*_tmp134;struct _dyneither_ptr*_tmp135;struct Cyc_Absyn_Exp*
_tmp136;struct _dyneither_ptr*_tmp137;struct Cyc_Absyn_Exp*_tmp138;struct Cyc_Absyn_Exp*
_tmp139;struct Cyc_List_List*_tmp13A;struct _tuple7*_tmp13B;struct Cyc_List_List*
_tmp13C;struct Cyc_List_List*_tmp13D;struct Cyc_Absyn_Vardecl*_tmp13E;struct Cyc_Absyn_Exp*
_tmp13F;struct Cyc_Absyn_Exp*_tmp140;struct _tuple0*_tmp141;struct Cyc_List_List*
_tmp142;struct Cyc_List_List*_tmp143;struct Cyc_List_List*_tmp144;struct Cyc_List_List*
_tmp145;struct Cyc_Absyn_Datatypefield*_tmp146;struct _tuple0*_tmp147;struct
_tuple0*_tmp148;struct Cyc_Absyn_MallocInfo _tmp149;int _tmp14A;struct Cyc_Absyn_Exp*
_tmp14B;void**_tmp14C;struct Cyc_Absyn_Exp*_tmp14D;struct Cyc_Absyn_Exp*_tmp14E;
struct Cyc_Absyn_Exp*_tmp14F;struct Cyc_Core_Opt*_tmp150;struct Cyc_List_List*
_tmp151;struct Cyc_Absyn_Stmt*_tmp152;_LLE6: if(*((int*)_tmpE3)!= 0)goto _LLE8;
_tmpE4=((struct Cyc_Absyn_Const_e_struct*)_tmpE3)->f1;if((_tmpE4.Char_c).tag != 2)
goto _LLE8;_tmpE5=(struct _tuple3)(_tmpE4.Char_c).val;_tmpE6=_tmpE5.f1;_tmpE7=
_tmpE5.f2;_LLE7: Cyc_Absyndump_dump_char((int)'\'');Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_char_escape(_tmpE7));Cyc_Absyndump_dump_char((int)'\'');goto _LLE5;
_LLE8: if(*((int*)_tmpE3)!= 0)goto _LLEA;_tmpE8=((struct Cyc_Absyn_Const_e_struct*)
_tmpE3)->f1;if((_tmpE8.Short_c).tag != 3)goto _LLEA;_tmpE9=(struct _tuple4)(_tmpE8.Short_c).val;
_tmpEA=_tmpE9.f1;_tmpEB=_tmpE9.f2;_LLE9: Cyc_Absyndump_dump((struct _dyneither_ptr)({
struct Cyc_Int_pa_struct _tmp155;_tmp155.tag=1;_tmp155.f1=(unsigned long)((int)
_tmpEB);{void*_tmp153[1]={& _tmp155};Cyc_aprintf(({const char*_tmp154="%d";
_tag_dyneither(_tmp154,sizeof(char),3);}),_tag_dyneither(_tmp153,sizeof(void*),1));}}));
goto _LLE5;_LLEA: if(*((int*)_tmpE3)!= 0)goto _LLEC;_tmpEC=((struct Cyc_Absyn_Const_e_struct*)
_tmpE3)->f1;if((_tmpEC.Int_c).tag != 4)goto _LLEC;_tmpED=(struct _tuple5)(_tmpEC.Int_c).val;
_tmpEE=_tmpED.f1;if((int)_tmpEE != 2)goto _LLEC;_tmpEF=_tmpED.f2;_LLEB: _tmpF3=
_tmpEF;goto _LLED;_LLEC: if(*((int*)_tmpE3)!= 0)goto _LLEE;_tmpF0=((struct Cyc_Absyn_Const_e_struct*)
_tmpE3)->f1;if((_tmpF0.Int_c).tag != 4)goto _LLEE;_tmpF1=(struct _tuple5)(_tmpF0.Int_c).val;
_tmpF2=_tmpF1.f1;if((int)_tmpF2 != 0)goto _LLEE;_tmpF3=_tmpF1.f2;_LLED: Cyc_Absyndump_dump((
struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp158;_tmp158.tag=1;_tmp158.f1=(
unsigned long)_tmpF3;{void*_tmp156[1]={& _tmp158};Cyc_aprintf(({const char*_tmp157="%d";
_tag_dyneither(_tmp157,sizeof(char),3);}),_tag_dyneither(_tmp156,sizeof(void*),1));}}));
goto _LLE5;_LLEE: if(*((int*)_tmpE3)!= 0)goto _LLF0;_tmpF4=((struct Cyc_Absyn_Const_e_struct*)
_tmpE3)->f1;if((_tmpF4.Int_c).tag != 4)goto _LLF0;_tmpF5=(struct _tuple5)(_tmpF4.Int_c).val;
_tmpF6=_tmpF5.f1;if((int)_tmpF6 != 1)goto _LLF0;_tmpF7=_tmpF5.f2;_LLEF: Cyc_Absyndump_dump((
struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp15B;_tmp15B.tag=1;_tmp15B.f1=(
unsigned int)_tmpF7;{void*_tmp159[1]={& _tmp15B};Cyc_aprintf(({const char*_tmp15A="%u";
_tag_dyneither(_tmp15A,sizeof(char),3);}),_tag_dyneither(_tmp159,sizeof(void*),1));}}));
goto _LLE5;_LLF0: if(*((int*)_tmpE3)!= 0)goto _LLF2;_tmpF8=((struct Cyc_Absyn_Const_e_struct*)
_tmpE3)->f1;if((_tmpF8.LongLong_c).tag != 5)goto _LLF2;_tmpF9=(struct _tuple6)(
_tmpF8.LongLong_c).val;_tmpFA=_tmpF9.f1;_tmpFB=_tmpF9.f2;_LLF1: Cyc_Absyndump_dump(({
const char*_tmp15C="<<FIX LONG LONG CONSTANT>>";_tag_dyneither(_tmp15C,sizeof(
char),27);}));goto _LLE5;_LLF2: if(*((int*)_tmpE3)!= 0)goto _LLF4;_tmpFC=((struct
Cyc_Absyn_Const_e_struct*)_tmpE3)->f1;if((_tmpFC.Float_c).tag != 6)goto _LLF4;
_tmpFD=(struct _dyneither_ptr)(_tmpFC.Float_c).val;_LLF3: Cyc_Absyndump_dump(
_tmpFD);goto _LLE5;_LLF4: if(*((int*)_tmpE3)!= 0)goto _LLF6;_tmpFE=((struct Cyc_Absyn_Const_e_struct*)
_tmpE3)->f1;if((_tmpFE.Null_c).tag != 1)goto _LLF6;_tmpFF=(int)(_tmpFE.Null_c).val;
_LLF5: Cyc_Absyndump_dump(({const char*_tmp15D="NULL";_tag_dyneither(_tmp15D,
sizeof(char),5);}));goto _LLE5;_LLF6: if(*((int*)_tmpE3)!= 0)goto _LLF8;_tmp100=((
struct Cyc_Absyn_Const_e_struct*)_tmpE3)->f1;if((_tmp100.String_c).tag != 7)goto
_LLF8;_tmp101=(struct _dyneither_ptr)(_tmp100.String_c).val;_LLF7: Cyc_Absyndump_dump_char((
int)'"');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_tmp101));Cyc_Absyndump_dump_char((
int)'"');goto _LLE5;_LLF8: if(*((int*)_tmpE3)!= 2)goto _LLFA;_tmp102=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmpE3)->f1;_LLF9: _tmp103=_tmp102;goto _LLFB;_LLFA: if(*((int*)_tmpE3)!= 1)goto
_LLFC;_tmp103=((struct Cyc_Absyn_Var_e_struct*)_tmpE3)->f1;_LLFB: Cyc_Absyndump_dumpqvar(
_tmp103);goto _LLE5;_LLFC: if(*((int*)_tmpE3)!= 3)goto _LLFE;_tmp104=(void*)((
struct Cyc_Absyn_Primop_e_struct*)_tmpE3)->f1;_tmp105=((struct Cyc_Absyn_Primop_e_struct*)
_tmpE3)->f2;_LLFD: {struct _dyneither_ptr _tmp15E=Cyc_Absynpp_prim2str(_tmp104);
switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp105)){case 1: _LL150:
if(_tmp104 == (void*)((void*)19)){Cyc_Absyndump_dump(({const char*_tmp15F="numelts(";
_tag_dyneither(_tmp15F,sizeof(char),9);}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp105))->hd);Cyc_Absyndump_dump(({const char*
_tmp160=")";_tag_dyneither(_tmp160,sizeof(char),2);}));}else{Cyc_Absyndump_dump(
_tmp15E);Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp105))->hd);}break;case 2: _LL151: Cyc_Absyndump_dumpexp_prec(myprec,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp105))->hd);Cyc_Absyndump_dump(
_tmp15E);Cyc_Absyndump_dump_char((int)' ');Cyc_Absyndump_dumpexp_prec(myprec,(
struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp105->tl))->hd);
break;default: _LL152:(int)_throw((void*)({struct Cyc_Core_Failure_struct*_tmp161=
_cycalloc(sizeof(*_tmp161));_tmp161[0]=({struct Cyc_Core_Failure_struct _tmp162;
_tmp162.tag=Cyc_Core_Failure;_tmp162.f1=({const char*_tmp163="Absyndump -- Bad number of arguments to primop";
_tag_dyneither(_tmp163,sizeof(char),47);});_tmp162;});_tmp161;}));}goto _LLE5;}
_LLFE: if(*((int*)_tmpE3)!= 4)goto _LL100;_tmp106=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmpE3)->f1;_tmp107=((struct Cyc_Absyn_AssignOp_e_struct*)_tmpE3)->f2;_tmp108=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmpE3)->f3;_LLFF: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp106);if(_tmp107 != 0)Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((void*)
_tmp107->v));Cyc_Absyndump_dump_nospace(({const char*_tmp164="=";_tag_dyneither(
_tmp164,sizeof(char),2);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp108);goto _LLE5;
_LL100: if(*((int*)_tmpE3)!= 5)goto _LL102;_tmp109=((struct Cyc_Absyn_Increment_e_struct*)
_tmpE3)->f1;_tmp10A=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmpE3)->f2;if((
int)_tmp10A != 0)goto _LL102;_LL101: Cyc_Absyndump_dump(({const char*_tmp165="++";
_tag_dyneither(_tmp165,sizeof(char),3);}));Cyc_Absyndump_dumpexp_prec(myprec,
_tmp109);goto _LLE5;_LL102: if(*((int*)_tmpE3)!= 5)goto _LL104;_tmp10B=((struct Cyc_Absyn_Increment_e_struct*)
_tmpE3)->f1;_tmp10C=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmpE3)->f2;if((
int)_tmp10C != 2)goto _LL104;_LL103: Cyc_Absyndump_dump(({const char*_tmp166="--";
_tag_dyneither(_tmp166,sizeof(char),3);}));Cyc_Absyndump_dumpexp_prec(myprec,
_tmp10B);goto _LLE5;_LL104: if(*((int*)_tmpE3)!= 5)goto _LL106;_tmp10D=((struct Cyc_Absyn_Increment_e_struct*)
_tmpE3)->f1;_tmp10E=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmpE3)->f2;if((
int)_tmp10E != 1)goto _LL106;_LL105: Cyc_Absyndump_dumpexp_prec(myprec,_tmp10D);Cyc_Absyndump_dump(({
const char*_tmp167="++";_tag_dyneither(_tmp167,sizeof(char),3);}));goto _LLE5;
_LL106: if(*((int*)_tmpE3)!= 5)goto _LL108;_tmp10F=((struct Cyc_Absyn_Increment_e_struct*)
_tmpE3)->f1;_tmp110=(void*)((struct Cyc_Absyn_Increment_e_struct*)_tmpE3)->f2;if((
int)_tmp110 != 3)goto _LL108;_LL107: Cyc_Absyndump_dumpexp_prec(myprec,_tmp10F);Cyc_Absyndump_dump(({
const char*_tmp168="--";_tag_dyneither(_tmp168,sizeof(char),3);}));goto _LLE5;
_LL108: if(*((int*)_tmpE3)!= 6)goto _LL10A;_tmp111=((struct Cyc_Absyn_Conditional_e_struct*)
_tmpE3)->f1;_tmp112=((struct Cyc_Absyn_Conditional_e_struct*)_tmpE3)->f2;_tmp113=((
struct Cyc_Absyn_Conditional_e_struct*)_tmpE3)->f3;_LL109: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp111);Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumpexp_prec(0,
_tmp112);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp_prec(myprec,
_tmp113);goto _LLE5;_LL10A: if(*((int*)_tmpE3)!= 7)goto _LL10C;_tmp114=((struct Cyc_Absyn_And_e_struct*)
_tmpE3)->f1;_tmp115=((struct Cyc_Absyn_And_e_struct*)_tmpE3)->f2;_LL10B: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp114);Cyc_Absyndump_dump(({const char*_tmp169=" && ";_tag_dyneither(
_tmp169,sizeof(char),5);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp115);goto _LLE5;
_LL10C: if(*((int*)_tmpE3)!= 8)goto _LL10E;_tmp116=((struct Cyc_Absyn_Or_e_struct*)
_tmpE3)->f1;_tmp117=((struct Cyc_Absyn_Or_e_struct*)_tmpE3)->f2;_LL10D: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp116);Cyc_Absyndump_dump(({const char*_tmp16A=" || ";_tag_dyneither(
_tmp16A,sizeof(char),5);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp117);goto _LLE5;
_LL10E: if(*((int*)_tmpE3)!= 9)goto _LL110;_tmp118=((struct Cyc_Absyn_SeqExp_e_struct*)
_tmpE3)->f1;_tmp119=((struct Cyc_Absyn_SeqExp_e_struct*)_tmpE3)->f2;_LL10F: Cyc_Absyndump_dump_char((
int)'(');Cyc_Absyndump_dumpexp_prec(myprec,_tmp118);Cyc_Absyndump_dump_char((int)',');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp119);Cyc_Absyndump_dump_char((int)')');
goto _LLE5;_LL110: if(*((int*)_tmpE3)!= 10)goto _LL112;_tmp11A=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmpE3)->f1;_tmp11B=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmpE3)->f2;_LL111:
_tmp11C=_tmp11A;_tmp11D=_tmp11B;goto _LL113;_LL112: if(*((int*)_tmpE3)!= 11)goto
_LL114;_tmp11C=((struct Cyc_Absyn_FnCall_e_struct*)_tmpE3)->f1;_tmp11D=((struct
Cyc_Absyn_FnCall_e_struct*)_tmpE3)->f2;_LL113: Cyc_Absyndump_dumpexp_prec(myprec,
_tmp11C);Cyc_Absyndump_dump_nospace(({const char*_tmp16B="(";_tag_dyneither(
_tmp16B,sizeof(char),2);}));Cyc_Absyndump_dumpexps_prec(20,_tmp11D);Cyc_Absyndump_dump_nospace(({
const char*_tmp16C=")";_tag_dyneither(_tmp16C,sizeof(char),2);}));goto _LLE5;
_LL114: if(*((int*)_tmpE3)!= 12)goto _LL116;_tmp11E=((struct Cyc_Absyn_Throw_e_struct*)
_tmpE3)->f1;_LL115: Cyc_Absyndump_dump(({const char*_tmp16D="throw";_tag_dyneither(
_tmp16D,sizeof(char),6);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp11E);goto _LLE5;
_LL116: if(*((int*)_tmpE3)!= 13)goto _LL118;_tmp11F=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmpE3)->f1;_LL117: _tmp120=_tmp11F;goto _LL119;_LL118: if(*((int*)_tmpE3)!= 14)
goto _LL11A;_tmp120=((struct Cyc_Absyn_Instantiate_e_struct*)_tmpE3)->f1;_LL119:
Cyc_Absyndump_dumpexp_prec(inprec,_tmp120);goto _LLE5;_LL11A: if(*((int*)_tmpE3)!= 
15)goto _LL11C;_tmp121=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmpE3)->f1;
_tmp122=((struct Cyc_Absyn_Cast_e_struct*)_tmpE3)->f2;_LL11B: Cyc_Absyndump_dump_char((
int)'(');Cyc_Absyndump_dumptyp(_tmp121);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp122);goto _LLE5;_LL11C: if(*((int*)_tmpE3)!= 16)goto _LL11E;_tmp123=((
struct Cyc_Absyn_Address_e_struct*)_tmpE3)->f1;_LL11D: Cyc_Absyndump_dump_char((
int)'&');Cyc_Absyndump_dumpexp_prec(myprec,_tmp123);goto _LLE5;_LL11E: if(*((int*)
_tmpE3)!= 17)goto _LL120;_tmp124=((struct Cyc_Absyn_New_e_struct*)_tmpE3)->f1;
_tmp125=((struct Cyc_Absyn_New_e_struct*)_tmpE3)->f2;_LL11F: Cyc_Absyndump_dump(({
const char*_tmp16E="new ";_tag_dyneither(_tmp16E,sizeof(char),5);}));Cyc_Absyndump_dumpexp_prec(
myprec,_tmp125);goto _LLE5;_LL120: if(*((int*)_tmpE3)!= 18)goto _LL122;_tmp126=(
void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmpE3)->f1;_LL121: Cyc_Absyndump_dump(({
const char*_tmp16F="sizeof(";_tag_dyneither(_tmp16F,sizeof(char),8);}));Cyc_Absyndump_dumptyp(
_tmp126);Cyc_Absyndump_dump_char((int)')');goto _LLE5;_LL122: if(*((int*)_tmpE3)!= 
19)goto _LL124;_tmp127=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmpE3)->f1;_LL123:
Cyc_Absyndump_dump(({const char*_tmp170="sizeof(";_tag_dyneither(_tmp170,sizeof(
char),8);}));Cyc_Absyndump_dumpexp(_tmp127);Cyc_Absyndump_dump_char((int)')');
goto _LLE5;_LL124: if(*((int*)_tmpE3)!= 40)goto _LL126;_tmp128=(void*)((struct Cyc_Absyn_Valueof_e_struct*)
_tmpE3)->f1;_LL125: Cyc_Absyndump_dump(({const char*_tmp171="valueof(";
_tag_dyneither(_tmp171,sizeof(char),9);}));Cyc_Absyndump_dumptyp(_tmp128);Cyc_Absyndump_dump_char((
int)')');goto _LLE5;_LL126: if(*((int*)_tmpE3)!= 39)goto _LL128;_tmp129=((struct Cyc_Absyn_Tagcheck_e_struct*)
_tmpE3)->f1;_tmp12A=((struct Cyc_Absyn_Tagcheck_e_struct*)_tmpE3)->f2;_LL127: Cyc_Absyndump_dump(({
const char*_tmp172="tagcheck(";_tag_dyneither(_tmp172,sizeof(char),10);}));Cyc_Absyndump_dumpexp(
_tmp129);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp12A);
Cyc_Absyndump_dump_char((int)')');goto _LLE5;_LL128: if(*((int*)_tmpE3)!= 20)goto
_LL12A;_tmp12B=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpE3)->f1;_tmp12C=(
void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpE3)->f2;if(*((int*)_tmp12C)!= 0)
goto _LL12A;_tmp12D=((struct Cyc_Absyn_StructField_struct*)_tmp12C)->f1;_LL129: Cyc_Absyndump_dump(({
const char*_tmp173="offsetof(";_tag_dyneither(_tmp173,sizeof(char),10);}));Cyc_Absyndump_dumptyp(
_tmp12B);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dump_nospace(*_tmp12D);
Cyc_Absyndump_dump_char((int)')');goto _LLE5;_LL12A: if(*((int*)_tmpE3)!= 20)goto
_LL12C;_tmp12E=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpE3)->f1;_tmp12F=(
void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmpE3)->f2;if(*((int*)_tmp12F)!= 1)
goto _LL12C;_tmp130=((struct Cyc_Absyn_TupleIndex_struct*)_tmp12F)->f1;_LL12B: Cyc_Absyndump_dump(({
const char*_tmp174="offsetof(";_tag_dyneither(_tmp174,sizeof(char),10);}));Cyc_Absyndump_dumptyp(
_tmp12E);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dump((struct
_dyneither_ptr)({struct Cyc_Int_pa_struct _tmp177;_tmp177.tag=1;_tmp177.f1=(
unsigned long)((int)_tmp130);{void*_tmp175[1]={& _tmp177};Cyc_aprintf(({const char*
_tmp176="%d";_tag_dyneither(_tmp176,sizeof(char),3);}),_tag_dyneither(_tmp175,
sizeof(void*),1));}}));Cyc_Absyndump_dump_char((int)')');goto _LLE5;_LL12C: if(*((
int*)_tmpE3)!= 21)goto _LL12E;_tmp131=((struct Cyc_Absyn_Gentyp_e_struct*)_tmpE3)->f1;
_tmp132=(void*)((struct Cyc_Absyn_Gentyp_e_struct*)_tmpE3)->f2;_LL12D: Cyc_Absyndump_dump(({
const char*_tmp178="__gen(";_tag_dyneither(_tmp178,sizeof(char),7);}));Cyc_Absyndump_dumptvars(
_tmp131);Cyc_Absyndump_dumptyp(_tmp132);Cyc_Absyndump_dump_char((int)')');goto
_LLE5;_LL12E: if(*((int*)_tmpE3)!= 22)goto _LL130;_tmp133=((struct Cyc_Absyn_Deref_e_struct*)
_tmpE3)->f1;_LL12F: Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dumpexp_prec(
myprec,_tmp133);goto _LLE5;_LL130: if(*((int*)_tmpE3)!= 23)goto _LL132;_tmp134=((
struct Cyc_Absyn_AggrMember_e_struct*)_tmpE3)->f1;_tmp135=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmpE3)->f2;_LL131: Cyc_Absyndump_dumpexp_prec(myprec,_tmp134);Cyc_Absyndump_dump_char((
int)'.');Cyc_Absyndump_dump_nospace(*_tmp135);goto _LLE5;_LL132: if(*((int*)_tmpE3)
!= 24)goto _LL134;_tmp136=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpE3)->f1;
_tmp137=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmpE3)->f2;_LL133: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp136);Cyc_Absyndump_dump_nospace(({const char*_tmp179="->";
_tag_dyneither(_tmp179,sizeof(char),3);}));Cyc_Absyndump_dump_nospace(*_tmp137);
goto _LLE5;_LL134: if(*((int*)_tmpE3)!= 25)goto _LL136;_tmp138=((struct Cyc_Absyn_Subscript_e_struct*)
_tmpE3)->f1;_tmp139=((struct Cyc_Absyn_Subscript_e_struct*)_tmpE3)->f2;_LL135: Cyc_Absyndump_dumpexp_prec(
myprec,_tmp138);Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp139);
Cyc_Absyndump_dump_char((int)']');goto _LLE5;_LL136: if(*((int*)_tmpE3)!= 26)goto
_LL138;_tmp13A=((struct Cyc_Absyn_Tuple_e_struct*)_tmpE3)->f1;_LL137: Cyc_Absyndump_dump(({
const char*_tmp17A="$(";_tag_dyneither(_tmp17A,sizeof(char),3);}));Cyc_Absyndump_dumpexps_prec(
20,_tmp13A);Cyc_Absyndump_dump_char((int)')');goto _LLE5;_LL138: if(*((int*)_tmpE3)
!= 27)goto _LL13A;_tmp13B=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmpE3)->f1;
_tmp13C=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmpE3)->f2;_LL139: Cyc_Absyndump_dump_char((
int)'(');Cyc_Absyndump_dumptyp((*_tmp13B).f3);Cyc_Absyndump_dump_char((int)')');((
void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct _dyneither_ptr
start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,
_tmp13C,({const char*_tmp17B="{";_tag_dyneither(_tmp17B,sizeof(char),2);}),({
const char*_tmp17C="}";_tag_dyneither(_tmp17C,sizeof(char),2);}),({const char*
_tmp17D=",";_tag_dyneither(_tmp17D,sizeof(char),2);}));goto _LLE5;_LL13A: if(*((
int*)_tmpE3)!= 28)goto _LL13C;_tmp13D=((struct Cyc_Absyn_Array_e_struct*)_tmpE3)->f1;
_LL13B:((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp13D,({const char*_tmp17E="{";_tag_dyneither(_tmp17E,
sizeof(char),2);}),({const char*_tmp17F="}";_tag_dyneither(_tmp17F,sizeof(char),2);}),({
const char*_tmp180=",";_tag_dyneither(_tmp180,sizeof(char),2);}));goto _LLE5;
_LL13C: if(*((int*)_tmpE3)!= 29)goto _LL13E;_tmp13E=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmpE3)->f1;_tmp13F=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpE3)->f2;
_tmp140=((struct Cyc_Absyn_Comprehension_e_struct*)_tmpE3)->f3;_LL13D: Cyc_Absyndump_dump(({
const char*_tmp181="new {for";_tag_dyneither(_tmp181,sizeof(char),9);}));Cyc_Absyndump_dump_str((*
_tmp13E->name).f2);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(
_tmp13F);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(_tmp140);Cyc_Absyndump_dump_char((
int)'}');goto _LLE5;_LL13E: if(*((int*)_tmpE3)!= 30)goto _LL140;_tmp141=((struct Cyc_Absyn_Aggregate_e_struct*)
_tmpE3)->f1;_tmp142=((struct Cyc_Absyn_Aggregate_e_struct*)_tmpE3)->f2;_tmp143=((
struct Cyc_Absyn_Aggregate_e_struct*)_tmpE3)->f3;_LL13F: Cyc_Absyndump_dumpqvar(
_tmp141);Cyc_Absyndump_dump_char((int)'{');if(_tmp142 != 0)Cyc_Absyndump_dumptps(
_tmp142);((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp143,({const char*_tmp182="";_tag_dyneither(_tmp182,
sizeof(char),1);}),({const char*_tmp183="}";_tag_dyneither(_tmp183,sizeof(char),2);}),({
const char*_tmp184=",";_tag_dyneither(_tmp184,sizeof(char),2);}));goto _LLE5;
_LL140: if(*((int*)_tmpE3)!= 31)goto _LL142;_tmp144=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmpE3)->f2;_LL141:((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,
struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpde,_tmp144,({const char*_tmp185="{";_tag_dyneither(_tmp185,
sizeof(char),2);}),({const char*_tmp186="}";_tag_dyneither(_tmp186,sizeof(char),2);}),({
const char*_tmp187=",";_tag_dyneither(_tmp187,sizeof(char),2);}));goto _LLE5;
_LL142: if(*((int*)_tmpE3)!= 32)goto _LL144;_tmp145=((struct Cyc_Absyn_Datatype_e_struct*)
_tmpE3)->f1;_tmp146=((struct Cyc_Absyn_Datatype_e_struct*)_tmpE3)->f3;_LL143: Cyc_Absyndump_dumpqvar(
_tmp146->name);if(_tmp145 != 0)((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*
l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumpexp,_tmp145,({const char*_tmp188="(";_tag_dyneither(_tmp188,
sizeof(char),2);}),({const char*_tmp189=")";_tag_dyneither(_tmp189,sizeof(char),2);}),({
const char*_tmp18A=",";_tag_dyneither(_tmp18A,sizeof(char),2);}));goto _LLE5;
_LL144: if(*((int*)_tmpE3)!= 33)goto _LL146;_tmp147=((struct Cyc_Absyn_Enum_e_struct*)
_tmpE3)->f1;_LL145: Cyc_Absyndump_dumpqvar(_tmp147);goto _LLE5;_LL146: if(*((int*)
_tmpE3)!= 34)goto _LL148;_tmp148=((struct Cyc_Absyn_AnonEnum_e_struct*)_tmpE3)->f1;
_LL147: Cyc_Absyndump_dumpqvar(_tmp148);goto _LLE5;_LL148: if(*((int*)_tmpE3)!= 35)
goto _LL14A;_tmp149=((struct Cyc_Absyn_Malloc_e_struct*)_tmpE3)->f1;_tmp14A=
_tmp149.is_calloc;_tmp14B=_tmp149.rgn;_tmp14C=_tmp149.elt_type;_tmp14D=_tmp149.num_elts;
_LL149: if(_tmp14A){if(_tmp14B != 0){Cyc_Absyndump_dump(({const char*_tmp18B="rcalloc(";
_tag_dyneither(_tmp18B,sizeof(char),9);}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_tmp14B);Cyc_Absyndump_dump(({const char*_tmp18C=",";_tag_dyneither(_tmp18C,
sizeof(char),2);}));}else{Cyc_Absyndump_dump(({const char*_tmp18D="calloc";
_tag_dyneither(_tmp18D,sizeof(char),7);}));}Cyc_Absyndump_dumpexp(_tmp14D);Cyc_Absyndump_dump(({
const char*_tmp18E=",";_tag_dyneither(_tmp18E,sizeof(char),2);}));Cyc_Absyndump_dumpexp(
Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmp14C)),0));Cyc_Absyndump_dump(({
const char*_tmp18F=")";_tag_dyneither(_tmp18F,sizeof(char),2);}));}else{if(
_tmp14B != 0){Cyc_Absyndump_dump(({const char*_tmp190="rmalloc(";_tag_dyneither(
_tmp190,sizeof(char),9);}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp14B);
Cyc_Absyndump_dump(({const char*_tmp191=",";_tag_dyneither(_tmp191,sizeof(char),2);}));}
else{Cyc_Absyndump_dump(({const char*_tmp192="malloc(";_tag_dyneither(_tmp192,
sizeof(char),8);}));}if(_tmp14C != 0)Cyc_Absyndump_dumpexp(Cyc_Absyn_times_exp(
Cyc_Absyn_sizeoftyp_exp(*_tmp14C,0),_tmp14D,0));else{Cyc_Absyndump_dumpexp(
_tmp14D);}Cyc_Absyndump_dump(({const char*_tmp193=")";_tag_dyneither(_tmp193,
sizeof(char),2);}));}goto _LLE5;_LL14A: if(*((int*)_tmpE3)!= 36)goto _LL14C;_tmp14E=((
struct Cyc_Absyn_Swap_e_struct*)_tmpE3)->f1;_tmp14F=((struct Cyc_Absyn_Swap_e_struct*)
_tmpE3)->f2;_LL14B: Cyc_Absyndump_dump(({const char*_tmp194="cycswap(";
_tag_dyneither(_tmp194,sizeof(char),9);}));Cyc_Absyndump_dumpexp_prec(myprec,
_tmp14E);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(myprec,
_tmp14F);Cyc_Absyndump_dump_char((int)')');goto _LLE5;_LL14C: if(*((int*)_tmpE3)!= 
37)goto _LL14E;_tmp150=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpE3)->f1;
_tmp151=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmpE3)->f2;_LL14D:((void(*)(
void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,
_tmp151,({const char*_tmp195="{";_tag_dyneither(_tmp195,sizeof(char),2);}),({
const char*_tmp196="}";_tag_dyneither(_tmp196,sizeof(char),2);}),({const char*
_tmp197=",";_tag_dyneither(_tmp197,sizeof(char),2);}));goto _LLE5;_LL14E: if(*((
int*)_tmpE3)!= 38)goto _LLE5;_tmp152=((struct Cyc_Absyn_StmtExp_e_struct*)_tmpE3)->f1;
_LL14F: Cyc_Absyndump_dump_nospace(({const char*_tmp198="({";_tag_dyneither(
_tmp198,sizeof(char),3);}));Cyc_Absyndump_dumpstmt(_tmp152);Cyc_Absyndump_dump_nospace(({
const char*_tmp199="})";_tag_dyneither(_tmp199,sizeof(char),3);}));goto _LLE5;
_LLE5:;}if(inprec >= myprec)Cyc_Absyndump_dump_char((int)')');}void Cyc_Absyndump_dumpexp(
struct Cyc_Absyn_Exp*e){Cyc_Absyndump_dumpexp_prec(0,e);}void Cyc_Absyndump_dumpswitchclauses(
struct Cyc_List_List*scs){for(0;scs != 0;scs=scs->tl){struct Cyc_Absyn_Switch_clause*
_tmp19A=(struct Cyc_Absyn_Switch_clause*)scs->hd;if(_tmp19A->where_clause == 0  && (
_tmp19A->pattern)->r == (void*)((void*)0))Cyc_Absyndump_dump(({const char*_tmp19B="default:";
_tag_dyneither(_tmp19B,sizeof(char),9);}));else{Cyc_Absyndump_dump(({const char*
_tmp19C="case";_tag_dyneither(_tmp19C,sizeof(char),5);}));Cyc_Absyndump_dumppat(
_tmp19A->pattern);if(_tmp19A->where_clause != 0){Cyc_Absyndump_dump(({const char*
_tmp19D="&&";_tag_dyneither(_tmp19D,sizeof(char),3);}));Cyc_Absyndump_dumpexp((
struct Cyc_Absyn_Exp*)_check_null(_tmp19A->where_clause));}Cyc_Absyndump_dump_nospace(({
const char*_tmp19E=":";_tag_dyneither(_tmp19E,sizeof(char),2);}));}Cyc_Absyndump_dumpstmt(
_tmp19A->body);}}void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s){void*_tmp19F=
s->r;struct Cyc_Absyn_Exp*_tmp1A0;struct Cyc_Absyn_Stmt*_tmp1A1;struct Cyc_Absyn_Stmt*
_tmp1A2;struct Cyc_Absyn_Exp*_tmp1A3;struct Cyc_Absyn_Exp*_tmp1A4;struct Cyc_Absyn_Exp*
_tmp1A5;struct Cyc_Absyn_Stmt*_tmp1A6;struct Cyc_Absyn_Stmt*_tmp1A7;struct _tuple8
_tmp1A8;struct Cyc_Absyn_Exp*_tmp1A9;struct Cyc_Absyn_Stmt*_tmp1AA;struct
_dyneither_ptr*_tmp1AB;struct Cyc_Absyn_Exp*_tmp1AC;struct _tuple8 _tmp1AD;struct
Cyc_Absyn_Exp*_tmp1AE;struct _tuple8 _tmp1AF;struct Cyc_Absyn_Exp*_tmp1B0;struct Cyc_Absyn_Stmt*
_tmp1B1;struct Cyc_Absyn_Exp*_tmp1B2;struct Cyc_List_List*_tmp1B3;struct Cyc_Absyn_Decl*
_tmp1B4;struct Cyc_Absyn_Stmt*_tmp1B5;struct _dyneither_ptr*_tmp1B6;struct Cyc_Absyn_Stmt*
_tmp1B7;struct Cyc_Absyn_Stmt*_tmp1B8;struct _tuple8 _tmp1B9;struct Cyc_Absyn_Exp*
_tmp1BA;struct Cyc_List_List*_tmp1BB;struct Cyc_List_List*_tmp1BC;struct Cyc_Absyn_Stmt*
_tmp1BD;struct Cyc_List_List*_tmp1BE;struct Cyc_Absyn_Exp*_tmp1BF;_LL155: if((int)
_tmp19F != 0)goto _LL157;_LL156: Cyc_Absyndump_dump_semi();goto _LL154;_LL157: if(
_tmp19F <= (void*)1)goto _LL159;if(*((int*)_tmp19F)!= 0)goto _LL159;_tmp1A0=((
struct Cyc_Absyn_Exp_s_struct*)_tmp19F)->f1;_LL158: Cyc_Absyndump_dumploc(s->loc);
Cyc_Absyndump_dumpexp(_tmp1A0);Cyc_Absyndump_dump_semi();goto _LL154;_LL159: if(
_tmp19F <= (void*)1)goto _LL15B;if(*((int*)_tmp19F)!= 1)goto _LL15B;_tmp1A1=((
struct Cyc_Absyn_Seq_s_struct*)_tmp19F)->f1;_tmp1A2=((struct Cyc_Absyn_Seq_s_struct*)
_tmp19F)->f2;_LL15A: if(Cyc_Absynpp_is_declaration(_tmp1A1)){Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpstmt(_tmp1A1);Cyc_Absyndump_dump_char((int)'}');}
else{Cyc_Absyndump_dumpstmt(_tmp1A1);}if(Cyc_Absynpp_is_declaration(_tmp1A2)){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp1A2);Cyc_Absyndump_dump_char((
int)'}');}else{Cyc_Absyndump_dumpstmt(_tmp1A2);}goto _LL154;_LL15B: if(_tmp19F <= (
void*)1)goto _LL15D;if(*((int*)_tmp19F)!= 2)goto _LL15D;_tmp1A3=((struct Cyc_Absyn_Return_s_struct*)
_tmp19F)->f1;if(_tmp1A3 != 0)goto _LL15D;_LL15C: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({
const char*_tmp1C0="return;";_tag_dyneither(_tmp1C0,sizeof(char),8);}));goto
_LL154;_LL15D: if(_tmp19F <= (void*)1)goto _LL15F;if(*((int*)_tmp19F)!= 2)goto
_LL15F;_tmp1A4=((struct Cyc_Absyn_Return_s_struct*)_tmp19F)->f1;_LL15E: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp1C1="return";_tag_dyneither(_tmp1C1,
sizeof(char),7);}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(
_tmp1A4));Cyc_Absyndump_dump_semi();goto _LL154;_LL15F: if(_tmp19F <= (void*)1)goto
_LL161;if(*((int*)_tmp19F)!= 3)goto _LL161;_tmp1A5=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp19F)->f1;_tmp1A6=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp19F)->f2;_tmp1A7=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp19F)->f3;_LL160: Cyc_Absyndump_dumploc(s->loc);
Cyc_Absyndump_dump(({const char*_tmp1C2="if(";_tag_dyneither(_tmp1C2,sizeof(char),
4);}));Cyc_Absyndump_dumpexp(_tmp1A5);{void*_tmp1C3=_tmp1A6->r;_LL17C: if(_tmp1C3
<= (void*)1)goto _LL184;if(*((int*)_tmp1C3)!= 1)goto _LL17E;_LL17D: goto _LL17F;
_LL17E: if(*((int*)_tmp1C3)!= 11)goto _LL180;_LL17F: goto _LL181;_LL180: if(*((int*)
_tmp1C3)!= 3)goto _LL182;_LL181: goto _LL183;_LL182: if(*((int*)_tmp1C3)!= 12)goto
_LL184;_LL183: Cyc_Absyndump_dump_nospace(({const char*_tmp1C4="){";_tag_dyneither(
_tmp1C4,sizeof(char),3);}));Cyc_Absyndump_dumpstmt(_tmp1A6);Cyc_Absyndump_dump_char((
int)'}');goto _LL17B;_LL184:;_LL185: Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpstmt(
_tmp1A6);_LL17B:;}{void*_tmp1C5=_tmp1A7->r;_LL187: if((int)_tmp1C5 != 0)goto _LL189;
_LL188: goto _LL186;_LL189:;_LL18A: Cyc_Absyndump_dump(({const char*_tmp1C6="else{";
_tag_dyneither(_tmp1C6,sizeof(char),6);}));Cyc_Absyndump_dumpstmt(_tmp1A7);Cyc_Absyndump_dump_char((
int)'}');goto _LL186;_LL186:;}goto _LL154;_LL161: if(_tmp19F <= (void*)1)goto _LL163;
if(*((int*)_tmp19F)!= 4)goto _LL163;_tmp1A8=((struct Cyc_Absyn_While_s_struct*)
_tmp19F)->f1;_tmp1A9=_tmp1A8.f1;_tmp1AA=((struct Cyc_Absyn_While_s_struct*)
_tmp19F)->f2;_LL162: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*
_tmp1C7="while(";_tag_dyneither(_tmp1C7,sizeof(char),7);}));Cyc_Absyndump_dumpexp(
_tmp1A9);Cyc_Absyndump_dump_nospace(({const char*_tmp1C8="){";_tag_dyneither(
_tmp1C8,sizeof(char),3);}));Cyc_Absyndump_dumpstmt(_tmp1AA);Cyc_Absyndump_dump_char((
int)'}');goto _LL154;_LL163: if(_tmp19F <= (void*)1)goto _LL165;if(*((int*)_tmp19F)
!= 5)goto _LL165;_LL164: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const
char*_tmp1C9="break;";_tag_dyneither(_tmp1C9,sizeof(char),7);}));goto _LL154;
_LL165: if(_tmp19F <= (void*)1)goto _LL167;if(*((int*)_tmp19F)!= 6)goto _LL167;
_LL166: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*_tmp1CA="continue;";
_tag_dyneither(_tmp1CA,sizeof(char),10);}));goto _LL154;_LL167: if(_tmp19F <= (void*)
1)goto _LL169;if(*((int*)_tmp19F)!= 7)goto _LL169;_tmp1AB=((struct Cyc_Absyn_Goto_s_struct*)
_tmp19F)->f1;_LL168: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*
_tmp1CB="goto";_tag_dyneither(_tmp1CB,sizeof(char),5);}));Cyc_Absyndump_dump_str(
_tmp1AB);Cyc_Absyndump_dump_semi();goto _LL154;_LL169: if(_tmp19F <= (void*)1)goto
_LL16B;if(*((int*)_tmp19F)!= 8)goto _LL16B;_tmp1AC=((struct Cyc_Absyn_For_s_struct*)
_tmp19F)->f1;_tmp1AD=((struct Cyc_Absyn_For_s_struct*)_tmp19F)->f2;_tmp1AE=
_tmp1AD.f1;_tmp1AF=((struct Cyc_Absyn_For_s_struct*)_tmp19F)->f3;_tmp1B0=_tmp1AF.f1;
_tmp1B1=((struct Cyc_Absyn_For_s_struct*)_tmp19F)->f4;_LL16A: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp1CC="for(";_tag_dyneither(_tmp1CC,
sizeof(char),5);}));Cyc_Absyndump_dumpexp(_tmp1AC);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(
_tmp1AE);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp1B0);Cyc_Absyndump_dump_nospace(({
const char*_tmp1CD="){";_tag_dyneither(_tmp1CD,sizeof(char),3);}));Cyc_Absyndump_dumpstmt(
_tmp1B1);Cyc_Absyndump_dump_char((int)'}');goto _LL154;_LL16B: if(_tmp19F <= (void*)
1)goto _LL16D;if(*((int*)_tmp19F)!= 9)goto _LL16D;_tmp1B2=((struct Cyc_Absyn_Switch_s_struct*)
_tmp19F)->f1;_tmp1B3=((struct Cyc_Absyn_Switch_s_struct*)_tmp19F)->f2;_LL16C: Cyc_Absyndump_dumploc(
s->loc);Cyc_Absyndump_dump(({const char*_tmp1CE="switch(";_tag_dyneither(_tmp1CE,
sizeof(char),8);}));Cyc_Absyndump_dumpexp(_tmp1B2);Cyc_Absyndump_dump_nospace(({
const char*_tmp1CF="){";_tag_dyneither(_tmp1CF,sizeof(char),3);}));Cyc_Absyndump_dumpswitchclauses(
_tmp1B3);Cyc_Absyndump_dump_char((int)'}');goto _LL154;_LL16D: if(_tmp19F <= (void*)
1)goto _LL16F;if(*((int*)_tmp19F)!= 11)goto _LL16F;_tmp1B4=((struct Cyc_Absyn_Decl_s_struct*)
_tmp19F)->f1;_tmp1B5=((struct Cyc_Absyn_Decl_s_struct*)_tmp19F)->f2;_LL16E: Cyc_Absyndump_dumpdecl(
_tmp1B4);Cyc_Absyndump_dumpstmt(_tmp1B5);goto _LL154;_LL16F: if(_tmp19F <= (void*)1)
goto _LL171;if(*((int*)_tmp19F)!= 12)goto _LL171;_tmp1B6=((struct Cyc_Absyn_Label_s_struct*)
_tmp19F)->f1;_tmp1B7=((struct Cyc_Absyn_Label_s_struct*)_tmp19F)->f2;_LL170: if(
Cyc_Absynpp_is_declaration(_tmp1B7)){Cyc_Absyndump_dump_str(_tmp1B6);Cyc_Absyndump_dump_nospace(({
const char*_tmp1D0=": {";_tag_dyneither(_tmp1D0,sizeof(char),4);}));Cyc_Absyndump_dumpstmt(
_tmp1B7);Cyc_Absyndump_dump_char((int)'}');}else{Cyc_Absyndump_dump_str(_tmp1B6);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpstmt(_tmp1B7);}goto _LL154;
_LL171: if(_tmp19F <= (void*)1)goto _LL173;if(*((int*)_tmp19F)!= 13)goto _LL173;
_tmp1B8=((struct Cyc_Absyn_Do_s_struct*)_tmp19F)->f1;_tmp1B9=((struct Cyc_Absyn_Do_s_struct*)
_tmp19F)->f2;_tmp1BA=_tmp1B9.f1;_LL172: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({
const char*_tmp1D1="do{";_tag_dyneither(_tmp1D1,sizeof(char),4);}));Cyc_Absyndump_dumpstmt(
_tmp1B8);Cyc_Absyndump_dump_nospace(({const char*_tmp1D2="}while(";_tag_dyneither(
_tmp1D2,sizeof(char),8);}));Cyc_Absyndump_dumpexp(_tmp1BA);Cyc_Absyndump_dump_nospace(({
const char*_tmp1D3=");";_tag_dyneither(_tmp1D3,sizeof(char),3);}));goto _LL154;
_LL173: if(_tmp19F <= (void*)1)goto _LL175;if(*((int*)_tmp19F)!= 10)goto _LL175;
_tmp1BB=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp19F)->f1;if(_tmp1BB != 0)goto
_LL175;_LL174: Cyc_Absyndump_dump(({const char*_tmp1D4="fallthru;";_tag_dyneither(
_tmp1D4,sizeof(char),10);}));goto _LL154;_LL175: if(_tmp19F <= (void*)1)goto _LL177;
if(*((int*)_tmp19F)!= 10)goto _LL177;_tmp1BC=((struct Cyc_Absyn_Fallthru_s_struct*)
_tmp19F)->f1;_LL176: Cyc_Absyndump_dump(({const char*_tmp1D5="fallthru(";
_tag_dyneither(_tmp1D5,sizeof(char),10);}));Cyc_Absyndump_dumpexps_prec(20,
_tmp1BC);Cyc_Absyndump_dump_nospace(({const char*_tmp1D6=");";_tag_dyneither(
_tmp1D6,sizeof(char),3);}));goto _LL154;_LL177: if(_tmp19F <= (void*)1)goto _LL179;
if(*((int*)_tmp19F)!= 14)goto _LL179;_tmp1BD=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp19F)->f1;_tmp1BE=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp19F)->f2;_LL178:
Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*_tmp1D7="try";
_tag_dyneither(_tmp1D7,sizeof(char),4);}));Cyc_Absyndump_dumpstmt(_tmp1BD);Cyc_Absyndump_dump(({
const char*_tmp1D8="catch{";_tag_dyneither(_tmp1D8,sizeof(char),7);}));Cyc_Absyndump_dumpswitchclauses(
_tmp1BE);Cyc_Absyndump_dump_char((int)'}');goto _LL154;_LL179: if(_tmp19F <= (void*)
1)goto _LL154;if(*((int*)_tmp19F)!= 15)goto _LL154;_tmp1BF=((struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp19F)->f1;_LL17A: Cyc_Absyndump_dumploc(s->loc);Cyc_Absyndump_dump(({const char*
_tmp1D9="reset_region(";_tag_dyneither(_tmp1D9,sizeof(char),14);}));Cyc_Absyndump_dumpexp(
_tmp1BF);Cyc_Absyndump_dump(({const char*_tmp1DA=");";_tag_dyneither(_tmp1DA,
sizeof(char),3);}));goto _LL154;_LL154:;}struct _tuple17{struct Cyc_List_List*f1;
struct Cyc_Absyn_Pat*f2;};void Cyc_Absyndump_dumpdp(struct _tuple17*dp){Cyc_Absyndump_egroup(
Cyc_Absyndump_dumpdesignator,(*dp).f1,({const char*_tmp1DB="";_tag_dyneither(
_tmp1DB,sizeof(char),1);}),({const char*_tmp1DC="=";_tag_dyneither(_tmp1DC,
sizeof(char),2);}),({const char*_tmp1DD="=";_tag_dyneither(_tmp1DD,sizeof(char),2);}));
Cyc_Absyndump_dumppat((*dp).f2);}void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){
void*_tmp1DE=p->r;void*_tmp1DF;int _tmp1E0;void*_tmp1E1;int _tmp1E2;void*_tmp1E3;
int _tmp1E4;char _tmp1E5;struct _dyneither_ptr _tmp1E6;struct Cyc_Absyn_Vardecl*
_tmp1E7;struct Cyc_Absyn_Pat*_tmp1E8;struct Cyc_Absyn_Pat _tmp1E9;void*_tmp1EA;
struct Cyc_Absyn_Vardecl*_tmp1EB;struct Cyc_Absyn_Pat*_tmp1EC;struct Cyc_List_List*
_tmp1ED;int _tmp1EE;struct Cyc_Absyn_Pat*_tmp1EF;struct Cyc_Absyn_Vardecl*_tmp1F0;
struct Cyc_Absyn_Pat*_tmp1F1;struct Cyc_Absyn_Pat _tmp1F2;void*_tmp1F3;struct Cyc_Absyn_Vardecl*
_tmp1F4;struct Cyc_Absyn_Pat*_tmp1F5;struct Cyc_Absyn_Tvar*_tmp1F6;struct Cyc_Absyn_Vardecl*
_tmp1F7;struct _tuple0*_tmp1F8;struct _tuple0*_tmp1F9;struct Cyc_List_List*_tmp1FA;
int _tmp1FB;struct Cyc_Absyn_AggrInfo*_tmp1FC;struct Cyc_Absyn_AggrInfo _tmp1FD;
union Cyc_Absyn_AggrInfoU _tmp1FE;struct Cyc_List_List*_tmp1FF;struct Cyc_List_List*
_tmp200;int _tmp201;struct Cyc_Absyn_AggrInfo*_tmp202;struct Cyc_List_List*_tmp203;
struct Cyc_List_List*_tmp204;int _tmp205;struct Cyc_Absyn_Datatypefield*_tmp206;
struct Cyc_List_List*_tmp207;int _tmp208;struct Cyc_Absyn_Enumfield*_tmp209;struct
Cyc_Absyn_Enumfield*_tmp20A;struct Cyc_Absyn_Exp*_tmp20B;_LL18C: if((int)_tmp1DE != 
0)goto _LL18E;_LL18D: Cyc_Absyndump_dump_char((int)'_');goto _LL18B;_LL18E: if((int)
_tmp1DE != 1)goto _LL190;_LL18F: Cyc_Absyndump_dump(({const char*_tmp20C="NULL";
_tag_dyneither(_tmp20C,sizeof(char),5);}));goto _LL18B;_LL190: if(_tmp1DE <= (void*)
2)goto _LL192;if(*((int*)_tmp1DE)!= 7)goto _LL192;_tmp1DF=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp1DE)->f1;if((int)_tmp1DF != 2)goto _LL192;_tmp1E0=((struct Cyc_Absyn_Int_p_struct*)
_tmp1DE)->f2;_LL191: _tmp1E2=_tmp1E0;goto _LL193;_LL192: if(_tmp1DE <= (void*)2)goto
_LL194;if(*((int*)_tmp1DE)!= 7)goto _LL194;_tmp1E1=(void*)((struct Cyc_Absyn_Int_p_struct*)
_tmp1DE)->f1;if((int)_tmp1E1 != 0)goto _LL194;_tmp1E2=((struct Cyc_Absyn_Int_p_struct*)
_tmp1DE)->f2;_LL193: Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_struct
_tmp20F;_tmp20F.tag=1;_tmp20F.f1=(unsigned long)_tmp1E2;{void*_tmp20D[1]={&
_tmp20F};Cyc_aprintf(({const char*_tmp20E="%d";_tag_dyneither(_tmp20E,sizeof(char),
3);}),_tag_dyneither(_tmp20D,sizeof(void*),1));}}));goto _LL18B;_LL194: if(_tmp1DE
<= (void*)2)goto _LL196;if(*((int*)_tmp1DE)!= 7)goto _LL196;_tmp1E3=(void*)((
struct Cyc_Absyn_Int_p_struct*)_tmp1DE)->f1;if((int)_tmp1E3 != 1)goto _LL196;
_tmp1E4=((struct Cyc_Absyn_Int_p_struct*)_tmp1DE)->f2;_LL195: Cyc_Absyndump_dump((
struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp212;_tmp212.tag=1;_tmp212.f1=(
unsigned int)_tmp1E4;{void*_tmp210[1]={& _tmp212};Cyc_aprintf(({const char*_tmp211="%u";
_tag_dyneither(_tmp211,sizeof(char),3);}),_tag_dyneither(_tmp210,sizeof(void*),1));}}));
goto _LL18B;_LL196: if(_tmp1DE <= (void*)2)goto _LL198;if(*((int*)_tmp1DE)!= 8)goto
_LL198;_tmp1E5=((struct Cyc_Absyn_Char_p_struct*)_tmp1DE)->f1;_LL197: Cyc_Absyndump_dump(({
const char*_tmp213="'";_tag_dyneither(_tmp213,sizeof(char),2);}));Cyc_Absyndump_dump_nospace(
Cyc_Absynpp_char_escape(_tmp1E5));Cyc_Absyndump_dump_nospace(({const char*_tmp214="'";
_tag_dyneither(_tmp214,sizeof(char),2);}));goto _LL18B;_LL198: if(_tmp1DE <= (void*)
2)goto _LL19A;if(*((int*)_tmp1DE)!= 9)goto _LL19A;_tmp1E6=((struct Cyc_Absyn_Float_p_struct*)
_tmp1DE)->f1;_LL199: Cyc_Absyndump_dump(_tmp1E6);goto _LL18B;_LL19A: if(_tmp1DE <= (
void*)2)goto _LL19C;if(*((int*)_tmp1DE)!= 0)goto _LL19C;_tmp1E7=((struct Cyc_Absyn_Var_p_struct*)
_tmp1DE)->f1;_tmp1E8=((struct Cyc_Absyn_Var_p_struct*)_tmp1DE)->f2;_tmp1E9=*
_tmp1E8;_tmp1EA=_tmp1E9.r;if((int)_tmp1EA != 0)goto _LL19C;_LL19B: Cyc_Absyndump_dumpqvar(
_tmp1E7->name);goto _LL18B;_LL19C: if(_tmp1DE <= (void*)2)goto _LL19E;if(*((int*)
_tmp1DE)!= 0)goto _LL19E;_tmp1EB=((struct Cyc_Absyn_Var_p_struct*)_tmp1DE)->f1;
_tmp1EC=((struct Cyc_Absyn_Var_p_struct*)_tmp1DE)->f2;_LL19D: Cyc_Absyndump_dumpqvar(
_tmp1EB->name);Cyc_Absyndump_dump(({const char*_tmp215=" as ";_tag_dyneither(
_tmp215,sizeof(char),5);}));Cyc_Absyndump_dumppat(_tmp1EC);goto _LL18B;_LL19E: if(
_tmp1DE <= (void*)2)goto _LL1A0;if(*((int*)_tmp1DE)!= 3)goto _LL1A0;_tmp1ED=((
struct Cyc_Absyn_Tuple_p_struct*)_tmp1DE)->f1;_tmp1EE=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp1DE)->f2;_LL19F: {struct _dyneither_ptr term=_tmp1EE?({const char*_tmp218=", ...)";
_tag_dyneither(_tmp218,sizeof(char),7);}):({const char*_tmp219=")";_tag_dyneither(
_tmp219,sizeof(char),2);});((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*
l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dumppat,_tmp1ED,({const char*_tmp216="$(";_tag_dyneither(_tmp216,
sizeof(char),3);}),term,({const char*_tmp217=",";_tag_dyneither(_tmp217,sizeof(
char),2);}));goto _LL18B;}_LL1A0: if(_tmp1DE <= (void*)2)goto _LL1A2;if(*((int*)
_tmp1DE)!= 4)goto _LL1A2;_tmp1EF=((struct Cyc_Absyn_Pointer_p_struct*)_tmp1DE)->f1;
_LL1A1: Cyc_Absyndump_dump(({const char*_tmp21A="&";_tag_dyneither(_tmp21A,sizeof(
char),2);}));Cyc_Absyndump_dumppat(_tmp1EF);goto _LL18B;_LL1A2: if(_tmp1DE <= (void*)
2)goto _LL1A4;if(*((int*)_tmp1DE)!= 1)goto _LL1A4;_tmp1F0=((struct Cyc_Absyn_Reference_p_struct*)
_tmp1DE)->f1;_tmp1F1=((struct Cyc_Absyn_Reference_p_struct*)_tmp1DE)->f2;_tmp1F2=*
_tmp1F1;_tmp1F3=_tmp1F2.r;if((int)_tmp1F3 != 0)goto _LL1A4;_LL1A3: Cyc_Absyndump_dump(({
const char*_tmp21B="*";_tag_dyneither(_tmp21B,sizeof(char),2);}));Cyc_Absyndump_dumpqvar(
_tmp1F0->name);goto _LL18B;_LL1A4: if(_tmp1DE <= (void*)2)goto _LL1A6;if(*((int*)
_tmp1DE)!= 1)goto _LL1A6;_tmp1F4=((struct Cyc_Absyn_Reference_p_struct*)_tmp1DE)->f1;
_tmp1F5=((struct Cyc_Absyn_Reference_p_struct*)_tmp1DE)->f2;_LL1A5: Cyc_Absyndump_dump(({
const char*_tmp21C="*";_tag_dyneither(_tmp21C,sizeof(char),2);}));Cyc_Absyndump_dumpqvar(
_tmp1F4->name);Cyc_Absyndump_dump(({const char*_tmp21D=" as ";_tag_dyneither(
_tmp21D,sizeof(char),5);}));Cyc_Absyndump_dumppat(_tmp1F5);goto _LL18B;_LL1A6: if(
_tmp1DE <= (void*)2)goto _LL1A8;if(*((int*)_tmp1DE)!= 2)goto _LL1A8;_tmp1F6=((
struct Cyc_Absyn_TagInt_p_struct*)_tmp1DE)->f1;_tmp1F7=((struct Cyc_Absyn_TagInt_p_struct*)
_tmp1DE)->f2;_LL1A7: Cyc_Absyndump_dumpqvar(_tmp1F7->name);Cyc_Absyndump_dump_char((
int)'<');Cyc_Absyndump_dumptvar(_tmp1F6);Cyc_Absyndump_dump_char((int)'>');goto
_LL18B;_LL1A8: if(_tmp1DE <= (void*)2)goto _LL1AA;if(*((int*)_tmp1DE)!= 12)goto
_LL1AA;_tmp1F8=((struct Cyc_Absyn_UnknownId_p_struct*)_tmp1DE)->f1;_LL1A9: Cyc_Absyndump_dumpqvar(
_tmp1F8);goto _LL18B;_LL1AA: if(_tmp1DE <= (void*)2)goto _LL1AC;if(*((int*)_tmp1DE)
!= 13)goto _LL1AC;_tmp1F9=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp1DE)->f1;
_tmp1FA=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp1DE)->f2;_tmp1FB=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp1DE)->f3;_LL1AB: {struct _dyneither_ptr term=_tmp1FB?({const char*_tmp220=", ...)";
_tag_dyneither(_tmp220,sizeof(char),7);}):({const char*_tmp221=")";_tag_dyneither(
_tmp221,sizeof(char),2);});Cyc_Absyndump_dumpqvar(_tmp1F9);((void(*)(void(*f)(
struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,
_tmp1FA,({const char*_tmp21E="(";_tag_dyneither(_tmp21E,sizeof(char),2);}),term,({
const char*_tmp21F=",";_tag_dyneither(_tmp21F,sizeof(char),2);}));goto _LL18B;}
_LL1AC: if(_tmp1DE <= (void*)2)goto _LL1AE;if(*((int*)_tmp1DE)!= 5)goto _LL1AE;
_tmp1FC=((struct Cyc_Absyn_Aggr_p_struct*)_tmp1DE)->f1;if(_tmp1FC == 0)goto _LL1AE;
_tmp1FD=*_tmp1FC;_tmp1FE=_tmp1FD.aggr_info;_tmp1FF=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp1DE)->f2;_tmp200=((struct Cyc_Absyn_Aggr_p_struct*)_tmp1DE)->f3;_tmp201=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp1DE)->f4;_LL1AD: {struct _tuple0*_tmp223;
struct _tuple9 _tmp222=Cyc_Absyn_aggr_kinded_name(_tmp1FE);_tmp223=_tmp222.f2;{
struct _dyneither_ptr term=_tmp201?({const char*_tmp229=", ...)";_tag_dyneither(
_tmp229,sizeof(char),7);}):({const char*_tmp22A=")";_tag_dyneither(_tmp22A,
sizeof(char),2);});Cyc_Absyndump_dumpqvar(_tmp223);Cyc_Absyndump_dump_char((int)'{');((
void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumptvar,_tmp1FF,({const char*_tmp224="[";_tag_dyneither(_tmp224,
sizeof(char),2);}),({const char*_tmp225="]";_tag_dyneither(_tmp225,sizeof(char),2);}),({
const char*_tmp226=",";_tag_dyneither(_tmp226,sizeof(char),2);}));((void(*)(void(*
f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,
_tmp200,({const char*_tmp227="";_tag_dyneither(_tmp227,sizeof(char),1);}),term,({
const char*_tmp228=",";_tag_dyneither(_tmp228,sizeof(char),2);}));goto _LL18B;}}
_LL1AE: if(_tmp1DE <= (void*)2)goto _LL1B0;if(*((int*)_tmp1DE)!= 5)goto _LL1B0;
_tmp202=((struct Cyc_Absyn_Aggr_p_struct*)_tmp1DE)->f1;if(_tmp202 != 0)goto _LL1B0;
_tmp203=((struct Cyc_Absyn_Aggr_p_struct*)_tmp1DE)->f2;_tmp204=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp1DE)->f3;_tmp205=((struct Cyc_Absyn_Aggr_p_struct*)_tmp1DE)->f4;_LL1AF: {
struct _dyneither_ptr term=_tmp205?({const char*_tmp230=", ...)";_tag_dyneither(
_tmp230,sizeof(char),7);}):({const char*_tmp231=")";_tag_dyneither(_tmp231,
sizeof(char),2);});Cyc_Absyndump_dump_char((int)'{');((void(*)(void(*f)(struct
Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct
_dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,
_tmp203,({const char*_tmp22B="[";_tag_dyneither(_tmp22B,sizeof(char),2);}),({
const char*_tmp22C="]";_tag_dyneither(_tmp22C,sizeof(char),2);}),({const char*
_tmp22D=",";_tag_dyneither(_tmp22D,sizeof(char),2);}));((void(*)(void(*f)(struct
_tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr
end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp204,({
const char*_tmp22E="";_tag_dyneither(_tmp22E,sizeof(char),1);}),term,({const char*
_tmp22F=",";_tag_dyneither(_tmp22F,sizeof(char),2);}));goto _LL18B;}_LL1B0: if(
_tmp1DE <= (void*)2)goto _LL1B2;if(*((int*)_tmp1DE)!= 6)goto _LL1B2;_tmp206=((
struct Cyc_Absyn_Datatype_p_struct*)_tmp1DE)->f2;_tmp207=((struct Cyc_Absyn_Datatype_p_struct*)
_tmp1DE)->f3;_tmp208=((struct Cyc_Absyn_Datatype_p_struct*)_tmp1DE)->f4;_LL1B1: {
struct _dyneither_ptr term=_tmp208?({const char*_tmp234=", ...)";_tag_dyneither(
_tmp234,sizeof(char),7);}):({const char*_tmp235=")";_tag_dyneither(_tmp235,
sizeof(char),2);});Cyc_Absyndump_dumpqvar(_tmp206->name);if(_tmp207 != 0)((void(*)(
void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,
struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,
_tmp207,({const char*_tmp232="(";_tag_dyneither(_tmp232,sizeof(char),2);}),term,({
const char*_tmp233=",";_tag_dyneither(_tmp233,sizeof(char),2);}));goto _LL18B;}
_LL1B2: if(_tmp1DE <= (void*)2)goto _LL1B4;if(*((int*)_tmp1DE)!= 10)goto _LL1B4;
_tmp209=((struct Cyc_Absyn_Enum_p_struct*)_tmp1DE)->f2;_LL1B3: _tmp20A=_tmp209;
goto _LL1B5;_LL1B4: if(_tmp1DE <= (void*)2)goto _LL1B6;if(*((int*)_tmp1DE)!= 11)goto
_LL1B6;_tmp20A=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp1DE)->f2;_LL1B5: Cyc_Absyndump_dumpqvar(
_tmp20A->name);goto _LL18B;_LL1B6: if(_tmp1DE <= (void*)2)goto _LL18B;if(*((int*)
_tmp1DE)!= 14)goto _LL18B;_tmp20B=((struct Cyc_Absyn_Exp_p_struct*)_tmp1DE)->f1;
_LL1B7: Cyc_Absyndump_dumpexp(_tmp20B);goto _LL18B;_LL18B:;}void Cyc_Absyndump_dumpdatatypefield(
struct Cyc_Absyn_Datatypefield*ef){Cyc_Absyndump_dumpqvar(ef->name);if(ef->typs != 
0)Cyc_Absyndump_dumpargs(ef->typs);}void Cyc_Absyndump_dumpdatatypefields(struct
Cyc_List_List*fields){((void(*)(void(*f)(struct Cyc_Absyn_Datatypefield*),struct
Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpdatatypefield,
fields,({const char*_tmp236=",";_tag_dyneither(_tmp236,sizeof(char),2);}));}void
Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield*ef){Cyc_Absyndump_dumpqvar(
ef->name);if(ef->tag != 0){Cyc_Absyndump_dump(({const char*_tmp237=" = ";
_tag_dyneither(_tmp237,sizeof(char),4);}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)
_check_null(ef->tag));}}void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*
fields){((void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l,
struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,
fields,({const char*_tmp238=",";_tag_dyneither(_tmp238,sizeof(char),2);}));}void
Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields){for(0;fields != 0;fields=
fields->tl){struct Cyc_Absyn_Aggrfield _tmp23A;struct _dyneither_ptr*_tmp23B;struct
Cyc_Absyn_Tqual _tmp23C;void*_tmp23D;struct Cyc_Absyn_Exp*_tmp23E;struct Cyc_List_List*
_tmp23F;struct Cyc_Absyn_Aggrfield*_tmp239=(struct Cyc_Absyn_Aggrfield*)fields->hd;
_tmp23A=*_tmp239;_tmp23B=_tmp23A.name;_tmp23C=_tmp23A.tq;_tmp23D=_tmp23A.type;
_tmp23E=_tmp23A.width;_tmp23F=_tmp23A.attributes;{void*_tmp240=Cyc_Cyclone_c_compiler;
_LL1B9: if((int)_tmp240 != 0)goto _LL1BB;_LL1BA:((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(
_tmp23C,_tmp23D,Cyc_Absyndump_dump_str,_tmp23B);Cyc_Absyndump_dumpatts(_tmp23F);
goto _LL1B8;_LL1BB: if((int)_tmp240 != 1)goto _LL1B8;_LL1BC: Cyc_Absyndump_dumpatts(
_tmp23F);((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),
struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp23C,_tmp23D,Cyc_Absyndump_dump_str,
_tmp23B);goto _LL1B8;_LL1B8:;}if(_tmp23E != 0){Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp23E);}Cyc_Absyndump_dump_semi();}}
void Cyc_Absyndump_dumptypedefname(struct Cyc_Absyn_Typedefdecl*td){Cyc_Absyndump_dumpqvar(
td->name);Cyc_Absyndump_dumptvars(td->tvs);}static void Cyc_Absyndump_dump_atts_qvar(
struct Cyc_Absyn_Fndecl*fd){Cyc_Absyndump_dumpatts(fd->attributes);Cyc_Absyndump_dumpqvar(
fd->name);}static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple9*pr){{void*
_tmp241=(*pr).f1;_LL1BE: if((int)_tmp241 != 8)goto _LL1C0;_LL1BF: goto _LL1BD;_LL1C0:
if((int)_tmp241 != 0)goto _LL1C2;_LL1C1: Cyc_Absyndump_dump(({const char*_tmp242="_stdcall";
_tag_dyneither(_tmp242,sizeof(char),9);}));goto _LL1BD;_LL1C2: if((int)_tmp241 != 1)
goto _LL1C4;_LL1C3: Cyc_Absyndump_dump(({const char*_tmp243="_cdecl";_tag_dyneither(
_tmp243,sizeof(char),7);}));goto _LL1BD;_LL1C4: if((int)_tmp241 != 2)goto _LL1C6;
_LL1C5: Cyc_Absyndump_dump(({const char*_tmp244="_fastcall";_tag_dyneither(_tmp244,
sizeof(char),10);}));goto _LL1BD;_LL1C6:;_LL1C7: goto _LL1BD;_LL1BD:;}Cyc_Absyndump_dumpqvar((*
pr).f2);}static void Cyc_Absyndump_dump_callconv_fdqvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dump_callconv(fd->attributes);Cyc_Absyndump_dumpqvar(fd->name);}
static void Cyc_Absyndump_dumpids(struct Cyc_List_List*vds){for(0;vds != 0;vds=vds->tl){
Cyc_Absyndump_dumpqvar(((struct Cyc_Absyn_Vardecl*)vds->hd)->name);if(vds->tl != 0)
Cyc_Absyndump_dump_char((int)',');}}void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*
vd,struct Cyc_Position_Segment*loc){struct Cyc_Absyn_Vardecl _tmp246;void*_tmp247;
struct _tuple0*_tmp248;struct Cyc_Absyn_Tqual _tmp249;void*_tmp24A;struct Cyc_Absyn_Exp*
_tmp24B;struct Cyc_List_List*_tmp24C;struct Cyc_Absyn_Vardecl*_tmp245=vd;_tmp246=*
_tmp245;_tmp247=_tmp246.sc;_tmp248=_tmp246.name;_tmp249=_tmp246.tq;_tmp24A=
_tmp246.type;_tmp24B=_tmp246.initializer;_tmp24C=_tmp246.attributes;Cyc_Absyndump_dumploc(
loc);{void*_tmp24D=Cyc_Cyclone_c_compiler;_LL1C9: if((int)_tmp24D != 0)goto _LL1CB;
_LL1CA: if(_tmp247 == (void*)3  && Cyc_Absyndump_qvar_to_Cids){void*_tmp24E=Cyc_Tcutil_compress(
_tmp24A);_LL1CE: if(_tmp24E <= (void*)4)goto _LL1D0;if(*((int*)_tmp24E)!= 8)goto
_LL1D0;_LL1CF: goto _LL1CD;_LL1D0:;_LL1D1: Cyc_Absyndump_dumpscope(_tmp247);_LL1CD:;}
else{Cyc_Absyndump_dumpscope(_tmp247);}((void(*)(struct Cyc_Absyn_Tqual,void*,
void(*f)(struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd)(_tmp249,_tmp24A,
Cyc_Absyndump_dumpqvar,_tmp248);Cyc_Absyndump_dumpatts(_tmp24C);goto _LL1C8;
_LL1CB: if((int)_tmp24D != 1)goto _LL1C8;_LL1CC: Cyc_Absyndump_dumpatts(_tmp24C);Cyc_Absyndump_dumpscope(
_tmp247);{struct _RegionHandle _tmp24F=_new_region("temp");struct _RegionHandle*
temp=& _tmp24F;_push_region(temp);{struct Cyc_Absyn_Tqual _tmp251;void*_tmp252;
struct Cyc_List_List*_tmp253;struct _tuple11 _tmp250=Cyc_Absynpp_to_tms(temp,
_tmp249,_tmp24A);_tmp251=_tmp250.f1;_tmp252=_tmp250.f2;_tmp253=_tmp250.f3;{void*
call_conv=(void*)8;{struct Cyc_List_List*tms2=_tmp253;for(0;tms2 != 0;tms2=tms2->tl){
void*_tmp254=(void*)tms2->hd;struct Cyc_List_List*_tmp255;_LL1D3: if(*((int*)
_tmp254)!= 5)goto _LL1D5;_tmp255=((struct Cyc_Absyn_Attributes_mod_struct*)_tmp254)->f2;
_LL1D4: for(0;_tmp255 != 0;_tmp255=_tmp255->tl){void*_tmp256=(void*)_tmp255->hd;
_LL1D8: if((int)_tmp256 != 0)goto _LL1DA;_LL1D9: call_conv=(void*)0;goto _LL1D7;
_LL1DA: if((int)_tmp256 != 1)goto _LL1DC;_LL1DB: call_conv=(void*)1;goto _LL1D7;
_LL1DC: if((int)_tmp256 != 2)goto _LL1DE;_LL1DD: call_conv=(void*)2;goto _LL1D7;
_LL1DE:;_LL1DF: goto _LL1D7;_LL1D7:;}goto _LL1D2;_LL1D5:;_LL1D6: goto _LL1D2;_LL1D2:;}}
Cyc_Absyndump_dumptq(_tmp251);Cyc_Absyndump_dumpntyp(_tmp252);{struct _tuple9
_tmp257=({struct _tuple9 _tmp258;_tmp258.f1=call_conv;_tmp258.f2=_tmp248;_tmp258;});((
void(*)(struct Cyc_List_List*tms,void(*f)(struct _tuple9*),struct _tuple9*a))Cyc_Absyndump_dumptms)(
Cyc_List_imp_rev(_tmp253),Cyc_Absyndump_dump_callconv_qvar,& _tmp257);}}}
_npop_handler(0);goto _LL1C8;;_pop_region(temp);}_LL1C8:;}if(_tmp24B != 0){Cyc_Absyndump_dump_char((
int)'=');Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp24B);}Cyc_Absyndump_dump_semi();}
struct _tuple18{struct Cyc_Position_Segment*f1;struct _tuple0*f2;int f3;};void Cyc_Absyndump_dumpdecl(
struct Cyc_Absyn_Decl*d){void*_tmp259=d->r;struct Cyc_Absyn_Vardecl*_tmp25A;struct
Cyc_Absyn_Fndecl*_tmp25B;struct Cyc_Absyn_Aggrdecl*_tmp25C;struct Cyc_Absyn_Datatypedecl*
_tmp25D;struct Cyc_Absyn_Datatypedecl _tmp25E;void*_tmp25F;struct _tuple0*_tmp260;
struct Cyc_List_List*_tmp261;struct Cyc_Core_Opt*_tmp262;int _tmp263;struct Cyc_Absyn_Enumdecl*
_tmp264;struct Cyc_Absyn_Enumdecl _tmp265;void*_tmp266;struct _tuple0*_tmp267;
struct Cyc_Core_Opt*_tmp268;struct Cyc_Absyn_Pat*_tmp269;struct Cyc_Absyn_Exp*
_tmp26A;struct Cyc_List_List*_tmp26B;struct Cyc_Absyn_Tvar*_tmp26C;struct Cyc_Absyn_Vardecl*
_tmp26D;int _tmp26E;struct Cyc_Absyn_Exp*_tmp26F;struct Cyc_Absyn_Exp*_tmp270;
struct Cyc_Absyn_Tvar*_tmp271;struct Cyc_Absyn_Vardecl*_tmp272;struct Cyc_Absyn_Typedefdecl*
_tmp273;struct _dyneither_ptr*_tmp274;struct Cyc_List_List*_tmp275;struct _tuple0*
_tmp276;struct Cyc_List_List*_tmp277;struct Cyc_List_List*_tmp278;struct Cyc_List_List*
_tmp279;struct Cyc_List_List*_tmp27A;_LL1E1: if(_tmp259 <= (void*)2)goto _LL1FD;if(*((
int*)_tmp259)!= 0)goto _LL1E3;_tmp25A=((struct Cyc_Absyn_Var_d_struct*)_tmp259)->f1;
_LL1E2: Cyc_Absyndump_dumpvardecl(_tmp25A,d->loc);goto _LL1E0;_LL1E3: if(*((int*)
_tmp259)!= 1)goto _LL1E5;_tmp25B=((struct Cyc_Absyn_Fn_d_struct*)_tmp259)->f1;
_LL1E4: Cyc_Absyndump_dumploc(d->loc);{void*_tmp27B=Cyc_Cyclone_c_compiler;_LL202:
if((int)_tmp27B != 1)goto _LL204;_LL203: Cyc_Absyndump_dumpatts(_tmp25B->attributes);
goto _LL201;_LL204: if((int)_tmp27B != 0)goto _LL201;_LL205: goto _LL201;_LL201:;}if(
_tmp25B->is_inline){void*_tmp27C=Cyc_Cyclone_c_compiler;_LL207: if((int)_tmp27C != 
1)goto _LL209;_LL208: Cyc_Absyndump_dump(({const char*_tmp27D="__inline";
_tag_dyneither(_tmp27D,sizeof(char),9);}));goto _LL206;_LL209:;_LL20A: Cyc_Absyndump_dump(({
const char*_tmp27E="inline";_tag_dyneither(_tmp27E,sizeof(char),7);}));goto _LL206;
_LL206:;}Cyc_Absyndump_dumpscope(_tmp25B->sc);{void*t=(void*)({struct Cyc_Absyn_FnType_struct*
_tmp280=_cycalloc(sizeof(*_tmp280));_tmp280[0]=({struct Cyc_Absyn_FnType_struct
_tmp281;_tmp281.tag=8;_tmp281.f1=({struct Cyc_Absyn_FnInfo _tmp282;_tmp282.tvars=
_tmp25B->tvs;_tmp282.effect=_tmp25B->effect;_tmp282.ret_typ=_tmp25B->ret_type;
_tmp282.args=((struct Cyc_List_List*(*)(struct _tuple7*(*f)(struct _tuple10*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp25B->args);
_tmp282.c_varargs=_tmp25B->c_varargs;_tmp282.cyc_varargs=_tmp25B->cyc_varargs;
_tmp282.rgn_po=_tmp25B->rgn_po;_tmp282.attributes=0;_tmp282;});_tmp281;});
_tmp280;});{void*_tmp27F=Cyc_Cyclone_c_compiler;_LL20C: if((int)_tmp27F != 0)goto
_LL20E;_LL20D:((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),
struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_atts_qvar,
_tmp25B);goto _LL20B;_LL20E: if((int)_tmp27F != 1)goto _LL20B;_LL20F:((void(*)(
struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))
Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_callconv_fdqvar,
_tmp25B);goto _LL20B;_LL20B:;}Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(
_tmp25B->body);Cyc_Absyndump_dump_char((int)'}');goto _LL1E0;}_LL1E5: if(*((int*)
_tmp259)!= 6)goto _LL1E7;_tmp25C=((struct Cyc_Absyn_Aggr_d_struct*)_tmp259)->f1;
_LL1E6: Cyc_Absyndump_dumpscope(_tmp25C->sc);Cyc_Absyndump_dumpaggr_kind(_tmp25C->kind);
Cyc_Absyndump_dumpqvar(_tmp25C->name);Cyc_Absyndump_dumptvars(_tmp25C->tvs);if(
_tmp25C->impl == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((int)'{');
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp25C->impl))->exist_vars != 0)((
void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(
Cyc_Absyndump_dumpkindedtvar,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp25C->impl))->exist_vars,({
const char*_tmp283="<";_tag_dyneither(_tmp283,sizeof(char),2);}),({const char*
_tmp284=">";_tag_dyneither(_tmp284,sizeof(char),2);}),({const char*_tmp285=",";
_tag_dyneither(_tmp285,sizeof(char),2);}));if(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp25C->impl))->rgn_po != 0){Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dump_rgnpo(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp25C->impl))->rgn_po);}Cyc_Absyndump_dumpaggrfields(((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp25C->impl))->fields);Cyc_Absyndump_dump(({
const char*_tmp286="}";_tag_dyneither(_tmp286,sizeof(char),2);}));Cyc_Absyndump_dumpatts(
_tmp25C->attributes);Cyc_Absyndump_dump(({const char*_tmp287=";";_tag_dyneither(
_tmp287,sizeof(char),2);}));}goto _LL1E0;_LL1E7: if(*((int*)_tmp259)!= 7)goto
_LL1E9;_tmp25D=((struct Cyc_Absyn_Datatype_d_struct*)_tmp259)->f1;_tmp25E=*
_tmp25D;_tmp25F=_tmp25E.sc;_tmp260=_tmp25E.name;_tmp261=_tmp25E.tvs;_tmp262=
_tmp25E.fields;_tmp263=_tmp25E.is_extensible;_LL1E8: Cyc_Absyndump_dumpscope(
_tmp25F);if(_tmp263)Cyc_Absyndump_dump(({const char*_tmp288="@extensible ";
_tag_dyneither(_tmp288,sizeof(char),13);}));Cyc_Absyndump_dump(({const char*
_tmp289="datatype ";_tag_dyneither(_tmp289,sizeof(char),10);}));Cyc_Absyndump_dumpqvar(
_tmp260);Cyc_Absyndump_dumptvars(_tmp261);if(_tmp262 == 0)Cyc_Absyndump_dump_semi();
else{Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpdatatypefields((struct
Cyc_List_List*)_tmp262->v);Cyc_Absyndump_dump_nospace(({const char*_tmp28A="};";
_tag_dyneither(_tmp28A,sizeof(char),3);}));}goto _LL1E0;_LL1E9: if(*((int*)_tmp259)
!= 8)goto _LL1EB;_tmp264=((struct Cyc_Absyn_Enum_d_struct*)_tmp259)->f1;_tmp265=*
_tmp264;_tmp266=_tmp265.sc;_tmp267=_tmp265.name;_tmp268=_tmp265.fields;_LL1EA:
Cyc_Absyndump_dumpscope(_tmp266);Cyc_Absyndump_dump(({const char*_tmp28B="enum ";
_tag_dyneither(_tmp28B,sizeof(char),6);}));Cyc_Absyndump_dumpqvar(_tmp267);if(
_tmp268 == 0)Cyc_Absyndump_dump_semi();else{Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpenumfields((
struct Cyc_List_List*)_tmp268->v);Cyc_Absyndump_dump_nospace(({const char*_tmp28C="};";
_tag_dyneither(_tmp28C,sizeof(char),3);}));}return;_LL1EB: if(*((int*)_tmp259)!= 
2)goto _LL1ED;_tmp269=((struct Cyc_Absyn_Let_d_struct*)_tmp259)->f1;_tmp26A=((
struct Cyc_Absyn_Let_d_struct*)_tmp259)->f3;_LL1EC: Cyc_Absyndump_dump(({const char*
_tmp28D="let";_tag_dyneither(_tmp28D,sizeof(char),4);}));Cyc_Absyndump_dumppat(
_tmp269);Cyc_Absyndump_dump_char((int)'=');Cyc_Absyndump_dumpexp(_tmp26A);Cyc_Absyndump_dump_semi();
goto _LL1E0;_LL1ED: if(*((int*)_tmp259)!= 3)goto _LL1EF;_tmp26B=((struct Cyc_Absyn_Letv_d_struct*)
_tmp259)->f1;_LL1EE: Cyc_Absyndump_dump(({const char*_tmp28E="let ";_tag_dyneither(
_tmp28E,sizeof(char),5);}));Cyc_Absyndump_dumpids(_tmp26B);Cyc_Absyndump_dump_semi();
goto _LL1E0;_LL1EF: if(*((int*)_tmp259)!= 4)goto _LL1F1;_tmp26C=((struct Cyc_Absyn_Region_d_struct*)
_tmp259)->f1;_tmp26D=((struct Cyc_Absyn_Region_d_struct*)_tmp259)->f2;_tmp26E=((
struct Cyc_Absyn_Region_d_struct*)_tmp259)->f3;_tmp26F=((struct Cyc_Absyn_Region_d_struct*)
_tmp259)->f4;_LL1F0: Cyc_Absyndump_dump(({const char*_tmp28F="region";
_tag_dyneither(_tmp28F,sizeof(char),7);}));Cyc_Absyndump_dump(({const char*
_tmp290="<";_tag_dyneither(_tmp290,sizeof(char),2);}));Cyc_Absyndump_dumptvar(
_tmp26C);Cyc_Absyndump_dump(({const char*_tmp291="> ";_tag_dyneither(_tmp291,
sizeof(char),3);}));Cyc_Absyndump_dumpqvar(_tmp26D->name);if(_tmp26E)Cyc_Absyndump_dump(({
const char*_tmp292="@resetable";_tag_dyneither(_tmp292,sizeof(char),11);}));if(
_tmp26F != 0){Cyc_Absyndump_dump(({const char*_tmp293=" = open(";_tag_dyneither(
_tmp293,sizeof(char),9);}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp26F);
Cyc_Absyndump_dump(({const char*_tmp294=")";_tag_dyneither(_tmp294,sizeof(char),2);}));}
Cyc_Absyndump_dump_semi();goto _LL1E0;_LL1F1: if(*((int*)_tmp259)!= 5)goto _LL1F3;
_tmp270=((struct Cyc_Absyn_Alias_d_struct*)_tmp259)->f1;_tmp271=((struct Cyc_Absyn_Alias_d_struct*)
_tmp259)->f2;_tmp272=((struct Cyc_Absyn_Alias_d_struct*)_tmp259)->f3;_LL1F2: Cyc_Absyndump_dump(({
const char*_tmp295="alias ";_tag_dyneither(_tmp295,sizeof(char),7);}));Cyc_Absyndump_dump(({
const char*_tmp296="<";_tag_dyneither(_tmp296,sizeof(char),2);}));Cyc_Absyndump_dumptvar(
_tmp271);Cyc_Absyndump_dump(({const char*_tmp297=">";_tag_dyneither(_tmp297,
sizeof(char),2);}));Cyc_Absyndump_dumpqvar(_tmp272->name);Cyc_Absyndump_dump(({
const char*_tmp298=" = ";_tag_dyneither(_tmp298,sizeof(char),4);}));Cyc_Absyndump_dumpexp(
_tmp270);Cyc_Absyndump_dump_semi();goto _LL1E0;_LL1F3: if(*((int*)_tmp259)!= 9)
goto _LL1F5;_tmp273=((struct Cyc_Absyn_Typedef_d_struct*)_tmp259)->f1;_LL1F4: if(!
Cyc_Absyndump_expand_typedefs  || _tmp273->defn != 0  && Cyc_Absynpp_is_anon_aggrtype((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp273->defn))->v)){Cyc_Absyndump_dump(({
const char*_tmp299="typedef";_tag_dyneither(_tmp299,sizeof(char),8);}));{void*t;
if(_tmp273->defn == 0)t=Cyc_Absyn_new_evar(_tmp273->kind,0);else{t=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp273->defn))->v;}((void(*)(struct Cyc_Absyn_Tqual,
void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(
_tmp273->tq,t,Cyc_Absyndump_dumptypedefname,_tmp273);Cyc_Absyndump_dumpatts(
_tmp273->atts);Cyc_Absyndump_dump_semi();}}goto _LL1E0;_LL1F5: if(*((int*)_tmp259)
!= 10)goto _LL1F7;_tmp274=((struct Cyc_Absyn_Namespace_d_struct*)_tmp259)->f1;
_tmp275=((struct Cyc_Absyn_Namespace_d_struct*)_tmp259)->f2;_LL1F6: Cyc_Absyndump_dump(({
const char*_tmp29A="namespace";_tag_dyneither(_tmp29A,sizeof(char),10);}));Cyc_Absyndump_dump_str(
_tmp274);Cyc_Absyndump_dump_char((int)'{');for(0;_tmp275 != 0;_tmp275=_tmp275->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp275->hd);}Cyc_Absyndump_dump_char((
int)'}');goto _LL1E0;_LL1F7: if(*((int*)_tmp259)!= 11)goto _LL1F9;_tmp276=((struct
Cyc_Absyn_Using_d_struct*)_tmp259)->f1;_tmp277=((struct Cyc_Absyn_Using_d_struct*)
_tmp259)->f2;_LL1F8: Cyc_Absyndump_dump(({const char*_tmp29B="using";
_tag_dyneither(_tmp29B,sizeof(char),6);}));Cyc_Absyndump_dumpqvar(_tmp276);Cyc_Absyndump_dump_char((
int)'{');for(0;_tmp277 != 0;_tmp277=_tmp277->tl){Cyc_Absyndump_dumpdecl((struct
Cyc_Absyn_Decl*)_tmp277->hd);}Cyc_Absyndump_dump_char((int)'}');goto _LL1E0;
_LL1F9: if(*((int*)_tmp259)!= 12)goto _LL1FB;_tmp278=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp259)->f1;_LL1FA: Cyc_Absyndump_dump(({const char*_tmp29C="extern \"C\" {";
_tag_dyneither(_tmp29C,sizeof(char),13);}));for(0;_tmp278 != 0;_tmp278=_tmp278->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp278->hd);}Cyc_Absyndump_dump_char((
int)'}');goto _LL1E0;_LL1FB: if(*((int*)_tmp259)!= 13)goto _LL1FD;_tmp279=((struct
Cyc_Absyn_ExternCinclude_d_struct*)_tmp259)->f1;_tmp27A=((struct Cyc_Absyn_ExternCinclude_d_struct*)
_tmp259)->f2;_LL1FC: Cyc_Absyndump_dump(({const char*_tmp29D="extern \"C include\" {";
_tag_dyneither(_tmp29D,sizeof(char),21);}));for(0;_tmp279 != 0;_tmp279=_tmp279->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp279->hd);}Cyc_Absyndump_dump_char((
int)'}');if(_tmp27A != 0){Cyc_Absyndump_dump(({const char*_tmp29E=" export {";
_tag_dyneither(_tmp29E,sizeof(char),10);}));for(0;_tmp27A != 0;_tmp27A=_tmp27A->tl){
struct _tuple0*_tmp2A0;struct _tuple18 _tmp29F=*((struct _tuple18*)_tmp27A->hd);
_tmp2A0=_tmp29F.f2;Cyc_Absyndump_dumpqvar(_tmp2A0);if(_tmp27A->tl != 0)Cyc_Absyndump_dump_char((
int)',');}Cyc_Absyndump_dump(({const char*_tmp2A1="}";_tag_dyneither(_tmp2A1,
sizeof(char),2);}));}goto _LL1E0;_LL1FD: if((int)_tmp259 != 0)goto _LL1FF;_LL1FE: Cyc_Absyndump_dump(({
const char*_tmp2A2=" __cyclone_port_on__; ";_tag_dyneither(_tmp2A2,sizeof(char),
23);}));goto _LL1E0;_LL1FF: if((int)_tmp259 != 1)goto _LL1E0;_LL200: Cyc_Absyndump_dump(({
const char*_tmp2A3=" __cyclone_port_off__; ";_tag_dyneither(_tmp2A3,sizeof(char),
24);}));goto _LL1E0;_LL1E0:;}static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*
e){struct _tuple12 pr=Cyc_Evexp_eval_const_uint_exp(e);if(pr.f1 != 1  || !pr.f2){Cyc_Absyndump_dump_char((
int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)'}');}}void Cyc_Absyndump_dumptms(
struct Cyc_List_List*tms,void(*f)(void*),void*a){if(tms == 0){f(a);return;}{void*
_tmp2A4=(void*)tms->hd;struct Cyc_Absyn_PtrAtts _tmp2A5;void*_tmp2A6;union Cyc_Absyn_Constraint*
_tmp2A7;union Cyc_Absyn_Constraint*_tmp2A8;union Cyc_Absyn_Constraint*_tmp2A9;
struct Cyc_Absyn_Tqual _tmp2AA;_LL211: if(*((int*)_tmp2A4)!= 2)goto _LL213;_tmp2A5=((
struct Cyc_Absyn_Pointer_mod_struct*)_tmp2A4)->f1;_tmp2A6=_tmp2A5.rgn;_tmp2A7=
_tmp2A5.nullable;_tmp2A8=_tmp2A5.bounds;_tmp2A9=_tmp2A5.zero_term;_tmp2AA=((
struct Cyc_Absyn_Pointer_mod_struct*)_tmp2A4)->f2;_LL212:{void*_tmp2AB=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp2A8);struct Cyc_Absyn_Exp*_tmp2AC;_LL216: if((int)_tmp2AB
!= 0)goto _LL218;_LL217: Cyc_Absyndump_dump_char((int)'?');goto _LL215;_LL218: if(
_tmp2AB <= (void*)1)goto _LL215;if(*((int*)_tmp2AB)!= 0)goto _LL215;_tmp2AC=((
struct Cyc_Absyn_Upper_b_struct*)_tmp2AB)->f1;_LL219: Cyc_Absyndump_dump_char((int)(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp2A7)?'*':'@'));
Cyc_Absyndump_dump_upperbound(_tmp2AC);goto _LL215;_LL215:;}if(((int(*)(int y,
union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2A9))Cyc_Absyndump_dump(({
const char*_tmp2AD="@zeroterm";_tag_dyneither(_tmp2AD,sizeof(char),10);}));{void*
_tmp2AE=Cyc_Tcutil_compress(_tmp2A6);struct Cyc_Absyn_Tvar*_tmp2AF;struct Cyc_Core_Opt*
_tmp2B0;_LL21B: if((int)_tmp2AE != 2)goto _LL21D;_LL21C: goto _LL21A;_LL21D: if(
_tmp2AE <= (void*)4)goto _LL221;if(*((int*)_tmp2AE)!= 1)goto _LL21F;_tmp2AF=((
struct Cyc_Absyn_VarType_struct*)_tmp2AE)->f1;_LL21E: Cyc_Absyndump_dump_str(
_tmp2AF->name);goto _LL21A;_LL21F: if(*((int*)_tmp2AE)!= 0)goto _LL221;_tmp2B0=((
struct Cyc_Absyn_Evar_struct*)_tmp2AE)->f2;if(_tmp2B0 != 0)goto _LL221;_LL220: Cyc_Absyndump_dumpntyp(
Cyc_Tcutil_compress(_tmp2A6));goto _LL21A;_LL221:;_LL222:({void*_tmp2B1=0;Cyc_Tcutil_impos(({
const char*_tmp2B2="dumptms: bad rgn type in Pointer_mod";_tag_dyneither(_tmp2B2,
sizeof(char),37);}),_tag_dyneither(_tmp2B1,sizeof(void*),0));});_LL21A:;}Cyc_Absyndump_dumptq(
_tmp2AA);Cyc_Absyndump_dumptms(tms->tl,f,a);return;_LL213:;_LL214: {int
next_is_pointer=0;if(tms->tl != 0){void*_tmp2B3=(void*)((struct Cyc_List_List*)
_check_null(tms->tl))->hd;_LL224: if(*((int*)_tmp2B3)!= 2)goto _LL226;_LL225:
next_is_pointer=1;goto _LL223;_LL226:;_LL227: goto _LL223;_LL223:;}if(
next_is_pointer)Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptms(tms->tl,
f,a);if(next_is_pointer)Cyc_Absyndump_dump_char((int)')');{void*_tmp2B4=(void*)
tms->hd;union Cyc_Absyn_Constraint*_tmp2B5;struct Cyc_Absyn_Exp*_tmp2B6;union Cyc_Absyn_Constraint*
_tmp2B7;void*_tmp2B8;struct Cyc_List_List*_tmp2B9;int _tmp2BA;struct Cyc_Absyn_VarargInfo*
_tmp2BB;struct Cyc_Core_Opt*_tmp2BC;struct Cyc_List_List*_tmp2BD;void*_tmp2BE;
struct Cyc_List_List*_tmp2BF;struct Cyc_Position_Segment*_tmp2C0;struct Cyc_List_List*
_tmp2C1;struct Cyc_Position_Segment*_tmp2C2;int _tmp2C3;struct Cyc_List_List*
_tmp2C4;_LL229: if(*((int*)_tmp2B4)!= 0)goto _LL22B;_tmp2B5=((struct Cyc_Absyn_Carray_mod_struct*)
_tmp2B4)->f1;_LL22A: Cyc_Absyndump_dump(({const char*_tmp2C5="[]";_tag_dyneither(
_tmp2C5,sizeof(char),3);}));if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(
0,_tmp2B5))Cyc_Absyndump_dump(({const char*_tmp2C6="@zeroterm";_tag_dyneither(
_tmp2C6,sizeof(char),10);}));goto _LL228;_LL22B: if(*((int*)_tmp2B4)!= 1)goto
_LL22D;_tmp2B6=((struct Cyc_Absyn_ConstArray_mod_struct*)_tmp2B4)->f1;_tmp2B7=((
struct Cyc_Absyn_ConstArray_mod_struct*)_tmp2B4)->f2;_LL22C: Cyc_Absyndump_dump_char((
int)'[');Cyc_Absyndump_dumpexp(_tmp2B6);Cyc_Absyndump_dump_char((int)']');if(((
int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2B7))Cyc_Absyndump_dump(({
const char*_tmp2C7="@zeroterm";_tag_dyneither(_tmp2C7,sizeof(char),10);}));goto
_LL228;_LL22D: if(*((int*)_tmp2B4)!= 3)goto _LL22F;_tmp2B8=(void*)((struct Cyc_Absyn_Function_mod_struct*)
_tmp2B4)->f1;if(*((int*)_tmp2B8)!= 1)goto _LL22F;_tmp2B9=((struct Cyc_Absyn_WithTypes_struct*)
_tmp2B8)->f1;_tmp2BA=((struct Cyc_Absyn_WithTypes_struct*)_tmp2B8)->f2;_tmp2BB=((
struct Cyc_Absyn_WithTypes_struct*)_tmp2B8)->f3;_tmp2BC=((struct Cyc_Absyn_WithTypes_struct*)
_tmp2B8)->f4;_tmp2BD=((struct Cyc_Absyn_WithTypes_struct*)_tmp2B8)->f5;_LL22E: Cyc_Absyndump_dumpfunargs(
_tmp2B9,_tmp2BA,_tmp2BB,_tmp2BC,_tmp2BD);goto _LL228;_LL22F: if(*((int*)_tmp2B4)!= 
3)goto _LL231;_tmp2BE=(void*)((struct Cyc_Absyn_Function_mod_struct*)_tmp2B4)->f1;
if(*((int*)_tmp2BE)!= 0)goto _LL231;_tmp2BF=((struct Cyc_Absyn_NoTypes_struct*)
_tmp2BE)->f1;_tmp2C0=((struct Cyc_Absyn_NoTypes_struct*)_tmp2BE)->f2;_LL230:((
void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_List_List*l,struct
_dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(
Cyc_Absyndump_dump_str,_tmp2BF,({const char*_tmp2C8="(";_tag_dyneither(_tmp2C8,
sizeof(char),2);}),({const char*_tmp2C9=")";_tag_dyneither(_tmp2C9,sizeof(char),2);}),({
const char*_tmp2CA=",";_tag_dyneither(_tmp2CA,sizeof(char),2);}));goto _LL228;
_LL231: if(*((int*)_tmp2B4)!= 4)goto _LL233;_tmp2C1=((struct Cyc_Absyn_TypeParams_mod_struct*)
_tmp2B4)->f1;_tmp2C2=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp2B4)->f2;
_tmp2C3=((struct Cyc_Absyn_TypeParams_mod_struct*)_tmp2B4)->f3;_LL232: if(_tmp2C3)
Cyc_Absyndump_dumpkindedtvars(_tmp2C1);else{Cyc_Absyndump_dumptvars(_tmp2C1);}
goto _LL228;_LL233: if(*((int*)_tmp2B4)!= 5)goto _LL235;_tmp2C4=((struct Cyc_Absyn_Attributes_mod_struct*)
_tmp2B4)->f2;_LL234: Cyc_Absyndump_dumpatts(_tmp2C4);goto _LL228;_LL235: if(*((int*)
_tmp2B4)!= 2)goto _LL228;_LL236:({void*_tmp2CB=0;Cyc_Tcutil_impos(({const char*
_tmp2CC="dumptms";_tag_dyneither(_tmp2CC,sizeof(char),8);}),_tag_dyneither(
_tmp2CB,sizeof(void*),0));});_LL228:;}return;}_LL210:;}}void Cyc_Absyndump_dumptqtd(
struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){struct _RegionHandle
_tmp2CD=_new_region("temp");struct _RegionHandle*temp=& _tmp2CD;_push_region(temp);{
struct Cyc_Absyn_Tqual _tmp2CF;void*_tmp2D0;struct Cyc_List_List*_tmp2D1;struct
_tuple11 _tmp2CE=Cyc_Absynpp_to_tms(temp,tq,t);_tmp2CF=_tmp2CE.f1;_tmp2D0=_tmp2CE.f2;
_tmp2D1=_tmp2CE.f3;Cyc_Absyndump_dumptq(_tmp2CF);Cyc_Absyndump_dumpntyp(_tmp2D0);
Cyc_Absyndump_dumptms(Cyc_List_imp_rev(_tmp2D1),f,a);};_pop_region(temp);}void
Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
Cyc_Absyndump_pos=0;*Cyc_Absyndump_dump_file=f;for(0;tdl != 0;tdl=tdl->tl){Cyc_Absyndump_dumpdecl((
struct Cyc_Absyn_Decl*)tdl->hd);}({void*_tmp2D2=0;Cyc_fprintf(f,({const char*
_tmp2D3="\n";_tag_dyneither(_tmp2D3,sizeof(char),2);}),_tag_dyneither(_tmp2D2,
sizeof(void*),0));});}
