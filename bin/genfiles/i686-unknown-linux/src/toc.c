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
void*v;};extern char Cyc_Core_Invalid_argument[21];struct Cyc_Core_Invalid_argument_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[12];struct Cyc_Core_Failure_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[15];struct Cyc_Core_Impossible_struct{
char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[14];extern char Cyc_Core_Unreachable[
16];struct Cyc_Core_Unreachable_struct{char*tag;struct _dyneither_ptr f1;};extern
struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_Core_NewRegion Cyc_Core_new_dynregion();
extern char Cyc_Core_Open_Region[16];extern char Cyc_Core_Free_Region[16];void Cyc_Core_free_dynregion(
struct _DynRegionHandle*);typedef struct{int __count;union{unsigned int __wch;char
__wchb[4];}__value;}Cyc___mbstate_t;typedef struct{long __pos;Cyc___mbstate_t
__state;}Cyc__G_fpos_t;typedef Cyc__G_fpos_t Cyc_fpos_t;struct Cyc___cycFILE;extern
struct Cyc___cycFILE*Cyc_stderr;struct Cyc_Cstdio___abstractFILE;struct Cyc_String_pa_struct{
int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_struct{int tag;unsigned long f1;}
;struct Cyc_Double_pa_struct{int tag;double f1;};struct Cyc_LongDouble_pa_struct{int
tag;long double f1;};struct Cyc_ShortPtr_pa_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_struct{
int tag;unsigned long*f1;};struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,
struct _dyneither_ptr);int Cyc_fflush(struct Cyc___cycFILE*);int Cyc_fprintf(struct
Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_struct{
int tag;short*f1;};struct Cyc_UShortPtr_sa_struct{int tag;unsigned short*f1;};
struct Cyc_IntPtr_sa_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_struct{int tag;
unsigned int*f1;};struct Cyc_StringPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
struct Cyc_DoublePtr_sa_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_struct{
int tag;float*f1;};struct Cyc_CharPtr_sa_struct{int tag;struct _dyneither_ptr f1;};
int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
extern char Cyc_FileCloseError[19];extern char Cyc_FileOpenError[18];struct Cyc_FileOpenError_struct{
char*tag;struct _dyneither_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*
tl;};struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);int Cyc_List_length(
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
struct _dyneither_ptr s1,struct _dyneither_ptr s2);struct _dyneither_ptr Cyc_strconcat(
struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_Iter_Iter{void*env;int(*
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
_dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};extern
char Cyc_Position_Exit[9];struct Cyc_Position_Segment;struct _dyneither_ptr Cyc_Position_string_of_segment(
struct Cyc_Position_Segment*);struct Cyc_Position_Error{struct _dyneither_ptr source;
struct Cyc_Position_Segment*seg;void*kind;struct _dyneither_ptr desc;};extern char
Cyc_Position_Nocontext[14];struct Cyc_Xarray_Xarray{struct _RegionHandle*r;struct
_dyneither_ptr elmts;int num_elmts;};int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(
struct _RegionHandle*);int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
struct Cyc_Absyn_Loc_n_struct{int tag;};struct Cyc_Absyn_Rel_n_struct{int tag;struct
Cyc_List_List*f1;};struct Cyc_Absyn_Abs_n_struct{int tag;struct Cyc_List_List*f1;};
union Cyc_Absyn_Nmspace_union{struct Cyc_Absyn_Loc_n_struct Loc_n;struct Cyc_Absyn_Rel_n_struct
Rel_n;struct Cyc_Absyn_Abs_n_struct Abs_n;};struct _tuple1{union Cyc_Absyn_Nmspace_union
f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Conref;struct Cyc_Absyn_Tqual{int
print_const;int q_volatile;int q_restrict;int real_const;struct Cyc_Position_Segment*
loc;};struct Cyc_Absyn_Eq_constr_struct{int tag;void*f1;};struct Cyc_Absyn_Forward_constr_struct{
int tag;struct Cyc_Absyn_Conref*f1;};struct Cyc_Absyn_No_constr_struct{int tag;};
union Cyc_Absyn_Constraint_union{struct Cyc_Absyn_Eq_constr_struct Eq_constr;struct
Cyc_Absyn_Forward_constr_struct Forward_constr;struct Cyc_Absyn_No_constr_struct
No_constr;};struct Cyc_Absyn_Conref{union Cyc_Absyn_Constraint_union v;};struct Cyc_Absyn_Eq_kb_struct{
int tag;void*f1;};struct Cyc_Absyn_Unknown_kb_struct{int tag;struct Cyc_Core_Opt*f1;
};struct Cyc_Absyn_Less_kb_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;};struct
Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_Upper_b_struct{
int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{struct Cyc_Position_Segment*
ptr_loc;struct Cyc_Position_Segment*rgn_loc;struct Cyc_Position_Segment*zt_loc;};
struct Cyc_Absyn_PtrAtts{void*rgn;struct Cyc_Absyn_Conref*nullable;struct Cyc_Absyn_Conref*
bounds;struct Cyc_Absyn_Conref*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct
Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts
ptr_atts;};struct Cyc_Absyn_VarargInfo{struct Cyc_Core_Opt*name;struct Cyc_Absyn_Tqual
tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;struct
Cyc_Core_Opt*effect;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*
cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct
Cyc_Absyn_UnknownTunionInfo{struct _tuple1*name;int is_xtunion;int is_flat;};struct
Cyc_Absyn_UnknownTunion_struct{int tag;struct Cyc_Absyn_UnknownTunionInfo f1;};
struct Cyc_Absyn_KnownTunion_struct{int tag;struct Cyc_Absyn_Tuniondecl**f1;};union
Cyc_Absyn_TunionInfoU_union{struct Cyc_Absyn_UnknownTunion_struct UnknownTunion;
struct Cyc_Absyn_KnownTunion_struct KnownTunion;};struct Cyc_Absyn_TunionInfo{union
Cyc_Absyn_TunionInfoU_union tunion_info;struct Cyc_List_List*targs;struct Cyc_Core_Opt*
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
_dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[15];int Cyc_Absyn_qvar_cmp(
struct _tuple1*,struct _tuple1*);struct Cyc_Absyn_Conref*Cyc_Absyn_compress_conref(
struct Cyc_Absyn_Conref*x);void*Cyc_Absyn_conref_def(void*,struct Cyc_Absyn_Conref*
x);extern struct Cyc_Absyn_Conref*Cyc_Absyn_true_conref;extern struct Cyc_Absyn_Conref*
Cyc_Absyn_false_conref;extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_exn_typ;extern void*Cyc_Absyn_bounds_one;
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*
zero_term);void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);void*Cyc_Absyn_dyneither_typ(
void*t,void*rgn,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Conref*zero_term);void*
Cyc_Absyn_void_star_typ();void*Cyc_Absyn_strct(struct _dyneither_ptr*name);void*
Cyc_Absyn_strctq(struct _tuple1*name);void*Cyc_Absyn_unionq_typ(struct _tuple1*
name);void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*
num_elts,struct Cyc_Absyn_Conref*zero_term,struct Cyc_Position_Segment*ztloc);
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(
union Cyc_Absyn_Cnst_union,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(void*,int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,
struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,struct
Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct
_dyneither_ptr f,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(
struct _dyneither_ptr s,struct Cyc_Position_Segment*);struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(
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
struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,struct Cyc_Position_Segment*);struct
Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,
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
Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,struct Cyc_Position_Segment*
loc);struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*
e2,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*
r,struct Cyc_Position_Segment*loc);struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(
struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);int Cyc_Absyn_is_lvalue(struct
Cyc_Absyn_Exp*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,
struct _dyneither_ptr*);struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(
struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _dyneither_ptr*Cyc_Absyn_fieldname(
int);struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU_union
info);extern int Cyc_Absyn_no_regions;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct
Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs: 1;int qvar_to_Cids: 1;int
add_cyc_prefix: 1;int to_VC: 1;int decls_first: 1;int rewrite_temp_tvars: 1;int
print_all_tvars: 1;int print_all_kinds: 1;int print_all_effects: 1;int
print_using_stmts: 1;int print_externC_stmts: 1;int print_full_evars: 1;int
print_zeroterm: 1;int generate_line_directives: 1;int use_curr_namespace: 1;struct Cyc_List_List*
curr_namespace;};struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct
_dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct _dyneither_ptr
Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);struct _dyneither_ptr Cyc_Absynpp_qvar2string(
struct _tuple1*);struct Cyc_RgnOrder_RgnPO;struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(
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
struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};void*Cyc_Tcutil_impos(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_terr(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void Cyc_Tcutil_warn(struct Cyc_Position_Segment*,
struct _dyneither_ptr fmt,struct _dyneither_ptr ap);void*Cyc_Tcutil_tvar_kind(struct
Cyc_Absyn_Tvar*t);void*Cyc_Tcutil_typ_kind(void*t);void*Cyc_Tcutil_compress(void*
t);int Cyc_Tcutil_is_pointer_type(void*t);int Cyc_Tcutil_is_pointer_or_boxed(void*
t,int*is_dyneither_ptr);int Cyc_Tcutil_unify(void*,void*);struct Cyc_List_List*Cyc_Tcutil_resolve_struct_designators(
struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,struct Cyc_List_List*des,
struct Cyc_List_List*fields);int Cyc_Tcutil_is_tagged_pointer_typ(void*);int Cyc_Tcutil_is_tagged_pointer_typ_elt(
void*t,void**elt_typ_dest);int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**
elt_typ_dest);int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**
ptr_type,int*is_dyneither,void**elt_type);struct _tuple4{struct Cyc_Absyn_Tqual f1;
void*f2;};void*Cyc_Tcutil_snd_tqt(struct _tuple4*);struct _tuple5{unsigned int f1;
int f2;};struct _tuple5 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);int Cyc_Evexp_c_can_eval(
struct Cyc_Absyn_Exp*e);int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2);struct Cyc_CfFlowInfo_VarRoot_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_MallocPt_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;
};struct Cyc_CfFlowInfo_InitParam_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Place{
void*root;struct Cyc_List_List*fields;};struct Cyc_CfFlowInfo_EqualConst_struct{
int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessVar_struct{int tag;struct Cyc_Absyn_Vardecl*
f1;};struct Cyc_CfFlowInfo_LessNumelts_struct{int tag;struct Cyc_Absyn_Vardecl*f1;}
;struct Cyc_CfFlowInfo_LessConst_struct{int tag;unsigned int f1;};struct Cyc_CfFlowInfo_LessEqNumelts_struct{
int tag;struct Cyc_Absyn_Vardecl*f1;};union Cyc_CfFlowInfo_RelnOp_union{struct Cyc_CfFlowInfo_EqualConst_struct
EqualConst;struct Cyc_CfFlowInfo_LessVar_struct LessVar;struct Cyc_CfFlowInfo_LessNumelts_struct
LessNumelts;struct Cyc_CfFlowInfo_LessConst_struct LessConst;struct Cyc_CfFlowInfo_LessEqNumelts_struct
LessEqNumelts;};struct Cyc_CfFlowInfo_Reln{struct Cyc_Absyn_Vardecl*vd;union Cyc_CfFlowInfo_RelnOp_union
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
Cyc_CfFlowInfo_Aggregate_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_CfFlowInfo_ConsumeInfo{
struct Cyc_Dict_Dict consumed;struct Cyc_List_List*may_consume;};struct Cyc_CfFlowInfo_BottomFL_struct{
int tag;};struct Cyc_CfFlowInfo_ReachableFL_struct{int tag;struct Cyc_Dict_Dict f1;
struct Cyc_List_List*f2;struct Cyc_CfFlowInfo_ConsumeInfo f3;};union Cyc_CfFlowInfo_FlowInfo_union{
struct Cyc_CfFlowInfo_BottomFL_struct BottomFL;struct Cyc_CfFlowInfo_ReachableFL_struct
ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*
unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*
esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*
dummy_place;};struct Cyc_CfFlowInfo_Region_k_struct{int tag;struct Cyc_Absyn_Tvar*
f1;};struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);struct _tuple1*Cyc_Toc_temp_var();
extern struct _dyneither_ptr Cyc_Toc_globals;extern int Cyc_noexpand_r;int Cyc_Toc_warn_bounds_checks=
0;int Cyc_Toc_warn_all_null_deref=0;unsigned int Cyc_Toc_total_bounds_checks=0;
unsigned int Cyc_Toc_bounds_checks_eliminated=0;static struct Cyc_List_List*Cyc_Toc_result_decls=
0;struct Cyc_Toc_TocState{struct _DynRegionHandle*dyn;struct Cyc_List_List**
tuple_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_Set_Set**tunions_so_far;
struct Cyc_Dict_Dict*xtunions_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*
temp_labels;};static struct Cyc_Toc_TocState*Cyc_Toc_toc_state=0;struct _tuple6{
struct _tuple1*f1;struct _dyneither_ptr f2;};int Cyc_Toc_qvar_tag_cmp(struct _tuple6*
x,struct _tuple6*y){struct _tuple1*_tmp1;struct _dyneither_ptr _tmp2;struct _tuple6
_tmp0=*x;_tmp1=_tmp0.f1;_tmp2=_tmp0.f2;{struct _tuple1*_tmp4;struct _dyneither_ptr
_tmp5;struct _tuple6 _tmp3=*y;_tmp4=_tmp3.f1;_tmp5=_tmp3.f2;{int i=Cyc_Absyn_qvar_cmp(
_tmp1,_tmp4);if(i != 0)return i;return Cyc_strcmp((struct _dyneither_ptr)_tmp2,(
struct _dyneither_ptr)_tmp5);}}}struct _tuple7{struct Cyc_Absyn_Aggrdecl*f1;void*f2;
};void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
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
struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_vfprintf(Cyc_stderr,fmt,ap);({
void*_tmpF=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp10="\n";_tag_dyneither(
_tmp10,sizeof(char),2);}),_tag_dyneither(_tmpF,sizeof(void*),0));});Cyc_fflush((
struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)Cyc_Toc_Toc_Unimplemented);}
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){Cyc_vfprintf(
Cyc_stderr,fmt,ap);({void*_tmp11=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp12="\n";
_tag_dyneither(_tmp12,sizeof(char),2);}),_tag_dyneither(_tmp11,sizeof(void*),0));});
Cyc_fflush((struct Cyc___cycFILE*)Cyc_stderr);(int)_throw((void*)Cyc_Toc_Toc_Impossible);}
char Cyc_Toc_Match_error[16]="\000\000\000\000Match_error\000";static char _tmp13[5]="curr";
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp13,_tmp13,_tmp13 + 5};static
struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp14[4]="tag";
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp14,_tmp14,_tmp14 + 4};static
struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp15[4]="val";
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp15,_tmp15,_tmp15 + 4};static
struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp16[14]="_handler_cons";
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp16,_tmp16,_tmp16 + 14};
static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;
static char _tmp17[8]="handler";static struct _dyneither_ptr Cyc_Toc_handler_string={
_tmp17,_tmp17,_tmp17 + 8};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;
static char _tmp18[14]="_RegionHandle";static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={
_tmp18,_tmp18,_tmp18 + 14};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=&
Cyc_Toc__RegionHandle_string;static char _tmp19[17]="_DynRegionHandle";static
struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={_tmp19,_tmp19,_tmp19 + 17};
static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;
static char _tmp1A[16]="_DynRegionFrame";static struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={
_tmp1A,_tmp1A,_tmp1A + 16};static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=&
Cyc_Toc__DynRegionFrame_string;struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(
void*)0,(void*)(0 + 0)};static char _tmp1B[7]="_throw";static struct _dyneither_ptr
Cyc_Toc__throw_str={_tmp1B,_tmp1B,_tmp1B + 7};static struct _tuple1 Cyc_Toc__throw_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__throw_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(void*)((
void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)((void*)& Cyc_Toc__throw_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=&
Cyc_Toc__throw_ev;static char _tmp1E[7]="setjmp";static struct _dyneither_ptr Cyc_Toc_setjmp_str={
_tmp1E,_tmp1E,_tmp1E + 7};static struct _tuple1 Cyc_Toc_setjmp_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc_setjmp_ev={0,(void*)((void*)& Cyc_Toc_setjmp_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp21[
14]="_push_handler";static struct _dyneither_ptr Cyc_Toc__push_handler_str={_tmp21,
_tmp21,_tmp21 + 14};static struct _tuple1 Cyc_Toc__push_handler_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)((void*)& Cyc_Toc__push_handler_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=&
Cyc_Toc__push_handler_ev;static char _tmp24[13]="_pop_handler";static struct
_dyneither_ptr Cyc_Toc__pop_handler_str={_tmp24,_tmp24,_tmp24 + 13};static struct
_tuple1 Cyc_Toc__pop_handler_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_handler_re={
1,& Cyc_Toc__pop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={
0,(void*)((void*)& Cyc_Toc__pop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static
char _tmp27[12]="_exn_thrown";static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={
_tmp27,_tmp27,_tmp27 + 12};static struct _tuple1 Cyc_Toc__exn_thrown_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__exn_thrown_re={1,& Cyc_Toc__exn_thrown_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)((void*)& Cyc_Toc__exn_thrown_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=&
Cyc_Toc__exn_thrown_ev;static char _tmp2A[14]="_npop_handler";static struct
_dyneither_ptr Cyc_Toc__npop_handler_str={_tmp2A,_tmp2A,_tmp2A + 14};static struct
_tuple1 Cyc_Toc__npop_handler_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__npop_handler_re={
1,& Cyc_Toc__npop_handler_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={
0,(void*)((void*)& Cyc_Toc__npop_handler_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;
static char _tmp2D[12]="_check_null";static struct _dyneither_ptr Cyc_Toc__check_null_str={
_tmp2D,_tmp2D,_tmp2D + 12};static struct _tuple1 Cyc_Toc__check_null_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)((void*)& Cyc_Toc__check_null_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=&
Cyc_Toc__check_null_ev;static char _tmp30[28]="_check_known_subscript_null";static
struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp30,_tmp30,
_tmp30 + 28};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_known_subscript_null_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={
0,(void*)((void*)& Cyc_Toc__check_known_subscript_null_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;
static char _tmp33[31]="_check_known_subscript_notnull";static struct _dyneither_ptr
Cyc_Toc__check_known_subscript_notnull_str={_tmp33,_tmp33,_tmp33 + 31};static
struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_known_subscript_notnull_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_known_subscript_notnull_re={1,&
Cyc_Toc__check_known_subscript_notnull_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)((void*)& Cyc_Toc__check_known_subscript_notnull_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=&
Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp36[27]="_check_dyneither_subscript";
static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={_tmp36,_tmp36,
_tmp36 + 27};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_dyneither_subscript_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__check_dyneither_subscript_re={1,& Cyc_Toc__check_dyneither_subscript_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={
0,(void*)((void*)& Cyc_Toc__check_dyneither_subscript_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;
static char _tmp39[15]="_dyneither_ptr";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={
_tmp39,_tmp39,_tmp39 + 15};static struct _tuple1 Cyc_Toc__dyneither_ptr_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_re={1,& Cyc_Toc__dyneither_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={0,(void*)((
void*)& Cyc_Toc__dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;static
char _tmp3C[15]="_tag_dyneither";static struct _dyneither_ptr Cyc_Toc__tag_dyneither_str={
_tmp3C,_tmp3C,_tmp3C + 15};static struct _tuple1 Cyc_Toc__tag_dyneither_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__tag_dyneither_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__tag_dyneither_re={1,& Cyc_Toc__tag_dyneither_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={0,(void*)((
void*)& Cyc_Toc__tag_dyneither_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static
char _tmp3F[20]="_init_dyneither_ptr";static struct _dyneither_ptr Cyc_Toc__init_dyneither_ptr_str={
_tmp3F,_tmp3F,_tmp3F + 20};static struct _tuple1 Cyc_Toc__init_dyneither_ptr_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__init_dyneither_ptr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__init_dyneither_ptr_re={1,& Cyc_Toc__init_dyneither_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(
void*)((void*)& Cyc_Toc__init_dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=& Cyc_Toc__init_dyneither_ptr_ev;
static char _tmp42[21]="_untag_dyneither_ptr";static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={
_tmp42,_tmp42,_tmp42 + 21};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__untag_dyneither_ptr_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__untag_dyneither_ptr_re={1,& Cyc_Toc__untag_dyneither_ptr_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(
void*)((void*)& Cyc_Toc__untag_dyneither_ptr_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;
static char _tmp45[20]="_get_dyneither_size";static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={
_tmp45,_tmp45,_tmp45 + 20};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_dyneither_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_dyneither_size_re={1,& Cyc_Toc__get_dyneither_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(
void*)((void*)& Cyc_Toc__get_dyneither_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;
static char _tmp48[19]="_get_zero_arr_size";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={
_tmp48,_tmp48,_tmp48 + 19};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_re={1,& Cyc_Toc__get_zero_arr_size_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(
void*)((void*)& Cyc_Toc__get_zero_arr_size_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;
static char _tmp4B[24]="_get_zero_arr_size_char";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={
_tmp4B,_tmp4B,_tmp4B + 24};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_char_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_char_re={1,& Cyc_Toc__get_zero_arr_size_char_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(
void*)((void*)& Cyc_Toc__get_zero_arr_size_char_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;
static char _tmp4E[25]="_get_zero_arr_size_short";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={
_tmp4E,_tmp4E,_tmp4E + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_short_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_short_re={1,& Cyc_Toc__get_zero_arr_size_short_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={
0,(void*)((void*)& Cyc_Toc__get_zero_arr_size_short_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;
static char _tmp51[23]="_get_zero_arr_size_int";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={
_tmp51,_tmp51,_tmp51 + 23};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_int_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_int_re={1,& Cyc_Toc__get_zero_arr_size_int_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(
void*)((void*)& Cyc_Toc__get_zero_arr_size_int_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;
static char _tmp54[25]="_get_zero_arr_size_float";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={
_tmp54,_tmp54,_tmp54 + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_float_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_float_re={1,& Cyc_Toc__get_zero_arr_size_float_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={
0,(void*)((void*)& Cyc_Toc__get_zero_arr_size_float_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;
static char _tmp57[26]="_get_zero_arr_size_double";static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={
_tmp57,_tmp57,_tmp57 + 26};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_double_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_double_re={1,& Cyc_Toc__get_zero_arr_size_double_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={
0,(void*)((void*)& Cyc_Toc__get_zero_arr_size_double_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;
static char _tmp5A[30]="_get_zero_arr_size_longdouble";static struct _dyneither_ptr
Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp5A,_tmp5A,_tmp5A + 30};static
struct _tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_longdouble_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1,&
Cyc_Toc__get_zero_arr_size_longdouble_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)((void*)& Cyc_Toc__get_zero_arr_size_longdouble_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=&
Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp5D[28]="_get_zero_arr_size_voidstar";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp5D,_tmp5D,
_tmp5D + 28};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__get_zero_arr_size_voidstar_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1,& Cyc_Toc__get_zero_arr_size_voidstar_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={
0,(void*)((void*)& Cyc_Toc__get_zero_arr_size_voidstar_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;
static char _tmp60[20]="_dyneither_ptr_plus";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={
_tmp60,_tmp60,_tmp60 + 20};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_plus_re={1,& Cyc_Toc__dyneither_ptr_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(
void*)((void*)& Cyc_Toc__dyneither_ptr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;
static char _tmp63[15]="_zero_arr_plus";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={
_tmp63,_tmp63,_tmp63 + 15};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)((
void*)& Cyc_Toc__zero_arr_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static
char _tmp66[20]="_zero_arr_plus_char";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={
_tmp66,_tmp66,_tmp66 + 20};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_char_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_char_re={1,& Cyc_Toc__zero_arr_plus_char_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_char_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;
static char _tmp69[21]="_zero_arr_plus_short";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={
_tmp69,_tmp69,_tmp69 + 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_short_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_short_re={1,& Cyc_Toc__zero_arr_plus_short_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_short_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;
static char _tmp6C[19]="_zero_arr_plus_int";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={
_tmp6C,_tmp6C,_tmp6C + 19};static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_int_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_int_re={1,& Cyc_Toc__zero_arr_plus_int_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_int_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;
static char _tmp6F[21]="_zero_arr_plus_float";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={
_tmp6F,_tmp6F,_tmp6F + 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_float_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_float_re={1,& Cyc_Toc__zero_arr_plus_float_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_float_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;
static char _tmp72[22]="_zero_arr_plus_double";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={
_tmp72,_tmp72,_tmp72 + 22};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_double_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_double_re={1,& Cyc_Toc__zero_arr_plus_double_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_double_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;
static char _tmp75[26]="_zero_arr_plus_longdouble";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={
_tmp75,_tmp75,_tmp75 + 26};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_longdouble_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_longdouble_re={1,& Cyc_Toc__zero_arr_plus_longdouble_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_plus_longdouble_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;
static char _tmp78[24]="_zero_arr_plus_voidstar";static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={
_tmp78,_tmp78,_tmp78 + 24};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_plus_voidstar_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_plus_voidstar_re={1,& Cyc_Toc__zero_arr_plus_voidstar_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_plus_voidstar_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;
static char _tmp7B[28]="_dyneither_ptr_inplace_plus";static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={
_tmp7B,_tmp7B,_tmp7B + 28};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_inplace_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={
0,(void*)((void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;
static char _tmp7E[23]="_zero_arr_inplace_plus";static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={
_tmp7E,_tmp7E,_tmp7E + 23};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_re={1,& Cyc_Toc__zero_arr_inplace_plus_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(
void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;
static char _tmp81[28]="_zero_arr_inplace_plus_char";static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={
_tmp81,_tmp81,_tmp81 + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_char_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_char_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_char_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;
static char _tmp84[29]="_zero_arr_inplace_plus_short";static struct _dyneither_ptr
Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp84,_tmp84,_tmp84 + 29};static struct
_tuple1 Cyc_Toc__zero_arr_inplace_plus_short_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_short_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1,&
Cyc_Toc__zero_arr_inplace_plus_short_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_short_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=&
Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp87[27]="_zero_arr_inplace_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp87,_tmp87,
_tmp87 + 27};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_int_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_int_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_int_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;
static char _tmp8A[29]="_zero_arr_inplace_plus_float";static struct _dyneither_ptr
Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp8A,_tmp8A,_tmp8A + 29};static struct
_tuple1 Cyc_Toc__zero_arr_inplace_plus_float_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_float_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1,&
Cyc_Toc__zero_arr_inplace_plus_float_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_float_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=&
Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp8D[30]="_zero_arr_inplace_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp8D,
_tmp8D,_tmp8D + 30};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_double_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1,&
Cyc_Toc__zero_arr_inplace_plus_double_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_double_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=&
Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmp90[34]="_zero_arr_inplace_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp90,
_tmp90,_tmp90 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={
1,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=&
Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmp93[32]="_zero_arr_inplace_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp93,
_tmp93,_tmp93 + 32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1,&
Cyc_Toc__zero_arr_inplace_plus_voidstar_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=&
Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmp96[33]="_dyneither_ptr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp96,
_tmp96,_tmp96 + 33};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,&
Cyc_Toc__dyneither_ptr_inplace_plus_post_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)((void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=&
Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmp99[28]="_zero_arr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp99,_tmp99,
_tmp99 + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={
0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;
static char _tmp9C[33]="_zero_arr_inplace_plus_post_char";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp9C,_tmp9C,_tmp9C
+ 33};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_char_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,&
Cyc_Toc__zero_arr_inplace_plus_post_char_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=&
Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmp9F[34]="_zero_arr_inplace_plus_post_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp9F,
_tmp9F,_tmp9F + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_short_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={
1,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=&
Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmpA2[32]="_zero_arr_inplace_plus_post_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmpA2,
_tmpA2,_tmpA2 + 32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_int_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1,&
Cyc_Toc__zero_arr_inplace_plus_post_int_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=&
Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmpA5[34]="_zero_arr_inplace_plus_post_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmpA5,
_tmpA5,_tmpA5 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_float_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={
1,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)((void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=&
Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmpA8[35]="_zero_arr_inplace_plus_post_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmpA8,
_tmpA8,_tmpA8 + 35};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_double_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={
1,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)((void*)&
Cyc_Toc__zero_arr_inplace_plus_post_double_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;
static char _tmpAB[39]="_zero_arr_inplace_plus_post_longdouble";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmpAB,_tmpAB,
_tmpAB + 39};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={
1,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)((
void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;
static char _tmpAE[37]="_zero_arr_inplace_plus_post_voidstar";static struct
_dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmpAE,_tmpAE,
_tmpAE + 37};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={(
union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={
1,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)((
void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;
static char _tmpB1[10]="_cycalloc";static struct _dyneither_ptr Cyc_Toc__cycalloc_str={
_tmpB1,_tmpB1,_tmpB1 + 10};static struct _tuple1 Cyc_Toc__cycalloc_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cycalloc_re={1,& Cyc_Toc__cycalloc_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__cycalloc_ev={0,(void*)((void*)& Cyc_Toc__cycalloc_re),0,(void*)((void*)
Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;
static char _tmpB4[11]="_cyccalloc";static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={
_tmpB4,_tmpB4,_tmpB4 + 11};static struct _tuple1 Cyc_Toc__cyccalloc_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cyccalloc_re={1,& Cyc_Toc__cyccalloc_pr,(void*)((void*)0)};static struct
Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)((void*)& Cyc_Toc__cyccalloc_re),0,(
void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=&
Cyc_Toc__cyccalloc_ev;static char _tmpB7[17]="_cycalloc_atomic";static struct
_dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmpB7,_tmpB7,_tmpB7 + 17};static
struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={(union Cyc_Absyn_Nmspace_union)((struct
Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__cycalloc_atomic_re={1,& Cyc_Toc__cycalloc_atomic_pr,(void*)((void*)0)};
static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)((void*)& Cyc_Toc__cycalloc_atomic_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=&
Cyc_Toc__cycalloc_atomic_ev;static char _tmpBA[18]="_cyccalloc_atomic";static
struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={_tmpBA,_tmpBA,_tmpBA + 18};
static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__cyccalloc_atomic_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__cyccalloc_atomic_re={1,& Cyc_Toc__cyccalloc_atomic_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)((
void*)& Cyc_Toc__cyccalloc_atomic_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;
static char _tmpBD[15]="_region_malloc";static struct _dyneither_ptr Cyc_Toc__region_malloc_str={
_tmpBD,_tmpBD,_tmpBD + 15};static struct _tuple1 Cyc_Toc__region_malloc_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__region_malloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)((
void*)& Cyc_Toc__region_malloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static
char _tmpC0[15]="_region_calloc";static struct _dyneither_ptr Cyc_Toc__region_calloc_str={
_tmpC0,_tmpC0,_tmpC0 + 15};static struct _tuple1 Cyc_Toc__region_calloc_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__region_calloc_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__region_calloc_re={1,& Cyc_Toc__region_calloc_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)((
void*)& Cyc_Toc__region_calloc_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static
char _tmpC3[13]="_check_times";static struct _dyneither_ptr Cyc_Toc__check_times_str={
_tmpC3,_tmpC3,_tmpC3 + 13};static struct _tuple1 Cyc_Toc__check_times_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)((void*)& Cyc_Toc__check_times_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=&
Cyc_Toc__check_times_ev;static char _tmpC6[12]="_new_region";static struct
_dyneither_ptr Cyc_Toc__new_region_str={_tmpC6,_tmpC6,_tmpC6 + 12};static struct
_tuple1 Cyc_Toc__new_region_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__new_region_re={
1,& Cyc_Toc__new_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={
0,(void*)((void*)& Cyc_Toc__new_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static
char _tmpC9[13]="_push_region";static struct _dyneither_ptr Cyc_Toc__push_region_str={
_tmpC9,_tmpC9,_tmpC9 + 13};static struct _tuple1 Cyc_Toc__push_region_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)((void*)& Cyc_Toc__push_region_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=&
Cyc_Toc__push_region_ev;static char _tmpCC[12]="_pop_region";static struct
_dyneither_ptr Cyc_Toc__pop_region_str={_tmpCC,_tmpCC,_tmpCC + 12};static struct
_tuple1 Cyc_Toc__pop_region_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_region_re={
1,& Cyc_Toc__pop_region_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={
0,(void*)((void*)& Cyc_Toc__pop_region_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static
char _tmpCF[16]="_open_dynregion";static struct _dyneither_ptr Cyc_Toc__open_dynregion_str={
_tmpCF,_tmpCF,_tmpCF + 16};static struct _tuple1 Cyc_Toc__open_dynregion_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__open_dynregion_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__open_dynregion_re={1,& Cyc_Toc__open_dynregion_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={0,(void*)((
void*)& Cyc_Toc__open_dynregion_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__open_dynregion_e=& Cyc_Toc__open_dynregion_ev;
static char _tmpD2[16]="_push_dynregion";static struct _dyneither_ptr Cyc_Toc__push_dynregion_str={
_tmpD2,_tmpD2,_tmpD2 + 16};static struct _tuple1 Cyc_Toc__push_dynregion_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__push_dynregion_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__push_dynregion_re={1,& Cyc_Toc__push_dynregion_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={0,(void*)((
void*)& Cyc_Toc__push_dynregion_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__push_dynregion_e=& Cyc_Toc__push_dynregion_ev;
static char _tmpD5[15]="_pop_dynregion";static struct _dyneither_ptr Cyc_Toc__pop_dynregion_str={
_tmpD5,_tmpD5,_tmpD5 + 15};static struct _tuple1 Cyc_Toc__pop_dynregion_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__pop_dynregion_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__pop_dynregion_re={1,& Cyc_Toc__pop_dynregion_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={0,(void*)((
void*)& Cyc_Toc__pop_dynregion_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static
struct Cyc_Absyn_Exp*Cyc_Toc__pop_dynregion_e=& Cyc_Toc__pop_dynregion_ev;static
char _tmpD8[14]="_reset_region";static struct _dyneither_ptr Cyc_Toc__reset_region_str={
_tmpD8,_tmpD8,_tmpD8 + 14};static struct _tuple1 Cyc_Toc__reset_region_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_struct
Cyc_Toc__reset_region_re={1,& Cyc_Toc__reset_region_pr,(void*)((void*)0)};static
struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={0,(void*)((void*)& Cyc_Toc__reset_region_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=&
Cyc_Toc__reset_region_ev;static char _tmpDB[19]="_throw_arraybounds";static struct
_dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmpDB,_tmpDB,_tmpDB + 19};static
struct _tuple1 Cyc_Toc__throw_arraybounds_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__throw_arraybounds_str};static struct
Cyc_Absyn_Var_e_struct Cyc_Toc__throw_arraybounds_re={1,& Cyc_Toc__throw_arraybounds_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(
void*)((void*)& Cyc_Toc__throw_arraybounds_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;
static char _tmpDE[29]="_dyneither_ptr_decrease_size";static struct _dyneither_ptr
Cyc_Toc__dyneither_ptr_decrease_size_str={_tmpDE,_tmpDE,_tmpDE + 29};static struct
_tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={(union Cyc_Absyn_Nmspace_union)((
struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__dyneither_ptr_decrease_size_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,&
Cyc_Toc__dyneither_ptr_decrease_size_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp
Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)((void*)& Cyc_Toc__dyneither_ptr_decrease_size_re),
0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=&
Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmpE1[11]="_swap_word";static
struct _dyneither_ptr Cyc_Toc__swap_word_str={_tmpE1,_tmpE1,_tmpE1 + 11};static
struct _tuple1 Cyc_Toc__swap_word_pr={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& Cyc_Toc__swap_word_str};static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_word_re={
1,& Cyc_Toc__swap_word_pr,(void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={
0,(void*)((void*)& Cyc_Toc__swap_word_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char
_tmpE4[16]="_swap_dyneither";static struct _dyneither_ptr Cyc_Toc__swap_dyneither_str={
_tmpE4,_tmpE4,_tmpE4 + 16};static struct _tuple1 Cyc_Toc__swap_dyneither_pr={(union
Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){0}),& Cyc_Toc__swap_dyneither_str};
static struct Cyc_Absyn_Var_e_struct Cyc_Toc__swap_dyneither_re={1,& Cyc_Toc__swap_dyneither_pr,(
void*)((void*)0)};static struct Cyc_Absyn_Exp Cyc_Toc__swap_dyneither_ev={0,(void*)((
void*)& Cyc_Toc__swap_dyneither_re),0,(void*)((void*)Cyc_Absyn_EmptyAnnot)};
static struct Cyc_Absyn_Exp*Cyc_Toc__swap_dyneither_e=& Cyc_Toc__swap_dyneither_ev;
static struct Cyc_Absyn_AggrType_struct Cyc_Toc_dyneither_ptr_typ_v={10,{(union Cyc_Absyn_AggrInfoU_union)((
struct Cyc_Absyn_UnknownAggr_struct){0,(void*)((void*)0),& Cyc_Toc__dyneither_ptr_pr}),
0}};static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0};static struct Cyc_Absyn_Stmt*
Cyc_Toc_skip_stmt_dl(){static struct Cyc_Absyn_Stmt**skip_stmt_opt=0;if(
skip_stmt_opt == 0)skip_stmt_opt=({struct Cyc_Absyn_Stmt**_tmpE9=_cycalloc(sizeof(*
_tmpE9));_tmpE9[0]=Cyc_Absyn_skip_stmt(0);_tmpE9;});return*skip_stmt_opt;}static
struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){return Cyc_Absyn_cast_exp(
t,e,0,(void*)1,0);}static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_struct*_tmpEA=_cycalloc(sizeof(*_tmpEA));
_tmpEA[0]=({struct Cyc_Absyn_Cast_e_struct _tmpEB;_tmpEB.tag=15;_tmpEB.f1=(void*)t;
_tmpEB.f2=e;_tmpEB.f3=0;_tmpEB.f4=(void*)((void*)1);_tmpEB;});_tmpEA;});}static
void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Absyn_Deref_e_struct*
_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC[0]=({struct Cyc_Absyn_Deref_e_struct
_tmpED;_tmpED.tag=22;_tmpED.f1=e;_tmpED;});_tmpEC;});}static void*Cyc_Toc_subscript_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){return(void*)({struct Cyc_Absyn_Subscript_e_struct*
_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE[0]=({struct Cyc_Absyn_Subscript_e_struct
_tmpEF;_tmpEF.tag=25;_tmpEF.f1=e1;_tmpEF.f2=e2;_tmpEF;});_tmpEE;});}static void*
Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){return(void*)({struct Cyc_Absyn_StmtExp_e_struct*
_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0[0]=({struct Cyc_Absyn_StmtExp_e_struct
_tmpF1;_tmpF1.tag=38;_tmpF1.f1=s;_tmpF1;});_tmpF0;});}static void*Cyc_Toc_sizeoftyp_exp_r(
void*t){return(void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*_tmpF2=_cycalloc(
sizeof(*_tmpF2));_tmpF2[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct _tmpF3;_tmpF3.tag=
18;_tmpF3.f1=(void*)t;_tmpF3;});_tmpF2;});}static void*Cyc_Toc_fncall_exp_r(
struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){return(void*)({struct Cyc_Absyn_FnCall_e_struct*
_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4[0]=({struct Cyc_Absyn_FnCall_e_struct
_tmpF5;_tmpF5.tag=11;_tmpF5.f1=e;_tmpF5.f2=es;_tmpF5.f3=0;_tmpF5;});_tmpF4;});}
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_Absyn_Exp_s_struct*
_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6[0]=({struct Cyc_Absyn_Exp_s_struct _tmpF7;
_tmpF7.tag=0;_tmpF7.f1=e;_tmpF7;});_tmpF6;});}static void*Cyc_Toc_seq_stmt_r(
struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){return(void*)({struct Cyc_Absyn_Seq_s_struct*
_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8[0]=({struct Cyc_Absyn_Seq_s_struct _tmpF9;
_tmpF9.tag=1;_tmpF9.f1=s1;_tmpF9.f2=s2;_tmpF9;});_tmpF8;});}static void*Cyc_Toc_conditional_exp_r(
struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){return(
void*)({struct Cyc_Absyn_Conditional_e_struct*_tmpFA=_cycalloc(sizeof(*_tmpFA));
_tmpFA[0]=({struct Cyc_Absyn_Conditional_e_struct _tmpFB;_tmpFB.tag=6;_tmpFB.f1=e1;
_tmpFB.f2=e2;_tmpFB.f3=e3;_tmpFB;});_tmpFA;});}static void*Cyc_Toc_aggrmember_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){return(void*)({struct Cyc_Absyn_AggrMember_e_struct*
_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC[0]=({struct Cyc_Absyn_AggrMember_e_struct
_tmpFD;_tmpFD.tag=23;_tmpFD.f1=e;_tmpFD.f2=n;_tmpFD;});_tmpFC;});}static void*Cyc_Toc_aggrarrow_exp_r(
struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){return(void*)({struct Cyc_Absyn_AggrArrow_e_struct*
_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE[0]=({struct Cyc_Absyn_AggrArrow_e_struct
_tmpFF;_tmpFF.tag=24;_tmpFF.f1=e;_tmpFF.f2=n;_tmpFF;});_tmpFE;});}static void*Cyc_Toc_unresolvedmem_exp_r(
struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){return(void*)({struct Cyc_Absyn_UnresolvedMem_e_struct*
_tmp100=_cycalloc(sizeof(*_tmp100));_tmp100[0]=({struct Cyc_Absyn_UnresolvedMem_e_struct
_tmp101;_tmp101.tag=37;_tmp101.f1=tdopt;_tmp101.f2=ds;_tmp101;});_tmp100;});}
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_Goto_s_struct*_tmp102=_cycalloc(sizeof(*_tmp102));
_tmp102[0]=({struct Cyc_Absyn_Goto_s_struct _tmp103;_tmp103.tag=7;_tmp103.f1=v;
_tmp103.f2=s;_tmp103;});_tmp102;});}static struct Cyc_Absyn_Const_e_struct Cyc_Toc_zero_exp={
0,(union Cyc_Absyn_Cnst_union)((struct Cyc_Absyn_Int_c_struct){2,(void*)((void*)0),
0})};struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*
fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*
fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};struct
Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,&
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
struct Cyc_Toc_functionSet*fS,void*t){struct Cyc_Absyn_Exp*function;{void*_tmp106=
t;void*_tmp107;int _tmp108;_LL1: if(_tmp106 <= (void*)4)goto _LL3;if(*((int*)_tmp106)
!= 5)goto _LL3;_tmp107=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp106)->f2;_LL2:{
void*_tmp109=_tmp107;_LLC: if((int)_tmp109 != 0)goto _LLE;_LLD: function=fS->fchar;
goto _LLB;_LLE: if((int)_tmp109 != 1)goto _LL10;_LLF: function=fS->fshort;goto _LLB;
_LL10: if((int)_tmp109 != 2)goto _LL12;_LL11: function=fS->fint;goto _LLB;_LL12:;
_LL13:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp10A=_cycalloc(
sizeof(*_tmp10A));_tmp10A[0]=({struct Cyc_Core_Impossible_struct _tmp10B;_tmp10B.tag=
Cyc_Core_Impossible;_tmp10B.f1=({const char*_tmp10C="impossible IntType (not char, short or int)";
_tag_dyneither(_tmp10C,sizeof(char),44);});_tmp10B;});_tmp10A;}));_LLB:;}goto
_LL0;_LL3: if((int)_tmp106 != 1)goto _LL5;_LL4: function=fS->ffloat;goto _LL0;_LL5:
if(_tmp106 <= (void*)4)goto _LL9;if(*((int*)_tmp106)!= 6)goto _LL7;_tmp108=((struct
Cyc_Absyn_DoubleType_struct*)_tmp106)->f1;_LL6: switch(_tmp108){case 1: _LL14:
function=fS->flongdouble;break;default: _LL15: function=fS->fdouble;}goto _LL0;_LL7:
if(*((int*)_tmp106)!= 4)goto _LL9;_LL8: function=fS->fvoidstar;goto _LL0;_LL9:;_LLA:(
int)_throw((void*)({struct Cyc_Core_Impossible_struct*_tmp10D=_cycalloc(sizeof(*
_tmp10D));_tmp10D[0]=({struct Cyc_Core_Impossible_struct _tmp10E;_tmp10E.tag=Cyc_Core_Impossible;
_tmp10E.f1=({const char*_tmp10F="impossible expression type (not int, float, double, or pointer)";
_tag_dyneither(_tmp10F,sizeof(char),64);});_tmp10E;});_tmp10D;}));_LL0:;}return
function;}struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*fS,
struct Cyc_Absyn_Exp*arr){return Cyc_Toc_getFunctionType(fS,Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(arr->topt))->v));}struct Cyc_Absyn_Exp*
Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*
arr){void*_tmp110=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
arr->topt))->v);struct Cyc_Absyn_PtrInfo _tmp111;void*_tmp112;_LL18: if(_tmp110 <= (
void*)4)goto _LL1A;if(*((int*)_tmp110)!= 4)goto _LL1A;_tmp111=((struct Cyc_Absyn_PointerType_struct*)
_tmp110)->f1;_tmp112=(void*)_tmp111.elt_typ;_LL19: return Cyc_Toc_getFunctionType(
fS,_tmp112);_LL1A:;_LL1B:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113[0]=({struct Cyc_Core_Impossible_struct
_tmp114;_tmp114.tag=Cyc_Core_Impossible;_tmp114.f1=({const char*_tmp115="impossible type (not pointer)";
_tag_dyneither(_tmp115,sizeof(char),30);});_tmp114;});_tmp113;}));_LL17:;}struct
_tuple8{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static int Cyc_Toc_is_zero(
struct Cyc_Absyn_Exp*e){void*_tmp116=(void*)e->r;union Cyc_Absyn_Cnst_union _tmp117;
char _tmp118;union Cyc_Absyn_Cnst_union _tmp119;short _tmp11A;union Cyc_Absyn_Cnst_union
_tmp11B;int _tmp11C;union Cyc_Absyn_Cnst_union _tmp11D;long long _tmp11E;union Cyc_Absyn_Cnst_union
_tmp11F;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_List_List*_tmp121;struct Cyc_List_List*
_tmp122;struct Cyc_List_List*_tmp123;struct Cyc_List_List*_tmp124;struct Cyc_List_List*
_tmp125;_LL1D: if(*((int*)_tmp116)!= 0)goto _LL1F;_tmp117=((struct Cyc_Absyn_Const_e_struct*)
_tmp116)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp116)->f1).Char_c).tag != 
0)goto _LL1F;_tmp118=(_tmp117.Char_c).f2;_LL1E: return _tmp118 == '\000';_LL1F: if(*((
int*)_tmp116)!= 0)goto _LL21;_tmp119=((struct Cyc_Absyn_Const_e_struct*)_tmp116)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp116)->f1).Short_c).tag != 1)goto _LL21;
_tmp11A=(_tmp119.Short_c).f2;_LL20: return _tmp11A == 0;_LL21: if(*((int*)_tmp116)!= 
0)goto _LL23;_tmp11B=((struct Cyc_Absyn_Const_e_struct*)_tmp116)->f1;if(((((struct
Cyc_Absyn_Const_e_struct*)_tmp116)->f1).Int_c).tag != 2)goto _LL23;_tmp11C=(
_tmp11B.Int_c).f2;_LL22: return _tmp11C == 0;_LL23: if(*((int*)_tmp116)!= 0)goto
_LL25;_tmp11D=((struct Cyc_Absyn_Const_e_struct*)_tmp116)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp116)->f1).LongLong_c).tag != 3)goto _LL25;_tmp11E=(_tmp11D.LongLong_c).f2;
_LL24: return _tmp11E == 0;_LL25: if(*((int*)_tmp116)!= 0)goto _LL27;_tmp11F=((struct
Cyc_Absyn_Const_e_struct*)_tmp116)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp116)->f1).Null_c).tag != 6)goto _LL27;_LL26: return 1;_LL27: if(*((int*)_tmp116)
!= 15)goto _LL29;_tmp120=((struct Cyc_Absyn_Cast_e_struct*)_tmp116)->f2;_LL28:
return Cyc_Toc_is_zero(_tmp120);_LL29: if(*((int*)_tmp116)!= 26)goto _LL2B;_tmp121=((
struct Cyc_Absyn_Tuple_e_struct*)_tmp116)->f1;_LL2A: return((int(*)(int(*pred)(
struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,
_tmp121);_LL2B: if(*((int*)_tmp116)!= 28)goto _LL2D;_tmp122=((struct Cyc_Absyn_Array_e_struct*)
_tmp116)->f1;_LL2C: _tmp123=_tmp122;goto _LL2E;_LL2D: if(*((int*)_tmp116)!= 30)goto
_LL2F;_tmp123=((struct Cyc_Absyn_Struct_e_struct*)_tmp116)->f3;_LL2E: _tmp124=
_tmp123;goto _LL30;_LL2F: if(*((int*)_tmp116)!= 27)goto _LL31;_tmp124=((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp116)->f2;_LL30: _tmp125=_tmp124;goto _LL32;_LL31: if(*((int*)_tmp116)!= 37)goto
_LL33;_tmp125=((struct Cyc_Absyn_UnresolvedMem_e_struct*)_tmp116)->f2;_LL32: for(0;
_tmp125 != 0;_tmp125=_tmp125->tl){if(!Cyc_Toc_is_zero((*((struct _tuple8*)_tmp125->hd)).f2))
return 0;}return 1;_LL33:;_LL34: return 0;_LL1C:;}static int Cyc_Toc_is_nullable(void*
t){void*_tmp126=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp127;struct Cyc_Absyn_PtrAtts
_tmp128;struct Cyc_Absyn_Conref*_tmp129;_LL36: if(_tmp126 <= (void*)4)goto _LL38;if(*((
int*)_tmp126)!= 4)goto _LL38;_tmp127=((struct Cyc_Absyn_PointerType_struct*)
_tmp126)->f1;_tmp128=_tmp127.ptr_atts;_tmp129=_tmp128.nullable;_LL37: return((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp129);_LL38:;_LL39:({
void*_tmp12A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp12B="is_nullable";_tag_dyneither(_tmp12B,sizeof(char),12);}),
_tag_dyneither(_tmp12A,sizeof(void*),0));});_LL35:;}static char _tmp138[1]="";
static char _tmp13A[8]="*bogus*";static struct _tuple1*Cyc_Toc_collapse_qvar_tag(
struct _tuple1*x,struct _dyneither_ptr tag){struct _DynRegionHandle*_tmp12D;struct
Cyc_Dict_Dict*_tmp12E;struct Cyc_Toc_TocState _tmp12C=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmp12D=_tmp12C.dyn;_tmp12E=_tmp12C.qvar_tags;{
static struct _dyneither_ptr bogus_string={_tmp13A,_tmp13A,_tmp13A + 8};static struct
_tuple1 bogus_qvar={(union Cyc_Absyn_Nmspace_union)((struct Cyc_Absyn_Loc_n_struct){
0}),& bogus_string};static struct _tuple6 pair={& bogus_qvar,{_tmp138,_tmp138,_tmp138
+ 1}};pair=({struct _tuple6 _tmp12F;_tmp12F.f1=x;_tmp12F.f2=tag;_tmp12F;});{struct
_DynRegionFrame _tmp130;struct _RegionHandle*d=_open_dynregion(& _tmp130,_tmp12D);{
struct _tuple1**_tmp131=((struct _tuple1**(*)(struct Cyc_Dict_Dict d,struct _tuple6*k))
Cyc_Dict_lookup_opt)(*_tmp12E,(struct _tuple6*)& pair);if(_tmp131 != 0){struct
_tuple1*_tmp132=*_tmp131;_npop_handler(0);return _tmp132;}{struct _tuple6*_tmp133=({
struct _tuple6*_tmp137=_cycalloc(sizeof(*_tmp137));_tmp137->f1=x;_tmp137->f2=tag;
_tmp137;});struct _tuple1*res=({struct _tuple1*_tmp135=_cycalloc(sizeof(*_tmp135));
_tmp135->f1=(*x).f1;_tmp135->f2=({struct _dyneither_ptr*_tmp136=_cycalloc(sizeof(*
_tmp136));_tmp136[0]=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)*(*
x).f2,(struct _dyneither_ptr)tag);_tmp136;});_tmp135;});*_tmp12E=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple6*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmp12E,(
struct _tuple6*)_tmp133,res);{struct _tuple1*_tmp134=res;_npop_handler(0);return
_tmp134;}}};_pop_dynregion(d);}}}struct _tuple9{void*f1;struct Cyc_List_List*f2;};
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){struct
_DynRegionHandle*_tmp13C;struct Cyc_List_List**_tmp13D;struct Cyc_Toc_TocState
_tmp13B=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp13C=
_tmp13B.dyn;_tmp13D=_tmp13B.tuple_types;{struct _DynRegionFrame _tmp13E;struct
_RegionHandle*d=_open_dynregion(& _tmp13E,_tmp13C);{struct Cyc_List_List*_tmp13F=*
_tmp13D;for(0;_tmp13F != 0;_tmp13F=_tmp13F->tl){struct _tuple9 _tmp141;void*_tmp142;
struct Cyc_List_List*_tmp143;struct _tuple9*_tmp140=(struct _tuple9*)_tmp13F->hd;
_tmp141=*_tmp140;_tmp142=_tmp141.f1;_tmp143=_tmp141.f2;{struct Cyc_List_List*
_tmp144=tqs0;for(0;_tmp144 != 0  && _tmp143 != 0;(_tmp144=_tmp144->tl,_tmp143=
_tmp143->tl)){if(!Cyc_Tcutil_unify((*((struct _tuple4*)_tmp144->hd)).f2,(void*)
_tmp143->hd))break;}if(_tmp144 == 0  && _tmp143 == 0){void*_tmp145=_tmp142;
_npop_handler(0);return _tmp145;}}}}{struct _dyneither_ptr*xname=({struct
_dyneither_ptr*_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154[0]=(struct
_dyneither_ptr)({struct Cyc_Int_pa_struct _tmp157;_tmp157.tag=1;_tmp157.f1=(
unsigned long)Cyc_Toc_tuple_type_counter ++;{void*_tmp155[1]={& _tmp157};Cyc_aprintf(({
const char*_tmp156="_tuple%d";_tag_dyneither(_tmp156,sizeof(char),9);}),
_tag_dyneither(_tmp155,sizeof(void*),1));}});_tmp154;});void*x=Cyc_Absyn_strct(
xname);struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,
void*(*f)(struct _tuple4*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,
tqs0);struct Cyc_List_List*_tmp146=0;struct Cyc_List_List*ts2=ts;{int i=1;for(0;ts2
!= 0;(ts2=ts2->tl,i ++)){_tmp146=({struct Cyc_List_List*_tmp147=_cycalloc(sizeof(*
_tmp147));_tmp147->hd=({struct Cyc_Absyn_Aggrfield*_tmp148=_cycalloc(sizeof(*
_tmp148));_tmp148->name=Cyc_Absyn_fieldname(i);_tmp148->tq=Cyc_Toc_mt_tq;_tmp148->type=(
void*)((void*)ts2->hd);_tmp148->width=0;_tmp148->attributes=0;_tmp148;});_tmp147->tl=
_tmp146;_tmp147;});}}_tmp146=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(_tmp146);{struct Cyc_Absyn_Aggrdecl*_tmp149=({struct Cyc_Absyn_Aggrdecl*
_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150->kind=(void*)((void*)0);_tmp150->sc=(
void*)((void*)2);_tmp150->name=({struct _tuple1*_tmp152=_cycalloc(sizeof(*_tmp152));
_tmp152->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp153;(
_tmp153.Rel_n).tag=1;(_tmp153.Rel_n).f1=0;_tmp153;});_tmp152->f2=xname;_tmp152;});
_tmp150->tvs=0;_tmp150->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp151=_cycalloc(
sizeof(*_tmp151));_tmp151->exist_vars=0;_tmp151->rgn_po=0;_tmp151->fields=
_tmp146;_tmp151;});_tmp150->attributes=0;_tmp150;});Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A->hd=Cyc_Absyn_new_decl((
void*)({struct Cyc_Absyn_Aggr_d_struct*_tmp14B=_cycalloc(sizeof(*_tmp14B));
_tmp14B[0]=({struct Cyc_Absyn_Aggr_d_struct _tmp14C;_tmp14C.tag=4;_tmp14C.f1=
_tmp149;_tmp14C;});_tmp14B;}),0);_tmp14A->tl=Cyc_Toc_result_decls;_tmp14A;});*
_tmp13D=({struct Cyc_List_List*_tmp14D=_region_malloc(d,sizeof(*_tmp14D));_tmp14D->hd=({
struct _tuple9*_tmp14E=_region_malloc(d,sizeof(*_tmp14E));_tmp14E->f1=x;_tmp14E->f2=
ts;_tmp14E;});_tmp14D->tl=*_tmp13D;_tmp14D;});{void*_tmp14F=x;_npop_handler(0);
return _tmp14F;}}};_pop_dynregion(d);}}struct _tuple1*Cyc_Toc_temp_var(){int
_tmp158=Cyc_Toc_temp_var_counter ++;struct _tuple1*res=({struct _tuple1*_tmp159=
_cycalloc(sizeof(*_tmp159));_tmp159->f1=(union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union
_tmp15E;(_tmp15E.Loc_n).tag=0;_tmp15E;});_tmp159->f2=({struct _dyneither_ptr*
_tmp15A=_cycalloc(sizeof(*_tmp15A));_tmp15A[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_struct
_tmp15D;_tmp15D.tag=1;_tmp15D.f1=(unsigned int)_tmp158;{void*_tmp15B[1]={&
_tmp15D};Cyc_aprintf(({const char*_tmp15C="_tmp%X";_tag_dyneither(_tmp15C,sizeof(
char),7);}),_tag_dyneither(_tmp15B,sizeof(void*),1));}});_tmp15A;});_tmp159;});
return res;}static struct _dyneither_ptr*Cyc_Toc_fresh_label(){struct
_DynRegionHandle*_tmp160;struct Cyc_Xarray_Xarray*_tmp161;struct Cyc_Toc_TocState
_tmp15F=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp160=
_tmp15F.dyn;_tmp161=_tmp15F.temp_labels;{struct _DynRegionFrame _tmp162;struct
_RegionHandle*d=_open_dynregion(& _tmp162,_tmp160);{int _tmp163=Cyc_Toc_fresh_label_counter
++;if(_tmp163 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp161)){
struct _dyneither_ptr*_tmp164=((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,
int))Cyc_Xarray_get)(_tmp161,_tmp163);_npop_handler(0);return _tmp164;}{struct
_dyneither_ptr*res=({struct _dyneither_ptr*_tmp168=_cycalloc(sizeof(*_tmp168));
_tmp168[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp16B;_tmp16B.tag=1;
_tmp16B.f1=(unsigned int)_tmp163;{void*_tmp169[1]={& _tmp16B};Cyc_aprintf(({const
char*_tmp16A="_LL%X";_tag_dyneither(_tmp16A,sizeof(char),6);}),_tag_dyneither(
_tmp169,sizeof(void*),1));}});_tmp168;});if(((int(*)(struct Cyc_Xarray_Xarray*,
struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp161,res)!= _tmp163)({void*_tmp165=
0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp166="fresh_label: add_ind returned bad index...";_tag_dyneither(
_tmp166,sizeof(char),43);}),_tag_dyneither(_tmp165,sizeof(void*),0));});{struct
_dyneither_ptr*_tmp167=res;_npop_handler(0);return _tmp167;}}};_pop_dynregion(d);}}
static struct Cyc_Absyn_Exp*Cyc_Toc_tunion_tag(struct Cyc_Absyn_Tuniondecl*td,
struct _tuple1*name,int carries_value){int ans=0;struct Cyc_List_List*_tmp16C=(
struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;while(Cyc_Absyn_qvar_cmp(
name,((struct Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp16C))->hd)->name)
!= 0){if((td->is_flat  || carries_value  && ((struct Cyc_Absyn_Tunionfield*)_tmp16C->hd)->typs
!= 0) || !carries_value  && ((struct Cyc_Absyn_Tunionfield*)_tmp16C->hd)->typs == 0)
++ ans;_tmp16C=_tmp16C->tl;}return Cyc_Absyn_uint_exp((unsigned int)ans,0);}static
int Cyc_Toc_num_void_tags(struct Cyc_Absyn_Tuniondecl*td){int ans=0;{struct Cyc_List_List*
_tmp16D=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
for(0;_tmp16D != 0;_tmp16D=_tmp16D->tl){if(((struct Cyc_Absyn_Tunionfield*)_tmp16D->hd)->typs
== 0)++ ans;}}return ans;}static void*Cyc_Toc_typ_to_c(void*t);static struct _tuple2*
Cyc_Toc_arg_to_c(struct _tuple2*a){struct Cyc_Core_Opt*_tmp16F;struct Cyc_Absyn_Tqual
_tmp170;void*_tmp171;struct _tuple2 _tmp16E=*a;_tmp16F=_tmp16E.f1;_tmp170=_tmp16E.f2;
_tmp171=_tmp16E.f3;return({struct _tuple2*_tmp172=_cycalloc(sizeof(*_tmp172));
_tmp172->f1=_tmp16F;_tmp172->f2=_tmp170;_tmp172->f3=Cyc_Toc_typ_to_c(_tmp171);
_tmp172;});}static struct _tuple4*Cyc_Toc_typ_to_c_f(struct _tuple4*x){struct Cyc_Absyn_Tqual
_tmp174;void*_tmp175;struct _tuple4 _tmp173=*x;_tmp174=_tmp173.f1;_tmp175=_tmp173.f2;
return({struct _tuple4*_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176->f1=_tmp174;
_tmp176->f2=Cyc_Toc_typ_to_c(_tmp175);_tmp176;});}static void*Cyc_Toc_typ_to_c_array(
void*t){void*_tmp177=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp178;
void*_tmp179;struct Cyc_Absyn_Tqual _tmp17A;struct Cyc_Absyn_Exp*_tmp17B;struct Cyc_Absyn_Conref*
_tmp17C;struct Cyc_Position_Segment*_tmp17D;struct Cyc_Core_Opt*_tmp17E;struct Cyc_Core_Opt
_tmp17F;void*_tmp180;_LL3B: if(_tmp177 <= (void*)4)goto _LL3F;if(*((int*)_tmp177)!= 
7)goto _LL3D;_tmp178=((struct Cyc_Absyn_ArrayType_struct*)_tmp177)->f1;_tmp179=(
void*)_tmp178.elt_type;_tmp17A=_tmp178.tq;_tmp17B=_tmp178.num_elts;_tmp17C=
_tmp178.zero_term;_tmp17D=_tmp178.zt_loc;_LL3C: return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(
_tmp179),_tmp17A,_tmp17B,Cyc_Absyn_false_conref,_tmp17D);_LL3D: if(*((int*)
_tmp177)!= 0)goto _LL3F;_tmp17E=((struct Cyc_Absyn_Evar_struct*)_tmp177)->f2;if(
_tmp17E == 0)goto _LL3F;_tmp17F=*_tmp17E;_tmp180=(void*)_tmp17F.v;_LL3E: return Cyc_Toc_typ_to_c_array(
_tmp180);_LL3F:;_LL40: return Cyc_Toc_typ_to_c(t);_LL3A:;}static struct Cyc_Absyn_Aggrfield*
Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){return({struct Cyc_Absyn_Aggrfield*
_tmp181=_cycalloc(sizeof(*_tmp181));_tmp181->name=f->name;_tmp181->tq=Cyc_Toc_mt_tq;
_tmp181->type=(void*)Cyc_Toc_typ_to_c((void*)f->type);_tmp181->width=f->width;
_tmp181->attributes=f->attributes;_tmp181;});}static void Cyc_Toc_enumfields_to_c(
struct Cyc_List_List*fs){return;}static void*Cyc_Toc_char_star_typ(){static void**
cs=0;if(cs == 0)cs=({void**_tmp182=_cycalloc(sizeof(*_tmp182));_tmp182[0]=Cyc_Absyn_star_typ(
Cyc_Absyn_char_typ,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);_tmp182;});
return*cs;}static void*Cyc_Toc_rgn_typ(){static void**r=0;if(r == 0)r=({void**
_tmp183=_cycalloc(sizeof(*_tmp183));_tmp183[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(
Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq);_tmp183;});return*r;}static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;if(r == 0)r=({void**_tmp184=_cycalloc(sizeof(*_tmp184));_tmp184[0]=
Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq);
_tmp184;});return*r;}static void*Cyc_Toc_typ_to_c(void*t){void*_tmp185=t;struct
Cyc_Core_Opt*_tmp186;struct Cyc_Core_Opt*_tmp187;struct Cyc_Core_Opt _tmp188;void*
_tmp189;struct Cyc_Absyn_Tvar*_tmp18A;struct Cyc_Absyn_TunionInfo _tmp18B;union Cyc_Absyn_TunionInfoU_union
_tmp18C;struct Cyc_Absyn_Tuniondecl**_tmp18D;struct Cyc_Absyn_Tuniondecl*_tmp18E;
struct Cyc_Absyn_TunionFieldInfo _tmp18F;union Cyc_Absyn_TunionFieldInfoU_union
_tmp190;struct Cyc_Absyn_Tuniondecl*_tmp191;struct Cyc_Absyn_Tunionfield*_tmp192;
struct Cyc_Absyn_PtrInfo _tmp193;void*_tmp194;struct Cyc_Absyn_Tqual _tmp195;struct
Cyc_Absyn_PtrAtts _tmp196;struct Cyc_Absyn_Conref*_tmp197;struct Cyc_Absyn_ArrayInfo
_tmp198;void*_tmp199;struct Cyc_Absyn_Tqual _tmp19A;struct Cyc_Absyn_Exp*_tmp19B;
struct Cyc_Position_Segment*_tmp19C;struct Cyc_Absyn_FnInfo _tmp19D;void*_tmp19E;
struct Cyc_List_List*_tmp19F;int _tmp1A0;struct Cyc_Absyn_VarargInfo*_tmp1A1;struct
Cyc_List_List*_tmp1A2;struct Cyc_List_List*_tmp1A3;void*_tmp1A4;struct Cyc_List_List*
_tmp1A5;struct Cyc_Absyn_AggrInfo _tmp1A6;union Cyc_Absyn_AggrInfoU_union _tmp1A7;
struct Cyc_List_List*_tmp1A8;struct _tuple1*_tmp1A9;struct Cyc_List_List*_tmp1AA;
struct _tuple1*_tmp1AB;struct Cyc_List_List*_tmp1AC;struct Cyc_Absyn_Typedefdecl*
_tmp1AD;void**_tmp1AE;void*_tmp1AF;_LL42: if((int)_tmp185 != 0)goto _LL44;_LL43:
return t;_LL44: if(_tmp185 <= (void*)4)goto _LL56;if(*((int*)_tmp185)!= 0)goto _LL46;
_tmp186=((struct Cyc_Absyn_Evar_struct*)_tmp185)->f2;if(_tmp186 != 0)goto _LL46;
_LL45: return Cyc_Absyn_sint_typ;_LL46: if(*((int*)_tmp185)!= 0)goto _LL48;_tmp187=((
struct Cyc_Absyn_Evar_struct*)_tmp185)->f2;if(_tmp187 == 0)goto _LL48;_tmp188=*
_tmp187;_tmp189=(void*)_tmp188.v;_LL47: return Cyc_Toc_typ_to_c(_tmp189);_LL48: if(*((
int*)_tmp185)!= 1)goto _LL4A;_tmp18A=((struct Cyc_Absyn_VarType_struct*)_tmp185)->f1;
_LL49: if(Cyc_Tcutil_tvar_kind(_tmp18A)== (void*)0)return(void*)0;else{return Cyc_Absyn_void_star_typ();}
_LL4A: if(*((int*)_tmp185)!= 2)goto _LL4C;_tmp18B=((struct Cyc_Absyn_TunionType_struct*)
_tmp185)->f1;_tmp18C=_tmp18B.tunion_info;if((((((struct Cyc_Absyn_TunionType_struct*)
_tmp185)->f1).tunion_info).KnownTunion).tag != 1)goto _LL4C;_tmp18D=(_tmp18C.KnownTunion).f1;
_tmp18E=*_tmp18D;_LL4B: if(_tmp18E->is_flat)return Cyc_Absyn_unionq_typ(Cyc_Toc_collapse_qvar_tag(
_tmp18E->name,({const char*_tmp1B0="_union";_tag_dyneither(_tmp1B0,sizeof(char),7);})));
else{return Cyc_Absyn_void_star_typ();}_LL4C: if(*((int*)_tmp185)!= 2)goto _LL4E;
_LL4D:({void*_tmp1B1=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp1B2="unresolved TunionType";_tag_dyneither(
_tmp1B2,sizeof(char),22);}),_tag_dyneither(_tmp1B1,sizeof(void*),0));});_LL4E:
if(*((int*)_tmp185)!= 3)goto _LL50;_tmp18F=((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp185)->f1;_tmp190=_tmp18F.field_info;if((((((struct Cyc_Absyn_TunionFieldType_struct*)
_tmp185)->f1).field_info).KnownTunionfield).tag != 1)goto _LL50;_tmp191=(_tmp190.KnownTunionfield).f1;
_tmp192=(_tmp190.KnownTunionfield).f2;_LL4F: if(_tmp191->is_flat)return Cyc_Absyn_unionq_typ(
Cyc_Toc_collapse_qvar_tag(_tmp191->name,({const char*_tmp1B3="_union";
_tag_dyneither(_tmp1B3,sizeof(char),7);})));if(_tmp192->typs == 0){if(_tmp191->is_xtunion)
return Cyc_Toc_char_star_typ();else{return Cyc_Absyn_uint_typ;}}else{return Cyc_Absyn_strctq(
Cyc_Toc_collapse_qvar_tag(_tmp192->name,({const char*_tmp1B4="_struct";
_tag_dyneither(_tmp1B4,sizeof(char),8);})));}_LL50: if(*((int*)_tmp185)!= 3)goto
_LL52;_LL51:({void*_tmp1B5=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1B6="unresolved TunionFieldType";
_tag_dyneither(_tmp1B6,sizeof(char),27);}),_tag_dyneither(_tmp1B5,sizeof(void*),
0));});_LL52: if(*((int*)_tmp185)!= 4)goto _LL54;_tmp193=((struct Cyc_Absyn_PointerType_struct*)
_tmp185)->f1;_tmp194=(void*)_tmp193.elt_typ;_tmp195=_tmp193.elt_tq;_tmp196=
_tmp193.ptr_atts;_tmp197=_tmp196.bounds;_LL53: _tmp194=Cyc_Toc_typ_to_c_array(
_tmp194);{union Cyc_Absyn_Constraint_union _tmp1B7=(Cyc_Absyn_compress_conref(
_tmp197))->v;void*_tmp1B8;_LL7D: if((_tmp1B7.No_constr).tag != 2)goto _LL7F;_LL7E:
goto _LL80;_LL7F: if((_tmp1B7.Eq_constr).tag != 0)goto _LL81;_tmp1B8=(_tmp1B7.Eq_constr).f1;
if((int)_tmp1B8 != 0)goto _LL81;_LL80: return Cyc_Toc_dyneither_ptr_typ;_LL81:;_LL82:
return Cyc_Absyn_star_typ(_tmp194,(void*)2,_tmp195,Cyc_Absyn_false_conref);_LL7C:;}
_LL54: if(*((int*)_tmp185)!= 5)goto _LL56;_LL55: goto _LL57;_LL56: if((int)_tmp185 != 
1)goto _LL58;_LL57: goto _LL59;_LL58: if(_tmp185 <= (void*)4)goto _LL70;if(*((int*)
_tmp185)!= 6)goto _LL5A;_LL59: return t;_LL5A: if(*((int*)_tmp185)!= 7)goto _LL5C;
_tmp198=((struct Cyc_Absyn_ArrayType_struct*)_tmp185)->f1;_tmp199=(void*)_tmp198.elt_type;
_tmp19A=_tmp198.tq;_tmp19B=_tmp198.num_elts;_tmp19C=_tmp198.zt_loc;_LL5B: return
Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c_array(_tmp199),_tmp19A,_tmp19B,Cyc_Absyn_false_conref,
_tmp19C);_LL5C: if(*((int*)_tmp185)!= 8)goto _LL5E;_tmp19D=((struct Cyc_Absyn_FnType_struct*)
_tmp185)->f1;_tmp19E=(void*)_tmp19D.ret_typ;_tmp19F=_tmp19D.args;_tmp1A0=_tmp19D.c_varargs;
_tmp1A1=_tmp19D.cyc_varargs;_tmp1A2=_tmp19D.attributes;_LL5D: {struct Cyc_List_List*
_tmp1B9=0;for(0;_tmp1A2 != 0;_tmp1A2=_tmp1A2->tl){void*_tmp1BA=(void*)_tmp1A2->hd;
_LL84: if((int)_tmp1BA != 3)goto _LL86;_LL85: goto _LL87;_LL86: if((int)_tmp1BA != 4)
goto _LL88;_LL87: goto _LL89;_LL88: if(_tmp1BA <= (void*)17)goto _LL8C;if(*((int*)
_tmp1BA)!= 3)goto _LL8A;_LL89: continue;_LL8A: if(*((int*)_tmp1BA)!= 4)goto _LL8C;
_LL8B: continue;_LL8C:;_LL8D: _tmp1B9=({struct Cyc_List_List*_tmp1BB=_cycalloc(
sizeof(*_tmp1BB));_tmp1BB->hd=(void*)((void*)_tmp1A2->hd);_tmp1BB->tl=_tmp1B9;
_tmp1BB;});goto _LL83;_LL83:;}{struct Cyc_List_List*_tmp1BC=((struct Cyc_List_List*(*)(
struct _tuple2*(*f)(struct _tuple2*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,
_tmp19F);if(_tmp1A1 != 0){void*_tmp1BD=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ((
void*)_tmp1A1->type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));struct
_tuple2*_tmp1BE=({struct _tuple2*_tmp1C0=_cycalloc(sizeof(*_tmp1C0));_tmp1C0->f1=
_tmp1A1->name;_tmp1C0->f2=_tmp1A1->tq;_tmp1C0->f3=_tmp1BD;_tmp1C0;});_tmp1BC=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(
_tmp1BC,({struct Cyc_List_List*_tmp1BF=_cycalloc(sizeof(*_tmp1BF));_tmp1BF->hd=
_tmp1BE;_tmp1BF->tl=0;_tmp1BF;}));}return(void*)({struct Cyc_Absyn_FnType_struct*
_tmp1C1=_cycalloc(sizeof(*_tmp1C1));_tmp1C1[0]=({struct Cyc_Absyn_FnType_struct
_tmp1C2;_tmp1C2.tag=8;_tmp1C2.f1=({struct Cyc_Absyn_FnInfo _tmp1C3;_tmp1C3.tvars=0;
_tmp1C3.effect=0;_tmp1C3.ret_typ=(void*)Cyc_Toc_typ_to_c(_tmp19E);_tmp1C3.args=
_tmp1BC;_tmp1C3.c_varargs=_tmp1A0;_tmp1C3.cyc_varargs=0;_tmp1C3.rgn_po=0;_tmp1C3.attributes=
_tmp1B9;_tmp1C3;});_tmp1C2;});_tmp1C1;});}}_LL5E: if(*((int*)_tmp185)!= 9)goto
_LL60;_tmp1A3=((struct Cyc_Absyn_TupleType_struct*)_tmp185)->f1;_LL5F: _tmp1A3=((
struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct _tuple4*),struct Cyc_List_List*x))
Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1A3);return Cyc_Toc_add_tuple_type(_tmp1A3);
_LL60: if(*((int*)_tmp185)!= 11)goto _LL62;_tmp1A4=(void*)((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp185)->f1;_tmp1A5=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp185)->f2;_LL61:
return(void*)({struct Cyc_Absyn_AnonAggrType_struct*_tmp1C4=_cycalloc(sizeof(*
_tmp1C4));_tmp1C4[0]=({struct Cyc_Absyn_AnonAggrType_struct _tmp1C5;_tmp1C5.tag=11;
_tmp1C5.f1=(void*)_tmp1A4;_tmp1C5.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*
f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,
_tmp1A5);_tmp1C5;});_tmp1C4;});_LL62: if(*((int*)_tmp185)!= 10)goto _LL64;_tmp1A6=((
struct Cyc_Absyn_AggrType_struct*)_tmp185)->f1;_tmp1A7=_tmp1A6.aggr_info;_tmp1A8=
_tmp1A6.targs;_LL63: {struct Cyc_Absyn_Aggrdecl*_tmp1C6=Cyc_Absyn_get_known_aggrdecl(
_tmp1A7);if((void*)_tmp1C6->kind == (void*)1)return Cyc_Toc_aggrdecl_type(_tmp1C6->name,
Cyc_Absyn_unionq_typ);else{return Cyc_Toc_aggrdecl_type(_tmp1C6->name,Cyc_Absyn_strctq);}}
_LL64: if(*((int*)_tmp185)!= 12)goto _LL66;_tmp1A9=((struct Cyc_Absyn_EnumType_struct*)
_tmp185)->f1;_LL65: return t;_LL66: if(*((int*)_tmp185)!= 13)goto _LL68;_tmp1AA=((
struct Cyc_Absyn_AnonEnumType_struct*)_tmp185)->f1;_LL67: Cyc_Toc_enumfields_to_c(
_tmp1AA);return t;_LL68: if(*((int*)_tmp185)!= 16)goto _LL6A;_tmp1AB=((struct Cyc_Absyn_TypedefType_struct*)
_tmp185)->f1;_tmp1AC=((struct Cyc_Absyn_TypedefType_struct*)_tmp185)->f2;_tmp1AD=((
struct Cyc_Absyn_TypedefType_struct*)_tmp185)->f3;_tmp1AE=((struct Cyc_Absyn_TypedefType_struct*)
_tmp185)->f4;_LL69: if(_tmp1AE == 0  || Cyc_noexpand_r){if(_tmp1AC != 0)return(void*)({
struct Cyc_Absyn_TypedefType_struct*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));_tmp1C7[0]=({
struct Cyc_Absyn_TypedefType_struct _tmp1C8;_tmp1C8.tag=16;_tmp1C8.f1=_tmp1AB;
_tmp1C8.f2=0;_tmp1C8.f3=_tmp1AD;_tmp1C8.f4=0;_tmp1C8;});_tmp1C7;});else{return t;}}
else{return(void*)({struct Cyc_Absyn_TypedefType_struct*_tmp1C9=_cycalloc(sizeof(*
_tmp1C9));_tmp1C9[0]=({struct Cyc_Absyn_TypedefType_struct _tmp1CA;_tmp1CA.tag=16;
_tmp1CA.f1=_tmp1AB;_tmp1CA.f2=0;_tmp1CA.f3=_tmp1AD;_tmp1CA.f4=({void**_tmp1CB=
_cycalloc(sizeof(*_tmp1CB));_tmp1CB[0]=Cyc_Toc_typ_to_c_array(*_tmp1AE);_tmp1CB;});
_tmp1CA;});_tmp1C9;});}_LL6A: if(*((int*)_tmp185)!= 18)goto _LL6C;_LL6B: return Cyc_Absyn_uint_typ;
_LL6C: if(*((int*)_tmp185)!= 14)goto _LL6E;_tmp1AF=(void*)((struct Cyc_Absyn_RgnHandleType_struct*)
_tmp185)->f1;_LL6D: return Cyc_Toc_rgn_typ();_LL6E: if(*((int*)_tmp185)!= 15)goto
_LL70;_LL6F: return Cyc_Toc_dyn_rgn_typ();_LL70: if((int)_tmp185 != 2)goto _LL72;
_LL71:({void*_tmp1CC=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp1CD="Toc::typ_to_c: type translation passed the heap region";
_tag_dyneither(_tmp1CD,sizeof(char),55);}),_tag_dyneither(_tmp1CC,sizeof(void*),
0));});_LL72: if((int)_tmp185 != 3)goto _LL74;_LL73:({void*_tmp1CE=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp1CF="Toc::typ_to_c: type translation passed the unique region";
_tag_dyneither(_tmp1CF,sizeof(char),57);}),_tag_dyneither(_tmp1CE,sizeof(void*),
0));});_LL74: if(_tmp185 <= (void*)4)goto _LL76;if(*((int*)_tmp185)!= 19)goto _LL76;
_LL75: goto _LL77;_LL76: if(_tmp185 <= (void*)4)goto _LL78;if(*((int*)_tmp185)!= 20)
goto _LL78;_LL77: goto _LL79;_LL78: if(_tmp185 <= (void*)4)goto _LL7A;if(*((int*)
_tmp185)!= 21)goto _LL7A;_LL79:({void*_tmp1D0=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1D1="Toc::typ_to_c: type translation passed an effect";
_tag_dyneither(_tmp1D1,sizeof(char),49);}),_tag_dyneither(_tmp1D0,sizeof(void*),
0));});_LL7A: if(_tmp185 <= (void*)4)goto _LL41;if(*((int*)_tmp185)!= 17)goto _LL41;
_LL7B:({void*_tmp1D2=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp1D3="Toc::typ_to_c: type translation passed a valueof_t";
_tag_dyneither(_tmp1D3,sizeof(char),51);}),_tag_dyneither(_tmp1D2,sizeof(void*),
0));});_LL41:;}static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct
Cyc_Absyn_Exp*e,struct Cyc_Position_Segment*l){void*_tmp1D4=t;struct Cyc_Absyn_ArrayInfo
_tmp1D5;void*_tmp1D6;struct Cyc_Absyn_Tqual _tmp1D7;_LL8F: if(_tmp1D4 <= (void*)4)
goto _LL91;if(*((int*)_tmp1D4)!= 7)goto _LL91;_tmp1D5=((struct Cyc_Absyn_ArrayType_struct*)
_tmp1D4)->f1;_tmp1D6=(void*)_tmp1D5.elt_type;_tmp1D7=_tmp1D5.tq;_LL90: return Cyc_Toc_cast_it(
Cyc_Absyn_star_typ(_tmp1D6,(void*)2,_tmp1D7,Cyc_Absyn_false_conref),e);_LL91:;
_LL92: return Cyc_Toc_cast_it(t,e);_LL8E:;}static int Cyc_Toc_atomic_typ(void*t){
void*_tmp1D8=Cyc_Tcutil_compress(t);struct Cyc_Absyn_ArrayInfo _tmp1D9;void*
_tmp1DA;struct Cyc_Absyn_AggrInfo _tmp1DB;union Cyc_Absyn_AggrInfoU_union _tmp1DC;
struct Cyc_List_List*_tmp1DD;struct Cyc_Absyn_TunionFieldInfo _tmp1DE;union Cyc_Absyn_TunionFieldInfoU_union
_tmp1DF;struct Cyc_Absyn_Tuniondecl*_tmp1E0;struct Cyc_Absyn_Tunionfield*_tmp1E1;
struct Cyc_List_List*_tmp1E2;_LL94: if((int)_tmp1D8 != 0)goto _LL96;_LL95: return 1;
_LL96: if(_tmp1D8 <= (void*)4)goto _LL9E;if(*((int*)_tmp1D8)!= 1)goto _LL98;_LL97:
return 0;_LL98: if(*((int*)_tmp1D8)!= 5)goto _LL9A;_LL99: goto _LL9B;_LL9A: if(*((int*)
_tmp1D8)!= 12)goto _LL9C;_LL9B: goto _LL9D;_LL9C: if(*((int*)_tmp1D8)!= 13)goto _LL9E;
_LL9D: goto _LL9F;_LL9E: if((int)_tmp1D8 != 1)goto _LLA0;_LL9F: goto _LLA1;_LLA0: if(
_tmp1D8 <= (void*)4)goto _LLB6;if(*((int*)_tmp1D8)!= 6)goto _LLA2;_LLA1: goto _LLA3;
_LLA2: if(*((int*)_tmp1D8)!= 8)goto _LLA4;_LLA3: goto _LLA5;_LLA4: if(*((int*)_tmp1D8)
!= 18)goto _LLA6;_LLA5: return 1;_LLA6: if(*((int*)_tmp1D8)!= 7)goto _LLA8;_tmp1D9=((
struct Cyc_Absyn_ArrayType_struct*)_tmp1D8)->f1;_tmp1DA=(void*)_tmp1D9.elt_type;
_LLA7: return Cyc_Toc_atomic_typ(_tmp1DA);_LLA8: if(*((int*)_tmp1D8)!= 10)goto _LLAA;
_tmp1DB=((struct Cyc_Absyn_AggrType_struct*)_tmp1D8)->f1;_tmp1DC=_tmp1DB.aggr_info;
_LLA9:{union Cyc_Absyn_AggrInfoU_union _tmp1E3=_tmp1DC;_LLB9: if((_tmp1E3.UnknownAggr).tag
!= 0)goto _LLBB;_LLBA: return 0;_LLBB:;_LLBC: goto _LLB8;_LLB8:;}{struct Cyc_Absyn_Aggrdecl*
_tmp1E4=Cyc_Absyn_get_known_aggrdecl(_tmp1DC);if(_tmp1E4->impl == 0)return 0;{
struct Cyc_List_List*_tmp1E5=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1E4->impl))->fields;
for(0;_tmp1E5 != 0;_tmp1E5=_tmp1E5->tl){if(!Cyc_Toc_atomic_typ((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp1E5->hd)->type))return 0;}}return 1;}_LLAA: if(*((int*)_tmp1D8)!= 11)goto _LLAC;
_tmp1DD=((struct Cyc_Absyn_AnonAggrType_struct*)_tmp1D8)->f2;_LLAB: for(0;_tmp1DD
!= 0;_tmp1DD=_tmp1DD->tl){if(!Cyc_Toc_atomic_typ((void*)((struct Cyc_Absyn_Aggrfield*)
_tmp1DD->hd)->type))return 0;}return 1;_LLAC: if(*((int*)_tmp1D8)!= 3)goto _LLAE;
_tmp1DE=((struct Cyc_Absyn_TunionFieldType_struct*)_tmp1D8)->f1;_tmp1DF=_tmp1DE.field_info;
if((((((struct Cyc_Absyn_TunionFieldType_struct*)_tmp1D8)->f1).field_info).KnownTunionfield).tag
!= 1)goto _LLAE;_tmp1E0=(_tmp1DF.KnownTunionfield).f1;_tmp1E1=(_tmp1DF.KnownTunionfield).f2;
_LLAD: _tmp1E2=_tmp1E1->typs;goto _LLAF;_LLAE: if(*((int*)_tmp1D8)!= 9)goto _LLB0;
_tmp1E2=((struct Cyc_Absyn_TupleType_struct*)_tmp1D8)->f1;_LLAF: for(0;_tmp1E2 != 0;
_tmp1E2=_tmp1E2->tl){if(!Cyc_Toc_atomic_typ((*((struct _tuple4*)_tmp1E2->hd)).f2))
return 0;}return 1;_LLB0: if(*((int*)_tmp1D8)!= 2)goto _LLB2;_LLB1: goto _LLB3;_LLB2:
if(*((int*)_tmp1D8)!= 4)goto _LLB4;_LLB3: goto _LLB5;_LLB4: if(*((int*)_tmp1D8)!= 14)
goto _LLB6;_LLB5: return 0;_LLB6:;_LLB7:({struct Cyc_String_pa_struct _tmp1E8;_tmp1E8.tag=
0;_tmp1E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp1E6[1]={& _tmp1E8};((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1E7="atomic_typ:  bad type %s";
_tag_dyneither(_tmp1E7,sizeof(char),25);}),_tag_dyneither(_tmp1E6,sizeof(void*),
1));}});_LL93:;}static int Cyc_Toc_is_void_star(void*t){void*_tmp1E9=Cyc_Tcutil_compress(
t);struct Cyc_Absyn_PtrInfo _tmp1EA;void*_tmp1EB;_LLBE: if(_tmp1E9 <= (void*)4)goto
_LLC0;if(*((int*)_tmp1E9)!= 4)goto _LLC0;_tmp1EA=((struct Cyc_Absyn_PointerType_struct*)
_tmp1E9)->f1;_tmp1EB=(void*)_tmp1EA.elt_typ;_LLBF: {void*_tmp1EC=Cyc_Tcutil_compress(
_tmp1EB);_LLC3: if((int)_tmp1EC != 0)goto _LLC5;_LLC4: return 1;_LLC5:;_LLC6: return 0;
_LLC2:;}_LLC0:;_LLC1: return 0;_LLBD:;}static int Cyc_Toc_is_poly_field(void*t,
struct _dyneither_ptr*f){void*_tmp1ED=Cyc_Tcutil_compress(t);struct Cyc_Absyn_AggrInfo
_tmp1EE;union Cyc_Absyn_AggrInfoU_union _tmp1EF;struct Cyc_List_List*_tmp1F0;_LLC8:
if(_tmp1ED <= (void*)4)goto _LLCC;if(*((int*)_tmp1ED)!= 10)goto _LLCA;_tmp1EE=((
struct Cyc_Absyn_AggrType_struct*)_tmp1ED)->f1;_tmp1EF=_tmp1EE.aggr_info;_LLC9: {
struct Cyc_Absyn_Aggrdecl*_tmp1F1=Cyc_Absyn_get_known_aggrdecl(_tmp1EF);if(
_tmp1F1->impl == 0)({void*_tmp1F2=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1F3="is_poly_field: type missing fields";
_tag_dyneither(_tmp1F3,sizeof(char),35);}),_tag_dyneither(_tmp1F2,sizeof(void*),
0));});_tmp1F0=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1F1->impl))->fields;
goto _LLCB;}_LLCA: if(*((int*)_tmp1ED)!= 11)goto _LLCC;_tmp1F0=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp1ED)->f2;_LLCB: {struct Cyc_Absyn_Aggrfield*_tmp1F4=Cyc_Absyn_lookup_field(
_tmp1F0,f);if(_tmp1F4 == 0)({struct Cyc_String_pa_struct _tmp1F7;_tmp1F7.tag=0;
_tmp1F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);{void*_tmp1F5[1]={&
_tmp1F7};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp1F6="is_poly_field: bad field %s";_tag_dyneither(_tmp1F6,sizeof(
char),28);}),_tag_dyneither(_tmp1F5,sizeof(void*),1));}});return Cyc_Toc_is_void_star((
void*)_tmp1F4->type);}_LLCC:;_LLCD:({struct Cyc_String_pa_struct _tmp1FA;_tmp1FA.tag=
0;_tmp1FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(
t));{void*_tmp1F8[1]={& _tmp1FA};((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp1F9="is_poly_field: bad type %s";
_tag_dyneither(_tmp1F9,sizeof(char),27);}),_tag_dyneither(_tmp1F8,sizeof(void*),
1));}});_LLC7:;}static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){void*
_tmp1FB=(void*)e->r;struct Cyc_Absyn_Exp*_tmp1FC;struct _dyneither_ptr*_tmp1FD;
struct Cyc_Absyn_Exp*_tmp1FE;struct _dyneither_ptr*_tmp1FF;_LLCF: if(*((int*)
_tmp1FB)!= 23)goto _LLD1;_tmp1FC=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1FB)->f1;
_tmp1FD=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp1FB)->f2;_LLD0: return Cyc_Toc_is_poly_field((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1FC->topt))->v,_tmp1FD);_LLD1: if(*((
int*)_tmp1FB)!= 24)goto _LLD3;_tmp1FE=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp1FB)->f1;_tmp1FF=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp1FB)->f2;_LLD2: {
void*_tmp200=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp1FE->topt))->v);struct Cyc_Absyn_PtrInfo _tmp201;void*_tmp202;_LLD6: if(_tmp200
<= (void*)4)goto _LLD8;if(*((int*)_tmp200)!= 4)goto _LLD8;_tmp201=((struct Cyc_Absyn_PointerType_struct*)
_tmp200)->f1;_tmp202=(void*)_tmp201.elt_typ;_LLD7: return Cyc_Toc_is_poly_field(
_tmp202,_tmp1FF);_LLD8:;_LLD9:({struct Cyc_String_pa_struct _tmp205;_tmp205.tag=0;
_tmp205.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp1FE->topt))->v));{void*_tmp203[1]={&
_tmp205};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp204="is_poly_project: bad type %s";_tag_dyneither(_tmp204,sizeof(
char),29);}),_tag_dyneither(_tmp203,sizeof(void*),1));}});_LLD5:;}_LLD3:;_LLD4:
return 0;_LLCE:;}static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*
s){return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,({struct Cyc_List_List*_tmp206=
_cycalloc(sizeof(*_tmp206));_tmp206->hd=s;_tmp206->tl=0;_tmp206;}),0);}static
struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__cycalloc_atomic_e,({struct Cyc_List_List*_tmp207=_cycalloc(sizeof(*
_tmp207));_tmp207->hd=s;_tmp207->tl=0;_tmp207;}),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){if(Cyc_Toc_atomic_typ(t))return
Cyc_Toc_malloc_atomic(s);return Cyc_Toc_malloc_ptr(s);}static struct Cyc_Absyn_Exp*
Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){return Cyc_Absyn_fncall_exp(
Cyc_Toc__region_malloc_e,({struct Cyc_Absyn_Exp*_tmp208[2];_tmp208[1]=s;_tmp208[0]=
rgn;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp208,sizeof(struct Cyc_Absyn_Exp*),2));}),0);}static struct Cyc_Absyn_Exp*
Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type))return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,({
struct Cyc_Absyn_Exp*_tmp209[2];_tmp209[1]=n;_tmp209[0]=s;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp209,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);else{return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,({struct Cyc_Absyn_Exp*
_tmp20A[2];_tmp20A[1]=n;_tmp20A[0]=s;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp20A,sizeof(struct Cyc_Absyn_Exp*),
2));}),0);}}static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*
rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,({
struct Cyc_Absyn_Exp*_tmp20B[3];_tmp20B[2]=n;_tmp20B[1]=s;_tmp20B[0]=rgn;((struct
Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp20B,
sizeof(struct Cyc_Absyn_Exp*),3));}),0);}static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(
struct Cyc_Absyn_Exp*e){return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,({struct Cyc_List_List*
_tmp20C=_cycalloc(sizeof(*_tmp20C));_tmp20C->hd=e;_tmp20C->tl=0;_tmp20C;}),0);}
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*
sz,struct Cyc_Absyn_Exp*e){int is_string=0;{void*_tmp20D=(void*)e->r;union Cyc_Absyn_Cnst_union
_tmp20E;_LLDB: if(*((int*)_tmp20D)!= 0)goto _LLDD;_tmp20E=((struct Cyc_Absyn_Const_e_struct*)
_tmp20D)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp20D)->f1).String_c).tag
!= 5)goto _LLDD;_LLDC: is_string=1;goto _LLDA;_LLDD:;_LLDE: goto _LLDA;_LLDA:;}{
struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*xplussz;if(is_string){struct _tuple1*
x=Cyc_Toc_temp_var();void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,(
struct Cyc_Absyn_Exp*)sz,Cyc_Absyn_false_conref,0);struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(
x,vd_typ,(struct Cyc_Absyn_Exp*)e);Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp20F=_cycalloc(sizeof(*_tmp20F));_tmp20F->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp210=_cycalloc(sizeof(*_tmp210));_tmp210[0]=({
struct Cyc_Absyn_Var_d_struct _tmp211;_tmp211.tag=0;_tmp211.f1=vd;_tmp211;});
_tmp210;}),0);_tmp20F->tl=Cyc_Toc_result_decls;_tmp20F;});xexp=Cyc_Absyn_var_exp(
x,0);xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
e);xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
struct Cyc_Absyn_Exp*urm_exp;urm_exp=Cyc_Absyn_unresolvedmem_exp(0,({struct
_tuple8*_tmp212[3];_tmp212[2]=({struct _tuple8*_tmp215=_cycalloc(sizeof(*_tmp215));
_tmp215->f1=0;_tmp215->f2=xplussz;_tmp215;});_tmp212[1]=({struct _tuple8*_tmp214=
_cycalloc(sizeof(*_tmp214));_tmp214->f1=0;_tmp214->f2=xexp;_tmp214;});_tmp212[0]=({
struct _tuple8*_tmp213=_cycalloc(sizeof(*_tmp213));_tmp213->f1=0;_tmp213->f2=xexp;
_tmp213;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp212,sizeof(struct _tuple8*),3));}),0);return urm_exp;}}}struct
Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;
struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_Env{struct _dyneither_ptr**
break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*
fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;};static int Cyc_Toc_is_toplevel(
struct Cyc_Toc_Env*nv){struct Cyc_Toc_Env _tmp217;int _tmp218;struct Cyc_Toc_Env*
_tmp216=nv;_tmp217=*_tmp216;_tmp218=_tmp217.toplevel;return _tmp218;}static struct
Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple1*x){struct
Cyc_Toc_Env _tmp21A;struct Cyc_Dict_Dict _tmp21B;struct Cyc_Toc_Env*_tmp219=nv;
_tmp21A=*_tmp219;_tmp21B=_tmp21A.varmap;return((struct Cyc_Absyn_Exp*(*)(struct
Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp21B,x);}static struct Cyc_Toc_Env*
Cyc_Toc_empty_env(struct _RegionHandle*r){return({struct Cyc_Toc_Env*_tmp21C=
_region_malloc(r,sizeof(*_tmp21C));_tmp21C->break_lab=(struct _dyneither_ptr**)0;
_tmp21C->continue_lab=(struct _dyneither_ptr**)0;_tmp21C->fallthru_info=(struct
Cyc_Toc_FallthruInfo*)0;_tmp21C->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(
r,Cyc_Absyn_qvar_cmp);_tmp21C->toplevel=(int)1;_tmp21C;});}static struct Cyc_Toc_Env*
Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env
_tmp21E;struct _dyneither_ptr**_tmp21F;struct _dyneither_ptr**_tmp220;struct Cyc_Toc_FallthruInfo*
_tmp221;struct Cyc_Dict_Dict _tmp222;int _tmp223;struct Cyc_Toc_Env*_tmp21D=e;
_tmp21E=*_tmp21D;_tmp21F=_tmp21E.break_lab;_tmp220=_tmp21E.continue_lab;_tmp221=
_tmp21E.fallthru_info;_tmp222=_tmp21E.varmap;_tmp223=_tmp21E.toplevel;return({
struct Cyc_Toc_Env*_tmp224=_region_malloc(r,sizeof(*_tmp224));_tmp224->break_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp21F);_tmp224->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp220);_tmp224->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp221;_tmp224->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp222);_tmp224->toplevel=(int)_tmp223;_tmp224;});}static struct Cyc_Toc_Env*
Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env
_tmp226;struct _dyneither_ptr**_tmp227;struct _dyneither_ptr**_tmp228;struct Cyc_Toc_FallthruInfo*
_tmp229;struct Cyc_Dict_Dict _tmp22A;int _tmp22B;struct Cyc_Toc_Env*_tmp225=e;
_tmp226=*_tmp225;_tmp227=_tmp226.break_lab;_tmp228=_tmp226.continue_lab;_tmp229=
_tmp226.fallthru_info;_tmp22A=_tmp226.varmap;_tmp22B=_tmp226.toplevel;return({
struct Cyc_Toc_Env*_tmp22C=_region_malloc(r,sizeof(*_tmp22C));_tmp22C->break_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp227);_tmp22C->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp228);_tmp22C->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp229;_tmp22C->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp22A);_tmp22C->toplevel=(int)0;_tmp22C;});}static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(
struct _RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp22E;struct
_dyneither_ptr**_tmp22F;struct _dyneither_ptr**_tmp230;struct Cyc_Toc_FallthruInfo*
_tmp231;struct Cyc_Dict_Dict _tmp232;int _tmp233;struct Cyc_Toc_Env*_tmp22D=e;
_tmp22E=*_tmp22D;_tmp22F=_tmp22E.break_lab;_tmp230=_tmp22E.continue_lab;_tmp231=
_tmp22E.fallthru_info;_tmp232=_tmp22E.varmap;_tmp233=_tmp22E.toplevel;return({
struct Cyc_Toc_Env*_tmp234=_region_malloc(r,sizeof(*_tmp234));_tmp234->break_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp22F);_tmp234->continue_lab=(
struct _dyneither_ptr**)((struct _dyneither_ptr**)_tmp230);_tmp234->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp231;_tmp234->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp232);_tmp234->toplevel=(int)1;_tmp234;});}static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){{
union Cyc_Absyn_Nmspace_union _tmp235=(*x).f1;_LLE0: if((_tmp235.Rel_n).tag != 1)
goto _LLE2;_LLE1:({struct Cyc_String_pa_struct _tmp238;_tmp238.tag=0;_tmp238.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x));{void*
_tmp236[1]={& _tmp238};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp237="Toc::add_varmap on Rel_n: %s\n";
_tag_dyneither(_tmp237,sizeof(char),30);}),_tag_dyneither(_tmp236,sizeof(void*),
1));}});_LLE2:;_LLE3: goto _LLDF;_LLDF:;}{struct Cyc_Toc_Env _tmp23A;struct
_dyneither_ptr**_tmp23B;struct _dyneither_ptr**_tmp23C;struct Cyc_Toc_FallthruInfo*
_tmp23D;struct Cyc_Dict_Dict _tmp23E;int _tmp23F;struct Cyc_Toc_Env*_tmp239=e;
_tmp23A=*_tmp239;_tmp23B=_tmp23A.break_lab;_tmp23C=_tmp23A.continue_lab;_tmp23D=
_tmp23A.fallthru_info;_tmp23E=_tmp23A.varmap;_tmp23F=_tmp23A.toplevel;{struct Cyc_Dict_Dict
_tmp240=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*
v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))
Cyc_Dict_rshare)(r,_tmp23E),x,y);return({struct Cyc_Toc_Env*_tmp241=
_region_malloc(r,sizeof(*_tmp241));_tmp241->break_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp23B);_tmp241->continue_lab=(struct _dyneither_ptr**)((
struct _dyneither_ptr**)_tmp23C);_tmp241->fallthru_info=(struct Cyc_Toc_FallthruInfo*)
_tmp23D;_tmp241->varmap=(struct Cyc_Dict_Dict)_tmp240;_tmp241->toplevel=(int)
_tmp23F;_tmp241;});}}}static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct
_RegionHandle*r,struct Cyc_Toc_Env*e){struct Cyc_Toc_Env _tmp243;struct
_dyneither_ptr**_tmp244;struct _dyneither_ptr**_tmp245;struct Cyc_Toc_FallthruInfo*
_tmp246;struct Cyc_Dict_Dict _tmp247;int _tmp248;struct Cyc_Toc_Env*_tmp242=e;
_tmp243=*_tmp242;_tmp244=_tmp243.break_lab;_tmp245=_tmp243.continue_lab;_tmp246=
_tmp243.fallthru_info;_tmp247=_tmp243.varmap;_tmp248=_tmp243.toplevel;return({
struct Cyc_Toc_Env*_tmp249=_region_malloc(r,sizeof(*_tmp249));_tmp249->break_lab=(
struct _dyneither_ptr**)0;_tmp249->continue_lab=(struct _dyneither_ptr**)0;_tmp249->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)_tmp246;_tmp249->varmap=(struct Cyc_Dict_Dict)((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(
r,_tmp247);_tmp249->toplevel=(int)_tmp248;_tmp249;});}static struct Cyc_Toc_Env*
Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,
struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*
fallthru_binders,struct Cyc_Toc_Env*next_case_env){struct Cyc_List_List*
fallthru_vars=0;for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){
fallthru_vars=({struct Cyc_List_List*_tmp24A=_region_malloc(r,sizeof(*_tmp24A));
_tmp24A->hd=((struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name;_tmp24A->tl=
fallthru_vars;_tmp24A;});}fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(fallthru_vars);{struct Cyc_Toc_Env _tmp24C;struct
_dyneither_ptr**_tmp24D;struct _dyneither_ptr**_tmp24E;struct Cyc_Toc_FallthruInfo*
_tmp24F;struct Cyc_Dict_Dict _tmp250;int _tmp251;struct Cyc_Toc_Env*_tmp24B=e;
_tmp24C=*_tmp24B;_tmp24D=_tmp24C.break_lab;_tmp24E=_tmp24C.continue_lab;_tmp24F=
_tmp24C.fallthru_info;_tmp250=_tmp24C.varmap;_tmp251=_tmp24C.toplevel;{struct Cyc_Toc_Env
_tmp253;struct Cyc_Dict_Dict _tmp254;struct Cyc_Toc_Env*_tmp252=next_case_env;
_tmp253=*_tmp252;_tmp254=_tmp253.varmap;{struct Cyc_Toc_FallthruInfo*fi=({struct
Cyc_Toc_FallthruInfo*_tmp257=_region_malloc(r,sizeof(*_tmp257));_tmp257->label=
fallthru_l;_tmp257->binders=fallthru_vars;_tmp257->next_case_env=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp254);_tmp257;});
return({struct Cyc_Toc_Env*_tmp255=_region_malloc(r,sizeof(*_tmp255));_tmp255->break_lab=(
struct _dyneither_ptr**)({struct _dyneither_ptr**_tmp256=_region_malloc(r,sizeof(*
_tmp256));_tmp256[0]=break_l;_tmp256;});_tmp255->continue_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp24E);_tmp255->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)fi;_tmp255->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp250);_tmp255->toplevel=(
int)_tmp251;_tmp255;});}}}}static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){struct
Cyc_Toc_Env _tmp259;struct _dyneither_ptr**_tmp25A;struct _dyneither_ptr**_tmp25B;
struct Cyc_Toc_FallthruInfo*_tmp25C;struct Cyc_Dict_Dict _tmp25D;int _tmp25E;struct
Cyc_Toc_Env*_tmp258=e;_tmp259=*_tmp258;_tmp25A=_tmp259.break_lab;_tmp25B=_tmp259.continue_lab;
_tmp25C=_tmp259.fallthru_info;_tmp25D=_tmp259.varmap;_tmp25E=_tmp259.toplevel;
return({struct Cyc_Toc_Env*_tmp25F=_region_malloc(r,sizeof(*_tmp25F));_tmp25F->break_lab=(
struct _dyneither_ptr**)({struct _dyneither_ptr**_tmp260=_region_malloc(r,sizeof(*
_tmp260));_tmp260[0]=break_l;_tmp260;});_tmp25F->continue_lab=(struct
_dyneither_ptr**)((struct _dyneither_ptr**)_tmp25B);_tmp25F->fallthru_info=(
struct Cyc_Toc_FallthruInfo*)0;_tmp25F->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp25D);_tmp25F->toplevel=(
int)_tmp25E;_tmp25F;});}static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(
struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){struct Cyc_Toc_Env
_tmp262;struct _dyneither_ptr**_tmp263;struct _dyneither_ptr**_tmp264;struct Cyc_Toc_FallthruInfo*
_tmp265;struct Cyc_Dict_Dict _tmp266;int _tmp267;struct Cyc_Toc_Env*_tmp261=e;
_tmp262=*_tmp261;_tmp263=_tmp262.break_lab;_tmp264=_tmp262.continue_lab;_tmp265=
_tmp262.fallthru_info;_tmp266=_tmp262.varmap;_tmp267=_tmp262.toplevel;return({
struct Cyc_Toc_Env*_tmp268=_region_malloc(r,sizeof(*_tmp268));_tmp268->break_lab=(
struct _dyneither_ptr**)0;_tmp268->continue_lab=(struct _dyneither_ptr**)((struct
_dyneither_ptr**)_tmp264);_tmp268->fallthru_info=(struct Cyc_Toc_FallthruInfo*)({
struct Cyc_Toc_FallthruInfo*_tmp269=_region_malloc(r,sizeof(*_tmp269));_tmp269->label=
next_l;_tmp269->binders=0;_tmp269->next_case_env=((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp);
_tmp269;});_tmp268->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct
_RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp266);_tmp268->toplevel=(
int)_tmp267;_tmp268;});}static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Exp*e);static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
s);static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){void*_tmp26A=(void*)e->annot;
_LLE5: if(*((void**)_tmp26A)!= Cyc_CfFlowInfo_UnknownZ)goto _LLE7;_LLE6: return Cyc_Toc_is_nullable((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);_LLE7: if(*((void**)_tmp26A)
!= Cyc_CfFlowInfo_NotZero)goto _LLE9;_LLE8: return 0;_LLE9: if(_tmp26A != Cyc_CfFlowInfo_IsZero)
goto _LLEB;_LLEA:({void*_tmp26B=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp26C="dereference of NULL pointer";
_tag_dyneither(_tmp26C,sizeof(char),28);}),_tag_dyneither(_tmp26B,sizeof(void*),
0));});return 0;_LLEB: if(_tmp26A != Cyc_Absyn_EmptyAnnot)goto _LLED;_LLEC: return 0;
_LLED: if(*((void**)_tmp26A)!= Cyc_CfFlowInfo_HasTagCmps)goto _LLEF;_LLEE:({void*
_tmp26D=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp26E="compiler oddity: pointer compared to tag type";
_tag_dyneither(_tmp26E,sizeof(char),46);}),_tag_dyneither(_tmp26D,sizeof(void*),
0));});return Cyc_Toc_is_nullable((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_LLEF:;_LLF0:({void*_tmp26F=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp270="need_null_check";
_tag_dyneither(_tmp270,sizeof(char),16);}),_tag_dyneither(_tmp26F,sizeof(void*),
0));});_LLE4:;}static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp271=(void*)e->annot;struct Cyc_List_List*_tmp272;struct Cyc_List_List*
_tmp273;_LLF2: if(*((void**)_tmp271)!= Cyc_CfFlowInfo_UnknownZ)goto _LLF4;_tmp272=((
struct Cyc_CfFlowInfo_UnknownZ_struct*)_tmp271)->f1;_LLF3: return _tmp272;_LLF4: if(*((
void**)_tmp271)!= Cyc_CfFlowInfo_NotZero)goto _LLF6;_tmp273=((struct Cyc_CfFlowInfo_NotZero_struct*)
_tmp271)->f1;_LLF5: return _tmp273;_LLF6: if(_tmp271 != Cyc_CfFlowInfo_IsZero)goto
_LLF8;_LLF7:({void*_tmp274=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp275="dereference of NULL pointer";
_tag_dyneither(_tmp275,sizeof(char),28);}),_tag_dyneither(_tmp274,sizeof(void*),
0));});return 0;_LLF8: if(*((void**)_tmp271)!= Cyc_CfFlowInfo_HasTagCmps)goto _LLFA;
_LLF9: goto _LLFB;_LLFA: if(_tmp271 != Cyc_Absyn_EmptyAnnot)goto _LLFC;_LLFB: return 0;
_LLFC:;_LLFD:({void*_tmp276=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp277="get_relns";
_tag_dyneither(_tmp277,sizeof(char),10);}),_tag_dyneither(_tmp276,sizeof(void*),
0));});_LLF1:;}static int Cyc_Toc_check_const_array(unsigned int i,void*t){void*
_tmp278=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp279;struct Cyc_Absyn_PtrAtts
_tmp27A;struct Cyc_Absyn_Conref*_tmp27B;struct Cyc_Absyn_Conref*_tmp27C;struct Cyc_Absyn_ArrayInfo
_tmp27D;struct Cyc_Absyn_Exp*_tmp27E;_LLFF: if(_tmp278 <= (void*)4)goto _LL103;if(*((
int*)_tmp278)!= 4)goto _LL101;_tmp279=((struct Cyc_Absyn_PointerType_struct*)
_tmp278)->f1;_tmp27A=_tmp279.ptr_atts;_tmp27B=_tmp27A.bounds;_tmp27C=_tmp27A.zero_term;
_LL100: {void*_tmp27F=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp27B);struct
Cyc_Absyn_Exp*_tmp280;_LL106: if((int)_tmp27F != 0)goto _LL108;_LL107: return 0;
_LL108: if(_tmp27F <= (void*)1)goto _LL105;if(*((int*)_tmp27F)!= 0)goto _LL105;
_tmp280=((struct Cyc_Absyn_Upper_b_struct*)_tmp27F)->f1;_LL109: {unsigned int
_tmp282;int _tmp283;struct _tuple5 _tmp281=Cyc_Evexp_eval_const_uint_exp(_tmp280);
_tmp282=_tmp281.f1;_tmp283=_tmp281.f2;return _tmp283  && i <= _tmp282;}_LL105:;}
_LL101: if(*((int*)_tmp278)!= 7)goto _LL103;_tmp27D=((struct Cyc_Absyn_ArrayType_struct*)
_tmp278)->f1;_tmp27E=_tmp27D.num_elts;_LL102: if(_tmp27E == 0)return 0;{
unsigned int _tmp285;int _tmp286;struct _tuple5 _tmp284=Cyc_Evexp_eval_const_uint_exp((
struct Cyc_Absyn_Exp*)_tmp27E);_tmp285=_tmp284.f1;_tmp286=_tmp284.f2;return
_tmp286  && i <= _tmp285;}_LL103:;_LL104: return 0;_LLFE:;}static int Cyc_Toc_check_leq_size_var(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Vardecl*y){
for(0;relns != 0;relns=relns->tl){struct Cyc_CfFlowInfo_Reln*_tmp287=(struct Cyc_CfFlowInfo_Reln*)
relns->hd;if(_tmp287->vd != y)continue;{union Cyc_CfFlowInfo_RelnOp_union _tmp288=
_tmp287->rop;struct Cyc_Absyn_Vardecl*_tmp289;struct Cyc_Absyn_Vardecl*_tmp28A;
_LL10B: if((_tmp288.LessNumelts).tag != 2)goto _LL10D;_tmp289=(_tmp288.LessNumelts).f1;
_LL10C: _tmp28A=_tmp289;goto _LL10E;_LL10D: if((_tmp288.LessEqNumelts).tag != 4)goto
_LL10F;_tmp28A=(_tmp288.LessEqNumelts).f1;_LL10E: if(_tmp28A == v)return 1;else{
goto _LL10A;}_LL10F:;_LL110: continue;_LL10A:;}}return 0;}static int Cyc_Toc_check_leq_size(
struct Cyc_List_List*relns,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){{void*
_tmp28B=(void*)e->r;void*_tmp28C;struct Cyc_Absyn_Vardecl*_tmp28D;void*_tmp28E;
struct Cyc_Absyn_Vardecl*_tmp28F;void*_tmp290;struct Cyc_Absyn_Vardecl*_tmp291;
void*_tmp292;struct Cyc_Absyn_Vardecl*_tmp293;void*_tmp294;struct Cyc_List_List*
_tmp295;struct Cyc_List_List _tmp296;struct Cyc_Absyn_Exp*_tmp297;_LL112: if(*((int*)
_tmp28B)!= 1)goto _LL114;_tmp28C=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp28B)->f2;
if(_tmp28C <= (void*)1)goto _LL114;if(*((int*)_tmp28C)!= 4)goto _LL114;_tmp28D=((
struct Cyc_Absyn_Pat_b_struct*)_tmp28C)->f1;_LL113: _tmp28F=_tmp28D;goto _LL115;
_LL114: if(*((int*)_tmp28B)!= 1)goto _LL116;_tmp28E=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp28B)->f2;if(_tmp28E <= (void*)1)goto _LL116;if(*((int*)_tmp28E)!= 3)goto _LL116;
_tmp28F=((struct Cyc_Absyn_Local_b_struct*)_tmp28E)->f1;_LL115: _tmp291=_tmp28F;
goto _LL117;_LL116: if(*((int*)_tmp28B)!= 1)goto _LL118;_tmp290=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp28B)->f2;if(_tmp290 <= (void*)1)goto _LL118;if(*((int*)_tmp290)!= 0)goto _LL118;
_tmp291=((struct Cyc_Absyn_Global_b_struct*)_tmp290)->f1;_LL117: _tmp293=_tmp291;
goto _LL119;_LL118: if(*((int*)_tmp28B)!= 1)goto _LL11A;_tmp292=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp28B)->f2;if(_tmp292 <= (void*)1)goto _LL11A;if(*((int*)_tmp292)!= 2)goto _LL11A;
_tmp293=((struct Cyc_Absyn_Param_b_struct*)_tmp292)->f1;_LL119: if(_tmp293->escapes)
return 0;if(Cyc_Toc_check_leq_size_var(relns,v,_tmp293))return 1;goto _LL111;_LL11A:
if(*((int*)_tmp28B)!= 3)goto _LL11C;_tmp294=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp28B)->f1;if((int)_tmp294 != 19)goto _LL11C;_tmp295=((struct Cyc_Absyn_Primop_e_struct*)
_tmp28B)->f2;if(_tmp295 == 0)goto _LL11C;_tmp296=*_tmp295;_tmp297=(struct Cyc_Absyn_Exp*)
_tmp296.hd;_LL11B:{void*_tmp298=(void*)_tmp297->r;void*_tmp299;struct Cyc_Absyn_Vardecl*
_tmp29A;void*_tmp29B;struct Cyc_Absyn_Vardecl*_tmp29C;void*_tmp29D;struct Cyc_Absyn_Vardecl*
_tmp29E;void*_tmp29F;struct Cyc_Absyn_Vardecl*_tmp2A0;_LL11F: if(*((int*)_tmp298)
!= 1)goto _LL121;_tmp299=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp298)->f2;if(
_tmp299 <= (void*)1)goto _LL121;if(*((int*)_tmp299)!= 4)goto _LL121;_tmp29A=((
struct Cyc_Absyn_Pat_b_struct*)_tmp299)->f1;_LL120: _tmp29C=_tmp29A;goto _LL122;
_LL121: if(*((int*)_tmp298)!= 1)goto _LL123;_tmp29B=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp298)->f2;if(_tmp29B <= (void*)1)goto _LL123;if(*((int*)_tmp29B)!= 3)goto _LL123;
_tmp29C=((struct Cyc_Absyn_Local_b_struct*)_tmp29B)->f1;_LL122: _tmp29E=_tmp29C;
goto _LL124;_LL123: if(*((int*)_tmp298)!= 1)goto _LL125;_tmp29D=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp298)->f2;if(_tmp29D <= (void*)1)goto _LL125;if(*((int*)_tmp29D)!= 0)goto _LL125;
_tmp29E=((struct Cyc_Absyn_Global_b_struct*)_tmp29D)->f1;_LL124: _tmp2A0=_tmp29E;
goto _LL126;_LL125: if(*((int*)_tmp298)!= 1)goto _LL127;_tmp29F=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp298)->f2;if(_tmp29F <= (void*)1)goto _LL127;if(*((int*)_tmp29F)!= 2)goto _LL127;
_tmp2A0=((struct Cyc_Absyn_Param_b_struct*)_tmp29F)->f1;_LL126: return _tmp2A0 == v;
_LL127:;_LL128: goto _LL11E;_LL11E:;}goto _LL111;_LL11C:;_LL11D: goto _LL111;_LL111:;}
return 0;}static int Cyc_Toc_check_bounds(struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*
a,struct Cyc_Absyn_Exp*i){{void*_tmp2A1=(void*)a->r;void*_tmp2A2;struct Cyc_Absyn_Vardecl*
_tmp2A3;void*_tmp2A4;struct Cyc_Absyn_Vardecl*_tmp2A5;void*_tmp2A6;struct Cyc_Absyn_Vardecl*
_tmp2A7;void*_tmp2A8;struct Cyc_Absyn_Vardecl*_tmp2A9;_LL12A: if(*((int*)_tmp2A1)
!= 1)goto _LL12C;_tmp2A2=(void*)((struct Cyc_Absyn_Var_e_struct*)_tmp2A1)->f2;if(
_tmp2A2 <= (void*)1)goto _LL12C;if(*((int*)_tmp2A2)!= 4)goto _LL12C;_tmp2A3=((
struct Cyc_Absyn_Pat_b_struct*)_tmp2A2)->f1;_LL12B: _tmp2A5=_tmp2A3;goto _LL12D;
_LL12C: if(*((int*)_tmp2A1)!= 1)goto _LL12E;_tmp2A4=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2A1)->f2;if(_tmp2A4 <= (void*)1)goto _LL12E;if(*((int*)_tmp2A4)!= 3)goto _LL12E;
_tmp2A5=((struct Cyc_Absyn_Local_b_struct*)_tmp2A4)->f1;_LL12D: _tmp2A7=_tmp2A5;
goto _LL12F;_LL12E: if(*((int*)_tmp2A1)!= 1)goto _LL130;_tmp2A6=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2A1)->f2;if(_tmp2A6 <= (void*)1)goto _LL130;if(*((int*)_tmp2A6)!= 0)goto _LL130;
_tmp2A7=((struct Cyc_Absyn_Global_b_struct*)_tmp2A6)->f1;_LL12F: _tmp2A9=_tmp2A7;
goto _LL131;_LL130: if(*((int*)_tmp2A1)!= 1)goto _LL132;_tmp2A8=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2A1)->f2;if(_tmp2A8 <= (void*)1)goto _LL132;if(*((int*)_tmp2A8)!= 2)goto _LL132;
_tmp2A9=((struct Cyc_Absyn_Param_b_struct*)_tmp2A8)->f1;_LL131: if(_tmp2A9->escapes)
return 0;inner_loop: {void*_tmp2AA=(void*)i->r;void*_tmp2AB;struct Cyc_Absyn_Exp*
_tmp2AC;union Cyc_Absyn_Cnst_union _tmp2AD;void*_tmp2AE;int _tmp2AF;union Cyc_Absyn_Cnst_union
_tmp2B0;void*_tmp2B1;int _tmp2B2;union Cyc_Absyn_Cnst_union _tmp2B3;void*_tmp2B4;
int _tmp2B5;void*_tmp2B6;struct Cyc_List_List*_tmp2B7;struct Cyc_List_List _tmp2B8;
struct Cyc_Absyn_Exp*_tmp2B9;struct Cyc_List_List*_tmp2BA;struct Cyc_List_List
_tmp2BB;struct Cyc_Absyn_Exp*_tmp2BC;void*_tmp2BD;struct Cyc_Absyn_Vardecl*_tmp2BE;
void*_tmp2BF;struct Cyc_Absyn_Vardecl*_tmp2C0;void*_tmp2C1;struct Cyc_Absyn_Vardecl*
_tmp2C2;void*_tmp2C3;struct Cyc_Absyn_Vardecl*_tmp2C4;_LL135: if(*((int*)_tmp2AA)
!= 15)goto _LL137;_tmp2AB=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp2AA)->f1;
_tmp2AC=((struct Cyc_Absyn_Cast_e_struct*)_tmp2AA)->f2;_LL136: i=_tmp2AC;goto
inner_loop;_LL137: if(*((int*)_tmp2AA)!= 0)goto _LL139;_tmp2AD=((struct Cyc_Absyn_Const_e_struct*)
_tmp2AA)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)_tmp2AA)->f1).Int_c).tag != 2)
goto _LL139;_tmp2AE=(_tmp2AD.Int_c).f1;if((int)_tmp2AE != 2)goto _LL139;_tmp2AF=(
_tmp2AD.Int_c).f2;_LL138: _tmp2B2=_tmp2AF;goto _LL13A;_LL139: if(*((int*)_tmp2AA)!= 
0)goto _LL13B;_tmp2B0=((struct Cyc_Absyn_Const_e_struct*)_tmp2AA)->f1;if(((((
struct Cyc_Absyn_Const_e_struct*)_tmp2AA)->f1).Int_c).tag != 2)goto _LL13B;_tmp2B1=(
_tmp2B0.Int_c).f1;if((int)_tmp2B1 != 0)goto _LL13B;_tmp2B2=(_tmp2B0.Int_c).f2;
_LL13A: return _tmp2B2 >= 0  && Cyc_Toc_check_const_array((unsigned int)(_tmp2B2 + 1),(
void*)_tmp2A9->type);_LL13B: if(*((int*)_tmp2AA)!= 0)goto _LL13D;_tmp2B3=((struct
Cyc_Absyn_Const_e_struct*)_tmp2AA)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp2AA)->f1).Int_c).tag != 2)goto _LL13D;_tmp2B4=(_tmp2B3.Int_c).f1;if((int)
_tmp2B4 != 1)goto _LL13D;_tmp2B5=(_tmp2B3.Int_c).f2;_LL13C: return Cyc_Toc_check_const_array((
unsigned int)(_tmp2B5 + 1),(void*)_tmp2A9->type);_LL13D: if(*((int*)_tmp2AA)!= 3)
goto _LL13F;_tmp2B6=(void*)((struct Cyc_Absyn_Primop_e_struct*)_tmp2AA)->f1;if((
int)_tmp2B6 != 4)goto _LL13F;_tmp2B7=((struct Cyc_Absyn_Primop_e_struct*)_tmp2AA)->f2;
if(_tmp2B7 == 0)goto _LL13F;_tmp2B8=*_tmp2B7;_tmp2B9=(struct Cyc_Absyn_Exp*)_tmp2B8.hd;
_tmp2BA=_tmp2B8.tl;if(_tmp2BA == 0)goto _LL13F;_tmp2BB=*_tmp2BA;_tmp2BC=(struct Cyc_Absyn_Exp*)
_tmp2BB.hd;_LL13E: return Cyc_Toc_check_leq_size(relns,_tmp2A9,_tmp2BC);_LL13F: if(*((
int*)_tmp2AA)!= 1)goto _LL141;_tmp2BD=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2AA)->f2;if(_tmp2BD <= (void*)1)goto _LL141;if(*((int*)_tmp2BD)!= 4)goto _LL141;
_tmp2BE=((struct Cyc_Absyn_Pat_b_struct*)_tmp2BD)->f1;_LL140: _tmp2C0=_tmp2BE;goto
_LL142;_LL141: if(*((int*)_tmp2AA)!= 1)goto _LL143;_tmp2BF=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2AA)->f2;if(_tmp2BF <= (void*)1)goto _LL143;if(*((int*)_tmp2BF)!= 3)goto _LL143;
_tmp2C0=((struct Cyc_Absyn_Local_b_struct*)_tmp2BF)->f1;_LL142: _tmp2C2=_tmp2C0;
goto _LL144;_LL143: if(*((int*)_tmp2AA)!= 1)goto _LL145;_tmp2C1=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2AA)->f2;if(_tmp2C1 <= (void*)1)goto _LL145;if(*((int*)_tmp2C1)!= 0)goto _LL145;
_tmp2C2=((struct Cyc_Absyn_Global_b_struct*)_tmp2C1)->f1;_LL144: _tmp2C4=_tmp2C2;
goto _LL146;_LL145: if(*((int*)_tmp2AA)!= 1)goto _LL147;_tmp2C3=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp2AA)->f2;if(_tmp2C3 <= (void*)1)goto _LL147;if(*((int*)_tmp2C3)!= 2)goto _LL147;
_tmp2C4=((struct Cyc_Absyn_Param_b_struct*)_tmp2C3)->f1;_LL146: if(_tmp2C4->escapes)
return 0;{struct Cyc_List_List*_tmp2C5=relns;for(0;_tmp2C5 != 0;_tmp2C5=_tmp2C5->tl){
struct Cyc_CfFlowInfo_Reln*_tmp2C6=(struct Cyc_CfFlowInfo_Reln*)_tmp2C5->hd;if(
_tmp2C6->vd == _tmp2C4){union Cyc_CfFlowInfo_RelnOp_union _tmp2C7=_tmp2C6->rop;
struct Cyc_Absyn_Vardecl*_tmp2C8;struct Cyc_Absyn_Vardecl*_tmp2C9;unsigned int
_tmp2CA;_LL14A: if((_tmp2C7.LessNumelts).tag != 2)goto _LL14C;_tmp2C8=(_tmp2C7.LessNumelts).f1;
_LL14B: if(_tmp2A9 == _tmp2C8)return 1;else{goto _LL149;}_LL14C: if((_tmp2C7.LessVar).tag
!= 1)goto _LL14E;_tmp2C9=(_tmp2C7.LessVar).f1;_LL14D:{struct Cyc_List_List*_tmp2CB=
relns;for(0;_tmp2CB != 0;_tmp2CB=_tmp2CB->tl){struct Cyc_CfFlowInfo_Reln*_tmp2CC=(
struct Cyc_CfFlowInfo_Reln*)_tmp2CB->hd;if(_tmp2CC->vd == _tmp2C9){union Cyc_CfFlowInfo_RelnOp_union
_tmp2CD=_tmp2CC->rop;struct Cyc_Absyn_Vardecl*_tmp2CE;struct Cyc_Absyn_Vardecl*
_tmp2CF;unsigned int _tmp2D0;struct Cyc_Absyn_Vardecl*_tmp2D1;_LL153: if((_tmp2CD.LessEqNumelts).tag
!= 4)goto _LL155;_tmp2CE=(_tmp2CD.LessEqNumelts).f1;_LL154: _tmp2CF=_tmp2CE;goto
_LL156;_LL155: if((_tmp2CD.LessNumelts).tag != 2)goto _LL157;_tmp2CF=(_tmp2CD.LessNumelts).f1;
_LL156: if(_tmp2A9 == _tmp2CF)return 1;goto _LL152;_LL157: if((_tmp2CD.EqualConst).tag
!= 0)goto _LL159;_tmp2D0=(_tmp2CD.EqualConst).f1;_LL158: return Cyc_Toc_check_const_array(
_tmp2D0,(void*)_tmp2A9->type);_LL159: if((_tmp2CD.LessVar).tag != 1)goto _LL15B;
_tmp2D1=(_tmp2CD.LessVar).f1;_LL15A: if(Cyc_Toc_check_leq_size_var(relns,_tmp2A9,
_tmp2D1))return 1;goto _LL152;_LL15B:;_LL15C: goto _LL152;_LL152:;}}}goto _LL149;
_LL14E: if((_tmp2C7.LessConst).tag != 3)goto _LL150;_tmp2CA=(_tmp2C7.LessConst).f1;
_LL14F: return Cyc_Toc_check_const_array(_tmp2CA,(void*)_tmp2A9->type);_LL150:;
_LL151: goto _LL149;_LL149:;}}}goto _LL134;_LL147:;_LL148: goto _LL134;_LL134:;}goto
_LL129;_LL132:;_LL133: goto _LL129;_LL129:;}return 0;}static void*Cyc_Toc_get_c_typ(
struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*_tmp2D2=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp2D3="Missing type in primop ";
_tag_dyneither(_tmp2D3,sizeof(char),24);}),_tag_dyneither(_tmp2D2,sizeof(void*),
0));});return Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);}
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){if(e->topt == 0)({void*
_tmp2D4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp2D5="Missing type in primop ";_tag_dyneither(_tmp2D5,sizeof(char),
24);}),_tag_dyneither(_tmp2D4,sizeof(void*),0));});return(void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v;}static struct _tuple4*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*
e){return({struct _tuple4*_tmp2D6=_cycalloc(sizeof(*_tmp2D6));_tmp2D6->f1=Cyc_Toc_mt_tq;
_tmp2D6->f2=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);
_tmp2D6;});}static struct _tuple8*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*e){Cyc_Toc_exp_to_c(nv,e);return({struct _tuple8*_tmp2D7=
_cycalloc(sizeof(*_tmp2D7));_tmp2D7->f1=0;_tmp2D7->f2=e;_tmp2D7;});}static struct
Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct _tuple1*x,void*
struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int
is_atomic){struct Cyc_Absyn_Exp*eo;void*t;if(pointer){t=Cyc_Absyn_cstar_typ(
struct_typ,Cyc_Toc_mt_tq);{struct Cyc_Absyn_Exp*_tmp2D8=Cyc_Absyn_sizeofexp_exp(
Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(x,0),0),0);if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=(struct Cyc_Absyn_Exp*)(is_atomic?Cyc_Toc_malloc_atomic(_tmp2D8): Cyc_Toc_malloc_ptr(
_tmp2D8));else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)rgnopt;Cyc_Toc_exp_to_c(
nv,r);eo=(struct Cyc_Absyn_Exp*)Cyc_Toc_rmalloc_exp(r,_tmp2D8);}}}else{t=
struct_typ;eo=0;}return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(x,t,eo,s,0),0);}
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct
Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct
Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s);static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*
lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){int count=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(dles0)- 1;{struct Cyc_List_List*_tmp2D9=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;
_tmp2D9 != 0;_tmp2D9=_tmp2D9->tl){struct _tuple8 _tmp2DB;struct Cyc_List_List*
_tmp2DC;struct Cyc_Absyn_Exp*_tmp2DD;struct _tuple8*_tmp2DA=(struct _tuple8*)
_tmp2D9->hd;_tmp2DB=*_tmp2DA;_tmp2DC=_tmp2DB.f1;_tmp2DD=_tmp2DB.f2;{struct Cyc_Absyn_Exp*
e_index;if(_tmp2DC == 0)e_index=Cyc_Absyn_signed_int_exp(count --,0);else{if(
_tmp2DC->tl != 0)({void*_tmp2DE=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp2DF="multiple designators in array";
_tag_dyneither(_tmp2DF,sizeof(char),30);}),_tag_dyneither(_tmp2DE,sizeof(void*),
0));});{void*_tmp2E0=(void*)_tmp2DC->hd;void*_tmp2E1=_tmp2E0;struct Cyc_Absyn_Exp*
_tmp2E2;_LL15E: if(*((int*)_tmp2E1)!= 0)goto _LL160;_tmp2E2=((struct Cyc_Absyn_ArrayElement_struct*)
_tmp2E1)->f1;_LL15F: Cyc_Toc_exp_to_c(nv,_tmp2E2);e_index=_tmp2E2;goto _LL15D;
_LL160: if(*((int*)_tmp2E1)!= 1)goto _LL15D;_LL161:({void*_tmp2E3=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*
_tmp2E4="field name designators in array";_tag_dyneither(_tmp2E4,sizeof(char),32);}),
_tag_dyneither(_tmp2E3,sizeof(void*),0));});_LL15D:;}}{struct Cyc_Absyn_Exp*lval=
Cyc_Absyn_subscript_exp(lhs,e_index,0);void*_tmp2E5=(void*)_tmp2DD->r;struct Cyc_List_List*
_tmp2E6;struct Cyc_Absyn_Vardecl*_tmp2E7;struct Cyc_Absyn_Exp*_tmp2E8;struct Cyc_Absyn_Exp*
_tmp2E9;int _tmp2EA;void*_tmp2EB;struct Cyc_List_List*_tmp2EC;_LL163: if(*((int*)
_tmp2E5)!= 28)goto _LL165;_tmp2E6=((struct Cyc_Absyn_Array_e_struct*)_tmp2E5)->f1;
_LL164: s=Cyc_Toc_init_array(nv,lval,_tmp2E6,s);goto _LL162;_LL165: if(*((int*)
_tmp2E5)!= 29)goto _LL167;_tmp2E7=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2E5)->f1;_tmp2E8=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2E5)->f2;
_tmp2E9=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2E5)->f3;_tmp2EA=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp2E5)->f4;_LL166: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp2E7,_tmp2E8,_tmp2E9,_tmp2EA,s,0);goto _LL162;_LL167: if(*((int*)
_tmp2E5)!= 31)goto _LL169;_tmp2EB=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp2E5)->f1;_tmp2EC=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp2E5)->f2;_LL168:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp2EB,_tmp2EC,s);goto _LL162;_LL169:;_LL16A:
Cyc_Toc_exp_to_c(nv,_tmp2DD);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
lhs,e_index,0),_tmp2DD,0),s,0);goto _LL162;_LL162:;}}}}return s;}static struct Cyc_Absyn_Stmt*
Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,struct
Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,
struct Cyc_Absyn_Stmt*s,int e1_already_translated){struct _tuple1*_tmp2ED=vd->name;
void*_tmp2EE=Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)_check_null(e2->topt))->v);
if(!e1_already_translated)Cyc_Toc_exp_to_c(nv,e1);{struct _RegionHandle _tmp2EF=
_new_region("r2");struct _RegionHandle*r2=& _tmp2EF;_push_region(r2);{struct Cyc_Toc_Env*
nv2=Cyc_Toc_add_varmap(r2,nv,_tmp2ED,Cyc_Absyn_varb_exp(_tmp2ED,(void*)({struct
Cyc_Absyn_Local_b_struct*_tmp2F9=_cycalloc(sizeof(*_tmp2F9));_tmp2F9[0]=({struct
Cyc_Absyn_Local_b_struct _tmp2FA;_tmp2FA.tag=3;_tmp2FA.f1=vd;_tmp2FA;});_tmp2F9;}),
0));struct _tuple1*max=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(
Cyc_Absyn_var_exp(_tmp2ED,0),Cyc_Absyn_signed_int_exp(0,0),0);struct Cyc_Absyn_Exp*
eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp2ED,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp2ED,0),0);
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp2ED,0),
0);struct Cyc_Absyn_Stmt*body;{void*_tmp2F0=(void*)e2->r;struct Cyc_List_List*
_tmp2F1;struct Cyc_Absyn_Vardecl*_tmp2F2;struct Cyc_Absyn_Exp*_tmp2F3;struct Cyc_Absyn_Exp*
_tmp2F4;int _tmp2F5;void*_tmp2F6;struct Cyc_List_List*_tmp2F7;_LL16C: if(*((int*)
_tmp2F0)!= 28)goto _LL16E;_tmp2F1=((struct Cyc_Absyn_Array_e_struct*)_tmp2F0)->f1;
_LL16D: body=Cyc_Toc_init_array(nv2,lval,_tmp2F1,Cyc_Toc_skip_stmt_dl());goto
_LL16B;_LL16E: if(*((int*)_tmp2F0)!= 29)goto _LL170;_tmp2F2=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp2F0)->f1;_tmp2F3=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2F0)->f2;
_tmp2F4=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp2F0)->f3;_tmp2F5=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp2F0)->f4;_LL16F: body=Cyc_Toc_init_comprehension(
nv2,lval,_tmp2F2,_tmp2F3,_tmp2F4,_tmp2F5,Cyc_Toc_skip_stmt_dl(),0);goto _LL16B;
_LL170: if(*((int*)_tmp2F0)!= 31)goto _LL172;_tmp2F6=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp2F0)->f1;_tmp2F7=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp2F0)->f2;_LL171:
body=Cyc_Toc_init_anon_struct(nv,lval,_tmp2F6,_tmp2F7,Cyc_Toc_skip_stmt_dl());
goto _LL16B;_LL172:;_LL173: Cyc_Toc_exp_to_c(nv2,e2);body=Cyc_Absyn_assign_stmt(
lval,e2,0);goto _LL16B;_LL16B:;}{struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,
ec,body,0);if(zero_term){struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(
Cyc_Absyn_new_exp((void*)lhs->r,0),Cyc_Absyn_var_exp(max,0),0),Cyc_Toc_cast_it(
_tmp2EE,Cyc_Absyn_uint_exp(0,0)),0);s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(
ex,0),0);}{struct Cyc_Absyn_Stmt*_tmp2F8=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(
max,Cyc_Absyn_uint_typ,(struct Cyc_Absyn_Exp*)e1,Cyc_Absyn_declare_stmt(_tmp2ED,
Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp2F8;}}};
_pop_region(r2);}}static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*
s){{struct Cyc_List_List*_tmp2FB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(dles);for(0;_tmp2FB != 0;_tmp2FB=_tmp2FB->tl){struct _tuple8 _tmp2FD;
struct Cyc_List_List*_tmp2FE;struct Cyc_Absyn_Exp*_tmp2FF;struct _tuple8*_tmp2FC=(
struct _tuple8*)_tmp2FB->hd;_tmp2FD=*_tmp2FC;_tmp2FE=_tmp2FD.f1;_tmp2FF=_tmp2FD.f2;
if(_tmp2FE == 0)({void*_tmp300=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp301="empty designator list";
_tag_dyneither(_tmp301,sizeof(char),22);}),_tag_dyneither(_tmp300,sizeof(void*),
0));});if(_tmp2FE->tl != 0)({void*_tmp302=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp303="too many designators in anonymous struct";
_tag_dyneither(_tmp303,sizeof(char),41);}),_tag_dyneither(_tmp302,sizeof(void*),
0));});{void*_tmp304=(void*)_tmp2FE->hd;struct _dyneither_ptr*_tmp305;_LL175: if(*((
int*)_tmp304)!= 1)goto _LL177;_tmp305=((struct Cyc_Absyn_FieldName_struct*)_tmp304)->f1;
_LL176: {struct Cyc_Absyn_Exp*lval=Cyc_Absyn_aggrmember_exp(lhs,_tmp305,0);{void*
_tmp306=(void*)_tmp2FF->r;struct Cyc_List_List*_tmp307;struct Cyc_Absyn_Vardecl*
_tmp308;struct Cyc_Absyn_Exp*_tmp309;struct Cyc_Absyn_Exp*_tmp30A;int _tmp30B;void*
_tmp30C;struct Cyc_List_List*_tmp30D;_LL17A: if(*((int*)_tmp306)!= 28)goto _LL17C;
_tmp307=((struct Cyc_Absyn_Array_e_struct*)_tmp306)->f1;_LL17B: s=Cyc_Toc_init_array(
nv,lval,_tmp307,s);goto _LL179;_LL17C: if(*((int*)_tmp306)!= 29)goto _LL17E;_tmp308=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp306)->f1;_tmp309=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp306)->f2;_tmp30A=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp306)->f3;
_tmp30B=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp306)->f4;_LL17D: s=Cyc_Toc_init_comprehension(
nv,lval,_tmp308,_tmp309,_tmp30A,_tmp30B,s,0);goto _LL179;_LL17E: if(*((int*)
_tmp306)!= 31)goto _LL180;_tmp30C=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp306)->f1;_tmp30D=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp306)->f2;_LL17F:
s=Cyc_Toc_init_anon_struct(nv,lval,_tmp30C,_tmp30D,s);goto _LL179;_LL180:;_LL181:
Cyc_Toc_exp_to_c(nv,_tmp2FF);if(Cyc_Toc_is_poly_field(struct_type,_tmp305))
_tmp2FF=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp2FF);s=Cyc_Absyn_seq_stmt(
Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp2FF,0),0),s,0);goto _LL179;
_LL179:;}goto _LL174;}_LL177:;_LL178:({void*_tmp30E=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp30F="array designator in struct";
_tag_dyneither(_tmp30F,sizeof(char),27);}),_tag_dyneither(_tmp30E,sizeof(void*),
0));});_LL174:;}}}return s;}static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct
Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
struct _RegionHandle _tmp310=_new_region("r");struct _RegionHandle*r=& _tmp310;
_push_region(r);{struct Cyc_List_List*_tmp311=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))
Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);void*_tmp312=Cyc_Toc_add_tuple_type(
_tmp311);struct _tuple1*_tmp313=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp314=
Cyc_Absyn_var_exp(_tmp313,0);struct Cyc_Absyn_Stmt*_tmp315=Cyc_Absyn_exp_stmt(
_tmp314,0);struct Cyc_Absyn_Exp*(*_tmp316)(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
int is_atomic=1;struct Cyc_List_List*_tmp317=((struct Cyc_List_List*(*)(struct
_RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);{int i=((int(*)(struct
Cyc_List_List*x))Cyc_List_length)(_tmp317);for(0;_tmp317 != 0;(_tmp317=_tmp317->tl,
-- i)){struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp317->hd;struct Cyc_Absyn_Exp*
lval=_tmp316(_tmp314,Cyc_Absyn_fieldname(i),0);is_atomic=is_atomic  && Cyc_Toc_atomic_typ((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v);{void*_tmp318=(void*)e->r;
struct Cyc_List_List*_tmp319;struct Cyc_Absyn_Vardecl*_tmp31A;struct Cyc_Absyn_Exp*
_tmp31B;struct Cyc_Absyn_Exp*_tmp31C;int _tmp31D;_LL183: if(*((int*)_tmp318)!= 28)
goto _LL185;_tmp319=((struct Cyc_Absyn_Array_e_struct*)_tmp318)->f1;_LL184: _tmp315=
Cyc_Toc_init_array(nv,lval,_tmp319,_tmp315);goto _LL182;_LL185: if(*((int*)_tmp318)
!= 29)goto _LL187;_tmp31A=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp318)->f1;
_tmp31B=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp318)->f2;_tmp31C=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp318)->f3;_tmp31D=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp318)->f4;_LL186: _tmp315=Cyc_Toc_init_comprehension(nv,lval,_tmp31A,_tmp31B,
_tmp31C,_tmp31D,_tmp315,0);goto _LL182;_LL187:;_LL188: Cyc_Toc_exp_to_c(nv,e);
_tmp315=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp316(
_tmp314,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp315,0);goto _LL182;_LL182:;}}}{
struct Cyc_Absyn_Exp*_tmp31E=Cyc_Toc_make_struct(nv,_tmp313,_tmp312,_tmp315,
pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp31E;}};_pop_region(r);}
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*
struct_type,int has_exists,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*
dles,struct _tuple1*tdn){struct _tuple1*_tmp31F=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp320=Cyc_Absyn_var_exp(_tmp31F,0);struct Cyc_Absyn_Stmt*_tmp321=Cyc_Absyn_exp_stmt(
_tmp320,0);struct Cyc_Absyn_Exp*(*_tmp322)(struct Cyc_Absyn_Exp*,struct
_dyneither_ptr*,struct Cyc_Position_Segment*)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Absyn_aggrmember_exp;
void*_tmp323=Cyc_Toc_aggrdecl_type(tdn,Cyc_Absyn_strctq);int is_atomic=1;struct
Cyc_Absyn_Aggrdecl*ad;{void*_tmp324=Cyc_Tcutil_compress(struct_type);struct Cyc_Absyn_AggrInfo
_tmp325;union Cyc_Absyn_AggrInfoU_union _tmp326;_LL18A: if(_tmp324 <= (void*)4)goto
_LL18C;if(*((int*)_tmp324)!= 10)goto _LL18C;_tmp325=((struct Cyc_Absyn_AggrType_struct*)
_tmp324)->f1;_tmp326=_tmp325.aggr_info;_LL18B: ad=Cyc_Absyn_get_known_aggrdecl(
_tmp326);goto _LL189;_LL18C:;_LL18D:({void*_tmp327=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp328="init_struct: bad struct type";
_tag_dyneither(_tmp328,sizeof(char),29);}),_tag_dyneither(_tmp327,sizeof(void*),
0));});_LL189:;}{struct _RegionHandle _tmp329=_new_region("r");struct _RegionHandle*
r=& _tmp329;_push_region(r);{struct Cyc_List_List*_tmp32A=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp32A
!= 0;_tmp32A=_tmp32A->tl){struct _tuple8 _tmp32C;struct Cyc_List_List*_tmp32D;
struct Cyc_Absyn_Exp*_tmp32E;struct _tuple8*_tmp32B=(struct _tuple8*)_tmp32A->hd;
_tmp32C=*_tmp32B;_tmp32D=_tmp32C.f1;_tmp32E=_tmp32C.f2;is_atomic=is_atomic  && 
Cyc_Toc_atomic_typ((void*)((struct Cyc_Core_Opt*)_check_null(_tmp32E->topt))->v);
if(_tmp32D == 0)({void*_tmp32F=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp330="empty designator list";
_tag_dyneither(_tmp330,sizeof(char),22);}),_tag_dyneither(_tmp32F,sizeof(void*),
0));});if(_tmp32D->tl != 0){struct _tuple1*_tmp331=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
_tmp332=Cyc_Absyn_var_exp(_tmp331,0);for(0;_tmp32D != 0;_tmp32D=_tmp32D->tl){void*
_tmp333=(void*)_tmp32D->hd;struct _dyneither_ptr*_tmp334;_LL18F: if(*((int*)
_tmp333)!= 1)goto _LL191;_tmp334=((struct Cyc_Absyn_FieldName_struct*)_tmp333)->f1;
_LL190: if(Cyc_Toc_is_poly_field(struct_type,_tmp334))_tmp332=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp332);_tmp321=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp322(_tmp320,_tmp334,0),_tmp332,0),0),_tmp321,0);goto _LL18E;_LL191:;_LL192:({
void*_tmp335=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp336="array designator in struct";_tag_dyneither(_tmp336,sizeof(
char),27);}),_tag_dyneither(_tmp335,sizeof(void*),0));});_LL18E:;}Cyc_Toc_exp_to_c(
nv,_tmp32E);_tmp321=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp332,_tmp32E,0),0),_tmp321,0);}else{void*_tmp337=(void*)_tmp32D->hd;struct
_dyneither_ptr*_tmp338;_LL194: if(*((int*)_tmp337)!= 1)goto _LL196;_tmp338=((
struct Cyc_Absyn_FieldName_struct*)_tmp337)->f1;_LL195: {struct Cyc_Absyn_Exp*lval=
_tmp322(_tmp320,_tmp338,0);{void*_tmp339=(void*)_tmp32E->r;struct Cyc_List_List*
_tmp33A;struct Cyc_Absyn_Vardecl*_tmp33B;struct Cyc_Absyn_Exp*_tmp33C;struct Cyc_Absyn_Exp*
_tmp33D;int _tmp33E;void*_tmp33F;struct Cyc_List_List*_tmp340;_LL199: if(*((int*)
_tmp339)!= 28)goto _LL19B;_tmp33A=((struct Cyc_Absyn_Array_e_struct*)_tmp339)->f1;
_LL19A: _tmp321=Cyc_Toc_init_array(nv,lval,_tmp33A,_tmp321);goto _LL198;_LL19B: if(*((
int*)_tmp339)!= 29)goto _LL19D;_tmp33B=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp339)->f1;_tmp33C=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp339)->f2;
_tmp33D=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp339)->f3;_tmp33E=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp339)->f4;_LL19C: _tmp321=Cyc_Toc_init_comprehension(
nv,lval,_tmp33B,_tmp33C,_tmp33D,_tmp33E,_tmp321,0);goto _LL198;_LL19D: if(*((int*)
_tmp339)!= 31)goto _LL19F;_tmp33F=(void*)((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp339)->f1;_tmp340=((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp339)->f2;_LL19E:
_tmp321=Cyc_Toc_init_anon_struct(nv,lval,_tmp33F,_tmp340,_tmp321);goto _LL198;
_LL19F:;_LL1A0: {int was_ptr_type=Cyc_Tcutil_is_pointer_type((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp32E->topt))->v);Cyc_Toc_exp_to_c(nv,_tmp32E);{struct Cyc_Absyn_Aggrfield*
_tmp341=Cyc_Absyn_lookup_decl_field(ad,_tmp338);if(Cyc_Toc_is_poly_field(
struct_type,_tmp338) && !was_ptr_type)_tmp32E=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),
_tmp32E);if(has_exists)_tmp32E=Cyc_Toc_cast_it((void*)((struct Cyc_Absyn_Aggrfield*)
_check_null(_tmp341))->type,_tmp32E);_tmp321=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(
Cyc_Absyn_assign_exp(lval,_tmp32E,0),0),_tmp321,0);goto _LL198;}}_LL198:;}goto
_LL193;}_LL196:;_LL197:({void*_tmp342=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp343="array designator in struct";
_tag_dyneither(_tmp343,sizeof(char),27);}),_tag_dyneither(_tmp342,sizeof(void*),
0));});_LL193:;}}}{struct Cyc_Absyn_Exp*_tmp344=Cyc_Toc_make_struct(nv,_tmp31F,
_tmp323,_tmp321,pointer,rgnopt,is_atomic);_npop_handler(0);return _tmp344;};
_pop_region(r);}}struct _tuple10{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct
_tuple10*pr){return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}static struct
Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){return Cyc_Absyn_address_exp(
e1,0);}static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,void*
incr){return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_struct*
_tmp345=_cycalloc(sizeof(*_tmp345));_tmp345[0]=({struct Cyc_Absyn_Increment_e_struct
_tmp346;_tmp346.tag=5;_tmp346.f1=e1;_tmp346.f2=(void*)incr;_tmp346;});_tmp345;}),
0);}static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*
fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);static void
Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*,void*),void*f_env){void*_tmp347=(void*)e1->r;struct Cyc_Absyn_Stmt*
_tmp348;void*_tmp349;struct Cyc_Absyn_Exp*_tmp34A;struct Cyc_Absyn_Exp*_tmp34B;
struct _dyneither_ptr*_tmp34C;_LL1A2: if(*((int*)_tmp347)!= 38)goto _LL1A4;_tmp348=((
struct Cyc_Absyn_StmtExp_e_struct*)_tmp347)->f1;_LL1A3: Cyc_Toc_lvalue_assign_stmt(
_tmp348,fs,f,f_env);goto _LL1A1;_LL1A4: if(*((int*)_tmp347)!= 15)goto _LL1A6;
_tmp349=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp347)->f1;_tmp34A=((struct Cyc_Absyn_Cast_e_struct*)
_tmp347)->f2;_LL1A5: Cyc_Toc_lvalue_assign(_tmp34A,fs,f,f_env);goto _LL1A1;_LL1A6:
if(*((int*)_tmp347)!= 23)goto _LL1A8;_tmp34B=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp347)->f1;_tmp34C=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp347)->f2;_LL1A7:(
void*)(e1->r=(void*)((void*)_tmp34B->r));Cyc_Toc_lvalue_assign(e1,(struct Cyc_List_List*)({
struct Cyc_List_List*_tmp34D=_cycalloc(sizeof(*_tmp34D));_tmp34D->hd=_tmp34C;
_tmp34D->tl=fs;_tmp34D;}),f,f_env);goto _LL1A1;_LL1A8:;_LL1A9: {struct Cyc_Absyn_Exp*
e1_copy=Cyc_Absyn_copy_exp(e1);for(0;fs != 0;fs=fs->tl){e1_copy=Cyc_Absyn_aggrmember_exp(
e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}(void*)(e1->r=(void*)((void*)(
f(e1_copy,f_env))->r));goto _LL1A1;}_LL1A1:;}static void Cyc_Toc_lvalue_assign_stmt(
struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,
void*),void*f_env){void*_tmp34E=(void*)s->r;struct Cyc_Absyn_Exp*_tmp34F;struct
Cyc_Absyn_Decl*_tmp350;struct Cyc_Absyn_Stmt*_tmp351;struct Cyc_Absyn_Stmt*_tmp352;
_LL1AB: if(_tmp34E <= (void*)1)goto _LL1B1;if(*((int*)_tmp34E)!= 0)goto _LL1AD;
_tmp34F=((struct Cyc_Absyn_Exp_s_struct*)_tmp34E)->f1;_LL1AC: Cyc_Toc_lvalue_assign(
_tmp34F,fs,f,f_env);goto _LL1AA;_LL1AD: if(*((int*)_tmp34E)!= 11)goto _LL1AF;
_tmp350=((struct Cyc_Absyn_Decl_s_struct*)_tmp34E)->f1;_tmp351=((struct Cyc_Absyn_Decl_s_struct*)
_tmp34E)->f2;_LL1AE: Cyc_Toc_lvalue_assign_stmt(_tmp351,fs,f,f_env);goto _LL1AA;
_LL1AF: if(*((int*)_tmp34E)!= 1)goto _LL1B1;_tmp352=((struct Cyc_Absyn_Seq_s_struct*)
_tmp34E)->f2;_LL1B0: Cyc_Toc_lvalue_assign_stmt(_tmp352,fs,f,f_env);goto _LL1AA;
_LL1B1:;_LL1B2:({struct Cyc_String_pa_struct _tmp355;_tmp355.tag=0;_tmp355.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));{void*
_tmp353[1]={& _tmp355};Cyc_Toc_toc_impos(({const char*_tmp354="lvalue_assign_stmt: %s";
_tag_dyneither(_tmp354,sizeof(char),23);}),_tag_dyneither(_tmp353,sizeof(void*),
1));}});_LL1AA:;}static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*
r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){struct Cyc_List_List*
result;struct Cyc_List_List*prev;if(x == 0)return 0;result=({struct Cyc_List_List*
_tmp356=_region_malloc(r2,sizeof(*_tmp356));_tmp356->hd=(void*)f((void*)x->hd,
env);_tmp356->tl=0;_tmp356;});prev=result;for(x=x->tl;x != 0;x=x->tl){((struct Cyc_List_List*)
_check_null(prev))->tl=({struct Cyc_List_List*_tmp357=_region_malloc(r2,sizeof(*
_tmp357));_tmp357->hd=(void*)f((void*)x->hd,env);_tmp357->tl=0;_tmp357;});prev=((
struct Cyc_List_List*)_check_null(prev))->tl;}return result;}static struct Cyc_List_List*
Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){return Cyc_Toc_rmap_2c(
Cyc_Core_heap_region,f,env,x);}static struct _tuple8*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*
e){return({struct _tuple8*_tmp358=_cycalloc(sizeof(*_tmp358));_tmp358->f1=0;
_tmp358->f2=e;_tmp358;});}static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*
t){void*_tmp359=Cyc_Tcutil_compress(t);struct Cyc_Absyn_PtrInfo _tmp35A;_LL1B4: if(
_tmp359 <= (void*)4)goto _LL1B6;if(*((int*)_tmp359)!= 4)goto _LL1B6;_tmp35A=((
struct Cyc_Absyn_PointerType_struct*)_tmp359)->f1;_LL1B5: return _tmp35A;_LL1B6:;
_LL1B7:({void*_tmp35B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp35C="get_ptr_typ: not a pointer!";
_tag_dyneither(_tmp35C,sizeof(char),28);}),_tag_dyneither(_tmp35B,sizeof(void*),
0));});_LL1B3:;}static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){struct
Cyc_Absyn_Exp*res;{void*_tmp35D=Cyc_Tcutil_compress(t);void*_tmp35E;void*_tmp35F;
void*_tmp360;void*_tmp361;void*_tmp362;void*_tmp363;void*_tmp364;void*_tmp365;
void*_tmp366;void*_tmp367;_LL1B9: if(_tmp35D <= (void*)4)goto _LL1C9;if(*((int*)
_tmp35D)!= 4)goto _LL1BB;_LL1BA: res=Cyc_Absyn_null_exp(0);goto _LL1B8;_LL1BB: if(*((
int*)_tmp35D)!= 5)goto _LL1BD;_tmp35E=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp35D)->f1;_tmp35F=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp35D)->f2;if((
int)_tmp35F != 0)goto _LL1BD;_LL1BC: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmp368;(_tmp368.Char_c).tag=0;(_tmp368.Char_c).f1=(
void*)_tmp35E;(_tmp368.Char_c).f2='\000';_tmp368;}),0);goto _LL1B8;_LL1BD: if(*((
int*)_tmp35D)!= 5)goto _LL1BF;_tmp360=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp35D)->f1;_tmp361=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp35D)->f2;if((
int)_tmp361 != 1)goto _LL1BF;_LL1BE: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({
union Cyc_Absyn_Cnst_union _tmp369;(_tmp369.Short_c).tag=1;(_tmp369.Short_c).f1=(
void*)_tmp360;(_tmp369.Short_c).f2=0;_tmp369;}),0);goto _LL1B8;_LL1BF: if(*((int*)
_tmp35D)!= 12)goto _LL1C1;_LL1C0: goto _LL1C2;_LL1C1: if(*((int*)_tmp35D)!= 13)goto
_LL1C3;_LL1C2: _tmp362=(void*)1;goto _LL1C4;_LL1C3: if(*((int*)_tmp35D)!= 5)goto
_LL1C5;_tmp362=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp35D)->f1;_tmp363=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp35D)->f2;if((int)_tmp363 != 2)goto
_LL1C5;_LL1C4: _tmp364=_tmp362;goto _LL1C6;_LL1C5: if(*((int*)_tmp35D)!= 5)goto
_LL1C7;_tmp364=(void*)((struct Cyc_Absyn_IntType_struct*)_tmp35D)->f1;_tmp365=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp35D)->f2;if((int)_tmp365 != 3)goto
_LL1C7;_LL1C6: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp36A;(_tmp36A.Int_c).tag=2;(_tmp36A.Int_c).f1=(void*)_tmp364;(_tmp36A.Int_c).f2=
0;_tmp36A;}),0);goto _LL1B8;_LL1C7: if(*((int*)_tmp35D)!= 5)goto _LL1C9;_tmp366=(
void*)((struct Cyc_Absyn_IntType_struct*)_tmp35D)->f1;_tmp367=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp35D)->f2;if((int)_tmp367 != 4)goto _LL1C9;_LL1C8: res=Cyc_Absyn_const_exp((
union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union _tmp36B;(_tmp36B.LongLong_c).tag=
3;(_tmp36B.LongLong_c).f1=(void*)_tmp366;(_tmp36B.LongLong_c).f2=(long long)0;
_tmp36B;}),0);goto _LL1B8;_LL1C9: if((int)_tmp35D != 1)goto _LL1CB;_LL1CA: goto _LL1CC;
_LL1CB: if(_tmp35D <= (void*)4)goto _LL1CD;if(*((int*)_tmp35D)!= 6)goto _LL1CD;
_LL1CC: res=Cyc_Absyn_const_exp((union Cyc_Absyn_Cnst_union)({union Cyc_Absyn_Cnst_union
_tmp36C;(_tmp36C.Float_c).tag=4;(_tmp36C.Float_c).f1=({const char*_tmp36D="0.0";
_tag_dyneither(_tmp36D,sizeof(char),4);});_tmp36C;}),0);goto _LL1B8;_LL1CD:;
_LL1CE:({struct Cyc_String_pa_struct _tmp370;_tmp370.tag=0;_tmp370.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));{void*_tmp36E[1]={&
_tmp370};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp36F="found non-zero type %s in generate_zero";_tag_dyneither(
_tmp36F,sizeof(char),40);}),_tag_dyneither(_tmp36E,sizeof(void*),1));}});_LL1B8:;}
res->topt=({struct Cyc_Core_Opt*_tmp371=_cycalloc(sizeof(*_tmp371));_tmp371->v=(
void*)t;_tmp371;});return res;}static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct
Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){void*fat_ptr_type=
Cyc_Absyn_dyneither_typ(elt_type,(void*)2,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp372=Cyc_Toc_typ_to_c(elt_type);void*_tmp373=Cyc_Toc_typ_to_c(
fat_ptr_type);void*_tmp374=Cyc_Absyn_cstar_typ(_tmp372,Cyc_Toc_mt_tq);struct Cyc_Core_Opt*
_tmp375=({struct Cyc_Core_Opt*_tmp3B2=_cycalloc(sizeof(*_tmp3B2));_tmp3B2->v=(
void*)_tmp374;_tmp3B2;});struct Cyc_Absyn_Exp*xinit;{void*_tmp376=(void*)e1->r;
struct Cyc_Absyn_Exp*_tmp377;struct Cyc_Absyn_Exp*_tmp378;struct Cyc_Absyn_Exp*
_tmp379;_LL1D0: if(*((int*)_tmp376)!= 22)goto _LL1D2;_tmp377=((struct Cyc_Absyn_Deref_e_struct*)
_tmp376)->f1;_LL1D1: if(!is_dyneither){_tmp377=Cyc_Toc_cast_it(fat_ptr_type,
_tmp377);_tmp377->topt=({struct Cyc_Core_Opt*_tmp37A=_cycalloc(sizeof(*_tmp37A));
_tmp37A->v=(void*)fat_ptr_type;_tmp37A;});}Cyc_Toc_exp_to_c(nv,_tmp377);xinit=
_tmp377;goto _LL1CF;_LL1D2: if(*((int*)_tmp376)!= 25)goto _LL1D4;_tmp378=((struct
Cyc_Absyn_Subscript_e_struct*)_tmp376)->f1;_tmp379=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp376)->f2;_LL1D3: if(!is_dyneither){_tmp378=Cyc_Toc_cast_it(fat_ptr_type,
_tmp378);_tmp378->topt=({struct Cyc_Core_Opt*_tmp37B=_cycalloc(sizeof(*_tmp37B));
_tmp37B->v=(void*)fat_ptr_type;_tmp37B;});}Cyc_Toc_exp_to_c(nv,_tmp378);Cyc_Toc_exp_to_c(
nv,_tmp379);xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*
_tmp37C[3];_tmp37C[2]=_tmp379;_tmp37C[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(
elt_type),0);_tmp37C[0]=_tmp378;((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp37C,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
goto _LL1CF;_LL1D4:;_LL1D5:({void*_tmp37D=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp37E="found bad lhs for zero-terminated pointer assignment";
_tag_dyneither(_tmp37E,sizeof(char),53);}),_tag_dyneither(_tmp37D,sizeof(void*),
0));});_LL1CF:;}{struct _tuple1*_tmp37F=Cyc_Toc_temp_var();struct _RegionHandle
_tmp380=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp380;_push_region(rgn2);{
struct Cyc_Toc_Env*_tmp381=Cyc_Toc_add_varmap(rgn2,nv,_tmp37F,Cyc_Absyn_var_exp(
_tmp37F,0));struct Cyc_Absyn_Vardecl*_tmp382=({struct Cyc_Absyn_Vardecl*_tmp3B1=
_cycalloc(sizeof(*_tmp3B1));_tmp3B1->sc=(void*)((void*)2);_tmp3B1->name=_tmp37F;
_tmp3B1->tq=Cyc_Toc_mt_tq;_tmp3B1->type=(void*)_tmp373;_tmp3B1->initializer=(
struct Cyc_Absyn_Exp*)xinit;_tmp3B1->rgn=0;_tmp3B1->attributes=0;_tmp3B1->escapes=
0;_tmp3B1;});struct Cyc_Absyn_Local_b_struct*_tmp383=({struct Cyc_Absyn_Local_b_struct*
_tmp3AF=_cycalloc(sizeof(*_tmp3AF));_tmp3AF[0]=({struct Cyc_Absyn_Local_b_struct
_tmp3B0;_tmp3B0.tag=3;_tmp3B0.f1=_tmp382;_tmp3B0;});_tmp3AF;});struct Cyc_Absyn_Exp*
_tmp384=Cyc_Absyn_varb_exp(_tmp37F,(void*)_tmp383,0);_tmp384->topt=({struct Cyc_Core_Opt*
_tmp385=_cycalloc(sizeof(*_tmp385));_tmp385->v=(void*)fat_ptr_type;_tmp385;});{
struct Cyc_Absyn_Exp*_tmp386=Cyc_Absyn_deref_exp(_tmp384,0);_tmp386->topt=({
struct Cyc_Core_Opt*_tmp387=_cycalloc(sizeof(*_tmp387));_tmp387->v=(void*)
elt_type;_tmp387;});Cyc_Toc_exp_to_c(_tmp381,_tmp386);{struct _tuple1*_tmp388=Cyc_Toc_temp_var();
_tmp381=Cyc_Toc_add_varmap(rgn2,_tmp381,_tmp388,Cyc_Absyn_var_exp(_tmp388,0));{
struct Cyc_Absyn_Vardecl*_tmp389=({struct Cyc_Absyn_Vardecl*_tmp3AE=_cycalloc(
sizeof(*_tmp3AE));_tmp3AE->sc=(void*)((void*)2);_tmp3AE->name=_tmp388;_tmp3AE->tq=
Cyc_Toc_mt_tq;_tmp3AE->type=(void*)_tmp372;_tmp3AE->initializer=(struct Cyc_Absyn_Exp*)
_tmp386;_tmp3AE->rgn=0;_tmp3AE->attributes=0;_tmp3AE->escapes=0;_tmp3AE;});
struct Cyc_Absyn_Local_b_struct*_tmp38A=({struct Cyc_Absyn_Local_b_struct*_tmp3AC=
_cycalloc(sizeof(*_tmp3AC));_tmp3AC[0]=({struct Cyc_Absyn_Local_b_struct _tmp3AD;
_tmp3AD.tag=3;_tmp3AD.f1=_tmp389;_tmp3AD;});_tmp3AC;});struct Cyc_Absyn_Exp*
z_init=e2;if(popt != 0){struct Cyc_Absyn_Exp*_tmp38B=Cyc_Absyn_varb_exp(_tmp388,(
void*)_tmp38A,0);_tmp38B->topt=_tmp386->topt;z_init=Cyc_Absyn_prim2_exp((void*)
popt->v,_tmp38B,e2,0);z_init->topt=_tmp38B->topt;}Cyc_Toc_exp_to_c(_tmp381,
z_init);{struct _tuple1*_tmp38C=Cyc_Toc_temp_var();struct Cyc_Absyn_Vardecl*
_tmp38D=({struct Cyc_Absyn_Vardecl*_tmp3AB=_cycalloc(sizeof(*_tmp3AB));_tmp3AB->sc=(
void*)((void*)2);_tmp3AB->name=_tmp38C;_tmp3AB->tq=Cyc_Toc_mt_tq;_tmp3AB->type=(
void*)_tmp372;_tmp3AB->initializer=(struct Cyc_Absyn_Exp*)z_init;_tmp3AB->rgn=0;
_tmp3AB->attributes=0;_tmp3AB->escapes=0;_tmp3AB;});struct Cyc_Absyn_Local_b_struct*
_tmp38E=({struct Cyc_Absyn_Local_b_struct*_tmp3A9=_cycalloc(sizeof(*_tmp3A9));
_tmp3A9[0]=({struct Cyc_Absyn_Local_b_struct _tmp3AA;_tmp3AA.tag=3;_tmp3AA.f1=
_tmp38D;_tmp3AA;});_tmp3A9;});_tmp381=Cyc_Toc_add_varmap(rgn2,_tmp381,_tmp38C,
Cyc_Absyn_var_exp(_tmp38C,0));{struct Cyc_Absyn_Exp*_tmp38F=Cyc_Absyn_varb_exp(
_tmp388,(void*)_tmp38A,0);_tmp38F->topt=_tmp386->topt;{struct Cyc_Absyn_Exp*
_tmp390=Cyc_Toc_generate_zero(elt_type);struct Cyc_Absyn_Exp*_tmp391=Cyc_Absyn_prim2_exp((
void*)5,_tmp38F,_tmp390,0);_tmp391->topt=({struct Cyc_Core_Opt*_tmp392=_cycalloc(
sizeof(*_tmp392));_tmp392->v=(void*)Cyc_Absyn_sint_typ;_tmp392;});Cyc_Toc_exp_to_c(
_tmp381,_tmp391);{struct Cyc_Absyn_Exp*_tmp393=Cyc_Absyn_varb_exp(_tmp38C,(void*)
_tmp38E,0);_tmp393->topt=_tmp386->topt;{struct Cyc_Absyn_Exp*_tmp394=Cyc_Toc_generate_zero(
elt_type);struct Cyc_Absyn_Exp*_tmp395=Cyc_Absyn_prim2_exp((void*)6,_tmp393,
_tmp394,0);_tmp395->topt=({struct Cyc_Core_Opt*_tmp396=_cycalloc(sizeof(*_tmp396));
_tmp396->v=(void*)Cyc_Absyn_sint_typ;_tmp396;});Cyc_Toc_exp_to_c(_tmp381,_tmp395);{
struct Cyc_List_List*_tmp397=({struct Cyc_Absyn_Exp*_tmp3A8[2];_tmp3A8[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_type),0);_tmp3A8[0]=Cyc_Absyn_varb_exp(_tmp37F,(void*)
_tmp383,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp3A8,sizeof(struct Cyc_Absyn_Exp*),2));});struct Cyc_Absyn_Exp*
_tmp398=Cyc_Absyn_uint_exp(1,0);struct Cyc_Absyn_Exp*xsize;xsize=Cyc_Absyn_prim2_exp((
void*)5,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp397,0),_tmp398,0);{
struct Cyc_Absyn_Exp*_tmp399=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp391,
_tmp395,0),0);struct Cyc_Absyn_Stmt*_tmp39A=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__throw_arraybounds_e,0,0),0);struct Cyc_Absyn_Exp*_tmp39B=Cyc_Absyn_aggrmember_exp(
Cyc_Absyn_varb_exp(_tmp37F,(void*)_tmp383,0),Cyc_Toc_curr_sp,0);_tmp39B=Cyc_Toc_cast_it(
_tmp374,_tmp39B);{struct Cyc_Absyn_Exp*_tmp39C=Cyc_Absyn_deref_exp(_tmp39B,0);
struct Cyc_Absyn_Exp*_tmp39D=Cyc_Absyn_assign_exp(_tmp39C,Cyc_Absyn_var_exp(
_tmp38C,0),0);struct Cyc_Absyn_Stmt*_tmp39E=Cyc_Absyn_exp_stmt(_tmp39D,0);_tmp39E=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp399,_tmp39A,Cyc_Absyn_skip_stmt(
0),0),_tmp39E,0);_tmp39E=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp39F=
_cycalloc(sizeof(*_tmp39F));_tmp39F->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp3A0=_cycalloc(sizeof(*_tmp3A0));_tmp3A0[0]=({struct Cyc_Absyn_Var_d_struct
_tmp3A1;_tmp3A1.tag=0;_tmp3A1.f1=_tmp38D;_tmp3A1;});_tmp3A0;}));_tmp39F->loc=0;
_tmp39F;}),_tmp39E,0);_tmp39E=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp3A2=
_cycalloc(sizeof(*_tmp3A2));_tmp3A2->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp3A3=_cycalloc(sizeof(*_tmp3A3));_tmp3A3[0]=({struct Cyc_Absyn_Var_d_struct
_tmp3A4;_tmp3A4.tag=0;_tmp3A4.f1=_tmp389;_tmp3A4;});_tmp3A3;}));_tmp3A2->loc=0;
_tmp3A2;}),_tmp39E,0);_tmp39E=Cyc_Absyn_decl_stmt(({struct Cyc_Absyn_Decl*_tmp3A5=
_cycalloc(sizeof(*_tmp3A5));_tmp3A5->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp3A6=_cycalloc(sizeof(*_tmp3A6));_tmp3A6[0]=({struct Cyc_Absyn_Var_d_struct
_tmp3A7;_tmp3A7.tag=0;_tmp3A7.f1=_tmp382;_tmp3A7;});_tmp3A6;}));_tmp3A5->loc=0;
_tmp3A5;}),_tmp39E,0);(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(_tmp39E));}}}}}}}}}}}};
_pop_region(rgn2);}}struct _tuple11{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*
f3;};struct _tuple12{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static
void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){void*_tmp3B3=(
void*)e->r;if(e->topt == 0)({struct Cyc_String_pa_struct _tmp3B6;_tmp3B6.tag=0;
_tmp3B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));{
void*_tmp3B4[1]={& _tmp3B6};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(({const char*_tmp3B5="exp_to_c: no type for %s";
_tag_dyneither(_tmp3B5,sizeof(char),25);}),_tag_dyneither(_tmp3B4,sizeof(void*),
1));}});{void*old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;void*
_tmp3B7=_tmp3B3;union Cyc_Absyn_Cnst_union _tmp3B8;struct _tuple1*_tmp3B9;void*
_tmp3BA;struct _tuple1*_tmp3BB;void*_tmp3BC;struct Cyc_List_List*_tmp3BD;struct Cyc_Absyn_Exp*
_tmp3BE;void*_tmp3BF;struct Cyc_Absyn_Exp*_tmp3C0;struct Cyc_Core_Opt*_tmp3C1;
struct Cyc_Absyn_Exp*_tmp3C2;struct Cyc_Absyn_Exp*_tmp3C3;struct Cyc_Absyn_Exp*
_tmp3C4;struct Cyc_Absyn_Exp*_tmp3C5;struct Cyc_Absyn_Exp*_tmp3C6;struct Cyc_Absyn_Exp*
_tmp3C7;struct Cyc_Absyn_Exp*_tmp3C8;struct Cyc_Absyn_Exp*_tmp3C9;struct Cyc_Absyn_Exp*
_tmp3CA;struct Cyc_Absyn_Exp*_tmp3CB;struct Cyc_Absyn_Exp*_tmp3CC;struct Cyc_List_List*
_tmp3CD;struct Cyc_Absyn_Exp*_tmp3CE;struct Cyc_List_List*_tmp3CF;struct Cyc_Absyn_VarargCallInfo*
_tmp3D0;struct Cyc_Absyn_Exp*_tmp3D1;struct Cyc_List_List*_tmp3D2;struct Cyc_Absyn_VarargCallInfo*
_tmp3D3;struct Cyc_Absyn_VarargCallInfo _tmp3D4;int _tmp3D5;struct Cyc_List_List*
_tmp3D6;struct Cyc_Absyn_VarargInfo*_tmp3D7;struct Cyc_Absyn_Exp*_tmp3D8;struct Cyc_Absyn_Exp*
_tmp3D9;struct Cyc_Absyn_Exp*_tmp3DA;struct Cyc_List_List*_tmp3DB;void*_tmp3DC;
void**_tmp3DD;struct Cyc_Absyn_Exp*_tmp3DE;int _tmp3DF;void*_tmp3E0;struct Cyc_Absyn_Exp*
_tmp3E1;struct Cyc_Absyn_Exp*_tmp3E2;struct Cyc_Absyn_Exp*_tmp3E3;struct Cyc_Absyn_Exp*
_tmp3E4;void*_tmp3E5;void*_tmp3E6;void*_tmp3E7;struct _dyneither_ptr*_tmp3E8;void*
_tmp3E9;void*_tmp3EA;unsigned int _tmp3EB;struct Cyc_Absyn_Exp*_tmp3EC;struct Cyc_Absyn_Exp*
_tmp3ED;struct _dyneither_ptr*_tmp3EE;struct Cyc_Absyn_Exp*_tmp3EF;struct
_dyneither_ptr*_tmp3F0;struct Cyc_Absyn_Exp*_tmp3F1;struct Cyc_Absyn_Exp*_tmp3F2;
struct Cyc_List_List*_tmp3F3;struct Cyc_List_List*_tmp3F4;struct Cyc_Absyn_Vardecl*
_tmp3F5;struct Cyc_Absyn_Exp*_tmp3F6;struct Cyc_Absyn_Exp*_tmp3F7;int _tmp3F8;
struct _tuple1*_tmp3F9;struct Cyc_List_List*_tmp3FA;struct Cyc_List_List*_tmp3FB;
struct Cyc_Absyn_Aggrdecl*_tmp3FC;void*_tmp3FD;struct Cyc_List_List*_tmp3FE;struct
Cyc_List_List*_tmp3FF;struct Cyc_Absyn_Tuniondecl*_tmp400;struct Cyc_Absyn_Tunionfield*
_tmp401;struct Cyc_List_List*_tmp402;struct Cyc_Absyn_Tuniondecl*_tmp403;struct Cyc_Absyn_Tunionfield*
_tmp404;struct Cyc_Absyn_MallocInfo _tmp405;int _tmp406;struct Cyc_Absyn_Exp*_tmp407;
void**_tmp408;struct Cyc_Absyn_Exp*_tmp409;int _tmp40A;struct Cyc_Absyn_Exp*_tmp40B;
struct Cyc_Absyn_Exp*_tmp40C;struct Cyc_Absyn_Stmt*_tmp40D;_LL1D7: if(*((int*)
_tmp3B7)!= 0)goto _LL1D9;_tmp3B8=((struct Cyc_Absyn_Const_e_struct*)_tmp3B7)->f1;
if(((((struct Cyc_Absyn_Const_e_struct*)_tmp3B7)->f1).Null_c).tag != 6)goto _LL1D9;
_LL1D8: {struct Cyc_Absyn_Exp*_tmp40E=Cyc_Absyn_uint_exp(0,0);if(Cyc_Tcutil_is_tagged_pointer_typ(
old_typ)){if(Cyc_Toc_is_toplevel(nv))(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_dyn_arr(
old_typ,_tmp40E,_tmp40E))->r));else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,({
struct Cyc_Absyn_Exp*_tmp40F[3];_tmp40F[2]=_tmp40E;_tmp40F[1]=_tmp40E;_tmp40F[0]=
_tmp40E;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp40F,sizeof(struct Cyc_Absyn_Exp*),3));})));}}else{(void*)(e->r=(
void*)((void*)& Cyc_Toc_zero_exp));}goto _LL1D6;}_LL1D9: if(*((int*)_tmp3B7)!= 0)
goto _LL1DB;_LL1DA: goto _LL1D6;_LL1DB: if(*((int*)_tmp3B7)!= 1)goto _LL1DD;_tmp3B9=((
struct Cyc_Absyn_Var_e_struct*)_tmp3B7)->f1;_tmp3BA=(void*)((struct Cyc_Absyn_Var_e_struct*)
_tmp3B7)->f2;_LL1DC:{struct _handler_cons _tmp410;_push_handler(& _tmp410);{int
_tmp412=0;if(setjmp(_tmp410.handler))_tmp412=1;if(!_tmp412){(void*)(e->r=(void*)((
void*)(Cyc_Toc_lookup_varmap(nv,_tmp3B9))->r));;_pop_handler();}else{void*
_tmp411=(void*)_exn_thrown;void*_tmp414=_tmp411;_LL230: if(_tmp414 != Cyc_Dict_Absent)
goto _LL232;_LL231:({struct Cyc_String_pa_struct _tmp417;_tmp417.tag=0;_tmp417.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp3B9));{
void*_tmp415[1]={& _tmp417};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_toc_impos)(({const char*_tmp416="Can't find %s in exp_to_c, Var\n";
_tag_dyneither(_tmp416,sizeof(char),32);}),_tag_dyneither(_tmp415,sizeof(void*),
1));}});_LL232:;_LL233:(void)_throw(_tmp414);_LL22F:;}}}goto _LL1D6;_LL1DD: if(*((
int*)_tmp3B7)!= 2)goto _LL1DF;_tmp3BB=((struct Cyc_Absyn_UnknownId_e_struct*)
_tmp3B7)->f1;_LL1DE:({void*_tmp418=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp419="unknownid";
_tag_dyneither(_tmp419,sizeof(char),10);}),_tag_dyneither(_tmp418,sizeof(void*),
0));});_LL1DF: if(*((int*)_tmp3B7)!= 3)goto _LL1E1;_tmp3BC=(void*)((struct Cyc_Absyn_Primop_e_struct*)
_tmp3B7)->f1;_tmp3BD=((struct Cyc_Absyn_Primop_e_struct*)_tmp3B7)->f2;_LL1E0: {
struct Cyc_List_List*_tmp41A=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp3BD);((void(*)(void(*
f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*
x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp3BD);{void*_tmp41B=_tmp3BC;_LL235:
if((int)_tmp41B != 19)goto _LL237;_LL236: {struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3BD))->hd;{void*_tmp41C=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v);struct Cyc_Absyn_ArrayInfo
_tmp41D;struct Cyc_Absyn_Exp*_tmp41E;struct Cyc_Absyn_PtrInfo _tmp41F;void*_tmp420;
struct Cyc_Absyn_PtrAtts _tmp421;struct Cyc_Absyn_Conref*_tmp422;struct Cyc_Absyn_Conref*
_tmp423;struct Cyc_Absyn_Conref*_tmp424;_LL24A: if(_tmp41C <= (void*)4)goto _LL24E;
if(*((int*)_tmp41C)!= 7)goto _LL24C;_tmp41D=((struct Cyc_Absyn_ArrayType_struct*)
_tmp41C)->f1;_tmp41E=_tmp41D.num_elts;_LL24B: if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)
_check_null(_tmp41E)))({void*_tmp425=0;Cyc_Tcutil_terr(e->loc,({const char*
_tmp426="can't calculate numelts";_tag_dyneither(_tmp426,sizeof(char),24);}),
_tag_dyneither(_tmp425,sizeof(void*),0));});(void*)(e->r=(void*)((void*)_tmp41E->r));
goto _LL249;_LL24C: if(*((int*)_tmp41C)!= 4)goto _LL24E;_tmp41F=((struct Cyc_Absyn_PointerType_struct*)
_tmp41C)->f1;_tmp420=(void*)_tmp41F.elt_typ;_tmp421=_tmp41F.ptr_atts;_tmp422=
_tmp421.nullable;_tmp423=_tmp421.bounds;_tmp424=_tmp421.zero_term;_LL24D:{void*
_tmp427=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp423);struct Cyc_Absyn_Exp*
_tmp428;_LL251: if((int)_tmp427 != 0)goto _LL253;_LL252:(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__get_dyneither_size_e,({struct Cyc_Absyn_Exp*_tmp429[2];_tmp429[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp420),0);_tmp429[0]=(struct Cyc_Absyn_Exp*)_tmp3BD->hd;((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp429,sizeof(struct Cyc_Absyn_Exp*),2));})));goto _LL250;_LL253: if(_tmp427 <= (
void*)1)goto _LL250;if(*((int*)_tmp427)!= 0)goto _LL250;_tmp428=((struct Cyc_Absyn_Upper_b_struct*)
_tmp427)->f1;_LL254: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp424)){struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunction(& Cyc_Toc__get_zero_arr_size_functionSet,(
struct Cyc_Absyn_Exp*)_tmp3BD->hd);(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
function_e,({struct Cyc_Absyn_Exp*_tmp42A[2];_tmp42A[1]=_tmp428;_tmp42A[0]=(
struct Cyc_Absyn_Exp*)_tmp3BD->hd;((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp42A,sizeof(struct Cyc_Absyn_Exp*),2));})));}
else{if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp422)){
if(!Cyc_Evexp_c_can_eval(_tmp428))({void*_tmp42B=0;Cyc_Tcutil_terr(e->loc,({
const char*_tmp42C="can't calculate numelts";_tag_dyneither(_tmp42C,sizeof(char),
24);}),_tag_dyneither(_tmp42B,sizeof(void*),0));});(void*)(e->r=(void*)Cyc_Toc_conditional_exp_r(
arg,_tmp428,Cyc_Absyn_uint_exp(0,0)));}else{(void*)(e->r=(void*)((void*)_tmp428->r));
goto _LL250;}}goto _LL250;_LL250:;}goto _LL249;_LL24E:;_LL24F:({struct Cyc_String_pa_struct
_tmp430;_tmp430.tag=0;_tmp430.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)_check_null(arg->topt))->v));{
struct Cyc_String_pa_struct _tmp42F;_tmp42F.tag=0;_tmp42F.f1=(struct _dyneither_ptr)((
struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v));{void*_tmp42D[2]={& _tmp42F,& _tmp430};((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp42E="size primop applied to non-array %s (%s)";_tag_dyneither(_tmp42E,
sizeof(char),41);}),_tag_dyneither(_tmp42D,sizeof(void*),2));}}});_LL249:;}goto
_LL234;}_LL237: if((int)_tmp41B != 0)goto _LL239;_LL238:{void*_tmp431=Cyc_Tcutil_compress((
void*)((struct Cyc_List_List*)_check_null(_tmp41A))->hd);struct Cyc_Absyn_PtrInfo
_tmp432;void*_tmp433;struct Cyc_Absyn_PtrAtts _tmp434;struct Cyc_Absyn_Conref*
_tmp435;struct Cyc_Absyn_Conref*_tmp436;_LL256: if(_tmp431 <= (void*)4)goto _LL258;
if(*((int*)_tmp431)!= 4)goto _LL258;_tmp432=((struct Cyc_Absyn_PointerType_struct*)
_tmp431)->f1;_tmp433=(void*)_tmp432.elt_typ;_tmp434=_tmp432.ptr_atts;_tmp435=
_tmp434.bounds;_tmp436=_tmp434.zero_term;_LL257:{void*_tmp437=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp435);struct Cyc_Absyn_Exp*_tmp438;_LL25B: if((int)_tmp437
!= 0)goto _LL25D;_LL25C: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp3BD))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3BD->tl))->hd;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*_tmp439[3];_tmp439[2]=e2;
_tmp439[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp433),0);_tmp439[0]=e1;((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp439,sizeof(struct Cyc_Absyn_Exp*),3));})));goto _LL25A;}_LL25D: if(_tmp437 <= (
void*)1)goto _LL25A;if(*((int*)_tmp437)!= 0)goto _LL25A;_tmp438=((struct Cyc_Absyn_Upper_b_struct*)
_tmp437)->f1;_LL25E: if(((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,_tmp436)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp3BD))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct
Cyc_List_List*)_check_null(_tmp3BD->tl))->hd;(void*)(e->r=(void*)((void*)(Cyc_Absyn_fncall_exp(
Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_plus_functionSet,e1),({struct Cyc_Absyn_Exp*
_tmp43A[3];_tmp43A[2]=e2;_tmp43A[1]=_tmp438;_tmp43A[0]=e1;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp43A,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))->r));}goto _LL25A;_LL25A:;}goto _LL255;_LL258:;_LL259: goto _LL255;_LL255:;}
goto _LL234;_LL239: if((int)_tmp41B != 2)goto _LL23B;_LL23A: {void*elt_typ=(void*)0;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)
_check_null(_tmp41A))->hd,& elt_typ)){struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3BD))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3BD->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ((
void*)((struct Cyc_List_List*)_check_null(_tmp41A->tl))->hd)){(void*)(e1->r=(void*)
Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp));(
void*)(e2->r=(void*)Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((void*)e2->r,0),
Cyc_Toc_curr_sp));(void*)(e->r=(void*)((void*)(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(
e),Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r));}else{(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,({struct Cyc_Absyn_Exp*
_tmp43B[3];_tmp43B[2]=Cyc_Absyn_prim1_exp((void*)2,e2,0);_tmp43B[1]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(elt_typ),0);_tmp43B[0]=e1;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp43B,sizeof(struct Cyc_Absyn_Exp*),
3));})));}}goto _LL234;}_LL23B: if((int)_tmp41B != 5)goto _LL23D;_LL23C: goto _LL23E;
_LL23D: if((int)_tmp41B != 6)goto _LL23F;_LL23E: goto _LL240;_LL23F: if((int)_tmp41B != 
7)goto _LL241;_LL240: goto _LL242;_LL241: if((int)_tmp41B != 9)goto _LL243;_LL242: goto
_LL244;_LL243: if((int)_tmp41B != 8)goto _LL245;_LL244: goto _LL246;_LL245: if((int)
_tmp41B != 10)goto _LL247;_LL246: {struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3BD))->hd;struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((
struct Cyc_List_List*)_check_null(_tmp3BD->tl))->hd;void*t1=(void*)((struct Cyc_List_List*)
_check_null(_tmp41A))->hd;void*t2=(void*)((struct Cyc_List_List*)_check_null(
_tmp41A->tl))->hd;if(Cyc_Tcutil_is_tagged_pointer_typ(t1))(void*)(e1->r=(void*)
Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((void*)e1->r,0),Cyc_Toc_curr_sp));if(
Cyc_Tcutil_is_tagged_pointer_typ(t2))(void*)(e2->r=(void*)Cyc_Toc_aggrmember_exp_r(
Cyc_Absyn_new_exp((void*)e2->r,0),Cyc_Toc_curr_sp));goto _LL234;}_LL247:;_LL248:
goto _LL234;_LL234:;}goto _LL1D6;}_LL1E1: if(*((int*)_tmp3B7)!= 5)goto _LL1E3;
_tmp3BE=((struct Cyc_Absyn_Increment_e_struct*)_tmp3B7)->f1;_tmp3BF=(void*)((
struct Cyc_Absyn_Increment_e_struct*)_tmp3B7)->f2;_LL1E2: {void*e2_cyc_typ=(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp3BE->topt))->v;void*ptr_type=(void*)0;void*
elt_type=(void*)0;int is_dyneither=0;struct _dyneither_ptr incr_str=({const char*
_tmp44E="increment";_tag_dyneither(_tmp44E,sizeof(char),10);});if(_tmp3BF == (
void*)2  || _tmp3BF == (void*)3)incr_str=({const char*_tmp43C="decrement";
_tag_dyneither(_tmp43C,sizeof(char),10);});if(Cyc_Tcutil_is_zero_ptr_deref(
_tmp3BE,& ptr_type,& is_dyneither,& elt_type)){({struct Cyc_String_pa_struct _tmp43F;
_tmp43F.tag=0;_tmp43F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);{
void*_tmp43D[1]={& _tmp43F};Cyc_Tcutil_terr(e->loc,({const char*_tmp43E="in-place %s is not supported when dereferencing a zero-terminated pointer";
_tag_dyneither(_tmp43E,sizeof(char),74);}),_tag_dyneither(_tmp43D,sizeof(void*),
1));}});({void*_tmp440=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp441="in-place inc on zero-term";
_tag_dyneither(_tmp441,sizeof(char),26);}),_tag_dyneither(_tmp440,sizeof(void*),
0));});}Cyc_Toc_exp_to_c(nv,_tmp3BE);{void*elt_typ=(void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(
old_typ,& elt_typ)){struct Cyc_Absyn_Exp*fn_e;int change=1;fn_e=(_tmp3BF == (void*)1
 || _tmp3BF == (void*)3)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
if(_tmp3BF == (void*)2  || _tmp3BF == (void*)3)change=- 1;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
fn_e,({struct Cyc_Absyn_Exp*_tmp442[3];_tmp442[2]=Cyc_Absyn_signed_int_exp(change,
0);_tmp442[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp442[0]=
Cyc_Absyn_address_exp(_tmp3BE,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp442,sizeof(struct Cyc_Absyn_Exp*),3));})));}
else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
fn_e;{void*_tmp443=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(
_tmp3BE->topt))->v);void*_tmp444;int _tmp445;_LL260: if(_tmp443 <= (void*)4)goto
_LL262;if(*((int*)_tmp443)!= 5)goto _LL262;_tmp444=(void*)((struct Cyc_Absyn_IntType_struct*)
_tmp443)->f2;_LL261:{void*_tmp446=_tmp444;_LL26B: if((int)_tmp446 != 0)goto _LL26D;
_LL26C: fn_e=_tmp3BF == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_char_e: Cyc_Toc__zero_arr_inplace_plus_char_e;
goto _LL26A;_LL26D: if((int)_tmp446 != 1)goto _LL26F;_LL26E: fn_e=_tmp3BF == (void*)1?
Cyc_Toc__zero_arr_inplace_plus_post_short_e: Cyc_Toc__zero_arr_inplace_plus_short_e;
goto _LL26A;_LL26F: if((int)_tmp446 != 2)goto _LL271;_LL270: fn_e=_tmp3BF == (void*)1?
Cyc_Toc__zero_arr_inplace_plus_post_int_e: Cyc_Toc__zero_arr_inplace_plus_int_e;
goto _LL26A;_LL271:;_LL272:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp447=_cycalloc(sizeof(*_tmp447));_tmp447[0]=({struct Cyc_Core_Impossible_struct
_tmp448;_tmp448.tag=Cyc_Core_Impossible;_tmp448.f1=({const char*_tmp449="impossible IntType (not char, short or int)";
_tag_dyneither(_tmp449,sizeof(char),44);});_tmp448;});_tmp447;}));_LL26A:;}goto
_LL25F;_LL262: if((int)_tmp443 != 1)goto _LL264;_LL263: fn_e=_tmp3BF == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_float_e:
Cyc_Toc__zero_arr_inplace_plus_float_e;goto _LL25F;_LL264: if(_tmp443 <= (void*)4)
goto _LL268;if(*((int*)_tmp443)!= 6)goto _LL266;_tmp445=((struct Cyc_Absyn_DoubleType_struct*)
_tmp443)->f1;_LL265: switch(_tmp445){case 1: _LL273: fn_e=_tmp3BF == (void*)1?Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e:
Cyc_Toc__zero_arr_inplace_plus_longdouble_e;break;default: _LL274: fn_e=_tmp3BF == (
void*)1?Cyc_Toc__zero_arr_inplace_plus_post_double_e: Cyc_Toc__zero_arr_inplace_plus_double_e;}
goto _LL25F;_LL266: if(*((int*)_tmp443)!= 4)goto _LL268;_LL267: fn_e=_tmp3BF == (void*)
1?Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e: Cyc_Toc__zero_arr_inplace_plus_voidstar_e;
goto _LL25F;_LL268:;_LL269:(int)_throw((void*)({struct Cyc_Core_Impossible_struct*
_tmp44A=_cycalloc(sizeof(*_tmp44A));_tmp44A[0]=({struct Cyc_Core_Impossible_struct
_tmp44B;_tmp44B.tag=Cyc_Core_Impossible;_tmp44B.f1=({const char*_tmp44C="impossible expression type (not int, float, double, or pointer)";
_tag_dyneither(_tmp44C,sizeof(char),64);});_tmp44B;});_tmp44A;}));_LL25F:;}(void*)(
e->r=(void*)Cyc_Toc_fncall_exp_r(fn_e,({struct Cyc_Absyn_Exp*_tmp44D[2];_tmp44D[1]=
Cyc_Absyn_signed_int_exp(1,0);_tmp44D[0]=_tmp3BE;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp44D,sizeof(struct Cyc_Absyn_Exp*),
2));})));}else{if(elt_typ == (void*)0  && !Cyc_Absyn_is_lvalue(_tmp3BE)){Cyc_Toc_lvalue_assign(
_tmp3BE,0,Cyc_Toc_incr_lvalue,_tmp3BF);(void*)(e->r=(void*)((void*)_tmp3BE->r));}}}
goto _LL1D6;}}_LL1E3: if(*((int*)_tmp3B7)!= 4)goto _LL1E5;_tmp3C0=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp3B7)->f1;_tmp3C1=((struct Cyc_Absyn_AssignOp_e_struct*)_tmp3B7)->f2;_tmp3C2=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp3B7)->f3;_LL1E4: {void*ptr_type=(void*)0;
void*elt_type=(void*)0;int is_dyneither=0;if(Cyc_Tcutil_is_zero_ptr_deref(_tmp3C0,&
ptr_type,& is_dyneither,& elt_type)){Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp3C0,
_tmp3C1,_tmp3C2,ptr_type,is_dyneither,elt_type);return;}{int e1_poly=Cyc_Toc_is_poly_project(
_tmp3C0);void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3C0->topt))->v;
void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3C2->topt))->v;Cyc_Toc_exp_to_c(
nv,_tmp3C0);Cyc_Toc_exp_to_c(nv,_tmp3C2);{int done=0;if(_tmp3C1 != 0){void*elt_typ=(
void*)0;if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){struct Cyc_Absyn_Exp*
change;{void*_tmp44F=(void*)_tmp3C1->v;_LL277: if((int)_tmp44F != 0)goto _LL279;
_LL278: change=_tmp3C2;goto _LL276;_LL279: if((int)_tmp44F != 2)goto _LL27B;_LL27A:
change=Cyc_Absyn_prim1_exp((void*)2,_tmp3C2,0);goto _LL276;_LL27B:;_LL27C:({void*
_tmp450=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp451="bad t ? pointer arithmetic";_tag_dyneither(_tmp451,sizeof(
char),27);}),_tag_dyneither(_tmp450,sizeof(void*),0));});_LL276:;}done=1;{struct
Cyc_Absyn_Exp*_tmp452=Cyc_Toc__dyneither_ptr_inplace_plus_e;(void*)(e->r=(void*)
Cyc_Toc_fncall_exp_r(_tmp452,({struct Cyc_Absyn_Exp*_tmp453[3];_tmp453[2]=change;
_tmp453[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0);_tmp453[0]=Cyc_Absyn_address_exp(
_tmp3C0,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp453,sizeof(struct Cyc_Absyn_Exp*),3));})));}}else{if(Cyc_Tcutil_is_zero_pointer_typ_elt(
old_typ,& elt_typ)){void*_tmp454=(void*)_tmp3C1->v;_LL27E: if((int)_tmp454 != 0)
goto _LL280;_LL27F: done=1;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunction(&
Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp3C0),({struct Cyc_Absyn_Exp*
_tmp455[2];_tmp455[1]=_tmp3C2;_tmp455[0]=_tmp3C0;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp455,sizeof(struct Cyc_Absyn_Exp*),
2));})));goto _LL27D;_LL280:;_LL281:({void*_tmp456=0;((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp457="bad zero-terminated pointer arithmetic";
_tag_dyneither(_tmp457,sizeof(char),39);}),_tag_dyneither(_tmp456,sizeof(void*),
0));});_LL27D:;}}}if(!done){if(e1_poly)(void*)(_tmp3C2->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp3C2->r,0)));if(!Cyc_Absyn_is_lvalue(
_tmp3C0)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*,struct _tuple10*),struct _tuple10*f_env))Cyc_Toc_lvalue_assign)(
_tmp3C0,0,Cyc_Toc_assignop_lvalue,({struct _tuple10*_tmp458=_cycalloc(sizeof(
struct _tuple10)* 1);_tmp458[0]=({struct _tuple10 _tmp459;_tmp459.f1=_tmp3C1;
_tmp459.f2=_tmp3C2;_tmp459;});_tmp458;}));(void*)(e->r=(void*)((void*)_tmp3C0->r));}}
goto _LL1D6;}}}_LL1E5: if(*((int*)_tmp3B7)!= 6)goto _LL1E7;_tmp3C3=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp3B7)->f1;_tmp3C4=((struct Cyc_Absyn_Conditional_e_struct*)_tmp3B7)->f2;
_tmp3C5=((struct Cyc_Absyn_Conditional_e_struct*)_tmp3B7)->f3;_LL1E6: Cyc_Toc_exp_to_c(
nv,_tmp3C3);Cyc_Toc_exp_to_c(nv,_tmp3C4);Cyc_Toc_exp_to_c(nv,_tmp3C5);goto _LL1D6;
_LL1E7: if(*((int*)_tmp3B7)!= 7)goto _LL1E9;_tmp3C6=((struct Cyc_Absyn_And_e_struct*)
_tmp3B7)->f1;_tmp3C7=((struct Cyc_Absyn_And_e_struct*)_tmp3B7)->f2;_LL1E8: Cyc_Toc_exp_to_c(
nv,_tmp3C6);Cyc_Toc_exp_to_c(nv,_tmp3C7);goto _LL1D6;_LL1E9: if(*((int*)_tmp3B7)!= 
8)goto _LL1EB;_tmp3C8=((struct Cyc_Absyn_Or_e_struct*)_tmp3B7)->f1;_tmp3C9=((
struct Cyc_Absyn_Or_e_struct*)_tmp3B7)->f2;_LL1EA: Cyc_Toc_exp_to_c(nv,_tmp3C8);
Cyc_Toc_exp_to_c(nv,_tmp3C9);goto _LL1D6;_LL1EB: if(*((int*)_tmp3B7)!= 9)goto
_LL1ED;_tmp3CA=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp3B7)->f1;_tmp3CB=((struct
Cyc_Absyn_SeqExp_e_struct*)_tmp3B7)->f2;_LL1EC: Cyc_Toc_exp_to_c(nv,_tmp3CA);Cyc_Toc_exp_to_c(
nv,_tmp3CB);goto _LL1D6;_LL1ED: if(*((int*)_tmp3B7)!= 10)goto _LL1EF;_tmp3CC=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp3B7)->f1;_tmp3CD=((struct Cyc_Absyn_UnknownCall_e_struct*)
_tmp3B7)->f2;_LL1EE: _tmp3CE=_tmp3CC;_tmp3CF=_tmp3CD;goto _LL1F0;_LL1EF: if(*((int*)
_tmp3B7)!= 11)goto _LL1F1;_tmp3CE=((struct Cyc_Absyn_FnCall_e_struct*)_tmp3B7)->f1;
_tmp3CF=((struct Cyc_Absyn_FnCall_e_struct*)_tmp3B7)->f2;_tmp3D0=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp3B7)->f3;if(_tmp3D0 != 0)goto _LL1F1;_LL1F0: Cyc_Toc_exp_to_c(nv,_tmp3CE);((
void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,
struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp3CF);goto _LL1D6;
_LL1F1: if(*((int*)_tmp3B7)!= 11)goto _LL1F3;_tmp3D1=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp3B7)->f1;_tmp3D2=((struct Cyc_Absyn_FnCall_e_struct*)_tmp3B7)->f2;_tmp3D3=((
struct Cyc_Absyn_FnCall_e_struct*)_tmp3B7)->f3;if(_tmp3D3 == 0)goto _LL1F3;_tmp3D4=*
_tmp3D3;_tmp3D5=_tmp3D4.num_varargs;_tmp3D6=_tmp3D4.injectors;_tmp3D7=_tmp3D4.vai;
_LL1F2:{struct _RegionHandle _tmp45A=_new_region("r");struct _RegionHandle*r=&
_tmp45A;_push_region(r);{struct _tuple1*argv=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*
argvexp=Cyc_Absyn_var_exp(argv,0);struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((
unsigned int)_tmp3D5,0);void*cva_type=Cyc_Toc_typ_to_c((void*)_tmp3D7->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)
num_varargs_exp,Cyc_Absyn_false_conref,0);int num_args=((int(*)(struct Cyc_List_List*
x))Cyc_List_length)(_tmp3D2);int num_normargs=num_args - _tmp3D5;struct Cyc_List_List*
new_args=0;{int i=0;for(0;i < num_normargs;(++ i,_tmp3D2=_tmp3D2->tl)){Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp3D2))->hd);
new_args=({struct Cyc_List_List*_tmp45B=_cycalloc(sizeof(*_tmp45B));_tmp45B->hd=(
struct Cyc_Absyn_Exp*)_tmp3D2->hd;_tmp45B->tl=new_args;_tmp45B;});}}new_args=({
struct Cyc_List_List*_tmp45C=_cycalloc(sizeof(*_tmp45C));_tmp45C->hd=Cyc_Absyn_fncall_exp(
Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*_tmp45D[3];_tmp45D[2]=
num_varargs_exp;_tmp45D[1]=Cyc_Absyn_sizeoftyp_exp(cva_type,0);_tmp45D[0]=
argvexp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp45D,sizeof(struct Cyc_Absyn_Exp*),3));}),0);_tmp45C->tl=
new_args;_tmp45C;});new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(new_args);Cyc_Toc_exp_to_c(nv,_tmp3D1);{struct Cyc_Absyn_Stmt*s=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp3D1,new_args,0),0);if(_tmp3D7->inject){
struct Cyc_Absyn_Tuniondecl*tud;{void*_tmp45E=Cyc_Tcutil_compress((void*)_tmp3D7->type);
struct Cyc_Absyn_TunionInfo _tmp45F;union Cyc_Absyn_TunionInfoU_union _tmp460;struct
Cyc_Absyn_Tuniondecl**_tmp461;struct Cyc_Absyn_Tuniondecl*_tmp462;_LL283: if(
_tmp45E <= (void*)4)goto _LL285;if(*((int*)_tmp45E)!= 2)goto _LL285;_tmp45F=((
struct Cyc_Absyn_TunionType_struct*)_tmp45E)->f1;_tmp460=_tmp45F.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp45E)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL285;_tmp461=(_tmp460.KnownTunion).f1;_tmp462=*_tmp461;_LL284: tud=
_tmp462;goto _LL282;_LL285:;_LL286:({void*_tmp463=0;((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp464="toc: unknown tunion in vararg with inject";
_tag_dyneither(_tmp464,sizeof(char),42);}),_tag_dyneither(_tmp463,sizeof(void*),
0));});_LL282:;}{struct _dyneither_ptr vs=({unsigned int _tmp46E=(unsigned int)
_tmp3D5;struct _tuple1**_tmp46F=(struct _tuple1**)_region_malloc(r,_check_times(
sizeof(struct _tuple1*),_tmp46E));struct _dyneither_ptr _tmp471=_tag_dyneither(
_tmp46F,sizeof(struct _tuple1*),_tmp46E);{unsigned int _tmp470=_tmp46E;
unsigned int i;for(i=0;i < _tmp470;i ++){_tmp46F[i]=Cyc_Toc_temp_var();}}_tmp471;});
if(_tmp3D5 != 0){struct Cyc_List_List*_tmp465=0;{int i=_tmp3D5 - 1;for(0;i >= 0;-- i){
_tmp465=({struct Cyc_List_List*_tmp466=_cycalloc(sizeof(*_tmp466));_tmp466->hd=
Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)
_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0),0));_tmp466->tl=
_tmp465;_tmp466;});}}s=Cyc_Absyn_declare_stmt(argv,arr_type,(struct Cyc_Absyn_Exp*)
Cyc_Absyn_unresolvedmem_exp(0,_tmp465,0),s,0);{int i=0;for(0;_tmp3D2 != 0;(((
_tmp3D2=_tmp3D2->tl,_tmp3D6=_tmp3D6->tl)),++ i)){struct Cyc_Absyn_Exp*arg=(struct
Cyc_Absyn_Exp*)_tmp3D2->hd;void*arg_type=(void*)((struct Cyc_Core_Opt*)
_check_null(arg->topt))->v;struct _tuple1*var=*((struct _tuple1**)
_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));struct Cyc_Absyn_Exp*
varexp=Cyc_Absyn_var_exp(var,0);struct Cyc_Absyn_Tunionfield _tmp468;struct _tuple1*
_tmp469;struct Cyc_List_List*_tmp46A;struct Cyc_Absyn_Tunionfield*_tmp467=(struct
Cyc_Absyn_Tunionfield*)((struct Cyc_List_List*)_check_null(_tmp3D6))->hd;_tmp468=*
_tmp467;_tmp469=_tmp468.name;_tmp46A=_tmp468.typs;{void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp46A))->hd)).f2);Cyc_Toc_exp_to_c(
nv,arg);if(Cyc_Toc_is_void_star(field_typ))arg=Cyc_Toc_cast_it(field_typ,arg);s=
Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(varexp,Cyc_Absyn_fieldname(
1),0),arg,0),s,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(
varexp,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(tud,_tmp469,1),0),s,0);s=Cyc_Absyn_declare_stmt(
var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(_tmp469,({const char*_tmp46B="_struct";
_tag_dyneither(_tmp46B,sizeof(char),8);}))),0,s,0);}}}}else{struct Cyc_List_List*
_tmp46C=({struct _tuple8*_tmp46D[3];_tmp46D[2]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(
0,0));_tmp46D[1]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0));_tmp46D[0]=Cyc_Toc_make_dle(
Cyc_Absyn_uint_exp(0,0));((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp46D,sizeof(struct _tuple8*),3));});s=Cyc_Absyn_declare_stmt(
argv,Cyc_Absyn_void_star_typ(),(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),s,0);}}}
else{{int i=0;for(0;_tmp3D2 != 0;(_tmp3D2=_tmp3D2->tl,++ i)){Cyc_Toc_exp_to_c(nv,(
struct Cyc_Absyn_Exp*)_tmp3D2->hd);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(
argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp3D2->hd,
0),s,0);}}s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(
s));}};_pop_region(r);}goto _LL1D6;_LL1F3: if(*((int*)_tmp3B7)!= 12)goto _LL1F5;
_tmp3D8=((struct Cyc_Absyn_Throw_e_struct*)_tmp3B7)->f1;_LL1F4: Cyc_Toc_exp_to_c(
nv,_tmp3D8);(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(
old_typ),Cyc_Toc_newthrow_exp(_tmp3D8),0))->r));goto _LL1D6;_LL1F5: if(*((int*)
_tmp3B7)!= 13)goto _LL1F7;_tmp3D9=((struct Cyc_Absyn_NoInstantiate_e_struct*)
_tmp3B7)->f1;_LL1F6: Cyc_Toc_exp_to_c(nv,_tmp3D9);goto _LL1D6;_LL1F7: if(*((int*)
_tmp3B7)!= 14)goto _LL1F9;_tmp3DA=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp3B7)->f1;
_tmp3DB=((struct Cyc_Absyn_Instantiate_e_struct*)_tmp3B7)->f2;_LL1F8: Cyc_Toc_exp_to_c(
nv,_tmp3DA);for(0;_tmp3DB != 0;_tmp3DB=_tmp3DB->tl){void*k=Cyc_Tcutil_typ_kind((
void*)_tmp3DB->hd);if(((k != (void*)6  && k != (void*)3) && k != (void*)4) && k != (
void*)5){{void*_tmp472=Cyc_Tcutil_compress((void*)_tmp3DB->hd);_LL288: if(_tmp472
<= (void*)4)goto _LL28C;if(*((int*)_tmp472)!= 1)goto _LL28A;_LL289: goto _LL28B;
_LL28A: if(*((int*)_tmp472)!= 2)goto _LL28C;_LL28B: continue;_LL28C:;_LL28D:(void*)(
e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct
Cyc_Core_Opt*)_check_null(e->topt))->v),_tmp3DA,0))->r));goto _LL287;_LL287:;}
break;}}goto _LL1D6;_LL1F9: if(*((int*)_tmp3B7)!= 15)goto _LL1FB;_tmp3DC=(void*)((
struct Cyc_Absyn_Cast_e_struct*)_tmp3B7)->f1;_tmp3DD=(void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp3B7)->f1);_tmp3DE=((struct Cyc_Absyn_Cast_e_struct*)_tmp3B7)->f2;_tmp3DF=((
struct Cyc_Absyn_Cast_e_struct*)_tmp3B7)->f3;_tmp3E0=(void*)((struct Cyc_Absyn_Cast_e_struct*)
_tmp3B7)->f4;_LL1FA: {void*old_t2=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp3DE->topt))->v;void*new_typ=*_tmp3DD;*_tmp3DD=Cyc_Toc_typ_to_c(new_typ);Cyc_Toc_exp_to_c(
nv,_tmp3DE);{struct _tuple0 _tmp474=({struct _tuple0 _tmp473;_tmp473.f1=Cyc_Tcutil_compress(
old_t2);_tmp473.f2=Cyc_Tcutil_compress(new_typ);_tmp473;});void*_tmp475;struct
Cyc_Absyn_PtrInfo _tmp476;void*_tmp477;struct Cyc_Absyn_PtrInfo _tmp478;void*
_tmp479;struct Cyc_Absyn_PtrInfo _tmp47A;void*_tmp47B;_LL28F: _tmp475=_tmp474.f1;
if(_tmp475 <= (void*)4)goto _LL291;if(*((int*)_tmp475)!= 4)goto _LL291;_tmp476=((
struct Cyc_Absyn_PointerType_struct*)_tmp475)->f1;_tmp477=_tmp474.f2;if(_tmp477 <= (
void*)4)goto _LL291;if(*((int*)_tmp477)!= 4)goto _LL291;_tmp478=((struct Cyc_Absyn_PointerType_struct*)
_tmp477)->f1;_LL290: {int _tmp47C=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(
0,(_tmp476.ptr_atts).nullable);int _tmp47D=((int(*)(int,struct Cyc_Absyn_Conref*x))
Cyc_Absyn_conref_def)(0,(_tmp478.ptr_atts).nullable);void*_tmp47E=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp476.ptr_atts).bounds);void*_tmp47F=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,(_tmp478.ptr_atts).bounds);int _tmp480=((int(*)(int,struct
Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp476.ptr_atts).zero_term);int
_tmp481=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,(_tmp478.ptr_atts).zero_term);{
struct _tuple0 _tmp483=({struct _tuple0 _tmp482;_tmp482.f1=_tmp47E;_tmp482.f2=
_tmp47F;_tmp482;});void*_tmp484;struct Cyc_Absyn_Exp*_tmp485;void*_tmp486;struct
Cyc_Absyn_Exp*_tmp487;void*_tmp488;struct Cyc_Absyn_Exp*_tmp489;void*_tmp48A;void*
_tmp48B;void*_tmp48C;struct Cyc_Absyn_Exp*_tmp48D;void*_tmp48E;void*_tmp48F;
_LL296: _tmp484=_tmp483.f1;if(_tmp484 <= (void*)1)goto _LL298;if(*((int*)_tmp484)!= 
0)goto _LL298;_tmp485=((struct Cyc_Absyn_Upper_b_struct*)_tmp484)->f1;_tmp486=
_tmp483.f2;if(_tmp486 <= (void*)1)goto _LL298;if(*((int*)_tmp486)!= 0)goto _LL298;
_tmp487=((struct Cyc_Absyn_Upper_b_struct*)_tmp486)->f1;_LL297: if((!Cyc_Evexp_c_can_eval(
_tmp485) || !Cyc_Evexp_c_can_eval(_tmp487)) && !Cyc_Evexp_same_const_exp(_tmp485,
_tmp487))({void*_tmp490=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp491="can't validate cast due to potential size differences";
_tag_dyneither(_tmp491,sizeof(char),54);}),_tag_dyneither(_tmp490,sizeof(void*),
0));});if(_tmp47C  && !_tmp47D){if(Cyc_Toc_is_toplevel(nv))({void*_tmp492=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*
_tmp493="can't do NULL-check conversion at top-level";_tag_dyneither(_tmp493,
sizeof(char),44);}),_tag_dyneither(_tmp492,sizeof(void*),0));});if(_tmp3E0 != (
void*)2)({struct Cyc_String_pa_struct _tmp496;_tmp496.tag=0;_tmp496.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));{void*_tmp494[1]={&
_tmp496};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({
const char*_tmp495="null-check conversion mis-classified: %s";_tag_dyneither(
_tmp495,sizeof(char),41);}),_tag_dyneither(_tmp494,sizeof(void*),1));}});{int
do_null_check=Cyc_Toc_need_null_check(_tmp3DE);if(do_null_check){if(!_tmp3DF)({
void*_tmp497=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp498="inserted null check due to implicit cast from * to @ type";
_tag_dyneither(_tmp498,sizeof(char),58);}),_tag_dyneither(_tmp497,sizeof(void*),
0));});(void*)(e->r=(void*)Cyc_Toc_cast_it_r(*_tmp3DD,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp499=_cycalloc(sizeof(*_tmp499));_tmp499->hd=_tmp3DE;
_tmp499->tl=0;_tmp499;}),0)));}}}goto _LL295;_LL298: _tmp488=_tmp483.f1;if(_tmp488
<= (void*)1)goto _LL29A;if(*((int*)_tmp488)!= 0)goto _LL29A;_tmp489=((struct Cyc_Absyn_Upper_b_struct*)
_tmp488)->f1;_tmp48A=_tmp483.f2;if((int)_tmp48A != 0)goto _LL29A;_LL299: if(!Cyc_Evexp_c_can_eval(
_tmp489))({void*_tmp49A=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp49B="cannot perform coercion since numelts cannot be determined statically.";
_tag_dyneither(_tmp49B,sizeof(char),71);}),_tag_dyneither(_tmp49A,sizeof(void*),
0));});if(_tmp3E0 == (void*)2)({struct Cyc_String_pa_struct _tmp49E;_tmp49E.tag=0;
_tmp49E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));{
void*_tmp49C[1]={& _tmp49E};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Tcutil_impos)(({const char*_tmp49D="conversion mis-classified as null-check: %s";
_tag_dyneither(_tmp49D,sizeof(char),44);}),_tag_dyneither(_tmp49C,sizeof(void*),
1));}});if(Cyc_Toc_is_toplevel(nv)){if((_tmp480  && !(_tmp478.elt_tq).real_const)
 && !_tmp481)_tmp489=Cyc_Absyn_prim2_exp((void*)2,_tmp489,Cyc_Absyn_uint_exp(1,0),
0);(void*)(e->r=(void*)((void*)(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp489,
_tmp3DE))->r));}else{struct Cyc_Absyn_Exp*_tmp49F=Cyc_Toc__tag_dyneither_e;if(
_tmp480){struct _tuple1*_tmp4A0=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp4A1=
Cyc_Absyn_var_exp(_tmp4A0,0);struct Cyc_Absyn_Exp*arg3;{void*_tmp4A2=(void*)
_tmp3DE->r;union Cyc_Absyn_Cnst_union _tmp4A3;_LL29F: if(*((int*)_tmp4A2)!= 0)goto
_LL2A1;_tmp4A3=((struct Cyc_Absyn_Const_e_struct*)_tmp4A2)->f1;if(((((struct Cyc_Absyn_Const_e_struct*)
_tmp4A2)->f1).String_c).tag != 5)goto _LL2A1;_LL2A0: arg3=_tmp489;goto _LL29E;_LL2A1:;
_LL2A2: arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,
_tmp3DE),({struct Cyc_Absyn_Exp*_tmp4A4[2];_tmp4A4[1]=_tmp489;_tmp4A4[0]=_tmp4A1;((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp4A4,sizeof(struct Cyc_Absyn_Exp*),2));}),0);goto _LL29E;_LL29E:;}if(!_tmp481
 && !(_tmp478.elt_tq).real_const)arg3=Cyc_Absyn_prim2_exp((void*)2,arg3,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Exp*_tmp4A5=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp478.elt_typ),0);struct Cyc_Absyn_Exp*_tmp4A6=Cyc_Absyn_fncall_exp(
_tmp49F,({struct Cyc_Absyn_Exp*_tmp4A8[3];_tmp4A8[2]=arg3;_tmp4A8[1]=_tmp4A5;
_tmp4A8[0]=_tmp4A1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp4A8,sizeof(struct Cyc_Absyn_Exp*),3));}),0);struct Cyc_Absyn_Stmt*
_tmp4A7=Cyc_Absyn_exp_stmt(_tmp4A6,0);_tmp4A7=Cyc_Absyn_declare_stmt(_tmp4A0,Cyc_Toc_typ_to_c(
old_t2),(struct Cyc_Absyn_Exp*)_tmp3DE,_tmp4A7,0);(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(
_tmp4A7));}}else{(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp49F,({struct Cyc_Absyn_Exp*
_tmp4A9[3];_tmp4A9[2]=_tmp489;_tmp4A9[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp478.elt_typ),0);_tmp4A9[0]=_tmp3DE;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A9,sizeof(struct Cyc_Absyn_Exp*),
3));})));}}goto _LL295;_LL29A: _tmp48B=_tmp483.f1;if((int)_tmp48B != 0)goto _LL29C;
_tmp48C=_tmp483.f2;if(_tmp48C <= (void*)1)goto _LL29C;if(*((int*)_tmp48C)!= 0)goto
_LL29C;_tmp48D=((struct Cyc_Absyn_Upper_b_struct*)_tmp48C)->f1;_LL29B: if(!Cyc_Evexp_c_can_eval(
_tmp48D))({void*_tmp4AA=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp4AB="cannot perform coercion since numelts cannot be determined statically.";
_tag_dyneither(_tmp4AB,sizeof(char),71);}),_tag_dyneither(_tmp4AA,sizeof(void*),
0));});if(Cyc_Toc_is_toplevel(nv))({void*_tmp4AC=0;((int(*)(struct _dyneither_ptr
fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp4AD="can't coerce t? to t* or t@ at the top-level";
_tag_dyneither(_tmp4AD,sizeof(char),45);}),_tag_dyneither(_tmp4AC,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*_tmp4AE=_tmp48D;if(_tmp480  && !_tmp481)_tmp4AE=Cyc_Absyn_add_exp(
_tmp48D,Cyc_Absyn_uint_exp(1,0),0);{struct Cyc_Absyn_Exp*_tmp4AF=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmp4B0=Cyc_Absyn_fncall_exp(_tmp4AF,({struct Cyc_Absyn_Exp*
_tmp4B2[3];_tmp4B2[2]=_tmp4AE;_tmp4B2[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp476.elt_typ),0);_tmp4B2[0]=_tmp3DE;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4B2,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);if(_tmp47D)(void*)(_tmp4B0->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp4B1=_cycalloc(sizeof(*_tmp4B1));_tmp4B1->hd=Cyc_Absyn_copy_exp(
_tmp4B0);_tmp4B1->tl=0;_tmp4B1;})));(void*)(e->r=(void*)Cyc_Toc_cast_it_r(*
_tmp3DD,_tmp4B0));goto _LL295;}}_LL29C: _tmp48E=_tmp483.f1;if((int)_tmp48E != 0)
goto _LL295;_tmp48F=_tmp483.f2;if((int)_tmp48F != 0)goto _LL295;_LL29D: DynCast: if((
_tmp480  && !_tmp481) && !(_tmp478.elt_tq).real_const){if(Cyc_Toc_is_toplevel(nv))({
void*_tmp4B3=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({
const char*_tmp4B4="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";
_tag_dyneither(_tmp4B4,sizeof(char),70);}),_tag_dyneither(_tmp4B3,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*_tmp4B5=Cyc_Toc__dyneither_ptr_decrease_size_e;(void*)(
e->r=(void*)Cyc_Toc_fncall_exp_r(_tmp4B5,({struct Cyc_Absyn_Exp*_tmp4B6[3];
_tmp4B6[2]=Cyc_Absyn_uint_exp(1,0);_tmp4B6[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c((
void*)_tmp476.elt_typ),0);_tmp4B6[0]=_tmp3DE;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4B6,sizeof(struct Cyc_Absyn_Exp*),
3));})));}}goto _LL295;_LL295:;}goto _LL28E;}_LL291: _tmp479=_tmp474.f1;if(_tmp479
<= (void*)4)goto _LL293;if(*((int*)_tmp479)!= 4)goto _LL293;_tmp47A=((struct Cyc_Absyn_PointerType_struct*)
_tmp479)->f1;_tmp47B=_tmp474.f2;if(_tmp47B <= (void*)4)goto _LL293;if(*((int*)
_tmp47B)!= 5)goto _LL293;_LL292:{void*_tmp4B7=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,(
_tmp47A.ptr_atts).bounds);_LL2A4: if((int)_tmp4B7 != 0)goto _LL2A6;_LL2A5:(void*)(
_tmp3DE->r=(void*)Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp((void*)_tmp3DE->r,
_tmp3DE->loc),Cyc_Toc_curr_sp));goto _LL2A3;_LL2A6:;_LL2A7: goto _LL2A3;_LL2A3:;}
goto _LL28E;_LL293:;_LL294: goto _LL28E;_LL28E:;}goto _LL1D6;}_LL1FB: if(*((int*)
_tmp3B7)!= 16)goto _LL1FD;_tmp3E1=((struct Cyc_Absyn_Address_e_struct*)_tmp3B7)->f1;
_LL1FC:{void*_tmp4B8=(void*)_tmp3E1->r;struct _tuple1*_tmp4B9;struct Cyc_List_List*
_tmp4BA;struct Cyc_List_List*_tmp4BB;struct Cyc_List_List*_tmp4BC;_LL2A9: if(*((int*)
_tmp4B8)!= 30)goto _LL2AB;_tmp4B9=((struct Cyc_Absyn_Struct_e_struct*)_tmp4B8)->f1;
_tmp4BA=((struct Cyc_Absyn_Struct_e_struct*)_tmp4B8)->f2;_tmp4BB=((struct Cyc_Absyn_Struct_e_struct*)
_tmp4B8)->f3;_LL2AA: if(Cyc_Toc_is_toplevel(nv))({struct Cyc_String_pa_struct
_tmp4BF;_tmp4BF.tag=0;_tmp4BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp3E1->loc));{void*_tmp4BD[1]={& _tmp4BF};((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*
_tmp4BE="%s: & on non-identifiers at the top-level";_tag_dyneither(_tmp4BE,
sizeof(char),42);}),_tag_dyneither(_tmp4BD,sizeof(void*),1));}});(void*)(e->r=(
void*)((void*)(Cyc_Toc_init_struct(nv,(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp3E1->topt))->v,_tmp4BA != 0,1,0,_tmp4BB,_tmp4B9))->r));goto _LL2A8;_LL2AB: if(*((
int*)_tmp4B8)!= 26)goto _LL2AD;_tmp4BC=((struct Cyc_Absyn_Tuple_e_struct*)_tmp4B8)->f1;
_LL2AC: if(Cyc_Toc_is_toplevel(nv))({struct Cyc_String_pa_struct _tmp4C2;_tmp4C2.tag=
0;_tmp4C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(
_tmp3E1->loc));{void*_tmp4C0[1]={& _tmp4C2};((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*_tmp4C1="%s: & on non-identifiers at the top-level";
_tag_dyneither(_tmp4C1,sizeof(char),42);}),_tag_dyneither(_tmp4C0,sizeof(void*),
1));}});(void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,1,0,_tmp4BC))->r));
goto _LL2A8;_LL2AD:;_LL2AE: Cyc_Toc_exp_to_c(nv,_tmp3E1);if(!Cyc_Absyn_is_lvalue(
_tmp3E1)){((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*
f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp3E1,0,Cyc_Toc_address_lvalue,
1);(void*)(e->r=(void*)Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),_tmp3E1));}goto _LL2A8;_LL2A8:;}goto _LL1D6;_LL1FD: if(*((
int*)_tmp3B7)!= 17)goto _LL1FF;_tmp3E2=((struct Cyc_Absyn_New_e_struct*)_tmp3B7)->f1;
_tmp3E3=((struct Cyc_Absyn_New_e_struct*)_tmp3B7)->f2;_LL1FE: if(Cyc_Toc_is_toplevel(
nv))({struct Cyc_String_pa_struct _tmp4C5;_tmp4C5.tag=0;_tmp4C5.f1=(struct
_dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp3E3->loc));{
void*_tmp4C3[1]={& _tmp4C5};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr
ap))Cyc_Toc_unimp)(({const char*_tmp4C4="%s: new at top-level";_tag_dyneither(
_tmp4C4,sizeof(char),21);}),_tag_dyneither(_tmp4C3,sizeof(void*),1));}});{void*
_tmp4C6=(void*)_tmp3E3->r;struct Cyc_List_List*_tmp4C7;struct Cyc_Absyn_Vardecl*
_tmp4C8;struct Cyc_Absyn_Exp*_tmp4C9;struct Cyc_Absyn_Exp*_tmp4CA;int _tmp4CB;
struct _tuple1*_tmp4CC;struct Cyc_List_List*_tmp4CD;struct Cyc_List_List*_tmp4CE;
struct Cyc_List_List*_tmp4CF;_LL2B0: if(*((int*)_tmp4C6)!= 28)goto _LL2B2;_tmp4C7=((
struct Cyc_Absyn_Array_e_struct*)_tmp4C6)->f1;_LL2B1: {struct _tuple1*_tmp4D0=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4D1=Cyc_Absyn_var_exp(_tmp4D0,0);struct Cyc_Absyn_Stmt*
_tmp4D2=Cyc_Toc_init_array(nv,_tmp4D1,_tmp4C7,Cyc_Absyn_exp_stmt(_tmp4D1,0));
void*old_elt_typ;{void*_tmp4D3=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp4D4;void*_tmp4D5;struct Cyc_Absyn_Tqual _tmp4D6;struct Cyc_Absyn_PtrAtts _tmp4D7;
struct Cyc_Absyn_Conref*_tmp4D8;_LL2BB: if(_tmp4D3 <= (void*)4)goto _LL2BD;if(*((int*)
_tmp4D3)!= 4)goto _LL2BD;_tmp4D4=((struct Cyc_Absyn_PointerType_struct*)_tmp4D3)->f1;
_tmp4D5=(void*)_tmp4D4.elt_typ;_tmp4D6=_tmp4D4.elt_tq;_tmp4D7=_tmp4D4.ptr_atts;
_tmp4D8=_tmp4D7.zero_term;_LL2BC: old_elt_typ=_tmp4D5;goto _LL2BA;_LL2BD:;_LL2BE:
old_elt_typ=({void*_tmp4D9=0;Cyc_Toc_toc_impos(({const char*_tmp4DA="exp_to_c:new array expression doesn't have ptr type";
_tag_dyneither(_tmp4DA,sizeof(char),52);}),_tag_dyneither(_tmp4D9,sizeof(void*),
0));});_LL2BA:;}{void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*_tmp4DB=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp4DC=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
elt_typ,0),Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(
_tmp4C7),0),0);struct Cyc_Absyn_Exp*e1;if(_tmp3E2 == 0  || Cyc_Absyn_no_regions)e1=
Cyc_Toc_malloc_exp(old_elt_typ,_tmp4DC);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)
_tmp3E2;Cyc_Toc_exp_to_c(nv,r);e1=Cyc_Toc_rmalloc_exp(r,_tmp4DC);}(void*)(e->r=(
void*)Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp4D0,_tmp4DB,(struct Cyc_Absyn_Exp*)
e1,_tmp4D2,0)));goto _LL2AF;}}_LL2B2: if(*((int*)_tmp4C6)!= 29)goto _LL2B4;_tmp4C8=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp4C6)->f1;_tmp4C9=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp4C6)->f2;_tmp4CA=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp4C6)->f3;
_tmp4CB=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp4C6)->f4;_LL2B3: {int
is_dyneither_ptr=0;{void*_tmp4DD=Cyc_Tcutil_compress(old_typ);struct Cyc_Absyn_PtrInfo
_tmp4DE;void*_tmp4DF;struct Cyc_Absyn_Tqual _tmp4E0;struct Cyc_Absyn_PtrAtts _tmp4E1;
struct Cyc_Absyn_Conref*_tmp4E2;struct Cyc_Absyn_Conref*_tmp4E3;_LL2C0: if(_tmp4DD
<= (void*)4)goto _LL2C2;if(*((int*)_tmp4DD)!= 4)goto _LL2C2;_tmp4DE=((struct Cyc_Absyn_PointerType_struct*)
_tmp4DD)->f1;_tmp4DF=(void*)_tmp4DE.elt_typ;_tmp4E0=_tmp4DE.elt_tq;_tmp4E1=
_tmp4DE.ptr_atts;_tmp4E2=_tmp4E1.bounds;_tmp4E3=_tmp4E1.zero_term;_LL2C1:
is_dyneither_ptr=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp4E2)== (void*)0;
goto _LL2BF;_LL2C2:;_LL2C3:({void*_tmp4E4=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp4E5="exp_to_c: comprehension not an array type";
_tag_dyneither(_tmp4E5,sizeof(char),42);}),_tag_dyneither(_tmp4E4,sizeof(void*),
0));});_LL2BF:;}{struct _tuple1*max=Cyc_Toc_temp_var();struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp4CA->topt))->v;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);void*ptr_typ=Cyc_Absyn_cstar_typ(
elt_typ,Cyc_Toc_mt_tq);Cyc_Toc_exp_to_c(nv,_tmp4C9);{struct Cyc_Absyn_Exp*_tmp4E6=
Cyc_Absyn_var_exp(max,0);if(_tmp4CB)_tmp4E6=Cyc_Absyn_add_exp(_tmp4E6,Cyc_Absyn_uint_exp(
1,0),0);{struct Cyc_Absyn_Stmt*s=Cyc_Toc_init_comprehension(nv,Cyc_Absyn_var_exp(
a,0),_tmp4C8,Cyc_Absyn_var_exp(max,0),_tmp4CA,_tmp4CB,Cyc_Toc_skip_stmt_dl(),1);{
struct _RegionHandle _tmp4E7=_new_region("r");struct _RegionHandle*r=& _tmp4E7;
_push_region(r);{struct Cyc_List_List*decls=({struct Cyc_List_List*_tmp4F8=
_region_malloc(r,sizeof(*_tmp4F8));_tmp4F8->hd=({struct _tuple11*_tmp4F9=
_region_malloc(r,sizeof(*_tmp4F9));_tmp4F9->f1=max;_tmp4F9->f2=Cyc_Absyn_uint_typ;
_tmp4F9->f3=(struct Cyc_Absyn_Exp*)_tmp4C9;_tmp4F9;});_tmp4F8->tl=0;_tmp4F8;});
struct Cyc_Absyn_Exp*ai;if(_tmp3E2 == 0  || Cyc_Absyn_no_regions)ai=Cyc_Toc_malloc_exp(
old_elt_typ,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*
_tmp4E8[2];_tmp4E8[1]=_tmp4E6;_tmp4E8[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp4E8,sizeof(struct Cyc_Absyn_Exp*),2));}),0));else{struct Cyc_Absyn_Exp*r=(
struct Cyc_Absyn_Exp*)_tmp3E2;Cyc_Toc_exp_to_c(nv,r);ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(
Cyc_Toc__check_times_e,({struct Cyc_Absyn_Exp*_tmp4E9[2];_tmp4E9[1]=_tmp4E6;
_tmp4E9[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4E9,sizeof(struct Cyc_Absyn_Exp*),
2));}),0));}{struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);decls=({
struct Cyc_List_List*_tmp4EA=_region_malloc(r,sizeof(*_tmp4EA));_tmp4EA->hd=({
struct _tuple11*_tmp4EB=_region_malloc(r,sizeof(*_tmp4EB));_tmp4EB->f1=a;_tmp4EB->f2=
ptr_typ;_tmp4EB->f3=(struct Cyc_Absyn_Exp*)ainit;_tmp4EB;});_tmp4EA->tl=decls;
_tmp4EA;});if(is_dyneither_ptr){struct _tuple1*_tmp4EC=Cyc_Toc_temp_var();void*
_tmp4ED=Cyc_Toc_typ_to_c(old_typ);struct Cyc_Absyn_Exp*_tmp4EE=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmp4EF=Cyc_Absyn_fncall_exp(_tmp4EE,({struct Cyc_Absyn_Exp*
_tmp4F2[3];_tmp4F2[2]=_tmp4E6;_tmp4F2[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0);
_tmp4F2[0]=Cyc_Absyn_var_exp(a,0);((struct Cyc_List_List*(*)(struct _dyneither_ptr))
Cyc_List_list)(_tag_dyneither(_tmp4F2,sizeof(struct Cyc_Absyn_Exp*),3));}),0);
decls=({struct Cyc_List_List*_tmp4F0=_region_malloc(r,sizeof(*_tmp4F0));_tmp4F0->hd=({
struct _tuple11*_tmp4F1=_region_malloc(r,sizeof(*_tmp4F1));_tmp4F1->f1=_tmp4EC;
_tmp4F1->f2=_tmp4ED;_tmp4F1->f3=(struct Cyc_Absyn_Exp*)_tmp4EF;_tmp4F1;});_tmp4F0->tl=
decls;_tmp4F0;});s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(
_tmp4EC,0),0),0);}else{s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(
a,0),0),0);}{struct Cyc_List_List*_tmp4F3=decls;for(0;_tmp4F3 != 0;_tmp4F3=_tmp4F3->tl){
struct _tuple1*_tmp4F5;void*_tmp4F6;struct Cyc_Absyn_Exp*_tmp4F7;struct _tuple11
_tmp4F4=*((struct _tuple11*)_tmp4F3->hd);_tmp4F5=_tmp4F4.f1;_tmp4F6=_tmp4F4.f2;
_tmp4F7=_tmp4F4.f3;s=Cyc_Absyn_declare_stmt(_tmp4F5,_tmp4F6,_tmp4F7,s,0);}}(void*)(
e->r=(void*)Cyc_Toc_stmt_exp_r(s));}};_pop_region(r);}goto _LL2AF;}}}}_LL2B4: if(*((
int*)_tmp4C6)!= 30)goto _LL2B6;_tmp4CC=((struct Cyc_Absyn_Struct_e_struct*)_tmp4C6)->f1;
_tmp4CD=((struct Cyc_Absyn_Struct_e_struct*)_tmp4C6)->f2;_tmp4CE=((struct Cyc_Absyn_Struct_e_struct*)
_tmp4C6)->f3;_LL2B5:(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(nv,(void*)((
struct Cyc_Core_Opt*)_check_null(_tmp3E3->topt))->v,_tmp4CD != 0,1,_tmp3E2,_tmp4CE,
_tmp4CC))->r));goto _LL2AF;_LL2B6: if(*((int*)_tmp4C6)!= 26)goto _LL2B8;_tmp4CF=((
struct Cyc_Absyn_Tuple_e_struct*)_tmp4C6)->f1;_LL2B7:(void*)(e->r=(void*)((void*)(
Cyc_Toc_init_tuple(nv,1,_tmp3E2,_tmp4CF))->r));goto _LL2AF;_LL2B8:;_LL2B9: {void*
old_elt_typ=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp3E3->topt))->v;void*
elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);struct _tuple1*_tmp4FA=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4FB=Cyc_Absyn_var_exp(_tmp4FA,0);struct Cyc_Absyn_Exp*
mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp4FB,0),0);struct Cyc_Absyn_Exp*
inner_mexp=mexp;if(_tmp3E2 == 0  || Cyc_Absyn_no_regions)mexp=Cyc_Toc_malloc_exp(
old_elt_typ,mexp);else{struct Cyc_Absyn_Exp*r=(struct Cyc_Absyn_Exp*)_tmp3E2;Cyc_Toc_exp_to_c(
nv,r);mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{int done=0;{void*_tmp4FC=(void*)_tmp3E3->r;
void*_tmp4FD;struct Cyc_Absyn_Exp*_tmp4FE;_LL2C5: if(*((int*)_tmp4FC)!= 15)goto
_LL2C7;_tmp4FD=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp4FC)->f1;_tmp4FE=((
struct Cyc_Absyn_Cast_e_struct*)_tmp4FC)->f2;_LL2C6:{struct _tuple0 _tmp500=({
struct _tuple0 _tmp4FF;_tmp4FF.f1=Cyc_Tcutil_compress(_tmp4FD);_tmp4FF.f2=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp4FE->topt))->v);_tmp4FF;});void*
_tmp501;struct Cyc_Absyn_PtrInfo _tmp502;void*_tmp503;struct Cyc_Absyn_PtrAtts
_tmp504;struct Cyc_Absyn_Conref*_tmp505;void*_tmp506;struct Cyc_Absyn_PtrInfo
_tmp507;struct Cyc_Absyn_PtrAtts _tmp508;struct Cyc_Absyn_Conref*_tmp509;_LL2CA:
_tmp501=_tmp500.f1;if(_tmp501 <= (void*)4)goto _LL2CC;if(*((int*)_tmp501)!= 4)goto
_LL2CC;_tmp502=((struct Cyc_Absyn_PointerType_struct*)_tmp501)->f1;_tmp503=(void*)
_tmp502.elt_typ;_tmp504=_tmp502.ptr_atts;_tmp505=_tmp504.bounds;_tmp506=_tmp500.f2;
if(_tmp506 <= (void*)4)goto _LL2CC;if(*((int*)_tmp506)!= 4)goto _LL2CC;_tmp507=((
struct Cyc_Absyn_PointerType_struct*)_tmp506)->f1;_tmp508=_tmp507.ptr_atts;
_tmp509=_tmp508.bounds;_LL2CB:{struct _tuple0 _tmp50B=({struct _tuple0 _tmp50A;
_tmp50A.f1=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp505);_tmp50A.f2=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp509);_tmp50A;});void*_tmp50C;void*_tmp50D;struct Cyc_Absyn_Exp*
_tmp50E;_LL2CF: _tmp50C=_tmp50B.f1;if((int)_tmp50C != 0)goto _LL2D1;_tmp50D=_tmp50B.f2;
if(_tmp50D <= (void*)1)goto _LL2D1;if(*((int*)_tmp50D)!= 0)goto _LL2D1;_tmp50E=((
struct Cyc_Absyn_Upper_b_struct*)_tmp50D)->f1;_LL2D0: Cyc_Toc_exp_to_c(nv,_tmp4FE);(
void*)(inner_mexp->r=(void*)Cyc_Toc_sizeoftyp_exp_r(elt_typ));done=1;{struct Cyc_Absyn_Exp*
_tmp50F=Cyc_Toc__init_dyneither_ptr_e;(void*)(e->r=(void*)Cyc_Toc_fncall_exp_r(
_tmp50F,({struct Cyc_Absyn_Exp*_tmp510[4];_tmp510[3]=_tmp50E;_tmp510[2]=Cyc_Absyn_sizeoftyp_exp(
Cyc_Toc_typ_to_c(_tmp503),0);_tmp510[1]=_tmp4FE;_tmp510[0]=mexp;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp510,sizeof(struct Cyc_Absyn_Exp*),
4));})));goto _LL2CE;}_LL2D1:;_LL2D2: goto _LL2CE;_LL2CE:;}goto _LL2C9;_LL2CC:;
_LL2CD: goto _LL2C9;_LL2C9:;}goto _LL2C4;_LL2C7:;_LL2C8: goto _LL2C4;_LL2C4:;}if(!
done){struct Cyc_Absyn_Stmt*_tmp511=Cyc_Absyn_exp_stmt(_tmp4FB,0);struct Cyc_Absyn_Exp*
_tmp512=Cyc_Absyn_signed_int_exp(0,0);Cyc_Toc_exp_to_c(nv,_tmp3E3);_tmp511=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp4FB,_tmp512,0),_tmp3E3,0),
_tmp511,0);{void*_tmp513=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);(void*)(e->r=(
void*)Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp4FA,_tmp513,(struct Cyc_Absyn_Exp*)
mexp,_tmp511,0)));}}goto _LL2AF;}}_LL2AF:;}goto _LL1D6;_LL1FF: if(*((int*)_tmp3B7)
!= 19)goto _LL201;_tmp3E4=((struct Cyc_Absyn_Sizeofexp_e_struct*)_tmp3B7)->f1;
_LL200: Cyc_Toc_exp_to_c(nv,_tmp3E4);goto _LL1D6;_LL201: if(*((int*)_tmp3B7)!= 18)
goto _LL203;_tmp3E5=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)_tmp3B7)->f1;
_LL202:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Sizeoftyp_e_struct*_tmp514=
_cycalloc(sizeof(*_tmp514));_tmp514[0]=({struct Cyc_Absyn_Sizeoftyp_e_struct
_tmp515;_tmp515.tag=18;_tmp515.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp3E5);_tmp515;});
_tmp514;})));goto _LL1D6;_LL203: if(*((int*)_tmp3B7)!= 21)goto _LL205;_LL204:({void*
_tmp516=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp517="__gen() in code generator";_tag_dyneither(_tmp517,sizeof(char),
26);}),_tag_dyneither(_tmp516,sizeof(void*),0));});_LL205: if(*((int*)_tmp3B7)!= 
20)goto _LL207;_tmp3E6=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp3B7)->f1;
_tmp3E7=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp3B7)->f2;if(*((int*)
_tmp3E7)!= 0)goto _LL207;_tmp3E8=((struct Cyc_Absyn_StructField_struct*)_tmp3E7)->f1;
_LL206:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp518=
_cycalloc(sizeof(*_tmp518));_tmp518[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp519;_tmp519.tag=20;_tmp519.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp3E6);_tmp519.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp51A=_cycalloc(sizeof(*
_tmp51A));_tmp51A[0]=({struct Cyc_Absyn_StructField_struct _tmp51B;_tmp51B.tag=0;
_tmp51B.f1=_tmp3E8;_tmp51B;});_tmp51A;}));_tmp519;});_tmp518;})));goto _LL1D6;
_LL207: if(*((int*)_tmp3B7)!= 20)goto _LL209;_tmp3E9=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)
_tmp3B7)->f1;_tmp3EA=(void*)((struct Cyc_Absyn_Offsetof_e_struct*)_tmp3B7)->f2;
if(*((int*)_tmp3EA)!= 1)goto _LL209;_tmp3EB=((struct Cyc_Absyn_TupleIndex_struct*)
_tmp3EA)->f1;_LL208:{void*_tmp51C=Cyc_Tcutil_compress(_tmp3E9);struct Cyc_Absyn_AggrInfo
_tmp51D;union Cyc_Absyn_AggrInfoU_union _tmp51E;struct Cyc_List_List*_tmp51F;_LL2D4:
if(_tmp51C <= (void*)4)goto _LL2DC;if(*((int*)_tmp51C)!= 10)goto _LL2D6;_tmp51D=((
struct Cyc_Absyn_AggrType_struct*)_tmp51C)->f1;_tmp51E=_tmp51D.aggr_info;_LL2D5: {
struct Cyc_Absyn_Aggrdecl*_tmp520=Cyc_Absyn_get_known_aggrdecl(_tmp51E);if(
_tmp520->impl == 0)({void*_tmp521=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp522="struct fields must be known";
_tag_dyneither(_tmp522,sizeof(char),28);}),_tag_dyneither(_tmp521,sizeof(void*),
0));});_tmp51F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp520->impl))->fields;
goto _LL2D7;}_LL2D6: if(*((int*)_tmp51C)!= 11)goto _LL2D8;_tmp51F=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp51C)->f2;_LL2D7: {struct Cyc_Absyn_Aggrfield*_tmp523=((struct Cyc_Absyn_Aggrfield*(*)(
struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp51F,(int)_tmp3EB);(void*)(e->r=(
void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp524=_cycalloc(sizeof(*
_tmp524));_tmp524[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp525;_tmp525.tag=20;
_tmp525.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp3E9);_tmp525.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp526=_cycalloc(sizeof(*_tmp526));_tmp526[0]=({
struct Cyc_Absyn_StructField_struct _tmp527;_tmp527.tag=0;_tmp527.f1=_tmp523->name;
_tmp527;});_tmp526;}));_tmp525;});_tmp524;})));goto _LL2D3;}_LL2D8: if(*((int*)
_tmp51C)!= 9)goto _LL2DA;_LL2D9:(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp528=_cycalloc(sizeof(*_tmp528));_tmp528[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp529;_tmp529.tag=20;_tmp529.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp3E9);_tmp529.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp52A=_cycalloc(sizeof(*
_tmp52A));_tmp52A[0]=({struct Cyc_Absyn_StructField_struct _tmp52B;_tmp52B.tag=0;
_tmp52B.f1=Cyc_Absyn_fieldname((int)(_tmp3EB + 1));_tmp52B;});_tmp52A;}));_tmp529;});
_tmp528;})));goto _LL2D3;_LL2DA: if(*((int*)_tmp51C)!= 3)goto _LL2DC;_LL2DB: if(
_tmp3EB == 0)(void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*
_tmp52C=_cycalloc(sizeof(*_tmp52C));_tmp52C[0]=({struct Cyc_Absyn_Offsetof_e_struct
_tmp52D;_tmp52D.tag=20;_tmp52D.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp3E9);_tmp52D.f2=(
void*)((void*)({struct Cyc_Absyn_StructField_struct*_tmp52E=_cycalloc(sizeof(*
_tmp52E));_tmp52E[0]=({struct Cyc_Absyn_StructField_struct _tmp52F;_tmp52F.tag=0;
_tmp52F.f1=Cyc_Toc_tag_sp;_tmp52F;});_tmp52E;}));_tmp52D;});_tmp52C;})));else{(
void*)(e->r=(void*)((void*)({struct Cyc_Absyn_Offsetof_e_struct*_tmp530=_cycalloc(
sizeof(*_tmp530));_tmp530[0]=({struct Cyc_Absyn_Offsetof_e_struct _tmp531;_tmp531.tag=
20;_tmp531.f1=(void*)Cyc_Toc_typ_to_c_array(_tmp3E9);_tmp531.f2=(void*)((void*)({
struct Cyc_Absyn_StructField_struct*_tmp532=_cycalloc(sizeof(*_tmp532));_tmp532[0]=({
struct Cyc_Absyn_StructField_struct _tmp533;_tmp533.tag=0;_tmp533.f1=Cyc_Absyn_fieldname((
int)_tmp3EB);_tmp533;});_tmp532;}));_tmp531;});_tmp530;})));}goto _LL2D3;_LL2DC:;
_LL2DD:({void*_tmp534=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))
Cyc_Toc_toc_impos)(({const char*_tmp535="impossible type for offsetof tuple index";
_tag_dyneither(_tmp535,sizeof(char),41);}),_tag_dyneither(_tmp534,sizeof(void*),
0));});_LL2D3:;}goto _LL1D6;_LL209: if(*((int*)_tmp3B7)!= 22)goto _LL20B;_tmp3EC=((
struct Cyc_Absyn_Deref_e_struct*)_tmp3B7)->f1;_LL20A: {void*_tmp536=Cyc_Tcutil_compress((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3EC->topt))->v);{void*_tmp537=
_tmp536;struct Cyc_Absyn_PtrInfo _tmp538;void*_tmp539;struct Cyc_Absyn_Tqual _tmp53A;
struct Cyc_Absyn_PtrAtts _tmp53B;void*_tmp53C;struct Cyc_Absyn_Conref*_tmp53D;
struct Cyc_Absyn_Conref*_tmp53E;struct Cyc_Absyn_Conref*_tmp53F;_LL2DF: if(_tmp537
<= (void*)4)goto _LL2E1;if(*((int*)_tmp537)!= 4)goto _LL2E1;_tmp538=((struct Cyc_Absyn_PointerType_struct*)
_tmp537)->f1;_tmp539=(void*)_tmp538.elt_typ;_tmp53A=_tmp538.elt_tq;_tmp53B=
_tmp538.ptr_atts;_tmp53C=(void*)_tmp53B.rgn;_tmp53D=_tmp53B.nullable;_tmp53E=
_tmp53B.bounds;_tmp53F=_tmp53B.zero_term;_LL2E0:{void*_tmp540=Cyc_Absyn_conref_def(
Cyc_Absyn_bounds_one,_tmp53E);struct Cyc_Absyn_Exp*_tmp541;_LL2E4: if(_tmp540 <= (
void*)1)goto _LL2E6;if(*((int*)_tmp540)!= 0)goto _LL2E6;_tmp541=((struct Cyc_Absyn_Upper_b_struct*)
_tmp540)->f1;_LL2E5: {int do_null_check=Cyc_Toc_need_null_check(_tmp3EC);Cyc_Toc_exp_to_c(
nv,_tmp3EC);if(do_null_check){if(Cyc_Toc_warn_all_null_deref)({void*_tmp542=0;
Cyc_Tcutil_warn(e->loc,({const char*_tmp543="inserted null check due to dereference";
_tag_dyneither(_tmp543,sizeof(char),39);}),_tag_dyneither(_tmp542,sizeof(void*),
0));});(void*)(_tmp3EC->r=(void*)Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp536),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_null_e,({struct Cyc_List_List*_tmp544=_cycalloc(sizeof(*_tmp544));
_tmp544->hd=Cyc_Absyn_copy_exp(_tmp3EC);_tmp544->tl=0;_tmp544;}),0)));}if(!((int(*)(
int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp53F)){unsigned int
_tmp546;int _tmp547;struct _tuple5 _tmp545=Cyc_Evexp_eval_const_uint_exp(_tmp541);
_tmp546=_tmp545.f1;_tmp547=_tmp545.f2;if(!_tmp547  || _tmp546 <= 0)({void*_tmp548=
0;Cyc_Tcutil_terr(e->loc,({const char*_tmp549="cannot determine dereference is in bounds";
_tag_dyneither(_tmp549,sizeof(char),42);}),_tag_dyneither(_tmp548,sizeof(void*),
0));});}goto _LL2E3;}_LL2E6: if((int)_tmp540 != 0)goto _LL2E3;_LL2E7: {struct Cyc_Absyn_Exp*
_tmp54A=Cyc_Absyn_uint_exp(0,0);_tmp54A->topt=({struct Cyc_Core_Opt*_tmp54B=
_cycalloc(sizeof(*_tmp54B));_tmp54B->v=(void*)Cyc_Absyn_uint_typ;_tmp54B;});(
void*)(e->r=(void*)Cyc_Toc_subscript_exp_r(_tmp3EC,_tmp54A));Cyc_Toc_exp_to_c(nv,
e);goto _LL2E3;}_LL2E3:;}goto _LL2DE;_LL2E1:;_LL2E2:({void*_tmp54C=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp54D="exp_to_c: Deref: non-pointer";_tag_dyneither(_tmp54D,sizeof(char),29);}),
_tag_dyneither(_tmp54C,sizeof(void*),0));});_LL2DE:;}goto _LL1D6;}_LL20B: if(*((
int*)_tmp3B7)!= 23)goto _LL20D;_tmp3ED=((struct Cyc_Absyn_AggrMember_e_struct*)
_tmp3B7)->f1;_tmp3EE=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp3B7)->f2;_LL20C:
Cyc_Toc_exp_to_c(nv,_tmp3ED);if(Cyc_Toc_is_poly_project(e))(void*)(e->r=(void*)((
void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,0),0))->r));goto _LL1D6;
_LL20D: if(*((int*)_tmp3B7)!= 24)goto _LL20F;_tmp3EF=((struct Cyc_Absyn_AggrArrow_e_struct*)
_tmp3B7)->f1;_tmp3F0=((struct Cyc_Absyn_AggrArrow_e_struct*)_tmp3B7)->f2;_LL20E: {
void*e1typ=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)_check_null(_tmp3EF->topt))->v);
int do_null_check=Cyc_Toc_need_null_check(_tmp3EF);Cyc_Toc_exp_to_c(nv,_tmp3EF);{
int is_poly=Cyc_Toc_is_poly_project(e);void*_tmp54F;struct Cyc_Absyn_Tqual _tmp550;
struct Cyc_Absyn_PtrAtts _tmp551;void*_tmp552;struct Cyc_Absyn_Conref*_tmp553;
struct Cyc_Absyn_Conref*_tmp554;struct Cyc_Absyn_Conref*_tmp555;struct Cyc_Absyn_PtrInfo
_tmp54E=Cyc_Toc_get_ptr_type(e1typ);_tmp54F=(void*)_tmp54E.elt_typ;_tmp550=
_tmp54E.elt_tq;_tmp551=_tmp54E.ptr_atts;_tmp552=(void*)_tmp551.rgn;_tmp553=
_tmp551.nullable;_tmp554=_tmp551.bounds;_tmp555=_tmp551.zero_term;{void*_tmp556=
Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp554);struct Cyc_Absyn_Exp*_tmp557;
_LL2E9: if(_tmp556 <= (void*)1)goto _LL2EB;if(*((int*)_tmp556)!= 0)goto _LL2EB;
_tmp557=((struct Cyc_Absyn_Upper_b_struct*)_tmp556)->f1;_LL2EA: {unsigned int
_tmp559;int _tmp55A;struct _tuple5 _tmp558=Cyc_Evexp_eval_const_uint_exp(_tmp557);
_tmp559=_tmp558.f1;_tmp55A=_tmp558.f2;if(_tmp55A){if(_tmp559 < 1)({void*_tmp55B=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp55C="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dyneither(
_tmp55C,sizeof(char),44);}),_tag_dyneither(_tmp55B,sizeof(void*),0));});if(
do_null_check){if(Cyc_Toc_warn_all_null_deref)({void*_tmp55D=0;Cyc_Tcutil_warn(e->loc,({
const char*_tmp55E="inserted null check due to dereference";_tag_dyneither(
_tmp55E,sizeof(char),39);}),_tag_dyneither(_tmp55D,sizeof(void*),0));});(void*)(
e->r=(void*)Cyc_Toc_aggrarrow_exp_r(Cyc_Toc_cast_it(Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp3EF->topt))->v),Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,({
struct Cyc_List_List*_tmp55F=_cycalloc(sizeof(*_tmp55F));_tmp55F->hd=_tmp3EF;
_tmp55F->tl=0;_tmp55F;}),0)),_tmp3F0));}}else{if(!Cyc_Evexp_c_can_eval(_tmp557))({
void*_tmp560=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp561="cannot determine pointer dereference in bounds";
_tag_dyneither(_tmp561,sizeof(char),47);}),_tag_dyneither(_tmp560,sizeof(void*),
0));});(void*)(e->r=(void*)Cyc_Toc_aggrarrow_exp_r(Cyc_Toc_cast_it(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3EF->topt))->v),Cyc_Absyn_fncall_exp(
Cyc_Toc__check_known_subscript_null_e,({struct Cyc_Absyn_Exp*_tmp562[4];_tmp562[3]=
Cyc_Absyn_uint_exp(0,0);_tmp562[2]=Cyc_Absyn_sizeoftyp_exp(_tmp54F,0);_tmp562[1]=
_tmp557;_tmp562[0]=_tmp3EF;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp562,sizeof(struct Cyc_Absyn_Exp*),4));}),0)),_tmp3F0));}goto
_LL2E8;}_LL2EB: if((int)_tmp556 != 0)goto _LL2E8;_LL2EC: {struct Cyc_Absyn_Exp*
_tmp563=Cyc_Toc__check_dyneither_subscript_e;void*ta1=Cyc_Toc_typ_to_c_array(
_tmp54F);(void*)(_tmp3EF->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,
_tmp550),Cyc_Absyn_fncall_exp(_tmp563,({struct Cyc_Absyn_Exp*_tmp564[3];_tmp564[2]=
Cyc_Absyn_uint_exp(0,0);_tmp564[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp564[0]=Cyc_Absyn_copy_exp(
_tmp3EF);((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp564,sizeof(struct Cyc_Absyn_Exp*),3));}),0)));goto _LL2E8;}
_LL2E8:;}if(is_poly)(void*)(e->r=(void*)((void*)(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v),Cyc_Absyn_new_exp((void*)e->r,
0),0))->r));goto _LL1D6;}}_LL20F: if(*((int*)_tmp3B7)!= 25)goto _LL211;_tmp3F1=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp3B7)->f1;_tmp3F2=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp3B7)->f2;_LL210: {void*_tmp565=Cyc_Tcutil_compress((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp3F1->topt))->v);{void*_tmp566=_tmp565;struct Cyc_List_List*
_tmp567;struct Cyc_Absyn_PtrInfo _tmp568;void*_tmp569;struct Cyc_Absyn_Tqual _tmp56A;
struct Cyc_Absyn_PtrAtts _tmp56B;void*_tmp56C;struct Cyc_Absyn_Conref*_tmp56D;
struct Cyc_Absyn_Conref*_tmp56E;struct Cyc_Absyn_Conref*_tmp56F;_LL2EE: if(_tmp566
<= (void*)4)goto _LL2F2;if(*((int*)_tmp566)!= 9)goto _LL2F0;_tmp567=((struct Cyc_Absyn_TupleType_struct*)
_tmp566)->f1;_LL2EF: Cyc_Toc_exp_to_c(nv,_tmp3F1);Cyc_Toc_exp_to_c(nv,_tmp3F2);{
unsigned int _tmp571;int _tmp572;struct _tuple5 _tmp570=Cyc_Evexp_eval_const_uint_exp(
_tmp3F2);_tmp571=_tmp570.f1;_tmp572=_tmp570.f2;if(!_tmp572)({void*_tmp573=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp574="unknown tuple subscript in translation to C";_tag_dyneither(
_tmp574,sizeof(char),44);}),_tag_dyneither(_tmp573,sizeof(void*),0));});(void*)(
e->r=(void*)Cyc_Toc_aggrmember_exp_r(_tmp3F1,Cyc_Absyn_fieldname((int)(_tmp571 + 
1))));goto _LL2ED;}_LL2F0: if(*((int*)_tmp566)!= 4)goto _LL2F2;_tmp568=((struct Cyc_Absyn_PointerType_struct*)
_tmp566)->f1;_tmp569=(void*)_tmp568.elt_typ;_tmp56A=_tmp568.elt_tq;_tmp56B=
_tmp568.ptr_atts;_tmp56C=(void*)_tmp56B.rgn;_tmp56D=_tmp56B.nullable;_tmp56E=
_tmp56B.bounds;_tmp56F=_tmp56B.zero_term;_LL2F1: {struct Cyc_List_List*_tmp575=
Cyc_Toc_get_relns(_tmp3F1);int in_bnds=0;{void*_tmp576=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,
_tmp56E);_LL2F5:;_LL2F6: in_bnds=Cyc_Toc_check_bounds(_tmp575,_tmp3F1,_tmp3F2);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds)({struct Cyc_String_pa_struct _tmp579;
_tmp579.tag=0;_tmp579.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
e));{void*_tmp577[1]={& _tmp579};Cyc_Tcutil_warn(e->loc,({const char*_tmp578="bounds check necessary for %s";
_tag_dyneither(_tmp578,sizeof(char),30);}),_tag_dyneither(_tmp577,sizeof(void*),
1));}});_LL2F4:;}Cyc_Toc_exp_to_c(nv,_tmp3F1);Cyc_Toc_exp_to_c(nv,_tmp3F2);++ Cyc_Toc_total_bounds_checks;{
void*_tmp57A=Cyc_Absyn_conref_def(Cyc_Absyn_bounds_one,_tmp56E);struct Cyc_Absyn_Exp*
_tmp57B;_LL2F8: if(_tmp57A <= (void*)1)goto _LL2FA;if(*((int*)_tmp57A)!= 0)goto
_LL2FA;_tmp57B=((struct Cyc_Absyn_Upper_b_struct*)_tmp57A)->f1;_LL2F9: {int
possibly_null=((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,
_tmp56D);void*ta1=Cyc_Toc_typ_to_c(_tmp569);void*ta2=Cyc_Absyn_cstar_typ(ta1,
_tmp56A);if(in_bnds)++ Cyc_Toc_bounds_checks_eliminated;else{if(((int(*)(int,
struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp56F)){if(!Cyc_Evexp_c_can_eval(
_tmp57B))({void*_tmp57C=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp57D="cannot determine subscript is in bounds";
_tag_dyneither(_tmp57D,sizeof(char),40);}),_tag_dyneither(_tmp57C,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunction(& Cyc_Toc__zero_arr_plus_functionSet,
_tmp3F1);(void*)(e->r=(void*)Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(
function_e,({struct Cyc_Absyn_Exp*_tmp57E[3];_tmp57E[2]=_tmp3F2;_tmp57E[1]=
_tmp57B;_tmp57E[0]=_tmp3F1;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp57E,sizeof(struct Cyc_Absyn_Exp*),3));}),0))));}}else{if(
possibly_null){if(!Cyc_Evexp_c_can_eval(_tmp57B))({void*_tmp57F=0;Cyc_Tcutil_terr(
e->loc,({const char*_tmp580="cannot determine subscript is in bounds";
_tag_dyneither(_tmp580,sizeof(char),40);}),_tag_dyneither(_tmp57F,sizeof(void*),
0));});if(Cyc_Toc_warn_all_null_deref)({void*_tmp581=0;Cyc_Tcutil_warn(e->loc,({
const char*_tmp582="inserted null check due to dereference";_tag_dyneither(
_tmp582,sizeof(char),39);}),_tag_dyneither(_tmp581,sizeof(void*),0));});(void*)(
e->r=(void*)Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,({
struct Cyc_Absyn_Exp*_tmp583[4];_tmp583[3]=_tmp3F2;_tmp583[2]=Cyc_Absyn_sizeoftyp_exp(
ta1,0);_tmp583[1]=_tmp57B;_tmp583[0]=_tmp3F1;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp583,sizeof(struct Cyc_Absyn_Exp*),
4));}),0))));}else{(void*)(_tmp3F2->r=(void*)Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,({
struct Cyc_Absyn_Exp*_tmp584[2];_tmp584[1]=Cyc_Absyn_copy_exp(_tmp3F2);_tmp584[0]=
_tmp57B;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(
_tag_dyneither(_tmp584,sizeof(struct Cyc_Absyn_Exp*),2));})));}}}goto _LL2F7;}
_LL2FA: if((int)_tmp57A != 0)goto _LL2F7;_LL2FB: {void*ta1=Cyc_Toc_typ_to_c_array(
_tmp569);if(in_bnds){++ Cyc_Toc_bounds_checks_eliminated;(void*)(e->r=(void*)Cyc_Toc_subscript_exp_r(
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp56A),Cyc_Absyn_aggrmember_exp(
_tmp3F1,Cyc_Toc_curr_sp,0)),_tmp3F2));}else{struct Cyc_Absyn_Exp*_tmp585=Cyc_Toc__check_dyneither_subscript_e;(
void*)(e->r=(void*)Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,
_tmp56A),Cyc_Absyn_fncall_exp(_tmp585,({struct Cyc_Absyn_Exp*_tmp586[3];_tmp586[2]=
_tmp3F2;_tmp586[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0);_tmp586[0]=_tmp3F1;((struct Cyc_List_List*(*)(
struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp586,sizeof(struct Cyc_Absyn_Exp*),
3));}),0))));}goto _LL2F7;}_LL2F7:;}goto _LL2ED;}_LL2F2:;_LL2F3:({void*_tmp587=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp588="exp_to_c: Subscript on non-tuple/array/tuple ptr";
_tag_dyneither(_tmp588,sizeof(char),49);}),_tag_dyneither(_tmp587,sizeof(void*),
0));});_LL2ED:;}goto _LL1D6;}_LL211: if(*((int*)_tmp3B7)!= 26)goto _LL213;_tmp3F3=((
struct Cyc_Absyn_Tuple_e_struct*)_tmp3B7)->f1;_LL212: if(!Cyc_Toc_is_toplevel(nv))(
void*)(e->r=(void*)((void*)(Cyc_Toc_init_tuple(nv,0,0,_tmp3F3))->r));else{struct
Cyc_List_List*_tmp589=((struct Cyc_List_List*(*)(struct _tuple4*(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp3F3);void*_tmp58A=Cyc_Toc_add_tuple_type(
_tmp589);struct Cyc_List_List*dles=0;{int i=1;for(0;_tmp3F3 != 0;(_tmp3F3=_tmp3F3->tl,
i ++)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp3F3->hd);dles=({struct Cyc_List_List*
_tmp58B=_cycalloc(sizeof(*_tmp58B));_tmp58B->hd=({struct _tuple8*_tmp58C=
_cycalloc(sizeof(*_tmp58C));_tmp58C->f1=0;_tmp58C->f2=(struct Cyc_Absyn_Exp*)
_tmp3F3->hd;_tmp58C;});_tmp58B->tl=dles;_tmp58B;});}}dles=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,dles));}goto _LL1D6;_LL213: if(*((int*)_tmp3B7)!= 28)goto _LL215;_tmp3F4=((struct
Cyc_Absyn_Array_e_struct*)_tmp3B7)->f1;_LL214:(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,_tmp3F4));{struct Cyc_List_List*_tmp58D=_tmp3F4;for(0;_tmp58D != 0;_tmp58D=
_tmp58D->tl){struct _tuple8 _tmp58F;struct Cyc_Absyn_Exp*_tmp590;struct _tuple8*
_tmp58E=(struct _tuple8*)_tmp58D->hd;_tmp58F=*_tmp58E;_tmp590=_tmp58F.f2;Cyc_Toc_exp_to_c(
nv,_tmp590);}}goto _LL1D6;_LL215: if(*((int*)_tmp3B7)!= 29)goto _LL217;_tmp3F5=((
struct Cyc_Absyn_Comprehension_e_struct*)_tmp3B7)->f1;_tmp3F6=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp3B7)->f2;_tmp3F7=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp3B7)->f3;
_tmp3F8=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp3B7)->f4;_LL216: {
unsigned int _tmp592;int _tmp593;struct _tuple5 _tmp591=Cyc_Evexp_eval_const_uint_exp(
_tmp3F6);_tmp592=_tmp591.f1;_tmp593=_tmp591.f2;{void*_tmp594=Cyc_Toc_typ_to_c((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp3F7->topt))->v);Cyc_Toc_exp_to_c(nv,
_tmp3F7);{struct Cyc_List_List*es=0;if(!Cyc_Toc_is_zero(_tmp3F7)){if(!_tmp593)({
void*_tmp595=0;Cyc_Tcutil_terr(_tmp3F6->loc,({const char*_tmp596="cannot determine value of constant";
_tag_dyneither(_tmp596,sizeof(char),35);}),_tag_dyneither(_tmp595,sizeof(void*),
0));});{unsigned int i=0;for(0;i < _tmp592;++ i){es=({struct Cyc_List_List*_tmp597=
_cycalloc(sizeof(*_tmp597));_tmp597->hd=({struct _tuple8*_tmp598=_cycalloc(
sizeof(*_tmp598));_tmp598->f1=0;_tmp598->f2=_tmp3F7;_tmp598;});_tmp597->tl=es;
_tmp597;});}}if(_tmp3F8){struct Cyc_Absyn_Exp*_tmp599=Cyc_Toc_cast_it(_tmp594,Cyc_Absyn_uint_exp(
0,0));es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))
Cyc_List_imp_append)(es,({struct Cyc_List_List*_tmp59A=_cycalloc(sizeof(*_tmp59A));
_tmp59A->hd=({struct _tuple8*_tmp59B=_cycalloc(sizeof(*_tmp59B));_tmp59B->f1=0;
_tmp59B->f2=_tmp599;_tmp59B;});_tmp59A->tl=0;_tmp59A;}));}}(void*)(e->r=(void*)
Cyc_Toc_unresolvedmem_exp_r(0,es));goto _LL1D6;}}}_LL217: if(*((int*)_tmp3B7)!= 30)
goto _LL219;_tmp3F9=((struct Cyc_Absyn_Struct_e_struct*)_tmp3B7)->f1;_tmp3FA=((
struct Cyc_Absyn_Struct_e_struct*)_tmp3B7)->f2;_tmp3FB=((struct Cyc_Absyn_Struct_e_struct*)
_tmp3B7)->f3;_tmp3FC=((struct Cyc_Absyn_Struct_e_struct*)_tmp3B7)->f4;_LL218: if(!
Cyc_Toc_is_toplevel(nv))(void*)(e->r=(void*)((void*)(Cyc_Toc_init_struct(nv,
old_typ,_tmp3FA != 0,0,0,_tmp3FB,_tmp3F9))->r));else{if(_tmp3FC == 0)({void*
_tmp59C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp59D="Struct_e: missing aggrdecl pointer";_tag_dyneither(_tmp59D,
sizeof(char),35);}),_tag_dyneither(_tmp59C,sizeof(void*),0));});{struct Cyc_Absyn_Aggrdecl*
sd2=(struct Cyc_Absyn_Aggrdecl*)_tmp3FC;struct _RegionHandle _tmp59E=_new_region("rgn");
struct _RegionHandle*rgn=& _tmp59E;_push_region(rgn);{struct Cyc_List_List*_tmp59F=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,e->loc,_tmp3FB,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
struct Cyc_List_List*_tmp5A0=0;struct Cyc_List_List*_tmp5A1=((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(sd2->impl))->fields;for(0;_tmp5A1 != 0;_tmp5A1=_tmp5A1->tl){struct Cyc_List_List*
_tmp5A2=_tmp59F;for(0;_tmp5A2 != 0;_tmp5A2=_tmp5A2->tl){if((*((struct _tuple12*)
_tmp5A2->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp5A1->hd){struct _tuple12
_tmp5A4;struct Cyc_Absyn_Aggrfield*_tmp5A5;struct Cyc_Absyn_Exp*_tmp5A6;struct
_tuple12*_tmp5A3=(struct _tuple12*)_tmp5A2->hd;_tmp5A4=*_tmp5A3;_tmp5A5=_tmp5A4.f1;
_tmp5A6=_tmp5A4.f2;{void*_tmp5A7=(void*)_tmp5A5->type;Cyc_Toc_exp_to_c(nv,
_tmp5A6);if(Cyc_Toc_is_void_star(_tmp5A7))(void*)(_tmp5A6->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp5A6->r,0)));_tmp5A0=({
struct Cyc_List_List*_tmp5A8=_cycalloc(sizeof(*_tmp5A8));_tmp5A8->hd=({struct
_tuple8*_tmp5A9=_cycalloc(sizeof(*_tmp5A9));_tmp5A9->f1=0;_tmp5A9->f2=_tmp5A6;
_tmp5A9;});_tmp5A8->tl=_tmp5A0;_tmp5A8;});break;}}}}(void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(
0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5A0)));};
_pop_region(rgn);}}goto _LL1D6;_LL219: if(*((int*)_tmp3B7)!= 31)goto _LL21B;_tmp3FD=(
void*)((struct Cyc_Absyn_AnonStruct_e_struct*)_tmp3B7)->f1;_tmp3FE=((struct Cyc_Absyn_AnonStruct_e_struct*)
_tmp3B7)->f2;_LL21A: {struct Cyc_List_List*fs;{void*_tmp5AA=Cyc_Tcutil_compress(
_tmp3FD);struct Cyc_List_List*_tmp5AB;_LL2FD: if(_tmp5AA <= (void*)4)goto _LL2FF;if(*((
int*)_tmp5AA)!= 11)goto _LL2FF;_tmp5AB=((struct Cyc_Absyn_AnonAggrType_struct*)
_tmp5AA)->f2;_LL2FE: fs=_tmp5AB;goto _LL2FC;_LL2FF:;_LL300:({struct Cyc_String_pa_struct
_tmp5AE;_tmp5AE.tag=0;_tmp5AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp3FD));{void*_tmp5AC[1]={& _tmp5AE};((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5AD="anon struct has type %s";
_tag_dyneither(_tmp5AD,sizeof(char),24);}),_tag_dyneither(_tmp5AC,sizeof(void*),
1));}});_LL2FC:;}{struct _RegionHandle _tmp5AF=_new_region("rgn");struct
_RegionHandle*rgn=& _tmp5AF;_push_region(rgn);{struct Cyc_List_List*_tmp5B0=((
struct Cyc_List_List*(*)(struct _RegionHandle*rgn,struct Cyc_Position_Segment*loc,
struct Cyc_List_List*des,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_struct_designators)(
rgn,e->loc,_tmp3FE,fs);for(0;_tmp5B0 != 0;_tmp5B0=_tmp5B0->tl){struct _tuple12
_tmp5B2;struct Cyc_Absyn_Aggrfield*_tmp5B3;struct Cyc_Absyn_Exp*_tmp5B4;struct
_tuple12*_tmp5B1=(struct _tuple12*)_tmp5B0->hd;_tmp5B2=*_tmp5B1;_tmp5B3=_tmp5B2.f1;
_tmp5B4=_tmp5B2.f2;{void*_tmp5B5=(void*)_tmp5B3->type;Cyc_Toc_exp_to_c(nv,
_tmp5B4);if(Cyc_Toc_is_void_star(_tmp5B5))(void*)(_tmp5B4->r=(void*)Cyc_Toc_cast_it_r(
Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp((void*)_tmp5B4->r,0)));}}(void*)(e->r=(
void*)Cyc_Toc_unresolvedmem_exp_r(0,_tmp3FE));};_pop_region(rgn);}goto _LL1D6;}
_LL21B: if(*((int*)_tmp3B7)!= 32)goto _LL21D;_tmp3FF=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp3B7)->f1;if(_tmp3FF != 0)goto _LL21D;_tmp400=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp3B7)->f2;_tmp401=((struct Cyc_Absyn_Tunion_e_struct*)_tmp3B7)->f3;if(!(!
_tmp400->is_flat))goto _LL21D;_LL21C: {struct _tuple1*qv=_tmp401->name;struct Cyc_Absyn_Exp*
tag_exp=_tmp400->is_xtunion?Cyc_Absyn_var_exp(qv,0): Cyc_Toc_tunion_tag(_tmp400,
qv,0);(void*)(e->r=(void*)((void*)tag_exp->r));goto _LL1D6;}_LL21D: if(*((int*)
_tmp3B7)!= 32)goto _LL21F;_tmp402=((struct Cyc_Absyn_Tunion_e_struct*)_tmp3B7)->f1;
_tmp403=((struct Cyc_Absyn_Tunion_e_struct*)_tmp3B7)->f2;_tmp404=((struct Cyc_Absyn_Tunion_e_struct*)
_tmp3B7)->f3;_LL21E: {void*tunion_ctype;struct Cyc_Absyn_Exp*tag_exp;struct
_tuple1*_tmp5B6=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp5B7=Cyc_Absyn_var_exp(
_tmp5B6,0);struct Cyc_Absyn_Exp*member_exp;if(_tmp403->is_flat){tunion_ctype=Cyc_Absyn_unionq_typ(
Cyc_Toc_collapse_qvar_tag(_tmp403->name,({const char*_tmp5B8="_union";
_tag_dyneither(_tmp5B8,sizeof(char),7);})));tag_exp=Cyc_Toc_tunion_tag(_tmp403,
_tmp404->name,1);{struct _tuple1 _tmp5BA;struct _dyneither_ptr*_tmp5BB;struct
_tuple1*_tmp5B9=_tmp404->name;_tmp5BA=*_tmp5B9;_tmp5BB=_tmp5BA.f2;member_exp=Cyc_Absyn_aggrmember_exp(
_tmp5B7,_tmp5BB,0);}}else{tunion_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp404->name,({const char*_tmp5BC="_struct";_tag_dyneither(_tmp5BC,sizeof(char),
8);})));tag_exp=_tmp403->is_xtunion?Cyc_Absyn_var_exp(_tmp404->name,0): Cyc_Toc_tunion_tag(
_tmp403,_tmp404->name,1);member_exp=_tmp5B7;}{struct Cyc_List_List*_tmp5BD=
_tmp404->typs;if(Cyc_Toc_is_toplevel(nv)){struct Cyc_List_List*dles=0;for(0;
_tmp402 != 0;(_tmp402=_tmp402->tl,_tmp5BD=_tmp5BD->tl)){struct Cyc_Absyn_Exp*cur_e=(
struct Cyc_Absyn_Exp*)_tmp402->hd;void*field_typ=Cyc_Toc_typ_to_c((*((struct
_tuple4*)((struct Cyc_List_List*)_check_null(_tmp5BD))->hd)).f2);Cyc_Toc_exp_to_c(
nv,cur_e);if(Cyc_Toc_is_void_star(field_typ))cur_e=Cyc_Toc_cast_it(field_typ,
cur_e);dles=({struct Cyc_List_List*_tmp5BE=_cycalloc(sizeof(*_tmp5BE));_tmp5BE->hd=({
struct _tuple8*_tmp5BF=_cycalloc(sizeof(*_tmp5BF));_tmp5BF->f1=0;_tmp5BF->f2=
cur_e;_tmp5BF;});_tmp5BE->tl=dles;_tmp5BE;});}dles=({struct Cyc_List_List*_tmp5C0=
_cycalloc(sizeof(*_tmp5C0));_tmp5C0->hd=({struct _tuple8*_tmp5C1=_cycalloc(
sizeof(*_tmp5C1));_tmp5C1->f1=0;_tmp5C1->f2=tag_exp;_tmp5C1;});_tmp5C0->tl=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp5C0;});(
void*)(e->r=(void*)Cyc_Toc_unresolvedmem_exp_r(0,dles));if(_tmp403->is_flat)(
void*)(e->r=(void*)Cyc_Toc_cast_it_r(Cyc_Absyn_strctq(Cyc_Toc_collapse_qvar_tag(
_tmp404->name,({const char*_tmp5C2="_struct";_tag_dyneither(_tmp5C2,sizeof(char),
8);}))),Cyc_Absyn_copy_exp(e)));}else{struct Cyc_List_List*_tmp5C3=({struct Cyc_List_List*
_tmp5C9=_cycalloc(sizeof(*_tmp5C9));_tmp5C9->hd=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(
member_exp,Cyc_Toc_tag_sp,0),tag_exp,0);_tmp5C9->tl=0;_tmp5C9;});{int i=1;for(0;
_tmp402 != 0;(((_tmp402=_tmp402->tl,i ++)),_tmp5BD=_tmp5BD->tl)){struct Cyc_Absyn_Exp*
cur_e=(struct Cyc_Absyn_Exp*)_tmp402->hd;void*field_typ=Cyc_Toc_typ_to_c((*((
struct _tuple4*)((struct Cyc_List_List*)_check_null(_tmp5BD))->hd)).f2);Cyc_Toc_exp_to_c(
nv,cur_e);if(Cyc_Toc_is_void_star(field_typ))cur_e=Cyc_Toc_cast_it(field_typ,
cur_e);{struct Cyc_Absyn_Stmt*_tmp5C4=Cyc_Absyn_assign_stmt(Cyc_Absyn_aggrmember_exp(
member_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);_tmp5C3=({struct Cyc_List_List*
_tmp5C5=_cycalloc(sizeof(*_tmp5C5));_tmp5C5->hd=_tmp5C4;_tmp5C5->tl=_tmp5C3;
_tmp5C5;});}}}{struct Cyc_Absyn_Stmt*_tmp5C6=Cyc_Absyn_exp_stmt(_tmp5B7,0);struct
Cyc_Absyn_Stmt*_tmp5C7=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(({struct Cyc_List_List*_tmp5C8=_cycalloc(sizeof(*_tmp5C8));
_tmp5C8->hd=_tmp5C6;_tmp5C8->tl=_tmp5C3;_tmp5C8;})),0);(void*)(e->r=(void*)Cyc_Toc_stmt_exp_r(
Cyc_Absyn_declare_stmt(_tmp5B6,tunion_ctype,0,_tmp5C7,0)));}}goto _LL1D6;}}_LL21F:
if(*((int*)_tmp3B7)!= 33)goto _LL221;_LL220: goto _LL222;_LL221: if(*((int*)_tmp3B7)
!= 34)goto _LL223;_LL222: goto _LL1D6;_LL223: if(*((int*)_tmp3B7)!= 35)goto _LL225;
_tmp405=((struct Cyc_Absyn_Malloc_e_struct*)_tmp3B7)->f1;_tmp406=_tmp405.is_calloc;
_tmp407=_tmp405.rgn;_tmp408=_tmp405.elt_type;_tmp409=_tmp405.num_elts;_tmp40A=
_tmp405.fat_result;_LL224: {void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(
_tmp408)));Cyc_Toc_exp_to_c(nv,_tmp409);if(_tmp40A){struct _tuple1*_tmp5CA=Cyc_Toc_temp_var();
struct _tuple1*_tmp5CB=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*
xexp;struct Cyc_Absyn_Exp*rexp;if(_tmp406){xexp=_tmp409;if(_tmp407 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp407;Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(
_tmp5CA,0));}else{pexp=Cyc_Toc_calloc_exp(*_tmp408,Cyc_Absyn_sizeoftyp_exp(t_c,0),
Cyc_Absyn_var_exp(_tmp5CA,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({
struct Cyc_Absyn_Exp*_tmp5CC[3];_tmp5CC[2]=Cyc_Absyn_var_exp(_tmp5CA,0);_tmp5CC[1]=
Cyc_Absyn_sizeoftyp_exp(t_c,0);_tmp5CC[0]=Cyc_Absyn_var_exp(_tmp5CB,0);((struct
Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5CC,
sizeof(struct Cyc_Absyn_Exp*),3));}),0);}else{xexp=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(
t_c,0),_tmp409,0);if(_tmp407 != 0  && !Cyc_Absyn_no_regions){struct Cyc_Absyn_Exp*
rgn=(struct Cyc_Absyn_Exp*)_tmp407;Cyc_Toc_exp_to_c(nv,rgn);pexp=Cyc_Toc_rmalloc_exp(
rgn,Cyc_Absyn_var_exp(_tmp5CA,0));}else{pexp=Cyc_Toc_malloc_exp(*_tmp408,Cyc_Absyn_var_exp(
_tmp5CA,0));}rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,({struct Cyc_Absyn_Exp*
_tmp5CD[3];_tmp5CD[2]=Cyc_Absyn_var_exp(_tmp5CA,0);_tmp5CD[1]=Cyc_Absyn_uint_exp(
1,0);_tmp5CD[0]=Cyc_Absyn_var_exp(_tmp5CB,0);((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp5CD,sizeof(struct Cyc_Absyn_Exp*),
3));}),0);}{struct Cyc_Absyn_Stmt*_tmp5CE=Cyc_Absyn_declare_stmt(_tmp5CA,Cyc_Absyn_uint_typ,(
struct Cyc_Absyn_Exp*)xexp,Cyc_Absyn_declare_stmt(_tmp5CB,Cyc_Absyn_cstar_typ(t_c,
Cyc_Toc_mt_tq),(struct Cyc_Absyn_Exp*)pexp,Cyc_Absyn_exp_stmt(rexp,0),0),0);(void*)(
e->r=(void*)Cyc_Toc_stmt_exp_r(_tmp5CE));}}else{if(_tmp407 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=(struct Cyc_Absyn_Exp*)_tmp407;Cyc_Toc_exp_to_c(nv,rgn);(
void*)(e->r=(void*)((void*)(Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,
0)))->r));}else{(void*)(e->r=(void*)((void*)(Cyc_Toc_malloc_exp(*_tmp408,Cyc_Absyn_sizeoftyp_exp(
t_c,0)))->r));}}goto _LL1D6;}_LL225: if(*((int*)_tmp3B7)!= 36)goto _LL227;_tmp40B=((
struct Cyc_Absyn_Swap_e_struct*)_tmp3B7)->f1;_tmp40C=((struct Cyc_Absyn_Swap_e_struct*)
_tmp3B7)->f2;_LL226: {int is_dyneither_ptr=0;void*e1_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp40B->topt))->v;void*e2_old_typ=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp40C->topt))->v;if(!Cyc_Tcutil_is_pointer_or_boxed(e1_old_typ,&
is_dyneither_ptr))({void*_tmp5CF=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5D0="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";
_tag_dyneither(_tmp5D0,sizeof(char),57);}),_tag_dyneither(_tmp5CF,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*swap_fn;if(is_dyneither_ptr)swap_fn=Cyc_Toc__swap_dyneither_e;
else{swap_fn=Cyc_Toc__swap_word_e;}if(!Cyc_Absyn_is_lvalue(_tmp40B))({struct Cyc_String_pa_struct
_tmp5D3;_tmp5D3.tag=0;_tmp5D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp40B));{void*_tmp5D1[1]={& _tmp5D3};((int(*)(struct
_dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5D2="Swap_e: %s is not an l-value\n";
_tag_dyneither(_tmp5D2,sizeof(char),30);}),_tag_dyneither(_tmp5D1,sizeof(void*),
1));}});if(!Cyc_Absyn_is_lvalue(_tmp40C))({struct Cyc_String_pa_struct _tmp5D6;
_tmp5D6.tag=0;_tmp5D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(
_tmp40C));{void*_tmp5D4[1]={& _tmp5D6};((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5D5="Swap_e: %s is not an l-value\n";
_tag_dyneither(_tmp5D5,sizeof(char),30);}),_tag_dyneither(_tmp5D4,sizeof(void*),
1));}});Cyc_Toc_exp_to_c(nv,_tmp40B);Cyc_Toc_exp_to_c(nv,_tmp40C);(void*)(e->r=(
void*)Cyc_Toc_fncall_exp_r(swap_fn,({struct Cyc_Absyn_Exp*_tmp5D7[2];_tmp5D7[1]=
Cyc_Absyn_address_exp(_tmp40C,0);_tmp5D7[0]=Cyc_Absyn_address_exp(_tmp40B,0);((
struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(
_tmp5D7,sizeof(struct Cyc_Absyn_Exp*),2));})));goto _LL1D6;}}_LL227: if(*((int*)
_tmp3B7)!= 38)goto _LL229;_tmp40D=((struct Cyc_Absyn_StmtExp_e_struct*)_tmp3B7)->f1;
_LL228: Cyc_Toc_stmt_to_c(nv,_tmp40D);goto _LL1D6;_LL229: if(*((int*)_tmp3B7)!= 37)
goto _LL22B;_LL22A:({void*_tmp5D8=0;((int(*)(struct _dyneither_ptr fmt,struct
_dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp5D9="UnresolvedMem";
_tag_dyneither(_tmp5D9,sizeof(char),14);}),_tag_dyneither(_tmp5D8,sizeof(void*),
0));});_LL22B: if(*((int*)_tmp3B7)!= 27)goto _LL22D;_LL22C:({void*_tmp5DA=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(({const char*
_tmp5DB="compoundlit";_tag_dyneither(_tmp5DB,sizeof(char),12);}),_tag_dyneither(
_tmp5DA,sizeof(void*),0));});_LL22D: if(*((int*)_tmp3B7)!= 39)goto _LL1D6;_LL22E:({
void*_tmp5DC=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp5DD="valueof(-)";_tag_dyneither(_tmp5DD,sizeof(char),11);}),
_tag_dyneither(_tmp5DC,sizeof(void*),0));});_LL1D6:;}}static struct Cyc_Absyn_Stmt*
Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*
fail_stmt){return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
Cyc_Toc_skip_stmt_dl(),0);}struct _tuple13{struct Cyc_Toc_Env*f1;struct Cyc_List_List*
f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple14{struct _tuple1*f1;void*f2;};struct
_tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};static struct _tuple13 Cyc_Toc_xlate_pat(
struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct
Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt**tag_fail_stmt,
struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){struct Cyc_Absyn_Stmt*s;{
void*_tmp5DE=(void*)p->r;struct Cyc_Absyn_Vardecl*_tmp5DF;struct Cyc_Absyn_Vardecl
_tmp5E0;struct _tuple1*_tmp5E1;struct Cyc_Absyn_Pat*_tmp5E2;struct Cyc_Absyn_Vardecl*
_tmp5E3;struct Cyc_Absyn_Vardecl _tmp5E4;struct _tuple1*_tmp5E5;struct Cyc_Absyn_Vardecl*
_tmp5E6;struct Cyc_Absyn_Pat*_tmp5E7;void*_tmp5E8;int _tmp5E9;char _tmp5EA;struct
_dyneither_ptr _tmp5EB;struct Cyc_Absyn_Enumdecl*_tmp5EC;struct Cyc_Absyn_Enumfield*
_tmp5ED;void*_tmp5EE;struct Cyc_Absyn_Enumfield*_tmp5EF;struct Cyc_Absyn_Tuniondecl*
_tmp5F0;struct Cyc_Absyn_Tunionfield*_tmp5F1;struct Cyc_List_List*_tmp5F2;struct
Cyc_Absyn_Pat*_tmp5F3;struct Cyc_Absyn_Pat _tmp5F4;void*_tmp5F5;struct Cyc_Absyn_Tuniondecl*
_tmp5F6;struct Cyc_Absyn_Tunionfield*_tmp5F7;struct Cyc_List_List*_tmp5F8;struct
Cyc_Absyn_Tuniondecl*_tmp5F9;struct Cyc_Absyn_Tunionfield*_tmp5FA;struct Cyc_List_List*
_tmp5FB;struct Cyc_List_List*_tmp5FC;struct Cyc_List_List*_tmp5FD;struct Cyc_Absyn_AggrInfo
_tmp5FE;union Cyc_Absyn_AggrInfoU_union _tmp5FF;struct Cyc_List_List*_tmp600;struct
Cyc_Absyn_Pat*_tmp601;_LL302: if(_tmp5DE <= (void*)2)goto _LL306;if(*((int*)_tmp5DE)
!= 0)goto _LL304;_tmp5DF=((struct Cyc_Absyn_Var_p_struct*)_tmp5DE)->f1;_tmp5E0=*
_tmp5DF;_tmp5E1=_tmp5E0.name;_tmp5E2=((struct Cyc_Absyn_Var_p_struct*)_tmp5DE)->f2;
_LL303: return Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp5E1,r),rgn,t,r,path,
_tmp5E2,tag_fail_stmt,fail_stmt,decls);_LL304: if(*((int*)_tmp5DE)!= 2)goto _LL306;
_tmp5E3=((struct Cyc_Absyn_TagInt_p_struct*)_tmp5DE)->f2;_tmp5E4=*_tmp5E3;_tmp5E5=
_tmp5E4.name;_LL305: nv=Cyc_Toc_add_varmap(rgn,nv,_tmp5E5,r);goto _LL307;_LL306:
if((int)_tmp5DE != 0)goto _LL308;_LL307: s=Cyc_Toc_skip_stmt_dl();goto _LL301;_LL308:
if(_tmp5DE <= (void*)2)goto _LL30A;if(*((int*)_tmp5DE)!= 1)goto _LL30A;_tmp5E6=((
struct Cyc_Absyn_Reference_p_struct*)_tmp5DE)->f1;_tmp5E7=((struct Cyc_Absyn_Reference_p_struct*)
_tmp5DE)->f2;_LL309: {struct _tuple1*_tmp602=Cyc_Toc_temp_var();decls=({struct Cyc_List_List*
_tmp603=_region_malloc(rgn,sizeof(*_tmp603));_tmp603->hd=({struct _tuple14*
_tmp604=_region_malloc(rgn,sizeof(*_tmp604));_tmp604->f1=_tmp602;_tmp604->f2=Cyc_Absyn_cstar_typ(
Cyc_Toc_typ_to_c(t),Cyc_Toc_mt_tq);_tmp604;});_tmp603->tl=decls;_tmp603;});nv=
Cyc_Toc_add_varmap(rgn,nv,_tmp5E6->name,Cyc_Absyn_var_exp(_tmp602,0));s=Cyc_Absyn_assign_stmt(
Cyc_Absyn_var_exp(_tmp602,0),Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(
t),Cyc_Toc_mt_tq),Cyc_Absyn_address_exp(path,0)),0);{struct _tuple13 _tmp605=Cyc_Toc_xlate_pat(
nv,rgn,t,r,path,_tmp5E7,tag_fail_stmt,fail_stmt,decls);_tmp605.f3=Cyc_Absyn_seq_stmt(
s,_tmp605.f3,0);return _tmp605;}}_LL30A: if((int)_tmp5DE != 1)goto _LL30C;_LL30B: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL301;_LL30C:
if(_tmp5DE <= (void*)2)goto _LL30E;if(*((int*)_tmp5DE)!= 7)goto _LL30E;_tmp5E8=(
void*)((struct Cyc_Absyn_Int_p_struct*)_tmp5DE)->f1;_tmp5E9=((struct Cyc_Absyn_Int_p_struct*)
_tmp5DE)->f2;_LL30D: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp5E8,_tmp5E9,0),
fail_stmt);goto _LL301;_LL30E: if(_tmp5DE <= (void*)2)goto _LL310;if(*((int*)_tmp5DE)
!= 8)goto _LL310;_tmp5EA=((struct Cyc_Absyn_Char_p_struct*)_tmp5DE)->f1;_LL30F: s=
Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp5EA,0),fail_stmt);goto _LL301;_LL310:
if(_tmp5DE <= (void*)2)goto _LL312;if(*((int*)_tmp5DE)!= 9)goto _LL312;_tmp5EB=((
struct Cyc_Absyn_Float_p_struct*)_tmp5DE)->f1;_LL311: s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(
_tmp5EB,0),fail_stmt);goto _LL301;_LL312: if(_tmp5DE <= (void*)2)goto _LL314;if(*((
int*)_tmp5DE)!= 10)goto _LL314;_tmp5EC=((struct Cyc_Absyn_Enum_p_struct*)_tmp5DE)->f1;
_tmp5ED=((struct Cyc_Absyn_Enum_p_struct*)_tmp5DE)->f2;_LL313: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_struct*_tmp606=_cycalloc(
sizeof(*_tmp606));_tmp606[0]=({struct Cyc_Absyn_Enum_e_struct _tmp607;_tmp607.tag=
33;_tmp607.f1=_tmp5ED->name;_tmp607.f2=(struct Cyc_Absyn_Enumdecl*)_tmp5EC;
_tmp607.f3=(struct Cyc_Absyn_Enumfield*)_tmp5ED;_tmp607;});_tmp606;}),0),
fail_stmt);goto _LL301;_LL314: if(_tmp5DE <= (void*)2)goto _LL316;if(*((int*)_tmp5DE)
!= 11)goto _LL316;_tmp5EE=(void*)((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp5DE)->f1;
_tmp5EF=((struct Cyc_Absyn_AnonEnum_p_struct*)_tmp5DE)->f2;_LL315: s=Cyc_Toc_if_neq_stmt(
r,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_struct*_tmp608=_cycalloc(
sizeof(*_tmp608));_tmp608[0]=({struct Cyc_Absyn_AnonEnum_e_struct _tmp609;_tmp609.tag=
34;_tmp609.f1=_tmp5EF->name;_tmp609.f2=(void*)_tmp5EE;_tmp609.f3=(struct Cyc_Absyn_Enumfield*)
_tmp5EF;_tmp609;});_tmp608;}),0),fail_stmt);goto _LL301;_LL316: if(_tmp5DE <= (void*)
2)goto _LL318;if(*((int*)_tmp5DE)!= 6)goto _LL318;_tmp5F0=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp5DE)->f1;_tmp5F1=((struct Cyc_Absyn_Tunion_p_struct*)_tmp5DE)->f2;_tmp5F2=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp5DE)->f3;if(_tmp5F2 != 0)goto _LL318;if(!(!
_tmp5F0->is_flat))goto _LL318;_LL317: {struct Cyc_Absyn_Exp*cmp_exp;if(_tmp5F0->is_xtunion)
cmp_exp=Cyc_Absyn_var_exp(_tmp5F1->name,0);else{cmp_exp=Cyc_Toc_tunion_tag(
_tmp5F0,_tmp5F1->name,0);r=Cyc_Toc_cast_it(Cyc_Absyn_sint_typ,r);}s=Cyc_Toc_if_neq_stmt(
r,cmp_exp,fail_stmt);goto _LL301;}_LL318: if(_tmp5DE <= (void*)2)goto _LL31A;if(*((
int*)_tmp5DE)!= 4)goto _LL31A;_tmp5F3=((struct Cyc_Absyn_Pointer_p_struct*)_tmp5DE)->f1;
_tmp5F4=*_tmp5F3;_tmp5F5=(void*)_tmp5F4.r;if(_tmp5F5 <= (void*)2)goto _LL31A;if(*((
int*)_tmp5F5)!= 6)goto _LL31A;_tmp5F6=((struct Cyc_Absyn_Tunion_p_struct*)_tmp5F5)->f1;
_tmp5F7=((struct Cyc_Absyn_Tunion_p_struct*)_tmp5F5)->f2;_tmp5F8=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp5F5)->f3;if(!(_tmp5F8 != 0  && !_tmp5F6->is_flat))goto _LL31A;_LL319: s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;struct _tuple1*tufstrct=Cyc_Toc_collapse_qvar_tag(_tmp5F7->name,({const
char*_tmp616="_struct";_tag_dyneither(_tmp616,sizeof(char),8);}));struct Cyc_Absyn_Exp*
rcast=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq),
r);struct Cyc_List_List*_tmp60A=_tmp5F7->typs;for(0;_tmp5F8 != 0;(((_tmp5F8=
_tmp5F8->tl,_tmp60A=((struct Cyc_List_List*)_check_null(_tmp60A))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp60B=(struct Cyc_Absyn_Pat*)_tmp5F8->hd;if((void*)_tmp60B->r
== (void*)0)continue;{void*_tmp60C=(*((struct _tuple4*)((struct Cyc_List_List*)
_check_null(_tmp60A))->hd)).f2;struct _tuple1*_tmp60D=Cyc_Toc_temp_var();void*
_tmp60E=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp60B->topt))->v;void*_tmp60F=
Cyc_Toc_typ_to_c(_tmp60E);struct Cyc_Absyn_Exp*_tmp610=Cyc_Absyn_aggrarrow_exp(
rcast,Cyc_Absyn_fieldname(cnt),0);if(Cyc_Toc_is_void_star(Cyc_Toc_typ_to_c(
_tmp60C)))_tmp610=Cyc_Toc_cast_it(_tmp60F,_tmp610);decls=({struct Cyc_List_List*
_tmp611=_region_malloc(rgn,sizeof(*_tmp611));_tmp611->hd=({struct _tuple14*
_tmp612=_region_malloc(rgn,sizeof(*_tmp612));_tmp612->f1=_tmp60D;_tmp612->f2=
_tmp60F;_tmp612;});_tmp611->tl=decls;_tmp611;});{struct _tuple13 _tmp613=Cyc_Toc_xlate_pat(
nv,rgn,_tmp60E,Cyc_Absyn_var_exp(_tmp60D,0),_tmp610,_tmp60B,(struct Cyc_Absyn_Stmt**)&
fail_stmt,fail_stmt,decls);nv=_tmp613.f1;decls=_tmp613.f2;{struct Cyc_Absyn_Stmt*
_tmp614=_tmp613.f3;struct Cyc_Absyn_Stmt*_tmp615=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp60D,0),_tmp610,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp615,_tmp614,
0),0);}}}}{struct Cyc_Absyn_Exp*test_exp;if(_tmp5F6->is_xtunion){struct Cyc_Absyn_Exp*
e2=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_void_star_typ(),Cyc_Toc_mt_tq),
r);struct Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e2,0);struct Cyc_Absyn_Exp*e=Cyc_Absyn_var_exp(
_tmp5F7->name,0);test_exp=Cyc_Absyn_neq_exp(e1,e,0);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
test_exp,fail_stmt,Cyc_Toc_skip_stmt_dl(),0),s,0);}else{struct Cyc_Absyn_Exp*e3=
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,Cyc_Toc_mt_tq),r);struct
Cyc_Absyn_Exp*e1=Cyc_Absyn_deref_exp(e3,0);struct Cyc_Absyn_Exp*e=Cyc_Toc_tunion_tag(
_tmp5F6,_tmp5F7->name,1);s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(
e1,e,0),fail_stmt,Cyc_Toc_skip_stmt_dl(),0),s,0);if(tag_fail_stmt != 0){int
max_tag=Cyc_Toc_num_void_tags(_tmp5F6);if(max_tag != 0){struct Cyc_Absyn_Exp*
max_tag_exp=Cyc_Absyn_uint_exp((unsigned int)max_tag,0);struct Cyc_Absyn_Exp*e5=
Cyc_Absyn_lte_exp(r,Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),max_tag_exp),0);s=
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(e5,*tag_fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),s,0);}}}goto _LL301;}}_LL31A: if(_tmp5DE <= (void*)2)goto _LL31C;if(*((int*)
_tmp5DE)!= 6)goto _LL31C;_tmp5F9=((struct Cyc_Absyn_Tunion_p_struct*)_tmp5DE)->f1;
_tmp5FA=((struct Cyc_Absyn_Tunion_p_struct*)_tmp5DE)->f2;_tmp5FB=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp5DE)->f3;if(!_tmp5F9->is_flat)goto _LL31C;_LL31B: {struct _tuple1 _tmp618;
struct _dyneither_ptr*_tmp619;struct _tuple1*_tmp617=_tmp5FA->name;_tmp618=*
_tmp617;_tmp619=_tmp618.f2;r=Cyc_Absyn_aggrmember_exp(r,_tmp619,0);path=Cyc_Absyn_aggrmember_exp(
path,_tmp619,0);s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(Cyc_Absyn_aggrmember_exp(
path,Cyc_Toc_tag_sp,0),Cyc_Toc_tunion_tag(_tmp5F9,_tmp5FA->name,1),0),fail_stmt,
Cyc_Toc_skip_stmt_dl(),0);{int cnt=1;for(0;_tmp5FB != 0;(_tmp5FB=_tmp5FB->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp61A=(struct Cyc_Absyn_Pat*)_tmp5FB->hd;if((void*)_tmp61A->r
== (void*)0)continue;{struct _tuple1*_tmp61B=Cyc_Toc_temp_var();void*_tmp61C=(
void*)((struct Cyc_Core_Opt*)_check_null(_tmp61A->topt))->v;decls=({struct Cyc_List_List*
_tmp61D=_region_malloc(rgn,sizeof(*_tmp61D));_tmp61D->hd=({struct _tuple14*
_tmp61E=_region_malloc(rgn,sizeof(*_tmp61E));_tmp61E->f1=_tmp61B;_tmp61E->f2=Cyc_Toc_typ_to_c(
_tmp61C);_tmp61E;});_tmp61D->tl=decls;_tmp61D;});{struct _tuple13 _tmp61F=Cyc_Toc_xlate_pat(
nv,rgn,_tmp61C,Cyc_Absyn_var_exp(_tmp61B,0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(
cnt),0),_tmp61A,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp61F.f1;
decls=_tmp61F.f2;{struct Cyc_Absyn_Stmt*_tmp620=_tmp61F.f3;struct Cyc_Absyn_Stmt*
_tmp621=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp61B,0),Cyc_Absyn_aggrmember_exp(
r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(
_tmp621,_tmp620,0),0);}}}}goto _LL301;}}_LL31C: if(_tmp5DE <= (void*)2)goto _LL31E;
if(*((int*)_tmp5DE)!= 6)goto _LL31E;_tmp5FC=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp5DE)->f3;_LL31D: _tmp5FD=_tmp5FC;goto _LL31F;_LL31E: if(_tmp5DE <= (void*)2)goto
_LL320;if(*((int*)_tmp5DE)!= 3)goto _LL320;_tmp5FD=((struct Cyc_Absyn_Tuple_p_struct*)
_tmp5DE)->f1;_LL31F: s=Cyc_Toc_skip_stmt_dl();{int cnt=1;for(0;_tmp5FD != 0;(
_tmp5FD=_tmp5FD->tl,++ cnt)){struct Cyc_Absyn_Pat*_tmp622=(struct Cyc_Absyn_Pat*)
_tmp5FD->hd;if((void*)_tmp622->r == (void*)0)continue;{struct _tuple1*_tmp623=Cyc_Toc_temp_var();
void*_tmp624=(void*)((struct Cyc_Core_Opt*)_check_null(_tmp622->topt))->v;decls=({
struct Cyc_List_List*_tmp625=_region_malloc(rgn,sizeof(*_tmp625));_tmp625->hd=({
struct _tuple14*_tmp626=_region_malloc(rgn,sizeof(*_tmp626));_tmp626->f1=_tmp623;
_tmp626->f2=Cyc_Toc_typ_to_c(_tmp624);_tmp626;});_tmp625->tl=decls;_tmp625;});{
struct _tuple13 _tmp627=Cyc_Toc_xlate_pat(nv,rgn,_tmp624,Cyc_Absyn_var_exp(_tmp623,
0),Cyc_Absyn_aggrmember_exp(path,Cyc_Absyn_fieldname(cnt),0),_tmp622,(struct Cyc_Absyn_Stmt**)&
fail_stmt,fail_stmt,decls);nv=_tmp627.f1;decls=_tmp627.f2;{struct Cyc_Absyn_Stmt*
_tmp628=_tmp627.f3;struct Cyc_Absyn_Stmt*_tmp629=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(
_tmp623,0),Cyc_Absyn_aggrmember_exp(r,Cyc_Absyn_fieldname(cnt),0),0);s=Cyc_Absyn_seq_stmt(
s,Cyc_Absyn_seq_stmt(_tmp629,_tmp628,0),0);}}}}goto _LL301;}_LL320: if(_tmp5DE <= (
void*)2)goto _LL322;if(*((int*)_tmp5DE)!= 5)goto _LL322;_tmp5FE=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp5DE)->f1;_tmp5FF=_tmp5FE.aggr_info;_tmp600=((struct Cyc_Absyn_Aggr_p_struct*)
_tmp5DE)->f3;_LL321: {struct Cyc_Absyn_Aggrdecl*_tmp62A=Cyc_Absyn_get_known_aggrdecl(
_tmp5FF);s=Cyc_Toc_skip_stmt_dl();for(0;_tmp600 != 0;_tmp600=_tmp600->tl){struct
_tuple15*_tmp62B=(struct _tuple15*)_tmp600->hd;struct Cyc_Absyn_Pat*_tmp62C=(*
_tmp62B).f2;if((void*)_tmp62C->r == (void*)0)continue;{struct _dyneither_ptr*f;{
void*_tmp62D=(void*)((struct Cyc_List_List*)_check_null((*_tmp62B).f1))->hd;
struct _dyneither_ptr*_tmp62E;_LL32B: if(*((int*)_tmp62D)!= 1)goto _LL32D;_tmp62E=((
struct Cyc_Absyn_FieldName_struct*)_tmp62D)->f1;_LL32C: f=_tmp62E;goto _LL32A;
_LL32D:;_LL32E:(int)_throw((void*)Cyc_Toc_Match_error);_LL32A:;}{struct _tuple1*
_tmp62F=Cyc_Toc_temp_var();void*_tmp630=(void*)((struct Cyc_Core_Opt*)_check_null(
_tmp62C->topt))->v;void*_tmp631=Cyc_Toc_typ_to_c(_tmp630);decls=({struct Cyc_List_List*
_tmp632=_region_malloc(rgn,sizeof(*_tmp632));_tmp632->hd=({struct _tuple14*
_tmp633=_region_malloc(rgn,sizeof(*_tmp633));_tmp633->f1=_tmp62F;_tmp633->f2=
_tmp631;_tmp633;});_tmp632->tl=decls;_tmp632;});{struct _tuple13 _tmp634=Cyc_Toc_xlate_pat(
nv,rgn,_tmp630,Cyc_Absyn_var_exp(_tmp62F,0),Cyc_Absyn_aggrmember_exp(path,f,0),
_tmp62C,(struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp634.f1;decls=
_tmp634.f2;{struct Cyc_Absyn_Exp*_tmp635=Cyc_Absyn_aggrmember_exp(r,f,0);if(Cyc_Toc_is_void_star((
void*)((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct
Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp62A->impl))->fields,f)))->type))_tmp635=
Cyc_Toc_cast_it(_tmp631,_tmp635);{struct Cyc_Absyn_Stmt*_tmp636=_tmp634.f3;struct
Cyc_Absyn_Stmt*_tmp637=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp62F,0),
_tmp635,0);s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_seq_stmt(_tmp637,_tmp636,0),0);}}}}}}
goto _LL301;}_LL322: if(_tmp5DE <= (void*)2)goto _LL324;if(*((int*)_tmp5DE)!= 4)goto
_LL324;_tmp601=((struct Cyc_Absyn_Pointer_p_struct*)_tmp5DE)->f1;_LL323: {struct
_tuple1*_tmp638=Cyc_Toc_temp_var();void*_tmp639=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp601->topt))->v;decls=({struct Cyc_List_List*_tmp63A=
_region_malloc(rgn,sizeof(*_tmp63A));_tmp63A->hd=({struct _tuple14*_tmp63B=
_region_malloc(rgn,sizeof(*_tmp63B));_tmp63B->f1=_tmp638;_tmp63B->f2=Cyc_Toc_typ_to_c(
_tmp639);_tmp63B;});_tmp63A->tl=decls;_tmp63A;});{struct _tuple13 _tmp63C=Cyc_Toc_xlate_pat(
nv,rgn,_tmp639,Cyc_Absyn_var_exp(_tmp638,0),Cyc_Absyn_deref_exp(path,0),_tmp601,(
struct Cyc_Absyn_Stmt**)& fail_stmt,fail_stmt,decls);nv=_tmp63C.f1;decls=_tmp63C.f2;{
struct Cyc_Absyn_Stmt*_tmp63D=_tmp63C.f3;struct Cyc_Absyn_Stmt*_tmp63E=Cyc_Absyn_seq_stmt(
Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp638,0),Cyc_Absyn_deref_exp(r,0),0),
_tmp63D,0);if(Cyc_Toc_is_nullable(t))s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(
Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,Cyc_Toc_skip_stmt_dl(),
0),_tmp63E,0);else{s=_tmp63E;}goto _LL301;}}}_LL324: if(_tmp5DE <= (void*)2)goto
_LL326;if(*((int*)_tmp5DE)!= 12)goto _LL326;_LL325:({void*_tmp63F=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp640="unknownid pat";_tag_dyneither(_tmp640,sizeof(char),14);}),
_tag_dyneither(_tmp63F,sizeof(void*),0));});_LL326: if(_tmp5DE <= (void*)2)goto
_LL328;if(*((int*)_tmp5DE)!= 13)goto _LL328;_LL327:({void*_tmp641=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp642="unknowncall pat";_tag_dyneither(_tmp642,sizeof(char),16);}),
_tag_dyneither(_tmp641,sizeof(void*),0));});_LL328: if(_tmp5DE <= (void*)2)goto
_LL301;if(*((int*)_tmp5DE)!= 14)goto _LL301;_LL329:({void*_tmp643=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp644="exp pat";_tag_dyneither(_tmp644,sizeof(char),8);}),_tag_dyneither(
_tmp643,sizeof(void*),0));});_LL301:;}return({struct _tuple13 _tmp645;_tmp645.f1=
nv;_tmp645.f2=decls;_tmp645.f3=s;_tmp645;});}struct _tuple16{struct _dyneither_ptr*
f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};static struct
_tuple16*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*
sc){return({struct _tuple16*_tmp646=_region_malloc(r,sizeof(*_tmp646));_tmp646->f1=
Cyc_Toc_fresh_label();_tmp646->f2=Cyc_Toc_fresh_label();_tmp646->f3=sc;_tmp646;});}
static int Cyc_Toc_is_mixed_tunion(void*t){{void*_tmp647=Cyc_Tcutil_compress(t);
struct Cyc_Absyn_TunionInfo _tmp648;union Cyc_Absyn_TunionInfoU_union _tmp649;struct
Cyc_Absyn_Tuniondecl**_tmp64A;struct Cyc_Absyn_Tuniondecl*_tmp64B;_LL330: if(
_tmp647 <= (void*)4)goto _LL332;if(*((int*)_tmp647)!= 2)goto _LL332;_tmp648=((
struct Cyc_Absyn_TunionType_struct*)_tmp647)->f1;_tmp649=_tmp648.tunion_info;if((((((
struct Cyc_Absyn_TunionType_struct*)_tmp647)->f1).tunion_info).KnownTunion).tag != 
1)goto _LL332;_tmp64A=(_tmp649.KnownTunion).f1;_tmp64B=*_tmp64A;if(!(!_tmp64B->is_flat))
goto _LL332;_LL331: {int seen_novalue=0;int seen_value=0;{struct Cyc_List_List*
_tmp64C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp64B->fields))->v;
for(0;(unsigned int)_tmp64C;_tmp64C=_tmp64C->tl){if(((struct Cyc_Absyn_Tunionfield*)
_tmp64C->hd)->typs == 0)seen_value=1;else{seen_novalue=1;}if(seen_value  && 
seen_novalue)return 1;}}goto _LL32F;}_LL332:;_LL333: goto _LL32F;_LL32F:;}return 0;}
static int Cyc_Toc_no_tag_test(struct Cyc_Absyn_Pat*p){void*_tmp64D=(void*)p->r;
struct Cyc_Absyn_Pat*_tmp64E;struct Cyc_Absyn_Pat*_tmp64F;struct Cyc_Absyn_Pat
_tmp650;void*_tmp651;struct Cyc_Absyn_Tuniondecl*_tmp652;struct Cyc_Absyn_Tunionfield*
_tmp653;struct Cyc_List_List*_tmp654;_LL335: if(_tmp64D <= (void*)2)goto _LL339;if(*((
int*)_tmp64D)!= 0)goto _LL337;_tmp64E=((struct Cyc_Absyn_Var_p_struct*)_tmp64D)->f2;
_LL336: return Cyc_Toc_no_tag_test(_tmp64E);_LL337: if(*((int*)_tmp64D)!= 4)goto
_LL339;_tmp64F=((struct Cyc_Absyn_Pointer_p_struct*)_tmp64D)->f1;_tmp650=*_tmp64F;
_tmp651=(void*)_tmp650.r;if(_tmp651 <= (void*)2)goto _LL339;if(*((int*)_tmp651)!= 
6)goto _LL339;_tmp652=((struct Cyc_Absyn_Tunion_p_struct*)_tmp651)->f1;_tmp653=((
struct Cyc_Absyn_Tunion_p_struct*)_tmp651)->f2;_tmp654=((struct Cyc_Absyn_Tunion_p_struct*)
_tmp651)->f3;if(!(_tmp654 != 0  && !_tmp652->is_flat))goto _LL339;_LL338: return 0;
_LL339:;_LL33A: return 1;_LL334:;}static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){
Cyc_Toc_exp_to_c(nv,e);{void*_tmp655=(void*)((struct Cyc_Core_Opt*)_check_null(e->topt))->v;
int leave_as_switch;{void*_tmp656=Cyc_Tcutil_compress(_tmp655);_LL33C: if(_tmp656
<= (void*)4)goto _LL340;if(*((int*)_tmp656)!= 5)goto _LL33E;_LL33D: goto _LL33F;
_LL33E: if(*((int*)_tmp656)!= 12)goto _LL340;_LL33F: leave_as_switch=1;goto _LL33B;
_LL340:;_LL341: leave_as_switch=0;goto _LL33B;_LL33B:;}{struct Cyc_List_List*
_tmp657=scs;for(0;_tmp657 != 0;_tmp657=_tmp657->tl){if((struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp657->hd)->pat_vars))->v
!= 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp657->hd)->where_clause != 0){
leave_as_switch=0;break;}}}if(leave_as_switch){struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();{
struct Cyc_List_List*_tmp658=scs;for(0;_tmp658 != 0;_tmp658=_tmp658->tl){struct Cyc_Absyn_Stmt*
_tmp659=((struct Cyc_Absyn_Switch_clause*)_tmp658->hd)->body;((struct Cyc_Absyn_Switch_clause*)
_tmp658->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp659,0);next_l=Cyc_Toc_fresh_label();{
struct _RegionHandle _tmp65A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp65A;
_push_region(rgn);Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),
_tmp659);;_pop_region(rgn);}}}return;}{struct _tuple1*v=Cyc_Toc_temp_var();struct
Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(
v,0);struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();struct _RegionHandle _tmp65B=
_new_region("rgn");struct _RegionHandle*rgn=& _tmp65B;_push_region(rgn);{struct Cyc_Toc_Env*
_tmp65C=Cyc_Toc_share_env(rgn,nv);struct Cyc_List_List*lscs=((struct Cyc_List_List*(*)(
struct _RegionHandle*,struct _tuple16*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),
struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,
rgn,scs);struct Cyc_List_List*test_stmts=0;struct Cyc_List_List*nvs=0;struct Cyc_List_List*
decls=0;int is_tunion=Cyc_Toc_is_mixed_tunion(_tmp655);int needs_tag_test=
is_tunion;{struct Cyc_List_List*_tmp65D=lscs;for(0;_tmp65D != 0;_tmp65D=_tmp65D->tl){
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple16*)_tmp65D->hd)).f3;struct
_dyneither_ptr*fail_lab=_tmp65D->tl == 0?end_l:(*((struct _tuple16*)((struct Cyc_List_List*)
_check_null(_tmp65D->tl))->hd)).f1;struct Cyc_Absyn_Stmt**tag_fail_stmt=0;if(
needs_tag_test  && !Cyc_Toc_no_tag_test(sc->pattern)){{struct Cyc_List_List*
_tmp65E=_tmp65D->tl;for(0;(unsigned int)_tmp65E;_tmp65E=_tmp65E->tl){if(Cyc_Toc_no_tag_test(((*((
struct _tuple16*)_tmp65E->hd)).f3)->pattern)){tag_fail_stmt=({struct Cyc_Absyn_Stmt**
_tmp65F=_region_malloc(rgn,sizeof(*_tmp65F));_tmp65F[0]=Cyc_Absyn_goto_stmt((*((
struct _tuple16*)_tmp65E->hd)).f1,0);_tmp65F;});needs_tag_test=0;break;}}}if(
tag_fail_stmt == 0)tag_fail_stmt=({struct Cyc_Absyn_Stmt**_tmp660=_region_malloc(
rgn,sizeof(*_tmp660));_tmp660[0]=Cyc_Absyn_goto_stmt(fail_lab,0);_tmp660;});}{
struct Cyc_Toc_Env*_tmp662;struct Cyc_List_List*_tmp663;struct Cyc_Absyn_Stmt*
_tmp664;struct _tuple13 _tmp661=Cyc_Toc_xlate_pat(_tmp65C,rgn,_tmp655,r,path,sc->pattern,
tag_fail_stmt,Cyc_Absyn_goto_stmt(fail_lab,0),decls);_tmp662=_tmp661.f1;_tmp663=
_tmp661.f2;_tmp664=_tmp661.f3;if(is_tunion  && Cyc_Toc_no_tag_test(sc->pattern))
needs_tag_test=1;if(sc->where_clause != 0){struct Cyc_Absyn_Exp*_tmp665=(struct Cyc_Absyn_Exp*)
_check_null(sc->where_clause);Cyc_Toc_exp_to_c(_tmp662,_tmp665);_tmp664=Cyc_Absyn_seq_stmt(
_tmp664,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)11,_tmp665,0),Cyc_Absyn_goto_stmt(
fail_lab,0),Cyc_Toc_skip_stmt_dl(),0),0);}decls=_tmp663;nvs=({struct Cyc_List_List*
_tmp666=_region_malloc(rgn,sizeof(*_tmp666));_tmp666->hd=_tmp662;_tmp666->tl=nvs;
_tmp666;});test_stmts=({struct Cyc_List_List*_tmp667=_region_malloc(rgn,sizeof(*
_tmp667));_tmp667->hd=_tmp664;_tmp667->tl=test_stmts;_tmp667;});}}}nvs=((struct
Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);test_stmts=((
struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{
struct Cyc_List_List*stmts=0;for(0;lscs != 0;(((lscs=lscs->tl,nvs=nvs->tl)),
test_stmts=test_stmts->tl)){struct _tuple16 _tmp669;struct _dyneither_ptr*_tmp66A;
struct _dyneither_ptr*_tmp66B;struct Cyc_Absyn_Switch_clause*_tmp66C;struct
_tuple16*_tmp668=(struct _tuple16*)lscs->hd;_tmp669=*_tmp668;_tmp66A=_tmp669.f1;
_tmp66B=_tmp669.f2;_tmp66C=_tmp669.f3;{struct Cyc_Toc_Env*_tmp66D=(struct Cyc_Toc_Env*)((
struct Cyc_List_List*)_check_null(nvs))->hd;struct Cyc_Absyn_Stmt*s=_tmp66C->body;{
struct _RegionHandle _tmp66E=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp66E;
_push_region(rgn2);if(lscs->tl != 0){struct _tuple16 _tmp670;struct _dyneither_ptr*
_tmp671;struct Cyc_Absyn_Switch_clause*_tmp672;struct _tuple16*_tmp66F=(struct
_tuple16*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;_tmp670=*_tmp66F;
_tmp671=_tmp670.f2;_tmp672=_tmp670.f3;Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(
rgn2,_tmp66D,end_l,_tmp671,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(_tmp672->pat_vars))->v,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)
_check_null(nvs->tl))->hd),s);}else{Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(
rgn2,_tmp66D,end_l),s);};_pop_region(rgn2);}s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(
_tmp66A,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,
0),Cyc_Absyn_label_stmt(_tmp66B,s,0),0);stmts=({struct Cyc_List_List*_tmp673=
_region_malloc(rgn,sizeof(*_tmp673));_tmp673->hd=s;_tmp673->tl=stmts;_tmp673;});}}{
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),Cyc_Absyn_label_stmt(end_l,
Cyc_Toc_skip_stmt_dl(),0),0);for(decls;decls != 0;decls=decls->tl){struct _tuple14
_tmp675;struct _tuple1*_tmp676;void*_tmp677;struct _tuple14*_tmp674=(struct
_tuple14*)decls->hd;_tmp675=*_tmp674;_tmp676=_tmp675.f1;_tmp677=_tmp675.f2;res=
Cyc_Absyn_declare_stmt(_tmp676,_tmp677,0,res,0);}(void*)(whole_s->r=(void*)((
void*)(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)((struct Cyc_Core_Opt*)
_check_null(e->topt))->v),(struct Cyc_Absyn_Exp*)e,res,0))->r));}}};_pop_region(
rgn);}}}static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,
struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*
init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);static void Cyc_Toc_fndecl_to_c(
struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);struct Cyc_Absyn_Stmt*
Cyc_Toc_make_npop_handler(int n){return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__npop_handler_e,({struct Cyc_List_List*_tmp678=_cycalloc(sizeof(*_tmp678));
_tmp678->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0);_tmp678->tl=0;_tmp678;}),
0),0);}void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){if(n > 0)(void*)(s->r=(
void*)Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt((void*)
s->r,0)));}static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*
s){while(1){void*_tmp679=(void*)s->r;struct Cyc_Absyn_Exp*_tmp67A;struct Cyc_Absyn_Stmt*
_tmp67B;struct Cyc_Absyn_Stmt*_tmp67C;struct Cyc_Absyn_Exp*_tmp67D;struct Cyc_Absyn_Exp*
_tmp67E;struct Cyc_Absyn_Stmt*_tmp67F;struct Cyc_Absyn_Stmt*_tmp680;struct _tuple3
_tmp681;struct Cyc_Absyn_Exp*_tmp682;struct Cyc_Absyn_Stmt*_tmp683;struct Cyc_Absyn_Stmt*
_tmp684;struct Cyc_Absyn_Stmt*_tmp685;struct Cyc_Absyn_Stmt*_tmp686;struct Cyc_Absyn_Exp*
_tmp687;struct _tuple3 _tmp688;struct Cyc_Absyn_Exp*_tmp689;struct _tuple3 _tmp68A;
struct Cyc_Absyn_Exp*_tmp68B;struct Cyc_Absyn_Stmt*_tmp68C;struct Cyc_Absyn_Exp*
_tmp68D;struct Cyc_List_List*_tmp68E;struct Cyc_List_List*_tmp68F;struct Cyc_Absyn_Switch_clause**
_tmp690;struct Cyc_Absyn_Decl*_tmp691;struct Cyc_Absyn_Stmt*_tmp692;struct
_dyneither_ptr*_tmp693;struct Cyc_Absyn_Stmt*_tmp694;struct Cyc_Absyn_Stmt*_tmp695;
struct _tuple3 _tmp696;struct Cyc_Absyn_Exp*_tmp697;struct Cyc_Absyn_Stmt*_tmp698;
struct Cyc_List_List*_tmp699;struct Cyc_Absyn_Tvar*_tmp69A;struct Cyc_Absyn_Vardecl*
_tmp69B;int _tmp69C;struct Cyc_Absyn_Exp*_tmp69D;struct Cyc_Absyn_Stmt*_tmp69E;
struct Cyc_Absyn_Exp*_tmp69F;struct Cyc_Absyn_Exp*_tmp6A0;struct Cyc_Absyn_Tvar*
_tmp6A1;struct Cyc_Absyn_Vardecl*_tmp6A2;struct Cyc_Absyn_Stmt*_tmp6A3;_LL343: if((
int)_tmp679 != 0)goto _LL345;_LL344: return;_LL345: if(_tmp679 <= (void*)1)goto _LL347;
if(*((int*)_tmp679)!= 0)goto _LL347;_tmp67A=((struct Cyc_Absyn_Exp_s_struct*)
_tmp679)->f1;_LL346: Cyc_Toc_exp_to_c(nv,_tmp67A);return;_LL347: if(_tmp679 <= (
void*)1)goto _LL349;if(*((int*)_tmp679)!= 1)goto _LL349;_tmp67B=((struct Cyc_Absyn_Seq_s_struct*)
_tmp679)->f1;_tmp67C=((struct Cyc_Absyn_Seq_s_struct*)_tmp679)->f2;_LL348: Cyc_Toc_stmt_to_c(
nv,_tmp67B);s=_tmp67C;continue;_LL349: if(_tmp679 <= (void*)1)goto _LL34B;if(*((int*)
_tmp679)!= 2)goto _LL34B;_tmp67D=((struct Cyc_Absyn_Return_s_struct*)_tmp679)->f1;
_LL34A: {struct Cyc_Core_Opt*topt=0;if(_tmp67D != 0){topt=({struct Cyc_Core_Opt*
_tmp6A4=_cycalloc(sizeof(*_tmp6A4));_tmp6A4->v=(void*)Cyc_Toc_typ_to_c((void*)((
struct Cyc_Core_Opt*)_check_null(_tmp67D->topt))->v);_tmp6A4;});Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_tmp67D);}if(s->try_depth > 0){if(topt != 0){struct
_tuple1*_tmp6A5=Cyc_Toc_temp_var();struct Cyc_Absyn_Stmt*_tmp6A6=Cyc_Absyn_return_stmt((
struct Cyc_Absyn_Exp*)Cyc_Absyn_var_exp(_tmp6A5,0),0);(void*)(s->r=(void*)((void*)(
Cyc_Absyn_declare_stmt(_tmp6A5,(void*)topt->v,_tmp67D,Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(
s->try_depth),_tmp6A6,0),0))->r));}else{Cyc_Toc_do_npop_before(s->try_depth,s);}}
return;}_LL34B: if(_tmp679 <= (void*)1)goto _LL34D;if(*((int*)_tmp679)!= 3)goto
_LL34D;_tmp67E=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp679)->f1;_tmp67F=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp679)->f2;_tmp680=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp679)->f3;_LL34C: Cyc_Toc_exp_to_c(nv,_tmp67E);Cyc_Toc_stmt_to_c(nv,_tmp67F);s=
_tmp680;continue;_LL34D: if(_tmp679 <= (void*)1)goto _LL34F;if(*((int*)_tmp679)!= 4)
goto _LL34F;_tmp681=((struct Cyc_Absyn_While_s_struct*)_tmp679)->f1;_tmp682=
_tmp681.f1;_tmp683=((struct Cyc_Absyn_While_s_struct*)_tmp679)->f2;_LL34E: Cyc_Toc_exp_to_c(
nv,_tmp682);{struct _RegionHandle _tmp6A7=_new_region("temp");struct _RegionHandle*
temp=& _tmp6A7;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),
_tmp683);;_pop_region(temp);}return;_LL34F: if(_tmp679 <= (void*)1)goto _LL351;if(*((
int*)_tmp679)!= 5)goto _LL351;_tmp684=((struct Cyc_Absyn_Break_s_struct*)_tmp679)->f1;
_LL350: {struct Cyc_Toc_Env _tmp6A9;struct _dyneither_ptr**_tmp6AA;struct Cyc_Toc_Env*
_tmp6A8=nv;_tmp6A9=*_tmp6A8;_tmp6AA=_tmp6A9.break_lab;if(_tmp6AA != 0)(void*)(s->r=(
void*)Cyc_Toc_goto_stmt_r(*_tmp6AA,0));{int dest_depth=_tmp684 == 0?0: _tmp684->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);return;}}_LL351: if(_tmp679 <= (
void*)1)goto _LL353;if(*((int*)_tmp679)!= 6)goto _LL353;_tmp685=((struct Cyc_Absyn_Continue_s_struct*)
_tmp679)->f1;_LL352: {struct Cyc_Toc_Env _tmp6AC;struct _dyneither_ptr**_tmp6AD;
struct Cyc_Toc_Env*_tmp6AB=nv;_tmp6AC=*_tmp6AB;_tmp6AD=_tmp6AC.continue_lab;if(
_tmp6AD != 0)(void*)(s->r=(void*)Cyc_Toc_goto_stmt_r(*_tmp6AD,0));_tmp686=_tmp685;
goto _LL354;}_LL353: if(_tmp679 <= (void*)1)goto _LL355;if(*((int*)_tmp679)!= 7)goto
_LL355;_tmp686=((struct Cyc_Absyn_Goto_s_struct*)_tmp679)->f2;_LL354: Cyc_Toc_do_npop_before(
s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp686))->try_depth,s);
return;_LL355: if(_tmp679 <= (void*)1)goto _LL357;if(*((int*)_tmp679)!= 8)goto
_LL357;_tmp687=((struct Cyc_Absyn_For_s_struct*)_tmp679)->f1;_tmp688=((struct Cyc_Absyn_For_s_struct*)
_tmp679)->f2;_tmp689=_tmp688.f1;_tmp68A=((struct Cyc_Absyn_For_s_struct*)_tmp679)->f3;
_tmp68B=_tmp68A.f1;_tmp68C=((struct Cyc_Absyn_For_s_struct*)_tmp679)->f4;_LL356:
Cyc_Toc_exp_to_c(nv,_tmp687);Cyc_Toc_exp_to_c(nv,_tmp689);Cyc_Toc_exp_to_c(nv,
_tmp68B);{struct _RegionHandle _tmp6AE=_new_region("temp");struct _RegionHandle*
temp=& _tmp6AE;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),
_tmp68C);;_pop_region(temp);}return;_LL357: if(_tmp679 <= (void*)1)goto _LL359;if(*((
int*)_tmp679)!= 9)goto _LL359;_tmp68D=((struct Cyc_Absyn_Switch_s_struct*)_tmp679)->f1;
_tmp68E=((struct Cyc_Absyn_Switch_s_struct*)_tmp679)->f2;_LL358: Cyc_Toc_xlate_switch(
nv,s,_tmp68D,_tmp68E);return;_LL359: if(_tmp679 <= (void*)1)goto _LL35B;if(*((int*)
_tmp679)!= 10)goto _LL35B;_tmp68F=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp679)->f1;
_tmp690=((struct Cyc_Absyn_Fallthru_s_struct*)_tmp679)->f2;_LL35A: {struct Cyc_Toc_Env
_tmp6B0;struct Cyc_Toc_FallthruInfo*_tmp6B1;struct Cyc_Toc_Env*_tmp6AF=nv;_tmp6B0=*
_tmp6AF;_tmp6B1=_tmp6B0.fallthru_info;if(_tmp6B1 == 0)({void*_tmp6B2=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*
_tmp6B3="fallthru in unexpected place";_tag_dyneither(_tmp6B3,sizeof(char),29);}),
_tag_dyneither(_tmp6B2,sizeof(void*),0));});{struct _dyneither_ptr*_tmp6B5;struct
Cyc_List_List*_tmp6B6;struct Cyc_Dict_Dict _tmp6B7;struct Cyc_Toc_FallthruInfo
_tmp6B4=*_tmp6B1;_tmp6B5=_tmp6B4.label;_tmp6B6=_tmp6B4.binders;_tmp6B7=_tmp6B4.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp6B5,0);Cyc_Toc_do_npop_before(s->try_depth
- ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp690)))->body)->try_depth,
s2);{struct Cyc_List_List*_tmp6B8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_rev)(_tmp6B6);struct Cyc_List_List*_tmp6B9=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp68F);for(0;_tmp6B8 != 0;(_tmp6B8=_tmp6B8->tl,
_tmp6B9=_tmp6B9->tl)){Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)
_check_null(_tmp6B9))->hd);s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct
Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp6B7,(
struct _tuple1*)_tmp6B8->hd),(struct Cyc_Absyn_Exp*)_tmp6B9->hd,0),s2,0);}(void*)(
s->r=(void*)((void*)s2->r));return;}}}}_LL35B: if(_tmp679 <= (void*)1)goto _LL35D;
if(*((int*)_tmp679)!= 11)goto _LL35D;_tmp691=((struct Cyc_Absyn_Decl_s_struct*)
_tmp679)->f1;_tmp692=((struct Cyc_Absyn_Decl_s_struct*)_tmp679)->f2;_LL35C:{void*
_tmp6BA=(void*)_tmp691->r;struct Cyc_Absyn_Vardecl*_tmp6BB;struct Cyc_Absyn_Pat*
_tmp6BC;struct Cyc_Absyn_Exp*_tmp6BD;struct Cyc_List_List*_tmp6BE;struct Cyc_Absyn_Fndecl*
_tmp6BF;_LL36A: if(_tmp6BA <= (void*)2)goto _LL372;if(*((int*)_tmp6BA)!= 0)goto
_LL36C;_tmp6BB=((struct Cyc_Absyn_Var_d_struct*)_tmp6BA)->f1;_LL36B:{struct
_RegionHandle _tmp6C0=_new_region("temp");struct _RegionHandle*temp=& _tmp6C0;
_push_region(temp);{struct Cyc_Toc_Env*_tmp6C1=Cyc_Toc_add_varmap(temp,nv,_tmp6BB->name,
Cyc_Absyn_varb_exp(_tmp6BB->name,(void*)({struct Cyc_Absyn_Local_b_struct*_tmp6C2=
_cycalloc(sizeof(*_tmp6C2));_tmp6C2[0]=({struct Cyc_Absyn_Local_b_struct _tmp6C3;
_tmp6C3.tag=3;_tmp6C3.f1=_tmp6BB;_tmp6C3;});_tmp6C2;}),0));Cyc_Toc_local_decl_to_c(
_tmp6C1,_tmp6C1,_tmp6BB,_tmp692);};_pop_region(temp);}goto _LL369;_LL36C: if(*((
int*)_tmp6BA)!= 2)goto _LL36E;_tmp6BC=((struct Cyc_Absyn_Let_d_struct*)_tmp6BA)->f1;
_tmp6BD=((struct Cyc_Absyn_Let_d_struct*)_tmp6BA)->f3;_LL36D:{void*_tmp6C4=(void*)
_tmp6BC->r;struct Cyc_Absyn_Vardecl*_tmp6C5;struct Cyc_Absyn_Pat*_tmp6C6;struct Cyc_Absyn_Pat
_tmp6C7;void*_tmp6C8;_LL375: if(_tmp6C4 <= (void*)2)goto _LL377;if(*((int*)_tmp6C4)
!= 0)goto _LL377;_tmp6C5=((struct Cyc_Absyn_Var_p_struct*)_tmp6C4)->f1;_tmp6C6=((
struct Cyc_Absyn_Var_p_struct*)_tmp6C4)->f2;_tmp6C7=*_tmp6C6;_tmp6C8=(void*)
_tmp6C7.r;if((int)_tmp6C8 != 0)goto _LL377;_LL376: {struct _tuple1*old_name=_tmp6C5->name;
struct _tuple1*new_name=Cyc_Toc_temp_var();_tmp6C5->name=new_name;_tmp6C5->initializer=(
struct Cyc_Absyn_Exp*)_tmp6BD;(void*)(_tmp691->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp6C9=_cycalloc(sizeof(*_tmp6C9));_tmp6C9[0]=({struct Cyc_Absyn_Var_d_struct
_tmp6CA;_tmp6CA.tag=0;_tmp6CA.f1=_tmp6C5;_tmp6CA;});_tmp6C9;})));{struct
_RegionHandle _tmp6CB=_new_region("temp");struct _RegionHandle*temp=& _tmp6CB;
_push_region(temp);{struct Cyc_Toc_Env*_tmp6CC=Cyc_Toc_add_varmap(temp,nv,
old_name,Cyc_Absyn_varb_exp(new_name,(void*)({struct Cyc_Absyn_Local_b_struct*
_tmp6CD=_cycalloc(sizeof(*_tmp6CD));_tmp6CD[0]=({struct Cyc_Absyn_Local_b_struct
_tmp6CE;_tmp6CE.tag=3;_tmp6CE.f1=_tmp6C5;_tmp6CE;});_tmp6CD;}),0));Cyc_Toc_local_decl_to_c(
_tmp6CC,nv,_tmp6C5,_tmp692);};_pop_region(temp);}goto _LL374;}_LL377:;_LL378:(
void*)(s->r=(void*)((void*)(Cyc_Toc_letdecl_to_c(nv,_tmp6BC,(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp6BD->topt))->v,_tmp6BD,_tmp692))->r));goto _LL374;_LL374:;}goto
_LL369;_LL36E: if(*((int*)_tmp6BA)!= 3)goto _LL370;_tmp6BE=((struct Cyc_Absyn_Letv_d_struct*)
_tmp6BA)->f1;_LL36F: {struct Cyc_List_List*_tmp6CF=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_rev)(_tmp6BE);if(_tmp6CF == 0)({void*_tmp6D0=0;((
int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const
char*_tmp6D1="empty Letv_d";_tag_dyneither(_tmp6D1,sizeof(char),13);}),
_tag_dyneither(_tmp6D0,sizeof(void*),0));});(void*)(_tmp691->r=(void*)((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp6D2=_cycalloc(sizeof(*_tmp6D2));_tmp6D2[0]=({
struct Cyc_Absyn_Var_d_struct _tmp6D3;_tmp6D3.tag=0;_tmp6D3.f1=(struct Cyc_Absyn_Vardecl*)
_tmp6CF->hd;_tmp6D3;});_tmp6D2;})));_tmp6CF=_tmp6CF->tl;for(0;_tmp6CF != 0;
_tmp6CF=_tmp6CF->tl){struct Cyc_Absyn_Decl*_tmp6D4=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp6D5=_cycalloc(sizeof(*_tmp6D5));_tmp6D5[0]=({
struct Cyc_Absyn_Var_d_struct _tmp6D6;_tmp6D6.tag=0;_tmp6D6.f1=(struct Cyc_Absyn_Vardecl*)
_tmp6CF->hd;_tmp6D6;});_tmp6D5;}),0);(void*)(s->r=(void*)((void*)(Cyc_Absyn_decl_stmt(
_tmp6D4,Cyc_Absyn_new_stmt((void*)s->r,0),0))->r));}Cyc_Toc_stmt_to_c(nv,s);goto
_LL369;}_LL370: if(*((int*)_tmp6BA)!= 1)goto _LL372;_tmp6BF=((struct Cyc_Absyn_Fn_d_struct*)
_tmp6BA)->f1;_LL371: {struct _tuple1*_tmp6D7=_tmp6BF->name;{struct _RegionHandle
_tmp6D8=_new_region("temp");struct _RegionHandle*temp=& _tmp6D8;_push_region(temp);{
struct Cyc_Toc_Env*_tmp6D9=Cyc_Toc_add_varmap(temp,nv,_tmp6BF->name,Cyc_Absyn_var_exp(
_tmp6D7,0));Cyc_Toc_fndecl_to_c(_tmp6D9,_tmp6BF,0);Cyc_Toc_stmt_to_c(_tmp6D9,
_tmp692);};_pop_region(temp);}goto _LL369;}_LL372:;_LL373:({void*_tmp6DA=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp6DB="bad nested declaration within function";_tag_dyneither(_tmp6DB,sizeof(
char),39);}),_tag_dyneither(_tmp6DA,sizeof(void*),0));});_LL369:;}return;_LL35D:
if(_tmp679 <= (void*)1)goto _LL35F;if(*((int*)_tmp679)!= 12)goto _LL35F;_tmp693=((
struct Cyc_Absyn_Label_s_struct*)_tmp679)->f1;_tmp694=((struct Cyc_Absyn_Label_s_struct*)
_tmp679)->f2;_LL35E: s=_tmp694;continue;_LL35F: if(_tmp679 <= (void*)1)goto _LL361;
if(*((int*)_tmp679)!= 13)goto _LL361;_tmp695=((struct Cyc_Absyn_Do_s_struct*)
_tmp679)->f1;_tmp696=((struct Cyc_Absyn_Do_s_struct*)_tmp679)->f2;_tmp697=_tmp696.f1;
_LL360:{struct _RegionHandle _tmp6DC=_new_region("temp");struct _RegionHandle*temp=&
_tmp6DC;_push_region(temp);Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp695);
Cyc_Toc_exp_to_c(nv,_tmp697);;_pop_region(temp);}return;_LL361: if(_tmp679 <= (
void*)1)goto _LL363;if(*((int*)_tmp679)!= 14)goto _LL363;_tmp698=((struct Cyc_Absyn_TryCatch_s_struct*)
_tmp679)->f1;_tmp699=((struct Cyc_Absyn_TryCatch_s_struct*)_tmp679)->f2;_LL362: {
struct _tuple1*h_var=Cyc_Toc_temp_var();struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(
h_var,0);struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);struct Cyc_Absyn_Exp*
was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);void*h_typ=Cyc_Absyn_strct(
Cyc_Toc__handler_cons_sp);void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ);void*
was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);e_exp->topt=({struct Cyc_Core_Opt*
_tmp6DD=_cycalloc(sizeof(*_tmp6DD));_tmp6DD->v=(void*)e_typ;_tmp6DD;});{struct
_RegionHandle _tmp6DE=_new_region("temp");struct _RegionHandle*temp=& _tmp6DE;
_push_region(temp);{struct Cyc_Toc_Env*_tmp6DF=Cyc_Toc_add_varmap(temp,nv,e_var,
e_exp);Cyc_Toc_stmt_to_c(_tmp6DF,_tmp698);{struct Cyc_Absyn_Stmt*_tmp6E0=Cyc_Absyn_seq_stmt(
_tmp698,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
struct _tuple1*_tmp6E1=Cyc_Toc_temp_var();struct Cyc_Absyn_Exp*_tmp6E2=Cyc_Absyn_var_exp(
_tmp6E1,0);struct Cyc_Absyn_Vardecl*_tmp6E3=Cyc_Absyn_new_vardecl(_tmp6E1,Cyc_Absyn_exn_typ,
0);_tmp6E2->topt=({struct Cyc_Core_Opt*_tmp6E4=_cycalloc(sizeof(*_tmp6E4));
_tmp6E4->v=(void*)Cyc_Absyn_exn_typ;_tmp6E4;});{struct Cyc_Absyn_Pat*_tmp6E5=({
struct Cyc_Absyn_Pat*_tmp6F5=_cycalloc(sizeof(*_tmp6F5));_tmp6F5->r=(void*)((void*)({
struct Cyc_Absyn_Var_p_struct*_tmp6F7=_cycalloc(sizeof(*_tmp6F7));_tmp6F7[0]=({
struct Cyc_Absyn_Var_p_struct _tmp6F8;_tmp6F8.tag=0;_tmp6F8.f1=_tmp6E3;_tmp6F8.f2=({
struct Cyc_Absyn_Pat*_tmp6F9=_cycalloc(sizeof(*_tmp6F9));_tmp6F9->r=(void*)((void*)
0);_tmp6F9->topt=({struct Cyc_Core_Opt*_tmp6FA=_cycalloc(sizeof(*_tmp6FA));
_tmp6FA->v=(void*)Cyc_Absyn_exn_typ;_tmp6FA;});_tmp6F9->loc=0;_tmp6F9;});_tmp6F8;});
_tmp6F7;}));_tmp6F5->topt=({struct Cyc_Core_Opt*_tmp6F6=_cycalloc(sizeof(*_tmp6F6));
_tmp6F6->v=(void*)Cyc_Absyn_exn_typ;_tmp6F6;});_tmp6F5->loc=0;_tmp6F5;});struct
Cyc_Absyn_Exp*_tmp6E6=Cyc_Absyn_throw_exp(_tmp6E2,0);_tmp6E6->topt=({struct Cyc_Core_Opt*
_tmp6E7=_cycalloc(sizeof(*_tmp6E7));_tmp6E7->v=(void*)((void*)0);_tmp6E7;});{
struct Cyc_Absyn_Stmt*_tmp6E8=Cyc_Absyn_exp_stmt(_tmp6E6,0);struct Cyc_Absyn_Switch_clause*
_tmp6E9=({struct Cyc_Absyn_Switch_clause*_tmp6F2=_cycalloc(sizeof(*_tmp6F2));
_tmp6F2->pattern=_tmp6E5;_tmp6F2->pat_vars=({struct Cyc_Core_Opt*_tmp6F3=
_cycalloc(sizeof(*_tmp6F3));_tmp6F3->v=({struct Cyc_List_List*_tmp6F4=_cycalloc(
sizeof(*_tmp6F4));_tmp6F4->hd=_tmp6E3;_tmp6F4->tl=0;_tmp6F4;});_tmp6F3;});
_tmp6F2->where_clause=0;_tmp6F2->body=_tmp6E8;_tmp6F2->loc=0;_tmp6F2;});struct
Cyc_Absyn_Stmt*_tmp6EA=Cyc_Absyn_switch_stmt(e_exp,((struct Cyc_List_List*(*)(
struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp699,({struct
Cyc_List_List*_tmp6F1=_cycalloc(sizeof(*_tmp6F1));_tmp6F1->hd=_tmp6E9;_tmp6F1->tl=
0;_tmp6F1;})),0);Cyc_Toc_stmt_to_c(_tmp6DF,_tmp6EA);{struct Cyc_Absyn_Exp*_tmp6EB=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,({struct Cyc_List_List*_tmp6F0=_cycalloc(
sizeof(*_tmp6F0));_tmp6F0->hd=Cyc_Absyn_aggrmember_exp(h_exp,Cyc_Toc_handler_sp,
0);_tmp6F0->tl=0;_tmp6F0;}),0);struct Cyc_Absyn_Stmt*_tmp6EC=Cyc_Absyn_exp_stmt(
Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,({struct Cyc_List_List*_tmp6EF=
_cycalloc(sizeof(*_tmp6EF));_tmp6EF->hd=Cyc_Absyn_address_exp(h_exp,0);_tmp6EF->tl=
0;_tmp6EF;}),0),0);struct Cyc_Absyn_Exp*_tmp6ED=Cyc_Absyn_int_exp((void*)0,0,0);
struct Cyc_Absyn_Exp*_tmp6EE=Cyc_Absyn_int_exp((void*)0,1,0);(void*)(s->r=(void*)((
void*)(Cyc_Absyn_declare_stmt(h_var,h_typ,0,Cyc_Absyn_seq_stmt(_tmp6EC,Cyc_Absyn_declare_stmt(
was_thrown_var,was_thrown_typ,(struct Cyc_Absyn_Exp*)_tmp6ED,Cyc_Absyn_seq_stmt(
Cyc_Absyn_ifthenelse_stmt(_tmp6EB,Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp6EE,0),
Cyc_Toc_skip_stmt_dl(),0),Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp((void*)
11,was_thrown_exp,0),_tmp6E0,Cyc_Absyn_declare_stmt(e_var,e_typ,(struct Cyc_Absyn_Exp*)
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp6EA,0),0),0),0),0),0))->r));}}}}};
_pop_region(temp);}return;}_LL363: if(_tmp679 <= (void*)1)goto _LL365;if(*((int*)
_tmp679)!= 15)goto _LL365;_tmp69A=((struct Cyc_Absyn_Region_s_struct*)_tmp679)->f1;
_tmp69B=((struct Cyc_Absyn_Region_s_struct*)_tmp679)->f2;_tmp69C=((struct Cyc_Absyn_Region_s_struct*)
_tmp679)->f3;_tmp69D=((struct Cyc_Absyn_Region_s_struct*)_tmp679)->f4;_tmp69E=((
struct Cyc_Absyn_Region_s_struct*)_tmp679)->f5;_LL364: {void*rh_struct_typ=Cyc_Absyn_strct(
Cyc_Toc__RegionHandle_sp);void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(
rh_struct_typ,Cyc_Toc_mt_tq);struct _tuple1*rh_var=Cyc_Toc_temp_var();struct
_tuple1*x_var=_tmp69B->name;struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,
0);struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);{struct _RegionHandle
_tmp6FB=_new_region("temp");struct _RegionHandle*temp=& _tmp6FB;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp69E);;_pop_region(
temp);}if(Cyc_Absyn_no_regions)(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_uint_exp(0,0),_tmp69E,0))->r));
else{if(_tmp69D == 0)(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(rh_var,
rh_struct_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,({
struct Cyc_List_List*_tmp6FC=_cycalloc(sizeof(*_tmp6FC));_tmp6FC->hd=Cyc_Absyn_string_exp(
Cyc_Absynpp_qvar2string(x_var),0);_tmp6FC->tl=0;_tmp6FC;}),0),Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,({
struct Cyc_Absyn_Exp*_tmp6FD[1];_tmp6FD[0]=x_exp;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6FD,sizeof(struct Cyc_Absyn_Exp*),
1));}),0),0),Cyc_Absyn_seq_stmt(_tmp69E,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(
Cyc_Toc__pop_region_e,({struct Cyc_Absyn_Exp*_tmp6FE[1];_tmp6FE[0]=x_exp;((struct
Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6FE,
sizeof(struct Cyc_Absyn_Exp*),1));}),0),0),0),0),0),0))->r));else{Cyc_Toc_exp_to_c(
nv,(struct Cyc_Absyn_Exp*)_tmp69D);(void*)(s->r=(void*)((void*)(Cyc_Absyn_declare_stmt(
rh_var,Cyc_Absyn_strct(Cyc_Toc__DynRegionFrame_sp),0,Cyc_Absyn_declare_stmt(
x_var,rh_struct_ptr_typ,(struct Cyc_Absyn_Exp*)Cyc_Absyn_fncall_exp(Cyc_Toc__open_dynregion_e,({
struct Cyc_Absyn_Exp*_tmp6FF[2];_tmp6FF[1]=(struct Cyc_Absyn_Exp*)_tmp69D;_tmp6FF[
0]=Cyc_Absyn_address_exp(rh_exp,0);((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6FF,sizeof(struct Cyc_Absyn_Exp*),
2));}),0),Cyc_Absyn_seq_stmt(_tmp69E,Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_dynregion_e,({
struct Cyc_Absyn_Exp*_tmp700[1];_tmp700[0]=x_exp;((struct Cyc_List_List*(*)(struct
_dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp700,sizeof(struct Cyc_Absyn_Exp*),
1));}),0),0),0),0),0))->r));}}return;}_LL365: if(_tmp679 <= (void*)1)goto _LL367;
if(*((int*)_tmp679)!= 16)goto _LL367;_tmp69F=((struct Cyc_Absyn_ResetRegion_s_struct*)
_tmp679)->f1;_LL366: if(Cyc_Absyn_no_regions)(void*)(s->r=(void*)((void*)0));
else{Cyc_Toc_exp_to_c(nv,_tmp69F);(void*)(s->r=(void*)Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(
Cyc_Toc__reset_region_e,({struct Cyc_List_List*_tmp701=_cycalloc(sizeof(*_tmp701));
_tmp701->hd=_tmp69F;_tmp701->tl=0;_tmp701;}),0)));}return;_LL367: if(_tmp679 <= (
void*)1)goto _LL342;if(*((int*)_tmp679)!= 17)goto _LL342;_tmp6A0=((struct Cyc_Absyn_Alias_s_struct*)
_tmp679)->f1;_tmp6A1=((struct Cyc_Absyn_Alias_s_struct*)_tmp679)->f2;_tmp6A2=((
struct Cyc_Absyn_Alias_s_struct*)_tmp679)->f3;_tmp6A3=((struct Cyc_Absyn_Alias_s_struct*)
_tmp679)->f4;_LL368: {struct _tuple1*old_name=_tmp6A2->name;struct _tuple1*
new_name=Cyc_Toc_temp_var();_tmp6A2->name=new_name;_tmp6A2->initializer=(struct
Cyc_Absyn_Exp*)_tmp6A0;(void*)(s->r=(void*)((void*)({struct Cyc_Absyn_Decl_s_struct*
_tmp702=_cycalloc(sizeof(*_tmp702));_tmp702[0]=({struct Cyc_Absyn_Decl_s_struct
_tmp703;_tmp703.tag=11;_tmp703.f1=({struct Cyc_Absyn_Decl*_tmp704=_cycalloc(
sizeof(*_tmp704));_tmp704->r=(void*)((void*)({struct Cyc_Absyn_Var_d_struct*
_tmp705=_cycalloc(sizeof(*_tmp705));_tmp705[0]=({struct Cyc_Absyn_Var_d_struct
_tmp706;_tmp706.tag=0;_tmp706.f1=_tmp6A2;_tmp706;});_tmp705;}));_tmp704->loc=0;
_tmp704;});_tmp703.f2=_tmp6A3;_tmp703;});_tmp702;})));{struct _RegionHandle
_tmp707=_new_region("temp");struct _RegionHandle*temp=& _tmp707;_push_region(temp);{
struct Cyc_Toc_Env*_tmp708=Cyc_Toc_add_varmap(temp,nv,old_name,Cyc_Absyn_varb_exp(
new_name,(void*)({struct Cyc_Absyn_Local_b_struct*_tmp709=_cycalloc(sizeof(*
_tmp709));_tmp709[0]=({struct Cyc_Absyn_Local_b_struct _tmp70A;_tmp70A.tag=3;
_tmp70A.f1=_tmp6A2;_tmp70A;});_tmp709;}),0));Cyc_Toc_local_decl_to_c(_tmp708,nv,
_tmp6A2,_tmp6A3);};_pop_region(temp);}return;}_LL342:;}}static void Cyc_Toc_stmttypes_to_c(
struct Cyc_Absyn_Stmt*s);struct _tuple17{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual
f2;void*f3;};static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*
f,int cinclude){f->tvs=0;f->effect=0;f->rgn_po=0;(void*)(f->ret_type=(void*)Cyc_Toc_typ_to_c((
void*)f->ret_type));{struct _RegionHandle _tmp70B=_new_region("frgn");struct
_RegionHandle*frgn=& _tmp70B;_push_region(frgn);{struct Cyc_Toc_Env*_tmp70C=Cyc_Toc_share_env(
frgn,nv);{struct Cyc_List_List*_tmp70D=f->args;for(0;_tmp70D != 0;_tmp70D=_tmp70D->tl){
struct _tuple1*_tmp70E=({struct _tuple1*_tmp70F=_cycalloc(sizeof(*_tmp70F));
_tmp70F->f1=(union Cyc_Absyn_Nmspace_union)((union Cyc_Absyn_Nmspace_union)({union
Cyc_Absyn_Nmspace_union _tmp710;(_tmp710.Loc_n).tag=0;_tmp710;}));_tmp70F->f2=(*((
struct _tuple17*)_tmp70D->hd)).f1;_tmp70F;});(*((struct _tuple17*)_tmp70D->hd)).f3=
Cyc_Toc_typ_to_c((*((struct _tuple17*)_tmp70D->hd)).f3);_tmp70C=Cyc_Toc_add_varmap(
frgn,_tmp70C,_tmp70E,Cyc_Absyn_var_exp(_tmp70E,0));}}if(cinclude){Cyc_Toc_stmttypes_to_c(
f->body);_npop_handler(0);return;}if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs))->name != 0){struct Cyc_Core_Opt*_tmp712;struct Cyc_Absyn_Tqual
_tmp713;void*_tmp714;int _tmp715;struct Cyc_Absyn_VarargInfo _tmp711=*((struct Cyc_Absyn_VarargInfo*)
_check_null(f->cyc_varargs));_tmp712=_tmp711.name;_tmp713=_tmp711.tq;_tmp714=(
void*)_tmp711.type;_tmp715=_tmp711.inject;{void*_tmp716=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(
_tmp714,(void*)2,_tmp713,Cyc_Absyn_false_conref));struct _tuple1*_tmp717=({struct
_tuple1*_tmp71A=_cycalloc(sizeof(*_tmp71A));_tmp71A->f1=(union Cyc_Absyn_Nmspace_union)((
union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp71B;(_tmp71B.Loc_n).tag=
0;_tmp71B;}));_tmp71A->f2=(struct _dyneither_ptr*)((struct Cyc_Core_Opt*)
_check_null(_tmp712))->v;_tmp71A;});f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*
x,struct Cyc_List_List*y))Cyc_List_append)(f->args,({struct Cyc_List_List*_tmp718=
_cycalloc(sizeof(*_tmp718));_tmp718->hd=({struct _tuple17*_tmp719=_cycalloc(
sizeof(*_tmp719));_tmp719->f1=(struct _dyneither_ptr*)_tmp712->v;_tmp719->f2=
_tmp713;_tmp719->f3=_tmp716;_tmp719;});_tmp718->tl=0;_tmp718;}));_tmp70C=Cyc_Toc_add_varmap(
frgn,_tmp70C,_tmp717,Cyc_Absyn_var_exp(_tmp717,0));f->cyc_varargs=0;}}{struct Cyc_List_List*
_tmp71C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;
for(0;_tmp71C != 0;_tmp71C=_tmp71C->tl){(void*)(((struct Cyc_Absyn_Vardecl*)
_tmp71C->hd)->type=(void*)Cyc_Toc_typ_to_c((void*)((struct Cyc_Absyn_Vardecl*)
_tmp71C->hd)->type));}}Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp70C),f->body);};
_pop_region(frgn);}}static void*Cyc_Toc_scope_to_c(void*s){void*_tmp71D=s;_LL37A:
if((int)_tmp71D != 1)goto _LL37C;_LL37B: return(void*)2;_LL37C: if((int)_tmp71D != 4)
goto _LL37E;_LL37D: return(void*)3;_LL37E:;_LL37F: return s;_LL379:;}static void Cyc_Toc_aggrdecl_to_c(
struct Cyc_Absyn_Aggrdecl*ad){struct _tuple1*_tmp71E=ad->name;struct
_DynRegionHandle*_tmp720;struct Cyc_Dict_Dict*_tmp721;struct Cyc_Toc_TocState
_tmp71F=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp720=
_tmp71F.dyn;_tmp721=_tmp71F.aggrs_so_far;{struct _DynRegionFrame _tmp722;struct
_RegionHandle*d=_open_dynregion(& _tmp722,_tmp720);{int seen_defn_before;struct
_tuple7**_tmp723=((struct _tuple7**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*
_tmp721,_tmp71E);if(_tmp723 == 0){seen_defn_before=0;{struct _tuple7*v;if((void*)
ad->kind == (void*)0)v=({struct _tuple7*_tmp724=_region_malloc(d,sizeof(*_tmp724));
_tmp724->f1=ad;_tmp724->f2=Cyc_Absyn_strctq(ad->name);_tmp724;});else{v=({struct
_tuple7*_tmp725=_region_malloc(d,sizeof(*_tmp725));_tmp725->f1=ad;_tmp725->f2=
Cyc_Absyn_unionq_typ(ad->name);_tmp725;});}*_tmp721=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple7*v))Cyc_Dict_insert)(*_tmp721,
_tmp71E,v);}}else{struct _tuple7 _tmp727;struct Cyc_Absyn_Aggrdecl*_tmp728;void*
_tmp729;struct _tuple7*_tmp726=*_tmp723;_tmp727=*_tmp726;_tmp728=_tmp727.f1;
_tmp729=_tmp727.f2;if(_tmp728->impl == 0){*_tmp721=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple7*v))Cyc_Dict_insert)(*_tmp721,
_tmp71E,({struct _tuple7*_tmp72A=_region_malloc(d,sizeof(*_tmp72A));_tmp72A->f1=
ad;_tmp72A->f2=_tmp729;_tmp72A;}));seen_defn_before=0;}else{seen_defn_before=1;}}(
void*)(ad->sc=(void*)((void*)2));ad->tvs=0;if(ad->impl != 0){((struct Cyc_Absyn_AggrdeclImpl*)
_check_null(ad->impl))->exist_vars=0;((struct Cyc_Absyn_AggrdeclImpl*)_check_null(
ad->impl))->rgn_po=0;if(seen_defn_before)ad->impl=0;else{struct Cyc_List_List*
_tmp72B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;
_tmp72B != 0;_tmp72B=_tmp72B->tl){((struct Cyc_Absyn_Aggrfield*)_tmp72B->hd)->tq=
Cyc_Toc_mt_tq;(void*)(((struct Cyc_Absyn_Aggrfield*)_tmp72B->hd)->type=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Absyn_Aggrfield*)_tmp72B->hd)->type));}}}};_pop_dynregion(d);}}
static void Cyc_Toc_tuniondecl_to_c(struct Cyc_Absyn_Tuniondecl*tud){struct
_DynRegionHandle*_tmp72D;struct Cyc_Set_Set**_tmp72E;struct Cyc_Toc_TocState
_tmp72C=*((struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp72D=
_tmp72C.dyn;_tmp72E=_tmp72C.tunions_so_far;{struct _DynRegionFrame _tmp72F;struct
_RegionHandle*d=_open_dynregion(& _tmp72F,_tmp72D);{struct _tuple1*_tmp730=tud->name;
if(tud->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*
_tmp72E,_tmp730)){_npop_handler(0);return;}*_tmp72E=((struct Cyc_Set_Set*(*)(
struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*
_tmp72E,_tmp730);};_pop_dynregion(d);}{struct Cyc_List_List*flat_structs=0;{
struct Cyc_List_List*_tmp731=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(tud->fields))->v;for(0;_tmp731 != 0;_tmp731=_tmp731->tl){struct Cyc_Absyn_Tunionfield*
f=(struct Cyc_Absyn_Tunionfield*)_tmp731->hd;if(f->typs != 0  || tud->is_flat){
struct Cyc_List_List*_tmp732=0;int i=1;{struct Cyc_List_List*_tmp733=f->typs;for(0;
_tmp733 != 0;(_tmp733=_tmp733->tl,i ++)){struct _dyneither_ptr*_tmp734=Cyc_Absyn_fieldname(
i);struct Cyc_Absyn_Aggrfield*_tmp735=({struct Cyc_Absyn_Aggrfield*_tmp737=
_cycalloc(sizeof(*_tmp737));_tmp737->name=_tmp734;_tmp737->tq=(*((struct _tuple4*)
_tmp733->hd)).f1;_tmp737->type=(void*)Cyc_Toc_typ_to_c_array((*((struct _tuple4*)
_tmp733->hd)).f2);_tmp737->width=0;_tmp737->attributes=0;_tmp737;});_tmp732=({
struct Cyc_List_List*_tmp736=_cycalloc(sizeof(*_tmp736));_tmp736->hd=_tmp735;
_tmp736->tl=_tmp732;_tmp736;});}}_tmp732=({struct Cyc_List_List*_tmp738=_cycalloc(
sizeof(*_tmp738));_tmp738->hd=({struct Cyc_Absyn_Aggrfield*_tmp739=_cycalloc(
sizeof(*_tmp739));_tmp739->name=Cyc_Toc_tag_sp;_tmp739->tq=Cyc_Toc_mt_tq;_tmp739->type=(
void*)Cyc_Absyn_sint_typ;_tmp739->width=0;_tmp739->attributes=0;_tmp739;});
_tmp738->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(
_tmp732);_tmp738;});{struct Cyc_Absyn_Aggrdecl*_tmp73A=({struct Cyc_Absyn_Aggrdecl*
_tmp746=_cycalloc(sizeof(*_tmp746));_tmp746->kind=(void*)((void*)0);_tmp746->sc=(
void*)((void*)2);_tmp746->name=Cyc_Toc_collapse_qvar_tag(f->name,({const char*
_tmp748="_struct";_tag_dyneither(_tmp748,sizeof(char),8);}));_tmp746->tvs=0;
_tmp746->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp747=_cycalloc(sizeof(*_tmp747));
_tmp747->exist_vars=0;_tmp747->rgn_po=0;_tmp747->fields=_tmp732;_tmp747;});
_tmp746->attributes=0;_tmp746;});Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp73B=_cycalloc(sizeof(*_tmp73B));_tmp73B->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Aggr_d_struct*_tmp73C=_cycalloc(sizeof(*_tmp73C));_tmp73C[0]=({
struct Cyc_Absyn_Aggr_d_struct _tmp73D;_tmp73D.tag=4;_tmp73D.f1=_tmp73A;_tmp73D;});
_tmp73C;}),0);_tmp73B->tl=Cyc_Toc_result_decls;_tmp73B;});if(tud->is_flat){
struct Cyc_Absyn_Aggrfield*_tmp73E=({struct Cyc_Absyn_Aggrfield*_tmp740=_cycalloc(
sizeof(*_tmp740));_tmp740->name=(*f->name).f2;_tmp740->tq=Cyc_Toc_mt_tq;_tmp740->type=(
void*)((void*)({struct Cyc_Absyn_AggrType_struct*_tmp741=_cycalloc(sizeof(*
_tmp741));_tmp741[0]=({struct Cyc_Absyn_AggrType_struct _tmp742;_tmp742.tag=10;
_tmp742.f1=({struct Cyc_Absyn_AggrInfo _tmp743;_tmp743.aggr_info=(union Cyc_Absyn_AggrInfoU_union)({
union Cyc_Absyn_AggrInfoU_union _tmp744;(_tmp744.KnownAggr).tag=1;(_tmp744.KnownAggr).f1=({
struct Cyc_Absyn_Aggrdecl**_tmp745=_cycalloc(sizeof(*_tmp745));_tmp745[0]=_tmp73A;
_tmp745;});_tmp744;});_tmp743.targs=0;_tmp743;});_tmp742;});_tmp741;}));_tmp740->width=
0;_tmp740->attributes=0;_tmp740;});flat_structs=({struct Cyc_List_List*_tmp73F=
_cycalloc(sizeof(*_tmp73F));_tmp73F->hd=_tmp73E;_tmp73F->tl=flat_structs;_tmp73F;});}}}}}
if(tud->is_flat){flat_structs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))
Cyc_List_imp_rev)(flat_structs);{struct Cyc_Absyn_Aggrdecl*_tmp749=({struct Cyc_Absyn_Aggrdecl*
_tmp74D=_cycalloc(sizeof(*_tmp74D));_tmp74D->kind=(void*)((void*)1);_tmp74D->sc=(
void*)((void*)2);_tmp74D->name=Cyc_Toc_collapse_qvar_tag(tud->name,({const char*
_tmp74F="_union";_tag_dyneither(_tmp74F,sizeof(char),7);}));_tmp74D->tvs=0;
_tmp74D->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp74E=_cycalloc(sizeof(*_tmp74E));
_tmp74E->exist_vars=0;_tmp74E->rgn_po=0;_tmp74E->fields=flat_structs;_tmp74E;});
_tmp74D->attributes=0;_tmp74D;});Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp74A=_cycalloc(sizeof(*_tmp74A));_tmp74A->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Aggr_d_struct*_tmp74B=_cycalloc(sizeof(*_tmp74B));_tmp74B[0]=({
struct Cyc_Absyn_Aggr_d_struct _tmp74C;_tmp74C.tag=4;_tmp74C.f1=_tmp749;_tmp74C;});
_tmp74B;}),0);_tmp74A->tl=Cyc_Toc_result_decls;_tmp74A;});}}}}static void Cyc_Toc_xtuniondecl_to_c(
struct Cyc_Absyn_Tuniondecl*xd){if(xd->fields == 0)return;{struct _DynRegionHandle*
_tmp751;struct Cyc_Dict_Dict*_tmp752;struct Cyc_Toc_TocState _tmp750=*((struct Cyc_Toc_TocState*)
_check_null(Cyc_Toc_toc_state));_tmp751=_tmp750.dyn;_tmp752=_tmp750.xtunions_so_far;{
struct _DynRegionFrame _tmp753;struct _RegionHandle*d=_open_dynregion(& _tmp753,
_tmp751);{struct _tuple1*_tmp754=xd->name;struct Cyc_List_List*_tmp755=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(xd->fields))->v;for(0;_tmp755 != 0;_tmp755=
_tmp755->tl){struct Cyc_Absyn_Tunionfield*f=(struct Cyc_Absyn_Tunionfield*)_tmp755->hd;
struct _dyneither_ptr*fn=(*f->name).f2;struct Cyc_Absyn_Exp*_tmp756=Cyc_Absyn_uint_exp(
_get_dyneither_size(*fn,sizeof(char))+ 4,0);void*_tmp757=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,
Cyc_Toc_mt_tq,(struct Cyc_Absyn_Exp*)_tmp756,Cyc_Absyn_false_conref,0);int*
_tmp758=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*
_tmp752,f->name);int _tmp759;_LL381: if(_tmp758 != 0)goto _LL383;_LL382: {struct Cyc_Absyn_Exp*
initopt=0;if((void*)f->sc != (void*)3){char zero='\000';initopt=(struct Cyc_Absyn_Exp*)
Cyc_Absyn_string_exp((struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp760;
_tmp760.tag=0;_tmp760.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);{
struct Cyc_Int_pa_struct _tmp75F;_tmp75F.tag=1;_tmp75F.f1=(unsigned long)((int)
zero);{struct Cyc_Int_pa_struct _tmp75E;_tmp75E.tag=1;_tmp75E.f1=(unsigned long)((
int)zero);{struct Cyc_Int_pa_struct _tmp75D;_tmp75D.tag=1;_tmp75D.f1=(
unsigned long)((int)zero);{struct Cyc_Int_pa_struct _tmp75C;_tmp75C.tag=1;_tmp75C.f1=(
unsigned long)((int)zero);{void*_tmp75A[5]={& _tmp75C,& _tmp75D,& _tmp75E,& _tmp75F,&
_tmp760};Cyc_aprintf(({const char*_tmp75B="%c%c%c%c%s";_tag_dyneither(_tmp75B,
sizeof(char),11);}),_tag_dyneither(_tmp75A,sizeof(void*),5));}}}}}}),0);}{struct
Cyc_Absyn_Vardecl*_tmp761=Cyc_Absyn_new_vardecl(f->name,_tmp757,initopt);(void*)(
_tmp761->sc=(void*)((void*)f->sc));Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp762=_cycalloc(sizeof(*_tmp762));_tmp762->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Var_d_struct*_tmp763=_cycalloc(sizeof(*_tmp763));_tmp763[0]=({
struct Cyc_Absyn_Var_d_struct _tmp764;_tmp764.tag=0;_tmp764.f1=_tmp761;_tmp764;});
_tmp763;}),0);_tmp762->tl=Cyc_Toc_result_decls;_tmp762;});*_tmp752=((struct Cyc_Dict_Dict(*)(
struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp752,f->name,(
void*)f->sc != (void*)3);if(f->typs != 0){struct Cyc_List_List*fields=0;int i=1;{
struct Cyc_List_List*_tmp765=f->typs;for(0;_tmp765 != 0;(_tmp765=_tmp765->tl,i ++)){
struct _dyneither_ptr*_tmp766=({struct _dyneither_ptr*_tmp76A=_cycalloc(sizeof(*
_tmp76A));_tmp76A[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_struct _tmp76D;
_tmp76D.tag=1;_tmp76D.f1=(unsigned long)i;{void*_tmp76B[1]={& _tmp76D};Cyc_aprintf(({
const char*_tmp76C="f%d";_tag_dyneither(_tmp76C,sizeof(char),4);}),_tag_dyneither(
_tmp76B,sizeof(void*),1));}});_tmp76A;});struct Cyc_Absyn_Aggrfield*_tmp767=({
struct Cyc_Absyn_Aggrfield*_tmp769=_cycalloc(sizeof(*_tmp769));_tmp769->name=
_tmp766;_tmp769->tq=(*((struct _tuple4*)_tmp765->hd)).f1;_tmp769->type=(void*)Cyc_Toc_typ_to_c_array((*((
struct _tuple4*)_tmp765->hd)).f2);_tmp769->width=0;_tmp769->attributes=0;_tmp769;});
fields=({struct Cyc_List_List*_tmp768=_cycalloc(sizeof(*_tmp768));_tmp768->hd=
_tmp767;_tmp768->tl=fields;_tmp768;});}}fields=({struct Cyc_List_List*_tmp76E=
_cycalloc(sizeof(*_tmp76E));_tmp76E->hd=({struct Cyc_Absyn_Aggrfield*_tmp76F=
_cycalloc(sizeof(*_tmp76F));_tmp76F->name=Cyc_Toc_tag_sp;_tmp76F->tq=Cyc_Toc_mt_tq;
_tmp76F->type=(void*)Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);
_tmp76F->width=0;_tmp76F->attributes=0;_tmp76F;});_tmp76E->tl=((struct Cyc_List_List*(*)(
struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp76E;});{struct Cyc_Absyn_Aggrdecl*
_tmp770=({struct Cyc_Absyn_Aggrdecl*_tmp774=_cycalloc(sizeof(*_tmp774));_tmp774->kind=(
void*)((void*)0);_tmp774->sc=(void*)((void*)2);_tmp774->name=Cyc_Toc_collapse_qvar_tag(
f->name,({const char*_tmp776="_struct";_tag_dyneither(_tmp776,sizeof(char),8);}));
_tmp774->tvs=0;_tmp774->impl=({struct Cyc_Absyn_AggrdeclImpl*_tmp775=_cycalloc(
sizeof(*_tmp775));_tmp775->exist_vars=0;_tmp775->rgn_po=0;_tmp775->fields=fields;
_tmp775;});_tmp774->attributes=0;_tmp774;});Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp771=_cycalloc(sizeof(*_tmp771));_tmp771->hd=Cyc_Absyn_new_decl((void*)({
struct Cyc_Absyn_Aggr_d_struct*_tmp772=_cycalloc(sizeof(*_tmp772));_tmp772[0]=({
struct Cyc_Absyn_Aggr_d_struct _tmp773;_tmp773.tag=4;_tmp773.f1=_tmp770;_tmp773;});
_tmp772;}),0);_tmp771->tl=Cyc_Toc_result_decls;_tmp771;});}}goto _LL380;}}_LL383:
if(_tmp758 == 0)goto _LL385;_tmp759=*_tmp758;if(_tmp759 != 0)goto _LL385;_LL384: if((
void*)f->sc != (void*)3){char zero='\000';struct Cyc_Absyn_Exp*_tmp777=Cyc_Absyn_string_exp((
struct _dyneither_ptr)({struct Cyc_String_pa_struct _tmp782;_tmp782.tag=0;_tmp782.f1=(
struct _dyneither_ptr)((struct _dyneither_ptr)*fn);{struct Cyc_Int_pa_struct _tmp781;
_tmp781.tag=1;_tmp781.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp780;_tmp780.tag=1;_tmp780.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp77F;_tmp77F.tag=1;_tmp77F.f1=(unsigned long)((int)zero);{struct Cyc_Int_pa_struct
_tmp77E;_tmp77E.tag=1;_tmp77E.f1=(unsigned long)((int)zero);{void*_tmp77C[5]={&
_tmp77E,& _tmp77F,& _tmp780,& _tmp781,& _tmp782};Cyc_aprintf(({const char*_tmp77D="%c%c%c%c%s";
_tag_dyneither(_tmp77D,sizeof(char),11);}),_tag_dyneither(_tmp77C,sizeof(void*),
5));}}}}}}),0);struct Cyc_Absyn_Vardecl*_tmp778=Cyc_Absyn_new_vardecl(f->name,
_tmp757,(struct Cyc_Absyn_Exp*)_tmp777);(void*)(_tmp778->sc=(void*)((void*)f->sc));
Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp779=_cycalloc(sizeof(*_tmp779));
_tmp779->hd=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_struct*_tmp77A=
_cycalloc(sizeof(*_tmp77A));_tmp77A[0]=({struct Cyc_Absyn_Var_d_struct _tmp77B;
_tmp77B.tag=0;_tmp77B.f1=_tmp778;_tmp77B;});_tmp77A;}),0);_tmp779->tl=Cyc_Toc_result_decls;
_tmp779;});*_tmp752=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*
k,int v))Cyc_Dict_insert)(*_tmp752,f->name,1);}goto _LL380;_LL385:;_LL386: goto
_LL380;_LL380:;}};_pop_dynregion(d);}}}static void Cyc_Toc_enumdecl_to_c(struct Cyc_Toc_Env*
nv,struct Cyc_Absyn_Enumdecl*ed){(void*)(ed->sc=(void*)((void*)2));if(ed->fields
!= 0)Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)
_check_null(ed->fields))->v);}static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*
body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*
s){void*old_typ=(void*)vd->type;(void*)(vd->type=(void*)Cyc_Toc_typ_to_c_array(
old_typ));if((void*)vd->sc == (void*)5  && Cyc_Tcutil_is_tagged_pointer_typ(
old_typ))(void*)(vd->sc=(void*)((void*)2));Cyc_Toc_stmt_to_c(body_nv,s);if(vd->initializer
!= 0){struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp783=(void*)init->r;struct Cyc_Absyn_Vardecl*_tmp784;struct Cyc_Absyn_Exp*
_tmp785;struct Cyc_Absyn_Exp*_tmp786;int _tmp787;_LL388: if(*((int*)_tmp783)!= 29)
goto _LL38A;_tmp784=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp783)->f1;
_tmp785=((struct Cyc_Absyn_Comprehension_e_struct*)_tmp783)->f2;_tmp786=((struct
Cyc_Absyn_Comprehension_e_struct*)_tmp783)->f3;_tmp787=((struct Cyc_Absyn_Comprehension_e_struct*)
_tmp783)->f4;_LL389: vd->initializer=0;(void*)(s->r=(void*)((void*)(Cyc_Toc_init_comprehension(
init_nv,Cyc_Absyn_var_exp(vd->name,0),_tmp784,_tmp785,_tmp786,_tmp787,Cyc_Absyn_new_stmt((
void*)s->r,0),0))->r));goto _LL387;_LL38A:;_LL38B: if((void*)vd->sc == (void*)0){
struct _RegionHandle _tmp788=_new_region("temp");struct _RegionHandle*temp=& _tmp788;
_push_region(temp);{struct Cyc_Toc_Env*_tmp789=Cyc_Toc_set_toplevel(temp,init_nv);
Cyc_Toc_exp_to_c(_tmp789,init);};_pop_region(temp);}else{Cyc_Toc_exp_to_c(
init_nv,init);}goto _LL387;_LL387:;}else{void*_tmp78A=Cyc_Tcutil_compress(old_typ);
struct Cyc_Absyn_ArrayInfo _tmp78B;void*_tmp78C;struct Cyc_Absyn_Exp*_tmp78D;struct
Cyc_Absyn_Conref*_tmp78E;_LL38D: if(_tmp78A <= (void*)4)goto _LL38F;if(*((int*)
_tmp78A)!= 7)goto _LL38F;_tmp78B=((struct Cyc_Absyn_ArrayType_struct*)_tmp78A)->f1;
_tmp78C=(void*)_tmp78B.elt_type;_tmp78D=_tmp78B.num_elts;_tmp78E=_tmp78B.zero_term;
if(!((int(*)(int,struct Cyc_Absyn_Conref*x))Cyc_Absyn_conref_def)(0,_tmp78E))goto
_LL38F;_LL38E: if(_tmp78D == 0)({void*_tmp78F=0;((int(*)(struct _dyneither_ptr fmt,
struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({const char*_tmp790="can't initialize zero-terminated array -- size unknown";
_tag_dyneither(_tmp790,sizeof(char),55);}),_tag_dyneither(_tmp78F,sizeof(void*),
0));});{struct Cyc_Absyn_Exp*num_elts=(struct Cyc_Absyn_Exp*)_tmp78D;struct Cyc_Absyn_Exp*
_tmp791=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),Cyc_Absyn_add_exp(
num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);struct Cyc_Absyn_Exp*_tmp792=Cyc_Absyn_signed_int_exp(
0,0);(void*)(s->r=(void*)Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(
_tmp791,_tmp792,0),0),Cyc_Absyn_new_stmt((void*)s->r,0)));goto _LL38C;}_LL38F:;
_LL390: goto _LL38C;_LL38C:;}}static struct Cyc_Absyn_Stmt**Cyc_Toc_throw_match_stmt_opt=
0;static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){if(Cyc_Toc_throw_match_stmt_opt
== 0)Cyc_Toc_throw_match_stmt_opt=({struct Cyc_Absyn_Stmt**_tmp793=_cycalloc(
sizeof(*_tmp793));_tmp793[0]=Cyc_Absyn_exp_stmt(Cyc_Toc_newthrow_exp(Cyc_Absyn_match_exn_exp(
0)),0);_tmp793;});return*((struct Cyc_Absyn_Stmt**)_check_null(Cyc_Toc_throw_match_stmt_opt));}
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*
p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();{struct _RegionHandle _tmp794=_new_region("prgn");
struct _RegionHandle*prgn=& _tmp794;_push_region(prgn);{struct Cyc_Absyn_Stmt*
_tmp795=Cyc_Toc_throw_match_stmt();struct Cyc_Toc_Env*_tmp796=Cyc_Toc_share_env(
prgn,nv);struct Cyc_Toc_Env*_tmp798;struct Cyc_List_List*_tmp799;struct Cyc_Absyn_Stmt*
_tmp79A;struct _tuple13 _tmp797=Cyc_Toc_xlate_pat(_tmp796,prgn,t,Cyc_Absyn_var_exp(
x,0),Cyc_Absyn_var_exp(x,0),p,(struct Cyc_Absyn_Stmt**)& _tmp795,Cyc_Toc_throw_match_stmt(),
0);_tmp798=_tmp797.f1;_tmp799=_tmp797.f2;_tmp79A=_tmp797.f3;Cyc_Toc_stmt_to_c(
_tmp798,s);s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),(struct Cyc_Absyn_Exp*)
e,Cyc_Absyn_seq_stmt(_tmp79A,s,0),0);for(0;_tmp799 != 0;_tmp799=_tmp799->tl){
struct _tuple14 _tmp79C;struct _tuple1*_tmp79D;void*_tmp79E;struct _tuple14*_tmp79B=(
struct _tuple14*)_tmp799->hd;_tmp79C=*_tmp79B;_tmp79D=_tmp79C.f1;_tmp79E=_tmp79C.f2;
s=Cyc_Absyn_declare_stmt(_tmp79D,_tmp79E,0,s,0);}};_pop_region(prgn);}return s;}}
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){void*_tmp79F=(void*)e->r;
struct Cyc_Absyn_Exp*_tmp7A0;struct Cyc_Absyn_Exp*_tmp7A1;struct Cyc_Absyn_Exp*
_tmp7A2;struct Cyc_Absyn_Exp*_tmp7A3;struct Cyc_Absyn_Exp*_tmp7A4;struct Cyc_Absyn_Exp*
_tmp7A5;struct Cyc_Absyn_Exp*_tmp7A6;struct Cyc_Absyn_Exp*_tmp7A7;struct Cyc_List_List*
_tmp7A8;struct Cyc_Absyn_Exp*_tmp7A9;struct Cyc_Absyn_Exp*_tmp7AA;struct Cyc_Absyn_Exp*
_tmp7AB;struct Cyc_Absyn_Exp*_tmp7AC;struct Cyc_Absyn_Exp*_tmp7AD;struct Cyc_Absyn_Exp*
_tmp7AE;struct Cyc_Absyn_Exp*_tmp7AF;struct Cyc_Absyn_Exp*_tmp7B0;struct Cyc_Absyn_Exp*
_tmp7B1;struct Cyc_Absyn_Exp*_tmp7B2;struct Cyc_Absyn_Exp*_tmp7B3;struct Cyc_Absyn_Exp*
_tmp7B4;struct Cyc_Absyn_Exp*_tmp7B5;struct Cyc_Absyn_Exp*_tmp7B6;struct Cyc_Absyn_Exp*
_tmp7B7;struct Cyc_Absyn_Exp*_tmp7B8;struct Cyc_List_List*_tmp7B9;struct Cyc_Absyn_Exp*
_tmp7BA;struct Cyc_List_List*_tmp7BB;void*_tmp7BC;void**_tmp7BD;struct Cyc_Absyn_Exp*
_tmp7BE;struct _tuple2*_tmp7BF;struct _tuple2 _tmp7C0;void*_tmp7C1;void**_tmp7C2;
struct Cyc_List_List*_tmp7C3;struct Cyc_List_List*_tmp7C4;struct Cyc_List_List*
_tmp7C5;void*_tmp7C6;void**_tmp7C7;void*_tmp7C8;void**_tmp7C9;struct Cyc_Absyn_Stmt*
_tmp7CA;struct Cyc_Absyn_MallocInfo _tmp7CB;struct Cyc_Absyn_MallocInfo*_tmp7CC;
_LL392: if(*((int*)_tmp79F)!= 22)goto _LL394;_tmp7A0=((struct Cyc_Absyn_Deref_e_struct*)
_tmp79F)->f1;_LL393: _tmp7A1=_tmp7A0;goto _LL395;_LL394: if(*((int*)_tmp79F)!= 23)
goto _LL396;_tmp7A1=((struct Cyc_Absyn_AggrMember_e_struct*)_tmp79F)->f1;_LL395:
_tmp7A2=_tmp7A1;goto _LL397;_LL396: if(*((int*)_tmp79F)!= 24)goto _LL398;_tmp7A2=((
struct Cyc_Absyn_AggrArrow_e_struct*)_tmp79F)->f1;_LL397: _tmp7A3=_tmp7A2;goto
_LL399;_LL398: if(*((int*)_tmp79F)!= 16)goto _LL39A;_tmp7A3=((struct Cyc_Absyn_Address_e_struct*)
_tmp79F)->f1;_LL399: _tmp7A4=_tmp7A3;goto _LL39B;_LL39A: if(*((int*)_tmp79F)!= 12)
goto _LL39C;_tmp7A4=((struct Cyc_Absyn_Throw_e_struct*)_tmp79F)->f1;_LL39B: _tmp7A5=
_tmp7A4;goto _LL39D;_LL39C: if(*((int*)_tmp79F)!= 13)goto _LL39E;_tmp7A5=((struct
Cyc_Absyn_NoInstantiate_e_struct*)_tmp79F)->f1;_LL39D: _tmp7A6=_tmp7A5;goto _LL39F;
_LL39E: if(*((int*)_tmp79F)!= 19)goto _LL3A0;_tmp7A6=((struct Cyc_Absyn_Sizeofexp_e_struct*)
_tmp79F)->f1;_LL39F: _tmp7A7=_tmp7A6;goto _LL3A1;_LL3A0: if(*((int*)_tmp79F)!= 5)
goto _LL3A2;_tmp7A7=((struct Cyc_Absyn_Increment_e_struct*)_tmp79F)->f1;_LL3A1: Cyc_Toc_exptypes_to_c(
_tmp7A7);goto _LL391;_LL3A2: if(*((int*)_tmp79F)!= 3)goto _LL3A4;_tmp7A8=((struct
Cyc_Absyn_Primop_e_struct*)_tmp79F)->f2;_LL3A3:((void(*)(void(*f)(struct Cyc_Absyn_Exp*),
struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp7A8);goto _LL391;
_LL3A4: if(*((int*)_tmp79F)!= 7)goto _LL3A6;_tmp7A9=((struct Cyc_Absyn_And_e_struct*)
_tmp79F)->f1;_tmp7AA=((struct Cyc_Absyn_And_e_struct*)_tmp79F)->f2;_LL3A5: _tmp7AB=
_tmp7A9;_tmp7AC=_tmp7AA;goto _LL3A7;_LL3A6: if(*((int*)_tmp79F)!= 8)goto _LL3A8;
_tmp7AB=((struct Cyc_Absyn_Or_e_struct*)_tmp79F)->f1;_tmp7AC=((struct Cyc_Absyn_Or_e_struct*)
_tmp79F)->f2;_LL3A7: _tmp7AD=_tmp7AB;_tmp7AE=_tmp7AC;goto _LL3A9;_LL3A8: if(*((int*)
_tmp79F)!= 9)goto _LL3AA;_tmp7AD=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp79F)->f1;
_tmp7AE=((struct Cyc_Absyn_SeqExp_e_struct*)_tmp79F)->f2;_LL3A9: _tmp7AF=_tmp7AD;
_tmp7B0=_tmp7AE;goto _LL3AB;_LL3AA: if(*((int*)_tmp79F)!= 25)goto _LL3AC;_tmp7AF=((
struct Cyc_Absyn_Subscript_e_struct*)_tmp79F)->f1;_tmp7B0=((struct Cyc_Absyn_Subscript_e_struct*)
_tmp79F)->f2;_LL3AB: _tmp7B1=_tmp7AF;_tmp7B2=_tmp7B0;goto _LL3AD;_LL3AC: if(*((int*)
_tmp79F)!= 36)goto _LL3AE;_tmp7B1=((struct Cyc_Absyn_Swap_e_struct*)_tmp79F)->f1;
_tmp7B2=((struct Cyc_Absyn_Swap_e_struct*)_tmp79F)->f2;_LL3AD: _tmp7B3=_tmp7B1;
_tmp7B4=_tmp7B2;goto _LL3AF;_LL3AE: if(*((int*)_tmp79F)!= 4)goto _LL3B0;_tmp7B3=((
struct Cyc_Absyn_AssignOp_e_struct*)_tmp79F)->f1;_tmp7B4=((struct Cyc_Absyn_AssignOp_e_struct*)
_tmp79F)->f3;_LL3AF: Cyc_Toc_exptypes_to_c(_tmp7B3);Cyc_Toc_exptypes_to_c(_tmp7B4);
goto _LL391;_LL3B0: if(*((int*)_tmp79F)!= 6)goto _LL3B2;_tmp7B5=((struct Cyc_Absyn_Conditional_e_struct*)
_tmp79F)->f1;_tmp7B6=((struct Cyc_Absyn_Conditional_e_struct*)_tmp79F)->f2;
_tmp7B7=((struct Cyc_Absyn_Conditional_e_struct*)_tmp79F)->f3;_LL3B1: Cyc_Toc_exptypes_to_c(
_tmp7B5);Cyc_Toc_exptypes_to_c(_tmp7B6);Cyc_Toc_exptypes_to_c(_tmp7B7);goto
_LL391;_LL3B2: if(*((int*)_tmp79F)!= 11)goto _LL3B4;_tmp7B8=((struct Cyc_Absyn_FnCall_e_struct*)
_tmp79F)->f1;_tmp7B9=((struct Cyc_Absyn_FnCall_e_struct*)_tmp79F)->f2;_LL3B3:
_tmp7BA=_tmp7B8;_tmp7BB=_tmp7B9;goto _LL3B5;_LL3B4: if(*((int*)_tmp79F)!= 10)goto
_LL3B6;_tmp7BA=((struct Cyc_Absyn_UnknownCall_e_struct*)_tmp79F)->f1;_tmp7BB=((
struct Cyc_Absyn_UnknownCall_e_struct*)_tmp79F)->f2;_LL3B5: Cyc_Toc_exptypes_to_c(
_tmp7BA);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(
Cyc_Toc_exptypes_to_c,_tmp7BB);goto _LL391;_LL3B6: if(*((int*)_tmp79F)!= 15)goto
_LL3B8;_tmp7BC=(void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp79F)->f1;_tmp7BD=(
void**)&((void*)((struct Cyc_Absyn_Cast_e_struct*)_tmp79F)->f1);_tmp7BE=((struct
Cyc_Absyn_Cast_e_struct*)_tmp79F)->f2;_LL3B7:*_tmp7BD=Cyc_Toc_typ_to_c(*_tmp7BD);
Cyc_Toc_exptypes_to_c(_tmp7BE);goto _LL391;_LL3B8: if(*((int*)_tmp79F)!= 27)goto
_LL3BA;_tmp7BF=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp79F)->f1;_tmp7C0=*
_tmp7BF;_tmp7C1=_tmp7C0.f3;_tmp7C2=(void**)&(*((struct Cyc_Absyn_CompoundLit_e_struct*)
_tmp79F)->f1).f3;_tmp7C3=((struct Cyc_Absyn_CompoundLit_e_struct*)_tmp79F)->f2;
_LL3B9:*_tmp7C2=Cyc_Toc_typ_to_c(*_tmp7C2);_tmp7C4=_tmp7C3;goto _LL3BB;_LL3BA: if(*((
int*)_tmp79F)!= 37)goto _LL3BC;_tmp7C4=((struct Cyc_Absyn_UnresolvedMem_e_struct*)
_tmp79F)->f2;_LL3BB: _tmp7C5=_tmp7C4;goto _LL3BD;_LL3BC: if(*((int*)_tmp79F)!= 28)
goto _LL3BE;_tmp7C5=((struct Cyc_Absyn_Array_e_struct*)_tmp79F)->f1;_LL3BD: for(0;
_tmp7C5 != 0;_tmp7C5=_tmp7C5->tl){struct Cyc_Absyn_Exp*_tmp7CE;struct _tuple8
_tmp7CD=*((struct _tuple8*)_tmp7C5->hd);_tmp7CE=_tmp7CD.f2;Cyc_Toc_exptypes_to_c(
_tmp7CE);}goto _LL391;_LL3BE: if(*((int*)_tmp79F)!= 20)goto _LL3C0;_tmp7C6=(void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp79F)->f1;_tmp7C7=(void**)&((void*)((
struct Cyc_Absyn_Offsetof_e_struct*)_tmp79F)->f1);_LL3BF: _tmp7C9=_tmp7C7;goto
_LL3C1;_LL3C0: if(*((int*)_tmp79F)!= 18)goto _LL3C2;_tmp7C8=(void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp79F)->f1;_tmp7C9=(void**)&((void*)((struct Cyc_Absyn_Sizeoftyp_e_struct*)
_tmp79F)->f1);_LL3C1:*_tmp7C9=Cyc_Toc_typ_to_c(*_tmp7C9);goto _LL391;_LL3C2: if(*((
int*)_tmp79F)!= 38)goto _LL3C4;_tmp7CA=((struct Cyc_Absyn_StmtExp_e_struct*)
_tmp79F)->f1;_LL3C3: Cyc_Toc_stmttypes_to_c(_tmp7CA);goto _LL391;_LL3C4: if(*((int*)
_tmp79F)!= 35)goto _LL3C6;_tmp7CB=((struct Cyc_Absyn_Malloc_e_struct*)_tmp79F)->f1;
_tmp7CC=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_struct*)_tmp79F)->f1;
_LL3C5: if(_tmp7CC->elt_type != 0)_tmp7CC->elt_type=({void**_tmp7CF=_cycalloc(
sizeof(*_tmp7CF));_tmp7CF[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp7CC->elt_type)));
_tmp7CF;});Cyc_Toc_exptypes_to_c(_tmp7CC->num_elts);goto _LL391;_LL3C6: if(*((int*)
_tmp79F)!= 0)goto _LL3C8;_LL3C7: goto _LL3C9;_LL3C8: if(*((int*)_tmp79F)!= 1)goto
_LL3CA;_LL3C9: goto _LL3CB;_LL3CA: if(*((int*)_tmp79F)!= 2)goto _LL3CC;_LL3CB: goto
_LL3CD;_LL3CC: if(*((int*)_tmp79F)!= 33)goto _LL3CE;_LL3CD: goto _LL3CF;_LL3CE: if(*((
int*)_tmp79F)!= 34)goto _LL3D0;_LL3CF: goto _LL391;_LL3D0: if(*((int*)_tmp79F)!= 31)
goto _LL3D2;_LL3D1: goto _LL3D3;_LL3D2: if(*((int*)_tmp79F)!= 32)goto _LL3D4;_LL3D3:
goto _LL3D5;_LL3D4: if(*((int*)_tmp79F)!= 30)goto _LL3D6;_LL3D5: goto _LL3D7;_LL3D6:
if(*((int*)_tmp79F)!= 29)goto _LL3D8;_LL3D7: goto _LL3D9;_LL3D8: if(*((int*)_tmp79F)
!= 26)goto _LL3DA;_LL3D9: goto _LL3DB;_LL3DA: if(*((int*)_tmp79F)!= 14)goto _LL3DC;
_LL3DB: goto _LL3DD;_LL3DC: if(*((int*)_tmp79F)!= 17)goto _LL3DE;_LL3DD: goto _LL3DF;
_LL3DE: if(*((int*)_tmp79F)!= 39)goto _LL3E0;_LL3DF: goto _LL3E1;_LL3E0: if(*((int*)
_tmp79F)!= 21)goto _LL391;_LL3E1:({void*_tmp7D0=0;Cyc_Tcutil_terr(e->loc,({const
char*_tmp7D1="Cyclone expression within C code";_tag_dyneither(_tmp7D1,sizeof(
char),33);}),_tag_dyneither(_tmp7D0,sizeof(void*),0));});goto _LL391;_LL391:;}
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){void*_tmp7D2=(void*)d->r;
struct Cyc_Absyn_Vardecl*_tmp7D3;struct Cyc_Absyn_Fndecl*_tmp7D4;struct Cyc_Absyn_Aggrdecl*
_tmp7D5;struct Cyc_Absyn_Enumdecl*_tmp7D6;struct Cyc_Absyn_Typedefdecl*_tmp7D7;
_LL3E3: if(_tmp7D2 <= (void*)2)goto _LL3FB;if(*((int*)_tmp7D2)!= 0)goto _LL3E5;
_tmp7D3=((struct Cyc_Absyn_Var_d_struct*)_tmp7D2)->f1;_LL3E4:(void*)(_tmp7D3->type=(
void*)Cyc_Toc_typ_to_c((void*)_tmp7D3->type));if(_tmp7D3->initializer != 0)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_check_null(_tmp7D3->initializer));goto _LL3E2;_LL3E5: if(*((
int*)_tmp7D2)!= 1)goto _LL3E7;_tmp7D4=((struct Cyc_Absyn_Fn_d_struct*)_tmp7D2)->f1;
_LL3E6:(void*)(_tmp7D4->ret_type=(void*)Cyc_Toc_typ_to_c((void*)_tmp7D4->ret_type));{
struct Cyc_List_List*_tmp7D8=_tmp7D4->args;for(0;_tmp7D8 != 0;_tmp7D8=_tmp7D8->tl){(*((
struct _tuple17*)_tmp7D8->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple17*)_tmp7D8->hd)).f3);}}
goto _LL3E2;_LL3E7: if(*((int*)_tmp7D2)!= 4)goto _LL3E9;_tmp7D5=((struct Cyc_Absyn_Aggr_d_struct*)
_tmp7D2)->f1;_LL3E8: Cyc_Toc_aggrdecl_to_c(_tmp7D5);goto _LL3E2;_LL3E9: if(*((int*)
_tmp7D2)!= 6)goto _LL3EB;_tmp7D6=((struct Cyc_Absyn_Enum_d_struct*)_tmp7D2)->f1;
_LL3EA: if(_tmp7D6->fields != 0){struct Cyc_List_List*_tmp7D9=(struct Cyc_List_List*)((
struct Cyc_Core_Opt*)_check_null(_tmp7D6->fields))->v;for(0;_tmp7D9 != 0;_tmp7D9=
_tmp7D9->tl){struct Cyc_Absyn_Enumfield*_tmp7DA=(struct Cyc_Absyn_Enumfield*)
_tmp7D9->hd;if(_tmp7DA->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)
_check_null(_tmp7DA->tag));}}goto _LL3E2;_LL3EB: if(*((int*)_tmp7D2)!= 7)goto
_LL3ED;_tmp7D7=((struct Cyc_Absyn_Typedef_d_struct*)_tmp7D2)->f1;_LL3EC: _tmp7D7->defn=({
struct Cyc_Core_Opt*_tmp7DB=_cycalloc(sizeof(*_tmp7DB));_tmp7DB->v=(void*)Cyc_Toc_typ_to_c_array((
void*)((struct Cyc_Core_Opt*)_check_null(_tmp7D7->defn))->v);_tmp7DB;});goto
_LL3E2;_LL3ED: if(*((int*)_tmp7D2)!= 2)goto _LL3EF;_LL3EE: goto _LL3F0;_LL3EF: if(*((
int*)_tmp7D2)!= 3)goto _LL3F1;_LL3F0: goto _LL3F2;_LL3F1: if(*((int*)_tmp7D2)!= 5)
goto _LL3F3;_LL3F2: goto _LL3F4;_LL3F3: if(*((int*)_tmp7D2)!= 8)goto _LL3F5;_LL3F4:
goto _LL3F6;_LL3F5: if(*((int*)_tmp7D2)!= 9)goto _LL3F7;_LL3F6: goto _LL3F8;_LL3F7:
if(*((int*)_tmp7D2)!= 10)goto _LL3F9;_LL3F8: goto _LL3FA;_LL3F9: if(*((int*)_tmp7D2)
!= 11)goto _LL3FB;_LL3FA:({void*_tmp7DC=0;Cyc_Tcutil_terr(d->loc,({const char*
_tmp7DD="Cyclone declaration within C code";_tag_dyneither(_tmp7DD,sizeof(char),
34);}),_tag_dyneither(_tmp7DC,sizeof(void*),0));});goto _LL3E2;_LL3FB: if((int)
_tmp7D2 != 0)goto _LL3FD;_LL3FC: goto _LL3FE;_LL3FD: if((int)_tmp7D2 != 1)goto _LL3E2;
_LL3FE: goto _LL3E2;_LL3E2:;}static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*
s){void*_tmp7DE=(void*)s->r;struct Cyc_Absyn_Exp*_tmp7DF;struct Cyc_Absyn_Stmt*
_tmp7E0;struct Cyc_Absyn_Stmt*_tmp7E1;struct Cyc_Absyn_Exp*_tmp7E2;struct Cyc_Absyn_Exp*
_tmp7E3;struct Cyc_Absyn_Stmt*_tmp7E4;struct Cyc_Absyn_Stmt*_tmp7E5;struct _tuple3
_tmp7E6;struct Cyc_Absyn_Exp*_tmp7E7;struct Cyc_Absyn_Stmt*_tmp7E8;struct Cyc_Absyn_Exp*
_tmp7E9;struct _tuple3 _tmp7EA;struct Cyc_Absyn_Exp*_tmp7EB;struct _tuple3 _tmp7EC;
struct Cyc_Absyn_Exp*_tmp7ED;struct Cyc_Absyn_Stmt*_tmp7EE;struct Cyc_Absyn_Exp*
_tmp7EF;struct Cyc_List_List*_tmp7F0;struct Cyc_Absyn_Decl*_tmp7F1;struct Cyc_Absyn_Stmt*
_tmp7F2;struct Cyc_Absyn_Stmt*_tmp7F3;struct _tuple3 _tmp7F4;struct Cyc_Absyn_Exp*
_tmp7F5;_LL400: if(_tmp7DE <= (void*)1)goto _LL412;if(*((int*)_tmp7DE)!= 0)goto
_LL402;_tmp7DF=((struct Cyc_Absyn_Exp_s_struct*)_tmp7DE)->f1;_LL401: Cyc_Toc_exptypes_to_c(
_tmp7DF);goto _LL3FF;_LL402: if(*((int*)_tmp7DE)!= 1)goto _LL404;_tmp7E0=((struct
Cyc_Absyn_Seq_s_struct*)_tmp7DE)->f1;_tmp7E1=((struct Cyc_Absyn_Seq_s_struct*)
_tmp7DE)->f2;_LL403: Cyc_Toc_stmttypes_to_c(_tmp7E0);Cyc_Toc_stmttypes_to_c(
_tmp7E1);goto _LL3FF;_LL404: if(*((int*)_tmp7DE)!= 2)goto _LL406;_tmp7E2=((struct
Cyc_Absyn_Return_s_struct*)_tmp7DE)->f1;_LL405: if(_tmp7E2 != 0)Cyc_Toc_exptypes_to_c((
struct Cyc_Absyn_Exp*)_tmp7E2);goto _LL3FF;_LL406: if(*((int*)_tmp7DE)!= 3)goto
_LL408;_tmp7E3=((struct Cyc_Absyn_IfThenElse_s_struct*)_tmp7DE)->f1;_tmp7E4=((
struct Cyc_Absyn_IfThenElse_s_struct*)_tmp7DE)->f2;_tmp7E5=((struct Cyc_Absyn_IfThenElse_s_struct*)
_tmp7DE)->f3;_LL407: Cyc_Toc_exptypes_to_c(_tmp7E3);Cyc_Toc_stmttypes_to_c(
_tmp7E4);Cyc_Toc_stmttypes_to_c(_tmp7E5);goto _LL3FF;_LL408: if(*((int*)_tmp7DE)!= 
4)goto _LL40A;_tmp7E6=((struct Cyc_Absyn_While_s_struct*)_tmp7DE)->f1;_tmp7E7=
_tmp7E6.f1;_tmp7E8=((struct Cyc_Absyn_While_s_struct*)_tmp7DE)->f2;_LL409: Cyc_Toc_exptypes_to_c(
_tmp7E7);Cyc_Toc_stmttypes_to_c(_tmp7E8);goto _LL3FF;_LL40A: if(*((int*)_tmp7DE)!= 
8)goto _LL40C;_tmp7E9=((struct Cyc_Absyn_For_s_struct*)_tmp7DE)->f1;_tmp7EA=((
struct Cyc_Absyn_For_s_struct*)_tmp7DE)->f2;_tmp7EB=_tmp7EA.f1;_tmp7EC=((struct
Cyc_Absyn_For_s_struct*)_tmp7DE)->f3;_tmp7ED=_tmp7EC.f1;_tmp7EE=((struct Cyc_Absyn_For_s_struct*)
_tmp7DE)->f4;_LL40B: Cyc_Toc_exptypes_to_c(_tmp7E9);Cyc_Toc_exptypes_to_c(_tmp7EB);
Cyc_Toc_exptypes_to_c(_tmp7ED);Cyc_Toc_stmttypes_to_c(_tmp7EE);goto _LL3FF;_LL40C:
if(*((int*)_tmp7DE)!= 9)goto _LL40E;_tmp7EF=((struct Cyc_Absyn_Switch_s_struct*)
_tmp7DE)->f1;_tmp7F0=((struct Cyc_Absyn_Switch_s_struct*)_tmp7DE)->f2;_LL40D: Cyc_Toc_exptypes_to_c(
_tmp7EF);for(0;_tmp7F0 != 0;_tmp7F0=_tmp7F0->tl){Cyc_Toc_stmttypes_to_c(((struct
Cyc_Absyn_Switch_clause*)_tmp7F0->hd)->body);}goto _LL3FF;_LL40E: if(*((int*)
_tmp7DE)!= 11)goto _LL410;_tmp7F1=((struct Cyc_Absyn_Decl_s_struct*)_tmp7DE)->f1;
_tmp7F2=((struct Cyc_Absyn_Decl_s_struct*)_tmp7DE)->f2;_LL40F: Cyc_Toc_decltypes_to_c(
_tmp7F1);Cyc_Toc_stmttypes_to_c(_tmp7F2);goto _LL3FF;_LL410: if(*((int*)_tmp7DE)!= 
13)goto _LL412;_tmp7F3=((struct Cyc_Absyn_Do_s_struct*)_tmp7DE)->f1;_tmp7F4=((
struct Cyc_Absyn_Do_s_struct*)_tmp7DE)->f2;_tmp7F5=_tmp7F4.f1;_LL411: Cyc_Toc_stmttypes_to_c(
_tmp7F3);Cyc_Toc_exptypes_to_c(_tmp7F5);goto _LL3FF;_LL412: if((int)_tmp7DE != 0)
goto _LL414;_LL413: goto _LL415;_LL414: if(_tmp7DE <= (void*)1)goto _LL416;if(*((int*)
_tmp7DE)!= 5)goto _LL416;_LL415: goto _LL417;_LL416: if(_tmp7DE <= (void*)1)goto
_LL418;if(*((int*)_tmp7DE)!= 6)goto _LL418;_LL417: goto _LL419;_LL418: if(_tmp7DE <= (
void*)1)goto _LL41A;if(*((int*)_tmp7DE)!= 7)goto _LL41A;_LL419: goto _LL3FF;_LL41A:
if(_tmp7DE <= (void*)1)goto _LL41C;if(*((int*)_tmp7DE)!= 10)goto _LL41C;_LL41B: goto
_LL41D;_LL41C: if(_tmp7DE <= (void*)1)goto _LL41E;if(*((int*)_tmp7DE)!= 12)goto
_LL41E;_LL41D: goto _LL41F;_LL41E: if(_tmp7DE <= (void*)1)goto _LL420;if(*((int*)
_tmp7DE)!= 14)goto _LL420;_LL41F: goto _LL421;_LL420: if(_tmp7DE <= (void*)1)goto
_LL422;if(*((int*)_tmp7DE)!= 15)goto _LL422;_LL421: goto _LL423;_LL422: if(_tmp7DE <= (
void*)1)goto _LL424;if(*((int*)_tmp7DE)!= 17)goto _LL424;_LL423: goto _LL425;_LL424:
if(_tmp7DE <= (void*)1)goto _LL3FF;if(*((int*)_tmp7DE)!= 16)goto _LL3FF;_LL425:({
void*_tmp7F6=0;Cyc_Tcutil_terr(s->loc,({const char*_tmp7F7="Cyclone statement in C code";
_tag_dyneither(_tmp7F7,sizeof(char),28);}),_tag_dyneither(_tmp7F6,sizeof(void*),
0));});goto _LL3FF;_LL3FF:;}static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct
_RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){if(!Cyc_Toc_is_toplevel(nv))({void*_tmp7F8=0;((int(*)(
struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*
_tmp7F9="decls_to_c: not at toplevel!";_tag_dyneither(_tmp7F9,sizeof(char),29);}),
_tag_dyneither(_tmp7F8,sizeof(void*),0));});{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)
ds->hd;void*_tmp7FA=(void*)d->r;struct Cyc_Absyn_Vardecl*_tmp7FB;struct Cyc_Absyn_Fndecl*
_tmp7FC;struct Cyc_Absyn_Aggrdecl*_tmp7FD;struct Cyc_Absyn_Tuniondecl*_tmp7FE;
struct Cyc_Absyn_Enumdecl*_tmp7FF;struct Cyc_Absyn_Typedefdecl*_tmp800;struct Cyc_List_List*
_tmp801;struct Cyc_List_List*_tmp802;struct Cyc_List_List*_tmp803;struct Cyc_List_List*
_tmp804;_LL427: if(_tmp7FA <= (void*)2)goto _LL437;if(*((int*)_tmp7FA)!= 0)goto
_LL429;_tmp7FB=((struct Cyc_Absyn_Var_d_struct*)_tmp7FA)->f1;_LL428: {struct
_tuple1*_tmp805=_tmp7FB->name;if((void*)_tmp7FB->sc == (void*)4)_tmp805=({struct
_tuple1*_tmp806=_cycalloc(sizeof(*_tmp806));_tmp806->f1=(union Cyc_Absyn_Nmspace_union)({
union Cyc_Absyn_Nmspace_union _tmp807;(_tmp807.Rel_n).tag=1;(_tmp807.Rel_n).f1=0;
_tmp807;});_tmp806->f2=(*_tmp805).f2;_tmp806;});if(_tmp7FB->initializer != 0){if(
cinclude)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp7FB->initializer));
else{Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp7FB->initializer));}}
nv=Cyc_Toc_add_varmap(r,nv,_tmp7FB->name,Cyc_Absyn_varb_exp(_tmp805,(void*)({
struct Cyc_Absyn_Global_b_struct*_tmp808=_cycalloc(sizeof(*_tmp808));_tmp808[0]=({
struct Cyc_Absyn_Global_b_struct _tmp809;_tmp809.tag=0;_tmp809.f1=_tmp7FB;_tmp809;});
_tmp808;}),0));_tmp7FB->name=_tmp805;(void*)(_tmp7FB->sc=(void*)Cyc_Toc_scope_to_c((
void*)_tmp7FB->sc));(void*)(_tmp7FB->type=(void*)Cyc_Toc_typ_to_c_array((void*)
_tmp7FB->type));Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp80A=_cycalloc(
sizeof(*_tmp80A));_tmp80A->hd=d;_tmp80A->tl=Cyc_Toc_result_decls;_tmp80A;});goto
_LL426;}_LL429: if(*((int*)_tmp7FA)!= 1)goto _LL42B;_tmp7FC=((struct Cyc_Absyn_Fn_d_struct*)
_tmp7FA)->f1;_LL42A: {struct _tuple1*_tmp80B=_tmp7FC->name;if((void*)_tmp7FC->sc
== (void*)4){_tmp80B=({struct _tuple1*_tmp80C=_cycalloc(sizeof(*_tmp80C));_tmp80C->f1=(
union Cyc_Absyn_Nmspace_union)({union Cyc_Absyn_Nmspace_union _tmp80D;(_tmp80D.Rel_n).tag=
1;(_tmp80D.Rel_n).f1=0;_tmp80D;});_tmp80C->f2=(*_tmp80B).f2;_tmp80C;});(void*)(
_tmp7FC->sc=(void*)((void*)2));}nv=Cyc_Toc_add_varmap(r,nv,_tmp7FC->name,Cyc_Absyn_var_exp(
_tmp80B,0));_tmp7FC->name=_tmp80B;Cyc_Toc_fndecl_to_c(nv,_tmp7FC,cinclude);Cyc_Toc_result_decls=({
struct Cyc_List_List*_tmp80E=_cycalloc(sizeof(*_tmp80E));_tmp80E->hd=d;_tmp80E->tl=
Cyc_Toc_result_decls;_tmp80E;});goto _LL426;}_LL42B: if(*((int*)_tmp7FA)!= 2)goto
_LL42D;_LL42C: goto _LL42E;_LL42D: if(*((int*)_tmp7FA)!= 3)goto _LL42F;_LL42E:({void*
_tmp80F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(({
const char*_tmp810="letdecl at toplevel";_tag_dyneither(_tmp810,sizeof(char),20);}),
_tag_dyneither(_tmp80F,sizeof(void*),0));});_LL42F: if(*((int*)_tmp7FA)!= 4)goto
_LL431;_tmp7FD=((struct Cyc_Absyn_Aggr_d_struct*)_tmp7FA)->f1;_LL430: Cyc_Toc_aggrdecl_to_c(
_tmp7FD);Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp811=_cycalloc(sizeof(*
_tmp811));_tmp811->hd=d;_tmp811->tl=Cyc_Toc_result_decls;_tmp811;});goto _LL426;
_LL431: if(*((int*)_tmp7FA)!= 5)goto _LL433;_tmp7FE=((struct Cyc_Absyn_Tunion_d_struct*)
_tmp7FA)->f1;_LL432: if(_tmp7FE->is_xtunion)Cyc_Toc_xtuniondecl_to_c(_tmp7FE);
else{Cyc_Toc_tuniondecl_to_c(_tmp7FE);}goto _LL426;_LL433: if(*((int*)_tmp7FA)!= 6)
goto _LL435;_tmp7FF=((struct Cyc_Absyn_Enum_d_struct*)_tmp7FA)->f1;_LL434: Cyc_Toc_enumdecl_to_c(
nv,_tmp7FF);Cyc_Toc_result_decls=({struct Cyc_List_List*_tmp812=_cycalloc(sizeof(*
_tmp812));_tmp812->hd=d;_tmp812->tl=Cyc_Toc_result_decls;_tmp812;});goto _LL426;
_LL435: if(*((int*)_tmp7FA)!= 7)goto _LL437;_tmp800=((struct Cyc_Absyn_Typedef_d_struct*)
_tmp7FA)->f1;_LL436: _tmp800->name=_tmp800->name;_tmp800->tvs=0;if(_tmp800->defn
!= 0)_tmp800->defn=({struct Cyc_Core_Opt*_tmp813=_cycalloc(sizeof(*_tmp813));
_tmp813->v=(void*)Cyc_Toc_typ_to_c_array((void*)((struct Cyc_Core_Opt*)
_check_null(_tmp800->defn))->v);_tmp813;});else{void*_tmp814=(void*)((struct Cyc_Core_Opt*)
_check_null(_tmp800->kind))->v;_LL444: if((int)_tmp814 != 2)goto _LL446;_LL445:
_tmp800->defn=({struct Cyc_Core_Opt*_tmp815=_cycalloc(sizeof(*_tmp815));_tmp815->v=(
void*)Cyc_Absyn_void_star_typ();_tmp815;});goto _LL443;_LL446:;_LL447: _tmp800->defn=({
struct Cyc_Core_Opt*_tmp816=_cycalloc(sizeof(*_tmp816));_tmp816->v=(void*)((void*)
0);_tmp816;});goto _LL443;_LL443:;}Cyc_Toc_result_decls=({struct Cyc_List_List*
_tmp817=_cycalloc(sizeof(*_tmp817));_tmp817->hd=d;_tmp817->tl=Cyc_Toc_result_decls;
_tmp817;});goto _LL426;_LL437: if((int)_tmp7FA != 0)goto _LL439;_LL438: goto _LL43A;
_LL439: if((int)_tmp7FA != 1)goto _LL43B;_LL43A: goto _LL426;_LL43B: if(_tmp7FA <= (
void*)2)goto _LL43D;if(*((int*)_tmp7FA)!= 8)goto _LL43D;_tmp801=((struct Cyc_Absyn_Namespace_d_struct*)
_tmp7FA)->f2;_LL43C: _tmp802=_tmp801;goto _LL43E;_LL43D: if(_tmp7FA <= (void*)2)goto
_LL43F;if(*((int*)_tmp7FA)!= 9)goto _LL43F;_tmp802=((struct Cyc_Absyn_Using_d_struct*)
_tmp7FA)->f2;_LL43E: _tmp803=_tmp802;goto _LL440;_LL43F: if(_tmp7FA <= (void*)2)goto
_LL441;if(*((int*)_tmp7FA)!= 10)goto _LL441;_tmp803=((struct Cyc_Absyn_ExternC_d_struct*)
_tmp7FA)->f1;_LL440: nv=Cyc_Toc_decls_to_c(r,nv,_tmp803,top,cinclude);goto _LL426;
_LL441: if(_tmp7FA <= (void*)2)goto _LL426;if(*((int*)_tmp7FA)!= 11)goto _LL426;
_tmp804=((struct Cyc_Absyn_ExternCinclude_d_struct*)_tmp7FA)->f1;_LL442: nv=Cyc_Toc_decls_to_c(
r,nv,_tmp804,top,1);goto _LL426;_LL426:;}}return nv;}static void Cyc_Toc_init(){
struct _DynRegionHandle*_tmp819;struct Cyc_Core_NewRegion _tmp818=Cyc_Core_new_dynregion();
_tmp819=_tmp818.dynregion;{struct _DynRegionFrame _tmp81A;struct _RegionHandle*d=
_open_dynregion(& _tmp81A,_tmp819);Cyc_Toc_toc_state=({struct Cyc_Toc_TocState*
_tmp81B=_cycalloc(sizeof(*_tmp81B));_tmp81B->dyn=(struct _DynRegionHandle*)
_tmp819;_tmp81B->tuple_types=(struct Cyc_List_List**)({struct Cyc_List_List**
_tmp820=_region_malloc(d,sizeof(*_tmp820));_tmp820[0]=0;_tmp820;});_tmp81B->aggrs_so_far=(
struct Cyc_Dict_Dict*)({struct Cyc_Dict_Dict*_tmp81F=_region_malloc(d,sizeof(*
_tmp81F));_tmp81F[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(
struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);_tmp81F;});
_tmp81B->tunions_so_far=(struct Cyc_Set_Set**)({struct Cyc_Set_Set**_tmp81E=
_region_malloc(d,sizeof(*_tmp81E));_tmp81E[0]=((struct Cyc_Set_Set*(*)(struct
_RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);
_tmp81E;});_tmp81B->xtunions_so_far=(struct Cyc_Dict_Dict*)({struct Cyc_Dict_Dict*
_tmp81D=_region_malloc(d,sizeof(*_tmp81D));_tmp81D[0]=((struct Cyc_Dict_Dict(*)(
struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(
d,Cyc_Absyn_qvar_cmp);_tmp81D;});_tmp81B->qvar_tags=(struct Cyc_Dict_Dict*)({
struct Cyc_Dict_Dict*_tmp81C=_region_malloc(d,sizeof(*_tmp81C));_tmp81C[0]=((
struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple6*,struct
_tuple6*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);_tmp81C;});_tmp81B->temp_labels=(
struct Cyc_Xarray_Xarray*)((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(
d);_tmp81B;});;_pop_dynregion(d);}Cyc_Toc_result_decls=0;Cyc_Toc_tuple_type_counter=
0;Cyc_Toc_temp_var_counter=0;Cyc_Toc_fresh_label_counter=0;Cyc_Toc_total_bounds_checks=
0;Cyc_Toc_bounds_checks_eliminated=0;Cyc_Toc_globals=_tag_dyneither(({struct
_dyneither_ptr**_tmp821=_cycalloc(sizeof(struct _dyneither_ptr*)* 38);_tmp821[0]=&
Cyc_Toc__throw_str;_tmp821[1]=& Cyc_Toc_setjmp_str;_tmp821[2]=& Cyc_Toc__push_handler_str;
_tmp821[3]=& Cyc_Toc__pop_handler_str;_tmp821[4]=& Cyc_Toc__exn_thrown_str;_tmp821[
5]=& Cyc_Toc__npop_handler_str;_tmp821[6]=& Cyc_Toc__check_null_str;_tmp821[7]=&
Cyc_Toc__check_known_subscript_null_str;_tmp821[8]=& Cyc_Toc__check_known_subscript_notnull_str;
_tmp821[9]=& Cyc_Toc__check_dyneither_subscript_str;_tmp821[10]=& Cyc_Toc__dyneither_ptr_str;
_tmp821[11]=& Cyc_Toc__tag_dyneither_str;_tmp821[12]=& Cyc_Toc__init_dyneither_ptr_str;
_tmp821[13]=& Cyc_Toc__untag_dyneither_ptr_str;_tmp821[14]=& Cyc_Toc__get_dyneither_size_str;
_tmp821[15]=& Cyc_Toc__get_zero_arr_size_str;_tmp821[16]=& Cyc_Toc__dyneither_ptr_plus_str;
_tmp821[17]=& Cyc_Toc__zero_arr_plus_str;_tmp821[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str;
_tmp821[19]=& Cyc_Toc__zero_arr_inplace_plus_str;_tmp821[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str;
_tmp821[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str;_tmp821[22]=& Cyc_Toc__cycalloc_str;
_tmp821[23]=& Cyc_Toc__cyccalloc_str;_tmp821[24]=& Cyc_Toc__cycalloc_atomic_str;
_tmp821[25]=& Cyc_Toc__cyccalloc_atomic_str;_tmp821[26]=& Cyc_Toc__region_malloc_str;
_tmp821[27]=& Cyc_Toc__region_calloc_str;_tmp821[28]=& Cyc_Toc__check_times_str;
_tmp821[29]=& Cyc_Toc__new_region_str;_tmp821[30]=& Cyc_Toc__push_region_str;
_tmp821[31]=& Cyc_Toc__pop_region_str;_tmp821[32]=& Cyc_Toc__open_dynregion_str;
_tmp821[33]=& Cyc_Toc__push_dynregion_str;_tmp821[34]=& Cyc_Toc__pop_dynregion_str;
_tmp821[35]=& Cyc_Toc__reset_region_str;_tmp821[36]=& Cyc_Toc__throw_arraybounds_str;
_tmp821[37]=& Cyc_Toc__dyneither_ptr_decrease_size_str;_tmp821;}),sizeof(struct
_dyneither_ptr*),38);}struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
Cyc_Toc_init();{struct _RegionHandle _tmp822=_new_region("start");struct
_RegionHandle*start=& _tmp822;_push_region(start);Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(
start),ds,1,0);{struct _DynRegionHandle*_tmp824;struct Cyc_Toc_TocState _tmp823=*((
struct Cyc_Toc_TocState*)_check_null(Cyc_Toc_toc_state));_tmp824=_tmp823.dyn;Cyc_Core_free_dynregion(
_tmp824);};_pop_region(start);}return((struct Cyc_List_List*(*)(struct Cyc_List_List*
x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);}
