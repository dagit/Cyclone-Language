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
  if (!_cus_arr.base) _throw_null();
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
  if (!_cus_arr.base) _throw_null(); \
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
void*v;};struct _tuple0{void*f1;void*f2;};void*Cyc_Core_snd(struct _tuple0*);
extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];struct Cyc_List_List{
void*hd;struct Cyc_List_List*tl;};struct Cyc_List_List*Cyc_List_list(struct
_dyneither_ptr);int Cyc_List_length(struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_rmap(
struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);struct Cyc_List_List*
Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*
x);extern char Cyc_List_List_mismatch[18];struct Cyc_List_List*Cyc_List_rrev(struct
_RegionHandle*,struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_imp_rev(
struct Cyc_List_List*x);struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,
struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_imp_append(
struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[8];struct Cyc_List_List*
Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*
x,struct Cyc_List_List*y);struct Cyc_List_List*Cyc_List_rtabulate_c(struct
_RegionHandle*r,int n,void*(*f)(void*,int),void*env);struct Cyc_Iter_Iter{void*env;
int(*next)(void*env,void*dest);};int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);
typedef struct{int __count;union{unsigned int __wch;char __wchb[4];}__value;}Cyc___mbstate_t;
typedef struct{long __pos;Cyc___mbstate_t __state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t
Cyc_fpos_t;struct Cyc___cycFILE;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;}
;struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{
int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{
int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct
_dyneither_ptr f1;};struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(
void*,void*);struct _RegionHandle*r;struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[
12];extern char Cyc_Dict_Absent[11];struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*r,struct Cyc_Dict_Dict d);struct _tuple0*Cyc_Dict_rchoose(struct
_RegionHandle*,struct Cyc_Dict_Dict d);struct Cyc_Set_Set;struct Cyc_Set_Set*Cyc_Set_rempty(
struct _RegionHandle*r,int(*cmp)(void*,void*));struct Cyc_Set_Set*Cyc_Set_rinsert(
struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);int Cyc_Set_cardinality(
struct Cyc_Set_Set*s);int Cyc_Set_is_empty(struct Cyc_Set_Set*s);int Cyc_Set_member(
struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[11];void*Cyc_Set_choose(
struct Cyc_Set_Set*s);int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct
_dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_Lineno_Pos{struct
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct Cyc_Position_Error{
struct _dyneither_ptr source;struct Cyc_Position_Segment*seg;void*kind;struct
_dyneither_ptr desc;};extern char Cyc_Position_Nocontext[14];struct Cyc_Absyn_Loc_n_struct{
int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{
int tag;struct Cyc_List_List*f1;};union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct
Loc_n;struct Cyc_Absyn_Rel_n_struct Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};
struct _tuple1{union Cyc_Absyn_Nmspace_union f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Conref;
struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;
};struct Cyc_Absyn_Forward_constr_struct{int tag;struct Cyc_Absyn_Conref*f1;};
struct Cyc_Absyn_No_constr_struct{int tag;};union Cyc_Absyn_Constraint_union{struct
Cyc_Absyn_Eq_constr_struct Eq_constr;struct Cyc_Absyn_Forward_constr_struct
Forward_constr;struct Cyc_Absyn_No_constr_struct No_constr;};struct Cyc_Absyn_Conref{
union Cyc_Absyn_Constraint_union v;};struct Cyc_Absyn_Eq_kb_struct{int tag;void*f1;}
;struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_struct{
int tag;struct Cyc_Core_Opt*f1;void*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*
name;int identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{int tag;struct Cyc_Absyn_Exp*
f1;};struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*ptr_loc;struct Cyc_Position_Segment*
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
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_struct{int tag;void*
f1;};struct Cyc_Absyn_DynRgnType_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;
void**f4;};struct Cyc_Absyn_ValueofType_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_TagType_struct{int tag;void*f1;};struct Cyc_Absyn_AccessEff_struct{
int tag;void*f1;};struct Cyc_Absyn_JoinEff_struct{int tag;struct Cyc_List_List*f1;};
struct Cyc_Absyn_RgnsEff_struct{int tag;void*f1;};struct Cyc_Absyn_NoTypes_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_WithTypes_struct{
int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;struct Cyc_Core_Opt*
f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Regparm_att_struct{int tag;int f1;};
struct Cyc_Absyn_Aligned_att_struct{int tag;int f1;};struct Cyc_Absyn_Section_att_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Format_att_struct{int tag;void*f1;
int f2;int f3;};struct Cyc_Absyn_Initializes_att_struct{int tag;int f1;};struct Cyc_Absyn_Mode_att_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_struct{int tag;struct
Cyc_Absyn_Conref*f1;struct Cyc_Position_Segment*f2;};struct Cyc_Absyn_ConstArray_mod_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Conref*f2;struct Cyc_Position_Segment*
f3;};struct Cyc_Absyn_Pointer_mod_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct
Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_struct{int tag;void*f1;};struct
Cyc_Absyn_TypeParams_mod_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Position_Segment*
f2;int f3;};struct Cyc_Absyn_Attributes_mod_struct{int tag;struct Cyc_Position_Segment*
f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Char_c_struct{int tag;void*f1;char f2;
};struct Cyc_Absyn_Short_c_struct{int tag;void*f1;short f2;};struct Cyc_Absyn_Int_c_struct{
int tag;void*f1;int f2;};struct Cyc_Absyn_LongLong_c_struct{int tag;void*f1;
long long f2;};struct Cyc_Absyn_Float_c_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_Absyn_String_c_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Null_c_struct{
int tag;};union Cyc_Absyn_Cnst_union{struct Cyc_Absyn_Char_c_struct Char_c;struct Cyc_Absyn_Short_c_struct
Short_c;struct Cyc_Absyn_Int_c_struct Int_c;struct Cyc_Absyn_LongLong_c_struct
LongLong_c;struct Cyc_Absyn_Float_c_struct Float_c;struct Cyc_Absyn_String_c_struct
String_c;struct Cyc_Absyn_Null_c_struct Null_c;};struct Cyc_Absyn_VarargCallInfo{
int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
struct Cyc_Absyn_StructField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_struct{
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
_dyneither_ptr*f2;};struct Cyc_Absyn_AggrArrow_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Subscript_e_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_struct{int tag;struct Cyc_List_List*
f1;};struct _tuple2{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
struct Cyc_Absyn_CompoundLit_e_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*
f2;};struct Cyc_Absyn_Array_e_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;
int f4;};struct Cyc_Absyn_Struct_e_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*
f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_struct{
int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Tunion_e_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_Absyn_Tuniondecl*f2;struct Cyc_Absyn_Tunionfield*
f3;};struct Cyc_Absyn_Enum_e_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*
f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_struct{int tag;
struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_struct{
int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_struct{int tag;
struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_struct{
int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Valueof_e_struct{int tag;void*f1;
};struct Cyc_Absyn_Exp{struct Cyc_Core_Opt*topt;void*r;struct Cyc_Position_Segment*
loc;void*annot;};struct Cyc_Absyn_Exp_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};
struct Cyc_Absyn_Seq_s_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*
f2;};struct Cyc_Absyn_Return_s_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};
struct _tuple3{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_struct{
int tag;struct _tuple3 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_struct{int tag;struct
Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_struct{int tag;struct _dyneither_ptr*f1;
struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_struct{int tag;struct Cyc_Absyn_Exp*
f1;struct _tuple3 f2;struct _tuple3 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_struct{
int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_struct{
int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_struct{
int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_struct{
int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple3 f2;};struct Cyc_Absyn_TryCatch_s_struct{
int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Region_s_struct{
int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*
f4;struct Cyc_Absyn_Stmt*f5;};struct Cyc_Absyn_ResetRegion_s_struct{int tag;struct
Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Alias_s_struct{int tag;struct Cyc_Absyn_Exp*f1;
struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;struct Cyc_Absyn_Stmt*f4;};
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
int tag;char f1;};struct Cyc_Absyn_Float_p_struct{int tag;struct _dyneither_ptr f1;};
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
struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*
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
struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_struct{
int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_struct{
int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_struct{int tag;
struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;
struct Cyc_Position_Segment*loc;};struct Cyc_Absyn_ArrayElement_struct{int tag;
struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_struct{int tag;struct
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Conref*Cyc_Absyn_empty_conref();
void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*x);extern struct Cyc_Absyn_Conref*
Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*Cyc_Absyn_false_conref;void*
Cyc_Absyn_compress_kb(void*);void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct
Cyc_Core_Opt*tenv);void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);extern void*Cyc_Absyn_char_typ;
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_float_typ;
extern void*Cyc_Absyn_empty_effect;struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,
struct Cyc_Position_Segment*s);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;
struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;
int add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_kind2string(void*);struct _dyneither_ptr Cyc_Absynpp_kindbound2string(
void*);struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct
_dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_RgnOrder_RgnPO;
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
le;int allow_valueof;};void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct
Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,struct _tuple1*);struct Cyc_Absyn_Aggrdecl**
Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,
struct _tuple1*);struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*,struct Cyc_Position_Segment*,
struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);void Cyc_Tcenv_check_rgn_accessible(
struct Cyc_Tcenv_Tenv*,struct Cyc_Position_Segment*,void*rgn);void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);int Cyc_Tcutil_kind_leq(void*k1,
void*k2);void*Cyc_Tcutil_compress(void*t);int Cyc_Tcutil_coerceable(void*);extern
struct Cyc_Core_Opt Cyc_Tcutil_rk;extern struct Cyc_Core_Opt Cyc_Tcutil_trk;extern
struct Cyc_Core_Opt Cyc_Tcutil_ak;extern struct Cyc_Core_Opt Cyc_Tcutil_mk;int Cyc_Tcutil_unify(
void*,void*);void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,
void*);struct _tuple4{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct
_tuple5{struct Cyc_Absyn_Tvar*f1;void*f2;};struct _tuple5*Cyc_Tcutil_r_make_inst_var(
struct _tuple4*,struct Cyc_Absyn_Tvar*);void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*
vs,struct Cyc_Position_Segment*loc,struct _dyneither_ptr err_msg);struct Cyc_List_List*
Cyc_Tcutil_resolve_struct_designators(struct _RegionHandle*rgn,struct Cyc_Position_Segment*
loc,struct Cyc_List_List*des,struct Cyc_List_List*fields);int Cyc_Tcutil_is_noalias_pointer(
void*t);int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);struct Cyc_Absyn_Tvar*
Cyc_Tcutil_new_tvar(void*k);int Cyc_Tcutil_is_const_exp(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Exp*e);void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct
Cyc_Absyn_Exp*);struct _tuple6{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
struct Cyc_Tcpat_TcPatResult{struct _tuple6*tvars_and_bounds_opt;struct Cyc_List_List*
patvars;};struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,
struct Cyc_Absyn_Pat*p,void**topt);void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_switch_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_List_List*);int Cyc_Tcpat_check_let_pat_exhaustive(struct Cyc_Position_Segment*,
struct Cyc_Absyn_Pat*p);void Cyc_Tcpat_check_catch_overlap(struct Cyc_Position_Segment*,
struct Cyc_List_List*);struct _tuple7{unsigned int f1;int f2;};struct _tuple7 Cyc_Evexp_eval_const_uint_exp(
struct Cyc_Absyn_Exp*e);struct _tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*
f2;};static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*
p){void*_tmp0=(void*)p->r;struct _tuple1*_tmp1;struct _tuple1*_tmp2;struct Cyc_List_List*
_tmp3;int _tmp4;struct Cyc_Absyn_AggrInfo _tmp5;union Cyc_Absyn_AggrInfoU_union _tmp6;
void*_tmp7;struct _tuple1*_tmp8;struct Cyc_List_List*_tmp9;struct Cyc_List_List*
_tmpA;struct Cyc_List_List**_tmpB;struct Cyc_List_List*_tmpC;int _tmpD;struct Cyc_Absyn_Exp*
_tmpE;_LL1: if(_tmp0 <= (void*)2)goto _LL9;if(*((int*)_tmp0)!= 12)goto _LL3;_tmp1=((
struct Cyc_Absyn_UnknownId_p_struct*)_tmp0)->f1;_LL2:{struct _handler_cons _tmpF;
_push_handler(& _tmpF);{int _tmp11=0;if(setjmp(_tmpF.handler))_tmp11=1;if(!_tmp11){{
struct _RegionHandle _tmp12=_new_region("r");struct _RegionHandle*r=& _tmp12;
_push_region(r);{void*_tmp13=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp1);struct
Cyc_Absyn_Tuniondecl*_tmp14;struct Cyc_Absyn_Tunionfield*_tmp15;struct Cyc_Absyn_Enumdecl*
_tmp16;struct Cyc_Absyn_Enumfield*_tmp17;void*_tmp18;struct Cyc_Absyn_Enumfield*
_tmp19;_LLC: if(*((int*)_tmp13)!= 1)goto _LLE;_LLD:(void*)(p->r=(void*)((void*)0));({
void*_tmp1A=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp1B="struct tag used without arguments in pattern";
_tag_dyneither(_tmp1B,sizeof(char),45);}),_tag_dyneither(_tmp1A,sizeof(void*),0));});
_npop_handler(1);return;_LLE: if(*((int*)_tmp13)!= 2)goto _LL10;_tmp14=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp13)->f1;_tmp15=((struct Cyc_Tcenv_TunionRes_struct*)_tmp13)->f2;_LLF:(void*)(
p->r=(void*)((void*)({struct Cyc_Absyn_Tunion_p_struct*_tmp1C=_cycalloc(sizeof(*
_tmp1C));_tmp1C[0]=({struct Cyc_Absyn_Tunion_p_struct _tmp1D;_tmp1D.tag=6;_tmp1D.f1=
_tmp14;_tmp1D.f2=_tmp15;_tmp1D.f3=0;_tmp1D.f4=0;_tmp1D;});_tmp1C;})));
_npop_handler(1);return;_LL10: if(*((int*)_tmp13)!= 3)goto _LL12;_tmp16=((struct
Cyc_Tcenv_EnumRes_struct*)_tmp13)->f1;_tmp17=((struct Cyc_Tcenv_EnumRes_struct*)
_tmp13)->f2;_LL11:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Enum_p_struct*
_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E[0]=({struct Cyc_Absyn_Enum_p_struct
_tmp1F;_tmp1F.tag=10;_tmp1F.f1=_tmp16;_tmp1F.f2=_tmp17;_tmp1F;});_tmp1E;})));
_npop_handler(1);return;_LL12: if(*((int*)_tmp13)!= 4)goto _LL14;_tmp18=(void*)((
struct Cyc_Tcenv_AnonEnumRes_struct*)_tmp13)->f1;_tmp19=((struct Cyc_Tcenv_AnonEnumRes_struct*)
_tmp13)->f2;_LL13:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_AnonEnum_p_struct*
_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20[0]=({struct Cyc_Absyn_AnonEnum_p_struct
_tmp21;_tmp21.tag=11;_tmp21.f1=(void*)_tmp18;_tmp21.f2=_tmp19;_tmp21;});_tmp20;})));
_npop_handler(1);return;_LL14: if(*((int*)_tmp13)!= 0)goto _LLB;_LL15: goto _LLB;
_LLB:;};_pop_region(r);};_pop_handler();}else{void*_tmp10=(void*)_exn_thrown;
void*_tmp23=_tmp10;_LL17: if(_tmp23 != Cyc_Dict_Absent)goto _LL19;_LL18: goto _LL16;
_LL19:;_LL1A:(void)_throw(_tmp23);_LL16:;}}}{union Cyc_Absyn_Nmspace_union _tmp24=(*
_tmp1).f1;struct Cyc_List_List*_tmp25;_LL1C: if((_tmp24.Loc_n).tag != 0)goto _LL1E;
_LL1D: goto _LL1F;_LL1E: if((_tmp24.Rel_n).tag != 1)goto _LL20;_tmp25=(_tmp24.Rel_n).f1;
if(_tmp25 != 0)goto _LL20;_LL1F:(*_tmp1).f1=(union Cyc_Absyn_Nmspace_union)({union
Cyc_Absyn_Nmspace_union _tmp26;(_tmp26.Loc_n).tag=0;_tmp26;});(void*)(p->r=(void*)((
void*)({struct Cyc_Absyn_Var_p_struct*_tmp27=_cycalloc(sizeof(*_tmp27));_tmp27[0]=({
struct Cyc_Absyn_Var_p_struct _tmp28;_tmp28.tag=0;_tmp28.f1=Cyc_Absyn_new_vardecl(
_tmp1,(void*)0,0);_tmp28.f2=Cyc_Absyn_new_pat((void*)0,0);_tmp28;});_tmp27;})));
return;_LL20:;_LL21:({void*_tmp29=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp2A="qualified variable in pattern";
_tag_dyneither(_tmp2A,sizeof(char),30);}),_tag_dyneither(_tmp29,sizeof(void*),0));});
return;_LL1B:;}_LL3: if(*((int*)_tmp0)!= 13)goto _LL5;_tmp2=((struct Cyc_Absyn_UnknownCall_p_struct*)
_tmp0)->f1;_tmp3=((struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f2;_tmp4=((
struct Cyc_Absyn_UnknownCall_p_struct*)_tmp0)->f3;_LL4:{struct _handler_cons _tmp2B;
_push_handler(& _tmp2B);{int _tmp2D=0;if(setjmp(_tmp2B.handler))_tmp2D=1;if(!
_tmp2D){{struct _RegionHandle _tmp2E=_new_region("r");struct _RegionHandle*r=&
_tmp2E;_push_region(r);{void*_tmp2F=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp2);
struct Cyc_Absyn_Aggrdecl*_tmp30;struct Cyc_Absyn_Tuniondecl*_tmp31;struct Cyc_Absyn_Tunionfield*
_tmp32;_LL23: if(*((int*)_tmp2F)!= 1)goto _LL25;_tmp30=((struct Cyc_Tcenv_AggrRes_struct*)
_tmp2F)->f1;_LL24: if((void*)_tmp30->kind == (void*)1){(void*)(p->r=(void*)((void*)
0));({void*_tmp33=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp34="cannot pattern-match a union";
_tag_dyneither(_tmp34,sizeof(char),29);}),_tag_dyneither(_tmp33,sizeof(void*),0));});
_npop_handler(1);return;}{struct Cyc_List_List*_tmp35=0;for(0;_tmp3 != 0;_tmp3=
_tmp3->tl){_tmp35=({struct Cyc_List_List*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36->hd=({
struct _tuple8*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->f1=0;_tmp37->f2=(struct
Cyc_Absyn_Pat*)_tmp3->hd;_tmp37;});_tmp36->tl=_tmp35;_tmp36;});}(void*)(p->r=(
void*)((void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp38=_cycalloc(sizeof(*_tmp38));
_tmp38[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp39;_tmp39.tag=5;_tmp39.f1=({struct
Cyc_Absyn_AggrInfo _tmp3A;_tmp3A.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({
union Cyc_Absyn_AggrInfoU_union _tmp3B;(_tmp3B.KnownAggr).tag=1;(_tmp3B.KnownAggr).f1=({
struct Cyc_Absyn_Aggrdecl**_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C[0]=_tmp30;
_tmp3C;});_tmp3B;});_tmp3A.targs=0;_tmp3A;});_tmp39.f2=0;_tmp39.f3=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp35);_tmp39.f4=_tmp4;_tmp39;});
_tmp38;})));_npop_handler(1);return;}_LL25: if(*((int*)_tmp2F)!= 2)goto _LL27;
_tmp31=((struct Cyc_Tcenv_TunionRes_struct*)_tmp2F)->f1;_tmp32=((struct Cyc_Tcenv_TunionRes_struct*)
_tmp2F)->f2;_LL26:(void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Tunion_p_struct*
_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D[0]=({struct Cyc_Absyn_Tunion_p_struct
_tmp3E;_tmp3E.tag=6;_tmp3E.f1=_tmp31;_tmp3E.f2=_tmp32;_tmp3E.f3=_tmp3;_tmp3E.f4=
_tmp4;_tmp3E;});_tmp3D;})));_npop_handler(1);return;_LL27: if(*((int*)_tmp2F)!= 3)
goto _LL29;_LL28: goto _LL2A;_LL29: if(*((int*)_tmp2F)!= 4)goto _LL2B;_LL2A:({void*
_tmp3F=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp40="enum tag used with arguments in pattern";
_tag_dyneither(_tmp40,sizeof(char),40);}),_tag_dyneither(_tmp3F,sizeof(void*),0));});(
void*)(p->r=(void*)((void*)0));_npop_handler(1);return;_LL2B: if(*((int*)_tmp2F)
!= 0)goto _LL22;_LL2C: goto _LL22;_LL22:;};_pop_region(r);};_pop_handler();}else{
void*_tmp2C=(void*)_exn_thrown;void*_tmp42=_tmp2C;_LL2E: if(_tmp42 != Cyc_Dict_Absent)
goto _LL30;_LL2F: goto _LL2D;_LL30:;_LL31:(void)_throw(_tmp42);_LL2D:;}}}({struct
Cyc_String_pa_struct _tmp45;_tmp45.tag=0;_tmp45.f1=(struct _dyneither_ptr)((struct
_dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2));{void*_tmp43[1]={& _tmp45};Cyc_Tcutil_terr(
p->loc,({const char*_tmp44="%s is not a constructor in pattern";_tag_dyneither(
_tmp44,sizeof(char),35);}),_tag_dyneither(_tmp43,sizeof(void*),1));}});(void*)(p->r=(
void*)((void*)0));return;_LL5: if(*((int*)_tmp0)!= 5)goto _LL7;_tmp5=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp0)->f1;_tmp6=_tmp5.aggr_info;if((((((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL7;_tmp7=(_tmp6.UnknownAggr).f1;_tmp8=(_tmp6.UnknownAggr).f2;_tmp9=
_tmp5.targs;_tmpA=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f2;_tmpB=(struct Cyc_List_List**)&((
struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f2;_tmpC=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp0)->f3;_tmpD=((struct Cyc_Absyn_Aggr_p_struct*)_tmp0)->f4;_LL6:{struct
_handler_cons _tmp46;_push_handler(& _tmp46);{int _tmp48=0;if(setjmp(_tmp46.handler))
_tmp48=1;if(!_tmp48){{struct Cyc_Absyn_Aggrdecl**_tmp49=Cyc_Tcenv_lookup_aggrdecl(
te,p->loc,_tmp8);struct Cyc_Absyn_Aggrdecl*_tmp4A=*_tmp49;if((void*)_tmp4A->kind
== (void*)1){({void*_tmp4B=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp4C="cannot pattern-match a union";
_tag_dyneither(_tmp4C,sizeof(char),29);}),_tag_dyneither(_tmp4B,sizeof(void*),0));});(
void*)(p->r=(void*)((void*)0));_npop_handler(0);return;}if(_tmp4A->impl == 0){({
void*_tmp4D=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp4E="can't destructure an abstract struct";
_tag_dyneither(_tmp4E,sizeof(char),37);}),_tag_dyneither(_tmp4D,sizeof(void*),0));});(
void*)(p->r=(void*)((void*)0));_npop_handler(0);return;}{int more_exists=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4A->impl))->exist_vars)- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*
_tmpB);if(more_exists < 0){({void*_tmp4F=0;Cyc_Tcutil_terr(p->loc,({const char*
_tmp50="too many existentially bound type variables in pattern";_tag_dyneither(
_tmp50,sizeof(char),55);}),_tag_dyneither(_tmp4F,sizeof(void*),0));});{struct Cyc_List_List**
_tmp51=_tmpB;{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp4A->impl))->exist_vars);for(0;n != 0;-- n){_tmp51=&((struct Cyc_List_List*)
_check_null(*_tmp51))->tl;}}*_tmp51=0;}}else{if(more_exists > 0){struct Cyc_List_List*
_tmp52=0;for(0;more_exists != 0;-- more_exists){_tmp52=({struct Cyc_List_List*
_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->hd=Cyc_Tcutil_new_tvar((void*)({struct
Cyc_Absyn_Unknown_kb_struct*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54[0]=({struct
Cyc_Absyn_Unknown_kb_struct _tmp55;_tmp55.tag=1;_tmp55.f1=0;_tmp55;});_tmp54;}));
_tmp53->tl=_tmp52;_tmp53;});}*_tmpB=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpB,_tmp52);}}(void*)(p->r=(
void*)((void*)({struct Cyc_Absyn_Aggr_p_struct*_tmp56=_cycalloc(sizeof(*_tmp56));
_tmp56[0]=({struct Cyc_Absyn_Aggr_p_struct _tmp57;_tmp57.tag=5;_tmp57.f1=({struct
Cyc_Absyn_AggrInfo _tmp58;_tmp58.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({
union Cyc_Absyn_AggrInfoU_union _tmp59;(_tmp59.KnownAggr).tag=1;(_tmp59.KnownAggr).f1=
_tmp49;_tmp59;});_tmp58.targs=_tmp9;_tmp58;});_tmp57.f2=*_tmpB;_tmp57.f3=_tmpC;
_tmp57.f4=_tmpD;_tmp57;});_tmp56;})));}};_pop_handler();}else{void*_tmp47=(void*)
_exn_thrown;void*_tmp5B=_tmp47;_LL33: if(_tmp5B != Cyc_Dict_Absent)goto _LL35;_LL34:({
void*_tmp5C=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp5D="Non-struct name has designator patterns";
_tag_dyneither(_tmp5D,sizeof(char),40);}),_tag_dyneither(_tmp5C,sizeof(void*),0));});(
void*)(p->r=(void*)((void*)0));goto _LL32;_LL35:;_LL36:(void)_throw(_tmp5B);_LL32:;}}}
return;_LL7: if(*((int*)_tmp0)!= 14)goto _LL9;_tmpE=((struct Cyc_Absyn_Exp_p_struct*)
_tmp0)->f1;_LL8: Cyc_Tcexp_tcExp(te,0,_tmpE);if(!Cyc_Tcutil_is_const_exp(te,_tmpE)){({
void*_tmp5E=0;Cyc_Tcutil_terr(p->loc,({const char*_tmp5F="non-constant expression in case pattern";
_tag_dyneither(_tmp5F,sizeof(char),40);}),_tag_dyneither(_tmp5E,sizeof(void*),0));});(
void*)(p->r=(void*)((void*)0));}{unsigned int _tmp61;int _tmp62;struct _tuple7
_tmp60=Cyc_Evexp_eval_const_uint_exp(_tmpE);_tmp61=_tmp60.f1;_tmp62=_tmp60.f2;(
void*)(p->r=(void*)((void*)({struct Cyc_Absyn_Int_p_struct*_tmp63=_cycalloc(
sizeof(*_tmp63));_tmp63[0]=({struct Cyc_Absyn_Int_p_struct _tmp64;_tmp64.tag=7;
_tmp64.f1=(void*)((void*)2);_tmp64.f2=(int)_tmp61;_tmp64;});_tmp63;})));return;}
_LL9:;_LLA: return;_LL0:;}static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*
vd){return(*vd->name).f2;}static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,
void**topt){if(topt != 0)return*topt;return Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_mk,({struct Cyc_Core_Opt*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->v=s;
_tmp65;}));}static void*Cyc_Tcpat_num_type(void**topt,void*numt){if(topt != 0  && 
Cyc_Tcutil_coerceable(*topt))return*topt;{void*_tmp66=Cyc_Tcutil_compress(numt);
_LL38: if(_tmp66 <= (void*)4)goto _LL3A;if(*((int*)_tmp66)!= 12)goto _LL3A;_LL39: if(
topt != 0)return*topt;goto _LL37;_LL3A:;_LL3B: goto _LL37;_LL37:;}return numt;}static
void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl*vd,struct Cyc_List_List**
v_result_ptr,void*t){(void*)(vd->type=(void*)t);vd->tq=Cyc_Absyn_empty_tqual(0);*
v_result_ptr=(struct Cyc_List_List*)({struct Cyc_List_List*_tmp67=_cycalloc(
sizeof(*_tmp67));_tmp67->hd=vd;_tmp67->tl=*v_result_ptr;_tmp67;});}static struct
Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,
struct Cyc_Tcpat_TcPatResult res2){struct _tuple6*_tmp69;struct Cyc_List_List*_tmp6A;
struct Cyc_Tcpat_TcPatResult _tmp68=res1;_tmp69=_tmp68.tvars_and_bounds_opt;_tmp6A=
_tmp68.patvars;{struct _tuple6*_tmp6C;struct Cyc_List_List*_tmp6D;struct Cyc_Tcpat_TcPatResult
_tmp6B=res2;_tmp6C=_tmp6B.tvars_and_bounds_opt;_tmp6D=_tmp6B.patvars;if(_tmp69 != 
0  || _tmp6C != 0){if(_tmp69 == 0)_tmp69=({struct _tuple6*_tmp6E=_cycalloc(sizeof(*
_tmp6E));_tmp6E->f1=0;_tmp6E->f2=0;_tmp6E;});if(_tmp6C == 0)_tmp6C=({struct
_tuple6*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->f1=0;_tmp6F->f2=0;_tmp6F;});
return({struct Cyc_Tcpat_TcPatResult _tmp70;_tmp70.tvars_and_bounds_opt=({struct
_tuple6*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((struct _tuple6*)
_check_null(_tmp69))).f1,(*((struct _tuple6*)_check_null(_tmp6C))).f1);_tmp71->f2=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*((
struct _tuple6*)_check_null(_tmp69))).f2,(*((struct _tuple6*)_check_null(_tmp6C))).f2);
_tmp71;});_tmp70.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct
Cyc_List_List*y))Cyc_List_append)(_tmp6A,_tmp6D);_tmp70;});}return({struct Cyc_Tcpat_TcPatResult
_tmp72;_tmp72.tvars_and_bounds_opt=0;_tmp72.patvars=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp6A,_tmp6D);
_tmp72;});}}static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(struct Cyc_Position_Segment*
loc){return({struct Cyc_Absyn_Pat*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->loc=
loc;_tmp73->topt=0;_tmp73->r=(void*)((void*)0);_tmp73;});}struct _tuple9{struct
Cyc_Absyn_Tqual f1;void*f2;};struct _tuple10{struct Cyc_Absyn_Aggrfield*f1;struct
Cyc_Absyn_Pat*f2;};static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*
te,struct Cyc_Absyn_Pat*p,void**topt,void**rgn_opt){Cyc_Tcpat_resolve_pat(te,p);{
void*t;struct Cyc_Tcpat_TcPatResult res=({struct Cyc_Tcpat_TcPatResult _tmp142;
_tmp142.tvars_and_bounds_opt=0;_tmp142.patvars=0;_tmp142;});{void*_tmp74=(void*)
p->r;struct Cyc_Absyn_Vardecl*_tmp75;struct Cyc_Absyn_Pat*_tmp76;struct Cyc_Absyn_Vardecl*
_tmp77;struct Cyc_Absyn_Pat*_tmp78;struct Cyc_Absyn_Tvar*_tmp79;struct Cyc_Absyn_Vardecl*
_tmp7A;void*_tmp7B;void*_tmp7C;void*_tmp7D;struct Cyc_Absyn_Enumdecl*_tmp7E;void*
_tmp7F;struct Cyc_Absyn_Pat*_tmp80;struct Cyc_List_List*_tmp81;struct Cyc_List_List**
_tmp82;int _tmp83;struct Cyc_Absyn_AggrInfo _tmp84;union Cyc_Absyn_AggrInfoU_union
_tmp85;struct Cyc_Absyn_Aggrdecl**_tmp86;struct Cyc_Absyn_Aggrdecl*_tmp87;struct
Cyc_List_List*_tmp88;struct Cyc_List_List**_tmp89;struct Cyc_List_List*_tmp8A;
struct Cyc_List_List*_tmp8B;struct Cyc_List_List**_tmp8C;int _tmp8D;struct Cyc_Absyn_Tuniondecl*
_tmp8E;struct Cyc_Absyn_Tunionfield*_tmp8F;struct Cyc_List_List*_tmp90;struct Cyc_List_List**
_tmp91;int _tmp92;struct Cyc_Absyn_AggrInfo _tmp93;union Cyc_Absyn_AggrInfoU_union
_tmp94;_LL3D: if((int)_tmp74 != 0)goto _LL3F;_LL3E: t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(
te),topt);goto _LL3C;_LL3F: if(_tmp74 <= (void*)2)goto _LL53;if(*((int*)_tmp74)!= 0)
goto _LL41;_tmp75=((struct Cyc_Absyn_Var_p_struct*)_tmp74)->f1;_tmp76=((struct Cyc_Absyn_Var_p_struct*)
_tmp74)->f2;_LL40: res=Cyc_Tcpat_tcPatRec(te,_tmp76,topt,rgn_opt);t=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp76->topt))->v;Cyc_Tcpat_set_vd(_tmp75,& res.patvars,
t);goto _LL3C;_LL41: if(*((int*)_tmp74)!= 1)goto _LL43;_tmp77=((struct Cyc_Absyn_Reference_p_struct*)
_tmp74)->f1;_tmp78=((struct Cyc_Absyn_Reference_p_struct*)_tmp74)->f2;_LL42: res=
Cyc_Tcpat_tcPatRec(te,_tmp78,topt,rgn_opt);t=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp78->topt))->v;if(rgn_opt == 0){({void*_tmp95=0;Cyc_Tcutil_terr(p->loc,({
const char*_tmp96="* pattern would point into an unknown/unallowed region";
_tag_dyneither(_tmp96,sizeof(char),55);}),_tag_dyneither(_tmp95,sizeof(void*),0));});
goto _LL3C;}else{if(Cyc_Tcutil_is_noalias_pointer(t))({void*_tmp97=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp98="* pattern cannot take the address of an alias-free path";_tag_dyneither(
_tmp98,sizeof(char),56);}),_tag_dyneither(_tmp97,sizeof(void*),0));});}Cyc_Tcpat_set_vd(
_tmp77,& res.patvars,(void*)({struct Cyc_Absyn_PointerType_struct*_tmp99=_cycalloc(
sizeof(*_tmp99));_tmp99[0]=({struct Cyc_Absyn_PointerType_struct _tmp9A;_tmp9A.tag=
4;_tmp9A.f1=({struct Cyc_Absyn_PtrInfo _tmp9B;_tmp9B.elt_typ=(void*)t;_tmp9B.elt_tq=
Cyc_Absyn_empty_tqual(0);_tmp9B.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmp9C;_tmp9C.rgn=(
void*)*rgn_opt;_tmp9C.nullable=Cyc_Absyn_false_conref;_tmp9C.bounds=Cyc_Absyn_empty_conref();
_tmp9C.zero_term=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmp9C.ptrloc=
0;_tmp9C;});_tmp9B;});_tmp9A;});_tmp99;}));goto _LL3C;_LL43: if(*((int*)_tmp74)!= 
2)goto _LL45;_tmp79=((struct Cyc_Absyn_TagInt_p_struct*)_tmp74)->f1;_tmp7A=((
struct Cyc_Absyn_TagInt_p_struct*)_tmp74)->f2;_LL44: Cyc_Tcpat_set_vd(_tmp7A,& res.patvars,(
void*)_tmp7A->type);{struct _RegionHandle _tmp9D=_new_region("r2");struct
_RegionHandle*r2=& _tmp9D;_push_region(r2);Cyc_Tcenv_add_type_vars(r2,p->loc,te,({
struct Cyc_Absyn_Tvar*_tmp9E[1];_tmp9E[0]=_tmp79;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp9E,sizeof(struct Cyc_Absyn_Tvar*),
1));}));;_pop_region(r2);}if(res.tvars_and_bounds_opt == 0)res.tvars_and_bounds_opt=({
struct _tuple6*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->f1=0;_tmp9F->f2=0;_tmp9F;});(*
res.tvars_and_bounds_opt).f1=({struct Cyc_List_List*_tmpA0=_cycalloc(sizeof(*
_tmpA0));_tmpA0->hd=_tmp79;_tmpA0->tl=(*res.tvars_and_bounds_opt).f1;_tmpA0;});t=
Cyc_Absyn_uint_typ;goto _LL3C;_LL45: if(*((int*)_tmp74)!= 7)goto _LL47;_tmp7B=(void*)((
struct Cyc_Absyn_Int_p_struct*)_tmp74)->f1;if((int)_tmp7B != 1)goto _LL47;_LL46: t=
Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);goto _LL3C;_LL47: if(*((int*)_tmp74)!= 
7)goto _LL49;_tmp7C=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp74)->f1;if((int)
_tmp7C != 2)goto _LL49;_LL48: goto _LL4A;_LL49: if(*((int*)_tmp74)!= 7)goto _LL4B;
_tmp7D=(void*)((struct Cyc_Absyn_Int_p_struct*)_tmp74)->f1;if((int)_tmp7D != 0)
goto _LL4B;_LL4A: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto _LL3C;_LL4B:
if(*((int*)_tmp74)!= 8)goto _LL4D;_LL4C: t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);
goto _LL3C;_LL4D: if(*((int*)_tmp74)!= 9)goto _LL4F;_LL4E: t=Cyc_Tcpat_num_type(topt,
Cyc_Absyn_float_typ);goto _LL3C;_LL4F: if(*((int*)_tmp74)!= 10)goto _LL51;_tmp7E=((
struct Cyc_Absyn_Enum_p_struct*)_tmp74)->f1;_LL50: t=Cyc_Tcpat_num_type(topt,(void*)({
struct Cyc_Absyn_EnumType_struct*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1[0]=({
struct Cyc_Absyn_EnumType_struct _tmpA2;_tmpA2.tag=12;_tmpA2.f1=_tmp7E->name;
_tmpA2.f2=(struct Cyc_Absyn_Enumdecl*)_tmp7E;_tmpA2;});_tmpA1;}));goto _LL3C;_LL51:
if(*((int*)_tmp74)!= 11)goto _LL53;_tmp7F=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp74)->f1;_LL52: t=Cyc_Tcpat_num_type(topt,_tmp7F);goto _LL3C;_LL53: if((int)
_tmp74 != 1)goto _LL55;_LL54: if(topt != 0){void*_tmpA3=Cyc_Tcutil_compress(*topt);
_LL66: if(_tmpA3 <= (void*)4)goto _LL68;if(*((int*)_tmpA3)!= 4)goto _LL68;_LL67: t=*
topt;goto tcpat_end;_LL68:;_LL69: goto _LL65;_LL65:;}{struct Cyc_Core_Opt*_tmpA4=Cyc_Tcenv_lookup_opt_type_vars(
te);t=(void*)({struct Cyc_Absyn_PointerType_struct*_tmpA5=_cycalloc(sizeof(*
_tmpA5));_tmpA5[0]=({struct Cyc_Absyn_PointerType_struct _tmpA6;_tmpA6.tag=4;
_tmpA6.f1=({struct Cyc_Absyn_PtrInfo _tmpA7;_tmpA7.elt_typ=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_ak,_tmpA4);_tmpA7.elt_tq=Cyc_Absyn_empty_tqual(0);
_tmpA7.ptr_atts=({struct Cyc_Absyn_PtrAtts _tmpA8;_tmpA8.rgn=(void*)Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_rk,_tmpA4);_tmpA8.nullable=Cyc_Absyn_true_conref;
_tmpA8.bounds=Cyc_Absyn_empty_conref();_tmpA8.zero_term=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmpA8.ptrloc=0;_tmpA8;});_tmpA7;});_tmpA6;});_tmpA5;});
goto _LL3C;}_LL55: if(_tmp74 <= (void*)2)goto _LL57;if(*((int*)_tmp74)!= 4)goto _LL57;
_tmp80=((struct Cyc_Absyn_Pointer_p_struct*)_tmp74)->f1;_LL56: {void*inner_typ=(
void*)0;void**_tmpA9=0;if(topt != 0){void*_tmpAA=Cyc_Tcutil_compress(*topt);
struct Cyc_Absyn_PtrInfo _tmpAB;void*_tmpAC;_LL6B: if(_tmpAA <= (void*)4)goto _LL6D;
if(*((int*)_tmpAA)!= 4)goto _LL6D;_tmpAB=((struct Cyc_Absyn_PointerType_struct*)
_tmpAA)->f1;_tmpAC=(void*)_tmpAB.elt_typ;_LL6C: inner_typ=_tmpAC;_tmpA9=(void**)&
inner_typ;goto _LL6A;_LL6D:;_LL6E: goto _LL6A;_LL6A:;}{void*ptr_rgn=Cyc_Absyn_new_evar((
struct Cyc_Core_Opt*)& Cyc_Tcutil_trk,Cyc_Tcenv_lookup_opt_type_vars(te));res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,_tmp80,_tmpA9,(void**)& ptr_rgn));if(_tmpA9 == 0){void*
_tmpAD=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp80->topt))->v);
struct Cyc_Absyn_TunionFieldInfo _tmpAE;union Cyc_Absyn_TunionFieldInfoU_union
_tmpAF;struct Cyc_Absyn_Tuniondecl*_tmpB0;struct Cyc_Absyn_Tunionfield*_tmpB1;
struct Cyc_List_List*_tmpB2;_LL70: if(_tmpAD <= (void*)4)goto _LL72;if(*((int*)
_tmpAD)!= 3)goto _LL72;_tmpAE=((struct Cyc_Absyn_TunionFieldType_struct*)_tmpAD)->f1;
_tmpAF=_tmpAE.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmpAD)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LL72;_tmpB0=(_tmpAF.KnownTunionfield).f1;_tmpB1=(_tmpAF.KnownTunionfield).f2;
_tmpB2=_tmpAE.targs;if(!(!_tmpB0->is_flat))goto _LL72;_LL71: t=(void*)({struct Cyc_Absyn_TunionType_struct*
_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3[0]=({struct Cyc_Absyn_TunionType_struct
_tmpB4;_tmpB4.tag=2;_tmpB4.f1=({struct Cyc_Absyn_TunionInfo _tmpB5;_tmpB5.tunion_info=(
union Cyc_Absyn_TunionInfoU_union)({union Cyc_Absyn_TunionInfoU_union _tmpB7;(
_tmpB7.KnownTunion).tag=1;(_tmpB7.KnownTunion).f1=({struct Cyc_Absyn_Tuniondecl**
_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8[0]=_tmpB0;_tmpB8;});_tmpB7;});_tmpB5.targs=
_tmpB2;_tmpB5.rgn=({struct Cyc_Core_Opt*_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6->v=(
void*)ptr_rgn;_tmpB6;});_tmpB5;});_tmpB4;});_tmpB3;});goto _LL6F;_LL72:;_LL73: t=(
void*)({struct Cyc_Absyn_PointerType_struct*_tmpB9=_cycalloc(sizeof(*_tmpB9));
_tmpB9[0]=({struct Cyc_Absyn_PointerType_struct _tmpBA;_tmpBA.tag=4;_tmpBA.f1=({
struct Cyc_Absyn_PtrInfo _tmpBB;_tmpBB.elt_typ=(void*)((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp80->topt))->v);_tmpBB.elt_tq=Cyc_Absyn_empty_tqual(0);_tmpBB.ptr_atts=({
struct Cyc_Absyn_PtrAtts _tmpBC;_tmpBC.rgn=(void*)ptr_rgn;_tmpBC.nullable=((struct
Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmpBC.bounds=Cyc_Absyn_empty_conref();
_tmpBC.zero_term=((struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmpBC.ptrloc=
0;_tmpBC;});_tmpBB;});_tmpBA;});_tmpB9;});goto _LL6F;_LL6F:;}else{t=(void*)({
struct Cyc_Absyn_PointerType_struct*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD[0]=({
struct Cyc_Absyn_PointerType_struct _tmpBE;_tmpBE.tag=4;_tmpBE.f1=({struct Cyc_Absyn_PtrInfo
_tmpBF;_tmpBF.elt_typ=(void*)((void*)((struct Cyc_Core_Opt*)_check_null(_tmp80->topt))->v);
_tmpBF.elt_tq=Cyc_Absyn_empty_tqual(0);_tmpBF.ptr_atts=({struct Cyc_Absyn_PtrAtts
_tmpC0;_tmpC0.rgn=(void*)ptr_rgn;_tmpC0.nullable=((struct Cyc_Absyn_Conref*(*)())
Cyc_Absyn_empty_conref)();_tmpC0.bounds=Cyc_Absyn_empty_conref();_tmpC0.zero_term=((
struct Cyc_Absyn_Conref*(*)())Cyc_Absyn_empty_conref)();_tmpC0.ptrloc=0;_tmpC0;});
_tmpBF;});_tmpBE;});_tmpBD;});}goto _LL3C;}}_LL57: if(_tmp74 <= (void*)2)goto _LL59;
if(*((int*)_tmp74)!= 3)goto _LL59;_tmp81=((struct Cyc_Absyn_Tuple_p_struct*)_tmp74)->f1;
_tmp82=(struct Cyc_List_List**)&((struct Cyc_Absyn_Tuple_p_struct*)_tmp74)->f1;
_tmp83=((struct Cyc_Absyn_Tuple_p_struct*)_tmp74)->f2;_LL58: {struct Cyc_List_List*
_tmpC1=*_tmp82;struct Cyc_List_List*pat_ts=0;struct Cyc_List_List*topt_ts=0;if(
topt != 0){void*_tmpC2=Cyc_Tcutil_compress(*topt);struct Cyc_List_List*_tmpC3;
_LL75: if(_tmpC2 <= (void*)4)goto _LL77;if(*((int*)_tmpC2)!= 9)goto _LL77;_tmpC3=((
struct Cyc_Absyn_TupleType_struct*)_tmpC2)->f1;_LL76: topt_ts=_tmpC3;if(_tmp83){
int _tmpC4=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC1);int _tmpC5=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC3);if(_tmpC4 < _tmpC5){struct
Cyc_List_List*wild_ps=0;{int i=0;for(0;i < _tmpC5 - _tmpC4;++ i){wild_ps=({struct Cyc_List_List*
_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6->hd=Cyc_Tcpat_wild_pat(p->loc);_tmpC6->tl=
wild_ps;_tmpC6;});}}*_tmp82=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,
struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpC1,wild_ps);_tmpC1=*_tmp82;}
else{if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpC1)== ((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(_tmpC3))({void*_tmpC7=0;Cyc_Tcutil_warn(
p->loc,({const char*_tmpC8="unnecessary ... in tuple pattern";_tag_dyneither(
_tmpC8,sizeof(char),33);}),_tag_dyneither(_tmpC7,sizeof(void*),0));});}}goto
_LL74;_LL77:;_LL78: goto _LL74;_LL74:;}else{if(_tmp83)({void*_tmpC9=0;Cyc_Tcutil_terr(
p->loc,({const char*_tmpCA="cannot determine missing fields for ... in tuple pattern";
_tag_dyneither(_tmpCA,sizeof(char),57);}),_tag_dyneither(_tmpC9,sizeof(void*),0));});}
for(0;_tmpC1 != 0;_tmpC1=_tmpC1->tl){void**_tmpCB=0;if(topt_ts != 0){_tmpCB=(void**)&(*((
struct _tuple9*)topt_ts->hd)).f2;topt_ts=topt_ts->tl;}res=Cyc_Tcpat_combine_results(
res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmpC1->hd,_tmpCB,rgn_opt));
pat_ts=({struct Cyc_List_List*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->hd=({
struct _tuple9*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD->f1=Cyc_Absyn_empty_tqual(
0);_tmpCD->f2=(void*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Pat*)
_tmpC1->hd)->topt))->v;_tmpCD;});_tmpCC->tl=pat_ts;_tmpCC;});}t=(void*)({struct
Cyc_Absyn_TupleType_struct*_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE[0]=({struct
Cyc_Absyn_TupleType_struct _tmpCF;_tmpCF.tag=9;_tmpCF.f1=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts);_tmpCF;});_tmpCE;});goto _LL3C;}
_LL59: if(_tmp74 <= (void*)2)goto _LL5B;if(*((int*)_tmp74)!= 5)goto _LL5B;_tmp84=((
struct Cyc_Absyn_Aggr_p_struct*)_tmp74)->f1;_tmp85=_tmp84.aggr_info;if((((((
struct Cyc_Absyn_Aggr_p_struct*)_tmp74)->f1).aggr_info).KnownAggr).tag != 1)goto
_LL5B;_tmp86=(_tmp85.KnownAggr).f1;_tmp87=*_tmp86;_tmp88=_tmp84.targs;_tmp89=(
struct Cyc_List_List**)&(((struct Cyc_Absyn_Aggr_p_struct*)_tmp74)->f1).targs;
_tmp8A=((struct Cyc_Absyn_Aggr_p_struct*)_tmp74)->f2;_tmp8B=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp74)->f3;_tmp8C=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_struct*)
_tmp74)->f3;_tmp8D=((struct Cyc_Absyn_Aggr_p_struct*)_tmp74)->f4;_LL5A: {struct
Cyc_List_List*_tmpD0=*_tmp8C;if(_tmp87->impl == 0){({void*_tmpD1=0;Cyc_Tcutil_terr(
p->loc,({const char*_tmpD2="can't destructure an abstract struct";_tag_dyneither(
_tmpD2,sizeof(char),37);}),_tag_dyneither(_tmpD1,sizeof(void*),0));});t=Cyc_Absyn_wildtyp(
Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL3C;}if(_tmp8A != 0)rgn_opt=0;{struct
_RegionHandle _tmpD3=_new_region("rgn");struct _RegionHandle*rgn=& _tmpD3;
_push_region(rgn);{struct Cyc_List_List*_tmpD4=0;struct Cyc_List_List*
outlives_constraints=0;struct Cyc_List_List*_tmpD5=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp87->impl))->exist_vars;{struct Cyc_List_List*t=_tmp8A;for(0;t != 0;
t=t->tl){struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;struct Cyc_Absyn_Tvar*
uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmpD5))->hd;_tmpD5=
_tmpD5->tl;{void*_tmpD6=Cyc_Absyn_compress_kb((void*)tv->kind);void*_tmpD7=Cyc_Absyn_compress_kb((
void*)uv->kind);int error=0;void*k2;{void*_tmpD8=_tmpD7;void*_tmpD9;void*_tmpDA;
_LL7A: if(*((int*)_tmpD8)!= 2)goto _LL7C;_tmpD9=(void*)((struct Cyc_Absyn_Less_kb_struct*)
_tmpD8)->f2;_LL7B: _tmpDA=_tmpD9;goto _LL7D;_LL7C: if(*((int*)_tmpD8)!= 0)goto _LL7E;
_tmpDA=(void*)((struct Cyc_Absyn_Eq_kb_struct*)_tmpD8)->f1;_LL7D: k2=_tmpDA;goto
_LL79;_LL7E:;_LL7F:({void*_tmpDB=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpDC="unconstrained existential type variable in struct";
_tag_dyneither(_tmpDC,sizeof(char),50);}),_tag_dyneither(_tmpDB,sizeof(void*),0));});
_LL79:;}{void*_tmpDD=_tmpD6;void*_tmpDE;struct Cyc_Core_Opt*_tmpDF;struct Cyc_Core_Opt**
_tmpE0;void*_tmpE1;struct Cyc_Core_Opt*_tmpE2;struct Cyc_Core_Opt**_tmpE3;_LL81:
if(*((int*)_tmpDD)!= 0)goto _LL83;_tmpDE=(void*)((struct Cyc_Absyn_Eq_kb_struct*)
_tmpDD)->f1;_LL82: if(!Cyc_Tcutil_kind_leq(k2,_tmpDE))error=1;goto _LL80;_LL83: if(*((
int*)_tmpDD)!= 2)goto _LL85;_tmpDF=((struct Cyc_Absyn_Less_kb_struct*)_tmpDD)->f1;
_tmpE0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_struct*)_tmpDD)->f1;
_tmpE1=(void*)((struct Cyc_Absyn_Less_kb_struct*)_tmpDD)->f2;_LL84: _tmpE3=_tmpE0;
goto _LL86;_LL85: if(*((int*)_tmpDD)!= 1)goto _LL80;_tmpE2=((struct Cyc_Absyn_Unknown_kb_struct*)
_tmpDD)->f1;_tmpE3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_struct*)
_tmpDD)->f1;_LL86:*_tmpE3=({struct Cyc_Core_Opt*_tmpE4=_cycalloc(sizeof(*_tmpE4));
_tmpE4->v=(void*)_tmpD7;_tmpE4;});goto _LL80;_LL80:;}if(error)({struct Cyc_String_pa_struct
_tmpE9;_tmpE9.tag=0;_tmpE9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(
k2));{struct Cyc_String_pa_struct _tmpE8;_tmpE8.tag=0;_tmpE8.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(_tmpD6));{
struct Cyc_String_pa_struct _tmpE7;_tmpE7.tag=0;_tmpE7.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)*tv->name);{void*_tmpE5[3]={& _tmpE7,& _tmpE8,& _tmpE9};Cyc_Tcutil_terr(
p->loc,({const char*_tmpE6="type variable %s has kind %s but must have at least kind %s";
_tag_dyneither(_tmpE6,sizeof(char),60);}),_tag_dyneither(_tmpE5,sizeof(void*),3));}}}});{
void*vartype=(void*)({struct Cyc_Absyn_VarType_struct*_tmpEF=_cycalloc(sizeof(*
_tmpEF));_tmpEF[0]=({struct Cyc_Absyn_VarType_struct _tmpF0;_tmpF0.tag=1;_tmpF0.f1=
tv;_tmpF0;});_tmpEF;});_tmpD4=({struct Cyc_List_List*_tmpEA=_region_malloc(rgn,
sizeof(*_tmpEA));_tmpEA->hd=(void*)((void*)vartype);_tmpEA->tl=_tmpD4;_tmpEA;});
if(k2 == (void*)3)outlives_constraints=({struct Cyc_List_List*_tmpEB=_cycalloc(
sizeof(*_tmpEB));_tmpEB->hd=({struct _tuple0*_tmpEC=_cycalloc(sizeof(*_tmpEC));
_tmpEC->f1=Cyc_Absyn_empty_effect;_tmpEC->f2=vartype;_tmpEC;});_tmpEB->tl=
outlives_constraints;_tmpEB;});else{if(k2 == (void*)4  || k2 == (void*)5)({void*
_tmpED=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmpEE="opened existential had unique or top region kind";
_tag_dyneither(_tmpEE,sizeof(char),49);}),_tag_dyneither(_tmpED,sizeof(void*),0));});}}}}}
_tmpD4=Cyc_List_imp_rev(_tmpD4);{struct _RegionHandle _tmpF1=_new_region("r2");
struct _RegionHandle*r2=& _tmpF1;_push_region(r2);{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(
r2,p->loc,te,_tmp8A);struct Cyc_List_List*_tmpF2=Cyc_Tcenv_lookup_type_vars(te2);
struct _tuple4 _tmpF3=({struct _tuple4 _tmp113;_tmp113.f1=_tmpF2;_tmp113.f2=rgn;
_tmp113;});struct Cyc_List_List*_tmpF4=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,struct Cyc_Absyn_Tvar*),struct
_tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,&
_tmpF3,_tmp87->tvs);struct Cyc_List_List*_tmpF5=((struct Cyc_List_List*(*)(struct
_RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*
y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp87->impl))->exist_vars,
_tmpD4);struct Cyc_List_List*_tmpF6=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmpF4);struct Cyc_List_List*_tmpF7=((struct Cyc_List_List*(*)(void*(*f)(struct
_tuple5*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple5*))Cyc_Core_snd,
_tmpF5);struct Cyc_List_List*_tmpF8=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmpF4,_tmpF5);
if(_tmp8A != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp87->impl))->rgn_po
!= 0){if(res.tvars_and_bounds_opt == 0)res.tvars_and_bounds_opt=({struct _tuple6*
_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9->f1=0;_tmpF9->f2=0;_tmpF9;});(*res.tvars_and_bounds_opt).f1=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*
res.tvars_and_bounds_opt).f1,_tmp8A);(*res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,
outlives_constraints);{struct Cyc_List_List*_tmpFA=0;{struct Cyc_List_List*_tmpFB=((
struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp87->impl))->rgn_po;for(0;_tmpFB != 
0;_tmpFB=_tmpFB->tl){_tmpFA=({struct Cyc_List_List*_tmpFC=_cycalloc(sizeof(*
_tmpFC));_tmpFC->hd=({struct _tuple0*_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD->f1=
Cyc_Tcutil_rsubstitute(rgn,_tmpF8,(*((struct _tuple0*)_tmpFB->hd)).f1);_tmpFD->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmpF8,(*((struct _tuple0*)_tmpFB->hd)).f2);_tmpFD;});
_tmpFC->tl=_tmpFA;_tmpFC;});}}_tmpFA=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(_tmpFA);(*res.tvars_and_bounds_opt).f2=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,
_tmpFA);}}*_tmp89=_tmpF6;t=(void*)({struct Cyc_Absyn_AggrType_struct*_tmpFE=
_cycalloc(sizeof(*_tmpFE));_tmpFE[0]=({struct Cyc_Absyn_AggrType_struct _tmpFF;
_tmpFF.tag=10;_tmpFF.f1=({struct Cyc_Absyn_AggrInfo _tmp100;_tmp100.aggr_info=(
union Cyc_Absyn_AggrInfoU_union)({union Cyc_Absyn_AggrInfoU_union _tmp101;(_tmp101.KnownAggr).tag=
1;(_tmp101.KnownAggr).f1=({struct Cyc_Absyn_Aggrdecl**_tmp102=_cycalloc(sizeof(*
_tmp102));_tmp102[0]=_tmp87;_tmp102;});_tmp101;});_tmp100.targs=*_tmp89;_tmp100;});
_tmpFF;});_tmpFE;});if(_tmp8D){int _tmp103=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmpD0);int _tmp104=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp87->impl))->fields);if(_tmp103 < _tmp104){struct Cyc_List_List*
wild_dps=0;{int i=0;for(0;i < _tmp104 - _tmp103;++ i){wild_dps=({struct Cyc_List_List*
_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105->hd=({struct _tuple8*_tmp106=
_cycalloc(sizeof(*_tmp106));_tmp106->f1=0;_tmp106->f2=Cyc_Tcpat_wild_pat(p->loc);
_tmp106;});_tmp105->tl=wild_dps;_tmp105;});}}*_tmp8C=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpD0,
wild_dps);_tmpD0=*_tmp8C;}else{if(_tmp103 == _tmp104)({void*_tmp107=0;Cyc_Tcutil_warn(
p->loc,({const char*_tmp108="unnecessary ... in struct pattern";_tag_dyneither(
_tmp108,sizeof(char),34);}),_tag_dyneither(_tmp107,sizeof(void*),0));});}}{
struct Cyc_List_List*fields=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,
struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,struct Cyc_List_List*
fields))Cyc_Tcutil_resolve_struct_designators)(rgn,p->loc,_tmpD0,((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(_tmp87->impl))->fields);for(0;fields != 0;fields=fields->tl){struct
_tuple10 _tmp10A;struct Cyc_Absyn_Aggrfield*_tmp10B;struct Cyc_Absyn_Pat*_tmp10C;
struct _tuple10*_tmp109=(struct _tuple10*)fields->hd;_tmp10A=*_tmp109;_tmp10B=
_tmp10A.f1;_tmp10C=_tmp10A.f2;{void*_tmp10D=Cyc_Tcutil_rsubstitute(rgn,_tmpF8,(
void*)_tmp10B->type);res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te2,
_tmp10C,(void**)& _tmp10D,rgn_opt));if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp10C->topt))->v,_tmp10D))({struct Cyc_String_pa_struct _tmp112;
_tmp112.tag=0;_tmp112.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp10C->topt))->v));{struct Cyc_String_pa_struct
_tmp111;_tmp111.tag=0;_tmp111.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp10D));{struct Cyc_String_pa_struct _tmp110;_tmp110.tag=
0;_tmp110.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp10B->name);{void*
_tmp10E[3]={& _tmp110,& _tmp111,& _tmp112};Cyc_Tcutil_terr(p->loc,({const char*
_tmp10F="field %s of struct pattern expects type %s != %s";_tag_dyneither(
_tmp10F,sizeof(char),49);}),_tag_dyneither(_tmp10E,sizeof(void*),3));}}}});}}}};
_pop_region(r2);}};_pop_region(rgn);}goto _LL3C;}_LL5B: if(_tmp74 <= (void*)2)goto
_LL5D;if(*((int*)_tmp74)!= 6)goto _LL5D;_tmp8E=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp74)->f1;_tmp8F=((struct Cyc_Absyn_Tunion_p_struct*)_tmp74)->f2;_tmp90=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp74)->f3;_tmp91=(struct Cyc_List_List**)&((
struct Cyc_Absyn_Tunion_p_struct*)_tmp74)->f3;_tmp92=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp74)->f4;_LL5C: {struct Cyc_List_List*_tmp114=*_tmp91;if(_tmp8E->is_flat)
rgn_opt=0;{struct _RegionHandle _tmp115=_new_region("rgn");struct _RegionHandle*rgn=&
_tmp115;_push_region(rgn);{struct Cyc_List_List*tqts=_tmp8F->typs;struct Cyc_List_List*
_tmp116=Cyc_Tcenv_lookup_type_vars(te);struct _tuple4 _tmp117=({struct _tuple4
_tmp140;_tmp140.f1=_tmp116;_tmp140.f2=rgn;_tmp140;});struct Cyc_List_List*_tmp118=((
struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple5*(*f)(struct _tuple4*,
struct Cyc_Absyn_Tvar*),struct _tuple4*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(
rgn,Cyc_Tcutil_r_make_inst_var,& _tmp117,_tmp8E->tvs);struct Cyc_List_List*_tmp119=((
struct Cyc_List_List*(*)(void*(*f)(struct _tuple5*),struct Cyc_List_List*x))Cyc_List_map)((
void*(*)(struct _tuple5*))Cyc_Core_snd,_tmp118);if(tqts == 0  || _tmp8E->is_flat){
struct Cyc_Core_Opt*ropt=0;if(!_tmp8E->is_flat)ropt=({struct Cyc_Core_Opt*_tmp11A=
_cycalloc(sizeof(*_tmp11A));_tmp11A->v=(void*)Cyc_Absyn_new_evar((struct Cyc_Core_Opt*)&
Cyc_Tcutil_rk,({struct Cyc_Core_Opt*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B->v=
_tmp116;_tmp11B;}));_tmp11A;});t=(void*)({struct Cyc_Absyn_TunionType_struct*
_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C[0]=({struct Cyc_Absyn_TunionType_struct
_tmp11D;_tmp11D.tag=2;_tmp11D.f1=({struct Cyc_Absyn_TunionInfo _tmp11E;_tmp11E.tunion_info=(
union Cyc_Absyn_TunionInfoU_union)({union Cyc_Absyn_TunionInfoU_union _tmp11F;(
_tmp11F.KnownTunion).tag=1;(_tmp11F.KnownTunion).f1=({struct Cyc_Absyn_Tuniondecl**
_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120[0]=_tmp8E;_tmp120;});_tmp11F;});
_tmp11E.targs=_tmp119;_tmp11E.rgn=ropt;_tmp11E;});_tmp11D;});_tmp11C;});}else{t=(
void*)({struct Cyc_Absyn_TunionFieldType_struct*_tmp121=_cycalloc(sizeof(*_tmp121));
_tmp121[0]=({struct Cyc_Absyn_TunionFieldType_struct _tmp122;_tmp122.tag=3;_tmp122.f1=({
struct Cyc_Absyn_TunionFieldInfo _tmp123;_tmp123.field_info=(union Cyc_Absyn_TunionFieldInfoU_union)({
union Cyc_Absyn_TunionFieldInfoU_union _tmp124;(_tmp124.KnownTunionfield).tag=1;(
_tmp124.KnownTunionfield).f1=_tmp8E;(_tmp124.KnownTunionfield).f2=_tmp8F;_tmp124;});
_tmp123.targs=_tmp119;_tmp123;});_tmp122;});_tmp121;});}if(topt != 0  && (tqts == 0
 || _tmp8E->is_flat)){void*_tmp125=Cyc_Tcutil_compress(*topt);struct Cyc_Absyn_TunionInfo
_tmp126;struct Cyc_List_List*_tmp127;_LL88: if(_tmp125 <= (void*)4)goto _LL8C;if(*((
int*)_tmp125)!= 3)goto _LL8A;_LL89: t=(void*)({struct Cyc_Absyn_TunionFieldType_struct*
_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128[0]=({struct Cyc_Absyn_TunionFieldType_struct
_tmp129;_tmp129.tag=3;_tmp129.f1=({struct Cyc_Absyn_TunionFieldInfo _tmp12A;
_tmp12A.field_info=(union Cyc_Absyn_TunionFieldInfoU_union)({union Cyc_Absyn_TunionFieldInfoU_union
_tmp12B;(_tmp12B.KnownTunionfield).tag=1;(_tmp12B.KnownTunionfield).f1=_tmp8E;(
_tmp12B.KnownTunionfield).f2=_tmp8F;_tmp12B;});_tmp12A.targs=_tmp119;_tmp12A;});
_tmp129;});_tmp128;});goto _LL87;_LL8A: if(*((int*)_tmp125)!= 2)goto _LL8C;_tmp126=((
struct Cyc_Absyn_TunionType_struct*)_tmp125)->f1;_tmp127=_tmp126.targs;_LL8B: {
struct Cyc_List_List*_tmp12C=_tmp119;for(0;_tmp12C != 0  && _tmp127 != 0;(_tmp12C=
_tmp12C->tl,_tmp127=_tmp127->tl)){Cyc_Tcutil_unify((void*)_tmp12C->hd,(void*)
_tmp127->hd);}goto _LL87;}_LL8C:;_LL8D: goto _LL87;_LL87:;}if(_tmp92){int _tmp12D=((
int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp114);int _tmp12E=((int(*)(
struct Cyc_List_List*x))Cyc_List_length)(tqts);if(_tmp12D < _tmp12E){struct Cyc_List_List*
wild_ps=0;{int i=0;for(0;i < _tmp12E - _tmp12D;++ i){wild_ps=({struct Cyc_List_List*
_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F->hd=Cyc_Tcpat_wild_pat(p->loc);
_tmp12F->tl=wild_ps;_tmp12F;});}}*_tmp91=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp114,wild_ps);_tmp114=*_tmp91;}
else{if(_tmp12D == _tmp12E)({struct Cyc_String_pa_struct _tmp132;_tmp132.tag=0;
_tmp132.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp8E->name));{void*_tmp130[1]={& _tmp132};Cyc_Tcutil_warn(p->loc,({const char*
_tmp131="unnecessary ... in tunion field %s";_tag_dyneither(_tmp131,sizeof(char),
35);}),_tag_dyneither(_tmp130,sizeof(void*),1));}});}}for(0;_tmp114 != 0  && tqts
!= 0;(_tmp114=_tmp114->tl,tqts=tqts->tl)){struct Cyc_Absyn_Pat*_tmp133=(struct Cyc_Absyn_Pat*)
_tmp114->hd;void*_tmp134=Cyc_Tcutil_rsubstitute(rgn,_tmp118,(*((struct _tuple9*)
tqts->hd)).f2);res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmp133,(
void**)& _tmp134,rgn_opt));if(!Cyc_Tcutil_unify((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp133->topt))->v,_tmp134))({struct Cyc_String_pa_struct _tmp139;
_tmp139.tag=0;_tmp139.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp133->topt))->v));{struct Cyc_String_pa_struct
_tmp138;_tmp138.tag=0;_tmp138.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp134));{struct Cyc_String_pa_struct _tmp137;_tmp137.tag=
0;_tmp137.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp8F->name));{void*_tmp135[3]={& _tmp137,& _tmp138,& _tmp139};Cyc_Tcutil_terr(
_tmp133->loc,({const char*_tmp136="%s expects argument type %s, not %s";
_tag_dyneither(_tmp136,sizeof(char),36);}),_tag_dyneither(_tmp135,sizeof(void*),
3));}}}});}if(_tmp114 != 0)({struct Cyc_String_pa_struct _tmp13C;_tmp13C.tag=0;
_tmp13C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(
_tmp8F->name));{void*_tmp13A[1]={& _tmp13C};Cyc_Tcutil_terr(p->loc,({const char*
_tmp13B="too many arguments for tunion constructor %s";_tag_dyneither(_tmp13B,
sizeof(char),45);}),_tag_dyneither(_tmp13A,sizeof(void*),1));}});if(tqts != 0)({
struct Cyc_String_pa_struct _tmp13F;_tmp13F.tag=0;_tmp13F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp8F->name));{void*_tmp13D[1]={&
_tmp13F};Cyc_Tcutil_terr(p->loc,({const char*_tmp13E="too few arguments for tunion constructor %s";
_tag_dyneither(_tmp13E,sizeof(char),44);}),_tag_dyneither(_tmp13D,sizeof(void*),
1));}});};_pop_region(rgn);}goto _LL3C;}_LL5D: if(_tmp74 <= (void*)2)goto _LL5F;if(*((
int*)_tmp74)!= 5)goto _LL5F;_tmp93=((struct Cyc_Absyn_Aggr_p_struct*)_tmp74)->f1;
_tmp94=_tmp93.aggr_info;if((((((struct Cyc_Absyn_Aggr_p_struct*)_tmp74)->f1).aggr_info).UnknownAggr).tag
!= 0)goto _LL5F;_LL5E: goto _LL60;_LL5F: if(_tmp74 <= (void*)2)goto _LL61;if(*((int*)
_tmp74)!= 12)goto _LL61;_LL60: goto _LL62;_LL61: if(_tmp74 <= (void*)2)goto _LL63;if(*((
int*)_tmp74)!= 14)goto _LL63;_LL62: goto _LL64;_LL63: if(_tmp74 <= (void*)2)goto _LL3C;
if(*((int*)_tmp74)!= 13)goto _LL3C;_LL64: t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(
te));goto _LL3C;_LL3C:;}tcpat_end: p->topt=({struct Cyc_Core_Opt*_tmp141=_cycalloc(
sizeof(*_tmp141));_tmp141->v=(void*)t;_tmp141;});return res;}}struct Cyc_Tcpat_TcPatResult
Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt){struct
Cyc_Tcpat_TcPatResult _tmp143=Cyc_Tcpat_tcPatRec(te,p,(void**)topt,0);{struct
_RegionHandle _tmp144=_new_region("r");struct _RegionHandle*r=& _tmp144;
_push_region(r);Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*
x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp143.patvars),p->loc,({const char*
_tmp145="pattern contains a repeated variable";_tag_dyneither(_tmp145,sizeof(
char),37);}));;_pop_region(r);}return _tmp143;}void Cyc_Tcpat_check_pat_regions(
struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){void*_tmp146=(void*)p->r;struct
Cyc_Absyn_Pat*_tmp147;struct Cyc_List_List*_tmp148;struct Cyc_List_List*_tmp149;
struct Cyc_List_List*_tmp14A;_LL8F: if(_tmp146 <= (void*)2)goto _LL97;if(*((int*)
_tmp146)!= 4)goto _LL91;_tmp147=((struct Cyc_Absyn_Pointer_p_struct*)_tmp146)->f1;
_LL90: Cyc_Tcpat_check_pat_regions(te,_tmp147);if(Cyc_Tcutil_is_noalias_pointer_or_aggr((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp147->topt))->v)){({void*_tmp14B=0;
Cyc_Tcutil_terr(p->loc,({const char*_tmp14C="Pattern dereferences to a non-aliased pointer; use swap";
_tag_dyneither(_tmp14C,sizeof(char),56);}),_tag_dyneither(_tmp14B,sizeof(void*),
0));});return;}if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v)){({void*_tmp14D=0;Cyc_Tcutil_terr(p->loc,({const char*
_tmp14E="Pattern dereferences a non-aliased pointer; use swap";_tag_dyneither(
_tmp14E,sizeof(char),53);}),_tag_dyneither(_tmp14D,sizeof(void*),0));});return;}{
void*_tmp14F=(void*)((struct Cyc_Core_Opt*)_check_null(p->topt))->v;struct Cyc_Absyn_PtrInfo
_tmp150;struct Cyc_Absyn_PtrAtts _tmp151;void*_tmp152;struct Cyc_Absyn_TunionInfo
_tmp153;struct Cyc_Core_Opt*_tmp154;struct Cyc_Core_Opt _tmp155;void*_tmp156;_LL9A:
if(_tmp14F <= (void*)4)goto _LL9E;if(*((int*)_tmp14F)!= 4)goto _LL9C;_tmp150=((
struct Cyc_Absyn_PointerType_struct*)_tmp14F)->f1;_tmp151=_tmp150.ptr_atts;
_tmp152=(void*)_tmp151.rgn;_LL9B: _tmp156=_tmp152;goto _LL9D;_LL9C: if(*((int*)
_tmp14F)!= 2)goto _LL9E;_tmp153=((struct Cyc_Absyn_TunionType_struct*)_tmp14F)->f1;
_tmp154=_tmp153.rgn;if(_tmp154 == 0)goto _LL9E;_tmp155=*_tmp154;_tmp156=(void*)
_tmp155.v;_LL9D: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp156);return;_LL9E:;
_LL9F:({void*_tmp157=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp158="check_pat_regions: bad pointer type";
_tag_dyneither(_tmp158,sizeof(char),36);}),_tag_dyneither(_tmp157,sizeof(void*),
0));});_LL99:;}_LL91: if(*((int*)_tmp146)!= 6)goto _LL93;_tmp148=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp146)->f3;_LL92: for(0;_tmp148 != 0;_tmp148=_tmp148->tl){Cyc_Tcpat_check_pat_regions(
te,(struct Cyc_Absyn_Pat*)_tmp148->hd);}{void*_tmp159=(void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v;struct Cyc_Absyn_TunionInfo _tmp15A;struct Cyc_Core_Opt*
_tmp15B;struct Cyc_Absyn_TunionInfo _tmp15C;struct Cyc_Core_Opt*_tmp15D;struct Cyc_Core_Opt
_tmp15E;void*_tmp15F;_LLA1: if(_tmp159 <= (void*)4)goto _LLA7;if(*((int*)_tmp159)!= 
2)goto _LLA3;_tmp15A=((struct Cyc_Absyn_TunionType_struct*)_tmp159)->f1;_tmp15B=
_tmp15A.rgn;if(_tmp15B != 0)goto _LLA3;_LLA2: return;_LLA3: if(*((int*)_tmp159)!= 2)
goto _LLA5;_tmp15C=((struct Cyc_Absyn_TunionType_struct*)_tmp159)->f1;_tmp15D=
_tmp15C.rgn;if(_tmp15D == 0)goto _LLA5;_tmp15E=*_tmp15D;_tmp15F=(void*)_tmp15E.v;
_LLA4: Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp15F);return;_LLA5: if(*((int*)
_tmp159)!= 3)goto _LLA7;_LLA6: return;_LLA7:;_LLA8:({void*_tmp160=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp161="check_pat_regions: bad tunion type";_tag_dyneither(_tmp161,sizeof(char),
35);}),_tag_dyneither(_tmp160,sizeof(void*),0));});_LLA0:;}_LL93: if(*((int*)
_tmp146)!= 5)goto _LL95;_tmp149=((struct Cyc_Absyn_Aggr_p_struct*)_tmp146)->f3;
_LL94: for(0;_tmp149 != 0;_tmp149=_tmp149->tl){Cyc_Tcpat_check_pat_regions(te,(*((
struct _tuple8*)_tmp149->hd)).f2);}return;_LL95: if(*((int*)_tmp146)!= 3)goto _LL97;
_tmp14A=((struct Cyc_Absyn_Tuple_p_struct*)_tmp146)->f1;_LL96: for(0;_tmp14A != 0;
_tmp14A=_tmp14A->tl){Cyc_Tcpat_check_pat_regions(te,(struct Cyc_Absyn_Pat*)
_tmp14A->hd);}return;_LL97:;_LL98: return;_LL8E:;}struct Cyc_Tcpat_Name_v_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Tcpat_Int_v_struct{int tag;int f1;};
union Cyc_Tcpat_Name_value_union{struct Cyc_Tcpat_Name_v_struct Name_v;struct Cyc_Tcpat_Int_v_struct
Int_v;};struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value_union name;int arity;int*
span;struct Cyc_Absyn_Pat*orig_pat;};struct Cyc_Tcpat_Con_struct{int tag;struct Cyc_Tcpat_Con_s*
f1;struct Cyc_List_List*f2;};static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*
c1,struct Cyc_Tcpat_Con_s*c2){union Cyc_Tcpat_Name_value_union _tmp162=c1->name;
struct _dyneither_ptr _tmp163;int _tmp164;_LLAA: if((_tmp162.Name_v).tag != 0)goto
_LLAC;_tmp163=(_tmp162.Name_v).f1;_LLAB: {union Cyc_Tcpat_Name_value_union _tmp165=
c2->name;struct _dyneither_ptr _tmp166;_LLAF: if((_tmp165.Name_v).tag != 0)goto _LLB1;
_tmp166=(_tmp165.Name_v).f1;_LLB0: return Cyc_strcmp((struct _dyneither_ptr)_tmp163,(
struct _dyneither_ptr)_tmp166);_LLB1: if((_tmp165.Int_v).tag != 1)goto _LLAE;_LLB2:
return - 1;_LLAE:;}_LLAC: if((_tmp162.Int_v).tag != 1)goto _LLA9;_tmp164=(_tmp162.Int_v).f1;
_LLAD: {union Cyc_Tcpat_Name_value_union _tmp167=c2->name;int _tmp168;_LLB4: if((
_tmp167.Name_v).tag != 0)goto _LLB6;_LLB5: return 1;_LLB6: if((_tmp167.Int_v).tag != 1)
goto _LLB3;_tmp168=(_tmp167.Int_v).f1;_LLB7: return _tmp164 - _tmp168;_LLB3:;}_LLA9:;}
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct _RegionHandle*r){return((
struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,
struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}static int Cyc_Tcpat_one_opt=
1;static int Cyc_Tcpat_two_opt=2;static int Cyc_Tcpat_twofiftysix_opt=256;static
struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*
p){return({struct Cyc_Tcpat_Con_s*_tmp169=_region_malloc(r,sizeof(*_tmp169));
_tmp169->name=(union Cyc_Tcpat_Name_value_union)({union Cyc_Tcpat_Name_value_union
_tmp16A;(_tmp16A.Name_v).tag=0;(_tmp16A.Name_v).f1=({const char*_tmp16B="NULL";
_tag_dyneither(_tmp16B,sizeof(char),5);});_tmp16A;});_tmp169->arity=0;_tmp169->span=(
int*)& Cyc_Tcpat_two_opt;_tmp169->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp169;});}
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct _RegionHandle*r,struct
Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*_tmp16C=_region_malloc(r,sizeof(*
_tmp16C));_tmp16C->name=(union Cyc_Tcpat_Name_value_union)({union Cyc_Tcpat_Name_value_union
_tmp16D;(_tmp16D.Name_v).tag=0;(_tmp16D.Name_v).f1=({const char*_tmp16E="&";
_tag_dyneither(_tmp16E,sizeof(char),2);});_tmp16D;});_tmp16C->arity=1;_tmp16C->span=(
int*)& Cyc_Tcpat_two_opt;_tmp16C->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp16C;});}
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*
p){return({struct Cyc_Tcpat_Con_s*_tmp16F=_region_malloc(r,sizeof(*_tmp16F));
_tmp16F->name=(union Cyc_Tcpat_Name_value_union)({union Cyc_Tcpat_Name_value_union
_tmp170;(_tmp170.Name_v).tag=0;(_tmp170.Name_v).f1=({const char*_tmp171="&";
_tag_dyneither(_tmp171,sizeof(char),2);});_tmp170;});_tmp16F->arity=1;_tmp16F->span=(
int*)& Cyc_Tcpat_one_opt;_tmp16F->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp16F;});}
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(struct _RegionHandle*r,int i,struct
Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*_tmp172=_region_malloc(r,sizeof(*
_tmp172));_tmp172->name=(union Cyc_Tcpat_Name_value_union)({union Cyc_Tcpat_Name_value_union
_tmp173;(_tmp173.Int_v).tag=1;(_tmp173.Int_v).f1=i;_tmp173;});_tmp172->arity=0;
_tmp172->span=0;_tmp172->orig_pat=p;_tmp172;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(
struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){return({
struct Cyc_Tcpat_Con_s*_tmp174=_region_malloc(r,sizeof(*_tmp174));_tmp174->name=(
union Cyc_Tcpat_Name_value_union)({union Cyc_Tcpat_Name_value_union _tmp175;(
_tmp175.Name_v).tag=0;(_tmp175.Name_v).f1=f;_tmp175;});_tmp174->arity=0;_tmp174->span=
0;_tmp174->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp174;});}static struct Cyc_Tcpat_Con_s*
Cyc_Tcpat_char_con(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){return({
struct Cyc_Tcpat_Con_s*_tmp176=_region_malloc(r,sizeof(*_tmp176));_tmp176->name=(
union Cyc_Tcpat_Name_value_union)({union Cyc_Tcpat_Name_value_union _tmp177;(
_tmp177.Int_v).tag=1;(_tmp177.Int_v).f1=(int)c;_tmp177;});_tmp176->arity=0;
_tmp176->span=(int*)& Cyc_Tcpat_twofiftysix_opt;_tmp176->orig_pat=(struct Cyc_Absyn_Pat*)
p;_tmp176;});}static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(struct
_RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){return({struct Cyc_Tcpat_Con_s*
_tmp178=_region_malloc(r,sizeof(*_tmp178));_tmp178->name=(union Cyc_Tcpat_Name_value_union)({
union Cyc_Tcpat_Name_value_union _tmp179;(_tmp179.Name_v).tag=0;(_tmp179.Name_v).f1=({
const char*_tmp17A="$";_tag_dyneither(_tmp17A,sizeof(char),2);});_tmp179;});
_tmp178->arity=i;_tmp178->span=(int*)& Cyc_Tcpat_one_opt;_tmp178->orig_pat=p;
_tmp178;});}static void*Cyc_Tcpat_null_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*
p){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp17B=_region_malloc(r,sizeof(*
_tmp17B));_tmp17B[0]=({struct Cyc_Tcpat_Con_struct _tmp17C;_tmp17C.tag=0;_tmp17C.f1=
Cyc_Tcpat_null_con(r,p);_tmp17C.f2=0;_tmp17C;});_tmp17B;});}static void*Cyc_Tcpat_int_pat(
struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp17D=_region_malloc(r,sizeof(*_tmp17D));_tmp17D[0]=({struct Cyc_Tcpat_Con_struct
_tmp17E;_tmp17E.tag=0;_tmp17E.f1=Cyc_Tcpat_int_con(r,i,p);_tmp17E.f2=0;_tmp17E;});
_tmp17D;});}static void*Cyc_Tcpat_char_pat(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*
p){return(void*)({struct Cyc_Tcpat_Con_struct*_tmp17F=_region_malloc(r,sizeof(*
_tmp17F));_tmp17F[0]=({struct Cyc_Tcpat_Con_struct _tmp180;_tmp180.tag=0;_tmp180.f1=
Cyc_Tcpat_char_con(r,c,p);_tmp180.f2=0;_tmp180;});_tmp17F;});}static void*Cyc_Tcpat_float_pat(
struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){return(void*)({
struct Cyc_Tcpat_Con_struct*_tmp181=_region_malloc(r,sizeof(*_tmp181));_tmp181[0]=({
struct Cyc_Tcpat_Con_struct _tmp182;_tmp182.tag=0;_tmp182.f1=Cyc_Tcpat_float_con(r,
f,p);_tmp182.f2=0;_tmp182;});_tmp181;});}static void*Cyc_Tcpat_null_ptr_pat(
struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp183=_region_malloc(r,sizeof(*_tmp183));_tmp183[0]=({struct Cyc_Tcpat_Con_struct
_tmp184;_tmp184.tag=0;_tmp184.f1=Cyc_Tcpat_null_ptr_con(r,p0);_tmp184.f2=({
struct Cyc_List_List*_tmp185=_region_malloc(r,sizeof(*_tmp185));_tmp185->hd=(void*)
p;_tmp185->tl=0;_tmp185;});_tmp184;});_tmp183;});}static void*Cyc_Tcpat_ptr_pat(
struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp186=_region_malloc(r,sizeof(*_tmp186));_tmp186[0]=({struct Cyc_Tcpat_Con_struct
_tmp187;_tmp187.tag=0;_tmp187.f1=Cyc_Tcpat_ptr_con(r,p0);_tmp187.f2=({struct Cyc_List_List*
_tmp188=_region_malloc(r,sizeof(*_tmp188));_tmp188->hd=(void*)p;_tmp188->tl=0;
_tmp188;});_tmp187;});_tmp186;});}static void*Cyc_Tcpat_tuple_pat(struct
_RegionHandle*r,struct Cyc_List_List*ss,struct Cyc_Absyn_Pat*p){return(void*)({
struct Cyc_Tcpat_Con_struct*_tmp189=_region_malloc(r,sizeof(*_tmp189));_tmp189[0]=({
struct Cyc_Tcpat_Con_struct _tmp18A;_tmp18A.tag=0;_tmp18A.f1=Cyc_Tcpat_tuple_con(r,
Cyc_List_length(ss),p);_tmp18A.f2=ss;_tmp18A;});_tmp189;});}static void*Cyc_Tcpat_con_pat(
struct _RegionHandle*r,struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*
ps,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_s*c=({struct Cyc_Tcpat_Con_s*
_tmp18D=_region_malloc(r,sizeof(*_tmp18D));_tmp18D->name=(union Cyc_Tcpat_Name_value_union)({
union Cyc_Tcpat_Name_value_union _tmp18E;(_tmp18E.Name_v).tag=0;(_tmp18E.Name_v).f1=
con_name;_tmp18E;});_tmp18D->arity=Cyc_List_length(ps);_tmp18D->span=span;
_tmp18D->orig_pat=(struct Cyc_Absyn_Pat*)p;_tmp18D;});return(void*)({struct Cyc_Tcpat_Con_struct*
_tmp18B=_region_malloc(r,sizeof(*_tmp18B));_tmp18B[0]=({struct Cyc_Tcpat_Con_struct
_tmp18C;_tmp18C.tag=0;_tmp18C.f1=c;_tmp18C.f2=ps;_tmp18C;});_tmp18B;});}static
void*Cyc_Tcpat_compile_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){void*s;{
void*_tmp18F=(void*)p->r;void*_tmp190;int _tmp191;char _tmp192;struct
_dyneither_ptr _tmp193;struct Cyc_Absyn_Pat*_tmp194;struct Cyc_Absyn_Pat*_tmp195;
struct Cyc_Absyn_Pat*_tmp196;struct Cyc_Absyn_Tuniondecl*_tmp197;struct Cyc_Absyn_Tunionfield*
_tmp198;struct Cyc_List_List*_tmp199;struct Cyc_List_List*_tmp19A;struct Cyc_Absyn_AggrInfo
_tmp19B;union Cyc_Absyn_AggrInfoU_union _tmp19C;struct Cyc_Absyn_Aggrdecl**_tmp19D;
struct Cyc_Absyn_Aggrdecl*_tmp19E;struct Cyc_List_List*_tmp19F;struct Cyc_Absyn_Enumdecl*
_tmp1A0;struct Cyc_Absyn_Enumfield*_tmp1A1;void*_tmp1A2;struct Cyc_Absyn_Enumfield*
_tmp1A3;_LLB9: if((int)_tmp18F != 0)goto _LLBB;_LLBA: goto _LLBC;_LLBB: if(_tmp18F <= (
void*)2)goto _LLBD;if(*((int*)_tmp18F)!= 2)goto _LLBD;_LLBC: s=(void*)0;goto _LLB8;
_LLBD: if((int)_tmp18F != 1)goto _LLBF;_LLBE: s=Cyc_Tcpat_null_pat(r,p);goto _LLB8;
_LLBF: if(_tmp18F <= (void*)2)goto _LLC1;if(*((int*)_tmp18F)!= 7)goto _LLC1;_tmp190=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp18F)->f1;_tmp191=((struct Cyc_Absyn_Int_p_struct*)
_tmp18F)->f2;_LLC0: s=Cyc_Tcpat_int_pat(r,_tmp191,(struct Cyc_Absyn_Pat*)p);goto
_LLB8;_LLC1: if(_tmp18F <= (void*)2)goto _LLC3;if(*((int*)_tmp18F)!= 8)goto _LLC3;
_tmp192=((struct Cyc_Absyn_Char_p_struct*)_tmp18F)->f1;_LLC2: s=Cyc_Tcpat_char_pat(
r,_tmp192,p);goto _LLB8;_LLC3: if(_tmp18F <= (void*)2)goto _LLC5;if(*((int*)_tmp18F)
!= 9)goto _LLC5;_tmp193=((struct Cyc_Absyn_Float_p_struct*)_tmp18F)->f1;_LLC4: s=
Cyc_Tcpat_float_pat(r,_tmp193,p);goto _LLB8;_LLC5: if(_tmp18F <= (void*)2)goto _LLC7;
if(*((int*)_tmp18F)!= 0)goto _LLC7;_tmp194=((struct Cyc_Absyn_Var_p_struct*)
_tmp18F)->f2;_LLC6: s=Cyc_Tcpat_compile_pat(r,_tmp194);goto _LLB8;_LLC7: if(_tmp18F
<= (void*)2)goto _LLC9;if(*((int*)_tmp18F)!= 1)goto _LLC9;_tmp195=((struct Cyc_Absyn_Reference_p_struct*)
_tmp18F)->f2;_LLC8: s=Cyc_Tcpat_compile_pat(r,_tmp195);goto _LLB8;_LLC9: if(_tmp18F
<= (void*)2)goto _LLCB;if(*((int*)_tmp18F)!= 4)goto _LLCB;_tmp196=((struct Cyc_Absyn_Pointer_p_struct*)
_tmp18F)->f1;_LLCA:{void*_tmp1A4=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(p->topt))->v);struct Cyc_Absyn_PtrInfo _tmp1A5;struct Cyc_Absyn_PtrAtts
_tmp1A6;struct Cyc_Absyn_Conref*_tmp1A7;struct Cyc_Absyn_TunionInfo _tmp1A8;struct
Cyc_Core_Opt*_tmp1A9;struct Cyc_Core_Opt _tmp1AA;_LLDE: if(_tmp1A4 <= (void*)4)goto
_LLE2;if(*((int*)_tmp1A4)!= 4)goto _LLE0;_tmp1A5=((struct Cyc_Absyn_PointerType_struct*)
_tmp1A4)->f1;_tmp1A6=_tmp1A5.ptr_atts;_tmp1A7=_tmp1A6.nullable;_LLDF: {int
is_nullable=0;int still_working=1;while(still_working){union Cyc_Absyn_Constraint_union
_tmp1AB=_tmp1A7->v;struct Cyc_Absyn_Conref*_tmp1AC;int _tmp1AD;_LLE5: if((_tmp1AB.Forward_constr).tag
!= 1)goto _LLE7;_tmp1AC=(_tmp1AB.Forward_constr).f1;_LLE6: _tmp1A7->v=_tmp1AC->v;
continue;_LLE7: if((_tmp1AB.No_constr).tag != 2)goto _LLE9;_LLE8: _tmp1A7->v=(union
Cyc_Absyn_Constraint_union)({union Cyc_Absyn_Constraint_union _tmp1AE;(_tmp1AE.Eq_constr).tag=
0;(_tmp1AE.Eq_constr).f1=(void*)0;_tmp1AE;});is_nullable=0;still_working=0;goto
_LLE4;_LLE9: if((_tmp1AB.Eq_constr).tag != 0)goto _LLE4;_tmp1AD=(_tmp1AB.Eq_constr).f1;
_LLEA: is_nullable=(int)_tmp1AD;still_working=0;goto _LLE4;_LLE4:;}{void*ss=Cyc_Tcpat_compile_pat(
r,_tmp196);if(is_nullable)s=Cyc_Tcpat_null_ptr_pat(r,ss,p);else{s=Cyc_Tcpat_ptr_pat(
r,ss,p);}goto _LLDD;}}_LLE0: if(*((int*)_tmp1A4)!= 2)goto _LLE2;_tmp1A8=((struct Cyc_Absyn_TunionType_struct*)
_tmp1A4)->f1;_tmp1A9=_tmp1A8.rgn;if(_tmp1A9 == 0)goto _LLE2;_tmp1AA=*_tmp1A9;_LLE1:{
void*_tmp1AF=(void*)_tmp196->r;struct Cyc_Absyn_Tuniondecl*_tmp1B0;struct Cyc_Absyn_Tunionfield*
_tmp1B1;struct Cyc_List_List*_tmp1B2;_LLEC: if(_tmp1AF <= (void*)2)goto _LLEE;if(*((
int*)_tmp1AF)!= 6)goto _LLEE;_tmp1B0=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1AF)->f1;
_tmp1B1=((struct Cyc_Absyn_Tunion_p_struct*)_tmp1AF)->f2;_tmp1B2=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp1AF)->f3;_LLED: {int*span;if(_tmp1B0->is_xtunion)span=0;else{span=({int*
_tmp1B3=_region_malloc(r,sizeof(*_tmp1B3));_tmp1B3[0]=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp1B0->fields))->v);_tmp1B3;});}s=Cyc_Tcpat_con_pat(r,*(*_tmp1B1->name).f2,
span,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct
_RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*
x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp1B2),p);goto _LLEB;}_LLEE:;
_LLEF:({void*_tmp1B4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Tcutil_impos)(({const char*_tmp1B5="non-[x]tunion pattern has tunion type";
_tag_dyneither(_tmp1B5,sizeof(char),38);}),_tag_dyneither(_tmp1B4,sizeof(void*),
0));});_LLEB:;}goto _LLDD;_LLE2:;_LLE3:({void*_tmp1B6=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1B7="pointer pattern does not have pointer type";
_tag_dyneither(_tmp1B7,sizeof(char),43);}),_tag_dyneither(_tmp1B6,sizeof(void*),
0));});_LLDD:;}goto _LLB8;_LLCB: if(_tmp18F <= (void*)2)goto _LLCD;if(*((int*)
_tmp18F)!= 6)goto _LLCD;_tmp197=((struct Cyc_Absyn_Tunion_p_struct*)_tmp18F)->f1;
_tmp198=((struct Cyc_Absyn_Tunion_p_struct*)_tmp18F)->f2;_tmp199=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp18F)->f3;_LLCC: {int*span;{void*_tmp1B8=Cyc_Tcutil_compress((void*)((struct
Cyc_Core_Opt*)_check_null(p->topt))->v);_LLF1: if(_tmp1B8 <= (void*)4)goto _LLF5;
if(*((int*)_tmp1B8)!= 2)goto _LLF3;_LLF2: if(_tmp197->is_xtunion)span=0;else{span=({
int*_tmp1B9=_region_malloc(r,sizeof(*_tmp1B9));_tmp1B9[0]=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(
_tmp197->fields))->v);_tmp1B9;});}goto _LLF0;_LLF3: if(*((int*)_tmp1B8)!= 3)goto
_LLF5;_LLF4: span=(int*)& Cyc_Tcpat_one_opt;goto _LLF0;_LLF5:;_LLF6: span=({void*
_tmp1BA=0;((int*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp1BB="void tunion pattern has bad type";_tag_dyneither(_tmp1BB,
sizeof(char),33);}),_tag_dyneither(_tmp1BA,sizeof(void*),0));});goto _LLF0;_LLF0:;}
s=Cyc_Tcpat_con_pat(r,*(*_tmp198->name).f2,span,((struct Cyc_List_List*(*)(struct
_RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,
r,_tmp199),p);goto _LLB8;}_LLCD: if(_tmp18F <= (void*)2)goto _LLCF;if(*((int*)
_tmp18F)!= 3)goto _LLCF;_tmp19A=((struct Cyc_Absyn_Tuple_p_struct*)_tmp18F)->f1;
_LLCE: s=Cyc_Tcpat_tuple_pat(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp19A),(
struct Cyc_Absyn_Pat*)p);goto _LLB8;_LLCF: if(_tmp18F <= (void*)2)goto _LLD1;if(*((
int*)_tmp18F)!= 5)goto _LLD1;_tmp19B=((struct Cyc_Absyn_Aggr_p_struct*)_tmp18F)->f1;
_tmp19C=_tmp19B.aggr_info;if((((((struct Cyc_Absyn_Aggr_p_struct*)_tmp18F)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LLD1;_tmp19D=(_tmp19C.KnownAggr).f1;_tmp19E=*_tmp19D;_tmp19F=((struct
Cyc_Absyn_Aggr_p_struct*)_tmp18F)->f3;_LLD0: {struct Cyc_List_List*ps=0;{struct
Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp19E->impl))->fields;
for(0;fields != 0;fields=fields->tl){int found=Cyc_strcmp((struct _dyneither_ptr)*((
struct Cyc_Absyn_Aggrfield*)fields->hd)->name,({const char*_tmp1CA="";
_tag_dyneither(_tmp1CA,sizeof(char),1);}))== 0;{struct Cyc_List_List*dlps0=
_tmp19F;for(0;!found  && dlps0 != 0;dlps0=dlps0->tl){struct _tuple8 _tmp1BD;struct
Cyc_List_List*_tmp1BE;struct Cyc_Absyn_Pat*_tmp1BF;struct _tuple8*_tmp1BC=(struct
_tuple8*)dlps0->hd;_tmp1BD=*_tmp1BC;_tmp1BE=_tmp1BD.f1;_tmp1BF=_tmp1BD.f2;{
struct Cyc_List_List*_tmp1C0=_tmp1BE;struct Cyc_List_List _tmp1C1;void*_tmp1C2;
struct _dyneither_ptr*_tmp1C3;struct Cyc_List_List*_tmp1C4;_LLF8: if(_tmp1C0 == 0)
goto _LLFA;_tmp1C1=*_tmp1C0;_tmp1C2=(void*)_tmp1C1.hd;if(*((int*)_tmp1C2)!= 1)
goto _LLFA;_tmp1C3=((struct Cyc_Absyn_FieldName_struct*)_tmp1C2)->f1;_tmp1C4=
_tmp1C1.tl;if(_tmp1C4 != 0)goto _LLFA;_LLF9: if(Cyc_strptrcmp(_tmp1C3,((struct Cyc_Absyn_Aggrfield*)
fields->hd)->name)== 0){ps=({struct Cyc_List_List*_tmp1C5=_region_malloc(r,
sizeof(*_tmp1C5));_tmp1C5->hd=(void*)Cyc_Tcpat_compile_pat(r,_tmp1BF);_tmp1C5->tl=
ps;_tmp1C5;});found=1;}goto _LLF7;_LLFA:;_LLFB:({void*_tmp1C6=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1C7="bad designator(s)";
_tag_dyneither(_tmp1C7,sizeof(char),18);}),_tag_dyneither(_tmp1C6,sizeof(void*),
0));});_LLF7:;}}}if(!found)({void*_tmp1C8=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1C9="bad designator";
_tag_dyneither(_tmp1C9,sizeof(char),15);}),_tag_dyneither(_tmp1C8,sizeof(void*),
0));});}}s=Cyc_Tcpat_tuple_pat(r,ps,(struct Cyc_Absyn_Pat*)p);goto _LLB8;}_LLD1:
if(_tmp18F <= (void*)2)goto _LLD3;if(*((int*)_tmp18F)!= 10)goto _LLD3;_tmp1A0=((
struct Cyc_Absyn_Enum_p_struct*)_tmp18F)->f1;_tmp1A1=((struct Cyc_Absyn_Enum_p_struct*)
_tmp18F)->f2;_LLD2: {int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1A0->fields))->v);s=
Cyc_Tcpat_con_pat(r,*(*_tmp1A1->name).f2,({int*_tmp1CB=_region_malloc(r,sizeof(*
_tmp1CB));_tmp1CB[0]=span;_tmp1CB;}),0,p);goto _LLB8;}_LLD3: if(_tmp18F <= (void*)2)
goto _LLD5;if(*((int*)_tmp18F)!= 11)goto _LLD5;_tmp1A2=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp18F)->f1;_tmp1A3=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp18F)->f2;_LLD4: {
struct Cyc_List_List*fields;{void*_tmp1CC=Cyc_Tcutil_compress(_tmp1A2);struct Cyc_List_List*
_tmp1CD;_LLFD: if(_tmp1CC <= (void*)4)goto _LLFF;if(*((int*)_tmp1CC)!= 13)goto _LLFF;
_tmp1CD=((struct Cyc_Absyn_AnonEnumType_struct*)_tmp1CC)->f1;_LLFE: fields=_tmp1CD;
goto _LLFC;_LLFF:;_LL100:({void*_tmp1CE=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1CF="bad type in AnonEnum_p";
_tag_dyneither(_tmp1CF,sizeof(char),23);}),_tag_dyneither(_tmp1CE,sizeof(void*),
0));});_LLFC:;}{int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
s=Cyc_Tcpat_con_pat(r,*(*_tmp1A3->name).f2,({int*_tmp1D0=_region_malloc(r,
sizeof(*_tmp1D0));_tmp1D0[0]=span;_tmp1D0;}),0,p);goto _LLB8;}}_LLD5: if(_tmp18F <= (
void*)2)goto _LLD7;if(*((int*)_tmp18F)!= 12)goto _LLD7;_LLD6: goto _LLD8;_LLD7: if(
_tmp18F <= (void*)2)goto _LLD9;if(*((int*)_tmp18F)!= 13)goto _LLD9;_LLD8: goto _LLDA;
_LLD9: if(_tmp18F <= (void*)2)goto _LLDB;if(*((int*)_tmp18F)!= 5)goto _LLDB;_LLDA:
goto _LLDC;_LLDB: if(_tmp18F <= (void*)2)goto _LLB8;if(*((int*)_tmp18F)!= 14)goto
_LLB8;_LLDC: s=(void*)0;_LLB8:;}return s;}struct Cyc_Tcpat_Pos_struct{int tag;struct
Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_struct{int tag;
struct Cyc_Set_Set*f1;};struct Cyc_Tcpat_Failure_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_struct{
int tag;void*f1;};struct Cyc_Tcpat_IfEq_struct{int tag;struct Cyc_List_List*f1;
struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};struct _tuple11{struct Cyc_List_List*f1;
struct Cyc_List_List*f2;struct Cyc_List_List*f3;};static void*Cyc_Tcpat_add_neg(
struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*c){void*_tmp1D1=td;struct Cyc_Set_Set*
_tmp1D2;_LL102: if(*((int*)_tmp1D1)!= 1)goto _LL104;_tmp1D2=((struct Cyc_Tcpat_Neg_struct*)
_tmp1D1)->f1;_LL103: if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))
Cyc_Set_member)(_tmp1D2,c))({void*_tmp1D3=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1D4="add_neg called when constructor already in set";
_tag_dyneither(_tmp1D4,sizeof(char),47);}),_tag_dyneither(_tmp1D3,sizeof(void*),
0));});if(c->span != 0  && ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(
_tmp1D2)+ 1 >= *((int*)_check_null(c->span)))({void*_tmp1D5=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1D6="add_neg called when |cs U {c}| >= span(c)";
_tag_dyneither(_tmp1D6,sizeof(char),42);}),_tag_dyneither(_tmp1D5,sizeof(void*),
0));});return(void*)({struct Cyc_Tcpat_Neg_struct*_tmp1D7=_region_malloc(r,
sizeof(*_tmp1D7));_tmp1D7[0]=({struct Cyc_Tcpat_Neg_struct _tmp1D8;_tmp1D8.tag=1;
_tmp1D8.f1=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,
struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(r,_tmp1D2,c);_tmp1D8;});_tmp1D7;});
_LL104: if(*((int*)_tmp1D1)!= 0)goto _LL101;_LL105:({void*_tmp1D9=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp1DA="add_neg called when td is Positive";_tag_dyneither(_tmp1DA,sizeof(char),
35);}),_tag_dyneither(_tmp1D9,sizeof(void*),0));});_LL101:;}static void*Cyc_Tcpat_static_match(
struct Cyc_Tcpat_Con_s*c,void*td){void*_tmp1DB=td;struct Cyc_Tcpat_Con_s*_tmp1DC;
struct Cyc_Set_Set*_tmp1DD;_LL107: if(*((int*)_tmp1DB)!= 0)goto _LL109;_tmp1DC=((
struct Cyc_Tcpat_Pos_struct*)_tmp1DB)->f1;_LL108: if(Cyc_Tcpat_compare_con(c,
_tmp1DC)== 0)return(void*)0;else{return(void*)1;}_LL109: if(*((int*)_tmp1DB)!= 1)
goto _LL106;_tmp1DD=((struct Cyc_Tcpat_Neg_struct*)_tmp1DB)->f1;_LL10A: if(((int(*)(
struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp1DD,c))return(
void*)1;else{if(c->span != 0  && *((int*)_check_null(c->span))== ((int(*)(struct
Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp1DD)+ 1)return(void*)0;else{return(void*)
2;}}_LL106:;}struct _tuple12{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
static struct Cyc_List_List*Cyc_Tcpat_augment(struct _RegionHandle*r,struct Cyc_List_List*
ctxt,void*dsc){struct Cyc_List_List*_tmp1DE=ctxt;struct Cyc_List_List _tmp1DF;
struct _tuple12*_tmp1E0;struct _tuple12 _tmp1E1;struct Cyc_Tcpat_Con_s*_tmp1E2;
struct Cyc_List_List*_tmp1E3;struct Cyc_List_List*_tmp1E4;_LL10C: if(_tmp1DE != 0)
goto _LL10E;_LL10D: return 0;_LL10E: if(_tmp1DE == 0)goto _LL10B;_tmp1DF=*_tmp1DE;
_tmp1E0=(struct _tuple12*)_tmp1DF.hd;_tmp1E1=*_tmp1E0;_tmp1E2=_tmp1E1.f1;_tmp1E3=
_tmp1E1.f2;_tmp1E4=_tmp1DF.tl;_LL10F: return({struct Cyc_List_List*_tmp1E5=
_region_malloc(r,sizeof(*_tmp1E5));_tmp1E5->hd=({struct _tuple12*_tmp1E6=
_region_malloc(r,sizeof(*_tmp1E6));_tmp1E6->f1=_tmp1E2;_tmp1E6->f2=(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp1E7=_region_malloc(r,sizeof(*_tmp1E7));_tmp1E7->hd=(void*)
dsc;_tmp1E7->tl=_tmp1E3;_tmp1E7;});_tmp1E6;});_tmp1E5->tl=_tmp1E4;_tmp1E5;});
_LL10B:;}static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct _RegionHandle*r,
struct Cyc_List_List*ctxt){struct Cyc_List_List*_tmp1E8=ctxt;struct Cyc_List_List
_tmp1E9;struct _tuple12*_tmp1EA;struct _tuple12 _tmp1EB;struct Cyc_Tcpat_Con_s*
_tmp1EC;struct Cyc_List_List*_tmp1ED;struct Cyc_List_List*_tmp1EE;_LL111: if(
_tmp1E8 != 0)goto _LL113;_LL112:({void*_tmp1EF=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1F0="norm_context: empty context";
_tag_dyneither(_tmp1F0,sizeof(char),28);}),_tag_dyneither(_tmp1EF,sizeof(void*),
0));});_LL113: if(_tmp1E8 == 0)goto _LL110;_tmp1E9=*_tmp1E8;_tmp1EA=(struct _tuple12*)
_tmp1E9.hd;_tmp1EB=*_tmp1EA;_tmp1EC=_tmp1EB.f1;_tmp1ED=_tmp1EB.f2;_tmp1EE=
_tmp1E9.tl;_LL114: return Cyc_Tcpat_augment(r,_tmp1EE,(void*)({struct Cyc_Tcpat_Pos_struct*
_tmp1F1=_region_malloc(r,sizeof(*_tmp1F1));_tmp1F1[0]=({struct Cyc_Tcpat_Pos_struct
_tmp1F2;_tmp1F2.tag=0;_tmp1F2.f1=_tmp1EC;_tmp1F2.f2=Cyc_List_rrev(r,_tmp1ED);
_tmp1F2;});_tmp1F1;}));_LL110:;}static void*Cyc_Tcpat_build_desc(struct
_RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
struct _tuple6 _tmp1F4=({struct _tuple6 _tmp1F3;_tmp1F3.f1=ctxt;_tmp1F3.f2=work;
_tmp1F3;});struct Cyc_List_List*_tmp1F5;struct Cyc_List_List*_tmp1F6;struct Cyc_List_List*
_tmp1F7;struct Cyc_List_List*_tmp1F8;struct Cyc_List_List*_tmp1F9;struct Cyc_List_List
_tmp1FA;struct _tuple12*_tmp1FB;struct _tuple12 _tmp1FC;struct Cyc_Tcpat_Con_s*
_tmp1FD;struct Cyc_List_List*_tmp1FE;struct Cyc_List_List*_tmp1FF;struct Cyc_List_List*
_tmp200;struct Cyc_List_List _tmp201;struct _tuple11*_tmp202;struct _tuple11 _tmp203;
struct Cyc_List_List*_tmp204;struct Cyc_List_List*_tmp205;_LL116: _tmp1F5=_tmp1F4.f1;
if(_tmp1F5 != 0)goto _LL118;_tmp1F6=_tmp1F4.f2;if(_tmp1F6 != 0)goto _LL118;_LL117:
return dsc;_LL118: _tmp1F7=_tmp1F4.f1;if(_tmp1F7 != 0)goto _LL11A;_LL119: goto _LL11B;
_LL11A: _tmp1F8=_tmp1F4.f2;if(_tmp1F8 != 0)goto _LL11C;_LL11B:({void*_tmp206=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp207="build_desc: ctxt and work don't match";_tag_dyneither(_tmp207,
sizeof(char),38);}),_tag_dyneither(_tmp206,sizeof(void*),0));});_LL11C: _tmp1F9=
_tmp1F4.f1;if(_tmp1F9 == 0)goto _LL115;_tmp1FA=*_tmp1F9;_tmp1FB=(struct _tuple12*)
_tmp1FA.hd;_tmp1FC=*_tmp1FB;_tmp1FD=_tmp1FC.f1;_tmp1FE=_tmp1FC.f2;_tmp1FF=
_tmp1FA.tl;_tmp200=_tmp1F4.f2;if(_tmp200 == 0)goto _LL115;_tmp201=*_tmp200;_tmp202=(
struct _tuple11*)_tmp201.hd;_tmp203=*_tmp202;_tmp204=_tmp203.f3;_tmp205=_tmp201.tl;
_LL11D: {struct Cyc_Tcpat_Pos_struct*_tmp208=({struct Cyc_Tcpat_Pos_struct*_tmp209=
_region_malloc(r,sizeof(*_tmp209));_tmp209[0]=({struct Cyc_Tcpat_Pos_struct
_tmp20A;_tmp20A.tag=0;_tmp20A.f1=_tmp1FD;_tmp20A.f2=Cyc_List_rappend(r,Cyc_List_rrev(
r,_tmp1FE),({struct Cyc_List_List*_tmp20B=_region_malloc(r,sizeof(*_tmp20B));
_tmp20B->hd=(void*)dsc;_tmp20B->tl=_tmp204;_tmp20B;}));_tmp20A;});_tmp209;});
return Cyc_Tcpat_build_desc(r,_tmp1FF,(void*)_tmp208,_tmp205);}_LL115:;}static
void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*
dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct
Cyc_List_List*rules);static void*Cyc_Tcpat_or_match(struct _RegionHandle*r,void*
dsc,struct Cyc_List_List*allmrules){struct Cyc_List_List*_tmp20C=allmrules;struct
Cyc_List_List _tmp20D;struct _tuple0*_tmp20E;struct _tuple0 _tmp20F;void*_tmp210;
void*_tmp211;struct Cyc_List_List*_tmp212;_LL11F: if(_tmp20C != 0)goto _LL121;_LL120:
return(void*)({struct Cyc_Tcpat_Failure_struct*_tmp213=_cycalloc(sizeof(*_tmp213));
_tmp213[0]=({struct Cyc_Tcpat_Failure_struct _tmp214;_tmp214.tag=0;_tmp214.f1=(
void*)dsc;_tmp214;});_tmp213;});_LL121: if(_tmp20C == 0)goto _LL11E;_tmp20D=*
_tmp20C;_tmp20E=(struct _tuple0*)_tmp20D.hd;_tmp20F=*_tmp20E;_tmp210=_tmp20F.f1;
_tmp211=_tmp20F.f2;_tmp212=_tmp20D.tl;_LL122: return Cyc_Tcpat_match(r,_tmp210,0,
dsc,0,0,_tmp211,_tmp212);_LL11E:;}static void*Cyc_Tcpat_match_compile(struct
_RegionHandle*r,struct Cyc_List_List*allmrules){return Cyc_Tcpat_or_match(r,(void*)({
struct Cyc_Tcpat_Neg_struct*_tmp215=_region_malloc(r,sizeof(*_tmp215));_tmp215[0]=({
struct Cyc_Tcpat_Neg_struct _tmp216;_tmp216.tag=1;_tmp216.f1=Cyc_Tcpat_empty_con_set(
r);_tmp216;});_tmp215;}),allmrules);}static void*Cyc_Tcpat_and_match(struct
_RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*
right_hand_side,struct Cyc_List_List*rules){struct Cyc_List_List*_tmp217=work;
struct Cyc_List_List _tmp218;struct _tuple11*_tmp219;struct _tuple11 _tmp21A;struct
Cyc_List_List*_tmp21B;struct Cyc_List_List*_tmp21C;struct Cyc_List_List*_tmp21D;
struct Cyc_List_List*_tmp21E;struct Cyc_List_List _tmp21F;struct _tuple11*_tmp220;
struct _tuple11 _tmp221;struct Cyc_List_List*_tmp222;struct Cyc_List_List*_tmp223;
struct Cyc_List_List*_tmp224;struct Cyc_List_List*_tmp225;_LL124: if(_tmp217 != 0)
goto _LL126;_LL125: return(void*)({struct Cyc_Tcpat_Success_struct*_tmp226=
_region_malloc(r,sizeof(*_tmp226));_tmp226[0]=({struct Cyc_Tcpat_Success_struct
_tmp227;_tmp227.tag=1;_tmp227.f1=(void*)right_hand_side;_tmp227;});_tmp226;});
_LL126: if(_tmp217 == 0)goto _LL128;_tmp218=*_tmp217;_tmp219=(struct _tuple11*)
_tmp218.hd;_tmp21A=*_tmp219;_tmp21B=_tmp21A.f1;if(_tmp21B != 0)goto _LL128;_tmp21C=
_tmp21A.f2;if(_tmp21C != 0)goto _LL128;_tmp21D=_tmp21A.f3;if(_tmp21D != 0)goto
_LL128;_tmp21E=_tmp218.tl;_LL127: return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(
r,ctx),_tmp21E,right_hand_side,rules);_LL128: if(_tmp217 == 0)goto _LL123;_tmp21F=*
_tmp217;_tmp220=(struct _tuple11*)_tmp21F.hd;_tmp221=*_tmp220;_tmp222=_tmp221.f1;
_tmp223=_tmp221.f2;_tmp224=_tmp221.f3;_tmp225=_tmp21F.tl;_LL129: if((_tmp222 == 0
 || _tmp223 == 0) || _tmp224 == 0)({void*_tmp228=0;Cyc_Tcutil_impos(({const char*
_tmp229="tcpat:and_match: malformed work frame";_tag_dyneither(_tmp229,sizeof(
char),38);}),_tag_dyneither(_tmp228,sizeof(void*),0));});{struct Cyc_List_List
_tmp22B;void*_tmp22C;struct Cyc_List_List*_tmp22D;struct Cyc_List_List*_tmp22A=(
struct Cyc_List_List*)_tmp222;_tmp22B=*_tmp22A;_tmp22C=(void*)_tmp22B.hd;_tmp22D=
_tmp22B.tl;{struct Cyc_List_List _tmp22F;struct Cyc_List_List*_tmp230;struct Cyc_List_List*
_tmp231;struct Cyc_List_List*_tmp22E=(struct Cyc_List_List*)_tmp223;_tmp22F=*
_tmp22E;_tmp230=(struct Cyc_List_List*)_tmp22F.hd;_tmp231=_tmp22F.tl;{struct Cyc_List_List
_tmp233;void*_tmp234;struct Cyc_List_List*_tmp235;struct Cyc_List_List*_tmp232=(
struct Cyc_List_List*)_tmp224;_tmp233=*_tmp232;_tmp234=(void*)_tmp233.hd;_tmp235=
_tmp233.tl;{struct _tuple11*_tmp236=({struct _tuple11*_tmp238=_region_malloc(r,
sizeof(*_tmp238));_tmp238->f1=_tmp22D;_tmp238->f2=_tmp231;_tmp238->f3=_tmp235;
_tmp238;});return Cyc_Tcpat_match(r,_tmp22C,_tmp230,_tmp234,ctx,({struct Cyc_List_List*
_tmp237=_region_malloc(r,sizeof(*_tmp237));_tmp237->hd=_tmp236;_tmp237->tl=
_tmp225;_tmp237;}),right_hand_side,rules);}}}}_LL123:;}static struct Cyc_List_List*
Cyc_Tcpat_getdargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,void*dsc){
void*_tmp239=dsc;struct Cyc_Set_Set*_tmp23A;struct Cyc_List_List*_tmp23B;_LL12B:
if(*((int*)_tmp239)!= 1)goto _LL12D;_tmp23A=((struct Cyc_Tcpat_Neg_struct*)_tmp239)->f1;
_LL12C: {void*any=(void*)({struct Cyc_Tcpat_Neg_struct*_tmp23E=_region_malloc(r,
sizeof(*_tmp23E));_tmp23E[0]=({struct Cyc_Tcpat_Neg_struct _tmp23F;_tmp23F.tag=1;
_tmp23F.f1=Cyc_Tcpat_empty_con_set(r);_tmp23F;});_tmp23E;});struct Cyc_List_List*
_tmp23C=0;{int i=0;for(0;i < pcon->arity;++ i){_tmp23C=({struct Cyc_List_List*
_tmp23D=_region_malloc(r,sizeof(*_tmp23D));_tmp23D->hd=(void*)any;_tmp23D->tl=
_tmp23C;_tmp23D;});}}return _tmp23C;}_LL12D: if(*((int*)_tmp239)!= 0)goto _LL12A;
_tmp23B=((struct Cyc_Tcpat_Pos_struct*)_tmp239)->f2;_LL12E: return _tmp23B;_LL12A:;}
struct _tuple13{struct _RegionHandle*f1;struct Cyc_List_List*f2;};static struct Cyc_List_List*
Cyc_Tcpat_getoarg(struct _tuple13*env,int i){struct _tuple13 _tmp241;struct
_RegionHandle*_tmp242;struct Cyc_List_List*_tmp243;struct _tuple13*_tmp240=env;
_tmp241=*_tmp240;_tmp242=_tmp241.f1;_tmp243=_tmp241.f2;return({struct Cyc_List_List*
_tmp244=_region_malloc(_tmp242,sizeof(*_tmp244));_tmp244->hd=(void*)(i + 1);
_tmp244->tl=_tmp243;_tmp244;});}static struct Cyc_List_List*Cyc_Tcpat_getoargs(
struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){struct
_tuple13 _tmp245=({struct _tuple13 _tmp246;_tmp246.f1=r;_tmp246.f2=obj;_tmp246;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*
f)(struct _tuple13*,int),struct _tuple13*env))Cyc_List_rtabulate_c)(r,pcon->arity,
Cyc_Tcpat_getoarg,& _tmp245);}static void*Cyc_Tcpat_match(struct _RegionHandle*r,
void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*
work,void*right_hand_side,struct Cyc_List_List*rules){void*_tmp247=p;struct Cyc_Tcpat_Con_s*
_tmp248;struct Cyc_List_List*_tmp249;_LL130: if((int)_tmp247 != 0)goto _LL132;_LL131:
return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,
rules);_LL132: if(_tmp247 <= (void*)1)goto _LL12F;if(*((int*)_tmp247)!= 0)goto
_LL12F;_tmp248=((struct Cyc_Tcpat_Con_struct*)_tmp247)->f1;_tmp249=((struct Cyc_Tcpat_Con_struct*)
_tmp247)->f2;_LL133: {void*_tmp24A=Cyc_Tcpat_static_match(_tmp248,dsc);_LL135:
if((int)_tmp24A != 0)goto _LL137;_LL136: {struct Cyc_List_List*ctx2=({struct Cyc_List_List*
_tmp24D=_region_malloc(r,sizeof(*_tmp24D));_tmp24D->hd=({struct _tuple12*_tmp24E=
_region_malloc(r,sizeof(*_tmp24E));_tmp24E->f1=_tmp248;_tmp24E->f2=0;_tmp24E;});
_tmp24D->tl=ctx;_tmp24D;});struct _tuple11*work_frame=({struct _tuple11*_tmp24C=
_region_malloc(r,sizeof(*_tmp24C));_tmp24C->f1=_tmp249;_tmp24C->f2=Cyc_Tcpat_getoargs(
r,_tmp248,obj);_tmp24C->f3=Cyc_Tcpat_getdargs(r,_tmp248,dsc);_tmp24C;});struct
Cyc_List_List*work2=({struct Cyc_List_List*_tmp24B=_region_malloc(r,sizeof(*
_tmp24B));_tmp24B->hd=work_frame;_tmp24B->tl=work;_tmp24B;});return Cyc_Tcpat_and_match(
r,ctx2,work2,right_hand_side,rules);}_LL137: if((int)_tmp24A != 1)goto _LL139;
_LL138: return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);
_LL139: if((int)_tmp24A != 2)goto _LL134;_LL13A: {struct Cyc_List_List*ctx2=({struct
Cyc_List_List*_tmp255=_region_malloc(r,sizeof(*_tmp255));_tmp255->hd=({struct
_tuple12*_tmp256=_region_malloc(r,sizeof(*_tmp256));_tmp256->f1=_tmp248;_tmp256->f2=
0;_tmp256;});_tmp255->tl=ctx;_tmp255;});struct _tuple11*work_frame=({struct
_tuple11*_tmp254=_region_malloc(r,sizeof(*_tmp254));_tmp254->f1=_tmp249;_tmp254->f2=
Cyc_Tcpat_getoargs(r,_tmp248,obj);_tmp254->f3=Cyc_Tcpat_getdargs(r,_tmp248,dsc);
_tmp254;});struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp253=
_region_malloc(r,sizeof(*_tmp253));_tmp253->hd=work_frame;_tmp253->tl=work;
_tmp253;});void*_tmp24F=Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);
void*_tmp250=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,Cyc_Tcpat_add_neg(r,
dsc,_tmp248),work),rules);return(void*)({struct Cyc_Tcpat_IfEq_struct*_tmp251=
_region_malloc(r,sizeof(*_tmp251));_tmp251[0]=({struct Cyc_Tcpat_IfEq_struct
_tmp252;_tmp252.tag=2;_tmp252.f1=obj;_tmp252.f2=_tmp248;_tmp252.f3=(void*)
_tmp24F;_tmp252.f4=(void*)_tmp250;_tmp252;});_tmp251;});}_LL134:;}_LL12F:;}
static void Cyc_Tcpat_check_exhaust_overlap(struct _RegionHandle*r,void*d,void(*
not_exhaust)(struct _RegionHandle*,void*,void*),void*env1,void(*rhs_appears)(void*,
void*),void*env2,int*exhaust_done){void*_tmp257=d;void*_tmp258;void*_tmp259;void*
_tmp25A;void*_tmp25B;_LL13C: if(*((int*)_tmp257)!= 0)goto _LL13E;_tmp258=(void*)((
struct Cyc_Tcpat_Failure_struct*)_tmp257)->f1;_LL13D: if(!(*exhaust_done)){
not_exhaust(r,env1,_tmp258);*exhaust_done=1;}goto _LL13B;_LL13E: if(*((int*)
_tmp257)!= 1)goto _LL140;_tmp259=(void*)((struct Cyc_Tcpat_Success_struct*)_tmp257)->f1;
_LL13F: rhs_appears(env2,_tmp259);goto _LL13B;_LL140: if(*((int*)_tmp257)!= 2)goto
_LL13B;_tmp25A=(void*)((struct Cyc_Tcpat_IfEq_struct*)_tmp257)->f3;_tmp25B=(void*)((
struct Cyc_Tcpat_IfEq_struct*)_tmp257)->f4;_LL141: Cyc_Tcpat_check_exhaust_overlap(
r,_tmp25A,not_exhaust,env1,rhs_appears,env2,exhaust_done);Cyc_Tcpat_check_exhaust_overlap(
r,_tmp25B,not_exhaust,env1,rhs_appears,env2,exhaust_done);goto _LL13B;_LL13B:;}
struct _tuple14{int f1;struct Cyc_Position_Segment*f2;};struct _tuple15{void*f1;
struct _tuple14*f2;};struct _tuple16{struct _RegionHandle*f1;int*f2;};static struct
_tuple15*Cyc_Tcpat_get_match(struct _tuple16*env,struct Cyc_Absyn_Switch_clause*
swc){struct _tuple16 _tmp25D;struct _RegionHandle*_tmp25E;int*_tmp25F;struct
_tuple16*_tmp25C=env;_tmp25D=*_tmp25C;_tmp25E=_tmp25D.f1;_tmp25F=_tmp25D.f2;{
void*sp0=Cyc_Tcpat_compile_pat(_tmp25E,swc->pattern);struct _tuple14*rhs=({struct
_tuple14*_tmp265=_region_malloc(_tmp25E,sizeof(*_tmp265));_tmp265->f1=0;_tmp265->f2=(
swc->pattern)->loc;_tmp265;});void*sp;if(swc->where_clause != 0){sp=Cyc_Tcpat_tuple_pat(
_tmp25E,({struct Cyc_List_List*_tmp260=_region_malloc(_tmp25E,sizeof(*_tmp260));
_tmp260->hd=(void*)sp0;_tmp260->tl=({struct Cyc_List_List*_tmp261=_region_malloc(
_tmp25E,sizeof(*_tmp261));_tmp261->hd=(void*)Cyc_Tcpat_int_pat(_tmp25E,*_tmp25F,
0);_tmp261->tl=0;_tmp261;});_tmp260;}),0);*_tmp25F=*_tmp25F + 1;}else{sp=Cyc_Tcpat_tuple_pat(
_tmp25E,({struct Cyc_List_List*_tmp262=_region_malloc(_tmp25E,sizeof(*_tmp262));
_tmp262->hd=(void*)sp0;_tmp262->tl=({struct Cyc_List_List*_tmp263=_region_malloc(
_tmp25E,sizeof(*_tmp263));_tmp263->hd=(void*)((void*)0);_tmp263->tl=0;_tmp263;});
_tmp262;}),0);}return({struct _tuple15*_tmp264=_region_malloc(_tmp25E,sizeof(*
_tmp264));_tmp264->f1=sp;_tmp264->f2=rhs;_tmp264;});}}char Cyc_Tcpat_Desc2string[
16]="\000\000\000\000Desc2string\000";static struct _dyneither_ptr Cyc_Tcpat_descs2string(
struct _RegionHandle*r,struct Cyc_List_List*);static struct _dyneither_ptr Cyc_Tcpat_desc2string(
struct _RegionHandle*r,void*d){void*_tmp266=d;struct Cyc_Tcpat_Con_s*_tmp267;
struct Cyc_List_List*_tmp268;struct Cyc_Set_Set*_tmp269;_LL143: if(*((int*)_tmp266)
!= 0)goto _LL145;_tmp267=((struct Cyc_Tcpat_Pos_struct*)_tmp266)->f1;_tmp268=((
struct Cyc_Tcpat_Pos_struct*)_tmp266)->f2;_LL144: {union Cyc_Tcpat_Name_value_union
_tmp26A=_tmp267->name;struct Cyc_Absyn_Pat*_tmp26B=_tmp267->orig_pat;if(_tmp26B == 
0)return Cyc_Tcpat_desc2string(r,(void*)((struct Cyc_List_List*)_check_null(
_tmp268))->hd);{void*_tmp26C=(void*)_tmp26B->r;struct Cyc_Absyn_Vardecl*_tmp26D;
struct Cyc_Absyn_Vardecl*_tmp26E;struct Cyc_Absyn_Tvar*_tmp26F;struct Cyc_Absyn_Vardecl*
_tmp270;struct Cyc_Absyn_Pat*_tmp271;struct Cyc_Absyn_Pat _tmp272;void*_tmp273;
struct Cyc_Absyn_Tunionfield*_tmp274;struct Cyc_Absyn_AggrInfo _tmp275;union Cyc_Absyn_AggrInfoU_union
_tmp276;struct Cyc_Absyn_Aggrdecl**_tmp277;struct Cyc_Absyn_Aggrdecl*_tmp278;
struct Cyc_Absyn_Tunionfield*_tmp279;int _tmp27A;char _tmp27B;struct _dyneither_ptr
_tmp27C;struct Cyc_Absyn_Enumfield*_tmp27D;struct Cyc_Absyn_Enumfield*_tmp27E;
struct Cyc_Absyn_Exp*_tmp27F;_LL148: if((int)_tmp26C != 0)goto _LL14A;_LL149: return({
const char*_tmp280="_";_tag_dyneither(_tmp280,sizeof(char),2);});_LL14A: if(
_tmp26C <= (void*)2)goto _LL15A;if(*((int*)_tmp26C)!= 0)goto _LL14C;_tmp26D=((
struct Cyc_Absyn_Var_p_struct*)_tmp26C)->f1;_LL14B: return Cyc_Absynpp_qvar2string(
_tmp26D->name);_LL14C: if(*((int*)_tmp26C)!= 1)goto _LL14E;_tmp26E=((struct Cyc_Absyn_Reference_p_struct*)
_tmp26C)->f1;_LL14D: return(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp283;_tmp283.tag=0;_tmp283.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp26E->name));{void*_tmp281[1]={& _tmp283};Cyc_aprintf(({
const char*_tmp282="*%s";_tag_dyneither(_tmp282,sizeof(char),4);}),_tag_dyneither(
_tmp281,sizeof(void*),1));}});_LL14E: if(*((int*)_tmp26C)!= 2)goto _LL150;_tmp26F=((
struct Cyc_Absyn_TagInt_p_struct*)_tmp26C)->f1;_tmp270=((struct Cyc_Absyn_TagInt_p_struct*)
_tmp26C)->f2;_LL14F: return(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp287;_tmp287.tag=0;_tmp287.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*
_tmp26F->name);{struct Cyc_String_pa_struct _tmp286;_tmp286.tag=0;_tmp286.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp270->name));{
void*_tmp284[2]={& _tmp286,& _tmp287};Cyc_aprintf(({const char*_tmp285="%s<`%s>";
_tag_dyneither(_tmp285,sizeof(char),8);}),_tag_dyneither(_tmp284,sizeof(void*),2));}}});
_LL150: if(*((int*)_tmp26C)!= 3)goto _LL152;_LL151: return(struct _dyneither_ptr)({
struct Cyc_String_pa_struct _tmp28A;_tmp28A.tag=0;_tmp28A.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp268));{void*_tmp288[1]={&
_tmp28A};Cyc_aprintf(({const char*_tmp289="$(%s)";_tag_dyneither(_tmp289,sizeof(
char),6);}),_tag_dyneither(_tmp288,sizeof(void*),1));}});_LL152: if(*((int*)
_tmp26C)!= 4)goto _LL154;_tmp271=((struct Cyc_Absyn_Pointer_p_struct*)_tmp26C)->f1;
_tmp272=*_tmp271;_tmp273=(void*)_tmp272.r;if(_tmp273 <= (void*)2)goto _LL154;if(*((
int*)_tmp273)!= 6)goto _LL154;_tmp274=((struct Cyc_Absyn_Tunion_p_struct*)_tmp273)->f2;
_LL153: return(struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp28E;_tmp28E.tag=
0;_tmp28E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(
r,_tmp268));{struct Cyc_String_pa_struct _tmp28D;_tmp28D.tag=0;_tmp28D.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp274->name));{
void*_tmp28B[2]={& _tmp28D,& _tmp28E};Cyc_aprintf(({const char*_tmp28C="&%s(%s)";
_tag_dyneither(_tmp28C,sizeof(char),8);}),_tag_dyneither(_tmp28B,sizeof(void*),2));}}});
_LL154: if(*((int*)_tmp26C)!= 4)goto _LL156;_LL155: return(struct _dyneither_ptr)({
struct Cyc_String_pa_struct _tmp291;_tmp291.tag=0;_tmp291.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp268));{void*_tmp28F[1]={&
_tmp291};Cyc_aprintf(({const char*_tmp290="&%s";_tag_dyneither(_tmp290,sizeof(
char),4);}),_tag_dyneither(_tmp28F,sizeof(void*),1));}});_LL156: if(*((int*)
_tmp26C)!= 5)goto _LL158;_tmp275=((struct Cyc_Absyn_Aggr_p_struct*)_tmp26C)->f1;
_tmp276=_tmp275.aggr_info;if((((((struct Cyc_Absyn_Aggr_p_struct*)_tmp26C)->f1).aggr_info).KnownAggr).tag
!= 1)goto _LL158;_tmp277=(_tmp276.KnownAggr).f1;_tmp278=*_tmp277;_LL157: return(
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp295;_tmp295.tag=0;_tmp295.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp268));{
struct Cyc_String_pa_struct _tmp294;_tmp294.tag=0;_tmp294.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp278->name));{void*_tmp292[2]={&
_tmp294,& _tmp295};Cyc_aprintf(({const char*_tmp293="%s{%s}";_tag_dyneither(
_tmp293,sizeof(char),7);}),_tag_dyneither(_tmp292,sizeof(void*),2));}}});_LL158:
if(*((int*)_tmp26C)!= 6)goto _LL15A;_tmp279=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp26C)->f2;_LL159: return(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp298;_tmp298.tag=0;_tmp298.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp279->name));{void*_tmp296[1]={& _tmp298};Cyc_aprintf(({
const char*_tmp297="%s";_tag_dyneither(_tmp297,sizeof(char),3);}),_tag_dyneither(
_tmp296,sizeof(void*),1));}});_LL15A: if((int)_tmp26C != 1)goto _LL15C;_LL15B:
return({const char*_tmp299="NULL";_tag_dyneither(_tmp299,sizeof(char),5);});
_LL15C: if(_tmp26C <= (void*)2)goto _LL168;if(*((int*)_tmp26C)!= 7)goto _LL15E;
_tmp27A=((struct Cyc_Absyn_Int_p_struct*)_tmp26C)->f2;_LL15D: return(struct
_dyneither_ptr)({struct Cyc_Int_pa_struct _tmp29C;_tmp29C.tag=1;_tmp29C.f1=(
unsigned long)_tmp27A;{void*_tmp29A[1]={& _tmp29C};Cyc_aprintf(({const char*
_tmp29B="%d";_tag_dyneither(_tmp29B,sizeof(char),3);}),_tag_dyneither(_tmp29A,
sizeof(void*),1));}});_LL15E: if(*((int*)_tmp26C)!= 8)goto _LL160;_tmp27B=((struct
Cyc_Absyn_Char_p_struct*)_tmp26C)->f1;_LL15F: return(struct _dyneither_ptr)({
struct Cyc_Int_pa_struct _tmp29F;_tmp29F.tag=1;_tmp29F.f1=(unsigned long)((int)
_tmp27B);{void*_tmp29D[1]={& _tmp29F};Cyc_aprintf(({const char*_tmp29E="%d";
_tag_dyneither(_tmp29E,sizeof(char),3);}),_tag_dyneither(_tmp29D,sizeof(void*),1));}});
_LL160: if(*((int*)_tmp26C)!= 9)goto _LL162;_tmp27C=((struct Cyc_Absyn_Float_p_struct*)
_tmp26C)->f1;_LL161: return _tmp27C;_LL162: if(*((int*)_tmp26C)!= 10)goto _LL164;
_tmp27D=((struct Cyc_Absyn_Enum_p_struct*)_tmp26C)->f2;_LL163: _tmp27E=_tmp27D;
goto _LL165;_LL164: if(*((int*)_tmp26C)!= 11)goto _LL166;_tmp27E=((struct Cyc_Absyn_AnonEnum_p_struct*)
_tmp26C)->f2;_LL165: return Cyc_Absynpp_qvar2string(_tmp27E->name);_LL166: if(*((
int*)_tmp26C)!= 14)goto _LL168;_tmp27F=((struct Cyc_Absyn_Exp_p_struct*)_tmp26C)->f1;
_LL167: return Cyc_Absynpp_exp2string(_tmp27F);_LL168:;_LL169:(int)_throw((void*)
Cyc_Tcpat_Desc2string);_LL147:;}}_LL145: if(*((int*)_tmp266)!= 1)goto _LL142;
_tmp269=((struct Cyc_Tcpat_Neg_struct*)_tmp266)->f1;_LL146: if(((int(*)(struct Cyc_Set_Set*
s))Cyc_Set_is_empty)(_tmp269))return({const char*_tmp2A0="_";_tag_dyneither(
_tmp2A0,sizeof(char),2);});{struct Cyc_Tcpat_Con_s*_tmp2A1=((struct Cyc_Tcpat_Con_s*(*)(
struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp269);struct Cyc_Absyn_Pat*_tmp2A2=
_tmp2A1->orig_pat;if(_tmp2A2 == 0)(int)_throw((void*)Cyc_Tcpat_Desc2string);{void*
_tmp2A3=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp2A2->topt))->v);
void*_tmp2A4;struct Cyc_Absyn_PtrInfo _tmp2A5;struct Cyc_Absyn_PtrAtts _tmp2A6;
struct Cyc_Absyn_TunionInfo _tmp2A7;union Cyc_Absyn_TunionInfoU_union _tmp2A8;struct
Cyc_Absyn_Tuniondecl**_tmp2A9;struct Cyc_Absyn_Tuniondecl*_tmp2AA;_LL16B: if(
_tmp2A3 <= (void*)4)goto _LL173;if(*((int*)_tmp2A3)!= 5)goto _LL16D;_tmp2A4=(void*)((
struct Cyc_Absyn_IntType_struct*)_tmp2A3)->f2;if((int)_tmp2A4 != 0)goto _LL16D;
_LL16C:{int i=0;for(0;i < 256;++ i){struct Cyc_Tcpat_Con_s*_tmp2AB=Cyc_Tcpat_char_con(
r,(char)i,(struct Cyc_Absyn_Pat*)_tmp2A2);if(!((int(*)(struct Cyc_Set_Set*s,struct
Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp269,_tmp2AB))return(struct
_dyneither_ptr)({struct Cyc_Int_pa_struct _tmp2AE;_tmp2AE.tag=1;_tmp2AE.f1=(
unsigned long)i;{void*_tmp2AC[1]={& _tmp2AE};Cyc_aprintf(({const char*_tmp2AD="%d";
_tag_dyneither(_tmp2AD,sizeof(char),3);}),_tag_dyneither(_tmp2AC,sizeof(void*),1));}});}}(
int)_throw((void*)Cyc_Tcpat_Desc2string);_LL16D: if(*((int*)_tmp2A3)!= 5)goto
_LL16F;_LL16E:{unsigned int i=0;for(0;i < 0 - 1;++ i){struct Cyc_Tcpat_Con_s*_tmp2AF=
Cyc_Tcpat_int_con(r,(int)i,_tmp2A2);if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*
elt))Cyc_Set_member)(_tmp269,_tmp2AF))return(struct _dyneither_ptr)({struct Cyc_Int_pa_struct
_tmp2B2;_tmp2B2.tag=1;_tmp2B2.f1=(unsigned long)((int)i);{void*_tmp2B0[1]={&
_tmp2B2};Cyc_aprintf(({const char*_tmp2B1="%d";_tag_dyneither(_tmp2B1,sizeof(char),
3);}),_tag_dyneither(_tmp2B0,sizeof(void*),1));}});}}(int)_throw((void*)Cyc_Tcpat_Desc2string);
_LL16F: if(*((int*)_tmp2A3)!= 4)goto _LL171;_tmp2A5=((struct Cyc_Absyn_PointerType_struct*)
_tmp2A3)->f1;_tmp2A6=_tmp2A5.ptr_atts;_LL170: {struct Cyc_Absyn_Conref*_tmp2B3=
_tmp2A6.nullable;int is_nullable=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp2B3);if(is_nullable){if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*
elt))Cyc_Set_member)(_tmp269,Cyc_Tcpat_null_con(r,(struct Cyc_Absyn_Pat*)_tmp2A2)))
return({const char*_tmp2B4="NULL";_tag_dyneither(_tmp2B4,sizeof(char),5);});}
return({const char*_tmp2B5="&_";_tag_dyneither(_tmp2B5,sizeof(char),3);});}_LL171:
if(*((int*)_tmp2A3)!= 2)goto _LL173;_tmp2A7=((struct Cyc_Absyn_TunionType_struct*)
_tmp2A3)->f1;_tmp2A8=_tmp2A7.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp2A3)->f1).tunion_info).KnownTunion).tag != 1)goto _LL173;_tmp2A9=(_tmp2A8.KnownTunion).f1;
_tmp2AA=*_tmp2A9;_LL172: if(_tmp2AA->is_xtunion)(int)_throw((void*)Cyc_Tcpat_Desc2string);{
struct Cyc_List_List*_tmp2B6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp2AA->fields))->v;int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp2B6);for(0;(unsigned int)_tmp2B6;_tmp2B6=_tmp2B6->tl){struct _dyneither_ptr n=*(*((
struct Cyc_Absyn_Tunionfield*)_tmp2B6->hd)->name).f2;struct Cyc_List_List*_tmp2B7=((
struct Cyc_Absyn_Tunionfield*)_tmp2B6->hd)->typs;if(!((int(*)(struct Cyc_Set_Set*s,
struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp269,(struct Cyc_Tcpat_Con_s*)({
struct Cyc_Tcpat_Con_s*_tmp2B8=_cycalloc(sizeof(*_tmp2B8));_tmp2B8->name=(union
Cyc_Tcpat_Name_value_union)({union Cyc_Tcpat_Name_value_union _tmp2B9;(_tmp2B9.Name_v).tag=
0;(_tmp2B9.Name_v).f1=n;_tmp2B9;});_tmp2B8->arity=0;_tmp2B8->span=0;_tmp2B8->orig_pat=
_tmp2A2;_tmp2B8;}))){if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2B7)
== 0)return n;else{if(_tmp2AA->is_flat)return(struct _dyneither_ptr)({struct Cyc_String_pa_struct
_tmp2BC;_tmp2BC.tag=0;_tmp2BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n);{
void*_tmp2BA[1]={& _tmp2BC};Cyc_aprintf(({const char*_tmp2BB="%s(...)";
_tag_dyneither(_tmp2BB,sizeof(char),8);}),_tag_dyneither(_tmp2BA,sizeof(void*),1));}});
else{return(struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp2BF;_tmp2BF.tag=
0;_tmp2BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n);{void*_tmp2BD[1]={&
_tmp2BF};Cyc_aprintf(({const char*_tmp2BE="&%s(...)";_tag_dyneither(_tmp2BE,
sizeof(char),9);}),_tag_dyneither(_tmp2BD,sizeof(void*),1));}});}}}}(int)_throw((
void*)Cyc_Tcpat_Desc2string);}_LL173:;_LL174:(int)_throw((void*)Cyc_Tcpat_Desc2string);
_LL16A:;}}_LL142:;}static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(struct
_RegionHandle*r,void*d){return({struct _dyneither_ptr*_tmp2C0=_cycalloc(sizeof(*
_tmp2C0));_tmp2C0[0]=Cyc_Tcpat_desc2string(r,d);_tmp2C0;});}static struct
_dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*
ds){struct Cyc_List_List*_tmp2C1=((struct Cyc_List_List*(*)(struct _RegionHandle*,
struct _dyneither_ptr*(*f)(struct _RegionHandle*,void*),struct _RegionHandle*env,
struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_desc2stringptr,r,ds);struct
_dyneither_ptr*comma=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),",",
sizeof(char),2);{struct Cyc_List_List*_tmp2C2=_tmp2C1;for(0;_tmp2C2 != 0;_tmp2C2=((
struct Cyc_List_List*)_check_null(_tmp2C2))->tl){if(_tmp2C2->tl != 0){_tmp2C2->tl=(
struct Cyc_List_List*)({struct Cyc_List_List*_tmp2C3=_cycalloc(sizeof(*_tmp2C3));
_tmp2C3->hd=comma;_tmp2C3->tl=_tmp2C2->tl;_tmp2C3;});_tmp2C2=_tmp2C2->tl;}}}
return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp2C1);}static void Cyc_Tcpat_not_exhaust_err(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,void*desc){struct
_handler_cons _tmp2C5;_push_handler(& _tmp2C5);{int _tmp2C7=0;if(setjmp(_tmp2C5.handler))
_tmp2C7=1;if(!_tmp2C7){{struct _dyneither_ptr _tmp2C8=Cyc_Tcpat_desc2string(r,desc);({
struct Cyc_String_pa_struct _tmp2CB;_tmp2CB.tag=0;_tmp2CB.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp2C8);{void*_tmp2C9[1]={& _tmp2CB};Cyc_Tcutil_terr(loc,({
const char*_tmp2CA="patterns may not be exhaustive.\n\tmissing case for %s";
_tag_dyneither(_tmp2CA,sizeof(char),53);}),_tag_dyneither(_tmp2C9,sizeof(void*),
1));}});};_pop_handler();}else{void*_tmp2C6=(void*)_exn_thrown;void*_tmp2CD=
_tmp2C6;_LL176: if(_tmp2CD != Cyc_Tcpat_Desc2string)goto _LL178;_LL177:({void*
_tmp2CE=0;Cyc_Tcutil_terr(loc,({const char*_tmp2CF="patterns may not be exhaustive.";
_tag_dyneither(_tmp2CF,sizeof(char),32);}),_tag_dyneither(_tmp2CE,sizeof(void*),
0));});goto _LL175;_LL178:;_LL179:(void)_throw(_tmp2CD);_LL175:;}}}static void Cyc_Tcpat_rule_occurs(
int dummy,struct _tuple14*rhs){(*rhs).f1=1;}void Cyc_Tcpat_check_switch_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*swcs){struct _RegionHandle
_tmp2D0=_new_region("r");struct _RegionHandle*r=& _tmp2D0;_push_region(r);{int
_tmp2D1=0;struct _tuple16 _tmp2D2=({struct _tuple16 _tmp2DE;_tmp2DE.f1=r;_tmp2DE.f2=&
_tmp2D1;_tmp2DE;});struct Cyc_List_List*_tmp2D3=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple15*(*f)(struct _tuple16*,struct Cyc_Absyn_Switch_clause*),
struct _tuple16*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match,&
_tmp2D2,swcs);void*_tmp2D4=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*
allmrules))Cyc_Tcpat_match_compile)(r,_tmp2D3);int _tmp2D5=0;((void(*)(struct
_RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple14*),
int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp2D4,Cyc_Tcpat_not_exhaust_err,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp2D5);for(0;_tmp2D3 != 0;_tmp2D3=_tmp2D3->tl){
struct _tuple15 _tmp2D7;struct _tuple14*_tmp2D8;struct _tuple14 _tmp2D9;int _tmp2DA;
struct Cyc_Position_Segment*_tmp2DB;struct _tuple15*_tmp2D6=(struct _tuple15*)
_tmp2D3->hd;_tmp2D7=*_tmp2D6;_tmp2D8=_tmp2D7.f2;_tmp2D9=*_tmp2D8;_tmp2DA=_tmp2D9.f1;
_tmp2DB=_tmp2D9.f2;if(!_tmp2DA)({void*_tmp2DC=0;Cyc_Tcutil_terr(_tmp2DB,({const
char*_tmp2DD="redundant pattern";_tag_dyneither(_tmp2DD,sizeof(char),18);}),
_tag_dyneither(_tmp2DC,sizeof(void*),0));});}};_pop_region(r);}struct _tuple17{
struct Cyc_Position_Segment*f1;int f2;};static void Cyc_Tcpat_not_exhaust_warn(
struct _RegionHandle*r,struct _tuple17*pr,void*desc){(*pr).f2=0;{struct
_handler_cons _tmp2DF;_push_handler(& _tmp2DF);{int _tmp2E1=0;if(setjmp(_tmp2DF.handler))
_tmp2E1=1;if(!_tmp2E1){{struct _dyneither_ptr _tmp2E2=Cyc_Tcpat_desc2string(r,desc);({
struct Cyc_String_pa_struct _tmp2E5;_tmp2E5.tag=0;_tmp2E5.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)_tmp2E2);{void*_tmp2E3[1]={& _tmp2E5};Cyc_Tcutil_warn((*pr).f1,({
const char*_tmp2E4="pattern not exhaustive.\n\tmissing case for %s";
_tag_dyneither(_tmp2E4,sizeof(char),45);}),_tag_dyneither(_tmp2E3,sizeof(void*),
1));}});};_pop_handler();}else{void*_tmp2E0=(void*)_exn_thrown;void*_tmp2E7=
_tmp2E0;_LL17B: if(_tmp2E7 != Cyc_Tcpat_Desc2string)goto _LL17D;_LL17C:({void*
_tmp2E8=0;Cyc_Tcutil_warn((*pr).f1,({const char*_tmp2E9="pattern not exhaustive.";
_tag_dyneither(_tmp2E9,sizeof(char),24);}),_tag_dyneither(_tmp2E8,sizeof(void*),
0));});goto _LL17A;_LL17D:;_LL17E:(void)_throw(_tmp2E7);_LL17A:;}}}}static void Cyc_Tcpat_dummy_fn(
int i,int j){return;}struct _tuple18{void*f1;int f2;};int Cyc_Tcpat_check_let_pat_exhaustive(
struct Cyc_Position_Segment*loc,struct Cyc_Absyn_Pat*p){struct _RegionHandle _tmp2EA=
_new_region("r");struct _RegionHandle*r=& _tmp2EA;_push_region(r);{struct Cyc_List_List*
_tmp2EB=({struct Cyc_List_List*_tmp2F1=_region_malloc(r,sizeof(*_tmp2F1));_tmp2F1->hd=({
struct _tuple18*_tmp2F2=_region_malloc(r,sizeof(*_tmp2F2));_tmp2F2->f1=Cyc_Tcpat_compile_pat(
r,p);_tmp2F2->f2=0;_tmp2F2;});_tmp2F1->tl=0;_tmp2F1;});void*_tmp2EC=((void*(*)(
struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,
_tmp2EB);struct _tuple17 _tmp2ED=({struct _tuple17 _tmp2F0;_tmp2F0.f1=loc;_tmp2F0.f2=
1;_tmp2F0;});int _tmp2EE=0;((void(*)(struct _RegionHandle*r,void*d,void(*
not_exhaust)(struct _RegionHandle*,struct _tuple17*,void*),struct _tuple17*env1,
void(*rhs_appears)(int,int),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(
r,_tmp2EC,Cyc_Tcpat_not_exhaust_warn,& _tmp2ED,Cyc_Tcpat_dummy_fn,0,& _tmp2EE);{
int _tmp2EF=_tmp2ED.f2;_npop_handler(0);return _tmp2EF;}};_pop_region(r);}static
struct _tuple15*Cyc_Tcpat_get_match2(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
swc){void*sp0=Cyc_Tcpat_compile_pat(r,swc->pattern);struct _tuple14*rhs=({struct
_tuple14*_tmp2F4=_region_malloc(r,sizeof(*_tmp2F4));_tmp2F4->f1=0;_tmp2F4->f2=(
swc->pattern)->loc;_tmp2F4;});return({struct _tuple15*_tmp2F3=_region_malloc(r,
sizeof(*_tmp2F3));_tmp2F3->f1=sp0;_tmp2F3->f2=rhs;_tmp2F3;});}static void Cyc_Tcpat_not_exhaust_err2(
struct _RegionHandle*r,struct Cyc_Position_Segment*loc,void*d){;}void Cyc_Tcpat_check_catch_overlap(
struct Cyc_Position_Segment*loc,struct Cyc_List_List*swcs){struct _RegionHandle
_tmp2F5=_new_region("r");struct _RegionHandle*r=& _tmp2F5;_push_region(r);{struct
Cyc_List_List*_tmp2F6=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct
_tuple15*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct
_RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match2,
r,swcs);void*_tmp2F7=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*
allmrules))Cyc_Tcpat_match_compile)(r,_tmp2F6);int _tmp2F8=0;((void(*)(struct
_RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct Cyc_Position_Segment*,
void*),struct Cyc_Position_Segment*env1,void(*rhs_appears)(int,struct _tuple14*),
int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp2F7,Cyc_Tcpat_not_exhaust_err2,
loc,Cyc_Tcpat_rule_occurs,0,& _tmp2F8);for(0;_tmp2F6 != 0;_tmp2F6=_tmp2F6->tl){
struct _tuple15 _tmp2FA;struct _tuple14*_tmp2FB;struct _tuple14 _tmp2FC;int _tmp2FD;
struct Cyc_Position_Segment*_tmp2FE;struct _tuple15*_tmp2F9=(struct _tuple15*)
_tmp2F6->hd;_tmp2FA=*_tmp2F9;_tmp2FB=_tmp2FA.f2;_tmp2FC=*_tmp2FB;_tmp2FD=_tmp2FC.f1;
_tmp2FE=_tmp2FC.f2;if(!_tmp2FD)({void*_tmp2FF=0;Cyc_Tcutil_terr(_tmp2FE,({const
char*_tmp300="redundant pattern";_tag_dyneither(_tmp300,sizeof(char),18);}),
_tag_dyneither(_tmp2FF,sizeof(void*),0));});}};_pop_region(r);}
